"""Execute Ninja's texture readback and the signed C runtime comparator after the SH-4 LTO link."""

from pathlib import Path
import subprocess
import tempfile
import unittest

from support import CC, ROOT, SH4ZAM, SH_ELF, gcc_flags
from test_zam import LEVELS, Machine, STACK, TEXT, read_linked, signed, toolchain


class TransferMachine(Machine):
    """Record the scalar stores and the block counts passed to Ninja's assembly copy routines."""
    def __init__(self, names):
        super().__init__(None)
        self.block_entries = {names[f'njMemCopy{width}']: width for width in (8, 16, 32)}
        self.blocks = []
        self.stores = []

    def execute(self, pc):
        if pc in self.block_entries:
            self.blocks.append((self.block_entries[pc], self.r[6]))
        return super().execute(pc)

    def store_sized(self, address, word, size):
        self.stores.append((address, size))
        super().store_sized(address, word, size)


class CopyCompare(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        toolchain()
        work = tempfile.TemporaryDirectory()
        cls.addClassCleanup(work.cleanup)
        work = Path(work.name)
        shckit = ROOT / 'build/shckit'
        if not shckit.is_file():
            shckit = work / 'shckit'
            subprocess.run([*CC, '-std=c11', ROOT / 'tools/shckit/main.c', ROOT / 'tools/shckit/shckit.c',
                            '-o', shckit], check=True)
        mem = work / 'mem.s'
        subprocess.run([shckit, 'translate', '-o', mem, ROOT / 'sdk/ninja/njMem.src'], check=True)
        script = work / 'link.ld'
        script.write_text(f'SECTIONS {{ . = {TEXT:#x}; .text : {{ *(.text*) *(PSG) }} .rodata : {{ *(.rodata*) }} }}')
        sources = ('sdk/ninja/njTexLow.c', 'sdk/ninja/njTexScalar.c', 'engine/dc/texture_copy.c',
                   'sdk/runtime/strcmp.c', 'engine/dc/strcmp.c')
        roots = ('njReadTexturePartLow', 'njTexMemCopy', '_quick_strcmp1', '_slow_strcmp1', 'strcmp',
                 *(f'njTexMemCopy{n}' for n in (1, 2, 4, 8, 16, 32)))
        cls.images = {}
        for level in LEVELS:
            elf = work / (level + '.elf')
            subprocess.run([SH_ELF / 'bin/sh-elf-gcc', *gcc_flags(SH_ELF), '-I.', '-Isdk/include', '-isystem',
                            SH4ZAM / 'include', level, '-flto', '-nostdlib', f'-Wl,-T,{script}', '-Wl,--gc-sections',
                            '-Wl,-e,_njReadTexturePartLow', *[f'-Wl,-u,_{name}' for name in roots], '-o', elf,
                            *sources, mem], cwd=ROOT, check=True)
            cls.images[level] = read_linked(elf, {})

    def machine(self, level, trace=False):
        code, image, names = self.images[level]
        machine = TransferMachine(names) if trace else Machine(None)
        machine.code = code
        machine.memory.update(image)
        machine.r[15] = STACK
        return machine, names

    def test_signed_comparison_through_runtime_entry(self):
        for level in LEVELS:
            for ax, bx in ((0, 0), (0, 1), (1, 0), (1, 1)):
                for prefix in (0, 3, 4, 8):
                    for left in (0, 127, 128, 255):
                        for right in (0, 127, 128, 255):
                            a, b = 0x1000 + ax, 0x2000 + bx
                            machine, names = self.machine(level)
                            for address, byte in ((a, left), (b, right)):
                                machine.memory.update(enumerate(b'x' * prefix + bytes((byte, 0)), address))
                            entries = ['strcmp', '_slow_strcmp1']
                            if ax == bx == 0:
                                entries.append('_quick_strcmp1')
                            for name in entries:
                                with self.subTest(level=level, name=name, prefix=prefix, left=left, right=right):
                                    want = (left if left < 128 else left - 256) - (right if right < 128 else right - 256)
                                    self.assertEqual(signed(machine.run(names[name], (a, b))), want)

    def test_texture_read_uses_surface_offset_and_exact_copy_extent(self):
        record, surface, destination = 0x1000, 0xa4003000, 0x5000
        pixels = bytes((i * 19 + 7) & 255 for i in range(160))
        for level in LEVELS:
            for width in (1, 2, 4, 8, 16, 32):
                for start, offset, count in ((width, 32, 64), (32, width, 64), (32, 64, width * 3), (0, 0, 0)):
                    with self.subTest(level=level, start=start, offset=offset, count=count):
                        machine, names = self.machine(level, trace=True)
                        # The retail NJS_TEXMEMLIST holds texinfo.texsurface.pSurface at byte 52.
                        machine.store(record + 52, 0x3000)
                        machine.memory.update(enumerate(pixels, surface))
                        machine.memory.update(enumerate(bytes([0xa5]) * 160, destination))
                        machine.written.clear()
                        self.assertEqual(machine.run(names['njReadTexturePartLow'],
                                                     (record, destination + start, offset, count)), 1)
                        observed = bytes(machine.memory[destination + i] for i in range(160))
                        self.assertEqual(observed, b'\xa5' * start + pixels[offset:offset + count]
                                         + b'\xa5' * (160 - start - count))
                        self.assertEqual({a for a in machine.written if a < STACK - 128},
                                         set(range(destination + start, destination + start + count)))
                        if count and width >= 8:
                            self.assertEqual(machine.blocks, [(width, count // width)])
                        else:
                            self.assertEqual(machine.blocks, [])
                            self.assertEqual([size for addr, size in machine.stores
                                              if destination <= addr < destination + 160], [width] * (count // width))
                        self.assertEqual(machine.sz, 0)

    def test_texture_forward_overlap_and_sub_block_guards(self):
        base = 0x3000
        for level in LEVELS:
            for width in (1, 2, 4, 8, 16, 32):
                for offset in (-width, 0, width):
                    machine, names = self.machine(level)
                    expected = bytearray((71 * i + 11) & 255 for i in range(160))
                    machine.memory.update(enumerate(expected, base))
                    for i in range(64):
                        expected[32 + offset + i] = expected[32 + i]
                    machine.run(names['njTexMemCopy'], (base + 32 + offset, base + 32, 64))
                    self.assertEqual(bytes(machine.memory[base + i] for i in range(160)), expected,
                                     (level, width, offset))
                machine, names = self.machine(level)
                machine.run(names[f'njTexMemCopy{width}'], (base, base + 64, width - 1))
                self.assertFalse({a for a in machine.written if a < STACK - 128})
