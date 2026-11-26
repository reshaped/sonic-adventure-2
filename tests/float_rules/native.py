"""Regenerate the original SHC observations for the play float pass on a Linux host."""
import argparse
import csv
import hashlib
import io
import os
from pathlib import Path
import re
import shutil
import subprocess
import sys
import tempfile

HERE = Path(__file__).resolve().parent
sys.path[:0] = [str(HERE.parents[1] / 'tools'), str(HERE.parent)]
import sysrof

RELEASES = {
    'r11': ('shc-v5.1r11', '10d544d829edabc0ac3b26161e311e8bd09edeba00f0bf76ad80743f3299eca6'),
    'r13': ('shc-v5.1r13', '6b8db1f92c53f146be06f6faec61a58212ad15669ddc1a5a48405ceb726e3987'),
    'r26': ('shc-v5.0r26', 'dd59c6089bffb98e661412b98e6be1aa66905228665a6783d1b9056f15912d9f'),
}
WIBO = '2575d3b0a2f408b2c2b0850db56f1af5d005a138394a6774eba77b6708ecc304'
ROUTES = ('ninja', 'game', 'aggressive', 'modem')
MNEMONIC = re.compile(r'^.{3} [0-9A-F]{8} ([0-9A-F]{4}) +([A-Z.]+)\b')
FUNCTION = re.compile(r'; function: (\w+)\s*$')
BINARY = {0xf000: 'FADD', 0xf001: 'FSUB', 0xf002: 'FMUL', 0xf003: 'FDIV', 0xf00e: 'FMAC'}


def fingerprint(path, expected):
    if hashlib.sha256(path.read_bytes()).hexdigest() != expected:
        raise ValueError(f'{path}: wrong compiler or launcher digest')


def observed(listing):
    result = {}
    function = None
    for line in listing.splitlines():
        found = FUNCTION.search(line)
        if found:
            function = found[1]
            if function in result:
                raise ValueError(f'{function}: duplicate native function')
            result[function] = []
        op = MNEMONIC.match(line)
        if function and op and (op[2] in BINARY.values() or op[2] == 'FNEG'):
            word = int(op[1], 16)
            decoded = 'FNEG' if word & 0xf0ff == 0xf04d else BINARY.get(word & 0xf00f)
            if decoded != op[2]:
                raise ValueError(f'{function}: listing mnemonic disagrees with opcode {word:04x}')
            result[function].append(decoded)
    if not any(result.values()):
        raise ValueError('missing native arithmetic in listing')
    return result


def regenerate(tools, fixture):
    tools = tools.resolve()
    runner = tools / 'wibo-i686'
    fingerprint(runner, WIBO)
    makefile = "include config/compilers.mk\nflags:\n\t@printf '%s\\n' $($(route).release) $($(route).flags)\n"
    rows, profiles = {}, []
    for route in ROUTES:
        output = subprocess.check_output(['make', '-s', '--no-print-directory', '-f', '-', 'flags', 'route=' + route],
                                         cwd=HERE.parents[1], input=makefile, text=True)
        release, *flags = output.split()
        release = release.lower()
        directory, expected = RELEASES[release]
        compiler = tools / directory / 'shc.exe'
        fingerprint(compiler, expected)
        profile = release + '_' + route
        profiles.append(profile)
        with tempfile.TemporaryDirectory() as scratch:
            work = Path(scratch)
            shutil.copyfile(fixture, work / 'probes.c')
            (work / 'tmp').mkdir()
            env = dict(os.environ, SHC_LIB=str(compiler.parent), SHC_TMP='tmp', SHC_INC='.')
            subprocess.run([runner, compiler, 'probes.c', *flags, '-code=machinecode',
                            '-objectfile=probe.obj', '-listfile=probe.lis', '-show=object'],
                           cwd=work, env=env, check=True)
            found = observed((work / 'probe.lis').read_text(encoding='latin-1'))
            sections = sysrof.sections(work / 'probe.obj')
            exports = sysrof.exports(work / 'probe.obj')
            missing = [name for name, (section, _) in exports.items()
                       if sections[section].code and (name[1:] if name.startswith('_') else name) not in found]
            if missing:
                raise ValueError(f'{profile}: listing lacks exported code {missing}')
            for function, ops in found.items():
                rows.setdefault(function, {})[profile] = ' '.join(ops) or '-'
    output = io.StringIO()
    writer = csv.writer(output, delimiter='\t', lineterminator='\n')
    writer.writerow(['function', *profiles])
    for func, values in sorted(rows.items()):
        writer.writerow([func, *(values.get(p, 'inlined') for p in profiles)])
    return output.getvalue()


if __name__ == '__main__':
    args = argparse.ArgumentParser(description=__doc__)
    args.add_argument('--tools', type=Path, default=HERE.parents[1] / 'tools',
                      help='directory containing the pinned Linux wibo and SHC releases')
    args.add_argument('--fixture', type=Path, default=HERE / 'expressions.c')
    args.add_argument('--expected', type=Path, default=HERE / 'native.tsv')
    args.add_argument('--write', action='store_true', help='replace expectations with new measurements')
    options = args.parse_args()
    text = regenerate(options.tools, options.fixture)
    if options.write:
        options.expected.write_text(text)
    elif options.expected.read_text() != text:
        import difflib
        delta = difflib.unified_diff(options.expected.read_text().splitlines(), text.splitlines(),
                                     fromfile=str(options.expected), tofile='observed', lineterm='')
        raise SystemExit('\n'.join(delta))
    print(f'{len(text.splitlines()) - 1} probe functions verified on four native routes')
