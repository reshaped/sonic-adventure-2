"""Only real texture-slot and NJS_TEX fields may be referenced inside a kart polygon list."""

import struct
from types import SimpleNamespace
import unittest

import cart_polygons


def halfwords(*words):
    return struct.pack(f'<{len(words)}H', *[v & 0xffff for v in words])


def object_at(name, kind, address, size):
    return SimpleNamespace(symbol=name, where=name, kind=kind, section='D', address=address, size=size)


class CartPolygons(unittest.TestCase):
    # A D8 chunk contains three vertices at indices 4..6, then NJD_CE.
    vertices = struct.pack('<II', (13 << 16) | 35, (3 << 16) | 4) + bytes(48) + struct.pack('<I', 255)
    # Same UVs under two distinct TID words: member identity decides which texture governs each pair.
    strip = [65, 12, 1, -3, 4, -128, 64, 5, 0, 256, 6, 192, 512, 0]

    def fields(self, polygons, vertices=None):
        v = self.vertices if vertices is None else vertices
        p = halfwords(*polygons)
        return cart_polygons.patch_fields(object_at('poly', 'chunk_polygons', len(v), len(p)),
                                          object_at('vertices', 'chunk_vertices', 0, len(v)),
                                          v + p, 0)

    def test_only_distinct_texture_and_uv_record_starts_are_exposed(self):
        fields = self.fields([19, 4, 0, 0, 0, 0, 8, 0x4041, *self.strip,
                              8, 0x4038, 66, *self.strip[1:], 255])
        local = [(f.address - len(self.vertices), f.kind, f.texture_word - len(self.vertices)) for f in fields]
        self.assertEqual(local, [(14, 'cart_tid', 14), (26, 'cart_uv', 14), (32, 'cart_uv', 14),
                                 (38, 'cart_uv', 14), (46, 'cart_tid', 46), (58, 'cart_uv', 46),
                                 (64, 'cart_uv', 46), (70, 'cart_uv', 46)])
        self.assertEqual([f.symbol for f in fields], [f'poly+{offset}' for offset, _, _ in local])

    def test_strip_triangle_user_words_are_not_uvs_or_indices(self):
        # Each triangle has two additional user halfwords after the third vertex.
        strip = [65, 14, 0x8001, 3, 6, 0, 0, 5, 0, 0, 4, 0, 0, 123, 456, 0]
        fields = self.fields([8, 4, *strip, 255])
        self.assertEqual([f.address - len(self.vertices) for f in fields], [2, 14, 20, 26])

    def test_rejects_inconsistent_counts_indices_and_padding(self):
        for idx, value, reason in ((1, 30, 'runs past the list'), (1, 10, 'runs past its chunk'),
                                   (2, 0, 'empty kart strip'), (2, 2, 'fewer than three vertices'),
                                   (3, -4, 'runs past its chunk'), (3, 2, 'fewer than three vertices'),
                                   (4, 3, 'outside its vertex list'), (7, 7, 'outside its vertex list'),
                                   (13, 1, 'extra words')):
            with self.subTest(idx=idx, value=value):
                strip = list(self.strip)
                strip[idx] = value
                with self.assertRaisesRegex(SystemExit, reason):
                    self.fields([8, 4, *strip, 255])
        for command in ([*self.strip, 255], [8, 4, 19, 2, 0, 0, 255], [8, 4, 255, 0], [8, 4]):
            with self.subTest(command=command), self.assertRaises(SystemExit):
                self.fields(command)

    def test_rejects_a_list_size_that_is_not_positive(self):
        polygons = halfwords(8, 4, *self.strip, 255)
        data = polygons + self.vertices
        for size in (-len(self.vertices), 0):
            with self.subTest(size=size), self.assertRaisesRegex(SystemExit, 'complete writable kart polygon list'):
                cart_polygons.patch_fields(object_at('poly', 'chunk_polygons', 0, size),
                                           object_at('vertices', 'chunk_vertices', len(polygons), len(self.vertices)),
                                           data, 0)
        self.assertEqual(len(cart_polygons.patch_fields(
            object_at('poly', 'chunk_polygons', 0, len(polygons)),
            object_at('vertices', 'chunk_vertices', len(polygons), len(self.vertices)), data, 0)), 4)

    def test_rejects_inconsistent_vertex_chunk_and_absent_end(self):
        for offset, value in ((0, 1), (2, 9), (4, 0xffff), (6, 4), (56, 0)):
            changed = bytearray(self.vertices)
            struct.pack_into('<H', changed, offset, value)
            with self.subTest(offset=offset), self.assertRaisesRegex(SystemExit, 'vertex extent'):
                self.fields([8, 4, *self.strip, 255], bytes(changed))
