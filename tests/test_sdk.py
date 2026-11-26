"""The SDK's libraries, as the host runs their C: the harnesses of tests/middleware/, tests/sofdec/,
tests/sound/, tests/runtime/, tests/heap/ and tests/shinobi/."""

import subprocess
import tempfile
import unittest
from pathlib import Path

from support import compile_host, extract


class Repairs(unittest.TestCase):
    """Run the non-matching sides that repair a memory or an uninitialized tag in sdk/middleware and sdk/sofdec.

    Each harness under tests/middleware and tests/sofdec reaches the repairs of one source. A harness either includes
    its source whole, or includes extracted.inc, which this module writes from the source's GCC view: the #defines its
    prefixes name, then the functions it names, so that the harness can stand in for the rest. Each harness fails on its
    source as main held it before the repairs."""

    def run_harness(self, harness, directory, *flags, path=None, names=(), prefixes=()):
        with tempfile.TemporaryDirectory() as scratch:
            scratch = Path(scratch)
            if path is not None:
                extract(path, names, prefixes, scratch / 'extracted.inc')
            program = scratch / Path(harness).stem
            compile_host(program, '-std=gnu17', '-O2', '-I', scratch, f'-I{directory}', *flags, harness)
            subprocess.run([program], check=True)

    def test_renderer_with_a_null_joint(self):
        self.run_harness('tests/middleware/renderer.c', 'sdk/middleware')

    def test_sound_port_the_library_refuses(self):
        self.run_harness('tests/middleware/sound_port.c', 'sdk/middleware', '-ftrivial-auto-var-init=pattern',
                         path='sdk/middleware/mw_sdsb.c',
                         names=('mwlSbAlloc', 'mwSbOpenPort', 'mwSbGetPlayPos', 'mwSbGetTrnsStat'),
                         prefixes=('#define BYTE_BITS ',))

    def test_name_split_within_its_buffers(self):
        self.run_harness('tests/middleware/split_name.c', 'sdk/middleware')

    def test_wav_header_cut_inside_an_id(self):
        self.run_harness('tests/sofdec/wav_scan.c', 'sdk/sofdec')

    def test_ahx_header_of_three_channels(self):
        self.run_harness('tests/sofdec/ahx_channels.c', 'sdk/sofdec')

    def test_joint_bound_of_each_layer(self):
        self.run_harness('tests/sofdec/joint_bound.c', 'sdk/sofdec', '-fsanitize=bounds',
                         '-fno-sanitize-recover=bounds')

    def test_packet_of_another_stream(self):
        self.run_harness('tests/sofdec/packet_copy.c', 'sdk/sofdec', '-fsanitize=bounds',
                         '-fno-sanitize-recover=bounds', path='sdk/sofdec/sfd_mps.c', names=('sfmps_CopyPketData',),
                         prefixes=('#define NO_ROOM ', '#define TAKEN ', '#define INVALID_PACKET '))

    def test_audio_create_without_a_joint(self):
        self.run_harness('tests/sofdec/audio_create.c', 'sdk/sofdec', path='sdk/sofdec/sfd_adxt.c',
                         names=('SFADXT_Create',), prefixes=('#define NO_PLAYER ', '#define NO_STREAM '))


class PadGunIdentify(unittest.TestCase):
    """Run the peripheral server with a light gun that has no controller function."""

    def test_gun_support(self):
        with tempfile.TemporaryDirectory() as directory:
            program = Path(directory) / 'gun-identify'
            compile_host(program, '-std=c89', '-O2', '-ftrivial-auto-var-init=pattern', '-fsanitize=address,bounds',
                         '-fno-sanitize-recover=all', 'tests/shinobi/gun_identify.c')
            subprocess.run([program], check=True)


SANITIZE = ('-fsanitize=address,bounds', '-fno-sanitize-recover=all')


class RuntimeBounds(unittest.TestCase):
    """Run the runtime's rounding and formatter at the edges of their buffers, under the address sanitizer."""

    def run_program(self, name, *args, sdk_first=False):
        with tempfile.TemporaryDirectory() as directory:
            program = Path(directory) / name
            compile_host(program, '-std=c89', '-O2', *SANITIZE, *args, sdk_first=sdk_first)
            subprocess.run([program], check=True)

    def test_round_carry(self):
        self.run_program('round-carry', 'tests/runtime/round_carry.c')

    def test_format_bounds(self):
        self.run_program('format-bounds', 'sdk/runtime/_ctype.c', 'sdk/runtime/_errno.c',
                         'tests/runtime/format_bounds.c', sdk_first=True)


class ShinobiHeap(unittest.TestCase):
    """Exercise Shinobi's default public allocator on a bounded host buffer."""

    def test_spans_and_resize(self):
        with tempfile.TemporaryDirectory() as scratch:
            program = Path(scratch) / 'allocations'
            compile_host(program, '-std=c89', '-O2', '-Isdk/shinobi', 'tests/heap/allocations.c')
            subprocess.run([program], check=True)


class SoundMemoryBounds(unittest.TestCase):
    """Run the sound memory calls on a Uint32 or a Sint32 at each end of sound memory."""

    def test_bounds(self):
        with tempfile.TemporaryDirectory() as directory:
            program = Path(directory) / 'memory-bounds'
            compile_host(program, '-std=c89', '-O2', 'tests/sound/memory_bounds.c')
            subprocess.run([program], check=True)


if __name__ == '__main__':
    unittest.main()
