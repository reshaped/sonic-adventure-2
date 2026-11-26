"""Patrol path counts, task pointers and debug labels survive relocation."""
from pathlib import Path
import struct

import content
from support import TreeTest


class PatrolData(TreeTest):
    def setUp(self):
        super().setUp()
        source = 'modules/stage/titles.c'
        self.write(source, 'const char labels[8] = {\n#include "stage/titles/labels.inc"\n};\n')
        self.write('offsets/modules/stage/titles.txt', 'datum labels C 0x00000000 8\n')
        self.image('stage', f'file PATROL.BIN\nC 0x00001000 0x00001008\n{source} C 0x00001000 0x00001008\n'
                            'D 0x00001008 0x0000103c\ndisc D 0x00001008 0x0000103c\n', '0x00001000 labels\n')
        self.write('config/stage/data.txt', f'0x00001000 labels text_bank {source} 8\n'
                   '0x00001008 .points path_points 1\n0x0000101c .route stage_path .points -\n'
                   '0x0000102c .routes path_ptrs 1\n0x00001030 .titles text_ptrs 3\n')
        self.data = bytearray(b'ONE\0TWO\0' + bytes(20) + struct.pack('<Hhf6I', 0x8001, 1, 4.0, 0x1008, 0,
                                                                           0x101c, 0x1004, 0, 0x1000))

    def test_paths_and_title_pointers_use_their_named_targets(self):
        self.write('PATROL.BIN', bytes(self.data))
        content.main(['stage', 'PATROL.BIN', 'patrol.s'])
        asm = Path('patrol.s').read_text().splitlines()
        for symbol in ['.Lpoints', '.Lroute', '_labels+4', '_labels']:
            self.assertIn('.long ' + symbol, asm)

    def test_rejects_wrong_layout_count_callback_or_title_address(self):
        for offset, fmt, value, error in [(28, 'H', 2, 'unsupported point layout'), (30, 'h', 2, '2 points'),
                                          (30, 'h', 0, 'point count 0, which must be positive'),
                                          (40, 'I', 0x1000, '.step'), (48, 'I', 0x1001, 'expected a text')]:
            with self.subTest(offset=offset):
                data = self.data[:]
                struct.pack_into('<' + fmt, data, offset, value)
                self.write('PATROL.BIN', bytes(data))
                self.assertIn(error, self.fails(content.main, ['stage', 'PATROL.BIN', 'patrol.s']))

    def test_resident_task_callback_and_position_only_points_in_data_section(self):
        self.image('1st_read', 'file START.BIN\nP 0x00002000 0x00002010\ndisc P 0x00002000 0x00002010\n',
                   '0x00002004 path_rail_init\n')
        self.image('stage', 'file STAGE.BIN\nDATA 0x00001000 0x00001020\ndisc DATA 0x00001000 0x00001020\n',
                   '0x0000101c hint_file\n')
        self.write('config/stage/data.txt', '0x00001000 .positions points 1\n'
                   '0x0000100c .rail stage_path .positions path_rail_init\n'
                   '0x0000101c hint_file pointer_slot -\n')
        data = bytearray(12) + struct.pack('<HhfIII', 0x8000, 1, 0.0, 0x1000, 0x2004, 0)
        self.write('STAGE.BIN', bytes(data))
        content.main(['stage', 'STAGE.BIN', 'stage.s'])
        self.assertIn('.long _path_rail_init', Path('stage.s').read_text().splitlines())
        for offset, value, error in [(12, 1, 'points of type path_points'), (24, 0x2008, '.step'),
                                     (28, 0x2004, 'hint_file in')]:
            with self.subTest(offset=offset):
                altered = data[:]
                struct.pack_into('<H' if offset == 12 else '<I', altered, offset, value)
                self.write('STAGE.BIN', bytes(altered))
                self.assertIn(error, self.fails(content.main, ['stage', 'STAGE.BIN', 'stage.s']))
        self.write('STAGE.BIN', bytes(data))
        for task, error in [('hint_file', 'hint_file is not a declared path task'),
                            ('path_join_step', 'has no address for path task path_join_step')]:
            with self.subTest(task=task):
                self.write('config/stage/data.txt', '0x00001000 .positions points 1\n'
                           f'0x0000100c .rail stage_path .positions {task}\n'
                           '0x0000101c hint_file pointer_slot -\n')
                self.assertIn(error, self.fails(content.main, ['stage', 'STAGE.BIN', 'stage.s']))
