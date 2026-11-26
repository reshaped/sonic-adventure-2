"""config/kept.txt's rows as tools/kept.py reads them: their form, their words and their overlaps, and the
repository's own rows, each in its image's span."""

import os
import unittest

from config import Image
from kept import reasons
from relink import span
from support import ROOT, TreeTest

# a stage and the main executable, each with one section
SPLITS = {'stg03': 'file SONIC2/STG03.PRS\nP 0x8c500000 0x8c500200\n',
          '1st_read': 'file 1ST_READ.BIN\nP 0x8c010000 0x8c010200\n'}


class Reasons(TreeTest):
    def reasons(self, *rows):
        for name, splits in SPLITS.items():
            self.image(name, splits)
        self.write('config/kept.txt', ''.join(f'{row}\n' for row in rows))
        return reasons()

    def test_reads_a_word_or_a_range_by_image_with_its_reason(self):
        # a range may start and end off 4, where it holds a word
        found = self.reasons('# a comment', 'stg03  0x8c500010  two   instructions',
                             'stg03  0x8c500100  0x8c500110  a glyph bitmap', 'stg03  0x8c500113  0x8c500115  a text',
                             '1st_read  0x8c0101a0  text')
        self.assertEqual(found, {'stg03': [(0x8C500010, 0x8C500014, 'two instructions', 'config/kept.txt:2'),
                                           (0x8C500100, 0x8C500110, 'a glyph bitmap', 'config/kept.txt:3'),
                                           (0x8C500113, 0x8C500115, 'a text', 'config/kept.txt:4')],
                                 '1st_read': [(0x8C0101A0, 0x8C0101A4, 'text', 'config/kept.txt:5')]})

    def test_rejects_rows_out_of_form(self):
        for row, problem in (('stg03  0x8c500010', 'not a row: IMAGE START [END] REASON'),
                             ('stg03  0x8c500010  0x8c500014', 'not a row: IMAGE START [END] REASON'),
                             ('none  0x8c500010  text', 'none: no image of config/'),
                             ('stg03  8c500010  text', '8c500010: not an address'),
                             ('stg03  0x8c500010  0x8c500010  text', '0x8c500010 to 0x8c500010 holds no byte'),
                             ('stg03  0x8c500011  0x8c500014  text', '0x8c500011 to 0x8c500014 holds no word'),
                             ('stg03  0x8c500012  text', "0x8c500012: not on 4, so no word's address")):
            with self.subTest(row=row):
                self.assertIn(f'config/kept.txt:1: {problem}', self.fails(self.reasons, row))

    def test_reads_an_address_after_start_as_end(self):
        found = self.reasons('stg03  0x8c500010  0x8c500020  is literal data',
                             'stg03  0x8c500020  0x8c500024  0x8c500030 is literal data')
        self.assertEqual(found['stg03'], [(0x8C500010, 0x8C500020, 'is literal data', 'config/kept.txt:1'),
                                          (0x8C500020, 0x8C500024, '0x8c500030 is literal data',
                                           'config/kept.txt:2')])

    def test_rejects_a_row_that_gives_a_word_an_earlier_row_of_its_image_gives(self):
        # another image's row may hold the same addresses, and rows that share bytes but no word both stand
        self.reasons('stg03  0x8c500014  text', '1st_read  0x8c500014  text')
        self.reasons('stg03  0x8c500014  text', 'stg03  0x8c500015  0x8c500020  another reason')
        self.assertIn('config/kept.txt:2: 0x8c500010 to 0x8c500020 overlaps the row at config/kept.txt:1',
                      self.fails(self.reasons, "stg03  0x8c500014  0x8c500018  a model's UVs",
                                 'stg03  0x8c500010  0x8c500020  another reason'))
        # each pair shares only the byte at 0x8c500014, and so gives the word there twice
        for rows, later in ((('stg03  0x8c500014  0x8c500015  text', 'stg03  0x8c500014  another reason'),
                             '0x8c500014 to 0x8c500018'),
                            (('stg03  0x8c500014  text', 'stg03  0x8c500011  0x8c500015  another reason'),
                             '0x8c500011 to 0x8c500015')):
            with self.subTest(rows=rows):
                self.assertIn(f'config/kept.txt:2: {later} overlaps the row at config/kept.txt:1',
                              self.fails(self.reasons, *rows))


class Repository(unittest.TestCase):
    def test_reads_the_repositorys_rows_each_in_its_images_span(self):
        # config/kept.txt as make shift reads it, so that a malformed row fails without the disc
        self.addCleanup(os.chdir, os.getcwd())
        os.chdir(ROOT)
        for image, rows in reasons().items():
            low, high = span(Image(image))
            for start, end, _, where in rows:
                self.assertTrue(low <= start and end <= high, where)


if __name__ == '__main__':
    unittest.main()
