"""Read packed Ninja motions and relocate their node tracks and player registrations."""
from struct import Struct

from config import fail
from kinds import KINDS


# NJS_MOTION and NJS_MDATA2/3/4 from sdk/include/ninja.h. Each track
# contains all channel pointers followed by the corresponding key counts.
HEADER = Struct('<IIHH')
TRACKS = {count: Struct('<' + 'I' * (2 * count)) for count in (2, 3, 4)}
KEY_FRAME = Struct('<I')
# NJS_MKEY_F and NJS_MKEY_A store a frame and three components.
# NJS_MKEY_A1 stores a frame and one camera angle. NJS_MKEY_P stores
# a frame and a pointer to the node's vertices or normals at that frame.
KEY_SIZES = {'position': 16, 'angles': 16, 'scale': 16, 'target': 16, 'roll': 8, 'fov': 8,
             'vertices': 8, 'normals': 8}
FACTORS = {
    0x003: ('position', 'angles'),
    0x007: ('position', 'angles', 'scale'),
    0x030: ('vertices', 'normals'),
    0x041: ('position', 'target'),
    0x0c1: ('position', 'target', 'roll'),
    0x1c1: ('position', 'target', 'roll', 'fov'),
}
# src/player/player.h: struct motion_entry.
ENTRY = Struct('<hhI')


def object_lines(symbol, size):
    return [f'.type {symbol}, @object', f'{symbol}:', f'.size {symbol}, {size}']


def shape_keys(data, first, count, addresses, filename, where):
    """Relocate each NJS_MKEY_P to a complete vertex or normal array of equal length."""
    lines = []
    size = None
    for offset in range(first, first + count * 8, 8):
        pointer, = KEY_FRAME.unpack_from(data, offset + 4)
        target = addresses.get(pointer)
        if target is None or target.kind != 'points':
            fail(f'{where}: shape key at {offset:#x} has no point array at {pointer:#010x}')
        if size is not None and target.size != size:
            fail(f'{where}: shape keys have different point counts')
        size = target.size
        lines.extend([f'.incbin "{filename}", {offset}, 4', f'.long {target.symbol}'])
    return lines


def motion(obj, data, base, filename, addresses):
    """The keys precede the node tracks and the final NJS_MOTION header."""
    header = obj.address + obj.size - HEADER.size
    tracks, frames, flags, method = HEADER.unpack_from(data, header - base)
    if obj.kind == 'player_motion' and (flags != 3 or method != 2):
        fail(f'{obj.where}: expected a position/angle motion with two channels')
    factors = FACTORS.get(flags)
    if not frames or factors is None or method != len(factors):
        fail(f'{obj.where}: unsupported motion with flags {flags:#x}, method {method} and {frames} frames')
    track = TRACKS[method]
    if not obj.address <= tracks < header or (header - tracks) % track.size:
        fail(f'{obj.where}: invalid NJS_MDATA{method} table at {tracks:#010x}')
    nodes = (header - tracks) // track.size
    keys = {}
    channels = []
    for node in range(nodes):
        at = tracks - base + node * track.size
        row = track.unpack_from(data, at)
        targets = []
        for kind, start, count in zip(factors, row[:method], row[method:]):
            if not start and not count:
                targets.append('0')
                continue
            if not count or start < obj.address or start + count * KEY_SIZES[kind] > tracks:
                fail(f'{obj.where}: node {node} has invalid {kind} keys')
            if start in keys:
                other_kind, other_count, name = keys[start]
                if (kind, count) != (other_kind, other_count):
                    fail(f'{obj.where}: incompatible keys at {start:#010x}')
            else:
                name = f'{obj.symbol}.joint_{node}_{kind}'
                keys[start] = kind, count, name
            targets.append(name)
        channels.append(targets)
    lines = []
    cursor = obj.address
    for start, (kind, count, name) in sorted(keys.items()):
        if start != cursor:
            fail(f'{obj.where}: expected the next key range at {cursor:#010x}, got {start:#010x}')
        key_size = KEY_SIZES[kind]
        previous = -1
        end = start - base + count * key_size
        for frame_at in range(start - base, end, key_size):
            frame, = KEY_FRAME.unpack_from(data, frame_at)
            # The Chaos reach animation repeats its last rotation key verbatim.
            repeated_end = (frame == previous == frames - 1 and frame_at + key_size == end
                            and data[frame_at - key_size:frame_at] == data[frame_at:end])
            if not repeated_end and not previous < frame < frames:
                fail(f'{obj.where}: {name} has invalid key frame {frame}')
            previous = frame
        size = count * key_size
        lines.extend(object_lines(name, size))
        if kind in ('vertices', 'normals'):
            lines.extend(shape_keys(data, start - base, count, addresses, filename, obj.where))
        else:
            lines.append(f'.incbin "{filename}", {start - base}, {size}')
        cursor += size
    if cursor != tracks:
        fail(f'{obj.where}: key ranges end before the node tracks')
    track_name = obj.symbol + '.tracks'
    lines.extend(object_lines(track_name, nodes * track.size))
    for node, targets in enumerate(channels):
        counts = tracks - base + node * track.size + method * 4
        lines.append('.long ' + ', '.join(targets))
        lines.append(f'.incbin "{filename}", {counts}, {method * 4}')
    lines.extend(object_lines(obj.symbol, HEADER.size))
    lines.extend([f'.long {track_name}', f'.incbin "{filename}", {header - base + 4}, 8'])
    return lines, header, nodes


def registry(obj, data, base, filename, motions):
    """Each slot carries the node count of the motion it installs. The final slot is -1."""
    lines = object_lines(obj.symbol, obj.size)
    stride = KINDS['motion_entries'].size
    for offset in range(0, obj.size, stride):
        at = obj.address - base + offset
        slot, nodes, pointer = ENTRY.unpack_from(data, at)
        if offset + stride == obj.size:
            if (slot, nodes, pointer) != (-1, 0, 0):
                fail(f'{obj.where}: missing final motion sentinel')
            target = '0'
        elif slot < 0 or pointer not in motions or nodes != motions[pointer][1]:
            fail(f'{obj.where}: slot {slot} has no matching {nodes}-node motion at {pointer:#010x}')
        else:
            target = motions[pointer][0]
        lines.extend([f'.incbin "{filename}", {at}, 4', f'.long {target}'])
    return lines
