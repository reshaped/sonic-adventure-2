"""The image tables: what config.py reads, and each rule it holds a table to."""

import subprocess
from pathlib import Path
from unittest import mock

import config
from support import TreeTest

SPLITS = """\
file    SONIC2/STG13.PRS
D       0x8c500100  0x8c500200   # a comment
P       0x8c500000  0x8c500100
B       0x8c500200  0x8c500300

disc            P  0x8c500000  0x8c500040
modules/a.c     P  0x8c500040  0x8c5000c0
disc            P  0x8c500060  0x8c500080
modules/b.src   P  0x8c5000e0  0x8c500100
modules/a.c     D  0x8c500100  0x8c500110
modules/a.c     B  0x8c500200  0x8c500220
"""
# SPLITS' last row, which a test puts a row of its own ahead of
BSS = 'modules/a.c     B  0x8c500200  0x8c500220\n'


class Tables(TreeTest):
    def setUp(self):
        super().setUp()
        self.write('modules/a.c', '')
        self.write('modules/b.src', '')

    def read(self, splits, name='stg13'):
        self.image(name, splits)
        return config.Image(name)

    def test_reads_a_table(self):
        image = self.read(SPLITS)
        self.assertEqual(image.file, 'SONIC2/STG13.PRS')
        self.assertTrue(image.compressed)
        self.assertEqual(image.base, 0x8c500000)
        self.assertEqual([name for name, _, _ in image.sections], ['P', 'D', 'B'])
        self.assertEqual(image.sources(), ['modules/a.c', 'modules/b.src'])
        self.assertEqual(image.ranges('modules/a.c'), [('P', 0x8c500040, 0x8c5000c0), ('D', 0x8c500100, 0x8c500110),
                                                        ('B', 0x8c500200, 0x8c500220)])

    def test_a_linked_source_holds_no_bss(self):
        held = self.read(SPLITS).held('modules/a.c')
        self.assertEqual([section for section, _, _ in held], ['P', 'D'])

    def test_a_disc_row_inside_a_source_is_its_unmatched_part(self):
        image = self.read(SPLITS)
        self.assertEqual(image.unmatched('modules/a.c'), [(0x8c500060, 0x8c500080)])
        self.assertEqual(image.unmatched('modules/b.src'), [])

    def refused(self, splits):
        self.image('stg13', splits)
        return self.fails(config.Image, 'stg13')

    def test_names_the_line_of_a_malformed_row(self):
        self.assertIn('config/stg13/splits.txt:2: not a row', self.refused('file A.BIN\nP 0x8c500000\n'))
        self.assertIn('splits.txt:2: 0x8C500000: not an address', self.refused('file A.BIN\nP 0x8C500000 0x8c500100\n'))
        self.assertIn('holds no byte', self.refused('file A.BIN\nP 0x8c500100 0x8c500100\n'))
        self.assertIn("not a file's path", self.refused('file a.bin\nP 0x8c500000 0x8c500100\n'))

    def test_a_section_name_may_hold_an_underscore(self):
        # as DSG_INIT_, the section #pragma section _INIT_ makes of DSG
        image = self.read('file A.BIN\nDSG_INIT_ 0x8c500000 0x8c500100\nmodules/a.c DSG_INIT_ 0x8c500000 0x8c500004\n')
        self.assertEqual(image.sections, [('DSG_INIT_', 0x8c500000, 0x8c500100)])
        self.assertEqual(image.ranges('modules/a.c'), [('DSG_INIT_', 0x8c500000, 0x8c500004)])
        self.assertIn('splits.txt:2: not a row', self.refused('file A.BIN\n_DSG 0x8c500000 0x8c500100\n'))

    def test_needs_one_file_row_and_a_section(self):
        self.assertIn('no file row', self.refused('P 0x8c500000 0x8c500100\n'))
        self.assertIn('a second file row', self.refused('file A.BIN\nfile B.BIN\n'))
        self.assertIn('no section rows', self.refused('file A.BIN\n'))
        self.assertIn('a second row for section P', self.refused(SPLITS + 'P 0x8c600000 0x8c600100\n'))

    def test_input_section_must_keep_its_storage_and_single_output(self):
        self.assertIn('must both be code, stored data or bss',
                      self.refused(SPLITS.replace('modules/a.c     D', 'modules/a.c     PATCH:D')))
        self.assertIn('places its INPUT in two output sections',
                      self.refused(SPLITS.replace('modules/a.c     D', 'modules/a.c     INPUT:D')
                                   .replace('B       0x8c500200', 'C       0x8c500200')
                                   .replace('modules/a.c     B', 'modules/a.c     INPUT:C')))
        self.assertIn('a disc row names its output section only',
                      self.refused(SPLITS.replace('disc            P', 'disc            PTEXT:P', 1)))

    def test_sections_must_not_overlap(self):
        self.assertIn('splits.txt:3: section D starts at 0x8c5000f0, inside P, at config/stg13/splits.txt:2',
                      self.refused('file A.BIN\nP 0x8c500000 0x8c500100\nD 0x8c5000f0 0x8c500200\n'))
        # written in reverse address order, each row keeps its own line
        self.assertIn('splits.txt:2: section D starts at 0x8c5000f0, inside P, at config/stg13/splits.txt:3',
                      self.refused('file A.BIN\nD 0x8c5000f0 0x8c500200\nP 0x8c500000 0x8c500100\n'))

    def test_a_placement_lies_in_its_section(self):
        self.assertIn('modules/a.c lies outside the section D',
                      self.refused(SPLITS.replace('D  0x8c500100  0x8c500110', 'D  0x8c5000f0  0x8c500110')))

    def test_a_source_must_be_a_file(self):
        self.assertIn('modules/c.c: no such file', self.refused(SPLITS.replace(BSS, 'modules/c.c D 0x8c500120 0x8c500130\n' + BSS)))

    def test_a_c_and_an_assembly_source_of_one_stem_would_build_one_object(self):
        self.write('modules/a.src', '')
        self.assertIn('would build one object', self.refused(SPLITS.replace(BSS, 'modules/a.src D 0x8c500120 0x8c500130\n' + BSS)))

    def test_sources_must_not_overlap(self):
        self.assertIn('modules/b.src starts inside modules/a.c',
                      self.refused(SPLITS.replace('0x8c5000e0  0x8c500100', '0x8c5000b0  0x8c500100')))

    def test_placements_run_in_address_order(self):
        self.assertIn('splits.txt:12: a row that starts at 0x8c5000c0 comes after the one at config/stg13/splits.txt:11, '
                      'out of address order', self.refused(SPLITS + 'modules/a.c P 0x8c5000c0 0x8c5000d0\n'))

    def test_a_disc_row_lies_in_one_source_row_or_in_none(self):
        crossing = SPLITS.replace('disc            P  0x8c500060  0x8c500080', 'disc  P  0x8c5000a0  0x8c5000d0')
        self.assertIn('crosses the edge of modules/a.c', self.refused(crossing))
        inside = SPLITS.replace('modules/a.c     P', 'disc P 0x8c500020 0x8c500030\nmodules/a.c     P')
        self.assertIn('row starts inside the one', self.refused(inside))

    def test_the_disc_gives_no_bss(self):
        self.assertIn('bss holds zeros', self.refused(SPLITS + 'disc B 0x8c500220 0x8c500240\n'))


class Bounds(TreeTest):
    def setUp(self):
        super().setUp()
        self.write('modules/a.c', '')
        self.write('modules/b.src', '')

    def bounds(self, text):
        self.image('stg13', SPLITS)
        self.write('config/stg13/bounds.txt', text)
        return config.Image('stg13').bounds()

    def test_reads_each_sections_bound_in_order(self):
        self.assertEqual(self.bounds('P 0x8c500000 0x8c500100  # P\n$G0 0x8c500100 0x8c500100\nB 0x8c500200 0x8c500220\n'),
                         [('P', 0x8c500000, 0x8c500100), ('$G0', 0x8c500100, 0x8c500100), ('B', 0x8c500200, 0x8c500220)])
        # an image without bounds.txt has none
        self.image('stg14', SPLITS)
        self.assertEqual(config.Image('stg14').bounds(), [])

    def test_refuses_a_bound_its_tables_contradict(self):
        for text, problem in (('P 0x8c500000\n', 'bounds.txt:1: not a row: SECTION START END'),
                              ('P 0x8c500010 0x8c500000\n', 'bounds.txt:1: P ends at 0x8c500000, before its start'),
                              ('R 0x8c500100 0x8c500100\nR 0x8c500100 0x8c500100\n', 'bounds.txt:2: a second row for R'),
                              ('R 0x8c500100 0x8c500104\n', 'bounds.txt:1: R has no row in config/stg13/splits.txt, so it '
                                                            'holds no byte and ends where it starts'),
                              ('B 0x8c500200 0x8c500220\nP 0x8c500000 0x8c500100\n',
                               'bounds.txt:2: P starts at 0x8c500000, before B, out of address order'),
                              ('P 0x8c500004 0x8c500100\n', 'bounds.txt:1: P starts at 0x8c500004, and its row in '
                                                            'config/stg13/splits.txt at 0x8c500000')):
            with self.subTest(text=text):
                self.assertIn(problem, self.fails(self.bounds, text))


    def test_refuses_a_bound_whose_end_drifts_from_its_sections_rows(self):
        # B's row ends at 0x8c500300, and a.c's placement there at 0x8c500220: a bound that ends at either holds
        self.assertEqual(self.bounds('B 0x8c500200 0x8c500300\n'), [('B', 0x8c500200, 0x8c500300)])
        for end in ('0x8c500224', '0x8c500210', '0x8c500304'):
            with self.subTest(end=end):
                self.assertIn(f'bounds.txt:1: B ends at {end}, and config/stg13/splits.txt ends it at 0x8c500220 or '
                              '0x8c500300', self.fails(self.bounds, f'B 0x8c500200 {end}\n'))
        # a disc row that runs from a.c's data to D's end may be the gap after D's last object: the bound may end
        # where it starts, and nowhere else inside it
        self.image('stg13', SPLITS.replace(BSS, 'disc            D  0x8c500110  0x8c500200\n' + BSS))
        self.write('config/stg13/bounds.txt', 'D 0x8c500100 0x8c500110\n')
        self.assertEqual(config.Image('stg13').bounds(), [('D', 0x8c500100, 0x8c500110)])
        self.write('config/stg13/bounds.txt', 'D 0x8c500100 0x8c500108\n')
        self.assertIn('bounds.txt:1: D ends at 0x8c500108, and config/stg13/splits.txt ends it at 0x8c500110 or '
                      '0x8c500200', self.fails(lambda: config.Image('stg13').bounds()))

    def test_a_disc_row_that_ends_with_a_source_is_no_gap(self):
        # in either order, the disc row lies inside a.c, and P ends where a.c does
        disc, source = 'disc P 0x8c500010 0x8c500020\n', 'modules/a.c P 0x8c500010 0x8c500020\n'
        self.write('config/stg13/bounds.txt', 'P 0x8c500000 0x8c500010\n')
        for rows in (disc + source, source + disc):
            with self.subTest(rows=rows):
                self.image('stg13', 'file A.BIN\nP 0x8c500000 0x8c500020\n' + rows)
                self.assertEqual(config.Image('stg13').section_ends('P'), [0x8c500020])
                self.assertIn('P ends at 0x8c500010', self.fails(lambda: config.Image('stg13').bounds()))
        # with no source there, the disc row may be the gap after P's last object
        self.image('stg13', 'file A.BIN\nP 0x8c500000 0x8c500020\n' + disc)
        self.assertEqual(config.Image('stg13').bounds(), [('P', 0x8c500000, 0x8c500010)])


class Names(TreeTest):
    def test_an_image_takes_its_own_names_then_the_main_executables(self):
        self.image('1st_read', 'file 1ST_READ.BIN\nP 0x8c010000 0x8c020000\n',
                   '0x8c010000 start\n0x8c010100 shared\n')
        self.image('stg13', 'file SONIC2/STG13.PRS\nP 0x8c500000 0x8c500100\n', '0x8c500000 shared\n')
        self.assertEqual(config.Image('stg13').symbols(), [(0x8c500000, 'shared'), (0x8c010000, 'start')])
        self.assertEqual(config.Image('1st_read').symbols(), [(0x8c010000, 'start'), (0x8c010100, 'shared')])

    def test_an_image_without_a_symbols_table_names_nothing_of_its_own(self):
        self.image('1st_read', 'file 1ST_READ.BIN\nP 0x8c010000 0x8c020000\n')
        self.assertEqual(config.Image('1st_read').symbols(), [])

    def test_a_name_has_one_row(self):
        self.write('symbols.txt', '0x8c010000 twice\n0x8c010004 twice\n')
        self.assertIn('symbols.txt:2: twice has a row already, at symbols.txt:1', self.fails(config.named, 'symbols.txt'))

    def test_names_run_in_address_order(self):
        self.write('symbols.txt', '0x8c010004 later\n0x8c010004 same\n0x8c010000 first\n')
        self.assertIn('symbols.txt:3: first comes after same, out of address order',
                      self.fails(config.named, 'symbols.txt'))

    def test_lists_the_images_in_name_order(self):
        self.image('stg13', '')
        self.image('1st_read', '')
        self.write('config/compilers.mk', '')
        self.assertEqual(config.images(), ['1st_read', 'stg13'])


class Files(TreeTest):
    def test_a_save_leaves_a_file_beside_its_path_alone(self):
        self.write('out.part', 'sentinel')
        config.save('out', b'saved')
        self.assertEqual(Path('out').read_bytes(), b'saved')
        self.assertEqual(Path('out.part').read_text(), 'sentinel')
        self.assertEqual(sorted(path.name for path in Path().iterdir()), ['out', 'out.part'])

    def test_a_failed_save_keeps_the_file_at_its_path(self):
        self.write('out', 'before')
        with mock.patch.object(Path, 'replace', side_effect=OSError(28, 'No space left on device')):
            self.assertIn('out: No space left on device', self.fails(config.save, 'out', b'saved'))
        self.assertEqual(Path('out').read_text(), 'before')
        self.assertEqual(sorted(path.name for path in Path().iterdir()), ['out'])

    def test_an_optional_table_that_cant_be_read_is_not_absent(self):
        self.image('stg13', 'file A.BIN\nP 0x8c500000 0x8c500020\n')
        self.assertEqual((config.named('config/stg13/symbols.txt'), config.Image('stg13').bounds()), ([], []))
        # a symlink to itself is a table there that no read can open
        for table in ('symbols.txt', 'bounds.txt'):
            Path('config/stg13', table).symlink_to(table)
        self.assertIn('config/stg13/symbols.txt: Too many levels of symbolic links',
                      self.fails(config.named, 'config/stg13/symbols.txt'))
        self.assertIn('config/stg13/bounds.txt: Too many levels of symbolic links',
                      self.fails(lambda: config.Image('stg13').bounds()))

    def test_a_stub_source_is_named_as_the_tables_name_it(self):
        self.write('offsets/modules/a.txt', '')
        self.assertEqual(config.stub_sources(), {'modules/a.c'})

    def test_a_failed_command_is_named_by_its_program(self):
        for command in ('make', ['make', 'dc'], Path('make')):
            with self.subTest(command=command):
                self.assertEqual(config.said(subprocess.CalledProcessError(2, command)), 'make: exit 2')
        self.assertEqual(config.said(subprocess.CalledProcessError(-9, 'make')), 'make: killed by signal 9')
