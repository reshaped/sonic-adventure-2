"""Rewrite each .bindings row that spells a pointer's target as C into a relocation.

    python3 tools/relocate.py [TABLE...]

A pointer row is a relocation: the name its field points at and how many
bytes past that name's start, as NAME+0x48c, with the + and the hex left
out at 0 (body_bindings.py). A row that spells its target as C becomes one:

- &NAME, and @NAME for a function of the source's own, point at NAME;
- NAME with a fifth field points as many bytes past NAME as the field
  gives;
- a member path such as NAME[1].mesh points as many bytes past NAME as the
  path's member lies in NAME's declaration in scope at the row's datum.

A pointer row that gives 0 goes, since a field that no row names holds
null. So does a reserve row, since body_offsets.py finds the storage it
measures by the declarations of the names rows give. Every other line stays
as it is. A source's own table left with no row goes too, but a shared one
stays, since tables include it.

TABLE is offsets/PATH.bindings, whose member paths are read through
PATH.c's declarations, or layouts/NAME.bindings, read through those of each
source whose table includes it. With no TABLE it rewrites every table of
offsets/ and layouts/. The run fails on a member path that the declarations
don't hold, that two sources lay out apart, or that lies in a shared table
no source includes. Its message names the row, and it changes no table.
"""
import argparse
from collections import defaultdict
from functools import lru_cache
from pathlib import Path
import re

from body_bindings import SUBOBJECT, dependencies
from body_data import Targets, bodies, declared_base, row_target, subobject
from config import NAME, address, at_root, content, fail, save
from decls import declarations
from layout import Array, LayoutError

# a row's fields, each a run of characters but blanks
WORD = re.compile(r'\S+')


class MemberPaths:
    """The offsets of the member paths that a source's rows spell, by its
    declarations."""

    def __init__(self, source):
        self.source = source
        texts, self.types, declarators = declarations(source)
        self.targets = Targets(self.types, declarators, {}, ())
        # each body's scope, for the names its rows give: the function whose blocks declare it, or None
        self.scopes = {each.name: each.function for each in bodies(source, texts, declarators)}

    def offset(self, owner, name, path, where):
        """The offset of path, as [1].mesh, from name's start, in name's
        declaration in scope at owner, a row's datum. An array whose
        declarations give it no extent holds any index. The run fails,
        naming where, on a path the declaration doesn't hold."""
        term = self.targets.declared(name, where, self.scopes.get(owner))
        if term is None:
            fail(f'{where}: no declaration gives {name}')
        try:
            base, _ = declared_base(self.types, term)
            layout = (Array(self.types.layout(base[1]), None) if base[0] == 'array' and base[2] is None
                      else self.types.layout(term))
        except LayoutError as error:
            fail(f'{where}: {name}: {error}')
        return subobject(layout, path, f'{where}: {name}{path}')[1]


def relocation(fields, where, readers):
    """The TARGET of the relocation that fields spell, or None for a row
    that gives 0. fields are a pointer row's four, or its five where the
    fourth is a plain NAME. readers() gives the MemberPaths of each source
    that reads the row. Any other target stays as it is: a relocation, or
    one that body_bindings.py refuses."""
    target = fields[3]
    if len(fields) == 5:
        return row_target(target, address(fields[4], where))
    if target == '0':
        return None
    if target[0] in '&@' and NAME.fullmatch(target[1:]):
        return target[1:]
    spelled = SUBOBJECT.fullmatch(target)
    if not spelled or not spelled[2]:
        return target
    name, path = spelled.groups()
    # each offset the readers give, by the first source that gives it
    offsets = {}
    for reader in readers():
        offsets.setdefault(reader.offset(fields[1], name, path, where), reader.source)
    if not offsets:
        fail(f'{where}: {target}: no source\'s table includes this one, to read the path by')
    if len(offsets) > 1:
        first, *rest = sorted(offsets.items())
        fail(f'{where}: {target} lies {first[0]:#x} bytes past {name} in {first[1]}'
             + ''.join(f', {offset:#x} in {source}' for offset, source in rest))
    [offset] = offsets
    return row_target(name, offset)


def rewritten(table, readers):
    """The text of table, a .bindings file, with each of its rows rewritten,
    and whether it keeps a row, as (text, kept). readers is
    relocation()'s."""
    lines, kept = [], False
    for number, line in enumerate(content(table).decode().splitlines(keepends=True), 1):
        row, mark, comment = line.partition('#')
        fields = row.split()
        if fields[:1] == ['reserve']:
            continue
        # the older reader took a fifth field only after a plain NAME, so any other five-field row stays for
        # body_bindings.py to refuse
        if fields[:1] == ['pointer'] and (len(fields) == 4 or len(fields) == 5 and NAME.fullmatch(fields[3])):
            target = relocation(fields, f'{table}:{number}', readers)
            if target is None:
                continue
            spans = [word.span() for word in WORD.finditer(row)]
            line = row[:spans[3][0]] + target + row[spans[-1][1]:] + mark + comment
        kept = kept or bool(fields)
        lines.append(line)
    return ''.join(lines), kept


def main(argv=None):
    parser = argparse.ArgumentParser(prog='relocate', description=__doc__.splitlines()[0])
    parser.add_argument('tables', nargs='*', type=Path, metavar='TABLE',
                        help='an offsets/ or layouts/ .bindings file; every one where none is given')
    args = parser.parse_args(argv)
    for table in args.tables:
        if (table.suffix != '.bindings' or '..' in table.parts
                or table.parts[0] != 'offsets' and table.parent != Path('layouts')):
            parser.error(f'{table}: not a .bindings file of offsets/ or layouts/')
    at_root('offsets/')
    tables = args.tables or sorted([*Path('offsets').rglob('*.bindings'), *Path('layouts').glob('*.bindings')])
    member_paths = lru_cache(maxsize=None)(MemberPaths)

    @lru_cache(maxsize=None)
    def sharing():
        """The sources whose tables include each shared table, read once a shared table's member path needs them."""
        found = defaultdict(list)
        for own in Path('offsets').rglob('*.bindings'):
            source = own.relative_to('offsets').with_suffix('.c')
            for shared in dependencies(source):
                found[shared].append(source)
        return found

    def readers(table):
        sources = sorted(sharing().get(table, ())) if table.parent == Path('layouts') else [
            table.relative_to('offsets').with_suffix('.c')]
        return [member_paths(source) for source in sources]

    found = {table: rewritten(table, lambda table=table: readers(table)) for table in tables}
    for table, (text, kept) in found.items():
        if not kept and table.parts[0] == 'offsets':
            table.unlink()
        elif text.encode() != content(table):
            save(table, text.encode())


if __name__ == '__main__':
    main()
