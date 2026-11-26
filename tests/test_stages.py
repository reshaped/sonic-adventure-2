"""The stages' own objects, as the host runs their C: the harnesses of tests/stages/."""

import re
import subprocess
import sys
import tempfile
import unittest
from pathlib import Path

from csource import GCC, code, parse
from support import ROOT, compile_host


class LandClip(unittest.TestCase):
    """Step Cannon's Core's land blocks in Eggman's and Tails's routes while slot 0 holds no player."""

    def test_empty_slot_keeps_the_mask(self):
        for image, name in (('stg35', 'core_eggman'), ('stg36', 'core_tails')):
            with self.subTest(source=name), tempfile.TemporaryDirectory() as directory:
                work = Path(directory)
                source = Path('modules') / image / f'{name}.c'
                subprocess.run([sys.executable, 'tools/body_data.py', '--stub', image, source, work], cwd=ROOT,
                               check=True)
                program = work / 'land-clip'
                compile_host(program, '-std=c89', '-O2', '-D_builtin_fabsf=__builtin_fabsf',
                             f'-DSOURCE="{source}"', '-I', work / 'variants' / source.with_suffix(''), '-I',
                             work / 'modules', '-I.', '-Isrc', '-Imodules', 'tests/stages/land_clip.c', '-lm')
                subprocess.run([program], check=True)


class MadSpaceWind(unittest.TestCase):
    """Step Mad Space while slot 0 holds no player."""

    def test_empty_slot_plays_no_low_wind(self):
        with tempfile.TemporaryDirectory() as directory:
            work = Path(directory)
            subprocess.run([sys.executable, 'tools/body_data.py', '--stub', 'stg44', 'modules/stg44/mad_space.c',
                            work], cwd=ROOT, check=True)
            program = work / 'mad-space-wind'
            compile_host(program, '-std=c89', '-O2', '-D_builtin_fabsf=__builtin_fabsf', '-I', work / 'modules',
                         '-I.', '-Isrc', '-Imodules', 'tests/stages/mad_space_wind.c', '-lm')
            subprocess.run([program], check=True)


class StackedPlatform(unittest.TestCase):
    """Exercise the two desert stack constructors through the portable task scheduler."""

    def test_partial_stack_releases_placement_and_can_respawn(self):
        with tempfile.TemporaryDirectory() as directory:
            scratch = Path(directory)
            task_header = Path('shared/stacked_platform_task.h')
            text = (ROOT / 'modules' / task_header).read_text()
            # Simulate construction only. Frame and draw callbacks require the
            # renderer and combat systems. Keep the actual stack step, which
            # dereferences the parent's table on the following frame.
            callbacks = {'stack_draw', 'tier_step', 'tier_draw', 'target_step'}
            blocks = [b for b in parse(text).blocks if b.name in callbacks]
            self.assertCountEqual([b.name for b in blocks], callbacks)
            for block in reversed(blocks):
                text = text[:block.opening.end] + text[block.closing.start:]
            local = scratch / task_header
            local.parent.mkdir()
            local.write_text(text)
            subprocess.run([sys.executable, 'tools/body_data.py', '--stub', '1st_read',
                            'src/enemy/enemy.c', scratch], cwd=ROOT, check=True)
            for stage in ('stg21', 'stg23'):
                with self.subTest(stage=stage):
                    unit = Path('modules') / stage / 'stacked_platform.c'
                    subprocess.run([sys.executable, 'tools/body_data.py', '--stub', stage, unit, scratch],
                                   cwd=ROOT, check=True)
                    executable = scratch / stage
                    compile_host(executable, '-std=c89', '-O2', '-D_builtin_fabsf=__builtin_fabsf', '-I', scratch,
                                 '-I', scratch / 'src', '-I', scratch / 'variants' / unit.with_suffix(''), '-I.',
                                 '-Isrc', '-Imodules', unit, 'src/system/task.c', 'src/collision/collision.c',
                                 'src/enemy/enemy.c', 'tests/stages/stacked_platform.c', '-lm')
                    subprocess.run([executable], check=True)


def run_object(scratch, stage, source, harness, emptied):
    """Build source, from stage's image, with harness and tests/stages/object_world.c into a program, and run it.
    The bodies of the functions emptied names are left empty: the draws and steps the harness never calls, whose
    callees it would otherwise need."""
    source = Path(source)
    subprocess.run([sys.executable, 'tools/body_data.py', '--stub', stage, source, scratch], cwd=ROOT, check=True)
    text = (ROOT / source).read_text()
    blocks = [block for block in parse(text).blocks if block.name in emptied]
    assert sorted(block.name for block in blocks) == sorted(emptied), source
    for block in sorted(blocks, key=lambda block: block.opening.start, reverse=True):
        text = text[:block.opening.end] + text[block.closing.start:]
    local = scratch / 'emptied' / source
    local.parent.mkdir(parents=True, exist_ok=True)
    local.write_text(text)
    program = scratch / source.stem
    compile_host(program, '-std=c89', '-O2', '-D_builtin_fabsf=__builtin_fabsf', '-I', scratch / 'modules',
                 '-I', scratch / 'variants' / source.with_suffix(''), '-I.', '-Isrc', '-Imodules', local, harness,
                 'tests/stages/object_world.c', 'src/system/task.c', '-lm')
    subprocess.run([program], check=True)


class StageRecords(unittest.TestCase):
    """Stage objects' non-matching side, built for the host, where the image reads or writes what it doesn't hold:
    an object that lacks a record leaves nothing of itself behind or waits for the record, none writes past what it
    was given, and none moves or draws by a field it hasn't set."""

    def setUp(self):
        directory = tempfile.TemporaryDirectory()
        self.addCleanup(directory.cleanup)
        self.scratch = Path(directory.name)

    def test_turtle_without_ground(self):
        run_object(self.scratch, 'stg18', 'modules/stg18/turtle.c', 'tests/stages/turtle_ground.c',
                   {'turtle_step', 'turtle_draw', 'turtle_sphere_create'})

    def test_spinning_hammer_without_drum_or_bar(self):
        run_object(self.scratch, 'stg14', 'modules/stg14/spinning_hammer.c', 'tests/stages/spinning_hammer.c',
                   {'spinning_hammer_step', 'spinning_hammer_draw', 'spinning_hammer_draw_shadow'})

    def test_pool_without_water_object(self):
        run_object(self.scratch, 'stg18', 'modules/stg18/pool.c', 'tests/stages/pool_water.c',
                   {'pool_draw', 'pool_child_step'})

    def test_shed_target_without_record(self):
        run_object(self.scratch, 'stg05', 'modules/stg05/shed.c', 'tests/stages/shed_target.c',
                   {'shed_step', 'shed_draw'})

    def test_tower_without_tier_table(self):
        run_object(self.scratch, 'stg23', 'modules/stg23/tower.c', 'tests/stages/tower_tiers.c', {'tier_init'})

    def test_bats_drawn_before_their_first_step(self):
        run_object(self.scratch, 'stg25', 'modules/stg25/bats.c', 'tests/stages/bats_home.c', set())

    def test_butterflies_first_heading(self):
        run_object(self.scratch, 'stg18', 'modules/stg18/butterflies.c', 'tests/stages/butterflies_heading.c', set())

    def test_lane_writes_within_its_shape(self):
        run_object(self.scratch, 'stg14', 'modules/stg14/lane.c', 'tests/stages/lane_shapes.c',
                   {'lane_draw', 'quake_step'})

    def test_lion_head_without_body(self):
        run_object(self.scratch, 'stg25', 'modules/stg25/lion_head.c', 'tests/stages/lion_head_body.c',
                   {'chamber_lion_head_step', 'chamber_lion_head_draw'})

    def test_car_without_record(self):
        run_object(self.scratch, 'stg13', 'modules/stg13/car.c', 'tests/stages/car_record.c', {'car_step', 'car_draw'})

    def test_pillar_without_world_object(self):
        run_object(self.scratch, 'stg21', 'modules/stg21/pillar.c', 'tests/stages/pillar_shaft.c',
                   {'pillar_step', 'pillar_draw', 'shaft_step', 'shaft_draw'})

    def test_sandglass_door_without_world_object(self):
        run_object(self.scratch, 'stg25', 'modules/stg25/sandglass_door.c', 'tests/stages/sandglass_door_collider.c',
                   {'sandglass_door_draw', 'sandglass_door_shut_step', 'sandglass_door_opening_step',
                    'sandglass_door_bouncing_step', 'sandglass_door_closing_step'})

    def test_reactor_manager_without_player(self):
        run_object(self.scratch, 'stg24', 'modules/stg24/eternal_engine.c', 'tests/stages/reactor_voice.c', set())


class StageTextureDraw(unittest.TestCase):
    """Exercise stage objects and the demo frame through their real draw calls."""

    def test_selected_slot_and_demo_blink(self):
        # These are the two complete frame functions, including the demo quit path.
        # The fixture supplies the surrounding scheduler and Ninja draw interfaces.
        stage = ROOT / 'src/stage/stage.c'
        source = stage.read_text()
        names = {'stage_run_solo', 'stage_run'}
        functions = {block.name: source[block.head[0].start:block.closing.end]
                     for block in parse(source).blocks if block.closing and block.name in names}
        constants = {'PRESS_START_TEXTURE', 'PRESS_START_BLINK', 'PRESS_START_READY',
                     'SOLO_STEPS', 'PAIR_STEPS', 'FRAME_STEPS', 'PAL_FRAMES',
                     'BLANKS_PER_TIME', 'DRAW_MARGIN'}
        defines = {name: line for line in code(stage, GCC).splitlines()
                   if (match := re.match(r'^#define (\w+)', line))
                   and (name := match[1]) in constants}
        self.assertEqual(defines.keys(), constants)
        self.assertEqual(functions.keys(), names)
        with tempfile.TemporaryDirectory() as tmp:
            work = Path(tmp)
            (work / 'demo_frame.inc').write_text('\n'.join([*defines.values(), functions['stage_run_solo'],
                                                           functions['stage_run']]) + '\n')
            poster = 'modules/stg13/poster.c'
            jungle = 'modules/shared/jungle_prop.c'
            for image, source in (('stg13', poster), ('stg19', jungle), ('1st_read', 'src/system/texture.c')):
                subprocess.run([sys.executable, 'tools/body_data.py', '--stub', image, source, work],
                               cwd=ROOT, check=True)
            program = work / 'stage-textures'
            compile_host(program, '-std=c89', '-O2', '-Dinline=__inline__', '-D_builtin_fabsf=__builtin_fabsf',
                         '-Itests/textures', '-I.', '-Isrc', '-Imodules', '-I', work, '-I', work / 'modules',
                         '-I', work / 'src', poster, jungle, 'src/system/texture.c', 'tests/stages/texture_draw.c')
            subprocess.run([program], check=True, timeout=10)


if __name__ == '__main__':
    unittest.main()
