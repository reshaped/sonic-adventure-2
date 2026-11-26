"""Check the Japanese disc's two external Chao archives against the SH-4 layout.

Run make build/prskit first. Pass the two original PRS paths, model followed by
motion. Validation uses the compressed and decoded SHA-256 from the disc
inventory and follows the pointers from the ELF provider's named objects.
"""

import argparse
import hashlib
from pathlib import Path
import struct
import subprocess
import tempfile

from chao_banks import MODELS, MOTIONS, objects
from config import at_root, fail
from tool_process import run


# (compressed size, compressed SHA-256, uncompressed SHA-256)
DISC = {
    MODELS: (1515112, '7b755450517db4ff2e0fe202f4bb51817a9df2d1e9ade6bf4058dbdbb88622c8',
             '5a2c963cb5ba3edc9e18d231e6f9e41417f5471fd4ebc8125fd8b7e759d41eb8'),
    MOTIONS: (434888, 'ef31853e6b3711f06e188c943600b30fdb74e5a32d9c0dc4457d5d934408f5a7',
              '5155dddcfca4b0327628e6267b64c21bb27152f6fd61acfce8709e98e5e17d47'),
}


def offset(bank, address, width, alignment=4):
    if address % alignment:
        raise ValueError(f'{bank.name}: {width} bytes off a {alignment}-byte boundary at {address:#010x}')
    start = address - bank.base
    if start < 0 or start + width > bank.size:
        raise ValueError(f'{bank.name}: {width} bytes outside the archive at {address:#010x}')
    return start


def models(data, entries):
    if not entries:
        raise ValueError(f'{MODELS.name}: no named root to follow')
    pending = []
    visited = set()
    meshes = set()
    for obj in entries:
        if obj.name == 'chao_costume_models':
            start = offset(MODELS, obj.address, obj.width)
            pending.extend(struct.unpack_from(f'<{obj.count}I', data, start))
        else:
            pending.append(obj.address)
    while pending:
        address = pending.pop()
        if not address or address in visited:
            continue
        start = offset(MODELS, address, 52)
        visited.add(address)
        mesh, = struct.unpack_from('<I', data, start + 4)
        if mesh:
            mesh_start = offset(MODELS, mesh, 24)
            vertices, polygons = struct.unpack_from('<II', data, mesh_start)
            if vertices:
                offset(MODELS, vertices, 4)
            if polygons:
                offset(MODELS, polygons, 2, alignment=2)
            meshes.add(mesh)
        pending.extend(struct.unpack_from('<II', data, start + 44))
    return len(visited), len(meshes)


def motions(data, entries):
    if not entries:
        raise ValueError(f'{MOTIONS.name}: no named header to follow')
    keys = 0
    for obj in entries:
        start = offset(MOTIONS, obj.address, obj.width)
        channels, frames, factors, interpolation = struct.unpack_from('<IIHH', data, start)
        if not frames or factors != 3 or (interpolation & 15) != 2:
            raise ValueError(f'{obj.name}: expected a position/rotation Ninja motion')
        start = offset(MOTIONS, channels, 40 * 16)
        for node in range(40):
            position, rotation, positions, rotations = struct.unpack_from('<4I', data, start + node * 16)
            for pointer, count in ((position, positions), (rotation, rotations)):
                if count:
                    offset(MOTIONS, pointer, count * 16)
                    keys += count
    return keys


def read_disc(path, bank, decoder, directory):
    size, packed_hash, raw_hash = DISC[bank]
    if path.stat().st_size != size or hashlib.sha256(path.read_bytes()).hexdigest() != packed_hash:
        raise ValueError(f'{path}: does not match the Japanese disc')
    raw = directory / bank.name
    run([decoder, 'decompress', path, raw], check=True)
    data = raw.read_bytes()
    if len(data) != bank.size or hashlib.sha256(data).hexdigest() != raw_hash:
        raise ValueError(f'{path}: decoded archive does not match the Japanese disc')
    return data


def main():
    parser = argparse.ArgumentParser(prog='check_chao_banks', description=__doc__)
    parser.add_argument('model', type=Path)
    parser.add_argument('motion', type=Path)
    parser.add_argument('--prskit', type=Path, default=Path('build/prskit'))
    args = parser.parse_args()
    at_root('config/')
    try:
        entries = objects(Path('config'))
        with tempfile.TemporaryDirectory() as directory:
            directory = Path(directory)
            nodes, meshes = models(read_disc(args.model, MODELS, args.prskit, directory), entries[MODELS])
            keys = motions(read_disc(args.motion, MOTIONS, args.prskit, directory), entries[MOTIONS])
    except (OSError, ValueError, subprocess.CalledProcessError) as error:
        fail(error)
    print(f'{nodes} model nodes, {meshes} meshes, {len(entries[MOTIONS])} motion headers, {keys} keys')


if __name__ == '__main__':
    main()
