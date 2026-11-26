"""The portable camset has a private range for each of Chao's four cameras."""

import struct
import subprocess
import tempfile
import unittest
from pathlib import Path

from support import ROOT, SH_ELF, SH4ZAM


class CameraZoneBounds(unittest.TestCase):
    def test_four_camera_stage_uses_two_empty_private_ranges(self):
        gcc = SH_ELF / 'bin/sh-elf-gcc'
        if not gcc.is_file() or not (SH4ZAM / 'include/sh4zam').is_dir():
            self.skipTest('need SH-4 GCC and SH4ZAM headers')
        with tempfile.TemporaryDirectory() as temp:
            work = Path(temp)
            subprocess.run([gcc, '-c', '-Os', '-ml', '-m4-single-only',
                            '-ffreestanding', '-fdata-sections', '-DNONMATCHING', '-DDREAMCAST',
                            '-I', ROOT / 'src', '-I', ROOT / 'sdk/include',
                            '-isystem', SH4ZAM / 'include', ROOT / 'src/camera/camset.c',
                            '-o', 'camset.o'], cwd=work, check=True)
            subprocess.run([SH_ELF / 'bin/sh-elf-objcopy', '--dump-section',
                            '.data.camera_zone_tables=bounds.bin', 'camset.o'], cwd=work, check=True)
            data = (work / 'bounds.bin').read_bytes()
        bounds = struct.unpack('<6I', data)
        self.assertEqual(list(zip(bounds, bounds[1:])), [
            (0x8cbe0000, 0x8cbf0000),  # shared
            (0x8cbf0000, 0x8cbf4000),  # player 0
            (0x8cbf4000, 0x8cbf8000),  # player 1
            (0x8cbf8000, 0x8cbf8000),  # camera 2
            (0x8cbf8000, 0x8cbf8000),  # camera 3
        ])
