"""The Chao world's two fixed archives keep their original pointers and storage."""

import hashlib
import os
from pathlib import Path
import struct
import subprocess
import sys
import tempfile
import unittest
from unittest import mock

from chao_banks import MODELS, MOTIONS, Bank, Object, declarations, objects, sections
from check_chao_banks import DISC, models, motions, read_disc, offset


ROOT = Path(__file__).resolve().parents[1]
SH_ELF = Path(os.environ.get('SH_ELF', ROOT / 'tools/sh-elf')).resolve()
# the offsets of the NJS_CNK_OBJECT fields the walk follows, model, child and sibling, and of the NJS_CNK_MODEL
# fields it checks, vlist and plist
MODEL, CHILD, SIBLING, VLIST, PLIST = 4, 44, 48, 0, 4


def archive(bank, words):
    """bank's decoded archive, zeros but for words, {address: (format, values...)}."""
    data = bytearray(bank.size)
    for address, (form, *values) in words.items():
        struct.pack_into(form, data, address - bank.base, *values)
    return data


class Walk(unittest.TestCase):
    """models() and motions() over archives that hold only what each walk reads."""

    # the models: TRUNK's child is BRANCH, BRANCH's sibling is LEAF, and LEAF's child is TRUNK again. TRUNK and
    # BRANCH share MESH, whose polygon list lies on 2. TABLE, the costume table, holds COSTUME, a null and TRUNK,
    # and COSTUME's model, BARE, has no lists
    TRUNK, BRANCH, LEAF, COSTUME, MESH, BARE, TABLE = (MODELS.base + at
                                                       for at in (0, 0x40, 0x80, 0xc0, 0x100, 0x118, 0x140))
    MODEL_ENTRIES = [Object(TRUNK, 'chao_model_trunk', 'NJS_CNK_OBJECT', 0, 52),
                     Object(TABLE, 'chao_costume_models', 'NJS_CNK_OBJECT *', 29, 116)]
    MODEL_WORDS = {TRUNK + MODEL: ('<I', MESH), TRUNK + CHILD: ('<I', BRANCH), BRANCH + MODEL: ('<I', MESH),
                   BRANCH + SIBLING: ('<I', LEAF), LEAF + CHILD: ('<I', TRUNK), COSTUME + MODEL: ('<I', BARE),
                   MESH + VLIST: ('<I', MODELS.base + 0x200), MESH + PLIST: ('<I', MODELS.base + 0x206),
                   TABLE: ('<3I', COSTUME, 0, TRUNK)}
    # the motions: STAND and WALK share CHANNELS, a table of 40. The first channel holds 2 position keys and 3
    # rotation keys, and the last 1 rotation key. WALK's interpolation sets a bit above its low four, which hold 2
    STAND, WALK, CHANNELS = MOTIONS.base, MOTIONS.base + 12, MOTIONS.base + 0x100
    MOTION_ENTRIES = [Object(STAND, 'chao_clip_stand', 'NJS_MOTION', 0, 12),
                      Object(WALK, 'chao_clip_walk', 'NJS_MOTION', 0, 12)]
    MOTION_WORDS = {STAND: ('<IIHH', CHANNELS, 30, 3, 2), WALK: ('<IIHH', CHANNELS, 60, 3, 0x12),
                    CHANNELS: ('<4I', MOTIONS.base + 0x400, MOTIONS.base + 0x420, 2, 3),
                    CHANNELS + 39 * 16: ('<4I', 0, MOTIONS.base + 0x450, 0, 1)}

    def test_counts_each_node_and_mesh_of_the_models_once(self):
        self.assertEqual(models(archive(MODELS, self.MODEL_WORDS), self.MODEL_ENTRIES), (4, 2))

    def test_refuses_a_model_pointer_outside_the_archive_or_off_its_alignment(self):
        # a node of 52 bytes and a model of 24, each starting 4 bytes short of fitting before the end
        end = MODELS.base + MODELS.size
        for case, (address, pointer, problem) in {
                'a node past the end': (self.BRANCH + SIBLING, end - 48, 'outside the archive'),
                'a node before the start': (self.TRUNK + CHILD, MODELS.base - 52, 'outside the archive'),
                'a node off 4': (self.BRANCH + SIBLING, self.LEAF + 2, 'off a 4-byte boundary'),
                'a model past the end': (self.TRUNK + MODEL, end - 20, 'outside the archive'),
                'vertices off 4': (self.MESH + VLIST, MODELS.base + 0x202, 'off a 4-byte boundary'),
                'polygons off 2': (self.MESH + PLIST, MODELS.base + 0x207, 'off a 2-byte boundary')}.items():
            with self.subTest(case), self.assertRaisesRegex(ValueError, f'^models: .* {problem} at {pointer:#010x}$'):
                models(archive(MODELS, {**self.MODEL_WORDS, address: ('<I', pointer)}), self.MODEL_ENTRIES)

    def test_refuses_a_bank_with_nothing_to_follow(self):
        # with no named object, a walk would check no address of its archive and still report a count
        with self.assertRaisesRegex(ValueError, '^models: no named root to follow$'):
            models(archive(MODELS, self.MODEL_WORDS), [])
        with self.assertRaisesRegex(ValueError, '^motions: no named header to follow$'):
            motions(archive(MOTIONS, self.MOTION_WORDS), [])

    def test_counts_each_key_of_each_motion(self):
        self.assertEqual(motions(archive(MOTIONS, self.MOTION_WORDS), self.MOTION_ENTRIES), 12)

    def test_refuses_a_motion_of_another_kind_or_a_pointer_outside_the_archive(self):
        # WALK with a scale factor too, with no frames, and with an interpolation of 3; then WALK's table of 40
        # channels of 16 bytes, and the last channel's 2 rotation keys of 16, each starting 4 bytes short of fitting
        # before the end
        end = MOTIONS.base + MOTIONS.size
        channels, keys = end - (40 * 16 - 4), end - (2 * 16 - 4)
        kind = '^chao_clip_walk: expected a position/rotation Ninja motion$'
        def outside(pointer):
            return f'^motions: .* outside the archive at {pointer:#010x}$'
        for case, (address, word, problem) in {
                'scaled': (self.WALK, ('<IIHH', self.CHANNELS, 60, 7, 2), kind),
                'no frames': (self.WALK, ('<IIHH', self.CHANNELS, 0, 3, 2), kind),
                'interpolation of 3': (self.WALK, ('<IIHH', self.CHANNELS, 60, 3, 3), kind),
                'channels': (self.WALK, ('<IIHH', channels, 60, 3, 2), outside(channels)),
                'keys': (self.CHANNELS + 39 * 16, ('<4I', 0, keys, 0, 2), outside(keys))}.items():
            with self.subTest(case), self.assertRaisesRegex(ValueError, problem):
                motions(archive(MOTIONS, {**self.MOTION_WORDS, address: word}), self.MOTION_ENTRIES)


class ChaoBanksTest(unittest.TestCase):
    def test_native_objects_and_bounds(self):
        banks = objects(ROOT / 'config')
        self.assertEqual((len(banks[MODELS]), len(banks[MOTIONS])), (123, 241))
        first = banks[MODELS][0]
        self.assertEqual((first.name, first.address - MODELS.base, first.width),
                         ('chao_model_neutral_child_base', 0x4b84, 52))
        last = banks[MODELS][-1]
        self.assertEqual((last.name, last.count, last.address + last.width),
                         ('chao_costume_models', 29, MODELS.base + MODELS.size))
        self.assertLess(MODELS.base + MODELS.size, MOTIONS.base)
        self.assertLess(MOTIONS.base + MOTIONS.size, 0x0c9c0000)
        self.assertTrue(all(obj.width == 12 for obj in banks[MOTIONS]))

    def test_reject_other_editions_and_outside_pointers(self):
        with tempfile.TemporaryDirectory() as directory:
            work = Path(directory)
            source = work / 'other.prs'
            source.write_bytes(b'other edition')
            with self.assertRaisesRegex(ValueError, 'Japanese disc'):
                read_disc(source, MODELS, work / 'unused-decoder', work)
        with self.assertRaisesRegex(ValueError, 'outside'):
            offset(MODELS, MODELS.base + MODELS.size - 4, 52)
        # an address off its alignment inside the archive names the alignment, not the bounds
        with self.assertRaisesRegex(ValueError, 'off a 4-byte boundary'):
            offset(MOTIONS, MOTIONS.base + 1, 12)
        self.assertEqual(offset(MOTIONS, MOTIONS.base, 12), 0)

    def test_rejects_a_root_its_symbols_place_outside_its_archive(self):
        # a name gives an object its archive, so a model root placed past the model archive's end fails, where the
        # address alone would leave it out of the walk
        with tempfile.TemporaryDirectory() as directory:
            config = Path(directory)
            (config / 'chao').mkdir()
            (config / 'chao/symbols.txt').write_text(f'{MODELS.base + MODELS.size:#010x} chao_model_planted\n')
            with self.assertRaisesRegex(ValueError, 'chao_model_planted: .* not wholly in models'):
                objects(config)

    def test_names_both_models_of_an_overlap(self):
        # two aligned models of 52 bytes, the second starting 4 bytes before the first ends
        with tempfile.TemporaryDirectory() as directory:
            config = Path(directory)
            (config / 'chao').mkdir()
            (config / 'chao/symbols.txt').write_text(f'{MODELS.base:#010x} chao_model_first\n'
                                                     f'{MODELS.base + 48:#010x} chao_model_second\n')
            with self.assertRaisesRegex(ValueError, f'^chao_model_second: starts at {MODELS.base + 48:#010x}, '
                                                    f'before chao_model_first ends at {MODELS.base + 52:#010x}$'):
                objects(config)

    def test_rejects_a_model_or_a_motion_whose_pointers_leave_its_archive(self):
        root = Object(MODELS.base + 0x100, 'chao_model_root', 'NJS_CNK_OBJECT', 0, 52)
        archive = bytearray(MODELS.size)
        self.assertEqual(models(archive, [root]), (1, 0))
        # the root's child lies before the archive
        struct.pack_into('<I', archive, 0x100 + 44, MODELS.base - 52)
        with self.assertRaisesRegex(ValueError, 'outside the archive'):
            models(archive, [root])
        header = Object(MOTIONS.base, 'chao_clip_planted', 'NJS_MOTION', 0, 12)
        archive = bytearray(MOTIONS.size)
        # a position and rotation motion of one frame, whose channels follow it and whose first node holds one key
        struct.pack_into('<IIHH', archive, 0, MOTIONS.base + 16, 1, 3, 2)
        struct.pack_into('<4I', archive, 16, MOTIONS.base + 0x400, 0, 1, 0)
        self.assertEqual(motions(archive, [header]), 1)
        struct.pack_into('<I', archive, 16, MOTIONS.base + MOTIONS.size - 8)
        with self.assertRaisesRegex(ValueError, 'outside the archive'):
            motions(archive, [header])

    def test_prints_its_prerequisite_in_help_with_or_without_docstrings(self):
        for flags in ([], ['-OO']):
            with self.subTest(flags):
                result = subprocess.run([sys.executable, *flags, ROOT / 'tools/check_chao_banks.py', '--help'],
                                        cwd=ROOT, env=dict(os.environ, PYTHONPATH=str(ROOT / 'tools'), COLUMNS='80'),
                                        capture_output=True, text=True, check=True)
                self.assertEqual('make build/prskit' in result.stdout, not flags)

    def test_rejects_an_archive_whose_decoded_bytes_differ(self):
        # the packed bytes are the disc's, and the decoder writes the archive's length of other bytes
        with tempfile.TemporaryDirectory() as directory:
            work = Path(directory)
            archive = work / 'motion.prs'
            archive.write_bytes(b'packed fixture')
            decoder = work / 'prskit'
            decoder.write_text(f'#!/bin/sh\nhead -c {MOTIONS.size} /dev/zero > "$3"\n')
            decoder.chmod(0o755)
            digest = hashlib.sha256(archive.read_bytes()).hexdigest()
            with mock.patch.dict(DISC, {MOTIONS: (archive.stat().st_size, digest, DISC[MOTIONS][2])}):
                with self.assertRaisesRegex(ValueError, 'decoded archive does not match'):
                    read_disc(archive, MOTIONS, decoder, work)

    @mock.patch.dict(os.environ, {'HUNG': '0.3s'})
    def test_reports_a_stalled_archive_decoder(self):
        with tempfile.TemporaryDirectory() as directory:
            work = Path(directory)
            archive = work / 'model.prs'
            archive.write_bytes(b'compressed fixture')
            decoder = work / 'prskit'
            decoder.write_text('#!/bin/sh\nexec sleep 5\n')
            decoder.chmod(0o755)
            digest = hashlib.sha256(archive.read_bytes()).hexdigest()
            with mock.patch.dict(DISC, {MODELS: (archive.stat().st_size, digest, '')}):
                with self.assertRaisesRegex(SystemExit, r'prskit decompress .*timed out'):
                    read_disc(archive, MODELS, decoder, work)

    def test_gnu_storage_and_c_types(self):
        gcc, ld, nm, readelf = (SH_ELF / 'bin' / ('sh-elf-' + tool)
                                for tool in ('gcc', 'ld', 'nm', 'readelf'))
        if not gcc.is_file():
            self.skipTest('SH-4 GCC is unavailable')
        cc = [gcc, '-ml', '-m4-single-only', '-std=c89', '-Isdk/include',
              '-ffreestanding', '-fno-common', '-ffat-lto-objects', '-flto']
        banks = objects(ROOT / 'config')
        with tempfile.TemporaryDirectory() as directory:
            work = Path(directory)
            def run(args, check=True):
                return subprocess.run(args, cwd=ROOT, stdout=subprocess.PIPE, stderr=subprocess.STDOUT,
                                      text=True, check=check)
            source = work / 'roots.c'
            provider = work / 'roots.o'
            source.write_text('\n'.join(declarations(banks)) + '\n')
            run([*cc, '-c', source, '-o', provider])
            script = work / 'banks.ld'
            script.write_text('SECTIONS {\n' + '\n'.join(sections(banks)) + '\n}\n')
            # An unrelated, genuinely missing object stays undefined.
            source.write_text('extern int missing_model;\nint *reference = &missing_model;\n')
            run([*cc, '-c', source, '-o', work / 'consumer.o'])
            output = work / 'banks.o'
            run([ld, '-EL', '-r', '-T', script, provider, work / 'consumer.o', '-o', output])
            table = {row[0]: row[1:] for s in run([nm, '-P', output]).stdout.splitlines() if (row := s.split())}
            self.assertEqual(table['_missing_model'][0], 'U')
            summary = run([readelf, '-SW', output]).stdout.splitlines()
            for bank, entries in banks.items():
                self.assertEqual(int(table['_chao_' + bank.name + '_upload'][1], 16), bank.base | 0x80000000)
                line = next(row.split() for row in summary if ('.chao_' + bank.name) in row.split())
                index = line.index('.chao_' + bank.name)
                self.assertEqual((line[index + 1], int(line[index + 2], 16), int(line[index + 4], 16)),
                                 ('NOBITS', bank.base, bank.size))
                for obj in entries:
                    kind, address, size = table['_' + obj.name][:3]
                    self.assertEqual((kind, int(address, 16), int(size, 16)), ('B', obj.address, obj.width))
            source.write_text('extern double chao_clip_stand;\n'
                              'double sample(void) { return chao_clip_stand; }\n')
            run([*cc, '-c', source, '-o', work / 'wrong.o'])
            wrong = run([*cc, '-Werror=lto-type-mismatch', '-r', '-flinker-output=nolto-rel', '-nostdlib',
                         work / 'wrong.o', provider, '-o', work / 'wrong-link.o'], check=False)
            self.assertNotEqual(wrong.returncode, 0)
            self.assertIn('lto-type-mismatch', wrong.stdout)
            # a provider of 48 bytes aligned to 8: padding moves it from offset 4 to 8, and it ends at 56,
            # where the declared 52 bytes from 4 would
            bank = Bank('models', MODELS.base, 0x100)
            script.write_text('SECTIONS {\n' + '\n'.join(sections(
                {bank: [Object(bank.base + 4, 'chao_model_padded', 'NJS_CNK_OBJECT', 0, 52)]})) + '\n}\n')
            source.write_text('char chao_model_padded[48] '
                              '__attribute__((aligned(8), section(".bss.chao_bank.chao_model_padded")));\n')
            run([*cc, '-c', source, '-o', work / 'padded.o'])
            padded = run([ld, '-EL', '-r', '-T', script, work / 'padded.o', '-o', output], check=False)
            self.assertNotEqual(padded.returncode, 0)
            self.assertIn('chao_model_padded address', padded.stdout)
