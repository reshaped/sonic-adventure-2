"""Link an object where its image's tables place its source, with R32's
asmsh, lnk and cnvs.

    python3 tools/link.py build/obj/src/system/task.obj 1st_read
    python3 tools/link.py --rows

The linked source is the bytes of the source's ranges, joined in table order,
but for its bss, which holds no bytes to link. It goes to the source's path
under build/link/IMAGE/, with .bin for its suffix, and the link's intermediate
files go beside it. WIBO and R32 in the environment name wibo and R32's
directory. HUNG and GRACE give run()'s two timeouts, as timeout(1) reads a
duration. The Makefile exports all four.

Beside the linked source, with .rows for its suffix, goes what the link read
of the tables: each of the source's ranges, then each name the object
defines or takes with the address the tables give it, or - where they give
none, then each bound the link took, or - - where bounds.txt now gives
none. --rows rewrites each such file whose rows the tables now give
otherwise, and links nothing: make runs it as it starts, so that it relinks
only what a table's edit reaches. It reads an image's tables only where one,
or the directory that holds them, is newer than a file of its rows: a table
deleted or added changes its directory.

Each of the object's sections starts at its source's first row in that
section, and the section's last row ends where the object's section does: the
object's size and alignment are evidence for its rows. A section that holds
bytes and has no row fails the link, and so does a row for a section the
object lacks.

An image's bounds.txt gives where its link laid out the sections whose bounds
a table of the image holds, as sdk/shinobi/strt1.src's do by STARTOF and
SIZEOF. The link takes those bounds only for an object whose relocations ask
a section's start or size, as sysrof.bounded() reads them, and an object that
asks a section bounds.txt doesn't give fails. An empty section of such an
object that no row places starts at its bound. The link reserves the rest
of each bound after the object's bytes, in a section of the same name and
kind, code or data, which lnk joins to the object's. STARTOF and SIZEOF then
give the image's own bounds. A section that starts anywhere but its bound's
start fails the link, and so does one that runs past its bound's end, or
ends short of it by less than its alignment, which a reserve can't fill. Any
other object links as though the image had no bounds.

The link defines each name the object takes from outside at the address the
image's tables give it. A name they don't give fails the link, and so does one
they put inside the source's own rows, which is the object's to define. The
one exception is a name at the address of a label the object exports from a
code section, as a function's is: it is a second name for that label
(TOOLCHAIN.md#second-names). Each name the object defines for other objects
must have a row at the address the link gives it, so a table can't name a
function or a variable where it isn't. An export in an empty section that
nothing places fails the link.

A byte of the source's ranges that the object leaves unwritten is 0. A byte
the object writes outside those ranges, or in bss, fails the link.
"""

import argparse
import os
import re
import signal
import subprocess
import sys
from pathlib import Path, PureWindowsPath

import sysrof
from tool_process import seconds, stop
from config import CONFIG, LINKS, MAIN, OBJECTS, Image, aligned, code, content, fail, hexa, images, save, zeroed


WIBO = os.environ.get('WIBO', 'tools/wibo-i686')
R32 = Path(os.environ.get('R32', 'tools/shc-v5.0r32'))
# seconds before run() takes a tool for hung and stops it, and seconds a
# stopped tool has to exit on TERM before KILL: the Makefile's HUNG and
# GRACE, which it exports. A tool under wibo now and then deadlocks
HUNG = seconds('HUNG', 600)
GRACE = seconds('GRACE', 10)
SRECORD = re.compile(r'S[0-9](?:[0-9A-F]{2})+')
# the address width in bytes of each S-record kind cnvs writes. S0 is the
# header, S1 to S3 write data at their address, and S7 to S9 end the file.
ADDRESS_WIDTH = {0: 2, 1: 2, 2: 3, 3: 4, 7: 4, 8: 3, 9: 2}
# the characters lnk reads of a line of its commands
LINE = 510


def dos(path):
    """path as asmsh, lnk and cnvs take one: they read / as an option's prefix."""
    return str(PureWindowsPath(path))


def attempt(argv, stdin):
    """Return the tool's output, exit status and whether its supervisor timed out."""
    # in a session of its own, the tool and what it starts form a process
    # group, which stop() can end without signalling make's
    try:
        tool = subprocess.Popen([WIBO, *map(str, argv)], stdin=None if stdin is None else subprocess.PIPE,
                                stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True, errors='replace',
                                start_new_session=True)
    except OSError as error:
        fail(f"{WIBO}: can't run it: {error.strerror}")
    with tool:
        try:
            output = tool.communicate(stdin, timeout=HUNG)[0]
            return output, tool.returncode, False
        except subprocess.TimeoutExpired as error:
            stop(tool, GRACE)
            output = (error.output or b'').decode(errors='replace')
            return output, tool.returncode, True
        except BaseException:
            # ^C, or a signal stoppable() turns into SystemExit, reaches this process and not the tool
            stop(tool, GRACE)
            raise


def run(*argv, stdin=None):
    """Run a linker tool, retaining diagnostics from the sole optional timeout retry."""
    name = Path(argv[0]).name
    retry = os.environ.get('RETRY_WIBO_TIMEOUT') == '1'
    for number in (1, 2):
        output, status, expired = attempt(argv, stdin)
        # lnk and asmsh can exit successfully after warnings. Diagnose them
        # even if the tool went on to hang.
        problems = [line.strip(': ') for line in output.splitlines() if re.search(r'\*\* |\([EWF]\) ', line)]
        if expired:
            message = f'{name} ran {HUNG:g} seconds and was stopped'
            if not (retry and number == 1 and not problems):
                fail(*output.splitlines(), message)
            print(*output.splitlines(), f'{message}; retrying once', sep='\n', file=sys.stderr, flush=True)
            continue
        if status or problems:
            detail = [f'{name} exited with status {status}'] if status else []
            fail(*(problems or output.splitlines()), *detail)
        if number == 2:
            print(f'{name}: wibo retry succeeded', file=sys.stderr, flush=True)
        return


def srecords(path):
    """The bytes the S-records at path write, by address.

    A line fails where it isn't an S-record, its count or checksum is wrong,
    its kind is one cnvs doesn't write, or it ends the file and holds data.
    The file fails unless it ends with one end record. A second write to one
    address fails too."""
    written, ended = {}, False
    for number, record in enumerate(content(path).decode('latin-1').splitlines(), 1):
        where = f'{path}:{number}'
        if ended or not SRECORD.fullmatch(record):
            fail(f'{where}: {"a record after the end" if ended else "not an S-record"}')
        raw = bytes.fromhex(record[2:])
        if raw[0] != len(raw) - 1 or sum(raw) & 0xFF != 0xFF:
            fail(f'{where}: its count or checksum is wrong')
        kind = int(record[1])
        if kind not in ADDRESS_WIDTH:
            fail(f"{where}: S{kind}, a record cnvs doesn't write")
        # raw holds the count, the address, any data and the checksum
        width = ADDRESS_WIDTH[kind]
        if len(raw) < width + 2:
            fail(f'{where}: S{kind} too short for its {width}-byte address')
        if kind in (1, 2, 3):
            first = int.from_bytes(raw[1:width + 1], 'big')
            for at, byte in enumerate(raw[width + 1:-1], first):
                if at in written:
                    fail(f'{where}: writes {hexa(at)} again')
                written[at] = byte
        elif kind in (7, 8, 9):
            if len(raw) != width + 2:
                fail(f'{where}: S{kind} holds data after its address')
            ended = True
    if not ended:
        fail(f'{path}: no end record')
    return written


def starts(source, ranges, declared, bounds=()):
    """Where the link starts each of the object's sections that has a row, or
    a bound, as {name: address} in the order lnk starts them: by address,
    and at one address in the bounds' order, then the object's. ranges are
    source's rows, as (section, start, end) in table order, declared is the
    object's section table, and bounds the image's, as Image.bounds() gives
    them. An empty section that no row places starts at its bound's start.

    Each of these fails, naming source: a row for a section the object lacks;
    a section that holds bytes and has no row; and a section whose last row
    doesn't end where the object's section does."""
    first, last = {}, {}
    for name, start, end in ranges:
        if name not in declared:
            fail(f'{source}: its {name} row names no section of its object')
        first.setdefault(name, start)
        last[name] = max(end, last.get(name, end))
    given = {name: start for name, start, _ in bounds}
    for name, section in declared.items():
        if name not in first:
            if section.size:
                fail(f"{source}: its object's {name} holds {section.size:#x} bytes, and no row places it")
            if name in given:
                first[name] = given[name]
        elif last[name] != first[name] + section.size:
            fail(f"{source}: its {name} rows end at {hexa(last[name])}, and its object's {name} at "
                 f'{hexa(first[name] + section.size)}')
    # the bounds give the image's order of the sections they hold, which keeps
    # an empty section ahead of one at its address that holds bytes or a
    # reserve
    rank = {name: index for index, name in enumerate(dict.fromkeys([*given, *declared]))}
    return dict(sorted(first.items(), key=lambda item: (item[1], rank[item[0]])))


def reserves(source, placed, declared, bounds):
    """The space the link reserves after the object's bytes, as (name, size,
    alignment, code) for each section of placed, {name: address}, that
    bounds holds: the rest of the bound past the object's section, which lnk
    starts on the section's alignment. declared is the object's section
    table, and bounds the bounds the link takes, as taken() gives them.

    Each of these fails, naming source: a section placed anywhere but its
    bound's start, where STARTOF wouldn't give the bound's start; one that
    runs past its bound's end; and one that ends short of it by less than
    its alignment, where SIZEOF would give the object's size."""
    found = []
    for name, start, end in bounds:
        if name not in placed:
            continue
        section = declared[name]
        ends, after = start + section.size, aligned(start + section.size, section.alignment)
        if placed[name] != start:
            fail(f"{source}: its object's {name} starts at {hexa(placed[name])}, and its bound at {hexa(start)}")
        if ends > end:
            fail(f"{source}: its object's {name} runs to {hexa(ends)}, past its bound's end at {hexa(end)}")
        if ends < end < after:
            fail(f"{source}: its object's {name} ends at {hexa(ends)}, and its bound at {hexa(end)}, short of the "
                 f'{section.alignment}-byte alignment a reserve starts on')
        if after < end:
            found.append((name, end - after, section.alignment, section.code))
    return found


def code_exports(exported, placed):
    """The addresses of the labels an object exports from its code sections,
    as config.code() names them: a function's, or any other label there.
    exported is the object's exports, and placed where each of its sections
    starts."""
    return {placed[section] + offset for section, offset in exported.values() if code(section)}


def bound(source, image, used, ranges, exports=()):
    """The (address, name) pairs the link defines for source's object: each
    name in used, the object's references, at the address image's tables give
    it. A name the tables don't give fails, as does one they put inside
    ranges, source's own rows. A name the tables put at an address in
    exports, code_exports()'s, is a second name for that label, and passes."""
    given = image.symbol_addresses
    names = sorted(name[1:] for name in used if name.startswith('_'))
    missing = [name for name in names if name not in given]
    if missing or len(names) != len(used):
        strange = sorted(name for name in used if not name.startswith('_'))
        fail(*(f'{source}: it takes {name} from outside, and no symbols.txt of {image.name} gives it'
               for name in missing + strange))
    inside = [name for name in names
              if given[name] not in exports and any(start <= given[name] < end for _, start, end in ranges)]
    if inside:
        fail(*(f'{source}: it takes {name} from outside, and the tables put it at {hexa(given[name])}, '
               'inside its own rows' for name in inside))
    return [(given[name], name) for name in names]


def defined(source, image, exported, placed):
    """Fail unless image's tables give each name source's object defines the
    address the link gives it. exported is the object's exports, and placed
    where each of its sections starts. An export in a section placed lacks
    fails too."""
    given = image.symbol_addresses
    unplaced = sorted((name, section) for name, (section, _) in exported.items() if section not in placed)
    if unplaced:
        fail(*(f'{source}: it defines {sysrof.c_name(name)} in its {section}, which holds no byte, and no row or '
               'bound places it' for name, section in unplaced))
    faults = []
    for name, (section, offset) in sorted(exported.items(), key=lambda item: placed[item[1][0]] + item[1][1]):
        at, plain = placed[section] + offset, sysrof.c_name(name)
        if plain not in given:
            faults.append(f'{source}: it defines {plain} at {hexa(at)}, and no symbols.txt of {image.name} gives it')
        elif given[plain] != at:
            faults.append(f'{source}: it defines {plain} at {hexa(at)}, and the tables put it at {hexa(given[plain])}')
    if faults:
        fail(*faults)


def define(syms, names, reserved=()):
    """Assemble an object that exports each of names, (address, name), as
    _name at that address, and return its path: syms with .obj for its suffix.
    It reserves each of reserved, (name, size, alignment, code), as size
    bytes on alignment in a section of that name, code where code is true and
    data otherwise. The object's assembly text goes to syms."""
    lines = ['        .SECTION SYMBOLS,DATA,ALIGN=4']
    for address, name in names:
        lines += [f'        .EXPORT _{name}', f"_{name}: .EQU H'{address:08x}"]
    for name, size, alignment, holds_code in reserved:
        lines += [f'        .SECTION {name},{"CODE" if holds_code else "DATA"},ALIGN={alignment}',
                  f'        .RES.B {size}']
    save(syms, '\n'.join([*lines, '        .END', '']).encode())
    obj = syms.with_suffix('.obj')
    run(R32 / 'asmsh.exe', dos(syms), f'-object={dos(obj)}', '-cpu=sh4', '-endian=little')
    return obj


def place(objects, placed, out, options=()):
    """The bytes a link of objects writes, by address. lnk links objects in
    order and starts each section of placed, {name: address}, at its address.
    The sections go to lnk in address order, a start command for each
    address, which names the sections that start there in placed's order:
    lnk lays them out one after another from there, so placed's order puts
    an empty section ahead of one that holds bytes at its address, as
    starts() orders it. options are lnk commands given ahead of the start
    commands. A command longer than the LINE characters lnk reads of a line
    fails, naming out. cnvs then converts the link to S-records. lnk's output
    is out with .abs for its suffix, and cnvs's is out with .mot."""
    absolute, records = out.with_suffix('.abs'), out.with_suffix('.mot')
    groups = {}
    # sorted() keeps placed's order among sections at one address
    for name, address in sorted(placed.items(), key=lambda item: item[1]):
        groups.setdefault(address, []).append(name)
    # leading zeros keep an address a number in lnk's command
    commands = [*(f'input {dos(obj)}' for obj in objects), f'output {dos(absolute)}', 'form a', *options,
                *(f'start {",".join(names)}({address:08x})' for address, names in groups.items()), 'exit', '']
    long = [command for command in commands if len(command) > LINE]
    if long:
        fail(*(f"{out}: lnk reads {LINE} characters of a line, and this command's {len(command)}: {command}"
               for command in long))
    run(R32 / 'lnk.exe', '-noecho', stdin='\n'.join(commands))
    run(R32 / 'cnvs.exe', dos(absolute), dos(records))
    return srecords(records)


def joined(source, image, written):
    """source's linked source for image: the bytes of the ranges
    image.held() gives it, joined in table order. written is the bytes the
    link wrote, by address. A byte it didn't write is 0. An address in
    written outside those ranges fails."""
    held = image.held(source)
    stray = sorted(at for at in written if not any(start <= at < end for _, start, end in held))
    if stray:
        bss = any(zeroed(name) and start <= stray[0] < end for name, start, end in image.ranges(source))
        fail(f'{source}: {len(stray)} bytes at {hexa(stray[0])} fall '
             f'{"in its bss" if bss else f"outside its rows in {image.splits}"}')
    return b''.join(bytes(written.get(at, 0) for at in range(start, end)) for _, start, end in held)


def taken(source, image, asked):
    """The bounds a link of source takes: image's bounds of each section
    named in asked, the sections whose start or size its object asks, as
    (name, start, end) in bounds.txt's order. A section of asked that
    bounds.txt doesn't bound fails, naming source."""
    given = [row for row in image.bounds() if row[0] in asked]
    missing = sorted(set(asked) - {name for name, _, _ in given})
    if missing:
        fail(*(f"{source}: it takes the start or the size of {name}, and {image.path / 'bounds.txt'} gives no bound "
               'of it' for name in missing))
    return given


def rows_given(image, source, names, asked=()):
    """The rows image's tables give a link of source, as text: each of
    source's ranges, then each of names, as C spells them, with the address
    the tables give it, or - where they give none, then the bound of each
    section of asked, after the word bound, in bounds.txt's order, and then
    each section of asked that bounds.txt doesn't bound, with - - for its
    bound. A link fails on such a section first, in taken(), and --rows
    records it, so that make relinks the object that asked it."""
    given = image.symbol_addresses
    ranges = [f'{section} {hexa(start)} {hexa(end)}' for section, start, end in image.ranges(source)]
    addresses = [f'{name} {hexa(given[name]) if name in given else "-"}' for name in sorted(names)]
    found = [row for row in image.bounds() if row[0] in asked]
    bounds = [f'bound {name} {hexa(start)} {hexa(end)}' for name, start, end in found]
    bounds += [f'bound {name} - -' for name in sorted(set(asked) - {name for name, _, _ in found})]
    return '\n'.join([*ranges, *addresses, *bounds, ''])


def refresh(links=LINKS):
    """Rewrite each link's rows under LINKS whose rows the tables now give
    otherwise. An image whose tables, and the directories that hold them,
    are all older than each file of its rows is skipped, and so is a file of
    an image or source the tables no longer place."""
    for name in images():
        held = sorted((links / name).rglob('*.rows'))
        tables = [CONFIG / name / table for table in ('splits.txt', 'symbols.txt', 'bounds.txt')]
        # a table deleted leaves no time of its own, but changes its directory's
        tables += [CONFIG / name, CONFIG / MAIN / 'symbols.txt', CONFIG / MAIN]
        newest = max(path.stat().st_mtime_ns for path in tables if path.exists())
        if not held or all(path.stat().st_mtime_ns > newest for path in held):
            continue
        image = Image(name)
        placed = {Path(source).with_suffix(''): source for source in image.sources()}
        for path in held:
            source = placed.get(path.relative_to(links / name).with_suffix(''))
            if source is None:
                continue
            old = content(path).decode()
            # a range has three fields, a name two and a bound four, so a name or
            # a section may be called bound
            lines = [line.split() for line in old.splitlines()]
            names = [fields[0] for fields in lines if len(fields) == 2]
            # the sections whose bounds the link took, which only a new object can change
            asked = [fields[1] for fields in lines if len(fields) == 4 and fields[0] == 'bound']
            rows = rows_given(image, source, names, asked)
            if rows != old:
                save(path, rows.encode())


def main(argv):
    parser = argparse.ArgumentParser(prog='link', description=__doc__.split('\n\n')[0])
    parser.add_argument('obj', metavar='OBJECT', type=Path, nargs='?',
                        help=f"a source's path under {OBJECTS}/, with .obj for its suffix")
    parser.add_argument('image', metavar='IMAGE', nargs='?', help='the image to link for, config/IMAGE/')
    parser.add_argument('--rows', action='store_true',
                        help='rewrite each link\'s rows the tables now give otherwise, and link nothing')
    parser.add_argument('--stub', action='store_true', help='link a disc-free layout object under build/stub/')
    args = parser.parse_args(argv)
    if args.rows == bool(args.obj or args.image) or bool(args.obj) != bool(args.image):
        parser.error('give OBJECT and IMAGE, or --rows alone')
    if args.rows:
        if args.stub:
            parser.error('--rows refreshes both kinds of link')
        refresh()
        if Path('build/stub/link').is_dir():
            refresh(Path('build/stub/link'))
        return
    if args.obj.suffix != '.obj':
        fail(f'{args.obj}: not an object, with .obj for its suffix')
    if not args.obj.is_file():
        fail(f'{args.obj}: no such object')
    objects = Path('build/stub/obj') if args.stub else OBJECTS
    links = Path('build/stub/link') if args.stub else LINKS
    try:
        held = args.obj.resolve().relative_to(objects.resolve())
    except ValueError:
        fail(f'{args.obj}: not under {objects}/')
    # asmsh and cnvs allow one dot in a file's name, and the link's files take the object's stem
    if '.' in held.stem:
        fail(f"{args.obj}: its stem holds a dot, and asmsh and cnvs allow one in a file's name")
    # lnk rejects an absolute path, so its commands name the object under build/
    obj, stem = objects / held, held.with_suffix('')
    image = Image(args.image)
    source = next((each for each in image.sources() if Path(each).with_suffix('') == stem), None)
    if source is None:
        fail(f'{stem}: no row of {image.splits} places it')
    out = links / image.name / stem.with_suffix('.bin')
    ranges = image.ranges(source)
    exported, referenced, declared = sysrof.exports(obj), sysrof.references(obj), sysrof.sections(obj)
    asked = sysrof.bounded(obj)
    bounds = taken(source, image, asked)
    placed = starts(source, ranges, declared, bounds)
    defined(source, image, exported, placed)
    syms = define(out.with_name(out.stem + '_syms.src'),
                  bound(source, image, referenced, ranges, code_exports(exported, placed)),
                  reserves(source, placed, declared, bounds))
    linked = joined(source, image, place([obj, syms], placed, out))
    # the rows go first, so the linked source is never older than they are
    names = set(map(sysrof.c_name, [*exported, *referenced]))
    save(out.with_suffix('.rows'), rows_given(image, source, names, asked).encode())
    save(out, linked)


def stoppable():
    """Have a hangup, ^\\ or TERM raise SystemExit with status 128 plus the
    signal's number, as ^C raises KeyboardInterrupt, so run() stops its tool
    on any of them. A signal ignored at the start, as under nohup, stays
    ignored."""
    for number in (signal.SIGHUP, signal.SIGQUIT, signal.SIGTERM):
        if signal.getsignal(number) != signal.SIG_IGN:
            signal.signal(number, lambda received, _: sys.exit(128 + received))


if __name__ == '__main__':
    stoppable()
    main(sys.argv[1:])
