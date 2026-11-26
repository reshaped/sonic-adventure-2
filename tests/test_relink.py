"""The whole link's own rules, apart from the tools it runs: the order of its
objects, the runs it cuts and names apart and the space it reserves, the
copies of objects it renames, the names it binds, the image it compares
with the splicer's, and the shift test's links, words and reasons."""

import io
import struct
import tracemalloc
from contextlib import redirect_stdout
from pathlib import Path
from unittest import mock

import config
import relink
import sysrof
from config import content
from support import TreeTest, obj, record

# a main executable of three sources. b.c's code starts on 32, past a disc
# row; its CUP section lies between a.c's constants and c.c's; and its bss
# starts past bss the tables give no object
SPLITS = """\
file    1ST_READ.BIN
P       0x8c010000  0x8c010060
C       0x8c010060  0x8c010080
B       0x8c010080  0x8c0100a0
src/a.c    P      0x8c010000  0x8c010010
disc       P      0x8c010010  0x8c010040
src/b.c    P      0x8c010040  0x8c010060
src/a.c    C      0x8c010060  0x8c010066
src/b.c    CUP:C  0x8c010068  0x8c010070
src/c.c    C      0x8c010070  0x8c010078
disc       C      0x8c010078  0x8c010080
src/a.c    B      0x8c010080  0x8c010088
src/b.c    B      0x8c010090  0x8c0100a0
"""
# the same, but b.c's bss comes ahead of a.c's, where their code runs the
# other way
CROSSED = SPLITS.replace('src/a.c    B      0x8c010080  0x8c010088\nsrc/b.c    B      0x8c010090  0x8c0100a0\n',
                         'src/b.c    B      0x8c010080  0x8c010090\nsrc/a.c    B      0x8c010090  0x8c010098\n')
# each object's sections, as (name, size, alignment, code); a.c's D32 is
# empty, as strt1.src's is
SECTIONS = {
    'src/a.c': [('P', 0x10, 4, True), ('C', 6, 4, False), ('B', 8, 4, False), ('D32', 0, 32, False)],
    'src/b.c': [('P', 0x20, 32, True), ('CUP', 8, 4, False), ('B', 0x10, 4, False)],
    'src/c.c': [('C', 8, 4, False)],
}
# the image's stored bytes: each source's, the disc's, and fill; bss lies
# past the file
ORIGINAL = b'a' * 0x10 + b'D' * 0x30 + b'b' * 0x20 + b'c' * 6 + bytes(2) + b'u' * 8 + b'C' * 8 + b'E' * 8
SOURCES = ['src/a.c', 'src/b.c', 'src/c.c']
TAKEN = {'P', 'C', 'B', 'CUP', 'D32'}


def linked():
    """The bytes the whole link of SPLITS writes, by address: ORIGINAL's outside the disc rows."""
    return {0x8c010000 + at: byte for at, byte in enumerate(ORIGINAL)
            if not 0x10 <= at < 0x40 and not 0x78 <= at < 0x80}


def layout(objects=None):
    """The section tables of the sources' objects, SECTIONS's or objects's."""
    return {source: {name: sysrof.Section(size, alignment, holds) for name, size, alignment, holds in sections}
            for source, sections in (objects or SECTIONS).items()}


class Fixture(TreeTest):
    def setUp(self):
        super().setUp()
        for source, sections in SECTIONS.items():
            self.write(source, '')
            self.write(Path('build/obj', source).with_suffix('.obj'), obj(sections))
        self.image('1st_read', SPLITS)


class Order(Fixture):
    def test_a_run_is_the_rows_of_one_output_section_that_share_an_input_section(self):
        image = config.Image('1st_read')
        found = [(output, [(row.writer, image.input_section(row)) for row in rows])
                 for output, rows in relink.runs(image)]
        self.assertEqual(found, [('P', [('src/a.c', 'P'), ('src/b.c', 'P')]), ('C', [('src/a.c', 'C')]),
                                 ('C', [('src/b.c', 'CUP')]), ('C', [('src/c.c', 'C')]),
                                 ('B', [('src/a.c', 'B'), ('src/b.c', 'B')])])
        # and where the next row's object comes earlier in input order
        cut = relink.runs(image, {'src/b.c': 0, 'src/a.c': 1, 'src/c.c': 2})
        self.assertEqual([[row.writer for row in rows] for output, rows in cut if output == 'B'],
                         [['src/a.c'], ['src/b.c']])

    def test_orders_each_runs_objects_by_their_rows_and_others_by_the_table(self):
        self.assertEqual(relink.order(config.Image('1st_read')), SOURCES)
        # c.c's constants ahead of a.c's, which puts c.c first, though the table names it last
        self.image('1st_read', SPLITS.replace('src/a.c    C      0x8c010060  0x8c010066\n', '')
                   .replace('disc       C      0x8c010078', 'src/a.c    C      0x8c010078'))
        self.assertEqual(relink.order(config.Image('1st_read')), ['src/c.c', 'src/a.c', 'src/b.c'])

    def test_takes_the_first_waiting_source_where_the_runs_disagree(self):
        # c.c is ready, and goes first; then a.c and b.c each wait on the other, and a.c comes first in the table
        self.image('1st_read', CROSSED)
        image = config.Image('1st_read')
        self.assertEqual(relink.order(image), ['src/c.c', 'src/a.c', 'src/b.c'])
        # unless a run of a section whose bounds an object asks holds them in order
        self.assertEqual(relink.order(image, {'B'}), ['src/c.c', 'src/b.c', 'src/a.c'])
        self.assertIn("no one order of the objects keeps the rows of each section whose start or size an object asks "
                      'in order: src/b.c goes before src/a.c in B, src/a.c goes before src/b.c in P',
                      self.fails(relink.order, image, {'P', 'B'}))


class Plan(Fixture):
    def test_starts_each_run_where_the_one_before_ends_and_reserves_what_no_object_holds(self):
        starts, renamed, holes, _, counts = relink.plan(config.Image('1st_read'), SOURCES, layout(), TAKEN)
        # c.c's C follows b.c's CUP, so it takes a name of its own, and lnk lays the three runs out in turn
        self.assertEqual(list(starts.items()), [('P', 0x8c010000), ('C', 0x8c010060), ('CUP', 0x8c010060),
                                                ('C2', 0x8c010060), ('B', 0x8c010080)])
        self.assertEqual(renamed, {'src/c.c': {'C': 'C2'}})
        self.assertEqual(counts, (1, 0))
        # the disc row up to b.c's code, where lnk would start it on 32 at 0x8c010020, and the bss between a.c's
        # and b.c's; the disc row after c.c's constants needs nothing
        self.assertEqual(holes, {'src/b.c': [('P', 0x30, 4, True), ('B', 8, 4, False)]})

    def test_cuts_a_run_where_input_order_goes_back(self):
        self.image('1st_read', CROSSED)
        starts, renamed, _, report, counts = relink.plan(config.Image('1st_read'), SOURCES, layout(), TAKEN)
        self.assertEqual(list(starts)[-2:], ['B', 'B2'])
        self.assertEqual(renamed, {'src/c.c': {'C': 'C2'}, 'src/a.c': {'B': 'B2'}})
        self.assertIn("cuts B at src/a.c, whose object comes before src/b.c's", report)
        self.assertEqual(counts, (2, 1))

    def test_fails_a_new_name_for_a_section_whose_bounds_an_object_asks(self):
        # c.c's C, past b.c's CUP, would link as C2, and lnk would give C the size of a.c's alone
        self.assertIn("src/c.c's C would link as section C2, apart from the run of C before it, and an object asks "
                      'the start or size of C',
                      self.fails(relink.plan, config.Image('1st_read'), SOURCES, layout(), TAKEN, {'C'}))

    def test_starts_an_empty_section_at_its_bound_ahead_of_the_runs_there(self):
        # D32's bound lies where C starts, and an empty section no object declares has no start
        self.write('config/1st_read/bounds.txt', 'D32 0x8c010060 0x8c010060\nB 0x8c010080 0x8c0100a0\n'
                                                  'R 0x8c0100a0 0x8c0100a0\n')
        starts, _, _, _, _ = relink.plan(config.Image('1st_read'), SOURCES, layout(), TAKEN)
        self.assertEqual(list(starts.items()), [('P', 0x8c010000), ('D32', 0x8c010060), ('C', 0x8c010060),
                                                ('CUP', 0x8c010060), ('C2', 0x8c010060), ('B', 0x8c010080)])

    def test_fails_a_row_lnk_cant_start_its_object_at(self):
        off = layout({**SECTIONS, 'src/c.c': [('C', 8, 32, False)]})
        self.assertIn("src/c.c's C starts at 0x8c010070, off its object's alignment of 32",
                      self.fails(relink.plan, config.Image('1st_read'), SOURCES, off, TAKEN))
        # a.c's code ends 2 bytes short of b.c's, which lnk would start on 2 where a.c's ends
        self.image('1st_read', SPLITS.replace('src/a.c    P      0x8c010000  0x8c010010\n'
                                              'disc       P      0x8c010010  0x8c010040\n',
                                              'src/a.c    P      0x8c010000  0x8c01003e\n'
                                              'disc       P      0x8c01003e  0x8c010040\n'))
        tight = layout({**SECTIONS, 'src/a.c': [('P', 0x3e, 4, True), *SECTIONS['src/a.c'][1:]],
                        'src/b.c': [('P', 0x20, 2, True), *SECTIONS['src/b.c'][1:]]})
        self.assertIn('src/b.c starts at 0x8c010040, and lnk would start it at 0x8c01003e, with too little space '
                      'between to reserve on 4',
                      self.fails(relink.plan, config.Image('1st_read'), SOURCES, tight, TAKEN))

    def test_a_new_name_takes_the_lowest_number_from_2_that_no_name_has(self):
        self.assertEqual(relink.fresh('C', {'C'}), 'C2')
        self.assertEqual(relink.fresh('PSGWORD', {'PSGWORD', 'PSGWORD2', 'PSGWORD3'}), 'PSGWORD4')


# the module records of an object of module m: its two headers, as SHC
# writes them, each naming m once
HEADS = (record(0x04, b'(261123100839\x00\x01\x000200\x08  \x00\x00\x00\x00\x00\x01m\x02SH'),
         record(0x06, b'@\x00\x01\x00\x00\x00\x00\x01m\x04A_SH261123100839\x00'))


class Renamed(TreeTest):
    def test_names_the_module_and_a_section_anew(self):
        self.write('m.obj', b''.join(HEADS) + obj([('P', 4, 4, True), ('C', 8, 4, False), ('CUP', 4, 4, False)]))
        self.write('m2.obj', relink.renamed_object('m.obj', ('m', 'm2'), {'C': 'C2'}))
        # every record of the copy passes its checksum, and each header and the section table grow by a byte
        self.assertEqual(list(sysrof.sections('m2.obj')), ['P', 'C2', 'CUP'])
        self.assertEqual(sysrof.sections('m2.obj')['C2'], sysrof.Section(8, 4, False))
        self.assertEqual(len(content('m2.obj')), len(content('m.obj')) + 3)
        heads = [record for _, record in sysrof.records('m2.obj')][:2]
        self.assertEqual([record.count(b'\x02m2') for record in heads], [1, 1])

    def test_fails_a_name_the_object_lacks_and_an_entry_that_crosses_records(self):
        self.write('m.obj', b''.join(HEADS) + obj([('P', 4, 4, True)]))
        self.assertIn('no section C to name anew', self.fails(relink.renamed_object, 'm.obj', None, {'C': 'D'}))
        self.assertIn("its 0x04 header doesn't name its module q",
                      self.fails(relink.renamed_object, 'm.obj', ('q', 'r')))
        # P's entry split across two records of one run
        table = [record for _, record in sysrof.records('m.obj')][2][2:-1]
        self.write('cut.obj', b''.join(HEADS) + record(0x08, table[:10], last=False) + record(0x08, table[10:])
                   + record(0x7F, b''))
        self.assertIn('an entry of its section table crosses into the next record',
                      self.fails(relink.renamed_object, 'cut.obj', None, {'P': 'Q'}))


    def test_names_the_module_in_each_headers_module_field_alone(self):
        # the 0x04 header's processor field spells m as the module field does, and stays as it was
        twin = record(0x04, HEADS[0][2:-1].replace(b'\x02SH', b'\x01m'))
        self.write('m.obj', twin + HEADS[1] + obj([('P', 4, 4, True)]))
        self.write('m2.obj', relink.renamed_object('m.obj', ('m', 'm2')))
        self.assertEqual([record for _, record in sysrof.records('m2.obj')][0][2:-1],
                         HEADS[0][2:-1].replace(b'\x01m\x02SH', b'\x02m2\x01m'))
        # two 0x04 headers, and no 0x06, don't name the module in both
        self.write('twice.obj', HEADS[0] + HEADS[0] + obj([('P', 4, 4, True)]))
        self.assertIn('twice.obj: two 0x04 headers name its module',
                      self.fails(relink.renamed_object, 'twice.obj', ('m', 'm2')))
        self.write('half.obj', HEADS[0] + obj([('P', 4, 4, True)]))
        self.assertIn('half.obj: no 0x06 header names its module',
                      self.fails(relink.renamed_object, 'half.obj', ('m', 'm2')))

    def test_counts_a_new_name_in_its_bytes_and_fails_one_no_record_can_hold(self):
        self.write('m.obj', b''.join(HEADS) + obj([('C' * 230, 8, 4, False)]))
        self.write('e.obj', relink.renamed_object('m.obj', ('m', '\xe9')))
        self.assertEqual([record[2 + 28:2 + 30] for _, record in sysrof.records('e.obj')][0], b'\x01\xe9')
        self.assertIn('m.obj: the module name \u0101 has no byte for each character',
                      self.fails(relink.renamed_object, 'm.obj', ('m', '\u0101')))
        self.assertIn(f'm.obj: the module name {"n" * 256} is 256 bytes long, past the 255 a length byte counts',
                      self.fails(relink.renamed_object, 'm.obj', ('m', 'n' * 256)))
        # the section table's record grows from 250 bytes to 261
        self.assertIn('m.obj: a record named anew would be 261 bytes long, past the 255 its size byte counts',
                      self.fails(relink.renamed_object, 'm.obj', None, {'C' * 230: 'C' * 241}))


class Names(Fixture):
    def test_binds_each_name_no_object_defines_where_the_tables_give_it(self):
        self.write('build/obj/src/a.obj', obj(SECTIONS['src/a.c'], references=('_shared', '_outside')))
        self.write('build/obj/src/b.obj', obj(SECTIONS['src/b.c'], references=('_outside',),
                                              exports=(('_shared', 0, 0),)))
        self.write('config/1st_read/symbols.txt', '0x8c010040 shared\n0x8c020000 outside\n')
        objects = {source: Path('build/obj', source).with_suffix('.obj') for source in SECTIONS}
        self.assertEqual(relink.bindings(config.Image('1st_read'), objects), [(0x8c020000, 'outside')])
        self.write('build/obj/src/c.obj', obj(SECTIONS['src/c.c'], references=('_missing', 'strange')))
        failed = self.fails(relink.bindings, config.Image('1st_read'), objects)
        self.assertIn('src/c.c: it takes missing from outside, which no object of 1st_read defines and no '
                      'symbols.txt of 1st_read gives', failed)
        self.assertIn('src/c.c: it takes strange from outside', failed)

    def test_reports_where_a_name_bound_by_address_lies(self):
        image = config.Image('1st_read')
        self.assertEqual(relink.holder(image, 0x8c010044), "in src/b.c's P")
        self.assertEqual(relink.holder(image, 0x8c010020), 'in a disc row of P')
        self.assertEqual(relink.holder(image, 0x8c01008c), 'where no row lies')

    def test_holds_each_name_an_object_exports_to_its_row_in_the_map(self):
        self.write('config/1st_read/symbols.txt', '0x8c010000 first\n0x8c010040 second\n0x8c010060 local\n')
        listed = ('***  LINKAGE EDITOR EXTERNALLY DEFINED SYMBOLS LIST  ***\r\n\r\n'
                  "_first                            H'8C010000               DAT\r\n"
                  '_a_name_longer_than_thirty_two_characters\r\n'
                  "                                  H'8C010080               DAT\r\n"
                  "_second                           H'8C010044               DAT\r\n")
        self.write('link.map', listed)
        image = config.Image('1st_read')
        # local, a static's, has no row in the list, and no object exports it
        failed = self.fails(relink.compare_names, image, Path('link.map'), {'first', 'second'})
        self.assertIn('config/1st_read/symbols.txt: second lies at 0x8c010040, and the whole link of 1st_read '
                      'defines it at 0x8c010044', failed)
        self.assertNotIn('first', failed)
        self.assertIn('local, which the whole link of 1st_read exports, has no address in link.map',
                      self.fails(relink.compare_names, image, Path('link.map'), {'first', 'local'}))
        # a map row of a name no object exports goes unchecked, at whatever address
        self.write('link.map', listed + "_local                            H'8C010064               DAT\r\n")
        relink.compare_names(image, Path('link.map'), {'first'})
        self.write('link.map', 'LINKAGE EDITOR LINK MAP LIST\r\n')
        self.assertIn('link.map: no externally defined symbols list, so it may be cut short',
                      self.fails(relink.compare_names, image, Path('link.map'), set()))


class Joined(Fixture):
    def test_takes_the_disc_rows_from_the_original_and_fails_a_byte_outside_it(self):
        image = config.Image('1st_read')
        self.assertEqual(relink.joined(image, linked(), ORIGINAL), ORIGINAL)
        # a byte the link writes in a disc row gives way to the disc's, as a source's does in the splicer
        self.assertEqual(relink.joined(image, {**linked(), 0x8c010020: 0x99}, ORIGINAL), ORIGINAL)
        self.assertIn('1st_read: the whole link writes at 0x8c010080, outside its 0x80 stored bytes',
                      self.fails(relink.joined, image, {**linked(), 0x8c010080: 0}, ORIGINAL))

    def test_fails_a_disc_row_past_the_original_and_copies_one_that_ends_at_its_end(self):
        image = config.Image('1st_read')
        # the disc row from 0x8c010078 ends at the stored end, and a byte the link writes there gives way to it
        self.assertEqual(relink.joined(image, {**linked(), 0x8c01007f: 0x99}, ORIGINAL), ORIGINAL)
        # an original 4 bytes short holds only half of that disc row
        short = {at: byte for at, byte in linked().items() if at < 0x8c01007c}
        self.assertIn('config/1st_read/splits.txt:11: the disc row from 0x8c010078 to 0x8c010080 lies past the '
                      "original's 0x7c stored bytes",
                      self.fails(relink.joined, image, short, ORIGINAL[:0x7c]))

    def test_fails_a_byte_that_differs_from_the_splice_naming_its_writer(self):
        image = config.Image('1st_read')
        relink.compare(image, ORIGINAL, Path('spliced.bin'), ORIGINAL)
        for at, writer in ((0x44, 'src/b.c'), (0x66, "the linker's fill"), (0x7c, 'the disc')):
            with self.subTest(writer=writer):
                spliced = ORIGINAL[:at] + b'\xff' + ORIGINAL[at + 1:]
                self.assertIn(f'spliced.bin: its byte at {0x8c010000 + at:#010x}, which {writer} writes, is 0xff, '
                              f'and the whole link of 1st_read writes {ORIGINAL[at]:#04x}',
                              self.fails(relink.compare, image, ORIGINAL, Path('spliced.bin'), spliced))
        self.assertIn('spliced.bin: its byte at 0x8c01007f, which the disc writes, is 0x45, and the whole link of '
                      '1st_read ends before it',
                      self.fails(relink.compare, image, ORIGINAL[:-1], Path('spliced.bin'), ORIGINAL))


class Main(Fixture):
    def relink(self, spliced):
        """The inputs relink.main gives lnk for the main executable, against spliced, and what it prints, with lnk's
        link standing in: it writes ORIGINAL's bytes outside the disc rows, and a map that lists no name."""
        self.write('orig/1ST_READ.BIN', ORIGINAL)
        self.write('build/1ST_READ.BIN', spliced)
        inputs = []

        def place(objects, starts, out, options):
            inputs.extend(objects)
            self.write(out.with_name('link.map'), '***  LINKAGE EDITOR EXTERNALLY DEFINED SYMBOLS LIST  ***\r\n')
            return linked()

        printed = io.StringIO()
        with mock.patch.object(relink, 'define', lambda path, names, reserved=(): path.with_suffix('.obj')), \
                mock.patch.object(relink, 'place', place), redirect_stdout(printed):
            relink.main(['1st_read', 'orig/1ST_READ.BIN', 'build/1ST_READ.BIN'])
        return inputs, printed.getvalue()

    def test_writes_the_image_once_it_matches_the_splice(self):
        inputs, printed = self.relink(ORIGINAL)
        self.assertEqual(content('build/relink/1st_read.bin'), ORIGINAL)
        self.assertEqual(printed, '1st_read: new section names for 1 of its runs, 0 of them cut where input order '
                                  'goes back\n')
        # b.c's reserves go just ahead of it, c.c links from a copy whose C is renamed, and the symbols last
        self.assertEqual([str(path) for path in inputs],
                         ['build/obj/src/a.obj', 'build/relink/1st_read/hole1.obj', 'build/obj/src/b.obj',
                          'build/relink/1st_read/modules/c.obj', 'build/relink/1st_read/symbols.obj'])
        self.assertEqual(list(sysrof.sections('build/relink/1st_read/modules/c.obj')), ['C2'])

    def test_a_mismatch_fails_and_leaves_no_image(self):
        spliced = ORIGINAL[:0x50] + b'\x00' + ORIGINAL[0x51:]
        failed = self.fails(self.relink, spliced)
        self.assertIn('build/1ST_READ.BIN: its byte at 0x8c010050, which src/b.c writes, is 0x00, and the whole link '
                      'of 1st_read writes 0x62', failed)
        self.assertFalse(Path('build/relink/1st_read.bin').exists())


def written(data, words):
    """data with each of words, {offset: value}, written over it as a 32-bit little-endian word."""
    data = bytearray(data)
    for at, value in words.items():
        struct.pack_into('<I', data, at, value)
    return bytes(data)


def at_shifts(data, *changed):
    """data as the links at SHIFTS's shifts hold it, where each of changed, (offset, value under the first shift,
    value under the second), gives a word that changes."""
    return [written(data, {at: values[number] for at, *values in changed}) for number in range(len(relink.SHIFTS))]


# the image's words at its base: a.c's code holds a pointer to b.c's, then
# three addresses in the span, one in each of P1, P0 and P2. b.c's code
# holds an address far past the span, one in a region the game doesn't use,
# the span's end, an address in its bss, and the address just past the
# span's end; the first disc row holds an address in the span, which the
# disc gives
WORDS = {0x00: 0x8c010040, 0x04: 0x8c010060, 0x08: 0x0c010064, 0x0c: 0xac010068, 0x10: 0x8c010000,
         0x40: 0x8c300000, 0x44: 0x4c010000, 0x48: 0x8c0100a0, 0x4c: 0x8c01009c, 0x54: 0x8c0100a4}
BASE = written(ORIGINAL, WORDS)
# the word at 0, which each link moves by its shift
MOVED = (0x00, *(0x8c010040 + shift for shift in relink.SHIFTS))


class Shifted(Fixture):
    def test_counts_the_words_that_move_and_lists_those_that_keep_a_value_in_the_span(self):
        moved, fixed = relink.shifted(config.Image('1st_read'), BASE, at_shifts(BASE, MOVED))
        self.assertEqual(moved, 1)
        self.assertEqual(fixed, [(0x8c010004, 0x8c010060, 'src/a.c'), (0x8c010008, 0x0c010064, 'src/a.c'),
                                 (0x8c01000c, 0xac010068, 'src/a.c'), (0x8c010048, 0x8c0100a0, 'src/b.c'),
                                 (0x8c01004c, 0x8c01009c, 'src/b.c')])

    def test_passes_over_a_word_that_a_disc_row_gives_a_byte_of(self):
        # the disc row's word moves by other than the shift, which in the image only the disc could give it
        links = at_shifts(BASE, MOVED, (0x10, 0x12345678, 0x9abcdef0))
        self.assertEqual(relink.shifted(config.Image('1st_read'), BASE, links)[0], 1)

    def test_passes_over_a_word_that_a_disc_row_starts_or_ends_inside(self):
        # inside b.c's code, a disc row from the middle of the word at 0x8c010040 to its end, and one from the
        # start of the word at 0x8c010048 to its middle. Each of those words moves by other than the shift
        code = 'src/b.c    P      0x8c010040  0x8c010060\n'
        self.image('1st_read', SPLITS.replace(code, code + 'disc       P      0x8c010042  0x8c010044\n'
                                                         'disc       P      0x8c010048  0x8c01004a\n'))
        links = at_shifts(BASE, MOVED, (0x40, 0x12345678, 0x9abcdef0), (0x48, 0x12345678, 0x9abcdef0))
        self.assertEqual(relink.shifted(config.Image('1st_read'), BASE, links)[0], 1)

    def test_fails_a_word_that_moves_by_other_than_each_shift(self):
        # b.c's word at 0x8c010040 moves 4 bytes too far under the first shift, and its half of an address at
        # 0x8c010050 moves by the shift's high half alone
        wrong = (0x40, 0x8c300000 + relink.SHIFTS[0] + 4, 0x8c300000 + relink.SHIFTS[1])
        self.assertIn('1st_read: its word at 0x8c010040, which src/b.c writes, holds 0x8c300000, and 0x8d534564 '
                      'and 0x8e768ac0 at the shifts',
                      self.fails(relink.shifted, config.Image('1st_read'), BASE, at_shifts(BASE, MOVED, wrong)))
        half = written(BASE, {0x50: 0x8c01})
        halves = (0x50, *(0x8c01 + (shift >> 16) for shift in relink.SHIFTS))
        failed = self.fails(relink.shifted, config.Image('1st_read'), half, at_shifts(half, MOVED, wrong, halves))
        self.assertIn('1st_read: 2 of its words move, but not by each shift', failed)
        self.assertIn('1st_read: its word at 0x8c010040', failed)

    def test_counts_every_word_that_moves_wrong_and_keeps_only_the_first(self):
        # 64 Ki words, each moving by 1 and 2 but those the two disc rows give a byte of
        image = bytes(0x40000)
        links = [b'\x01\x00\x00\x00' * 0x10000, b'\x02\x00\x00\x00' * 0x10000]
        tracemalloc.start()
        try:
            failed = self.fails(relink.shifted, config.Image('1st_read'), image, links)
            peak = tracemalloc.get_traced_memory()[1]
        finally:
            tracemalloc.stop()
        self.assertIn('1st_read: 65,522 of its words move, but not by each shift', failed)
        self.assertIn('1st_read: its word at 0x8c010000, which src/a.c writes, holds 0x00000000, and 0x00000001 and '
                      '0x00000002 at the shifts', failed)
        # under a word's worth of memory for each word checked, as the image's own copies take
        self.assertLess(peak, 0x40000 * 3)

    def test_moves_the_names_the_images_own_table_gives_in_its_span(self):
        # the stage ends at 0x8c5001f4, and its span at the next boundary of 32, where its workspace starts.
        # alias is the stage's own byte through P0, and past lies past the span. The stage's table repeats
        # main's stage_slot at its base, and gives its own head there
        self.image('stg03', 'file SONIC2/STG03.PRS\nP 0x8c500000 0x8c5001f4\n',
                   '0x0c500010 alias\n0x8c500000 head\n0x8c500000 stage_slot\n0x8c500200 workspace\n'
                   '0x8c500204 past\n')
        self.write('config/1st_read/symbols.txt', '0x8c500000 stage_slot\n')
        self.assertEqual(relink.span(config.Image('stg03')), (0x8c500000, 0x8c500200))
        self.assertEqual(relink.own_names(config.Image('stg03')), {'head', 'alias', 'workspace'})

    def test_each_shift_keeps_every_alignment_of_32_and_moves_both_halves(self):
        for shift in relink.SHIFTS:
            self.assertEqual((shift % 32, bool(shift & 0xffff), bool(shift >> 16)), (0, True, True))


class Reviewed(Fixture):
    FIXED = [(0x8c010004, 0x8c010060, 'src/a.c'), (0x8c01004c, 0x8c01009c, 'src/b.c')]
    ROWS = [(0x8c010000, 0x8c010008, 'two instructions', 'config/kept.txt:1'),
            (0x8c01004c, 0x8c010050, 'a count', 'config/kept.txt:2')]

    def test_gives_each_word_its_rows_reason(self):
        self.assertEqual(relink.reviewed(config.Image('1st_read'), self.FIXED, self.ROWS),
                         [(0x8c010004, 0x8c010060, 'src/a.c', 'two instructions'),
                          (0x8c01004c, 0x8c01009c, 'src/b.c', 'a count')])

    def test_fails_a_word_no_row_holds_and_a_row_that_holds_no_word(self):
        failed = self.fails(relink.reviewed, config.Image('1st_read'), self.FIXED,
                            [self.ROWS[0], (0x8c010050, 0x8c010054, 'a count', 'config/kept.txt:2')])
        self.assertIn("1st_read: its word at 0x8c01004c, which src/b.c writes, keeps 0x8c01009c, in its span, and "
                      'config/kept.txt gives no reason', failed)
        self.assertIn('config/kept.txt:2: no word of 1st_read from 0x8c010050 to 0x8c010054 keeps a value in its '
                      'span', failed)


class Shift(Fixture):
    def setUp(self):
        super().setUp()
        # a.c takes a name in a disc row of the image, and one past its span, which the tables give
        self.write('build/obj/src/a.obj', obj(SECTIONS['src/a.c'], references=('_inside', '_outside')))
        self.write('config/1st_read/symbols.txt', '0x8c010020 inside\n0x8c300000 outside\n')
        self.write('orig/1ST_READ.BIN', BASE)
        self.write('build/1ST_READ.BIN', BASE)

    def relink(self, kept, *flags):
        """Each link relink.main gives lnk for the main executable with --shift and flags, against kept for
        config/kept.txt, as (its directory, its starts, the names its symbols object defines), and what it prints.
        lnk's link stands in: it writes BASE's bytes outside the disc rows, but for the word at 0, which moves by
        the link's shift, and a map that gives inside and outside their rows."""
        self.write('config/kept.txt', kept)
        links, names = [], {}

        def place(objects, starts, out, options):
            shift = starts['P'] - 0x8c010000
            links.append((out.parent.name, starts, names[out.parent]))
            self.write(out.with_name('link.map'), '***  LINKAGE EDITOR EXTERNALLY DEFINED SYMBOLS LIST  ***\r\n'
                                                  "_inside                           H'8C010020               DAT\r\n"
                                                  "_outside                          H'8C300000               DAT\r\n")
            image = written(BASE, {0: 0x8c010040 + shift})
            return {0x8c010000 + at + shift: byte for at, byte in enumerate(image)
                    if not 0x10 <= at < 0x40 and not 0x78 <= at < 0x80}

        def define(path, defined, reserved=()):
            names[path.parent] = defined
            return path.with_suffix('.obj')

        printed = io.StringIO()
        with mock.patch.object(relink, 'define', define), mock.patch.object(relink, 'place', place), \
                redirect_stdout(printed):
            relink.main(['--shift', '1st_read', 'orig/1ST_READ.BIN', 'build/1ST_READ.BIN', *flags])
        return links, printed.getvalue()

    KEPT = ('1st_read  0x8c010004  0x8c010010  three addresses, one in each region\n'
            '1st_read  0x8c010048  0x8c010050  two counts\n')

    def test_links_the_image_at_two_shifted_bases_and_writes_each_words_reason(self):
        links, printed = self.relink(self.KEPT)
        # each section, and the name inside the image's span, moves by the shift; outside stays where it is
        for (directory, starts, defined), name, shift in zip(links, ('1st_read', 'shift1', 'shift2'),
                                                             (0, *relink.SHIFTS)):
            with self.subTest(shift=shift):
                self.assertEqual(directory, name)
                self.assertEqual(list(starts.items()), [('P', 0x8c010000 + shift), ('C', 0x8c010060 + shift),
                                                        ('CUP', 0x8c010060 + shift), ('C2', 0x8c010060 + shift),
                                                        ('B', 0x8c010080 + shift)])
                self.assertEqual(defined, [(0x8c010020 + shift, 'inside'), (0x8c300000, 'outside')])
        self.assertEqual(content('build/relink/1st_read.bin'), BASE)
        self.assertEqual(content('build/relink/1st_read/fixed.txt').decode(),
                         '1st_read  0x8c010004  0x8c010060  src/a.c  three addresses, one in each region\n'
                         '1st_read  0x8c010008  0x0c010064  src/a.c  three addresses, one in each region\n'
                         '1st_read  0x8c01000c  0xac010068  src/a.c  three addresses, one in each region\n'
                         '1st_read  0x8c010048  0x8c0100a0  src/b.c  two counts\n'
                         '1st_read  0x8c01004c  0x8c01009c  src/b.c  two counts\n')
        self.assertTrue(printed.endswith('1st_read: each shift moves 1 of its words, and keeps 5 at a value in its '
                                         'span\n'), printed)

    def test_lists_each_name_bound_by_address_with_verbose(self):
        _, printed = self.relink(self.KEPT, '--verbose')
        self.assertIn('1st_read: binds inside at 0x8c010020 by address, in a disc row of P\n', printed)
        self.assertIn('1st_read: binds outside at 0x8c300000 by address, outside the image\n', printed)

    def test_fails_a_word_without_a_reason_and_writes_neither_file(self):
        # another image's row at the word's offset gives no reason for it
        self.image('stg03', 'file SONIC2/STG03.PRS\nP 0x8c500000 0x8c500200\n')
        failed = self.fails(self.relink, '1st_read  0x8c010004  0x8c010010  three addresses, one in each region\n'
                                         'stg03  0x8c50004c  another image\n')
        self.assertIn("1st_read: its word at 0x8c01004c, which src/b.c writes, keeps 0x8c01009c, in its span, and "
                      'config/kept.txt gives no reason', failed)
        self.assertFalse(Path('build/relink/1st_read.bin').exists())
        self.assertFalse(Path('build/relink/1st_read/fixed.txt').exists())

    def test_fails_a_malformed_row_before_any_link(self):
        self.assertIn('config/kept.txt:1: not a row: IMAGE START [END] REASON',
                      self.fails(self.relink, '1st_read  0x8c010004\n'))
        self.assertFalse(Path('build/relink/1st_read').exists())

    def test_fails_a_section_aligned_to_more_than_the_shifts_keep(self):
        self.write('build/obj/src/c.obj', obj([('C', 8, 64, False)]))
        self.assertIn("src/c.c: aligns C to 64, more than the shift test's shifts keep",
                      self.fails(self.relink, self.KEPT))
