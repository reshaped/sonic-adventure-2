"""tools/floats.py as make dc runs it on a play source's preprocessed C: its command line, the lines it reads as
SHC's, its edits, its division by a constant under -aggressive=2, and GCC's code once it has run. The GCC tests
compile under config/gcc.mk's flags with the -ffp-contract=on dc.mk gives a play source, where SH_ELF names a
toolchain, and skip where it doesn't."""

import os
import re
import subprocess
import sys
import tempfile
import unittest
from pathlib import Path
from unittest import mock

import float_edits
import float_tokens
import floats
import sh4
from support import ROOT, SH_ELF, run
from test_float_fuse import AGGRESSIVE, GAME, SYSTEM, passed, scratch, squeezed

# what make reads to give a variable of config/gcc.mk's, and the two of the Makefile's macros it calls
FLAGS = """comma := ,
quoted = '$(subst ','\\\\'',$1)'
include config/gcc.mk
flags:
\t@printf '%s\\n' $({name})
"""
# the levels make dc compiles a play source at, after GCCFLAGS: -Os, GCC_HOT's -O2, and the -O0 of a source SHC
# builds under -optimize=0; and the contraction dc.mk gives a play source after GCCFLAGS' -ffp-contract=off
LEVELS = (['-Os'], ['-O2'], ['-Os', '-O0'])
FUSING = ['-ffp-contract=on']
# the SH-4's floating arithmetic: a sum, a difference, a product, a quotient, a multiply-add and a negation
ARITHMETIC = ('fadd', 'fsub', 'fmul', 'fdiv', 'fmac', 'fneg')
# the instructions that call, load a register from memory, and load and store a float, as GCC's listing spells each,
# with Rn and Rm
ACCESSES = {'jsr': r'jsr @(?P<n>r\d+)', 'mov.l @Rm,Rn': r'mov\.l @(?P<m>r\d+),(?P<n>r\d+)',
            'fmov.s @Rm,FRn': r'fmov\.s @(?P<m>r\d+),(?P<n>fr\d+)',
            'fmov.s FRm,@Rn': r'fmov\.s (?P<m>fr\d+),@(?P<n>r\d+)'}


def variable(name):
    """config/gcc.mk's variable name, as make gives it with SH_ELF naming the toolchain: one word to a flag."""
    env = {key: value for key, value in os.environ.items() if key not in ('MAKEFLAGS', 'MFLAGS', 'MAKELEVEL')}
    made = subprocess.run(['make', '-s', '--no-print-directory', '-f', '-', f'SH_ELF={SH_ELF}', 'flags'],
                          input=FLAGS.format(name=name), cwd=ROOT, env=env, capture_output=True, text=True)
    if made.returncode:
        raise AssertionError(f'make: config/gcc.mk gives no {name}:\n{made.stdout}{made.stderr}')
    return made.stdout.split()


def function(listing, name='f'):
    """The instructions of the function name in listing, GCC's assembly, in order, each its mnemonic and its
    operands one space apart."""
    lines = listing[listing.index(f'_{name}:\n'):].splitlines()[1:]
    return [' '.join(line.split()) for line in lines[:next((at for at, line in enumerate(lines) if
                                                            re.match(r'\s*\.size\s', line)), len(lines))]
            if line.startswith('\t') and not line.lstrip().startswith('.')]


def arithmetic(instructions):
    """The floating arithmetic among instructions, by mnemonic, in sorted order."""
    return sorted(name for name in (instruction.split()[0] for instruction in instructions) if name in ARITHMETIC)


def accesses(instructions):
    """The calls, loads and stores among instructions, in order, each ACCESSES' name with Rn and Rm, or None."""
    return [(name, match['n'], match.groupdict().get('m')) for instruction in instructions
            for name, form in ACCESSES.items() for match in [re.fullmatch(form, instruction)] if match]


class CommandLine(unittest.TestCase):
    def test_rewrites_in_place_and_fails_naming_the_tool(self):
        # and leaves the C of a source it fails on as it was
        good = 'float f(float x, float y)\n{\n    return y - x * y;\n}\n'
        bad = 'int f(int i)\n{\n    return i - z * i;\n}\n'
        with scratch({'a.c': good, 'b.c': bad, 'a.i': f'# 1 "a.c"\n{good}', 'b.i': f'# 1 "b.c"\n{bad}'}) as root:
            self.assertEqual(run(floats.main, '--shc', 'r13', '-fpu=single', '--system', SYSTEM[0], 'a.i'), (0, ''))
            self.assertIn('__floats_p', (root / 'a.i').read_text())
            status, said = run(floats.main, '--shc', 'r11', '-fpu=single', 'b.i')
            self.assertEqual(status, 1)
            self.assertRegex(said, r"\A[^\n]*: b\.c:3: i - z \* i: C the tool can't tell is floating")
            self.assertEqual((root / 'b.i').read_text(), f'# 1 "b.c"\n{bad}')

    def test_r08_preserves_integer_arithmetic_in_the_lcp_route(self):
        source = 'unsigned int words(unsigned int length) { return (length + 3U) / 4U; }\n'
        original = f'# 1 "a.c"\n{source}'
        with scratch({'a.c': source, 'a.i': original}) as root:
            self.assertEqual(run(floats.main, '--shc', 'r08', '-fpu=single', 'a.i'), (0, ''))
            self.assertEqual((root / 'a.i').read_text(), original)

    def test_takes_the_releases_and_the_flags_it_reads_alone(self):
        # and prints its usage for -h
        for argv in (['--shc', 'r32', 'a.i'], ['--shc', 'r13', '-fpu=double', 'a.i'], ['a.i']):
            with self.subTest(argv):
                self.assertEqual(run(floats.main, *argv)[0], 2)
        status, said = run(floats.main, '-h')
        self.assertEqual(status, 0)
        self.assertTrue(said.startswith('usage: floats --shc RELEASE'))

    def test_prints_its_usage_with_docstrings_stripped(self):
        printed = subprocess.run([sys.executable, '-OO', str(ROOT / 'tools/floats.py'), '--help'],
                                 capture_output=True, text=True)
        self.assertEqual(printed.returncode, 0, printed.stderr)
        self.assertTrue(printed.stdout.startswith('usage: floats --shc RELEASE'))


class Lines(unittest.TestCase):
    def test_takes_a_header_under_a_directory_of_system_headers_for_a_system_header_however_its_marker_names_it(self):
        # shz_trig.h, named by its path from the root, by an absolute path, as GCC names it under an absolute
        # -isystem, and through a ..: each lies under SYSTEM's directory once resolved, so its product stays as GCC
        # reads it. Without SYSTEM, the header's lines are the tree's, which GCC alone reads
        body = 'static float g(float x) { return 1.0f - x * x; }\n'
        files = {'a.c': '#include <sh4zam/shz_trig.h>\n', 'tools/sh4zam/include/sh4zam/shz_trig.h': body,
                 'tools/sh4zam/source/shz_trig.c': ''}
        with scratch(files) as root:
            for named in ('tools/sh4zam/include/sh4zam/shz_trig.h', f'{root}/tools/sh4zam/include/sh4zam/shz_trig.h',
                          'tools/sh4zam/source/../include/sh4zam/shz_trig.h'):
                c = f'# 1 "a.c"\n# 1 "{named}" 1 3\n{body}'
                with self.subTest(named):
                    self.assertEqual(floats.translate(c, 'a.i', GAME, SYSTEM), (c, []))
                    self.assertIn('__floats_p', floats.translate(c, 'a.i', GAME)[0])

    def test_reads_each_line_s_file_and_whether_shc_reads_it(self):
        # a.c's own lines, SHC's but the non-matching side's; b.h, which a.c includes on that side, GCC's own; and
        # c.h, which a.c includes for both, SHC's again once b.h returns
        files = {'a.c': '#ifdef NONMATCHING\n#include "b.h"\nint n;\n#endif\n#include "c.h"\nint a;\n',
                 'b.h': 'int b;\n', 'c.h': 'int c;\n'}
        i = '# 1 "a.c"\n\n# 1 "b.h" 1\nint b;\n# 3 "a.c" 2\nint n;\n\n# 1 "c.h" 1\nint c;\n# 6 "a.c" 2\nint a;\n'
        with scratch(files):
            toks = float_tokens.marked(i, 'a.i')
        names = [(toks[at].text, toks.where(at), toks.shc[at]) for at in range(len(toks)) if toks[at].text != ';']
        self.assertEqual(names, [('int', 'b.h:1', False), ('b', 'b.h:1', False), ('int', 'a.c:3', False),
                                 ('n', 'a.c:3', False), ('int', 'c.h:1', True), ('c', 'c.h:1', True),
                                 ('int', 'a.c:6', True), ('a', 'a.c:6', True)])

    def test_takes_a_file_that_gcc_s_command_line_includes_for_gcc_s_own(self):
        # f.h, as -include names it, which SHC never reads; and a.c's own line once GCC returns from it, SHC's. There
        # is no file <command-line> to read the side of an #include from
        i = ('# 0 "a.c"\n# 0 "<built-in>"\n# 0 "<command-line>"\n# 1 "f.h" 1\nint f;\n# 0 "<command-line>" 2\n'
             '# 1 "a.c"\nint a;\n')
        with scratch({'a.c': 'int a;\n', 'f.h': 'int f;\n'}):
            toks = float_tokens.marked(i, 'a.i')
        names = [(toks[at].text, toks.where(at), toks.shc[at]) for at in range(len(toks)) if toks[at].text != ';']
        self.assertEqual(names, [('int', 'f.h:1', False), ('f', 'f.h:1', False), ('int', 'a.c:1', True),
                                 ('a', 'a.c:1', True)])

    def test_takes_a_header_in_gcc_s_include_directory_for_a_system_header_however_its_marker_names_it(self):
        # stddef.h, by its path from the toolchain's root, as GCC names it from there, and by an absolute path
        header = 'lib/gcc/sh-elf/16.2.0/include/stddef.h'
        with scratch({'a.c': '#include <stddef.h>\n', header: 'int s;\n'}) as root:
            for named in (header, f'{root}/{header}'):
                with self.subTest(named):
                    toks = float_tokens.marked(f'# 1 "a.c"\n# 1 "{named}" 1 3 4\nint s;\n', 'a.i')
                    self.assertEqual((toks.system, toks.shc), ([True] * 3, [False] * 3))

    def test_names_the_line_of_the_declaration_c_ends_inside(self):
        # by its marker, as each problem names its line, and not by the .i's own lines
        with scratch({'b.h': '\n' * 22 + 'float value =\n'}):
            self.assertEqual(floats.translate('# 23 "b.h"\nfloat value =\n', 'b.i', GAME),
                             (None, ['b.h:23: C that ends inside a declaration']))

    def test_lets_an_index_error_past_the_walk_raise(self):
        # an IndexError from the rewrite is the tool's defect, not C that ends early
        def defect(*_):
            raise IndexError('an edit past the last token')
        with scratch({'b.h': '\n' * 22 + 'float value;\n'}), mock.patch.object(floats, 'fuse', defect):
            with self.assertRaisesRegex(IndexError, 'an edit past the last token'):
                floats.translate('# 23 "b.h"\nfloat value;\n', 'b.i', GAME)


class Edits(unittest.TestCase):
    # a.c's line 1 and the start of its line 2, as tokens: y = a * b + c ;
    C = '# 1 "a.c"\ny = a *\n    b + c;\n'

    def plan(self):
        with scratch({'a.c': self.C[10:]}):
            return float_edits.Plan(float_tokens.marked(self.C, 'a.i'))

    def test_writes_an_edit_in_its_first_token_s_place_and_each_edit_inside_its_spans_there(self):
        # f(a * b, c) in a's place, its spans on one line; what lies between the edit's tokens stays, and its other
        # tokens give nothing. b's edit lies inside the span a * b, and the edit inside that span is written there
        plan = self.plan()
        plan.edit(2, 7, ['f(', (2, 5), ', ', (6, 7), ')'])
        self.assertEqual(plan.text(self.C), '# 1 "a.c"\ny = f(a * b, c) \n      ;\n')
        plan.edit(4, 5, ['B'])
        self.assertEqual(plan.text(self.C), '# 1 "a.c"\ny = f(a * B, c) \n      ;\n')

    def test_writes_pieces_it_can_read_once(self):
        # the outer edit's pieces, an iterator, which text() reads both to check c's edit and to write
        plan = self.plan()
        plan.edit(2, 7, iter(['(', (2, 7), ')']))
        plan.edit(6, 7, ['x'])
        for _ in range(2):
            self.assertEqual(plan.text(self.C), '# 1 "a.c"\ny = (a * b + x) \n      ;\n')

    def test_fails_on_an_edit_it_can_t_write_and_keeps_the_plan(self):
        # no tokens, tokens past the end, a span outside the edit's, and a text of two lines, which moves b to line 3
        for span, pieces in (((2, 2), []), ((7, 9), ['x']), ((2, 5), ['(', (2, 7), ')']), ((2, 3), ['(\na)'])):
            with self.subTest(span):
                plan = self.plan()
                with self.assertRaisesRegex(ValueError, f"^{re.escape(f'toks[{span[0]}:{span[1]}]: ')}"):
                    plan.edit(*span, pieces)
                self.assertEqual(plan.text(self.C), self.C)

    def test_fails_on_two_edits_that_overlap_but_one_inside_a_span_of_the_other(self):
        # one past another's end, one inside another but outside its spans, and two of one span
        overlap = "two of the tool's edits overlap here"
        for spans, problem in ((((2, 5), (4, 7)), f'a.c:2: b + c: {overlap}'),
                               (((2, 7), (6, 7)), f'a.c:2: c: {overlap}')):
            with self.subTest(spans):
                plan = self.plan()
                plan.edit(*spans[0], ['(', (2, 5), ')'])
                plan.edit(*spans[1], ['q'])
                with self.assertRaisesRegex(ValueError, f'^{re.escape(problem)}$'):
                    plan.text(self.C)
        plan = self.plan()
        plan.edit(2, 5, ['p'])
        with self.assertRaisesRegex(ValueError, f'^a\\.c:1: a \\* b: {overlap}$'):
            plan.edit(2, 5, ['q'])


class Divisions(unittest.TestCase):
    def written(self, statement, route=AGGRESSIVE):
        """statement in a function of the floats x and y, the int i and the pointer p, as the tool writes it under
        route, on one line; and the problems."""
        c = f'float f(float x, float y, int i, float *p)\n{{\n    {statement}\n}}\n'
        text, problems = passed(c, route)
        return (text and squeezed(text.splitlines()[3])), problems

    def test_writes_a_division_by_a_constant_as_the_product_by_its_reciprocal(self):
        # NJM_RAD_ANG's division by 2 * NJD_PI, a constant float and an integer, a power of two, and a /=, as the
        # aggressive route's probe multiplies by each, and fuses the product of a sum's quotient as a product's; a
        # negative quotient in a sum, which the rule doesn't fuse, wrapped
        for statement, written in (('return x / (2 * 3.141592);',
                                    'return x * ((__typeof__(x / (2 * 3.141592)))1 / ((2 * 3.141592)));'),
                                   ('return x / 3.0F;', 'return x * ((__typeof__(x / 3.0F))1 / (3.0F));'),
                                   ('return x / 3;', 'return x * ((__typeof__(x / 3))1 / (3));'),
                                   ('return y + x / 3.0F;', 'return y + x * ((__typeof__(x / 3.0F))1 / (3.0F));'),
                                   ('return y + x / -3.0F;', 'return y + ({ __typeof__(x / -3.0F) __floats_p = x * '
                                                             '((__typeof__(x / -3.0F))1 / (-3.0F)); __floats_p; });'),
                                   ('return x * y / 0x8000;',
                                    'return x * y * ((__typeof__(x * y / 0x8000))1 / (0x8000));'),
                                   ('*p /= 3.0F;', '*p *= ((__typeof__(*p / (3.0F)))1 / (3.0F));'),
                                   ('x /= 1 << 2;', 'x *= ((__typeof__(x / (1 << 2)))1 / (1 << 2));')):
            with self.subTest(statement):
                self.assertEqual(self.written(statement), (written, []))

    def test_leaves_a_division_whose_operator_only_gcc_reads(self):
        # a shared dividend and ;, with the / and its divisor on the non-matching side, a division SHC never sees
        c = 'float f(float x)\n{\n    return x\n#ifdef NONMATCHING\n        / 3.0F\n#endif\n        ;\n}\n'
        i = '# 1 "a.c"\nfloat f(float x)\n{\n    return x\n\n        / 3.0F\n\n        ;\n}\n'
        text, problems = passed(c, AGGRESSIVE, i=i)
        self.assertEqual((squeezed(text), problems), (squeezed(i), []))

    def test_leaves_a_quotient_of_constants_one_by_no_constant_and_another_route_s(self):
        # a quotient of constants, which SHC folds as GCC does; an integer quotient; and a division on a route
        # without -aggressive=2
        for statement, route in (('return 5.0F / 3.0F;', AGGRESSIVE), ('return x / y;', AGGRESSIVE),
                                 ('return i / 3;', AGGRESSIVE), ('return x / 3.0F;', GAME)):
            with self.subTest(statement):
                self.assertEqual(self.written(statement, route), (statement, []))


@unittest.skipUnless((SH_ELF / 'bin/sh-elf-gcc').is_file(), 'no SH-4 GCC at SH_ELF')
class Gcc(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.gccflags = variable('GCCFLAGS')

    def compiled(self, c, more, route=GAME):
        """The instructions of a.c's f, as GCC compiles it under GCCFLAGS and the flags more, as GCC's preprocessor
        writes it and as the tool rewrites it under route, by case. GCC and the tool run from the root, as make dc
        runs them, where GCCFLAGS' paths resolve, and take a.c by its absolute path."""
        gcc, found = str(SH_ELF / 'bin/sh-elf-gcc'), {}
        with tempfile.TemporaryDirectory() as scratch_:
            source, i, s = (str(Path(scratch_).resolve() / name) for name in ('a.c', 'a.i', 'a.s'))
            Path(source).write_text(c)
            flags = [*self.gccflags, *more]
            subprocess.run([gcc, *flags, '-E', '-o', i, source], cwd=ROOT, check=True, capture_output=True)
            for case in ('as written', 'rewritten'):
                if case == 'rewritten':
                    written, problems = floats.translate(Path(i).read_text(), i, route, SYSTEM)
                    self.assertEqual(problems, [])
                    Path(i).write_text(written)
                subprocess.run([gcc, *flags, '-S', '-o', s, i], cwd=ROOT, check=True, capture_output=True)
                found[case] = function(Path(s).read_text())
            # the words of the rewritten code's constant pool
            self.pool = [int(word, 0) for word in re.findall(r'\.long\s+(-?[0-9]\S*)', Path(s).read_text())]
        return found

    def test_per_function_contraction_survives_the_final_overlay_link(self):
        gcc = SH_ELF / 'bin/sh-elf-gcc'
        ld = SH_ELF / 'bin/sh-elf-ld'
        objdump = SH_ELF / 'bin/sh-elf-objdump'
        if not all(tool.is_file() for tool in (gcc, ld, objdump)):
            self.skipTest(f'need SH-4 GCC, ld and objdump under {SH_ELF}')
        function_body = '(float a, float b, float c) { return a * b + c; }\n'
        with tempfile.TemporaryDirectory() as directory:
            work = Path(directory)
            sources = {
                'play': 'float play' + function_body + 'int linked_value(void) { return 123; }\n',
                'sdk': 'float sdk' + function_body + 'int linked_value(void);\n'
                       'int from_another_object(void) { return linked_value(); }\n',
            }
            flags = [*variable('GCCFLAGS'), '-Os', *variable('GCC_LTO')]
            for name, source in sources.items():
                path = work / (name + '.c')
                path.write_text(source)
                contraction = variable('GCC_FUSING') if name == 'play' else []
                subprocess.run([gcc, *flags, *contraction, '-c', path, '-o', work / (name + '.o')],
                               cwd=ROOT, check=True)
            terminal = work / 'census.o'
            subprocess.run([gcc, *variable('GCC_LTO_LINK'), '-r', '-nostdlib', '-flinker-output=nolto-rel',
                            '-o', terminal, work / 'play.o', work / 'sdk.o'], cwd=ROOT, check=True)
            final = work / 'overlay.elf'
            subprocess.run([ld, '-EL', '--no-undefined', '--defsym', 'overlay_base=0x8c500000',
                            '-T', ROOT / 'config/overlay.ld', '-e', '_play', '-o', final, terminal], check=True)
            code = sh4.functions(objdump, final)
            for name, wanted in (('play', ['fmac']), ('sdk', ['fadd', 'fmul'])):
                actual = sorted(op for _, op, _ in code[name] if op in ARITHMETIC)
                self.assertEqual(actual, wanted, code[name])
            # Fat objects also carry ordinary machine code. Folding this other
            # translation unit's return value proves the LTO plugin actually ran.
            self.assertIn(('mov', ['#123', 'r0']), [(op, args) for _, op, args in code['from_another_object']])

    def test_gcc_fuses_a_product_where_shc_does_once_the_tool_wraps_the_others(self):
        # GCC fuses c - a * b by FNEG and FMAC as written, and once the tool wraps the product, multiplies by FMUL and
        # subtracts by FSUB, as SHC does; and fuses a * b - 0.5F by FMAC, adding -0.5F, as written and as the tool
        # leaves it, as SHC does
        for c, instructions in (('float f(float a, float b, float c)\n{\n    return c - a * b;\n}\n',
                                 {'as written': ['fmac', 'fneg'], 'rewritten': ['fmul', 'fsub']}),
                                ('float f(float a, float b)\n{\n    return a * b - 0.5F;\n}\n',
                                 {'as written': ['fmac'], 'rewritten': ['fmac']})):
            for level in LEVELS:
                with self.subTest(c, level=level):
                    found = self.compiled(c, [*level, *FUSING])
                    self.assertEqual({case: arithmetic(found[case]) for case in found}, instructions)

    def test_gcc_fuses_a_product_of_a_typedef_into_a_float_s_sum_once_the_tool_gives_it_the_sum_s_type(self):
        # fence_draw_posed's 1.0F + sine * gain, of two Floats, a product added to a double constant, and one a cast
        # parts from its sum
        for c in ('typedef float Float;\nfloat f(Float a, Float b)\n{\n    return 1.0F + a * b;\n}\n',
                  'float f(float a, float b)\n{\n    return a * b + 1.0;\n}\n',
                  'typedef float Float;\nfloat f(Float a, Float b, float c)\n{\n    return c + (Float)(a * b);\n}\n'):
            for level in LEVELS:
                with self.subTest(c, level=level):
                    found = self.compiled(c, [*level, *FUSING])
                    self.assertEqual({case: arithmetic(found[case]) for case in found},
                                     {'as written': ['fadd', 'fmul'], 'rewritten': ['fmac']})

    def test_gcc_fuses_a_plus_equals_of_a_product_holding_a_call_once_the_tool_writes_it(self):
        # camera_step_follow's drift, with an inline function of inline assembly in njCos's place, and a call that
        # can move the pointer the left operand reads: as written, GCC multiplies by FMUL, then adds by FADD; once
        # the tool writes the += as an addition, it fuses by FMAC, as SHC does
        same = 'static inline float same(float a) { float r; __asm__("fmov %1,%0" : "=&f"(r) : "f"(a)); return r; }\n'
        for c in (f'struct v {{ float x; }} *s;\n{same}void f(float a)\n{{\n    s->x += same(a) * 0.2F;\n}}\n',
                  'extern float move(void);\nfloat *aim;\nvoid f(float a)\n{\n    *aim += move() * a;\n}\n'):
            for level in LEVELS:
                with self.subTest(c, level=level):
                    found = self.compiled(c, [*level, *FUSING])
                    self.assertEqual({case: arithmetic(found[case]) for case in found},
                                     {'as written': ['fadd', 'fmul'], 'rewritten': ['fmac']})

    # -O0 loads an address anew for each access, into a register another access reuses, so the two tests that follow
    # the left operand's registers run at -Os and -O2

    def test_gcc_reads_a_plus_equals_left_operand_s_address_before_the_right_s_call_and_its_value_after(self):
        # SHC evaluates *p's address, then h(), then reads *p. The addition the tool writes leaves the read and the
        # call in either order; under -ffp-contract=on, GCC 16.2 loads p once, before the call, and *p after it, as
        # the addend of the FMAC, and stores through the p it loaded. As written, it loads p after the call
        c = 'extern float h(void);\nfloat *p;\nvoid f(float b)\n{\n    *p += h() * b;\n}\n'
        for level in LEVELS[:2]:
            with self.subTest(level=level):
                found = self.compiled(c, [*level, *FUSING])
                rewritten = accesses(found['rewritten'])
                call = [name for name, _, _ in rewritten].index('jsr')
                before, after = rewritten[:call], rewritten[call + 1:]
                pointers = [n for name, n, _ in rewritten if name == 'mov.l @Rm,Rn']
                self.assertEqual(len(pointers), 1)
                self.assertIn(('mov.l @Rm,Rn', pointers[0]), [(name, n) for name, n, _ in before])
                self.assertNotIn('fmov.s @Rm,FRn', [name for name, _, _ in before])
                self.assertIn(pointers[0], [m for name, _, m in after if name == 'fmov.s @Rm,FRn'])
                self.assertEqual(rewritten[-1][:2], ('fmov.s FRm,@Rn', pointers[0]))
                names = [name for name, _, _ in accesses(found['as written'])]
                self.assertGreater(names.index('mov.l @Rm,Rn'), names.index('jsr'))

    def test_gcc_reads_a_plus_equals_left_name_after_the_right_s_call(self):
        # SHC reads a name of static storage after the call, so where h writes x, x += h() * b adds to what h wrote.
        # The tool writes x = x + (h() * b), which leaves the read and the call in either order; under
        # -ffp-contract=on, GCC 16.2 reads x once, after the call, as the addend of the FMAC
        c = 'extern float h(void);\nfloat x;\nvoid f(float b)\n{\n    x += h() * b;\n}\n'
        for level in LEVELS[:2]:
            with self.subTest(level=level):
                found = self.compiled(c, [*level, *FUSING])
                self.assertEqual(arithmetic(found['rewritten']), ['fmac'])
                rewritten = accesses(found['rewritten'])
                call = [name for name, _, _ in rewritten].index('jsr')
                name, through, _ = rewritten[-1]
                self.assertEqual(name, 'fmov.s FRm,@Rn')
                reads = [at for at, (name, _, m) in enumerate(rewritten) if name == 'fmov.s @Rm,FRn' and m == through]
                self.assertEqual(len(reads), 1)
                self.assertGreater(reads[0], call)

    def test_gcc_multiplies_by_the_reciprocal_shc_does_once_the_tool_writes_it(self):
        # NJM_RAD_ANG's division by 2 * NJD_PI, which GCC divides as written, and multiplies by H'3E22F986 once the
        # tool writes it, as sonic_pick_mode's FMUL at 0x8c06c9d8 does
        c = 'float f(float x)\n{\n    return x / (2 * 3.141592);\n}\n'
        for level in LEVELS:
            with self.subTest(level=level):
                found = self.compiled(c, [*level, *FUSING], AGGRESSIVE)
                self.assertEqual({case: arithmetic(found[case]) for case in found},
                                 {'as written': ['fdiv'], 'rewritten': ['fmul']})
                self.assertIn(0x3E22F986, self.pool)

    def test_gcc_keeps_a_product_on_a_non_matching_side_unfused_once_the_tool_wraps_it(self):
        # njAng2Quat's c[0] * c[1] - c[1] * c[2] beside its hand-fused product, which GCC would fuse under on
        c = ('float f(float *c)\n{\n#ifdef NONMATCHING\n'
             '    return __builtin_fmaf(c[2], c[0], c[0] * c[1] - c[1] * c[2]);\n#else\n'
             '    return c[0] * c[1] - c[1] * c[2] + c[2] * c[0];\n#endif\n}\n')
        for level in LEVELS:
            with self.subTest(level=level):
                found = self.compiled(c, [*level, *FUSING])
                self.assertEqual({case: arithmetic(found[case]) for case in found},
                                 {'as written': ['fmac', 'fmac', 'fmul', 'fneg'],
                                  'rewritten': ['fmac', 'fmul', 'fmul', 'fsub']})


if __name__ == '__main__':
    unittest.main()
