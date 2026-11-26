"""Link an overlay and read its loader descriptor and bindings back from the final SH-4 ELF."""
import os
from pathlib import Path
import struct
import subprocess
import tempfile
from types import SimpleNamespace
import unittest

from dc_overlay import ET_EXEC, ET_REL, MODELS, Elf, Section, chao_imports, link_image, verify_header


ROOT = Path(__file__).resolve().parents[1]
TOOLCHAIN = Path(os.environ.get('SH_ELF', ROOT / 'tools/sh-elf')).resolve()


def function(name):
    return f'.global {name}\n.type {name},@function\n{name}:\nrts\nnop\n.size {name},.-{name}\n'


def head(tag, mode, callbacks):
    words = callbacks if mode is None else [str(mode), *callbacks]
    return '.section .head,"aw"\n.balign 4\n.ascii "' + tag + '"\n.long ' + ','.join(words) + '\n'


class InvalidOverlay(unittest.TestCase):
    def test_race_archive_import_requires_its_owning_section(self):
        name = '_race_omo_starter_accessory'
        address = 0x0c8b9ddc
        models = Section('.chao_models', 8, 3, MODELS.base, 0, MODELS.size, 0, 0, 0)
        other = Section('.bss', 8, 3, address, 0, 52, 0, 0, 0)
        sections = {s.name: s for s in (models, other)}
        parent = SimpleNamespace(defined={name: address}, object_sections={name: other},
                                 section=sections.__getitem__, path='chao-placed.o')
        self.assertEqual(chao_imports('chaostgrace', parent, {name}), {})
        parent.object_sections[name] = models
        self.assertEqual(chao_imports('chaostgrace', parent, {name}), {name: address})

    def test_chao_import_requires_the_c_spelling(self):
        # an unprefixed draw would otherwise be checked as raw, a name in
        # neither native map
        address = 0x8c600000
        parent = SimpleNamespace(defined={'draw': address, '_draw': address}, path='chao.elf')
        self.assertEqual(chao_imports('chaostgneut', parent, {'draw'}), {})
        self.assertEqual(chao_imports('chaostgneut', parent, {'_draw'}), {'_draw': address})

    def test_image_name_and_parent_are_diagnosed_apart(self):
        with tempfile.TemporaryDirectory() as directory:
            work = Path(directory)
            args = work / 'a.o', work / 'main.elf', work / 'out.elf', work / 'no-toolchain'
            with self.assertRaisesRegex(ValueError, 'not an overlay'):
                link_image('1st_read', *args)
            with self.assertRaisesRegex(ValueError, 'requires --chao'):
                link_image('chaostgneut', *args)
            with self.assertRaisesRegex(ValueError, 'only a Chao area takes --chao'):
                link_image('title', *args, work / 'chao.elf')

    def test_output_cannot_name_an_input(self):
        with tempfile.TemporaryDirectory() as directory:
            work = Path(directory)
            terminal = work / 'title.o'
            terminal.write_bytes(b'placed object')
            with self.assertRaisesRegex(ValueError, 'would replace an input'):
                link_image('title', terminal, work / 'main.elf', work / '.' / 'title.o', work / 'no-toolchain')
            self.assertEqual(terminal.read_bytes(), b'placed object')

    def test_unrecognized_object_format_is_rejected_before_the_link(self):
        with tempfile.TemporaryDirectory() as directory:
            work = Path(directory)
            main = work / 'main.elf'
            terminal = work / 'title.o'
            output = work / 'title.elf'
            main.write_bytes(b'\x7fELF')
            terminal.write_bytes(b'original section map')
            with self.assertRaises(ValueError):
                link_image('title', terminal, main, output, work / 'no-toolchain')
            self.assertFalse(output.exists())


@unittest.skipUnless((TOOLCHAIN / 'bin/sh-elf-as').is_file(), 'requires SH-4 binutils under SH_ELF')
class OverlayLink(unittest.TestCase):
    def setUp(self):
        temporary = tempfile.TemporaryDirectory()
        self.addCleanup(temporary.cleanup)
        self.work = Path(temporary.name)
        self.resident = self.make_resident()

    def run_tool(self, tool, *args):
        return subprocess.check_output([TOOLCHAIN / 'bin' / ('sh-elf-' + tool), *args],
                                       cwd=self.work, text=True, stderr=subprocess.STDOUT)

    def assemble(self, name, source):
        asm = self.work / (name + '.s')
        obj = asm.with_suffix('.o')
        asm.write_text(source)
        self.run_tool('as', '-little', '-isa=sh4', '-o', obj, asm)
        return obj

    def executable(self, name, obj, script, entry):
        layout = self.work / (name + '.ld')
        elf = self.work / (name + '.elf')
        layout.write_text(script)
        self.run_tool('ld', '-EL', '-e', entry, '-T', layout, '-o', elf, obj)
        return elf

    def make_resident(self, extra=''):
        # A real data export is deliberately preceded by another word, so the
        # positive test distinguishes its address from the section base.
        source = ('.section .text,"ax"\n' + function('_START_SYS')
                  + '.section .data,"aw"\n.long 0\n.global _resident_value\n'
                  '_resident_value: .long 123\n' + extra)
        obj = self.assemble('main', source)
        script = ('PHDRS { main PT_LOAD FLAGS(7); }\n'
                  'SECTIONS { . = 0x8c010000; .text : { *(.text) } :main .data : { *(.data) } :main }\n')
        return self.executable('main', obj, script, '_START_SYS')

    def make_title(self, import_name='_resident_value', tag='TITLE   ', callback='_title_step'):
        callbacks = ['_title_init', '_title_exit', callback]
        source = head(tag, 1, callbacks) + '.text\n'
        source += ''.join(function(n) for n in ('_title_init', '_title_exit', '_title_step'))
        source += '.section .data,"aw"\n.balign 4\n.long ' + import_name + '\n'
        return self.assemble('title', source)

    def make_chao(self, archive_symbol=None):
        callbacks = ['_chao_init', '_chao_exit', '_chao_step']
        source = head('ALGARDEN', 0, callbacks) + '.text\n'
        source += ''.join(function(n) for n in callbacks)
        source += ('.data\n.global _garden_shared\n_garden_shared: .long 456\n'
                   '.section .chao_models,"aw",@nobits\n.global _model_record\n'
                   '.type _model_record,@object\n_model_record: .space 16\n.size _model_record,.-_model_record\n'
                   '.org 0x2216dc\n' + (archive_symbol or '') + '.org 0x221c94\n'
                   '.section .chao_motions,"aw",@nobits\n.global _motion_record\n_motion_record: .space 0x1042d0\n')
        obj = self.assemble('garden', source)
        script = (
            'PHDRS { header PT_LOAD FLAGS(6); code PT_LOAD FLAGS(5); data PT_LOAD FLAGS(6); '
            'models PT_LOAD FLAGS(6); motions PT_LOAD FLAGS(6); }\n'
            'SECTIONS { .chao_models 0x0c698700 (NOLOAD) : { *(.chao_models) } :models\n'
            '.chao_motions 0x0c8ba800 (NOLOAD) : { *(.chao_motions) } :motions\n'
            '_chao_models_upload = ADDR(.chao_models) | 0x80000000;\n'
            '_chao_motions_upload = ADDR(.chao_motions) | 0x80000000;\n'
            '.head 0x8c500000 : { *(.head) } :header\n'
            '.text : { *(.text) } :code\n.data : { *(.data) } :data }\n')
        return self.executable('garden', obj, script, '0x8c500000'), obj

    def make_child(self, image='chaostgneut', extra=''):
        name = '_' + image + '_step'
        tag = 'ALSTRACE' if image == 'chaostgrace' else 'ALSTNEUT'
        source = (head(tag, None, [name]) + '.text\n' + function(name)
                  + '.data\n.balign 4\n.long _garden_shared, _resident_value\n' + extra)
        return self.assemble(image, source)

    def link(self, image, obj, chao=None):
        dest = self.work / (image + '-final.elf')
        link_image(image, obj, self.resident, dest, TOOLCHAIN, chao)
        return dest

    def rejected(self, image, obj, chao=None):
        with self.assertRaises(ValueError):
            self.link(image, obj, chao)
        self.assertFalse((self.work / (image + '-final.elf')).exists())

    def symbols(self, elf):
        self.assertEqual(self.run_tool('nm', '-u', elf), '')
        return {row[0]: int(row[2], 16) for row in
                (s.split() for s in self.run_tool('nm', '-P', elf).splitlines()) if len(row) > 2}

    def section(self, elf, section):
        output = self.work / 'section.bin'
        self.run_tool('objcopy', '-O', 'binary', '-j', section, elf, output)
        return output.read_bytes()

    def load_flags(self, elf, address):
        data = elf.read_bytes()
        start, = struct.unpack_from('<I', data, 28)
        stride, count = struct.unpack_from('<HH', data, 42)
        matches = []
        for i in range(count):
            kind, _, vaddr, _, _, memsz, flags, _ = struct.unpack_from('<8I', data, start + stride * i)
            if kind == 1 and vaddr <= address < vaddr + memsz:
                matches.append(flags)
        return matches

    def test_program_uses_live_resident_export_and_local_callbacks(self):
        elf = self.link('title', self.make_title())
        symbols = self.symbols(elf)
        self.assertEqual(self.section(elf, '.data'), struct.pack('<I', self.symbols(self.resident)['_resident_value']))
        callbacks = [symbols[n] for n in ('_title_init', '_title_exit', '_title_step')]
        self.assertEqual(self.section(elf, '.head'), struct.pack('<8s4I', b'TITLE   ', 1, *callbacks))
        with elf.open('rb') as stream:
            self.assertEqual(struct.unpack_from('<I', stream.read(52), 24)[0], 0x8c500000)

    def test_header_slots_keep_the_functions_named_by_the_input_relocations(self):
        terminal = self.make_title()
        linked = self.link('title', terminal)
        elf = Elf(linked, ET_EXEC)
        # Both values are valid code addresses. Calling exit on entry would
        # still pass a check that only recognizes functions in the image.
        data = bytearray(elf.data)
        first = elf.section('.head').offset + 12
        data[first:first + 8] = data[first + 4:first + 8] + data[first:first + 4]
        linked.write_bytes(data)
        with self.assertRaisesRegex(ValueError, r'header callback at \+0xc holds .* expected'):
            verify_header('title', Elf(linked, ET_EXEC), Elf(terminal, ET_REL))

    def test_dir32_uses_the_input_word_as_its_addend(self):
        source = head('TITLE   ', 1, ['_first', '0', '_first + 4'])
        # The middle slot has an explicit RELA addend and a zero input word;
        # the last slot has its displacement in the input word, as SH ld uses.
        source += '.reloc .-8, R_SH_DIR32, _first + 4\n.text\n' + function('_first') + function('_second')
        linked = self.link('title', self.assemble('addends', source))
        symbols = self.symbols(linked)
        self.assertEqual(struct.unpack_from('<3I', self.section(linked, '.head'), 12),
                         (symbols['_first'], symbols['_first'], symbols['_second']))

    def test_literal_callback_is_rejected_even_at_a_function(self):
        def source(step):
            return (head('TITLE   ', 1, ['_first', '0', step]) + '.text\n'
                    + function('_first') + function('_second'))
        relocated = self.link('title', self.assemble('relocated', source('_second')))
        second = self.symbols(relocated)['_second']
        relocated.unlink()
        # the same layout puts _second at the same address, so only the
        # missing relocation tells the literal from the symbol
        self.rejected('title', self.assemble('literal', source(hex(second))))

    def test_duplicate_allocated_section_name_is_rejected(self):
        obj = self.assemble('twice', '.section .data,"aw"\n.long 1\n'
                                     '.section .data,"aw",@progbits,unique,1\n.long 2\n')
        with self.assertRaisesRegex(ValueError, 'two allocated sections named .data'):
            Elf(obj, ET_REL)

    def test_section_relative_private_function_remains_a_valid_callback(self):
        source = head('TITLE   ', 1, ['0', '0', 'local_step']) + '.text\nnop\n'
        source += '.type local_step,@function\nlocal_step: rts\nnop\n.size local_step,.-local_step\n'
        self.link('title', self.assemble('private', source))

    def test_private_callback_named_in_two_inputs_is_diagnosed(self):
        private = '.text\nnop\n.type local_step,@function\nlocal_step: rts\nnop\n.size local_step,.-local_step\n'
        first = self.assemble('first', head('TITLE   ', 1, ['0', '0', 'local_step']) + private)
        second = self.assemble('second', private)
        merged = self.work / 'merged.o'
        self.run_tool('ld', '-EL', '-r', '-o', merged, first, second)
        with self.assertRaisesRegex(ValueError, 'private local_step that another input also defines'):
            self.link('title', merged)

    def test_advertise_init_can_install_the_step(self):
        source = head('ADVERTIZ', 3, ['_advertise_init', '_advertise_exit', '0'])
        obj = self.assemble('advertise', source + '.text\n' + function('_advertise_init') + function('_advertise_exit'))
        elf = self.link('advertise', obj)
        symbols = self.symbols(elf)
        self.assertEqual(self.section(elf, '.head'), struct.pack('<8s4I', b'ADVERTIZ', 3,
                                                                 symbols['_advertise_init'],
                                                                 symbols['_advertise_exit'], 0))

    def test_descriptor_is_writable_and_callbacks_are_executable(self):
        elf = self.link('title', self.make_title())
        symbol = self.symbols(elf)
        self.assertEqual(self.load_flags(elf, 0x8c500000), [6])  # PF_R | PF_W
        self.assertEqual(self.load_flags(elf, symbol['_title_exit']), [5])  # PF_R | PF_X
        self.assertNotEqual(0x8c500000 >> 12, symbol['_title_exit'] >> 12)

    def test_child_uses_both_providers_and_its_p0_address(self):
        _, obj = self.make_chao()
        placed = self.work / 'garden-placed.o'
        self.run_tool('ld', '-EL', '-r', '-T', self.work / 'garden.ld', '-o', placed, obj)
        garden = self.link('chao', placed)
        exports = self.symbols(garden)
        self.assertEqual(exports['_model_record'], 0x0c698700)
        self.assertEqual(exports['_motion_record'], 0x0c8ba800)
        self.assertEqual(exports['_chao_models_upload'], exports['_model_record'] | 0x80000000)
        self.assertEqual(exports['_chao_motions_upload'], exports['_motion_record'] | 0x80000000)
        elf = self.link('chaostgneut', self.make_child(), garden)
        step = self.symbols(elf)['_chaostgneut_step']
        self.assertEqual(self.section(elf, '.head'), struct.pack('<8sI', b'ALSTNEUT', step))
        self.assertEqual(self.section(elf, '.data'), struct.pack('<2I', exports['_garden_shared'],
                                                                 self.symbols(self.resident)['_resident_value']))
        self.assertLess(0x0c9c0000, step)
        self.assertLess(step, 0x0c9d0000)

    def test_missing_import_cannot_fall_back_to_retail_map(self):
        # rand is in the real main symbol map, but this main ELF supplies no rand.
        self.rejected('title', self.make_title('_rand'))

    def test_absolute_resident_symbol_is_not_backed_by_storage(self):
        self.resident = self.make_resident('.global _number\n.set _number,0x8c010000\n')
        self.rejected('title', self.make_title('_number'))

    def test_ambiguous_child_import_is_rejected(self):
        self.resident = self.make_resident('.data\n.global _garden_shared\n_garden_shared: .long 99\n')
        garden, _ = self.make_chao()
        self.rejected('chaostgneut', self.make_child(), garden)

    def test_child_requires_a_fully_linked_parent(self):
        _, garden_object = self.make_chao()
        child = self.make_child()
        self.rejected('chaostgneut', child)
        self.rejected('chaostgneut', child, garden_object)

    def test_child_cannot_import_an_unlisted_chao_archive_object(self):
        garden, _ = self.make_chao()
        for extra in ('.data\n.long _model_record\n',
                      '.global _model_record\n.set _model_record,0x0c698700\n'):
            with self.subTest(extra=extra):
                self.rejected('chaostgneut', self.make_child(extra=extra), garden)

    def test_race_import_requires_storage_in_the_models_bank(self):
        name = '_race_omo_starter_accessory'
        child = self.make_child('chaostgrace', '.long ' + name + '\n')
        for declaration in ('', f'.type {name},@object\n'):
            with self.subTest(declaration=declaration):
                garden, _ = self.make_chao(f'.global {name}\n{name}:\n{declaration}')
                self.rejected('chaostgrace', child, garden)
        garden, _ = self.make_chao(f'.global {name}\n.type {name},@object\n{name}: .space 52\n.size {name},52\n')
        result = Elf(self.link('chaostgrace', child, garden), ET_EXEC)
        self.assertEqual(result.absolute[name], 0x0c8b9ddc)

    def test_stale_prebound_parent_export_is_rejected(self):
        garden, _ = self.make_chao()
        obj = self.make_child(extra='.global _garden_shared\n.set _garden_shared,0x8c500000\n')
        self.rejected('chaostgneut', obj, garden)

    def test_loader_rejects_an_empty_header_tag(self):
        self.rejected('title', self.make_title(tag=r'\000' * 8))

    def test_data_pointer_cannot_serve_as_a_callback(self):
        self.rejected('title', self.make_title(callback='_resident_value'))
