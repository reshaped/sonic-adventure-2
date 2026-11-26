"""Split a C source's inline bodies from its C, for make dc.

    python3 tools/split.py SOURCE OUT TEXT

OUT gets SOURCE line for line, under #line 1 "SOURCE", with each inline
body blank: its #pragma inline_asm line, its lines and its function's
closing brace. Its opening brace becomes a ;, so that the function's head
declares the function. Balanced guards in that declaration stay in OUT.

TEXT gets each body GCC compiles, in source order, after a comment that
names its line and its function, as SHC writes a body out as a function of
its own (TOOLCHAIN.md#assembly): an .EXPORT unless the function is static,
its label, its lines, then the .ALIGN 4, RTS and NOP SHC writes after one.
That RTS returns from a body that runs off its end. SHC writes a static
body out nowhere, but GCC's code calls it, so TEXT writes it out too.

Where TEXT holds a body, OUT ends in a file-scope asm that includes TEXT's
translation, TEXT with a .s suffix. The translation selects its own
section, P, and GCC would go on writing its code where the asm left GNU as.
So the asm keeps GCC's section on GNU as's stack around the include. It
names each body's function as a ":" operand, so that GCC binds even a
static one to the asm's label.

A body GCC doesn't compile, one on the matching side of a guard, is SHC's
alone: TEXT doesn't take it, and the asm names no function for it. make
dc's compile defines NONMATCHING and DREAMCAST, so the tool reads a side
by the #ifdef of a switch that opens its guard, as tools/guards.py reads
directives.

The tool finds a body as tools/inline.py does. It writes nothing, and
fails, naming each line and its function, on:

- a #pragma inline_asm not in inline.py's form above a function, or with
  another function below it;
- a #pragma asm GCC reads unless the block declares its own P code section
  and defines all of its exported functions, or ends in RTS/NOP and defines
  a private label a public block branches to. These blocks keep their
  assembly, including their literal pools. A fragment between C instructions
  depends on SHC's layout and cannot be moved;
- a directive inside a body, which the split can't carry into TEXT;
- a body under a conditional other than a guard's #ifdef of a switch;

and in a body GCC compiles, on:

- a .SECTION, which would leave the code after it outside the function
  that the label opens;
- an .END, which would truncate the GNU assembly. A file-scope P block may
  end with a bare .END if only comments and closing guards follow it. TEXT
  leaves that terminal directive to the native assembler;
- a BRA, BSR, BT or BF to other than a label, such as to a name plus or
  minus an offset, a place in SHC's layout that GCC's doesn't keep. A BRAF
  or BSRF adds a register to its own address, which the tool doesn't read:
  a table of the body's own label differences keeps it inside the body.

It fails first, naming the path, where SOURCE, or TEXT with a .s suffix,
holds other than letters, digits, _, ., / and -: OUT's #line names SOURCE,
and its .include TEXT's translation, each unescaped. It fails too where two
of SOURCE, OUT and TEXT name one file, which a save would overwrite.

PORTS.md#inline-assembly says how make dc builds OUT and TEXT into the
source's object.
"""

import argparse
import re
from pathlib import Path

import csource
import guards
import inline
from config import content, fail, save

# a .SECTION statement, after any label. A statement is a body's line up to
# its first ;, where asmsh opens a comment unless a string holds the ;,
# which the tool doesn't tell apart
SECTION = re.compile(r'\s*(?:[\w?]+:)?\s*\.SECTION\b', re.I)
# a branch, after any label, and its target, the rest of its statement; and
# the one target the tool takes, a label
BRANCH = re.compile(r'\s*(?:[\w?]+:)?\s*(?:BSR|BRA|BT|BF|BT/S|BF/S)\s+(.*?)\s*$', re.I)
TARGET = re.compile(r'[A-Za-z_?][\w?]*')
CODE_SECTION = re.compile(r'\s*\.SECTION\s+P\s*,\s*CODE\s*,\s*ALIGN=\d+\s*', re.I)
MODULE_END = re.compile(r'\s*(?:[\w?]+:)?\s*\.END\b', re.I)
EXPORT = re.compile(r'\s*\.EXPORT\s+_([A-Za-z_]\w*)\s*', re.I)
# the characters of a path OUT's #line and .include write unescaped
PATH = re.compile(r'[A-Za-z0-9_./-]+')
# the lines SHC writes after a body in its function of its own
TRAILER = ['        .ALIGN      4', '        RTS', '        NOP']


def compiled(text, defined=guards.SWITCHES):
    """For each line of text, C, whether a compile that defines the
    switches in defined reads it: True, False, or None under a conditional
    the tool can't read. make dc's compile defines both switches. An
    #ifdef of a switch reads as whether defined holds it, and its #else as
    the other; any other conditional, and each of its groups, reads as
    None."""
    lines = text.split('\n')
    marks = {line - 1: (name, names) for line, name, names in guards.directives(text)}
    found, groups = [], []
    for at in range(len(lines)):
        name, names = marks.get(at, (None, []))
        if name in guards.OPENING:
            groups.append(names[0] in defined if name == 'ifdef' and names[:1] and names[0] in guards.SWITCHES
                          else None)
        elif name in guards.LATER:
            groups[-1:] = [None]
        elif name == 'else' and groups:
            groups[-1] = None if groups[-1] is None else not groups[-1]
        elif name == 'endif' and groups:
            groups.pop()
        found.append(False if False in groups else None if None in groups else True)
    return found


def problems_in(path, name, first, lines):
    """A problem for each .SECTION, .END and branch to other than one label
    in lines, a body's from line first, counting from 1, of function name."""
    found = []
    for number, line in enumerate(lines, first):
        statement = line.split(';')[0]
        if SECTION.match(statement):
            found.append(f'{path}:{number}: {name}: a .SECTION, which would leave the code after it outside the '
                         'function its label opens')
        if MODULE_END.match(statement):
            found.append(f'{path}:{number}: {name}: the .END directive would cut off the rest of the GNU assembly')
        branch = BRANCH.match(statement)
        if branch and not TARGET.fullmatch(branch[1]):
            found.append(f"{path}:{number}: {name}: {statement.strip()}, a branch to other than a label, which may "
                         "be a place in SHC's layout that GCC's doesn't keep")
    return found


def block_entries(lines):
    """Public entries, or private branch labels, in a standalone P block.
    A private block must return without falling through into the following C."""
    statements = [line.partition(';')[0].strip() for line in lines]
    statements = [statement for statement in statements if statement]
    if not statements or not CODE_SECTION.fullmatch(statements[0]):
        return [], set()
    if any(SECTION.match(statement) or MODULE_END.match(statement) or statement.startswith('#')
           for statement in statements[1:]):
        return [], set()
    exports = [statement for statement in statements if statement.upper().startswith('.EXPORT')]
    names = [entry[1] for statement in exports if (entry := EXPORT.fullmatch(statement))]
    if len(names) < len(exports):
        return [], set()
    if names:
        return (names if all(f'_{name}:' in statements for name in names) else []), set()
    if [statement.upper() for statement in statements[-2:]] != ['RTS', 'NOP']:
        return [], set()
    return [], {statement[:-1] for statement in statements if re.fullmatch(r'_[A-Za-z_]\w*:', statement)}


def assembly_blocks(source):
    """The lines in each real pragma asm block, keyed by its opening line's index."""
    if 'endasm' not in source.replace('\\\n', ''):
        return {}
    lines, blocks, opened = source.split('\n'), {}, None
    for directive in csource.parse(source).directives:
        if directive.name != 'pragma' or len(directive.words) != 1:
            continue
        name = directive.words[0].text
        if name == 'asm':
            opened = directive.line - 1
        elif name == 'endasm' and opened is not None:
            blocks[opened] = lines[opened + 1:directive.line - 1]
            opened = None
    return blocks


def split(path, source, included):
    """OUT's lines and TEXT's lines for source, the C at path. included is
    the path of TEXT translated for GNU as, which OUT's asm includes. The
    run fails on the refusals the module docstring lists, naming each."""
    lines = source.split('\n')
    compiles = compiled(source)
    directed = {line - 1: name for line, name, _ in guards.directives(source)}
    problems, empty, openings, pieces = [], set(), set(), []
    private, targets = [], set()
    parsed = csource.parse(source)
    pragmas = {directive.line - 1: directive.words[0].text for directive in parsed.directives
               if directive.name == 'pragma' and directive.words}
    found = {at: body for body in inline.BODY.finditer(source)
             if pragmas.get(at := source.count('\n', 0, body.start())) == 'inline_asm'}
    blocks = assembly_blocks(source)
    for at, line in enumerate(lines):
        if pragmas.get(at) == 'asm' and compiles[at] is not False:
            held = blocks.get(at, [])
            body = held
            last = max((i for i, row in enumerate(held) if row.partition(';')[0].strip()), default=None)
            if last is not None and held[last].partition(';')[0].strip().upper() == '.END':
                # The last native module directive has no place in the GCC include: .end would cut off GCC's
                # own code. Drop it only at actual end of source, beyond comments and closing guards.
                tail = csource.parse('\n'.join(lines[at + len(held) + 2:]))
                if not tail.parsed and all(d.name == 'endif' and not d.words for d in tail.directives):
                    body = held[:last] + held[last + 1:]
            entries, labels = block_entries(body) if compiles[at] else ([], set())
            if (entries or labels) and not any(b.opening.line <= at + 1 <= csource.closed(b, lines) + 1
                                               for b in parsed.blocks):
                header = next(i for i, row in enumerate(held) if CODE_SECTION.fullmatch(row.partition(';')[0]))
                name = entries[0] if entries else 'private block'
                problems += problems_in(path, name, at + header + 3, body[header + 1:])
                if entries:
                    targets.update(branch[1] for row in body if (branch := BRANCH.fullmatch(row.partition(';')[0])))
                else:
                    private.append((at, labels))
                empty.update(range(at, at + len(held) + 2))
                pieces.append((at, entries, body))
            else:
                problems.append(f"{path}:{at + 1}: #pragma asm, whose block the split doesn't take")
        if pragmas.get(at) == 'inline_asm' and at not in found:
            problems.append(f'{path}:{at + 1}: not #pragma inline_asm(<function>) or '
                            '#pragma inline_asm(<function>(size=<n>)) above a function')
    for at, labels in private:
        if labels.isdisjoint(targets):
            problems.append(f'{path}:{at + 1}: private assembly has no branch from a public block')
    for at, body in sorted(found.items()):
        name = body[1]
        opening = source.count('\n', 0, body.start(2)) - 1
        closing = source.count('\n', 0, body.end())
        head = ' '.join(lines[at + 1:opening + 1])
        declared = re.search(r'(\w+)\s*\(', head)
        if not declared or declared[1] != name:
            problems.append(f'{path}:{at + 1}: #pragma inline_asm({name}), with another function below it')
        # A conditional entirely within the C declaration stays there in OUT.
        # A conditional enclosing the opening brace would instead change which
        # assembly is present. Diagnose it along with directives in the assembly.
        guards_in_head = []
        for i, directive in directed.items():
            if not at < i < closing:
                continue
            if i < opening:
                if directive in guards.OPENING:
                    guards_in_head.append(i)
                    continue
                if directive == 'endif' and guards_in_head:
                    guards_in_head.pop()
                    continue
                if directive in ('else', *guards.LATER) and guards_in_head:
                    continue
            problems.append(f"{path}:{i + 1}: {name}: a directive inside the body, which the split can't carry into TEXT")
        problems += [f"{path}:{i + 1}: {name}: a conditional in the declaration crosses the opening brace"
                     for i in guards_in_head]
        if compiles[at] is None:
            problems.append(f"{path}:{at + 1}: {name}: a body under a conditional the tool can't read")
        empty.update([at, *range(opening + 1, closing + 1)])
        openings.add(opening)
        if not compiles[at]:
            continue
        held = lines[opening + 1:closing]
        problems += problems_in(path, name, opening + 2, held)
        exported = [] if re.match(r'\s*static\b', head) else [f'        .EXPORT     _{name}']
        pieces.append((at, [name], [f'; {path}:{at + 1}: {name}', *exported, f'_{name}:', *held, *TRAILER]))
    if problems:
        fail(*problems)
    # an opening brace may close its function's head's last line. The empty
    # string after SOURCE's last LF ends that line rather than starting one
    rows = lines[:-1] if source.endswith('\n') else lines
    c = [f'#line 1 "{path}"'] + ['{0};{2}'.format(*line.rpartition('{')) if i in openings else '' if i in empty
                                 else line for i, line in enumerate(rows)]
    pieces.sort()
    if pieces:
        operands = ', '.join(f'":"({name})' for _, names, _ in pieces for name in names)
        c += [f'__asm__(".pushsection .text\\n.include \\"{included}\\"\\n.popsection"', f'        : : {operands});']
    return c, [line for _, _, piece in pieces for line in piece]


def main(argv=None):
    parser = argparse.ArgumentParser(prog='split', description=__doc__.split('\n\n')[0])
    parser.add_argument('source', metavar='SOURCE', help='a C source with inline bodies, such as sdk/ninja/njSin.c')
    parser.add_argument('out', metavar='OUT', help="where to write SOURCE's C, such as build/dc/sdk/ninja/njSin.c")
    parser.add_argument('text', metavar='TEXT',
                        help="where to write SOURCE's bodies, such as build/dc/sdk/ninja/njSin_inline.src")
    args = parser.parse_args(argv)
    included = Path(args.text).with_suffix('.s')
    problems = [f"{path}: holds other than letters, digits, _, ., / and -, so OUT can't write it unescaped"
                for path in (args.source, str(included)) if not PATH.fullmatch(path)]
    named = {}
    for path in (args.source, args.out, args.text):
        named.setdefault(Path(path).resolve(), []).append(path)
    problems += [f"{' and '.join(paths)}: name one file, which a save would overwrite"
                 for paths in named.values() if len(paths) > 1]
    if problems:
        fail(*problems)
    # latin-1 maps each byte to one character and back, so OUT and TEXT keep SOURCE's bytes, Shift JIS comments
    # included
    c, assembly = split(args.source, content(args.source).decode('latin-1'), included)
    save(args.text, ''.join(f'{line}\n' for line in assembly).encode('latin-1'))
    save(args.out, ''.join(f'{line}\n' for line in c).encode('latin-1'))


if __name__ == '__main__':
    main()
