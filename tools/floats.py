"""Rewrite a play source's preprocessed C so that GCC fuses the products SHC
fuses and no others, and divides by a constant as SHC does, for make dc.

    python3 tools/floats.py --shc RELEASE [FLAG]... [--system DIR]... C

C is a source's C as GCC's preprocessor writes it, such as
build/dc/src/main.i, which the tool rewrites in place, from the root, where
the line markers' paths lead to the files of the tree. RELEASE names the
source's SHC compiler (r08, r11, r13 or r26), and each FLAG is one of its
-fpu=single and -aggressive=2 that its build takes, the tool reading no
other of its flags. Each DIR holds system headers, as SH4ZAM's include
directory does for make dc. The tool fails on each problem below and in its
modules' docstrings, naming its file and line, and then leaves C as it
was. All it doesn't rewrite stays byte for byte.

The floating-point rules were checked with R11, R13 and R26, under the
GCC 16.2 flags in config/gcc.mk and the -ffp-contract=on in dc.mk. The
R08 LCP member uses integer arithmetic throughout. For expressions the
rules do not cover, GCC evaluates the C as written.

The tool reads C through tools/csource.py. Two modules give its arithmetic
rules, and fuse() applies both: tools/float_fuse.py SHC's fusion, and
tools/float_divide.py its division by a constant under -aggressive=2.

A decimal constant needs no rewrite. Every route but runtime64 takes
-round=nearest, under which SHC reads a constant as the float nearest it,
as GCC does, and folds a constant expression to the float nearest its
value, as GCC folds it. runtime64's one constant no binary64 holds, 0.301L
in sdk/runtime/_log10.c, lies above the binary64 nearest it, which SHC's
truncation gives too (PORTS.md#floats).

The tool writes C in five forms and no other: a product P as the wrap; a
product's left factor A as (T)(A), of the type T of the sum it is fused
into; a run of + and - as SHC adds it; L += R as the addition; and a
quotient by a constant as the product by its reciprocal. None changes what
an lvalue designates or how often an object is read or written: each
__typeof__ the tool writes takes an operand of floating type, which GCC
doesn't evaluate. None evaluates an operand a ?:, && or || skips. Each
orders what C leaves unsequenced only as C allows, but the addition: the
+= reads L after each call R makes, where the addition leaves that read
unsequenced with R, so a call in R that writes the object L names can
write it after the addition reads it. Fusing R into the addition doesn't
order that read, as the multiply-add GCC builds needs L's value, not a
load of it after R. GCC 16.2 loads L after the call at -Os and -O2 in
x += h() * b and *p += h() * b, as tests/test_float_pass.py holds it to;
no other shape is tested.

PORTS.md#floats gives what make dc does with C.
"""

import argparse
import sys

from config import content, fail, save
from float_edits import Plan
from float_fuse import Route, fuse
from float_scope import Scopes
from float_tokens import marked
from layout import Types


def translate(text, path, route, system=()):
    """text, GCC's preprocessed C at path, as the tool writes it under route,
    or None where it fails; and the problems, each naming its file and line.
    system is as marked() takes it."""
    toks = marked(text, path, system)
    types = Types([])
    file = types.take([token.text for token in toks])
    plan = Plan(toks)
    try:
        try:
            scopes = Scopes(toks, types, file)
        except IndexError:
            # the walk runs past the last token only where C ends inside a declaration
            return None, [f'{toks.where(len(toks) - 1)}: C that ends inside a declaration']
        problems = fuse(toks, scopes, route, plan)
        written = plan.text(text)
    except ValueError as error:
        return None, [str(error)]
    return (None if problems else written), problems


USAGE = '%(prog)s --shc RELEASE [FLAG]... [--system DIR]... C'


def main(argv):
    parser = argparse.ArgumentParser(prog='floats', usage=USAGE, description=(__doc__ or '').split('\n\n')[0])
    parser.add_argument('c', metavar='C', help="rewrite C, a play source's preprocessed C, in place")
    parser.add_argument('--shc', required=True, choices=('r08', 'r11', 'r13', 'r26'), metavar='RELEASE',
                        help='the SHC release that builds the source (r08, r11, r13 or r26)')
    parser.add_argument('-fpu', choices=('single',), help="FLAG: SHC's -fpu=single for the source")
    parser.add_argument('-aggressive', choices=('2',), help="FLAG: SHC's -aggressive=2 for the source")
    parser.add_argument('--system', action='append', default=[], metavar='DIR',
                        help="a directory of system headers, whose lines the tool leaves as they are")
    args = parser.parse_args(argv)
    route = Route(args.shc, bool(args.fpu), bool(args.aggressive))
    text = content(args.c).decode('latin-1')
    written, problems = translate(text, args.c, route, args.system)
    if problems:
        fail(*problems)
    if written != text:
        save(args.c, written.encode('latin-1'))


if __name__ == '__main__':
    main(sys.argv[1:])
