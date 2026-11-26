# Toolchain

The build reconstructs each of the game's binaries from source and disc-derived data, verifies it against the original, and accounts for every byte. Much of the complexity comes from SHC: moving an include can move a variable, and combining two sources can move a literal pool.

[README.md](README.md#build) gives the build commands and requirements. [CONTRIBUTING.md](CONTRIBUTING.md) covers changes to the matching sources. [PORTS.md](PORTS.md) describes the GCC build.

## Build and verification

From the `.gdi` in `orig/`, or the one `GDI` names, make extracts the files `orig/` lacks. It installs the extracted files in `orig/` only after they all match their hashes in `SHA256SUMS`. `make unpack` does just the extraction and check.

### Build model

An **image** is a binary as it links, decompressed if the disc stores it as PRS. The originals are in `orig/`. Each image has a directory under `config/` for its [tables](#image-tables).

1. Each source builds to one object, `build/obj/PATH.obj`. A C source's **route** in [`config/compilers.mk`](config/compilers.mk) selects the compiler, flags and headers. C normally compiles straight to an object. Sources with [assembly](#assembly) or a [patch](#patched-sources) go through the assembler.
2. [`tools/link.py`](tools/link.py) links the object once for each image that places it. `src/system/task.c` links to `build/link/1st_read/src/system/task.bin`. The same `modules/shared/motions.c` object links at the addresses of 35 overlays: `build/link/stg03/modules/shared/motions.bin` in STG03 and a separate link in each of the other 34.
3. For a `.PRS` file, prskit decompresses the original.
4. [`tools/splice.py`](tools/splice.py) builds the image from zeros. It places the linked sources, takes the ranges marked `disc` from the original, and checks each range against the original.
5. For a `.PRS` file, prskit compresses the new image. `make` checks each rebuilt file against its original's hash in `SHA256SUMS`.

A source with disc-derived initializers builds twice. Its stub link fixes their layout without the disc; its full link supplies their values ([Stub and full links](#stub-and-full-links)).

### What writes each byte

Each stored byte has one writer:

| Writer | Bytes |
|---|---|
| Source | The linked object's code and data, except where a `disc` row takes precedence. |
| Disc | The original bytes in a `disc` row, whether code or data. |
| Storage | Bss inside the file, stored as zeros. |
| Fill | Zeros in a gap the [fill rules](#fill) account for. |

A `disc` row can cover part of a source. The object still links whole, but only the disc writes those bytes in the image. If the linked bytes match throughout the range, the splice rejects the row.

A missing writer or a second writer fails the splice. Without the disc, `make check` runs `tools/splice.py --check` to check coverage outside bss and each linked source's size against its placements.

### Whole link

`make relink` links each image's objects together with R32's lnk. In a source's own link, outside references take their addresses from the tables. In the whole link, lnk resolves references between objects using their exports and lays out their sections by size and alignment. [`tools/relink.py`](tools/relink.py) applies the same `disc` rows and compares the result with the splice, byte for byte. A difference can locate a misplaced object, a gap counted as fill, or a wrong section bound.

lnk places inputs of one section name in input order. In STG03, `modules/shared/messenger.c` precedes `modules/shared/messenger_art.c` in constants but follows it in data. There is also a `CUPGRADE` section from `modules/shared/upgrade.c` between two runs of `C`. The whole link gives later runs distinct section names and splits a run where the input order reverses. `make relink V=1` lists these substitutions.

### Shift test

`make shift` takes each whole link that matches and links it twice more, shifted by different multiples of 32 bytes. References into the main executable stay fixed, including `stage_slot` in a stage's own table. Each aligned 32-bit word must move by both shifts or keep its value under both. Words touched by a `disc` row are skipped.

A fixed word is a candidate address if its value lies between the image's base and the first 32-byte boundary at or past its end, inclusive. This includes the workspace at the end of a stage and the land buffers of four Chao areas. The test recognizes all three views of RAM: `0x0c…`, `0x8c…` and `0xac…`.

An address that should move takes a name in `symbols.txt` and an `extern` declaration in the source. [`config/kept.txt`](config/kept.txt) gives the reason for keeping any other candidate, including data that only looks like an address. The test rejects both an unexplained candidate and a reason with no candidate. `build/relink/fixed.txt` lists the words, their writers and their reasons.

### Accounting

Counts are of stored bytes per image. An object in 35 overlays counts 35 times. Bss past the end of a file counts nowhere. `make V=1` prints each image's code and data by writer; the totals must add up to the image's size.

`make progress` writes the code table in README.md; `make check` checks that it is current. `AREAS` in [`tools/progress.py`](tools/progress.py) groups the images into the table's rows.

**Code bytes** are the stored bytes of sections starting with `P`, less linker fill. The main executable's 48-byte entry is in `DSGLH` and counts as data.

- **Assembly:** code from `.src` files, the functions SHC writes for inline bodies, their expanded copies, and the instructions and data of `#pragma asm` blocks. The asmsh listing shows where each copy appears.
- **C:** the rest of the code compiled from C, including patched sources.
- **From the disc:** code under `disc` rows.

The shares in each row round down to a tenth of a percent, with one exception. The last nonzero share in table order (C, assembly, disc) takes the rest. A nonzero share below a tenth reads `<0.1%`. The headline is the **All** row's C and assembly together.

The line under the table gives the share of the **All** row's assembly bytes covered by a non-`open` [`@asm` tag](TAGS.md#assembly) on a `.src` function, an inline body or a `#pragma asm` block. It counts each placement, as the table does, and rounds down to a tenth of a percent. A `.src` function runs from its label to the next function's, so its pool and padding count with it. A pool word in a C source counts where every instruction that loads it is tagged assembly.

**Initialized data** excludes stored bss. The sources' own C and assembly supply 1,078,563 bytes, disc-derived initializers 64,392,617, direct disc copies 20,498 and linker fill 11,192. The initializer count covers the data and texts in each source's `offsets/` table, excluding bss and any bytes a `disc` row supplies. `make progress` prints these counts.

Of the direct copies, 20,441 bytes are in runs of data in five stages. The other 57 are at section edges where alignment doesn't account for the gap.

## Images and placement

### Images

| Load | Link | Binary |
|---|---|---|
| `0x8c010000` | `0x8c010000` | `1ST_READ.BIN`, stored as it loads |
| `0x8c500000` | `0x8c500000` | Stages, bosses and the 8 other programs |
| `0x8c500000` | `0x0c500000` | `CHAO.PRS`, in the stage slot |
| `0x8c9c0000` | `0x0c9c0000` | A Chao area, beside `CHAO.PRS` |

All overlays are stored as PRS. `0x0c000000` and `0x8c000000` are two views of the same RAM. The `config/` tables use linked addresses.

An overlay carries the library members it calls that the main executable lacks. For example, 52 overlays link `sdk/runtime/logf.c`.

`sdk/shinobi/strt1.src` writes two tables of section bounds in the main executable: one at the start of `C` (`0x8c0cd860`), the other at the start of `CSG` (`0x8c179e8c`). They give the layout of the game's sections, the SDK sections and the three `PSGSFD` sections after `DSG32`.

Each stage-slot overlay opens with its name and pointers to its init, exit and step functions. A stage's object list follows, with a second list in `STG05` for two players. A Chao area opens with its name and a pointer to its step function, then its object list if it has one. [`loader.h`](src/system/loader.h) declares both heads; the main executable reads the first kind and the Chao program reads the second.

Each overlay's `head.c` places its head in `DHEAD` with `#pragma section HEAD`. Code starts on a 32-byte boundary after this section, 32 bytes into the image or 64 in `STG05`. The overlay's `fill.txt` accounts for the zeros from the end of `DHEAD` to the start of code.

Constants (`C`) and initialized data (`D`) normally follow the code in that order. `OTHERPRINT` has data before constants. `CHAOSTGRACE` has `B` between code and constants. `C` and `D` start at their first placement and may include linker fill after their last.

`CHAOSTGKINDER`, `STG13`, `STG25`, `STG26` and `STG28` also have a section between `C` and `D`. The tables call it `DATA`; the file doesn't store section names. `#pragma section ATA` puts data there and `-section=c=DATA` puts constants there.

In `CHAOSTGKINDER` the modem library's code is in `PSG`. The image records the bounds of `DSG_INIT_` and `DSG_END_` at `0x0c9fb670`. Its startup clears two bss ranges from the table at `0x0c9fb690`; the second, `BSG`, is past the end of the file.

### Image tables

`tools/config.py` reads the tables in each image's `config/` directory. Ranges include `START` and exclude `END`. Write a linked address as `0x` and eight lowercase hex digits. `#` starts a comment.

#### `splits.txt`

```text
file PATH
SECTION START END
SOURCE SECTION START END
disc SECTION START END
```

For example:

```text
file                    1ST_READ.BIN
P                       0x8c010080  0x8c0cd860
src/collision/polygon.c P           0x8c0129c0  0x8c017c74
```

One `file` row gives the uppercase disc path. Section rows don't overlap. A name starting with `P` marks code and one starting with `B` marks bss.

Source and `disc` placements run in address order inside their output sections. Use `INPUT:OUTPUT` where an object's section name differs from the output name. Each input section goes to one output section. The source's first and last rows for that input give its extent. Every section holding object bytes needs a placement. No object may write outside its rows.

Source rows don't overlap. A `disc` row lies wholly inside one source row or outside them all. Bss doesn't take `disc` rows.

#### `symbols.txt`

`ADDRESS NAME` gives a symbol its C name, without the object format's leading underscore. Rows run in address order. Names are unique; several may share an address. An image with no names doesn't need a `symbols.txt`.

Overlays inherit the main executable's names unless their own tables override them. A symbol need not lie inside the image: `config/chao/symbols.txt` places `stage_slot` at `0x8c500000` for `modules/shared/field.c`. Each row names a symbol an object defines or uses, a name the image's `data.txt` declares, or a function that `tools/words.py` measures.

#### `bounds.txt`

`SECTION START END` gives the bounds the image records for a section. Rows run in address order, with ties in link order. An empty section such as `D32` has equal bounds. A nonempty section starts with its `splits.txt` row and can end before trailing fill or a final `disc` range. For example, `PSGSFD00` ends with `sdk/sofdec/mpvabdec.src`, two bytes short of the row's end.

`tools/link.py` uses these bounds for objects with `STARTOF` or `SIZEOF` relocations. It puts an empty section with no placement at its recorded start. If an object's section begins at the output section's recorded start, `tools/link.py` reserves space up to the recorded end.

#### Other tables

`fill.txt` accounts for gaps between sections. [`data.txt`](#disc-data-tables) describes data placed directly from the disc. `data_sources.txt` lists data sources for the [GCC link](PORTS.md#link-time-optimization).

### Fill

The linker rounds the end of one object's section up to the alignment of the next object's section. The end includes space the object reserves. Between two sources in the same section, the splice counts the gap as fill only if that rounding gives the second source's start and the bytes are zero.

At a section boundary, the same rule applies when the preceding source ends the first section and the next source starts the second. Stored data sections start on at least four-byte alignment. Gaps at bss edges do not count as fill.

A `disc` row usually doesn't give an object's size or alignment. One exception is a complete pool of `texture_filename` strings in `C`: `data.txt` places the pool and SHC starts each string on a four-byte boundary. The splicer checks the fill on either side. A `disc` row covering only justified zero fill is rejected. Other bytes at the same alignment still belong to the disc.

A row in `fill.txt` establishes both edges of a gap between sections:

| Row | What sets the edges |
|---|---|
| `SECTION NEXT END_WORD START_WORD` | Two words from linked sources give the end and start in `bounds.txt`. The placements meet those bounds. |
| `SECTION NEXT inputs SHA256 TARGET WORD` | The inventory accounts for every input of `SECTION`, at its size and alignment. A relocated word points to the first input of `NEXT`. |
| Same row with `TARGET` set to `C32` or `CSG32` | The startup object declares an empty intervening section aligned to 32 bytes. Its `STARTOF NEXT` relocation places the following section. |
| `DHEAD P inputs SHA256 EXPORT WORD` | The head is one complete input. Its relocated function pointer and the export's offset give the start of the first code object. |

`SHA256` pins the input list in order. The splice checks the gap's bytes against the disc.

Padding on R13's `game` route also distinguishes objects. After a literal pool, R13 pads to the next function with `NOP`s inside an object. After the final pool it rounds up the object's size instead, so the space links as zeros. A section ending with an instruction ends there: `src/draw/fade.c` ends at `0x8c056f50` and linker fill runs to `0x8c056f60`. Where the padding doesn't decide a seam between objects, record the other evidence beside the row.

## Compiling the sources

### Releases

Five Hitachi releases build the binaries under wibo. Katana release numbers refer to the Dreamcast SDK, not the compiler.

| Release | SHC version | Builds |
|---|---|---|
| R08 | 5.1 Release 8 | Kindergarten's PPP Link Control Protocol member |
| R11 | 5.1 Release 11 | Ninja's C members, except Kamui's |
| R13 | 5.1 Release 13 | The game, the C runtime and the remaining SDK sources |
| R26 | 5.0 Release 26 | The modem library's C members |
| R32 | 5.0 Release 32 | All assembly and links, with asmsh, lnk and cnvs |

R32's cnvs converts the link to S-records for `tools/link.py`.

### Flags

[`config/compilers.mk`](config/compilers.mk) assigns sources to routes. A directory's route applies to sources under it unless a more specific route matches. A C source without a route fails the build. The common flags are `-cpu=sh4 -endian=little -fpu=single -round=nearest -define=DREAMCAST`; the table gives additions and exceptions.

| Route | Sources | Changes to the common flags |
|---|---|---|
| `game` | `src/`, `modules/` | `-speed -macsave=0 -extra=a=1800` |
| `plain` | Unoptimized game sources, including `src/draw/debug_draw.c` | `-optimize=0 -size -macsave=0 -extra=a=1800` |
| `plain_aggressive` | `modules/event/model_burst.c` | `plain` plus `-aggressive=2` |
| `aggressive` | `src/system/util.c`, `src/player/sonic.c` | `game` plus `-aggressive=2` |
| `noinline` | `src/sound/sound.c` | `game` plus `-noinline` |
| `vertex_colors` | `modules/shared/vertex_colors.c` | `game` with `-cpu=sh3e` instead of `-cpu=sh4` |
| `sdk` | `sdk/` | `-size -macsave=0 -section=p=PSG,c=CSG,d=DSG,b=BSG` |
| `mpvspeed` | Sofdec video decoder members, including `sdk/sofdec/mpv_err.c` | `-speed -section=p=PSG,c=CSG,d=DSG,b=BSG` |
| `mpvlib` | `sdk/sofdec/mpv_lib.c`, `sdk/sofdec/mpvadec.c` | `mpvspeed` plus `-noinline -noloop -macsave=0` |
| `rnv2` | Movie renderer members, including `sdk/middleware/rnv2mvf.c` | `-optimize=0 -macsave=0 -section=p=PSG,c=CSG,d=DSG,b=BSG` |
| `ninja` | `sdk/ninja/` | Same as `sdk` |
| `kamui` | `sdk/ninja/*_kamui_lib.c` | `-speed -noloop -macsave=0 -extra=a=800 -section=p=PSG,c=CSG,d=DSG,b=BSG` |
| `modem` | `sdk/modem/` | `sdk` plus `-extra=a=400` |
| `backup` | `sdk/backup/` | `-optimize=0 -size -macsave=0` |
| `network` | `sdk/network/` | `-size -macsave=0` |
| `network_lcp` | `sdk/network/ppp_lcp.c` | Same as `network` |
| `network_data` | Four Network SDK members, including `sdk/network/ppp_fcs.c` | `network` plus `-section=c=DATA` |
| `runtime` | `sdk/runtime/` | None |
| `runtime64` | Double-precision runtime members, including `sdk/runtime/_xti.c` | Omit `-fpu=single -round=nearest` |

`vertex_colors` uses the SH-3E scheduler for the D8 color setter. Its 22 bytes use instructions shared with SH-4. R32's asmsh assembles the three adjacent functions for SH-4.

`-fpu=single` gives `double` the behavior of `float`. `-round=nearest` rounds float constants to the nearest value rather than toward zero. `-aggressive=2` lets SHC multiply by a reciprocal instead of dividing by a constant.

`-noinline` leaves only the inlining requested by `#pragma inline`. `-noloop` keeps loops rolled and `-macsave=0` omits the save and restore of `MACL` and `MACH`. Bit `0x800` in `-extra` aligns code to 32 bytes. `-extra=a=400` enables the SH-4 workaround that inserts a `NOP` after a copy into R0 outside a delay slot.

asmsh takes `-cpu=sh4 -endian=little -sjis -private` for every assembly. `-private` admits `FSCA`.

### Running SHC

SHC, asmsh, lnk and cnvs are 32-bit Windows programs. `WIBO` selects wibo (1.2.0). `R08`, `R11`, `R13` and `R26` name the directories with `shc.exe` and its supporting files. `R32` supplies `asmsh.exe`, `lnk.exe` and `cnvs.exe`, in lowercase.

Keep hyphens out of source paths and use one dot in a source filename. A compile fails on an error or an unlisted warning; a link fails on any message.

### C for SHC

SHC reads C89. For an uninitialized variable, the first declaration fixes its order in bss. An `extern` in a header counts. Initialized variables follow definition order. Moving an include can move a variable.

SDK sources follow their members' export lists: external linkage for an exported name, `static` otherwise. SHC can fold a `static const` into its uses where it would load an external constant. Shared helpers defined `static` under `#pragma inline` expand at their calls without a separate function.

SHC puts strings used by code before strings used only in initializers. In `modules/shared/grind_rail.c` an unreachable block names the SET editor captions to keep them among the code's strings.

Folded float expressions use the route's rounding too. In `modules/shared/dirt.c`, `1.0F / LIFE_STEPS` gives `0x3c888889`.

### Second names

A second name has its own declaration and a `symbols.txt` row at the same address as the first. Put it after the first name: `tools/words.py` uses the first row to name a function. Second names serve three needs:

- **A function address held in a register.** `modules/shared/enemy_chaos.c` needs the addresses of four of its own functions in registers. Each function has external linkage and a second name that the source declares but doesn't define. The linker accepts the external reference if it points to an exported code label in the same object. For a new name of this kind, show that the caller matches with the callee outside the source and differs with it defined.
- **A call with an extra argument.** `kyoko_draw` passes a `Float` after the bits that `draw_control_set` takes. `modules/shared/enemy_kyoko.c` calls it as `draw_control_set_float`; `config/1st_read/symbols.txt` binds both names to the same function.
- **Data accessed through another type.** `modules/shared/parked_car.c` copies `modifier_volume_box_model` through the array declaration `modifier_volume_box_model_array` in `modules/shared/modifier_volume.h`.

### Variants

A shared source compiles once and links in several images. A **variant** compiles the same text into a second object. `STG23`, `STG25`, `STG26` and `STG28` contain both `modules/shared/wood_box.c` and `modules/shared/wood_box_e.c`. The second copy works on models with texture names ending in `e`.

The variant renames the exports and sets any parameters before it includes the source. Private names stay the same. Each object has its own placements and exported names; the variant's static functions don't take `symbols.txt` rows. A variant that imports data has its own offsets and bindings, as `modules/stg18/lagoon_lens_flare.c` does for Wild Canyon's lens flare in Dry Lagoon.

### Assembly

R32's asmsh assembles three forms of source:

- A **`.src` file** is standalone assembly in asmsh syntax.
- An **inline body** is a C function under `#pragma inline_asm`. SHC expands it at each call. For a non-`static` body it also emits a separate function, adding alignment to 4, an `RTS` and a `NOP` after the body. A `static` body exists only at its expansions.
- A **`#pragma asm` block** places instructions among C functions or inside one.

SHC normally writes objects with `-code=machinecode`. For inline assembly it writes `build/obj/PATH.src` with `-code=asmcode` and asmsh assembles the result.

[Accounting](#accounting) uses the listing, `build/obj/PATH.lis`, to locate the emitted functions, expansions and pragma instructions in the object. An alignment or reservation directive doesn't emit an instruction word. [TAGS.md](TAGS.md#assembly) describes the `@asm` tags.

### Patched sources

`PATCHED` in `config/compilers.mk` names the C sources that need a patch to SHC's assembly text. `build/shckit` applies the source's `.patches` to `build/obj/PATH.src`; asmsh assembles `build/obj/PATH_patched.src`.

- **`sdk/ninja/njControl.c`** takes [align-pool](https://github.com/reshaped/shckit/blob/02d9f1191426d38fb9c230a2293a36f8ed3e3996/docs/patches.md#align-pool). R11 emits the member's instructions but starts the first long of its pool at `0x8c1188fc`. The image has a `NOP` at `0x8c1188fa`, four zero bytes, then the long at `0x8c118900`. The patch changes the pool layout in the text; it doesn't read any bytes from the image.

### Warned sources

`WARNED` in `config/compilers.mk` names sources with an accepted SHC warning. The source's `.warnings` lists the numbers allowed. The build fails if SHC reports an unlisted warning or doesn't report a listed one.

`sdk/ninja/njQuaternionEx.c` draws warning 1016, “Argument mismatch”, for passing `cur`, an `NJS_MATRIX` of sixteen floats, to `st_ext` and `ld_ext`. The intrinsics take four rows. Passing four rows changes 108 of the function's 223 words; casting the rows at both calls or passing `void *` changes 211, and using a union changes 43. Passing the bare matrix produces the matching code.

### Kindergarten's SDK members

`CHAOSTGKINDER` contains members of Katana R10's `ppp.lib` and R11b's `modem.lib`. Each source below builds as one C object with its data and has no `disc` row inside its placements:

| Library | Sources | Route |
|---|---|---|
| PPP | `sdk/network/ppp_chap.c`, `ppp_ipcp.c`, `ppp_pap.c` | `network` |
| PPP | `sdk/network/ppp_lcp.c` | `network_lcp` |
| Modem | `sdk/modem/tci_modm.c`, `atcom.c`, `ee_drver.c` | `modem` |

A member's literal pools can serve more than one function. In `tci_modm.c`, `TCI_timer` loads the address of `TCI_timer_10ms` from the pool after `TCI_tx_response`. The four PPP members place their protocol records and names in `DATA`.

#### The modem library

`sdk/modem/` builds the C++ modem library from C. The suffix of a library name starts with `__`. A class name, if present, comes next, length first. `F` introduces the parameter types: `v` for none, `c` for `char`, `s` for `short`, `i` for `int`, `l` for `long`, `U` for unsigned and `P` for pointer. The return type comes from the code and its callers, not the name ([Names](CONTRIBUTING.md#names)).

Five members supply constructor entries in `DSG_INIT_`; two supply destructor entries in `DSG_END_`. The C sources place function pointers there with `#pragma section`. Kindergarten calls the constructors after it clears bss and the destructors as it exits, skipping null and -1 entries. The `dteapi` entries construct and destroy its `CmiStatic`.

## Imported data

### What a source declares

Values that describe the game's rules belong beside the code: a time limit, a flag, a list of stages. `src/hud/hud.c` puts each time limit beside its stage. Asset values usually come from the disc. Even a kart course made of indices is an asset: `modules/cart/layout.c` declares the five courses and imports their pieces.

A source defines the data in its placements, including assets and records containing pointers. Give an imported datum its type, linkage and full extent. Only its initializer comes from the disc:

```c
static Sint16 gold_beetle_node_5_polygons[411] = {
#include "shared/gold_beetle/gold_beetle_node_5_polygons.inc"
};
```

A chunk list is normally a flat `Sint16` or `Sint32` array. Use a struct where the code names its chunks. The generator treats such a struct as a record.

Five stages also have data placed directly by `disc` rows: Versus land in STG19, flag supports in STG21 and STG23, and paintings and filenames in STG25 and STG26. An `extern` lets code use a name at one of those addresses without defining the data.

### Data initializers

The generated initializer is a **body**. For datum `NAME` in `DIR/PATH.c`, the include is `PATH/NAME.inc`. For example, `sdk/modem/ee_drver.c` includes `modem/ee_drver/eeprom_profile_registers.inc` in a `static const Uint8` array. The source's [offsets table](#the-offsets-table) locates the array inside its object's `CSG` section.

#### Stub and full links

The stub build compiles with includes from `build/stub/` and doesn't require the disc. Numbers are zero except for selectors and stop records. Pointers name their targets; named strings have placeholders of the right width. `make check` uses this build.

The full build compiles the same source on the same route, with values from the original in `build/gen/`. For every image that places the source, `make` compares the two links. Outside the declared data and texts they must agree byte for byte, including instructions and padding. Inside, each stub value must have its prescribed form, with pointer words unchanged. `python3 tools/body_links.py IMAGE SOURCE` checks one pair.

#### Bodies, pointers and texts

[`tools/body_data.py`](tools/body_data.py) turns pointer bindings into C expressions. A function pointer names its function. A pointer to data names the object, member or element at its destination, preferring the outermost expression that fits the field without a cast. For example, a `Sint32 *` can take `egg_key_vertices.head` and an `NJS_CNK_MODEL *` can take `&flag_blue_frames[0].mesh`.

The full build checks each pointer word against the target's linked address plus its offset. A null field must be zero. An integer field holding a known address in a 32-bit word is rejected: the address needs a relocation.

A **named text** is one string literal. The source includes `text.inc` and uses its macro, as `modules/advertise/main_menu.c` passes `MAIN_MENU_ARCHIVE` to `texture_load_pvm`. The offsets table places the string and the bindings give its width. Stub placeholders of the same width differ so SHC won't pool them. In a body, a pointer to a string names the string's macro.

**Numbered texts** (`TEXT_1`, `TEXT_2`, …) supply strings inside a file-scope datum. A `char` array takes them in order; a struct of `char` arrays takes one per member. In a string bank, each text before the last includes the null and padding up to the next string. The last string must end in a null with only zeros after it. The stub defines numbered texts as `""`.

For a binary32 field, the generator writes the shortest decimal in significant digits that rounds to the stored value without relying on a tie. It preserves the sign of zero. A subnormal, infinity or NaN needs a placement that preserves its bits rather than a float literal.

A chunk model's `plist` or `vlist` binding identifies a flat array as a polygon or vertex list. The full build walks its chunks through `NJD_CE`, which must be the last element. Vertex counts and widths must fit the chunks.

#### Runs of data

A data-only source can hold a **run**: consecutive data placed in the same order by the same images. A run ends at code, a change of images, a fill gap or a change of pointer bank. A string run in `C` can join the run that names it if the same images place both and neither refers to another such run. One run can contain more than one original object.

Each datum takes its declared type, or the type its kind gives in [`tools/kinds.py`](tools/kinds.py). Data used from another source have external linkage. A run beside its bank of names can go in the bank's source if that source doesn't define a function and serves every image that places the run.

Interior bytes with no known use form an array of words, halfwords or bytes. The array uses the widest element type whose alignment skips only zero bytes at the start of the range. Those zeros count as linker [fill](#fill). Identical runs beside the same source in several images can share a source in `modules/shared/`, such as [`dat_effect_object_0.c`](modules/shared/dat_effect_object_0.c).

### Offsets tables

Each body source has a measured table under `offsets/`. Its bindings file supplies information the assembler or compiler listing doesn't give, such as a pointer target or a string width.

#### The offsets table

`offsets/PATH.txt` places items in the input sections of `PATH.c`:

```text
datum NAME SECTION OFFSET SIZE
text MACRO SECTION OFFSET SIZE
entry NAME SECTION OFFSET SIZE
```

`OFFSET` is the byte offset from the start of the source's section, written as `0x` followed by eight hex digits. `SIZE` is decimal bytes. Items don't overlap. From `offsets/sdk/modem/ee_drver.txt`:

```text
datum  eeprom_profile_registers  CSG  0x00000000  40
datum  eeprom_initial_header      CSG  0x00000028  12
```

A `datum` can be a variable or the constant template SHC uses to initialize an automatic array. A `text` size includes the terminating null. An `entry` places the first two bytes of a static function named by a pointer binding. The table also places private uninitialized data used as pointer targets. Other targets need an address in the image tables, including static data with an initializer written directly in C.

`make offsets` measures the tables from stub listings, writes missing or changed tables and removes ones that no body source owns. Don't edit them by hand. The stub reads the bindings, not the offsets. `make check` measures the tables again under `build/offsets/` and compares them with the tracked files.

#### The bindings file

`offsets/PATH.bindings` uses these rows:

| Row | Meaning |
|---|---|
| `pointer DATUM OFFSET TARGET` | The word at `OFFSET` in `DATUM` relocates to `TARGET`, a name in scope or a text macro. Append `+0x` and a byte offset for a pointer past its start. A pointer field without a row is null. |
| `text MACRO SIZE` | The named string occupies `SIZE` bytes including its terminating null. |
| `selector DATUM.FIELD CONSTANT` | Keep this integer field at `CONSTANT` in the stub. The full build checks it against the original. |
| `stop TABLE [FIELD] CONSTANT` | A negative constant ends an array of signed integers. For an array of records, `FIELD` names the field that holds the constant in the final record. The stub puts the stop first, making the table empty. |
| `include NAME` | Read `layouts/NAME.bindings` here. Shared files can include other shared files. |

For example, `offsets/modules/shared/dirt.bindings` gives a texture name its width and binds a pointer to it:

```text
text     DIRT_MATERIAL_01  12
pointer  dirt_texname      0x00000000  DIRT_MATERIAL_01
```

Write a `pointer` row's target as a name and an optional byte offset, not as a C expression such as `&NAME` or `NAME[0].member`. [`tools/relocate.py`](tools/relocate.py) rewrites rows with targets in those C forms.

### Disc data tables

For the stages with directly placed assets, [`tools/names.py`](tools/names.py) reads names from `data.txt`. The splicer uses the table's filename pools to account for [fill](#fill); it copies the data itself from the original. The GCC build reads the whole table: [Data tables](PORTS.md#data-tables) gives its rows and rules.

## Maintenance

### Incremental builds

An object rebuilds when its source, an included header or its compile route changes. A link reruns when its object, placements or symbol addresses change. A splice reruns when an input link or the image's `splits.txt` changes.

### Hashes

`tools/SHA256SUMS` pins outside inputs: wibo, the SHC archives, the lint wheels, the GCC toolchain sources, SH4ZAM and the vendored tools. When you change a pin, name the input's release or commit. `python3 tools/vendor.py TOOL COMMIT` refreshes a vendored tool; `make test` checks the copied files.

`SHA256SUMS` at the root records originals and linked sources, one `HASH  PATH` per line in `LC_ALL=C sort -k 2` order. A rebuilt file such as `build/SONIC2/STG13.PRS` is checked against its original's line.

| Line | What it covers | Checked by |
|---|---|---|
| Original in `orig/` | The file as stored on the disc | Each goal that reads it; `make` also checks the rebuilt file |
| `build/link/`, source without bodies | The complete linked source, including bytes masked by `disc` rows in the image | `make check`, `make source` |
| `build/link/`, body source | The full link with disc-derived values | `make`, `make source` |
| `build/stub/link/` | The stub link, at the addresses of every image that places the source | `make check`, `make` |

[`tools/sums.py`](tools/sums.py) checks the manifest's contents and order; a missing or stale entry fails the check. [Adding a source](CONTRIBUTING.md#adding-a-source) covers new lines and [What passing means](CONTRIBUTING.md#what-passing-means) covers changes to a recorded hash.

### Lint

The pinned clang-format and clang-tidy wheels are for x86-64 Linux and macOS. `make lint` checks formatting in `src/`, `modules/`, `sdk/`, `engine/` and `tests/kernels/`, except for the upstream `tests/kernels/sh4_fpu_approx.h`. `make format` formats those files in place without moving includes. Keep inline assembly and hand-aligned tables between `/* clang-format off */` and `/* clang-format on */`.

clang-tidy reads the C sources in `src/`, `modules/` and `sdk/` with stub initializers and neither console switch defined. shckit checks and lints the `.src` files. [`tools/decls.py`](tools/decls.py) checks declarations across sources and headers. The [tag checks](TAGS.md#what-lint-checks) report a file and line; they don't scan `.inc` files.

### Tools

| Task | Command |
|---|---|
| Splice every image and show its bytes by writer | `make V=1 -W tools/splice.py` |
| Count differing words by function | `make source SOURCE=PATH` |
| Compare whole links with splices and list section substitutions | `make relink V=1` |
| Measure the body offsets again | `make offsets` |

The host C compiler builds `build/gdi` to extract the disc's files, `build/prskit` to decompress and compress PRS, and `build/shckit` to check assembly and patch SHC's text.
