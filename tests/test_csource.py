"""The C reader: tokens, directives, blocks, guards' sides, includes,
expressions and declarations, and every C file of the tree read on each
side of its guards."""

import os
import re
import unittest
import unittest.mock
from pathlib import Path

import body_data
import csource
import decls
import inline
import split
from config import content
from support import ROOT, TreeTest

# C with a /* */ comment over two lines, a // comment a splice continues, a string that holds /* */, and a guard
TEXT = '''int a; /* one
two */ int b; // three \\
four
char *c = "/* five */";
#ifdef NONMATCHING
int gcc;
#else
int shc;
#endif
'''
# an inline body as the tree writes one: under DREAMCAST, its non-matching
# side first, and the declaration a build off the console takes after its
# #else
BODY = '''#ifdef DREAMCAST
#ifdef NONMATCHING
void push(void)
{
    zam_push();
}
#else
#pragma inline_asm(push)
void push(void)
{
        RTS
        NOP
}
#endif
#else
void push(void);
#endif
'''


class Code(TreeTest):
    def test_blanks_each_comment_but_for_its_line_ends_and_splices(self):
        self.write('a.c', TEXT)
        self.assertEqual(csource.code('a.c').splitlines()[:4],
                         ['int a;       ', '       int b;          \\', '    ', 'char *c = "/* five */";'])

    def test_finds_a_comment_whose_opener_a_splice_splits(self):
        # C joins spliced lines before it finds comments, so a / and a * that a splice parts open one, but not in a
        # string
        self.write('a.c', 'int a; /\\\n* gone */ int b; "/\\\n*";\n')
        self.assertEqual(csource.code('a.c'), 'int a;  \\\n          int b; "/\\\n*";\n')

    def test_empties_the_side_of_each_guard_the_compiler_skips(self):
        self.write('a.c', TEXT)
        self.assertEqual([csource.code('a.c', compiler).splitlines()[4:] for compiler in (None, 'SHC', 'GCC')],
                         [['#ifdef NONMATCHING', 'int gcc;', '#else', 'int shc;', '#endif'],
                          ['#ifdef NONMATCHING', '', '#else', 'int shc;', '#endif'],
                          ['#ifdef NONMATCHING', 'int gcc;', '#else', '', '#endif']])

    def test_empties_a_64_bit_hosts_group_for_either_compiler(self):
        # SHC and make dc's GCC both build for the SH-4, whose long has 32 bits, so each takes the #else
        self.write('a.c', '#ifdef __LP64__\ntypedef int word;\n#else\ntypedef long word;\n#endif\n')
        self.assertEqual([csource.code('a.c', compiler).splitlines() for compiler in (None, 'SHC', 'GCC')],
                         [['#ifdef __LP64__', 'typedef int word;', '#else', 'typedef long word;', '#endif'],
                          *[['#ifdef __LP64__', '', '#else', 'typedef long word;', '#endif']] * 2])

    def test_parts_no_guard_of_dreamcast(self):
        # both compilers define DREAMCAST, so neither compiles the #else of its guard, and code() keeps it for each
        self.write('a.c', '#ifdef DREAMCAST\nint console;\n#else\nint host;\n#endif\n')
        self.assertEqual([csource.code('a.c', compiler).splitlines() for compiler in ('SHC', 'GCC')],
                         [['#ifdef DREAMCAST', 'int console;', '#else', 'int host;', '#endif']] * 2)

    def test_keeps_an_inline_body_for_shc_and_its_non_matching_side_for_gcc(self):
        # code() parts no guard of DREAMCAST, which both compilers define, so the declaration after its #else stays
        # on each side
        self.write('a.c', BODY)
        shc, gcc = (csource.blanked(csource.code('a.c', compiler)).split('\n') for compiler in ('SHC', 'GCC'))
        self.assertEqual((csource.bodies('a.c', shc), list(csource.spans(shc))),
                         ([(7, 'push', False, 9, 12)], [('push', 9, 12)]))
        self.assertEqual((csource.bodies('a.c', gcc), list(csource.spans(gcc))), ([], [('push', 3, 5)]))
        self.assertEqual([shc[15], gcc[15]], ['void push(void);'] * 2)


class Made(TreeTest):
    def test_reads_a_text_code_made_as_it_reads_the_text_alone(self):
        # each made from TEXT's C, with its comments, a guard's side or its literals blank, and each line past an
        # emptied one moved up
        self.write('a.c', TEXT)
        made = [csource.code('a.c', compiler) for compiler in (None, 'SHC', 'GCC')]
        made.append(csource.blanked(made[2]))
        for text in made:
            found, alone = csource.parse(text), csource.C(text)
            self.assertIsNot(found, csource.read(text))
            self.assertEqual((found.lexemes, found.tokens, found.directives, found.blocks),
                             (alone.lexemes, alone.tokens, alone.directives, alone.blocks))
            self.assertEqual(csource.tokens(text), [(token.text, token.line) for token in alone.tokens
                                                    if token.kind != 'comment'])

    def test_reads_each_of_two_views_of_one_spelling_by_its_own_source(self):
        # the first's comment runs its #define on past the line end, and the second's ends at it, so only the second
        # declares x; a plain string with their spelling reads as written, whichever was blanked last
        sources = ('#define N /*\n*/ int x;\n', '#define N //\n   int x;\n')
        made = [csource.blank(text, ('comment',)) for text in sources]
        self.assertEqual(*made)
        for views in (made, made[::-1]):
            self.assertEqual({view.made[0]: [token.text for token in csource.parse(view).parsed] for view in views},
                             {sources[0]: [], sources[1]: ['int', 'x', ';']})
            self.assertEqual([token.text for token in csource.parse(str(made[0])).parsed], ['int', 'x', ';'])

    def test_reads_a_source_s_tokens_once_for_each_view_of_it(self):
        text, scans = 'int a; /* b */ char *c = "d";\n', []
        between = csource.C.between
        with unittest.mock.patch.object(csource.C, 'between', lambda c, start, end: scans.append(start) or
                                        between(c, start, end)):
            views = [csource.parse(text), csource.parse(csource.blank(text, ('comment',))),
                     csource.parse(csource.blanked(text))]
            self.assertEqual([[token.text for token in view.tokens] for view in views],
                             [['int', 'a', ';', '/* b */', 'char', '*', 'c', '=', '"d"', ';'],
                              ['int', 'a', ';', 'char', '*', 'c', '=', '"d"', ';'],
                              ['int', 'a', ';', 'char', '*', 'c', '=', ';']])
        self.assertEqual(scans, [0])


class Uncommented(unittest.TestCase):
    def test_makes_each_comment_one_blank_and_keeps_its_line_ends_where_asked(self):
        text = '#define A 1 /* one\ntwo */ + 2\nchar *b = "/* three */"; // four\n'
        self.assertEqual((csource.uncommented(text), csource.uncommented(text, lines=False)),
                         ('#define A 1  \n + 2\nchar *b = "/* three */";  \n',
                          '#define A 1   + 2\nchar *b = "/* three */";  \n'))


class Compiled(unittest.TestCase):
    def test_reads_the_groups_of_the_guards_tests_and_no_other(self):
        # make dc's compile defines NONMATCHING and DREAMCAST, so it takes an #ifdef of either and skips its #else. It
        # may take or skip any other group, an #if's or another macro's, but a group it skips settles each inside it
        text = ('#ifdef NONMATCHING\na\n#else\nb\n#endif\n#ifdef DREAMCAST\nc\n#else\nd\n#endif\n'
                '#if 1\ne\n#else\nf\n#endif\n#ifdef X\n#ifdef NONMATCHING\ng\n#else\nh\n#endif\n#endif\ni\n'
                '#ifndef A_H\nj\n#endif')
        lines = text.split('\n')
        found = dict(zip(lines, csource.compiled(lines)))
        self.assertEqual({line: found[line] for line in 'abcdefghij'},
                         {'a': True, 'b': False, 'c': True, 'd': False, 'e': None, 'f': None, 'g': None, 'h': False,
                          'i': True, 'j': None})

    def test_skips_each_later_group_of_a_guard_it_takes(self):
        # the compile takes the #ifdef's group, so it takes no #elif's and no #else's, whatever FEATURE is
        lines = '#ifdef NONMATCHING\na\n#elif FEATURE\nb\n#else\nc\n#endif'.split('\n')
        self.assertEqual((csource.compiled(lines)[1::2], sorted(csource.matching_side(lines))),
                         ([True, False, False], [3, 5]))


class Tokens(unittest.TestCase):
    def test_gives_each_token_its_line(self):
        self.assertEqual(csource.tokens('int a =\n    0x10;\n'),
                         [('int', 1), ('a', 1), ('=', 1), ('0x10', 2), (';', 2)])

    def test_ends_a_character_constant_at_its_line(self):
        # SHC's assembly spells hexadecimal H'7B, whose ' opens no constant its line closes
        self.assertEqual(csource.tokens("MOV #H'7B,R0\nRTS\n"),
                         [('MOV', 1), ('#', 1), ('H', 1), ("'", 1), ('7B', 1), (',', 1), ('R0', 1), ('RTS', 2)])

    def test_leaves_out_each_directive_where_asked(self):
        # B's # opens no directive, since a splice joins its line to the one above
        text = '#define A \\\n    1\nint a; \\\n# define B\n#if A\nint b;\n#endif\n'
        self.assertEqual(csource.tokens(text, directives=False),
                         [('int', 3), ('a', 3), (';', 3), ('#', 4), ('define', 4), ('B', 4), ('int', 6), ('b', 6),
                          (';', 6)])
        # a comment that holds a line end runs Y on to line 2, and int is on line 3
        self.assertEqual(csource.tokens('#define Y 1 /* one\ntwo */ 2\nint a;\n', directives=False),
                         [('int', 3), ('a', 3), (';', 3)])

    def test_reads_a_token_a_splice_splits_as_one_on_its_first_line(self):
        self.assertEqual(csource.tokens('int imp\\\nort = 0x0cc0\\\n0080;\nint b;\n'),
                         [('int', 1), ('import', 1), ('=', 2), ('0x0cc00080', 2), (';', 3), ('int', 4), ('b', 4),
                          (';', 4)])


class MacroDirectives(unittest.TestCase):
    def test_gives_each_definition_and_undef_in_order_with_its_line(self):
        # F's ellipsis takes the name __VA_ARGS__, as its body spells it. NOT's # is on a line a splice joins to
        # the one above, so it opens no directive
        text = ('#define A 0x10\n#  define F(x, ...) x, __VA_ARGS__\n#define G() 2\nint a; \\\n#define NOT 1\n'
                '#undef A\n#define B \\\n    (A + 1)\n')
        self.assertEqual(csource.macro_directives(text), [(1, 'A', (None, ['0x10'])),
                                                          (2, 'F', (('x', '__VA_ARGS__'), ['x', ',', '__VA_ARGS__'])),
                                                          (3, 'G', ((), ['2'])), (6, 'A', None),
                                                          (7, 'B', (None, ['(', 'A', '+', '1', ')']))])

    def test_runs_a_directive_on_past_a_comment_that_holds_a_line_end(self):
        # a line end inside a comment doesn't end a directive, as GCC reads it
        self.assertEqual(csource.macro_directives('#define Y 1 /* one\ntwo */ 2\nint a;\n'),
                         [(1, 'Y', (None, ['1', '2']))])

    def test_reads_a_macro_whose_name_a_splice_parts_from_its_parameters_as_function_like(self):
        for text in ('#define F\\\n(x) x\n', '#define F\\\r\n(x) x\n'):
            self.assertEqual(csource.macro_directives(text), [(1, 'F', (('x',), ['x']))])
        self.assertEqual(csource.macro_directives('#define F (x) x\n'), [(1, 'F', (None, ['(', 'x', ')', 'x']))])

    def test_takes_a_named_ellipsis_by_its_name(self):
        # in GNU C, args ... names the variable arguments args, as the body spells them
        self.assertEqual(csource.macro_directives('#define LOG(fmt, args ...) f(fmt, args)\n'),
                         [(1, 'LOG', (('fmt', 'args'), ['f', '(', 'fmt', ',', 'args', ')']))])


class Spans(unittest.TestCase):
    def test_reads_each_group_s_head_from_the_one_before_the_conditional(self):
        # the #else's f opens with its own head, not the #ifdef's int value =; and both groups take static
        text = '#ifdef NONMATCHING\nint value =\n#else\nint f(void) { return 0; }\nint value =\n#endif\n1;\n'
        self.assertEqual(list(csource.spans(text.split('\n'))), [('f', 3, 3)])
        text = 'static\n#ifdef NONMATCHING\nint f(void) {}\n#else\nint g(void) {}\n#endif\n'
        self.assertEqual([(block.name, [token.text for token in block.head]) for block in csource.parse(text).blocks],
                         [(name, ['static', 'int', name, '(', 'void', ')']) for name in 'fg'])

    def test_names_a_function_past_its_attributes_arguments(self):
        c = csource.parse('__attribute__((section(".text"))) int f(void) {}\nint g(void) {}\n')
        self.assertEqual([(block.name, block.head[0].text) for block in c.blocks],
                         [('f', '__attribute__'), ('g', 'int')])

    def test_closes_a_function_at_the_brace_that_matches_its_opening(self):
        text = 'int f(void)\n{\n    if (a) {\n    }\n} /* NOLINT */\nint g(void)\n{\n}\n'
        self.assertEqual(list(csource.spans(csource.blanked(text).split('\n'))), [('f', 1, 4), ('g', 6, 7)])

    def test_reads_an_old_form_head_through_its_parameters_declarations(self):
        text = 'void f(a, b)\n    int a;\n    char *b;\n{\n}\nint g[] = {1};\n'
        self.assertEqual(list(csource.spans(text.split('\n'))), [('f', 3, 4)])

    def test_takes_the_braces_of_the_first_group_to_change_them(self):
        # with the non-matching side empty, as code() gives it for SHC, only the #else group opens the if's brace
        text = 'int f(void)\n{\n#ifdef NONMATCHING\n\n#else\n    if (a) {\n#endif\n        b();\n    }\n}\n'
        self.assertEqual(list(csource.spans(text.split('\n'))), [('f', 1, 9)])

    def test_closes_an_inline_body_at_its_first_closing_brace(self):
        # an inline body is assembly, so its { opens nothing
        text = '#pragma inline_asm(f)\nstatic int f(void)\n{\n        RTS ; {\n        NOP\n}\nint g(void)\n{\n}\n'
        lines = csource.blanked(text).split('\n')
        self.assertEqual((list(csource.spans(lines)), csource.bodies('a.c', lines)),
                         ([('f', 2, 5), ('g', 7, 8)], [(0, 'f', True, 2, 5)]))

    def test_opens_no_body_with_another_function_s_braces(self):
        # a #pragma inline_asm takes the first block below it only where that block is its own function's
        text = ('#pragma inline_asm(ghost)\nvoid make(void)\n{\n    run();\n}\n'
                '#pragma inline_asm(table)\nstatic int table[] = {1};\n')
        problems = []
        self.assertEqual(csource.bodies('a.c', csource.blanked(text).split('\n'), problems), [])
        self.assertEqual(problems, ['a.c:1: #pragma inline_asm(ghost), with another function, make, below it',
                                    'a.c:6: #pragma inline_asm(table), with no function after it'])

    def test_names_a_body_only_in_inline_py_s_form(self):
        # a #pragma inline_asm opens a body where tools/inline.py reads one, and nowhere else
        for text, names in (('#pragma inline_asm(f)\n', ['f']), ('#pragma inline_asm(f(size=4))\n', ['f']),
                            ('#  pragma   inline_asm(f)\n', []), ('#pragma inline_asm(f) /* x */\n', [])):
            with self.subTest(text=text):
                sites = csource.inline_sites(csource.blanked(text).split('\n'))
                self.assertEqual((list(sites.values()), inline.bodies(text)), (names, names))

    def test_reads_a_helper_a_pragma_inline_names_as_c(self):
        # a header's helper, as sdk/include/float_bits.h defines one: SHC expands it at each call, and its body is C
        text = '#pragma inline(whole)\nstatic int whole(float x)\n{\n    if (x) {\n    }\n    return 1;\n}\n'
        lines = csource.blanked(text).split('\n')
        self.assertEqual((list(csource.spans(lines)), csource.bodies('a.h', lines)), ([('whole', 2, 6)], []))


class Included(TreeTest):
    def included(self, path):
        texts = {}
        csource.included(Path(path), texts)
        return list(texts)

    def test_an_include_inside_a_comment_includes_nothing(self):
        for path, text in {'a.c': '/*\n#include "b.h"\n*/\n#include "c.h"\n', 'b.h': '', 'c.h': ''}.items():
            self.write(path, text)
        self.assertEqual(self.included('a.c'), [Path('a.c'), Path('c.h')])

    def test_spliced_header_names_share_the_original_directive_and_token_spans(self):
        text = '// hidden \\\r\n#include "absent.h"\n#inc\\\nlude /*x*/ "loc\\\r\nal.h"\r\n#include <nes\\\nted.h>\n'
        self.write('src/local.h', '')
        self.write('sdk/include/nested.h', '')
        rows = list(csource.header_names(text))
        self.assertEqual([(d.line, quoted, name) for d, quoted, name in rows],
                         [(3, True, 'local.h'), (6, False, 'nested.h')])
        self.assertEqual(text[rows[0][0].words[0].start:rows[0][0].words[0].end], '"loc\\\r\nal.h"')
        self.assertEqual(list(csource.includes(Path('src/a.c'), text)),
                         [Path('src/local.h'), Path('sdk/include/nested.h')])

    def test_reads_an_angle_header_name_as_written_and_a_comment_after_it(self):
        self.assertEqual([(quoted, name) for _, quoted, name in csource.header_names('#include <dir//a.h> // b\n')],
                         [(False, 'dir//a.h')])
        self.assertEqual([token.text for token in csource.parse('#include <dir//a.h> // b\n').lexemes], ['#', '// b'])

    def test_an_angle_include_searches_the_roots_alone_and_a_quoted_one_its_directory_first(self):
        for path in ('src/x.h', 'src/y.h', 'sdk/include/x.h', 'sdk/include/y.h'):
            self.write(path, '')
        self.write('src/a.c', '#include <x.h>\n#include "y.h"\n')
        self.assertEqual(self.included('src/a.c'), [Path('src/a.c'), Path('sdk/include/x.h'), Path('src/y.h')])

    def test_a_file_is_read_once_however_its_path_is_spelled(self):
        # a header that includes itself through .., and another through a
        # link to their directory
        self.write('include/a.h', '#ifndef A\n#define A\n#include "../include/a.h"\n#include "link/b.h"\n#endif\n')
        self.write('include/b.h', '#include "link/b.h"\n')
        os.symlink('.', 'include/link')
        self.assertEqual(self.included('include/a.h'), [Path('include/a.h'), Path('include/b.h')])

    def test_a_variant_includes_its_text_and_the_text_s_headers(self):
        # as modules/shared/pickup_object_2.c and the near clips of sdk/ninja/ include theirs (TOOLCHAIN.md#variants)
        files = {'modules/shared/box_e.c': '#define BOX_SUFFIX "e"\n\n#include "shared/box.c"\n',
                 'modules/shared/box.c': '#include "shared/box.h"\n', 'modules/shared/box.h': '',
                 'sdk/ninja/clipD.c': '#include "../ninja/clip.c"\n', 'sdk/ninja/clip.c': '#include <ninja.h>\n',
                 'sdk/include/ninja.h': ''}
        for path, text in files.items():
            self.write(path, text)
        self.assertEqual([self.included('modules/shared/box_e.c'), self.included('sdk/ninja/clipD.c')],
                         [[Path('modules/shared/box_e.c'), Path('modules/shared/box.c'), Path('modules/shared/box.h')],
                          [Path('sdk/ninja/clipD.c'), Path('sdk/ninja/clip.c'), Path('sdk/include/ninja.h')]])

    def test_a_header_of_engine_is_found_from_the_root(self):
        # a non-matching side names a header of engine/ by its path from the root, which make dc's GCC searches last
        self.write('src/player/player.c', '#ifdef NONMATCHING\n#include "engine/dc/turn.h"\n#endif\n')
        self.write('engine/dc/turn.h', '')
        self.assertEqual(self.included('src/player/player.c'), [Path('src/player/player.c'), Path('engine/dc/turn.h')])


class Preprocessed(unittest.TestCase):
    def test_reads_a_name_with_a_dollar_sign_and_a_raw_string_to_its_delimiter(self):
        c = csource.parse('int a$b = R"x(")" )x";\nchar *c = uR"(\n)", *d = xR"(e)";\n')
        self.assertEqual([(token.text, token.kind) for token in c.parsed if token.kind != 'punctuator'],
                         [('int', 'name'), ('a$b', 'name'), ('R"x(")" )x"', 'string'), ('char', 'name'),
                          ('c', 'name'), ('uR"(\n)"', 'string'), ('d', 'name'), ('xR', 'name'),
                          ('"(e)"', 'string')])

    def test_ends_a_raw_string_a_splice_parts_at_its_delimiter_as_written(self):
        # the splice stays in a raw string's text, so the )" before it closes nothing, and the { is the string's
        text = 'const char *s = R"(a)\\\n" { still literal text })";\n'
        c = csource.parse(text)
        self.assertEqual(([(token.kind, text[token.start:token.end]) for token in c.lexemes], c.blocks),
                         ([('string', 'R"(a)\\\n" { still literal text })"')], []))

    def test_gives_each_line_marker_and_leaves_its_tokens_out(self):
        # with each marker's flags, as 1 for a file GCC enters and 3 for a system header
        c = csource.parse('# 1 "a.c"\nint a;\n# 7 "b.h" 1 3\n#pragma pack\nint b;\n')
        self.assertEqual(c.markers, [(1, 'a.c', 1, ()), (3, 'b.h', 7, (1, 3))])
        self.assertEqual([token.text for token in c.parsed], ['int', 'a', ';', 'int', 'b', ';'])


class Brackets(unittest.TestCase):
    def test_pairs_brackets_of_any_kind_and_ends_an_initializer_outside_them(self):
        tokens = csource.parse('f(a[1}, {2, 3}, b) , c;').parsed
        # 0 f ( a [ 1 } , { 2 , 3 } , b ) , c ;
        self.assertEqual([tokens.closing(1), tokens.closing(3), tokens.closing(7)], [14, 5, 11])
        self.assertEqual([tokens.ending(0), tokens.ending(7), tokens.ending(16)], [15, 12, 17])
        self.assertEqual(list(tokens.listed(7, 12)), [(8, 9), (10, 11)])

    def test_names_a_token_that_ends_no_element_of_a_braced_list(self):
        tokens = csource.parse('int a[] = {1; 2}; int b[] = {1, 2,};').parsed
        with self.assertRaisesRegex(ValueError, r'^line 1: ; in a braced list$'):
            list(tokens.listed(5, 10))
        self.assertEqual(list(tokens.listed(16, 22)), [(17, 18), (19, 20)])

    def test_names_a_bracket_no_bracket_closes(self):
        tokens = csource.parse('int a[2 = {1;').parsed
        with self.assertRaisesRegex(ValueError, r'^line 1: \[ with no bracket closing it$'):
            tokens.closing(2)


class Reader(csource.Expressions):
    def typedef_at(self, name, at):
        return name == 'T'

    def typename(self, text, at):
        return text == 'T'

    def term(self, words):
        return ' '.join(words)


def shape(node):
    """node as nested tuples of its kind, its what and its operands', and a
    leaf as its what alone."""
    return (node.kind, node.what, *(shape(operand) for operand in node.operands)) if node.operands else node.what


class Expressions(unittest.TestCase):
    def read(self, text):
        return Reader(csource.parse(text).parsed).full()

    def test_reads_operators_by_precedence_and_casts_by_the_words_inside_and_after(self):
        self.assertEqual(shape(self.read('a = b + c * -d ? (T) e : (f) - g, h')),
                         ('comma', ',', ('assign', '=', 'a', ('conditional', '?', ('binary', '+', 'b', (
                             'binary', '*', 'c', ('unary', '-', 'd'))), ('cast', 'T', 'e'), ('binary', '-', (
                                 'parentheses', '(', 'f'), 'g'))), 'h'))
        self.assertEqual(self.read('(unsigned char *) p').term, 'unsigned char *')

    def test_takes_a_call_a_subscript_and_a_compound_literal_whole(self):
        self.assertEqual(shape(self.read('f(a, b + 1)[2] + (T) { 3, T }')),
                         ('binary', '+', ('opaque', 'a subscript', ('opaque', 'a call', 'f', 'a', (
                             'binary', '+', 'b', '1')), '2'), ('opaque', 'a compound literal', '3', 'T')))

    def test_reads_a_conditional_s_middle_operand_as_a_whole_expression(self):
        self.assertEqual(shape(self.read('a ? b, c : d')), ('conditional', '?', 'a', ('comma', ',', 'b', 'c'), 'd'))

    def test_reads_a_size_operand_whole_and_a_compound_literal_s_postfix(self):
        # (p)[i++] is the sizeof's operand, which no node of the expression holds; (T) is its type name
        node = self.read('sizeof (p)[i++] + 1')
        self.assertEqual((shape(node), node.operands[0].end), (('binary', '+', 'sizeof', '1'), 8))
        self.assertEqual([shape(self.read(text)) for text in ('sizeof (T) * 2', '(T){0}.x')],
                         [('binary', '*', 'sizeof', '2'),
                          ('opaque', "., an operator the reader doesn't take", ('opaque', 'a compound literal', '0'))])

    def test_reads_a_run_of_strings_as_one(self):
        # as modules/shared/hint_box.c keeps a text of three strings for its constants
        node = self.read('(void)"\\x82\\xde"\n      "\\x46" "!", f')
        self.assertEqual(shape(node), ('comma', ',', ('cast', 'void', '"\\x82\\xde" "\\x46" "!"'), 'f'))
        self.assertEqual((node.operands[0].operands[0].start, node.operands[0].operands[0].end), (3, 6))


class Walk(csource.Walk):
    def __init__(self, tokens):
        self.names, self.statics = [], []
        super().__init__(tokens)

    def typedef_at(self, name, at):
        return name == 'T'

    def specifiers(self, at):
        if self.tokens[at].text not in ('int', 'T'):
            raise csource.Unread(f'{self.tokens[at].text}: no type')
        return self.tokens[at].text, at + 1

    def declarator(self, at, base):
        function = self.tokens[at + 1].text == '('
        return self.tokens[at].text, base, function, self.tokens.closing(at + 1) + 1 if function else at + 1

    def declared(self, name, kind, type, span, start, end):
        self.names.append((name, kind, start, end))

    def enumerated(self, name, start, end):
        self.names.append((name, 'enumerator', start, end))

    def static(self, start, end, type, why):
        self.statics.append((start, end, type, why))


class Walks(unittest.TestCase):
    def test_binds_each_name_in_its_scope_and_gives_each_initializer_and_expression(self):
        tokens = csource.parse('int a = 1; int f(int b) { T c = b; if (c) { static x y = 2; } return a; }').parsed
        walk = Walk(tokens)
        self.assertEqual(walk.names, [('a', 'object', 2, 33), ('f', 'object', 11, 33), ('b', 'object', 11, 32),
                                      ('c', 'object', 14, 32)])
        self.assertEqual(walk.statics, [(3, 4, 'int', None), (26, 27, None, 'x: no type')])
        self.assertEqual([' '.join(token.text for token in tokens[start:end]) for start, end in walk.expressions],
                         ['b', 'c', 'a'])
        self.assertEqual(walk.unread, [(22, 28, 'x: no type')])
        self.assertEqual([walk.code(at) for at in (3, 13, 15, 30)], [False, False, True, True])

    def test_skips_a_declaration_at_file_scope_it_cant_read(self):
        # which hides no name, so the walk reads on past its ; and leaves it out of unread
        walk = Walk(csource.parse('x y; int a;').parsed)
        self.assertEqual((walk.skipped, walk.unread, walk.names), ([(0, 'x: no type')], [], [('a', 'object', 5, 6)]))

    def test_closes_a_for_s_declaration_with_the_statement_it_controls(self):
        # past the braces of its body, or past an if and its else, and before the declaration after the for
        for body in ('{ i; }', 'if (b) i; else i;'):
            with self.subTest(body=body):
                tokens = csource.parse(f'int f(int b) {{ for (int i = 0; i < b; ++i) {body} int after; }}').parsed
                walk = Walk(tokens)
                start, end = next((start, end) for name, _, start, end in walk.names if name == 'i')
                self.assertEqual(([token.text for token in tokens[start:start + 2]],
                                  [token.text for token in tokens[end:end + 2]]), (['=', '0'], ['int', 'after']))

    def test_ends_an_initializer_past_a_comma_in_a_conditional_s_middle_operand(self):
        tokens = csource.parse('int f(int a) { int x = a ? b, c : d; }').parsed
        start, end = Walk(tokens).expressions[0]
        self.assertEqual(shape(Reader(tokens, start, end).full()),
                         ('conditional', '?', 'a', ('comma', ',', 'b', 'c'), 'd'))

    def test_passes_over_an_attribute_between_the_specifiers_and_the_declarator(self):
        # as the non-matching side of sdk/ninja/njRotateX.c keeps its turn of a matrix in memory out of line
        tokens = csource.parse('static int __attribute__((noinline, cold)) f(int b) { return b; }').parsed
        walk = Walk(tokens)
        self.assertEqual((walk.names, walk.unread, walk.declarators[0]),
                         ([('f', 'object', 15, 20), ('b', 'object', 15, 19)], [], (2, 15)))
        self.assertEqual([' '.join(token.text for token in tokens[start:end]) for start, end in walk.expressions],
                         ['b'])


class AssemblyBlocks(unittest.TestCase):
    def test_gives_each_line_less_its_trailing_blanks(self):
        self.assertEqual(csource.assembly_blocks('#pragma asm\n    MOV R0,R1\t\n#pragma endasm\n'),
                         [('    MOV R0,R1',)])


# the tree's own C: each C file make lint reads, and the headers of config/ that clang-tidy reads ahead of each source
LINTED = ('src', 'sdk', 'modules', 'engine', 'tests/kernels')
C_FILES = sorted([*(path for root in LINTED for path in (ROOT / root).rglob('*.[ch]')), *(ROOT / 'config').glob('*.h')])
# the directories of the tree's C, each header and .inc file of which some file must include
HEADER_DIRECTORIES = ('config', 'engine', 'modules', 'sdk', 'src', 'tests')
# an #include in the C a Python script writes
SCRIPT_INCLUDE = re.compile(r'#[ \t]*include[ \t]*[<"]([^>"\n]+)[>"]')
# a test's -I of a directory of tests/, whose header the test reads in place of the tree's of the same name
TEST_ROOT = re.compile(r"'-I(tests/[^']+)'")
# each bracket that opens, and the one that closes it
CLOSES = {'(': ')', '[': ']', '{': '}'}


def unread(name, c):
    """A line for each part of c, a C of the file called name, that it
    reads otherwise than C does: a block no brace closes, a bracket that
    closes none or that another kind of bracket closes, and a conditional
    no #endif closes or an #else or #endif that closes none."""
    found = [f'{name}:{block.opening.line}: a block no brace closes' for block in c.blocks if block.closing is None]
    tokens, depth = c.parsed, 0
    for at, close in enumerate(tokens.closes):
        text = tokens[at].text
        if text in CLOSES and (close is None or tokens[close].text != CLOSES[text]):
            found.append(f'{name}:{tokens[at].line}: {text}, which {"no" if close is None else "another"} bracket '
                         'closes')
        depth += (text in CLOSES) - (text in CLOSES.values())
        if depth < 0:
            found.append(f'{name}:{tokens[at].line}: {text}, which closes no bracket')
            depth = 0
    opened = []
    for directive in c.directives:
        if directive.name in csource.OPENING:
            opened.append(directive.line)
        elif directive.name in (*csource.LATER, 'endif') and not opened:
            found.append(f'{name}:{directive.line}: #{directive.name}, in no conditional')
        elif directive.name == 'endif':
            opened.pop()
    return found + [f'{name}:{line}: a conditional no #endif closes' for line in opened]


class TheTree(unittest.TestCase):
    def test_reads_each_side_of_every_c_file_as_c_does(self):
        # each side of each guard, as SHC and make dc's GCC compile it, and each #pragma inline_asm opens its own
        # function's body
        self.assertTrue(C_FILES)
        problems = []
        for path in C_FILES:
            name = path.relative_to(ROOT).as_posix()
            for compiler in (csource.SHC, csource.GCC):
                text = csource.code(path, compiler)
                problems += [f'{line} ({compiler})' for line in unread(name, csource.parse(text))]
                csource.bodies(name, csource.blanked(text).split('\n'), problems)
        self.assertEqual(problems, [])

    def test_finds_what_it_doesn_t_read(self):
        # the problems test_reads_each_side_of_every_c_file_as_c_does fails on
        c = csource.parse(')\nint f(void)\n{\n    g(a];\n#else\n#if X\n')
        self.assertEqual(unread('a.c', c), ['a.c:3: a block no brace closes', 'a.c:1: ), which closes no bracket',
                                            'a.c:3: {, which no bracket closes',
                                            'a.c:4: (, which another bracket closes', 'a.c:5: #else, in no conditional',
                                            'a.c:6: a conditional no #endif closes'])

    def test_reads_every_declaration_of_each_source_as_shc_compiles_it(self):
        # decls.py's reader reads each C source, and each file it includes, as SHC compiles them, and fails on a
        # declaration it can't read. Every include the build generates for a source is its text.inc or the body of a
        # declaration the reader reads, and body_data.declares_automatic() finds each body a function declares
        # with automatic storage
        self.addCleanup(os.chdir, Path.cwd())
        os.chdir(ROOT)
        problems = []
        for path in C_FILES:
            if path.suffix == '.c':
                source = path.relative_to(ROOT)
                try:
                    texts, _, declarators = decls.declarations(source)
                    automatic = body_data.automatic(body_data.bodies(source, texts, declarators))
                    if body_data.declares_automatic(source, texts) != bool(automatic):
                        problems.append(f'{source}: declares_automatic() and its declarations disagree')
                except SystemExit as error:
                    problems.append(str(error.code))
        self.assertEqual(problems, [])

    def test_reads_what_split_py_and_inline_py_read_as_they_do(self):
        # tools/split.py reads the lines make dc's GCC compile takes, and tools/inline.py the inline bodies, each by its
        # own means: on each file, both sides of its guards, the reader gives the same answer
        differ = []
        for path in C_FILES:
            name, text = path.relative_to(ROOT).as_posix(), content(path).decode('latin-1')
            lines = csource.blanked(csource.code(path)).split('\n')
            if csource.compiled(lines) != split.compiled(text):
                differ.append(f'{name}: compiled() and split.py read its guards otherwise')
            if [body[1] for body in csource.bodies(name, lines, [])] != inline.bodies(text):
                differ.append(f'{name}: bodies() and inline.py find other inline bodies')
        self.assertEqual(differ, [])

    def test_includes_every_header(self):
        # each header and .inc file of HEADER_DIRECTORIES has an includer, which names it as a compiler finds it:
        # from the includer's own directory, from a route's include directory or from a test's. The includer is a
        # C file of HEADER_DIRECTORIES, a script of tests/ or tools/, which writes C, or compile_flags.txt, which
        # has clang read its headers ahead of each source. An #include of a macro names the string each #define
        # gives the macro
        scripts = [path for directory in ('tests', 'tools') for path in (ROOT / directory).glob('*.py')]
        roots = [ROOT / root for root in csource.INCLUDE_ROOTS]
        roots += sorted({ROOT / root for path in scripts for root in TEST_ROOT.findall(path.read_text())})
        files = sorted(path for directory in HEADER_DIRECTORIES for path in (ROOT / directory).rglob('*')
                       if path.suffix in ('.c', '.h', '.inc'))

        def tried(includer, quoted, name):
            # each path a compiler tries for an #include of name in the file includer
            return {os.path.normpath(root / name) for root in ([includer.parent] if quoted else []) + roots}

        found, includers, strings = set(), {}, []
        for path in files:
            text = content(path).decode('latin-1')
            for _, quoted, name in csource.header_names(text):
                found |= tried(path, quoted, name)
            for directive in csource.parse(text).directives:
                words = directive.words
                if directive.name == 'include' and len(words) == 1 and words[0].kind == 'name':
                    includers.setdefault(words[0].text, []).append(path)
                elif directive.name == 'define' and len(words) == 2 and words[1].kind == 'string':
                    strings.append((words[0].text, words[1].text[1:-1]))
        for macro, name in strings:
            for includer in includers.get(macro, []):
                found |= tried(includer, True, name)
        for path in scripts:
            for name in SCRIPT_INCLUDE.findall(path.read_text()):
                found |= tried(path, False, name)
        flags = (ROOT / 'compile_flags.txt').read_text().split()
        found.update(os.path.normpath(ROOT / name) for option, name in zip(flags, flags[1:]) if option == '-include')
        unincluded = [str(path.relative_to(ROOT)) for path in files if path.suffix != '.c' and str(path) not in found]
        self.assertEqual(unincluded, [])


if __name__ == '__main__':
    unittest.main()
