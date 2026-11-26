"""Water effects, flags and shaft walls store Ninja polygon lists, vertices, models and nodes in pose arrays."""

import re
import struct
from typing import NamedTuple

import cart_polygons
from config import aligned, fail
import kinds


# the kinds of pose bank this module lays out
BANK_KINDS = ('chunk_poses', 'textured_chunk_poses', 'shaft_wall_poses', 'chunk_pose_tree')

MODEL_SIZE = kinds.KINDS['chunk_model'].size
NODE_SIZE = kinds.KINDS['chunk_object'].size
# a textured pose's material: its NJS_TEXNAME, then its one-entry NJS_TEXLIST
TEXNAME_SIZE = kinds.KINDS['texture_names'].size
MATERIAL_SIZE = TEXNAME_SIZE + kinds.KINDS['texture_list'].size
# the material's words: the texture's filename, attr and texaddr, and the list's textures and count
MATERIAL = struct.Struct('<IIIII')


class Shape(NamedTuple):
    count: int
    polygon_words: int
    vertex_words: int

    @property
    def vertices(self):
        return aligned(2 * self.polygon_words, 4)

    @property
    def mesh(self):
        return self.vertices + 4 * self.vertex_words

    @property
    def node(self):
        return self.mesh + MODEL_SIZE

    @property
    def stride(self):
        return self.node + NODE_SIZE


class Field(NamedTuple):
    kind: str
    address: int
    symbol: str

    @property
    def symbol_address(self):
        return self.address


def shape(fields, where):
    if not all(re.fullmatch(r'[1-9][0-9]{0,8}', field) for field in fields):
        fail(f'{where}: a pose count or word length must be a positive decimal of at most nine digits, '
             'without leading zeros')
    return Shape(*map(int, fields))


def layout(kind, fields, where):
    if kind == 'chunk_pose_tree':
        if not fields or len(fields) % 3:
            fail(f'{where}: chunk_pose_tree needs a count and polygon and vertex word lengths for each run')
        runs = (shape(fields[i:i + 3], where) for i in range(0, len(fields), 3))
        return sum(run.count * run.stride for run in runs) + NODE_SIZE, 4, tuple(fields)
    if kind == 'textured_chunk_poses':
        if len(fields) != 3:
            fail(f'{where}: textured poses need a pose count, polygon words and vertex words')
        run = shape(fields, where)
        return run.count * (MATERIAL_SIZE + run.stride), 4, tuple(fields)
    if kind == 'shaft_wall_poses' and len(fields) != 3:
        fail(f'{where}: shaft wall poses need only the count, polygon words and vertex words')
    if len(fields) not in (3, 4):
        fail(f'{where}: chunk_poses needs a pose count, polygon words and vertex words per pose')
    if len(fields) == 4 and not re.fullmatch(r'\.?[A-Za-z_]\w*\[(?:0|[1-9][0-9]*)\]\.node', fields[3]):
        fail(f'{where}: an external pose hierarchy link must name a node in another pose bank')
    run = shape(fields[:3], where)
    return run.count * run.stride, 4, tuple(fields)


def poses(bank):
    """Yield the index, bank-relative polygon-list offset and shape of each pose in storage order."""
    runs = bank.targets if bank.kind == 'chunk_pose_tree' else bank.targets[:3]
    index = offset = 0
    for i in range(0, len(runs), 3):
        run = Shape(*map(int, runs[i:i + 3]))
        for _ in range(run.count):
            if bank.kind == 'textured_chunk_poses':
                offset += MATERIAL_SIZE
            yield index, offset, run
            index += 1
            offset += run.stride


def field(bank, kind, offset):
    symbol = f'{bank.symbol}+{offset}' if offset else bank.symbol
    return Field(kind, bank.address + offset, symbol)


def fields(bank):
    for index, offset, run in poses(bank):
        if bank.kind == 'textured_chunk_poses':
            material = offset - MATERIAL_SIZE
            yield f'{bank.name}[{index}].texture', field(bank, 'texture_names', material)
            yield f'{bank.name}[{index}].texlist', field(bank, 'texture_list', material + TEXNAME_SIZE)
        for member, local, kind in (('polygons', 0, 'chunk_polygons'), ('vertices', run.vertices, 'chunk_vertices'),
                                    ('mesh', run.mesh, 'chunk_model'), ('node', run.node, 'chunk_object')):
            yield f'{bank.name}[{index}].{member}', field(bank, kind, offset + local)
    if bank.kind == 'chunk_pose_tree':
        yield bank.name + '.root', field(bank, 'chunk_object', bank.size - NODE_SIZE)


def pointers(bank, data, start, members, addresses):
    """Each pose owns its mesh. An authored hierarchy may continue at a named node in another bank."""
    nodes = {node.address: node for name, node in fields(bank) if name.endswith('.node')}
    external = None
    used_external = False
    links = {}
    if bank.kind == 'chunk_poses' and len(bank.targets) == 4:
        external = members.get(bank.targets[3])
        if external is None or external.kind != 'chunk_object' or external.address in nodes:
            fail(f'{bank.where}: {bank.targets[3]} must be a node in another pose bank')
        nodes[external.address] = external
    for index, offset, run in poses(bank):
        prefix = f'{bank.name}[{index}]'
        polygons, vertices, mesh, node = (members[f'{prefix}.{member}'] for member in ('polygons', 'vertices', 'mesh', 'node'))
        if bank.kind == 'textured_chunk_poses':
            material = offset - MATERIAL_SIZE
            filename, attributes, handle, names, count = MATERIAL.unpack_from(data, start + material)
            target = addresses.get(filename)
            if target is None or target.kind != 'texture_filename':
                fail(f'{bank.where}: {prefix} needs a declared texture filename')
            texture = members[f'{prefix}.texture']
            if attributes or handle or names != texture.address or count != 1:
                fail(f'{bank.where}: {prefix} needs its own one-entry texture list')
            yield material, target
            yield material + TEXNAME_SIZE, texture
        padding = offset + 2 * run.polygon_words
        if any(data[start + padding:start + vertices.address - bank.address]):
            fail(f'{bank.where}: nonzero polygon alignment in {prefix}')
        if bank.kind == 'shaft_wall_poses':
            at = start + vertices.address - bank.address
            if data[at] != 35:  # NJD_CV_D8
                fail(f'{bank.where}: {prefix} requires diffuse-color vertices')
            polygon_list = bank._replace(address=polygons.address, size=2 * run.polygon_words, kind=polygons.kind)
            vertex_list = bank._replace(address=vertices.address, size=4 * run.vertex_words, kind=vertices.kind)
            cart_polygons.patch_fields(polygon_list, vertex_list, data, bank.address - start)
        for name, member, extent in (('polygons', polygons, 2 * run.polygon_words),
                                     ('vertices', vertices, 4 * run.vertex_words)):
            at = start + member.address - bank.address
            try:
                kinds.walk(member.kind, data[at:at + extent])
            except kinds.ChunkError as error:
                fail(f'{bank.where}: {prefix}.{name}: {error}')
        for address, expected in ((mesh.address, vertices), (mesh.address + 4, polygons), (node.address + 4, mesh)):
            offset = address - bank.address
            value, = struct.unpack_from('<I', data, start + offset)
            if value != expected.address:
                fail(f'{bank.where}: {prefix} does not reference its own {expected.kind}')
            yield offset, expected
        for address in (node.address + 44, node.address + 48):
            offset = address - bank.address
            value, = struct.unpack_from('<I', data, start + offset)
            if bank.kind == 'textured_chunk_poses' and value:
                fail(f'{bank.where}: {prefix} must be an independent textured pose')
            if bank.kind == 'shaft_wall_poses':
                previous = members[f'{bank.name}[{index - 1}].node'].address if index and address == node.address + 48 else 0
                if value != previous:
                    fail(f'{bank.where}: {prefix} has an incorrect pointer in its wall hierarchy')
            if value and value not in nodes:
                fail(f'{bank.where}: {prefix} points outside its pose hierarchy')
            target = nodes.get(value)
            if target is not None and target == external:
                used_external = True
            links.setdefault(node.address, []).append(value)
            yield offset, target
    if external is not None and not used_external:
        fail(f'{bank.where}: {bank.name} has no hierarchy link to {bank.targets[3]}')
    if bank.kind == 'chunk_pose_tree':
        root = bank.size - NODE_SIZE
        for offset in (4, 44, 48):
            value, = struct.unpack_from('<I', data, start + root + offset)
            if (offset == 44 and value not in nodes) or (offset != 44 and value):
                fail(f'{bank.where}: {bank.name} needs a model-free root with one child in its pose bank')
            yield root + offset, nodes.get(value)
        child, = struct.unpack_from('<I', data, start + root + 44)
        walked = set()
        pending = [child]
        while pending:
            address = pending.pop()
            if address in walked:
                fail(f'{bank.where}: {bank.name} reaches the node at {address:#010x} twice, '
                     'so its poses are not a tree')
            walked.add(address)
            pending.extend(link for link in links[address] if link)
