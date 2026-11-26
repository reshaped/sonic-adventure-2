"""List each product SHC fuses into a sum by FMAC in the tree's compiled C,
from SHC's assembly of each source, and count them by route, by directory and
by who reads the floats the source computes.

    python3 tools/fmacs.py DIR

DIR holds SHC's assembly of C sources as make lays out build/obj: the text
for SOURCE.c at DIR/SOURCE.src, as -code=asmcode writes it. A line of the
text may open with its instruction's address in an image, 0x and eight hex
digits, and a space, which the tool drops. The tool reads each C source of
src/, sdk/ and modules/ that has its text in DIR, and counts each other as
unlisted. It takes a source's assembly from the source alone, and the
tree's headers hold none. It edits nothing.

An FMAC is SHC's where the text writes FMAC under a function's label, which
SHC marks "; function: NAME". A source's inline bodies are assembly, which
SHC copies into the text line for line as the source spells them: an FMAC
inside a complete expansion belongs to the body. SHC follows each expansion
with .ALIGN 4. A named assembly function identifies its own body. In a C
function a copy must also be distinguishable from SHC's instructions; if the
origin is ambiguous the census fails. File-scope #pragma asm blocks occur
once per source definition. Each reported FMAC retains its function and,
where the text gives one, its address.

The tool prints each FMAC as PATH: FUNCTION [ADDRESS]: FMAC FR0,FRm,FRn,
in path order, then its counts, each as FMACs in sources:

- by route, as config/compilers.mk gives each source its route and release;
- by directory, the first two parts of a source's directory;
- by bin, as config/parts.txt gives each source the bin of tools/parts.py:
  play, picture or none.

It fails, naming the file, on an FMAC under no function's label that no
#pragma asm block accounts for, on a C source that config/compilers.mk
routes never or twice, listed or not, on a listed source that takes no row
of config/parts.txt, and on DIR where it holds no text of a source.
"""

import argparse
import os
import re
import subprocess
from collections import Counter, defaultdict
from pathlib import Path

import csource
import inline
import parts
from config import at_root, content, fail
from tool_process import run

# the directories of the tree's C
TOPS = ('src', 'sdk', 'modules')
# what make reads to print each route of config/compilers.mk: its name, its
# release and its sources, a route to a line
ROUTES = """include config/compilers.mk
.PHONY: routes
routes:
\t@$(foreach r,$(ROUTES),printf '%s %s %s\\n' $r $($r.release) '$($r.sources)';)
"""
# in SHC's text: a function's label, an instruction's address and the text
# after it, and an FMAC's operands
FUNCTION = re.compile(r'\s*([\w$]+):\s*; function: (\w+)\s*')
LABEL = re.compile(r'\s*([\w$]+):\s*')
ANNOTATION = re.compile(r'\s*; function: (\w+)\s*')
ADDRESSED = re.compile(r'0x([0-9a-fA-F]{8}) (.*)')
FMAC = re.compile(r'\s*(?:[\w?$]+:\s*)?FMAC\s+(FR0\s*,\s*FR\d+\s*,\s*FR\d+)\s*(?:;.*)?$', re.I)


def routes():
    """Each C source's route and release, by path, as make reads config/compilers.mk from the root. A working
    directory without config/compilers.mk fails."""
    at_root('config/compilers.mk')
    env = {name: value for name, value in os.environ.items() if name not in ('MAKEFLAGS', 'MFLAGS', 'MAKELEVEL')}
    made = run(['make', '-s', '--no-print-directory', '-f', '-', 'routes'], input=ROUTES, env=env,
               stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
    if made.returncode:
        fail(f'make: config/compilers.mk gives no routes:\n{made.stderr}{made.stdout}')
    found = {}
    for line in made.stdout.splitlines():
        route, release, *sources = line.split()
        for source in sources:
            if not Path(source).is_file():
                fail(f'config/compilers.mk: {source}: no such source')
            source = os.path.relpath(source)
            if source in found:
                fail(f'{source}: listed more than once in routes {found[source][0]} {route}')
            found[source] = (route, release)
    return found


def functions(lines):
    """SHC marks each function on its label, or just below a long label."""
    function = None
    for at, line in enumerate(lines):
        label = FUNCTION.fullmatch(line)
        if label:
            function = label[2]
        elif (annotation := ANNOTATION.fullmatch(line)) and at and LABEL.fullmatch(lines[at - 1]):
            function = annotation[1]
        elif line.split(maxsplit=1)[:1] == ['.SECTION']:
            function = None
        yield function


def assembly_fmacs(source, lines, hosts, where):
    """The listing lines proven to come from assembly authored in this C source."""
    if 'pragma' not in source or 'asm' not in source:
        return set()
    starts = defaultdict(list)
    for at, line in enumerate(lines):
        starts[line].append(at)

    def copies(body):
        body = list(body)
        return [at for at in starts[body[0]] if lines[at:at + len(body)] == body]

    excluded = set()
    incomplete = {}
    ambiguous = {}
    names = set()
    for raw, count in Counter(csource.assembly_blocks(source)).items():
        sites = [i for i, line in enumerate(raw) if FMAC.fullmatch(line)]
        if sites:
            matches = copies(raw)
            if len(matches) != count:
                fail(f'{where}: ambiguous #pragma asm FMAC: {count} blocks in C, {len(matches)} in the assembly')
            excluded.update(at + i for at in matches for i in sites)

    original = source.splitlines()
    clean = csource.blanked(source)
    masked = clean.splitlines()
    for directive in csource.parse(clean).directives:
        if directive.name != 'pragma' or not directive.words:
            continue
        kind = directive.words[0].text
        if kind == 'inline_asm' and not csource.site(directive, clean):
            fail(f'{where}: unsupported pragma spelling at source line {directive.line}')
        if kind in ('asm', 'endasm') and not re.fullmatch(
                r'[ \t]*#pragma[ \t]+' + kind + r'[ \t]*', masked[directive.line - 1]):
            fail(f'{where}: unsupported pragma spelling at source line {directive.line}')
    for _, name, _, first, last in csource.bodies(where, masked):
        body = [line.rstrip() for line in original[first + 1:last]]
        sites = [i for i, line in enumerate(body) if FMAC.fullmatch(line)]
        names.add(name)
        if not sites:
            continue
        compiler_shaped = inline.compiler_columns(body)
        for at in starts[body[0]]:
            end = at + len(body)
            matched = next((i for i, line in enumerate(body) if lines[at + i:at + i + 1] != [line]), len(body))
            # a shorter body's expansion can open this one's: skip only a copy whose FMACs another accounts for
            open_sites = [at + i for i in sites if i < matched and at + i not in excluded]
            if not open_sites:
                continue
            if matched < len(body) or lines[end:end + 1] != ['          .ALIGN      4']:
                if hosts[at] == name or not inline.compiler_columns(body[:matched]):
                    incomplete[open_sites[0]] = name
                continue
            if hosts[at] is None:
                fail(f'{where}:{open_sites[0] + 1}: inline body {name} under no function')
            if hosts[at] != name and compiler_shaped:
                ambiguous[open_sites[0]] = (name, hosts[at])
                continue
            excluded.update(open_sites)
    for at, host in enumerate(hosts):
        if at in excluded:
            continue
        if at in incomplete:
            fail(f'{where}:{at + 1}: incomplete inline body {incomplete[at]} or missing SHC alignment')
        if at in ambiguous:
            fail(f'{where}:{at + 1}: ambiguous FMAC: inline body {ambiguous[at][0]} in {ambiguous[at][1]} '
                 'is indistinguishable from compiler-generated code')
        if host in names and FMAC.fullmatch(lines[at]):
            fail(f'{where}:{at + 1}: no complete assembly body accounts for the FMAC in {host}')
    return excluded


def fused(listing, source, where):
    """Each compiler-generated (function, address or None, operands) in a complete assembly text."""
    lines, addresses = [], []
    for line in listing.splitlines():
        addressed = ADDRESSED.fullmatch(line)
        addresses.append(addressed[1] if addressed else None)
        lines.append((addressed[2] if addressed else line).rstrip())
    hosts = list(functions(lines))
    excluded = assembly_fmacs(source, lines, hosts, where)
    found = []
    for at, line in enumerate(lines):
        fmac = FMAC.fullmatch(line)
        if fmac and at not in excluded:
            if hosts[at] is None:
                fail(f'{where}: an FMAC under no function')
            found.append((hosts[at], addresses[at], re.sub(r'\s+', '', fmac[1].upper())))
    return found


def sources():
    """The tree's C sources, as paths from the root."""
    return sorted(str(path) for top in TOPS for path in Path(top).rglob('*.c'))


def table(title, counts, holders):
    """Lines that count FMACs and their sources under title, a key to a line."""
    return [title] + [f'  {key}: {counts[key]} in {len(holders[key])}' for key in sorted(holders)]


def report(directory):
    """The tool's lines for DIR, directory."""
    routed, found_parts = routes(), parts.rows()
    lines, unlisted, listed = [], 0, 0
    by = {kind: (Counter(), {}) for kind in ('route', 'directory', 'bin')}
    for path in sources():
        if path not in routed:
            fail(f'{path}: no matching compiler route')
        route, release = routed[path]
        text = Path(directory) / (path[:-2] + '.src')
        if not text.is_file():
            unlisted += 1
            continue
        listed += 1
        row = parts.part(found_parts, path)
        if row is None:
            fail(f'{path}: no row of {parts.PARTS} gives its bin')
        found = fused(content(text).decode('latin-1'), content(path).decode('latin-1'), text)
        keys = {'route': f'{route} {release}', 'directory': '/'.join(Path(path).parent.parts[:2]), 'bin': row.bin}
        for kind, key in keys.items():
            counts, holders = by[kind]
            counts[key] += len(found)
            holders.setdefault(key, set())
            if found:
                holders[key].add(path)
        lines += [f'{path}: {function}{" " + "0x" + address if address else ""}: FMAC {operands}'
                  for function, address, operands in found]
    if not listed:
        fail(f'{directory}: no text of a source')
    total = sum(by['route'][0].values())
    lines.append(f'{total} FMACs in {len(set().union(*by["route"][1].values()))} of {listed} sources listed; '
                 f'{unlisted} sources unlisted')
    for kind, title in (('route', 'by route'), ('directory', 'by directory'), ('bin', 'by bin')):
        lines += table(title, *by[kind])
    return lines


def main(argv=None):
    parser = argparse.ArgumentParser(prog='fmacs', description=__doc__.split('\n\n')[0])
    parser.add_argument('directory', metavar='DIR', help="a directory of SHC's assembly of C sources, as build/obj")
    args = parser.parse_args(argv)
    print('\n'.join(report(args.directory)))


if __name__ == '__main__':
    main()
