"""Run each tool with a command line in an empty directory, and hold it to the contract TOOLCHAIN.md#tools gives."""

import os
import re
import subprocess
import sys
import tempfile
import unittest

from support import TOOLS

# a tool's entry point, in either quote
MAIN = re.compile(r'''^if __name__ == (['"])__main__\1:''', re.M)


def outside(path):
    """The problem a tool fails with where the working directory lacks path, as one outside the repository does."""
    return f"{path}: not found; run tools from the repository's root"


# each tool with a command line, by name: a command line it takes, and the problem that line fails with in an empty
# directory. Without its last word, the line lacks an argument the tool requires. A tool that requires none has an
# empty line
COMMANDS = {
    'addresses': (['--bodies', 'bodies.txt'], 'config/addresses.txt: No such file or directory'),
    'ask_lexicon': (['lexicon.txt', 'out.inc'], 'lexicon.txt: No such file or directory'),
    'body_data': (['stg01', 'src/a.c', 'build/gen'], 'config/stg01/splits.txt: No such file or directory'),
    'body_inputs': (['out.mk', 'src/a.c'], 'src/a.c: no placement in config/*/splits.txt'),
    'body_links': ([], outside('config/')),
    'body_offsets': (['src/a.c', 'a.lis', 'out.txt'], 'src/a.c: No such file or directory'),
    'census': (['linked.o', 'out.txt'], 'linked.o: No such file or directory'),
    'chao_area': (['--toolchain', 'tools/sh-elf', 'chaostglobby', 'area.o', 'world.elf', 'out.o'],
                  'area.o: No such file or directory'),
    'chao_banks': (['c', 'out.c'], outside('config/')),
    'check_chao_banks': (['model.prs', 'motion.prs'], outside('config/')),
    'chunk_colors': (['stg01', 'a.elf', 'out.txt'], 'config/stg01/splits.txt: No such file or directory'),
    'content': (['stg01'], 'config/stg01/splits.txt: No such file or directory'),
    'dc_extracted': (['manifest.txt', 'disc', 'out'], 'manifest.txt: No such file or directory'),
    'dc_loads': ([], outside('config/loads.txt')),
    'dc_manifest': (['manifest.txt'], outside('config/')),
    'dc_memory': (['stg03', 'build/dc'], outside('src/system/areas.h')),
    'dc_overlay': (['--toolchain', 'tools/sh-elf', 'stg01', 'stg01.o', '1st_read.elf', 'out.elf'],
                   '1st_read.elf: No such file or directory'),
    'dc_publish': (['stg01', 'build/dc'], 'config/stg01/splits.txt: No such file or directory'),
    'decls': (['a.c'], 'a.c: No such file or directory'),
    'disc': (['manifest.txt', 'disc.gdi', 'out'], 'manifest.txt: No such file or directory'),
    'flatten': (['a.c'], 'a.c: No such file or directory'),
    'floats': (['--shc', 'r08', 'a.i'], 'a.i: No such file or directory'),
    'fmacs': (['build/obj'], outside('config/compilers.mk')),
    'guards': (['a.c'], 'a.c: No such file or directory'),
    'keep_stamps': (['build/.stamps.a'], 'build/.stamps.a: No such file or directory'),
    'keeps': ([], outside('config/')),
    'link': (['a.obj', 'stg01'], 'a.obj: no such object'),
    'names': ([], outside('config/')),
    'polygon_lists': (['src/a.c', 'out.c'], 'src/a.c: No such file or directory'),
    'progress': ([], outside('config/')),
    'relink': (['stg01', 'original.bin', 'spliced.bin'], outside('config/')),
    'relocate': ([], outside('offsets/')),
    'splice': (['--check'], outside('config/')),
    'split': (['a.c', 'out.c', 'text.src'], 'a.c: No such file or directory'),
    'sums': (['expected.txt'], 'expected.txt: No such file or directory'),
    'tags': (['a.c'], 'a.c: No such file or directory'),
    'vendor': (['--check'], 'tools/SHA256SUMS: No such file or directory'),
    'windows': ([], outside('src/')),
    'words': (['stg01', 'src/a.c'], 'config/stg01/splits.txt: No such file or directory'),
}


def run(tool, *args):
    """The status, stdout and stderr of tools/TOOL.py run with args in an empty directory."""
    # there, a command line a tool wrongly takes can touch nothing in the checkout
    with tempfile.TemporaryDirectory() as empty:
        # from Python 3.14, argparse colors its usage where the environment asks, and PYTHON_COLORS=0 overrides that
        result = subprocess.run([sys.executable, str(TOOLS / f'{tool}.py'), *args], cwd=empty,
                                env=dict(os.environ, PYTHON_COLORS='0'), capture_output=True, text=True, check=False)
    return result.returncode, result.stdout, result.stderr


class CommandLine(unittest.TestCase):
    def test_covers_every_tool_with_a_command_line(self):
        found = {path.stem for path in TOOLS.glob('*.py') if MAIN.search(path.read_text())}
        self.assertEqual(found, set(COMMANDS))

    def test_prints_its_usage_for_h(self):
        for tool in COMMANDS:
            with self.subTest(tool=tool):
                status, out, err = run(tool, '-h')
                self.assertEqual((status, err), (0, ''))
                self.assertTrue(out.startswith(f'usage: {tool} '), out)

    def test_exits_2_on_an_unknown_option(self):
        for tool, (args, _) in COMMANDS.items():
            with self.subTest(tool=tool):
                status, out, err = run(tool, *args, '--no-such-option')
                self.assertEqual((status, out), (2, ''))
                self.assertTrue(err.startswith(f'usage: {tool} '), err)
                self.assertEqual(err.splitlines()[-1], f'{tool}: error: unrecognized arguments: --no-such-option')

    def test_exits_2_on_a_missing_argument(self):
        for tool, (args, _) in COMMANDS.items():
            if args:
                with self.subTest(tool=tool):
                    status, out, err = run(tool, *args[:-1])
                    self.assertEqual((status, out), (2, ''))
                    self.assertTrue(err.startswith(f'usage: {tool} '), err)
                    self.assertTrue(err.splitlines()[-1].startswith(f'{tool}: error: '), err)

    def test_exits_1_on_a_failure_naming_the_tool(self):
        for tool, (args, problem) in COMMANDS.items():
            with self.subTest(tool=tool):
                self.assertEqual(run(tool, *args), (1, '', f'{tool}: {problem}\n'))


if __name__ == '__main__':
    unittest.main()
