"""Find a C source's inline bodies, and the code SHC writes each out as.

An inline body is a function whose body is assembly, named by a line of its
own directly above it:

    #pragma inline_asm(NAME)
    #pragma inline_asm(NAME(size=BYTES))

SHC expands the body at each call. A body that isn't static it also writes
out once as a function of its own, with an alignment, an RTS and a NOP after
the body's last line. A source with a body compiles to assembly text, which
asmsh assembles, and asmsh's listing gives each line of that text its place
in the object. The text keeps a body's lines as the source has them, so each
copy of a body is found by its lines. A #pragma asm block also keeps its
source lines. The listing identifies their instructions, initialized data and
the literal pools generated for them.
"""

import re
from collections import Counter

from config import code, fail

# a line that names an inline body, and one in the form above
NAMED = re.compile(r'^#pragma inline_asm\b.*$', re.M)
FORM = re.compile(r'#pragma inline_asm\((\w+)(?:\(size=\d+\))?\)')
# a listing's line for a .SECTION line of the text, and for the label SHC
# opens a function with: the listing's own line number, the line's place in
# its section, its number in the text, then the line itself
SECTION = re.compile(r' *\d+ [0-9A-F]{8} +\d+ +\.SECTION +(\w+),')
FUNCTION = re.compile(r' *\d+ ([0-9A-F]{8}) +\d+ +[\w$]+: +; function: (\w+)')
# After a long label, SHC puts the function's comment on the next line.
LONG_LABEL = re.compile(r'[\w$]+:')
NAMED_BELOW = re.compile(r' *; function: (\w+)')
# an inline body in the form above: its name, then its lines, between the
# braces of the function the line names
BODY = re.compile(r'^#pragma inline_asm\((\w+)(?:\(size=\d+\))?\)\n[^{;]*\{[ \t]*\n(.*?)^\}', re.M | re.S)
# a listing's line for a line of the text: the line's place in its section,
# where it has one, and after the line's number in the text, the line
LINE = re.compile(r' *\d+ (?:(?:([0-9A-F]{8})| {8}) (?:[0-9A-F]{2,})?)? +\d+ {5}(.*)')

# A file-scope assembly block retains its source lines in asmsh's listing.
# A label may share an instruction's line. Directives start with a dot.
BLOCK = re.compile(r'^[ \t]*#pragma[ \t]+asm[ \t]*\n(.*?)^[ \t]*#pragma[ \t]+endasm\b', re.M | re.S)
INSTRUCTION = re.compile(r'^\s*(?:[\w?$]+:\s*)?([A-Za-z][\w./]*)(?:\s|$)')
# The assembler prints initialized instruction and data bytes in the same
# column. Literal pool rows name the assembly source lines they serve.
EMITTED = re.compile(r' *\d+ ([0-9A-F]{8}) ([0-9A-F]+) +(\d+) ')
LITERAL = re.compile(r' *\d+ ([0-9A-F]{8}) ([0-9A-F]+) +(ALIGNMENT CODE|DATA FOR SOURCE-LINE(?: +(.*))?)$')
POOL_OWNERS = re.compile(r'\d+(?: *, *\d+)*')
# asmsh ends a listing row at this column, even in the middle of a source-line number.
LISTING_WIDTH = 132
LABEL = r'^\s*(?:[\w?$]+:\s*)?'
DATA = re.compile(LABEL + r'\.(?:DATA\.([BWL])|FDATA\.([SD]))\s', re.I)
LABEL_NAME = re.compile(r'^\s*([\w?$]+):')
PC_DATA = re.compile(LABEL + r'(?:MOVA|MOV\.[WL])\s+([\w?$]+)\s*,', re.I)
ALIGN = re.compile(LABEL + r'\.ALIGN\s+(\d+)\s*$', re.I)
WIDTH = {'B': 1, 'W': 2, 'L': 4, 'S': 4, 'D': 8}


def bodies(text):
    """The names of the inline bodies in text, a C source's, in its order. A
    line that names one in another form gives none: tools/tags.py fails it."""
    return [named[1] for named in map(FORM.fullmatch, NAMED.findall(text)) if named]


def instructions(block):
    """The indices of the instruction lines of block, excluding labels,
    comments and directives. Each SH instruction occupies one halfword."""
    return [at for at, line in enumerate(block) if INSTRUCTION.match(line.partition(';')[0])]


def compiler_columns(body):
    """Whether all instructions use the indentation SHC uses for its own
    output: ten spaces, so that each starts in column 11."""
    return all(re.match(r' {10}\S', body[i]) for i in instructions(body))


def assembled(blocks, listing, where, *, include_pools=True):
    """Where the listing places the raw blocks' instructions and initialized
    data, and the stops for written(). A reservation or standalone alignment
    supplies no range, but still stops a span. A repeated spelling must
    occur equally often in the listing and the source. Defer pools when
    inline copies also contribute instructions to the same listing."""
    found, ends, previous, lines, section = {}, {}, {}, [], None
    # A raw block after an inline function's return stops its span. asmsh
    # gives .ALIGN the address after its padding, so retain the preceding
    # instruction's end for that section.
    for row in listing.splitlines():
        if entered := SECTION.match(row):
            section = entered[1]
        if held := LINE.match(row):
            data = EMITTED.match(row)
            line = held[2].rstrip()
            lines.append((line, section, data, previous.get(section, 0)))
            if data and INSTRUCTION.match(line.partition(';')[0]):
                previous[section] = int(data[1], 16) + 2
    source = [line[0] for line in lines]
    for body, expected in Counter(blocks).items():
        matches = [at for at, line in enumerate(source)
                   if line == body[0] and tuple(source[at:at + len(body)]) == body]
        if len(matches) != expected:
            fail(f'{where}: #pragma asm block count differs: {expected} in the source, {len(matches)} in the listing')
        for at in matches:
            seen, labels, previous_end, alignment = set(), set(), None, None
            placed = lines[at:at + len(body)]
            references = {(section, load[1]) for line, section, _, _ in placed
                          if (load := PC_DATA.match(line.partition(';')[0]))}
            for line, section, data, before in placed:
                if not section or not code(section):
                    previous_end = alignment = None
                    labels.clear()
                    continue
                if section not in seen:
                    ends.setdefault(section, []).append(before)
                    seen.add(section)
                statement = line.partition(';')[0].strip()
                if label := LABEL_NAME.match(statement):
                    labels.add((section, label[1]))
                if not statement or statement.endswith(':'):
                    continue
                if boundary := ALIGN.fullmatch(statement):
                    labels.clear()
                    alignment = int(boundary[1])
                    continue
                datum = DATA.match(statement)
                # An unrelated data declaration in the code section is not
                # part of the handwritten routine. Only include a value named
                # by one of its own PC-relative loads or address calculations.
                if not (INSTRUCTION.match(statement) or datum and not labels.isdisjoint(references)):
                    labels.clear()
                    previous_end = alignment = None
                    continue
                labels.clear()
                if not data:
                    fail(f'{where}: no emitted bytes for {statement}')
                start = int(data[1], 16)
                end = start + len(data[2]) // 2
                if datum:
                    width = WIDTH[(datum[1] or datum[2]).upper()]
                    # A typed value can own its minimum natural alignment
                    # inside the block. A reserve or an arbitrary .ALIGN
                    # between two emissions must never become initialized data.
                    if previous_end is not None and alignment == width and start == ((previous_end + width - 1) & -width):
                        start = previous_end
                found.setdefault(section, []).append((start, end))
                previous_end, alignment = end, None
    return (pooled(listing, found, where) if include_pools else joined(found)), ends


def pooled(listing, ranges, where):
    """Count a pool value and its natural alignment as assembly exactly when
    all the instructions asmsh names for it are assembly. A pool can follow
    the expanded body, or land inside the span of a different body."""
    if 'DATA FOR SOURCE-LINE' not in listing:
        return joined(ranges)
    owners, pools = {}, []
    section = padding = None
    for line in listing.splitlines():
        if entered := SECTION.match(line):
            section = entered[1]
            padding = None
        if section not in ranges:
            continue
        if pool := LITERAL.fullmatch(line.rstrip()):
            start = int(pool[1], 16)
            width = len(pool[2]) // 2
            end = start + width
            if pool[3] == 'ALIGNMENT CODE':
                padding = (section, start, end)
                continue
            if len(line) >= LISTING_WIDTH or not POOL_OWNERS.fullmatch(pool[4] or ''):
                fail(f'{where}: incomplete literal pool source-line list at {section}+{start:#x}')
            if (padding and padding[0] == section and padding[2] == start
                    and start == ((padding[1] + width - 1) & -width)):
                start = padding[1]
            pools.append((section, start, end, tuple(int(n) for n in pool[4].split(','))))
        elif (data := EMITTED.match(line)) and (held := LINE.match(line)):
            start = int(data[1], 16)
            end = start + len(data[2]) // 2
            if INSTRUCTION.match(held[2].partition(';')[0]):
                key = section, int(data[3])
                owners[key] = None if key in owners else any(low <= start and end <= high for low, high in ranges[section])
        # Page headers between consecutive numbered listing rows emit nothing.
        if line.lstrip()[:1].isdigit():
            padding = None

    found = {name: list(spans) for name, spans in ranges.items()}
    owned = {}
    for section, start, end, numbers in pools:
        instructions = [owners.get((section, number)) for number in numbers]
        if None in instructions:
            fail(f'{where}: no unique instruction for literal pool source line {numbers[instructions.index(None)]} in {section}')
        if all(instructions):
            owned.setdefault(section, []).append((start, end))
        else:
            # Copied and written bodies cover contiguous spans. Remove any
            # literal in one that also serves C, including its own padding.
            kept = []
            for low, high in found[section]:
                if low < start:
                    kept.append((low, min(high, start)))
                if end < high:
                    kept.append((max(low, end), high))
            found[section] = kept
    return joined(found, owned)


def rows(listing):
    """Each listing row of a line of the text, as (section, line, opened):
    line is LINE's match, and opened is (offset, name) where the line opens
    a function and None elsewhere. A function's comment below its long label
    opens it at the label's offset."""
    section = label = None
    for row in listing.splitlines():
        if entered := SECTION.match(row):
            section, label = entered[1], None
        elif section is not None and (held := LINE.match(row)):
            opened = None
            if named := FUNCTION.match(row):
                opened = int(named[1], 16), named[2]
            elif label is not None and (named := NAMED_BELOW.fullmatch(held[2])):
                opened = label, named[1]
            label = int(held[1], 16) if held[1] and LONG_LABEL.fullmatch(held[2].strip()) else None
            yield section, held, opened


def written(names, listing, sizes, stops=None):
    """Where an object holds the functions SHC wrote out for the inline bodies
    called names, as {section: [(start, end), ...]} in offsets from each
    section's start. listing is the text of asmsh's listing for the object,
    and sizes gives the size of each of its sections.

    A function runs from its label to the next function's in its section, or
    to the section's end, so it holds what SHC adds after the body. The
    stops end a span at SHC's last instruction before a file-scope
    assembly block. Only a code section, as config.code() tells one,
    gives ranges."""
    labels = {}
    for section, _, opened in rows(listing):
        if opened:
            labels.setdefault(section, []).append(opened)
    found = {}
    for section, functions in labels.items():
        if not code(section):
            continue
        ends = [start for start, _ in functions[1:]] + [sizes[section]]
        if stops and section in stops:
            ends = [min((cut for cut in stops[section] if start < cut < end), default=end)
                    for (start, _), end in zip(functions, ends)]
        ranges = [(start, end) for (start, name), end in zip(functions, ends) if name in names]
        if ranges:
            found[section] = ranges
    return found


def copied(text, listing, sizes, where, raw=None):
    """Where an object holds each copy of the inline bodies of text, a C
    source's, as written() gives ranges: a copy at each call that expands a
    body, and one in the function SHC writes a body out as. listing is
    written()'s, sizes gives the size of each code section, and where names
    the listing in a failure.

    SHC follows a copy with its own .ALIGN 4 line. A raw assembly block
    can have the same spelling, so exclude its already identified ranges.
    A body whose lines cannot be distinguished from generated C fails."""
    if raw is None:
        from csource import assembly_blocks
        blocks = assembly_blocks(text, directives=True)
        raw = assembled(blocks, listing, where, include_pools=False)[0] if blocks else {}
    lines, hosts = [], {}
    for section, held, opened in rows(listing):
        if opened:
            hosts[section] = opened[1]
        lines.append([section, held[1] and int(held[1], 16), held[2].rstrip(), hosts.get(section)])
    # a line without a place of its own takes the next line's in its section
    after = dict(sizes)
    for line in reversed(lines):
        if line[1] is None:
            line[1] = after.get(line[0])
        after[line[0]] = line[1]
    found = {}
    extracted = BODY.findall(text)
    for name in sorted(set(bodies(text)) - {name for name, _ in extracted}):
        fail(f'{where}: no lines between braces below the inline body {name}\'s #pragma inline_asm')
    for name, body in extracted:
        body = [line.rstrip() for line in body.splitlines()]
        matched = False
        for at in range(len(lines) - len(body)):
            end = at + len(body)
            section, start, _, host = lines[at]
            if (not code(section) or lines[end][0] != section or lines[end][2] != '          .ALIGN      4'
                    or [line[2] for line in lines[at:end]] != body):
                continue
            stop = lines[end][1]
            if any(low < stop and start < high for low, high in raw.get(section, ())):
                continue
            # SHC's own instructions start in column 11. Even the same
            # following alignment can also introduce a generated literal
            # pool. Only its named out-of-line function proves such a body.
            if host != name and compiler_columns(body):
                fail(f'{where}: inline body {name} in {host} is indistinguishable from compiler-generated code')
            found.setdefault(section, []).append((start, stop))
            matched = True
        if not matched:
            fail(f'{where}: no copy of the inline body {name}')
    return found


def joined(*ranges):
    """The ranges of each section in every {section: [(start, end), ...]} of
    ranges, as one such, in order and with those that meet made one."""
    found = {}
    for section in {section for each in ranges for section in each}:
        for start, end in sorted(pair for each in ranges for pair in each.get(section, [])):
            if found.get(section) and start <= found[section][-1][1]:
                found[section][-1] = (found[section][-1][0], max(end, found[section][-1][1]))
            else:
                found.setdefault(section, []).append((start, end))
    return found
