"""A motion's keys keep their original words; every SH-4 address follows the asset."""
from pathlib import Path
import struct

import content
from config import Image
from support import TreeTest


class PlayerMotions(TreeTest):
    base = 0x8c500000
    original = Path('stage.bin')

    def setUp(self):
        super().setUp()
        self.image('stage', 'file STAGE.BIN\nD 0x8c500000 0x8c50003c\n'
                   'disc D 0x8c500000 0x8c50003c\n', '0x8c50002c module_motions\n')
        self.write('config/stage/data.txt', '0x8c500000 layout stage_tracks\n')
        self.write('layouts/stage_tracks.txt', '.turn player_motion 44\nmodule_motions motion_entries 2\n')
        self.data = bytearray(struct.pack('<4I4IIIHHhhIhhI', 0, 0x80000000, 0x3f800000, 0,
                                          self.base, 0, 1, 0, self.base + 16, 1, 3, 2,
                                          17, 1, self.base + 32, -1, 0, 0))
        self.entries = content.objects(Image('stage'))

    def assembly(self):
        self.original.write_bytes(self.data)
        return content.assembly(self.entries, self.original, self.base).decode()

    def test_only_retail_scalar_fields_enter_the_link_verbatim(self):
        assembly = self.assembly()
        self.assertIn('.incbin "stage.bin", 0, 16', assembly)
        self.assertIn('.long .Lstage_tracks.turn.joint_0_position, 0', assembly)
        self.assertIn('.long .Lstage_tracks.turn.tracks\n', assembly)
        self.assertIn('.long .Lstage_tracks.turn\n', assembly)
        self.assertIn('.global _module_motions', assembly)

    def test_rejects_wrong_key_extent_header_registry_and_sentinel(self):
        for at, size, value, error in ((24, 'I', 2, 'invalid position keys'), (0, 'I', 1, 'invalid key frame'),
                                       (32, 'I', self.base + 12, 'invalid NJS_MDATA2'),
                                       (40, 'H', 7, 'position/angle motion'),
                                       (46, 'h', 2, 'no matching 2-node motion'),
                                       (48, 'I', self.base, 'no matching 1-node motion'),
                                       (52, 'h', 0, 'missing final motion sentinel')):
            with self.subTest(error=error):
                original = self.data[:]
                struct.pack_into('<' + size, self.data, at, value)
                self.assertIn(error, self.fails(self.assembly))
                self.data[:] = original


class RepeatedMotionKeys(TreeTest):
    def test_only_identical_final_frame_can_be_repeated(self):
        base = 0x8c500000
        self.image('stage', 'file STAGE.BIN\nD 0x8c500000 0x8c50004c\n'
                   'disc D 0x8c500000 0x8c50004c\n', '0x8c500040 reach\n')
        self.write('config/stage/data.txt', '0x8c500000 reach ninja_motion 76\n')
        keys = struct.pack('<4I', 0, 1, 2, 3) + struct.pack('<4I', 1, 4, 5, 6) * 2
        data = bytearray(keys + struct.pack('<6I2H', 0, base, 0, 3, base + len(keys), 2, 3, 2))
        entries = content.objects(Image('stage'))
        original = Path('stage.bin')
        original.write_bytes(data)
        self.assertIn('.joint_0_angles', content.assembly(entries, original, base).decode())
        for position in (16, 36):
            changed = data[:]
            struct.pack_into('<I', changed, position, 0)
            original.write_bytes(changed)
            self.assertIn('invalid key frame', self.fails(content.assembly, entries, original, base))


class ShapeMotions(TreeTest):
    def test_frame_arrays_relocate_and_have_equal_vertex_counts(self):
        base = 0x8c500000
        self.image('stage', 'file STAGE.BIN\nD 0x8c500000 0x8c500050\n'
                   'disc D 0x8c500000 0x8c500050\n', '0x8c500044 water_shape\n')
        self.write('config/stage/data.txt', '0x8c500000 layout enemy\n')
        self.write('layouts/enemy.txt', '.first points 1\n.second points 2\nwater_shape ninja_motion 44\n')
        # Two keys select the same vertex array. The normal track is unused.
        data = bytearray(36) + bytearray(struct.pack('<10I2H', 0, base, 1, base, base + 36, 0, 2, 0,
                                                      base + 52, 2, 0x30, 2))
        original = Path('stage.bin')
        entries = content.objects(Image('stage'))
        original.write_bytes(data)
        assembly = content.assembly(entries, original, base).decode()
        self.assertEqual(assembly.count('.long .Lenemy.first\n'), 2)
        for pointer, message in ((base + 12, 'different point counts'), (base + 4, 'has no point array')):
            struct.pack_into('<I', data, 48, pointer)
            original.write_bytes(data)
            self.assertIn(message, self.fails(content.assembly, entries, original, base))


class CameraMotions(TreeTest):
    def setUp(self):
        super().setUp()
        self.base = 0x8c500000
        self.image('boss', 'file BOSS.BIN\nD 0x8c500000 0x8c50004c\n'
                   'disc D 0x8c500000 0x8c50004c\n', '0x8c500038 intro_camera\n')
        self.write('config/boss/data.txt', '0x8c500000 layout cameras\n')
        self.write('layouts/cameras.txt', 'intro_camera ninja_motion 68\n'
                   '.intro_action camera_action - intro_camera\n')
        # The camera has a position key and a roll key. Target and FOV are unset.
        self.data = bytearray(struct.pack('<6I8IIIHHII',
                                         0, 0x80000000, 0, 0, 0, 0xffff8000,
                                         self.base, 0, self.base + 16, 0, 1, 0, 1, 0,
                                         self.base + 24, 1, 0x1c1, 4, 0, self.base + 56))
        self.entries = content.objects(Image('boss'))

    def assembly(self):
        original = Path('boss.bin')
        original.write_bytes(self.data)
        return content.assembly(self.entries, original, self.base).decode()

    def test_camera_header_and_action_keep_their_symbols(self):
        assembly = self.assembly()
        self.assertIn('.incbin "boss.bin", 16, 8', assembly)
        self.assertIn('.long _intro_camera.joint_0_position, 0, _intro_camera.joint_0_roll, 0', assembly)
        self.assertIn('_intro_camera:\n.size _intro_camera, 12\n.long _intro_camera.tracks', assembly)
        self.assertIn('.long 0\n.long _intro_camera\n', assembly)

    def test_clip_records_relocate_only_their_motion_pointer(self):
        table = Path('config/boss/splits.txt')
        table.write_text(table.read_text().replace('0x8c50004c', '0x8c500084'))
        with Path('layouts/cameras.txt').open('a') as layout:
            layout.write('.clips motion_clips 2\n')
        self.entries = content.objects(Image('boss'))
        clip = struct.pack('<IHHii3f', self.base + 56, 1, 2, -1, -40, 2.0, 8.0, 0.5)
        self.data.extend(clip * 2)
        assembly = self.assembly()
        self.assertIn('.long _intro_camera\n.incbin "boss.bin", 80, 24\n'
                      '.long _intro_camera\n.incbin "boss.bin", 108, 24', assembly)
        struct.pack_into('<I', self.data, 104, self.base)
        self.assertIn('expected a ninja_motion', self.fails(self.assembly))

    def test_overlapping_key_ranges_name_both_boundaries(self):
        struct.pack_into('<I', self.data, 32, self.base + 8)
        self.assertIn('expected the next key range at 0x8c500010, got 0x8c500008', self.fails(self.assembly))

    def test_rejects_wrong_channel_count_and_action_pointing_at_the_keys(self):
        struct.pack_into('<H', self.data, 66, 3)
        self.assertIn('unsupported motion', self.fails(self.assembly))
        struct.pack_into('<H', self.data, 66, 4)
        struct.pack_into('<I', self.data, 72, self.base)
        self.assertIn('intro_action.motion', self.fails(self.assembly))

    def test_motion_pointer_array_resolves_the_exported_header(self):
        self.image('boss', 'file BOSS.BIN\nD 0x8c500000 0x8c500054\n'
                   'disc D 0x8c500000 0x8c500054\n',
                   '0x8c500038 intro_camera\n0x8c50004c camera_motions\n')
        self.write('layouts/cameras.txt', 'intro_camera ninja_motion 68\n'
                   '.intro_action camera_action - intro_camera\n'
                   'camera_motions ninja_motion_ptrs 2\n')
        self.data.extend(struct.pack('<2I', self.base + 56, 0))
        self.entries = content.objects(Image('boss'))
        self.assertIn('camera_motions:\n.long _intro_camera\n.long 0\n', self.assembly())
        struct.pack_into('<I', self.data, 76, self.base)
        self.assertIn('expected a ninja_motion', self.fails(self.assembly))
