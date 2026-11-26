"""The address census: each number for RAM that the port's sources, tables
and generated data hold, held to a row of config/addresses.txt."""

import struct
from unittest import mock

import addresses
import content
from body_data import float_literal
from support import TreeTest, run

HEAD = '# file  start  end  count  disposition, then the reason\n'
# the main executable, a stage and CHAO.PRS with one of its areas, each
# 0x100 bytes from its base
IMAGES = {'1st_read': 'file 1ST_READ.BIN\nP 0x8c010000 0x8c010100\n',
          'stg13': 'file SONIC2/STG13.PRS\nP 0x8c500000 0x8c500100\n',
          'chao': 'file SONIC2/CHAO.PRS\nP 0x0c500000 0x0c500100\n',
          'chaostgrace': 'file SONIC2/CHAOSTGRACE.PRS\nP 0x0c9c0000 0x0c9c0100\n'}
# a stage's disc data: two Sint32, which may hold an address, and four
# halfwords of a polygon list, which kinds.NUMBERS says hold none
DATA = '0x8c500100  .numbers  sint32  2\n0x8c500108  .polygons  chunk_polygons  4\n'


class Census(TreeTest):
    def setUp(self):
        super().setUp()
        for name, splits in IMAGES.items():
            self.image(name, splits)
        self.write('bodies.txt', '')
        self.table('')

    def table(self, rows):
        self.write('config/addresses.txt', HEAD + rows)

    def census(self, *argv):
        """What the census fails with, given argv after --bodies bodies.txt,
        or '' where it passes."""
        status, output = run(addresses.main, '--bodies', 'bodies.txt', *argv)
        self.assertEqual(status != 0, bool(output), output)
        return output

    def test_fails_on_a_number_for_ram_that_no_row_explains_and_passes_once_one_does(self):
        self.write('src/stage/areas.h', '#define SLOT ((Uint8 *)0x8c500000)\n')
        self.assertIn('src/stage/areas.h:1: 0x8c500000, which no row of config/addresses.txt explains',
                      self.census())
        self.table('src/stage/areas.h  0x8c500000  1  anchor  SLOT  the slot\n')
        self.assertEqual(self.census(), '')

    def test_reads_each_view_of_ram_and_no_other_number(self):
        self.write('src/a.c', 'int a[] = {0x0c000000, 0x8cffffff, 0xac000004U, 0x0bffffff, 0x0d000000, '
                              '0xa05f7800, 0x8d000000, 201326592};\n')
        self.table('src/a.c  0x0c000000  0x0d000000  2  data  two numbers\n'
                   'src/a.c  0x8cffffff  1  data  one\n'
                   'src/a.c  0xac000004  1  data  one\n')
        self.assertEqual(self.census(), '')

    def test_reads_both_sides_of_a_guard_and_no_comment_or_string(self):
        self.write('modules/stg13/a.c', '#ifdef NONMATCHING\n#define AREA 0x8c600000\n#else\n#define AREA 0x8c700000\n'
                                        '#endif\n/* 0x8c800000 */\nchar *s = "0x8c900000";\n')
        self.table('modules/stg13/a.c  0x8c600000  0x8c800000  2  anchor  AREA  the area on each side\n')
        self.assertEqual(self.census(), '')

    def test_holds_a_rows_count_to_the_numbers_it_explains(self):
        self.write('src/a.c', 'int a = 0x8c500000;\nint b = 0x8c500004;\n')
        self.table('src/a.c  0x8c500000  0x8c500100  3  data  numbers\n')
        self.assertIn('config/addresses.txt:2: the row counts 3 of the numbers it spans, and src/a.c holds 2',
                      self.census())
        self.table('src/a.c  0x8c500000  0x8c500100  2  data  numbers\nsrc/a.c  0x8c600000  1  data  gone\n')
        self.assertIn('config/addresses.txt:3: src/a.c holds no number the row spans', self.census())

    def test_reads_assembly_numbers_outside_comments_and_strings(self):
        self.write('sdk/shinobi/a.src', "VECTOR: .EQU H'8C0000BC\n        MOV #12,R0 ; H'8C000000\n"
                                        '        .SDATA "H\'8C000000"\n')
        self.assertIn("sdk/shinobi/a.src:1: 0x8c0000bc, which no row", self.census())
        self.table('sdk/shinobi/a.src  0x8c0000bc  1  anchor  VECTOR  the boot ROM\'s word\n')
        self.assertEqual(self.census(), '')

    def test_reads_an_inline_body_and_an_asm_block_as_assembly(self):
        self.write('src/a.c', '#pragma inline_asm(twice)\nint twice(int x)\n{\n        RTS ; at 0x8c033000\n'
                              "        ADD R4,R4 ; H'8C000000\n}\n#pragma asm\nWORD: .EQU H'8C0080F0\n"
                              '; at 0x8c014000\n#pragma endasm\n')
        self.assertIn('src/a.c:8: 0x8c0080f0, which no row', self.census())
        self.table("src/a.c  0x8c0080f0  1  anchor  WORD  the app root record\n")
        self.assertEqual(self.census(), '')

    def test_reads_a_sum_of_a_macro_and_an_offset_in_bytes(self):
        self.write('src/system/areas.h', '#define WORK_AREA ((Uint8 *)0x8c300000)\n'
                                         '#define WORK_AREA_FILE (WORK_AREA + 0x100000)\n')
        self.write('src/a.c', '#include "system/areas.h"\nint *a = (int *)(WORK_AREA - 0x10);\n')
        self.table('src/system/areas.h  0x8c300000  1  anchor  WORK_AREA  the area\n')
        output = self.census()
        self.assertIn('src/system/areas.h:2: 0x8c400000, which no row', output)
        self.assertIn('src/a.c:2: 0x8c2ffff0, which no row', output)
        # a cast to a pointer to a byte takes a pointer's number whole
        self.write('src/b.c', '#define WORDS ((Uint32 *)0x8c500000)\nUint8 *b = (Uint8 *)WORDS + 4;\n')
        self.assertIn('src/b.c:2: 0x8c500004, which no row', self.census())

    def test_takes_no_value_for_a_sum_it_cant_count_in_bytes(self):
        for name, text, problem in (
                ('words', '#define AREA ((Uint32 *)0x8c300000)\nUint32 *a = AREA + 4;\n',
                 'AREA there is neither an integer nor a pointer to a byte'),
                ('operand', '#define AREA 0x8c300000\nint a = AREA + 4 * 2;\n',
                 'the * beside it makes it no whole operand'),
                ('two values', '#define AREA 0x8c300000\n#undef AREA\n#define AREA 0x8c400000\nint a = AREA + 4;\n',
                 'define AREA as more than one number for RAM'),
                ('narrowed', '#define AREA ((unsigned short)0x8c300000)\nunsigned a = (unsigned)AREA + 4;\n',
                 'AREA there is neither an integer nor a pointer to a byte'),
                ('long long', '#define AREA 0x8c300000\nlong long a = (long long)AREA + 4;\n',
                 'AREA there is neither an integer nor a pointer to a byte'),
                ('a number and more', '#define AREA 0x8c300000\n#undef AREA\n#define AREA (x)\nint a = AREA + 4;\n',
                 'or as one and as something else')):
            with self.subTest(name):
                self.write('src/a.c', text)
                self.assertIn('takes no value for it', self.fails(addresses.main, ['--bodies', 'bodies.txt']))
                self.assertIn(problem, self.fails(addresses.main, ['--bodies', 'bodies.txt']))

    def test_takes_a_sum_in_32_bits_as_the_sh4_adds(self):
        self.write('src/a.c', '#define BASE 0x8c000000u\nunsigned a = BASE + 0x80000000u;\n')
        self.table('src/a.c  0x8c000000  1  anchor  BASE  the base\n')
        self.assertIn('src/a.c:2: 0x0c000000, which no row', self.census())
        # an offset wider than 32 bits is no integer constant the check takes, so it makes no sum
        self.write('src/a.c', '#define BASE 0x8c000000u\nunsigned long long a = BASE + 0x80000000ULL;\n')
        self.assertEqual(self.census(), '')

    def test_takes_a_macro_from_the_files_a_sum_reaches_and_from_no_other(self):
        # other/ lies outside SOURCE_ROOTS, so the check reads none of its numbers but those of a sum
        self.write('other/base.h', '#define BASE 0x8c001000\n')
        self.write('src/a.c', '#include "other/base.h"\nint a = BASE + 4;\n')
        self.assertIn('src/a.c:2: 0x8c001004, which no row', self.census())
        # index is a number for RAM in b.c alone, and a parameter in c.c
        self.write('src/a.c', 'int a;\n')
        self.write('src/b.c', '#define index 0x8c000000\n')
        self.write('src/c.c', 'int f(int index) { return index + 1; }\n')
        self.table('src/b.c  0x8c000000  1  anchor  index  the base\n')
        self.assertEqual(self.census(), '')

    def test_spells_no_name_of_an_assembly_comment_in_c(self):
        self.write('src/a.c', "#pragma asm\nWORD: .EQU H'8C0080F0\n        MOV R0,R1 ; OldAnchor\n#pragma endasm\n")
        self.table('src/a.c  0x8c0080f0  1  anchor  OldAnchor  the app root record\n')
        self.assertIn('config/addresses.txt:2: OldAnchor: a name no source spells', self.census())
        self.table('src/a.c  0x8c0080f0  1  anchor  WORD  the app root record\n')
        self.assertEqual(self.census(), '')

    def test_reads_no_address_outside_ram_of_a_symbols_txt(self):
        self.write('config/1st_read/symbols.txt', '0x8c010000 main\n0xa05f8000 holly\n')
        self.assertEqual(self.census(), '')

    def test_reads_each_name_a_table_gives_outside_its_image_and_those_beside_it(self):
        # stage_slot lies in no image, the work past STG13 in none, a name of the main executable's lies in it,
        # and a Chao area's name of CHAO's lies in CHAO
        self.write('config/1st_read/symbols.txt', '0x8c010000 main\n0x8c500000 stage_slot\n')
        self.write('config/stg13/symbols.txt', '0x8c010040 effect_texlist\n0x8c500000 stage\n0x8c500100 work\n')
        self.write('config/chaostgrace/symbols.txt', '0x0c500010 garden\n0x0c8b9208 race_omo_model\n')
        output = self.census()
        self.assertIn('config/1st_read/symbols.txt:2: 0x8c500000, which no row', output)
        self.assertIn('config/stg13/symbols.txt:3: 0x8c500100, which no row', output)
        self.assertIn('config/chaostgrace/symbols.txt:2: 0x0c8b9208, which no row', output)
        self.assertEqual(len(output.splitlines()), 3, output)
        # the names the non-matching side takes in their places
        self.write('src/system/areas.h', 'extern char end[];\n#define STAGE_SLOT stage\n'
                                         '#define CHAO_MODEL_SLOT models\n')
        self.table('config/1st_read/symbols.txt  0x8c500000  1  anchor  STAGE_SLOT  stage_slot\n'
                   'config/stg13/symbols.txt  0x8c500100  1  placed  end  the workspace past the image\n'
                   'config/chaostgrace/symbols.txt  0x0c698700  0x0c8ba394  1  anchor  CHAO_MODEL_SLOT  a model\n')
        self.assertEqual(self.census(), '')

    def test_reads_each_body_the_list_names_and_counts_its_rows_alone(self):
        body = 'build/stub/src/a/table.inc'
        self.write('src/a.c', 'unsigned long table[6] = {\n#include "a/table.inc"\n};\n')
        self.write(body, '1.8e-5F, 0x8c500000, -1940914176, 201326592U, 2898715097U, "0x8c000000"\n')
        self.write('bodies.txt', f'src/a.c {body}\n')
        self.table(f'{body}  0x8c500000  2  data  two words\n{body}  0x0c000000  1  data  a word\n'
                   f'{body}  0xac000000  0xad000000  1  data  a color\n'
                   'build/gen/src/a/table.inc  0x8c500000  1  data  a row of the full build\n')
        self.assertEqual(self.census(), '')
        self.table(f'{body}  0x8c500000  2  data  two words\n{body}  0x0c000000  1  data  a word\n'
                   f'{body}  0xac000000  0xad000000  1  data  a color\n'
                   'build/stub/src/b/gone.inc  0x8c500000  1  data  a body the build no longer writes\n')
        self.assertIn('config/addresses.txt:5: build/stub/src/b/gone.inc: the check reads no such file', self.census())
        self.write('bodies.txt', f'{body}\n')
        self.assertIn(f'bodies.txt: {body} comes before any source', self.census())

    def test_reads_no_float_field_and_no_list_a_model_reaches_but_an_integer_of_the_same_bits(self):
        # 0x0c430029 is the head of boss_bigfoot's body_vertices, an NJD_CV_VN chunk of 0xc43 words. It opens
        # vertices, which model's vlist reaches, and plain, which nothing reaches. model's center[0] holds the
        # same bits as a float. Its r is the float 201326750.0F, whose digits read as 0x0c00009e
        bits = 0x0c430029
        self.write('sdk/sample/provider.c', 'typedef long Sint32;\ntypedef short Sint16;\n'
                   'typedef struct { Sint32 *vlist; Sint16 *plist; float center[3]; float r; } NJS_CNK_MODEL;\n'
                   'static Sint32 vertices[3] = {\n#include "sample/provider/vertices.inc"\n};\n'
                   'static Sint32 plain[3] = {\n#include "sample/provider/plain.inc"\n};\n'
                   'static NJS_CNK_MODEL model = {\n#include "sample/provider/model.inc"\n};\n')
        self.write('offsets/sdk/sample/provider.bindings', 'pointer model 0x00000000 vertices\n')
        root = 'build/gen/sdk/sample/provider'
        self.write(f'{root}/vertices.inc', f'    {bits}, 34275328,\n    255\n')
        self.write(f'{root}/plain.inc', f'    {bits}, 34275328, 255\n')
        self.write(f'{root}/model.inc', f'    vertices, 0, {{{float_literal(bits)}, 0.0F, 0.0F}}, '
                                        f'{float_literal(0x4d40000a)}\n')
        self.write('bodies.txt', f'sdk/sample/provider.c {root}/vertices.inc {root}/plain.inc {root}/model.inc\n')
        output = self.census()
        self.assertIn(f'{root}/plain.inc:1: 0x0c430029, which no row', output)
        self.assertEqual(len(output.splitlines()), 1, output)
        self.table(f'{root}/plain.inc  0x0c430029  1  data  a word\n{root}/vertices.inc  0x0c430029  1  data  a head\n')
        self.assertIn(f'config/addresses.txt:3: {root}/vertices.inc holds no number the row spans', self.census())

    def test_excuses_a_row_of_the_other_runs_bodies_alone(self):
        body = 'build/stub/src/a/table.inc'
        self.write('src/a.c', 'unsigned long table[1] = {\n#include "a/table.inc"\n};\n')
        self.write(body, '0x8c500000\n')
        self.write('bodies.txt', f'src/a.c {body}\n')
        self.table(f'{body}  0x8c500000  1  data  a word\n'
                   'build/gen/src/a/table.inc  0x8c500000  1  data  a row of the full build\n'
                   'build/gne/src/a/table.inc  0x8c500000  1  data  a row of no build\n')
        output = self.census()
        self.assertIn('config/addresses.txt:4: build/gne/src/a/table.inc: the check reads no such file', output)
        self.assertEqual(len(output.splitlines()), 1, output)
        # a LIST that names no body names no run, so neither run's rows are excused
        self.write('bodies.txt', '')
        self.table(f'{body}  0x8c500000  1  data  a word\n')
        self.assertIn(f'config/addresses.txt:2: {body}: the check reads no such file', self.census())

    def test_fails_on_a_name_no_source_spells(self):
        self.write('src/a.c', '#define SLOT ((char *)0x8c500000)\n')
        self.write('sdk/a.src', "HOOK: .EQU H'AC004000\n")
        self.table('src/a.c  0x8c500000  1  anchor  SLOT  the slot\nsdk/a.src  0xac004000  1  anchor  HOOK  a hook\n')
        self.assertEqual(self.census(), '')
        self.table('src/a.c  0x8c500000  1  anchor  STAGE_SLOT  the slot\n'
                   'sdk/a.src  0xac004000  1  anchor  HOOK  a hook\n')
        self.assertIn('config/addresses.txt:2: STAGE_SLOT: a name no source spells', self.census())

    def test_fails_on_a_row_of_a_file_it_doesnt_read(self):
        self.table('src/gone.c  0x8c500000  1  data  a source since removed\n'
                   'config/stg13/data.txt  0x8c500000  1  data  the disc data, which only make dc reads\n')
        output = self.census()
        self.assertIn('config/addresses.txt:2: src/gone.c: the check reads no such file', output)
        self.assertNotIn('data.txt', output)

    def stage_data(self, numbers, polygons):
        """Write stg13's disc data, its original image and the content
        tools/content.py assembles of it."""
        self.image('stg13', IMAGES['stg13'] + 'D 0x8c500100 0x8c500110\ndisc D 0x8c500100 0x8c500110\n')
        self.write('config/stg13/data.txt', DATA)
        self.write('image.bin', bytes(0x100) + struct.pack('<2I', *numbers) + struct.pack('<4H', *polygons))
        content.main(['stg13', 'image.bin', 'build/dc/content/stg13.s'])

    def test_reads_each_word_content_keeps_of_a_kind_that_may_hold_an_address(self):
        # the second Sint32 holds an address of STG13, and the polygon list a pair of halfwords that reads as one
        self.stage_data((1, 0x8c500004), (0x0004, 0x8c50, 0x0010, 0x0c00))
        self.assertIn('config/stg13/data.txt:1: .numbers+0x4, at 0x8c500104: 0x8c500004, which no row',
                      self.census('build/dc/content/stg13.s'))
        self.table('config/stg13/data.txt  0x8c500004  1  data  a count\n')
        self.assertEqual(self.census('build/dc/content/stg13.s'), '')
        # without the content, as make dc-check runs it, the census counts no row of a data.txt
        self.assertEqual(self.census(), '')

    def handmade(self, *lines):
        """Write stg13's disc data and original image as stage_data() does,
        with build/dc/content/stg13.s of lines alone. The original holds a
        word for RAM at 0x8c5000fc, before the first datum, and the second
        Sint32 one at 0x8c500104."""
        self.stage_data((1, 0x8c500004), (0, 0, 0, 0))
        self.write('image.bin', bytes(0xfc) + struct.pack('<4I', 0x8c500008, 1, 0x8c500004, 0) + bytes(8))
        self.write('build/dc/content/stg13.s', ''.join(line + '\n' for line in lines))

    def test_reads_an_indented_incbin_and_fails_on_one_it_cant(self):
        self.handmade('\t.incbin "image.bin", 260, 4')
        self.assertIn('.numbers+0x4, at 0x8c500104: 0x8c500004, which no row', self.census('build/dc/content/stg13.s'))
        self.handmade('.incbin\t"image.bin", 260, 4')
        self.assertIn('build/dc/content/stg13.s:1: an .incbin out of the form content.py writes',
                      self.census('build/dc/content/stg13.s'))

    def test_fails_on_a_word_no_datum_holds(self):
        # the word before the first datum would otherwise take the last's kind, a polygon list's, and go unread
        self.handmade('.incbin "image.bin", 252, 4')
        self.assertIn('build/dc/content/stg13.s:1: the word at 0x8c5000fc lies in no datum of stg13',
                      self.census('build/dc/content/stg13.s'))

    def test_loads_each_original_once(self):
        self.handmade('.incbin "image.bin", 256, 4', '.incbin "image.bin", 260, 4')
        self.table('config/stg13/data.txt  0x8c500004  1  data  a count\n')
        with mock.patch.object(addresses, 'content', wraps=addresses.content) as loaded:
            self.assertEqual(self.census('build/dc/content/stg13.s'), '')
        self.assertEqual([call.args for call in loaded.call_args_list].count(('image.bin',)), 1)

    def test_fails_on_a_row_out_of_form(self):
        for name, row, problem in (
                ('short', 'src/a.c  0x8c500000  data\n', 'not a row: FILE START [END] COUNT'),
                ('count', 'src/a.c  0x8c500000  01  data  x\n', 'not a row'),
                ('nameless', 'src/a.c  0x8c500000  1  anchor\n', 'not a row'),
                ('two views', 'src/a.c  0x0cff0000  0x8c000000  1  data  x\n', 'not a span of RAM, in one view'),
                ('no ram', 'src/a.c  0x0d000000  1  data  x\n', 'not a span of RAM, in one view'),
                ('overlap', 'src/a.c  0x8c500000  0x8c500100  1  data  x\nsrc/a.c  0x8c5000f0  1  data  y\n',
                 'config/addresses.txt:3: its numbers meet those of the row at config/addresses.txt:2')):
            with self.subTest(name):
                self.table(row)
                self.assertIn(problem, self.fails(addresses.table))
