"""Check the form, place and cited addresses of each @asm, @hack and @bug
tag, list the open @asm tags, or check that GCC's code may call each piece
of assembly as a function.

    python3 tools/tags.py FILE...
    python3 tools/tags.py --review FILE...
    python3 tools/tags.py --abi [--contract NAME]... FILE...

TAGS.md defines the tags. A tag is @<tag> <kind>: <text>. In C it opens a
plain /* */ comment. In a .src file it starts a ; comment line and runs to
the end of that run of comment lines. This fails, naming each fault's
path:line, on:

- a tag that doesn't open its comment, or a second in one comment;
- a tag in a /** */ comment;
- a tag without a kind TAGS.md gives it, or without text;
- an @hack whose text gives a count, "N of M words differ", names a
  register, or cites an address or an offset of its object's code or data:
  it says what is kept and the plain form that stands for, and
  tools/words.py keeps the measure. A register is R0 to R15, FRn, DRn,
  XDn, FVn, MACH, MACL, FPUL or GBR, a word of its own in capitals, so a C
  name such as FPSCR_PR or a field r15 is none;
- an @hack of any kind but redundant whose text keeps the storage class
  register on a local or a parameter in the words this reads: "declared
  register", "register variable", "register on" or "the register on"
  opening the text, or "a register" before local, parameter, int or
  Float. A storage class fits neither implicit nor type, so TAGS.md's
  first fit is redundant;
- an @asm, but for an open one, whose text doesn't open with a rule of its
  kind and a comma, or cites no address;
- an inline body whose #pragma inline_asm line isn't in inline.py's form;
- an inline body, or a #pragma asm block that holds more than comments,
  without an @asm tag directly above it;
- an @asm tag in C anywhere but directly above a #pragma inline_asm or
  such a block's #pragma asm, and an @asm tag inside any #pragma asm block;
- a .src function without one @asm tag above a label of its, and an @asm tag
  that doesn't end the comment directly above a function's label;
- an @hack in a .src file or a #pragma asm block;
- an if, or a while whose body opens with {, whose condition is an integer
  literal 0 or FALSE, in any parentheses, alone or ahead of a &&, without an
  @hack redundant tag directly above it or in the stack of @hack tags
  directly above it. One on the non-matching side of a guard takes none,
  since SHC never compiles it;
- any other word that starts with @ at the start of a comment.

A .src function is the code after a run of labels in a code section, one of
them exported. A label before data, as a .DATA or .RES directive places,
starts none, though a blank line parts them. A #pragma asm block holds more than comments where one of its
lines holds an instruction, a label or a directive. Its comments are
assembly's, read as a .src file's.

It reads each image's tables for the addresses a tag cites: each 0x and
eight hex digits in its text. A function's code runs from its name in an
image's symbols.txt to the next name or its row's end. It fails, naming the
tag's path:line, on:

- an address an @asm tag cites outside the code it may cite, in every
  image that places its source. That is the code of its function, of the
  hosts of its inline body, and of each function of its source that its
  text names. A host is a function SHC compiles that calls the body by
  name, directly or through a macro's body. A .src function's names are
  its run's labels, each without the _ that assembly puts before a C name.
  A #pragma asm block's tag, and a tag where no image names its function
  or a host, may cite anywhere in its source's code rows;
- an address an @bug tag cites outside every image's sections;
- an offset "of the object's code" past the end of its source's code, or
  "of the object's data" past the end of its source's initialized data.

It checks a tag's form, place and citations, not its argument: whether the
form it names is plainer and builds other code, the bug is one, or the basis
holds. A reviewer checks those, and tools/words.py measures a plain form.

--review fails on nothing and reads no table. It prints each open @asm tag
in TAGS.md's form as path:line and its text: the assembly whose basis is
still to find. Then it counts the @asm tags in that form by kind, those
above #pragma asm blocks among them.

--abi reads no table either. It fails on a tag's form and place as the
plain run does, and on:

- each finding that no --contract names: each place an inline body GCC
  compiles may not be call-safe, as below, and each function whose @asm tag
  is abi, of an inline body or a #pragma asm block GCC compiles, or of a
  .src file;
- a --contract that names no finding;
- an inline body it can't read, whatever --contract names.

GCC compiles each inline body that make dc's compile reads: one on the
matching side of a guard, under the #else of #ifdef NONMATCHING, is SHC's
alone. tools/split.py writes out each other as a function of its own, which
returns where the body runs off its end. The function is call-safe where it
does what SHC's expansion of the body does.

A --contract is NAME:KEY. NAME is a function as C spells it, so an
assembly function goes by its label without the underscore. In a .src file,
that is the label directly below the function's tag. Under a tag above a
#pragma asm block, it is each function the block supplies, as tools/split.py
reads it. KEY is a finding's: the register it reads or changes, as the body
spells it at its entry; FR or SZ for a call or a return with that bit of
FPSCR set; frame for a move of R15 or a store into the caller's frame; PR
for a return through a PR that may not hold the return address; leaves
for a way out where a host goes on; abi
for an @asm tag of abi. One contract answers each finding of its function
with its key, and no other. make dc runs --abi on each source it splits or
assembles, with a --contract for each of the source's pairs in
config/gcc.mk's CONTRACTS. Each answers a finding whose GCC callers meet
the function's contract, or one of a function no GCC code calls.

--abi reads each body GCC compiles whose tag isn't abi under GCC's calling
convention, and finds that its function may not be call-safe where the
body:

- reads a register that no argument passes, or one that a call it makes
  can change;
- takes R15 above its entry, into its caller's frame, or stores where a
  byte may reach that frame: at or above R15's value at entry. A store at
  @(R0,R15) may reach it unless R0 is known: --abi follows R0 through a MOV
  of an immediate and ADDs of immediates. Any store through R15 may reach
  it once --abi no longer knows R15's value against its entry;
- calls or returns with FPSCR's FR or SZ bit set, which GCC's code takes
  clear;
- returns with a register changed that GCC's callers keep across a call,
  R15 among them;
- returns through a PR that may not hold its return address. A call writes
  PR before its slot runs, and a pop restores PR only from the slot a push
  of the return address filled;
- leaves by a return, a jump or a branch out of the body, where a C
  function of its source that GCC compiles goes on past a call of the
  body's function. In SHC's expansion, that way out leaves the caller.

A C function goes on past a call of the body's function unless it calls
that function by name, and ends at each such call, by returning just its
value or with just the call as its last statement. A call through a
macro, function-like or object-like, counts as one of the function's, and
as one it goes on past.

--abi follows R15 through pushes, ADDs of an immediate, and pops into a
register other than R15. It reads a MOV's or an ADD's immediate as the
SH-4 does, a signed byte, so #H'FC is -4. A pop restores a register only
from the slot that a push of its value at entry filled, with no store into
that slot since: a store at @R15 or @(disp,R15) writes the slots its bytes
cover, and one at @(R0,R15) those too where R0 is known, and any slot
where it isn't. --abi reads SP as R15, as asmsh does, and takes a BRAF as
going to any label of the body's code, or out of the body. A slot that a
label marks runs twice in its reading: as its branch's slot, and below the
label.

A body --abi passes may still not be call-safe. --abi takes a call as
returning, and as reading only its target, so it doesn't check the
registers the callee reads. It takes R0 and FR0 as set after a call, so it
doesn't check that the callee returns a value there. It takes a load as reading only its address's
registers, so it misses a read of the caller's frame at an offset from
R15. It takes a push as saving the registers it stores, not reading them,
so it misses a register a call changed that the body pushes, pops back and
then reads. It misses a store through a copy of R15, to a pushed slot or
to the caller's frame.

--abi can't read a body with:

- a parameter of a type it can't place;
- an instruction it doesn't read, such as a BSRF, a BSR to a label of the
  body or a write to FPSCR;
- a delayed branch whose slot it can't place: one with no instruction
  below it, one whose slot a directive stands above, and one whose slot
  branches, jumps, calls or returns;
- a directive that may place bytes, as .DATA.W and .ALIGN may, where a
  path runs into it;
- an instruction that no path reaches, but a directive;
- two paths that meet with FR, SZ or R15 apart.

A .src function is SEGA's or SHC's runtime's own, written as a function, and
--abi reads none of their code: only their tags.
"""

import argparse
import os
import re
from bisect import bisect_left
from collections import Counter, namedtuple
from itertools import chain

import csource
import guards
import inline
import split
from config import Image, code, content, fail, hexa, images, named

KINDS = {'asm': ('opcode', 'operand', 'abi', 'epilogue', 'name', 'open'),
         'hack': ('implicit', 'type', 'redundant', 'spelling'),
         'bug': ('memory', 'uninitialized', 'logic', 'arithmetic', 'mismatched-call')}
# what an @asm tag's text opens with, by its kind: the rule of shckit inspect
# that reports its basis, or what SHC's trailer holds. A name tag opens with
# the member's name
RULES = {'opcode': ('rotcl', 'rotcr', 'swap', 'dmuls', 'dmulu', 'movca', 'cache-line', 'sett', 'cmp-str'),
         'operand': ('fipr-registers', 'fipr-unloaded', 'fipr-shared', 'ftrv-registers', 'fsca-registers',
                     'small-constant', 'integer-constant', 'zero-displacement', 'fschg-slot', 'fschg-pairs',
                     'fschg-dt', 'fschg-stores', 'fschg-register-moves'),
         'abi': ('stack-written', 'fpscr-save', 'pr-save-below-branch', 'return-above-entry', 'return-below-entry'),
         'epilogue': ('dead-return', 'fill')}
TAG = re.compile(r'@(asm|hack|bug)\b')
OPENED = re.compile(r'@(asm|hack|bug) ([a-z-]+): (\S.*)')
COUNT = re.compile(r'(\d+) of (\d+) words differ')
CITED = re.compile(r'\b0x[0-9a-f]{8}\b')
# a run of offsets in a source that several images place, as 0x36e and 0x37e
# of the object's code, or 0xcc of the object's data
OFFSETS = re.compile(r"\b((?:0x[0-9a-f]+(?:,? and |, | to ))*0x[0-9a-f]+) of the object's (code|data)")
# in a .src file: a line that is all comment, a label with what follows it on
# its line, the names an .EXPORT gives, and a section's attribute
REMARK = re.compile(r'\s*;(.*)')
LABEL = re.compile(r'([A-Za-z_$?][\w$]*):(.*)')
EXPORT = re.compile(r'\s+\.EXPORT\s+([^;]*)', re.I)
SECTION = re.compile(r'\s+\.SECTION\s+\w+\s*,\s*(\w+)', re.I)
# a directive that gives a label a value in place of an address, and one
# that places data or reserves room for it, as .DATA.L and .RES.L do
VALUE = re.compile(r'\s*\.(EQU|ASSIGN|REG|FREG)\b', re.I)
DATA = re.compile(r'\s*\.[SFX]?(?:DATA|RES)[BCZ]?\b', re.I)
# an integer literal 0, decimal, octal or hex, with any suffix; or FALSE
ZERO = re.compile(r'(?:0+|0[xX]0+)[uUlL]*|FALSE')
# the switches SHC's build defines: DREAMCAST, and not NONMATCHING
SHC = ('DREAMCAST',)

# a tag as faults() and assembly_faults() read it: its line, its tag (asm,
# hack or bug), its kind, its text and names. names are the functions an
# @asm tag stands for, as the tables name them: an inline body's function
# and its hosts, or a .src function's labels. They are empty for any other
# tag, and for a #pragma asm block's
Tag = namedtuple('Tag', 'line tag kind text names')

# GCC's calling convention under config/gcc.mk's -ml -m4-single-only, by which
# --abi reads a body's function. A function takes its integer arguments in R4
# to R7 and its float ones in FR4 to FR11, and returns in R0 or FR0. A call
# can change R0 to R7, FR0 to FR11, FPUL, T, MACH, MACL and PR. It keeps R8 to
# R14 and FR12 to FR15. --abi names a register of the FPU by its bank: FAn is
# the register FRn names at the body's entry, and FBn the one XFn names
# there. GCC allocates no FB register, so its callers keep none across a call
INTEGER_ARGUMENTS = ('R4', 'R5', 'R6', 'R7')
FLOAT_ARGUMENTS = tuple(f'FA{n}' for n in range(4, 12))
CLOBBERED = {*(f'R{n}' for n in range(8)), *(f'FA{n}' for n in range(12)), 'FPUL', 'T', 'MACH', 'MACL', 'PR'}
KEPT = {*(f'R{n}' for n in range(8, 15)), *(f'FA{n}' for n in range(12, 16))}
# the words that spell a parameter type --abi places in a register, in C or
# as the SDK's typedefs: an integer of 32 bits or fewer, and a float.
# -m4-single-only makes a double a float
INTEGER_TYPES = {'char', 'short', 'int', 'long', 'signed', 'unsigned', 'Sint8', 'Uint8', 'Sint16', 'Uint16', 'Sint32',
                 'Uint32', 'Int', 'Bool', 'Angle'}
FLOAT_TYPES = {'float', 'double', 'Float'}
# the registers a body reads as the machine's, never as an argument: SR's and
# FPSCR's modes, GBR, the exception registers and the FB bank, which holds
# XMTRX. --abi trusts that XMTRX holds the current matrix at the entry of each
# body GCC compiles, as at SHC's expansion: PORTS.md#shcs-c-under-gcc says
# which of GCC's code writes XMTRX, and holds that code to the shipped matrix
MACHINE = {'SR', 'FPSCR', 'GBR', 'VBR', 'SSR', 'SPC', 'SGR', 'DBR', *(f'FB{n}' for n in range(16))}
# each instruction --abi reads, by its operands' roles, a letter each: r read,
# w written, b both, and l a label
ROLES = {roles: mnemonics.split() for roles, mnemonics in {
    'rw': 'MOV MOV.B MOV.W MOV.L MOVA MOVCA.L EXTU.B EXTU.W EXTS.B EXTS.W NEG NOT SWAP.B SWAP.W FMOV FMOV.S FLDS FSTS '
          'FTRC FLOAT FSCA LDS LDS.L LDC LDC.L STS STS.L STC STC.L',
    'rb': 'ADD ADDC ADDV SUB SUBC SUBV AND OR XOR SHAD SHLD FADD FSUB FMUL FDIV FIPR FTRV',
    'rr': 'CMP/EQ CMP/GE CMP/GT CMP/HI CMP/HS CMP/STR TST FCMP/EQ FCMP/GT MUL.L MULS.W MULU.W DMULS.L DMULU.L',
    'rrb': 'FMAC',
    'b': 'DT SHLL SHLR SHAL SHAR SHLL2 SHLL8 SHLL16 SHLR2 SHLR8 SHLR16 ROTL ROTR ROTCL ROTCR FABS FNEG FSQRT FSRRA',
    'w': 'FLDI0 FLDI1 MOVT',
    'r': 'CMP/PL CMP/PZ JMP JSR BRAF PREF OCBI OCBP OCBWB',
    'l': 'BT BF BT/S BF/S BRA BSR',
    '': 'NOP RTS RTE CLRT SETT CLRMAC FSCHG FRCHG'}.items()}
ROLE = {mnemonic: roles for roles, mnemonics in ROLES.items() for mnemonic in mnemonics}
# the registers an instruction writes that no operand names: a product's
# MACL, and MACH too for a product of 64 bits and for CLRMAC
IMPLIED = {'MUL.L': ['MACL'], 'MULS.W': ['MACL'], 'MULU.W': ['MACL'], 'DMULS.L': ['MACH', 'MACL'],
           'DMULU.L': ['MACH', 'MACL'], 'CLRMAC': ['MACH', 'MACL']}
# the instructions that read T, and those that write it: the comparisons and
# tests among ROLES' rr, and no product
READS_T = {'ADDC', 'SUBC', 'ROTCL', 'ROTCR', 'MOVT', 'BT', 'BF', 'BT/S', 'BF/S'}
WRITES_T = {*ROLES['rr'], 'ADDC', 'SUBC', 'ADDV', 'SUBV', 'DT', 'SHLL', 'SHLR', 'SHAL', 'SHAR', 'ROTL', 'ROTR',
            'ROTCL', 'ROTCR', 'CMP/PL', 'CMP/PZ', 'CLRT', 'SETT'} - set(IMPLIED)
# the instructions with a delay slot, the calls among them, and those --abi
# counts as leaving the body whatever their operands. A branch to a label
# leaves the body where the label marks neither an instruction of the body
# nor its end
DELAYED = {'BT/S', 'BF/S', 'BRA', 'BSR', 'JMP', 'JSR', 'BRAF', 'BSRF', 'RTS', 'RTE'}
CALLS = {'BSR', 'JSR', 'BSRF'}
LEAVES = {'JMP', 'BRAF', 'RTS', 'RTE'}
# the directives that place no byte in a body's code, which instructions()
# passes over. One that gives a label a value, as VALUE reads it, places none
# either
PLACES_NOTHING = {'.IMPORT', '.EXPORT', '.GLOBAL'}
# an immediate, in decimal as #-4 or in hex as #H'10
IMMEDIATE = re.compile(r"#(?:(-?\d+)|H'([0-9A-F]+))$", re.I)
# an operand that names registers: a numbered one, which NUMBERED bounds,
# XMTRX or a special register
REGISTER = re.compile(r'(?:(R|FR|DR|XD|FV)(0|[1-9]\d*)|XMTRX|PR|MACH|MACL|FPUL|SR|FPSCR|GBR|VBR|SSR|SPC|SGR|DBR)$',
                      re.I)
# the numbers each prefix takes: R0 to R15, FR0 to FR15, the pairs DR0 to
# DR14 and XD0 to XD14 by twos, and the vectors FV0, FV4, FV8 and FV12. asmsh
# reads a name with any other number, or with a leading zero, as a symbol
NUMBERED = {'R': range(16), 'FR': range(16), 'DR': range(0, 16, 2), 'XD': range(0, 16, 2), 'FV': range(0, 16, 4)}
# a register's name as a word of an @hack's text, in capitals: one that
# NUMBERED numbers, MACH, MACL, FPUL or GBR
REGISTER_NAME = re.compile(r'\b(?:{})\b'.format('|'.join(
    [*(f'{kind}{n}' for kind, numbers in NUMBERED.items() for n in numbers), 'MACH', 'MACL', 'FPUL', 'GBR'])))
# words that show an @hack's text keeps the storage class register on a
# local or a parameter, as "count declared register" or "register on p and
# s". "register on" counts only opening the text, since "an AICA register on
# the G2 bus" names a register of the machine
REGISTER_CLASS = re.compile(
    r'^(?:the )?register on\b|\b(?:declared register|register variables?|a register (?:local|parameter|int|Float))\b')
# an address: @Rn, @-Rn, @Rn+, or @(X,B) with Rn, GBR or PC as its base B. It
# reads Rn or GBR, and R0 too where X is R0
AT = re.compile(r'@(?:(-)?(R\d+)(\+)?|\((?:(R0)|[^,]+),(R\d+|GBR|PC)\))$', re.I)
# SP as a word of an operand, which asmsh reads as R15: no byte a symbol can
# hold stands on either side
SP = re.compile(r'(?<![\w.?$])SP(?![\w.?$])', re.I)
# a C token, an ellipsis, a word or any other character but a blank; and a
# #define with its continuations, its name and the rest
TOKEN = re.compile(r'\.\.\.|\w+|\S')
DEFINE = re.compile(r'^[ \t]*#[ \t]*define[ \t]+(\w+)((?:\\\n|[^\n])*)', re.M)


def block_comments(text):
    """Each /* */ comment of text, C, as csource reads it, so that none lies
    in a string, a character constant or a // comment: (where it starts,
    where it ends, its text as written)."""
    return [(token.start, token.end, text[token.start:token.end]) for token in csource.parse(text).lexemes
            if token.kind == 'comment' and token.text.startswith('/*')]


def flat(comment):
    """A C comment's words on one line, without its marks or the * that opens
    each of its lines."""
    return ' '.join(line.strip().lstrip('*').strip() for line in comment[2:-2].splitlines()).strip()


def fault(body, assembly=False):
    """What keeps body, a comment's words on one line, from TAGS.md's form, or
    None. A comment without a tag has none, unless it opens with another
    word that starts with @. assembly says the comment is a .src file's."""
    tags = TAG.findall(body)
    if not tags:
        return f'{body.split()[0]}: not a tag TAGS.md defines' if body.startswith('@') else None
    opened = OPENED.match(body)
    if len(tags) > 1 or not body.startswith('@'):
        return 'a tag opens a comment of its own'
    if not opened:
        return f'not a tag: @{tags[0]} KIND: TEXT'
    tag, kind, text = opened.groups()
    if kind not in KINDS[tag]:
        return f'{kind}: not a kind of @{tag}; they are {", ".join(KINDS[tag])}'
    if tag == 'hack' and assembly:
        return 'an @hack in assembly: a hack steers SHC, which writes none of a .src file'
    if tag == 'hack' and COUNT.search(text):
        return f"{COUNT.search(text)[0]}: an @hack's text gives no count; tools/words.py measures one"
    if tag == 'hack' and (CITED.search(text) or OFFSETS.search(text)):
        return "an @hack's text cites no address: it says what is kept, and the plain form that stands for"
    if tag == 'hack' and (register := REGISTER_NAME.search(text)):
        return (f"{register[0]}: an @hack's text names no register: it says what is kept, and the plain form that "
                'stands for')
    if tag == 'hack' and kind != 'redundant' and REGISTER_CLASS.search(text):
        return (f'{kind}: an @hack that keeps register on a local or a parameter is redundant, the first kind that '
                'fits a storage class')
    if tag == 'asm' and kind != 'open':
        rule = text.split(',')[0]
        if ',' not in text or (rule not in RULES[kind] if kind in RULES else not re.fullmatch(r'\w+', rule)):
            opens = f'one of {", ".join(RULES[kind])}' if kind in RULES else "the member's name"
            return f"an @asm {kind}'s text opens with {opens}, then a comma"
        if not CITED.search(text):
            return f'an @asm {kind} cites an address, 0x and eight lowercase hex digits'
    return None


def faults(path, text):
    """What keeps the tags of text, the C source or header at path, from
    TAGS.md's form and place, and the tags in that form, as two lists: a
    line for each fault, and each Tag in file order. An @asm tag directly
    above an inline body names the body's function and its hosts: each
    function SHC compiles that calls it by name, directly or through a
    macro's body."""
    found, tags = [], []
    # each @hack tag's kind and first line, by the line directly below it
    hacks = {}

    def line(at):
        return text.count('\n', 0, at) + 1

    # where the line directly under each @asm tag starts, and each #pragma
    # asm block's lines, by its opening line's index
    under_asm = asm_above(text)
    blocks = split.assembly_blocks(text)
    starts = [0, *(newline.end() for newline in re.finditer('\n', text))]
    # where the opening line of each block that holds more than comments
    # starts: each takes an @asm tag directly above it
    openings = {starts[at] for at, block in blocks.items() if any(each.partition(';')[0].strip() for each in block)}
    # the functions and macros SHC compiles, read once a tag above a body
    # needs them
    shc_definitions = None
    for start, end, comment in block_comments(text):
        body, number = flat(comment), line(start)
        if comment.startswith('/**') and TAG.match(body):
            found.append(f'{path}:{number}: a tag opens a plain /* */ comment, not a /** */ one')
            continue
        if problem := fault(body):
            found.append(f'{path}:{number}: {problem}')
            continue
        if opened := OPENED.match(body):
            names = ()
            if opened[1] == 'asm' and (pragma := inline.FORM.match(text, end + 1)):
                shc_definitions = shc_definitions or definitions(side(text, SHC))
                names = (pragma[1], *hosts(pragma[1], *shc_definitions))
            tags.append(Tag(number, *opened.groups(), names))
            if opened[1] == 'hack':
                hacks[line(end) + 1] = opened[2], number
        if body.startswith('@asm') and not inline.NAMED.match(text, end + 1) and end + 1 not in openings:
            found.append(f"{path}:{number}: an @asm tag goes directly above an inline body's #pragma inline_asm or "
                         "the #pragma asm of a block that holds more than comments")
    for pragma in inline.NAMED.finditer(text):
        if not inline.FORM.fullmatch(pragma[0]):
            found.append(f'{path}:{line(pragma.start())}: not #pragma inline_asm(NAME) or '
                         '#pragma inline_asm(NAME(size=BYTES))')
        elif pragma.start() not in under_asm:
            found.append(f'{path}:{line(pragma.start())}: an inline body without an @asm tag directly above it')
    for at, block in blocks.items():
        if starts[at] in openings and starts[at] not in under_asm:
            found.append(f'{path}:{at + 1}: a #pragma asm block without an @asm tag directly above it')
        # blank lines ahead of the block's give the assembly the C file's numbers
        faulted, held = assembly_faults(path, '\n' * (at + 1) + '\n'.join(block), in_block=True)
        found += faulted
        tags += held
    untagged = [(at, test) for at, test in never_run(text) if not redundant(at, hacks)]
    if untagged:
        # SHC never compiles a guard's non-matching side, so no hack stands there
        skipped = csource.nonmatching_side(csource.blanked(text).split('\n'))
        found += [f'{path}:{at}: {test} has no @hack redundant tag directly above it' for at, test in untagged
                  if at - 1 not in skipped]
    return sorted(found, key=lambda each: int(each.split(':')[1])), sorted(tags)


def asm_above(text):
    """Each comment of text, a C source, that opens with @asm, by where the
    line below it starts, as its tag's kind, or None where the tag isn't in
    TAGS.md's form."""
    above = {}
    for _, end, comment in block_comments(text):
        body = flat(comment)
        if body.startswith('@asm'):
            opened = OPENED.match(body)
            above[end + 1] = opened[2] if opened else None
    return above


def redundant(line, hacks):
    """Whether an @hack redundant tag stands directly above line, alone or in
    the stack of @hack tags there. hacks gives each @hack tag's kind and
    first line, by the line directly below it."""
    while line in hacks:
        kind, line = hacks[line]
        if kind == 'redundant':
            return True
    return False


def zero(tokens):
    """The literal that keeps a condition, the texts of its tokens, from ever
    holding: an integer 0 or FALSE, in any parentheses, alone or ahead of a
    &&. It returns the literal and whether a && follows it, or None where it
    finds no such literal."""
    depth, first = 0, len(tokens)
    for at, token in enumerate(tokens):
        depth += (token in '([') - (token in ')]')
        if depth == 0 and token in ('||', '?', ','):
            return None
        if depth == 0 and token == '&&':
            first = min(first, at)
    head, more = tokens[:first], first < len(tokens)
    if len(head) == 1 and ZERO.fullmatch(head[0]):
        return head[0], more
    if len(head) > 2 and head[0] == '(' and csource.pairs(head)[0] == len(head) - 1:
        inner = zero(head[1:-1])
        return inner and (inner[0], inner[1] or more)
    return None


def never_run(text):
    """Each if of C text, and each while whose body opens with {, whose
    condition zero() reads as never true, as its line and the test as lint
    names it, as "if (0 && ...)". A directive's #if is neither."""
    c = csource.parse(text)
    opened = {each.start for each in c.directives}
    for line, spelled in enumerate(c.blank.split('\n')):
        # a line whose text spells neither word holds neither
        if 'if' not in spelled and 'while' not in spelled:
            continue
        found = [token for token in c.between(c.lines[line], c.lines[line + 1]) if token.kind != 'comment']
        for at, token in enumerate(found):
            if token.kind != 'name' or token.text not in ('if', 'while') or \
                    at and found[at - 1].text == '#' and found[at - 1].start in opened:
                continue
            after = (each.text for each in chain(found[at + 1:], c.onward(line + 1)) if each.kind != 'comment')
            if next(after, None) != '(':
                continue
            condition, depth = [], 1
            for each in after:
                depth += (each == '(') - (each == ')')
                if not depth:
                    break
                condition.append(each)
            zeroed = zero(condition)
            if zeroed and (token.text == 'if' or next(after, None) == '{'):
                yield token.line, f'{token.text} ({zeroed[0]}{" && ..." * zeroed[1]})'


def remarks(path, above, found):
    """Check the comment above, a .src file's run of comment lines as (number,
    words), adding a line to found for each fault. A tag runs from the first
    line that opens with @ to the comment's end. Returns the comment's Tag,
    which names no function yet, or None where it has none in TAGS.md's
    form."""
    first = next((at for at, (_, words) in enumerate(above) if words.startswith('@')), len(above))
    for number, words in above[:first]:
        if problem := fault(words, assembly=True):
            found.append(f'{path}:{number}: {problem}')
    if first == len(above):
        return None
    number, body = above[first][0], ' '.join(words for _, words in above[first:])
    if problem := fault(body, assembly=True):
        found.append(f'{path}:{number}: {problem}')
        return None
    return Tag(number, *OPENED.match(body).groups(), ())


def run_faults(path, labels, tags, function):
    """The faults of a run of labels, as (number, name), under tags, the
    numbers of the @asm tags' lines above them. function says the run names
    a function, which takes one tag; any other run takes none."""
    number, name = labels[0]
    if not function:
        return [f"{path}:{tag}: an @asm tag goes above a function's label, and {name} names none" for tag in tags]
    if not tags:
        return [f'{path}:{number}: {name}: a function without an @asm tag']
    return [f'{path}:{tag}: {name}: a second @asm tag for one function' for tag in tags[1:]]


def symbol(label):
    """The name the tables give a label: its C spelling, without the _ that
    assembly puts before a C name."""
    return label[1:] if label.startswith('_') else label


def assembly_faults(path, text, in_block=False):
    """What keeps the tags of text, the .src file at path, from TAGS.md's
    form and place, and the tags in that form, as faults() gives them. An
    @asm tag above a function's label names the run's labels as the tables
    do. in_block says text is a #pragma asm block's, whose @asm tag stands
    in C above it: the block holds no @asm tag, and its functions take none
    of their own."""
    lines = text.split('\n')
    exported = {name.strip() for line in lines if (given := EXPORT.match(line)) for name in given[1].split(',')}
    found, tags, code = [], [], True
    # the comment lines directly above the line in hand, and the run of
    # labels directly above it, with the indexes in tags of the @asm tags
    # above those
    above, labels, asm_tags = [], [], []
    # a last line that is no comment and no label ends what is still open
    for number, line in enumerate([*lines, ''], 1):
        if words := REMARK.fullmatch(line):
            above.append((number, words[1].strip()))
            continue
        label = LABEL.match(line)
        if label and VALUE.match(label[2]):
            label = None
        tag = remarks(path, above, found)
        above = []
        if in_block and tag and tag.tag == 'asm':
            found.append(f'{path}:{tag.line}: an @asm tag inside a #pragma asm block: a block takes its tag in C, '
                         'directly above its #pragma asm')
            tag = None
        tags += [tag] * (tag is not None)
        asm = tag is not None and tag.tag == 'asm'
        if label:
            labels.append((number, label[1]))
            asm_tags += [len(tags) - 1] * asm
            if not label[2].strip() or REMARK.fullmatch(label[2]):
                continue
        elif asm:
            found.append(f"{path}:{tag.line}: an @asm tag ends the comment directly above a function's label")
        # a blank line under a run of labels leaves to the statement below it whether they name a function
        if labels and not label and not line.strip() and number <= len(lines):
            continue
        if labels:
            names = tuple(symbol(name) for _, name in labels)
            for at in asm_tags:
                tags[at] = tags[at]._replace(names=names)
            # the run's first statement, on its last label's line or below
            below = label[2] if label else line
            if not in_block:
                found += run_faults(path, labels, [tags[at].line for at in asm_tags], code and not DATA.match(below)
                                    and exported.intersection(name for _, name in labels))
            labels, asm_tags = [], []
        if section := SECTION.match(line):
            code = section[1].upper() == 'CODE'
    return sorted(found, key=lambda each: int(each.split(':')[1])), tags


class Layout:
    """Where the tables put the code and the sections that tags cite: each
    source's code rows in every image that places it, the code of each
    function its rows name, the size of its code and of its initialized
    data, and every image's sections."""

    def __init__(self, tables):
        self.code, self.functions, self.sizes, self.sections = {}, {}, {'code': {}, 'data': {}}, []
        for image in tables:
            self.sections += [(start, end) for _, start, end in image.sections]
            symbols = named(image.path / 'symbols.txt')
            addresses = [address for address, _ in symbols]
            for source in image.sources():
                ranges = list(image.ranges(source))
                rows = [(start, end) for section, start, end in ranges if code(section)]
                self.code.setdefault(source, []).extend(rows)
                # a function's code runs from its name's address to the next
                # higher one a name holds, or to its row's end
                functions = self.functions.setdefault(source, {})
                for start, end in rows:
                    inside = symbols[bisect_left(addresses, start):bisect_left(addresses, end)]
                    for at, (address, name) in enumerate(inside):
                        after = next((later for later, _ in inside[at + 1:] if later > address), end)
                        functions.setdefault(name, []).append((address, after))
                # SHC names a source's initialized data D, or DSG in the SDK
                held = {'code': rows, 'data': [(start, end) for section, start, end in ranges if section[0] == 'D']}
                for kind, sizes in self.sizes.items():
                    sizes[source] = max(sizes.get(source, 0), sum(end - start for start, end in held[kind]))

    def stray(self, path, tag):
        """Each citation of tag, a Tag of the file at path, that lies outside
        where TAGS.md puts it, as a line naming it. An @asm tag may cite the
        code of the functions it names, and of each function of its source
        that its text names. Where no image names any of the functions it
        names, it may cite anywhere in its source's code."""
        where, found = f'{path}:{tag.line}', []
        functions = self.functions.get(path, {})
        # the functions whose code the tag may cite, and what a fault names:
        # their code, or where the tables name none, the source's
        citable = set(tag.names) & functions.keys()
        if citable:
            citable |= set(re.findall(r'\w+', tag.text)) & functions.keys()
            *others, last = sorted(citable)
            listed = f"{', '.join(others)} and {last}" if others else last
            scope = f'the code of {listed}'
        else:
            scope = "its source's code in every image that places it"
        rows = [row for name in citable for row in functions[name]] or self.code.get(path, [])
        for address in map(lambda each: int(each, 16), CITED.findall(tag.text)):
            if tag.tag == 'bug' and not any(start <= address < end for start, end in self.sections):
                found.append(f"{where}: {hexa(address)} lies in no image's sections; a constant is spelled "
                             f"H'{address:08X}")
            elif tag.tag == 'asm' and not any(start <= address < end for start, end in rows):
                found.append(f'{where}: {hexa(address)} lies outside {scope}')
        for run, kind in OFFSETS.findall(tag.text):
            sizes = self.sizes[kind]
            for offset in re.findall(r'0x[0-9a-f]+', run):
                if path in sizes and int(offset, 16) >= sizes[path]:
                    found.append(f"{where}: {offset} of the object's {kind} lies past its end, at {sizes[path]:#x}")
        return found


def registers(operand, fr, paired):
    """The registers an operand names, or None for a constant, a label or an
    address. fr is FPSCR's FR bit, which swaps the banks FRn and XFn name.
    paired is whether the instruction moves pairs, as an FMOV does with
    FPSCR's SZ bit set. FRn then names DRn for an even n, and XD(n-1) for an
    odd n."""
    match = REGISTER.match(operand)
    if not match:
        return None
    kind, n = (match[1] or match[0]).upper(), int(match[2] or 0)
    if kind in NUMBERED and n not in NUMBERED[kind]:
        return None
    if kind == 'R':
        return [f'R{n}']
    if kind == 'FR' and paired:
        kind, n = ('XD', n - 1) if n % 2 else ('DR', n)
    if kind not in ('FR', 'DR', 'XD', 'FV', 'XMTRX'):
        return [kind]
    bank = 'FB' if (kind in ('XD', 'XMTRX')) != fr else 'FA'
    return [f'{bank}{i}' for i in range(n, n + {'FR': 1, 'FV': 4, 'XMTRX': 16}.get(kind, 2))]


# an instruction's registers: those it reads, those it writes and those it
# pushes, each a list
Effects = namedtuple('Effects', 'reads writes pushed')


def effects(mnemonic, operands, fr, sz):
    """An instruction's Effects, or None for an instruction --abi doesn't read.
    fr and sz are FPSCR's FR and SZ bits. A push at @-R15 counts the
    registers it stores as read and as pushed. T counts as a register, and an
    RTS reads PR."""
    roles = ROLE.get(mnemonic)
    if roles is None or len(roles) != len(operands):
        return None
    paired = sz and mnemonic.startswith('FMOV')
    reads, writes = [], []
    for role, operand in zip(roles, operands):
        if address := AT.match(operand):
            base = [address[2] or address[5]] if (address[5] or '').upper() != 'PC' else []
            reads += base + ['R0'] * bool(address[4])
            writes += base * bool(address[1] or address[3])
        elif role != 'l' and (names := registers(operand, fr, paired)):
            reads += names * (role in 'rb')
            writes += names * (role in 'wb')
    reads += ['T'] * (mnemonic in READS_T) + ['PR'] * (mnemonic == 'RTS')
    writes += ['T'] * (mnemonic in WRITES_T) + IMPLIED.get(mnemonic, [])
    pushed = registers(operands[0], fr, paired) if [o.upper() for o in operands[1:]] == ['@-R15'] else None
    return Effects([read.upper() for read in reads], [write.upper() for write in writes], pushed or [])


def instructions(lines):
    """A body's instructions and labels, given its lines. Each instruction is
    (its index in lines, its mnemonic in capitals, its operands without
    blanks and with SP as R15). A directive that may place bytes, as .DATA.W
    and .ALIGN may, is one too, so that a path can't run through its bytes
    unseen. Each label maps to the index of the instruction below it. It
    maps to None where it takes a value, as .EQU gives one, and to the count
    of instructions where none follows. A blank or a tab parts a mnemonic
    from its operands."""
    found, labels, waiting = [], {}, []
    for i, line in enumerate(lines):
        statement = line.split(';')[0]
        while label := re.match(r'\s*([\w?]+):', statement):
            waiting.append(label[1])
            statement = statement[label.end():]
        mnemonic, rest = (statement.split(None, 1) + ['', ''])[:2]
        if not mnemonic or mnemonic.upper() in PLACES_NOTHING:
            continue
        labels.update(dict.fromkeys(waiting, None if VALUE.match(statement) else len(found)))
        waiting = []
        if VALUE.match(statement):
            continue
        operands = re.split(r',(?![^(]*\))', ''.join(rest.split())) if rest.strip() else []
        found.append((i, mnemonic.upper(), [SP.sub('R15', operand) for operand in operands]))
    labels.update(dict.fromkeys(waiting, len(found)))
    return found, labels


def is_directive(mnemonic):
    """Whether an instruction, as instructions() gives it, is a directive."""
    return mnemonic.startswith('.')


def spelling(register):
    """A register as the body spells it at its entry: FA0 as FR0, and FB0 as
    XF0."""
    return {'FA': 'FR', 'FB': 'XF'}.get(register[:2], register[:2]) + register[2:]


def refusal(lines, code, labels, k):
    """Why --abi can't read code[k], or None where it can. lines are a body's
    lines, and code and labels its instructions and labels, as
    instructions() gives them. --abi doesn't read a BSR to a label of the
    body, a write to FPSCR, or an instruction ROLES doesn't hold with its
    operand count. It can't place a delayed branch's slot where no
    instruction follows the branch. It can't place one that a directive
    stands above, or one that branches, jumps, calls or returns. A directive
    is refused only where a path runs into it, as call_safety() finds."""
    _, mnemonic, operands = code[k]
    if is_directive(mnemonic):
        return None
    # FR and SZ change only which registers an operand names, not whether
    # effects() reads it or finds FPSCR written
    effect = effects(mnemonic, operands, False, False)
    if effect is None or 'FPSCR' in effect.writes or (mnemonic == 'BSR' and labels.get(operands[0]) is not None):
        return "which --abi doesn't read"
    if mnemonic not in DELAYED:
        return None
    if k + 1 == len(code) or code[k + 1][1] in DELAYED | {'BT', 'BF'} or is_directive(code[k + 1][1]):
        return "whose slot --abi can't place"
    # instructions() keeps no directive that places nothing, so what a line
    # between the branch's and its slot's states after its labels is one
    if any(re.sub(r'^(\s*[\w?]+:)*', '', line.split(';')[0]).strip()
           for line in lines[code[k][0] + 1:code[k + 1][0]]):
        return "whose slot --abi can't place"
    return None


def edges(code, labels):
    """The paths through a body's instructions, as (after, ran, slots). A
    path's nodes are the instructions, node k for code[k], then the end of
    the body, node len(code). after gives for each node the nodes control can
    reach next, with None for a way out of the body, ran the index in code of
    the instruction each node runs, and slots the index of the delayed branch
    whose slot each slot's node is. A delayed branch takes effect after its
    slot, so the slot's node holds the branch's edges. Where a label marks
    the slot, a path that reaches the label goes on below it, so the slot
    the branch runs is a node of its own, past the end's. code and labels
    are the body's instructions and labels, as instructions() gives them, and
    refusal() must pass each instruction."""
    after = {k: [k + 1] for k in range(len(code))}
    ran = {k: k for k in range(len(code))}
    slots = {}
    marked = sorted({to for to in labels.values()
                     if to is not None and (to == len(code) or not is_directive(code[to][1]))})
    for k, (_, mnemonic, operands) in enumerate(code):
        to = labels.get(operands[0]) if ROLE.get(mnemonic) == 'l' else None
        slot = k
        if mnemonic in DELAYED:
            slot = k + 1
            if slot in marked:
                # ran holds a node for each instruction and each such slot so far
                slot = len(ran) + 1
                ran[slot], after[k], after[slot] = k + 1, [slot], [k + 2]
            slots[slot] = k
        if mnemonic in ('BT', 'BF', 'BT/S', 'BF/S'):
            after[slot] = [ran[slot] + 1, to]
        elif mnemonic == 'BRAF':
            after[slot] = [None, *marked]
        elif mnemonic in LEAVES or mnemonic == 'BRA':
            after[slot] = [to]
    return after, ran, slots


def moved(slots, offset):
    """The stack's slots once R15 moves offset bytes other than by a push: up
    where offset is positive, and down where it is negative. The slots are
    (size, registers) pairs, top last. A slot's registers are those whose
    values at entry the push that made it stored, or () for a slot a move
    made. A move up drops the slots it passes. Where it stops inside a slot,
    the rest of that slot becomes a slot of its own. A move down makes a slot
    of the bytes it passes."""
    while slots and offset > 0:
        offset -= slots[-1][0]
        slots = slots[:-1]
    return slots + ((-offset, ()),) if offset < 0 else slots


def overwritten(slots, operand, size, r0):
    """The stack's slots, as moved() gives them, once a store of size bytes
    to operand runs. A store at @R15 or @(disp,R15) writes the slots its
    bytes cover, which then hold no register's value at entry, and so does
    one at @(R0,R15) where r0, R0's value, is known. Where it isn't, the
    store can write any slot. A store elsewhere writes none."""
    if operand == '@R15':
        first = 0
    elif found := re.fullmatch(r"@\((?:(\d+)|H'([0-9A-F]+)|(R0)),R15\)", operand):
        if found[3] and r0 is None:
            return tuple((each, ()) for each, _ in slots)
        first = r0 if found[3] else int(found[1]) if found[1] else int(found[2], 16)
    else:
        return slots
    kept, at = [], 0
    for each, held in reversed(slots):
        kept.append((each, () if at < first + size and first < at + each else held))
        at += each
    return tuple(reversed(kept))


# what --abi knows on entry to a body's instruction, from the paths that reach
# it so far. defined holds the registers every path has set. clobbered maps
# each register a call can have changed since a path set it, and which the
# path hasn't written since, to the call's line. changed holds the kept
# registers a path has changed. slots holds the stack's slots, as moved()
# gives them, where every path leaves the same slots, and none where paths
# leave different ones. fr and sz are FPSCR's FR and SZ bits. stack is R15
# less its value at entry. It is None once a path sets R15 other than by a
# push, an ADD of an immediate or a pop into another register. r0 is R0's
# value where every path has set it by a MOV of an immediate and ADDs of
# immediates since, and None otherwise. pr is None where every path leaves PR
# holding the body's return address, and otherwise what took it from PR on
# a path, as "line 9 writes"
State = namedtuple('State', 'defined clobbered changed slots fr sz stack r0 pr')


def immediate(operand):
    """The value of an immediate operand, or None for any other."""
    found = IMMEDIATE.match(operand)
    return None if found is None else int(found[1]) if found[1] else int(found[2], 16)


def byte(operand):
    """The value a MOV or an ADD of an immediate operand gives its register:
    the immediate's byte, sign-extended, so that #H'FC adds -4. It is None
    for any other operand, and for an immediate no byte holds."""
    value = immediate(operand)
    return None if value is None or not -128 <= value <= 255 else (value + 128) % 256 - 128


def stored(state, mnemonic, upper, size):
    """Whether an instruction stores through R15 where it may reach its
    caller's frame: a byte at or above R15's value at entry. upper are the
    instruction's operands in capitals, and size the bytes it moves. A store
    at @(R0,R15) may reach it where R0 isn't known, and any store through
    R15 may where R15's value against its entry isn't."""
    if ROLE.get(mnemonic) != 'rw' or len(upper) != 2:
        return False
    if upper[1] == '@R15':
        offset = 0
    elif upper[1] == '@-R15':
        # a push stores just below R15, so where R15 is known it reaches the
        # frame only from above the entry, which step() names on its own
        return state.stack is None
    elif upper[1] == '@(R0,R15)':
        offset = state.r0
    elif found := re.fullmatch(r"@\(([^,]+),R15\)", upper[1]):
        offset = immediate('#' + found[1])
    else:
        return False
    return offset is None or state.stack is None or state.stack + offset + size > 0


def step(state, mnemonic, operands, line):
    """The state once an instruction at line runs from state, and each
    finding there, as (line, key, what the body does): a read of a register
    that no argument passes or that a call can have changed, keyed by the
    register, a read of PR once it no longer holds the return address, keyed
    PR, and a move of R15 above its entry or a store that may reach its
    caller's frame, keyed frame. effects() must read the instruction. A call
    writes PR before its slot runs."""
    reads, writes, pushed = effects(mnemonic, operands, state.fr, state.sz)
    found = set()
    # a push saves the registers it stores and doesn't read them, so a body
    # can save a kept register no argument passes
    for register in set(reads) - set(pushed):
        if register == 'PR':
            if state.pr is not None:
                found.add((line, 'PR', f'it reads PR at line {line}, which {state.pr}'))
        elif register in state.clobbered:
            found.add((line, spelling(register), f'it reads {spelling(register)} at line {line}, which the call at '
                                                 f'line {state.clobbered[register]} can change'))
        elif register not in state.defined and register not in MACHINE:
            found.add((line, spelling(register), f'it reads {spelling(register)} at line {line}, which no argument '
                                                 'passes'))
    upper = [operand.upper() for operand in operands]
    size = 8 if mnemonic.startswith('FMOV') and state.sz else {'.B': 1, '.W': 2}.get(mnemonic[-2:], 4)
    if stored(state, mnemonic, upper, size):
        found.add((line, 'frame', f"it may store into its caller's frame at line {line}"))
    r0 = state.r0
    if 'R0' in writes:
        value = byte(upper[0]) if upper[1:] == ['R0'] else None
        r0 = value if mnemonic == 'MOV' else r0 + value if mnemonic == 'ADD' and None not in (r0, value) else None
    # a pop restores the registers it loads only from a slot that holds their
    # whole values at entry
    popped = upper[:1] == ['@R15+']
    loaded = tuple(register for register in writes if register != 'R15')
    restored = popped and state.slots[-1:] == ((size, loaded),)
    if restored:
        changed = state.changed - set(loaded)
    else:
        changed = state.changed | (set(writes) & KEPT)
    pr = state.pr
    if mnemonic in CALLS:
        pr = f'the call at line {line} can change'
    elif 'PR' in writes:
        pr = None if restored else f'line {line} writes'
    # the registers that still hold their values at entry, which a push saves
    held = KEPT - changed | ({'PR'} if state.pr is None else set())
    slots = overwritten(state.slots, upper[-1], size, state.r0) if len(upper) > 1 else state.slots
    stack = state.stack
    if 'R15' in writes and stack is not None:
        added = byte(upper[0]) if mnemonic == 'ADD' and upper[1:] == ['R15'] else None
        if upper[1:] == ['@-R15']:
            stack -= size
            slots += ((size, tuple(pushed) if size >= 4 and set(pushed) <= held else ()),)
        elif popped and upper[1:] != ['R15']:
            stack += size
            slots = moved(slots, size)
        elif added is not None:
            stack += added
            slots = moved(slots, added)
        else:
            stack, slots = None, ()
        if stack is not None and stack > 0:
            found.add((line, 'frame', f"it takes R15 above its entry at line {line}, into its caller's frame"))
    clobbered = {register: at for register, at in state.clobbered.items() if register not in writes}
    return State(state.defined | set(writes), clobbered, changed, slots, state.fr ^ (mnemonic == 'FRCHG'),
                 state.sz ^ (mnemonic == 'FSCHG'), stack, r0, pr), found


def after_call(state, line, call):
    """The state once a call returns, and each finding there, as (line, key,
    what the body does). state is the state after the call's slot, line the
    slot's line and call the call's. A finding is a call made with FPSCR's FR
    or SZ bit set, keyed by the bit. The callee can change each register
    CLOBBERED holds, but PR, which the call itself writes, as step() reads
    it. --abi takes R0 and FA0 as set after the call, since a callee returns
    its value in one of them, though to no value it knows."""
    found = {(line, bit, f"its call at line {call} runs with FPSCR's {bit} bit set, which GCC's code takes clear")
             for bit, on in (('FR', state.fr), ('SZ', state.sz)) if on}
    clobbered = {**state.clobbered, **dict.fromkeys(state.defined & CLOBBERED - {'R0', 'FA0', 'PR'}, call)}
    return state._replace(defined=state.defined - CLOBBERED | {'R0', 'FA0'}, clobbered=clobbered, r0=None), found


def return_findings(state, line):
    """Each finding at a way out of the body at line, as (line, key, what
    the body does): a return with a register changed that GCC's callers
    keep, R15 among them, keyed by the register, or with FPSCR's FR or SZ
    bit set, keyed by the bit."""
    changed = state.changed | ({'R15'} if state.stack != 0 else set())
    return ({(line, spelling(register), f'it returns at line {line} with {spelling(register)} changed, which its '
                                        'GCC callers keep')
             for register in changed}
            | {(line, bit, f"it returns at line {line} with FPSCR's {bit} bit set, which GCC's code takes clear")
               for bit, on in (('FR', state.fr), ('SZ', state.sz)) if on})


def joined(old, new):
    """The state on entry to an instruction once a path that brings new joins
    the paths that brought old. It is None where the two have FR, SZ or R15
    apart."""
    if (old.fr, old.sz, old.stack) != (new.fr, new.sz, new.stack):
        return None
    return old._replace(defined=old.defined & new.defined, clobbered={**new.clobbered, **old.clobbered},
                        changed=old.changed | new.changed, slots=old.slots if old.slots == new.slots else (),
                        r0=old.r0 if old.r0 == new.r0 else None, pr=old.pr or new.pr)


def call_safety(path, name, lines, first, passed, problems):
    """The first instruction that leaves a body, and each finding: a place
    where GCC's function for the body may do otherwise than SHC's expansion,
    whatever the body's callers do. lines are the body's lines, of which the
    first is line first of path, and passed the registers that pass the
    function's arguments. The instruction is (its line, its statement), or
    None where none leaves. The findings are sorted (line, key, what the
    body does) triples, those that step(), after_call() and return_findings()
    give. The RTS that tools/split.py writes after the body is a finding too,
    keyed PR, where the PR it reads may no longer hold the return address: a
    call writes PR, and a pop restores it only from the slot a push of it
    filled while it held the return address.

    Where --abi can't read the body, call_safety() returns None and puts
    each reason on problems, naming path and its line. Each instruction
    refusal() names is a reason. Where refusal() names none, the reason is a
    place where two paths meet with FR, SZ or R15 apart. Where no paths meet
    so, the reason is the first instruction no path reaches."""
    code, labels = instructions(lines)
    statements = [' '.join(lines[i].split(';')[0].split()) for i, _, _ in code]
    refused = [f'{path}:{first + i}: {name}: {statements[k]}, {why}'
               for k, (i, _, _) in enumerate(code) if (why := refusal(lines, code, labels, k))]
    if refused:
        problems += refused
        return None
    after, ran, slots = edges(code, labels)
    # the first instruction that leaves the body: a jump, a return, or a
    # branch that DELAYED's comment says leaves it
    leaving = next((k for k, (_, mnemonic, operands) in enumerate(code) if mnemonic in LEAVES or (
        ROLE.get(mnemonic) == 'l' and mnemonic != 'BSR' and labels.get(operands[0]) is None)), None)
    # each node's line: its instruction's, and for the end the body's last
    # line, which stands for the RTS that tools/split.py writes after it
    numbers = {node: first + code[k][0] for node, k in ran.items()}
    numbers[len(code)] = first + len(lines) - 1
    states = {0: State(frozenset([*passed, 'R15', 'PR']), {}, frozenset(), (), False, False, 0, None, None)}
    work, found = [0], set()
    while work:
        k = work.pop()
        state, line = states[k], numbers[k]
        if k == len(code):
            found |= return_findings(state, line)
            if state.pr is not None:
                found.add((line, 'PR', f'its return reads PR, which {state.pr}'))
            continue
        if is_directive(code[ran[k]][1]):
            problems.append(f"{path}:{line}: {name}: {statements[ran[k]]}, which a path of --abi's runs as code")
            return None
        state, more = step(state, *code[ran[k]][1:], line)
        found |= more
        # a call's slot runs before the callee
        if k in slots and code[slots[k]][1] in CALLS:
            state, more = after_call(state, line, numbers[slots[k]])
            found |= more
        for to in after[k]:
            if to is None:
                found |= return_findings(state, line)
                continue
            new = joined(states[to], state) if to in states else state
            if new is None:
                problems.append(f"{path}:{numbers[to]}: {name}: two paths meet here with FPSCR's FR or SZ bits or "
                                "R15 apart, which --abi can't read")
                return None
            if new != states.get(to):
                states[to] = new
                work.append(to)
    reached = {ran[node] for node in states if node != len(code)}
    unreached = next((k for k in range(len(code)) if k not in reached and not is_directive(code[k][1])), None)
    if unreached is not None:
        problems.append(f"{path}:{numbers[unreached]}: {name}: {statements[unreached]}, which no path of --abi's "
                        "reaches")
        return None
    return ((numbers[leaving], statements[leaving]) if leaving is not None else None), sorted(found)


def arguments(head, name):
    """The registers that pass a function's arguments, and the parameters
    whose type --abi can't place, as two lists, given its head, the C from
    the function's #pragma inline_asm line to its opening brace, and its
    name. Its pointers and integers take R4 to R7 in order, and its floats
    FA4 to FA11. INTEGER_TYPES and FLOAT_TYPES spell the integers' and the
    floats' types. A parameter past R7 or FA11 passes on the stack, in no
    register. --abi can't place a parameter of any other type, a long long
    and a variadic tail among them. A comment in head counts as a blank."""
    head = csource.blanked(head)
    tokens = TOKEN.findall(head[re.search(rf'\b{name}\s*\(', head).end():])
    # the parameters, split at each comma outside parentheses, up to the ) that
    # closes the list
    parameters, depth = [[]], 0
    for token in tokens:
        depth += (token == '(') - (token == ')')
        if depth < 0:
            break
        if token == ',' and not depth:
            parameters.append([])
        else:
            parameters[-1].append(token)
    passed, unplaced = [], []
    for each in parameters if parameters not in ([[]], [['void']]) else []:
        words = [word for word in each if re.fullmatch(r'\w+', word) and word not in ('const', 'volatile')]
        # the parameter's name, where it has one, follows its type
        if len(words) > 1 and words[-1] not in INTEGER_TYPES | FLOAT_TYPES:
            words.pop()
        if '*' in each or '[' in each or (words and set(words) <= INTEGER_TYPES and words.count('long') < 2):
            candidates = INTEGER_ARGUMENTS
        elif len(words) == 1 and words[0] in FLOAT_TYPES:
            candidates = FLOAT_ARGUMENTS
        else:
            unplaced.append(' '.join(each).replace(' * ', ' *'))
            continue
        passed += [register for register in candidates if register not in passed][:1]
    return passed, unplaced


def side(text, defined=guards.SWITCHES):
    """text, C, as a compile that defines the switches in defined reads it,
    for the C around its inline bodies: each body, comment and literal
    blank, and each line on a side the compile doesn't read, as
    tools/split.py finds it. make dc's compile defines both switches. Every
    newline stays, so a place keeps its line."""
    def blank(found):
        return re.sub(r'[^\n]', ' ', found[0])
    read = split.compiled(text, defined)
    text = csource.blanked(inline.BODY.sub(blank, text))
    return '\n'.join(line if read[i] is not False else '' for i, line in enumerate(text.split('\n')))


def definitions(text):
    """The functions and the macros of text, C as side() gives it, as
    {name: tokens} and {name: (function-like, tokens)}. A function's tokens
    are those between its braces, and a macro's its replacement: those after
    its parameters where it takes them, and all of its rest where it
    doesn't."""
    functions, macros = {}, {}
    for define in DEFINE.finditer(text):
        rest = define[2].replace('\\\n', ' ')
        if rest.startswith('('):
            macros[define[1]] = True, TOKEN.findall(rest[rest.index(')') + 1:])
        else:
            macros[define[1]] = False, TOKEN.findall(rest)
    tokens = TOKEN.findall(DEFINE.sub(lambda found: re.sub(r'[^\n]', ' ', found[0]), text))
    i = 0
    while i < len(tokens):
        if tokens[i] != '{':
            i += 1
            continue
        # a brace at file scope opens a function's body where a ) closes its
        # head, and an initializer's or a type's otherwise
        end, depth = i, 1
        while depth and end + 1 < len(tokens):
            end += 1
            depth += (tokens[end] == '{') - (tokens[end] == '}')
        if i and tokens[i - 1] == ')':
            # the function's name stands before the ( that the ) closes
            j, inside = i - 1, 0
            while j > 0:
                inside += (tokens[j] == ')') - (tokens[j] == '(')
                if not inside:
                    break
                j -= 1
            functions[tokens[j - 1]] = tokens[i + 1:end]
        i = end + 1
    return functions, macros


def sites(body, invoked, named):
    """Where a body, as definitions() gives it, calls through the names
    reached() gives: the index of each word of invoked that a ( follows, and
    of each word of named."""
    if invoked.isdisjoint(body) and named.isdisjoint(body):
        return []
    return [at for at, word in enumerate(body) if word in named or word in invoked and body[at + 1:at + 2] == ['(']]


def reached(name, macros):
    """The names through which C calls name, given macros as definitions()
    gives them: name and the function-like macros that call it, directly or
    through other macros, as invoked, and the object-like macros that do as
    named. A function-like macro calls where a ( follows its name, and an
    object-like one wherever its name stands."""
    invoked, named = {name}, set()
    while more := {macro for macro, (_, body) in macros.items()
                   if macro not in invoked | named and sites(body, invoked, named)}:
        invoked |= {macro for macro in more if macros[macro][0]}
        named |= {macro for macro in more if not macros[macro][0]}
    return invoked, named


def hosts(name, functions, macros):
    """The functions that call name, directly or through macros. A function
    that calls one of them is not one."""
    invoked, named = reached(name, macros)
    return [function for function, body in functions.items() if sites(body, invoked, named)]


def ends_on(name, body, macros):
    """Whether a function's body, as definitions() gives it, calls name, and
    ends the function at each call: by returning just the call's value, or
    with just the call as its last statement. A body that calls name through
    a macro, or that does anything with a call's value but return it,
    doesn't."""
    found = sites(body, *reached(name, macros))
    return bool(found) and all(body[at] == name and terminal(body, at) for at in found)


def terminal(body, at):
    """Whether the call whose name stands at at in body, as definitions()
    gives it, is all of a return's value or all of the body's last
    statement."""
    depth = 0
    for close in range(at + 1, len(body)):
        depth += (body[close] == '(') - (body[close] == ')')
        if not depth:
            break
    if body[close + 1:close + 2] != [';']:
        return False
    return at and body[at - 1] == 'return' or close + 2 == len(body) and (not at or body[at - 1] in ('{', '}', ';'))


def abi_c(path, text, problems):
    """Each finding of each inline body of text, the C source at path, that
    GCC compiles, as (line, its function's name, key, why): the body's
    #pragma inline_asm line, and the key and why of a finding call_safety()
    gives, of a way out of the body where a host goes on past its call,
    keyed leaves, or of an @asm tag of abi, keyed abi. A body --abi can't
    read gives one entry, with key and why None, and each reason goes on
    problems. Each function of a #pragma asm block GCC compiles whose @asm
    tag is abi gives one entry too, keyed abi, at the block's #pragma asm
    line. A tag above a block that supplies no function gives none: GCC
    compiles no such block, since tools/split.py refuses it."""
    lines = text.split('\n')
    read = split.compiled(text)
    starts = [0, *(newline.end() for newline in re.finditer('\n', text))]
    # each @asm tag's kind, by where the line directly below it starts: a
    # body's #pragma inline_asm or a block's #pragma asm where the tag is in
    # place
    kinds = asm_above(text)
    functions, macros = definitions(side(text))
    found = []
    for body in inline.BODY.finditer(text):
        at, name = text.count('\n', 0, body.start()), body[1]
        if not read[at]:
            continue
        if kinds.get(body.start()) == 'abi':
            found.append((at + 1, name, 'abi', 'its @asm tag is abi'))
            continue
        opening = text.count('\n', 0, body.start(2)) - 1
        closing = text.count('\n', 0, body.end())
        passed, unplaced = arguments('\n'.join(lines[at + 1:opening + 1]), name)
        problems += [f"{path}:{at + 2}: {name}: its parameter {each}, which --abi can't place" for each in unplaced]
        safety = None if unplaced else call_safety(path, name, lines[opening + 1:closing], opening + 2, passed,
                                                    problems)
        if safety is None:
            found.append((at + 1, name, None, None))
            continue
        leaving, reasons = safety
        goes_on = [host for host in hosts(name, functions, macros) if not ends_on(name, functions[host], macros)]
        if leaving and goes_on:
            reasons.append((leaving[0], 'leaves', f'its {leaving[1]} at line {leaving[0]} leaves it, and '
                                                  f'{goes_on[0]} goes on past its call'))
        found += [(at + 1, name, key, why) for _, key, why in sorted(reasons)]
    for at, block in split.assembly_blocks(text).items():
        if read[at] and kinds.get(starts[at]) == 'abi':
            entries, labels = split.block_entries(block)
            found += [(at + 1, name, 'abi', 'its @asm tag is abi') for name in entries or
                      sorted(label[1:] for label in labels)]
    return found


def abi_src(text, tags):
    """Each function of text, a .src file, whose @asm tag is abi, as abi_c()
    gives its findings: the line of the label directly below the tag, the
    label as C spells it, the key abi, and why. tags are those
    assembly_faults() gives the file."""
    lines, found = text.split('\n'), []
    for tag in tags:
        if (tag.tag, tag.kind) != ('asm', 'abi'):
            continue
        below = tag.line - 1
        while below < len(lines) and REMARK.fullmatch(lines[below]):
            below += 1
        if below < len(lines) and (label := LABEL.match(lines[below])):
            found.append((below + 1, symbol(label[1]), 'abi', 'its @asm tag is abi'))
    return found


def summary(held):
    """The line --review ends on: how many @asm tags held, each file's
    Tags, gives of each kind, in TAGS.md's order."""
    kinds = Counter(tag.kind for tags in held for tag in tags if tag.tag == 'asm')
    each = ', '.join(f'{kinds[kind]:,} {kind}' for kind in KINDS['asm'][:-1])
    return (f'@asm tags of inline bodies, #pragma asm blocks and .src functions by kind: {each} and '
            f'{kinds["open"]:,} open; {sum(kinds.values()):,} in all')


def main(argv=None):
    parser = argparse.ArgumentParser(prog='tags', description=__doc__.split('\n\n')[0])
    parser.add_argument('files', metavar='FILE', nargs='+', help='a C source or header, or a .src file')
    mode = parser.add_mutually_exclusive_group()
    mode.add_argument('--review', action='store_true',
                      help='list each open @asm tag, then count the @asm tags by kind, and fail on nothing')
    mode.add_argument('--abi', action='store_true',
                      help='also fail on each function GCC may call that may not be call-safe, or whose @asm tag is '
                           'abi, that no --contract names, and read no table')
    parser.add_argument('--contract', metavar='NAME:KEY', action='append', default=[],
                        help="with --abi, a finding --abi answers by a contract: a function, as C spells it, and the "
                             "finding's key, as walk:R12")
    args = parser.parse_args(argv)
    if args.contract and not args.abi:
        parser.error('--contract needs --abi')
    if malformed := [each for each in args.contract if not re.fullmatch(r'\w+:\w+', each)]:
        parser.error(f'--contract {malformed[0]}: not NAME:KEY')
    found, tags, needs = [], {}, []
    # the tables name a file by its path from the root, as src/a.c, so ./src/a.c is that file too
    for path in dict.fromkeys(os.path.relpath(each) for each in args.files):
        text = content(path).decode('latin-1')
        faulted, tags[path] = (assembly_faults if path.endswith('.src') else faults)(path, text)
        found += faulted
        if args.abi:
            needs += [(path, *need) for need in (abi_src(text, tags[path]) if path.endswith('.src') else
                                                 abi_c(path, text, found))]
    if args.review:
        for path, held in tags.items():
            for tag in held:
                if (tag.tag, tag.kind) == ('asm', 'open'):
                    print(f'{path}:{tag.line}: {tag.text}')
        print(summary(tags.values()))
        return
    if args.abi:
        found += [f'{path}:{line}: {name}: {why}, and no --contract names {name}:{key}'
                  for path, line, name, key, why in needs if why is not None and f'{name}:{key}' not in args.contract]
        keyed = {f'{name}:{key}' for _, _, name, key, _ in needs}
        found += [f'{contract}: --contract names no finding of --abi' for contract in args.contract
                  if contract not in keyed]
    else:
        layout = Layout(Image(name) for name in images())
        found += [each for path, held in tags.items() for tag in held for each in layout.stray(path, tag)]
    if found:
        fail(*found)


if __name__ == '__main__':
    main()
