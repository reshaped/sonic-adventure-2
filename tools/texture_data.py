"""Read the short UV-key lists used by chunk texture animations."""
import struct
from typing import NamedTuple

from config import fail
from kinds import KINDS


def check_uvs(obj, data, start):
    """An entry holds a polygon offset, a vertex count and UV pairs for each key.
    The caller has bounded the object within data, in whole shorts."""
    words = [value for value, in struct.iter_unpack('<h', data[start:start + obj.size])]
    if len(words) < 2 or min(words[:2]) <= 0:
        fail(f'{obj.where}: invalid UV entry or key count')
    entries, keys = words[:2]
    cursor = 2
    for _ in range(entries):
        if cursor + 2 > len(words):
            fail(f'{obj.where}: incomplete texture UV entry')
        polygon, vertices = words[cursor:cursor + 2]
        if polygon < 0 or vertices <= 0:
            fail(f'{obj.where}: invalid UV polygon offset or vertex count')
        cursor += 2 + vertices * keys * 2
    if cursor != len(words):
        fail(f'{obj.where}: UV keys do not cover their declared extent')


class TexturePointer(NamedTuple):
    address: int
    symbol: str


def slice_pointers(obj, textures, data, start):
    """Each NJS_TEXLIST spans consecutive entries of one NJS_TEXNAME array.
    The caller has bounded the object within data, in whole lists."""
    stride, list_size = KINDS['texture_names'].size, KINDS['texture_list'].size
    for offset in range(0, obj.size, list_size):
        pointer, count = struct.unpack_from('<II', data, start + offset)
        index = pointer - textures.address
        if not count or index < 0 or index % stride or index + count * stride > textures.size:
            fail(f'{obj.where}: {obj.name}[{offset // list_size}] exceeds its texture_names array')
        symbol = f'{textures.symbol}+{index}' if index else textures.symbol
        yield offset, TexturePointer(pointer, symbol)
