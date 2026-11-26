"""Run terminal play code with round toward zero and denormals flushed (FPSCR = 0x00040001)."""
from pathlib import Path
import subprocess
import tempfile
import unittest

import floats
import sh4
from support import ROOT, SH_ELF
from test_float_fuse import AGGRESSIVE, GAME, SYSTEM
from test_float_pass import variable

PROBES = ROOT / 'tests/float_rules'
# These values are IEEE binary32 words. The product in the first two rows is
# exactly 1 - 2^-46. Rounding the product toward zero gives 1 - 2^-24.
VECTORS = (
    ('add_product', (0x3f800001, 0x3f7ffffe, 0xbf800000), 0xa8800000),
    ('subtract_product', (0x3f800001, 0x3f7ffffe, 0x3f800000), 0x33800000),
    ('add_product', (0x3f800000, 0x3f800000, 0x33c00000), 0x3f800000),
    ('add_product', (0x80000000, 0x40000000, 0x80000000), 0x80000000),
    ('add_product', (0x80000000, 0x40000000, 0x00000000), 0),
    # Changing the expression's types, operand order or sign can change which
    # multiply rounds independently. Use the same near-cancelling factors.
    ('fused_typedef', (0xbf800001, 0x3f7ffffe), 0x28800000),
    ('fused_cast', (0x3f800001, 0x3f7ffffe, 0xbf800000), 0xa8800000),
    ('sum_products', (0x3f800001, 0x3f7ffffe, 0xbf800000, 0x3f800000), 0xa8800000),
    ('ordered_parentheses', (0xbf800000, 0, 0x3f800001, 0x3f7ffffe), 0xa8800000),
    ('order_names', (0x3f800000, 0, 0x3f800001, 0x3f7ffffe), 0xb3800000),
    ('negative_product', (0x3f800001, 0x3f7ffffe, 0x3f800000), 0x33800000),
    ('subtract_products', (0x3f800001, 0x3f7ffffe, 0x3f800000), 0xb3800000),
    ('across_statement', (0x3f800001, 0x3f7ffffe, 0xbf800000), 0xb3800000),
)
# With RM = 1, a direct division of 1 by 3 rounds down. Under -aggressive=2
# SHC multiplies by the nearest compile-time reciprocal, 0x3eaaaaab.
RECIPROCAL = {'normal': 0x3eaaaaaa, 'aggressive': 0x3eaaaaab}


def symbols(nm, path):
    listing = subprocess.check_output([nm, '-P', '-g', '--defined-only', path], text=True)
    return {fields[0][1:]: int(fields[2], 16) for row in listing.splitlines()
            if len(fields := row.split()) >= 3 and fields[0].startswith('_')}


def link_probe(work, profile, *, rewrite=True, sources=(PROBES / 'expressions.c', PROBES / 'callback.c'),
               entry='add_product'):
    gcc = SH_ELF / 'bin/sh-elf-gcc'
    flags = [*variable('GCCFLAGS'), '-Os', *variable('GCC_LTO')]
    modules = []
    for index, source in enumerate(sources):
        name = source.stem
        opts = flags + (variable('GCC_FUSING') if index == 0 else [])
        pp = work / (name + '.i')
        subprocess.run([gcc, *opts, '-E', source, '-o', pp], cwd=ROOT, check=True)
        if index == 0 and rewrite:
            text, problems = floats.translate(pp.read_text(), str(pp), AGGRESSIVE if profile == 'aggressive' else GAME, SYSTEM)
            if problems:
                raise ValueError(problems)
            pp.write_text(text)
        module = work / (name + '.o')
        subprocess.run([gcc, *opts, '-c', pp, '-o', module], cwd=ROOT, check=True)
        modules.append(module)
    terminal = work / 'terminal.o'
    subprocess.run([gcc, *variable('GCC_LTO_LINK'), '-r', '-nostdlib', '-flinker-output=nolto-rel',
                    '-o', terminal, *modules], cwd=ROOT, check=True)
    final = work / 'overlay.elf'
    subprocess.run([SH_ELF / 'bin/sh-elf-ld', '-EL', '--no-undefined', '--defsym', 'overlay_base=0x0c500000',
                    '-T', ROOT / 'config/overlay.ld', '-e', '_' + entry, '-o', final, terminal], check=True)
    return terminal, final


def model_words(elf, vectors):
    objdump = SH_ELF / 'bin/sh-elf-objdump'
    code = sh4.functions(objdump, elf)
    addresses = symbols(SH_ELF / 'bin/sh-elf-nm', elf)
    model = sh4.Machine(None)
    model.memory.update(sh4.image(objdump, elf))
    model.r[15] = 0x0cfe0000
    results = []
    for function, args, _ in vectors:
        model.front[4:4 + len(args)] = args
        model.call(code[function])
        results.append(model.front[0])
    for function in ('assign_pointer', 'assign_name'):
        model.store(addresses['probe_global'], 0x3f800000)
        model.store(addresses['probe_next'], 0x41f00000)
        model.store(addresses['probe_pointer'], addresses['probe_global'])
        model.front[4] = 0x40400000
        model.call(code[function], linked=[code['probe_change']])
        results.extend([model.load(addresses['probe_global']), model.load(addresses['probe_next']),
                        int(model.load(addresses['probe_pointer']) == addresses['probe_next'])])
    return results


@unittest.skipUnless((SH_ELF / 'bin/sh-elf-gcc').is_file(), 'requires the SH-4 release toolchain')
class TerminalFloatRules(unittest.TestCase):
    def test_rounding_and_compound_assignment_at_the_final_link(self):
        for profile, reciprocal in RECIPROCAL.items():
            with self.subTest(profile=profile), tempfile.TemporaryDirectory() as temp:
                work = Path(temp)
                _, final = link_probe(work, profile)
                vectors = (*VECTORS, ('reciprocal', (0x3f800000,), reciprocal))
                # In both += cases the callback writes 4 before the addend is read.
                # In *p the old pointer must have been saved before that call.
                expected = [bits for _, _, bits in vectors] + [0x41200000, 0x41f00000, 1] * 2
                self.assertEqual(model_words(final, vectors), expected)

    def test_untranslated_code_changes_the_observed_results(self):
        with tempfile.TemporaryDirectory() as temp:
            work = Path(temp)
            _, final = link_probe(work, 'aggressive', rewrite=False)
            vectors = (*VECTORS, ('reciprocal', (0x3f800000,), RECIPROCAL['aggressive']))
            words = model_words(final, vectors)
            # The unprotected subtraction contracts. Division remains an FDIV.
            self.assertEqual(words[1], 0x28800000)
            self.assertEqual(words[len(VECTORS)], RECIPROCAL['normal'])
            # The original destination stays 4 and the moved pointer receives 36.
            self.assertEqual(words[len(vectors):len(vectors) + 3], [0x40800000, 0x42100000, 1])
