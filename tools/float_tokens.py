"""GCC's preprocessed C as tools/floats.py's tokens: the tokens
tools/csource.py's parse() reads outside the directives, each with the file
and line its line marker gives it, whose lines SHC reads, and how a message
quotes a run of them.

GCC's line markers give the file and line each line of C comes from, so a
macro's operands lie where the macro expands. The tree's lines are those of
every file but a system header, which SHC never reads. marked() says which
files are system headers. Of the tree's lines, SHC reads those of a file it
includes but those on a guard's non-matching side, as tools/csource.py's
nonmatching_side() gives them. SHC includes the source, and each file whose
#include lies on a line it reads. Each other line of the tree's is GCC's
own: a non-matching side, and a file that GCC alone includes, as
sdk/include/ninja.h's non-matching side includes ninja_zam.h. A file whose
#include lies on a system header's line is GCC's own too, and so is one
that GCC's <command-line> includes, as -include does.

A file's lines take the side of the #include that GCC enters it by. A
guarded header that GCC first includes on a non-matching side is GCC's own
throughout, even where SHC includes it later, since GCC never enters it
again: no header of the tree that holds floats is included so. The side of
a line is read from the file and line its marker names, so the tool holds
only for C whose markers name each line where it lies, as no #line of the
tree's moves them.
"""

from bisect import bisect_right
from pathlib import Path
import re

from config import content
from csource import LINE, Tokens, blanked, following, nonmatching_side, parse

# a header in GCC's own include directory, as a line marker names it
GCC_INCLUDE = re.compile(r'.*/lib/gcc/sh-elf/[^/]+/include/')
# the flag of a line marker that enters a file, and the one that returns to its includer
ENTERS, RETURNS = 1, 2


class Marker:
    """A line marker of GCC's preprocessed C, as the tokens after it take it:
    file, the file it names; offset, the number to add to a line of the C
    for its line in file; system, whether file is a system header; and shc,
    whether SHC includes file."""

    __slots__ = ('file', 'offset', 'system', 'shc')

    def __init__(self, file, offset, system, shc):
        self.file, self.offset, self.system, self.shc = file, offset, system, shc


class Marked(Tokens):
    """parse()'s Tokens of GCC's preprocessed C but its directives, with the
    line marker above each token: markers holds each Marker, and firsts the
    index of the first token after each. Where markers share a first token,
    the last of them applies. system says whether each token lies on a
    system header's line, and shc whether SHC reads the line."""

    def __init__(self, tokens, firsts, markers, sides):
        super().__init__(tokens)
        self.firsts, self.markers, self.system, self.shc = firsts, markers, [], []
        for marker, first, past in zip(markers, firsts, [*firsts[1:], len(tokens)]):
            self.system += [marker.system] * (past - first)
            side = sides(marker.file) if marker.shc and past > first else ()
            self.shc += [marker.shc and tokens[at].line + marker.offset - 1 not in side for at in range(first, past)]

    def marker(self, at):
        return self.markers[bisect_right(self.firsts, at) - 1]

    def line(self, at):
        return self[at].line + self.marker(at).offset

    def where(self, at):
        return f'{self.marker(at).file}:{self.line(at)}'


def marked(text, path, system=()):
    """The Marked tokens of text, GCC's preprocessed C at path, from a
    source of the tree. A marker's file is a system header where it lies
    under a directory system names, once both resolve against the working
    directory, or in GCC's own include directory. A tree header that a
    system header includes is the tree's, though GCC's marker flags it as a
    system header too. marked() reads a file SHC can read, from the working
    directory, for the lines on its non-matching side, once a token or an
    #include lies on one of its lines; one that can't be read fails, naming
    it."""
    roots = [Path(root).resolve() for root in system]
    c, system_header, sides = parse(text), {}, {}

    def header(file):
        if file not in system_header:
            resolved = Path(file).resolve()
            gcc = GCC_INCLUDE.match(file) or GCC_INCLUDE.match(resolved.as_posix())
            system_header[file] = bool(gcc) or any(
                root == resolved or root in resolved.parents for root in roots)
        return system_header[file]

    def side(file):
        if file not in sides:
            sides[file] = nonmatching_side(blanked(content(file).decode('latin-1')).split('\n'))
        return sides[file]

    tokens = c.parsed
    # the C's first line is line 1 of path. Each marker that enters a file
    # pushes the marker it enters from, and each that returns pops it
    firsts, markers, at, stack = [0], [Marker(path, 0, False, True)], 0, []
    for line, file, named, flags in c.markers:
        at = following(tokens, line + 1, at, LINE)
        above = markers[-1]
        if ENTERS in flags:
            # the #include lies on the line of the file it enters from that
            # the marker's own line would be. GCC's <command-line>, which a
            # file -include names enters from, has no lines SHC reads
            included = line + above.offset
            stack.append(above)
            shc = above.shc and not above.system and not above.file.startswith('<') and \
                included - 1 not in side(above.file)
        elif RETURNS in flags and stack:
            shc = stack.pop().shc
        else:
            shc = above.shc
        is_system = header(file)
        firsts.append(at)
        markers.append(Marker(file, named - line - 1, is_system, shc and not is_system))
    return Marked(tokens, firsts, markers, side)


def quoted(toks):
    """toks, a nonempty run of tokens, as a message quotes it on one line:
    each one's text, and a space where the C parts two, as a line marker
    does."""
    return ''.join(token.text + ' ' * (after.start > token.end) for token, after in zip(toks, toks[1:])) + toks[-1].text
