"""The kinds of the original's data, and the walkers of Ninja's chunk lists.

A kind is what the tools take a datum for. KINDS gives each kind with a
fixed element: its C type, the element's size, the alignment the image
starts each datum at, and each pointer field of the element, with the kind the field
points at. A record is one element whose data.txt row names each field's
target. Any other kind is an array, whose row counts its elements and whose
words give its targets. A kind whose layout its row gives, a pose bank, a
packed motion or a filename bank, belongs to the module that lays it out,
which takes its fixed parts from here.

DECLARED gives the kind of each C type whose fields reach a chunk list. A
flat array takes its kind from the field that reaches it. The walkers read
a chunk list as Ninja's drawers do. NUMBERS gives the kinds whose words
tools/addresses.py takes for numbers and never an address, and how the game
reads them.
"""

import struct
from typing import NamedTuple, Optional


class Choice(NamedTuple):
    """A target kind that the element's own bytes choose. mask selects bits
    of the little-endian word, of size bytes, at offset, and choices pairs
    each supported value with its kind."""
    offset: int
    size: int
    mask: int
    choices: tuple

    @property
    def kinds(self):
        return {kind for _, kind in self.choices}

    def of(self, element):
        """The kind element's bytes choose, or None for a value choices lacks.
        Raises ValueError where element ends before the word does."""
        word = element[self.offset:self.offset + self.size]
        if len(word) != self.size:
            raise ValueError(f'{len(element)} bytes end before the {self.size}-byte word at {self.offset}')
        value = int.from_bytes(word, 'little') & self.mask
        return next((kind for choice, kind in self.choices if choice == value), None)


class Field(NamedTuple):
    """A pointer field: its offset in the element, its C member's name, and
    the kind it points at. The name is '' where the element is the pointer
    itself. target is a kind's name, a Choice, or None for a field that
    holds null in every datum of the kind."""
    offset: int
    name: str
    target: object

    def member(self, element):
        """This field of element, a C name, in C: element's member of its
        name, or element itself where the element is the pointer."""
        return f'{element}.{self.name}' if self.name else element


class Kind(NamedTuple):
    """A kind: the C type of its element, or None where no one type
    declares the element, the element's size, the alignment of a datum's
    start, its pointer fields, and whether it is a record."""
    ctype: Optional[str]
    size: int
    alignment: int
    pointers: tuple = ()
    record: bool = False


def pointer_array(ctype, target):
    """The kind of an array of pointers to target, each element ctype."""
    return Kind(ctype, 4, 4, (Field(0, '', target),))


# src/collision/world.h: a land entry whose flags hold LAND_DRAWN is drawn,
# and its object is a chunk object
LAND_DRAWN = 0x80000000
LAND_OBJECT = Choice(28, 4, LAND_DRAWN, ((LAND_DRAWN, 'chunk_object'), (0, 'basic_object')))
# src/stage/path.h: PATH_LAYOUT_BITS of a path's flags give its points'
# layout. The tools read two layouts
PATH_LAYOUT_BITS = 0x7fff
PATH_LAYOUT_POS = 0
PATH_LAYOUT_SLOPES = 1
PATH_POINTS = Choice(0, 2, PATH_LAYOUT_BITS, ((PATH_LAYOUT_POS, 'points'), (PATH_LAYOUT_SLOPES, 'path_points')))

KINDS = {
    # sdk/include/ninja.h's and nj_render_types.h's
    'chunk_object': Kind('NJS_CNK_OBJECT', 52, 4, (Field(4, 'model', 'chunk_model'),
                                                   Field(44, 'child', 'chunk_object'),
                                                   Field(48, 'sibling', 'chunk_object')), record=True),
    'chunk_model': Kind('NJS_CNK_MODEL', 24, 4, (Field(0, 'vlist', 'chunk_vertices'),
                                                 Field(4, 'plist', 'chunk_polygons')), record=True),
    'chunk_vertices': Kind('Sint32', 4, 4),
    'chunk_polygons': Kind('Sint16', 2, 2),
    'basic_object': Kind('NJS_OBJECT', 52, 4, (Field(4, 'model', 'basic_model'),
                                               Field(44, 'child', 'basic_object'),
                                               Field(48, 'sibling', 'basic_object')), record=True),
    'basic_model': Kind('NJS_MODEL', 40, 4, (Field(0, 'points', 'points'), Field(4, 'normals', 'points'),
                                             Field(12, 'meshsets', 'basic_meshsets'),
                                             Field(16, 'mats', 'basic_materials')), record=True),
    'basic_meshsets': Kind('NJS_MESHSET', 24, 4, (Field(4, 'meshes', 'basic_indices'), Field(8, 'attrs', None),
                                                  Field(12, 'normals', 'points'),
                                                  Field(16, 'vertcolor', 'basic_colors'),
                                                  Field(20, 'vertuv', 'basic_uvs'))),
    'basic_indices': Kind('Sint16', 2, 2),
    'basic_materials': Kind('NJS_MATERIAL', 20, 4),
    'basic_colors': Kind('NJS_COLOR', 4, 4),
    # a meshset's UVs, each a pair of Sint16 as NJS_TEX holds them. The image
    # places every list of them on a word, which NJS_COLOR's Uint32 gives
    # and NJS_TEX's alignment of 2 doesn't
    'basic_uvs': Kind('NJS_COLOR', 4, 4),
    'points': Kind('NJS_POINT3', 12, 4),
    'texture_list': Kind('NJS_TEXLIST', 8, 4, (Field(0, 'textures', 'texture_names'),), record=True),
    'texture_names': Kind('NJS_TEXNAME', 12, 4, (Field(0, 'filename', 'texture_filename'),)),
    # SHC starts each string on a word
    'texture_filename': Kind('char', 1, 4),
    # 28 printable characters of a PVM's name and four NULs
    'padded_texture_filename': Kind('char[32]', 32, 4, record=True),
    'texanims': Kind('NJS_TEXANIM', 20, 2),
    'polygon_vertices': Kind('NJS_POLYGON_VTX', 16, 4),
    'ninja_action': Kind('NJS_ACTION', 8, 4, (Field(0, 'object', 'chunk_object'),
                                              Field(4, 'motion', 'ninja_motion')), record=True),
    'camera_action': Kind('NJS_CACTION', 8, 4, (Field(0, 'camera', 'camera'),
                                                Field(4, 'motion', 'ninja_motion')), record=True),
    'chunk_object_ptrs': pointer_array('NJS_CNK_OBJECT *', 'chunk_object'),
    'chunk_object_tables': pointer_array('NJS_CNK_OBJECT **', 'chunk_object_ptrs'),
    'chunk_model_ptrs': pointer_array('NJS_CNK_MODEL *', 'chunk_model'),
    'ninja_motion_ptrs': pointer_array('NJS_MOTION *', 'ninja_motion'),
    'texture_list_ptrs': pointer_array('NJS_TEXLIST *', 'texture_list'),
    # a group's lists end at a null pointer, and its table's groups too
    'texture_group': pointer_array('NJS_TEXLIST *', 'texture_list'),
    'texture_groups': pointer_array('NJS_TEXLIST **', 'texture_group'),
    'text_ptrs': pointer_array('char *', 'text'),
    'sint32': Kind('Sint32', 4, 4),
    'uint32': Kind('Uint32', 4, 4),
    'float32': Kind('Float', 4, 4),
    # the game's own, from src/ and modules/
    'land_table': Kind('struct land_table', 32, 4, (Field(16, 'entries', 'land_entries'), Field(20, 'motions', None),
                                                    Field(24, 'pvm', 'texture_filename'),
                                                    Field(28, 'texlist', 'texture_list')), record=True),
    'land_entries': Kind('struct land_entry', 32, 4, (Field(16, 'object', LAND_OBJECT), Field(20, 'built', None))),
    # the background's land, whose entries are all drawn
    'scenery_land': Kind('struct land_table', 32, 4, (Field(16, 'entries', 'scenery_entries'),
                                                      Field(20, 'motions', None),
                                                      Field(24, 'pvm', 'texture_filename'),
                                                      Field(28, 'texlist', 'texture_list')), record=True),
    'scenery_entries': Kind('struct land_entry', 32, 4, (Field(16, 'object', 'chunk_object'),
                                                         Field(20, 'built', None))),
    'texture_pvms': Kind('struct texture_pvm', 8, 4, (Field(0, 'name', 'texture_filename'),
                                                      Field(4, 'texlist', 'texture_list'))),
    'model_builds': Kind('struct model_build', 16, 4, (Field(4, 'texlist', 'texture_list'),
                                                       Field(8, 'object', 'chunk_object'), Field(12, 'built', None))),
    'container_pieces': Kind('struct container_piece', 12, 4, (Field(0, 'textures', 'texture_list'),
                                                               Field(4, 'model', 'chunk_model'))),
    'stage_path': Kind('struct path', 16, 4, (Field(8, 'points', PATH_POINTS), Field(12, 'step', 'path_step')),
                       record=True),
    'path_points': Kind('struct path_point', 20, 4),
    'path_ptrs': pointer_array('struct path *', 'stage_path'),
    'uv_scroll': Kind('struct texture_anim', 36, 4, (Field(12, 'key_lengths', None),
                                                     Field(16, 'uvs', 'texture_uv_keys'),
                                                     Field(20, 'textures', None), Field(24, 'cycles', None),
                                                     Field(28, 'model', 'chunk_model')), record=True),
    'uv_scroll_ptrs': pointer_array('struct texture_anim *', 'uv_scroll'),
    'texture_uv_keys': Kind('Sint16', 2, 2),
    'motion_entries': Kind('struct motion_entry', 8, 4, (Field(4, 'motion', 'ninja_motion'),)),
    'motion_clips': Kind('struct motion_table_entry', 28, 4, (Field(0, 'motion', 'ninja_motion'),)),
    'enemy_actions': Kind('struct enemy_action', 16, 4, (Field(0, 'motion', 'ninja_motion'),)),
    'personal_toys': Kind('struct personal_toy', 8, 4, (Field(0, 'model', 'chunk_object'),)),
    'collision_shapes': Kind('struct collision_shape', 48, 4),
    'hunt_hints': Kind('struct hunt_hint', 8, 2),
    'staff_glyphs': Kind('struct staff_glyph', 12, 4),
    'cart_pieces': Kind('struct cart_piece', 108, 4, (Field(0, 'model', 'chunk_object'),
                                                      Field(4, 'solid', 'basic_object'),
                                                      Field(96, 'compiled', None), Field(100, 'signs', 'cart_signs'))),
    'cart_signs': Kind('struct cart_sign', 16, 4),
    'cart_gate_lists': Kind('struct cart_gate_list', 8, 4, (Field(0, 'gates', 'cart_gates'),)),
    # a cross-section of the road
    'cart_gates': Kind('struct cart_gate_point[4]', 32, 4),
    'cart_texture_remaps': Kind('struct cart_texture_remap', 12, 4, (Field(0, 'object', 'chunk_object'),
                                                                     Field(8, 'patches', 'cart_patch_list'))),
    'cart_patch_list': pointer_array('struct cart_texture_patch *', 'cart_texture_patches'),
    'cart_texture_patches': Kind('struct cart_texture_patch', 16, 4, (Field(4, 'texture_word', 'cart_tid'),
                                                                      Field(12, 'uv', 'cart_uv_patches'))),
    'cart_uv_patches': Kind('struct cart_uv_patch', 8, 4, (Field(0, 'target', 'cart_uv'),)),
    # one null pointer field, whatever the C declares there
    'pointer_slot': Kind(None, 4, 4, (Field(0, '', None),), record=True),
}

DECLARED = {'NJS_CNK_MODEL': 'chunk_model'}

# the kinds of table that the code scans to a sentinel, by the field of an
# element that holds it and the C constant it is
SENTINELS = {'motion_entries': ('index', 'MOTION_ENTRY_END'), 'hunt_hints': ('kind', 'HUNT_HINT_END')}

# the kinds whose words hold numbers and no address, with how the game reads
# them. A word of any other kind that tools/content.py keeps as the disc
# holds it, and that holds a number for RAM, needs a row of
# config/addresses.txt (tools/addresses.py)
NUMBERS = {
    'chunk_polygons': "a model's chunk polygons, pairs of Sint16",
    'chunk_vertices': "a model's chunk vertices: halfword pairs, floats, and each vertex's packed color, flags or "
                      'normal',
    'path_points': "a path's points, each a pair of Sint16 slopes and four floats",
}


# Ninja's chunk lists, from sdk/include/ninja.h. A chunk's type is the
# TYPE_BITS of its first halfword in a polygon list, or of its first word in
# a vertex list. Each class of chunk takes the types from its offset up to
# the next class's
NJD_CE = 255
NJD_TINYOFF = 8
NJD_MATOFF = 16
NJD_VERTOFF = 32
NJD_VOLOFF = 56
NJD_STRIPOFF = 64
TYPE_BITS = 0xff
# the tiny chunk that sets a texture
NJD_CT_TID = 8
# the material chunks of a diffuse color, and of a diffuse and an ambient
NJD_CM_D = 17
NJD_CM_DA = 19
# the vertex chunks of a point and a packed color, and of a point and a normal
NJD_CV_D8 = 35
NJD_CV_VN = 41
# the words of a vertex of each vertex chunk type, from NJD_CV_SH on: a point,
# three floats or four under SH, then each part the type names after CV
VERTEX_WORDS = dict(enumerate((4, 8, 3, 4, 4, 4, 4, 4, 4, 6, 7, 7, 7, 7, 7, 7, 4, 5, 5), NJD_VERTOFF))


class ChunkError(ValueError):
    """A chunk list that Ninja's walk can't read. The message names the
    offset, in bytes, of the chunk it stops at."""


class Chunk(NamedTuple):
    """A chunk of a list: its offset and extent in the list's elements, and
    its type. A list's NJD_CE is its last chunk: a polygon chunk of type
    NJD_CE with flags set is a strip chunk, as Ninja reads it."""
    offset: int
    type: int
    extent: int


def polygon_chunks(halves):
    """Yield each chunk of a polygon list, halves its Uint16 elements, up to
    and including its NJD_CE, as Ninja's drawers skip them
    (sdk/ninja/njCnkModelS.c). A bits chunk is one halfword and a tiny chunk
    two. A material, volume or strip chunk is its head, a halfword that
    counts the halfwords after it, and those. Raises ChunkError at a vertex
    chunk's type, at a chunk that runs past halves, and where halves end
    before NJD_CE."""
    at = 0
    while at < len(halves):
        head = halves[at]
        chunk_type = head & TYPE_BITS
        if head == NJD_CE:
            yield Chunk(at, NJD_CE, 1)
            return
        if NJD_VERTOFF <= chunk_type < NJD_VOLOFF:
            raise ChunkError(f'vertex chunk type {chunk_type} at byte {2 * at} of a polygon list')
        if chunk_type < NJD_TINYOFF:
            extent = 1
        elif chunk_type < NJD_MATOFF:
            extent = 2
        else:
            # a list that ends at the head holds no size, and runs past
            extent = 2 + (halves[at + 1] if at + 1 < len(halves) else 0)
        if at + extent > len(halves):
            raise ChunkError(f'chunk at byte {2 * at} runs past the list')
        yield Chunk(at, chunk_type, extent)
        at += extent
    raise ChunkError(f'no NJD_CE by byte {2 * at}')


def vertex_chunks(words):
    """Yield each chunk of a vertex list, words its Uint32 elements, up to
    and including the word whose low half is NJD_CE. Ninja's drawers read
    the list by halfwords: a vertex chunk's low half is its head, and its
    high half counts the words after it. Raises ChunkError at a type that
    VERTEX_WORDS lacks, and as polygon_chunks does."""
    at = 0
    while at < len(words):
        head = words[at]
        chunk_type = head & TYPE_BITS
        if head & 0xffff == NJD_CE:
            yield Chunk(at, NJD_CE, 1)
            return
        if chunk_type not in VERTEX_WORDS:
            raise ChunkError(f'chunk type {chunk_type} at byte {4 * at} of a vertex list')
        extent = 1 + (head >> 16)
        if at + extent > len(words):
            raise ChunkError(f'chunk at byte {4 * at} runs past the list')
        yield Chunk(at, chunk_type, extent)
        at += extent
    raise ChunkError(f'no NJD_CE by byte {4 * at}')


def vertex_range(words, chunk):
    """The vertex buffer's indices that chunk, a vertex chunk of the list
    words, fills, as a range. The word after the chunk's head holds the
    first index in its low half and the count in its high half. Raises
    ChunkError where the words after those two aren't that many vertices of
    the chunk's type, and where the chunk holds no word after its head."""
    if chunk.extent < 2:
        raise ChunkError(f'chunk type {chunk.type} at byte {4 * chunk.offset} holds no index and count')
    index = words[chunk.offset + 1]
    first, count, width = index & 0xffff, index >> 16, VERTEX_WORDS[chunk.type]
    if chunk.extent != 2 + width * count:
        raise ChunkError(f'chunk type {chunk.type} at byte {4 * chunk.offset} holds {4 * chunk.extent} bytes, '
                         f'where its vertex count, {count}, needs {4 * (2 + width * count)}')
    return range(first, first + count)


# the strip chunks SA2's Ninja draws. NJD_CS's vertex is its index alone, and
# NJD_CS_UVN's and NJD_CS_UVH's add a u and a v
NJD_CS = NJD_STRIPOFF
NJD_CS_UVN = 65
NJD_CS_UVH = 66
STRIP_UVS = {NJD_CS: 0, NJD_CS_UVN: 2, NJD_CS_UVH: 2}
# a strip chunk's halfword after its size counts its strips below this bit,
# and from it up the user halfwords after each vertex past a strip's first two
NJD_UFO_SHIFT = 14


class Strip(NamedTuple):
    """A strip of a strip chunk: the offset of the halfword that gives its
    length, its vertices' count, the halfwords of a vertex, and the user
    halfwords after each vertex past its first two."""
    offset: int
    length: int
    width: int
    users: int

    @property
    def extent(self):
        return 1 + self.width * self.length + self.users * max(self.length - 2, 0)

    def vertices(self):
        """Yield the offset of each vertex's index."""
        at = self.offset + 1
        for vertex in range(self.length):
            yield at
            at += self.width + (self.users if vertex >= 2 else 0)


def strips(halves, chunk):
    """Yield each strip of chunk, a strip chunk of the polygon list halves. A
    negative length reverses the winding of the strip's first triangle.
    Raises ChunkError for a type STRIP_UVS lacks and for a strip that runs
    past its chunk. The halfwords after the last strip are the chunk's
    own."""
    end = chunk.offset + chunk.extent
    if chunk.type not in STRIP_UVS or chunk.extent < 3:
        raise ChunkError(f'no strips in chunk type {chunk.type} at byte {2 * chunk.offset}')
    header = halves[chunk.offset + 2]
    width = 1 + STRIP_UVS[chunk.type]
    at = chunk.offset + 3
    for _ in range(header & ((1 << NJD_UFO_SHIFT) - 1)):
        # a strip at the chunk's end holds no length, and runs past
        length = halves[at] if at < end else 0
        strip = Strip(at, 0x10000 - length if length & 0x8000 else length, width, header >> NJD_UFO_SHIFT)
        if at + strip.extent > end:
            raise ChunkError(f'strip at byte {2 * at} runs past its chunk')
        yield strip
        at += strip.extent


# each kind of chunk list, by the format of its elements and its walker
LISTS = {'chunk_polygons': ('<H', polygon_chunks), 'chunk_vertices': ('<I', vertex_chunks)}


def walk(kind, data):
    """The elements of data, a chunk list of kind, and its chunks. Raises
    ChunkError where the walk stops, where its NJD_CE isn't the list's last
    element, and where data ends inside an element."""
    element, walker = LISTS[kind]
    partial = len(data) % struct.calcsize(element)
    if partial:
        raise ChunkError(f'a partial element at byte {len(data) - partial}')
    elements = [value for value, in struct.iter_unpack(element, data)]
    chunks = list(walker(elements))
    if chunks[-1].offset + 1 != len(elements):
        raise ChunkError(f'NJD_CE at byte {chunks[-1].offset * struct.calcsize(element)}, '
                         f'before the list ends at byte {len(data)}')
    return elements, chunks
