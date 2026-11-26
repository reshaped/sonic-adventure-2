"""Check config/addresses.txt, which explains each number for RAM that the
port's sources, tables and generated data hold, against those numbers.

    python3 tools/addresses.py --bodies LIST [CONTENT ...]

make dc's links lay out every image anew. A number that holds an original
address keeps its meaning only where that place keeps its address, or where
the number is no address at all. The table explains each number for RAM by
a row of the number's file, whose fields and dispositions its header gives.
A number is for RAM where it lies in RAM's 16 MB through one of VIEWS: from
0x0c000000, 0x8c000000 or 0xac000000. A row counts the numbers in its span,
not which they are, so a number that takes another's place passes.

The numbers are:

- each integer constant of the C under SOURCE_ROOTS, outside comments and
  strings, on both sides of each guard, and each sum of the C, as below. The
  lines of a #pragma asm block or of an inline body hold SHC's assembly, and
  are read as assembly;
- each number of the assembly under SOURCE_ROOTS: H'hex or decimal, outside
  comments and strings;
- each address a symbols.txt gives outside its own image and the images
  loaded beside it: 1ST_READ.BIN beside every other image, and CHAO.PRS
  beside a Chao area. The port's final links bind each name their objects
  take to a definition they link, or fail (PORTS.md#the-census), so an
  address in one of those images needs no row;
- each integer constant of each body that LIST names, outside strings. A -
  before one takes its two's complement in 32 bits. LIST names each body
  source, a .c file, and then the bodies a build writes for it. The check
  skips a body that body_data.chunk_lists() reads from its source as a
  chunk list of a kind kinds.NUMBERS names. A vertex list's declaration
  types each word as a 32-bit integer, whatever the chunk holds there: its
  head, a float's bits or a packed color;
- each 4-aligned word that a CONTENT file, the assembly tools/content.py
  writes for an image, keeps from the disc by .incbin, unless the datum
  that holds it is of a kind kinds.NUMBERS names. The check reads the word
  as its bytes, so a float's bits count as any other word's. Its file is
  the image's data.txt. content.py relocates each pointer it knows, and
  keeps every other word as the disc holds it.

A sum is MACRO + N or MACRO - N, where N is an integer constant and MACRO is
a macro whose body holds one integer constant, a number for RAM, in the
sum's file or in a file that file includes. Its value is that number plus
or minus N, in 32 bits, as the SH-4 adds. The check takes no value for a
sum, and fails, where:

- MACRO's definitions there aren't all one;
- the sum is no whole operand, as where a * or a [ stands beside it;
- N doesn't count bytes: the body casts its number to neither an integer of
  32 bits nor a pointer to a byte, or a cast before the sum takes the
  number from a body that holds it in fewer bits.

The check holds the COUNT of each row of a file it reads to the numbers the
row explains, and fails on:

- a row out of the table's form, one whose span lies in two views or in
  none, and two rows of one file that span one number;
- a number no row of its file explains;
- a row whose COUNT isn't the number of numbers it explains;
- an anchor or a placed row whose NAME no C file or assembly under
  SOURCE_ROOTS spells;
- a row of a file it doesn't read, where no other run reads it either. A
  row of a body under the one of BODY_ROOTS that LIST's bodies lie outside
  is the other run's, and so is a row of a data.txt where no CONTENT is
  given;
- a sum whose value it can't take;
- an .incbin out of the form content.py writes, and a word of one that no
  datum holds.

The check sees no number for RAM split across constants, encoded otherwise
or built at run time. It decodes no floating constant, so a float of the C
or of a body whose bits are a number for RAM goes unseen.

make dc runs it with the full build's bodies, under build/gen/, and with
each image's CONTENT. make dc-check runs it with the stub build's, under
build/stub/, which need no disc.
"""

import argparse
import re
import struct
from bisect import bisect_right
from collections import Counter, namedtuple
from pathlib import Path

import layout
from body_data import chunk_lists
from config import ADDRESS, CHAO_AREAS, CONFIG, MAIN, NAME, Image, address, content, fail, hexa, images, named, rows
from content import objects
from csource import code, included, macro_directives, pairs, tokens
from inline import BLOCK, BODY
from kinds import NUMBERS

TABLE = CONFIG / 'addresses.txt'
# the directories whose C and assembly make dc compiles and assembles
SOURCE_ROOTS = ('src', 'modules', 'sdk', 'engine')
C_FILES = ('.c', '.h', '.inc')
ASSEMBLY = '.src'
# the starts of the views the game and its SDK reach RAM through: P0's and
# P1's, which the cache serves, and P2's, which it doesn't. RAM is 16 MB,
# 1 << 24 bytes, from each start, so an address's top byte gives its view
RAM = 0x01000000
VIEWS = (0x0C000000, 0x8C000000, 0xAC000000)
VIEW_BYTES = frozenset(view >> 24 for view in VIEWS)
# a byte of VIEW_BYTES, as a pattern over a run of bytes
VIEW_BYTE = re.compile(b'[' + re.escape(bytes(sorted(VIEW_BYTES))) + b']')
# what drops the view from an address, leaving its place in the physical map
PHYSICAL = 0x1FFFFFFF
# each disposition, with the number of fields it takes before the reason
DISPOSITIONS = {'anchor': 1, 'placed': 1, 'data': 0}
COUNT = re.compile(r'[1-9][0-9]*')
ASSEMBLY_NUMBER = re.compile(r"\bH'([0-9A-Fa-f]+)|\b([0-9]+)\b")
ASSEMBLY_STRING = re.compile(r'"[^"\n]*"')
ASSEMBLY_NAME = re.compile(r'\b[A-Za-z_]\w*')
# the directories of the bodies of make dc's run and of make dc-check's
BODY_ROOTS = ('build/gen/', 'build/stub/')
# an integer constant of a body, as tools/body_data.py writes one, with the -
# before it, where one stands. No . or exponent stands beside one, which
# would make it part of a float
BODY_NUMBER = re.compile(r'(?<![\w.])(-\s*)?(0[xX][0-9a-fA-F]+|[0-9]+)([uUlL]*)(?![\w.])')
BODY_STRING = re.compile(r'"(?:[^"\\\n]|\\.)*"')
# the tokens that join a sum to more beside it, so that it is no whole
# operand: JOINS_BEFORE before MACRO, and JOINS_AFTER after N. Each binds
# tighter than the sum's + or -, or makes the sum part of a longer one
JOINS_BEFORE = {'*', '/', '%', '+', '-', '~', '!', 'sizeof', '.', '->', ')'}
JOINS_AFTER = {'*', '/', '%', '+', '-', '[', '(', '.', '->'}
# the names that spell a byte, and those that spell an integer of 32 bits,
# which holds an address whole. Either may take a sign
BYTE = {'char', 'Sint8', 'Uint8'}
INTEGER = {'int', 'long', 'Sint32', 'Uint32'}
SIGNS = {'signed', 'unsigned'}
# an .incbin line of content's assembly: the file, the offset of its bytes
# and their count
INCBIN = re.compile(r'\.incbin "((?:[^"\\]|\\.)*)", ([0-9]+), ([0-9]+)')

# a row of the table. It spans the numbers of file from start up to end,
# which is start + 1 for a row without END. count is how many it says the
# file holds there. name is an anchor's or a placed row's NAME, and None for
# data. at is the row's path:line
Row = namedtuple('Row', 'file start end count name at')


def ram(value):
    """Whether value is an address of RAM in one of VIEWS."""
    return value >> 24 in VIEW_BYTES


def table():
    """The table's rows, each a Row, in its order. A row out of the form its
    header gives fails, as does one whose numbers aren't addresses of RAM in
    one view, and one that spans a number an earlier row of its file
    spans."""
    found = []
    for at, fields in rows(TABLE):
        form = f'{at}: not a row: FILE START [END] COUNT, then anchor NAME, placed NAME or data, then the reason'
        if len(fields) < 4:
            fail(form)
        file, start, *rest = fields
        start = address(start, at)
        end = address(rest.pop(0), at) if ADDRESS.fullmatch(rest[0]) else start + 1
        if len(rest) < 2:
            fail(form)
        count, disposition, *rest = rest
        if not COUNT.fullmatch(count) or disposition not in DISPOSITIONS or len(rest) <= DISPOSITIONS[disposition]:
            fail(form)
        if disposition != 'data' and not NAME.fullmatch(rest[0]):
            fail(f'{at}: {rest[0]}: no name, which a row that is {disposition} gives')
        last = end - 1
        if not (ram(start) and start <= last and start // RAM == last // RAM):
            fail(f'{at}: {hexa(start)} to {hexa(end)}: not a span of RAM, in one view')
        row = Row(file, start, end, int(count), rest[0] if DISPOSITIONS[disposition] else None, at)
        other = next((other for other in found if other.file == file and other.start < end and start < other.end),
                     None)
        if other is not None:
            fail(f'{at}: its numbers meet those of the row at {other.at}')
        found.append(row)
    return found


def integer(word):
    """The value of word, a C token, as an integer constant, or None."""
    # most tokens don't start with a digit, and testing that first spares
    # layout.number() a raise for each
    if not word[:1].isdigit():
        return None
    try:
        return layout.number(word)[0]
    except layout.LayoutError:
        return None


def operands(line):
    """line, of SHC's assembly, less its comment and with each string empty."""
    return ASSEMBLY_STRING.sub('""', line).split(';')[0]


def assembled(path, lines, first=1):
    """The numbers for RAM of lines, SHC's assembly, from the file at path,
    each as (value, place), place its path:line. The first of lines is the
    file's line first."""
    found = []
    for line, text in enumerate(lines, first):
        for match in ASSEMBLY_NUMBER.finditer(operands(text)):
            value = int(match.group(1), 16) if match.group(1) else int(match.group(2))
            if ram(value):
                found.append((value, f'{path}:{line}'))
    return found


def assembly_lines(text):
    """The indexes of the lines of text, C, that hold SHC's assembly: those of
    each #pragma asm block and of each inline body, between its braces."""
    found = set()
    for pattern in (BLOCK, BODY):
        for match in pattern.finditer(text):
            block = match.lastindex
            first = text.count('\n', 0, match.start(block))
            found.update(range(first, first + text.count('\n', match.start(block), match.end(block))))
    return found


def divided(path):
    """The C file at path, as (assembly, c): assembly each line that holds
    SHC's assembly, as (line, text), line counting from 1, and c its text
    with each of those lines empty."""
    text = code(path)
    lines = text.split('\n')
    held = assembly_lines(text)
    c = '\n'.join('' if i in held else line for i, line in enumerate(lines))
    return [(i + 1, lines[i]) for i in sorted(held)], c


def cast(words):
    """The type that words, a macro's body holding one integer constant, cast
    it to, as the cast's names and stars: () where the body is the integer
    alone, and None where it is no cast of the integer to names and stars.
    Parentheses may enclose the integer, the whole, or both."""
    words = bare(words)
    if len(words) == 1:
        return ()
    close = pairs(words)[0] if words[0] == '(' else None
    operand = bare(words[close + 1:]) if close is not None else []
    if len(operand) != 1 or integer(operand[0]) is None:
        return None
    kind = words[1:close]
    return tuple(kind) if all(word == '*' or NAME.fullmatch(word) for word in kind) else None


def bare(words):
    """words less each pair of parentheses that encloses them all."""
    while words[:1] == ['('] and pairs(words)[0] == len(words) - 1:
        words = words[1:-1]
    return words


def whole(kind):
    """Whether a value of kind, a type as cast() gives one, holds a number
    for RAM whole: an integer constant's own type, an integer of 32 bits and
    a pointer do. A qualifier changes nothing."""
    words = [word for word in kind if word not in layout.QUALIFIERS]
    # long long holds 64 bits, and a set of the words would read it as long
    return words[-1:] == ['*'] or set(words) <= INTEGER | SIGNS and words.count('long') < 2


def bytewise(kind):
    """Whether + 1 moves a value of kind, a type as cast() gives one, by one
    byte: an integer constant's own type, an integer of 32 bits and a
    pointer to a byte do. A qualifier changes nothing."""
    words = [word for word in kind if word not in layout.QUALIFIERS]
    if words[-1:] == ['*']:
        return bool(set(words[:-1]) & BYTE) and set(words[:-1]) <= BYTE | SIGNS
    return whole(words)


def defined(text):
    """Each macro a #define or an #undef of the C text names, with the set of
    what each of them gives it: (value, kind) for a #define without
    parameters whose body holds one integer constant, a number for RAM, kind
    being its cast(); None for any other."""
    found = {}
    for _, name, macro in macro_directives(text):
        words = macro[1] if macro is not None and macro[0] is None else []
        values = [value for value in map(integer, words) if value is not None]
        held = (values[0], cast(words)) if len(values) == 1 and ram(values[0]) else None
        found.setdefault(name, set()).add(held)
    return found


def summed(path, words, definitions):
    """The sums of words, the tokens of the C of the file at path, each as
    (value, place), place its path:line. definitions holds what defined()
    gives of each file read so far, by path, and takes each file this one
    reaches. A sum whose value can't be taken fails."""
    found, scope, opening = [], None, None
    for at, ((name, line), (sign, _), (number, _)) in enumerate(zip(words, words[1:], words[2:])):
        offset = integer(number) if sign in ('+', '-') and NAME.fullmatch(name) else None
        if offset is None:
            continue
        if scope is None:
            texts = {}
            included(Path(path), texts)
            scope = {}
            for found_path in texts:
                if found_path not in definitions:
                    definitions[found_path] = defined(code(found_path))
                for each, held in definitions[found_path].items():
                    scope.setdefault(each, set()).update(held)
        held = scope.get(name, set())
        # a name no file in reach defines as a number for RAM is no MACRO
        if not held - {None}:
            continue
        use = f'{path}:{line}: {name} {sign} {number}'
        if len(held) != 1:
            fail(f'{use}: {path} and the files it includes define {name} as more than one number for RAM, or as '
                 'one and as something else, so the check takes no value for it')
        ((base, kind),) = held
        before = words[at - 1][0] if at else ''
        after = words[at + 3][0] if at + 3 < len(words) else ''
        if before == ')':
            # a cast to names and stars opens at the ( this ) closes, and the
            # token before it is the one before the sum
            if opening is None:
                opening = {close: opened for opened, close in enumerate(pairs([word for word, _ in words]))
                           if close is not None}
            opened = opening.get(at - 1, -1)
            is_cast = opened >= 0 and words[opened][0] == '(' and all(
                word == '*' or NAME.fullmatch(word) for word, _ in words[opened + 1:at - 1])
            # a cast gives the number only where the body's own type held it whole
            kept = is_cast and kind is not None and whole(kind)
            kind = tuple(word for word, _ in words[opened + 1:at - 1]) if kept else None
            before = words[opened - 1][0] if is_cast and opened else ''
        if before in JOINS_BEFORE or after in JOINS_AFTER:
            fail(f'{use}: the {before if before in JOINS_BEFORE else after} beside it makes it no whole operand, so '
                 'the check takes no value for it')
        if kind is None or not bytewise(kind):
            fail(f'{use}: {name} there is neither an integer nor a pointer to a byte, as the check reads it, so it '
                 'takes no value for it')
        # integer() takes no constant wider than 32 bits, and whole() no wider
        # type, so the sum wraps as the SH-4's does
        found.append(((base + offset if sign == '+' else base - offset) & 0xFFFFFFFF, f'{path}:{line}'))
    return found


def compiled(path, definitions):
    """The numbers for RAM of the C file at path, each as (value, place),
    place its path:line: its integer constants and its sums, on both sides
    of each guard, and the numbers of its assembly. definitions is as
    summed() takes it."""
    assembly, c = divided(path)
    found = [hit for line, text in assembly for hit in assembled(path, [text], line)]
    words = tokens(c)
    found += [(value, f'{path}:{line}') for value, line in ((integer(word), line) for word, line in words)
              if value is not None and ram(value)]
    found += [(value, place) for value, place in summed(path, words, definitions) if ram(value)]
    return found


def sources():
    """The C files and the assembly under SOURCE_ROOTS, as paths, in order."""
    return sorted(str(path) for root in SOURCE_ROOTS for path in Path(root).rglob('*')
                  if path.suffix in (*C_FILES, ASSEMBLY) and path.is_file())


def source_numbers(paths):
    """The numbers for RAM of each file of paths, C or assembly, by path,
    each as (value, place)."""
    found, definitions = {}, {}
    for path in paths:
        if path.endswith(ASSEMBLY):
            found[path] = assembled(path, content(path).decode(errors='replace').splitlines())
        else:
            found[path] = compiled(path, definitions)
    return found


def spelled(paths):
    """Each name that a file of paths, C or assembly, spells outside its
    comments and strings. A C file's assembly is read as assembly."""
    found = set()
    for path in paths:
        if path.endswith(ASSEMBLY):
            lines = content(path).decode(errors='replace').splitlines()
        else:
            assembly, c = divided(path)
            lines = [text for _, text in assembly]
            found.update(word for word, _ in tokens(c) if NAME.fullmatch(word))
        for line in lines:
            found.update(ASSEMBLY_NAME.findall(operands(line)))
    return found


def symbol_numbers():
    """The numbers for RAM of each image's symbols.txt, by path: each
    address of RAM it gives outside its image and the images loaded beside
    it, as (value, place)."""
    spans = {}
    for name in images():
        image = Image(name)
        spans[name] = (image.base & PHYSICAL, image.sections[-1][2] & PHYSICAL)
    found = {}
    for name in images():
        beside = [name, *([MAIN] if name != MAIN else []), *(['chao'] if name in CHAO_AREAS else [])]
        path = CONFIG / name / 'symbols.txt'
        if not path.exists():
            continue
        found[str(path)] = [(value, where) for (where, _), (value, _) in zip(rows(path), named(path))
                            if ram(value) and not any(spans[each][0] <= value & PHYSICAL < spans[each][1]
                                                      for each in beside)]
    return found


def body_sources(listing):
    """The source of each body that LIST, the file at listing, names, by the
    body's path. A body before any source fails."""
    found, source = {}, None
    for path in content(listing).decode().split():
        if path.endswith('.c'):
            source = path
        elif source is None:
            fail(f'{listing}: {path} comes before any source')
        else:
            found[path] = source
    return found


def body_numbers(listing):
    """The numbers for RAM of each body the file at listing names, by path,
    each as (value, place). A body that body_data.chunk_lists() reads as a
    chunk list of a kind kinds.NUMBERS names gives none."""
    found, kinds = {}, {}
    for path, source in body_sources(listing).items():
        text = content(path).decode(errors='replace')
        numbers = []
        for line, each in enumerate(BODY_STRING.sub('""', text).split('\n'), 1):
            for match in BODY_NUMBER.finditer(each):
                value = integer(match.group(2) + match.group(3))
                if value is not None and match.group(1):
                    value = -value & 0xFFFFFFFF
                if value is not None and ram(value):
                    numbers.append((value, f'{path}:{line}'))
        # chunk_lists() parses a source's declarations, a minute's work over
        # every body source, so the check reads only a source whose body
        # holds a number for RAM. The body of a datum NAME is NAME.inc
        if numbers:
            if source not in kinds:
                kinds[source] = {name: kind for name, (kind, _) in chunk_lists(Path(source)).items()}
            if kinds[source].get(Path(path).stem) in NUMBERS:
                numbers = []
        found[path] = numbers
    return found


def content_numbers(paths):
    """The numbers for RAM of the disc data each of paths, a content file
    build/dc/content/IMAGE.s, keeps, by its image's data.txt: each 4-aligned
    word of an .incbin, unless the datum that holds it is of a kind of
    kinds.NUMBERS, as (value, place). place gives the datum's row, its name
    and the word's offset in it, and the word's address."""
    found, originals = {}, {}
    for path in paths:
        name = Path(path).stem
        image = Image(name)
        held = sorted(objects(image).values(), key=lambda obj: obj.address)
        starts = [obj.address for obj in held]
        numbers = []
        for number, line in enumerate(content(path).decode().splitlines(), 1):
            match = INCBIN.fullmatch(line.strip())
            if not match:
                if line.split(None, 1)[:1] == ['.incbin']:
                    fail(f'{path}:{number}: an .incbin out of the form content.py writes, which the check can\'t read')
                continue
            original = re.sub(r'\\(.)', r'\1', match.group(1))
            if original not in originals:
                originals[original] = content(original)
            data = originals[original]
            start, size = int(match.group(2)), int(match.group(3))
            first = -(-start // 4) * 4
            # the top byte of each whole word from first on, which alone tells a number for RAM
            tops = data[first + 3:start + size:4]
            for top in VIEW_BYTE.finditer(tops):
                at = first + 4 * top.start()
                (word,) = struct.unpack_from('<I', data, at)
                index = bisect_right(starts, image.base + at) - 1
                obj = held[index] if index >= 0 else None
                if obj is None or image.base + at >= obj.address + obj.size:
                    fail(f'{path}:{number}: the word at {hexa(image.base + at)} lies in no datum of {name}')
                if obj.kind not in NUMBERS:
                    numbers.append((word, f'{obj.where}: {obj.name}+{image.base + at - obj.address:#x}, at '
                                          f'{hexa(image.base + at)}'))
        found[str(CONFIG / name / 'data.txt')] = numbers
    return found


def problems(table_rows, found, counted):
    """A line for each number of found, by file, that no row of table_rows in
    its file explains, and for each row of counted whose COUNT isn't the
    number of them it explains."""
    lines, counts = [], Counter()
    for file, numbers in sorted(found.items()):
        own = [row for row in table_rows if row.file == file]
        for value, place in numbers:
            row = next((row for row in own if row.start <= value < row.end), None)
            if row is None:
                lines.append(f'{place}: {hexa(value)}, which no row of {TABLE} explains')
            else:
                counts[row] += 1
    for row in counted:
        if not counts[row]:
            lines.append(f'{row.at}: {row.file} holds no number the row spans')
        elif counts[row] != row.count:
            lines.append(f'{row.at}: the row counts {row.count} of the numbers it spans, and {row.file} holds '
                         f'{counts[row]}')
    return lines


def main(argv=None):
    parser = argparse.ArgumentParser(prog='addresses', description=__doc__.split('\n\n')[0])
    parser.add_argument('--bodies', metavar='LIST', required=True,
                        help='a file that names each body source and then the bodies the build writes for it, such '
                             'as build/dc/addresses/stub.txt')
    parser.add_argument('content', metavar='CONTENT', nargs='*',
                        help="an image's disc data as tools/content.py writes it, such as build/dc/content/stg13.s")
    args = parser.parse_args(argv)
    table_rows = table()
    paths = sources()
    found = source_numbers(paths)
    found.update(symbol_numbers())
    bodies = body_numbers(args.bodies)
    found.update(bodies)
    found.update(content_numbers(args.content))
    # the other run's directory: the one of BODY_ROOTS that no body of LIST
    # lies under, where its bodies lie under the other. A LIST that names no
    # body names no run, and excuses no row of either
    runs = {root for root in BODY_ROOTS for path in bodies if path.startswith(root)}
    peers = tuple(root for root in BODY_ROOTS if root not in runs) if len(runs) == 1 else ()

    def another_runs(file):
        return file.startswith(peers) or file.endswith('/data.txt') and not args.content

    lines = problems(table_rows, found, [row for row in table_rows if row.file in found])
    lines += [f'{row.at}: {row.file}: the check reads no such file' for row in table_rows
              if row.file not in found and not another_runs(row.file)]
    names = spelled(paths)
    lines += [f'{row.at}: {row.name}: a name no source spells' for row in table_rows
              if row.name is not None and row.name not in names]
    if lines:
        fail(*lines)


if __name__ == '__main__':
    main()
