"""Laying out C types and evaluating integer constant expressions as R11,
R13 and R26 do, the releases of SHC that compile the tree's C. The host's
build of that C gives Uint32 and Sint32 SHC's 4 bytes."""

import ctypes
import re
import tempfile
import unittest
from unittest import mock
from pathlib import Path

import csource
import layout
from layout import Array, FunctionPointer, LayoutError, Pointer, Record, Scalar, Types, number, spelled
from support import ROOT, compile_host


XPT = '''typedef unsigned char Uint8;
typedef signed char Sint8;
typedef unsigned short Uint16;
typedef short Sint16;
typedef unsigned long Uint32;
typedef long Sint32;
typedef float Float;
enum bool { false, true };
typedef enum bool Bool;
'''


def types(*texts):
    return Types([XPT, *texts])


def offsets(layout):
    return [(name, offset) for name, _, offset in layout.fields]


def member(layout, name):
    return next(field for each, field, _ in layout.fields if each == name)


class Layouts(unittest.TestCase):
    def test_scalars_take_shcs_sizes(self):
        found = types()
        for ctype, code in (('char', 'c'), ('signed char', 'b'), ('Uint8', 'B'), ('short', 'h'), ('Uint16', 'H'),
                            ('int', 'i'), ('unsigned', 'I'), ('long', 'i'), ('Uint32', 'I'), ('Float', 'f'),
                            ('Bool', 'i'), ('enum bool', 'i')):
            with self.subTest(ctype=ctype):
                self.assertEqual(found.layout(ctype), Scalar(code))

    def test_a_struct_aligns_each_member_and_pads_its_end(self):
        layout = types('struct a { Uint8 b; Sint16 h; Uint8 c; Sint32 w; Sint8 e; };').layout('struct a')
        self.assertEqual(offsets(layout), [('b', 0), ('h', 2), ('c', 4), ('w', 8), ('e', 12)])
        self.assertEqual((layout.size, layout.alignment, layout.holes), (16, 4, ((1, 1), (5, 3), (13, 3))))

    def test_one_declaration_declares_each_member(self):
        layout = types('struct a { Float x, y; Uint8 *p, m[3]; };').layout('struct a')
        self.assertEqual(offsets(layout), [('x', 0), ('y', 4), ('p', 8), ('m', 12)])
        self.assertEqual(member(layout, 'p'), Pointer('Uint8'))
        self.assertEqual(member(layout, 'm'), Array(Scalar('B'), 3))

    def test_a_union_holds_its_largest_member_and_writes_its_first(self):
        layout = types('union u { Uint8 b; Uint32 w; Sint16 h[3]; };').layout('union u')
        self.assertTrue(layout.union)
        self.assertEqual((layout.size, layout.holes), (8, ((1, 7),)))

    def test_typedefs_of_records_arrays_and_pointers(self):
        found = types('typedef struct { Float x, y; } P2, *P2P;\ntypedef Float TABLE[4][2];\n'
                      'typedef struct node { struct node *next; P2 at; } NODE;')
        self.assertEqual(found.layout('P2').size, 8)
        # an unnamed struct has no name a cast can take
        self.assertEqual(found.layout('P2P'), Pointer(None))
        self.assertEqual(found.layout('TABLE'), Array(Array(Scalar('f'), 2), 4))
        self.assertEqual(offsets(found.layout('NODE')), [('next', 0), ('at', 4)])
        self.assertEqual(member(found.layout('NODE'), 'next'), Pointer('struct node'))

    def test_a_typedef_s_attribute_after_its_declarator_gives_no_other_type(self):
        # NJS_MATRIX as sdk/include/ninja.h declares it for GCC, beside a second declarator
        found = types('typedef Float NJS_MATRIX[16] __attribute__((aligned(8), warn_if_not_aligned(8))), *ROW;')
        self.assertEqual(found.layout('NJS_MATRIX'), Array(Scalar('f'), 16))
        self.assertEqual(found.layout('ROW'), Pointer('Float'))

    def test_a_function_lists_the_types_of_its_parameters(self):
        # each as declared, then ... where the list ends in one. void alone lists none, an empty list no type, and a
        # typedef's list of names a type each, as a typedef holds no old-style definition
        found = types('typedef void Hook(Uint8 count, const char *names[], ...);\ntypedef int None(void);\n'
                      'typedef int Old();\ntypedef int Named(first, second);\n'
                      'typedef void (*Take(Float (*)(Uint16), Bool))(Sint8);\n')
        names = ('array', ('pointer', ('qualified', ('base', ('char',)), ('const',))), None)
        self.assertEqual(found.typedefs['Hook'], {('function', ('base', ('void',)), (('typedef', 'Uint8'), names, '...'))})
        for name, listed in (('None', ()), ('Old', None), ('Named', (('typedef', 'first'), ('typedef', 'second')))):
            with self.subTest(name=name):
                self.assertEqual(found.typedefs[name], {('function', ('base', ('int',)), listed)})
        self.assertEqual(found.typedefs['Take'], {('function', (
            'pointer', ('function', ('base', ('void',)), (('typedef', 'Sint8'),))), (
            ('pointer', ('function', ('typedef', 'Float'), (('typedef', 'Uint16'),))), ('typedef', 'Bool')))})
        with self.assertRaisesRegex(LayoutError, 'y: no parameter'):
            found.term('int (*)(int x y)')
        # a definition's declarator still lists names alone as an old-style list
        file = found.take(spelled('int f(first, second)'))
        self.assertEqual(found.declarator(file, 1)[1], [('function', None)])

    def test_a_typedef_s_parameter_is_a_type_whichever_header_defines_it(self):
        for texts in (('typedef int (*CB)(T);', 'typedef int T;', 'typedef int (*CB)(T);'),
                      ('typedef int T;', 'typedef int (*CB)(T);')):
            with self.subTest(texts=texts):
                found = Types(list(texts))
                self.assertEqual(found.typedefs['CB'], {('pointer', ('function', ('base', ('int',)),
                                                                     (('typedef', 'T'),)))})
                self.assertEqual(found.layout('CB'), FunctionPointer(None))

    def test_an_abstract_function_lists_its_parameters(self):
        found = types()
        self.assertEqual(found.term('int ()'), ('function', ('base', ('int',)), None))
        self.assertEqual(found.term('int (void)'), ('function', ('base', ('int',)), ()))
        for ctype in ('int ()', 'int (void)'):
            with self.subTest(ctype=ctype), self.assertRaisesRegex(LayoutError, '^a function: no layout$'):
                found.layout(ctype)
        self.assertEqual(found.layout('int (*)(void)'), FunctionPointer(None))

    def test_a_type_name_is_its_whole_argument(self):
        found = types()
        self.assertEqual(found.term('int'), ('base', ('int',)))
        with self.assertRaisesRegex(LayoutError, 'int; double: no type name'):
            found.term('int; double')

    def test_type_words_before_typedef_are_its_type_s(self):
        self.assertEqual(types('short typedef int T;').layout('T'), Scalar('h'))

    def test_a_pointer_names_its_pointee_as_a_cast_would(self):
        layout = types('struct t; struct a { const Uint16 (*rows)[4][2]; struct t **list; void (*f)(int); '
                       'const Uint8 *bytes; };').layout('struct a')
        self.assertEqual([field for _, field, _ in layout.fields],
                         [Pointer('const Uint16[4][2]'), Pointer('struct t *'), Pointer(None), Pointer('const Uint8')])

    def test_a_record_inside_a_record(self):
        layout = types('struct a { Uint8 b; struct { Sint16 h; union { Float f; Uint8 c; } u; } in; };').layout(
            'struct a')
        self.assertEqual(offsets(layout), [('b', 0), ('in', 4)])
        self.assertIsInstance(member(layout, 'in'), Record)
        self.assertEqual(layout.size, 12)

    def test_two_types_share_no_definition(self):
        self.assertEqual(types('struct a { Sint32 w; Uint8 b; };').layout('struct a').size, 8)
        self.assertEqual(types('struct a { Uint8 b; };').layout('struct a').size, 1)

    def test_comments_strings_and_directives_hide_no_declaration(self):
        found = types('/* struct a { int x; }; */\nchar *s = "struct a { char x; }";\n#if 0\n#endif\n'
                      '// struct a { short x; }; \\\nstruct a { long x; };\nstruct a { Float f; };')
        self.assertEqual(member(found.layout('struct a'), 'f'), Scalar('f'))

    def test_a_comment_inside_a_macro_joins_its_lines(self):
        # a comment's line break ends no directive, spliced or not
        for macro in ('#define N \\\n    /* a comment of \\\n     * two lines */ \\\n    (1 + 2)\n',
                      '#define N (1 + /* a comment of\n     * two lines */ 2)\n'):
            with self.subTest(macro=macro):
                self.assertEqual(types(macro + 'struct a { char x[N]; };').layout('struct a').size, 3)

    def test_what_has_no_layout_fails_naming_why(self):
        found = types('struct bits { Uint32 flag : 1; };\nstruct d { double x; };\nstruct a { int x[]; };\n'
                      'typedef void FUNCTION(void);\n#define N (1 << 31)\nstruct s { char a[N]; };\n'
                      'struct n { char a[-1]; };\ntypedef LOOP2 LOOP1;\ntypedef LOOP1 LOOP2;\n'
                      'struct self { struct self x; };\ntypedef ROWS TABLE[2];\ntypedef TABLE ROWS;')
        for ctype, reason in (('struct bits', 'flag is a bit-field'), ('struct d', 'double: no layout'),
                              ('struct a', r'int\[\]: no extent'), ('FUNCTION', 'a function'),
                              ('struct s', r'\[N\]: 1 << 31: 2147483648 lies outside int'),
                              ('struct n', r'\[- 1\]: a negative extent'),
                              ('struct none', 'struct none: no definition'), ('NOTHING', 'NOTHING: no type'),
                              ('LOOP1', 'LOOP1: leads back to itself'), ('struct self', 'struct self: holds itself'),
                              ('TABLE', 'ROWS: holds itself'), ('void', 'void: no layout'),
                              ('struct c { int x; }', 'a type name with a body')):
            with self.subTest(ctype=ctype), self.assertRaisesRegex(LayoutError, reason):
                found.layout(ctype)

    def test_a_tag_defined_two_ways_fails(self):
        found = types('struct a { Sint32 w; };', 'struct a { Uint8 b; };')
        with self.assertRaisesRegex(LayoutError, 'struct a: defined two ways'):
            found.layout('struct a')

    def test_a_member_two_branches_declare_fails(self):
        found = types('struct g2 {\n#ifdef NONMATCHING\n    volatile Uint32 device;\n#else\n    Uint32 device;\n'
                      '#endif\n};')
        with self.assertRaisesRegex(LayoutError, '^struct g2: two members named device$'):
            found.layout('struct g2')

    def test_a_typedef_defined_two_ways_fails(self):
        with self.assertRaisesRegex(LayoutError, '^T: defined two ways$'):
            types('typedef short T;', 'typedef long T;').layout('T')

    def test_a_layout_taken_before_another_file_is_laid_out_again(self):
        # T and a record that holds it, each laid out before the file that defines T a second way
        for asked in ((), ('T',), ('struct s',), ('T', 'struct s')):
            with self.subTest(asked=asked):
                found = types('typedef char T;\nstruct s { T t; };')
                for ctype in asked:
                    found.layout(ctype)
                found.take(spelled('typedef int T;'))
                for ctype in ('T', 'struct s'):
                    with self.assertRaisesRegex(LayoutError, 'T: defined two ways'):
                        found.layout(ctype)

    def test_type_words_that_name_no_type_fail(self):
        found = types('#define MEMBER int x;\nstruct s { MEMBER char y; };\nstruct t { struct s int x; };')
        for ctype, reason in (('struct s', 'char after MEMBER'), ('struct t', 'int after struct s'),
                              ('unsigned float', 'unsigned float: no C type'),
                              ('signed unsigned', 'signed unsigned: no C type'), ('short char', 'short char: no C type'),
                              ('long float', 'long float: no C type'), ('int int', 'int int: no C type')):
            with self.subTest(ctype=ctype), self.assertRaisesRegex(LayoutError, re.escape(reason)):
                found.layout(ctype)
        for ctype, code in (('unsigned short int', 'H'), ('signed char', 'b'), ('long unsigned int', 'I'),
                            ('short signed', 'h'), ('unsigned', 'I')):
            with self.subTest(ctype=ctype):
                self.assertEqual(found.layout(ctype), Scalar(code))

    def test_an_enum_tag_defined_two_ways_lays_out_as_an_int(self):
        self.assertEqual(types('enum e { A };', 'enum e { B };').layout('enum e'), Scalar('i'))

    def test_a_64_bit_hosts_group_goes_unread(self):
        found = Types(['#ifdef __LP64__\ntypedef unsigned int Uint32;\n#else\n'
                       'typedef unsigned long Uint32;\n#endif\nstruct w { Uint32 word; };'])
        self.assertEqual(found.typedefs, {'Uint32': {('base', ('unsigned', 'long'))}})
        self.assertEqual(offsets(found.layout('struct w')), [('word', 0)])


class Constants(unittest.TestCase):
    def test_extents_take_macros_and_enumerators(self):
        found = types('#define ROWS 3\n#define COLS (ROWS * 2 + 1)\nenum { RED, GREEN = 4, BLUE, COUNT };\n'
                      'struct a { Uint8 grid[ROWS][COLS]; Sint16 colors[COUNT]; };')
        self.assertEqual(member(found.layout('struct a'), 'grid'), Array(Array(Scalar('B'), 7), 3))
        self.assertEqual(found.layout('struct a').size, 21 + 1 + 12)
        self.assertEqual(found.evaluate(['BLUE']), 5)

    def test_a_macro_or_an_enumerator_defined_two_ways_fails(self):
        found = types('#define N 1\nenum { E = 1 };', '#define N 2\nenum { E = 2 };')
        for name in ('N', 'E'):
            with self.subTest(name=name), self.assertRaisesRegex(LayoutError, f'^{name}: defined two ways$'):
                found.evaluate([name])

    def test_a_macro_that_is_also_an_enumerator_or_a_function_like_macro_fails(self):
        found = types('enum { A = 1, B = A };\n#define A 4\n#if X\n#define N 4\n#else\n#define N(x) (x)\n#endif\n')
        for name, reason in (('B', 'A: defined two ways'), ('N', 'N: defined two ways')):
            with self.subTest(name=name), self.assertRaisesRegex(LayoutError, f'^{reason}$'):
                found.evaluate([name])

    def test_an_enumerator_after_an_item_types_can_t_read_fails(self):
        found = types('#define ITEM(n) n = 7\nenum { ITEM(A), B };\nenum { C = 7, D };')
        with self.assertRaisesRegex(LayoutError, re.escape("B: counts on from ITEM ( A ), which Types can't read")):
            found.evaluate(['B'])
        self.assertEqual(found.evaluate(['D']), 8)

    def test_an_enumerator_given_one_value_in_two_enums_is_defined_once(self):
        self.assertEqual(types('enum first { P, N = 4 };', 'enum other { N = 4 };').evaluate(['N']), 4)

    def test_a_macro_expands_as_text(self):
        # as the preprocessor does, without parentheses of its own
        self.assertEqual(types('#define SUM 1 + 2').evaluate(['SUM', '*', '3']), 7)

    def test_c_arithmetic(self):
        # each as R11, R13 and R26 fold it
        found = types()
        for text, value in (('-7 / 2', -3), ('-7 % 2', -1), ('7 % -2', 1), ('1 << 4 | 1', 17), ('~0 & 0xff', 255),
                            ('(2 + 3) * 4', 20), ('6 - 2 - 1', 3), ('1 < 2 && 2 < 3', 1), ('-16 >> 2', -4),
                            ('-3 >> 1', -2), ('-3 << 2', -12)):
            with self.subTest(text=text):
                self.assertEqual(found.evaluate(spelled(text)), value)

    def test_unsigned_and_long_values_that_fit_their_types(self):
        # each as R11, R13 and R26 fold it. A long and an unsigned int convert to long, in arithmetic as in a
        # comparison
        found = types()
        for text, value in (('12U + 40UL', 52), ('0x80000000U >> 28', 8), ('0xFFFFFFFF >> 28', 15),
                            ('3000000000 / 1000000000', 3), ('1 < 2U', 1), ('-1 && 1U', 1), ('5U - 3', 2),
                            ('-1L < 1U', 1), ('1 < 0xffffffffUL', 1), ('0x80000000L >> 31', 1), ('-1 >> 1U', -1),
                            ('(1 < 2U) - 2', -1), ('!0U - 2', -1), ('-1L / 2U', 0), ('(-1L + 0U) >> 31', -1)):
            with self.subTest(text=text):
                self.assertEqual(found.evaluate(spelled(text)), value)

    def test_unsigned_values_wrap_as_c_s_do(self):
        # each as R11, R13 and R26 fold it. An unsigned value wraps modulo
        # 2**32, and so does an operand that converts to unsigned
        found = types()
        for text, value in (('~0U >> 31', 1), ('(~0U >> 31) + 1', 2), ('0xffffffffU + 1U', 0),
                            ('0xffffffffU + 2U', 1), ('-1 < 1U', 0), ('(1 - 2U) >> 28', 15),
                            ('-2147483648', 0x80000000), ('-2147483648 >> 30', 2)):
            with self.subTest(text=text):
                self.assertEqual(found.evaluate(spelled(text)), value)

    def test_unsigned_values_wrap_without_a_host_sized_length(self):
        def counted(sized):
            # as a 32-bit Python's len() can't count 2**32 values
            if isinstance(sized, range) and sized.stop > 0x7fffffff:
                raise OverflowError
            return len(sized)
        with mock.patch.object(layout, 'len', counted, create=True):
            self.assertEqual(types().evaluate(spelled('0xffffffffU + 1U')), 0)

    def test_integer_constants_take_shcs_types(self):
        self.assertEqual([number(text) for text in ('0', '017', '0x1F', '12U', '40UL', '10L', '0x80000000',
                                                    '3000000000', '0x80000000L')],
                         [(0, 'int'), (15, 'int'), (31, 'int'), (12, 'unsigned int'), (40, 'unsigned long'),
                          (10, 'long'), (0x80000000, 'unsigned int'), (3000000000, 'unsigned int'),
                          (0x80000000, 'unsigned long')])
        for text, reason in (('1.5F', '1.5F: no integer constant'), ('10LL', '10LL: no integer constant'),
                             ('0x100000000', '0x100000000: wider than 32 bits'), ('4294967296', 'wider than 32 bits'),
                             ('1' * 5000, 'wider than 32 bits')):
            with self.subTest(text=text), self.assertRaisesRegex(LayoutError, reason):
                number(text)

    def test_what_is_no_constant_fails(self):
        found = types('#define HALF(x) ((x) / 2)\n#define LOOP LOOP\n')
        for tokens, reason in ((['HALF', '(', '4', ')'], 'HALF: a function-like macro'), (['LOOP'], 'LOOP: no'),
                               (['1', '/', '0'], '1 / 0: a division by zero'), (['sizeof', 'x'], 'sizeof')):
            with self.subTest(tokens=tokens), self.assertRaisesRegex(LayoutError, reason):
                found.evaluate(tokens)

    def test_a_value_shc_gives_otherwise_fails_naming_its_operation(self):
        # R11, R13 and R26 give each one a value unbounded integers don't, as
        # they wrap, convert or shift. 3000000000 is an unsigned int, which
        # converts to long, so 1L < 3000000000 is 0. 0x100000000 is 0, its
        # low 32 bits
        found = types('enum { TOP = 0x7fffffff, OVER };')
        for text, reason in (('1L < 0xffffffff', '1L < 0xffffffff: 4294967295 lies outside long'),
                             ('1L < 3000000000', '1L < 3000000000: 3000000000 lies outside long'),
                             ('0x100000000', '0x100000000: wider than 32 bits'),
                             ('0x7fffffff + 1', '0x7fffffff + 1: 2147483648 lies outside int'),
                             ('1 << 31', '1 << 31: 2147483648 lies outside int'),
                             ('OVER', 'OVER: 2147483648 lies outside int'),
                             ('1 << 32', '1 << 32: a shift by 32, outside 0 to 31'),
                             ('1 << -1', '1 << - 1: a shift by -1, outside 0 to 31')):
            with self.subTest(text=text), self.assertRaisesRegex(LayoutError, re.escape(reason)):
                found.evaluate(spelled(text))

    def test_what_shc_gives_no_value_fails(self):
        # R11, R13 and R26 refuse each, and refuse a division by zero even
        # on the side of && or || that C skips
        found = types()
        for text, reason in (('0 && (1 / 0)', '1 / 0: a division by zero'),
                             ('1 || (1 % 0)', '1 % 0: a division by zero'),
                             ('(-2147483647 - 1) / -1', '( - 2147483647 - 1 ) / - 1: 2147483648 lies outside int'),
                             ('(-2147483647 - 1) % -1',
                              '( - 2147483647 - 1 ) % - 1: its quotient 2147483648 lies outside int'),
                             ('10LL', '10LL: no integer constant')):
            with self.subTest(text=text), self.assertRaisesRegex(LayoutError, re.escape(reason)):
                found.evaluate(spelled(text))

    def test_a_conditional_takes_one_arm_in_the_arms_type(self):
        # text_sprite.c's KANJI_PLACE gives kanji_widths its extent through a ?:. Both arms convert to the later of
        # their types, so R13 and R26 give the arms of -1L and 0U a long's type, and the comparison 1
        found = types()
        for text, value in (('1 ? 2 : 3', 2), ('0 ? 2 : 3', 3), ('0 ? 1 : 0 ? 2 : 3', 3), ('(2 > 1 ? 10 : 20) * 2', 20),
                            ('1 ? -1 : 0U', 0xffffffff), ('(1 ? -1L : 0U) < 0', 1), ('(1 ? -1 : 0U) < 0', 0),
                            ('0x72 - ((0x72) > 0x7e ? 1 : 0)', 0x72)):
            with self.subTest(text=text):
                self.assertEqual(found.evaluate(spelled(text)), value)
        for text, reason in (('1 ? 2 : 1 / 0', '1 / 0: a division by zero'), ('1 ? 2', 'no integer constant')):
            with self.subTest(text=text), self.assertRaisesRegex(LayoutError, re.escape(reason)):
                found.evaluate(spelled(text))

    def test_both_sides_of_and_are_evaluated(self):
        # R11, R13 and R26 give this 0, skipping its shift. Types evaluates
        # the shift, and fails
        with self.assertRaisesRegex(LayoutError, re.escape('1 << 32: a shift by 32, outside 0 to 31')):
            types().evaluate(spelled('0 && (1 << 32)'))

    def test_an_enum_of_thousands_counts_on(self):
        # past Python's recursion limit
        found = types('enum many { ' + ', '.join(f'E{index}' for index in range(5000)) + ' };')
        self.assertEqual(found.evaluate(['E4999']), 4999)


# a laid-out type's failure that Types gives for what has no layout, not for a declaration it can't read: a bit-field,
# as the modem library's headers hold, a struct or union no header defines, and a function's type
REFUSED = re.compile(r'is a bit-field$|: no definition$|^a function: no layout$')


class TheTree(unittest.TestCase):
    def test_lays_out_every_type_of_the_headers_as_shc_compiles_them(self):
        # every typedef, struct and union of the headers' matching sides, each laid out or refused as having none
        paths = sorted(path for root in ('src', 'sdk', 'modules') for path in (ROOT / root).rglob('*.h'))
        found = Types([csource.code(path, csource.SHC) for path in paths])
        self.assertTrue(found.typedefs)
        problems = []
        for term in [*(('typedef', name) for name in found.typedefs), *found.definitions]:
            if term[0] == 'enum':
                continue
            try:
                found.layout(term)
            except LayoutError as error:
                if not REFUSED.search(str(error)):
                    problems.append(f'{term[1]}: {error}')
        self.assertEqual(problems, [])


class TheHost(unittest.TestCase):
    def test_uint32_and_sint32_take_shcs_4_bytes(self):
        with tempfile.TemporaryDirectory() as directory:
            source, library = Path(directory, 'widths.c'), Path(directory, 'widths.so')
            source.write_text('#include <sg_xpt.h>\nconst unsigned widths[] = {sizeof(Uint32), sizeof(Sint32)};\n')
            compile_host(library, source, shared=True)
            widths = (ctypes.c_uint * 2).in_dll(ctypes.CDLL(str(library)), 'widths')
            self.assertEqual(list(widths), [4, 4])
