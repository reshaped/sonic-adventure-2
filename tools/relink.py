"""Link all of an image's objects in one pass, with R32's lnk, and compare the
result with the splicer's image.

    python3 tools/relink.py stg03 build/orig/SONIC2/STG03.BIN build/SONIC2/STG03.BIN

link.py links each source alone, with every name it takes from outside bound
at the address the tables give it, and the splicer joins the links, the
disc's ranges and the linker's fill. relink links, in one pass, the object
under build/obj/ of every source the image's splits.txt places, so that lnk
resolves each name one of them takes from another and lays out each section
itself:

- lnk lays out the pieces of one section name one after another, in input
  order. A run is the rows of one output section that follow one another
  with the same input section, and lnk links each run as one section. So
  relink gives lnk the objects in an order that keeps each run's rows in
  address order, and otherwise in table order. Where the runs disagree, as
  where one source's constants come before another's and its data after
  them, the waiting object first in table order goes next, and each run is
  cut where that order goes back: its rows from there on are a run of their
  own. A run of a section whose start or size an object asks is never cut,
  since a cut would change them, and where those runs alone disagree,
  relink fails, naming each step of the cycle.
- Each output section's runs follow one another, the first from its first
  row, or from the space reserved ahead of that row, and each empty section
  of bounds.txt starts at its bound. A run whose input section an earlier
  run already has links from copies of its objects, in which the section
  takes a new name: the name with the lowest number from 2 on after it
  that no section of the link has, as C2. Such a run of a section whose
  start or size an object asks fails, since lnk would lay it out apart.
- Where lnk would start a row's object short of the row, as past a disc row
  or bss the tables give no object, an object of relink's own just ahead
  of it in input order reserves the space between, so that lnk starts the
  object where the row does. Reserved space starts on LEAST, and a row
  that starts short of that fails, as does one off its object's alignment.
- A symbols object defines each name some object takes from outside and no
  object defines, at the address the tables give it: the names the image
  takes from other images, and the names its tables give inside its own
  sections, which relink binds by address, as each source's link does. A
  name the tables don't give fails.
- lnk keeps one module of a name, and SHC and asmsh name an object's module
  after the object's file, less its suffix. Where a source's file name,
  less its suffix, is an earlier source's, or one relink gives its own
  objects, symbols and hole1, hole2 and so on, relink links a copy of the
  source's object whose module is named apart: that name with the lowest
  number from 2 on after it that no module of the link has.
- align_section lets lnk join the pieces of a section that align it
  differently.

The link writes its files to build/relink/IMAGE/, among them lnk's map,
link.map. relink fails where the map holds no externally defined symbols
list. Where the list lacks a name of the image's symbols.txt that an object
of the link exports, or gives one another address than the name's row does,
relink fails, naming each such row. The map lists only the names objects
export, so a row whose name no object exports, such as a static's, goes
unchecked.

The image holds the bytes the link writes, then the disc's bytes in each
disc row, including a disc row inside a source's row. The disc's bytes come
from ORIGINAL, the image's original bytes, as the splicer takes them. A
byte neither writes is 0, as the linker's fill and stored bss are. The
image is as long as ORIGINAL, and a byte the link writes past either end
fails. relink compares the image with SPLICED, the splicer's, byte for
byte, and a difference fails, naming the first address that differs and
what the splicer says writes the byte there. Once every check passes,
relink writes the image to build/relink/IMAGE.bin, then, with --shift,
build/relink/IMAGE/fixed.txt, so a failed check leaves both as they were.

--shift adds the shift test. relink links the image twice more, each time
one of SHIFTS past its base, into build/relink/IMAGE/shift1/ and shift2/.
Each lnk section moves by the shift, and so does each name bound by address
that the image's own symbols.txt gives in its span. Each name the image
takes from the main executable stays where it is. So does a row of the
image's own table that main's table gives too, since the name is main's:
each stage's table repeats main's stage_slot at the stage's base. An object
that aligns a section to more than the shifts keep fails first, since lnk
would lay out the shifted image anew. Each aligned word must then move by
each shift, or keep its value under both. A word that a disc row gives a
byte of is the disc's, which no link moves, and the test passes over it.
Where words do neither, relink fails, naming how many and the first: its
address, its values and what the splicer says writes it.

The image's span runs from its base through the first boundary of
WORKSPACE_ALIGNMENT at or past the end of its last section. A stage's model
workspace starts on that boundary, as do the land buffers of four Chao
areas, and each moves with its image. A value lies in the span through any
of REGIONS. A word that keeps a value in the span is a candidate fixed
address: a number where a relocatable build needs a name, an address the
game fixes whatever the image's layout, or data that only looks like an
address. The first takes a name in the image's symbols.txt, which the
source that holds the word declares extern, so that the link writes the
address. config/kept.txt gives the reason each other candidate takes no
name. relink fails on a candidate it gives no reason, and on a row of the
image's that holds no candidate, naming each. fixed.txt holds a line for
each candidate: the image, the word's address, its value, what writes it
and the reason.

Where some run takes a new name, relink prints how many do, and how many of
those it cut. With --shift, it prints how many words each shift moves, and
how many keep a value in the span. --verbose lists each stand-in for what
the tree lacks: each name bound by address, each run of space reserved,
each run cut or given a new name, and each module named apart. WIBO and R32
name wibo and R32's directory, as for link.py.
"""

import argparse
import re
import struct
import sys
from pathlib import Path

import link
import splice
import sysrof
from config import CONFIG, DISC, MAIN, OBJECTS, Image, aligned, content, fail, hexa, images, named, save
from kept import KEPT, reasons
from link import define, place, stoppable

RELINKED = Path('build/relink')
# the two kinds of SYSROF record that name an object's module, its headers,
# each with the offset in its body of the module's name, its length byte
# first, as SHC and asmsh write them
HEADERS = {0x04: 28, 0x06: 7}
# the most a SYSROF length byte counts, of a name or of a whole record
LONGEST = 0xFF
# the alignment of the space relink reserves: asmsh warns on a section
# aligned to less
LEAST = 4
# the shift test's two shifts past an image's base. Each is a multiple of
# 32, the most an object of the tree aligns a section to, so the image keeps
# its layout, and relink fails on an object that aligns one to more. Each is
# nonzero in both halves, so a word that holds half of an address moves by
# other than the shift
SHIFTS = (0x01234560, 0x02468ac0)
# the top three bits of an address in each region the game reaches RAM
# through: P0's 0x0c..., P1's 0x8c... and P2's 0xac..., which bypasses the
# cache
REGIONS = (0x00000000, 0x80000000, 0xa0000000)
# the bits of an address that select its region. The bits below them give
# the same byte of RAM through each of REGIONS
REGION_BITS = 0xe0000000
# a stage's model workspace starts on the first boundary of 32 at or past
# the end of its image, as do four Chao areas' land buffers
WORKSPACE_ALIGNMENT = 32
# a row of the externally defined symbols list of lnk's map: a C name the
# link exports, with its leading underscore, its address and how it is
# defined. A name past 32 characters takes a line of its own, and its
# address the next
EXPORTED = re.compile(r"^_(\w+)\s+H'([0-9A-F]{8})\s+\w+\r?$", re.M)


def runs(image, position=None):
    """The rows lnk links as one section, as (output section, rows) for each
    run of an output section's source rows that share an input section, in
    address order. Given position, {source: index}, a run ends too where
    the next row's object would come earlier in input order."""
    found = []
    for name, _, _ in image.sections:
        rows = sorted((row for row in image.placements if row.section == name and row.writer != DISC),
                      key=lambda row: row.start)
        for row in rows:
            last = found[-1][1][-1] if found and found[-1][0] == name else None
            if last and image.input_section(last) == image.input_section(row) \
                    and (position is None or position[last.writer] < position[row.writer]):
                found[-1][1].append(row)
            else:
                found.append((name, [row]))
    return found


def order(image, kept=frozenset()):
    """The image's sources in the order relink gives lnk their objects: each
    run's in the order its rows place them, and otherwise in table order.

    kept holds the sections whose start or size an object asks, which a
    cut would change. A source is ready once each source a run puts before
    it has gone. Where none is, the next is the first waiting source, in
    table order, whose predecessors in every run of kept have gone. Where
    no source passes even that, the runs of kept form a cycle, and order()
    fails, naming each step of it and the section that takes it."""
    rank = {source: index for index, source in enumerate(image.sources())}
    # for each source, the sources a run puts before it, and those a run of
    # kept puts there, each with its section
    before, held = {source: set() for source in rank}, {source: {} for source in rank}
    for _, rows in runs(image):
        section = image.input_section(rows[0])
        for earlier, later in zip(rows, rows[1:]):
            before[later.writer].add(earlier.writer)
            if section in kept:
                held[later.writer].setdefault(earlier.writer, section)
    found, done = [], set()
    while len(found) < len(rank):
        waiting = [source for source in rank if source not in done]
        source = next((source for source in waiting if before[source] <= done), None)
        if source is None:
            source = next((source for source in waiting if held[source].keys() <= done), None)
        if source is None:
            # follow the waiting sources back until one repeats
            chain = [waiting[0]]
            while chain.count(chain[-1]) < 2:
                chain.append(next(earlier for earlier in held[chain[-1]] if earlier not in done))
            cycle = chain[chain.index(chain[-1]):]
            fail(f"{image.splits}: no one order of the objects keeps the rows of each section whose start or size "
                 'an object asks in order: '
                 + ', '.join(f'{earlier} goes before {later} in {held[later][earlier]}'
                             for later, earlier in zip(cycle, cycle[1:])))
        found.append(source)
        done.add(source)
    return found


def fresh(name, taken):
    """name with the lowest number from 2 on after it that gives a name
    taken doesn't hold."""
    number = 2
    while f'{name}{number}' in taken:
        number += 1
    return f'{name}{number}'


def plan(image, sources, declared, taken, kept=frozenset()):
    """How lnk lays out image's sections from the objects of sources, in
    input order. declared gives each source's section table, and taken the
    section names an object or a table holds, which a new name avoids. kept
    holds the sections whose start or size an object asks. Returns:

    - starts, {lnk section: address} in the order place() takes them;
    - renamed, {source: {input section: lnk section}} where they differ;
    - holes, {source: [(lnk section, size, alignment, code)]}, the space
      reserved just ahead of each source's object;
    - report, a line for each stand-in;
    - and counts, (new, cut): how many runs take a new name, and how many
      of those are cut where input order goes back.

    A row lnk can't start its object at fails: one off its object's
    alignment, or short of the space relink would reserve ahead of it. So
    does a run of kept that would take a new name."""
    position = {source: index for index, source in enumerate(sources)}
    starts, renamed, holes, report = [], {}, {}, []
    # for each output section, its first piece's start, its last run and
    # where that ends; the lnk sections the runs make; and the names a new
    # one avoids
    first, last, ends, made, taken = {}, {}, {}, set(), set(taken)
    new = cut = 0
    for output, rows in runs(image, position):
        input_name = image.input_section(rows[0])
        name = fresh(input_name, taken) if input_name in made else input_name
        made.add(name)
        taken.add(name)
        if output in last and image.input_section(last[output][-1]) == input_name:
            report.append(f"cuts {output} at {rows[0].writer}, whose object comes before {last[output][-1].writer}'s")
            cut += 1
        if name != input_name:
            if input_name in kept:
                fail(f"{image.where[rows[0]]}: {rows[0].writer}'s {input_name} would link as section {name}, apart "
                     f'from the run of {input_name} before it, and an object asks the start or size of {input_name}')
            report.append(f'links {input_name} of {rows[0].writer} and {len(rows) - 1} more as section {name}')
            new += 1
            for row in rows:
                renamed.setdefault(row.writer, {})[input_name] = name
        at = ends.get(output, next(start for section, start, _ in image.sections if section == output))
        for row in rows:
            piece = declared[row.writer][input_name]
            if row.start % piece.alignment:
                fail(f"{image.where[row]}: {row.writer}'s {input_name} starts at {hexa(row.start)}, off its "
                     f"object's alignment of {piece.alignment}")
            begins = row.start
            if aligned(at, piece.alignment) != row.start:
                begins = aligned(at, LEAST)
                if begins >= row.start:
                    fail(f'{image.where[row]}: {row.writer} starts at {hexa(row.start)}, and lnk would start it at '
                         f'{hexa(aligned(at, piece.alignment))}, with too little space between to reserve on '
                         f'{LEAST}')
                holes.setdefault(row.writer, []).append((name, row.start - begins, LEAST, piece.code))
                report.append(f'reserves {output} {hexa(begins)} to {hexa(row.start)}, ahead of {row.writer}, '
                              'where no object of the link lies')
            first.setdefault(output, begins)
            at = row.end
        ends[output], last[output] = at, rows
        starts.append((name, first[output]))
    # at one address, the bounds' empty sections go ahead of the runs, in
    # bounds.txt's order, and the runs keep theirs
    bounds = image.bounds()
    starts = [(address, len(bounds), number, name) for number, (name, address) in enumerate(starts)]
    starts += [(start, number, 0, name) for number, (name, start, _) in enumerate(bounds)
               if name not in made and any(name in table for table in declared.values())]
    return {name: start for start, _, _, name in sorted(starts)}, renamed, holes, report, (new, cut)


def counted(path, name, what):
    """name as a SYSROF record spells it: its length in a byte, then its
    bytes, one a character. A name with a character past 0xff, or more
    bytes than a length byte counts, fails, naming path and what the name
    is."""
    try:
        spelled = name.encode('latin-1')
    except UnicodeEncodeError:
        fail(f'{path}: the {what} {name} has no byte for each character')
    if len(spelled) > LONGEST:
        fail(f'{path}: the {what} {name} is {len(spelled)} bytes long, past the {LONGEST} a length byte counts')
    return bytes([len(spelled)]) + spelled


def renamed_object(path, module=None, sections=None):
    """The bytes of the object at path with its module and sections named
    anew. module, (old, new), names the module new, not old, in the module
    field of each of HEADERS, which the object must hold once and which
    must name old. sections, {old: new}, names each section old new
    instead, inside its record of the section table: SHC and asmsh give
    each entry a record of its own, and a record whose entry crosses into
    the next fails. Each record takes its size and its checksum anew, so a
    new name may differ in length from the old, but a record that would
    pass LONGEST bytes fails."""
    records = [bytearray(record) for _, record in sysrof.records(path)]
    named_in, left = set(), dict(sections or {})
    if module:
        old, new = counted(path, module[0], 'module name'), counted(path, module[1], 'module name')
    for record in records:
        kind = record[0] & ~sysrof.LAST
        if module and kind in HEADERS:
            if kind in named_in:
                fail(f'{path}: two {kind:#04x} headers name its module')
            at = 2 + HEADERS[kind]
            if record[at:at + len(old)] != old:
                fail(f"{path}: its {kind:#04x} header doesn't name its module {module[0]}")
            record[at:at + len(old)] = new
            named_in.add(kind)
        elif left and record[0] & ~sysrof.LAST == sysrof.SECTIONS:
            body, at, entries = record[2:-1], 0, []
            # an entry is 16 bytes, then the name's length and the name
            while at < len(body):
                if at + 17 > len(body) or (end := at + 17 + body[at + 16]) > len(body):
                    fail(f'{path}: an entry of its section table crosses into the next record')
                name = body[at + 17:end].decode('latin-1')
                entries.append(body[at:at + 16] + counted(path, left.pop(name, name), 'section name'))
                at = end
            record[2:-1] = b''.join(entries)
    for kind in HEADERS:
        if module and kind not in named_in:
            fail(f'{path}: no {kind:#04x} header names its module')
    if left:
        fail(f'{path}: no section {", ".join(left)} to name anew')
    for record in records:
        if len(record) > LONGEST:
            fail(f'{path}: a record named anew would be {len(record)} bytes long, past the {LONGEST} its size byte '
                 'counts')
        record[1] = len(record)
        record[-1] = 0xFF - sum(record[:-1]) & 0xFF
    return b''.join(records)


def bindings(image, objects):
    """Each name some of objects, {source: path}, takes from outside and none
    defines, as (address, name) in address order, with the address image's
    tables give it. A name the tables don't give fails, naming a source
    that takes it."""
    defined = set().union(*map(sysrof.exports, objects.values()))
    wanted = {}
    for source, obj in objects.items():
        for name in sorted(sysrof.references(obj) - defined):
            wanted.setdefault(name, source)
    addresses = image.symbol_addresses
    missing = [f'{source}: it takes {sysrof.c_name(name)} from outside, which no object of {image.name} defines '
               f'and no symbols.txt of {image.name} gives' for name, source in wanted.items()
               if not name.startswith('_') or name[1:] not in addresses]
    if missing:
        fail(*missing)
    return sorted((addresses[name[1:]], name[1:]) for name in wanted)


def holder(image, address):
    """Where image's tables put address, as words for a report: in a
    source's row, in a disc row of no source's, or in no row."""
    # a source's row ahead of a disc row inside it
    rows = sorted((row for row in image.placements if row.start <= address < row.end),
                  key=lambda row: row.writer == DISC)
    if not rows:
        return 'where no row lies'
    row = rows[0]
    return f'in a disc row of {row.section}' if row.writer == DISC else f"in {row.writer}'s {row.section}"


def joined(image, written, original):
    """The image the link writes: written, the bytes its S-records write by
    address, then the disc's bytes in each disc row, from original, the
    image's original bytes. A byte neither writes is 0. A byte written
    outside original's span fails, as does a disc row that passes its end."""
    base = image.base
    stray = sorted(at for at in written if not base <= at < base + len(original))
    if stray:
        fail(f'{image.name}: the whole link writes at {hexa(stray[0])}, outside its {len(original):#x} stored bytes')
    out = bytearray(len(original))
    for at, byte in written.items():
        out[at - base] = byte
    for row in image.disc():
        if row.end - base > len(original):
            fail(f'{image.where[row]}: the disc row from {hexa(row.start)} to {hexa(row.end)} lies past the '
                 f"original's {len(original):#x} stored bytes")
        out[row.start - base:row.end - base] = original[row.start - base:row.end - base]
    return bytes(out)


def compare_names(image, path, exported):
    """Check the image's symbols.txt against lnk's map of the whole link at
    path. The map must hold its externally defined symbols list, and the
    list a row for each name of the table in exported, the names the link's
    objects export. A name not in exported, such as a static's, goes
    unchecked. Fails without the list, and where the list lacks a required
    name or gives one another address than its row does, naming each such
    row."""
    _, heading, listed = content(path).decode('latin-1').partition('EXTERNALLY DEFINED SYMBOLS LIST')
    if not heading:
        fail(f'{path}: no externally defined symbols list, so it may be cut short')
    found = {name: int(address, 16) for name, address in EXPORTED.findall(listed)}
    table = image.path / 'symbols.txt'
    problems = []
    for address, name in named(table):
        if name not in exported:
            continue
        if name not in found:
            problems.append(f'{table}: {name}, which the whole link of {image.name} exports, has no address in {path}')
        elif found[name] != address:
            problems.append(f'{table}: {name} lies at {hexa(address)}, and the whole link of {image.name} defines it '
                            f'at {hexa(found[name])}')
    if problems:
        fail(*problems)


def writer(spans, address):
    """What the splicer says writes the byte at address, by spans, as
    splice.spans() gives them: a source, the disc or the linker's fill; or
    nothing."""
    for start, end, kind, source in spans:
        if start <= address < end:
            return source if kind == splice.SOURCE else "the linker's fill" if kind == splice.FILL else 'the disc'
    return 'nothing'


def compare(image, relinked, path, spliced):
    """Fail where relinked, the whole link's image, differs from spliced,
    the splicer's at path, naming the first address that differs, or where
    the shorter of the two ends, and what the splicer says writes the
    splice's byte there."""
    if relinked == spliced:
        return
    at = next((at for at, (ours, theirs) in enumerate(zip(relinked, spliced)) if ours != theirs),
              min(len(relinked), len(spliced)))
    address = image.base + at
    if at < len(spliced):
        by = writer(splice.spans(image), address)
        theirs = f'its byte at {hexa(address)}, which {by} writes, is {spliced[at]:#04x}'
        ours = f'writes {relinked[at]:#04x}' if at < len(relinked) else 'ends before it'
    else:
        theirs, ours = f'ends at {hexa(address)}', f'writes {relinked[at]:#04x} there'
    fail(f'{path}: {theirs}, and the whole link of {image.name} {ours}')


def span(image):
    """image's span, (low, high): its base, and the first boundary of
    WORKSPACE_ALIGNMENT at or past the end of its last section, bss
    included. The span holds high."""
    return image.base, aligned(max(end for _, _, end in image.sections), WORKSPACE_ALIGNMENT)


def within(value, low, high):
    """Whether value lies in [low, high] through any of REGIONS."""
    return (value & REGION_BITS) in REGIONS \
        and (low & ~REGION_BITS) <= (value & ~REGION_BITS) <= (high & ~REGION_BITS)


def own_names(image):
    """The names of image's own symbols.txt in its span, through any of
    REGIONS: those the shift test moves with the image. A row that the main
    executable's table gives too is main's, and is left out."""
    low, high = span(image)
    main = set() if image.name == MAIN else set(named(CONFIG / MAIN / 'symbols.txt'))
    return {name for address, name in named(image.path / 'symbols.txt')
            if within(address, low, high) and (address, name) not in main}


def linked(image, inputs, starts, bound, original, at, shift=0, inside=frozenset()):
    """The image the whole link writes, as joined() gives it from original,
    with lnk's files, its map among them, in the directory at. lnk links
    inputs, then a symbols object that defines bound, (address, name)
    pairs, and starts each lnk section of starts at its address. A shift
    moves each section, and each name of bound that inside holds, that many
    bytes on, and the image still holds each byte at its unshifted
    address."""
    names = [(address + shift if name in inside else address, name) for address, name in bound]
    written = place([*inputs, define(at / 'symbols.src', names)],
                    {name: start + shift for name, start in starts.items()}, at / 'link',
                    ['align_section', f'print {link.dos(at / "link.map")}'])
    return joined(image, {address - shift: byte for address, byte in written.items()}, original)


def words(data):
    """Yield data's 4-byte little-endian words, the last filled out with
    zeros."""
    for word, in struct.iter_unpack('<I', data + bytes(-len(data) % 4)):
        yield word


def shifted(image, relinked, links):
    """Check each aligned word of relinked, the whole link at image's base,
    against the same word of each of links, its links at SHIFTS's shifts as
    linked() gives them: it moves by each shift, or keeps its value under
    both. A word that a disc row gives a byte of goes unchecked. Returns
    how many words move, and each that keeps a value in the image's span,
    through any of REGIONS, as (address, value, what writes it). Where
    words do neither, fails, naming how many and the first: its address,
    its values and what the splicer says writes it."""
    base, (low, high) = image.base, span(image)
    disc = set()
    for row in image.disc():
        disc.update(range((row.start - base) // 4, (row.end - base + 3) // 4))
    # the words that move by other than each shift, and the first of them
    moved, fixed, wrong, first = 0, [], 0, None
    for at, (own, *others) in enumerate(zip(*map(words, (relinked, *links)))):
        if at in disc:
            continue
        moves = tuple((other - own) % (1 << 32) for other in others)
        if moves == SHIFTS:
            moved += 1
        elif any(moves):
            wrong += 1
            first = first or (base + 4 * at, own, others)
        elif within(own, low, high):
            fixed.append((base + 4 * at, own))
    spans = splice.spans(image) if wrong or fixed else []
    if wrong:
        address, own, others = first
        named_first = (f'{image.name}: its word at {hexa(address)}, which {writer(spans, address)} writes, holds '
                       f'{hexa(own)}, and {" and ".join(map(hexa, others))} at the shifts')
        if wrong > 1:
            fail(f'{image.name}: {wrong:,} of its words move, but not by each shift', named_first)
        fail(named_first)
    return moved, [(address, value, writer(spans, address)) for address, value in fixed]


def reviewed(image, fixed, rows):
    """fixed, the words shifted() finds keeping a value in the image's span,
    each with the reason of the row of rows, config/kept.txt's for the
    image, that holds it, as (address, value, what writes it, reason).
    Where a word has no row, or a row holds no word, fails, naming each."""
    found, used, problems = [], set(), []
    for address, value, by in fixed:
        row = next((row for row in rows if row[0] <= address < row[1]), None)
        if row is None:
            problems.append(f'{image.name}: its word at {hexa(address)}, which {by} writes, keeps {hexa(value)}, '
                            f'in its span, and {KEPT} gives no reason')
            continue
        used.add(row)
        found.append((address, value, by, row[2]))
    for start, end, _, where in (row for row in rows if row not in used):
        problems.append(f'{where}: no word of {image.name} from {hexa(start)} to {hexa(end)} keeps a value in its '
                        'span')
    if problems:
        fail(*problems)
    return found


def main(argv):
    parser = argparse.ArgumentParser(prog='relink', description=__doc__.split('\n\n')[0])
    parser.add_argument('image', metavar='IMAGE', help='the image to link, config/IMAGE/')
    parser.add_argument('original', metavar='ORIGINAL', type=Path, help="the image's original bytes")
    parser.add_argument('spliced', metavar='SPLICED', type=Path,
                        help="the splicer's image to compare the link with, such as build/SONIC2/STG03.BIN")
    parser.add_argument('--shift', action='store_true',
                        help='link the image at two shifted bases too, and write the words that keep a value in '
                             'its span to build/relink/IMAGE/fixed.txt')
    parser.add_argument('--verbose', action='store_true', help='list each stand-in for what the tree lacks')
    args = parser.parse_args(argv)
    if args.image not in images():
        fail(f'config/{args.image}/: no such image')
    image = Image(args.image)
    side = RELINKED / image.name
    objects = {source: OBJECTS / Path(source).with_suffix('.obj') for source in image.sources()}
    for source, obj in objects.items():
        if not obj.is_file():
            fail(f'{source}: no object at {obj}, so build the image first')
    kept = {name for obj in objects.values() for name in sysrof.bounded(obj)}
    sources = order(image, kept)
    declared = {source: sysrof.sections(obj) for source, obj in objects.items()}
    if args.shift:
        # before any link: config/kept.txt's rows for the image, and each section's alignment
        rows = reasons().get(image.name, [])
        for source, table in declared.items():
            for name, section in table.items():
                if any(shift % section.alignment for shift in SHIFTS):
                    fail(f"{source}: aligns {name} to {section.alignment}, more than the shift test's shifts keep")
    bounds = image.bounds()
    for source in sources:
        # each object's sections fit its rows, as for its own link
        link.starts(source, image.ranges(source), declared[source], bounds)
    taken = {name for table in declared.values() for name in table}
    taken |= {name for name, _, _ in image.sections} | {name for name, _, _ in bounds}
    starts, renamed, holes, report, (new, cut) = plan(image, sources, declared, taken, kept)

    # the modules of relink's own objects, which asmsh names after their files
    own = {'symbols', *(f'hole{number}' for number in range(1, len(holes) + 1))}
    inputs, seen, modules = [], set(own), own | {Path(source).stem for source in sources}
    numbered = iter(range(1, len(holes) + 1))
    for source in sources:
        if source in holes:
            inputs.append(define(side / f'hole{next(numbered)}.src', [], holes[source]))
        obj, stem, module = objects[source], Path(source).stem, None
        if stem in seen:
            module = (stem, fresh(stem, modules))
            modules.add(module[1])
            report.append(f'links {source} as module {module[1]}, since another module is named {stem} too')
        seen.add(stem)
        if module or source in renamed:
            obj = side / 'modules' / f'{module[1] if module else stem}.obj'
            save(obj, renamed_object(objects[source], module, renamed.get(source)))
        inputs.append(obj)

    bound = bindings(image, objects)
    low, high = image.base, max(end for _, _, end in image.sections)
    for address, name in bound:
        where = holder(image, address) if low <= address < high else 'outside the image'
        report.append(f'binds {name} at {hexa(address)} by address, {where}')
    exported = {sysrof.c_name(name) for obj in objects.values() for name in sysrof.exports(obj)}
    exported |= {name for _, name in bound}
    original = content(args.original)
    relinked = linked(image, inputs, starts, bound, original, side)
    if new:
        print(f'{image.name}: new section names for {new} of its runs, {cut} of them cut where input order goes back')
    if args.verbose:
        for line in report:
            print(f'{image.name}: {line}')
    compare_names(image, side / 'link.map', exported)
    compare(image, relinked, args.spliced, content(args.spliced))
    if not args.shift:
        save(RELINKED / f'{image.name}.bin', relinked)
        return
    inside = own_names(image)
    links = [linked(image, inputs, starts, bound, original, side / f'shift{number}', shift, inside)
             for number, shift in enumerate(SHIFTS, 1)]
    moved, fixed = shifted(image, relinked, links)
    fixed = reviewed(image, fixed, rows)
    save(RELINKED / f'{image.name}.bin', relinked)
    # after the image, which make's rule for fixed.txt takes as a prerequisite
    save(side / 'fixed.txt', ''.join(f'{image.name}  {hexa(address)}  {hexa(value)}  {by}  {reason}\n'
                                     for address, value, by, reason in fixed).encode())
    print(f'{image.name}: each shift moves {moved:,} of its words, and keeps {len(fixed):,} at a value in its span')


if __name__ == '__main__':
    stoppable()
    main(sys.argv[1:])
