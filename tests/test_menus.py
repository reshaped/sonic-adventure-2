"""The menus' screens, as the host runs their C: the harnesses of tests/advertise/."""

import subprocess
import sys
import tempfile
import unittest
from pathlib import Path

from support import ROOT, compile_host


class KartPreview(unittest.TestCase):
    """Check both drivers' displayed stats through the kart menu's actual step."""

    def test_second_pick_and_single_player_sentinel(self):
        with tempfile.TemporaryDirectory() as directory:
            work = Path(directory)
            subprocess.run([sys.executable, 'tools/body_data.py', '--stub', 'advertise',
                            'modules/advertise/kart_race_panel.c', work], cwd=ROOT, check=True)
            program = work / 'kart-preview'
            compile_host(program, '-std=c89', '-O2', '-fsanitize=bounds', '-fno-sanitize-recover=bounds', '-I.',
                         '-Isrc', '-Imodules', '-I', work / 'modules', 'tests/advertise/kart_preview.c')
            subprocess.run([program, '4'], check=True)


class KartTimes(unittest.TestCase):
    """Draw a course's best times in the kart race panel at the image's depths."""

    def test_three_times_draw_at_140_145_and_150(self):
        with tempfile.TemporaryDirectory() as directory:
            work = Path(directory)
            subprocess.run([sys.executable, 'tools/body_data.py', '--stub', 'advertise',
                            'modules/advertise/kart_race_panel.c', work], cwd=ROOT, check=True)
            program = work / 'kart-times'
            compile_host(program, '-std=c89', '-O2', '-I.', '-Isrc', '-Imodules', '-I', work / 'modules',
                         'tests/advertise/kart_times.c')
            subprocess.run([program], check=True)


class StoryChapters(unittest.TestCase):
    """Exercise all three chapter menus with and without their disc-backed directories."""

    def test_chapter_selection_and_missing_directory(self):
        owner = Path('modules/advertise/story_select.c')
        with tempfile.TemporaryDirectory() as directory:
            generated = Path(directory)
            subprocess.run([sys.executable, 'tools/body_data.py', '--stub', 'advertise',
                            owner, generated], cwd=ROOT, check=True)
            tables = generated / owner.with_suffix('')
            for mode in ('blank', 'populated'):
                with self.subTest(mode=mode):
                    if mode == 'populated':
                        for story, (name, count) in enumerate(
                                [('hero', 15), ('dark', 15), ('last', 4)]):
                            rows = [f'{{ {chapter * (story + 2)}, 0 }}'
                                    for chapter in range(count)]
                            (tables / f'{name}_chapters.inc').write_text(',\n'.join(rows))
                    program = generated / 'chapters'
                    compile_host(program, '-std=c89', '-O2', '-Dinline=__inline__', '-Isrc', '-Imodules',
                                 '-I', generated / 'modules', owner, 'tests/advertise/story_chapters.c')
                    subprocess.run([program, mode], check=True)


class VersusPairs(unittest.TestCase):
    """Read the versus select's pair for the random game and the stages game as the image does."""

    def test_games_without_a_pair_read_the_work_past_pairs(self):
        with tempfile.TemporaryDirectory() as directory:
            work = Path(directory)
            subprocess.run([sys.executable, 'tools/body_data.py', '--stub', 'advertise',
                            'modules/advertise/versus_select.c', work], cwd=ROOT, check=True)
            program = work / 'versus-pairs'
            compile_host(program, '-std=c89', '-O2', '-I.', '-Isrc', '-Imodules', '-I', work / 'modules',
                         'tests/advertise/versus_pairs.c')
            subprocess.run([program], check=True)


if __name__ == '__main__':
    unittest.main()
