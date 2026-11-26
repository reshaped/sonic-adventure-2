"""The splicer: what writes each byte, and each way an image fails."""

from pathlib import Path
import unittest
from unittest import mock

import config
import content
import splice
from support import TreeTest, obj, record, run

SPLITS = """\
file    1ST_READ.BIN
P       0x8c010000  0x8c010040
D       0x8c010040  0x8c010048
B       0x8c010048  0x8c010050
disc       P  0x8c010000  0x8c010008
src/a.c    P  0x8c010008  0x8c010018
disc       P  0x8c010010  0x8c010014
src/b.c    P  0x8c010020  0x8c010040
disc       D  0x8c010040  0x8c010048
src/a.c    B  0x8c010048  0x8c010050
"""
# the image: the disc's bytes, a.c's with four the disc keeps, fill to b.c on
# 32, b.c's, data, and stored bss
ORIGINAL = b'D' * 8 + b'a' * 8 + b'XXXX' + b'a' * 4 + bytes(8) + b'b' * 32 + b'D' * 8 + bytes(8)
LINKED = {'build/link/1st_read/src/a.bin': b'a' * 8 + b'????' + b'a' * 4, 'build/link/1st_read/src/b.bin': b'b' * 32}
# asmsh's listing of a.c's text: a function of C, then an inline body's
LISTING = '''\
    1                               1               .EXPORT     _twice
    2 00000000                      2               .SECTION    P,CODE,ALIGN=4
    3 00000000                      3     _first:                          ; function: first
    4 00000000 000B                 4               RTS
    5 00000002 0009                 5               NOP
    6 00000008                      6     _twice:                          ; function: twice
    7 00000008 344C                 7             ADD         R4,R4
    8 0000000C                      8               .ALIGN      4
'''


# Real R11 source and assembler listing for a function with two inlined
# copies, then an independent file-scope instruction block in the same section.
MIXED = '''\
#pragma inline_asm(double_value)
int double_value(int value)
{
        MOV         R4,R0
        ADD         R0,R0
}

#pragma asm
        .RES.W      2
        .EXPORT     _fixture_selector
_fixture_selector:
        SHLL        R4
        .ALIGN      4
        .DATA.W     H'1234
        .RES.W      2
fixture_shift:
        SHLR        R4
#pragma endasm

int first(int value)
{
    return double_value(value) + double_value(value + 1);
}

int last(int value)
{
    return (value + 1) * 2;
}
'''
MIXED_LISTING = '''\
    1                               1               .EXPORT     _double_value
    2                               2               .EXPORT     _first
    3                               3               .EXPORT     _last
    4 00000000                      4               .SECTION    P,CODE,ALIGN=4
    5 00000000                      5     _double_value:                   ; function: double_value
    6                               6                                      ; frame size=0
    7 00000000 6043                 7             MOV         R4,R0
    8 00000002 300C                 8             ADD         R0,R0
    9 00000004                      9               .ALIGN      4
   10 00000004 000B                10               RTS
   11 00000006 0009                11               NOP
   12 00000008                     12     L243:
   13 00000008 00000004            13             .RES.W      2
   14                              14             .EXPORT     _fixture_selector
   15 0000000C                     15     _fixture_selector:
   16 0000000C 4400                16             SHLL        R4
   17 00000010                     17             .ALIGN      4
   18 00000010 1234                18             .DATA.W     H'1234
   19 00000012 00000004            19             .RES.W      2
   20 00000016                     20     fixture_shift:
   21 00000016 4401                21             SHLR        R4
   22 00000018                     22               .ALIGN      4
   23 00000018                     23     _first:                          ; function: first
   24                              24                                      ; frame size=8
   25 00000018 2F86                25               MOV.L       R8,@-R15
   26 0000001A 7FFC                26               ADD         #-4,R15
   27 0000001C 2F42                27               MOV.L       R4,@R15
   28 0000001E 6043                28             MOV         R4,R0
   29 00000020 300C                29             ADD         R0,R0
   30 00000024                     30               .ALIGN      4
   31 00000024 64F2                31               MOV.L       @R15,R4
   32 00000026 6803                32               MOV         R0,R8
   33 00000028 7401                33               ADD         #1,R4
   34 0000002A                     34     L244:
   35 0000002A 6043                35             MOV         R4,R0
   36 0000002C 300C                36             ADD         R0,R0
   37 00000030                     37               .ALIGN      4
   38 00000030 308C                38               ADD         R8,R0
   39 00000032 7F04                39               ADD         #4,R15
   40 00000034 000B                40               RTS
   41 00000036 68F6                41               MOV.L       @R15+,R8
   42 00000038                     42     _last:                           ; function: last
   43                              43                                      ; frame size=0
   44 00000038 6043                44               MOV         R4,R0
   45 0000003A 7001                45               ADD         #1,R0
   46 0000003C 000B                46               RTS
   47 0000003E 4000                47               SHLL        R0
   48                              48               .END
'''


def align(source, section):
    return 32


class Splice(TreeTest):
    def setUp(self):
        super().setUp()
        self.write('src/a.c', '')
        self.write('src/b.c', '')

    def read(self, splits=SPLITS):
        self.image('1st_read', splits)
        return config.Image('1st_read')

    def build(self, splits=SPLITS, original=ORIGINAL, linked=None):
        linked = LINKED if linked is None else linked
        return splice.splice(self.read(splits), original, align, lambda path: linked[str(path)])

    def test_gives_each_range_one_writer(self):
        self.assertEqual(splice.spans(self.read(), align), [
            (0x8c010000, 0x8c010008, 'disc', None), (0x8c010008, 0x8c010010, 'source', 'src/a.c'),
            (0x8c010010, 0x8c010014, 'disc', 'src/a.c'), (0x8c010014, 0x8c010018, 'source', 'src/a.c'),
            (0x8c010018, 0x8c010020, 'fill', None), (0x8c010020, 0x8c010040, 'source', 'src/b.c'),
            (0x8c010040, 0x8c010048, 'disc', None)])

    def test_builds_the_original(self):
        self.assertEqual(self.build(), ORIGINAL)

    def test_counts_code_and_data_by_writer(self):
        self.assertEqual(splice.counts(self.read(), align, lambda source: {}),
                         ({'disc': 12, 'C': 44, 'fill': 8}, {'disc': 8}))

    def test_counts_a_sources_datums_and_texts_in_initialized_data_as_generated_c(self):
        # b.c's table gives its D a datum, which the disc row inside b.c's row cuts, and a text. It gives its P a
        # function's entry and its bss a datum, neither of them generated C
        splits = SPLITS.replace('disc       D  0x8c010040  0x8c010048', 'src/b.c    D  0x8c010040  0x8c010048\n'
                                'disc       D  0x8c010044  0x8c010046').replace('src/a.c    B', 'src/b.c    B')
        self.write('offsets/src/b.txt', 'entry  b_main  P  0x00000000  2\ndatum  frames  D  0x00000000  6\n'
                   'text  LABEL  D  0x00000006  2\ndatum  state  B  0x00000000  4\n')
        image = self.read(splits)
        self.assertEqual(splice.generated_ranges(image, 'src/b.c'), [(0x8c010040, 0x8c010046), (0x8c010046, 0x8c010048)])
        # the datum's bytes outside the disc row, and the text's
        self.assertEqual(splice.counts(image, align, lambda source: {})[1], {'disc': 2, 'generated C': 6})
        self.write('offsets/src/b.txt', 'datum  frames  D  0x00000000  4\n')
        self.assertEqual(splice.counts(self.read(splits), align, lambda source: {})[1],
                         {'disc': 2, 'generated C': 4, 'C': 2})

    def test_counts_the_bss_the_file_stores_and_fails_where_the_counts_miss_a_stored_byte(self):
        image = self.read()
        self.assertEqual(splice.counts(image, align, lambda source: {}, stored=len(ORIGINAL))[1],
                         {'disc': 8, 'storage': 8})
        # a file that ends inside bss stores only the bss before its end
        self.assertEqual(splice.counts(image, align, lambda source: {}, stored=len(ORIGINAL) - 4)[1],
                         {'disc': 8, 'storage': 4})
        self.assertIn('1st_read: its categories count 80 of its 84 stored bytes',
                      self.fails(splice.counts, image, align, lambda source: {}, stored=len(ORIGINAL) + 4))

    def test_counts_each_image_it_splices_and_prints_the_counts_with_verbose(self):
        self.read()
        self.write('build/obj/src/a.obj', obj([('P', 0x10, 32, True), ('B', 8, 4, False)]))
        self.write('build/obj/src/b.obj', obj([('P', 0x20, 32, True)]))
        for path, data in LINKED.items():
            self.write(path, data)
        self.write('orig/1ST_READ.BIN', ORIGINAL)
        argv = ['1st_read', 'orig/1ST_READ.BIN', 'build/1ST_READ.BIN']
        # a count that misses a stored byte fails the splice, which then writes no image
        def miscounts(image, stored):
            config.fail(f'{image.name}: its categories count {stored - 1} of its {stored} stored bytes')
        with mock.patch.object(splice, 'counts', miscounts):
            self.assertIn('1st_read: its categories count 79 of its 80 stored bytes', self.fails(splice.main, argv))
        self.assertFalse(Path('build/1ST_READ.BIN').exists())
        self.assertEqual(run(splice.main, *argv), (0, '  splice build/1ST_READ.BIN\n'))
        self.assertEqual(run(splice.main, *argv, '--verbose'), (0, '  splice build/1ST_READ.BIN\n'
                                                                  '               44 code bytes from C\n'
                                                                  '               12 code bytes from disc\n'
                                                                  '                8 code bytes from fill\n'
                                                                  '                8 data bytes from disc\n'
                                                                  '                8 data bytes from storage\n'))

    def test_counts_assembly_where_the_object_holds_it(self):
        # b.c's first 8 bytes, and a.c's last 6, 2 of them under its disc row
        held = {'src/a.c': {'P': [(10, 16)]}, 'src/b.c': {'P': [(0, 8)]}}
        self.assertEqual(splice.counts(self.read(), align, lambda source: held[source])[0],
                         {'disc': 12, 'C': 32, 'assembly': 12, 'fill': 8})

    def test_two_object_sections_in_one_image_section(self):
        image = self.read(SPLITS.replace('src/b.c    P', 'src/a.c    PSECOND:P'))
        linked = {'build/link/1st_read/src/a.bin': b''.join(LINKED.values())}
        calls = []
        def input_alignment(source, section):
            calls.append((source, section))
            return 32
        self.assertEqual(splice.splice(image, ORIGINAL, input_alignment, lambda path: linked[str(path)]), ORIGINAL)
        self.assertEqual(set(calls), {('src/a.c', 'PSECOND')})
        # A range in PSECOND is relative to that input, even though its
        # output and source also hold the earlier range in P.
        assembled = {'P': [(10, 16)], 'PSECOND': [(0, 8)]}
        self.assertEqual(splice.counts(image, input_alignment, lambda source: assembled)[0],
                         {'disc': 12, 'C': 32, 'assembly': 12, 'fill': 8})

    def test_a_src_file_is_assembly_whole(self):
        self.write('src/c.src', '')
        self.write('build/obj/src/c.obj', obj([('P', 0x18, 4, True), ('D', 8, 4, False)]))
        self.assertEqual(splice.assembly('src/c.src'), {'P': [(0, 0x18)]})
        splits = SPLITS.replace('src/b.c    P  0x8c010020  0x8c010040', 'src/c.src  P  0x8c010020  0x8c010038\n'
                                'disc       P  0x8c010038  0x8c010040').replace('disc       D', 'src/c.src  D')
        self.assertEqual(splice.counts(self.read(splits), align),
                         ({'disc': 20, 'C': 12, 'assembly': 24, 'fill': 8}, {'assembly': 8}))

    def test_a_c_source_is_assembly_where_its_listing_puts_an_inline_body(self):
        self.write('src/a.c', '#pragma inline_asm(twice)\nint twice(int x)\n{\n        ADD         R4,R4\n}\n')
        self.write('build/obj/src/a.obj', obj([('P', 0x10, 4, True)]))
        self.write('build/obj/src/a.lis', LISTING)
        self.assertEqual(splice.assembly('src/a.c'), {'P': [(8, 0x10)]})
        self.assertEqual(splice.assembly('src/b.c'), {})

    def test_raw_instructions_and_inline_copies_share_a_code_section(self):
        self.write('src/a.c', MIXED)
        self.write('build/obj/src/a.obj', obj([('P', 0x40, 4, True)]))
        self.write('build/obj/src/a.lis', MIXED_LISTING)
        self.assertEqual(splice.assembly('src/a.c'),
                         {'P': [(0, 8), (0xC, 0xE), (0x16, 0x18), (0x1E, 0x24), (0x2A, 0x30)]})

    def test_assembly_directives_alone_need_no_object(self):
        self.write('src/a.c', '#pragma asm\n        .ALIGN      4\n        .RES.W      9\n#pragma endasm\n')
        self.assertEqual(splice.assembly('src/a.c'), {})

    def test_a_source_must_match_outside_its_disc_rows(self):
        wrong = dict(LINKED, **{'build/link/1st_read/src/b.bin': b'b' * 31 + b'!'})
        self.assertIn("src/b.c: its byte at 0x8c01003f differs from the disc's", self.fails(self.build, linked=wrong))

    def test_a_disc_row_over_bytes_that_match_has_no_reason_left(self):
        fixed = dict(LINKED, **{'build/link/1st_read/src/a.bin': b'a' * 8 + b'XXXX' + b'a' * 4})
        self.assertIn('src/a.c matches the disc from 0x8c010010 to 0x8c010014', self.fails(self.build, linked=fixed))

    def test_a_linked_source_must_be_its_rows_size(self):
        short = dict(LINKED, **{'build/link/1st_read/src/b.bin': b'b' * 31})
        self.assertIn("0x1f bytes, and src/b.c's rows hold 0x20", self.fails(self.build, linked=short))

    def test_output_section_alignment_needs_both_linked_bounds(self):
        splits = SPLITS.replace('0x8c010020  0x8c010040', '0x8c010020  0x8c01003e').replace(
            'disc       D', 'src/a.c    D')
        linked = dict(LINKED)
        linked['build/link/1st_read/src/b.bin'] = b'b' * 30
        words = bytes.fromhex('3e00018c4000018c')
        linked['build/link/1st_read/src/a.bin'] += words
        original = ORIGINAL[:0x3e] + bytes(2) + words + ORIGINAL[0x48:]
        bounds = 'P 0x8c010000 0x8c01003e\nD 0x8c010040 0x8c010048\n'
        self.write('config/1st_read/bounds.txt', bounds)
        # the gap is fill by D's first object's alignment alone, and a witness of its edges holds them too
        self.assertEqual(self.build(splits, original, linked), original)
        self.write('config/1st_read/fill.txt', 'P D 0x8c010040 0x8c010044\n')
        self.assertEqual(self.build(splits, original, linked), original)
        stale = splits.replace('src/a.c    D', 'disc       P  0x8c01003e  0x8c010040\nsrc/a.c    D')
        self.assertIn("holds only the linker's fill", self.fails(self.build, stale, original, linked))
        self.assertIn("isn't zero", self.fails(self.build, splits, original[:0x3e] + b'X' + original[0x3f:], linked))
        self.assertIn("not the next source's alignment", self.fails(
            lambda: splice.spans(self.read(splits), lambda source, section: 1, lambda path: linked[str(path)])))
        self.write('config/1st_read/fill.txt', 'P D 0x8c010044 0x8c010040\n')
        self.assertIn('does not hold 0x8c01003e', self.fails(self.build, splits, original, linked))
        self.write('config/1st_read/fill.txt', 'P D 0x8c010010 0x8c010044\n')
        self.assertIn('no source writes the word', self.fails(self.build, splits, original, linked))

    def test_a_splice_aligns_each_object_as_the_images_stubs_place_it(self):
        image = self.read()
        image.stubs = frozenset({'src/b.c'})
        with mock.patch.object(splice, 'alignment', return_value=32) as alignment:
            self.assertEqual(splice.splice(image, ORIGINAL, read=lambda path: LINKED[f'build/link/1st_read/src/'
                                                                                     f'{Path(path).name}']),
                             ORIGINAL)
        alignment.assert_called_with('src/b.c', 'P', image.stubs)

    def test_no_fill_where_the_alignment_puts_the_object_elsewhere(self):
        moved = SPLITS.replace('src/a.c    P  0x8c010008  0x8c010018', 'src/a.c    P  0x8c010008  0x8c010016')
        linked = dict(LINKED, **{'build/link/1st_read/src/a.bin': b'a' * 8 + b'????' + b'a' * 2})
        self.assertIn('nothing writes 0x8c010016 to 0x8c010020, code in P',
                      self.fails(lambda: splice.splice(self.read(moved), ORIGINAL, lambda source, section: 4,
                                                       lambda path: linked[str(path)])))

    def test_a_stored_byte_needs_a_writer(self):
        self.assertIn('nothing writes 0x8c010040 to 0x8c010048, data in D',
                      self.fails(self.build, SPLITS.replace('disc       D  0x8c010040  0x8c010048\n', '')))

    def test_fill_and_stored_bss_are_zeros(self):
        filled = ORIGINAL[:0x18] + b'\x09' + ORIGINAL[0x19:]
        self.assertIn("the disc's byte at 0x8c010018 isn't zero", self.fails(self.build, original=filled))
        stored = ORIGINAL[:-1] + b'\x01'
        self.assertIn("the disc's byte at 0x8c01004f isn't zero", self.fails(self.build, original=stored))

    def test_every_stored_byte_lies_in_a_section(self):
        self.assertIn('no section row holds 0x8c010050 to 0x8c010054', self.fails(self.build, original=ORIGINAL + bytes(4)))

    def test_a_row_past_the_file_fails(self):
        self.assertIn("lies outside the image's 0x40 stored bytes", self.fails(self.build, original=ORIGINAL[:0x40]))

    def test_a_whole_optical_filename_input_witnesses_just_its_native_alignment(self):
        splits = ('file 1ST_READ.BIN\nC 0x8c010000 0x8c010014\n'
                  'src/a.c C 0x8c010000 0x8c010005\ndisc C 0x8c010008 0x8c01000f\n'
                  'src/b.c C 0x8c010010 0x8c010014\n')
        image = self.read(splits)
        links = {'a.bin': b'first', 'b.bin': b'last'}
        read = lambda path: links[Path(path).name]
        original = b'first' + bytes(3) + b'abc\0de\0' + bytes(1) + b'last'
        self.write('config/1st_read/data.txt', '0x8c010008 layout names\n')
        self.write('layouts/names.txt', '.first texture_filename 4\n.second texture_filename 3\n')
        align4 = lambda source, section: 4
        self.assertEqual(splice.splice(image, original, align4, read), original)
        self.assertIn("isn't zero",
                      self.fails(splice.splice, image, original[:5] + b'x' + original[6:], align4, read))
        # A separate disc row cannot reclaim the three bytes of proven fill.
        covered = splits.replace('disc C', 'disc C 0x8c010005 0x8c010008\ndisc C', 1)
        self.assertIn("holds only the linker's fill",
                      self.fails(splice.spans, self.read(covered), align4, read, original))
        # The next native input still supplies its own alignment.
        self.assertIn('nothing writes 0x8c01000f to 0x8c010010',
                      self.fails(splice.check, image, lambda source, section: 1, read))
        # a row that is no pool leaves the gap unwritten: one too short, one of another kind, or a layout with
        # one. The splicer reads no field of another kind, so a chunk_polygons row of no words, which only the
        # GNU build refuses, can't fail a splice
        self.write('layouts/broken.txt', '.first texture_filename 4\n.broken chunk_polygons 0\n')
        for row in ('0x8c010008 .short texture_filename 6', '0x8c010008 .opaque chunk_polygons 3',
                    '0x8c010008 .broken chunk_polygons 0', '0x8c010008 layout broken', ''):
            self.write('config/1st_read/data.txt', row + '\n')
            with self.subTest(row=row):
                self.assertIn('nothing writes 0x8c010005 to 0x8c010008', self.fails(splice.check, image, align4, read))

    def test_a_pool_is_one_filename_row_or_a_layout_of_them_and_the_splicer_reads_no_other_row(self):
        splits = ('file 1ST_READ.BIN\nC 0x8c010000 0x8c010014\n'
                  'src/a.c C 0x8c010000 0x8c010005\ndisc C 0x8c010008 0x8c01000f\n'
                  'src/b.c C 0x8c010010 0x8c010014\n')
        image = self.read(splits)
        pool = image.disc()[0]
        self.write('layouts/names.txt', '.first texture_filename 4\n.second texture_filename 3\n')
        # the rows away from the pool's start are the GNU build's, and may hold what only it refuses: an address
        # the tables can't spell, or an import from nowhere
        others = '0X8C010010 .other no_such_kind\nimport nowhere nothing\n'
        for table in ('0x8c010008 .name texture_filename 7\n', '0x8c010008 .name texture_filename 7 3\n',
                      '0x8c010008 layout names\n' + others):
            self.write('config/1st_read/data.txt', table)
            with self.subTest(table=table):
                self.assertEqual(splice.filename_pools(image, [pool]), {pool})
        # a pool starts on a word, whether one row or a layout declares it. A layout whose string starts on the
        # next word would leave three bytes before it in the row
        moved = self.read(splits.replace('C 0x8c010008', 'C 0x8c010009'))
        self.write('layouts/padded.txt', '.name texture_filename 3\n')
        for table in ('0x8c010009 .name texture_filename 6\n', '0x8c010009 layout padded\n'):
            self.write('config/1st_read/data.txt', table)
            with self.subTest(table=table):
                self.assertEqual(splice.filename_pools(moved, moved.disc()), set())
        # where no placement in C starts on a word, the splicer opens no data.txt: a line of it that isn't UTF-8
        # fails only the GNU build
        self.write('config/1st_read/data.txt', b'# \xff\n')
        self.assertEqual(splice.filename_pools(moved, moved.disc()), set())
        self.assertIn("config/1st_read/data.txt:1: a byte that isn't UTF-8",
                      self.fails(splice.filename_pools, image, [pool]))
        # a filename at the start that the splicer can't size fails, naming its row
        for row in ('0x8c010008 .name texture_filename seven', '0x8c010008 .name texture_filename 7 3 1'):
            self.write('config/1st_read/data.txt', row + '\n')
            with self.subTest(row=row):
                self.assertIn('config/1st_read/data.txt:1: a texture_filename needs its byte count',
                              self.fails(splice.filename_pools, image, [pool]))
        # so does a string, its null included, that its capacity can't hold. One that fills it is a pool
        self.write('config/1st_read/data.txt', '0x8c010008 .name texture_filename 7 8\n')
        self.assertIn('config/1st_read/data.txt:1: a texture_filename string, its null included, is longer than its '
                      'capacity', self.fails(splice.filename_pools, image, [pool]))
        self.write('config/1st_read/data.txt', '0x8c010008 .name texture_filename 7 7\n')
        self.assertEqual(splice.filename_pools(image, [pool]), {pool})

    def test_a_disc_row_of_the_linkers_fill_alone_has_no_reason_left(self):
        filled = SPLITS.replace('src/b.c ', 'disc       P  0x8c010018  0x8c010020\nsrc/b.c ')
        self.assertIn("config/1st_read/splits.txt:8: the disc row from 0x8c010018 to 0x8c010020 holds only the "
                      "linker's fill before src/b.c",
                      self.fails(self.build, filled))
        # b.c's alignment shows only that fill could lie there: without the original a check passes the row, and
        # where the disc's bytes there aren't zeros, the row holds data
        splice.check(self.read(filled), align, lambda path: LINKED[str(path)])
        data = ORIGINAL[:0x18] + b'x' * 8 + ORIGINAL[0x20:]
        self.assertEqual(self.build(filled, data), data)
        # where b.c's object would start elsewhere, the disc's bytes there are no fill
        self.assertEqual(splice.spans(self.read(filled), lambda source, section: 64)[4],
                         (0x8c010018, 0x8c010020, 'disc', None))



class SectionEdges(TreeTest):
    """A section starts on its first object's alignment, so the gap from a
    section's last source to it is the linker's fill."""
    HEAD = """\
file    SONIC2/STG01.PRS
P       0x8c500000  0x8c500010
C       0x8c500010  {c_end}
{data}D       {d_start}  0x8c500020
src/a.c    P  0x8c500000  0x8c500010
src/a.c    C  0x8c500010  {a_end}
"""
    # constants that end three bytes short of a word, then data
    ORIGINAL = b'a' * 0x10 + b'c' * 5 + bytes(3) + b'd' * 8
    LINKED = {'a.bin': b'a' * 0x10 + b'c' * 5, 'b.bin': b'd' * 8}

    def setUp(self):
        super().setUp()
        for source in ('src/a.c', 'src/b.c'):
            self.write(source, '')

    def read(self, rows='src/b.c    D  0x8c500018  0x8c500020\n', c_end='0x8c500015', d_start='0x8c500018', data='',
             a_end='0x8c500015'):
        self.image('stg01', self.HEAD.format(c_end=c_end, d_start=d_start, data=data, a_end=a_end) + rows)
        return config.Image('stg01')

    def build(self, image, alignment=4, linked=None):
        linked = self.LINKED if linked is None else linked
        return splice.splice(image, self.ORIGINAL, lambda source, section: alignment,
                             lambda path: linked[Path(path).name])

    def test_the_gap_between_two_sections_rows_is_fill_on_the_first_objects_alignment(self):
        self.assertEqual(self.build(self.read()), self.ORIGINAL)
        self.assertIn((0x8c500015, 0x8c500018, 'fill', None), splice.spans(self.read(), lambda source, section: 4))
        self.assertEqual(splice.counts(self.read(), lambda source, section: 4, lambda source: {})[1],
                         {'C': 13, 'fill': 3})
        self.assertIn('no section row holds 0x8c500015 to 0x8c500018', self.fails(self.build, self.read(), 16))

    def test_the_gap_may_end_its_sections_row(self):
        self.assertEqual(self.build(self.read(c_end='0x8c500018')), self.ORIGINAL)

    def test_a_section_whose_row_starts_before_its_first_object_takes_no_fill(self):
        # b.c's alignment would start its object where D's row starts, and its row starts a word later
        spans = splice.spans(self.read('src/b.c    D  0x8c50001c  0x8c500020\n'), lambda source, section: 4)
        self.assertEqual([span for span in spans if span[2] == 'fill'], [])

    def test_a_section_of_stored_data_starts_on_a_word(self):
        # so a row can't open the next section with the gap after constants that end short of a word
        b = 'src/b.c    D  0x8c500018  0x8c500020\n'
        for image in (self.read('disc       D  0x8c500015  0x8c500018\n' + b, d_start='0x8c500015'),
                      self.read(data='DATA    0x8c500015  0x8c500018\n')):
            with self.subTest(splits=image.sections[2]):
                self.assertIn(f'section {image.sections[2][0]} starts at 0x8c500015, and a section of stored data '
                              'starts on 4', self.fails(splice.spans, image, lambda source, section: 4))

    def test_a_disc_row_that_starts_the_next_section_shows_no_alignment(self):
        # the disc's data opens D, so nothing shows where its object starts
        rows = 'disc       D  0x8c500018  0x8c50001c\nsrc/b.c    D  0x8c50001c  0x8c500020\n'
        linked = dict(self.LINKED, **{'b.bin': b'd' * 4})
        self.assertIn('no section row holds 0x8c500015 to 0x8c500018',
                      self.fails(self.build, self.read(rows), 4, linked))
        self.assertIn('no section row holds 0x8c500015 to 0x8c500018',
                      self.fails(self.build, self.read('disc       D  0x8c500018  0x8c500020\n')))
        # so the gap stays the disc's, and the row that holds it has its reason
        held = self.read('disc       C  0x8c500015  0x8c500018\n' + rows, c_end='0x8c500018')
        self.assertEqual(self.build(held, linked=linked), self.ORIGINAL)

    def test_a_disc_row_of_the_fill_at_a_sections_edge_has_no_reason_left(self):
        # the row ends C, opens D, or is the whole of a section. One that opens a section starts on a word,
        # so there the constants end on a word, and b.c's object aligns to 8
        b = 'src/b.c    D  0x8c500018  0x8c500020\n'
        on_a_word = {'a_end': '0x8c500014', 'c_end': '0x8c500014'}
        for image in (self.read('disc       C  0x8c500015  0x8c500018\n' + b, c_end='0x8c500018'),
                      self.read('disc       D  0x8c500014  0x8c500018\n' + b, d_start='0x8c500014', **on_a_word),
                      self.read('disc       DATA  0x8c500014  0x8c500018\n' + b,
                                data='DATA    0x8c500014  0x8c500018\n', **on_a_word)):
            row = image.placements[-2]
            fill = self.ORIGINAL[:0x14] + bytes(4) + self.ORIGINAL[0x18:]
            data = self.ORIGINAL[:0x14] + b'x' * 4 + self.ORIGINAL[0x18:]
            with self.subTest(splits=row):
                self.assertIn(f"the disc row from {config.hexa(row.start)} to 0x8c500018 holds only the linker's "
                              'fill at an edge',
                              self.fails(splice.spans, image, lambda source, section: 8, original=fill))
                # where the disc's bytes there aren't zeros, or the first object's alignment puts it elsewhere,
                # the row holds data
                self.assertEqual(splice.spans(image, lambda source, section: 8, original=data)[2],
                                 (row.start, row.end, 'disc', None))
                # where the first object's alignment puts it elsewhere, the row holds data
                self.assertEqual(splice.spans(image, lambda source, section: 16)[2],
                                 (row.start, row.end, 'disc', None))

    def test_a_row_that_opens_a_section_of_the_sdks_keeps_its_start(self):
        # the image's own record may start DSG before its first object, so the row shows that bound
        self.image('stg01', 'file    SONIC2/STG01.PRS\nP       0x8c500000  0x8c500010\nC       0x8c500010  0x8c500014\n'
                            'DSG     0x8c500014  0x8c500020\nsrc/a.c    P  0x8c500000  0x8c500010\n'
                            'src/a.c    C  0x8c500010  0x8c500014\ndisc       DSG  0x8c500014  0x8c500018\n'
                            'src/b.c    DSG  0x8c500018  0x8c500020\n')
        self.assertEqual(splice.spans(config.Image('stg01'), lambda source, section: 8)[2],
                         (0x8c500014, 0x8c500018, 'disc', None))


class CompleteInputs(TreeTest):
    splits = """file 1ST_READ.BIN
P 0x8c010000 0x8c010010
C 0x8c010010 0x8c010018
PSG 0x8c010018 0x8c01001c
src/a.c P 0x8c010000 0x8c010006
src/b.c P 0x8c010008 0x8c01000e
src/table.c C 0x8c010010 0x8c010018
src/start.c PSG 0x8c010018 0x8c01001c
"""
    # Digest of the two ordinary inputs in the original full link, including
    # their start/end and declared alignment. It is independent of splits.
    witness = 'P C inputs 3f677c4665b04f761dc569cc1e3c80941189cc9ae8e88da5321f1b2da280bcca __D_ROM 0x8c010018\n'
    pointer = bytes.fromhex('1000018c')
    original = b'a' * 6 + bytes(2) + b'b' * 6 + bytes(2) + b'c' * 8 + pointer

    def setUp(self):
        super().setUp()
        for name, section, data in [('a', 'P', b'a' * 6), ('b', 'P', b'b' * 6), ('table', 'C', b'c' * 8),
                                     ('start', 'PSG', self.pointer)]:
            self.write(f'src/{name}.c', '')
            self.write(f'build/link/1st_read/src/{name}.bin', data)
            self.write(f'build/obj/src/{name}.obj', obj([(section, len(data), 4, section != 'C')]))
        self.table()
        self.start()
        self.write('config/1st_read/fill.txt', self.witness)

    def table(self, offset=0, alignment=4):
        self.write('build/obj/src/table.obj', obj([('C', 8, alignment, False)], exports=[('__D_ROM', 0, offset)]))

    def start(self, offset=0, reference='__D_ROM'):
        data = obj([('PSG', 4, 4, True)], [reference])
        field = bytes([0x34]) + offset.to_bytes(4, 'big') + bytes.fromhex('18 08 10 08 08 08 00 08 04 02 00 00 ff')
        self.write('build/obj/src/start.obj', data[:-3] + record(0x1a, bytes(4)) + record(0x20, field) + data[-3:])

    def game(self, splits=None):
        self.image('1st_read', self.splits if splits is None else splits)
        return config.Image('1st_read')

    def test_whole_section_and_relocated_successor_count_only_fill(self):
        game = self.game()
        self.assertEqual(splice.splice(game, self.original), self.original)
        self.assertEqual(splice.counts(game), ({'C': 16, 'fill': 4}, {'C': 8}))
        for byte in (14, 15):
            with self.subTest(byte=byte):
                changed = self.original[:byte] + b'x' + self.original[byte + 1:]
                self.assertIn("isn't zero", self.fails(splice.splice, game, changed))

    def test_missing_or_substituted_owner(self):
        for row in ('src/a.c P 0x8c010000 0x8c010006\n', 'src/b.c P 0x8c010008 0x8c01000e\n'):
            with self.subTest(row=row):
                self.assertTrue(self.fails(splice.check, self.game(self.splits.replace(row, ''))))
        self.write('src/other.c', '')
        self.write('build/obj/src/other.obj', obj([('P', 6, 4, True)]))
        other = self.game(self.splits.replace('b.c', 'other.c'))
        self.assertIn('ordered P inputs differ', self.fails(splice.check, other))

    def test_incomplete_inputs_and_overrides(self):
        for row in ('disc P 0x8c010003 0x8c010004', 'disc P 0x8c010006 0x8c010008',
                    'disc P 0x8c01000e 0x8c010010'):
            with self.subTest(row=row):
                before = 'src/b.c P' if '0003' in row or '0006' in row else 'src/table.c'
                game = self.game(self.splits.replace(before, row + '\n' + before))
                self.assertIn('requires a complete ordinary input', self.fails(splice.check, game))
        self.write('build/obj/src/b.obj', obj([('P', 8, 4, True)]))
        self.assertIn('entire native P', self.fails(splice.check, self.game()))

    def test_interior_and_successor_alignment(self):
        self.write('build/obj/src/b.obj', obj([('P', 6, 2, True)]))
        self.assertIn('native alignment after', self.fails(splice.check, self.game()))
        self.write('build/obj/src/b.obj', obj([('P', 6, 4, True)]))
        self.table(alignment=2)
        self.assertIn('does not align', self.fails(splice.check, self.game()))

    def test_no_credit_without_a_witness_where_alignment_leaves_the_gap(self):
        self.write('config/1st_read/fill.txt', '')
        self.assertEqual(splice.counts(self.game()), ({'C': 16, 'fill': 4}, {'C': 8}))
        self.table(alignment=2)
        self.assertIn('nothing writes 0x8c01000e to 0x8c010010', self.fails(splice.check, self.game()))

    def test_successor_export_and_relocation(self):
        self.table(offset=4)
        self.assertIn('must begin', self.fails(splice.check, self.game()))
        self.table()
        for offset, symbol in [(2, '__D_ROM'), (0, '_other')]:
            with self.subTest(offset=offset, symbol=symbol):
                self.start(offset, symbol)
                self.assertIn('needs an independent relocation', self.fails(splice.check, self.game()))
        self.start()
        self.write('build/link/1st_read/src/start.bin', bytes.fromhex('0e00018c'))
        self.assertIn('does not hold 0x8c010010', self.fails(splice.check, self.game()))

    def test_successor_word_and_relocated_pointer_must_come_from_source(self):
        for row, reason in [('disc C 0x8c010010 0x8c010014\n', 'first word of C'),
                            ('disc PSG 0x8c010018 0x8c01001c\n', 'no source writes the word')]:
            with self.subTest(row=row):
                splits = self.splits + row
                if 'disc C' in row:
                    splits = self.splits.replace('src/start.c PSG', row + 'src/start.c PSG')
                self.assertIn(reason, self.fails(splice.check, self.game(splits)))


class EmptyConstantSection(TreeTest):
    splits = """file 1ST_READ.BIN
C 0x8c010000 0x8c010020
D 0x8c010020 0x8c010024
src/start.c C 0x8c010000 0x8c010008
src/math.c C 0x8c010008 0x8c010014
src/data.c D 0x8c010020 0x8c010024
"""
    witness = 'C D inputs fac0a0144d7964594195555f26faa4f96ecf0132f2a87ef378250e486bf52bd5 C32 0x8c010000\n'
    pointer = bytes.fromhex('2000018c')

    def setUp(self):
        super().setUp()
        self.game = self.prepare()
        self.write('config/1st_read/fill.txt', self.witness)
        self.write('config/1st_read/bounds.txt', 'D 0x8c010020 0x8c010024\n')
        self.startup()
        self.write('build/obj/src/math.obj', obj([('C', 12, 4, False)]))
        self.write('build/obj/src/data.obj', obj([('D', 4, 4, False)]))
        self.write('build/link/1st_read/src/start.bin', self.pointer + b's' * 4)
        self.write('build/link/1st_read/src/math.bin', b'm' * 12)
        self.write('build/link/1st_read/src/data.bin', b'd' * 4)

    def prepare(self, extra=''):
        for name in ('start', 'math', 'data'):
            self.write(f'src/{name}.c', '')
        self.image('1st_read', self.splits + extra)
        return config.Image('1st_read')

    def startup(self, sections=None, expression='050002ff', offset=0):
        if sections is None:
            sections = [('C', 8, 4, False), ('C32', 0, 32, False), ('D', 0, 4, False)]
        data = obj(sections, ['_data'])
        field = bytes.fromhex('35') + offset.to_bytes(4, 'big') + bytes.fromhex('18 08 10 08 08 08 00 08')
        expr = bytes.fromhex(expression)
        self.write('build/obj/src/start.obj', data[:-3] + record(0x1a, bytes(4)) +
                   record(0x20, field + bytes([len(expr)]) + expr) + data[-3:])

    def test_empty_c32_aligns_the_first_four_aligned_d_input(self):
        original = self.pointer + b's' * 4 + b'm' * 12 + bytes(12) + b'd' * 4
        self.assertEqual(splice.splice(self.game, original), original)
        self.assertEqual(splice.counts(self.game), ({}, {'C': 24, 'fill': 12}))
        self.assertIn("isn't zero", self.fails(splice.splice, self.game, original[:25] + b'x' + original[26:]))
        self.write('config/1st_read/fill.txt', '')
        self.assertIn('nothing writes', self.fails(splice.check, self.game))

    def test_c32_must_be_empty_and_in_its_original_position(self):
        c, inner, d = ('C', 8, 4, False), ('C32', 0, 32, False), ('D', 0, 4, False)
        for sections in ((c, d), (c, d, inner), (c, ('C32', 4, 32, False), d), (c, ('C32', 0, 16, False), d)):
            with self.subTest(sections=sections):
                self.startup(sections)
                self.assertIn('must declare empty C32', self.fails(splice.check, self.game))
        self.startup()
        for size, alignment in [(4, 32), (0, 64)]:
            with self.subTest(size=size, alignment=alignment):
                self.write('build/obj/src/data.obj', obj([('D', 4, 4, False), ('C32', size, alignment, False)]))
                self.assertIn('changes the empty C32', self.fails(splice.check, self.game))

    def test_only_startof_d_at_c_plus_zero_witnesses_the_successor(self):
        # A direct external reference, SIZEOF D and STARTOF C32 can all be
        # made to link to the same numeric value in a single-word fixture.
        for expression, offset in [('020000ff', 0), ('040002ff', 0), ('050001ff', 0), ('050002ff', 4)]:
            with self.subTest(expression=expression, offset=offset):
                self.startup(expression=expression, offset=offset)
                self.assertIn('relocation to STARTOF D', self.fails(splice.check, self.game))
        self.startup()
        self.write('build/link/1st_read/src/start.bin', bytes.fromhex('1400018c') + b's' * 4)
        self.assertIn('does not hold 0x8c010020', self.fails(splice.check, self.game))
        self.write('config/1st_read/bounds.txt', '')
        self.assertIn('needs its bounds.txt row', self.fails(splice.check, self.game))

    def test_d_head_and_the_whole_c_roster_still_have_to_match(self):
        self.write('build/obj/src/data.obj', obj([('D', 4, 64, False)]))
        self.assertIn('does not align', self.fails(splice.check, self.game))
        self.write('build/obj/src/data.obj', obj([('D', 4, 4, False)]))
        self.write('build/obj/src/math.obj', obj([('C', 11, 4, False)]))
        self.assertIn('entire native C', self.fails(splice.check, self.game))
        self.write('build/obj/src/math.obj', obj([('C', 12, 4, False)]))
        self.write('config/1st_read/fill.txt', self.witness.replace('fac0a0', '123456'))
        self.assertIn('ordered C inputs differ', self.fails(splice.check, self.game))


class Check(TreeTest):
    def setUp(self):
        super().setUp()
        self.write('src/a.c', '')
        self.write('src/b.c', '')

    def read(self, splits=SPLITS):
        self.image('1st_read', splits)
        return config.Image('1st_read')

    def check(self, splits=SPLITS, linked=None):
        linked = LINKED if linked is None else linked
        return splice.check(self.read(splits), align, lambda path: linked[str(path)])

    def test_needs_no_original(self):
        self.assertEqual(sorted(self.check()), ['src/a.c', 'src/b.c'])

    def test_fails_a_byte_with_no_writer(self):
        self.assertIn('nothing writes 0x8c010040 to 0x8c010048, data in D',
                      self.fails(self.check, SPLITS.replace('disc       D  0x8c010040  0x8c010048\n', '')))

    def test_fails_a_linked_source_that_isnt_its_rows_size(self):
        short = dict(LINKED, **{'build/link/1st_read/src/b.bin': b'b' * 31})
        self.assertIn("0x1f bytes, and src/b.c's rows hold 0x20", self.fails(self.check, linked=short))

    def test_takes_an_image_or_every_image_and_no_original(self):
        self.assertEqual(run(splice.main, '--check', '1st_read', 'orig/1ST_READ.BIN')[0], 2)
        self.assertEqual(run(splice.main, '1st_read', 'orig/1ST_READ.BIN')[0], 2)
        self.read()
        self.assertIn('no object at build/obj/src/', run(splice.main, '--check')[1])


class PaintingFilenamePools(unittest.TestCase):
    def test_each_stages_first_painting_owns_both_filenames(self):
        for name in ('stg25', 'stg26'):
            with self.subTest(stage=name):
                image = config.Image(name)
                shadow = image.symbol_addresses['painting_shadow_name']
                pool = next(row for row in image.disc() if row.start == shadow and row.section == 'C')
                # SHC put the shared shadow and the first picture in one word-aligned input.
                self.assertEqual(splice.filename_pools(image, [pool]), {pool})


class TheTree(unittest.TestCase):
    def test_each_pool_the_splicer_reads_holds_the_filenames_the_gnu_build_declares_there(self):
        # the splicer reads its pools without tools/content.py, which must find in each the same strings
        for name in config.images():
            image = config.Image(name)
            pools = splice.filename_pools(image, image.disc())
            declared = content.objects(image).values() if pools else ()
            for pool in sorted(pools, key=lambda row: row.start):
                with self.subTest(image=name, pool=config.hexa(pool.start)):
                    inside = [obj for obj in declared if pool.start <= obj.address < pool.end]
                    self.assertEqual({obj.kind for obj in inside}, {'texture_filename'})
                    self.assertEqual((inside[0].address, inside[-1].address + inside[-1].size), (pool.start, pool.end))
