"""Check that no outside name is declared in two sources, or in a header and
again in a source, and that no file of src/ includes a header of modules/.

    python3 tools/decls.py FILE...

FILE is each C source and header to check. A declaration here is one at file
scope, neither static nor a typedef, of a function without its body or of an
extern variable without an initializer. A source's outside names are those
it declares and doesn't define. Each file is read as SHC compiles it and as
make dc's GCC does, apart from the files it includes, and this fails, naming
each place, where either compile finds:

- a name that two sources declare as an outside name;
- a name that a header declares and a source declares again as an outside
  name;
- a source or header under src/ that includes a header of modules/: an
  overlay loads and unloads beside the main executable, which can't rely on
  its declarations.

sdk/network/sg_dpldr.c declares sbExitSystem, which src/system/sbinit.h
declares too. It is the Network SDK's call into the game, and the network
route searches no header of src/, so those two may repeat it, and no other
file may.

declarations() reads what a C source and the files it includes declare,
as SHC compiles them or as make dc's GCC does, and what each declarator's
code names, for the tools that read the tree's data.
"""

import argparse
import os
import re
from bisect import bisect_right
from collections import namedtuple
from functools import lru_cache
from operator import itemgetter
from pathlib import Path

from config import content, fail
from csource import (LATER, OPENING, STATIC_ASSERT, WORD, Token, Tokens, Typed, blank, bodies as inline_bodies,
                     following, header_names, included, key, matching_side, nonmatching_side, parse, sites)
from layout import STORAGE, LayoutError, Types
from split import assembly_blocks

# each name that may repeat, by the files that may declare it
EXEMPT = {'sbExitSystem': {'sdk/network/sg_dpldr.c', 'src/system/sbinit.h'}}
# the one macro SHC's flags define (config/compilers.mk): DREAMCAST, one of
# the two switches a guard tests (PORTS.md#switches). -define=DREAMCAST
# gives it no value the tool knows, so an #if that tests its value fails
DEFINED = {'DREAMCAST': None}
# the macros make dc's GCC flags define (config/gcc.mk): both switches, and
# NDEBUG, each a -D that gives it the value 1
GCC_DEFINED = {'NONMATCHING': '1', 'DREAMCAST': '1', 'NDEBUG': '1'}
# the macros whose definition a compile fixes: those either compile defines
KNOWN = frozenset(GCC_DEFINED)
# the directives of a conditional
CONDITIONAL = (*OPENING, *LATER, 'endif')
# a literal #include of a file of any suffix, as modules/stg25/torch.c
# includes its task's code from stg25/standing_torch_task.inc. A file the
# build generates lies under build/, where no search finds it
ANY_SUFFIX = re.compile(r'#include [<"][^>"]+[>"]')
# where a game source's #include finds a header: the game route's search
# path in config/compilers.mk, after the including file's directory for a
# quoted name
SEARCHED = ('sdk/include', 'src', 'modules')

# a comment, a string or character literal, and a preprocessor line with its
# continuations: what a scan for declarations blanks
NOISE = re.compile(r'/\*.*?\*/|//[^\n]*|"(?:\\.|[^"\\\n])*"|\'(?:\\.|[^\'\\\n])*\'|^[ \t]*#(?:\\\n|[^\n])*',
                   re.S | re.M)
TOKEN = re.compile(r'[A-Za-z_]\w*|\S')
NAME = re.compile(r'[A-Za-z_]\w*')
KEYWORDS = {'auto', 'break', 'case', 'char', 'const', 'continue', 'default', 'do', 'double', 'else', 'enum', 'extern',
            'float', 'for', 'goto', 'if', 'int', 'long', 'register', 'return', 'short', 'signed', 'sizeof', 'static',
            'struct', 'switch', 'typedef', 'union', 'unsigned', 'void', 'volatile', 'while'}
TAGGED = ('struct', 'union', 'enum')
# the tokens a statement in a block can follow
STATEMENT_OPENS = (';', '{', '}', 'else')


def blanked(text):
    """text, C, with its noise blanked to spaces, keeping its lines."""
    blocks = assembly_blocks(text)
    if blocks:
        lines = text.split('\n')
        for first, body in blocks.items():
            # Keep the directives for NOISE. Between them SHC reads assembly,
            # whose comments and labels never form part of a C declaration.
            for at in range(first + 1, first + 1 + len(body)):
                lines[at] = ' ' * len(lines[at])
        text = '\n'.join(lines)
    return NOISE.sub(lambda noise: re.sub(r'[^\n]', ' ', noise[0]), text)


def statements(text):
    """Each statement at file scope of text, C with its noise blanked, as
    (line, tokens, body): its first line, its tokens with each brace's
    contents left out, and whether it ends in a function's body."""
    found, start, depth, held = [], None, 0, []
    # the first line of the last statement found, and where it starts, from which the next count of line ends runs
    line, counted = 1, 0

    def first_line():
        """The line start lies on, counting from 1."""
        nonlocal line, counted
        line, counted = line + text.count('\n', counted, start), start
        return line

    for token in TOKEN.finditer(text):
        word = token[0]
        if depth:
            depth += {'{': 1, '}': -1}.get(word, 0)
            if depth == 0:
                held.append('}')
                if body(held):
                    found.append((first_line(), held, True))
                    start, held = None, []
            continue
        if start is None:
            start = token.start()
        if word == ';':
            found.append((first_line(), held, False))
            start, held = None, []
        elif word == '{':
            depth = 1
            held.append('{')
        else:
            held.append(word)
    return found


def body(tokens):
    """Whether tokens, a statement's through a closing brace, end in a
    function's body: the brace follows a declarator's parentheses, with no
    initializer before it. A struct, union or enum's own brace follows its tag
    or its keyword, so a definition that returns one still ends in a body."""
    return len(tokens) > 2 and tokens[-3] == ')' and '=' not in tokens


def closing(tokens, at):
    """The index of the parenthesis that closes the one at tokens[at]."""
    depth = 0
    for index in range(at, len(tokens)):
        depth += {'(': 1, ')': -1}.get(tokens[index], 0)
        if not depth:
            return index
    return len(tokens)


def declarator(tokens):
    """The name a declarator's tokens declare, after any specifiers, and
    whether it declares a function, as (name, function). name is None where
    they declare none, as for struct task."""
    for at, token in enumerate(tokens):
        if token == '(':
            close = closing(tokens, at)
            if tokens[at + 1:at + 2] == ['*']:
                return declarator(tokens[at + 1:close])
            if at and NAME.fullmatch(tokens[at - 1]) and tokens[at - 1] not in KEYWORDS:
                return tokens[at - 1], True
            # parentheses around a direct declarator, as in int (step)(void)
            name, function = declarator(tokens[at + 1:close])
            return name, function or tokens[close + 1:close + 2] == ['(']
        if token in ('[', '='):
            tokens = tokens[:at]
            break
    names = [word for at, word in enumerate(tokens) if NAME.fullmatch(word) and word not in KEYWORDS
             and not (at and tokens[at - 1] in TAGGED)]
    return (names[-1] if names else None), False


def split(tokens):
    """tokens, a declaration's, cut at each comma outside parentheses and
    brackets: its declarators, the first with the specifiers."""
    parts, depth = [[]], 0
    for token in tokens:
        depth += {'(': 1, '[': 1, ')': -1, ']': -1}.get(token, 0)
        if token == ',' and not depth:
            parts.append([])
        else:
            parts[-1].append(token)
    return parts


def unbracketed(tokens):
    """tokens, a declaration's, less those inside its parentheses and
    brackets: its specifiers and its declarators' names, without a
    parameter's."""
    found, depth = [], 0
    for token in tokens:
        depth += {'(': 1, '[': 1, ')': -1, ']': -1}.get(token, 0)
        if not depth and token not in (')', ']'):
            found.append(token)
    return found


def scan(text, gcc=None):
    """What text, a C file's, declares and defines at file scope, as two
    {name: line} maps: in every group of each conditional, or with gcc
    False or True, in the groups skipped() doesn't give."""
    lines = blanked(text).split('\n')
    for at in () if gcc is None else skipped(text, gcc):
        lines[at] = ''
    declared, defined = {}, {}
    for line, tokens, has_body in statements('\n'.join(lines)):
        outside = unbracketed(tokens)
        if not tokens or 'typedef' in outside or 'static' in outside:
            continue
        if has_body:
            name, _ = declarator(tokens[:tokens.index('{')])
            if name:
                defined.setdefault(name, line)
            continue
        for part in split(tokens):
            name, function = declarator(part)
            if not name:
                continue
            if function or ('extern' in outside and '=' not in part):
                declared.setdefault(name, line)
            else:
                defined.setdefault(name, line)
    return declared, defined


def skipped(text, gcc):
    """The indexes of the lines of text, a C file's, in each group that SHC
    skips, or with gcc, make dc's GCC, as far as the file alone decides. A
    test decides where the macros it names are KNOWN or defined or undefined
    by the file above it. Each other test takes every group of its
    conditional, and a macro that such a group defines or undefines is
    unknown past it."""
    lines = blank(text, ('comment',)).split('\n')
    for first, block in assembly_blocks(text).items():
        lines[first + 1:first + 1 + len(block)] = [''] * len(block)
    macros, known, types = predefined(gcc), set(KNOWN), Types([])
    # the conditionals open, as conditional() keeps them, and the index of the line past the last directive
    found, opened, past = set(), [], 0
    for directive in parse('\n'.join(lines)).directives:
        groups = [group for group, _ in opened]
        if False in groups:
            found.update(range(past, directive.line - 1))
        if directive.name in CONDITIONAL:
            conditional(directive, opened, macros, types, known)
        elif False not in groups and directive.name in ('define', 'undef') and directive.words:
            name = directive.words[0].text
            if None in groups:
                macros.pop(name, None)
                known.discard(name)
            else:
                define(directive, macros, types)
                known.add(name)
        past = directive.last
    if any(group is False for group, _ in opened):
        found.update(range(past, len(lines)))
    return found


def resolved(path, quoted, name):
    """The file an #include of name in the file at path finds, as a normal
    path, or None: from the file's directory first for a quoted name, then
    from SEARCHED."""
    for directory in ([Path(path).parent] if quoted else []) + [Path(each) for each in SEARCHED]:
        if (directory / name).is_file():
            return Path(os.path.normpath(directory / name))
    return None


def crossings(path, text):
    """A line for each header of modules/ that text, a file's under src/ at
    path, includes."""
    found = []
    for directive, quoted, name in header_names(text):
        header = resolved(path, quoted, name)
        if header is not None and header.parts[0] == 'modules':
            found.append(f'{path}:{directive.line}: includes {header.as_posix()}, a '
                         "header of the overlays, which the main executable can't rely on")
    return found


def repeats(files):
    """Each name that a source declares as an outside name and another
    source or a header declares too, as SHC or make dc's GCC reads them, as
    {name: [path:line, ...]}, its headers' places first. files is {path:
    text}."""
    found = {}
    for gcc in (False, True):
        places = {}
        for path in sorted(files, key=lambda each: (not each.endswith('.h'), each)):
            declared, defined = scan(files[path], gcc)
            for name, line in declared.items():
                if path.endswith('.h') or name not in defined:
                    places.setdefault(name, set()).add((not path.endswith('.h'), path, line))
        for name, where in places.items():
            paths = {path for _, path, _ in where}
            headers = all(path.endswith('.h') for path in paths)
            if len(paths) > 1 and not headers and not paths <= EXEMPT.get(name, set()):
                found.setdefault(name, set()).update(where)
    return {name: [f'{path}:{line}' for _, path, line in sorted(where)] for name, where in found.items()}


class Declarator(namedtuple('Declarator', 'name storage term initialized body file line last function names')):
    """A declarator, as declarations() reads it:

    - name: the name it declares, as the compiler sees it;
    - storage: its declaration's storage class, one of layout.py's STORAGE,
      or None;
    - term: its type, as declarations()' Types gives it;
    - initialized: whether its declarator has an initializer;
    - body: the name of the #include its initializer's braces hold alone,
      one the build generates, which no search of the tree finds, as a
      Path, or None;
    - file: the file its declaration opens in, as included() keys it;
    - line: the line its declaration opens on, counting from 1;
    - last: the line its initializer ends on, or its declarator where it
      has none;
    - function: the function whose blocks declare it, or None at file
      scope;
    - names: the set of each object and function its code names, as
      entity() gives them: those its initializer names, and for a
      function's definition, those its body names outside each initializer
      in it. A name that sizeof takes names none, nor does one in an if
      (0)'s block, which never runs."""


def entity(name, storage, term, function):
    """The object or function a declarator binds, as (name, function):
    function is the one whose blocks declare it static, or None where the
    name has linkage. A typedef or an automatic object binds none, and
    gives None. storage, term and function are as Declarator gives them. A
    block's extern and its declaration of a function bind the name with
    linkage."""
    if storage == 'typedef':
        return None
    if function is None or storage == 'extern' or term[0] == 'function':
        return name, None
    return (name, function) if storage == 'static' else None


class Unit(Tokens):
    """The tokens of a translation unit, as declarations() reads it, with
    the file each one lies in: files holds each token's, as included() keys
    it."""

    def __init__(self, tokens, files):
        super().__init__(tokens)
        self.files = files

    def where(self, at):
        return f'{self.files[at]}:{self[at].line}'


def declarations(path, gcc=False):
    """What the C source at path declares, with each file it includes, as
    SHC compiles them, or with gcc, as make dc's GCC compiles them: (texts,
    types, declarators). texts is included()'s texts of the source. types is
    the Types of the C the compiler reads, which lays out each declarator's
    term. declarators holds each Declarator at file scope and in each
    function's blocks, in the order the compiler reads them.

    The read runs as the compiler's preprocessor does. From the source's
    first line, it reads the file each literal #include finds, whatever its
    suffix, as csource's sites() finds it, where the #include stands, each
    time the read reaches it. A macro applies from its #define to its
    #undef, as expanded() replaces it. SHC's flags define DEFINED alone, and
    GCC's define GCC_DEFINED alone. The read defines no other macro a
    compiler predefines: __LINE__ and __GNUC__ read as names no macro
    replaces. Each conditional takes the group the compiler takes. A #pragma
    asm block and an inline body hold assembly, which declares nothing.

    The run fails, naming each, where csource's Walk can't read a
    declaration, where an #if's or an #elif's test the read reaches is one
    tested() can't evaluate, where a function's blocks declare two statics
    of one name, and where an #include reaches a file the read is inside."""
    texts = {}
    included(Path(path), texts, ANY_SUFFIX)
    types, macros = Types([]), predefined(gcc)
    # the unit's tokens, and the file each lies in; each directive the read takes but a conditional's, as the index
    # of the token after it and the name of the #include, or None for any other directive; and the files it is inside
    tokens, files, taken, inside = [], [], [], []
    # the file each #include of a file finds, by the directive's start, and the name each literal #include gives
    # that no search finds, one the build generates; and each file's key, which a read finds once
    found, names, keyed = {}, {}, lru_cache(maxsize=None)(key)

    def place(file, run):
        read = expanded(run, macros)
        tokens.extend(read)
        files.extend([file] * len(read))

    def take(file):
        if file in inside:
            fail(f'{file}: an #include reaches it again inside itself')
        inside.append(file)
        c, stops = compiled_c(file, texts[file], gcc)
        if file not in found:
            found[file] = {directive.start: keyed(each) for directive, each in sites(file, c.text, ANY_SUFFIX)}
            names[file] = {directive.start: Path(name) for directive, _, name in header_names(c.text)
                           if directive.start not in found[file]}
        at, opened = 0, []
        for directive, stop in zip(c.directives, stops):
            if all(group is not False for group, _ in opened):
                place(file, c.parsed[at:stop])
                if directive.name not in CONDITIONAL:
                    taken.append((len(tokens), names[file].get(directive.start)))
                    define(directive, macros, types)
                    if directive.start in found[file]:
                        take(found[file][directive.start])
            if directive.name in CONDITIONAL:
                conditional(directive, opened, macros, types)
                if opened and opened[-1][0] is None:
                    fail(f"{file}:{directive.line}: an #{directive.name} whose test the tool can't evaluate")
            at = stop
        place(file, c.parsed[at:])
        inside.pop()

    take(keyed(Path(path)))
    unit = Unit(tokens, files)
    try:
        reader = Reader(unit, types, types.take([token.text for token in unit]))
    except ValueError as error:
        fail(str(error))
    except IndexError:
        fail(f'{unit.where(len(unit) - 1)}: C that ends inside a declaration')
    return texts, types, reader.kept_declarators(taken)


# the read reaches a header at each #include of it, in each source that
# includes it, so a process makes each file's C once
@lru_cache(maxsize=None)
def compiled_c(path, text, gcc):
    """The C SHC reads of text, the file's at path, or with gcc, the C make
    dc's GCC reads, as csource's parse() reads it, and the index in its
    parsed tokens of the first past each of its directives. Its comments are
    blank, and each line is empty that lies on the side of a guard the
    compiler skips, in a #pragma asm block or inside an inline body. Each
    conditional's directive keeps its line."""
    lines = blank(text, ('comment',)).split('\n')
    for at in (matching_side if gcc else nonmatching_side)(lines):
        lines[at] = ''
    for first, block in assembly_blocks(text).items():
        lines[first + 1:first + 1 + len(block)] = [''] * len(block)
    for _, _, _, opening, closing in inline_bodies(path, blank('\n'.join(lines), ('string', 'character')).split('\n')):
        lines[opening + 1:closing] = [''] * (closing - opening - 1)
    c, stops = parse('\n'.join(lines)), []
    for directive in c.directives:
        stops.append(following(c.parsed, directive.start, stops[-1] if stops else 0))
    return c, stops


def predefined(gcc):
    """The macros SHC's flags define, or with gcc, make dc's GCC's, as
    define() keeps them, with None for a body the tool doesn't know."""
    return {name: (None, None if value is None else (Token(value, 'number', 0, 0, 0),))
            for name, value in (GCC_DEFINED if gcc else DEFINED).items()}


def conditional(directive, opened, macros, types, known=None):
    """Apply directive, a conditional's, to opened, as SHC's preprocessor
    does. opened holds a pair for each conditional open, innermost last:
    whether the read takes its group, and whether it took one of its groups,
    each None where the tool can't tell. macros, types and known are
    tested()'s."""
    if directive.name == 'endif':
        if opened:
            opened.pop()
        return
    if directive.name in OPENING:
        skipped = any(group is False for group, _ in opened)
        opened.append((False, True) if skipped else (None, False))
        if skipped:
            return
    if not opened:
        return
    took = opened[-1][1]
    if took:
        opened[-1] = False, True
    elif took is None:
        opened[-1] = None, None
    else:
        group = tested(directive, macros, types, known)
        opened[-1] = group, group


def tested(directive, macros, types, known=None):
    """Whether SHC's preprocessor takes the group that directive opens, in a
    conditional none of whose groups it took before: True, False, or None
    where the tool can't tell. It can't where Types can't evaluate an #if's
    or #elif's test, where the test's expansion leaves a macro's name, as
    one whose body the tool doesn't know, and with known, the set of the
    macros whose definition the tool knows, where the test names a macro
    outside it. macros and types are declarations()'."""
    name, words = directive.name, directive.words
    if name == 'else':
        return True
    if name in ('ifdef', 'ifndef', 'elifdef', 'elifndef'):
        if words and known is not None and words[0].text not in known:
            return None
        return (bool(words) and words[0].text in macros) == (name in ('ifdef', 'elifdef'))
    test, at = [], 0
    try:
        while at < len(words):
            # defined NAME and defined(NAME) read before the macros expand
            if words[at].text == 'defined':
                parenthesized = words[at + 1].text == '('
                named = words[at + 1 + parenthesized]
                if known is not None and named.text not in known:
                    return None
                test.append(named._replace(text=str(int(named.text in macros)), kind='number'))
                at += 2 + 2 * parenthesized
            else:
                test.append(words[at])
                at += 1
        read = expanded(test, macros)
        if any(word.kind == 'name' and (word.text in macros or known is not None and word.text not in known)
               for word in read):
            return None
        # a name no macro replaces reads as 0
        return bool(types.evaluate(['0' if word.kind == 'name' else word.text for word in read]))
    except (LayoutError, IndexError):
        return None


def define(directive, macros, types):
    """Apply directive to macros, as SHC's preprocessor does: a #define sets
    its macro, as (its parameters, or None for an object-like macro, and its
    body), and an #undef takes its name out. types takes each #define as
    its read() does, so that its evaluate() reads a macro's name, as a stop
    row's constant is (body_data.py's signed_stops())."""
    words = directive.words
    if directive.name not in ('define', 'undef') or not words or words[0].kind != 'name':
        return
    name, body, parameters = words[0].text, words[1:], None
    if directive.name == 'undef':
        macros.pop(name, None)
        return
    if body and body[0].text == '(' and body[0].start == words[0].end:
        close = next((at for at, word in enumerate(body) if word.text == ')'), len(body))
        parameters = tuple('__VA_ARGS__' if word.text == '...' else word.text
                           for word in body[1:close] if word.text != ',')
        body = body[close + 1:]
        types.functions.add(name)
    else:
        types.macros.setdefault(name, set()).add(tuple(word.text for word in body))
    macros[name] = parameters, tuple(body)


def expanded(tokens, macros, seen=frozenset()):
    """tokens, C's, with each macro of macros replaced as the preprocessor
    replaces it: an object-like macro's name by its body, and a
    function-like macro's name and arguments by its body, with each
    argument in place of its parameter. A replacement's tokens take the
    name's line, and are read again with the tokens after them, so a
    function-like macro's name at a replacement's end takes its arguments
    from those. No macro of seen is replaced, nor one in a replacement of
    its own, and a function-like macro's name with no arguments after it
    stays as written, as does a macro whose body the tool doesn't know."""
    # the tokens left to read, the next last, each with the macros its replacement can't replace
    found, ahead = [], [(token, seen) for token in reversed(tokens)]
    while ahead:
        token, hidden = ahead.pop()
        macro = macros.get(token.text) if token.kind == 'name' and token.text not in hidden else None
        if macro is None or macro[1] is None:
            found.append(token)
            continue
        parameters, body = macro
        if parameters is not None:
            given = arguments(ahead)
            if given is None:
                found.append(token)
                continue
            body = substituted(parameters, body, given, macros, hidden)
        ahead += [(word._replace(line=token.line), hidden | {token.text}) for word in reversed(body)]
    return found


def arguments(ahead):
    """The arguments of a function-like macro, each a list of its tokens,
    taken from ahead, expanded()'s tokens left to read, through their ), or
    None, leaving ahead as it was, where the next is no ( or no ) closes
    it."""
    if not ahead or ahead[-1][0].text != '(':
        return None
    taken, found, depth = [ahead.pop()], [[]], 0
    while ahead:
        taken.append(ahead.pop())
        token = taken[-1][0]
        if not depth and token.text == ')':
            return found
        if not depth and token.text == ',':
            found.append([])
        else:
            depth += (token.text == '(') - (token.text == ')')
            found[-1].append(token)
    ahead += reversed(taken)
    return None


def substituted(parameters, body, given, macros, seen):
    """body, a function-like macro's, with each of its parameters replaced
    by its argument of given, as the preprocessor replaces them: after a #
    by the argument spelled as a string, beside a ## by the argument as
    written, pasted to the token on the ##'s other side, and elsewhere by
    the argument with its macros replaced. __VA_ARGS__ takes the arguments
    past the named parameters, with their commas."""
    named = dict(zip(parameters, given))
    if parameters[-1:] == ('__VA_ARGS__',):
        comma = Token(',', 'punctuator', 0, 0, 0)
        named['__VA_ARGS__'] = [word for at, argument in enumerate(given[len(parameters) - 1:])
                                for word in [comma][:at] + argument]
    # None stands for an empty argument beside a ##, which pastes to nothing
    found, at = [], 0
    while at < len(body):
        word, after = body[at], body[at + 1] if at + 1 < len(body) else None
        if word.text == '#' and after and after.text in named:
            found.append(word._replace(text=quoted(named[after.text]), kind='string'))
            at += 1
        elif word.text == '##' and found and after:
            left, right = found.pop(), list(named.get(after.text, [after]))
            if left is not None and right:
                right[0] = pasted(left, right[0])
            found += right or [left]
            at += 1
        elif word.text in named:
            argument = named[word.text]
            found += (argument or [None]) if after and after.text == '##' else expanded(argument, macros, seen)
        else:
            found.append(word)
        at += 1
    return [word for word in found if word is not None]


def pasted(left, right):
    """The token ## makes of left and right, two tokens, as the kind WORD
    reads its text as."""
    text = left.text + right.text
    match = WORD.fullmatch(text)
    return left._replace(text=text, kind=match.lastgroup if match else 'other')


def quoted(tokens):
    """tokens spelled as a string, as # spells them: one blank where blanks
    part two of them in their text, and a \\ before each \\ and " of a string
    or a character constant."""
    spelled = ''
    for at, token in enumerate(tokens):
        text = token.text
        if token.kind in ('string', 'character'):
            text = text.replace('\\', '\\\\').replace('"', '\\"')
        spelled += (' ' if at and token.start > tokens[at - 1].end else '') + text
    return f'"{spelled}"'


class Reader(Typed):
    """The Walk of a translation unit's Unit, whose texts types.take() read
    as file, that keeps each declarator at file scope and in each function's
    blocks, but a parameter's, and what each one's code names."""

    def __init__(self, tokens, types, file):
        # the index of the first token of each declaration the walk is in, innermost last
        self.heads = []
        # each function the walk is in, innermost last, as its name and the index of its body's {
        self.functions = []
        # each declarator kept, as (name, term, head, past, function). head is the index of its declaration's first
        # token, and past is the index past the declarator and its attributes, where an = opens its initializer
        self.kept = []
        # the index of each declaration's first declarator, past its specifiers, by the index of its first token
        self.specified = {}
        # each initializer the walk reads, as its first index and the one past its last, by the index of its =
        self.initializers = {}
        # each binding of each name, a parameter's and an enumerator's too, as the index where its scope opens, the
        # one where it closes, and the object or function it binds, as entity() gives it, or None
        self.bound = {}
        # each function's body, as the index of its { and the one past its }, by the index in kept of its definition
        self.defined = {}
        super().__init__(tokens, types, file)

    def declaration(self, at, end, outer):
        self.heads.append(at)
        try:
            return super().declaration(at, end, outer)
        finally:
            self.heads.pop()

    def function(self, name, first, at):
        body = at
        while self.tokens[body].text != '{':
            body += 1
        self.functions.append((name, body))
        # declared() kept the definition's declarator last
        definition = len(self.kept) - 1
        try:
            past = super().function(name, first, at)
        finally:
            self.functions.pop()
        self.defined[definition] = body, past
        return past

    def declared(self, name, kind, term, span, start, end):
        head, bound = self.heads[-1], None
        function = self.functions[-1][0] if self.functions else None
        # a parameter's declaration lies ahead of its function's body
        if function is None or head > self.functions[-1][1]:
            self.specified.setdefault(head, span[0])
            self.kept.append((name, term, head, start, function))
            bound = entity(name, self.storage(head), term, function)
        self.bound.setdefault(name, []).append((start, end, bound))

    def storage(self, head):
        """The storage class of the declaration whose first token is at head, or None."""
        return next((token.text for token in self.tokens[head:self.specified[head]] if token.text in STORAGE), None)

    def initializer(self, start, end, static, term, why):
        if why is None:
            self.initializers[start - 1] = start, end
        super().initializer(start, end, static, term, why)

    def typedef_at(self, name, at):
        return name in self.types.typedefs

    def enumerated(self, name, start, end):
        self.bound.setdefault(name, []).append((start, end, None))

    def static(self, start, end, term, why):
        pass

    def named(self):
        """The objects and functions each declarator kept names, as entity()
        gives them, in a set by its index in kept: those its initializer
        names, and for a function's definition, those its body names outside
        each initializer in it."""
        tokens, unnamed = self.tokens, self.unnamed()
        spans = [(*self.initializers[past], index) for index, (_, _, _, past, _) in enumerate(self.kept)
                 if past in self.initializers]
        spans += [(*span, index) for index, span in self.defined.items()]
        # each token's owner: the declarator whose span holds it innermost, an initializer in a function's body
        owners = {}
        for start, stop, index in sorted(spans, key=lambda span: (span[0], -span[1])):
            owners.update(dict.fromkeys(range(start, stop), index))
        found = [set() for _ in self.kept]
        for at, index in owners.items():
            token = tokens[at]
            # a member's name or a tag binds no object or function
            if token.kind == 'name' and at not in unnamed and tokens[at - 1].text not in ('.', '->', *TAGGED):
                # the innermost binding in scope
                bound = max((each for each in self.bound.get(token.text, ()) if each[0] <= at < each[1]),
                            key=itemgetter(0), default=None)
                if bound and bound[2]:
                    found[index].add(bound[2])
        return found

    def unnamed(self):
        """The indexes of the tokens whose names a declarator's code doesn't
        take: each declarator's own; each of a struct's or a union's members;
        a label, and a goto's; the operand of a sizeof or of a static
        assertion, which the program never evaluates; and an if (0)'s block,
        which never runs."""
        tokens, found = self.tokens, set()
        for first, past in self.declarators:
            found.update(range(first, past))
        for at, token in enumerate(tokens):
            # a label opens a statement, where the middle of a ?: can't stand
            labeled = tokens[at + 1:at + 2] and tokens[at + 1].text == ':' and tokens[at - 1].text in STATEMENT_OPENS
            if token.kind == 'name' and (labeled or tokens[at - 1].text == 'goto'):
                found.add(at)
            elif token.text in ('struct', 'union') and (opening := tokens.members(at)) is not None:
                found.update(range(opening, tokens.closing(opening)))
            elif token.text in ('sizeof', STATIC_ASSERT):
                found.update(range(at + 1, self.operand(at + 1)))
            elif token.text == 'if' and [each.text for each in tokens[at + 1:at + 5]] == ['(', '0', ')', '{']:
                found.update(range(at + 4, tokens.closing(at + 4)))
        return found

    def operand(self, at):
        """The index past the operand of a sizeof or of a static assertion,
        which opens at at: a parenthesized type or expression, or a unary
        expression."""
        tokens = self.tokens
        while tokens[at].text in ('*', '&', '-', '+', '!', '~'):
            at += 1
        # closing() gives a token that opens no bracket as its own close
        at = tokens.closing(at) + 1
        while tokens[at].text in ('[', '(', '.', '->'):
            at = tokens.closing(at) + 1 if tokens[at].text in ('[', '(') else at + 2
        return at

    def kept_declarators(self, taken):
        """The Declarator of each declarator kept. taken is declarations()':
        each directive the read takes but a conditional's, as the index of
        the token after it and the name of the #include, or None. The run
        fails on each declaration the walk can't read, and on each static of
        a function's that shares its name with another of the function's."""
        tokens, after = self.tokens, [index for index, _ in taken]
        unread = sorted({*self.skipped, *((start, why) for start, _, why in self.unread)})
        if unread:
            fail(*(f'{tokens.where(start)}: a declaration the tool can\'t read: {why}' for start, why in unread))
        # a function's statics, by the entity() each gives, which two of one name would share
        statics, twice = {}, []
        for name, _, head, _, function in self.kept:
            if function is not None and self.storage(head) == 'static':
                if (name, function) in statics:
                    twice.append(f'{tokens.where(head)}: a second static {name} in {function}, after '
                                 f'{tokens.where(statics[name, function])}, which the tool can\'t tell from it')
                statics.setdefault((name, function), head)
        if twice:
            fail(*twice)
        found = []
        for (name, term, head, start, function), names in zip(self.kept, self.named()):
            initialized, body = self.initializers.get(start), None
            if initialized:
                first, stop = initialized
                # braces that hold one #include alone, of the directives the read takes
                held = [include for _, include in taken[bisect_right(after, first):bisect_right(after, stop - 1)]]
                if stop == first + 2 and tokens[first].text == '{' and len(held) == 1:
                    body = held[0]
            last = tokens[(initialized[1] if initialized else start) - 1].line
            found.append(Declarator(name, self.storage(head), term, bool(initialized), body, tokens.files[head],
                                    tokens[head].line, last, function, frozenset(names)))
        return found


def main(argv=None):
    parser = argparse.ArgumentParser(prog='decls', description=__doc__.split('\n\n')[0])
    parser.add_argument('files', metavar='FILE', nargs='+', help='a C source or header')
    args = parser.parse_args(argv)
    # each file once, by its path from the root, however it was spelled
    files = {Path(os.path.relpath(path)).as_posix(): path for path in args.files}
    files = {path: content(given).decode('latin-1') for path, given in files.items()}
    found = [line for path, text in files.items() if path.startswith('src/') for line in crossings(path, text)]
    found += [f"{name}: declared at {', '.join(where)}, more than once as an outside name; one header these files "
              'include can declare it' for name, where in repeats(files).items()]
    if found:
        fail(*found)


if __name__ == '__main__':
    main()
