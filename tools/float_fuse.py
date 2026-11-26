"""SHC's fusion of products into sums by FMAC, as a rule, and what the tool
writes so that GCC fuses as the rule says, for tools/floats.py.

The production R11, R13 and R26 routes can be probed with the recipe in
tests/float_rules/README.md. It maps the rule families to measured
instructions, terminal results and the exceptions below. A floating run of +
and - holds its operands, with a constant sum or difference as one
operand. A binary -'s right operand is negative in the run, and so are a
unary -'s operand and a product with a negated factor, where the -
negates a floating value. An assignment bounds a run, so c - -(a * b) adds
the product, where x -= -(a * b) subtracts it negated.

SHC adds a run's operands in one of two orders, and the fold of its
constants last, negated where C subtracts it, as in a * b - 0.5F. Where an
operand reads a member, by . or ->, or calls a function, or no product is
positive, it adds them in C's order, whatever C's parentheses say: the
probes' m_sub_first, call_name, names_call_product and two_neg, and
bridge_step's spring. Elsewhere it adds the floating products first, in
C's order, then the operands that read memory, an element, what a pointer
points to or an object of file scope, then the rest, each in C's order:
the probes' neg_elem, neg_names, xxp, xpp, prod_neg_prod, mid_prod_neg,
minus_k, load_name, name_load, global_name and deref_name. In that order,
SHC fuses a positive product, by FMAC, where a positive operand comes
before it; where none comes before it and the next is positive; or where
every other operand is constant. So of two products that open a run, it
fuses the first into the second, whether or not either holds a call.
Where only negative operands come before a product, it fuses it only
where an operand reads a member, as bridge_step's -(spring * K) +
(speed * D) does by FNEG and FMAC. FMAC takes binary32 alone, so
without -fpu=single SHC fuses no product of doubles, as runtime64's
c + a * b by FMUL DR6,DR4 and FADD DR8,DR4.

SHC computes once a product that two expressions of one stretch of code
spell alike, where neither holds a call or a side effect and nothing
between writes what it reads, and fuses it into neither, as
hunt_draw_gauge's place * GAUGE_GAP. A call or indirect store can change
a file-scope, static or addressed local object, and the address of a
member or an element addresses the local that holds it. A volatile
access always reads again. tests/floats/shared.c and its SHC listings'
opcode sequences in shared.tsv cover these cases and an unexposed local
across a call.
Where the parts around the two are alike too, hold no call and read
what nothing between writes, SHC computes the larger part once instead,
its product fused as the rule says, which GCC's two computations match.
tests/floats/shared_reads.c covers an addend written between, a local's
member a pointer reads, and a += beside a sum. Under -aggressive=2,
a division by a constant that tools/float_divide.py writes as a product
is a product to the rule, as SHC fuses it.

GCC, under the -ffp-contract=on dc.mk gives each source tools/parts.py
classes as play, fuses as its C front end reads each + and - of an
expression: the left operand where it is a product, else the right where
it is, through parentheses and a unary +, and a product it subtracts or
subtracts from too, by FNEG and FMAC. It fuses a product only where the
product's type is the sum's own, as tools/float_scope.py's kept() reads
both: a typedef's, such as Float, is another type than float, and a
product of floats added to a double is another type than its sum. It
fuses no constant product, which it folds, as SHC does. So the tool writes
four forms. It wraps each floating product of a function's statements and
automatic objects' initializers that the rule doesn't fuse as

    ({ __typeof__(P) __floats_p = P; __floats_p; })

whose temporary ends GCC's contraction, as a statement does, where
({ P; }) doesn't. Where GCC would fuse a product the rule fuses but for its
type, it gives the product's left factor the sum's type, as (T)(A) * B,
which changes no value: a float, a double and a typedef of either are one
format under GCC's -m4-single-only. Where a cast stands between the two,
the product takes the cast's type, and the tool fails where the sum's is
wider. Where SHC's order of a run would sum its operands otherwise than
C's, it writes the run in SHC's order, the constants last: c - d + a * b
as ((a * b) + (c) - (d)), whose product GCC fuses into c, as SHC does,
and a * b + c + 1.0F + d as ((a * b) + (c) + (d) + ((1.0F))). C
converts each integer constant of the run to the run's floating type, so
the fold casts each: c + 1u - 2u + a * b as
((c) + (a * b) + ((float)(1u) - (float)(2u))). A sum of two operands is
one sum in either order. And it writes a += as an
addition, below.

The rule is SHC's, and holds only on SHC's lines, as
tools/float_tokens.py's marked() gives them. A line GCC alone reads, on a
guard's non-matching side or in a file GCC alone includes, is written for
GCC's -ffp-contract=off, under which it fuses only __builtin_fmaf. So the
tool wraps each floating product a + or a - takes there, or a += or a -=,
whatever SHC would do, and each of a run or a += with a token there, as
SHC never reads it whole. It leaves the lines of a system header as they
are, where GCC fuses as its own contraction reads them.

The tool reads the types of a run's operands and a product's from the
declarations in scope. It fails where it can't tell whether a product it
wraps is floating, whether an operand of the run is constant or floating,
whether what a - negates is floating, or the type of a product GCC fuses
and of its sum. A run goes through no comma, so c - (d, a * b) keeps GCC's
FNEG and FMAC.

The rule follows none of these: a product SHC fuses across statements,
or once it inlines a function; one it computes once for expressions the
stretches above don't take in; and one that both arms of an if spell,
which SHC can compute once ahead of the branch and fuse into neither, as
it does jiggle_bend's half_sine * half_cosine * x * 2.0F by the FMUL at
0x8c03109c in 1ST_READ.BIN and the probes' arms, arms_compound,
arms_global and arms_nested, but not hammer_step's stroke * 0.5F nor
jiggle_step's s1 * 1.6F. No clause of names alone told the two apart over
the tree.

GCC evaluates a compound assignment's right operand first, and fuses no
product into the assignment, where the operand holds a call, an inline
function's included, a statement expression, the tool's wrap among them,
++, --, an assignment or a volatile object, as probes show. It still fuses
a statement expression's product into a local's member. So where R is
such a floating product, and SHC holds R and L in binary32, which FMAC
takes, the tool writes L += R as an addition, which GCC fuses:

    L = L + (R)

where L is a name, in parentheses or not, which names one object however
R runs and may have no address, as a register object has none, and else

    ({ __typeof__(L) *__floats_l = &(L); *__floats_l = *__floats_l + (R); })

which evaluates L once, as the += does, and ahead of R, as SHC does. So
where R changes what L names, as a call that moves a pointer L reads does,
the addition reads and writes the object L named before R. Where R holds a
call, the tool takes an L that holds no side effect: a name, or
parentheses, a cast, a unary & or *, a member access or a subscript by a
name or a constant, of one of them. Where R holds none, it takes an L with
++ or -- too, and a subscript by any expression that holds no call, as
player.c's *pos++ += *push++ * 0.5F: C leaves L's and R's side effects
unsequenced, and a C whose R reads what L's side effect writes has no
defined result, so ahead of R is as good an order as C's. No object L
reads may be volatile, a pointer a declarator qualifies volatile after its
* included, which Types doesn't keep. The tool fails on any other L, and
where it can't read L or the types of what L and R name.

SHC fuses no product into a -=, nor into a += of a constant product, which
it folds, of one its -s negate, of one it computes once for a sum too, of
an integer, which it converts, adds to and converts back, or of doubles
it holds in binary64. So the tool wraps each such product but the
constant one, which GCC folds too, and a binary64 one that holds what
GCC evaluates ahead of the +=, as GCC fuses no such product into it.
Where R is a run of + and -, SHC fuses its products as it does a run's
alone, then adds or subtracts R, as GCC does, so the tool reads that run
as any other.

In a product it wraps and a += it writes as an addition, the tool fails on
what it doesn't read outside the brackets Reader takes whole: a cast to a
type Types doesn't read, C Reader reads as no expression, and a keyword
Reader reads as a name. It also fails on a name, a constant, a string or a
{ right after one, where Reader stopped inside its last operand, as no
expression comes before any of them.
"""

from csource import ASSIGNMENTS, GNU, KEYWORDS, PRIMARY, SIZES
from float_divide import divided, reciprocal
from float_expr import Reader
from float_scope import BUILTINS, FLOATING, INT, TYPES, category, shown
from float_tokens import quoted
from layout import LayoutError

# the opaque nodes Reader reads, by what each is up to a comma: a postfix
# operator's, which takes what comes before it, a call, a subscript, a
# member access, ++ and --; parentheses that hold no one expression; a
# compound literal; and SIZES' operators. Any other is C Reader reads as no
# expression
POSTFIX = {'a call', 'a subscript', '.', '->', '++', '--'}
OPAQUE = {*POSTFIX, '(', 'a compound literal', *SIZES}

# the tokens after which an expression opens: punctuators, the assignments,
# and the keywords before a statement's
OPENS = {';', '{', '}', '(', '[', ',', '?', ':', *ASSIGNMENTS, 'return', 'case', 'else', 'do'}
# the words that open no expression: C's and GNU's keywords but sizeof
WORDS = (KEYWORDS - {'sizeof'}) | GNU | {'_Bool'}


class Route:
    """The SHC that builds a source, and the flags of its that the pass
    reads: release, the compiler release; single, whether its flags make a double
    and a long double binary32, as -fpu=single does; and reciprocal,
    whether they hold -aggressive=2."""

    def __init__(self, release, single=False, reciprocal=False):
        self.release, self.single, self.reciprocal = release, single, reciprocal

    def binary32(self, type):
        """Whether SHC holds a floating type in binary32, which FMAC takes."""
        return type == 'float' or self.single


def fuse(toks, scopes, route, plan):
    """Take into plan each product the tool wraps or gives its sum's type,
    and each += it writes as an addition, under route on the tree's lines of
    toks, GCC's preprocessed C as marked() gives it, whose declarations
    scopes, its Scopes, reads; and give the problems, each naming its file
    and line."""
    chains = Chains(toks, scopes, route)
    if route.reciprocal:
        chains.divisions, problems = divided(toks, scopes)
        chains.problems += problems
    chains.share()
    chains.read(0, len(toks))
    for left, right in chains.compounds.values():
        chains.compound(left, right)
    # agree() reads a run in the order the tool writes it, which regroup()
    # can change, so each run takes its order first
    for root in chains.runs:
        chains.regroup(root)
    for root in chains.runs:
        chains.agree(root)
    for left, right in chains.added:
        chains.agree_added(left, right)
    chains.write(plan)
    return chains.problems


class Chains:
    """What the tool rewrites on the tree's lines of toks under route, as
    fuse() takes them: in wraps, each product it wraps; in written, each +=
    it writes as an addition; in recast, each product it gives its sum's
    type; and in problems, each it can't rewrite, naming its file and
    line."""

    def __init__(self, toks, scopes, route):
        self.toks, self.scopes, self.types, self.route, self.problems = toks, scopes, scopes.types, route, []
        # the span of each node unsure() took a problem for
        self.unsure_of = set()
        # each binary + or - and unary - that a run of + and - takes, as the
        # index of its first token and the index past its last
        self.summed = set()
        # the index past each product to wrap, by the index of its first token,
        # and the span of each the tool took a problem for in its place
        self.wraps, self.failed = {}, set()
        # where each initializer of an object of static storage starts, and
        # the index past it, which read() skips: each is constant
        self.folded = {start: end for start, end, _, _ in scopes.statics}
        # each += written as an addition, by the index of its first token and
        # the index past its last: its pieces, as Plan.edit() takes them
        self.written = {}
        # the left operand of each += and -=, by its operator's index; and
        # each += assigned() leaves for compound(), as its left operand, or
        # None where the tool reads none, and its right one, by the right
        # one's first token's index
        self.lefts, self.compounds = {}, {}
        # the root of each run on SHC's lines, and the left and right operand
        # of each += that adds a product, which agree() reads for the type of
        # the product GCC fuses
        self.runs, self.added = [], []
        # the type each product the tool recasts takes, and its left factor's
        # span, which the tool casts, each by the product's span
        self.recast, self.factors = {}, {}
        # the nonconstant operands of the run each operand of a run on SHC's
        # lines opens, in C's order, and its index there, by its span
        self.order = {}
        # the root of each run the tool writes as SHC sums it, and its operands,
        # each with whether the run subtracts it, by the run's span
        self.reorders = {}
        # the span of each product share() finds another expression of its
        # stretch of code spells too
        self.shared = set()
        # each division by a constant under -aggressive=2 that
        # tools/float_divide.py writes as a product, by its span
        self.divisions = {}
        # the index of each ) that closes the head of an if, a for, a while
        # or a switch, after which a statement opens
        self.heads = {toks.closing(at + 1) for at, token in enumerate(toks[:-1])
                      if token.text in ('if', 'for', 'while', 'switch') and toks[at + 1].text == '('}

    def constant(self, node):
        """Whether scopes shows node is a constant expression SHC folds; where
        it can't tell, False, and unsure() takes the problem."""
        try:
            return self.scopes.constant(node)
        except LayoutError as error:
            self.unsure(node, f'C the tool can\'t tell is constant: {error}')
            return False

    def stripped(self, node):
        """node without the parentheses, the unary +s and the casts to a
        floating type around it."""
        while node.kind == 'parentheses' or node.kind == 'unary' and node.what == '+' or \
                node.kind == 'cast' and shown(self.types, node):
            node = node.operands[0]
        return node

    def own(self, node):
        """Whether a token of node lies on a line GCC alone reads, of the
        tree's: SHC never reads such a node whole, so its rule doesn't hold."""
        return any(not self.toks.shc[at] and not self.toks.system[at] for at in range(node.start, node.end))

    def wrap(self, node):
        """Wrap node, through parentheses, a unary + and casts to a floating
        type, where it is a floating product on the tree's lines of a
        function's code; or, where the tool can't tell whether it is
        floating, take the problem that says why."""
        node = self.stripped(node)
        if not self.product(node):
            return
        where = f'{self.toks.where(node.start)}: {quoted(self.toks[node.start:node.end])}'
        problem = f'{where}: a product SHC doesn\'t fuse'
        try:
            if not self.scopes.floating(node):
                return
        except LayoutError as error:
            self.problems.append(f'{problem}, whose type the tool can\'t tell: {error}')
            self.failed.add((node.start, node.end))
            return
        unread = self.unread(node)
        if unread:
            self.problems.append(f'{problem}, beside {unread}')
            self.failed.add((node.start, node.end))
            return
        self.wraps.setdefault(node.start, set()).add(node.end)

    def product(self, node):
        """Whether node is a product on the tree's lines of a function's code,
        or a quotient tools/float_divide.py writes as one."""
        return node.kind == 'binary' and (node.what == '*' or self.quotient(node)) and \
            not self.toks.system[node.operands[0].end] and self.scopes.code(node.start)

    def floating_product(self, node):
        """Whether node, through parentheses, a unary + and casts to a floating
        type, is a product() whose value is floating, as the rule's are: an
        integer product SHC adds where C does, and the tool moves none."""
        node = self.stripped(node)
        return self.product(node) and not self.nonfloating(node)

    def binary32(self, node):
        """Whether SHC holds node's value in binary32, which FMAC takes; where
        the tool can't read its type, True, and unsure() takes the problem."""
        if self.route.single:
            return True
        try:
            return self.route.binary32(category(self.types, self.scopes.type(node)))
        except LayoutError as error:
            self.unsure(node, f'C whose type the tool can\'t tell: {error}')
            return True

    def quotient(self, node):
        """Whether node is a quotient tools/float_divide.py writes as the
        product by its divisor's reciprocal, which SHC fuses as a product."""
        return (node.start, node.end) in self.divisions and node.kind == 'binary'

    def nonfloating(self, node):
        """Whether scopes shows that node's value isn't floating; where it
        can't tell, False, and unsure() takes the problem."""
        try:
            return not self.scopes.floating(node)
        except LayoutError as error:
            self.unsure(node, f'C the tool can\'t tell is floating: {error}')
            return False

    def unsure(self, node, why):
        """Take the problem why says, once for node, where node lies on the
        tree's lines of a function's code."""
        if not self.toks.system[node.start] and self.scopes.code(node.start) and \
                (node.start, node.end) not in self.unsure_of:
            self.unsure_of.add((node.start, node.end))
            self.problems.append(f'{self.toks.where(node.start)}: {quoted(self.toks[node.start:node.end])}: {why}')

    def signed(self, node, negative=False, whole=True):
        """The operands of the run of + and - that node opens, in C's order,
        each with whether the run subtracts it: whether an odd number of the
        run's -s negate it. The run goes through parentheses, a unary + and
        casts to a floating type, and through each unary -, binary + and
        binary - of a floating value, which it takes into summed. A constant
        sum or difference, which SHC folds whole, is one operand where whole
        is True."""
        while node.kind == 'parentheses' or node.kind == 'unary' and node.what == '+' or \
                node.kind == 'cast' and shown(self.types, node) or \
                node.kind == 'unary' and node.what == '-' and not self.nonfloating(node.operands[0]):
            if node.kind == 'unary' and node.what == '-':
                negative = not negative
                self.summed.add((node.start, node.end))
            node = node.operands[0]
        if self.sum(node) and not (whole and self.constant(node)) and not self.nonfloating(node):
            self.summed.add((node.start, node.end))
            left, right = node.operands
            return self.signed(left, negative, whole) + self.signed(right, negative != (node.what == '-'), whole)
        return [(node, negative)]

    def negative(self, term):
        """Whether term, an operand of a run with whether the run subtracts
        it, is negative in the run: where it is a product an odd number of -s
        negate the parts of, it is negative where the run adds it."""
        node, subtracted = term
        return subtracted != (node.kind == 'binary' and node.what in ('*', '/') and self.negated(node))

    def negated(self, node):
        """Whether an odd number of unary -s of floating operands negate the
        parts of node's run of * and /, through parentheses, a unary + and
        casts to a floating type."""
        node = self.stripped(node)
        if node.kind == 'unary' and node.what == '-' and not self.nonfloating(node.operands[0]):
            return not self.negated(node.operands[0])
        if node.kind == 'binary' and node.what in ('*', '/'):
            return self.negated(node.operands[0]) != self.negated(node.operands[1])
        return False

    @staticmethod
    def sum(node):
        return node.kind == 'binary' and node.what in ('+', '-')

    def take(self, root):
        """Wrap each product of the run of + and - that root opens that the
        rule doesn't fuse, or on a line GCC alone reads, each product."""
        terms = self.signed(root, whole=not self.constant(root))
        nonconstant = [term for term in terms if not self.constant(term[0])]
        if self.own(root):
            for leaf, _ in nonconstant:
                self.wrap(leaf)
            return
        if self.toks.system[root.start]:
            return
        self.runs.append(root)
        constant = [term for term in terms if self.constant(term[0])]
        # where no operand reads a member or calls a function, and a floating
        # product is positive, SHC adds the products first, then the operands
        # that read memory, then the rest
        kept = any(self.member(leaf) for leaf, _ in terms)
        if not kept and not any(self.called(leaf) for leaf, _ in terms) and \
                any(not self.negative(term) and self.floating_product(term[0]) for term in nonconstant):
            sorted_ = self.sorted_run(nonconstant)
            # a sum of two operands is one sum in either order
            if [id(leaf) for leaf, _ in sorted_[2:]] != [id(leaf) for leaf, _ in nonconstant[2:]]:
                self.reorders[root.start, root.end] = root, sorted_ + constant
                nonconstant = sorted_
        # SHC adds the fold of the constants last, where C can add one before
        # an operand that isn't constant
        if (root.start, root.end) not in self.reorders and len(terms) > 2 and \
                any(self.constant(leaf) for leaf, _ in terms[:len(nonconstant)]):
            self.reorders[root.start, root.end] = root, nonconstant + constant
        order = [self.stripped(leaf) for leaf, _ in nonconstant]
        for index, leaf in enumerate(order):
            self.order[leaf.start, leaf.end] = order, index, len(constant), root, terms
        signs = [self.negative(term) for term in nonconstant]
        for index, (leaf, _) in enumerate(nonconstant):
            before, after = signs[:index], signs[index + 1:index + 2]
            # SHC holds a product of another format than binary32 apart from
            # its sum, as FMAC takes none
            if signs[index] or (all(before) and not kept if before else any(after)) or \
                    (self.stripped(leaf).start, self.stripped(leaf).end) in self.shared or \
                    self.product(self.stripped(leaf)) and not self.binary32(self.stripped(leaf)):
                self.wrap(leaf)

    def sorted_run(self, operands):
        """operands, a run's that aren't constant, as SHC adds them where none
        reads a member: the floating products, then the operands that read
        memory, then the rest, each in C's order. SHC computes the second
        product first and fuses the first into it, as GCC fuses the first of
        two it reads, so C's order of the products gives SHC's sums."""
        products = [term for term in operands if self.floating_product(term[0])]
        others = [term for term in operands if not self.floating_product(term[0])]
        loads = [term for term in others if self.loads(term[0])]
        return products + loads + [term for term in others if not self.loads(term[0])]

    def loads(self, node):
        """Whether node reads memory, as SHC holds no local in it: an element,
        what a pointer points to, or an object of file scope."""
        node = self.stripped(node)
        if node.kind == 'opaque' and node.what.partition(',')[0] == 'a subscript' or \
                node.kind == 'unary' and node.what == '*':
            return True
        if node.kind != 'name' or node.what in BUILTINS:
            return False
        try:
            return self.scopes.lookup(node.what, node.start).end == len(self.toks)
        except LayoutError:
            return False

    def share(self):
        """Take into shared each product on SHC's lines that SHC computes once
        for two expressions, alone, and so fuses into neither: two products
        of one spelling in one stretch of a function's code, as unbroken()
        reads it, that hold no call and no side effect, where no larger part
        of the two expressions around them is one spelling too. Where one is,
        SHC computes that part once, the product fused inside it as the rule
        fuses it, as GCC computes each."""
        products, parents = {}, {}
        # A call can change an automatic object through an address taken
        # earlier. A block static can already have escaped on a previous call.
        exposed = set(self.scopes.static_objects)
        writes, operators, calls = {}, set(), set()
        for start, end in self.scopes.expressions:
            if self.toks.system[start]:
                continue
            pending = [Reader(self.toks, self.types, self.scopes.typedef_at, start, end).full()]
            while pending:
                node = pending.pop()
                for operand in node.operands:
                    parents[id(operand)] = node
                pending += node.operands
                if node.kind == 'opaque' and node.what == 'a call':
                    # Its arguments are evaluated before the call can modify storage.
                    calls.add(node.end)
                if node.kind == 'unary' and node.what == '&':
                    name = self.direct_object(self.container(node.operands[0]))
                    if name is not None:
                        exposed.add(name)
                op = node.what.partition(',')[0]
                if node.kind == 'assign' or op in ('++', '--'):
                    target = node.operands[0]
                    operators.add(node.start if node.kind == 'unary' else target.end)
                    # The store happens after its right operand. In particular,
                    # out[0] = a*b+c can itself alter a if out points there.
                    writes.setdefault(node.end, []).append(self.direct_object(target))
                if self.product(node) and self.toks.shc[node.start] and \
                        not self.called(node) and not any(token.text in ('++', '--', *ASSIGNMENTS) for token in self.toks[node.start:node.end]):
                    products.setdefault(self.spelling(node), []).append(node)
        for nodes in products.values():
            nodes.sort(key=lambda node: node.start)
            for first, second in zip(nodes, nodes[1:]):
                outer = []
                for node in (first, second):
                    node = parents.get(id(node))
                    while node is not None and node.kind == 'parentheses':
                        node = parents.get(id(node))
                    outer.append(node)
                # SHC computes the larger part once only where it can: where
                # it holds no call and nothing between changes what it reads
                if None not in outer and self.spelling(outer[0]) == self.spelling(outer[1]) and \
                        not self.called(outer[0]) and self.unbroken(*outer, exposed, writes, operators, calls):
                    continue
                if self.unbroken(first, second, exposed, writes, operators, calls):
                    self.shared.update({(first.start, first.end), (second.start, second.end)})

    def spelling(self, node):
        """The texts of node's tokens."""
        return tuple(token.text for token in self.toks[node.start:node.end])

    def container(self, node):
        """The object whose address &node exposes: node, or the struct, union
        or array object a member access by . or a subscript of an array holds
        it in, through parentheses."""
        while True:
            while node.kind == 'parentheses':
                node = node.operands[0]
            what = node.what.partition(',')[0] if node.kind == 'opaque' else None
            try:
                inside = what == '.' or what == 'a subscript' and self.scopes.type(node.operands[0])[0] == 'array'
            except LayoutError:
                inside = False
            if not inside:
                return node
            node = node.operands[0]

    def direct_object(self, node):
        """A directly named object's binding, or None for an indirect or unread lvalue."""
        while node.kind == 'parentheses':
            node = node.operands[0]
        if node.kind == 'name':
            found = self.scopes.binding(node.what, node.start)
            if found is not None and found.kind == 'object':
                return node.what, found.start
        return None

    def product_reads(self, node):
        """The named inputs of node, an expression, and whether it reads other storage.
        A volatile read or an operand the parser can't account for rules out sharing."""
        names, indirect, pending = set(), False, [node]
        try:
            if self.volatile(node) is not None:
                return None
            while pending:
                part = pending.pop()
                pending.extend(part.operands)
                if part.kind == 'name':
                    found = self.scopes.lookup(part.what, part.start)
                    if found.kind == 'object':
                        names.add((part.what, found.start))
                elif part.kind == 'cast' and isinstance(part.term, LayoutError):
                    return None
                elif part.kind == 'opaque':
                    if part.what.partition(',')[0] not in ('.', '->', 'a subscript'):
                        return None
                    indirect = True
                elif part.kind == 'unary' and part.what == '*':
                    indirect = True
                elif part.kind not in ('number', 'character', 'binary', 'unary', 'cast', 'parentheses'):
                    return None
        except LayoutError:
            return None
        return names, indirect

    def unbroken(self, first, second, exposed, writes, operators, calls):
        """Whether first and second, two expressions of one spelling, read the
        same values. An indirect store or
        a call can reach file-scope and static objects and addressed locals.
        A direct write to a separate, unexposed local cannot alias a factor."""
        reads = self.product_reads(first)
        if reads is None or reads != self.product_reads(second):
            return False
        names, indirect = reads
        memory = indirect or bool(names & exposed)
        for at in range(first.end, second.start):
            text = self.toks[at].text
            if text in ('{', '}', 'if', 'else', 'for', 'while', 'do', 'switch', 'case', 'default', 'goto', 'return',
                        'break', 'continue', '?', '&&', '||') or text == ':' and self.toks[at - 1].kind == 'name':
                return False
            for target in writes.get(at, ()):
                if target in names or (target is None and memory) or (indirect and target in exposed):
                    return False
            if at not in operators and text in (*ASSIGNMENTS, '++', '--'):
                # A declaration's initializer isn't an assignment node. A
                # write inside an opaque expression may have no readable lvalue.
                token = self.toks[at - 1]
                found = self.scopes.binding(token.text, at)
                target = (token.text, found.start) if found is not None else None
                if target in names or target is None or (indirect and target in exposed):
                    return False
            if text in ('asm', '__asm', '__asm__') or memory and at in calls:
                return False
        return True

    @staticmethod
    def member(node):
        """Whether node reads a struct's or a union's member, by . or ->."""
        pending = [node]
        while pending:
            inner = pending.pop()
            if inner.kind == 'opaque' and inner.what.partition(',')[0] in ('.', '->'):
                return True
            pending += inner.operands
        return False

    def called(self, node):
        """Whether node holds a call, as Reader reads one, or a statement
        expression."""
        pending = [node]
        while pending:
            inner = pending.pop()
            if inner.kind == 'opaque' and inner.what == 'a call':
                return True
            pending += inner.operands
        return any(self.toks[at].text == '(' and self.toks[at + 1].text == '{' for at in range(node.start, node.end))

    def read(self, start, end):
        """Wrap each product of each expression of toks[start:end] that SHC
        doesn't fuse, and of each one inside its parentheses and braces that
        Reader reads no further, and take each += of one for compound()."""
        reader, at = Reader(self.toks, self.types, self.scopes.typedef_at, start, end), start
        while at < end:
            token = self.toks[at]
            if at in self.folded:
                at = self.folded[at]
                continue
            # a brace opens no expression but the next, as a statement's or an
            # initializer's, and an if's, a for's, a while's or a switch's
            # head's ) opens its statement
            if token.text in WORDS or token.text in ('{', '}') or at > start and \
                    self.toks[at - 1].text not in OPENS and at - 1 not in self.heads:
                at += 1
                continue
            reader.at = at
            node = reader.conditional()
            # Reader reads each operand of a += or -= as an expression of its
            # own, or as an opaque node's operand, as in a call's argument
            if reader.at < end and self.toks[reader.at].text in ('+=', '-='):
                self.lefts[reader.at] = node
            if at:
                self.assigned(node)
            pending = [node]
            while pending:
                inner = pending.pop()
                if inner.kind == 'opaque' and inner.what == '(':
                    self.read(inner.start + 1, inner.end - 1)
                elif inner.kind == 'opaque' and inner.what == 'a compound literal':
                    self.read(self.toks.closing(inner.start) + 2, inner.end - 1)
                if inner.kind == 'opaque':
                    for operand in inner.operands:
                        if operand.end < end and self.toks[operand.end].text in ('+=', '-='):
                            self.lefts[operand.end] = operand
                    for operand in inner.operands:
                        self.assigned(operand)
                elif (self.sum(inner) or inner.kind == 'unary' and inner.what == '-') and \
                        (inner.start, inner.end) not in self.summed and \
                        any(token.text == '*' or token.text == '/' and self.route.reciprocal
                            for token in self.toks[inner.start:inner.end]):
                    self.take(inner)
                pending += inner.operands
            at = max(reader.at, at + 1)

    def assigned(self, node):
        """Where node is the right operand of a += or -=, wrap it where it is a
        product SHC doesn't fuse into the assignment: a -='s, which subtracts
        it, or one an odd number of -s negate, or one share() takes, or any
        on a line GCC alone reads. Take any other right operand of a += for
        compound(), which keeps only a product that isn't constant."""
        operator = self.toks[node.start - 1].text
        if operator in ('-=', '+='):
            if operator == '-=' or self.own(node) or self.negated(node) or \
                    (self.stripped(node).start, self.stripped(node).end) in self.shared:
                self.wrap(node)
            elif not self.toks.system[node.start] and not self.constant(node):
                # the whole operand comes ahead of an opaque node's first
                # operand, which starts where it does
                self.compounds.setdefault(node.start, (self.lefts.get(node.start - 1), node))

    def compound(self, left, right):
        """Write the += of left and right as an addition where right is a
        floating product of a function's code on the tree's lines that holds
        what effect() names, and SHC holds left and right in binary32. Wrap
        right instead where left isn't floating, as SHC converts left and
        fuses nothing into it. Where the tool can't read left or the types it
        needs, or doesn't take left, take the problem that says why."""
        product = self.stripped(right)
        if not self.product(product):
            return
        problem = f'{self.toks.where((left or right).start)}: {quoted(self.toks[(left or right).start:right.end])}: '
        try:
            if not self.scopes.floating(product):
                return
            if left is None:
                self.problems.append(problem + 'a product SHC can fuse, beside a left operand the tool doesn\'t read')
                return
            if not self.scopes.floating(left):
                self.wrap(right)
                return
            effect = self.effect(right)
            # SHC holds a value of another format apart, as FMAC takes none,
            # and GCC fuses no product that holds an effect into the +=
            if not all(self.route.binary32(category(self.types, self.scopes.type(node))) for node in (left, product)):
                if effect is None:
                    self.wrap(right)
                return
            if effect is None:
                self.added.append((left, right))
                return
            why = self.untaken(left, self.called(right))
        except LayoutError as error:
            self.problems.append(problem + f'a product SHC can fuse, whose types the tool can\'t read: {error}')
            return
        if why:
            self.problems.append(problem + f'a product SHC fuses that holds {effect}, which GCC fuses only as an '
                                           f'addition, beside a left operand the tool doesn\'t take: {why}')
            return
        unread = self.unread(left) or self.unread(right)
        if unread:
            self.problems.append(problem + f'a product SHC fuses, beside {unread}')
            return
        # a name names one object however right runs; another left operand
        # GCC evaluates once, through a pointer of a name C reserves for the
        # implementation, so that right can't move what the addition writes
        self.added.append((left, right))
        held, added = (left.start, left.end), (right.start, right.end)
        named = left
        while named.kind == 'parentheses':
            named = named.operands[0]
        self.written[left.start, right.end] = [held, ' = ', held, ' + (', added, ')'] if named.kind == 'name' else \
            ['({ __typeof__(', held, ') *__floats_l = &(', held, '); *__floats_l = *__floats_l + (', added, '); })']

    def effect(self, node):
        """What node holds that GCC evaluates ahead of the += it is the right
        operand of, as it does each side effect: a call, an inline function's
        too, a statement expression, a product the tool wraps, ++, --, an
        assignment or a volatile object; or None. A LayoutError says why the
        tool can't tell whether an object is volatile."""
        for at in range(node.start, node.end):
            text = self.toks[at].text
            if text in ('++', '--'):
                return text
            if text in ASSIGNMENTS:
                return 'an assignment'
            if text == '(' and self.toks[at + 1].text == '{':
                return 'a statement expression'
            if any(past <= node.end for past in self.wraps.get(at, ())):
                return 'a product the tool wraps'
        pending = [node]
        while pending:
            inner = pending.pop()
            if inner.kind == 'opaque' and inner.what == 'a call':
                return 'a call'
            pending += inner.operands
        held = self.volatile(node)
        return held and f'{quoted(self.toks[held.start:held.end])}, a volatile object'

    def untaken(self, node, called):
        """Why the tool doesn't take node, a +='s left operand, or None where
        it does. It takes a form shape() takes, with side effects where
        called, whether the right operand holds a call, is false, and where
        no object node reads is volatile. A LayoutError says why the tool
        can't tell."""
        why = self.shape(node, not called)
        if why:
            return why
        held = self.volatile(node)
        return held and f'{quoted(self.toks[held.start:held.end])}, a volatile object'

    def shape(self, node, effects=False):
        """Why node isn't a left operand the tool takes, or None where it is:
        a name, or parentheses, a cast, a unary & or *, a member access or a
        subscript by a name or a constant, of one of them; and where effects
        is true, a ++ or -- of one, an assignment to one, or a subscript by
        any expression that holds no call."""
        what = node.what.partition(',')[0] if node.kind == 'opaque' else node.what
        text = quoted(self.toks[node.start:node.end])
        if effects and node.kind in ('unary', 'opaque') and what in ('++', '--'):
            return self.shape(node.operands[0], effects)
        if node.kind in ('parentheses', 'cast') or node.kind == 'unary' and what in ('&', '*') or \
                node.kind == 'opaque' and what in ('.', '->', 'a subscript'):
            index = node.operands[1:]
            if what == 'a subscript' and not (len(index) == 1 and (index[0].kind == 'name' or
                                                                   self.constant(index[0]) or
                                                                   effects and not self.called(index[0]))):
                return f'{text}, a subscript by an expression'
            return self.shape(node.operands[0], effects)
        if node.kind != 'name' or self.constant(node):
            kinds = {'a call': 'a call', '++': 'an increment', '--': 'a decrement'}
            return f'{text}, {kinds.get(what, "none of the left operands the tool takes")}'
        return None

    def volatile(self, node):
        """An object node reads that is volatile, through its typedefs, or
        None: a name, a member, or what a pointer points to. A pointer a
        declarator qualifies volatile after its *, which Types doesn't keep,
        counts too. A LayoutError says why the tool can't tell."""
        scopes, pending = self.scopes, [node]
        while pending:
            inner = pending.pop()
            pending += inner.operands
            what = inner.what.partition(',')[0] if inner.kind == 'opaque' else None
            terms, qualified = [], False
            if inner.kind == 'name' and inner.what not in BUILTINS:
                found = scopes.lookup(inner.what, inner.start)
                if found.kind == 'object':
                    terms.append(found.term)
                    qualified = (inner.what, found.start) in scopes.volatile_pointers
            elif what in ('.', '->', 'a subscript') or inner.kind == 'unary' and inner.what == '*':
                outer = scopes.type(inner.operands[0])
                if what != '.':
                    terms.append(outer[1] if outer[0] in ('pointer', 'array') else outer)
                if what in ('.', '->'):
                    whole = self.types.resolve(terms[-1]) if what == '->' else outer
                    name = self.toks[inner.end - 1].text
                    terms.append(scopes.member(whole, name))
                    qualified = (whole[:2], name) in scopes.volatile_pointers
            if qualified or any(volatile(self.types, term, scopes.volatile_pointers) for term in terms):
                return inner
        return None

    def unread(self, node):
        """What the tool doesn't read in node, a product to wrap or an operand
        of a += to write as an addition, outside the brackets of what Reader
        takes as opaque, which bound it: a cast to a type the tool doesn't
        read; C Reader reads as no expression, or a keyword it reads as a
        name; or the token after node, where it opens an operand, as no
        expression comes before; or None."""
        after = self.toks[node.end] if node.end < len(self.toks) else None
        if after is not None and (after.kind in PRIMARY or after.text == '{'):
            return f'{after.text} after it, which follows no expression'
        pending = [node]
        while pending:
            inner = pending.pop()
            text = quoted(self.toks[inner.start:inner.end]) if inner.end > inner.start else 'a missing operand'
            if inner.kind == 'cast':
                try:
                    category(self.types, inner.term)
                except LayoutError as error:
                    return f'{text}, a cast to a type the tool doesn\'t read: {error}'
            elif inner.kind == 'opaque' and inner.what.partition(',')[0] not in OPAQUE or \
                    inner.kind == 'name' and inner.what in WORDS:
                return f'{text}, C Reader doesn\'t read'
            if inner.kind != 'opaque':
                pending += inner.operands[::-1]
            elif inner.what.partition(',')[0] in POSTFIX:
                # what a postfix operator takes, outside its brackets
                pending.append(inner.operands[0])
        return None

    def taken(self, node):
        """The product GCC sees at node, an operand of a + or -, through
        parentheses, a unary + and casts to a floating type, where the tool
        doesn't wrap it, and the type of the innermost such cast, or None;
        or None. GCC sees a product through a cast only where the product
        has the cast's type, which match() gives it. A LayoutError says why
        the tool can't read a cast's type."""
        cast = None
        while node.kind == 'parentheses' or node.kind == 'unary' and node.what == '+' or \
                node.kind == 'cast' and shown(self.types, node):
            if node.kind == 'cast':
                cast = self.scopes.kept(node)
            node = node.operands[0]
        # GCC folds a constant product, and fuses none
        if self.product(node) and node.end not in self.wraps.get(node.start, ()) and \
                (node.start, node.end) not in self.failed and not self.constant(node):
            return node, cast
        return None

    def regroup(self, node):
        """At each + and - of the run of + and - node opens, where GCC would
        fuse a product the rule fuses into another sum than SHC's, write the
        run as SHC sums it, as addend() reads the two."""
        while node.kind == 'parentheses' or node.kind == 'unary' and node.what in ('+', '-') or \
                node.kind == 'cast' and shown(self.types, node):
            node = node.operands[0]
        if not (self.sum(node) and (node.start, node.end) in self.summed):
            return
        for operand in node.operands:
            self.regroup(operand)
        try:
            fused = [self.taken(operand) for operand in node.operands]
        except LayoutError:
            # agree() takes the problem
            return
        if fused[0] is not None:
            self.addend(fused[0][0], node.operands[1])
        elif fused[1] is not None:
            self.addend(fused[1][0], node.operands[0])

    def agree(self, node):
        """The type GCC's C front end gives node, an operand of a run, as
        kept() reads it once the tool recasts the products below it, or the
        LayoutError that says why the tool can't read it. At each + and - of
        the run, give the product GCC fuses its sum's type, where the
        product's own differs. A run the tool writes as SHC sums it,
        agree_ordered() reads in that order."""
        if (node.start, node.end) in self.reorders:
            return self.agree_ordered(*self.reorders[node.start, node.end])
        if node.kind == 'parentheses' or node.kind == 'unary' and node.what in ('+', '-') or \
                node.kind == 'cast' and shown(self.types, node):
            inner = self.agree(node.operands[0])
            return inner if node.kind != 'cast' else self.read_kept(node)
        if not (self.sum(node) and (node.start, node.end) in self.summed):
            return self.read_kept(node)
        types = [self.agree(operand) for operand in node.operands]
        try:
            fused = [self.taken(operand) for operand in node.operands]
        except LayoutError as error:
            self.problems.append(f'{self.toks.where(node.start)}: {quoted(self.toks[node.start:node.end])}: a sum '
                                 f'SHC fuses a product into, whose cast the tool can\'t read: {error}')
            return self.read_kept(node)
        if fused[0] is not None:
            self.match(node, *fused[0], types[1])
        elif fused[1] is not None:
            self.match(node, *fused[1], types[0])
        return self.read_kept(node)

    def agree_ordered(self, root, terms):
        """agree() of root, a run the tool writes as ordered() writes terms:
        GCC sums its operands that aren't constant from left to right, and
        fuses the first sum's left operand where it is a product, else the
        right operand of each sum."""
        operands = [leaf for leaf, _ in terms if not self.constant(leaf)]
        total = None
        for at, leaf in enumerate(operands):
            other = self.agree(leaf)
            if at:
                try:
                    left = self.taken(operands[0]) if at == 1 else None
                    right = None if left else self.taken(leaf)
                except LayoutError as error:
                    self.problems.append(f'{self.toks.where(root.start)}: {quoted(self.toks[root.start:root.end])}: '
                                         f'a sum SHC fuses a product into, whose cast the tool can\'t read: {error}')
                    return self.read_kept(root)
                if left is not None:
                    self.match(root, *left, other)
                elif right is not None:
                    self.match(root, *right, total)
                other = self.joined(self.read_kept(operands[0]) if at == 1 else total, self.read_kept(leaf))
            total = other
        return self.read_kept(root)

    def joined(self, first, second):
        """The type kept() gives a sum of operands whose kept() types are
        first and second, or the LayoutError that says why the tool can't
        read either."""
        for kind in (first, second):
            if isinstance(kind, LayoutError):
                return kind
        if first == second:
            return first
        try:
            kinds = [category(self.types, first), category(self.types, second)]
        except LayoutError as error:
            return error
        floats = [kind for kind in kinds if kind in TYPES]
        if len(floats) == 1:
            return first if kinds[0] in TYPES else second
        return FLOATING[max(floats, key=list(TYPES).index)] if floats else INT

    def addend(self, fused, other):
        """Where GCC would fuse fused, a product the rule fuses, into other,
        and SHC into another sum, write fused's run as SHC sums it. SHC fuses
        it into the sum of the run's operands before it that aren't
        constant, or where none comes before, the operand after it, or where
        none comes after either, the fold of the run's constants, as a run
        that adds its operands left to right, its constants last, gives."""
        order, index, constants, root, terms = self.order.get((fused.start, fused.end), (None,) * 5)
        if order is None or (root.start, root.end) in self.reorders:
            return
        held = self.signed(other)
        given = [(leaf.start, leaf.end) for leaf, _ in held if not self.constant(leaf)]
        folded = len(held) - len(given)
        expected = order[:index] if index else order[1:2]
        if given != [(leaf.start, leaf.end) for leaf in expected] or \
                folded != (constants if len(order) == 1 else 0):
            self.reorders[root.start, root.end] = root, terms

    def read_kept(self, node):
        """kept()'s type of node once the tool recasts the products below it,
        or the LayoutError that says why the tool can't read it."""
        try:
            return self.scopes.kept(node, self.recast)
        except LayoutError as error:
            return error

    def agree_added(self, left, right):
        """Give the product of a += that GCC fuses, right, its sum's type,
        where its own differs."""
        try:
            fused = self.taken(right)
        except LayoutError as error:
            self.problems.append(f'{self.toks.where(left.start)}: {quoted(self.toks[left.start:right.end])}: a sum '
                                 f'SHC fuses a product into, whose cast the tool can\'t read: {error}')
            return
        if fused is not None:
            self.match(left, *fused, self.read_kept(left))

    def match(self, node, fused, cast, other):
        """Give fused, a product GCC fuses into node's sum with an operand of
        the type other, the sum's type, where its own differs: the floating
        type of C's usual arithmetic conversions, which has no typedef's
        name; and where a cast stands between them, cast, the cast's type.
        GCC's sum of an integer and a product takes the product's type. A
        problem says where the tool can't read the types, as other's
        LayoutError says, and where a cast to a narrower type than the sum's
        parts the product from the sum."""
        where = f'{self.toks.where(fused.start)}: {quoted(self.toks[fused.start:fused.end])}: a product SHC fuses'
        try:
            if isinstance(other, LayoutError):
                raise other
            own = self.scopes.kept(fused, self.recast)
            kinds = [category(self.types, own), category(self.types, other)]
        except LayoutError as error:
            self.problems.append(f'{where}, whose sum\'s type the tool can\'t read: {error}')
            return
        if kinds[0] not in TYPES:
            return
        floats = [kind for kind in (category(self.types, cast) if cast else kinds[0], kinds[1]) if kind in TYPES]
        target = FLOATING[max(floats, key=list(TYPES).index)]
        # FMAC takes no sum SHC holds in another format than binary32, and
        # take() wraps a product of one
        if not self.route.binary32(category(self.types, target)):
            return
        if cast is not None and target != cast:
            self.problems.append(f'{where}, which a cast to {quoted(self.toks[node.start:node.end])}\'s narrower '
                                 'type parts from the sum, so that GCC can\'t fuse it')
            return
        if own == other and cast is None or kinds[1] not in TYPES and cast is None:
            return
        if own != target:
            self.recast[fused.start, fused.end] = target
            self.factors[fused.start, fused.end] = fused.operands[0].start, fused.operands[0].end

    def write(self, plan):
        """Take each product the tool wraps or recasts, and each += it writes
        as an addition, into plan. A wrap is a statement expression that
        holds the product P in a temporary, of a name C reserves for the
        implementation, which ends GCC's contraction as a statement does:
        ({ __typeof__(P) __floats_p = P; __floats_p; }). A recast casts the
        product's left factor A to the sum's type T: (T)(A)."""
        for start, ends in self.wraps.items():
            for end in ends:
                product = reciprocal(self.divisions[start, end]) if (start, end) in self.divisions else [(start, end)]
                plan.edit(start, end, ['({ __typeof__(', (start, end), ') __floats_p = ', *product, '; __floats_p; })'])
        for (start, end), (root, terms) in self.reorders.items():
            # C converts each integer constant to the floating sum it joins,
            # and the fold adds them apart from the run, so each takes the
            # run's type
            integers = {(leaf.start, leaf.end) for leaf, _ in terms if self.constant(leaf) and self.nonfloating(leaf)}
            kind = self.read_kept(root)
            if integers and (isinstance(kind, LayoutError) or category(self.types, kind) not in TYPES):
                self.problems.append(f'{self.toks.where(start)}: {quoted(self.toks[start:end])}: a run SHC sums in '
                                     f'another order, whose floating type the tool can\'t read: {kind}')
                continue
            spelled = category(self.types, kind) if integers else None
            plan.edit(start, end, ordered(terms, self.constant, integers, spelled))
        for span, node in self.divisions.items():
            if span[1] not in self.wraps.get(span[0], ()):
                plan.edit(*span, reciprocal(node))
        for (start, end), pieces in self.written.items():
            plan.edit(start, end, pieces)
        for span, target in self.recast.items():
            spelled = next(name for name, term in FLOATING.items() if term == target)
            plan.edit(*self.factors[span], [f'({spelled})(', self.factors[span], ')'])


def ordered(terms, constant, integers, spelled):
    """The pieces, as Plan.edit() takes them, that write a run of terms, its
    operands and whether the run subtracts each, as SHC sums it: each
    operand that constant() says isn't constant in C's order, from left to
    right, then the fold of the constants, in parentheses, each with its
    sign. Each constant whose span integers holds is cast to spelled, the
    run's floating type."""
    pieces, operands = ['('], [term for term in terms if not constant(term[0])]
    constants = [term for term in terms if constant(term[0])]
    for at, (leaf, negative) in enumerate(operands):
        pieces += [' - ' if negative else ' + '] if at else ['-'] if negative else []
        pieces += ['(', (leaf.start, leaf.end), ')']
    if constants:
        pieces.append(' + (')
        for at, (leaf, negative) in enumerate(constants):
            pieces += [' - ' if negative else ' + '] if at else ['-'] if negative else []
            span = (leaf.start, leaf.end)
            pieces += [f'({spelled})(' if span in integers else '(', span, ')']
        pieces.append(')')
    return pieces + [')']


def volatile(types, term, typedefs):
    """Whether term, a Types term, is volatile, through the typedefs it
    names. A typedef in typedefs is volatile, as a declarator qualifies it
    volatile after its *, which Types doesn't keep. A LayoutError says why
    Types can't resolve term."""
    types.resolve(term)
    while term[0] in ('qualified', 'typedef'):
        if term[0] == 'qualified' and 'volatile' in term[2] or term[0] == 'typedef' and term[1] in typedefs:
            return True
        term = term[1] if term[0] == 'qualified' else next(iter(types.typedefs[term[1]]))
    return False
