"""The kart registry stores paired mesh roots, counted patch tables and independently saved UVs."""

from types import SimpleNamespace
import struct
import unittest

import cart_data
import cart_polygons


class CourseRecords(unittest.TestCase):
    def setUp(self):
        self.data = bytearray(512)
        self.names = {}
        self.addresses = {}

    def obj(self, address, name, kind, size, targets=()):
        obj = SimpleNamespace(name=name, kind=kind, size=size, address=address,
                              targets=targets, symbol='_' + name, where='course')
        self.names[name] = obj
        self.addresses[address] = obj
        return obj

    def fields(self, obj):
        return list(cart_data.pointers(obj, self.data, 0, self.addresses, self.names))

    def test_course_piece_selects_roots_and_counts_its_signs(self):
        pieces = self.obj(4, 'section', 'cart_pieces', 108)
        draw = self.obj(120, 'draw', 'chunk_object', 52)
        solid = self.obj(172, 'solid', 'basic_object', 52)
        signs = self.obj(224, 'signs', 'cart_signs', 32)
        struct.pack_into('<II', self.data, 4, draw.address, solid.address)
        struct.pack_into('<III', self.data, 4 + 96, 0, signs.address, 2)
        self.assertEqual(self.fields(pieces), [(0, draw), (4, solid), (96, None), (100, signs)])
        for offset, value, reason in ((0, solid.address, 'expects chunk_object'), (4, 0, 'paired'),
                                      (96, 4, 'compiled slot'), (104, 3, '3 cart_signs')):
            changed = self.data[:]
            struct.pack_into('<I', self.data, 4 + offset, value)
            with self.subTest(offset=offset), self.assertRaisesRegex(SystemExit, reason):
                self.fields(pieces)
            self.data[:] = changed

    def test_piece_gates_count_complete_rows_of_four_xz_points(self):
        table = self.obj(4, 'gates', 'cart_gate_lists', 16)
        points = self.obj(32, 'road', 'cart_gates', 64)
        struct.pack_into('<4I', self.data, 4, points.address, 2, 0, 0)
        self.assertEqual(self.fields(table), [(0, points), (8, None)])
        for offset, value in ((4, 40), (8, 1), (12, points.address), (16, 1)):
            saved = self.data[:]
            struct.pack_into('<I', self.data, offset, value)
            with self.subTest(offset=offset), self.assertRaises(SystemExit):
                self.fields(table)
            self.data[:] = saved

    def test_uv_belongs_to_its_own_strip_and_keeps_the_saved_coordinates(self):
        uvs = self.obj(4, 'offsets', 'cart_uv_patches', 8)
        patch = self.obj(20, 'water', 'cart_texture_patches', 16)
        slot = cart_polygons.Field(80, 'poly+4', 'cart_tid', 80)
        coord = cart_polygons.Field(90, 'poly+14', 'cart_uv', 80)
        self.addresses.update({80: slot, 90: coord, 96: cart_polygons.Field(96, 'poly+20', 'cart_uv', 94)})
        struct.pack_into('<Ihh', self.data, 4, 90, -80, 120)
        struct.pack_into('<hh', self.data, 90, -80, 120)
        struct.pack_into('<H', self.data, 80, 0x402b)
        struct.pack_into('<4I', self.data, 20, 43, 80, 1, 4)
        self.assertEqual(self.fields(uvs), [(0, coord)])
        self.assertEqual(self.fields(patch), [(4, slot), (12, uvs)])
        for offset, value, record, reason in ((4, 91, uvs, 'expects cart_uv'), (8, 0, uvs, 'saved UV'),
                                              (4, 96, patch, 'different texture word'),
                                              (20, 2, patch, 'texture ID'), (28, 2, patch, '2 cart_uv_patches')):
            changed = self.data[:]
            struct.pack_into('<I', self.data, offset, value)
            with self.subTest(offset=offset), self.assertRaisesRegex(SystemExit, reason):
                self.fields(record)
            self.data[:] = changed

    def test_terminal_remap_and_descendant_model_ownership(self):
        remaps = self.obj(4, 'remaps', 'cart_texture_remaps', 24)
        node = self.obj(36, 'root', 'chunk_object', 52, ('-', 'child', '-'))
        self.obj(88, 'child', 'chunk_object', 52, ('mesh', '-', '-'))
        self.obj(140, 'mesh', 'chunk_model', 24, ('vtx', 'poly'))
        self.obj(164, 'poly', 'chunk_polygons', 20)
        table = self.obj(188, 'table', 'cart_patch_list', 4)
        patch = self.obj(192, 'patch', 'cart_texture_patches', 16)
        struct.pack_into('<3I', self.data, 4, 36, 1, 188)
        struct.pack_into('<I', self.data, 188, 192)
        struct.pack_into('<I', self.data, 196, 168)
        self.assertEqual(self.fields(table), [(0, patch)])
        self.assertEqual(self.fields(remaps), [(0, node), (8, table), (12, None), (20, None)])
        for offset, value, reason in ((196, 400, 'object hierarchy'), (20, 1, 'terminal record'),
                                      (4, 0, 'terminal record'), (8, 3, '3 cart_patch_list')):
            changed = self.data[:]
            struct.pack_into('<I', self.data, offset, value)
            with self.subTest(offset=offset), self.assertRaisesRegex(SystemExit, reason):
                self.fields(remaps)
            self.data[:] = changed

    def test_a_kind_without_a_decoder_is_refused(self):
        signs = self.obj(4, 'signs', 'cart_signs', 16)
        with self.assertRaisesRegex(SystemExit, 'no kart pointer decoder'):
            self.fields(signs)
