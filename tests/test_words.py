"""The count an @hack tag gives: the words control reaches, and those that differ."""

import struct

import config
import words
from support import TreeTest

RTS, NOP = 0x000B, 0x0009


def code(*ops):
    return struct.pack(f'<{len(ops)}H', *ops)


class Reached(TreeTest):
    def test_runs_to_the_return_and_its_delay_slot(self):
        self.assertEqual(words.reached(code(0xE000, RTS, NOP, 0x1234), 0x8c010000), {0, 2, 4})

    def test_follows_both_ways_from_a_conditional_branch(self):
        # BT to the last return, over a return of its own
        function = code(0x8901, RTS, NOP, RTS, NOP)
        self.assertEqual(words.reached(function, 0x8c010000), {0, 2, 4, 6, 8})

    def test_a_branch_skips_what_it_jumps_over(self):
        # BRA over two words that nothing reaches
        function = code(0xA002, NOP, 0xFFFF, 0xFFFF, RTS, NOP)
        self.assertEqual(words.reached(function, 0x8c010000), {0, 2, 8, 10})

    def test_goes_on_past_a_call(self):
        function = code(0x430B, NOP, RTS, NOP)
        self.assertEqual(words.reached(function, 0x8c010000), {0, 2, 4, 6})

    def test_leaves_out_a_pool(self):
        # MOV.L @(disp,PC),R3 loads a long that follows the return
        function = code(0xD301, RTS, NOP, NOP) + struct.pack('<I', 0x8C123456)
        self.assertEqual(words.reached(function, 0x8c010000), {0, 2, 4})

    def test_reaches_a_switch_through_its_table(self):
        # MOVA table,R0; BRAF R0; NOP; a two-entry table; two cases
        function = code(0xC701, 0x0023, NOP, 0x0009) + struct.pack('<2h', 12 - 6, 16 - 6) + code(RTS, NOP, RTS, NOP)
        self.assertEqual(words.reached(function, 0x8c010000), {0, 2, 4, 12, 14, 16, 18})

    def test_goes_on_past_a_delay_slot_a_branch_reaches(self):
        # BT to the first return's delay slot, which runs on to the second return
        function = code(0x8900, RTS, NOP, RTS, NOP)
        self.assertEqual(words.reached(function, 0x8c010000), {0, 2, 4, 6, 8})

    def test_takes_a_switch_table_from_the_path_to_braf(self):
        # MOVA table,R0; BT to BRAF R0; MOVA elsewhere,R0; RTS; NOP; BRAF's NOP, a table, and its case
        function = code(0xC703, 0x8902, 0xC700, RTS, NOP, 0x0023, NOP, NOP) + struct.pack('<h', 4) + code(RTS, NOP)
        self.assertEqual(words.reached(function, 0x8c010000), {0, 2, 4, 6, 8, 10, 12, 18, 20})


class Counts(TreeTest):
    def test_counts_reached_words_that_differ_at_their_offsets(self):
        theirs = code(0xE000, 0xE101, RTS, NOP) + struct.pack('<I', 0x11111111)
        ours = code(0xE000, 0xE102, RTS, NOP) + struct.pack('<I', 0x22222222)
        self.assertEqual(words.differing(ours, theirs, 0x8c010000), (1, 4, 0x8c010002))
        self.assertEqual(words.differing(theirs, theirs, 0x8c010000), (0, 4, None))

    def test_a_byte_the_link_leaves_out_differs(self):
        theirs = code(0xE000, RTS, NOP)
        self.assertEqual(words.differing([0x00, 0xE0, 0x0B, 0x00, 0x09, None], theirs, 0x8c010000),
                         (1, 3, 0x8c010004))

    def test_a_function_runs_to_the_next_name_or_its_rows_end(self):
        self.write('src/a.c', '')
        self.image('1st_read', 'file 1ST_READ.BIN\nP 0x8c010000 0x8c010100\nsrc/a.c P 0x8c010000 0x8c010040\n',
                   '0x8c010000 first\n0x8c010010 second\n0x8c010010 alias\n0x8c010040 other\n')
        image = config.Image('1st_read')
        self.assertEqual(words.functions(image, image.held('src/a.c')),
                         [('first', 0x8c010000, 0x8c010010), ('second', 0x8c010010, 0x8c010040)])


class Measures(TreeTest):
    def setUp(self):
        super().setUp()
        self.write('src/a.c', '')
        self.write('build/obj/src/a.obj', b'')

    def test_fails_where_no_name_falls_in_the_code(self):
        self.image('1st_read', 'file 1ST_READ.BIN\nP 0x8c010000 0x8c010100\nsrc/a.c P 0x8c010000 0x8c010040\n',
                   '0x8c010040 other\n')
        self.write('orig/1ST_READ.BIN', bytes(0x100))
        self.assertIn("src/a.c: 1st_read's symbols name no address in its code",
                      self.fails(words.main, ['1st_read', 'src/a.c']))

    def test_fails_where_the_original_ends_inside_a_function(self):
        self.image('1st_read', 'file 1ST_READ.BIN\nP 0x8c010000 0x8c010100\nsrc/a.c P 0x8c010000 0x8c010040\n',
                   '0x8c010000 first\n')
        self.write('orig/1ST_READ.BIN', bytes(0x20))
        self.assertIn('orig/1ST_READ.BIN: 0x8c010000 to 0x8c010040 lies outside its 32 bytes from 0x8c010000',
                      self.fails(words.main, ['1st_read', 'src/a.c']))
