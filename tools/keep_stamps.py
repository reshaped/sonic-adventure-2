"""Finish GNU make 4.3's uncertain stamp comparisons before it updates targets.

make 4.3 can keep the final newline of a long $(file <...) read (Savannah
#58497), so the Makefile can't trust a stamp's text that it finds changed.
It writes the new text beside the stamp instead, as a proposal: a file at
the stamp's path with BATCH's file name appended. BATCH lists each such
stamp, one to a line. A proposal that holds its stamp's bytes is removed,
and any other replaces its stamp. BATCH is then removed. A file that can't
be read, replaced or removed fails, naming it.
"""

import argparse
import os
from pathlib import Path

from config import content, fail


def finish(batch):
    # make ends each path with LF, so the bytes split there before os.fsdecode decodes them.
    # str.splitlines would also end a path at a character such as U+2028.
    # os.fsdecode turns any bytes into a name that open() encodes back to them.
    names = (os.fsdecode(name) for name in content(batch).split(b'\n') if name)
    # a Path drops ./ and doubled slashes, so a stamp listed in two spellings is finished once
    for stamp in dict.fromkeys(Path(name) for name in names):
        proposed = stamp.with_name(stamp.name + batch.name)
        if content(proposed) == content(stamp):
            proposed.unlink()
        else:
            proposed.replace(stamp)
    batch.unlink()


def main():
    parser = argparse.ArgumentParser(prog='keep_stamps', description=__doc__.split('\n\n')[0])
    parser.add_argument('batch', metavar='BATCH', type=Path, help='the list of stamps, as the Makefile writes it')
    args = parser.parse_args()
    try:
        finish(args.batch)
    except OSError as error:
        fail(error)


if __name__ == '__main__':
    main()
