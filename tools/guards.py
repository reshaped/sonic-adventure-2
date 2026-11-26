"""Check how C tells builds apart: it tests NONMATCHING and DREAMCAST, the
two switches a build sets, by #ifdef alone, and a non-matching side that
holds code says why it differs.

    python3 tools/guards.py FILE...

FILE is each C source and header to check. SHC's builds define DREAMCAST, a
build of the non-matching code defines NONMATCHING, and clang-tidy defines
neither (PORTS.md#switches). This fails, naming each place, on:

- a test of NONMATCHING or DREAMCAST other than an #ifdef: an #ifndef, an
  #elifdef or #elifndef of one, or an #if or #elif that names one;
- an #elif, #elifdef or #elifndef in an #ifdef of one, whose one other
  group is its #else;
- a conditional that names MATCHING, PORT, GCC or __clang__, each a name
  a build might be told apart by in place of a switch;
- a #define or #undef of NONMATCHING or DREAMCAST, which only a build's
  flags define;
- in a file under a directory EXPLAINED names, and not one UNEXPLAINED
  names, an #ifdef NONMATCHING whose side holds code but no comment that
  says why it differs. Each token but a comment is code, a directive's
  among them. A comment counts for each side that holds it, so one inside a
  nested conditional counts for the side around it too. A side whose code
  is __attribute__((used)) alone, or a #define of one name to it, needs
  none. A comment gives no reason where, past a block comment's stars or a
  Doxygen marker, it holds no word or opens with a tag, a NOLINT or a
  clang-format control.
"""

import argparse
import os
import re
from bisect import bisect_left, bisect_right

from config import content, fail
from csource import parse

SWITCHES = ('NONMATCHING', 'DREAMCAST')
REFUSED = ('MATCHING', 'PORT', 'GCC', '__clang__')
OPENING = ('if', 'ifdef', 'ifndef')
LATER = ('elif', 'elifdef', 'elifndef')
# the directories held to the rule that a non-matching side that holds code says why it differs
EXPLAINED = ('src/', 'engine/', 'sdk/', 'modules/')
# the files under EXPLAINED that the rule skips, each of which still holds a side without a reason
UNEXPLAINED = frozenset((
    'sdk/ninja/bgplane_kamui_lib.c', 'sdk/ninja/dma_kamui_lib.c', 'sdk/ninja/fballoc_kamui_lib.c',
    'sdk/ninja/inifile_kamui_lib.c', 'sdk/ninja/interupt_kamui_lib.c', 'sdk/ninja/kmglobal_kamui_lib.c',
    'sdk/ninja/kmsurfac_kamui_lib.c', 'sdk/ninja/kmtex_kamui_lib.c', 'sdk/ninja/kmvertex_kamui_lib.c',
    'sdk/ninja/kmvtxbuf_kamui_lib.c', 'sdk/ninja/njDirectClipD8.c', 'sdk/ninja/njDirectClipLit.c'))
# what a comment opens with where it is a tag or a tool's control, not a reason
CONTROLS = ('@', 'NOLINT', 'clang-format')
# what a comment may open with before its text: a block comment's stars or a Doxygen marker
DECORATION = '*/! \t\n'
# the tokens of the attribute by which GCC keeps a definition nothing references
USED = ['__attribute__', '(', '(', 'used', ')', ')']


def directives(text):
    """Each preprocessor directive of text, C, as (line, name, names): its
    first line, its name, and the identifiers after the name, in order. A
    comment or a literal gives none."""
    return [(directive.line, directive.name, [word.text for word in directive.words if word.kind == 'name'])
            for directive in parse(text).directives if directive.name.isidentifier()]


def unexplained(text):
    """The line of each #ifdef NONMATCHING in text, C, whose side holds code
    but no comment that gives a reason, as the module docstring says."""
    source = parse(text)
    tokens, found = source.tokens, []
    defines = {directive.start: directive.end for directive in source.directives if directive.name == 'define'}
    lines = [token.line for token in tokens]
    for opening, end in sides(source.directives):
        side = tokens[bisect_right(lines, opening.last):bisect_left(lines, end.line)]
        code = [token for token in side if token.kind != 'comment']
        words = [token.text for token in code]
        # a side that is one #define alone is held by its replacement list
        body = words[3:] if words[:2] == ['#', 'define'] and code[-1].end <= defines[code[0].start] else words
        reasons = [token for token in side if token.kind == 'comment' and reason(token.text)]
        if words and body != USED and not reasons:
            found.append(opening.line)
    return found


def reason(comment):
    """Whether comment, a comment's text with its delimiters, gives a
    reason: past its DECORATION, it holds a word and opens with none of
    CONTROLS."""
    words = (comment[2:-2] if comment.startswith('/*') else comment[2:]).lstrip(DECORATION)
    return bool(re.search(r'\w', words)) and not words.startswith(CONTROLS)


def sides(directives):
    """Each #ifdef NONMATCHING of directives, a file's, with the directive
    that ends its side: its #else, its #endif, or the first of its later
    groups."""
    found, opened = [], []
    for directive in directives:
        if directive.name in OPENING:
            names = [word.text for word in directive.words if word.kind == 'name']
            opened.append(directive if directive.name == 'ifdef' and names[:1] == ['NONMATCHING'] else None)
        elif directive.name in (*LATER, 'else', 'endif') and opened:
            if opened[-1]:
                found.append((opened[-1], directive))
                opened[-1] = None
            if directive.name == 'endif':
                opened.pop()
    return found


def problems(path, text):
    """Each problem the module's docstring lists in text, the C of the file
    at path, as a line that names its place. The reason check reads path
    relative to the working directory, the repository's root, however it is
    spelled."""
    found, file = [], os.path.relpath(path)
    # for each conditional open, the switch an #ifdef that opens it tests, else None
    opened = []
    for line, name, names in directives(text):
        where = f'{path}:{line}'
        if name in OPENING + LATER:
            refused = sorted(set(names) & set(REFUSED))
            switches = sorted(set(names) & set(SWITCHES))
            if refused:
                found.append(f'{where}: #{name} of {", ".join(refused)}: a build is told apart by NONMATCHING or DREAMCAST')
            if switches and name != 'ifdef':
                found.append(f'{where}: #{name} of {", ".join(switches)}: a switch is tested by #ifdef alone')
        if name in OPENING:
            opened.append(names[0] if name == 'ifdef' and names and names[0] in SWITCHES else None)
        elif name in LATER and opened and opened[-1]:
            found.append(f'{where}: #{name} in an #ifdef of {opened[-1]}, whose one other group is #else')
        elif name == 'endif' and opened:
            opened.pop()
        if name in ('define', 'undef') and names and names[0] in SWITCHES:
            found.append(f"{where}: #{name} of {names[0]}: only a build's flags define it")
    if file.startswith(EXPLAINED) and file not in UNEXPLAINED:
        found += [f'{path}:{line}: #ifdef NONMATCHING whose side holds code and no reason: a comment on the side says '
                  'why it differs' for line in unexplained(text)]
    return found


def main(argv=None):
    parser = argparse.ArgumentParser(prog='guards', description=__doc__ and __doc__.split('\n\n')[0])
    parser.add_argument('files', metavar='FILE', nargs='+', help='a C source or header')
    args = parser.parse_args(argv)
    found = [line for path in args.files for line in problems(path, content(path).decode('latin-1'))]
    if found:
        fail(*found)


if __name__ == '__main__':
    main()
