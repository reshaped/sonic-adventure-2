"""engine/dc's kernels, three ways. tests/kernels/kernels.c, built with the host's C compiler, holds each kernel's C
model to the shipped instructions it stands for, on tests/kernels/console.c's FIPR, FTRV and FSCA, and each window
that calls a kernel to its function's listing. Where SH_ELF, or make dc's default tools/sh-elf, holds an SH-4 GCC, the
asm test compiles engine/dc as make dc compiles its C, and runs each function's instructions in tests/sh4.py, a
reading of the instruction set, beside its model. Each asm must give its model's bits, but where turn.h names an
infinite input's NaN, and leave what GCC's calls expect: R8 to R15, FR12 to FR15, FPSCR.SZ clear, and FPSCR.FR and
XMTRX as its contract says. The caller's floats are NaNs, so an asm that reads one it never set gives a NaN. It holds
each function as GCC compiles it out of line. A separate linked caller tests the multi-axis turns when LTO
allocates their operands anew. The window test links the game's functions that are a window and no more with the
kernels, and runs each in tests/sh4.py through its call of the kernel, against the shipped calls it replaces."""

import ctypes
import random
import struct
import subprocess
import tempfile
import unittest
from pathlib import Path

import sh4
from support import ROOT, SH4ZAM, SH_ELF, compile_host, gcc_flags

KERNELS = ('engine/dc/xmtrx.c', 'engine/dc/turn.c')
# a model may spell FMUL and FMAC as * and fmaf, which round as the run sets the host's FPU, so the compiler may
# neither fuse nor fold them
HOST_FLAGS = ('-std=gnu17', '-O1', '-ffp-contract=off', '-frounding-math', '-Wall', '-Wextra', '-Werror',
              '-Iengine/dc', '-Itests/kernels')
# config/gcc.mk gives a source make dc compiles GCCFLAGS and one of LEVELS. Its intermediate code writes no code of
# its own, and the kernels include no header of a route's
LEVELS = ('-Os', '-O2')
# the cases each kernel runs at each level, on random inputs that take the edge cases now and then
CASES = 400
# the angles FSCA gives exact floats for, or that Ninja's tests of 0 tell apart
ANGLES = (0, 0x4000, 0x8000, 0xc000, 0x10000, 0x2000, 1, 0xffff, 0xffffffff)
INFINITIES = (0x7f800000, 0xff800000)
# FIPR's NaN, which turn.h names, and the float each turn gives it in where in has an infinite component
FIPR_NAN = 0x7fbfffff
NAMED = {'zam_vec3_turn_x': 0, 'zam_vec3_turn_y': 1, 'zam_vec3_turn_z': 2, 'zam_vec3_turn_yxz': 2,
         'zam_vec3_turn_zxy': 1}
# where a call's memory lies, and the stack R15 starts at. A load where nothing was put reads 0s
STACK, IN, OUT, POS, WORK = 0x8000, 0x1000, 0x2000, 0x3000, 0x5000
# the sources of the windows that Windows runs whole, with the level config/gcc.mk gives them and the include
# directories make dc gives their route, the root last, and where the link puts their code
WINDOWED = ('src/player/player.c', 'src/camera/helpers.c')
WINDOWED_LEVEL = '-Os'
INCLUDES = ('-Isdk/include', '-Isrc', '-Imodules', '-I.')
TEXT = 0x10000
# the floats of the unit matrix, as njUnitMatrix and matrix_push_unit load it
UNIT = [0x3f800000 if i % 5 == 0 else 0 for i in range(16)]
# what each turn of njRotateX, njRotateY and njRotateZ makes of FSCA's sine and cosine with a null matrix: the two
# vectors its FTRVs take, each a float's four sources, and the two rows of the matrix their products replace.
# FLDI0 gives each 0, and FNEG the negated sine
TURNED = {
    'X': ((('zero', 'cos', 'sin', 'zero'), ('zero', '-sin', 'cos', 'zero')), (1, 2)),
    'Y': ((('cos', 'zero', '-sin', 'zero'), ('sin', 'zero', 'cos', 'zero')), (0, 2)),
    'Z': ((('cos', 'sin', 'zero', 'zero'), ('-sin', 'cos', 'zero', 'zero')), (0, 1)),
}
# each function of WINDOWED that is a window and no more: the vector it turns, work's in place or the x axis into
# out, and its turns, each as its axis, which of its three angles it reads and whether NEG negates it, as its
# listing gives them
WHOLE = {
    # turn_from_world at 0x8c045d88 and turn_to_world at 0x8c045e20 read work->ang, 8 bytes into work, and turn the
    # vector in place
    'turn_from_world': ('work', (('Y', 1, False), ('X', 0, True), ('Z', 2, True))),
    'turn_to_world': ('work', (('Z', 2, False), ('X', 0, False), ('Y', 1, True))),
    # turn_x_axis at 0x8c0ac25a reads ang and turns x into out
    'turn_x_axis': ('x', (('Z', 2, False), ('X', 0, False), ('Y', 1, True))),
}


class Models(unittest.TestCase):
    def test_models_give_the_shipped_bits(self):
        with tempfile.TemporaryDirectory() as scratch:
            program = Path(scratch) / 'kernels'
            compile_host(program, *HOST_FLAGS, 'tests/kernels/kernels.c', 'tests/kernels/console.c', *KERNELS, '-lm')
            ran = subprocess.run([str(program)], cwd=ROOT, stdout=subprocess.PIPE, stderr=subprocess.STDOUT,
                                 text=True)
            self.assertEqual(ran.returncode, 0, ran.stdout)


def bits(value):
    """A float's bits, as a single float holds it."""
    return struct.unpack('<I', struct.pack('<f', value))[0]


class Console:
    """console.c's FIPR, FTRV and FSCA for tests/sh4.py, and the C models beside them, in one host library."""

    def __init__(self, library):
        self.library = ctypes.CDLL(str(library))
        self.library.console_fipr.restype = ctypes.c_uint32
        self.xmtrx = (ctypes.c_uint32 * 16).in_dll(self.library, 'dc_xmtrx')

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

    def model(self, name, *arguments):
        """Run the kernel model in round-toward-zero mode and restore the host's previous rounding mode.
        Each list supplies writable float bits. Passing the same list twice passes one array."""
        arrays = {}
        passed = [arrays.setdefault(id(argument), (ctypes.c_uint32 * len(argument))(*argument))
                  if isinstance(argument, list) else
                  ctypes.c_int32(argument - (1 << 32) if argument >= 1 << 31 else argument)
                  for argument in arguments]
        mode = self.library.console_toward_zero()
        try:
            returned = getattr(self.library, name)(*passed)
        finally:
            self.library.console_rounding(mode)
        for argument, given in zip(arguments, passed):
            if isinstance(argument, list):
                argument[:] = list(given)
        return returned


class Assembly(unittest.TestCase):
    """Each asm body of engine/dc, as make dc's GCC compiles it at each of LEVELS, against its model."""

    @classmethod
    def setUpClass(cls):
        gcc, objdump = SH_ELF / 'bin/sh-elf-gcc', SH_ELF / 'bin/sh-elf-objdump'
        if not gcc.is_file() or not objdump.is_file() or not (SH4ZAM / 'include/sh4zam').is_dir():
            raise unittest.SkipTest(f'need SH-4 GCC and objdump under {SH_ELF} and SH4ZAM headers under {SH4ZAM}')
        scratch = tempfile.TemporaryDirectory()
        cls.addClassCleanup(scratch.cleanup)
        library = Path(scratch.name) / 'console.so'
        compile_host(library, *HOST_FLAGS, 'tests/kernels/console.c', *KERNELS, '-lm', shared=True)
        cls.console = Console(library)
        flags = ['-Isdk/include', '-isystem', str(SH4ZAM / 'include'), *gcc_flags(SH_ELF)]
        cls.code = {}
        for level in LEVELS:
            cls.code[level] = {}
            for source in KERNELS:
                obj = Path(scratch.name) / level / Path(source).with_suffix('.o').name
                obj.parent.mkdir(exist_ok=True)
                built = subprocess.run([str(gcc), *flags, level, '-c', '-o', str(obj), source], cwd=ROOT,
                                       stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True)
                if built.returncode or built.stdout:
                    raise AssertionError(f'{source} at {level}:\n{built.stdout}')
                cls.code[level].update(sh4.functions(objdump, obj))

    def setUp(self):
        self.random = random.Random(0x5ca1ab1e)

    def float_bits(self, low, high):
        """A float of either sign whose magnitude lies from low to high, or now and then 0."""
        if self.random.random() < 1 / 16:
            return self.random.choice((0, sh4.SIGN))
        return bits(self.random.uniform(low, high)) ^ self.random.choice((0, sh4.SIGN))

    def angle(self):
        return self.random.choice(ANGLES) if self.random.random() < 0.25 else self.random.getrandbits(32)

    def matrix(self):
        return [self.float_bits(0.01, 4.0) for _ in range(16)]

    def machine(self, xmtrx, entry_fr):
        """A machine as GCC's code calls a function: every register but R15 and XMTRX holds junk, the floats NaNs,
        and FPSCR's PR and SZ are clear. FR selects either bank."""
        machine = sh4.Machine(self.console)
        machine.r = [0xdead0000 + n for n in range(15)] + [STACK]
        machine.banks = [[0x7fc10000 + 16 * bank + n for n in range(16)] for bank in range(2)]
        machine.fr = entry_fr
        machine.xmtrx[:] = xmtrx
        self.callers = (machine.r[8:], machine.front[12:])
        return machine

    def put(self, machine, address, words):
        for i, word in enumerate(words):
            machine.memory.update(zip(range(address + 4 * i, address + 4 * i + 4), word.to_bytes(4, 'little')))

    def got(self, machine, address, count):
        return [machine.load(address + 4 * i) for i in range(count)]

    def call(self, level, name, machine, *arguments, stacked=(), writes=()):
        """Call name's code at level on machine, and check what GCC's callers rely on: R8 to R15 and the front
        bank's FR12 to FR15 as they were, SZ clear, and no store outside writes, ranges of addresses."""
        returned = machine.call(self.code[level][name], arguments, stacked)
        self.assertEqual((machine.r[8:], machine.front[12:]), self.callers, 'a register GCC keeps')
        self.assertEqual(machine.sz, 0, 'FPSCR.SZ')
        stray = {address for address in machine.written if address < STACK - 64 and
                 not any(start <= address < end for start, end in writes)}
        self.assertFalse(stray, 'a store outside the outputs')
        return returned

    def test_points(self):
        for level in LEVELS:
            for case in range(CASES):
                entry_fr = (case // 10) % 2
                with self.subTest(level=level, case=case):
                    count = case % 10
                    in_stride, out_stride = (12 + 4 * self.random.randrange(4) for _ in range(2))
                    in_place = out_stride <= in_stride and case % 3 == 0
                    xmtrx = self.matrix()
                    points = [self.float_bits(0.001, 1000.0) for _ in range(60)]
                    out = list(points) if in_place else [self.float_bits(0.001, 1000.0) for _ in range(60)]
                    machine = self.machine(xmtrx, entry_fr)
                    self.put(machine, IN, points)
                    self.put(machine, OUT, out)
                    source = OUT if in_place else IN
                    self.call(level, 'zam_xmtrx_transform_points', machine, source, in_stride, OUT, out_stride,
                              stacked=[count], writes=[(OUT, OUT + 4 * 60)])
                    self.console.xmtrx[:] = xmtrx
                    model_in = out if in_place else points
                    self.console.model('zam_xmtrx_transform_points', model_in, in_stride,
                                       model_in if in_place else out, out_stride, count)
                    self.assertEqual(self.got(machine, OUT, 60), out)
                    self.assertEqual((machine.fr, machine.xmtrx), (entry_fr, xmtrx))

    def test_linked_turn_operands(self):
        """The altered pointer and the first angle start equal; LTO must preserve the late angle reads."""
        gcc, objdump = SH_ELF / 'bin/sh-elf-gcc', SH_ELF / 'bin/sh-elf-objdump'
        flags = [*gcc_flags(SH_ELF), '-Isdk/include', '-Iengine/dc', '-isystem', str(SH4ZAM / 'include')]
        words = [0x1357, 0x6789, 0xabcdef, 0xe0123456]
        vector = [bits(1.75), bits(-3.25), bits(4.5)]
        with tempfile.TemporaryDirectory() as directory:
            for level in LEVELS:
                elf = Path(directory) / 'turn.elf'
                subprocess.run([gcc, *flags, level, '-flto', '-nostdlib', '-Wl,--gc-sections',
                                f'-Wl,-Ttext,{TEXT:#x}', '-Wl,-e,_turn_with_live_inputs', '-o', elf,
                                'engine/dc/turn.c', 'tests/kernels/turn_caller.c'], cwd=ROOT, check=True)
                code = sh4.functions(objdump, elf)['turn_with_live_inputs']
                self.assertTrue(any(op == 'fipr' for _, op, _ in code), 'the caller must inline the turns')
                for zxy, name in enumerate(('zam_vec3_turn_yxz', 'zam_vec3_turn_zxy')):
                    for entry_fr in (0, 1):
                        for output in (OUT, IN):
                            with self.subTest(level=level, kernel=name, in_place=output == IN, entry_fr=entry_fr):
                                xmtrx = self.matrix()
                                machine = self.machine(xmtrx, entry_fr)
                                self.put(machine, IN, vector)
                                self.put(machine, WORK, words)
                                machine.call(code, (IN, output, WORK, zxy))
                                expected = [0, 0, 0]
                                self.console.model(name, output, words[0], words[1], vector[:], expected)
                                self.assertEqual(self.got(machine, output, 3), expected)
                                self.assertEqual(self.got(machine, WORK, 4), words[-1:] + words[:-1])
                                self.assertEqual((machine.r[8:], machine.front[12:]), self.callers)
                                self.assertEqual((machine.fr, machine.sz, machine.xmtrx), (entry_fr, 0, xmtrx))

    def test_turns(self):
        for level in LEVELS:
            for case in range(CASES):
                entry_fr = (case // 6) % 2
                xmtrx = self.matrix()
                vector = [self.float_bits(0.001, 1000.0) for _ in range(3)]
                pos = [self.float_bits(0.001, 1000.0) for _ in range(3)]
                if case % 6 == 0:
                    vector[self.random.randrange(3)] = self.random.choice(INFINITIES)
                if case % 6 == 3:
                    pos[self.random.randrange(3)] = self.random.choice(INFINITIES)
                x, y, z = (self.angle() for _ in range(3))
                for name, arguments, stacked, model in (
                        ('zam_vec3_turn_x', (x, IN, OUT), (), (x,)),
                        ('zam_vec3_turn_y', (y, IN, OUT), (), (y,)),
                        ('zam_vec3_turn_z', (z, IN, OUT), (), (z,)),
                        ('zam_vec3_turn_yxz', (x, y, z, IN), (OUT,), (x, y, z)),
                        ('zam_vec3_turn_zxy', (x, y, z, IN), (OUT,), (x, y, z)),
                        ('zam_vec3_place_y', (POS, y, IN, OUT), (), (pos, y)),
                        ('zam_vec3_place_xyz', (POS, x, y, z), (IN, OUT), (pos, x, y, z))):
                    with self.subTest(level=level, kernel=name, case=case):
                        machine = self.machine(xmtrx, entry_fr)
                        self.put(machine, IN, vector)
                        self.put(machine, POS, pos)
                        self.call(level, name, machine, *arguments, stacked=stacked, writes=[(OUT, OUT + 12)])
                        expected = [0, 0, 0]
                        self.console.model(name, *model, list(vector), expected)
                        if name in NAMED and any((word & sh4.MAGNITUDE) == INFINITIES[0] for word in vector):
                            expected[NAMED[name]] = FIPR_NAN
                        self.assertEqual(self.got(machine, OUT, 3), expected)
                        self.assertEqual((machine.fr, machine.xmtrx), (entry_fr, xmtrx))


def shipped_window(console, turns, vector):
    """vector, three floats' bits, as a window's shipped calls turn it: the unit matrix, each turn of turns, an
    (axis, angle) pair, as njRotateX, njRotateY or njRotateZ runs it with a null matrix, and njCalcVector's FTRV of
    the vector and 0. Both FTRVs of a turn read the matrix before either row is replaced."""
    matrix = list(UNIT)
    for axis, angle in turns:
        sine, cosine = console.fsca(angle & 0xffffffff)
        floats = {'zero': 0, 'sin': sine, '-sin': sine ^ sh4.SIGN, 'cos': cosine}
        vectors, rows = TURNED[axis]
        products = [console.ftrv(matrix, [floats[source] for source in sources]) for sources in vectors]
        for row, product in zip(rows, products):
            matrix[4 * row:4 * row + 4] = product
    return console.ftrv(matrix, [*vector, 0])[:3]


class Windows(unittest.TestCase):
    """Each function of WINDOWED that is a window and no more, as make dc's GCC compiles its source, linked with
    turn.c as GCC compiles it at each of LEVELS, against its shipped instructions on console.c's FSCA and FTRV. The
    run goes through the call to the kernel. A finite vector of the function's takes the shipped bits."""

    @classmethod
    def setUpClass(cls):
        tools = {name: SH_ELF / f'bin/sh-elf-{name}' for name in ('gcc', 'ld', 'objdump')}
        if not all(tool.is_file() for tool in tools.values()) or not (SH4ZAM / 'include/sh4zam').is_dir():
            raise unittest.SkipTest(f'no SH-4 GCC, ld and objdump under {SH_ELF}, or no SH4ZAM at {SH4ZAM}: SH_ELF '
                                    'names a toolchain, and SH4ZAM its headers')
        scratch = tempfile.TemporaryDirectory()
        cls.addClassCleanup(scratch.cleanup)
        library = Path(scratch.name) / 'console.so'
        compile_host(library, *HOST_FLAGS, 'tests/kernels/console.c', *KERNELS, '-lm', shared=True)
        cls.console = Console(library)
        flags = gcc_flags(SH_ELF)
        cls.code = {}
        for level in LEVELS:
            objects = []
            for source in (*WINDOWED, 'engine/dc/turn.c'):
                obj = Path(scratch.name) / level / Path(source).with_suffix('.o').name
                obj.parent.mkdir(exist_ok=True)
                own = ((WINDOWED_LEVEL, '-isystem', str(SH4ZAM / 'include'), *INCLUDES) if source in WINDOWED else
                       (level,))
                built = subprocess.run([str(tools['gcc']), *flags, *own, '-c', '-o', str(obj), source], cwd=ROOT,
                                       stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True)
                if built.returncode or built.stdout:
                    raise AssertionError(f'{source} at {level}:\n{built.stdout}')
                objects.append(str(obj))
            # the sources' other names are left at 0: no function run here reaches one
            linked = Path(scratch.name) / level / 'linked.elf'
            built = subprocess.run([str(tools['ld']), '-EL', f'-Ttext={TEXT:#x}', '--unresolved-symbols=ignore-all',
                                    '-e', '0', '-nostdlib', '-o', str(linked), *objects],
                                   stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True)
            if built.returncode:
                raise AssertionError(f'the link at {level}:\n{built.stdout}')
            cls.code[level] = (sh4.functions(tools['objdump'], linked), sh4.image(tools['objdump'], linked))

    def test_functions_give_the_shipped_bits(self):
        chosen = random.Random(0x7e57ab1e)
        for level in LEVELS:
            functions, image = self.code[level]
            for name, (vector, turns) in WHOLE.items():
                self.assertIn(name, functions, 'GCC kept no code of its own for it')
                for case in range(CASES // 4):
                    with self.subTest(level=level, function=name, case=case):
                        angles = [chosen.choice(ANGLES) if chosen.random() < 0.25 else chosen.getrandbits(32)
                                  for _ in range(3)]
                        machine = sh4.Machine(self.console)
                        machine.r[15] = STACK
                        machine.banks[1][:] = xmtrx = [bits(chosen.uniform(-4.0, 4.0)) for _ in range(16)]
                        machine.memory.update(image)
                        if vector == 'work':
                            given = [bits(chosen.uniform(-1000.0, 1000.0)) for _ in range(3)]
                            for i, word in enumerate(angles + given):
                                machine.store((WORK + 8 if i < 3 else IN - 12) + 4 * i, word)
                        else:
                            given = [bits(1.0), 0, 0]
                            for i, word in enumerate(angles):
                                machine.store(WORK + 4 * i, word)
                        machine.call(functions[name], (WORK, IN), linked=functions.values())
                        turned = [(axis, -angles[i] if negated else angles[i]) for axis, i, negated in turns]
                        self.assertEqual([machine.load(IN + 4 * i) for i in range(3)],
                                         shipped_window(self.console, turned, given))
                        self.assertEqual((machine.fr, machine.xmtrx), (0, xmtrx))
                        self.assertEqual(machine.r[15], STACK)


class Interpreter(unittest.TestCase):
    """tests/sh4.py's reading of what an instruction does."""

    def run_code(self, lines, machine=None):
        machine = machine or sh4.Machine(None)
        code = [(2 * i, mnemonic, operands.split(',') if operands else [])
                for i, (mnemonic, operands) in enumerate(line.partition(' ')[::2] for line in lines)]
        machine.call(code)
        return machine

    def test_rounds_toward_zero_and_flushes_a_denormal(self):
        # each exact result lies past half a step, where rounding to nearest would give the float above
        self.assertEqual(sh4.fmul(0x3f800001, bits(1.5)), 0x3fc00001)
        self.assertEqual(sh4.fadd(bits(1.0), bits(-1.25 * 2 ** -24)), 0x3f7ffffe)
        # a product under the least normal is 0, and so is a denormal operand
        self.assertEqual(sh4.fmac(bits(2.0 ** -100), bits(-(2.0 ** -30)), sh4.SIGN), sh4.SIGN)
        self.assertEqual(sh4.fmul(1, bits(2.0 ** 100)), 0)

    def test_runs_a_slot_whether_a_branch_takes_it_or_not(self):
        # T is clear, so BT/S falls through, and BF/S branches past the ADD of 4. both slots run
        machine = self.run_code(['mov #1,r1', 'tst r1,r1', 'bt.s c', 'add #1,r2', 'bf.s e', 'add #2,r2',
                                 'add #4,r2', 'rts ', 'nop '])
        self.assertEqual(machine.r[2], 3)

    def test_names_xd_pairs_as_objdump_prints_them_while_sz_is_set(self):
        machine = sh4.Machine(None)
        machine.r[4] = 0x100
        machine.banks[1][0:2] = [11, 12]
        machine.banks[0][0:2] = [21, 22]
        self.run_code(['fschg ', 'fmov fr1,@-r4', 'fmov fr0,@-r4', 'fschg ', 'rts ', 'nop '], machine)
        self.assertEqual([machine.load(0xf0 + 4 * i) for i in range(4)], [21, 22, 11, 12])

    def test_stops_at_an_instruction_it_does_not_know(self):
        with self.assertRaisesRegex(sh4.Unmodeled, 'fsqrt'):
            self.run_code(['fsqrt fr0', 'rts ', 'nop '])


class DirectPoints(unittest.TestCase):
    """Exercise the selected direct-point loops and their near-clip callback ABI."""

    def test_clip_masks_strip_windows_and_short_counts(self):
        with tempfile.TemporaryDirectory() as temp:
            output = Path(temp) / 'direct-points'
            for level in ('-Os', '-O2'):
                with self.subTest(level=level):
                    compile_host(output, '-std=gnu17', level, '-ffp-contract=off', '-frounding-math', '-Wall',
                                 '-Wextra', '-Werror', '-Iengine/dc', '-Itests/kernels', 'engine/dc/direct_points.c',
                                 'tests/kernels/console.c', 'tests/kernels/direct_points.c', '-lm')
                    subprocess.run([output], check=True, timeout=10)


if __name__ == '__main__':
    unittest.main()
