"""Inline bodies: the lines that name them, and where a listing puts each."""

import csource
import inline
import splice
from support import TreeTest, obj

# asmsh's listing of a text with two functions of C around two inline
# bodies' own, and a constant section between its two runs of code
LISTING = '''\
*** SuperH RISC engine ASSEMBLER Ver. 4.1B ***    11/13/26 23:57:35                  PAGE     1
PROGRAM NAME =

    1                               1               .EXPORT     _twice
    2 00000000                      2               .SECTION    PSG,CODE,ALIGN=4
    3 00000000                      3     _first:                          ; function: first
    4 00000000 000B                 4               RTS
    5 00000002 0009                 5               NOP
    6 00000004                      6     _twice:                          ; function: twice
    7                               7                                      ; frame size=0
    8 00000004 344C                 8             ADD         R4,R4
    9 00000008                      9               .ALIGN      4
   10 00000008 000B                10               RTS
   11 0000000A 0009                11               NOP
   12 0000000C                     12     __$last:                         ; function: last
   13 0000000C 000B                13               RTS
   14 0000000E 0009                14               NOP
   15 00000000                     15               .SECTION    CSG,DATA,ALIGN=4
   16 00000000                     16     _table:                          ; function: thrice
   17 00000000                     17               .SECTION    PSG,CODE,ALIGN=4
   18 00000010                     18     _thrice:                         ; function: thrice
   19 00000010 344C                19             ADD         R4,R4
   20 00000020                     20               .ALIGN      4
'''
# a copy of a body whose constant asmsh keeps in a pool among its lines
POOLED = '''\
    1 00000000                      1               .SECTION    PSG,CODE,ALIGN=4
    2 00000000                      2     _first:                          ; function: first
    3 00000000 D001                 3             MOV.L       #H'12345678,R0
    4 00000002 A003                 4             BRA         ?on
    5 00000004 0009                 5             NOP
    6                                     ***** BEGIN-POOL *****
    7 00000006 0000                       ALIGNMENT CODE
    8 00000008 12345678                   DATA FOR SOURCE-LINE 3
    9                                     ***** END-POOL *****
   10 0000000C                      6     ?on:
   11 0000000C                      7               .ALIGN      4
   12 0000000C 000B                 8               RTS
   13 0000000E 0009                 9               NOP
'''
# Two PC-relative loads in a C caller, followed by its return and literal pool.
TERMINAL_POOL = """\
    1 00000000                      1               .SECTION    PSG,CODE,ALIGN=4
    2 00000000                      2     _caller:                         ; function: caller
    3 00000000 D002                 3             MOV.L       #H'12345678,R0
    4 00000002 6003                 4             MOV         R0,R0
    5 00000004                      5               .ALIGN      4
    6 00000004 D102                 6             MOV.L       #H'87654321,R1
    7 00000006 000B                 7             RTS
    8 00000008 0009                 8             NOP
    9                                     ***** BEGIN-POOL *****
   10 0000000A 0000                       ALIGNMENT CODE
   11 0000000C 12345678                   DATA FOR SOURCE-LINE 3
   12 00000010 87654321                   DATA FOR SOURCE-LINE 6
   13                                     ***** END-POOL *****
"""
# The same two loads of one value. asmsh gives both source-line numbers.
SHARED_POOL = TERMINAL_POOL.replace("D102", "D101").replace("#H'87654321", "#H'12345678").replace(
    'SOURCE-LINE 3\n   12 00000010 87654321                   DATA FOR SOURCE-LINE 6', 'SOURCE-LINE 3,6')
TWICE = '#pragma inline_asm(twice)\nint twice(int x)\n{\n        ADD         R4,R4\n}\n'
# the code sections' sizes, as copied() takes them
SIZES = {'PSG': 0x20}


# R11's emitted selector and asmsh's original address and word columns.
SELECTOR_LISTING = '''\
    4 00000000                      4               .SECTION    PSG,CODE,ALIGN=4
    5 00000000                      5     _njTexCopyWords:                 ; function: njTexCopyWords
    6                               6                                      ; frame size=0
    7 00000000                      7     L258:
    8 00000000 6056                 8               MOV.L       @R5+,R0
    9 00000002 4610                 9               DT          R6
   10 00000004 2402                10               MOV.L       R0,@R4
   11 00000006 8FFB                11               BF/S        L258
   12 00000008 7404                12               ADD         #4,R4
   13 0000000A 000B                13               RTS
   14 0000000C 0009                14               NOP
   15                              15             .EXPORT     _njTexSelectHalfword
   16                              16             .EXPORT     _njTexMemCopy2
   17                              17     ; The dispatcher branches here with the combined address and size bits in R0.
   18                              18     ; @asm open: TST reads the dispatcher's live R0 across the word-copy loop.
   19 0000000E                     19     _njTexSelectHalfword:
   20 0000000E C801                20             TST         #H'01,R0
   21 00000010 8B07                21             BF          _njTexMemCopy1
   22                              22     ; The public two-byte entry takes its byte count in R6.
   23                              23     ; @asm open: SHLR converts the count before the C halfword loop.
   24 00000012                     24     _njTexMemCopy2:
   25 00000012 4601                25             SHLR        R6
   26 00000014                     26               .ALIGN      4
'''

class Inline(TreeTest):
    def test_names_each_body_in_the_form(self):
        text = ('#pragma inline_asm(twice)\nint twice(int x)\n{\n}\n#pragma inline_asm(square(size=4))\n'
                '#pragma inline_asm(a, b)\n /* #pragma inline_asm(no) */\n#pragma inline_asm\n')
        self.assertEqual(inline.bodies(text), ['twice', 'square'])
        self.assertEqual(inline.NAMED.findall(text), ['#pragma inline_asm(twice)', '#pragma inline_asm(square(size=4))',
                                                      '#pragma inline_asm(a, b)', '#pragma inline_asm'])

    def test_a_body_runs_to_the_next_function_or_its_sections_end(self):
        sizes = {'PSG': 0x20, 'CSG': 8}
        self.assertEqual(inline.written(['twice', 'thrice'], LISTING, sizes), {'PSG': [(4, 0xC), (0x10, 0x20)]})
        self.assertEqual(inline.written(['last'], LISTING, sizes), {'PSG': [(0xC, 0x10)]})

    def test_a_long_label_opens_its_function_where_its_comment_is_below_it(self):
        sizes = {'PSG': 0x20, 'CSG': 8}
        # the same label too long for its comment to share its line
        split = LISTING.replace('__$last:                         ; function: last',
                                '__$last:\n   12                              12                                      ; function: last')
        self.assertEqual(inline.written(['twice'], split, sizes), {'PSG': [(4, 0xC)]})
        self.assertEqual(inline.written(['last'], split, sizes), {'PSG': [(0xC, 0x10)]})

    def test_a_body_shc_wrote_out_nowhere_gives_no_range(self):
        self.assertEqual(inline.written(['static_body'], LISTING, {'PSG': 0x20, 'CSG': 8}), {})

    def test_a_copy_runs_from_its_first_line_to_the_next_line_after_it(self):
        source = TWICE + 'int thrice(int x) { return twice(x); }\n'
        self.assertEqual(inline.copied(source, LISTING, SIZES, 'a.lis'), {'PSG': [(4, 8), (0x10, 0x20)]})

    def test_a_copy_holds_the_pool_among_its_lines(self):
        text = ("#pragma inline_asm(load)\nstatic void load(void)\n{\n        MOV.L       #H'12345678,R0\n"
                '        BRA         ?on\n        NOP\n?on:\n}\n')
        self.assertEqual(inline.copied(text, POOLED, {'PSG': 0x10}, 'a.lis'), {'PSG': [(0, 0xC)]})

    def test_static_copy_owns_its_literal_after_the_callers_return(self):
        load = "        MOV.L       #H'12345678,R0"
        source = f'#pragma inline_asm(load)\nstatic int load(void)\n{{\n{load}\n        MOV         R0,R0\n}}\n'
        self.write('src/a.c', source)
        self.write('build/obj/src/a.obj', obj([('PSG', 0x14, 4, True)]))
        self.write('build/obj/src/a.lis', TERMINAL_POOL)
        self.assertEqual(splice.assembly('src/a.c'), {'PSG': [(0, 4), (10, 16)]})

    def test_raw_and_copied_loads_share_one_pool_only_when_both_are_assembly(self):
        first = "        MOV.L       #H'12345678,R0"
        second = first.replace('R0', 'R1')
        source = f'#pragma inline_asm(load)\nstatic int load(void)\n{{\n{first}\n        MOV         R0,R0\n}}\n'
        self.write('build/obj/src/a.obj', obj([('PSG', 0x10, 4, True)]))
        self.write('build/obj/src/a.lis', SHARED_POOL)
        self.write('src/a.c', source)
        self.assertEqual(splice.assembly('src/a.c'), {'PSG': [(0, 4)]})
        self.write('src/a.c', source + f'#pragma asm\n{second}\n#pragma endasm\n')
        self.assertEqual(splice.assembly('src/a.c'), {'PSG': [(0, 6), (10, 16)]})

    def test_a_shared_literal_inside_the_copy_still_serves_a_c_instruction(self):
        body = "        MOV.L       #H'12345678,R1\n        RTS\n        NOP\n?done:"
        self.write('src/a.c', f'#pragma inline_asm(load)\nstatic void load(void)\n{{\n{body}\n}}\n')
        self.write('build/obj/src/a.obj', obj([('PSG', 0x10, 4, True)]))
        listing = SHARED_POOL + '   14 00000010                      9     ?done:\n'
        listing += '   15 00000010                     10               .ALIGN      4\n'
        self.write('build/obj/src/a.lis', listing)
        self.assertEqual(splice.assembly('src/a.c'), {'PSG': [(4, 10)]})

    def test_only_immediately_preceding_natural_padding_belongs_to_a_pool(self):
        ranges = {'PSG': [(0, 2)]}
        too_large = POOLED.replace('00000006 0000 ', '00000004 00000000 ')
        self.assertEqual(inline.pooled(too_large, ranges, 'pool.lis'), {'PSG': [(0, 2), (8, 12)]})
        interrupted = POOLED.replace('    8 00000008', '    8                               6     ; a source line\n'
                                     '    9 00000008')
        self.assertEqual(inline.pooled(interrupted, ranges, 'pool.lis'), {'PSG': [(0, 2), (8, 12)]})
        # Standalone alignment inside an existing written function still
        # belongs to it even when this listing also has a literal elsewhere.
        terminal = POOLED + '   15 00000010 0000                       ALIGNMENT CODE\n'
        self.assertEqual(inline.pooled(terminal, {'PSG': [(0, 0x12)]}, 'pool.lis'), {'PSG': [(0, 0x12)]})

    def test_a_partial_list_of_literal_owners_fails(self):
        # R32 truncates at column 132 without a continuation or warning. Its
        # final '2' here is the first digit of source line 28.
        prefix = '   32 00000038 12345678                   DATA FOR SOURCE-LINE '
        clipped = prefix + ','.join(str(n) for n in range(3, 28)) + ',2'
        self.assertEqual(len(clipped), 132)
        for row in (clipped, clipped[:-1] + ',', prefix + '3,', prefix + '3,,4'):
            with self.subTest(row=row):
                listing = POOLED + row + '\n'
                self.assertIn('a.lis: incomplete literal pool source-line list at PSG+0x38',
                              self.fails(inline.pooled, listing, {'PSG': [(0, 2)]}, 'a.lis'))

    def test_unknown_and_duplicate_owner_lines_fail(self):
        unknown = POOLED.replace('SOURCE-LINE 3', 'SOURCE-LINE 3,999')
        repeated = POOLED.replace('0009                 5', '0009                 3')
        for listing, number in ((unknown, 999), (repeated, 3)):
            self.assertIn(f'no unique instruction for literal pool source line {number} in PSG',
                          self.fails(inline.pooled, listing, {'PSG': [(0, 2)]}, 'pool.lis'))

    def test_sections_have_independent_instruction_owners_and_padding(self):
        listing = POOLED.replace('    8 00000008',
                                 '    8 00000000                      6               .SECTION    P,CODE,ALIGN=4\n'
                                 "    9 00000004 D200                 3             MOV.L       #H'12345678,R2\n"
                                 '   10 00000008')
        # Even a different code section with assembly at the same relative
        # address does not own this reference to PSG's assembler line 3.
        ranges = {'PSG': [(0, 2)], 'P': [(0, 2)]}
        self.assertEqual(inline.pooled(listing, ranges, 'pool.lis'), ranges)
        listing = listing.replace('   10 00000008', '   10 00000000 D001                 7             MOV.L       #0,R0\n'
                                  '   10 00000008').replace('SOURCE-LINE 3', 'SOURCE-LINE 7')
        self.assertEqual(inline.pooled(listing, ranges, 'pool.lis'), {'PSG': [(0, 2)], 'P': [(0, 2), (8, 12)]})

    def test_a_compiler_instruction_before_its_own_alignment_is_ambiguous(self):
        # R11's njSetDepthQueue return slot followed by the production align-pool patch.
        listing = """\
    2 00000000                      2               .SECTION    PSG,CODE,ALIGN=4
   69 0000005A                     69     _njSetDepthQueue:                ; function: njSetDepthQueue
   83 00000072 000B                83               RTS
   84 00000074 F33A                84               FMOV.S      FR3,@R3
   85 00000078                     85               .ALIGN      4
"""
        text = ('#pragma inline_asm(store)\nstatic void store(void)\n{\n          FMOV.S      FR3,@R3\n}\n'
                'void njSetDepthQueue(void) { if (0) store(); }\n')
        self.assertIn('indistinguishable from compiler-generated code',
                      self.fails(inline.copied, text, listing, {'PSG': 0x78}, 'depth.lis'))
        # A named standalone body supplies its own function label as evidence.
        self.assertEqual(inline.copied(text, listing.replace('njSetDepthQueue', 'store'),
                                       {'PSG': 0x78}, 'depth.lis'), {'PSG': [(0x74, 0x78)]})

    def test_an_unused_inline_body_cannot_claim_an_aligned_raw_instruction(self):
        source = ('#pragma inline_asm(step)\nstatic void step(void)\n{\n        NOP\n}\n'
                  'void only(void) {\n#pragma asm\n        NOP\n#pragma endasm\n}\n')
        listing = """\
    2 00000000                      2               .SECTION    PSG,CODE,ALIGN=4
    3 00000000                      3     _only:                           ; function: only
    4                               4                                      ; frame size=0
    5 00000000 0009                 5             NOP
    6 00000004                      6               .ALIGN      4
    7 00000004 000B                 7               RTS
    8 00000006 0009                 8               NOP
"""
        self.write('src/a.c', source)
        self.write('build/obj/src/a.obj', obj([('PSG', 8, 4, True)]))
        self.write('build/obj/src/a.lis', listing)
        self.assertIn('no copy of the inline body step', self.fails(splice.assembly, 'src/a.c'))
        self.assertIn('no copy of the inline body step', self.fails(inline.copied, source, listing, {'PSG': 8}, 'a.lis'))

    def test_space_after_a_bodys_brace_leaves_its_copies(self):
        source = TWICE.replace('{\n', '{ \n') + 'int thrice(int x) { return twice(x); }\n'
        self.assertEqual(inline.copied(source, LISTING, SIZES, 'a.lis'), {'PSG': [(4, 8), (0x10, 0x20)]})

    def test_a_named_body_whose_lines_cannot_be_read_fails(self):
        text = '#pragma inline_asm(twice)\nint twice(int x) {        ADD         R4,R4\n}\n'
        self.assertIn("a.lis: no lines between braces below the inline body twice's #pragma inline_asm",
                      self.fails(inline.copied, text, LISTING, SIZES, 'a.lis'))

    def test_only_ten_spaces_are_shcs_indentation(self):
        self.assertTrue(inline.compiler_columns(['          NOP']))
        self.assertFalse(inline.compiler_columns(['           NOP']))
        self.assertFalse(inline.compiler_columns(['        NOP']))

    def test_a_body_with_no_copy_fails(self):
        text = '#pragma inline_asm(unused)\nstatic void unused(void)\n{\n        SETT\n}\n'
        self.assertIn('a.lis: no copy of the inline body unused', self.fails(inline.copied, text, LISTING, SIZES, 'a.lis'))

    def test_raw_dispatch_counts_only_its_three_words(self):
        body = tuple(row[2].rstrip() for line in SELECTOR_LISTING.splitlines()[11:22]
                     if (row := inline.LINE.match(line)))
        source = '#pragma asm\n' + '\n'.join(body) + '\n#pragma endasm\n'
        self.assertEqual(inline.assembled(csource.assembly_blocks(source), SELECTOR_LISTING, 'selector.lis')[0],
                         {'PSG': [(0xE, 0x14)]})

    def test_raw_load_owns_its_aligned_pool_even_after_the_block(self):
        # Listing row numbers and assembler source line numbers diverge after a pool.
        listing = POOLED.replace('DATA FOR SOURCE-LINE 3', 'DATA FOR SOURCE-LINE 3,4')
        source = "#pragma asm\n        MOV.L       #H'12345678,R0\n        BRA         ?on\n#pragma endasm\n"
        self.assertEqual(inline.assembled(csource.assembly_blocks(source), listing, 'literal.lis')[0],
                         {'PSG': [(0, 4), (6, 12)]})
        # A generated C reference to that same value leaves the shared pool unattributed.
        listing = listing.replace('SOURCE-LINE 3,4', 'SOURCE-LINE 3,9')
        self.assertEqual(inline.assembled(csource.assembly_blocks(source), listing, 'literal.lis')[0],
                         {'PSG': [(0, 4)]})

    def test_raw_float_owns_only_its_natural_alignment_after_an_emitted_word(self):
        listing = """\
    1 00000000                      1               .SECTION    P,CODE,ALIGN=4
    2          00000004             2     FLOAT_ALIGN: .EQU 4
    3 00000000 C700                 3             MOVA        ?half,R0
    4 00000004                      4             .ALIGN      4
    5 00000004                      5     ?half:
    6 00000004 0000003F             6             .FDATA.S    F'0.5
    7 00000008 C701                 7             MOVA        ?two,R0
    8 0000000A                      8             .RES.B      4
    9 00000010                      9             .ALIGN      4
   10 00000010 00000040            10     ?two:   .FDATA.S    F'2.0
   11 00000014 C702                11             MOVA        ?one,R0
   12 00000020                     12             .ALIGN      32
   13 00000020 0000803F            13     ?one:   .FDATA.S    F'1.0
   14 00000028                     14             .ALIGN      8
"""
        body = [inline.LINE.match(line)[2] for line in listing.splitlines()[1:]]
        source = '#pragma asm\n' + '\n'.join(body) + '\n#pragma endasm\n'
        self.assertEqual(inline.assembled(csource.assembly_blocks(source), listing, 'data.lis')[0],
                         {'P': [(0, 10), (16, 22), (32, 36)]})

    def test_compiler_output_that_duplicates_a_handwritten_block_is_ambiguous(self):
        source = '#pragma asm\n        ADD         R4,R4\n#pragma endasm\n'
        blocks = csource.assembly_blocks(source)
        self.assertIn('#pragma asm block count differs: 1 in the source, 2 in the listing',
                      self.fails(inline.assembled, blocks, LISTING, 'a.lis'))
        self.assertEqual(inline.assembled(blocks + blocks, LISTING, 'a.lis')[0],
                         {'PSG': [(4, 6), (0x10, 0x12)]})

    def test_port_only_and_commented_blocks_do_not_require_a_native_listing(self):
        source = ('#ifdef NONMATCHING\n#pragma asm\n        NOP\n#pragma endasm\n#endif\n'
                  '/*\n#pragma asm\n        SHLR        R6\n#pragma endasm\n*/\n')
        self.assertEqual(csource.assembly_blocks(source), [])

    def test_ranges_that_meet_join(self):
        self.assertEqual(inline.joined({'P': [(8, 0x10), (0, 4)]}, {'P': [(4, 6), (0xC, 0x14)], 'Q': [(0, 2)]}),
                         {'P': [(0, 6), (8, 0x14)], 'Q': [(0, 2)]})
