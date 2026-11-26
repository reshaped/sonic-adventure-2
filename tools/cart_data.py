"""Relocate the kart course piece table and the original writable texture animation records."""

from bisect import bisect_right
import struct

import cart_polygons
from config import fail
from kinds import KINDS


# the arrays of modules/cart/track.h and texture_remap.h whose pointers pointers() resolves
ARRAYS = {'cart_pieces', 'cart_uv_patches', 'cart_texture_patches', 'cart_patch_list', 'cart_texture_remaps',
          'cart_gate_lists'}


def values(obj, data, base, record=0):
    start = obj.address - base
    if start < 0 or start + obj.size > len(data):
        fail(f'{obj.where}: original image too short for {obj.name}')
    return struct.unpack_from(f'<{KINDS[obj.kind].size // 4}I', data, start + record)


def patch_fields(entries, data, base):
    """Use the owning model's vertex indices to identify TID words and UVs inside its polygon list."""
    required = set()
    for obj in entries.values():
        if obj.kind == 'cart_texture_patches':
            for offset in range(0, obj.size, KINDS[obj.kind].size):
                required.add(values(obj, data, base, offset)[1])
    if not required:
        return {}
    models = {}
    for model in entries.values():
        if model.kind == 'chunk_model' and '-' not in model.targets:
            vertices, polygons = (entries[name] for name in model.targets)
            models[polygons.address] = polygons, vertices
    starts = sorted(models)
    used = set()
    result = {}
    for word in sorted(required):
        index = bisect_right(starts, word) - 1
        if index < 0 or word >= starts[index] + models[starts[index]][0].size:
            fail(f'kart texture word {word:#010x} is outside the course polygon lists')
        polygons, vertices = models[starts[index]]
        if polygons.address not in used:
            result.update((field.address, field) for field in cart_polygons.patch_fields(polygons, vertices, data, base))
            used.add(polygons.address)
    return result


def polygon_spans(root, entries):
    """The animation can patch any model in this Ninja object's child and sibling hierarchy."""
    todo = [root]
    seen = set()
    while todo:
        node = todo.pop()
        if node.name in seen:
            continue
        seen.add(node.name)
        model, child, sibling = node.targets
        if model != '-':
            polygon = entries.get(entries[model].targets[1])
            if polygon:
                yield polygon.address, polygon.address + polygon.size
        todo.extend(entries[name] for name in (child, sibling) if name != '-')


def pointers(obj, data, base, addresses, entries):
    """Yield real symbol links or nulls. Counted arrays must span their referenced objects."""
    def target(address, kind, required=True):
        item = addresses.get(address)
        if not address and not required:
            return None
        if item is None or item.kind != kind:
            fail(f'{obj.where}: {obj.name} expects {kind} at {address:#010x}')
        return item

    def counted(address, kind, count):
        item = target(address, kind, required=bool(count))
        if item is not None and item.size != count * KINDS[kind].size:
            fail(f'{obj.where}: {obj.name} expects {count} {kind} elements')
        return item

    for at in range(0, obj.size, KINDS[obj.kind].size):
        row = values(obj, data, base, at)
        if obj.kind == 'cart_pieces':
            draw, solid = row[:2]
            compiled, signs, count = row[24:]
            if compiled or bool(draw) != bool(solid):
                fail(f'{obj.where}: {obj.name} expects paired draw and collision roots and an empty compiled slot')
            yield at, target(draw, 'chunk_object', False)
            yield at + 4, target(solid, 'basic_object', False)
            yield at + 96, None
            yield at + 100, counted(signs, 'cart_signs', count)
        elif obj.kind == 'cart_gate_lists':
            gates, count = row
            yield at, counted(gates, 'cart_gates', count)
        elif obj.kind == 'cart_uv_patches':
            uv, original = row
            field = target(uv, 'cart_uv')
            if struct.unpack_from('<I', data, uv - base)[0] != original:
                fail(f'{obj.where}: {obj.name} saved UV differs from its original strip vertex')
            yield at, field
        elif obj.kind == 'cart_texture_patches':
            texture, word, count, uvs = row
            field = target(word, 'cart_tid')
            if struct.unpack_from('<H', data, word - base)[0] & 0x1fff != texture:
                fail(f'{obj.where}: {obj.name} texture ID differs from the original polygon word')
            patches = counted(uvs, 'cart_uv_patches', count)
            if patches is not None:
                for i in range(0, patches.size, KINDS['cart_uv_patches'].size):
                    uv = values(patches, data, base, i)[0]
                    if target(uv, 'cart_uv').texture_word != word:
                        fail(f'{obj.where}: {obj.name} UV belongs to a different texture word')
            yield at + 4, field
            yield at + 12, patches
        elif obj.kind == 'cart_patch_list':
            yield at, target(row[0], 'cart_texture_patches')
        elif obj.kind == 'cart_texture_remaps':
            root, count, list_address = row
            if not root:
                if count or list_address or at + 12 != obj.size:
                    fail(f'{obj.where}: {obj.name} has an invalid terminal record')
                yield at, None
                yield at + 8, None
                continue
            if not count or at + 12 == obj.size:
                fail(f'{obj.where}: {obj.name} requires counted patches followed by a null sentinel')
            node = target(root, 'chunk_object')
            patches = counted(list_address, 'cart_patch_list', count)
            owned = tuple(polygon_spans(node, entries))
            for i in range(0, patches.size, KINDS['cart_patch_list'].size):
                patch = target(values(patches, data, base, i)[0], 'cart_texture_patches')
                word = values(patch, data, base)[1]
                if not any(start <= word < end for start, end in owned):
                    fail(f'{obj.where}: {obj.name} texture word is outside its object hierarchy')
            yield at, node
            yield at + 8, patches
        else:
            fail(f'{obj.where}: {obj.name} is a {obj.kind}, which has no kart pointer decoder')
