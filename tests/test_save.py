"""The saves and the VMU, as the host runs their C: the harnesses of tests/save/, tests/backup/ and
tests/fibers/."""

import subprocess
import sys
import tempfile
import unittest
from pathlib import Path

from csource import parse
from support import ROOT, compile_host


class CardTransactions(unittest.TestCase):
    """Run the save/card callbacks with deterministic VMU and allocation results."""

    def test_completion_and_lifetime(self):
        with tempfile.TemporaryDirectory() as scratch:
            work = Path(scratch)
            for source in ('src/save/card.c', 'src/save/save.c'):
                subprocess.run([sys.executable, ROOT / 'tools/body_data.py', '1st_read',
                                source, work, '--stub'], cwd=ROOT, check=True)
            # The retail program reserves DEMO_AREA at a fixed address. Give
            # its normal download scanner a real writable area on this host.
            areas = (ROOT / 'src/system/areas.h').read_text()
            (work / 'system').mkdir()
            (work / 'system/areas.h').write_text(areas.replace(
                '#define DEMO_AREA ((Uint8 *)0x8cb00000)',
                'extern void *download_workspace[];\n#define DEMO_AREA ((Uint8 *)download_workspace)'))
            # Host collection, scheduling and rendering are supplied by the test device.
            sources = ('sdk/backup/vmsutil.c', 'src/save/card.c', 'src/save/save.c', 'tests/save/transactions.c')
            compile_host(work / 'test', '-std=gnu99', '-O2', '-fno-strict-aliasing', '-D_builtin_get_imask()=0',
                         '-D_builtin_set_imask(x)=((void)(x))', '-D_builtin_fabsf=__builtin_fabsf', '-I', work,
                         '-I', work / 'src', '-I.', '-Isrc', '-Imodules', *sources)
            subprocess.run([work / 'test'], check=True)


class Fibers(unittest.TestCase):
    """VMU fiber admission on the host."""

    def test_capacity_and_context(self):
        with tempfile.TemporaryDirectory() as temp:
            exe = Path(temp) / 'admission'
            compile_host(exe, '-std=c89', '-O2', '-ffreestanding', '-Wno-pointer-to-int-cast', 'tests/fibers/server.c',
                         sdk_first=True)
            subprocess.run([exe], check=True, timeout=10)


def function(path, name):
    source = (ROOT / path).read_text()
    block, = (b for b in parse(source).blocks if b.name == name)
    return source[block.head[0].start:block.closing.end]


class SaveRecords(unittest.TestCase):
    """Exercise stage and kart records through the game's save and HUD entry points."""

    def test_reference_and_portable_records(self):
        with tempfile.TemporaryDirectory() as directory:
            scratch = Path(directory)
            subprocess.run([sys.executable, 'tools/body_data.py', '--stub', '1st_read', 'src/save/save.c', scratch],
                           cwd=ROOT, check=True)
            # The HUD's renderer requires disc sprite data. Its initialization
            # and record lookup run unchanged with a stub frame callback.
            hud = scratch / 'hud.c'
            hud.write_text('#include "cart/race.h"\n#include "cart/course.h"\n'
                           '#include "system/task.h"\n#include "system/game_state.h"\n'
                           'static const char layout_name[] = "layoutExec";\n'
                           'static void hud_step(struct task *task) {}\n#ifdef NONMATCHING\n' +
                           function('modules/cart/race_clock.c', 'cart_time_record_available') +
                           '\n#endif\n' + function('modules/cart/race_hud.c', 'cart_start_hud'))
            for name, nonmatching in [('original', False), ('portable', True)]:
                with self.subTest(route=name):
                    exe = scratch / name
                    compile_host(exe, '-std=c89', '-O2', '-Dinline=__inline__', '-I.', '-Isrc', '-Imodules',
                                 '-I', scratch / 'src', 'src/save/save.c', 'tests/save/records.c', hud,
                                 nonmatching=nonmatching)
                    subprocess.run([exe], check=True)


class VmuDefrag(unittest.TestCase):
    """Defragment a card that holds a block no file starts at."""

    def test_lone_block(self):
        with tempfile.TemporaryDirectory() as directory:
            program = Path(directory) / 'defrag'
            compile_host(program, '-std=c89', '-O2', '-fsanitize=address,bounds', '-fno-sanitize-recover=all',
                         'tests/backup/defrag.c')
            subprocess.run([program], check=True)


class DriveFibers(unittest.TestCase):
    """Check the drive-to-fiber registration and rollback used by buInit."""

    def test_registration_and_retry(self):
        backup = ROOT / 'sdk/backup'
        source = (backup / 'vmscmd.c').read_text()
        # Starting the drive command loop would require the Maple filesystem.
        # Keep each real entry point and exercise its drive mapping once.
        dispatch, = (block for block in parse(source).blocks if block.name == 'vmsfs_c_dispatch')
        source = (source[:dispatch.opening.end] + '\n    observe_drive(drive);\n' +
                  source[dispatch.closing.start:])
        with tempfile.TemporaryDirectory() as directory:
            scratch = Path(directory)
            command = scratch / 'vmscmd.c'
            command.write_text('void observe_drive(long drive);\n' + source)
            executable = scratch / 'drive-fibers'
            compile_host(executable, '-std=c89', '-O2', '-I', backup, command, backup / 'vmsfib.c',
                         'tests/fibers/drives.c')
            subprocess.run([executable], check=True)


if __name__ == '__main__':
    unittest.main()
