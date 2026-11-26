"""HUNG and GRACE read zero as timeout(1) does: as no limit."""
import os
import subprocess
import unittest
from unittest import mock

import tool_process


class Zero(unittest.TestCase):
    def test_reads_as_no_limit(self):
        for given in ('0', '0s', '0m'):
            with self.subTest(given=given), mock.patch.dict(os.environ, {'HUNG': given}):
                self.assertIsNone(tool_process.seconds('HUNG', 600))

    def test_hung_zero_lets_a_tool_run_to_its_end(self):
        with mock.patch.dict(os.environ, {'HUNG': '0'}):
            self.assertEqual(tool_process.run(['sleep', '0.2']).returncode, 0)

    def test_grace_zero_waits_for_the_leader_to_take_term(self):
        tool = subprocess.Popen(['sh', '-c', 'trap "sleep 0.3; exit 3" TERM; echo; while :; do sleep 0.05; done'],
                                stdout=subprocess.PIPE, start_new_session=True)
        with tool, mock.patch.dict(os.environ, {'GRACE': '0'}):
            tool.stdout.readline()
            tool_process.stop(tool, tool_process.seconds('GRACE', 10))
        self.assertEqual(tool.returncode, 3)
