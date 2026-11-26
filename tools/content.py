"""Supply a GNU image with its declared disc data and relocate Ninja object links."""

import argparse
from bisect import bisect_right
from pathlib import Path
import re
import struct
from typing import NamedTuple

import cart_data
import chunk_poses
import land_data
import ninja_motions
import stage_paths
import texture_data
from config import CHAO_AREAS, DISC, MAIN, Image, NAME, address, aligned, content, fail, named, rows, save
from kinds import KINDS, Choice


LAYOUT_NAME = re.compile(r'\.?' + NAME.pattern)

# the arrays whose elements each hold one pointer, at their start, which
# object_array_pointers resolves to an object of this image or of one it imports
POINTER_ARRAYS = {'chunk_object_ptrs', 'chunk_object_tables', 'chunk_model_ptrs', 'ninja_motion_ptrs',
                  'texture_list_ptrs', 'texture_group', 'texture_groups', 'uv_scroll_ptrs', 'path_ptrs',
                  'text_ptrs', 'personal_toys', 'enemy_actions', 'motion_clips'}
# the directive that opens each section the assembly holds an object in
DIRECTIVES = {'.rodata': '.section .rodata', '.data': '.data'}


class Object(NamedTuple):
    name: str
    address: int
    size: int
    alignment: int
    padding: int
    section: str
    kind: str
    targets: tuple
    where: str
    filename_widths: tuple = ()
    path_step_address: int = 0

    @property
    def symbol_address(self):
        if self.kind in ('player_motion', 'ninja_motion'):
            return self.address + self.size - ninja_motions.HEADER.size
        return self.address

    @property
    def public(self):
        return not self.name.startswith('.')

    @property
    def symbol(self):
        return '_' + self.name if self.public else '.L' + self.name[1:]


def layout(kind, fields, where):
    if kind in chunk_poses.BANK_KINDS:
        return chunk_poses.layout(kind, fields, where)
    if kind in ('player_motion', 'ninja_motion'):
        if len(fields) != 1 or not re.fullmatch(r'[1-9][0-9]{0,9}', fields[0]):
            fail(f'{where}: {kind} needs its byte extent')
        size = int(fields[0])
        step = ninja_motions.TRACKS[2].size if kind == 'player_motion' else 4
        if size < ninja_motions.HEADER.size + ninja_motions.TRACKS[2].size or (size - ninja_motions.HEADER.size) % step:
            fail(f'{where}: invalid motion extent {size}')
        return size, 4, ()
    if kind == 'motion_entries':
        if len(fields) != 1 or not re.fullmatch(r'[1-9][0-9]{0,9}', fields[0]):
            fail(f'{where}: motion_entries needs its slot count, including the sentinel')
        return int(fields[0]) * KINDS[kind].size, KINDS[kind].alignment, ()
    if kind == 'texture_list' and len(fields) > 1:
        return land_data.texture_slice(fields, where)
    if kind == 'texture_filenames':
        return land_data.fixed_filenames(fields, where)
    if kind == 'texture_filename' and len(fields) == 2:
        if not all(re.fullmatch(r'[1-9][0-9]{0,8}', value) for value in fields):
            fail(f'{where}: a fixed filename array needs its capacity and string length')
        capacity, length = map(int, fields)
        if length > capacity:
            fail(f'{where}: filename including its terminator exceeds its array')
        return capacity, KINDS['texture_filename'].alignment, (str(length),)
    if kind in land_data.BANK_STRINGS:
        return land_data.string_bank_layout(kind, fields, where)
    if kind in land_data.ARRAYS:
        return land_data.layout(kind, fields, where)
    if kind == 'texture_slices':
        if len(fields) != 2 or not re.fullmatch(r'[1-9][0-9]{0,9}', fields[0]):
            fail(f'{where}: texture_slices needs a list count and a texture_names array')
        texture_list = KINDS['texture_list']
        return int(fields[0]) * texture_list.size, texture_list.alignment, (fields[1],)
    if kind not in KINDS:
        fail(f'{where}: {kind}: unknown data kind')
    element = KINDS[kind]
    if not element.record:
        if len(fields) != 1 or not re.fullmatch(r'[1-9][0-9]{0,9}', fields[0]):
            fail(f'{where}: {kind} needs a positive decimal element count')
        return element.size * int(fields[0]), element.alignment, ()
    if len(fields) != len(element.pointers):
        fail(f"{where}: {kind} needs pointer targets for {', '.join(field.name for field in element.pointers)}")
    return element.size, element.alignment, tuple(fields)


def declaration(where, fields, scope=''):
    if len(fields) < 2 or not LAYOUT_NAME.fullmatch(fields[0]):
        fail(f'{where}: expected ADDRESS NAME KIND COUNT or typed pointer targets')
    name, kind, *arguments = fields
    if kind == 'player_motion' and not name.startswith('.'):
        fail(f'{where}: a packed player motion has a private header at the end of its keys and tracks')
    size, alignment, targets = layout(kind, arguments, where)
    if scope:
        def scoped(label):
            return '.' + scope + label if label.startswith('.') else label
        name = scoped(name)
        targets = tuple(scoped(target) for target in targets)
    return name, kind, size, alignment, targets


def declarations(image):
    """An image can place a single object or a complete, shared member layout."""
    for where, fields in rows(image.path / 'data.txt'):
        if fields[0] == 'import':
            continue
        start = address(fields[0], where)
        if len(fields) != 3 or fields[1] != 'layout':
            yield where, start, None, declaration(where, fields[1:])
            continue
        name = fields[2]
        if not NAME.fullmatch(name):
            fail(f'{where}: invalid layout name {name}')
        path = Path('layouts') / (name + '.txt')
        cursor = start
        for line, member in rows(path):
            item = declaration(line, member, name)
            _, _, size, alignment, _ = item
            cursor = aligned(cursor, alignment)
            yield line, cursor, start, item
            cursor += size
        if cursor == start:
            fail(f'{where}: empty layout {path}')


def objects(image):
    """Declarations cover each claimed native disc placement up to inter-object alignment."""
    path = image.path / 'data.txt'
    symbols = set(named(image.path / 'symbols.txt'))
    disc = [row for row in image.disc() if row.section in ('C', 'D', 'DATA')]
    starts = [row.start for row in disc]
    ends = {}
    sources = [row for row in image.placements if row.writer != DISC]
    result = {}
    previous_end = image.base
    layouts = {}
    for where, start, layout_start, (name, kind, size, alignment, targets) in declarations(image):
        end = start + size
        if start % alignment:
            fail(f'{where}: {name} needs {alignment}-byte alignment')
        if start < previous_end:
            fail(f'{where}: {name} overlaps or precedes the previous placement')
        if name in result:
            fail(f'{where}: {name} already declared at {result[name].where}')
        if kind in land_data.BANK_STRINGS:
            obj = Object(name, start, size, alignment, 0, 'C', kind, targets, where)
            if (start, name) not in symbols:
                fail(f'{where}: {name} has no symbol at {start:#010x} in {image.name}')
            widths = land_data.string_bank_provider(image, obj)
            result[name] = obj._replace(filename_widths=widths)
            previous_end = end
            continue
        index = bisect_right(starts, start) - 1
        if index < 0 or end > disc[index].end:
            fail(f'{where}: {name} must fit within one disc C, D or DATA range in {image.splits}')
        placement = disc[index]
        if kind == 'texture_names' and targets[-1:] == ('empty',) and placement.section != 'D':
            fail(f'{where}: {name} needs a writable D range for its empty texture slots')
        if kind == 'model_builds' and placement.section == 'C':
            fail(f'{where}: {name} needs writable storage for compiled models')
        if layout_start is not None and placement.start != layout_start:
            fail(f'{where}: layout at {layout_start:#010x} must own one complete native placement')
        if placement in layouts and layout_start != layouts[placement]:
            fail(f'{where}: object and layout declarations share the range at {image.where[placement]}')
        layouts[placement] = layout_start
        cursor = ends.get(placement, placement.start)
        next_start = aligned(cursor, alignment) if placement in ends else cursor
        if start != next_start:
            fail(f'{where}: {name} starts at {start:#010x}, expected {next_start:#010x} '
                 f'to cover the disc range at {image.where[placement]}')
        ends[placement] = end
        for row in sources:
            if start < row.end and row.start < end:
                fail(f'{where}: {name} overlaps {row.writer} at {image.where[row]}')
        obj = Object(name, start, size, alignment, start - cursor, placement.section, kind, targets, where)
        if obj.public and (obj.symbol_address, name) not in symbols:
            fail(f'{where}: {name} has no symbol at {obj.symbol_address:#010x} in {image.name}')
        if kind == 'stage_path' and targets[1] != '-':
            obj = obj._replace(path_step_address=stage_paths.step_address(image, targets[1], where))
        result[name] = obj
        previous_end = end
    if not result:
        fail(f'{path}: no data rows')
    for placement, end in ends.items():
        if end != placement.end:
            fail(f'{path}: declarations end at {end:#010x}, short of disc range at {image.where[placement]}')
    members = pose_members(result)
    for obj in result.values():
        if obj.kind == 'texture_slices':
            target = result.get(obj.targets[0])
            if not target or target.kind != 'texture_names':
                fail(f'{obj.where}: {obj.name} needs a declared texture_names array')
        if obj.kind in KINDS and KINDS[obj.kind].record:
            for field, target in zip(KINDS[obj.kind].pointers, obj.targets):
                item = result.get(target) or members.get(target)
                member = field.member(obj.name)
                kind = field.target
                if obj.kind == 'texture_list' and len(obj.targets) == 3 and item is None:
                    fail(f'{obj.where}: {obj.name} needs a declared texture name array')
                if field.name == 'pvm' and item and item.kind == 'land_filenames':
                    continue
                if kind == 'path_step':
                    continue
                if isinstance(kind, Choice):
                    if item is None or item.kind not in kind.kinds:
                        fail(f"{obj.where}: {member} needs a declared {' or '.join(sorted(kind.kinds))}, got {target}")
                    continue
                if kind is None and target != '-':
                    fail(f'{obj.where}: {member} must be null')
                if target != '-' and (item is None or item.kind != kind):
                    fail(f'{obj.where}: {member} needs a declared {kind} or -, got {target}')
    return result


def imported_objects(image, entries):
    """Resolve each optical pointer-table import against both images' native declarations.
    An import of a kind that no pointer table of the image takes fails."""
    symbols = set(named(image.path / 'symbols.txt'))
    taken = {field.target for obj in entries.values() if obj.kind in POINTER_ARRAYS
             for field in KINDS[obj.kind].pointers}
    providers = {}
    result = {}
    for where, fields in rows(image.path / 'data.txt'):
        if fields[0] != 'import':
            continue
        if (len(fields) != 3 or (fields[1] != MAIN and not NAME.fullmatch(fields[1]))
                or not NAME.fullmatch(fields[2])):
            fail(f'{where}: expected import PROVIDER PUBLIC_OBJECT')
        _, provider, name = fields
        if provider == image.name or name in entries:
            fail(f'{where}: {name} must come from a separate provider')
        if provider != MAIN and (provider != 'chao' or image.name not in CHAO_AREAS):
            fail(f'{where}: {provider} is not loaded beside {image.name}')
        if provider not in providers:
            providers[provider] = objects(Image(provider))
        obj = providers[provider].get(name)
        if obj is None or not obj.public or (obj.symbol_address, name) not in symbols:
            fail(f'{where}: {provider}.{name} must have the same public symbol address in {image.name}')
        if obj.kind not in taken:
            fail(f'{where}: {provider}.{name} is a {obj.kind}, which no pointer table of {image.name} takes')
        if obj.symbol_address in result:
            fail(f'{where}: repeated import at {obj.symbol_address:#010x}')
        result[obj.symbol_address] = obj
    return result


def pose_members(entries):
    """Every pose bank's generated members by name. No name may repeat a declaration or another member."""
    members = {}
    for bank in entries.values():
        if bank.kind in chunk_poses.BANK_KINDS:
            for name, member in chunk_poses.fields(bank):
                if name in entries or name in members:
                    fail(f'{bank.where}: {bank.name} generates {name}, which is already declared or generated')
                members[name] = member
    return members


def object_array_pointers(obj, data, start, addresses):
    """Each element's pointer, which starts the element, to a declared object of its field's kind."""
    (field,) = KINDS[obj.kind].pointers
    stride, target_kind = KINDS[obj.kind].size, field.target
    for index, offset in enumerate(range(0, obj.size, stride)):
        word, = struct.unpack_from('<I', data, start + offset)
        if obj.kind in ('texture_group', 'texture_groups') and (word == 0) != (offset + stride == obj.size):
            fail(f'{obj.where}: {obj.name} needs one terminating null pointer at its end')
        if obj.kind == 'personal_toys' and (not word or struct.unpack_from('<I', data, start + offset + 4)[0]):
            fail(f'{obj.where}: {obj.name}[{index}] needs a model and a zero reserved field')
        target = addresses.get(word)
        if word and (target is None or target.kind != target_kind):
            fail(f'{obj.where}: {obj.name}[{index}] points to {word:#010x}, expected a {target_kind}')
        yield offset, target


class Pointer(NamedTuple):
    address: int
    symbol: str


def record_pointers(obj, entries, data, start, original, addresses):
    if obj.kind in KINDS and KINDS[obj.kind].record:
        for field, target in zip(KINDS[obj.kind].pointers, obj.targets):
            offset, member = field.offset, field.member(obj.name)
            item = entries.get(target)
            expected = item.symbol_address if item else 0
            if item and item.kind == 'land_filenames' and len(item.targets) == 2:
                expected += int(item.targets[1])
                item = addresses.get(expected)
                if not item or item.kind != 'texture_filename':
                    fail(f'{obj.where}: {member} needs the start of an archive filename')
            if obj.kind == 'stage_path' and field.name == 'step' and obj.path_step_address:
                expected = obj.path_step_address
                item = Pointer(expected, '_' + target)
            if obj.kind == 'texture_list' and len(obj.targets) == 3:
                addend = int(obj.targets[1]) * KINDS['texture_names'].size
                expected += addend
                if addend:
                    item = Pointer(expected, f'{item.symbol}+{addend}')
            stored, = struct.unpack_from('<I', data, start + offset)
            if stored != expected:
                fail(f'{obj.where}: {member} in {original} points to {stored:#010x}, '
                     f'expected {expected:#010x} ({target})')
            yield offset, item


def sections(entries):
    """Yield each object the assembly holds, with its section and its offset
    there, in the assembly's order. An object of a C placement lies in
    .rodata and any other in .data, each at its alignment past the one
    before. A bank is its source's datum, and the assembly holds none."""
    ends = {}
    for obj in entries.values():
        if obj.kind in land_data.BANK_STRINGS:
            continue
        section = '.rodata' if obj.section == 'C' else '.data'
        offset = aligned(ends.get(section, 0), obj.alignment)
        ends[section] = offset + obj.size
        yield obj, section, offset


def assembly(entries, original, base, imports=None):
    """Keep scalar bytes from the verified image. Rebind every supported pointer field."""
    data = content(original)
    filename = str(original)
    if any(ord(c) < 32 for c in filename):
        fail(f'{filename!r}: control character in assembly filename')
    filename = filename.replace('\\', '\\\\').replace('"', '\\"')
    lines = []
    motions = {}
    addresses = {obj.symbol_address: obj for obj in entries.values()}
    members = pose_members(entries)
    addresses.update((member.address, member) for member in members.values())
    targets = {**entries, **members}
    strings = {}
    for obj in entries.values():
        if obj.kind in land_data.BANK_STRINGS or obj.kind == 'texture_filenames':
            strings.update((name.address, name) for name in land_data.bank_strings(obj, data, base))
    addresses.update(strings)
    addresses.update(cart_data.patch_fields(entries, data, base))
    imports = imports or {}
    if addresses.keys() & imports.keys():
        fail(f'{original}: imported object shares an address with local data')
    array_targets = {**addresses, **imports} if imports else addresses
    unused_imports = {obj.symbol for obj in imports.values()}
    for obj, section, _ in sections(entries):
        start = obj.address - base
        if start + obj.size > len(data):
            fail(f'{original}: too short for {obj.name} at {start:#x} ({obj.size} bytes)')
        if any(data[start - obj.padding:start]):
            fail(f'{obj.where}: nonzero alignment before {obj.name} in {original}')
        lines.extend([DIRECTIVES[section], f'.balign {obj.alignment}'])
        if obj.public:
            lines.append(f'.global {obj.symbol}')
        if obj.kind in ('player_motion', 'ninja_motion'):
            body, header, nodes = ninja_motions.motion(obj, data, base, filename, addresses)
            motions[header] = obj.symbol, nodes
            lines.extend(body)
            continue
        if obj.kind == 'motion_entries':
            lines.extend(ninja_motions.registry(obj, data, base, filename, motions))
            continue
        lines.extend([f'.type {obj.symbol}, @object', f'{obj.symbol}:'])
        cursor = 0
        if obj.kind == 'texture_uv_keys':
            texture_data.check_uvs(obj, data, start)
        if obj.kind == 'uv_scroll' and (not struct.unpack_from('<I', data, start)[0] & 2 or obj.targets[1] == '-'):
            fail(f'{obj.where}: a UV scroll needs the scroll flag and UV keys')
        if obj.kind == 'stage_path':
            stage_paths.check_points(obj, entries[obj.targets[0]], data, start)
        land_data.check_record(obj, data, start, entries)
        if obj.kind in land_data.ARRAYS:
            pointers = land_data.array_pointers(obj, data, start, addresses)
        elif obj.kind in chunk_poses.BANK_KINDS:
            pointers = chunk_poses.pointers(obj, data, start, members, addresses)
        elif obj.kind == 'texture_slices':
            pointers = texture_data.slice_pointers(obj, entries[obj.targets[0]], data, start)
        elif obj.kind in cart_data.ARRAYS:
            pointers = cart_data.pointers(obj, data, base, addresses, entries)
        elif obj.kind in POINTER_ARRAYS:
            pointers = object_array_pointers(obj, data, start, array_targets)
        else:
            pointers = record_pointers(obj, targets, data, start, original, addresses)
        for offset, item in pointers:
            if not cursor <= offset <= obj.size - 4:
                fail(f'{obj.where}: {obj.name} has a pointer at offset {offset}, '
                     f'out of order or past the end of its {obj.size} bytes')
            if item and unused_imports:
                unused_imports.discard(item.symbol)
            if item and strings:
                strings.pop(item.address, None)
            if cursor < offset:
                lines.append(f'.incbin "{filename}", {start + cursor}, {offset - cursor}')
            lines.append(f'.long {item.symbol if item else 0}')
            cursor = offset + 4
        if cursor < obj.size:
            lines.append(f'.incbin "{filename}", {start + cursor}, {obj.size - cursor}')
        lines.append(f'.size {obj.symbol}, . - {obj.symbol}')
    if unused_imports:
        fail(f'{original}: no pointer table references the imported object {min(unused_imports)}')
    if strings:
        first = strings[min(strings)]
        if first.kind == 'text':
            fail(f'{original}: string at {first.address:#010x} has no text reference')
        fail(f'{original}: filename at {first.address:#010x} has no terrain or texture reference')
    return ('\n'.join(lines) + '\n').encode()


def main(argv=None):
    parser = argparse.ArgumentParser(prog='content', description=__doc__)
    parser.add_argument('image')
    parser.add_argument('original', nargs='?', type=Path,
                        help='decoded and verified disc image, which the assembly reads in place by this path: '
                             'keep it unchanged, and assemble from this directory unless the path is absolute')
    parser.add_argument('output', nargs='?', type=Path, help='GNU assembly to write')
    args = parser.parse_args(argv)
    if args.original is not None and args.output is None:
        parser.error('an original image needs an output path')
    if args.output is not None and args.output.exists() and args.original.exists() \
            and args.output.samefile(args.original):
        parser.error('the output would replace the original image it is made from')
    image = Image(args.image)
    entries = objects(image)
    imports = imported_objects(image, entries)
    if args.original is not None:
        save(args.output, assembly(entries, args.original, image.base, imports))


if __name__ == '__main__':
    main()
