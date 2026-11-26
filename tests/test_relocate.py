"""relocate.py rewrites each pointer row that spells its target as C into a relocation, through the declarations of
each source that reads the row, and drops each null row and each reserve row."""
from pathlib import Path
from unittest import mock

import body_bindings
import relocate
from support import ROOT, TreeTest, run

# the types the rows point into: struct mixed holds padding between tag and count, and frames gives no extent
DECLARATIONS = """\
struct pair { short first, second; };
struct mixed { char tag; int count; };
union cell { short halves[2]; int word; };
struct frame { int id; char name[4]; };
struct refs { void *at[8]; };
extern char next[4];
extern struct mixed mixed;
extern union cell cell;
extern short rows[3][2];
extern struct frame frames[];
typedef struct frame Frames[];
extern Frames listed;
void tick(void);
"""
BODY = 'struct refs refs = {\n#include "rows/refs.inc"\n};\n'


class Relocate(TreeTest):
    source = Path('src/rows.c')
    table = Path('offsets/src/rows.bindings')

    def setUp(self):
        super().setUp()
        self.write(self.source, DECLARATIONS + BODY)

    def relocated(self, rows, *tables):
        """The table's text once relocate.py has rewritten rows, or None where it went. A run that fails gives its
        message."""
        self.write(self.table, rows)
        status, output = run(relocate.main, *map(str, tables))
        if status:
            return output
        return self.table.read_text() if self.table.exists() else None

    def test_each_target_spelled_as_c_becomes_a_relocation(self):
        rows = ('# rows of refs\n'
                'text  LABEL  4\n'
                'pointer  refs  0x00000000  &mixed\n'
                'pointer  refs  0x00000004  @tick\n'
                'pointer  refs  0x00000008  next  0x00000002\n'
                'pointer  refs  0x0000000c  next  0x00000000\n'
                '\n'
                'pointer refs 0x00000010 mixed.count   # its count\n'
                'pointer  refs  0x00000014  cell.halves[1]\n'
                'pointer  refs  0x00000018  rows[1][1]\n'
                'pointer  refs  0x0000001c  frames[1].name\n')
        self.assertEqual(self.relocated(rows), '# rows of refs\n'
                                               'text  LABEL  4\n'
                                               'pointer  refs  0x00000000  mixed\n'
                                               'pointer  refs  0x00000004  tick\n'
                                               'pointer  refs  0x00000008  next+0x2\n'
                                               'pointer  refs  0x0000000c  next\n'
                                               '\n'
                                               'pointer refs 0x00000010 mixed+0x4   # its count\n'
                                               'pointer  refs  0x00000014  cell+0x2\n'
                                               'pointer  refs  0x00000018  rows+0x6\n'
                                               'pointer  refs  0x0000001c  frames+0xc\n')

    def test_a_null_row_and_a_reserve_row_go_and_every_other_row_stays(self):
        rows = ('pointer  refs  0x00000000  LABEL\n'
                'pointer  refs  0x00000004  0\n'
                'pointer  refs  0x00000008  next+0x3\n'
                'reserve  spare\n'
                'selector  refs.at[1]  0\n'
                'stop  refs  END\n')
        self.assertEqual(self.relocated(rows), 'pointer  refs  0x00000000  LABEL\npointer  refs  0x00000008  next+0x3\n'
                                               'selector  refs.at[1]  0\nstop  refs  END\n')
        # a source's own table left with no row goes, its comments with it, and a shared one stays
        self.assertIsNone(self.relocated('# null fields\npointer  refs  0x00000000  0\nreserve  spare\n'))
        self.write('layouts/nulls.bindings', 'pointer  refs  0x00000000  0\n')
        self.assertEqual(self.relocated('include  nulls\n'), 'include  nulls\n')
        self.assertEqual(Path('layouts/nulls.bindings').read_text(), '')

    def test_a_fifth_field_after_any_target_but_a_name_stays_for_the_reader_to_refuse(self):
        # the older reader took a fifth field after a plain NAME alone, so a rewrite of any other would guess
        rows = ('pointer  refs  0x00000000  &mixed  0x00000002\n'
                'pointer  refs  0x00000004  mixed.count  0x00000002\n'
                'pointer  refs  0x00000008  0  0x00000004\n'
                'pointer  refs  0x0000000c  next+0x2  0x00000002\n')
        self.assertEqual(self.relocated(rows), rows)
        for row in rows.splitlines(keepends=True):
            with self.subTest(row=row):
                self.write(self.table, row)
                self.assertIn('tools/relocate.py writes for a target spelled as C',
                              self.fails(body_bindings.read, self.source))

    def test_a_member_path_reads_the_declaration_in_scope_at_the_rows_datum(self):
        # a static of the datum's own function's blocks, and an array that a typedef gives no extent, which holds
        # any index
        self.write(self.source, DECLARATIONS + 'void draw(void)\n{\n    static struct pair pairs;\n'
                   '    static struct refs refs = {\n#include "rows/refs.inc"\n    };\n}\n')
        self.assertEqual(self.relocated('pointer  refs  0x00000000  pairs.second\n'
                                        'pointer  refs  0x00000004  listed[3].name\n'),
                         'pointer  refs  0x00000000  pairs+0x2\npointer  refs  0x00000004  listed+0x1c\n')
        # a block's static beside a declaration at file scope may or may not hide it, by blocks no declarator gives
        self.write(self.source, DECLARATIONS + 'void draw(void)\n{\n    static struct pair mixed;\n'
                   '    static struct refs refs = {\n#include "rows/refs.inc"\n    };\n}\n')
        self.assertIn('mixed has a static declaration in a block of draw, and another declaration',
                      self.relocated('pointer  refs  0x00000000  mixed.second\n'))

    def test_a_shared_tables_member_path_reads_each_source_that_includes_it(self):
        shared = Path('layouts/cells.bindings')
        self.write(shared, 'pointer  refs  0x00000000  cell.word\npointer  refs  0x00000004  cell.halves[1]\n')
        other = Path('src/other.c')
        self.write(other, DECLARATIONS + 'struct refs refs = {\n#include "other/refs.inc"\n};\n')
        self.write('offsets/src/other.bindings', 'include  cells\n')
        self.assertEqual(self.relocated('include  cells\n', shared), 'include  cells\n')
        self.assertEqual(shared.read_text(), 'pointer  refs  0x00000000  cell\npointer  refs  0x00000004  cell+0x2\n')
        self.write(shared, 'pointer  refs  0x00000000  cell.halves[1]\n')
        self.write(other, DECLARATIONS.replace('short halves[2]', 'int halves[2]') +
                   'struct refs refs = {\n#include "other/refs.inc"\n};\n')
        self.assertIn('layouts/cells.bindings:1: cell.halves[1] lies 0x2 bytes past cell in src/rows.c, 0x4 in '
                      'src/other.c', self.relocated('include  cells\n', shared))
        self.write('offsets/src/other.bindings', '')
        self.write(self.table, '')
        status, output = run(relocate.main, str(shared))
        self.assertEqual(status, 1)
        self.assertIn("cell.halves[1]: no source's table includes this one", output)

    def test_a_sources_own_table_reads_no_other_sources_includes(self):
        with mock.patch.object(relocate, 'dependencies', side_effect=AssertionError('an include was read')):
            self.assertEqual(self.relocated('pointer  refs  0x00000000  mixed.count\n', self.table),
                             'pointer  refs  0x00000000  mixed+0x4\n')

    def test_a_path_the_declarations_dont_hold_fails_and_changes_no_table(self):
        other = Path('offsets/src/other.bindings')
        self.write('src/other.c', DECLARATIONS + 'struct refs refs = {\n#include "other/refs.inc"\n};\n')
        for row, error in (('mixed.missing', 'mixed.missing has no declared member missing'),
                           ('frames.id', 'frames.id has no declared member id'),
                           ('rows[3][0]', 'rows[3][0] indexes outside its declared array'),
                           ('frames[0].id[0]', 'frames[0].id[0] indexes outside its declared array'),
                           ('absent.count', 'no declaration gives absent')):
            with self.subTest(row=row):
                self.write(other, 'pointer  refs  0x00000000  &mixed\n')
                self.assertIn(f'offsets/src/rows.bindings:1: {error}',
                              self.relocated(f'pointer  refs  0x00000000  {row}\n'))
                self.assertEqual(self.table.read_text(), f'pointer  refs  0x00000000  {row}\n')
                self.assertEqual(other.read_text(), 'pointer  refs  0x00000000  &mixed\n')

    def test_a_table_outside_offsets_and_layouts_is_refused(self):
        self.write('config/rows.bindings', '')
        status, output = run(relocate.main, 'config/rows.bindings')
        self.assertEqual(status, 2)
        self.assertIn('config/rows.bindings: not a .bindings file of offsets/ or layouts/', output)
        # a shared table spelled through offsets/ would be taken for a source's own, and go with no row left
        self.write('layouts/nulls.bindings', '# null fields\n')
        status, output = run(relocate.main, 'offsets/../layouts/nulls.bindings')
        self.assertEqual(status, 2)
        self.assertIn('offsets/../layouts/nulls.bindings: not a .bindings file of offsets/ or layouts/', output)
        self.assertEqual(Path('layouts/nulls.bindings').read_text(), '# null fields\n')


class TheTree(TreeTest):
    def test_every_pointer_row_of_the_tree_is_a_relocation(self):
        def unread():
            raise AssertionError('a member path needs no declaration here')

        tables = sorted([*(ROOT / 'offsets').rglob('*.bindings'), *(ROOT / 'layouts').glob('*.bindings')])
        self.assertTrue(tables)
        for table in tables:
            with self.subTest(table=table.relative_to(ROOT)):
                self.assertEqual(relocate.rewritten(table, unread), (table.read_text(), True))
