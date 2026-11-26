# Contributing

Contributions are welcome! There's plenty of logic in this repo that we need to understand better.

A better name, a comment that explains the game's behavior or unusual code, or a clearer C spelling for a function that already matches is worth sharing. So is a typed declaration for data a `disc` range imports from the disc, matching C in place of open assembly, stronger evidence for a [tag](TAGS.md), or an improvement to either build or its tools.

Not sure about something? Open an issue, or a draft pull request with whatever you've got. It doesn't have to be finished. [README.md](README.md#build) gets you to a first build.

## Sending a change

Send pull requests against `main`, one change per commit, with a subject that says what changes. In the pull request, say what changed, which binaries it affects, which [checks](#checks) you ran, and what's still uncertain. A sentence or two is plenty for a small fix.

By sending a pull request, you dedicate whatever rights you hold in your change to the public domain under [CC0 1.0](https://creativecommons.org/publicdomain/zero/1.0/). Never commit the disc's files, or anything make extracts or builds from them.

No disc? You can still read the code, run the disc-free checks, and open a draft that says what's unverified. A change to matching sources, tables or tools gets the full build with the disc before it merges: `make check` can't stand in for it.

If you change a recorded hash, what counts as code, the splice rules or a table the checks rely on, call it out separately. Those changes affect [what passing means](#what-passing-means).

## Checks

Run `make test`, `make check` and `make lint` for every change. All three work without the disc. On macOS, use CI's `make check` result. [CI](.github/workflows/check.yml) has no disc: it runs those three, then `make dc-check` and `make test` with the SH-4 toolchain. To find open assembly, run `make review`: it reports the `@asm open` tags in the matching C and assembly. It's a report, not a gate.

For the port changes below, also run `make dc-check` and `make test SH_ELF=tools/sh-elf`. The checks in every applicable row must pass before the change merges. If both builds compile the code you changed, run the checks in the first two rows. A play test doesn't replace the build checks for the code it exercises.

| What changed | With the disc |
|---|---|
| A matching source, an image table, a compiler route or a build tool | `make` |
| Non-matching code, Ninja's math, `engine/`, a `data.txt` or a `layouts/` file | `make dc` and `make dc-images` |
| How the port loads or packs its programs | `make dc-files` and `make dc-disc` |
| How the port plays | Test the change in the game |

For a play test, give the commit, the program, the path you took through the game, what you ran it on and what you saw. Test save changes with a save you can lose. Test a claim about the hardware on a console and name it.

### Every gate

`make accept` runs the checks for both builds. Run it on its own from a clean checkout, with the disc. Leave `HEAD` and the tree unchanged until it finishes. The checks take only three settings from the `make accept` command line: `GDI`, `DISC_FILES` and `SH_ELF`. Put any other setting, such as `SH4ZAM` or `DC_LTO_JOBS`, in the environment.

`build/accept.txt` records the commit, each command and its result. A passing record ends with the hashes of `build/dc/manifest.txt` and the files in `build/dc/disc/`, then `accepted`. To check the files against the record:

```sh
sed -n 's/^sum *//p' build/accept.txt | sha256sum -c -
```

## One source, end to end

`modules/shared/spin_solid.c` holds the spinning crate that doesn't break. It compiles once. Meteor Herd (`STG32`) and Mad Space (`STG44`) place the object at different addresses. To see where it lands in Meteor Herd:

```sh
grep 'spin_solid\.c' config/stg32/splits.txt
```

```text
modules/shared/spin_solid.c         P           0x8c505b60  0x8c505ce0
modules/shared/spin_solid.c         C           0x8c54a3c0  0x8c54a3c7
modules/shared/spin_solid.c         D           0x8c596ec0  0x8c596f0c
```

`P` holds the five functions, `C` the 7 bytes of `"ROTSPD"` and `D` the 76 bytes of data. Mad Space places the same three sections, with code at `0x8c503b60` ([Image tables](TOOLCHAIN.md#image-tables)).

Check both links against `SHA256SUMS`:

```sh
make source SOURCE=modules/shared/spin_solid.c
```

With the disc, `make source` also compares each function's instruction words with the original:

```text
modules/shared/spin_solid.c in stg32: no word differs
modules/shared/spin_solid.c in stg44: no word differs
  sums   2 of 2 links match SHA256SUMS
```

Without the disc, run the same command to check the hashes alone. A source with disc-supplied initializers needs the disc for this step.

With the disc, rebuild both stages and compare them with the originals:

```sh
make build/SONIC2/STG32.PRS build/SONIC2/STG44.PRS
cmp build/SONIC2/STG32.PRS orig/SONIC2/STG32.PRS
cmp build/SONIC2/STG44.PRS orig/SONIC2/STG44.PRS
```

`cmp` prints nothing when the files match. For a mismatch, start with the source and its `splits.txt` rows.

## Adding a source

A new source takes its bytes from a `disc` row or from another source's rows ([What writes each byte](TOOLCHAIN.md#what-writes-each-byte)).

1. **Find every image that holds the object.** For an existing source, substitute its path for `WRITER` in `grep -l '^WRITER ' config/*/splits.txt`. For a `disc` row, search the other originals for a run of bytes containing no address. `make` puts the decompressed `.PRS` originals in `build/orig/`. Check for all the object's sections in each candidate image.

2. **Write the whole object.** Include every section and keep the definitions in order ([C for SHC](TOOLCHAIN.md#c-for-shc)). For disc-supplied data, write the declaration and include its initializer ([What a source declares](TOOLCHAIN.md#what-a-source-declares)). If the GCC build needs to retain the storage, see [Keeps](PORTS.md#keeps).

3. **Choose the compiler route.** Use the directory's release and flags unless the bytes need different ones. For an exception, add the source to the route in [`config/compilers.mk`](config/compilers.mk) and say what differs without it ([Flags](TOOLCHAIN.md#flags)).

4. **Update each image's tables.** In `splits.txt`, add a row for each section and shorten or remove the rows it replaces. In `symbols.txt`, add missing names the object uses and names it defines for other objects. Update `data.txt` where it places data the new source writes ([Data tables](PORTS.md#data-tables)). For disc-supplied initializers, run `make offsets` and commit its changes ([Offsets tables](TOOLCHAIN.md#offsets-tables)). Remove a source's `.bindings` file if the change makes it unnecessary.

5. **Build the links.** Run `make source SOURCE=PATH` with the disc ([worked example](#one-source-end-to-end)). Expect a missing `SHA256SUMS` line for each new link.

6. **Record the hashes.** For each new link, run:

   ```sh
   python3 tools/sums.py --add build/link/IMAGE/PATH.bin
   ```

   `--add` takes only new entries under `build/link/`. For a source with disc-supplied initializers, run `make check` to build its stub links. Add each missing stub line by hand:

   ```sh
   sha256sum build/stub/link/IMAGE/PATH.bin >> SHA256SUMS
   LC_ALL=C sort -k 2 -o SHA256SUMS SHA256SUMS
   ```

   If you shortened another source, update its changed hashes by hand and explain why. Remove entries for links that no image places; `make check` names them.

7. **Run `make` with the disc.** The new hashes stand only once every image matches. If a pointer in a disc-supplied initializer differs, `make` gives the row to add to the source's `.bindings` file. Run the other [checks](#checks) for the change.

## Writing for this tree

Use `/* */` for comments and put a block's declarations before its statements. When the builds need different C, keep both spellings under a [guard](PORTS.md#switches). Use `#ifdef NONMATCHING` with an optional `#else`; for matching-only code, leave the `NONMATCHING` side empty. Inside a non-matching side with code, say why it differs. A lone `used` attribute or a macro defining it doesn't need an explanation.

Put a shared declaration in its owner's header. A file in `src/` can't include a header from `modules/`. Outside `engine/`, include an engine header by its path from the root, on a non-matching side. An SDK export with no public SDK declaration and no outside user stays in its source, with a comment. A variant's header declares its renamed exports.

A comment says what the code can't. Give a function's contract in its header: what it requires of its caller, what it returns, keeps and changes, and what it leaves on failure. Say which pointers must be non-null. In the code, explain a reason, a unit or an exception rather than narrating the statements. Give meaningful numbers names in the code. Don't assign intent the bytes don't show.

Write comments in lowercase and in the present tense, with no closing period. Keep the case of names, instructions and registers:

```c
/* task_levels has a head for each level. task_create links root tasks into
 * levels 0 to 6, since its level 7 means TASK_UNLINKED */
#define TASK_LEVELS 8
```

A data run's opening comment says what it holds and what lies on either side. Where other data touch the run, put a comment beside its `splits.txt` row explaining why the run ends there.

[Tag](TAGS.md) constructs kept for SHC's output, bugs the game shipped with and each piece of assembly. [Lint](TOOLCHAIN.md#lint) gives the formatting and source checks.

### Names

Keep `fun_<address>` and `dat_<address>` until their uses establish a role. In a shared source, use `fun_<file>_<offset>` with the function's offset in the object's code. Use `dat_<file>_<n>` for data, numbered in declaration order. For a separate run of unknown data beside that file, take the first unused number. `make check` checks address and offset placeholders against the image tables.

An SDK source holds one library member and takes its name. Drop a trailing underscore: `njCnkCompile_` from `ninja.lib` becomes `sdk/ninja/njCnkCompile.c`. Keep leading underscores: `__divls` in the C runtime is `sdk/runtime/__divls.src`. If no available library holds the code, use the module named in its build stamp, as in `sdk/network/ntinf.c`.

Use the unmangled name for a free function in the [modem library](TOOLCHAIN.md#the-modem-library). Name a method `CLASS_NAME` and give it a `struct CLASS *` first parameter. A constructor is `CLASS_ct` and returns the object. A destructor is `CLASS_dt`. Use the SDK types for the remaining parameters.

### Editors

`make stub-includes` writes the stub includes and `build/compile_commands.json` for clangd. The database takes precedence over `compile_flags.txt` and selects a variant's own stub bodies. The goal needs neither the disc nor SHC. Run the goal again after adding a source or test, or changing an initializer body's declaration.

## What passing means

Never change a hash to accept a mismatch. Change an original file's hash only for corrected disc input, and say where it came from. For a changed link or stub link, explain why the bytes changed and show `make` passing with the disc.

If you change a compiler route, an image table or what counts as code, compare the output of `make V=1 -W tools/splice.py` before and after. Account for each changed writer in each image. For changes to code accounting or README.md's rows, include both sets of `make progress` totals ([Accounting](TOOLCHAIN.md#accounting)).

Give the reason for a change to the port's [import table](PORTS.md#the-census), [assembly contracts](PORTS.md#inline-assembly), [parts table](PORTS.md#floats) or [data tables](PORTS.md#data-tables), including any exception.

What passing doesn't show:

- `make check` verifies linked sources, not whole images. Only a build with the disc checks the disc's initializer values.
- `make test` skips a test whose toolchain or disc file is missing. With `SH_ELF` set, a missing toolchain or SH4ZAM is an error.
- `make dc-check` builds each image's [census](PORTS.md#the-census). A name can still be missing at the final link.
- `make dc-disc` reads the programs back from the disc it builds. It doesn't boot them.
- Lint can miss an untagged hack or bug. A pass doesn't show that a tag's argument holds ([Reviewing a tag](TAGS.md#reviewing-a-tag)).
