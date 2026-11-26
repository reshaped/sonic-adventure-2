"""Check that SHA256SUMS lists exactly the files make expects, in order, or
add the lines of new links.

    python3 tools/sums.py build/expected.txt
    python3 tools/sums.py --add LINK...

EXPECTED holds the paths make expects, split by blanks: the disc's files
under orig/, and each linked source under build/link/. SHA256SUMS has one
line for each, a hash, two spaces and the path, in path order as
LC_ALL=C sort -k 2 sorts them. A line ends at LF alone. It fails at the
first line that isn't UTF-8 or isn't that form, naming it. Once every line
is, it fails naming together each path out of order or listed twice, each
expected path with no line, and each line for a path make doesn't expect.

It checks the list, not the hashes: make checks those with sha256sum -c.

--add gives each LINK, a path under build/link/, a line with its hash, in
path order. It refuses a LINK with a blank, which no line can hold. It
refuses a LINK that SHA256SUMS lists already, by normal path on both sides,
so it never accepts a link that differs from its line: a hash changes only
by hand, for a reason. It refuses a SHA256SUMS that lists a path twice, as a
text merge can leave it, since writing the file again would keep one of the
two hashes.
"""

import argparse
import hashlib
import posixpath
import re
from collections import Counter

from config import LINKS, content, fail, save

SUMS = 'SHA256SUMS'
LINE = re.compile(r'([0-9a-f]{64})  (\S+)')
BLANK = re.compile(r'\s')


def lines(sums=SUMS):
    """Each line of the sums file at sums, as its path and its text, in its
    order. A line ends at LF alone, not at each boundary str.splitlines
    splits at, such as a vertical tab. A line that isn't UTF-8, or isn't a
    hash, two spaces and a path, fails, naming its sums:line."""
    data = content(sums)
    found = []
    for number, line in enumerate((data[:-1] if data.endswith(b'\n') else data).split(b'\n') if data else [], 1):
        try:
            line = line.decode()
        except UnicodeDecodeError:
            fail(f'{sums}:{number}: not UTF-8')
        match = LINE.fullmatch(line)
        if not match:
            fail(f'{sums}:{number}: not a line: 64 lowercase hex digits, two spaces and a path')
        found.append((match[2], line))
    return found


def listed(sums=SUMS):
    """The paths the sums file at sums lists, in its order, failing as lines()
    does."""
    return [path for path, _ in lines(sums)]


def doubled(found, sums=SUMS):
    """A fault for each path that found, the paths the sums file at path sums
    lists, holds more than once."""
    return [f'{sums}: {path} has {count} lines' for path, count in sorted(Counter(found).items()) if count > 1]


def problems(expected, found, sums=SUMS):
    """What keeps found, the paths a sums file lists in order, from being
    exactly expected, a set of paths, in path order: a line for each fault."""
    faults = [f'{sums}: {later} comes after {earlier}, out of path order'
              for earlier, later in zip(found, found[1:]) if later.encode() < earlier.encode()]
    faults += doubled(found, sums)
    faults += [f'{sums}: no line for {path}' for path in sorted(expected - set(found))]
    faults += [f'{sums}: a line for {path}, which make no longer builds or reads'
               for path in sorted(set(found) - expected)]
    return faults


def added(links, sums=SUMS):
    """The text of the sums file at sums with a line for each of links, in
    path order, each link by its normal path. A link it lists already fails,
    as does one with a blank or one that isn't a path under build/link/, and
    so does a file that lists a path twice."""
    found, links = lines(sums), [posixpath.normpath(link) for link in links]
    known = {posixpath.normpath(path) for path, _ in found}
    faults = doubled([path for path, _ in found], sums)
    faults += [f'{link}: not a path under {LINKS}/' for link in links if not link.startswith(f'{LINKS}/')]
    faults += [f'{link}: has a blank, and a line of {sums} holds a path without one'
               for link in links if BLANK.search(link)]
    faults += [f'{link}: {sums} lists it already, and a hash changes only by hand' for link in links if link in known]
    if faults:
        fail(*faults)
    text = {**dict(found), **{link: f'{hashlib.sha256(content(link)).hexdigest()}  {link}' for link in links}}
    return ''.join(f'{text[path]}\n' for path in sorted(text, key=str.encode))


def main(argv=None):
    parser = argparse.ArgumentParser(prog='sums', description=__doc__.split('\n\n')[0])
    parser.add_argument('expected', metavar='EXPECTED', nargs='?',
                        help='a file of the paths make expects, split by blanks')
    parser.add_argument('--add', metavar='LINK', nargs='+', help="give each LINK its line, in path order")
    args = parser.parse_args(argv)
    if (args.expected is None) == (args.add is None):
        parser.error('give EXPECTED, or --add and the links')
    if args.add is not None:
        save(SUMS, added(args.add).encode())
        return
    try:
        expected = set(content(args.expected).decode().split())
    except UnicodeDecodeError:
        fail(f'{args.expected}: not UTF-8')
    faults = problems(expected, listed())
    if faults:
        fail(*faults)


if __name__ == '__main__':
    main()
