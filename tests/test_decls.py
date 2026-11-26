"""What a source declares, the outside names' declarations, and the overlays'
headers src/ includes."""

import os
from pathlib import Path

import decls
from csource import parse
from layout import Types
from support import TreeTest, run

# a source that declares and defines at file scope in each form the scan
# reads, with noise that holds what would read as a declaration
SOURCE = """\
#include "a.h"
#define DECLARE(name) int name(void);
struct task;
typedef int counter;
static int hidden(void);
extern int seen, *pointed, held[4];
extern void (*handlers[])(struct task *task);
void (*current_call(void))(struct task *task);
struct task *task_create(void (*step)(struct task *task), const char *name);
extern int initialized = 1;
int tentative;
struct pair { int a; int b; } pair;
enum { FIRST, SECOND };
/* extern int commented; */
static const char text[] = "extern int quoted;";
int own(void);
int own(void)
{
    extern int inner;
    return inner;
}
struct task *made(void)
{
    return 0;
}
void after(void);
"""


class Scan(TreeTest):
    def test_reads_what_a_file_declares_and_defines_at_file_scope(self):
        declared, defined = decls.scan(SOURCE)
        self.assertEqual(declared, {'seen': 6, 'pointed': 6, 'held': 6, 'handlers': 7, 'current_call': 8,
                                    'task_create': 9, 'own': 16, 'after': 26})
        self.assertEqual(defined, {'initialized': 10, 'tentative': 11, 'pair': 12, 'own': 17, 'made': 22})

    def test_reads_an_inline_body_as_a_definition(self):
        text = ('#pragma inline_asm(twice)\nint twice(int x)\n{\n        ADD R4,R4 ; it\'s doubled\n}\n'
                'int after(void);\n')
        self.assertEqual(decls.scan(text), ({'after': 6}, {'twice': 2}))

    def test_reads_storage_from_the_specifiers_and_a_name_through_its_parentheses(self):
        text = ('int (step)(void);\nvoid use(int a[static 1]);\nint typedef Fn(void);\nstatic int hidden(void);\n'
                'int (*pointer)(void);\n')
        self.assertEqual(decls.scan(text), ({'step': 1, 'use': 2}, {'pointer': 5}))


class Repeats(TreeTest):
    def test_raw_assembly_leaves_real_declarations_on_both_sides(self):
        files = {'a.h': 'void before(void);\nvoid from_assembly(void);\nvoid after(void);\n',
                 'a.c': 'void before(void);\n# pragma asm\n'
                        '        .SECTION P,CODE,ALIGN=4\n; void from_assembly(void);\n'
                        '_entry:\n        RTS ; {\n        NOP\n# pragma endasm\nvoid after(void);\n'}
        self.assertEqual(decls.repeats(files), {'before': ['a.h:1', 'a.c:1'], 'after': ['a.h:3', 'a.c:9']})

    def test_commented_assembly_directives_leave_c_visible(self):
        text = '/*\n#pragma asm\n*/\nvoid between(void);\n/*\n#pragma endasm\n*/\n'
        self.assertEqual(decls.scan(text), ({'between': 4}, {}))

    def test_a_spliced_endasm_directive_closes_the_assembly(self):
        text = '#pragma asm\n; int bogus(void);\n#pragma end\\\nasm\nextern int after;\n'
        self.assertEqual(decls.scan(text), ({'after': 5}, {}))

    def test_two_sources_declaring_one_outside_name_repeat_it(self):
        files = {'src/a.c': 'int f(void);\n', 'src/b.c': '\n\nint f(void);\n'}
        self.assertEqual(decls.repeats(files), {'f': ['src/a.c:1', 'src/b.c:3']})

    def test_a_source_declaring_what_a_header_declares_repeats_it(self):
        files = {'src/a.c': 'extern int x;\n', 'src/a.h': 'extern int x;\n'}
        self.assertEqual(decls.repeats(files), {'x': ['src/a.h:1', 'src/a.c:1']})

    def test_a_definition_that_returns_a_struct_ends_at_its_body(self):
        # read on to the next ;, a.c's definition and declaration would read as one declaration of make
        files = {'src/a.c': 'struct task *make(void)\n{\n    return 0;\n}\nvoid f(void);\n',
                 'src/b.c': 'void f(void);\n', 'src/a.h': 'struct task *make(void);\n'}
        self.assertEqual(decls.repeats(files), {'f': ['src/a.c:5', 'src/b.c:1']})

    def test_a_sources_own_name_or_two_headers_repeat_nothing(self):
        files = {'src/a.c': 'int f(void);\nint f(void)\n{\n}\n', 'src/b.c': 'int f(void);\n',
                 'src/a.h': 'int g(void);\n',
                 'sdk/include/b.h': 'int g(void);\n'}
        self.assertEqual(decls.repeats(files), {})

    def test_the_network_sdks_call_into_the_game_may_repeat_with_its_header_alone(self):
        declared = 'void sbExitSystem(void);\n'
        files = {'src/system/sbinit.h': declared, 'sdk/network/sg_dpldr.c': declared}
        self.assertEqual(decls.repeats(files), {})
        files['sdk/network/other.c'] = files.pop('sdk/network/sg_dpldr.c')
        self.assertEqual(list(decls.repeats(files)), ['sbExitSystem'])
        files = {'sdk/network/sg_dpldr.c': declared, 'src/b.c': declared}
        self.assertEqual(decls.repeats(files), {'sbExitSystem': ['sdk/network/sg_dpldr.c:1', 'src/b.c:1']})
        files['src/system/sbinit.h'] = declared
        self.assertEqual(decls.repeats(files), {'sbExitSystem': ['src/system/sbinit.h:1', 'sdk/network/sg_dpldr.c:1',
                                                                 'src/b.c:1']})

    def test_reads_the_groups_each_compiler_takes(self):
        # a definition in a group no compiler takes leaves the name outside; one the file can't decide reads
        for test, repeated in (('0', True), ('1', False), ('UNKNOWN', False)):
            with self.subTest(test=test):
                files = {'src/a.c': f'void f(void);\n#if {test}\nvoid f(void) {{}}\n#endif\n',
                         'src/b.c': 'void f(void);\n'}
                self.assertEqual(decls.repeats(files), {'f': ['src/a.c:1', 'src/b.c:1']} if repeated else {})
        # each compiler alone: SHC repeats g, and GCC repeats h
        files = {'src/a.c': '#ifdef NONMATCHING\nvoid g(void) {}\nvoid h(void);\n#else\nvoid g(void);\n'
                            'void h(void) {}\n#endif\n',
                 'src/b.c': 'void g(void);\nvoid h(void);\n'}
        self.assertEqual(decls.repeats(files), {'g': ['src/a.c:5', 'src/b.c:1'], 'h': ['src/a.c:3', 'src/b.c:2']})
        # a macro the file defines decides, and one an undecided group defines doesn't
        files = {'src/a.c': '#define ON 1\n#if ON\nint x;\n#else\nextern int x;\n#endif\n'
                            '#if UNKNOWN\n#define LATE\n#endif\n#ifndef LATE\nint y;\n#else\nextern int y;\n#endif\n',
                 'src/b.c': 'extern int x;\nextern int y;\n'}
        self.assertEqual(decls.repeats(files), {})


class Includes(TreeTest):
    def test_names_a_header_of_modules_a_file_of_src_includes(self):
        self.write('modules/shared/ring_group.h', '')
        self.write('src/shared/kept.h', '')
        self.write('src/draw/local.h', '')
        text = ('#include "shared/ring_group.h"\n#include <shared/ring_group.h>\n#include "shared/kept.h"\n'
                '#include "local.h"\n#include "../../modules/shared/ring_group.h"\n')
        crossing = ("includes modules/shared/ring_group.h, a header of the overlays, which the main executable can't "
                    'rely on')
        self.assertEqual(decls.crossings('src/draw/a.c', text),
                         [f'src/draw/a.c:{line}: {crossing}' for line in (1, 2, 5)])

    def test_only_literal_includes_cross_the_boundary_with_their_physical_line(self):
        self.write('modules/shared/ring.h', '')
        self.write('modules/shared/overlay.inc', '')
        self.write('sdk/modem/private.h', '')
        self.write('local.h', '')
        for newline in ('\n', '\r\n'):
            text = newline.join(['/*', '#include "shared/ring.h"', '*/',
                                 '// continued comment \\', '#include "shared/ring.h"',
                                 'const char *s = "#include \\"shared/ring.h\\"";',
                                 '/* prefix */ #inc\\', 'lude /* heading */ <shared/ri\\', 'ng.h>',
                                 '#include "shared/overlay.inc"', '#include MACRO', '#include <private.h>',
                                 '#include <local.h>', ''])
            with self.subTest(newline=newline):
                self.assertEqual(decls.crossings('src/draw/a.c', text), [
                    f'src/draw/a.c:{line}: includes {header}, a header of the overlays, '
                    "which the main executable can't rely on"
                    for line, header in ((7, 'modules/shared/ring.h'), (10, 'modules/shared/overlay.inc'))])


class Declarations(TreeTest):
    def read(self, path='src/a.c', gcc=False):
        """Each declarator declarations() reads of the source at path, by its name, and the Types that lays them out."""
        _, types, declarators = decls.declarations(path, gcc)
        return {each.name: each for each in declarators}, types

    def test_reads_each_declarator_of_a_declaration_with_its_storage_type_and_lines(self):
        self.write('src/a.h', 'typedef int Sint32;\nextern Sint32 seen, *pointed,\n    held[4];\n')
        self.write('src/a.c', '#include "a.h"\nstatic int alone;\n')
        found, _ = self.read()
        self.assertEqual({name: (each.storage, each.term, each.file.as_posix(), each.line, each.last)
                          for name, each in found.items()},
                         {'Sint32': ('typedef', ('base', ('int',)), 'src/a.h', 1, 1),
                          'seen': ('extern', ('typedef', 'Sint32'), 'src/a.h', 2, 2),
                          'pointed': ('extern', ('pointer', ('typedef', 'Sint32')), 'src/a.h', 2, 2),
                          'held': ('extern', ('array', ('typedef', 'Sint32'), ('4',)), 'src/a.h', 2, 3),
                          'alone': ('static', ('base', ('int',)), 'src/a.c', 2, 2)})

    def test_reads_a_body_in_any_spelling_of_its_declaration(self):
        # a keep macro or an attribute ahead of the declaration, its qualifiers and storage class in any order, a
        # comment beside the #include, and a struct or union the declaration defines
        self.write('src/a.c', '#ifdef NONMATCHING\n#define KEEP __attribute__((used))\n#else\n#define KEEP\n#endif\n'
                   'KEEP\nstatic const int first[2] = {\n#include "a/first.inc" /* the first */\n};\n'
                   'int const static second = {\n#include "a/second.inc"\n};\n'
                   '__attribute__((used)) static struct { int x; char y; } third = {\n#include "a/third.inc"\n};\n'
                   'static union { char c; short h; } fourth = {\n#include "a/fourth.inc"\n};\n'
                   'typedef short Pair[2];\nstatic Pair fifth = {\n#include "a/fifth.inc"\n};\n')
        found, types = self.read()
        self.assertEqual({name: (each.storage, each.body) for name, each in found.items() if each.body},
                         {name: ('static', Path(f'a/{name}.inc')) for name in ('first', 'second', 'third', 'fourth',
                                                                               'fifth')})
        self.assertEqual((found['first'].term, found['second'].term),
                         (('array', ('qualified', ('base', ('int',)), ('const',)), ('2',)),
                          ('qualified', ('base', ('int',)), ('const',))))
        self.assertEqual([types.layout(found[name].term).size for name in ('third', 'fourth', 'fifth')], [8, 2, 4])

    def test_reads_a_body_where_braces_hold_one_include_alone(self):
        self.write('src/a.c', 'int none;\nint braces[2] = {1, 2};\nchar string[] = "a" "b";\nint expression = 1 + 2;\n'
                   'int body[2] = {\n#include "a/body.inc"\n};\nint partial[3] = {\n1,\n#include "a/partial.inc"\n};\n'
                   'int guarded[1] = {\n#ifndef DREAMCAST\n#include "a/host.inc"\n#endif\n'
                   '#include "a/guarded.inc"\n};\n')
        found, _ = self.read()
        self.assertEqual({name: (each.initialized, each.body) for name, each in found.items()},
                         {'none': (False, None), 'braces': (True, None), 'string': (True, None),
                          'expression': (True, None), 'body': (True, Path('a/body.inc')), 'partial': (True, None),
                          'guarded': (True, Path('a/guarded.inc'))})

    def test_reads_a_functions_statics_and_automatic_objects_and_none_of_its_parameters(self):
        self.write('src/a.c', 'void draw(int count)\n{\n    static int pose[2] = {\n#include "a/pose.inc"\n    };\n'
                   '    int offsets[3] = {\n#include "a/offsets.inc"\n    };\n}\n'
                   'int old(first)\nint first;\n{\n    return first;\n}\nint after;\n')
        found, _ = self.read()
        self.assertEqual({name: (each.function, each.storage, each.line, each.last) for name, each in found.items()},
                         {'draw': (None, None, 1, 1), 'pose': ('draw', 'static', 3, 5),
                          'offsets': ('draw', None, 6, 8), 'old': (None, None, 10, 10), 'after': (None, None, 15, 15)})

    def test_reads_the_c_an_include_finds_in_the_tree_whatever_its_suffix(self):
        # an include the build generates, which no search finds, is a body; one the tree holds is C
        self.write('src/a/task.inc', 'static int step(void)\n{\n    return held;\n}\n')
        self.write('src/a/names.inc', '1, 2\n')
        self.write('src/a.c', 'static int held;\nstatic int titles[2] = {\n#include "a/titles.inc"\n};\n'
                   'static int names[2] = {\n#include "a/names.inc"\n};\n#include "a/task.inc"\n')
        found, _ = self.read()
        self.assertEqual({name: (each.file.as_posix(), each.body, sorted(each.names)) for name, each in found.items()},
                         {'held': ('src/a.c', None, []), 'titles': ('src/a.c', Path('a/titles.inc'), []),
                          'names': ('src/a.c', None, []), 'step': ('src/a/task.inc', None, [('held', None)])})

    def test_names_what_each_initializer_and_each_function_body_takes(self):
        # the innermost declaration in scope binds each name, and a block's extern binds the name with linkage. An
        # automatic object's initializer is its own, and the object binds none. A member, a tag, sizeof's operand
        # and an if (0)'s block name nothing
        self.write('src/a.c', 'struct pair { int a; int b; };\nstatic int count;\nstatic int table[2];\n'
                   'static int *first = &table[1];\nstatic struct pair pair;\nint shadowed;\n'
                   'int draw(int shadowed)\n{\n    static int *held = &count;\n    int *seen = table;\n'
                   '    extern int outside;\n    if (0) {\n        count = 1;\n    }\n'
                   '    return pair.a + sizeof table + shadowed + outside + *held + *seen;\n}\n')
        found, _ = self.read()
        self.assertEqual({name: sorted(each.names) for name, each in found.items() if each.names},
                         {'first': [('table', None)], 'held': [('count', None)], 'seen': [('table', None)],
                          'draw': [('held', 'draw'), ('outside', None), ('pair', None)]})

    def test_reads_the_side_of_each_guard_shc_compiles(self):
        self.write('src/a.c', '#ifdef NONMATCHING\nint portable;\n#else\nint original;\n#endif\n'
                   '#ifdef DREAMCAST\nint console;\n#else\nint host;\n#endif\n'
                   '#ifndef DREAMCAST\nint other_host;\n#endif\n')
        self.assertEqual(sorted(self.read()[0]), ['console', 'original'])

    def test_reads_the_side_of_each_guard_gcc_compiles(self):
        # make dc's GCC defines both switches and NDEBUG, and a static assertion declares nothing
        self.write('src/a.c', '#ifdef NONMATCHING\nint portable;\n_Static_assert(sizeof(int) == 4, "int");\n#else\n'
                   'int original;\n#endif\n#ifdef DREAMCAST\nint console;\n#endif\n#ifdef NDEBUG\nint quiet;\n#endif\n'
                   'int last(void);\n')
        self.assertEqual(sorted(self.read(gcc=True)[0]), ['console', 'last', 'portable', 'quiet'])

    def test_reads_names_as_shc_sees_them_where_the_source_defines_its_macros(self):
        # a variant renames its provider's names and sets its parameters, from its #define to its #undef; a
        # compiler's own macro, which no #define reaches, is undefined
        self.write('src/shared.c', 'extern int shared_model;\n#ifndef VARIANT\nenum { COUNT = 6 };\n#endif\n'
                   'static int table[COUNT];\n#undef shared_model\nextern int shared_model;\n'
                   '#ifdef VARIANT\nint variant_only;\n#else\nint shared_only;\n#endif\n'
                   '#ifdef __GNUC__\nint gnu;\n#else\nint other;\n#endif\n')
        self.write('src/a.c', 'extern int shared_model;\n#define shared_model variant_model\n#define VARIANT\n'
                   '#define COUNT 5\n#include "shared.c"\n')
        found, types = self.read()
        self.assertEqual(sorted(found), ['other', 'shared_model', 'table', 'variant_model', 'variant_only'])
        self.assertEqual(types.layout(found['table'].term).count, 5)
        found, types = self.read('src/shared.c')
        self.assertEqual(sorted(found), ['other', 'shared_model', 'shared_only', 'table'])
        self.assertEqual(types.layout(found['table'].term).count, 6)

    def test_takes_the_group_of_each_if_its_test_gives(self):
        # defined reads before the macros expand, and a name no macro gives reads as 0
        self.write('src/a.c', '#define TWO 2\n#define ON\n'
                   '#if defined(ON) && TWO > 1\nint first;\n#elif 1\nint not_second;\n#else\nint not_third;\n#endif\n'
                   '#if !defined ON || UNDEFINED\nint not_fourth;\n#elif TWO == 2\nint fifth;\n#endif\n')
        self.assertEqual(sorted(self.read()[0]), ['fifth', 'first'])

    def test_fails_a_test_it_cant_evaluate(self):
        # rather than read both groups, where FLAG can't be defined in the one that tests it
        self.write('src/a.c', "#define TWO 2\n#if TWO == '2'\n#define FLAG\n#else\n#ifdef FLAG\n"
                   'extern int impossible;\n#endif\n#endif\n')
        self.assertIn("src/a.c:2: an #if whose test the tool can't evaluate", self.fails(decls.declarations, 'src/a.c'))

    def test_reads_the_value_each_compilers_flags_give(self):
        # GCC's -D gives 1, and SHC's -define=DREAMCAST a value the tool doesn't know
        self.write('src/a.c', '#if NDEBUG + 0 == 1 && DREAMCAST == 1\nint valued;\n#else\nint unvalued;\n#endif\n')
        self.assertEqual(sorted(self.read(gcc=True)[0]), ['valued'])
        self.write('src/a.c', '#if DREAMCAST == 1\nint valued;\n#endif\n')
        self.assertIn("src/a.c:1: an #if whose test the tool can't evaluate", self.fails(decls.declarations, 'src/a.c'))

    def test_reads_what_a_function_like_macro_declares(self):
        # its arguments in place of its parameters, a ## that pastes, a # that spells a string, the arguments past
        # its named parameters, and an invocation among a declaration's specifiers or after its declarator
        self.write('src/a.h', '#define PANEL(n) extern int panel_##n; extern int panel_uv_##n\n'
                   '#define MESSAGE(name, bytes, text) static const char name[bytes] = text\n'
                   '#define NAMED(name) static const char name##_name[] = #name\n'
                   '#define LIST(name, ...) static int name[] = {__VA_ARGS__}\n'
                   '#define NOOP(name) int name(void) { return 0; }\n#define BANK(name, alignment)\n')
        self.write('src/a.c', '#include "a.h"\nPANEL(1);\nPANEL(2);\nMESSAGE(no_handle, 8, "handle");\n'
                   'NAMED(ring);\nLIST(sizes, 1, 2, 3);\nstatic NOOP(start) static NOOP(stop)\n'
                   'int buffer[4] BANK(easy, 32);\n')
        found, types = self.read()
        self.assertEqual(sorted(found), ['buffer', 'no_handle', 'panel_1', 'panel_2', 'panel_uv_1', 'panel_uv_2',
                                         'ring_name', 'sizes', 'start', 'stop'])
        self.assertEqual({name: (found[name].storage, found[name].line, found[name].term) for name in
                          ('panel_uv_2', 'no_handle', 'start')},
                         {'panel_uv_2': ('extern', 3, ('base', ('int',))),
                          'no_handle': ('static', 4, ('array', ('qualified', ('base', ('char',)), ('const',)), ('8',))),
                          'start': ('static', 7, ('function', ('base', ('int',)), ()))})

    def test_rescans_a_replacement_with_the_tokens_after_it(self):
        # a function-like macro's name that ends a replacement takes its arguments from the source, each time
        self.write('src/a.c', '#define DECL(x) extern int x\n#define ALIAS DECL\nALIAS(score);\nALIAS(other);\n')
        found, _ = self.read()
        self.assertEqual({name: (each.storage, each.line) for name, each in found.items()},
                         {'score': ('extern', 3), 'other': ('extern', 4)})

    def test_spells_a_forwarded_argument_with_its_blanks(self):
        macros, types = {}, Types([])
        for directive in parse('#define S(x) #x\n#define FORWARD(x) S(x)\n').directives:
            decls.define(directive, macros, types)
        for call in ('S', 'FORWARD'):
            with self.subTest(call=call):
                spelled = [token.text for token in decls.expanded(parse(f'{call}(one two) {call}(a+b)').parsed, macros)]
                self.assertEqual(spelled, ['"one two"', '"a+b"'])

    def test_reads_no_body_without_braces_or_where_the_tree_holds_the_include(self):
        self.write('src/one.inc', '1\n')
        self.write('src/a.c', 'int value = -\n#include "one.inc"\n;\nint body[1] = {\n#include "a/body.inc"\n};\n')
        found, _ = self.read()
        self.assertEqual({name: each.body for name, each in found.items()}, {'value': None, 'body': Path('a/body.inc')})

    def test_names_no_object_by_a_label(self):
        self.write('src/a.c', 'int retry;\nvoid f(int x)\n{\n    goto retry;\nretry:\n    x = x ? retry : 0;\n}\n')
        self.assertEqual(self.read()[0]['f'].names, {('retry', None)})
        self.write('src/a.c', 'int retry;\nvoid f(void)\n{\n    goto retry;\nretry:\n    ;\n}\n')
        self.assertEqual(self.read()[0]['f'].names, set())

    def test_fails_two_statics_of_one_name_in_a_function(self):
        # where the two would give one entity(); an automatic object that shadows binds none
        self.write('src/a.c', 'int n;\nvoid f(void)\n{\n    {\n        static int n;\n        ++n;\n    }\n    {\n'
                   '        int n;\n        ++n;\n    }\n}\n')
        self.assertEqual(self.read()[0]['f'].names, {('n', 'f')})
        self.write('src/a.c', 'void f(void)\n{\n    {\n        static int n;\n    }\n    {\n'
                   '        static int n;\n    }\n}\n')
        self.assertIn("src/a.c:7: a second static n in f, after src/a.c:4, which the tool can't tell from it",
                      self.fails(decls.declarations, 'src/a.c'))

    def test_reads_an_included_file_where_its_include_stands(self):
        # each time the read reaches it, with the macros defined there: a header that a function includes declares
        # in the function's blocks
        self.write('src/glow.h', '#ifdef AXIS_X\nstatic int glow_x[2] = {\n#include "glow/glow_x.inc"\n};\n#else\n'
                   'static int glow_y[2] = {\n#include "glow/glow_y.inc"\n};\n#endif\n')
        self.write('src/a.c', 'void draw(void)\n{\n#define AXIS_X\n#include "glow.h"\n#undef AXIS_X\n'
                   '#include "glow.h"\n}\n')
        found, _ = self.read()
        self.assertEqual([(each.name, each.function, each.file.as_posix(), each.body) for each in found.values()],
                         [('draw', None, 'src/a.c', None), ('glow_x', 'draw', 'src/glow.h', Path('glow/glow_x.inc')),
                          ('glow_y', 'draw', 'src/glow.h', Path('glow/glow_y.inc'))])

    def test_reads_no_assembly(self):
        # an inline body's and a #pragma asm block's lines hold assembly, which reads as no C
        self.write('src/a.c', '#pragma inline_asm(twice)\nstatic int twice(int x)\n{\n'
                   '        ADD R4,R4 ; { int bogus;\n}\n#pragma asm\n_label: .DATA.L 0 ; int bogus;\n#pragma endasm\n'
                   'int after;\n')
        self.assertEqual(sorted(self.read()[0]), ['after', 'twice'])

    def test_fails_each_declaration_it_cant_read(self):
        # at file scope, as in a block, naming the file that holds it
        self.write('src/a.h', 'int first;\nDECLARE(x) int y[2] = {\n#include "a/y.inc"\n};\n')
        self.write('src/a.c', '#include "a.h"\nvoid f(void)\n{\n    static int a b;\n}\n')
        failed = self.fails(decls.declarations, 'src/a.c')
        self.assertEqual([line.partition(': ')[2] for line in failed.splitlines()],
                         ["src/a.h:2: a declaration the tool can't read: int after a declarator",
                          "src/a.c:4: a declaration the tool can't read: b after a declarator"])

    def test_fails_a_file_its_own_include_reaches(self):
        self.write('src/a.h', '#include "a.h"\n')
        self.write('src/a.c', '#include "a.h"\n')
        self.assertIn('src/a.h: an #include reaches it again inside itself', self.fails(decls.declarations, 'src/a.c'))


class Main(TreeTest):
    def check(self):
        """decls.main's status over a.c and b.c, and each line it failed with, less the tool's name."""
        status, output = run(decls.main, 'src/a.c', 'src/b.c')
        return status, [line.split(': ', 1)[1] for line in output.splitlines()]

    def test_passes_sources_that_repeat_nothing(self):
        self.write('src/a.c', 'int f(void);\n')
        self.write('src/b.c', 'extern int x;\n')
        self.assertEqual(self.check(), (0, []))

    def test_fails_each_name_two_sources_declare(self):
        self.write('src/a.c', 'int f(void);\nextern int x;\n')
        self.write('src/b.c', 'int f(void);\nextern int x;\n')
        repeated = 'more than once as an outside name; one header these files include can declare it'
        self.assertEqual(self.check(), (1, [f'f: declared at src/a.c:1, src/b.c:1, {repeated}',
                                            f'x: declared at src/a.c:2, src/b.c:2, {repeated}']))

    def test_reads_each_file_once_however_its_path_is_spelled(self):
        self.write('modules/shared/ring.h', '')
        self.write('src/a.c', '#include "shared/ring.h"\nextern int x;\n')
        absolute = os.path.abspath('src/a.c')
        for spelled in (['src/a.c'], ['./src/a.c'], [absolute], ['src/a.c', './src/a.c', absolute]):
            with self.subTest(spelled=spelled):
                status, output = run(decls.main, *spelled)
                self.assertEqual((status, [line.split(': ', 1)[1] for line in output.splitlines()]), (1, [
                    "src/a.c:1: includes modules/shared/ring.h, a header of the overlays, which the main executable "
                    "can't rely on"]))
