"""Hold config/loads.txt to the tree, and a small disc's files to the rooms of a table of its own."""

import hashlib
import os
from pathlib import Path
import unittest

import dc_loads
from support import ROOT, TreeTest, run
from test_dc_disc import compile_prskit, compress_prs

# a tree's headers: WORK_AREA holds two sectors, and each room a number
HEADERS = {
    'sdk/include/shinobi.h': '#define GDD_FS_SCTSIZE 2048\n',
    'src/system/areas.h': '#define WORK_AREA ((Uint8 *)0x8c300000)\n#define WORK_AREA_END ((Uint8 *)0x8c301000)\n'
                          '#define DEMO_AREA ((Uint8 *)0x8cb00000)\n#define CAMSET_AREA ((Uint8 *)0x8cb00040)\n'
                          '#ifdef NONMATCHING\n#define DEMO_FILE_BYTES ((Uint32)(CAMSET_AREA - DEMO_AREA))\n'
                          '#else\n#define SHIPPED_BYTES 32\n#endif\n'
                          '#define SCRIPT_BYTES 4096\n#define SCENE_BYTES 48\n',
}
SOURCE = 'src/load.c'
# the loads of the tree's source, one of each form and bound a row may have
LOADS = ('SONIC2/DEMO[0-9][0-9].PRS  prs     exactly  src/system/areas.h  DEMO_FILE_BYTES  src/load.c  a demo\n'
         'SONIC2/EVENT/ME*.SCR       file    within   src/system/areas.h  SCRIPT_BYTES     src/load.c  a script\n'
         'SONIC2/EVENT/E000?.PRS     prs     within   src/system/areas.h  SCENE_BYTES      src/load.c  a scene\n'
         'SONIC2/STG13CAM.PRS        buffer  within   src/system/areas.h  SCRIPT_BYTES     src/load.c  a camera file\n')
# bytes that PRS can't shorten, so that a file of them outgrows a room packed
NOISE = hashlib.shake_256(b'loads').digest(6000)


class Tree(unittest.TestCase):
    def test_the_table_holds_to_the_tree(self):
        self.addCleanup(os.chdir, os.getcwd())
        os.chdir(ROOT)
        demo = dc_loads.loads()[0]
        self.assertEqual((demo.pattern, demo.bound, demo.room, demo.bytes),
                         ('SONIC2/DEMO[0-9][0-9].PRS', 'exactly', 'DEMO_FILE_BYTES', 0xe0000))
        self.assertEqual(demo.files, [f'SONIC2/DEMO{stage}.PRS' for stage in ('09', '13', '14', '16', '18', '21')])


class Disc(TreeTest):
    """Runs in a tree of its own, with a table of four loads and a disc of the files they name."""

    def setUp(self):
        super().setUp()
        compile_prskit()
        for path, text in HEADERS.items():
            self.write(path, text)
        self.write(SOURCE, 'DEMO_FILE_BYTES SCRIPT_BYTES SCENE_BYTES\n')
        self.write('config/loads.txt', LOADS)
        # each file fills its room: the demo to the byte, the script and the camera file to the sector
        self.files = {'IP.BIN': b'boot', 'SONIC2/DEMO13.PRS': compress_prs(bytes(64)),
                      'SONIC2/EVENT/ME0001_0.SCR': bytes(4096), 'SONIC2/EVENT/E0004.PRS': compress_prs(bytes(48)),
                      'SONIC2/STG13CAM.PRS': compress_prs(bytes(4096))}
        self.catalog()

    def catalog(self):
        """Write the disc's files under disc/, and the inventory that names each by its bytes."""
        for name, data in self.files.items():
            self.write(Path('disc') / name, data)
        self.write('config/jp-disc.sha256', ''.join(f'{hashlib.sha256(data).hexdigest()}  {name}\n'
                                                    for name, data in sorted(self.files.items())))

    def problems(self):
        return dc_loads.problems(dc_loads.loads(), Path('disc'), Path('build/prskit'))

    def test_files_that_fill_their_rooms_fit(self):
        self.assertEqual(self.problems(), [])
        self.assertEqual(run(dc_loads.main), (0, '4 loads name 4 files of the disc\n'))
        self.assertEqual(run(dc_loads.main, 'disc'), (0, "4 of the disc's files fit the rooms of 4 loads\n"))

    def test_names_each_file_that_outgrows_its_room_with_its_size_and_the_room(self):
        # a demo a byte short and a scene a byte long, a script and a packed camera file a byte into another
        # sector, and a packed scene of three sectors, more than WORK_AREA holds
        self.files.update({'SONIC2/DEMO13.PRS': compress_prs(bytes(63)), 'SONIC2/EVENT/ME0001_0.SCR': bytes(4097),
                           'SONIC2/EVENT/E0004.PRS': compress_prs(bytes(49)),
                           'SONIC2/EVENT/E0005.PRS': compress_prs(NOISE[:4000]),
                           'SONIC2/STG13CAM.PRS': compress_prs(NOISE)})
        self.catalog()
        scene, camera = len(self.files['SONIC2/EVENT/E0005.PRS']), len(self.files['SONIC2/STG13CAM.PRS'])
        self.assertEqual((-(-scene // 2048), -(-camera // 2048)), (3, 4))
        faults = [
            'disc/SONIC2/DEMO13.PRS: decodes to 63 bytes, where its load at config/loads.txt:1 takes only '
            'DEMO_FILE_BYTES, 64 bytes',
            'disc/SONIC2/EVENT/ME0001_0.SCR: its 4097 bytes take 3 sectors, past its room at config/loads.txt:2, '
            'SCRIPT_BYTES, 4096 bytes',
            'disc/SONIC2/EVENT/E0004.PRS: decodes to 49 bytes, past its room at config/loads.txt:3, SCENE_BYTES, '
            '48 bytes',
            f"disc/SONIC2/EVENT/E0005.PRS: its {scene} bytes take 3 sectors, past WORK_AREA's 4096 bytes, where "
            'its load at config/loads.txt:3 reads it',
            f'disc/SONIC2/STG13CAM.PRS: its {camera} bytes take 4 sectors, past its room at config/loads.txt:4, '
            'SCRIPT_BYTES, 4096 bytes']
        self.assertEqual(self.problems(), faults)
        status, text = run(dc_loads.main, 'disc')
        self.assertEqual((status, [line.split(': ', 1)[1] for line in text.splitlines()]), (1, faults))

    def test_names_a_demo_that_decodes_past_its_room(self):
        self.files['SONIC2/DEMO13.PRS'] = compress_prs(bytes(65))
        self.catalog()
        self.assertEqual(self.problems(), ['disc/SONIC2/DEMO13.PRS: decodes to 65 bytes, where its load at '
                                           'config/loads.txt:1 takes only DEMO_FILE_BYTES, 64 bytes'])

    def test_names_a_buffer_file_that_decodes_past_its_room(self):
        self.files['SONIC2/STG13CAM.PRS'] = compress_prs(bytes(4097))
        self.catalog()
        self.assertEqual(self.problems(), ['disc/SONIC2/STG13CAM.PRS: decodes to 4097 bytes, past its room at '
                                           'config/loads.txt:4, SCRIPT_BYTES, 4096 bytes'])

    def test_names_a_file_the_extraction_lacks_or_holds_changed_or_that_wont_decode(self):
        # a stream cut before its end marker, which the inventory names by those bytes
        self.files['SONIC2/EVENT/E0004.PRS'] = self.files['SONIC2/EVENT/E0004.PRS'][:-2]
        self.catalog()
        Path('disc/SONIC2/DEMO13.PRS').unlink()
        self.write('disc/SONIC2/STG13CAM.PRS', compress_prs(bytes(8)))
        cut = len(self.files['SONIC2/EVENT/E0004.PRS'])
        self.assertEqual(self.problems(), [
            'disc/SONIC2/DEMO13.PRS: missing from the extraction',
            "disc/SONIC2/EVENT/E0004.PRS: build/prskit can't decode it, for its load at config/loads.txt:3: the "
            f'input ends before the end marker at {cut:#x}',
            "disc/SONIC2/STG13CAM.PRS: not the Japanese disc's file, by config/jp-disc.sha256"])

    def test_refuses_a_row_out_of_form_or_that_the_tree_doesnt_bear(self):
        row = LOADS.splitlines()[0]
        for case, (old, new, problem) in {
                'five fields': ('src/load.c  a demo', '', 'not a row: FILES FORM BOUND HEADER ROOM SOURCE'),
                'another form': ('prs ', 'raw ', 'raw: not a form, one of file, prs, buffer'),
                'another bound': ('exactly', 'at most', 'at: not a bound of a prs row, within or exactly'),
                'a room of three': ('DEMO_FILE_BYTES ', 'A-B-C ', 'A-B-C: not a room, a macro or NAME-NAME'),
                'a macro of the matching side': ('DEMO_FILE_BYTES ', 'SHIPPED_BYTES ',
                                                 'src/system/areas.h gives no number for SHIPPED_BYTES'),
                'a macro the source lacks': ('DEMO_FILE_BYTES ', 'DEMO_FILE_BYTES-WORK_AREA ',
                                             "src/load.c doesn't name WORK_AREA, which the row's room reads"),
                'no file': ('DEMO[0-9][0-9]', 'DEMO[0-9]', 'SONIC2/DEMO[0-9].PRS: names no file of '
                                                         'config/jp-disc.sha256')}.items():
            with self.subTest(case):
                self.assertEqual(row.count(old), 1)
                self.write('config/loads.txt', row.replace(old, new) + '\n')
                self.assertTrue(self.fails(dc_loads.loads).endswith(f': config/loads.txt:1: {problem}'))
        self.write('config/loads.txt', LOADS.replace('file    within ', 'file    exactly'))
        self.assertTrue(self.fails(dc_loads.loads).endswith(
            ': config/loads.txt:2: exactly: not a bound of a file row, within'))


if __name__ == '__main__':
    unittest.main()
