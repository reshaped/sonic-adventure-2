"""Read and check each image's tables, config/<image>/splits.txt and symbols.txt,
and its bounds.txt where it has one.

TOOLCHAIN.md#image-tables gives the rows and their rules. Every range is
[start, end) in linked addresses. A malformed row fails, naming its file and
line, and so does a table that breaks a rule.

The tables hold every fact about an image: its file on the disc, its sections,
what writes each range, and its names. This module fixes none of them. It
fixes only what the tables share:

- a section whose name starts with P holds code, and one whose name starts
  with B is bss, as SHC names them;
- every image but the main executable takes the main executable's names;
- Chao area programs load alongside CHAO and can import its compiled objects.
"""

import os
import re
import secrets
import subprocess
import sys
from functools import cached_property
from pathlib import Path
from typing import NamedTuple

CONFIG = Path('config')
# where make builds each source's object, and each of its links
OBJECTS = Path('build/obj')
LINKS = Path('build/link')
# the main executable's image, which stays in memory beside every other
MAIN = '1st_read'
# Chao areas also import symbols from the garden executable resident beside them.
CHAO_AREAS = frozenset(('chaostgneut', 'chaostghero', 'chaostgdark', 'chaostglobby',
                       'chaostgentrance', 'chaostgrace', 'chaostgkinder'))
# in place of a source, the row of a range the build takes from the disc
DISC = 'disc'

ADDRESS = re.compile(r'0x[0-9a-f]{8}')
SECTION = re.compile(r'[A-Z][A-Z0-9_]*')
# a section's name in bounds.txt, which may hold a $, as SHC's $G0 does
BOUNDED = re.compile(r'[A-Z$][A-Z0-9_$]*')
NAME = re.compile(r'[A-Za-z_][A-Za-z0-9_]*')
SOURCE = re.compile(r'(?:[a-z0-9_]+/)+[A-Za-z0-9_]+\.(?:c|src)')
FILE = re.compile(r'(?:[A-Z0-9_]+/)*[A-Z0-9_]+\.(?:BIN|PRS)')


def fail(*problems):
    """Exit with status 1, writing each line of each problem to stderr after
    the tool's name."""
    tool = Path(sys.argv[0]).stem
    raise SystemExit('\n'.join(f'{tool}: {line}' for problem in problems for line in said(problem).splitlines()))


def said(problem):
    """problem as fail() writes it. An OSError that names its file reads PATH:
    REASON. One that names two, as a failed rename does, reads SOURCE ->
    TARGET: REASON. A child process that failed its check reads PROGRAM:
    exit STATUS, or PROGRAM: killed by signal NUMBER, where PROGRAM is the
    command's first argument, or the whole command where it was one string."""
    if isinstance(problem, OSError) and problem.filename is not None:
        files = problem.filename if problem.filename2 is None else f'{problem.filename} -> {problem.filename2}'
        return f'{files}: {problem.strerror}'
    if isinstance(problem, subprocess.CalledProcessError):
        command = problem.cmd
        program = os.fsdecode(command if isinstance(command, (str, bytes, os.PathLike)) else command[0])
        # a child that a signal killed has the signal's number, negated, as its returncode
        if problem.returncode < 0:
            return f'{program}: killed by signal {-problem.returncode}'
        return f'{program}: exit {problem.returncode}'
    return str(problem)


def content(path):
    """The bytes of the file at path. One that can't be read fails, naming it."""
    try:
        return Path(path).read_bytes()
    except OSError as error:
        fail(f'{path}: {error.strerror}')


def save(path, data):
    """Write data, bytes, to the file at path, making its directory. A file
    that can't be written fails, naming it. The bytes go first to a new file
    beside it that only this call made. A write that fails leaves no part of
    data at path, and two saves to path never share a file."""
    path = Path(path)
    part = path.with_name(f'{path.name}.{secrets.token_hex(8)}.part')
    made = False
    try:
        path.parent.mkdir(parents=True, exist_ok=True)
        with open(part, 'xb') as file:
            made = True
            file.write(data)
        part.replace(path)
    except OSError as error:
        if made:
            part.unlink(missing_ok=True)
        fail(error if error.filename else f'{path}: {error.strerror}')


def rows(path):
    """Yield each row of the table at path as its path:line and its fields.
    A # starts a comment, and a line without fields is no row. A line that
    isn't UTF-8 fails, naming its path:line."""
    for number, line in enumerate(content(path).split(b'\n'), 1):
        try:
            fields = line.decode().split('#')[0].split()
        except UnicodeDecodeError:
            fail(f"{path}:{number}: a byte that isn't UTF-8")
        if fields:
            yield f'{path}:{number}', fields


def hexa(address):
    """address as the tables write one: 0x and eight lowercase hex digits."""
    return f'0x{address:08x}'


def aligned(address, alignment):
    """address rounded up to a multiple of alignment."""
    return -(-address // alignment) * alignment


def code(section):
    """Whether the section called section holds code."""
    return section.startswith('P')


def zeroed(section):
    """Whether the section called section is bss: zeros, which no object writes."""
    return section.startswith('B')


def address(field, where):
    """The address a table's field gives. A field that isn't 0x and eight
    lowercase hex digits fails, naming where."""
    if not ADDRESS.fullmatch(field):
        fail(f'{where}: {field}: not an address, 0x and eight lowercase hex digits')
    return int(field, 16)


def interval(start, end, where):
    """A row's (start, end), from its two address fields. One that holds no
    byte fails, naming where."""
    start, end = address(start, where), address(end, where)
    if start >= end:
        fail(f'{where}: {hexa(start)} to {hexa(end)} holds no byte')
    return start, end


def named(path):
    """A symbols.txt's (address, name) rows, in table order, which is address
    order. Two names can share an address, and a name has one row. An image
    that names nothing has no symbols.txt, and gives no rows. An empty path
    names no file, and fails as the reader fails it."""
    found, seen = [], {}
    if path and not os.path.lexists(path):
        return found
    for where, row in rows(path):
        if len(row) != 2 or not NAME.fullmatch(row[1]):
            fail(f'{where}: not a row: ADDRESS NAME')
        if row[1] in seen:
            fail(f'{where}: {row[1]} has a row already, at {seen[row[1]]}')
        seen[row[1]] = where
        found.append((address(row[0], where), row[1]))
        if len(found) > 1 and found[-1][0] < found[-2][0]:
            fail(f'{where}: {row[1]} comes after {found[-2][1]}, out of address order')
    return found


class Range(NamedTuple):
    """A row that places bytes: what writes them, a source's path or DISC,
    their section, and their [start, end)."""

    writer: str
    section: str
    start: int
    end: int


def stub_sources():
    """The source paths whose disc-free build substitutes layout-only initializers."""
    return {path.relative_to('offsets').with_suffix('.c').as_posix() for path in Path('offsets').rglob('*.txt')}


def native_path(path, source, stubs):
    """Only a check can supply stubs. A full link always uses the original build tree."""
    return Path('build/stub') / path.relative_to('build') if source in stubs else path


class Image:
    """One image's file, sections, placements and names, checked as they're read.

    file is the image's path on the disc. sections are (name, start, end) in
    address order, and section_where gives each one's path:line. placements
    are the rows of the sources and of the disc, as Ranges in table order,
    and where gives each one's path:line.
    """

    def __init__(self, name, stubs=frozenset()):
        self.stubs = stubs
        self.name = name
        self.path = CONFIG / name
        self.splits = self.path / 'splits.txt'
        self.file = None
        self.sections, self.placements, self.where = [], [], {}
        self.input_sections = {}
        self.section_where = rowed = {}
        for where, row in rows(self.splits):
            if len(row) == 2 and row[0] == 'file':
                if self.file is not None:
                    fail(f'{where}: a second file row')
                if not FILE.fullmatch(row[1]):
                    fail(f"{where}: {row[1]}: not a file's path on the disc, in capitals, ending .BIN or .PRS")
                self.file = row[1]
            elif len(row) == 3 and SECTION.fullmatch(row[0]):
                if row[0] in rowed:
                    fail(f'{where}: a second row for section {row[0]}, after {rowed[row[0]]}')
                rowed[row[0]] = where
                self.sections.append((row[0], *interval(*row[1:], where)))
            elif len(row) == 4 and (row[0] == DISC or SOURCE.fullmatch(row[0])):
                source, sections = row[:2]
                input_name, separator, output = sections.partition(':')
                if not separator:
                    output = input_name
                if not SECTION.fullmatch(input_name) or not SECTION.fullmatch(output):
                    fail(f'{where}: {sections}: expected SECTION or INPUT:OUTPUT')
                if separator and source == DISC:
                    fail(f'{where}: a disc row names its output section only')
                if (code(input_name), zeroed(input_name)) != (code(output), zeroed(output)):
                    fail(f'{where}: {input_name} and {output} must both be code, stored data or bss')
                placement = Range(source, output, *interval(*row[2:], where))
                if input_name != output:
                    self.input_sections[placement] = input_name
                if placement in self.where:
                    fail(f'{where}: repeats the row at {self.where[placement]}')
                self.placements.append(placement)
                self.where[placement] = where
            else:
                fail(f'{where}: not a row: file PATH, SECTION START END, SOURCE SECTION START END, '
                     f'or {DISC} SECTION START END')
        if self.file is None:
            fail(f'{self.splits}: no file row')
        if not self.sections:
            fail(f'{self.splits}: no section rows')
        self.sections.sort(key=lambda row: row[1])
        self.check()

    def check(self):
        """Fail a row that breaks a rule of TOOLCHAIN.md#image-tables, naming
        it by its path:line."""
        at = self.where
        for (name, _, end), (later, start, _) in zip(self.sections, self.sections[1:]):
            if start < end:
                fail(f'{self.section_where[later]}: section {later} starts at {hexa(start)}, inside {name}, '
                     f'at {self.section_where[name]}')
        for first, second in zip(self.placements, self.placements[1:]):
            if second.start < first.start:
                fail(f'{at[second]}: a row that starts at {hexa(second.start)} comes after the one at '
                     f'{at[first]}, out of address order')
        stems, outputs = {}, {}
        for placement in self.placements:
            writer, section, start, end = placement
            where = at[placement]
            if not any(name == section and low <= start and end <= high for name, low, high in self.sections):
                fail(f'{where}: {writer} lies outside the section {section}')
            if writer == DISC:
                continue
            input_name = self.input_section(placement)
            if outputs.setdefault((writer, input_name), section) != section:
                fail(f'{where}: {writer} places its {input_name} in two output sections')
            if not Path(writer).is_file():
                fail(f'{where}: {writer}: no such file')
            stem = Path(writer).with_suffix('')
            if stems.setdefault(stem, writer) != writer:
                fail(f'{where}: {writer} and {stems[stem]} would build one object')
        owned = sorted((row for row in self.placements if row.writer != DISC), key=lambda row: row.start)
        for first, second in zip(owned, owned[1:]):
            if second.start < first.end:
                fail(f'{at[second]}: {second.writer} starts inside {first.writer}, at {at[first]}')
        copied = sorted(self.disc(), key=lambda row: row.start)
        for first, second in zip(copied, copied[1:]):
            if second.start < first.end:
                fail(f'{at[second]}: a {DISC} row starts inside the one at {at[first]}')
        for row in copied:
            if zeroed(row.section):
                fail(f'{at[row]}: a {DISC} row in {row.section}, and bss holds zeros, which the disc need not give')
            over = [each for each in owned if each.start < row.end and row.start < each.end]
            if over and not (len(over) == 1 and over[0].start <= row.start and row.end <= over[0].end):
                fail(f'{at[row]}: a {DISC} row crosses the edge of {over[0].writer}, at {at[over[0]]}: '
                     'it lies inside one source row, or in none')

    @property
    def base(self):
        """The linked address of the image's first byte."""
        return self.sections[0][1]

    @property
    def compressed(self):
        """Whether the disc stores the image as PRS."""
        return self.file.endswith('.PRS')

    def sources(self):
        """Every source the image places, in table order."""
        return list(dict.fromkeys(row.writer for row in self.placements if row.writer != DISC))

    def input_section(self, row):
        """The object's section at row. Its output section is row.section."""
        return self.input_sections.get(row, row.section)

    def ranges(self, source):
        """Each (input section, start, end) placed from source, in table order."""
        return [(self.input_section(row), row.start, row.end) for row in self.placements if row.writer == source]

    def held(self, source):
        """source's ranges that its linked source holds: every one but its bss."""
        return [row for row in self.ranges(source) if not zeroed(row[0])]

    def disc(self):
        """The rows of the ranges the build takes from the disc, in table order."""
        return [row for row in self.placements if row.writer == DISC]

    def unmatched(self, source):
        """The disc's ranges inside source's rows, as (start, end) in address
        order: the parts of source's object that don't match the original yet,
        where the disc's bytes stand."""
        own = self.ranges(source)
        return sorted((row.start, row.end) for row in self.disc()
                      if any(start <= row.start and row.end <= end for _, start, end in own))

    def bounds(self):
        """Where the image's link laid out each section whose bounds a table
        of the image holds, as (name, start, end) in the order of bounds.txt's
        rows. A row is SECTION START END, and the rows run in address order.
        An image without bounds.txt gives none.

        Each bound is checked against splits.txt. A section without a row
        there holds no byte, so its START is its END. A section with one
        starts where its row does. It ends where its row ends, or where its
        last placement ends, source or disc. Where that placement is the
        disc's and runs to the row's end, it may end where the placement
        starts: the gap the image's link left after the section's last
        object."""
        path = self.path / 'bounds.txt'
        if not os.path.lexists(path):
            return []
        found, rowed = [], {name: (start, end) for name, start, end in self.sections}
        for where, row in rows(path):
            if len(row) != 3 or not BOUNDED.fullmatch(row[0]):
                fail(f'{where}: not a row: SECTION START END')
            start, end = address(row[1], where), address(row[2], where)
            if end < start:
                fail(f'{where}: {row[0]} ends at {hexa(end)}, before its start')
            if any(name == row[0] for name, _, _ in found):
                fail(f'{where}: a second row for {row[0]}')
            if found and start < found[-1][1]:
                fail(f'{where}: {row[0]} starts at {hexa(start)}, before {found[-1][0]}, out of address order')
            if row[0] not in rowed and start != end:
                fail(f'{where}: {row[0]} has no row in {self.splits}, so it holds no byte and ends where it starts')
            if row[0] in rowed and start != rowed[row[0]][0]:
                fail(f"{where}: {row[0]} starts at {hexa(start)}, and its row in {self.splits} at "
                     f'{hexa(rowed[row[0]][0])}')
            if row[0] in rowed and end not in self.section_ends(row[0]):
                fail(f'{where}: {row[0]} ends at {hexa(end)}, and {self.splits} ends it at '
                     f'{" or ".join(map(hexa, self.section_ends(row[0])))}')
            found.append((row[0], start, end))
        return found

    def section_ends(self, section):
        """Where the image's link can have ended section, which has a row, as
        bounds() says, in address order: its row's end, its last
        placement's, and that placement's start where it is the disc's and
        runs to the row's end."""
        end = next(high for name, _, high in self.sections if name == section)
        placed = [row for row in self.placements if row.section == section]
        # a disc row that ends where a source's does lies inside it, and isn't the gap after the last object
        last = max(placed, key=lambda row: (row.end, row.writer != DISC), default=None)
        ends = {end} if last is None else {end, last.end} | ({last.start} if last.writer == DISC and last.end == end
                                                              else set())
        return sorted(ends)

    def symbols(self):
        """The (address, name) pairs the image's code or data can use: the
        rows of its own symbols.txt, then, for an image other than the main
        executable, each row of the main executable's whose name its own
        doesn't give."""
        own = named(self.path / 'symbols.txt')
        if self.name == MAIN:
            return own
        given = {name for _, name in own}
        return own + [(address, name) for address, name in named(CONFIG / MAIN / 'symbols.txt') if name not in given]

    @cached_property
    def symbol_addresses(self):
        """An address lookup shared by the objects linked against this image's tables."""
        return {name: address for address, name in reversed(self.symbols())}


def at_root(*paths):
    """Fail unless the working directory holds each of paths, as the
    repository's root does. A path that ends in / names a directory."""
    for path in paths:
        if not (Path(path).is_dir() if path.endswith('/') else Path(path).is_file()):
            fail(f"{path}: not found; run tools from the repository's root")


def images():
    """The name of every image config/ describes, a directory there, in name
    order. A working directory without config/ fails."""
    at_root(f'{CONFIG}/')
    return sorted(path.name for path in CONFIG.iterdir() if path.is_dir())
