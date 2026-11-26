"""Run the GCC game's radar, which follows every piece of the hunt not taken."""

import subprocess
import tempfile
import unittest
from pathlib import Path

from csource import GCC, code, parse
from support import ROOT, compile_host

FUNCS = ('hunt_sound_radar', 'hunt_draw_gauge', 'hunt_draw')
DEFINES = ('#define BEEP_', '#define SOUND_BEEP ', '#define HEARD_', '#define VOICE_NEAR_',
           '#define RADAR_', '#define TEX_FLASH ', '#define GAUGE_', '#define RING_')


class HuntRadar(unittest.TestCase):
    def test_every_piece_beeps_speaks_and_shows_its_gauge(self):
        source = code(ROOT / 'src/player/knuckles.c', GCC)
        bodies = {}
        for block in parse(source).blocks:
            if block.name in FUNCS:
                signature = ' '.join(token.text for token in block.head)
                bodies[block.name] = signature + ' ' + source[block.opening.start:block.closing.end]
        self.assertEqual(sorted(bodies), sorted(FUNCS))
        defines = [line for line in source.splitlines() if line.startswith(DEFINES)]
        with tempfile.TemporaryDirectory() as directory:
            scratch = Path(directory)
            (scratch / 'hunt_radar.inc').write_text(
                '\n'.join([*defines, *(bodies[name] for name in FUNCS)]) + '\n')
            compile_host(scratch / 'hunt-radar', '-std=c89', '-O2', '-I', scratch, '-Isrc',
                         'tests/player/hunt_radar.c', '-lm')
            subprocess.run([scratch / 'hunt-radar'], check=True)


if __name__ == '__main__':
    unittest.main()
