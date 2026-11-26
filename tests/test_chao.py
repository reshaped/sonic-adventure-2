"""Chao, their gardens, the kindergarten and the races, as the host runs their C: the harnesses of tests/chao/."""

import subprocess
import sys
import tempfile
import unittest
from pathlib import Path

from csource import parse
from support import ROOT, compile_host


class ChaoAdmission(unittest.TestCase):
    """The garden may publish an area only after both its image and its tasks are ready, and stops the game where
    one of its textures won't load."""

    def test_area_controller_and_texture_stops(self):
        with tempfile.TemporaryDirectory() as directory:
            for name in ('area_admission', 'texture_admission'):
                program = Path(directory) / name
                compile_host(program, '-std=c89', '-O2', '-Dinline=__inline__', '-I.', '-Isrc', '-Imodules',
                             f'tests/chao/{name}.c', '-lm')
                subprocess.run([program], check=True)


# The task scheduler retains these callbacks' addresses. Their frame simulation
# and renderer need the rest of the game. Keep the constructor, init step and
# destructor of each task intact, with its actual private record definitions.
FRAME_CALLBACKS = {
    'actor': ('chao_actor_step', 'chao_actor_draw'),
    'cocoon': ('cocoon_step', 'cocoon_draw'),
    'drive': ('drive_step', 'drive_draw'),
    'egg': ('egg_step', 'egg_draw', 'egg_shell_top_step', 'egg_shell_top_draw'),
    'garden_ball': ('ball_step', 'ball_draw'),
    'minimal': ('minimal_step', 'minimal_draw'),
    'world': ('garden_control_step', 'garden_control_draw'),
}


class ChaoConstructors(unittest.TestCase):
    """Exercise the portable Chao constructors and retry their garden callers."""

    def test_failure_cleanup_and_retry(self):
        with tempfile.TemporaryDirectory() as directory:
            scratch = Path(directory)
            for member in ('drive', 'minimal'):
                subprocess.run([sys.executable, 'tools/body_data.py', '--stub', 'chao',
                                f'modules/chao/{member}.c', scratch], cwd=ROOT, check=True)
            subprocess.run([sys.executable, 'tools/body_data.py', '--stub', '1st_read',
                            'src/hud/item_stock.c', scratch], cwd=ROOT, check=True)
            sources = []
            for name, callbacks in FRAME_CALLBACKS.items():
                text = (ROOT / 'modules' / 'chao' / (name + '.c')).read_text()
                blocks = [block for block in parse(text).blocks if block.name in callbacks]
                self.assertCountEqual([block.name for block in blocks], callbacks)
                for block in reversed(blocks):
                    text = text[:block.opening.end] + text[block.closing.start:]
                source = scratch / (name + '.c')
                source.write_text(text)
                sources.append(source)
            # The resident spawn table requires disc assets. Only its three
            # packing/clearing functions participate in the lifetime protocol.
            resident_source = (ROOT / 'modules/chao/residents.c').read_text()
            packed = ('garden_clear_chao', 'garden_pack_chao', 'garden_pack_egg')
            blocks = [b for b in parse(resident_source).blocks if b.name in packed]
            self.assertCountEqual([b.name for b in blocks], packed)
            residents = scratch / 'residents.c'
            residents.write_text('#include <string.h>\n#include "chao/residents.h"\n'
                                 '#include "chao/work.h"\n#include "system/task.h"\n' +
                                 '\n'.join(resident_source[b.head[0].start:b.closing.end] for b in blocks))
            # Only allocation and release of the shared trail participate here.
            shared = (ROOT / 'modules/shared/common_object.c').read_text()
            trail_ops = ('trail_alloc', 'trail_free')
            blocks = [b for b in parse(shared).blocks if b.name in trail_ops]
            self.assertCountEqual([b.name for b in blocks], trail_ops)
            trails = scratch / 'trails.c'
            trails.write_text('#include <shinobi.h>\n#include "shared/common_object.h"\n' +
                              '\n'.join(shared[b.head[0].start:b.closing.end] for b in blocks))
            sources.append(trails)
            program = scratch / 'chao'
            compile_host(program, '-std=c89', '-O2', '-Dinline=__inline__', '-D_builtin_fabsf=__builtin_fabsf', '-I.',
                         '-Isrc', '-Imodules', '-I', scratch / 'modules', '-I', scratch / 'src', *sources,
                         'src/system/task.c', 'src/object/mover.c', 'src/object/fall.c', 'src/collision/collision.c',
                         'modules/chao/world_slots.c', 'src/hud/item_stock.c', residents, 'tests/chao/constructors.c',
                         '-lm')
            subprocess.run([program], check=True)


class HeadgearSources(unittest.TestCase):
    """Exercise worn hats and eggshells across failed pickup admission."""

    def test_hats_and_shells_survive_rejected_drops(self):
        with tempfile.TemporaryDirectory() as directory:
            scratch = Path(directory)
            binary = scratch / 'headgear'
            # Keep the hatch/break state machine intact. Its draw callbacks
            # normally pull the entire stage renderer into the linked image.
            source = (ROOT / 'modules/chao/egg.c').read_text()
            renderers = ('egg_draw', 'egg_shell_top_draw')
            blocks = [block for block in parse(source).blocks if block.name in renderers]
            self.assertCountEqual([block.name for block in blocks], renderers)
            for block in reversed(blocks):
                source = source[:block.opening.end] + source[block.closing.start:]
            (scratch / 'chao').mkdir()
            (scratch / 'chao/egg.c').write_text(source)
            compile_host(binary, '-std=c89', '-O2', '-Dinline=__inline__', '-D_builtin_fabsf=__builtin_fabsf',
                         '-I', scratch, '-Isrc', '-Imodules', 'tests/chao/headgear_sources.c', '-lm')
            subprocess.run([binary], check=True)


class ChaoMatrixStack(unittest.TestCase):
    """A Chao can retry its body pose and draw after its caller releases matrices."""

    def test_body_pose_draw_and_special_models(self):
        chao = Path('modules/chao')
        with tempfile.TemporaryDirectory() as directory:
            program = Path(directory) / 'chao-matrices'
            compile_host(program, '-std=c89', '-Dinline=__inline__', '-O2', '-Imodules', '-Isrc',
                         chao / 'actor.c', chao / 'body_pose.c', chao / 'body_pose_work.c', chao / 'body_draw.c',
                         'tests/chao/matrix_stack.c')
            subprocess.run([program], check=True)


class GardenPickupSources(unittest.TestCase):
    """Exercise the saved tree and VMU transfer that supply garden pickups."""

    def compile_and_run(self, directory, units, fixture, dependencies=()):
        sources = []
        for name, callbacks in units.items():
            source = (ROOT / 'modules/chao' / (name + '.c')).read_text()
            blocks = [block for block in parse(source).blocks if block.name in callbacks]
            self.assertCountEqual([block.name for block in blocks], callbacks)
            for block in reversed(blocks):
                source = source[:block.opening.end] + source[block.closing.start:]
            path = directory / (name + '.c')
            path.write_text(source)
            sources.append(path)
        # The fixture sees the real private records and callbacks.
        with sources[0].open('a') as unit:
            unit.write(f'\n#include "{fixture}"\n')
        program = directory / 'pickup-source'
        compile_host(program, '-std=c89', '-O2', '-Dinline=__inline__', '-D_builtin_fabsf=__builtin_fabsf', '-I.',
                     '-Imodules', '-Isrc', '-I', directory / 'modules', '-Itests/chao', *sources, *dependencies, '-lm')
        subprocess.run([program], check=True)

    def test_saved_tree_drop(self):
        with tempfile.TemporaryDirectory() as directory:
            scratch = Path(directory)
            subprocess.run([sys.executable, 'tools/body_data.py', '--stub', 'chao',
                            'modules/chao/tree.c', scratch], cwd=ROOT, check=True)
            self.compile_and_run(scratch, {'tree': ('garden_tree_draw',),
                                           'fruit': ('fruit_step', 'fruit_draw')},
                                 'tree_pickup.c', ('src/system/task.c',
                                                   'src/object/fall.c', 'src/object/mover.c',
                                                   'src/collision/collision.c', 'modules/chao/world.c',
                                                   'modules/chao/world_slots.c'))

    def test_received_items(self):
        with tempfile.TemporaryDirectory() as directory:
            draws = ('draw_neutral_machine', 'draw_hero_machine', 'draw_dark_machine')
            self.compile_and_run(Path(directory), {'transfer_machine': draws}, 'transfer_pickup.c')


class GardenPickups(unittest.TestCase):
    """Exercise the portable garden pickup constructors through their first frames."""

    def test_deferred_initialization_and_retry(self):
        with tempfile.TemporaryDirectory() as directory:
            scratch = Path(directory)
            sources = []
            for name, prefix in (('fruit', 'fruit'), ('seed', 'seed'), ('headgear', 'headgear'),
                                 ('world', 'garden_control')):
                source = (ROOT / 'modules/chao' / (name + '.c')).read_text()
                # The frame movement and renderer require stage data. Leave
                # the installed callbacks addressable; retain the real init
                # and destructor, including their collision and registry work.
                callbacks = (prefix + '_step', prefix + '_draw')
                blocks = [block for block in parse(source).blocks if block.name in callbacks]
                self.assertCountEqual([block.name for block in blocks], callbacks)
                for block in reversed(blocks):
                    source = source[:block.opening.end] + source[block.closing.start:]
                unit = scratch / (name + '.c')
                unit.write_text(source)
                sources.append(unit)
            program = scratch / 'pickups'
            compile_host(program, '-std=c89', '-O2', '-Dinline=__inline__', '-D_builtin_fabsf=__builtin_fabsf', '-I.',
                         '-Isrc', '-Imodules', *sources, 'modules/chao/world_slots.c', 'src/system/task.c',
                         'src/object/mover.c', 'src/object/fall.c', 'src/collision/collision.c',
                         'tests/chao/pickups.c', '-lm')
            subprocess.run([program], check=True)


class RaceCameraFiles(unittest.TestCase):
    """Exercise the camera editor's normal course changes and retry paths."""

    def test_keep_live_keys_until_the_replacement_is_ready(self):
        with tempfile.TemporaryDirectory() as directory:
            program = Path(directory) / 'race-camera-files'
            compile_host(program, '-std=c89', '-O2', '-Isrc', '-Imodules', 'tests/chao/race_camera_files.c',
                         'modules/chaostgrace/editor.c', 'modules/chaostgrace/edit_menu.c')
            subprocess.run([program], check=True)


class GardenRegistry(unittest.TestCase):
    """Exercise live garden enumeration and save gathering with the portable game code."""

    def test_sparse_slots_and_shared_toy_storage(self):
        with tempfile.TemporaryDirectory() as directory:
            work = Path(directory)
            for member in ('garden_save', 'residents'):
                subprocess.run([sys.executable, 'tools/body_data.py', '--stub', 'chao',
                                f'modules/chao/{member}.c', work], cwd=ROOT, check=True)
            members = ('world', 'world_slots', 'garden_save', 'save_buffers', 'residents', 'race_state')
            program = work / 'garden'
            compile_host(program, '-std=c89', '-O2', '-D_builtin_fabsf=__builtin_fabsf', '-I.', '-Isrc', '-Imodules',
                         '-I', work / 'modules', *(f'modules/chao/{member}.c' for member in members),
                         'tests/chao/registry.c', '-lm')
            subprocess.run([program], check=True)


class GardenFile(unittest.TestCase):
    """Round-trip the garden writer's VMU payload and keep rejected reads out of live storage."""

    def test_stage_unpack_and_protect_rejected_file(self):
        with tempfile.TemporaryDirectory() as directory:
            work = Path(directory)
            save = 'modules/chao/garden_save.c'
            subprocess.run([sys.executable, 'tools/body_data.py', 'chao', save, work, '--stub'],
                           cwd=ROOT, check=True)
            program = work / 'garden-file'
            compile_host(program, '-std=c89', '-O2', '-Isrc', '-Imodules', '-I', work / 'modules', save,
                         'modules/chao/save_buffers.c', 'modules/chao/resident_slots.c', 'tests/chao/save_file.c')
            subprocess.run([program], check=True)


class ChaoSceneLand(unittest.TestCase):
    """Exercise landscape acquisition and retry through the Chao scene admission helpers."""

    def test_scene_acquisition_and_retry(self):
        with tempfile.TemporaryDirectory() as temp:
            work = Path(temp)
            subprocess.run([sys.executable, 'tools/body_data.py', 'chaostglobby',
                            'modules/chaostglobby/lobby.c', work, '--stub'], cwd=ROOT, check=True)
            subprocess.run([sys.executable, 'tools/body_data.py', 'chaostgrace',
                            'modules/chaostgrace/scene_assets.c', work, '--stub'], cwd=ROOT, check=True)
            program = work / 'admission'
            compile_host(program, '-std=c89', '-O2', '-I.', '-Isrc', '-Imodules', '-I', work / 'modules',
                         'tests/chao/scene_land.c', 'modules/chaostgrace/scene_assets.c', 'modules/chao/movement.c',
                         '-lm')
            subprocess.run([program], check=True)


class ToyConstructors(unittest.TestCase):
    """Run the garden's toy allocation, replacement and teardown paths."""

    def test_allocation_failure_and_owner_order(self):
        with tempfile.TemporaryDirectory() as directory:
            scratch = Path(directory)
            subprocess.run([sys.executable, 'tools/body_data.py', 'chao', 'modules/chao/small_toys.c',
                            scratch, '--stub'], cwd=ROOT, check=True)
            sources = []
            # The scheduler retains frame callbacks. Only the book's opening
            # and closing steps are needed for these ownership checks.
            for name, frames in (('rocking_horse', ('horse_step', 'horse_draw')),
                                 ('small_toys', ('bubble_draw', 'bubble_step', 'book_draw'))):
                source = (ROOT / 'modules/chao' / (name + '.c')).read_text()
                blocks = [b for b in parse(source).blocks if b.name in frames]
                self.assertCountEqual([b.name for b in blocks], frames)
                for block in reversed(blocks):
                    source = source[:block.opening.end] + source[block.closing.start:]
                target = scratch / (name + '.c')
                target.write_text(source)
                sources.append(target)
            program = scratch / 'toys'
            compile_host(program, '-std=c89', '-O2', '-Dinline=__inline__', '-D_builtin_fabsf=__builtin_fabsf', '-I.',
                         '-Isrc', '-Imodules', '-I', scratch / 'modules', *sources, 'src/system/task.c',
                         'src/object/mover.c', 'src/collision/collision.c', 'src/draw/motion.c', 'modules/chao/world.c',
                         'modules/chao/world_slots.c', 'modules/chao/behavior.c', 'tests/chao/toy_constructors.c',
                         '-lm')
            subprocess.run([program], check=True)


class GardenFields(unittest.TestCase):
    """Exercise garden fields through their task, collision and SET entry points."""

    def test_field_lifetime_and_placement_retry(self):
        with tempfile.TemporaryDirectory() as directory:
            program = Path(directory) / 'fields'
            compile_host(program, '-std=c99', '-O2', '-D_builtin_fabsf=__builtin_fabsf', '-I.', '-Isrc', '-Imodules',
                         'modules/chao/garden_field.c', 'src/system/task.c', 'src/collision/collision.c',
                         'tests/chao/garden_fields.c', '-lm')
            subprocess.run([program], check=True)


class KinderComposition(unittest.TestCase):
    """The Kindergarten's kana conversion steps through each phrase of its reading and back."""

    def test_conversion_steps_through_each_phrase(self):
        with tempfile.TemporaryDirectory() as directory:
            work = Path(directory)
            subprocess.run([sys.executable, 'tools/body_data.py', 'chaostgkinder',
                            'modules/chaostgkinder/composition.c', work, '--stub'], cwd=ROOT, check=True)
            program = work / 'composition'
            compile_host(program, '-std=c89', '-O2', '-fsanitize=address,bounds', '-fno-sanitize-recover=all', '-Isrc',
                         '-Imodules', '-I', work / 'variants/modules/chaostgkinder/composition',
                         'modules/chaostgkinder/composition.c', 'modules/chaostgkinder/wide_text.c',
                         'tests/chao/kinder_composition.c')
            subprocess.run([program], check=True)


class KinderStrings(unittest.TestCase):
    """Load, reject and retry the Kindergarten's indexed keyboard strings."""

    def test_rejects_invalid_banks_and_clears_widget_pointers(self):
        with tempfile.TemporaryDirectory() as directory:
            program = Path(directory) / 'strings'
            compile_host(program, '-std=c89', '-O2', '-Isrc', '-Imodules', 'modules/chaostgkinder/strings.c',
                         'tests/chao/kinder_strings.c')
            subprocess.run([program], check=True)


class KinderWidgets(unittest.TestCase):
    """A Kindergarten container and entry frame free what they allocate."""

    def test_failed_creates_and_destroys_free_every_block(self):
        with tempfile.TemporaryDirectory() as directory:
            program = Path(directory) / 'widgets'
            compile_host(program, '-std=c89', '-O2', '-Isrc', '-Imodules', 'modules/chaostgkinder/container.c',
                         'modules/chaostgkinder/entry_frame.c', 'tests/chao/kinder_widgets.c')
            subprocess.run([program], check=True)


class RaceFieldSignals(unittest.TestCase):
    """Run the race's tree and answer-lane actors with an exhausted field allocator."""

    def test_field_retries(self):
        with tempfile.TemporaryDirectory() as directory:
            scratch = Path(directory)
            for member in ('tree', 'hunt_data'):
                subprocess.run([sys.executable, 'tools/body_data.py', '--stub', 'chaostgrace',
                                f'modules/chaostgrace/{member}.c', scratch], cwd=ROOT, check=True)
            guest = scratch / 'race-fields'
            compile_host(guest, '-std=c89', '-O2', '-Dinline=__inline__', '-Isrc', '-Imodules',
                         '-I', scratch / 'modules', 'tests/chao/race_field_signals.c',
                         'modules/chaostgrace/obstacles.c', 'modules/chaostgrace/hunt_data.c', 'modules/chao/world.c',
                         'modules/chao/world_slots.c', '-lm')
            for case in ('normal-tree', 'normal-answer', 'entrant', 'fruit', 'answer'):
                with self.subTest(case=case):
                    subprocess.run([guest, case], check=True)


class RaceItems(unittest.TestCase):
    """Exercise race pickup ownership and recovery through the course callbacks."""

    def test_lifetimes_and_scarcity(self):
        with tempfile.TemporaryDirectory() as directory:
            scratch = Path(directory)
            for member in ('tree', 'hunt_data', 'hunt_toy'):
                subprocess.run([sys.executable, 'tools/body_data.py', '--stub', 'chaostgrace',
                                f'modules/chaostgrace/{member}.c', scratch], cwd=ROOT, check=True)
            sources = []
            # These callbacks need course meshes or the world's terrain.
            # Keep the original task walk, game logic, movers and collisions.
            for member, callbacks in (
                    ('modules/chaostgrace/fruit.c', ('fruit_draw',)),
                    ('modules/chaostgrace/hunt_toy.c', ('scavenger_item_draw',)),
                    ('modules/chaostgrace/scavenger.c', ('question_draw', 'answer_draw', 'penalty_draw')),
                    ('modules/chaostgrace/tree.c', ('tree_draw',)),
                    ('src/object/mover.c', ('mover_meet_stage',))):
                source = (ROOT / member).read_text()
                blocks = [block for block in parse(source).blocks if block.name in callbacks]
                self.assertCountEqual([block.name for block in blocks], callbacks)
                for block in reversed(blocks):
                    source = source[:block.opening.end] + source[block.closing.start:]
                unit = scratch / Path(member).name
                unit.write_text(source)
                sources.append(unit)
            program = scratch / 'race-items'
            compile_host(program, '-std=c89', '-O2', '-Dinline=__inline__', '-D_builtin_fabsf=__builtin_fabsf', '-I.',
                         '-Isrc', '-Imodules', '-I', scratch / 'modules', *sources, 'modules/chaostgrace/hunt_data.c',
                         'modules/chaostgrace/obstacles.c', 'modules/chao/world.c', 'modules/chao/world_slots.c',
                         'src/system/task.c', 'src/object/fall.c', 'src/collision/collision.c',
                         'tests/chao/race_items.c', '-lm')
            subprocess.run([program], check=True)


if __name__ == '__main__':
    unittest.main()
