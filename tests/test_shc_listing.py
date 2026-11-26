"""SHC's and asmsh's listings: where each datum, string and callback lies, the labels each function names, and the
templates each line of C loads."""

from pathlib import Path

import shc_listing
from support import TreeTest, obj


def size(count, section='D', kind='DATA'):
    """SHC's row that gives section, of kind, its size of count bytes."""
    return f'{f"{kind:8} SECTION ({section}):":51} {count:08X} Byte(s)'


# the row that gives a D section of 0x20 bytes its size
SIZE = size(0x20)


def label(offset, name, section='D'):
    """SHC's row of a static's label at offset."""
    return f'{section:3} {offset:08X}              L{offset + 1}:{" " * 28}; static: {name}'


def directive(offset, operands, op='.DATA.L'):
    """SHC's row of a directive at offset, in the section of the row above."""
    return f'    {offset:08X} {"":10} {op:12}{operands}'


def instruction(offset, code, text):
    """SHC's row of an instruction at offset, whose code is code, in the section of the row above."""
    return f'    {offset:08X} {code:10} {text}'


def more(operands):
    """SHC's row that goes on with the operands of the directive above it."""
    return f'{"":24}+{"":10}{operands}'


def marker(section):
    """The line marker -optimize=0 writes ahead of section's first code, which gives the section with no offset."""
    return f'{section:26}; File a.c      , Line 3        ; block'


def page(number):
    """The header SHC prints at the top of each page past the first."""
    return ['', f'SH SERIES C/C++ Compiler (Ver. 5.1 Release 13)  20-Oct-2026 16:26:38  PAGE {number:4}', '',
            'SCT OFFSET   CODE       C LABEL     INSTRUCTION OPERAND    COMMENT', '']


def assembled_listing(rows):
    """asmsh's listing of rows, which open a D section of 0x20 bytes: (offset, text, code), or a continuation's
    (offset, None, code)."""
    lines = ['\f*** SuperH RISC engine ASSEMBLER Ver. 4.0 ***    10/20/26 16:26:38', 'PROGRAM NAME =', '',
             f'    1 00000000 {"":16}     1               .SECTION    D,DATA,ALIGN=4']
    for number, (offset, text, *code) in enumerate(rows, 2):
        code = code[0] if code else ''
        if text is None:
            lines.append(f'      {offset:08X} {code}')
        elif text.startswith('L'):
            lines.append(f'{number:5} {offset:08X} {code:16} {number:5}     {text}')
        else:
            lines.append(f'{number:5} {offset:08X} {code:16} {number:5}               {text}')
    lines += ['*** SECTION DATA LIST', '', 'SECTION                          ATTRIBUTE    SIZE             START', '',
              'D                                REL-DATA    000000020        ']
    return '\n'.join(lines) + '\n'


class Listing(TreeTest):
    def listed(self, rows):
        """listing() of a listing of rows, one a line."""
        self.write('a.lis', '\n'.join(rows) + '\n')
        return shc_listing.listing(Path('a.lis'))

    def test_gives_each_named_datum_a_row_in_the_listings_order(self):
        # two functions' statics named at, with another datum between them
        rows = [label(0, 'at'), directive(0, "H'00000000,H'00000000"), label(8, 'other'), directive(8, "H'00000000"),
                label(12, 'at'), directive(12, "H'00000000"), SIZE]
        self.assertEqual(self.listed(rows)[0], [('at', 'D', 0, 8), ('other', 'D', 8, 4), ('at', 'D', 12, 4)])

    def test_a_datums_operands_go_on_in_the_rows_that_continue_them(self):
        # SHC writes a long .DATA.L's operands on rows of their own, each opening with a +
        rows = [label(0, 'table'), directive(0, "H'00000001,H'00000002"), more("H'00000003,H'00000004"),
                more("H'00000005"), SIZE]
        self.assertEqual(self.listed(rows)[0], [('table', 'D', 0, 20)])

    def test_gives_each_unnamed_datum_outside_the_code_and_the_labels_each_function_names(self):
        # two unnamed data in D, then f, whose code names its pool's label and, in its comments, the two data's
        # labels. The pool's label is code's, and no datum, and the pool's words end the datum above them
        rows = ['D   00000000              L41:', directive(0, "H'00000000"),
                'D   00000004              L40:', directive(4, "H'00000000,H'00000000"),
                'P   00000000              L1:                              ; function: f',
                instruction(0, 'D203', 'MOV.L       L2,R2; L40'), instruction(2, 'D304', 'MOV.L       L2+4,R3; L41'),
                '    00000010              L2:', instruction(0x10, '<00000000>', '.DATA.L     L40'),
                instruction(0x14, '<00000000>', '.DATA.L     L41'), SIZE]
        self.assertEqual(self.listed(rows),
                         ([], {'L41': ('D', 0, 4), 'L40': ('D', 4, 8)}, {'f': {'L1', 'L2', 'L40', 'L41'}}))

    def test_places_the_first_instruction_of_each_callback_asked_for(self):
        # step and draw begin with their own global labels. Only step is asked for, and draw's code is no datum
        rows = ['P   00000000              _step:                           ; function: step',
                instruction(0, '000B', 'RTS'), instruction(2, '0009', 'NOP'),
                '    00000004              _draw:                           ; function: draw',
                instruction(4, '000B', 'RTS'), instruction(6, '0009', 'NOP'),
                size(8, 'P', 'PROGRAM')]
        self.write('a.lis', '\n'.join(rows) + '\n')
        self.assertEqual(shc_listing.listing(Path('a.lis'), {'step'})[0], [('step', 'P', 0, 2)])

    def test_an_assembled_listing_places_each_callback_asked_for(self):
        # cb begins P with its own global label, and is asked for
        text = assembled_listing([(0, '.SECTION    P,CODE,ALIGN=4'),
                                  (0, '_cb:                             ; function: cb'),
                                  (0, 'RTS', '000B'), (2, 'NOP', '0009')])
        self.write('a.lis', text.replace('D                                REL-DATA    000000020',
                                         'P                                REL-CODE    000000004'))
        self.assertEqual(shc_listing.listing(Path('a.lis'), {'cb'})[0], [('cb', 'P', 0, 2)])

    def test_a_datum_ends_where_the_next_section_starts(self):
        # L1's one byte in D, then C's first row, whose bytes are no part of L1's though no label comes between
        rows = ['D   00000000              L1:', directive(0, "H'01", '.DATA.B'),
                f'C   00000000 {"":10} {".DATA.B":12}H\'02,H\'03', size(1), size(4, 'C', 'CONST')]
        self.write('a.lis', '\n'.join(rows) + '\n')
        self.assertEqual(shc_listing.listing(Path('a.lis'))[1], {'L1': ('D', 0, 1)})
        self.assertEqual(shc_listing.strings(Path('a.lis')), [('D', 0, b'\x01')])

    def test_needs_no_object_for_sections_of_one_abbreviation_that_hold_no_row_it_gives(self):
        # PAB1 and PAB2, which SHC's rows both show as PAB, hold only code, and nothing tells them apart. The string
        # in C is placed without them
        rows = ['PAB 00000000              _f:                              ; function: f',
                instruction(0, '000B', 'RTS'), instruction(2, '0009', 'NOP'),
                'PAB 00000000              _g:                              ; function: g',
                instruction(0, '000B', 'RTS'), instruction(2, '0009', 'NOP'),
                'C   00000000              L1:', directive(0, '"ab"', '.SDATA'), directive(2, "H'00", '.DATA.B'),
                size(4, 'PAB1', 'PROGRAM'), size(4, 'PAB2', 'PROGRAM'), size(4, 'C', 'CONST')]
        self.write('a.lis', '\n'.join(rows) + '\n')
        self.assertEqual(shc_listing.strings(Path('a.lis')), [('C', 0, b'ab\0')])
        self.assertEqual(shc_listing.listing(Path('a.lis'))[1], {'L1': ('C', 0, 3)})

    def test_an_assembled_listing_keeps_its_sections_full_names(self):
        # asmsh's sections DAT0 and DAT1, which SHC's rows would both show as DAT. DAT1 opens with 8 bytes no label
        # names, which are no part of first
        text = assembled_listing(
            [(0, 'L1:                              ; static: first'), (0, ".DATA.L     H'00000000", '00000000'),
             (0, '.SECTION    DAT1,DATA,ALIGN=4'), (0, ".DATA.L     H'00000000,H'00000000", '0000000000000000'),
             (8, 'L2:                              ; static: second'),
             (8, ".DATA.L     H'00000000,H'00000000", '0000000000000000')])
        text = text.replace('.SECTION    D,', '.SECTION    DAT0,').replace(
            'D                                REL-DATA    000000020',
            'DAT0                             REL-DATA    000000004\n'
            'DAT1                             REL-DATA    000000010')
        self.write('a.lis', text)
        self.assertEqual(shc_listing.listing(Path('a.lis'))[0], [('first', 'DAT0', 0, 4), ('second', 'DAT1', 8, 8)])

    def test_tells_two_sections_of_one_abbreviation_apart_by_the_objects_exports(self):
        # CSG and CSGATLINE, which SHC's rows both show as CSG. The object beside the listing exports table from
        # CSG. CSGATLINE exports nothing, and is the one left. DSG and DSGATLINE are empty, and no row needs them
        # told apart
        sizes = [size(8, 'CSG', 'CONST'), size(4, 'CSGATLINE', 'CONST'), size(0, 'DSG'), size(0, 'DSGATLINE')]
        rows = ['CSG 00000000              _table:                          ; static: table',
                directive(0, "H'00000000,H'00000000"), label(0, 'line', 'CSG'), directive(0, "H'00000000"), *sizes]
        sections = [('CSG', 8, 4, False), ('CSGATLINE', 4, 4, False)]
        self.write('a.obj', obj(sections, exports=[('_table', 0, 0)]))
        self.assertEqual(self.listed(rows)[0], [('table', 'CSG', 0, 8), ('line', 'CSGATLINE', 0, 4)])
        # a CSG row that doesn't start its section, one of the two sections' rows alone, an export at another
        # offset, no export, and exports that put both sections' rows in CSG
        both = [*rows[:2], f'CSG 00000000              {"_line:":33}; static: line', *rows[3:]]
        for case, exports, problem in (
                ([*rows[:2], label(4, 'line', 'CSG'), *rows[3:]], [('_table', 0, 0)],
                 r'a\.lis:3: ambiguous CSG section resumes at 00000004$'),
                ([*rows[:2], *sizes], [('_table', 0, 0)], r'a\.lis: cannot identify all CSG sections from the object$'),
                (rows, [('_table', 0, 4)], r'a\.lis:1: _table disagrees with its object offset$'),
                (rows, [], r'a\.lis: object exports do not distinguish the CSG sections$'),
                (both, [('_table', 0, 0), ('_line', 0, 0)],
                 r'a\.lis: object exports do not distinguish the CSG sections$')):
            self.write('a.obj', obj(sections, exports=exports))
            with self.subTest(problem), self.assertRaisesRegex(SystemExit, problem):
                self.listed(case)

    def test_a_line_marker_starts_a_section_of_one_abbreviation(self):
        # the same CSG and CSGATLINE, each named by the marker ahead of it, and not by its first row
        sizes = [size(8, 'CSG', 'CONST'), size(4, 'CSGATLINE', 'CONST')]
        rows = [marker('CSG'), '    00000000              _table:                          ; static: table',
                directive(0, "H'00000000,H'00000000"), marker('CSG'), label(0, 'line', ''), directive(0, "H'00000000"),
                *sizes]
        self.write('a.obj', obj([('CSG', 8, 4, False), ('CSGATLINE', 4, 4, False)], exports=[('_table', 0, 0)]))
        self.assertEqual(self.listed(rows)[0], [('table', 'CSG', 0, 8), ('line', 'CSGATLINE', 0, 4)])

    def test_an_assembled_listings_long_line_goes_on_in_its_code(self):
        # a line too long for the listing is cut short, and its code goes on in rows of their own
        text = assembled_listing([(0, 'L1:                              ; static: table'),
                                  (0, ".DATA.L     H'00000000,H'00000000,H'00000000,H'000", '0000000000000000'),
                                  (8, None, '0000000000000000')])
        self.write('a.lis', text)
        self.assertEqual(shc_listing.listing(Path('a.lis'))[0], [('table', 'D', 0, 16)])

    def test_an_assembled_listings_operands_go_on_in_the_lines_that_open_with_a_plus(self):
        # a .DATA.L whose source goes on in a line of its own, which gives its code
        text = assembled_listing([(0, 'L1:                              ; static: table'),
                                  (0, ".DATA.L     H'00000001,H'00000002,", '0000000100000002'),
                                  (8, "+           H'00000003", '00000003')])
        self.write('a.lis', text)
        self.assertEqual(shc_listing.listing(Path('a.lis'))[0], [('table', 'D', 0, 12)])

    def test_an_assembled_listing_gives_its_unnamed_data_and_the_labels_each_function_names(self):
        # f's code and pool name L2 and L40, and L40 is an unnamed datum in D, which the .ALIGN ends
        text = assembled_listing(
            [(0, '.SECTION    P,CODE,ALIGN=4'), (0, '_f:                              ; function: f'),
             (0, 'MOV.L       L2,R2', 'D201'), (2, 'RTS', '000B'), (4, 'NOP', '0009'), (8, 'L2:'),
             (8, '.DATA.L     L40', '00000000'), (0, '.SECTION    D,DATA,ALIGN=4'), (0, 'L40:'),
             (0, ".DATA.L     H'00000001", '00000001'), (4, '.ALIGN      8'),
             (8, ".DATA.L     H'00000002", '00000002')])
        self.write('a.lis', text)
        self.assertEqual(shc_listing.listing(Path('a.lis')), ([], {'L40': ('D', 0, 4)}, {'f': {'L2', 'L40'}}))

    def test_a_long_global_names_comment_goes_on_in_a_line_of_its_own(self):
        # SHC writes the label of a global whose name has 31 characters or more past its comment's column, and the
        # comment on the next line, or past the header of the next page
        name = 'a' * 31
        for gap in ([], page(2)):
            rows = [f'D   00000000              _{name}:', *gap, f'{"":59}; static: {name}',
                    directive(0, "H'00000000,H'00000000"), size(8)]
            with self.subTest(lines=len(gap)):
                self.assertEqual(self.listed(rows)[0], [(name, 'D', 0, 8)])

    def test_a_page_header_joins_no_label_to_another_names_comment(self):
        # a long global's label ends a page, and the next page opens with a comment that names another global
        name = 'a' * 31
        rows = [f'D   00000000              _{name}:', *page(2), f'{"":59}; static: b{name[1:]}',
                directive(0, "H'00000000"), size(4)]
        self.assertEqual(self.listed(rows)[0], [])

    def test_a_res_sizes_the_datum_it_starts_and_pads_the_one_whose_values_it_follows(self):
        # two reserved longwords, then a word and the .RES that pads it to the next datum, which adds nothing to
        # the word with a unit or without one, in SHC's listing and in asmsh's
        for padding in ('.RES.W', '.RES'):
            rows = [label(0, 'space'), directive(0, '2', '.RES.L'), label(8, 'word'),
                    directive(8, "H'0001", '.DATA.W'), directive(10, '1', padding), SIZE]
            self.write('a.lis', '\n'.join(rows) + '\n')
            self.write('b.lis', assembled_listing(
                [(0, 'L1:                              ; static: space'), (0, '.RES.L      2', '00000008'),
                 (8, 'L2:                              ; static: word'), (8, ".DATA.W     H'0001", '0001'),
                 (10, f'{padding:12}1', '00000002')]))
            for path in ('a.lis', 'b.lis'):
                with self.subTest(padding=padding, path=path):
                    self.assertEqual(shc_listing.listing(Path(path))[0], [('space', 'D', 0, 8), ('word', 'D', 8, 2)])

    def test_a_strings_bytes_give_its_datums_size(self):
        # a ; in a string, and the bytes c2 a3, which UTF-8 would read as one character
        rows = [label(0, 'semi'), directive(0, '"a;b"', '.SDATA'), label(4, 'pound'),
                directive(4, '"\xc2\xa3"', '.SDATA'), size(8)]
        self.write('a.lis', ('\n'.join(rows) + '\n').encode('latin-1'))
        self.assertEqual(shc_listing.listing(Path('a.lis'))[0], [('semi', 'D', 0, 3), ('pound', 'D', 4, 2)])

    def test_a_listing_fails_naming_the_line_of_a_datum_it_cant_size_or_place(self):
        # a datum past its section's 4 bytes, below a label whose comment goes on past the next page's header; a
        # count and a string SHC doesn't write; a .DATA, a .DATAB and a .RES that name no unit; a row of a section
        # the listing doesn't size; and a file that gives no section's size
        name = 'a' * 31
        for rows, problem in (
                ([f'D   00000000              _{name}:', *page(2), f'{"":59}; static: {name}',
                  directive(0, "H'00000000"), label(4, 'x'), directive(4, "H'00000000"), size(4)],
                 r'a\.lis:9: x runs past its section, D, of 4 bytes$'),
                ([label(0, 'x'), directive(0, "H'10,0", '.DATAB.B'), size(4)],
                 r"a\.lis:2: H'10,0: its count isn't a decimal number$"),
                ([label(0, 'x'), directive(0, '"abc', '.SDATA'), size(4)], r'a\.lis:2: \.SDATA "abc: no whole string$'),
                ([label(0, 'x'), directive(0, "H'00000000", '.DATA'), size(4)],
                 r'a\.lis:2: \.DATA: no \.B, \.W or \.L unit$'),
                ([label(0, 'x'), directive(0, '4,0', '.DATAB'), size(4)],
                 r'a\.lis:2: \.DATAB: no \.B, \.W or \.L unit$'),
                ([label(0, 'x'), directive(0, '4', '.RES'), size(4)], r'a\.lis:2: \.RES: no \.B, \.W or \.L unit$'),
                ([label(0, 'x', 'B'), directive(0, '4', '.RES.B'), size(4)], r'a\.lis:1: no one section starts B$'),
                ([], r"a\.lis: gives no section's size")):
            with self.subTest(problem=problem), self.assertRaisesRegex(SystemExit, problem):
                self.listed(rows)

    def test_reads_no_count_of_the_res_that_pads_a_datum(self):
        # the padding's count sizes nothing and goes unread, where the count of a .RES that starts a datum fails
        rows = [label(0, 'x'), directive(0, "H'0001", '.DATA.W'), directive(2, 'nonsense', '.RES.B'), size(4)]
        self.assertEqual(self.listed(rows)[0], [('x', 'D', 0, 2)])
        with self.assertRaisesRegex(SystemExit, r"a\.lis:2: nonsense: its count isn't a decimal number$"):
            self.listed([label(0, 'x'), directive(0, 'nonsense', '.RES.B'), size(4)])

    def test_an_assembled_listing_fails_naming_the_line_of_a_count_with_no_unit(self):
        # a .DATAB and a .RES that name no .B, .W or .L. A .DATA that names none is measured by its code, as one that
        # names a unit is
        named = (0, 'L1:                              ; static: x')
        for op in ('.DATAB      4,0', '.RES        4'):
            self.write('a.lis', assembled_listing([named, (0, op)]))
            with self.subTest(op=op), self.assertRaisesRegex(SystemExit,
                                                             rf'a\.lis:6: \{op.split()[0]}: no \.B, \.W or \.L unit$'):
                shc_listing.listing(Path('a.lis'))
        self.write('a.lis', assembled_listing([named, (0, ".DATA       H'00000000", '00000000')]))
        self.assertEqual(shc_listing.listing(Path('a.lis'))[0], [('x', 'D', 0, 4)])


class Strings(TreeTest):
    def listed(self, rows, end='\n'):
        """strings() of a listing of rows, each ended by end."""
        self.write('a.lis', ''.join(row + end for row in rows).encode('latin-1'))
        return shc_listing.strings(Path('a.lis'))

    def test_a_string_holds_any_byte_but_lf(self):
        # 0x85, which str.splitlines() ends a line at, as TEXT_133's placeholder starts with it, in lines that end
        # in CRLF as SHC's do
        rows = ['D   00000000              L1:', directive(0, '"\x85@"', '.SDATA'), directive(2, "H'00", '.DATA.B'),
                size(4)]
        self.assertEqual(self.listed(rows, '\r\n'), [('D', 0, b'\x85@\0')])
        self.assertEqual(shc_listing.listing(Path('a.lis'))[1], {'L1': ('D', 0, 3)})

    def test_a_line_marker_starts_its_section_for_the_strings_too(self):
        # -optimize=0 names P on a line marker ahead of a function. The bytes below a label in the function's code
        # are the code's and no string, and D holds a string
        rows = ['P                         ; File a.c      , Line 3        ; block',
                '    00000000              L1:                              ; function: f',
                instruction(0, 'D000', 'MOV.L       L2,R0'), '    00000004              L2:',
                directive(4, "H'02,H'04", '.DATA.B'), 'D   00000000              L3:',
                directive(0, '"ab"', '.SDATA'), directive(2, "H'00", '.DATA.B'),
                size(8, 'P', 'PROGRAM'),
                size(4)]
        self.assertEqual(self.listed(rows), [('D', 0, b'ab\0')])
        self.assertEqual(shc_listing.listing(Path('a.lis')), ([], {'L3': ('D', 0, 3)}, {'f': {'L1', 'L2'}}))

    def test_a_strings_section_is_the_one_whose_name_its_rows_abbreviate(self):
        # CSGATLINE, which SHC's rows show as CSG
        rows = ['CSG 00000000              L1:', directive(0, '"ab"', '.SDATA'), directive(2, "H'00", '.DATA.B'),
                size(4, 'CSGATLINE', 'CONST')]
        self.assertEqual(self.listed(rows), [('CSGATLINE', 0, b'ab\0')])

    def test_a_strings_bytes_go_on_in_the_rows_that_continue_them(self):
        rows = ['D   00000000              L1:', directive(0, "H'61,H'62", '.DATA.B'), more("H'63,0"), SIZE]
        self.assertEqual(self.listed(rows), [('D', 0, b'abc\0')])
        # a row that ends in the comma between its bytes and the next row's
        rows = ['D   00000000              L1:', directive(0, "H'61,H'62,", '.DATA.B'), more("H'63,0"), SIZE]
        self.assertEqual(self.listed(rows), [('D', 0, b'abc\0')])

    def test_a_label_whose_rows_give_no_characters_starts_no_string(self):
        # an automatic array's template of one longword, which is an unnamed datum and no string; then a string,
        # one of a run of bytes, and an empty one, which is its null alone
        rows = ['D   00000000              L1:', directive(0, "H'00000001"),
                'D   00000004              L2:', directive(4, '"ab"', '.SDATA'), directive(6, "H'00", '.DATA.B'),
                'D   00000008              L3:', directive(8, "2,H'2A", '.DATAB.B'), directive(10, '0', '.DATA.B'),
                'D   0000000C              L4:', directive(12, "H'00", '.DATA.B'), SIZE]
        self.write('a.lis', '\n'.join(rows) + '\n')
        self.assertEqual(shc_listing.listing(Path('a.lis'))[1],
                         {'L1': ('D', 0, 4), 'L2': ('D', 4, 3), 'L3': ('D', 8, 3), 'L4': ('D', 12, 1)})
        self.assertEqual(shc_listing.strings(Path('a.lis')), [('D', 4, b'ab\0'), ('D', 8, b'**\0'), ('D', 12, b'\0')])

    def test_a_strings_operand_ends_at_its_closing_quote(self):
        # a string that holds a ; and a doubled quote, with a comment past it; text past a string's closing quote,
        # which SHC doesn't write; and asmsh's listing, which cuts a long string short and sizes it by its code
        for operand in ('"a;""b" ; a comment', '"a;""b"; a comment', '"a;""b"  '):
            rows = ['D   00000000              L1:', directive(0, operand, '.SDATA'), SIZE]
            with self.subTest(operand=operand):
                self.assertEqual(self.listed(rows), [('D', 0, b'a;"b')])
                self.assertEqual(shc_listing.listing(Path('a.lis'))[1], {'L1': ('D', 0, 4)})
        for operand in ('"a"junk', '"a"junk ; a comment', '"a" "b"', '"a","b"'):
            rows = ['D   00000000              L1:', directive(0, operand, '.SDATA'), SIZE]
            for read in (shc_listing.listing, shc_listing.strings):
                with self.subTest(operand=operand, read=read.__name__), \
                        self.assertRaisesRegex(SystemExit, r'a\.lis:2: \.SDATA "a".*: no whole string$'):
                    self.write('a.lis', '\n'.join(rows) + '\n')
                    read(Path('a.lis'))
        self.write('a.lis', assembled_listing([(0, 'L1:                              ; static: name'),
                                               (0, '.SDATA      "abcdefghijklmnopqrstu', '6162636465666768'),
                                               (8, None, '696A6B6C6D6E6F70'), (16, None, '7172737475767778')]))
        self.assertEqual(shc_listing.listing(Path('a.lis'))[0], [('name', 'D', 0, 24)])
        self.assertEqual(shc_listing.strings(Path('a.lis')), [])

    def test_a_string_fails_naming_the_line_of_a_row_that_doesnt_give_its_bytes(self):
        # a byte SHC doesn't write, a byte past 255, a .DATAB.B of no byte, a row that starts past the string's
        # bytes, and a string past its section's 4 bytes
        for rows, problem in (
                ([directive(0, "H'4G", '.DATA.B')], r"a\.lis:2: H'4G: no byte$"),
                ([directive(0, '256', '.DATA.B')], r'a\.lis:2: 256: no byte$'),
                ([directive(0, '2', '.DATAB.B')], r'a\.lis:2: \.DATAB\.B 2: not a count and a byte$'),
                ([directive(0, '1,,2', '.DATA.B')], r'a\.lis:2: \.DATA\.B 1,,2: an empty byte$'),
                ([directive(0, f"{10 ** 20},H'00", '.DATAB.B')],
                 rf"a\.lis:2: \.DATAB\.B {10 ** 20},H'00: runs past its section$"),
                ([directive(0, '"ab"', '.SDATA'), directive(3, "H'00", '.DATA.B')],
                 r"a\.lis:3: \.DATA\.B starts at 00000003, and its string's bytes end at 00000002$"),
                ([directive(0, '"abcd"', '.SDATA'), directive(4, "H'00", '.DATA.B')],
                 r'a\.lis:1: its string runs past its section, D, of 4 bytes$')):
            with self.subTest(problem=problem), self.assertRaisesRegex(SystemExit, problem):
                self.listed(['D   00000000              L1:', *rows, size(4)])


class InitializerLoads(TreeTest):
    # grid.c's lines 3 to 5 and 6 to 8, each an automatic array whose template the next MOV.L loads
    VELOCITY = ['          grid.c     3        int velocity[3] = {', '          grid.c     4    #include "v.inc"',
                '          grid.c     5        };']
    ORIGIN = ['          grid.c     6        int origin[3] = {', '          grid.c     7    #include "o.inc"',
              '          grid.c     8        };']

    def loads(self, rows):
        self.write('a.lis', '\n'.join(rows) + '\n')
        return dict(shc_listing.initializer_loads(Path('a.lis')))

    def test_follows_each_lines_load_to_the_pool_word_it_reads(self):
        # MOV.L at 0 with displacement 18 reads the word at 76, and at 2 with displacement 1 the word at 8. A page
        # header between a line and its load leaves the line standing
        rows = ['P   00000000              _launch:                         ; function: launch', *self.VELOCITY,
                instruction(0, 'D312', 'MOV.L       L10+68,R3; L91'), *self.ORIGIN, *page(2),
                instruction(2, 'D201', 'MOV.L       L10,R2'),
                instruction(8, '<00000000>', '.DATA.L     L88'), instruction(76, '<00000000>', '.DATA.L     L91')]
        self.assertEqual(self.loads(rows), {('launch', 'grid.c', 5): {'L91'}, ('launch', 'grid.c', 8): {'L88'}})

    def test_takes_no_load_whose_comment_names_another_label_or_that_no_line_leads(self):
        # the load at 0 says L90, and its pool word holds L91. The load at 2 comes in the next function, before any
        # line of its own
        rows = ['P   00000000              _launch:                         ; function: launch', *self.VELOCITY,
                instruction(0, 'D302', 'MOV.L       L10+4,R3; L90'),
                '    00000002              _land:                           ; function: land',
                instruction(2, 'D201', 'MOV.L       L10,R2'), instruction(8, '<00000000>', '.DATA.L     L88'),
                instruction(12, '<00000000>', '.DATA.L     L91')]
        self.assertEqual(self.loads(rows), {})

    def test_tells_two_sections_of_one_abbreviation_apart(self):
        # PAB1 and PAB2, which SHC's rows both show as PAB, each with a load at 0 of its own pool word at 4
        rows = ['PAB 00000000              _launch:                         ; function: launch', *self.VELOCITY,
                instruction(0, 'D200', 'MOV.L       L10,R2'), instruction(4, '<00000000>', '.DATA.L     L91'),
                'PAB 00000000              _land:                           ; function: land', *self.ORIGIN,
                instruction(0, 'D200', 'MOV.L       L20,R2'), instruction(4, '<00000000>', '.DATA.L     L88')]
        self.assertEqual(self.loads(rows), {('launch', 'grid.c', 5): {'L91'}, ('land', 'grid.c', 8): {'L88'}})

    def test_a_long_functions_comment_on_a_line_of_its_own_names_its_loads(self):
        # the label of a function whose name has 31 characters or more, with its comment on the next line, or past
        # the header of the next page
        name = 'a' * 31
        for gap in ([], page(2)):
            rows = [f'P   00000000              _{name}:', *gap, f'{"":59}; function: {name}', *self.VELOCITY,
                    instruction(0, 'D200', 'MOV.L       L10,R2'), instruction(4, '<00000000>', '.DATA.L     L91')]
            with self.subTest(lines=len(gap)):
                self.assertEqual(self.loads(rows), {(name, 'grid.c', 5): {'L91'}})

    def test_a_line_marker_starts_the_code(self):
        # a datum in D, then the marker that names P ahead of launch's code, whose rows give no section
        rows = ['D   00000000              L1:', directive(0, "H'00000000"), marker('P'),
                '    00000000              _launch:                         ; function: launch', *self.VELOCITY,
                instruction(0, 'D200', 'MOV.L       L10,R2'), instruction(4, '<00000000>', '.DATA.L     L91')]
        self.assertEqual(self.loads(rows), {('launch', 'grid.c', 5): {'L91'}})

    def test_a_lines_file_is_its_name_by_either_separator(self):
        # grid.c, as src\\grid.c for velocity's lines and src/grid.c for origin's
        rows = ['P   00000000              _launch:                         ; function: launch',
                *(line.replace('grid.c', 'src\\grid.c') for line in self.VELOCITY),
                instruction(0, 'D201', 'MOV.L       L10,R2'),
                *(line.replace('grid.c', 'src/grid.c') for line in self.ORIGIN),
                instruction(2, 'D302', 'MOV.L       L10+4,R3'),
                instruction(8, '<00000000>', '.DATA.L     L88'), instruction(12, '<00000000>', '.DATA.L     L91')]
        self.assertEqual(self.loads(rows), {('launch', 'grid.c', 5): {'L88'}, ('launch', 'grid.c', 8): {'L91'}})
