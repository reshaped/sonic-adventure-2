"""Build a GDI from the Japanese extraction and verify every file in the completed data track.

Run it from the root, where it reads SHA256SUMS and config/jp-disc.sha256.
"""

import argparse
import hashlib
from pathlib import Path
import re

from config import fail, rows
from dc_iso import HIGH, SECTOR, write_disc
from dc_manifest import DIGEST, replacements
from disc import disc_map, receipt, retail_hashes, sha256, staged_disc


CATALOG = Path('config/jp-disc.sha256')
# ISO paths in the Japanese file inventory have a single extension and use
# uppercase ASCII. Require this spelling before looking up a host file.
DISC_PATH = re.compile(r'(?:[A-Z0-9_]+/)*[A-Z0-9_]+\.[A-Z0-9]+')


def inventory(path):
    expected = {}
    for where, fields in rows(path):
        if len(fields) != 2 or not DIGEST.fullmatch(fields[0]) or not DISC_PATH.fullmatch(fields[1]):
            raise ValueError(f'{where}: expected SHA256 DISC_PATH')
        digest, name = fields
        if name in expected:
            raise ValueError(f'{where}: duplicate disc file {name}')
        expected[name] = digest
    if 'IP.BIN' not in expected:
        raise ValueError(f'{path}: missing bootstrap IP.BIN')
    return expected


def verify_track(directory, extents, expected, bootstrap):
    """Use the independent GDI reader's ISO extents to read back every stored file."""
    mapped, _ = disc_map(directory / 'disc.gdi')
    if ({name: (extent.start, extent.size) for name, extent in mapped.items()} != extents
            or mapped.keys() != expected.keys()
            or '1ST_READ.BIN' not in mapped or not mapped['1ST_READ.BIN'].boot):
        raise ValueError('rebuilt ISO directory or boot entry differs from its placement plan')
    with (directory / 'track03.bin').open('rb') as track:
        if track.read(len(bootstrap)) != bootstrap:
            raise ValueError('high-density bootstrap differs from IP.BIN')
        for name, entry in sorted(mapped.items(), key=lambda pair: pair[1].start):
            track.seek((entry.start - HIGH) * SECTOR)
            digest = hashlib.sha256()
            remaining = entry.size
            while remaining:
                block = track.read(min(1 << 20, remaining))
                if not block:
                    raise ValueError(f'{name}: truncated in high-density track')
                digest.update(block)
                remaining -= len(block)
            if digest.hexdigest() != expected[name]:
                raise ValueError(f'{name}: high-density track differs from its expected SHA-256')
    return mapped


def build(manifest, original, out):
    manifest_hash = sha256(manifest)
    products = replacements(manifest)
    retail = retail_hashes()
    catalog_hash = sha256(CATALOG)
    expected = inventory(CATALOG)
    if products.keys() != retail.keys() or any(expected.get(name) != digest for name, digest in retail.items()):
        raise ValueError('program inventory differs between the manifest, SHA256SUMS and Japanese disc catalog')
    files = {name: original / name for name in expected}
    for name, path in files.items():
        if not path.is_file():
            raise ValueError(f'{path}: missing Japanese disc file')
    if out.resolve() == original.resolve() or out.resolve() in original.resolve().parents:
        raise ValueError(f'{out}: destination contains the original disc files')
    # The copied files are checked as they are written. Check the originals of
    # the programs separately, since their replacements supply those sectors.
    for name, (source, digest) in products.items():
        if sha256(files[name]) != retail[name]:
            raise ValueError(f'{files[name]}: differs from the original Japanese program')
        files[name] = source
        expected[name] = digest
    # A bootstrap fills 16 sectors. One byte more is enough for the digest to
    # refuse a larger file without reading it whole.
    with files['IP.BIN'].open('rb') as stream:
        bootstrap = stream.read(16 * SECTOR + 1)
    bootstrap_hash = hashlib.sha256(bootstrap).hexdigest()
    if bootstrap_hash != expected['IP.BIN']:
        raise ValueError(f'{files["IP.BIN"]}: bootstrap differs from the Japanese inventory')
    with staged_disc(out) as staged:
        staged.mkdir()
        extents = write_disc(staged, files, expected, bootstrap)
        mapped = verify_track(staged, extents, expected, bootstrap)
        if sha256(manifest) != manifest_hash:
            raise ValueError(f'{manifest}: changed during the disc build')
        if sha256(CATALOG) != catalog_hash:
            raise ValueError(f'{CATALOG}: changed during the disc build')
        programs = {name: digest for name, (_, digest) in products.items()}
        source = {'catalog_sha256': catalog_hash, 'file_count': len(expected), 'bootstrap_sha256': bootstrap_hash}
        receipt(staged, staged / 'disc.gdi', manifest_hash, mapped, programs, 'extracted', source)


def main(argv=None):
    parser = argparse.ArgumentParser(prog='dc_extracted', description=__doc__)
    parser.add_argument('manifest', type=Path)
    parser.add_argument('directory', type=Path, help='extracted retail disc including IP.BIN')
    parser.add_argument('output', type=Path)
    args = parser.parse_args(argv)
    try:
        build(args.manifest, args.directory, args.output)
    except (OSError, ValueError) as error:
        fail(error)


if __name__ == '__main__':
    main()
