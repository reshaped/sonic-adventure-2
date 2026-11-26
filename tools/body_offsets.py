"""Measure and check a body source's placements from its original compiler's stub listing."""
import argparse
import os
import re
from collections import Counter, defaultdict
from pathlib import Path
from typing import NamedTuple

import body_bindings
import body_data
import shc_listing
from config import SOURCE, code, fail, hexa, images, rows, save, zeroed
from csource import SPLICE, includes
from decls import declarations

# An inexpensive superset for the reverse index. Only includes() below
# decides whether a candidate really includes the provider. The lookahead
# retains overlapping candidates, including a '<' in a quoted basename.
# A basename that holds a '>' or a '"' is missed: the tree names none.
POSSIBLE_PARENT = re.compile(r'(?=[</"]([^/">\n]+\.[ch])[">])')


def sources():
    """Each source an image places whose C, or a file's it includes, holds
    an include the build generates, as body_data.generated() finds one: a
    body or a text.inc. A file that reaches one is found only under the
    top directory of a source an image places."""
    # Only the placing source names matter to discovery. Compiling goals
    # validate section addresses separately, and lint can plan while they are edited.
    units = {Path(fields[0]) for name in images() for _, fields in rows(Path('config') / name / 'splits.txt')
             if len(fields) == 4 and fields[0].endswith('.c') and SOURCE.fullmatch(fields[0])}
    roots = {unit.parts[0] for unit in units}
    files = (path for root in roots for suffix in ('*.c', '*.h') for path in Path(root).rglob(suffix))
    # Generated headers use the body provider's own directory. A wrapper
    # can reach that provider through another source or header.
    parents = defaultdict(list)
    direct = set()
    for path in files:
        source = path.read_text(errors='replace')
        spliced = SPLICE.sub('', source)
        # an include the build generates names a .inc under the file's own directory, once the splices go
        directory = path.with_suffix('').relative_to(path.parts[0]).as_posix() + '/'
        if directory in spliced and '.inc' in spliced and next(body_data.generated({path: source}), None):
            direct.add(path)
        for name in set(POSSIBLE_PARENT.findall(spliced)):
            parents[name].append(path)
    consumers = set(direct)
    pending = list(direct)
    while pending:
        target = pending.pop()
        for path in parents[target.name]:
            if path not in consumers and any(Path(os.path.normpath(item)) == target
                                             for item in includes(path, path.read_text(errors='replace'))):
                consumers.add(path)
                pending.append(path)
    return sorted(units & consumers)


class Place(NamedTuple):
    kind: str
    section: str
    offset: int
    size: int

    def row(self, name):
        return f'{self.kind}  {name}  {self.section}  {hexa(self.offset)}  {self.size}\n'


def measured(source, listing):
    """The places of source's stub in its listing, by name, as Places: each
    body, each datum that takes numbered texts, each automatic body's
    template, each text, and each private target of its pointer rows, as
    body_data.private_targets() finds them, a function by its entry. The
    run fails on a name the listing places other than once, and on a place
    that disagrees with the name's C."""
    texts, types, declarators = declarations(source)
    definitions = body_data.bodies(source, texts, declarators)
    layouts = {name: layout for name, (layout, _) in body_data.typed_bodies(source, types, definitions).items()}
    scopes = body_data.automatic(definitions)
    lengths, pointers = body_bindings.read(source)
    private_functions, private_data = body_data.private_targets(types, declarators, definitions, pointers,
                                                                body_bindings.path(source))
    named, templates, loads = shc_listing.listing(listing, private_functions)
    names = defaultdict(list)
    for name, section, offset, size in named:
        names[name].append((section, offset, size))
    result = {}
    source_loads = None
    local_sizes = Counter((scope.function, layouts[name].size) for name, scope in scopes.items())
    claimed = {}
    for name in sorted(layouts.keys() | private_functions | private_data.keys()):
        candidates = names[name]
        if name in scopes:
            scope = scopes[name]
            labels = {label for label in loads.get(scope.function, ()) if label in templates
                      and templates[label][2] == layouts[name].size}
            if len(labels) > 1 or local_sizes[scope.function, layouts[name].size] > 1:
                if source_loads is None:
                    source_loads = shc_listing.initializer_loads(listing)
                owned = set()
                for line in range(scope.line, scope.last + 1):
                    owned.update(source_loads.get((scope.function, scope.file.name, line), ()))
                labels &= owned
            candidates = [templates[label] for label in labels]
            if len(candidates) == 1:
                place = candidates[0]
                if place in claimed:
                    fail(f'{listing}: {source}: {name} and {claimed[place]} share an automatic template')
                claimed[place] = name
        if len(candidates) != 1:
            fail(f'{listing}: {source}: {name} has {len(candidates)} native storage candidates')
        section, offset, size = candidates[0]
        if name in private_functions:
            if not code(section) or offset % 2 or size != 2:
                fail(f'{listing}: {source}: {name} has no aligned SH instruction entry')
            result[name] = Place('entry', section, offset, size)
            continue
        if name in private_data and not zeroed(section):
            fail(f'{listing}: {source}: {name}, which the source leaves uninitialized, is in {section}')
        layout = layouts[name] if name in layouts else private_data[name]
        if size != layout.size or offset % layout.alignment:
            fail(f'{listing}: {source}: {name} disagrees with its C type')
        result[name] = Place('datum', section, offset, size)
    if lengths:
        strings = defaultdict(list)
        for section, offset, data in shc_listing.strings(listing):
            strings[data].append((section, offset))
        counts = defaultdict(int)
        for name, size in lengths.items():
            placeholder = bytes(body_data.stub_text(size, counts[size])) + b'\0'
            counts[size] += 1
            candidates = strings[placeholder]
            if len(candidates) != 1:
                fail(f'{listing}: {source}: {name} has {len(candidates)} native string placements')
            section, offset = candidates[0]
            result[name] = Place('text', section, offset, size)
    return result


def refreshed(table, placed):
    """Keep existing comments, ordering and formatting of measurements that still agree."""
    found = []
    remaining = dict(placed)
    for line in table.read_text(encoding='utf-8').splitlines(keepends=True) if table.is_file() else ():
        # a last line without its newline would run into the row after it
        line = line.rstrip('\n') + '\n'
        words = line.partition('#')[0].split()
        if not words:
            found.append(line)
            continue
        if len(words) != 5 or words[0] not in ('datum', 'text', 'entry'):
            fail(f'{table}: expected a measured datum, text or function entry')
        name = words[1]
        place = remaining.pop(name, None)
        if place:
            new = place.row(name)
            if words == new.split():
                found.append(line)
            else:
                comment = line.partition('#')[2]
                if comment:
                    new = new.rstrip('\n') + '  #' + comment.rstrip('\n') + '\n'
                found.append(new)
    for name, place in sorted(remaining.items(), key=lambda item: (item[1].section, item[1].offset, item[0])):
        found.append(place.row(name))
    return ''.join(found).encode()


def main(argv=None):
    parser = argparse.ArgumentParser(prog='body_offsets', description=__doc__)
    parser.add_argument('--sources', action='store_true',
                        help='list placing C translation units that include a generated body or text.inc')
    parser.add_argument('source', type=Path, nargs='?')
    parser.add_argument('listing', type=Path, nargs='?')
    parser.add_argument('output', type=Path, nargs='?')
    args = parser.parse_args(argv)
    if args.sources:
        if args.source is not None:
            parser.error('--sources takes no paths')
        print(*sources())
        return
    if args.output is None:
        parser.error('give SOURCE LISTING OUTPUT')
    # the source names its table under offsets/, so it is a checkout-relative path
    if not SOURCE.fullmatch(args.source.as_posix()):
        parser.error(f'{args.source}: give SOURCE relative to the checkout, as an image places it')
    table = Path('offsets') / args.source.with_suffix('.txt')
    save(args.output, refreshed(table, measured(args.source, args.listing)))


if __name__ == '__main__':
    main()
