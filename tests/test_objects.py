"""The shared objects, the bosses and the enemies, as the host runs their C: the harnesses of tests/objects/
and tests/enemy/."""

import re
import subprocess
import sys
import tempfile
import unittest
from pathlib import Path

from csource import parse
from support import ROOT, compile_host


# each mech boss's source, the functions its program runs, and the motion whose link its make tests: a link shorter
# than the frames given makes no boss
MECH_BOSSES = {
    'BIGFOOT': ('boss_bigfoot/bigfoot.c', {'bigfoot_create', 'bigfoot_charge', 'bigfoot_hit_in_air'}, 'MOTION_LEAN', 3),
    'FDOG': ('boss_fdog/fdog.c', {'fdog_create', 'fdog_charge', 'fdog_beam'}, 'MOTION_OPEN', 1),
    'HOTSHOT': ('boss_hotshot/hotshot.c', {'hotshot_create', 'hotshot_charge', 'hotshot_track'}, 'MOTION_LEAN', 3),
}


class BossBugSides(unittest.TestCase):
    """Run the bosses' non-matching sides that keep or repair a memory or an uninitialized tag."""

    def setUp(self):
        scratch = tempfile.TemporaryDirectory()
        self.addCleanup(scratch.cleanup)
        self.scratch = Path(scratch.name)

    def kept(self, source, names, bodies=()):
        """Copy source, a path under modules/, to the scratch directory's modules/, with each function emptied but
        those names holds, so that a program needs only what those call. The copy drops each section(".data"),
        which Mach-O rejects: it names a section by segment and section. bodies holds each generated body the copy
        includes, as (its path under modules/, its text)."""
        text = (ROOT / 'modules' / source).read_text().replace(', section(".data")', '')
        blocks = [block for block in parse(text).blocks if block.name and block.name not in names]
        for block in sorted(blocks, key=lambda block: block.opening.end, reverse=True):
            text = text[:block.opening.end] + text[block.closing.start:]
        for path, body in ((source, text), *bodies):
            (self.scratch / 'modules' / path).parent.mkdir(parents=True, exist_ok=True)
            (self.scratch / 'modules' / path).write_text(body)

    def run_program(self, *args):
        program = self.scratch / 'boss'
        compile_host(program, '-std=c89', '-O2', '-D_builtin_fabsf=__builtin_fabsf', '-I', self.scratch / 'modules',
                     '-I.', '-Isrc', '-Imodules', *args)
        subprocess.run([program], check=True, timeout=10)

    def test_explosion_without_its_record(self):
        self.run_program('tests/objects/golem_explosion.c', 'src/system/task.c')

    def test_slam_whose_wide_blows_land_on_no_foothold(self):
        # each local without an initializer starts as a pattern, so a landed that the slam leaves unset isn't 0
        for fight in ([], ['-DGOLEM_E']):
            with self.subTest(fight=fight):
                self.run_program(*fight, '-ftrivial-auto-var-init=pattern', 'tests/objects/golem_slam.c')

    def test_shatter_with_and_without_its_pieces(self):
        # drift starts as a pattern, as the slam's landed does
        self.kept('boss_golem/footholds.c', {'foothold_shatter'})
        self.run_program('-ftrivial-auto-var-init=pattern', 'tests/objects/golem_shatter.c')

    def test_wave_without_its_record(self):
        self.kept('boss_golem/tactics.c', {'golem_wave_create'},
                  [('boss_golem/tactics/hand_points.inc', '{0.0F, 0.0F, 0.0F}, {0.0F, 0.0F, 0.0F}\n')])
        self.run_program('tests/objects/golem_wave.c')

    def test_mech_bosses_without_their_parts(self):
        for boss, (source, names, motion, frames) in MECH_BOSSES.items():
            with self.subTest(boss=boss):
                motions = source.replace('.c', f'/{Path(source).stem}_motions.inc')
                self.kept(source, names, [(motions, f'[{motion}].link_frames = {frames}\n')])
                self.run_program(f'-D{boss}', 'tests/objects/mech_boss_parts.c')


class EnemyDefeat(unittest.TestCase):
    """Defeat an enemy that has no collision record."""

    def test_no_record(self):
        with tempfile.TemporaryDirectory() as directory:
            work = Path(directory)
            source = 'src/enemy/enemy.c'
            subprocess.run([sys.executable, 'tools/body_data.py', '--stub', '1st_read', source, work], cwd=ROOT,
                           check=True)
            program = work / 'enemy-defeat'
            compile_host(program, '-std=c89', '-O2', '-D_builtin_fabsf=__builtin_fabsf', '-fsanitize=address,bounds',
                         '-fno-sanitize-recover=all', '-I.', '-Isrc', '-Imodules', '-I', work / 'src',
                         'tests/enemy/defeat.c', '-lm')
            subprocess.run([program], check=True)


class FinalRushSound(unittest.TestCase):
    """Exercise Final Rush's positional fragment sound as camera 0 disappears and returns."""

    def test_fragment_sound_with_and_without_camera(self):
        with tempfile.TemporaryDirectory() as directory:
            work = Path(directory)
            unit = Path('modules/stg30/final_rush.c')
            subprocess.run([sys.executable, 'tools/body_data.py', '--stub', 'stg30', unit, work],
                           check=True, cwd=ROOT)
            executable = work / 'rockfall-sound'
            compile_host(executable, '-std=c89', '-O2', '-D_builtin_fabsf=__builtin_fabsf', '-Isrc', '-Imodules',
                         '-I', work / 'variants' / unit.with_suffix(''), 'tests/objects/rockfall_sound.c',
                         'modules/stg30/space_motion.c', 'sdk/ninja/njDistanceP2P.c', 'sdk/ninja/njDistanceP2L.c',
                         '-lm')
            subprocess.run([executable], check=True)


class LockOnMissile(unittest.TestCase):
    """Exercise Iron Gate's launcher through the portable game callbacks."""

    def test_flight_and_target_lifetime(self):
        with tempfile.TemporaryDirectory() as directory:
            work = Path(directory)
            launcher = 'modules/stg11/lock_on_missile.c'
            # The flight harness needs the model links when the launcher sheds its cover.
            subprocess.run([sys.executable, 'tools/body_data.py', 'stg11', launcher, work, '--stub'],
                           cwd=ROOT, check=True)
            program = work / 'launcher'
            compile_host(program, '-std=c89', '-O2', '-D_builtin_fabsf=__builtin_fabsf', '-I', work / 'modules',
                         '-I.', '-Isrc', '-Imodules', launcher, 'tests/objects/lock_on_missile.c', '-lm')
            subprocess.run([program], check=True)


# each script's lines, as struct voice_line's voice, closing_voice, timer, unk_0a and power. AT_ONCE is 10
SCRIPTS = ',\n'.join(['{{1, 0, 0, 0, 10}, {2, 0, 0, 0, 1}, {3, 0, 0, 0, 0}}'] * 3) + '\n'


class MechBossVoice(unittest.TestCase):
    """Play the Flying Dog's voice script to its end, past its last line's power."""

    def test_last_line_plays_out(self):
        with tempfile.TemporaryDirectory() as directory:
            work = Path(directory)
            table = work / 'shared/mech_boss_voice/voice_scripts.inc'
            table.parent.mkdir(parents=True)
            table.write_text(SCRIPTS)
            program = work / 'mech_boss_voice'
            compile_host(program, '-std=c89', '-O2', '-fsanitize=bounds', '-fno-sanitize-recover=bounds', '-I', work,
                         '-Isrc', '-Imodules', 'tests/objects/mech_boss_voice.c', 'modules/shared/mech_boss_voice.c')
            subprocess.run([program], check=True, timeout=10)


# each source, by its image, whose init tests/objects/init_failures.c runs
INITS = [
    ('stg26', 'snake_pot'),
    *(('stg28', name) for name in ('awning', 'block', 'cave_pyramid', 'eye', 'firepot', 'key_door', 'keyhole',
                                   'sandglass_door', 'snake_head', 'snake_rail', 'snake_statue', 'wall',
                                   'windmill')),
    ('stg32', 'large_meteor'),
    ('stg32', 'meteors'),
    ('stg44', 'ore'),
]
# the functions a make only installs, as a step, a draw or a mode's start. no run here calls one
INSTALLED = re.compile(r'\w+_(step|draw|start)')


class ObjectInitFailures(unittest.TestCase):
    """Make objects while the heap or the world's pool withholds what the make allocates: a collision object, a body, a
    collision record or a record of the object's own."""

    def setUp(self):
        scratch = tempfile.TemporaryDirectory()
        self.addCleanup(scratch.cleanup)
        self.scratch = Path(scratch.name)

    def emptied(self, image, name, installed=INSTALLED):
        """Copy image's source name under the scratch directory, and empty each function whose name installed
        matches, so that a program needs nothing those functions call. Write stubs for the copy's generated
        bodies. Return the copy's path and the include flags for the stubs, as (path, flags)."""
        source = Path('modules') / image / f'{name}.c'
        # the copy drops each section(".data"), which Mach-O rejects: it names a section by segment and section.
        # no run here reads where a datum lies
        text = (ROOT / source).read_text().replace(', section(".data")', '')
        blocks = [block for block in parse(text).blocks if block.name and installed.fullmatch(block.name)]
        self.assertTrue(blocks, source)
        for block in sorted(blocks, key=lambda block: block.opening.end, reverse=True):
            text = text[:block.opening.end] + text[block.closing.start:]
        (self.scratch / source).parent.mkdir(parents=True, exist_ok=True)
        (self.scratch / source).write_text(text)
        subprocess.run([sys.executable, 'tools/body_data.py', '--stub', image, source, self.scratch], cwd=ROOT,
                       check=True)
        return self.scratch / source, ['-I', self.scratch / 'variants' / source.with_suffix('')]

    def run_program(self, sources, includes, test):
        program = self.scratch / Path(test).stem
        compile_host(program, '-std=c89', '-O2', '-D_builtin_fabsf=__builtin_fabsf', '-I', self.scratch, *includes,
                     '-I', self.scratch / 'modules', '-I.', '-Isrc', '-Imodules', *sources, test, '-lm')
        subprocess.run([program], check=True)

    def test_inits_without_their_records(self):
        sources, includes = [], []
        for image, name in INITS:
            source, flags = self.emptied(image, name)
            sources.append(source)
            includes += flags
        self.run_program(sources, includes, 'tests/objects/init_failures.c')

    def test_sandglass_without_its_record(self):
        source, includes = self.emptied('stg28', 'sandglass')
        self.run_program([source], includes, 'tests/objects/sandglass_records.c')

    def test_key_without_its_body(self):
        # key_loose_step stays: the test calls it. key_loose.c includes the emptied source
        _, includes = self.emptied('stg28', 'key', re.compile(r'(?!key_loose_step$)\w+_(step|draw|start)'))
        self.run_program([], includes, 'tests/objects/key_loose.c')

    def test_drill_without_its_record(self):
        # drill_start stays: the test calls it. drill_start.c includes the emptied source
        _, includes = self.emptied('stg43', 'drill', re.compile(r'\w+_(step|draw)'))
        self.run_program([], includes, 'tests/objects/drill_start.c')

    def test_racer_without_its_kart(self):
        # cart_create_racer stays, and so does each function it calls before it reads its kart
        source, includes = self.emptied('cart', 'vehicle', re.compile(r'init_racer|racer_(step|draw|destroy)'))
        self.run_program([source], includes, 'tests/cart/racer_without_kart.c')


class Pillar4(unittest.TestCase):
    """Make a pillar where the world's pool has no object to lend, then again where it has one."""

    def test_pool_empty(self):
        with tempfile.TemporaryDirectory() as directory:
            program = Path(directory) / 'pillar4'
            compile_host(program, '-std=c89', '-O2', '-I.', '-Isrc', '-Imodules', 'tests/objects/pillar4.c',
                         'modules/shared/pillar4.c', 'src/system/task.c')
            subprocess.run([program], check=True, timeout=10)


class RockfallLifetime(unittest.TestCase):
    """Exercise the Final Rush rock constructor and its deferred task teardown."""

    def test_attach_step_and_teardown(self):
        with tempfile.TemporaryDirectory() as directory:
            program = Path(directory) / 'rockfall'
            generated = Path(directory) / 'generated'
            subprocess.run([sys.executable, 'tools/body_data.py', 'stg30', 'modules/stg30/rockfall.c',
                            generated, '--stub'], cwd=ROOT, check=True)

            def compile_fixture():
                compile_host(program, '-std=c89', '-O2', '-D_builtin_fabsf=__builtin_fabsf',
                             '-I', generated / 'modules', '-I.', '-Isrc', '-Imodules',
                             'tests/objects/rockfall_lifetime.c', 'modules/stg30/rockfall.c', 'src/system/task.c',
                             'src/collision/collision.c', '-lm')
            compile_fixture()
            subprocess.run([program, 'empty'], check=True)
            dimensions = generated / 'modules/stg30/rockfall/sizes.inc'
            dimensions.write_text('{3, 4, 5}, {7, 8, 9}, {2, 5, 2}, {8, 2, 8}\n')
            compile_fixture()
            subprocess.run([program], check=True)


class SandglassRegistry(unittest.TestCase):
    """Register a sandglass where the heap has no room for the table."""

    def test_table_refused(self):
        with tempfile.TemporaryDirectory() as directory:
            program = Path(directory) / 'sandglass_registry'
            compile_host(program, '-std=c89', '-O2', '-Isrc', '-Imodules', 'tests/objects/sandglass_registry.c',
                         'modules/shared/sandglass_registry.c')
            subprocess.run([program], check=True, timeout=10)


if __name__ == '__main__':
    unittest.main()
