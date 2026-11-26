"""Sector placement and readback of a rebuilt disc, using a small ISO9660 fixture."""

import hashlib
import json
from pathlib import Path
import struct
import subprocess
import tempfile
from unittest import mock

import disc
from support import CC, ROOT, TreeTest


def compile_disc_tool(name):
    Path('build').mkdir(exist_ok=True)
    source = ROOT / 'tools' / name
    subprocess.run([*CC, '-std=c11', source / 'main.c', source / (name + '.c'), '-o', Path('build') / name],
                   check=True)


def compile_gdi():
    compile_disc_tool('gdi')


def compile_prskit():
    compile_disc_tool('prskit')


def compress_prs(data):
    with tempfile.TemporaryDirectory() as directory:
        raw, packed = Path(directory) / 'file.bin', Path(directory) / 'file.prs'
        raw.write_bytes(data)
        subprocess.run(['build/prskit', 'compress', raw, packed], check=True)
        return packed.read_bytes()


def fixture(directory):
    """One Katana data track with a boot program, an overlay and an adjoining sound file."""
    directory.mkdir()
    base, block = 45000, disc.SECTOR
    track = bytearray(32 * block)

    def put(sector, offset, data):
        start = sector * block + offset
        track[start:start + len(data)] = data

    def both(code, value):
        return struct.pack('<' + code, value) + struct.pack('>' + code, value)

    def entry(name, sector, size, flags=0):
        length = 33 + len(name) + (len(name) % 2 == 0)
        record = bytearray(length)
        record[0] = length
        record[2:10] = both('I', base + sector)
        record[10:18] = both('I', size)
        record[25] = flags
        record[28:32] = both('H', 1)
        record[32] = len(name)
        record[33:33 + len(name)] = name
        return record

    put(0, 0, b'SEGA SEGAKATANA ')
    put(0, 0x38, b'0000000 ')
    put(0, 0x60, b'1ST_READ.BIN    ')
    put(16, 0, b'\x01CD001\x01')
    put(17, 0, b'\xffCD001\x01')
    put(16, 80, both('I', 32))
    put(16, 128, both('H', block))
    put(16, 132, both('I', 10))
    for sector, order, at in ((18, '<', 140), (19, '>', 148)):
        put(16, at, struct.pack(order + 'I', base + sector))
        put(sector, 0, struct.pack(order + 'BBIHBB', 1, 0, base + 20, 1, 0, 0))
    root = entry(b'\0', 20, block, 2)
    put(16, 156, root)
    records = root + entry(b'\1', 20, block, 2)
    payloads = {'STAGE.PRS': compress_prs(b'stage'), 'MUSIC.ADX': b'keep', '1ST_READ.BIN': b'boot'}
    for (name, data), sector in zip(payloads.items(), (22, 23, 28)):
        records += entry(name.encode() + b';1', sector, len(data))
        put(sector, 0, data)
    put(20, 0, records)
    (directory / 'track1.bin').write_bytes(bytes(block))
    (directory / 'audio.raw').write_bytes(b'x' * 2352)
    (directory / 'track 3.bin').write_bytes(track)
    image = directory / 'test.gdi'
    image.write_text('3\n1 0 4 2048 track1.bin 0\n2 1 0 2352 audio.raw 0\n3 45000 4 2048 "track 3.bin" 0\n')
    return image, payloads


class DiscTests(TreeTest):
    def test_boot_keeps_its_end_and_cannot_take_an_occupied_sector(self):
        files = {'1ST_READ.BIN': disc.Extent(500000, 2, 3000, True)}
        self.assertEqual(disc.placements(files, [], {'1ST_READ.BIN': 1}), {'1ST_READ.BIN': 500001})
        self.assertEqual(disc.placements(files, [(499998, 500000)], {'1ST_READ.BIN': 4100}), {'1ST_READ.BIN': 499999})
        with self.assertRaisesRegex(ValueError, 'fixed end'):
            disc.placements(files, [], {'1ST_READ.BIN': 4100})

    def test_growing_files_try_their_own_starts_before_any_moves(self):
        files = {'1ST_READ.BIN': disc.Extent(100, 1, 1, True), 'A.PRS': disc.Extent(10, 1, 1, False),
                 'B.PRS': disc.Extent(20, 1, 1, False)}
        sizes = {'1ST_READ.BIN': 1, 'A.PRS': 2 * disc.SECTOR, 'B.PRS': 3 * disc.SECTOR}
        self.assertEqual(disc.placements(files, [(21, 23), (30, 32)], sizes), {'A.PRS': 30, 'B.PRS': 20})

    def test_an_output_cannot_take_a_name_another_output_reserves(self):
        for name in ('alpha.previous', '.alpha.work'):
            with self.assertRaisesRegex(ValueError, 'reserves this name'):
                with disc.staged_disc(Path('release') / name):
                    pass
        self.assertFalse(Path('release').exists())

    def test_the_original_cannot_lie_in_a_directory_a_build_deletes(self):
        out = Path('release/alpha')
        for directory in (out, disc.previous(out)):
            self.write(directory / 'track.bin', directory.name)
            self.write(directory / disc.RECEIPT, json.dumps({'format': disc.FORMAT, 'files': ['track.bin']}))
        self.write(disc.work(out) / 'game.gdi', 'original')
        for directory in (out, disc.previous(out), disc.work(out)):
            with self.assertRaisesRegex(ValueError, 'contains the original disc'):
                disc.build(Path('manifest.txt'), directory / 'game.gdi', out)
        self.assertEqual(set(out.parent.iterdir()), {out, disc.previous(out), disc.work(out)})

    def test_a_build_reclaims_the_work_a_killed_build_left(self):
        out = Path('release/alpha')
        self.write('release/.alpha.work/disc/track.bin', b'abandoned')
        self.write('release/.alpha-unrelated/track.bin', b'kept')
        with disc.staged_disc(out) as staged:
            self.write(staged / 'track.bin', b'new')
        self.assertEqual(set(out.parent.iterdir()),
                         {out, out.parent / '.alpha.lock', out.parent / '.alpha-unrelated'})
        self.assertEqual((out / 'track.bin').read_bytes(), b'new')

    def test_rebuild_with_real_gdi_preserves_media_and_checks_programs(self):
        compile_gdi()
        compile_prskit()
        image, originals = fixture(Path('original disc'))
        source_track = disc.sha256(image.parent / 'track 3.bin')
        programs = {'1ST_READ.BIN': b'new boot', 'STAGE.PRS': hashlib.shake_256(b'points').digest(2200)}
        self.write('SHA256SUMS', ''.join(f'{hashlib.sha256(originals[name]).hexdigest()}  orig/{name}\n'
                                        for name in programs))
        manifest = Path('build/dc/manifest.txt')
        records = []
        for name, data in programs.items():
            ident = Path(name).stem.lower()
            self.image(ident, f'file {name}\nP 0x8c500000 0x8c500100\n')
            optical = compress_prs(data) if name.endswith('.PRS') else data
            self.write(manifest.parent / name, optical)
            elf = manifest.parent / 'images' / (ident + '.elf')
            self.write(elf, b'linked test program')
            digest = hashlib.sha256(data).hexdigest()
            records.append(f'{ident}  {disc.sha256(elf)}  {digest}  {hashlib.sha256(optical).hexdigest()}  {name}\n')
        self.write(manifest, ''.join(records))
        out = manifest.parent / 'disc'
        disc.build(manifest, image.parent, out)
        record = json.loads((out / disc.RECEIPT).read_text())
        self.assertEqual(record['manifest_sha256'], disc.sha256(manifest))
        self.assertEqual(record['source_identity']['files_sha256'],
                         {path.name: disc.sha256(path) for path in image.parent.iterdir()})
        self.assertEqual(disc.sector_count(record['programs']['STAGE.PRS']['bytes']), 2)
        self.assertEqual(record['programs']['STAGE.PRS']['lba'], 45021)
        self.assertEqual(record['programs']['1ST_READ.BIN']['lba'], 45028)
        self.assertEqual((out / 'audio.raw').read_bytes(), (image.parent / 'audio.raw').read_bytes())
        disc.check_payloads(out / image.name, {'MUSIC.ADX': hashlib.sha256(b'keep').hexdigest()}, out.parent)
        disc.build(manifest, image, out)
        track = disc.sha256(out / 'track 3.bin')
        run_gdi = disc.gdi

        def corrupt_readback(operation, source, *args):
            result = run_gdi(operation, source, *args)
            if operation == 'extract' and source != image:
                (Path(args[0]) / 'STAGE.PRS').write_bytes(b'corrupt')
            return result

        with mock.patch.object(disc, 'gdi', side_effect=corrupt_readback):
            with self.assertRaisesRegex(ValueError, 'STAGE.PRS SHA-256'):
                disc.build(manifest, image, out)
        self.assertEqual(disc.sha256(out / 'track 3.bin'), track)
        sums = Path('SHA256SUMS').read_text().replace(hashlib.sha256(originals['STAGE.PRS']).hexdigest(), '0' * 64)
        self.write('SHA256SUMS', sums)
        with self.assertRaisesRegex(ValueError, 'STAGE.PRS SHA-256'):
            disc.build(manifest, image, out)
        self.assertEqual(disc.sha256(image.parent / 'track 3.bin'), source_track)
