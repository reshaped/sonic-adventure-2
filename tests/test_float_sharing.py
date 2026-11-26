"""Bit results from the original compilers and the terminal GNU shared-product probes."""
from pathlib import Path
import tempfile
import unittest

import sh4
from support import ROOT, SH_ELF
from test_float_fuse import wrapped
from test_float_terminal import link_probe, symbols

FIXTURES = ROOT / 'tests/floats'
# (1+2^-23)*(1-2^-23) is exactly 1-2^-46. With FPSCR.RM=1,
# FMAC with -1 returns -2^-46. A prior FMUL truncates the product to
# 1-2^-24, and its FADD with -1 returns -2^-24. A changed sign reverses both.
FUSED, SPLIT = 0xa8800000, 0xb3800000
SIGN_BIT = 0x80000000
# Each 'changed' call negates its observed factor.
CHANGED = (FUSED, FUSED ^ SIGN_BIT)
ROUNDED = (SPLIT, SPLIT)


def model_evidence(final):
    """Execute the global-call and pure-local cases in the linked SH-4 instructions."""
    dump = SH_ELF / 'bin/sh-elf-objdump'
    functions = sh4.functions(dump, final)
    addresses = symbols(SH_ELF / 'bin/sh-elf-nm', final)
    machine = sh4.Machine(None)
    machine.memory.update(sh4.image(dump, final))
    machine.r[15] = 0x0cfe0000
    out = 0x0cfd0000
    machine.store(addresses['shared_factor'], 0x3f800001)
    machine.store(addresses['shared_calls'], 0)
    machine.r[4] = out
    machine.front[4:7] = [0x3f7ffffe, 0xbf800000, 0x3f800000]
    machine.call(functions['shared_global_call'], linked=[functions['shared_change']])
    global_pair = (machine.load(out), machine.load(out + 4))
    effects = (machine.load(addresses['shared_factor']), machine.load(addresses['shared_calls']))
    machine.r[4] = out
    machine.front[4:8] = [0x3f800001, 0x3f7ffffe, 0xbf800000, 0xbf800000]
    machine.call(functions['shared_parameters'])
    return global_pair, effects, (machine.load(out), machine.load(out + 4))


@unittest.skipUnless((SH_ELF / 'bin/sh-elf-gcc').is_file(), 'requires the SH-4 release toolchain')
class TerminalSharedProducts(unittest.TestCase):
    def test_observable_results_and_calls(self):
        with tempfile.TemporaryDirectory() as temp:
            work = Path(temp)
            _, final = link_probe(work, 'normal', sources=(FIXTURES / 'shared.c', FIXTURES / 'shared_callback.c'),
                                     entry='shared_global_call')
            global_pair, effects, local_pair = model_evidence(final)
            self.assertEqual((global_pair, effects, local_pair), (CHANGED, (0xbf800001, 1), ROUNDED))

    def test_the_previous_global_call_classification_changes_both_words(self):
        # Recreate the old pass's two actual anti-contraction wrappers in this
        # one function, keeping both memory reads, stores and the callback.
        with tempfile.TemporaryDirectory() as temp:
            work = Path(temp)
            source = (FIXTURES / 'shared.c').read_text()
            expr = 'shared_factor * b'
            block = (f'out[0] = {expr} + c;\n    shared_change();\n    out[1] = {expr} + d;')
            self.assertEqual(source.count(block), 1)
            old = work / 'shared.c'
            old.write_text(source.replace(block, block.replace(expr, wrapped(expr))))
            _, final = link_probe(work, 'normal', rewrite=False, sources=(old, FIXTURES / 'shared_callback.c'),
                                     entry='shared_global_call')
            global_pair, effects, _ = model_evidence(final)
            self.assertEqual((global_pair, effects), ((SPLIT, SPLIT ^ SIGN_BIT), (0xbf800001, 1)))
