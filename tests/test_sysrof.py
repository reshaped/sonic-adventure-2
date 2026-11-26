"""The object reader: the section table, the references, and a damaged object."""

import sysrof
from support import TreeTest, obj, record


class Objects(TreeTest):
    def test_reads_sections_and_references(self):
        self.write('a.obj', obj([('P', 0x840, 32, True), ('D', 4, 4, False)], ['_syMalloc', '_task_levels']))
        self.assertEqual(sysrof.sections('a.obj'), {'P': sysrof.Section(0x840, 32, True),
                                                    'D': sysrof.Section(4, 4, False)})
        self.assertEqual(sysrof.references('a.obj'), {'_syMalloc', '_task_levels'})

    def test_reads_each_export_with_its_section_and_offset(self):
        self.write('a.obj', obj([('P', 0x40, 4, True), ('D', 8, 4, False)], exports=[('_f', 0, 0x18), ('_d', 1, 4)]))
        self.assertEqual(sysrof.exports('a.obj'), {'_f': ('P', 0x18), '_d': ('D', 4)})
        self.write('b.obj', obj([('P', 0x40, 4, True)], exports=[('_d', 1, 4)]))
        self.assertIn('name section 1, and the object has 1', self.fails(sysrof.exports, 'b.obj'))
        # one name at two places fails in either order; two names at one place don't
        for given in ([('_f', 0, 0), ('_f', 0, 4)], [('_f', 0, 4), ('_f', 0, 0)]):
            self.write('c.obj', obj([('P', 8, 4, True)], exports=given))
            self.assertIn('put _f at P+', self.fails(sysrof.exports, 'c.obj'))
        self.write('d.obj', obj([('P', 8, 4, True)], exports=[('_f', 0, 4), ('_g', 0, 4), ('_f', 0, 4)]))
        self.assertEqual(sysrof.exports('d.obj'), {'_f': ('P', 4), '_g': ('P', 4)})

    def test_a_run_joins_the_bodies_of_its_records(self):
        names = b'\xc0\x04_one\xc0\x04_two'
        self.write('a.obj', record(0x0C, names[:7], last=False) + record(0x0C, names[7:]) + record(0x7F, b''))
        self.assertEqual(sysrof.references('a.obj'), {'_one', '_two'})

    def test_refuses_a_damaged_object(self):
        good = obj([('P', 4, 4, True)])
        self.write('sum.obj', good[:5] + bytes([good[5] ^ 1]) + good[6:])
        self.assertIn('fails its checksum', self.fails(sysrof.sections, 'sum.obj'))
        self.write('short.obj', good[:-1])
        self.assertIn('is cut short', self.fails(sysrof.sections, 'short.obj'))
        self.write('open.obj', record(0x08, b''))
        self.assertIn('no end record', self.fails(sysrof.sections, 'open.obj'))
        self.write('long.obj', good + b'\x00')
        self.assertIn('bytes follow its end', self.fails(sysrof.sections, 'long.obj'))
        # a reader that stops at the end record still meets the bytes after it
        self.write('end.obj', record(0x7F, b'') + b'\x00')
        self.assertIn('bytes follow its end', self.fails(next, sysrof.records('end.obj')))
        self.write('end.obj', record(0x7F, b''))
        self.assertEqual(list(sysrof.records('end.obj')), [(0, record(0x7F, b''))])

    def test_reads_the_sections_whose_bounds_a_relocation_takes(self):
        # three relocations of a table's words: (STARTOF R)+(SIZEOF R) in a long head, SIZEOF B in a short one, and an
        # outside name plus a constant, which asks no bounds. The sections come in the table's order
        def long(offset, expression):
            return bytes([0x35]) + offset.to_bytes(4, 'big') + bytes.fromhex('1808100808080008') + \
                bytes([len(expression)]) + expression

        def short(offset, expression):
            return bytes([0x04]) + offset.to_bytes(4, 'big') + b'\x00\x20' + bytes([len(expression)]) + expression
        sections = [('C', 12, 4, False), ('R', 0, 4, False), ('B', 0, 4, False), ('D', 0, 4, False)]
        relocations = (long(0, bytes.fromhex('05000104000120ff')) + short(4, bytes.fromhex('040002ff')) +
                       long(8, bytes.fromhex('020000030400000010' + '20ff')))
        # each object below is table with a run of relocations ahead of its end record
        table = obj(sections, ['_f'])
        self.write('a.obj', table[:-3] + record(0x20, relocations) + table[-3:])
        self.assertEqual(sysrof.bounded('a.obj'), ['R', 'B'])
        # an object whose relocations take no section's bounds asks none, and one whose relocation doesn't parse fails
        self.write('b.obj', table[:-3] + record(0x20, long(8, bytes.fromhex('020000ff'))) + table[-3:])
        self.assertEqual(sysrof.bounded('b.obj'), [])
        for broken in (b'\x36' + long(0, b'\x05\x00\x01\xff')[1:], long(0, bytes.fromhex('0500010aff')),
                       long(0, bytes.fromhex('050009ff')), long(0, bytes.fromhex('050004ff'))):
            with self.subTest(broken=broken.hex()):
                self.write('c.obj', table[:-3] + record(0x20, broken) + table[-3:])
                self.assertIn("the relocations at", self.fails(sysrof.bounded, 'c.obj'))

    def test_a_relative_branch_takes_its_target_from_another_object(self):
        # R32's BSR _outside at offset 0 in PSG. It shifts (target - (PC + 4))
        # right one bit. The absolute reference that follows takes SIZEOF R.
        table = obj([('PSG', 8, 4, True), ('R', 0, 4, False)], ['_outside'])
        branch = bytes.fromhex('14 00000000 0c040008 1a '
                               '020000 000000 030400000000 20 030400000004 20 21 28 250101 ff')
        bound = bytes.fromhex('04 00000004 0020 04 040001 ff')
        self.write('branch.obj', table[:-3] + record(0x20, branch + bound) + table[-3:])
        self.assertEqual(sysrof.bounded('branch.obj'), ['R'])
        # A shift count cannot run past the expression, even when another
        # relocation follows it.
        truncated = branch[:-3] + bytes.fromhex('0401ff')
        self.write('short.obj', table[:-3] + record(0x20, truncated + bound) + table[-3:])
        self.assertIn("the relocations at", self.fails(sysrof.bounded, 'short.obj'))

    def test_reads_the_target_of_a_word(self):
        # the long at offset 4 in D takes reference 0, and the one at 8 its own SIZEOF, which names no target
        word = bytes([0x35, 0, 0, 0, 4]) + bytes.fromhex('1808100808080008') + bytes([4]) + bytes.fromhex('020000ff')
        size = bytes([0x35, 0, 0, 0, 8]) + bytes.fromhex('1808100808080008') + bytes([4]) + bytes.fromhex('040000ff')

        def with_relocations(references):
            table = obj([('D', 12, 4, False)], references)
            return table[:-3] + record(0x1A, bytes(4)) + record(0x20, word + size) + table[-3:]
        self.write('a.obj', with_relocations(['_f']))
        self.assertEqual(sysrof.word_target('a.obj', 'D', 4), ('name', '_f'))
        self.assertIsNone(sysrof.word_target('a.obj', 'D', 8))
        self.write('b.obj', with_relocations([]))
        self.assertIn('take reference 0, and the object has 0', self.fails(sysrof.word_target, 'b.obj', 'D', 4))

    def test_spells_a_name_as_c_does(self):
        self.assertEqual([sysrof.c_name(name) for name in ('_task_alloc', '__nj_stack', 'R0')],
                         ['task_alloc', '_nj_stack', 'R0'])

    def test_refuses_an_alignment_that_is_no_power_of_two(self):
        self.write('a.obj', obj([('P', 4, 12, True)]))
        self.assertIn('align P to 12', self.fails(sysrof.sections, 'a.obj'))
