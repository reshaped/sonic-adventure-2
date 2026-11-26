"""Count each function's instruction words that differ from the original.

    make build/obj/src/system/task.obj
    python3 tools/words.py 1st_read src/system/task.c

It measures a hack before the hack takes its tag (TAGS.md#hacks): build the
source in the plain form, run this, and read the function's line. It links the
source's object where IMAGE's tables start its sections, whatever its size, so
a form that changes the code's size still measures. For each address the
tables name in the source's code, it prints a line under the address's first
name, in the order of the source's rows and then of address:

    task_alloc: 3 of 64 words differ, first at 0x8c05fd30

M is the 2-byte words of the original function that control reaches from its
entry, and N those that differ at the same offset in the link, where a byte
the link doesn't write differs from any. A function with no difference prints
nothing. The last line says whether any differs. The exit status is 3 if any
does, apart from a failure's 1 and a refused command line's 2.

It needs the source's object under build/obj/ and the image's original: its
file in orig/, or for a .PRS file the .BIN that make writes under build/orig/.
"""

import argparse
from pathlib import Path

import link
import sysrof
from config import LINKS, OBJECTS, Image, code, content, fail, hexa

# the exit status where a function differs, which no failure takes
DIFFERS = 3


def sext(value, bits):
    """value, an unsigned number of that many bits, as two's complement."""
    return value - (1 << bits) if value & 1 << (bits - 1) else value


def reached(function, at):
    """The offsets in function, a function's bytes at address at, of the
    instruction words control reaches from its entry.

    Control follows each branch and its delay slot, both ways from a
    conditional one, and goes on past a call. It stops at a return and at a
    jump through a register. A switch's cases are reached through its table:
    16-bit offsets at the target of the last MOVA on the path to BRAF, which
    BRAF adds to its own address plus 4, or the cases' addresses as longs,
    which a JMP reaches. A delay slot counts without control running on from
    it, so a branch to one still goes on past it."""
    last = len(function) - 1
    # seen holds the words control ran on from, slots the delay slots
    seen, slots, work = set(), set(), [(0, None)]

    def word(k):
        return function[k] | function[k + 1] << 8

    while work:
        while work:
            k, table = work.pop()
            while 0 <= k < last and k not in seen:
                seen.add(k)
                op = word(k)
                if op >> 8 == 0xC7:  # MOVA @(disp,PC),R0
                    table = ((at + k) & ~3) + 4 + (op & 0xFF) * 4 - at
                if op >> 12 == 0xA:  # BRA
                    work.append((k + 4 + sext(op & 0xFFF, 12) * 2, table))
                    slots.add(k + 2)
                    break
                if op >> 8 in (0x89, 0x8B):  # BT, BF
                    work.append((k + 4 + sext(op & 0xFF, 8) * 2, table))
                    k += 2
                elif op >> 8 in (0x8D, 0x8F):  # BT/S, BF/S
                    work.append((k + 4 + sext(op & 0xFF, 8) * 2, table))
                    slots.add(k + 2)
                    k += 4
                elif op in (0x000B, 0x002B) or op & 0xF0FF == 0x402B:  # RTS, RTE, JMP @Rm
                    slots.add(k + 2)
                    break
                elif op & 0xF0FF == 0x0023:  # BRAF Rm, a switch
                    slots.add(k + 2)
                    entry, first = table, last
                    while table is not None and entry < first:
                        case = k + 4 + sext(word(entry), 16)
                        if case % 2 or not 0 <= case < last:
                            break
                        if case > entry:
                            first = min(first, case)
                        work.append((case, None))
                        entry += 2
                    break
                elif op >> 12 == 0xB or op & 0xF0FF in (0x400B, 0x0003):  # BSR, JSR, BSRF: a call returns
                    slots.add(k + 2)
                    k += 4
                else:
                    k += 2
        # a long holding one of the function's own addresses that control
        # hasn't reached is a switch's case, which a JMP @Rm reaches
        for k in range(0, last - 2, 2):
            if k in seen or k in slots or k + 2 in seen or k + 2 in slots or (at + k) % 4:
                continue
            target = int.from_bytes(function[k:k + 4], 'little') - at
            if 0 <= target < last and target % 2 == 0 and target not in seen:
                work.append((target, None))
    return {k for k in seen | slots if k < last}


def differing(ours, theirs, at):
    """How theirs, a function's original bytes at address at, compares with
    ours, the link's bytes there, None where it writes none: (N, M, the
    address of the first word that differs or None)."""
    words = sorted(reached(theirs, at))
    differ = [k for k in words if list(ours[k:k + 2]) != list(theirs[k:k + 2])]
    return len(differ), len(words), at + differ[0] if differ else None


def functions(image, ranges):
    """The functions the tables name in ranges, a source's code rows, as
    (name, start, end), row by row and in address order within one. An
    address's name is its first in image.symbols(). A function ends at the
    next named address, or where its row does."""
    found = []
    for _, low, high in ranges:
        inside = sorted({address: name for address, name in reversed(image.symbols()) if low <= address < high}.items())
        found += [(name, start, end) for (start, name), end in zip(inside, [address for address, _ in inside[1:]] + [high])]
    return found


def main(argv=None):
    parser = argparse.ArgumentParser(prog='words', description=__doc__.split('\n\n')[0])
    parser.add_argument('image', metavar='IMAGE', help='the image to compare in, config/IMAGE/')
    parser.add_argument('source', metavar='SOURCE', help='a source the image places')
    args = parser.parse_args(argv)
    image = Image(args.image)
    ranges = [row for row in image.held(args.source) if code(row[0])]
    if not ranges:
        fail(f'{args.source}: {image.splits} places no code of it')
    obj = OBJECTS / Path(args.source).with_suffix('.obj')
    if not obj.is_file():
        fail(f'{obj}: no such object; make it first')
    original = Path('build/orig', image.file).with_suffix('.BIN') if image.compressed else Path('orig', image.file)
    theirs = content(original)
    measured = functions(image, ranges)
    if not measured:
        fail(f"{args.source}: {image.name}'s symbols name no address in its code")
    outside = [(start, end) for _, start, end in measured
               if start < image.base or end > image.base + len(theirs)]
    if outside:
        fail(*(f'{original}: {hexa(start)} to {hexa(end)} lies outside its {len(theirs)} bytes from {hexa(image.base)}'
               for start, end in outside))
    placed = {}
    for name, start, _ in image.ranges(args.source):
        placed.setdefault(name, start)
    declared = sysrof.sections(obj)
    placed = {name: start for name, start in placed.items() if name in declared}
    out = LINKS / image.name / Path(args.source).with_suffix('')
    names = link.bound(args.source, image, sysrof.references(obj), ())
    written = link.place([obj, link.define(out.with_name(out.name + '_words_syms.src'), names)], placed,
                         out.with_name(out.name + '_words'))
    differ = 0
    for name, start, end in measured:
        ours = [written.get(at) for at in range(start, end)]
        count, total, first = differing(ours, theirs[start - image.base:end - image.base], start)
        if count:
            differ += 1
            print(f'{name}: {count} of {total} words differ, first at {hexa(first)}')
    summary = {0: 'no word differs', 1: '1 function differs'}.get(differ, f'{differ} functions differ')
    print(f'{args.source} in {image.name}: {summary}')
    raise SystemExit(DIFFERS if differ else 0)


if __name__ == '__main__':
    link.stoppable()
    main()
