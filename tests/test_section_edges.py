"""Original native and SDK section transitions checked from their relocations."""

import hashlib
import config
import splice
from support import TreeTest, obj, record


def input_hash(source, start, end, alignment):
    row = f'{source}\t0x{start:08x}\t0x{end:08x}\t{alignment}\n'
    return hashlib.sha256(row.encode()).hexdigest()


def relocated(raw, fields):
    # The native table is the first section in the test object.
    relocations = record(0x1a, bytes(4))
    for offset, expression in fields:
        field = b'\x35' + offset.to_bytes(4, 'big') + bytes.fromhex('1808100808080008')
        relocations += record(0x20, field + bytes([len(expression)]) + expression)
    return raw[:-3] + relocations + raw[-3:]


class LibraryBoundary(TreeTest):
    def setUp(self):
        super().setUp()
        for stem in ('start', 'data', 'matrix'):
            self.write(f'src/{stem}.c', '')
        self.image('1st_read', '''file 1ST_READ.BIN
CSG 0x8c010000 0x8c010020
DSG 0x8c010020 0x8c010040
DSG32 0x8c010040 0x8c010044
src/start.c CSG 0x8c010000 0x8c010008
src/data.c DSG 0x8c010020 0x8c010024
src/matrix.c DSG32 0x8c010040 0x8c010044
''')
        self.game = config.Image('1st_read')
        csg = input_hash('src/start.c', 0x8c010000, 0x8c010008, 4)
        dsg = input_hash('src/data.c', 0x8c010020, 0x8c010024, 4)
        self.write('config/1st_read/fill.txt', f'CSG DSG inputs {csg} CSG32 0x8c010000\n'
                                               f'DSG DSG32 inputs {dsg} _matrix 0x8c010004\n')
        self.write('config/1st_read/bounds.txt', 'DSG 0x8c010020 0x8c010024\nDSG32 0x8c010040 0x8c010044\n')
        self.start()
        self.write('build/obj/src/data.obj', obj([('DSG', 4, 4, False)]))
        self.write('build/obj/src/matrix.obj', obj([('DSG32', 4, 32, False)], exports=[('_matrix', 0, 0)]))
        self.pointers = bytes.fromhex('2000018c4000018c')
        self.write('build/link/1st_read/src/start.bin', self.pointers)
        self.write('build/link/1st_read/src/data.bin', b'DATA')
        self.write('build/link/1st_read/src/matrix.bin', b'MTRX')

    def start(self, first=2, second=3):
        raw = obj([('CSG', 8, 4, False), ('CSG32', 0, 32, False), ('DSG', 0, 4, False), ('DSG32', 0, 32, False)])
        fields = [(off, bytes([5, 0, index, 255])) for off, index in ((0, first), (4, second))]
        self.write('build/obj/src/start.obj', relocated(raw, fields))

    def test_both_edges_come_from_the_complete_objects(self):
        original = self.pointers + bytes(24) + b'DATA' + bytes(28) + b'MTRX'
        self.assertEqual(splice.splice(self.game, original), original)
        self.assertEqual(splice.counts(self.game), ({}, {'C': 16, 'fill': 52}))

    def test_wrong_section_relocations_fail_even_when_the_words_match(self):
        for first, second, expected in ((1, 3, 'STARTOF DSG'), (2, 2, 'relocation to DSG32')):
            with self.subTest(first=first, second=second):
                self.start(first, second)
                self.assertIn(expected, self.fails(splice.check, self.game))


class OverlayBoundary(TreeTest):
    def setUp(self):
        super().setUp()
        for stem in ('head', 'code'):
            self.write(f'src/{stem}.c', '')
        self.image('1st_read', '''file 1ST_READ.BIN
DHEAD 0x8c010000 0x8c010020
P 0x8c010020 0x8c010028
src/head.c DHEAD 0x8c010000 0x8c01000c
src/code.c P 0x8c010020 0x8c010028
''')
        self.game = config.Image('1st_read')
        digest = input_hash('src/head.c', 0x8c010000, 0x8c01000c, 4)
        self.write('config/1st_read/fill.txt', f'DHEAD P inputs {digest} _step 0x8c010008\n')
        self.header = b'GARDEN  ' + bytes.fromhex('2200018c')
        self.write('build/link/1st_read/src/head.bin', self.header)
        self.write('build/link/1st_read/src/code.bin', b'abcdefgh')
        self.write('build/obj/src/head.obj', relocated(obj([('DHEAD', 12, 4, False)], ['_step']),
                                                       [(8, bytes.fromhex('020000ff'))]))
        self.code(2)

    def code(self, offset):
        self.write('build/obj/src/code.obj', obj([('P', 8, 32, True)], exports=[('_step', 0, offset)]))

    def test_header_points_inside_first_code_input(self):
        original = self.header + bytes(20) + b'abcdefgh'
        self.assertEqual(splice.splice(self.game, original), original)
        self.assertEqual(splice.counts(self.game), ({'C': 8}, {'C': 12, 'fill': 20}))

    def test_other_offsets_do_not_establish_the_code_start(self):
        for offset, rejection in ((0, 'does not hold'), (1, 'instruction'), (8, 'instruction')):
            with self.subTest(offset=offset):
                self.code(offset)
                self.assertIn(rejection, self.fails(splice.check, self.game))


class PrivateDataBoundary(TreeTest):
    def setUp(self):
        super().setUp()
        self.write('src/head.c', '')
        self.write('src/menu.c', '')
        self.image('1st_read', '''file 1ST_READ.BIN
DHEAD 0x8c010000 0x8c010004
C 0x8c010004 0x8c010008
D 0x8c010008 0x8c010010
src/head.c DHEAD 0x8c010000 0x8c010004
src/menu.c C 0x8c010004 0x8c010007
src/menu.c D 0x8c010008 0x8c010010
''')
        self.game = config.Image('1st_read')
        digest = input_hash('src/menu.c', 0x8c010004, 0x8c010007, 4)
        self.write('config/1st_read/fill.txt', f'C D inputs {digest} D 0x8c01000c\n')
        self.write('build/obj/src/head.obj', obj([('DHEAD', 4, 4, False)]))
        self.write('build/link/1st_read/src/head.bin', b'HEAD')
        self.menu()

    def menu(self, term='000000ff', extra=(), pointer=0x8c010008):
        # A private pointer to the start of D is held in D itself, as in TITLE.
        data = obj([('D', 8, 4, False), ('C', 3, 4, False), *extra])
        self.write('build/obj/src/menu.obj', relocated(data, [(4, bytes.fromhex(term))]))
        self.write('build/link/1st_read/src/menu.bin', b'END' + b'DATA' + pointer.to_bytes(4, 'little'))

    def test_private_data_reference_proves_the_overlay_boundary(self):
        original = b'HEADEND\x00DATA' + bytes.fromhex('0800018c')
        self.assertEqual(splice.splice(self.game, original), original)
        self.assertEqual(splice.counts(self.game), ({}, {'C': 15, 'fill': 1}))

    def test_relocation_and_direct_section_order_are_required(self):
        for term, extra, pointer, expected in (
            ('000001ff', (), 0x8c010008, 'own D input'),
            ('050000ff', (), 0x8c010008, 'own D input'),
            ('000000ff', [('C32', 0, 32, False)], 0x8c010008, 'declares C32'),
            ('000000ff', (), 0x8c01000c, 'does not hold'),
        ):
            with self.subTest(term=term, extra=extra, pointer=pointer):
                self.menu(term, extra, pointer)
                self.assertIn(expected, self.fails(splice.check, self.game))

    def test_another_object_names_the_export_at_the_start_of_data(self):
        # The Chao race scene references the key light exported by its first D object.
        self.write('src/scene.c', '')
        self.image('1st_read', '''file 1ST_READ.BIN
DHEAD 0x8c010000 0x8c010004
P 0x8c010004 0x8c010008
C 0x8c010008 0x8c01000c
D 0x8c01000c 0x8c010010
src/head.c DHEAD 0x8c010000 0x8c010004
src/scene.c P 0x8c010004 0x8c010008
src/menu.c C 0x8c010008 0x8c01000b
src/menu.c D 0x8c01000c 0x8c010010
''')
        game = config.Image('1st_read')
        digest = input_hash('src/menu.c', 0x8c010008, 0x8c01000b, 4)
        self.write('config/1st_read/fill.txt', f'C D inputs {digest} D 0x8c010004\n')
        pointer = bytes.fromhex('0c00018c')
        self.write('build/link/1st_read/src/scene.bin', pointer)
        self.write('build/link/1st_read/src/menu.bin', b'ENDDATA')
        scene = obj([('P', 4, 4, True)], ['_light'])
        self.write('build/obj/src/scene.obj', relocated(scene, [(0, bytes.fromhex('020000ff'))]))
        for name, offset in (('_other', 0), ('_light', 1), ('_light', 0)):
            with self.subTest(name=name, offset=offset):
                self.write('build/obj/src/menu.obj', obj([('D', 4, 4, False), ('C', 3, 4, False)],
                                                         exports=[(name, 0, offset)]))
                if name == '_light' and offset == 0:
                    original = b'HEAD' + pointer + b'END\x00DATA'
                    self.assertEqual(splice.splice(game, original), original)
                    self.assertEqual(splice.counts(game), ({'C': 4}, {'C': 11, 'fill': 1}))
                else:
                    self.assertIn('export at the start', self.fails(splice.check, game))
