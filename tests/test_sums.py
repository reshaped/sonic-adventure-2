"""SHA256SUMS's list: exactly the files make expects, in path order."""

import hashlib
import unittest
from pathlib import Path

import sums
from config import Image, images, stub_sources
from support import TreeTest, run

HASH = 'ab' * 32


class Sums(TreeTest):
    def test_reads_the_paths_in_order(self):
        self.write('SHA256SUMS', f'{HASH}  build/link/a.bin\n{HASH}  orig/A.BIN\n')
        self.assertEqual(sums.listed(), ['build/link/a.bin', 'orig/A.BIN'])

    def test_names_the_line_that_is_no_hash_and_path(self):
        self.write('SHA256SUMS', f'{HASH}  orig/A.BIN\n{HASH} orig/B.BIN\n')
        self.assertIn('SHA256SUMS:2: not a line', self.fails(sums.listed))

    def test_ends_a_line_at_lf_alone(self):
        self.write('SHA256SUMS', f'{HASH}  orig/A.BIN\v{HASH}  orig/B.BIN\n')
        self.assertIn('SHA256SUMS:1: not a line', self.fails(sums.listed))
        self.assertIn('SHA256SUMS:1: not a line', self.fails(sums.added, ['build/link/a.bin']))
        self.write('SHA256SUMS', f'{HASH}  orig/A.BIN\n{HASH}  orig/B.BIN\n')
        self.assertEqual(sums.listed(), ['orig/A.BIN', 'orig/B.BIN'])

    def test_names_the_line_that_is_not_utf_8(self):
        self.write('SHA256SUMS', f'{HASH}  orig/A.BIN\n'.encode() + HASH.encode() + b'  orig/\xff\n')
        self.assertIn('SHA256SUMS:2: not UTF-8', self.fails(sums.listed))
        self.assertIn('SHA256SUMS:2: not UTF-8', self.fails(sums.added, ['build/link/a.bin']))
        self.write('SHA256SUMS', f'{HASH}  orig/\ufffd\n')
        self.assertEqual(sums.listed(), ['orig/\ufffd'])

    def test_names_an_expected_list_that_is_not_utf_8(self):
        self.write('SHA256SUMS', '')
        self.write('build/expected.txt', b'orig/\xff\n')
        status, output = run(sums.main, 'build/expected.txt')
        self.assertEqual(status, 1)
        self.assertIn('build/expected.txt: not UTF-8', output)

    def test_passes_the_expected_list(self):
        self.assertEqual(sums.problems({'a', 'b'}, ['a', 'b']), [])

    def test_names_each_fault(self):
        self.assertEqual(sums.problems({'a', 'b', 'c'}, ['b', 'a', 'a', 'd']), [
            'SHA256SUMS: a comes after b, out of path order', 'SHA256SUMS: a has 2 lines',
            'SHA256SUMS: no line for c', 'SHA256SUMS: a line for d, which make no longer builds or reads'])

    def test_orders_paths_by_their_bytes_as_the_c_locale_does(self):
        self.assertEqual(sums.problems({'orig/B', 'orig/a'}, ['orig/B', 'orig/a']), [])


class Add(TreeTest):
    def test_gives_each_new_link_its_line_in_path_order(self):
        self.write('SHA256SUMS', f'{HASH}  build/link/1st_read/src/b.bin\n{HASH}  orig/1ST_READ.BIN\n')
        self.write('build/link/1st_read/src/a.bin', b'a')
        self.write('build/link/stg13/modules/c.bin', b'c')
        self.assertEqual(run(sums.main, '--add', 'build/link/stg13/modules/c.bin', 'build/link/1st_read/src/a.bin'),
                         (0, ''))
        self.assertEqual(sums.listed(), ['build/link/1st_read/src/a.bin', 'build/link/1st_read/src/b.bin',
                                         'build/link/stg13/modules/c.bin', 'orig/1ST_READ.BIN'])
        self.assertIn(f'{hashlib.sha256(b"a").hexdigest()}  build/link/1st_read/src/a.bin\n',
                      Path('SHA256SUMS').read_text())

    def test_never_changes_a_line_it_lists(self):
        self.write('SHA256SUMS', f'{HASH}  build/link/1st_read/src/b.bin\n')
        self.write('build/link/1st_read/src/b.bin', b'b')
        self.assertIn('build/link/1st_read/src/b.bin: SHA256SUMS lists it already',
                      self.fails(sums.added, ['build/link/1st_read/src/b.bin']))
        self.assertIn('orig/1ST_READ.BIN: not a path under build/link/',
                      self.fails(sums.added, ['orig/1ST_READ.BIN']))

    def test_refuses_a_file_that_lists_a_path_twice(self):
        # a text merge can leave two lines for a path, and writing the file would keep only one of their hashes
        doubled = f'{HASH}  build/link/1st_read/src/a.bin\n{"cd" * 32}  build/link/1st_read/src/a.bin\n'
        self.write('SHA256SUMS', doubled)
        self.write('build/link/1st_read/src/b.bin', b'b')
        status, output = run(sums.main, '--add', 'build/link/1st_read/src/b.bin')
        self.assertEqual(status, 1)
        self.assertIn('SHA256SUMS: build/link/1st_read/src/a.bin has 2 lines', output)
        self.assertEqual(Path('SHA256SUMS').read_text(), doubled)

    def test_takes_each_link_by_its_normal_path(self):
        self.write('SHA256SUMS', f'{HASH}  build/link/1st_read/src/b.bin\n')
        self.write('build/link/1st_read/src/a.bin', b'a')
        self.assertIn('SHA256SUMS: not a path under build/link/',
                      self.fails(sums.added, ['build/link/../../SHA256SUMS']))
        self.assertIn('build/link/1st_read/src/b.bin: SHA256SUMS lists it already',
                      self.fails(sums.added, ['build/link/1st_read/./src/b.bin']))
        self.assertIn(f'{hashlib.sha256(b"a").hexdigest()}  build/link/1st_read/src/a.bin\n',
                      sums.added(['build/link/stg13/../1st_read/src/a.bin']))

    def test_refuses_a_link_with_a_blank(self):
        self.write('SHA256SUMS', f'{HASH}  build/link/1st_read/src/b.bin\n')
        self.write('build/link/a b', b'a')
        status, output = run(sums.main, '--add', 'build/link/a b')
        self.assertEqual(status, 1)
        self.assertIn('build/link/a b: has a blank', output)
        self.assertEqual(Path('SHA256SUMS').read_text(), f'{HASH}  build/link/1st_read/src/b.bin\n')

    def test_refuses_a_link_whose_listed_path_is_not_normal(self):
        self.write('SHA256SUMS', f'{HASH}  build/link/./a.bin\n')
        self.write('build/link/a.bin', b'a')
        for link in 'build/link/a.bin', 'build/link/./a.bin':
            self.assertIn('build/link/a.bin: SHA256SUMS lists it already', self.fails(sums.added, [link]))

    def test_takes_expected_or_links_and_not_both(self):
        self.assertEqual(run(sums.main, 'build/expected.txt', '--add', 'build/link/a.bin')[0], 2)
        self.assertEqual(run(sums.main, '', '--add', 'build/link/a.bin')[0], 2)
        self.assertEqual(run(sums.main)[0], 2)


class PublishedSums(unittest.TestCase):
    def test_each_placing_image_has_its_disc_free_members(self):
        sources = stub_sources()
        expected = set()
        for name in images():
            for source in sources.intersection(Image(name).sources()):
                expected.add(f'build/stub/link/{name}/{Path(source).with_suffix(".bin")}')
        listed = {path for path in sums.listed() if path.startswith('build/stub/link/')}
        self.assertSetEqual(expected, listed)
