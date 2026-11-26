# Tags

Tags track three things in the source: assembly, choices made only to match SHC's output, and bugs preserved from the shipped game.

| Tag | Marks | Its text gives |
|---|---|---|
| `@asm` | Each piece of **assembly**: an inline body, a `.src` function or a `#pragma asm` block that holds more than comments | The evidence that the original was written in assembly, or for `open`, what that evidence lacks |
| `@hack` | A **hack**: a choice in the source kept only because SHC's output needs it | What is kept, and the plain form it stands for |
| `@bug` | A **bug**: a defect of the shipped game that the source keeps | What fails, and the addresses that show it |

`make lint` checks each tag's form, each `@asm` tag's place, and whether cited addresses are in range ([What lint checks](#what-lint-checks)). A reviewer checks each tag's argument ([Reviewing a tag](#reviewing-a-tag)).

## Writing a tag

Write a tag as `@<tag> <kind>: <text>`, in a comment directly above what it tags. For the exact placement of an `@asm` tag, see [Where the tag goes](#where-the-tag-goes).

```text
/* @<tag> <kind>: <text> */     in C: a plain comment of its own, not a /** */ one
; @<tag> <kind>: <text>         in a .src file: from the start of a comment line to the comment's end
```

This one from `src/system/task.c` spans two lines of one comment:

```c
/* @hack spelling: the assignment inside the test, where a statement of
 * its own would do */
if ((task = syMalloc(sizeof(struct task))) == NULL) {
```

A tag records one finding. Tags stack only where their findings are separate. A tag doesn't certify the code around it. Ordinary C doesn't take a tag: a construct the behavior needs isn't a hack, however unusual it looks.

An `@bug` tag cites evidence of the bug. An `@asm` tag cites evidence for its basis unless its kind is `open`. An `@hack` tag doesn't cite anything ([Hacks](#hacks)).

| A tag cites | As |
|---|---|
| Code or data of a source that one image places | Its address, `0x` and eight hex digits: `0x8c05ce18` |
| An instruction of a source that several images place | Its offset: `0x36e of the object's code` |
| A datum of such a source | Its offset in the data the source initializes: `0xcc of the object's data` |

In a tag, lint treats any word in the address form above as a citation. Write a 32-bit constant another way, as `H'FF000000`.

## Hacks

An `@hack` tag marks a choice in the source kept only because SHC's output needs it, where plainer C would express the same behavior. That plainer C is the tag's plain form. It keeps the behavior under any precondition the text names, or it isn't a plain form.

A hack is measured before it gets its tag. Write the source in the plain form and run `make source SOURCE=PATH`, which builds it. For each image that places the source, `make source` runs [`tools/words.py`](tools/words.py) to list the functions whose instruction words differ from the original's. The target then checks each of the source's links against its line in `SHA256SUMS` to catch differences in data too. Where no word differs and every link matches its line, the plain form replaces the construct, and doesn't take a tag.

The text says what is kept and names the plain form it stands for, with any precondition under which the two forms are equal. In "where X would do", X is that plain form: X would do the same, and would change the bytes.

```text
the assignment inside the test, where a statement of its own would do
entry stepped ahead of left, not in world_build_land's order
the reads of the pad's r and l, which nothing uses
```

If the construct could simply be deleted, describe it without naming a plain form. The text tells a reader what to leave alone, and no more. The experiment that found the hack stays out of the source: the text doesn't give a count of differing words, a register, an instruction or an address. Lint rejects a count, a register or an address in an `@hack` tag.

| Kind | Hack |
|---|---|
| `implicit` | The source leaves out a value, a return value or an evaluation order the behavior needs, and SHC's code supplies it. |
| `type` | A type, conversion, representation or type qualifier chosen for SHC's output. |
| `redundant` | A construct the behavior doesn't need, kept for SHC's output. |
| `spelling` | Code written in another form that does the same, chosen for SHC's output. |

Each kind covers shapes such as these:

- `implicit`: a return with no value, where R0 holds the result each caller takes; a local left unset, whose register holds the value its use needs; an argument a call leaves out, which the callee reads from the caller's register; two evaluations C leaves unsequenced, which SHC's code orders; a local without `volatile` that C leaves indeterminate after a `longjmp`, which SHC's code keeps in the frame.
- `type`: an integer of another width or sign; a cast, or a conversion through another type; an object declared as an array of one; a datum read through its address as another type, as `*(int *)&magnitude`; a `volatile` on a datum that code reads; a datum's second name, declared with another type ([Second names](TOOLCHAIN.md#second-names)).
- `redundant`: code that no path reaches, as a block under `if (0)`; a store that nothing reads, or one the next store replaces; a local that nothing reads, `volatile` or not; a test another test makes needless; `register` on a local or a parameter.
- `spelling`: an assignment inside a test; a value held in a local where its use could compute it, or read again where a local holds it; a variable reused for a second value; a pointer stepped across a struct's members; statements in another order; a test or a loop in another shape; a jump in place of structure; a function called through a second name.

Where a hack fits more than one kind, the first of `implicit`, `type`, `redundant` and `spelling` that fits decides. `register` is a storage class, not a qualifier, so `type` doesn't fit it, and `redundant` does. A `volatile` on a datum that code reads is `type`: its plain form is the same datum without the qualifier. A local that nothing reads is `redundant`, `volatile` or not: its plain form has no local to qualify. A local that only sizes the frame is `redundant`; a local holding a value its use could compute is `spelling`.

The definition of a datum that no code reads doesn't take a tag: the source defines the datum because the image's layout needs it, not to steer SHC's code. A comment over the definition says that nothing reads it. A statement that only places a datum is still code, and takes a tag, as `(void)"ROTSPD";` does under `if (0)` in `modules/shared/spin_solid.c`. So does a local's initializer that nothing reads.

An `if` or a `while` loop whose condition is a literal `0` or `FALSE` takes an `@hack redundant` tag of its own directly above it, alone or in a stack of `@hack` tags. In a macro, the tag stands inside the definition, above the test, as in `src/system/vector.c`.

## Bugs

An `@bug` tag marks an evidenced defect of the shipped game that the source keeps, whether the source makes it or SHC's code does. A bug stays because fixing it would change the bytes. Its first clause says what fails, in plain words, and the rest gives when it fails and the addresses that show it. This one is from `src/system/task.c`:

```c
/* @bug memory: reads task's child after its step may have freed it.
 * a killed task's step is task_destroy, which JSR @R13 at 0x8c05fee8
 * calls, and MOV.L @(12,R14),R2 at 0x8c05fef4 then reads the freed
 * task */
if (task->child != NULL) {
```

"reads task's child after its step may have freed it" says what fails; "invalid memory handling" doesn't.

| Kind | Bug |
|---|---|
| `memory` | An invalid read, write or free, or memory that no path frees: an access through null, outside its object, after its free, or through a pointer to no object the code means. |
| `uninitialized` | A value used where its path never set it: a local, an out-parameter a failed call didn't write, a heap field as its allocator left it, or R0 where a path returns no value and a caller uses it. |
| `logic` | Well-defined code that does the wrong thing: an operation on another operand, field, element or unit than the one meant, a test that always, never or wrongly holds, a state update missing, repeated or out of order. |
| `arithmetic` | A numeric result other than its expression's ordinary meaning, from machine arithmetic, an invalid operation, a conversion or SHC's code: an overflow, a shift by the width or more, a division by zero, a float converted out of range. |
| `mismatched-call` | A call whose arguments, result or calling convention disagree with its callee's independently established interface. The text says what the callee takes. |

Where an invalid access is established, the bug is `memory`, whatever computed the address, and its text names the cause. A tag classifies the failure its evidence shows, not one that failure might lead to.

An allocator's contract says whether it can fail and what it then returns. A caller's routine assumption that it succeeds doesn't take a tag. A failure path that does more keeps its tag: a function that goes on after its allocation fails, leaks what it already allocated, or leaves its state inconsistent.

The source's own tests are evidence too. A null test shows that the code expects null where the test stands. The test supports a `memory` tag only if a path carries that null to an untested read of the same pointer for the same subject.

- **A path carries the null.** One tag marks one such path: one way the null arises that the source doesn't guard against. The tag sits on the code that lets the null through, or on the first untested read, and cites the test and each function that reads through the null.
- **The test is of something else.** A scan that skips each empty player slot says nothing about the slot a call names, and a test of one player's work says nothing about another's. A test for a case the design leaves null says nothing about an allocation that fails.
- **Nothing shows null reaching the read.** The read doesn't take a tag. Where callers must pass a pointer that isn't null, the header states that precondition.

A tag outside a guard describes the matching side. Under an `@bug` tag, a guard's non-matching side may repair the bug or keep its result, and its comment says which ([Switches](PORTS.md#switches)).

## Assembly

Each inline body, each `#pragma asm` block that holds more than comments and each `.src` function carries one `@asm` tag; [Assembly](TOOLCHAIN.md#assembly) gives the three forms. One instruction, label or directive is enough: a block with just one `.ALIGN` takes a tag. If C is found that matches, it replaces the assembly.

A **basis** is evidence that the original was written in assembly. It isn't proof that no C could match. A tag's kind records its basis, or is `open` until one is established:

| Kind | Basis |
|---|---|
| `opcode` | An instruction that no SHC release is known to write from C. |
| `operand` | An instruction SHC writes, but with operands or registers it isn't known to use for that instruction. |
| `abi` | A use of the stack pointer or of FPSCR that no function SHC compiles from C is known to make. |
| `epilogue` | SHC's own lines after an inline body: an `RTS` and a `NOP` that follow a body which has already returned, or the alignment before them. |
| `name` | The SDK's library names the member that holds the function as assembly. |
| `open` | None holds yet. |

The `opcode`, `operand` and `abi` bases come from `shckit inspect`'s [rules](https://github.com/reshaped/shckit/blob/02d9f1191426d38fb9c230a2293a36f8ed3e3996/docs/inspect.md#the-rules). It's possible a rule gets disproven later, but each one has been tested extensively: inspect's [evidence](https://github.com/reshaped/shckit/blob/02d9f1191426d38fb9c230a2293a36f8ed3e3996/docs/inspect.md#evidence) page has the details.

Where more than one basis holds, the first in the table's order decides. An `open` tag doesn't say how the original was written: it marks work left, a basis to find or C to write. `make review` lists each `open` tag with its place ([What lint checks](#what-lint-checks)).

A failed attempt at C isn't a basis. Neither is padding between functions, data among code, or an object shared with assembly: each can help an investigation, but none of them shows how a function was written. A match with the image isn't a basis either: it shows the bytes, not how the original was written.

### Where the tag goes

For an inline body, the tag goes directly above its `#pragma inline_asm`, under the body's comment, as in `sdk/ninja/njPushMatrix.c`:

```c
/* @asm opcode: movca, MOVCA.L R0,@R5 at 0x8c1197c6 takes the current slot's
 * first cache line */
#pragma inline_asm(njPushMatrix)
Bool njPushMatrix(NJS_MATRIX *m)
```

For a `#pragma asm` block, the tag goes in C directly above its `#pragma asm`, and the block's own comments don't hold an `@asm` tag, as in `modules/shared/vertex_colors.c`:

```c
/* @asm open: no inspect rule reads any of its instructions */
#pragma asm
        .SECTION    P,CODE,ALIGN=32
        .EXPORT     _point_midpoint
```

A block that exports several functions takes one tag whose kind holds for all of them. A tag other than `open` gives a rule that holds for each function, and cites a witness in each. A function whose kind or rule differs from the others' goes in a block of its own, as `point_midpoint` does in `vertex_colors.c` and `UTY_Dmuls` in `sdk/sofdec/sh_dmul.c`.

For a `.src` function, the tag ends the comment above one of its labels, as in `sdk/shinobi/vmsasm.src`:

```text
; Uint16 vmsfs_getword_big(void *buffer, Sint32 offset)
; @asm opcode: swap, SWAP.B at 0x8c15ced8 swaps the field's bytes
_vmsfs_getword_big:
```

A `.src` function is the code after a run of labels in a code section, one of them exported. A label before data doesn't start a function, so a word of data in a code section doesn't take a tag. Labels in a run name the same code, so one tag covers them.

### What the text says

For every kind but `open`, the text starts with a rule or member name and a comma. It then says what the witness does and cites its address. For `opcode`, `operand` and `abi`, `RULES` in [`tools/tags.py`](tools/tags.py) lists inspect's rules by kind. The other two kinds open so:

- `epilogue`: `dead-return`, citing SHC's `RTS` and the body's own return, or `fill`, citing the padding;
- `name`: the member's name in place of a rule, then the library that holds it and the function's first address.

An `open` tag's text is one short line that says what is missing, as `no inspect rule reads any of its instructions` does.

### Checking a basis

For an `opcode`, `operand` or `abi` tag, have inspect report the rule at the cited instruction in the text asmsh assembled. For an inline body or a block, use SHC's output at `build/obj/PATH.src`; for a `.src` function, use the source file itself.

```sh
make build/shckit build/obj/sdk/ninja/njPushMatrix.obj
build/shckit inspect --entry _njPushMatrix --all build/obj/sdk/ninja/njPushMatrix.src
```

```text
build/obj/sdk/ninja/njPushMatrix.src:22: _njPushMatrix: opcode: movca
build/obj/sdk/ninja/njPushMatrix.src:24: _njPushMatrix: opcode: movca
build/obj/sdk/ninja/njPushMatrix.src:39: _njPushMatrix: opcode: movca
build/obj/sdk/ninja/njPushMatrix.src:41: _njPushMatrix: opcode: movca
```

`--entry` is the function's label, and `--end` the next function's; without `--end` inspect reads to the end of the section's block. asmsh's listing beside the object, `build/obj/PATH.lis`, gives each line of the text its offset in the object. A cited address is that offset past the start of the source's row: line 22 lies at offset 0x22, and `njPushMatrix.c`'s row starts at `0x8c1197a4`, so the tag cites `0x8c1197c6`.

An `epilogue` tag is checked in the same listing, which shows SHC's lines after the body's last. A `name` tag is checked against the library it names. The `A_SH` producer in SYSROF record 0x86 identifies the tool that wrote an object. SHC can pass C through that same assembler, so the record isn't a basis.

## Reviewing a tag

A reviewer checks what lint can't:

- an `@asm` tag's basis, at each address it cites ([Checking a basis](#checking-a-basis));
- an `@hack` tag's plain form: it keeps the behavior under the preconditions the tag names. Put it in the construct's place and use `make source SOURCE=PATH` to check that the bytes differ from the original's ([Hacks](#hacks));
- an `@bug` tag's failure: the code it cites fails as the first clause says, under the conditions the rest gives. Where its citations play different roles, as a test, a trigger and a read do, the reviewer checks what connects them, not each address alone.

An `@asm` tag whose basis doesn't hold is `open` until one does. A hack whose plain form builds the same bytes goes, and its tag with it.

## What lint checks

`make lint` runs [`tools/tags.py`](tools/tags.py) over each C file it reads ([Lint](TOOLCHAIN.md#lint)) and each `.src` file under `src/`, `sdk/` and `modules/`. It fails, naming the file and line, on each of these.

**Form:**

- a comment that opens with `@` and a word that isn't a tag's name;
- a comment that holds a tag anywhere but at its start, or holds two;
- a tag in a `/** */` comment;
- a tag not of the form `@<tag> <kind>: <text>`, or of a kind its tag doesn't take;
- an `@asm` tag other than `open` with no cited address or the wrong opening. The text must start with a rule of its kind and a comma. For `name`, lint checks for a word and a comma instead;
- an `@hack` whose text gives a count, as in `N of M words differ`, or names a register. Lint matches `R0` to `R15`, `FRn`, `DRn`, `XDn`, `FVn`, `MACH`, `MACL`, `FPUL` or `GBR` as whole words in capitals. It doesn't match a C name such as `FPSCR_PR` or a field `r15`;
- an `@hack` other than `redundant` with one of these descriptions of a `register` local or parameter: "declared register" or "register variable" anywhere in the text; "register on" or "the register on" at the start; or "a register" before "local", "parameter", "int" or "Float".

**Assembly's place:**

- an `@hack` in a `.src` file or a `#pragma asm` block;
- a `#pragma inline_asm` line not of the form `#pragma inline_asm(NAME)` or `#pragma inline_asm(NAME(size=BYTES))`;
- an inline body, or a `#pragma asm` block that holds more than comments, with no `@asm` tag directly above it;
- an `@asm` tag in C anywhere but directly above a `#pragma inline_asm` or such a block's `#pragma asm`;
- an `@asm` tag inside any `#pragma asm` block, whose comments lint reads as a `.src` file's;
- a `.src` function with no `@asm` tag, or with two; an `@asm` tag above a label that doesn't start a function; and one that doesn't end the comment directly above a function's label.

**Citations:**

- an address an `@hack` tag cites, or an offset it gives of its object's code or data;
- an `@asm` citation outside the allowed code in every image that places its source;
- an address an `@bug` tag cites outside every image's sections;
- an offset "of the object's code" past the end of its source's code, and one "of the object's data" past the end of its initialized data.

An `@asm` tag may cite this code:

| Tag on | May cite |
|---|---|
| A `.src` function | The function, under any label of its run, and each function of its source that its text names |
| An inline body | The body's function, its hosts, and each function of its source that its text names |
| A `#pragma asm` block | Anywhere in its source's code |
| Any of them, where no image names its function or a host | Anywhere in its source's code |

A function's code runs from its name in an image's `symbols.txt` to the next name or its row's end. An inline body's hosts are the functions SHC compiles that call it by name, directly or through a macro's body. A call through a macro's argument, as `UNCACHED(walk)(...)` in `sdk/shinobi/syCache.c`, doesn't make a host. A citation in range isn't evidence that it shows the defect or the basis its tag claims.

**Never-run tests:**

- a test whose condition is an integer literal `0` or `FALSE`, with no `@hack redundant` tag directly above it, alone or in a stack of `@hack` tags. A test is an `if`, or a `while` whose body opens with `{`. The `0` or `FALSE` may stand in any parentheses, and ahead of a `&&`. Lint doesn't require the tag on a guard's non-matching side, which SHC never compiles.

Lint doesn't check where any other `@hack` or `@bug` tag stands in C: either may stand in any comment of its own. It doesn't check whether a tag's claim holds ([Reviewing a tag](#reviewing-a-tag)). It requires a tag only of the patterns above: beyond a never-run `if` or `while`, it doesn't find a hack or bug that lacks a tag. It doesn't read `.inc` files, so a tag in one, as in `modules/shared/flag_task.inc`, goes unchecked.

`make review` runs `tools/tags.py --review` over the C files `make lint` checks and the `.src` files. It prints each `@asm open` tag with its place and text, then counts the `@asm` tags by kind, those above `#pragma asm` blocks among them. It doesn't check a tag's claim or fail on the tags it reports.
