"""The resident startup's pointer words follow their live SH4 storage."""
import os
from pathlib import Path
import subprocess
import tempfile
import unittest


ROOT = Path(__file__).resolve().parents[1]
TOOLS = Path(os.environ.get('SH_ELF', ROOT / 'tools/sh-elf')).resolve() / 'bin'
CONFIG = ROOT / 'config/1st_read'


class MainStartup(unittest.TestCase):
    def test_live_bounds_at_two_load_addresses(self):
        compiler = TOOLS / 'sh-elf-gcc'
        if not compiler.is_file():
            self.skipTest(f'need SH-4 GCC under {TOOLS}')
        with tempfile.TemporaryDirectory() as directory:
            work = Path(directory)

            def run(*args):
                return subprocess.check_output(args, cwd=work, text=True, stderr=subprocess.STDOUT)

            def assemble(name, source):
                (work / (name + '.s')).write_text(source)
                run(TOOLS / 'sh-elf-as', '-little', '-isa=sh4', '-o', name + '.o', name + '.s')

            def compile_member(name, source):
                (work / (name + '.c')).write_text(source)
                run(compiler, '-ml', '-m4-single-only', '-fdata-sections', '-fno-common',
                    '-ffreestanding', '-c', name + '.c', '-o', name + '.o')

            assemble('strt1', '.section DSGLH,"aw"\n.global _START_SYS\n_START_SYS:\n.space 48\n'
                     '.section "$G0","aw",@nobits\n.space 8\n'
                     '.section DSG_INIT_,"aw"\n.long _sdk_data\n'
                     '.section DSG_END_,"aw"\n.long _sdk_data\n'
                     '.section D_INIT_,"aw"\n.long _game_data\n'
                     '.section D_END_,"aw"\n.long _game_data\n')
            run(compiler, '-ml', '-m4-single-only', '-std=c89', '-pedantic-errors', '-Os',
                '-ffreestanding', '-DNONMATCHING', '-I', ROOT / 'sdk/include', '-c',
                ROOT / 'sdk/shinobi/strt1_records.c',
                '-o', 'strt1_records.o')
            compile_member('game', 'int game_data = 7, game_bss; char matrix_stack[64];')
            compile_member('1st_read-sdk', 'int sdk_data = 11, sdk_bss; char ovg_libhdl[32] = {1};')
            compile_member('1st_read-cache', 'int cache_data = 23, cache_bss;')
            ld = TOOLS / 'sh-elf-ld'
            run(ld, '-EL', '-r', '-T', CONFIG / 'storage.ld', '-o', 'storage.o',
                'strt1.o', 'strt1_records.o', 'game.o', '1st_read-sdk.o', '1st_read-cache.o')
            run(ld, '-EL', '-r', '-T', CONFIG / 'startup.ld', '-o', 'layout.o', 'storage.o')

            # Keep the classed sections together when giving the image an address.
            (work / 'placed.ld').write_text(
                'SECTIONS { . = base; .header : { *(.text.resident_entry) } '
                '.data : { *(.data.resident .data.resident_sdk) } '
                '.bss (NOLOAD) : { *(.bss.resident) } }')
            previous = {}
            for base in (0x100000, 0x300000):
                run(ld, '-EL', '--defsym', f'base={base}', '-T', 'placed.ld', '-o', 'placed.elf', 'layout.o')
                symbols = {v[0]: int(v[2], 16) for line in run(TOOLS / 'sh-elf-nm', '-P', 'placed.elf').splitlines()
                           if len(v := line.split()) >= 3}
                starts = {n[10:]: a for n, a in symbols.items() if n.startswith('__startof_')}
                ends = {n[8:]: a for n, a in symbols.items() if n.startswith('__endof_')}
                self.assertEqual(len(starts) + len(ends), 33)
                self.assertEqual(starts['DSGLH'], base)
                self.assertEqual(symbols['_ovg_libhdl'], base + 64)
                self.assertEqual(ends['DSGLH'], starts['DSGLE'])
                self.assertEqual(ends['DSGLE'], base + 128)
                self.assertEqual(starts['D'], symbols['_game_data'])
                self.assertEqual(starts['DSG'], symbols['_sdk_data'])
                self.assertEqual(starts['DSG32'], symbols['_cache_data'])
                for section in ('D_INIT_', 'D_END_', 'DSG_INIT_', 'DSG_END_'):
                    self.assertEqual(ends[section] - starts[section], 4, section)
                for section in ('R', 'R32', 'RSG', 'RSG32'):
                    self.assertEqual(starts[section], ends[section], section)
                self.assertEqual(ends['R32'], starts['R'])
                self.assertEqual(ends['RSG32'], starts['RSG'])
                self.assertEqual(starts['B'] - starts['$G0'], 32)
                for section, symbol, size in (('B', '_game_bss', 4), ('B32', '_matrix_stack', 64),
                                              ('BSG', '_sdk_bss', 4), ('BSG32', '_cache_bss', 4)):
                    self.assertEqual((starts[section], ends[section]), (symbols[symbol], symbols[symbol] + size))
                relative = {n: a - base for n, a in symbols.items() if n.startswith(('__startof_', '__endof_'))}
                if previous:
                    self.assertEqual(relative, previous)
                previous = relative
            run(ld, '-EL', '-T', CONFIG / 'image.ld', '-o', 'resident.elf', 'layout.o')
            run(TOOLS / 'sh-elf-objcopy', '-O', 'binary', '-j', '.resident_header',
                'resident.elf', 'resident_header.bin')
            header_bytes = (work / 'resident_header.bin').read_bytes()
            self.assertEqual(header_bytes[48:64], b'Lib Handle Start')
            self.assertEqual(header_bytes[96:], bytes(16) + b'Lib Handle End  ')
            header = (work / 'resident.elf').read_bytes()[:28]
            self.assertEqual(int.from_bytes(header[16:18], 'little'), 2)  # ET_EXEC
            self.assertEqual(int.from_bytes(header[24:28], 'little'), 0x8c010000)
            assemble('overlap', '.section .bss.resident,"aw",@nobits\n.space 0x300000\n')
            with self.assertRaises(subprocess.CalledProcessError) as overlap:
                run(ld, '-EL', '-T', CONFIG / 'image.ld', '-o', 'overlap.elf', 'layout.o', 'overlap.o')
            self.assertIn('resident overlaps the loader workspace', overlap.exception.output)
            assemble('orphan', '.section abandoned,"a"\n.long 1\n')
            with self.assertRaises(subprocess.CalledProcessError) as orphan:
                run(ld, '-EL', '-T', CONFIG / 'image.ld', '-o', 'orphan.elf', 'layout.o', 'orphan.o')
            self.assertIn('unplaced resident storage', orphan.exception.output)
            assemble('copy', '.section R,"aw",@nobits\n.space 4\n')
            with self.assertRaises(subprocess.CalledProcessError) as failure:
                run(ld, '-EL', '-r', '-T', CONFIG / 'startup.ld', '-o', 'bad.o', 'storage.o', 'copy.o')
            self.assertIn('Resident initialized data must run in place', failure.exception.output)
