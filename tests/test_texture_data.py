"""The scrolling UV strips consumed by texture_anim_scroll."""
from pathlib import Path
import subprocess
import tempfile
from types import SimpleNamespace
import struct
import unittest

from content import object_array_pointers
from support import compile_host
from texture_data import check_uvs, slice_pointers


class TextureUVs(unittest.TestCase):
    def check(self, words):
        data = struct.pack('<' + 'h' * len(words), *words)
        check_uvs(SimpleNamespace(size=len(data), where='tread'), data, 0)

    def test_each_strip_has_one_uv_pair_per_vertex_and_key(self):
        self.check([2, 1, 3, 2, -40, 100, 200, -300, 15, 1, 20, 22])
        self.check([1, 2, 5, 1, 0, 20, 24, 26])

    def test_rejects_short_or_trailing_keys_and_invalid_counts(self):
        for words in ([1, 1, 5], [1, 0], [1, 2, 5, 1, 2, 3], [1, 1, -1, 1, 0, 0],
                      [1, 1, 4, 0], [1, 1, 4, 1, 0, 1, 2]):
            with self.subTest(words=words), self.assertRaises(SystemExit):
                self.check(words)

    def test_texture_lists_keep_interior_entry_offsets(self):
        names = SimpleNamespace(address=0x2000, size=36, symbol='_skins')
        lists = SimpleNamespace(size=16, where='discharge', name='frames')
        data = struct.pack('<4I', 0x2000, 3, 0x200c, 2)
        self.assertEqual(list(slice_pointers(lists, names, data, 0)),
                         [(0, (0x2000, '_skins')), (8, (0x200c, '_skins+12'))])
        for address, count in ((0x2001, 1), (0x1ff4, 1), (0x2018, 2), (0x2000, 0)):
            with self.subTest(address=address, count=count), self.assertRaises(SystemExit):
                list(slice_pointers(lists, names, struct.pack('<4I', 0x2000, 3, address, count), 0))

    def test_water_frame_tables_bind_each_scroll_descriptor(self):
        frames = SimpleNamespace(kind='uv_scroll_ptrs', size=8, name='south_water', where='lagoon')
        keys = SimpleNamespace(kind='texture_uv_keys')
        scroll = SimpleNamespace(kind='uv_scroll')
        addresses = {0x2000: keys, 0x2010: scroll}
        data = struct.pack('<II', 0x2010, 0)
        self.assertEqual(list(object_array_pointers(frames, data, 0, addresses)),
                         [(0, scroll), (4, None)])
        with self.assertRaisesRegex(SystemExit, 'expected a uv_scroll'):
            list(object_array_pointers(frames, struct.pack('<II', 0x2000, 0), 0, addresses))


class TextureAnimation(unittest.TestCase):
    def test_empty_descriptor_and_timed_scroll(self):
        with tempfile.TemporaryDirectory() as temp:
            program = Path(temp) / 'scroll'
            compile_host(program, '-std=c89', '-O2', '-D_builtin_prefetch=__builtin_prefetch', '-Isrc',
                         'src/draw/texture_anim.c', 'tests/kernels/texture_scroll.c')
            subprocess.run([program], check=True)
