"""The extraction route authenticates and reads back media without needing a raw disc."""

import hashlib
import json
from pathlib import Path
import tracemalloc
from unittest import mock

import dc_extracted
import dc_iso
from disc import disc_map, sha256
from support import ROOT, TreeTest
from test_dc_disc import compile_gdi, compile_prskit, compress_prs


def digest(data):
    return hashlib.sha256(data).hexdigest()


class ExtractedDiscTests(TreeTest):
    def test_catalog_covers_the_japanese_extraction(self):
        path = ROOT / 'config/jp-disc.sha256'
        self.assertEqual(len(dc_extracted.inventory(path)), 2568)
        self.assertEqual(sha256(path), '6aa182e69856ba8cfbfb017fe7b7c7b6c879c02693faf2dcef106b38e008f9ef')

    def test_complete_directory_bootstrap_and_payload_readback(self):
        compile_gdi()
        compile_prskit()
        # The commercial leadout would leave a sparse gigabyte-sized file.
        # The small volume exercises the same directory layout and writer.
        self.enter_small_volume()
        base = Path('retail')
        root = Path('build/dc')
        output = root / 'disc'
        manifest = root / 'manifest.txt'
        bootstrap = bytearray(16 * dc_iso.SECTOR)
        bootstrap[:16] = b'SEGA SEGAKATANA '
        bootstrap[0x38:0x40] = b'0000000 '
        bootstrap[0x60:0x70] = b'1ST_READ.BIN    '
        originals = {'1ST_READ.BIN': b'boot', 'IP.BIN': bytes(bootstrap),
                     'SONIC2/STAGE.PRS': compress_prs(b'stage'), 'SONIC2/TEXT.PRS': compress_prs(b'asset')}
        stage = hashlib.shake_256(b'points').digest(2100)
        programs = {'1st_read': ('1ST_READ.BIN', b'new boot'), 'stage': ('SONIC2/STAGE.PRS', stage)}
        self.write('config/jp-disc.sha256', ''.join(f'{digest(data)}  {name}\n' for name, data in originals.items()))
        self.write('SHA256SUMS', ''.join(f'{digest(originals[name])}  orig/{name}\n' for name, _ in programs.values()))
        for name, data in originals.items():
            self.write(base / name, data)
        records = []
        for image, (name, data) in programs.items():
            self.image(image, f'file {name}\nP 0x8c500000 0x8c500004\n')
            self.write(root / 'images' / (image + '.elf'), b'linked')
            optical = compress_prs(data) if name.endswith('.PRS') else data
            self.write(root / name, optical)
            records.append(f'{image}  {digest(b"linked")}  {digest(data)}  {digest(optical)}  {name}\n')
        self.write(manifest, ''.join(records))
        dc_extracted.build(manifest, base, output)
        record = json.loads((output / 'disc.txt').read_text())
        self.assertEqual(record['source'], 'extracted')
        self.assertEqual(record['source_identity'],
                         {'file_count': len(originals), 'bootstrap_sha256': digest(bootstrap),
                          'catalog_sha256': sha256(dc_extracted.CATALOG)})
        mapped, _ = disc_map(output / 'disc.gdi')
        self.assertEqual(set(mapped), set(originals))
        self.assertEqual(mapped['SONIC2/STAGE.PRS'].sectors, 2)
        self.assertEqual(mapped['1ST_READ.BIN'].start + mapped['1ST_READ.BIN'].sectors, dc_iso.LEADOUT)
        self.assertEqual((output / 'track03.bin').read_bytes()[:len(bootstrap)], bootstrap)
        stock = output / 'track03.bin'
        checked = sha256(stock)
        # A sparse 64 MiB IP.BIN is refused without being read whole.
        with (base / 'IP.BIN').open('r+b') as oversized:
            oversized.truncate(1 << 26)
        tracemalloc.start()
        try:
            with self.assertRaisesRegex(ValueError, 'IP.BIN: bootstrap differs'):
                dc_extracted.build(manifest, base, output)
            _, peak = tracemalloc.get_traced_memory()
        finally:
            tracemalloc.stop()
        self.assertLess(peak, 1 << 24)
        self.write(base / 'IP.BIN', bytes(bootstrap))
        self.write(base / 'SONIC2/TEXT.PRS', b'wrong asset')
        with self.assertRaisesRegex(ValueError, 'TEXT.PRS'):
            dc_extracted.build(manifest, base, output)
        self.assertEqual(sha256(stock), checked)

        # Damage the final optical track after construction. Its catalog
        # still parses and points to the same file, but independent readback fails.
        extent = mapped['SONIC2/TEXT.PRS']
        with stock.open('r+b') as track:
            track.seek((extent.start - dc_iso.HIGH) * dc_iso.SECTOR)
            track.write(b'x')
        extents = {name: (part.start, part.size) for name, part in mapped.items()}
        expected = {name: digest(data) for name, data in originals.items()}
        expected.update({name: sha256(root / name) for name, _ in programs.values()})
        with self.assertRaisesRegex(ValueError, 'TEXT.PRS: high-density track'):
            dc_extracted.verify_track(output, extents, expected, bootstrap)
        # Maps that agree but omit the boot program are a placement error.
        with mock.patch.object(dc_extracted, 'disc_map', return_value=({}, [])):
            with self.assertRaisesRegex(ValueError, 'boot entry differs'):
                dc_extracted.verify_track(output, {}, {}, bootstrap)

    def test_a_source_that_changes_after_layout_names_the_change(self):
        for planned, stored, error in ((b'a', b'ab', 'changed size'), (b'ab', b'a', 'changed size'),
                                       (b'ab', b'ba', 'stored bytes differ')):
            source = Path('source')
            source.write_bytes(planned)
            stat = source.stat()
            source.write_bytes(stored)
            with mock.patch.object(Path, 'stat', return_value=stat):
                with self.assertRaisesRegex(ValueError, f'DATA.BIN: {error}'):
                    dc_iso.write_volume(Path('track.bin'), 0, 100, {Path('DATA.BIN'): source},
                                        {'DATA.BIN': digest(planned)})
            Path('track.bin').unlink()

    def enter_small_volume(self):
        changed = mock.patch.object(dc_iso, 'LEADOUT', dc_iso.HIGH + 96)
        changed.start()
        self.addCleanup(changed.stop)
