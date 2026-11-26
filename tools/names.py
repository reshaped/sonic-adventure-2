"""Check that every row of the symbol tables has a reason to be there, and
that each placeholder names its own address.

    python3 tools/names.py

A symbols.txt row earns its place in one of four ways:

- an object its image places defines the name, at the row's address, which
  tools/link.py checks;
- the image's data.txt declares the name, for a typed disc object of its
  own or one it imports. tools/content.py, which make dc-check runs,
  checks that the row gives the object's address;
- an object its image places takes the name from outside. The main
  executable's rows serve every image, so an object of any image can take
  one;
- the row lies in the code of a source's own rows: a function no other
  object uses, which tools/words.py measures by.

Any other row fails, with its table, address and name: nothing reads it, so
nothing would notice it going wrong.

A placeholder names a number, and a row fails where the number isn't the
row's: fun_<address> and dat_<address> at another address, and
fun_<file>_<offset> where the image places a source called <file> and no
such source holds the row in its code, <offset> bytes into it. An address
is eight lowercase hex digits and an offset any number of them; a name
spelled with an uppercase hex letter is no placeholder, and goes unchecked.

It reads the object of each source the tables place, so the objects must be
built: make check builds them. It needs no disc.
"""

import argparse
import re
from pathlib import Path

import sysrof
from config import MAIN, OBJECTS, Image, code, fail, hexa, images, named, native_path, rows, stub_sources

# a placeholder that names its address, and one that names its offset in a
# source's code
ADDRESSED = re.compile(r'(?:fun|dat)_([0-9a-f]{8})')
OFFSET = re.compile(r'fun_(\w+)_([0-9a-f]+)')


def used(source, read, stubs):
    """The names source's object defines and the names it takes from outside,
    as two sets, each name as C spells it. read(path) gives an object's
    exports and its references."""
    exported, referenced = read(native_path(OBJECTS / Path(source).with_suffix('.obj'), source, stubs))
    return set(map(sysrof.c_name, exported)), set(map(sysrof.c_name, referenced))


def declared(image):
    """The names image's data.txt declares: each row's object, each object of
    a layout a row places, and each object a row imports. It reads them as
    text, and leaves each row's kind and fields to tools/content.py."""
    path = image.path / 'data.txt'
    if not path.is_file():
        return set()
    found = set()
    for _, fields in rows(path):
        if fields[0] == 'import':
            found.update(fields[2:3])
        elif len(fields) == 3 and fields[1] == 'layout':
            found.update(member[0] for _, member in rows(Path('layouts') / (fields[2] + '.txt')))
        else:
            found.update(fields[1:2])
    return found


def unused(read=lambda path: (sysrof.exports(path), sysrof.references(path))):
    """The rows no object uses, as (image, address, name) in table order.
    read is as used() takes it."""
    objects, tables, anywhere = {}, {}, set()
    stubs = stub_sources()
    for name in images():
        image = Image(name)
        defined, taken = set(), set()
        for source in image.sources():
            if source not in objects:
                objects[source] = used(source, read, stubs)
            defined |= objects[source][0]
            taken |= objects[source][1]
        anywhere |= taken
        tables[name] = image, defined, taken
    found = []
    for name, (image, defined, taken) in tables.items():
        inside = [(start, end) for source in image.sources() for section, start, end in image.ranges(source)
                  if code(section)]
        data = declared(image)
        for address, symbol in named(image.path / 'symbols.txt'):
            if (symbol not in defined and symbol not in taken and symbol not in data
                    and not (name == MAIN and symbol in anywhere)
                    and not any(start <= address < end for start, end in inside)):
                found.append((name, address, symbol))
    return found


def misnamed():
    """The rows whose placeholder names another number than the row's, as
    (image, address, name, what the name says) in table order."""
    found = []
    for name in images():
        # each source's code ranges, by the source's file name less its
        # extension, which two sources the image places may share; a source
        # that places no code has none
        image, sources = Image(name), {}
        for source in image.sources():
            ranges = [(start, end) for section, start, end in image.ranges(source) if code(section)]
            sources.setdefault(Path(source).stem, []).append((source, ranges))
        for address, symbol in named(image.path / 'symbols.txt'):
            digits, offset = ADDRESSED.fullmatch(symbol), OFFSET.fullmatch(symbol)
            if digits and int(digits[1], 16) != address:
                found.append((name, address, symbol, hexa(int(digits[1], 16))))
            elif offset and offset[1] in sources:
                said = [placed(source, ranges, offset[2]) for source, ranges in sources[offset[1]]]
                if all(at != address for at, _ in said):
                    found.append((name, address, symbol, ' or '.join(words for _, words in said)))
    return found


def placed(source, ranges, digits):
    """The address the hex digits give as an offset into source's code,
    whose ranges are ranges, and what a name with those digits says, as
    (address, words). The offset counts from the first range's start, and
    the address is None where no range holds it."""
    if not ranges:
        return None, f"0x{digits} into {source}'s code, and it places none"
    at = ranges[0][0] + int(digits, 16)
    if not any(start <= at < end for start, end in ranges):
        return None, f"0x{digits} into {source}'s code, {hexa(at)}, outside it"
    return at, f"0x{digits} into {source}'s code, {hexa(at)}"


def main(argv=None):
    parser = argparse.ArgumentParser(prog='names', description=__doc__.split('\n\n')[0])
    parser.parse_args(argv)
    faults = [f'config/{image}/symbols.txt: {hexa(address)} {symbol}: its name says {said}'
              for image, address, symbol, said in misnamed()]
    faults += [f'config/{image}/symbols.txt: {hexa(address)} {symbol}: no object its image places defines or '
               "takes it, its data.txt doesn't declare it, and no source's code holds it"
               for image, address, symbol in unused()]
    if faults:
        fail(*faults)


if __name__ == '__main__':
    main()
