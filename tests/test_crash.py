"""engine/dc's crash screen, two ways. The host's C compiler builds crash_report.c, whose lines the tests read from
crash_screen as text, and whose reading of a master's frame they hold to the pushes of sdk/shinobi/syIntIsr.src's two
masters. Where SH_ELF, or make dc's default tools/sh-elf, holds an SH-4 GCC, the console test compiles the screen as
make dc compiles engine/dc's C, at each optimization level. The host also exercises vector ownership and the watchdog
through ordinary mocked Shinobi calls, and runs crash.c's capture of a hang over stand-ins for the control registers,
for what Shinobi's master keeps and for main memory. That build holds crash.c's glyphs too, and every character of
the lines it draws must have one. Register capture and scanout on Dreamcast hardware are outside these tests."""

import ctypes
import re
import string
import subprocess
import tempfile
import unittest
from pathlib import Path

from support import ROOT, SH4ZAM, SH_ELF, compile_host, gcc_flags

REPORT = 'engine/dc/crash_report.c'
HANG = 'tests/crash/hang.c'
CONSOLE = ('engine/dc/crash.c', 'engine/dc/crash_watch.c', REPORT)
HOST_FLAGS = ('-std=gnu17', '-O1', '-Wall', '-Wextra', '-Werror')
LEVELS = ('-Os', '-O2')
# the include directories make dc gives engine/'s C: the game route's, then the root
INCLUDES = ('-Isdk/include', '-Isrc', '-Imodules', '-I.')
# crash_report.h's sizes, which the tests read from the header, as the C reads them
HEADER = (ROOT / 'engine/dc/crash_report.h').read_text()
SIZES = {name: int(value) for name, value in re.findall(r'#define (CRASH_\w+) \(?(-?\d+)\)?$', HEADER, re.M)}
COLUMNS, CALLERS, LINES = SIZES['CRASH_COLUMNS'], SIZES['CRASH_CALLERS'], SIZES['CRASH_LINES']
SR_RB = 0x20000000
SR_BL = 0x10000000
# main memory in P1's view, and its bytes
MEMORY, MEMORY_SIZE = 0x8c000000, 0x01000000


class Stop(ctypes.Structure):
    """crash_report.h's struct crash_stop, as the host lays it out."""
    _fields_ = [('what', ctypes.c_char_p), ('number', ctypes.c_uint32), ('name', ctypes.c_char_p),
                ('code', ctypes.c_uint32), ('address', ctypes.c_uint32), ('pc', ctypes.c_uint32),
                ('sr', ctypes.c_uint32), ('sp', ctypes.c_uint32), ('registers', ctypes.c_int),
                ('pr', ctypes.c_uint32), ('r', ctypes.c_uint32 * 15), ('mode', ctypes.c_int32),
                ('stage', ctypes.c_int32), ('slot', ctypes.c_char * SIZES['CRASH_NAME_BYTES']), ('callers', ctypes.c_int),
                ('caller_offset', ctypes.c_uint32 * CALLERS), ('caller', ctypes.c_uint32 * CALLERS)]


def stopped(**given):
    """A crash_stop: a crash of an address error on a write, with registers, during the card warning, unless
    given says otherwise."""
    stop = Stop(what=b'CRASH', number=0x100, code=0x100, address=0x8c0fffff, pc=0x8c500a2e, sr=0x40000001,
                sp=0x8c27f000, registers=1, pr=0x8c0123ab, mode=20, stage=0, slot=b'MCWARN  ', callers=0)
    for i in range(15):
        stop.r[i] = 0x01010101 * i
    for name, value in given.items():
        setattr(stop, name, value)
    return stop


def pushes(source, start):
    """The registers a master of source pushes as its frame, from the label start: each STC.L, STS.L or MOV.L to
    @-R15 after the pop of the stub's R1 or the label, up to FPSCR's, by word from the frame's lowest address."""
    lines = source[source.index(f'\n{start}:\n'):].splitlines()[1:]
    pushed = []
    for line in lines:
        found = re.match(r'\s+(?:STC\.L|STS\.L|MOV\.L)\s+(\w+),@-R15\b', line)
        if found:
            pushed.append(found[1])
            if found[1] == 'FPSCR':
                return pushed[::-1]
        elif pushed and not line.strip().startswith(';'):
            raise AssertionError(f'{start}: the pushes break at {line.strip()!r}')
    raise AssertionError(f'{start}: no push of FPSCR')


def floats(source, start):
    """The words of the floats the master at start pushes by FMOV pairs to @-R15 before it calls a handler."""
    body = source[source.index(f'\n{start}:\n'):]
    body = body[:body.index('JSR')]
    return 2 * len(re.findall(r'^\s+FMOV\s+(?:DR|XD)\d+,@-R15$', body, re.M))


# the instructions of syIntIsr.src that write the general register they name last, those that only read it, and
# those that leave T as it was
WRITERS = {'ADD', 'AND', 'DT', 'MOV', 'MOV.L', 'MOVA', 'OR', 'SHAR', 'SHLL8', 'SHLL16', 'STC', 'XOR'}
READERS = {'CMP/GE', 'TST'}
T_KEEPERS = {'ADD', 'MOV', 'MOV.L', 'MOVA', 'NOP'}


def moved(held, by):
    """What a register holds once by is added to held, as kept_stacks describes the two."""
    return held and (held[0], 0 if held[0] == 'stack' else held[1] + by)


def kept_stacks(source):
    """Each store of source, syIntIsr.src, to syIntLastStack: what it stores, as the stopped code's R15 plus that many
    bytes, or None for anything else.

    This reads every instruction in the file's order. It forgets what the registers hold at each label, where a
    branch may arrive. It places every store: on a stack, at a label's own word, or at one of the master's words,
    which end the file. It fails on a store it can't place. It fails on a store to syIntLastStack that a run inside
    another reaches: any but one past a BF or BF/S whose T says that syIntNestCount was 0 at the master's entry, past
    a BF/S's delay slot, and before the next label. Each stub enters the master with one change to the stopped code's
    R15, its push of R1, or this fails.

    What a register holds is None where this doesn't know, or a kind and a number: 'word', the address that many
    bytes past syIntLastStack; 'stopped', the address that many bytes past the stopped code's R15; 'stack', another
    address on a stack; 'label', a label's own word; 'count', syIntNestCount's value at the master's entry plus that
    many; and 'const', the number."""
    for stub in ('_syIntExpISRTop', '?int_latch'):
        entry = source[source.index(f'\n{stub}:\n'):]
        if re.findall(r'^\s+\S+\s+(\S*R15\S*)', entry[:entry.index('JMP')], re.M) != ['R1,@-R15']:
            raise AssertionError(f'{stub}: the stub does more to R15 than push R1')
    lines = [line.split(';')[0].rstrip() for line in source.splitlines()]
    # the master's words by their bytes from syIntLastStack, and each pool word that holds the address of one
    words, size = {}, 0
    for line in lines[lines.index('_syIntLastStack:'):]:
        name, *operands = line.split() or ['']
        if name.endswith(':'):
            words[name[:-1]] = size
        elif name in ('.DATA.L', '.RES.L'):
            size += 4 * (int(operands[0]) if name == '.RES.L' else 1)
        elif name not in ('', '.END'):
            raise AssertionError(f'{line.strip()!r} lies among the master\'s words')
    pools = {label[:-1]: words[found[1]] for label, line in zip(lines, lines[1:])
             if label.endswith(':') and (found := re.fullmatch(r'\s+\.DATA\.L\s+(\w+)', line)) and found[1] in words}
    last_stack, nest_count = ('word', words['_syIntLastStack']), ('word', words['_syIntNestCount'])
    # count is what syIntNestCount holds, less its value at the master's entry. flag is the 'count' that T says is 0.
    # past the branch only an outermost run falls through, shared counts the instructions that every run still
    # executes: the delay slot
    holds, count, flag, shared, stores = {}, None, None, None, []

    def hold(register):
        return holds.get(register) or (('stack', 0) if register == 'R15' else None)

    for line in lines:
        name, *operands = line.split() or ['']
        if not line[:1].isspace():
            if line:
                holds, count, flag, shared = {}, None, None, None
                if name == '_syIntMasterISR_2:':
                    holds, count = {'R15': ('stopped', -4)}, 0
            continue
        if name.startswith('.'):
            continue
        *sources, last = re.split(r',(?![^(]*\))', ''.join(operands))
        source = sources[0] if sources else ''
        if name in ('BF', 'BF/S') and flag == ('count', 0):
            shared = 1 if name == 'BF/S' else 0
            continue
        if name == 'JSR':
            holds = {}
        kept = hold(source)
        if popped := re.fullmatch(r'@(R\d+)\+', source):
            holds[popped[1]] = moved(hold(popped[1]), 4)
        if last.startswith('@') and name not in ('JMP', 'JSR'):
            found = re.fullmatch(r'@(-?)(R\d+)', last)
            at = found and moved(hold(found[2]), -4 if found[1] else 0)
            if found and found[1]:
                holds[found[2]] = at
            if not at or not (at[0] in ('stack', 'stopped') or at == ('label', 0) or
                              (at[0] == 'word' and at[1] in words.values())):
                raise AssertionError(f'{line.strip()!r} stores where this can\'t place')
            if at == last_stack:
                if shared != 0:
                    raise AssertionError(f'{line.strip()!r} stores to syIntLastStack on a run inside another')
                stores.append(kept[1] if kept and kept[0] == 'stopped' else None)
            elif at == nest_count:
                count = kept[1] if kept and kept[0] == 'count' else None
        elif re.fullmatch(r'R\d+', last) and name not in READERS:
            if name not in WRITERS:
                raise AssertionError(f'{line.strip()!r}: this doesn\'t read {name}')
            new = None
            if name == 'MOVA':
                new = ('word', words[source]) if source in words else ('label', 0)
            elif name == 'MOV.L' and not source.startswith('@'):
                new = ('word', pools[source]) if source in pools else None
            elif name == 'MOV.L' and re.fullmatch(r'@R\d+', source):
                new = ('count', count) if hold(source[1:]) == nest_count and count is not None else None
            elif name == 'MOV':
                new = ('const', int(source[1:])) if re.fullmatch(r'#-?\d+', source) else kept
            elif name == 'ADD' and re.fullmatch(r'#-?\d+', source):
                new = moved(hold(last), int(source[1:]))
            elif name == 'AND':
                # a copy of R15 under a negative number is R15 rounded down, so still on its stack
                pair = {kept, hold(last)}
                rounds = any(held and held[0] == 'const' and held[1] < 0 for held in pair)
                new = ('stack', 0) if ('stack', 0) in pair and rounds else None
            elif name == 'DT':
                held = hold(last)
                new = flag = moved(held, -1) if held and held[0] == 'count' else None
            holds[last] = new
        if name not in T_KEEPERS and name != 'DT':
            flag = None
        if shared:
            shared -= 1
    return stores


class Host(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        scratch = tempfile.TemporaryDirectory()
        cls.addClassCleanup(scratch.cleanup)
        library = Path(scratch.name) / 'crash_report.so'
        compile_host(library, *HOST_FLAGS, REPORT, shared=True)
        cls.library = ctypes.CDLL(str(library))
        cls.screen = (ctypes.c_char * (COLUMNS + 1) * LINES).in_dll(cls.library, 'crash_screen')
        # the frame's words, as crash_report.h's enum crash_frame counts them
        cls.frame_words = ctypes.c_int.in_dll(cls.library, 'crash_frame_words').value

    def lines(self):
        """The screen's lines as crash_draw last drew them."""
        return [bytes(line).split(b'\0')[0].decode() for line in self.screen]

    def stop(self, **given):
        return stopped(**given)

    def show(self, stop):
        self.library.crash_show(ctypes.byref(stop))
        return self.lines()

    def test_a_crash_draws_each_line_in_order_and_blanks_the_rest(self):
        stop = self.stop(callers=4)
        for i, (offset, word) in enumerate(((0, 0x8c012344), (12, 0x8c50001c), (40, 0x0c9c0100), (2044, 0x8c0abcde))):
            stop.caller_offset[i], stop.caller[i] = offset, word
        self.assertEqual(self.show(stop), [
            'CRASH 00000100',
            'PC 8c500a2e PR 8c0123ab SR 40000001',
            'SP 8c27f000 ADDR 8c0fffff',
            'MODE 20 STAGE 0 SLOT MCWARN  ',
            'R0 00000000 01010101 02020202 03030303',
            'R4 04040404 05050505 06060606 07070707',
            'R8 08080808 09090909 0a0a0a0a 0b0b0b0b',
            'R12 0c0c0c0c 0d0d0d0d 0e0e0e0e',
            '000 8c012344  00c 8c50001c  028 0c9c0100',
            '7fc 8c0abcde',
        ] + [''] * (LINES - 10))

    def test_only_an_address_error_and_trapa_draw_the_address(self):
        for code, where in ((0x0e0, 'SP 8c27f000 ADDR 0000002c'), (0x160, 'SP 8c27f000 TRAPA 0000002c'),
                            (0x180, 'SP 8c27f000'), (0x1a0, 'SP 8c27f000'), (0, 'SP 8c27f000')):
            with self.subTest(code=hex(code)):
                self.assertEqual(self.show(self.stop(code=code, address=0x2c))[2], where)

    def test_a_stop_draws_its_name_in_capitals_and_no_registers(self):
        lines = self.show(self.stop(what=b'read runs on', number=0xfffffffd, name=b'stg13.prs', code=0,
                                    registers=0, callers=-1, mode=-1, stage=13, slot=b'STG13   '))
        self.assertEqual(lines[:6], ['READ RUNS ON fffffffd', 'STG13.PRS', 'PC 8c500a2e SR 40000001', 'SP 8c27f000',
                                     'MODE -1 STAGE 13 SLOT STG13   ', 'NO STACK AT SP'])
        self.assertEqual(lines[6:], [''] * (LINES - 6))

    def test_slot_shows_an_overlays_name_and_a_question_mark_for_other_bytes(self):
        # names as the disc's heads give them, and what the slot holds between overlays or under another file
        for slot, shown in ((b'EMBLEM_G', 'EMBLEM_G'), (b'STG13   ', 'STG13   '), (b'LASTBS2 ', 'LASTBS2 '),
                            (b'', '?'), (b'stg13   ', '?'), (b'1STG    ', '?'), (b' STG13  ', '?'),
                            (b'STG13\x00\x00\x00', '?'), (b'\x10\x00\x00\x00\x48\x01\x00\x00', '?')):
            with self.subTest(slot=slot):
                self.assertEqual(self.show(self.stop(slot=slot))[3], f'MODE 20 STAGE 0 SLOT {shown}')

    def test_a_line_stops_at_the_columns(self):
        lines = self.show(self.stop(name=b'x' * 60))
        self.assertEqual(lines[1], 'X' * COLUMNS)

    def test_decimal_takes_the_least_and_the_most_of_an_int32(self):
        self.assertEqual(self.show(self.stop(mode=-2 ** 31, stage=2 ** 31 - 1))[3],
                         'MODE -2147483648 STAGE 2147483647 SLOT MCWAR')

    def test_the_most_callers_fill_the_screen_three_to_a_line(self):
        stop = self.stop(callers=CALLERS)
        for i in range(CALLERS):
            stop.caller_offset[i], stop.caller[i] = 4 * i, 0x8c010000 + 4 * i
        lines = self.show(stop)
        self.assertEqual(lines[8:], [f'{12 * n:03x} {0x8c010000 + 12 * n:08x}  {12 * n + 4:03x} {0x8c010004 + 12 * n:08x}'
                                     f'  {12 * n + 8:03x} {0x8c010008 + 12 * n:08x}' for n in range(CALLERS // 3)] + [''])
        self.assertEqual(len(lines), LINES)
        # a screen drawn again keeps none of the last one's lines
        self.assertEqual(self.show(self.stop(registers=0))[4:], [''] * (LINES - 4))

    def test_a_report_that_faulted_draws_the_first_exception_then_the_second(self):
        self.show(self.stop(callers=-1))
        self.library.crash_show_fault(ctypes.byref(self.stop(name=b'NOT DRAWN')), ctypes.c_uint32(0x0e0),
                                      ctypes.c_uint32(0x8c010abc))
        self.assertEqual(self.lines()[:5], ['CRASH 00000100', 'PC 8c500a2e PR 8c0123ab SR 40000001',
                                            'SP 8c27f000 ADDR 8c0fffff', 'REPORT FAULTED 000000e0 PC 8c010abc',
                                            'R0 00000000 01010101 02020202 03030303'])

    def test_an_unknown_stop_draws_its_name_and_no_pc_sr_or_sp(self):
        self.show(self.stop(callers=4))
        self.library.crash_show_unknown(ctypes.byref(self.stop(what=b'HANG', number=1800, name=b'a name')))
        self.assertEqual(self.lines(), ['HANG 00000708', 'A NAME', 'PC SR AND SP NOT KNOWN',
                                        'MODE 20 STAGE 0 SLOT MCWARN  '] + [''] * (LINES - 4))

    def test_the_frame_is_the_one_each_master_pushes(self):
        source = (ROOT / 'sdk/shinobi/syIntIsr.src').read_text()
        for master, start in (('syIntMasterISR', '_syIntMasterISR'), ('syIntMasterISR_2', '?nested_entry')):
            words = pushes(source, start)
            self.assertEqual(len(words), self.frame_words, master)
            self.assertEqual(floats(source, start), SIZES['CRASH_FLOAT_WORDS'], master)
            frame = (ctypes.c_uint32 * len(words))(*(0x1000 * (i + 1) for i in range(len(words))))
            bank = (ctypes.c_uint32 * 8)(*(0xbbbb0000 + i for i in range(8)))
            at = {name: frame[i] for i, name in enumerate(words)}
            for rb in (0, SR_RB):
                with self.subTest(master=master, rb=rb):
                    frame[words.index('SSR')] = rb | 0x1f0
                    stop = Stop()
                    self.library.crash_read_frame(ctypes.byref(stop), frame, bank)
                    own = [at[f'R{i}'] for i in range(8)] if rb else list(bank)
                    self.assertEqual((stop.pc, stop.sr, stop.pr, list(stop.r), stop.registers),
                                     (at['SPC'], rb | 0x1f0, at['PR'], own + [at[f'R{i}'] for i in range(8, 15)], 1))


class Hang(unittest.TestCase):
    """crash.c's capture of a hang on the host. tests/crash/hang.c includes its text over stand-ins for the control
    registers, for what syIntMasterISR_2 keeps and for main memory. Each test lays out the code a vblank stopped, runs
    the capture, and reads the screen."""
    # where the stopped code's stack pointer is, where the master's stack ends, and a handler's stack pointer on it
    STACK, MASTER_END, HANDLER_STACK = 0x8c27f000, 0x8c2e8000, 0x8c2e7e00
    # the stopped code's PC, SR and PR, and its callers by their offsets from its stack pointer
    PC, SR, PR = 0x8c500a2e, 0x40000001, 0x8c0123ab
    CALLERS = ((0x000, 0x8c012344), (0x00c, 0x8c50001c), (0x028, 0x0c9c0100))
    # what an interrupt leaves in SGR, SPC and SSR where it stops a handler of the vblank chain, which runs in
    # bank 1 under mask 6, and that handler's callers
    LEFT = {'sgr': HANDLER_STACK, 'spc': 0x8c10f9a2, 'ssr': 0x60000060}
    HANDLER_CALLERS = ((0x000, 0x8c10f9b4), (0x008, 0x8c14d22a))
    WHERE = ['PC 8c500a2e PR 8c0123ab SR 40000001', 'SP 8c27f000', 'MODE 0 STAGE 0 SLOT ?',
             'R0 00000000 01010101 02020202 03030303', 'R4 04040404 05050505 06060606 07070707',
             'R8 08080808 09090909 0a0a0a0a 0b0b0b0b', 'R12 0c0c0c0c 0d0d0d0d 0e0e0e0e',
             '000 8c012344  00c 8c50001c  028 0c9c0100']
    # the name line of a report whose vblank stopped another event's handler
    UNDER = 'CODE UNDER A HANDLER WHOSE PC IS NOT KNOWN'
    # hang.c's hang_switch: a handler asks for a switch of fibers while the capture reads the frame, and the words of
    # the frame that crash_read_frame had read by then
    SWITCH, READ_PC, READ_SR = 1, 2, 4

    @classmethod
    def setUpClass(cls):
        scratch = tempfile.TemporaryDirectory()
        cls.addClassCleanup(scratch.cleanup)
        library = Path(scratch.name) / 'hang.so'
        # a Uint32 keeps the low 32 bits of a host's pointer, where the SH-4's address fills it. make dc compiles
        # crash.c without strict aliasing too
        compile_host(library, *HOST_FLAGS, '-fno-strict-aliasing', '-Wno-pointer-to-int-cast', '-I.', '-Isrc', HANG,
                     REPORT, shared=True)
        cls.library = ctypes.CDLL(str(library))
        cls.screen = (ctypes.c_char * (COLUMNS + 1) * LINES).in_dll(cls.library, 'crash_screen')
        cls.memory = (ctypes.c_uint32 * (MEMORY_SIZE // 4)).in_dll(cls.library, 'crash_memory')
        cls.code = (ctypes.c_uint16 * (MEMORY_SIZE // 2)).from_buffer(cls.memory)
        # the frame's registers from its lowest address, as syIntMasterISR_2 pushes them
        cls.frame = pushes((ROOT / 'sdk/shinobi/syIntIsr.src').read_text(), '?nested_entry')

    def setUp(self):
        """The machine as a vblank that stopped the code alone leaves it, before any later interrupt: the master's
        outermost run holds the code's frame and stack pointer, bank 0 its R0 to R7, and SGR, SPC and SSR its stack
        pointer, PC and SR. The frame's R0 to R7 are bank 1's, which aren't the code's."""
        ctypes.memset(self.memory, 0, ctypes.sizeof(self.memory))
        self.set(ctypes.c_void_p, syIntIntStack=self.MASTER_END, syIntLastStack=self.STACK)
        self.set(ctypes.c_int32, syIntNestCount=1, hang_switch=0)
        self.set(ctypes.c_uint32, isr_pc=0, isr_sr=0, crash_sgr=self.STACK, crash_spc=self.PC, crash_ssr=self.SR,
                 **{f'crash_r{n}_bank': 0x01010101 * n for n in range(8)})
        self.put_frame(SPC=self.PC, SSR=self.SR, PR=self.PR, **{f'R{n}': 0xb1b1b100 + n for n in range(8)},
                       **{f'R{n}': 0x01010101 * n for n in range(8, 15)})
        self.put_callers(self.STACK, self.CALLERS)
        self.put_callers(self.HANDLER_STACK, self.HANDLER_CALLERS)

    def set(self, kind, **values):
        """Set each of hang.c's variables of ctypes type kind that values names."""
        for name, value in values.items():
            kind.in_dll(self.library, name).value = value

    def put_frame(self, **registers):
        """Write registers into the frame that ends at MASTER_END, the outermost run's."""
        base = self.MASTER_END - 4 * len(self.frame)
        for name, value in registers.items():
            self.memory[(base - MEMORY) // 4 + self.frame.index(name)] = value

    def frame_word(self, name):
        """The word of the outermost run's frame that holds the register name."""
        return self.memory[(self.MASTER_END - 4 * len(self.frame) - MEMORY) // 4 + self.frame.index(name)]

    def put_callers(self, stack, callers):
        """Write each of callers at its offset from stack, and the JSR @R0 it returns from, 4 bytes before it in
        either view of main memory."""
        for offset, address in callers:
            self.memory[(stack + offset - MEMORY) // 4] = address
            self.code[((address & 0x1fffffff) - (MEMORY & 0x1fffffff) - 4) // 2] = 0x400b

    def hang(self):
        """The screen's lines after the capture of a hang 1,800 vblanks long."""
        self.library.crash_capture_hang(ctypes.c_uint32(1800))
        return [bytes(line).split(b'\0')[0].decode() for line in self.screen]

    def test_a_hang_reports_the_stopped_codes_stack_and_callers_whatever_a_later_interrupt_left(self):
        # an interrupt that came and went before the watch ran leaves SGR, SPC and SSR at the handler it stopped
        for later, registers in (('none', {}), ('came and went', self.LEFT)):
            with self.subTest(later=later):
                self.set(ctypes.c_uint32, **{f'crash_{name}': value for name, value in registers.items()})
                self.assertEqual(self.hang(), ['HANG 00000708'] + self.WHERE + [''] * (LINES - 9))

    def test_a_vblank_that_stopped_a_handler_reports_the_code_under_the_handlers(self):
        # SGR, SPC and SSR are then the handler's at best, and the outermost run still holds the code's
        self.set(ctypes.c_uint32, **{f'crash_{name}': value for name, value in self.LEFT.items()})
        for count in (2, 3):
            with self.subTest(count=count):
                self.set(ctypes.c_int32, syIntNestCount=count)
                self.assertEqual(self.hang(), ['HANG 00000708', self.UNDER] + self.WHERE + [''] * (LINES - 10))

    def test_the_longest_report_of_a_hang_fills_the_screen_and_no_more(self):
        # the name line of a vblank that stopped a handler, and the most callers
        self.put_callers(self.STACK, [(4 * i, 0x8c010000 + 4 * i) for i in range(CALLERS)])
        self.set(ctypes.c_int32, syIntNestCount=2)
        report = ['HANG 00000708', self.UNDER] + self.WHERE[:-1] + [
            '  '.join(f'{4 * i:03x} {0x8c010000 + 4 * i:08x}' for i in range(first, first + 3))
            for first in range(0, CALLERS, 3)]
        self.assertEqual(len(report), LINES)
        self.assertEqual(self.hang(), report)

    def test_a_fiber_switch_gives_the_pc_and_sr_it_kept(self):
        # the frame as a switch leaves it, which a vblank function of the middleware's asks for before the watch
        # runs, whether or not the vblank stopped a handler
        start = ctypes.cast(self.library.syFbrSwitchToFiber_ISR_START, ctypes.c_void_p).value & 0xffffffff
        self.put_frame(SPC=start, SSR=self.SR | SR_BL)
        self.set(ctypes.c_uint32, isr_pc=self.PC, isr_sr=self.SR)
        for count, name in ((1, []), (2, [self.UNDER])):
            with self.subTest(count=count):
                self.set(ctypes.c_int32, syIntNestCount=count)
                self.assertEqual(self.hang(), ['HANG 00000708'] + name + self.WHERE + [''] * (LINES - 9 - len(name)))

    def test_a_fiber_switch_asked_for_while_the_capture_reads_the_frame_gives_the_stopped_codes_pc_and_sr(self):
        # a handler of an interrupt that the capture doesn't mask asks for the switch. crash_read_frame had read
        # neither, either or both of the frame's PC and SR by then
        start = ctypes.cast(self.library.syFbrSwitchToFiber_ISR_START, ctypes.c_void_p).value & 0xffffffff
        for read in (0, self.READ_PC, self.READ_SR, self.READ_PC | self.READ_SR):
            with self.subTest(read=read):
                self.put_frame(SPC=self.PC, SSR=self.SR)
                self.set(ctypes.c_uint32, isr_pc=0, isr_sr=0)
                self.set(ctypes.c_int32, hang_switch=self.SWITCH | read)
                self.assertEqual(self.hang(), ['HANG 00000708'] + self.WHERE + [''] * (LINES - 9))
                # the switch itself, as the capture leaves it for the interrupt's return
                self.assertEqual(self.frame_word('SPC'), start)
                self.assertEqual(self.frame_word('SSR'), self.SR | SR_BL)

    def test_a_hang_with_no_frame_to_read_says_its_pc_sr_and_sp_are_not_known(self):
        # the masters have no stack of their own, or have one with no run under way. the screen before is a full one
        self.set(ctypes.c_uint32, **{f'crash_{name}': value for name, value in self.LEFT.items()})
        for case, stack in (('no stack', None), ('no run', self.MASTER_END)):
            with self.subTest(case=case):
                self.set(ctypes.c_void_p, syIntIntStack=self.MASTER_END)
                self.set(ctypes.c_int32, syIntNestCount=1)
                self.hang()
                self.set(ctypes.c_void_p, syIntIntStack=stack)
                self.set(ctypes.c_int32, syIntNestCount=0)
                self.assertEqual(self.hang(), ['HANG 00000708', 'PC SR AND SP NOT KNOWN', 'MODE 0 STAGE 0 SLOT ?']
                                 + [''] * (LINES - 3))

    def test_every_character_a_report_draws_has_a_glyph(self):
        glyph = self.library.hang_glyph
        glyph.argtypes, glyph.restype = [ctypes.c_char], ctypes.c_uint32
        drawn = set()

        def read():
            drawn.update(*(bytes(line).split(b'\0')[0].decode() for line in self.screen))

        # a crash with callers under an overlay, a trap under no overlay, and a stop the game names
        crash = stopped(callers=2, mode=-1, slot=b'EMBLEM_G')
        crash.caller_offset[1], crash.caller[0], crash.caller[1] = 0x7fc, 0x8c012344, 0x0c9c0100
        named = stopped(what=b'read runs on', number=0x89abcdef, name=b'the quick brown fox jumps over a lazy dog',
                        code=0, registers=0, callers=-1, slot=b'STG13   ')
        for stop in (crash, stopped(code=0x160, address=0, slot=b''), named, stopped(what=b'PVM', name=b'al_ws_08.pvm')):
            self.library.crash_show(ctypes.byref(stop))
            read()
        # a report that faulted, a hang under a handler, and a hang with no frame to read
        self.library.crash_show_fault(ctypes.byref(crash), ctypes.c_uint32(0x0e0), ctypes.c_uint32(0x8c010abc))
        read()
        self.set(ctypes.c_int32, syIntNestCount=2)
        drawn.update(*self.hang())
        self.set(ctypes.c_int32, syIntNestCount=0)
        drawn.update(*self.hang())
        # the screens held every digit and letter, and each mark a line can hold: SLOT's ? among them
        self.assertEqual(''.join(sorted(drawn)), ' -.' + string.digits + '?' + string.ascii_uppercase + '_abcdef')
        glyphs = {character: glyph(character.encode()) for character in drawn}
        self.assertEqual([character for character, dots in glyphs.items() if not dots], [' '])
        # a glyph is five rows of three dots, and the ? reads as no other character
        self.assertEqual([character for character, dots in glyphs.items() if dots >> 15], [])
        self.assertEqual([character for character, dots in glyphs.items() if dots == glyphs['?']], ['?'])

    def test_the_master_keeps_the_stopped_stack_pointer_on_its_outermost_run_alone(self):
        self.assertEqual(kept_stacks((ROOT / 'sdk/shinobi/syIntIsr.src').read_text()), [0])


class Watch(unittest.TestCase):
    def test_vblank_cadence_and_vector_ownership(self):
        with tempfile.TemporaryDirectory() as scratch:
            exe = Path(scratch) / 'watch'
            # a vector word has the SH-4's 32 bits, so it keeps the low 32 of a host's handler address
            compile_host(exe, '-std=c99', '-Wall', '-Wextra', '-Werror', '-Wno-pointer-to-int-cast', '-I.',
                         'tests/crash/watch.c')
            subprocess.run([str(exe)], check=True)


class Console(unittest.TestCase):
    def test_gcc_compiles_the_screen_as_make_dc_does_and_says_nothing(self):
        gcc = SH_ELF / 'bin/sh-elf-gcc'
        if not gcc.is_file() or not (SH4ZAM / 'include/sh4zam').is_dir():
            raise unittest.SkipTest(f'no SH-4 GCC under {SH_ELF}, or no SH4ZAM at {SH4ZAM}: SH_ELF names a '
                                    'toolchain, and SH4ZAM its headers')
        flags = gcc_flags(SH_ELF)
        with tempfile.TemporaryDirectory() as scratch:
            for level in LEVELS:
                for source in CONSOLE:
                    with self.subTest(level=level, source=source):
                        built = subprocess.run([str(gcc), *flags, level, '-isystem', str(SH4ZAM / 'include'),
                                                *INCLUDES, '-c', '-o', str(Path(scratch) / 'crash.o'), source],
                                               cwd=ROOT, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True)
                        self.assertEqual((built.returncode, built.stdout), (0, ''))


if __name__ == '__main__':
    unittest.main()
