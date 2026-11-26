"""Check the vendored tools' files against tools/SHA256SUMS, or refresh one
from its repository.

    python3 tools/vendor.py --check
    python3 tools/vendor.py TOOL COMMIT [--from CLONE]

gdi, prskit and shckit change in their own repositories. Each has a block in
tools/SHA256SUMS: a "# REPOSITORY COMMIT" line, then the hash of each file
copied into tools/TOOL/, in name order.

--check fails where a tool has no block, its block lists other files than
LICENSE, main.c, TOOL.c and TOOL.h, a tool's file is hashed twice or outside
its block, its directory holds other files than its block, or a file's hash
isn't its line's.

A refresh copies TOOL's files as COMMIT has them into tools/TOOL/ and
rewrites its block, leaving the rest of tools/SHA256SUMS as it was. It reads
them from a clone of the block's repository, which it makes in a temporary
directory, or from CLONE, a clone that holds COMMIT already. CLONE only has
to hold COMMIT; a pin that others can refresh needs COMMIT pushed to the
block's repository too. A refresh fails before changing anything where git
can't give a file, or gives a commit ID that isn't SHA-1's 40 digits.
"""

import argparse
import hashlib
import re
import subprocess
import tempfile
from pathlib import Path

from config import content, fail, save
from tool_process import run

SUMS = Path('tools/SHA256SUMS')
TOOLS = ('gdi', 'prskit', 'shckit')
COMMIT_ID = re.compile(r'[0-9a-f]{40}')
PIN = re.compile(rf'# (\S+) ({COMMIT_ID.pattern})')
LINE = re.compile(r'([0-9a-f]{64})  (\S+)')


def files(tool):
    """The paths of the files vendored for tool, in name order."""
    return sorted(f'tools/{tool}/{name}' for name in ('LICENSE', 'main.c', f'{tool}.c', f'{tool}.h'))


def vendored(line):
    """The tool whose directory holds the file line hashes, or None where line
    hashes no file of a tool's."""
    if entry := LINE.fullmatch(line):
        return next((tool for tool in TOOLS if entry[2].startswith(f'tools/{tool}/')), None)
    return None


def blocks(text):
    """Each tool's block in text, tools/SHA256SUMS's, as {tool: (repository,
    commit, {path: hash})}. A block runs from its pin line to the first line
    that hashes no file of its tool's. A pin line whose repository names none
    of TOOLS fails, as does a second block for one tool. A hash of a tool's
    file outside that tool's block fails, as does a second hash of one file."""
    found, tool = {}, None
    for number, line in enumerate(text.splitlines(), 1):
        if pin := PIN.fullmatch(line):
            tool = pin[1].rsplit('/', 1)[-1]
            if tool not in TOOLS or tool in found:
                fail(f'{SUMS}:{number}: {pin[1]}: {"a second block for" if tool in found else "not the repository of"} '
                     f'a vendored tool, {", ".join(TOOLS)}')
            found[tool] = (pin[1], pin[2], {})
        elif owner := vendored(line):
            checksum, path = LINE.fullmatch(line).groups()
            if owner != tool:
                fail(f"{SUMS}:{number}: {path}: outside {owner}'s block")
            if path in found[tool][2]:
                fail(f'{SUMS}:{number}: {path}: hashed twice')
            found[tool][2][path] = checksum
        else:
            tool = None
    return found


def check():
    """Fail, naming each fault, unless every tool's files are its block's."""
    pinned, faults = blocks(content(SUMS).decode()), []
    for tool in TOOLS:
        if tool not in pinned:
            faults.append(f'{SUMS}: no block for {tool}')
            continue
        sums = pinned[tool][2]
        if sorted(sums) != files(tool):
            faults.append(f"{SUMS}: {tool}'s block lists {', '.join(sorted(sums))}, not {', '.join(files(tool))}")
        directory = Path('tools', tool)
        if not directory.is_dir():
            faults.append(f'{directory}: missing, or not a directory')
            continue
        held = sorted(path.as_posix() for path in directory.iterdir())
        for path in held:
            if path not in sums:
                faults.append(f'{path}: not in {SUMS}')
            elif not Path(path).is_file():
                faults.append(f'{path}: not a file')
            elif hashlib.sha256(content(path)).hexdigest() != sums[path]:
                faults.append(f'{path}: not the file {SUMS} pins; refresh {tool} or restore it')
        faults += [f'{path}: listed in {SUMS}, and missing' for path in sums if path not in held]
    if faults:
        fail(*faults)


def git(clone, *args):
    """git's output for args, run in clone. A failure fails, with git's message."""
    result = run(['git', '-C', str(clone), *args], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    if result.returncode:
        fail(f'git {" ".join(args)}: {result.stderr.decode(errors="replace").strip()}')
    return result.stdout


def refresh(tool, commit, clone):
    """Copy tool's files as commit has them into tools/TOOL/, and rewrite its
    block in tools/SHA256SUMS. clone is a clone that holds commit, or None to
    clone the block's repository."""
    text = content(SUMS).decode()
    pinned = blocks(text)
    if tool not in pinned:
        fail(f'{SUMS}: no block for {tool}')
    repository, old, _ = pinned[tool]
    with tempfile.TemporaryDirectory() as scratch:
        if clone is None:
            clone = scratch
            git('.', 'clone', '--quiet', '--no-checkout', repository, scratch)
        commit = git(clone, 'rev-parse', '--verify', f'{commit}^{{commit}}').decode().strip()
        if not COMMIT_ID.fullmatch(commit):
            fail(f'{commit}: not a SHA-1 commit ID; {SUMS} pins only those')
        fetched = {path: git(clone, 'show', f'{commit}:{Path(path).name}') for path in files(tool)}
    for path, data in fetched.items():
        save(path, data)
    kept, block = [], False
    for line in text.splitlines(keepends=True):
        row = line.splitlines()[0]
        if row == f'# {repository} {old}':
            block = True
            kept.append(f'# {repository} {commit}\n')
            kept += [f'{hashlib.sha256(data).hexdigest()}  {path}\n' for path, data in fetched.items()]
        elif not (block and vendored(row) == tool):
            block = False
            kept.append(line)
    save(SUMS, ''.join(kept).encode())


def main(argv=None):
    parser = argparse.ArgumentParser(prog='vendor', description=__doc__.split('\n\n')[0])
    parser.add_argument('--check', action='store_true', help="check every tool's files against tools/SHA256SUMS")
    parser.add_argument('tool', metavar='TOOL', nargs='?', choices=TOOLS, help='the tool to refresh')
    parser.add_argument('commit', metavar='COMMIT', nargs='?', help="the commit of TOOL's repository to take")
    parser.add_argument('--from', dest='clone', metavar='CLONE', type=Path,
                        help='a clone that holds COMMIT, read in place of cloning the repository; '
                        "push COMMIT to the block's repository too, or no one else can refresh from the pin")
    args = parser.parse_args(argv)
    if args.check == bool(args.tool) or bool(args.tool) != bool(args.commit) or (args.check and args.clone):
        parser.error('give --check alone, or TOOL and COMMIT')
    if args.check:
        check()
    else:
        refresh(args.tool, args.commit, args.clone)


if __name__ == '__main__':
    main()
