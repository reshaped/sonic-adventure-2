"""Check the changes a linked member's disc-free initializers are allowed to make."""
import argparse
from collections import Counter, defaultdict
from pathlib import Path

import body_bindings
from body_data import apply_selectors, bodies, integer_selectors, locations, signed_stops, stub_text, typed_bodies
from config import LINKS, Image, code, content, fail, hexa, images, stub_sources, zeroed
from decls import declarations


def expected(image, source, full):
    """Replace measured data with the stub values, keeping every linked pointer and instruction."""
    table = Path('offsets') / source.with_suffix('.txt')
    lengths, pointers, stop_bindings, fields = body_bindings.read_all(source)
    data = locations(image, source, table)
    stops = {}
    selectors = {}
    if stop_bindings or fields:
        texts, types, declarators = declarations(source)
        definitions = bodies(source, texts, declarators)
        stops = signed_stops(source, types, definitions, stop_bindings)
        if fields:
            selectors = integer_selectors(source, types, typed_bodies(source, types, definitions), fields)
    if lengths != {name: datum.size for name, datum in data.items() if datum.text}:
        fail(f'{table}: measured texts and bound lengths disagree')
    # The compiler pools equal literals. Each placeholder of the same width has
    # its own ordinal in binding order, regardless of the order in the listing.
    counts = Counter()
    texts = {}
    for name, size in lengths.items():
        texts[name] = stub_text(size, counts[size]) + b'\0'
        counts[size] += 1

    words = defaultdict(list)
    for name, offset in pointers:
        words[name].append(offset)
    regions = {}
    cursor = 0
    for section, start, end in image.held(str(source)):
        regions.setdefault(section, []).append((start, end, cursor))
        cursor += end - start
    if len(full) != cursor:
        fail(f'{image.name}: {source}: linked member has {len(full)} bytes, placements hold {cursor}')
    result = bytearray(full)
    for name, datum in data.items():
        if code(datum.section) or zeroed(datum.section):
            continue
        for start, end, cursor in regions[datum.section]:
            if start <= datum.at and datum.at + datum.size <= end:
                at = cursor + datum.at - start
                break
        else:
            fail(f'{table}: {name} lies outside the linked member')
        result[at:at + datum.size] = texts[name] if datum.text else bytes(datum.size)
        if name in selectors:
            view = memoryview(result)[at:at + datum.size]
            apply_selectors(view, selectors.pop(name), source, original=full[at:at + datum.size])
        if name in stops and not datum.text:
            stop = stops.pop(name)
            stop.check(full[at:at + datum.size], f'{table}: {name}')
            first = at + stop.offset
            result[first:first + len(stop.marker)] = stop.marker
        for offset in words.pop(name, ()):
            if datum.text or offset % 4 or offset + 4 > datum.size:
                fail(f'{table}: pointer outside {name} at offset {hexa(offset)}')
            word = at + offset
            result[word:word + 4] = full[word:word + 4]
    if selectors:
        fail(f'{table}: selectors outside initialized data in {", ".join(sorted(selectors))}')
    if stops:
        fail(f'{table}: stop tables outside initialized data in {", ".join(sorted(stops))}')
    if words:
        fail(f'{table}: pointer fields outside initialized data in {", ".join(sorted(words))}')
    return result


def check(image, source):
    name = Path(image.name) / source.with_suffix('.bin')
    wanted = expected(image, source, content(LINKS / name))
    stub = content(Path('build/stub/link') / name)
    if len(stub) != len(wanted):
        fail(f'{image.name}: {source}: full and stub links have different lengths')
    if stub != wanted:
        offset = next(i for i, (a, b) in enumerate(zip(stub, wanted)) if a != b)
        for _, start, end in image.held(str(source)):
            if offset < end - start:
                fail(f'{image.name}: {source}: stub differs from its expected byte at {hexa(start + offset)}')
            offset -= end - start


def main():
    parser = argparse.ArgumentParser(prog='body_links', description=__doc__)
    parser.add_argument('image', nargs='?', help='check only this image')
    parser.add_argument('source', nargs='?', help='check only this member of the image')
    args = parser.parse_args()
    sources = stub_sources()
    for name in images() if args.image is None else [args.image]:
        image = Image(name)
        selected = set(image.sources()) & sources
        if args.source is not None:
            if args.source not in selected:
                fail(f'{name}: {args.source} has no disc-backed member placement')
            selected = {args.source}
        for source in sorted(selected):
            check(image, Path(source))


if __name__ == '__main__':
    main()
