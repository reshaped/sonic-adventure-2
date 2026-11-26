"""Stage path geometry and the task callbacks declared in src/stage/path_task.h."""

import struct

from config import fail
from kinds import KINDS, PATH_POINTS


STEPS = {'path_rail_init', 'path_rail_inverted_init', 'path_current_step', 'path_join_step'}


def step_address(image, name, where):
    """Look up the actual resident entry for this named path task."""
    if name not in STEPS:
        fail(f'{where}: {name} is not a declared path task')
    if name not in image.symbol_addresses:
        fail(f'{where}: {image.name} has no address for path task {name}')
    return image.symbol_addresses[name]


def check_points(path, points, data, start):
    """Check that points is the array the path header at data[start] names; the caller bounds the header in data."""
    flags, count = struct.unpack_from('<Hh', data, start)
    kind = PATH_POINTS.of(data[start:start + KINDS['stage_path'].size])
    if kind is None:
        fail(f'{path.where}: {path.name} has unsupported point layout {flags & PATH_POINTS.mask}')
    if count <= 0:
        fail(f'{path.where}: {path.name} has point count {count}, which must be positive')
    if points.kind != kind or points.size != count * KINDS[kind].size:
        fail(f'{path.where}: {path.name} requires {count} points of type {kind} in {points.name}')
