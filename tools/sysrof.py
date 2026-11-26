"""Read an object's sections, the names it defines and the names it takes
from outside, and the sections whose bounds its relocations take.

SHC and asmsh write objects in Hitachi's SYSROF format, a sequence of records.
A record is a kind byte, a size byte that counts the whole record, a body, and
a checksum byte that brings the low byte of the record's sum to 0xff. Records of one kind form
a run, whose bodies join into one, and bit 0x80 of the kind marks a run's last
record. The run of kind END ends the object.

An object holds the time it was built, so two builds of one source can differ.
Compare what they link to, not the objects.
"""

from typing import NamedTuple

from config import content, fail

# the record kinds read here: the section table, the external references,
# the exported definitions, the relocations and the end of the object
SECTIONS, REFERENCES, EXPORTS, RELOCATIONS, END = 0x08, 0x0C, 0x14, 0x20, 0x7F
LAST = 0x80
# a relocation's expression is a run of terms and operators, in postfix,
# ended by END_OF_EXPRESSION. A term is a section's address, an outside
# name, a section's size or start, each by its index in two bytes, or a
# constant, its length in a byte and then its bytes. Shifts also carry their
# count this way. The other operators have no operand bytes.
TERMS = {'address': 0x00, 'name': 0x02, 'constant': 0x03, 'size': 0x04, 'start': 0x05}
OPERATORS = range(0x20, 0x30)
SHIFTS = (0x25, 0x26)  # right, left
END_OF_EXPRESSION = 0xFF
# Four eight-bit field slices at positions 24, 16, 8 and 0 write a little-endian long.
LITTLE_ENDIAN_LONG = bytes.fromhex('18 08 10 08 08 08 00 08')


class Section(NamedTuple):
    """A section as its object declares it. size counts the bytes the object
    reserves without writing, and alignment is a power of two."""

    size: int
    alignment: int
    code: bool


def records(path):
    """Yield each record of the object at path as (offset, bytes), in file
    order through the record that ends the object. A record fails where it is
    cut short, holds under 3 bytes or has a wrong checksum. Bytes after the
    end record fail too, as does an object without one."""
    data = content(path)
    at = 0
    while at < len(data):
        if at + 2 > len(data) or at + data[at + 1] > len(data):
            fail(f'{path}: the record at {at:#x} is cut short')
        record = data[at:at + data[at + 1]]
        if len(record) < 3:
            fail(f'{path}: the record at {at:#x} has size {len(record)}')
        if sum(record) & 0xFF != 0xFF:
            fail(f'{path}: the record at {at:#x} fails its checksum')
        if record[0] == LAST | END and at + len(record) < len(data):
            fail(f'{path}: bytes follow its end, at {at + len(record):#x}')
        yield at, record
        if record[0] == LAST | END:
            return
        at += len(record)
    fail(f'{path}: no end record')


def runs(path):
    """Yield each run of the object at path as (offset, kind, body): the
    offset of the run's first record, the kind without LAST, and the bodies of
    the run's records joined. A record of another kind inside a run fails."""
    start, kind, body = 0, None, bytearray()
    for at, record in records(path):
        this = record[0] & ~LAST
        if kind not in (None, this):
            fail(f'{path}: the record at {at:#x} breaks a run of kind {kind:#04x}')
        kind = this
        body += record[2:-1]
        if record[0] & LAST:
            yield start, kind, bytes(body)
            start, kind, body = at + len(record), None, bytearray()


def sections(path):
    """The sections of the object at path, as {name: Section} in the table's
    order. A section table that doesn't parse fails, as do a name it gives
    twice and an alignment that isn't a power of two."""
    # an entry is 16 bytes, then the name's length and the name. The size is
    # the big-endian word at offset 5 and the alignment the one at 9. Bit 0x10
    # of byte 13 marks data.
    found = {}
    for start, kind, body in runs(path):
        if kind != SECTIONS:
            continue
        at = 0
        while at < len(body):
            if at + 17 > len(body) or (end := at + 17 + body[at + 16]) > len(body):
                fail(f"{path}: the sections at {start:#x} don't parse")
            name = body[at + 17:end].decode('latin-1')
            size = int.from_bytes(body[at + 5:at + 9], 'big')
            alignment = int.from_bytes(body[at + 9:at + 13], 'big')
            if name in found:
                fail(f'{path}: the sections at {start:#x} name {name} again')
            if not alignment or alignment & (alignment - 1):
                fail(f'{path}: the sections at {start:#x} align {name} to {alignment}, not a power of two')
            found[name] = Section(size, alignment, not body[at + 13] & 0x10)
            at = end
    return found


def relocations(path):
    """Yield each relocation of the object at path as (offset, section, field,
    expression). offset is that of the first record of the relocation's run,
    so every relocation of a run shares it. section is the index of the
    section the last 0x1a run selected, or None before the first: a 0x1a run
    selects the section whose offsets the data and relocations after it use.
    field is the relocation's descriptor, and expression its postfix
    expression through END_OF_EXPRESSION. A relocation that doesn't parse
    fails."""
    section = None
    for start, kind, body in runs(path):
        if kind == 0x1A:
            if len(body) != 4:
                fail(f"{path}: the section selection at {start:#x} doesn't parse")
            section = int.from_bytes(body, 'big')
        if kind != RELOCATIONS:
            continue
        at = 0
        while at < len(body):
            # Field offsets take four bytes. The descriptor following them
            # takes two bytes for 04/05, four for 14 and eight for 34/35.
            head = {0x04: 7, 0x05: 7, 0x14: 9, 0x34: 13, 0x35: 13}.get(body[at])
            if head is None or at + head >= len(body):
                fail(f"{path}: the relocations at {start:#x} don't parse")
            size = body[at + head]
            expr = body[at + head + 1:at + head + 1 + size]
            if not expr or len(expr) != size or expr[-1] != END_OF_EXPRESSION:
                fail(f"{path}: the relocations at {start:#x} don't parse")
            yield start, section, body[at:at + head], expr
            at += head + 1 + size


def bounded(path):
    """The names of the sections whose start or size a relocation of the
    object at path takes, as an assembler's STARTOF and SIZEOF give them, in
    the table's order. A relocation fails where relocations() can't parse it.
    It also fails where its expression holds a byte that is no term or
    operator, an operand runs into END_OF_EXPRESSION, or a bound's index
    passes the table. The check reads the expression's tokens, not its postfix
    order, so two terms with no operator between them pass."""
    names, found = list(sections(path)), set()
    for start, _, _, expression in relocations(path):
        i = 0
        while i < len(expression) - 1:
            term = expression[i]
            if term in (TERMS['size'], TERMS['start']):
                index = int.from_bytes(expression[i + 1:i + 3], 'big')
                if index >= len(names):
                    fail(f"{path}: the relocations at {start:#x} don't parse")
                found.add(index)
            if term in (TERMS['address'], TERMS['name'], TERMS['size'], TERMS['start']):
                i += 3
            elif term == TERMS['constant'] or term in SHIFTS:
                if i + 1 >= len(expression) - 1:
                    fail(f"{path}: the relocations at {start:#x} don't parse")
                i += 2 + expression[i + 1]
            elif term in OPERATORS:
                i += 1
            else:
                fail(f"{path}: the relocations at {start:#x} don't parse")
        if i != len(expression) - 1:
            fail(f"{path}: the relocations at {start:#x} don't parse")
    return [name for index, name in enumerate(names) if index in found]


def word_target(path, section, offset):
    """The target of a complete little-endian long at offset in section
    when its sole relocation term is a name, local section, or STARTOF.
    Return (term, name), or None for any other field or expression. A name,
    section or STARTOF whose index passes its table fails."""
    inputs = list(sections(path))
    if section not in inputs:
        return None
    target = inputs.index(section)
    for start, index, field, expression in relocations(path):
        if index != target or int.from_bytes(field[1:5], 'big') != offset:
            continue
        if field[0] not in (0x34, 0x35) or field[5:] != LITTLE_ENDIAN_LONG or len(expression) != 4:
            continue
        term, operand = expression[0], int.from_bytes(expression[1:3], 'big')
        if term == TERMS['name']:
            names = list(reference_names(path))
            if operand >= len(names):
                fail(f'{path}: the relocations at {start:#x} take reference {operand}, and the object has {len(names)}')
            return 'name', names[operand]
        if term in (TERMS['start'], TERMS['address']):
            if operand >= len(inputs):
                fail(f'{path}: the relocations at {start:#x} take section {operand}, and the object has {len(inputs)}')
            return ('start' if term == TERMS['start'] else 'address'), inputs[operand]
    return None


def c_name(name):
    """name, as an object spells it, as C spells it: without the underscore
    that opens a C name."""
    return name[1:] if name.startswith('_') else name


def references(path):
    """The names the object at path takes from outside, as it spells them: a
    C name has a leading underscore. References that don't parse fail."""
    return set(reference_names(path))


def reference_names(path):
    """The external references in the order relocations index them."""
    # an entry is 0xc0, the name's length and the name
    for start, kind, body in runs(path):
        if kind != REFERENCES:
            continue
        at = 0
        while at < len(body):
            if at + 2 > len(body) or body[at] != 0xC0 or (end := at + 2 + body[at + 1]) > len(body):
                fail(f"{path}: the references at {start:#x} don't parse")
            yield body[at + 2:end].decode('latin-1')
            at = end


def exports(path):
    """The names the object at path defines for other objects, as {name:
    (section, offset)}: the name as the object spells it, the section that
    holds it and its offset there. Exports that don't parse fail, as do one in
    a section the table lacks and a name exported at two places."""
    # an entry is its section's index in the table, in two bytes, a byte of
    # flags, the big-endian offset, then the name's length and the name
    names, found = list(sections(path)), {}
    for start, kind, body in runs(path):
        if kind != EXPORTS:
            continue
        at = 0
        while at < len(body):
            if at + 8 > len(body) or (end := at + 8 + body[at + 7]) > len(body):
                fail(f"{path}: the exports at {start:#x} don't parse")
            index = int.from_bytes(body[at:at + 2], 'big')
            if index >= len(names):
                fail(f'{path}: the exports at {start:#x} name section {index}, and the object has {len(names)}')
            name, place = body[at + 8:end].decode('latin-1'), (names[index], int.from_bytes(body[at + 3:at + 7], 'big'))
            if found.setdefault(name, place) != place:
                fail(f'{path}: the exports at {start:#x} put {name} at {place[0]}+{place[1]:#x}'
                     f' and at {found[name][0]}+{found[name][1]:#x}')
            at = end
    return found
