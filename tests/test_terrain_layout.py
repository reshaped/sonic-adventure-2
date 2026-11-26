"""Terrain workspace addresses stay attached to the last occupied overlay section."""
import os
from pathlib import Path
import struct
import subprocess
import tempfile
import unittest

ROOT = Path(__file__).resolve().parents[1]
SCRIPT = ROOT / 'config/terrain.ld'
TOOLS = Path(os.environ.get('SH_ELF', ROOT / 'tools/sh-elf')).resolve() / 'bin'


class TerrainLayoutTest(unittest.TestCase):
    @unittest.skipUnless((TOOLS / 'sh-elf-as').is_file(), 'requires the SH-4 toolchain')
    def test_first_free_address_moves_with_storage(self):
        def run(tool, *args):
            return subprocess.check_output([TOOLS / ('sh-elf-' + tool), *args], cwd=work, text=True)

        with tempfile.TemporaryDirectory() as directory:
            work = Path(directory)
            for section, bss in [('.data', ''), ('.bss', '.section .bss.state,"aw",@nobits\n.space 20\n')]:
                with self.subTest(section=section):
                    (work / 'overlay.s').write_text(
                        '.section .head,"aw",@progbits\n.long _end\n'
                        '.section .text.step,"ax",@progbits\nrts\nnop\n'
                        '.section .data.land,"aw",@progbits\n.long 1,2,3\n' + bss)
                    run('as', '--little', '--isa=sh4', '-o', 'overlay.o', 'overlay.s')
                    run('ld', '-EL', '-r', '-T', SCRIPT, '-o', 'terrain.o', 'overlay.o')
                    symbol = next(line.split() for line in run('objdump', '-t', 'terrain.o').splitlines()
                                  if line.split()[-1:] == ['_end'])
                    self.assertEqual(symbol[-3], section)
                    for base in (0x120000, 0x854000):
                        (work / 'stage.ld').write_text(
                            'PHDRS { code PT_LOAD FLAGS(5); records PT_LOAD FLAGS(6); } '
                            f'SECTIONS {{ . = {base}; .head : {{ *(.head) }} :code .text : {{ *(.text) }} :code '
                            '.data : { *(.data) } :records .bss : { *(.bss) } :records }')
                        run('ld', '-EL', '-T', 'stage.ld', '-o', 'stage.elf', 'terrain.o')
                        run('objcopy', '-O', 'binary', '-j', '.head', 'stage.elf', 'header.bin')
                        cursor, = struct.unpack('<I', (work / 'header.bin').read_bytes())
                        self.assertEqual(cursor, base + 20 + (20 if bss else 0))
