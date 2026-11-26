"""Each division by a constant that -aggressive=2 has SHC compute as the
product by the constant's reciprocal, and that product's spelling, for
tools/floats.py.

Under -aggressive=2, SHC multiplies a floating value by the float nearest
the reciprocal of a constant divisor's float, where GCC divides by the
constant: sonic_pick_mode's FMUL by H'3E22F986 at 0x8c06c9d8 takes
NJM_RAD_ANG's division by 2 * NJD_PI, which GCC's FDIV rounds otherwise for
about half the angles, and a probe's x / 0.13F multiplies by H'40F62763,
the float nearest the reciprocal of 0.13F's float, not of 0.13. SHC folds a
quotient of two constants as the division, as GCC does, and fuses the
product into a sum as any other, as a probe's c + x / 3.0F does by FMAC.
So the tool writes each floating division on SHC's lines of a function's
code, a / or a /=, by a constant expression, of a dividend that isn't
constant, as the product by the reciprocal in the quotient's type, which
GCC folds to the value of that type nearest it: A / C as

    A * ((__typeof__(A / C))1 / (C))

and A /= C as A *= ((__typeof__(A / (C)))1 / (C)). Each probe above
divides a float, so its reciprocal is the float nearest, as SHC's is. The
product groups as the quotient did, as * and / take the same precedence,
and evaluates A once. Where the reciprocal is a power of two that the
quotient's type holds, the product is the quotient exactly, as it is under
SHC. A float doesn't hold 2 to the 128, the reciprocal of 0x1p-128F. tools/float_fuse.py takes each such quotient
as a product, and writes each. The tool reads each full expression
tools/float_scope.py's walk gives, inside the brackets Reader takes whole,
and fails on a division it can't read, in C Reader doesn't read or whose
type the tool can't tell.
"""

from float_expr import Reader
from float_tokens import quoted
from layout import LayoutError


def divided(toks, scopes):
    """Each division on SHC's lines of the code scopes walks in toks that the
    tool writes as the product by its reciprocal, a / or a /= as Reader
    reads it, by its span; and the problems, each naming its file and
    line."""
    problems, divisions = [], {}
    found, unread = quotients(toks, scopes)
    for start, end in unread:
        if any(toks[at].text in ('/', '/=') and toks.shc[at] and scopes.code(at) for at in range(start, end)):
            problems.append(f'{toks.where(start)}: {quoted(toks[start:end])}: C Reader doesn\'t read, which holds a '
                            'division')
    for node in found:
        dividend, divisor = node.operands
        # the operator, past its dividend
        if not toks.shc[dividend.end] or not scopes.code(dividend.end):
            continue
        try:
            floating = scopes.floating(node) if node.kind == 'binary' else \
                scopes.floating(dividend) or scopes.floating(divisor)
            if not floating or not scopes.constant(divisor) or scopes.constant(dividend):
                continue
        except LayoutError as error:
            problems.append(f'{toks.where(node.start)}: {quoted(toks[node.start:node.end])}: a division the tool '
                            f'can\'t read: {error}')
            continue
        divisions[node.start, node.end] = node
    return divisions, problems


def reciprocal(node):
    """The pieces, as Plan.edit() takes them, that write node, a division A /
    C or A /= C, as the product by C's reciprocal."""
    (dividend, divisor), by = node.operands, (node.operands[1].start, node.operands[1].end)
    if node.kind == 'assign':
        return [(dividend.start, dividend.end), ' *= ((__typeof__(', (dividend.start, dividend.end), ' / (', by,
                ')))1 / (', by, '))']
    return [(dividend.start, dividend.end), ' * ((__typeof__(', (node.start, node.end), '))1 / (', by, '))']


def quotients(toks, scopes):
    """Each division of the full expressions scopes gives of toks, a / or a
    /= as Reader reads it, and inside each bracket that Reader takes whole
    but a statement expression's, whose own the walk gives; and the span of
    each part of an expression Reader doesn't read."""
    found, unread, spans = [], [], list(scopes.expressions)
    while spans:
        start, end = spans.pop()
        reader = Reader(toks, scopes.types, scopes.typedef_at, start, end)
        nodes = [reader.full()]
        if reader.at < end:
            unread.append((reader.at, end))
        while nodes:
            node = nodes.pop()
            if node.kind == 'opaque' and node.what in ('a call', 'a subscript'):
                nodes.append(node.operands[0])
                spans.append((node.operands[0].end + 1, node.end - 1))
            elif node.kind == 'opaque' and node.what == '(':
                if toks[node.start + 1].text != '{':
                    spans.append((node.start + 1, node.end - 1))
            elif node.kind == 'opaque' and node.what == 'a compound literal':
                spans += toks.listed(toks.closing(node.start) + 1, node.end)
            else:
                if node.kind == 'binary' and node.what == '/' or node.kind == 'assign' and node.what == '/=':
                    found.append(node)
                nodes += node.operands
    return found, unread
