"""Pack the kana readings and written candidates of the small ASK dictionary."""

import argparse
import struct
from pathlib import Path

from config import fail, save

PAGE_BYTES = 1024
INDEX_BYTES = 4 * PAGE_BYTES
LEARNING_PAGES = 8
SMALL_ROM_BYTES = 332 * PAGE_BYTES
PREFIX_OFFSET = 78
PREFIX_ROW_BYTES = 11
FIRST_KANA = 0xA1
LONG_VOWEL = 0x9D
LAST_CLASS = 56
LAST_INFLECTION = 11


def reading_key(reading):
    """ASK stores one byte per kana, including the prolonged sound mark."""
    key = bytearray()
    for kana in reading:
        if kana == 'ー':
            key.append(LONG_VOWEL)
        else:
            pair = kana.encode('cp932')
            if len(pair) != 2 or pair[0] != 0x82 or not 0x9f <= pair[1] <= 0xf1:
                raise ValueError(f'{kana} is outside the ASK hiragana alphabet')
            key.append(pair[1] + 2)
    if not 1 <= len(key) <= 255:
        raise ValueError('a reading must contain between 1 and 255 kana')
    return key


def candidate(field):
    kind, frequency, inflection, spelling = field.split(':', 3)
    kind = int(kind)
    if not 0 <= kind <= LAST_CLASS:
        raise ValueError(f'unknown grammatical class {kind}')
    entry = bytearray([kind, int(frequency)])
    if inflection != '-':
        form = int(inflection)
        if not 0 <= form <= LAST_INFLECTION:
            raise ValueError(f'unknown inflection {form}')
        entry.append(form)
    if not spelling:
        raise ValueError('a candidate needs a written form')
    for character in spelling:
        pair = character.encode('cp932')
        if len(pair) != 2:
            raise ValueError(f'{character} is outside the double-byte character set')
        entry.extend(pair)
    return entry


def pack(lines):
    """Build dictionary pages and derive the two-kana filter and lookup index."""
    header = bytearray(PAGE_BYTES)
    signature = b'ASK Copyright (C) ACCESS CO.,LTD.'
    header[:len(signature)] = signature
    index = bytearray()
    pages = []
    page = bytearray()
    previous = b''
    for lineno, line in enumerate(lines, 1):
        line = line.rstrip('\r\n')
        if line.startswith('#'):
            continue
        if not line:
            if page:
                pages.append(page.ljust(PAGE_BYTES, b'\0'))
                page = bytearray()
            continue
        try:
            reading, *candidates = line.split('\t')
            key = reading_key(reading)
            if key <= previous or not candidates:
                raise ValueError('readings must be unique, ordered and have candidates')
            record = bytearray([len(key)]) + key
            for field in candidates:
                record.extend(candidate(field))
            size = len(record) + 2
            if len(page) + size > PAGE_BYTES:
                raise ValueError('the reading crosses its page boundary')
            if not page:
                if len(index) == INDEX_BYTES:
                    raise ValueError('the dictionary holds at most 1024 pages')
                if key[:4] == previous[:4]:
                    raise ValueError('this page and the last both hold readings that start with these four kana, '
                                     "which the index can't tell apart")
                first = key if index else b'\x01'
                index.extend(first[:4].ljust(4, b'\0'))
            previous = key
            page.extend(struct.pack('<H', size))
            page.extend(record)
            if len(key) <= 2 and all(k >= FIRST_KANA for k in key):
                column = key[1] - FIRST_KANA + 1 if len(key) == 2 else 0
                row = (key[0] - FIRST_KANA) * PREFIX_ROW_BYTES
                header[PREFIX_OFFSET + row + column // 8] |= 1 << (column % 8)
        except (ValueError, IndexError) as error:
            raise ValueError(f'line {lineno}: {error}') from error
    if page:
        pages.append(page.ljust(PAGE_BYTES, b'\0'))
    if not pages:
        raise ValueError('the dictionary needs at least one page')
    struct.pack_into('<4H', header, 40, 3, INDEX_BYTES, LEARNING_PAGES, len(pages))
    return b''.join([header, index.ljust(INDEX_BYTES, b'\0'),
                     bytes(PAGE_BYTES * LEARNING_PAGES), *pages])


def main():
    parser = argparse.ArgumentParser(prog='ask_lexicon', description=__doc__)
    parser.add_argument('lexicon', type=Path,
                        help='a reading per line, in the order of its ASK key, then a tab before each of its '
                             'class:frequency:inflection:spelling candidates, with - for no inflection; '
                             'a blank line ends a ROM page, and a line that starts with # is a comment')
    parser.add_argument('output', type=Path)
    parser.add_argument('--binary', action='store_true', help='write the native ROM format')
    args = parser.parse_args()
    try:
        with args.lexicon.open(encoding='utf-8') as lexicon:
            rom = pack(lexicon)
    except OSError as error:
        fail(error)
    except ValueError as error:
        fail(f'{args.lexicon}: {error}')
    if len(rom) != SMALL_ROM_BYTES:
        fail(f'{args.lexicon}: expected {SMALL_ROM_BYTES} bytes in the small dictionary, got {len(rom)}')
    if args.binary:
        save(args.output, rom)
    else:
        rows = [''.join(f'0x{byte:02x},' for byte in rom[start:start + 16]) + '\n'
                for start in range(0, len(rom), 16)]
        save(args.output, ''.join(rows).encode('ascii'))


if __name__ == '__main__':
    main()
