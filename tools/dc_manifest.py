"""Collect the hashes of the ELF, decoded bytes and optical file for every configured program."""

import argparse
from hashlib import sha256
from pathlib import Path
import re
import tempfile

from config import Image, fail, images, rows, save
from tool_process import run


DIGEST = re.compile(r'[0-9a-f]{64}')
HEADER = '# image  ELF SHA-256  decoded SHA-256  disc SHA-256  optical path\n'


def inventory():
    return {name: Image(name).file for name in images()}


def checked(root, records, expected):
    """Check that the records name each configured file once, and that each file's bytes match the SHA-256s its
    record gives, before gdi takes any of them."""
    found = {}
    for where, fields in records:
        if len(fields) != 5:
            raise ValueError(f'{where}: expected IMAGE ELF_SHA256 DECODED_SHA256 DISC_SHA256 PATH')
        name, linked, decoded, optical, file = fields
        if expected.get(name) != file:
            raise ValueError(f'{where}: {name} does not supply {file}')
        if file in found or not all(DIGEST.fullmatch(value) for value in (linked, decoded, optical)):
            raise ValueError(f'{where}: duplicate optical file or invalid SHA-256')
        if not file.endswith('.PRS') and decoded != optical:
            raise ValueError(f'{where}: raw image hashes differ')
        output = root / file
        for path, digest in ((root / 'images' / (name + '.elf'), linked), (output, optical)):
            if sha256(path.read_bytes()).hexdigest() != digest:
                raise ValueError(f'{where}: {path} differs from its recorded SHA-256')
        if file.endswith('.PRS'):
            with tempfile.TemporaryDirectory() as directory:
                binary = Path(directory) / 'decoded.bin'
                result = run(['build/prskit', 'decompress', output, binary], check=False)
                if result.returncode or sha256(binary.read_bytes()).hexdigest() != decoded:
                    raise ValueError(f'{where}: {file} does not decode to its recorded SHA-256')
        found[file] = (output, optical)
    if len(found) != len(expected):
        missing = sorted(set(expected.values()).difference(found))
        raise ValueError('missing optical programs: ' + ', '.join(missing))
    return found


def replacements(manifest):
    return checked(manifest.parent, rows(manifest), inventory())


def collect(manifest):
    expected = inventory()
    records = []
    for name in expected:
        path = manifest.parent / 'receipts' / (name + '.txt')
        entry = list(rows(path))
        if len(entry) != 1 or entry[0][1][0] != name:
            raise ValueError(f'{path}: expected one publication record for {name}')
        records.extend(entry)
    if manifest in (output for output, _ in checked(manifest.parent, records, expected).values()):
        raise ValueError(f'{manifest}: is an optical program, which the manifest would overwrite')
    save(manifest, (HEADER + ''.join('  '.join(fields) + '\n' for _, fields in records)).encode())


def main(argv=None):
    parser = argparse.ArgumentParser(prog='dc_manifest', description=__doc__)
    parser.add_argument('manifest', type=Path)
    args = parser.parse_args(argv)
    try:
        collect(args.manifest)
    except (OSError, ValueError) as error:
        fail(error)


if __name__ == '__main__':
    main()
