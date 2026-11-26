"""keep_stamps: each listed stamp takes its proposal's bytes, or keeps its own where they match."""

from pathlib import Path

import keep_stamps
from support import TreeTest

BATCH = Path('build/.stamps.a')


class KeepStamps(TreeTest):
    def stamp(self, path, text, proposed):
        self.write(path, text)
        self.write(path + BATCH.name, proposed)

    def test_replaces_a_changed_stamp_and_drops_the_proposal_of_an_equal_one(self):
        self.stamp('build/a.cmd', 'old\n', 'new\n')
        self.stamp('build/b.cmd', 'same\n', 'same\n')
        self.write(BATCH, 'build/a.cmd\nbuild/b.cmd\n')
        keep_stamps.finish(BATCH)
        self.assertEqual(Path('build/a.cmd').read_text(), 'new\n')
        self.assertEqual(Path('build/b.cmd').read_text(), 'same\n')
        self.assertEqual(sorted(str(path) for path in Path('build').iterdir()), ['build/a.cmd', 'build/b.cmd'])

    def test_ends_a_path_at_lf_alone(self):
        for ending in ('\n', ''):
            with self.subTest(ending=ending):
                self.stamp('build/a', 'old\n', 'new\n')
                self.stamp('build/a b', 'old\n', 'new\n')
                self.write(BATCH, f'build/a b{ending}')
                keep_stamps.finish(BATCH)
                self.assertEqual(Path('build/a b').read_text(), 'new\n')
                self.assertEqual(Path('build/a').read_text(), 'old\n')
                self.assertTrue(Path('build/a' + BATCH.name).exists())

    def test_finishes_a_stamp_listed_in_two_spellings_once(self):
        self.stamp('build/a.cmd', 'old\n', 'new\n')
        self.write(BATCH, 'build/a.cmd\n./build/a.cmd\n')
        keep_stamps.finish(BATCH)
        self.assertEqual(Path('build/a.cmd').read_text(), 'new\n')
        self.assertFalse(BATCH.exists())

    def test_names_a_missing_proposal_and_keeps_the_batch(self):
        self.write('build/a.cmd', 'old\n')
        self.write(BATCH, 'build/a.cmd\n')
        self.assertIn('build/a.cmd.stamps.a: No such file or directory', self.fails(keep_stamps.finish, BATCH))
        self.assertTrue(BATCH.exists())
