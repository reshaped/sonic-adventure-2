"""The C runtime's float kernels in round-toward-zero mode and the float constants of the tree's assembly.

Each kernel is checked against a reference written in the order of its shipped instructions. The reference uses
tests/sh4.py's exact values with FPSCR.RM = 1 and DN = 1.
FMUL, FADD, FSUB and FDIV each round once, FMAC rounds its sum of the exact product once, and FLOAT and FTRC convert.
The host's compiler builds the members' C with NONMATCHING defined, and runs it rounding toward zero, on inputs whose
shipped arithmetic meets no denormal, which the host would keep. The plain side, which multiplies and then adds where
SHC's code fuses, must give other bits for some of those inputs, so the check of the fused side can fail. Where SH_ELF
names a toolchain and SH4ZAM its headers, GCC's code for each member, as make dc compiles it, holds an FMAC wherever
the shipped code holds one, and calls no fmaf.

Every .FDATA.S operand of the tree's assembly lies an eighth of a step or more from a tie between two floats, and
GNU as reads each as the float nearest it. A constant on a tie has two nearest floats, and GNU as takes the one above
it: 16777217.0 reads as H'4B800001. The eighth is a margin for a reader that falls short of a decimal's value."""

import ctypes
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

# the members whose kernels run here, and the shipped code's FMACs in each of their functions
MEMBERS = ('_a_sins', '_a_coss', '_a_atans', '_in_logs', '_in_exps', '_i_atans', '_in_coss', '_i_fmds', '_i_ldes')
SHIPPED_FMACS = {'_app_sins': 1, '_app_coss': 1, '_app_atans': 1, '_in_logs': 1, '_in_exps': 2, '_in_atans': 1,
                 '_in_coss': 0, '_in_fmods': 0, '_in_ldexps': 0}
# the kernels whose shipped code fuses a product into a sum, itself or in a kernel it calls
FUSED = ('_app_sins', '_app_coss', '_app_atans', '_in_logs', '_in_exps', '_in_atans', '_in_coss')
# the host's flags for the members: C as GCC reads it, each operation rounded as the run sets the host's FPU, with
# no product fused and no operation folded. The plain side of _in_atans leaves its result unset for an eighth past 8,
# which no input gives, and float_bits.h's helpers go unused in some members
HOST_FLAGS = ('-std=gnu17', '-O1', '-ffp-contract=off', '-frounding-math', '-fno-strict-aliasing', '-Wall', '-Wextra',
              '-Werror', '-Wno-unknown-pragmas', '-Wno-unused-function', '-Wno-uninitialized')
# the cases each kernel runs, on random inputs
CASES = 1500
SIGN, MAGNITUDE = sh4.SIGN, sh4.MAGNITUDE
EXPONENT = 0x7f800000
# the least normal float, below which the console's result is a 0 and the host's a denormal
LEAST_NORMAL = Fraction(2) ** -126
# a .FDATA.S line's operands and an F' constant's digits and exponent, as asmsh reads them
FDATA = re.compile(r'^[ \t]*(?:[A-Za-z_?$.][\w?$.]*:?)?[ \t]+\.FDATA\.S[ \t]+([^;\n]*)', re.I | re.M)
CONSTANT = re.compile(r"F'([+-]?)([0-9]*)\.?([0-9]*)(?:[SE]([+-]?[0-9]+))?", re.I)
# how near a tie between two floats a constant may lie, in steps
MARGIN = Fraction(1, 8)

# the shipped constants, by their bits in the image: the runtime's shared constants and each member's tables
ONES, MINUS_ONES, HALFS, MINUS_HALFS, MINUS_QUOTS = 0x3f800000, 0xbf800000, 0x3f000000, 0xbf000000, 0xbe800000
QUARTER_PI, MINUS_QUARTER_PI, LOG_2 = 0x3f490fdb, 0xbf490fdb, 0x3f317218
SINES = (0x2f309046, 0xb2d73229, 0x3638ef1e, 0xb9500d02, 0x3c088889, 0xbe2aaaac, 0x3f800000)
COSINES = (0x310f74c3, 0xb493f27c, 0x37d00d01, 0xbab60b62, 0x3d2aaaac, 0xbf000000, 0x3f800000)
ARC_TANGENTS = (0xbdba2de9, 0x3de38e39, 0xbe124925, 0x3e4ccccd, 0xbeaaaaab, 0x3f800000)
LOGARITHMS = (0x3e078d63, 0x3e1d920a, 0x3e3a2e64, 0x3e638e39, 0x3e924925, 0x3ecccccd, 0x3f2aaaab, 0x40000000)
ROOT_TWO, MINUS_ROOT_TWO = 0x3fb504f4, 0xbfb504f4
NUMERATOR = (0x35a9c374, 0x385ee4ec, 0x3a8b5575, 0x3c5104f8, 0x3dc3f6bf, 0x3ed79043, 0x3f579042)
DENOMINATOR = (0x35a9c374, 0xb85ee4ec, 0x3a8b5575, 0xbc5104f8, 0x3dc3f6bf, 0xbed79043, 0x3f579042)
ONE_OVER_LOG_2 = 0x3fb8aa3c
ATAN_QUARTER, ATAN_HALF, ATAN_THREE_QUARTERS = 0x3e7adbb1, 0x3eed6338, 0x3f24bc7d
THREE_QUARTERS, MINUS_THREE_QUARTERS, AN_EIGHTH = 0x3f400000, 0xbf400000, 0x3e000000
TWO_TO_31, MINUS_TWO_TO_32 = 0x4f000000, 0xcf800000
WHOLE_EXPONENT = 0x4b000000


class Denormal(Exception):
    """A shipped operation whose exact result lies below the least normal float, which the console flushes to 0
    and the host keeps."""


def rounded(value, zero):
    """Round value toward zero to binary32, keeping the supplied sign for an exact zero."""
    if value and abs(value) < LEAST_NORMAL:
        raise Denormal
    return sh4.toward_zero(value, zero)


def fmul(n, m):
    return rounded(sh4.exact(n) * sh4.exact(m), (n ^ m) & SIGN)


def fadd(n, m):
    return rounded(sh4.exact(n) + sh4.exact(m), n & m & SIGN)


def fsub(n, m):
    """FRn - FRm."""
    return fadd(n, m ^ SIGN)


def fdiv(n, m):
    """FRn / FRm."""
    return rounded(sh4.exact(n) / sh4.exact(m), (n ^ m) & SIGN)


def fmac(f0, m, n):
    """FR0 * FRm + FRn."""
    return rounded(sh4.exact(f0) * sh4.exact(m) + sh4.exact(n), (f0 ^ m) & n & SIGN)


def fcmp_gt(m, n):
    """FCMP/GT FRm,FRn's T: whether FRn exceeds FRm."""
    return sh4.exact(n) > sh4.exact(m)


def ftrc(n):
    """FTRC's integer, toward zero. The kernels below meet none past an int."""
    whole = int(sh4.exact(n))
    assert -2 ** 31 <= whole < 2 ** 31
    return whole


def float_(whole):
    return rounded(Fraction(whole), 0)


def series(t, coefficients):
    """The shipped loop: the first coefficient, then for each next one an FMAC of FR0 t, FRm the sum and FRn the
    coefficient."""
    total = coefficients[0]
    for coefficient in coefficients[1:]:
        total = fmac(t, total, coefficient)
    return total


def app_sins(x):
    return fmul(series(fmul(x, x), SINES), x)


def app_coss(x):
    return series(fmul(x, x), COSINES)


def app_atans(x):
    return fmul(series(fmul(x, x), ARC_TANGENTS), x)


def in_logs(x):
    """For a positive normal x other than 1."""
    fraction = x & 0x807fffff | ONES
    power = fmul(fadd(float_(((x & EXPONENT) >> 23) - 127), HALFS), LOG_2)
    reduced = fdiv(fadd(MINUS_ROOT_TWO, fraction), fadd(ROOT_TWO, fraction))
    return fadd(power, fmul(reduced, series(fmul(reduced, reduced), LOGARITHMS)))


def in_ldexps(x, n):
    """For a finite x: _in_ldexps' integer arithmetic on x's bits."""
    exponent = x & EXPONENT
    if exponent == 0:
        return x & SIGN
    exponent = (exponent >> 23) + n
    if not 0 < exponent < 255:
        return (x & SIGN) | (EXPONENT if n >= 0 else 0)
    return x & SIGN | exponent << 23 | x & 0x7fffff


def in_exps(x):
    """For a finite x other than 0 whose unbiased exponent lies below 15."""
    scaled = fmul(x, ONE_OVER_LOG_2)
    part = float_(ftrc(scaled)) if scaled & EXPONENT < WHOLE_EXPONENT else scaled
    part |= scaled & SIGN
    fraction = fsub(scaled, part)
    power = ftrc(part)
    if ((fraction & EXPONENT) >> 23) - 127 >= -1:
        if fraction & SIGN:
            fraction, power = fadd(ONES, fraction), power - 1
        else:
            fraction, power = fadd(MINUS_ONES, fraction), power + 1
    t = fmul(LOG_2, fraction)
    return in_ldexps(fdiv(series(t, NUMERATOR), series(t, DENOMINATOR)), power)


def in_fmods(x, y):
    """For a finite x and a finite y other than 0, whose quotient lies below 2 to the 31: the remainder and the
    quotient."""
    sign, x, y = x & SIGN, x & MAGNITUDE, y & MAGNITUDE
    quotient = fdiv(x, y)
    if quotient & EXPONENT < WHOLE_EXPONENT:
        quotient = float_(ftrc(quotient))
    whole = ftrc(fadd(quotient, MINUS_TWO_TO_32) if fcmp_gt(TWO_TO_31, quotient) else quotient)
    return fsub(x, fmul(quotient, y)) | sign, whole & 0xffffffff


def in_atans(x):
    """For a finite x other than 0, 1 and -1 whose unbiased exponent lies below 32."""
    magnitude = x & MAGNITUDE
    if fcmp_gt(magnitude, ONES):
        offset, reduced = 0, magnitude
    else:
        offset, reduced = QUARTER_PI, fdiv(fadd(MINUS_ONES, magnitude), fadd(ONES, magnitude))
    eighth = in_fmods(reduced, AN_EIGHTH)[1]
    if eighth == 0:
        head, argument = offset, reduced
    elif eighth in (1, 2):
        head = fadd(offset, ATAN_QUARTER)
        argument = fdiv(fadd(MINUS_QUOTS, reduced), fadd(ONES, in_ldexps(reduced, -2)))
    elif eighth in (3, 4):
        head = fadd(offset, ATAN_HALF)
        argument = fdiv(fadd(MINUS_HALFS, reduced), fadd(ONES, in_ldexps(reduced, -1)))
    elif eighth in (5, 6):
        head = fadd(offset, ATAN_THREE_QUARTERS)
        argument = fdiv(fadd(reduced, MINUS_THREE_QUARTERS), fmac(THREE_QUARTERS, reduced, ONES))
    else:
        head = fadd(offset, QUARTER_PI)
        argument = fdiv(fadd(MINUS_ONES, reduced), fadd(ONES, reduced))
    return fadd(head, app_atans(argument)) ^ (x & SIGN)


def in_coss(x):
    """For a finite x whose remainder by pi/4 has a quotient below 2 to the 31."""
    remainder, octant = in_fmods(x & MAGNITUDE, QUARTER_PI)
    if fcmp_gt(QUARTER_PI, remainder):
        return ONES
    octant &= 7
    if octant & 1:
        remainder = fadd(MINUS_QUARTER_PI, remainder)
    if octant in (0, 7):
        return app_coss(remainder)
    if octant in (1, 2):
        return app_sins(remainder) ^ SIGN
    if octant in (3, 4):
        return app_coss(remainder) ^ SIGN
    return app_sins(remainder)


def bits(value):
    return struct.unpack('<I', struct.pack('<f', value))[0]


def value(word):
    return struct.unpack('<f', struct.pack('<I', word))[0]


ROUNDING = '''\
#include <fenv.h>
int floats_toward_zero(void)
{
    int mode = fegetround();
    fesetround(FE_TOWARDZERO);
    return mode;
}
void floats_rounding(int mode)
{
    fesetround(mode);
}
'''


def build(scratch, name, nonmatching):
    """The members' library, from their C on the side nonmatching names, at scratch/name."""
    stub = Path(scratch) / 'rounding.c'
    stub.write_text(ROUNDING)
    library = Path(scratch) / name
    compile_host(library, *HOST_FLAGS, stub, 'sdk/runtime/consts.c', *(f'sdk/runtime/{member}.c' for member in MEMBERS),
                 '-lm', nonmatching=nonmatching, shared=True)
    return Library(library)


class Library:
    """The members' C on the host, each kernel run rounding toward zero."""

    def __init__(self, path):
        self.library = ctypes.CDLL(str(path))
        for name in FUSED:
            getattr(self.library, name).restype = ctypes.c_float
            getattr(self.library, name).argtypes = [ctypes.c_float]
        self.library._in_fmods.restype = ctypes.c_float
        self.library._in_fmods.argtypes = [ctypes.c_float, ctypes.c_float, ctypes.POINTER(ctypes.c_uint)]

    def run(self, name, *arguments):
        """Kernel name's result on arguments, floats' bits, as bits; and _in_fmods' quotient after it."""
        quotient = ctypes.c_uint(0)
        passed = [value(argument) for argument in arguments]
        if name == '_in_fmods':
            passed.append(ctypes.byref(quotient))
        mode = self.library.floats_toward_zero()
        try:
            returned = getattr(self.library, name)(*passed)
        finally:
            self.library.floats_rounding(mode)
        return (bits(returned), quotient.value) if name == '_in_fmods' else bits(returned)


class Inputs:
    """Random floats for each kernel, from one seed."""

    def __init__(self):
        self.random = random.Random(0xf1a75)

    def magnitude(self, low, high):
        """A float of either sign whose magnitude lies from 2 to the low to 2 to the high, its exponent drawn
        uniformly."""
        exponent = self.random.randint(low, high - 1)
        return (exponent + 127) << 23 | self.random.getrandbits(23) | self.random.choice((0, SIGN))

    def signed(self, low, high):
        """A float of either sign whose magnitude lies from low to high, drawn uniformly."""
        return bits(self.random.uniform(low, high)) | self.random.choice((0, SIGN))

    def kernels(self):
        """Each kernel's name, its reference and one case's arguments, CASES times over. Half of _app_atans' and
        _in_atans' arguments lie from 0.1 to 1/8, where _app_atans' products come nearest its coefficients."""
        either = self.random.random
        for _ in range(CASES):
            yield '_app_sins', app_sins, (self.signed(0.0, 0.8),)
            yield '_app_coss', app_coss, (self.signed(0.0, 0.8),)
            yield '_app_atans', app_atans, (self.magnitude(-40, -3) if either() < 0.5 else self.signed(0.1, 0.125),)
            yield '_in_logs', in_logs, (self.magnitude(-126, 128) & MAGNITUDE,)
            yield '_in_exps', in_exps, (self.signed(0.0, 89.0) if either() < 0.75 else self.magnitude(-60, 6),)
            yield '_in_atans', in_atans, (self.magnitude(-40, 31) if either() < 0.5 else self.signed(0.1, 0.125),)
            yield '_in_coss', in_coss, (self.signed(0.0, 2000.0) if either() < 0.75 else self.magnitude(-30, 16),)
            yield '_in_fmods', in_fmods, (self.magnitude(-20, 24), self.magnitude(-20, 4))


def outcomes(library):
    """Each case Inputs gives whose shipped arithmetic meets no denormal, as (kernel, arguments, the reference's
    result, the library's)."""
    found = []
    for name, reference, arguments in Inputs().kernels():
        if name == '_in_fmods' and value(arguments[0] & MAGNITUDE) / value(arguments[1] & MAGNITUDE) >= 2 ** 30:
            continue
        if name == '_in_logs' and arguments[0] == ONES or name == '_in_atans' and arguments[0] & MAGNITUDE == ONES:
            continue
        try:
            expected = reference(*arguments)
        except Denormal:
            continue
        found.append((name, arguments, expected, library.run(name, *arguments)))
    return found


class Kernels(unittest.TestCase):
    """The members' C against the references, on the host."""

    @classmethod
    def setUpClass(cls):
        scratch = tempfile.TemporaryDirectory()
        cls.addClassCleanup(scratch.cleanup)
        cls.fused_library = build(scratch.name, 'fused.so', True)
        cls.fused = outcomes(cls.fused_library)
        cls.plain = outcomes(build(scratch.name, 'plain.so', False))

    def test_runtime_constants_keep_their_exported_bit_patterns(self):
        words = {'IFZEROS': 0, 'IFINFS': EXPONENT, 'IFONES': ONES, 'HALFS': HALFS, 'QUOTS': 0x3e800000,
                 'TWOS': 0x40000000, 'FOURS': 0x40800000, 'PAIS': 0x40490fdc, '2PAIS': 0x40c90fdb,
                 'HLFPAIS': 0x3fc90fdb, 'QUOPAIS': QUARTER_PI, 'LOGE2S': LOG_2}
        expected = {'_IFNANS': 0x7f800001}
        for name, word in words.items():
            expected['_' + name] = word
            expected['_N' + name] = word | SIGN
        for name, word in expected.items():
            with self.subTest(symbol=name):
                self.assertEqual(ctypes.c_uint32.in_dll(self.fused_library.library, name).value, word)

    def test_the_non_matching_side_gives_the_shipped_bits(self):
        ran = {name for name, _, _, _ in self.fused}
        self.assertEqual(ran, set(SHIPPED_FMACS) - {'_in_ldexps'})
        for name, arguments, expected, got in self.fused:
            with self.subTest(kernel=name, arguments=[f'{word:#010x}' for word in arguments]):
                self.assertEqual(got, expected)

    def test_the_plain_side_gives_other_bits_where_the_shipped_code_fuses(self):
        # a sum the shipped code fuses rounds twice here, so the check of the fused side can fail. rounding toward
        # zero, a product of its addend's sign whose step is no wider than the addend's gives the same sum rounded
        # first or not. each step of _in_logs' sum is such a sum, and so is _in_atans' own 1 + r b: their plain sides
        # part from the shipped bits only through _app_atans
        parted = {name for name, _, expected, got in self.plain if got != expected}
        self.assertEqual(parted, set(FUSED) - {'_in_logs'})


class Objects(unittest.TestCase):
    """GCC's code for each member, as make dc compiles it."""

    def setUp(self):
        if not (SH_ELF / 'bin/sh-elf-gcc').is_file() or not (SH4ZAM / 'include/sh4zam').is_dir():
            self.skipTest('no SH-4 GCC and SH4ZAM at SH_ELF and SH4ZAM')

    def test_gcc_fuses_where_the_shipped_code_does_and_calls_no_fmaf(self):
        found = {}
        with tempfile.TemporaryDirectory() as scratch:
            for member in MEMBERS:
                obj = Path(scratch) / f'{member}.o'
                built = subprocess.run([str(SH_ELF / 'bin/sh-elf-gcc'), *gcc_flags(SH_ELF), '-Os', '-isystem',
                                        str(SH4ZAM / 'include'), '-Isdk/include', '-c', '-o', str(obj),
                                        f'sdk/runtime/{member}.c'], cwd=ROOT,
                                       stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True)
                self.assertEqual((built.returncode, built.stdout), (0, ''), member)
                for name, code in sh4.functions(SH_ELF / 'bin/sh-elf-objdump', obj).items():
                    found[name] = sum(mnemonic == 'fmac' for _, mnemonic, _ in code)
                undefined = subprocess.run([str(SH_ELF / 'bin/sh-elf-nm'), '-u', str(obj)], stdout=subprocess.PIPE,
                                           check=True, text=True).stdout
                self.assertNotIn('fmaf', undefined, member)
        self.assertEqual(found, SHIPPED_FMACS)


def operands(text):
    """Each .FDATA.S operand of text, assembly as asmsh reads it, as (line, operand)."""
    return [(text.count('\n', 0, line.start()) + 1, operand.strip())
            for line in FDATA.finditer(text) for operand in line[1].split(',')]


def decimal(operand):
    """The value of operand, an F' constant, as a Fraction."""
    sign, whole, fraction, exponent = CONSTANT.fullmatch(operand).groups()
    magnitude = int(whole + fraction or '0') * Fraction(10) ** (int(exponent or 0) - len(fraction))
    return -magnitude if sign == '-' else magnitude


def steps_past_tie(magnitude):
    """How far magnitude, a positive value in the normal floats' range, lies past the tie between the floats on
    either side of it, in steps of those floats: from -1/2 to 1/2."""
    power = magnitude.numerator.bit_length() - magnitude.denominator.bit_length()
    while Fraction(2) ** power > magnitude:
        power -= 1
    while Fraction(2) ** (power + 1) <= magnitude:
        power += 1
    return magnitude / Fraction(2) ** (power - 23) % 1 - Fraction(1, 2)


def nearest(magnitude):
    """The float nearest magnitude, as bits, away from a tie."""
    below = sh4.toward_zero(magnitude, 0)
    return below + (steps_past_tie(magnitude) > 0)


def tree_operands():
    """Each .FDATA.S operand of the tree's .src files and C sources, as (path:line, operand)."""
    found = []
    for top in ('src', 'sdk', 'modules'):
        for path in sorted((ROOT / top).rglob('*')):
            if path.suffix in ('.src', '.c'):
                text = path.read_text(encoding='latin-1')
                found += [(f'{path.relative_to(ROOT)}:{line}', operand) for line, operand in operands(text)]
    return found


class Constants(unittest.TestCase):
    def test_each_constant_lies_off_a_tie(self):
        found = tree_operands()
        self.assertTrue(found)
        for at, operand in found:
            magnitude = abs(decimal(operand))
            if magnitude:
                with self.subTest(at=at, operand=operand):
                    self.assertGreaterEqual(abs(steps_past_tie(magnitude)), MARGIN)

    def test_reads_a_line_s_operands_and_a_constant_s_value(self):
        self.assertEqual(operands("L1:     .FDATA.S    F'1.0,F'-0.5 ; one\n        .DATA.L     1\n"
                                  "        .FDATA.S    F'3.4S+38\n"),
                         [(1, "F'1.0"), (1, "F'-0.5"), (3, "F'3.4S+38")])
        self.assertEqual(decimal("F'-10430.381"), Fraction(-10430381, 1000))
        self.assertEqual(decimal("F'3.4S+38"), 34 * Fraction(10) ** 37)
        # 2 to the 24 plus 1 lies on the tie between 2 to the 24 and the float above it
        self.assertEqual(steps_past_tie(Fraction(16777217)), 0)
        self.assertEqual(steps_past_tie(Fraction(1)), Fraction(-1, 2))

    def test_gnu_as_reads_each_constant_as_the_float_nearest_it(self):
        if not (SH_ELF / 'bin/sh-elf-as').is_file():
            self.skipTest(f'no GNU as under {SH_ELF}: SH_ELF names a toolchain')
        found = tree_operands()
        with tempfile.TemporaryDirectory() as scratch:
            text, obj = Path(scratch) / 'constants.s', Path(scratch) / 'constants.o'
            # as build/shckit translates them: .float, with e for asmsh's S
            text.write_text(''.join(f'\t.float {re.sub("[sS]", "e", operand[2:])}\n' for _, operand in found))
            subprocess.run([str(SH_ELF / 'bin/sh-elf-as'), '--little', '-o', str(obj), str(text)], check=True)
            image = Path(scratch) / 'constants.bin'
            subprocess.run([str(SH_ELF / 'bin/sh-elf-objcopy'), '-O', 'binary', '-j', '.text', str(obj),
                            str(image)], check=True)
            words = struct.unpack(f'<{len(found)}I', image.read_bytes())
        for (at, operand), word in zip(found, words):
            exact = decimal(operand)
            with self.subTest(at=at, operand=operand):
                self.assertEqual(word, nearest(abs(exact)) | (SIGN if exact < 0 else 0))


if __name__ == '__main__':
    unittest.main()
