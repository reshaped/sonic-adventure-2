"""The polygon list declarations name each datum of a body source that a chunk model's plist points into, and make
dc's GCC starts each such datum on 4 bytes once it has read them after the source."""

import subprocess
import unittest
from pathlib import Path

import polygon_lists
from sh_elf import ET_REL, ElfTables
from support import ROOT, SH_ELF, TreeTest, gcc_flags

SOURCE = Path('sdk/sample/provider.c')
# a material chunk, NJD_CM_D, and the end
MATERIAL = '17, 2, 0x1234, 0x5678, 255'
# each body of the provider, in its order: its declaration and its initializer. lead ends 2 bytes past a multiple
# of 4, as each list does, so GCC starts the datum after one there unless a declaration aligns it
BODIES = {
    'lead': ('static Sint16 lead[1]', '0'),
    'flat': ('static Sint16 flat[5]', MATERIAL),
    'record': ('static struct parts record', '{17, 2, 0x1234, 0x5678}, 255'),
    'bank': ('Sint16 bank[11]', f'17, 2, 0, 0, 255, 0, {MATERIAL}'),
    'even': ('static Sint16 even[2]', '0, 0'),
    'plain': ('static Sint16 plain[5]', MATERIAL),
    'model': ('NJS_CNK_MODEL model', '0, flat, {0, 0, 0}, 0'),
    'more': ('NJS_CNK_MODEL more[2]', '{0, (Sint16 *)&record, {0, 0, 0}, 0}, {0, &bank[6], {0, 0, 0}, 0}'),
}
TYPES = ('typedef long Sint32;\ntypedef short Sint16;\n'
         'typedef struct { Sint32 *vlist; Sint16 *plist; float center[3]; float r; } NJS_CNK_MODEL;\n'
         'struct parts { Sint16 material[4]; Sint16 end; };\n')
# model's plist points at flat, and more's at record's start and 12 bytes into bank. No plist points at plain
BINDINGS = 'pointer model 0x00000004 flat\npointer more 0x00000004 record\npointer more 0x0000001c bank+0xc\n'


class PolygonLists(TreeTest):
    def setUp(self):
        super().setUp()
        self.write(SOURCE, TYPES + ''.join(f'{declaration} = {{\n#include "sample/provider/{name}.inc"\n}};\n'
                                           for name, (declaration, _) in BODIES.items()))
        self.write('offsets/sdk/sample/provider.txt', '')
        self.write('offsets/sdk/sample/provider.bindings', BINDINGS)

    def test_finds_each_list_a_models_plist_points_into(self):
        # a flat array, a struct of a list's parts, and a list 12 bytes into its datum
        self.assertEqual(polygon_lists.polygon_lists(SOURCE), [('bank', 22, 12), ('flat', 10, 0), ('record', 10, 0)])

    def test_declares_each_datum_once_by_its_own_type(self):
        self.write('offsets/sdk/sample/provider.bindings', BINDINGS + 'pointer more 0x00000008 bank\n')
        polygon_lists.main([str(SOURCE), 'out/provider.lists.c'])
        self.assertEqual(Path('out/provider.lists.c').read_text(),
                         'extern __typeof__(bank) bank __attribute__((aligned(4)));\n'
                         'extern __typeof__(flat) flat __attribute__((aligned(4)));\n'
                         'extern __typeof__(record) record __attribute__((aligned(4)));\n')

    def test_a_source_whose_models_reach_no_list_declares_none(self):
        self.write('offsets/sdk/sample/provider.bindings', '')
        self.assertEqual(polygon_lists.declarations_text(SOURCE), '')

    def test_fails_on_a_list_in_a_functions_block(self):
        self.write(SOURCE, TYPES + 'void draw(void)\n{\n    static Sint16 inner[5] = {\n'
                   '#include "sample/provider/inner.inc"\n    };\n}\n'
                   'NJS_CNK_MODEL model = {\n#include "sample/provider/model.inc"\n};\n')
        self.write('offsets/sdk/sample/provider.bindings', 'pointer model 0x00000004 inner\n')
        self.assertIn("sdk/sample/provider.c: inner holds a polygon list, and a function's block declares it",
                      self.fails(polygon_lists.polygon_lists, SOURCE))

    @unittest.skipUnless((SH_ELF / 'bin/sh-elf-gcc').is_file(), 'requires SH-4 GCC')
    def test_gcc_starts_each_declared_datum_on_4_bytes_and_no_other(self):
        for name, (_, values) in BODIES.items():
            self.write(f'gen/sample/provider/{name}.inc', values + '\n')
        polygon_lists.main([str(SOURCE), 'provider.lists.c'])
        # as a source of GCC_DATA compiles, which keeps its data in one section, in the source's order
        flags = [*gcc_flags(SH_ELF), '-Os', '-fno-toplevel-reorder', '-Igen', '-c']

        def placed(*unit):
            subprocess.run([SH_ELF / 'bin/sh-elf-gcc', *flags, '-o', 'provider.o', *unit], check=True)
            found = {name: value for name, value, _, _, _ in
                     ElfTables('provider.o', Path('provider.o').read_bytes(), ET_REL).symbols()}
            return [found['_' + name] for name in ('lead', 'flat', 'record', 'bank', 'even', 'plain')]

        # GCC alone starts flat and bank 2 bytes past a multiple of 4
        self.assertEqual(placed(SOURCE), [0, 2, 12, 22, 44, 48])
        # after the declarations, flat, record and bank start on one. plain, which no plist reaches, keeps its
        # type's alignment, and starts 2 bytes past one
        self.assertEqual(placed('-include', SOURCE, 'provider.lists.c'), [0, 4, 16, 28, 50, 54])


class PlayRecipe(unittest.TestCase):
    def test_a_play_source_reaches_the_float_pass_as_itself(self):
        # the float pass leaves the lines of a file that -include names as they are, so play preprocesses the
        # source itself, and its declarations follow the text the pass has rewritten
        text = (ROOT / 'dc.mk').read_text()
        start = text.index('\nplay = ')
        play = text[start:text.index('\n\n', start)]
        self.assertNotIn('unit', play)
        self.assertLess(play.index('tools/floats.py'), play.index('$(lists_follow)'))
        self.assertLess(play.index('$(lists_follow)'), play.index(' -c -o $@'))
        self.assertIn('lists_follow = $(if $(dc_lists),&& cat $(dc_lists) >> $(@:.o=.i))', text)


if __name__ == '__main__':
    unittest.main()
