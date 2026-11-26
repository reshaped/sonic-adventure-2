"""The modem's driver, as the host runs its C: the harnesses of tests/modem/."""

import subprocess
import sys
import tempfile
import unittest
from pathlib import Path

from support import ROOT, compile_host


MODEM = ROOT / 'sdk/modem'


class ModemDirectoryTest(unittest.TestCase):
    """Run the portable AT directory commands against the modem's RAM-backed EEPROM."""

    @classmethod
    def setUpClass(cls):
        scratch = tempfile.TemporaryDirectory()
        cls.addClassCleanup(scratch.cleanup)
        work = Path(scratch.name)
        cls.program = work / 'directory'
        subprocess.run([sys.executable, ROOT / 'tools/body_data.py', 'chaostgkinder',
                        'sdk/modem/atcom.c', work], cwd=ROOT, check=True)
        # An empty register profile and a four-byte signature suffice for this
        # RAM-backed directory. Both settings lists use the driver's end marker.
        bodies = work / 'sdk/modem/ee_drver'
        bodies.mkdir(parents=True)
        for name, values in {
            'eeprom_profile_registers': 'EE_REGISTER_END',
            'eeprom_initial_header': '0, 0, 0, 0, EE_REGISTER_END',
            'eeprom_dial_label': '0',
            'eeprom_logon_label': '0',
        }.items():
            (bodies / (name + '.inc')).write_text(values + '\n')
        # The link drops the unrelated AT handlers that call the Dreamcast modem hardware.
        sources = [MODEM / name for name in ('atcom.c', 'country.c', 'ecdc_def.c', 'modemdef.c', 'v42def.c')]
        sources.extend(sorted(MODEM.glob('ee_*.c')))
        compile_host(cls.program, '-std=c89', '-O1', '-I', MODEM, '-Itests/modem/include', '-I', work / 'sdk',
                     'tests/modem/directory.c', *sources)

    def check_case(self, case):
        ran = subprocess.run([self.program, case], stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True)
        self.assertEqual(ran.returncode, 0, ran.stdout)

    def test_exact_fit_and_short_destination(self):
        self.check_case('append')

    def test_numbered_and_resumed_dial(self):
        self.check_case('dial')

    def test_store_and_prepare_leave_digits_in_the_shared_dial_buffer(self):
        self.check_case('scratch')

    def test_full_directory_preserves_saved_numbers(self):
        self.check_case('store')


class ModemFields(unittest.TestCase):
    """Check that the portable AT field readers return AT_FIELD_MISSING for a null cursor or output."""

    def test_null_cursor_or_output_is_missing(self):
        with tempfile.TemporaryDirectory() as directory:
            program = Path(directory) / 'fields'
            compile_host(program, '-std=c89', '-O2', '-Isdk/modem', 'tests/modem/fields.c', 'sdk/modem/atcom_ex.c')
            subprocess.run([program], check=True)


class PumpLevels(unittest.TestCase):
    """Check the pump's transmit gain bound and a K56flex retrain's configuration with the portable modem code."""

    def test_gain_bound_and_kept_configuration(self):
        with tempfile.TemporaryDirectory() as directory:
            program = Path(directory) / 'pump-levels'
            compile_host(program, '-std=c89', '-O2', '-Isdk/modem', 'tests/modem/pump_levels.c')
            subprocess.run([program], check=True)


class ModemRegisters(unittest.TestCase):
    """Check the portable C view of the Rockwell modem's register bits."""

    def test_register_bytes_and_status_flags(self):
        with tempfile.TemporaryDirectory() as directory:
            program = Path(directory) / 'registers'
            compile_host(program, '-std=c89', '-Wall', '-Wextra', '-Werror', '-Isdk/modem', 'tests/modem/registers.c')
            subprocess.run([program], check=True)


class ModemTerminal(unittest.TestCase):
    """Pass a complete escape command from the terminal recognizer to the AT buffer."""

    def test_completed_carrier_detect_command(self):
        sources = [MODEM / name for name in ('atcom.c', 'ecdc_def.c', 'modemdef.c')]
        sources.extend(sorted(MODEM.glob('tci_*.c')))
        with tempfile.TemporaryDirectory() as directory:
            work = Path(directory)
            program = work / 'terminal'
            subprocess.run([sys.executable, ROOT / 'tools/body_data.py', 'chaostgkinder',
                            'sdk/modem/atcom.c', work], cwd=ROOT, check=True)
            # This fixture accepts &C1. Other prefixes and the line-rate lookup
            # are unused in the carrier-detect command.
            tables = work / 'sdk/modem/tci_modm'
            tables.mkdir(parents=True)
            (tables / 'tci_line_rates.inc').write_text('0\n')
            for prefix in ('base', 'amp', 'percent', 'slash', 'star', 'at'):
                entries = '{0, 0}, {0, 0}, {1, 1}' if prefix == 'amp' else '{0, 0}'
                (tables / f'ties_{prefix}.inc').write_text(entries + '\n')
            compile_host(program, '-std=c89', '-O1', '-I', MODEM, '-Itests/modem/include', '-I', work / 'sdk',
                         'tests/modem/terminal.c', *sources)
            subprocess.run([program], check=True)


class ModemXidLengths(unittest.TestCase):
    """Check that the portable V.42 code takes no XID parameter past its frame or past hdlc."""

    def test_long_groups_and_options_are_dropped(self):
        with tempfile.TemporaryDirectory() as directory:
            program = Path(directory) / 'xid-lengths'
            compile_host(program, '-std=c89', '-O2', '-Isdk/modem', 'tests/modem/xid_lengths.c',
                         'sdk/modem/v42def.c', 'sdk/modem/ecdc_def.c')
            subprocess.run([program], check=True)


if __name__ == '__main__':
    unittest.main()
