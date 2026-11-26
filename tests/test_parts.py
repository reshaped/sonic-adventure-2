"""config/parts.txt, which gives each source its bin, as tools/parts.py reads it: a row's form, the row a source takes,
that the table's head says what each bin is, and that each source of the tree takes a row and each row a source."""

import os
import re
import unittest
from pathlib import Path

import parts
from parts import Part
from support import TreeTest

ROOT = Path(__file__).resolve().parents[1]
AT = 'config/parts.txt'
# a table: a directory of each bin, a directory under one, and a file under each
TABLE = ('# path  bin\n'
         'src/  play\n'
         'src/draw/  picture\n'
         '\n'
         'src/draw/motion.c  play  # a comment\n'
         'src/sound/  none\n'
         'sdk/ninja/njSin.c  play\n')
ROWS = {'src/': Part('play', f'{AT}:2'), 'src/draw/': Part('picture', f'{AT}:3'),
        'src/draw/motion.c': Part('play', f'{AT}:5'), 'src/sound/': Part('none', f'{AT}:6'),
        'sdk/ninja/njSin.c': Part('play', f'{AT}:7')}
# files that take each of TABLE's rows
FILES = ['src/main.c', 'src/draw/fade.c', 'src/draw/motion.c', 'src/sound/sound.h', 'sdk/ninja/njSin.c']


class Rows(TreeTest):
    def rows(self, table):
        self.write(AT, table)
        return parts.rows()

    def test_reads_each_row_by_its_path_with_its_bin_and_its_line(self):
        self.assertEqual(self.rows(TABLE), ROWS)

    def test_takes_each_bin_and_no_other(self):
        for held in parts.BINS:
            with self.subTest(bin=held):
                self.assertEqual(self.rows(f'src/  {held}\n')['src/'].bin, held)
        for row, problem in (('src/  game', 'game: not a bin, one of play, picture, none'),
                             ('src/  Play', 'Play: not a bin, one of ')):
            with self.subTest(row=row), self.assertRaisesRegex(SystemExit, rf': {AT}:2: {re.escape(problem)}'):
                self.rows(f'# a comment\n{row}\n')

    def test_rejects_a_row_of_other_than_two_fields(self):
        for row in ('src/', 'src/  play  picture'):
            with self.subTest(row=row), self.assertRaisesRegex(SystemExit, rf': {AT}:3: not a row: PATH BIN$'):
                self.rows(f'sdk/  play\n\n{row}\n')

    def test_rejects_a_second_row_for_a_path(self):
        # a directory and a file of one name but the / are two paths
        self.assertEqual(set(self.rows('src/  play\nsrc  play\n')), {'src/', 'src'})
        with self.assertRaisesRegex(SystemExit, rf': {AT}:3: src/a\.c has a row already, at {AT}:1$'):
            self.rows('src/a.c  play\nsrc/  play\nsrc/a.c  picture\n')


class Taken(unittest.TestCase):
    def test_a_source_takes_the_longest_row_that_is_it_or_a_directory_above_it(self):
        for path, row in (('src/main.c', 'src/'), ('src/draw/fade.c', 'src/draw/'),
                          ('src/draw/motion.c', 'src/draw/motion.c'), ('src/draw/motion.h', 'src/draw/'),
                          ('src/sound/sound.h', 'src/sound/'), ('src/system/vector.src', 'src/'),
                          ('sdk/ninja/njSin.c', 'sdk/ninja/njSin.c')):
            with self.subTest(path=path):
                self.assertEqual(parts.part(ROWS, path), ROWS[row])

    def test_a_path_under_no_row_or_no_source_takes_none(self):
        # a directory's row is no row of a path that only starts as it does; a file of another suffix, or under no
        # root, is no source
        for path in ('sdk/ninja/njCos.c', 'srcs/a.c', 'a/src/main.c', 'src/main.py', 'src/.clang-tidy', 'src/draw/',
                     'config/a.c', 'tests/kernels/console.c', ''):
            with self.subTest(path=path):
                self.assertIsNone(parts.part(ROWS, path))


class Faults(unittest.TestCase):
    def test_finds_none_where_each_source_takes_a_row_and_each_row_a_source(self):
        self.assertEqual(parts.faults(ROWS, [*FILES, 'src/README.md', 'tools/parts.py', 'config/a.c']), [])

    def test_names_each_source_that_takes_no_row(self):
        unplaced = ['sdk/a.c', 'modules/m/a.h', 'engine/dc/a.c', 'sdk/ninja/njCos.src']
        self.assertEqual(parts.faults(ROWS, [*FILES, *unplaced]),
                         [f'{path}: no row of {AT} gives its bin: give it one, or a directory above it, PATH BIN'
                          for path in unplaced])

    def test_names_each_row_that_no_source_takes(self):
        unused = "no source takes this row: none lies at or under its PATH, or each that does takes a longer row. " \
                 "Remove the row; a directory's PATH ends in /"
        # a file that isn't there, a directory with no source, and a directory whose every source takes a longer row
        for gone, where in (('sdk/ninja/njSin.c', f'{AT}:7: sdk/ninja/njSin.c'),
                            ('src/sound/sound.h', f'{AT}:6: src/sound/'),
                            ('src/draw/fade.c', f'{AT}:3: src/draw/')):
            with self.subTest(gone=gone):
                self.assertEqual(parts.faults(ROWS, [path for path in FILES if path != gone]), [f'{where}: {unused}'])


class Tree(unittest.TestCase):
    maxDiff = None

    def setUp(self):
        home = os.getcwd()
        self.addCleanup(os.chdir, home)
        os.chdir(ROOT)

    def test_each_source_takes_a_row_and_each_row_a_source(self):
        self.assertEqual(parts.faults(parts.rows(), parts.sources()), [])

    def test_the_table_s_head_says_what_each_bin_is_and_no_other(self):
        # the head lists each bin two spaces in after a #, a name and then its meaning
        head = re.findall(r'^#   (\w+) ', (ROOT / AT).read_text(), re.M)
        self.assertEqual(head, list(parts.BINS))


if __name__ == '__main__':
    unittest.main()
