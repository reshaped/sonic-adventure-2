"""The turn windows of the game's C: where each opens and closes, its shape
and marks, the summary of them, and the check of a guarded window's kernel
call."""

import subprocess
import sys

import windows
from support import ROOT, TreeTest, run

# a source with a window of each kind: one run of statements, spread,
# calling, branching, opened by matrix_push_unit, reloading the unit
# matrix, a stray unit load, a window no pop closes, and the same window on
# a guard's non-matching side, which SHC never reads. nested's windows push
# the unit matrix by njPushMatrix, and push again inside: by njPushMatrix
# of another matrix, popped alone, and by njPushMatrixEx, popped with the
# window by njPopMatrix(2)
SOURCE = """\
void run(Rotation *ang, NJS_VECTOR *v)
{
    njPushMatrixEx();
    njUnitMatrix(NULL);
    njRotateZ(NULL, ang->z);
    njRotateX(NULL, ang->x);
    njRotateY(NULL, -ang->y);
    njCalcVector(NULL, v, v);
    njPopMatrixEx();
}

void spread(NJS_VECTOR *v)
{
    njPushMatrix(NULL);
    njUnitMatrix(NULL);
    njRotateY(NULL, 0x4000);
    v->x = 1.0F;
    njCalcVector(NULL, v, v);
    njPopMatrix(1);
}

void drawn(NJS_POINT3 *pos, Angle y, NJS_POINT3 *at)
{
    matrix_push_unit();
    njTranslateV(NULL, pos);
    njRotateY(NULL, y);
    if (at != NULL) {
        njCalcPoint(NULL, at, at);
    }
    njCnkDrawObject(object(y));
    njPopMatrixEx();
}

void twice(NJS_VECTOR *v)
{
    njPushMatrixEx();
    njUnitMatrix(NULL);
    njRotateX(NULL, 1);
    njCalcVector(NULL, v, v);
    njUnitMatrix(NULL);
    njScale(NULL, 2.0F, 2.0F, 2.0F);
    njCalcVector(NULL, v, v);
    njPopMatrixEx();
    njUnitMatrix(NULL);
}

void open(void)
{
    njPushMatrixEx();
    njUnitMatrix(0);
#ifdef NONMATCHING
    njPushMatrixEx();
    njUnitMatrix(NULL);
    njPopMatrixEx();
#endif
}

void nested(NJS_POINT3 *p, NJS_MATRIX *m)
{
    njPushMatrix(&_nj_unit_matrix_);
    njRotateY(NULL, 0x4000);
    njPushMatrix(m);
    njCalcPoint(NULL, p, p);
    njPopMatrix(1);
    njCalcPoint(NULL, p, p);
    njPopMatrix(1);
    njPushMatrix(&_nj_unit_matrix_);
    njPushMatrixEx();
    njRotateZ(NULL, 1);
    njCalcVector(NULL, p, p);
    njPopMatrix(2);
}
"""

# a window that a guard replaces by its kernel's call, which takes the
# window's angles and vectors
GUARDED = """\
void turn(struct task_work *work, NJS_VECTOR *v, NJS_POINT3 *p)
{
#ifdef NONMATCHING
    /* keeps the shipped bits */
    zam_vec3_turn_yxz(work->ang.x, -work->ang.y, work->ang.z, &v->x, &p->x);
#else
    matrix_push_unit();
    njRotateZ(NULL, work->ang.z);
    njRotateX(NULL, work->ang.x);
    njRotateY(NULL, -work->ang.y);
    njCalcVector(NULL, v, p);
    njPopMatrixEx();
#endif
}
"""

# a window of one turn, which a guard replaces by its kernel's call
TURN = """\
void turn(Angle a, NJS_VECTOR *v, NJS_VECTOR *w)
{
#ifdef NONMATCHING
    zam_vec3_turn_x(a, &v->x, &w->x);
#else
    matrix_push_unit();
    njRotateX(NULL, a);
    njCalcVector(NULL, v, w);
    njPopMatrixEx();
#endif
}
"""
# TURN's guard written the other way round: the matching side first
INVERSE = TURN.replace('#ifdef NONMATCHING\n    zam_vec3_turn_x(a, &v->x, &w->x);\n#else\n', '#ifndef NONMATCHING\n').replace(
    '#endif', '#else\n    zam_vec3_turn_x(a, &v->x, &w->x);\n#endif')


def problems(text):
    """--check's lines for text's guarded windows."""
    return [line for window in windows.windows('a.c', text) if 'guarded' in window.marks
            for line in windows.problems(window)]


class Windows(TreeTest):
    def found(self, text):
        return {window.function: window for window in windows.windows('a.c', text)}

    def test_classes_each_window_by_its_steps_and_marks(self):
        found = windows.windows('a.c', SOURCE)
        self.assertEqual([(window.function, window.line, window.opener, window.shape, sorted(window.marks),
                           window.closed) for window in found], [
            ('run', 3, 'njPushMatrixEx()', 'Z X Y vector', [], True),
            ('spread', 14, 'njPushMatrix(NULL)', 'Y vector', ['spread'], True),
            ('drawn', 24, 'matrix_push_unit()', 'T Y point', ['branches', 'calls', 'spread'], True),
            ('twice', 36, 'njPushMatrixEx()', 'X vector U S vector', [], True),
            ('twice', 44, 'njUnitMatrix(NULL)', '', [], False),
            ('open', 49, 'njPushMatrixEx()', '', [], False),
            ('nested', 60, 'njPushMatrix(&_nj_unit_matrix_)', 'Y point point', ['calls'], True),
            ('nested', 67, 'njPushMatrix(&_nj_unit_matrix_)', 'Z vector', ['calls'], True)])

    def test_counts_each_push_and_pop_toward_the_depth(self):
        # the push of another matrix and its pop lie inside the first window, and the second closes with the pop
        # of two levels
        first, second = (window for window in windows.windows('a.c', SOURCE) if window.function == 'nested')
        self.assertEqual([str(call) for _, call in first.steps], [
            'njRotateY(NULL, 0x4000)', 'njPushMatrix(m)', 'njCalcPoint(NULL, p, p)', 'njPopMatrix(1)',
            'njCalcPoint(NULL, p, p)'])
        self.assertEqual([str(call) for _, call in second.steps], [
            'njPushMatrixEx()', 'njRotateZ(NULL, 1)', 'njCalcVector(NULL, p, p)'])

    def test_keeps_each_guards_two_sides_by_the_lines_of_its_matching_side(self):
        text = '#ifdef NONMATCHING\na();\n#else\nb();\nc();\n#endif\n#ifdef NONMATCHING\n#else\nd();\n#endif\n'
        self.assertEqual(windows.matching_side(text)[1], {4: (0, 'a();', 'b();\nc();'), 5: (0, 'a();', 'b();\nc();'),
                                                          9: (1, '', 'd();')})

    def test_names_the_kernel_of_a_shape_that_takes_one(self):
        found = self.found(SOURCE)
        self.assertEqual(found['run'].kernel, 'zam_vec3_turn_yxz')
        self.assertTrue(found['run'].taken)
        self.assertEqual(found['spread'].kernel, 'zam_vec3_turn_y')
        self.assertFalse(found['spread'].taken)
        self.assertIsNone(found['twice'].kernel)

    def test_keeps_each_steps_call_as_written(self):
        steps = self.found(SOURCE)['drawn'].steps
        self.assertEqual([(letters, str(call)) for letters, call in steps], [
            ('T', 'njTranslateV(NULL, pos)'), ('Y', 'njRotateY(NULL, y)'), ('point', 'njCalcPoint(NULL, at, at)'),
            ('call', 'njCnkDrawObject(object(y))')])

    def test_reads_a_guarded_window_from_its_matching_side(self):
        window = self.found(GUARDED)['turn']
        self.assertEqual((window.shape, window.marks), ('Z X Y vector', {'guarded'}))
        self.assertIn('zam_vec3_turn_yxz(work->ang.x', window.replaced)

    def test_gives_the_first_float_of_each_spelling_of_a_vector(self):
        self.assertEqual(windows.first_floats('&work->pos'), {'&work->pos.x'})
        self.assertEqual(windows.first_floats('pos'), {'&pos->x'})
        self.assertEqual(windows.first_floats('(NJS_POINT3 *)object->pos'), {'object->pos'})
        self.assertEqual(windows.first_floats('points + i'), {'&(points + i)->x'})
        # a cast binds tighter than +, and - is no member
        self.assertEqual(windows.first_floats('(V *)p + 1'), {'&((V *)p + 1)->x'})
        self.assertEqual(windows.first_floats('p-1'), {'&(p-1)->x'})

    def test_reads_an_integer_literal_as_its_count_of_levels(self):
        for count, levels in (('2', 2), ('2U', 2), ('0x2', 2), ('010', 8), ('PUSHES', 1)):
            self.assertEqual(windows.pops(windows.Call('njPopMatrix', [count], 1, 0, 0)), levels)

    def test_a_pop_past_the_windows_depth_closes_none(self):
        window, = windows.windows('a.c', TURN.replace('njPopMatrixEx()', 'njPopMatrix(2)'))
        self.assertFalse(window.closed)

    def test_lists_a_window_inside_another(self):
        outer = TURN.replace('#ifdef', '    matrix_push_unit();\n#ifdef').replace('#endif', '#endif\n    njPopMatrixEx();')
        self.assertEqual([(window.line, window.shape, window.pop.line) for window in windows.windows('a.c', outer)],
                         [(3, 'X vector', 12), (7, 'X vector', 10)])

    def test_reads_a_directives_continued_lines_as_the_directive(self):
        text = TURN.replace('{\n', '{\n#define UNUSED() \\\n    matrix_push_unit(); njPopMatrixEx();\n')
        self.assertEqual([window.line for window in windows.windows('a.c', text)], [8])

    def test_reads_each_spelling_of_a_guard_alike(self):
        self.assertEqual(self.found(INVERSE)['turn'].marks, {'guarded'})
        self.assertEqual(problems(INVERSE), [])
        for spelled in ('#if defined(NONMATCHING)', '#if defined NONMATCHING'):
            self.assertEqual(problems(TURN.replace('#ifdef NONMATCHING', spelled)), [])
        self.assertEqual(problems(INVERSE.replace('#ifndef NONMATCHING', '#if !defined(NONMATCHING)')), [])


class Check(TreeTest):
    def test_passes_a_call_that_takes_the_windows_angles_and_vectors(self):
        self.write('a.c', GUARDED)
        self.assertEqual(run(windows.main, '--check', 'a.c'), (0, ''))

    def test_fails_each_argument_the_window_does_not_give(self):
        self.write('a.c', GUARDED.replace('-work->ang.y, work->ang.z, &v->x, &p->x',
                                          'work->ang.y, work->ang.z, v, &p->x'))
        status, said = run(windows.main, '--check', 'a.c')
        self.assertEqual(status, 1)
        self.assertEqual([line.split(': ', 1)[1] for line in said.splitlines()], [
            "a.c:7: turn: zam_vec3_turn_yxz's argument 2 is work->ang.y, where the window gives -work->ang.y",
            "a.c:7: turn: zam_vec3_turn_yxz's argument 4 is v, where the window gives &v->x"])

    def test_fails_another_kernel_and_a_window_no_kernel_takes(self):
        self.write('a.c', GUARDED.replace('zam_vec3_turn_yxz', 'zam_vec3_turn_zxy'))
        status, said = run(windows.main, '--check', 'a.c')
        self.assertEqual((status, said.split(': ', 1)[1]),
                         (1, "a.c:7: turn: the guard's other side must be one call of zam_vec3_turn_yxz, and no more"))
        unexact = "a.c:7: turn: the guard's matching side must be the window, from its push to its pop, and no more"
        for moved in ('#endif\n    njPopMatrixEx();', '    njPopMatrixEx();\n    p->y = 2.0F;\n#endif'):
            self.write('a.c', GUARDED.replace('    njPopMatrixEx();\n#endif', moved))
            status, said = run(windows.main, '--check', 'a.c')
            self.assertEqual((status, said.split(': ', 1)[1]), (1, unexact))
        self.write('a.c', GUARDED.replace('#else\n', '#else\n    p->y = 2.0F;\n'))
        status, said = run(windows.main, '--check', 'a.c')
        self.assertEqual((status, said.split(': ', 1)[1]), (1, unexact.replace(':7:', ':8:')))
        self.write('a.c', GUARDED.replace('    njRotateY(NULL, -work->ang.y);\n', '    njRotateY(NULL, -work->ang.y);\n'
                                          '    njCalcVector(NULL, v, p);\n'))
        status, said = run(windows.main, '--check', 'a.c')
        self.assertEqual((status, said.split(': ', 1)[1]), (1, 'a.c:7: turn: a guard holds a window whose shape and '
                                                               'marks take no kernel: Z X Y vector vector'))

    def test_fails_a_call_that_is_not_its_own_statement(self):
        unlike = "a.c:{}: turn: the guard's other side must be one call of zam_vec3_turn_x, and no more"
        for line, other in ((6, '    if (0) zam_vec3_turn_x(a, &v->x, &w->x);\n'),
                            (7, '    zam_vec3_turn_x(a, &v->x, &w->x);\n    ++ticks;\n'),
                            (8, '#if 0\n    zam_vec3_turn_x(a, &v->x, &w->x);\n#endif\n'),
                            (8, '    zam_vec3_turn_x(a, &v->x, &w->x);\n#elif OTHER\n    wrong();\n')):
            self.assertEqual(problems(TURN.replace('    zam_vec3_turn_x(a, &v->x, &w->x);\n', other)),
                             [unlike.format(line)])
        self.assertEqual(problems(INVERSE.replace('zam_vec3_turn_x(a, &v->x, &w->x)', 'wrong()')),
                         [unlike.format(4)])

    def test_checks_a_guarded_window_inside_another(self):
        outer = TURN.replace('#ifdef', '    matrix_push_unit();\n#ifdef').replace('#endif', '#endif\n    njPopMatrixEx();')
        self.assertEqual(problems(outer), [])
        self.assertEqual(problems(outer.replace('zam_vec3_turn_x(a, &v->x, &w->x)', 'wrong()')), [
            "a.c:7: turn: the guard's other side must be one call of zam_vec3_turn_x, and no more"])

    def test_fails_a_pop_it_cannot_count_or_that_pops_past_the_window(self):
        self.assertEqual(problems(TURN.replace('njPopMatrixEx()', 'njPopMatrix(1U)')), [])
        self.assertEqual(problems(TURN.replace('njPopMatrixEx()', 'njPopMatrix(PUSHES)')), [
            'a.c:6: turn: njPopMatrix(PUSHES) pops levels that no number counts, so the window may close elsewhere'])
        self.assertEqual(problems(TURN.replace('njPopMatrixEx()', 'njPopMatrix(2)')), [
            'a.c:6: turn: a guard holds a window that no pop takes back to the depth it opened at'])

    def test_compares_each_argument_with_its_literals(self):
        self.assertEqual(problems(TURN.replace('(NULL, a)', "(NULL, angles[')'])").replace('(a,', "(angles[')'],")), [])
        self.assertEqual(problems(TURN.replace('(NULL, a)', "(NULL, angles['A'])").replace('(a,', "(angles['B'],")), [
            "a.c:6: turn: zam_vec3_turn_x's argument 1 is angles['B'], where the window gives angles['A']"])

    def test_fails_a_step_whose_arguments_call_or_change_something(self):
        for angle in ('advance()', 'a[i++]'):
            self.assertEqual(problems(TURN.replace('(NULL, a)', f'(NULL, {angle})').replace('(a,', f'({angle},')), [
                f'a.c:6: turn: njRotateX(NULL, {angle}) calls or changes something in its arguments, which its '
                'kernel may run in another order'])

    def test_fails_a_step_short_of_the_arguments_it_compares(self):
        self.assertEqual(problems(TURN.replace('njRotateX(NULL, a)', 'njRotateX(NULL)')), [
            'a.c:6: turn: njRotateX(NULL), on line 7, takes too few arguments for its step'])
        self.assertEqual(problems(TURN.replace('njCalcVector(NULL, v, w)', 'njCalcVector(NULL, v)')), [
            'a.c:6: turn: njCalcVector(NULL, v), on line 8, takes too few arguments for its step'])

    def test_reads_the_pops_own_semicolon(self):
        for spelled in ('njPopMatrixEx() ;', 'njPopMatrixEx() /* done */;'):
            self.assertEqual(problems(TURN.replace('njPopMatrixEx();', spelled)), [])
        self.assertEqual(problems(TURN.replace('njPopMatrixEx();', 'njPopMatrixEx(); ++ticks;')), [
            "a.c:6: turn: the guard's matching side must be the window, from its push to its pop, and no more"])


class Summary(TreeTest):
    def test_counts_each_shape_and_the_sources_that_hold_most(self):
        self.write('src/a.c', SOURCE)
        self.write('modules/b.c', GUARDED)
        self.assertEqual(run(windows.main), (0, """\
shape                windows  sources  calls  spread  branches  guarded  kernel
Z X Y vector               2        2      0       0         0        1  zam_vec3_turn_yxz
T Y point                  1        1      1       1         1        0  zam_vec3_place_y
X vector U S vector        1        1      0       0         0        0  -
Y point point              1        1      1       0         0        0  -
Y vector                   1        1      0       1         0        0  zam_vec3_turn_y
Z vector                   1        1      1       0         0        0  zam_vec3_turn_z
7 windows in 2 sources. 5 take a kernel's shape, 2 of them in one run of statements, and 1 of those are guarded
2 more, which --list names: an njUnitMatrix(NULL) that opens no window, or a window that no pop closes

sources with the most windows
      6  src/a.c
      1  modules/b.c
"""))

    def test_lists_each_window_on_a_line(self):
        self.write('a.c', SOURCE)
        status, said = run(windows.main, '--list', 'a.c')
        self.assertEqual(said.splitlines()[1:3], [
            'a.c:14: spread: njPushMatrix(NULL): Y vector [spread]: Y njRotateY(NULL, 0x4000); '
            'vector njCalcVector(NULL, v, v)',
            'a.c:24: drawn: matrix_push_unit(): T Y point [calls] [spread] [branches]: T njTranslateV(NULL, pos); '
            'Y njRotateY(NULL, y); point njCalcPoint(NULL, at, at); call njCnkDrawObject(object(y))'])
        self.assertEqual(said.splitlines()[4:7], [
            'a.c:44: twice: njUnitMatrix(NULL) opens no window',
            'a.c:49: open: njPushMatrixEx(): (none), which no pop closes: ',
            'a.c:60: nested: njPushMatrix(&_nj_unit_matrix_): Y point point [calls]: Y njRotateY(NULL, 0x4000); '
            'call njPushMatrix(m); point njCalcPoint(NULL, p, p); call njPopMatrix(1); point njCalcPoint(NULL, p, p)'])


class Tree(TreeTest):
    def test_each_guarded_window_of_the_tree_calls_its_kernel_as_the_form_says(self):
        checked = subprocess.run([sys.executable, str(ROOT / 'tools/windows.py'), '--check'], cwd=ROOT,
                                 capture_output=True, text=True, check=False)
        self.assertEqual((checked.returncode, checked.stderr), (0, ''))
