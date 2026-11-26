"""Find the texture words and vertex UVs patched by the kart course objects."""

from typing import NamedTuple

from config import fail
from kinds import (NJD_CM_D, NJD_CM_DA, NJD_CS_UVH, NJD_CS_UVN, NJD_CT_TID, NJD_CV_D8, NJD_CV_VN, ChunkError, strips,
                   vertex_range, walk)


# the material chunks a kart prop holds, by the halfwords after their size
MATERIAL_HALVES = {NJD_CM_D: 2, NJD_CM_DA: 4}
# the vertex chunks a kart prop holds
VERTEX_TYPES = {NJD_CV_D8, NJD_CV_VN}


class Field(NamedTuple):
    address: int
    symbol: str
    kind: str
    # the address of an NJD_CT_TID's texture word: a cart_tid field's own, and on a
    # cart_uv field the one in force for its strip
    texture_word: int


def vertex_indices(vertices, data, base):
    """The indices of a kart prop's vertices, which one vertex chunk holds before the list's end."""
    start = vertices.address - base
    if vertices.kind != 'chunk_vertices' or start < 0 or vertices.size < 12 or start + vertices.size > len(data):
        fail(f'{vertices.where}: expected a complete kart prop vertex list')
    try:
        words, chunks = walk('chunk_vertices', data[start:start + vertices.size])
        if len(chunks) != 2 or chunks[0].type not in VERTEX_TYPES:
            fail(f'{vertices.where}: invalid kart prop vertex extent')
        indices = vertex_range(words, chunks[0])
    except ChunkError as error:
        fail(f'{vertices.where}: invalid kart prop vertex extent: {error}')
    if not indices or indices.stop > 0x10000:
        fail(f'{vertices.where}: invalid kart prop vertex extent')
    return indices


def patch_fields(polygon, vertices, data, base):
    """Return writable fields from one prop model's paired polygon and vertex lists."""
    start = polygon.address - base
    if (polygon.kind != 'chunk_polygons' or polygon.section != 'D' or polygon.size <= 0
            or polygon.size % 2 or start < 0 or start + polygon.size > len(data)):
        fail(f'{polygon.where}: expected a complete writable kart polygon list')
    indices = vertex_indices(vertices, data, base)

    def field(half, kind, texture_word):
        offset = 2 * half
        return Field(polygon.address + offset, f'{polygon.symbol}+{offset}', kind, texture_word)

    try:
        halves, chunks = walk('chunk_polygons', data[start:start + polygon.size])
    except ChunkError as error:
        fail(f'{polygon.where}: invalid kart polygon list: {error}')
    fields = []
    texture_word = None
    for chunk in chunks[:-1]:
        end = chunk.offset + chunk.extent
        if chunk.type == NJD_CT_TID:
            texture_word = polygon.address + 2 * (chunk.offset + 1)
            fields.append(field(chunk.offset + 1, 'cart_tid', texture_word))
            continue
        if chunk.type in MATERIAL_HALVES and chunk.extent == 2 + MATERIAL_HALVES[chunk.type]:
            continue
        if chunk.type not in (NJD_CS_UVN, NJD_CS_UVH) or texture_word is None or chunk.extent == 2:
            fail(f'{polygon.where}: unsupported kart polygon chunk at byte {2 * chunk.offset}')
        try:
            group = list(strips(halves, chunk))
        except ChunkError as error:
            fail(f'{polygon.where}: invalid kart polygon list: {error}')
        if not group:
            fail(f'{polygon.where}: empty kart strip group')
        for strip in group:
            if strip.length < 3:
                fail(f'{polygon.where}: kart strip at byte {2 * strip.offset} has fewer than three vertices')
            for vertex in strip.vertices():
                if halves[vertex] not in indices:
                    fail(f'{polygon.where}: kart strip index {halves[vertex]} outside its vertex list')
                fields.append(field(vertex + 1, 'cart_uv', texture_word))
        # a strip chunk may end with one zero halfword, which ends the chunk on a
        # word from the list's start
        at = group[-1].offset + group[-1].extent
        if at != end and (at + 1 != end or at % 2 == 0 or halves[at] != 0):
            fail(f'{polygon.where}: extra words in a kart strip')
    return fields
