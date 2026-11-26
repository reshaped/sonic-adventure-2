"""The declarations of GCC's preprocessed C and the types of its
expressions, for tools/floats.py. Scopes is tools/csource.py's Typed walk
of the declarations and each function's statements, with the types
tools/layout.py's Types reads. It binds each name in the scope Walk gives
it, and gives each initializer of an object of static storage and each
full expression of a function's code. It reads an expression's type from
the declarations in scope: a name's, a member's in its struct's or
union's definition, and a call's in its function's. category() names what
a type is.

lookup() gives what a name names by the innermost declaration in scope: an
object or a function, a typedef or an enumerator, so an object hides an
enumerator or a typedef of its name. It fails where no declaration in scope
names it, or where one in a block that the walk can't read can hide it.
typedef_at() tells Reader and the walk whether a name names a typedef by
the same declarations, passing over those the walk can't read. Where the
walk reads no declaration of the name, it goes by the typedefs Types reads.

GCC's C front end gives each expression a type of its own, which kept()
reads: a typedef's, where the expression is a name declared with the
typedef or C's arithmetic keeps it, and a type with no typedef's name
otherwise. GCC fuses a product into a sum only where the two have one such
type (tools/float_fuse.py).
"""

from collections import namedtuple

from csource import ASSIGNMENTS, SIZES, Typed
from float_tokens import quoted
from layout import LayoutError, basic, derive, type_name

# C's floating types, by rank, and the suffix of each one's constants
TYPES = {'float': 'F', 'double': '', 'long double': 'L'}
# an integer type, by the struct code tools/layout.py's basic() gives it, and
# the type among INTEGER_TYPES that C's usual arithmetic conversions give it
Integer = namedtuple('Integer', 'code type')
# by its struct code, each integer type narrower than int, and the name a
# message gives it
NAMES = {'b': 'signed char', 'B': 'unsigned char', 'c': 'char', 'h': 'short', 'H': 'unsigned short'}
# what one declaration binds a name to: an 'object', which is an object or a
# function, a 'typedef' or an 'enumerator'; the Types term of its type, int
# for an enumerator; and the index where its scope opens and the one where
# it closes
Binding = namedtuple('Binding', 'kind term start end')
# the volatile qualifiers, and every qualifier a declarator can spell after a *
VOLATILE = {'volatile', '__volatile', '__volatile__'}
POINTER_QUALIFIERS = VOLATILE | {'const', '__const', '__const__', 'restrict', '__restrict', '__restrict__'}
# the Types terms of int and of each floating type
INT = ('base', ('int',))
FLOATING = {'float': ('base', ('float',)), 'double': ('base', ('double',)), 'long double': ('base', ('long', 'double'))}
# the type each floating builtin the tree's code calls returns, which GCC
# declares itself
BUILTINS = {'__builtin_fabsf': FLOATING['float'], '__builtin_sqrtf': FLOATING['float'],
            '__builtin_fmaf': FLOATING['float']}


def floating(number):
    """Whether a pp-number is a floating constant: a decimal one, with a
    point or an exponent, or a hexadecimal one with an exponent."""
    hexadecimal = number[:2] in ('0x', '0X')
    return any(mark in number for mark in ('pP' if hexadecimal else '.eE'))


def typed(number):
    """The type of a floating constant, by its suffix."""
    suffix = number[-1].upper() if number[-1] in 'fFlL' else ''
    return next(type for type, suffixed in TYPES.items() if suffixed == suffix)


def category(types, term):
    """What a type's term names once Types resolves it: a floating type, an
    Integer, 'pointer', or 'other' for an aggregate, a function or void. A
    LayoutError says where Types can't read it."""
    if isinstance(term, LayoutError):
        raise term
    term = types.resolve(term)
    if term[0] == 'enum':
        return Integer('i', 'int')
    if term[0] == 'pointer':
        return 'pointer'
    if term[0] != 'base':
        return 'other'
    words = list(term[1])
    if 'float' in words:
        return 'float'
    if 'double' in words:
        return 'long double' if 'long' in words else 'double'
    if 'void' in words:
        return 'other'
    code = basic(words).code
    if code in NAMES:
        return Integer(code, 'int')
    return Integer(code, ('unsigned ' if code == 'I' else '') + ('long' if 'long' in words else 'int'))


def shown(types, node):
    """Whether C shows node's value is floating: a floating constant, a cast
    to a floating type, or arithmetic or a conditional that one takes part
    in."""
    if node.kind == 'number':
        return floating(node.what)
    if node.kind == 'cast':
        try:
            return category(types, node.term) in TYPES
        except LayoutError:
            return False
    if node.kind == 'binary' and node.what not in ('+', '-', '*', '/'):
        return False
    if node.kind == 'unary' and node.what not in ('-', '+'):
        return False
    operands = node.operands[1:] if node.kind == 'conditional' else node.operands
    return node.kind in ('parentheses', 'unary', 'binary', 'conditional') and any(
        shown(types, operand) for operand in operands)


class Scopes(Typed):
    """The Walk of toks, GCC's preprocessed C as marked() gives it, whose
    texts types.take() read as file, and the types of the expressions that
    name what toks declares. Beside the walk's unread, bodies, declarators
    and expressions:

    - names: for each name, the Binding of each declaration of it;
    - statics: each initializer of an object of static storage on the
      tree's lines, at file scope and in a block whose declaration says
      static, as (start, end, term, why): the index of its first token and
      the one past its last, the Types term of its object's type and None,
      or None and why the walk can't read the declaration;
    - volatile_pointers: each declaration whose declarator qualifies a
      pointer volatile after its *, which Types doesn't keep. An object's
      entry is its name and where its scope opens, a typedef's its name, and
      a member's, once member() reads it, its struct's or union's term[:2]
      and its own name."""

    def __init__(self, toks, types, file):
        self.toks = toks
        self.names, self.statics, self.volatile_pointers = {}, [], set()
        super().__init__(toks, types, file)

    def declared(self, name, kind, term, span, start, end):
        self.names.setdefault(name, []).append(Binding(kind, term, start, end))
        if volatile_pointer([token.text for token in self.toks[span[0]:span[1]]]):
            self.volatile_pointers.add((name, start) if kind == 'object' else name)

    def enumerated(self, name, start, end):
        self.names.setdefault(name, []).append(Binding('enumerator', INT, start, end))

    def static(self, start, end, term, why):
        if not self.toks.system[start]:
            self.statics.append((start, end, term, why))

    def binding(self, name, at):
        """The Binding of the innermost declaration of name in scope at
        toks[at] that the walk reads, or None."""
        return max((found for found in self.names.get(name, ()) if found.start <= at < found.end),
                   key=lambda found: found.start, default=None)

    def lookup(self, name, at):
        """The Binding of the declaration of name in scope at toks[at], the
        innermost. A LayoutError says where a declaration the walk can't read
        can hide it, or no declaration in scope names it."""
        found = self.binding(name, at)
        for start, end, why in self.unread:
            if start <= at < end and (found is None or start > found.start):
                raise LayoutError(f'{name}, in the scope of a declaration the tool can\'t read: {why}')
        if found is None:
            raise LayoutError(f'{name}, which no declaration in scope names')
        return found

    def typedef_at(self, name, at):
        """Whether name names a typedef at toks[at]: whether the innermost
        declaration of it in scope there that the walk reads is a typedef,
        or, where the walk reads none, Types reads a typedef of it."""
        found = self.binding(name, at)
        return name in self.types.typedefs if found is None else found.kind == 'typedef'

    def constant(self, node):
        """Whether node is a constant expression SHC folds: numbers, character
        constants and enumerators, sizeof, and the casts and operators that
        take them, but a ?:, && or || with an operand C shows floating, which
        SHC applies as an operand that isn't constant. A name in BUILTINS
        names a function. A LayoutError says lookup() can't tell what another
        name names, where no other operand shows node isn't constant."""
        if node.kind in ('number', 'character'):
            return True
        if node.kind == 'name':
            return node.what not in BUILTINS and self.lookup(node.what, node.start).kind == 'enumerator'
        if node.kind == 'opaque':
            return node.what in SIZES
        if node.kind == 'unary' and node.what not in ('-', '+', '!', '~'):
            return False
        if (node.kind == 'conditional' or node.kind == 'binary' and node.what in ('&&', '||')) and any(
                shown(self.types, operand) for operand in node.operands):
            return False
        if node.kind not in ('parentheses', 'cast', 'unary', 'binary', 'conditional'):
            return False
        errors = []
        for operand in node.operands:
            try:
                if not self.constant(operand):
                    return False
            except LayoutError as error:
                errors.append(error)
        if errors:
            raise errors[0]
        return True

    def floating(self, node):
        """Whether node's value is floating. An arithmetic operation or a ?:
        with a floating operand is floating, whatever the tool reads of the
        others, where type() can't give its rank. A LayoutError says why the
        tool can't tell."""
        try:
            return category(self.types, self.type(node)) in TYPES
        except LayoutError as error:
            if not (node.kind in ('parentheses', 'conditional') or node.kind == 'unary' and node.what in ('+', '-') or
                    node.kind == 'binary' and node.what in ('+', '-', '*', '/')):
                raise
            for operand in node.operands[1:] if node.kind == 'conditional' else node.operands:
                try:
                    if self.floating(operand):
                        return True
                except LayoutError:
                    pass
            raise error

    def type(self, node):
        """The Types term of node's type, its outer qualifiers and typedefs
        resolved, as far as the tool reads it: a floating type, a pointer's,
        an array's, a function's, a struct's or a union's. An integer's is
        its declared type for a name, a cast, a member or a call, and int for
        the rest. A LayoutError says why the tool can't read it."""
        resolve, operands = self.types.resolve, node.operands
        if node.kind == 'number':
            return FLOATING[typed(node.what)] if floating(node.what) else INT
        if node.kind == 'character':
            return INT
        if node.kind == 'string':
            return ('pointer', ('base', ('char',)))
        if node.kind == 'name':
            if node.what in BUILTINS:
                return ('function', BUILTINS[node.what])
            found = self.lookup(node.what, node.start)
            if found.kind == 'typedef':
                raise LayoutError(f'{node.what}, a typedef, where an expression stands')
            return INT if found.kind == 'enumerator' else resolve(found.term)
        if node.kind == 'parentheses':
            return self.type(operands[0])
        if node.kind == 'cast':
            if isinstance(node.term, LayoutError):
                raise node.term
            return resolve(node.term)
        if node.kind == 'unary':
            if node.what == '!':
                return INT
            if node.what == '&':
                return ('pointer', self.type(operands[0]))
            term = self.type(operands[0])
            return self.pointee(term, node) if node.what == '*' else term
        if node.kind == 'binary':
            if node.what in ('+', '-', '*', '/'):
                return self.usual(node, operands)
            return INT
        if node.kind == 'conditional':
            return self.usual(node, operands[1:])
        operator = node.what.partition(',')[0]
        if operator == 'a call':
            term = self.type(operands[0])
            term = resolve(term[1]) if term[0] == 'pointer' else term
            if term[0] != 'function':
                raise LayoutError(f'{quoted(self.toks[node.start:node.end])}, a call of no function')
            return resolve(term[1])
        if operator == 'a subscript':
            return self.pointee(self.type(operands[0]), node)
        if operator in ('.', '->'):
            term = self.type(operands[0])
            return resolve(self.member(self.pointee(term, node) if operator == '->' else term,
                                       self.toks[node.end - 1].text))
        if operator in ('++', '--'):
            return self.type(operands[0])
        if operator in SIZES:
            return INT
        if operator == 'a compound literal':
            return resolve(self.types.term(' '.join(token.text for token in
                                                    self.toks[node.start + 1:self.toks.closing(node.start)])))
        if node.kind == 'assign' or operator == '(' and self.assignment(node):
            return self.type(operands[0])
        raise LayoutError(f'{quoted(self.toks[node.start:node.end]) if node.end > node.start else node.what}, C whose '
                          'type the tool doesn\'t read')

    def kept(self, node, forced=None):
        """The Types term of node's type as GCC's C front end gives it, where
        node's value is floating, its qualifiers off: the type a name's,
        a member's, an element's, a pointee's or a call's declaration spells,
        with the name of a typedef there, or a constant's. A cast's type has
        no typedef's name, as (Float)i is a float. The
        parentheses, a unary + or - and an assignment keep their operand's
        type, and one of two operands of one type keeps it. An arithmetic
        operation or a ?: of a floating value and an integer has the floating
        value's type; of two floating values of other types, the type C's
        usual arithmetic conversions give, with no typedef's name. forced
        gives the type of each node it holds, by (start, end). A LayoutError
        says why the tool can't read it."""
        if forced and (node.start, node.end) in forced:
            return forced[node.start, node.end]
        operator = node.what.partition(',')[0] if node.kind == 'opaque' else node.what
        if node.kind == 'number':
            return FLOATING[typed(node.what)] if floating(node.what) else INT
        if node.kind == 'name' and node.what not in BUILTINS:
            found = self.lookup(node.what, node.start)
            return INT if found.kind == 'enumerator' else bare(self.types, found.term)
        if node.kind == 'cast':
            if isinstance(node.term, LayoutError):
                raise node.term
            return bare(self.types, self.types.resolve(node.term))
        if node.kind == 'parentheses' or node.kind == 'unary' and node.what in ('+', '-'):
            return self.kept(node.operands[0], forced)
        if node.kind in ('binary', 'conditional') and operator in ('+', '-', '*', '/', '?'):
            first, second = (self.kept(operand, forced) for operand in node.operands[-2:])
            if first == second:
                return first
            kinds = [category(self.types, first), category(self.types, second)]
            floats = [kind for kind in kinds if kind in TYPES]
            if len(floats) == 1:
                return first if kinds[0] in TYPES else second
            return FLOATING[max(floats, key=list(TYPES).index)] if floats else INT
        if node.kind == 'unary' and operator == '*' or node.kind == 'opaque' and operator == 'a subscript':
            outer = self.type(node.operands[0])
            if outer[0] in ('pointer', 'array'):
                return bare(self.types, outer[1])
        if node.kind == 'opaque' and operator in ('.', '->'):
            outer = self.type(node.operands[0])
            whole = self.types.resolve(outer[1]) if operator == '->' else outer
            return bare(self.types, self.member(whole, self.toks[node.end - 1].text))
        if node.kind == 'opaque' and operator == 'a call':
            outer = self.type(node.operands[0])
            outer = self.types.resolve(outer[1]) if outer[0] == 'pointer' else outer
            if outer[0] == 'function':
                return bare(self.types, outer[1])
        if node.kind == 'assign' or node.kind == 'opaque' and operator == '(' and self.assignment(node):
            return self.kept(node.operands[0], forced)
        return self.type(node)

    def assignment(self, node):
        """Whether node, parentheses Reader takes whole, holds an assignment
        and no comma after it, whose type is its left operand's."""
        at = node.operands[0].end
        if self.toks[at].text not in ASSIGNMENTS:
            return False
        while at < node.end - 1 and self.toks[at].text != ',':
            at = self.toks.closing(at) + 1
        return at == node.end - 1

    def usual(self, node, operands):
        """The type of node, an arithmetic operation or a conditional of
        operands. Where one is floating, it is the widest floating type among
        them. An operand the tool can't read could be the widest, so a
        LayoutError says why, unless another is long double. Else it is a
        pointer's type, where one operand is a pointer and the other an
        integer, or a conditional has a pointer arm. A conditional of two
        integers has its first arm's type, and an operation of two integers
        or two pointers has int."""
        terms, problems = [], []
        for operand in operands:
            try:
                terms.append(self.type(operand))
            except LayoutError as error:
                problems.append(error)
        kinds = [category(self.types, term) for term in terms]
        widest = max((type for type in kinds if type in TYPES), key=list(TYPES).index, default=None)
        if widest == 'long double' or widest and not problems:
            return FLOATING[widest]
        if problems:
            raise problems[0]
        pointers = [term for term in terms if term[0] in ('pointer', 'array')]
        if len(pointers) == 1 or pointers and node.kind == 'conditional':
            return pointers[0]
        return terms[0] if node.kind == 'conditional' else INT

    def pointee(self, term, node):
        """The type of what term, a pointer's or an array's type, points to, or
        a function's own, as node reads it."""
        if term[0] in ('pointer', 'array'):
            return self.types.resolve(term[1])
        if term[0] == 'function' and node.kind == 'unary':
            return term
        raise LayoutError(f'{quoted(self.toks[node.start:node.end])}, which reads through no pointer')

    def member(self, term, name):
        """The Types term of the member name of term, a struct's or a union's
        type, where a bit-field and an unnamed struct's or union's members
        take part. A LayoutError says why the tool reads none."""
        what = type_name(term) or f'an unnamed {term[0]}'
        bodies = self.types.definitions.get(term[:2]) if term[0] in ('struct', 'union') else None
        if not bodies:
            raise LayoutError(f'.{name} of {what}, whose members the tool doesn\'t read')
        if len({tuple(self.types.files[file][open_ + 1:close]) for file, open_, close in bodies}) > 1:
            raise LayoutError(f'{what}: defined two ways')
        (file, open_, close), *_ = sorted(bodies)
        texts, at = self.types.files[file], open_ + 1
        while at < close:
            base, at = self.types.specifiers(file, at)
            resolved = self.types.resolve(base)
            if texts[at] == ';' and resolved[0] in ('struct', 'union'):
                # an unnamed struct's or union's members are its own
                try:
                    found = self.member(resolved, name)
                    if (resolved[:2], name) in self.volatile_pointers:
                        self.volatile_pointers.add((term[:2], name))
                    return found
                except NoMember:
                    at += 1
                    continue
            while True:
                first = at
                member, derivations, at = self.types.declarator(file, at)
                if member == name:
                    if volatile_pointer(texts[first:at]):
                        self.volatile_pointers.add((term[:2], name))
                    return derive(base, derivations)
                # past a bit-field's width, and the attributes after it
                while texts[at] not in (',', ';'):
                    at = self.types.closing(texts, at) + 1 if texts[at] in '([' else at + 1
                at += 1
                if texts[at - 1] == ';':
                    break
        raise NoMember(f'{what}: no member {name}')


class NoMember(LayoutError):
    """A struct or a union the tool reads whole, with no member of a name."""


def bare(types, term):
    """term, a Types term, without the qualifiers outside it, and a base
    floating type's as FLOATING spells it, as GCC's C front end gives an
    operand's type."""
    while term[0] == 'qualified':
        term = term[1]
    if term[0] == 'base' and category(types, term) in TYPES:
        return FLOATING[category(types, term)]
    return term


def volatile_pointer(texts):
    """Whether texts, the texts of a declarator's tokens, qualify a pointer
    volatile after its *, which Types doesn't keep. A * in a parameter list
    is a parameter's, so the scan passes over each list: a ( after a name, a
    ) or a ], where a grouping ( comes after a *, a qualifier or another (."""
    nested = 0
    for at, text in enumerate(texts):
        before = texts[at - 1] if at else '('
        if text == '(' and (nested or before in (')', ']') or
                            before.isidentifier() and before not in POINTER_QUALIFIERS):
            nested += 1
        elif text == ')' and nested:
            nested -= 1
        after = at + 1
        while text == '*' and not nested and after < len(texts) and texts[after] in POINTER_QUALIFIERS:
            if texts[after] in VOLATILE:
                return True
            after += 1
    return False

