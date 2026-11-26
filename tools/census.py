"""Write an image's census: the global and weak names its link-time
compile leaves undefined, less those the image takes from the main
executable.

    python3 tools/census.py LINKED OUT [--main MAIN] [--symbols SYMBOLS] [--content IMAGE]

LINKED is the relocatable ELF make dc links of an image's objects, the
archive of SH4ZAM's library and libgcc.a, through GCC's link-time compile
(PORTS.md#link-time-optimization). A name is LINKED's where a global or weak
symbol of LINKED's symbol table bears it, and it is undefined where each
such symbol is undefined.

MAIN and SYMBOLS, given for an image other than the main executable, are
the main executable's LINKED and its symbols.txt. The image takes each name
SYMBOLS gives, as tools/config.py gives every image but the main
executable the main executable's names. It takes each name MAIN defines
too, which holds the names the main executable's non-matching code adds to
its table's. OUT gets each name LINKED leaves undefined that the image
doesn't take, as C spells it, without the object format's leading
underscore, one to a line, in the C locale's order.

For IMAGE, its checked data.txt also supplies symbols the final link takes
from the original disc. Checking those declarations needs no disc.

It fails, naming the file, on an ELF that isn't 32-bit, little-endian and
relocatable, or that holds no symbol table, and on a name OUT would get
that doesn't open with an underscore, which no C name lacks. A malformed
row of SYMBOLS fails as tools/config.py fails it.
"""

import argparse

from config import Image, content, fail, named, save
from content import objects
from sh_elf import BOUND, ET_REL, SHN_UNDEF, ElfTables, WrongElfType

def symbols(path):
    """{name: defined} for each global or weak symbol of the ELF at path, a
    name defined where any symbol of it is."""
    try:
        table = ElfTables(path, content(path), ET_REL)
        found = {}
        for name, _, _, info, index in table.symbols():
            if info >> 4 in BOUND:
                found[name] = found.get(name, False) or index != SHN_UNDEF
        return found
    except WrongElfType:
        fail(f'{path}: not a 32-bit little-endian relocatable ELF for SH')
    except ValueError as error:
        fail(error)


def census(linked, main=None, table=None, image=None):
    """The names linked leaves undefined, less those main defines and those
    table, a symbols.txt, gives, as C spells them, in order. A name it
    would return without a leading underscore fails."""
    taken = {name for name, defined in symbols(main).items() if defined} if main is not None else set()
    taken |= {f'_{name}' for _, name in named(table)} if table is not None else set()
    if image is not None:
        taken.update(obj.symbol for obj in objects(Image(image)).values() if obj.public)
    needed = sorted(name for name, defined in symbols(linked).items() if not defined and name not in taken)
    odd = [name for name in needed if not name.startswith('_')]
    if odd:
        fail(f"{linked}: {', '.join(odd)}: undefined, and no C name, which opens with an underscore")
    return [name[1:] for name in needed]


def main(argv=None):
    parser = argparse.ArgumentParser(prog='census', description=__doc__.split('\n\n')[0])
    parser.add_argument('linked', metavar='LINKED', help="an image's relocatable link, such as build/dc/census/stg13.o")
    parser.add_argument('out', metavar='OUT', help='where to write the census, such as build/dc/census/stg13.txt')
    parser.add_argument('--main', metavar='MAIN', help="the main executable's relocatable link")
    parser.add_argument('--symbols', metavar='SYMBOLS', help="the main executable's symbols.txt")
    parser.add_argument('--content', metavar='IMAGE', help='image whose data.txt supplies disc objects at final link')
    args = parser.parse_args(argv)
    save(args.out, ''.join(f'{name}\n' for name in census(args.linked, args.main, args.symbols, args.content)).encode())


if __name__ == '__main__':
    main()
