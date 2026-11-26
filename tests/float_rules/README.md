# Reproduce the floating-point rules

These commands check the float pass against the original compilers. The `native.py` runs compile a fixture with SHC under four of the game's routes and compare each function's arithmetic instructions with a recorded table. The Python tests check the pass. They also run GCC's builds of `expressions.c` and `shared.c` in the repository's SH-4 model and compare each result's binary32 word.

Run the commands from the repository root on Linux. The first prepares the SHC releases and the launcher the game builds with. The second builds the GCC cross toolchain: skip it where `SH_ELF` names an existing GCC 16.2 toolchain.

```sh
make tools/wibo-i686 tools/shc-v5.1r11/shc.exe tools/shc-v5.1r13/shc.exe tools/shc-v5.0r26/shc.exe tools/shc-v5.0r32/shc.exe
make tools/sh-elf/bin/sh-elf-gcc
python3 tests/float_rules/native.py
python3 tests/float_rules/native.py --fixture tests/floats/shared.c --expected tests/floats/shared.tsv
python3 tests/float_rules/native.py --fixture tests/floats/shared_reads.c --expected tests/floats/shared_reads.tsv
PYTHONPATH=tools:tests python3 -m unittest test_float_fuse test_float_pass test_float_terminal test_float_sharing
```

## Instruction probes

`native.py` reads the release and flags of the `ninja`, `game`, `aggressive` and `modem` routes from [`config/compilers.mk`](../../config/compilers.mk): R11, R13, R13 under `-aggressive=2`, and R26. R08's one route, `network_lcp`, doesn't compile any floating-point C, so no probe runs under it, and [`test_float_pass.py`](../test_float_pass.py) checks that the pass leaves its C as it is. The runner checks the SHA-256 of the wibo launcher and of each `shc.exe`. Pass `--tools DIR` to check another installation.

The runner reads each function's `FADD`, `FSUB`, `FMUL`, `FDIV`, `FMAC` and `FNEG` from SHC's listing, checks each mnemonic against its instruction word, and compares the sequence with the table `--expected` names, [native.tsv](native.tsv) by default. A dash marks a function with none of them. `inlined` marks a static function with no separate body in that route's listing. R13 expands `multiply` into `across_inline`. `--write` replaces the table with what the run observes. Use it after an intended change to the fixture or the toolchain.

Most functions of [expressions.c](expressions.c) probe one clause of [the rule](../../PORTS.md#the-float-pass) each. Two groups set cases against each other. Under the `aggressive` route, SHC turns the division by a constant in `reciprocal`, `add_quotient` and `assign_quotient` into multiplication by the constant's reciprocal. `divide_parameter`'s division by a parameter stays an `FDIV`. `across_statement`, `across_inline` and `across_branch` put a product and its sum on either side of a statement, a call SHC may expand, and a branch. SHC doesn't fuse any of them, and computes `across_branch`'s product once, ahead of the branch. Their rows record these spellings on these releases, not a rule for what SHC does across such a boundary. [The game's own cases](#the-games-own-cases) show why.

## Shared products

The second run's fixture, [shared.c](../floats/shared.c), spells one product twice in each function, each time with its own addend. Its functions differ in what lies between the two sums, or in how they read an operand: an unknown call, a call through a pointer, a store that may alias the operand, an escaped static or automatic operand, a pointer, a member, a volatile object, or parameters alone. [shared.tsv](../floats/shared.tsv) gives each function one sequence on all four routes: `FMUL FADD FADD` where SHC computes the product once, and `FMAC FMAC` where it computes the product for each sum. The callback the calls reach, in [shared_callback.c](../floats/shared_callback.c), negates the global operand and the escaped one. So where the callback negates an operand, the second sum's sign shows whether SHC computed the product again. Each sum's rounding shows whether SHC fused its product.

The third run's fixture, [shared_reads.c](../floats/shared_reads.c), keeps each output in a local until both are computed, so no store through `out` falls between the sums. [Its table](../floats/shared_reads.tsv) shows SHC computing the product again past a store to a local whose member a pointer reads. It computes the product once past a store to another local, a changed addend, or a `+=` beside the sum.

## Tests of the pass

[test_float_fuse.py](../test_float_fuse.py) and [test_float_pass.py](../test_float_pass.py) test the pass's rule and its rewriting. [test_float_terminal.py](../test_float_terminal.py) and [test_float_sharing.py](../test_float_sharing.py) run GCC's builds of `expressions.c` and `shared.c`. One test of `test_float_pass.py` links a play object and an SDK object through the link-time compile and the overlay script. The play function keeps its `FMAC`; the SDK function keeps its `FMUL` and `FADD`. A call from one object into the other folds to the callee's constant, which only the link-time compile does. A test that compiles with GCC skips where `SH_ELF` doesn't hold a toolchain.

## Compare executed words

An instruction sequence that matches SHC's doesn't show that the results match. `test_float_terminal.py` compiles `expressions.c` through the float pass, the LTO flags, the terminal `nolto-rel` link and the overlay linker script. It runs the linked code in [sh4.py](../sh4.py), the repository's model of the SH-4, and compares each result's binary32 word. In one case the product and addend nearly cancel. (1 + 2^-23)(1 - 2^-23) is exactly 1 - 2^-46, which one `FMAC` with -1 keeps as -2^-46. An `FMUL` rounds it toward zero to 1 - 2^-24 first, so its `FADD` gives -2^-24. So on these inputs, a product GCC fuses where SHC doesn't, or leaves unfused where SHC fuses it, changes the result's word. Other cases check rounding toward zero, both signs of zero, division of 1 by 3, and a callback that moves a compound assignment's pointer and changes its old destination. A negative control compiles without the pass, and sees each difference: a subtraction contracts, the division by 3 stays a division, and the sum lands at the pointer's new destination.

The model rounds toward zero in these comparisons. The game starts in round-to-nearest mode ([Constants and the entry mode](../../PORTS.md#constants-and-the-entry-mode)). They cover finite normal operands and signed zero in single precision. They don't establish equivalence for every input or rounding mode: NaNs, infinities, subnormal operands and a console's own execution lie outside them.

## The game's own cases

Two functions of `src/system/util.c`, both on the `aggressive` route, spell one product in each arm of a branch.

`jiggle_bend` subtracts `half_sine * half_cosine * x * 2.0F` in one arm and adds it in the other. SHC computes the product once, before either arm runs: its last `FMUL`, at `0x8c03109c`, sits in the branch's delay slot. The arms hold an `FADD` and an `FSUB`, and neither fuses. The product by 2.0F is exact, so a fused arm would round its sum once, to the same word.

`jiggle_step` adds `c1 * 1.6F` in three arms of a chain of tests. SHC computes it inside the arms, not ahead of them. The first two arms share a tail, whose `FMAC` at `0x8c02f1fe` fuses it into their sum. In the third arm the `FMUL` at `0x8c02f242` computes `c1 * 1.6F`. Its result is the addend of another product's `FMAC`.

Nothing the probes record tells the two apart. So a site where a product crosses a statement, an inlined call or a branch keeps its C until a comparison of that function's result words supports a change.

## A count of the fused products

`python3 tools/fmacs.py DIR` counts the `FMAC`s of the tree's compiled C by route, by directory and by bin, and [`fmacs.py`](../../tools/fmacs.py) sets each inline body's copies apart. Put SHC's assembly output for every C source in `DIR`. Use each source's route to select the SHC release, include directories and flags, with `-code=asmcode` as in make's rule for sources with inline bodies. Make keeps that text only for the sources with inline bodies or a patch, and no goal writes the rest.

The table gives a pre-release count from SHC's assembly output for every C source in `src/`, `sdk/` and `modules/`. No check rebuilds it. The total is 6,989 `FMAC`s in 704 sources, inline bodies and `.src` files aside:

| Route | Release | `FMAC`s | Sources |
|---|---|---:|---:|
| `game` | R13 | 5,548 | 599 |
| `ninja` | R11 | 1,195 | 92 |
| `aggressive` | R13 | 195 | 2 |
| `sdk` | R13 | 28 | 2 |
| `noinline` | R13 | 8 | 1 |
| `runtime` | R13 | 7 | 6 |
| `plain_aggressive` | R13 | 5 | 1 |
| `plain` | R13 | 3 | 1 |

The count is zero for the other routes. By bin, `play` accounts for 5,625 `FMAC`s in 561 sources, `picture` for 1,316 in 138, and `none` for 48 in 5.

A function's count of `FMAC`s needn't be the count of products its C fuses. SHC copies a statement into each of a loop's unrolled steps or each arm of a branch, and merges tails.

Not every `FMAC` changes a bit. Take a normal product and a normal addend of one sign, where the product's unit in the last place is no wider than the addend's. Rounding toward zero, their sum gives the same word whether the product rounds first or not. Each step of `_in_logs`' series is such a sum, or adds a product of zero.
