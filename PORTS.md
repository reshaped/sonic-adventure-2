# Ports

The GCC build compiles the game for the Dreamcast, separately from the [matching build](TOOLCHAIN.md). [README.md](README.md#gcc-port) gives the commands and inputs. [ENGINE.md](ENGINE.md) describes the port's code and features.

## Build model

GCC compiles the same [disc-supplied initializers](TOOLCHAIN.md#data-initializers) as SHC. For stage data outside those sources, [`tools/content.py`](tools/content.py) builds relocatable data from the stage's [`data.txt`](#data-tables).

| Goal | Product |
|---|---|
| `make dc-check` | Builds with stub initializers instead of disc data and doesn't produce an executable. Writes a census of unresolved names for each image and a receipt in `build/dc/check.txt`. |
| `make dc` | Builds the objects under `build/dc/` with the disc data and writes the censuses in `build/dc/census/`. |
| `make dc-images` | Links the 68 programs at their load addresses as SH-4 ELFs in `build/dc/images/`. |
| `make dc-files` | Packs each ELF into its program file at the disc path under `build/dc/` and writes `build/dc/manifest.txt`. |
| `make dc-disc` | Replaces the program files and writes the GDI and its receipt in `build/dc/disc/`. |

The image, file and disc goals build what they need, but two checks run only under `make dc`: the [address check](#addresses) on the full data and the check of the [Chao archives](#image-layout).

GCC uses the include directories from each source's route in [`config/compilers.mk`](config/compilers.mk). [`config/dreamcast.mk`](config/dreamcast.mk) selects the `engine/dc/` sources that replace shipped objects (`DC_REPLACED`) or join an image (`DC_ADDED`).

## Switches

SHC and the port define `DREAMCAST` for SH-4 instructions and console-specific C. Only the port defines `NONMATCHING`. In shared sources, a port-specific change sits under `#ifdef NONMATCHING`, with the matching code in its `#else`. Keep the guard around the declarations or statements that differ.

An inline assembly body sits under `#ifdef DREAMCAST`. If the port supplies C instead, the body goes on the matching side.

The port checks bounds on PPP packets and EEPROM offsets in Kindergarten. The modem's AT memory diagnostic stays on the matching side alone. A port branch can also repair a [shipped bug](TAGS.md#bugs): the game compares a new score with a stage's second and third records using only the low 16 bits, so a record of 65,536 or more can lose to a lower score. The port compares whole scores.

## Toolchain

With the default `SH_ELF=tools/sh-elf`, make downloads GCC 16.2.0, binutils 2.47, GMP, MPFR and MPC. It checks their hashes against [`tools/SHA256SUMS`](tools/SHA256SUMS) and builds a little-endian, single-precision SH-4 compiler and libgcc. GCC supports link-time optimization (LTO), and binutils loads GCC's linker plugin.

`tools/sh-elf/request.txt` records the sources, hashes, host and configure flags. If the receipt is missing or differs from the current request, remove the toolchain and let make rebuild it. Set `SH_ELF` to use another installation with one GCC version and the LTO tools.

## Flags and warnings

[`config/gcc.mk`](config/gcc.mk) builds most C at `-Os` to fit the programs in the retail memory map. `GCC_HOT` selects `-O2` for loops that run every frame over vertices or particles. A source built with SHC's `-optimize=0` takes `-O0` under GCC.

GCC compiles the port's C as GNU C17 with `NDEBUG` defined. It ignores SHC's `#pragma inline` and `#pragma section`, so the modem library's constructor and destructor tables stay in `.data`. Warnings from GNU as or GCC fail the build, including GCC's warnings during the LTO compile.

## SHC's C under GCC

With `-fno-strict-aliasing`, GCC treats accesses through different types as possible aliases. `-fno-delete-null-pointer-checks` keeps GCC from assuming a pointer is non-null because it was dereferenced. `-fwrapv` makes signed addition, subtraction and multiplication wrap.

With `-m4-single-only`, a `double` takes four bytes, as it does on SHC's `-fpu=single` routes. The five `runtime64` members use `long double` for eight-byte arithmetic. `-fpack-struct=4` normally preserves SHC's member alignment. `NJS_MATRIX` needs eight-byte alignment for paired `FMOV`s, so structures that contain one use `#pragma pack(8)`.

[`machine.h`](sdk/include/machine.h) supplies SHC's intrinsics as SH-4 instructions. Assembly that reads or writes XMTRX is volatile: the FPU's back bank holds the matrix, outside GCC's view.

An integer division or remainder that can take a zero divisor needs a guard. For a zero divisor, SHC's helpers return zero and set `errno` to `EDIV`; libgcc does not set `errno`. The guard says whether the port keeps the shipped behavior or fixes the error.

SHC implements floating-point `<=` and `>=` by negating a strict comparison. With a NaN operand the SHC test succeeds and the C comparison fails. [`point_near` and `point_near_either`](src/object/set.c) use `!(sum > range)` so both compilers give the shipped result.

## Floats

### Who reads the floats

[`config/parts.txt`](config/parts.txt) groups sources by who reads their floating-point results. `play` means a game step reads them or the reader is unknown. `picture` means only the screen uses them. `none` covers other uses, including sound.

Play code must preserve the console's results down to the bits, including the operand order of an `FIPR`. A port branch that repairs a tagged bug may change them. The [turn kernels](ENGINE.md#kernels) promise the same bits only for finite vectors. Picture code can use faster arithmetic that rounds differently.

### Constants and the entry mode

Every SHC route except `runtime64` uses `-round=nearest`. GCC can therefore use its ordinary rounding for decimal constants and constant expressions. `runtime64` truncates constants toward zero; its `0.301L` has the same binary64 value either way.

The Japanese bootstrap enters the game with FPSCR set to `0x00040000`: single precision, round to nearest with ties to even, denormals flushed to zero, 32-bit transfers and bank 0. The port disc keeps that bootstrap. GCC can fold through calls and `static const` values that SHC evaluates at run time. A fold does not flush denormals.

### Fused products

An SH-4 `FMAC` adds a product to a sum with one rounding. Separate `FMUL` and `FADD` instructions round twice. That can change a squared distance the game uses for collision.

### The float pass

[`tools/floats.py`](tools/floats.py) rewrites the preprocessed C of `play` sources before GCC compiles them with `-ffp-contract=on`. For code both compilers read, it models SHC's fusion within an expression. It groups sums in SHC's order and uses a temporary to separate a product from a sum where SHC rounds them separately. [`float_fuse.py`](tools/float_fuse.py) gives the rules for R11, R13 and R26.

These rules cover one expression. They don't say what SHC does with a product across statements, an inlined call or the arms of a branch. At those sites, leave the C as it is unless a comparison of the function's result bits supports the change ([the game's own cases](tests/float_rules/README.md#the-games-own-cases)).

Under `-aggressive=2`, SHC replaces floating-point division by a constant with multiplication by its rounded reciprocal. The pass writes that multiplication for GCC. With a variable divisor, the pass keeps the division.

In non-matching game code the pass prevents implicit fusion. The non-matching side of a port guard uses `__builtin_fmaf` where it needs an `FMAC`. Sources outside `play` compile with `-ffp-contract=off`. The engine's point and line drawers build without contraction or unsafe math. The compiler probes test these rules; [their README](tests/float_rules/README.md) says what the comparisons cover.

## Inline assembly

[`tools/split.py`](tools/split.py) separates the inline bodies from the C, leaving out bodies hidden by a port guard. `build/shckit` translates them for GNU as, and the split C includes the translation through file-scope `asm`. A file-scope `#pragma asm` block can supply a complete function to both compilers. Standalone `.src` files go through shckit to GNU as too.

SHC expands an inline body in its caller. GCC calls it as a function, so its registers, stack use and return must suit GCC's calling convention. `tools/tags.py --abi` checks the assembly before splitting or translation. An exception needs a `CONTRACTS` entry in [`config/gcc.mk`](config/gcc.mk) for the function and the finding. Say how callers compiled by GCC meet the contract, or that there are none.

For example, the shipped `enter_p2` jumps to its caller's next instruction through the uncached P2 alias. A separate function can't switch its caller that way. The port calls the cache walks themselves at their P2 addresses.

## SH4ZAM and the runtime

The port uses [SH4ZAM at `ff39547`](https://github.com/gyrovorbis/sh4zam/tree/ff395477628e89bb403db16b17c4759cf4f788ac). Make stages the files in `tools/sh4zam/`, taking each from `SH4ZAM=dir` if it matches its pinned hash and downloading it otherwise. GCC and GNU as build the SH-4 backend unchanged, outside LTO, into `build/dc/sh4zam/libsh4zam.a`.

[`ninja_zam.h`](sdk/include/ninja_zam.h) supplies inline helpers for Ninja's matrix stack, transforms, vectors and trigonometry. Each Ninja function with one of these helpers has a callable definition for assembly and for code that takes its address. The helpers keep the shipped operations where the choice of instruction matters: `njCalcVector` uses three `FIPR`s for a matrix in memory. `njInvertMatrix`, `njMultiMatrix` and `njDetMatrix` keep their shipped assembly.

The matrix stack needs more than a matrix's eight-byte alignment. A push claims its slot's two cache lines with `MOVCA.L` before filling them, so [`matrix_stack`](src/main.c) starts on a 32-byte boundary and each 64-byte slot owns both of its lines.

In system RAM, [`memcpy`](sdk/runtime/memcpy.c) uses SH4ZAM for non-overlapping blocks at the original transfer widths. Other copies use `memmove` to preserve the shipped store order. [`memset`](sdk/runtime/memset.c) uses `shz_memset8` for blocks of at least 12 bytes in system RAM.

### A node's translation, turn and scale

Ninja's object and motion walks apply a node's translation, rotation and scale to the current matrix in that order. `njRotateEx` turns it one axis at a time and skips zero angles. Combining all three turns into one matrix would round the motion results differently and do extra work on nodes with fewer than three turns.

## Kernels

For the matrix calls the port replaces as a group rather than one helper at a time, see [Kernels](ENGINE.md#kernels).

## Link-time optimization

Each C object carries intermediate code beside its machine code (`-flto -ffat-lto-objects`), except those built from the data sources below. At link time GCC compiles each image's intermediate code together. The link takes the members it needs from the SH4ZAM archive and `libgcc.a`. `-r -flinker-output=nolto-rel` produces one relocatable object with machine code, ready for the census.

With the default `DC_LTO_JOBS=jobserver`, links share make's job slots: `make -j2 dc-images` runs at most two jobs at once. A count sets a separate worker limit for each link: `make -j1 DC_LTO_JOBS=2 dc-images` runs one make job at a time and lets each link use up to two workers.

Disc data needs to stay in disc order. `GCC_DATA` takes shared sources from [`config/data_sources.txt`](config/data_sources.txt) and sources unique to one image from its `data_sources.txt`. GCC compiles these outside LTO with `-fno-toplevel-reorder`, keeping unreferenced statics too. GCC places each datum at its first definition: if a pointer names a later datum, put tentative definitions through that datum in disc order. Within one source, the data in `.data` and the constants in `.rodata` each keep their original neighbors. The linker places each section as a whole.

### Keeps

In a source that defines code, GCC can drop an unused datum or fold it into an instruction. A non-matching `__attribute__((used))` keeps the storage. Use [`KEPT`](src/system/port.h) for a table read at a computed index, such as `groups` in [`main_menu.c`](modules/advertise/main_menu.c). A placement attribute goes in a guard above the declaration or in a macro such as [`HEAD_SECTION`](src/system/loader.h). A declaration compiled only by GCC can take the attribute directly.

## The census

[`tools/census.py`](tools/census.py) reads the names left undefined after LTO and any image-specific layout. An overlay's census leaves out names defined by the main executable's objects or listed in [`config/1st_read/symbols.txt`](config/1st_read/symbols.txt). `make dc-check` requires an exact match between each census and its rows in [`config/imports.txt`](config/imports.txt). For a new row, explain why the name stays undefined there.

`make dc-images` binds the names to their linked addresses. The main executable must have an empty census. Each overlay takes only the globals it imports from the linked executable. Chao areas can also name objects in the Chao parent and its two archives.

## Image layout

The main executable links at `0x8c010000`. Its initialized data and BSS must fit below the decompression workspace. Its [startup records](config/1st_read/startup.ld) relocate with the data they describe: initialized program and SDK data, two BSS ranges and the constructor and destructor tables. Initialized data stays where the loader puts it. Startup clears both BSS ranges through the end of their cache-aligned storage.

[Sofdec's linker script](config/1st_read/sofdec.ld.S) keeps the decoder's instruction groups together. Its startup table gives each group's linked bounds and byte count, excluding alignment between groups. Group 01 reserves 1,582 bytes for decoder work.

The 43 campaign stages, eight bosses, two gardens and Race Entrance build land models in RAM past the end of the overlay. The [terrain layout](config/terrain.ld) puts the loader header first and `end` after the last occupied section. The land compiler aligns its starting cursor to 32 bytes. [Kindergarten](config/chaostgkinder/bss.ld) has separate BSS ranges so the game and its network SDK can each clear their own storage.

The main executable loads `CHAO` at `0x8c500000`. CHAO stays there while it loads one of its seven areas at `0x8c9c0000`. The [area link](config/chao/areas.mk) checks types across the area and parent with LTO, then binds the area's references to the parent's linked addresses.

The loader reads `Chao_model.PRS` to `0x8c698700` and `Chao_motion.PRS` to `0x8c8ba800`, through RAM's P1 alias, before it loads CHAO. The pointers inside the archives use P0. Writable `NOLOAD` sections reserve the decoded extents at the P0 addresses without adding bytes to the program file. Model roots, the costume table and motion headers keep their offsets. `make dc` checks the archives if it finds them in `orig/`, `DISC_FILES` or the `.gdi`; otherwise it reports that it skipped the check.

Pointers in disc-derived C initializers relocate with the objects they name. Kart courses point into polygon lists to scroll road signs and change lamps; those pointers keep their offsets inside the models. Chao Race's prize table names toys in CHAO, so its pointers bind to the parent's objects.

Polygon lists need special alignment. `njCnkSetMaterial` reads colors with a 32-bit load, but GCC can align a `Sint16` array or a struct of halfwords on just two bytes. For body sources, [`polygon_lists.py`](tools/polygon_lists.py) declares four-byte alignment for each datum a model's `plist` points into. After linking, [`chunk_colors.py`](tools/chunk_colors.py) checks the color addresses in those lists and in the stage content. A failure names the datum before the image is packed.

### Data tables

STG19, STG21, STG23, STG25 and STG26 hold data that no source defines. Each has a `data.txt` that declares the data in the stage's `disc` rows. A row has one of three forms:

```text
ADDRESS NAME KIND [ARGUMENT...]
ADDRESS layout NAME
import IMAGE NAME
```

From `config/stg19/data.txt`:

```text
0x8c52d880  .draw_20_model  chunk_model  .draw_20_vertices .draw_20_polygons
0x8c52d898  .draw_20  chunk_object  .draw_20_model - -
```

- Addressed rows run in address order. Each datum starts on its kind's alignment and lies inside one `disc` row of `C`, `D` or `DATA`.
- The declarations cover each `disc` row they start in, with only alignment gaps between them. A bank of filenames or texts is the exception: it lies in the `C` row of the source that compiles it.
- A name starting with `.` is private to the table. The image's `symbols.txt` places any other name at the datum's address, and the GCC link exports it.
- `ADDRESS layout NAME` places `layouts/NAME.txt` from that address, and the layout fills its `disc` row. A layout's rows are `NAME KIND [ARGUMENT...]`, without addresses. Each placement gets its own copy of the layout's private names.
- `import IMAGE NAME` takes a public datum from `1st_read`, or from `chao` for a Chao area. The importing image's `symbols.txt` places the name at the same address.

Array kinds take counts; record kinds take their pointer targets in field order, with `-` for null. [`tools/content.py`](tools/content.py) and [`tools/kinds.py`](tools/kinds.py) give the arguments for each kind.

Support a row or layout with the original bytes and the code that reads them. `make dc-check` checks the complete tables and their import providers without the disc. With originals in `orig/`, `make test` checks the declared pointer targets and zero padding against them.

## Addresses

[`config/addresses.txt`](config/addresses.txt) classifies numbers in RAM's `0x0c`, `0x8c` and `0xac` views. An `anchor` stays fixed, like the loader's `WORK_AREA`. A `placed` address becomes a symbol, like a stage's `end`. A `data` word is a number, not a pointer.

[`tools/addresses.py`](tools/addresses.py) checks the source on both sides of guards, symbol addresses outside the images loaded together, and generated initializers. `make dc-check` uses the stub initializers. `make dc` uses the full data and includes words that `content.py` copies unchanged. Known numeric formats, such as chunk polygons, don't need a row.

## SH-4 memory ownership

Each linked image has to fit beside everything live with it. [`tools/dc_memory.py`](tools/dc_memory.py) takes workspace addresses from the game and SDK and groups the owners by loader state. `make dc-images` rejects an overlap within a state and writes the owners for each image to `build/dc/memory/IMAGE.txt`.

The same bytes can have different owners at different times. Boot releases `CONFIG_WORK` at `0x8c900000` after reading the language and sound settings. The garden can then use that RAM for the Chao motion archive.

### Drivers' buffers

Disc read buffers reserve whole 2,048-byte sectors. An asynchronous GD read needs a 32-byte-aligned destination. [`tools/dc_loads.py`](tools/dc_loads.py) checks the bounds in `config/loads.txt`. `make test` runs it against the tree. No goal runs it against the disc's files: after changing a bound, run `make build/prskit`, then `python3 tools/dc_loads.py DIRECTORY` on a complete extraction. [Checked loading](ENGINE.md#checked-loading) covers buffer reuse and the hand-off to an overlay.

## Program files and the disc

[`tools/dc_publish.py`](tools/dc_publish.py) packs the loaded segments of each ELF at their SH-4 addresses. The main executable's file ends after initialized data because startup clears its BSS. An overlay carries its BSS as zeros in its PRS stream. Each PRS must fit the loader's two-megabyte work area and decode to the packed image.

`build/dc/manifest.txt` records each program's disc path and the SHA-256 hashes of its ELF, decoded image and program file. The disc builder checks the hashes before replacing the programs.

With the original GDI and tracks, [`tools/disc.py`](tools/disc.py) keeps the boot program's original last sector. The overlays keep their first sectors wherever their new sizes fit.

With `DISC_FILES`, make first copies any game files `orig/` lacks from the complete extraction. [`tools/dc_extracted.py`](tools/dc_extracted.py) checks the extraction against the Japanese retail inventory. It keeps the extracted bootstrap and each file it doesn't replace, builds new ISO directories and writes a silent low-density audio track.

Both builds read the files back. The receipt in `build/dc/disc/` records the new programs' hashes and sectors. For a GDI it also records the hashes of the original descriptor and tracks. For an extraction it records the inventory hash, file count and bootstrap hash.
