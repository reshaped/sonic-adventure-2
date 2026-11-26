"""The mixed-source initializer reads values with the layout of the declared C type."""
import ctypes
from decimal import Context, Decimal
import itertools
import os
from pathlib import Path
import re
import struct
import subprocess
import sys
import tempfile
import unittest
import unittest.mock

import body_bindings
import body_links
from body_data import float_literal, generate, with_bodies
from body_inputs import write_rules
from config import Image


class BodyData(unittest.TestCase):
    source = Path('sdk/sample/provider.c')
    table = Path('offsets/sdk/sample/provider.txt')
    prefix = Path('sdk/sample/provider')
    config = Path('config/cart')
    offsets = 'datum callbacks D 0x00000000 12\ndatum tag D 0x0000000c 4\n'
    bindings = 'pointer callbacks 0x00000000 tag\npointer callbacks 0x00000008 start\n'
    image = struct.pack('<IHBBI4s', 0x100c, 0x8001, 255, 0, 0x2000, b'abc\0')

    def setUp(self):
        temp = tempfile.TemporaryDirectory()
        self.addCleanup(temp.cleanup)
        self.addCleanup(os.chdir, Path.cwd())
        os.chdir(temp.name)
        for parent in (self.source.parent, self.table.parent, self.config):
            parent.mkdir(parents=True)
        self.source.with_suffix('.h').write_text('''\
struct callbacks {
    char *tag;
    unsigned short id;
    unsigned char count;
    void (*start)(void);
};
void start(void);
''')
        self.source.write_text('''\
#include "provider.h"
static char tag[4];
struct callbacks callbacks = {
#include "sample/provider/callbacks.inc"
};
static char tag[4] = {
#include "sample/provider/tag.inc"
};
''')
        self.table.write_text(self.offsets)
        self.table.with_suffix('.bindings').write_text(self.bindings)
        (self.config / 'splits.txt').write_text('file CART.BIN\nD 0x00001000 0x00001010\n'
                                              'sdk/sample/provider.c D 0x00001000 0x00001010\n')
        (self.config / 'symbols.txt').write_text('0x00001000 callbacks\n0x00002000 start\n')

    def test_selectors_keep_control_fields_in_disc_free_layouts(self):
        self.source.write_text(
            'enum { ACCEPT = 3 };\n'
            'struct box { unsigned short id; signed short x; };\n'
            'struct box buttons[2] = {\n#include "sample/provider/buttons.inc"\n};\n')
        self.table.write_text('datum buttons D 0x00000004 8\n')
        binding = self.table.with_suffix('.bindings')
        binding.write_text('selector buttons[1].id ACCEPT\n')
        image = Image('cart')
        data = b'1234' + struct.pack('<HhHh', 1, -7, 3, 15) + b'5678'
        output = Path('output')
        body = output / self.prefix / 'buttons.inc'
        generate(image, self.source, output, data)
        self.assertEqual(body.read_text().strip(), '{1, -7}, {3, 15}')
        self.assertEqual(body_links.expected(image, self.source, data),
                         b'1234' + struct.pack('<HhHh', 0, 0, 3, 0) + b'5678')
        with self.assertRaisesRegex(SystemExit, 'differs from its declared value'):
            generate(image, self.source, output, bytes(len(data)))
        with self.assertRaisesRegex(SystemExit, 'differs from its declared value'):
            body_links.expected(image, self.source, bytes(len(data)))
        # without the disc, the zeros the table's place reads are no original to check
        generate(image, self.source, output)
        self.assertEqual(body.read_text().strip(), '{0, 0}, {3, 0}')
        self.table.unlink()
        generate(image, self.source, output, stub=True)
        self.assertEqual(body.read_text().strip(), '{0, 0}, {3, 0}')
        binding.write_text('selector buttons[2].id ACCEPT\n')
        with self.assertRaisesRegex(SystemExit, r'selector buttons\[2\]\.id indexes outside its declared array'):
            generate(image, self.source, output, stub=True)
        binding.write_text('selector buttons[1].unknown ACCEPT\n')
        with self.assertRaisesRegex(SystemExit, r'selector buttons\[1\]\.unknown has no declared member unknown'):
            generate(image, self.source, output, stub=True)

    def test_linked_stub_may_replace_only_measured_data(self):
        # The linked member omits the gap between the two writable ranges.
        # Its entry and BSS rows locate pointer targets.
        (self.config / 'splits.txt').write_text(
            'file CART.BIN\nP 0x00001000 0x00001004\nDSG 0x00002000 0x00002020\n'
            'B 0x00003000 0x00003004\n'
            'sdk/sample/provider.c P 0x00001000 0x00001004\n'
            'sdk/sample/provider.c D:DSG 0x00002000 0x00002010\n'
            'sdk/sample/provider.c D:DSG 0x00002014 0x00002020\n'
            'sdk/sample/provider.c B 0x00003000 0x00003004\n')
        self.table.write_text(self.offsets + 'entry start P 0x00000000 2\n'
                              'text FIRST D 0x00000014 4\ntext SECOND D 0x00000018 4\n'
                              'datum work B 0x00000000 4\n')
        self.table.with_suffix('.bindings').write_text(self.bindings + 'text SECOND 4\ntext FIRST 4\n')
        full = Path('build/link/cart/sdk/sample/provider.bin')
        stub = Path('build/stub/link/cart/sdk/sample/provider.bin')
        for path in (full, stub):
            path.parent.mkdir(parents=True)
        native = b'code' + struct.pack('<IHBBI', 0x200c, 0x8001, 255, 0, 0x1000) + b'abc\0'
        full.write_bytes(native + b'one\0two\0tail')
        checked = b'code' + struct.pack('<III', 0x200c, 0, 0x1000) + bytes(4) + b'"!!\0!!!\0tail'
        stub.write_bytes(checked)
        image = Image('cart')
        body_links.check(image, self.source)
        for index, address in ((1, 0x1001), (4, 0x2000), (8, 0x2004), (12, 0x2008),
                               (20, 0x2014), (27, 0x201b), (31, 0x201f)):
            with self.subTest(index=index), self.assertRaisesRegex(SystemExit, f'{address:#010x}'):
                damaged = bytearray(checked)
                damaged[index] ^= 1
                stub.write_bytes(damaged)
                body_links.check(image, self.source)
        stub.write_bytes(checked[:-1])
        with self.assertRaisesRegex(SystemExit, 'different lengths'):
            body_links.check(image, self.source)

    def test_an_empty_source_selects_no_member(self):
        # an empty source is a source no image places, not an omitted one
        with unittest.mock.patch.object(sys, 'argv', ['body_links', 'cart', '']), \
                self.assertRaisesRegex(SystemExit, 'cart:  has no disc-backed member placement'):
            body_links.main()

    def test_signed_scalar_stop_preserves_empty_neutral_scan(self):
        self.source.write_text('#define END_NODE (-1)\n'
                               'int nodes[3] = {\n#include "sample/provider/nodes.inc"\n};\n')
        self.table.write_text('datum nodes D 0x00000004 12\n')
        self.table.with_suffix('.bindings').write_text('stop nodes END_NODE\n')
        image = Image('cart')
        output = Path('output')
        body = output / self.prefix / 'nodes.inc'
        full = b'keep' + struct.pack('<iii', 3, 6, -1)
        generate(image, self.source, output, full)
        self.assertEqual(body.read_text().strip(), '3, 6, -1')
        generate(image, self.source, output, stub=True)
        self.assertEqual(body.read_text().strip(), '-1, 0, 0')
        self.assertEqual(body_links.expected(image, self.source, full),
                         b'keep' + struct.pack('<iii', -1, 0, 0))
        with self.assertRaisesRegex(SystemExit, 'final record'):
            generate(image, self.source, output, b'keep' + struct.pack('<iii', 3, -1, -1))

    def test_signed_stop_terminates_the_first_neutral_record(self):
        declaration = ('#define HUNT_HINT_END (-2)\n'
                       'struct hint { unsigned short id; signed short kind; };\n'
                       'struct hint hints[3] = {\n#include "sample/provider/hints.inc"\n};\n')
        self.source.write_text(declaration)
        self.table.write_text('datum hints D 0x00000004 12\n')
        bindings = self.table.with_suffix('.bindings')
        binding = 'stop hints kind HUNT_HINT_END\n'
        bindings.write_text(binding)
        image = Image('cart')
        rows = struct.pack('<HhHhHh', 8, 1, 9, 3, 7, -2)
        full = b'keep' + rows
        output = Path('output')
        body = output / self.prefix / 'hints.inc'
        generate(image, self.source, output, full)
        self.assertEqual(body.read_text().strip(), '{8, 1}, {9, 3}, {7, -2}')
        self.assertEqual(body_links.expected(image, self.source, full), b'keep\0\0\xfe\xff' + bytes(8))
        for row, kind in ((0, -2), (2, 0)):
            invalid = bytearray(full)
            struct.pack_into('<h', invalid, 4 + row * 4 + 2, kind)
            for check in (lambda: generate(image, self.source, output, invalid),
                          lambda: body_links.expected(image, self.source, invalid)):
                with self.assertRaisesRegex(SystemExit, 'final record'):
                    check()

        self.table.write_text('datum hints D 0x00000004 8\n')
        with self.assertRaisesRegex(SystemExit, 'stop table extent'):
            body_links.expected(image, self.source, full)
        self.table.unlink()  # Disc-free generation needs only the C declaration and its binding.
        generate(image, self.source, output, stub=True)
        self.assertEqual(body.read_text().strip(), '{0, -2}, {0, 0}, {0, 0}')
        for old, new, error in (('(-2)', '0', 'must be negative'), ('(-2)', '(-32769)', 'fit the signed field'),
                                ('signed short kind', 'unsigned short kind', 'signed integer member'),
                                ('signed short kind', 'void *kind', 'signed integer member'),
                                ('signed short kind', 'float kind', 'signed integer member'),
                                ('hints[3]', 'hints[0]', 'empty stop table')):
            with self.subTest(new=new), self.assertRaisesRegex(SystemExit, error):
                self.source.write_text(declaration.replace(old, new))
                generate(image, self.source, output, stub=True)
        self.source.write_text(declaration)
        for value, error in ((binding * 2, 'repeated stop'), (binding.replace('hints', 'absent'), 'no initialized'),
                             (binding.replace('kind', 'absent'), 'no such struct member'),
                             (binding.replace('HUNT_HINT_END', '-2'), 'CONSTANT_NAME')):
            with self.subTest(value=value), self.assertRaisesRegex(SystemExit, error):
                bindings.write_text(value)
                generate(image, self.source, output, stub=True)

    def test_a_stop_that_overwrites_a_selector_fails_without_the_disc(self):
        # the original's first record keeps its selected kind, and its last its stop
        self.source.write_text('#define HUNT_HINT_END (-2)\nenum { SEEK = 1 };\n'
                               'struct hint { unsigned short id; signed short kind; };\n'
                               'struct hint hints[2] = {\n#include "sample/provider/hints.inc"\n};\n')
        self.table.write_text('datum hints D 0x00000004 8\n')
        self.table.with_suffix('.bindings').write_text('stop hints kind HUNT_HINT_END\nselector hints[0].kind SEEK\n')
        output = Path('output')
        generate(Image('cart'), self.source, output, b'keep' + struct.pack('<HhHh', 8, 1, 9, -2))
        self.assertEqual((output / self.prefix / 'hints.inc').read_text().strip(), '{8, 1}, {9, -2}')
        for stub in (False, True):
            with self.subTest(stub=stub), self.assertRaisesRegex(
                    SystemExit, re.escape('hints: the stop in its first record overwrites selector hints[0].kind')):
                generate(Image('cart'), self.source, output, stub=stub)

    def test_build_rules_choose_the_first_image_and_keep_wrapper_inputs_separate(self):
        wrapper = self.source.with_name('wrapper.c')
        wrapper.write_text('#include "provider.c"\n')
        earlier = Path('config/1st_read/splits.txt')
        earlier.parent.mkdir()
        earlier.write_text('file 1ST_READ.BIN\nD 0x00002000 0x00002010\n'
                           'sdk/sample/provider.c D 0x00002000 0x00002010\n')
        (self.config / 'splits.txt').write_text('file CART.BIN\nD 0x00001000 0x00001020\n'
                                              'sdk/sample/provider.c D 0x00001000 0x00001010\n'
                                              'sdk/sample/wrapper.c D 0x00001010 0x00001020\n')
        self.table.with_suffix('.bindings').write_text('include tags\n')
        shared = Path('layouts/tags.bindings')
        shared.parent.mkdir()
        shared.write_text('pointer callbacks 0x00000000 tag\n')
        output = Path('build/inputs.mk')
        write_rules(output, [self.source, wrapper])
        rules = dict(line.split(' := ') for line in output.read_text().splitlines())
        self.assertEqual(rules['body_image.' + str(self.source)], '1st_read')
        self.assertEqual(rules['body_image.' + str(wrapper)], 'cart')
        self.assertIn(str(shared), rules['body_headers.' + str(self.source)].split())
        self.assertNotIn(str(shared), rules['body_headers.' + str(wrapper)].split())
        self.assertEqual(rules['body_files.' + str(wrapper)].split(),
                         ['sample/provider/callbacks.inc', 'sample/provider/tag.inc'])

    def test_build_rules_are_the_same_for_a_list_an_iterator_and_a_repeated_source(self):
        output = Path('build/inputs.mk')
        write_rules(output, [self.source])
        once = output.read_text()
        for sources in (iter([self.source]), [self.source, self.source]):
            output.unlink()
            write_rules(output, sources)
            self.assertEqual(output.read_text(), once)

    def test_source_local_callback_uses_its_native_program_entry(self):
        self.source.write_text('''
typedef void callback(void);
struct actions { callback *step; char *tag; };
static void tick(void) { }
static struct actions actions[1] = {
#include "sample/provider/actions.inc"
};
''')
        splits = Path(self.config, 'splits.txt')
        splits.write_text('file CART.BIN\n'
                          'D 0x00001000 0x00001008\n'
                          'P 0x00001008 0x00001010\n'
                          'sdk/sample/provider.c D 0x00001000 0x00001008\n'
                          'sdk/sample/provider.c P 0x00001008 0x00001010\n')
        (self.config / 'symbols.txt').write_text('')
        bindings = self.table.with_suffix('.bindings')
        bindings.write_text('pointer actions 0x00000000 tick\n')
        datum = 'datum actions D 0x00000000 8\n'
        self.table.write_text(datum + 'entry tick P 0x00000002 2\n')
        original = struct.pack('<4I', 0x100a, 0, 0, 0)
        generate(Image('cart'), self.source, Path('out'), original)
        include = Path('out/sdk/sample/provider/actions.inc')
        self.assertEqual(include.read_text().strip(), '{tick, 0}')
        self.table.unlink()
        generate(Image('cart'), self.source, Path('out'), None, stub=True)
        self.assertEqual(include.read_text().strip(), '{tick, 0}')
        for entry, error in [('entry tick P 0x00000000 2', 'expected tick'),
                             ('entry tick P 0x00000001 2', 'aligned SH instruction'),
                             ('entry tick P 0x00000008 2', 'beyond its P placement'),
                             ('entry tick D 0x00000000 2', 'invalid datum'),
                             ('entry other P 0x00000002 2',
                              'other: the table gives an entry, the source wants none; '
                              'tick: the table gives none, the source wants an entry')]:
            with self.subTest(entry=entry):
                self.table.write_text(datum + entry + '\n')
                with self.assertRaisesRegex(SystemExit, error):
                    generate(Image('cart'), self.source, Path('out'), original)
        self.table.write_text(datum + 'entry tick P 0x00000002 2\n')
        bindings.write_text('pointer actions 0x00000004 tick\n')
        with self.assertRaisesRegex(SystemExit, 'needs a C function pointer'):
            generate(Image('cart'), self.source, Path('out'), struct.pack('<4I', 0, 0x100a, 0, 0))

    def test_nested_union_initialized_with_its_scalar_word(self):
        self.source.write_text('''\
typedef union { unsigned int word; unsigned char bytes[4]; } color;
struct material { color diffuse, specular; unsigned int flags; };
static struct material material[1] = {
#include "sample/provider/material.inc"
};
''')
        self.table.write_text('datum material D 0x00000000 12\n')
        self.table.with_suffix('.bindings').write_text('')
        generate(Image('cart'), self.source, Path('out'), struct.pack('<III', 0xffffffff, 0, 4))
        result = Path('out/sdk/sample/provider/material.inc').read_text()
        self.assertEqual(result.strip(), '{4294967295U, 0, 4}')

    def test_first_array_in_a_nested_union_keeps_one_brace_group(self):
        self.source.write_text('''\
union filename { char text[8]; unsigned int alignment; };
struct names { union filename sky; char horizon[8]; };
static const struct names names = {
#include "sample/provider/names.inc"
};
''')
        self.table.write_text('datum names D 0x00000000 16\n')
        self.table.with_suffix('.bindings').write_text('')
        output = Path('out/sdk/sample/provider/names.inc')
        generate(Image('cart'), self.source, Path('out'), b'cloud\0\0\0sand\0\0\0\0')
        self.assertEqual(output.read_text().strip(),
                         '{99, 108, 111, 117, 100, 0, 0, 0}, {115, 97, 110, 100, 0, 0, 0, 0}')
        generate(Image('cart'), self.source, Path('out'), stub=True)
        self.assertEqual(output.read_text().strip(), '{0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}')

    def test_first_struct_in_a_nested_union_keeps_fields_together(self):
        self.source.write_text('''\
struct frame { unsigned int left, top; };
union detail { struct frame frame; unsigned char storage[12]; };
struct box { unsigned int width; union detail detail; unsigned int color; };
struct box boxes[1] = {
#include "sample/provider/boxes.inc"
};
''')
        self.table.write_text('datum boxes D 0x00000000 20\n')
        self.table.with_suffix('.bindings').write_text('')
        splits = self.config / 'splits.txt'
        splits.write_text(splits.read_text().replace('0x00001010', '0x00001014'))
        generate(Image('cart'), self.source, Path('out'), struct.pack('<5I', 7, 0, 0, 0, 21))
        result = Path('out/sdk/sample/provider/boxes.inc').read_text()
        self.assertEqual(result.strip(), '{7, {{0}}, 21}')
        payload = struct.pack('<5I', 7, 16, 20, 0, 21)
        generate(Image('cart'), self.source, Path('out'), payload)
        result = Path('out/sdk/sample/provider/boxes.inc').read_text()
        self.assertEqual(result.strip(), '{7, {16, 20}, 21}')
        with self.assertRaisesRegex(SystemExit, 'nonzero padding'):
            generate(Image('cart'), self.source, Path('out'), struct.pack('<5I', 7, 16, 20, 1, 21))

    def test_array_of_texture_groups_points_to_an_array_of_texture_pointers(self):
        self.source.with_suffix('.h').write_text('struct texture { unsigned count; };\n'
                                                'extern struct texture stage_texture;\n')
        self.source.write_text('#include "provider.h"\n'
                               'static struct texture *textures[2] = {\n'
                               '#include "sample/provider/textures.inc"\n};\n'
                               'static struct texture **groups[2] = {\n'
                               '#include "sample/provider/groups.inc"\n};\n')
        self.table.write_text('datum textures D 0x00000000 8\ndatum groups D 0x00000008 8\n')
        self.table.with_suffix('.bindings').write_text('pointer textures 0x00000000 stage_texture\n'
                                                       'pointer groups 0x00000000 textures\n')
        (self.config / 'symbols.txt').write_text('0x00002000 stage_texture\n')
        root = Path('gen')
        for original in (struct.pack('<4I', 0x2000, 0, 0x1000, 0), None):
            generate(Image('cart'), self.source, root, original)
            self.assertEqual((root / self.prefix / 'textures.inc').read_text().strip(), '&stage_texture, 0')
            self.assertEqual((root / self.prefix / 'groups.inc').read_text().strip(), 'textures, 0')

    def test_a_private_datum_takes_its_place_from_the_table(self):
        # a static the source leaves uninitialized, which the table places, whether the body's block names it
        # through an extern or not, and a public one, which symbols.txt places
        declarations = 'struct swing { int angle; };\nstruct swings { struct swing first, second; };\n'
        body = 'static struct swing *links[2] = {\n#include "sample/provider/links.inc"\n};\n'
        block = 'void f(void)\n{\n    extern struct swings current;\n' + body + '}\n'
        (self.config / 'splits.txt').write_text(
            'file CART.BIN\nD 0x00001000 0x00001008\nB 0x00002000 0x0000200c\n'
            'sdk/sample/provider.c D 0x00001000 0x00001008\n'
            'sdk/sample/provider.c B 0x00002000 0x0000200c\n')
        self.table.with_suffix('.bindings').write_text('pointer links 0x00000000 current\n'
                                                       'pointer links 0x00000004 current+0x4\n')
        (self.config / 'symbols.txt').write_text('0x00002004 current\n')
        original = struct.pack('<II', 0x2000, 0x2004)
        output = Path('gen')
        for text, table, data in (
                ('static struct swings current;\n' + body, 'datum current B 0x00000000 8\n', original),
                ('static struct swings current;\n' + block, 'datum current B 0x00000000 8\n', original),
                ('extern struct swings current;\nstruct swings current;\n' + body, '',
                 struct.pack('<II', 0x2004, 0x2008))):
            self.source.write_text(declarations + text)
            self.table.write_text('datum links D 0x00000000 8\n' + table)
            for stub in (False, True):
                with self.subTest(text=text, stub=stub):
                    generate(Image('cart'), self.source, output, None if stub else data, stub=stub)
                    self.assertEqual((output / self.prefix / 'links.inc').read_text().strip(),
                                     '&current.first, &current.second')
        for definition, table, error in (
                ('static', '', 'current: the table gives none, the source wants an uninitialized datum'),
                ('static', 'datum current B 0x00000000 12\n', r'current disagrees with its C type \(size 8'),
                ('', 'datum current B 0x00000000 8\n',
                 'current: the table gives an uninitialized datum, the source wants none')):
            with self.subTest(definition=definition, table=table), self.assertRaisesRegex(SystemExit, error):
                self.source.write_text(declarations + f'{definition} struct swings current;\n' + body)
                self.table.write_text('datum links D 0x00000000 8\n' + table)
                generate(Image('cart'), self.source, output, original)
        self.table.write_text('datum links D 0x00000000 8\ndatum current B 0x00000000 8\n')
        self.source.write_text(declarations + 'static struct missing current;\n' + body)
        with self.assertRaisesRegex(SystemExit, 'current: struct missing: no definition'):
            generate(Image('cart'), self.source, output, original)

    def test_a_static_in_a_block_is_named_from_the_block(self):
        # a body names a static of its own function's blocks, and a body at file scope names no block's static
        declarations = 'struct swing { int angle; };\nstruct pair { struct swing *link; };\n'
        block = ('void f(void)\n{\n    static struct swing current = {\n#include "sample/provider/current.inc"\n'
                 '    };\n    static struct pair links = {\n#include "sample/provider/links.inc"\n    };\n}\n')
        self.source.write_text(declarations + block)
        self.table.write_text('datum current D 0x00000000 4\ndatum links D 0x00000004 4\n')
        self.table.with_suffix('.bindings').write_text('pointer links 0x00000000 current\n')
        original = struct.pack('<4I', 7, 0x1000, 0, 0)
        for stub in (False, True):
            with self.subTest(stub=stub):
                generate(Image('cart'), self.source, Path('gen'), None if stub else original, stub=stub)
                self.assertEqual((Path('gen') / self.prefix / 'links.inc').read_text().strip(), '&current')
        self.source.write_text(declarations + block.replace(
            '    static struct pair links = {\n#include "sample/provider/links.inc"\n    };\n', '') +
            'static struct pair links = {\n#include "sample/provider/links.inc"\n};\n')
        with self.assertRaisesRegex(SystemExit, 'no declaration gives current'):
            generate(Image('cart'), self.source, Path('gen'), stub=True)

    def test_a_name_a_block_may_hide_fails(self):
        # a declarator doesn't give its block, so a block's declaration of the name may hide another, or lie in a
        # sibling block and hide none
        body = '    static struct pair links = {\n#include "sample/provider/links.inc"\n    };\n'
        self.table.write_text('datum links D 0x00000000 4\n')
        self.table.with_suffix('.bindings').write_text('pointer links 0x00000000 current\n')
        (self.config / 'symbols.txt').write_text('0x00002000 current\n')
        for declarations, block, error in (
                ('extern struct swing current;\n', '    static struct swing current;\n' + body,
                 'current has a static declaration in a block of f, and another declaration'),
                ('static struct swing current[2];\n', '    {\n        static struct swing current;\n    }\n' + body,
                 'current has a static declaration in a block of f, and another declaration'),
                # decls.py's declarations() fails two statics of one name in one function first
                ('extern struct swing current;\n', '    {\n        static struct swing current;\n    }\n'
                 '    {\n        static struct swing current;\n    }\n' + body, 'a second static current in f'),
                ('struct swing current;\n', '    struct swing current;\n' + body,
                 'a block of f declares current without static storage')):
            with self.subTest(declarations=declarations, block=block), self.assertRaisesRegex(SystemExit, error):
                self.source.write_text('struct swing { int angle; };\nstruct pair { struct swing *link; };\n' +
                                       declarations + 'void f(void)\n{\n' + block + '}\n')
                generate(Image('cart'), self.source, Path('gen'), stub=True)

    def test_pointer_array_bound_to_an_external_record(self):
        header = self.source.with_suffix('.h')
        definition = 'struct texture { unsigned count; };\n'
        header.write_text(definition + 'extern struct texture sprite;\n')
        self.source.write_text('#include "provider.h"\nstruct texture *sprites[4] = {\n'
                               '#include "sample/provider/sprites.inc"\n};\n')
        self.table.write_text('datum sprites D 0x00000000 16\n')
        bindings = self.table.with_suffix('.bindings')
        bindings.write_text('pointer sprites 0x00000000 sprite\n')
        (self.config / 'symbols.txt').write_text('0x00001000 sprites\n0x00002000 sprite\n')
        output = Path('gen')
        for original in (struct.pack('<4I', 0x2000, 0, 0, 0), None):
            generate(Image('cart'), self.source, output, original)
            self.assertEqual((output / self.prefix / 'sprites.inc').read_text().strip(), '&sprite, 0, 0, 0')
        with self.assertRaisesRegex(SystemExit, 'holds .*expected'):
            generate(Image('cart'), self.source, output, bytes(16))
        for declaration in ('extern struct texture other, sprite;', 'struct texture extern sprite;'):
            with self.subTest(declaration):
                header.write_text(definition + declaration)
                generate(Image('cart'), self.source, output)
                self.assertEqual((output / self.prefix / 'sprites.inc').read_text().strip(), '&sprite, 0, 0, 0')
        for declaration, spelled in (('extern int sprite;', '(struct texture *)&sprite'),
                                     ('extern struct texture *sprite;', '(struct texture *)&sprite'),
                                     ('extern struct texture sprite[4];', 'sprite')):
            with self.subTest(declaration):
                header.write_text(definition + declaration)
                generate(Image('cart'), self.source, output)
                self.assertEqual((output / self.prefix / 'sprites.inc').read_text().strip(), spelled + ', 0, 0, 0')
        header.write_text(definition + 'void f(void) { extern struct texture sprite; }')
        with self.assertRaisesRegex(SystemExit, 'no declaration gives sprite'):
            generate(Image('cart'), self.source, output)

    def test_external_record_address_in_qualified_void_pointer_field(self):
        header = self.source.with_suffix('.h')
        self.table.write_text('datum widget D 0x00000000 4\n')
        self.table.with_suffix('.bindings').write_text('pointer widget 0x00000000 style\n')
        symbols = self.config / 'symbols.txt'
        symbols.write_text('0x00002000 style\n')
        self.source.write_text('#include "provider.h"\nstruct widget widget = {\n'
                               '#include "sample/provider/widget.inc"\n};\n')

        def declare(source, destination):
            header.write_text('struct style { unsigned count; };\n' + source + ';\n' +
                              'struct widget { ' + destination + '; };\n')

        for source, field in (('extern struct style style', 'const void *settings'),
                              ('extern struct style style', 'void *settings'),
                              ('extern const struct style style', 'const void *settings'),
                              ('extern volatile struct style style', 'const volatile void *settings')):
            with self.subTest(source=source, field=field):
                declare(source, field)
                for original in (struct.pack('<I', 0x2000), None):
                    generate(Image('cart'), self.source, Path('output'), original)
                    self.assertEqual((Path('output') / self.prefix / 'widget.inc').read_text().strip(), '&style')

        for source, field, spelled in (('extern const struct style style', 'void *settings', '(void *)&style'),
                                       ('extern volatile struct style style', 'const void *settings',
                                        '(const void *)&style'),
                                       ('extern struct style style', 'int *settings', '(int *)&style')):
            with self.subTest(source=source, field=field):
                declare(source, field)
                generate(Image('cart'), self.source, Path('output'), stub=True)
                self.assertEqual((Path('output') / self.prefix / 'widget.inc').read_text().strip(), spelled)
        for source, field, error in (('extern struct style style', 'void (*notify)(void)', 'as a function pointer'),
                                     ('extern void style(void)', 'const void *settings', 'needs a C function pointer'),
                                     ('extern struct style different', 'const void *settings',
                                      'no declaration gives style')):
            with self.subTest(source=source, field=field):
                declare(source, field)
                with self.assertRaisesRegex(SystemExit, error):
                    generate(Image('cart'), self.source, Path('output'), stub=True)
        declare('extern struct style style', 'const void *settings')
        with self.assertRaisesRegex(SystemExit, 'holds .*expected'):
            generate(Image('cart'), self.source, Path('output'), bytes(4))
        symbols.write_text('')
        with self.assertRaisesRegex(SystemExit, "neither the source nor the image's tables place style"):
            generate(Image('cart'), self.source, Path('output'), stub=True)

    def test_external_member_and_array_bindings_follow_every_declaration(self):
        header = self.source.with_suffix('.h')
        declaration = ('#define WIDTH 12\nstruct materials { char stone[8]; char glow[WIDTH]; };\n'
                       'extern const struct materials materials;\nextern const char skins[2][WIDTH];\n'
                       'extern const char skins[2][12];\n')
        header.write_text(declaration)
        self.source.write_text('#include "provider.h"\nvoid *textures[4] = {\n'
                               '#include "sample/provider/textures.inc"\n};\n')
        self.table.write_text('datum textures D 0x00000000 16\n')
        bindings = self.table.with_suffix('.bindings')
        rows = ('pointer textures 0x00000000 materials+0x8\npointer textures 0x00000004 skins+0xc\n'
                'pointer textures 0x00000008 skins+0xb\n')
        bindings.write_text(rows)
        symbols = self.config / 'symbols.txt'
        symbols.write_text('0x00002000 materials\n0x00003000 skins\n')
        output = Path('gen')
        for stub in (False, True):
            data = None if stub else struct.pack('<4I', 0x2008, 0x300c, 0x300b, 0)
            generate(Image('cart'), self.source, output, data, stub=stub)
            self.assertEqual((output / self.prefix / 'textures.inc').read_text().strip(),
                             '(void *)materials.glow, (void *)skins[1], (void *)&skins[0][11], 0')
        for target, invalid, message in (('materials+0x8', 'materials+0x14', 'the byte lies outside it'),
                                         ('skins+0xc', 'skins+0x18', 'the byte lies outside it')):
            with self.subTest(invalid=invalid):
                bindings.write_text(rows.replace(target, invalid))
                with self.assertRaisesRegex(SystemExit, message):
                    generate(Image('cart'), self.source, output, stub=True)
        bindings.write_text(rows)
        with self.assertRaisesRegex(SystemExit, 'holds .*expected'):
            generate(Image('cart'), self.source, output, bytes(16))
        for pair in (('[][WIDTH]', '[2][12]'), ('[2][12]', '[][WIDTH]'),
                     ('[2][WIDTH]', '[2][12]'), ('[2][12]', '[2][WIDTH]')):
            with self.subTest(declarations=pair):
                first, second = pair
                header.write_text(declaration[:declaration.index('extern const char skins')] +
                                  f'extern const char skins{first};\nextern const char skins{second};\n')
                generate(Image('cart'), self.source, output, stub=True)
        for first, second in (('[][11]', '[2][12]'), ('[2][12]', '[][11]')):
            with self.subTest(declarations=(first, second)):
                header.write_text(declaration[:declaration.index('extern const char skins')] +
                                  f'extern const char skins{first};\nextern const char skins{second};\n')
                with self.assertRaisesRegex(SystemExit, 'conflicting declarations of skins'):
                    generate(Image('cart'), self.source, output, stub=True)
        header.write_text('typedef const char SkinChar;\n' + declaration.replace('const char skins[2][12]',
                                                                                 'SkinChar skins[2][12]'))
        generate(Image('cart'), self.source, output, stub=True)
        for invalid, message in (('const unsigned char skins[2][12]', 'conflicting declarations of skins'),
                                 ('char skins[2][12]', 'conflicting declarations of skins'),
                                 ('const char skins[2][UNKNOWN]', 'skins: UNKNOWN: no integer constant')):
            with self.subTest(declaration=invalid):
                header.write_text(declaration.replace('const char skins[2][12]', invalid))
                with self.assertRaisesRegex(SystemExit, message):
                    generate(Image('cart'), self.source, output, stub=True)
        header.write_text(declaration.replace('skins[2][12]', 'skins[3][12]'))
        with self.assertRaisesRegex(SystemExit, 'conflicting declarations of skins'):
            generate(Image('cart'), self.source, output, stub=True)
        header.write_text(declaration)
        symbols.write_text('0x00003000 skins\n')
        with self.assertRaisesRegex(SystemExit, "neither the source nor the image's tables place materials"):
            generate(Image('cart'), self.source, output, stub=True)

    def test_address_of_a_single_record(self):
        header = self.source.with_suffix('.h')
        header.write_text(header.read_text().replace('char *tag;', 'struct label *tag;')
                          + 'struct label { char text[4]; };\n')
        self.source.write_text(self.source.read_text().replace('char tag[4]', 'struct label tag'))
        for original in (self.image, None):
            generate(Image('cart'), self.source, Path('output'), original)
            body = Path('output/sdk/sample/provider/callbacks.inc').read_text()
            self.assertEqual(body.strip(), '&tag, 32769, 255, start' if original else '&tag, 0, 0, start')
        with self.assertRaisesRegex(SystemExit, 'holds .*expected &tag'):
            generate(Image('cart'), self.source, Path('output'), bytes(4) + self.image[4:])

    def test_private_object_shadows_an_unrelated_public_symbol(self):
        symbols = self.config / 'symbols.txt'
        symbols.write_text(symbols.read_text() + '0x00003000 tag\n')
        output = Path('gen')
        generate(Image('cart'), self.source, output, self.image)
        body = (output / self.prefix / 'callbacks.inc').read_text().strip()
        self.assertEqual(body, 'tag, 32769, 255, start')
        with self.assertRaisesRegex(SystemExit, 'holds .*expected tag'):
            generate(Image('cart'), self.source, output, struct.pack('<I', 0x3000) + self.image[4:])

        # a public datum keeps its symbol's place, whatever a function's local shares its name
        public = self.source.read_text().replace('static char tag', 'char tag')
        for text in (public, public + 'void f(void)\n{\n    char tag;\n}\n'):
            self.source.write_text(text)
            with self.subTest(text=text), self.assertRaisesRegex(SystemExit, 'tag is placed at .*symbols.txt gives'):
                generate(Image('cart'), self.source, output, self.image)

    def test_pointer_to_an_element_of_a_character_array(self):
        bindings = self.table.with_suffix('.bindings')
        bindings.write_text(self.bindings.replace('00 tag', '00 tag+0x2'))
        data = struct.pack('<I', 0x100e) + self.image[4:]
        root = Path('build/gen')
        for original in (data, None):
            generate(Image('cart'), self.source, root, original)
            self.assertIn('&tag[2], ', (root / self.prefix / 'callbacks.inc').read_text())
        with self.assertRaisesRegex(SystemExit, 'holds .*expected'):
            generate(Image('cart'), self.source, root, self.image)
        bindings.write_text(self.bindings.replace('00 tag', '00 tag+0x4'))
        with self.assertRaisesRegex(SystemExit, r'tag\+0x4: the byte lies outside it'):
            generate(Image('cart'), self.source, root)

    def test_pointer_array_names_a_member_of_its_own_translation_unit(self):
        self.source.write_text('struct panel { char label[2]; short id; };\n'
                               'struct panel panels[2] = {\n#include "sample/provider/panels.inc"\n};\n'
                               'short *selection[2] = {\n#include "sample/provider/selection.inc"\n};\n')
        self.table.write_text('datum panels D 0x00000000 8\ndatum selection D 0x00000008 8\n')
        bindings = self.table.with_suffix('.bindings')
        bindings.write_text('pointer selection 0x00000000 panels+0x6\n')
        root = Path('output')
        for original in (struct.pack('<8xII', 0x1006, 0), None):
            generate(Image('cart'), self.source, root, original)
            self.assertEqual((root / self.prefix / 'selection.inc').read_text().strip(), '&panels[1].id, 0')
        for target, error in [('panels+0x8', 'the byte lies outside it'), ('panels+0x7', r'inside \[1\]\.id')]:
            with self.subTest(target=target):
                bindings.write_text(f'pointer selection 0x00000000 {target}\n')
                with self.assertRaisesRegex(SystemExit, error):
                    generate(Image('cart'), self.source, root)

        bindings.write_text('pointer selection 0x00000000 panels+0x6\n')
        self.table.unlink()
        generate(Image('cart'), self.source, root, stub=True)
        self.assertEqual((root / self.prefix / 'selection.inc').read_text().strip(), '&panels[1].id, 0')

    def test_generated_pointer_uses_the_compilers_pooled_text(self):
        self.source.write_text('#include "sample/provider/text.inc"\n'
                               'struct slot { void *filename; };\n'
                               'struct slot slots[2] = {\n#include "sample/provider/slots.inc"\n};\n')
        self.table.write_text('text CLOTH D 0x00000000 2\ndatum slots D 0x00000004 8\n')
        self.table.with_suffix('.bindings').write_text('text CLOTH 2\n'
                                                       'pointer slots 0x00000000 CLOTH\n'
                                                       'pointer slots 0x00000004 CLOTH\n')
        root = Path('output')
        original = b'A\0\0\0' + struct.pack('<II', 0x1000, 0x1000)
        generate(Image('cart'), self.source, root, original)
        header = root / self.prefix / 'text.inc'
        self.assertEqual(header.read_text().strip(), '#define CLOTH "A"')
        result = root / self.prefix / 'slots.inc'
        self.assertEqual(result.read_text().strip(), '{CLOTH}, {CLOTH}')
        with self.assertRaisesRegex(SystemExit, 'expected CLOTH'):
            generate(Image('cart'), self.source, root, original[:8] + struct.pack('<I', 0x1001))
        self.table.unlink()
        generate(Image('cart'), self.source, root, stub=True)
        self.assertEqual(header.read_text().strip(), '#define CLOTH "!"')
        self.assertEqual(result.read_text().strip(), '{CLOTH}, {CLOTH}')
        self.source.write_text(self.source.read_text().replace('void *filename', 'float *filename'))
        with self.assertRaisesRegex(SystemExit, 'character or void pointer'):
            generate(Image('cart'), self.source, root, stub=True)

    def test_wrapper_compiles_shared_bindings_using_its_providers_text_header(self):
        self.source.write_text('#include "provider_impl.c"\n')
        self.source.with_name('provider_impl.c').write_text(
            '#include "sample/provider_impl/text.inc"\n#include "slots.h"\n')
        self.source.with_name('slots.h').write_text(
            'struct slot { void *filename; };\nstatic struct slot slots[2] = {\n'
            '#include "sample/slots/slots.inc"\n};\n')
        self.table.write_text('text CLOTH D 0x00000000 2\ndatum slots D 0x00000004 8\n')
        self.table.with_suffix('.bindings').write_text('include slots\n')
        shared = Path('layouts/slots.bindings')
        shared.parent.mkdir()
        shared.write_text('include filenames\npointer slots 0x00000000 CLOTH\npointer slots 0x00000004 CLOTH\n')
        filenames = Path('layouts/filenames.bindings')
        filenames.write_text('text CLOTH 2\n')
        inputs = subprocess.check_output([sys.executable, Path(__file__).resolve().parents[1] / 'tools/body_data.py',
                                          '--inputs', self.source], text=True).split()
        self.assertTrue({str(shared), str(filenames), 'sample/provider_impl/text.inc',
                         'sample/slots/slots.inc'} <= set(inputs))
        output = Path('out')
        private = output / 'variants' / self.source.with_suffix('')
        original = b'A\0\0\0' + struct.pack('<II', 0x1000, 0x1000)
        for data, value in ((original, 'A'), (None, '!')):
            generate(Image('cart'), self.source, output, data, stub=data is None)
            self.assertEqual((private / 'sample/provider_impl/text.inc').read_text().strip(),
                             f'#define CLOTH "{value}"')
            self.assertEqual((private / 'sample/slots/slots.inc').read_text().strip(), '{CLOTH}, {CLOTH}')
        for contents, error in (('include slots', 'repeated binding include'),
                                ('include missing', 'missing.bindings:'),
                                ('include ../other', 'expected include NAME'),
                                ('text CLOTH 2\ntext CLOTH 2', 'repeated text')):
            with self.subTest(contents=contents):
                filenames.write_text(contents + '\n')
                with self.assertRaisesRegex(SystemExit, error):
                    body_bindings.read(self.source)

    def test_values_and_references_under_both_builds(self):
        for directory, original, expected in (('gen', self.image, 'tag, 32769, 255, start'),
                                               ('stub', None, 'tag, 0, 0, start')):
            root = Path('build', directory)
            generate(Image('cart'), self.source, root, original)
            self.assertEqual((root / self.prefix / 'callbacks.inc').read_text().strip(), expected)
            tag = (root / self.prefix / 'tag.inc').read_text().strip()
            self.assertEqual(tag, '97, 98, 99, 0' if original else '0, 0, 0, 0')

    def test_type_and_declarator_can_be_on_separate_lines(self):
        self.source.write_text(self.source.read_text().replace('struct callbacks callbacks = {',
                                                               'struct callbacks\n    callbacks = {'))
        root = Path('output')
        generate(Image('cart'), self.source, root, self.image)
        self.assertEqual((root / self.prefix / 'callbacks.inc').read_text().strip(), 'tag, 32769, 255, start')

    def test_a_body_takes_any_spelling_of_its_declaration(self):
        # a keep macro ahead of it, its storage class after its type, and a comment beside its #include
        self.source.write_text(self.source.read_text()
                               .replace('struct callbacks callbacks = {',
                                        '#ifdef NONMATCHING\n#define KEEP __attribute__((used))\n#else\n'
                                        '#define KEEP\n#endif\nKEEP struct callbacks callbacks = {')
                               .replace('static char tag[4] = {', 'char static tag[4] = {')
                               .replace('tag.inc"', 'tag.inc" /* its letters */'))
        root = Path('output')
        generate(Image('cart'), self.source, root, self.image)
        self.assertEqual([(root / self.prefix / name).read_text().strip() for name in ('callbacks.inc', 'tag.inc')],
                         ['tag, 32769, 255, start', '97, 98, 99, 0'])

    def test_each_include_the_build_generates_is_text_inc_or_a_body_shc_compiles(self):
        # SHC's tables measure its own side of a guard alone
        for stray in ('#include "sample/provider/stray.inc"\n',
                      '#ifdef NONMATCHING\nint stray[1] = {\n#include "sample/provider/stray.inc"\n};\n#endif\n'):
            with self.subTest(stray=stray):
                self.source.write_text(self.source.read_text() + stray)
                with self.assertRaisesRegex(SystemExit, 'stray.inc is the initializer of no declaration SHC compiles'):
                    generate(Image('cart'), self.source, Path('output'), self.image)
                self.source.write_text(self.source.read_text().replace(stray, ''))

    def test_block_scope_preserves_the_declared_type(self):
        self.source.write_text(self.source.read_text()
                               .replace('struct callbacks callbacks = {',
                                        'void draw(void)\n{\n    struct callbacks callbacks = {')
                               .replace('};\nstatic char tag[4] = {', '    };\n}\nstatic char tag[4] = {'))
        (self.config / 'symbols.txt').write_text('0x00002000 start\n')
        root = Path('build/gen')
        generate(Image('cart'), self.source, root, self.image)
        self.assertEqual((root / self.prefix / 'callbacks.inc').read_text().strip(), 'tag, 32769, 255, start')

    def test_variant_reads_its_own_bytes_through_nested_c_includes(self):
        variant = self.source.with_name('variant.c')
        variant.write_text('#include "nested.c"\n')
        self.source.with_name('nested.c').write_text('#include "provider.c"\n')
        self.table.with_name('variant.txt').write_text(self.offsets)
        self.table.with_name('variant.bindings').write_text(self.bindings)
        root = Path('build/gen')
        generate(Image('cart'), self.source, root, self.image)
        split = self.config / 'splits.txt'
        split.write_text(split.read_text().replace(str(self.source), str(variant)))
        generate(Image('cart'), variant, root, self.image[:-4] + b'xyz\0')
        generated = root / 'variants/sdk/sample/variant/sample/provider/tag.inc'
        self.assertEqual(generated.read_text().strip(), '120, 121, 122, 0')
        self.assertEqual((root / self.prefix / 'tag.inc').read_text().strip(), '97, 98, 99, 0')
        generate(Image('cart'), variant, Path('build/stub'))
        self.assertEqual((Path('build/stub') / generated.relative_to(root)).read_text().strip(), '0, 0, 0, 0')

    def test_rejects_misplaced_bytes_incomplete_original_and_unmapped_pointers(self):
        offsets, bindings = self.offsets, self.bindings
        cases = [
            (offsets, bindings, self.image[:-1], 'original image ends'),
            (offsets, bindings, self.image[:7] + b'x' + self.image[8:], 'nonzero padding'),
            (offsets, bindings, self.image[:8] + bytes(4) + self.image[12:], 'expected start'),
            (offsets, bindings.replace('00 tag', '00 start'), self.image, 'needs a C function pointer for start'),
            (offsets, bindings.replace('00 tag', '00 &tag'), self.image,
             'tools/relocate.py writes for a target spelled as C'),
            (offsets, bindings.replace('00 tag', '00 tag 0x00000001'), self.image, 'tools/relocate.py writes'),
            (offsets, 'pointer callbacks\n', self.image, 'expected pointer NAME OFFSET TARGET'),
            (offsets, bindings.replace('start', 'missing'), self.image, "tables place missing"),
            (offsets, bindings.replace('pointer callbacks 0x00000008 start\n', ''), self.image,
             'callbacks[+]0x00000008 holds 0x00002000, not null: its row is pointer  callbacks  0x00000008  start'),
            (offsets, bindings + 'pointer callbacks 0x00000004 tag\n', self.image, 'no initialized C pointer'),
            (offsets + 'pointer callbacks 0x00000000 tag\n', bindings, self.image,
             'expected datum, text or entry NAME SECTION OFFSET SIZE'),
            (offsets.replace('0x0000000c', '0x0000000b'), bindings, self.image, 'overlaps'),
            (offsets.replace('0x0000000c', '0x00000010'), bindings, self.image, 'beyond its D placement'),
            (offsets.replace('12\n', '10\n'), bindings, self.image, 'disagrees with its C type'),
            (offsets.replace('datum tag ', 'datum label '), bindings, self.image,
             'label: the table gives an initialized datum, the source wants none; '
             'tag: the table gives none, the source wants an initialized datum'),
        ]
        for table, rows, data, error in cases:
            with self.subTest(error):
                self.table.write_text(table)
                self.table.with_suffix('.bindings').write_text(rows)
                with self.assertRaisesRegex(SystemExit, error):
                    generate(Image('cart'), self.source, Path('output'), data)
                self.assertFalse(Path('output').exists())

    def literal_source(self):
        self.source.write_text('#include "sample/provider/text.inc"\n'
                               'char *labels[] = { FIRST, SECOND, QUOTED };\n')
        self.table.write_text('text FIRST D 0x00000000 2\n'
                              'text SECOND D 0x00000002 2\n'
                              'text QUOTED D 0x00000004 7\n')
        self.table.with_suffix('.bindings').write_text('')
        return b'A\0B\0\x017?"\\\n\0'

    def generated_inputs(self, source):
        tool = Path(__file__).resolve().parents[1] / 'tools/body_data.py'
        scan = subprocess.check_output([sys.executable, tool, '--inputs', source], text=True)
        return [path for path in scan.split() if path.endswith('.inc')]

    def test_text_only_provider_lists_its_generated_header(self):
        self.literal_source()
        self.assertEqual(self.generated_inputs(self.source), ['sample/provider/text.inc'])

    def test_nested_variant_keeps_text_and_typed_fields_together(self):
        self.source.write_text('#include "sample/provider/text.inc"\n'
                               'char *labels[] = { FIRST, SECOND };\n'
                               'struct panel { void (*draw)(void); float scale; };\nvoid start(void);\n'
                               'struct panel panel = {\n#include "sample/provider/panel.inc"\n};\n')
        nested = self.source.with_name('nested.c')
        nested.write_text('#include "provider.c"\n')
        variant = self.source.with_name('variant.c')
        variant.write_text('#include "nested.c"\n')
        offsets = 'datum panel D 0x00000000 8\ntext FIRST D 0x00000008 2\ntext SECOND D 0x0000000a 2\n'
        for name in (self.source, variant):
            self.table.with_name(name.stem + '.txt').write_text(offsets)
            self.table.with_name(name.stem + '.bindings').write_text('pointer panel 0x00000000 start\n')
            self.assertCountEqual(self.generated_inputs(name), ['sample/provider/panel.inc', 'sample/provider/text.inc'])
        direct = Path('output') / self.prefix
        private = Path('output/variants/sdk/sample/variant/sample/provider')
        split = self.config / 'splits.txt'
        generate(Image('cart'), self.source, Path('output'), struct.pack('<If', 0x2000, -0.0) + b'A\0B\0')
        split.write_text(split.read_text().replace(str(self.source), str(variant)))
        generate(Image('cart'), variant, Path('output'), struct.pack('<If', 0x2000, 1.5) + b'X\0Y\0')
        self.assertEqual((direct / 'panel.inc').read_text().strip(), 'start, -0.0F')
        self.assertEqual((private / 'panel.inc').read_text().strip(), 'start, 1.5F')
        self.assertEqual((direct / 'text.inc').read_text().splitlines(), ['#define FIRST "A"', '#define SECOND "B"'])
        self.assertEqual((private / 'text.inc').read_text().splitlines(), ['#define FIRST "X"', '#define SECOND "Y"'])
        generate(Image('cart'), variant, Path('stub'))
        stub = Path('stub') / private.relative_to('output')
        self.assertEqual((stub / 'panel.inc').read_text().strip(), 'start, 0.0F')
        self.assertEqual((stub / 'text.inc').read_text().splitlines(), ['#define FIRST "!"', r'#define SECOND "\""'])

    def test_literal_escapes_and_distinct_width_preserving_stubs(self):
        data = self.literal_source()
        root = Path('output')
        header = root / self.prefix / 'text.inc'
        generate(Image('cart'), self.source, root, data)
        self.assertEqual(header.read_text().splitlines(), [
            '#define FIRST "A"', '#define SECOND "B"', r'#define QUOTED "\0017\?\"\\\n"'])
        header.unlink()
        generate(Image('cart'), self.source, root)
        self.assertEqual(header.read_text().splitlines(), [
            '#define FIRST "!"', r'#define SECOND "\""', '#define QUOTED "!!!!!!"'])

    def test_literal_table_requires_complete_strings_and_a_real_macro_use(self):
        for problem in ('short', 'unterminated', 'embedded', 'unused', 'header'):
            with self.subTest(problem):
                data = self.literal_source()
                if problem == 'short':
                    data = data[:-1]
                elif problem == 'unterminated':
                    data = data[:-1] + b'!'
                elif problem == 'embedded':
                    data = data[:5] + b'\0' + data[6:]
                elif problem == 'unused':
                    self.source.write_text(self.source.read_text().replace('QUOTED', '"QUOTED" /* QUOTED */'))
                else:
                    self.source.write_text(self.source.read_text().replace('#include "sample/provider/text.inc"\n',
                                                                           ''))
                with self.assertRaisesRegex(SystemExit, 'string length|no use of text macro|text rows'):
                    generate(Image('cart'), self.source, Path('output'), data)
                self.assertFalse(Path('output').exists())

    def test_long_arrays_stay_within_the_compilers_line_limit(self):
        self.source.write_text('typedef unsigned char Uint8;\nUint8 payload[1024] = {\n'
                               '#include "sample/provider/payload.inc"\n};\n')
        self.table.write_text('datum payload D 0x00000000 1024\n')
        self.table.with_suffix('.bindings').write_text('')
        self.config.joinpath('splits.txt').write_text(
            'file TRACK.BIN\nD 0x00001000 0x00001400\n'
            'sdk/sample/provider.c D 0x00001000 0x00001400\n')
        raw = bytes(range(256)) * 4
        generate(Image('cart'), self.source, Path('out'), raw)
        text = (Path('out') / self.prefix / 'payload.inc').read_text()
        self.assertLessEqual(max(map(len, text.splitlines())), 96)
        self.assertEqual(bytes(int(word) for word in text.split(',')), raw)

    def test_lint_copy_holds_each_body_on_its_includes_line(self):
        # a two-line body, an include whose comment runs onto a second line, and text.inc, which stays an include
        self.source.write_text('#include "provider.h"\n#include "sample/provider/text.inc"\n'
                               'struct callbacks callbacks = {\n'
                               '#include "sample/provider/callbacks.inc" /* its\n    fields */\n'
                               '};\nint count;\n')
        root = Path('stub/sdk')
        (root / 'sample/provider').mkdir(parents=True)
        (root / 'sample/provider/callbacks.inc').write_text('    TAG, 0,\n    0, start\n')
        (root / 'sample/provider/text.inc').write_text('#define TAG "!!!"\n')
        self.assertEqual(with_bodies(self.source, root).decode().split('\n'),
                         ['#include "provider.h"', '#include "sample/provider/text.inc"',
                          'struct callbacks callbacks = {', 'TAG, 0, 0, start /* NOLINT(misc-include-cleaner) */',
                          '', '};', 'int count;', ''])
        # a datum named text has a text.inc of its own, its body
        self.source.write_text('char text[4] = {\n#include "sample/provider/text.inc"\n};\nint count;\n')
        (root / 'sample/provider/text.inc').write_text('    97, 98,\n    99, 0\n')
        self.assertEqual(with_bodies(self.source, root).decode().split('\n'),
                         ['char text[4] = {', '97, 98, 99, 0 /* NOLINT(misc-include-cleaner) */', '};', 'int count;',
                          ''])

    def chunk_model(self, polygons, vertices, extra=''):
        """A source of a chunk model whose plist and vlist reach flat arrays of polygons and vertices, from 0x1000
        on, and its image."""
        self.source.write_text(
            'typedef long Sint32;\ntypedef short Sint16;\n'
            'typedef struct { Sint32 *vlist; Sint16 *plist; float center[3]; float r; } NJS_CNK_MODEL;\n'
            f'static Sint16 polygons[{len(polygons)}] = {{\n#include "sample/provider/polygons.inc"\n}};\n'
            f'static Sint32 vertices[{len(vertices)}] = {{\n#include "sample/provider/vertices.inc"\n}};\n'
            'static NJS_CNK_MODEL model = {\n#include "sample/provider/model.inc"\n};\n' + extra)
        at = -(-2 * len(polygons) // 4) * 4
        end = at + 4 * len(vertices)
        self.table.write_text(f'datum polygons D 0x00000000 {2 * len(polygons)}\n'
                              f'datum vertices D {at:#010x} {4 * len(vertices)}\ndatum model D {end:#010x} 24\n')
        self.table.with_suffix('.bindings').write_text('pointer model 0x00000000 vertices\n'
                                                       'pointer model 0x00000004 polygons\n')
        self.config.joinpath('splits.txt').write_text(
            f'file TRACK.BIN\nD 0x00001000 {0x1000 + end + 24:#010x}\n'
            f'sdk/sample/provider.c D 0x00001000 {0x1000 + end + 24:#010x}\n')
        return (struct.pack(f'<{len(polygons)}H', *polygons).ljust(at, b'\0')
                + struct.pack(f'<{len(vertices)}I', *vertices) + struct.pack('<II16x', 0x1000 + at, 0x1000))

    def test_a_flat_chunk_list_is_written_a_chunk_to_a_row(self):
        # a bits chunk, a material, a texture, a strip chunk of one strip and its padding, and a strip chunk of a
        # type SA2's Ninja doesn't draw, which is one row
        polygons = [0x0801, 17, 2, 0x1234, 0x5678, 8, 5, 0x0141, 12, 1, 3, 0, 10, 20, 1, 30, 40, 2, 50, 60, 0,
                    67, 2, 1, 3, 255]
        # an NJD_CV_D8 chunk of two vertices, and the end
        vertices = [(9 << 16) | 35, (2 << 16) | 4, 1, 2, 3, 4, 5, 6, 7, 8, 255]
        original = self.chunk_model(polygons, vertices)
        root = Path('out')
        generate(Image('cart'), self.source, root, original)
        self.assertEqual((root / self.prefix / 'polygons.inc').read_text(),
                         '    2049,\n    17, 2, 4660, 22136,\n    8, 5,\n    321, 12, 1,\n'
                         '    3, 0, 10, 20, 1, 30, 40, 2, 50, 60,\n    0,\n    67, 2, 1, 3,\n    255\n')
        self.assertEqual((root / self.prefix / 'vertices.inc').read_text(),
                         '    589859, 131076,\n    1, 2, 3, 4,\n    5, 6, 7, 8,\n    255\n')
        # the build without the disc reads no chunk, and writes each list as any array
        generate(Image('cart'), self.source, root, stub=True)
        self.assertEqual((root / self.prefix / 'polygons.inc').read_text(), '    ' + ', '.join(['0'] * 26) + '\n')

    def test_a_row_longer_than_the_line_goes_on_four_columns_further_in(self):
        strip = [99, *range(1000, 1099)]
        original = self.chunk_model([64, 1 + len(strip), 1, *strip, 255], [255])
        generate(Image('cart'), self.source, Path('out'), original)
        lines = (Path('out') / self.prefix / 'polygons.inc').read_text().splitlines()
        self.assertEqual(lines[0], '    64, 101, 1,')
        self.assertTrue(all(len(line) <= 96 and line.startswith('        ') for line in lines[2:-1]))
        self.assertEqual(lines[-1], '    255')
        self.assertEqual(' '.join(lines[1:-1]).split(), [f'{value},' for value in strip])

    def test_a_flat_chunk_list_must_walk_to_its_end(self):
        for polygons, vertices, problem in (([8, 5, 255, 0], [255], 'polygons, a chunk_polygons list: NJD_CE at byte 4'),
                                            ([255], [34, 255, 0], 'vertices, a chunk_vertices list: NJD_CE at byte 4'),
                                            ([40, 0, 255], [255], 'vertex chunk type 40 at byte 0'),
                                            ([255], [(2 << 16) | 34, 3 << 16, 7, 255],
                                             'holds 12 bytes, where its vertex count, 3, needs 44')):
            original = self.chunk_model(polygons, vertices)
            with self.subTest(polygons=polygons, vertices=vertices), self.assertRaisesRegex(SystemExit, problem):
                generate(Image('cart'), self.source, Path('out'), original)

    def test_a_flat_array_takes_one_kind_of_list_and_its_elements(self):
        self.chunk_model([255], [255], 'static NJS_CNK_MODEL other = {\n#include "sample/provider/other.inc"\n};\n')
        bindings = self.table.with_suffix('.bindings')
        bindings.write_text(bindings.read_text() + 'pointer other 0x00000000 vertices\n'
                            'pointer other 0x00000004 vertices\n')
        with self.assertRaisesRegex(SystemExit, 'vertices is reached as chunk_vertices and as chunk_polygons'):
            generate(Image('cart'), self.source, Path('out'), stub=True)
        self.chunk_model([255], [255])
        self.source.write_text(self.source.read_text().replace('static Sint16 polygons', 'static float polygons'))
        with self.assertRaisesRegex(SystemExit, 'polygons, a chunk_polygons list, needs Sint16 elements'):
            generate(Image('cart'), self.source, Path('out'), stub=True)

    def test_float_fields_take_their_shortest_literals_and_keep_signed_zero(self):
        self.source.write_text('float samples[6] = {\n'
                               '#include "sample/provider/samples.inc"\n};\n')
        self.table.write_text('datum samples D 0x00000000 24\n')
        self.table.with_suffix('.bindings').write_text('')
        self.config.joinpath('splits.txt').write_text(
            'file TRACK.BIN\nD 0x00001000 0x00001018\n'
            'sdk/sample/provider.c D 0x00001000 0x00001018\n')
        words = (0, 0x80000000, 0x00800000, 0x3dcccccd, 0xbf800000, 0x7f7fffff)
        raw = struct.pack('<6I', *words)
        root = Path('out')
        generate(Image('cart'), self.source, root, raw)
        self.assertEqual((root / self.prefix / 'samples.inc').read_text(),
                         '    0.0F, -0.0F, 1.1754944e-38F, 0.1F, -1.0F, 3.4028235e+38F\n')
        for word in (1, 0x807fffff, 0x7f800000, 0x7fc00040):
            with self.subTest(word=word), self.assertRaisesRegex(SystemExit, f'{word:#010x}'):
                generate(Image('cart'), self.source, root, struct.pack('<I', word) + raw[4:])

    # a char array that two numbered texts initialize, then a struct of char arrays that two more do, one each
    banked = ('#include "sample/provider/text.inc"\n'
              'struct names { char first[8]; char second[4]; };\n'
              'char bank[14] = TEXT_1 TEXT_2;\n'
              'struct names names = {TEXT_3, TEXT_4};\n')
    bank = b'ab\0\0cdefghi\0\0\0' + b'xy\0\0\0\0\0\0z\0\0\0'

    def banked_source(self, source=banked):
        self.source.write_text(source)
        self.table.write_text('datum bank D 0x00000000 14\ndatum names D 0x0000000e 12\n')
        self.table.with_suffix('.bindings').write_text('')
        self.config.joinpath('splits.txt').write_text(
            'file BANK.BIN\nD 0x00001000 0x0000101a\nsdk/sample/provider.c D 0x00001000 0x0000101a\n')

    def test_numbered_texts_spell_a_datums_strings_from_the_disc(self):
        self.banked_source()
        root = Path('out')
        generate(Image('cart'), self.source, root, self.bank)
        # an array's text but its last spells the zeros up to the next string, and a member's its string alone
        self.assertEqual((root / self.prefix / 'text.inc').read_text().splitlines(), [
            r'#define TEXT_1 "ab\000\000"', '#define TEXT_2 "cdefghi"', '#define TEXT_3 "xy"', '#define TEXT_4 "z"'])
        self.assertEqual(sorted(path.name for path in (root / self.prefix).iterdir()), ['text.inc'])
        generate(Image('cart'), self.source, root, stub=True)
        self.assertEqual((root / self.prefix / 'text.inc').read_text().splitlines(),
                         [f'#define TEXT_{number} ""' for number in range(1, 5)])

    def test_numbered_texts_come_in_order_one_datum_each(self):
        for problem, old, new in (
                ('TEXT_2 comes where TEXT_1 should', 'TEXT_1 TEXT_2', 'TEXT_2 TEXT_1'),
                ('TEXT_5 lies in the declaration of no initialized datum at file scope', 'TEXT_4};\n',
                 'TEXT_4};\nvoid f(void) { f(TEXT_5); }\n'),
                ('TEXT_1 lies on a line that bank and more share', '= TEXT_1 TEXT_2', '= TEXT_1, more[4] = TEXT_2'),
                ('need its include of sample/provider/text.inc', '#include "sample/provider/text.inc"\n', ''),
                ('a struct of char arrays one each', 'TEXT_3, TEXT_4', 'TEXT_3'),
                ('a struct of char arrays one each', 'char second', 'unsigned char second')):
            with self.subTest(problem):
                self.banked_source(self.banked.replace(old, new))
                with self.assertRaisesRegex(SystemExit, re.escape(problem)):
                    generate(Image('cart'), self.source, Path('out'), self.bank)

    def test_a_numbered_text_is_no_text_row_and_holds_no_pointer(self):
        self.banked_source()
        bindings = self.table.with_suffix('.bindings')
        bindings.write_text('text TEXT_1 2\n')
        with self.assertRaisesRegex(SystemExit, 'TEXT_1 is both a text row and a numbered text'):
            generate(Image('cart'), self.source, Path('out'), stub=True)
        self.banked_source(self.banked + 'char *label = FIRST;\n')
        bindings.write_text('text FIRST 2\npointer bank 0x00000000 FIRST\n')
        with self.assertRaisesRegex(SystemExit, r'bank\+0x00000000 has no initialized C pointer'):
            generate(Image('cart'), self.source, Path('out'), stub=True)

    def test_numbered_texts_take_the_disc_strings_their_datum_holds(self):
        for problem, at, put in (('a byte past TEXT_2, the last text of its array', 13, b'k'),
                                 ('a byte past TEXT_3, the last text of its array', 17, b'w'),
                                 ('no string is left in the array for TEXT_2', 4, bytes(7)),
                                 ('no nonempty, null-terminated string at offset 0x00000000 for TEXT_3', 14, b'\0'),
                                 ('no nonempty, null-terminated string at offset 0x00000008 for TEXT_4', 22, b'zzzz')):
            with self.subTest(problem):
                self.banked_source()
                with self.assertRaisesRegex(SystemExit, re.escape(problem)):
                    generate(Image('cart'), self.source, Path('out'), self.bank[:at] + put + self.bank[at + len(put):])


class Resolver(unittest.TestCase):
    """A pointer row is a relocation: the name its field points at and how
    many bytes past that name's start. Each test spells one row as C, by its
    field's type and the declarations in scope."""
    source = Path('src/rows.c')
    table = Path('offsets/src/rows.txt')
    body = Path('stub/src/rows/refs.inc')
    config = Path('config/cart')
    # the types the rows point into. struct mixed holds padding between tag and count
    declarations = ('struct pair { short first, second; };\n'
                    'struct mixed { char tag; int count; };\n'
                    'union cell { short halves[2]; int word; };\n'
                    'struct frame { int id; char name[4]; };\n'
                    'extern char first[4], next[4];\n'
                    'extern struct mixed mixed;\n'
                    'extern union cell cell;\n'
                    'extern short rows[3][2];\n'
                    'extern struct frame frames[];\n'
                    'void tick(void);\n')
    symbols = {0x2000: 'first', 0x2004: 'next', 0x2010: 'mixed', 0x2020: 'cell', 0x2040: 'rows', 0x2100: 'frames',
               0x3000: 'tick'}

    def setUp(self):
        temp = tempfile.TemporaryDirectory()
        self.addCleanup(temp.cleanup)
        self.addCleanup(os.chdir, Path.cwd())
        os.chdir(temp.name)
        for parent in (self.source.parent, self.table.parent, self.config):
            parent.mkdir(parents=True)
        (self.config / 'splits.txt').write_text('file CART.BIN\nD 0x00001000 0x00001008\n'
                                                'src/rows.c D 0x00001000 0x00001008\n')
        self.name()

    def name(self, *more):
        """Write the image's names: symbols, then more, each (address, name)."""
        rows = sorted([*self.symbols.items(), *more])
        (self.config / 'symbols.txt').write_text(''.join(f'{at:#010x} {name}\n' for at, name in rows))

    def spelled(self, field, row, *, more='', original=None):
        """The stub's C for refs, a struct of field, with its pointer at offset 0 bound by row. With original, the
        bytes the full build reads at refs, the full build's C."""
        self.source.write_text(self.declarations + more + f'struct refs {{ {field}; int spare; }};\n'
                               'struct refs refs = {\n#include "rows/refs.inc"\n};\n')
        self.table.with_suffix('.bindings').write_text(f'pointer refs 0x00000000 {row}\n' if row else '')
        self.table.write_text('datum refs D 0x00000000 8\n')
        generate(Image('cart'), self.source, Path('stub'), original, stub=original is None)
        text = self.body.read_text().strip()
        return text[:-len(', 0')] if text.endswith(', 0') else text

    def test_a_word_at_one_datums_end_names_the_next(self):
        self.assertEqual(self.spelled('char *name', 'next', original=struct.pack('<2I', 0x2004, 0)), 'next')
        with self.assertRaisesRegex(SystemExit, r'first\+0x4: the byte lies outside it'):
            self.spelled('char *name', 'first+0x4')

    def test_two_names_at_one_address_each_spell_their_own(self):
        self.name((0x2010, 'mixed_tag'))
        more = 'extern char mixed_tag;\n'
        for row, spelled in (('mixed', '&mixed'), ('mixed_tag', '&mixed_tag')):
            with self.subTest(row=row):
                self.assertEqual(self.spelled('void *at', row, more=more, original=struct.pack('<2I', 0x2010, 0)),
                                 spelled)

    def test_the_outermost_subobject_that_converts_is_spelled(self):
        # a union offers each member in turn, and an array is its name
        for field, row, spelled in (('short *half', 'cell', 'cell.halves'), ('int *word', 'cell', '&cell.word'),
                                    ('void *any', 'cell', '&cell'), ('short *half', 'cell+0x2', '&cell.halves[1]'),
                                    ('int *count', 'mixed+0x4', '&mixed.count'), ('short *row', 'rows+0x4', 'rows[1]'),
                                    ('short *cell', 'rows+0x6', '&rows[1][1]')):
            with self.subTest(field=field, row=row):
                self.assertEqual(self.spelled(field, row), spelled)
        # a const target converts to a pointer to const
        self.name((0x2030, 'pair'))
        self.assertEqual(self.spelled('const short *half', 'pair+0x2', more='extern const struct pair pair;\n'),
                         '&pair.second')

    def test_the_object_alone_is_read_at_its_start(self):
        # at its start, an object's own type decides, so its layout and its extents are never needed
        more = 'struct bits { int low : 3; };\nextern struct bits flags;\nextern char sized[sizeof(int)];\n'
        self.name((0x2300, 'flags'), (0x2310, 'sized'))
        for field, row, spelled in (('void *any', 'flags', '&flags'), ('char *name', 'sized', 'sized')):
            with self.subTest(row=row):
                self.assertEqual(self.spelled(field, row, more=more), spelled)

    def test_types_compare_as_c_compares_them(self):
        # a long double is no double, and a pointer to a function pointer converts where the parameters agree
        more = ('extern double narrow;\nextern long double wide;\ntypedef void (*Hook)(int);\n'
                'extern void (*hook)(void);\nextern void (*other)(int);\n')
        self.name((0x2300, 'narrow'), (0x2308, 'wide'), (0x2310, 'hook'), (0x2314, 'other'))
        for field, row, spelled in (('double *at', 'narrow', '&narrow'), ('double *at', 'wide', '(double *)&wide'),
                                    ('Hook *slot', 'other', '&other'), ('Hook *slot', 'hook', '(Hook *)&hook')):
            with self.subTest(row=row):
                self.assertEqual(self.spelled(field, row, more=more), spelled)
        # of a function's declarations, one with no prototype agrees with one whose parameters don't promote
        for declaration, conflicts in (('void tick();\n', False), ('void tick(int);\n', True),
                                       ('void tick(void) { }\n', False), ('int tick(void);\n', True)):
            with self.subTest(declaration=declaration):
                if conflicts:
                    with self.assertRaisesRegex(SystemExit, 'conflicting declarations of tick'):
                        self.spelled('void (*step)(void)', 'tick', more=declaration)
                else:
                    self.assertEqual(self.spelled('void (*step)(void)', 'tick', more=declaration), 'tick')
        for declarations, conflicts in (('void step(int);\nvoid step();\n', False),
                                        ('void step(char);\nvoid step();\n', True),
                                        ('void step(const char *, ...);\nvoid step();\n', True)):
            with self.subTest(declarations=declarations):
                self.name((0x3010, 'step'))
                if conflicts:
                    with self.assertRaisesRegex(SystemExit, 'conflicting declarations of step'):
                        self.spelled('void (*step)(int)', 'step', more=declarations)
                else:
                    self.assertEqual(self.spelled('void (*step)(int)', 'step', more=declarations), 'step')

    def test_each_pair_of_declarations_must_agree(self):
        # an array without an extent agrees with one of any extent, but [2] and [3] disagree, in any order
        self.name((0x2300, 'trio'))
        extents = ('[]', '[2]', '[3]')
        for order in itertools.permutations(extents):
            with self.subTest(order=order), self.assertRaisesRegex(SystemExit, 'conflicting declarations of trio'):
                self.spelled('int *at', 'trio', more=''.join(f'extern int trio{each};\n' for each in order))
        # of [] and [2], the extent decides, so a byte past it lies outside
        for order in (('[]', '[2]'), ('[2]', '[]')):
            with self.subTest(order=order), self.assertRaisesRegex(SystemExit, 'the byte lies outside it'):
                self.spelled('int *at', 'trio+0x8', more=''.join(f'extern int trio{each};\n' for each in order))

    def test_a_byte_no_subobject_starts_at_fails(self):
        for row, error in (('mixed+0x1', 'the byte lies in padding of the object'),
                           ('mixed+0x5', 'the byte lies inside .count'), ('cell+0x1', 'inside .halves[0]')):
            with self.subTest(row=row), self.assertRaisesRegex(SystemExit, re.escape(error)):
                self.spelled('void *at', row)

    def test_where_none_converts_the_outermost_is_cast(self):
        more = 'extern const struct pair pair;\n'
        self.name((0x2030, 'pair'))
        for field, row, spelled in (('short *half', 'mixed', '(short *)&mixed'),
                                    ('void *any', 'pair', '(void *)&pair'),
                                    ('short *half', 'next', '(short *)next')):
            with self.subTest(field=field, row=row):
                self.assertEqual(self.spelled(field, row, more=more), spelled)

    def test_a_pointer_to_an_array_takes_the_arrays_address(self):
        for row, spelled in (('rows', 'rows'), ('rows+0x4', '&rows[1]'), ('next', '(short (*)[2])next')):
            with self.subTest(row=row):
                self.assertEqual(self.spelled('short (*row)[2]', row), spelled)

    def test_an_array_without_an_extent_takes_an_element_where_no_name_lies_between(self):
        self.assertEqual(self.spelled('char *name', 'frames+0xc', original=struct.pack('<2I', 0x210c, 0)),
                         'frames[1].name')
        # an array that a typedef gives no extent
        self.name((0x2200, 'listed'))
        self.assertEqual(self.spelled('char *name', 'listed+0xc', more='typedef struct frame Frames[];\n'
                                      'extern Frames listed;\n'), 'listed[1].name')
        self.name((0x2108, 'second_frame'))
        with self.assertRaisesRegex(SystemExit, r'frames\+0xc lies at or past second_frame'):
            self.spelled('char *name', 'frames+0xc')

    def test_a_char_arrays_byte(self):
        for field in ('char *letter', 'void *letter'):
            with self.subTest(field=field):
                self.assertEqual(self.spelled(field, 'next+0x2', original=struct.pack('<2I', 0x2006, 0)), '&next[2]')

    def test_a_function_initializes_a_function_pointer_alone(self):
        self.assertEqual(self.spelled('void (*step)(void)', 'tick', original=struct.pack('<2I', 0x3000, 0)), 'tick')
        for field, row, error in (('void *step', 'tick', 'needs a C function pointer for tick'),
                                  ('void (*step)(int)', 'tick', "tick's type isn't the function type that refs.step "
                                                                'points to'),
                                  ('int (*step)(void)', 'tick', "tick's type isn't the function type"),
                                  ('void (*step)(void)', 'tick+0x2', "points past a function's entry"),
                                  ('void (*step)(void)', 'next', 'cannot use object next as a function pointer')):
            with self.subTest(field=field, row=row), self.assertRaisesRegex(SystemExit, re.escape(error)):
                self.spelled(field, row)

    def test_a_text_initializes_a_char_or_void_pointer_at_its_start(self):
        more = '#include "rows/text.inc"\n'
        self.table.with_suffix('.bindings').write_text('text LABEL 4\n')
        (self.config / 'splits.txt').write_text('file CART.BIN\nD 0x00001000 0x0000100c\n'
                                                'src/rows.c D 0x00001000 0x0000100c\n')

        def spelled(field, row):
            self.source.write_text(more + self.declarations + f'struct refs {{ {field}; int spare; }};\n'
                                   'struct refs refs = {\n#include "rows/refs.inc"\n};\n')
            self.table.with_suffix('.bindings').write_text(f'text LABEL 4\npointer refs 0x00000000 {row}\n')
            generate(Image('cart'), self.source, Path('stub'), stub=True)
            return self.body.read_text().strip()

        for field in ('char *label', 'const void *label'):
            with self.subTest(field=field):
                self.assertEqual(spelled(field, 'LABEL'), 'LABEL, 0')
        for field, row in (('short *label', 'LABEL'), ('char *label', 'LABEL+0x1')):
            with self.subTest(field=field, row=row), self.assertRaisesRegex(SystemExit, 'needs a character or void'):
                spelled(field, row)

    def test_a_word_no_row_names_asks_for_its_row(self):
        # the source's own datum that holds the word first, then the first name the tables give at it
        self.name((0x2004, 'next_alias'))
        for word, row in ((0x2004, 'next'), (0x1004, 'refs+0x4')):
            with self.subTest(row=row):
                original = struct.pack('<2I', word, 0)
                with self.assertRaisesRegex(SystemExit, re.escape(
                        f'offsets/src/rows.bindings: refs+0x00000000 holds {word:#010x}, not null: its row is pointer  '
                        f'refs  0x00000000  {row}')):
                    self.spelled('char *name', None, original=original)
                self.spelled('char *name', row, original=original)
        with self.assertRaisesRegex(SystemExit, re.escape('holds 0x00002004, expected first: its row is pointer  refs  '
                                                          '0x00000000  next')):
            self.spelled('char *name', 'first', original=struct.pack('<2I', 0x2004, 0))
        with self.assertRaisesRegex(SystemExit, 'holds 0x00002006, not null, which no table names$'):
            self.spelled('char *name', None, original=struct.pack('<2I', 0x2006, 0))
        with self.assertRaisesRegex(SystemExit, 'holds null, expected next: a null field takes no row$'):
            self.spelled('char *name', 'next', original=struct.pack('<2I', 0, 0))

    def test_an_integer_that_holds_a_named_address_fails(self):
        # the stub reads no word, and an address that no table names is a number
        self.assertEqual(self.spelled('char *name', None), '0')
        self.assertEqual(self.spelled('char *name', None, original=struct.pack('<2I', 0, 0x2011)), '0, 8209')
        for word, name in ((0x2010, 'mixed'), (0x1000, 'refs')):
            with self.subTest(name=name), self.assertRaisesRegex(SystemExit, re.escape(
                    f'refs+0x00000004 holds {word:#010x}, the address of {name}, in a field its declaration types as '
                    'an integer')):
                self.spelled('char *name', None, original=struct.pack('<2I', 0, word))


class FloatLiterals(unittest.TestCase):
    def test_each_literal_reads_as_its_word_however_a_tie_breaks(self):
        # strtof reads a decimal as the float nearest it. A literal on a tie
        # between two floats has a neighbour of 60 digits that reads as the
        # other float. A literal of up to 9 digits on no tie lies farther
        # than that from every halfway point.
        strtof = ctypes.CDLL(None).strtof
        strtof.restype, strtof.argtypes = ctypes.c_float, (ctypes.c_char_p, ctypes.c_void_p)
        beside = Context(prec=60)
        for exponent in range(1, 255):
            for significand in (0, 1, 0x400000, 0x7fffff):
                for sign in (0, 0x80000000):
                    word = sign | exponent << 23 | significand
                    literal = Decimal(float_literal(word)[:-1])
                    for decimal in (literal.next_minus(beside), literal, literal.next_plus(beside)):
                        read, = struct.unpack('<I', struct.pack('<f', strtof(str(decimal).encode(), None)))
                        self.assertEqual(read, word, decimal)

    def test_each_literal_takes_the_fewest_digits_and_then_the_nearest(self):
        for word, literal in ((0x3dcccccd, '0.1F'),
                              # 1.666 reads as the float above
                              (0x3fd53f7c, '1.6659999F'),
                              # 1234567.25, halfway between 1234567.2 and 1234567.3, which both read back
                              (0x4996b43a, '1234567.2F'),
                              # 2**87, whose nearer literal 1.547425e+26 reads as the float below
                              (0x6b000000, '1.5474251e+26F'),
                              # 50331650 lies halfway between 50331648 and the float above, 50331652
                              (0x4c400000, '50331648.0F'),
                              # fixed notation from 0.0001 to below 1e9
                              (0x38d1b716, '9.999999e-5F'),
                              (0xb8d1b717, '-0.0001F'),
                              (0x4e6e6b27, '999999940.0F'),
                              (0x4e6e6b28, '1e+9F')):
            with self.subTest(word=hex(word)):
                self.assertEqual(float_literal(word), literal)
