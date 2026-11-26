"""make dc and make dc-check: their host checks, the compile of each source
by its route, the split of inline bodies, each image's census, the sums,
SH4ZAM's files and the toolchain's receipt. Each test runs make or make -n
in a scratch tree of its own, with a toolchain, build/shckit and
tools/census.py stood in, and needs no disc, no SHC and no GCC, but
Checkouts', which runs where SH_ELF names a toolchain."""

import hashlib
import os
import re
import shlex
import subprocess
import tempfile
import unittest
from pathlib import Path

import split
from support import ROOT, SH_ELF as TOOLCHAIN
from test_make import (COMMANDS, COMPILERS, LINUX, MACS, MOVED, PORT_GOALS, RAN, append, checkout, host,
                       macos_sha256sum, make, replace, settle, stand_in)

# the toolchain's files make dc's stamps hash and its checks look for, by
# their paths in SH_ELF
LIBEXEC = 'libexec/gcc/sh-elf/16.2.0'
LIBGCC = 'lib/gcc/sh-elf/16.2.0/libgcc.a'
SH_ELF_FILES = ('bin/sh-elf-ld', f'{LIBEXEC}/cc1', *(f'{LIBEXEC}/{name}' for name in (
    'lto1', 'lto-wrapper', 'collect2', 'liblto_plugin.so')), 'lib/gcc/sh-elf/16.2.0/include/stddef.h')
# A GCC that writes the .d file -MF names, a rule of -MT's target and its
# inputs, and writes to -o's file each of its inputs, those an @ file lists
# among them, one after another: an object holds its C, and a census's link
# its objects. A flag's argument is no input
GCC = '''#!/bin/sh
out= d= t= in=
while [ $# -gt 0 ]; do
    case $1 in
        -MF) d=$2; shift;; -MT) t=$2; shift;; -o) out=$2; shift;;
        -isystem|-idirafter|-iquote) shift;;
        @*) in="$in $(cat "${1#@}")";;
        -*) ;;
        *) in="$in $1";;
    esac
    shift
done
[ -z "$d" ] || echo "$t: $in" > "$d"
cat $in > "$out"
'''
# GNU as and shckit's translate copy their input, and the archiver writes
# its members' names
AS = '#!/bin/sh\nwhile [ $# -gt 1 ]; do [ "$1" = -o ] && out=$2; shift; done\ncp "$1" "$out"\n'
SHCKIT = '#!/bin/sh\n[ "$1" = translate ] && [ "$2" = -o ] && cp "$4" "$3"\n'
AR = '#!/bin/sh\nshift\nout=$1\nshift\nprintf "%s\\n" "$@" > "$out"\n'
# objcopy copies its input to its output, the last two of its arguments,
# and removes no section
OBJCOPY = '#!/bin/sh\nwhile [ $# -gt 2 ]; do shift; done\ncp "$1" "$2"\n'
# a census that writes the NAME of each line of the link that reads U NAME,
# the stood-in sources' way to leave NAME undefined
CENSUS = ('import sys\nfrom pathlib import Path\n'
          "lines = Path(sys.argv[1]).read_text().splitlines()\n"
          "Path(sys.argv[2]).write_text(''.join(line[2:] + '\\n' for line in lines if line.startswith('U ')))\n")
# SH4ZAM's library as its fetch leaves it: a LICENSE, a header, and the SH-4
# back end's C and assembly, which the build compiles, and the software back
# end's C, which it leaves out. tools/SHA256SUMS gives each file its line,
# and the fetch's record holds those lines
LIBRARY = {f'tools/sh4zam/{path}': f'{path}\n' for path in ('LICENSE', 'include/sh4zam/a.h', 'source/sh4/a.c',
                                                            'source/sh4/b.s', 'source/sw/c.c')}
SUMS = ''.join(f'{hashlib.sha256(text.encode()).hexdigest()}  {path}\n' for path, text in LIBRARY.items())
# what make dc needs beyond a checkout(): the stood-in toolchain under
# sh-elf/, build/shckit, which settle() dates after its sources, the census,
# an empty config/imports.txt, which make dc-check reads, a
# config/addresses.txt of no rows, which make dc and dc-check read, SH4ZAM's
# files, and the repository's dc.mk and config/gcc.mk, which read the
# toolchain through SH_ELF
NEEDS = {
    **{f'sh-elf/{path}': '' for path in SH_ELF_FILES}, f'sh-elf/{LIBGCC}': 'libgcc\n',
    'sh-elf/bin/sh-elf-gcc': GCC, 'sh-elf/bin/sh-elf-as': AS, 'sh-elf/bin/sh-elf-ar': AR,
    'sh-elf/bin/sh-elf-objcopy': OBJCOPY,
    'build/shckit': SHCKIT, 'build/tools/vendored.ok': '', 'tools/census.py': CENSUS, 'config/imports.txt': '',
    'config/addresses.txt': '# file  start  end  count  disposition, then the reason\n',
    **LIBRARY, 'tools/SHA256SUMS': SUMS, 'tools/sh4zam/fetched.txt': SUMS,
    'dc.mk': (ROOT / 'dc.mk').read_text(), 'config/gcc.mk': (ROOT / 'config/gcc.mk').read_text(),
    'config/dc_links.mk': (ROOT / 'config/dc_links.mk').read_text(),
    'config/dreamcast.mk': 'DC_REPLACED :=\nDC_ADDED :=\n', 'config/parts.txt': 'src/ picture\nmodules/ picture\n',
}
# make dc's settings in a test: the stood-in toolchain
SH_ELF = 'SH_ELF=sh-elf'
# a source of one inline body, on GCC's side, and one whose body a guard
# keeps for SHC
INLINE = '/* @asm open: a sum */\n#pragma inline_asm(twice)\nint twice(int x)\n{\n        RTS\n        ADD R4,R4\n}\n'
# the three sources, each of which a stood-in GCC or GNU as copies to its
# object
SOURCES = {'src/main.c': 'main\n', 'modules/shared/a.c': 'a\n', 'modules/stg13/b.src': 'b\n'}
SHCS = ('#ifdef NONMATCHING\nint twice(int x) { return x + x; }\n#else\n'
        '/* @asm open: a sum */\n#pragma inline_asm(twice)\nint twice(int x)\n{\n        RTS\n        ADD R4,R4\n}\n'
        '#endif\n')


def stood_in(files=None):
    """A checkout() with NEEDS and files, its scripts executable."""
    every = {**NEEDS, **(files or {})}
    tree = checkout(every)

    class Tree:
        def __enter__(self):
            root = tree.__enter__()
            for path, text in every.items():
                if isinstance(text, str) and text.startswith('#!'):
                    (root / path).chmod(0o755)
            settle(root)
            return root

        def __exit__(self, *raised):
            return tree.__exit__(*raised)
    return Tree()


def sums(root):
    """The hash of each named product in the disc-free receipt."""
    result = {}
    for line in (root / 'build/dc/check.txt').read_text().splitlines():
        digest, path = line.split('  ')
        result[path] = digest
    return result


def commands(output):
    """Each GCC or GNU as command make -n prints, as its shell's words, by
    the file its -o writes."""
    found = {}
    for line in output.splitlines():
        for command in line.split(' && '):
            words = shlex.split(command.split(' > ')[0]) if 'sh-elf/bin/sh-elf-' in command else []
            if '-o' in words:
                found[words[words.index('-o') + 1]] = words
    return found


class Host(unittest.TestCase):
    def test_final_link_requires_the_resident_layout_only_when_requested(self):
        final_rules = {'config/images.mk': (ROOT / 'config/images.mk').read_text()}
        with stood_in({**SOURCES, **final_rules}) as root:
            status, output = make(root, SH_ELF, 'dc-images')
            self.assertNotEqual(status, 0, output)
            self.assertIn('dc-images needs the resident startup objects and layout', output)
            status, output = make(root, SH_ELF, 'dc-check')
            self.assertEqual(status, 0, output)
            self.assertFalse((root / 'build/dc/images').exists())

    def test_links_the_resident_only_beside_an_empty_census(self):
        # a census rule of no recipe stands in for one that wrote nothing:
        # make takes it as made, so the guard alone must refuse the missing
        # census, as it does one that names a symbol
        driver = ('MAIN_LAYOUT := layout.o\nMAIN_CENSUS := census\nDC_MAIN := 1st_read\nDC_IMAGES := 1st_read\n'
                  'dc_terminal = $1.o\ninclude config/images.mk\ncensus.txt:\n')
        ld = '#!/bin/sh\nwhile [ "$1" != -o ]; do shift; done\n: > "$2"\n'
        files = {'driver.mk': driver, 'config/images.mk': (ROOT / 'config/images.mk').read_text(),
                 'config/1st_read/image.ld': '', '1st_read.o': '', 'sh-elf/bin/sh-elf-ld': ld}
        resident = 'build/dc/images/1st_read.elf'
        with checkout(files) as root:
            (root / 'sh-elf/bin/sh-elf-ld').chmod(0o755)
            for census, linked in (('', True), ('stray\n', False), (None, False)):
                with self.subTest(census=census):
                    (root / resident).unlink(missing_ok=True)
                    if census is None:
                        (root / 'census.txt').unlink()
                    else:
                        (root / 'census.txt').write_text(census)
                    status, output = make(root, '-f', 'driver.mk', SH_ELF, resident)
                    self.assertEqual((status == 0, (root / resident).exists()), (linked, linked), output)
                    if census:
                        self.assertIn('stray', output)

    def test_packs_an_image_once_each_state_that_holds_it_keeps_the_memory_contract(self):
        # the first two overrides stand in for the resident's census, which no
        # stood-in link writes. The third empties the memory contract's
        # sources, which the tree lacks. With or without them, make -n prints
        # the same commands
        files = {**SOURCES, 'config/images.mk': (ROOT / 'config/images.mk').read_text(),
                 **{path: '' for path in ('config/1st_read/image.ld', 'config/overlay.ld', 'build/prskit')}}
        settings = (SH_ELF, '--eval=override MAIN_LAYOUT := main.ld',
                    '--eval=override MAIN_CENSUS := build/dc/census/1st_read',
                    '--eval=override DC_MEMORY_SOURCES :=', '-n')

        def run(image):
            return f'python3 tools/dc_memory.py {image} build/dc'

        with stood_in(files) as root:
            for goal, image, states in (('build/dc/SONIC2/STG13.PRS', 'stg13', ['stg13']),
                                        ('build/dc/1ST_READ.BIN', '1st_read', ['1st_read', 'stg13'])):
                with self.subTest(goal=goal):
                    status, output = make(root, *settings, goal)
                    self.assertEqual(status, 0, output)
                    lines = output.splitlines()
                    pack = lines.index(f'python3 tools/dc_publish.py {image} build/dc')
                    self.assertEqual([line for line in lines[:pack] if 'dc_memory.py' in line],
                                     [run(state) for state in states])
            status, output = make(root, *settings, 'dc-images')
            self.assertEqual(status, 0, output)
            self.assertIn(run('1st_read'), output.splitlines())
            self.assertIn(run('stg13'), output.splitlines())

    def test_disc_asset_reaches_the_terrain_link_only_in_the_playable_build(self):
        files = {**SOURCES, 'config/stg13/symbols.txt': '0x8c500200 menu_cells\n',
                 'config/stg13/data.txt': '# layout comment\n0x8c500200 layout menu#cells\n',
                 'layouts/menu.txt': 'menu_cells texanims 1\n',
                 'config/terrain.ld': (ROOT / 'config/terrain.ld').read_text(),
                 'config/dreamcast.mk': NEEDS['config/dreamcast.mk'] + 'DC_TERRAIN_LAYOUT := config/terrain.ld\n'}
        with stood_in(files) as root:
            append('config/stg13/splits.txt', 'D 0x8c500200 0x8c500214\ndisc D 0x8c500200 0x8c500214\n')(root)
            status, output = make(root, SH_ELF, '-n', 'dc-check')
            self.assertEqual(status, 0, output)
            self.assertIn('--content stg13', output)
            receipt = (root / 'build/dc/check/objects.txt').read_text().split()
            self.assertIn('build/dc/check/census/stg13-terrain.o', receipt)
            self.assertNotIn('build/dc/census/stg13-terrain.o', receipt)
            self.assertNotIn('build/dc/check/census/stg13.o', receipt)
            self.assertNotIn('tools/content.py stg13 build/orig/', output)
            asset = 'build/dc/content/stg13.o'
            self.assertNotIn(asset, (root / 'build/dc/check/census/stg13.in').read_text().split())
            self.assertIn(asset, (root / 'build/dc/census/stg13.in').read_text().split())
            # Supply the source prerequisite for make -n, which prints the
            # authenticated decode and actual link without reading this file.
            (root / 'orig/SONIC2').mkdir(parents=True, exist_ok=True)
            (root / 'orig/SONIC2/STG13.PRS').touch()
            status, output = make(root, SH_ELF, '-n', 'build/dc/census/stg13-terrain.o')
            self.assertEqual(status, 0, output)
            self.assertIn('tools/content.py stg13 build/orig/SONIC2/STG13.BIN', output)
            self.assertIn('-T config/terrain.ld -o build/dc/census/stg13-terrain.o build/dc/census/stg13.o', output)
            (root / 'layouts/menu.txt').unlink()
            status, output = make(root, SH_ELF, '-n', 'build/dc/content/stg13.s')
            self.assertNotEqual(status, 0)
            self.assertIn('layouts/menu.txt', output)

    def test_check_checks_each_data_table_without_the_disc(self):
        files = {**SOURCES, 'config/stg13/symbols.txt': '0x8c500200 menu_cells\n',
                 'config/stg13/data.txt': '0x8c500200 menu_cells texanims 1\n'}
        with stood_in(files) as root:
            append('config/stg13/splits.txt', 'D 0x8c500200 0x8c500214\ndisc D 0x8c500200 0x8c500214\n')(root)
            status, output = make(root, SH_ELF, 'dc-check')
            self.assertEqual(status, 0, output)
            replace('config/stg13/data.txt', 'texanims 1', 'texanims 2')(root)
            status, output = make(root, SH_ELF, 'dc-check')
            self.assertNotEqual(status, 0, output)
            self.assertIn('config/stg13/data.txt:1: menu_cells must fit within one disc C, D or DATA range', output)

    def test_imported_optical_object_depends_on_its_provider_layout(self):
        files = {**SOURCES, 'config/stg13/symbols.txt': '0x8c500200 model\n',
                 'config/stg13/data.txt': 'import 1st_read model#prize\n',
                 'config/1st_read/data.txt': '0x8c500200 layout prize\n',
                 'config/1st_read/symbols.txt': '0x8c500200 model\n'}
        with stood_in(files) as root:
            (root / 'orig/SONIC2').mkdir(parents=True, exist_ok=True)
            (root / 'orig/SONIC2/STG13.PRS').touch()
            status, output = make(root, SH_ELF, '-n', 'build/dc/content/stg13.s')
            self.assertNotEqual(status, 0)
            self.assertIn('layouts/prize.txt', output)

    def test_names_each_command_dc_lacks_before_any_step(self):
        # with neither SH4ZAM's files nor the toolchain in place, make dc downloads both, and builds the toolchain
        lacking = ('sha256sum', 'curl', 'tar', 'gzip', 'xz', 'c++')
        with checkout({'dc.mk': NEEDS['dc.mk'], 'config/gcc.mk': '', 'config/dreamcast.mk': '',
                       'tools/SHA256SUMS': SUMS}) as root, host(*lacking) as path:
            for goal in ('dc', 'dc-check'):
                with self.subTest(goal=goal):
                    status, output = make(root, goal, 'CC=no-such-cc', path=path)
                    self.assertNotEqual(status, 0, output)
                    self.assertIn('*** no-such-cc sha256sum curl tar gzip xz c++: not found; see README.md#build',
                                  output)
                    self.assertFalse((root / 'build/dc').exists())

    def test_needs_no_download_command_once_its_tools_are_in_place(self):
        with stood_in() as root, host(*COMMANDS[2:], 'xz', 'c++') as path:
            status, output = make(root, '-n', 'dc', SH_ELF, path=path)
        self.assertEqual(status, 0, output)

    def test_takes_sh4zams_files_from_a_whole_copy_without_curl_and_names_curl_where_the_copy_fails(self):
        copy = {f'copy/{path[len("tools/sh4zam/"):]}': text for path, text in LIBRARY.items()}
        for name, altered, fails in (('all match', {}, False), ('one differs', {'copy/LICENSE': 'other\n'}, True)):
            with self.subTest(name), stood_in({**copy, **altered, 'tools/sh4zam/fetched.txt': ''}) as root, \
                    host('curl') as path:
                for library in LIBRARY:
                    (root / library).unlink()
                status, output = make(root, 'dc', SH_ELF, 'SH4ZAM=copy', path=path)
                self.assertEqual(status != 0, fails, output)
                self.assertNotIn('curl: not found', output)
                self.assertEqual('tools/sh4zam/LICENSE: SH4ZAM holds no copy that matches its line of '
                                 'tools/SHA256SUMS, and its fetch needs curl' in output, fails, output)


class Compile(unittest.TestCase):
    def test_compiles_each_source_by_its_route_and_assembles_each_src_file(self):
        with stood_in({'config/compilers.mk': COMPILERS + MOVED}) as root:
            status, output = make(root, '-n', 'dc', SH_ELF)
        self.assertEqual(status, 0, output)
        built = commands(output)
        main, shared = built['build/dc/src/main.o'], built['build/dc/modules/shared/a.o']
        # each searches its route's directories, as SHC does, then the root, and carries GCC's intermediate code
        for words, source in ((main, 'src/main.c'), (shared, 'modules/shared/a.c')):
            self.assertEqual([word for word in words if word.startswith('-I')],
                             ['-Isdk/include', '-Isrc', '-Imodules', '-I.'])
            self.assertIn(f'-frandom-seed={source}', words)
            self.assertIn('-flto', words)
            self.assertEqual(words[-1], source)
        # main.c's route takes -optimize=0, and GCC -O0 after the level
        self.assertEqual([word for word in main if word.startswith('-O')], ['-Os', '-O0'])
        self.assertEqual([word for word in shared if word.startswith('-O')], ['-Os'])
        self.assertIn('build/shckit translate -o build/dc/modules/stg13/b.s modules/stg13/b.src', output)
        self.assertEqual(built['build/dc/modules/stg13/b.o'][1:],
                         ['--isa=sh4', '--fatal-warnings', '--little', '--no-pad-sections', '-o',
                          'build/dc/modules/stg13/b.o', 'build/dc/modules/stg13/b.s'])

    def test_compiles_each_hot_source_at_o2(self):
        hot = {'config/1st_read/splits.txt': ('file 1ST_READ.BIN\nP 0x8c010000 0x8c010200\n'
                                              'src/main.c P 0x8c010000 0x8c010040\n'
                                              'src/system/util.c P 0x8c010040 0x8c010080\n'),
               'src/system/util.c': ''}
        with stood_in(hot) as root:
            status, output = make(root, '-n', 'dc', SH_ELF)
        self.assertEqual(status, 0, output)
        self.assertEqual([word for word in commands(output)['build/dc/src/system/util.o'] if word.startswith('-O')],
                         ['-O2'])

    def test_splits_a_source_of_inline_bodies_and_compiles_its_c_with_the_intermediate_code(self):
        with stood_in({'src/main.c': INLINE}) as root:
            status, output = make(root, 'dc', SH_ELF)
            self.assertEqual(status, 0, output)
            self.assertIn('  gcc    src/main.c (split)\n', output)
            c = (root / 'build/dc/src/main.c').read_text()
            self.assertIn('.include \\"build/dc/src/main_inline.s\\"', c)
            self.assertIn('_twice:', (root / 'build/dc/src/main_inline.s').read_text())
            # the object is the split C, and its compile finds the source's quoted includes in src/
            self.assertEqual((root / 'build/dc/src/main.o').read_text(), c)
            status, output = make(root, '-n', '-W', 'src/main.c', 'dc', SH_ELF)
        words = commands(output)['build/dc/src/main.o']
        self.assertEqual(words[words.index('-iquote') + 1], 'src')
        self.assertIn('-frandom-seed=src/main.c', words)

    def test_records_the_assembly_and_the_split_in_each_objects_command(self):
        with stood_in({**SOURCES, 'src/main.c': INLINE}) as root:
            status, output = make(root, '-n', 'dc', SH_ELF)
            self.assertEqual(status, 0, output)
            assembled = (root / 'build/dc/modules/stg13/b.cmd').read_text()
            split_c = (root / 'build/dc/src/main.cmd').read_text()
        for text in (assembled, split_c):
            self.assertIn('python3 tools/tags.py --abi', text)
        self.assertIn('build/shckit translate -o build/dc/$*.s $<', assembled)
        self.assertIn('python3 tools/split.py', split_c)

    def test_translates_no_text_where_every_body_is_shcs(self):
        with stood_in({'src/main.c': SHCS}) as root:
            status, output = make(root, 'dc', SH_ELF)
            self.assertEqual(status, 0, output)
            self.assertEqual((root / 'build/dc/src/main_inline.src').read_text(), '')
            self.assertFalse((root / 'build/dc/src/main_inline.s').exists())
            self.assertNotIn('.include', (root / 'build/dc/src/main.c').read_text())

    def test_checks_each_piece_of_assembly_gcc_may_call_before_it_splits_or_assembles_it(self):
        # twice changes R8, which GCC's callers keep, and b.src's function is tagged abi
        unsafe = INLINE.replace('ADD R4,R4', 'MOV R4,R8')
        tagged = ('        .EXPORT     _b\n        .SECTION    P,CODE,ALIGN=4\n; void b(void)\n'
                  '; @asm abi: stack-written, at 0x8c500000\n_b:\n        RTS\n        NOP\n')
        with stood_in({'src/main.c': unsafe, 'modules/stg13/b.src': tagged}) as root:
            status, output = make(root, '-k', 'dc', SH_ELF)
            self.assertNotEqual(status, 0, output)
            self.assertIn('tags: src/main.c:2: twice: it returns at line 6 with R8 changed, which its GCC callers '
                          'keep, and no --contract names twice:R8', output)
            self.assertIn('tags: modules/stg13/b.src:5: b: its @asm tag is abi, and no --contract names b:abi', output)
            self.assertFalse((root / 'build/dc/src/main.o').exists())
            self.assertFalse((root / 'build/dc/modules/stg13/b.o').exists())
            # a row of CONTRACTS answers each, and each object's command holds its source's
            append('config/gcc.mk', 'CONTRACTS += src/main.c:twice:R8 modules/stg13/b.src:b:abi\n')(root)
            status, output = make(root, 'dc', SH_ELF)
            self.assertEqual(status, 0, output)
            status, output = make(root, '-n', '-W', 'src/main.c', '-W', 'modules/stg13/b.src', 'dc', SH_ELF)
        self.assertIn('python3 tools/tags.py --abi --contract twice:R8 src/main.c', output)
        self.assertIn('python3 tools/tags.py --abi --contract b:abi modules/stg13/b.src', output)

    def test_fails_each_compile_and_each_census_link_on_a_warning(self):
        with stood_in() as root:
            status, output = make(root, '-n', 'dc', SH_ELF)
        self.assertEqual(status, 0, output)
        built = commands(output)
        for target in ('build/dc/src/main.o', 'build/dc/sh4zam/sh4/a.o', 'build/dc/census/stg13.o'):
            self.assertIn('-Werror', built[target], target)

    def test_builds_nothing_again_once_built(self):
        with stood_in() as root:
            status, output = make(root, 'dc', SH_ELF)
            self.assertEqual(status, 0, output)
            status, output = make(root, 'dc', SH_ELF)
        self.assertEqual((status, output), (0, "make: Nothing to be done for 'dc'.\n"))

    def test_builds_again_what_an_edit_reaches_and_nothing_else(self):
        # a route's flags reach its sources' objects, a .src file's its object, the warnings every C object, and
        # each reaches the censuses of the images that place what it rebuilds
        edits = [
            ("a route's flags", replace('config/compilers.mk', '-cpu=sh4 -speed', '-cpu=sh4 -optimize=0'), (),
             {'src/main.o', 'modules/shared/a.o', 'census/1st_read.o', 'census/stg13.o'}),
            ('a .src file', append('modules/stg13/b.src', '; more\n'), (), {'modules/stg13/b.o', 'census/stg13.o'}),
            ('the warnings', replace('config/gcc.mk', '-Wno-switch ', ''), (),
             {'src/main.o', 'modules/shared/a.o', 'sh4zam/sh4/a.o', 'census/1st_read.o', 'census/stg13.o'}),
            ('a comment in config/gcc.mk', append('config/gcc.mk', '# a comment\n'), (), set()),
        ]
        for name, edit, given, rebuilt in edits:
            with self.subTest(name), stood_in() as root:
                status, output = make(root, 'dc', SH_ELF)
                self.assertEqual(status, 0, output)
                settle(root)
                edit(root)
                status, output = make(root, '-n', 'dc', SH_ELF, *given)
                self.assertEqual(status, 0, output)
                self.assertEqual({path[len('build/dc/'):] for path in commands(output)}, rebuilt, output)


class Floats(unittest.TestCase):
    # modules/shared/a.c, play's, and src/main.c, picture's
    PARTS = {'config/parts.txt': 'src/ picture\nmodules/ play\n'}

    def test_compiles_a_play_source_through_the_float_pass_under_contraction_and_any_other_as_before(self):
        with stood_in(self.PARTS) as root:
            status, output = make(root, '-n', 'dc', SH_ELF)
        self.assertEqual(status, 0, output)
        built = commands(output)
        preprocessed, compiled = built['build/dc/modules/shared/a.i'], built['build/dc/modules/shared/a.o']
        self.assertEqual((preprocessed[preprocessed.index('-E') + 1:], compiled[-1]),
                         (['-o', 'build/dc/modules/shared/a.i', 'modules/shared/a.c'], 'build/dc/modules/shared/a.i)'))
        self.assertIn('python3 tools/floats.py --shc r13 --system tools/sh4zam/include build/dc/modules/shared/a.i',
                      output)
        # GCC_FUSING after GCCFLAGS' -ffp-contract=off, in the compile alone
        self.assertEqual([word for word in compiled if word.startswith('-ffp-contract')],
                         ['-ffp-contract=off', '-ffp-contract=on'])
        self.assertEqual([word for word in preprocessed if word.startswith('-ffp-contract')], ['-ffp-contract=off'])
        main = built['build/dc/src/main.o']
        self.assertEqual(([word for word in main if word.startswith('-ffp-contract')], main[-1]),
                         (['-ffp-contract=off'], 'src/main.c'))
        self.assertNotIn('build/dc/src/main.i', built)

    def test_runs_the_pass_and_keeps_no_preprocessed_c(self):
        with stood_in(self.PARTS) as root:
            status, output = make(root, 'dc', SH_ELF)
            self.assertEqual(status, 0, output)
            self.assertIn('  gcc    modules/shared/a.c (play)\n', output)
            self.assertTrue((root / 'build/dc/modules/shared/a.o').exists())
            self.assertFalse((root / 'build/dc/modules/shared/a.i').exists())

    def test_builds_again_each_play_object_once_the_pass_changes_and_nothing_else(self):
        # and the census of each image that places one
        with stood_in(self.PARTS) as root:
            status, output = make(root, 'dc', SH_ELF)
            self.assertEqual(status, 0, output)
            settle(root)
            append('tools/float_fuse.py', '# more\n')(root)
            status, output = make(root, '-n', 'dc', SH_ELF)
        self.assertEqual(status, 0, output)
        self.assertEqual({path[len('build/dc/'):] for path in commands(output)},
                         {'modules/shared/a.i', 'modules/shared/a.o', 'census/stg13.o'})

    def test_builds_a_play_object_again_once_its_source_moves_to_a_route_the_pass_reads_otherwise(self):
        # the aggressive route's release, includes and GCC flags are the game's, and only -aggressive=2, which the
        # pass reads, tells them apart: a.c's object builds again under it, and its census, and the pass takes the
        # flag
        routes = {**self.PARTS, 'config/compilers.mk': COMPILERS + (
            'aggressive.release  := R13\naggressive.flags    := -cpu=sh4 -speed -aggressive=2\n'
            'aggressive.includes := sdk/include,src,modules\naggressive.sources  :=\n'
            'game.sources := $(filter-out $(aggressive.sources),$(game.sources))\nROUTES += aggressive\n')}
        with stood_in(routes) as root:
            status, output = make(root, 'dc', SH_ELF)
            self.assertEqual(status, 0, output)
            settle(root)
            replace('config/compilers.mk', 'aggressive.sources  :=', 'aggressive.sources  := modules/shared/a.c')(root)
            status, output = make(root, '-n', 'dc', SH_ELF)
        self.assertEqual(status, 0, output)
        self.assertEqual({path[len('build/dc/'):] for path in commands(output)},
                         {'modules/shared/a.i', 'modules/shared/a.o', 'census/stg13.o'})
        self.assertIn('python3 tools/floats.py --shc r13 -aggressive=2 --system tools/sh4zam/include '
                      'build/dc/modules/shared/a.i', output)

    def test_stops_on_a_c_source_no_row_of_config_parts_txt_gives_a_bin(self):
        with stood_in({'config/parts.txt': 'src/ picture\n'}) as root:
            status, output = make(root, '-n', 'dc', SH_ELF)
        self.assertNotEqual(status, 0, output)
        self.assertIn('tools/parts.py: no bin for each C source of the routes', output)


class Census(unittest.TestCase):
    def test_compiles_each_images_objects_whole_and_writes_what_they_leave_undefined(self):
        with stood_in({**SOURCES, 'modules/shared/a.c': 'a\nU stray\n'}) as root:
            status, output = make(root, 'dc', SH_ELF)
            self.assertEqual(status, 0, output)
            census = root / 'build/dc/census'
            self.assertEqual((census / '1st_read.in').read_text(), 'build/dc/src/main.o\n')
            self.assertEqual((census / 'stg13.in').read_text(),
                             'build/dc/modules/shared/a.o build/dc/modules/stg13/b.o\n')
            # each image's link takes its objects, the archive and libgcc.a
            archive = 'build/dc/sh4zam/sh4/a.o\nbuild/dc/sh4zam/sh4/b.o\n'
            self.assertEqual((census / 'stg13.o').read_text(), f'a\nU stray\nb\n{archive}libgcc\n')
            self.assertEqual((census / '1st_read.o').read_text(), f'main\n{archive}libgcc\n')
            self.assertEqual((census / 'stg13.txt').read_text(), 'stray\n')
            self.assertEqual((census / '1st_read.txt').read_text(), '')
            status, output = make(root, '-n', '-W', 'src/main.c', '-W', 'modules/stg13/b.src', 'dc', SH_ELF)
        # the stage's census takes the main executable's names, and the main executable's census takes none
        self.assertIn('python3 tools/census.py build/dc/census/stg13.o build/dc/census/stg13.txt --main '
                      'build/dc/census/1st_read.o --symbols config/1st_read/symbols.txt\n', output)
        self.assertIn('python3 tools/census.py build/dc/census/1st_read.o build/dc/census/1st_read.txt\n', output)
        link = commands(output)['build/dc/census/stg13.o']
        self.assertEqual(link[link.index('-g0') + 1:], ['-r', '-nostdlib', '-flinker-output=nolto-rel', '-o',
                                                       'build/dc/census/stg13.o', '@build/dc/census/stg13.in',
                                                       'build/dc/sh4zam/libsh4zam.a', 'sh-elf/lib/gcc/sh-elf/*/libgcc.a'])
        self.assertIn('-flto-partition=1to1', link)


    def test_lto_budget_reaches_the_terminal_link(self):
        target = 'build/dc/census/stg13.o'
        with stood_in(SOURCES) as root:
            gcc = root / 'sh-elf/bin/sh-elf-gcc'
            # Only the actual terminal invocation has a response file.
            (root / 'pool.mk').write_text('all: ; @:\n')
            watch = ('case " $* " in *" @"*) printf "%s\\n" "$MAKEFLAGS" "$*" >> lto-runs; '
                     'if test -n "$MAKEFLAGS"; then "$MAKE" --no-print-directory -f pool.mk > pool-log 2>&1 '
                     '|| exit 1; fi ;; esac\n')
            gcc.write_text(gcc.read_text().replace('#!/bin/sh\n', '#!/bin/sh\n' + watch))
            seen = root / 'lto-runs'
            status, output = make(root, '-j2', SH_ELF, target)
            self.assertEqual(status, 0, output)
            flags, args = seen.read_text().splitlines()
            self.assertIn('--jobserver-', flags)
            # GNU make itself verifies that the descriptor or FIFO works.
            self.assertNotIn('jobserver unavailable', (root / 'pool-log').read_text())
            self.assertIn('-flto=1', args.split())
            seen.unlink()
            for mode, expected in (('-n', 0), ('-q', 1), ('-t', 0)):
                with self.subTest(mode=mode):
                    status, output = make(root, '-j2', mode, '-W', 'build/dc/modules/shared/a.o', SH_ELF, target)
                    self.assertEqual(status, expected, output)
                    self.assertFalse(seen.exists(), output)

            status, output = make(root, '-j2', SH_ELF, 'DC_LTO_JOBS=2', target)
            self.assertEqual(status, 0, output)
            flags, args = seen.read_text().splitlines()
            self.assertEqual(flags, '')
            self.assertIn('-flto=2', args.split())
            seen.unlink()
            status, output = make(root, '-j2', SH_ELF, 'DC_LTO_JOBS=2', target)
            self.assertEqual(status, 0, output)
            self.assertFalse(seen.exists(), output)
            status, output = make(root, SH_ELF, 'DC_LTO_JOBS=auto', target)
            self.assertNotEqual(status, 0, output)
            self.assertIn('positive decimal worker count', output)

    def test_disc_free_census_tracks_the_data_schema_and_filename_provider(self):
        provider = 'modules/shared/a'
        report = 'build/dc/check/census/stg13.txt'
        census = f'python3 tools/census.py build/dc/check/census/stg13.o {report}'
        files = {**SOURCES, 'config/bodies.mk': (ROOT / 'config/bodies.mk').read_text(),
                 f'{provider}.c': '#include "names.h"\nconst char names[4] = {\n#include "shared/a/names.inc"\n};\n',
                 'modules/shared/names.h': 'extern const char names[4];\n',
                 f'offsets/{provider}.bindings': '',
                 f'offsets/{provider}.txt': 'datum names C 0x00000000 4\n',
                 'config/stg13/data.txt': f'0x8c500200 names land_filenames {provider}.c 4\n',
                 'config/stg13/symbols.txt': '0x8c500200 names\n'}
        with stood_in(files) as root:
            append('config/stg13/splits.txt', f'C 0x8c500200 0x8c500204\n{provider}.c C 0x8c500200 0x8c500204\n')(root)
            status, output = make(root, SH_ELF, report)
            self.assertEqual(status, 0, output)
            # make --trace reports the exact stale prerequisite when an unchanged link runs.
            # Record both inventories before the dry run: make updates them while reading the rules.
            link = root / 'build/dc/check/census/stg13.o'
            stamps = (link.with_suffix('.in'), link.with_suffix('.cmd'))

            def identities():
                return [(path.name, path.stat().st_mtime_ns, path.read_bytes()) for path in stamps]

            before = identities()
            status, output = make(root, '--trace', '-n', SH_ELF, report)
            self.assertEqual(status, 0, output)
            if census in output:
                output += (f'\nStage link mtime (ns): {link.stat().st_mtime_ns}\n'
                           f'Inventory and command before: {before!r}\nAfter: {identities()!r}')
            self.assertNotIn(census, output)
            for source in ('tools/land_data.py', 'tools/cart_data.py', 'tools/chunk_poses.py', 'tools/layout.py',
                           'modules/shared/names.h', f'offsets/{provider}.txt', f'offsets/{provider}.bindings'):
                with self.subTest(source=source):
                    status, output = make(root, '-n', '-W', source, SH_ELF, report)
                    self.assertEqual(status, 0, output)
                    self.assertIn(census, output)
            (root / f'offsets/{provider}.bindings').unlink()
            status, output = make(root, '-n', SH_ELF, report)
            self.assertEqual(status, 0, output)
            self.assertIn(census, output)
            (root / f'offsets/{provider}.txt').unlink()
            status, output = make(root, SH_ELF, report)
            self.assertNotEqual(status, 0, output)
            self.assertIn(f'offsets/{provider}.txt', output)

    def test_generates_the_content_again_once_an_optional_table_leaves(self):
        provider = 'modules/shared/a'
        content, decoded = 'tools/content.py stg13', 'build/orig/SONIC2/STG13.BIN'
        files = {**SOURCES, 'config/bodies.mk': (ROOT / 'config/bodies.mk').read_text(),
                 f'{provider}.c': 'const char names[4];\n', f'offsets/{provider}.txt': 'datum names C 0x00000000 4\n',
                 'config/stg13/data.txt': f'0x8c500200 names land_filenames {provider}.c 4\n',
                 'config/stg13/symbols.txt': '0x8c500200 names\n', 'orig/SONIC2/STG13.PRS': ''}
        with stood_in(files) as root:
            status, output = make(root, SH_ELF, '-n', '-o', decoded, 'build/dc/content/stg13.s')
            self.assertEqual(status, 0, output)
            self.assertIn(content, output)
            (root / 'build/dc/content').mkdir(parents=True, exist_ok=True)
            (root / 'build/dc/content/stg13.s').write_text('')
            settle(root)
            status, output = make(root, SH_ELF, '-n', '-o', decoded, 'build/dc/content/stg13.s')
            self.assertEqual(status, 0, output)
            self.assertNotIn(content, output)
            (root / f'offsets/{provider}.txt').unlink()
            status, output = make(root, SH_ELF, '-n', '-o', decoded, 'build/dc/content/stg13.s')
        self.assertEqual(status, 0, output)
        self.assertIn(content, output)

    def test_make_43_retained_newline_does_not_rewrite_a_census_command(self):
        with stood_in(SOURCES) as root:
            # Exercise the 4.3 read result without depending on the host allocator or make version.
            source = root / 'Makefile'
            text = source.read_text()
            wanted = 'build/dc/census/stg13.cmd'
            start = 'keep = '
            before, line = text.split(start, 1)
            rule, after = line.split('\n', 1)
            retained = '$(if $(filter ' + wanted + ',$1),$(retained_newline))'
            source.write_text(before + 'define retained_newline\n\n\nendef\n' + start
                              + rule.replace('$(file <$1)', '$(file <$1)' + retained) + '\n' + after)
            settle(root)
            args = (SH_ELF, '--eval=MAKE_VERSION := 4.3', 'build/dc/census/stg13.txt')
            status, output = make(root, *args)
            self.assertEqual(status, 0, output)
            stamp = root / wanted
            command = stamp.read_bytes()
            original_time = stamp.stat().st_mtime_ns
            status, output = make(root, '-n', *args)
            self.assertEqual(status, 0, output)
            self.assertNotIn('python3 tools/census.py', output)
            self.assertEqual(stamp.stat().st_mtime_ns, original_time)
            self.assertEqual(stamp.read_bytes(), command)
            # A real difference still invalidates the link, including whitespace inside the command.
            self.assertIn(b' -O2 ', command)
            stamp.write_bytes(command.replace(b' -O2 ', b'  -O2 ', 1))
            status, output = make(root, '-n', *args)
            self.assertEqual(status, 0, output)
            self.assertIn('python3 tools/census.py', output)
            self.assertEqual(stamp.read_bytes(), command)

    def test_takes_a_name_the_main_executables_table_gains_out_of_each_other_census_and_links_nothing(self):
        with stood_in(SOURCES) as root:
            status, output = make(root, 'dc', SH_ELF)
            self.assertEqual(status, 0, output)
            settle(root)
            append('config/1st_read/symbols.txt', '0x8c010100 other\n')(root)
            status, output = make(root, '-n', 'dc', SH_ELF)
        self.assertEqual(status, 0, output)
        self.assertEqual(commands(output), {}, output)
        self.assertIn('python3 tools/census.py build/dc/census/stg13.o build/dc/census/stg13.txt', output)
        self.assertNotIn('python3 tools/census.py build/dc/census/1st_read.o', output)

    def test_writes_an_empty_census_for_an_image_that_places_no_source(self):
        title = {'config/title/splits.txt': 'file SONIC2/TITLE.PRS\nP 0x8c500000 0x8c500100\n'}
        with stood_in(title) as root:
            status, output = make(root, 'dc', SH_ELF)
            self.assertEqual(status, 0, output)
            self.assertEqual((root / 'build/dc/census/title.in').read_text(), '\n')
            self.assertTrue((root / 'build/dc/census/title.txt').exists())


class Check(unittest.TestCase):
    def test_layout_only_bodies_stay_out_of_the_production_link(self):
        stage = 'modules/shared/a'
        check_obj, real_obj = f'build/dc/check/{stage}.o', f'build/dc/{stage}.o'
        # make takes a.c for a body source, though it includes no body, and config/bodies.mk gives only the two
        # functions dc.mk reads. a.c is no C, so a polygon_lists that declares nothing stands in
        settings = (SH_ELF, f'--eval=override BODY_C := {stage}.c')
        bodies = f'body_root = $2/modules\nbody_includes = $2/{stage}/values.inc\n'
        lists = "import sys\nfrom pathlib import Path\nPath(sys.argv[2]).write_text('')\n"
        with stood_in({**SOURCES, 'config/bodies.mk': bodies, f'build/stub/{stage}/values.inc': '0\n',
                       'tools/polygon_lists.py': lists}) as root:
            status, output = make(root, 'dc-check', *settings)
            self.assertEqual(status, 0, output)
            self.assertTrue((root / check_obj).is_file())
            self.assertFalse((root / real_obj).exists())
            self.assertFalse((root / 'build/dc/check/census/1st_read.o').exists())
            checked, real = (root / f'build/dc/{sub}census/stg13.in' for sub in ('check/', ''))
            self.assertEqual(checked.read_text(), real.read_text().replace(real_obj, check_obj))
            status, output = make(root, 'build/dc/census/stg13.o', *settings)
            self.assertNotEqual(status, 0, output)
            self.assertIn(f'build/gen/{stage}/values.inc', output)
            body = root / f'build/gen/{stage}/values.inc'
            body.parent.mkdir(parents=True)
            body.write_text('42\n')
            status, output = make(root, '-n', '-W', f'{stage}.c', 'dc-check', 'build/dc/census/stg13.o', *settings)
            self.assertEqual(status, 0, output)
            built = commands(output)
            self.assertIn('-Ibuild/stub/modules', built[check_obj])
            self.assertIn('-Ibuild/gen/modules', built[real_obj])
            self.assertNotIn('-Ibuild/stub/modules', built[real_obj])
            # each object compiles the source's polygon list declarations, with the source included ahead of them
            for obj in (check_obj, real_obj):
                self.assertEqual(built[obj][-3:], ['-include', f'{stage}.c', f'build/dc/{stage}.lists.c'])

    def test_fails_on_a_number_for_ram_that_no_row_of_config_addresses_txt_explains(self):
        # a header and a stub body each hold one. make dc-check reads the stub body, and make dc the full build's.
        # The check reads the body's kind from a.c's declarations. make fails on a source that includes a body
        # but has no offsets table, so a.c has an empty one
        stage = 'modules/shared/a'
        settings = (SH_ELF, f'--eval=override BODY_C := {stage}.c')
        bodies = f'body_root = $2/modules\nbody_includes = $2/{stage}/values.inc\n'
        files = {**SOURCES, f'{stage}.c': 'long values[1] = {\n#include "shared/a/values.inc"\n};\n',
                 f'offsets/{stage}.txt': '', 'config/bodies.mk': bodies,
                 'src/areas.h': '#define SLOT ((char *)0x8c500000)\n',
                 f'build/stub/{stage}/values.inc': '0x8c500000\n', f'build/gen/{stage}/values.inc': '0\n'}
        with stood_in(files) as root:
            status, output = make(root, 'dc-check', *settings)
            self.assertNotEqual(status, 0, output)
            self.assertIn('src/areas.h:1: 0x8c500000, which no row of config/addresses.txt explains', output)
            self.assertIn(f'build/stub/{stage}/values.inc:1: 0x8c500000, which no row', output)
            append('config/addresses.txt', 'src/areas.h  0x8c500000  1  anchor  SLOT  the slot\n'
                   f'build/stub/{stage}/values.inc  0x8c500000  1  data  a value\n')(root)
            status, output = make(root, 'dc-check', *settings)
            self.assertEqual(status, 0, output)
            self.assertIn('  addrs  config/addresses.txt\n', output)
            status, output = make(root, 'dc', *settings)
            self.assertEqual(status, 0, output)
            self.assertIn('  addrs  config/addresses.txt\n', output)
            status, output = make(root, '-n', '-W', 'src/areas.h', 'dc', *settings)
        self.assertEqual(status, 0, output)
        self.assertIn('python3 tools/addresses.py --bodies build/dc/addresses/gen.txt ', output)

    def test_sums_input_and_terminal_objects_reports_link_commands_and_tool_identities(self):
        with stood_in(SOURCES) as root:
            status, output = make(root, 'dc-check', SH_ELF)
            self.assertEqual(status, 0, output)
            lines = (root / 'build/dc/check.txt').read_text().splitlines()
        tools = ['bin/sh-elf-' + name for name in ('as', 'gcc', 'ld', 'ar', 'objcopy')]
        tools += [f'{LIBEXEC}/{name}' for name in ('cc1', 'lto1', 'lto-wrapper', 'collect2', 'liblto_plugin.so')]
        expected = [f'sh-elf/{name}' for name in [*tools, LIBGCC]]
        expected += [f'build/dc/census/{image}.{ext}' for image in ('1st_read', 'stg13') for ext in ('cmd', 'o', 'txt')]
        expected += ['build/dc/modules/shared/a.o', 'build/dc/modules/stg13/b.o', 'build/dc/sh4zam/libsh4zam.a',
                     'build/dc/sh4zam/sh4/a.o', 'build/dc/sh4zam/sh4/b.o', 'build/dc/src/main.o']
        self.assertEqual([line.split('  ')[1] for line in lines], sorted(expected))

    def test_fails_on_each_name_a_census_holds_that_no_row_of_its_image_expects(self):
        # the main executable and the stage each leave shared_name undefined, and the stage leaves stage_name too
        files = {**SOURCES, 'src/main.c': 'main\nU shared_name\n',
                 'modules/shared/a.c': 'a\nU shared_name\nU stage_name\n',
                 'config/imports.txt': '# image  name  reason\nstg13  stage_name  a test\nstg13  shared_name  a test\n'}
        with stood_in(files) as root:
            status, output = make(root, 'dc-check', SH_ELF)
            self.assertNotEqual(status, 0, output)
            self.assertIn('build/dc/census/1st_read.txt: shared_name: undefined, and no row of config/imports.txt '
                          'gives it for 1st_read\n', output)
            self.assertNotIn('census/stg13.txt', output)
            self.assertNotIn('  census ', output)
            append('config/imports.txt', '1st_read  shared_name  a test\n')(root)
            status, output = make(root, 'dc-check', SH_ELF)
        self.assertEqual(status, 0, output)
        self.assertTrue(output.endswith('  census 3 names in 2 censuses, 2 of them distinct\n'), output)

    def test_fails_on_each_row_whose_name_its_images_census_doesnt_hold_and_on_a_row_without_a_reason(self):
        files = {**SOURCES, 'config/imports.txt': '# image  name  reason\n\nstg13  gone  a test\n1st_read  bare\n'}
        with stood_in(files) as root:
            status, output = make(root, 'dc-check', SH_ELF)
        self.assertNotEqual(status, 0, output)
        self.assertIn('config/imports.txt:3: gone: the census of stg13 holds no such name\n', output)
        self.assertIn('config/imports.txt:4: not a row: IMAGE NAME REASON\n', output)
        self.assertNotIn(': bare:', output)

    def test_stops_the_sums_on_a_stamp_line_whose_path_doesnt_open_with_sh_elfs(self):
        # the shell that refreshes GCC's stamp expands SH_ELF's glob, sh-el[f], to sh-elf, the name it matches
        with stood_in(SOURCES) as root:
            status, output = make(root, 'dc-check', 'SH_ELF=sh-el[f]')
            self.assertNotEqual(status, 0, output)
            self.assertIn('  sums   build/dc/check.txt\nbuild/tools/gcc.id:1: not a hash and a path under sh-el[f]/\n',
                          output)
            self.assertFalse((root / 'build/dc/check.txt').exists())

    def test_jobserver_and_one_private_worker_have_distinct_link_receipts(self):
        census = 'build/dc/census/'
        kinder = census + 'chaostgkinder'
        stamps = {census + '1st_read.cmd', census + 'stg13.cmd', kinder + '.cmd', kinder + '-layout.cmd'}
        links = {census + '1st_read.o', census + 'stg13.o', kinder + '.o', kinder + '-sdk.o', kinder + '-layout.o'}
        files = {**SOURCES, 'config/chaostgkinder/bss.ld': '',
                 'config/chaostgkinder/splits.txt': ('file SONIC2/CHAOSTGKINDER.BIN\n'
                                                     'P 0x8c900000 0x8c900040\nPSG 0x8c900040 0x8c900080\n'
                                                     'modules/shared/a.c P 0x8c900000 0x8c900040\n'
                                                     'modules/stg13/b.src PSG 0x8c900040 0x8c900080\n')}
        with stood_in(files) as root:
            def check(budget, expected_links):
                status, output = make(root, '-j2', 'V=1', SH_ELF, 'DC_LTO_JOBS=' + budget, 'dc-check')
                self.assertEqual(status, 0, output)
                self.assertEqual(links.intersection(commands(output)), expected_links, output)
                return sums(root)

            # Both modes pass -flto=1, but only the first inherits Make's pool.
            shared = check('jobserver', links)
            private = check('1', links)
            self.assertEqual(shared.keys(), private.keys())
            self.assertEqual({path for path in shared if shared[path] != private[path]}, stamps)
            self.assertEqual(check('1', set()), private)
            self.assertEqual(check('jobserver', links), shared)

    def test_tool_identity_and_link_flags_survive_a_toolchain_move(self):
        with tempfile.TemporaryDirectory() as outside, stood_in(SOURCES) as root:
            prefix = Path(outside) / 'first'
            (root / 'sh-elf').rename(prefix)

            def receipt():
                status, output = make(root, 'dc-check', f'SH_ELF={prefix}')
                self.assertEqual(status, 0, output)
                return sums(root)

            original = receipt()
            prefix = prefix.rename(Path(outside) / 'second')
            self.assertEqual(receipt(), original)
            tool = f'{LIBEXEC}/lto1'
            (prefix / tool).write_text('new link-time compiler\n')
            changed = receipt()
            self.assertEqual([p for p in original if changed[p] != original[p]], [f'sh-elf/{tool}'])
            replace('config/gcc.mk', '-O2 -flto=', '-O1 -flto=')(root)
            flags = receipt()
            self.assertEqual([p for p in changed if flags[p] != changed[p]],
                             ['build/dc/census/1st_read.cmd', 'build/dc/census/stg13.cmd'])


class Sh4zam(unittest.TestCase):
    def test_compiles_the_sh_4_back_end_alone_and_archives_it(self):
        with stood_in() as root:
            status, output = make(root, 'dc', SH_ELF)
            self.assertEqual(status, 0, output)
            self.assertEqual((root / 'build/dc/sh4zam/libsh4zam.a').read_text(),
                             'build/dc/sh4zam/sh4/a.o\nbuild/dc/sh4zam/sh4/b.o\n')
            self.assertFalse((root / 'build/dc/sh4zam/sw').exists())
            status, output = make(root, '-n', '-W', 'tools/sh4zam/source/sh4/a.c', 'dc', SH_ELF)
        words = commands(output)['build/dc/sh4zam/sh4/a.o']
        self.assertIn('-Isdk/include', words)
        self.assertEqual([word for word in words if word.startswith('-O')], ['-O2'])
        self.assertNotIn('-flto', words)

    def test_copies_each_file_whose_hash_matches_from_sh4zam_and_fetches_the_others(self):
        copy = {f'copy/{path[len("tools/sh4zam/"):]}': text for path, text in LIBRARY.items()}
        unfetched = {path: None for path in LIBRARY}
        for name, altered, fetches in (('all match', {}, False), ('one differs', {'copy/LICENSE': 'other\n'}, True)):
            with self.subTest(name), stood_in({**copy, **altered, 'tools/sh4zam/fetched.txt': ''}) as root:
                for path in unfetched:
                    (root / path).unlink()
                curl = '#!/bin/sh\necho "curl $*" >&2\nexit 1\n'
                (root / 'bin').mkdir()
                (root / 'bin/curl').write_text(curl)
                (root / 'bin/curl').chmod(0o755)
                with host() as path:
                    status, output = make(root, 'dc', SH_ELF, 'SH4ZAM=copy', path=f'{root / "bin"}:{path}')
                self.assertEqual(status != 0, fetches, output)
                downloads = [shlex.split(line) for line in output.splitlines() if line.startswith('curl ')]
                self.assertEqual(len(downloads), int(fetches), output)
                if fetches:
                    self.assertEqual(downloads[0][-3:-1], ['-o', 'tools/sh4zam/LICENSE.part'])
                    self.assertRegex(downloads[0][-1], r'^https://raw\.githubusercontent\.com/'
                                     r'gyrovorbis/sh4zam/[0-9a-f]{40}/LICENSE$')
                self.assertEqual((root / 'tools/sh4zam/fetched.txt').exists(), not fetches)
                if not fetches:
                    self.assertEqual((root / 'tools/sh4zam/source/sh4/a.c').read_text(), 'source/sh4/a.c\n')

    def test_copies_each_file_from_sh4zam_with_a_sha256sum_like_macos_s(self):
        # each file of the copy matches its line, so make fetches none
        copy = {f'copy/{path[len("tools/sh4zam/"):]}': text for path, text in LIBRARY.items()}
        with stood_in({**copy, 'tools/sh4zam/fetched.txt': ''}) as root:
            for path in LIBRARY:
                (root / path).unlink()
            stand_in(root, 'bin/curl', RAN)
            status, output = make(root, 'dc', SH_ELF, 'SH4ZAM=copy', path=macos_sha256sum(root), uname=MACS[0])
            self.assertEqual(status, 0, output)
            self.assertEqual({path: (root / path).read_text() for path in LIBRARY}, LIBRARY)
            self.assertFalse((root / 'ran').exists())

    def test_fetches_every_file_again_for_another_pin(self):
        with stood_in({'tools/sh4zam/old.h': '', 'tools/sh4zam/fetched.txt': 'another pin\n'}) as root:
            status, output = make(root, '-n', 'dc', SH_ELF)
        self.assertEqual(status, 0, output)
        self.assertEqual(output.count("'fetch' 'tools/sh4zam/"), len(LIBRARY), output)

    def test_removes_each_file_and_link_the_pin_no_longer_lists_by_its_whole_path(self):
        copy = {f'copy/{path[len("tools/sh4zam/"):]}': text for path, text in LIBRARY.items()}
        with stood_in({**copy, 'tools/sh4zam/old notes.txt': '', 'notes.txt': 'kept\n', 'elsewhere.h': 'kept\n',
                       'tools/sh4zam/fetched.txt': 'another pin\n'}) as root:
            (root / 'tools/sh4zam/include/old.h').symlink_to('../../../elsewhere.h')
            status, output = make(root, 'tools/sh4zam/fetched.txt', SH_ELF, 'SH4ZAM=copy')
            self.assertEqual(status, 0, output)
            self.assertFalse((root / 'tools/sh4zam/old notes.txt').exists())
            self.assertFalse((root / 'tools/sh4zam/include/old.h').is_symlink())
            self.assertEqual([(root / path).read_text() for path in ('notes.txt', 'elsewhere.h')], ['kept\n'] * 2)
            self.assertEqual({path: (root / path).read_text() for path in LIBRARY}, LIBRARY)
            self.assertEqual((root / 'tools/sh4zam/fetched.txt').read_text(), SUMS)

    def test_removes_nothing_where_tools_sha256sums_names_no_file_of_sh4zams(self):
        for name, sums in (('missing', None), ('no line', '')):
            with self.subTest(name), stood_in() as root:
                (root / 'tools/sh4zam/fetched.txt').unlink()
                if sums is None:
                    (root / 'tools/SHA256SUMS').unlink()
                else:
                    (root / 'tools/SHA256SUMS').write_text(sums)
                status, output = make(root, 'tools/sh4zam/fetched.txt', SH_ELF)
                self.assertNotEqual(status, 0, output)
                self.assertIn('tools/SHA256SUMS: no line for a file under tools/sh4zam/, so make removes none', output)
                self.assertEqual({path: (root / path).read_text() for path in LIBRARY}, LIBRARY)

    def test_writes_no_record_of_the_fetch_over_a_file_edited_since(self):
        with stood_in() as root:
            (root / 'tools/sh4zam/include/sh4zam/a.h').write_text('edited\n')
            status, output = make(root, 'tools/sh4zam/fetched.txt', SH_ELF)
            self.assertNotEqual(status, 0, output)
            self.assertIn('tools/sh4zam: a file differs from its line of tools/SHA256SUMS; make fetches every file '
                          'again', output)
            self.assertFalse((root / 'tools/sh4zam/fetched.txt').exists())
            status, output = make(root, '-n', 'dc', SH_ELF)
        self.assertEqual(status, 0, output)
        self.assertEqual(output.count("'fetch' 'tools/sh4zam/"), len(LIBRARY), output)

    def test_assembles_a_pins_assembly_though_its_old_c_of_one_stem_is_still_in_place(self):
        with stood_in({'tools/sh4zam/source/sh4/b.c': 'b\n'}) as root:
            status, output = make(root, '-n', 'dc', SH_ELF)
        self.assertEqual(status, 0, output)
        self.assertEqual(commands(output)['build/dc/sh4zam/sh4/b.o'][0], 'sh-elf/bin/sh-elf-as')


class Toolchain(unittest.TestCase):
    def test_toolchain_recipe_can_be_read_without_game_link_settings(self):
        target = 'tools/sh-elf/bin/sh-elf-gcc'
        with stood_in({'config/gcc.mk': '$(error read game link settings)\n'}) as root:
            status, output = make(root, '-nB', target)
            self.assertEqual(status, 0, output)
            self.assertIn('--target=sh-elf', output)
            self.assertFalse((root / target).exists())

    def test_writes_the_host_into_the_toolchains_receipt(self):
        # the Makefile asks uname for the host before it reads dc.mk, and the receipt holds it
        with stood_in() as root:
            for uname in (LINUX, *MACS):
                with self.subTest(uname):
                    status, output = make(root, '-nB', 'tools/sh-elf/bin/sh-elf-gcc', uname=uname)
                    self.assertEqual(status, 0, output)
                    self.assertIn(f' host {uname} binutils ', output)

    def test_refuses_a_toolchain_built_from_another_request_and_leaves_it_in_place(self):
        pinned = {f'tools/sh-elf/{path}': '' for path in ('bin/sh-elf-gcc', *SH_ELF_FILES)}
        with stood_in({**pinned, 'tools/sh-elf/request.txt': 'another build\n'}) as root:
            status, output = make(root, '-n', 'dc')
            self.assertTrue((root / 'tools/sh-elf/bin/sh-elf-gcc').exists())
        self.assertNotEqual(status, 0, output)
        self.assertIn('tools/sh-elf: its request.txt names another build than dc.mk\'s; remove tools/sh-elf', output)

    def test_refuses_a_toolchain_without_a_program_of_the_link_time_compile_and_names_it(self):
        with stood_in() as root:
            (root / f'sh-elf/{LIBEXEC}/lto1').unlink()
            status, output = make(root, '-n', 'dc', SH_ELF)
        self.assertNotEqual(status, 0, output)
        self.assertIn('sh-elf: its libexec/gcc/sh-elf/*/ holds no lto1, which make dc\'s link-time compile runs; '
                      'SH_ELF must name a toolchain whose GCC was configured with --enable-lto', output)

    def test_refuses_a_toolchain_of_more_than_one_gcc_version_and_names_each(self):
        # the rules name GCC's headers, cc1 and libgcc.a by a glob that takes one version
        with stood_in({'sh-elf/lib/gcc/sh-elf/16.1.0/include/stddef.h': ''}) as root:
            status, output = make(root, '-n', 'dc', SH_ELF)
        self.assertNotEqual(status, 0, output)
        self.assertRegex(output, r'\Adc\.mk:\d+: \*\*\* sh-elf: its lib/gcc/sh-elf/ holds more than one version of '
                                 r'GCC, (16\.1\.0 16\.2\.0|16\.2\.0 16\.1\.0), and make dc takes a toolchain of one\.  '
                                 r'Stop\.\n\Z')

    def test_fetches_the_toolchains_sources_as_tools_sha256sums_pins_them(self):
        pins = ''.join(line for line in (ROOT / 'tools/SHA256SUMS').read_text().splitlines(True)
                       if ' tools/sh-elf.src/' in line)
        with stood_in({'tools/SHA256SUMS': SUMS + pins, 'bin/xz': '#!/bin/sh\n', 'bin/c++': '#!/bin/sh\n'}) as root, \
                host() as path:
            status, output = make(root, '-n', 'dc', path=f'{root / "bin"}:{path}')
        self.assertEqual(status, 0, output)
        for source in ('binutils/binutils-2.47.tar.xz', 'gcc/gcc-16.2.0/gcc-16.2.0.tar.xz', 'mpc/mpc-1.3.1.tar.gz'):
            self.assertIn(f'https://ftp.gnu.org/gnu/{source}', output)
        self.assertIn('--enable-lto', output)


class Composition(unittest.TestCase):
    def test_compiles_a_replacement_in_its_sources_place_and_an_addition_into_its_images_census(self):
        composed = {'config/dreamcast.mk': 'DC_REPLACED := modules/stg13/b.src:engine/b.c\nDC_ADDED := stg13:engine/c.c\n',
                    'engine/b.c': 'b in C\n', 'engine/c.c': 'c\n'}
        with stood_in(composed) as root:
            status, output = make(root, 'dc', SH_ELF)
            self.assertEqual(status, 0, output)
            self.assertEqual((root / 'build/dc/modules/stg13/b.o').read_text(), 'b in C\n')
            self.assertEqual((root / 'build/dc/census/stg13.in').read_text(),
                             'build/dc/modules/shared/a.o build/dc/modules/stg13/b.o build/dc/engine/c.o\n')
        self.assertIn('  gcc    engine/b.c\n', output)
        self.assertNotIn('  gas    modules/stg13/b.src\n', output)

    def test_game_include_order_change_rebuilds_replacement_and_added_objects(self):
        composed = {'config/dreamcast.mk': 'DC_REPLACED := modules/stg13/b.src:engine/b.c\nDC_ADDED := stg13:engine/c.c\n',
                    'engine/b.c': 'b in C\n', 'engine/c.c': 'c\n'}
        objects = ('build/dc/modules/stg13/b.o', 'build/dc/engine/c.o')
        with stood_in(composed) as root:
            status, output = make(root, SH_ELF, *objects)
            self.assertEqual(status, 0, output)
            settle(root)
            replace('config/compilers.mk', 'sdk/include,src,modules', 'modules,src,sdk/include')(root)
            status, output = make(root, '-n', SH_ELF, *objects)
        self.assertEqual(status, 0, output)
        compiled = commands(output)
        self.assertEqual(set(compiled), set(objects), output)
        for obj in objects:
            self.assertEqual([word for word in compiled[obj] if word.startswith('-I')],
                             ['-Imodules', '-Isrc', '-Isdk/include', '-I.'])

    def test_compiles_a_source_the_main_executable_adds_that_nothing_calls(self):
        # the Dreamcast engine's kernels: two sources of engine/dc/, each including its header by a quoted name,
        # which the main executable adds and no source of the game calls yet
        kernels = {'config/dreamcast.mk': ('DC_REPLACED :=\n'
                                           'DC_ADDED := 1st_read:engine/dc/xmtrx.c 1st_read:engine/dc/turn.c\n'),
                   'engine/dc/xmtrx.h': '', 'engine/dc/xmtrx.c': 'xmtrx\n', 'engine/dc/turn.c': 'turn\n'}
        with stood_in({**SOURCES, **kernels}) as root:
            status, output = make(root, 'dc', SH_ELF)
            self.assertEqual(status, 0, output)
            self.assertEqual((root / 'build/dc/engine/dc/turn.o').read_text(), 'turn\n')
            self.assertEqual((root / 'build/dc/census/1st_read.in').read_text(),
                             'build/dc/src/main.o build/dc/engine/dc/xmtrx.o build/dc/engine/dc/turn.o\n')
            self.assertTrue((root / 'build/dc/census/1st_read.o').read_text().startswith('main\nxmtrx\nturn\n'))
            status, output = make(root, '-n', '-W', 'engine/dc/xmtrx.c', 'dc', SH_ELF)
        words = commands(output)['build/dc/engine/dc/xmtrx.o']
        self.assertEqual([word for word in words if word.startswith('-I')],
                         ['-Isdk/include', '-Isrc', '-Imodules', '-I.'])
        self.assertIn('-frandom-seed=engine/dc/xmtrx.c', words)
        self.assertEqual(words[-1], 'engine/dc/xmtrx.c')

    def test_compiles_once_a_source_two_images_add(self):
        # the crash screen, which the main executable adds and so does an overlay that boots under the retail one
        composed = {'config/dreamcast.mk': 'DC_REPLACED :=\nDC_ADDED := 1st_read:engine/c.c stg13:engine/c.c\n',
                    'engine/c.c': 'c\n'}
        with stood_in({**SOURCES, **composed}) as root:
            status, output = make(root, 'dc', SH_ELF)
            self.assertEqual(status, 0, output)
            self.assertEqual(output.count('  gcc    engine/c.c\n'), 1, output)
            for image, objects in (('1st_read', 'build/dc/src/main.o'),
                                   ('stg13', 'build/dc/modules/shared/a.o build/dc/modules/stg13/b.o')):
                self.assertEqual((root / f'build/dc/census/{image}.in').read_text(), f'{objects} build/dc/engine/c.o\n')

    def test_stops_on_an_added_c_that_compiles_to_a_sources_object(self):
        with stood_in({'config/dreamcast.mk': 'DC_REPLACED :=\nDC_ADDED := stg13:src/main.c\n'}) as root:
            status, output = make(root, '-n', 'dc', SH_ELF)
        self.assertNotEqual(status, 0, output)
        self.assertRegex(output, r"\Adc\.mk:\d+: \*\*\* build/dc/src/main\.o: two of make dc's rules build it, by "
                                 r"DC_C_OBJ DC_PAIRED_OBJ\.  Stop\.\n\Z")

    def test_stops_on_an_entry_that_names_no_source_or_no_image(self):
        for text, refused in (('DC_REPLACED := src/none.c:engine/b.c\n', 'src/none.c:engine/b.c: an entry of '
                               'DC_REPLACED'), ('DC_ADDED := stg99:engine/b.c\n', 'stg99:engine/b.c: an entry of '
                                                'DC_ADDED'),
                              ('DC_ADDED := %:engine/b.c\n', '%:engine/b.c: an entry of DC_ADDED'),
                              ('DC_ADDED := stg13::engine/b.c\n', 'stg13::engine/b.c: an entry of DC_ADDED'),
                              ('DC_REPLACED := src/main.c::engine/b.c\n', 'src/main.c::engine/b.c: an entry of '
                               'DC_REPLACED')):
            with self.subTest(text), stood_in({'config/dreamcast.mk': text, 'engine/b.c': ''}) as root:
                status, output = make(root, '-n', 'dc', SH_ELF)
                self.assertNotEqual(status, 0, output)
                self.assertIn(refused, output)


# two sources of C and SH4ZAM's library as make dc builds them with GCC, for
# Checkouts: a function, a constant and an initialized datum, one of them
# from a header, and the library's one C source and its assembly, empty
COMPILED = {'src/main.c': ('#include <a.h>\nstatic const char name[] = "main";\n'
                           'const char *main_name(void) { return name; }\nint main_count = A;\n'),
            'sdk/include/a.h': '#define A 3\n',
            'modules/shared/a.c': 'int shared_value = 1;\nint shared(void) { return shared_value; }\n',
            'tools/sh4zam/source/sh4/a.c': 'int shz_a(void) { return 1; }\n', 'tools/sh4zam/source/sh4/b.s': ''}
COMPILED_SUMS = ''.join(f'{hashlib.sha256(text.encode()).hexdigest()}  {path}\n'
                        for path, text in {**LIBRARY, **COMPILED}.items() if path.startswith('tools/sh4zam/'))


class Checkouts(unittest.TestCase):
    def test_two_checkouts_of_one_tree_at_two_paths_write_the_same_sums(self):
        # GCC writes each compile's working directory into an object's intermediate code, so the objects differ;
        # their sums, without it, don't
        if not (TOOLCHAIN / 'bin/sh-elf-gcc').is_file():
            self.skipTest('no SH-4 GCC at SH_ELF')
        tree = {**COMPILED, 'tools/census.py': (ROOT / 'tools/census.py').read_text(),
                'tools/SHA256SUMS': COMPILED_SUMS, 'tools/sh4zam/fetched.txt': COMPILED_SUMS}
        sums, objects = [], []
        for _ in range(2):
            with stood_in(tree) as root, tempfile.TemporaryDirectory() as outside:
                prefix = Path(outside) / 'sh-elf'
                prefix.symlink_to(TOOLCHAIN, target_is_directory=True)
                status, output = make(root, 'dc-check', f'SH_ELF={prefix}')
                self.assertEqual(status, 0, output)
                sums.append((root / 'build/dc/check.txt').read_text())
                objects.append((root / 'build/dc/src/main.o').read_bytes())
        self.assertNotEqual(objects[0], objects[1])
        self.assertEqual(sums[0], sums[1])
        self.assertIn('build/dc/src/main.o', sums[0])
        self.assertIn('build/dc/census/stg13.o', sums[0])

    def test_terminal_code_changes_its_sum_without_changing_the_report(self):
        if not (TOOLCHAIN / 'bin/sh-elf-gcc').is_file():
            self.skipTest('no SH-4 GCC at SH_ELF')
        tree = {**COMPILED, 'tools/census.py': (ROOT / 'tools/census.py').read_text(),
                'tools/SHA256SUMS': COMPILED_SUMS, 'tools/sh4zam/fetched.txt': COMPILED_SUMS}
        with stood_in(tree) as root:
            def receipt():
                status, output = make(root, 'dc-check', f'SH_ELF={TOOLCHAIN}')
                self.assertEqual(status, 0, output)
                return sums(root)

            before = receipt()
            stage = 'build/dc/census/stg13.o'
            self.assertEqual(before[stage], hashlib.sha256((root / stage).read_bytes()).hexdigest())
            code = root / 'code.bin'
            copy = TOOLCHAIN / 'bin/sh-elf-objcopy'
            subprocess.run([copy, '--dump-section', f'.text.shared={code}', stage], cwd=root, check=True)
            section = bytearray(code.read_bytes())
            self.assertGreaterEqual(len(section), 2)
            section[0] ^= 1
            code.write_bytes(section)
            subprocess.run([copy, '--update-section', f'.text.shared={code}', stage], cwd=root, check=True)
            after = receipt()
            self.assertEqual(before.keys(), after.keys())
            self.assertEqual([path for path in before if before[path] != after[path]], [stage])


# CHAO, placing one source, with the rules and the script its census reads, and the race, an area of the disc's;
# the archives' check reads both images' symbols
CHAO = {'config/chao/splits.txt': ('file SONIC2/CHAO.PRS\nP 0x0c500000 0x0c500040\n'
                                   'modules/shared/a.c P 0x0c500000 0x0c500040\n'),
        'config/chaostgrace/splits.txt': ('file SONIC2/CHAOSTGRACE.PRS\nP 0x0c9c0000 0x0c9c0040\n'
                                          'disc P 0x0c9c0000 0x0c9c0040\n'),
        'config/chao/symbols.txt': '', 'config/chaostgrace/symbols.txt': '', 'config/chao/resident.ld': '',
        'config/chao/areas.mk': (ROOT / 'config/chao/areas.mk').read_text(), 'sdk/include/ninja.h': '',
        'sdk/include/sg_xpt.h': ''}
ARCHIVES = ('SONIC2/CHAO_MODEL.PRS', 'SONIC2/CHAO_MOTION.PRS')
# build/gdi extract IMAGE DIRECTORY PATH..., which writes each path as IMAGE's name and fails on one that exists
GDI = ('#!/bin/sh\nimage=$2 into=$3\nshift 3\nfor path; do\n'
       '    [ ! -e "$into/$path" ] || { echo "gdi: $into/$path: can\'t write the file: File exists" >&2; exit 2; }\n'
       '    mkdir -p "$(dirname "$into/$path")" && echo "$image" > "$into/$path" || exit 2\ndone\n')


class ChaoArchives(unittest.TestCase):
    def test_checks_the_archives_orig_or_the_disc_gives_and_says_it_skips_them_without(self):
        # orig/'s archives, read in place, or else DISC_FILES's or a .gdi's, taken into build/dc/chao/
        taken = ' '.join(f'build/dc/chao/{archive}' for archive in ARCHIVES)
        for name, files, settings, take, read in (
                ('orig/', {f'orig/{archive}': '' for archive in ARCHIVES}, (), None,
                 ' '.join(f'orig/{archive}' for archive in ARCHIVES)),
                ('DISC_FILES', {}, ('DISC_FILES=the disc',),
                 "cp 'the disc/SONIC2/CHAO_MODEL.PRS' 'the disc/SONIC2/CHAO_MOTION.PRS' build/dc/chao/SONIC2", taken),
                ('a .gdi', {"orig/Tails' disc.gdi": ''}, (),
                 f"build/gdi extract 'orig/Tails'\\'' disc.gdi' build/dc/chao {' '.join(ARCHIVES)}", taken)):
            with self.subTest(name), stood_in({**SOURCES, **CHAO, **files}) as root:
                status, output = make(root, '-n', 'dc', SH_ELF, *settings)
                self.assertEqual(status, 0, output)
                if take:
                    self.assertIn(take, output)
                self.assertIn(f'python3 tools/check_chao_banks.py {read} --prskit build/prskit > '
                              'build/dc/chao/banks.txt', output)
                self.assertNotIn('skipped', output)
        with stood_in({**SOURCES, **CHAO}) as root:
            status, output = make(root, '-n', 'dc', SH_ELF)
        self.assertEqual(status, 0, output)
        self.assertNotIn('check_chao_banks', output)
        self.assertIn('skipped: orig/ lacks SONIC2/CHAO_MODEL.PRS or SONIC2/CHAO_MOTION.PRS, and neither DISC_FILES '
                      'nor one .gdi gives them', output)

    def test_takes_the_archives_again_from_another_disc_and_checks_them_again(self):
        # each copy holds the name of the disc it came from. The check's stand-in prints what the check prints once
        # both archives pass. build/gdi's stand-in, as build/gdi does, fails rather than replace a file, and a newer
        # build/gdi takes the archives again
        discs = {f'{disc}/{archive}': f'{disc}\n' for disc in ('first', 'second') for archive in ARCHIVES}
        stand_ins = {'tools/check_chao_banks.py': "print('checked')\n", 'build/prskit': '', 'build/gdi': GDI}
        checked, done = '  chao   checked\n', "make: 'build/dc/chao/banks.txt' is up to date.\n"
        extracted = f'  gdi    two.gdi\n{checked}'
        with stood_in({**SOURCES, **CHAO, **discs, **stand_ins}) as root:
            for setting, output, held in (('DISC_FILES=first', checked, 'first'), ('DISC_FILES=first', done, 'first'),
                                          ('DISC_FILES=second', checked, 'second'),
                                          ('GDI=one.gdi', f'  gdi    one.gdi\n{checked}', 'one.gdi'),
                                          ('GDI=one.gdi', done, 'one.gdi'), ('GDI=two.gdi', extracted, 'two.gdi')):
                self.assertEqual(make(root, 'build/dc/chao/banks.txt', SH_ELF, setting), (0, output))
                self.assertEqual((root / 'build/dc/chao/SONIC2/CHAO_MOTION.PRS').read_text(), f'{held}\n')
            (root / 'build/gdi').touch()
            self.assertEqual(make(root, 'build/dc/chao/banks.txt', SH_ELF, 'GDI=two.gdi'), (0, extracted))
            # an archive replaced in place, and then orig/'s, older than the last check
            (root / 'second/SONIC2/CHAO_MOTION.PRS').write_text('replaced\n')
            self.assertEqual(make(root, 'build/dc/chao/banks.txt', SH_ELF, 'DISC_FILES=second'), (0, checked))
            self.assertEqual((root / 'build/dc/chao/SONIC2/CHAO_MOTION.PRS').read_text(), 'replaced\n')
            for archive in ARCHIVES:
                (root / 'orig' / archive).parent.mkdir(parents=True, exist_ok=True)
                (root / 'orig' / archive).write_text('orig\n')
                os.utime(root / 'orig' / archive, (0, 0))
            self.assertEqual(make(root, 'build/dc/chao/banks.txt', SH_ELF), (0, checked))


class ChaoResident(unittest.TestCase):
    def test_an_edit_to_the_areas_rules_builds_the_banks_the_placed_resident_and_its_archive_again(self):
        # config/chao/areas.mk holds the recipes of all three, so a change to one of its commands reaches each
        produced = ('build/dc/census/chao-bank-objects.ld', 'build/dc/census/chao-placed.o',
                    'build/dc/census/chao-members.a')
        with stood_in({**SOURCES, **CHAO, 'sh-elf/bin/sh-elf-ld': AS}) as root:
            status, output = make(root, *produced, SH_ELF)
            self.assertEqual(status, 0, output)
            settle(root)
            status, output = make(root, '-n', '-W', 'config/chao/areas.mk', *produced, SH_ELF)
        self.assertEqual(status, 0, output)
        for command in ('python3 tools/chao_banks.py c build/dc/census/chao-bank-objects.c',
                        'python3 tools/chao_banks.py ld build/dc/census/chao-bank-objects.ld',
                        '-o build/dc/census/chao-placed.o build/dc/census/chao.o',
                        'sh-elf/bin/sh-elf-ar rcsDP build/dc/census/chao-members.a'):
            self.assertIn(command, output)


# what make -n dc-images, dc-files and dc-disc need beyond NEEDS:
# config/images.mk, the final links' rules; the main executable's layout,
# which config/1st_read/startup.mk gives in the tree; and the linker
# scripts. The override empties the memory contract's sources, which the
# tree lacks. With or without them, make -n prints the same commands
IMAGES = {'config/images.mk': (ROOT / 'config/images.mk').read_text(),
          'config/dreamcast.mk': NEEDS['config/dreamcast.mk'] + 'area_bound.build/dc/census/1st_read := '
                                 'build/dc/census/1st_read.o\noverride DC_MEMORY_SOURCES :=\n',
          'config/1st_read/image.ld': '', 'config/overlay.ld': ''}


class Mac(unittest.TestCase):
    def test_plans_each_port_goal_as_x86_64_linux_does(self):
        # macOS refuses the matching build alone: make -n plans each port goal on a Mac as on x86-64 Linux, and
        # reaches no stamp or download of SHC's tools
        with stood_in(IMAGES) as root:
            for goal in PORT_GOALS:
                linux = make(root, '-n', goal, SH_ELF)
                self.assertEqual(linux[0], 0, linux[1])
                for mac in MACS:
                    with self.subTest(goal, mac=mac):
                        self.assertEqual(make(root, '-n', goal, SH_ELF, uname=mac), linux)

    def test_names_make_dc_check_where_a_port_goal_lacks_the_disc(self):
        # stg13's data.txt takes a layout from the disc, so make dc needs orig/ and stops, and names the goal that
        # builds the port without the disc, on x86-64 Linux as on a Mac
        files = {**SOURCES, 'config/stg13/symbols.txt': '0x8c500200 menu_cells\n',
                 'config/stg13/data.txt': '0x8c500200 layout menu#cells\n',
                 'layouts/menu.txt': 'menu_cells texanims 1\n'}
        with stood_in(files) as root:
            append('config/stg13/splits.txt', 'D 0x8c500200 0x8c500214\ndisc D 0x8c500200 0x8c500214\n')(root)
            for uname in (LINUX, *MACS):
                with self.subTest(uname):
                    status, output = make(root, '-n', 'dc', SH_ELF, uname=uname)
                    self.assertNotEqual(status, 0, output)
                    self.assertRegex(output, r"\n  Without the disc, make dc-check builds and sums the port's "
                                             r'objects\.\nMakefile:\d+: \*\*\* no disc in orig/\.  Stop\.\n\Z')
            self.assertEqual(make(root, '-n', 'dc-check', SH_ELF, uname=MACS[0])[0], 0)


class Matching(unittest.TestCase):
    def test_reads_no_dc_mk_for_a_goal_of_the_matching_build(self):
        broken = {'dc.mk': '$(error dc.mk read)\n', 'config/gcc.mk': '$(error gcc.mk read)\n'}
        with stood_in(broken) as root:
            for goal in ('check', 'help', 'test'):
                with self.subTest(goal=goal):
                    status, output = make(root, '-n', goal)
                    self.assertNotIn('.mk read', output)
            status, output = make(root, '-n', 'dc', SH_ELF)
        self.assertNotEqual(status, 0, output)
        self.assertIn('dc.mk read', output)


def unknown_gcc_pragmas(source):
    # GCC applies pack and GCC diagnostic, ignores inline and section, and split.py removes inline_asm.
    known = ('pack', 'GCC', 'inline', 'inline_asm', 'section')
    unknown = [(line, match[1]) for line, text in enumerate(source.split('\n'))
               if (match := re.match(r'^\s*#\s*pragma\s+(\w+)', text)) and match[1] not in known]
    if not unknown:
        return []
    # A pragma in a matching-only group cannot reach GCC. Treat an unresolved conditional as reachable.
    compiled = split.compiled(source)
    return [pragma for line, pragma in unknown if compiled[line] is not False]


def gcc_pragmas(path):
    source = path.read_text('latin-1')
    # Follow make's INLINE selection for C files. Check the text that reaches
    # GCC so an unconsumed raw block still fails just like an unknown pragma.
    if path.suffix == '.c' and re.search(r'^#pragma (inline_asm|asm)', source, re.M):
        c, _ = split.split(str(path), source, 'inline.s')
        source = '\n'.join(c)
    return unknown_gcc_pragmas(source)


class Pragmas(unittest.TestCase):
    def test_only_a_proven_matching_guard_can_hide_an_unknown_pragma(self):
        guarded = '#ifdef NONMATCHING\n#pragma vendor_a\n#else\n#pragma asm\n#pragma endasm\n#endif\n'
        self.assertEqual(unknown_gcc_pragmas(guarded + '#if VARIANT\n#pragma vendor_b\n#endif\n#pragma asm\n'),
                         ['vendor_a', 'vendor_b', 'asm'])

    def test_gcc_reads_no_pragma_but_those_it_knows_may_ignore_or_the_split_takes_away(self):
        found = [f'{path.relative_to(ROOT)}: #pragma {pragma}'
                 for top in ('src', 'sdk', 'modules', 'engine') for path in sorted((ROOT / top).rglob('*.[ch]'))
                 for pragma in gcc_pragmas(path)]
        self.assertEqual(found, [])


if __name__ == '__main__':
    unittest.main()
