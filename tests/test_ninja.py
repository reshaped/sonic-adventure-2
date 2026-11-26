"""Ninja's drawing and motion, as the host runs their C: the harnesses of tests/ninja/, and the D8 vertex colors
drawn through it."""

import ctypes
import subprocess
import sys
import tempfile
import unittest
from pathlib import Path

from support import compile_host


class DirectCompile(unittest.TestCase):
    """Check that a direct object compile that writes no head writes through none."""

    def test_hidden_tree_writes_through_no_head(self):
        with tempfile.TemporaryDirectory() as directory:
            program = Path(directory) / 'direct_compile'
            compile_host(program, '-std=c89', '-O2', 'sdk/ninja/njCnkCompile.c', 'tests/ninja/direct_compile.c')
            subprocess.run([program], check=True, timeout=10)


WALKS = (
    ('njCnkMotionS', 'draw_motion_tree', 'draw_shape_tree'),
    ('njCnkMotionE', 'draw_motion_tree', 'draw_shape_tree'),
    ('njCnkMotionEM', 'draw_multi_motion_tree', 'draw_multi_shape_tree'),
    ('njCnkMotLinkS', 'njCnkSimplePushPopMotionLink', 'njCnkSimplePushPopShapeLink'),
    ('njCnkMotLinkE', 'njCnkEasyPushPopMotionLink', 'njCnkEasyPushPopShapeLink'),
)


class MotionMatrixStack(unittest.TestCase):
    """Check Ninja's animated object walks with both a full and a roomy matrix stack."""

    def test_cursor_and_stack_unwind(self):
        with tempfile.TemporaryDirectory() as directory:
            scratch = Path(directory)
            sources = []
            for unit, motion, shape in WALKS:
                source = scratch / (unit + '_fixture.c')
                source.write_text(f'#include "{unit}.c"\n'
                                  f'void run_{unit}(NJS_CNK_OBJECT *node, int shaped)\n'
                                  f'{{ if (shaped) {shape}(node); else {motion}(node); }}\n')
                sources.append(source)
            exe = scratch / 'motion_matrix_stack'
            compile_host(exe, '-std=c89', '-O2', '-Isdk/ninja', *sources, 'tests/ninja/motion_matrix.c')
            subprocess.run([exe], check=True)


# each simple multi clip that holds the near plane's test, and the near clip it calls
SOURCES = [f'sdk/ninja/njCnkSimpleMulti{name}.c' for name in (
    'PolygonClip', 'PolygonNearClip', 'PolygonEnvClip', 'TextureEnvNearClip', 'PolygonDiffEnvClip',
    'TextureDiffEnvNearClip', 'TextureClip', 'TextureNearClip', 'TextureDiffClip', 'TextureDiffNearClip')]


class NearClip(unittest.TestCase):
    """Check that a simple multi clip sends nothing for a triangle with no corner beyond the near plane."""

    def test_triangle_at_near_plane_sends_nothing(self):
        with tempfile.TemporaryDirectory() as directory:
            program = Path(directory) / 'near_clip'
            compile_host(program, '-std=c89', '-O2', '-D_builtin_prefetch=__builtin_prefetch', *SOURCES,
                         'tests/ninja/near_clip.c')
            subprocess.run([program], check=True, timeout=10)


class ShapeLink(unittest.TestCase):
    """Check that a shape link slot moves no normals its model's vertices lack."""

    def test_slot_without_setup_moves_no_missing_normals(self):
        with tempfile.TemporaryDirectory() as directory:
            program = Path(directory) / 'shape_link'
            compile_host(program, '-std=c89', '-O2', 'sdk/ninja/njCnkShapeLinkS.c', 'tests/ninja/shape_link.c')
            subprocess.run([program], check=True, timeout=10)


class Vertex(ctypes.Structure):
    _fields_ = [('point', ctypes.c_float * 3), ('color', ctypes.c_uint32)]


class VertexColors(unittest.TestCase):
    """Check D8 vertex updates in a model whose points and colors have the SH-4's widths."""

    def test_only_vertex_colors_change(self):
        with tempfile.TemporaryDirectory() as directory:
            library = Path(directory) / 'colors.so'
            compile_host(library, '-std=c89', '-O2', '-Isrc', '-Imodules', 'modules/shared/vertex_colors.c',
                         shared=True)
            setter = ctypes.CDLL(str(library)).vlist_set_color
            setter.argtypes = [ctypes.POINTER(ctypes.c_int32), ctypes.c_uint32]
            setter.restype = ctypes.c_int32
            for count in (0, 1, 2, 19, 16383):
                class Chunk(ctypes.Structure):
                    _fields_ = [('before', ctypes.c_uint32), ('head', ctypes.c_int32 * 2),
                                ('vertices', Vertex * count), ('end', ctypes.c_int32), ('after', ctypes.c_uint32)]

                chunk = Chunk()
                ctypes.memset(ctypes.byref(chunk), 0xA5, ctypes.sizeof(chunk))
                chunk.head[:] = (0x23 | ((1 + 4 * count) << 16), 17 | (count << 16))
                chunk.end = 0xFF
                for index, vertex in enumerate(chunk.vertices):
                    vertex.point[:] = (float(index), -1.25, 2.5)
                expected = bytearray(bytes(chunk))
                for color in (0, 0xFFFFFFFF, 0x80AA05FC):
                    with self.subTest(count=count, color=color):
                        self.assertEqual(setter(chunk.head, color), 0)
                        packed = color.to_bytes(ctypes.sizeof(ctypes.c_uint32), sys.byteorder)
                        for index in range(count):
                            start = Chunk.vertices.offset + index * ctypes.sizeof(Vertex) + Vertex.color.offset
                            expected[start:start + len(packed)] = packed
                        self.assertEqual(bytes(chunk), expected)


if __name__ == '__main__':
    unittest.main()
