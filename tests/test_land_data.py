"""A static land relocates both kinds of object and its texture filenames."""

from pathlib import Path
import struct
import subprocess
import unittest

import content
from kinds import LAND_OBJECT
import land_data
from support import ROOT, SH_ELF, TreeTest


class StageFilenameProvider(unittest.TestCase):
    def test_a_bank_of_numbered_texts_reads_from_its_declaration(self):
        # content reads a bank's widths from its declared type, whatever its initializer: none for a char array,
        # and each member's for a struct. An image whose data stage 10 declared has no data.txt for content to read
        for image, bank, widths in (('stg37', 'shaft_rotor_texture_name', ()),
                                    ('stg18', 'lagoon_container_variant_names', (8, 7))):
            with self.subTest(image=image):
                if not (ROOT / 'config' / image / 'data.txt').exists():
                    self.skipTest(f'stage 10 declared {image}: content reads no data.txt there')
                self.assertEqual(content.objects(content.Image(image))[bank].filename_widths, widths)


class Checks(unittest.TestCase):
    def test_a_bank_reads_only_whole_slots_inside_the_image(self):
        def bank(address):
            return content.Object('names', address, 4, 4, 0, 'C', 'land_filenames', ('a.c',), 'row')

        data = b'one\0two\0'
        self.assertEqual(list(land_data.bank_strings(bank(0x1004), data, 0x1000)),
                         [land_data.BankString(0x1004, '_names', 'texture_filename')])
        for address in (0x0ffc, 0x1008):
            with self.subTest(address=address), self.assertRaisesRegex(SystemExit, 'does not hold names'):
                list(land_data.bank_strings(bank(address), data, 0x1000))

    def test_a_bank_checks_each_slot_before_it_yields_it(self):
        bank = content.Object('names', 0, 4, 4, 0, 'C', 'land_filenames', ('a.c',), 'row')
        with self.assertRaisesRegex(SystemExit, 'nonzero alignment'):
            next(land_data.bank_strings(bank, b'a\0!\0', 0))

    def test_any_negative_drawn_count_takes_every_entry_in_one_count(self):
        land = content.Object('land', 0, 16, 4, 0, 'D', 'land_table', ('.entries', '-'), 'row')
        entries = content.Object('.entries', 16, 96, 4, 0, 'D', 'land_entries', ('3',), 'row')
        for drawn in (-1, -2):
            with self.subTest(drawn=drawn):
                land_data.check_record(land, struct.pack('<4h', 3, drawn, 0, 0), 0, {'.entries': entries})

    def test_a_sublist_of_an_undeclared_array_names_its_row(self):
        texlist = content.Object('list', 0, 8, 4, 0, 'D', 'texture_list', ('missing', '0', '1'), 'row')
        with self.assertRaisesRegex(SystemExit, 'row: list selects an invalid range of missing'):
            land_data.check_record(texlist, struct.pack('<II', 0, 1), 0, {})


class PaddedPvmNames(unittest.TestCase):
    def test_padded_record_binds_a_named_texture_slot(self):
        name = content.Object('.tx', 32, 32, 4, 0, 'C', 'padded_texture_filename', (), 'PVM')
        slots = content.Object('.slots', 64, 12, 4, 0, 'D', 'texture_names', ('1',), 'PVM')
        data = bytearray(76)
        data[32:64] = b'kart01'.ljust(28, b' ') + bytes(4)
        struct.pack_into('<I', data, 64, name.address)
        land_data.check_record(name, data, 32, {})
        self.assertEqual(list(land_data.array_pointers(slots, data, 64, {32: name})), [(0, name)])
        for position, value in ((32, 32), (36, 0), (59, 255), (63, 1)):
            with self.subTest(position=position):
                changed = data[:]
                changed[position] = value
                with self.assertRaisesRegex(SystemExit, 'padded PVM name'):
                    land_data.check_record(name, changed, 32, {})
        struct.pack_into('<I', data, 64, name.address + 1)
        with self.assertRaisesRegex(SystemExit, 'expected texture_filename'):
            list(land_data.array_pointers(slots, data, 64, {32: name}))


class SharedTextures(unittest.TestCase):
    def test_archive_and_group_tables_keep_their_links_and_terminators(self):
        def item(name, kind, at, size):
            return content.Object(name, at, size, 4, 0, 'D', kind, (), 'shared textures')

        filename = item('archive', 'texture_filename', 8, 8)
        texlist = item('list', 'texture_list', 16, 8)
        archives = item('pvms', 'texture_pvms', 24, 24)
        group = item('models', 'texture_group', 48, 8)
        groups = item('groups', 'texture_groups', 56, 8)
        addresses = {obj.address: obj for obj in (filename, texlist, group)}
        image = bytearray(64)
        struct.pack_into('<6I', image, 24, 8, 16, 0, 16, 0, 0)
        struct.pack_into('<4I', image, 48, 16, 0, 48, 0)

        def pointers(obj):
            read = land_data.array_pointers if obj == archives else content.object_array_pointers
            return list(read(obj, image, obj.address, addresses))

        self.assertEqual(pointers(archives), [(0, filename), (4, texlist), (8, None), (12, texlist),
                                               (16, None), (20, None)])
        self.assertEqual(pointers(group), [(0, texlist), (4, None)])
        self.assertEqual(pointers(groups), [(0, group), (4, None)])
        for obj, at, value in ((archives, 24, 16), (archives, 28, 8), (archives, 28, 0),
                               (archives, 40, 8), (archives, 44, 16), (group, 48, 8),
                               (group, 48, 0), (group, 52, 16), (groups, 56, 16),
                               (groups, 56, 0), (groups, 60, 48)):
            with self.subTest(table=obj.kind, offset=at, value=value):
                word = image[at:at + 4]
                struct.pack_into('<I', image, at, value)
                with self.assertRaises(SystemExit):
                    pointers(obj)
                image[at:at + 4] = word


class ContainerArt(TreeTest):
    base = 0x8c500000
    table = '''\
0x8c500000 .textures texture_list .slots
0x8c500008 .model chunk_model - -
0x8c500020 .object chunk_object .model - -
0x8c500054 pieces container_pieces 2
0x8c50006c builds model_builds 2
0x8c50008c .slots texture_names 1 empty
'''

    def setUp(self):
        super().setUp()
        self.image('stage', 'file ART.BIN\nD 0x8c500000 0x8c500098\ndisc D 0x8c500000 0x8c500098\n',
                   '0x8c500054 pieces\n0x8c50006c builds\n')
        self.write('config/stage/data.txt', self.table)
        self.original = bytearray(152)
        textures, model, node = self.base, self.base + 8, self.base + 32
        struct.pack_into('<2I', self.original, 0, self.base + 140, 1)
        struct.pack_into('<I', self.original, 36, model)
        struct.pack_into('<6I', self.original, 84, textures, model, 1, textures, model, 2)
        struct.pack_into('<8I', self.original, 108, 1, textures, node, 0, 3, 0, node, 0)

    def test_pieces_and_builds_relocate_each_field(self):
        self.write('ART.BIN', bytes(self.original))
        content.main(['stage', 'ART.BIN', 'art.s'])
        assembly = Path('art.s').read_text()
        pieces = assembly.split('_pieces:\n')[1].split('.size ')[0]
        builds = assembly.split('_builds:\n')[1].split('.size ')[0]
        self.assertEqual([line for line in pieces.splitlines() if line.startswith('.long')],
                         ['.long .Ltextures', '.long .Lmodel'] * 2)
        self.assertEqual([line for line in builds.splitlines() if line.startswith('.long')],
                         ['.long .Ltextures', '.long .Lobject', '.long 0',
                          '.long 0', '.long .Lobject', '.long 0'])
        for position in (92, 104, 108, 124):
            self.assertIn(f'.incbin "ART.BIN", {position}, 4', assembly)

    def test_signed_counts_and_default_build_kinds_pass_through(self):
        for offset, value in [(92, -1), (104, 0), (108, -1), (124, 4)]:
            struct.pack_into('<i', self.original, offset, value)
        struct.pack_into('<2I', self.original, 84, 0, 0)
        self.write('ART.BIN', bytes(self.original))
        content.main(['stage', 'ART.BIN', 'art.s'])
        assembly = Path('art.s').read_text()
        pieces = assembly.split('_pieces:\n')[1].split('.size ')[0]
        self.assertEqual(pieces.splitlines()[:2], ['.long 0', '.long 0'])
        for offset in (92, 104, 108, 124):
            self.assertIn(f'.incbin "ART.BIN", {offset}, 4', assembly)

    def test_rejects_invalid_fields_and_read_only_builds(self):
        for offset, value, error in [(96, self.base + 8, 'expected texture_list'),
                                     (100, self.base + 32, 'expected chunk_model'),
                                     (128, self.base + 8, 'expected texture_list'),
                                     (132, 0, 'needs a chunk_object'), (132, self.base + 8, 'expected chunk_object'),
                                     (136, self.base + 32, 'expected a null pointer')]:
            with self.subTest(offset=offset, value=value):
                damaged = self.original[:]
                struct.pack_into('<I', damaged, offset, value)
                self.write('ART.BIN', bytes(damaged))
                self.assertIn(error, self.fails(content.main, ['stage', 'ART.BIN', 'art.s']))
        path = Path('config/stage/splits.txt')
        path.write_text(path.read_text().replace('D 0x', 'C 0x'))
        self.assertIn('writable storage', self.fails(content.main, ['stage']))


class Terrain(TreeTest):
    items = [
        ('.archive', 'texture_filename', '4'), ('.filename', 'texture_filename', '4'),
        ('.entries', 'land_entries', '1', '1'), ('land', 'land_table', '.entries', '-', '.archive', '.texlist'),
        ('.textures', 'texture_names', '1'), ('.texlist', 'texture_list', '.textures'),
        ('.indices', 'basic_indices', '3'), ('.materials', 'basic_materials', '1'),
        ('.points', 'points', '3'), ('.meshsets', 'basic_meshsets', '1'),
        ('.basic', 'basic_model', '.points', '-', '.meshsets', '.materials'),
        ('.polygons', 'chunk_polygons', '1'), ('.vertices', 'chunk_vertices', '1'),
        ('.chunk', 'chunk_model', '.vertices', '.polygons'),
        ('.visible', 'chunk_object', '.chunk', '-', '-'), ('.solid', 'basic_object', '.basic', '-', '-'),
    ]

    def setUp(self):
        super().setUp()
        base = 0x8c500000
        cursor = base
        rows = []
        self.offsets = {}
        for name, kind, *arguments in self.items:
            size, alignment, _ = content.layout(kind, arguments, 'test')
            cursor = (cursor + alignment - 1) // alignment * alignment
            self.offsets[name] = cursor - base
            rows.append(f'{cursor:#010x} {name} {kind} ' + ' '.join(arguments))
            cursor += size
        self.image('stage', f'file LAND.BIN\nC {base:#010x} {base+8:#010x}\n'
                   f'D {base+8:#010x} {cursor:#010x}\ndisc C {base:#010x} {base+8:#010x}\n'
                   f'disc D {base+8:#010x} {cursor:#010x}\n', f'{base+self.offsets["land"]:#010x} land\n')
        self.write('config/stage/data.txt', '\n'.join(rows))
        self.original = bytearray(cursor - base)
        self.original[:8] = b'pvm\0tex\0'

        def ptr(name):
            return base + self.offsets[name]

        self.pack('land', 0, '4h', 2, 1, -1, 0)
        self.pack('land', 16, '4I', ptr('.entries'), 0, ptr('.archive'), ptr('.texlist'))
        self.pack('.entries', 16, '4I', ptr('.visible'), 0, 0, 0x80000000)
        self.pack('.entries', 48, 'I', ptr('.solid'))
        self.pack('.textures', 0, 'I', ptr('.filename'))
        self.pack('.texlist', 0, 'II', ptr('.textures'), 1)
        self.pack('.meshsets', 0, 'HHI', 0, 1, ptr('.indices'))
        self.pack('.basic', 0, '5IHH', ptr('.points'), 0, 3, ptr('.meshsets'), ptr('.materials'), 1, 1)
        self.pack('.chunk', 0, '2I', ptr('.vertices'), ptr('.polygons'))
        self.pack('.visible', 4, 'I', ptr('.chunk'))
        self.pack('.solid', 4, 'I', ptr('.basic'))

    def pack(self, name, offset, fmt, *values):
        struct.pack_into('<' + fmt, self.original, self.offsets[name] + offset, *values)

    def test_every_kind_of_record_binds_its_own_symbol(self):
        self.write('LAND.BIN', bytes(self.original))
        content.main(['stage', 'LAND.BIN', 'generated.s'])
        asm = Path('generated.s').read_text().splitlines()
        self.assertIn('.long .Lvisible', asm)
        self.assertIn('.long .Lsolid', asm)
        self.assertIn('.long .Lindices', asm)
        self.assertIn('.long .Lfilename', asm)
        self.assertEqual([line for line in asm if line.startswith('.global ')], ['.global _land'])

    def test_texture_archive_can_fill_initially_empty_slots(self):
        table = Path('config/stage/data.txt')
        table.write_text(table.read_text().replace('.textures texture_names 1', '.textures texture_names 1 empty'))
        self.pack('.textures', 0, 'I', 0)
        self.write('LAND.BIN', bytes(self.original))
        content.main(['stage', 'LAND.BIN', 'generated.s'])
        self.assertNotIn('.long .Lfilename', Path('generated.s').read_text().splitlines())
        self.pack('.textures', 8, 'I', 1)
        self.write('LAND.BIN', bytes(self.original))
        with self.assertRaisesRegex(SystemExit, 'initially empty texture slots'):
            content.main(['stage', 'LAND.BIN', 'generated.s'])

    def test_empty_texture_slots_require_writable_storage(self):
        self.image('stage', 'file LAND.BIN\nC 0x8c500000 0x8c50000c\n'
                   'disc C 0x8c500000 0x8c50000c\n', '')
        self.write('config/stage/data.txt', '0x8c500000 .textures texture_names 1 empty\n')
        with self.assertRaisesRegex(SystemExit, 'writable D range'):
            content.main(['stage'])

    def test_mistyped_targets_counts_and_live_fields_are_rejected(self):
        initial = bytes(self.original)
        for name, offset, fmt, value, message in [
                ('.entries', 16, 'I', 0x8c500000 + self.offsets['.solid'], 'expected chunk_object'),
                ('.entries', 20, 'I', 0x8c500000 + self.offsets['.visible'], 'expected a null pointer'),
                ('.entries', 28, 'I', 0, 'drawn entry range'),
                ('.textures', 8, 'I', 1, 'unloaded named texture'),
                ('.meshsets', 4, 'I', 0x8c500000 + self.offsets['.indices'] + 2, 'expected basic_indices'),
                ('.basic', 8, 'I', 4, 'requires 4 elements'),
                ('.texlist', 4, 'I', 2, 'requires 2 elements'),
                ('land', 2, 'h', 2, 'entry counts'),
                ('.filename', 3, 'B', 65, 'first NUL')]:
            with self.subTest(name=name, offset=offset):
                self.original[:] = initial
                self.pack(name, offset, fmt, value)
                self.write('LAND.BIN', bytes(self.original))
                self.assertIn(message, self.fails(content.main, ['stage', 'LAND.BIN', 'generated.s']))

    def test_interleaved_entries_choose_model_type_from_flags(self):
        table = Path('config/stage/data.txt')
        table.write_text(table.read_text().replace('land_entries 1 1', 'land_entries 2'))
        self.pack('land', 2, 'h', -1)
        start = self.offsets['.entries']
        records = self.original[start:start + 64]
        self.original[start:start + 64] = records[32:] + records[:32]
        self.write('LAND.BIN', bytes(self.original))
        content.main(['stage', 'LAND.BIN', 'generated.s'])
        asm = Path('generated.s').read_text()
        self.assertLess(asm.index('.long .Lsolid'), asm.index('.long .Lvisible'))
        self.pack('.entries', 28, 'I', 0x80000000)
        self.write('LAND.BIN', bytes(self.original))
        self.assertIn('expected chunk_object', self.fails(content.main, ['stage', 'LAND.BIN', 'generated.s']))

    def test_texture_list_can_select_slots_inside_a_shared_array(self):
        base = 0x8c500000
        address = base + len(self.original)
        filename = base + self.offsets['.filename']
        self.original.extend(struct.pack('<8I', filename, 0, 0, filename, 0, 0, address + 12, 1))
        splits = Path('config/stage/splits.txt')
        splits.write_text(splits.read_text().replace(f'{address:#010x}', f'{address + 32:#010x}'))
        table = Path('config/stage/data.txt')
        table.write_text(table.read_text().rstrip() + f'\n{address:#010x} .names texture_names 2\n'
                         f'{address + 24:#010x} .water texture_list .names 1 1\n')
        self.write('LAND.BIN', bytes(self.original))
        content.main(['stage', 'LAND.BIN', 'generated.s'])
        self.assertIn('.long .Lnames+12', Path('generated.s').read_text().splitlines())
        self.original[-4:] = struct.pack('<I', 2)
        self.write('LAND.BIN', bytes(self.original))
        self.assertIn('invalid range', self.fails(content.main, ['stage', 'LAND.BIN', 'generated.s']))
        self.original[-8:] = struct.pack('<2I', address + 4, 1)
        self.write('LAND.BIN', bytes(self.original))
        self.assertIn('expected', self.fails(content.main, ['stage', 'LAND.BIN', 'generated.s']))

    def test_background_iterates_culling_entries_with_a_separate_object_count(self):
        table = Path('config/stage/data.txt')
        table.write_text(table.read_text().replace('land_entries 1 1', 'scenery_entries 2 3')
                         .replace('land land_table ', 'land scenery_land '))
        first = self.offsets['.entries']
        self.original[first + 32:first + 64] = self.original[first:first + 32]
        self.pack('land', 2, 'h', 3)
        self.write('LAND.BIN', bytes(self.original))
        content.main(['stage', 'LAND.BIN', 'generated.s'])
        self.assertEqual(Path('generated.s').read_text().splitlines().count('.long .Lvisible'), 2)
        self.pack('land', 2, 'h', 2)
        self.write('LAND.BIN', bytes(self.original))
        self.assertIn('entry counts', self.fails(content.main, ['stage', 'LAND.BIN', 'generated.s']))

    def use_compiled_filenames(self):
        source = 'modules/stage/terrain.c'
        table = Path('config/stage/data.txt')
        rows = table.read_text().splitlines()
        rows[0:2] = ['0x8c500000 land_names land_filenames ' + source + ' 8']
        table.write_text('\n'.join(row.replace('.archive', 'land_names') for row in rows))
        splits = Path('config/stage/splits.txt')
        splits.write_text(splits.read_text().replace('disc C ', source + ' C '))
        names = Path('config/stage/symbols.txt')
        names.write_text('0x8c500000 land_names\n' + names.read_text())
        self.write(source, 'const char land_names[8] = {\n'
                   '#include "stage/terrain/land_names.inc"\n};\n')
        self.write('offsets/modules/stage/terrain.txt', 'datum land_names C 0x00000000 8\n')
        return Path(source)

    def test_references_to_compiled_filenames_do_not_allocate_another_pool(self):
        self.use_compiled_filenames()
        self.write('LAND.BIN', bytes(self.original))
        content.main(['stage', 'LAND.BIN', 'generated.s'])
        text = Path('generated.s').read_text().splitlines()
        self.assertIn('.long _land_names', text)
        self.assertIn('.long _land_names+4', text)
        self.assertNotIn('.section .rodata', text)
        self.assertNotIn('_land_names:', text)
        self.pack('.textures', 0, 'I', 0x8c500005)
        self.write('LAND.BIN', bytes(self.original))
        self.assertIn('expected texture_filename', self.fails(content.main, ['stage', 'LAND.BIN', 'generated.s']))

    def test_archive_can_follow_texture_names_in_its_compiled_bank(self):
        self.use_compiled_filenames()
        table = Path('config/stage/data.txt')
        table.write_text(table.read_text().replace('terrain.c 8', 'terrain.c 8 4'))
        self.pack('land', 24, 'I', 0x8c500004)
        self.pack('.textures', 0, 'I', 0x8c500000)
        self.write('LAND.BIN', bytes(self.original))
        content.main(['stage', 'LAND.BIN', 'generated.s'])
        self.assertIn('.long _land_names+4', Path('generated.s').read_text().splitlines())
        self.original[:8] = b'longer!\0'
        self.write('LAND.BIN', bytes(self.original))
        self.assertIn('start of an archive filename', self.fails(content.main, ['stage', 'LAND.BIN', 'generated.s']))

    def test_a_filename_bank_must_have_a_complete_compiled_owner(self):
        source = self.use_compiled_filenames()
        content.main(['stage'])
        # its qualifier may follow its type
        self.write(source, source.read_text().replace('const char', 'char const'))
        content.main(['stage'])
        self.write(source, source.read_text().replace('char const', 'const unsigned char'))
        self.assertIn('public const char', self.fails(content.main, ['stage']))

    def test_adjacent_compiled_filename_arrays_keep_their_own_symbols_and_bounds(self):
        source = self.use_compiled_filenames()
        names = Path('config/stage/symbols.txt')
        names.write_text(names.read_text().replace('land_names\n', 'land_names\n0x8c500004 stone_name\n'))
        table = Path('config/stage/data.txt')
        table.write_text(table.read_text().replace(f'{source} 8',
            f'{source} 4\n0x8c500004 stone_name land_filenames {source} 4'))
        definition = source.read_text().replace('[8]', '[4]')
        self.write(source, definition + definition.replace('land_names', 'stone_name'))
        self.write('offsets/modules/stage/terrain.txt',
                   'datum land_names C 0x00000000 4\ndatum stone_name C 0x00000004 4\n')
        self.write('LAND.BIN', bytes(self.original))
        content.main(['stage', 'LAND.BIN', 'generated.s'])
        assembly = Path('generated.s').read_text()
        self.assertIn('.long _stone_name\n', assembly)
        self.assertNotIn('_stone_name:', assembly)
        self.original[3] = ord('x')
        self.write('LAND.BIN', bytes(self.original))
        self.assertIn('unterminated filename', self.fails(content.main, ['stage', 'LAND.BIN', 'generated.s']))

    def test_mixed_owner_fields_bound_each_filename(self):
        source = self.use_compiled_filenames()
        # The two four-byte fields follow an independent constant in the native C section.
        splits = Path('config/stage/splits.txt')
        splits.write_text(splits.read_text().replace('C 0x8c500000 0x8c500008',
                                                    'C 0x8c4ffffc 0x8c500008'))
        self.write('offsets/modules/stage/terrain.txt', 'datum land_names C 0x00000004 8\n')
        definition = ('struct texture_names { char archive[4]; char stone[4]; };\n'
                      'const struct texture_names land_names = {\n'
                      '#include "stage/terrain/land_names.inc"\n};\n')
        self.write(source, definition)
        self.write('LAND.BIN', b'room' + self.original)
        content.main(['stage', 'LAND.BIN', 'generated.s'])
        self.assertIn('.long _land_names+4', Path('generated.s').read_text().splitlines())
        self.write(source, definition.replace('char archive[4]; char stone[4]', 'char archive[8]'))
        self.assertIn('disagrees with its C field', self.fails(content.main, ['stage', 'LAND.BIN', 'generated.s']))
        self.write(source, definition.replace('char stone[4]', 'unsigned char stone[4]'))
        self.assertIn('word-aligned char array', self.fails(content.main, ['stage']))

    @unittest.skipUnless((SH_ELF / 'bin/sh-elf-gcc').is_file(), 'requires SH-4 GCC')
    def test_array_pointers_reads_its_scalars_at_their_gcc_offsets(self):
        # tests/test_kinds.py holds each kind's size and pointers to its C type
        source = ['#include "collision/world.h"']
        for ctype, field, offset in (('struct land_entry', 'flags', LAND_OBJECT.offset), ('NJS_TEXNAME', 'attr', 4),
                                     ('NJS_TEXNAME', 'texaddr', 8)):
            source.append(f'typedef char offset_{field}[__builtin_offsetof({ctype}, {field}) == {offset} ? 1 : -1];')
        subprocess.run([SH_ELF / 'bin/sh-elf-gcc', '-std=c89', '-fsyntax-only', '-x', 'c',
                        '-Isdk/include', '-Isrc', '-'], input='\n'.join(source) + '\n',
                       cwd=ROOT, text=True, check=True)
