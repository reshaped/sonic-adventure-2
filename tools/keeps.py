"""List the keeps of the tree's C by class, or take the keeps of some
classes out of the tree.

    python3 tools/keeps.py [--drop CLASS...]

A keep is an __attribute__((used)), or ((used, section(NAME))), on the
non-matching side of a guard, which make dc's GCC reads in a static datum's
declaration, in one of three forms:

- guard: the attribute alone in its guard, ahead of a declaration that SHC
  reads too;
- macro: a name that a non-matching side defines as the attribute and
  nothing more, ahead of the declaration or after its declarator;
- twin: the attribute ahead of a declaration that lies whole on one
  non-matching side, which SHC never reads: GCC's copy of data that SHC
  keeps another way, as modules/stg09/cage_door.c's captions, which SHC
  keeps through a block no path reaches.

GCC drops a static datum that nothing in its object reaches, unless the
datum is used. So a keep's class says what the attribute does in GCC's
object:

- placed: the attribute names a section too, and so places the datum.
- twin: the keep's form is twin, so taking the keep out takes the datum's
  declaration with it.
- reached: in each source that compiles the datum, the tool's walk of
  names reaches it: from an object or a function with external linkage,
  through the names that each function's body and each initializer take,
  and through the pointer rows of each body
  (TOOLCHAIN.md#data-initializers). The walk reads source, not
  GCC's object: a name in a branch GCC folds away reaches the datum too.
  Without the attribute GCC may still drop a reached datum, as where it
  finds it read-only and folds its value into the code that reads it.
- unreached: in each source that compiles the datum, the walk reaches
  none of the keep's data. Without the attribute GCC would drop them; only
  a rebuild shows that it does.
- mixed: the walk reaches some of the keep's data, or one datum in one
  source and not in another: as a keep over static int a, b; where code
  reaches a alone.

The tool reads each C source that an image table places as make dc's GCC
compiles it, through decls.declarations(), so a name that only a guard's
matching side takes reaches nothing.

It prints each keep, one to a line, as PATH:LINE: CLASS FORM DATUM..., in
path and line order, then the count of each class. With --drop it takes
each keep of each CLASS out of the tree instead: its lines, and the
directives of each guard that held one of them and that they leave
holding no C. A comment stays, in the guard or over the keep. It fails,
naming each, on a keep whose declaration no source reads or declares no
datum, on a keep macro that one file defines with a section and another
without, and with --drop, on a keep that shares a line with other C or
with part of a comment.
"""

import argparse
from collections import Counter, namedtuple
from pathlib import Path

from body_bindings import read_all
from config import Image, content, fail, images, save
from csource import LATER, OPENING, Tokens, blank, blanked, key, nonmatching_side, parse
from decls import declarations, entity

CLASSES = ('placed', 'twin', 'reached', 'unreached', 'mixed')
# the classes --drop takes. A placed keep holds its datum in the section it
# names: an image's head, which the link script takes by its section, or a
# datum the port holds in .data. A mixed keep's data are reached in part
DROPPED = ('twin', 'reached', 'unreached')
ATTRIBUTE = '__attribute__'
# the suffixes of the tree's C: a source's, and each that an #include
# takes
SUFFIXES = ('*.c', '*.h', '*.inc')


class Keep(namedtuple('Keep', 'file line form placed lines')):
    """A keep, as keeps_in() finds it:

    - file: the key() of the file that holds it;
    - line: the line of its attribute or its macro, counting from 1, on
      which GCC's read of its declaration opens;
    - form: guard, macro or twin;
    - placed: whether its attribute names a section too;
    - lines: the indexes of the lines it takes, counting from 0, a twin's
      declaration's among them, or None where it shares one with other
      C or with part of a comment."""


def used_attribute(tokens, at):
    """The attribute that opens at tokens[at], a Tokens', where it marks
    its declaration used and does nothing else but name a section: whether
    it names one, and the index past it. None where no such attribute opens
    there."""
    if [token.text for token in tokens[at:at + 3]] != [ATTRIBUTE, '(', '(']:
        return None
    close = tokens.closing(at + 1)
    # the attributes the list names, outside their arguments, as aligned(used) names aligned alone
    names, depth = set(), 0
    for token in tokens[at + 3:close - 1]:
        if token.kind == 'name' and not depth:
            names.add(token.text)
        depth += (token.text == '(') - (token.text == ')')
    return ('section' in names, close + 1) if 'used' in names and names <= {'used', 'section'} else None


def keep_macros(paths):
    """Each macro that a non-matching side of a file of paths defines as a
    used attribute and nothing more, as {name: whether the attribute names
    a section too}. It fails on a name defined both with a section and
    without, whose uses the tool couldn't place."""
    found, conflicts = {}, []
    for path in paths:
        text = content(path).decode('latin-1')
        if ATTRIBUTE not in text:
            continue
        nonmatching = nonmatching_side(blanked(text).split('\n'))
        for directive in parse(text).directives:
            attribute = directive.name == 'define' and directive.line - 1 in nonmatching and \
                used_attribute(Tokens(directive.words), 1)
            if attribute and attribute[1] == len(directive.words):
                name = directive.words[0].text
                if found.setdefault(name, attribute[0]) != attribute[0]:
                    conflicts.append(f'{key(Path(path))}:{directive.line}: {name}, a keep macro another file defines '
                                     f"{'without' if attribute[0] else 'with'} a section")
    if conflicts:
        fail(*conflicts)
    return found


def keeps_in(path, macros):
    """Each Keep of the file at path, in order. macros is keep_macros()'."""
    text = content(path).decode('latin-1')
    tokens, nonmatching = parse(text).parsed, nonmatching_side(blanked(text).split('\n'))
    found = []
    for at, token in enumerate(tokens):
        attribute = token.text == ATTRIBUTE and token.line - 1 in nonmatching and used_attribute(tokens, at)
        if token.text in macros and at + 1 < len(tokens):
            form, placed, past = 'macro', macros[token.text], at + 1
        elif attribute and attribute[1] < len(tokens):
            placed, past = attribute
            # the ; that ends the declaration, outside its braces and parentheses, or the last token
            end = past
            while end < len(tokens) - 1 and tokens[end].text != ';':
                end = min(tokens.closing(end) + 1, len(tokens) - 1)
            # a twin takes its declaration's lines; the other forms, the attribute's or the macro's
            form = 'twin' if set(range(token.line - 1, tokens[end].line)) <= nonmatching else 'guard'
            if form == 'twin':
                past = end + 1
        else:
            continue
        alone = (at == 0 or tokens[at - 1].line < token.line) and \
            (past == len(tokens) or tokens[past].line > tokens[past - 1].line)
        found.append(Keep(key(Path(path)), token.line, form, placed,
                          range(token.line - 1, tokens[past - 1].line) if alone else None))
    if found:
        # each comment's first and last line's indexes. One that runs into a keep's lines from outside them, or out
        # of them, would lose its opener or its closer with them
        comments = [(each.line - 1, each.line - 1 + text.count('\n', each.start, each.end))
                    for each in parse(text).lexemes if each.kind == 'comment']
        found = [each if each.lines is None or not any(
            (first < each.lines[0] <= last) or (first <= each.lines[-1] < last) for first, last in comments)
            else each._replace(lines=None) for each in found]
    return found


def reached(source, declarators):
    """The objects and functions that code reaches in the source at
    source, as decls.entity() gives them. declarators are the source's
    declarations(). Code reaches each object and function with external
    linkage, and each that the code of one it reaches names: in its
    declarators' names, in each automatic object's initializer in its
    body, and as the target of a pointer row of its bodies."""
    names, internal, linked = {}, set(), set()
    # the object or function each declarator binds, or for an automatic object, its function
    holder = {each: entity(each.name, each.storage, each.term, each.function) or (each.function, None)
              for each in declarators}
    for each, held in holder.items():
        names.setdefault(held, set()).update(each.names)
        if each.function is None and each.storage != 'typedef':
            (internal if each.storage == 'static' else linked).add(held)
    owners = {each.name: holder[each] for each in declarators if each.body}
    _, pointers, _, _ = read_all(Path(source))
    for (owner, _), (name, _) in pointers.items():
        # a row's owner may be a body on a guard's matching side alone, which GCC never compiles, as
        # modules/shared/congratulations.c's texlist is
        if owner in owners:
            local = name, owners[owner][1]
            names[owners[owner]].add(local if local in names else (name, None))
    found, pending = set(), list(linked - internal)
    while pending:
        held = pending.pop()
        if held not in found:
            found.add(held)
            pending += names.get(held, ())
    return found


def class_of(keep, reach):
    """The class of keep, a Keep. reach holds, for each source that
    compiles its data, whether code reaches them there."""
    if keep.placed:
        return 'placed'
    if keep.form == 'twin':
        return 'twin'
    if len(reach) > 1:
        return 'mixed'
    return 'reached' if True in reach else 'unreached'


def classify():
    """Each Keep of the tree, with its class and the names of the data it
    keeps, in path and line order."""
    paths = sorted(str(path) for top in ('src', 'sdk', 'modules') for suffix in SUFFIXES
                   for path in Path(top).rglob(suffix))
    macros = keep_macros(paths)
    # each Keep, with each of its data that a source's read declares, as its name and whether code reaches it there
    held = {each: set() for path in paths for each in keeps_in(path, macros)}
    # each Keep on whose line a function's or a typedef's declaration opens
    nondata = set()
    # each Keep by the file and the line on which its declaration opens
    opening = {}
    for each in held:
        opening.setdefault((each.file, each.line), []).append(each)
    sources = sorted({placement.writer for image in images() for placement in Image(image).placements
                      if placement.writer.endswith('.c')})
    for source in sources:
        _, _, declarators = declarations(source, gcc=True)
        data = []
        for each in declarators:
            for keep in opening.get((each.file, each.line), ()):
                if each.storage == 'typedef' or each.term[0] == 'function':
                    nondata.add(keep)
                else:
                    data.append((keep, each))
        if data:
            code = reached(source, declarators)
            for keep, each in data:
                held[keep].add((each.name, entity(each.name, each.storage, each.term, each.function) in code))
    unread = [f'{each.file}:{each.line}: ' + ('a keep whose declaration declares no datum' if each in nondata else
                                              'a keep whose declaration no source reads')
              for each, data in held.items() if not data]
    if unread:
        fail(*unread)
    return [(each, class_of(each, {reaching for _, reaching in held[each]}), sorted({name for name, _ in held[each]}))
            for each in sorted(held)]


def dropped(text, lines):
    """text, a C file's, without the lines whose indexes lines holds, and
    without the directives of each guard with no #else that held one of
    them and that they leave holding no C: a comment in it stays."""
    lines = set(lines)
    empty = [not line.strip() for line in blank(text, ('comment',)).split('\n')]
    # each conditional open, innermost last: its directive where it is an #ifdef NONMATCHING with no #else yet, or
    # None
    opened = []
    for directive in parse(text).directives:
        if directive.name in OPENING:
            tested = [word.text for word in directive.words[:1]]
            opened.append(directive if directive.name == 'ifdef' and tested == ['NONMATCHING'] else None)
        elif directive.name in LATER and opened:
            opened[-1] = None
        elif directive.name == 'endif' and opened:
            guard = opened.pop()
            inside = range(guard.last, directive.line - 1) if guard else ()
            if any(at in lines for at in inside) and all(at in lines or empty[at] for at in inside):
                lines.update(range(guard.line - 1, guard.last), range(directive.line - 1, directive.last))
    return '\n'.join(line for at, line in enumerate(text.split('\n')) if at not in lines)


def main(argv=None):
    parser = argparse.ArgumentParser(prog='keeps', description=__doc__.split('\n\n')[0])
    parser.add_argument('--drop', metavar='CLASS', nargs='+', choices=DROPPED,
                        help=f"take each keep of each CLASS out of the tree: {', '.join(DROPPED)}")
    args = parser.parse_args(argv)
    found = classify()
    if not args.drop:
        print('\n'.join(f"{each.file}:{each.line}: {kind} {each.form} {' '.join(data)}" for each, kind, data in found))
        counts = Counter(kind for _, kind, _ in found)
        print('\n'.join(f'{counts[kind]} {kind}' for kind in CLASSES))
        return
    taken = [each for each, kind, _ in found if kind in args.drop]
    shared = [each for each in taken if each.lines is None]
    if shared:
        fail(*(f'{each.file}:{each.line}: a keep that shares a line with other C or with part of a comment'
               for each in shared))
    lines = {}
    for each in taken:
        lines.setdefault(each.file, set()).update(each.lines)
    for path, held in lines.items():
        save(path, dropped(content(path).decode('latin-1'), held).encode('latin-1'))


if __name__ == '__main__':
    main()
