"""Link actual Lobby code against the garden's resident save flags."""

import subprocess
import sys
import tempfile
import unittest
from pathlib import Path

from support import ROOT, SH_ELF, SH4ZAM
from test_census import elf


class ResidentChaoTest(unittest.TestCase):
    def test_bind_lobby_and_retain_missing_symbols(self):
        gcc, ld, nm = (SH_ELF / 'bin' / ('sh-elf-' + tool) for tool in ('gcc', 'ld', 'nm'))
        if not gcc.is_file() or not (SH4ZAM / 'include/sh4zam').is_dir():
            self.skipTest('need SH-4 GCC and SH4ZAM headers')
        cc = [gcc, '-ml', '-m4-single-only', '-std=gnu17', '-ffreestanding', '-fno-builtin',
              '-ffunction-sections', '-fdata-sections', '-Os', '-DDREAMCAST', '-DNONMATCHING']
        for include in ('sdk/include', 'src', 'modules'):
            cc.extend(['-I', ROOT / include])
        cc.extend(['-isystem', SH4ZAM / 'include'])
        with tempfile.TemporaryDirectory() as temp:
            work = Path(temp)

            def run(command, **options):
                return subprocess.run(command, cwd=ROOT, text=True,
                                      stdout=subprocess.PIPE, stderr=subprocess.STDOUT, **options)

            run(['python3', 'tools/body_data.py', 'chao', 'modules/chao/garden_save.c', work, '--stub'], check=True)
            lobby = 'modules/chaostglobby/lobby.c'
            run(['python3', 'tools/body_data.py', 'chaostglobby', lobby, work, '--stub'], check=True)
            cc.extend(['-I', work / 'modules'])
            for path in ('modules/chao/garden_save.c', 'modules/chao/save_buffers.c', lobby):
                run([*cc, '-c', ROOT / path, '-o', work / (Path(path).stem + '.o')], check=True)
            # ld keeps the real flag predicate and its real save record in the
            # executable. The child must follow their location on either link.
            for address in (0x0c510000, 0x0c512000):
                with self.subTest(address=address):
                    run([ld, '-EL', '--gc-sections', '-e', '_garden_flag_test', f'-Ttext={address:#x}',
                         work / 'garden_save.o', work / 'save_buffers.o', '-o', work / 'world.elf'], check=True)
                    run(['python3', 'tools/chao_area.py', 'chaostglobby', work / 'lobby.o', work / 'world.elf',
                         work / 'bound.o', '--toolchain', SH_ELF], check=True)
                    table = run([nm, '-P', work / 'bound.o'], check=True).stdout.splitlines()
                    self.assertIn(['_garden_flag_test', 'A', f'{address:x}'], [line.split()[:3] for line in table])
                    self.assertTrue(any(line.split()[:2] == ['_garden_messages_load', 'U'] for line in table))
                    run([ld, '-EL', '--gc-sections', '-e', '_lobby_layout', '-Ttext=0x0c9c0000',
                         work / 'bound.o', '-o', work / 'lobby.elf'], check=True)
                    self.assertEqual(run([nm, '-u', work / 'lobby.elf'], check=True).stdout, '')

            # The paired LTO check rejects an incompatible source declaration.
            source = work / 'wrong.c'
            source.write_text('extern void *garden_flag_test(double *);\n'
                              'void *read_mask(double *p) { return garden_flag_test(p); }\n')
            for path, name in [(source, 'wrong'), (ROOT / 'modules/chao/garden_save.c', 'world')]:
                run([*cc, '-flto', '-c', path, '-o', work / (name + '.lto.o')], check=True)
            wrong = run([*cc, '-flto', '-Werror=lto-type-mismatch', '-r', '-flinker-output=nolto-rel', '-nostdlib',
                         work / 'wrong.lto.o', work / 'world.lto.o', '-o', work / 'wrong-link.o'])
            self.assertNotEqual(wrong.returncode, 0)
            self.assertIn('lto-type-mismatch', wrong.stdout)

    def test_fails_naming_a_resident_it_cant_read(self):
        # area.o reads as an object with no symbols, so the resident is the file the tool fails on
        with tempfile.TemporaryDirectory() as temp:
            Path(temp, 'area.o').write_bytes(elf([]))
            failed = subprocess.run([sys.executable, ROOT / 'tools/chao_area.py', '--toolchain', 'tools/sh-elf',
                                     'chaostglobby', 'area.o', 'world.elf', 'out.o'], cwd=temp, capture_output=True,
                                    text=True, check=False)
        self.assertEqual((failed.returncode, failed.stderr), (1, 'chao_area: world.elf: No such file or directory\n'))

    def test_refuses_an_output_that_is_the_resident(self):
        with tempfile.TemporaryDirectory() as temp:
            Path(temp, 'area.o').write_bytes(elf([]))
            resident = Path(temp, 'world.elf')
            resident.write_bytes(b'resident')
            failed = subprocess.run([sys.executable, ROOT / 'tools/chao_area.py', '--toolchain', 'tools/sh-elf',
                                     'chaostglobby', 'area.o', 'world.elf', './world.elf'], cwd=temp,
                                    capture_output=True, text=True, check=False)
            self.assertEqual(resident.read_bytes(), b'resident')
        self.assertEqual((failed.returncode, failed.stderr),
                         (1, 'chao_area: world.elf: would replace the resident it binds against\n'))
