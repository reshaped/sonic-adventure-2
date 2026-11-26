"""Read config/kept.txt, the shift test's reasons, for tools/relink.py."""

from config import ADDRESS, CONFIG, address, aligned, fail, hexa, images, interval, rows

# the aligned 32-bit words the shift test may find keeping a value in their
# image's span, each with the reason it takes no name
KEPT = CONFIG / 'kept.txt'


def reasons():
    """config/kept.txt's rows by image, as lists of (start, end, reason,
    where) in table order. start and end bound the aligned 32-bit words the
    shift test may find keeping a value in the image's span. reason says why
    each takes no name, and where gives the row's path:line.

    A row is IMAGE START [END] REASON, and a field after START that reads
    as an address is END. START alone gives the word at START, which must
    lie on 4. With END, the row gives each aligned word whose address lies
    in [START, END). A reason that starts with an address therefore needs
    END, as START + 4 for a single word.

    A row fails where it:
    - names no image of config/;
    - holds no word;
    - gives no reason;
    - gives a word an earlier row of its image gives.
    """
    found, known = {}, set(images())
    for where, row in rows(KEPT):
        if len(row) < 3:
            fail(f'{where}: not a row: IMAGE START [END] REASON')
        image, start, *rest = row
        if image not in known:
            fail(f'{where}: {image}: no image of {CONFIG}/')
        if ADDRESS.fullmatch(rest[0]):
            start, end = interval(start, rest.pop(0), where)
            if aligned(start, 4) >= end:
                fail(f'{where}: {hexa(start)} to {hexa(end)} holds no word')
        else:
            start = address(start, where)
            if start % 4:
                fail(f"{where}: {hexa(start)}: not on 4, so no word's address")
            end = start + 4
        if not rest:
            fail(f'{where}: not a row: IMAGE START [END] REASON')
        for low, high, _, earlier in found.get(image, []):
            if aligned(max(start, low), 4) < min(end, high):
                fail(f'{where}: {hexa(start)} to {hexa(end)} overlaps the row at {earlier}')
        found.setdefault(image, []).append((start, end, ' '.join(rest), where))
    return found
