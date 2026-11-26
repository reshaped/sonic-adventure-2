"""Native compiler placements are measured independently of the zero placeholder bytes."""
import os
from pathlib import Path
import struct
import unittest

import body_bindings
import body_offsets
from body_inputs import write_rules
from body_data import generate
from config import Image, rows
from support import ROOT, TreeTest, obj


def row(section, offset, code, statement):
    return f'{section:3} {offset:08X} {code:10} {statement}\n'


class BodyOffsets(TreeTest):
    source = Path('src/grid.c')
    table = Path('offsets/src/grid.txt')
    listing = Path('build/stub/obj/src/grid.lis')

    def setUp(self):
        super().setUp()
        self.image('1st_read', 'file 1ST_READ.BIN\nD 0x00001000 0x00001010\n'
                   'src/grid.c D 0x00001000 0x00001010\n')
        self.write(self.source, 'static int first[2] = {\n#include "grid/first.inc"\n};\n'
                   'static int second[2] = {\n#include "grid/second.inc"\n};\n')
        self.write(self.listing, row('D', 0, '', 'L100:  ; static: first') +
                   row('', 0, '00000000', ".DATA.L     H'00000000,H'00000000") +
                   row('', 8, '', 'L101:  ; static: second') +
                   row('', 8, '00000000', ".DATA.L     H'00000000,H'00000000") +
                   'DATA SECTION (D): 00000010 Byte(s)\n')

    def test_equal_sized_automatic_initializers_follow_the_source_line_load(self):
        # Both native templates are all zero. The first local references the later
        # label, and the pool references it from a different PC-relative displacement.
        rules = Path('build/body-inputs/sources.mk')
        write_rules(rules, [self.source])
        self.assertNotIn('body_listing.', rules.read_text())
        self.write(self.source, 'void launch(void)\n{\n    int velocity[3] = {\n'
                   '#include "grid/velocity.inc"\n    };\n    int origin[3] = {\n'
                   '#include "grid/origin.inc"\n    };\n}\n')
        write_rules(rules, [self.source])
        self.assertIn('body_listing.src/grid.c := source,object', rules.read_text().splitlines())
        source = ('          grid.c     3        int velocity[3] = {\n'
                  '          grid.c     4    #include "grid/velocity.inc"\n'
                  '          grid.c     5        };\n')
        other = ('          grid.c     6        int origin[3] = {\n'
                 '          grid.c     7    #include "grid/origin.inc"\n'
                 '          grid.c     8        };\n')
        listing = (row('P', 0, '', '_launch:  ; function: launch') +
                   source + row('', 0, 'D302', 'MOV.L       L10+4,R3; L91') +
                   other + row('', 2, 'D201', 'MOV.L       L10,R2; L88') +
                   row('', 8, '<00000000>', '.DATA.L     L88') +
                   row('', 12, '<00000000>', '.DATA.L     L91') +
                   row('C', 0, '', 'L88:') +
                   row('', 0, '00000000', ".DATA.L     H'00000000,H'00000000,H'00000000") +
                   row('', 12, '', 'L91:') +
                   row('', 12, '00000000', ".DATA.L     H'00000000,H'00000000,H'00000000") +
                   'PROGRAM  SECTION (P): 00000010 Byte(s)\nCONSTANT SECTION (C): 00000018 Byte(s)\n')
        self.write(self.listing, listing)
        measured = body_offsets.measured(self.source, self.listing)
        self.assertEqual(measured, {'velocity': ('datum', 'C', 12, 12), 'origin': ('datum', 'C', 0, 12)})
        # A stale table which swaps two valid, equal-sized ranges must disagree.
        self.write(self.table, 'datum  velocity  C  0x00000000  12\ndatum  origin  C  0x0000000c  12\n')
        self.assertNotEqual(self.table.read_bytes(), body_offsets.refreshed(self.table, measured))
        for broken in (listing.replace('D302', 'D301'), listing.replace(source, ''),
                       listing.replace('grid.c     5', 'other.c    5')):
            self.write(self.listing, broken)
            self.assertIn('0 native storage candidates', self.fails(body_offsets.measured, self.source, self.listing))

    def test_asmsh_numbered_data_continuations(self):
        self.write(self.source, 'static int panels[6] = {\n#include "grid/panels.inc"\n};\n')
        listing = ("*** SuperH RISC engine ASSEMBLER Ver. 4.0 ***\n"
                   "  100 00000000                   100     .SECTION D,DATA,ALIGN=4\n"
                   "  101 00000000                   101     L1009:              ; static: panels\n"
                   "  102 00000000 0000000000000000  102     .DATA.L _p0,_p1,_p2,_p3,\n"
                   "  103 00000008 0000000000000000  103     +       _p4,_p5\n"
                   "      00000010 0000000000000000\n"
                   "  104 00000018                   104     .RES.L 1\n"
                   "D REL-DATA 0000001C\n")
        self.write(self.listing, listing)
        self.assertEqual(body_offsets.measured(self.source, self.listing), {'panels': ('datum', 'D', 0, 24)})
        for broken in (listing.replace('103 00000008', '103 0000000C'),
                       listing.replace('+       _p4,_p5', '.ALIGN 4'),
                       listing.replace('.DATA.L _p0,_p1,_p2,_p3,', '.RES.L 2')):
            with self.subTest(broken=broken):
                self.write(self.listing, broken)
                self.assertIn('disagrees with its C type', self.fails(body_offsets.measured, self.source, self.listing))

    def test_global_label_can_end_a_listing_page(self):
        name = 'death_chamber_center_middle_model'
        self.write(self.source, f'int {name}[2] = {{\n#include "grid/{name}.inc"\n}};\n')
        header = ('\n\nSH SERIES C/C++ Compiler (Ver. 5.1(Release13))   21-Nov-2026 01:25:15  PAGE   11\n\n'
                  'SCT OFFSET   CODE       C LABEL     INSTRUCTION OPERAND    COMMENT\n\n')
        label = row('D', 0, '', f'_{name}:')
        note = f'                                                           ; static: {name}\n'
        body = row('', 0, '00000000', ".DATA.L     H'00000000,H'00000000") + 'DATA SECTION (D): 00000008 Byte(s)\n'
        self.write(self.listing, label + header + note + body)
        self.assertEqual(body_offsets.measured(self.source, self.listing), {name: ('datum', 'D', 0, 8)})
        for gap, annotation in [(header.replace('PAGE   11', 'line 11'), note),
                                (header, note.replace(name, 'different_global')),
                                (header + row('', 0, '', 'L100:'), note)]:
            with self.subTest(gap=gap, annotation=annotation):
                self.write(self.listing, label + gap + annotation + body)
                self.assertIn('0 native storage candidates', self.fails(body_offsets.measured, self.source, self.listing))

    def test_a_private_function_is_measured_at_its_compiler_entry(self):
        self.write(self.source, 'static void tick(void) { }\nstatic void missing(void);\n' + self.source.read_text())
        bindings = self.table.with_suffix('.bindings')
        self.write(bindings, 'pointer first 0x00000000 tick\n')
        listing = self.listing.read_text() + (
            row('P', 0, '', 'L10:  ; function: unrelated') +
            row('', 0, '0009', 'NOP') +
            row('', 2, '', 'L11:  ; function: tick') +
            row('', 2, '000B', 'RTS') + 'PROGRAM SECTION (P): 00000004 Byte(s)\n')
        self.write(self.listing, listing)
        found = body_offsets.measured(self.source, self.listing)
        self.assertEqual(found['tick'], ('entry', 'P', 2, 2))
        self.assertNotIn('unrelated', found)
        self.write(self.table, 'entry  tick  P  0x00000000  2\n')
        self.assertIn(b'entry  tick  P  0x00000002  2\n', body_offsets.refreshed(self.table, found))
        self.write(bindings, 'pointer first 0x00000000 missing\n')
        with self.assertRaisesRegex(SystemExit, 'missing has 0 native storage candidates'):
            body_offsets.measured(self.source, self.listing)

    def test_the_table_places_each_private_target_and_no_other(self):
        # the table places count, a static that a row gives and the source leaves uninitialized, and none of total,
        # which is public, limit, which C initializes, or spare, which no row gives
        self.write(self.source, 'static int count;\nint total;\nstatic int limit = 3;\nstatic int spare;\n'
                   + self.source.read_text())
        self.write(self.table.with_suffix('.bindings'), 'pointer first 0x00000000 count\n'
                   'pointer first 0x00000004 total\npointer second 0x00000000 limit\n')
        listing = self.listing.read_text() + (
            row('B', 0, '', '_count:  ; static: count') + row('', 0, '00000004', '.RES.L      1') +
            row('', 4, '', '_total:  ; static: total') + row('', 4, '00000004', '.RES.L      1') +
            row('', 8, '', '_spare:  ; static: spare') + row('', 8, '00000004', '.RES.L      1') +
            'BSS SECTION (B): 0000000C Byte(s)\n')
        self.write(self.listing, listing)
        self.assertEqual(body_offsets.measured(self.source, self.listing),
                         {'first': ('datum', 'D', 0, 8), 'second': ('datum', 'D', 8, 8), 'count': ('datum', 'B', 0, 4)})
        # make offsets refuses a place for count that the full build would refuse
        self.write(self.source, self.source.read_text().replace('static int count;', 'static short count;'))
        self.assertIn('count disagrees with its C type', self.fails(body_offsets.measured, self.source, self.listing))

    def test_equal_zero_values_do_not_hide_exchanged_offsets(self):
        self.write(self.table, 'datum  first  D  0x00000008  8\ndatum  second  D  0x00000000  8\n')
        measured = body_offsets.measured(self.source, self.listing)
        self.assertEqual(measured, {'first': ('datum', 'D', 0, 8), 'second': ('datum', 'D', 8, 8)})
        self.assertNotEqual(self.table.read_bytes(), body_offsets.refreshed(self.table, measured))
        self.table.unlink()
        generate(Image('1st_read'), self.source, Path('stub'), stub=True)
        self.assertEqual(Path('stub/src/grid/first.inc').read_bytes(), Path('stub/src/grid/second.inc').read_bytes())
        self.assertEqual(body_offsets.refreshed(self.table, measured),
                         b'datum  first  D  0x00000000  8\ndatum  second  D  0x00000008  8\n')

    def test_a_last_comment_without_its_newline_keeps_the_next_row_apart(self):
        self.write(self.table, '# measured, café'.encode())
        measured = {'first': body_offsets.Place('datum', 'D', 0, 8)}
        refreshed = body_offsets.refreshed(self.table, measured)
        self.assertEqual(refreshed, '# measured, café\ndatum  first  D  0x00000000  8\n'.encode())
        self.write(self.table, refreshed)
        self.assertEqual(body_offsets.refreshed(self.table, measured), refreshed)

    def test_a_source_names_its_table_relative_to_the_checkout(self):
        for source in (Path.cwd() / self.source, Path('..') / self.source):
            with self.subTest(source=source), self.assertRaises(SystemExit):
                body_offsets.main([str(source), str(self.listing), 'out.txt'])
        self.assertFalse(Path('out.txt').exists())

    def test_string_offsets_follow_the_compilers_stub_labels(self):
        self.write(self.source, '#include "grid/text.inc"\nchar *labels[] = { FIRST, SECOND };\n')
        self.write(self.table.with_suffix('.bindings'), 'text FIRST 2\ntext SECOND 2\n')
        self.write(self.listing, row('C', 0, '', 'L200:') +
                   row('', 0, '22', ".DATA.B     H'22,H'00") + row('', 2, '', 'L201:') +
                   row('', 2, '21', ".DATA.B     H'21,H'00") + 'CONSTANT SECTION (C): 00000004 Byte(s)\n')
        self.assertEqual(body_offsets.measured(self.source, self.listing),
                         {'FIRST': ('text', 'C', 2, 2), 'SECOND': ('text', 'C', 0, 2)})
        self.listing.write_text(self.listing.read_text().replace("H'22", "H'21"))
        self.assertIn('2 native string placements', self.fails(body_offsets.measured, self.source, self.listing))

    def test_symbolic_bindings_survive_a_missing_table(self):
        self.write(self.source, 'struct frame { int id; char name[4]; };\n'
                   'struct frame frames[2] = {\n#include "grid/frames.inc"\n};\n'
                   'struct refs { void *nested; int *counter; char *letter; };\n'
                   'struct refs refs = {\n#include "grid/refs.inc"\n};\n'
                   'int count = {\n#include "grid/count.inc"\n};\n'
                   'char text[3] = {\n#include "grid/text.inc"\n};\n')
        self.image('1st_read', 'file 1ST_READ.BIN\nD 0x00001000 0x00001023\n'
                   'src/grid.c D 0x00001000 0x00001023\n')
        self.write(self.table, 'datum frames D 0x00000000 16\ndatum refs D 0x00000010 12\n'
                   'datum count D 0x0000001c 4\ndatum text D 0x00000020 3\n')
        bindings = self.table.with_suffix('.bindings')
        facts = 'pointer refs 0x00000000 frames+0xc\npointer refs 0x00000004 count\npointer refs 0x00000008 text+0x2\n'
        self.write(bindings, facts)
        image = Image('1st_read')
        data = bytes(16) + struct.pack('<3I', 0x100c, 0x101c, 0x1022) + bytes(7)
        generate(image, self.source, Path('real'), data)
        self.table.unlink()
        generate(image, self.source, Path('stub'), stub=True)
        emitted = Path('real/src/grid/refs.inc').read_text()
        self.assertEqual(emitted.strip(), 'frames[1].name, &count, &text[2]')
        self.assertEqual(Path('stub/src/grid/refs.inc').read_text(), emitted)
        for bad, diagnostic in [('frames+0x10', 'the byte lies outside it'), ('frames+0x9', 'inside [1].id')]:
            self.write(bindings, facts.replace('frames+0xc', bad))
            self.assertIn(diagnostic, self.fails(generate, image, self.source, Path('stub'), stub=True))

    def test_same_abbreviation_is_bound_by_the_companion_object(self):
        self.write(self.listing, row('CSG', 0, '', '_first:  ; static: first') +
                   row('', 0, '00000000', ".DATA.L     H'00000000,H'00000000") +
                   row('CSG', 0, '', 'L101:  ; static: second') +
                   row('', 0, '00000000', ".DATA.L     H'00000000,H'00000000") +
                   'CONSTANT SECTION (CSG): 00000008 Byte(s)\nCONSTANT SECTION (CSGATLINE): 00000008 Byte(s)\n')
        sections = [('CSG', 8, 4, False), ('CSGATLINE', 8, 4, False)]
        self.write(self.listing.with_suffix('.obj'), obj(sections, exports=[('_first', 1, 0)]))
        self.assertEqual(body_offsets.measured(self.source, self.listing),
                         {'first': ('datum', 'CSGATLINE', 0, 8), 'second': ('datum', 'CSG', 0, 8)})
        self.write(self.listing.with_suffix('.obj'), obj(sections))
        self.assertIn('do not distinguish', self.fails(body_offsets.measured, self.source, self.listing))

    def test_static_header_initializers_are_private_to_each_consumer(self):
        header = Path('src/wheels.h')
        first = Path('src/grid.c')
        second = Path('src/van.c')
        for consumer in (first, second):
            self.write(consumer, '#include "wheels.h"\n')
            self.write(Path('offsets') / consumer.with_suffix('.txt'), 'datum tread D 0x00000000 4\n')
        declaration = 'unsigned tread = {\n#include "wheels/tread.inc"\n};\n'
        self.write(header, 'static ' + declaration)
        self.image('1st_read', 'file 1ST_READ.BIN\nD 0x00001000 0x00001008\n'
                   'src/grid.c D 0x00001000 0x00001004\nsrc/van.c D 0x00001004 0x00001008\n')
        self.assertEqual(body_offsets.sources(), [first, second])
        for consumer, value in ((first, 7), (second, 11)):
            generate(Image('1st_read'), consumer, Path('gen'), struct.pack('<II', 7, 11))
            result = Path('gen/variants') / consumer.with_suffix('') / 'wheels/tread.inc'
            self.assertEqual(int(result.read_text().strip(), 0), value)
        self.write(header, declaration)
        self.assertEqual(body_offsets.sources(), [first, second])
        self.assertIn('internal linkage', self.fails(generate, Image('1st_read'), first, Path('gen'), stub=True))
        self.write('src/wheels/tread.inc', '7\n')
        self.assertEqual(body_offsets.sources(), [])

    def test_inventory_finds_indirect_c_consumers_and_ignores_authored_includes(self):
        self.write('src/grid_wrapper.c', '/* bridge */ #inc\\\nlude "grid_bridge.h"\n')
        self.write('src/grid_bridge.h', '#include /* source */ "sha\\\nred.c"\n')
        self.write('src/shared.c', self.source.read_text().replace('grid/', 'shared/'))
        self.source.write_text(self.source.read_text() +
                               'int fixed[2] = {\n#include "grid/fixed.inc"\n};\n')
        self.write('src/grid/fixed.inc', '4, 5\n')
        generate(Image('1st_read'), self.source, Path('stub'), stub=True)
        self.assertFalse(Path('stub/src/grid/fixed.inc').exists())
        self.write('src/lamp.c', '/* #include "grid_bridge.h" */\nint colors[2] = {\n#include "lamp/colors.inc"\n};\n')
        self.write('src/lamp/colors.inc', '4, 5\n')
        self.image('1st_read', 'file 1ST_READ.BIN\nD 0x00001000 0x00001030\n'
                   'src/grid.c D 0x00001000 0x00001010\nsrc/grid_wrapper.c D 0x00001010 0x00001020\n'
                   'src/lamp.c D 0x00001020 0x00001028\n')
        self.assertEqual(body_offsets.sources(), [self.source, Path('src/grid_wrapper.c')])


class ShippedOffsets(unittest.TestCase):
    def test_every_placed_body_source_has_native_offsets(self):
        self.addCleanup(os.chdir, Path.cwd())
        os.chdir(ROOT)
        expected = {Path('offsets') / source.with_suffix('.txt') for source in body_offsets.sources()}
        missing = sorted(expected - set(Path('offsets').rglob('*.txt')))
        self.assertFalse(missing, f'{missing}: run make offsets to measure the missing placements')

    def test_stub_has_the_width_of_every_measured_string(self):
        self.addCleanup(os.chdir, Path.cwd())
        os.chdir(ROOT)
        offsets = Path('offsets')
        for table in sorted(offsets.rglob('*.txt')):
            source = table.relative_to(offsets).with_suffix('.c')
            with self.subTest(source=source):
                measured = {fields[1]: int(fields[-1]) for _, fields in rows(table) if fields[0] == 'text'}
                lengths, _ = body_bindings.read(source)
                self.assertEqual(measured, lengths)
