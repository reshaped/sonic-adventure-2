"""The tags: what TAGS.md lets a tag be, and where its addresses lie."""

import re

import tags
from support import TreeTest, run


def faults(text):
    return tags.faults('a.c', text)[0]


def line(text, marker):
    """The number of the line of text where marker starts."""
    return text[:text.index(marker)].count('\n') + 1


# what faults() gives an @asm tag that stands above neither a #pragma
# inline_asm nor a #pragma asm that takes one, a block without its tag, and
# an @asm tag inside a block; and a block that exports a function
ABOVE = ("an @asm tag goes directly above an inline body's #pragma inline_asm or the #pragma asm of a block that "
         'holds more than comments')
UNTAGGED = 'a #pragma asm block without an @asm tag directly above it'
INSIDE = 'an @asm tag inside a #pragma asm block: a block takes its tag in C, directly above its #pragma asm'
# the line --review ends on for one open @asm tag
ONE_OPEN = ('@asm tags of inline bodies, #pragma asm blocks and .src functions by kind: 0 opcode, 0 operand, 0 abi, '
            '0 epilogue, 0 name and 1 open; 1 in all')
BLOCK = ('#pragma asm\n        .SECTION P,CODE,ALIGN=32\n        .EXPORT _f\n_f:\n        RTS\n        NOP\n'
         '#pragma endasm\n')


class Tags(TreeTest):
    def test_passes_well_formed_tags(self):
        self.assertEqual(faults('/* @hack spelling: first, a local for the base, where the member would do */\n'
                                '/* @hack redundant: register\n * on count */\n'
                                '/* @bug memory: reads task after its step may have freed it */\n'
                                '/* an ordinary comment that mentions nobody@example */\n'), [])

    def test_a_tag_opens_a_comment_of_its_own(self):
        self.assertEqual(faults('\n/* note: @hack spelling: the local */'),
                         ['a.c:2: a tag opens a comment of its own'])
        self.assertEqual(faults('/* @bug logic: a. @bug logic: b */'), ['a.c:1: a tag opens a comment of its own'])

    def test_a_tag_opens_a_plain_comment(self):
        slashed = 'a.c:1: a tag opens a plain /* */ comment, not a /** */ one'
        self.assertEqual(faults('/** @hack spelling: the local, where the member would do */'), [slashed])
        self.assertEqual(faults('/**\n * @bug logic: reads the slot twice\n */'), [slashed])
        # a /** */ comment without a tag is no fault
        self.assertEqual(faults('/** the record */\n/**/'), [])

    def test_a_tag_takes_a_kind_of_its_own_and_text(self):
        self.assertEqual(faults('/* @hack memory: the local */'),
                         ['a.c:1: memory: not a kind of @hack; they are implicit, type, redundant, spelling'])
        self.assertEqual(faults('/* @bug memory */'), ['a.c:1: not a tag: @bug KIND: TEXT'])

    def test_a_hack_gives_no_count_and_cites_no_address(self):
        self.assertEqual(faults('/* @hack spelling: the local, where the member would do */'), [])
        self.assertEqual(faults('/* @hack spelling: the local; without it, 3 of 64 words differ */'),
                         ["a.c:1: 3 of 64 words differ: an @hack's text gives no count; tools/words.py measures one"])
        cites = "a.c:1: an @hack's text cites no address: it says what is kept, and the plain form that stands for"
        self.assertEqual(faults('/* @hack spelling: the local, which keeps the base in R12 (0x8c0ac7d0) */'), [cites])
        self.assertEqual(faults("/* @hack spelling: the local (FDIV at 0x36 of the object's code) */"), [cites])
        self.assertEqual(faults("/* @hack spelling: the entry (0xcc of the object's data) */"), [cites])

    def test_a_hack_names_no_register(self):
        named = "a.c:1: {}: an @hack's text names no register: it says what is kept, and the plain form that stands for"
        for text, register in (('a bare return, where return NULL would do: R0 holds the null', 'R0'),
                               ('the store at @R15, where a local would do', 'R15'),
                               ('the product kept in MACL, where a local would do', 'MACL'),
                               ('the load into XD2, where FR2 would do', 'XD2')):
            with self.subTest(register=register):
                self.assertEqual(faults(f'/* @hack spelling: {text} */'), [named.format(register)])
        # but not a C name, a local or a field spelled as a register in lower case, or R16, which names none
        for text in ('fpscr read and written, where set_fpscr(get_fpscr() & ~FPSCR_PR) would do',
                     'rcdpi->r15 read as a byte, where its field would do', 'the label R16, where R_16 would do'):
            with self.subTest(text=text):
                self.assertEqual(faults(f'/* @hack spelling: {text} */'), [])

    def test_register_on_a_local_is_redundant(self):
        kept = ('a.c:1: {}: an @hack that keeps register on a local or a parameter is redundant, the first kind that '
                'fits a storage class')
        for text in ('count declared register, where a plain local would do',
                     'first and next declared\n * register, where plain locals would do',
                     'register on p and s, where plain locals would do',
                     'the register on budget',
                     'task a register variable, where a plain parameter would do',
                     'task and work register variables, where plain ones would do',
                     'a register local, where a plain one would do',
                     'task a register parameter, where a plain parameter would do',
                     'count a register int, where an int would do',
                     'step as a register Float, where a Float would do'):
            with self.subTest(text=text):
                self.assertEqual(faults(f'/* @hack redundant: {text} */'), [])
                for kind in ('implicit', 'type', 'spelling'):
                    self.assertEqual(faults(f'/* @hack {kind}: {text} */'), [kept.format(kind)])
        # but not a register of the machine, a call of collision_register, or a local's place among register ones
        for text in ('the AICA register on the G2 bus read as a Sint32, where a Uint32 would do',
                     'a collision_register on each path through nested tests, where one call would do',
                     "the fit inside the register's argument, where a statement of its own would do",
                     'sy declared after the register locals, where beside z would do'):
            with self.subTest(text=text):
                self.assertEqual(faults(f'/* @hack type: {text} */'), [])

    def test_an_asm_tag_names_its_body_and_the_hosts_shc_compiles(self):
        # a host calls the body by name, directly or through a macro. NONMATCHING's side is GCC's alone
        c = ('/* @asm open: nothing yet */\n#pragma inline_asm(walk)\nstatic void walk(void)\n{\n        NOP\n}\n'
             'void host(void)\n{\n    walk();\n}\n#define WALK() walk()\nvoid macro_host(void)\n{\n    WALK();\n}\n'
             '#ifdef NONMATCHING\nvoid port(void)\n{\n    walk();\n}\n#endif\n')
        self.assertEqual(tags.faults('a.c', c),
                         ([], [tags.Tag(1, 'asm', 'open', 'nothing yet', ('walk', 'host', 'macro_host'))]))
        # an object-like macro calls the body wherever its name stands
        c += '#define RUN walk()\nvoid object_host(void)\n{\n    RUN;\n    next();\n}\n'
        self.assertEqual(tags.faults('a.c', c)[1][0].names, ('walk', 'host', 'macro_host', 'object_host'))

    def test_reads_tags_in_comments_alone(self):
        # a string's bytes or a // comment's hold no tag, but the same bytes as a comment do, on their line
        for c in ('const char *example = "/* @bug memory: example */";\n', '// /* @bug memory: example */\n',
                  'const char *example = "/* @asm open: example */";\n'):
            with self.subTest(c):
                self.assertEqual(tags.faults('a.c', c), ([], []))
                self.assertEqual(tags.asm_above(c), {})
        c = 'int x;\n/* @bug memory: example */\nconst char *example = "";\n'
        self.assertEqual(tags.faults('a.c', c), ([], [tags.Tag(2, 'bug', 'memory', 'example', ())]))

    def test_no_other_tag_is_defined(self):
        self.assertEqual(faults('/* @todo fix this */'), ['a.c:1: @todo: not a tag TAGS.md defines'])

    def test_an_asm_tag_opens_with_its_rule_and_cites_an_address(self):
        body = '\n#pragma inline_asm(f)\nvoid f(void)\n{\n}\n'
        self.assertEqual(faults('/* @asm operand: fipr-registers, FIPR FV4,FV4 at 0x8c0140b6 */' + body
                                + '/* @asm open: no rule reads its FSCHG */\n#pragma inline_asm(g(size=4))\n'), [])
        self.assertEqual(faults('/* @asm operand: FIPR FV4,FV4 at 0x8c0140b6 */' + body),
                         ["a.c:1: an @asm operand's text opens with one of fipr-registers, fipr-unloaded, fipr-shared, "
                          'ftrv-registers, fsca-registers, small-constant, integer-constant, zero-displacement, '
                          'fschg-slot, fschg-pairs, fschg-dt, fschg-stores, fschg-register-moves, then a comma'])
        self.assertEqual(faults('/* @asm epilogue: dead-return, after the RTS */' + body),
                         ['a.c:1: an @asm epilogue cites an address, 0x and eight lowercase hex digits'])
        self.assertEqual(faults('/* @asm name: two words, at 0x8c0140b6 */' + body),
                         ["a.c:1: an @asm name's text opens with the member's name, then a comma"])

    def test_an_inline_body_takes_one_asm_tag_directly_above(self):
        self.assertEqual(faults('/* its comment */\n#pragma inline_asm(f)\n'),
                         ['a.c:2: an inline body without an @asm tag directly above it'])
        self.assertEqual(faults('/* @asm open: nothing yet */\n\n#pragma inline_asm(f)\n'),
                         [f'a.c:1: {ABOVE}', 'a.c:3: an inline body without an @asm tag directly above it'])
        self.assertEqual(faults('/* @asm open: nothing yet */\n#pragma inline_asm(f, g)\n'),
                         ['a.c:2: not #pragma inline_asm(NAME) or #pragma inline_asm(NAME(size=BYTES))'])

    def test_a_blocks_tag_stands_directly_above_its_pragma_asm(self):
        tagged = '/* @asm open: nothing yet */\n' + BLOCK
        self.assertEqual(tags.faults('a.c', tagged), ([], [tags.Tag(1, 'asm', 'open', 'nothing yet', ())]))
        # above a block of instructions that supplies no function too, and one of directives alone
        self.assertEqual(faults('/* @asm open: nothing yet */\n#pragma asm\n        MOV #1,R0\n#pragma endasm\n'), [])
        self.assertEqual(faults('/* @asm open: nothing yet */\n#pragma asm\n        .ALIGN 32\n#pragma endasm\n'), [])
        # under the other tags of a stack
        self.assertEqual(faults('/* @bug logic: the padding */\n/* @asm open: nothing yet */\n#pragma asm\n'
                                '        .ALIGN 32\n#pragma endasm\n'), [])
        # and nowhere else
        self.assertEqual(faults('/* @asm open: nothing yet */\n\n' + BLOCK), [f'a.c:1: {ABOVE}', f'a.c:3: {UNTAGGED}'])

    def test_a_block_of_more_than_comments_takes_a_tag(self):
        self.assertEqual(faults(BLOCK), [f'a.c:1: {UNTAGGED}'])
        padding = '/* the padding */\n#pragma asm\n        .ALIGN 32 ; to the next object\n#pragma endasm\n'
        self.assertEqual(faults(padding), [f'a.c:2: {UNTAGGED}'])
        self.assertEqual(faults('#pragma asm\n?loop:\n#pragma endasm\n'), [f'a.c:1: {UNTAGGED}'])
        # but a block of comments takes none, and a #pragma asm in a comment opens no block
        comments = '#pragma asm\n; a comment\n\n#pragma endasm\n'
        self.assertEqual(faults(comments), [])
        self.assertEqual(faults('/* @asm open: nothing yet */\n' + comments), [f'a.c:1: {ABOVE}'])
        self.assertEqual(faults('/*\n#pragma asm\n        NOP\n#pragma endasm\n*/\n'), [])

    def test_a_block_holds_no_tag_of_its_own(self):
        tagged = '/* @asm open: nothing yet */\n' + BLOCK
        self.assertEqual(faults(tagged.replace('_f:', '; @asm open: nothing yet\n_f:')), [f'a.c:5: {INSIDE}'])
        # where no tag stands above it, and where it holds only comments
        self.assertEqual(faults(BLOCK.replace('_f:', '; @asm open: nothing yet\n_f:')),
                         [f'a.c:1: {UNTAGGED}', f'a.c:4: {INSIDE}'])
        self.assertEqual(faults('#pragma asm\n; @asm open: nothing yet\n#pragma endasm\n'), [f'a.c:2: {INSIDE}'])
        # nor any @hack, which steers SHC, though an @bug may stand there
        self.assertEqual(faults(tagged.replace('_f:', '; @hack spelling: the label\n_f:')),
                         ['a.c:5: an @hack in assembly: a hack steers SHC, which writes none of a .src file'])
        self.assertEqual(faults(tagged.replace('_f:', '; @bug logic: the label\n_f:')), [])


# a.c with each never-run test under its @hack redundant tag: one in a
# macro, one alone, and one under a stack of tags whose redundant one isn't
# the last
HACKED = r"""
#define WEIGH(value)                                                           \
    /* @hack redundant: a test of value that never runs */                    \
    if (0) {                                                                   \
        (void)(value);                                                         \
    }

void outer(int x)
{
    /* @hack redundant: a call that never runs */
    if (0) {
        host();
    }
    /* @hack redundant: the test that never runs */
    /* @hack spelling: the empty else, where none would do */
    if (0) {
        if (x) {
        }
    } else {
    }
    /* @bug memory: reads through a null pointer */
    WEIGH(*(int *)0);
}
"""
# the never-run call's tag, and the if (0) under it
CALL = '    /* @hack redundant: a call that never runs */\n    if (0) {\n'


def untagged(text, marker, test='if (0)'):
    """The fault faults() gives text for the never-run test on the line where
    marker starts in it."""
    return f'a.c:{line(text, marker)}: {test} has no @hack redundant tag directly above it'


class NeverRun(TreeTest):
    def test_wants_a_redundant_tag_above_each_never_run_test(self):
        self.assertEqual(faults(HACKED), [])
        plain = HACKED.replace(CALL, '    /* a call that never runs */\n    if (0) {\n')
        self.assertEqual(faults(plain), [untagged(plain, '    if (0) {\n        host')])
        bare = HACKED.replace(CALL, '    if (0) {\n')
        self.assertEqual(faults(bare), [untagged(bare, '    if (0) {\n        host')])
        # in a #define too, on a line of its own or the directive's
        macro = HACKED.replace('/* @hack redundant: a test of value', '/* a test of value')
        self.assertEqual(faults(macro), [untagged(macro, '    if (0) {     ')])
        never = HACKED + '#define NEVER() if (0) { }\n'
        self.assertEqual(faults(never), [untagged(never, '#define NEVER')])

    def test_wants_the_tag_directly_above(self):
        tag = CALL.split('\n')[0]
        for above in (tag + '\n\n', tag + '\n    /* a call that never runs */\n', tag + '\n    host();\n',
                      '    /* @bug memory: a call through a null pointer */\n',
                      '    /* @hack spelling: the call, where none would do */\n'):
            with self.subTest(above=above):
                text = HACKED.replace(CALL, above + '    if (0) {\n')
                self.assertEqual(faults(text), [untagged(text, '    if (0) {\n        host')])

    def test_reads_each_spelling_of_a_never_run_if_and_while(self):
        for spelling, shown in (('if (0U) {', 'if (0U)'), ('if (FALSE) {', 'if (FALSE)'),
                                ('if (0 && x) {', 'if (0 && ...)'), ('if(0x0) {', 'if (0x0)'),
                                ('if ( 0L ) {', 'if (0L)'), ('if (0 &&\n        x) {', 'if (0 && ...)'),
                                ('if (0 \\\n        && x) {', 'if (0 && ...)'), ('if (00) {', 'if (00)'),
                                ('if ((0)) {', 'if (0)'), ('if ((0) && x) {', 'if (0 && ...)'),
                                ('if ((0 && x)) {', 'if (0 && ...)'), ('if (0 && (x || x)) {', 'if (0 && ...)'),
                                ('while (0) {', 'while (0)'), ('while (FALSE)\n    {', 'while (FALSE)'),
                                ('while (0) \\\n    {', 'while (0)')):
            with self.subTest(spelling=spelling):
                text = HACKED.replace(CALL, f'    {spelling}\n')
                self.assertEqual(faults(text), [untagged(text, f'    {spelling}', shown)])
                self.assertEqual(faults(HACKED.replace('    if (0) {\n        host();',
                                                       f'    {spelling}\n        host();')), [])
        # after an else, on whose line the tag stands above
        text = HACKED.replace(CALL, '    if (x) {\n    } else if (0) {\n')
        self.assertEqual(faults(text), [untagged(text, '    } else if (0)')])
        self.assertEqual(faults(HACKED.replace(CALL, '    if (x) {\n' + CALL.replace('if (0) {', '} else if (0) {'))),
                         [])

    def test_reads_a_never_run_test_only_in_code(self):
        others = ('/* if (0) { } */\n'
                  'static const char *never = "if (0) {";\n'
                  "static const char flag = '0';\n"
                  '#if (0)\n'
                  '#endif\n'
                  'void others(int x, int FALSE_START, int x0)\n'
                  '{\n'
                  '    do {\n'
                  '    } while (0);\n'
                  '    do\n'
                  '        x++;\n'
                  '    while (FALSE);\n'
                  '    if (0x10 || 01 || 0.5F || x0) {\n'
                  '    }\n'
                  '    if (0 || x || 0 == x || FALSE_START) {\n'
                  '    }\n'
                  '    if (0 && x || x0) {\n'
                  '    }\n'
                  '    if ((0) + 1 || (0) || x || (0 && x) || x0) {\n'
                  '    }\n'
                  '    while (0 ? x : x0) {\n'
                  '    }\n'
                  '    if (0 && x ? x : x0) {\n'
                  '    }\n'
                  '    if (0 && x, x0) {\n'
                  '    }\n'
                  '    elif(0);\n'
                  '}\n')
        self.assertEqual(faults(HACKED + others), [])
        # but finds the one untagged if (0) among them
        text = HACKED.replace(CALL, '    if (0) {\n') + others
        self.assertEqual(faults(text), [untagged(text, '    if (0) {\n        host')])

    def test_wants_no_tag_on_a_guards_non_matching_side(self):
        # which SHC never compiles, so no hack stands there
        guarded = ('void f(int x)\n{\n#ifdef NONMATCHING\n    if (FALSE) {\n#else\n    if (x) {\n#endif\n    }\n'
                   '#ifdef NONMATCHING\n#define NEVER() if (0) { }\n#endif\n}\n')
        self.assertEqual(faults(guarded), [])
        # but on the matching side, as outside any guard
        matching = guarded.replace('#ifdef NONMATCHING\n    if (FALSE) {\n#else\n    if (x) {\n',
                                   '#ifdef NONMATCHING\n    if (x) {\n#else\n    if (FALSE) {\n')
        self.assertEqual(faults(matching), [untagged(matching, '    if (FALSE)', 'if (FALSE)')])


SRC = """\
          .EXPORT     _copy, _table
          .SECTION    P,CODE,ALIGN=4
SIZE:     .EQU        4
; void copy(void *to)
; copy a word
; @asm name: copyasm, the member that holds it; it starts at
; 0x8c0c0d6c
_copy:
_copy_alias:
          MOV.L       @R4,R0
?loop:
          RTS
          NOP
; a helper nothing exports
local:
          RTS
          NOP
          .SECTION    D,DATA,ALIGN=4
; the table
_table:
          .DATA.L     0
          .END
"""


class AssemblyTags(TreeTest):
    def faults(self, text):
        return tags.assembly_faults('a.src', text)[0]

    def test_passes_one_tag_ending_the_comment_above_each_function(self):
        self.assertEqual(self.faults(SRC), [])
        self.assertEqual(self.faults(SRC.replace('_copy:\n_copy_alias:', '_copy_alias:\n; an entry\n_copy:')), [])

    def test_a_function_takes_one_tag(self):
        bare = SRC.replace('; @asm name: copyasm, the member that holds it; it starts at\n; 0x8c0c0d6c\n', '')
        self.assertEqual(self.faults(bare), ['a.src:6: _copy: a function without an @asm tag'])
        twice = SRC.replace('_copy_alias:', '; @asm open: nothing yet\n_copy_alias:')
        self.assertEqual(self.faults(twice), ['a.src:9: _copy: a second @asm tag for one function'])

    def test_a_tag_goes_above_a_functions_label_alone(self):
        self.assertEqual(self.faults(SRC.replace('; a helper nothing exports', '; @asm open: nothing yet')),
                         ["a.src:14: an @asm tag goes above a function's label, and local names none"])
        self.assertEqual(self.faults(SRC.replace('; the table', '; @asm open: nothing yet')),
                         ["a.src:19: an @asm tag goes above a function's label, and _table names none"])
        self.assertEqual(self.faults(SRC.replace('?loop:', '; @asm open: nothing yet\n\n?loop:')),
                         ["a.src:11: an @asm tag ends the comment directly above a function's label"])

    def test_a_label_before_data_starts_no_function(self):
        # in a code section too: it takes no tag, and a tag above it fails
        data = '_word:\n          .DATA.L     0\n'
        word = SRC.replace('_copy, _table', '_copy, _table, _word').replace(
            '; a helper nothing exports\n', f'; a word of data\n{data}; a helper nothing exports\n')
        # a blank line below the label leaves the statement under it to say so
        apart = word.replace(data, '_word:\n\n          .DATA.L     0\n')
        self.assertEqual(self.faults(apart.replace('.DATA.L     0', 'RTS')), ['a.src:15: _word: a function without an '
                                                                            '@asm tag'])
        for directive, spelled in (('.DATA.L', word), ('.RES.L', word.replace(data, '_word:    .RES.L      1\n')),
                                   ('a blank line, then .DATA.L', apart)):
            with self.subTest(directive=directive):
                self.assertEqual(self.faults(spelled), [])
                self.assertEqual(self.faults(spelled.replace('; a word of data', '; @asm open: nothing yet')),
                                 ["a.src:14: an @asm tag goes above a function's label, and _word names none"])

    def test_a_tag_keeps_its_form(self):
        self.assertEqual(self.faults(SRC.replace('; copy a word', '; copy a word, @asm open: x')),
                         ['a.src:5: a tag opens a comment of its own'])
        self.assertEqual(self.faults(SRC.replace('; a helper nothing exports',
                                                 '; @hack spelling: the local')),
                         ['a.src:14: an @hack in assembly: a hack steers SHC, which writes none of a .src file'])
        self.assertEqual(self.faults(SRC.replace('; the table', '; @bug logic: the table is one entry short')), [])


# the main executable's code, a.c's and b.src's, and a stage that places a
# shared source's 0x40 bytes of code. The main executable names a.c's
# functions, and two labels of b.src's one function
MAIN = """\
file 1ST_READ.BIN
P 0x8c010000 0x8c010100
D 0x8c010100 0x8c010200
src/a.c P 0x8c010000 0x8c010040
src/b.src P 0x8c010040 0x8c010080
src/a.c D 0x8c010100 0x8c010110
"""
MAIN_SYMBOLS = """\
0x8c010000 entry
0x8c010010 walk
0x8c010020 host
0x8c010030 other
0x8c010040 copy
0x8c010040 copy_alias
"""
STAGE = """\
file SONIC2/STG13.PRS
P 0x8c500000 0x8c500100
D 0x8c500100 0x8c500200
modules/shared/c.c P 0x8c500020 0x8c500060
modules/shared/c.c D 0x8c500100 0x8c500120
"""


class Citations(TreeTest):
    def setUp(self):
        super().setUp()
        for path in ('src/a.c', 'src/b.src', 'modules/shared/c.c'):
            self.write(path, '')
        self.image('1st_read', MAIN, MAIN_SYMBOLS)
        self.image('stg13', STAGE)
        self.layout = tags.Layout(tags.Image(name) for name in tags.images())

    def faults(self, path, tag, text, names=()):
        return self.layout.stray(path, tags.Tag(1, tag, 'open', text, names))

    def test_an_asm_cites_its_own_sources_code(self):
        self.assertEqual(self.faults('src/a.c', 'asm', 'swap, SWAP.B at 0x8c010010'), [])
        self.assertEqual(self.faults('src/a.c', 'asm', 'swap, SWAP.B at 0x8c010104'),
                         ["src/a.c:1: 0x8c010104 lies outside its source's code in every image that places it"])

    def test_an_asm_cites_its_functions_code(self):
        # an inline body's tag names its function and its hosts, and may cite their code, or that of a function of
        # its source that its text names
        names = ('walk', 'host')
        self.assertEqual(self.faults('src/a.c', 'asm', 'swap, SWAP.B at 0x8c010010 and 0x8c01002e', names), [])
        for cited in ('0x8c010000', '0x8c010030'):
            with self.subTest(cited=cited):
                self.assertEqual(self.faults('src/a.c', 'asm', f'swap, SWAP.B at {cited}', names),
                                 [f'src/a.c:1: {cited} lies outside the code of host and walk'])
        self.assertEqual(self.faults('src/a.c', 'asm', 'swap, SWAP.B in entry, at 0x8c010000', names), [])
        # a .src function's tag names its run's labels, whose code runs to the next name or the row's end
        self.assertEqual(self.faults('src/b.src', 'asm', 'swap, SWAP.B at 0x8c01007e', ('copy',)), [])
        self.assertEqual(self.faults('src/b.src', 'asm', 'swap, SWAP.B at 0x8c010080', ('copy',)),
                         ['src/b.src:1: 0x8c010080 lies outside the code of copy'])
        # a tag where no image names its function or a host cites its source's code, and so does a #pragma asm
        # block's, which names none
        for names in (('gone',), ()):
            with self.subTest(names=names):
                self.assertEqual(self.faults('src/a.c', 'asm', 'swap, SWAP.B at 0x8c010030', names), [])
                self.assertEqual(self.faults('src/a.c', 'asm', 'swap, SWAP.B at 0x8c010040', names),
                                 ["src/a.c:1: 0x8c010040 lies outside its source's code in every image that places "
                                  'it'])

    def test_a_bug_cites_an_address_of_some_image(self):
        self.assertEqual(self.faults('src/a.c', 'bug', 'reads 0x8c0101f0 and 0x8c5000f0'), [])
        self.assertEqual(self.faults('src/a.c', 'bug', 'stores 0x20000000 bytes above'),
                         ["src/a.c:1: 0x20000000 lies in no image's sections; a constant is spelled H'20000000"])

    def test_an_offset_lies_in_its_objects_code(self):
        self.assertEqual(self.faults('modules/shared/c.c', 'bug', "FDIV at 0x36 and 0x3e of the object's code"), [])
        self.assertEqual(self.faults('modules/shared/c.c', 'bug', "FDIV at 0x36 and 0x40 of the object's code"),
                         ["modules/shared/c.c:1: 0x40 of the object's code lies past its end, at 0x40"])

    def test_an_offset_lies_in_its_objects_data(self):
        self.assertEqual(self.faults('modules/shared/c.c', 'bug', "its y, at 0x1c of the object's data, is -10"), [])
        self.assertEqual(self.faults('modules/shared/c.c', 'bug', "its y, at 0x20 of the object's data, is -10"),
                         ["modules/shared/c.c:1: 0x20 of the object's data lies past its end, at 0x20"])

    def test_names_a_file_by_its_path_from_the_root_however_spelled(self):
        # the tables place src/a.c, so the tag of ./src/a.c may cite its code
        self.write('src/a.c', f'/* @asm opcode: swap, SWAP.B at 0x8c010010 */\n{BLOCK}')
        self.assertEqual(run(tags.main, './src/a.c', 'src/../src/b.src'), (0, ''))

    def test_reads_each_tag_in_form_with_its_line(self):
        text = '/* a comment */\n/* @bug logic: one\n * and two */\n/* @hack */\n'
        self.assertEqual(tags.faults('a.c', text)[1], [tags.Tag(2, 'bug', 'logic', 'one and two', ())])
        # a .src function's tag names the run's labels as the tables do
        self.assertEqual(tags.assembly_faults('a.src', SRC)[1],
                         [tags.Tag(6, 'asm', 'name', 'copyasm, the member that holds it; it starts at 0x8c0c0d6c',
                                   ('copy', 'copy_alias'))])

    def test_reviews_assembly_entries_in_a_c_file(self):
        code = '        .SECTION P,CODE,ALIGN=32\n        .EXPORT _vertex\n'
        end = '_vertex:\n        RTS\n        NOP\n#pragma endasm\n#endif\n'
        tag = '/* @asm open: the source form remains open */\n'
        self.write('src/a.c', f'#ifdef DREAMCAST\n{tag}#pragma asm\n{code}{end}')
        self.assertEqual(run(tags.main, '--review', 'src/a.c'),
                         (0, f'src/a.c:2: the source form remains open\n{ONE_OPEN}\n'))

    def test_reviews_the_open_assembly_and_checks_nothing(self):
        named = '; @asm name: copyasm, the member that holds it; it starts at\n; 0x8c0c0d6c'
        self.write('src/b.src', SRC.replace(named, '; @asm open: every instruction is one SHC writes for C'))
        self.write('src/a.c', '/* @bug logic: stores 0x20000000 bytes */\n')
        self.assertEqual(run(tags.main, '--review', 'src/a.c', 'src/b.src'),
                         (0, f'src/b.src:6: every instruction is one SHC writes for C\n{ONE_OPEN}\n'))
        status, output = run(tags.main, 'src/a.c', 'src/b.src')
        self.assertEqual(status, 1)
        self.assertIn("src/a.c:1: 0x20000000 lies in no image's sections", output)

    def test_counts_each_kind(self):
        # b.src's function has a name tag, a.c's first block an opcode tag and its second an open one, and the @bug
        # tag is of no @asm kind
        self.write('src/b.src', SRC)
        tagged = f"/* @asm opcode: swap, SWAP.B at 0x8c010010 */\n{BLOCK}/* @asm open: nothing yet */\n"
        self.write('src/a.c', f"/* @bug logic: one */\n{tagged}{BLOCK.replace('_f', '_g')}")
        status, output = run(tags.main, '--review', 'src/a.c', 'src/b.src')
        self.assertEqual((status, output.splitlines()), (0, [
            'src/a.c:10: nothing yet',
            '@asm tags of inline bodies, #pragma asm blocks and .src functions by kind: 1 opcode, 0 operand, 0 abi, '
            '0 epilogue, 1 name and 1 open; 3 in all']))


def walk(*body, head='static void walk(Uint32 *line)', host='    walk(0);\n'):
    """a.c with one inline body, walk, and a C function, host. walk's head is
    head and its lines are body, where a line that ends in : is a label.
    host's statements are host. walk's #pragma inline_asm is line 4."""
    lines = ''.join(f'{line}\n' if line.endswith(':') else f'        {line}\n' for line in body)
    return ('/* clang-format off */\n#ifdef DREAMCAST\n/* @asm open: a walk */\n#pragma inline_asm(walk)\n'
            f'{head}\n{{\n{lines}}}\n#endif\n/* clang-format on */\n\nint host(void)\n{{\n{host}}}\n')


def end(c):
    """The line of the last of walk's lines in c, where --abi reads the RTS
    that tools/split.py writes after the body."""
    return line(c, '}\n#endif') - 1


# a .src file whose one function, tagged abi, opens at an unexported label
# and runs on through an exported one, and a datum after it
ABI_SRC = """\
          .EXPORT     _next
          .SECTION    P,CODE,ALIGN=4
; void next(void)
; @asm abi: stack-written, its RTS at 0x8c010004 returns
next_entry:
_next:
          RTS
          NOP
          .SECTION    D,DATA,ALIGN=4
; the table
_table:
          .DATA.L     0
          .END
"""


class Abi(TreeTest):
    """tags --abi, over a.c and a.src in a tree that holds no table."""

    def test_section_assembly_contract_only_applies_to_a_gnu_entry(self):
        tag = 'abi: stack-written, the entry stores on the caller stack at 0x8c534700'
        code = '        .SECTION P,CODE,ALIGN=32\n        .EXPORT _vertex\n'
        end = '_vertex:\n        MOV.L R0,@R15\n        RTS\n        NOP\n#pragma endasm\n'
        block = f'/* @asm {tag} */\n#pragma asm\n{code}{end}'
        self.passes('#ifdef NONMATCHING\n#else\n' + block + '#endif\n')
        self.passes('#ifdef DREAMCAST\n' + block + '#endif\n', 'vertex:abi')
        self.assertEqual(self.abi('#ifdef DREAMCAST\n' + block + '#endif\n'),
                         (1, ['a.c:3: vertex: its @asm tag is abi, and no --contract names vertex:abi']))

    def abi(self, c, *contracts, src=None):
        """tags --abi's status over a.c, which holds c, and a.src, which holds
        src where given, with each of contracts, and each problem it names,
        without the tool's name."""
        self.write('a.c', c)
        paths = ['a.c']
        if src is not None:
            self.write('a.src', src)
            paths.append('a.src')
        status, output = run(tags.main, '--abi', *(f'--contract={name}' for name in contracts), *paths)
        return status, [each.split(': ', 1)[1] for each in output.splitlines()]

    def passes(self, c, *contracts, src=None):
        self.assertEqual(self.abi(c, *contracts, src=src), (0, []))

    def unsafe(self, c, key, why):
        """Check that --abi fails on c's body, walk, for why, keyed key, among
        the body's findings. A contract for each of their keys passes the
        body, and each but walk:key leaves why failing."""
        status, problems = self.abi(c)
        self.assertEqual(status, 1, problems)
        self.assertIn(f'a.c:4: walk: {why}, and no --contract names walk:{key}', problems)
        keys = {re.search(r'names (walk:\w+)$', each)[1] for each in problems}
        self.passes(c, *keys)
        status, problems = self.abi(c, *keys - {f'walk:{key}'})
        self.assertEqual(problems, [f'a.c:4: walk: {why}, and no --contract names walk:{key}'])

    def refuses(self, c, problem):
        """Check that --abi can't read c's body, for problem, whatever a
        --contract names."""
        status, problems = self.abi(c, 'walk:R4')
        self.assertEqual(status, 1, problems)
        self.assertIn(f'a.c:{problem}', problems)

    def test_reads_no_table_and_checks_no_citation(self):
        # the tree holds no config/, which the plain run reads for the addresses a tag cites
        c = walk('OCBI @R4').replace('@asm open: a walk', '@asm opcode: cache-line, OCBI @R4 at 0x8c010000')
        self.passes(c)
        self.assertEqual(run(tags.main, 'a.c')[0], 1)

    def test_names_each_abi_function_no_contract_names(self):
        # the tag's function is the label directly below it, as C spells it
        status, output = self.abi(walk('NOP'), src=ABI_SRC)
        self.assertEqual((status, output), (1, ['a.src:5: next_entry: its @asm tag is abi, and no --contract '
                                                'names next_entry:abi']))
        self.passes(walk('NOP'), 'next_entry:abi', src=ABI_SRC)
        self.passes(walk('NOP'), 'next:abi', src=ABI_SRC.replace('next_entry:\n', ''))
        # and a body's, its function's, whatever the body holds
        tagged = walk('MOV.L @R4,R8').replace('@asm open: a walk', '@asm abi: stack-written, at 0x8c010000')
        self.assertEqual(self.abi(tagged), (1, ['a.c:4: walk: its @asm tag is abi, and no --contract names '
                                                'walk:abi']))
        self.passes(tagged, 'walk:abi')

    def test_names_each_contract_that_names_no_finding(self):
        # walk reads R12, and a contract for another register of it, or for another function, answers nothing
        c = walk('MOV.L @R4,R1', 'OR R12,R1', 'OCBI @R1')
        for contract in ('walk:R11', 'gone:R12'):
            with self.subTest(contract):
                status, problems = self.abi(c, 'walk:R12', contract)
                self.assertEqual((status, problems), (1, [f'{contract}: --contract names no finding of --abi']))

    def test_takes_a_contract_of_a_name_and_a_key_with_abi_alone(self):
        self.write('a.c', walk('NOP'))
        for argv, said in ((['--contract=walk:R4', 'a.c'], '--contract needs --abi'),
                           (['--abi', '--contract=walk', 'a.c'], '--contract walk: not NAME:KEY')):
            with self.subTest(argv):
                status, output = run(tags.main, *argv)
                self.assertEqual(status, 2)
                self.assertIn(said, output)

    def test_names_every_finding_of_a_body_and_a_contract_answers_its_key_alone(self):
        # a body that pushes R9 and stores over its slot, and writes out an FR5 no argument passes, as a 3D drawer
        # does: a contract for FR5 leaves R9's finding failing
        c = walk('MOV.L R9,@-R15', 'STS.L PR,@-R15', 'MOV.L R4,@(4,R15)', 'MOV.L #_f,R1', 'JSR @R1', 'NOP',
                 'FMOV.S FR5,@R0', 'LDS.L @R15+,PR', 'MOV.L @R15+,R9')
        status, problems = self.abi(c, 'walk:FR5')
        self.assertEqual((status, problems), (1, [f'a.c:4: walk: it returns at line {end(c)} with R9 changed, which '
                                                  'its GCC callers keep, and no --contract names walk:R9']))
        self.unsafe(c, 'FR5', f'it reads FR5 at line {line(c, "FMOV.S FR5")}, which no argument passes')

    def test_still_checks_each_tag_s_form(self):
        c = walk('NOP') + '/* @bug typo: a shipped bug */\n'
        self.assertEqual(self.abi(c), (1, [f'a.c:{line(c, "/* @bug")}: typo: not a kind of @bug; they are memory, '
                                           'uninitialized, logic, arithmetic, mismatched-call']))

    def test_names_a_body_that_reads_a_register_no_argument_passes(self):
        c = walk('MOV.L @R4,R1', 'OR R12,R1', 'OCBI @R1')
        self.unsafe(c, 'R12', f'it reads R12 at line {line(c, "OR R12")}, which no argument passes')
        # the one float argument takes FR4 alone. No argument passes FR5, T, or a register only one path sets
        c = walk('FMOV.S FR4,@R4', 'FMOV.S FR5,@R4', head='static void walk(Uint32 *line, Float scale)')
        self.unsafe(c, 'FR5', f'it reads FR5 at line {line(c, "FMOV.S FR5")}, which no argument passes')
        c = walk('BT ?done', 'NOP', '?done:')
        self.unsafe(c, 'T', f'it reads T at line {line(c, "BT")}, which no argument passes')
        c = walk('TST R4,R4', 'BT ?read', 'MOV #1,R1', '?read:', 'MOV.L R1,@R4')
        self.unsafe(c, 'R1', f'it reads R1 at line {line(c, "MOV.L R1")}, which no argument passes')
        # but a push only saves a register, and SR and the other bank are the machine's. FR0 names the other bank
        # after an FRCHG, and with SZ set, an FMOV of FR1 moves XD0, a pair of it
        self.passes(walk('MOV.L R12,@-R15', 'MOV.L @R15+,R12', 'STC SR,R0', 'FRCHG', 'FMOV.S FR0,@R4', 'FRCHG',
                         'FSCHG', 'FMOV FR1,@R4', 'FSCHG'))
        # R16 names no register: asmsh reads it as a symbol, here a load's label
        self.passes(walk('MOV.L R16,R1', 'MOV.L R1,@R4'))
        # a product writes MACL, which no operand names
        self.passes(walk('MOV.L @R4,R1', 'MUL.L R1,R1', 'STS MACL,R1', 'MOV.L R1,@R4'))
        c = walk('STS MACH,R1', 'MOV.L R1,@R4')
        self.unsafe(c, 'MACH', f'it reads MACH at line {line(c, "STS MACH")}, which no argument passes')

    def test_names_a_body_that_reads_a_register_a_call_can_change(self):
        c = walk('MOV R4,R6', 'MOV.L #_f,R1', 'STS.L PR,@-R15', 'JSR @R1', 'NOP', 'LDS.L @R15+,PR', 'MOV.L R0,@R6')
        self.unsafe(c, 'R6', f'it reads R6 at line {line(c, "MOV.L R0")}, which the call at line {line(c, "JSR")} can '
                       'change')
        # the RTS that tools/split.py writes after the body reads PR
        lost = walk('MOV.L #_f,R1', 'JSR @R1', 'NOP')
        self.unsafe(lost, 'PR', f'its return reads PR, which the call at line {line(lost, "JSR")} can change')
        # but a register the callee keeps still holds the body's value after the call
        self.passes(c.replace('MOV R4,R6', 'MOV.L R8,@-R15\n        MOV R4,R8').replace(
            'MOV.L R0,@R6', 'MOV.L R0,@R8\n        MOV.L @R15+,R8'))

    def test_names_a_return_whose_pr_holds_no_return_address(self):
        # PR set from an argument, or popped from a push in a call's slot, which runs once the call has written PR
        c = walk('LDS R4,PR')
        self.unsafe(c, 'PR', f'its return reads PR, which line {line(c, "LDS")} writes')
        c = walk('MOV.L #_f,R1', 'JSR @R1', 'STS.L PR,@-R15', 'LDS.L @R15+,PR')
        self.unsafe(c, 'PR', f'its return reads PR, which line {line(c, "LDS")} writes')
        # and the body's own return reads it too
        c = walk('LDS R4,PR', 'RTS', 'NOP', host='    return walk(0);\n')
        self.unsafe(c, 'PR', f'it reads PR at line {line(c, "RTS")}, which line {line(c, "LDS")} writes')
        # but a push before the call saves it, at an offset a known R0 gives too
        self.passes(walk('STS.L PR,@-R15', 'MOV.L #_f,R1', 'JSR @R1', 'NOP', 'LDS.L @R15+,PR'))
        self.passes(walk('STS.L PR,@-R15', 'ADD #-8,R15', 'MOV #4,R0', 'MOV.L R4,@(R0,R15)', 'MOV.L #_f,R1',
                         'JSR @R1', 'NOP', 'ADD #8,R15', 'LDS.L @R15+,PR'))

    def test_names_a_body_that_returns_with_a_kept_register_changed(self):
        c = walk('MOV.L @R4,R8')
        self.unsafe(c, 'R8', f'it returns at line {end(c)} with R8 changed, which its GCC callers keep')
        # or popped from a slot that doesn't hold its whole value at entry. These slots hold R9, a word of R8, R8 once
        # changed, and R8 where one path stores over it through R15, at @R15 or at its offset
        for body in (('MOV.L R9,@-R15', 'MOV #0,R8', 'MOV.L @R15+,R8'),
                     ('MOV.W R8,@-R15', 'MOV #0,R8', 'MOV.W @R15+,R8'),
                     ('MOV #0,R8', 'MOV.L R8,@-R15', 'MOV.L @R15+,R8'),
                     ('MOV.L R8,@-R15', 'TST R4,R4', 'BT ?pop', 'MOV.L R4,@R15', '?pop:', 'MOV.L @R15+,R8'),
                     ('MOV.L R8,@-R15', 'ADD #-4,R15', 'MOV.L R4,@(4,R15)', 'ADD #4,R15', 'MOV.L @R15+,R8'),
                     ('MOV.L R8,@-R15', 'ADD #-4,R15', 'MOV #4,R0', 'MOV.L R4,@(R0,R15)', 'ADD #4,R15',
                      'MOV.L @R15+,R8')):
            with self.subTest(body=body):
                c = walk(*body)
                self.unsafe(c, 'R8', f'it returns at line {end(c)} with R8 changed, which its GCC callers keep')
        # R15 too, moved by a push, an ADD, or a copy or a pop into it. With SZ set, an FMOV.S pushes a pair. Each
        # spells R15 as R15 or SP, in any case
        for body in (('MOV.L R4,@-R15',), ('ADD #-4,R15',), ('MOV R4,R15',), ('MOV.L R4,@-R15', 'MOV.L @R15+,R15'),
                     ('FSCHG', 'FMOV.S FR0,@-R15', 'FSCHG', 'MOV.L @R15+,R0'), ('ADD #-4,SP',), ('MOV.L R4,@-sp',)):
            with self.subTest(body=body):
                c = walk(*body)
                self.unsafe(c, 'R15', f'it returns at line {end(c)} with R15 changed, which its GCC callers keep')
        # a pop past the body's entry
        c = walk('MOV.L @R15+,R1', 'NOP')
        self.unsafe(c, 'frame', f"it takes R15 above its entry at line {line(c, 'MOV.L')}, into its caller's frame")
        # and FPSCR's FR and SZ bits, at a return or a call
        for bit, swap in (('FR', 'FRCHG'), ('SZ', 'FSCHG')):
            with self.subTest(bit=bit):
                c = walk(swap)
                self.unsafe(c, bit, f"it returns at line {end(c)} with FPSCR's {bit} bit set, which GCC's code "
                                    'takes clear')
                c = walk('MOV.L #_f,R1', swap, 'STS.L PR,@-R15', 'JSR @R1', 'NOP', 'LDS.L @R15+,PR', swap)
                self.unsafe(c, bit, f"its call at line {line(c, 'JSR')} runs with FPSCR's {bit} bit set, which GCC's code "
                               'takes clear')
        # but not where the body puts each back, and a store to a slot of its own leaves the pushed ones as they were
        self.passes(walk('ADD #-8,R15', 'MOV.L R4,@R15', "ADD #H'8,R15", 'FRCHG', 'FRCHG'))
        self.passes(walk('MOV.L R8,@-R15', 'ADD #-4,R15', 'MOV #0,R8', 'ADD #4,R15', 'MOV.L @R15+,R8'))
        self.passes(walk('MOV.L R8,@-R15', 'ADD #-8,R15', 'MOV.L R4,@R15', 'MOV.L R4,@(4,R15)', 'MOV #0,R8',
                         'ADD #8,R15', 'MOV.L @R15+,R8'))
        self.passes(walk('FSCHG', 'FMOV.S FR0,@-R15', 'FSCHG', 'ADD #8,R15'))
        self.passes(walk('ADD #-8,SP', 'MOV.L R4,@(4,SP)', 'ADD #8,R15'))

    def test_names_a_body_that_may_store_into_its_callers_frame(self):
        # with no frame of its own, a store at @R15, at an offset from it, or at an R0 it doesn't know
        for body, store in ((('MOV.L R4,@R15',), 'MOV.L R4,@R15'), (('MOV.L R4,@(4,R15)',), 'MOV.L R4,@(4,R15)'),
                            (('MOV R5,R0', 'MOV.L R4,@(R0,R15)'), 'MOV.L R4,@(R0'),
                            (('ADD #-4,R15', 'MOV.L R4,@(4,R15)', 'ADD #4,R15'), 'MOV.L R4,@(4'),
                            (('ADD #-8,R15', 'MOV #4,R0', 'ADD #4,R0', 'FMOV.S FR4,@(R0,R15)', 'ADD #8,R15'),
                             'FMOV.S FR4')):
            with self.subTest(body=body):
                c = walk(*body, head='static void walk(Uint32 *line, Uint32 n, Float x)')
                self.unsafe(c, 'frame', f"it may store into its caller's frame at line {line(c, store)}")
        # or through an R15 it no longer knows against its entry, though a contract answers R15's return
        for body in (('MOV R4,SP', 'MOV.L R5,@SP'), ('MOV R4,R15', 'MOV.L R5,@-R15')):
            with self.subTest(body=body):
                c = walk(*body, head='static void walk(Uint32 *line, Uint32 n)')
                self.unsafe(c, 'frame', f"it may store into its caller's frame at line {line(c, 'MOV.L R5')}")
        # but not inside a frame of its own, at a known R0, as njLtSrcs3.c's light loop stores
        self.passes(walk('ADD #-12,R15', 'MOV #4,R0', 'MOV.L R4,@R15', 'MOV.L R4,@(R0,R15)', 'ADD #4,R0',
                         'MOV.L R4,@(R0,R15)', 'ADD #12,R15'))

    def test_reads_an_immediate_of_a_mov_or_an_add_as_a_signed_byte(self):
        # #H'FC adds -4, as the SH-4 sign-extends the byte, to R15 or to R0
        self.passes(walk("ADD #H'FC,R15", 'ADD #4,R15'))
        self.passes(walk('ADD #-8,R15', "MOV #H'FC,R0", 'ADD #8,R0', 'MOV.L R4,@(R0,R15)', 'ADD #8,R15'))

    def test_names_a_body_that_leaves_where_its_host_goes_on(self):
        on = '    walk(0);\n    return 1;\n'
        c = walk('TST R4,R4', 'BF ?push', 'RTS', 'MOV #0,R0', '?push:', 'MOV #1,R0', host=on)
        self.unsafe(c, 'leaves', f'its RTS at line {line(c, "RTS")} leaves it, and host goes on past its call')
        out = walk('BRA _elsewhere', 'NOP', host=on)
        self.unsafe(out, 'leaves', f'its BRA _elsewhere at line {line(out, "BRA")} leaves it, and host goes on past its call')
        # a jump to the body's own P2 alias, a BRAF and an RTE
        for body, statement in (
                (('MOVA ?UNCACHED,R0', 'MOV.L ?P2_BASE,R1', 'OR R0,R1', 'JMP @R1', 'NOP', '.ALIGN 4', '?P2_BASE:',
                  ".DATA.L H'A0000000", '?UNCACHED:'), 'JMP @R1'),
                (('BRAF R4', 'NOP'), 'BRAF R4'), (('RTE', 'NOP'), 'RTE')):
            with self.subTest(statement=statement):
                jump = walk(*body, host=on)
                self.unsafe(jump, 'leaves', f'its {statement} at line {line(jump, statement)} leaves it, and host goes on past '
                                  'its call')
        # through a macro too, function-like or object-like, and where a direct call ends the host
        for host, macro in (('    WALK();\n', '#define WALK() walk(0)\n'),
                            ('    RUN;\n    next();\n', '#define RUN walk(0)\n'),
                            ('    WALK();\n    walk(0);\n', '#define WALK() walk(0)\n')):
            with self.subTest(host):
                self.unsafe(c.replace(on, host) + macro, 'leaves',
                            f'its RTS at line {line(c, "RTS")} leaves it, and host goes on past its call')
        # and where the host does more with the call's value than return it
        for host in ('    return walk(0) + 1;\n', '    consume(walk(0));\n'):
            with self.subTest(host):
                self.unsafe(c.replace(on, host), 'leaves',
                            f'its RTS at line {line(c, "RTS")} leaves it, and host goes on past its call')
        # and where the non-matching side of a guard goes on, though the matching side returns the call's value
        guarded = c.replace(f'int host(void)\n{{\n{on}}}\n', f'#ifdef NONMATCHING\nint host(void)\n{{\n{on}}}\n#else\n'
                            'int host(void)\n{\n    return walk(0);\n}\n#endif\n')
        self.unsafe(guarded, 'leaves', f'its RTS at line {line(c, "RTS")} leaves it, and host goes on past its call')
        # but not where its host returns the call's value or ends on the call, where it calls the body by its P2
        # address, or where only the matching side of a guard goes on
        for host in ('    return walk(0);\n', '    if (1) {\n        return walk(0);\n    }\n    walk(0);\n',
                     '    ((void (*)(Uint32 *))((Uint32)walk | 0xa0000000))(0);\n    return 1;\n',
                     f'#ifdef NONMATCHING\n    return walk(0);\n#else\n{on}#endif\n'):
            with self.subTest(host):
                self.passes(c.replace(on, host))

    def test_reads_a_slot_a_label_marks_on_both_of_its_paths(self):
        # the slot runs as the BRA's, then at ?loop, and below ?done, then off the body's end
        self.passes(walk('?loop:', 'DT R4', 'BT ?done', 'BRA ?loop', '?done:', 'MOV R4,R0'))
        # and a read there is one on either path
        c = walk('TST R4,R4', 'BT ?done', 'MOV #1,R1', 'BRA ?done', '?done:', 'MOV.L R1,@R4')
        self.unsafe(c, 'R1', f'it reads R1 at line {line(c, "MOV.L R1")}, which no argument passes')

    def test_fails_on_a_body_it_can_t_read(self):
        # a call to a label of the body, and a write to FPSCR, among them
        for statement, shown in (('MAC.L @R4+,@R5+', 'MAC.L @R4+,@R5+'), ('BSRF R4', 'BSRF R4'),
                                 ('BSR ?self', 'BSR ?self'), ('LDS R4,FPSCR', 'LDS R4,FPSCR')):
            with self.subTest(statement=statement):
                c = walk('?self:', statement, 'NOP')
                self.refuses(c, f"{line(c, statement)}: walk: {shown}, which --abi doesn't read")
        # a delayed branch whose slot it can't place: a return or a call that ends the body, a slot under a directive,
        # and a slot that branches or returns
        for body, statement in ((('NOP', 'RTS'), 'RTS'), (('MOV.L #_f,R1', 'JSR @R1'), 'JSR @R1'),
                                (('BRA ?next', '.ALIGN 4', 'NOP', '?next:', 'NOP'), 'BRA ?next'),
                                (('BRA ?next', 'BT ?next', '?next:', 'NOP'), 'BRA ?next'),
                                (('BRA ?next', 'RTS', 'NOP', '?next:', 'NOP'), 'BRA ?next')):
            with self.subTest(body=body):
                c = walk(*body)
                self.refuses(c, f"{line(c, statement)}: walk: {statement}, whose slot --abi can't place")
        # a directive that places bytes where a path runs into it, below an instruction, at the entry or at a branch's
        # label, though not a pool a branch passes
        for body, statement in (((".DATA.W H'E800'",), ".DATA.W H'E800'"),
                                (('MOV R4,R0', ".DATA.W H'E800'"), ".DATA.W H'E800'"),
                                (('BRA ?pool', 'NOP', '?pool:', '.DATA.L 0'), '.DATA.L 0')):
            with self.subTest(body=body):
                c = walk(*body)
                self.refuses(c, f"{line(c, statement)}: walk: {statement}, which a path of --abi's runs as code")
        self.passes(walk('BRA ?on', 'NOP', '.ALIGN 4', '?pool:', ".DATA.L H'A0000000", '?on:', 'MOV.L @R4,R1'))
        # one no path reaches
        c = walk('RTS', 'NOP', 'MOV #1,R1')
        self.refuses(c, f"{line(c, 'MOV #1')}: walk: MOV #1,R1, which no path of --abi's reaches")
        # and where two paths meet with FR, SZ or R15 apart
        for apart in ('FSCHG', 'ADD #-4,R15'):
            with self.subTest(apart=apart):
                c = walk('TST R4,R4', 'BT ?done', apart, '?done:', 'NOP')
                self.refuses(c, f"{line(c, '?done:') + 1}: walk: two paths meet here with FPSCR's FR or SZ bits or "
                                "R15 apart, which --abi can't read")

    def test_reads_a_tab_as_a_blank(self):
        lines = ['?top:        MOV.L @R4,R1 ; a load', '        BRA ?top', '        NOP']
        self.assertEqual(tags.instructions([each.replace(' ', '\t') for each in lines]), tags.instructions(lines))

    def test_fails_on_a_parameter_it_can_t_place(self):
        # a float typedef it doesn't know, a long long and a variadic tail, whatever --contract names
        for head, parameter in (('Uint32 *line, KMFLOAT scale', 'KMFLOAT scale'),
                                ('Uint32 *line, long long n', 'long long n'), ('Uint32 *line, ...', '...')):
            with self.subTest(head=head):
                self.refuses(walk('OCBI @R4', head=f'static void walk({head})'),
                             f"5: walk: its parameter {parameter}, which --abi can't place")
        # but places floats and integers as C and the SDK spell them, in any order, and a pointer past R7 on the stack
        self.passes(walk('FMOV.S FR4,@R4', 'FMOV.S FR5,@R4', 'MOV.L R5,@R4',
                         head='static void walk(Uint32 *line, const Float a, double b, Sint32 n)'))
        self.passes(walk('MOV.L @R15,R1', 'MOV.L R7,@R1',
                         head='static void walk(Uint32 *line, Int a, Int b, Int c, Uint32 *out)'))
        # a comment is a blank, so its * makes no pointer
        self.assertEqual(tags.arguments('void f(float x /* radians */)', 'f'), (['FA4'], []))
        self.assertEqual(tags.arguments('void f(KMFLOAT x /* radians */)', 'f'), ([], ['KMFLOAT x']))

    def test_reads_no_body_on_the_matching_side_of_a_guard(self):
        c = walk('MOV.L @R4,R1', 'OR R12,R1', 'OCBI @R1')
        guarded = c.replace('#ifdef DREAMCAST\n', '#ifdef DREAMCAST\n#ifdef NONMATCHING\n#else\n').replace(
            '#endif\n', '#endif\n#endif\n')
        self.passes(guarded)
        self.assertEqual(self.abi(guarded, 'walk:R12'), (1, ['walk:R12: --contract names no finding of --abi']))

    def test_reads_bodies_only_under_abi(self):
        # the plain run and --review check tags alone
        self.write('a.c', walk('MAC.L @R4+,@R5+', 'NOP').replace('@asm open: a walk', '@asm open: nothing yet'))
        self.assertEqual(run(tags.main, '--review', 'a.c'), (0, f'a.c:3: nothing yet\n{ONE_OPEN}\n'))
