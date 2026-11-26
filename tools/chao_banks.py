"""Place the typed objects stored in the two Chao archives at their RAM addresses.

The resident garden loads these two files before any area. Their internal Ninja
pointers already use the P0 alias of RAM. The original game writes through P1.
The linker reserves each complete file once, including unnamed data between
the public objects. A bank contributes no initialized bytes to the executable.
"""

import argparse
from pathlib import Path
from typing import NamedTuple

from config import at_root, fail, named, save


class Bank(NamedTuple):
    name: str
    base: int
    size: int


# the archives' banks, which tools/dc_memory.py holds to where
# src/system/areas.h places each archive
MODELS = Bank('models', 0x0c698700, 2235540)
MOTIONS = Bank('motions', 0x0c8ba800, 1065680)
# The four held props are declared as arrays by the race renderer. Each
# referenced object is the root of one Ninja tree, including its children.
RACE_PROPS = {'race_omo_accessory_a', 'race_omo_accessory_b', 'race_omo_both_hands',
              'race_omo_starter_accessory'}
EXTRA_MODELS = {'garden_pumpkin_model', 'chao_hat_1', 'chao_hat_2', 'chao_hat_3',
                'chao_special_model_1', 'chao_special_model_2', 'race_omo_model'}


class Object(NamedTuple):
    address: int
    name: str
    ctype: str
    count: int
    width: int


def objects(root):
    """The objects the Chao images' symbols.txt place in the two archives, by
    bank, each bank's in address order. An object's name gives its bank and
    type. A name at two addresses fails, as does an object that is
    misaligned, overlaps the one before it or isn't wholly in its bank."""
    result = {MODELS: [], MOTIONS: []}
    for image in ('chao', 'chaostgrace'):
        for address, name in named(root / image / 'symbols.txt'):
            if name.startswith('chao_model_') or name in EXTRA_MODELS or name in RACE_PROPS:
                result[MODELS].append(Object(address, name, 'NJS_CNK_OBJECT', int(name in RACE_PROPS), 52))
            elif name == 'chao_costume_models':
                result[MODELS].append(Object(address, name, 'NJS_CNK_OBJECT *', 29, 116))
            elif name.startswith('chao_clip_'):
                result[MOTIONS].append(Object(address, name, 'NJS_MOTION', 0, 12))
    for bank in result:
        # An area and its resident can both name the same object.
        entries = sorted(set(result[bank]))
        result[bank] = entries
        names = set()
        previous = None
        for obj in entries:
            if obj.name in names:
                raise ValueError(f'{obj.name}: two addresses in {bank.name}')
            names.add(obj.name)
            if obj.address % 4:
                raise ValueError(f'{obj.name}: misaligned at {obj.address:#010x}')
            if obj.address < bank.base or obj.address + obj.width > bank.base + bank.size:
                raise ValueError(f'{obj.name}: {obj.width} bytes at {obj.address:#010x} are not wholly in {bank.name}')
            if previous and obj.address < previous.address + previous.width:
                raise ValueError(f'{obj.name}: starts at {obj.address:#010x}, '
                                 f'before {previous.name} ends at {previous.address + previous.width:#010x}')
            previous = obj
    return result


def declarations(banks):
    yield '#include <ninja.h>'
    yield ''
    for entries in banks.values():
        for obj in entries:
            index = f'[{obj.count}]' if obj.count else ''
            yield (f'{obj.ctype} {obj.name}{index} '
                   f'__attribute__((used, section(".bss.chao_bank.{obj.name}")));')


def sections(banks):
    for bank, entries in banks.items():
        section = f'.chao_{bank.name}'
        yield f'{section} {bank.base:#010x} (NOLOAD) : {{'
        for obj in entries:
            yield f'  . = {obj.address - bank.base:#x};'
            yield f'  KEEP(*(.bss.chao_bank.{obj.name}))'
            # A changed C type or a missing provider cannot silently blend into
            # unlabelled space between the objects.
            yield f'  ASSERT(. == {obj.address - bank.base + obj.width:#x}, "{obj.name} size");'
        yield f'  . = {bank.size:#x};'
        yield '}'
        yield f'_chao_{bank.name}_upload = ADDR({section}) | 0x80000000;'
        # SIZEOF holds the section to the reservation, not to the file.
        # tools/check_chao_banks.py holds the decoded archive to that length.
        yield f'ASSERT(SIZEOF({section}) == {bank.size:#x}, "{bank.name} length")'
        # Alignment padding can move an object yet keep its end where the
        # size check wants it. SH-4 GCC spells each C symbol with a leading _.
        for obj in entries:
            yield f'ASSERT(_{obj.name} == {obj.address:#010x}, "{obj.name} address")'


def main():
    parser = argparse.ArgumentParser(prog='chao_banks', description=__doc__.splitlines()[0])
    parser.add_argument('kind', choices=('c', 'ld'))
    parser.add_argument('output', type=Path)
    args = parser.parse_args()
    at_root('config/')
    try:
        banks = objects(Path('config'))
    except ValueError as error:
        fail(error)
    lines = declarations(banks) if args.kind == 'c' else sections(banks)
    save(args.output, ('\n'.join(lines) + '\n').encode())


if __name__ == '__main__':
    main()
