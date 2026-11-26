"""SH4ZAM as the game and its SDK take it. Each SH4ZAM entry a source of src/, modules/ or sdk/ calls gives the
console's bits whatever its arguments, as sdk/include/ninja_zam.h says, and none of those sources marks a function
SHZ_FAST_MATH. A body that hands SH4ZAM a caller's matrix as its own type, which aligns to 8, traps in a build without
NDEBUG on one that lies off 8, and GCC warns of a structure that would hold a matrix off 8. The lists hold an entry's
name and not how a body puts entries together. engine/, the drawing engine, holds a line of its own.

With an SH-4 toolchain, the tests compile Ninja's sources as make dc does and run GCC's code in
tests/sh4.py beside references written from the image's instructions. Both use round toward zero and
tests/kernels/console.c's model of the SH-4 vector instructions. CalcPoints exercises the linked batch and its scalar fallback. Helpers runs njInnerProduct's and
njRotateZXY's non-matching sides. CalcVector runs njCalcVector's for a matrix in memory. Order runs each function whose
non-matching side spells SHC's FMACs by __builtin_fmaf, and holds each sum to the image's order: its output, and the
values its FMACs make, which an angle's truncation would hide. Fused holds each such function to as many FMACs as
the image's code runs. Each callee those functions take is a stub that runs its shipped arithmetic, but
njCheckBBox, which passes, and the bounds it would test."""

import ctypes
from itertools import product
import math
import random
import re
import struct
import subprocess
import tempfile
import unittest
from fractions import Fraction
from pathlib import Path

import sh4
from support import ROOT, SH4ZAM, SH_ELF, compile_host, gcc_flags

# the sources of play's math and of the SDK, each held to the lists
CHECKED = ('src', 'modules', 'sdk')
# a call of an SH4ZAM function or function-like macro
CALL = re.compile(r'\b((?:shz|SHZ)_\w+)\s*\(')
# a comment, and a string or character literal, which a scan for calls blanks
NOISE = re.compile(r'/\*.*?\*/|//[^\n]*|"(?:\\.|[^"\\\n])*"|\'(?:\\.|[^\'\\\n])*\'', re.S)
# the entries a source may call. each is a back-end entry, or a public one with no branch on a constant, so it runs
# the same instructions whatever its arguments. XMTRX's unaligned load and store are the exception: they move a
# matrix by pairs or by single floats as its address lies, to the same bits. shz_floorf and shz_ceilf fold a constant
# by GCC's floor and ceil, which round nothing, and mathf.h gives a zero result x's sign either way.
# shz_mat4x4_transform_vec3 is off the list: its third FIPR holds the first's result in a lane, against a 0, so its z
# is a NaN where its x is infinite or a NaN, and njCalcVector runs ninja_zam.h's zam_fipr_columns in its place
EXACT = {
    # four FIPRs, the vector first in each, of which a point keeps the first three's results
    'shz_mat4x4_transform_vec4', 'shz_mat4x4_transform_point3',
    # FSRRA and FSCA
    'shz_inv_sqrtf_fsrra_sh4', 'shz_sincosu16_sh4',
    # XMTRX's loads, stores and moves, an FTRV, the translation's FTRV, the scale's FMULs, and the turns by FSCA
    # and two FTRVs
    'shz_xmtrx_load_4x4', 'shz_xmtrx_store_4x4', 'shz_xmtrx_load_unaligned_4x4', 'shz_xmtrx_store_unaligned_4x4', 'shz_xmtrx_read_col',
    'shz_xmtrx_write_col', 'shz_xmtrx_get_translation', 'shz_xmtrx_transform_vec4', 'shz_xmtrx_transform_vec3',
    'shz_xmtrx_transform_point3', 'shz_xmtrx_translate', 'shz_xmtrx_scale', 'shz_xmtrx_rotate_x_u16', 'shz_xmtrx_rotate_y_u16', 'shz_xmtrx_rotate_z_u16',
    # XMTRX's literal identity, independent of Ninja's writable unit matrix
    'shz_xmtrx_init_identity',
    # one product, or one sum, in each lane
    'shz_vec3_scale', 'shz_vec4_scale', 'shz_vec3_add', 'shz_vec3_sub',
    # a float's integer part and a comparison, exact
    'shz_floorf', 'shz_ceilf',
    # a vector of floats as given, and a vector's floats as a value of another type of its size
    'shz_vec3_init', 'shz_vec4_init', 'shz_vec4_deref', 'shz_vec3_to',
    # bytes
    'shz_memcpy1', 'shz_memcpy2', 'shz_memcpy4', 'shz_memset8',
}
# SH4ZAM's mark that gives a function fast-math, which reorders arithmetic and so changes its bits
FAST_MATH = re.compile(r'\bSHZ_FAST_MATH\b')
# the functions that hand SH4ZAM a matrix a caller passes, as an shz_mat4x4_t, which aligns to 8: ninja_zam.h's
# helpers of njCalcPoint and njTranslate, which BOUNDARIES compiles into a function each. Each asserts that the
# matrix lies on 8
BOUNDARIES = '''\
#include <ninja.h>

void calc_point(NJS_MATRIX *m, NJS_POINT3 *p)
{
    njCalcPoint(m, p, p);
}

void translate(NJS_MATRIX *m, NJS_VECTOR *v)
{
    njTranslateV(m, v);
}
'''
# what a false assertion runs in a build without NDEBUG, as sdk/include/assert.h spells it, and GCC's test of an
# address's low three bits, which decides the trap of an assertion that the address lies on 8
TRAP = 'trapa #0xc3'
LOW_THREE = '\ttst\t#7,r0\n'
# declarations that hold an NJS_MATRIX, each with what GCC says of it under -fpack-struct=4, which aligns a member to
# 4 at most, or None where GCC says nothing: a matrix 4 past a boundary, one that starts a structure of 4's
# alignment, an array of them, a union's, one that #pragma pack(8) wraps, and a pointer to one
HOLDERS = (
    ('struct off {\n    int word;\n    NJS_MATRIX matrix;\n};\n',
     "'matrix' offset 4 in 'struct off' isn't aligned to 8"),
    ('struct under {\n    NJS_MATRIX matrix;\n    int word;\n};\n', "alignment 4 of 'struct under' is less than 8"),
    ('struct rows {\n    int word;\n    NJS_MATRIX matrices[2];\n};\n',
     "'matrices' offset 4 in 'struct rows' isn't aligned to 8"),
    ('union either {\n    NJS_MATRIX matrix;\n    int word;\n};\n', "alignment 4 of 'union either' is less than 8"),
    ('#pragma pack(push, 8)\nstruct wrapped {\n    int word;\n    NJS_MATRIX matrix;\n};\n#pragma pack(pop)\n', None),
    ('struct points {\n    int word;\n    NJS_MATRIX *matrix;\n};\n', None),
)
# the flag of make dc's that the compile tests leave out: their build holds the assertions, which traps
SHIPPING = '-DNDEBUG'
# the levels make dc compiles a source at
LEVELS = ('-Os', '-O2')
# the sources whose non-matching sides spell SHC's FMACs by __builtin_fmaf, and for each function the FMACs in the
# image's code of it, as port.sh dis counts them in 1ST_READ.BIN. Each other function of these sources holds none
FUSED = {
    'sdk/ninja/njCameraMotion.c': {'linear_angle': 1, 'linear_vector': 3, 'overhauser_vector': 9},
    'sdk/ninja/njCnkMotSrc.c': {
        'njCnkLinearMotionQ': 10, 'njCnkLinearMotionF': 3, 'njCnkLinearMotionA': 3, 'njCnkOverhauserMotionF': 9,
        'njCnkOverhauserMotionA': 9, 'njCnkBezierMotionF': 12, 'njCnkBezierMotionA': 12, 'njCnkLinearMotionAOld': 3,
        'njCnkBezierMotionAOld': 12},
    'sdk/ninja/njCnkMotLinkSrc.c': {
        'njCnkLinearMotLinkF': 3, 'njCnkLinearMotLinkA': 3, 'njAng2Quat': 4, 'njCnkOverhauserMotLinkF': 12,
        'njCnkOverhauserMotLinkA': 12, 'njCnkBezierMotLinkF': 12, 'njCnkBezierMotLinkA': 12, 'njCnkLinearMotLinkQ': 10,
        'njCnkLinearIntF': 3, 'njCnkLinearIntQ': 10, 'njCnkLinearIntA': 3, 'njCnkHermiteIntF': 9,
        'njCnkHermiteIntA': 9, 'njCnkLinearMotLinkAOld': 3, 'njCnkLinearIntAOld': 3, 'njCnkBezierMotLinkAOld': 12},
    'sdk/ninja/njCollisionCheckBC.c': {'njCollisionCheckBC': 61},
}
# the FMACs GCC's code of a function leaves out, each of a sum the shipped code works out twice from the same
# operands, two FMACs to a sum. In njCollisionCheckBC, nd is cosine again, edge_length's sum is that of t's divisor,
# and the end test's t is the axis test's
REPEATED = {'njCollisionCheckBC': 6}
# where a helper's run puts its vectors, and the stack R15 starts at
V1, V2, STACK = 0x1000, 0x2000, 0x8000
# a v1 and a v2 whose FIPR gives another sum where the operands swap places, as bits. The shipped FIPR FV4,FV0 takes v2
# as FVm and v1 as FVn, for 0x462e10ea, where v1 first gives 0x462e10e9
ORDERED = ((0x4204181a, 0x42372e1b, 0x428a12b9), (0x41d48132, 0x4299cd20, 0x42c34c50))
# njRotateZXY's turns, in the shipped body's order: for each, the index of its angle in (angx, angy, angz), and the
# two rows of XMTRX it replaces, each with the turn's row that FTRV multiplies by XMTRX, from FSCA's sine and cosine
# as bits. A turn's FNEG of a sine of 0 gives -0
TURNS = (
    (1, ((0, lambda sin, cos: [cos, 0, sin ^ sh4.SIGN, 0]), (2, lambda sin, cos: [sin, 0, cos, 0]))),
    (0, ((1, lambda sin, cos: [0, cos, sin, 0]), (2, lambda sin, cos: [0, sin ^ sh4.SIGN, cos, 0]))),
    (2, ((0, lambda sin, cos: [cos, sin, 0, 0]), (1, lambda sin, cos: [sin ^ sh4.SIGN, cos, 0, 0]))),
)
# the angles whose FSCA results are exact, or that the turns' tests of 0 tell apart
ANGLES = (0, 0x4000, 0x8000, 0xc000, 0x10000, 1, 0xffff, 0xffffffff)
# the random cases each helper runs at each level
CASES = 300
# floats as bits that a FIPR treats apart from an ordinary one: an infinity, the SH-4's one NaN, and a float whose
# product with itself lies below the least normal, which DN flushes to a zero of the product's sign
INFINITE, NAN, TINY = 0x7f800000, 0x7fbfffff, 0x19000000
# where Order links a source's object, puts the names a stub stands for, 16 bytes apart, and the other names the
# object takes, 256 bytes apart. A call returns to RETURN, and a run stops past STEPS instructions
TEXT, STUBS, DATA, RETURN, STEPS = 0x0c800000, 0x0c900000, 0x0ca00000, 0xfffffff0, 200000
# where Order puts a call's keys or values, its output, and njCollisionCheckBC's box and capsule
KEYS, OUT, BOX, CAPSULE = 0x10000, 0x11000, 0x12000, 0x13000
# the words OUT holds before a call, which a word the call leaves keeps
UNSET = 0xdeadbeef
# the random cases each fused function runs at each level, and njCollisionCheckBC, which runs longer, at -Os
SUMS, COLLISIONS = 100, 60
# floats as bits: the constants the fused functions take, and 0.025F, njDistanceP2P's and njCollisionCheckBC's
# tolerance
ZERO, ONE, HALF, QUARTER, TWO, THREE = 0, 0x3f800000, 0x3f000000, 0x3e800000, 0x40000000, 0x40400000
ONE_AND_HALF, TWO_AND_HALF, THIRD, TOLERANCE = 0x3fc00000, 0x40200000, 0x3eaaaa3b, 0x3ccccccd
# half a turn and a turn of Ninja's angles, as floats' bits and as integers
HALF_TURN_F, TURN_F, HALF_TURN, TURN = 0x47000000, 0x47800000, 0x8000, 0x10000
# the size of a struct nj_motion_info, and the offset of its frame
MOTION_INFO, FRAME = 28, 12
# njCollisionCheckBC's box faces, each by its corners in turn, and the face across each face's edge from its corner
# k to the next
CORNERS = ((0, 1, 2, 3), (4, 7, 6, 5), (0, 3, 7, 4), (2, 1, 5, 6), (0, 4, 5, 1), (3, 2, 6, 7))
ACROSS = ((4, 3, 5, 2), (2, 5, 3, 4), (0, 5, 1, 4), (0, 4, 1, 5), (2, 1, 3, 0), (0, 3, 1, 2))


def blanked(text):
    """text, C, with each comment and literal blanked to spaces, its newlines kept."""
    return NOISE.sub(lambda noise: re.sub(r'[^\n]', ' ', noise[0]), text)


def calls(path, text):
    """Each call of an SH4ZAM entry in path's text, outside comments and literals, as (path:line, entry)."""
    code = blanked(text)
    return [(f'{path}:{code.count(chr(10), 0, call.start()) + 1}', call[1]) for call in CALL.finditer(code)]


def refused(found):
    """Each of found, as calls() gives them, that EXACT lacks, as path:line: entry."""
    return [f'{at}: {entry}' for at, entry in found if entry not in EXACT]


def idle(found):
    """Each entry EXACT lists that none of found, as calls() gives them, calls, as what to do."""
    called = {entry for _, entry in found}
    return [f'tests/test_zam.py: EXACT lists {entry}, which no source calls: take it out'
            for entry in sorted(EXACT - called)]


def marked(text):
    """Whether text, a source's, marks a function SHZ_FAST_MATH, outside comments and literals."""
    return bool(FAST_MATH.search(blanked(text)))


def unchecked(assembly, functions):
    """What to do for each of functions, C names, whose code in assembly, GCC's for an SH-4 source, runs no TRAP
    with LOW_THREE ahead of it, or that assembly doesn't define. A trap that another test decides, of the address's
    low two bits or of another argument, holds no matrix to 8."""
    found = []
    for name in functions:
        defined = re.search(rf'^_{name}:\n(.*?)^\t\.size\t_{name},', assembly, re.S | re.M)
        if not defined:
            found.append(f'no {name} is defined: name in tests/test_zam.py the function that hands SH4ZAM the matrix '
                         'now')
        elif TRAP not in defined[1] or LOW_THREE not in defined[1].partition(TRAP)[0]:
            found.append(f"{name} hands SH4ZAM a caller's matrix as an shz_mat4x4_t, which aligns to 8, and runs no "
                         "trap that a test of the address's low three bits decides: assert((uintptr_t)m % 8 == 0) "
                         'ahead of the cast')
    return found


def sources():
    """The C sources and headers of CHECKED, as paths from the root."""
    return sorted(str(path.relative_to(ROOT)) for top in CHECKED for pattern in ('*.c', '*.h')
                  for path in (ROOT / top).rglob(pattern))


def gcc(*argv):
    """GCC's run of argv from the root, as the completed process: under the flags make dc gives a source of the SDK at
    -Os, config/gcc.mk's but SHIPPING, with SH4ZAM's headers where SH4ZAM names them."""
    flags = [flag for flag in gcc_flags(SH_ELF) if flag != SHIPPING]
    return subprocess.run([str(SH_ELF / 'bin/sh-elf-gcc'), *flags, '-Os', '-isystem', str(SH4ZAM / 'include'),
                           '-Isdk/include', *argv], cwd=ROOT, stdout=subprocess.PIPE, stderr=subprocess.STDOUT,
                          text=True)


class Exact(unittest.TestCase):
    maxDiff = None

    def test_each_source_calls_only_entries_the_list_allows_and_the_list_only_entries_a_source_calls(self):
        found = []
        for path in sources():
            text = (ROOT / path).read_text(errors='replace')
            if 'shz_' in text or 'SHZ_' in text:
                found += calls(path, text)
        self.assertTrue(found)
        self.assertEqual(refused(found), [])
        self.assertEqual(idle(found), [])

    def test_names_a_call_off_the_list_and_none_in_a_comment(self):
        # shz_sqrtf approximates by FSRRA, shz_fmaf is a * b + c, rounded twice where GCC fuses nothing, and
        # shz_vec3_dot sums C products, whose bits needn't match FIPR's
        text = ('/* shz_sqrtf(x) */\nfloat f(float x, shz_vec3_t a, shz_vec3_t b)\n{\n'
                '    return shz_sqrtf(x) + shz_vec3_dot(a, b) + shz_fmaf(x, x, x);\n}\n')
        self.assertEqual(refused(calls('src/a.c', text)),
                         ['src/a.c:4: shz_sqrtf', 'src/a.c:4: shz_vec3_dot', 'src/a.c:4: shz_fmaf'])
        self.assertEqual(refused(calls('src/a.c', 'char *s = "shz_sqrtf(x)";\n')), [])

    def test_names_a_listed_entry_that_no_source_calls(self):
        found = [('src/b.c:1', entry) for entry in EXACT - {'shz_memcpy2'}]
        self.assertEqual(idle(found), ['tests/test_zam.py: EXACT lists shz_memcpy2, which no source calls: take it out'])
        self.assertEqual(idle(found + [('src/c.c:9', 'shz_memcpy2')]), [])


class FastMath(unittest.TestCase):
    def test_no_source_of_play_or_the_sdk_marks_fast_math(self):
        self.assertEqual([path for path in sources() if marked((ROOT / path).read_text(errors='replace'))], [])

    def test_names_a_mark_and_none_in_a_comment_or_a_literal(self):
        self.assertTrue(marked('static SHZ_FAST_MATH float f(float x)\n{\n    return x;\n}\n'))
        self.assertFalse(marked('/* SHZ_FAST_MATH */\nfloat f(float x);\nconst char *g = "SHZ_FAST_MATH";\n'))


class Alignment(unittest.TestCase):
    def setUp(self):
        if not (SH_ELF / 'bin/sh-elf-gcc').is_file() or not (SH4ZAM / 'include/sh4zam').is_dir():
            self.skipTest('no SH-4 GCC and SH4ZAM at SH_ELF and SH4ZAM')

    def test_a_build_without_ndebug_traps_where_a_caller_hands_sh4zam_a_matrix_off_8(self):
        # an NJS_MATRIX aligns to 8 in a build of the non-matching code, as an shz_mat4x4_t does, and ninja.h says
        # what keeps one there. each function that casts a caller's matrix to SH4ZAM's type asserts it
        with tempfile.TemporaryDirectory() as scratch:
            boundaries = Path(scratch) / 'boundaries.c'
            boundaries.write_text(BOUNDARIES)
            compiled = gcc('-S', '-o', '-', str(boundaries))
            self.assertEqual(compiled.returncode, 0, compiled.stdout)
            self.assertEqual(unchecked(compiled.stdout, ['calc_point', 'translate']), [])

    def test_gcc_warns_of_a_structure_that_holds_a_matrix_off_8(self):
        # -fpack-struct=4 would put a structure's matrix on 4, and a body handed its address would give SH4ZAM a
        # matrix off 8. ninja.h has GCC warn of the structure, and a build that takes -Werror fails on the warning
        with tempfile.TemporaryDirectory() as scratch:
            holder = Path(scratch) / 'holder.c'
            for text, said in HOLDERS:
                holder.write_text(f'#include <ninja.h>\n\n{text}')
                compiled = gcc('-fsyntax-only', str(holder))
                if said is None:
                    self.assertEqual(compiled.returncode, 0, compiled.stdout)
                else:
                    self.assertIn(f'error: {said} [-Werror=if-not-aligned]', compiled.stdout, text)


class Trap(unittest.TestCase):
    """unchecked() on assembly written here, which needs no toolchain."""

    def test_names_a_function_that_holds_no_matrix_to_8_and_one_the_assembly_lacks(self):
        trapped = ('_first:\n\tmov\tr4,r0\n\ttst\t#7,r0\n\tbt\t.L2\n.L3:\n\ttrapa #0xc3\n\tbra\t.L3\n\tnop\n'
                   '.L2:\n\trts\t\n\tnop\n\t.size\t_first, .-_first\n')
        bare = '_second:\n\trts\t\n\tnop\n\t.size\t_second, .-_second\n'

        def lacks(name):
            return (f"{name} hands SH4ZAM a caller's matrix as an shz_mat4x4_t, which aligns to 8, and runs no trap "
                    "that a test of the address's low three bits decides: assert((uintptr_t)m % 8 == 0) ahead of the "
                    'cast')
        self.assertEqual(unchecked(trapped + bare, ['first']), [])
        # a trap in the function before it, or after it, is none of its own
        self.assertEqual(unchecked(trapped + bare + trapped.replace('first', 'third'), ['first', 'second', 'third']),
                         [lacks('second')])
        # a trap for an address off 4, one for a null argument, and a test of the low three bits that comes after
        # the trap hold no matrix to 8
        for name, old, new in (('on_4', '#7,r0', '#3,r0'), ('null', '\ttst\t#7,r0', '\ttst\tr5,r5'),
                               ('after', '\ttst\t#7,r0\n\tbt\t.L2\n.L3:\n\ttrapa #0xc3\n',
                                '\ttst\tr5,r5\n\tbt\t.L2\n.L3:\n\ttrapa #0xc3\n\ttst\t#7,r0\n')):
            self.assertEqual(unchecked(trapped.replace('first', name).replace(old, new), [name]), [lacks(name)])
        self.assertEqual(unchecked(trapped, ['second']),
                         ['no second is defined: name in tests/test_zam.py the function that hands SH4ZAM the matrix '
                          'now'])


def compile_console(output):
    """tests/kernels/console.c as a host library at output."""
    compile_host(output, '-std=gnu17', '-O1', '-Itests/kernels', '-Iengine/dc', 'tests/kernels/console.c', '-lm',
                 shared=True)


def compile_sh4(output, source, level):
    """GCC's object of source at level, at output, under the flags make dc gives a source of the SDK. Returns GCC's
    complaint, or None."""
    built = subprocess.run([str(SH_ELF / 'bin/sh-elf-gcc'), *gcc_flags(SH_ELF), level, '-isystem',
                            str(SH4ZAM / 'include'), '-Isdk/include', '-I.', '-c', '-o', str(output), source], cwd=ROOT,
                           stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True)
    return built.stdout if built.returncode or built.stdout else None


def toolchain():
    """Skip the test unless SH_ELF holds an SH-4 GCC and objdump, and SH4ZAM its headers."""
    if not (SH_ELF / 'bin/sh-elf-gcc').is_file() or not (SH_ELF / 'bin/sh-elf-objdump').is_file() \
            or not (SH4ZAM / 'include/sh4zam').is_dir():
        raise unittest.SkipTest('no SH-4 GCC, objdump and SH4ZAM headers at SH_ELF and SH4ZAM')


def bits(value):
    """A float's bits, as a single float holds it."""
    return struct.unpack('<I', struct.pack('<f', value))[0]


def fsub(a, b):
    """FSUB's a - b, as bits: an exact zero difference is +0, but -0 for -0 less +0."""
    return sh4.toward_zero(sh4.exact(a) - sh4.exact(b), a & ~b & sh4.SIGN)


def fdiv(a, b):
    """FDIV's a / b, as bits. A divisor of 0 stops the run, as sh4.py stops at an operation it doesn't model."""
    if sh4.exact(b) == 0:
        raise sh4.Unmodeled('a division by 0')
    return sh4.toward_zero(sh4.exact(a) / sh4.exact(b), (a ^ b) & sh4.SIGN)


def fsqrt(a):
    """FSQRT's root of a, as bits: the float at or below the exact root. A negative a stops the run."""
    value = sh4.exact(a)
    if value < 0:
        raise sh4.Unmodeled('a root of a negative')
    scaled = value.numerator * 2 ** 200 // value.denominator
    return sh4.toward_zero(Fraction(math.isqrt(scaled), 2 ** 100), a & sh4.SIGN)


def ffloat(word):
    """FLOAT's float of word, a signed 32-bit integer as bits."""
    return sh4.toward_zero(Fraction(word - (1 << 32) if word & sh4.SIGN else word), 0)


def ftrc(a):
    """FTRC's integer of a, toward 0, as 32 bits: one past the range gives its nearer end."""
    whole = max(-(1 << 31), min((1 << 31) - 1, int(sh4.exact(a))))
    return whole & 0xffffffff


def fgreater(a, b):
    """FCMP/GT's test of whether a is greater than b."""
    return sh4.exact(a) > sh4.exact(b)


def signed(word):
    """A 32-bit word as a signed integer."""
    return word - (1 << 32) if word & sh4.SIGN else word


class Machine(sh4.Machine):
    """tests/sh4.py's machine, taught what GCC's code of the fused functions runs: the loads and stores of each
    width and addressing mode, the integer arithmetic, compares and shifts, calls and returns, and FSUB, FDIV, FSQRT,
    FLOAT, FTRC, FABS and the float compares. code maps each address of a linked object to its instruction, and
    stubs an address to a Python function that stands in for what a call there runs. fused logs each FMAC's and
    FDIV's result, as (mnemonic, bits)."""

    def __init__(self, fpu):
        super().__init__(fpu)
        self.pc = 0
        self.pr = 0
        self.macl = 0
        self.code = {}
        self.stubs = {}
        self.fused = []

    def run(self, entry, arguments=(), floats=()):
        """Call the function at entry as GCC calls one: R4 to R7 take arguments, and FR4 on floats. Returns R0
        once the function returns to its caller."""
        for i, argument in enumerate(arguments):
            self.r[4 + i] = argument & 0xffffffff
        for i, argument in enumerate(floats):
            self.front[4 + i] = argument
        self.pr = RETURN
        pc = entry
        while pc != RETURN:
            if pc in self.stubs:
                self.stubs[pc](self)
                pc = self.pr
                continue
            if self.counted > STEPS:
                raise sh4.Unmodeled('a run past its steps')
            taken = self.execute(pc)
            if taken is None:
                pc += 2
            elif taken[0] == 'now':
                pc = taken[1]
            else:
                if self.execute(pc + 2) is not None:
                    raise sh4.Unmodeled(f'{pc + 2:#x}: a branch in a slot')
                pc = {'slot': lambda: taken[1], 'past': lambda: pc + 4, 'return': lambda: self.pr}[taken[0]]()
        return self.r[0]

    def words(self, address, count):
        """The count words from address."""
        return [self.load(address + 4 * i) for i in range(count)]

    def execute(self, pc):
        """Run the instruction at pc, as step runs one."""
        self.pc = pc
        if pc not in self.code:
            raise sh4.Unmodeled(f'no code at {pc:#x}')
        mnemonic, operands = self.code[pc]
        return self.step((pc, mnemonic, operands))

    def value(self, operand):
        """An operand's value: #imm, or a register's."""
        return int(operand[1:], 0) & 0xffffffff if operand.startswith('#') else self.r[self.number(operand)]

    def address(self, operand, size):
        """The address an operand names, after the change @Rn+ or @-Rn makes to Rn. A bare address is
        objdump's spelling of a PC-relative operand."""
        if re.fullmatch(r'[0-9a-f]+', operand):
            return int(operand, 16)
        moved = re.fullmatch(r'@(-?)r(\d+)(\+?)', operand)
        if moved:
            n = int(moved[2])
            if moved[1]:
                self.r[n] = (self.r[n] - size) & 0xffffffff
            found = self.r[n]
            if moved[3]:
                self.r[n] = (self.r[n] + size) & 0xffffffff
            return found
        indexed = re.fullmatch(r'@\(r0,r(\d+)\)', operand)
        if indexed:
            return (self.r[0] + self.r[int(indexed[1])]) & 0xffffffff
        displaced = re.fullmatch(r'@\((\d+),r(\d+)\)', operand)
        if displaced:
            return (int(displaced[1]) + self.r[int(displaced[2])]) & 0xffffffff
        raise sh4.Unmodeled(f'an operand: {operand}')

    def move(self, source, target, size):
        """MOV.B, MOV.W or MOV.L: a load sign-extends to 32 bits."""
        if source.startswith('r') and not target.startswith('r'):
            self.store_sized(self.address(target, size), self.r[self.number(source)], size)
            return
        loaded = self.load_sized(self.address(source, size), size)
        top = 1 << (8 * size - 1)
        self.r[self.number(target)] = (loaded - 2 * top if loaded & top else loaded) & 0xffffffff

    def load_sized(self, address, size):
        if address % size:
            raise sh4.Unmodeled(f'a load off {size}, at {address:#x}')
        return int.from_bytes(bytes(self.memory.get(address + i, 0) for i in range(size)), 'little')

    def store_sized(self, address, word, size):
        if address % size:
            raise sh4.Unmodeled(f'a store off {size}, at {address:#x}')
        for i, byte in enumerate((word & ((1 << 8 * size) - 1)).to_bytes(size, 'little')):
            self.memory[address + i] = byte
            self.written.add(address + i)

    def do_mov_l(self, source, target):
        self.move(source, target, 4)

    def do_mov_w(self, source, target):
        self.move(source, target, 2)

    def do_mov_b(self, source, target):
        self.move(source, target, 1)

    def do_mova(self, source, target):
        self.r[0] = int(source, 16)

    def do_sts(self, source, target):
        self.r[self.number(target)] = {'fpul': self.fpul, 'macl': self.macl, 'pr': self.pr}[source]

    def do_sts_l(self, source, target):
        self.store(self.address(target, 4), {'pr': self.pr, 'fpul': self.fpul, 'macl': self.macl}[source])

    def do_lds_l(self, source, target):
        loaded = self.load(self.address(source, 4))
        if target == 'pr':
            self.pr = loaded
        elif target == 'fpul':
            self.fpul = loaded
        else:
            raise sh4.Unmodeled(f'lds.l {source},{target}')

    def do_jsr(self, target):
        self.pr = self.pc + 4
        return ('slot', self.r[self.number(target)])

    def do_jmp(self, target):
        return ('slot', self.r[self.number(target)])

    def do_bsr(self, target):
        self.pr = self.pc + 4
        return ('slot', int(target, 16))

    def arithmetic(self, target, result):
        self.r[self.number(target)] = result & 0xffffffff

    def do_sub(self, source, target):
        self.arithmetic(target, self.r[self.number(target)] - self.value(source))

    def do_neg(self, source, target):
        self.arithmetic(target, -self.value(source))

    def do_and(self, source, target):
        self.arithmetic(target, self.r[self.number(target)] & self.value(source))

    def do_or(self, source, target):
        self.arithmetic(target, self.r[self.number(target)] | self.value(source))

    def do_xor(self, source, target):
        self.arithmetic(target, self.r[self.number(target)] ^ self.value(source))

    def do_not(self, source, target):
        self.arithmetic(target, ~self.value(source))

    def do_addc(self, source, target):
        total = self.r[self.number(target)] + self.value(source) + self.t
        self.arithmetic(target, total)
        self.t = total >> 32

    def do_subc(self, source, target):
        total = self.r[self.number(target)] - self.value(source) - self.t
        self.arithmetic(target, total)
        self.t = int(total < 0)

    def do_mul_l(self, source, target):
        self.macl = (self.value(source) * self.r[self.number(target)]) & 0xffffffff

    def do_extu_w(self, source, target):
        self.arithmetic(target, self.value(source) & 0xffff)

    def do_extu_b(self, source, target):
        self.arithmetic(target, self.value(source) & 0xff)

    def do_exts_w(self, source, target):
        self.arithmetic(target, ((self.value(source) & 0xffff) ^ 0x8000) - 0x8000)

    def do_exts_b(self, source, target):
        self.arithmetic(target, ((self.value(source) & 0xff) ^ 0x80) - 0x80)

    def do_shll(self, target):
        self.t = self.r[self.number(target)] >> 31
        self.arithmetic(target, self.r[self.number(target)] << 1)

    do_shal = do_shll

    def do_swap_b(self, source, target):
        value = self.value(source)
        self.arithmetic(target, (value & 0xffff0000) | ((value & 0xff) << 8) | ((value >> 8) & 0xff))

    def do_shld(self, source, target):
        count = self.value(source)
        value = self.value(target)
        if count & sh4.SIGN:
            self.arithmetic(target, value >> (1 + (~count & 31)))
        else:
            self.arithmetic(target, value << (count & 31))

    def do_shlr(self, target):
        self.t = self.r[self.number(target)] & 1
        self.arithmetic(target, self.r[self.number(target)] >> 1)

    def do_shar(self, target):
        self.t = self.r[self.number(target)] & 1
        self.arithmetic(target, signed(self.r[self.number(target)]) >> 1)

    def do_shll2(self, target):
        self.arithmetic(target, self.r[self.number(target)] << 2)

    def do_shll8(self, target):
        self.arithmetic(target, self.r[self.number(target)] << 8)

    def do_shll16(self, target):
        self.arithmetic(target, self.r[self.number(target)] << 16)

    def do_shlr2(self, target):
        self.arithmetic(target, self.r[self.number(target)] >> 2)

    def do_shlr8(self, target):
        self.arithmetic(target, self.r[self.number(target)] >> 8)

    def do_shlr16(self, target):
        self.arithmetic(target, self.r[self.number(target)] >> 16)

    def do_movt(self, target):
        self.r[self.number(target)] = self.t

    def do_tst(self, source, target):
        self.t = int(self.value(source) & self.r[self.number(target)] == 0)

    def do_cmp_eq(self, source, target):
        self.t = int(self.value(source) == self.r[self.number(target)])

    def do_cmp_gt(self, source, target):
        self.t = int(signed(self.r[self.number(target)]) > signed(self.value(source)))

    def do_cmp_ge(self, source, target):
        self.t = int(signed(self.r[self.number(target)]) >= signed(self.value(source)))

    def do_cmp_hi(self, source, target):
        self.t = int(self.r[self.number(target)] > self.value(source))

    def do_cmp_hs(self, source, target):
        self.t = int(self.r[self.number(target)] >= self.value(source))

    def do_cmp_pl(self, target):
        self.t = int(signed(self.r[self.number(target)]) > 0)

    def do_cmp_pz(self, target):
        self.t = int(signed(self.r[self.number(target)]) >= 0)

    def do_fmov(self, source, target):
        if self.sz:
            super().do_fmov(source, target)
        elif source.startswith('@'):
            self.front[self.number(target)] = self.load(self.address(source, 4))
        elif target.startswith('@'):
            self.store(self.address(target, 4), self.front[self.number(source)])
        else:
            self.front[self.number(target)] = self.front[self.number(source)]

    do_fmov_s = do_fmov

    def do_fsub(self, source, target):
        n = self.number(target)
        self.front[n] = fsub(self.front[n], self.front[self.number(source)])

    def do_fdiv(self, source, target):
        n = self.number(target)
        self.front[n] = fdiv(self.front[n], self.front[self.number(source)])
        self.fused.append(('fdiv', self.front[n]))

    def do_fmac(self, f0, source, target):
        super().do_fmac(f0, source, target)
        self.fused.append(('fmac', self.front[self.number(target)]))

    def do_fsqrt(self, target):
        n = self.number(target)
        self.front[n] = fsqrt(self.front[n])

    def do_fabs(self, target):
        self.front[self.number(target)] &= ~sh4.SIGN

    def do_float(self, source, target):
        self.front[self.number(target)] = ffloat(self.fpul)

    def do_ftrc(self, source, target):
        self.fpul = ftrc(self.front[self.number(source)])

    def do_fcmp_gt(self, source, target):
        self.t = int(fgreater(self.front[self.number(target)], self.front[self.number(source)]))

    def do_fcmp_eq(self, source, target):
        self.t = int(sh4.exact(self.front[self.number(target)]) == sh4.exact(self.front[self.number(source)]))


class Console:
    """console.c's FSCA, FIPR and FTRV, as tests/sh4.py's Machine takes them: bits in and bits out."""

    def __init__(self, library):
        self.library = ctypes.CDLL(str(library))
        self.library.console_fipr.restype = ctypes.c_uint32

    def fsca(self, angle):
        pair = (ctypes.c_uint32 * 2)()
        self.library.console_fsca(ctypes.c_uint32(angle), pair)
        return pair[0], pair[1]

    def fipr(self, m, n):
        return self.library.console_fipr((ctypes.c_uint32 * 4)(*m), (ctypes.c_uint32 * 4)(*n))

    def ftrv(self, xmtrx, v):
        vector = (ctypes.c_uint32 * 4)(*v)
        self.library.console_ftrv((ctypes.c_uint32 * 16)(*xmtrx), vector)
        return list(vector)


def turned(console, xmtrx, angles):
    """XMTRX after the shipped njRotateZXY's turns of the current matrix, as TURNS gives them: angles holds angx,
    angy and angz, and an angle of 0 skips its turn, as the body's TST does. Both FTRVs of a turn read XMTRX before
    the turn writes either row."""
    xmtrx = list(xmtrx)
    for angle, rows in TURNS:
        if angles[angle] & 0xffffffff == 0:
            continue
        sin, cos = console.fsca(angles[angle])
        products = [(row, console.ftrv(xmtrx, made(sin, cos))) for row, made in rows]
        for row, product in products:
            xmtrx[4 * row:4 * row + 4] = product
    return xmtrx


class Helpers(unittest.TestCase):
    """GCC's code of njInnerProduct's and njRotateZXY's non-matching sides, at each of LEVELS, against the shipped
    instructions on the silicon's model."""

    @classmethod
    def setUpClass(cls):
        toolchain()
        scratch = tempfile.TemporaryDirectory()
        cls.addClassCleanup(scratch.cleanup)
        library = Path(scratch.name) / 'console.so'
        compile_console(library)
        cls.console = Console(library)
        cls.code = {}
        for level in LEVELS:
            cls.code[level] = {}
            for source in ('sdk/ninja/njInnerProduct.c', 'sdk/ninja/njRotateZXY.c'):
                obj = Path(scratch.name) / f'{Path(source).stem}{level}.o'
                failed = compile_sh4(obj, source, level)
                if failed:
                    raise AssertionError(f'{source} at {level}:\n{failed}')
                cls.code[level].update(sh4.functions(SH_ELF / 'bin/sh-elf-objdump', obj))

    def setUp(self):
        self.random = random.Random(0x2a3)

    def float_bits(self):
        """A float of either sign whose magnitude lies from 0.01 to 100, or now and then 0."""
        if self.random.random() < 1 / 16:
            return self.random.choice((0, sh4.SIGN))
        return bits(self.random.uniform(0.01, 100.0)) ^ self.random.choice((0, sh4.SIGN))

    def call(self, level, name, xmtrx, arguments, memory=()):
        """Run name's code at level, as GCC's code calls a function: every register but R15 and XMTRX holds junk,
        the floats NaNs. memory gives (address, words) to put first. Checks that R8 to R15, FR12 to FR15, SZ and FR
        come back as they went. Returns the machine."""
        machine = Machine(self.console)
        machine.r = [0xdead0000 + n for n in range(15)] + [STACK]
        machine.banks = [[0x7fc10000 + 16 * bank + n for n in range(16)] for bank in range(2)]
        machine.banks[1][:] = xmtrx
        for address, words in memory:
            for i, word in enumerate(words):
                machine.store(address + 4 * i, word)
        kept = (machine.r[8:], machine.banks[0][12:])
        machine.call(self.code[level][name], arguments)
        self.assertEqual((machine.r[8:], machine.front[12:], machine.sz, machine.fr), (*kept, 0, 0))
        return machine

    def test_inner_product_takes_v2_first_as_the_shipped_fipr_does(self):
        v1, v2 = ORDERED
        self.assertNotEqual(self.console.fipr([*v2, 0], [*v1, 0]), self.console.fipr([*v1, 0], [*v2, 0]))
        cases = [ORDERED] + [(tuple(self.float_bits() for _ in range(3)), tuple(self.float_bits() for _ in range(3)))
                             for _ in range(CASES)]
        for level in LEVELS:
            for v1, v2 in cases:
                with self.subTest(level=level, v1=v1, v2=v2):
                    machine = self.call(level, 'njInnerProduct', [0] * 16, [V1, V2], [(V1, v1), (V2, v2)])
                    self.assertEqual(machine.front[0], self.console.fipr([*v2, 0], [*v1, 0]))

    def test_rotate_zxy_turns_the_current_matrix_as_the_shipped_body_does(self):
        for level in LEVELS:
            for case in range(CASES):
                xmtrx = [self.float_bits() for _ in range(16)]
                angles = [self.random.choice(ANGLES) if self.random.random() < 0.3 else self.random.getrandbits(32)
                          for _ in range(3)]
                with self.subTest(level=level, case=case, angles=angles):
                    machine = self.call(level, 'njRotateZXY', xmtrx, [0, *angles])
                    self.assertEqual(machine.xmtrx, turned(self.console, xmtrx, angles))


def turned_by(console, matrix, vs):
    """The shipped njCalcVector's x, y and z for a matrix in memory, as bits: three FIPRs, each of vs and a 0 as FVm
    with a column of matrix as FVn, a 0 in place of the column's last float."""
    return [console.fipr([*vs, 0], [matrix[k], matrix[4 + k], matrix[8 + k], 0]) for k in range(3)]


def of_columns(columns):
    """A matrix whose first three columns hold columns' floats, three to a column. Each float the shipped
    njCalcVector doesn't read is a NaN."""
    return [columns[k][row] if row < 3 and k < 3 else NAN for row in range(4) for k in range(4)]


def edges():
    """njCalcVector's cases at the edges of FIPR, as (matrix, vs): an x that is an infinity or a NaN, a zero of
    either sign in every lane, a z whose products are all zeros beside an x of either sign or either zero, and sums
    that cancel or fall below the least normal."""
    one, less, signs = ONE, ONE | sh4.SIGN, (0, sh4.SIGN)
    rest, ones = [(TWO, THREE, HALF), (HALF, TWO, THREE)], (one, one, one)
    found = []
    # an infinity or a NaN in the first column or in vs, an infinity times 0, and infinities of both signs
    for special in (INFINITE, INFINITE | sh4.SIGN, NAN):
        found.append((of_columns([(special, TWO, THREE), *rest]), ones))
        found.append((of_columns([(TWO, THREE, HALF), *rest]), (special, one, one)))
    found.append((of_columns([(INFINITE, TWO, THREE), *rest]), (0, one, one)))
    found.append((of_columns([(INFINITE, INFINITE | sh4.SIGN, THREE), *rest]), ones))
    # a zero of either sign in every lane of every FIPR
    for vs in product(signs, repeat=3):
        for column in product(signs, repeat=3):
            found.append((of_columns([column] * 3), vs))
    # an x of 1 or -1, or one that falls to a zero of either sign, beside a z whose products are all zeros
    for x, first in ((one, one), (less, one), (TINY, TINY), (TINY | sh4.SIGN, TINY)):
        for y, z in product(signs, repeat=2):
            for third in product(signs, repeat=3):
                found.append((of_columns([(first, 0, 0), (0, 0, 0), third]), (x, y, z)))
    # products that cancel, and one that falls below the least normal, of either sign
    for sign in signs:
        found.append((of_columns([(one ^ sign, less ^ sign, 0)] * 3), (one, one, 0)))
        found.append((of_columns([(TINY ^ sign, 0, 0)] * 3), (TINY, 0, 0)))
    return found


class CalcVector(unittest.TestCase):
    """GCC's code of njCalcVector for a matrix in memory, at each of LEVELS, against the shipped body's three FIPRs
    on the silicon's model. _nj_matrix_flag_ stays 0, as src/main.c leaves it, so no run scales its result."""

    @classmethod
    def setUpClass(cls):
        toolchain()
        scratch = tempfile.TemporaryDirectory()
        cls.addClassCleanup(scratch.cleanup)
        library = Path(scratch.name) / 'console.so'
        compile_console(library)
        cls.console = Console(library)
        cls.linked = {}
        for level in LEVELS:
            obj = Path(scratch.name) / f'njCalcVector{level}.o'
            failed = compile_sh4(obj, 'sdk/ninja/njCalcVector.c', level)
            if failed:
                raise AssertionError(f'sdk/ninja/njCalcVector.c at {level}:\n{failed}')
            cls.linked[level] = linked(obj, {})

    def setUp(self):
        self.random = random.Random(0xca1c)

    def float_bits(self):
        """A float of either sign whose magnitude lies from 0.01 to 100, or now and then 0."""
        if self.random.random() < 1 / 16:
            return self.random.choice((0, sh4.SIGN))
        return bits(self.random.uniform(0.01, 100.0)) ^ self.random.choice((0, sh4.SIGN))

    def call(self, level, matrix, vs, at, out):
        """Run njCalcVector's code at level on matrix at at, V2 or past it, and vs at V1, writing to out, as
        Helpers.call runs a function. Checks that R8 to R15, FR12 to FR15, XMTRX, SZ and FR come back as they went,
        and that the run stores below V2 to out's three floats alone. Returns the machine."""
        code, image, names = self.linked[level]
        machine = Machine(self.console)
        machine.code = code
        machine.memory = dict(image)
        machine.r = [0xdead0000 + n for n in range(15)] + [STACK]
        machine.banks = [[0x7fc10000 + 16 * bank + n for n in range(16)] for bank in range(2)]
        for address, words in ((at, matrix), (V1, vs), (names['__nj_matrix_flag_'], [0])):
            for i, word in enumerate(words):
                machine.store(address + 4 * i, word)
        machine.written.clear()
        kept = (machine.r[8:], machine.banks[0][12:], list(machine.banks[1]))
        machine.run(names['njCalcVector'], (at, V1, out))
        self.assertEqual((machine.r[8:], machine.front[12:], machine.xmtrx, machine.sz, machine.fr), (*kept, 0, 0))
        self.assertEqual({address for address in machine.written if address < V2}, set(range(out, out + 12)))
        return machine

    def test_a_matrix_in_memory_turns_a_vector_by_the_shipped_three_fiprs(self):
        special = (0, sh4.SIGN, ONE, ONE | sh4.SIGN, TINY, TINY | sh4.SIGN, sh4.LARGEST, sh4.LARGEST | sh4.SIGN,
                   INFINITE, INFINITE | sh4.SIGN, NAN)
        cases = edges()
        cases += [([self.float_bits() for _ in range(16)], [self.float_bits() for _ in range(3)])
                  for _ in range(CASES)]
        cases += [([self.random.choice(special) for _ in range(16)], [self.random.choice(special) for _ in range(3)])
                  for _ in range(CASES)]
        # the cases reach an x of each infinity, of the NaN and of each zero, each beside a z that is another float
        beside = {x for x, _, z in (turned_by(self.console, matrix, vs) for matrix, vs in cases) if z != x}
        self.assertLessEqual({INFINITE, INFINITE | sh4.SIGN, NAN, 0, sh4.SIGN}, beside)
        for level in LEVELS:
            for case, (matrix, vs) in enumerate(cases):
                # every other case writes over vs, as a caller that turns a vector in place has it. every other
                # pair holds its matrix off 8, where the shipped body's single loads read it too
                out = V1 if case % 2 else V1 + 0x100
                at = V2 + 4 * (case // 2 % 2)
                with self.subTest(level=level, case=case, matrix=matrix, vs=vs):
                    machine = self.call(level, matrix, vs, at, out)
                    self.assertEqual(machine.words(out, 3), turned_by(self.console, matrix, vs))


class MatrixStack(unittest.TestCase):
    def test_rigid_tree_keeps_siblings_in_the_parent_coordinate_system(self):
        toolchain()
        with tempfile.TemporaryDirectory() as scratch:
            root = Path(scratch)
            for level in LEVELS:
                objects = []
                for name in ('njCnkModelS', 'njRotateEx'):
                    obj = root / (name + '.o')
                    self.assertIsNone(compile_sh4(obj, 'sdk/ninja/' + name + '.c', level))
                    objects.append(obj)
                obj = root / 'simple.o'
                subprocess.run([SH_ELF / 'bin/sh-elf-ld', '-EL', '-r', '-o', obj, *objects], check=True)
                code, image, names = linked(obj, {})
                machine = Machine(None)
                machine.code = code
                machine.memory = dict(image)
                machine.r[15] = STACK
                machine.banks[1][:] = initial = [ONE + i for i in range(16)]
                matrix = V2
                machine.store(names['__nj_current_matrix_ptr_'], matrix)
                machine.store(names['__nj_stack_size_'], 1)
                machine.store(names['__nj_max_stack_size_'], 3)
                nodes = [0x4000 + i * 52 for i in range(6)]
                # Nodes 0, 1 and 2 need a transform. At capacity, 2's child
                # is omitted, its identity sibling draws, then 1's sibling
                # can use the slot released by 1. All angles are zero.
                for i, (flags, child, sibling) in enumerate((
                        (5, nodes[1], 0), (5, nodes[2], nodes[5]), (5, nodes[3], nodes[4]),
                        (0x17, 0, 0), (0x17, 0, 0), (0x15, 0, 0))):
                    machine.store(nodes[i], flags)
                    machine.store(nodes[i] + 4, i + 1)
                    machine.store(nodes[i] + 44, child)
                    machine.store(nodes[i] + 48, sibling)
                visited = []

                def draw_model(cpu):
                    visited.append((cpu.r[4], cpu.load(names['__nj_stack_size_'])))
                    cpu.r[0] = 0

                machine.stubs[names['_njCnkSimpleDrawModel']] = draw_model
                machine.run(names['_njCnkSimpleDrawObject'], (nodes[0],))
                self.assertEqual(visited, [(1, 2), (2, 3), (5, 3), (6, 3)])
                self.assertEqual(machine.load(names['__nj_stack_size_']), 1)
                self.assertEqual(machine.load(names['__nj_current_matrix_ptr_']), matrix)
                self.assertEqual(machine.xmtrx, initial)

    def test_public_stack_keeps_the_base_and_live_xmtrx(self):
        toolchain()
        with tempfile.TemporaryDirectory() as scratch:
            root = Path(scratch)
            for level in LEVELS:
                objects = []
                for name in ('njInitMatrix', 'njPushMatrixEx', 'njPopMatrix', 'njPopMatrixEx'):
                    obj = root / (name + '.o')
                    self.assertIsNone(compile_sh4(obj, 'sdk/ninja/' + name + '.c', level))
                    objects.append(obj)
                obj = root / 'stack.o'
                subprocess.run([SH_ELF / 'bin/sh-elf-ld', '-EL', '-r', '-o', obj, *objects], check=True)
                code, image, names = linked(obj, {})
                machine = Machine(None)
                machine.code = code
                machine.memory = dict(image)
                machine.r[15] = STACK
                base = V2
                for i in range(16):
                    machine.store(names['__nj_unit_matrix_'] + 4 * i, ONE if i % 5 == 0 else 0)
                machine.run(names['njInitMatrix'], (base, 4, 7))
                reference = [ONE if i % 5 == 0 else 0 for i in range(16)]
                reference[0] ^= sh4.SIGN
                reference[5] ^= sh4.SIGN
                self.assertEqual(machine.xmtrx, reference)

                def state():
                    return tuple(machine.load(names[n]) for n in (
                        '__nj_stack_size_', '__nj_current_matrix_ptr_', '__nj_base_matrix_ptr_',
                        '__nj_max_stack_size_', '__nj_matrix_flag_')) + (list(machine.xmtrx),)

                def refused(name, *args):
                    before = state()
                    self.assertEqual(machine.run(names[name], args), 0)
                    self.assertEqual(state(), before)

                for address, capacity in ((0, 4), (base + 8, 4), (base, 0), (base, -1)):
                    before = state()
                    machine.run(names['njInitMatrix'], (address, capacity, 1))
                    self.assertEqual(state(), before)
                matrices = []
                for slot in range(3):
                    matrix = [ONE + 100 * slot + i for i in range(16)]
                    machine.banks[1][:] = matrix
                    matrices.append(matrix)
                    self.assertEqual(machine.run(names['njPushMatrixEx']), 1)
                    self.assertEqual(machine.words(base + 64 * slot, 16), matrix)
                machine.banks[1][:] = [TWO] * 16
                refused('njPushMatrixEx')
                for count in (4, 5, 0xffffffff):
                    refused('njPopMatrix', count)
                before = state()
                self.assertEqual(machine.run(names['njPopMatrix'], (0,)), 1)
                self.assertEqual(state(), before)
                self.assertEqual(machine.run(names['njPopMatrix'], (1,)), 1)
                self.assertEqual(machine.xmtrx, matrices[2])
                self.assertEqual(machine.run(names['njPopMatrix'], (2,)), 1)
                self.assertEqual(state(), (1, base, base, 4, 7, matrices[0]))
                refused('njPopMatrixEx')
                self.assertEqual((machine.fr, machine.sz), (0, 0))


class MatrixTransfers(unittest.TestCase):
    def test_machine_intrinsics_select_the_transfer_width_of_each_pointer(self):
        toolchain()

        class Transfers(Machine):
            pair_loads = 0
            pair_stores = 0

            def fmov_pair(self, source, target):
                self.pair_loads += source.startswith('@')
                self.pair_stores += target.startswith('@')
                super().fmov_pair(source, target)

        with tempfile.TemporaryDirectory() as scratch:
            root = Path(scratch)
            source = root / 'transfer.c'
            source.write_text('#include <machine.h>\n'
                              'void transfer(float in[4][4], float out[4][4]) { ld_ext(in); st_ext(out); }\n')
            for level in LEVELS:
                obj = root / 'transfer.o'
                self.assertIsNone(compile_sh4(obj, source, level))
                code, image, names = linked(obj, {})
                for src in (V1, V1 + 4):
                    for dst in (V2, V2 + 4):
                        with self.subTest(level=level, src=src, dst=dst):
                            machine = Transfers(None)
                            machine.code = code
                            machine.memory = dict(image)
                            machine.r[15] = STACK
                            words = [0x7f800001 + i for i in range(16)]
                            for i, word in enumerate(words):
                                machine.store(src + 4 * i, word)
                            machine.store(dst - 4, 23)
                            machine.store(dst + 64, 47)
                            machine.run(names['transfer'], (src, dst))
                            self.assertEqual(machine.words(dst - 4, 18), [23, *words, 47])
                            self.assertEqual(machine.xmtrx, words)
                            self.assertEqual((machine.fr, machine.sz), (0, 0))
                            self.assertEqual(machine.pair_loads, 8 if src == V1 else 0)
                            self.assertEqual(machine.pair_stores, 8 if dst == V2 else 0)


class CalcPoints(unittest.TestCase):
    def test_current_and_explicit_matrices_with_overlapping_points(self):
        toolchain()

        # Points occupy low memory, below the matrix at V2 and the stack.
        class PointMemory(dict):
            def __init__(self, image):
                super().__init__(image)
                self.reads = set()

            def get(self, address, default=None):
                if address < V2:
                    self.reads.add(address)
                return super().get(address, default)

        with tempfile.TemporaryDirectory() as scratch:
            root = Path(scratch)
            compile_console(root / 'console.so')
            console = Console(root / 'console.so')
            sources = ('sdk/ninja/njCalcPoints.c', 'engine/dc/xmtrx.c')
            for level, terminal in product(LEVELS, (False, True)):
                if terminal:
                    elf = root / 'points-lto.elf'
                    subprocess.run([SH_ELF / 'bin/sh-elf-gcc', *gcc_flags(SH_ELF), '-Isdk/include', '-I.',
                                    '-isystem', SH4ZAM / 'include', level, '-flto', '-nostdlib', '-Wl,--gc-sections',
                                    f'-Wl,-Ttext,{TEXT:#x}', '-Wl,-e,_njCalcPoints', '-o', elf, *sources],
                                   cwd=ROOT, check=True)
                    code, image, names = read_linked(elf, {})
                    # FV4 is the second vertex of the inlined batch. The scalar fallback uses FV8.
                    self.assertNotIn('zam_xmtrx_transform_points', names)
                    self.assertIn(('ftrv', ['xmtrx', 'fv4']), code.values())
                else:
                    # With separate objects, the batch's fifth argument is on the stack.
                    objects = []
                    for source in sources:
                        obj = root / (Path(source).stem + '.o')
                        self.assertIsNone(compile_sh4(obj, source, level))
                        objects.append(obj)
                    obj = root / 'linked.o'
                    subprocess.run([SH_ELF / 'bin/sh-elf-ld', '-EL', '-r', '-o', obj, *objects], check=True)
                    code, image, names = linked(obj, {})
                current = [bits(i + 0.25) for i in range(16)]
                matrix = list(reversed(current))
                for count, src, dst in ((-1, 0, 0), (0, 0, 0), (1, 0, 3), (4, 0, 12),
                                        (3, 0, 0), (4, 0, 3), (4, 3, 0)):
                    for m in (0, V2):
                        with self.subTest(level=level, terminal=terminal, count=count, src=src, dst=dst, matrix=m):
                            machine = Machine(console)
                            machine.code = code
                            machine.memory = PointMemory(image)
                            machine.r[15] = STACK
                            machine.banks[1][:] = current
                            for i, word in enumerate(matrix):
                                machine.store(V2 + 4 * i, word)
                            expected = [bits(i - 2.5) for i in range(max(src, dst) + 3 * max(count, 0))]
                            for i, word in enumerate(expected):
                                machine.store(V1 + 4 * i, word)
                            for i in range(max(count, 0)):
                                j, k = src + 3 * i, dst + 3 * i
                                expected[k:k + 3] = console.ftrv(matrix if m else current,
                                                                 expected[j:j + 3] + [ONE])[:3]
                            src_address = V1 + 4 * src
                            dst_address = V1 + 4 * dst
                            size = 12 * max(count, 0)
                            machine.written.clear()
                            machine.run(names['njCalcPoints'], (m, src_address, dst_address, count))
                            self.assertEqual(machine.memory.reads, set(range(src_address, src_address + size)))
                            writes = {address for address in machine.written if address < V2}
                            self.assertEqual(writes, set(range(dst_address, dst_address + size)))
                            self.assertEqual(machine.words(V1, len(expected)), expected)
                            self.assertEqual(machine.xmtrx, current)


class Fused(unittest.TestCase):
    def test_each_function_fuses_as_many_products_as_the_image_s_code(self):
        toolchain()
        with tempfile.TemporaryDirectory() as scratch:
            for source, fused in FUSED.items():
                with self.subTest(source=source):
                    obj = Path(scratch) / (Path(source).stem + '.o')
                    failed = compile_sh4(obj, source, '-Os')
                    self.assertIsNone(failed, failed)
                    counts = {name: sum(1 for _, mnemonic, _ in code if mnemonic == 'fmac')
                              for name, code in sh4.functions(SH_ELF / 'bin/sh-elf-objdump', obj).items()}
                    self.assertEqual(sorted(set(fused) - set(counts)), [])
                    self.assertEqual(counts, {name: fused.get(name, 0) - REPEATED.get(name, 0) for name in counts})


def fuse(log, a, b, c):
    """FMAC's a * b + c, rounded once, which log, a set, takes."""
    fused = sh4.fmac(a, b, c)
    log.add(('fmac', fused))
    return fused


def fmac_values(log):
    """The values log's FMACs made: an angle's blend truncates its sum, which can hide a sum's order, where the
    FMAC's own result shows it."""
    return {value for mnemonic, value in log if mnemonic == 'fmac'}


def divide(log, a, b):
    """FDIV's a / b, which log, a set, takes."""
    quotient = fdiv(a, b)
    log.add(('fdiv', quotient))
    return quotient


def float_unsigned(word):
    """A float of word, an unsigned 32-bit integer, rounded toward 0."""
    return sh4.toward_zero(Fraction(word), 0)


def root(n):
    """njSqrt's root of n: FSQRT of n's magnitude, negated for an n below 0."""
    return fsqrt(n & sh4.MAGNITUDE) ^ sh4.SIGN if fgreater(ZERO, n) else fsqrt(n)


def keyed_t(keys, index, nb, frame):
    """The linear and Overhauser steps' t: frame's place from key index to the next, over the keyframes' difference,
    or over 1 where the difference isn't above 0 as a signed value. keys holds (keyframe, words) pairs."""
    key, after = keys[index][0], keys[(index + 1) % nb][0]
    difference = (after - key) & 0xffffffff
    span = float_unsigned(difference) if signed(difference) > 0 else ONE
    return fdiv(fsub(frame, float_unsigned(key)), span)


def to_float(words, angles):
    """words as floats: as they are, or converted where they are angles."""
    return [ffloat(word) for word in words] if angles else list(words)


def from_float(words, angles):
    """words as a function stores them: as they are, or truncated to angles."""
    return [ftrc(word) for word in words] if angles else list(words)


def linear(keys, index, nb, frame, angles):
    """njCnkLinearMotionF's and A's out, and their links' and the camera motion's. The image rounds each product by
    t, and fuses the product by s into the sum."""
    log = set()
    t = keyed_t(keys, index, nb, frame)
    s = fsub(ONE, t)
    key, after = to_float(keys[index][1], angles), to_float(keys[(index + 1) % nb][1], angles)
    return from_float([fuse(log, s, k, sh4.fmul(n, t)) for k, n in zip(key, after)], angles), log


def linear_old(keys, index, nb, frame):
    """njCnkLinearMotionAOld's out, and its link's: the next key's angle comes within half a turn of key's first,
    by integer sums. The blend fuses as linear's does."""
    log = set()
    t = keyed_t(keys, index, nb, frame)
    s = fsub(ONE, t)
    out = []
    for key, after in zip(keys[index][1], keys[(index + 1) % nb][1]):
        turn = signed((after - key) & 0xffffffff)
        turn += -TURN if turn > HALF_TURN else TURN if turn < -HALF_TURN else 0
        near = (turn + key) & 0xffffffff
        out.append(ftrc(fuse(log, s, ffloat(key), sh4.fmul(ffloat(near), t))))
    return out, log


def squared_scale(log, out):
    """out scaled to length 1, as the quaternion blends scale it. The image rounds out[2]'s square, and fuses those
    of out[1], out[3] and out[0] into the sum, in that order."""
    length2 = fuse(log, out[0], out[0], fuse(log, out[3], out[3], fuse(log, out[1], out[1], sh4.fmul(out[2], out[2]))))
    scale = divide(log, ONE, fsqrt(length2))
    return [sh4.fmul(word, scale) for word in out]


def linear_q(keys, index, nb, frame):
    """njCnkLinearMotionQ's out, and its link's. The image's inner product rounds the second product, and fuses the
    first, third and fourth into the sum. Its blend fuses as linear's does."""
    log = set()
    t = keyed_t(keys, index, nb, frame)
    s = fsub(ONE, t)
    key, after = keys[index][1], keys[(index + 1) % nb][1]
    dot = fuse(log, key[3], after[3], fuse(log, key[2], after[2], fuse(log, key[0], after[0],
                                                                      sh4.fmul(key[1], after[1]))))
    if fgreater(ZERO, dot):
        t ^= sh4.SIGN
    return squared_scale(log, [fuse(log, s, k, sh4.fmul(n, t)) for k, n in zip(key, after)]), log


def overhauser_weights(t):
    """The Catmull-Rom weights of the keys before, at, after and two after t's span, as C groups them."""
    t2 = sh4.fmul(t, t)
    t3 = sh4.fmul(t2, t)
    return (sh4.fmul(fsub(fsub(sh4.fmul(TWO, t2), t), t3), HALF),
            sh4.fadd(fsub(sh4.fmul(ONE_AND_HALF, t3), sh4.fmul(TWO_AND_HALF, t2)), ONE),
            sh4.fadd(fsub(sh4.fmul(HALF, t), sh4.fmul(ONE_AND_HALF, t3)), sh4.fmul(TWO, t2)),
            sh4.fmul(fsub(t3, t2), HALF))


def overhauser(keys, index, nb, frame, angles, tangents=False):
    """njCnkOverhauserMotionF's and A's out, the camera motion's, and their links' where tangents is true. The image
    rounds the product by the second weight, and fuses those by the first, third and fourth into the sum. A link's
    tangents, out[6] to [8], round the product by t / 2, and fuse the product by 1/2 - t / 2."""
    log = set()
    t = keyed_t(keys, index, nb, frame)
    wa, wb, wc, wd = overhauser_weights(t)
    four = [keys[(index + offset) % nb][1] for offset in (nb - 1, 0, 1, 2)]
    before, key, after, past = (to_float(words, angles) for words in four)
    out = from_float([fuse(log, wd, p, fuse(log, wc, n, fuse(log, wa, b, sh4.fmul(wb, k))))
                      for b, k, n, p in zip(before, key, after, past)], angles)
    if not tangents:
        return out, log
    t = sh4.fmul(t, HALF)
    s = fsub(HALF, t)
    if angles:
        slopes = [ftrc(fuse(log, ffloat((n - b) & 0xffffffff), s, sh4.fmul(ffloat((p - k) & 0xffffffff), t)))
                  for b, k, n, p in zip(*four)]
    else:
        slopes = [fuse(log, fsub(n, b), s, sh4.fmul(fsub(p, k), t)) for b, k, n, p in zip(*four)]
    return out + [UNSET] * 3 + slopes, log


def wrapped(turn):
    """A float difference of angles within half a turn, as the Old Bezier steps bring one."""
    if fgreater(turn, HALF_TURN_F):
        return fsub(turn, TURN_F)
    if not fgreater(turn, HALF_TURN_F ^ sh4.SIGN):
        return sh4.fadd(turn, TURN_F)
    return turn


def bezier(keys, index, nb, frame, kind):
    """njCnkBezierMotionF's, A's and AOld's out, by kind 'F', 'A' or 'AOld', and their links'. The image fuses
    third's product by the first tangent into the first control point. Each point rounds the product by the second
    weight, and fuses those by the first, third and fourth into the sum."""
    log = set()
    before, key, after, past = (keys[(index + offset) % nb] for offset in (nb - 1, 0, 1, 2))

    def span(later, earlier):
        gap = fsub(float_unsigned(later[0]), float_unsigned(earlier[0]))
        return gap if fgreater(gap, ZERO) else ONE

    span1, span2, span0 = span(after, key), span(past, after), span(key, before)
    t = fdiv(fsub(frame, float_unsigned(key[0])), span1)
    third = sh4.fmul(span1, THIRD)
    bias2 = fdiv(span1, sh4.fadd(span2, span1))
    bias0 = fdiv(span1, sh4.fadd(span0, span1))
    s = fsub(ONE, t)
    t2, s2 = sh4.fmul(t, t), sh4.fmul(s, s)
    b3, b0 = sh4.fmul(t, t2), sh4.fmul(s, s2)
    b1, b2 = sh4.fmul(sh4.fmul(t, s2), THREE), sh4.fmul(sh4.fmul(t2, s), THREE)
    out = []
    for b, k, n, p in zip(before[1], key[1], after[1], past[1]):
        if kind == 'F':
            d1, d0, d2 = fsub(n, k), fsub(k, b), fsub(p, n)
        else:
            d0, d2 = ffloat((k - b) & 0xffffffff), ffloat((p - n) & 0xffffffff)
            d1 = ffloat((n - k) & 0xffffffff) if kind == 'A' else fsub(ffloat(n), ffloat(k))
            k, n = ffloat(k), ffloat(n)
            if kind == 'AOld':
                d1 = wrapped(d1)
                n = sh4.fadd(d1, k)
                d0, d2 = wrapped(d0), wrapped(d2)
        if index == nb - 2:
            tangent0 = sh4.fadd(fdiv(sh4.fmul(fsub(ONE, bias0), d1), span1), fdiv(sh4.fmul(bias0, d0), span0))
            tangent1 = fsub(fdiv(sh4.fmul(sh4.fadd(bias0, ONE), d1), span1), fdiv(sh4.fmul(bias0, d0), span0))
        elif index == 0:
            tangent0 = fsub(fdiv(sh4.fmul(sh4.fadd(bias2, ONE), d1), span1), fdiv(sh4.fmul(bias2, d2), span2))
            tangent1 = sh4.fadd(fdiv(sh4.fmul(fsub(ONE, bias2), d1), span1), fdiv(sh4.fmul(bias2, d2), span2))
        else:
            tangent0 = sh4.fadd(fdiv(sh4.fmul(fsub(ONE, bias0), d1), span1), fdiv(sh4.fmul(bias0, d0), span0))
            tangent1 = sh4.fadd(fdiv(sh4.fmul(fsub(ONE, bias2), d1), span1), fdiv(sh4.fmul(bias2, d2), span2))
        c1 = fuse(log, tangent0, third, k)
        c2 = fsub(n, sh4.fmul(third, tangent1))
        point = fuse(log, b3, n, fuse(log, b2, c2, fuse(log, b0, k, sh4.fmul(b1, c1))))
        out.append(point if kind == 'F' else ftrc(point))
    return out, log


def blend(name, values, rate):
    """The out of njCnkLinearIntF, Q, A or AOld, or njCnkHermiteIntF or A, at rate between the two slots' values.
    The image's linear blends round one product of each sum, and fuse the other: out[0]'s by t, the others' by s.
    The quaternion's inner product rounds the third product, and fuses the second, fourth and first. Hermite's
    out[0] rounds the product by h2, and fuses those by h1, h3 and h0. Its out[1] and out[2] round the product by
    h1, and fuse those by h0, h2 and h3."""
    log = set()
    t, s = rate, fsub(ONE, rate)
    angles = name.endswith(('A', 'AOld'))
    v = to_float(values, angles)
    if name == 'njCnkLinearIntQ':
        dot = fuse(log, v[0], v[4], fuse(log, v[3], v[7], fuse(log, v[1], v[5], sh4.fmul(v[2], v[6]))))
        if fgreater(ZERO, dot):
            t ^= sh4.SIGN
        return squared_scale(log, [fuse(log, t, v[4], sh4.fmul(v[0], s))] +
                             [fuse(log, s, v[i], sh4.fmul(v[i + 4], t)) for i in (1, 2, 3)]), log
    if name == 'njCnkLinearIntAOld':
        near = []
        for i in range(3):
            turn = signed((values[i + 3] - values[i]) & 0xffffffff)
            turn += -TURN if turn > HALF_TURN else TURN if turn < -HALF_TURN else 0
            near.append(ffloat((turn + values[i]) & 0xffffffff))
        return [ftrc(fuse(log, s, v[i], sh4.fmul(near[i], t))) for i in range(3)], log
    if name.startswith('njCnkLinearInt'):
        return from_float([fuse(log, t, v[3], sh4.fmul(v[0], s))] +
                          [fuse(log, s, v[i], sh4.fmul(v[i + 3], t)) for i in (1, 2)], angles), log
    t2 = sh4.fmul(rate, rate)
    t3 = sh4.fmul(t2, rate)
    h0 = sh4.fadd(fsub(sh4.fmul(TWO, t3), sh4.fmul(THREE, t2)), ONE)
    h1 = fsub(sh4.fmul(THREE, t2), sh4.fmul(TWO, t3))
    h2 = sh4.fadd(fsub(rate, sh4.fmul(TWO, t2)), t3)
    h3 = fsub(t3, t2)
    return from_float([fuse(log, h0, v[0], fuse(log, h3, v[9], fuse(log, h1, v[3], sh4.fmul(h2, v[6]))))] +
                      [fuse(log, h3, v[i + 9], fuse(log, h2, v[i + 6], fuse(log, h0, v[i], sh4.fmul(h1, v[i + 3]))))
                       for i in (1, 2)], angles), log


def ang2quat(console, ang, zxy):
    """njAng2Quat's quaternion of ang, by njSin's and njCos's FSCA. The image takes the cosines' products by c[2]
    into the first sum by FMACs, and fuses the product by s[2] into q[1]'s sum, and that by s[0] into q[3]'s."""
    log = set()
    about_x = ang[0]
    about_y, about_z = (ang[2], ang[1]) if zxy else (ang[1], ang[2])
    (s0, c0), (s1, c1), (s2, c2) = (console.fsca(angle & 0xffff) for angle in (about_z, about_y, about_x))
    total = sh4.fadd(fuse(log, c2, c0, fuse(log, c2, c1, fsub(sh4.fmul(c0, c1), sh4.fmul(sh4.fmul(s0, s1), s2)))),
                     ONE)
    re = sh4.fmul(fsqrt(total), HALF)
    scale = fdiv(QUARTER, re)
    return [re,
            sh4.fmul(scale, fuse(log, s2, sh4.fadd(c0, c1), sh4.fmul(sh4.fmul(s0, s1), c2))),
            sh4.fmul(scale, sh4.fadd(fsub(sh4.fmul(sh4.fmul(c0, s1), c2), sh4.fmul(s0, s2)), s1)),
            sh4.fmul(scale, fuse(log, s0, sh4.fadd(c1, c2), sh4.fmul(sh4.fmul(c0, s1), s2)))], log


def distance_p2p(p1, p2):
    """The shipped njDistanceP2P's distance, which njCollisionCheckBC calls: 0 for a squared distance under 0.025F,
    else its root. It rounds the square of y's difference, and fuses x's and then z's."""
    dx, dy, dz = (fsub(a, b) for a, b in zip(p1, p2))
    squared = sh4.fmac(dz, dz, sh4.fmac(dx, dx, sh4.fmul(dy, dy)))
    return ZERO if fgreater(TOLERANCE, squared) else root(squared)


def collision(box, c1, c2, r, log):
    """njCollisionCheckBC's result for box's eight corners and the capsule from c1 to c2 of radius r, as the shipped
    code works it out where njCheckBBox passes. log, a set, takes each FMAC's and FDIV's result. Each sum of three
    products rounds the second product, and fuses the first and then the third into it. A face's normal fuses its
    second product into its first, negated. Each point a direction times t places fuses the product into the sum."""
    def sum3(a, b, c, d, e, f):
        return fuse(log, e, f, fuse(log, a, b, sh4.fmul(c, d)))

    def minus(a, b):
        return [fsub(x, y) for x, y in zip(a, b)]

    def unit(v):
        scale = divide(log, ONE, root(sum3(v[0], v[0], v[1], v[1], v[2], v[2])))
        return [sh4.fmul(x, scale) for x in v]

    def distance(a, b):
        d = minus(a, b)
        return root(sum3(d[0], d[0], d[1], d[1], d[2], d[2]))

    def within(a, b):
        return not fgreater(a, b)

    length = distance_p2p(c1, c2)
    axis = unit(minus(c2, c1))
    normals = []
    for corner in CORNERS:
        e1, e2 = minus(box[corner[1]], box[corner[0]]), minus(box[corner[2]], box[corner[0]])
        normals.append(unit([fuse(log, e1[2], e2[1], sh4.fmul(e1[1], e2[2]) ^ sh4.SIGN),
                             fuse(log, e1[0], e2[2], sh4.fmul(e1[2], e2[0]) ^ sh4.SIGN),
                             fuse(log, e1[1], e2[0], sh4.fmul(e1[0], e2[1]) ^ sh4.SIGN)]))
    inside, near, behind, total = [True, True], [0, 0], [0, 0], [ZERO, ZERO]
    for face, corner in enumerate(CORNERS):
        n = normals[face]
        v = [box[k] for k in corner]
        offset = fsub(fsub(sh4.fmul(n[0] ^ sh4.SIGN, v[0][0]), sh4.fmul(n[1], v[0][1])), sh4.fmul(n[2], v[0][2]))
        if fgreater(sum3(axis[0], n[0], axis[1], n[1], axis[2], n[2]) & sh4.MAGNITUDE, TOLERANCE):
            t = sh4.fadd(sum3(n[0], c1[0], n[1], c1[1], n[2], c1[2]), offset)
            t = divide(log, t ^ sh4.SIGN, sum3(n[0], axis[0], n[1], axis[1], n[2], axis[2]))
            p = [fuse(log, axis[k], t, c1[k]) for k in range(3)]
            to_c1, to_c2 = distance(p, c1), distance(p, c2)
            whole, edges, corners, inner = True, 0, 0, 0
            for k in range(4):
                a, b = v[k], v[(k + 1) % 4]
                edge, along = minus(a, b), minus(p, a)
                t = divide(log, sum3(edge[0], along[0], edge[1], along[1], edge[2], along[2]),
                           sum3(edge[0], edge[0], edge[1], edge[1], edge[2], edge[2]))
                q = [fuse(log, edge[j], t, a[j]) for j in range(3)]
                d = unit(minus(q, p))
                across = normals[ACROSS[face][k]]
                off = [fsub(across[j], d[j]) & sh4.MAGNITUDE for j in range(3)]
                segment = sh4.fadd(fsub(to_c1, length), to_c2) & sh4.MAGNITUDE
                if all(within(x, TOLERANCE) for x in off) and (
                        within(to_c1, r) or within(to_c2, r) or within(segment, TOLERANCE)):
                    inner += 1
                    continue
                whole = False
                to_a, to_b, edge_length = distance(a, q), distance(b, q), distance(a, b)
                nearer = to_c1 if fgreater(to_c2, to_c1) else to_c2
                reach = root(fsub(sh4.fmul(r, r), sh4.fmul(nearer, nearer)))
                if within(fsub(sh4.fadd(to_a, to_b), TOLERANCE), sh4.fadd(edge_length, TOLERANCE)):
                    edges += fgreater(reach, distance(p, q))
                else:
                    to_a, to_b = distance(a, p), distance(b, p)
                    corners += within(to_a, reach) or within(to_b, reach)
            if whole or edges + inner == 4 or corners + inner == 4:
                return 1
        nn = sum3(n[0], n[0], n[1], n[1], n[2], n[2])
        for end, c in enumerate((c1, c2)):
            t = divide(log, sh4.fadd(sum3(n[0], c[0], n[1], c[1], n[2], c[2]), offset) ^ sh4.SIGN, nn)
            foot = [fuse(log, n[j], t, c[j]) for j in range(3)]
            down = unit(minus(foot, c))
            if all(within(fsub(n[j], down[j]) & sh4.MAGNITUDE, TOLERANCE) for j in range(3)):
                behind[end] += 1
            else:
                inside[end] = False
                to_foot = distance(c, foot)
                near[end] += within(to_foot, r)
                total[end] = sh4.fadd(total[end], to_foot)
    if any(inside) or behind[0] + near[0] == 6 or behind[1] + near[1] == 6:
        reach2 = sh4.fmul(sh4.fmul(r, r), TWO)
        if any(near[end] == 2 and fgreater(reach2, sh4.fmul(total[end], total[end])) for end in (0, 1)):
            return 1
        if near[0] < 2 or near[1] < 2:
            return 1
    return 0


def linked(obj, stubs):
    """obj linked at TEXT, as Order's machine runs it: each name stubs gives at a stub's address, and each other name
    the object takes at its own in DATA. Returns the code by address, the bytes of the code and data by address, and
    the address of each name the link holds."""
    tools = SH_ELF / 'bin'
    taken = subprocess.run([str(tools / 'sh-elf-nm'), '-u', str(obj)], stdout=subprocess.PIPE, text=True,
                           check=True).stdout.split()
    names = {name: STUBS + 16 * sorted(stubs).index(name) if name in stubs else DATA + 256 * i
             for i, name in enumerate(name for name in taken if name != 'U')}
    elf = obj.with_suffix('.elf')
    subprocess.run([str(tools / 'sh-elf-ld'), '-EL', '-N', '-e', '0', f'-Ttext={TEXT:#x}', '-o', str(elf), str(obj),
                    *(f'--defsym={name}={address:#x}' for name, address in names.items())],
                   stdout=subprocess.PIPE, stderr=subprocess.STDOUT, check=True)
    return read_linked(elf, names)


def read_linked(elf, names):
    """Read the SH-4 instructions and memory actually placed in the final ELF."""
    tools = SH_ELF / 'bin'
    image = elf.with_suffix('.bin')
    subprocess.run([str(tools / 'sh-elf-objcopy'), '-O', 'binary', '--only-section=.text', '--only-section=.rodata',
                    '--only-section=.data', str(elf), str(image)], check=True)
    listing = subprocess.run([str(tools / 'sh-elf-objdump'), '-d', str(elf)], stdout=subprocess.PIPE, text=True,
                             check=True).stdout
    code = {}
    for line in listing.splitlines():
        found = re.match(r'([0-9a-f]+) <_(\w+)>:', line)
        if found:
            names[found[2]] = int(found[1], 16)
            continue
        row = sh4.LINE.match(line)
        if row:
            operands = re.sub(r'\s*<[^>]*>', '', row[5].partition('!')[0]).strip()
            code[int(row[1], 16)] = (row[4], [part.strip() for part in sh4.OPERANDS.split(operands)]
                                     if operands else [])
    return code, dict(enumerate(image.read_bytes(), TEXT)), names


def udivide(machine):
    """libgcc's ___udivsi3_i4i: R0 gets R4 over R5, unsigned."""
    machine.r[0] = machine.r[4] // machine.r[5]


def copy_bytes(machine):
    """memcpy: R6 bytes from R5 to R4, which R0 returns."""
    for i in range(machine.r[6]):
        machine.memory[machine.r[4] + i] = machine.memory.get(machine.r[5] + i, 0)
    machine.r[0] = machine.r[4]


def passes(machine):
    """njCheckBBox, which the test lets pass."""
    machine.r[0] = 1


def distance_between(machine):
    """njDistanceP2P, as distance_p2p works it out."""
    machine.front[0] = distance_p2p(machine.words(machine.r[4], 3), machine.words(machine.r[5], 3))


# what stands in for each function the fused functions call: njCollisionCheckBC's box and capsule bounds, which
# njCheckBBox would test, it leaves alone
CALLED = {'___udivsi3_i4i': udivide, '_memcpy': copy_bytes, '_njCheckBBox': passes,
          '_njGetBoxMinMax': lambda machine: None, '_njGetCapsuleMinMax': lambda machine: None,
          '_njDistanceP2P': distance_between}
# each keyed function: the bytes its key takes, the words it reads of each, what they are, and its reference as
# (keys, index, nb, frame) -> out
KEYED = {
    'njCnkLinearMotionF': (16, 3, 'float', lambda *key: linear(*key, False)),
    'njCnkLinearMotionA': (16, 3, 'angle', lambda *key: linear(*key, True)),
    'njCnkLinearMotionQ': (20, 4, 'unit', linear_q),
    'njCnkLinearMotionAOld': (16, 3, 'angle', linear_old),
    'njCnkOverhauserMotionF': (16, 3, 'float', lambda *key: overhauser(*key, False)),
    'njCnkOverhauserMotionA': (16, 3, 'angle', lambda *key: overhauser(*key, True)),
    'njCnkBezierMotionF': (16, 3, 'float', lambda *key: bezier(*key, 'F')),
    'njCnkBezierMotionA': (16, 3, 'angle', lambda *key: bezier(*key, 'A')),
    'njCnkBezierMotionAOld': (16, 3, 'angle', lambda *key: bezier(*key, 'AOld')),
    'njCnkLinearMotLinkF': (16, 3, 'float', lambda *key: linear(*key, False)),
    'njCnkLinearMotLinkA': (16, 3, 'angle', lambda *key: linear(*key, True)),
    'njCnkLinearMotLinkQ': (20, 4, 'unit', linear_q),
    'njCnkLinearMotLinkAOld': (16, 3, 'angle', linear_old),
    'njCnkOverhauserMotLinkF': (16, 3, 'float', lambda *key: overhauser(*key, False, True)),
    'njCnkOverhauserMotLinkA': (16, 3, 'angle', lambda *key: overhauser(*key, True, True)),
    'njCnkBezierMotLinkF': (16, 3, 'float', lambda *key: bezier(*key, 'F')),
    'njCnkBezierMotLinkA': (16, 3, 'angle', lambda *key: bezier(*key, 'A')),
    'njCnkBezierMotLinkAOld': (16, 3, 'angle', lambda *key: bezier(*key, 'AOld')),
    'linear_angle': (8, 1, 'angle', lambda *key: linear(*key, True)),
    'linear_vector': (16, 3, 'float', lambda *key: linear(*key, False)),
    'overhauser_vector': (16, 3, 'float', lambda *key: overhauser(*key, False)),
}
# the value blends: how many words of values each reads, and what they are
BLENDS = {'njCnkLinearIntF': (6, 'float'), 'njCnkLinearIntQ': (8, 'unit'), 'njCnkLinearIntA': (6, 'angle'),
          'njCnkLinearIntAOld': (6, 'angle'), 'njCnkHermiteIntF': (12, 'float'), 'njCnkHermiteIntA': (12, 'angle')}
# where each source's keyed functions read the frame: the info and the slot, which _nj_mot_current_data_ names for a
# link's
FRAMES = {'sdk/ninja/njCnkMotSrc.c': ('__nj_motion_data_info_', 0),
          'sdk/ninja/njCnkMotLinkSrc.c': ('__nj_motlink_data_info_', 1),
          'sdk/ninja/njCameraMotion.c': ('__nj_cmotion_data_info_', 0)}


class Order(unittest.TestCase):
    """GCC's code of each fused function, as make dc compiles it, against a reference written from the order of the
    image's sums using sh4.py's FMUL and FMAC in round-toward-zero mode. Each runs in tests/sh4.py at each of
    LEVELS, but for njCollisionCheckBC, at -Os alone, where -O2's copy of its tables calls into libgcc."""

    @classmethod
    def setUpClass(cls):
        toolchain()
        scratch = tempfile.TemporaryDirectory()
        cls.addClassCleanup(scratch.cleanup)
        library = Path(scratch.name) / 'console.so'
        compile_console(library)
        cls.console = Console(library)
        cls.linked = {}
        for level in LEVELS:
            for source in (*FUSED, 'sdk/ninja/njCnkShapeSrc.c'):
                obj = Path(scratch.name) / f'{Path(source).stem}{level}.o'
                failed = compile_sh4(obj, source, level)
                if failed:
                    raise AssertionError(f'{source} at {level}:\n{failed}')
                cls.linked[level, source] = linked(obj, CALLED)

    def setUp(self):
        self.random = random.Random(0x5ca1e)

    def run_fused(self, level, source, name, arguments, floats=(), memory=()):
        """Run name's code in source's object at level, as Helpers.call runs a function, with CALLED's stubs at
        the names they stand for. memory gives (address, words) to put first. Returns the machine."""
        code, image, names = self.linked[level, source]
        machine = Machine(self.console)
        machine.code = code
        machine.memory = dict(image)
        machine.stubs = {names[name]: stub for name, stub in CALLED.items() if name in names}
        machine.r = [0xdead0000 + n for n in range(15)] + [STACK]
        machine.banks = [[0x7fc10000 + 16 * bank + n for n in range(16)] for bank in range(2)]
        for address, words in memory:
            for i, word in enumerate(words):
                machine.store(address + 4 * i, word)
        kept = (machine.r[8:], machine.banks[0][12:])
        machine.run(names[name], arguments, floats)
        self.assertEqual((machine.r[8:], machine.front[12:], machine.sz, machine.fr), (*kept, 0, 0))
        return machine

    def test_shape_vertices_round_the_second_key_before_adding_the_first(self):
        # Cannon's Core's pipe at frame 2.5, between its first two shape keys.
        first = [bits(-25.0), 0, 0]
        second = [bits(-25.0), 0, 0xc21f26ca]
        rate = fdiv(bits(2.5), bits(19.0))
        weight = fsub(ONE, rate)
        for axis in range(3):
            expected = [sh4.fmac(a, weight, sh4.fmul(b, rate)) for a, b in zip(first, second)]
            for level in LEVELS:
                with self.subTest(level=level, axis=axis):
                    machine = self.run_fused(level, 'sdk/ninja/njCnkShapeSrc.c', 'njCnkInterpolatesF3',
                                             [V1, V2, OUT], [rate], [(V1, first), (V2, second), (OUT, [UNSET] * 4)])
                    self.assertEqual(machine.words(OUT, 4), [*expected, UNSET])
            first = first[1:] + first[:1]
            second = second[1:] + second[:1]

    def word(self, kind):
        """A random word of kind: a float from -300 to 300, one of a unit quaternion's from -1 to 1, or an angle of
        up to one and a half turns either way."""
        if kind == 'float':
            return bits(self.random.uniform(-300.0, 300.0))
        if kind == 'unit':
            return bits(self.random.uniform(-1.0, 1.0))
        return self.random.randrange(-3 * HALF_TURN, 3 * HALF_TURN) & 0xffffffff

    def test_value_blends_sum_in_the_shipped_order(self):
        for level in LEVELS:
            for name, (count, kind) in BLENDS.items():
                for case in range(SUMS):
                    values = [self.word(kind) for _ in range(count)]
                    rate = bits(self.random.uniform(0.0, 1.0))
                    with self.subTest(level=level, name=name, case=case):
                        machine = self.run_fused(level, 'sdk/ninja/njCnkMotLinkSrc.c', name, [KEYS, OUT], [rate],
                                                 [(KEYS, values), (OUT, [UNSET] * 12)])
                        out, log = blend(name, values, rate)
                        self.assertEqual((machine.words(OUT, 12), fmac_values(machine.fused)),
                                         (out + [UNSET] * (12 - len(out)), fmac_values(log)))

    def test_ang2quat_sums_in_the_shipped_order(self):
        for level in LEVELS:
            for case in range(SUMS):
                ang = [self.random.getrandbits(32) for _ in range(3)]
                zxy = case % 2
                try:
                    out, log = ang2quat(self.console, ang, zxy)
                except sh4.Unmodeled:
                    continue
                with self.subTest(level=level, case=case, ang=ang, zxy=zxy):
                    machine = self.run_fused(level, 'sdk/ninja/njCnkMotLinkSrc.c', 'njAng2Quat', [KEYS, OUT, zxy],
                                             memory=[(KEYS, ang), (OUT, [UNSET] * 4)])
                    self.assertEqual((machine.words(OUT, 4), fmac_values(machine.fused)), (out, fmac_values(log)))

    def test_keyed_interpolators_sum_in_the_shipped_order(self):
        for level in LEVELS:
            for source, fused in FUSED.items():
                if source not in FRAMES:
                    continue
                info, slot = FRAMES[source]
                for name in sorted(set(fused) & set(KEYED)):
                    size, count, kind, reference = KEYED[name]
                    for case in range(SUMS):
                        nb = self.random.randrange(3, 7)
                        frames = sorted(self.random.sample(range(400), nb))
                        if case % 5 == 0:
                            frames[1] = frames[0]
                        keys = [(frame, [self.word(kind) for _ in range(count)]) for frame in frames]
                        index = case % nb
                        start = frames[index]
                        frame = bits(self.random.uniform(start, max(frames[(index + 1) % nb], start + 1)))
                        names = self.linked[level, source][2]
                        memory = [(KEYS + size * i, [key, *words]) for i, (key, words) in enumerate(keys)]
                        memory += [(OUT, [UNSET] * 12), (names[info] + MOTION_INFO * slot + FRAME, [frame])]
                        if slot:
                            memory.append((names['__nj_mot_current_data_'], [slot]))
                        out, log = reference(keys, index, nb, frame)
                        with self.subTest(level=level, name=name, case=case, index=index, nb=nb):
                            machine = self.run_fused(level, source, name, [KEYS, index, nb, OUT], memory=memory)
                            self.assertEqual((machine.words(OUT, 12), fmac_values(machine.fused)),
                                             (out + [UNSET] * (12 - len(out)), fmac_values(log)))

    def test_collision_check_sums_in_the_shipped_order(self):
        # each run's result, and the set of values its FMACs and FDIVs make. GCC works out each of three repeated
        # sums once, as REPEATED says, so a set and not a count holds them
        source = 'sdk/ninja/njCollisionCheckBC.c'
        for case in range(COLLISIONS):
            size = [self.random.uniform(1.0, 40.0) for _ in range(3)]
            center = [self.random.uniform(-50.0, 50.0) for _ in range(3)]
            turn = [self.random.uniform(-math.pi, math.pi) for _ in range(2)]
            box = []
            for x, y, z in ((-1, -1, -1), (1, -1, -1), (1, 1, -1), (-1, 1, -1),
                            (-1, -1, 1), (1, -1, 1), (1, 1, 1), (-1, 1, 1)):
                x, y, z = x * size[0], y * size[1], z * size[2]
                x, y = x * math.cos(turn[0]) - y * math.sin(turn[0]), x * math.sin(turn[0]) + y * math.cos(turn[0])
                y, z = y * math.cos(turn[1]) - z * math.sin(turn[1]), y * math.sin(turn[1]) + z * math.cos(turn[1])
                box.append([bits(center[0] + x), bits(center[1] + y), bits(center[2] + z)])
            reach = max(size)
            c1 = [bits(center[k] + self.random.uniform(-1.3, 1.3) * reach) for k in range(3)]
            c2 = [bits(sh4.exact(c1[k]) + self.random.uniform(-1.0, 1.0) * reach) for k in range(3)]
            r = bits(self.random.uniform(0.05, 0.6) * reach)
            log = set()
            result = collision(box, c1, c2, r, log)
            with self.subTest(case=case):
                machine = self.run_fused('-Os', source, 'njCollisionCheckBC', [BOX, CAPSULE],
                                         memory=[(BOX, [word for corner in box for word in corner]),
                                                 (CAPSULE, c1 + c2 + [r])])
                self.assertEqual((machine.r[0], set(machine.fused)), (result, log))
