"""Bind a Chao area's external references to the resident Chao image's placed definitions.

The main executable loads CHAO into the stage slot. CHAO loads one of seven areas
above that slot. The two images retain separate code, data and BSS. The resident
input can be its placed relocatable object or the final SH-4 ELF at the same base.
Before this step, GCC checks the area's declarations against the resident sources
through an LTO link. The output keeps every import that the resident does not own.
"""

import argparse
import os
import subprocess
import tempfile
from pathlib import Path

from census import symbols
from config import fail
from dc_overlay import Elf, chao_imports
from tool_process import run


def bind(area, image, resident, output, toolchain):
    if output.resolve() == resident.resolve():
        raise ValueError(f'{output}: would replace the resident it binds against')
    ld = toolchain / 'bin/sh-elf-ld'
    wanted = {name for name, defined in symbols(area).items() if not defined}
    imports = chao_imports(image, Elf(resident, wanted_exports=wanted), wanted)
    exports = [f'--defsym={name}={address:#x}' for name, address in sorted(imports.items())]

    with tempfile.TemporaryDirectory(dir=output.parent) as scratch:
        scratch = Path(scratch)
        # Import addresses only from the linked resident. Its own undefined
        # symbols and its private copies of libgcc stay out of the child.
        linked = scratch / 'area.o'
        run([ld, '-EL', '-r', area, *exports, '-o', linked], check=True)
        os.replace(linked, output)
    return len(exports)


def main():
    parser = argparse.ArgumentParser(prog='chao_area', description=__doc__.splitlines()[0])
    parser.add_argument('image')
    parser.add_argument('area', type=Path)
    parser.add_argument('resident', type=Path)
    parser.add_argument('output', type=Path)
    parser.add_argument('--toolchain', type=Path, required=True)
    args = parser.parse_args()
    try:
        count = bind(args.area, args.image, args.resident, args.output, args.toolchain)
    except (OSError, ValueError, subprocess.CalledProcessError) as error:
        fail(error)
    print(f'{args.image}: {count} resident imports bound')


if __name__ == '__main__':
    main()
