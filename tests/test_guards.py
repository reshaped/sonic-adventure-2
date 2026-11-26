"""The guards' two switches, the one form a guard takes, and the comment
a non-matching side that holds code gives."""

import os
import subprocess
import sys
import unittest
from unittest import mock

import guards
from support import ROOT, TreeTest, run

# guards in every form that passes: each switch by #ifdef, with and without
# an #else, one inside the other, each non-matching side of code with its
# comment, and conditionals that name no switch
SOUND = """\
#ifndef A_H
#define A_H
#ifdef NONMATCHING
/* GCC reads an empty list as no parameters */
int f(void);
#else
int f();
#endif
#ifdef DREAMCAST
#ifdef NONMATCHING
#define sqrtf __builtin_sqrtf /* FSQRT */
#endif
#else
float sqrtf(float x);
#endif
#if defined(OTHER) || VERSION > 2
#elif VERSION
#endif
#endif
"""
# a guard whose non-matching side holds code and no comment
BARE = '#ifdef NONMATCHING\nint f(void);\n#else\nint f();\n#endif\n'
# what the problem with such a guard says after its place
PROBLEM = '#ifdef NONMATCHING whose side holds code and no reason: a comment on the side says why it differs'


class Directives(TreeTest):
    def test_reads_each_directive_with_its_line_and_names(self):
        text = '  #  ifdef DREAMCAST /* #else */\n#define TWO(a, b) \\\n    ((a) + (b))\n#endif\n'
        self.assertEqual(guards.directives(text), [(1, 'ifdef', ['DREAMCAST']), (2, 'define', ['TWO', 'a', 'b', 'a', 'b']),
                                                   (4, 'endif', [])])

    def test_reads_none_in_a_comment_or_a_literal(self):
        text = '/*\n#ifndef DREAMCAST\n*/\nchar *s = "\\n#if GCC";\n        MOV #3,R3 ; it\'s 3\n'
        self.assertEqual(guards.directives(text), [])

    def test_physical_lines_and_line_splicing_precede_comment_and_name_recognition(self):
        for newline in ('\n', '\r\n'):
            text = newline.join(['// continue the comment \\', '#if NONMATCHING', '/* leading */ #de\\',
                                 'fine NONMA\\', 'TCHING 1', 'const char *s = "not a directive \\',
                                 '#if GCC";', '#if PORT', '#endif', ''])
            with self.subTest(newline=newline):
                self.assertEqual(guards.directives(text), [(3, 'define', ['NONMATCHING']), (8, 'if', ['PORT']),
                                                           (9, 'endif', [])])
                self.assertEqual(guards.problems('a.c', text), [
                    "a.c:3: #define of NONMATCHING: only a build's flags define it",
                    'a.c:8: #if of PORT: a build is told apart by NONMATCHING or DREAMCAST'])


class Problems(TreeTest):
    def test_passes_each_sound_form(self):
        self.assertEqual(guards.problems('a.h', SOUND), [])

    def test_a_switch_takes_an_ifdef_alone(self):
        rule = 'a switch is tested by #ifdef alone'
        text = ('#ifndef DREAMCAST\n#endif\n#if defined(NONMATCHING)\n#elif DREAMCAST && NONMATCHING\n#endif\n'
                '#ifdef OTHER\n#elifdef DREAMCAST\n#endif\n')
        self.assertEqual(guards.problems('a.c', text), [
            f'a.c:1: #ifndef of DREAMCAST: {rule}', f'a.c:3: #if of NONMATCHING: {rule}',
            f'a.c:4: #elif of DREAMCAST, NONMATCHING: {rule}', f'a.c:7: #elifdef of DREAMCAST: {rule}'])

    def test_a_switchs_one_other_group_is_its_else(self):
        text = '#ifdef NONMATCHING\n#ifdef OTHER\n#elif VERSION\n#endif\n#elif VERSION\n#else\n#endif\n'
        self.assertEqual(guards.problems('a.c', text),
                         ['a.c:5: #elif in an #ifdef of NONMATCHING, whose one other group is #else'])

    def test_a_guard_names_no_compiler_and_no_other_purpose(self):
        rule = 'a build is told apart by NONMATCHING or DREAMCAST'
        text = '#ifdef __clang__\n#endif\n#if defined(GCC) || PORT\n#endif\n#ifndef MATCHING\n#endif\n#ifdef PORT_H\n#endif\n'
        self.assertEqual(guards.problems('a.c', text), [
            f'a.c:1: #ifdef of __clang__: {rule}', f'a.c:3: #if of GCC, PORT: {rule}',
            f'a.c:5: #ifndef of MATCHING: {rule}'])

    def test_only_a_builds_flags_define_a_switch(self):
        text = '#define DREAMCAST 1\n#undef NONMATCHING\n#define DREAMCAST_H\n'
        self.assertEqual(guards.problems('a.c', text), [
            "a.c:1: #define of DREAMCAST: only a build's flags define it",
            "a.c:2: #undef of NONMATCHING: only a build's flags define it"])


class Unexplained(TreeTest):
    def test_finds_a_side_that_holds_code_and_no_comment(self):
        self.assertEqual(guards.unexplained(BARE), [1])
        self.assertEqual(guards.unexplained('int x;\n' + BARE.replace('#else\nint f();\n', '')), [2])

    def test_a_comment_anywhere_on_the_side_gives_its_reason(self):
        for side in ('/* why */\nint f(void);\n', 'int f(void); // why\n', 'int f(void);\n/* why */\n'):
            with self.subTest(side=side):
                text = f'#ifdef NONMATCHING\n{side}#else\nint f();\n#endif\n'
                self.assertEqual(guards.unexplained(text), [])

    def test_a_comment_above_the_guard_or_on_its_else_gives_none(self):
        text = '/* why */\n' + BARE.replace('#else\n', '#else\n/* why */\n')
        self.assertEqual(guards.unexplained(text), [2])

    def test_an_empty_comment_a_tag_or_a_tools_control_gives_no_reason(self):
        for comment in ('/**/', '/* */', '//', '/* --- */', '/** @brief */', '/* @bug logic: the test is backwards */',
                        '// NOLINT(misc-redundant-expression)', '/* NOLINTNEXTLINE */', '/* NOLINT */',
                        '// clang-format off'):
            with self.subTest(comment=comment):
                self.assertEqual(guards.unexplained(BARE.replace('int f(void);', f'{comment}\nint f(void);')), [1])
        text = BARE.replace('int f(void);', '/* @bug logic: kept */\n/* GCC reads () as no parameters */\nint f(void);')
        self.assertEqual(guards.unexplained(text), [])
        text = BARE.replace('int f(void);', '/** SHC keeps this symbol without an attribute. */\nint f(void);')
        self.assertEqual(guards.unexplained(text), [])

    def test_a_directive_is_code(self):
        self.assertEqual(guards.unexplained('#ifdef NONMATCHING\n#include <string.h>\n#endif\n'), [1])

    def test_a_side_without_code_or_with_the_attribute_alone_needs_no_comment(self):
        text = ('#ifdef NONMATCHING\n#else\nint x;\n#endif\n'
                '#ifdef NONMATCHING\n__attribute__((used))\n#endif\nstatic int y;\n'
                '#ifdef NONMATCHING\n#define KEPT __attribute__((used))\n#else\n#define KEPT\n#endif\n'
                '#ifdef NONMATCHING\n#define HELD \\\n    __attribute__((used))\n#endif\n')
        self.assertEqual(guards.unexplained(text), [])

    def test_the_attribute_after_a_define_is_not_its_replacement(self):
        text = '#ifdef NONMATCHING\n#define FLAG\n__attribute__((used))\n#endif\nstatic int x;\n'
        self.assertEqual(guards.unexplained(text), [1])

    def test_the_attribute_beside_other_code_is_code(self):
        text = ('#ifdef NONMATCHING\n__attribute__((used)) static int x;\n#endif\n'
                '#ifdef NONMATCHING\n__attribute__((used, section("D")))\n#endif\nint y;\n')
        self.assertEqual(guards.unexplained(text), [1, 4])

    def test_a_later_group_ends_the_side(self):
        text = '#ifdef NONMATCHING\nint x;\n#elif VERSION\n/* not the side */\n#endif\n'
        self.assertEqual(guards.unexplained(text), [1])

    def test_a_comment_counts_for_each_side_that_holds_it(self):
        inner = '#ifdef NONMATCHING\nint f(void);\n#ifdef DREAMCAST\n/* why */\nint g(void);\n#endif\n#endif\n'
        self.assertEqual(guards.unexplained(inner), [])
        outer = '#ifdef NONMATCHING\n/* why */\n#ifdef NONMATCHING\nint g(void);\n#endif\n#endif\n'
        self.assertEqual(guards.unexplained(outer), [3])
        self.assertEqual(guards.unexplained('#ifdef DREAMCAST\n#ifdef NONMATCHING\nint f(void);\n#endif\n#endif\n'),
                         [2])

    def test_holds_the_explained_directories_alone(self):
        for directory in guards.EXPLAINED:
            with self.subTest(directory=directory):
                self.assertEqual(guards.problems(f'{directory}a.c', BARE), [f'{directory}a.c:1: {PROBLEM}'])
        self.assertEqual(guards.problems('tests/a.c', BARE), [])

    def test_skips_a_file_unexplained_names(self):
        path = f'{guards.EXPLAINED[0]}a.c'
        with mock.patch.object(guards, 'UNEXPLAINED', frozenset([path])):
            self.assertEqual(guards.problems(path, BARE), [])

    def test_reads_each_spelling_of_a_path_as_its_file(self):
        directory = guards.EXPLAINED[0]
        for spelling in (f'./{directory}a.c', f'{directory}./a.c', os.path.abspath(f'{directory}a.c')):
            with self.subTest(spelling=spelling):
                self.assertEqual(guards.problems(spelling, BARE), [f'{spelling}:1: {PROBLEM}'])
                with mock.patch.object(guards, 'UNEXPLAINED', frozenset([f'{directory}a.c'])):
                    self.assertEqual(guards.problems(spelling, BARE), [])


class TheTree(unittest.TestCase):
    def test_each_unexplained_file_lies_under_explained_and_still_fails_the_rule(self):
        for path in sorted(guards.UNEXPLAINED):
            with self.subTest(path=path):
                self.assertTrue(path.startswith(guards.EXPLAINED))
                self.assertTrue(guards.unexplained((ROOT / path).read_text(encoding='latin-1')))


class Main(TreeTest):
    def test_passes_sound_files_and_fails_each_problem_by_its_place(self):
        self.write('src/a.h', SOUND)
        self.assertEqual(run(guards.main, 'src/a.h'), (0, ''))
        self.write('src/a.c', '#ifndef __clang__\n#endif\n' + BARE)
        status, output = run(guards.main, 'src/a.h', 'src/a.c')
        self.assertEqual(status, 1)
        self.assertEqual([line.split(': ', 1)[1] for line in output.splitlines()],
                         ['src/a.c:1: #ifndef of __clang__: a build is told apart by NONMATCHING or DREAMCAST',
                          f'src/a.c:3: {PROBLEM}'])

    def test_helps_without_its_docstring(self):
        tools = os.path.dirname(guards.__file__)
        result = subprocess.run([sys.executable, '-OO', os.path.join(tools, 'guards.py'), '--help'],
                                capture_output=True, text=True, env={**os.environ, 'PYTHONPATH': tools})
        self.assertEqual(result.returncode, 0, result.stderr)
        self.assertIn('usage: guards', result.stdout)
