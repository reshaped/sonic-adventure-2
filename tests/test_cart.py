"""The kart races, as the host runs their C: the harnesses of tests/cart/, and the kart lists' Ninja reader."""

import ctypes
import struct
import subprocess
import sys
import tempfile
import unittest
from pathlib import Path

from config import rows
from support import ROOT, compile_host


END = b'\xff\0'


def halves(*values):
    return struct.pack('<' + 'H' * len(values), *(v & 0xffff for v in values))


def vertex_chunk(first, count, opcode=41, stride=6):
    return halves(opcode, 1 + stride * count, first, count) + bytes(4 * stride * count)


def strip_chunk(opcode, strips, extras=0):
    payload = [len(strips) | extras << 14]
    for length, indices in strips:
        payload.append(length)
        for i, index in enumerate(indices):
            payload.append(index)
            if (opcode & 255) in (65, 66):
                payload += [0x8000, 0x7fff]
            if i >= 2:
                payload += [0x6543] * extras
    if len(payload) % 2:
        payload.append(0)
    return halves(opcode, len(payload), *payload)


class CartChunks(unittest.TestCase):
    """Check the downloadable kart lists against the portable Ninja reader."""

    @classmethod
    def setUpClass(cls):
        directory = tempfile.TemporaryDirectory()
        cls.addClassCleanup(directory.cleanup)
        library = Path(directory.name) / 'cart_chunks.so'
        compile_host(library, '-std=c89', '-O2', '-Imodules', 'engine/dc/cart_chunk.c', shared=True)
        word = ctypes.c_uint32
        check = ctypes.CDLL(str(library)).cart_chunk_streams_valid
        check.argtypes = [ctypes.c_char_p, word, word, word, word,
                          ctypes.POINTER(word), ctypes.POINTER(word)]
        check.restype = ctypes.c_int32
        cls.check = staticmethod(check)

    def admitted(self, data, vertices, polygons, textures=1):
        vlen, plen = ctypes.c_uint32(12345), ctypes.c_uint32(54321)
        if self.check(data, len(data), vertices, polygons, textures,
                      ctypes.byref(vlen), ctypes.byref(plen)):
            return vlen.value, plen.value
        self.assertEqual((vlen.value, plen.value), (12345, 54321))
        return None

    def lists(self, vertices, polygons, textures=1):
        data = bytes(4) + vertices + polygons
        return self.admitted(data, 4, 4 + len(vertices), textures)

    def test_disjoint_vertex_ranges_signed_strips_and_user_words(self):
        vertices = vertex_chunk(2, 2) + vertex_chunk(4094, 2, 0x8022, 3) + END + bytes(2)
        strips = strip_chunk(0x241, [(-4, [2, 4095, 3, 4094]), (3, [2, 3, 4095])], extras=2)
        textured = halves(0xc408, 0xe000) + strips + END
        self.assertEqual(self.lists(vertices, textured), (len(vertices), len(textured)))
        for index in [1, 4, 4096, 0xffff]:
            with self.subTest(index=index):
                plain = strip_chunk(64, [(3, [2, index, 4094])]) + END
                self.assertIsNone(self.lists(vertices, plain))
        # A shared polygon stream must be valid for each model's own vertices.
        missing = vertex_chunk(2, 2) + END + bytes(2)
        self.assertIsNone(self.lists(missing, textured))

    def test_vertex_records_and_boundaries(self):
        for header, payload in [(halves(41, 1, 0, 0), b''),
                                (halves(41, 1, 0, 1), b''),
                                (halves(41, 7, 0, 2), bytes(24)),
                                (halves(41, 13, 4095, 2), bytes(48)),
                                (halves(44, 8, 0, 1), bytes(28)),
                                (halves(0xff, 1), b'')]:
            with self.subTest(header=header):
                self.assertIsNone(self.lists(header + payload + END + bytes(2), END))
        vertices = vertex_chunk(0, 3) + END + bytes(2)
        for end in range(len(vertices)):
            self.assertIsNone(self.admitted(bytes(4) + vertices[:end], 4, 0))
        self.assertIsNone(self.admitted(bytes(4) + vertices, 5, 0))
        self.assertIsNone(self.admitted(bytes(4) + vertices, 0xffffffff, 0))
        self.assertEqual(self.admitted(bytes(4), 0, 0), (0, 0))

    def test_material_texture_and_strip_boundaries(self):
        vertices = vertex_chunk(0, 3) + END + bytes(2)
        material = halves(0x2513, 4) + bytes(8)
        texture = halves(0x3408, 0x4001)
        strip = strip_chunk(65, [(3, [0, 1, 2])])
        valid = material + texture + strip + END
        self.assertEqual(self.lists(vertices, valid, 2), (len(vertices), len(valid)))
        for end in range(len(valid)):
            with self.subTest(end=end):
                self.assertIsNone(self.lists(vertices, valid[:end], 2))
        self.assertIsNone(self.lists(vertices, valid, 1))
        self.assertIsNone(self.lists(vertices, strip + END, 2))
        for invalid in (halves(4), halves(5), halves(17, 1, 0),
                        halves(0) + material, texture + material, texture,
                        halves(64, 1, 0), halves(64, 2, 1, 2),
                        halves(64, 3, 1, 3, 0), halves(64, 5, 1, 3, 0, 1, 3),
                        halves(0x8040, 5, 1, 3, 0, 1, 2),
                        halves(64, 6, 1, 3, 0, 1, 2, 9),
                        halves(64, 5, 0x4001, 3, 0, 1, 2)):
            with self.subTest(invalid=invalid):
                self.assertIsNone(self.lists(vertices, invalid + END, 2))
        # A bump texture may consume six vector halfwords after a material
        # header. Both interpretations then resume at the following strip.
        bump = texture + halves(24, 6) + bytes(12) + strip + END
        self.assertEqual(self.lists(vertices, bump, 2), (len(vertices), len(bump)))


def course(first_kind):
    """A seven-gate course with one SET placement and a separate model directory."""
    pieces = bytes([3]) * 7
    data = bytearray(256)
    struct.pack_into('<7I', data, 0, 36, 168, 64, 28, len(pieces), 232, 24)
    data[28:35] = pieces
    data[104:112] = b'kart.adx'
    struct.pack_into('<I', data, 168, 1)
    struct.pack_into('<H', data, 200, first_kind)
    # A literal-only PRS file. All 256 body bytes consume complete control bytes.
    return b''.join(b'\xff' + data[i:i + 8] for i in range(0, len(data), 8)) + b'\x02\0\0'


class CartDownload(unittest.TestCase):
    """The kart stage publishes a new course only after its model has loaded."""

    def test_card_replacement(self):
        with tempfile.TemporaryDirectory() as scratch:
            work = Path(scratch)
            headers = work / 'system'
            headers.mkdir()
            (headers / 'areas.h').write_text(
                '#include <sg_xpt.h>\n#include "save/save.h"\n'
                'extern Uint8 kart_work[];\n#define WORK_AREA kart_work\n'
                '#define WORK_AREA_FILE (kart_work + DOWNLOAD_SIZE)\n')
            lib = work / 'kart.so'
            compile_host(lib, '-std=gnu89', '-O2', '-I', work, '-Imodules', '-Isrc', 'tests/cart/download.c',
                         'modules/cart/download.c', 'engine/dc/prs_checked.c', shared=True)
            kart = ctypes.CDLL(str(lib))
            kart.kart_test_card.argtypes = [ctypes.c_void_p, ctypes.c_uint32, ctypes.c_int32]
            kart.cart_download_copy_set.argtypes = [ctypes.c_void_p, ctypes.c_uint32]
            kart.cart_download_copy_layout.argtypes = [ctypes.c_void_p, ctypes.c_uint32]
            kart.cart_download_model_size.restype = ctypes.c_uint32

            def load(source, available=True):
                kart.kart_test_card(source, len(source) if source else 0, available)
                return kart.cart_download_load()

            def current_kind():
                dest = ctypes.create_string_buffer(64)
                kart.cart_download_copy_set(dest, len(dest))
                return struct.unpack_from('<H', dest, 32)[0]

            self.assertEqual(load(course(1)), 1)
            self.assertEqual(current_kind(), 1)
            self.assertEqual(kart.cart_download_model_size(), 24)
            pieces = ctypes.create_string_buffer(b'........')
            kart.cart_download_copy_layout(pieces, 6)
            self.assertEqual(pieces.value, b'........')
            kart.cart_download_copy_layout(pieces, 7)
            self.assertEqual(pieces.value, b'\x03' * 7 + b'.')
            self.assertEqual(load(None), 0)
            self.assertEqual(load(course(2), available=False), 0)
            self.assertEqual(current_kind(), 1)
            self.assertEqual(load(course(2)), 1)
            self.assertEqual(current_kind(), 2)


class CartLayouts(unittest.TestCase):
    """The courses' layouts are bodies, so a build without the disc holds none of their pieces."""

    # each course's layout in modules/cart/layout.c, in cart_course_mode's order
    COURSES = ('tails_pieces', 'rouge_pieces', 'beginner_pieces', 'standard_pieces', 'expert_pieces')

    def test_each_course_takes_its_datums_count_of_pieces(self):
        # each layout's bytes, as make offsets measures them in SHC's stub build
        sizes = {fields[1]: fields[4] for _, fields in rows(ROOT / 'offsets/modules/cart/layout.txt')}
        with tempfile.TemporaryDirectory() as directory:
            work = Path(directory)
            subprocess.run([sys.executable, ROOT / 'tools/body_data.py', 'cart', 'modules/cart/layout.c',
                            work, '--stub'], cwd=ROOT, check=True)
            for side, nonmatching in [('matching', False), ('port', True)]:
                with self.subTest(side=side):
                    program = work / side
                    compile_host(program, '-std=c89', '-O2', '-I', work / 'modules', '-Imodules', '-Isrc',
                                 'tests/cart/layout.c', nonmatching=nonmatching)
                    subprocess.run([program, *(sizes[course] for course in self.COURSES)], check=True)


class CartStage(unittest.TestCase):
    """Exercise the kart overlay across retained stage entries."""

    def test_course_files_and_deferred_lifetime(self):
        with tempfile.TemporaryDirectory() as directory:
            program = Path(directory) / 'sets'
            compile_host(program, '-std=gnu17', '-O2', '-Isrc', '-Imodules', 'tests/cart/stage_lifetime.c')
            subprocess.run([program], check=True)


class CartTraffic(unittest.TestCase):
    """Run Kart traffic admission with the real task scheduler and collision allocator."""

    def test_failed_car_leaves_the_remaining_traffic_live(self):
        with tempfile.TemporaryDirectory() as directory:
            work = Path(directory)
            subprocess.run([sys.executable, ROOT / 'tools/body_data.py', 'cart', 'modules/cart/traffic.c',
                            work, '--stub'], cwd=ROOT, check=True)
            program = work / 'traffic'
            compile_host(program, '-std=c89', '-O2', '-D_builtin_fabsf=__builtin_fabsf', '-I.', '-I', work / 'modules',
                         '-Imodules', '-Isrc', 'tests/cart/traffic.c', 'src/system/task.c', 'src/collision/collision.c',
                         '-lm')
            subprocess.run([program], check=True)


if __name__ == '__main__':
    unittest.main()
