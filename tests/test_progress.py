"""README.md's numbers: the shares' rounding, the rows, the data's line and the marks."""

import progress
from support import TreeTest


class Shares(TreeTest):
    def test_never_overstate_the_source(self):
        self.assertEqual(progress.shares(999, 0, 1), (999, 0, 1))
        self.assertEqual(progress.shares(2112, 0, 1304912), (1, 0, 999))
        self.assertEqual(progress.shares(1, 1, 9998), (0, 0, 1000))
        self.assertEqual(sum(progress.shares(333, 333, 334)), 1000)
        self.assertEqual(progress.shares(0, 0, 0), (0, 0, 0))
        self.assertEqual(progress.shares(4804, 248, 1301972), (3, 0, 997))

    def test_assembly_takes_the_rest_once_nothing_is_the_discs(self):
        self.assertEqual(progress.shares(9769, 231, 0), (976, 24, 0))

    def test_shows_none_and_under_a_tenth_apart(self):
        self.assertEqual(progress.percent(0, 0), '0%')
        self.assertEqual(progress.percent(0, 5), '<0.1%')
        self.assertEqual(progress.percent(1000, 5), '100.0%')
        self.assertEqual(progress.percent(977, 5), '97.7%')

    def test_gives_each_image_its_row(self):
        rows = {name: progress.area(name) for name in ('1st_read', 'stg13', 'boss_golem_e', 'chao', 'chaostgkinder',
                                                       'cart', 'title')}
        self.assertEqual(rows, {'1st_read': 'Main executable', 'stg13': 'Stages', 'boss_golem_e': 'Bosses',
                                'chao': 'Chao', 'chaostgkinder': 'Chao', 'cart': 'Other programs',
                                'title': 'Other programs'})

    def test_the_table_ends_with_a_row_for_all(self):
        lines = progress.table({'Main executable': [1, 750, 50, 200, 0], 'Stages': [2, 0, 0, 1000, 0]})
        self.assertEqual(lines[0], "**40.0% of the game's code builds from source, byte for byte.**")
        self.assertEqual(lines[-5:-2], ['| Main executable | 1 | 1,000 | 75.0% | 5.0% | 20.0% |',
                                      '| Stages | 2 | 1,000 | 0% | 0% | 100.0% |',
                                      '| **All** | **3** | **2,000** | **37.5%** | **2.5%** | **60.0%** |'])

    def test_the_headline_leaves_the_disc_its_share_of_all(self):
        # C's share and assembly's round down to a tenth each, so the source's share is their sum, not its own
        lines = progress.table({'Main executable': [1, 15, 15, 9970, 0]})
        self.assertEqual(lines[0], "**0.2% of the game's code builds from source, byte for byte.**")
        self.assertEqual(lines[-3], '| **All** | **1** | **10,000** | **0.1%** | **0.1%** | **99.8%** |')

    def test_the_discs_column_goes_once_no_code_is_the_discs(self):
        lines = progress.table({'Main executable': [1, 977, 23, 0, 23]})
        self.assertEqual(lines[:-2], ["**100% of the game's code builds from source, byte for byte.**", '',
                                 '| | Binaries | Code bytes | C | Assembly |', '|---|---:|---:|---:|---:|',
                                 '| Main executable | 1 | 1,000 | 97.7% | 2.3% |',
                                 '| **All** | **1** | **1,000** | **97.7%** | **2.3%** |'])

    def test_ends_with_the_assemblys_share_that_a_basis_covers(self):
        # rounded down, as C's share is, so the line never overstates the evidence
        for covered, share in ((0, '0%'), (1, '<0.1%'), (1999, '99.9%'), (2000, '100.0%')):
            with self.subTest(covered=covered):
                lines = progress.table({'Main executable': [1, 8000, 2000, 0, covered]})
                self.assertEqual(lines[-2:], ['', '[@asm tags](TAGS.md#assembly) record evidence of handwritten '
                                                  f'origins for {share} of the assembly.'])
        # with no assembly, none of it is covered
        self.assertIn(' for 0% of ', progress.table({'Main executable': [1, 1000, 0, 0, 0]})[-1])


class Data(TreeTest):
    def test_counts_the_bytes_by_what_supplies_them(self):
        line = progress.data_line({'C': 1500, 'assembly': 100, 'generated C': 8000, 'disc': 300, 'fill': 100})
        self.assertEqual(line, 'Initialized data: 10,000 bytes. The sources supply 1,600, disc-derived initializers '
                               '8,000, direct disc copies 300 and linker fill 100.')

    def test_counts_a_writer_that_supplies_nothing_as_zero(self):
        line = progress.data_line({'C': 1659, 'generated C': 8332, 'fill': 9})
        self.assertIn('direct disc copies 0 and linker fill 9.', line)


class Marks(TreeTest):
    def test_writes_between_the_marks(self):
        text = 'intro\n<!-- progress -->\nold\n<!-- /progress -->\nrest\n'
        self.assertEqual(progress.written(text, ['new', 'lines']),
                         'intro\n<!-- progress -->\nnew\nlines\n<!-- /progress -->\nrest\n')

    def test_takes_marks_on_the_first_and_last_lines(self):
        text = '<!-- progress -->\nold\n<!-- /progress -->'
        fresh = progress.written(text, ['new'])
        self.assertEqual(fresh, '<!-- progress -->\nnew\n<!-- /progress -->')
        self.assertEqual(progress.written(fresh, ['new']), fresh)

    def test_needs_one_of_each_mark_in_order(self):
        for text in ('no marks\n', 'a\n<!-- /progress -->\n<!-- progress -->\n',
                     'a\n<!-- progress -->\n<!-- progress -->\n<!-- /progress -->\n',
                     'a\n<!-- progress -->\nold\nkeep this: <!-- /progress -->\nrest\n',
                     'a\n<!-- progress -->\nold\n<!-- /progress -->\n<!-- /progress -->\n'):
            with self.subTest(text=text):
                self.assertIn('needs one <!-- progress --> line', self.fails(progress.written, text, []))
