"""Disc-owned data keeps its retail representation and acquires ordinary GNU symbols."""

import contextlib
import io
from pathlib import Path
import struct
import subprocess
import sys
import tempfile
import types
import unittest
from unittest import mock

import content
from config import Image, rows
from decls import declarations
from support import ROOT, SH_ELF, TreeTest, compile_host, original_image


SPLITS = """\
file TRACK.BIN
P 0x8c500000 0x8c500004
D 0x8c500004 0x8c500020
disc D 0x8c500004 0x8c500010
disc D 0x8c500014 0x8c500020
"""
SYMBOLS = """\
0x8c500004 route_a
0x8c500014 route_b
"""
TABLE = """\
0x8c500004 route_a points 1
0x8c500014 route_b points 1
"""


class Content(TreeTest):
    def setUp(self):
        super().setUp()
        self.image('track', SPLITS, SYMBOLS)
        self.table = Path('config/track/data.txt')
        self.write(self.table, TABLE)

    def test_fixed_width_texture_filenames_keep_the_exported_array(self):
        self.image('names', """file NAMES.BIN
C 0x8c500000 0x8c500018
D 0x8c500018 0x8c500030
disc C 0x8c500000 0x8c500018
disc D 0x8c500018 0x8c500030
""", "0x8c500000 filenames\n0x8c500018 textures\n")
        self.write('config/names/data.txt',
                   "0x8c500000 filenames texture_filenames 2 12\n0x8c500018 textures texture_names 2\n")
        original = Path('NAMES.BIN')
        slots = b'eye\0'.ljust(12, b'\0') + b'longer_name\0'
        names = struct.pack('<6I', 0x8c500000, 0, 0, 0x8c50000c, 0, 0)
        self.write(original, slots + names)
        entries = content.objects(Image('names'))
        self.assertEqual(entries['filenames'].size, 24)
        assembly = content.assembly(entries, original, 0x8c500000).decode()
        self.assertIn('.size _filenames, . - _filenames', assembly)
        self.assertIn('.long _filenames\n', assembly)
        self.assertIn('.long _filenames+12\n', assembly)
        for damaged, message in [(b'\0' + slots[1:], 'invalid fixed-width filename'),
                                 (slots[:4] + b'X' + slots[5:], 'invalid fixed-width filename'),
                                 (slots[:-1] + b'X', 'invalid fixed-width filename')]:
            with self.subTest(damaged=damaged):
                self.write(original, damaged + names)
                self.assertIn(message, self.fails(content.assembly, entries, original, 0x8c500000))
        self.write(original, slots + names[:12] + struct.pack('<3I', 0x8c50000d, 0, 0))
        self.assertIn('expected texture_filename', self.fails(content.assembly, entries, original, 0x8c500000))
        for fields in (['2', '0'], ['2', '7'], ['2'], ['999999999', '999999996']):
            with self.subTest(fields=fields):
                self.fails(content.layout, 'texture_filenames', fields, 'example')

    def test_check_needs_only_local_tables(self):
        content.main(['track'])
        self.assertEqual([(obj.name, obj.address, obj.size) for obj in content.objects(Image('track')).values()],
                         [('route_a', 0x8c500004, 12), ('route_b', 0x8c500014, 12)])
        self.image('1st_read', SPLITS, SYMBOLS)
        self.write('config/track/symbols.txt', '')
        self.assertIn('has no symbol', self.fails(content.main, ['track']))

    def test_bad_rows_name_the_table_and_line(self):
        for field, replacement, problem in [('points', 'pointers', 'unknown data kind'),
                                            ('points 1', 'points 0', 'positive decimal'),
                                            ('points 1', 'points -1', 'positive decimal'),
                                            ('route_a', 'bad:name', 'ADDRESS NAME KIND COUNT'),
                                            ('8c500004', '8c500006', 'alignment'),
                                            ('route_a', 'route_b', 'has no symbol'),
                                            ('points 1', 'points 2', 'within one disc C, D or DATA range')]:
            with self.subTest(replacement=replacement):
                self.write(self.table, TABLE.replace(field, replacement, 1))
                message = self.fails(content.main, ['track'])
                self.assertIn('config/track/data.txt:1:', message)
                self.assertIn(problem, message)

    def test_data_cannot_repeat_or_overlap_a_source(self):
        self.write(self.table, TABLE.splitlines()[0] + '\n' + TABLE)
        self.assertIn('overlaps or precedes', self.fails(content.main, ['track']))
        self.write(self.table, TABLE)
        self.write('modules/route.c', '')
        self.image('track', SPLITS.replace('disc D', 'modules/route.c D 0x8c500004 0x8c500010\ndisc D', 1), SYMBOLS)
        self.assertIn('overlaps modules/route.c', self.fails(content.main, ['track']))

    def test_truncated_input_preserves_previous_output(self):
        self.write('original', bytes(31))
        self.write('data.s', 'previous assembly\n')
        self.assertIn('too short for route_b', self.fails(content.main, ['track', 'original', 'data.s']))
        self.assertEqual(Path('data.s').read_text(), 'previous assembly\n')

    def test_output_cannot_replace_the_original(self):
        self.write('TRACK.BIN', bytes(32))
        Path('alias.bin').symlink_to('TRACK.BIN')
        for output in ('TRACK.BIN', 'alias.bin'):
            with self.subTest(output=output):
                errors = io.StringIO()
                with contextlib.redirect_stderr(errors):
                    self.assertEqual(self.fails(content.main, ['track', 'TRACK.BIN', output]), '2')
                self.assertIn('would replace the original', errors.getvalue())
                self.assertEqual(Path('TRACK.BIN').read_bytes(), bytes(32))

    def test_pointers_come_in_order_and_inside_their_object(self):
        self.write('TRACK.BIN', bytes(32))
        content.main(['track', 'TRACK.BIN', 'data.s'])
        for offsets in ((4, 0), (0, 2), (9,)):
            with self.subTest(offsets=offsets), \
                    mock.patch.object(content, 'record_pointers', lambda *_: ((offset, None) for offset in offsets)):
                self.assertIn(f'route_a has a pointer at offset {offsets[-1]}',
                              self.fails(content.main, ['track', 'TRACK.BIN', 'data.s']))
        with mock.patch.object(content, 'record_pointers', lambda *_: iter(((0, None), (4, None)))):
            content.main(['track', 'TRACK.BIN', 'data.s'])
        self.assertIn('.long 0\n.long 0\n.incbin "TRACK.BIN", 12, 4\n', Path('data.s').read_text())

    def test_a_pose_member_name_has_one_meaning(self):
        bank = types.SimpleNamespace(name='.tree', kind='chunk_pose_tree', where='data.txt:1')
        for entries, generated in [({'.tree': bank, '.tree.root': bank}, ['.tree.root']),
                                   ({'.tree': bank}, ['.tree.root', '.tree.root'])]:
            with self.subTest(generated=generated), \
                    mock.patch.object(content.chunk_poses, 'fields', lambda _: ((name, None) for name in generated)):
                self.assertIn('.tree generates .tree.root, which is already declared or generated',
                              self.fails(content.pose_members, entries))

    @unittest.skipUnless((SH_ELF / 'bin/sh-elf-gcc').is_file(), 'requires SH-4 GCC')
    def test_staff_font_agrees_with_the_credit_renderer(self):
        terms = {each.name: each.term for each in declarations(ROOT / 'modules/shared/staff_font.c')[2]
                 if each.initialized}
        kind, element, (extent,) = terms['staff_glyphs']
        self.assertEqual((kind, element), ('array', ('struct', 'staff_glyph')))
        self.assertEqual(terms['staff_photo_cell'], ('typedef', 'NJS_TEXANIM'))
        count = int(extent)
        source = ('#include "shared/staff_art.h"\n'
                  f'typedef char glyph_size[sizeof(staff_glyphs) == {12 * count} ? 1 : -1];\n'
                  'typedef char glyph_cell[__builtin_offsetof(struct staff_glyph, cell) == 4 ? 1 : -1];\n'
                  'typedef char glyph_width[__builtin_offsetof(struct staff_glyph, width) == 8 ? 1 : -1];\n'
                  f'extern struct staff_glyph staff_glyphs[{count}];\n'
                  'extern NJS_TEXANIM staff_photo_cell;\n')
        subprocess.run([SH_ELF / 'bin/sh-elf-gcc', '-ml', '-m4-single-only', '-std=c89', '-fsyntax-only', '-x', 'c', '-Imodules', '-Isdk/include', '-'],
                       input=source, cwd=ROOT, text=True, check=True)

    def test_sprite_cells_agree_with_their_game_declarations(self):
        (_, datum), = rows(ROOT / 'offsets/modules/chaostgrace/hud_art.txt')
        self.assertEqual(datum[:4], ['datum', 'race_hud_cells', 'C', '0x00000000'])
        source = ['#include "chaostgrace/hud_art.h"',
                  f'typedef char race_size[sizeof(race_hud_cells) == {int(datum[4])} ? 1 : -1];',
                  'extern const NJS_TEXANIM race_hud_cells[RACE_HUD_CELL_COUNT];']
        self.write('cells.c', '\n'.join(source) + '\n')
        compile_host(Path('cells').resolve(), '-fsyntax-only', '-std=c89', '-Imodules', Path('cells.c').resolve())

    @unittest.skipUnless((SH_ELF / 'bin/sh-elf-as').is_file(), 'requires SH-4 binutils')
    def test_target_object_keeps_float_bits_and_symbol_extents(self):
        # Distinguish negative zero, subnormals and a NaN payload. Copy only
        # the objects across the two gaps in the original image.
        first = bytes.fromhex('00000080 01000000 a100c07f')
        second = bytes.fromhex('0100803f 000080ff aabbccdd')
        original = Path('disc "A"') / 'TRACK.BIN'
        self.write(original, b'head' + first + b'skip' + second)
        content.main(['track', str(original), 'data.s'])
        tools = SH_ELF / 'bin'
        subprocess.run([tools / 'sh-elf-as', '--little', 'data.s', '-o', 'data.o'], check=True)
        subprocess.run([tools / 'sh-elf-objcopy', '-O', 'binary', '-j', '.data', 'data.o', 'data.bin'], check=True)
        self.assertEqual(Path('data.bin').read_bytes(), first + second)
        symbols = subprocess.check_output([tools / 'sh-elf-nm', '-P', '-S', 'data.o'], text=True)
        self.assertEqual([line.split() for line in symbols.splitlines()],
                         [['_route_a', 'D', '0', 'c'], ['_route_b', 'D', 'c', 'c']])


class ChunkObjects(TreeTest):
    base = 0x8c500000
    # The graph includes both forward and backward references, a null model,
    # and two distinct nodes that share geometry.
    fields = {
        16: 12, 20: 4,                         # model.vlist, model.plist
        44: 16, 84: 92, 88: None,              # root.model, child, sibling
        96: None, 136: None, 140: 144,         # first child
        148: 16, 188: None, 192: None,         # second child
    }
    table = """\
0x8c500004 .polygons chunk_polygons 3
0x8c50000c .vertices chunk_vertices 1
0x8c500010 .geometry chunk_model .vertices .polygons
0x8c500028 root chunk_object .geometry .child -
0x8c50005c .child chunk_object - - .sibling
0x8c500090 .sibling chunk_object .geometry - -
"""

    def setUp(self):
        super().setUp()
        splits = ['file TRACK.BIN', 'P 0x8c500000 0x8c500004', 'D 0x8c500004 0x8c5000c4']
        for start, end in [(4, 10), (10, 12), (12, 16), (16, 40), (40, 92), (92, 144), (144, 196)]:
            splits.append(f'disc D {self.base + start:#010x} {self.base + end:#010x}')
        self.image('track', '\n'.join(splits), '0x8c500028 root\n')
        self.write('config/track/data.txt', self.table)
        self.original = bytearray(range(196))
        self.original[10:12] = bytes(2)  # natural four-byte vertex alignment
        for offset, target in self.fields.items():
            struct.pack_into('<I', self.original, offset, self.base + target if target is not None else 0)
        self.write('TRACK.BIN', bytes(self.original))

    def test_prize_tables_relocate_public_models_from_the_loaded_provider(self):
        owner = self.base + 0x1000
        self.image('chao', f'file GARDEN.BIN\nD {owner:#010x} {owner + 52:#010x}\n'
                             f'disc D {owner:#010x} {owner + 52:#010x}\n', f'{owner:#010x} toy\n')
        self.write('config/chao/data.txt', f'{owner:#010x} toy chunk_object - - -\n')
        self.image('chaostgrace', f'file RACE.BIN\nD {self.base:#010x} {self.base + 12:#010x}\n'
                           f'disc D {self.base:#010x} {self.base + 12:#010x}\n',
                   f'{self.base:#010x} garden_toys\n{self.base + 4:#010x} personal_toys\n{owner:#010x} toy\n')
        table = (f'{self.base:#010x} garden_toys chunk_object_ptrs 1\n'
                 f'{self.base + 4:#010x} personal_toys personal_toys 1\n')
        path = 'config/chaostgrace/data.txt'
        self.write(path, table + 'import chao toy\n')
        self.write('RACE.BIN', struct.pack('<3I', owner, owner, 0))
        argv = ['chaostgrace', 'RACE.BIN', 'race.s']
        content.main(argv)
        self.assertEqual(Path('race.s').read_text().count('.long _toy\n'), 2)
        Path('config/chao').rename('config/1st_read')
        self.write(path, table + 'import 1st_read toy')
        content.main(argv)
        self.assertEqual(Path('race.s').read_text().count('.long _toy\n'), 2)
        Path('config/1st_read').rename('config/chao')
        self.write(path, table + 'import chao toy')
        for stored, diagnostic in [((owner + 4, owner, 0), 'expected a chunk_object'),
                                   ((owner, 0, 0), 'model and a zero reserved'),
                                   ((owner, owner, 1), 'zero reserved')]:
            with self.subTest(stored=stored):
                self.write('RACE.BIN', struct.pack('<3I', *stored))
                self.assertIn(diagnostic, self.fails(content.main, argv))
        for declaration, diagnostic in [('import chao toy\nimport chao toy', 'repeated import'),
                                        ('import chao private_node', 'same public symbol'),
                                        ('import chaostgrace garden_toys', 'separate provider'),
                                        ('import stg13 toy', 'not loaded beside chaostgrace'),
                                        ('import missing', 'expected import')]:
            with self.subTest(declaration=declaration):
                self.write(path, table + declaration)
                self.assertIn(diagnostic, self.fails(content.main, ['chaostgrace']))
        # an import no pointer table of the image could take fails without the disc; one a table could take
        # but none points to fails with it
        self.write(path, f'{self.base:#010x} garden_toys sint32 3\nimport chao toy\n')
        self.assertIn('chao.toy is a chunk_object, which no pointer table of chaostgrace takes',
                      self.fails(content.main, ['chaostgrace']))
        self.write(path, f'{self.base:#010x} garden_toys chunk_object_ptrs 3\nimport chao toy\n')
        self.write('RACE.BIN', bytes(12))
        self.assertIn('no pointer table references', self.fails(content.main, argv))
        self.write(path, table + 'import chao toy')
        self.write('config/chao/data.txt', f'{owner:#010x} toy chunk_vertices 13\n')
        self.assertIn('chao.toy is a chunk_vertices, which no pointer table of chaostgrace takes',
                      self.fails(content.main, ['chaostgrace']))
        self.write('config/chao/data.txt', f'{owner:#010x} .toy chunk_object - - -\n')
        self.assertIn('same public symbol', self.fails(content.main, ['chaostgrace']))
        self.write('config/chao/data.txt', f'{owner:#010x} toy chunk_object - - -\n')
        self.write('config/chaostgrace/symbols.txt', f'{self.base:#010x} garden_toys\n'
                                              f'{self.base + 4:#010x} personal_toys\n{owner + 4:#010x} toy\n')
        self.assertIn('same public symbol', self.fails(content.main, ['chaostgrace']))

    def test_target_kind_name_and_arity_are_checked_without_the_disc(self):
        for old, new, error in [('.vertices .polygons', '.polygons .vertices', 'vlist needs'),
                                ('.geometry .child -', '0x8c500010 .child -', 'model needs'),
                                ('.geometry .child -', '.geometry absent -', 'child needs'),
                                ('.geometry .child -', '.geometry .child', 'needs pointer targets'),
                                ('.sibling chunk_object', '.child chunk_object', 'already declared'),
                                ('.polygons chunk_polygons', 'polygons chunk_polygons', 'has no symbol')]:
            with self.subTest(new=new):
                self.write('config/track/data.txt', self.table.replace(old, new))
                self.assertIn(error, self.fails(content.main, ['track']))

    def test_animal_rows_relocate_only_to_objects_of_the_declared_kind(self):
        splits = ('file TRACK.BIN\nP 0x8c500000 0x8c500004\nD 0x8c500004 0x8c5000d4\n'
                  'disc D 0x8c500004 0x8c5000d4\n')
        self.image('track', splits, '0x8c500028 root\n')
        self.write('config/track/data.txt', self.table + '0x8c5000c4 .animal chunk_object_ptrs 3\n'
                   '0x8c5000d0 .species chunk_object_tables 1\n')
        body = self.original + struct.pack('<4I', self.base + 92, 0, self.base + 40, self.base + 196)
        self.write('TRACK.BIN', bytes(body))
        content.main(['track', 'TRACK.BIN', 'data.s'])
        assembly = Path('data.s').read_text()
        self.assertIn('.Lanimal:\n.long .Lchild\n.long 0\n.long _root\n', assembly)
        self.assertIn('.Lspecies:\n.long .Lanimal\n', assembly)
        for offset, wrong in ((196, 17), (196, 16), (208, 92)):
            with self.subTest(offset=offset, wrong=wrong):
                altered = body.copy()
                struct.pack_into('<I', altered, offset, self.base + wrong)
                self.write('TRACK.BIN', bytes(altered))
                self.assertIn('points to', self.fails(content.main, ['track', 'TRACK.BIN', 'data.s']))

    def test_texture_scroll_consumes_one_uv_key_per_vertex(self):
        keys = (1, 1, 11, 2, -3, 4, 5, -6)
        obj = content.Object('.uvs', self.base, 2 * len(keys), 2, 0, 'D', 'texture_uv_keys', (), 'UV keys')
        content.texture_data.check_uvs(obj, struct.pack('<8h', *keys), 0)
        for index, wrong in ((0, 2), (1, 2), (2, -1), (3, -1), (3, 1), (3, 3)):
            with self.subTest(index=index, value=wrong), self.assertRaises(SystemExit):
                bad = list(keys)
                bad[index] = wrong
                content.texture_data.check_uvs(obj, struct.pack('<8h', *bad), 0)

    def test_a_whole_placement_accepts_only_the_required_zero_alignment(self):
        splits = ('file TRACK.BIN\nP 0x8c500000 0x8c500004\nD 0x8c500004 0x8c5000c4\n'
                  'disc D 0x8c500004 0x8c5000c4\n')
        self.image('track', splits, '0x8c500028 root\n')
        content.main(['track', 'TRACK.BIN', 'data.s'])
        altered = self.original.copy()
        altered[10] = 1
        self.write('TRACK.BIN', bytes(altered))
        self.assertIn('nonzero alignment', self.fails(content.main, ['track', 'TRACK.BIN', 'data.s']))
        for omitted, error in [(0, 'expected'), (1, 'expected'), (5, 'short of disc range')]:
            lines = self.table.splitlines()
            del lines[omitted]
            self.write('config/track/data.txt', '\n'.join(lines))
            self.assertIn(error, self.fails(content.main, ['track']))

    def test_shared_layout_keeps_member_local_names_and_exact_bounds(self):
        splits = ('file TRACK.BIN\nP 0x8c500000 0x8c500004\nD 0x8c500004 0x8c5000c4\n'
                  'disc D 0x8c500004 0x8c5000c4\n')
        self.image('track', splits, '0x8c500028 root\n')
        member = '\n'.join(row.split(maxsplit=1)[1] for row in self.table.splitlines())
        self.write('layouts/sample.txt', member)
        self.write('config/track/data.txt', '0x8c500004 layout sample')
        content.main(['track', 'TRACK.BIN', 'data.s'])
        assembly = Path('data.s').read_text()
        self.assertIn('.long .Lsample.geometry', assembly)
        self.assertNotIn('.global .L', assembly)
        # The same member is loaded at a different address in another image.
        self.image('menu', splits.replace('8c50', '8c62'), '0x8c620028 root\n')
        self.write('config/menu/data.txt', '0x8c620004 layout sample')
        menu = self.original.copy()
        for offset, target in self.fields.items():
            if target is not None:
                struct.pack_into('<I', menu, offset, 0x8c620000 + target)
        self.write('MENU.BIN', bytes(menu))
        content.main(['menu', 'MENU.BIN', 'menu.s'])
        self.assertEqual(Path('menu.s').read_text().replace('MENU.BIN', 'TRACK.BIN'), assembly)
        self.image('track', splits.replace('disc D 0x8c500004', 'disc D 0x8c500004 0x8c500010\ndisc D 0x8c500010'),
                   '0x8c500028 root\n')
        self.assertIn('must own one complete', self.fails(content.main, ['track']))
        self.write('config/track/data.txt', '0x8c500004 layout ../sample')
        self.assertIn('invalid layout', self.fails(content.main, ['track']))

    def test_each_retail_word_including_null_must_agree(self):
        self.write('data.s', 'previous output')
        for offset in self.fields:
            with self.subTest(offset=offset):
                altered = self.original.copy()
                altered[offset] ^= 4
                self.write('TRACK.BIN', bytes(altered))
                self.assertIn('points to', self.fails(content.main, ['track', 'TRACK.BIN', 'data.s']))
                self.assertEqual(Path('data.s').read_text(), 'previous output')

    @unittest.skipUnless((SH_ELF / 'bin/sh-elf-as').is_file(), 'requires SH-4 binutils')
    def test_graph_relocates_with_only_its_root_exported(self):
        content.main(['track', 'TRACK.BIN', 'data.s'])
        tools = SH_ELF / 'bin'
        subprocess.run([tools / 'sh-elf-as', '--little', 'data.s', '-o', 'data.o'], check=True)
        symbols = subprocess.check_output([tools / 'sh-elf-nm', '-g', '-P', '-S', 'data.o'], text=True)
        self.assertEqual(symbols.split(), ['_root', 'D', '24', '34'])
        relocs = subprocess.check_output([tools / 'sh-elf-readelf', '-r', 'data.o'], text=True)
        self.assertEqual(relocs.count('R_SH_DIR32'), sum(value is not None for value in self.fields.values()))
        # Rebase the graph onto a different address. Every pointer must move
        # to its new object while every scalar byte stays exactly as stored.
        load = 0x8c622004
        self.write('image.ld', f'SECTIONS {{ . = {load:#x}; .data : {{ *(.data) }} }}')
        subprocess.run([tools / 'sh-elf-ld', '-EL', '-T', 'image.ld', 'data.o', '-o', 'image'], check=True)
        subprocess.run([tools / 'sh-elf-objcopy', '-O', 'binary', '-j', '.data', 'image', 'data.bin'], check=True)
        expected = self.original[4:]
        for offset, target in self.fields.items():
            if target is not None:
                struct.pack_into('<I', expected, offset - 4, load + target - 4)
        self.assertEqual(Path('data.bin').read_bytes(), bytes(expected))


class TheDisc(unittest.TestCase):
    def test_every_table_agrees_with_its_original(self):
        # Only the goals that build the GCC images give content.py an
        # original. No gate runs them.
        files = {table.parent.name: Path('orig', Image(table.parent.name).file)
                 for table in sorted(ROOT.glob('config/*/data.txt'))}
        missing = [file for file in files.values() if not (ROOT / file).is_file()]
        if missing:
            self.skipTest(f"needs the disc's files in orig/, and {missing[0]} is missing")
        with tempfile.TemporaryDirectory() as directory:
            work = Path(directory)
            for image, file in files.items():
                with self.subTest(image=image):
                    checked = subprocess.run([sys.executable, 'tools/content.py', image, original_image(file, work),
                                              work / f'{image}.s'], cwd=ROOT, capture_output=True, text=True)
                    if checked.returncode:
                        self.fail(checked.stderr)
