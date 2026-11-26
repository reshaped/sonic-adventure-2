"""Check the network overlay's four BSS boundaries in final SH-4 executables."""

import os
import struct
import subprocess
import tempfile
import unittest
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
SH_ELF = Path(os.environ.get('SH_ELF', ROOT / 'tools/sh-elf')).resolve()


class KindergartenBssTest(unittest.TestCase):
    def test_runtime_addresses_follow_the_game_and_sdk_storage(self):
        gcc = SH_ELF / 'bin/sh-elf-gcc'
        nm = SH_ELF / 'bin/sh-elf-nm'
        objcopy = SH_ELF / 'bin/sh-elf-objcopy'
        if not all(tool.is_file() for tool in (gcc, nm, objcopy)):
            self.skipTest(f'need SH-4 GCC, nm and objcopy under {SH_ELF}')

        with tempfile.TemporaryDirectory() as temp:
            work = Path(temp)
            cc = [gcc, '-ml', '-m4-single-only', '-std=c89', '-DNONMATCHING', '-ffunction-sections',
                  '-fdata-sections', '-I', ROOT / 'sdk/include']
            # The game dictionary flags and the SDK's EEPROM are
            # actual B and BSG inputs in the retail overlay.
            for source, obj in [('sdk/network/ask_dictionary.c', 'game.o'), ('sdk/modem/ee_ram.c', 'chaostgkinder-sdk.o')]:
                subprocess.run([*cc, '-c', ROOT / source, '-o', obj], cwd=work, check=True)
            subprocess.run([*cc, '-nostdlib', '-r', '-Wl,-T,' + str(ROOT / 'config/chaostgkinder/bss.ld'),
                            'game.o', 'chaostgkinder-sdk.o', '-o', 'placed.o'], cwd=work, check=True)

            (work / 'bounds.c').write_text(
                'extern unsigned char kinder_bss_area[], kinder_bss_end[], kinder_sg_bss_area[], kinder_sg_bss_end[];\n'
                'const void *const bounds[] = {kinder_bss_area, kinder_bss_end, kinder_sg_bss_area, kinder_sg_bss_end};\n'
                'const void *const *start(void) { return bounds; }\n')
            for origin in (0x200000, 0x218024):
                with self.subTest(origin=origin):
                    subprocess.run([*cc, '-nostdlib', '-Wl,--gc-sections,-e,_start,-Tbss=' + hex(origin),
                                    'bounds.c', 'placed.o', '-o', 'image.elf'], cwd=work, check=True)
                    subprocess.run([objcopy, '--dump-section', '.rodata=bounds.bin', 'image.elf'],
                                   cwd=work, check=True)
                    game, game_end, sdk, sdk_end = struct.unpack('<4I', (work / 'bounds.bin').read_bytes())
                    rows = subprocess.check_output([nm, '-S', 'image.elf'], cwd=work, text=True).splitlines()
                    objects = {name: (int(at, 16), int(size, 16)) for at, size, _, name in
                               (row.split() for row in rows if len(row.split()) == 4)}
                    self.assertEqual(game, origin)
                    self.assertEqual(game_end, sdk)
                    flags = sorted(objects[name] for name in ('_dic_fd', '_dic_rw', '_gaku_mode', '_suji_mode'))
                    self.assertEqual(flags, [(game + offset, 4) for offset in range(0, 16, 4)])
                    self.assertEqual(game_end, game + 16)
                    self.assertEqual(objects['_RamTable'], (sdk, sdk_end - sdk))
