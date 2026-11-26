"""The resident startup describes Sofdec's actual code after the GNU LTO link."""

from pathlib import Path
import subprocess
import tempfile
import unittest

from support import CC, ROOT, SH_ELF, SH4ZAM


class SofdecSections(unittest.TestCase):
    def test_resident_groups_contain_their_compiled_members(self):
        gcc = SH_ELF / 'bin/sh-elf-gcc'
        if not gcc.is_file():
            self.skipTest(f'need SH-4 GCC under {SH_ELF}')
        with tempfile.TemporaryDirectory() as temp:
            work = Path(temp)

            def run(*args):
                return subprocess.check_output(args, cwd=work, text=True)

            shckit = ROOT / 'build/shckit'
            if not shckit.is_file():
                shckit = work / 'shckit'
                run(*CC, '-std=c11', ROOT / 'tools/shckit/main.c', ROOT / 'tools/shckit/shckit.c', '-o', shckit)
            objects = []
            for stem in ('mpv_cmc', 'dct_fz', 'mpv_cdec', 'mpvadec'):
                obj = stem + '.o'
                run(gcc, '-c', '-Os', '-flto', '-ml', '-m4-single-only', '-ffreestanding',
                    '-DDREAMCAST', '-DNONMATCHING', '-I', ROOT / 'sdk/include',
                    '-isystem', SH4ZAM / 'include',
                    ROOT / 'sdk/sofdec' / (stem + '.c'), '-o', obj)
                objects.append(obj)
            for source in ('sdk/shinobi/strt1.src', 'sdk/sofdec/dctafz.src', 'sdk/sofdec/mpvasec.src'):
                obj = Path(source).stem + '.o'
                run(shckit, 'translate', '-o', 'member.s', ROOT / source)
                run(SH_ELF / 'bin/sh-elf-as', '--isa=sh4', '--little', '--no-pad-sections', 'member.s', '-o', obj)
                objects.append(obj)
            run(gcc, '-E', '-P', '-x', 'c', ROOT / 'config/1st_read/sofdec.ld.S', '-o', 'sofdec.ld')
            run(gcc, '-r', '-nostdlib', '-flto', '-flinker-output=nolto-rel', '-ffunction-sections',
                '-ml', '-m4-single-only', '-Wl,-T,sofdec.ld', *objects, '-o', 'placed.o')
            output = run(SH_ELF / 'bin/sh-elf-objdump', '-t', 'placed.o')
            symbols = {fields[-1]: (int(fields[0], 16), fields[-3]) for line in output.splitlines()
                       if len(fields := line.split()) >= 5 and fields[0].isalnum()}
            for ordinary in ('_DCT_FzInit', '_MPVCMC_Init', '_MPV_DecodePicAtr'):
                self.assertTrue(symbols[ordinary][1].startswith('.text.'), ordinary)
                self.assertNotEqual(symbols[ordinary][1], '.text.sofdec', ordinary)
            previous = 0
            for i in range(16):
                with self.subTest(group=i):
                    group = f'PSGSFD{i:02d}'
                    start, end, size = [symbols[f'__{field}of_{group}'] for field in ('start', 'end', 'size')]
                    self.assertEqual((start[1], end[1], size[1]), ('.text.sofdec', '.text.sofdec', '*ABS*'))
                    self.assertEqual(start[0], (previous + 3) & ~3)
                    self.assertEqual(end[0] - start[0], size[0])
                    if i == 0:
                        for name in ('_DCT_FzTrans6Blk', '_MPVCDEC_IntraBlocks', '_MPVCMC_IntraRt', '_DCTAFZ_SparseDc'):
                            self.assertEqual(symbols[name][1], '.text.sofdec', name)
                            self.assertTrue(start[0] <= symbols[name][0] < end[0], name)
                    elif i == 1:
                        self.assertEqual(size[0], 1582)
                    elif i == 2:
                        for name in ('_mpvdec_DecIpicMb', '_mpvdec_DecPpicMb', '_mpvdec_DecBpicMb'):
                            self.assertEqual(symbols[name][1], '.text.sofdec', name)
                            self.assertTrue(start[0] <= symbols[name][0] < end[0], name)
                    else:
                        self.assertEqual(size[0], 0)
                    previous = end[0]
