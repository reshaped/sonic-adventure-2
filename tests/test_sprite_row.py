"""Draw a row of sprites that ends on the last of its cells, and read no cell past them."""

import signal
import subprocess
import sys
import tempfile
import unittest
from pathlib import Path

from support import ROOT, compile_host


class SpriteRow(unittest.TestCase):
    def test_only_the_matching_side_reads_past_the_cells(self):
        with tempfile.TemporaryDirectory() as directory:
            work = Path(directory)
            subprocess.run([sys.executable, 'tools/body_data.py', '--stub', 'advertise',
                            'modules/advertise/menu_draw.c', work], cwd=ROOT, check=True)
            for nonmatching in (False, True):
                with self.subTest(nonmatching=nonmatching):
                    program = work / f'sprite-row-{int(nonmatching)}'
                    # gnu89, not c89, for the host's MAP_ANON. -O0 keeps each read the side spells: above it a
                    # compiler may drop the matching side's last read, whose result nothing uses
                    compile_host(program, '-std=gnu89', '-O0', '-I.', '-Isrc', '-Imodules', '-I', work / 'modules',
                                 'tests/advertise/sprite_row.c', nonmatching=nonmatching)
                    status = subprocess.run([program]).returncode
                    # the matching side's read of the cell past the last faults on the page after the cells
                    self.assertIn(status, [0] if nonmatching else [-signal.SIGSEGV, -signal.SIGBUS])
