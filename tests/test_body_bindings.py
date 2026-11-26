"""body_bindings.py reads a source's optional binding table, and names the
table or row it can't read."""
import os
from pathlib import Path

import body_bindings
from support import TreeTest


class Bindings(TreeTest):
    source = Path('src/foo.c')
    table = Path('offsets/src/foo.bindings')

    def test_an_absent_table_binds_nothing(self):
        self.assertEqual(body_bindings.read_all(self.source), ({}, {}, {}, {}))
        self.assertEqual(body_bindings.dependencies(self.source), set())

    def test_a_directory_or_a_dangling_link_at_the_table_fails_naming_it(self):
        self.table.mkdir(parents=True)
        for call in body_bindings.read_all, body_bindings.dependencies:
            with self.subTest(call=call.__name__):
                self.assertIn(str(self.table), self.fails(call, self.source))
        self.table.rmdir()
        os.symlink('absent.bindings', self.table)
        for call in body_bindings.read_all, body_bindings.dependencies:
            with self.subTest(call=call.__name__, link=True):
                self.assertIn(str(self.table), self.fails(call, self.source))

    def test_each_malformed_row_names_the_form_to_write(self):
        for row, form in (('selector widget.kind 1U\n', 'unsuffixed decimal or 0x hex'),
                          ('stop TABLE -1\n', 'CONSTANT_NAME, the negative constant by its name'),
                          ('includ tags\n', 'or include NAME')):
            with self.subTest(row=row):
                self.write(self.table, row)
                message = self.fails(body_bindings.read_all, self.source)
                self.assertIn(f'{self.table}:1: expected', message)
                self.assertIn(form, message)
