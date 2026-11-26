"""Disc layouts of static land, Ninja Basic collision models and texture lists.

A land's draw takes its first land.drawn entries, Ninja chunk objects, and
collision takes the rest, Ninja Basic objects. Where drawn is negative, both
take every entry, and each entry's flags choose its object's type. The two
object types have the same size but point to different model types.
"""

from pathlib import Path
import re
import struct
from typing import NamedTuple

from body_data import declared_base
from config import SOURCE, aligned, fail
from decls import declarations
from kinds import KINDS, LAND_DRAWN, LAND_OBJECT, Choice
from layout import Array, Record, Scalar


# the arrays whose elements' pointers array_pointers resolves
ARRAYS = {'land_entries', 'scenery_entries', 'texture_names', 'basic_meshsets', 'container_pieces',
          'model_builds', 'texture_pvms'}


def layout(kind, fields, where):
    if kind == 'scenery_entries':
        if (len(fields) != 2 or any(not re.fullmatch(r'[1-9][0-9]{0,8}', value) for value in fields)
                or int(fields[1]) < int(fields[0])):
            fail(f'{where}: {kind} needs its culling entry and object counts')
        return KINDS[kind].size * int(fields[0]), KINDS[kind].alignment, tuple(fields)
    if kind == 'texture_names' and len(fields) == 2 and fields[1] == 'empty':
        size, alignment, _ = layout(kind, fields[:1], where)
        return size, alignment, tuple(fields)
    counts = (1, 2) if kind == 'land_entries' else (1,)
    if len(fields) not in counts or any(not re.fullmatch(r'[1-9][0-9]{0,8}', value) for value in fields):
        required = ('a total count or positive drawn and collision counts'
                    if kind == 'land_entries' else 'a positive element count')
        fail(f'{where}: {kind} needs {required}')
    return KINDS[kind].size * sum(map(int, fields)), KINDS[kind].alignment, tuple(fields)


BANK_STRINGS = {'land_filenames': 'texture_filename', 'text_bank': 'text'}


def string_bank_layout(kind, fields, where):
    counts = (2, 3) if kind == 'land_filenames' else (2,)
    if len(fields) not in counts or not SOURCE.fullmatch(fields[0]) or not fields[0].endswith('.c'):
        fail(f'{where}: {kind} needs its C source and byte count')
    if not re.fullmatch(r'[1-9][0-9]{0,8}', fields[1]):
        fail(f'{where}: {kind} needs a positive byte count')
    if len(fields) == 3:
        offset = fields[2]
        if (not re.fullmatch(r'[1-9][0-9]{0,8}', offset)
                or int(offset) >= int(fields[1]) or int(offset) % 4):
            fail(f'{where}: archive offset must be word-aligned inside the filename bank')
    return int(fields[1]), 4, (fields[0], *fields[2:])


def fixed_filenames(fields, where):
    if len(fields) != 2 or not all(re.fullmatch(r'[1-9][0-9]{0,8}', field) for field in fields):
        fail(f'{where}: a filename table needs a positive slot count and byte width')
    count, width = map(int, fields)
    if width % 4 or count * width > 0xffffffff:
        fail(f'{where}: the fixed-width filename table must fit in SH-4 memory with word-aligned slots')
    return count * width, 4, (str(width),)


def string_bank_provider(image, obj):
    """Return the widths of named struct fields, or an empty tuple for a packed char array.

    objects has already bound obj.name to obj.address through the image's
    symbols. A char array that is its source's only C range packs several
    aligned filenames. One among other C ranges holds exactly one filename.
    """
    source = Path(obj.targets[0])
    end = obj.address + obj.size
    placed = image.ranges(str(source))
    if not obj.public or not any(section == 'C' and start <= obj.address and end <= stop
                                 for section, start, stop in placed):
        fail(f'{obj.where}: {obj.name} needs a compiled C owner in {source}')
    if any(start < end and stop > obj.address for start, stop in image.unmatched(str(source))):
        fail(f'{obj.where}: {obj.name} overlaps a disc row inside its C owner')
    _, types, declarators = declarations(source)
    bank = next((each for each in declarators if each.name == obj.name and each.function is None
                 and each.initialized), None)
    if bank is None or bank.storage == 'static':
        fail(f'{obj.where}: {obj.name} needs a public const char array or struct of filename fields')
    layout = types.layout(bank.term)
    if layout.size != obj.size:
        fail(f'{obj.where}: {obj.name} needs its own contiguous filename datum')
    term, qualifiers = declared_base(types, bank.term)
    while term[0] == 'array':
        # an array is as qualified as its elements
        term, more = declared_base(types, term[1])
        qualifiers |= more
    if 'const' in qualifiers and layout == Array(Scalar('c'), obj.size):
        constants = [(start, stop) for section, start, stop in placed if section == 'C']
        return () if constants == [(obj.address, end)] else (obj.size,)
    if 'const' not in qualifiers or not isinstance(layout, Record) or layout.union or layout.holes:
        fail(f'{obj.where}: {obj.name} needs a public const char array or struct of filename fields')
    for field, member, offset in layout.fields:
        if not isinstance(member, Array) or member.element != Scalar('c') or member.count < 1 or offset % 4:
            fail(f'{obj.where}: {obj.name}.{field} must be a word-aligned char array')
    return tuple(member.count for _, member, _ in layout.fields)


def texture_slice(fields, where):
    if (len(fields) != 3 or not re.fullmatch(r'0|[1-9][0-9]{0,8}', fields[1])
            or not re.fullmatch(r'[1-9][0-9]{0,8}', fields[2])):
        fail(f'{where}: a texture sublist needs its array, first slot and positive count')
    return 8, 4, tuple(fields)


class BankString(NamedTuple):
    address: int
    symbol: str
    kind: str


def bank_strings(bank, data, base):
    """Index complete word-aligned strings in their actual compiled char array."""
    start = bank.address - base
    end = start + bank.size
    if start < 0 or end > len(data):
        fail(f'{bank.where}: original image does not hold {bank.name}')
    if bank.kind == 'texture_filenames':
        width = int(bank.targets[0])
        for cursor in range(start, end, width):
            slot = data[cursor:cursor + width]
            name, terminator, padding = slot.partition(b'\0')
            if not name or not terminator or any(padding):
                fail(f'{bank.where}: invalid fixed-width filename at {cursor - start:#x} in {bank.name}')
            offset = cursor - start
            symbol = bank.symbol if not offset else f'{bank.symbol}+{offset}'
            yield BankString(base + cursor, symbol, 'texture_filename')
        return
    widths = iter(bank.filename_widths)
    cursor = start
    while cursor < end:
        nul = data.find(b'\0', cursor, end)
        if nul <= cursor:
            fail(f'{bank.where}: empty or unterminated filename in {bank.name} at {cursor - start:#x}')
        offset = cursor - start
        following = min(aligned(nul + 1, 4), end)
        if bank.filename_widths and next(widths, None) != following - cursor:
            fail(f'{bank.where}: filename at {offset:#x} disagrees with its C field in {bank.name}')
        if any(data[nul + 1:following]):
            fail(f'{bank.where}: nonzero alignment after a filename in {bank.name}')
        yield BankString(base + cursor, bank.symbol if not offset else f'{bank.symbol}+{offset}', BANK_STRINGS[bank.kind])
        cursor = following


def check_record(obj, data, start, names):
    """Check dimensions in the native record against the declared arrays it owns."""
    def check_count(field, count, stride):
        target = names.get(field)
        if not target or target.size != stride * count:
            fail(f'{obj.where}: {obj.name} requires {count} elements at {field}')

    if obj.kind == 'texture_filename':
        text = data[start:start + obj.size]
        length = int(obj.targets[0]) if obj.targets else obj.size
        if text[length - 1:length] != b'\0' or b'\0' in text[:length - 1] or any(text[length:]):
            fail(f'{obj.where}: {obj.name} must end at its first NUL with zero-filled array padding')
    elif obj.kind == 'padded_texture_filename':
        name = data[start:start + obj.size]
        if not re.fullmatch(rb'[!-~][ -~]{27}\0{4}', name):
            fail(f'{obj.where}: {obj.name} must contain a 28-character padded PVM name and four NULs')
    elif obj.kind in ('land_table', 'scenery_land'):
        count, drawn, _, motions = struct.unpack_from('<4h', data, start)
        entries = names.get(obj.targets[0])
        if obj.kind == 'scenery_land':
            # The background renderer uses count culling records. Its authored
            # object count can include extra transforms sharing a record.
            counts = (count, drawn)
        else:
            counts = (count,) if drawn < 0 else (drawn, count - drawn)
        if (not entries or counts != tuple(map(int, entries.targets))
                or motions != 0 or obj.targets[1] != '-'):
            fail(f'{obj.where}: {obj.name} disagrees with its static land entry counts')
    elif obj.kind == 'texture_list':
        count, = struct.unpack_from('<I', data, start + 4)
        if len(obj.targets) == 1:
            check_count(obj.targets[0], count, KINDS['texture_names'].size)
        else:
            array, first, length = obj.targets
            names_array = names.get(array)
            if (not names_array or count != int(length)
                    or int(first) + count > names_array.size // KINDS['texture_names'].size):
                fail(f'{obj.where}: {obj.name} selects an invalid range of {array}')
    elif obj.kind == 'basic_model':
        points, = struct.unpack_from('<i', data, start + 8)
        meshsets, materials = struct.unpack_from('<HH', data, start + 20)
        check_count(obj.targets[0], points, KINDS['points'].size)
        if obj.targets[1] != '-':
            check_count(obj.targets[1], points, KINDS['points'].size)
        check_count(obj.targets[2], meshsets, KINDS['basic_meshsets'].size)
        check_count(obj.targets[3], materials, KINDS['basic_materials'].size)


def array_pointers(obj, data, start, addresses):
    """Resolve each original address only to a declared object of the field's type."""
    if obj.kind == 'texture_names' and obj.targets[-1:] == ('empty',):
        if any(data[start:start + obj.size]):
            fail(f'{obj.where}: {obj.name} needs initially empty texture slots')
        return
    stride = KINDS[obj.kind].size
    drawn = int(obj.targets[0]) if obj.kind == 'land_entries' and len(obj.targets) == 2 else None
    for index in range(obj.size // stride):
        record = start + index * stride
        element = data[record:record + stride]
        if obj.kind in ('land_entries', 'scenery_entries'):
            flags, = struct.unpack_from('<I', element, LAND_OBJECT.offset)
            visible = bool(flags & LAND_DRAWN)
            if obj.kind == 'scenery_entries' and not visible:
                fail(f'{obj.where}: {obj.name}[{index}] needs a visible background object')
            if drawn is not None and visible != (index < drawn):
                fail(f'{obj.where}: {obj.name}[{index}] disagrees with the drawn entry range')
        elif obj.kind == 'texture_pvms':
            # texture_load_shared stops at the first null list. A live list
            # can have no archive name, in which case it supplies existing handles.
            name, texlist = struct.unpack_from('<II', element)
            last = (index + 1) * stride == obj.size
            if (texlist == 0) != last or (last and name):
                fail(f'{obj.where}: {obj.name} needs a final empty PVM entry after its texture lists')
        elif obj.kind == 'texture_names':
            attr, texaddr = struct.unpack_from('<II', element, 4)
            if attr or texaddr:
                fail(f'{obj.where}: {obj.name}[{index}] must be an unloaded named texture')
        for offset, field, kind in KINDS[obj.kind].pointers:
            if isinstance(kind, Choice):
                kind = kind.of(element)
            value, = struct.unpack_from('<I', element, offset)
            target = addresses.get(value)
            padded_name = field == 'filename' and target and target.kind == 'padded_texture_filename'
            if value and (not target or (target.kind != kind and not padded_name)):
                fail(f'{obj.where}: {obj.name}[{index}].{field} points to {value:#010x}, '
                     f'expected {kind or "a null pointer"}')
            if not value and field in ('object', 'filename', 'meshes'):
                fail(f'{obj.where}: {obj.name}[{index}].{field} needs a {kind}')
            yield index * stride + offset, target
