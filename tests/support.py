"""What the tests share: a scratch repository to run the tools in, and the host's build of the tree's C."""

import contextlib
import hashlib
import io
import os
import shlex
import subprocess
import sys
import tempfile
import unittest
from pathlib import Path

from csource import GCC, code, parse

TOOLS = Path(__file__).resolve().parent.parent / 'tools'
ROOT = TOOLS.parent
# Subprocesses also run in scratch checkouts. Resolve the caller's paths before changing directories.
SH_ELF = Path(os.environ.get('SH_ELF', ROOT / 'tools/sh-elf')).resolve()
SH4ZAM = Path(os.environ.get('SH4ZAM', ROOT / 'tools/sh4zam')).resolve()
# the host's C compiler: CC, as a shell splits it, or cc
CC = shlex.split(os.environ.get('CC', 'cc'))
# the link flag that drops each section no kept symbol reaches
DISCARD = '-Wl,-dead_strip' if sys.platform == 'darwin' else '-Wl,--gc-sections'
# what make reads to give GCCFLAGS: config/gcc.mk, and the two of the
# Makefile's macros it calls
GCCFLAGS = """comma := ,
quoted = '$(subst ','\\'',$1)'
include config/gcc.mk
flags:
	@printf '%s\\n' $(GCCFLAGS)
"""


class TreeTest(unittest.TestCase):
    """A test that runs in an empty directory of its own, as the tools run
    in the repository's root."""

    def setUp(self):
        scratch = tempfile.TemporaryDirectory()
        self.addCleanup(scratch.cleanup)
        home = os.getcwd()
        self.addCleanup(os.chdir, home)
        os.chdir(scratch.name)

    def write(self, path, text):
        """Write text, or bytes, to the file at path, making its directory."""
        path = Path(path)
        path.parent.mkdir(parents=True, exist_ok=True)
        if isinstance(text, bytes):
            path.write_bytes(text)
        else:
            path.write_text(text)

    def image(self, name, splits, symbols=None):
        """Write image name's splits.txt, and its symbols.txt if given."""
        self.write(f'config/{name}/splits.txt', splits)
        if symbols is not None:
            self.write(f'config/{name}/symbols.txt', symbols)

    def fails(self, call, *args, **kwargs):
        """The message call(*args, **kwargs) fails with, as config.fail() exits."""
        with self.assertRaises(SystemExit) as caught:
            call(*args, **kwargs)
        return str(caught.exception.code)


def gcc_flags(sh_elf):
    """GCCFLAGS, make dc's GCC flags for the tree's C, as config/gcc.mk gives
    them with SH_ELF naming the toolchain at sh_elf: one word to a flag, as a
    shell hands them to GCC. A flag holds no level and no include directory
    of a route's. make reads the file from the root, apart from any make
    running this."""
    env = {name: value for name, value in os.environ.items() if name not in ('MAKEFLAGS', 'MFLAGS', 'MAKELEVEL')}
    made = subprocess.run(['make', '-s', '--no-print-directory', '-f', '-', f'SH_ELF={sh_elf}', 'flags'],
                          input=GCCFLAGS, cwd=ROOT, env=env, stdout=subprocess.PIPE, stderr=subprocess.STDOUT,
                          text=True, check=False)
    if made.returncode:
        raise AssertionError(f'make: config/gcc.mk gives no GCCFLAGS:\n{made.stdout}')
    return made.stdout.splitlines()


def compile_host(output, *args, nonmatching=True, sdk_first=False, shared=False):
    """Build the tree's C for the host by CC into the program at output, or with shared into a library ctypes can
    load. args are the sources and the test's own flags, and a relative path is the root's. The build defines
    NONMATCHING unless nonmatching is False, and keeps assert's checks. It reads the SDK's headers after the host's,
    or with sdk_first before them, as tests/fibers/server.c needs for the SDK's 38-word jmp_buf. sg_xpt.h's Uint32
    and Sint32 have 32 bits, as on the SH-4, so a pointer kept in one keeps only its low 32 bits. Each function and
    datum takes a section of its own, and the link drops those nothing reaches, so a source may hold functions whose
    callees the program lacks. A build that fails raises AssertionError with the compiler's output."""
    built = subprocess.run([*CC, '-UNDEBUG', *(['-DNONMATCHING'] if nonmatching else []), '-ffunction-sections',
                            '-fdata-sections', *args, '-isystem' if sdk_first else '-idirafter', 'sdk/include',
                            *(['-shared', '-fPIC'] if shared else []), DISCARD, '-o', output],
                           cwd=ROOT, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True)
    if built.returncode:
        raise AssertionError(f'{output}: the build fails:\n{built.stdout}')


def original_image(path, directory):
    """The image that the original file at path holds, where path is the file's name in SHA256SUMS, from the root.
    The image is the file itself, or for a .PRS, the file decoded to the same path under directory but ending .BIN.
    The first .PRS decoded in directory builds tools/prskit there. A file that has no line in SHA256SUMS, or whose
    SHA-256 differs from its line, raises AssertionError."""
    path, directory = Path(path), Path(directory)
    sums = {name: digest for digest, name in map(str.split, (ROOT / 'SHA256SUMS').read_text().splitlines())}
    if path.as_posix() not in sums:
        raise AssertionError(f'{path}: not in SHA256SUMS')
    if hashlib.sha256((ROOT / path).read_bytes()).hexdigest() != sums[path.as_posix()]:
        raise AssertionError(f'{path}: not the original file')
    if path.suffix != '.PRS':
        return ROOT / path
    decoder = directory / 'prskit'
    if not decoder.exists():
        source = ROOT / 'tools/prskit'
        subprocess.run([*CC, '-std=c99', '-O2', source / 'prskit.c', source / 'main.c', '-o', decoder], check=True)
    decoded = directory / path.with_suffix('.BIN')
    decoded.parent.mkdir(parents=True, exist_ok=True)
    subprocess.run([decoder, 'decompress', ROOT / path, decoded], check=True)
    return decoded


def extract(path, names, prefixes, include):
    """Write to include the GCC view of the source at path: each #define that starts with one of prefixes, then
    each type and function of names, in the order names gives them."""
    source = code(ROOT / path, GCC)
    found = {}
    for block in parse(source).blocks:
        head = ' '.join(token.text for token in block.head)
        name = block.name or head
        if name in names:
            found[name] = head + ' ' + source[block.opening.start:block.closing.end] + ('' if block.name else ';')
    defines = [line for line in source.splitlines() if line.startswith(prefixes)]
    Path(include).write_text('\n'.join([*defines, *(found[name] for name in names)]) + '\n')


def run(main, *argv):
    """main(argv)'s exit status and what it printed and failed with, as (status, text)."""
    out = io.StringIO()
    with contextlib.redirect_stdout(out), contextlib.redirect_stderr(out):
        try:
            main(list(argv))
        except SystemExit as stopped:
            if isinstance(stopped.code, int) or stopped.code is None:
                return stopped.code or 0, out.getvalue()
            return 1, out.getvalue() + str(stopped.code)
    return 0, out.getvalue()


def record(kind, body, last=True):
    """One SYSROF record of kind holding body."""
    head = bytes([kind | (0x80 if last else 0), len(body) + 3]) + body
    return head + bytes([0xFF - sum(head) & 0xFF])


def obj(sections=(), references=(), exports=()):
    """An object's bytes: its section table, as (name, size, alignment,
    code) entries, its references, its exports, as (name, section index,
    offset) entries, and its end record."""
    table = b''.join(b'\x40' + bytes(4) + size.to_bytes(4, 'big') + alignment.to_bytes(4, 'big')
                     + bytes([0 if code else 0x10]) + b'\xff\xc0' + bytes([len(name)]) + name.encode()
                     for name, size, alignment, code in sections)
    names = b''.join(b'\xc0' + bytes([len(name)]) + name.encode() for name in references)
    given = b''.join(index.to_bytes(2, 'big') + b'\x00' + offset.to_bytes(4, 'big') + bytes([len(name)]) + name.encode()
                     for name, index, offset in exports)
    return (record(0x08, table) + (record(0x0C, names) if names else b'') + (record(0x14, given) if given else b'')
            + record(0x7F, b''))


sys.path[:0] = [str(TOOLS)]
