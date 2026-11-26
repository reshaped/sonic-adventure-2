"""The link's own rules, apart from the tools it runs: S-records, the rows an
object's sections must fit, the names it binds, and the linked source."""

import contextlib
import io
import os
from pathlib import Path
from unittest import mock

import config
import link
import sysrof
from config import content
from support import TreeTest, run


def srecord(kind, address, data=b''):
    width = link.ADDRESS_WIDTH[kind]
    body = bytes([width + len(data) + 1]) + address.to_bytes(width, 'big') + data
    return f'S{kind}{body.hex().upper()}{0xFF - sum(body) & 0xFF:02X}'


class Records(TreeTest):
    def test_reads_the_bytes_by_address(self):
        self.write('a.mot', '\n'.join([srecord(0, 0, b'a'), srecord(3, 0x8c010000, b'\x01\x02'),
                                       srecord(3, 0x8c010010, b'\x03'), srecord(7, 0x8c010000)]) + '\n')
        self.assertEqual(link.srecords('a.mot'), {0x8c010000: 1, 0x8c010001: 2, 0x8c010010: 3})

    def test_refuses_what_cnvs_would_not_write(self):
        end = srecord(7, 0)
        self.write('sum.mot', srecord(3, 0x8c010000, b'\x01')[:-2] + '00\n' + end + '\n')
        self.assertIn('sum.mot:1: its count or checksum is wrong', self.fails(link.srecords, 'sum.mot'))
        self.write('twice.mot', '\n'.join([srecord(3, 4, b'\x01'), srecord(3, 4, b'\x01'), end]) + '\n')
        self.assertIn('twice.mot:2: writes 0x00000004 again', self.fails(link.srecords, 'twice.mot'))
        self.write('open.mot', srecord(3, 4, b'\x01') + '\n')
        self.assertIn('no end record', self.fails(link.srecords, 'open.mot'))
        self.write('after.mot', end + '\n' + srecord(3, 4, b'\x01') + '\n')
        self.assertIn('after.mot:2: a record after the end', self.fails(link.srecords, 'after.mot'))

    def test_an_end_record_holds_only_its_address(self):
        self.write('end.mot', 'S9030000FC\n')
        self.assertEqual(link.srecords('end.mot'), {})
        self.write('full.mot', 'S9040000AA51\n')
        self.assertIn('full.mot:1: S9 holds data after its address', self.fails(link.srecords, 'full.mot'))


class Rows(TreeTest):
    DECLARED = {'P': sysrof.Section(0x40, 32, True), 'B': sysrof.Section(8, 4, False), 'C': sysrof.Section(0, 4, False)}

    def test_each_section_starts_at_its_first_row(self):
        ranges = [('P', 0x8c010000, 0x8c010020), ('P', 0x8c010020, 0x8c010040), ('B', 0x8c020000, 0x8c020008)]
        self.assertEqual(link.starts('a.c', ranges, self.DECLARED), {'P': 0x8c010000, 'B': 0x8c020000})

    def test_the_rows_must_fit_the_object(self):
        placed = [('P', 0x8c010000, 0x8c010040), ('B', 0x8c020000, 0x8c020008)]
        self.assertIn("its object's B holds 0x8 bytes, and no row places it",
                      self.fails(link.starts, 'a.c', placed[:1], self.DECLARED))
        self.assertIn('its D row names no section of its object',
                      self.fails(link.starts, 'a.c', [*placed, ('D', 0x8c030000, 0x8c030004)], self.DECLARED))
        short = [('P', 0x8c010000, 0x8c010020), placed[1]]
        self.assertIn("its P rows end at 0x8c010020, and its object's P at 0x8c010040",
                      self.fails(link.starts, 'a.c', short, self.DECLARED))


class Bounds(TreeTest):
    # an object like strt1.src's: rows for its tables, the first of C's, and
    # empty sections it takes the bounds of
    DECLARED = {'C': sysrof.Section(8, 4, False), 'D': sysrof.Section(0, 4, False),
                'D32': sysrof.Section(0, 32, False), 'B': sysrof.Section(0, 4, False), 'P': sysrof.Section(0, 4, True)}
    BOUNDS = [('C', 0x8c010000, 0x8c010100), ('D', 0x8c010100, 0x8c010150), ('D32', 0x8c010160, 0x8c010160),
              ('B', 0x8c010160, 0x8c010200)]

    def test_starts_an_empty_section_at_its_bound(self):
        placed = link.starts('a.src', [('C', 0x8c010000, 0x8c010008)], self.DECLARED, self.BOUNDS)
        # P has no bound, so lnk places it where it will. D32, empty, goes ahead of B, which reserves bytes there
        self.assertEqual(list(placed.items()), [('C', 0x8c010000), ('D', 0x8c010100), ('D32', 0x8c010160),
                                                ('B', 0x8c010160)])
        # the bounds' order rules at one address, whatever the object's table says
        swapped = dict(reversed(self.DECLARED.items()))
        self.assertEqual(list(link.starts('a.src', [('C', 0x8c010000, 0x8c010008)], swapped, self.BOUNDS)),
                         ['C', 'D', 'D32', 'B'])

    def test_reserves_the_rest_of_each_bound_past_the_objects_bytes(self):
        placed = link.starts('a.src', [('C', 0x8c010000, 0x8c010008)], self.DECLARED, self.BOUNDS)
        self.assertEqual(link.reserves('a.src', placed, self.DECLARED, self.BOUNDS),
                         [('C', 0xf8, 4, False), ('D', 0x50, 4, False), ('B', 0xa0, 4, False)])
        # a section that runs past its bound's end fails
        self.assertIn("its object's C runs to 0x8c010108, past its bound's end at 0x8c010100",
                      self.fails(link.reserves, 'a.src', {'C': 0x8c010100}, self.DECLARED,
                                 [('C', 0x8c010100, 0x8c010100)]))

    def test_fails_a_bound_no_reserve_can_give(self):
        # inside its bound, STARTOF would give the section's own start, even where it fits
        for start in (0x8c010004, 0x8c0100fc):
            self.assertIn(f"its object's C starts at {start:#010x}, and its bound at 0x8c010000",
                          self.fails(link.reserves, 'a.src', {'C': start}, self.DECLARED, self.BOUNDS[:1]))
        # a byte on 4 takes no reserve to end its bound a byte on, and none can end it two bytes on
        byte = {'E': sysrof.Section(1, 4, False)}
        self.assertEqual(link.reserves('a.src', {'E': 0x8c010000}, byte, [('E', 0x8c010000, 0x8c010001)]), [])
        self.assertIn("its object's E ends at 0x8c010001, and its bound at 0x8c010002, short of the 4-byte alignment",
                      self.fails(link.reserves, 'a.src', {'E': 0x8c010000}, byte, [('E', 0x8c010000, 0x8c010002)]))


    def test_places_each_group_of_one_address_in_address_order(self):
        # the sections go to lnk by address, a start command for each, which names those at one address in
        # placed's order, after the options
        given = {}
        with mock.patch.object(link, 'run', lambda *argv, stdin=None: given.setdefault('lnk', stdin)), \
                mock.patch.object(link, 'srecords', lambda path: {}):
            link.place(['a.obj'], {'B': 0x8c010160, 'C': 0x8c010000, 'D32': 0x8c010160, 'D': 0x8c010100}, Path('a.bin'),
                       ['align_section'])
        self.assertEqual(given['lnk'].splitlines()[-5:], ['align_section', 'start C(8c010000)', 'start D(8c010100)',
                                                            'start B,D32(8c010160)', 'exit'])

    def test_fails_a_command_longer_than_lnk_reads(self):
        # 64 names of eight characters at one address, which lnk would read only the first 510 characters of
        placed = {f'D{number:07d}': 0x8c010000 for number in range(64)}
        with mock.patch.object(link, 'run', lambda *argv, stdin=None: self.fail('lnk ran')):
            failed = self.fails(link.place, ['a.obj'], placed, Path('a.bin'))
        self.assertIn("a.bin: lnk reads 510 characters of a line, and this command's 591: start D0000000,", failed)

    def test_takes_bounds_only_for_the_sections_an_object_asks(self):
        self.write('src/a.src', '')
        self.image('1st_read', 'file 1ST_READ.BIN\nC 0x8c010000 0x8c010100\nD 0x8c010100 0x8c010150\n'
                               'src/a.src C 0x8c010000 0x8c010008\n', '0x8c010000 a\n')
        self.write('config/1st_read/bounds.txt', 'C 0x8c010000 0x8c010008\nD 0x8c010100 0x8c010150\n'
                                                  'D32 0x8c010160 0x8c010160\n')
        image = config.Image('1st_read')
        self.assertEqual(link.taken('src/a.src', image, ['D']), [('D', 0x8c010100, 0x8c010150)])
        self.assertEqual(link.taken('src/a.src', image, []), [])
        self.assertIn('it takes the start or the size of R, and config/1st_read/bounds.txt gives no bound of it',
                      self.fails(link.taken, 'src/a.src', image, ['D', 'R']))
        # its rows record a bound bounds.txt no longer gives, which only the link fails
        self.assertEqual(link.rows_given(image, 'src/a.src', set(), ['R', 'D']),
                         'C 0x8c010000 0x8c010008\nbound D 0x8c010100 0x8c010150\nbound R - -\n')
        # and its rows name the bounds it took, so that an edit of another bound relinks nothing
        self.assertEqual(link.rows_given(image, 'src/a.src', {'a'}), 'C 0x8c010000 0x8c010008\na 0x8c010000\n')
        self.assertEqual(link.rows_given(image, 'src/a.src', {'a'}, ['D32']),
                         'C 0x8c010000 0x8c010008\na 0x8c010000\nbound D32 0x8c010160 0x8c010160\n')

class Names(TreeTest):
    def setUp(self):
        super().setUp()
        self.write('src/a.c', '')
        self.image('1st_read', 'file 1ST_READ.BIN\nP 0x8c010000 0x8c020000\nsrc/a.c P 0x8c010000 0x8c010040\n',
                   '0x8c010020 inside\n0x8c011000 outside\n0x8c012000 other\n')
        self.image = config.Image('1st_read')
        self.ranges = self.image.ranges('src/a.c')

    def test_binds_what_the_object_takes_from_outside(self):
        self.assertEqual(link.bound('src/a.c', self.image, {'_outside'}, self.ranges), [(0x8c011000, 'outside')])

    def test_refuses_a_name_no_table_gives(self):
        self.assertIn('it takes missing from outside, and no symbols.txt of 1st_read gives it',
                      self.fails(link.bound, 'src/a.c', self.image, {'_missing', '_outside'}, self.ranges))

    def test_refuses_a_name_inside_the_sources_own_rows(self):
        self.assertIn('the tables put it at 0x8c010020, inside its own rows',
                      self.fails(link.bound, 'src/a.c', self.image, {'_inside'}, self.ranges))

    def test_takes_a_second_name_at_the_address_of_a_function_the_object_exports(self):
        self.write('config/1st_read/symbols.txt',
                   '0x8c010020 inside\n0x8c010020 inside_outside\n0x8c010030 table_outside\n')
        image = config.Image('1st_read')
        placed = {'P': 0x8c010000, 'D': 0x8c010030}
        exports = link.code_exports({'_inside': ('P', 0x20), '_table': ('D', 0)}, placed)
        self.assertEqual(exports, {0x8c010020})
        self.assertEqual(link.bound('src/a.c', image, {'_inside_outside'}, self.ranges, exports),
                         [(0x8c010020, 'inside_outside')])
        self.assertIn('the tables put it at 0x8c010030, inside its own rows',
                      self.fails(link.bound, 'src/a.c', image, {'_table_outside'}, self.ranges, exports))

    def test_each_name_the_object_defines_has_its_row(self):
        placed = {'P': 0x8c010000}
        link.defined('src/a.c', self.image, {'_inside': ('P', 0x20)}, placed)
        self.assertIn('it defines inside at 0x8c010024, and the tables put it at 0x8c010020',
                      self.fails(link.defined, 'src/a.c', self.image, {'_inside': ('P', 0x24)}, placed))
        self.assertIn('it defines unnamed at 0x8c010008, and no symbols.txt of 1st_read gives it',
                      self.fails(link.defined, 'src/a.c', self.image, {'_unnamed': ('P', 8)}, placed))

    def test_fails_an_export_in_a_section_nothing_places(self):
        # E is empty, and has neither a row nor a bound
        self.assertIn('it defines end in its E, which holds no byte, and no row or bound places it',
                      self.fails(link.defined, 'src/a.c', self.image, {'_inside': ('P', 0x20), '_end': ('E', 0)},
                                 {'P': 0x8c010000}))


class Joined(TreeTest):
    def setUp(self):
        super().setUp()
        self.write('src/a.c', '')
        self.image('1st_read', 'file 1ST_READ.BIN\nP 0x8c010000 0x8c010100\nD 0x8c010100 0x8c010200\n'
                   'B 0x8c010200 0x8c010300\nsrc/a.c P 0x8c010000 0x8c010004\nsrc/a.c D 0x8c010100 0x8c010102\n'
                   'src/a.c B 0x8c010200 0x8c010208\n')
        self.image = config.Image('1st_read')

    def test_joins_the_rows_in_table_order_with_zeros_where_nothing_was_written(self):
        written = {0x8c010000: 1, 0x8c010003: 4, 0x8c010101: 9}
        self.assertEqual(link.joined('src/a.c', self.image, written), bytes([1, 0, 0, 4, 0, 9]))

    def test_refuses_a_byte_outside_the_rows(self):
        self.assertIn('1 bytes at 0x8c010004 fall outside its rows',
                      self.fails(link.joined, 'src/a.c', self.image, {0x8c010004: 1}))
        self.assertIn('1 bytes at 0x8c010200 fall in its bss',
                      self.fails(link.joined, 'src/a.c', self.image, {0x8c010200: 1}))


class ToolRetry(TreeTest):
    def test_timed_out_link_resends_stdin_and_checks_the_final_result(self):
        for final_status in (0, 5):
            with self.subTest(final_status=final_status):
                Path('calls').unlink(missing_ok=True)
                program = Path('wibo').resolve()
                self.write(program, '#!/bin/sh\ncat >> calls\n'
                           'if [ "$(wc -l < calls)" -eq 1 ]; then echo first-output; exec sleep 10; fi\n'
                           f'echo second-output; exit {final_status}\n')
                program.chmod(0o755)
                messages = io.StringIO()
                with mock.patch.object(link, 'WIBO', str(program)), mock.patch.object(link, 'HUNG', 0.25), \
                        mock.patch.dict(os.environ, {'RETRY_WIBO_TIMEOUT': '1'}), contextlib.redirect_stderr(messages):
                    if final_status:
                        error = self.fails(link.run, 'lnk.exe', stdin='payload\n')
                        self.assertIn('second-output', error)
                        self.assertIn('status 5', error)
                    else:
                        link.run('lnk.exe', stdin='payload\n')
                        self.assertIn('wibo retry succeeded', messages.getvalue())
                self.assertIn('first-output', messages.getvalue())
                self.assertEqual(Path('calls').read_text(), 'payload\npayload\n')

    def test_warning_is_fatal_on_a_tool_exit_and_before_a_timeout(self):
        for ending in ('exit 0', 'exec sleep 10'):
            with self.subTest(ending=ending):
                Path('calls').unlink(missing_ok=True)
                program = Path('wibo').resolve()
                self.write(program, '#!/bin/sh\necho attempt >> calls\necho "** fake warning"\n' + ending + '\n')
                program.chmod(0o755)
                with mock.patch.object(link, 'WIBO', str(program)), mock.patch.object(link, 'HUNG', 0.25), \
                        mock.patch.dict(os.environ, {'RETRY_WIBO_TIMEOUT': '1'}):
                    self.assertIn('fake warning', self.fails(link.run, 'asmsh.exe'))
                self.assertEqual(Path('calls').read_text().splitlines(), ['attempt'])


class Seconds(TreeTest):
    def test_takes_a_timeout_from_the_environment_or_its_default(self):
        with mock.patch.dict(os.environ, {'HUNG': '0.5'}):
            self.assertEqual(link.seconds('HUNG', 600), 0.5)
            self.assertEqual(link.seconds('NO_SUCH_TIMEOUT', 10), 10)

    def test_reads_a_duration_as_timeout_does(self):
        for given, seconds in (('10', 10), ('1.5s', 1.5), ('10m', 600), ('2h', 7200), ('1d', 86400)):
            with self.subTest(given=given), mock.patch.dict(os.environ, {'HUNG': given}):
                self.assertEqual(link.seconds('HUNG', 600), seconds)
        for given in ('soon', '10x', 'm', ''):
            with self.subTest(given=given), mock.patch.dict(os.environ, {'HUNG': given}):
                self.assertIn(f'HUNG={given}: not a duration as timeout(1) reads one',
                              self.fails(link.seconds, 'HUNG', 600))


class Read(TreeTest):
    def setUp(self):
        super().setUp()
        self.write('src/a.c', '')
        self.image('1st_read', 'file 1ST_READ.BIN\nP 0x8c010000 0x8c020000\nsrc/a.c P 0x8c010000 0x8c010040\n',
                   '0x8c010000 a\n0x8c011000 outside\n')
        self.rows = 'build/link/1st_read/src/a.rows'

    def test_writes_the_ranges_and_each_names_address(self):
        image = config.Image('1st_read')
        self.assertEqual(link.rows_given(image, 'src/a.c', {'outside', 'a', 'missing'}),
                         'P 0x8c010000 0x8c010040\na 0x8c010000\nmissing -\noutside 0x8c011000\n')

    def test_rewrites_the_rows_the_tables_now_give_otherwise(self):
        self.write(self.rows, 'P 0x8c010000 0x8c010040\na 0x8c010000\noutside 0x8c011000\n')
        self.write('build/link/1st_read/src/gone.rows', 'P 0x8c010040 0x8c010080\n')
        os.utime(self.rows, ns=(0, 0))
        link.refresh()
        self.assertEqual(os.stat(self.rows).st_mtime_ns, 0)
        self.write('config/1st_read/symbols.txt', '0x8c010000 a\n0x8c012000 outside\n0x8c013000 other\n')
        link.refresh()
        self.assertEqual(content(self.rows), b'P 0x8c010000 0x8c010040\na 0x8c010000\noutside 0x8c012000\n')
        self.assertEqual(content('build/link/1st_read/src/gone.rows'), b'P 0x8c010040 0x8c010080\n')

    def test_refresh_reads_symbols_once_for_all_objects_of_an_image(self):
        self.write('src/b.c', '')
        splits = Path('config/1st_read/splits.txt')
        splits.write_text(splits.read_text() + 'src/b.c P 0x8c010040 0x8c010050\n')
        other = 'build/link/1st_read/src/b.rows'
        for path in (self.rows, other):
            self.write(path, 'outside -\n')
            os.utime(path, ns=(0, 0))
        with mock.patch.object(config.Image, 'symbols', autospec=True, side_effect=config.Image.symbols) as symbols:
            link.refresh()
        self.assertEqual(symbols.call_count, 1)
        for path in (self.rows, other):
            self.assertIn('outside 0x8c011000\n', Path(path).read_text())

    def test_reads_no_table_older_than_each_file_of_rows(self):
        self.write(self.rows, 'P 0x8c010000 0x8c010040\noutside 0x8c010004\n')
        for path in ('config/1st_read/splits.txt', 'config/1st_read/symbols.txt', 'config/1st_read'):
            os.utime(path, ns=(0, 0))
        link.refresh()
        self.assertEqual(content(self.rows), b'P 0x8c010000 0x8c010040\noutside 0x8c010004\n')

    def test_reads_the_tables_where_one_is_deleted(self):
        self.write('config/1st_read/bounds.txt', 'P 0x8c010000 0x8c020000\n')
        self.write(self.rows, 'P 0x8c010000 0x8c010040\na 0x8c010000\nbound P 0x8c010000 0x8c020000\n')
        for path in ('config/1st_read/splits.txt', 'config/1st_read/symbols.txt', 'config/1st_read/bounds.txt',
                     'config/1st_read'):
            os.utime(path, ns=(0, 0))
        os.utime(self.rows, ns=(10**9, 10**9))
        os.remove('config/1st_read/bounds.txt')
        link.refresh()
        self.assertEqual(content(self.rows), b'P 0x8c010000 0x8c010040\na 0x8c010000\nbound P - -\n')

    def test_records_a_bound_the_tables_no_longer_give(self):
        # the object's next link fails on P, or links where its source no longer asks it
        self.write(self.rows, 'P 0x8c010000 0x8c010040\na 0x8c010000\nbound P 0x8c010000 0x8c020000\n')
        os.utime(self.rows, ns=(0, 0))
        link.refresh()
        self.assertEqual(content(self.rows), b'P 0x8c010000 0x8c010040\na 0x8c010000\nbound P - -\n')

    def test_tells_a_name_called_bound_from_a_bound(self):
        self.write('config/1st_read/symbols.txt', '0x8c010000 a\n0x8c011000 bound\n')
        self.write('config/1st_read/bounds.txt', 'P 0x8c010000 0x8c020000\n')
        rows = 'P 0x8c010000 0x8c010040\nbound 0x8c011000\nbound P 0x8c010000 0x8c020000\n'
        self.write(self.rows, rows)
        os.utime(self.rows, ns=(0, 0))
        link.refresh()
        self.assertEqual(content(self.rows), rows.encode())

    def test_rewrites_rows_or_links_an_object_and_never_both(self):
        for argv in (['--rows', 'build/obj/src/a.obj'], ['build/obj/src/a.obj'], []):
            self.assertEqual(run(link.main, *argv)[0], 2)

    def test_links_the_object_it_was_given(self):
        for path in ('build/obj/src/a.tmp', 'build/obj/src/a.obj', 'build/obj/src/a.b.obj'):
            self.write(path, '')
        status, failed = run(link.main, 'build/obj/src/a.tmp', '1st_read')
        self.assertEqual(status, 1)
        self.assertIn('build/obj/src/a.tmp: not an object, with .obj for its suffix', failed)
        status, failed = run(link.main, 'build/obj/src/a.b.obj', '1st_read')
        self.assertEqual(status, 1)
        self.assertIn("build/obj/src/a.b.obj: its stem holds a dot, and asmsh and cnvs allow one in a file's name",
                      failed)
        self.assertFalse(Path('build/link').exists())
