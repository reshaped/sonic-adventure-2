"""The effects, as the host runs their C: the harnesses of tests/effects/, and the scale their particles take
random numbers by."""

import subprocess
import sys
import tempfile
import unittest
from pathlib import Path

from support import ROOT, compile_host


class BubbleFree(unittest.TestCase):
    """Step a bubble that reaches the surface, which bubbles_step frees."""

    def test_freed_bubble(self):
        with tempfile.TemporaryDirectory() as directory:
            work = Path(directory)
            source = 'src/effect/bubbles.c'
            subprocess.run([sys.executable, 'tools/body_data.py', '--stub', '1st_read', source, work], cwd=ROOT,
                           check=True)
            program = work / 'bubble-free'
            compile_host(program, '-std=c89', '-O2', '-D_builtin_fabsf=__builtin_fabsf', '-fsanitize=address,bounds',
                         '-fno-sanitize-recover=all', '-I.', '-Isrc', '-Imodules', '-I', work / 'src',
                         'tests/effects/bubble_free.c', '-lm')
            subprocess.run([program], check=True)


PROBE = r'''
#include <assert.h>
#include <stdlib.h>
#ifdef TEST_RANDOM_MAX
#undef RAND_MAX
#define RAND_MAX TEST_RANDOM_MAX
#endif
#include "system/util.h"

static void check_index(int sample, int count)
{
    Float fraction = sample * RAND_SCALE;
    int index = (int)((Float)sample * count * RAND_SCALE);
    assert(fraction >= 0.0F && fraction < 1.0F);
    assert(index >= 0 && index < count);
}

int main(void)
{
    int i;
    for (i = 0; i < 4096; ++i) {
        check_index(i, 257);
        check_index(RAND_MAX - i, 3);
        check_index(RAND_MAX - i, 256);
        check_index(RAND_MAX - i, 1025);
    }
    if (RAND_MAX == 32767) {
        assert(RAND_SCALE == 1.0F / 32768.0F);
    }
    return 0;
}
'''


class RandomScale(unittest.TestCase):
    """Check the Float used for particle timing and random array indices."""

    def test_runtime_maxima(self):
        with tempfile.TemporaryDirectory() as directory:
            source = Path(directory) / 'scale.c'
            program = Path(directory) / 'scale'
            source.write_text(PROBE)
            for maximum in (None, 32767, 16777215, 16777216, 2147483647):
                with self.subTest(maximum=maximum):
                    options = [] if maximum is None else [f'-DTEST_RANDOM_MAX={maximum}']
                    compile_host(program, '-std=c89', '-O2', *options, '-Isrc', source)
                    subprocess.run([program], check=True)


class SonicEffectSlot(unittest.TestCase):
    """Draw and step Sonic's effects once his slot is empty."""

    def test_empty_slot(self):
        with tempfile.TemporaryDirectory() as directory:
            work = Path(directory)
            source = 'src/effect/sonic_effect.c'
            subprocess.run([sys.executable, 'tools/body_data.py', '--stub', '1st_read', source, work], cwd=ROOT,
                           check=True)
            program = work / 'sonic-effect-slot'
            compile_host(program, '-std=c89', '-O2', '-D_builtin_fabsf=__builtin_fabsf', '-fsanitize=address,bounds',
                         '-fno-sanitize-recover=all', '-I.', '-Isrc', '-Imodules', '-I', work / 'src',
                         'tests/effects/sonic_effect_slot.c', '-lm')
            subprocess.run([program], check=True)


class SpeedTextures(unittest.TestCase):
    """Exercise the speed characters' effect texture task and PVM lifetimes."""

    def test_shared_archive_teardown_and_retry(self):
        with tempfile.TemporaryDirectory() as directory:
            work = Path(directory)
            actor = 'src/effect/sonic_effect.c'
            for source in (actor, 'src/system/texture.c'):
                subprocess.run([sys.executable, 'tools/body_data.py', '--stub', '1st_read', source, work],
                               cwd=ROOT, check=True)
            program = work / 'speed-textures'
            compile_host(program, '-std=c89', '-O2', '-D_builtin_fabsf=__builtin_fabsf', '-Itests/textures', '-I.',
                         '-Isrc', '-Imodules', '-I', work / 'src', actor, 'src/system/task.c', 'src/system/texture.c',
                         'engine/dc/pvm.c', 'sdk/ninja/njCheckTextureFormat.c', 'sdk/ninja/njTextureGlobal.c',
                         'sdk/ninja/njReleaseTexture.c', 'tests/effects/speed_textures.c')
            subprocess.run([program], check=True, timeout=5)


class ThunderLight(unittest.TestCase):
    """Step a bolt past its length where the heap gives no light."""

    def test_no_light(self):
        with tempfile.TemporaryDirectory() as directory:
            work = Path(directory)
            source = 'src/effect/thunder.c'
            subprocess.run([sys.executable, 'tools/body_data.py', '--stub', '1st_read', source, work], cwd=ROOT,
                           check=True)
            program = work / 'thunder-light'
            compile_host(program, '-std=c89', '-O2', '-fsanitize=address,bounds', '-fno-sanitize-recover=all',
                         '-I.', '-Isrc', '-Imodules', '-I', work / 'src', 'tests/effects/thunder_light.c', '-lm')
            subprocess.run([program], check=True)


if __name__ == '__main__':
    unittest.main()
