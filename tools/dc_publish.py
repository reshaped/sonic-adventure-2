"""Pack a linked Dreamcast ELF at the addresses the game loads from its optical file."""

import argparse
from hashlib import sha256
from pathlib import Path
import struct
import subprocess
import tempfile

from config import Image, MAIN, fail, save
from dc_overlay import ET_EXEC, MODELS, MOTIONS, Elf, chao_banks, physical, verify_header
from tool_process import run


def resident_startup(elf, data):
    """Check startup's copy and clear ranges before leaving BSS out of the boot file."""
    def word(name):
        offset = elf.defined.get(name, 0) - elf.entry
        if not 0 <= offset <= len(data) - 4:
            raise ValueError(f'{elf.path}: missing resident startup word {name}')
        return struct.unpack_from('<I', data, offset)[0]

    head, bss = elf.section('.resident_header'), elf.section('.bss')
    if (head.address != 0x8c010000 or elf.entry != head.address or head.size != 128
            or elf.defined.get('_START_SYS') != head.address
            or struct.unpack_from('<I', data, 0x2c)[0] != elf.defined.get('__INIT')):
        raise ValueError(f'{elf.path}: invalid boot entry or startup record')
    first, middle, second, end = (word(n) for n in ('__B_BGN', '__B_END', '__BSG_BGN', '__BSG_END'))
    if not (first == bss.address and first < middle == second < end == bss.address + bss.size):
        raise ValueError(f'{elf.path}: startup does not clear the complete BSS')
    if word('__D_BGN') != word('__D_END') or word('__DSG_BGN') != word('__DSG_END'):
        raise ValueError(f'{elf.path}: initialized resident data must run in place')


def payload(name, elf):
    """Resident BSS is cleared by startup. The overlay loader copies its BSS from the PRS."""
    resident = name == MAIN
    if resident:
        if elf.entry != 0x8c010000:
            raise ValueError(f'{elf.path}: invalid resident base')
    else:
        verify_header(name, elf)
    external = {}
    if name == 'chao':
        chao_banks(elf)
        external = {(bank.base, bank.size): '.chao_' + bank.name for bank in (MODELS, MOTIONS)}
    if any(section.kind in (4, 9) and section.size for section in elf.sections):
        raise ValueError(f'{elf.path}: executable contains relocation records')
    segments = []
    banks = set()
    for load in elf.segments:
        if load.physical_address != load.address:
            raise ValueError(f'{elf.path}: different load and virtual addresses')
        extent = (load.address, load.memory_size)
        if extent in external:
            if load.file_size or load.flags != 6:
                raise ValueError(f'{elf.path}: archive bank must be writable and empty')
            banks.add(extent)
        else:
            offset = load.address - elf.entry
            if offset < 0 or offset != physical(load.address) - physical(elf.entry):
                raise ValueError(f"{elf.path}: load at {load.address:#010x} doesn't lie at or past the entry, "
                                 f"{elf.entry:#010x}, in the entry's view")
            segments.append(load)
    if banks != set(external) or not segments or min(load.address for load in segments) != elf.entry:
        raise ValueError(f'{elf.path}: image entry or reserved archive bank missing')
    memory_end = max(load.address + load.memory_size for load in segments)
    stored_end = max(load.address + load.file_size for load in segments if load.file_size)
    output = bytearray((stored_end if resident else memory_end) - elf.entry)
    for load in segments:
        start = load.address - elf.entry
        output[start:start + load.file_size] = elf.slice(load.offset, load.file_size)
    for section in elf.named.values():
        if not section.size or section.name in external.values():
            continue
        start = section.address - elf.entry
        if start < 0 or start + section.size > memory_end - elf.entry:
            raise ValueError(f'{elf.path}: {section.name} lies outside the image')
        stored = output[start:start + section.size]
        if section.kind == 8 or section.name == '.bss':
            if section.name != '.bss' or any(stored) or (not resident and len(stored) != section.size):
                raise ValueError(f'{elf.path}: invalid BSS at {section.address:#010x}')
        elif stored != elf.contents(section):
            raise ValueError(f'{elf.path}: stored bytes differ in {section.name}')
    if resident:
        resident_startup(elf, output)
    elif name.startswith(('stg', 'boss_')) or name in ('chaostgentrance', 'chaostghero', 'chaostgneut'):
        if elf.defined.get('_end') != memory_end:
            raise ValueError(f'{elf.path}: terrain allocation starts inside or beyond the loaded image')
    elif name == 'chaostgkinder' and elf.defined.get('_kinder_sg_bss_end') != memory_end:
        raise ValueError(f'{elf.path}: Kindergarten land allocation starts at the wrong address')
    return output


def publish(name, root, prskit):
    layout = Image(name)
    elf = Elf(root / 'images' / (name + '.elf'), ET_EXEC)
    raw = payload(name, elf)
    output = root / layout.file
    output.parent.mkdir(parents=True, exist_ok=True)
    with tempfile.TemporaryDirectory(dir=output.parent) as directory:
        work = Path(directory)
        binary = work / 'image.bin'
        binary.write_bytes(raw)
        if layout.compressed:
            packed = work / 'image.prs'
            run([prskit, 'compress', binary, packed], check=True)
            # loader_read_prs reads complete GD sectors into the two-megabyte WORK_AREA.
            if packed.stat().st_size > 2 * 1024 * 1024:
                raise ValueError(f'{layout.file}: PRS exceeds the loader workspace')
            decoded = work / 'decoded.bin'
            run([prskit, 'decompress', packed, decoded], check=True)
            if decoded.read_bytes() != raw:
                raise ValueError(f'{layout.file}: PRS does not decode to the linked image')
            binary = packed
        optical = binary.read_bytes()
        row = (name, sha256(elf.data).hexdigest(), sha256(raw).hexdigest(), sha256(optical).hexdigest(), layout.file)
        # A receipt marks a finished publish, so a stop before save leaves
        # none rather than the old program's hashes beside the new one.
        receipt = root / 'receipts' / (name + '.txt')
        receipt.unlink(missing_ok=True)
        binary.replace(output)
        save(receipt, ('  '.join(row) + '\n').encode())


def main(argv=None):
    parser = argparse.ArgumentParser(prog='dc_publish', description=__doc__)
    parser.add_argument('image')
    parser.add_argument('root', type=Path, help='GNU build directory containing images/')
    parser.add_argument('--prskit', type=Path, default=Path('build/prskit'))
    args = parser.parse_args(argv)
    try:
        publish(args.image, args.root, args.prskit)
    except (OSError, ValueError, subprocess.CalledProcessError) as error:
        fail(error)


if __name__ == '__main__':
    main()
