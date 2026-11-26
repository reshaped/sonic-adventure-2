"""The vendored tools' pins: each tool's files are its block's."""

import hashlib
import os
from pathlib import Path
import subprocess
import unittest
from unittest import mock

import vendor
from support import ROOT, SH4ZAM, SH_ELF, TreeTest

COMMIT = '0123456789abcdef0123456789abcdef01234567'


class Vendored(TreeTest):
    def setUp(self):
        super().setUp()
        lines = [f'{"ab" * 32}  tools/wibo-i686']
        for tool in vendor.TOOLS:
            lines.append(f'# https://github.com/reshaped/{tool} {COMMIT}')
            for path in vendor.files(tool):
                self.write(path, path)
                lines.append(f'{hashlib.sha256(path.encode()).hexdigest()}  {path}')
        self.sums = '\n'.join(lines) + '\n'
        self.write('tools/SHA256SUMS', self.sums)

    def test_passes_files_that_are_their_blocks(self):
        vendor.check()

    def test_reads_each_tools_block(self):
        pinned = vendor.blocks(self.sums)
        self.assertEqual(sorted(pinned), sorted(vendor.TOOLS))
        self.assertEqual(pinned['gdi'][:2], ('https://github.com/reshaped/gdi', COMMIT))
        self.assertEqual(sorted(pinned['gdi'][2]), vendor.files('gdi'))

    def test_names_a_changed_an_added_and_a_missing_file(self):
        self.write('tools/gdi/gdi.c', 'edited')
        self.write('tools/prskit/extra.c', '')
        (vendor.Path('tools/shckit/shckit.h')).unlink()
        message = self.fails(vendor.check)
        self.assertIn('tools/gdi/gdi.c: not the file tools/SHA256SUMS pins', message)
        self.assertIn('tools/prskit/extra.c: not in tools/SHA256SUMS', message)
        self.assertIn('tools/shckit/shckit.h: listed in tools/SHA256SUMS, and missing', message)

    def test_a_tool_needs_a_block(self):
        self.write('tools/SHA256SUMS', self.sums.split('# https://github.com/reshaped/shckit')[0])
        self.assertIn('no block for shckit', self.fails(vendor.check))

    def test_a_file_hashed_twice_fails(self):
        right = f'{hashlib.sha256(b"tools/gdi/main.c").hexdigest()}  tools/gdi/main.c'
        self.write('tools/SHA256SUMS', self.sums.replace(right, f'{"cd" * 32}  tools/gdi/main.c\n{right}'))
        self.assertIn('tools/SHA256SUMS:7: tools/gdi/main.c: hashed twice', self.fails(vendor.check))

    def test_a_tools_file_hashed_outside_its_block_fails(self):
        self.write('tools/SHA256SUMS', f'{self.sums}\n{"cd" * 32}  tools/gdi/main.c\n')
        self.assertIn("tools/SHA256SUMS:18: tools/gdi/main.c: outside gdi's block", self.fails(vendor.check))

    def test_names_a_missing_directory_and_a_directory_for_a_file(self):
        for path in vendor.files('gdi'):
            Path(path).unlink()
        Path('tools/gdi').rmdir()
        Path('tools/prskit/main.c').unlink()
        Path('tools/prskit/main.c').mkdir()
        self.write('tools/shckit/shckit.c', 'edited')
        message = self.fails(vendor.check)
        self.assertIn('tools/gdi: missing, or not a directory', message)
        self.assertIn('tools/prskit/main.c: not a file', message)
        self.assertIn('tools/shckit/shckit.c: not the file tools/SHA256SUMS pins', message)

    def test_refresh_rewrites_its_block_and_nothing_else(self):
        upstream = Path('upstream')
        upstream.mkdir()
        for path in vendor.files('gdi'):
            self.write(upstream / Path(path).name, f'new {path}')
        git = ['git', '-C', 'upstream', '-c', 'user.name=t', '-c', 'user.email=t@t']
        for args in (['init', '-q'], ['add', '.'], ['commit', '-q', '-m', 'new'], ['tag', 'v1']):
            subprocess.run([*git, *args], check=True)
        commit = subprocess.run([*git, 'rev-parse', 'HEAD'], check=True, capture_output=True, text=True).stdout.strip()
        head, rest = self.sums.split('# https://github.com/reshaped/prskit')
        tail = f'{"cd" * 32}  tools/other.dat\r\n# https://github.com/reshaped/prskit{rest[:-1]}'
        self.write('tools/SHA256SUMS', head + tail)
        vendor.refresh('gdi', 'v1', upstream)
        block = [f'# https://github.com/reshaped/gdi {commit}']
        block += [f'{hashlib.sha256(f"new {path}".encode()).hexdigest()}  {path}' for path in vendor.files('gdi')]
        gdi = head.index('# https://github.com/reshaped/gdi')
        self.assertEqual(Path('tools/SHA256SUMS').read_bytes().decode(), head[:gdi] + '\n'.join(block) + '\n' + tail)
        vendor.check()

    def test_refresh_refuses_a_commit_id_no_pin_can_hold(self):
        before = {path: Path(path).read_bytes() for path in vendor.files('gdi')}
        self.write('bin/git', f'#!/bin/sh\necho {"ab" * 32}\n')
        Path('bin/git').chmod(0o755)
        with mock.patch.dict(os.environ, {'PATH': f'{Path("bin").resolve()}{os.pathsep}{os.environ["PATH"]}'}):
            message = self.fails(vendor.refresh, 'gdi', 'v1', Path('.'))
        self.assertIn(f'{"ab" * 32}: not a SHA-1 commit ID', message)
        self.assertEqual(Path('tools/SHA256SUMS').read_text(), self.sums)
        self.assertEqual({path: Path(path).read_bytes() for path in before}, before)

    def test_stalled_git_leaves_the_pinned_inputs_in_place(self):
        before = {path: Path(path).read_bytes() for path in vendor.files('gdi')}
        self.write('bin/git', '#!/bin/sh\necho "waiting for remote" >&2\nexec sleep 30\n')
        Path('bin/git').chmod(0o755)
        env = {'PATH': f'{Path("bin").resolve()}{os.pathsep}{os.environ["PATH"]}', 'HUNG': '0.5', 'GRACE': '0.2'}
        with mock.patch.dict(os.environ, env):
            message = self.fails(vendor.refresh, 'gdi', COMMIT, None)
        self.assertIn('git -C . clone', message)
        self.assertIn('timed out after 0.5 seconds', message)
        self.assertIn('waiting for remote', message)
        self.assertEqual(Path('tools/SHA256SUMS').read_text(), self.sums)
        self.assertEqual({path: Path(path).read_bytes() for path in before}, before)


class Sh4zam(unittest.TestCase):
    def test_cross_tests_use_the_pinned_headers(self):
        include = SH4ZAM / 'include'
        if not (SH_ELF / 'bin/sh-elf-gcc').is_file() or not (include / 'sh4zam').is_dir():
            self.skipTest('no SH-4 GCC and SH4ZAM headers')
        prefix = 'tools/sh4zam/include/'
        pinned = {}
        for line in (ROOT / 'tools/SHA256SUMS').read_text().splitlines():
            checksum, _, name = line.partition('  ')
            if name.startswith(prefix):
                pinned[name[len(prefix):]] = checksum
        self.assertTrue(pinned)
        for name, checksum in pinned.items():
            with self.subTest(header=name):
                path = include / name
                self.assertTrue(path.is_file(), str(path))
                self.assertEqual(hashlib.sha256(path.read_bytes()).hexdigest(), checksum, str(path))
