"""Run the end of an event's typed line, which the GCC game's skip calls."""
import subprocess
import tempfile
import unittest
from pathlib import Path

from support import ROOT, compile_host


class EventTyping(unittest.TestCase):
    def test_a_line_ended_early_destroys_its_sprites(self):
        # the line's place is the disc's data, so the test writes one in
        place = '#include "event/typing/position.inc"'
        source = (ROOT / 'modules/event/typing.c').read_text()
        self.assertEqual(source.count(place), 1)
        with tempfile.TemporaryDirectory() as directory:
            scratch = Path(directory)
            (scratch / 'typing.inc').write_text(source.replace(place, '{0.0F, 400.0F}, 1.0F'))
            compile_host(scratch / 'typing', '-std=c89', '-O2', '-I', scratch, '-Isrc', '-Imodules',
                         'tests/event/typing.c')
            subprocess.run([scratch / 'typing'], check=True)


if __name__ == '__main__':
    unittest.main()
