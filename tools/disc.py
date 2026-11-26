"""Replace the programs on a Japanese SA2 GDI and read them back before publishing the disc."""

import argparse
from contextlib import contextmanager
import fcntl
import hashlib
import json
import os
from pathlib import Path
import re
import shutil
from subprocess import PIPE
import tempfile
from typing import NamedTuple

from config import fail, rows
from dc_manifest import replacements
from tool_process import run


SECTOR = 2048
RECEIPT = 'disc.txt'
PREVIOUS = '.previous'
FORMAT = 'sa2-gdi-1'
# The two row forms written by build/gdi map. Paths can contain spaces.
NAMED = re.compile(r' *(\d+) +(\d+) +(\d+)  (.+?)( \(boot\))?')
UNNAMED = re.compile(r' *(\d+) +(\d+) {13}(\S.*)')


class Extent(NamedTuple):
    start: int
    sectors: int
    size: int
    boot: bool


def gdi(*args):
    command = ['build/gdi', *args]
    result = run(command, stdout=PIPE, stderr=PIPE, text=True)
    if result.returncode:
        raise ValueError(result.stderr.strip() or f'{command[0]} {args[0]} failed')
    return result.stdout


def disc_map(original):
    files, free = {}, []
    for number, line in enumerate(gdi('map', original).splitlines(), 1):
        named = NAMED.fullmatch(line)
        if named:
            start, sectors, size = map(int, named.group(1, 2, 3))
            path = named[4]
            if not path.endswith('/'):
                if path in files:
                    raise ValueError(f'{original}: duplicate file {path}')
                files[path] = Extent(start, sectors, size, bool(named[5]))
            continue
        rest = UNNAMED.fullmatch(line)
        if not rest:
            raise ValueError(f'{original}: unreadable sector map at row {number}')
        if rest[3] == 'unreferenced':
            first, count = map(int, rest.group(1, 2))
            free.append((first, first + count))
    return files, free


def sector_count(size):
    return (size + SECTOR - 1) // SECTOR


def reserve(free, start, count):
    """Take [start, start + count) out of a single free extent."""
    for index, (low, high) in enumerate(free):
        if low <= start and start + count <= high:
            free[index:index + 1] = ([(low, start)] if low < start else []) + (
                [(start + count, high)] if start + count < high else [])
            return True
    return False


def placements(files, unreferenced, sizes):
    """Keep the boot file's exclusive end and existing starts where the new files fit."""
    boot = '1ST_READ.BIN'
    if [path for path in files if files[path].boot] != [boot] or boot not in sizes:
        raise ValueError(f'the disc and manifest must identify {boot} as the boot program')
    available = list(unreferenced)
    grow = []
    for path, size in sorted(sizes.items()):
        if path not in files or size < 1:
            raise ValueError(f'{path}: missing from the disc or empty replacement')
        old = files[path]
        count = sector_count(size)
        if path == boot or count > old.sectors:
            available.append((old.start, old.start + old.sectors))
            if path != boot:
                grow.append(path)
        elif count < old.sectors:
            available.append((old.start + count, old.start + old.sectors))
    free = []
    for low, high in sorted(available):
        if low == high:
            continue
        if free and free[-1][1] == low:
            free[-1] = (free[-1][0], high)
        else:
            free.append((low, high))
    count = sector_count(sizes[boot])
    old = files[boot]
    start = old.start + old.sectors - count
    if not reserve(free, start, count):
        raise ValueError(f'{boot}: {count} sectors do not fit before its fixed end {old.start + old.sectors}')
    moved = {boot: start} if start != old.start else {}
    # gdi requires an explicit placement whenever a file gains sectors,
    # including when free space immediately following it permits its old start.
    # Every growing file tries its old start before any moves, so a moved file can't take another's room.
    for path in grow:
        if reserve(free, files[path].start, sector_count(sizes[path])):
            moved[path] = files[path].start
    for path in grow:
        if path in moved:
            continue
        count = sector_count(sizes[path])
        start = next((low for low, high in free if high - low >= count), None)
        if start is None:
            raise ValueError(f'{path}: no free run for {count} sectors')
        reserve(free, start, count)
        moved[path] = start
    return moved


def sha256(path):
    digest = hashlib.sha256()
    with path.open('rb') as stream:
        for block in iter(lambda: stream.read(1 << 20), b''):
            digest.update(block)
    return digest.hexdigest()


def retail_hashes():
    result = {}
    for where, fields in rows(Path('SHA256SUMS')):
        digest, name = fields
        if name.startswith('orig/'):
            path = name[5:]
            if path in result or not re.fullmatch('[0-9a-f]{64}', digest):
                raise ValueError(f'{where}: duplicate or invalid disc hash for {path}')
            result[path] = digest
    return result


def check_payloads(image, expected, scratch):
    """Check the actual ISO9660 payloads independently of the rebuilder's placement report."""
    with tempfile.TemporaryDirectory(dir=scratch) as directory:
        gdi('extract', image, directory, *sorted(expected))
        for path, digest in expected.items():
            if sha256(Path(directory) / path) != digest:
                raise ValueError(f'{image}: {path} SHA-256 differs from its expected disc hash')


def check_output(out):
    """A prior disc can be rebuilt in place only when its directory still holds its own files."""
    if not os.path.lexists(out):
        return
    if out.is_dir() and not out.is_symlink() and (out / RECEIPT).is_file():
        record = json.loads((out / RECEIPT).read_text())
        if isinstance(record, dict) and record.get('format') == FORMAT:
            declared = record.get('files')
            if isinstance(declared, list) and all(isinstance(name, str) for name in declared):
                entries = list(out.iterdir())
                if ({entry.name for entry in entries} == {RECEIPT, *declared}
                        and all(entry.is_file() and not entry.is_symlink() for entry in entries)):
                    return
    raise ValueError(f'{out}: existing directory is not an earlier dc-disc output')


def previous(out):
    """Where publishing out keeps its earlier disc until the new one is in place."""
    return out.with_name(out.name + PREVIOUS)


def work(out):
    """Where a build of out stages its disc, reclaimed by the next build if one is killed."""
    return out.with_name('.' + out.name + '.work')


@contextmanager
def staged_disc(out):
    """Publish a completed generation; restore a previous one left by an interruption."""
    # A build of NAME adopts NAME.previous and deletes .NAME.work beside it, so no output may take either form of name.
    if out.name.startswith('.') or out.name.endswith(PREVIOUS):
        raise ValueError(f'{out}: a build of another output reserves this name')
    out.parent.mkdir(parents=True, exist_ok=True)
    # Parallel make invocations can share one output. Keep the last checked disc
    # available throughout verification and restore it if installing the new one fails.
    with (out.parent / ('.' + out.name + '.lock')).open('a') as lock:
        fcntl.flock(lock, fcntl.LOCK_EX)
        backup = previous(out)
        if os.path.lexists(backup):
            check_output(backup)
            if not os.path.lexists(out):
                backup.rename(out)
        check_output(out)
        scratch = work(out)
        if os.path.lexists(scratch):
            shutil.rmtree(scratch)
        scratch.mkdir()
        try:
            if backup.exists():
                # An interruption after installing the new image left both generations.
                backup.rename(scratch / 'interrupted')
            staged = scratch / 'disc'
            yield staged
            if out.exists():
                out.rename(backup)
            try:
                staged.rename(out)
            except BaseException:
                if backup.exists():
                    backup.rename(out)
                raise
            if backup.exists():
                backup.rename(scratch / 'previous')
        finally:
            shutil.rmtree(scratch)


def receipt(directory, image, manifest_hash, files, expected, edition, source_identity):
    record = {'format': FORMAT, 'manifest_sha256': manifest_hash, 'gdi': image.name, 'source': edition,
              'source_identity': source_identity,
              'files': sorted(entry.name for entry in directory.iterdir()),
              'programs': {path: {'lba': files[path].start, 'bytes': files[path].size,
                                  'sha256': expected[path]} for path in sorted(expected)}}
    with (directory / RECEIPT).open('x') as stream:
        json.dump(record, stream, indent=2)
        stream.write('\n')


def build(manifest, original, out):
    if original.is_dir():
        images = sorted(path for path in original.iterdir() if path.is_file() and path.suffix.lower() == '.gdi')
        if len(images) != 1:
            raise ValueError(f'{original}: expected one original .gdi and its tracks, found {len(images)}')
        original = images[0]
    for removed in (out, previous(out), work(out)):
        if removed.resolve() in original.resolve().parents:
            raise ValueError(f'{removed}: the build can delete this directory, which contains the original disc')
    manifest_hash = sha256(manifest)
    products = replacements(manifest)
    originals = retail_hashes()
    if products.keys() != originals.keys():
        raise ValueError(f'{manifest}: list of programs differs from the original files in SHA256SUMS')
    sizes = {path: source.stat().st_size for path, (source, _) in products.items()}
    expected = {path: digest for path, (_, digest) in products.items()}
    files, free = disc_map(original)
    places = placements(files, free, sizes)
    edits = []
    for path in sorted(products):
        edits.extend(('--replace', path, products[path][0]))
        if path in places:
            edits.extend(('--place', path, str(places[path])))
    with staged_disc(out) as staged:
        scratch = staged.parent
        check_payloads(original, originals, scratch)
        gdi('rebuild', original, staged, *edits)
        image = staged / original.name
        check_payloads(image, expected, scratch)
        rebuilt, _ = disc_map(image)
        planned = dict(files)
        for path, size in sizes.items():
            old = files[path]
            planned[path] = Extent(places.get(path, old.start), sector_count(size), size, old.boot)
        if rebuilt != planned:
            raise ValueError(f'{image}: rebuilt file extents disagree with the plan')
        if sha256(manifest) != manifest_hash:
            raise ValueError(f'{manifest}: changed during the disc build')
        # gdi accepts only sibling track names and recreates the descriptor
        # and each referenced track under those same names.
        # These hashes are read after the rebuild, so they name its input only while the original stays unchanged.
        source_files = {entry.name: sha256(original.parent / entry.name) for entry in sorted(staged.iterdir())}
        receipt(staged, image, manifest_hash, rebuilt, expected, 'gdi', {'files_sha256': source_files})


def main(argv=None):
    parser = argparse.ArgumentParser(prog='disc', description=__doc__)
    parser.add_argument('manifest', type=Path)
    parser.add_argument('gdi', type=Path)
    parser.add_argument('output', type=Path)
    args = parser.parse_args(argv)
    try:
        build(args.manifest, args.gdi, args.output)
    except (OSError, ValueError) as error:
        fail(error)


if __name__ == '__main__':
    main()
