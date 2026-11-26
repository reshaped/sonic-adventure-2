"""The optical pack uses the loaded address span and binds the results to their ELF."""

from hashlib import sha256
from pathlib import Path
import shutil
import subprocess
from types import SimpleNamespace
import unittest
from unittest import mock

from dc_manifest import collect, replacements
from dc_overlay import Elf, ET_EXEC, Load
import dc_publish
from dc_publish import payload
from sh_elf import Section
from support import ROOT, SH_ELF, TreeTest
from test_dc_disc import compile_prskit, compress_prs


class Manifest(TreeTest):
    def setUp(self):
        super().setUp()
        compile_prskit()
        self.image('stage', 'file STAGE.PRS\nP 0x8c500000 0x8c500010\n')
        self.manifest = Path('release/manifest.txt')
        self.write('release/images/stage.elf', b'linked')
        self.packed = compress_prs(b'decoded')
        self.write('release/STAGE.PRS', self.packed)
        digests = [sha256(data).hexdigest() for data in (b'linked', b'decoded', self.packed)]
        self.record = '  '.join(['stage', *digests, 'STAGE.PRS']) + '\n'
        self.write('release/receipts/stage.txt', self.record)

    def test_published_inventory_identifies_every_output(self):
        collect(self.manifest)
        found = replacements(self.manifest)
        self.assertEqual(found, {'STAGE.PRS': (Path('release/STAGE.PRS'), sha256(self.packed).hexdigest())})
        self.write(self.manifest, self.record + self.record)
        with self.assertRaisesRegex(ValueError, 'duplicate optical file'):
            replacements(self.manifest)
        self.write(self.manifest, '')
        with self.assertRaisesRegex(ValueError, 'missing optical programs'):
            replacements(self.manifest)
        self.write(self.manifest, self.record.replace(sha256(b'decoded').hexdigest(), '0' * 64))
        with self.assertRaisesRegex(ValueError, 'does not decode'):
            replacements(self.manifest)

    def test_manifest_cannot_overwrite_an_optical_program(self):
        with self.assertRaisesRegex(ValueError, 'is an optical program'):
            collect(Path('release/STAGE.PRS'))
        self.assertEqual(Path('release/STAGE.PRS').read_bytes(), self.packed)

    def test_changed_elf_or_disc_file_is_rejected(self):
        collect(self.manifest)
        for path in ('release/images/stage.elf', 'release/STAGE.PRS'):
            with self.subTest(path=path):
                original = Path(path).read_bytes()
                self.write(path, b'other')
                with self.assertRaisesRegex(ValueError, 'differs from its recorded SHA-256'):
                    replacements(self.manifest)
                self.write(path, original)


class Receipt(TreeTest):
    def test_failed_receipt_write_leaves_no_receipt_beside_the_new_program(self):
        root = Path('release')
        output = root / 'STAGE.PRS'
        receipt = root / 'receipts/stage.txt'
        self.image('stage', 'file STAGE.PRS\nP 0x8c500000 0x8c500004\n')
        self.write(output, b'previous program')
        self.write(receipt, b'previous receipt')
        with mock.patch.object(dc_publish, 'Elf', return_value=mock.Mock(data=b'linked')), \
                mock.patch.object(dc_publish, 'payload', return_value=b'new'), \
                mock.patch.object(dc_publish, 'run', side_effect=lambda command, **_: shutil.copy(*command[2:])), \
                mock.patch.object(dc_publish, 'save', side_effect=OSError('disk full')):
            with self.assertRaises(OSError):
                dc_publish.publish('stage', root, Path('build/prskit'))
        self.assertEqual(output.read_bytes(), b'new')
        self.assertFalse(receipt.exists())


class Sections(unittest.TestCase):
    def payload(self, *sections):
        """Pack an overlay of eight bytes at 0x8c500000, whose ELF holds them at offset 0."""
        data = b'TITLE \x01\x02'
        elf = SimpleNamespace(
            path='title.elf', entry=0x8c500000, data=data, defined={},
            segments=[Load(0x8c500000, 0x8c500000, 0, len(data), len(data), 5)],
            sections=sections, named={section.name: section for section in sections},
            slice=lambda offset, size: data[offset:offset + size],
            contents=lambda section: data[section.offset:section.offset + section.size])
        with mock.patch.object(dc_publish, 'verify_header'):
            return payload('title', elf)

    def section(self, name, address, offset, size, kind=1):
        return Section(name, kind, 2, address, offset, size, 0, 0, 0)

    def test_section_below_the_entry_is_outside_the_image(self):
        # Its bytes match the image's first four, where a slice from -8 would look.
        below = self.section('.data', 0x8c500000 - 8, 0, 4)
        with self.assertRaisesRegex(ValueError, r'\.data lies outside the image'):
            self.payload(self.section('.head', 0x8c500000, 0, 8), below)

    def test_file_backed_bss_must_hold_zeros(self):
        bss = self.section('.bss', 0x8c500006, 6, 2)
        with self.assertRaisesRegex(ValueError, 'invalid BSS at 0x8c500006'):
            self.payload(self.section('.head', 0x8c500000, 0, 6), bss)


@unittest.skipUnless((SH_ELF / 'bin/sh-elf-as').is_file(), 'requires SH-4 binutils')
class OpticalSpan(TreeTest):
    def test_child_cannot_mix_cached_and_uncached_load_addresses(self):
        self.write('area.s', '.section .head,"aw"\n.ascii "AREA    "\n.long _step\n'
                             '.text\n.global _step\n.type _step,@function\n_step: rts\nnop\n.size _step,.-_step\n'
                             '.data\n.long 123\n')
        self.write('area.ld', 'PHDRS { head PT_LOAD FLAGS(6); code PT_LOAD FLAGS(5); data PT_LOAD FLAGS(6); }\n'
                              'SECTIONS { .head 0x0c9c0000 : { *(.head) } :head '
                              '.text 0x0c9c1000 : { *(.text) } :code .data 0x8c9c2000 : { *(.data) } :data }\n')
        subprocess.run([SH_ELF / 'bin/sh-elf-as', '--little', '-o', 'area.o', 'area.s'], check=True)
        subprocess.run([SH_ELF / 'bin/sh-elf-ld', '-EL', '-z', 'max-page-size=4096', '-T', 'area.ld',
                        '-e', '0x0c9c0000', '-o', 'area.elf', 'area.o'], check=True)
        with self.assertRaisesRegex(ValueError, "load at 0x8c9c2000 doesn't lie at or past the entry, 0x0c9c0000, "
                                                "in the entry's view"):
            payload('chaostglobby', Elf(Path('area.elf'), ET_EXEC))

    def test_overlay_zeroes_the_gaps_and_bss_at_their_loaded_addresses(self):
        self.write('stage.s', '.section .head,"aw"\n.ascii "TITLE   "\n.long 1, _step, 0, 0\n'
                              '.text\n.global _step\n.type _step,@function\n_step: rts\nnop\n.size _step,.-_step\n'
                              '.data\n.byte 17,18,19,20\n.bss\n.balign 4\n.space 96\n')
        subprocess.run([SH_ELF / 'bin/sh-elf-as', '--little', '-o', 'stage.o', 'stage.s'], check=True)
        subprocess.run([SH_ELF / 'bin/sh-elf-ld', '-EL', '-z', 'max-page-size=4096', '--defsym',
                        'overlay_base=0x8c500000', '-T', ROOT / 'config/overlay.ld', '-e', '0x8c500000',
                        '-o', 'stage.elf', 'stage.o'], check=True)
        elf = Elf(Path('stage.elf'), ET_EXEC)
        raw = payload('title', elf)
        bss = elf.section('.bss')
        self.assertEqual(len(raw), bss.address + 96 - elf.entry)
        self.assertEqual(raw[:8], b'TITLE   ')
        self.assertEqual(raw[24:0x1000], bytes(0x1000 - 24))
        self.assertEqual(raw[-100:], bytes([17, 18, 19, 20]) + bytes(96))
        # The same ELF cannot be packaged as a terrain stage without the
        # allocator boundary that keeps model construction past the overlay.
        with self.assertRaisesRegex(ValueError, 'terrain allocation'):
            payload('stg01', elf)
