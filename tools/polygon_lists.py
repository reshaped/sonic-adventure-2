"""Write the declarations that start a body source's polygon lists on 4 bytes under GCC.

    python3 tools/polygon_lists.py SOURCE OUT

njCnkSetMaterial reads a material chunk's colors from its polygon list as
Uint32, and the SH-4 faults on a 32-bit load from an address that isn't a
multiple of 4. SHC's object starts each datum that holds a polygon list on
a multiple of 4. GCC starts a datum at its type's alignment, which is 2 for
an array of Sint16 and for a struct of them.

A polygon list is one that the plist of a chunk model among SOURCE's bodies
points into, by its row of the source's bindings. The datum that holds it
is a flat array, a struct of the list's parts, or a bank of poses. OUT gets
a declaration for each such datum, which gives it 4 bytes' alignment. Each
names its datum's type by __typeof__, so it follows the datum's definition:
make dc's GCC compiles OUT after SOURCE: with SOURCE included ahead of it,
or, for a play source, at the end of the text the float pass has rewritten
(dc.mk). GCC
takes a datum's alignment from its declarations alone, and the greatest of
them. A static keeps its linkage under the later extern.

polygon_lists() gives the lists, which tools/chunk_colors.py reads in each
image's ELF. The run fails on a list in a datum that a function's block
declares, which no declaration at file scope reaches.
"""
import argparse
from pathlib import Path

import body_bindings
from body_data import bodies, declared_kinds, typed_bodies
from config import fail, save
from decls import declarations
from kinds import DECLARED, KINDS
from layout import LayoutError

# the alignment each datum takes, in bytes: the SH-4's 32-bit load takes an address that is a multiple of it
WORD = 4


def polygon_lists(source):
    """Each polygon list that a chunk model among source's bodies points
    into, as (the body that holds it, the body's size, the list's offset in
    the body), in order."""
    texts, types, declarators = declarations(source)
    definitions = bodies(source, texts, declarators)
    layouts = typed_bodies(source, types, definitions)
    _, pointers, _, _ = body_bindings.read_all(source)
    declared = {}
    for ctype, kind in DECLARED.items():
        try:
            declared[types.resolve(types.term(ctype))] = kind
        except LayoutError:
            continue  # a type that no header of the source declares
    scoped = {each.name for each in definitions if each.function is not None}
    found = set()
    for each in definitions:
        for at, kind in declared_kinds(types, each.term, declared):
            for field in KINDS[kind].pointers:
                target, addend = pointers.get((each.name, at + field.offset), (None, None))
                if field.target != 'chunk_polygons' or target not in layouts:
                    continue
                if target in scoped:
                    fail(f"{source}: {target} holds a polygon list, and a function's block declares it")
                found.add((target, layouts[target][0].size, addend))
    return sorted(found)


def declarations_text(source):
    """The text of source's declarations: a line for each datum that holds a polygon list."""
    names = sorted({name for name, _, _ in polygon_lists(source)})
    return ''.join(f'extern __typeof__({name}) {name} __attribute__((aligned({WORD})));\n' for name in names)


def main(argv=None):
    parser = argparse.ArgumentParser(prog='polygon_lists', description=__doc__.split('\n\n')[0])
    parser.add_argument('source', metavar='SOURCE', help='a body source, such as modules/shared/gold_beetle.c')
    parser.add_argument('out', metavar='OUT', help='where to write the declarations')
    args = parser.parse_args(argv)
    save(args.out, declarations_text(Path(args.source)).encode())


if __name__ == '__main__':
    main()
