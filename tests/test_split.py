"""The split of a C source's inline bodies from its C, for make dc."""

import unittest
from pathlib import Path

import split
from support import TreeTest, run

ROOT = Path(__file__).resolve().parents[1]

# a source of two bodies, a static one and an exported one, each on GCC's
# side of its guard, between C of its own
SOURCE = '''\
#include "a.h"

#ifdef DREAMCAST
/* @asm open: every instruction is one SHC writes for C */
#pragma inline_asm(twice)
static int twice(int x)
{
        ADD         R4,R4
?done:
        RTS
        MOV         R4,R0
}

/* @asm open: every instruction is one SHC writes for C */
#pragma inline_asm(thrice(size=8))
int thrice(int x) {
        BRA         ?done
        NOP
?done:
}
#endif

int six(int x) { return thrice(twice(x)); }
'''
# a body that a guard keeps for SHC, beside C GCC compiles in its place
SHCS = '''\
#ifdef NONMATCHING
int twice(int x) { return x + x; }
#else
#pragma inline_asm(twice)
int twice(int x)
{
        RTS
        ADD         R4,R4
}
#endif
'''


def body(text):
    """A source of one body, named twice, whose lines are text."""
    return f'#pragma inline_asm(twice)\nint twice(int x)\n{{\n{text}}}\n'


class Split(TreeTest):
    def test_a_section_with_its_own_public_entries_keeps_its_assembly(self):
        rows = ['        .SECTION P,CODE,ALIGN=32', '        .EXPORT _vertex', '_vertex:',
                '        RTS', '        NOP']
        block = '#pragma asm\n' + '\n'.join(rows) + '\n#pragma endasm\n'
        source = 'void vertex(void);\n#ifdef DREAMCAST\n' + block + '#endif\n'
        c, asm = split.split('src/a.c', source, 'build/dc/src/a_inline.s')
        self.assertEqual(asm, rows)
        self.assertNotIn('_vertex:', c)
        self.assertIn('":"(vertex)', c[-1])
        for text in [block.replace('        RTS', '        BRA .+4'), 'void caller(void)\n{\n' + block + '}\n',
                     block.replace('_vertex:', '_unexported:')]:
            with self.subTest(source=text):
                self.assertIn('src/a.c:', self.fails(split.split, 'src/a.c', text, 'build/dc/src/a_inline.s'))
        self.assertEqual(split.split('src/a.c', '/*\n' + block + '*/\n', 'build/dc/src/a_inline.s')[1], [])

    def test_a_public_sibling_can_branch_to_a_private_assembly_entry(self):
        private = '#pragma asm\n.SECTION P,CODE,ALIGN=32\n_helper:\nRTS\nNOP\n#pragma endasm\n'
        public = '#pragma asm\n.SECTION P,CODE,ALIGN=32\n.EXPORT _run\n_run:\nBRA _helper\nNOP\n#pragma endasm\n'
        source = 'void run(void);\n' + private + 'void empty(void) {}\n' + public
        c, assembly = split.split('src/a.c', source, 'build/dc/src/a_inline.s')
        self.assertIn('_helper:', assembly)
        self.assertIn('BRA _helper', assembly)
        self.assertEqual(c[-1], '        : : ":"(run));')
        for rejected in (source.replace('BRA _helper', 'BRA _other'),
                         source.replace('RTS\nNOP', 'ADD #4,R0\nNOP'),
                         source.replace('_helper:', '.EXPORT _missing\n_helper:'),
                         source.replace(public, '#ifdef NONMATCHING\n#else\n' + public + '#endif\n')):
            with self.subTest(source=rejected):
                self.assertIn('src/a.c:', self.fails(split.split, 'src/a.c', rejected, 'build/dc/src/a_inline.s'))

    def test_drops_end_only_at_the_end_of_the_native_module(self):
        rows = ['        .SECTION P,CODE,ALIGN=32', '        .EXPORT _last', '_last:', '        RTS', '        NOP']
        block = '#pragma asm\n' + '\n'.join(rows) + '\n        .END ; after the delay slot\n#pragma endasm\n'
        source = 'void last(void);\n#ifdef DREAMCAST\n' + block + '/* done */\n#endif\n'
        _, asm = split.split('src/a.c', source, 'build/dc/src/a_inline.s')
        self.assertEqual(asm, rows)
        for later in ('int another;\n', '#include "more.h"\n', block, '#define MORE 1\n'):
            with self.subTest(later=later):
                self.assertIn("#pragma asm, whose block the split doesn't take",
                              self.fails(split.split, 'src/a.c', source + later, 'build/dc/src/a_inline.s'))
        for bad in (block.replace('        NOP', '        .END\n        NOP'), block.replace('.END ;', '.END _last ;')):
            with self.subTest(body=bad):
                self.assertIn("#pragma asm, whose block the split doesn't take",
                              self.fails(split.split, 'src/a.c', bad, 'build/dc/src/a_inline.s'))

    def test_blanks_each_body_and_includes_its_text_by_a_file_scope_asm(self):
        c, text = split.split('src/a.c', SOURCE, 'build/dc/src/a_inline.s')
        lines = SOURCE.split('\n')
        self.assertEqual(c[0], '#line 1 "src/a.c"')
        # line for line: the pragmas, the bodies and their closing braces go, and each opening brace is a ;
        self.assertEqual(len(c), len(lines) + 2)
        self.assertEqual(c[1:len(lines)], ['#include "a.h"', '', '#ifdef DREAMCAST',
                                               '/* @asm open: every instruction is one SHC writes for C */', '',
                                               'static int twice(int x)', ';', '', '', '', '', '', '',
                                               '/* @asm open: every instruction is one SHC writes for C */', '',
                                               'int thrice(int x) ;', '', '', '', '', '#endif', '',
                                               'int six(int x) { return thrice(twice(x)); }'])
        self.assertEqual(c[-2:], ['__asm__(".pushsection .text\\n.include \\"build/dc/src/a_inline.s\\"'
                                  '\\n.popsection"', '        : : ":"(twice), ":"(thrice));'])
        # each body as SHC writes one out, the static one without its .EXPORT
        self.assertEqual(text, ['; src/a.c:5: twice', '_twice:', '        ADD         R4,R4', '?done:',
                                '        RTS', '        MOV         R4,R0', '        .ALIGN      4', '        RTS',
                                '        NOP',
                                '; src/a.c:15: thrice', '        .EXPORT     _thrice', '_thrice:',
                                '        BRA         ?done', '        NOP', '?done:', '        .ALIGN      4',
                                '        RTS', '        NOP'])

    def test_leaves_a_body_on_the_matching_side_to_shc(self):
        c, text = split.split('src/a.c', SHCS, 'build/dc/src/a_inline.s')
        self.assertEqual(text, [])
        self.assertEqual(c[1:3], ['#ifdef NONMATCHING', 'int twice(int x) { return x + x; }'])
        self.assertEqual([line for line in c[4:] if line], ['int twice(int x)', ';', '#endif'])

    def test_leaves_matching_only_assembly_reservations_inside_their_guard(self):
        source = ('#ifdef NONMATCHING\n#else\n#pragma asm\n'
                  '        .RES.B      4096\n#pragma endasm\n#endif\n')
        c, text = split.split('src/a.c', source + SOURCE, 'build/dc/src/a_inline.s')
        self.assertEqual(c[1:7], source.splitlines())
        self.assertNotIn('.RES.B', '\n'.join(text))
        self.assertIn('_twice:', text)

    def test_preserves_a_guard_within_the_c_parameter_list(self):
        head = ('static int\n#ifdef NONMATCHING\ntwice(int x)\n'
                '#else\ntwice(void)\n#endif\n')
        source = '#pragma inline_asm(twice)\n' + head + '{\n        MOV R4,R0\n}\n'
        c, assembly = split.split('src/a.c', source, 'build/dc/src/a_inline.s')
        self.assertIn(head + ';', '\n'.join(c))
        self.assertIn('_twice:', assembly)
        self.assertIn('        MOV R4,R0', assembly)
        self.assertFalse(any('#' in line for line in assembly))

    def test_refuses_what_it_cant_carry_and_names_each_line(self):
        cases = [
            ('#pragma inline_asm(twice)\nint twice(int x);\n', 'src/a.c:1: not #pragma inline_asm(<function>)'),
            ('#pragma inline_asm(twice)\nint thrice(int x)\n{\n        RTS\n        NOP\n}\n',
             'src/a.c:1: #pragma inline_asm(twice), with another function below it'),
            ('#pragma asm\n        NOP\n#pragma endasm\n', "src/a.c:1: #pragma asm, whose block the split doesn't take"),
            ('#if OPTION\n#pragma asm\n        NOP\n#pragma endasm\n#endif\n',
             "src/a.c:2: #pragma asm, whose block the split doesn't take"),
            ('#pragma inline_asm(twice)\nstatic int twice(int x)\n#ifdef NONMATCHING\n'
             '{\n        MOV R4,R0\n}\n#endif\n', 'src/a.c:3: twice: a conditional in the declaration crosses'),
            (body('#ifdef DREAMCAST\n        RTS\n#endif\n'), 'src/a.c:4: twice: a directive inside the body'),
            ('#if 1\n' + body('        RTS\n        NOP\n') + '#endif\n',
             "src/a.c:2: twice: a body under a conditional the tool can't read"),
            (body('        .SECTION    D,DATA,ALIGN=4\n        RTS\n        NOP\n'), 'src/a.c:4: twice: a .SECTION'),
            (body('        .END\n'), 'src/a.c:4: twice: the .END directive would cut off'),
            (body('?stop: .END _twice\n'), 'src/a.c:4: twice: the .END directive would cut off'),
            (body('        BRA         _thrice+4\n        NOP\n'),
             'src/a.c:4: twice: BRA         _thrice+4, a branch to other than a label'),
        ]
        for source, refused in cases:
            with self.subTest(refused):
                self.assertIn(refused, self.fails(split.split, 'src/a.c', source, 'build/dc/src/a_inline.s'))

    def test_reads_a_branch_to_a_label_and_a_jump_through_a_register(self):
        _, text = split.split('src/a.c', body('        BT/S        ?out\n        NOP\n        JMP         @R1\n'
                                              '        NOP\n?out:\n'), 'build/dc/src/a_inline.s')
        self.assertIn('        BT/S        ?out', text)

    def test_writes_the_c_and_the_text_with_the_sources_bytes(self):
        self.write('src/a.c', SOURCE.replace('/* @asm open', '/* \x82\xa0 @asm open').encode('latin-1'))
        status, output = run(split.main, 'src/a.c', 'build/dc/src/a.c', 'build/dc/src/a_inline.src')
        self.assertEqual((status, output), (0, ''))
        self.assertIn(b'/* \x82\xa0 @asm open', Path('build/dc/src/a.c').read_bytes())
        self.assertTrue(Path('build/dc/src/a_inline.src').read_text().startswith('; src/a.c:5: twice\n_twice:\n'))

    def test_refuses_a_path_it_cant_write_unescaped_and_writes_nothing(self):
        self.write('src/a b.c', SOURCE)
        status, output = run(split.main, 'src/a b.c', 'build/a.c', 'build/a_inline.src')
        self.assertEqual(status, 1)
        self.assertIn("src/a b.c: holds other than letters, digits, _, ., / and -", output)
        status, output = run(split.main, 'src/a.c', 'build/a.c', 'build/a"_inline.src')
        self.assertEqual(status, 1)
        self.assertIn('build/a"_inline.s: holds other than', output)

    def test_keeps_a_source_line_for_line_through_its_last_lf(self):
        for source in ('int x;\n', 'int x;\n\n'):
            with self.subTest(source=source):
                c, _ = split.split('src/a.c', source, 'build/dc/src/a_inline.s')
                self.assertEqual(''.join(f'{line}\n' for line in c[1:]), source)

    def test_keeps_a_comment_whose_shift_jis_holds_0x85_on_its_line(self):
        rows = ['        .SECTION P,CODE,ALIGN=32', '        .EXPORT _f', '_f:', '        RTS ; \x83\x85 the end',
                '        NOP']
        source = 'void f(void);\n#pragma asm\n' + '\n'.join(rows) + '\n#pragma endasm\nint after;\n'
        c, asm = split.split('src/a.c', source, 'build/dc/src/a_inline.s')
        self.assertEqual(asm, rows)
        self.assertIn('int after;', c)

    def test_reads_a_pragma_by_its_name_not_its_spelling(self):
        block = '        .SECTION P,CODE,ALIGN=32\n        .EXPORT _f\n_f:\n        RTS\n        NOP\n#pragma endasm\n'
        _, asm = split.split('src/a.c', 'void f(void);\n  # pragma asm\n' + block, 'build/dc/src/a_inline.s')
        self.assertIn('_f:', asm)
        self.assertIn("src/a.c:1: #pragma asm, whose block the split doesn't take",
                      self.fails(split.split, 'src/a.c', '# pragma asm\n        NOP\n#pragma endasm\n',
                                 'build/dc/src/a_inline.s'))
        self.assertIn('src/a.c:1: not #pragma inline_asm(<function>)',
                      self.fails(split.split, 'src/a.c', body('        RTS\n        NOP\n').replace('#pragma', '# pragma'),
                                 'build/dc/src/a_inline.s'))
        for source in ('/*\n#pragma inline_asm(twice)\n*/\nint x;\n', '#pragma asmx\nint x;\n'):
            with self.subTest(source=source):
                self.assertEqual(split.split('src/a.c', source, 'build/dc/src/a_inline.s')[1], [])

    def test_refuses_a_pragma_that_names_other_than_the_function_its_head_declares(self):
        self.assertIn('src/a.c:1: #pragma inline_asm(twice), with another function below it',
                      self.fails(split.split, 'src/a.c', body('        RTS\n        NOP\n').replace(
                          'int twice(int x)', 'int thrice(int twice(void))'), 'build/dc/src/a_inline.s'))

    def test_refuses_an_export_beside_one_the_block_defines(self):
        rows = '        .SECTION P,CODE,ALIGN=32\n        .EXPORT _f\n        .EXPORT other\n_f:\n        RTS\n        NOP\n'
        self.assertIn("src/a.c:2: #pragma asm, whose block the split doesn't take",
                      self.fails(split.split, 'src/a.c', 'void f(void);\n#pragma asm\n' + rows + '#pragma endasm\n',
                                 'build/dc/src/a_inline.s'))

    def test_drops_a_terminal_end_that_comments_follow_and_keeps_them(self):
        rows = ['        .SECTION P,CODE,ALIGN=32', '        .EXPORT _f', '_f:', '        RTS', '        NOP']
        block = '#pragma asm\n' + '\n'.join(rows) + '\n        .END\n\n; the module ends\n#pragma endasm\n'
        _, asm = split.split('src/a.c', 'void f(void);\n' + block, 'build/dc/src/a_inline.s')
        self.assertEqual(asm, [*rows, '', '; the module ends'])
        self.assertIn("#pragma asm, whose block the split doesn't take",
                      self.fails(split.split, 'src/a.c', 'void f(void);\n' + block.replace('; the module', 'NOP ; the'),
                                 'build/dc/src/a_inline.s'))

    def test_refuses_paths_that_name_one_file_and_writes_nothing(self):
        self.write('src/a.c', SOURCE)
        for out, text in (('src/a.c', 'build/a_inline.src'), ('build/a.c', './src/a.c'), ('build/a.c', 'build/./a.c')):
            with self.subTest(out=out, text=text):
                status, output = run(split.main, 'src/a.c', out, text)
                self.assertEqual(status, 1)
                self.assertIn('name one file, which a save would overwrite', output)
                self.assertEqual(Path('src/a.c').read_text(), SOURCE)
                self.assertFalse(Path('build').exists())


class Tree(unittest.TestCase):
    def test_splits_each_source_of_the_tree_that_holds_a_body(self):
        # a source whose bodies all stand on the matching side of a guard gives no text
        for path in sorted(path for top in ('src', 'sdk', 'modules') for path in (ROOT / top).rglob('*.c')):
            text = path.read_text('latin-1')
            if '#pragma inline_asm' in text:
                with self.subTest(path=path.relative_to(ROOT)):
                    c, assembly = split.split(str(path.relative_to(ROOT)), text, 'build/dc/a_inline.s')
                    self.assertEqual(bool(assembly), any('.include' in line for line in c))
