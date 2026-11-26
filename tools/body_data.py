"""Materialize typed C initializers from a source's checked section offsets.

A datum row gives NAME SECTION OFFSET SIZE. A text row uses the same fields
for the C string macro and counts its terminating null. Both offsets are
measured from the start of the input section. A pointer row gives the
containing datum, its field's offset, the name the field points at and how
many bytes past that name's start. The generator spells the row as C from
the declarations in scope, and a pointer field that no row names is null.

A source spells the strings of a datum from the disc as numbered texts,
TEXT_1 on, which its text.inc defines. The datum's row is a datum row, as
a body's is (numbered_texts).

With --lint, it writes a source with its bodies in place instead, as
lint's include-cleaner reads it (with_bodies).
"""
import argparse
from bisect import bisect_right
from collections import Counter, defaultdict
from dataclasses import dataclass
from decimal import ROUND_CEILING, ROUND_FLOOR, ROUND_HALF_EVEN, Decimal
import itertools
from pathlib import Path
import re
import struct
import textwrap
from typing import NamedTuple

import body_bindings
from config import Image, NAME, SECTION, SOURCE, address, code, content, fail, hexa, rows, save, zeroed
from csource import INCLUDE_ROOTS, blank, header_names, included, parse, tokens
from decls import ANY_SUFFIX, declarations, entity
from kinds import DECLARED, KINDS, LISTS, NJD_CE, STRIP_UVS, VERTEX_WORDS, ChunkError, strips, vertex_range, walk
from layout import Array, FunctionPointer, LayoutError, Pointer, Record, Scalar

# SHC limits the length of each physical source line, including generated headers
LINE_WIDTH = 96


@dataclass(frozen=True)
class Datum:
    at: int
    size: int
    section: str
    text: bool


def locations(image, source, table):
    """The places that table, source's measured table, gives its data,
    texts and function entries in image, by name, as Datums. The run fails
    on a row that is malformed or repeats a name, and on a place that lies
    outside source's placement or overlaps another."""
    placed = image.ranges(str(source))
    data = {}
    for where, fields in rows(table):
        if len(fields) == 5 and fields[0] in ('datum', 'text', 'entry'):
            kind, name, section, offset, size = fields
            if (not NAME.fullmatch(name) or not SECTION.fullmatch(section) or code(section) != (kind == 'entry')
                    or not re.fullmatch(r'[1-9][0-9]{0,8}', size)):
                fail(f'{where}: invalid datum name, section or size')
            if kind == 'text' and (zeroed(section) or name != name.upper()):
                fail(f'{where}: a text needs an uppercase macro and initialized data')
            ranges = [(a, b) for sec, a, b in placed if sec == section]
            if not ranges:
                fail(f'{where}: {source} has no {section} placement')
            start = min(a for a, _ in ranges) + address(offset, where)
            if kind == 'entry' and (size != '2' or start % 2):
                fail(f'{where}: a function entry needs one aligned SH instruction')
            end = start + int(size)
            if not any(a <= start and end <= b for a, b in ranges):
                fail(f'{where}: {name} extends beyond its {section} placement')
            if name in data:
                fail(f'{where}: repeated datum {name}')
            data[name] = Datum(start, int(size), section, kind == 'text')
        else:
            fail(f'{where}: expected datum, text or entry NAME SECTION OFFSET SIZE')
    ordered = sorted(data.items(), key=lambda entry: entry[1].at)
    for (first, left), (second, right) in zip(ordered, ordered[1:]):
        if left.at + left.size > right.at:
            fail(f'{table}: {first} overlaps {second}')
    return data


def authored(provider, include):
    """Does the quoted include resolve to source-controlled data along the native search path?"""
    return any((root / include).is_file() for root in (provider.parent, *map(Path, INCLUDE_ROOTS)))


def generated(texts):
    """Yield each include the build generates for texts, each C file's text
    by its path: an include of a .inc under the file's own directory below
    its root, as sdk/sample/provider.c's sample/provider/tag.inc, that no
    file of the tree answers along the compiler's search path."""
    for provider, text in texts.items():
        directory = provider.with_suffix('').relative_to(provider.parts[0])
        for _, _, name in header_names(text):
            if Path(name).suffix == '.inc' and Path(name).parent == directory and not authored(provider, name):
                yield Path(name)


def bodies(source, texts, declarators):
    """Each of declarators whose initializer the build generates, in order:
    a body, or the numbered texts that numbered_texts() finds. texts and
    declarators are decls.declarations()' reading of source. A body's
    include names its declarator and .inc, under the directory of the
    declarator's file. A header's body takes internal linkage, so that each
    source that includes the header has its own. Every include the build
    generates for texts is a body or a provider's text.inc. The run fails on
    a body or an include that breaks one of these."""
    found = set(numbered_texts(source, texts, declarators))
    for declarator in declarators:
        if declarator.body is None or authored(declarator.file, declarator.body):
            continue
        provider, name = declarator.file, declarator.name
        if provider.suffix == '.h' and declarator.storage != 'static':
            fail(f'{provider}: a generated initializer in a header must have internal linkage')
        if declarator.body != provider.with_suffix('').relative_to(provider.parts[0]) / (name + '.inc'):
            fail(f'{provider}: invalid body include {declarator.body} for {name}')
        found.add(declarator)
    for path in sorted(set(generated(texts)) - text_headers(texts).keys() - {each.body for each in found}):
        fail(f'{source}: {path} is the initializer of no declaration SHC compiles')
    return [each for each in declarators if each in found]


# a numbered text, which an initializer names in place of a string from the disc
NUMBERED = re.compile(r'TEXT_[1-9][0-9]*')


def numbered_texts(source, texts, declarators):
    """The numbered texts that each datum of source's own file takes, in
    order, by the datum's declarator. texts and declarators are
    decls.declarations()' reading of source. A numbered text is a macro of
    source's text.inc: TEXT_1, TEXT_2 and on. source's code names each once,
    in that order. The initialized datum at file scope whose declaration
    spans the name's line takes it. The run fails on a numbered text out of
    order, and on one whose line no such declaration spans, or two do."""
    owners = [each for each in declarators
              if each.file == source and each.function is None and each.initialized and each.body is None]
    found = defaultdict(list)
    spelled = [(word, line) for word, line in tokens(texts[source], directives=False) if NUMBERED.fullmatch(word)]
    for number, (word, line) in enumerate(spelled, 1):
        if word != f'TEXT_{number}':
            fail(f'{source}:{line}: {word} comes where TEXT_{number} should')
        held = [each for each in owners if each.line <= line <= each.last]
        if not held:
            fail(f'{source}:{line}: {word} lies in the declaration of no initialized datum at file scope')
        if len(held) > 1:
            fail(f'{source}:{line}: {word} lies on a line that ' + ' and '.join(each.name for each in held) + ' share')
        found[held[0]].append(word)
    return found


def automatic(definitions):
    """Each of definitions, as bodies() gives them, that a function declares
    with automatic storage, by its name. A static in a block has a named
    place of its own."""
    return {each.name: each for each in definitions if each.function is not None and each.storage != 'static'}


def declares_automatic(source, texts):
    """Whether a function of source, whose texts source_inputs() gives,
    declares a body with automatic storage. It reads source's declarations
    only where a file's own text holds a generated include inside a
    function's braces, as each such body's file in the tree does
    (tests/test_csource.py)."""
    for provider, text in texts.items():
        paths = set(generated({provider: text}))
        starts = [directive.start for directive, _, name in header_names(text) if Path(name) in paths]
        if starts and any(block.opening.start < start < block.closing.start for block in parse(text).blocks
                          if block.name and block.closing for start in starts):
            _, _, declarators = declarations(source)
            return bool(automatic(bodies(source, texts, declarators)))
    return False


def typed_bodies(source, types, definitions):
    """Each of definitions, as bodies() gives them, by its name, as the C
    layout of its declaration and its body's include, or None for numbered
    texts."""
    found = {}
    for declarator in definitions:
        name = declarator.name
        if name in found:
            fail(f'{source}: repeated body include for {name}')
        try:
            layout = types.layout(declarator.term)
        except LayoutError as error:
            fail(f'{source}: {name}: {error}')
        found[name] = (layout, declarator.body)
    return found


def declared_kinds(types, term, declared):
    """The offset and kind of each subobject of an object of type term whose
    type has a kind. declared gives the kinds, by their types' resolved
    terms. A subobject is an element of an array or a member of a struct,
    or of a union its first member, the one an initializer sets."""
    term = types.resolve(term)
    if term in declared:
        return ((0, declared[term]),)
    if term[0] == 'array':
        inner = declared_kinds(types, term[1], declared)
        size = types.layout(term[1]).size
        return tuple((index * size + at, kind) for index in range(types.layout(term).count) for at, kind in inner)
    if term[0] not in ('struct', 'union'):
        return ()
    found = []
    for (_, member), (_, _, offset) in zip(types.members(*term), types.layout(term).fields):
        found.extend((offset + at, kind) for at, kind in declared_kinds(types, member, declared))
        if term[0] == 'union':
            break
    return tuple(found)


def list_kinds(source, types, definitions, layouts, pointers):
    """The kind of each of layouts' flat arrays that a chunk list field of
    a body reaches, by its name. A record's declared type gives its kind,
    and that kind's fields give the kinds they reach. A field reaches the
    array where its row in pointers points at the array's start. The run
    fails on an array that two kinds reach, and on one whose elements aren't
    its kind's."""
    declared = {}
    for ctype, kind in DECLARED.items():
        try:
            declared[types.resolve(types.term(ctype))] = kind
        except LayoutError:
            continue  # a type that no header of the source declares
    found = {}
    for each in definitions:
        for at, kind in declared_kinds(types, each.term, declared):
            for field in KINDS[kind].pointers:
                target, addend = pointers.get((each.name, at + field.offset), (None, None))
                if field.target not in LISTS or target not in layouts or addend:
                    continue
                layout = layouts[target][0]
                if not isinstance(layout, Array) or not isinstance(layout.element, Scalar):
                    continue  # a record that holds the list's parts
                if found.setdefault(target, field.target) != field.target:
                    fail(f'{source}: {target} is reached as {found[target]} and as {field.target}')
                element = KINDS[field.target]
                if layout.element.code not in 'hHiI' or layout.element.size != element.size:
                    fail(f'{source}: {target}, a {field.target} list, needs {element.ctype} elements')
    return found


def chunk_lists(source):
    """The kind and size of each of source's bodies that is a flat chunk
    list, by its name, as list_kinds() finds them."""
    texts, types, declarators = declarations(source)
    definitions = bodies(source, texts, declarators)
    layouts = typed_bodies(source, types, definitions)
    _, pointers, _, _ = body_bindings.read_all(source)
    return {name: (kind, layouts[name][0].size)
            for name, kind in list_kinds(source, types, definitions, layouts, pointers).items()}


def polygon_rows(halves, chunks):
    """The rows of a polygon list, as (first, end) of halves. A strip chunk
    of a type in STRIP_UVS takes a row for its head, size and strip count, a
    row for each strip, and a row for any halfwords of its own after them.
    Any other chunk is a row."""
    for chunk in chunks:
        end = chunk.offset + chunk.extent
        if chunk.type not in STRIP_UVS:
            yield chunk.offset, end
            continue
        at = chunk.offset + 3
        yield chunk.offset, at
        for strip in strips(halves, chunk):
            at = strip.offset + strip.extent
            yield strip.offset, at
        if at < end:
            yield at, end


def vertex_rows(words, chunks):
    """The rows of a vertex list, as (first, end) of words: a vertex chunk's
    head and index words, then each of its vertices, and the end. Raises
    ChunkError as kinds.vertex_range() does."""
    for chunk in chunks:
        if chunk.type == NJD_CE:
            yield chunk.offset, chunk.offset + 1
            continue
        count, width = len(vertex_range(words, chunk)), VERTEX_WORDS[chunk.type]
        yield chunk.offset, chunk.offset + 2
        yield from ((at, at + width) for at in range(chunk.offset + 2, chunk.offset + 2 + count * width, width))


# how each kind of chunk list is written, a row at a time
LIST_ROWS = {'chunk_polygons': polygon_rows, 'chunk_vertices': vertex_rows}


def list_body(values, rows):
    """The C of a flat chunk list: values, its elements' C, a row of rows
    to a line. A row longer than the line goes on, indented four more."""
    lines = [', '.join(values[first:end]) + (',' if end < len(values) else '') for first, end in rows]
    return '\n'.join(textwrap.fill(line, width=LINE_WIDTH, initial_indent='    ', subsequent_indent='        ',
                                   break_long_words=False, break_on_hyphens=False) for line in lines)


class SignedStop(NamedTuple):
    extent: int
    stride: int
    offset: int
    marker: bytes

    def check(self, data, where):
        """Only the last record of the original array may end the scan."""
        if len(data) != self.extent:
            fail(f'{where}: stop table extent disagrees with its C type')
        last = self.extent - self.stride + self.offset
        for at in range(self.offset, self.extent, self.stride):
            marked = data[at:at + len(self.marker)] == self.marker
            if marked != (at == last):
                fail(f'{where}: expected the signed stop only in the final record')


def signed_stops(source, types, definitions, bindings):
    """Resolve each selected record field and stop constant from its C declaration."""
    result, names = {}, {}
    for declarator in definitions:
        if declarator.name in bindings:
            if declarator.name in names:
                fail(f'{source}: repeated stop table declaration for {declarator.name}')
            names[declarator.name] = declarator.term
    for name, (field, constant) in bindings.items():
        where = f'{source}: stop {name}' + (f'.{field}' if field else '')
        if name not in names:
            fail(f'{where}: no initialized C body')
        try:
            table = types.resolve(names[name])
            element = types.resolve(table[1]) if table[0] == 'array' else ()
            if not element:
                fail(f'{where}: expected an array')
            array = types.layout(table)
            if not array.count:
                fail(f'{where}: empty stop table')
            member, offset = table[1], 0
            if field is not None:
                if element[0] != 'struct' or not NAME.fullmatch(element[1]):
                    fail(f'{where}: expected an array of named structs')
                member = dict(types.members(*element)).get(field)
                if member is None:
                    fail(f'{where}: no such struct member')
                offset = next(at for key, _, at in array.element.fields if key == field)
            scalar = types.layout(member)
            if types.resolve(member)[0] != 'base' or not isinstance(scalar, Scalar) or scalar.code not in 'bhi':
                fail(f'{where}: expected a signed integer member')
            value = types.evaluate([constant])
            if not -(1 << (8 * scalar.size - 1)) <= value < 0:
                fail(f'{where}: {constant} must be negative and fit the signed field')
        except LayoutError as error:
            fail(f'{where}: {error}')
        result[name] = SignedStop(array.size, array.element.size, offset,
                                  value.to_bytes(scalar.size, 'little', signed=True))
    return result


def subobject(layout, path, where):
    """The layout of the subobject that path, as [1].mesh, leads to in an
    object laid out as layout, and its offset from the object's start, as
    (layout, offset). An array that gives no extent holds any index. The
    run fails on a path that layout doesn't hold, and where names that path
    in the message."""
    offset = 0
    for index, member in body_bindings.FIELD.findall(path):
        if index:
            if not isinstance(layout, Array) or layout.count is not None and int(index) >= layout.count:
                fail(f'{where} indexes outside its declared array')
            layout = layout.element
            offset += int(index) * layout.size
        else:
            fields = {key: (inner, at) for key, inner, at in layout.fields} if isinstance(layout, Record) else {}
            if member not in fields:
                fail(f'{where} has no declared member {member}')
            layout, at = fields[member]
            offset += at
    return layout, offset


def integer_selectors(source, types, layouts, bindings):
    """Locate the declared integer fields that keep the source's control flow intact."""
    result = {}
    for target, constant in bindings.items():
        name, path = body_bindings.SUBOBJECT.fullmatch(target).groups()
        where = f'{source}: selector {target}'
        if name not in layouts:
            fail(f'{where}: missing initialized C body')
        field, offset = subobject(layouts[name][0], path, where)
        if not isinstance(field, Scalar) or field.code not in 'bBhHiI':
            fail(f'{where}: expected an integer member')
        try:
            value = types.evaluate([constant])
            encoded = struct.pack('<' + field.code, value)
        except (LayoutError, struct.error) as error:
            fail(f'{where}: {error}')
        result.setdefault(name, []).append((offset, encoded, target))
    return result


def apply_selectors(data, selectors, where, *, original=None):
    """Verify every selected field against optical input; retain its value in the stub."""
    for offset, encoded, field in selectors:
        end = offset + len(encoded)
        if end > len(data):
            fail(f'{where}: selector {field} outside datum')
        if original is not None and original[offset:end] != encoded:
            fail(f'{where}: original selector {field} differs from its declared value')
        data[offset:end] = encoded


# the bits of a binary32's sign and of its exponent and fraction
SIGN = 0x80000000
MAGNITUDE = 0x7fffffff
# the magnitudes of the least normal binary32 and of the largest finite one
LEAST_NORMAL = 0x00800000
LARGEST = 0x7f7fffff
# the decimal exponents of a float literal in fixed notation, %g's rule at
# nine digits: from 0.0001 to below 1e9. Any other literal takes an exponent
FIXED = range(-4, 9)


def roundings(value):
    """Yield the Decimal value's roundings to one significant digit, then to
    two, and so on. At each count the rounding to nearest, ties to even,
    comes first, and then the rounding the other way."""
    for digits in itertools.count(1):
        step = Decimal(1).scaleb(value.adjusted() - digits + 1)
        nearer = value.quantize(step, ROUND_HALF_EVEN)
        yield nearer
        yield value.quantize(step, ROUND_FLOOR if nearer > value else ROUND_CEILING)


def float_literal(word):
    """The C literal of word, a zero or a normal binary32. It has the fewest
    significant digits of any literal that reads as word under
    round-to-nearest, however the reader breaks a tie. Of the literals that
    short it is the one nearest word's value, and of two as near, the one
    whose last digit is even. A zero keeps its sign."""
    sign = '-' if word & SIGN else ''
    magnitude = word & MAGNITUDE
    if not magnitude:
        return sign + '0.0F'
    below, value, above = struct.unpack('<3f', struct.pack('<3I', magnitude - 1, magnitude, magnitude + 1))
    # Round-to-nearest reads a decimal as the word between the halfway points
    # to the floats on either side. Past the largest float it reads as if
    # 2**128 were the next float. A decimal on a halfway point reads as
    # either float by the reader's rule for a tie, so the literal lies
    # strictly between them. Neighbouring floats' halfway point needs at
    # most 25 significant bits, so a double holds it exactly.
    low = Decimal.from_float((below + value) / 2)
    high = Decimal.from_float((value + (above if magnitude < LARGEST else 2.0 ** 128)) / 2)
    # At each power of two but the least normal, the float below lies nearer
    # than the float above. There the rounding the other way can read as the
    # word though the rounding to nearest doesn't.
    literal = next(rounding for rounding in roundings(Decimal.from_float(value)) if low < rounding < high).normalize()
    text = format(literal, 'f' if literal.adjusted() in FIXED else 'e')
    return sign + text + ('.0' if text.isdigit() else '') + 'F'


def initializer(layout, data, at, pointer, outer=True):
    """Return C89 fields. Only a declared pointer can consume a relocation."""
    if isinstance(layout, Pointer):
        return pointer(at, int.from_bytes(data[at:at + 4], 'little'), layout)
    if isinstance(layout, Scalar):
        if layout.code == 'f':
            bits, = struct.unpack_from('<I', data, at)
            magnitude = bits & MAGNITUDE
            if 0 < magnitude < LEAST_NORMAL or magnitude > LARGEST:
                fail(f'binary32 {hexa(bits)} at offset {hexa(at)} requires a bit-preserving data object')
            return float_literal(bits)
        value = struct.unpack_from('<' + ('B' if layout.code == 'c' else layout.code), data, at)[0]
        if value == -0x80000000:
            return '(-2147483647L - 1L)'
        return str(value) + ('U' if value > 0x7fffffff else '')
    if isinstance(layout, Array):
        members = [(layout.element, i * layout.element.size) for i in range(layout.count)]
    elif isinstance(layout, Record):
        if any(any(data[at + start:at + start + length]) for start, length in layout.holes):
            fail(f'nonzero padding in datum at offset {hexa(at)}')
        fields = layout.fields[:1] if layout.union else layout.fields
        if layout.union:
            # R13 rejects extra braces around NJS_COLOR's scalar and service_box.detail's
            # frame. Keep the first struct's own braces when the union is nested.
            first = fields[0][1]
            if isinstance(first, Scalar):
                return initializer(first, data, at, pointer)
            if isinstance(first, Array):
                # SHC takes one brace group around the first array of a nested union.
                return initializer(first, data, at, pointer, outer)
            if isinstance(first, Record) and not first.union:
                # A zero frame is one zero initializer. GNU SH-4 can then fold
                # copies of service_box without emitting a second constant object.
                if (all(isinstance(field, Scalar) for _, field, _ in first.fields)
                        and not any(data[at:at + first.size])):
                    return '{0}' if outer else '{{0}}'
                return initializer(first, data, at, pointer, outer)
        members = [(member, offset) for _, member, offset in fields]
    else:
        fail(f'unsupported C layout {layout!r}')
    values = ', '.join(initializer(member, data, at + offset, pointer, False) for member, offset in members)
    return values if outer else '{' + values + '}'


def integer_fields(layout, at=0):
    """Yield the offset of each 32-bit integer field that initializer()
    writes of layout, from offset at: of a union, its first member's
    alone."""
    if isinstance(layout, Scalar):
        if layout.code in 'iI':
            yield at
    elif isinstance(layout, Array):
        for index in range(layout.count):
            yield from integer_fields(layout.element, at + index * layout.element.size)
    elif isinstance(layout, Record):
        for _, member, offset in layout.fields[:1] if layout.union else layout.fields:
            yield from integer_fields(member, at + offset)


def quoted(data):
    """A C89 string, including a fixed-width escape where a byte could join its neighbours."""
    escaped = {9: r'\t', 10: r'\n', 13: r'\r', 34: r'\"', 63: r'\?', 92: r'\\'}
    return '"' + ''.join(escaped.get(byte, chr(byte) if 32 <= byte < 127 else f'\\{byte:03o}')
                         for byte in data) + '"'


def stub_text(size, index):
    """Give each literal of one width a distinct value to preserve the original count after string pooling."""
    value = bytearray(b'!' * (size - 1))
    for pos in range(len(value)):
        index, digit = divmod(index, 255)
        value[pos] = (digit + 32) % 255 + 1
        if not index:
            break
    if index:
        fail(f'too many distinct texts of length {size}')
    return value


def text_headers(texts):
    """Each C provider's own text.inc, which it includes and the build
    generates, with the provider's text, by the include's name."""
    headers = {}
    for provider, text in texts.items():
        if provider.suffix != '.c':
            continue
        path = provider.with_suffix('').relative_to(provider.parts[0]) / 'text.inc'
        count = sum(Path(name) == path for _, _, name in header_names(text))
        if count > 1:
            fail(f'{provider}: repeated include of {path}')
        if count and not authored(provider, path):
            headers[path] = text
    return headers


def text_values(layout, data, names, where):
    """The value of each of names, the numbered texts that initialize a
    datum of layout, as bytes. data is the datum's bytes, or None for the
    build without the disc, where each value is empty. A char array takes
    every text in turn. Each but the last spells its string, its null and
    the zeros that follow them, up to the next string. The last spells its
    string alone, since C fills the rest of the array with zeros. A struct
    of char arrays takes a text for each member, which spells the member's
    string alone. The run fails on any other layout, and on a struct whose
    members and texts differ in count. Given data, it also fails on an array
    whose strings run out before its texts, and on an empty or unterminated
    string. It fails as well on a byte past an array's last string that
    isn't zero."""
    if isinstance(layout, Array) and layout.element == Scalar('c'):
        arrays = [(0, layout.size, names)]
    elif (isinstance(layout, Record) and not layout.union and len(layout.fields) == len(names)
          and all(isinstance(member, Array) and member.element == Scalar('c') for _, member, _ in layout.fields)):
        arrays = [(offset, member.size, [name]) for (_, member, offset), name in zip(layout.fields, names)]
    else:
        fail(f'{where}: numbered texts initialize a char array, or a struct of char arrays one each')
    if data is None:
        return [b''] * len(names)
    values = []
    for offset, size, held in arrays:
        at, end = offset, offset + size
        for index, name in enumerate(held, 1):
            if at == end:
                fail(f'{where}: no string is left in the array for {name}')
            null = data.find(0, at, end)
            if null <= at:
                fail(f'{where}: no nonempty, null-terminated string at offset {hexa(at)} for {name}')
            if index < len(held):
                following = end - len(data[null:end].lstrip(b'\0'))
                values.append(data[at:following])
                at = following
            elif any(data[null:end]):
                fail(f'{where}: a byte past {name}, the last text of its array, isn\'t zero')
            else:
                values.append(data[at:null])
    return values


def string_macros(source, descriptors, texts, layouts, pointers, image, original, numbered=None):
    """Read the literal macros used by each provider, preserving pooling in
    the stub build. numbered holds the value of each numbered text by its
    name, which the source's own text.inc defines after its text rows'
    macros. The run fails on a name that both define."""
    names = {name: datum for name, datum in descriptors.items() if datum.text}
    numbered = numbered or {}
    for name in sorted(names.keys() & numbered.keys()):
        fail(f'{source}: {name} is both a text row and a numbered text')
    initializers = {path for _, path in layouts.values()}
    providers = {path: text for path, text in text_headers(texts).items() if path not in initializers}
    main_header = source.with_suffix('').relative_to(source.parts[0]) / 'text.inc'
    if numbered and main_header not in providers:
        fail(f'{source}: its numbered texts need its include of {main_header}')
    if bool(names or numbered) != bool(providers):
        fail(f'{source}: text rows and the include of text.inc disagree')
    identifiers = {path: set(re.findall(r'\b[A-Za-z_]\w*\b', blank(text, ('comment', 'string', 'character'))))
                   for path, text in providers.items()}
    # A generated pointer can be the only C use of its pooled string. A
    # shared initializer inherits the translation unit's text header unless
    # its own source includes one.
    for (owner, _), (target, _) in pointers.items():
        # a datum of numbered texts holds no pointer, and generate() fails the row it leaves unconsumed
        if target not in names or owner not in layouts or layouts[owner][1] is None:
            continue
        provider_header = layouts[owner][1].with_name('text.inc')
        header = provider_header if provider_header in providers else main_header
        if header not in providers and len(providers) == 1:
            # A wrapper may compile a C provider that owns the sole text header.
            header = next(iter(providers))
        if header not in providers:
            fail(f'{source}: {owner} needs a text header for {target}')
        identifiers[header].add(target)
    counts = {}
    lines = {path: [] for path in providers}
    for name, datum in names.items():
        matches = [path for path, uses in identifiers.items() if name in uses]
        if not matches:
            fail(f'{source}: no use of text macro {name}')
        if len(matches) > 1:
            fail(f'{source}: multiple text headers use macro {name}')
        if original is None:
            index = counts.get(datum.size, 0)
            value = stub_text(datum.size, index)
            counts[datum.size] = index + 1
        else:
            start = datum.at - image.base
            raw = original[start:start + datum.size]
            if len(raw) != datum.size or not raw.endswith(b'\0') or b'\0' in raw[:-1]:
                fail(f'{source}: {name} differs from its recorded string length')
            value = raw[:-1]
        lines[matches[0]].append(f'#define {name} {quoted(value)}\n')
    if numbered:
        lines[main_header].extend(f'#define {name} {quoted(value)}\n' for name, value in numbered.items())
    for path, macros in lines.items():
        if not macros:
            fail(f'{source}: text rows and the include of {path} disagree')
    return {path: ''.join(macros) for path, macros in lines.items()}


def declared_base(types, term):
    """A declared object's type, term, with its qualifiers and typedefs
    resolved, and the qualifiers they gave, as (term, qualifiers). Raises
    LayoutError where Types.resolve() does."""
    base, qualifiers = types.resolve(term), set()
    while term != base:
        if term[0] == 'qualified':
            qualifiers.update(term[2])
            term = term[1]
        else:
            term, = types.typedefs[term[1]]
    return base, qualifiers


# the kinds of base type, in the order a type's words decide its kind. Words
# that name none of them are an int
BASE_KINDS = ('char', 'short', 'float', 'double', 'void', 'long')
# the kinds that C's default argument promotions change, to int or double
PROMOTED = ('char', 'short', 'float')
# the forms of void and of plain char, as shape() gives them
VOID = ('base', '', 'void')
CHAR = ('base', '', 'char')


def base_form(words):
    """The form of the base type that words name, in any order or spelling,
    as ('base', sign, kind). long double and long long are kinds of their
    own. Of the types a sign tells apart, only char has a plain type besides
    its signed one, so sign is 'signed' for signed char alone."""
    count = Counter(words)
    kind = next((each for each in BASE_KINDS if each in count), 'int')
    if kind == 'double' and count['long'] or count['long'] > 1:
        kind = 'long ' + kind
    sign = 'unsigned' if 'unsigned' in count else 'signed' if kind == 'char' and 'signed' in count else ''
    return 'base', sign, kind


def shape(types, term, held=frozenset()):
    """term as C compares types, with held's qualifiers added, as
    (qualifiers, form). Its typedefs are followed. An array's qualifiers are
    its element's, as C holds them, so an array's own are empty. form is
    one of:

    - base_form()'s form;
    - (kind, tag) for a struct, union or enum;
    - ('pointer', shape);
    - ('array', shape, extent), with the extent's tokens, or None for none;
    - ('function', shape, parameters), by its result's shape, and
      parameter()'s form of each type its list gives, with '...' as it is,
      or None for a list that gives no types.

    Raises LayoutError where Types can't resolve a typedef."""
    term, qualifiers = declared_base(types, term)
    held = frozenset(held | qualifiers)
    if term[0] == 'array':
        return frozenset(), ('array', shape(types, term[1], held), term[2])
    if term[0] == 'function':
        listed = term[2] if term[2] is None else tuple(each if each == '...' else parameter(types, each)
                                                       for each in term[2])
        return frozenset(), ('function', shape(types, term[1]), listed)
    if term[0] == 'pointer':
        return held, ('pointer', shape(types, term[1]))
    return held, base_form(term[1]) if term[0] == 'base' else term


def parameter(types, term):
    """The form of a parameter of type term, as C compares functions: an
    array is a pointer to its element, a function is a pointer to it, and
    the parameter's own qualifiers are dropped."""
    _, form = shape(types, term)
    if form[0] == 'array':
        return 'pointer', form[1]
    if form[0] == 'function':
        return 'pointer', (frozenset(), form)
    return form


def same(types, form, other):
    """Whether two forms are one type, as C compares them. An array without
    an extent matches one of any extent. A function whose list gives no
    types matches one whose parameters C's default argument promotions
    leave as they are, with no '...'. Raises LayoutError for an extent Types
    can't evaluate, where two extents are spelled apart."""
    if form[0] != other[0] or form[0] not in ('pointer', 'function', 'array'):
        return form == other
    if (form[0] == 'array' and None not in (form[2], other[2]) and form[2] != other[2]
            and types.evaluate(form[2]) != types.evaluate(other[2])):
        return False
    if form[0] == 'function' and not same_parameters(types, form[2], other[2]):
        return False
    (qualifiers, inner), (other_qualifiers, other_inner) = form[1], other[1]
    return qualifiers == other_qualifiers and same(types, inner, other_inner)


def same_parameters(types, listed, other):
    """Whether two functions' parameters, as shape() gives them, agree as C
    compares them."""
    if listed is None or other is None:
        given = other if listed is None else listed
        return given is None or '...' not in given and all(form[0] != 'base' or form[2] not in PROMOTED
                                                           for form in given)
    return len(listed) == len(other) and all(
        each == another if '...' in (each, another) else same(types, each, another)
        for each, another in zip(listed, other))


def unqualified(typed):
    """The qualifiers of typed, a shape, and its form without them, as
    (qualifiers, form). An array's qualifiers are its element's, which its
    form then leaves out."""
    held, form = typed
    if form[0] != 'array':
        return held, form
    held, element = unqualified(form[1])
    return held, ('array', (frozenset(), element), form[2])


def converts(types, target, pointee):
    """Whether a pointer to an object of shape target initializes a pointer
    to shape pointee with no cast: pointee keeps each of target's
    qualifiers, and is void or target's type."""
    (held, form), (kept, wanted) = unqualified(target), unqualified(pointee)
    return held <= kept and (wanted == VOID or same(types, form, wanted))


def extentless(types, term):
    """Whether term is an array type whose declaration gives no extent."""
    term, _ = declared_base(types, term)
    return term[0] == 'array' and term[2] is None


def subobjects(types, term, addend):
    """Yield each subobject of an object of type term that starts at byte
    addend, as (path, shape), outermost first: the object itself where
    addend is 0, then, a level at a time, each member or element that holds
    the byte and starts at it. Each of a union's members holds its bytes, in
    order. path spells the subobject from the object, as [1].mesh. An array
    without an extent holds every byte past its start. A level is laid out
    only once the caller asks past the level above it. Raises LayoutError
    where the byte lies outside the object, and, naming what holds the
    byte, where no subobject starts there."""
    if addend < 0 or addend and not extentless(types, term) and addend >= types.layout(term).size:
        raise LayoutError('the byte lies outside it')
    found, level = False, [('', term, frozenset(), 0)]
    while level:
        for path, term, held, start in level:
            if start == addend:
                found = True
                yield path, shape(types, term, held)
        below = []
        for path, term, held, start in level:
            term, qualifiers = declared_base(types, term)
            held |= qualifiers
            if term[0] == 'array':
                size = types.layout(term[1]).size
                index = (addend - start) // size
                below.append((f'{path}[{index}]', term[1], held, start + index * size))
            elif term[0] in ('struct', 'union'):
                for (member, inner), (_, layout, offset) in zip(types.members(*term), types.layout(term).fields):
                    if start + offset <= addend < start + offset + layout.size:
                        below.append((f'{path}.{member}', inner, held, start + offset))
        if not found and not below:
            path, term, _, _ = level[0]
            where = 'in padding of' if declared_base(types, term)[0][0] in ('struct', 'union') else 'inside'
            raise LayoutError(f'no member or element starts there: the byte lies {where} {path or "the object"}')
        level = below


def pointer_field(types, term, at):
    """The pointer that initializer() writes at byte at of an object of
    type term, as (path, term). path spells it from the object, as
    [1].step. Of an array, the element that holds the byte leads to it; of
    a struct, the member; and of a union, its first member, the one an
    initializer sets."""
    path = ''
    while True:
        term = declared_base(types, term)[0]
        if term[0] == 'array':
            size = types.layout(term[1]).size
            path, term, at = path + f'[{at // size}]', term[1], at % size
        elif term[0] in ('struct', 'union'):
            held = [(member, inner, offset)
                    for (member, inner), (_, layout, offset) in zip(types.members(*term), types.layout(term).fields)
                    if term[0] == 'union' or offset <= at < offset + layout.size]
            member, term, offset = held[0]
            path, at = f'{path}.{member}', at - offset
        else:
            return path, term


def spelling(types, name, term, addend, pointee):
    """The C that points a pointer to pointee, a type name as Pointer spells
    one, addend bytes past the start of name, an object of type term. Of
    subobjects(), it is the first that initializes the pointer with no
    cast: an array of pointee's type by its name, which C converts to its
    first element, and any other by its address. Where none does, the
    outermost is cast to the pointer's type. Raises LayoutError where
    Pointer spells no pointee for the pointer, and where subobjects()
    does."""
    if not pointee:
        raise LayoutError("no type name spells the field's pointee, to convert or to cast to")
    wanted = shape(types, types.term(pointee))
    outermost = None
    for path, (held, form) in subobjects(types, term, addend):
        if form[0] == 'array' and converts(types, form[1], wanted):
            return name + path
        if converts(types, (held, form), wanted):
            return '&' + name + path
        outermost = outermost or (path, form)
    path, form = outermost
    element, bracket, extents = pointee.partition('[')
    cast = f'({element} (*)[{extents})' if bracket else f'({pointee} *)'
    return cast + ('' if form[0] == 'array' else '&') + name + path


def row_target(name, addend):
    """The TARGET of a pointer row that points addend bytes past name."""
    return name + (f'+{addend:#x}' if addend else '')


class Targets:
    """What a source's pointer rows can name, and where each lies: a text by
    its macro, a function by its name, and an object by its declaration in
    scope at the row's datum.

    descriptors are the source's own places, as generate() reads them, and
    symbols the image's tables' (address, name) rows. unmeasured holds the
    names the source places that no descriptor gives, each at 0: in the
    stub, its private targets (private_targets())."""

    def __init__(self, types, declarators, descriptors, symbols, unmeasured=()):
        self.types = types
        self.texts = {name for name, datum in descriptors.items() if datum.text}
        # each name's declarators by the function whose blocks declare it, or None at file scope
        self.declarators, self.known = defaultdict(list), {}
        for each in declarators:
            self.declarators[each.function, each.name].append(each)
        self.places = {**{name: at for at, name in symbols}, **{name: datum.at for name, datum in descriptors.items()},
                       **dict.fromkeys(unmeasured, 0)}
        self.symbols = sorted(symbols)
        self.addresses = [at for at, _ in self.symbols]
        # each address's name: the source's own, and otherwise the first the tables give
        self.names = {**dict(reversed(symbols)), **{datum.at: name for name, datum in descriptors.items()}}
        self.owned = sorted((datum.at, datum.at + datum.size, name) for name, datum in descriptors.items())
        self.starts = [start for start, _, _ in self.owned]

    def scoped(self, name, where, scope=None):
        """The declarators of name that scope sees. scope is a function,
        whose blocks' declarations of name hide those at file scope, or None
        for file scope alone. A block's declaration hides them only inside
        its block, and a declarator doesn't give its block. So the run fails
        where name may mean two things in scope. One is a block's
        declaration without static storage, as an automatic object's. The
        other is a block's static beside another declaration of name, in
        another block or at file scope. where names the row in the
        message."""
        outer = self.declarators.get((None, name), [])
        inner = self.declarators.get((scope, name), []) if scope is not None else []
        bound = [entity(each.name, each.storage, each.term, each.function) for each in inner]
        if None in bound:
            fail(f'{where}: a block of {scope} declares {name} without static storage, which may hide the {name} '
                 'the row means')
        statics = sum(function is not None for _, function in bound)
        if statics and statics + bool(outer or statics < len(inner)) > 1:
            fail(f'{where}: {name} has a static declaration in a block of {scope}, and another declaration. Which '
                 'one the row means depends on blocks the tool doesn\'t read')
        return inner or outer

    def declared(self, name, where, scope=None):
        """The type that the declarations of name in scope give it, as a
        term, or None where none declares it, with scope as scoped() takes
        it. Of declarations that differ only where one gives an array no
        extent, it is one that gives the extent. The run fails on
        declarations that differ otherwise."""
        found = self.scoped(name, where, scope)
        key = (found[0].function if found else None, name)
        if key not in self.known:
            terms = [each.term for each in found]
            try:
                # an array without an extent matches any, so each pair must agree, not each with the first
                shapes = [shape(self.types, term) for term in terms]
                differ = any(held != other_held or not same(self.types, form, other)
                             for (held, form), (other_held, other) in itertools.combinations(shapes, 2))
            except LayoutError as error:
                fail(f'{where}: {name}: {error}')
            if differ:
                fail(f'{where}: conflicting declarations of {name}')
            complete = [term for term in terms if not extentless(self.types, term)]
            self.known[key] = (complete or terms or [None])[0]
        return self.known[key]

    def expression(self, target, addend, field, where, scope, declared):
        """The C that points field, a pointer's layout, addend bytes past
        target's start. declared is the field's path from its datum's name,
        as refs.step, and its type, as pointer_field() gives them. A text is
        its macro, for a pointer to char or void at its start. A function is
        its name, for a pointer to its type. An object is spelling()'s, from
        its declaration in scope. The run fails on any other, and on a
        target that no declaration gives or no place holds."""
        if target not in self.places:
            fail(f'{where}: neither the source nor the image\'s tables place {target}')
        try:
            if target in self.texts:
                pointee = shape(self.types, self.types.term(field.pointee))[1] if field.pointee else None
                if addend or pointee not in (VOID, CHAR):
                    fail(f'{where} needs a character or void pointer to the start of {target}')
                return target
            term = self.declared(target, where, scope)
            if term is None:
                fail(f'{where}: no declaration gives {target}')
            if declared_base(self.types, term)[0][0] == 'function':
                if not isinstance(field, FunctionPointer):
                    fail(f'{where} needs a C function pointer for {target}')
                if addend:
                    fail(f'{where}: {row_target(target, addend)} points past a function\'s entry')
                path, pointer = declared
                _, (_, (_, wanted)) = shape(self.types, pointer)
                if not same(self.types, shape(self.types, term)[1], wanted):
                    fail(f'{where}: {target}\'s type isn\'t the function type that {path} points to')
                return target
            if isinstance(field, FunctionPointer):
                fail(f'{where} cannot use object {target} as a function pointer')
            if extentless(self.types, term) and addend:
                start = self.places[target]
                between = self.symbols[bisect_right(self.addresses, start):bisect_right(self.addresses, start + addend)]
                other = next((name for _, name in between if name != target), None)
                if other:
                    fail(f'{where}: {row_target(target, addend)} lies at or past {other}, and {target} declares no '
                         'extent')
            return spelling(self.types, target, term, addend, field.pointee)
        except LayoutError as error:
            fail(f'{where}: {row_target(target, addend)}: {error}')

    def row_for(self, word):
        """The TARGET of the row that a pointer holding word asks for: the
        source's own datum, text or function that holds the address, and
        otherwise the first name the image's tables give at it. None where
        neither names it."""
        at = bisect_right(self.starts, word) - 1
        if at >= 0 and word < self.owned[at][1]:
            start, _, name = self.owned[at]
            return row_target(name, word - start)
        return self.names.get(word)


def private_targets(types, declarators, definitions, pointers, where):
    """The targets of pointers that the source's own table places beside its
    bodies and texts, as (functions, data). Each is a function, or a datum
    the source leaves uninitialized, that the declarations in scope at its
    row's datum declare static. Where those are a block's declarations
    with linkage, an extern's or a function's, the declarations at file
    scope decide, as they decide the name's linkage in C. functions is a
    set of names, and data maps each datum's name to its declared layout.
    definitions are the source's bodies, as bodies() gives them: a row's
    scope is its datum's. where is the rows' table, which a failure names.

    The image's tables must place every other name a row gives, or the
    build fails on it. A name with external linkage is a symbol there. A
    static datum that C initializes takes no place in the source's table,
    since body_links.py expects the stub link to hold zeros at every datum
    the table places in initialized data."""
    targets = Targets(types, declarators, {}, ())
    scopes = {each.name: each.function for each in definitions}
    functions, data = set(), {}
    for (owner, _), (target, _) in pointers.items():
        found = targets.scoped(target, where, scopes.get(owner))
        if found and found[0].function is not None and found[0].storage != 'static':
            # a block's declarations with linkage, which scoped() gives where it gives no block's static
            found = targets.scoped(target, where)
        if not any(each.storage == 'static' for each in found):
            continue
        try:
            term = targets.declared(target, where, found[0].function)
            if declared_base(types, term)[0][0] == 'function':
                functions.add(target)
            elif not any(each.initialized for each in found):
                data[target] = types.layout(term)
        except LayoutError as error:
            fail(f'{where}: {target}: {error}')
    return functions, data


def include_root(source, paths):
    """Choose the compiler include directory shared by this member's generated bodies."""
    owner = source.with_suffix('').relative_to(source.parts[0])
    if any(path.parent != owner for path in paths):
        return Path('variants') / source.with_suffix('')
    return Path(source.parts[0])


def generate(image, source, output, original=None, stub=False):
    """Generate the native/GNU data bodies, or a stub from C types and bindings alone."""
    table = Path('offsets') / source.with_suffix('.txt')
    bindings_path = body_bindings.path(source)
    texts, types, declarators = declarations(source)
    definitions = bodies(source, texts, declarators)
    layouts = typed_bodies(source, types, definitions)
    owners = {each.name: each for each in definitions}
    lengths, pointers, bindings, fields = body_bindings.read_all(source)
    selectors = integer_selectors(source, types, layouts, fields)
    stops = signed_stops(source, types, definitions, bindings) if bindings else {}
    if stub:
        descriptors = {name: Datum(0, size, '', True) for name, size in lengths.items()}
        descriptors.update({name: Datum(0, layout.size, '', False) for name, (layout, _) in layouts.items()})
    else:
        descriptors = locations(image, source, table)
    private_functions, private_data = private_targets(types, declarators, definitions, pointers, bindings_path)
    if not stub:
        # the row the table gives each name but a text, and the row the source wants: an initialized datum for a
        # body or a datum that takes numbered texts, an entry for a private function, and an uninitialized datum
        # for a private datum
        held = {name: 'an entry' if code(datum.section) else 'an uninitialized datum' if zeroed(datum.section)
                else 'an initialized datum' for name, datum in descriptors.items() if not datum.text}
        wanted = {**dict.fromkeys(layouts, 'an initialized datum'), **dict.fromkeys(private_functions, 'an entry'),
                  **dict.fromkeys(private_data, 'an uninitialized datum')}
        differ = [f'{name}: the table gives {held.get(name, "none")}, the source wants {wanted.get(name, "none")}'
                  for name in sorted(held.keys() | wanted.keys()) if held.get(name) != wanted.get(name)]
        if differ:
            fail(f'{table}: ' + '; '.join(differ))

    def datum_bytes(name, layout):
        # name's bytes: the image's, or zeros for the build without the disc. The run fails where name's place
        # disagrees with layout's size or alignment
        datum = descriptors[name]
        if datum.size != layout.size or datum.at % layout.alignment:
            fail(f'{table}: {name} disagrees with its C type (size {layout.size}, alignment {layout.alignment})')
        start = datum.at - image.base
        data = bytes(datum.size) if original is None else original[start:start + datum.size]
        if len(data) != datum.size:
            fail(f'{source}: original image ends before the end of {name}')
        return data

    numbered = {}
    for each, names in numbered_texts(source, texts, declarators).items():
        layout = layouts[each.name][0]
        data = datum_bytes(each.name, layout)
        numbered.update(zip(names, text_values(layout, None if original is None else data, names,
                                               f'{source}: {each.name}')))
    headers = string_macros(source, descriptors, texts, layouts, pointers, image, original, numbered)
    symbols = image.symbols()
    targets = Targets(types, declarators, descriptors, symbols, private_functions | private_data.keys() if stub else ())
    if not stub:
        # a name with external linkage at file scope is a symbol, and symbols.txt gives its place. A body that a
        # function declares is its block's own, whatever symbol shares its name
        scoped = [each for each in declarators if each.function is None]
        external = ({each.name for each in scoped} - {each.name for each in scoped if each.storage == 'static'}
                    - {each.name for each in definitions if each.function is not None})
        placed = {name: at for at, name in symbols}
        for name, datum in descriptors.items():
            if name in external and name in placed and placed[name] != datum.at:
                fail(f'{table}: {name} is placed at {hexa(datum.at)}, symbols.txt gives {hexa(placed[name])}')
        for name, layout in sorted(private_data.items()):
            datum = descriptors[name]
            if datum.size != layout.size or datum.at % layout.alignment:
                fail(f'{table}: {name} disagrees with its C type (size {layout.size}, alignment {layout.alignment})')
    lists = list_kinds(source, types, definitions, layouts, pointers)
    result = {}
    for name, (layout, path) in layouts.items():
        if path is None:
            continue  # numbered texts, which text.inc spells
        # the body's scope, for the names its rows give: the function whose blocks declare it, or None at file scope
        scope, data = owners[name].function, datum_bytes(name, layout)
        if name in selectors:
            data = bytearray(data)
            apply_selectors(data, selectors[name], source, original=None if original is None else data)
        if name in stops:
            stop = stops[name]
            if original is None:
                data = bytearray(data)
                data[stop.offset:stop.offset + len(stop.marker)] = stop.marker
                for offset, encoded, field in selectors.get(name, ()):
                    if data[offset:offset + len(encoded)] != encoded:
                        fail(f'{source}: {name}: the stop in its first record overwrites selector {field}')
            else:
                stop.check(data, f'{source}: {name}')

        def pointer(at, word, field):
            # a field that no row names holds null
            where = f'{bindings_path}: {name}+{hexa(at)}'
            target, addend = pointers.pop((name, at), (None, 0))
            if target is None:
                expression, expected = '0', 0
            else:
                member, declared = pointer_field(types, owners[name].term, at)
                expression = targets.expression(target, addend, field, where, scope, (name + member, declared))
                expected = targets.places[target] + addend
            if original is None or word == expected:
                return expression
            if not word:
                fail(f'{where} holds null, expected {expression}: a null field takes no row')
            row = targets.row_for(word)
            fail(f'{where} holds {hexa(word)}, ' + ('not null' if target is None else f'expected {expression}') +
                 (f': its row is pointer  {name}  {hexa(at)}  {row}' if row else ', which no table names'))

        kind = lists.get(name) if original is not None else None
        if kind:
            try:
                rows = list(LIST_ROWS[kind](*walk(kind, data)))
            except ChunkError as error:
                fail(f'{source}: {name}, a {kind} list: {error}')
            lines = list_body([initializer(layout.element, data, at, pointer)
                               for at in range(0, len(data), layout.element.size)], rows)
        else:
            lines = textwrap.fill(initializer(layout, data, 0, pointer), width=LINE_WIDTH, initial_indent='    ',
                                  subsequent_indent='    ', break_long_words=False, break_on_hyphens=False)
        if original is not None:
            # GCC relocates a pointer alone, so a word that holds an address needs a pointer's type
            for at in integer_fields(layout):
                word = int.from_bytes(data[at:at + 4], 'little')
                if word in targets.names:
                    fail(f'{table}: {name}+{hexa(at)} holds {hexa(word)}, the address of {targets.names[word]}, in '
                         'a field its declaration types as an integer')
        result[path] = lines + '\n'
    if pointers:
        name, at = next(iter(pointers))
        fail(f'{bindings_path}: {name}+{hexa(at)} has no initialized C pointer')
    result.update(headers)
    root = output / include_root(source, result)
    for path, body in result.items():
        save(root / path, body.encode())


def source_inputs(source, *, generated_headers, texts=None):
    """Return the dependencies and generated headers. Optionally retain the source texts for the caller. The
    dependencies are each file of the tree that source includes, whatever its suffix, as declarations() finds them,
    and each shared bindings table that its own table includes."""
    if texts is None:
        texts = {}
    included(source, texts, ANY_SUFFIX)
    headers = sorted(texts) + sorted(body_bindings.dependencies(source))
    return headers, sorted(set(generated(texts)) if generated_headers else ())


def with_bodies(source, root):
    """The bytes of source, a C source, as lint's include-cleaner reads it:
    each body that source's own text includes, as the build generated it
    under root, in place of its #include. include-cleaner counts only the
    names spelled in the file it reads, so the copy makes a body's names its
    source's uses. Each body's lines are joined into one, so that every line
    of source keeps its number. The joined line reads as the body's lines
    do, because generate writes no directive, // comment or line splice into
    a body. A NOLINT ends that line: without it, include-cleaner would ask
    there, on a line no one wrote, for the header of each name the body
    takes through another header's includes. A text.inc of #defines stays
    an include, since each #define needs a line of its own. A datum named
    text has a text.inc too, its body, which holds no directive."""
    # latin-1 reads each byte as one character, so every byte outside the
    # bodies' includes is the source's
    text = content(source).decode('latin-1')
    own = set(generated({source: text}))
    pieces, at = [], 0
    for directive, _, name in header_names(text):
        if Path(name) in own:
            lines = content(root / name).decode('latin-1').splitlines()
            if any(line.startswith('#') for line in lines):
                continue
            body = ' '.join(line.strip() for line in lines)
            pieces += [text[at:directive.start], body, ' /* NOLINT(misc-include-cleaner) */',
                       '\n' * (directive.last - directive.line)]
            at = directive.end
    return (''.join(pieces) + text[at:]).encode('latin-1')


def main():
    parser = argparse.ArgumentParser(prog='body_data', description=__doc__.splitlines()[0])
    parser.add_argument('files', nargs='*', metavar='PATH')
    mode = parser.add_mutually_exclusive_group()
    mode.add_argument('--stub', action='store_true', help='compile from C declarations, text widths and pointer names')
    mode.add_argument('--original', type=Path, help='read real values from this decompressed disc image')
    mode.add_argument('--lint', action='store_true',
                      help="write COPY: SOURCE with its bodies under ROOT in place, for lint's include-cleaner")
    scan = parser.add_mutually_exclusive_group()
    scan.add_argument('--dependencies', action='store_true', help='print source and transitive headers')
    scan.add_argument('--inputs', action='store_true', help='print dependencies and generated include names')
    args = parser.parse_args()
    scanning = args.dependencies or args.inputs
    if len(args.files) != (1 if scanning else 3) or (scanning and (args.original or args.stub or args.lint)):
        parser.error('give IMAGE SOURCE OUTPUT_ROOT [--original FILE], ROOT SOURCE COPY --lint, '
                     'or (--dependencies|--inputs) SOURCE')
    source = Path(args.files[0 if scanning else 1])
    if not SOURCE.fullmatch(str(source)) or source.suffix != '.c':
        parser.error('source must be a project C translation unit')
    if scanning:
        headers, generated = source_inputs(source, generated_headers=args.inputs)
        print(*headers, *generated)
        return
    if args.lint:
        save(args.files[2], with_bodies(source, Path(args.files[0])))
        return
    generate(Image(args.files[0]), source, Path(args.files[2]), content(args.original) if args.original else None, args.stub)


if __name__ == '__main__':
    main()
