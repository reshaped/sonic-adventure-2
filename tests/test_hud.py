"""What play prints over the screen, as the host runs its C: the harnesses of tests/hud/ and tests/otherprint/."""

import subprocess
import sys
import tempfile
import unittest
from pathlib import Path

from support import ROOT, compile_host


class BossTitle(unittest.TestCase):
    """Exercise the shared boss title's empty, boundary and ordinary layouts."""

    def test_counts_and_first_letter(self):
        with tempfile.TemporaryDirectory() as directory:
            work = Path(directory)
            program = work / 'boss_title'
            subprocess.run([sys.executable, 'tools/body_data.py', 'boss_last1', 'modules/shared/boss_title.c',
                            work, '--stub'], cwd=ROOT, check=True)
            compile_host(program, '-std=c89', '-O2', '-fno-strict-aliasing', '-I', work / 'modules', '-Isrc',
                         '-Imodules', 'tests/hud/boss_title.c', 'modules/shared/boss_title.c', 'src/system/task.c')
            subprocess.run([program], check=True, timeout=10)


class MissionMessage(unittest.TestCase):
    """Check title-card caption admission and lifetime through the resident HUD."""

    def test_caption_load_and_retry(self):
        with tempfile.TemporaryDirectory() as directory:
            work = Path(directory)
            subprocess.run([sys.executable, 'tools/body_data.py', '1st_read',
                            'src/hud/mission_message.c', work, '--stub'], cwd=ROOT, check=True)
            exe = work / 'mission'
            compile_host(exe, '-std=gnu17', '-O2', '-Isrc', '-Imodules', '-I', work / 'src',
                         'tests/hud/mission_message.c')
            subprocess.run([exe], check=True)


class Presentations(unittest.TestCase):
    """Exercise the startup logos and ending through task and texture acquisition."""

    def test_admission_release_and_retry(self):
        owners = [Path('modules/otherprint') / name for name in ('logos.c', 'tento_ending.c')]
        with tempfile.TemporaryDirectory() as temp:
            directory = Path(temp)
            for owner in owners:
                subprocess.run([sys.executable, 'tools/body_data.py', '--stub', 'otherprint',
                                owner, directory], cwd=ROOT, check=True)
            for mode in ('empty', 'populated'):
                with self.subTest(mode=mode):
                    if mode == 'populated':
                        for owner, count in zip(owners, (11, 8)):
                            name = owner.stem
                            table = directory / owner.with_suffix('') / (name + '_texlist.inc')
                            table.write_text(f'{name}_texnames, {count}\n')
                    program = directory / 'presentations'
                    compile_host(program, '-std=c89', '-O2', '-Dinline=__inline__', '-I.', '-Isrc', '-Imodules',
                                 '-I', directory / 'modules', *owners, 'engine/dc/sh4zam_splash.c',
                                 'tests/otherprint/presentations.c')
                    subprocess.run([program, mode], check=True)


class TitleCard(unittest.TestCase):
    """Exercise stage and character selection of the title-card archive."""

    def test_stage_archive(self):
        with tempfile.TemporaryDirectory() as temp:
            work = Path(temp)
            subprocess.run([sys.executable, 'tools/body_data.py', '--stub', '1st_read',
                            'src/hud/title_card.c', work], cwd=ROOT, check=True)
            program = work / 'title-card'
            compile_host(program, '-std=c89', '-O2', '-I.', '-Isrc', '-Imodules', '-I', work / 'src',
                         'tests/hud/title_card.c')
            subprocess.run([program], check=True)


if __name__ == '__main__':
    unittest.main()
