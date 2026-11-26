"""The census of an image: the names its link-time compile leaves undefined."""

import struct
from pathlib import Path

import census
from support import TreeTest, run

# a symbol's binding, as its info's top four bits: local, global and weak
LOCAL, GLOBAL, WEAK = 0, 1, 2
# a section index that defines a symbol, and the one that leaves it undefined
DEFINED, UNDEFINED = 1, 0


def elf(symbols, kind=1, cls=1, data=1, table=True, machine=42):
    """A relocatable ELF's bytes: its header, a text section, its symbol
    table and the table's strings, and their section headers. symbols are
    (name, binding, section index) entries. kind, cls and data are the
    header's type, class and data, and without table the file holds no
    symbol table."""
    strings, entries = (b'\0' if symbols else b''), [bytes(16)]
    for name, binding, index in symbols:
        entries.append(struct.pack('<IIIBBH', len(strings), 0, 0, binding << 4, 0, index))
        strings += name.encode() + b'\0'
    symtab = b''.join(entries)
    body = symtab + strings
    sections = [bytes(40),
                struct.pack('<IIIIIIIIII', 0, 1, 6, 0, 52, 0, 0, 0, 4, 0),
                struct.pack('<IIIIIIIIII', 0, 2 if table else 1, 0, 0, 52, len(symtab), 3, 1, 4, 16),
                struct.pack('<IIIIIIIIII', 0, 3, 0, 0, 52 + len(symtab), len(strings), 0, 0, 1, 0)]
    header = struct.pack('<16sHHIIIIIHHHHHH', b'\x7fELF' + bytes([cls, data, 1]) + bytes(9), kind, machine, 1, 0, 0,
                         52 + len(body), 0, 52, 0, 0, 40, len(sections), 0)
    return header + body + b''.join(sections)


class Census(TreeTest):
    def test_names_each_undefined_name_without_its_underscore_in_order(self):
        self.write('a.o', elf([('_zeta', GLOBAL, UNDEFINED), ('_alpha', WEAK, UNDEFINED), ('_kept', GLOBAL, DEFINED),
                               ('_local', LOCAL, UNDEFINED), ('__divls', GLOBAL, UNDEFINED)]))
        self.assertEqual(census.census('a.o'), ['_divls', 'alpha', 'zeta'])

    def test_leaves_out_what_the_main_executable_defines(self):
        self.write('stage.o', elf([('_taken', GLOBAL, UNDEFINED), ('_needed', GLOBAL, UNDEFINED)]))
        self.write('main.o', elf([('_taken', GLOBAL, DEFINED), ('_needed', GLOBAL, UNDEFINED)]))
        self.assertEqual(census.census('stage.o', 'main.o'), ['needed'])

    def test_leaves_out_each_name_the_main_executables_table_gives(self):
        # _nj_screen_ lies in the main executable's disc bytes: its table names it, and no object defines it
        self.write('stage.o', elf([('__nj_screen_', GLOBAL, UNDEFINED), ('_needed', GLOBAL, UNDEFINED),
                                   ('_made', GLOBAL, UNDEFINED)]))
        self.write('main.o', elf([('_made', GLOBAL, DEFINED), ('__nj_screen_', GLOBAL, UNDEFINED)]))
        self.write('config/1st_read/symbols.txt', '0x8c1a0000 _nj_screen_\n')
        self.assertEqual(census.census('stage.o', 'main.o', 'config/1st_read/symbols.txt'), ['needed'])
        self.assertEqual(census.census('stage.o', None, 'config/1st_read/symbols.txt'), ['made', 'needed'])
        self.assertEqual(census.census('stage.o', 'main.o'), ['_nj_screen_', 'needed'])

    def test_an_empty_path_for_the_main_executable_or_its_table_fails(self):
        self.write('stage.o', elf([('_needed', GLOBAL, UNDEFINED)]))
        self.assertIn('Is a directory', self.fails(census.census, 'stage.o', ''))
        self.assertIn('Is a directory', self.fails(census.census, 'stage.o', None, ''))

    def test_fails_on_a_malformed_row_of_the_table(self):
        self.write('stage.o', elf([]))
        self.write('config/1st_read/symbols.txt', '0x8c010000\n')
        self.assertIn('config/1st_read/symbols.txt:1: not a row: ADDRESS NAME',
                      self.fails(census.census, 'stage.o', None, 'config/1st_read/symbols.txt'))

    def test_a_definition_wins_over_another_symbol_of_the_same_name_in_either_order(self):
        self.write('a.o', elf([('_first', WEAK, DEFINED), ('_first', GLOBAL, UNDEFINED),
                               ('_last', WEAK, UNDEFINED), ('_last', GLOBAL, DEFINED)]))
        self.assertEqual(census.symbols('a.o'), {'_first': True, '_last': True})

    def test_fails_on_a_name_no_c_name_spells(self):
        self.write('a.o', elf([('label', GLOBAL, UNDEFINED)]))
        self.assertIn("a.o: label: undefined, and no C name", self.fails(census.census, 'a.o'))

    def test_fails_on_a_file_it_cant_read_as_a_relocatable_elf(self):
        for name, data, refused in (('short', b'\x7fELF', 'not an ELF file'),
                                    ('executable', elf([], kind=2), 'not a 32-bit little-endian relocatable ELF'),
                                    ('64-bit', elf([], cls=2), 'not a 32-bit little-endian relocatable ELF'),
                                    ('big-endian', elf([], data=2), 'not a 32-bit little-endian relocatable ELF'),
                                    ('no table', elf([], table=False), 'no symbol table')):
            with self.subTest(name):
                self.write('a.o', data)
                self.assertIn(f'a.o: {refused}', self.fails(census.census, 'a.o'))

    def test_a_symbol_table_holding_only_the_null_symbol_needs_no_names(self):
        self.write('empty.o', elf([]))
        self.assertEqual(census.symbols('empty.o'), {})

    def test_requires_superh_even_for_another_little_endian_32_bit_object(self):
        # EM_ARM, a valid ELF32 target distinct from the Dreamcast's SuperH.
        self.write('arm.o', elf([('_external', GLOBAL, UNDEFINED)], machine=40))
        self.assertIn('arm.o: not a 32-bit little-endian relocatable ELF for SH',
                      self.fails(census.census, 'arm.o'))

    def test_writes_the_census_one_name_to_a_line(self):
        self.write('build/dc/census/stg13.o', elf([('_b', GLOBAL, UNDEFINED), ('_a', GLOBAL, UNDEFINED)]))
        self.write('build/dc/census/1st_read.o', elf([('_b', GLOBAL, DEFINED)]))
        self.write('config/1st_read/symbols.txt', '0x8c010000 c\n')
        status, output = run(census.main, 'build/dc/census/stg13.o', 'build/dc/census/stg13.txt', '--main',
                             'build/dc/census/1st_read.o', '--symbols', 'config/1st_read/symbols.txt')
        self.assertEqual((status, output), (0, ''))
        self.assertEqual(Path('build/dc/census/stg13.txt').read_text(), 'a\n')
