"""The game's core, as the host runs its C: the harnesses of tests/tasks/, tests/system/, tests/sequence/,
tests/camera/, tests/collision/ and tests/game_bounds/."""

import subprocess
import sys
import tempfile
import unittest
from pathlib import Path

from support import ROOT, compile_host


class CamlinBounds(unittest.TestCase):
    """Exercise camlin.c's reads at the ends of its tables and of a path longer than CAMLIN_PATH_POINTS."""

    def test_bounds(self):
        with tempfile.TemporaryDirectory() as directory:
            program = Path(directory) / 'camlin-bounds'
            compile_host(program, '-std=c89', '-O2', '-D_builtin_fabsf=__builtin_fabsf',
                         '-fsanitize=address,bounds', '-fno-sanitize-recover=all', '-I.', '-Isrc', '-Imodules',
                         'tests/camera/camlin_bounds.c', 'src/system/curve.c', '-lm')
            subprocess.run([program], check=True)


class CollisionAllocation(unittest.TestCase):
    """Exercise allocation and ownership through the portable collision constructors."""

    def test_owned_borrowed_and_failed_allocations(self):
        with tempfile.TemporaryDirectory() as directory:
            program = Path(directory) / 'collision'
            compile_host(program, '-std=c89', '-O2', '-D_builtin_fabsf=__builtin_fabsf', '-I.', '-Isrc', '-Imodules',
                         'src/collision/collision.c', 'tests/collision/allocation.c', '-lm')
            subprocess.run([program], check=True)


class GameBoundaries(unittest.TestCase):
    """Exercise the physical pad slots and the stage visibility grid through their normal readers."""

    def test_pad_and_visibility_domains(self):
        with tempfile.TemporaryDirectory() as directory:
            for name in ('pad', 'world'):
                with self.subTest(name=name):
                    program = Path(directory) / name
                    compile_host(program, '-std=c89', '-Dinline=__inline__', '-O2', '-D_builtin_fabsf=__builtin_fabsf',
                                 '-fsanitize=address,bounds', '-fno-sanitize-recover=all', '-I.', '-Isrc',
                                 '-Imodules', f'tests/game_bounds/{name}.c', '-lm')
                    subprocess.run([program], check=True)


class MeshPoolOut(unittest.TestCase):
    """Update the collision meshes where the pool has no mesh left."""

    def test_pool_out(self):
        with tempfile.TemporaryDirectory() as directory:
            program = Path(directory) / 'mesh-pool-out'
            compile_host(program, '-std=c89', '-O2', '-D_builtin_fabsf=__builtin_fabsf', '-fsanitize=address,bounds',
                         '-fno-sanitize-recover=all', '-I.', '-Isrc', '-Imodules', 'tests/collision/mesh_pool_out.c',
                         '-lm')
            subprocess.run([program], check=True)


class CheckedPrs(unittest.TestCase):
    """Exercise the Dreamcast port's bounded PRS decoder."""

    def test_bounded_streams(self):
        with tempfile.TemporaryDirectory() as directory:
            program = Path(directory) / 'prs'
            compile_host(program, '-std=c89', '-O2', '-Isrc', 'engine/dc/prs_checked.c', 'tests/system/prs.c')
            subprocess.run([program], check=True)


class StorySequence(unittest.TestCase):
    """The story controller plays a table's steps and mini event, and goes to the menu at and past its end."""

    def test_steps_mini_event_and_table_end(self):
        with tempfile.TemporaryDirectory() as directory:
            program = Path(directory) / 'story'
            compile_host(program, '-std=c89', '-O2', '-Dinline=__inline__', '-Isrc', '-Imodules',
                         'src/system/sequence.c', 'tests/sequence/steps.c')
            subprocess.run([program], check=True)


class TaskSounds(unittest.TestCase):
    """Exercise task-owned sound slots through the scheduler, pause and resume."""

    def test_releases_followers_before_task_storage_is_reused(self):
        with tempfile.TemporaryDirectory() as directory:
            d = Path(directory)
            program = d / 'task_sound'
            subprocess.run([sys.executable, ROOT / 'tools/body_data.py', '1st_read', 'src/sound/sound.c',
                            d, '--stub'], cwd=ROOT, check=True, timeout=10)
            compile_host(program, '-std=c99', '-O2', '-D_builtin_fabsf=__builtin_fabsf', '-Isrc', '-I', d / 'src',
                         '-I.', 'tests/tasks/sound_lifetime.c', 'src/system/task.c')
            subprocess.run([program], check=True)


class TaskLifetimes(unittest.TestCase):
    """Run the portable task scheduler with the game's public creation, step, draw and destruction calls."""

    def test_order_and_lifetime(self):
        with tempfile.TemporaryDirectory() as directory:
            program = Path(directory) / 'tasks'
            compile_host(program, '-std=c89', '-O2', '-Isrc', 'tests/tasks/scheduler.c', 'src/system/task.c')
            subprocess.run([program], check=True)


if __name__ == '__main__':
    unittest.main()
