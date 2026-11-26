"""Hold each of the disc's files to the room its load gives it in the GCC
build.

    python3 tools/dc_loads.py [DIRECTORY]

make dc's loader refuses a file that doesn't fit the room its caller passes,
where the shipped loader reads the file unchecked. The port chose each room,
and no goal holds a room to the disc's files, so a room can be smaller than
a file the disc holds: an attract demo's file had the room of a mini
event's, and each of the disc's six decodes to 64 KB more.
config/loads.txt lists the loads, and its head gives a row's form and what
the table leaves out.

Without DIRECTORY, a run holds the table to the tree. It fails where:

- a row is out of the form the table's head gives;
- a row's header gives no number for its room, as make dc's GCC compiles
  the header;
- a row's source, as make dc's GCC compiles it, doesn't name each macro of
  the row's room;
- a row's pattern names no file of config/jp-disc.sha256, the disc's
  inventory.

With DIRECTORY, an extraction of the disc's files as make dc-disc's
DISC_FILES names one, a run then reads each file a row names. It fails
where:

- DIRECTORY lacks the file, or holds one whose SHA-256 isn't the
  inventory's;
- the file's whole sectors pass the bytes its load reads it into: the room
  for a file or a buffer row, and WORK_AREA for a prs row;
- build/prskit can't decode a prs or a buffer row's file. It refuses a
  stream with bytes, or set bits of its last control byte, after the end
  marker, which the loader's decoder takes;
- the file decodes to more bytes than the room, or for an exactly row to
  any other count.

A failure of a size names the file, its size and the room. A buffer row's
load decodes a file in the room it read the file into, from a copy on the
heap where the output would pass input not yet read. A run measures neither
that lead nor the heap.

Otherwise a run prints its count of the loads, and of the disc's files they
name or, with DIRECTORY, that fit their rooms.
"""
import argparse
import fnmatch
import re
import subprocess
import tempfile
from collections import namedtuple
from pathlib import Path

import csource
from config import at_root, fail, rows
from dc_extracted import CATALOG, inventory
from dc_memory import AREAS_H, numbers
from disc import sha256
from tool_process import run

LOADS = Path('config/loads.txt')
# the header that gives a sector's bytes, GDD_FS_SCTSIZE
SHINOBI_H = Path('sdk/include/shinobi.h')
FILE, PRS, BUFFER = 'file', 'prs', 'buffer'
FORMS = (FILE, PRS, BUFFER)
WITHIN, EXACTLY = 'within', 'exactly'
BOUNDS = (WITHIN, EXACTLY)
MACRO = r'[A-Z_][A-Z0-9_]*'
# a row's ROOM: a macro, or the difference of two
ROOM = re.compile(rf'({MACRO})(?:-({MACRO}))?')

# a row: its pattern, form and bound, its room as the row spells it and in
# bytes, the disc's files it names, in path order, and its path:line
Load = namedtuple('Load', 'pattern form bound room bytes files where')


def room(header, spelled, where):
    """The bytes of a row's ROOM, spelled, by the macros of its HEADER. One
    out of ROOM's form fails, naming where, as does one the header gives no
    number for."""
    names = ROOM.fullmatch(spelled)
    if names is None:
        fail(f'{where}: {spelled}: not a room, a macro or NAME-NAME')
    wanted = [name for name in names.groups() if name]
    try:
        given = numbers(Path(header), wanted)
    except SystemExit:
        fail(f'{where}: {header} gives no number for {spelled}')
    return (given[wanted[0]] - (given[wanted[1]] if len(wanted) == 2 else 0)) % 2**32


def loads(path=LOADS):
    """The rows of the table at path, as Loads, in order. It fails, naming
    the row's path:line, where the module's docstring says a run without
    DIRECTORY fails."""
    names = sorted(inventory(CATALOG))
    found = []
    for where, fields in rows(path):
        if len(fields) < 6:
            fail(f'{where}: not a row: FILES FORM BOUND HEADER ROOM SOURCE')
        pattern, form, bound, header, spelled, source = fields[:6]
        if form not in FORMS:
            fail(f'{where}: {form}: not a form, one of {", ".join(FORMS)}')
        if bound not in BOUNDS or form == FILE and bound == EXACTLY:
            fail(f'{where}: {bound}: not a bound of a {form} row, '
                 f'{WITHIN if form == FILE else " or ".join(BOUNDS)}')
        size = room(header, spelled, where)
        words = set(re.findall(MACRO, csource.code(source, csource.GCC)))
        missing = [name for name in ROOM.fullmatch(spelled).groups() if name and name not in words]
        if missing:
            fail(f"{where}: {source} doesn't name {missing[0]}, which the row's room reads")
        files = [name for name in names if fnmatch.fnmatchcase(name, pattern)]
        if not files:
            fail(f'{where}: {pattern}: names no file of {CATALOG}')
        found.append(Load(pattern, form, bound, spelled, size, files, where))
    return found


def held(load, name, directory, sector, work, prskit, scratch):
    """The problem with the disc's file name in directory as load reads it,
    or None. sector is a sector's bytes, and work WORK_AREA's. prskit decodes
    a PRS file into scratch, a path the call may write."""
    path = directory / name
    size = path.stat().st_size
    sectors = -(-size // sector)
    if load.form != PRS and sectors > load.bytes // sector:
        return (f'{path}: its {size} bytes take {sectors} sectors, past its room at {load.where}, {load.room}, '
                f'{load.bytes} bytes')
    if load.form == FILE:
        return None
    if load.form == PRS and sectors > work // sector:
        return (f"{path}: its {size} bytes take {sectors} sectors, past WORK_AREA's {work} bytes, where its load "
                f'at {load.where} reads it')
    decoder = run([prskit, 'decompress', path, scratch], stderr=subprocess.PIPE)
    if decoder.returncode:
        # prskit's own words, which follow the file's name
        said = decoder.stderr.decode(errors='replace').strip().rpartition(f'{path}: ')[2]
        return f"{path}: {prskit} can't decode it, for its load at {load.where}: {said}"
    decoded = scratch.stat().st_size
    if load.bound == EXACTLY and decoded != load.bytes:
        return (f'{path}: decodes to {decoded} bytes, where its load at {load.where} takes only {load.room}, '
                f'{load.bytes} bytes')
    if decoded > load.bytes:
        return f'{path}: decodes to {decoded} bytes, past its room at {load.where}, {load.room}, {load.bytes} bytes'
    return None


def problems(found, directory, prskit):
    """Each problem with the disc's files in directory as found, loads()'s
    rows, read them, as the module's docstring lists them, in the rows'
    order. prskit is the decoder's path."""
    digests = inventory(CATALOG)
    sector = numbers(SHINOBI_H, ('GDD_FS_SCTSIZE',))['GDD_FS_SCTSIZE']
    area = numbers(AREAS_H, ('WORK_AREA', 'WORK_AREA_END'))
    work = area['WORK_AREA_END'] - area['WORK_AREA']
    faults, checked = [], {}
    with tempfile.TemporaryDirectory() as scratch:
        for load in found:
            for name in load.files:
                if name not in checked:
                    path = directory / name
                    checked[name] = (f'{path}: missing from the extraction' if not path.is_file()
                                     else f"{path}: not the Japanese disc's file, by {CATALOG}"
                                     if sha256(path) != digests[name] else None)
                    if checked[name]:
                        faults.append(checked[name])
                if not checked[name]:
                    fault = held(load, name, directory, sector, work, prskit, Path(scratch) / 'decoded')
                    if fault:
                        faults.append(fault)
    return faults


def main(argv=None):
    parser = argparse.ArgumentParser(prog='dc_loads', description=__doc__.split('\n\n')[0])
    parser.add_argument('directory', metavar='DIRECTORY', type=Path, nargs='?',
                        help="an extraction of the disc's files, to hold to the table")
    parser.add_argument('--prskit', type=Path, default=Path('build/prskit'), help='the PRS decoder make builds')
    args = parser.parse_args(argv)
    at_root(str(LOADS), str(CATALOG), str(AREAS_H), str(SHINOBI_H))
    try:
        found = loads()
        files = {name for load in found for name in load.files}
        if args.directory is None:
            print(f'{len(found)} loads name {len(files)} files of the disc')
            return
        if not args.prskit.is_file():
            fail(f'{args.prskit}: not found; run make build/prskit')
        faults = problems(found, args.directory, args.prskit)
    except (OSError, ValueError) as error:
        fail(error)
    if faults:
        fail(*faults)
    print(f"{len(files)} of the disc's files fit the rooms of {len(found)} loads")


if __name__ == '__main__':
    main()
