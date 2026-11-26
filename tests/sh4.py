"""A reading of the SH-4's instruction set, enough to run engine/dc's kernels as sh-elf-objdump prints GCC's objects
of them. It is no console: it runs what the manual says each instruction does, as this file reads the manual, and a
console that does otherwise goes unseen.

It knows the moves, loads and stores of MOV, MOV.L, MOVCA.L, FMOV and FMOV.S, with FPSCR.SZ deciding whether FMOV
moves one float or a pair; MOVA, ADD, NEG, CMP/EQ, TST and DT; BT, BF, BT/S, BF/S, BRA, JSR and RTS, the last five with their
delay slots; LDS and STS of FPUL, and STS.L and LDS.L of PR; FLDS and FSTS; FLDI0, FLDI1, FNEG, FMUL, FADD, FSUB, FDIV and FMAC;
FSCA, FIPR and FTRV; FRCHG and FSCHG; and NOP. Any other instruction stops a run, and names itself. A call by JSR runs
the function that starts at its target, of those a run is given, and RTS returns to PR.

This model fixes FPSCR.RM = 1 and DN = 1. FMUL, FADD, FSUB, FDIV and FMAC round toward zero and flush denormals.
They work in exact fractions and stop a run at an infinite or NaN operand, whose rules they don't model. A caller gives FSCA, FIPR and
FTRV: tests/kernels/console.c's, which sh4_fpu_approx.h models as the silicon computes them. MOVCA.L fills the line
it claims with H'A5 bytes, as a cache line it claimed would hold what the cache held. FPSCR.PR stays clear: no
instruction this file knows sets it."""

import re
import subprocess
from fractions import Fraction

# the bits of a single float's sign and of its exponent and fraction
SIGN = 0x80000000
MAGNITUDE = 0x7fffffff
# the largest finite float, which an overflow rounds toward zero to
LARGEST = 0x7f7fffff
# the byte a claimed cache line holds where nothing stored it
CLAIMED = 0xa5
# PR's value while a run's first function runs, which its RTS returns to
RETURNED = 0xfffffffe
# an instruction's line in sh-elf-objdump's output: its address, its two bytes, its mnemonic and its operands
LINE = re.compile(r'\s*([0-9a-f]+):\t([0-9a-f]{2}) ([0-9a-f]{2})\s*\t(\S+)\s*(.*)')
FUNCTION = re.compile(r'[0-9a-f]+ <_(\w+)>:')
# a comma between operands, not one inside @(disp,Rn)
OPERANDS = re.compile(r',(?![^(]*\))')


class Unmodeled(Exception):
    """An instruction or an operand this reading doesn't cover."""


def functions(objdump, path):
    """Each function of the object at path, by its C name, as its instructions: (address, mnemonic, operands). Each
    function's addresses count from its section's start, as -ffunction-sections gives each its own."""
    text = subprocess.run([str(objdump), '-d', str(path)], stdout=subprocess.PIPE, check=True, text=True).stdout
    found, name = {}, None
    for line in text.splitlines():
        head = FUNCTION.match(line)
        if head:
            name = head[1]
            found[name] = []
            continue
        row = LINE.match(line)
        if row and name:
            # a load from the pool shows the word it loads after a !, and each address the symbol it lies in
            operands = re.sub(r'\s*<[^>]*>', '', re.sub(r'\s*!.*', '', row[5])).strip()
            found[name].append((int(row[1], 16), row[4], [part.strip() for part in OPERANDS.split(operands)]
                                if operands else []))
    return found


def image(objdump, path):
    """The bytes of the code of the object at path, its pools' words among them, by address: what a load from a
    pool reads."""
    text = subprocess.run([str(objdump), '-d', '-z', str(path)], stdout=subprocess.PIPE, check=True, text=True).stdout
    found = {}
    for row in map(LINE.match, text.splitlines()):
        if row:
            found[int(row[1], 16)], found[int(row[1], 16) + 1] = int(row[2], 16), int(row[3], 16)
    return found


def exact(bits):
    """The value of a float's bits, a denormal as 0."""
    exponent = bits >> 23 & 0xff
    if exponent == 0xff:
        raise Unmodeled(f'an infinite or NaN operand, {bits:#010x}')
    if exponent == 0:
        return Fraction(0)
    value = Fraction((bits & 0x7fffff) | 0x800000) * Fraction(2) ** (exponent - 150)
    return -value if bits & SIGN else value


def toward_zero(value, zero):
    """The float nearest value toward zero, as bits: zero, a 0 of either sign, where value is 0, and the 0 of
    value's sign where it lies below the least normal."""
    if value == 0:
        return zero
    sign = SIGN if value < 0 else 0
    value = abs(value)
    exponent = value.numerator.bit_length() - value.denominator.bit_length()
    while Fraction(2) ** exponent > value:
        exponent -= 1
    while Fraction(2) ** (exponent + 1) <= value:
        exponent += 1
    if exponent < -126:
        return sign
    if exponent > 127:
        return sign | LARGEST
    return sign | (exponent + 127) << 23 | int(value / Fraction(2) ** (exponent - 23)) & 0x7fffff


def fmul(a, b):
    return toward_zero(exact(a) * exact(b), (a ^ b) & SIGN)


def fadd(a, b):
    # an exact zero sum is +0, but -0 for two zeros of sign -
    return toward_zero(exact(a) + exact(b), a & b & SIGN)


def fmac(f0, fm, fn):
    """FR0 * FRm + FRn, rounded once."""
    return toward_zero(exact(f0) * exact(fm) + exact(fn), (f0 ^ fm) & fn & SIGN)


class Machine:
    """One SH-4 core's registers and memory. banks[0] and banks[1] are the FPU's two banks of sixteen floats, as
    bits: FPSCR.FR, fr, names banks[fr] FR0 to FR15, and the other bank XF0 to XF15, which is XMTRX. memory maps
    each byte's address to the byte, and written records each address a store wrote. fpu gives FSCA, FIPR and FTRV,
    as fsca(angle) -> (sine, cosine), fipr(m, n) -> bits and ftrv(xmtrx, v) -> v, all bits."""

    def __init__(self, fpu):
        self.fpu = fpu
        self.r = [0] * 16
        self.banks = [[0] * 16, [0] * 16]
        self.fr = 0
        self.sz = 0
        self.fpul = 0
        self.pr = RETURNED
        self.t = 0
        self.memory = {}
        self.written = set()
        self.counted = 0

    @property
    def front(self):
        return self.banks[self.fr]

    @property
    def xmtrx(self):
        return self.banks[1 - self.fr]

    def load(self, address):
        if address % 4:
            raise Unmodeled(f'a load off 4, at {address:#x}')
        return int.from_bytes(bytes(self.memory.get(address + i, 0) for i in range(4)), 'little')

    def store(self, address, word):
        if address % 4:
            raise Unmodeled(f'a store off 4, at {address:#x}')
        for i, byte in enumerate((word & 0xffffffff).to_bytes(4, 'little')):
            self.memory[address + i] = byte
            self.written.add(address + i)

    def call(self, code, arguments=(), stacked=(), linked=()):
        """Run code, a function as functions() gives it, from its first instruction to the RTS that returns from it,
        and that RTS's slot. R4 to R7 take arguments, and the stack at R15 takes stacked, as GCC's calls pass them.
        linked holds each other function a JSR may reach, as functions() gives it, at its own addresses. Returns
        R0."""
        for i, argument in enumerate(arguments):
            self.r[4 + i] = argument & 0xffffffff
        for i, argument in enumerate(stacked):
            self.store(self.r[15] + 4 * i, argument)
        self.pr = RETURNED
        # each instruction of each function the run may reach, by its address: its function and its place there
        at = {address: (function, i) for function in (code, *linked) for i, (address, _, _) in enumerate(function)}
        pc = 0
        while True:
            taken = self.step(code[pc])
            if taken is None:
                pc += 1
            elif taken[0] == 'now':
                code, pc = at[taken[1]]
            else:
                self.step(code[pc + 1])
                if taken[0] == 'past':
                    pc += 2
                    continue
                if taken[0] == 'call':
                    self.pr = code[pc + 2][0]
                elif taken[0] == 'return' and self.pr == RETURNED:
                    return self.r[0]
                if taken[1] not in at:
                    raise Unmodeled(f'a jump to {taken[1]:#x}, where no function it was given starts')
                code, pc = at[taken[1]]

    def step(self, instruction):
        """Run one instruction. Returns None to go on, ('now', target) for a branch with no slot, and ('slot',
        target), ('call', target), ('return', target) or ('past',) for one whose slot runs first."""
        address, mnemonic, operands = instruction
        self.counted += 1
        run = getattr(self, 'do_' + re.sub(r'\W', '_', mnemonic), None)
        if run is None:
            raise Unmodeled(f'{address:#x}: {mnemonic} {",".join(operands)}')
        return run(*operands)

    @staticmethod
    def number(register):
        """The number of the register an operand names: 4 for R4, @R4+, FR4, DR4 or FV4."""
        return int(re.search(r'\d+', register)[0])

    # integer moves and arithmetic
    def do_mov(self, source, target):
        self.r[self.number(target)] = (int(source[1:]) if source.startswith('#') else
                                       self.r[self.number(source)]) & 0xffffffff

    def do_mova(self, address, target):
        # objdump has resolved the aligned PC-relative address.
        if target != 'r0' or not re.fullmatch(r'[0-9a-f]+', address):
            raise Unmodeled(f'mova {address},{target}')
        self.r[0] = int(address, 16)

    def do_mov_l(self, source, target):
        shifted = re.match(r'@\((\d+),r(\d+)\)', source)
        if re.fullmatch(r'[0-9a-f]+', source):
            # a load from a pool, whose address objdump gives in place of @(disp,PC)
            self.r[self.number(target)] = self.load(int(source, 16))
        elif shifted:
            self.r[self.number(target)] = self.load(self.r[int(shifted[2])] + int(shifted[1]))
        elif source.startswith('@') and source.endswith('+'):
            n = self.number(source)
            self.r[self.number(target)] = self.load(self.r[n])
            self.r[n] = (self.r[n] + 4) & 0xffffffff
        elif source.startswith('@'):
            self.r[self.number(target)] = self.load(self.r[self.number(source)])
        elif target.startswith('@-'):
            n = self.number(target)
            value = self.r[self.number(source)]
            self.r[n] = (self.r[n] - 4) & 0xffffffff
            self.store(self.r[n], value)
        elif target.startswith('@('):
            shifted = re.fullmatch(r'@\((\d+),r(\d+)\)', target)
            if not shifted:
                raise Unmodeled(f'mov.l {source},{target}')
            self.store(self.r[int(shifted[2])] + int(shifted[1]), self.r[self.number(source)])
        elif target.startswith('@'):
            self.store(self.r[self.number(target)], self.r[self.number(source)])
        else:
            raise Unmodeled(f'mov.l {source},{target}')

    def do_pref(self, target):
        # A RAM prefetch only affects the cache. P4 includes the store queues.
        if self.r[self.number(target)] >= 0xe0000000:
            raise Unmodeled(f'pref {target} in P4')

    def do_movca_l(self, source, target):
        address = self.r[self.number(target)]
        for i in range(32):
            self.memory[(address & ~31) + i] = CLAIMED
            self.written.add((address & ~31) + i)
        self.store(address, self.r[self.number(source)])

    def do_add(self, source, target):
        value = int(source[1:]) if source.startswith('#') else self.r[self.number(source)]
        self.r[self.number(target)] = (self.r[self.number(target)] + value) & 0xffffffff

    def do_neg(self, source, target):
        self.r[self.number(target)] = -self.r[self.number(source)] & 0xffffffff

    def do_cmp_eq(self, first, second):
        self.t = int(self.r[self.number(first)] == self.r[self.number(second)])

    def do_tst(self, first, second):
        self.t = int(self.r[self.number(first)] & self.r[self.number(second)] == 0)

    def do_dt(self, register):
        n = self.number(register)
        self.r[n] = (self.r[n] - 1) & 0xffffffff
        self.t = int(self.r[n] == 0)

    def do_nop(self):
        pass

    # branches: BT and BF have no slot, and the others run theirs first
    def do_bt(self, target):
        return ('now', int(target, 16)) if self.t else None

    def do_bf(self, target):
        return None if self.t else ('now', int(target, 16))

    def do_bt_s(self, target):
        return ('slot', int(target, 16)) if self.t else ('past',)

    def do_bf_s(self, target):
        return ('past',) if self.t else ('slot', int(target, 16))

    def do_bra(self, target):
        return ('slot', int(target, 16))

    def do_jsr(self, target):
        return ('call', self.r[self.number(target)])

    def do_rts(self):
        return ('return', self.pr)

    # FPUL
    def do_lds(self, source, target):
        if target != 'fpul':
            raise Unmodeled(f'lds {source},{target}')
        self.fpul = self.r[self.number(source)]

    def do_sts(self, source, target):
        if source != 'fpul':
            raise Unmodeled(f'sts {source},{target}')
        self.r[self.number(target)] = self.fpul

    # PR, to and from the stack
    def do_sts_l(self, source, target):
        if source != 'pr' or not target.startswith('@-'):
            raise Unmodeled(f'sts.l {source},{target}')
        n = self.number(target)
        self.r[n] = (self.r[n] - 4) & 0xffffffff
        self.store(self.r[n], self.pr)

    def do_lds_l(self, source, target):
        if target != 'pr' or not source.endswith('+'):
            raise Unmodeled(f'lds.l {source},{target}')
        n = self.number(source)
        self.pr = self.load(self.r[n])
        self.r[n] = (self.r[n] + 4) & 0xffffffff

    def do_flds(self, source, target):
        self.fpul = self.front[self.number(source)]

    def do_fsts(self, source, target):
        self.front[self.number(target)] = self.fpul

    # floats
    def do_fldi0(self, target):
        self.front[self.number(target)] = 0

    def do_fldi1(self, target):
        self.front[self.number(target)] = 0x3f800000

    def do_fneg(self, target):
        self.front[self.number(target)] ^= SIGN

    def do_fmul(self, source, target):
        n = self.number(target)
        self.front[n] = fmul(self.front[n], self.front[self.number(source)])

    def do_fadd(self, source, target):
        n = self.number(target)
        self.front[n] = fadd(self.front[n], self.front[self.number(source)])

    def do_fsub(self, source, target):
        n = self.number(target)
        self.front[n] = fadd(self.front[n], self.front[self.number(source)] ^ SIGN)

    def do_fdiv(self, source, target):
        n = self.number(target)
        a, b = self.front[n], self.front[self.number(source)]
        if not exact(b):
            raise Unmodeled('FDIV with zero divisor')
        self.front[n] = toward_zero(exact(a) / exact(b), (a ^ b) & SIGN)

    def do_fmac(self, f0, source, target):
        n = self.number(target)
        self.front[n] = fmac(self.front[0], self.front[self.number(source)], self.front[n])

    def do_fsca(self, source, target):
        n = self.number(target)
        self.front[n], self.front[n + 1] = self.fpu.fsca(self.fpul)

    def do_fipr(self, first, second):
        m, n = self.number(first), self.number(second)
        self.front[n + 3] = self.fpu.fipr(self.front[m:m + 4], self.front[n:n + 4])

    def do_ftrv(self, matrix, vector):
        n = self.number(vector)
        self.front[n:n + 4] = self.fpu.ftrv(self.xmtrx, self.front[n:n + 4])

    def do_frchg(self):
        self.fr ^= 1

    def do_fschg(self):
        self.sz ^= 1

    def pair(self, register):
        """The bank and first float of a pair FMOV names while SZ is set. sh-elf-objdump prints XDn as FRn+1."""
        n = self.number(register)
        return (self.xmtrx, n - 1) if n % 2 else (self.front, n)

    def do_fmov(self, source, target):
        if self.sz:
            self.fmov_pair(source, target)
            return
        if source.startswith('@'):
            n = self.number(source)
            self.front[self.number(target)] = self.load(self.r[n])
            if source.endswith('+'):
                self.r[n] = (self.r[n] + 4) & 0xffffffff
        elif target.startswith('@-'):
            n = self.number(target)
            self.r[n] = (self.r[n] - 4) & 0xffffffff
            self.store(self.r[n], self.front[self.number(source)])
        elif target.startswith('@'):
            self.store(self.r[self.number(target)], self.front[self.number(source)])
        else:
            self.front[self.number(target)] = self.front[self.number(source)]

    do_fmov_s = do_fmov

    def fmov_pair(self, source, target):
        if source.startswith('@') and source.endswith('+'):
            n = self.number(source)
            bank, i = self.pair(target)
            bank[i], bank[i + 1] = self.load(self.r[n]), self.load(self.r[n] + 4)
            self.r[n] = (self.r[n] + 8) & 0xffffffff
        elif target.startswith('@-'):
            n = self.number(target)
            bank, i = self.pair(source)
            self.r[n] = (self.r[n] - 8) & 0xffffffff
            self.store(self.r[n], bank[i])
            self.store(self.r[n] + 4, bank[i + 1])
        elif re.fullmatch(r'@r\d+', target):
            address = self.r[self.number(target)]
            bank, i = self.pair(source)
            self.store(address, bank[i])
            self.store(address + 4, bank[i + 1])
        elif '@' not in source + target:
            (to, j), (bank, i) = self.pair(target), self.pair(source)
            to[j], to[j + 1] = bank[i], bank[i + 1]
        else:
            raise Unmodeled(f'fmov {source},{target} with SZ set')
