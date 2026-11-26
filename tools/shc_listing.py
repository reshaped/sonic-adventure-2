"""Read a listing SHC or asmsh writes: where each datum lies in its object's
sections, the labels each function's code names, and each string.

SHC lists the object it compiles under -listfile and -show=object, and
asmsh the one it assembles under -list. tools/body_offsets.py reads a body source's
stub listing through these, for where each of its data lies.
"""

import re
from bisect import bisect_right
from collections import Counter, defaultdict
from pathlib import Path, PureWindowsPath

import sysrof

from config import content, fail


# the bytes in a listing directive's .B, .W or .L unit
UNITS = {'B': 1, 'W': 2, 'L': 4}
# the banner asmsh's listing opens with, after a form feed
ASSEMBLED = re.compile(r'\f?\*\*\* SuperH RISC engine ASSEMBLER')
# in a listing: a static's label, a datum's directive and its operands, the
# function a row of code begins, the labels a row names, and a row of an L
# label alone, as an unnamed datum or a string starts with. A .SDATA's
# operand is its string in quotes, which can hold a ;, where a comment
# follows the string or nothing does. Any other operands run to the ; that
# starts a comment, as do those of a .SDATA with other text past its string,
# which sdata() fails on
STATIC = re.compile(r'\w+: +; static: (\w+)')
QUOTED = re.compile(r'"(?:[^"]|"")*"')
DIRECTIVE = re.compile(r'(\.(?:DATA|DATAB|RES|SDATA)(?:\.[BWL])?) +(' + QUOTED.pattern + r'(?=\s*(?:;|$))|[^;]*)')
BEGINS = re.compile(r'; function: (\w+)')
ENTRY = re.compile(r'\w+:\s*; function: (\w+)\s*$')
LABELS = re.compile(r'\bL[0-9]+\b')
UNNAMED = re.compile(r'(L[0-9]+): *')
# a .DATAB's or .RES's count, and a byte of .DATA.B or .DATAB.B, as SHC and
# asmsh write them
COUNT = re.compile(r'[0-9]+')
BYTE = re.compile(r"H'([0-9A-F]{1,2})|([0-9]{1,3})")
# in SHC's listing: a section's size, a row's section, offset and the rest,
# a row that goes on with the operands above it, and the line marker
# -optimize=0 writes ahead of a section's first code, which gives the
# section with no offset
SHC_SECTION = re.compile(r'\w+ +SECTION \((\w+)\): +([0-9A-F]+) Byte')
SHC_ROW = re.compile(r'(...) ([0-9A-F]{8}) [0-9A-F<> ]{10} *(.*)$')
SHC_MORE = re.compile(r' {13}[0-9A-F<> ]{10} *\+ +([^;]*)')
SHC_MARKER = re.compile(r'(\w{1,3}) +; ')
# A long global label reaches the comment column. Its annotation follows on
# the next source line, sometimes across SHC's printed page header.
SHC_PAGE = (r'\n*SH SERIES C/C\+\+ Compiler \(Ver\. [^\n]+\) +'
            r'\d{2}-[A-Za-z]{3}-\d{4} \d{2}:\d{2}:\d{2} +PAGE +\d+\n+'
            r'SCT OFFSET +CODE +C LABEL +INSTRUCTION OPERAND +COMMENT\n+')
SHC_WRAPPED = re.compile(r'^(.* _\w+:)(\n(?:' + SHC_PAGE + r')?) +(; (?:static|function): \w+)$', re.M)


def join_annotation(match):
    """Move a global annotation up to its label, preserving source line numbers."""
    label, gap, comment = match.groups()
    if gap != '\n' and not label.endswith(' _' + comment.rsplit(' ', 1)[1] + ':'):
        return match[0]
    return label + ' ' + comment + '\n' * gap.count('\n')



def full_section(where, given, sizes, abbreviated=True):
    """The one section of a listing that a row gives: the one whose name
    starts with the row's three letters, or, where the row's name isn't
    abbreviated, the one of that name. Fails, naming where, the row's path
    and line, where no section or several do."""
    full = [name for name in sizes if (name[:3] if abbreviated else name) == given]
    if len(full) != 1:
        fail(f'{where}: no one section {"starts" if abbreviated else "is named"} {given}')
    return full[0]


def section_names(path, text, sizes, needed):
    """Resolve the abbreviated section names in needed against the same compilation's exported labels.

    SHC prints only three letters. Its AT library, for example, contains both
    CSG and CSGATLINE. A new section starts at zero; a label exported in the
    SYSROF object identifies its section independently of any offsets table.
    A section without exports can be the one remaining candidate. Only a
    needed prefix that several sections share reads the object.
    """
    groups = defaultdict(list)
    matches = defaultdict(set)
    nonempty = {name for name, size in sizes.items() if size}
    for name in nonempty:
        matches[name[:3]].add(name)
    ambiguous = {prefix for prefix, names in matches.items() if len(names) > 1 and prefix in needed}
    if ambiguous:
        exports = sysrof.exports(Path(path).with_suffix('.obj'))
        current = marked = None
        for number, line in enumerate(text.split('\n'), 1):
            row = SHC_ROW.match(line)
            if not row:
                marker = SHC_MARKER.match(line)
                marked = marker[1] if marker else marked
                continue
            prefix, marked = row[1].strip() or marked, None
            if prefix:
                current = None
                if prefix in ambiguous:
                    if int(row[2], 16):
                        fail(f'{path}:{number}: ambiguous {prefix} section resumes at {row[2]}')
                    current = set(matches[prefix])
                    groups[prefix].append((number, current))
            if current is not None:
                label = row[3].partition(':')[0]
                if label in exports:
                    section, offset = exports[label]
                    if offset != int(row[2], 16):
                        fail(f'{path}:{number}: {label} disagrees with its object offset')
                    current.intersection_update((section,))
        for prefix in sorted(ambiguous):
            spans = groups[prefix]
            if len(spans) != len(matches[prefix]):
                fail(f'{path}: cannot identify all {prefix} sections from the object')
            pending = [candidates for _, candidates in spans]
            while pending:
                known = [candidates for candidates in pending if len(candidates) == 1]
                resolved = set().union(*known)
                if len(resolved) != len(known) or not known:
                    fail(f'{path}: object exports do not distinguish the {prefix} sections')
                pending = [candidates for candidates in pending if len(candidates) != 1]
                for candidates in pending:
                    candidates.difference_update(resolved)

    def resolve(short, line):
        if short in ambiguous:
            spans = groups[short]
            index = bisect_right([start for start, _ in spans], line) - 1
            if index < 0:
                fail(f'{path}:{line}: {short} has no section start')
            return next(iter(spans[index][1]))
        return full_section(f'{path}:{line}', short, nonempty)

    return resolve


def sdata(operand, where):
    """The bytes of a .SDATA's operand, as DIRECTIVE reads it: the string in
    its quotes, each doubled quote one quote. listing() sizes a string by
    them, and strings() takes them. Fails, naming where, the row's path and
    line, on an operand that isn't one whole string."""
    if not QUOTED.fullmatch(operand.strip()):
        fail(f'{where}: .SDATA {operand.strip()}: no whole string')
    return operand.strip()[1:-1].replace('""', '"').encode('latin-1')


def unit(op, where):
    """The bytes in the unit op names, a .DATA, .DATAB or .RES as DIRECTIVE
    reads it. Fails, naming where, the row's path and line, on one that
    names no .B, .W or .L."""
    if op[-2:-1] != '.':
        fail(f'{where}: {op}: no .B, .W or .L unit')
    return UNITS[op[-1]]


def directive_count(operands, where):
    """The count a .DATAB's or .RES's operands start with. Fails, naming
    where, the row's path and line, on one that isn't COUNT's."""
    found = operands.split(',')[0].strip()
    if not COUNT.fullmatch(found):
        fail(f'{where}: {operands.strip()}: its count isn\'t a decimal number')
    return int(found)


def byte(value, where):
    """The byte a .DATA.B's or .DATAB.B's value gives. Fails, naming
    where, the row's path and line, on one that isn't BYTE's or is past
    255."""
    found = BYTE.fullmatch(value.strip())
    if found and found.group(1):
        return int(found.group(1), 16)
    if not found or int(found.group(2)) > 255:
        fail(f'{where}: {value.strip()}: no byte')
    return int(found.group(2))


def listing(path, callbacks=()):
    """From the listing at path, SHC's or asmsh's:

    - each named datum, as its name, section, offset and size, in the
      listing's order. Two functions' statics can share a name, and each
      has its own;
    - the first instruction of each requested native callback, in the same form;
    - each unnamed datum outside the code, as an automatic array's template
      is: its section, offset and size, by its L label;
    - the L labels named in each function's code and pools, by the function.

    SHC rows abbreviate section names to three letters; its section table
    gives the full names. asmsh's .SECTION directives give full names,
    which are kept. A listing is read one byte to a character.
    Both listings end each line in CRLF, which is read as LF, the line end
    SHC_WRAPPED matches, and only LF ends a line. A .SDATA's string keeps
    its semicolons and doubled quotes, and a comment starts only past it.

    Where a returned row's three letters start several sections' names, the
    object beside the listing, of its stem, tells them apart by its exports.

    Fails, naming the row's path and line, where the row names no one
    section, where a datum runs past its section, or where the count of a
    .DATAB or .RES that sizes a datum isn't a decimal number, or a directive
    whose unit sizes a datum names no .B, .W or .L. A .RES that pads a datum
    past its values sizes nothing, and its count goes unread. SHC's listing
    fails so too where a .SDATA's operand isn't one whole string. asmsh's
    cuts a long string short, so a .SDATA there is sized by its code,
    whatever its operand. A listing that gives no section's size fails,
    naming it."""
    text = content(path).decode('latin-1').replace('\r\n', '\n')
    abbreviated = not ASSEMBLED.match(text)
    data, unnamed, sizes, loads = (compiled if abbreviated else assembled)(text, path, callbacks)
    if not sizes:
        fail(f'{path}: gives no section\'s size, as SHC\'s and asmsh\'s listings do')

    resolve = section_names(path, text, sizes, {datum[0] for datum in data + unnamed}) if abbreviated else None

    def placed(section, name, start, end, line):
        full = resolve(section, line) if abbreviated else full_section(f'{path}:{line}', section, sizes, False)
        if end > sizes[full]:
            fail(f'{path}:{line}: {name} runs past its section, {full}, of {sizes[full]} bytes')
        return name, full, start, end - start

    named = [placed(*datum) for datum in data]
    return named, {label: tuple(place) for label, *place in (placed(*datum) for datum in unnamed)}, loads


def initializer_loads(path):
    """The anonymous templates loaded at each C source line in an SHC -show=source,object listing.

    A SuperH MOV.L @(disp,PC),Rn takes its address from an aligned constant-pool word.
    Follow the displacement encoded in the instruction to the .DATA.L label at that word.
    Source lines and instructions can cross a printed page. Sections that
    share three letters are told apart by the order their offset-zero rows
    start them in.
    """
    text = SHC_WRAPPED.sub(join_annotation, content(path).decode('latin-1').replace('\r\n', '\n'))
    source_line = re.compile(r'^ +([^\s]+\.[ch]) +(\d+) {4}.*$')
    pc_load = re.compile(r'^[A-Z0-9 ]{3} ([0-9A-F]{8}) (D[0-9A-F]{3}) +MOV\.L +'
                         r'L[0-9]+(?:\+[0-9]+)?,R(?:1[0-5]|[0-9]) *(?:; *(L[0-9]+))? *$')
    label_word = re.compile(r'\.DATA\.L +(L[0-9]+) *$')
    pooled, loads, starts = {}, [], Counter()
    section = function = source = marked = None
    for line in text.split('\n'):
        mark = source_line.match(line)
        if mark:
            # the path as SHC printed it, whichever host reads it
            source = (function, PureWindowsPath(mark[1]).name, int(mark[2]))
            continue
        row = SHC_ROW.match(line)
        if not row:
            marker = SHC_MARKER.match(line)
            marked = marker[1] if marker else marked
            continue
        given, marked = row[1].strip() or marked, None
        if given:
            if not int(row[2], 16):
                starts[given] += 1
            section = given, starts[given]
        if not section or not section[0].startswith('P'):
            continue
        rest = row[3]
        begins = BEGINS.search(rest)
        if begins:
            function, source = begins[1], None
        word = label_word.fullmatch(rest)
        if word:
            pooled[section, int(row[2], 16)] = word[1]
        load = pc_load.fullmatch(line)
        if source and load:
            at = int(load[1], 16)
            address = ((at + 4) & ~3) + 4 * (int(load[2], 16) & 0xff)
            loads.append((source, (section, address), load[3]))
    result = defaultdict(set)
    for source, key, annotation in loads:
        target = pooled.get(key)
        if target is not None and (annotation is None or annotation == target):
            result[source].add(target)
    return result


def compiled(text, path, callbacks=()):
    """listing()'s named data, unnamed data, section sizes and loads, from
    SHC's listing at path. A datum is its section's letters, name, start,
    end and line."""
    data, sizes, section, datum, directive = [], {}, None, None, None
    unnamed, loads, function = [], {}, None
    text = SHC_WRAPPED.sub(join_annotation, text)

    def close():
        """Extend the datum over the last directive, its continuation lines
        too."""
        if datum is None or directive is None:
            return
        offset, op, operands, where = directive
        if op.startswith('.RES') and datum[3] > datum[2]:
            # a .RES after the datum's values is SHC's padding before the next
            # datum
            return
        if op.startswith('.SDATA'):
            size = len(sdata(operands, where))
        elif op.startswith(('.DATAB', '.RES')):
            size = unit(op, where) * directive_count(operands, where)
        else:
            size = unit(op, where) * len([value for value in operands.split(',') if value.strip()])
        datum[3] = max(datum[3], offset + size)

    for number, line in enumerate(text.split('\n'), 1):
        # the section, the offset, the code and the rest, in their columns
        info = SHC_SECTION.match(line)
        row = SHC_ROW.match(line)
        more = SHC_MORE.match(line)
        marker = SHC_MARKER.match(line)
        if info:
            sizes[info.group(1)] = int(info.group(2), 16)
        elif more and directive is not None:
            directive = (directive[0], directive[1], directive[2] + ',' + more.group(1), directive[3])
        elif marker:
            close()
            directive, datum = None, None
            section = marker.group(1)
        elif row:
            close()
            directive = None
            if row.group(1).strip():
                # a row that gives its section starts or resumes it, and no datum runs on into it
                section, datum = row.group(1).strip(), None
            offset, rest = int(row.group(2), 16), row.group(3)
            label = STATIC.match(rest)
            alone = UNNAMED.fullmatch(rest)
            op = DIRECTIVE.match(rest)
            in_code = section is not None and section.startswith('P')
            begins = BEGINS.search(rest)
            if in_code:
                if begins:
                    function = begins.group(1)
                    if function in callbacks and ENTRY.fullmatch(rest):
                        data.append([section, function, offset, offset + 2, number])
                loads.setdefault(function, set()).update(LABELS.findall(rest))
            if label:
                datum = [section, label.group(1), offset, offset, number]
                data.append(datum)
            elif alone and not in_code:
                datum = [section, alone.group(1), offset, offset, number]
                unnamed.append(datum)
            elif op and datum is not None:
                directive = (offset, op.group(1), op.group(2), f'{path}:{number}')
            else:
                # code, alignment, or a label of no datum's: the datum has ended
                datum = None
    close()
    return data, unnamed, sizes, loads


def assembled(text, path, callbacks=()):
    """listing()'s named data, unnamed data, section sizes and loads, from
    asmsh's listing at path, as compiled() gives them, but with each
    section's full name.

    A row is its line number, the offset, the code, and the source's line
    number and text. A row of an offset and code alone goes on with the code
    above it. The listing cuts the source's long lines short, so a datum's
    values are measured by their code. .SECTION starts a section, and the
    section list at the end gives the sizes."""
    data, sizes, section, datum, in_values = [], {}, None, None, False
    unnamed, loads, function = [], {}, None
    for number, line in enumerate(text.split('\n'), 1):
        size = re.match(r'(\w+) +REL-(?:CODE|DATA) +([0-9A-F]+)', line)
        row = re.match(r' *\d+ ([0-9A-F]{8}) ([0-9A-F]*) +\d+ (.*)$', line)
        more = re.match(r' {6}([0-9A-F]{8}) ([0-9A-F]+) *$', line)
        if size:
            sizes[size.group(1)] = int(size.group(2), 16)
        elif more:
            if datum is not None and in_values:
                datum[3] = max(datum[3], int(more.group(1), 16) + len(more.group(2)) // 2)
        elif row:
            offset, code, rest = int(row.group(1), 16), row.group(2), row.group(3).strip()
            # asmsh marks source operand continuations with '+'. Long lines
            # can be truncated; the object column gives the actual byte count.
            if in_values and offset == datum[3] and re.match(r'\+\s', rest):
                datum[3] += len(code) // 2
                continue
            in_values = False
            begun = re.match(r'\.SECTION +(\w+)', rest)
            if begun:
                section, datum = begun.group(1), None
                continue
            label = STATIC.match(rest)
            alone = UNNAMED.fullmatch(rest)
            op = DIRECTIVE.match(rest)
            in_code = section is not None and section.startswith('P')
            begins = BEGINS.search(rest)
            if in_code:
                if begins:
                    function = begins.group(1)
                    if function in callbacks and ENTRY.fullmatch(rest):
                        data.append([section, function, offset, offset + 2, number])
                loads.setdefault(function, set()).update(LABELS.findall(rest))
            if label:
                datum = [section, label.group(1), offset, offset, number]
                data.append(datum)
            elif alone and not in_code:
                datum = [section, alone.group(1), offset, offset, number]
                unnamed.append(datum)
            elif op and datum is not None and op.group(1).startswith(('.DATAB', '.RES')):
                # a .RES after the datum's values is SHC's padding before the
                # next datum
                if not (op.group(1).startswith('.RES') and datum[3] > datum[2]):
                    where = f'{path}:{number}'
                    datum[3] = max(datum[3], offset + unit(op.group(1), where) * directive_count(op.group(2), where))
            elif op and datum is not None:
                datum[3] = max(datum[3], offset + len(code) // 2)
                in_values = True
            else:
                # code, alignment, or a label of no datum's: the datum has ended
                datum = None
    return data, unnamed, sizes, loads


def strings(path):
    """Each string SHC's listing at path holds, as its section, offset and
    bytes. A string is a row of an L label alone outside the code, and the
    characters the .SDATA, .DATA.B and .DATAB.B rows below it give, each
    from where the one before it ends. A label whose rows give no
    characters is an unnamed datum's, as an automatic array's template is,
    and gives no string. The listing is read as listing() reads it. asmsh's
    listing gives no strings.

    Fails, naming the row's path and line, where a row gives a string's
    bytes in a form SHC doesn't write, or from an offset other than where
    the string's bytes so far end, or where a string runs past its
    section."""
    text = content(path).decode('latin-1').replace('\r\n', '\n')
    lines = text.split('\n')
    sizes = {info.group(1): int(info.group(2), 16) for info in map(SHC_SECTION.match, lines) if info}
    found, section, directive, string = [], None, None, None

    def close():
        if directive is None:
            return
        offset, op, operands, where = directive
        if offset != string[1] + len(string[2]):
            fail(f'{where}: {op} starts at {offset:08X}, and its string\'s bytes end at '
                 f'{string[1] + len(string[2]):08X}')
        if op == '.SDATA':
            data = sdata(operands, where)
        elif op == '.DATA.B':
            values = operands.split(',')
            if not all(value.strip() for value in values):
                fail(f'{where}: .DATA.B {operands.strip()}: an empty byte')
            data = bytes(byte(value, where) for value in values)
        else:
            if operands.count(',') != 1:
                fail(f'{where}: .DATAB.B {operands.strip()}: not a count and a byte')
            count = directive_count(operands, where)
            # a run past the largest section of its letters fails before its bytes are made
            if offset + count > max((size for name, size in sizes.items() if name[:3] == string[0]), default=0):
                fail(f'{where}: .DATAB.B {operands.strip()}: runs past its section')
            data = bytes([byte(operands.split(',')[1], where)]) * count
        string[2].extend(data)

    for number, line in enumerate(lines, 1):
        row = SHC_ROW.match(line)
        more = SHC_MORE.match(line)
        marker = SHC_MARKER.match(line)
        if SHC_SECTION.match(line):
            continue
        if more and directive is not None:
            # the row above can end its operands in the comma between them and these
            first = directive[2].rstrip()
            operands = (first[:-1] if first.endswith(',') else first) + ',' + more.group(1)
            directive = (directive[0], directive[1], operands, directive[3])
        elif marker:
            close()
            directive, string = None, None
            section = marker.group(1)
        elif row:
            close()
            directive = None
            if row.group(1).strip():
                # a row that gives its section starts or resumes it, and no string runs on into it
                section, string = row.group(1).strip(), None
            offset = int(row.group(2), 16)
            op = DIRECTIVE.match(row.group(3))
            in_code = section is not None and section.startswith('P')
            if UNNAMED.fullmatch(row.group(3)) and not in_code:
                string = [section, offset, bytearray(), number]
                found.append(string)
            elif op and op.group(1) in ('.SDATA', '.DATA.B', '.DATAB.B') and string is not None:
                directive = (offset, *op.groups(), f'{path}:{number}')
            else:
                string = None
    close()
    resolve = section_names(path, text, sizes, {short for short, _, data, _ in found if data})
    placed = []
    for short, offset, data, number in found:
        if not data:
            continue
        section = resolve(short, number)
        if offset + len(data) > sizes[section]:
            fail(f'{path}:{number}: its string runs past its section, {section}, of {sizes[section]} bytes')
        placed.append((section, offset, bytes(data)))
    return placed
