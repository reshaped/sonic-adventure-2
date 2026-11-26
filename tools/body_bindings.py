"""Source facts a native listing cannot supply until SHC has compiled the stub.

A text length includes its terminating NUL. A pointer row is a relocation:
the datum and the offset of its pointer field, then the name the field
points at and how many bytes past that name's start, as NAME+0x48c, with
the + and the hex left out at 0. A pointer field that no row names holds
null. tools/relocate.py rewrites a row that spells its target as C into
this form.
A selector keeps an integer field needed to dispatch or navigate in a disc-free build.
Its symbolic value is checked against the original before it can be preserved.
A stop names a signed integer array and a negative C constant, or an array
of structs with a signed integer member selected before the constant.
Only the final original record holds that stop. The first stub record holds
it so a sentinel-based consumer sees an empty table.
An include names shared source facts in layouts/NAME.bindings. Coordinates
remain in each compiling member's independently measured offset table.
"""
import re
from pathlib import Path

from config import NAME, address, fail, rows


# a subobject: its datum's name, then the indexes and members that lead to
# it, as buttons[1].id
SUBOBJECT = re.compile(rf'({NAME.pattern})((?:\[(?:0|[1-9][0-9]{{0,8}})\]|\.{NAME.pattern})*)')
FIELD = re.compile(rf'\[([0-9]+)\]|\.({NAME.pattern})')
# a relocation: the name, then a + and the bytes past its start in hex, which
# a relocation at the name's start leaves out
RELOCATION = re.compile(rf'({NAME.pattern})(?:\+0x([1-9a-f][0-9a-f]{{0,7}}))?')


def pointer(fields, pointers, where):
    """Read a pointer row, fields, into pointers: its datum and its field's
    offset, to the name the field points at and how many bytes past that
    name's start, as (name, addend)."""
    if len(fields) < 4 or not NAME.fullmatch(fields[1]):
        fail(f'{where}: expected pointer NAME OFFSET TARGET[+0xADDEND]')
    relocation = RELOCATION.fullmatch(fields[3]) if len(fields) == 4 else None
    if not relocation:
        fail(f'{where}: expected a target NAME or NAME+0xADDEND, which tools/relocate.py writes for a target spelled '
             'as C')
    key = fields[1], address(fields[2], where)
    if key in pointers:
        fail(f'{where}: repeated pointer {fields[1]}+{fields[2]}')
    pointers[key] = relocation[1], int(relocation[2] or '0', 16)


def path(source):
    return Path('offsets') / source.with_suffix('.bindings')


def present(table):
    """Whether anything stands at table, a dangling link among them. An
    absent table binds nothing; rows() fails on anything else there that
    isn't a file, naming its path."""
    return table.exists() or table.is_symlink()


def binding_rows(table, shared):
    """Expand shared source facts in place so text ordinals agree across the build routes."""
    for where, fields in rows(table):
        if fields[0] != 'include':
            yield where, fields
            continue
        if len(fields) != 2 or not NAME.fullmatch(fields[1]):
            fail(f'{where}: expected include NAME')
        target = Path('layouts') / (fields[1] + '.bindings')
        if target in shared:
            fail(f'{where}: repeated binding include {target}')
        shared.add(target)
        yield from binding_rows(target, shared)


def dependencies(source):
    """Shared bindings read in addition to the translation unit's optional binding table."""
    shared = set()
    table = path(source)
    if present(table):
        for _ in binding_rows(table, shared):
            pass
    return shared


def read_all(source):
    """The text widths, pointer rows, stops and selectors of source's table
    and the shared tables it includes, as (texts, pointers, stops,
    selectors)."""
    table = path(source)
    texts, pointers, stops, selectors = {}, {}, {}, {}
    if present(table):
        for where, fields in binding_rows(table, set()):
            if len(fields) == 3 and fields[0] == 'text':
                _, name, size = fields
                if (not NAME.fullmatch(name) or name != name.upper() or name in texts
                        or not re.fullmatch(r'[1-9][0-9]{0,8}', size)):
                    fail(f'{where}: invalid or repeated text name or size')
                texts[name] = int(size)
            elif fields[0] == 'pointer':
                pointer(fields, pointers, where)
            elif len(fields) == 3 and fields[0] == 'selector':
                _, field, value = fields
                target = SUBOBJECT.fullmatch(field)
                if (not target or not target[2] or field in selectors
                        or not (NAME.fullmatch(value) or re.fullmatch(r'0[xX][0-9a-fA-F]+|0|[1-9][0-9]*', value))):
                    fail(f'{where}: expected a distinct selector DATUM.FIELD VALUE, its VALUE a NAME or an '
                         'unsuffixed decimal or 0x hex integer')
                selectors[field] = value
            elif len(fields) in (3, 4) and fields[0] == 'stop':
                name, constant = fields[1], fields[-1]
                field = fields[2] if len(fields) == 4 else None
                if not all(NAME.fullmatch(word) for word in fields[1:]):
                    fail(f'{where}: expected stop TABLE [FIELD] CONSTANT_NAME, the negative constant by its name')
                if name in stops:
                    fail(f'{where}: repeated stop for {name}')
                stops[name] = field, constant
            else:
                fail(f'{where}: expected text NAME SIZE, pointer NAME OFFSET TARGET[+0xADDEND], '
                     'selector DATUM.FIELD VALUE, stop TABLE [FIELD] CONSTANT_NAME or include NAME')
    return texts, pointers, stops, selectors


def read(source):
    """The text widths and pointer rows a source's stub needs before SHC
    measures it, as (texts, pointers)."""
    texts, pointers, _, _ = read_all(source)
    return texts, pointers
