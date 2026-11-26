"""The kinds table holds the original's layouts as its C types lay them out, and the walkers read Ninja's chunk lists
as its drawers do."""

import struct
import subprocess
import unittest

from kinds import (DECLARED, KINDS, LAND_OBJECT, PATH_POINTS, Chunk, ChunkError, Choice, Strip, polygon_chunks,
                   strips, vertex_chunks, vertex_range, walk)
from layout import Pointer, Record, Scalar, Types
from support import ROOT, SH4ZAM, SH_ELF, gcc_flags


# the headers that define the kinds' C types, in an order each compiles in
HEADERS = ['sdk/include/sg_xpt.h', 'sdk/include/ninja.h', 'sdk/include/nj_render_types.h', 'src/system/angle.h',
           'src/collision/world.h', 'src/collision/collision.h', 'src/stage/path.h', 'src/stage/hunt.h',
           'src/draw/texture_anim.h', 'src/draw/motion.h', 'src/system/texture.h', 'src/enemy/enemy.h',
           'src/player/player.h', 'modules/shared/common_object.h', 'modules/shared/staff_art.h',
           'modules/stg18/container_art.h', 'modules/chaostgrace/prize_art.h', 'modules/cart/objects.h',
           'modules/cart/race.h', 'modules/cart/track.h', 'modules/cart/texture_remap.h']
TYPED = {name: kind for name, kind in KINDS.items() if kind.ctype}
# the pointees ninja.h gives a kind's data in place of the kind's own type:
# NJS_CNK_OBJECT lies as NJS_OBJECT does, and NJS_MESHSET reads its UVs
# through NJS_COLOR
STAND_INS = {('NJS_OBJECT', 'chunk_object'), ('NJS_COLOR', 'basic_uvs')}


class Walkers(unittest.TestCase):
    def test_a_polygon_list_skips_each_class_of_chunk_by_its_type(self):
        halves = [0, 0x0801, 0x3408, 0x4001, 0x2517, 2, 1, 2, 56, 3, 0, 1, 2, 64, 2, 7, 8, 255]
        self.assertEqual(list(polygon_chunks(halves)),
                         [Chunk(0, 0, 1), Chunk(1, 1, 1), Chunk(2, 8, 2), Chunk(4, 23, 4), Chunk(8, 56, 5),
                          Chunk(13, 64, 4), Chunk(17, 255, 1)])

    def test_the_end_is_the_whole_halfword_and_a_flagged_type_255_is_a_strip(self):
        self.assertEqual(list(polygon_chunks([0x01ff, 0, 255])), [Chunk(0, 255, 2), Chunk(2, 255, 1)])

    def test_a_polygon_list_fails_a_vertex_chunk_an_overrun_and_a_missing_end(self):
        for halves, problem in (([34, 0, 255], 'vertex chunk type 34 at byte 0'),
                                ([8, 1, 17, 5, 0, 255], 'chunk at byte 4 runs past the list'),
                                ([8, 1], 'no NJD_CE by byte 4')):
            with self.subTest(halves=halves), self.assertRaisesRegex(ChunkError, problem):
                list(polygon_chunks(halves))

    def test_a_vertex_list_counts_words_by_its_head_and_ends_at_a_low_half(self):
        words = [(3 << 16) | 0x8022, (1 << 16) | 5, 1, 2, (4 << 16) | 35, (1 << 16) | 6, 1, 2, 3, 0xffff00ff]
        self.assertEqual(list(vertex_chunks(words)), [Chunk(0, 34, 4), Chunk(4, 35, 5), Chunk(9, 255, 1)])
        for words, problem in (([8, 255], 'chunk type 8 at byte 0 of a vertex list'),
                               ([51, 255], 'chunk type 51 at byte 0 of a vertex list'),
                               ([(9 << 16) | 34, 255], 'chunk at byte 0 runs past the list'),
                               ([(1 << 16) | 34, 0], 'no NJD_CE by byte 8')):
            with self.subTest(words=words), self.assertRaisesRegex(ChunkError, problem):
                list(vertex_chunks(words))

    def test_a_vertex_chunk_holds_its_count_of_its_types_vertices(self):
        # two NJD_CV_VN vertices of six words, into the buffer from index 5
        words = [(13 << 16) | 41, (2 << 16) | 5, *range(12), 255]
        chunk = next(vertex_chunks(words))
        self.assertEqual(vertex_range(words, chunk), range(5, 7))
        for count, problem in ((3, 'chunk type 41 at byte 0 holds 56 bytes, where its vertex count, 3, needs 80'),
                               (1, 'chunk type 41 at byte 0 holds 56 bytes, where its vertex count, 1, needs 32')):
            words[1] = (count << 16) | 5
            with self.subTest(count=count), self.assertRaisesRegex(ChunkError, problem):
                vertex_range(words, chunk)

    def test_strips_take_their_lengths_vertices_and_user_halfwords(self):
        # two strips with one user halfword after each vertex past the first two, then a halfword of padding
        first = [0xfffd, 1, 0, 0, 2, 0, 0, 3, 0, 0, 9]
        second = [4, 4, 0, 0, 5, 0, 0, 6, 0, 0, 9, 7, 0, 0, 9]
        halves = [0x0141, 28, (1 << 14) | 2, *first, *second, 0, 255]
        chunk = next(polygon_chunks(halves))
        found = list(strips(halves, chunk))
        self.assertEqual(found, [Strip(3, 3, 3, 1), Strip(14, 4, 3, 1)])
        self.assertEqual([strip.extent for strip in found], [11, 15])
        self.assertEqual([list(strip.vertices()) for strip in found], [[4, 7, 10], [15, 18, 21, 25]])
        self.assertEqual([halves[at] for strip in found for at in strip.vertices()], [1, 2, 3, 4, 5, 6, 7])
        self.assertEqual(list(strips([64, 5, 1, 3, 1, 2, 3, 255], Chunk(0, 64, 7))), [Strip(3, 3, 1, 0)])

    def test_strips_fail_an_undrawn_type_and_an_overrun(self):
        with self.assertRaisesRegex(ChunkError, 'no strips in chunk type 67 at byte 0'):
            list(strips([67, 4, 1, 3, 1, 2, 255], Chunk(0, 67, 6)))
        halves = [65, 5, 1, 2, 0, 0, 0, 255]
        with self.assertRaisesRegex(ChunkError, 'strip at byte 6 runs past its chunk'):
            list(strips(halves, Chunk(0, 65, 7)))

    def test_walk_holds_the_end_to_the_last_element(self):
        elements, chunks = walk('chunk_polygons', struct.pack('<3H', 8, 1, 255))
        self.assertEqual((elements, chunks), ([8, 1, 255], [Chunk(0, 8, 2), Chunk(2, 255, 1)]))
        with self.assertRaisesRegex(ChunkError, 'NJD_CE at byte 4, before the list ends at byte 8'):
            walk('chunk_polygons', struct.pack('<4H', 8, 1, 255, 0))
        with self.assertRaisesRegex(ChunkError, 'NJD_CE at byte 0, before the list ends at byte 8'):
            walk('chunk_vertices', struct.pack('<2I', 255, 255))
        for kind, data, problem in (('chunk_polygons', b'\xff\x00\x01', 'a partial element at byte 2'),
                                    ('chunk_vertices', struct.pack('<I', 255) + b'\x00', 'a partial element at byte 4')):
            with self.subTest(kind=kind), self.assertRaisesRegex(ChunkError, problem):
                walk(kind, data)

    def test_a_vertex_chunk_without_its_index_word_holds_no_range(self):
        words = [35, 255]
        with self.assertRaisesRegex(ChunkError, 'chunk type 35 at byte 0 holds no index and count'):
            vertex_range(words, next(vertex_chunks(words)))
        words = [(1 << 16) | 35, 5, 255]
        self.assertEqual(vertex_range(words, next(vertex_chunks(words))), range(5, 5))

    def test_a_choice_needs_its_whole_word(self):
        with self.assertRaisesRegex(ValueError, '31 bytes end before the 4-byte word at 28'):
            LAND_OBJECT.of(bytes(31))
        self.assertEqual(LAND_OBJECT.of(bytes(32)), 'basic_object')
        self.assertIsNone(PATH_POINTS.of(bytes([2, 0])))


class Table(unittest.TestCase):
    def setUp(self):
        self.types = Types([(ROOT / header).read_text(encoding='latin-1') for header in HEADERS])

    def test_each_kind_is_its_c_type_as_shc_lays_it_out(self):
        for name, kind in TYPED.items():
            with self.subTest(kind=name):
                layout = self.types.layout(kind.ctype)
                self.assertEqual(layout.size, kind.size)
                self.assertEqual(kind.alignment % layout.alignment, 0)
                if isinstance(layout, Pointer):
                    pointers = [('', 0)]
                else:
                    fields = layout.fields if isinstance(layout, Record) else ()
                    pointers = [(field, offset) for field, member, offset in fields if isinstance(member, Pointer)]
                self.assertEqual([(field.name, field.offset) for field in kind.pointers], pointers)
                for field in kind.pointers:
                    if isinstance(field.target, Choice):
                        # each choice is by its element's flags
                        flags = [(member, layout) for member, layout, offset in layout.fields
                                 if offset == field.target.offset]
                        self.assertEqual([member for member, _ in flags], ['flags'])
                        self.assertIsInstance(flags[0][1], Scalar)
                        self.assertEqual(flags[0][1].size, field.target.size)

    def test_each_pointer_points_at_its_target_kinds_c_type(self):
        for name, kind in TYPED.items():
            layout = self.types.layout(kind.ctype)
            # a Record's members by name, or an array's one pointer
            members = {field: member for field, member, _ in layout.fields} if isinstance(layout, Record) else {'': layout}
            for field in kind.pointers:
                targets = field.target.kinds if isinstance(field.target, Choice) else {field.target}
                pointee = members[field.name].pointee
                for target in sorted(targets & TYPED.keys()):
                    with self.subTest(kind=name, field=field.name, target=target):
                        if (pointee, target) in STAND_INS:
                            self.assertEqual(self.types.layout(pointee).size, TYPED[target].size)
                        elif pointee != 'void':
                            self.assertEqual(self.types.layout(pointee), self.types.layout(TYPED[target].ctype))

    def test_each_declared_type_is_its_records_type(self):
        for ctype, name in DECLARED.items():
            with self.subTest(ctype=ctype):
                self.assertEqual(KINDS[name].ctype, ctype)
                self.assertTrue(KINDS[name].record)

    @unittest.skipUnless((SH_ELF / 'bin/sh-elf-gcc').is_file() and (SH4ZAM / 'include/sh4zam').is_dir(),
                         'requires SH-4 GCC and SH4ZAM')
    def test_each_kind_is_its_c_type_as_make_dc_lays_it_out(self):
        # content assembles each kind at its alignment, which the port's code must find its type at
        roots = ('sdk/include/', 'src/', 'modules/')
        source = [f'#include "{header[len(root):]}"' for header in HEADERS for root in roots if header.startswith(root)]
        for index, (name, kind) in enumerate(TYPED.items()):
            source.append(f'typedef char size_{index}[sizeof({kind.ctype}) == {kind.size} ? 1 : -1];')
            source.append(f'typedef char align_{index}[{kind.alignment} % __alignof__({kind.ctype}) == 0 ? 1 : -1];')
            for field in kind.pointers:
                if field.name:
                    source.append(f'typedef char {name}_{field.name}['
                                  f'__builtin_offsetof({kind.ctype}, {field.name}) == {field.offset} ? 1 : -1];')
        subprocess.run([SH_ELF / 'bin/sh-elf-gcc', *gcc_flags(SH_ELF), '-isystem', SH4ZAM / 'include',
                        *(f'-I{root}' for root in roots), '-fsyntax-only', '-x', 'c', '-'],
                       input='\n'.join(source) + '\n', text=True, cwd=ROOT, check=True)


if __name__ == '__main__':
    unittest.main()
