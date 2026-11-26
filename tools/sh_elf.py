"""Bounded section and symbol tables of Dreamcast little-endian SuperH ELF files."""
from dataclasses import dataclass
from struct import Struct


HEADER = Struct('<16sHHIIIIIHHHHHH')
SECTION = Struct('<10I')
SYMBOL = Struct('<IIIBBH')
ET_REL, ET_EXEC = 1, 2
SHN_UNDEF, SHN_XINDEX = 0, 0xffff
SHT_NULL, SHT_SYMTAB, SHT_STRTAB, SHT_NOBITS = 0, 2, 3, 8
STB_GLOBAL, STB_WEAK = 1, 2
BOUND = (STB_GLOBAL, STB_WEAK)


class WrongElfType(ValueError):
    """The input is for another architecture or is the wrong kind of ELF."""


@dataclass(frozen=True)
class Section:
    name: str
    kind: int
    flags: int
    address: int
    offset: int
    size: int
    link: int
    info: int
    entry_size: int



class ElfTables:
    """Read the file header, section directory and single static symbol table."""

    def __init__(self, path, data, expected_type=None, *, require_names=False):
        self.path, self.data = path, data
        if len(data) < HEADER.size or data[:4] != b'\x7fELF':
            raise ValueError(f'{path}: not an ELF file')
        (ident, self.file_type, machine, version, self.entry, self.phoff, shoff, _, ehsize,
         self.phsize, self.phnum, shsize, count, strings) = HEADER.unpack_from(data)
        if (ident[:7] != b'\x7fELF\x01\x01\x01' or version != 1 or machine != 42
                or self.file_type not in (ET_REL, ET_EXEC)):
            raise WrongElfType(f'{path}: expected a little-endian SH relocatable or executable')
        if expected_type is not None and self.file_type != expected_type:
            raise WrongElfType(f'{path}: expected ELF type {expected_type}, got {self.file_type}')
        if ehsize != HEADER.size or not count or shsize != SECTION.size or strings >= count:
            raise ValueError(f'{path}: invalid section directory')
        headers = list(SECTION.iter_unpack(self.slice(shoff, count * shsize)))
        if strings:
            if headers[strings][1] != SHT_STRTAB:
                raise ValueError(f'{path}: missing section names')
            names = self.slice(*headers[strings][4:6])
        elif require_names:
            raise ValueError(f'{path}: invalid section directory')
        else:
            # e_shstrndx == SHN_UNDEF is valid for an object without section names.
            names = b'\0'
        # An inactive slot's name is undefined, but it keeps its place in the numbering.
        self.sections = [Section('' if h[1] == SHT_NULL else self.string(names, h[0]), *h[1:8], h[9])
                         for h in headers]
        tables = [s for s in self.sections if s.kind == SHT_SYMTAB]
        if not tables:
            raise ValueError(f'{path}: no symbol table')
        if (len(tables) != 1 or tables[0].entry_size != SYMBOL.size
                or tables[0].size < SYMBOL.size or tables[0].size % SYMBOL.size):
            raise ValueError(f'{path}: expected one static symbol table')
        self.symbol_table = tables[0]
        link = self.symbol_table.link
        if link >= count or self.sections[link].kind != SHT_STRTAB:
            raise ValueError(f'{path}: missing symbol names')
        self.symbol_records = self.contents(self.symbol_table)
        self.symbol_names = self.contents(self.sections[link])

    def symbols(self):
        """Yield the name, value, size, binding/type and section index of each static symbol."""
        for at, value, size, info, _, index in SYMBOL.iter_unpack(self.symbol_records):
            # Special section indices include SHN_ABS and SHN_COMMON; SHN_XINDEX's real index lies in a table
            # this reader doesn't read.
            if len(self.sections) <= index < 0xff00 or index == SHN_XINDEX:
                raise ValueError(f'{self.path}: invalid symbol section index {index}')
            yield self.string(self.symbol_names, at), value, size, info, index

    def slice(self, offset, size):
        if offset < 0 or size < 0:
            raise ValueError(f'{self.path}: negative ELF range at {offset:#x}, {size:#x} bytes')
        if offset + size > len(self.data):
            raise ValueError(f'{self.path}: truncated ELF at {offset:#x}')
        return self.data[offset:offset + size]

    def contents(self, section):
        """The section's bytes in the file, which a SHT_NOBITS section has none of."""
        return b'' if section.kind == SHT_NOBITS else self.slice(section.offset, section.size)

    def string(self, names, offset):
        if offset == 0 and not names:
            return ''  # An empty ELF string table has no names.
        end = names.find(b'\0', offset)
        if offset < 0 or end < 0:
            raise ValueError(f'{self.path}: invalid ELF string')
        return names[offset:end].decode('latin1')
