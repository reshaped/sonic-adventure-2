"""The SH-4 ELF reader: the file header, the section directory and the one symbol table, each checked as it's read."""

import struct
import unittest

from sh_elf import ET_EXEC, ET_REL, SHT_NOBITS, SHT_STRTAB, SHT_SYMTAB, ElfTables, WrongElfType

# the sections elf() writes, by their index
TEXT, BSS, SYMTAB, STRTAB, NAMES = range(1, 6)
# the offset of each ELF header field a test changes, and its format
IDENT, TYPE, MACHINE, VERSION, SHOFF = (0, '16s'), (16, '<H'), (18, '<H'), (20, '<I'), (32, '<I')
EHSIZE, SHENTSIZE, SHNUM, SHSTRNDX = (40, '<H'), (46, '<H'), (48, '<H'), (50, '<H')
# the fields of a section header, each a 32-bit word, in order
SECTION_HEADER = ('sh_name', 'sh_type', 'sh_flags', 'sh_addr', 'sh_offset', 'sh_size', 'sh_link', 'sh_info',
                  'sh_addralign', 'sh_entsize')
# what sh_elf doesn't name: SuperH's and ARM's e_machine, a section of the file's bytes, a section's flags, and the
# special section indices of an absolute symbol and a common one
EM_SH, EM_ARM = 42, 40
SHT_PROGBITS = 1
SHF_WRITE, SHF_ALLOC, SHF_EXECINSTR = 1, 2, 4
SHN_ABS, SHN_COMMON, SHN_XINDEX = 0xfff1, 0xfff2, 0xffff
# a symbol's info: its binding in the top four bits and its type in the bottom four
LOCAL_FUNCTION, GLOBAL_OBJECT, WEAK_FUNCTION = 0x02, 0x11, 0x22
SYMBOLS = [('f', 0x8c010000, 4, LOCAL_FUNCTION, TEXT), ('_g', 0x8c010004, 8, GLOBAL_OBJECT, BSS),
           ('_h', 0x8c010002, 2, WEAK_FUNCTION, TEXT), ('_end', 0x8c01000c, 0, GLOBAL_OBJECT, SHN_ABS),
           ('_c', 4, 4, GLOBAL_OBJECT, SHN_COMMON), ('_extern', 0, 0, GLOBAL_OBJECT, 0)]


def elf(symbols=SYMBOLS, kind=ET_REL):
    """A little-endian SH ELF's bytes of kind: its header, then .text's 4 bytes, the symbols' table and strings and
    the sections' names, then the section headers. .bss holds 8 bytes, and none in the file. symbols are (name,
    value, size, info, section index) entries, which follow the null symbol."""
    strings, entries = b'\0', [bytes(16)]
    for name, value, size, info, index in symbols:
        entries.append(struct.pack('<IIIBBH', len(strings), value, size, info, 0, index))
        strings += name.encode() + b'\0'
    names = b'\0.text\0.bss\0.symtab\0.strtab\0.shstrtab\0'
    contents = [b'\x09\x00\x0b\x00', b''.join(entries), strings, names]
    at, offsets = 52, []
    for data in contents:
        offsets.append(at)
        at += len(data)
    text, symtab, strtab, shstrtab = zip(offsets, map(len, contents))
    # the index of the first symbol that isn't local: the null symbol and the locals come first
    first_global = 1 + sum(info >> 4 == 0 for _, _, _, info, _ in symbols)
    headers = [bytes(40),
               struct.pack('<10I', names.index(b'.text'), SHT_PROGBITS, SHF_ALLOC | SHF_EXECINSTR, 0x8c010000,
                           *text, 0, 0, 4, 0),
               struct.pack('<10I', names.index(b'.bss'), SHT_NOBITS, SHF_WRITE | SHF_ALLOC, 0x8c010004,
                           at, 8, 0, 0, 4, 0),
               struct.pack('<10I', names.index(b'.symtab'), SHT_SYMTAB, 0, 0, *symtab, STRTAB, first_global, 4, 16),
               struct.pack('<10I', names.index(b'.strtab'), SHT_STRTAB, 0, 0, *strtab, 0, 0, 1, 0),
               struct.pack('<10I', names.index(b'.shstrtab'), SHT_STRTAB, 0, 0, *shstrtab, 0, 0, 1, 0)]
    header = struct.pack('<16sHHIIIIIHHHHHH', b'\x7fELF\x01\x01\x01' + bytes(9), kind, EM_SH, 1, 0, 0, at, 0, 52, 0, 0,
                         40, len(headers), NAMES)
    return header + b''.join(contents) + b''.join(headers)


def put(data, field, value):
    """data with the field at (offset, format) set to value."""
    offset, form = field
    return data[:offset] + struct.pack(form, value) + data[offset + struct.calcsize(form):]


def section(data, index, name):
    """The field name, one of SECTION_HEADER, of section index's header in data, as put() takes a field."""
    (at,), (size,) = (struct.unpack_from(form, data, offset) for offset, form in (SHOFF, SHENTSIZE))
    return at + size * index + SECTION_HEADER.index(name) * 4, '<I'


class Read(unittest.TestCase):
    def test_reads_the_sections_and_the_symbols_of_an_object_and_an_executable(self):
        for kind in (ET_REL, ET_EXEC):
            with self.subTest(kind=kind):
                tables = ElfTables('a.elf', elf(kind=kind), kind)
                self.assertEqual(tables.file_type, kind)
                self.assertEqual([(s.name, s.kind, s.address) for s in tables.sections],
                                 [('', 0, 0), ('.text', SHT_PROGBITS, 0x8c010000), ('.bss', SHT_NOBITS, 0x8c010004),
                                  ('.symtab', SHT_SYMTAB, 0), ('.strtab', SHT_STRTAB, 0), ('.shstrtab', SHT_STRTAB, 0)])
                self.assertEqual(tables.symbol_table, tables.sections[SYMTAB])
                self.assertEqual(tables.contents(tables.sections[TEXT]), b'\x09\x00\x0b\x00')
                self.assertEqual(list(tables.symbols()), [('', 0, 0, 0, 0), *SYMBOLS])

    def test_reads_no_section_names_where_the_header_gives_none(self):
        # e_shstrndx 0, SHN_UNDEF, and every section's name at offset 0: an object may name no section, unless its
        # reader needs the names
        data = put(elf(), SHSTRNDX, 0)
        for index in range(TEXT, NAMES + 1):
            data = put(data, section(data, index, 'sh_name'), 0)
        self.assertEqual({s.name for s in ElfTables('a.elf', data).sections}, {''})
        with self.assertRaisesRegex(ValueError, r'^a\.elf: invalid section directory$'):
            ElfTables('a.elf', data, require_names=True)

    def test_names_an_inactive_section_nothing_and_keeps_its_place(self):
        # .text made SHT_NULL, its name offset past every name: the later sections keep their indices and names
        data = elf()
        data = put(put(data, section(data, TEXT, 'sh_type'), 0), section(data, TEXT, 'sh_name'), 0x1000)
        tables = ElfTables('a.elf', data)
        self.assertEqual([s.name for s in tables.sections], ['', '', '.bss', '.symtab', '.strtab', '.shstrtab'])
        self.assertEqual(tables.symbol_table, tables.sections[SYMTAB])

    def test_reads_no_file_bytes_for_a_section_that_holds_none(self):
        # .bss's offset at the section headers that follow it, then at the end of the file
        data = elf()
        for offset in (struct.unpack_from('<I', data, SHOFF[0])[0], len(data)):
            with self.subTest(offset=offset):
                tables = ElfTables('a.elf', put(data, section(data, BSS, 'sh_offset'), offset))
                self.assertEqual(tables.sections[BSS].size, 8)
                self.assertEqual(tables.contents(tables.sections[BSS]), b'')

    def test_refuses_a_negative_range_or_string_offset(self):
        tables = ElfTables('a.elf', elf())
        for offset, size in ((1, -2), (-1, 1)):
            with self.subTest(offset=offset, size=size), self.assertRaisesRegex(
                    ValueError, r'^a\.elf: negative ELF range'):
                tables.slice(offset, size)
        with self.assertRaisesRegex(ValueError, r'^a\.elf: invalid ELF string$'):
            tables.string(b'\0x\0', -2)
        self.assertEqual(tables.slice(len(tables.data), 0), b'')

    def test_refuses_another_machines_file_or_another_kind_as_the_wrong_type(self):
        # big-endian, 64-bit, ELF version 0 in the identity and in the header, ARM's, and a shared object (3): each
        # changes one identity field of an object whose tables hold together. Those fields, and the type a caller
        # asks for, alone raise WrongElfType, which a caller tells from a broken file
        cases = {'big-endian': put(elf(), IDENT, b'\x7fELF\x01\x02\x01' + bytes(9)),
                 '64-bit': put(elf(), IDENT, b'\x7fELF\x02\x01\x01' + bytes(9)),
                 'EI_VERSION 0': put(elf(), IDENT, b'\x7fELF\x01\x01\x00' + bytes(9)),
                 'e_version 0': put(elf(), VERSION, 0), 'ARM': put(elf(), MACHINE, EM_ARM),
                 'shared': put(elf(), TYPE, 3)}
        for case, data in cases.items():
            with self.subTest(case), self.assertRaisesRegex(
                    WrongElfType, r'^a\.elf: expected a little-endian SH relocatable or executable$'):
                ElfTables('a.elf', data)
        # a file without the ELF magic is no ELF at all, however long
        for data in (bytes(51), bytes(52), bytes(4096)):
            with self.subTest(size=len(data)), self.assertRaisesRegex(
                    ValueError, r'^a\.elf: not an ELF file$') as caught:
                ElfTables('a.elf', data)
            self.assertNotIsInstance(caught.exception, WrongElfType)
        with self.assertRaisesRegex(WrongElfType, r'^a\.elf: expected ELF type 2, got 1$'):
            ElfTables('a.elf', elf(), ET_EXEC)
        # a caller that catches ValueError alone catches these too
        self.assertTrue(issubclass(WrongElfType, ValueError))

    def test_refuses_a_file_whose_tables_dont_hold_together(self):
        data = elf()
        cases = {
            'a header cut short': (data[:51], 'not an ELF file'),
            'a header of another size': (put(data, EHSIZE, 64), 'invalid section directory'),
            'no section': (put(data, SHNUM, 0), 'invalid section directory'),
            'section headers of another size': (put(data, SHENTSIZE, 44), 'invalid section directory'),
            'names past the last section': (put(data, SHSTRNDX, 6), 'invalid section directory'),
            'section headers past the end': (data[:-1], 'truncated ELF at 0x'),
            'names in no string table': (put(data, section(data, NAMES, 'sh_type'), SHT_PROGBITS),
                                         'missing section names'),
            'a name without its null': (put(data, section(data, NAMES, 'sh_size'), 2), 'invalid ELF string'),
            'no symbol table': (put(data, section(data, SYMTAB, 'sh_type'), SHT_PROGBITS), 'no symbol table'),
            'two symbol tables': (put(data, section(data, STRTAB, 'sh_type'), SHT_SYMTAB),
                                  'expected one static symbol table'),
            'symbols of another size': (put(data, section(data, SYMTAB, 'sh_entsize'), 24),
                                        'expected one static symbol table'),
            'a symbol cut short': (put(data, section(data, SYMTAB, 'sh_size'), (len(SYMBOLS) + 1) * 16 - 1),
                                   'expected one static symbol table'),
            'an empty symbol table': (put(data, section(data, SYMTAB, 'sh_size'), 0),
                                      'expected one static symbol table'),
            'symbol names past the last section': (put(data, section(data, SYMTAB, 'sh_link'), 6),
                                                   'missing symbol names'),
            'symbol names in no string table': (put(data, section(data, SYMTAB, 'sh_link'), TEXT),
                                                'missing symbol names'),
            'symbols past the end': (put(data, section(data, SYMTAB, 'sh_offset'), len(data) - 16), 'truncated ELF at')}
        for case, (mutated, problem) in cases.items():
            with self.subTest(case), self.assertRaisesRegex(ValueError, rf'^a\.elf: {problem}') as caught:
                ElfTables('a.elf', mutated)
            self.assertNotIsInstance(caught.exception, WrongElfType)


class Symbols(unittest.TestCase):
    def test_refuses_a_symbol_in_a_section_past_the_last(self):
        # index 6, one past the file's last section
        tables = ElfTables('a.elf', elf([('_g', 0, 0, GLOBAL_OBJECT, 6)]))
        with self.assertRaisesRegex(ValueError, r'^a\.elf: invalid symbol section index 6$'):
            list(tables.symbols())

    def test_refuses_a_symbol_whose_section_index_lies_in_an_extended_table(self):
        # SHN_XINDEX, beside the absolute and common indices that pass
        tables = ElfTables('a.elf', elf([('_g', 0, 0, GLOBAL_OBJECT, SHN_XINDEX)]))
        with self.assertRaisesRegex(ValueError, r'^a\.elf: invalid symbol section index 65535$'):
            list(tables.symbols())

    def test_refuses_a_name_without_its_null(self):
        # the strings cut to their null and _g's underscore, and cut to nothing, which holds no name at _g's offset 1
        data = elf([('_g', 0, 0, GLOBAL_OBJECT, BSS)])
        for size in (2, 0):
            tables = ElfTables('a.elf', put(data, section(data, STRTAB, 'sh_size'), size))
            with self.subTest(size=size), self.assertRaisesRegex(ValueError, r'^a\.elf: invalid ELF string$'):
                list(tables.symbols())

    def test_reads_a_table_of_the_null_symbol_alone_without_names(self):
        # a string table of no bytes, where the null symbol's name lies at offset 0
        data = elf([])
        tables = ElfTables('a.elf', put(data, section(data, STRTAB, 'sh_size'), 0))
        self.assertEqual(list(tables.symbols()), [('', 0, 0, 0, 0)])
