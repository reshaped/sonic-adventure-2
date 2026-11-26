"""The colors check names each material chunk whose colors GCC's link puts 2 bytes past a multiple of 4, in
content's lists and in each list a body's chunk model points into, and fails on one."""

import struct
from pathlib import Path

import chunk_colors
from config import Image
from support import TreeTest

LOCAL, GLOBAL = 0, 1
OBJECT, FUNCTION = 1, 2
LINKED = 0x8c700000
# a material chunk, NJD_CM_D, and the end
MATERIAL = [17, 2, 0x1234, 0x5678, 255]


def executable(data, symbols):
    """An SH-4 executable's bytes: one .data section of data at LINKED, and
    symbols as (name, binding, address, size) entries in it, each an object
    unless a fifth element gives its type."""
    names = b'\0.data\0.symtab\0.strtab\0.shstrtab\0'
    strings, entries = b'\0', [bytes(16)]
    for name, binding, address, size, *kind in symbols:
        entries.append(struct.pack('<IIIBBH', len(strings), address, size, binding << 4 | (kind or [OBJECT])[0], 0, 1))
        strings += name.encode() + b'\0'
    symtab = b''.join(entries)
    body = data + symtab + strings + names
    at = [52, 52 + len(data), 52 + len(data) + len(symtab), 52 + len(data) + len(symtab) + len(strings)]
    sections = [bytes(40),
                struct.pack('<10I', 1, 1, 3, LINKED, at[0], len(data), 0, 0, 4, 0),
                struct.pack('<10I', 7, 2, 0, 0, at[1], len(symtab), 3, 1, 4, 16),
                struct.pack('<10I', 15, 3, 0, 0, at[2], len(strings), 0, 0, 1, 0),
                struct.pack('<10I', 23, 3, 0, 0, at[3], len(names), 0, 0, 1, 0)]
    header = struct.pack('<16sHHIIIIIHHHHHH', b'\x7fELF\x01\x01\x01' + bytes(9), 2, 42, 1, 0, 0, 52 + len(body), 0,
                         52, 0, 0, 40, len(sections), 4)
    return header + body + b''.join(sections)


class ChunkColors(TreeTest):
    def setUp(self):
        super().setUp()
        self.image('stage', 'file STAGE.BIN\nP 0x8c500000 0x8c500004\nD 0x8c500004 0x8c500040\n'
                   'disc D 0x8c500004 0x8c500018\nsdk/sample/provider.c D 0x8c500018 0x8c500040\n',
                   '0x8c500004 lights\n')
        # content's .data holds lights at 0 and .shade at 10, whose colors fall at 14
        self.write('config/stage/data.txt', '0x8c500004 lights chunk_polygons 5\n0x8c50000e .shade chunk_polygons 5\n')
        self.write('sdk/sample/provider.c', 'typedef long Sint32;\ntypedef short Sint16;\n'
                   'typedef struct { Sint32 *vlist; Sint16 *plist; float center[3]; float r; } NJS_CNK_MODEL;\n'
                   'struct parts { Sint16 material[4]; Sint16 end; };\n'
                   'static Sint16 flat[5] = {\n#include "sample/provider/flat.inc"\n};\n'
                   'static Sint16 plain[5] = {\n#include "sample/provider/plain.inc"\n};\n'
                   'static struct parts record = {\n#include "sample/provider/record.inc"\n};\n'
                   'Sint16 bank[11] = {\n#include "sample/provider/bank.inc"\n};\n'
                   'NJS_CNK_MODEL model = {\n#include "sample/provider/model.inc"\n};\n'
                   'NJS_CNK_MODEL more[2] = {\n#include "sample/provider/more.inc"\n};\n')
        self.write('offsets/sdk/sample/provider.txt', '')
        # more's plists point at record's start and 12 bytes into bank
        self.write('offsets/sdk/sample/provider.bindings', 'pointer model 0x00000004 flat\n'
                   'pointer more 0x00000004 record\npointer more 0x0000001c bank+0xc\n')
        self.lists = struct.pack('<10H', *MATERIAL, *MATERIAL)

    def report(self, symbols, data=None):
        self.write('image.elf', executable(self.lists.ljust(0x40, b'\0') if data is None else data, symbols))
        return chunk_colors.report(Image('stage'), 'image.elf')

    def test_names_content_and_body_lists_whose_colors_arent_4_byte_aligned(self):
        # flat lies 2 bytes past a multiple of 4; plain, which no model reaches, a flat of another size and a function
        # flat are no lists
        data = (self.lists + bytes(2) + struct.pack('<5H', *MATERIAL)).ljust(0x40, b'\0')
        lines = self.report([('_lights', GLOBAL, LINKED, 10), ('_flat', LOCAL, LINKED + 22, 10),
                             ('_flat', LOCAL, LINKED + 32, 4), ('_plain', LOCAL, LINKED + 22, 10),
                             ('_flat', LOCAL, LINKED + 0x30, 10, FUNCTION)], data)
        self.assertEqual(lines, (3, [".shade+0: chunk type 17's colors at 0x8c70000e\n",
                                     "flat+0: chunk type 17's colors at 0x8c70001a\n"]))

    def test_reads_a_struct_of_a_lists_parts_and_a_list_inside_its_datum(self):
        # record lies 2 bytes past a multiple of 4. bank lies on one, and its list starts 12 bytes in, after a list
        # that no model points at, and ends at its NJD_CE, before the bank's last element
        data = (self.lists + bytes(2) + struct.pack('<5H', *MATERIAL) + struct.pack('<11H', 17, 2, 0, 0, 255, 0,
                                                                                    *MATERIAL)).ljust(0x40, b'\0')
        symbols = [('_lights', GLOBAL, LINKED, 10), ('_record', LOCAL, LINKED + 22, 10),
                   ('_bank', GLOBAL, LINKED + 32, 22)]
        self.assertEqual(self.report(symbols, data), (4, [".shade+0: chunk type 17's colors at 0x8c70000e\n",
                                                          "record+0: chunk type 17's colors at 0x8c70001a\n"]))
        # the same bank 2 bytes on: the chunk 12 bytes into it has its colors off a multiple of 4
        moved = [('_lights', GLOBAL, LINKED, 10), ('_bank', GLOBAL, LINKED + 34, 22)]
        self.assertEqual(self.report(moved, data[:32] + bytes(2) + data[32:54] + bytes(8))[1][1:],
                         ["bank+12: chunk type 17's colors at 0x8c700032\n"])

    def test_a_bodys_list_must_reach_its_end_in_its_datum(self):
        data = (self.lists + bytes(2) + struct.pack('<5H', 17, 2, 0, 0, 0)).ljust(0x40, b'\0')
        self.assertIn('flat at 0x8c700016: no NJD_CE by byte 10',
                      self.fails(self.report, [('_lights', GLOBAL, LINKED, 10), ('_flat', LOCAL, LINKED + 22, 10)],
                                 data))

    def test_fails_on_a_chunk_it_names_and_counts_the_chunks_of_an_image_that_has_none(self):
        self.write('image.elf', executable(self.lists.ljust(0x40, b'\0'), [('_lights', GLOBAL, LINKED, 10)]))
        said = self.fails(chunk_colors.main, ['stage', 'image.elf', 'out.txt'])
        self.assertIn('image.elf: 1 of 2 material chunks hold colors off 4 bytes\n', said)
        self.assertTrue(said.endswith(".shade+0: chunk type 17's colors at 0x8c70000e"), said)
        self.assertFalse(Path('out.txt').exists())
        # .shade as an NJD_MATOFF chunk, which holds no colors
        passing = struct.pack('<10H', *MATERIAL, 16, 2, 0, 0, 255).ljust(0x40, b'\0')
        self.write('image.elf', executable(passing, [('_lights', GLOBAL, LINKED, 10)]))
        chunk_colors.main(['stage', 'image.elf', 'out.txt'])
        self.assertEqual(Path('out.txt').read_text(), '1\n')

    def test_lists_no_chunk_that_holds_no_colors(self):
        # .shade's colors would lie off a word, but an NJD_MATOFF or NJD_CM_BU chunk holds none
        for chunk_type in (16, 24):
            with self.subTest(chunk_type=chunk_type):
                self.lists = struct.pack('<10H', *MATERIAL, chunk_type, 2, 0, 0, 255)
                self.assertEqual(self.report([('_lights', GLOBAL, LINKED, 10)]), (1, []))

    def test_places_content_by_its_global_symbols_alone(self):
        # content's .data lies 2 bytes past a word, and a static of the same name at the word is another's
        data = (bytes(2) + self.lists).ljust(0x40, b'\0')
        self.assertEqual(self.report([('_lights', GLOBAL, LINKED + 2, 10), ('_lights', LOCAL, LINKED, 10)], data),
                         (2, ["lights+0: chunk type 17's colors at 0x8c700006\n"]))
        self.assertIn("no symbol places content's .data", self.fails(self.report, [('_lights', LOCAL, LINKED, 10)]))

    def test_reads_only_the_sections_the_image_loads(self):
        # .symtab, which the image doesn't load, lies at address 0
        self.report([('_lights', GLOBAL, LINKED, 10)])
        self.assertIn('no section holds 4 bytes at 0x00000000',
                      self.fails(chunk_colors.Linked('image.elf').read, 0, 4))

    def test_a_list_the_link_holds_must_walk_to_its_end(self):
        data = struct.pack('<10H', 17, 2, 0, 0, 0, *MATERIAL).ljust(0x40, b'\0')
        self.assertIn('lights at 0x8c700000: no NJD_CE by byte 10',
                      self.fails(self.report, [('_lights', GLOBAL, LINKED, 10)], data))
