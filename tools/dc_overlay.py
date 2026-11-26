"""Link one Dreamcast overlay with its loaded providers. Never import disc addresses."""
import argparse
from dataclasses import dataclass
from pathlib import Path
import re
from struct import Struct
import subprocess
import tempfile

from chao_banks import MODELS, MOTIONS
from config import CHAO_AREAS, fail, named
from tool_process import run
from sh_elf import BOUND, ET_EXEC, ET_REL, SHN_UNDEF, SHT_NOBITS, STB_GLOBAL, SYMBOL, ElfTables, Section


ROOT = Path(__file__).resolve().parents[1]
# the bases the overlays and the Chao areas link at, which verify_header
# holds each image's entry to. A Chao area's pointers address its bytes
# through 0x0c000000, as modules/chao/garden.c says, so the areas link in
# that view. tools/dc_memory.py holds each entry to the RAM that
# src/system/areas.h's STAGE_SLOT or garden.c's CHAO_AREA_SLOT names, though
# not to its view
STAGE_SLOT = 0x8c500000
CHAO_AREA_SLOT = 0x0c9c0000
PROGRAM = Struct('<8I')
WRITE, ALLOC, EXEC = 1, 2, 4
STT_OBJECT = 1
SHN_ABS = 0xfff1


def physical(address):
    """P0 and P1 are two views of Dreamcast main RAM."""
    if not (0x0c000000 <= address < 0x0d000000 or 0x8c000000 <= address < 0x8d000000):
        raise ValueError(f'{address:#010x} lies outside main RAM')
    return address & 0x1fffffff


def overlap(left, right):
    return max(left[0], right[0]) < min(left[1], right[1])


@dataclass(frozen=True)
class Load:
    address: int
    physical_address: int
    offset: int
    file_size: int
    memory_size: int
    flags: int


class Elf(ElfTables):
    """The sections, static symbol table and load segments of a little-endian SH ELF."""

    def __init__(self, path, expected_type=None, *, wanted_exports=None):
        super().__init__(path, path.read_bytes(), expected_type, require_names=True)
        # A symbol belongs to its section by index, so two allocated sections
        # of one name would leave one out of every check that walks named
        self.named = {}
        for section in self.sections:
            if section.flags & ALLOC:
                if section.name in self.named:
                    raise ValueError(f'{path}: two allocated sections named {section.name}')
                self.named[section.name] = section
        self.defined = {}
        self.object_sections = {}
        self.absolute = {}
        self.undefined = set()
        self.functions = set()
        for name, value, size, info, index in self.symbols():
            if not name:
                continue
            if wanted_exports is not None and (name not in wanted_exports or index == SHN_UNDEF):
                continue
            allocated = index < len(self.sections) and self.sections[index].flags & ALLOC
            if allocated:
                section = self.sections[index]
                if self.file_type == ET_REL:
                    value += section.address
                if (info & 15 == 2 and section.flags & EXEC
                        and section.address <= value < section.address + section.size):
                    self.functions.add(value)
            if info >> 4 not in BOUND:
                continue
            if index == SHN_UNDEF:
                self.undefined.add(name)
            elif index == SHN_ABS:
                self.absolute[name] = value
            elif allocated:
                if not section.address <= value <= value + size <= section.address + section.size:
                    raise ValueError(f'{path}: {name} lies outside {section.name}')
                if name in self.defined:
                    raise ValueError(f'{path}: duplicate definition of {name}')
                self.defined[name] = value
                if info >> 4 == STB_GLOBAL and info & 15 == STT_OBJECT and size:
                    self.object_sections[name] = section
        self.segments = []
        self.loads = []
        self.writable = []
        # Prebinding reads only the requested definitions. A standalone SH
        # provider may use the stock linker's separate stack segment. The
        # final image path always checks every symbol and all LOAD segments.
        if wanted_exports is not None:
            return
        if self.phnum:
            if self.phsize != PROGRAM.size:
                raise ValueError(f'{path}: invalid program header size')
            segments = PROGRAM.iter_unpack(self.slice(self.phoff, self.phnum * self.phsize))
            for kind, offset, address, paddr, initialized, size, flags, _ in segments:
                if kind != 1 or size == 0:
                    continue
                start = physical(address)
                if (initialized > size or offset + initialized > len(self.data)
                        or physical(address + size - 1) != start + size - 1):
                    raise ValueError(f'{path}: invalid LOAD at {address:#010x}')
                self.loads.append((start, start + size))
                self.segments.append(Load(address, paddr, offset, initialized, size, flags))
                if flags & 2:
                    self.writable.append((address, address + size))
        if self.file_type == ET_EXEC:
            if not self.loads or self.undefined:
                raise ValueError(f'{path}: incomplete executable; unresolved: {", ".join(sorted(self.undefined))}')
            ranges = sorted(self.loads)
            if any(overlap(left, right) for left, right in zip(ranges, ranges[1:])):
                raise ValueError(f'{path}: overlapping physical LOAD ranges')
            for section in self.named.values():
                if section.size:
                    start = physical(section.address)
                    if not any(a <= start < start + section.size <= b for a, b in ranges):
                        raise ValueError(f'{path}: {section.name} lies outside the loaded image')

    def section(self, name):
        if name not in self.named:
            raise ValueError(f'{self.path}: missing {name}')
        return self.named[name]


def callback_address(obj, elf, symbol_index, addend):
    """Resolve a header's R_SH_DIR32 using the functions in the final link."""
    if symbol_index >= len(obj.symbol_records) // SYMBOL.size:
        raise ValueError(f'{obj.path}: invalid overlay callback symbol index {symbol_index}')
    name_at, value, _, info, _, section = SYMBOL.unpack_from(obj.symbol_records, symbol_index * SYMBOL.size)
    name = obj.string(obj.symbol_names, name_at)
    if name and info >> 4 in BOUND and name in elf.defined:
        return (elf.defined[name] + addend) & 0xffffffff
    # GNU as can express a private callback as its input section plus a
    # displacement. Match the function at that offset in that exact section.
    target = (value + addend) & 0xffffffff
    names = {obj.string(obj.symbol_names, offset)
             for offset, entry, _, kind, _, owner in SYMBOL.iter_unpack(obj.symbol_records)
             if offset and owner == section and entry == target and kind & 15 == 2}
    addresses = {entry for offset, entry, _, kind, _, _ in SYMBOL.iter_unpack(elf.symbol_records)
                 if offset and kind & 15 == 2 and elf.string(elf.symbol_names, offset) in names}
    if len(addresses) > 1:
        # The final link keeps a private function's name but not its input
        # section, so two inputs' functions of one name can't be told apart
        raise ValueError(f'{obj.path}: the overlay callback at {target:#x} is a private '
                         f'{"/".join(sorted(names))} that another input also defines; rename one')
    if not addresses:
        raise ValueError(f'{obj.path}: cannot resolve the overlay callback {name!r} at {target:#x}')
    return addresses.pop()


def header_callbacks(obj, elf, prefix, length):
    header = obj.section('.head')
    data = obj.contents(header)
    expected = {offset: int.from_bytes(data[offset:offset + 4], 'little') for offset in range(prefix, length, 4)}
    relocated = set()
    for section in obj.sections:
        if section.kind not in (4, 9) or section.info >= len(obj.sections) or obj.sections[section.info] != header:
            continue
        relocation = Struct('<IIi' if section.kind == 4 else '<II')
        if (section.entry_size != relocation.size or section.size % relocation.size
                or section.link >= len(obj.sections) or obj.sections[section.link] != obj.symbol_table):
            raise ValueError(f'{obj.path}: invalid overlay header relocations')
        for offset, info, *_ in relocation.iter_unpack(obj.contents(section)):
            if offset not in expected or offset in relocated or info & 0xff != 1:
                raise ValueError(f'{obj.path}: unexpected overlay header relocation at +{offset:#x}')
            relocated.add(offset)
            # GNU ld for SH adds the DIR32 input word to the symbol value,
            # even for RELA entries that also carry an explicit addend.
            expected[offset] = callback_address(obj, elf, info >> 8, expected[offset])
    # A word no relocation rewrites leaves the link as it entered, so it
    # would match itself whatever function the link put at its address
    for offset, word in expected.items():
        if word and offset not in relocated:
            raise ValueError(f'{obj.path}: overlay header callback at +{offset:#x} has no relocation')
    return expected


def verify_header(image, elf, original=None):
    child = image in CHAO_AREAS
    base = CHAO_AREA_SLOT if child else STAGE_SLOT
    head = elf.section('.head')
    length = 12 if child else 24
    if head.kind != 1 or head.size != length or elf.entry != base or head.address != base:
        raise ValueError(f'{elf.path}: expected {length}-byte overlay header at {base:#010x}')
    data = elf.contents(head)
    prefix = 8 if child else 12
    if not any(data[:8]):
        raise ValueError(f'{elf.path}: empty overlay tag')
    if original is not None and data[:prefix] != original.contents(original.section('.head'))[:prefix]:
        raise ValueError(f'{elf.path}: invalid overlay header')
    if not any(start <= base and base + length <= end for start, end in elf.writable):
        raise ValueError(f'{elf.path}: overlay header is not writable')
    expected = None if original is None else header_callbacks(original, elf, prefix, length)
    for offset in range(prefix, length, 4):
        callback = int.from_bytes(data[offset:offset + 4], 'little')
        if expected is not None and callback != expected[offset]:
            raise ValueError(f'{elf.path}: header callback at +{offset:#x} holds {callback:#010x}, expected {expected[offset]:#010x}')
        if callback and callback not in elf.functions:
            raise ValueError(f'{elf.path}: header callback at +{offset:#x} has no function at {callback:#010x}')


def archive_section(elf, bank):
    section = elf.section('.chao_' + bank.name)
    if (section.kind != SHT_NOBITS or not section.flags & WRITE
            or section.address != bank.base or section.size != bank.size):
        raise ValueError(f'{elf.path}: misplaced {section.name}')
    return section


def chao_banks(elf):
    aliases = {}
    for bank in (MODELS, MOTIONS):
        archive_section(elf, bank)
        pointer = '_chao_' + bank.name + '_upload'
        if elf.absolute.get(pointer) != (bank.base | 0x80000000):
            raise ValueError(f'{elf.path}: misplaced {pointer}')
        aliases[pointer] = elf.absolute[pointer]
    for section in elf.named.values():
        if section.name not in ('.chao_models', '.chao_motions') and section.size:
            if section.address < STAGE_SLOT or section.address + section.size > (MODELS.base | 0x80000000):
                raise ValueError(f'{elf.path}: {section.name} lies outside the resident garden')
    return aliases


def chao_imports(image, parent, wanted):
    def native_symbols(owner):
        return {name: address for address, name in named(ROOT / 'config' / owner / 'symbols.txt')}

    world = native_symbols('chao')
    child = native_symbols(image)
    targets = set(child.values())
    exports = {}
    for symbol in wanted & parent.defined.keys():
        # The native maps name C symbols, which SH spells with a leading _
        if not symbol.startswith('_'):
            continue
        name = symbol[1:]
        address = parent.defined[symbol]
        physical_address = address & 0x1fffffff
        bank = next((b for b in (MODELS, MOTIONS) if b.base <= physical_address < b.base + b.size), None)
        if bank is not None:
            # The child names its archive records at their original P0 addresses.
            # Section-end labels and other symbols at the same address own no record.
            if (child.get(name) == physical_address
                    and parent.object_sections.get(symbol) is archive_section(parent, bank)):
                exports[symbol] = address
            continue
        target = world.get(name)
        # A renamed import can retain its original address under another name.
        # Portable-only APIs are type-checked by the area's paired LTO link.
        if target is not None:
            if target not in targets or child.get(name, target) != target:
                continue
        elif name in child:
            continue
        if not 0x0c500000 <= physical_address < MODELS.base:
            raise ValueError(f'{parent.path}: {symbol} is outside the resident Chao slot at {address:#010x}')
        exports[symbol] = address
    return exports


def imports(image, terminal, resident, parent, owned):
    # Older area links already bound some names from a placed Chao object.
    # They must still name the same storage in the Chao executable being loaded.
    chao_names = {}
    if parent is not None:
        chao_names.update(chao_banks(parent))
        chao_names.update(chao_imports(image, parent, terminal.undefined | terminal.absolute.keys()))
    for name, address in terminal.absolute.items():
        if owned.get(name) == address or chao_names.get(name) == address:
            continue
        raise ValueError(f'{terminal.path}: unverified absolute symbol {name} at {address:#010x}')
    bound, missing = {}, []
    for name in sorted(terminal.undefined):
        owners = [table[name] for table in (resident.defined, chao_names) if name in table]
        if len(owners) > 1:
            raise ValueError(f'{name} is defined by both the resident and Chao')
        if owners:
            if not re.fullmatch(r'[A-Za-z_.$][A-Za-z0-9_.$]*', name):
                raise ValueError(f'{terminal.path}: unsupported import spelling {name!r}')
            bound[name] = owners[0]
        else:
            missing.append(name)
    if missing:
        raise ValueError(f'{terminal.path}: unresolved: {", ".join(missing)}')
    return bound


def link_image(image, terminal, resident, output, toolchain, chao=None):
    child = image in CHAO_AREAS
    if not re.fullmatch(r'[a-z0-9_]+', image) or image == '1st_read':
        raise ValueError(f'{image}: not an overlay')
    if child != (chao is not None):
        raise ValueError(f'{image}: ' + ('a Chao area requires --chao' if child else 'only a Chao area takes --chao'))
    if output.resolve() in {p.resolve() for p in (terminal, resident, chao) if p is not None}:
        raise ValueError(f'{output}: would replace an input of its own link')
    main = Elf(resident, ET_EXEC)
    if main.entry != 0x8c010000:
        raise ValueError(f'{resident}: incorrect resident entry {main.entry:#010x}')
    parent = Elf(chao, ET_EXEC) if child else None
    if parent is not None:
        verify_header('chao', parent)
        if parent.contents(parent.section('.head'))[:8] != b'ALGARDEN':
            raise ValueError(f'{chao}: the area requires the Chao parent')
    obj = Elf(terminal, ET_REL)
    # These two names on chao-placed.o are loader P1 aliases of its P0 banks.
    # Their storage and coordinates are verified here and after the final link.
    owned = chao_banks(obj) if image == 'chao' else {}
    bound = imports(image, obj, main, parent, owned)
    base = CHAO_AREA_SLOT if child else STAGE_SLOT
    script = ROOT / 'config' / ('chao/image.ld' if image == 'chao' else 'overlay.ld')
    output.parent.mkdir(parents=True, exist_ok=True)
    with tempfile.TemporaryDirectory(dir=output.parent) as directory:
        temporary = Path(directory)
        names = temporary / 'imports.ld'
        names.write_text(''.join(f'{n} = 0x{v:08x};\n' for n, v in bound.items()))
        linked = temporary / 'image.elf'
        ld = toolchain / 'bin/sh-elf-ld'
        cmd = [ld, '-EL', '--no-undefined', '--fatal-warnings', '-z', 'max-page-size=4096',
               '--defsym', f'overlay_base={base:#x}']
        if image == 'chao':
            cmd.extend(['--no-warn-rwx-segments', '--defsym', f'chao_model_base={MODELS.base:#x}',
                        '--defsym', f'chao_motion_base={MOTIONS.base:#x}'])
        cmd.extend(['-T', names, '-T', script, '-e', hex(base), '-o', linked, terminal])
        run(cmd, check=True)
        result = Elf(linked, ET_EXEC)
        verify_header(image, result, obj)
        for name, address in bound.items():
            if result.absolute.get(name) != address:
                raise ValueError(f'{output}: final address of {name} differs from its provider')
        if image == 'chao':
            chao_banks(result)
            for section in obj.named.values():
                if section.size and result.section(section.name).address != section.address:
                    raise ValueError(f'{output}: final Chao {section.name} moved from {section.address:#010x}')
            if any(result.defined.get(n) != v for n, v in obj.defined.items()):
                raise ValueError(f'{output}: final Chao exports moved from the placed object')
        linked.replace(output)


def main():
    parser = argparse.ArgumentParser(prog='dc_overlay', description=__doc__)
    parser.add_argument('image')
    parser.add_argument('terminal', type=Path)
    parser.add_argument('resident', type=Path)
    parser.add_argument('output', type=Path)
    parser.add_argument('--toolchain', required=True, type=Path)
    parser.add_argument('--chao', type=Path)
    args = parser.parse_args()
    try:
        link_image(**vars(args))
    except (OSError, ValueError, subprocess.CalledProcessError) as error:
        fail(error)


if __name__ == '__main__':
    main()
