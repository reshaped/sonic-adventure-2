"""Write each chunk list that a struct of its chunks lays out as the flat
array Ninja walks.

    python3 tools/flatten.py [--dry-run] FILE...

A chunk list type is a struct, tagged or named by a typedef, that meets two
tests. Its members are chunk parts: a struct part_* or chunk_vertex_*, a
Uint16, Sint16 or Sint32, an array of one of them, or an untagged struct of
them. And a chunk model's vlist or plist field reaches each object of the
type, and each member of that type in a record: through a body's pointer
row at the list's first byte (TOOLCHAIN.md#offsets-tables), or, for an
extern, through the object's chunk_vertices or chunk_polygons row of
data.txt. The field gives the list's kind. A vertex list becomes Sint32
NAME[N], with N in words, and a polygon list Sint16 NAME[N], with N in
halves.

The tool reads each C source that an image table places and that includes
one of FILE, as SHC compiles it. A type flattens where every file of the
tree that spells it, outside its comments, is one of FILE, and spells it
only in its definition and in declarations of the objects and members the
reads found. Its definition is a struct's or an untagged struct's typedef
that defines nothing else. It flattens everywhere it stands: in its
objects' definitions and externs, and in the members of records, whose
structs stay. Its definition goes, and so does each #include of
draw/chunk.h or draw/chunk_polygon.h that leaves its file naming nothing
the header declares. A declaration keeps its line breaks, which
clang-format then mends.

A type stays where code names a list of it that isn't a record's member,
or names a record that holds one and reads a field through the member that
does: code that reads a list's fields needs them, so the unit that holds
the file gives that code named offsets into the flat array first. Code
that names a record holding a list otherwise takes the record's fields,
which stay. A type also stays where an object that holds it has an
initializer other than the #include of a body the build generates, whose
numbers follow the declared type, and where two of its definitions lay it
out differently.

With --dry-run the tool changes nothing. It prints each type that would
flatten, one to a line, as PATH:LINE: TYPE: ELEMENT[N] OBJECT..., where
PATH:LINE is the type's definition. Then it prints each type that stays, as
PATH:LINE: TYPE: stays: WHY. Last it prints each name the two headers
declare that no file names outside them, other than in the definition of
another such name. The run fails on a type that both kinds of field reach,
on one whose size or alignment no array of its element has, and on a part
header that declares more than macros and structs.
"""

import argparse
from collections import defaultdict
from functools import lru_cache
from pathlib import Path
import re

from body_bindings import read_all
from body_data import bodies, declared_kinds
from config import Image, content, fail, images, rows, save
from csource import header_names, included, key, parse, uncommented
from decls import ANY_SUFFIX, declarations, entity
from kinds import DECLARED, KINDS
from layout import LayoutError

# the element each kind of list is an array of, with its size
ELEMENTS = {'chunk_vertices': ('Sint32', 4), 'chunk_polygons': ('Sint16', 2)}
# the kind of list each of a chunk model's list fields reaches, by the
# field's offset
LIST_FIELDS = {field.offset: field.target for field in KINDS['chunk_model'].pointers}
# the structs that hold a chunk's parts, by their tags' prefixes, and the
# integers a list type spells the rest of its chunks with
PART_TAGS = ('part_', 'chunk_vertex_')
PART_INTEGERS = ('Uint16', 'Sint16', 'Sint32')
# the headers that declare the parts, as the tree's C includes them
PART_HEADERS = ('draw/chunk.h', 'draw/chunk_polygon.h')
# a name a part header declares, with its definition: a macro, through its
# continued lines, or a struct
HELPER = re.compile(r'^#define (\w+)(?:[^\n]*\\\n)*[^\n]*|^struct (\w+) \{.*?^\};', re.M | re.S)
# a part header's include guard, which declares nothing
GUARD = re.compile(r'^#ifndef (\w+)\n#define \1\n', re.M)
# a line of a part header that declares nothing
UNDECLARING = re.compile(r'^#(?:ifndef|endif|include)\b[^\n]*', re.M)
WORD = re.compile(r'\w+')
# the tree's C, which may name a type
TREE = ('src', 'sdk', 'modules')
SUFFIXES = ('.c', '.h', '.inc')


def bare(term):
    """term without its qualifiers."""
    while term[0] == 'qualified':
        term = term[1]
    return term


def is_part(types, term):
    """Whether term, a struct member's, is a chunk part."""
    term = bare(term)
    while term[0] == 'array':
        term = bare(term[1])
    if term[0] == 'typedef':
        return term[1] in PART_INTEGERS
    if term[0] != 'struct':
        return False
    return term[1].startswith(PART_TAGS) or term[1].startswith('#') and holds_parts(types, term)


def holds_parts(types, term):
    """Whether term is a struct whose members are all chunk parts."""
    term = types.resolve(term)
    if term[0] != 'struct':
        return False
    try:
        members = types.members(*term)
    except LayoutError:
        return False
    return bool(members) and all(is_part(types, member) for _, member in members)


def list_type(types, term):
    """The C name of term where it is a chunk list type: struct TAG, or a
    typedef's name. None for any other type, a part included, and for an
    untagged struct that no typedef names."""
    term = bare(term)
    if term[0] == 'typedef' and term[1] not in PART_INTEGERS:
        try:
            return term[1] if holds_parts(types, term) else None
        except LayoutError:
            return None
    if term[0] == 'struct' and not term[1].startswith(('#', *PART_TAGS)):
        return f'struct {term[1]}' if holds_parts(types, term) else None
    return None


def members(types, term):
    """term's members, as (name, term, offset, size): each of a struct's, or
    a union's first, the one an initializer sets."""
    held = [(name, member, offset, layout.size)
            for (name, member), (_, layout, offset) in zip(types.members(*term), types.layout(term).fields)]
    return held[:1] if term[0] == 'union' else held


def element_of(types, term):
    """term, less each array around it: the type of its innermost elements."""
    while types.resolve(term)[0] == 'array':
        term = types.resolve(term)[1]
    return term


def lists_in(types, term, at=0, member=None):
    """Each chunk list that an object of type term holds, as (offset, the
    type's name, the name of the innermost member that holds it, or None):
    the object itself, or each list among its elements and members,
    outermost first. No list holds another."""
    named = list_type(types, term)
    if named:
        yield at, named, member
        return
    term = types.resolve(term)
    if term[0] == 'array':
        size = types.layout(term[1]).size
        for index in range(types.layout(term).count):
            yield from lists_in(types, term[1], at + index * size, member)
    elif term[0] in ('struct', 'union'):
        for name, held, offset, _ in members(types, term):
            yield from lists_in(types, held, at + offset, name)


def list_at(types, term, addend):
    """The offset of the chunk list of an object of type term that starts
    at byte addend, with the list type's name, or None where no list does."""
    at = 0
    while True:
        named = list_type(types, term)
        if named:
            return (at, named) if at == addend else None
        term = types.resolve(term)
        if term[0] == 'array' and types.layout(term[1]).size:
            size = types.layout(term[1]).size
            index = (addend - at) // size
            if not 0 <= index < types.layout(term).count:
                return None
            at, term = at + index * size, term[1]
        elif term[0] in ('struct', 'union'):
            held = [(member, at + offset) for _, member, offset, size in members(types, term)
                    if offset <= addend - at < offset + size]
            if not held:
                return None
            term, at = held[0]
        else:
            return None


class Lists:
    """The chunk list types that the sources' reads find: their objects
    and members, by (entity, offset), each with its name; the kinds that
    reach each; the entities code names, with the files of that code; the
    lists whose object has an initializer the build doesn't generate; and
    by each type, its layouts, as (size, alignment), its objects'
    declarations, as (file, name), and the names of the members that hold
    it. An entity is (name, source, function) for a datum with no linkage,
    and (name, None, None) for one with linkage."""

    def __init__(self):
        self.objects = defaultdict(dict)
        self.whole = set()
        self.reached = {}
        self.named = defaultdict(set)
        self.written = set()
        self.layouts = defaultdict(set)
        self.sites = defaultdict(set)
        self.holders = defaultdict(set)

    def read(self, source, listed):
        """Read the C source at source as SHC compiles it. listed gives the
        data.txt kinds of each public name that a row lists as a chunk
        list."""
        texts, types, declarators = declarations(source)
        keyed = {}
        for each in declarators:
            bound = entity(each.name, each.storage, each.term, each.function)
            if bound and each.storage != 'typedef' and bare(each.term)[0] != 'function':
                linked = each.storage != 'static' and bound[1] is None
                keyed[each] = (bound[0], None, None) if linked else (bound[0], source, bound[1])
        for each, key in keyed.items():
            try:
                found = list(lists_in(types, each.term))
            except LayoutError:
                continue
            whole = list_type(types, element_of(types, each.term))
            for at, named, member in found:
                self.objects[named][key, at] = each.name
                self.sites[named].add((each.file, each.name))
                if member:
                    self.holders[named].add(member)
                if whole:
                    self.whole.add((key, at))
                if each.initialized and each.body is None:
                    self.written.add((key, at))
                layout = types.layout(types.term(named))
                self.layouts[named].add((layout.size, layout.alignment))
                # a row gives the kind of the list at its object's address alone
                if each.storage == 'extern' and each.name in listed and at == 0:
                    self.reached.setdefault((key, at), set()).update(listed[each.name])
        for each in declarators:
            if each.body is None:
                for name, function in each.names:
                    self.named[name, source, function].add(each.file)
                    if function is None:
                        self.named[name, None, None].add(each.file)
        declared = {}
        for ctype, kind in DECLARED.items():
            try:
                declared[types.resolve(types.term(ctype))] = kind
            except LayoutError:
                continue
        _, pointers, _, _ = read_all(Path(source))
        scope = {each.name: each for each in keyed if each.function is None}
        for owner in bodies(source, texts, declarators):
            for at, _ in declared_kinds(types, owner.term, declared):
                for offset, kind in LIST_FIELDS.items():
                    target, addend = pointers.get((owner.name, at + offset), (None, None))
                    if target not in scope:
                        continue
                    try:
                        start = list_at(types, scope[target].term, addend)
                    except LayoutError:
                        continue
                    if start:
                        self.reached.setdefault((keyed[scope[target]], start[0]), set()).add(kind)


def data_lists():
    """The kinds of each public name that a data.txt row lists as a chunk
    list, of every image's rows."""
    found = defaultdict(set)
    for image in images():
        path = Path('config') / image / 'data.txt'
        if path.is_file():
            for _, fields in rows(path):
                if len(fields) > 2 and fields[2] in ELEMENTS and not fields[1].startswith('.'):
                    found[fields[1]].add(fields[2])
    return found


def read(files):
    """The Lists of each C source that an image table places and that
    includes one of files, or is one."""
    wanted, found, listed = set(files), Lists(), data_lists()
    sources = sorted({placement.writer for image in images() for placement in Image(image).placements
                      if placement.writer.endswith('.c')})
    for source in sources:
        texts = {}
        included(Path(source), texts, ANY_SUFFIX)
        if wanted & set(map(Path, texts)):
            found.read(Path(source), listed)
    return found


def words():
    """Each file of the tree's C, by each word its text names outside its
    comments."""
    found = defaultdict(set)
    for top in TREE:
        for path in Path(top).rglob('*'):
            if path.suffix in SUFFIXES:
                for word in set(WORD.findall(uncommented(content(path).decode('latin-1'), lines=False))):
                    found[word].add(path)
    return found


@lru_cache(maxsize=256)
def tokens_of(text):
    """text's parsed tokens, which uses() and reads() share."""
    return parse(text).parsed


def uses(text, named):
    """Where text, a C file's, names the type named: each definition, as
    the indexes of its statement's first and last tokens, and each
    declaration, as the indexes of the type's first token and the token
    after its name, then of each declarator's name and the token after its
    extents, with the parsed tokens. Raises ValueError, naming the line,
    where text names the type any other way."""
    tokens = tokens_of(text)
    opens = {close: at for at, close in enumerate(tokens.closes) if close is not None}
    spelled = named.split()
    definitions, declared = [], []
    for at in range(len(tokens) - len(spelled) + 1):
        if [token.text for token in tokens[at:at + len(spelled)]] != spelled:
            continue
        before = tokens[at - 1].text if at else ';'
        if len(spelled) == 1 and before in ('struct', 'union', 'enum', '.', '->'):
            continue  # a tag or a member of the same spelling
        after, line = at + len(spelled), tokens[at].line
        if len(spelled) == 1 and before == '}':
            keyword = opens[at - 1] - 1
            tag = None
            if keyword > 0 and tokens[keyword].text not in ('struct', 'union'):
                tag, keyword = tokens[keyword].text, keyword - 1
            if keyword > 0 and tokens[keyword].text in ('struct', 'union') and tokens[keyword - 1].text == 'typedef':
                if tag:
                    raise ValueError(f'line {line}: a typedef of the type that defines '
                                     f'{tokens[keyword].text} {tag} too')
                if after >= len(tokens) or tokens[after].text != ';':
                    raise ValueError(f'line {line}: a typedef of the type that names more than the type')
                definitions.append((keyword - 1, after))
                continue
        if after < len(tokens) and tokens[after].text == '{':
            last = tokens.closes[after] + 1
            if before not in (';', '}', 'typedef') or tokens[last].text not in (';', *spelled[-1:]):
                raise ValueError(f'line {line}: a definition of the type that declares more than the type')
            if before != 'typedef':
                definitions.append((at, last))
            continue
        if before == 'typedef':
            raise ValueError(f'line {line}: a typedef of the type')
        held, index = [], after
        while True:
            if index >= len(tokens) or tokens[index].kind != 'name':
                raise ValueError(f'line {line}: the type, where no declaration of it is')
            name, index = index, index + 1
            while tokens[index].text == '[':
                index = tokens.closes[index] + 1
            held.append((name, index))
            while tokens[index].text in ('__attribute__', '__attribute'):
                index = tokens.closes[index + 1] + 1
            if tokens[index].text == '=':
                index += 1
                while tokens[index].text not in (',', ';'):
                    index = tokens.closes[index] + 1 if tokens[index].text in ('{', '(', '[') else index + 1
            if tokens[index].text == ';':
                break
            if tokens[index].text != ',':
                raise ValueError(f'line {line}: a declarator of the type, then {tokens[index].text}')
            index += 1
        declared.append((at, after, held))
    return tokens, definitions, declared


def flattened(text, lists):
    """text, a C file's, with each type of lists, by name as (element,
    count), written as its flat array, and the type's definition gone. A
    definition alone on its lines takes them, and the blank line after
    it where one stands before it too."""
    edits = []
    for named, (element, count) in lists.items():
        tokens, definitions, declared = uses(text, named)
        for first, last in definitions:
            start, end = tokens[first].start, tokens[last].end
            opening = text.rfind('\n', 0, start) + 1
            closing = text.find('\n', end) + 1 or len(text)
            if not text[opening:start].strip() and not text[end:closing].strip():
                start, end = opening, closing
                if text.endswith('\n\n', 0, start) and text.startswith('\n', end):
                    end += 1
            edits.append((start, end, ''))
        for first, after, held in declared:
            edits.append((tokens[first].start, tokens[after - 1].end, element))
            edits.extend((tokens[end - 1].end, tokens[end - 1].end, f'[{count}]') for _, end in held)
    for start, end, new in sorted(edits, reverse=True):
        text = text[:start] + new + text[end:]
    return text


def helpers():
    """Each name that a part header declares, by the header, with its
    definition's text. The header's include guard declares nothing. The run
    fails on a header that declares anything but macros and structs, whose
    names an include's removal would not account for."""
    found = {}
    for header in PART_HEADERS:
        text = uncommented(content(Path('src') / header).decode('latin-1'))
        guard = GUARD.search(text)
        found[header] = {match[1] or match[2]: match[0] for match in HELPER.finditer(text)
                         if not guard or match[1] != guard[1]}
        rest = UNDECLARING.sub('', HELPER.sub('', text)).split()
        if rest:
            fail(f'src/{header}: declares more than macros and structs, from {" ".join(rest[:4])}')
    return found


def without_unused_parts(text, declared):
    """text, a C file's, without each #include of a part header that leaves
    it naming none of the names the header declares, nor the blank line
    after the include where one stands before it too. declared is
    helpers()'."""
    named = set(WORD.findall(uncommented(text, lines=False)))
    lines = text.split('\n')
    for directive, _, name in reversed(list(header_names(text))):
        if name in declared and not named & declared[name].keys():
            at = directive.line - 1
            del lines[at]
            if 0 < at < len(lines) and lines[at - 1] == lines[at] == '':
                del lines[at]
    return '\n'.join(lines)


def unused(declared, index):
    """Each name of declared, helpers()', that no file but its header names,
    nor the definition of a name of either header that one does."""
    headers = {Path('src') / header for header in PART_HEADERS}
    definitions = {name: text for held in declared.values() for name, text in held.items()}
    used = {name for name in definitions if index.get(name, set()) - headers}
    while True:
        more = {name for name in definitions for user in used if re.search(rf'\b{name}\b', definitions[user])}
        if not more - used:
            return sorted(set(definitions) - used)
        used |= more


def reads(path, member):
    """Each place where the C file at path reads a field through a member
    named member, as PATH:LINE: a . or -> before member, and another after
    its subscripts."""
    tokens = tokens_of(content(path).decode('latin-1'))
    found = []
    for at, token in enumerate(tokens):
        if token.text != member or not at or tokens[at - 1].text not in ('.', '->'):
            continue
        after = at + 1
        while after < len(tokens) and tokens[after].text == '[' and tokens.closes[after] is not None:
            after = tokens.closes[after] + 1
        if after < len(tokens) and tokens[after].text in ('.', '->'):
            found.append(f'{path.as_posix()}:{token.line}')
    return found


def decide(found, files, index):
    """Each type of found that flattens, by name, as (element, count, its
    objects' names, its definition's place), and each that stays, by name,
    as (why, its definition's place). index is words()'."""
    wanted, flat, stay = set(files), {}, {}
    for named, objects in sorted(found.objects.items()):
        kinds = set().union(*(found.reached.get(where, ()) for where in objects))
        if not kinds:
            continue  # a record of integers alone, such as a table of code's, that no list field reaches
        if len(kinds) > 1:
            fail(f'{named}: both a vertex list and a polygon list reach it')
        place, problems, naming = None, [], []
        for path in sorted(index.get(named.split()[-1], ())):
            try:
                tokens, definitions, declared = uses(content(path).decode('latin-1'), named)
            except ValueError as error:
                problems.append(f'{path.as_posix()}: {error}')
                naming.append(path)
                continue
            if definitions or declared:
                naming.append(path)
            if definitions and place is None:
                place = path.as_posix(), tokens[definitions[0][0]].line
            for first, _, held in declared:
                for name in (tokens[at].text for at, _ in held):
                    if (path, name) not in found.sites[named] and name not in found.holders[named]:
                        problems.append(f'{path.as_posix()}: line {tokens[first].line}: no read finds {name}')
        place = place or (naming[0].as_posix() if naming else '?', 0)
        unreached = sorted({name for where, name in objects.items() if where not in found.reached})
        named_by_code = sorted({name for (key, at), name in objects.items()
                                if key in found.named and (key, at) in found.whole})
        coded = sorted({path for key, _ in objects if key in found.named for path in found.named[key]})
        read_by_code = [line for member in sorted(found.holders[named]) for path in coded
                        for line in reads(path, member)]
        written = sorted({name for where, name in objects.items() if where in found.written})
        outside = [path.as_posix() for path in naming if path not in wanted]
        if unreached:
            stay[named] = f'no vlist or plist reaches {", ".join(unreached)}', place
        elif named_by_code:
            stay[named] = f'code names {", ".join(named_by_code)}', place
        elif read_by_code:
            stay[named] = f'code reads a field through a member that holds it, at {", ".join(read_by_code)}', place
        elif written:
            stay[named] = f'the build generates no initializer of {", ".join(written)}', place
        elif problems:
            stay[named] = '; '.join(problems), place
        elif outside:
            stay[named] = f'named outside the files, in {", ".join(outside)}', place
        elif len(found.layouts[named]) > 1:
            laid = ' and '.join(f'{size} bytes aligned to {alignment}'
                                for size, alignment in sorted(found.layouts[named]))
            stay[named] = f'its definitions lay it out as {laid}', place
        else:
            kind, = kinds
            element, width = ELEMENTS[kind]
            (size, alignment), = found.layouts[named]
            if alignment != width or size % width:
                fail(f'{place[0]}:{place[1]}: {named}: {size} bytes aligned to {alignment}, as no {element} array is')
            flat[named] = element, size // width, sorted(set(objects.values())), place
    return flat, stay


def main(argv=None):
    parser = argparse.ArgumentParser(prog='flatten', description=__doc__.split('\n\n')[0])
    parser.add_argument('--dry-run', action='store_true', help='print what would flatten, and change nothing')
    parser.add_argument('files', nargs='+', type=Path, metavar='FILE')
    args = parser.parse_args(argv)
    missing = [path.as_posix() for path in args.files if not path.is_file()]
    if missing:
        fail(*(f'{path}: No such file or directory' for path in missing))
    files = [key(path) for path in args.files]
    index = words()
    flat, stay = decide(read(files), files, index)
    declared = helpers()
    if args.dry_run:
        for named, (element, count, objects, place) in sorted(flat.items(), key=lambda item: item[1][3]):
            print(f'{place[0]}:{place[1]}: {named}: {element}[{count}] {" ".join(objects)}')
        for named, (why, place) in sorted(stay.items(), key=lambda item: item[1][1]):
            print(f'{place[0]}:{place[1]}: {named}: stays: {why}')
        print(f'{len(flat)} types flatten and {len(stay)} stay')
        print('no file names ' + ' '.join(unused(declared, index)))
        return
    for path in files:
        text = content(path).decode('latin-1')
        lists = {named: (element, count) for named, (element, count, _, _) in flat.items()
                 if path in index.get(named.split()[-1], ())}
        if lists:
            save(path, without_unused_parts(flattened(text, lists), declared).encode('latin-1'))


if __name__ == '__main__':
    main()
