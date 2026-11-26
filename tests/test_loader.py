"""What the game loads from the disc, and the events it plays, as the host runs their C: the harnesses of
tests/loader/, tests/event/, tests/events/ and tests/textures/."""

import struct
import subprocess
import sys
import tempfile
import unittest
from pathlib import Path

from support import ROOT, compile_host


class AnimalArchive(unittest.TestCase):
    """Check stage animal slot publication and texture release."""

    def test_load_draw_and_retry(self):
        with tempfile.TemporaryDirectory() as directory:
            generated = Path(directory) / 'generated'
            subprocess.run([sys.executable, ROOT / 'tools/body_data.py', '1st_read', 'src/object/animal.c',
                            generated, '--stub'], cwd=ROOT, check=True)
            program = Path(directory) / 'animal'
            compile_host(program, '-std=gnu17', '-O2', '-I.', '-I', generated / 'src', '-Isrc', '-Imodules',
                         'tests/loader/animal.c')
            subprocess.run([program], check=True)


class CameraLoader(unittest.TestCase):
    """Exercise a stage's packed and loose camera files through the regular port loader."""

    def test_stage_camera_files(self):
        with tempfile.TemporaryDirectory() as directory:
            exe = Path(directory) / 'camera'
            compile_host(exe, '-std=gnu17', '-O2', '-I.', '-Isrc', '-Imodules', 'tests/loader/camera.c',
                         'engine/dc/prs_checked.c')
            subprocess.run([exe], check=True)

    def test_motion_without_frames_keeps_the_current_camera(self):
        with tempfile.TemporaryDirectory() as directory:
            exe = Path(directory) / 'camera-motion'
            compile_host(exe, '-std=c89', '-O2', '-Isrc', '-Imodules', 'tests/loader/camera_motion.c')
            subprocess.run([exe], check=True)


class DiscRead(unittest.TestCase):
    """Read a GD-ROM file through the port's resident loader with a stand-in drive."""

    def test_reads_must_fit_whole_sectors_and_complete_before_allocation(self):
        with tempfile.TemporaryDirectory() as directory:
            program = Path(directory) / 'read'
            compile_host(program, '-std=gnu17', '-O2', '-I.', '-Isrc', '-Imodules', 'tests/loader/disc.c')
            subprocess.run([program], check=True)


class EventFog(unittest.TestCase):
    """Exercise optional stage fog reads and the event player's fog table."""

    def test_optional_read_and_scene_bounds(self):
        with tempfile.TemporaryDirectory() as directory:
            program = Path(directory) / 'fog'
            compile_host(program, '-std=gnu17', '-O2', '-Isrc', '-Imodules', 'tests/loader/fog.c')
            subprocess.run([program], check=True)


class EventLoading(unittest.TestCase):
    """Exercise scene admission, texture rollback and the load result seen by the event overlay."""

    def test_event_assets(self):
        with tempfile.TemporaryDirectory() as directory:
            d = Path(directory)
            exe = d / 'event-loader'
            subprocess.run([sys.executable, ROOT / 'tools/body_data.py', 'event', 'modules/event/loader.c',
                            d, '--stub'], cwd=ROOT, check=True, timeout=10)
            compile_host(exe, '-std=c89', '-O2', '-I.', '-Isrc', '-Imodules', '-I', d / 'modules',
                         'tests/event/loader.c')
            subprocess.run([exe], check=True, timeout=10)


class MoviePlane(unittest.TestCase):
    """Fullscreen movie failures, optional plane loading, and resource release."""

    def test_movie_and_optional_plane(self):
        with tempfile.TemporaryDirectory() as directory:
            program = Path(directory) / 'movie-plane'
            compile_host(program, '-std=c89', '-O2', '-fno-strict-aliasing', '-Isrc', '-Imodules',
                         'tests/events/movie_plane.c')
            subprocess.run([program], check=True, timeout=10)


def write_strings(path, count, label, sentinel=False):
    width = (count + int(sentinel)) * 4
    offsets = [width] * count
    if sentinel:
        offsets.append(-1)
    body = struct.pack(f'<{len(offsets)}i', *offsets) + label + b'\0'
    body += b'\0' * (-len(body) % 8)
    # Eight literals per control byte, then the normal PRS end marker.
    path.write_bytes(b''.join(b'\xff' + body[i:i + 8] for i in range(0, len(body), 8)) + b'\x02\0\0')


class LanguageText(unittest.TestCase):
    """Exercise the resident text and warning files together through the disc loader."""

    def test_load_replace_and_release(self):
        with tempfile.TemporaryDirectory() as directory:
            d = Path(directory)
            for language in ('e', 'j'):
                write_strings(d / f'text_{language}.prs', 144, f'action {language}'.encode())
                write_strings(d / f'mcwarn_{language}.prs', 8, f'notice {language}'.encode(), sentinel=True)
            program = d / 'text'
            compile_host(program, '-std=gnu17', '-O2', '-I.', '-Isrc', '-Imodules', 'tests/loader/text.c',
                         'engine/dc/prs_checked.c', 'src/system/text.c')
            subprocess.run([program], check=True, cwd=d, timeout=10)


class MiniEventAdmission(unittest.TestCase):
    """Check the resident mini-event loader through normal, absent-file and retry paths."""

    def test_scene_and_optional_language_files(self):
        with tempfile.TemporaryDirectory() as directory:
            program = Path(directory) / 'read'
            compile_host(program, '-std=c89', '-O2', '-Isrc', '-Imodules', 'tests/loader/mini_event.c')
            subprocess.run([program], check=True)


class OverlayAdmission(unittest.TestCase):
    """Check executable-image admission and cache publication through the resident loader."""

    def test_complete_image_precedes_callback_publication(self):
        with tempfile.TemporaryDirectory() as directory:
            program = Path(directory) / 'read'
            compile_host(program, '-std=gnu17', '-O2', '-I.', '-Isrc', '-Imodules', 'tests/loader/overlay.c',
                         'engine/dc/prs_checked.c', 'engine/dc/crash_report.c')
            subprocess.run([program], check=True)


class SoundFiles(unittest.TestCase):
    """Drive the sound file entry points with a local disc and synchronous transfers."""

    def test_driver_units_and_voice_banks(self):
        with tempfile.TemporaryDirectory() as directory:
            d = Path(directory)
            driver = b'SDRV' + bytes(4) + struct.pack('<I', 64) + bytes(4) + bytes([2]) + bytes(47)
            unit = b'SMLT' + bytes([2]) + bytes(27)
            packed = b''.join(b'\xff' + unit[i:i + 8] for i in range(0, len(unit), 8)) + b'\x02\0\0'
            (d / 'manatee.drv').write_bytes(driver)
            (d / 'se_ch_ss.prs').write_bytes(packed)
            (d / 'se_ch_ss.mlt').write_bytes(unit + bytes(32))
            (d / 'e_test.mlt').write_bytes(unit + bytes(32))
            (d / 'e_test.prs').write_bytes(packed)
            (d / 'b03_sn.mpb').write_bytes(b'SMPB' + bytes(28))
            for source in ('src/sound/lists.c', 'src/sound/sound.c'):
                subprocess.run([sys.executable, ROOT / 'tools/body_data.py', '1st_read', source,
                                d, '--stub'], cwd=ROOT, check=True, timeout=10)
            # Name the two banks on this test disc in the generated sound registry.
            with (d / 'src/sound/lists/text.inc').open('a') as names:
                names.write('#undef EFFECTS_SONIC_SHADOW_MLT\n#define EFFECTS_SONIC_SHADOW_MLT "se_ch_ss.mlt"\n'
                            '#undef VOICES_SONIC_JP\n#define VOICES_SONIC_JP "b03_sn.mpb"\n')
            program = d / 'sound_files'
            compile_host(program, '-std=gnu17', '-O2', '-D_builtin_fabsf=__builtin_fabsf', '-I', d / 'src', '-I.',
                         '-Isrc', '-Imodules', 'tests/loader/sound_files.c', 'engine/dc/prs_checked.c',
                         'src/sound/lists.c')
            subprocess.run([program], check=True, cwd=d, timeout=10)


class StageLights(unittest.TestCase):
    """Exercise stage lighting through the resident optical reader."""

    def test_complete_read_replaces_lights(self):
        with tempfile.TemporaryDirectory() as directory:
            program = Path(directory) / 'lights'
            compile_host(program, '-std=gnu17', '-O2', '-D_builtin_fabsf=__builtin_fabsf', '-I.', '-Isrc',
                         '-Imodules', 'tests/loader/stage_lights.c')
            subprocess.run([program], check=True)


METAL_HARBOR_IMAGE = ROOT / 'build/orig/SONIC2/STG10.BIN'


class StageSets(unittest.TestCase):
    """Exercise stage SET publication through the resident reader."""

    def run_fixture(self, source, *extra_args):
        with tempfile.TemporaryDirectory() as directory:
            program = Path(directory) / 'sets'
            compile_host(program, '-std=gnu17', '-O2', '-Isrc', '-Imodules', Path('tests/loader') / source,
                         *extra_args)
            subprocess.run([program], check=True)

    def test_pair_and_single_file_admission(self):
        self.run_fixture('sets.c')

    def test_placement_capacity_and_current_kinds(self):
        self.run_fixture('placements.c', '-lm')

    def test_a_placed_task_whose_place_holds_a_nan_stays(self):
        self.run_fixture('placement_leave.c', '-lm')

    def check_metal_harbor_directory(self, original=None):
        head = 'modules/stg10/head.c'
        with tempfile.TemporaryDirectory() as directory:
            generated = Path(directory)
            mode = ['--original', original] if original else ['--stub']
            subprocess.run([sys.executable, ROOT / 'tools/body_data.py', *mode, 'stg10', head, generated],
                           cwd=ROOT, check=True)
            expected = 'METAL_HARBOR_KINDS' if original else '0'
            self.run_fixture('set_directory.c', f'-DEXPECTED_COUNT={expected}', '-I', generated / 'modules',
                             'src/object/set.c', '-lm')

    def test_stub_directory_admits_no_kinds(self):
        self.check_metal_harbor_directory()

    @unittest.skipUnless(METAL_HARBOR_IMAGE.is_file(), 'requires the decoded Metal Harbor overlay')
    def test_original_directory_admits_all_kinds(self):
        self.check_metal_harbor_directory(METAL_HARBOR_IMAGE)


class TextureLoading(unittest.TestCase):
    """Exercise the portable PVM loader through its file, memory, and shared-list APIs."""

    def test_publication_and_release(self):
        sources = ['engine/dc/pvm.c', 'src/system/texture.c', 'sdk/ninja/njCheckTextureFormat.c',
                   'sdk/ninja/njTextureGlobal.c', 'sdk/ninja/njReleaseTexture.c',
                   'tests/textures/publication.c']
        with tempfile.TemporaryDirectory() as directory:
            program = Path(directory) / 'texture-loading'
            subprocess.run([sys.executable, 'tools/body_data.py', '--stub', '1st_read',
                            'src/system/texture.c', directory], cwd=ROOT, check=True)
            compile_host(program, '-std=c89', '-O2', '-I', Path(directory) / 'src', '-Itests/textures', '-I.',
                         '-Isrc', *sources)
            for case in ('bounds', 'upload', 'ownership', 'file', 'allocation', 'room', 'set', 'shared'):
                with self.subTest(case=case):
                    subprocess.run([program, case], check=True, timeout=10)


@unittest.skipIf(sys.platform == 'darwin', 'macOS maps no data below 4 GB, where a texaddr word must point')
class TextureSelection(unittest.TestCase):
    """The portable Ninja texture selector preserves the previous material on failure."""

    def test_unavailable_and_loaded_slots(self):
        with tempfile.TemporaryDirectory() as directory:
            program = Path(directory) / 'texture-selection'
            compile_host(program, '-std=c89', '-O2', '-no-pie', '-include', 'config/intrinsics.h',
                         'sdk/ninja/njTexture.c', 'tests/textures/selection.c')
            subprocess.run([program], check=True)


if __name__ == '__main__':
    unittest.main()
