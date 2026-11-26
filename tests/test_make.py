"""The Makefile: its host checks, downloads and installs, stamps, the steps
each edit reruns, and the stops that name what is wrong. Each test runs make
or make -n in a scratch tree of its own, with stand-in tools, and needs no
disc and no SHC. The tests that commit a tree to git need git."""

import atexit
import contextlib
import functools
import hashlib
import json
import os
import re
import select
import shutil
import subprocess
import sys
import tempfile
import time
import unittest
from unittest import mock
from pathlib import Path

import tool_process
import vendor

ROOT = Path(__file__).resolve().parents[1]
# two routes, each finding its sources by directory, with the variables the
# Makefile's rules read
COMPILERS = """\
game.release  := R13
game.flags    := -cpu=sh4 -speed
game.includes := sdk/include,src,modules
game.sources  := $(shell find src modules -name '*.c' 2>/dev/null)
sdk.release   := R13
sdk.flags     := -cpu=sh4 -size
sdk.includes  := sdk/include
sdk.sources   := $(shell find sdk -name '*.c' 2>/dev/null)
ROUTES := game sdk
ASMFLAGS := -cpu=sh4
"""
HASH = '0' * 64
# the main executable and a stage, with their tables and sources: main.c in
# the main executable, and a shared source and an assembly source in the
# stage. Each table leaves its last 0x100 bytes to the rows a test adds.
TREE = {
    'config/1st_read/fill.txt': '',
    'config/1st_read/bounds.txt': '',
    'config/1st_read/splits.txt': ('file 1ST_READ.BIN\nP 0x8c010000 0x8c010200\n'
                                   'src/main.c P 0x8c010000 0x8c010040\ndisc P 0x8c010040 0x8c010100\n'),
    'config/1st_read/symbols.txt': '0x8c010000 main\n',
    'config/stg13/splits.txt': ('file SONIC2/STG13.PRS\nP 0x8c500000 0x8c500200\n'
                                'modules/shared/a.c P 0x8c500000 0x8c500040\n'
                                'modules/stg13/b.src P 0x8c500040 0x8c500060\ndisc P 0x8c500060 0x8c500100\n'),
    'src/main.c': '',
    'modules/shared/a.c': '',
    'modules/stg13/b.src': '',
    'sdk/include/.keep': '',
    'orig/.keep': '',
}
LINKS = ('build/link/1st_read/src/main.bin', 'build/link/stg13/modules/shared/a.bin',
         'build/link/stg13/modules/stg13/b.bin')


@contextlib.contextmanager
def checkout(files=None):
    """A tree make runs in: the repository's Makefile and Python tools, the
    vendored tools' files, empty, COMPILERS, TREE, and files."""
    tools = {f'tools/{path.name}': path.read_text() for path in (ROOT / 'tools').glob('*.py')}
    vendored = {path: '' for tool in vendor.TOOLS for path in vendor.files(tool)}
    every = {'Makefile': (ROOT / 'Makefile').read_text(), 'config/compilers.mk': COMPILERS,
             'SHA256SUMS': f'{HASH}  orig/1ST_READ.BIN\n', 'tools/SHA256SUMS': '', **tools, **vendored, **TREE,
             **(files or {})}
    with tempfile.TemporaryDirectory() as scratch:
        root = Path(scratch)
        for name, text in every.items():
            path = root / name
            path.parent.mkdir(parents=True, exist_ok=True)
            if isinstance(text, bytes):
                path.write_bytes(text)
            else:
                path.write_text(text)
        yield root


# what uname -sm prints on x86-64 Linux, the host a test's make reads unless
# the test names another, and on a Mac of each processor: Apple silicon, then
# Intel
LINUX = 'Linux x86_64'
MACS = ('Darwin arm64', 'Darwin x86_64')


@functools.lru_cache(maxsize=None)
def uname_for(host):
    """A directory holding a uname that prints host for -sm, as the Makefile
    asks it, and fails on any other arguments: an answer to those would come
    from the machine running the tests, not from host. The directory lasts
    until the tests end."""
    directory = tempfile.mkdtemp()
    atexit.register(shutil.rmtree, directory)
    script = Path(directory, 'uname')
    script.write_text('#!/bin/sh\n'
                      '[ "$*" = -sm ] || { echo "uname: the tests\' stand-in answers -sm alone" >&2; exit 2; }\n'
                      f"echo '{host}'\n")
    script.chmod(0o755)
    return directory


def make(root, *args, path=None, timeout=120, uname=LINUX):
    """make's status and output, stdout and stderr together, run in root
    with args, one job at a time, apart from any make running this and from
    the toolchain and SH4ZAM the environment may name, in the C locale, so
    that a host tool's messages read as the tests expect. PATH is
    uname_for(uname)'s directory, then path, or the environment's PATH where
    path is None. The Makefile asks uname for its host, so a test holds make
    to the rules of the host it names, x86-64 Linux by default, whichever
    machine runs it."""
    env = {name: value for name, value in os.environ.items()
           if name not in ('MAKEFLAGS', 'MFLAGS', 'MAKELEVEL', 'SH_ELF', 'SH4ZAM')}
    env['LC_ALL'] = 'C'
    env['PATH'] = os.pathsep.join((uname_for(uname), os.environ['PATH'] if path is None else path))
    run = tool_process.run(['make', '-j1', *args], cwd=root, env=env, stdout=subprocess.PIPE,
                           stderr=subprocess.STDOUT, text=True, timeout=timeout)
    return run.returncode, run.stdout


# the commands but the C compiler that the Makefile asks of the host
COMMANDS = ('sha256sum', 'timeout', 'curl', 'tar', 'gzip')


@contextlib.contextmanager
def host(*lacking):
    """A PATH of one directory, which holds each command PATH finds and each
    of COMMANDS, but none of lacking: a link to the command, or for one of
    COMMANDS that PATH lacks, a script that does nothing."""
    with tempfile.TemporaryDirectory() as bin:
        # a directory early in PATH links over a later one's command
        for directory in reversed(os.environ['PATH'].split(os.pathsep)):
            for found in Path(directory).glob('*') if Path(directory).is_dir() else ():
                with contextlib.suppress(OSError):
                    if found.is_file() and os.access(found, os.X_OK):
                        Path(bin, found.name).unlink(missing_ok=True)
                        Path(bin, found.name).symlink_to(found)
        for command in COMMANDS:
            if not Path(bin, command).exists():
                Path(bin, command).write_text('#!/bin/sh\n')
                Path(bin, command).chmod(0o755)
        for command in lacking:
            Path(bin, command).unlink(missing_ok=True)
        yield bin


def stand_in(root, path, text):
    """Write text, a script, to path under root, and let it run."""
    (root / path).parent.mkdir(parents=True, exist_ok=True)
    (root / path).write_text(text)
    (root / path).chmod(0o755)


def settle(root):
    """Date every file under root an hour back, and those under build/ half
    an hour back, so that make takes each file under build/ for newer than
    what it was made from."""
    now = time.time()
    for path in root.rglob('*'):
        age = 1800 if path.relative_to(root).parts[0] == 'build' else 3600
        os.utime(path, (now - age, now - age))


def placing(row):
    """The main executable's splits.txt with row added at its end."""
    return {'config/1st_read/splits.txt': TREE['config/1st_read/splits.txt'] + row}


def expected(root):
    """The paths make check wrote to build/check-expected.txt."""
    return set((root / 'build/check-expected.txt').read_text().split())


def python(setup):
    """A python3 that runs as this one does, but runs the statement setup
    before the code -c gives it."""
    return (f'#!{sys.executable}\n'
            'import os, sys\n'
            "if sys.argv[1:2] != ['-c']:\n"
            '    os.execv(sys.executable, [sys.executable, *sys.argv[1:]])\n'
            'code = sys.argv[2]\n'
            "sys.argv = ['-c', *sys.argv[3:]]\n"
            f'{setup}\n'
            "exec(compile(code, '<string>', 'exec'), {'__name__': '__main__'})\n")


def failing(tool):
    """A stand-in for tools/TOOL.py that fails as it starts, saying so."""
    return f"raise SystemExit('{tool} fails')\n"


# the tools make downloads on first use, in place
PINNED = {'tools/wibo-i686': '', 'tools/shc-v5.1r08/shc.exe': '', 'tools/shc-v5.1r11/shc.exe': '',
          'tools/shc-v5.1r13/shc.exe': '', 'tools/shc-v5.0r26/shc.exe': '', 'tools/shc-v5.0r32/shc.exe': ''}
# the tools lint and format install from WHEELS
CLANG_TOOLS = {'tools/llvm/bin/clang-format': '', 'tools/llvm/bin/clang-tidy': ''}
# the platform tag of the wheels the Makefile pins for each host, by what
# uname -sm prints there
PLATFORMS = {LINUX: 'manylinux_2_27_x86_64.manylinux_2_28_x86_64', MACS[0]: 'macosx_11_0_arm64',
             MACS[1]: 'macosx_10_9_x86_64'}
# the repository's tools/SHA256SUMS's lines for x86-64 Linux's wheels of
# clang-format and clang-tidy, in that order, and the wheels
WHEEL_SUMS = ''.join(line for line in (ROOT / 'tools/SHA256SUMS').read_text().splitlines(True)
                     if line.endswith(f'-{PLATFORMS[LINUX]}.whl\n'))
WHEELS = [line.split()[1] for line in WHEEL_SUMS.splitlines()]
# an install make takes for the one WHEELS pin: both tools, and WHEEL_SUMS in
# tools/SHA256SUMS and in the file an install writes them to last
INSTALLED = {**CLANG_TOOLS, 'tools/SHA256SUMS': WHEEL_SUMS, 'tools/llvm/wheels.txt': WHEEL_SUMS}


class Host(unittest.TestCase):
    def test_names_each_command_a_build_lacks_before_any_step(self):
        # with no pinned tool in place, each goal that builds runs curl to download them, and tar and gzip to
        # unpack the SHC releases
        with checkout() as root, host(*COMMANDS) as path:
            for goals in ((), ('check',), ('progress',), ('relink',), ('shift',)):
                with self.subTest(goals=goals):
                    status, output = make(root, *goals, 'CC=no-such-cc', path=path)
                    self.assertNotEqual(status, 0, output)
                    self.assertIn('*** no-such-cc sha256sum timeout curl tar gzip: not found; see README.md#build',
                                  output)
                    self.assertFalse((root / 'build/tools').exists())

    def test_an_explicit_test_toolchain_requires_its_programs_and_headers(self):
        with checkout() as root:
            status, output = make(root, 'test', 'SH_ELF=cross')
            self.assertNotEqual(status, 0)
            self.assertIn('SH_ELF lacks sh-elf-gcc', output)
            for tool in ('gcc', 'as', 'ld', 'nm', 'objcopy', 'objdump'):
                stand_in(root, f'cross/bin/sh-elf-{tool}', '#!/bin/sh\nexit 1\n')
            status, output = make(root, 'test', 'SH_ELF=cross')
            self.assertNotEqual(status, 0)
            self.assertIn('make test needs SH4ZAM headers', output)

    def test_needs_no_download_command_once_the_tools_are_in_place(self):
        with checkout(PINNED) as root, host('curl', 'tar', 'gzip') as path:
            status, output = make(root, 'check', 'CC=no-such-cc', path=path)
        self.assertNotEqual(status, 0, output)
        self.assertIn('*** no-such-cc: not found; see README.md#build', output)

    def test_needs_tar_and_gzip_for_an_shc_release_alone(self):
        releases = {path: text for path, text in PINNED.items() if path.endswith('/shc.exe')}
        with checkout(releases) as root, host('curl', 'tar', 'gzip') as path:
            status, output = make(root, 'check', 'CC=no-such-cc', path=path)
        self.assertNotEqual(status, 0, output)
        self.assertIn('*** no-such-cc curl: not found; see README.md#build', output)

    def test_lint_needs_the_c_compiler_alone_once_its_tools_are_in_place(self):
        with checkout(INSTALLED) as root, host(*COMMANDS) as path:
            status, output = make(root, 'lint', 'CC=no-such-cc', path=path)
        self.assertNotEqual(status, 0, output)
        self.assertIn('*** no-such-cc: not found; see README.md#build', output)

    def test_lint_and_format_need_sha256sum_and_curl_to_download_their_wheels(self):
        # with no install, and with an install of the wheels as tools/SHA256SUMS gave them other hashes
        other = {**INSTALLED, 'tools/llvm/wheels.txt': ''.join(f'{HASH}  {wheel}\n' for wheel in WHEELS)}
        for installed in ({}, other):
            with checkout(installed) as root, host(*COMMANDS) as path:
                for goal, lacks in (('lint', 'no-such-cc sha256sum curl'), ('format', 'sha256sum curl')):
                    with self.subTest(installed=bool(installed), goal=goal):
                        status, output = make(root, goal, 'CC=no-such-cc', path=path)
                        self.assertNotEqual(status, 0, output)
                        self.assertIn(f'*** {lacks}: not found; see README.md#build', output)

    def test_lint_and_format_need_ensurepip_to_install_their_tools(self):
        # the install runs python3 -m venv, which needs ensurepip, and this python3 lacks it, as Debian's and
        # Ubuntu's do without python3-venv; an install in place runs no venv. With curl gone, no make downloads
        for installed, goal, lacks in (({}, 'lint', 'no-such-cc curl python3-venv'),
                                       ({}, 'format', 'curl python3-venv'), (INSTALLED, 'lint', 'no-such-cc')):
            with self.subTest(installed=bool(installed), goal=goal), checkout(installed) as root, \
                    host('curl') as path:
                stand_in(root, 'bin/python3', python("sys.modules['ensurepip'] = None"))
                status, output = make(root, goal, 'CC=no-such-cc', path=f'{root / "bin"}{os.pathsep}{path}')
                self.assertNotEqual(status, 0, output)
                self.assertIn(f'*** {lacks}: not found; see README.md#build', output)

    def test_accept_needs_git_and_sha256sum_alone(self):
        # its gates check their own needs as each runs
        with checkout() as root, host(*COMMANDS, 'git') as path:
            status, output = make(root, 'accept', 'CC=no-such-cc', path=path)
        self.assertNotEqual(status, 0, output)
        self.assertIn('*** sha256sum git: not found; see README.md#build', output)

    def test_names_a_make_without_grouped_targets_and_the_make_a_mac_can_install(self):
        # GNU make 4.3 added grouped targets, and grouped-target in .FEATURES with them. macOS's own make is 3.81,
        # so the stop names Homebrew's. It comes before make asks its host, so it reads the same on every host, and
        # on a Mac it comes before the refusal of make check
        with checkout() as root:
            for uname in (LINUX, *MACS):
                with self.subTest(uname):
                    status, output = make(root, '-n', 'check', '.FEATURES=target-specific order-only', uname=uname)
                    self.assertNotEqual(status, 0, output)
                    self.assertRegex(output, r'\AMakefile:\d+: \*\*\* the build needs GNU make 4\.3 or later, and this '
                                             r'is [\d.]+; on macOS, brew install make and run gmake; see '
                                             r'README\.md#build\.  Stop\.\n\Z')

    def test_names_a_python_before_3_8_and_takes_3_8(self):
        for version, needed in (((3, 7, 99, 'final', 0), True), ((3, 8, 0, 'final', 0), False)):
            with self.subTest(version=version), checkout() as root:
                stand_in(root, 'bin/python3', python(f'sys.version_info = {version!r}'))
                status, output = make(root, '-n', 'test', path=f'{root / "bin"}{os.pathsep}{os.environ["PATH"]}')
                self.assertEqual(status != 0, needed, output)
                self.assertEqual('the build needs Python 3.8 or later, as python3' in output, needed, output)


# the goals a person types and the variables a person sets
GOALS = {'all', 'check', 'relink', 'shift', 'offsets', 'test', 'lint', 'format', 'review', 'stub-includes',
         'progress', 'source', 'clean', 'help', 'dc', 'dc-check', 'dc-images', 'dc-files', 'dc-disc', 'accept'}
VARIABLES = {'V', 'CC', 'WIBO', 'R08', 'R11', 'R13', 'R26', 'R32', 'SH_ELF', 'SH4ZAM', 'SOURCE', 'GDI', 'DISC_FILES',
             'HUNG', 'GRACE', 'DC_LTO_JOBS', 'FETCH_TIMEOUT'}


def written(root):
    """Each path make wrote under root's build/ but Python's bytecode."""
    return {path.relative_to(root) for path in (root / 'build').rglob('*') if 'pycache' not in path.parts}


class Help(unittest.TestCase):
    def test_names_each_goal_then_each_variable_beside_its_line(self):
        with checkout(PINNED) as root:
            status, output = make(root, 'help')
            self.assertEqual(written(root), set())
        self.assertEqual(status, 0, output)
        rows = [line.split(None, 1) for line in output.splitlines()]
        self.assertEqual(len({len(line) - len(row[1]) for line, row in zip(output.splitlines(), rows)}), 1, output)
        # a goal's line is the ## line directly above its rule, and a variable's is its ## line after NAME=form:
        text = (ROOT / 'Makefile').read_text()
        goals = [[goal, does] for does, goal in re.findall(r'^## (.+)\n([a-z-]+):', text, re.M)]
        variables = [list(row) for row in re.findall(r'^## ([A-Z0-9_]+=\S+): (.+)$', text, re.M)]
        self.assertEqual(rows, goals + variables, output)
        self.assertEqual({goal for goal, _ in goals}, GOALS)
        self.assertLessEqual(max(map(len, output.splitlines())), 80, output)
        self.assertEqual({name.split('=')[0] for name, _ in variables}, VARIABLES)


class Settings(unittest.TestCase):
    def test_refuses_a_setting_help_doesn_t_list_before_it_writes_anything(self):
        # SH_EFL misspells SH_ELF, and HOSTFLAGS, GCCFLAGS and body_root are the makefiles' own variables, which make
        # help doesn't list
        listed = re.search(r'^SETTINGS := ((?:.*\\\n)*.*)', (ROOT / 'Makefile').read_text(), re.M)[1]
        self.assertEqual(set(listed.replace('\\\n', ' ').split()), VARIABLES)
        with checkout(PINNED) as root:
            for goals, setting in (((), 'SH_EFL'), (('check',), 'HOSTFLAGS'), (('dc',), 'GCCFLAGS'),
                                   (('test',), 'body_root')):
                with self.subTest(setting=setting):
                    status, output = make(root, '-n', *goals, 'SH_ELF=sh-elf', f'{setting}=x')
                    self.assertNotEqual(status, 0, output)
                    self.assertRegex(output, rf'\AMakefile:\d+: \*\*\* {setting}: make takes no such setting; make '
                                             r'help lists those it takes\.  Stop\.\n\Z')
            self.assertEqual(written(root), set())
            status, output = make(root, 'help', *(f'{name}=x' for name in VARIABLES))
        self.assertEqual(status, 0, output)


# the goals a person types that run SHC's tools, which a Mac refuses, and
# what make says there after the goals or the file that need them
MATCHING_GOALS = ('all', 'check', 'offsets', 'progress', 'relink', 'shift', 'source', 'accept')
NO_SHC = ("the matching build needs Linux on x86, where wibo runs SHC's tools, and this is macOS; "
          'see README.md#build')
# the port's goals, which build with GCC
PORT_GOALS = ('dc', 'dc-check', 'dc-images', 'dc-files', 'dc-disc')
# the files lint reads besides the C, each empty
LINT_INPUTS = {path: '' for path in ('.clang-format', '.clang-tidy', 'compile_flags.txt', 'config/intrinsics.h',
                                     'config/intrinsics_machine.h')}
# a stand-in for a command a test's make may not run: it leaves the file ran
# where it runs
RAN = '#!/bin/sh\ntouch ran\n'


def stopped(said):
    """A pattern for the whole of make's output where the Makefile stops it
    with said."""
    return rf'\AMakefile:\d+: \*\*\* {re.escape(said)}\.  Stop\.\n\Z'


def held(root):
    """Each path under root."""
    return sorted(root.rglob('*'))


class Mac(unittest.TestCase):
    def test_refuses_each_goal_that_runs_shc_before_it_runs_python_or_fetches_a_tool(self):
        # SHC runs under wibo on Linux on x86 alone, so on a Mac of either processor make refuses make, which is
        # make all, and each goal that runs SHC, alone or beside one a Mac runs, under -n too. python3 and curl are
        # RAN, so a make that ran either would leave ran behind, and make writes nothing else either
        with checkout() as root:
            for command in ('python3', 'curl'):
                stand_in(root, f'bin/{command}', RAN)
            path, before = f'{root / "bin"}{os.pathsep}{os.environ["PATH"]}', held(root)
            for mac in MACS:
                for goals in ((), *((goal,) for goal in MATCHING_GOALS), ('lint', 'check')):
                    for flags in ((), ('-n',)):
                        with self.subTest(mac, goals=goals, flags=flags):
                            status, output = make(root, *flags, *goals, path=path, uname=mac)
                            self.assertEqual(status, 2, output)
                            refused = ' '.join(goal for goal in goals if goal in MATCHING_GOALS) or 'all'
                            self.assertRegex(output, stopped(f'{refused}: {NO_SHC}'))
            self.assertEqual(held(root), before)

    def test_plans_each_other_goal_as_x86_64_linux_does(self):
        # a Mac refuses no other goal a person types, and none reaches a stamp or a download of SHC's tools: make -n
        # plans each as x86-64 Linux does, but for the platform tag of the wheels lint and format install.
        # test_make_dc plans the port's goals
        with checkout(LINT_INPUTS) as root:
            for goal in sorted(GOALS - set(MATCHING_GOALS) - set(PORT_GOALS)):
                linux = make(root, '-n', goal)
                self.assertEqual(linux[0], 0, linux[1])
                for mac in MACS:
                    with self.subTest(goal, mac=mac):
                        status, output = make(root, '-n', goal, uname=mac)
                        self.assertEqual((status, output.replace(PLATFORMS[mac], PLATFORMS[LINUX])), linux)

    def test_no_other_host_refuses_the_matching_build(self):
        # the refusal names macOS, so no other host may meet it. Each plans make check as x86-64 Linux does: 32-bit
        # x86 Linux, Linux on arm64, FreeBSD, and a host whose uname prints nothing
        with checkout(PINNED) as root:
            linux = make(root, '-n', 'check')
            self.assertEqual(linux[0], 0, linux[1])
            for uname in ('Linux i686', 'Linux aarch64', 'FreeBSD amd64', ''):
                with self.subTest(uname):
                    self.assertEqual(make(root, '-n', 'check', uname=uname), linux)

    def test_a_goal_that_comes_to_need_shc_stops_at_its_stamp_or_download_and_names_it(self):
        # no goal a Mac runs may reach SHC. Here make review comes to depend on a stamp or a download, as a rule that
        # regresses would, and an object SHC compiles is named as a goal. Each stops make at the stamp or the
        # download, under -n too, and the stop names it; curl, which RAN stands in for, fetches nothing
        regressed = (ROOT / 'Makefile').read_text()
        cases = [(('review',), {'Makefile': f'{regressed}review: {needs}\n'}, needs)
                 for needs in ('build/tools/wibo.id', 'build/tools/R13.id', 'tools/wibo-i686',
                               'tools/shc-v5.1r13/shc.exe')]
        cases.append((('build/obj/src/main.obj',), {}, 'build/tools/R13.id'))
        for goals, files, stops in cases:
            with self.subTest(goals=goals, stops=stops), checkout(files) as root:
                stand_in(root, 'bin/curl', RAN)
                for flags in ((), ('-n',)):
                    status, output = make(root, *flags, *goals, path=f'{root / "bin"}{os.pathsep}{os.environ["PATH"]}',
                                          uname=MACS[0])
                    self.assertEqual(status, 2, output)
                    self.assertRegex(output, stopped(f'{stops}: {NO_SHC}'))
                self.assertFalse((root / 'ran').exists())
                # make writes CC's stamp as it reads the makefiles, and no stamp of SHC's tools
                self.assertEqual({path.name for path in root.glob('build/tools/*')} - {'cc.id'}, set())

    def test_stops_a_make_that_rosetta_2_translates_whatever_its_goal(self):
        # on a Mac, sysctl.proc_translated is 1 in a process Rosetta 2 translates and 0 in any other. A make there
        # stops before any goal, under -n too. Linux has no Rosetta 2, and asks no sysctl
        sysctl = '#!/bin/sh\n[ "$*" = "-n sysctl.proc_translated" ] || exit 2\ntouch asked\necho {}\n'.format
        said = 'the build needs a native make, and this one runs under Rosetta 2; see README.md#build'
        with checkout() as root:
            stand_in(root, 'bin/sysctl', sysctl(1))
            path = f'{root / "bin"}{os.pathsep}{os.environ["PATH"]}'
            for mac in MACS:
                for goals in (('help',), ('test',), ('lint', 'format'), ('check',), ('dc',)):
                    for flags in ((), ('-n',)):
                        with self.subTest(mac, goals=goals, flags=flags):
                            status, output = make(root, *flags, *goals, path=path, uname=mac)
                            self.assertEqual(status, 2, output)
                            self.assertRegex(output, stopped(said))
            (root / 'asked').unlink()
            self.assertEqual(make(root, 'help', path=path)[0], 0)
            self.assertFalse((root / 'asked').exists())
            stand_in(root, 'bin/sysctl', sysctl(0))
            self.assertEqual(make(root, 'help', path=path, uname=MACS[0])[0], 0)
            self.assertTrue((root / 'asked').exists())


# an assembly source for b.src, whose one function's @asm tag is open
OPEN = ('        .EXPORT     _b\n        .SECTION    P,CODE,ALIGN=4\n'
        '; @asm open: every instruction is one SHC writes for C\n_b:\n        RTS\n        NOP\n        .END\n')


class Review(unittest.TestCase):
    def test_lists_the_open_assembly_and_writes_nothing_under_build(self):
        with checkout({**PINNED, 'modules/stg13/b.src': OPEN}) as root:
            status, output = make(root, 'review')
            self.assertEqual(written(root), set())
        self.assertEqual((status, output.splitlines()), (0, [
            'modules/stg13/b.src:3: every instruction is one SHC writes for C',
            '@asm tags of inline bodies, #pragma asm blocks and .src functions by kind: 0 opcode, 0 operand, 0 abi, '
            '0 epilogue, 0 name and 1 open; 1 in all']))


class Clean(unittest.TestCase):
    def test_refuses_another_goal_and_removes_build_alone(self):
        # make -j runs the goals at once, so rm -rf build would race the other goal's writes
        with checkout({'build/kept': ''}) as root:
            for goals in (('clean', 'check'), ('check', 'clean')):
                with self.subTest(goals=goals):
                    status, output = make(root, '-n', *goals)
                    self.assertNotEqual(status, 0, output)
                    self.assertRegex(output, r'Makefile:\d+: \*\*\* make clean takes no other goal\.  Stop\.\n\Z')
            status, output = make(root, 'clean')
            self.assertEqual(status, 0, output)
            self.assertFalse((root / 'build').exists())


# GATE_MAKE(make=..., bin=...) is the text of bin/make, a stand-in for make
# that PATH finds first. Given accept, it runs the make that make= names,
# with MAKE in its environment naming the stand-in, which make takes for
# $(MAKE), so that accept runs each gate with the stand-in. That
# environment sets SH_ELF and SH4ZAM too, as a person's shell may. Given a
# gate, the stand-in adds a line to bin/gates.txt: its arguments, then the
# name of each of MAKEFLAGS, MFLAGS, MAKELEVEL, SH4ZAM and V that its
# environment sets. Then it runs bin/during with its arguments, where a test
# wrote one, and fails, as make does, where its arguments are
# bin/fails.txt's text
GATE_MAKE = ('#!/bin/sh\ncase " $* " in *" accept "*)\n'
             '    exec env MAKE={bin}/make SH_ELF=cross SH4ZAM=copy {make} "$@";; esac\n'
             'echo "$*${{MAKEFLAGS+ MAKEFLAGS}}${{MFLAGS+ MFLAGS}}${{MAKELEVEL+ MAKELEVEL}}'
             '${{SH4ZAM+ SH4ZAM}}${{V+ V}}" >> {bin}/gates.txt\n'
             '[ ! -x {bin}/during ] || {bin}/during "$*"\n'
             '[ "$*" != "$(cat {bin}/fails.txt 2> /dev/null)" ] || exit 2\n').format
# the environment of a candidate's git and its make accept: none of the
# host's GIT_ variables, as a hook's GIT_DIR, none of the host's git
# configuration, and an author and committer of its own
GIT_ENV = {**{name: value for name, value in os.environ.items() if not name.startswith('GIT_')},
           'GIT_CONFIG_GLOBAL': os.devnull, 'GIT_CONFIG_NOSYSTEM': '1', 'GIT_AUTHOR_NAME': 'a',
           'GIT_AUTHOR_EMAIL': 'a@a', 'GIT_COMMITTER_NAME': 'a', 'GIT_COMMITTER_EMAIL': 'a@a'}
# the files accept sums, which a candidate holds where its dc-files and
# dc-disc gates would leave them: the manifest and the disc's files, in
# sha256sum's order. SUM_LINES is the record's line for each
SUMMED = {'build/dc/manifest.txt': 'a manifest\n', 'build/dc/disc/disc.gdi': 'a gdi\n',
          'build/dc/disc/disc.txt': 'a receipt\n', 'build/dc/disc/track03.bin': 'a track\n'}
SUM_LINES = [f'sum      {hashlib.sha256(text.encode()).hexdigest()}  {path}' for path, text in SUMMED.items()]


class Accept(unittest.TestCase):
    # accept's gates in the order it runs them, each that uses the toolchain
    # with the SH_ELF its command line takes from GATE_MAKE's environment,
    # and test with tools/sh4zam's headers
    GATES = ('lint', 'all', 'check', 'dc-check SH_ELF=cross', 'dc SH_ELF=cross', 'dc-images SH_ELF=cross',
             'dc-files SH_ELF=cross', 'dc-disc SH_ELF=cross', 'test SH_ELF=cross SH4ZAM=tools/sh4zam')

    @contextlib.contextmanager
    def candidate(self):
        """A checkout committed to git, which ignores build/, with SUMMED's
        files, and os.environ as GIT_ENV. Yields its root, its commit, a PATH
        that finds GATE_MAKE's stand-in first, and the stand-in's directory.
        Skips the test on a host without git."""
        if not shutil.which('git'):
            self.skipTest('needs git')
        with checkout({**SUMMED, '.gitignore': '/build/\n'}) as root, tempfile.TemporaryDirectory() as scratch, \
                mock.patch.dict(os.environ, GIT_ENV, clear=True):
            bin = Path(scratch)
            stand_in(bin, 'make', GATE_MAKE(make=shutil.which('make'), bin=bin))
            for command in (['init', '-q'], ['add', '-A'], ['commit', '-q', '-m', 'a candidate']):
                subprocess.run(['git', *command], cwd=root, check=True, stdout=subprocess.DEVNULL)
            commit = subprocess.run(['git', 'rev-parse', 'HEAD'], cwd=root, check=True, stdout=subprocess.PIPE,
                                    text=True).stdout.strip()
            yield root, commit, f'{bin}{os.pathsep}{os.environ["PATH"]}', bin

    def record(self, root):
        """build/accept.txt's lines."""
        return (root / 'build/accept.txt').read_text().splitlines()

    def passed(self, bin, gates):
        """The record's line for each of gates, run by bin's make, that passed."""
        return [f'gate     0  {bin}/make -j1 {gate}' for gate in gates]

    def test_runs_each_gate_in_turn_as_it_runs_alone_and_records_accepted_once_all_pass(self):
        # of make accept's flags and command-line variables, only -j reaches a gate. Its environment reaches every
        # gate, SH4ZAM among it, and SH_ELF from it goes on the command line of each gate that uses the toolchain.
        # accept's own make writes nothing under build/ but its record: it reads no table and keeps no .cmd file
        with self.candidate() as (root, commit, path, bin):
            status, output = make(root, 'accept', 'V=1', path=path)
            self.assertEqual(status, 0, output)
            gates = (bin / 'gates.txt').read_text().splitlines()
            record = self.record(root)
            files = {name for name in written(root) if (root / name).is_file()}
        self.assertEqual(files, {Path(name) for name in (*SUMMED, 'build/accept.txt')})
        self.assertEqual(gates, [f'-j1 {gate} SH4ZAM' for gate in self.GATES])
        self.assertEqual(record, [f'commit   {commit}', *self.passed(bin, self.GATES), *SUM_LINES, 'accepted'])
        self.assertTrue(output.endswith('\n'.join([*record, ''])), output)

    def test_hands_each_gate_the_disc_its_command_line_or_its_environment_names(self):
        # GDI and DISC_FILES reach each gate whole, from either, and the command line's make flags still reach none
        disc = {'DISC_FILES': '/disc/an extraction', 'GDI': "/disc/it's.gdi"}
        for given in ('command line', 'environment'):
            with self.subTest(given), self.candidate() as (root, commit, path, bin):
                stand_in(bin, 'during', f'#!/bin/sh\nprintf "%s|%s\\n" "$DISC_FILES" "$GDI" >> {bin}/disc.txt\n')
                if given == 'command line':
                    status, output = make(root, 'accept', *(f'{name}={value}' for name, value in disc.items()),
                                          path=path)
                else:
                    with mock.patch.dict(os.environ, disc):
                        status, output = make(root, 'accept', path=path)
                self.assertEqual(status, 0, output)
                self.assertEqual((bin / 'disc.txt').read_text().splitlines(),
                                 [f"{disc['DISC_FILES']}|{disc['GDI']}"] * len(self.GATES))
                self.assertEqual((bin / 'gates.txt').read_text().splitlines(),
                                 [f'-j1 {gate} SH4ZAM' for gate in self.GATES])

    def test_sums_what_the_gates_built_and_fails_without_accepted_where_one_is_gone(self):
        # sha256sum -c on the sum lines holds the files to the run that passed. With the disc gone, the gates pass
        # but the record names no sum and says no accepted
        with self.candidate() as (root, commit, path, bin):
            status, output = make(root, 'accept', path=path)
            self.assertEqual(status, 0, output)
            sums = ''.join(f'{line.split(None, 1)[1]}\n' for line in self.record(root) if line.startswith('sum '))
            checked = subprocess.run(['sha256sum', '-c', '--quiet', '-'], cwd=root, input=sums, text=True,
                                     stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
            self.assertEqual(checked.returncode, 0, checked.stdout)
            shutil.rmtree(root / 'build/dc/disc')
            status, output = make(root, 'accept', path=path)
            self.assertNotEqual(status, 0, output)
            record = self.record(root)
        self.assertEqual(record, [f'commit   {commit}', *self.passed(bin, self.GATES)])

    def test_a_gate_that_fails_stops_the_run_and_its_record_says_so_and_not_accepted(self):
        # a second run fails at dc, after a first that accepted
        with self.candidate() as (root, commit, path, bin):
            status, output = make(root, 'accept', path=path)
            self.assertEqual(status, 0, output)
            (bin / 'gates.txt').unlink()
            (bin / 'fails.txt').write_text('-j1 dc SH_ELF=cross\n')
            status, output = make(root, 'accept', path=path)
            self.assertNotEqual(status, 0, output)
            gates = (bin / 'gates.txt').read_text().splitlines()
            record = self.record(root)
        self.assertEqual(gates, [f'-j1 {gate} SH4ZAM' for gate in self.GATES[:5]])
        self.assertEqual(record, [f'commit   {commit}', *self.passed(bin, self.GATES[:4]),
                                  f'gate     2  {bin}/make -j1 dc SH_ELF=cross'])

    def test_runs_no_gate_on_a_tree_that_differs_from_its_commit(self):
        # a tracked file changed, and an untracked one, which status.showUntrackedFiles=no hides from git status
        # though make's scans read it
        with self.candidate() as (root, commit, path, bin):
            subprocess.run(['git', 'config', 'status.showUntrackedFiles', 'no'], cwd=root, check=True)
            (root / 'config/1st_read/symbols.txt').write_text('0x8c010000 start\n')
            (root / 'src/a.h').write_text('')
            status, output = make(root, 'accept', path=path)
            self.assertNotEqual(status, 0, output)
            self.assertFalse((bin / 'gates.txt').exists())
            record = self.record(root)
        changed = ['changed   M config/1st_read/symbols.txt', 'changed  ?? src/a.h']
        self.assertEqual(record, [f'commit   {commit}', *changed])
        self.assertIn('\n'.join([*changed, f'build/accept.txt: the tree differs from {commit}', '']), output)

    def test_refuses_a_head_or_a_tree_that_changes_as_the_gates_run(self):
        # check commits, which moves HEAD, or edits a tracked file. Every gate runs and passes, and the record says
        # what changed, sums nothing and says no accepted
        for change in ('git commit -q --allow-empty -m moved', 'echo >> src/main.c'):
            with self.subTest(change=change), self.candidate() as (root, commit, path, bin):
                stand_in(bin, 'during', f'#!/bin/sh\n[ "$1" != "-j1 check" ] || {change}\n')
                status, output = make(root, 'accept', path=path)
                self.assertNotEqual(status, 0, output)
                self.assertEqual(self.record(root), [f'commit   {commit}', *self.passed(bin, self.GATES),
                                                     'changed  HEAD or the tree, as the gates ran'])

    def test_fails_where_git_fails_after_the_gates_though_it_prints_the_commit(self):
        # after the gates, git rev-parse still prints HEAD, and git status fails having printed nothing. Judged by
        # that output alone, the tree matches the commit
        with self.candidate() as (root, commit, path, bin):
            stand_in(bin, 'git', f'#!/bin/sh\nif [ "$1" = status ]; then echo >> {bin}/status.txt; '
                                 f'[ "$(wc -l < {bin}/status.txt)" -lt 2 ] || exit 128; fi\n'
                                 f'exec {shutil.which("git")} "$@"\n')
            status, output = make(root, 'accept', path=path)
            self.assertNotEqual(status, 0, output)
            record = self.record(root)
        self.assertEqual(record, [f'commit   {commit}', *self.passed(bin, self.GATES)])

    def test_runs_no_gate_and_writes_no_record_where_git_fails(self):
        # a tree with no .git, as an archive of it is. git fails there, and its empty output would pass for a clean
        # tree's
        with self.candidate() as (root, commit, path, bin), \
                mock.patch.dict(os.environ, GIT_CEILING_DIRECTORIES=str(root.parent)):
            shutil.rmtree(root / '.git')
            status, output = make(root, 'accept', path=path)
            self.assertNotEqual(status, 0, output)
            self.assertFalse((bin / 'gates.txt').exists())
            self.assertFalse((root / 'build/accept.txt').exists())

    def test_refuses_another_goal_before_it_reads_dc_mk_or_writes_under_build(self):
        # make -j runs the goals at once, so accept's gates would race the other goal's writes. make refuses before
        # it writes under build/ or reads dc.mk, whose host checks would stop it first: this dc.mk stops it at once
        with checkout({'dc.mk': '$(error make read dc.mk)\n'}) as root:
            for goals in (('accept', 'dc'), ('check', 'accept')):
                with self.subTest(goals=goals):
                    status, output = make(root, '-n', *goals)
                    self.assertNotEqual(status, 0, output)
                    self.assertRegex(output, r'Makefile:\d+: \*\*\* make accept takes no other goal\.  Stop\.\n\Z')
            self.assertEqual(written(root), set())


class Sources(unittest.TestCase):
    def test_requires_one_matching_compiler_route_per_source(self):
        source = 'src/main.c'
        with checkout() as root:
            routes = root / 'config/compilers.mk'
            (root / 'src/system').mkdir()
            for spelling in (source, './src/main.c', 'src/system/../main.c'):
                with self.subTest(spelling=spelling):
                    routes.write_text(COMPILERS + f'sdk.sources += {spelling}\n')
                    status, output = make(root, '-n', 'check')
                    self.assertNotEqual(status, 0, output)
                    self.assertIn(f'{source} listed more than once in routes game sdk', output)
                    self.assertNotIn('overriding recipe', output)

            reassigned = COMPILERS + f'game.sources := $(filter-out {source},$(game.sources))\n'
            routes.write_text(reassigned)
            status, output = make(root, '-n', 'check')
            self.assertNotEqual(status, 0, output)
            self.assertIn(f'no matching compiler route for {source}', output)

            routes.write_text(reassigned + 'sdk.sources += ./src/main.c\n')
            status, output = make(root, '-n', 'check')
            self.assertEqual(status, 0, output)
            self.assertNotIn('overriding recipe', output)

    def test_only_lcp_takes_release_8_among_the_network_sources(self):
        releases = ("route:\n\t@printf '%s\\n' $(foreach r,$(ROUTES),"
                    "$(if $(filter $(SOURCE),$($r.sources)),$($r.release)))")
        with checkout({'config/compilers.mk': (ROOT / 'config/compilers.mk').read_text(),
                       'sdk/network/ppp_lcp.c': '', 'sdk/network/ppp_session.c': ''}) as root:
            for source, release in (('ppp_lcp', 'R08'), ('ppp_session', 'R13')):
                with self.subTest(source=source):
                    status, output = make(root, '-f', 'config/compilers.mk', '--eval', releases,
                                          f'SOURCE=sdk/network/{source}.c', 'route')
                    self.assertEqual((status, output), (0, release + '\n'))

    def test_links_each_source_row_of_each_table(self):
        # a row split by tabs and ending in a comment links; a row commented out, a disc row and a section row don't
        row = '\tsrc/other.c\tP\t0x8c010100\t0x8c010110  # note\n# src/gone.c P 0x8c010110 0x8c010120\n'
        with checkout({**placing(row), 'src/other.c': '', 'src/gone.c': ''}) as root:
            status, output = make(root, '-n', 'check')
            self.assertNotEqual(status, 0, output)
            self.assertIn('src/gone has no row in any splits.txt', output)
            (root / 'src/gone.c').unlink()
            status, output = make(root, '-n', 'check')
            self.assertEqual(status, 0, output)
            self.assertEqual(expected(root), {'orig/1ST_READ.BIN', 'orig/SONIC2/STG13.PRS', *LINKS,
                                              'build/link/1st_read/src/other.bin'})
        self.assertIn('python3 tools/link.py build/obj/modules/stg13/b.obj stg13', output)

    def test_stops_on_a_source_no_table_places(self):
        for source in ('src/extra.c', 'sdk/ninja/extra.src'):
            with self.subTest(source=source), checkout({source: ''}) as root:
                status, output = make(root, '-n', 'check')
                self.assertNotEqual(status, 0, output)
                self.assertIn(f'{source[:-len(Path(source).suffix)]} has no row in any splits.txt', output)

    def test_stops_on_a_c_source_and_an_assembly_source_of_one_stem(self):
        # every image links build/obj/<stem>.obj, so x.c and x.src would build one object, whether two images place
        # them or the main executable places x.c beside an unplaced x.src
        stage = TREE['config/stg13/splits.txt'] + 'sdk/ninja/x.src P 0x8c500100 0x8c500110\n'
        for case, files in (('two images', {**placing('sdk/ninja/x.c P 0x8c010100 0x8c010110\n'),
                                            'config/stg13/splits.txt': stage}),
                            ('unplaced', placing('sdk/ninja/x.c P 0x8c010100 0x8c010110\n'))):
            with self.subTest(case), checkout({**files, 'sdk/ninja/x.c': '', 'sdk/ninja/x.src': ''}) as root:
                status, output = make(root, '-n', 'check')
                self.assertNotEqual(status, 0, output)
                self.assertIn('sdk/ninja/x has a .c and a .src, which would build one object', output)

    def test_stops_on_a_source_a_route_lists_that_isnt_there(self):
        listed = COMPILERS.replace('ROUTES := game sdk', 'listed.release := R13\nlisted.flags :=\nlisted.includes :=\n'
                                                         'listed.sources := sdk/runtime/_gone.c\n'
                                                         'ROUTES := game sdk listed')
        with checkout({'config/compilers.mk': listed}) as root:
            status, output = make(root, '-n', 'check')
        self.assertNotEqual(status, 0, output)
        self.assertIn('config/compilers.mk: sdk/runtime/_gone.c: no such source', output)

    def test_stops_on_a_route_whose_release_isnt_in_releases(self):
        # make fetches and stamps only the releases RELEASES names
        other = COMPILERS.replace('sdk.release   := R13', 'sdk.release   := R99')
        with checkout({'config/compilers.mk': other}) as root:
            status, output = make(root, '-n', 'check')
        self.assertNotEqual(status, 0, output)
        self.assertIn('config/compilers.mk: no release in RELEASES for R99', output)

    def test_stops_on_a_patched_or_warned_source_no_route_compiles_or_whose_list_is_empty(self):
        for routes, stop in (('PATCHED := src/gone.c\nsrc/gone.c.patches := p\n',
                              'src/gone.c: patched, but no route compiles it'),
                             ('PATCHED := src/main.c\n', 'src/main.c: patched, but its .patches names no patch'),
                             ('WARNED := src/gone.c\nsrc/gone.c.warnings := 100\n',
                              'src/gone.c: warned, but no route compiles it'),
                             ('WARNED := src/main.c\n', 'src/main.c: warned, but its .warnings names no warning')):
            with self.subTest(stop), checkout({'config/compilers.mk': COMPILERS + routes}) as root:
                status, output = make(root, '-n', 'check')
                self.assertNotEqual(status, 0, output)
                self.assertRegex(output, rf'\AMakefile:\d+: \*\*\* config/compilers\.mk: {re.escape(stop)}\.  '
                                         r'Stop\.\n\Z')


# C that mentions the pragmas without giving them, and C that gives each: an
# inline body, one whose pragma has a space before its (, and a block of
# assembly
MENTIONS = '/* Unlike #pragma inline_asm, this is ordinary C */\n'
GIVEN = {'inline_asm': '#pragma inline_asm(f)\nstatic void f(void)\n{\n        NOP\n}\n',
         'spaced inline_asm': '#pragma inline_asm (f)\nstatic void f(void)\n{\n        NOP\n}\n',
         'asm': '#pragma asm\n        NOP\n#pragma endasm\n'}


class InitializerBodies(unittest.TestCase):
    def test_stops_as_it_starts_where_a_scan_of_the_body_sources_fails(self):
        # tools/body_offsets.py finds the body sources, and tools/body_inputs.py each one's image, headers and
        # generated includes
        files = {**PINNED, 'config/bodies.mk': (ROOT / 'config/bodies.mk').read_text(),
                 **placing('D 0x8c010200 0x8c010204\nsrc/main.c D 0x8c010200 0x8c010204\n'),
                 'src/main.c': 'int value = {\n#include "main/value.inc"\n};\n',
                 'offsets/src/main.txt': 'datum value D 0x00000000 4\n'}
        for tool, stop in (('body_offsets', r'Makefile:\d+: \*\*\* tools/body_offsets\.py: cannot find native body '
                                            r'sources'),
                           ('body_inputs', r'config/bodies\.mk:\d+: \*\*\* tools/body_inputs\.py: initializer '
                                           r'dependency scan failed')):
            with self.subTest(tool), checkout({**files, f'tools/{tool}.py': failing(tool)}) as root:
                status, output = make(root, '-n', 'check')
                self.assertNotEqual(status, 0, output)
                self.assertRegex(output, rf'\A{tool} fails\n{stop}\.  Stop\.\n\Z')

    def test_a_stub_that_asmsh_assembles_has_one_rule_for_its_listing(self):
        # a body source with inline assembly builds a stub and assembles it: one rule gives its listing an empty
        # recipe, so make prints no warning that a second overrides it, and the stub still waits on the listing
        obj = 'build/stub/obj/src/main.obj'
        files = {**PINNED, 'config/bodies.mk': (ROOT / 'config/bodies.mk').read_text(),
                 **placing('D 0x8c010200 0x8c010204\nsrc/main.c D 0x8c010200 0x8c010204\n'),
                 'src/main.c': '#pragma inline_asm(f)\nint value = {\n#include "main/value.inc"\n};\n',
                 'offsets/src/main.txt': 'datum value D 0x00000000 4\n'}
        with checkout(files) as root:
            for goal in ('check', 'offsets'):
                with self.subTest(goal=goal):
                    status, output = make(root, '-n', goal)
                    self.assertEqual(status, 0, output)
                    self.assertNotIn('warning', output)
            # make -p prints its rules: the stub's line of prerequisites, among the lines of its variables
            status, rules = make(root, '-n', '-p', obj)
            self.assertEqual(status, 0, rules)
            needs = ' '.join(re.findall(rf'^{re.escape(obj)}: (.*)$', rules, re.M)).split()
            self.assertIn(obj.replace('.obj', '.lis'), needs)

    def test_check_stops_on_offsets_or_bindings_no_body_source_reads(self):
        # in a built tree, where make check has nothing else to do
        for path, stop in (('offsets/src/gone.txt', 'offsets without a body source; run make offsets'),
                           ('offsets/src/gone.bindings', 'bindings without a native body source')):
            with self.subTest(path), built() as root:
                (root / 'offsets/src').mkdir(parents=True)
                (root / path).write_text('')
                status, output = make(root, '-n', 'check')
                self.assertNotEqual(status, 0, output)
                self.assertRegex(output, rf'\AMakefile:\d+: \*\*\* {re.escape(path)}: {re.escape(stop)}\.  Stop\.\n\Z')

    def test_changing_the_sysrof_reader_rechecks_measured_offsets(self):
        obj = 'build/stub/obj/src/main.obj'
        checked = 'build/offsets/src/main.ok'
        command = 'python3 tools/body_offsets.py src/main.c'
        files = {**PINNED, **placing('D 0x8c010200 0x8c010204\nsrc/main.c D 0x8c010200 0x8c010204\n'),
                 'config/bodies.mk': (ROOT / 'config/bodies.mk').read_text(),
                 'src/main.c': 'int value = {\n#include "main/value.inc"\n};\n',
                 'offsets/src/main.txt': 'datum value D 0x00000000 4\n',
                 obj: b'', 'build/stub/obj/src/main.lis': (
                     "D   00000000            _value:  ; static: value\n"
                     "    00000000 00000000   .DATA.L H'00000000\n"
                     "DATA SECTION (D): 00000004 Byte(s)\n")}
        with checkout(files) as root:
            # Keep the compiled stub and run the ordinary measurement and comparison.
            for changes, rebuilt in (((), True), ((), False), (('-W', 'tools/sysrof.py'), True)):
                with self.subTest(changes=changes, rebuilt=rebuilt):
                    status, output = make(root, 'V=1', '-o', obj, *changes, checked)
                    self.assertEqual(status, 0, output)
                    self.assertEqual(command in output, rebuilt, output)
                    self.assertEqual('cmp build/offsets/src/main.txt' in output, rebuilt, output)

    def test_only_a_differing_table_asks_for_make_offsets(self):
        checked = 'build/offsets/src/main.ok'
        files = {**PINNED, **placing('D 0x8c010200 0x8c010204\nsrc/main.c D 0x8c010200 0x8c010204\n'),
                 'config/bodies.mk': (ROOT / 'config/bodies.mk').read_text(),
                 'src/main.c': 'int value = {\n#include "main/value.inc"\n};\n',
                 'offsets/src/main.txt': 'datum value D 0x00000000 4\n',
                 'build/offsets/src/main.txt': 'datum value D 0x00000000 8\n'}
        with checkout(files) as root:
            status, output = make(root, '-o', 'build/offsets/src/main.txt', checked)
            self.assertNotEqual(status, 0, output)
            self.assertIn('run make offsets', output)
            # a cmp that can't read the table compares nothing, and exits 2
            stand_in(root, 'bin/cmp', '#!/bin/sh\necho "cmp: $2: Permission denied" >&2\nexit 2\n')
            status, output = make(root, '-o', 'build/offsets/src/main.txt', checked,
                                  path=f'{root / "bin"}{os.pathsep}{os.environ["PATH"]}')
            self.assertNotEqual(status, 0, output)
            self.assertIn('Permission denied', output)
            self.assertNotIn('run make offsets', output)
            self.assertFalse((root / checked).exists())

    def test_regenerates_a_body_when_its_rules_change(self):
        include = 'build/stub/src/main/value.inc'
        files = {**PINNED, **placing('D 0x8c010200 0x8c010204\nsrc/main.c D 0x8c010200 0x8c010204\n'),
                 'config/bodies.mk': (ROOT / 'config/bodies.mk').read_text(),
                 'src/main.c': 'int value = {\n#include "main/value.inc"\n};\n',
                 'offsets/src/main.txt': 'datum value D 0x00000000 4\n'}
        command = 'tools/body_data.py 1st_read src/main.c'
        with checkout(files) as root:
            status, output = make(root, include)
            self.assertEqual(status, 0, output)
            for changes, regenerated in ((('-W', 'config/bodies.mk'), True), ((), False)):
                with self.subTest(changes=changes):
                    status, plan = make(root, '-n', *changes, include)
                    self.assertEqual(status, 0, plan)
                    self.assertEqual(command in plan, regenerated, plan)

    def test_disc_free_links_and_full_initializers_use_separate_build_products(self):
        source = ('typedef unsigned char Byte;\n'
                  'Byte speed[1] = {\n#include "main/speed.inc"\n};\n'
                  'Byte mode[1] = {\n#include "main/mode.inc"\n};\n')
        files = {**PINNED, 'src/main.c': source, 'config/bodies.mk': (ROOT / 'config/bodies.mk').read_text(),
                 'config/1st_read/splits.txt': TREE['config/1st_read/splits.txt'] + (
                     'D 0x8c010200 0x8c010202\nsrc/main.c D 0x8c010200 0x8c010202\n'),
                 'offsets/src/main.txt': 'datum speed D 0x00000000 1\ndatum mode D 0x00000001 1\n'}
        stub_dir = Path('build/stub/src/main')
        full_dir = Path('build/gen/src/main')
        with checkout(files) as root:
            status, plan = make(root, '-n', 'build/stub/link/1st_read/src/main.bin')
            self.assertEqual(status, 0, plan)
            self.assertIn('tools/link.py --stub build/stub/obj/src/main.obj 1st_read', plan)
            self.assertNotIn('--original', plan)

            # Removing one grouped output must regenerate it even with its sibling present.
            both = [str(stub_dir / name) for name in ('speed.inc', 'mode.inc')]
            status, output = make(root, *both)
            self.assertEqual(status, 0, output)
            (root / both[1]).unlink()
            status, output = make(root, *both)
            self.assertEqual(status, 0, output)
            self.assertTrue((root / both[1]).is_file())

            status, output = make(root, str(full_dir / 'speed.inc'))
            self.assertNotEqual(status, 0, output)
            self.assertIn('no disc in orig/', output)
            original = bytes(0x200) + bytes([57, 84])
            (root / 'orig/1ST_READ.BIN').write_bytes(original)
            (root / 'SHA256SUMS').write_text(f'{hashlib.sha256(original).hexdigest()}  orig/1ST_READ.BIN\n')
            status, output = make(root, str(full_dir / 'speed.inc'), *both)
            self.assertEqual(status, 0, output)
            for name, value in (('speed.inc', '57'), ('mode.inc', '84')):
                self.assertEqual((root / full_dir / name).read_text().strip(), value)
                self.assertEqual((root / stub_dir / name).read_text().strip(), '0')


    def test_nested_and_direct_initializers_share_the_members_include_directory(self):
        source = ('typedef unsigned char Byte;\n'
                  'Byte speed[1] = {\n#include "main/speed.inc"\n};\n'
                  '#include "main/mesh.h"\n')
        mesh = 'static Byte mode[1] = {\n#include "main/mesh/mode.inc"\n};\n'
        original = bytes(0x200) + bytes([57, 84])
        files = {**PINNED, 'src/main.c': source, 'src/main/mesh.h': mesh,
                 'config/bodies.mk': (ROOT / 'config/bodies.mk').read_text(),
                 **placing('D 0x8c010200 0x8c010202\nsrc/main.c D 0x8c010200 0x8c010202\n'),
                 'offsets/src/main.txt': 'datum speed D 0x00000000 1\ndatum mode D 0x00000001 1\n',
                 'orig/1ST_READ.BIN': original,
                 'SHA256SUMS': f'{hashlib.sha256(original).hexdigest()}  orig/1ST_READ.BIN\n'}
        with checkout(files) as root:
            for build, expected in (('gen', ['57', '84']), ('stub', ['0', '0'])):
                directory = Path('build') / build / 'variants/src/main/main'
                outputs = [directory / 'speed.inc', directory / 'mesh/mode.inc']
                status, log = make(root, *map(str, outputs))
                self.assertEqual(status, 0, log)
                self.assertEqual([(root / path).read_text().strip() for path in outputs], expected)

    def test_a_placed_c_wrapper_needs_its_own_offsets_before_compilation(self):
        files = {**PINNED, 'config/bodies.mk': (ROOT / 'config/bodies.mk').read_text(),
                 'src/provider.c': 'char value = {\n#include "provider/value.inc"\n};\n',
                 'src/main.c': '#include "provider.c"\n',
                 'offsets/src/provider.txt': 'datum value D 0x00000000 1\n',
                 'config/1st_read/splits.txt': TREE['config/1st_read/splits.txt'] + (
                     'D 0x8c010200 0x8c010202\nsrc/provider.c D 0x8c010200 0x8c010201\n'
                     'src/main.c D 0x8c010201 0x8c010202\n')}
        with checkout(files) as root:
            for goal in ('all', 'check', 'dc', 'lint', 'build/obj/src/main.obj'):
                with self.subTest(goal=goal):
                    status, output = make(root, '-n', goal)
                    self.assertNotEqual(status, 0, output)
                    self.assertIn('offsets/src/main.txt: missing offsets for src/main.c', output)
            status, output = make(root, '-n', 'offsets')
            self.assertEqual(status, 0, output)
            self.assertIn('tools/body_offsets.py src/main.c', output)

    def test_c_variants_build_from_separate_optical_placements(self):
        table = 'datum values D 0x00000000 1\n'
        files = {**PINNED, 'config/bodies.mk': (ROOT / 'config/bodies.mk').read_text(),
                 'src/provider.c': 'typedef unsigned char Byte;\nByte values[1] = {\n#include "provider/values.inc"\n};\n',
                 'src/main.c': '#include "provider.c"\n',
                 'offsets/src/provider.txt': table, 'offsets/src/main.txt': table,
                 'config/1st_read/splits.txt': TREE['config/1st_read/splits.txt'] + (
                     'D 0x8c010200 0x8c010202\nsrc/provider.c D 0x8c010200 0x8c010201\n'
                     'src/main.c D 0x8c010201 0x8c010202\n')}
        direct = 'src/provider/values.inc'
        variant = 'variants/src/main/provider/values.inc'
        with checkout(files) as root:
            status, output = make(root, 'build/stub/' + variant)
            self.assertEqual(status, 0, output)
            self.assertEqual((root / 'build/stub' / variant).read_text().strip(), '0')
            original = bytes(0x200) + bytes([57, 84])
            (root / 'orig/1ST_READ.BIN').write_bytes(original)
            (root / 'SHA256SUMS').write_text(f'{hashlib.sha256(original).hexdigest()}  orig/1ST_READ.BIN\n')
            status, output = make(root, 'build/gen/' + direct, 'build/gen/' + variant)
            self.assertEqual(status, 0, output)
            self.assertEqual((root / 'build/gen' / direct).read_text().strip(), '57')
            self.assertEqual((root / 'build/gen' / variant).read_text().strip(), '84')


    def test_deleted_transitive_text_header_is_rebuilt_in_the_variant_directory(self):
        table = 'datum values D 0x00000000 1\ntext LABEL D 0x00000001 2\n'
        files = {**PINNED, 'config/bodies.mk': (ROOT / 'config/bodies.mk').read_text(),
                 'src/provider.c': ('typedef unsigned char Byte;\n#include "provider/text.inc"\nchar *label = LABEL;\n'
                                    'Byte values[1] = {\n#include "provider/values.inc"\n};\n'),
                 'src/main.c': '#include "provider.c"\n',
                 'offsets/src/provider.txt': table, 'offsets/src/main.txt': table,
                 'config/1st_read/splits.txt': TREE['config/1st_read/splits.txt'] + (
                     'D 0x8c010200 0x8c010206\nsrc/provider.c D 0x8c010200 0x8c010203\n'
                     'src/main.c D 0x8c010203 0x8c010206\n')}
        with checkout(files) as root:
            original = bytes(0x200) + b'1A\x002B\x00'
            (root / 'orig/1ST_READ.BIN').write_bytes(original)
            (root / 'SHA256SUMS').write_text(f'{hashlib.sha256(original).hexdigest()}  orig/1ST_READ.BIN\n')
            direct = Path('build/gen/src/provider/text.inc')
            private = Path('build/gen/variants/src/main/provider/text.inc')
            sibling = private.with_name('values.inc')
            status, output = make(root, str(direct), str(sibling))
            self.assertEqual(status, 0, output)
            self.assertEqual((root / private).read_text().strip(), '#define LABEL "B"')
            (root / private).unlink()
            status, output = make(root, str(sibling), str(private))
            self.assertEqual(status, 0, output)
            self.assertEqual((root / private).read_text().strip(), '#define LABEL "B"')
            self.assertEqual((root / sibling).read_text().strip(), '50')
            self.assertEqual((root / direct).read_text().strip(), '#define LABEL "A"')


# a source with a body, and a variant that compiles its text again with a body
# of its own, each with its table and its row in the main executable
VARIANT = {'compile_flags.txt': (ROOT / 'compile_flags.txt').read_text(),
           'config/bodies.mk': (ROOT / 'config/bodies.mk').read_text(),
           'src/provider.c': 'typedef unsigned char Byte;\nByte values[1] = {\n#include "provider/values.inc"\n};\n',
           'src/variant.c': '#include "provider.c"\n',
           'offsets/src/provider.txt': 'datum values D 0x00000000 1\n',
           'offsets/src/variant.txt': 'datum values D 0x00000000 1\n',
           **placing('D 0x8c010200 0x8c010202\nsrc/provider.c D 0x8c010200 0x8c010201\n'
                     'src/variant.c D 0x8c010201 0x8c010202\n')}
# the directory of the variant's bodies, and the one body the provider's source
# and the variant each write by one name
VARIANT_ROOT = 'build/stub/variants/src/variant'
PROVIDED = 'build/stub/src/provider/values.inc'
OWN = f'{VARIANT_ROOT}/provider/values.inc'


class StubIncludes(unittest.TestCase):
    def test_writes_the_bodies_and_commands_without_the_disc_or_shc(self):
        # the tree holds neither the disc nor SHC, and make stub-includes compiles nothing: it writes the bodies and
        # a command for each C source and each test. The variant, and the test that includes its text, search its
        # directory first. cc's preprocessor, given a command's include directories, finds each file's own body
        flags = VARIANT['compile_flags.txt'].split()
        test = 'tests/variant_values.c'
        with checkout({**VARIANT, test: '#include "variant.c"\n'}) as root:
            status, output = make(root, 'stub-includes')
            self.assertEqual(status, 0, output)
            files = {str(path) for path in written(root) if (root / path).is_file()}
            self.assertEqual({path for path in files if not path.startswith('build/body-inputs/')},
                             {PROVIDED, OWN, 'build/compile_commands.json'})
            commands = json.loads((root / 'build/compile_commands.json').read_text())
            self.assertEqual({command['directory'] for command in commands}, {str(root.resolve())})
            compiled = ('src/main.c', 'modules/shared/a.c', 'src/provider.c', 'src/variant.c', test)
            self.assertEqual({command['file']: command['arguments'] for command in commands},
                             {file: ['clang', *([f'-I{VARIANT_ROOT}'] if file in ('src/variant.c', test) else []),
                                     *flags, '-c', file] for file in compiled})
            read = {}
            for command in commands:
                directories = [argument for argument in command['arguments'] if argument.startswith('-I')]
                scan = subprocess.run(['cc', '-MM', '-nostdinc', *directories, command['file']], cwd=root,
                                      capture_output=True, text=True)
                self.assertEqual(scan.returncode, 0, scan.stderr)
                read[command['file']] = scan.stdout.split()
        self.assertIn(PROVIDED, read['src/provider.c'])
        for file in ('src/variant.c', test):
            self.assertIn(OWN, read[file])
            self.assertNotIn(PROVIDED, read[file])

    def test_stops_where_a_test_s_scan_fails_and_names_the_test(self):
        # a cc that fails its scan of one test, having printed nothing. A test whose scan passes and includes no body
        # still gets its command
        with checkout({**VARIANT, 'tests/plain.c': '', 'tests/broken.c': ''}) as root:
            stand_in(root, 'bin/cc', '#!/bin/sh\ncase "$*" in *tests/broken.c*) exit 1;; esac\nexec cc "$@"\n')
            status, output = make(root, 'stub-includes', f'CC={root}/bin/cc')
            self.assertNotEqual(status, 0, output)
            self.assertRegex(output, rf'Makefile:\d+: \*\*\* tests/broken\.c: {re.escape(str(root))}/bin/cc -MM '
                                     r'failed\.  Stop\.\n\Z')
            self.assertFalse((root / 'build/compile_commands.json').exists())
            (root / 'tests/broken.c').unlink()
            status, output = make(root, 'stub-includes', f'CC={root}/bin/cc')
            self.assertEqual(status, 0, output)
            commands = json.loads((root / 'build/compile_commands.json').read_text())
            self.assertIn('tests/plain.c', {command['file'] for command in commands})

    def test_compile_flags_search_the_stub_bodies_of_each_top_directory(self):
        # a source's bodies lie under build/stub/ in the source's top directory, sdk/, src/ or modules/, unless the
        # source is a variant
        tops = {path.relative_to(ROOT / 'offsets').parts[0] for path in (ROOT / 'offsets').rglob('*.txt')}
        flags = (ROOT / 'compile_flags.txt').read_text().split()
        self.assertEqual({top for top in tops if f'-Ibuild/stub/{top}' not in flags}, set())


class Inline(unittest.TestCase):
    def test_takes_a_source_to_assembly_by_its_directives(self):
        # a source asmsh assembles keeps asmsh's macros in its .cmd file, so an edit of them builds it again
        for kind, text in (('mention', MENTIONS), *GIVEN.items()):
            with self.subTest(kind), checkout({**PINNED, 'src/main.c': text}) as root:
                status, output = make(root, '-n', 'check')
                self.assertEqual(status, 0, output)
                self.assertIn('shc.exe src/main.c -objectfile=build/obj/src/main.src -code=asmcode' if kind in GIVEN
                              else 'shc.exe src/main.c -objectfile=build/obj/src/main.obj -code=machinecode', output)
                self.assertEqual('asmsh.exe' in (root / 'build/obj/src/main.cmd').read_text(), kind in GIVEN)


# a wibo that ignores TERM, and makes the file "survived" if it runs two seconds
OUTLIVES_TERM = "#!/bin/sh\ntrap '' TERM\nsleep 2\ntouch survived\n"


class Hung(unittest.TestCase):
    def test_fixture_timeout_closes_the_recipe_helpers_pipe(self):
        # The helper ignores TERM and keeps the FIFO open until the group is killed.
        with tempfile.TemporaryDirectory() as scratch:
            root = Path(scratch)
            (root / 'Makefile').write_text('all:\n\t@./hold-open\n')
            stand_in(root, 'hold-open', f'#!{sys.executable}\n'
                     'import signal, time\nsignal.signal(signal.SIGTERM, signal.SIG_IGN)\n'
                     'with open("live", "w") as pipe:\n    print("helper ready", flush=True)\n    time.sleep(30)\n')
            os.mkfifo(root / 'live')
            with os.fdopen(os.open(root / 'live', os.O_RDONLY | os.O_NONBLOCK), 'rb') as reader:
                with mock.patch.dict(os.environ, {'GRACE': '0.2'}), self.assertRaises(SystemExit) as caught:
                    make(root, timeout=1)
                self.assertIn('make -j1: timed out after 1 seconds', str(caught.exception))
                self.assertIn('helper ready', str(caught.exception))
                self.assertTrue(select.select([reader], [], [], 2)[0])
                self.assertEqual(reader.read(1), b'')

    def test_ci_wibo_retries_only_a_timeout_and_keeps_both_attempts(self):
        obj = 'build/obj/modules/stg13/b.obj'
        target_log = obj + '.log'
        tool = 'tools/retry_wibo.sh'
        with checkout({**PINNED, tool: (ROOT / tool).read_text()}) as root:
            stand_in(root, 'tools/wibo-i686', '''#!/bin/sh
echo attempt >> calls
if [ ! -f started ]; then
    touch started
    echo first-attempt-output
    exec sleep 10
fi
echo final-attempt-output
''')
            # CI sets RETRY_WIBO_TIMEOUT in make's environment
            with mock.patch.dict(os.environ, {'RETRY_WIBO_TIMEOUT': '1'}):
                status, output = make(root, obj, 'HUNG=0.25', 'GRACE=1')
            self.assertEqual(status, 0, output)
            self.assertEqual((root / 'calls').read_text().splitlines(), ['attempt', 'attempt'])
            self.assertIn('first-attempt-output', (root / (target_log + '.timeout.asmsh.exe')).read_text())
            self.assertEqual((root / target_log).read_text(), 'final-attempt-output\n')
            self.assertIn('retry exited with status 0', output)

    def test_ci_wibo_preserves_native_compiler_diagnostics(self):
        obj = 'build/obj/src/main.obj'
        tool = 'tools/retry_wibo.sh'
        routes = COMPILERS + 'WARNED := src/main.c\nsrc/main.c.warnings := 100\n'
        for diagnostic, code, expected in [('source: 100 (W) expected', 0, 0),
                                           ('no warning', 0, 2),
                                           ('source: 200 (W) unexpected', 0, 2),
                                           ("timeout: sending signal TERM to command 'sh'", 124, 2),
                                           ('killed', 137, 2)]:
            with self.subTest(diagnostic=diagnostic), checkout({**PINNED, 'config/compilers.mk': routes,
                                                                  tool: (ROOT / tool).read_text()}) as root:
                stand_in(root, 'tools/wibo-i686', '#!/bin/sh\necho attempt >> calls\n'
                         + 'echo "' + diagnostic + '" >&2\nexit ' + str(code) + '\n')
                with mock.patch.dict(os.environ, {'RETRY_WIBO_TIMEOUT': '1'}):
                    status, output = make(root, obj)
                self.assertEqual(status, expected, output)
                self.assertEqual((root / 'calls').read_text().splitlines(), ['attempt'])

    def test_kills_a_tool_still_running_its_grace_after_term(self):
        # with the limits cut to half a second each, timeout sends the stand-in wibo TERM, then KILL, a second before
        # it would write survived
        with checkout(PINNED) as root:
            stand_in(root, 'tools/wibo-i686', OUTLIVES_TERM)
            status, output = make(root, 'build/obj/modules/stg13/b.obj', 'HUNG=0.5', 'GRACE=0.5')
            time.sleep(2)
            survived = (root / 'survived').exists()
        self.assertNotEqual(status, 0, output)
        for sent in ('TERM', 'KILL'):
            self.assertIn(f"timeout: sending signal {sent} to command 'tools/wibo-i686'", output)
        self.assertFalse(survived)


def depends(target, source, *headers):
    """A .d file's text for target, as the host compiler's -MM -MP writes it."""
    return f'{target}: {" ".join((source, *headers))}\n' + ''.join(f'{header}:\n' for header in headers)


# the originals, each holding its own name
ORIGINALS = {f'orig/{name}': name.encode() for name in ('1ST_READ.BIN', 'SONIC2/STG13.PRS')}
# a tree in which make and make check have only their hashes left to check:
# the originals; TREE's objects and links, with the rows each link read, and
# STG13.PRS's decoded and rebuilt files, built; SHA256SUMS's line for each
# original and link, in path order; and the pinned tools in place. The
# splicer, the word counter and the checks of make check read real objects,
# so stand-ins that pass take their places
LISTED = {**ORIGINALS, LINKS[0]: bytes(64), LINKS[1]: bytes(64), LINKS[2]: bytes(32)}
BUILT = {
    **PINNED, **LISTED, 'modules/shared/a.c': '#include "shared/a.h"\n', 'modules/shared/a.h': '',
    'tools/splice.py': '', 'tools/names.py': '', 'tools/progress.py': '', 'tools/words.py': '',
    'build/link/1st_read/src/main.rows': 'P 0x8c010000 0x8c010040\nmain 0x8c010000\n',
    'build/link/stg13/modules/shared/a.rows': 'P 0x8c500000 0x8c500040\n',
    'build/link/stg13/modules/stg13/b.rows': 'P 0x8c500040 0x8c500060\n',
    'SHA256SUMS': ''.join(f'{hashlib.sha256(data).hexdigest()}  {path}\n' for path, data in sorted(LISTED.items())),
    'build/1ST_READ.BIN': ORIGINALS['orig/1ST_READ.BIN'], 'build/SONIC2/STG13.PRS': ORIGINALS['orig/SONIC2/STG13.PRS'],
    **{path: '' for path in ('build/verified/1ST_READ.BIN.ok', 'build/verified/SONIC2/STG13.PRS.ok',
                             'build/tools/vendored.ok', 'build/prskit', 'build/orig/SONIC2/STG13.BIN',
                             'build/SONIC2/STG13.BIN', 'build/obj/src/main.obj', 'build/obj/modules/shared/a.obj',
                             'build/obj/modules/stg13/b.obj', 'build/obj/modules/stg13/b.lis')},
    'build/obj/src/main.d': depends('build/obj/src/main.obj', 'src/main.c'),
    'build/obj/modules/shared/a.d': depends('build/obj/modules/shared/a.obj', 'modules/shared/a.c',
                                            'modules/shared/a.h'),
}


@contextlib.contextmanager
def built(*args, files=None):
    """The checkout of files, BUILT unless given, settled once make check
    with args has stamped the tools."""
    with checkout(BUILT if files is None else files) as root:
        status, output = make(root, '-n', 'check', *args)
        if status:
            raise AssertionError(output)
        settle(root)
        yield root


class Sums(unittest.TestCase):
    def test_prints_one_line_for_the_files_that_match(self):
        # make checks SHA256SUMS's five lines, the two originals' against the files it rebuilds from them, and make
        # check the three links; neither prints a command, or a line for each file. CC=true passes the host check,
        # as nothing compiles, and the tree is built with it: another CC would compile each C object again
        for goals, count in (((), 5), (('check',), 3)):
            with self.subTest(goals=goals), built('CC=true') as root:
                status, output = make(root, *goals, 'CC=true')
                self.assertEqual((status, output), (0, f'  sums   {count} files match SHA256SUMS\n'))

    def test_all_and_check_each_hand_tools_sums_py_a_list_of_their_own(self):
        # make -j all check runs both recipes at once, so neither may rewrite the list the other's sums.py reads
        lists = {}
        with built('CC=true') as root:
            for goal in ('all', 'check'):
                status, output = make(root, '-n', goal, 'CC=true')
                self.assertEqual(status, 0, output)
                lists[goal] = re.findall(r'^python3 tools/sums\.py (\S+)$', output, re.M)
        self.assertEqual(lists, {'all': ['build/expected.txt'], 'check': ['build/check-expected.txt']})

    def test_names_a_file_that_differs_and_fails(self):
        # make check checks each link against its line, and make each rebuilt file against its original's, since the
        # splicer has checked the links against the originals
        for goals, changed in ((('check',), LINKS[2]), ((), 'build/1ST_READ.BIN')):
            with self.subTest(goals=goals), built('CC=true') as root:
                (root / changed).write_bytes(b'\x01' + (root / changed).read_bytes()[1:])
                # dated with the rest, so that make doesn't rebuild it
                settle(root)
                status, output = make(root, *goals, 'CC=true')
                self.assertNotEqual(status, 0, output)
                self.assertIn(f'{changed}: FAILED\nsha256sum: WARNING: 1 computed checksum did NOT match\n', output)
                self.assertNotIn('files match', output)


# each step, by the text make -n prints for it
MAIN_SHC = 'shc.exe src/main.c'
SHARED_SHC = 'shc.exe modules/shared/a.c'
ASM = 'asmsh.exe'
MAIN_LINK = 'tools/link.py build/obj/src/main.obj 1st_read'
SHARED_LINK = 'tools/link.py build/obj/modules/shared/a.obj stg13'
ASM_LINK = 'tools/link.py build/obj/modules/stg13/b.obj stg13'
MAIN_SPLICE = 'tools/splice.py 1st_read orig/1ST_READ.BIN build/1ST_READ.BIN'
STAGE_SPLICE = 'tools/splice.py stg13 build/orig/SONIC2/STG13.BIN build/SONIC2/STG13.BIN'
STEPS = (MAIN_SHC, SHARED_SHC, ASM, MAIN_LINK, SHARED_LINK, ASM_LINK, MAIN_SPLICE, STAGE_SPLICE)
MAIN_RELINK = 'tools/relink.py 1st_read orig/1ST_READ.BIN build/1ST_READ.BIN'
STAGE_RELINK = 'tools/relink.py stg13 build/orig/SONIC2/STG13.BIN build/SONIC2/STG13.BIN'
RELINK_STEPS = (*STEPS, MAIN_RELINK, STAGE_RELINK)
MAIN_SHIFT = 'tools/relink.py --shift 1st_read orig/1ST_READ.BIN build/1ST_READ.BIN'
STAGE_SHIFT = 'tools/relink.py --shift stg13 build/orig/SONIC2/STG13.BIN build/SONIC2/STG13.BIN'
FIXED = 'cat build/relink/1st_read/fixed.txt build/relink/stg13/fixed.txt > build/relink/fixed.txt'
SHIFT_STEPS = (*RELINK_STEPS, MAIN_SHIFT, STAGE_SHIFT, FIXED)


def touch(path):
    """An edit that dates the file at path now, and changes nothing in it."""
    return lambda root: os.utime(root / path)


def replace(path, old, new):
    """An edit that writes new in place of old in the file at path."""
    def edit(root):
        text = (root / path).read_text()
        if old not in text:
            raise AssertionError(f'{path} holds no {old}')
        (root / path).write_text(text.replace(old, new))
    return edit


def append(path, more):
    """An edit that adds more to the end of the file at path."""
    return lambda root: (root / path).write_text((root / path).read_text() + more)


# main.c on a route of its own, whose flags differ from the game's
MOVED = """\
game.sources := $(filter-out src/main.c,$(game.sources))
own.release  := R13
own.flags    := -cpu=sh4 -optimize=0
own.includes := sdk/include,src,modules
own.sources  := src/main.c
ROUTES += own
"""


class Freshness(unittest.TestCase):
    # each edit, the goal, the steps it reruns, and those it leaves alone
    EDITS = [
        ('a header', touch('modules/shared/a.h'), 'check', [SHARED_SHC, SHARED_LINK], [MAIN_SHC, MAIN_LINK]),
        ('a source', touch('src/main.c'), 'all', [MAIN_SHC, MAIN_LINK, MAIN_SPLICE], [SHARED_SHC, STAGE_SPLICE]),
        ("a table's date", touch('config/stg13/splits.txt'), 'all', [STAGE_SPLICE],
         [SHARED_LINK, ASM_LINK, MAIN_SPLICE]),
        ("a source's rows", replace('config/stg13/splits.txt', 'b.src P 0x8c500040 0x8c500060\ndisc P 0x8c500060',
                                    'b.src P 0x8c500040 0x8c500050\ndisc P 0x8c500050'), 'all',
         [ASM_LINK, STAGE_SPLICE], [ASM, SHARED_LINK, MAIN_LINK, MAIN_SPLICE]),
        ("a section gap's witnesses", touch('config/1st_read/fill.txt'), 'all', [MAIN_SPLICE],
         [MAIN_LINK, STAGE_SPLICE]),
        ("a section gap's bounds", touch('config/1st_read/bounds.txt'), 'all', [MAIN_SPLICE], [STAGE_SPLICE]),
        ('a name no link uses', append('config/1st_read/symbols.txt', '0x8c010100 other\n'), 'all', [], STEPS),
        ("the address of a name one link uses", replace('config/1st_read/symbols.txt', '0x8c010000 main',
                                                         '0x8c010004 main'), 'all', [MAIN_LINK, MAIN_SPLICE],
         [MAIN_SHC, SHARED_LINK, ASM_LINK, STAGE_SPLICE]),
        ('a comment in compilers.mk', append('config/compilers.mk', '# a comment\n'), 'check', [], STEPS),
        ("a route's flags", replace('config/compilers.mk', '-cpu=sh4 -speed', '-cpu=sh4 -size'), 'check',
         [MAIN_SHC, SHARED_SHC], [ASM]),
        ("asmsh's flags", replace('config/compilers.mk', 'ASMFLAGS := -cpu=sh4', 'ASMFLAGS := -cpu=sh4 -private'),
         'check', [ASM], [MAIN_SHC, SHARED_SHC]),
        ('the rule that assembles a .src file', replace('Makefile', '$$(call status,asm,', '$$(call status,as,'),
         'check', [ASM], [MAIN_SHC, SHARED_SHC]),
        ('a source moved to another route', append('config/compilers.mk', MOVED), 'check', [MAIN_SHC],
         [SHARED_SHC, ASM]),
        ('a comment in the Makefile', append('Makefile', '# a comment\n'), 'check', [], STEPS),
        ('the linker', touch('tools/link.py'), 'check', [MAIN_LINK, SHARED_LINK, ASM_LINK], [MAIN_SHC]),
        ('the splicer', touch('tools/splice.py'), 'all', [MAIN_SPLICE, STAGE_SPLICE], [MAIN_LINK]),
        ('what the splicer reads of listings', touch('tools/inline.py'), 'all', [MAIN_SPLICE, STAGE_SPLICE], []),
        ("the kinds, which give the splicer a filename's alignment", touch('tools/kinds.py'), 'all',
         [MAIN_SPLICE, STAGE_SPLICE], []),
        ("the offsets tables' reader, which gives the splicer each source's generated C", touch('tools/body_data.py'),
         'all', [MAIN_SPLICE, STAGE_SPLICE], []),
        ("the GNU build's data tool", touch('tools/content.py'), 'all', [], [MAIN_SPLICE, STAGE_SPLICE]),
    ]
    # each file a step writes, the goal, and the steps its loss reruns
    DELETED = [
        ('build/obj/src/main.d', 'check', [MAIN_SHC]),
        ('build/obj/src/main.cmd', 'check', [MAIN_SHC]),
        ('build/obj/modules/stg13/b.obj', 'check', [ASM]),
        ('build/obj/modules/stg13/b.lis', 'check', [ASM]),
        ('build/link/stg13/modules/shared/a.rows', 'check', [SHARED_LINK]),
    ]

    def test_resplices_a_stage_for_the_tables_of_its_filename_pools_and_no_other(self):
        # the splice reads data.txt where a disc row in C starts on a word, and the layout data.txt places at such a
        # start. Without that row, or for another layout, an edit reruns no splice and a missing layout stops no make
        tables = {'config/stg13/data.txt': '0x8c500200 layout filenames\n0x8c500060 layout other\n',
                  'layouts/filenames.txt': '.name texture_filename 16\n', 'layouts/other.txt': '.name points 1\n'}
        pool = {**BUILT, **tables,
                'config/stg13/splits.txt': TREE['config/stg13/splits.txt'] + 'C 0x8c500200 0x8c500300\n'
                                           'disc C 0x8c500200 0x8c500210\n'}
        def lose(path):
            return lambda root: (root / path).unlink()
        for name, files, edit, runs in (('the table', pool, touch('config/stg13/data.txt'), [STAGE_SPLICE]),
                                        ("the pool's layout", pool, touch('layouts/filenames.txt'), [STAGE_SPLICE]),
                                        ('another layout', pool, touch('layouts/other.txt'), []),
                                        ('another layout lost', pool, lose('layouts/other.txt'), []),
                                        ('no row in C', {**BUILT, **tables}, touch('config/stg13/data.txt'), []),
                                        ('no row in C, a layout lost', {**BUILT, **tables},
                                         lose('layouts/filenames.txt'), [])):
            with self.subTest(name), built(files=files) as root:
                edit(root)
                status, output = make(root, '-n', 'all')
                self.assertEqual(status, 0, output)
                self.assertEqual([step for step in STEPS if step in output], runs, output)

    def test_splices_a_stage_with_a_fill_table_and_no_bounds(self):
        with built() as root:
            (root / 'config/stg13/fill.txt').write_text('')
            status, output = make(root, '-n', 'all')
            self.assertEqual(status, 0, output)
            self.assertEqual([step for step in STEPS if step in output], [STAGE_SPLICE], output)

    def test_splices_again_an_image_that_loses_its_bounds_or_its_fill(self):
        # the splice read both tables, and with either gone none of its prerequisites is newer than the image
        for path in ('config/1st_read/bounds.txt', 'config/1st_read/fill.txt'):
            with self.subTest(path), built() as root:
                (root / path).unlink()
                status, output = make(root, '-n', 'all')
                self.assertEqual(status, 0, output)
                self.assertEqual([step for step in STEPS if step in output], [MAIN_SPLICE], output)

    def test_reruns_nothing_once_built(self):
        with built() as root:
            for goal in ('all', 'check'):
                status, output = make(root, '-n', goal)
                self.assertEqual(status, 0, output)
                self.assertEqual([step for step in STEPS if step in output], [], output)

    def test_reruns_the_steps_an_edit_reaches_and_no_other(self):
        for name, edit, goal, runs, skips in self.EDITS:
            with self.subTest(name), built() as root:
                edit(root)
                status, output = make(root, '-n', goal)
                self.assertEqual(status, 0, output)
                self.assertEqual([step for step in runs if step not in output], [], output)
                self.assertEqual([step for step in skips if step in output], [], output)

    def test_reruns_the_steps_a_deleted_file_feeds(self):
        for path, goal, runs in self.DELETED:
            with self.subTest(path), built() as root:
                (root / path).unlink()
                status, output = make(root, '-n', goal)
                self.assertEqual(status, 0, output)
                self.assertEqual([step for step in runs if step not in output], [], output)


class Relink(unittest.TestCase):
    # each edit, and every step make relink runs after it, in RELINK_STEPS's order
    EDITS = [
        ('the whole link', touch('tools/relink.py'), [MAIN_RELINK, STAGE_RELINK]),
        ("a stage's table", touch('config/stg13/splits.txt'), [STAGE_SPLICE, STAGE_RELINK]),
        ("the main executable's names", touch('config/1st_read/symbols.txt'), [MAIN_RELINK, STAGE_RELINK]),
        ('a source', touch('src/main.c'), [MAIN_SHC, MAIN_LINK, MAIN_SPLICE, MAIN_RELINK]),
    ]
    RELINKED = {**BUILT, 'build/relink/1st_read.bin': '', 'build/relink/stg13.bin': ''}

    def test_links_each_image_whole_after_its_splice(self):
        with built() as root:
            status, output = make(root, '-n', 'relink')
        self.assertEqual(status, 0, output)
        self.assertEqual([step for step in RELINK_STEPS if step in output], [MAIN_RELINK, STAGE_RELINK], output)

    def test_links_again_an_image_that_loses_its_names_or_its_bounds(self):
        files = {**self.RELINKED, 'config/stg13/symbols.txt': '0x8c500000 shared\n'}
        for path, runs in (('config/stg13/symbols.txt', [STAGE_RELINK]),
                           ('config/1st_read/bounds.txt', [MAIN_SPLICE, MAIN_RELINK])):
            with self.subTest(path), built(files=files) as root:
                (root / path).unlink()
                status, output = make(root, '-n', 'relink')
                self.assertEqual(status, 0, output)
                self.assertEqual([step for step in RELINK_STEPS if step in output], runs, output)

    def test_reruns_nothing_once_relinked(self):
        with built(files=self.RELINKED) as root:
            status, output = make(root, '-n', 'relink')
        self.assertEqual(status, 0, output)
        self.assertEqual([step for step in RELINK_STEPS if step in output], [], output)

    def test_reruns_the_steps_an_edit_reaches_and_no_other(self):
        for name, edit, runs in self.EDITS:
            with self.subTest(name), built(files=self.RELINKED) as root:
                edit(root)
                status, output = make(root, '-n', 'relink')
                self.assertEqual(status, 0, output)
                self.assertEqual([step for step in RELINK_STEPS if step in output], runs, output)


class Shift(unittest.TestCase):
    # each edit, and every step make shift runs after it, in SHIFT_STEPS's order
    EDITS = [
        ('the reasons', touch('config/kept.txt'), [MAIN_SHIFT, STAGE_SHIFT, FIXED]),
        ('their reader', touch('tools/kept.py'), [MAIN_SHIFT, STAGE_SHIFT, FIXED]),
        ('the whole link', touch('tools/relink.py'), [MAIN_RELINK, STAGE_RELINK, MAIN_SHIFT, STAGE_SHIFT, FIXED]),
        ("a stage's table", touch('config/stg13/splits.txt'), [STAGE_SPLICE, STAGE_RELINK, STAGE_SHIFT, FIXED]),
    ]
    TESTED = {**BUILT, 'config/kept.txt': ''}
    SHIFTED = {**Relink.RELINKED, 'config/kept.txt': '', 'build/relink/1st_read/fixed.txt': '',
               'build/relink/stg13/fixed.txt': '', 'build/relink/fixed.txt': ''}

    def test_links_each_image_whole_then_shifted_and_joins_the_reports(self):
        with built(files=self.TESTED) as root:
            status, output = make(root, '-n', 'shift')
        self.assertEqual(status, 0, output)
        self.assertEqual([step for step in SHIFT_STEPS if step in output],
                         [MAIN_RELINK, STAGE_RELINK, MAIN_SHIFT, STAGE_SHIFT, FIXED], output)

    def test_reruns_nothing_once_shifted(self):
        with built(files=self.SHIFTED) as root:
            status, output = make(root, '-n', 'shift')
        self.assertEqual(status, 0, output)
        self.assertEqual([step for step in SHIFT_STEPS if step in output], [], output)

    def test_reruns_the_steps_an_edit_reaches_and_no_other(self):
        for name, edit, runs in self.EDITS:
            with self.subTest(name), built(files=self.SHIFTED) as root:
                edit(root)
                status, output = make(root, '-n', 'shift')
                self.assertEqual(status, 0, output)
                self.assertEqual([step for step in SHIFT_STEPS if step in output], runs, output)


class Dependencies(unittest.TestCase):
    def test_names_a_header_that_only_shc_reads(self):
        # a source of inline bodies keeps its includes with them, inside #ifdef DREAMCAST, and SHC reads them: the
        # object's .d file names such a header though the host's compiler defines no DREAMCAST, so an edit of it
        # compiles again
        hidden = {'src/main.c': '#ifdef DREAMCAST\n#include "main.h"\n#endif\n', 'src/main.h': ''}
        with built(files={**BUILT, **hidden}) as root:
            os.utime(root / 'src/main.c')
            status, output = make(root, '-n', 'check')
            self.assertEqual(status, 0, output)
            scans = [line for line in output.splitlines() if ' -MM ' in line and line.endswith('build/obj/src/main.d')]
            self.assertEqual(len(scans), 1, output)
            subprocess.run(scans[0], shell=True, cwd=root, check=True)
            self.assertIn('src/main.h', (root / 'build/obj/src/main.d').read_text())


class Tables(unittest.TestCase):
    def test_reads_the_tables_as_it_starts_only_for_a_goal_that_compiles(self):
        # a table with a broken row and a source written before its row stop make check as it starts, and no other
        # goal; make -n runs no recipe, and lint's tag check reads the tables when it runs
        broken = {'config/stg13/splits.txt': 'file SONIC2/STG13.PRS\nP 0x8c500000\n', 'src/new.c': ''}
        with checkout({**INSTALLED, **LINT_INPUTS, **broken}) as root:
            for goal in ('test', 'lint', 'format', 'review', 'help'):
                with self.subTest(goal=goal):
                    status, output = make(root, '-n', goal)
                    self.assertEqual(status, 0, output)
            status, output = make(root, '-n', 'check')
        self.assertNotEqual(status, 0, output)
        # tools/config.py names the row, and make the query that read it
        self.assertRegex(output, r'config/stg13/splits\.txt:2: not a row: .*\nMakefile:\d+: \*\*\* tools/config\.py: '
                                 r'.+ failed\.  Stop\.\n\Z')

    def test_stops_as_it_starts_where_link_py_fails_to_rewrite_the_rows(self):
        with checkout({**PINNED, 'tools/link.py': failing('link')}) as root:
            status, output = make(root, '-n', 'check')
        self.assertNotEqual(status, 0, output)
        self.assertRegex(output, r'\Alink fails\nMakefile:\d+: \*\*\* tools/link\.py --rows failed\.  Stop\.\n\Z')

    def test_the_matching_goals_run_no_tool_only_the_gnu_build_runs_nor_import_one(self):
        # so a data.txt row the splicer doesn't read, or a table of tools/content.py's, fails only the GNU
        # build's goals
        def named(*paths):
            return {name for path in paths for name in re.findall(r'tools/(\w+)\.py', (ROOT / path).read_text())}
        matching = named('Makefile', 'config/bodies.mk', 'config/compilers.mk')
        gnu = named('dc.mk', 'config/dc_links.mk', 'config/dreamcast.mk', 'config/gcc.mk', 'config/images.mk')
        self.assertIn('content', gnu - matching)
        code = f'import sys, {", ".join(sorted(matching))}; print(*sys.modules)'
        env = {**os.environ, 'PYTHONPATH': 'tools', 'PYTHONDONTWRITEBYTECODE': '1'}
        loaded = subprocess.run([sys.executable, '-c', code], cwd=ROOT, env=env, capture_output=True, text=True,
                                check=True).stdout.split()
        self.assertEqual((gnu - matching) & set(loaded), set())


# a source whose link SHA256SUMS lacks, built
NEW = {**placing('src/new.c P 0x8c010100 0x8c010110\n'), 'src/new.c': '', 'build/obj/src/new.obj': '',
       'build/obj/src/new.d': depends('build/obj/src/new.obj', 'src/new.c'),
       'build/link/1st_read/src/new.bin': bytes(16), 'build/link/1st_read/src/new.rows': 'P 0x8c010100 0x8c010110\n'}


class Source(unittest.TestCase):
    def test_counts_the_words_where_orig_holds_the_original_and_checks_the_links(self):
        with built('CC=true') as root:
            status, output = make(root, '-n', 'source', 'SOURCE=modules/shared/a.c', 'CC=true')
            self.assertEqual(status, 0, output)
            self.assertIn('for i in stg13; do python3 tools/words.py $i modules/shared/a.c || [ $? = 3 ] || exit 1; '
                          'done', output)
            self.assertEqual(make(root, 'source', 'SOURCE=modules/shared/a.c', 'CC=true'),
                             (0, '  sums   1 of 1 links match SHA256SUMS\n'))

    def test_goes_on_past_a_function_that_differs_and_stops_on_a_failure(self):
        # tools/words.py exits 3 where a function differs, and 1 where it fails
        for status, stops in ((3, False), (1, True)):
            with self.subTest(status=status), built('CC=true') as root:
                (root / 'tools/words.py').write_text(f'raise SystemExit({status})\n')
                settle(root)
                code, output = make(root, 'source', 'SOURCE=modules/shared/a.c', 'CC=true')
                self.assertEqual(code != 0, stops, output)
                self.assertEqual('links match SHA256SUMS' in output, not stops, output)

    def test_extracts_the_originals_from_a_gdi_to_count_the_words(self):
        with checkout({**PINNED, 'orig/disc.gdi': ''}) as root:
            status, output = make(root, '-n', 'source', 'SOURCE=modules/shared/a.c')
        self.assertEqual(status, 0, output)
        self.assertIn('build/gdi extract', output)
        self.assertIn('tools/words.py $i modules/shared/a.c', output)

    def test_counts_no_words_where_orig_lacks_the_original(self):
        with checkout(PINNED) as root:
            status, output = make(root, '-n', 'source', 'SOURCE=modules/shared/a.c')
        self.assertEqual(status, 0, output)
        self.assertIn("echo 'make source counts no words: orig/ lacks SONIC2/STG13.PRS'", output)
        self.assertNotIn('tools/words.py', output)

    def test_fails_a_link_without_a_line_until_sums_add_gives_it_one(self):
        with built('CC=true', files={**BUILT, **NEW}) as root:
            status, output = make(root, 'source', 'SOURCE=src/new.c', 'CC=true')
            self.assertNotEqual(status, 0, output)
            self.assertIn('  sums   0 of 1 links match SHA256SUMS\nbuild/link/1st_read/src/new.bin: no line in '
                          'SHA256SUMS; tools/sums.py --add gives it one\n', output)
            subprocess.run([sys.executable, 'tools/sums.py', '--add', 'build/link/1st_read/src/new.bin'], cwd=root,
                           check=True)
            self.assertEqual(make(root, 'source', 'SOURCE=src/new.c', 'CC=true'),
                             (0, '  sums   1 of 1 links match SHA256SUMS\n'))

    def test_stops_on_a_source_no_table_places(self):
        # a link is named by its source's path less the extension, which src/main.typo shares with src/main.c
        for source in ('src/gone.c', 'src/main.typo', 'src/main.c modules/shared/a.c', 'src/%'):
            with self.subTest(source), checkout(PINNED) as root:
                status, output = make(root, '-n', 'source', f'SOURCE={source}')
                self.assertNotEqual(status, 0, output)
                self.assertRegex(output, rf'\AMakefile:\d+: \*\*\* SOURCE={re.escape(source)}: make source takes a '
                                         r'source that a splits\.txt places\.  Stop\.\n\Z')


# lint's settings, among them modules/.clang-tidy, which clang-tidy reads for
# a.c and which inherits the root's; its tools, which make takes for
# installed; each C source's lint target, whose .d file names the header a.c
# includes; and the other checks', passed
LINT = {'.clang-format': '', '.clang-tidy': '', 'modules/.clang-tidy': 'InheritParentConfig: true\n',
        'compile_flags.txt': '-Imodules\n', 'config/intrinsics.h': '', 'config/intrinsics_machine.h': '',
        **INSTALLED,
        'modules/shared/a.c': '#include "shared/a.h"\n', 'modules/shared/a.h': '',
        'build/lint/tidy-configs.txt': '.clang-tidy modules/.clang-tidy\n',
        'build/lint/files.txt': 'modules/shared/a.c modules/shared/a.h modules/stg13/b.src src/main.c\n',
        'build/lint/modules/shared/a.c.tidy': '', 'build/lint/src/main.c.tidy': '',
        'build/lint/modules/shared/a.c.d': depends('build/lint/modules/shared/a.c.tidy', 'modules/shared/a.c',
                                                    'modules/shared/a.h'),
        'build/lint/src/main.c.d': depends('build/lint/src/main.c.tidy', 'src/main.c'),
        **{f'build/lint/{check}.ok': '' for check in ('format', 'decls', 'guards', 'tags', 'asm')}, 'build/shckit': '',
        'build/tools/vendored.ok': ''}
TIDY = 'tools/llvm/bin/clang-tidy --quiet modules/shared/a.c'
DECLS = 'python3 tools/decls.py'
GUARDS = 'python3 tools/guards.py'
TAGS = 'python3 tools/tags.py'


class Lint(unittest.TestCase):
    def test_invalidates_tidy_when_source_or_included_header_settings_change(self):
        sources = ('modules/shared/a.c', 'src/main.c')
        # Both the source and the header included across the directory boundary
        # can contribute settings to a clang-tidy run.
        files = {**LINT, 'src/main.c': '#include "shared/a.h"\n',
                 'build/lint/src/main.c.d': depends('build/lint/src/main.c.tidy', 'src/main.c', 'modules/shared/a.h')}
        with checkout(files) as root:
            stand_in(root, 'tools/llvm/bin/clang-tidy', '#!/bin/sh\nprintf "%s\\n" "$2" >> tidy.calls\n')
            settle(root)

            def checked():
                calls = root / 'tidy.calls'
                calls.write_text('')
                status, output = make(root, *(f'build/lint/{source}.tidy' for source in sources))
                self.assertEqual(status, 0, output)
                return calls.read_text().splitlines()

            self.assertEqual(checked(), [])
            parent = root / 'modules/.clang-tidy'
            nested = root / 'modules/shared/.clang-tidy'
            parent.unlink()
            self.assertEqual(checked(), list(sources))
            nested.write_text('InheritParentConfig: true\n')
            self.assertEqual(checked(), list(sources))
            parent.write_text('InheritParentConfig: true\n')
            self.assertEqual(checked(), list(sources))
            parent.unlink()
            self.assertEqual(checked(), list(sources))
            nested.write_text('InheritParentConfig: false\n')
            self.assertEqual(checked(), list(sources))
            nested.rename(root / 'src/.clang-tidy')
            self.assertEqual(checked(), list(sources))
            self.assertEqual(checked(), [])
            os.utime(root / 'modules/shared/a.h')
            self.assertEqual(checked(), list(sources))

    def test_reruns_a_check_whose_dependency_file_is_gone(self):
        # only the .d file names a.c's header to its lint target, so a lint target whose .d file is gone reruns, as an
        # object does, and a header changed since can't pass unchecked
        with checkout(LINT) as root:
            settle(root)
            status, output = make(root, '-n', 'lint')
            self.assertEqual(status, 0, output)
            self.assertNotIn(TIDY, output)
            (root / 'build/lint/modules/shared/a.c.d').unlink()
            os.utime(root / 'modules/shared/a.h')
            status, output = make(root, '-n', 'lint')
        self.assertEqual(status, 0, output)
        self.assertIn(TIDY, output)

    def test_reads_the_dependency_file_of_a_lint_target_named_alone(self):
        # make build/lint/modules/shared/a.c.tidy, without lint, still reruns a.c's check when its header changes
        with checkout(LINT) as root:
            settle(root)
            status, output = make(root, '-n', 'build/lint/modules/shared/a.c.tidy')
            self.assertEqual(status, 0, output)
            self.assertNotIn(TIDY, output)
            os.utime(root / 'modules/shared/a.h')
            status, output = make(root, '-n', 'build/lint/modules/shared/a.c.tidy')
        self.assertEqual(status, 0, output)
        self.assertIn(TIDY, output)

    def test_reads_engines_c_and_the_kernels_tests_c_where_they_are(self):
        # engine/ holds C that make dc alone compiles, and tests/kernels/ C the kernels' test compiles for the host.
        # decls, guards and tags read both, and so does clang-format but for sh4_fpu_approx.h, which keeps its
        # author's form. clang-tidy reads neither, since both include the host's C library
        extra = {'engine/dc/a.c': '#include "a.h"\n', 'engine/dc/a.h': '', 'tests/kernels/b.c': '',
                 'tests/kernels/sh4_fpu_approx.h': ''}
        with checkout({**LINT, **extra}) as root:
            settle(root)
            os.utime(root / 'engine/dc/a.h')
            status, output = make(root, '-n', 'lint')
            self.assertEqual(status, 0, output)
            self.assertNotIn('clang-tidy --quiet engine/', output)
            self.assertNotIn('clang-tidy --quiet tests/', output)
            checks = [line for line in output.splitlines() if line.startswith((DECLS, GUARDS, TAGS))]
            self.assertEqual(len(checks), 3, output)
            read = {'engine/dc/a.c', 'engine/dc/a.h', 'tests/kernels/b.c', 'tests/kernels/sh4_fpu_approx.h'}
            self.assertEqual([check for check in checks if not read <= set(check.split())], [], output)
            os.utime(root / '.clang-format')
            status, output = make(root, '-n', 'lint')
            self.assertEqual(status, 0, output)
            formats = [line for line in output.splitlines() if 'clang-format --dry-run' in line]
            self.assertEqual(len(formats), 1, output)
            self.assertLessEqual({'engine/dc/a.c', 'engine/dc/a.h', 'tests/kernels/b.c'}, set(formats[0].split()))
            self.assertNotIn('tests/kernels/sh4_fpu_approx.h', formats[0])
            status, output = make(root, '-n', 'format')
        self.assertEqual(status, 0, output)
        self.assertIn('engine/dc/a.c', output)
        self.assertNotIn('sh4_fpu_approx.h', output)

    def test_tidies_each_source_and_builds_shckit_again_once_cc_changes(self):
        # CC writes each lint target's .d file, and builds build/shckit, which checks the assembly
        shckit = '-o build/shckit tools/shckit/shckit.c tools/shckit/main.c'
        with checkout(LINT) as root:
            settle(root)
            for cc, runs in (('cc', []), ('cc -O0', [TIDY, shckit])):
                with self.subTest(cc=cc):
                    status, output = make(root, '-n', 'lint', f'CC={cc}')
                    self.assertEqual(status, 0, output)
                    self.assertEqual([step for step in (TIDY, shckit) if step in output], runs, output)

    # each file a check reads that no .d file names, and the check a change to it reruns
    CHANGED = [
        ('compile_flags.txt', TIDY),
        ('.clang-tidy', TIDY),
        ('modules/.clang-tidy', TIDY),
        ('config/intrinsics.h', TIDY),
        ('config/intrinsics_machine.h', TIDY),
        ('tools/decls.py', DECLS),
        ('tools/guards.py', GUARDS),
        ('tools/csource.py', GUARDS),
        ('tools/layout.py', GUARDS),
        ('config/stg13/splits.txt', TAGS),
        ('config/1st_read/symbols.txt', TAGS),
        ('tools/tags.py', TAGS),
        ('tools/inline.py', TAGS),
        ('tools/split.py', TAGS),
    ]

    def test_reruns_the_check_a_changed_file_feeds(self):
        with checkout(LINT) as root:
            settle(root)
            status, output = make(root, '-n', 'lint')
            self.assertEqual(status, 0, output)
            self.assertEqual([check for check in (TIDY, DECLS, GUARDS, TAGS) if check in output], [], output)
            for path, check in self.CHANGED:
                with self.subTest(path):
                    os.utime(root / path)
                    try:
                        status, output = make(root, '-n', 'lint')
                        self.assertEqual(status, 0, output)
                        self.assertIn(check, output)
                    finally:
                        settle(root)

    def test_checks_the_whole_tree_again_once_a_file_is_added_with_an_old_date_or_removed(self):
        # a header copied in with its old date is newer than no check's stamp, and a removed file leaves none newer
        with checkout(LINT) as root:
            settle(root)
            header = root / 'src/b.h'

            def add():
                header.write_text('')
                os.utime(header, (0, 0))

            for name, edit in (('added', add), ('removed', header.unlink)):
                with self.subTest(name):
                    edit()
                    status, output = make(root, '-n', 'lint')
                    self.assertEqual(status, 0, output)
                    for check in (DECLS, GUARDS, TAGS, 'clang-format --dry-run'):
                        self.assertIn(check, output)
                    formats = [line for line in output.splitlines() if 'clang-format --dry-run' in line]
                    self.assertIn('src/main.c', formats[0].split())
                    settle(root)

    # a source with bodies, with its table and its row in the main executable
    BODY_SOURCE = {**PINNED, **LINT, 'config/bodies.mk': (ROOT / 'config/bodies.mk').read_text(),
                   **placing('D 0x8c010200 0x8c010208\nsrc/main.c D 0x8c010200 0x8c010208\n'),
                   'offsets/src/main.txt': 'datum counts D 0x00000000 8\n'}

    def test_include_cleaner_reads_a_body_source_with_its_bodies_in_place(self):
        # include-cleaner reads a copy with the body on its include's line, which the overlay shows clang-tidy as
        # src/main.c. Every other check reads src/main.c as it is
        source = 'static int counts[2] = {\n#include "main/counts.inc"\n};\n'
        with checkout({**self.BODY_SOURCE, 'src/main.c': source}) as root:
            stand_in(root, 'tools/llvm/bin/clang-tidy', '#!/bin/sh\necho "$*" >> tidy.calls\n')
            status, output = make(root, 'build/lint/src/main.c.tidy')
            self.assertEqual(status, 0, output)
            self.assertEqual((root / 'tidy.calls').read_text().splitlines(), [
                '--quiet src/main.c --extra-arg-before=-Ibuild/stub/src --checks=-misc-include-cleaner',
                '--quiet --checks=-*,misc-include-cleaner --vfsoverlay=build/lint/src/main.c.vfs src/main.c '
                '--extra-arg-before=-Ibuild/stub/src'])
            self.assertEqual((root / 'build/lint/src/main.c').read_text(),
                             source.replace('#include "main/counts.inc"', '0, 0 /* NOLINT(misc-include-cleaner) */'))
            self.assertEqual((root / 'build/lint/src/main.c.vfs').read_text(),
                             '{"version": 0, "use-external-names": false, "roots": [{"name": "src/main.c", '
                             '"type": "file", "external-contents": "build/lint/src/main.c"}]}\n')
            # a change to the copy's writer writes the bodies again, and so checks the copy again
            for changed, calls in ((None, 0), ('tools/body_data.py', 2)):
                settle(root)
                if changed:
                    os.utime(root / changed)
                (root / 'tidy.calls').write_text('')
                status, output = make(root, 'build/lint/src/main.c.tidy')
                self.assertEqual(status, 0, output)
                self.assertEqual(len((root / 'tidy.calls').read_text().splitlines()), calls, output)

    def test_a_variant_reads_its_own_bodies_ahead_of_its_providers(self):
        # compile_flags.txt searches build/stub/src/, which holds the provider's body by the name the variant's takes.
        # The variant's scan for its headers and both its clang-tidy runs search its own directory first
        with checkout({**PINNED, **LINT, **VARIANT}) as root:
            stand_in(root, 'tools/llvm/bin/clang-tidy', '#!/bin/sh\necho "$*" >> tidy.calls\n')
            status, output = make(root, PROVIDED, 'build/lint/src/variant.c.tidy')
            self.assertEqual(status, 0, output)
            scanned = (root / 'build/lint/src/variant.c.d').read_text().split()
            calls = (root / 'tidy.calls').read_text().splitlines()
        self.assertIn(OWN, scanned)
        self.assertNotIn(PROVIDED, scanned)
        self.assertEqual(len(calls), 2, calls)
        self.assertEqual([call for call in calls if f'--extra-arg-before=-I{VARIANT_ROOT}' not in call], [])

    @unittest.skipUnless((ROOT / 'tools/llvm/bin/clang-tidy').is_file(), 'needs the clang-tidy make lint installs')
    def test_a_header_whose_names_only_a_body_spells_is_used(self):
        # The body alone spells `count`, which count.h declares, and `total`, which count.h takes from total.h. So
        # count.h is used, and the body's line asks for no include of total.h. The one warning is at the include of
        # unused.h, which declares nothing. No header is main.h, since include-cleaner never flags a source's own
        # header
        settings = {name: (ROOT / name).read_text() for name in ('.clang-tidy', 'compile_flags.txt',
                                                                 'config/intrinsics.h', 'config/intrinsics_machine.h')}
        headers = {'src/count.h': '#include "total.h"\nextern int count;\n', 'src/total.h': 'extern int total;\n',
                   'src/unused.h': ''}
        files = {**self.BODY_SOURCE, **settings,
                 **{path: f'#pragma once\n{text}' for path, text in headers.items()},
                 'src/main.c': '#include "count.h"\n#include "unused.h"\n\n'
                               'static int *counts[2] = {\n#include "main/counts.inc"\n};\n',
                 'offsets/src/main.bindings': 'pointer counts 0x00000000 count\npointer counts 0x00000004 total\n',
                 'config/1st_read/symbols.txt': '0x8c010000 main\n0x8c010100 count\n0x8c010104 total\n'}
        with checkout(files) as root:
            (root / 'tools/llvm/bin/clang-tidy').unlink()
            (root / 'tools/llvm/bin/clang-tidy').symlink_to(ROOT / 'tools/llvm/bin/clang-tidy')
            status, output = make(root, 'build/lint/src/main.c.tidy')
        self.assertNotEqual(status, 0, output)
        self.assertEqual(re.findall(r'^\S*?(src/main\.c:\d+:\d+): error: (.*) \[', output, re.M),
                         [('src/main.c:2:1', 'included header unused.h is not used directly')], output)

# a sha256sum that, given -c, reads a list from its standard input only where
# the list is named -, and passes an empty list, which GNU's fails, as
# macOS's own sha256sum does. Otherwise it runs the host's
MACOS_SHA256SUM = ('#!/bin/sh\nfor last; do :; done\ncase " $* " in *" -c "*)\n'
                   '    [ "$last" = - ] || { echo "usage: sha256sum [-bctwz] [files ...]" >&2; exit 1; }\n'
                   '    list=$(cat)\n    [ -n "$list" ] || exit 0\n'
                   f'    printf "%s\\n" "$list" | {shutil.which("sha256sum")} "$@"; exit $?;;\nesac\n'
                   f'exec {shutil.which("sha256sum")} "$@"\n')


def macos_sha256sum(root):
    """A PATH whose sha256sum is MACOS_SHA256SUM, which it puts under root."""
    stand_in(root, 'bin/sha256sum', MACOS_SHA256SUM)
    return f'{root / "bin"}{os.pathsep}{os.environ["PATH"]}'


class Originals(unittest.TestCase):
    # what make says where orig/ lacks the disc: where to put it, then a goal that needs none, make check, or make
    # dc-check for a port goal or on a Mac, which refuses make check
    LACKS = ('orig/ lacks 2 of the 2 files the build needs, 1ST_READ.BIN among them.\n'
             "  Put them in orig/ as the disc names them, or put the disc's .gdi and its tracks there.\n")
    CHECK = '  Without the disc, make check checks the linked sources.\n'
    DC_CHECK = "  Without the disc, make dc-check builds and sums the port's objects.\n"
    STOP = r'Makefile:\d+: \*\*\* no disc in orig/\.  Stop\.\n\Z'
    # build/gdi's stand-in, which finds the image by the name make gives it, and writes each file it is asked for as
    # ORIGINALS holds it; SHA256SUMS's lines for ORIGINALS; and those lines but STG13.PRS's
    EXTRACT = ('#!/bin/sh\n[ "$1" = extract ] && [ -f "$2" ] || exit 1\ndir=$3\nshift 3\n'
               'for f; do mkdir -p "$dir/$(dirname "$f")" && printf %s "$f" > "$dir/$f" || exit 1; done\n')
    SUMS = ''.join(f'{hashlib.sha256(data).hexdigest()}  {path}\n' for path, data in sorted(ORIGINALS.items()))
    UNLISTED = ''.join(line for line in SUMS.splitlines(True) if not line.endswith('  orig/SONIC2/STG13.PRS\n'))

    def test_stops_without_the_disc_before_any_step(self):
        with checkout(PINNED) as root:
            status, output = make(root, '-n')
        self.assertNotEqual(status, 0, output)
        self.assertRegex(output, rf'\A{re.escape(self.LACKS + self.CHECK)}{self.STOP}')

    def test_names_make_dc_check_on_a_mac_without_the_disc(self):
        # a Mac refuses make check, so the stop names the goal it runs without the disc
        with checkout() as root:
            for mac in MACS:
                with self.subTest(mac):
                    status, output = make(root, '-n', 'build/orig/SONIC2/STG13.BIN', uname=mac)
                    self.assertNotEqual(status, 0, output)
                    self.assertRegex(output, rf'\A{re.escape(self.LACKS + self.DC_CHECK)}{self.STOP}')

    def test_checks_an_original_in_place_with_a_sha256sum_like_macos_s(self):
        # the original matches its line, and another file in its place doesn't
        stamp = 'build/verified/1ST_READ.BIN.ok'
        with checkout({**ORIGINALS, 'SHA256SUMS': self.SUMS}) as root:
            self.assertEqual(make(root, stamp, path=macos_sha256sum(root), uname=MACS[0]), (0, ''))
        with checkout({**ORIGINALS, 'orig/1ST_READ.BIN': 'another', 'SHA256SUMS': self.SUMS}) as root:
            status, output = make(root, stamp, path=macos_sha256sum(root), uname=MACS[0])
            self.assertNotEqual(status, 0, output)
            self.assertIn('orig/1ST_READ.BIN: not the original file; replace it\n', output)
            self.assertFalse((root / stamp).exists())

    def test_takes_the_originals_from_disc_files_with_a_sha256sum_like_macos_s(self):
        # each file of DISC_FILES that matches its line reaches orig/, and one SHA256SUMS gives no line doesn't
        disc = {f'files/{path[len("orig/"):]}': data for path, data in ORIGINALS.items()}
        with checkout({**disc, 'SHA256SUMS': self.SUMS}) as root:
            self.assertEqual(make(root, *ORIGINALS, 'DISC_FILES=files', path=macos_sha256sum(root), uname=MACS[0]),
                             (0, ''))
            self.assertEqual({path: (root / path).read_bytes() for path in ORIGINALS}, ORIGINALS)
        with checkout({**disc, 'SHA256SUMS': self.UNLISTED}) as root:
            status, output = make(root, *ORIGINALS, 'DISC_FILES=files', path=macos_sha256sum(root), uname=MACS[0])
            self.assertNotEqual(status, 0, output)
            self.assertRegex(output, r'\Aorig/SONIC2/STG13\.PRS: not in SHA256SUMS\nmake: \*\*\* .* Error 1\n\Z')
            self.assertEqual([(root / path).exists() for path in (*ORIGINALS, 'orig/SONIC2/STG13.PRS.part')],
                             [True, False, False])

    def test_extracts_from_a_gdi_with_a_sha256sum_like_macos_s(self):
        # every file extracted matches its line, or none reaches orig/: here one has no line
        for sums, said in ((self.SUMS, ''), (self.UNLISTED, 'orig/SONIC2/STG13.PRS: not in SHA256SUMS\n')):
            with self.subTest(listed=not said), checkout({'orig/disc.gdi': '', 'build/tools/vendored.ok': '',
                                                          'SHA256SUMS': sums}) as root:
                stand_in(root, 'build/gdi', self.EXTRACT)
                settle(root)
                status, output = make(root, 'unpack', path=macos_sha256sum(root), uname=MACS[0])
                self.assertEqual(status != 0, bool(said), output)
                self.assertRegex(output, rf'\A  gdi    orig/disc\.gdi\n{re.escape(said)}')
                self.assertEqual([(root / path).exists() for path in ORIGINALS], [not said] * 2)

    def test_stops_on_more_than_one_gdi_and_names_each(self):
        with checkout({**PINNED, 'orig/a.gdi': '', 'orig/b.gdi': ''}) as root:
            status, output = make(root, '-n')
        self.assertNotEqual(status, 0, output)
        self.assertRegex(output, r'\AMakefile:\d+: \*\*\* orig/ has 2 \.gdi files, and make takes one: '
                                 r'(orig/a\.gdi orig/b\.gdi|orig/b\.gdi orig/a\.gdi)\.  Stop\.\n\Z')

    def test_extracts_from_a_gdi_before_it_compiles(self):
        # extraction checks each file it extracts, so a wrong .gdi fails before any compile
        with checkout({**PINNED, 'orig/disc.gdi': ''}) as root:
            status, output = make(root, '-n')
        self.assertEqual(status, 0, output)
        lines = output.splitlines()
        extract = next(index for index, line in enumerate(lines) if line.startswith('build/gdi extract'))
        compiling = next(index for index, line in enumerate(lines) if 'shc.exe' in line)
        self.assertLess(extract, compiling, output)

    def test_extracts_from_a_gdi_whose_name_holds_quotes_or_a_dollar(self):
        # a .gdi takes whatever name its owner gives it, and EXTRACT finds the image by that name
        for gdi in ("orig/Sonic Adventure 2 (Tails' copy).gdi", 'orig/disc $HOME "two" `x`.gdi'):
            with self.subTest(gdi=gdi), checkout({gdi: '', 'build/tools/vendored.ok': '', 'SHA256SUMS': self.SUMS}) \
                    as root:
                stand_in(root, 'build/gdi', self.EXTRACT)
                settle(root)
                status, output = make(root, 'unpack')
                self.assertEqual((status, output), (0, f'  gdi    {gdi}\n'))
                self.assertEqual({path: (root / path).read_bytes() for path in ORIGINALS}, ORIGINALS)

    def test_unpack_stops_where_orig_lacks_files_and_has_no_one_gdi(self):
        # with no .gdi, as every goal that needs the disc stops, and with two, naming each: either way orig/ stays
        # as it was
        with checkout() as root:
            status, output = make(root, 'unpack')
            self.assertNotEqual(status, 0, output)
            self.assertRegex(output, rf'\A{re.escape(self.LACKS + self.CHECK)}{self.STOP}')
            self.assertEqual([(root / path).exists() for path in ORIGINALS], [False] * 2)
        with checkout({'orig/a.gdi': '', 'orig/b.gdi': ''}) as root:
            status, output = make(root, 'unpack')
            self.assertNotEqual(status, 0, output)
            self.assertRegex(output, r'\AMakefile:\d+: \*\*\* orig/ has 2 \.gdi files, and make takes one: '
                                     r'(orig/a\.gdi orig/b\.gdi|orig/b\.gdi orig/a\.gdi)\.  Stop\.\n\Z')
            self.assertEqual([(root / path).exists() for path in ORIGINALS], [False] * 2)

    def test_unpack_takes_the_originals_from_disc_files_and_has_nothing_to_do_once_orig_holds_them(self):
        disc = {f'files/{path[len("orig/"):]}': data for path, data in ORIGINALS.items()}
        with checkout({**disc, 'SHA256SUMS': self.SUMS}) as root:
            self.assertEqual(make(root, 'unpack', 'DISC_FILES=files', path=macos_sha256sum(root)), (0, ''))
            self.assertEqual({path: (root / path).read_bytes() for path in ORIGINALS}, ORIGINALS)
            # orig/ lacks nothing now, so unpack needs no disc
            status, output = make(root, 'unpack')
            self.assertEqual(status, 0, output)
            self.assertNotIn('***', output)


# a curl that writes "wheel" to the file -o names
CURL = '#!/bin/sh\nwhile [ $# -gt 1 ]; do [ "$1" = -o ] && out=$2; shift; done\nprintf wheel > "$out"\n'


def listing(paths):
    """tools/SHA256SUMS's lines for paths, each with the hash of CURL's file."""
    return ''.join(f'{hashlib.sha256(b"wheel").hexdigest()}  {path}\n' for path in paths)


class Downloads(unittest.TestCase):
    def test_names_each_download_as_it_starts(self):
        # tools/SHA256SUMS lists no SHC release and no wibo, so make refuses each before curl runs, and -k goes on
        # to the next. COMPILERS's routes all take R13, so make needs no R11. CC=true passes the host check, as
        # nothing here compiles
        with checkout() as root, host() as path:
            status, output = make(root, '-k', 'check', 'CC=true', path=path)
        self.assertNotEqual(status, 0, output)
        for download in ('tools/shc-v5.1r13.tar.gz', 'tools/shc-v5.0r32.tar.gz', 'tools/wibo-i686'):
            self.assertIn(f'  fetch  {download}\n{download}: not in tools/SHA256SUMS\n', output)

    def test_moves_a_wheel_into_place_only_once_tools_sha256sums_lists_its_hash(self):
        # tools/SHA256SUMS gives clang-format's wheel the hash of CURL's file, and clang-tidy's no line, a line only
        # for a longer path, or another hash, so make moves the first into place, refuses the second and installs
        # neither
        for second, refusal in (('', f'{WHEELS[1]}: not in tools/SHA256SUMS\n'),
                                (listing([f'{WHEELS[1]}.old']), f'{WHEELS[1]}: not in tools/SHA256SUMS\n'),
                                (f'{HASH}  {WHEELS[1]}\n', f'{WHEELS[1]}.part: FAILED\n')):
            with self.subTest(refusal=refusal), checkout({'tools/SHA256SUMS': listing(WHEELS[:1]) + second}) as root:
                stand_in(root, 'bin/curl', CURL)
                status, output = make(root, 'format', path=f'{root / "bin"}{os.pathsep}{os.environ["PATH"]}')
                self.assertNotEqual(status, 0, output)
                self.assertIn(refusal, output)
                self.assertEqual((root / WHEELS[0]).read_bytes(), b'wheel')
                self.assertFalse((root / WHEELS[1]).exists())
                self.assertFalse((root / (WHEELS[1] + '.part')).exists())
                self.assertFalse((root / 'tools/llvm/bin').exists())

    def test_installs_clang_format_and_clang_tidy_from_their_host_s_wheels_alone_then_removes_the_wheels(self):
        # pip reads no index, and installs no package a wheel depends on; nothing reads a wheel once it is installed.
        # The Makefile pins one release's wheel of each tool for x86-64 Linux and for a Mac of either processor, and
        # make fetches the two of the host uname names, each from PyPI by its project and file name
        for uname, platform in PLATFORMS.items():
            with self.subTest(uname), checkout() as root:
                status, output = make(root, '-n', 'format', uname=uname)
                self.assertEqual(status, 0, output)
                wheels = [wheel.replace(PLATFORMS[LINUX], platform) for wheel in WHEELS]
                for project, wheel in zip(('clang-format', 'clang-tidy'), wheels):
                    url = f'https://files.pythonhosted.org/packages/py2.py3/c/{project}/{Path(wheel).name}'
                    self.assertIn(f' -o {wheel}.part {url} && ', output)
                self.assertIn(f'pip install -q --no-index --no-deps --force-reinstall {" ".join(wheels)}\n'
                              f'rm {" ".join(wheels)}\n', output)

    # what make says on Linux on arm64, a host for which the Makefile pins no wheel
    NO_WHEEL = ('clang-format and clang-tidy need x86-64 Linux or macOS, whose wheels make pins, and this is '
                'Linux aarch64; see README.md#build')

    def test_lint_and_format_stop_on_a_host_whose_wheels_it_doesn_t_pin(self):
        # Linux on arm64 can't run the programs x86-64 Linux's wheels hold. make stops before it writes anything,
        # under -n too, with the tools installed or not
        for installed in ({}, INSTALLED):
            with self.subTest(installed=bool(installed)), checkout(installed) as root:
                for goals in (('lint',), ('format',), ('check', 'format')):
                    for flags in ((), ('-n',)):
                        with self.subTest(goals=goals, flags=flags):
                            status, output = make(root, *flags, *goals, uname='Linux aarch64')
                            self.assertEqual(status, 2, output)
                            self.assertRegex(output, stopped(self.NO_WHEEL))
                self.assertEqual(written(root), set())

    def test_the_install_stops_a_goal_that_reaches_it_on_a_host_whose_wheels_it_doesn_t_pin(self):
        # a tool the install puts in place is no goal make stops for as it starts. The install's recipe stops make
        # instead, under -n too, and curl, which RAN stands in for, fetches nothing
        with checkout() as root:
            stand_in(root, 'bin/curl', RAN)
            for flags in ((), ('-n',)):
                with self.subTest(flags=flags):
                    status, output = make(root, *flags, 'tools/llvm/bin/clang-format',
                                          path=f'{root / "bin"}{os.pathsep}{os.environ["PATH"]}', uname='Linux aarch64')
                    self.assertEqual(status, 2, output)
                    self.assertRegex(output, rf'\*\*\* {re.escape(self.NO_WHEEL)}\.  Stop\.\n\Z')
            self.assertFalse((root / 'ran').exists())


# a pip that installs each wheel's tool, named and versioned by the wheel's
# file name, and leaves a wheel installed at its version alone unless given
# --force-reinstall, as pip does. It adds its command line to its
# environment's pip.log
PIP = f'#!{sys.executable}\n' + '''import sys
from pathlib import Path
env = Path(sys.argv[0]).parents[1]
with open(env / 'pip.log', 'a') as log:
    print(*sys.argv[1:], file=log)
for wheel in [Path(arg).name for arg in sys.argv[1:] if arg.endswith('.whl')]:
    name, version = wheel.split('-')[:2]
    installed = env / 'lib' / name
    if '--force-reinstall' in sys.argv or not installed.exists() or installed.read_text() != version:
        installed.parent.mkdir(exist_ok=True)
        installed.write_text(version)
        tool = env / 'bin' / name.replace('_', '-')
        tool.write_text(f'#!/bin/sh\\n# {wheel}\\n')
        tool.chmod(0o755)
'''
# a python3 that runs as this one does, but for -m venv DIR, which makes DIR
# an environment whose pip is PIP, keeps one already there, as venv keeps its
# packages, and refuses a DIR that is a link, as venv does
VENV = f'#!{sys.executable}\n' + f'''import os, sys
if sys.argv[1:3] != ['-m', 'venv']:
    os.execv(sys.executable, [sys.executable, *sys.argv[1:]])
if os.path.islink(sys.argv[3]):
    sys.exit('Error: Unable to create directory ' + repr(sys.argv[3]))
pip = os.path.join(sys.argv[3], 'bin/pip')
if not os.path.exists(pip):
    os.makedirs(os.path.dirname(pip), exist_ok=True)
    with open(pip, 'w') as file:
        file.write({PIP!r})
    os.chmod(pip, 0o755)
'''
# WHEELS as a later release names them
LATER = [wheel.replace(f'-{wheel.split("-")[1]}-', '-99.0.0-') for wheel in WHEELS]


def pin(root, wheels):
    """Pin root's Makefile to wheels in WHEELS' place, and list them in its tools/SHA256SUMS. The Makefile names each
    wheel with $(WHEEL) for the host's platform tag."""
    text = (root / 'Makefile').read_text()
    for wheel, other in zip(WHEELS, wheels):
        text = text.replace(*(name.replace(PLATFORMS[LINUX], '$(WHEEL)') for name in (wheel, other)))
    (root / 'Makefile').write_text(text)
    (root / 'tools/SHA256SUMS').write_text(listing(wheels))


def pips(root):
    """How many times make has run the pip of root's tools/llvm."""
    log = root / 'tools/llvm/pip.log'
    return len(log.read_text().splitlines()) if log.exists() else 0


class Install(unittest.TestCase):
    def test_installs_the_wheels_again_when_a_tool_is_gone(self):
        # pip finds each wheel installed, so only a forced install brings the tool back
        for gone in CLANG_TOOLS:
            with self.subTest(gone=gone), self.stood_in() as (root, path):
                self.assertInstalls(root, path, WHEELS)
                (root / gone).unlink()
                self.assertInstalls(root, path, WHEELS)

    def test_installs_the_wheels_a_changed_pin_names(self):
        with self.stood_in() as (root, path):
            self.assertInstalls(root, path, WHEELS)
            pin(root, LATER)
            self.assertInstalls(root, path, LATER)

    def test_fetches_the_wheels_again_when_a_hash_changes_and_a_refused_one_leaves_the_install(self):
        # tools/SHA256SUMS gives clang-tidy's wheel another hash, so make fetches it again and refuses CURL's file.
        # That stops the install before pip, so the one there keeps its record: with the hash back, make runs no pip
        with self.stood_in() as (root, path):
            self.assertInstalls(root, path, WHEELS)
            (root / 'tools/SHA256SUMS').write_text(listing(WHEELS[:1]) + f'{HASH}  {WHEELS[1]}\n')
            status, output = make(root, 'format', path=path)
            self.assertNotEqual(status, 0, output)
            self.assertIn(f'  fetch  {WHEELS[1]}\n{WHEELS[1]}.part: FAILED\n', output)
            (root / 'tools/SHA256SUMS').write_text(listing(WHEELS))
            status, output = make(root, 'format', path=path)
            self.assertEqual(status, 0, output)
            self.assertEqual(pips(root), 1, output)

    def test_installs_the_wheels_again_after_pip_fails(self):
        # make runs a pip that fails: before it writes a tool, for LATER, or after it writes both, with clang-tidy
        # gone. The record went before pip ran and comes back only after an install, so either way the next make
        # installs the wheels
        for wheels, fails in ((LATER, 'exit 1'), (WHEELS, '"$0.real" "$@"; exit 1')):
            with self.subTest(fails=fails), self.stood_in() as (root, path):
                self.assertInstalls(root, path, WHEELS)
                pin(root, wheels)
                if wheels == WHEELS:
                    (root / 'tools/llvm/bin/clang-tidy').unlink()
                pip = root / 'tools/llvm/bin/pip'
                pip.rename(pip.with_name('pip.real'))
                stand_in(root, 'tools/llvm/bin/pip', f'#!/bin/sh\n{fails}\n')
                status, output = make(root, 'format', path=path)
                self.assertNotEqual(status, 0, output)
                pip.with_name('pip.real').replace(pip)
                self.assertInstalls(root, path, wheels)

    def test_names_a_tool_a_first_install_lacks_and_tries_again(self):
        # the Makefile pins a wheel of clang-tidy-extra in place of clang-tidy's, so a first install ends with no
        # clang-tidy and no record, and the next make tries it again
        lacking = [WHEELS[0], WHEELS[1].replace('/clang_tidy-', '/clang_tidy_extra-')]
        with self.stood_in() as (root, path):
            pin(root, lacking)
            for runs in (1, 2):
                status, output = make(root, 'format', path=path)
                self.assertNotEqual(status, 0, output)
                self.assertIn('tools/llvm/bin: no clang-tidy\n', output)
                self.assertEqual(pips(root), runs, output)

    def test_installs_with_a_sha256sum_like_macos_s(self):
        with self.stood_in() as (root, path):
            stand_in(root, 'bin/sha256sum', MACOS_SHA256SUM)
            self.assertInstalls(root, path, WHEELS)

    @contextlib.contextmanager
    def stood_in(self):
        """A checkout with CURL and VENV for curl and python3, and the PATH that finds them."""
        with checkout({'tools/SHA256SUMS': listing(WHEELS)}) as root:
            stand_in(root, 'bin/curl', CURL)
            stand_in(root, 'bin/python3', VENV)
            yield root, f'{root / "bin"}{os.pathsep}{os.environ["PATH"]}'

    def assertInstalls(self, root, path, wheels):
        """make format, in root, installs each tool from its wheel of wheels, and make format again runs no pip."""
        for runs in (1, 0):
            ran = pips(root)
            status, output = make(root, 'format', path=path)
            self.assertEqual(status, 0, output)
            self.assertEqual([(root / tool).read_text() if (root / tool).exists() else None for tool in CLANG_TOOLS],
                             [f'#!/bin/sh\n# {Path(wheel).name}\n' for wheel in wheels])
            self.assertEqual(pips(root) - ran, runs, output)


class Stamps(unittest.TestCase):
    def test_stops_on_a_tool_it_can_t_hash(self):
        # R32's shc.exe changed, beside a directory sha256sum can't read
        with built('CC=true') as root:
            (root / 'tools/shc-v5.0r32/shc.exe').write_text('another')
            (root / 'tools/shc-v5.0r32/notes.d').mkdir()
            status, output = make(root, 'check', 'CC=true')
        self.assertNotEqual(status, 0, output)
        self.assertIn('tools/shc-v5.0r32/notes.d: Is a directory\n', output)
        self.assertRegex(output, r"Makefile:\d+: \*\*\* build/tools/R32\.id: can't refresh it from "
                                 r'tools/shc-v5\.0r32/\*\.\*\.  Stop\.\n\Z')

    def test_stops_on_a_tool_outside_tools_that_isn_t_there_with_nothing_to_rebuild(self):
        with built('CC=true') as root:
            self.assertEqual(make(root, 'check', 'CC=true'), (0, '  sums   3 files match SHA256SUMS\n'))
            status, output = make(root, 'check', 'R32=custom', 'CC=true')
        self.assertNotEqual(status, 0, output)
        self.assertRegex(output, r'\AMakefile:\d+: \*\*\* custom/shc\.exe: no such file\.  Stop\.\n\Z')

    def test_fetches_the_default_r32_after_an_override_and_assembles_with_it_in_the_same_run(self):
        # built with R32=custom, then checked with the default R32, which tools/ lacks: one make fetches it, stamps it
        # and assembles again with it
        files = {**{path: text for path, text in BUILT.items() if path != 'tools/shc-v5.0r32/shc.exe'},
                 'custom/shc.exe': ''}
        with built('R32=custom', files=files) as root:
            status, output = make(root, '-n', 'check', 'R32=custom')
            self.assertEqual(status, 0, output)
            self.assertNotIn('asmsh.exe', output)
            status, output = make(root, '-n', 'check')
        self.assertEqual(status, 0, output)
        steps = ('tools/shc-v5.0r32.tar.gz', 'id="$(sha256sum tools/shc-v5.0r32/*.*)"', 'tools/shc-v5.0r32/asmsh.exe')
        for step in steps:
            self.assertIn(step, output)
        self.assertEqual(sorted(steps, key=output.index), list(steps))

    def test_compiles_nothing_again_with_a_tool_fetched_again_byte_for_byte(self):
        # wibo, CURL's file, gone from tools/ and fetched again
        files = {**BUILT, 'tools/wibo-i686': 'wheel', 'tools/SHA256SUMS': listing(['tools/wibo-i686'])}
        with built('CC=true', files=files) as root:
            stand_in(root, 'bin/curl', CURL)
            (root / 'tools/wibo-i686').unlink()
            status, output = make(root, 'check', 'CC=true', path=f'{root / "bin"}{os.pathsep}{os.environ["PATH"]}')
        self.assertEqual((status, output), (0, '  fetch  tools/wibo-i686\n  sums   3 files match SHA256SUMS\n'))

    def test_builds_again_what_cc_built_once_cc_changes(self):
        # CC builds build/prskit, and writes each C object's .d file in the object's recipe: another CC compiles
        # main.c and a.c again, and assembles no b.src. A tree built before CC had a stamp takes one dated the epoch,
        # and the same CC builds nothing again
        prskit = '-o build/prskit tools/prskit/prskit.c tools/prskit/main.c'
        with built() as root:
            (root / 'build/tools/cc.id').unlink()
            for cc, runs in (('cc', []), ('cc -O0', [MAIN_SHC, SHARED_SHC, prskit])):
                with self.subTest(cc=cc):
                    status, output = make(root, '-n', 'all', f'CC={cc}')
                    self.assertEqual(status, 0, output)
                    self.assertEqual([step for step in (MAIN_SHC, SHARED_SHC, ASM, prskit) if step in output], runs,
                                     output)

    def test_stops_under_make_4_3_where_it_can_t_batch_its_stamps_or_finish_the_batch(self):
        # Under make 4.3, each stamp make rewrites goes to a batch that mktemp makes under build/: here main.c's and
        # a.c's .cmd files, for their route's new flags. tools/keep_stamps.py finishes the batch once make has read
        # the makefiles. --eval sets MAKE_VERSION to 4.3 in whichever make runs the test
        for path, text, stop in (('bin/mktemp', "#!/bin/sh\necho 'mktemp fails' >&2\nexit 1\n",
                                  'could not create the stamp batch'),
                                 ('tools/keep_stamps.py', failing('keep_stamps'), 'could not update build stamps')):
            with self.subTest(path), built() as root:
                replace('config/compilers.mk', '-cpu=sh4 -speed', '-cpu=sh4 -size')(root)
                stand_in(root, path, text)
                status, output = make(root, '-n', 'check', '--eval=MAKE_VERSION := 4.3',
                                      path=f'{root / "bin"}{os.pathsep}{os.environ["PATH"]}')
                self.assertNotEqual(status, 0, output)
                self.assertRegex(output, rf'\A{Path(path).stem} fails\nMakefile:\d+: \*\*\* {stop}\.  Stop\.\n\Z')


class Progress(unittest.TestCase):
    def test_reports_then_writes_the_numbers_and_check_checks_them(self):
        with checkout(PINNED) as root:
            runs = {}
            for goal in ('progress', 'check'):
                status, output = make(root, '-n', goal)
                self.assertEqual(status, 0, output)
                runs[goal] = [line.split()[2:] for line in output.splitlines()
                              if line.startswith('python3 tools/progress.py')]
        self.assertEqual(runs, {'progress': [[], ['--write']], 'check': [['--check']]})


class Vendored(unittest.TestCase):
    def test_checks_the_vendored_files_before_it_builds_a_tool(self):
        # tools/SHA256SUMS records none of the tools' files
        with checkout() as root:
            for tool in vendor.TOOLS:
                status, output = make(root, f'build/{tool}')
                self.assertNotEqual(status, 0, output)
                self.assertIn(f'vendor: tools/SHA256SUMS: no block for {tool}', output)
                self.assertNotIn(f'  cc     build/{tool}', output)
                self.assertFalse((root / 'build' / tool).exists())


def shells(text):
    """Each $(shell ...) in a makefile's text, through its closing parenthesis."""
    for start in (index for index in range(len(text)) if text.startswith('$(shell', index)):
        depth = 0
        for end in range(start + 1, len(text)):
            depth += {'(': 1, ')': -1}.get(text[end], 0)
            if not depth:
                yield text[start:end + 1]
                break


class Bytecode(unittest.TestCase):
    def test_puts_the_bytecode_of_python_a_shell_runs_under_build(self):
        # make before 4.4 passes no exported variable to $(shell), so each that runs Python names the prefix through
        # $(python)
        calls = [call for path in ('Makefile', 'config/compilers.mk') for call in shells((ROOT / path).read_text())
                 if 'python' in call]
        self.assertTrue(calls)
        for call in calls:
            self.assertNotRegex(call, r'(?<![\w(])python3(?![\w-])', call)


class Quiet(unittest.TestCase):
    def test_hides_each_command_unless_v_is_set(self):
        # Q is @ unless V is set, so each command a recipe runs opens with it; a status line prints under either, and
        # a make function that runs no command needs neither
        text = (ROOT / 'Makefile').read_text().replace('\\\n', ' ')
        for line in re.findall(r'^\t.*', text, re.M):
            self.assertRegex(line, r'^\t\$\$?\(Q\)|^\t@\$\$?\(call status,|^\t\$\((file|if|info|error) ')


if __name__ == '__main__':
    unittest.main()
