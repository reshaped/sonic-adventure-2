"""Give each source its bin: who reads the floats it computes, as
config/parts.txt's rows have it.

config/parts.txt says of each source who reads the floats it computes, its
bin, which sets whether a build that isn't SHC's must keep the console's
bits there. Its head says what each bin is. A row is

    PATH  BIN

PATH is a file, or a directory, which ends in /, from the repository's
root. A source takes the row whose PATH is the longest that is the source
itself or a directory above it. A source is a file whose suffix is among
SOURCES, under one of ROOTS. BIN is one of BINS.

rows() reads the table. It fails, naming the row's path:line, on a row that
isn't PATH BIN, on a bin that isn't one of BINS, and on a second row for one
PATH. part() gives the row a source takes. faults() gives what the rows
leave wrong of a list of files: a source that takes no row, and a row that
no source takes. tests/test_parts.py holds the table to none.

source(), part() and faults() take each file's name as sources() spells it:
from the repository's root, with a / between its parts and no . or .. among
them. They match names as text, so a name spelled another way finds the
wrong row or none.
"""

from collections import namedtuple
from pathlib import Path

from config import fail, rows as table_rows

PARTS = Path('config/parts.txt')
# the directories whose sources take a row, and a source's suffixes: C, its
# headers, and assembly as asmsh reads it
ROOTS = ('src', 'sdk', 'modules', 'engine')
SOURCES = ('.c', '.h', '.src')
PLAY, PICTURE, NONE = 'play', 'picture', 'none'
BINS = (PLAY, PICTURE, NONE)

# a row's bin and its path:line
Part = namedtuple('Part', 'bin where')


def rows():
    """The rows of PARTS, as {PATH: Part}. Fails, naming the row's
    path:line, on a row out of the form the module docstring gives."""
    found = {}
    for where, fields in table_rows(PARTS):
        if len(fields) != 2:
            fail(f'{where}: not a row: PATH BIN')
        name, held = fields
        if held not in BINS:
            fail(f'{where}: {held}: not a bin, one of {", ".join(BINS)}')
        if name in found:
            fail(f'{where}: {name} has a row already, at {found[name].where}')
        found[name] = Part(held, where)
    return found


def source(path):
    """Whether path, a string, is a source: a file under one of ROOTS whose
    suffix is one of SOURCES."""
    return Path(path).suffix in SOURCES and path.startswith(tuple(f'{root}/' for root in ROOTS))


def part(found, path):
    """The Part of the row that path, a string, takes among found, rows()'s:
    the row whose PATH is the longest that is path or a directory above it.
    None where found has no such row, and where path is no source."""
    if not source(path):
        return None
    above = [at + 1 for at, character in enumerate(path) if character == '/']
    for end in (len(path), *reversed(above)):
        if path[:end] in found:
            return found[path[:end]]
    return None


def sources():
    """Each source under ROOTS, as a path from the root, in order."""
    return sorted(path.as_posix() for root in ROOTS for path in Path(root).rglob('*')
                  if path.is_file() and source(path.as_posix()))


def faults(found, files):
    """What found, rows()'s, leaves wrong of files, paths from the root: each
    source that takes no row, and each row that no source of files takes."""
    taken, problems = set(), []
    for path in files:
        row = part(found, path)
        if row is not None:
            taken.add(row.where)
        elif source(path):
            problems.append(f'{path}: no row of {PARTS} gives its bin: give it one, or a directory above it, '
                            'PATH BIN')
    problems += [f'{row.where}: {path}: no source takes this row: none lies at or under its PATH, or each that does '
                 "takes a longer row. Remove the row; a directory's PATH ends in /"
                 for path, row in found.items() if row.where not in taken]
    return problems
