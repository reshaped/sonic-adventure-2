"""tools/float_fuse.py's rule of SHC's fusion, as the tool writes C by it: the products it wraps, those it gives
their sum's type, the += it writes as additions, and what it refuses. Each case's comment names the listing or the
probe that bears its clause out; a probe is a small source through SHC under the game's route, R13, and Ninja's,
R11, which give one answer for each."""

import contextlib
import csv
import os
import re
import tempfile
import unittest
from pathlib import Path

import floats
from csource import parse
from float_fuse import Route

SYSTEM = ['tools/sh4zam/include']
# the routes' SHC as the tool reads each: the game's and Ninja's, with -fpu=single; runtime64's, with a double of
# binary64; and the aggressive route's
GAME, NINJA, WIDE, AGGRESSIVE = Route('r13', True), Route('r11', True), Route('r13'), Route('r13', True, True)


@contextlib.contextmanager
def scratch(files):
    """A temporary working directory holding files, a {path: text} map, as the tool runs in the tree's root."""
    home = os.getcwd()
    with tempfile.TemporaryDirectory() as root:
        for name, text in files.items():
            path = Path(root) / name
            path.parent.mkdir(parents=True, exist_ok=True)
            path.write_text(text)
        os.chdir(root)
        try:
            yield Path(root)
        finally:
            os.chdir(home)


def passed(c, route=GAME, files=None, i=None):
    """c, a.c's C, as the tool writes GCC's preprocessed C of it under route, the game's by default, with SYSTEM's
    headers as system headers: the C, or None where the tool fails, and the problems. The preprocessed C is i, or
    else c under a marker of a.c's line 1. files gives the other files of the tree beside a.c."""
    with scratch({'a.c': c, **(files or {})}):
        return floats.translate(f'# 1 "a.c"\n{c}' if i is None else i, 'a.i', route, SYSTEM)


def wrapped(product):
    """product as the tool wraps it."""
    return f'({{ __typeof__({product}) __floats_p = {product}; __floats_p; }})'


def captured(left, right):
    """left += right as the tool writes it as an addition where left is no name."""
    return f'({{ __typeof__({left}) *__floats_l = &({left}); *__floats_l = *__floats_l + ({right}); }})'


def squeezed(text):
    """text on one line, each run of whitespace a space, and none before a ) or a ;, where the whitespace between an
    edit's tokens can leave one."""
    return re.sub(r' (?=[);])', '', ' '.join(text.split()))


def returned(expression, route=GAME):
    """expression as the tool writes it under route, the game's by default, on one line, or None where it fails; and
    the problems. A function of the floats x and y, the int i, the unsigned u and p, a pointer to floats, returns
    expression."""
    c = f'float f(float x, float y, int i, unsigned u, const float *p)\n{{\n    return {expression};\n}}\n'
    text, problems = passed(c, route)
    if text is None:
        return None, problems
    return squeezed(text.splitlines()[3])[len('return '):-1], problems


class Wraps(unittest.TestCase):
    # each floating product that SHC doesn't fuse into its run of + and -, which GCC fuses by FMAC under the
    # -ffp-contract=on make dc gives a play source

    def test_wraps_each_floating_product_shc_subtracts_or_subtracts_from(self):
        # a binary -'s operand, on either side and both of a cross term, and a unary -'s, through parentheses, a unary +
        # and a cast to a floating type; the whole of a run of *, each product inside another, and one beside an int,
        # which C converts, or a negated one; one after negative operands alone, one in a sum a minus negates, one a
        # negated factor makes negative, one ahead of a negative name or of a run of integers C subtracts whole, and
        # one ahead of a minus a constant follows. njDistanceP2PL's d = -(3 products) runs FMUL and FSUB under R11,
        # and point_plane_distance's foot = p - part * v FMUL and FSUB under R13
        for expression, written in (('y - x * y', f'y - {wrapped("x * y")}'),
                                    ('x * y - y', f'{wrapped("x * y")} - y'),
                                    ('x * y - p[0] * p[1]', f'{wrapped("x * y")} - {wrapped("p[0] * p[1]")}'),
                                    ('-(x * y) + y', f'-({wrapped("x * y")}) + y'),
                                    ('y - +(float)(x * y)', f'y - +(float)({wrapped("x * y")})'),
                                    ('y - x * y * y', f'y - {wrapped("x * y * y")}'),
                                    ('y - x * (y - p[0] * p[1])',
                                     f'y - {wrapped("x * (y - " + wrapped("p[0] * p[1]") + ")")}'),
                                    ('i - x * i', f'i - {wrapped("x * i")}'),
                                    ('y - (-i) * y', f'y - {wrapped("(-i) * y")}'),
                                    ('y + (-x) * y', f'y + {wrapped("(-x) * y")}'),
                                    ('x * y + -y', f'{wrapped("x * y")} + -y'),
                                    ('x * y - (1 - i)', f'{wrapped("x * y")} - (1 - i)'),
                                    ('x * y - y - 0.5F', f'{wrapped("x * y")} - y - 0.5F')):
            with self.subTest(expression):
                self.assertEqual(returned(expression), (written, []))
        # the right operand of a -=, in a call's argument too, and of a +=, where -s negate its parts, and a product in
        # an automatic object's initializer, under R13 as under R11
        c = ('void g(float);\nvoid f(float *x, float y)\n{\n    float z = y - y * y;\n    *x -= y * y;\n'
             '    g(*x -= y * 2.0F);\n    *x += -y * y;\n    *x += y * 3.0F;\n}\n')
        for route in (GAME, NINJA):
            with self.subTest(route.release):
                text, problems = passed(c, route)
                self.assertEqual([squeezed(line) for line in text.splitlines()[4:9]],
                                 [f'float z = y - {wrapped("y * y")};', f'*x -= {wrapped("y * y")};',
                                  f'g(*x -= {wrapped("y * 2.0F")});', f'*x += {wrapped("-y * y")};', '*x += y * 3.0F;'])
                self.assertEqual(problems, [])

    def test_leaves_a_product_shc_adds_a_constant_one_and_an_integer_one(self):
        # a + takes the product after an operand it adds or ahead of one, a second minus, through a cast to a floating
        # type too, a unary - or a negated factor turns it back, as a negation of integers doesn't, or the run's
        # constants alone stand beside it, which SHC adds; C converts it to an integer or a conditional chooses it; it
        # multiplies integers; or both its factors are constant, which GCC folds as SHC does. point_segment_distance's
        # near.x = p + part * v fuses under R13, and njDistanceP2L's along under R11
        for expression in ('y + x * y', 'x * y + y', 'x * y + p[0] - y', 'y - -(x * y)',
                           'y - (-x) * y', 'y - x * -3.0F',
                           'x * y + -(1 - i)', 'x * y - 0.5F', 'x * y - -0.5F', '-(0.5F - x * y)',
                           'y - (int)(x * y)', 'y - (i ? x * y : y)', 'i - i * i', 'u - (u * 2)', '-(i * 2) + u',
                           'y + 0.1F * 3.0F', 'y - 0.1F * 0.3F', '0.1F * 0.3F - 0.03F'):
            with self.subTest(expression):
                self.assertEqual(returned(expression), (expression, []))
        # a static initializer, a type at file scope, an array's length in a block, and a system header's code
        c = ('float y;\n__typeof__(y - y * y) v;\nfloat f(float x)\n{\n    static float s = 1.0F - 0.5F * 0.5F;\n'
             '    float u[(int)(8 - x * x)];\n    return s;\n}\n'
             '# 1 "tools/sh4zam/include/sh4zam/shz_scalar.h" 1 3\nstatic float g(float x) { return 1.0f - x * x; }\n')
        self.assertEqual(passed(c), (f'# 1 "a.c"\n{c}', []))

    def test_writes_a_run_that_reads_no_member_and_calls_nothing_as_shc_orders_it(self):
        # where a product is positive, SHC adds the products first, in C's order, then the operands that read memory,
        # then the rest, each in C's order, and the constants last. See order_element, order_dereference and
        # ordered_parentheses in tests/float_rules/expressions.c. The expression y - p[0] + a * b computes
        # a * b - p[0] + y by FMUL, FSUB and FADD. The sum c - d + a * b fuses a * b into c, then subtracts d;
        # c + d + a * b fuses into c, then adds d; c - a * b + d * e computes d * e - a * b + c,
        # fusing neither; a * b - d + c * e fuses a * b into c * e, then subtracts d; and gc - gd + a * b and
        # y - *q + a * b read memory as p[0] does. The pass writes such a run in that order. A run of two operands is
        # one sum in either order, and a run with no positive product keeps C's order, as y - a * b - c * d does
        for expression, written in (('y - p[0] + x * y', f'(({wrapped("x * y")}) - (p[0]) + (y))'),
                                    ('-p[0] - y + x * y', f'(({wrapped("x * y")}) - (p[0]) - (y))'),
                                    ('y - x + p[0] * y', '((p[0] * y) + (y) - (x))'),
                                    ('y + x + x * y', '((x * y) + (y) + (x))'),
                                    ('y + (x * y + p[0])', '((x * y) + (p[0]) + (y))'),
                                    ('y - (p[0] - x * y)', f'(({wrapped("x * y")}) - (p[0]) + (y))'),
                                    ('y - (float)(p[0] - x * y)', f'(({wrapped("x * y")}) - (p[0]) + (y))'),
                                    ('y - x * y + p[0] * x',
                                     f'(-({wrapped("x * y")}) + ({wrapped("p[0] * x")}) + (y))'),
                                    ('x * y - p[0] + y * y', '((x * y) + (y * y) - (p[0]))'),
                                    ('y + x * y + 5.0F', 'y + x * y + 5.0F'), ('y - x * y - x * x', None)):
            with self.subTest(expression):
                text, problems = returned(expression)
                self.assertEqual((text, problems), (written or expression.replace(
                    'x * y - x * x', f'{wrapped("x * y")} - {wrapped("x * x")}'), []))

    def test_writes_a_run_whose_grouping_gives_a_fused_product_another_sum_as_shc_sums_it(self):
        # where an operand reads a member, SHC adds the run's operands in C's order, its constants last:
        # spinning_hammer_init's work->pos.y + ((25.0F * THICKNESS) + 5.0F) fuses the product into the height by the
        # FMAC at 0x8c50084e in SONIC2/STG14.PRS, then adds 5.0F. Where C's grouping agrees, the run stays
        c = ('struct v { float x; } *s;\nfloat f(float x, float y)\n{\n'
             '    s->x = s->x + ((2.0F * (x + 1.0F)) + 5.0F);\n    return s->x + x * y + 5.0F;\n}\n')
        text, problems = passed(c)
        self.assertEqual([squeezed(line) for line in text.splitlines()[4:6]],
                         ['s->x = ((s->x) + (2.0F * (x + 1.0F)) + ((5.0F)));', 'return s->x + x * y + 5.0F;'])
        self.assertEqual(problems, [])

    def test_adds_a_run_that_reads_a_member_or_calls_in_c_s_order(self):
        # call_name's y - g(a) + a * b fuses a * b into y - g(a), and names_call_product's c + d + g(a) * b into
        # c + d, as m_sub_first's c - s->pos * 0.3F + s->spd * 0.9F does into c - s->pos * 0.3F
        c = ('extern float g(float);\nstruct v { float pos; float spd; } *s;\nfloat f(float y, float a, float b)\n{\n'
             '    y = y - g(a) + a * b;\n    y = y + b + g(a) * b;\n    return y - s->pos * 0.3F + s->spd * 0.9F;\n}\n')
        text, problems = passed(c)
        self.assertEqual([squeezed(line) for line in text.splitlines()[5:8]], [
            'y = y - g(a) + a * b;', 'y = y + b + g(a) * b;',
            f'return y - {wrapped("s->pos * 0.3F")} + s->spd * 0.9F;'])
        self.assertEqual(problems, [])

    def test_fuses_a_product_after_negative_operands_alone_where_an_operand_reads_a_member(self):
        # bridge_step's spring, which SHC computes by FMUL, FNEG and FMAC, as the probe's -(s->pos * 0.3F) +
        # (s->spd * 0.9F) does, a member on either side of the run, or a member alone negated; with a parameter, a
        # global or an array's element alone, SHC computes the product less the rest, and fuses nothing
        c = ('struct spring { float pos; float spd; };\nfloat g;\nfloat f(struct spring *s, float a, float *p)\n{\n'
             '    s->spd = -(s->pos * 0.3F) + (s->spd * 0.9F);\n    a = -(s->pos * 0.3F) + (a * 0.9F);\n'
             '    a = -(a * 0.3F) + (s->spd * 0.9F);\n    a = -s->pos + (a * 0.9F);\n'
             '    a = -(a * 0.3F) + (g * 0.9F);\n    return -(p[0] * 0.3F) + (p[1] * 0.9F);\n}\n')
        text, problems = passed(c)
        self.assertEqual([squeezed(line) for line in text.splitlines()[5:11]], [
            f's->spd = -({wrapped("s->pos * 0.3F")}) + (s->spd * 0.9F);',
            f'a = -({wrapped("s->pos * 0.3F")}) + (a * 0.9F);', f'a = -({wrapped("a * 0.3F")}) + (s->spd * 0.9F);',
            'a = -s->pos + (a * 0.9F);', f'a = -({wrapped("a * 0.3F")}) + ({wrapped("g * 0.9F")});',
            f'return -({wrapped("p[0] * 0.3F")}) + ({wrapped("p[1] * 0.9F")});'])
        self.assertEqual(problems, [])

    def test_writes_a_run_in_shc_s_order_by_the_signs_and_the_types_c_gives_it(self):
        # a product a negated factor makes negative, which the run adds with its - inside it; integer products, which
        # SHC adds where C does, as it adds integers; a fold of integer constants, which C converts each to the run's
        # floating type, and the integer arithmetic inside one; and a constant C adds before an operand that isn't
        # constant, which SHC adds last
        for expression, written in (
                ('y + x * (-y) + x * x', f'(({wrapped("x * (-y)")}) + ({wrapped("x * x")}) + (y))'),
                ('y + i * i + i * 3', 'y + i * i + i * 3'),
                ('y + 1u - 2u + x * y + (3u - 5u)',
                 '((y) + (x * y) + ((float)(1u) - (float)(2u) + (float)(3u - 5u)))'),
                ('x * y + x + 1.0F + y', '((x * y) + (x) + (y) + ((1.0F)))')):
            with self.subTest(expression):
                self.assertEqual(returned(expression), (written, []))

    def test_wraps_a_product_two_expressions_of_one_stretch_share(self):
        # the spline weights' 2.0F * t2, which SHC computes once by FMUL and adds by FADD, as the probe's does; and
        # a product two alike sums hold, as player_face_along's x * x + y * y, which SHC computes once, fused. A
        # brace, a write of a factor or a call where a factor reads a member parts the two, and SHC computes each
        c = ('extern void g(void);\nstruct v { float t; } *s;\nfloat f(float t, float k, float *w)\n{\n'
             '    w[0] = (2.0F * t) - k;\n    w[1] = k + (2.0F * t);\n'
             '    w[2] = (t * t) + (k * k) + w[0];\n    w[3] = (t * t) + (k * k);\n'
             '    w[4] = k + (3.0F * t);\n    if (k) {\n    }\n    w[5] = k + (3.0F * t);\n'
             '    w[6] = k + (4.0F * t);\n    t = k;\n    w[7] = k + (4.0F * t);\n'
             '    w[8] = k + (5.0F * s->t);\n    g();\n    return k + (5.0F * s->t);\n}\n')
        text, problems = passed(c)
        self.assertEqual([squeezed(line) for line in text.splitlines()[5:9]], [
            f'w[0] = ({wrapped("2.0F * t")}) - k;', f'w[1] = k + ({wrapped("2.0F * t")});',
            'w[2] = (t * t) + (k * k) + w[0];', 'w[3] = (t * t) + (k * k);'])
        self.assertEqual([squeezed(line) for line in text.splitlines()[9:20]], [
            'w[4] = k + (3.0F * t);', 'if (k) {', '}', 'w[5] = k + (3.0F * t);', 'w[6] = k + (4.0F * t);', 't = k;',
            'w[7] = k + (4.0F * t);', 'w[8] = k + (5.0F * s->t);', 'g();', 'return k + (5.0F * s->t);', '}'])
        self.assertEqual(problems, [])

    def test_two_observed_results_match_the_native_shared_product_probes(self):
        # shared.c, and shared_reads.c: a store to a struct whose member's address a pointer holds, a store to
        # another local, an addend that changes between two sums spelled alike, and a += beside a sum
        root = Path(__file__).parent / 'floats'
        profiles = {'r11_ninja': NINJA, 'r13_game': GAME, 'r13_aggressive': AGGRESSIVE, 'r26_modem': Route('r26', True)}
        for fixture, profile in ((fixture, profile) for fixture in ('shared', 'shared_reads') for profile in profiles):
            source = (root / f'{fixture}.c').read_text()
            with (root / f'{fixture}.tsv').open() as native:
                observations = list(csv.DictReader(native, delimiter='\t'))
            written, problems = passed(source, profiles[profile])
            self.assertEqual(problems, [])
            bodies = {block.name: written[block.opening.end:block.closing.start]
                      for block in parse(written).blocks if block.name and block.closing}
            self.assertEqual(set(bodies), {row['function'] for row in observations})
            for row in observations:
                with self.subTest(profile=profile, function=row['function']):
                    # The two positive products fuse unless the original compiler
                    # shared one rounded multiplication between both sums.
                    wraps = {'FMAC FMAC': 0, 'FMUL FADD FADD': 2}[row[profile]]
                    self.assertEqual(bodies[row['function']].count('__floats_p ='), wraps)

    def test_shared_operands_keep_their_bindings_and_qualifiers(self):
        def observe(declarations, body, product='a * b'):
            c = (declarations + '\nvoid f(float *out, float b, float c, float d)\n{\n' + body + '\n}\n')
            text, problems = passed(c)
            self.assertEqual(problems, [])
            return text.count(wrapped(product))

        self.assertEqual(observe('extern float a;', '''
            float first;
            out[0] = a * b + c;
            first = a * b + d;
            out[1] = first;'''), 0)
        self.assertEqual(observe('extern float a; extern void take(float);', """
            take(a * b + c);
            out[1] = a * b + d;"""), 0)
        self.assertEqual(observe('extern float a; extern void change(void);', '''
            float a = b;
            out[0] = a * b + c;
            change();
            out[1] = a * b + d;'''), 2)
        for storage in ('extern', 'static'):
            with self.subTest(storage=storage):
                self.assertEqual(observe('extern void change(void);', f"""
                    {storage} float a;
                    out[0] = a * b + c;
                    change();
                    out[1] = a * b + d;"""), 0)
        # Both local assignments are disjoint from the inputs. The volatile
        # member or index alone prevents two reads from representing one value.
        for declarations, product in (
                ('typedef volatile float V; struct sample { V a; }; extern struct sample s;', 's.a * b'),
                ('extern float samples[4]; extern volatile int index;', 'samples[index] * b'),
                ('extern volatile float *sample;', '*sample * b')):
            body = f'float x, y; x = {product} + c; y = {product} + d; out[0] = x; out[1] = y;'
            with self.subTest(product=product):
                self.assertEqual(observe(declarations, body, product), 0)

    def test_reads_each_operand_s_type_from_the_declarations_in_scope(self):
        # a bit-field and an enumerator are integers, a member of an unnamed struct and a call's value are floating,
        # a local count hides the global one, an inner block's hides that, a typedef's cast is floating, a parameter
        # is what its declaration says, and an old-style definition declares its parameters before its body
        c = ('typedef float Float;\nenum { COUNT = 3 };\n'
             'struct v { Float x; int n : 4; struct { Float w; }; } *list[2];\n'
             'Float scale(int);\nint count;\nvoid f(struct v *s, int n)\n{\n    Float count = 2.0F;\n'
             '    s->x = s->x - s->n * COUNT;\n    s->x = s->x - list[0]->w * n;\n    s->x = s->x - scale(n) * n;\n'
             '    s->x = s->x - count * n;\n    {\n        int count = 1;\n        s->x = s->x - count * n;\n    }\n'
             '    s->x = s->x - (Float)n * n;\n    s->x = s->x - (*s).n * n;\n}\n'
             'int g(n, m)\nint n;\nfloat m;\n{\n    return n - m * n;\n}\n')
        text, problems = passed(c)
        lines = [squeezed(line) for line in text.splitlines()]
        self.assertEqual(lines[9:19], ['s->x = s->x - s->n * COUNT;', f's->x = s->x - {wrapped("list[0]->w * n")};',
                                       f's->x = s->x - {wrapped("scale(n) * n")};',
                                       f's->x = s->x - {wrapped("count * n")};', '{', 'int count = 1;',
                                       's->x = s->x - count * n;', '}', f's->x = s->x - {wrapped("(Float)n * n")};',
                                       's->x = s->x - (*s).n * n;'])
        self.assertEqual((lines[24], problems), (f'return n - {wrapped("m * n")};', []))

    def test_reads_each_name_as_the_innermost_declaration_in_scope_declares_it(self):
        # a parameter and a local K hide the enumerator K, so K * K is floating, and a local F hides the typedef F, so
        # (F) * y multiplies by it; past their scopes, K * K is an int and (F) * p casts *p, and the run adds the
        # constant K * K last
        c = ('typedef float F;\nenum { K = 2 };\nfloat f(float K, float y)\n{\n    return y - K * K;\n}\n'
             'float g(float y)\n{\n    float K = y, F = y;\n    return y - K * K - (F) * y;\n}\n'
             'float h(float y, float *p)\n{\n    return y - K * K - (F) * p;\n}\n')
        text, problems = passed(c)
        lines = [squeezed(line) for line in text.splitlines()]
        self.assertEqual([lines[5], lines[10], lines[14]], [
            f'return y - {wrapped("K * K")};', f'return y - {wrapped("K * K")} - {wrapped("(F) * y")};',
            'return ((y) - (* p) + (-(float)(K * K)));'])
        self.assertEqual(problems, [])

    def test_fails_where_it_can_t_tell_whether_a_product_is_floating(self):
        # a name no declaration in scope names, beside an integer, and a call of no function, which leave the run's
        # type unread too; a product beside a floating operand is floating whatever the other's type
        for expression, why in (('i - z * i', 'z, which no declaration in scope names'),
                                ('i - i(2) * i', 'i(2), a call of no function')):
            with self.subTest(expression):
                product = expression[4:]
                self.assertEqual(returned(expression)[1], [
                    f'a.c:3: {expression}: C the tool can\'t tell is floating: {why}',
                    f'a.c:3: {product}: a product SHC doesn\'t fuse, whose type the tool can\'t tell: {why}'])
        self.assertEqual(returned('y - z * y'), (f'y - {wrapped("z * y")}', []))

    def test_fails_where_it_can_t_tell_whether_an_operand_of_a_run_is_constant_or_floating(self):
        # in the scope of a declaration the walk can't read, which can hide the enumerator K and the int i: K is no
        # operand of the run beside a * b if it is constant, and -i is no negative one if i is an int
        for expression, what in (('a * b - K', 'constant'), ('a * b + -i', 'floating')):
            with self.subTest(expression):
                c = ('enum { K = 2 };\nfloat f(float x, int i)\n{\n    const __typeof__(x) z = x;\n'
                     f'    float a = x, b = x;\n    return {expression};\n}}\n')
                name = expression[-1]
                self.assertEqual(passed(c), (None, [
                    f"a.c:6: {name}: C the tool can't tell is {what}: {name}, in the scope of a declaration the tool "
                    "can't read: z after a declarator"]))

    def test_fails_on_a_product_to_wrap_beside_a_cast_it_doesn_t_read(self):
        # and wraps one beside a cast to float
        self.assertEqual(returned('y - (__typeof__(x)) y * y'), (None, [
            "a.c:3: (__typeof__(x)) y * y: a product SHC doesn't fuse, beside (__typeof__(x)) y, a cast to a type the "
            "tool doesn't read: __typeof__ ( x ): __typeof__, a word Types doesn't read"]))
        self.assertEqual(returned('y - (float) y * y'), (f'y - {wrapped("(float) y * y")}', []))


class Types(unittest.TestCase):
    # GCC fuses a product into a sum only where the two have one type, a typedef's name and all, as kept() reads it
    C = 'typedef float Float;\ntypedef Float Sub;\nextern Float g(int);\nstruct pt { Float x, y; };\n'

    def written(self, head, expression):
        """expression as the tool writes it in a function of the parameters head names, on one line."""
        c = f'{self.C}double f({head})\n{{\n    return {expression};\n}}\n'
        text, problems = passed(c)
        self.assertEqual(problems, [])
        return squeezed(text.splitlines()[7])[len('return '):-1]

    def test_gives_a_product_of_another_type_than_its_sum_s_the_sum_s_type(self):
        # Float's product beside a float, a float's beside a double constant, an integer factor's, which keeps the
        # Float, and an element's, a member's and a call's: SHC fuses each, as fence_draw_posed's
        # 1.0F + (sine * pulse_1_gain_y) by the FMAC at 0x8c50123a in SONIC2/STG09.PRS, and GCC fuses none until its
        # type is the sum's. In a +=, its left operand's type takes part
        for head, expression, written in (('float c, Float a, Float b', 'c + a * b', 'c + (float)(a) * b'),
                                          ('Float a, Float b', '1.0F + a * b', '1.0F + (float)(a) * b'),
                                          ('float a, float b', 'a * b + 1.0', '(double)(a) * b + 1.0'),
                                          ('float c, Float a, int i', 'c + a * i', 'c + (float)(a) * i'),
                                          ('float c, Float *v', 'c + v[0] * v[1]', 'c + (float)(v[0]) * v[1]'),
                                          ('float c, struct pt *p', 'c + p->x * p->y', 'c + (float)(p->x) * p->y'),
                                          ('float c, int k, Float b', 'c + g(k) * b', 'c + (float)(g(k)) * b'),
                                          ('Float c, Sub a, Sub b', 'c + a * b', 'c + (float)(a) * b'),
                                          ('float c, Float a, Float b, Float d', 'a * b + c * d',
                                           '(float)(a) * b + c * d'),
                                          ('float a, float b', '2.0 * 3.0 + a * b', '2.0 * 3.0 + (double)(a) * b'),
                                          ('float c, float a, float b, double *v', 'c + a * b + v[0]',
                                           '(((double)(a) * b) + (v[0]) + (c))')):
            with self.subTest(expression=expression, head=head):
                self.assertEqual(self.written(head, expression), written)
        c = f'{self.C}void f(float *x, Float a, Float b)\n{{\n    *x += a * b;\n}}\n'
        self.assertEqual(squeezed(passed(c)[0].splitlines()[7]), '*x += (float)(a) * b;')

    def test_types_a_product_by_the_sum_of_the_order_it_writes(self):
        # GCC's grouping fuses the product into v[0], a double, and the order SHC sums it in into p->x, whose sum is
        # a float, as the product is
        self.assertEqual(self.written('struct pt *p, float a, float b, double *v', 'p->x + (a * b + v[0])'),
                         '((p->x) + (a * b) + (v[0]))')

    def test_leaves_a_product_of_its_sum_s_type(self):
        # one typedef throughout, a float product beside a Float, which GCC's sum gives a float's type, a product of a
        # float and a Float, and a product beside an integer, whose type it keeps
        for head, expression in (('Float c, Float a, Float b', 'c + a * b'), ('Float c, Float a, float b', 'c + a * b'),
                                 ('Float c, float a, float b', 'c + a * b'), ('float c, Float a', 'c + a * 2.0F'),
                                 ('int i, Float a, Float b', 'i + a * b'), ('float a', 'a * 0.5 + a')):
            with self.subTest(expression=expression, head=head):
                self.assertEqual(self.written(head, expression), expression)

    def test_gives_a_product_a_cast_parts_from_its_sum_the_cast_s_type(self):
        # GCC's cast takes its type's main variant, which no typedef names, and stands between the sum and a product
        # of another type, but not one of its own. Where the cast's type is narrower than the sum's, the tool fails
        for head, expression, written in (('float c, Float a, Float b', 'c + (float)(a * b)',
                                           'c + (float)((float)(a) * b)'),
                                          ('Float c, Float a, Float b', 'c + (Float)(a * b)',
                                           'c + (Float)((float)(a) * b)'),
                                          ('float c, float a, float b', 'c + (float)(a * b)', 'c + (float)(a * b)')):
            with self.subTest(expression=expression, head=head):
                self.assertEqual(self.written(head, expression), written)
        c = f'{self.C}double f(double c, float a, float b)\n{{\n    return c + (float)(a * b);\n}}\n'
        self.assertEqual(passed(c)[1], [
            "a.c:7: a * b: a product SHC fuses, which a cast to c + (float)(a * b)'s narrower type parts from the sum, "
            "so that GCC can't fuse it"])


class Lines(unittest.TestCase):
    # the rule holds on SHC's lines alone: on a line GCC alone reads, written for -ffp-contract=off, the tool wraps
    # each product a sum takes

    def test_wraps_each_product_a_sum_takes_on_a_non_matching_side(self):
        # njAng2Quat's fused side in sdk/ninja/njCnkMotLinkSrc.c, whose c[0] * c[1] the shipped FSUB takes unfused,
        # and a += there; the matching side's line, blank in GCC's C, and the line after the guard follow SHC
        c = ('void f(float *s, float *c)\n{\n#ifdef NONMATCHING\n'
             '    *s = __builtin_fmaf(c[2], c[0], (c[0] * c[1]) - (c[1] * c[2])) + 1.0F;\n    *s += c[0] * c[1];\n'
             '#else\n    *s = (c[0] * c[1]) - (c[1] * c[2]) + (c[2] * c[0]) + 1.0F;\n#endif\n'
             '    *s += c[0] * c[1];\n}\n')
        i = ('# 1 "a.c"\nvoid f(float *s, float *c)\n{\n\n'
             '    *s = __builtin_fmaf(c[2], c[0], (c[0] * c[1]) - (c[1] * c[2])) + 1.0F;\n    *s += c[0] * c[1];\n'
             '\n\n\n    *s += c[0] * c[1];\n}\n')
        text, problems = passed(c, i=i)
        self.assertEqual([squeezed(line) for line in text.splitlines()[4:10]], [
            f'*s = __builtin_fmaf(c[2], c[0], ({wrapped("c[0] * c[1]")}) - ({wrapped("c[1] * c[2]")})) + 1.0F;',
            f'*s += {wrapped("c[0] * c[1]")};', '', '', '', '*s += c[0] * c[1];'])
        self.assertEqual(problems, [])

    def test_wraps_each_product_a_sum_takes_in_a_file_gcc_alone_includes(self):
        # b.h, which a.c includes on its non-matching side, as ninja.h includes ninja_zam.h, and c.h, which b.h
        # includes; d.h, which a.c includes for SHC too, follows SHC's rule, past its own guard's line
        files = {'b.h': 'static float b(float x) { return x * x + x; }\n#include "c.h"\n',
                 'c.h': 'static float c(float x) { return x * x + x; }\n',
                 'd.h': '#ifdef NONMATCHING\n#endif\nstatic float d(float x) { return x * x + x; }\n'}
        c = '#ifdef NONMATCHING\n#include "b.h"\n#endif\n#include "d.h"\nfloat a(float x) { return x * x + x; }\n'
        i = ('# 1 "a.c"\n\n# 1 "b.h" 1\nstatic float b(float x) { return x * x + x; }\n# 1 "c.h" 1\n'
             'static float c(float x) { return x * x + x; }\n# 3 "b.h" 2\n# 3 "a.c" 2\n\n# 1 "d.h" 1\n\n\n'
             'static float d(float x) { return x * x + x; }\n# 5 "a.c" 2\nfloat a(float x) { return x * x + x; }\n')
        text, problems = passed(c, files=files, i=i)
        bodies = [squeezed(line) for line in text.splitlines() if line.startswith(('static', 'float'))]
        self.assertEqual(bodies, [f'static float b(float x) {{ return {wrapped("x * x")} + x; }}',
                                  f'static float c(float x) {{ return {wrapped("x * x")} + x; }}',
                                  'static float d(float x) { return x * x + x; }',
                                  'float a(float x) { return x * x + x; }'])
        self.assertEqual(problems, [])

    def test_wraps_each_product_of_a_run_a_non_matching_side_splits(self):
        # SHC reads no run whole that a guard's non-matching side ends, so its rule doesn't hold there, as GCC alone
        # reads the + and the product; the run with no guard follows SHC
        c = ('float f(float a, float b, float c)\n{\n    return c\n#ifdef NONMATCHING\n        + a * b\n#endif\n'
             '        ;\n}\n')
        i = '# 1 "a.c"\nfloat f(float a, float b, float c)\n{\n    return c\n\n        + a * b\n\n        ;\n}\n'
        text, problems = passed(c, i=i)
        self.assertEqual((squeezed(text.splitlines()[5]), problems), (f'+ {wrapped("a * b")}', []))
        c = 'float f(float a, float b, float c)\n{\n    return c\n        + a * b\n        ;\n}\n'
        self.assertEqual(passed(c), (f'# 1 "a.c"\n{c}', []))

    def test_fails_on_a_file_it_can_t_read_for_its_guards(self):
        i = '# 1 "a.c"\n# 1 "e.h" 1\nstatic float e(float x) { return x * x + x; }\n'
        with self.assertRaisesRegex(SystemExit, r'e\.h: No such file or directory'):
            passed('#include "e.h"\n', i=i)


class Compounds(unittest.TestCase):
    # each += of a floating product that SHC fuses, which GCC doesn't where the product holds a side effect, and
    # each into an integer, which SHC doesn't, as probes of each case under -ffp-contract=on show
    C = ('typedef float Float;\nstruct v { Float x; Float a[4]; struct v *next; Float *vp; } *s, list[4], *f(void);\n'
         'struct w { short y; } *camera_work;\nstatic inline Float cosine(short n) { return n + n; }\n'
         'extern Float scale(Float);\nvolatile Float blue;\nvolatile struct v *body;\nint n;\n'
         'struct u { volatile Float m; Float *volatile q; } *pu;\nvolatile Float *pv, va[4];\n'
         'typedef volatile struct v Shaky;\nShaky *vv;\nFloat *volatile vp;\ntypedef Float *volatile Wobbly;\n'
         'Wobbly vw;\ndouble dd;\nextern double dcos(double);\nFloat *pos, *push;\n'
         'void g(Float y, struct v *p)\n{\n    {}\n}\n')

    def compounds(self, statements, route=GAME):
        """statements, in the body of g(Float y, struct v *p) beside C's declarations, as the tool writes them under
        route, the game's by default: each on one line, or None where the tool fails; and the problems."""
        text, problems = passed(self.C.replace('    {}\n', ''.join(f'    {line}\n' for line in statements)), route)
        if text is None:
            return None, problems
        lines = text.splitlines()
        first = lines.index('{') + 1
        return [squeezed(line) for line in lines[first:first + len(statements)]], problems

    def test_writes_a_plus_equals_of_a_product_holding_a_side_effect_as_an_addition(self):
        # camera_step_follow's drift, an inline function's call, and an external one's; a left operand of each
        # form the tool takes, as macros spell them too, after a brace and an if's, a for's, a while's and a switch's
        # head; a product holding the tool's wrap, ++, an assignment or a volatile object, a pointer a declarator
        # qualifies volatile too, and in parentheses, under a unary + and under a floating cast;
        # a += a call's argument holds; and a member that shares a volatile pointer's name
        written, problems = self.compounds([
            's->x += cosine(camera_work->y) * 0.2F;', 'y += scale(y) * y;', 'p->x += cosine(n) * y;',
            'list[n].x += cosine(n) * y;', 'p->a[2] += cosine(n) * y;', 'list[n].next->a[n] += cosine(n) * y;',
            '((struct v *)&list[n])->x += cosine(n) * y;', '{} ((*p)).x += cosine(n) * y;',
            'if (n) {} else { (p)->x += cosine(n) * y; }', 'do { *p->a += cosine(n) * y; } while (n);',
            'if (n) p->x += cosine(n) * y;', 'for (;;) p->x += cosine(n) * y;', 'while (n) p->x += cosine(n) * y;',
            'switch (n) { case 1: (p)->x += cosine(n) * y; }', 'y += (y - p->x * y) * y;',
            'y += p->a[n++] * y;', 'y += (n = 2) * y;', 'y += blue * y;', 'y += vp[0] * y;',
            'p->x += (cosine(n) * y);', 'p->x += +(cosine(n) * y);', 'p->x += (Float)(cosine(n) * y);',
            'scale(p->x += cosine(n) * y);', 'p->vp[0] += cosine(n) * y;'])
        drift = 'cosine(n) * y'
        self.assertEqual(written, [
            f'{captured("s->x", "cosine(camera_work->y) * 0.2F")};', 'y = y + (scale(y) * y);',
            f'{captured("p->x", drift)};', f'{captured("list[n].x", drift)};', f'{captured("p->a[2]", drift)};',
            f'{captured("list[n].next->a[n]", drift)};', f'{captured("((struct v *)&list[n])->x", drift)};',
            f'{{}} {captured("((*p)).x", drift)};', f'if (n) {{}} else {{ {captured("(p)->x", drift)}; }}',
            f'do {{ {captured("*p->a", drift)}; }} while (n);', f'if (n) {captured("p->x", drift)};',
            f'for (;;) {captured("p->x", drift)};', f'while (n) {captured("p->x", drift)};',
            f'switch (n) {{ case 1: {captured("(p)->x", drift)}; }}',
            f'y = y + ((y - {wrapped("p->x * y")}) * y);', 'y = y + (p->a[n++] * y);', 'y = y + ((n = 2) * y);',
            'y = y + (blue * y);', 'y = y + (vp[0] * y);', f'{captured("p->x", "(cosine(n) * y)")};',
            f'{captured("p->x", "+(cosine(n) * y)")};', f'{captured("p->x", "(Float)((float)(cosine(n)) * y)")};',
            f'scale({captured("p->x", drift)});', f'{captured("p->vp[0]", drift)};'])
        self.assertEqual(problems, [])

    def test_takes_a_left_operand_with_a_side_effect_beside_a_right_one_with_no_call(self):
        # player.c's *pos++ += *push++ * 0.5F, whose two increments C leaves unsequenced, and a subscript by an
        # expression
        self.assertEqual(self.compounds(['*pos++ += *push++ * 0.5F;', 'p->a[n + 1] += p->a[n++] * y;']), ([
            f'{captured("*pos++", "*push++ * 0.5F")};', f'{captured("p->a[n + 1]", "p->a[n++] * y")};'], []))

    def test_evaluates_a_left_operand_but_a_name_once_ahead_of_the_right(self):
        # a call that moves the pointer or the index the left operand reads, which a second reading would take as
        # another object, and a name, which names one object however the call runs
        c = ('extern float move(void);\nfloat slots[2], *aim, bias;\nint at;\n'
             'void f(float y)\n{\n    *aim += move() * y;\n    slots[at] += move() * y;\n    bias += move() * y;\n}\n')
        text, problems = passed(c)
        self.assertEqual([squeezed(line) for line in text.splitlines()[6:9]], [
            f'{captured("*aim", "move() * y")};', f'{captured("slots[at]", "move() * y")};',
            'bias = bias + (move() * y);'])
        self.assertEqual(problems, [])

    def test_fails_on_a_left_operand_it_doesn_t_take_beside_a_product_shc_fuses(self):
        # beside a call: a call, an increment and a decrement, a subscript by an expression, an assignment; and a
        # volatile object: a name, a member, what a pointer points to, through a typedef, an array's element and a
        # pointer a declarator qualifies volatile, a typedef of one and a member
        statements = ['f()->x += cosine(n) * y;', 'p++->x += cosine(n) * y;', '(++p)->x += cosine(n) * y;',
                      '(--p)->x += cosine(n) * y;', 'p->a[n + 1] += cosine(n) * y;', '(p = s)->x += cosine(n) * y;',
                      'body->x += cosine(n) * y;', 'pu->m += cosine(n) * y;', '*pv += cosine(n) * y;',
                      'vv->x += cosine(n) * y;', 'va[n] += cosine(n) * y;', 'vp[0] += cosine(n) * y;',
                      'vw[0] += cosine(n) * y;', 'pu->q[1] += cosine(n) * y;']
        why = ('a product SHC fuses that holds a call, which GCC fuses only as an addition, beside a left operand the '
               "tool doesn't take")
        first = self.C.count('\n') - 1
        self.assertEqual(self.compounds(statements), (None, [
            f'a.c:{first + at}: {statement[:-1]}: {why}: {left}'
            for at, (statement, left) in enumerate(zip(statements, (
                'f(), a call', 'p++, an increment', '++p, an increment', '--p, a decrement',
                'p->a[n + 1], a subscript by an expression', '(p = s), none of the left operands the tool takes',
                'body->x, a volatile object', 'pu->m, a volatile object', '*pv, a volatile object',
                'vv->x, a volatile object', 'va[n], a volatile object', 'vp, a volatile object',
                'vw, a volatile object', 'pu->q, a volatile object')))]))
        # a left operand whose type the tool can't read
        self.assertEqual(self.compounds(['q->x += cosine(n) * y;']), (None, [
            f"a.c:{first}: q->x += cosine(n) * y: a product SHC can fuse, whose types the tool can't read: q, which no "
            'declaration in scope names']))
        # a parameter a declarator qualifies volatile
        c = 'extern float cosine(short);\nvoid h(float *volatile w, short n)\n{\n    w[0] += cosine(n) * 2.0F;\n}\n'
        self.assertEqual(passed(c)[1], [f'a.c:4: w[0] += cosine(n) * 2.0F: {why}: w, a volatile object'])

    def test_fails_on_a_plus_equals_to_write_beside_a_cast_it_doesn_t_read(self):
        # as the product's rank, which the cast could widen; and writes one beside a cast to a typedef's type
        first = self.C.count('\n') - 1
        self.assertEqual(self.compounds(['p->x += (__typeof__(y)) cosine(n) * y;']), (None, [
            f"a.c:{first}: p->x += (__typeof__(y)) cosine(n) * y: a product SHC can fuse, whose types the tool can't "
            "read: __typeof__ ( y ): __typeof__, a word Types doesn't read"]))
        self.assertEqual(self.compounds(['p->x += (Float) cosine(n) * y;']),
                         ([f'{captured("p->x", "(Float) cosine(n) * y")};'], []))

    def test_leaves_a_plus_equals_gcc_fuses_as_shc_does(self):
        # a product with no side effect, one inside a run of + and - the right operand holds, an integer one, and a
        # constant one, which both fold; with a 64-bit double, a double's, which SHC computes in binary64 and FMAC
        # doesn't take, beside a float's it writes; and a system header's
        statements = ['p->x += p->a[n] * y;', 'p->x += y + cosine(n) * y;', 'n += n * n;', 'p->x += 0.1F * 3.0F;']
        self.assertEqual(self.compounds(statements), (statements, []))
        self.assertEqual(self.compounds(['dd += dcos(dd) * dd;', 'p->x += cosine(n) * y;'], WIDE),
                         (['dd += dcos(dd) * dd;', f'{captured("p->x", "cosine(n) * y")};'], []))
        c = ('# 1 "tools/sh4zam/include/sh4zam/shz_trig.h" 1 3\n'
             'static float turn(float a) { a += __builtin_sqrtf(a) * a; return a; }\n')
        self.assertEqual(passed(c), (f'# 1 "a.c"\n{c}', []))

    def test_wraps_a_product_a_plus_equals_shares(self):
        # SHC computes a * b once for x += a * b and a * b + c, and fuses it into neither, as shared_compound in
        # tests/floats/shared_reads.c does. Under -aggressive=2, a quotient by a constant two sums share is one
        # product SHC computes once, as y / 4.0F by FMUL, FADD and FADD under R13, as a product of y is
        quotient = 'y * ((__typeof__(y / 4.0F))1 / (4.0F))'
        for statements, written, route in (
                (['p->x += y * y;', 'p->a[0] = y * y + p->x;'],
                 [f'p->x += {wrapped("y * y")};', f'p->a[0] = {wrapped("y * y")} + p->x;'], GAME),
                (['p->x = y / 4.0F + p->a[0];', 'p->a[1] = y / 4.0F + p->a[2];'],
                 [f'p->x = ({{ __typeof__(y / 4.0F) __floats_p = {quotient}; __floats_p; }}) + p->a[0];',
                  f'p->a[1] = ({{ __typeof__(y / 4.0F) __floats_p = {quotient}; __floats_p; }}) + p->a[2];'],
                 AGGRESSIVE)):
            with self.subTest(statements[0]):
                self.assertEqual(self.compounds(statements, route), (written, []))

    def test_wraps_a_product_shc_holds_in_binary64(self):
        # under runtime64's flags, R13 adds a double product by FMUL DR6,DR4 and FADD DR8,DR4, where a double is a
        # float to GCC, whose FMAC would fuse it; under -fpu=single, SHC fuses both
        statements = ['dd += dd * dd;', 'dd = dd + dd * dd;']
        self.assertEqual(self.compounds(statements, WIDE),
                         ([f'dd += {wrapped("dd * dd")};', f'dd = dd + {wrapped("dd * dd")};'], []))
        self.assertEqual(self.compounds(statements), (statements, []))

    def test_wraps_the_product_of_a_plus_equals_into_an_integer(self):
        # SHC converts the integer, adds the product by FMUL and FADD, and converts back, with a call in the product
        # and without
        self.assertEqual(self.compounds(['n += y * y;', 'n += cosine(n) * y;']),
                         ([f'n += {wrapped("y * y")};', f'n += {wrapped("cosine(n) * y")};'], []))

    def test_writes_a_plus_equals_of_a_name_in_parentheses_as_a_name_s(self):
        # a register object has no address, so the addition takes none of r, in parentheses or not
        statements = ['register Float r = y;', '(r) += cosine(n) * y;', 'r += cosine(n) * y;']
        self.assertEqual(self.compounds(statements),
                         ([statements[0], '(r) = (r) + (cosine(n) * y);', 'r = r + (cosine(n) * y);'], []))

    def test_keeps_a_minus_equals_unfused(self):
        # SHC subtracts the product of a -=, by FMUL and FSUB
        self.assertEqual(self.compounds(['p->x -= cosine(n) * y;']), ([f'p->x -= {wrapped("cosine(n) * y")};'], []))


if __name__ == '__main__':
    unittest.main()
