"""The drawing kernels check the SDK records using the release SH-4 ABI."""
from pathlib import Path
import re
import subprocess
import tempfile
import unittest

from support import ROOT, SH4ZAM, SH_ELF, gcc_flags


class KernelLayout(unittest.TestCase):
    def test_strip_flag_offset_is_checked_at_the_drawer(self):
        gcc = SH_ELF / 'bin/sh-elf-gcc'
        if not gcc.is_file() or not (SH4ZAM / 'include/sh4zam').is_dir():
            self.skipTest('requires the SH-4 GCC and SH4ZAM headers')

        with tempfile.TemporaryDirectory() as directory:
            sdk = Path(directory)
            flags = [gcc, '-fsyntax-only', '-Os', '-I', sdk, '-Isdk/include',
                     '-isystem', SH4ZAM / 'include', *gcc_flags(SH_ELF)]
            drawer = 'engine/dc/direct_points.c'
            subprocess.run([*flags, drawer], cwd=ROOT, check=True)
            # Swap the two last words of the actual record. Its size and all
            # XYZ coordinates still agree, but the clipper reads the other word.
            header = (ROOT / 'sdk/include/ninja.h').read_text()
            changed, count = re.subn(r'(Uint32\s+)uv(\s*;\s*Uint32\s+)param(\s*;\s*}\s*NJS_DIRECT_VERTEX;)',
                                     r'\1param\2uv\3', header)
            self.assertEqual(count, 1)
            (sdk / 'ninja.h').write_text(changed)
            probe = '#include <ninja.h>\n#include <stddef.h>\n' \
                    'typedef char same_size[sizeof(NJS_DIRECT_VERTEX) == 32 && ' \
                    'offsetof(NJS_DIRECT_VERTEX, param) == 24 ? 1 : -1];\n'
            subprocess.run([*flags, '-x', 'c', '-'], cwd=ROOT, input=probe, text=True, check=True)
            compiled = subprocess.run([*flags, drawer], cwd=ROOT, capture_output=True, text=True)
            self.assertNotEqual(compiled.returncode, 0)
            self.assertIn('direct_vertex_layout', compiled.stderr)
