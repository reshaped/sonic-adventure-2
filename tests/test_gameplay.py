"""The players, and what play draws around them, as the host runs their C: the harnesses of tests/player/ and
tests/gameplay/."""

import subprocess
import sys
import tempfile
import unittest
from pathlib import Path

from csource import GCC, code, parse
from support import ROOT, compile_host, extract


class ItemIconMaterial(unittest.TestCase):
    """Run both item-icon renderers with an empty material bank and acquired slots."""

    def test_world_and_row_draw(self):
        with tempfile.TemporaryDirectory() as directory:
            scratch = Path(directory)
            subprocess.run([sys.executable, 'tools/body_data.py', '--stub', 'boss_bigfoot',
                            'modules/shared/item_icon.c', scratch], cwd=ROOT, check=True)
            subprocess.run([sys.executable, 'tools/body_data.py', '--stub', '1st_read',
                            'src/system/texture.c', scratch], cwd=ROOT, check=True)
            exe = scratch / 'item-icon'
            compile_host(exe, '-std=c89', '-O2', '-I.', '-Itests/textures', '-I', scratch / 'src',
                         '-I', scratch / 'modules', '-Imodules', '-Isrc', 'modules/shared/item_icon.c',
                         'tests/gameplay/item_icon_material.c', 'src/system/texture.c')
            subprocess.run([exe], check=True)


class ParticleMaterial(unittest.TestCase):
    """Run the particle task and the standalone quad draw with optional materials."""

    def test_loaded_frames_and_recovery(self):
        with tempfile.TemporaryDirectory() as directory:
            subprocess.run([sys.executable, 'tools/body_data.py', '--stub', '1st_read',
                            'src/system/texture.c', directory], cwd=ROOT, check=True)
            exe = Path(directory) / 'particle-material'
            compile_host(exe, '-std=c89', '-O2', '-I.', '-Itests/textures', '-I', Path(directory) / 'src',
                         '-include', 'config/intrinsics.h', '-Isrc', 'src/effect/particle.c',
                         'tests/gameplay/particle_material.c', 'src/system/texture.c')
            subprocess.run([exe], check=True)


class PlayerRepairs(unittest.TestCase):
    """Run the players' non-matching sides that repair a memory or an uninitialized tag."""

    def run_harness(self, harness, path, names, prefixes, *flags, more=()):
        with tempfile.TemporaryDirectory() as directory:
            scratch = Path(directory)
            extract(path, names, prefixes, scratch / 'extracted.inc')
            for include, *source in more:
                extract(*source, scratch / include)
            program = scratch / Path(harness).stem
            compile_host(program, '-std=gnu17', '-O2', '-D_builtin_fabsf=__builtin_fabsf', '-I', scratch, '-Isrc',
                         *flags, Path('tests/player') / harness, '-lm')
            subprocess.run([program], check=True)

    def test_earthquake_on_an_empty_slot(self):
        self.run_harness('earthquake.c', 'src/player/knuckles.c', ('struct earthquake', 'hunter_earthquake_end'),
                         ('#define EARTHQUAKE_', '#define SOUND_EARTHQUAKE '))

    def test_volley_at_an_empty_slot(self):
        self.run_harness('volley.c', 'src/player/walker_weapons.c', ('struct homing', 'volley_step'),
                         ('#define HOMING_', '#define VOLLEY_', '#define BEHIND_NUDGE '))

    def test_ricochet_at_an_empty_slot(self):
        self.run_harness('ricochet.c', 'src/player/walker_weapons.c', ('struct ricochet', 'ricochet_draw'),
                         ('#define RICOCHET_',))

    def test_wall_slide_normal_takes_the_y_a_mech_s_step_left(self):
        # each local without an initializer starts as a pattern, so a y no path sets is no zero
        self.run_harness('wall_slide.c', 'src/player/player.c', ('player_slide_along_wall',), ('#define TURN_AWAY ',),
                         '-ftrivial-auto-var-init=pattern',
                         more=[('walker.inc', 'src/player/walker.c',
                                ('walker_target_near', 'lockon_target_distance', 'lockon_box_distance',
                                 'lockon_player_distance'),
                                ('#define STRIKE_PLAYER_REACH ', '#define STRIKE_TARGET_REACH ', '#define STRIKE_NEAR ',
                                 '#define TARGET_LISTS ', '#define LOCKON_'))])

    def test_collide_normal_takes_the_y_the_call_before_left(self):
        self.run_harness('collide_normal.c', 'src/player/player.c',
                         ('player_apply_accel', 'player_apply_push', 'player_collide'),
                         ('#define CREEP_', '#define WATER_BRAKE_', '#define PUSH_SLOW_SPEED ',
                          '#define PUSH_STOP_SPEED ', '#define CRASH_ANY_SPEED ', '#define CRASH_HEAD_ON ',
                          '#define STOP_RUN_TURN ', '#define SIDE_PUSHED ', '#define POINTER_AS_FLOAT '),
                         '-ftrivial-auto-var-init=pattern')


class KillAreaFailure(unittest.TestCase):
    """Run the kill-area dispatcher with the real task allocator and a failing heap."""

    def test_admission_and_retry(self):
        types = ('struct miss', 'struct kill_areas')
        funcs = ('miss_destroy', 'miss_cry', 'area_miss_create', 'player_fall_in_area',
                 'player_sink_in_water', 'player_sink', 'player_keep_sunk', 'kill_areas_destroy',
                 'kill_areas_step', 'kill_areas_start')
        source = code(ROOT / 'src/player/player.c', GCC)
        declarations = {}
        for block in parse(source).blocks:
            signature = ' '.join(token.text for token in block.head)
            name = block.name or signature
            if name in types or name in funcs:
                declarations[name] = signature + ' ' + source[block.opening.start:block.closing.end]
        # Keep the production records, voice IDs and scheduler callbacks together.
        defines = [line for line in source.splitlines()
                   if line.startswith(('#define MISS(', '#define KILL_AREA_REACH ',
                                       '#define VOICE_MISS_'))]
        with tempfile.TemporaryDirectory() as directory:
            scratch = Path(directory)
            (scratch / 'kill_areas.inc').write_text('\n'.join(
                [*defines, *(declarations[name] + ';' for name in types),
                 *(declarations[name] for name in funcs)]))
            program = scratch / 'kill-area-failure'
            compile_host(program, '-std=c89', '-O2', '-D_builtin_fabsf=__builtin_fabsf', '-I', scratch, '-Isrc',
                         'tests/gameplay/kill_area_failure.c', 'src/system/task.c')
            subprocess.run([program], check=True)


class WalkerBlast(unittest.TestCase):
    """Check mesh selection and radius restoration for both walkers' lock-on blasts."""

    def test_blast_parts_with_available_data(self):
        owner = Path('src/player/walker_lock_assets.c')
        source = (ROOT / 'src/player/walker.c').read_text()
        draw, = [block for block in parse(source).blocks if block.name == 'lock_blast_draw_part']
        constants = source.index('#define BLAST_FADE_FULL ')
        original = ROOT / 'orig/1ST_READ.BIN'
        modes = [('empty', ['--stub'])]
        if original.is_file():
            modes.append(('optical', ['--original', original]))
        with tempfile.TemporaryDirectory() as directory:
            work = Path(directory)
            (work / 'blast-draw.inc').write_text(source[constants:draw.closing.end])
            for mode, inputs in modes:
                with self.subTest(mode=mode):
                    subprocess.run([sys.executable, 'tools/body_data.py', *inputs,
                                    '1st_read', owner, work], cwd=ROOT, check=True)
                    program = work / 'blast'
                    compile_host(program, '-std=c89', '-O2', '-Dinline=__inline__', '-I.', '-Isrc', '-Imodules',
                                 '-I', work / 'src', '-I', work, owner, 'tests/player/walker_blast.c')
                    subprocess.run([program, mode], check=True)


if __name__ == '__main__':
    unittest.main()
