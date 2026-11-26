"""Build an image from its linked sources, the disc's ranges and the linker's
fill, checking each against the original.

    python3 tools/splice.py 1st_read orig/1ST_READ.BIN build/1ST_READ.BIN
    python3 tools/splice.py --check [IMAGE]

ORIGINAL is the image's original bytes: its file from the disc, decompressed
where the disc stores it as PRS. The splicer starts from zeros, never from a
copy of ORIGINAL, and writes only what spans() gives a writer. It fails, and
writes no image, where:

- two spans write one byte, or a byte of a section that isn't bss has no
  writer;
- a section of stored data starts off a word, where no object's alignment
  starts one;
- a linked source's size isn't its rows';
- a disc row holds only zeros where the linker's fill would lie, between
  two sources or at a section's edge, or a disc row inside a source's row
  holds bytes the linked source matches, so the row has no reason left;
- a linked source differs from ORIGINAL outside the source's disc rows;
- the linker's fill or stored bss isn't zeros in ORIGINAL, or ORIGINAL
  stores a byte no section holds;
- the bytes counts() gives each kind of writer don't sum to ORIGINAL's
  size.

--check needs no disc and writes nothing. For IMAGE, or for every image, it
fails as a splice does, short of what needs ORIGINAL, such as a disc row of
zeros where fill would lie. It counts no bytes, so it reads no listing: a
build can still fail on one that is missing.

TOOLCHAIN.md#what-writes-each-byte gives the writers.
"""

import argparse
import hashlib
from pathlib import Path
import re

import csource
import inline
import sysrof
from body_data import locations
from config import DISC, LINKS, OBJECTS, Image, address, aligned, code, content, fail, hexa, images, native_path, rows, save, stub_sources, zeroed
from kinds import KINDS

# what writes a span, beside DISC: a source's object, or the linker
SOURCE, FILL = 'source', 'fill'
# what counts() calls a source's bytes, and the bss the file stores
C, ASSEMBLY, GENERATED, STORAGE = 'C', 'assembly', 'generated C', 'storage'
# the alignment SHC gives each string of a pool of filenames
FILENAME_ALIGNMENT = KINDS['texture_filename'].alignment
# every object the matching build makes aligns its stored data to 4 at least,
# and a section starts on its first object's alignment
DATA_ALIGNMENT = 4
# the sections whose rows the tables start at their first object
# (TOOLCHAIN.md#images). An SDK section may start where the image's own
# record puts it, before its first object: Kindergarten's DSG_END_ starts
# 4 bytes before its first, at the bound its table at 0x0c9fb670 holds
CONVENTIONAL = ('C', 'D', 'DATA')


def linked(image, source):
    """Where link.py writes source's linked source for image."""
    return native_path(LINKS / image.name / Path(source).with_suffix('.bin'), source, image.stubs)


def declared(source, stubs=frozenset()):
    """The section table of source's object, with the object's path. A source
    without an object under build/obj/ fails."""
    obj = native_path(OBJECTS / Path(source).with_suffix('.obj'), source, stubs)
    if not obj.is_file():
        fail(f'{source}: no object at {obj}, so build it first')
    return sysrof.sections(obj), obj


def alignment(source, section, stubs=frozenset()):
    """The alignment source's object declares for section. A source without
    an object under build/obj/ fails, as does an object without the section."""
    sections, obj = declared(source, stubs)
    if section not in sections:
        fail(f'{obj}: no {section} section')
    return sections[section].alignment


def assembly(source, stubs=frozenset()):
    """The code of source's object that is assembly, as {section: [(start,
    end), ...]} in offsets from each code section's start: all of a .src
    file's, and of a C source's the function SHC wrote out for each inline
    body, each copy of its lines, and the instructions and initialized data
    of #pragma asm blocks. The listing beside the object places each range. A source that holds
    assembly fails without its object under build/obj/."""
    text = '' if source.endswith('.src') else content(source).decode('latin-1')
    names = inline.bodies(text)
    blocks = csource.assembly_blocks(text, directives=bool(names))
    if not names and not blocks and not source.endswith('.src'):
        return {}
    sections, obj = declared(source, stubs)
    sizes = {name: section.size for name, section in sections.items() if code(name)}
    if source.endswith('.src'):
        return {name: [(0, size)] for name, size in sizes.items()}
    listing = obj.with_suffix('.lis')
    placed = content(listing).decode('latin-1')
    raw, stops = inline.assembled(blocks, placed, listing, include_pools=not names) if blocks else ({}, {})
    if not names:
        return raw
    ranges = inline.joined(raw, inline.written(names, placed, sizes, stops), inline.copied(text, placed, sizes, listing, raw))
    return inline.pooled(placed, ranges, listing)


def empty_constant_section(image, first, right, empty, word, where):
    """Check the empty intermediate section's order, alignment, and every
    input, plus the original startup relocation to the following data."""
    left = first.section
    sections, startup = declared(first.writer, image.stubs)
    order = list(sections)
    index = order.index(left)
    if order[index:index + 3] != [left, empty, right] or sections[empty] != sysrof.Section(0, 32, False):
        fail(f'{where}: startup must declare empty {empty} aligned to 32 between {left} and {right}')
    if left == 'C' and word != first.start:
        fail(f'{where}: the first word of C must hold a relocation to STARTOF D')
    if not first.start <= word <= first.end - 4 or sysrof.word_target(startup, left, word - first.start) != ('start', right):
        fail(f'{where}: the startup {left} word must hold a relocation to STARTOF {right}')
    for source in image.sources():
        other = declared(source, image.stubs)[0].get(empty)
        if other and (other.size or other.alignment > sections[empty].alignment):
            fail(f'{where}: {source} changes the empty {empty} section')
    return sections[empty].alignment


def overlay_data_start(image, beginning, word, where):
    """An ordinary overlay takes the start of its first D input through a
    private section address or an export. Check the linked word separately."""
    if not any(name == 'DHEAD' for name, _, _ in image.sections):
        fail(f'{where}: the direct C to D boundary requires an overlay header')
    if any('C32' in declared(source, image.stubs)[0] for source in image.sources()):
        fail(f'{where}: the overlay declares C32 between C and D')
    caller = next((row for row in image.placements if row.writer != DISC and row.section in ('P', 'D')
                   and row.start <= word and word + 4 <= row.end), None)
    if not caller or word & 3:
        fail(f'{where}: a native code or data input must own the aligned pointer to D')
    section = image.input_section(caller)
    base = next(start for name, start, _ in image.ranges(caller.writer) if name == section)
    target = sysrof.word_target(declared(caller.writer, image.stubs)[1], section, word - base)
    if caller.writer == beginning.writer:
        if target != ('address', 'D'):
            fail(f'{where}: the word at {hexa(word)} must relocate against its own D input')
    elif not target or target[0] != 'name' or sysrof.exports(declared(beginning.writer, image.stubs)[1]).get(target[1]) != ('D', 0):
        fail(f'{where}: the word at {hexa(word)} must reference an export at the start of the first D input')


def complete_inputs(image, left, right, digest, target, word, where):
    """Reconstruct every ordinary input and its alignment in the whole
    link. Each line in the receipt holds source, start, end and decimal
    alignment, separated by tabs. Return the gap and any alignment imposed
    by the evidenced empty section between its two stored sections."""
    if (left, right) not in (('P', 'C'), ('C', 'D'), ('CSG', 'DSG'), ('DSG', 'DSG32'), ('DHEAD', 'P')):
        fail(f'{where}: unsupported complete input boundary from {left} to {right}')
    limits = {name: (start, end) for name, start, end in image.sections}
    cursor, successor = limits[left]
    roster = hashlib.sha256()
    inputs = [row for row in image.placements if row.section == left]
    if not inputs or inputs[0].start != cursor:
        fail(f'{where}: no first input at the start of {left}')
    if left == 'DHEAD' and len(inputs) != 1:
        fail(f'{where}: the overlay header must be one complete DHEAD input')
    for row in inputs:
        if row.writer == DISC or image.input_section(row) != left:
            fail(f'{where}: {left} requires a complete ordinary input at {hexa(row.start)}')
        sections, obj = declared(row.writer, image.stubs)
        native = sections.get(left)
        if not native or native.code != code(left) or native.size != row.end - row.start:
            fail(f'{where}: {row.writer} does not supply its entire native {left} section')
        if aligned(cursor, native.alignment) != row.start:
            fail(f'{where}: {obj} does not start on its native alignment after {hexa(cursor)}')
        roster.update(f'{row.writer}\t{hexa(row.start)}\t{hexa(row.end)}\t{native.alignment}\n'.encode())
        cursor = row.end
    if roster.hexdigest() != digest:
        fail(f'{where}: the ordered {left} inputs differ from the complete link')

    beginning = next((row for row in image.placements if row.start == successor and row.section == right
                      and row.writer != DISC and image.input_section(row) == right), None)
    if not beginning:
        fail(f'{where}: no ordinary input begins {right} at {hexa(successor)}')
    sections, obj = declared(beginning.writer, image.stubs)
    native = sections.get(right)
    if not native or native.code != code(right) or native.size < 4 or native.size != beginning.end - successor:
        fail(f'{where}: {beginning.writer} must supply the complete native {right} input')
    intermediate = 1
    linked_address = successor
    if left == 'C' and target == 'D':
        overlay_data_start(image, beginning, word, where)
    elif left in ('C', 'CSG'):
        empty = left + '32'
        if target != empty:
            fail(f'{where}: the complete {left} witness requires the intervening {empty} section')
        intermediate = empty_constant_section(image, inputs[0], right, empty, word, where)
    else:
        export = sysrof.exports(obj).get(target)
        if left == 'DHEAD':
            if not export or export[0] != 'P' or export[1] & 1 or export[1] >= native.size:
                fail(f'{where}: {target} must be an instruction in the first native P input')
            linked_address += export[1]
        elif export != (right, 0):
            fail(f'{where}: {target} must begin the complete native {right} input')
        caller = next((row for row in image.placements if row.writer not in (DISC, beginning.writer)
                       and row.start <= word and word + 4 <= row.end), None)
        if not caller or (left == 'DHEAD' and caller != inputs[0]):
            fail(f'{where}: no independent source writes the startup word at {hexa(word)}')
        section = image.input_section(caller)
        base = next(start for name, start, _ in image.ranges(caller.writer) if name == section)
        expected = ('start', right) if left == 'DSG' else ('name', target)
        if sysrof.word_target(declared(caller.writer, image.stubs)[1], section, word - base) != expected:
            fail(f'{where}: the linked word at {hexa(word)} needs an independent relocation to {expected[1]}')
    if cursor >= successor or aligned(aligned(cursor, intermediate), native.alignment) != successor:
        fail(f'{where}: the complete {left} section does not align to {hexa(successor)}')
    return (cursor, successor), intermediate, linked_address


def section_fills(image, sources, read):
    """Gaps across adjacent output sections. Two words of linked sources
    must hold the bounds.txt edges, whatever source writes them, or complete
    native inputs, their section alignment and the relocated startup word
    must establish both edges."""
    path = image.path / 'fill.txt'
    if not path.exists():
        return {}
    bounds = {name: (low, high) for name, low, high in image.bounds()}
    adjacent = {(left[0], right[0]) for left, right in zip(image.sections, image.sections[1:])
                if left[2] == right[1]}
    fills, links = {}, {}
    for where, fields in rows(path):
        if tuple(fields[:2]) not in adjacent:
            fail(f'{where}: expected adjacent sections and their witnesses')
        left, right = fields[:2]
        intermediate = 1
        if len(fields) == 4:
            if left not in bounds or right not in bounds:
                fail(f'{where}: {left} and {right} need bounds.txt rows')
            edges = bounds[left][1], bounds[right][0]
            witnesses = zip(fields[2:], edges)
        elif len(fields) == 6 and fields[2] == 'inputs':
            local_data = (left, right, fields[4]) == ('C', 'D', 'D')
            if right in ('D', 'DSG', 'DSG32') and right not in bounds and not local_data:
                fail(f'{where}: STARTOF {right} needs its bounds.txt row')
            edges, intermediate, linked_address = complete_inputs(image, left, right, *fields[3:5], address(fields[5], where), where)
            if right in bounds and bounds[right][0] != edges[1]:
                fail(f'{where}: the complete input witness differs from the {right} bound')
            if left == 'DSG' and bounds.get(left, (None, None))[1] != edges[0]:
                fail(f'{where}: the complete inputs differ from the DSG bound')
            witnesses = [(fields[5], linked_address)]
            if not any(kind == SOURCE and start == edges[1] and edges[1] + 4 <= end for start, end, kind, _ in sources):
                fail(f'{where}: no source writes the first word of {right}')
        else:
            fail(f'{where}: expected END_WORD START_WORD or inputs SHA256 EXPORT START_WORD')
        if edges[0] >= edges[1] or (left, right) in fills:
            fail(f'{where}: {left} to {right} must name a single nonempty gap')
        for field, edge in witnesses:
            word = address(field, where)
            owner = next((source for start, end, kind, source in sources
                          if kind == SOURCE and start <= word and word + 4 <= end), None)
            if not owner:
                fail(f'{where}: no source writes the word at {field}')
            if owner not in links:
                links[owner] = parts(image, owner, read)
            start = max(low for low in links[owner] if low <= word)
            data = links[owner][start]
            if int.from_bytes(data[word - start:word - start + 4], 'little') != edge:
                fail(f'{where}: the linked word at {field} does not hold {hexa(edge)}')
        fills[left, right] = edges, intermediate
    return fills


def first_alignment(image, placements, align, section, start):
    """The alignment of the object that starts section at start: that of the
    source whose row starts there, as align() gives it. None where no row
    starts there, or a disc row does, which shows no object's alignment.
    placements are image's, or a view of them."""
    first = min((row for row in placements if row.section == section), key=lambda row: row.start, default=None)
    if first is None or first.start != start or first.writer == DISC:
        return None
    return align(first.writer, image.input_section(first))


def section_edges(image, align, witnessed, sections=None, placements=None):
    """The linker's fill at each edge between two sections, as (start, end)
    ranges. A section starts on its first object's alignment, so the gap
    from a section's last placement, a source's, to the next section's start
    is fill where a source starts the next section and its alignment rounds
    the placement's end up to that start. witnessed holds the pairs of
    sections that section_fills() witnesses, which take their gap there. No
    fill lies at an edge of bss, which no object writes and an image may not
    store. sections and placements are image's, or a view of them without a
    disc row, as stale_edges() asks."""
    sections = sorted(image.sections if sections is None else sections, key=lambda row: row[1])
    placements = image.placements if placements is None else placements
    found = []
    for (name, _, _), (later, start, _) in zip(sections, sections[1:]):
        held = [row for row in placements if row.section == name]
        if zeroed(name) or zeroed(later) or (name, later) in witnessed or not held:
            continue
        last = max(held, key=lambda row: row.end)
        if last.writer == DISC or last.end >= start:
            continue
        alignment = first_alignment(image, placements, align, later, start)
        if alignment is not None and aligned(last.end, alignment) == start:
            found.append((last.end, start))
    return found


def stale_edges(image, align, witnessed, alone):
    """Each of alone, the disc rows that lie in no source's row, that holds
    data and only the fill section_edges() finds without it: a row that ends
    its section, a row that opens a CONVENTIONAL section, which would start
    at the next placement, or a row that is its section's whole extent. A
    row of code is code that doesn't match yet, whatever its size."""
    stale = []
    for row in alone:
        section = next(each for each in image.sections if each[0] == row.section)
        name, low, high = section
        opens = row.start == low and row.end < high
        if code(name) or low < row.start and row.end < high or opens and name not in CONVENTIONAL:
            continue
        sections = [each for each in image.sections if each != section]
        if opens:
            sections.append((name, row.end, high))
        elif low < row.start:
            sections.append(section)
        placements = [each for each in image.placements if each != row]
        if (row.start, row.end) in section_edges(image, align, witnessed, sections, placements):
            stale.append(row)
    return stale


def filename_pools(image, placements):
    """The placements in C that each hold one whole pool of filenames, as
    image's data.txt declares it in a row at the placement's start: one
    texture_filename, or a layout of them. Each string starts on a word, the
    first at the placement's start, and the last ends the placement.

    It opens data.txt only where a placement in C starts on a word. It
    parses only the rows at those starts, and of a layout such a row
    places, its rows up to the first that isn't a texture_filename. It
    leaves the rest of the table to tools/content.py.

    A texture_filename it parses fails, naming its path:line, where its
    fields aren't a byte count, or a capacity and a string length, or where
    the string, its null included, is longer than the capacity. So does
    a line of data.txt that isn't UTF-8. A layout it can't read fails,
    naming it."""
    starts = {hexa(row.start): row for row in placements if row.section == 'C' and not row.start % FILENAME_ALIGNMENT}
    path = image.path / 'data.txt'
    if not starts or not path.is_file():
        return set()
    pools = set()
    for where, fields in rows(path):
        row = starts.get(fields[0])
        if row is None:
            continue
        if len(fields) == 3 and fields[1] == 'layout':
            declared = rows(Path('layouts') / (fields[2] + '.txt'))
        else:
            declared = [(where, fields[1:])]
        end = row.start
        for line, member in declared:
            if member[1:2] != ['texture_filename']:
                break
            if len(member) not in (3, 4) or not all(re.fullmatch(r'[1-9][0-9]*', field) for field in member[2:]):
                fail(f'{line}: a texture_filename needs its byte count, or its capacity and string length')
            if len(member) == 4 and int(member[3]) > int(member[2]):
                fail(f'{line}: a texture_filename string, its null included, is longer than its capacity')
            end = aligned(end, FILENAME_ALIGNMENT) + int(member[2])
        else:
            if end == row.end:
                pools.add(row)
    return pools


def spans(image, align=None, read=content, original=None):
    """What writes each range of image the tables account for, as (start,
    end, kind, source) in address order. kind is SOURCE, DISC or FILL. source
    is the source whose row holds the span, or None.

    A source's row gives SOURCE spans, but for the disc rows inside it. A disc
    row gives a DISC span, with the source whose row it lies in, if any. Bss
    gives no span: no object writes it.

    The linker's fill runs from the end of a source's row to the next row of
    its section, where that row is another source's first there and starts at
    that end rounded up to the alignment its object declares for the section,
    which align(source, section) gives. Across sections the two edges must
    also be witnessed by section_fills(), or the gap must be the one
    section_edges() finds at a section's start. A disc row that
    filename_pools() finds witnesses its own edges within C. An opaque disc
    row supplies neither edge. Given original, the image's original bytes, a
    disc row that would hold just the linker's fill fails where its bytes
    are zeros: the alignment shows only that fill could lie there. A section
    of stored data whose row starts off DATA_ALIGNMENT fails."""
    for name, start, _ in image.sections:
        if start % DATA_ALIGNMENT and not code(name) and not zeroed(name):
            fail(f'{image.splits}: section {name} starts at {hexa(start)}, and a section of stored data starts on '
                 f'{DATA_ALIGNMENT}')
    if align is None:
        align = lambda source, section: alignment(source, section, image.stubs)
    found, first = [], {}
    rows = sorted((row for row in image.placements if row.writer != DISC and not zeroed(row.section)),
                  key=lambda row: row.start)
    nested = set()
    for row in rows:
        first.setdefault((row.writer, image.input_section(row)), row.start)
        at = row.start
        for start, end in image.unmatched(row.writer):
            if row.start <= start and end <= row.end:
                nested.add((start, end))
                found += [(at, start, SOURCE, row.writer)] * (at < start) + [(start, end, DISC, row.writer)]
                at = end
        found += [(at, row.end, SOURCE, row.writer)] * (at < row.end)
    boundaries = section_fills(image, found, read)
    alone = [row for row in image.disc() if (row.start, row.end) not in nested]
    found += [(row.start, row.end, DISC, None) for row in alone]
    tiles = sorted(rows + alone, key=lambda row: row.start)

    pools = filename_pools(image, alone)

    def filled(before, after):
        # whether the linker starts after's object where after starts, past before
        if before.writer == DISC and (before not in pools or before.section != after.section):
            return False
        if after.writer == DISC:
            return (after in pools and before.section == after.section
                    and aligned(before.end, FILENAME_ALIGNMENT) == after.start)
        if first[after.writer, image.input_section(after)] != after.start:
            return False
        cursor = before.end
        if before.section != after.section:
            boundary = boundaries.get((before.section, after.section))
            if not boundary or boundary[0] != (cursor, after.start):
                return False
            cursor = aligned(cursor, boundary[1])
        return aligned(cursor, align(after.writer, image.input_section(after))) == after.start

    for before, after in zip(tiles, tiles[1:]):
        if before.end < after.start and filled(before, after):
            found.append((before.end, after.start, FILL, None))
    found += [(start, end, FILL, None) for start, end in section_edges(image, align, boundaries.keys())]
    if original is not None:
        zeros = lambda row: not any(original[row.start - image.base:row.end - image.base])
        stale = [f"{image.where[row]}: the {DISC} row from {hexa(row.start)} to {hexa(row.end)} holds only the "
                 f"linker's fill before {after.writer}, so it has no reason left"
                 for before, row, after in zip(tiles, tiles[1:], tiles[2:])
                 if row.writer == DISC and before.end == row.start and row.end == after.start
                 and row.section in (before.section, after.section) and filled(before, after) and zeros(row)]
        stale += [f"{image.where[row]}: the {DISC} row from {hexa(row.start)} to {hexa(row.end)} holds only the "
                  f"linker's fill at an edge of {row.section}, so it has no reason left"
                  for row in stale_edges(image, align, boundaries.keys(), alone) if zeros(row)]
        if stale:
            fail(*stale)
    gaps = {(start, end) for start, end, kind, _ in found if kind == FILL}
    for (left, right), (edges, _) in boundaries.items():
        if edges not in gaps:
            fail(f"{image.splits}: the gap from {left} to {right} is not the next source's alignment")
    return sorted(found)


def parts(image, source, read):
    """source's linked source for image, cut into the ranges it holds, as
    {start: bytes}. read(path) gives the linked source's bytes. One whose
    size isn't its ranges' fails."""
    held = image.held(source)
    data, size = read(linked(image, source)), sum(end - start for _, start, end in held)
    if len(data) != size:
        fail(f"{linked(image, source)}: {len(data):#x} bytes, and {source}'s rows hold {size:#x}; link it again")
    found, at = {}, 0
    for _, start, end in held:
        found[start] = data[at:at + end - start]
        at += end - start
    return found


def check(image, align=None, read=content, original=None):
    """Fail where image's spans write a byte twice or leave one of a section
    that isn't bss unwritten, or where a linked source's size isn't its
    rows': what a splice fails on that needs no original. align and
    original are as spans() takes them, and read(path) gives a linked
    source's bytes. Returns each linked source cut into its rows' parts, as
    parts() gives them."""
    base, end = image.base, max(high for _, _, high in image.sections)
    written, links = bytearray(end - base), {}
    for start, stop, _, source in spans(image, align, read, original):
        low, high = start - base, stop - base
        if any(written[low:high]):
            fail(f'{image.splits}: two rows write bytes of {hexa(start)} to {hexa(stop)}')
        written[low:high] = b'\x01' * (high - low)
        if source and source not in links:
            links[source] = parts(image, source, read)
    for name, start, stop in image.sections:
        if zeroed(name):
            continue
        for first, last in unwritten(written, start - base, stop - base):
            fail(f'{image.name}: nothing writes {hexa(base + first)} to {hexa(base + last)}, '
                 f'{"code" if code(name) else "data"} in {name}')
    return links


def splice(image, original, align=None, read=content):
    """image's bytes, built from its spans and checked against original, the
    image's original bytes. align is as spans() takes it, and read(path)
    gives a linked source's bytes. It fails as the module's docstring says,
    but for the sum of the counts, which main() checks."""
    links = check(image, align, read, original)
    base, size = image.base, len(original)
    out, written = bytearray(size), bytearray(size)
    for start, end, kind, source in spans(image, align, read, original):
        low, high = start - base, end - base
        if low < 0 or high > size:
            fail(f"{image.splits}: {hexa(start)} to {hexa(end)} lies outside the image's {size:#x} stored bytes")
        written[low:high] = b'\x01' * (high - low)
        theirs = original[low:high]
        if source:
            row = max(first for first in links[source] if first <= start)
            ours = links[source][row][start - row:end - row]
            if kind == SOURCE and ours != theirs:
                differs = next(at for at in range(len(ours)) if ours[at] != theirs[at])
                fail(f"{image.name}: {source}: its byte at {hexa(start + differs)} differs from the disc's")
            if kind == DISC and ours == theirs:
                fail(f'{image.splits}: {source} matches the disc from {hexa(start)} to {hexa(end)}, '
                     f'so the {DISC} row there has no reason left')
        if kind == SOURCE:
            out[low:high] = ours
        elif kind == DISC:
            out[low:high] = theirs
    for name, start, end in image.sections:
        low, high = max(start - base, 0), min(end - base, size)
        if zeroed(name) and low < high:
            written[low:high] = b'\x01' * (high - low)
    for first, last in unwritten(written, 0, size):
        fail(f'{image.name}: no section row holds {hexa(base + first)} to {hexa(base + last)}')
    if out != original:
        differs = next(at for at in range(size) if out[at] != original[at])
        fail(f"{image.name}: the disc's byte at {hexa(base + differs)} isn't zero, as fill and bss are")
    return bytes(out)


def unwritten(written, low, high):
    """The runs of zeros in written[low:high], as (start, end) offsets."""
    runs, at = [], low
    while (start := written.find(0, at, high)) >= 0:
        at = start
        while at < high and not written[at]:
            at += 1
        runs.append((start, at))
    return runs


def generated_ranges(image, source):
    """The ranges of image's initialized data that source's offsets table
    gives a datum or a text, as (start, end): the bytes whose C the build
    generates from the disc's. A source without a table gives none."""
    table = Path('offsets') / Path(source).with_suffix('.txt')
    if not table.is_file():
        return []
    return [(datum.at, datum.at + datum.size) for datum in locations(image, Path(source), table).values()
            if not code(datum.section) and not zeroed(datum.section)]


def overlap(start, end, ranges):
    """How many bytes from start to end the (low, high) ranges hold, no two
    of which overlap."""
    return sum(max(0, min(end, high) - max(start, low)) for low, high in ranges)


def counts(image, align=None, assembled=None, stored=None):
    """The bytes of image's spans in code and in data, as two {label: bytes}
    maps. A DISC or FILL span's label is its kind. A SOURCE span's code is
    ASSEMBLY where assembled(source) puts assembly, and C elsewhere. Its data
    is GENERATED where generated_ranges() puts a datum or a text of the
    source's, and elsewhere ASSEMBLY for a .src file and C for a C source.

    Given stored, the image's stored size, the data's map counts the bss the
    file stores as STORAGE, and counts() fails where the two maps don't sum
    to stored."""
    if assembled is None:
        assembled = lambda source: assembly(source, image.stubs)
    found, generated, assemblies = ({}, {}), {}, {}
    for start, end, kind, source in spans(image, align):
        # fill between two sections' rows is the earlier section's
        section = max((row for row in image.sections if row[1] <= start), key=lambda row: row[1])[0]
        tally = found[0 if code(section) else 1]
        if kind != SOURCE:
            tally[kind] = tally.get(kind, 0) + end - start
            continue
        if code(section):
            # the link starts the object's section at the source's first row in it
            ranges = image.ranges(source)
            input_name = next(name for name, low, high in ranges if low <= start < high)
            base = next(low for name, low, _ in ranges if name == input_name)
            if source not in assemblies:
                assemblies[source] = assembled(source)
            size = overlap(start - base, end - base, assemblies[source].get(input_name, []))
            labelled = ((ASSEMBLY, size), (C, end - start - size))
        else:
            if source not in generated:
                generated[source] = generated_ranges(image, source)
            size = overlap(start, end, generated[source])
            labelled = ((GENERATED, size), (ASSEMBLY if source.endswith('.src') else C, end - start - size))
        for label, count in labelled:
            if count:
                tally[label] = tally.get(label, 0) + count
    if stored is not None:
        bss = [(low, high) for name, low, high in image.sections if zeroed(name)]
        if held := overlap(image.base, image.base + stored, bss):
            found[1][STORAGE] = held
        total = sum(found[0].values()) + sum(found[1].values())
        if total != stored:
            fail(f'{image.name}: its categories count {total:,} of its {stored:,} stored bytes')
    return found


def main(argv=None):
    parser = argparse.ArgumentParser(prog='splice', description=__doc__.split('\n\n')[0])
    parser.add_argument('image', metavar='IMAGE', nargs='?', help='the image to build, config/IMAGE/')
    parser.add_argument('original', metavar='ORIGINAL', nargs='?', help="the image's original bytes")
    parser.add_argument('out', metavar='OUT', nargs='?', help='where to write the image')
    parser.add_argument('--verbose', action='store_true', help='print the bytes each kind of writer gives')
    parser.add_argument('--check', action='store_true',
                        help="check IMAGE's writers, or every image's, without the disc, and write nothing")
    args = parser.parse_args(argv)
    if args.check:
        if args.original or args.verbose:
            parser.error('--check takes IMAGE alone, or nothing')
        stubs = stub_sources()
        for name in [args.image] if args.image else images():
            check(Image(name, stubs))
        return
    if not args.out:
        parser.error('give IMAGE, ORIGINAL and OUT, or --check')
    image = Image(args.image)
    built = splice(image, content(args.original))
    tallies = counts(image, stored=len(built))
    save(args.out, built)
    print(f'  splice {args.out}')
    if args.verbose:
        for where, tally in zip(('code', 'data'), tallies):
            for label, size in sorted(tally.items()):
                print(f'         {size:8} {where} bytes from {label}')


if __name__ == '__main__':
    main()
