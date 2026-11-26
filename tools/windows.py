"""List the turn windows of the game's C, classed by shape: the places where
it builds a matrix on the unit matrix to transform a vector or a point by.
Or check the guarded ones.

    python3 tools/windows.py [--list | --check] [FILE...]

A window opens at a push of the unit matrix: matrix_push_unit(),
njPushMatrix(&_nj_unit_matrix_), or a push whose next call is
njUnitMatrix(NULL). A push is njPushMatrixEx(), or njPushMatrix with any
matrix. The window closes at the pop that takes the stack back to where
the window found it: njPopMatrixEx() pops one level, and njPopMatrix(n)
pops n. A pop past that level closes no window. A count that isn't a C
integer literal counts as one. FILE is each C source to read, by default
each .c file of src/ and modules/. Without FILE, a working directory that
lacks one of them fails. The C reads as SHC reads it, where a conditional
tests NONMATCHING or DREAMCAST by #ifdef, #ifndef or #if [!]defined: the
side SHC builds, with comments blanked. The scan for calls and branches
also blanks literals, and an argument keeps them as written.

A window's steps are its calls on the current matrix, in order, and its
shape names them: X, Y or Z for a turn by njRotateX, njRotateY or
njRotateZ, R for one by njRotate, XYZ or ZXY for njRotateXYZ or
njRotateZXY, T for a move by njTranslate, njTranslateV or njTranslateEx, S
for a scale by njScale, njScaleV or njScaleEx, U for njUnitMatrix, and
vector, point or points for a read by njCalcVector, njCalcPoint or
njCalcPoints, each with a null matrix. So "Y X Z vector" turns a vector by
the matrix that njRotateY, njRotateX and njRotateZ build, called in that
order. A window counts under four marks where they hold:

- calls: it calls a function that is none of its steps, such as a draw,
  whose use of the matrix this can't see.
- spread: a statement that is none of its calls lies between its push and
  its pop.
- branches: an if, a loop, a switch, a goto, a return or a ?: lies inside
  it, so its steps may not all run, or its pop may not close it.
- guarded: it stands on the matching side of a guard, whose non-matching
  side holds what replaces it.

KERNELS names the shapes whose bits a kernel of engine/dc/turn.h gives:
one read of a vector after one turn, or after three in an order that
zam_vec3_turn_yxz or zam_vec3_turn_zxy takes, and one read of a point
after a move and a turn about y, or after a move and three turns in the
order zam_vec3_place_xyz takes. A window of such a shape that counts under
no mark but guarded is one run of statements, which a guard can hold
whole: PORTS.md's form gives it its kernel's call (PORTS.md#kernels).

It prints a row for each shape: how many windows hold it, in how many
sources, how many of them count under each mark, and its kernel. Totals
follow, and the sources that hold the most windows. With --list it prints
each window instead, one to a line: its place, its function, the call
that opens it, its shape and marks, and each step's call. An
njUnitMatrix(NULL) that opens no window and lies in none, and a window
that no pop in its function closes, print as such.

With --check it checks each guarded window: its guard's matching side
must be exactly the window, its push's statement through its pop's, and
the guard's non-matching side must be one call statement, of its shape's
kernel. Each pop inside the window and the one that closes it must count
its levels by a number, no step's argument may call a function or change a
variable, and no directive may lie inside the guard.
The call must take each angle as the window's turn about that axis spells
it, and a move's vector and the read's two vectors each by its first
float: &v.x for &v, &p->x for p, and e for a cast (T *)e. It fails,
naming each window that breaks the rule and how. It measures and checks,
and changes no file.
"""

import argparse
import re
from collections import Counter, defaultdict
from dataclasses import dataclass, field
from pathlib import Path

from config import at_root, content, fail

# a string or character literal, which the scan for calls blanks
LITERAL = re.compile(r'"(?:\\.|[^"\\\n])*"|\'(?:\\.|[^\'\\\n])*\'')
# a comment or a literal: a comment is blanked, and a literal keeps a comment's
# opener in it from being one
NOISE = re.compile(r'/\*.*?\*/|//[^\n]*|' + LITERAL.pattern, re.S)
# a preprocessor line: its name and the rest
DIRECTIVE = re.compile(r'[ \t]*#[ \t]*([a-z]+)(.*)')
# a conditional's test of one name: whether it is defined, and the name
DEFINED = re.compile(r'(!?)\s*defined\s*(?:\(\s*(\w+)\s*\)|(\w+))\s*')
# the switches a conditional selects by, each with whether SHC's build defines it
SWITCHES = {'NONMATCHING': False, 'DREAMCAST': True}
# a C integer literal, and its digits
INTEGER = re.compile(r'(0[xX][0-9a-fA-F]+|0[0-7]*|[1-9][0-9]*)[uUlL]*')
# what changes a variable in a step's argument
ASSIGNS = re.compile(r'\+\+|--|(?<![=!<>])=(?!=)')
CALL = re.compile(r'\b([A-Za-z_]\w*)\s*\(')
# what makes a window's steps run in other than their written order
BRANCH = re.compile(r'\b(if|for|while|do|switch|goto|return|break|continue)\b|\?')
# what C spells with a parenthesis after a name, which no call is
KEYWORDS = {'if', 'for', 'while', 'switch', 'return', 'sizeof', 'case', 'do', 'else'}
# a null matrix, as the sources spell it, and the unit matrix a push loads
NULL = ('NULL', '0')
UNIT = '&_nj_unit_matrix_'
# a cast to a pointer, and what it casts
CAST = re.compile(r'\(\s*(?:const\s+)?\w+\s*\*\s*\)\s*(.+)')
# a name with members and subscripts, which binds tighter than & and a cast
POSTFIX = re.compile(r'[A-Za-z_]\w*(?:\.\w+|->\w+|\[[^\[\]]*\])*')

# each step's letters by the member that takes the matrix first, and by the
# member that works on the current matrix alone
STEPS = {'njRotateX': 'X', 'njRotateY': 'Y', 'njRotateZ': 'Z', 'njRotate': 'R', 'njRotateXYZ': 'XYZ',
         'njRotateZXY': 'ZXY', 'njTranslate': 'T', 'njTranslateV': 'T', 'njScale': 'S', 'njScaleV': 'S',
         'njUnitMatrix': 'U', 'njCalcVector': 'vector', 'njCalcPoint': 'point', 'njCalcPoints': 'points'}
CURRENT = {'njTranslateEx': 'T', 'njScaleEx': 'S'}
MARKS = ('calls', 'spread', 'branches', 'guarded')
# turn.h's kernel for each shape whose bits it gives, and what each of its
# arguments takes: the angle of the turn about an axis, the move's vector,
# or the read's vector in or out
KERNELS = {
    'X vector': ('zam_vec3_turn_x', ('X', 'in', 'out')),
    'Y vector': ('zam_vec3_turn_y', ('Y', 'in', 'out')),
    'Z vector': ('zam_vec3_turn_z', ('Z', 'in', 'out')),
    'Z X Y vector': ('zam_vec3_turn_yxz', ('X', 'Y', 'Z', 'in', 'out')),
    'Y X Z vector': ('zam_vec3_turn_zxy', ('X', 'Y', 'Z', 'in', 'out')),
    'T Y point': ('zam_vec3_place_y', ('T', 'Y', 'in', 'out')),
    'T Z Y X point': ('zam_vec3_place_xyz', ('T', 'X', 'Y', 'Z', 'in', 'out')),
}
# how many sources the summary names, the most windows first
MOST = 12


@dataclass
class Call:
    """A call in a function's body: its name, its arguments as written with
    their spaces collapsed, its line, and the span of its text."""
    name: str
    args: list
    line: int
    start: int
    end: int

    def __str__(self):
        return f'{self.name}({", ".join(self.args)})'


@dataclass
class Window:
    """A window, as the module's docstring defines it: where it opens, its
    function, the call that opens it, its steps as (letters, call), with
    'call' for a call that is none, and its marks. closed is False where no
    pop closes it, and pop is the pop that does. replaced is its guard's
    non-matching side, where it is guarded, and exact whether the guard's
    matching side is the window and no more."""
    path: str
    function: str
    line: int
    opener: str
    steps: list = field(default_factory=list)
    marks: set = field(default_factory=set)
    closed: bool = True
    pop: Call = None
    replaced: str = ''
    exact: bool = False

    @property
    def shape(self):
        return ' '.join(letters for letters, _ in self.steps if letters != 'call')

    @property
    def kernel(self):
        """turn.h's kernel for the window's shape, or None."""
        return KERNELS[self.shape][0] if self.shape in KERNELS else None

    @property
    def taken(self):
        """Whether the window is closed, has a kernel's shape and counts
        under no mark but guarded: one run of statements, which PORTS.md's
        form can hold. Whether a guard's call is the kernel's is --check's."""
        return bool(self.kernel) and self.closed and not self.marks - {'guarded'}


def selects(name, rest):
    """What a conditional opened by #name rest selects by: (switch, built)
    for a test of one of SWITCHES, where built is 0 if SHC builds its first
    side and 1 if its #else, or (None, None) for a test this can't select."""
    if name in ('ifdef', 'ifndef'):
        switch, defined = rest.strip(), name == 'ifdef'
    else:
        test = DEFINED.fullmatch(rest.strip())
        if not test:
            return None, None
        switch, defined = test[2] or test[3], not test[1]
    if switch not in SWITCHES:
        return None, None
    return switch, 0 if SWITCHES[switch] == defined else 1


def matching_side(text):
    """text, C, as SHC reads it, and its guards. Comments and preprocessor
    lines are blanked, with each line a directive continues, and so is each
    side of a conditional that SHC doesn't build. Every newline stays, so a
    place keeps its line, which counts from 1. The guards of NONMATCHING are
    {line: (guard, replaced, matched)} for each line of a guard's matching
    side: guard counts the guards in the order they close, from 0, and
    replaced and matched are the text of the guard's two sides, comments
    blanked, with each directive nested in them."""
    # the lines that end in a backslash, which C joins to the next before it
    # reads a comment
    spliced = {number for number, line in enumerate(text.split('\n'), 1) if line.endswith('\\')}
    text = NOISE.sub(lambda noise: noise[0] if noise[0][0] in '"\'' else re.sub(r'[^\n]', ' ', noise[0]), text)
    kept, guards, closed = [], {}, 0
    # for each conditional open: its switch, the side SHC builds, the side
    # its lines fall in now, and the lines of each side
    groups = []
    continued = False
    for number, line in enumerate(text.split('\n'), 1):
        directive = None if continued else DIRECTIVE.match(line)
        if not continued and not directive:
            built = all(switch is None or side == at for switch, side, at, _ in groups)
            kept.append(line if built else '')
            for group in groups:
                group[3][group[2]].append((number, line))
            continue
        continued = number in spliced
        kept.append('')
        name = directive[1] if directive else None
        # a directive lies in each side it stands inside: an #else or #endif
        # isn't its own conditional's, and an #elif is
        own = groups[-1] if groups and name in ('else', 'endif') else None
        for group in groups:
            if group is not own:
                group[3][group[2]].append((number, line))
        if name in ('if', 'ifdef', 'ifndef'):
            groups.append([*selects(name, directive[2]), 0, ([], [])])
        elif name == 'else' and groups:
            groups[-1][2] = 1
        elif name == 'endif' and groups:
            switch, side, _, sides = groups.pop()
            if switch == 'NONMATCHING':
                matching, replacing = sides[side], sides[1 - side]
                guard = (closed, '\n'.join(line for _, line in replacing), '\n'.join(line for _, line in matching))
                guards.update(dict.fromkeys((number for number, _ in matching), guard))
                closed += 1
    return '\n'.join(kept), guards


def close(text, at):
    """The index past the parenthesis that closes the one at text[at]."""
    depth = 0
    for i in range(at, len(text)):
        if text[i] == '(':
            depth += 1
        elif text[i] == ')':
            depth -= 1
            if depth == 0:
                return i + 1
    return len(text)


def blank(text):
    """text with each literal's characters blanked, so its length stays."""
    return LITERAL.sub(lambda literal: ' ' * len(literal[0]), text)


def arguments(text, scanned):
    """The arguments of a call whose parentheses hold text, each with its
    spaces collapsed. scanned is text with its literals blanked, where the
    commas between arguments are found."""
    found, depth, start = [], 0, 0
    for i, char in enumerate(scanned):
        if char in '([{':
            depth += 1
        elif char in ')]}':
            depth -= 1
        elif char == ',' and depth == 0:
            found.append(text[start:i])
            start = i + 1
    found.append(text[start:])
    return [' '.join(argument.split()) for argument in found] if text.strip() else []


def functions(text):
    """Each function of text, C as matching_side gives it, as (name, start,
    end): the span of its body, from its opening brace to past its
    closing one."""
    found, depth, opened = [], 0, None
    for brace in re.finditer(r'[{}]', text):
        i = brace.start()
        if brace[0] == '}':
            depth -= 1
            if depth == 0 and opened:
                found.append((*opened, i + 1))
                opened = None
            continue
        if depth == 0:
            j = i - 1
            while j >= 0 and text[j].isspace():
                j -= 1
            if j >= 0 and text[j] == ')':
                inside = 0
                while j >= 0:
                    inside += {')': 1, '(': -1}.get(text[j], 0)
                    if inside == 0:
                        break
                    j -= 1
                name = re.search(r'([A-Za-z_]\w*)\s*$', text[max(0, j - 200):j])
                opened = (name[1] if name else '?', i)
        depth += 1
    return found


def calls(text, start=0, end=None):
    """Each outermost call of text[start:end], in order, as a Call. A call
    inside another's arguments is the other's, and one inside a literal is
    none."""
    end = len(text) if end is None else end
    scanned = blank(text[start:end])
    found, past = [], 0
    line, counted = text.count('\n', 0, start) + 1, 0
    for call in CALL.finditer(scanned):
        if call.start() < past or call[1] in KEYWORDS:
            continue
        line += scanned.count('\n', counted, call.start())
        counted = call.start()
        opening = call.end() - 1
        past = close(scanned, opening)
        inner = slice(start + opening + 1, start + past - 1)
        found.append(Call(call[1], arguments(text[inner], scanned[opening + 1:past - 1]), line, start + call.start(),
                          start + past))
    return found


def stirs(call):
    """Whether an argument of call calls a function or changes a variable,
    which a kernel's call may run in another order than the window's."""
    inside = blank(', '.join(call.args))
    return any(name not in KEYWORDS for name in CALL.findall(inside)) or bool(ASSIGNS.search(inside))


def null(call):
    return bool(call.args) and call.args[0] in NULL


def pushes(call):
    """Whether call pushes the matrix stack."""
    return call.name in ('njPushMatrixEx', 'njPushMatrix', 'matrix_push_unit')


def pushes_unit(call):
    """Whether call pushes the matrix stack and makes the unit matrix
    current."""
    return call.name == 'matrix_push_unit' or call.name == 'njPushMatrix' and call.args == [UNIT]


def counted(call):
    """Whether call pops no levels, or as many as a number says."""
    return call.name != 'njPopMatrix' or bool(call.args) and bool(INTEGER.fullmatch(call.args[0]))


def pops(call):
    """How many levels of the matrix stack call pops."""
    if call.name == 'njPopMatrixEx':
        return 1
    if call.name != 'njPopMatrix':
        return 0
    if not counted(call):
        return 1
    digits = INTEGER.fullmatch(call.args[0])[1]
    return int(digits, 16 if digits[1:2] in ('x', 'X') else 8 if digits[0] == '0' else 10)


def letters(call):
    """call's letters as a window's shape names its step, or 'call'."""
    if call.name in CURRENT:
        return CURRENT[call.name]
    return STEPS[call.name] if call.name in STEPS and null(call) else 'call'


def windows(path, text):
    """Each window of text, the C of the file at path, and each
    njUnitMatrix(NULL) that opens none and lies in none, as a Window that
    call opens, with no steps, that no pop closes."""
    text, guards = matching_side(text)
    scanned = blank(text)
    found = []
    for function, start, end in functions(scanned):
        body = calls(text, start, end)
        # the places in body of the calls a window holds
        held = set()
        for i, call in enumerate(body):
            unit = call.name == 'njUnitMatrix' and null(call)
            if pushes_unit(call):
                first = i + 1
            elif unit and i and pushes(body[i - 1]) and not pushes_unit(body[i - 1]):
                first, call = i + 1, body[i - 1]
            elif unit and i not in held:
                found.append(Window(path, function, call.line, str(call), closed=False))
                continue
            else:
                continue
            window = Window(path, function, call.line, str(call))
            if call.line in guards:
                window.marks.add('guarded')
                window.replaced = guards[call.line][1]
            depth, last = 1, None
            for place, inner in enumerate(body[first:], first):
                held.add(place)
                depth += pushes(inner) - pops(inner)
                if depth <= 0:
                    last = inner if depth == 0 else None
                    break
                window.steps.append((letters(inner), inner))
            if last is None:
                window.closed = False
                found.append(window)
                continue
            window.pop = last
            statement = re.compile(r'\s*;').match(scanned, last.end)
            if call.line in guards and statement:
                window.exact = ' '.join(guards[call.line][2].split()) == ' '.join(
                    text[call.start:statement.end()].split())
            inside = scanned[call.end:last.start]
            if any(letters == 'call' for letters, _ in window.steps):
                window.marks.add('calls')
            if BRANCH.search(inside):
                window.marks.add('branches')
            for inner in body[i:first]:
                inside = inside.replace(scanned[inner.start:inner.end], ' ', 1)
            for _, inner in window.steps:
                inside = inside.replace(scanned[inner.start:inner.end], ' ', 1)
            if re.sub(r'[\s;]', '', inside):
                window.marks.add('spread')
            found.append(window)
    return found


def first_floats(pointer):
    """The spellings of the address of the first float of the vector that
    pointer, C, points to, as the module's docstring gives them."""
    if pointer.startswith('&') and POSTFIX.fullmatch(pointer[1:]):
        return {f'{pointer}.x'}
    cast = CAST.fullmatch(pointer)
    if cast and POSTFIX.fullmatch(cast[1]):
        return {cast[1]}
    return {f'&{pointer}->x' if POSTFIX.fullmatch(pointer) else f'&({pointer})->x'}


def problems(window):
    """What breaks --check's rule in window, a guarded window, each as a
    line that names its place."""
    where = f'{window.path}:{window.line}: {window.function}'
    if not window.closed:
        return [f'{where}: a guard holds a window that no pop takes back to the depth it opened at']
    if not window.kernel or not window.taken:
        return [f'{where}: a guard holds a window whose shape and marks take no kernel: {window.shape or "(none)"}']
    if not window.exact:
        return [f'{where}: the guard\'s matching side must be the window, from its push to its pop, and no more']
    guessed = [call for call in (*(call for _, call in window.steps), window.pop) if not counted(call)]
    if guessed:
        return [f'{where}: {guessed[0]} pops levels that no number counts, so the window may close elsewhere']
    stirring = [call for _, call in window.steps if stirs(call)]
    if stirring:
        return [f'{where}: {stirring[0]} calls or changes something in its arguments, which its kernel may run '
                'in another order']
    name, takes = KERNELS[window.shape]
    made = calls(window.replaced)
    if len(made) != 1 or made[0].name != name or window.replaced[:made[0].start].strip() or not re.fullmatch(
            r'\s*;\s*', window.replaced[made[0].end:]):
        return [f'{where}: the guard\'s other side must be one call of {name}, and no more']
    # how many arguments the check reads of each step's call: the turn's
    # angle, the move's vector, or the read's two vectors
    reads = {'X': 2, 'Y': 2, 'Z': 2, 'T': 1, 'vector': 3, 'point': 3}
    short = [call for letter, call in window.steps if len(call.args) < reads[letter]]
    if short:
        return [f'{where}: {short[0]}, on line {short[0].line}, takes too few arguments for its step']
    given = {}
    for letter, call in window.steps:
        if letter in ('X', 'Y', 'Z'):
            given[letter] = {call.args[1]}
        elif letter == 'T':
            given['T'] = first_floats(call.args[-1]) if call.name != 'njTranslate' else set()
        else:
            given['in'], given['out'] = first_floats(call.args[1]), first_floats(call.args[2])
    found = []
    for i, (taken, argument) in enumerate(zip(takes, made[0].args)):
        if argument not in given[taken]:
            want = ' or '.join(sorted(given[taken])) or 'a vector, which njTranslate gives as three floats'
            found.append(f'{where}: {name}\'s argument {i + 1} is {argument}, where the window gives {want}')
    if len(made[0].args) != len(takes):
        found.append(f'{where}: {name} takes {len(takes)} arguments, and the guard gives {len(made[0].args)}')
    return found


def listing(window):
    """window's line of --list."""
    where = f'{window.path}:{window.line}: {window.function}'
    if not window.closed and window.opener.startswith('njUnitMatrix('):
        return f'{where}: {window.opener} opens no window'
    marks = ''.join(f' [{mark}]' for mark in MARKS if mark in window.marks)
    unclosed = ', which no pop closes' if not window.closed else ''
    steps = '; '.join(f'{letters} {call}' for letters, call in window.steps)
    return f'{where}: {window.opener}: {window.shape or "(none)"}{marks}{unclosed}: {steps}'


def summary(found):
    """The summary's lines: a row for each shape, the totals, and the
    sources that hold the most windows."""
    whole = [window for window in found if window.closed]
    shapes = defaultdict(list)
    for window in whole:
        shapes[window.shape or '(none)'].append(window)
    width = max([len(shape) for shape in shapes] + [len('shape')])
    lines = [f'{"shape":<{width}}  windows  sources  ' + '  '.join(MARKS) + '  kernel']
    for shape, held in sorted(shapes.items(), key=lambda row: (-len(row[1]), row[0])):
        counts = '  '.join(f'{sum(mark in window.marks for window in held):{len(mark)}}' for mark in MARKS)
        lines.append(f'{shape:<{width}}  {len(held):7}  {len({window.path for window in held}):7}  {counts}  '
                     f'{held[0].kernel or "-"}')
    taken = [window for window in whole if window.taken]
    kernels = sum(1 for window in whole if window.kernel)
    lines.append(f'{len(whole)} windows in {len({window.path for window in whole})} sources. {kernels} take a '
                 f'kernel\'s shape, {len(taken)} of them in one run of statements, and '
                 f'{sum("guarded" in window.marks for window in taken)} of those are guarded')
    stray = len(found) - len(whole)
    if stray:
        lines.append(f'{stray} more, which --list names: an njUnitMatrix(NULL) that opens no window, or a window '
                     'that no pop closes')
    lines.append('')
    lines.append('sources with the most windows')
    for path, count in Counter(window.path for window in whole).most_common(MOST):
        lines.append(f'{count:7}  {path}')
    return lines


def sources():
    """Each .c file of src/ and modules/, in path order. A working directory
    that lacks one of them fails."""
    tops = ('src/', 'modules/')
    at_root(*tops)
    return sorted(str(path) for top in tops for path in Path(top).rglob('*.c'))


def main(argv=None):
    parser = argparse.ArgumentParser(prog='windows', description=__doc__.split('\n\n')[0])
    parser.add_argument('files', metavar='FILE', nargs='*', help='a C source; by default those of src/ and modules/')
    shown = parser.add_mutually_exclusive_group()
    shown.add_argument('--list', action='store_true', help='print each window, not the summary')
    shown.add_argument('--check', action='store_true', help="check each guarded window's kernel call")
    args = parser.parse_args(argv)
    paths = args.files or sources()
    found = [window for path in paths for window in windows(path, content(path).decode('latin-1'))]
    if args.check:
        broken = [line for window in found if 'guarded' in window.marks for line in problems(window)]
        if broken:
            fail(*broken)
        return
    print('\n'.join([listing(window) for window in found] if args.list else summary(found)))


if __name__ == '__main__':
    main()
