"""Hold each material chunk of a GNU image's polygon lists to colors that are 4-byte aligned.

    python3 tools/chunk_colors.py IMAGE ELF OUT

njCnkSetMaterial reads a material chunk's colors from its polygon list as
Uint32, and the SH-4 faults on a 32-bit load from an address that isn't a
multiple of 4. SHC's image puts every chunk's colors on a multiple of 4.
GCC aligns an array of Sint16 to 2, and a struct of them. make dc starts
each datum of a body source that holds a polygon list on 4 bytes, by the
declarations tools/polygon_lists.py writes, and this check holds each list
of the linked image to that.

The lists are content's, of data.txt and its pose banks, and those
polygon_lists() finds in each body source the image places: each list a
chunk model's plist points into, in a flat array, a struct of the list's
parts or a bank of poses. A content list lies where content.py assembles it
in its section, which the section's global symbols place in ELF. A body's
list lies at its offset in the ELF symbol of its datum's name and size,
where the link kept one as data, and runs to its NJD_CE.

The run fails on a material chunk whose colors aren't 4-byte aligned, and
names each: the datum, the chunk's byte in it, its type and its colors'
address. Otherwise OUT gets the count of the material chunks with colors
that the lists hold, on a line. The run fails too on a content list that
doesn't walk to its end, on a body's list that reaches no NJD_CE in its
datum, and on a list that no section ELF loads holds, and where the global
symbols of content's section place it nowhere, or at two addresses.
"""
import argparse
from pathlib import Path
import struct

import chunk_poses
import content
from config import Image, fail, save
from kinds import NJD_MATOFF, NJD_VERTOFF, ChunkError, polygon_chunks, walk
from polygon_lists import polygon_lists
from sh_elf import ET_EXEC, SHT_NOBITS, STB_GLOBAL, ElfTables

SHF_ALLOC = 2
STT_FUNC = 2

# sdk/ninja/njCnkSetMaterial.c: a material chunk's type, less NJD_MATOFF,
# holds a bit for each color the chunk holds, four bytes each
MAT_DIFFUSE, MAT_AMBIENT, MAT_SPECULAR = 1, 2, 4
MAT_COLORS = MAT_DIFFUSE | MAT_AMBIENT | MAT_SPECULAR
# a material chunk's colors start at this byte, after its head and its size
COLORS_OFFSET = 4


def content_lists(image):
    """Each polygon list content.py assembles for image, as (its name, the
    section it lies in, its offset there, its size), and each section's
    objects with an ELF symbol, as (the symbol, its offset there)."""
    lists, symbols = [], {}
    for obj, section, offset in content.sections(content.objects(image)):
        if obj.public:
            symbols.setdefault(section, []).append((obj.symbol, offset + obj.symbol_address - obj.address))
        if obj.kind == 'chunk_polygons':
            lists.append((obj.name, section, offset, obj.size))
        elif obj.kind in chunk_poses.BANK_KINDS:
            lists.extend((f'{obj.name}[{index}].polygons', section, offset + at, 2 * run.polygon_words)
                         for index, at, run in chunk_poses.poses(obj))
    return lists, symbols


def body_lists(image):
    """Each polygon list of a body source image places, as polygon_lists()
    gives it: (the body that holds it, the body's size, the list's offset
    in the body)."""
    found = []
    for source in map(Path, image.sources()):
        if source.suffix == '.c' and (Path('offsets') / source.with_suffix('.txt')).is_file():
            found.extend(polygon_lists(source))
    return found


class Linked(ElfTables):
    """An image's linked ELF: the address and size of each symbol of a name
    that isn't a function, by the name, and the bytes the image loads, by
    address. exported gives each global symbol's address, a function's too."""

    def __init__(self, path):
        super().__init__(path, Path(path).read_bytes(), ET_EXEC, require_names=True)
        self.placed, self.exported = {}, {}
        for name, value, size, info, index in self.symbols():
            if 0 < index < len(self.sections):
                if info & 15 != STT_FUNC:
                    self.placed.setdefault(name, set()).add((value, size))
                if info >> 4 == STB_GLOBAL:
                    self.exported[name] = value

    def read(self, address, size):
        for section in self.sections:
            if (section.kind != SHT_NOBITS and section.flags & SHF_ALLOC and section.address <= address
                    and address + size <= section.address + section.size):
                return self.slice(section.offset + address - section.address, size)
        fail(f'{self.path}: no section holds {size} bytes at {address:#010x}')


def colors(name, address, data, offset=None):
    """Each material chunk of the polygon list at address that holds
    colors, as (the report's name for it, its colors' address). data is the
    list, whole, or with offset, a body's bytes from the list's start at
    offset in the body, which the list's NJD_CE ends."""
    try:
        if offset is None:
            _, chunks = walk('chunk_polygons', data)
        else:
            chunks = list(polygon_chunks([half for half, in struct.iter_unpack('<H', data[:len(data) & ~1])]))
    except ChunkError as error:
        fail(f'{name} at {address:#010x}: {error}')
    for chunk in chunks:
        if NJD_MATOFF <= chunk.type < NJD_VERTOFF and (chunk.type - NJD_MATOFF) & MAT_COLORS:
            yield (f'{name}+{(offset or 0) + 2 * chunk.offset}: chunk type {chunk.type}',
                   address + 2 * chunk.offset + COLORS_OFFSET)


def report(image, elf):
    """image's report, its ELF at elf: how many material chunks of its
    lists hold colors, and a line for each whose colors aren't 4-byte
    aligned."""
    linked = Linked(elf)
    found = []
    if (image.path / 'data.txt').is_file():
        lists, symbols = content_lists(image)
        bases = {}
        for section, placed in symbols.items():
            starts = {linked.exported[symbol] - offset for symbol, offset in placed if symbol in linked.exported}
            if len(starts) > 1:
                fail(f'{elf}: content\'s {section} lies at {len(starts)} addresses by its symbols')
            bases.update((section, start) for start in starts)
        for name, section, offset, size in lists:
            if section not in bases:
                fail(f'{elf}: no symbol places content\'s {section}, which holds {name}')
            found.extend(colors(name, bases[section] + offset, linked.read(bases[section] + offset, size)))
    for name, size, offset in sorted(set(body_lists(image))):
        for value, placed in sorted(linked.placed.get('_' + name, ())):
            if placed == size:
                found.extend(colors(name, value + offset, linked.read(value + offset, size - offset), offset))
    return len(found), [f'{chunk}\'s colors at {at:#010x}\n' for chunk, at in found if at % 4]


def main(argv=None):
    parser = argparse.ArgumentParser(prog='chunk_colors', description=__doc__.split('\n\n')[0])
    parser.add_argument('image', metavar='IMAGE', help='an image of config/, such as stg13')
    parser.add_argument('elf', metavar='ELF', help="the image's linked ELF, such as build/dc/images/stg13.elf")
    parser.add_argument('out', metavar='OUT', help='where to write the count')
    args = parser.parse_args(argv)
    count, lines = report(Image(args.image), args.elf)
    if lines:
        fail(f'{args.elf}: {len(lines)} of {count} material chunks hold colors off 4 bytes', *lines)
    save(args.out, f'{count}\n'.encode())


if __name__ == '__main__':
    main()
