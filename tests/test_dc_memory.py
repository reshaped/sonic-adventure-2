"""Hold the GCC images, and the sources that fix RAM's other owners, to the memory contract."""

from pathlib import Path
import subprocess
from types import SimpleNamespace
import sys
import unittest

from dc_memory import SOURCES, Extent, Owner, contract, demo_state, extent, numbers, problems, state
from support import ROOT, SH_ELF, TOOLS, TreeTest

# extents as make dc-images once linked the images, each as
# (entry, start, end, land), in the 0x0c view
LINKED = {
    '1st_read': Extent(0x0c010000, 0x0c010000, 0x0c2b6a40, None),
    'stg13': Extent(0x0c500000, 0x0c500000, 0x0c73f224, 0x0c73f240),
    'chao': Extent(0x0c500000, 0x0c500000, 0x0c673948, None),
    'chaostgrace': Extent(0x0c9c0000, 0x0c9c0000, 0x0ca2db1c, None),
    'chaostglobby': Extent(0x0c9c0000, 0x0c9c0000, 0x0c9f4394, None),
}
# the plants: the camera areas run on into the heap, and the race and lobby
# compile their land over the race's image
CAMERAS_IN_THE_HEAP = ('src/system/areas.h', '#define CAMLIN_AREA_END ((Uint8 *)0x8cc00000)',
                       '#define CAMLIN_AREA_END ((Uint8 *)0x8cc08000)')
LAND_IN_THE_RACE = ('src/system/areas.h', '#define CHAO_LAND_BUILD_AREA ((Uint8 *)0x8caa0000)',
                    '#define CHAO_LAND_BUILD_AREA ((Uint8 *)0x8ca20000)')
# a demo file with room up to player 0's camera area, past CAMSET_AREA
DEMO_FILE_IN_THE_CAMERAS = ('src/system/areas.h', '(uintptr_t)CAMSET_AREA - (uintptr_t)DEMO_AREA',
                            '(uintptr_t)CAMSET_AREA_0 - (uintptr_t)DEMO_AREA')


class Sources(TreeTest):
    """Runs in a copy of the sources the contract reads, which a test can plant an owner in."""

    def setUp(self):
        super().setUp()
        for path in SOURCES:
            self.write(path, (ROOT / path).read_bytes())

    def plant(self, path, old, new):
        text = Path(path).read_text()
        self.assertEqual(text.count(old), 1, old)
        self.write(path, text.replace(old, new))


class Contract(Sources):
    def test_the_linked_images_hold_to_it(self):
        terms = contract()
        for image in LINKED:
            with self.subTest(image=image):
                self.assertEqual(problems(image, LINKED, terms), [])

    def test_each_state_holds_the_owners_its_loader_fills(self):
        terms = contract()
        owners = {image: {owner.name for owner in state(image, LINKED, terms)[1]} for image in LINKED}
        self.assertIn('CONFIG_WORK', owners['1st_read'])
        self.assertLessEqual({'STAGE_MODEL_BUILD_AREA', 'DEMO_AREA', 'CAMLIN_AREA_1'}, owners['stg13'])
        self.assertLessEqual({'CHAO_RACE_LAND_AREA', 'CHAO_LAND_BUILD_AREA', 'chao'}, owners['chaostgrace'])
        self.assertLessEqual({'CHAO_LOBBY_LAND_AREA', 'CHAO_LAND_BUILD_AREA'}, owners['chaostglobby'])
        self.assertNotIn('CHAO_RACE_LAND_AREA', owners['chaostglobby'])
        self.assertNotIn('DEMO_AREA', owners['chao'])
        self.assertNotIn('demo file', owners['stg13'])

    def test_a_stage_in_a_demo_holds_its_file_over_the_mini_events_areas(self):
        terms = contract()
        # the disc's six demo files each decode to 0xe0000 bytes
        self.assertEqual(terms.owners['demo file'], Owner('demo file', 0x0cb00000, 0x0cbe0000))
        name, held = demo_state('stg13', LINKED, terms)
        self.assertEqual(name, 'stg13, in a demo')
        owners = {owner.name for owner in held}
        self.assertLessEqual({'stg13', 'STAGE_MODEL_BUILD_AREA', 'demo file', 'CAMSET_AREA', 'CAMLIN_AREA_1'}, owners)
        self.assertFalse({'DEMO_AREA', 'MESSAGE_AREA', 'SCRIPT_AREA', 'REQUEST_AREA'} & owners)
        for image in ('1st_read', 'chao', 'chaostgrace'):
            with self.subTest(image=image):
                self.assertIsNone(demo_state(image, LINKED, terms))

    def test_a_demo_file_in_the_camera_areas_fails(self):
        self.plant(*DEMO_FILE_IN_THE_CAMERAS)
        self.assertEqual(problems('stg13', LINKED, contract()),
                         ['demo file, 0x0cb00000 to 0x0cbf0000, overlaps CAMSET_AREA, 0x0cbe0000 to 0x0cbf0000'])

    def test_no_state_holds_config_work_with_the_motion_archive_it_lies_in(self):
        terms = contract()
        boot = {owner.name: owner for owner in state('1st_read', LINKED, terms)[1]}
        archive = terms.owners['CHAO_MOTION_SLOT']
        self.assertTrue(archive.start < boot['CONFIG_WORK'].start < archive.end)
        self.assertEqual(boot['CONFIG_WORK'].end, terms.owners['heap'].start)
        for image in LINKED:
            with self.subTest(image=image):
                names = {owner.name for owner in state(image, LINKED, terms)[1]}
                self.assertFalse({'CONFIG_WORK', 'CHAO_MOTION_SLOT'} <= names)

    def test_config_work_in_the_main_executable_fails(self):
        self.plant('src/system/areas.h', '#define CONFIG_WORK ((void *)0x8c900000)',
                   '#define CONFIG_WORK ((void *)0x8c200000)')
        self.assertEqual(problems('1st_read', LINKED, contract()),
                         ['1st_read, 0x0c010000 to 0x0c2b6a40, overlaps CONFIG_WORK, 0x0c200000 to 0x0cc00000'])

    def test_a_camera_area_in_the_heap_fails(self):
        self.plant(*CAMERAS_IN_THE_HEAP)
        self.assertEqual(problems('stg13', LINKED, contract()),
                         ['CAMLIN_AREA_1, 0x0cbfc000 to 0x0cc08000, overlaps heap, 0x0cc00000 to 0x0d000000'])

    def test_player_1s_camera_area_takes_player_0s_size(self):
        self.plant('src/system/areas.h', '#define CAMLIN_AREA_1 ((Uint8 *)0x8cbfc000)',
                   '#define CAMLIN_AREA_1 ((Uint8 *)0x8cbfe000)')
        self.assertEqual(problems('stg13', LINKED, contract()),
                         ['CAMLIN_AREA_1, 0x0cbfe000 to 0x0cc04000, overlaps heap, 0x0cc00000 to 0x0d000000'])

    def test_the_land_build_area_in_the_race_fails(self):
        self.plant(*LAND_IN_THE_RACE)
        terms = contract()
        self.assertEqual(problems('chaostgrace', LINKED, terms),
                         ['CHAO_RACE_LAND_AREA: ends at 0x0ca20000, before it starts at 0x0ca40000',
                          'chaostgrace, 0x0c9c0000 to 0x0ca2db1c, overlaps CHAO_LAND_BUILD_AREA, '
                          '0x0ca20000 to 0x0cb00000'])
        self.assertEqual(problems('chaostglobby', LINKED, terms),
                         ['CHAO_LOBBY_LAND_AREA: ends at 0x0ca20000, before it starts at 0x0ca80000'])

    def test_a_chao_area_past_its_limit_fails(self):
        race = dict(LINKED, chaostgrace=LINKED['chaostgrace']._replace(end=0x0ca40020))
        self.assertEqual(problems('chaostgrace', race, contract()),
                         ['chaostgrace: it ends at 0x0ca40020, past its limit in area_files[], CHAO_RACE_LAND_AREA, '
                          '0x0ca40000',
                          'chaostgrace, 0x0c9c0000 to 0x0ca40020, overlaps CHAO_RACE_LAND_AREA, '
                          '0x0ca40000 to 0x0caa0000'])

    def test_the_race_holds_its_land_file_whatever_its_limit(self):
        self.plant('modules/chao/garden.c', '(uintptr_t)CHAO_RACE_LAND_AREA}', '(uintptr_t)MESSAGE_AREA}')
        race = dict(LINKED, chaostgrace=LINKED['chaostgrace']._replace(end=0x0ca40020))
        self.assertEqual(problems('chaostgrace', race, contract()),
                         ['chaostgrace, 0x0c9c0000 to 0x0ca40020, overlaps CHAO_RACE_LAND_AREA, '
                          '0x0ca40000 to 0x0caa0000'])

    def test_an_image_two_rows_name_takes_the_lower_limit(self):
        self.plant('modules/chao/garden.c', '{"ChaoStgLobby.PRS",',
                   '{"ChaoStgRace.PRS", "ALSTRACE", (uintptr_t)MESSAGE_AREA},\n    {"ChaoStgLobby.PRS",')
        self.assertEqual(contract().limits['chaostgrace'].name, 'CHAO_RACE_LAND_AREA')

    def test_an_owner_past_ram_fails(self):
        self.plant('src/system/areas.h', '#define CAMLIN_AREA_0 ((Uint8 *)0x8cbf8000)',
                   '#define CAMLIN_AREA_0 ((Uint8 *)0x8cffffd0)')
        self.plant('src/system/areas.h', '#define CAMLIN_AREA_1 ((Uint8 *)0x8cbfc000)',
                   '#define CAMLIN_AREA_1 ((Uint8 *)0x8cfffff0)')
        self.plant('src/system/areas.h', '#define CAMLIN_AREA_END ((Uint8 *)0x8cc00000)',
                   '#define CAMLIN_AREA_END ((Uint8 *)0x8d000000)')
        self.assertIn('CAMLIN_AREA_1, 0x0cfffff0 to 0x0d000010, holds bytes outside main RAM',
                      problems('stg13', LINKED, contract()))

    def test_a_reversed_camera_area_fails_before_it_widens(self):
        self.plant('src/system/areas.h', '#define CAMLIN_AREA_END ((Uint8 *)0x8cc00000)',
                   '#define CAMLIN_AREA_END ((Uint8 *)0x8cbfa000)')
        self.assertEqual(problems('stg13', LINKED, contract()),
                         ['CAMLIN_AREA_1: ends at 0x0cbfa000, before it starts at 0x0cbfc000'])

    def test_chao_with_only_its_archives_fails(self):
        terms = contract()
        archives = [terms.owners[name][1:] for name in ('CHAO_MODEL_SLOT', 'CHAO_MOTION_SLOT')]
        elf = SimpleNamespace(path='chao.elf', loads=archives, defined={}, entry=0x8c500000)
        with self.assertRaisesRegex(ValueError, '^chao.elf: holds no LOAD segment but the Chao archives$'):
            extent('chao', elf, terms)

    def test_an_image_away_from_its_loader_fails(self):
        moved = dict(LINKED, chaostglobby=LINKED['chaostglobby']._replace(entry=0x0c9c1000))
        self.assertEqual(problems('chaostglobby', moved, contract()),
                         ['chaostglobby: its entry is 0x0c9c1000, and its loader reads it to CHAO_AREA_SLOT, '
                          '0x0c9c0000'])

    def test_work_area_file_outside_work_area_fails(self):
        self.plant('src/system/areas.h', '(WORK_AREA + 0x100000)', '(WORK_AREA + 0x200000)')
        self.assertIn('src/system/areas.h: WORK_AREA_FILE, 0x8c500000, lies outside WORK_AREA, 0x8c300000 to '
                      '0x8c500000', self.fails(contract))

    def test_a_macro_takes_uint32_arithmetic(self):
        owner = contract().owners['CHAO_LAND_BUILD_AREA']
        self.plant('src/system/areas.h', '#define CHAO_LAND_BUILD_AREA ((Uint8 *)0x8caa0000)',
                   '#define CHAO_LAND_BUILD_AREA ((Uint8 *)(0x0caa0000U - 0x80000000U))')
        self.assertEqual(contract().owners['CHAO_LAND_BUILD_AREA'], owner)

    def test_a_macro_takes_its_tokens_as_c_substitutes_them(self):
        self.write('demo.h', '#define GAP 8 + 1\n#define BARE 16 - GAP\n#define GROUPED_GAP (8 + 1)\n'
                             '#define GROUPED 16 - GROUPED_GAP\n#define CAST (Uint32)32\n#define OCTAL 040\n')
        self.assertEqual(numbers(Path('demo.h'), ('BARE', 'GROUPED', 'CAST', 'OCTAL')),
                         {'BARE': 9, 'GROUPED': 7, 'CAST': 32, 'OCTAL': 32})

    def test_an_address_it_cannot_read_fails(self):
        self.plant('modules/chao/garden.c', '((struct chao_area_head *)0x8c9c0000)',
                   '((struct chao_area_head *)0x8c9c0000 + 1)')
        self.assertIn('modules/chao/garden.c: gives no number for CHAO_AREA_SLOT', self.fails(contract))


@unittest.skipUnless((SH_ELF / 'bin/sh-elf-ld').is_file(), 'requires SH-4 binutils')
class Build(Sources):
    """The command line, as make dc-images and make dc-files run it over the linked images."""

    def link(self, name, *sections, end=False):
        """Link build/dc/images/NAME.elf, entered at its first section. Each
        section is (its name, its address, its bytes, and its flags as
        .section takes them). With end, the image defines _end past the last."""
        source = ''.join(f'.section {section},{flags}\n.space {size}\n' for section, _, size, flags in sections)
        script = ('PHDRS {' + ''.join(f' p{at} PT_LOAD;' for at in range(len(sections))) + ' }\nSECTIONS {'
                  + ''.join(f' {section} {address:#x} : {{ *({section}) }} :p{at}'
                            for at, (section, address, _, _) in enumerate(sections)) + ' }\n')
        Path(f'{name}.s').write_text(source + ('.global _end\n_end:\n' if end else ''))
        Path(f'{name}.ld').write_text(script)
        Path('build/dc/images').mkdir(parents=True, exist_ok=True)
        subprocess.run([SH_ELF / 'bin/sh-elf-as', '--little', '-o', f'{name}.o', f'{name}.s'], check=True)
        subprocess.run([SH_ELF / 'bin/sh-elf-ld', '-EL', '-z', 'max-page-size=4096', '-T', f'{name}.ld', '-e',
                        hex(sections[0][1]), '-o', f'build/dc/images/{name}.elf', f'{name}.o'], check=True)

    def run_tool(self, image):
        """dc_memory's status and what it printed, run on image in build/dc."""
        done = subprocess.run([sys.executable, TOOLS / 'dc_memory.py', image, 'build/dc'], capture_output=True,
                              text=True, check=False)
        return done.returncode, done.stdout + done.stderr

    def garden(self, race_bytes):
        """Link the main executable, CHAO with its archives, and the race, race_bytes long."""
        self.link('1st_read', ('.text', 0x8c010000, 0x1000, '"ax"'))
        self.link('chao', ('.head', 0x8c500000, 0x1000, '"aw"'),
                  ('.chao_models', 0x0c698700, 0x221c94, '"aw",@nobits'),
                  ('.chao_motions', 0x0c8ba800, 0x1042d0, '"aw",@nobits'))
        self.link('chaostgrace', ('.head', 0x0c9c0000, race_bytes, '"aw"'))

    def test_a_camera_area_in_the_heap_fails_the_build(self):
        self.link('1st_read', ('.text', 0x8c010000, 0x1000, '"ax"'))
        self.link('stg13', ('.head', 0x8c500000, 0x1000, '"aw"'), ('.bss', 0x8c501000, 0x100, '"aw",@nobits'),
                  end=True)
        self.assertEqual(self.run_tool('stg13'), (0, ''))
        report = Path('build/dc/memory/stg13.txt').read_text()
        self.assertIn('  0x0c501100  0x0cb00000  STAGE_MODEL_BUILD_AREA\n', report)
        play, demo = report.split('stg13, in a demo\n')
        self.assertIn('  0x0cb00000  0x0cbd0000  DEMO_AREA\n  0x0cbd0000  0x0cbdc000  MESSAGE_AREA\n', play)
        self.assertIn('  0x0cb00000  0x0cbe0000  demo file\n  0x0cbe0000  0x0cbf0000  CAMSET_AREA\n', demo)
        self.plant(*CAMERAS_IN_THE_HEAP)
        self.assertEqual(self.run_tool('stg13'), (1, 'dc_memory: CAMLIN_AREA_1, 0x0cbfc000 to 0x0cc08000, overlaps '
                                                     'heap, 0x0cc00000 to 0x0d000000\n'))

    def test_the_land_build_area_in_the_race_fails_the_build(self):
        self.garden(0x70000)
        self.assertEqual(self.run_tool('chaostgrace'), (0, ''))
        self.plant(*LAND_IN_THE_RACE)
        status, out = self.run_tool('chaostgrace')
        self.assertEqual(status, 1)
        self.assertIn('dc_memory: chaostgrace, 0x0c9c0000 to 0x0ca30000, overlaps CHAO_LAND_BUILD_AREA, '
                      '0x0ca20000 to 0x0cb00000\n', out)

    def test_chao_with_an_archive_away_from_areas_h_fails(self):
        self.garden(0x1000)
        self.link('chao', ('.head', 0x8c500000, 0x1000, '"aw"'), ('.chao_models', 0x0c600000, 0x1000, '"aw",@nobits'),
                  ('.chao_motions', 0x0c8ba800, 0x1042d0, '"aw",@nobits'))
        self.assertEqual(self.run_tool('chao'), (1, 'dc_memory: build/dc/images/chao.elf: no segment lies at '
                                                    'CHAO_MODEL_SLOT, 0x0c698700 to 0x0c8ba394\n'))

    def test_the_race_must_leave_room_for_its_land_file(self):
        self.garden(0x80000 + 0x1000)
        status, out = self.run_tool('chaostgrace')
        self.assertEqual(status, 1)
        self.assertIn('overlaps CHAO_RACE_LAND_AREA', out)

    def test_a_resident_load_in_the_stage_slot_fails(self):
        self.link('1st_read', ('.text', 0x8c010000, 0x1000, '"ax"'), ('.overlap', 0x0c500000, 4, '"aw"'))
        self.link('title', ('.head', 0x8c500000, 0x18, '"aw"'))
        status, out = self.run_tool('title')
        self.assertEqual(status, 1)
        self.assertIn('dc_memory: 1st_read, 0x0c010000 to 0x0c500004, overlaps WORK_AREA', out)
        self.assertIn('overlaps title, 0x0c500000 to 0x0c500018', out)


if __name__ == '__main__':
    unittest.main()
