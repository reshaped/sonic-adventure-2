"""A frame table can take the address of a mesh inside a contiguous C array of water poses."""

from pathlib import Path
import struct

import chunk_poses
import content
from config import Image
from support import TreeTest


class ChunkPoses(TreeTest):
    base = 0x8c500000

    def setUp(self):
        super().setUp()
        # One 16-bit polygon word, two bytes of alignment, one vertex word,
        # a 24-byte model and a 52-byte node. The clock selects the second mesh first.
        self.image('room', f'file ROOM.BIN\nP {self.base:#x} {self.base + 4:#x}\n'
                   f'D {self.base + 4:#x} {self.base + 232:#x}\n'
                   f'disc D {self.base + 4:#x} {self.base + 232:#x}\n',
                   f'{self.base + 4:#x} poses\n')
        self.table = Path('config/room/data.txt')
        start = self.base + 4
        self.write(self.table, f'{start:#x} poses chunk_poses 2 1 1\n'
                   f'{start + 168:#x} .frames chunk_model_ptrs 2\n'
                   f'{start + 176:#x} .root chunk_object - poses[1].node -\n')
        self.optical = bytearray(232)
        for i in (0, 84):
            # each pose's polygon and vertex lists hold their NJD_CE alone
            struct.pack_into('<HHI', self.optical, 4 + i, 255, 0, 255)
            struct.pack_into('<II', self.optical, 4 + i + 8, start + i + 4, start + i)
            struct.pack_into('<I', self.optical, 4 + i + 36, start + i + 8)
        struct.pack_into('<I', self.optical, 4 + 84 + 80, start + 32)
        struct.pack_into('<II', self.optical, 4 + 168, start + 84 + 8, start + 8)
        struct.pack_into('<I', self.optical, 4 + 176 + 44, start + 84 + 32)

    def emit(self):
        self.write('original', bytes(self.optical))
        content.main(['room', 'original', 'room.s'])
        return Path('room.s').read_text()

    def test_contiguous_poses_and_nonsequential_frame_table(self):
        assembly = self.emit()
        self.assertIn('.size _poses, . - _poses', assembly)
        self.assertIn('.long _poses+92\n.long _poses+8', assembly)
        self.assertIn('.long _poses+116', assembly)
        self.assertIn('.long _poses+32', assembly)

    def test_textured_poses_relocate_each_frames_own_material_and_mesh(self):
        name = self.base + 4
        frames = self.base + 8
        self.image('room', f'file ROOM.BIN\nP {self.base:#x} {name:#x}\nC {name:#x} {frames:#x}\n'
                   f'D {frames:#x} {frames + 208:#x}\ndisc C {name:#x} {frames:#x}\n'
                   f'disc D {frames:#x} {frames + 208:#x}\n', f'{frames:#x} poses\n')
        self.write(self.table, f'{name:#x} .cloth_filename texture_filename 4\n'
                   f'{frames:#x} poses textured_chunk_poses 2 1 1\n')
        self.optical = bytearray(216)
        self.optical[4:8] = b'art\0'
        for frame in (8, 112):
            address = self.base + frame
            struct.pack_into('<IIIII', self.optical, frame, name, 0, 0, address, 1)
            struct.pack_into('<HHI', self.optical, frame + 20, 255, 0, 255)
            struct.pack_into('<II', self.optical, frame + 28, address + 24, address + 20)
            struct.pack_into('<I', self.optical, frame + 56, address + 28)
        assembly = self.emit()
        self.assertEqual(assembly.count('.long .Lcloth_filename'), 2)
        self.assertIn('.long _poses+104\n', assembly)
        self.assertIn('.long _poses+128\n.long _poses+124\n', assembly)
        self.assertIn('.size _poses, . - _poses', assembly)
        for field, value, error in ((0, frames, 'declared texture filename'),
                                    (8, 1, 'own one-entry texture list'),
                                    (12, frames, 'own one-entry texture list'),
                                    (16, 2, 'own one-entry texture list'),
                                    (100, frames + 52, 'independent textured pose')):
            with self.subTest(field=field):
                original = self.optical[:]
                struct.pack_into('<I', self.optical, 112 + field, value)
                self.assertIn(error, self.fails(self.emit))
                self.optical[:] = original

    def test_wrong_field_interior_and_nonzero_alignment_are_rejected(self):
        for offset, value, error in ((12, self.base + 8 + 84, 'own chunk_vertices'),
                                     (168, self.base + 36 + 4, 'outside its pose hierarchy'),
                                     (172, self.base + 36, 'expected a chunk_model'),
                                     (6, 1, 'nonzero polygon alignment')):
            with self.subTest(offset=offset):
                old = self.optical[:]
                struct.pack_into('<I', self.optical, offset, value)
                self.assertIn(error, self.fails(self.emit))
                self.optical[:] = old
        self.write(self.table, self.table.read_text().replace('poses[1].node', 'poses[2].node'))
        self.assertIn('needs a declared chunk_object', self.fails(self.emit))

    def test_hierarchy_continues_at_one_declared_node_in_another_bank(self):
        layout = self.table.read_text().replace('poses chunk_poses 2 1 1',
            f'poses chunk_poses 1 1 1\n{self.base + 88:#x} .tail chunk_poses 1 1 1 poses[0].node')
        self.write(self.table, layout.replace('poses[1].node', '.tail[0].node'))
        self.assertIn('.long .Ltail+8\n.long _poses+8', self.emit())
        self.assertIn('.long _poses+32', self.emit())
        struct.pack_into('<I', self.optical, 168, 0)
        self.assertIn('no hierarchy link', self.fails(self.emit))
        struct.pack_into('<I', self.optical, 168, self.base + 12)
        self.assertIn('outside its pose hierarchy', self.fails(self.emit))

    def test_mixed_pose_runs_and_their_root_keep_one_symbol(self):
        start = self.base + 4
        end = start + 232
        split = Path('config/room/splits.txt')
        split.write_text(split.read_text().replace(hex(self.base + 232), hex(end)))
        self.write(self.table, f'{start:#x} poses chunk_pose_tree 1 1 1 1 1 2\n'
                   f'{start + 224:#x} .frames chunk_model_ptrs 2\n')
        self.optical = bytearray(end - self.base)
        # the second run's vertex list is an empty NJD_CV chunk and NJD_CE
        struct.pack_into('<HHI', self.optical, 4, 255, 0, 255)
        struct.pack_into('<HHII', self.optical, 4 + 84, 255, 0, 34, 255)
        for offset, mesh in ((0, 8), (84, 96)):
            struct.pack_into('<II', self.optical, 4 + mesh, start + offset + 4, start + offset)
            struct.pack_into('<I', self.optical, 4 + mesh + 28, start + mesh)
        struct.pack_into('<I', self.optical, 4 + 172 + 44, start + 120)
        struct.pack_into('<II', self.optical, 4 + 224, start + 96, start + 8)
        self.assertIn('.long _poses+96\n.long _poses+8', self.emit())
        self.assertIn('.long _poses+120', self.emit())
        for slot, value in ((4, start + 96), (44, start + 96), (48, start + 120)):
            with self.subTest(slot=slot):
                old = self.optical[:]
                struct.pack_into('<I', self.optical, 4 + 172 + slot, value)
                self.assertIn('model-free root', self.fails(self.emit))
                self.optical[:] = old
        # the second pose's sibling names its own node, which every pointer check alone accepts
        struct.pack_into('<I', self.optical, 4 + 120 + 48, start + 120)
        self.assertIn('not a tree', self.fails(self.emit))

    def test_a_zero_in_a_full_run_names_the_lexical_rule(self):
        error = self.fails(content.layout, 'chunk_pose_tree', ['1', '1', '1', '1', '2', '0'], 'table')
        self.assertIn('table: a pose count or word length must be a positive decimal', error)
        self.assertNotIn('chunk_poses', error)

    def test_fixed_filename_keeps_its_zero_filled_storage(self):
        for fields, size, error in ((['16', '13'], 16, None), (['12', '13'], 0, 'exceeds its array')):
            if error:
                self.assertIn(error, self.fails(content.layout, 'texture_filename', fields, 'table'))
            else:
                self.assertEqual(content.layout('texture_filename', fields, 'table'), (size, 4, ('13',)))
        name = content.Object('sample', self.base, 8, 4, 0, 'C', 'texture_filename', ('5',), 'room')
        content.land_data.check_record(name, b'abcd\0\0\0\0', 0, {})
        self.assertIn('zero-filled', self.fails(content.land_data.check_record, name, b'abcd\0x\0\0', 0, {}))


class ShaftWallPoses(TreeTest):
    base = 0x8c500000

    def setUp(self):
        super().setUp()
        # One diffuse-color vertex and a terminated empty polygon list per pose.
        # Polygons occupy two bytes, followed by two bytes of vertex alignment.
        self.shape = chunk_poses.Shape(2, 1, 7)
        self.assertEqual((self.shape.vertices, self.shape.mesh, self.shape.node, self.shape.stride), (4, 32, 56, 108))
        bank = self.base + 4
        root = bank + 216
        frame = root + 52
        end = frame + 12
        self.image('track', f'file TRACK.BIN\nP {self.base:#x} {bank:#x}\nD {bank:#x} {end:#x}\n'
                   f'disc D {bank:#x} {end:#x}\n', f'{bank:#x} poses\n{frame:#x} frames\n')
        self.write('config/track/data.txt', f'{bank:#x} layout panels\n')
        self.layout = ('poses shaft_wall_poses 2 1 7\n'
                       '.root chunk_object - poses[1].node -\n'
                       'frames chunk_model_ptrs 3\n')
        self.write('layouts/panels.txt', self.layout)
        self.data = bytearray(end - self.base)
        for i in range(2):
            at = 4 + i * 108
            struct.pack_into('<H', self.data, at, 255)
            struct.pack_into('<II', self.data, at + 4, (5 << 16) | 35, 1 << 16)
            struct.pack_into('<I', self.data, at + 28, 255)
            struct.pack_into('<II', self.data, at + 32, self.base + at + 4, self.base + at)
            struct.pack_into('<I', self.data, at + 60, self.base + at + 32)
            struct.pack_into('<I', self.data, at + 104, bank + 56 if i else 0)
        struct.pack_into('<I', self.data, root - self.base + 44, bank + 108 + 56)
        struct.pack_into('<III', self.data, frame - self.base, bank + 108 + 32, bank + 32, bank + 32)

    def test_embedded_fields_and_frame_holds_follow_the_array_symbol(self):
        self.write('disc', bytes(self.data))
        content.main(['track', 'disc', 'stage.s'])
        assembly = Path('stage.s').read_text()
        self.assertIn('.size _poses, . - _poses', assembly)
        self.assertIn('.long _poses+164', assembly)
        self.assertIn('_frames:\n.long _poses+140\n.long _poses+32\n.long _poses+32', assembly)

    def test_only_valid_embedded_members_and_references_are_accepted(self):
        for offset, value, reason in [(6, 1, 'polygon alignment'), (8, 41, 'diffuse-color vertices'),
                                      (32, 0, 'vertex extent'), (112 + 104, 0, 'incorrect pointer'),
                                      (272, self.base + 4 + 33, 'expected a chunk_model')]:
            with self.subTest(offset=offset):
                damaged = bytearray(self.data)
                struct.pack_into('<I', damaged, offset, value)
                self.write('disc', bytes(damaged))
                self.assertIn(reason, self.fails(content.main, ['track', 'disc', 'stage.s']))
        for target in ('poses[2].node', 'poses[1].mesh'):
            self.write('layouts/panels.txt', self.layout.replace('poses[1].node', target))
            self.assertIn('needs a declared chunk_object', self.fails(content.objects, Image('track')))
