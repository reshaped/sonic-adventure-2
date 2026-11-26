"""Count each image's code and data by what writes them, and write README.md's
table.

    python3 tools/progress.py            print the table and the data's counts
    python3 tools/progress.py --write    write the table into README.md
    python3 tools/progress.py --check    fail where README.md's table is stale

The table goes between README.md's <!-- progress --> and <!-- /progress -->
lines. Each row counts the images AREAS gives it. An image's code is the
bytes of its sections whose names start with P, less the linker's fill:

- assembly is what asmsh assembles from the tree's .src files, and of a C
  source the function SHC writes out for each inline body and each copy of
  a body where a call expands it;
- C is the rest of what SHC compiles from the tree's C sources;
- the disc's is the code under disc rows. Its column appears only while
  there is any.

C's share and assembly's are rounded down to a tenth of a percent, and the
disc's is what is left, so the shares make 100.0% and the table never
overstates the source. Once no code is the disc's, assembly's share is what
C's leaves. A count above none that is given no tenth shows as <0.1%.

The line below the table gives the share of the assembly's code that an @asm
tag with a basis covers: one of any kind but open (TAGS.md#assembly),
rounded down as C's is. A .src function's code runs from its exported label
to the next function's, or to its section's end. A C source's inline bodies
and #pragma asm blocks are found as splice.assembly() finds them, each with
its own tag, and a pool word counts only where each line that loads it has a
basis.

The data's line is printed, not written to README.md. It counts the images'
initialized data, the bytes of their other sections but bss, by what
splice.counts() says writes them:

- generated C is each datum and text a source declares whose initializers
  the build writes from the disc's bytes;
- the disc's is the data under disc rows, which no source defines;
- the sources' own is the rest of what SHC compiles and asmsh assembles;
- and the linker's fill.

It reads the tables and each placed source's object, for the alignment that
tells the linker's fill and a .src function's label, and asmsh's listing
beside the object of a source with an inline body or a block, so the objects
must be built: make progress builds them. It doesn't need the disc.
"""

import argparse
from functools import lru_cache
from pathlib import Path

import csource
import inline
import sysrof
import tags
from config import DISC, Image, code, content, fail, images, save, stub_sources
from splice import ASSEMBLY, FILL, GENERATED, C, assembly, counts, declared

README = Path('README.md')
OPEN, CLOSE = '<!-- progress -->', '<!-- /progress -->'
# the table's rows, each with the images it counts, by the start of their names
AREAS = (('Main executable', ('1st_read',)),
         ('Stages', ('stg',)),
         ('Bosses', ('boss_',)),
         ('Chao', ('chao',)))
# the row of every image no row above counts
OTHER = 'Other programs'


def area(name):
    """The row that counts the image called name."""
    return next((label for label, starts in AREAS if name.startswith(starts)), OTHER)


def based(kind):
    """Whether an @asm tag of kind, None for no tag, records a basis."""
    return kind not in (None, 'open')


def functions(source, obj, sizes):
    """The code of each function of source, a .src file, whose @asm tag has
    a basis, as {section: [(start, end), ...]} in offsets from each code
    section's start. obj is its object, and sizes gives the size of each of
    its code sections. A function runs from its exported label to the next
    function's, or to its section's end. The ends come from the @asm tags
    alone, so they rest on make lint failing a function without one."""
    exported = {sysrof.c_name(name): place for name, place in sysrof.exports(obj).items()}
    starts = {}
    for tag in tags.assembly_faults(source, content(source).decode('latin-1'))[1]:
        place = next((exported[name] for name in tag.names if name in exported), None)
        if tag.tag == 'asm' and place and code(place[0]):
            starts.setdefault(place[0], []).append((place[1], based(tag.kind)))
    found = {}
    for section, held in starts.items():
        held.sort()
        ends = [start for start, _ in held[1:]] + [sizes[section]]
        found[section] = [(start, end) for (start, kept), end in zip(held, ends) if kept]
    return found


def pieces(source, obj, sizes):
    """The code of each inline body and #pragma asm block of source, a C
    file, whose @asm tag has a basis, as functions() gives it. The pieces are
    found as splice.assembly() finds them all, and a pool word counts where
    every line that loads it is of such a piece. A block counts where every
    block spelled as it is has a tag with a basis."""
    text = content(source).decode('latin-1')
    above, lines = tags.asm_above(text), text.splitlines()
    spelled = {}
    for block in inline.BLOCK.finditer(text):
        first, last = text.count('\n', 0, block.start(1)), text.count('\n', 0, block.end(1))
        body = tuple(line.rstrip() for line in lines[first:last])
        spelled.setdefault(body, set()).add(based(above.get(block.start())))
    names = inline.bodies(text)
    blocks = csource.assembly_blocks(text, directives=bool(names))
    held = [block for block in blocks if spelled.get(block) == {True}]
    listing = obj.with_suffix('.lis')
    placed = content(listing).decode('latin-1')
    if not names:
        return inline.assembled(held, placed, listing)[0] if held else {}
    raw, stops = inline.assembled(blocks, placed, listing, include_pools=False) if blocks else ({}, {})
    own = inline.assembled(held, placed, listing, include_pools=False)[0] if held else {}
    kept = [named[1] for pragma in inline.NAMED.finditer(text)
            if (named := inline.FORM.fullmatch(pragma[0])) and based(above.get(pragma.start()))]
    bodies = ''.join(body[0] + '\n' for body in inline.BODY.finditer(text) if body[1] in kept)
    ranges = inline.joined(own, inline.written(kept, placed, sizes, stops),
                           inline.copied(bodies, placed, sizes, listing, raw))
    return inline.pooled(placed, ranges, listing)


def basis(source, whole, stubs):
    """The ranges of whole, source's assembly as splice.assembly() gives it,
    that an @asm tag with a basis covers, as functions() and pieces() find
    them."""
    if not whole:
        return {}
    sections, obj = declared(source, stubs)
    sizes = {name: section.size for name, section in sections.items() if code(name)}
    found = (functions if source.endswith('.src') else pieces)(source, obj, sizes)
    return {name: [(max(low, start), min(high, end)) for low, high in ranges
                   for start, end in found.get(name, ()) if max(low, start) < min(high, end)]
            for name, ranges in whole.items()}


def tally():
    """Each row's [binaries, C bytes, assembly bytes, disc bytes, assembly
    bytes an @asm tag with a basis covers], by its label, in table order, and
    the images' initialized data as {label: bytes}, by splice.counts()'s
    labels."""
    rows = {label: [0] * 5 for label, _ in AREAS}
    rows[OTHER] = [0] * 5
    data = {}
    stubs = stub_sources()
    # a source placed in several images is read once
    whole = lru_cache(maxsize=None)(lambda source: assembly(source, stubs))
    covered = lru_cache(maxsize=None)(lambda source: basis(source, whole(source), stubs))
    for name in images():
        image = Image(name, stubs)
        written, held = counts(image, assembled=whole)
        row = rows[area(name)]
        row[0] += 1
        for at, label in enumerate((C, ASSEMBLY, DISC), 1):
            row[at] += written.get(label, 0)
        if written.keys() - {C, ASSEMBLY, DISC, FILL}:
            fail(f'{name}: code from {sorted(written.keys() - {C, ASSEMBLY, DISC, FILL})}, which no column counts')
        if held.keys() - {C, ASSEMBLY, GENERATED, DISC, FILL}:
            fail(f"{name}: data from {sorted(held.keys() - {C, ASSEMBLY, GENERATED, DISC, FILL})}, which the data's "
                 'line counts nowhere')
        for label, size in held.items():
            data[label] = data.get(label, 0) + size
        row[4] += counts(image, assembled=covered)[0].get(ASSEMBLY, 0)
    return rows, data


def shares(c, assembly, disc):
    """The three counts' shares of their sum, in tenths of a percent: C's and
    assembly's rounded down and the disc's the rest, or with nothing from
    the disc, assembly's the rest."""
    total = c + assembly + disc
    if not total:
        return 0, 0, 0
    first = c * 1000 // total
    second = assembly * 1000 // total if disc else 1000 - first
    return first, second, 1000 - first - second


def percent(tenths, count):
    """A share of tenths as the table shows it for count: a count of none as
    0%, and a count above none given no tenth as <0.1%."""
    if not count:
        return '0%'
    return '<0.1%' if not tenths else f'{tenths // 10}.{tenths % 10}%'


def table(rows):
    """README.md's lines for rows, as tally() gives them: the headline, then
    the table, with a row for all the images last, then the share of the
    assembly an @asm tag with a basis covers. The headline's share is what
    the row for all leaves the disc, so the two agree. The disc's column is
    left out where no row counts code from the disc."""
    total = [sum(row[at] for row in rows.values()) for at in range(5)]
    source, disc = total[1] + total[2], total[3]
    heads = ['Binaries', 'Code bytes', 'C', 'Assembly'] + ['From the disc'] * bool(disc)
    built = percent(1000 - shares(*total[1:4])[2], source) if disc else '100%'
    lines = [f"**{built} of the game's code builds from source, byte for byte.**", '',
             f'| | {" | ".join(heads)} |', '|---' + '|---:' * len(heads) + '|']
    for label, (binaries, c, assembly, held, _) in [*rows.items(), ('**All**', total)]:
        counted = (c, assembly, held)
        cells = [f'{binaries}', f'{sum(counted):,}',
                 *(percent(share, count) for share, count in zip(shares(*counted), counted))][:len(heads)]
        if label == '**All**':
            cells = [f'**{cell}**' for cell in cells]
        lines.append(f'| {label} | {" | ".join(cells)} |')
    covered = percent(total[4] * 1000 // total[2] if total[2] else 0, total[4])
    return lines + ['', f'[@asm tags](TAGS.md#assembly) record evidence of handwritten origins for {covered} of '
                        'the assembly.']


def data_line(data):
    """The line for data, the images' initialized data as tally() gives it:
    the bytes in all, and how many of them each writer supplies."""
    own, fill = data.get(C, 0) + data.get(ASSEMBLY, 0), data.get(FILL, 0)
    generated, disc = data.get(GENERATED, 0), data.get(DISC, 0)
    return (f'Initialized data: {own + fill + generated + disc:,} bytes. The sources supply {own:,}, '
            f'disc-derived initializers {generated:,}, direct disc copies {disc:,} and linker fill {fill:,}.')


def written(text, lines):
    """text, README.md's, with lines between its progress marks. A README
    without one line of each mark, in order, fails."""
    opening, closing = OPEN, CLOSE
    held = text.split('\n')
    if held.count(opening) != 1 or held.count(closing) != 1 or held.index(opening) > held.index(closing):
        fail(f'{README}: needs one {opening} line and one {closing} line after it')
    return '\n'.join(held[:held.index(opening) + 1] + lines + held[held.index(closing):])


def main(argv=None):
    parser = argparse.ArgumentParser(prog='progress', description=__doc__.split('\n\n')[0])
    group = parser.add_mutually_exclusive_group()
    group.add_argument('--write', action='store_true', help="write the table into README.md")
    group.add_argument('--check', action='store_true', help="fail where README.md's table is stale")
    args = parser.parse_args(argv)
    rows, data = tally()
    lines, line = table(rows), data_line(data)
    if not (args.write or args.check):
        print('\n'.join([*lines, '', line]))
        return
    text = content(README).decode()
    fresh = written(text, lines)
    if args.write:
        save(README, fresh.encode())
    elif fresh != text:
        fail(f"{README}: its table is stale; make progress writes it")


if __name__ == '__main__':
    main()
