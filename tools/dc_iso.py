"""Write SA2's three-track GDI from verified extracted files.

The ISO9660 directory records and path tables use absolute LBAs, including
on the high-density track. The boot program ends at the final data sector.
"""

import hashlib
from pathlib import PurePosixPath
import struct


SECTOR = 2048
HIGH = 45000
LEADOUT = 549150
ROOT = PurePosixPath('.')
BOOT = PurePosixPath('1ST_READ.BIN')
# Keep both volumes reproducible using the recording date from the extraction layout.
RECORD_DATE = bytes((101, 5, 29, 0, 0, 0, 0))
VOLUME_DATE = b'2001052900000000\0'


def sectors(size):
    return (size + SECTOR - 1) // SECTOR


def both(code, value):
    """ISO9660 stores these integers once in each byte order."""
    return struct.pack('<' + code, value) + struct.pack('>' + code, value)


def directory_record(name, lba, size, is_directory):
    length = 33 + len(name) + (len(name) % 2 == 0)
    if length > 255:
        raise ValueError(f'ISO9660 directory identifier is too long: {name!r}')
    record = bytearray(length)
    record[0] = length
    record[2:10] = both('I', lba)
    record[10:18] = both('I', size)
    record[18:25] = RECORD_DATE
    record[25] = 2 if is_directory else 0
    record[28:32] = both('H', 1)
    record[32] = len(name)
    record[33:33 + len(name)] = name
    return record


def directory_contents(path, children, starts, sizes):
    records = [directory_record(b'\0', starts[path], sizes[path], True),
               directory_record(b'\1', starts[path.parent], sizes[path.parent], True)]
    for child in children[path]:
        is_directory = child in children
        name = child.name.encode('ascii') + (b'' if is_directory else b';1')
        records.append(directory_record(name, starts[child], sizes[child], is_directory))
    directory = bytearray()
    for record in records:
        # Directory records cannot straddle a logical sector.
        if len(directory) % SECTOR + len(record) > SECTOR:
            directory.extend(bytes(-len(directory) % SECTOR))
        directory.extend(record)
    directory.extend(bytes(-len(directory) % SECTOR))
    return directory


def path_table(directories, starts, endian):
    numbers = {path: number for number, path in enumerate(directories, 1)}
    table = bytearray()
    for path in directories:
        name = b'\0' if path == ROOT else path.name.encode('ascii')
        table.extend(struct.pack(endian + 'BBIH', len(name), 0, starts[path], numbers[path.parent]))
        table.extend(name)
        if len(name) % 2:
            table.append(0)
    return table


def volume_descriptor(length, table_size, little_lba, big_lba, root_record):
    volume = bytearray(SECTOR)
    volume[:7] = b'\x01CD001\x01'
    volume[8:40] = b'SEGA SEGAKATANA'.ljust(32)
    volume[40:72] = b'SONIC2'.ljust(32)
    volume[80:88] = both('I', length)
    volume[120:124] = both('H', 1)
    volume[124:128] = both('H', 1)
    volume[128:132] = both('H', SECTOR)
    volume[132:140] = both('I', table_size)
    struct.pack_into('<I', volume, 140, little_lba)
    struct.pack_into('>I', volume, 148, big_lba)
    volume[156:190] = root_record
    volume[190:813] = b' ' * 623
    volume[813:830] = volume[830:847] = volume[864:881] = VOLUME_DATE
    volume[847:864] = b'0' * 16 + b'\0'
    volume[881] = 1
    return volume


def write_volume(target, base, end, files, expected, bootstrap=b''):
    directories = sorted({ROOT} | {parent for path in files for parent in path.parents},
                         key=lambda path: (len(path.parts), path.as_posix()))
    children = {path: [] for path in directories}
    if children.keys() & files.keys():
        raise ValueError('ISO9660 path names both a file and a directory')
    for path in [*directories[1:], *files]:
        children[path.parent].append(path)
    for entries in children.values():
        entries.sort(key=lambda path: path.name)
    sizes = {path: 0 for path in directories}
    for path, source in files.items():
        size = source.stat().st_size
        if size > (end - base) * SECTOR:
            raise ValueError(f'{path} exceeds the track capacity')
        sizes[path] = size
    starts = dict.fromkeys(sizes, 0)
    for path in directories:
        sizes[path] = len(directory_contents(path, children, starts, sizes))

    table_size = len(path_table(directories, starts, '<'))
    little_lba = base + 18
    big_lba = little_lba + sectors(table_size)
    next_lba = big_lba + sectors(table_size)
    for path in [*directories, *sorted(files.keys() - {BOOT})]:
        starts[path] = next_lba
        next_lba += sectors(sizes[path])
    first_boot_sector = end
    if BOOT in files:
        first_boot_sector -= sectors(sizes[BOOT])
        starts[BOOT] = first_boot_sector
    if next_lba > first_boot_sector:
        raise ValueError(f'{target}: files exceed the available data sectors')

    with target.open('xb') as track:
        def put(lba, data):
            track.seek((lba - base) * SECTOR)
            track.write(data)

        track.truncate((end - base) * SECTOR)
        put(base, bootstrap)
        root = directory_record(b'\0', starts[ROOT], sizes[ROOT], True)
        # The volume size counts from the track's first sector, though every extent is
        # disc-absolute. The first Sonic Adventure's retail data track advertises 549150 - 45000.
        put(base + 16, volume_descriptor(end - base, table_size, little_lba, big_lba, root))
        put(base + 17, b'\xffCD001\x01')
        put(little_lba, path_table(directories, starts, '<'))
        put(big_lba, path_table(directories, starts, '>'))
        for path in directories:
            put(starts[path], directory_contents(path, children, starts, sizes))
        for path in sorted(files):
            track.seek((starts[path] - base) * SECTOR)
            digest = hashlib.sha256()
            remaining = sizes[path]
            with files[path].open('rb') as source:
                for chunk in iter(lambda: source.read(1024 * 1024), b''):
                    remaining -= len(chunk)
                    if remaining < 0:
                        raise ValueError(f'{path}: changed size after layout')
                    track.write(chunk)
                    digest.update(chunk)
            if remaining:
                raise ValueError(f'{path}: changed size after layout')
            if digest.hexdigest() != expected[str(path)]:
                raise ValueError(f'{path}: stored bytes differ from the expected SHA-256')
    return {str(path): (starts[path], sizes[path]) for path in files}


def write_disc(directory, files, expected, bootstrap):
    """Write the four GDI files in directory and return the encoded file extents.

    The files must not exist yet. A failure leaves whatever was written, so the
    caller writes into a fresh directory and discards it on failure.
    """
    if files.keys() != expected.keys() or 'IP.BIN' not in files or str(BOOT) not in files:
        raise ValueError('the optical inventory must name every file, including IP.BIN and 1ST_READ.BIN')
    if len(bootstrap) != 16 * SECTOR or hashlib.sha256(bootstrap).hexdigest() != expected['IP.BIN']:
        raise ValueError('bootstrap differs from the recorded IP.BIN')
    paths = {}
    for name, source in files.items():
        path = PurePosixPath(name)
        if path == ROOT or path.is_absolute() or '..' in path.parts or str(path) != name:
            raise ValueError(f'{name}: expected a relative disc path')
        paths[path] = source
    write_volume(directory / 'track01.bin', 0, 300, {}, {})
    with (directory / 'track02.raw').open('xb') as audio:
        audio.truncate(300 * 2352)
    extents = write_volume(directory / 'track03.bin', HIGH, LEADOUT, paths, expected, bootstrap)
    with (directory / 'disc.gdi').open('x', encoding='ascii', newline='\n') as image:
        image.write('3\n1 0 4 2048 track01.bin 0\n2 450 0 2352 track02.raw 0\n'
                    f'3 {HIGH} 4 2048 track03.bin 0\n')
    return extents
