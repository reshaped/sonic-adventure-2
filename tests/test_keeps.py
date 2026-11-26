"""The keeps of the tree's C, their classes, and taking classes of them out."""

from pathlib import Path

import keeps
from support import TreeTest, run

# a source with a keep of each form and class but mixed: a guard over a placement, a macro over two data that only
# the pointer rows of a body draw() names reach, a guard over a datum only an if (0) names, and a twin
SOURCE = """\
#include "shared.h"
#ifdef NONMATCHING
#define KEPT __attribute__((used))
#else
#define KEPT
#endif
#ifdef NONMATCHING
__attribute__((used, section(".head")))
#endif
static int head = 0;
KEPT
static int model[2] = {
#include "a/model.inc"
};
KEPT
static int *object[1] = {
#include "a/object.inc"
};
static int **table[1] = {
#include "a/table.inc"
};
/* the readout's */
#ifdef NONMATCHING
__attribute__((used))
#endif
static int readout = 0;
#ifdef NONMATCHING
__attribute__((used)) static const char caption[] =
    "caption";
#endif
int draw(void)
{
    if (0) {
        return readout;
    }
    return **table[0] + shared;
}
"""
BINDINGS = """\
pointer  object  0x00000000  model
pointer  table  0x00000000  object
"""
# a header two sources include, whose datum one of them reaches
SHARED = """\
#ifdef NONMATCHING
__attribute__((used))
#endif
static int shared = 1;
"""
SPLITS = """\
file SONIC2/STG.PRS
P 0x8c500000 0x8c500100
modules/a.c P 0x8c500000 0x8c500080
modules/b.c P 0x8c500080 0x8c500100
"""


class Keeps(TreeTest):
    def setUp(self):
        super().setUp()
        self.image('stg', SPLITS)
        self.write('modules/a.c', SOURCE)
        self.write('offsets/modules/a.bindings', BINDINGS)
        self.write('modules/shared.h', SHARED)
        self.write('modules/b.c', '#include "shared.h"\n')

    def test_classes_each_keep_by_what_reaches_its_datum(self):
        self.assertEqual([(each.file.as_posix(), each.line, kind, each.form, data)
                          for each, kind, data in keeps.classify()],
                         [('modules/a.c', 8, 'placed', 'guard', ['head']),
                          ('modules/a.c', 11, 'reached', 'macro', ['model']),
                          ('modules/a.c', 15, 'reached', 'macro', ['object']),
                          ('modules/a.c', 24, 'unreached', 'guard', ['readout']),
                          ('modules/a.c', 28, 'twin', 'twin', ['caption']),
                          ('modules/shared.h', 2, 'mixed', 'guard', ['shared'])])

    def test_classes_a_datum_by_the_side_of_each_guard_gcc_compiles(self):
        self.write('modules/b.c', '#ifdef NONMATCHING\n__attribute__((used))\n#endif\nstatic int ported = 0;\n'
                   '#ifdef NONMATCHING\n__attribute__((used))\n#endif\nstatic int original = 0;\n'
                   'int step(void)\n{\n#ifdef NONMATCHING\n    return ported;\n#else\n    return original;\n'
                   '#endif\n}\n')
        self.assertEqual([(each.line, kind, data) for each, kind, data in keeps.classify()
                          if each.file.as_posix() == 'modules/b.c'],
                         [(2, 'reached', ['ported']), (6, 'unreached', ['original'])])

    def test_reads_a_macro_after_its_declarator(self):
        # as src/system/loader.h's HEAD_SECTION stands in each overlay's head
        self.write('modules/b.c', '#ifdef NONMATCHING\n#define HEAD __attribute__((used, section(".head")))\n#else\n'
                   '#define HEAD\n#endif\nstatic int b_head HEAD = 0;\n')
        self.assertEqual([(each.line, kind, each.form, data) for each, kind, data in keeps.classify()
                          if each.file.as_posix() == 'modules/b.c'],
                         [(6, 'placed', 'macro', ['b_head'])])

    def test_fails_on_a_keep_whose_declaration_no_source_reads(self):
        self.write('modules/unread.h', SHARED)
        status, output = run(keeps.main)
        self.assertEqual((status, output.splitlines()[-1].partition(': ')[2]),
                         (1, 'modules/unread.h:2: a keep whose declaration no source reads'))

    def test_drops_each_keep_of_a_class_and_each_guard_it_leaves_empty(self):
        self.assertEqual(run(keeps.main, '--drop', 'unreached', 'twin'), (0, ''))
        lines = SOURCE.split('\n')
        # the readout's guard and the twin's whole, and the comment over the readout's guard stays
        self.assertEqual(Path('modules/a.c').read_text(), '\n'.join(lines[:22] + lines[25:26] + lines[30:]))
        self.assertEqual(run(keeps.main, '--drop', 'reached'), (0, ''))
        # the macro's uses, and its definitions stay
        self.assertEqual(Path('modules/a.c').read_text(),
                         '\n'.join(lines[:10] + lines[11:14] + lines[15:22] + lines[25:26] + lines[30:]))
        self.assertEqual(Path('modules/shared.h').read_text(), SHARED)

    def test_drops_no_keep_that_shares_a_line_with_other_c(self):
        self.write('modules/b.c', '#ifdef NONMATCHING\n__attribute__((used)) static int one = 1; static int two;\n'
                   '#endif\n')
        status, output = run(keeps.main, '--drop', 'twin')
        self.assertEqual((status, output.splitlines()[-1].partition(': ')[2]),
                         (1, 'modules/b.c:2: a keep that shares a line with other C or with part of a comment'))

    def test_takes_a_twin_to_the_semicolon_that_ends_its_declaration(self):
        twin = '#ifdef NONMATCHING\n__attribute__((used))\nstatic struct {\n    int n;\n} copy = {1};\n#endif\n'
        self.write('modules/b.c', twin + 'int b = 0;\n')
        self.assertEqual(run(keeps.main, '--drop', 'twin'), (0, ''))
        self.assertEqual(Path('modules/b.c').read_text(), 'int b = 0;\n')

    def test_takes_no_twin_past_its_guard(self):
        # the attribute's line alone, as a guard's, and the declaration's static stays
        self.write('modules/b.c', '#ifdef NONMATCHING\n__attribute__((used))\nstatic\n#endif\nint copy;\n'
                   'int *b(void)\n{\n    return &copy;\n}\n')
        self.assertEqual([(each.line, kind, each.form) for each, kind, _ in keeps.classify()
                          if each.file.as_posix() == 'modules/b.c'], [(2, 'reached', 'guard')])

    def test_reads_no_attribute_inside_another_attribute_s_arguments(self):
        self.write('modules/b.c', 'enum { used = 32 };\n#ifdef NONMATCHING\n__attribute__((aligned(used)))\n'
                   '__attribute__((used, aligned(32)))\n#endif\nstatic int b = 0;\n'
                   '#ifdef NONMATCHING\n#define LINKED __attribute__((used)) static\n#else\n#define LINKED\n#endif\n'
                   'LINKED\nint c;\n')
        self.assertEqual([each for each, _, _ in keeps.classify() if each.file.as_posix() == 'modules/b.c'], [])

    def test_fails_on_a_keep_macro_defined_with_a_section_and_without(self):
        self.write('modules/b.c', '#ifdef NONMATCHING\n#define KEPT __attribute__((used, section(".head")))\n#endif\n')
        status, output = run(keeps.main)
        self.assertEqual((status, output.splitlines()[-1].partition(': ')[2]),
                         (1, 'modules/b.c:2: KEPT, a keep macro another file defines without a section'))

    def test_drops_no_keep_that_cuts_a_comment(self):
        self.write('modules/b.c', '#ifdef NONMATCHING\n#define HOLD __attribute__((used))\n#else\n#define HOLD\n'
                   '#endif\nHOLD /* the reason\n   it holds */\nstatic int b = 0;\n')
        status, output = run(keeps.main, '--drop', 'unreached')
        self.assertEqual((status, output.splitlines()[-1].partition(': ')[2]),
                         (1, 'modules/b.c:6: a keep that shares a line with other C or with part of a comment'))

    def test_fails_on_a_keep_over_a_function(self):
        self.write('modules/b.c', '#ifdef NONMATCHING\n__attribute__((used))\n#endif\nstatic void hook(void)\n{\n}\n')
        status, output = run(keeps.main, '--drop', 'unreached')
        self.assertEqual((status, output.splitlines()[-1].partition(': ')[2]),
                         (1, 'modules/b.c:2: a keep whose declaration declares no datum'))

    def test_drops_a_guard_s_directives_and_no_guard_it_left_alone(self):
        self.write('modules/b.c', '#ifdef NONMATCHING\n#endif\n#ifdef NONMATCHING\n/* why */\n__attribute__((used))\n'
                   '#endif\nstatic int b = 0;\n')
        self.assertEqual(run(keeps.main, '--drop', 'unreached'), (0, ''))
        self.assertEqual(Path('modules/b.c').read_text(), '#ifdef NONMATCHING\n#endif\n/* why */\nstatic int b = 0;\n')

    def test_calls_a_keep_mixed_whose_data_are_reached_in_part(self):
        self.write('modules/b.c', '#ifdef NONMATCHING\n__attribute__((used))\n#endif\nstatic int b, c;\n'
                   'int step(void)\n{\n    return b;\n}\n')
        self.assertEqual([(each.line, kind, data) for each, kind, data in keeps.classify()
                          if each.file.as_posix() == 'modules/b.c'], [(2, 'mixed', ['b', 'c'])])
