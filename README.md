# sonic-adventure-2

A complete matching decompilation of *Sonic Adventure 2* for the Dreamcast: the original Japanese release, HDR-0165, version 1.008.

<!-- progress -->
**100% of the game's code builds from source, byte for byte.**

| | Binaries | Code bytes | C | Assembly |
|---|---:|---:|---:|---:|
| Main executable | 1 | 1,306,485 | 94.9% | 5.1% |
| Stages | 43 | 11,471,114 | 99.8% | 0.2% |
| Bosses | 8 | 2,254,272 | 99.8% | 0.2% |
| Chao | 8 | 910,120 | 99.9% | 0.1% |
| Other programs | 8 | 553,742 | 99.7% | 0.3% |
| **All** | **68** | **16,495,733** | **99.4%** | **0.6%** |

[@asm tags](TAGS.md#assembly) record evidence of handwritten origins for 90.8% of the assembly.
<!-- /progress -->

See [Accounting](TOOLCHAIN.md#accounting) for methodology.

## Build

The matching build runs on x86-64 Linux with GNU make 4.3 or later and Python 3.8 or later. You also need a C compiler, `sha256sum` and `timeout`. The first build downloads wibo and five releases of Hitachi's SHC, so it also needs `curl`, `tar` and `gzip`. The first `make lint` or `make format` installs clang-format and clang-tidy into a Python venv, so it needs `curl` and Python's `ensurepip`, which Debian and Ubuntu ship as `python3-venv`. Each download is installed only once its hash matches [`tools/SHA256SUMS`](tools/SHA256SUMS).

Bring your own disc: the Japanese release, HDR-0165, version 1.008. None of the disc's files are in this repository. Put `1ST_READ.BIN` and `SONIC2/` in `orig/`, named as on the disc, or put the disc's `.gdi` and its tracks there. For a `.gdi` elsewhere, set `GDI=/path/to/disc.gdi`. Then:

```sh
make          # rebuild every binary and check it against your disc
make check    # check the linked sources without the disc
```

`make` compiles every binary's code, reads the data its sources declare from your disc, and checks each binary it rebuilds against the original, PRS compression and all. They land under `build/`, at the disc's own paths. `make` checks `1ST_READ.BIN` and each file it reads from `SONIC2/` against [`SHA256SUMS`](SHA256SUMS) before reading it. This is a matching build, so an edit that changes a byte of output fails it.

`make check` doesn't require the disc. It compiles every source, using placeholders for imported data where needed, and checks each linked source against its recorded hash, but doesn't build a complete binary. [Checks](CONTRIBUTING.md#checks) says which goals a change needs, and `make help` lists goals and their variables.

Run one `make` invocation at a time per checkout; `-j` within it is fine. SHC is a Windows program, and [wibo](https://github.com/decompals/wibo) runs it. SHC sometimes hangs under wibo, so `make` stops a tool after ten minutes. Run `make` again and it carries on. To use tools you already have, set `WIBO`, `R08`, `R11`, `R13`, `R26` and `R32`: [Running SHC](TOOLCHAIN.md#running-shc) says what each must hold.

On macOS, `brew install make` supplies `gmake`. It runs the [GCC port](#gcc-port)'s goals, `gmake test`, `gmake lint` and `gmake format`, tested on Apple silicon with macOS 26.6, GNU make 4.4.1 and Python 3.12. The matching build's goals need x86-64 Linux.

## The binaries

*Sonic Adventure 2* isn't one binary. Its main executable, `1ST_READ.BIN`, stays in memory, and 60 overlays take turns in one slot beside it: the 43 stages, the 8 bosses, the Chao program `CHAO.PRS` and 8 other programs. `CHAO.PRS` loads one of the 7 Chao areas above itself. [Images](TOOLCHAIN.md#images) maps where each one loads.

## How the build works

- Each binary has tables in `config/`: `splits.txt` places each source's sections at their original addresses and marks the disc ranges, and `symbols.txt` names the addresses the code refers to.
- Each C source compiles with a release of Hitachi's SHC and flags chosen to reproduce its bytes ([Releases](TOOLCHAIN.md#releases)). One source requires a [documented assembly patch](TOOLCHAIN.md#patched-sources) to achieve a match. Shared sources compile once and link at each binary's original addresses.
- Each binary is rebuilt from compiled source, explicitly declared disc data, stored bss and linker fill. Any unaccounted-for byte fails the build.
- For PRS files, the original is decompressed before the byte check and the rebuilt image is compressed afterward. Every binary `make` rebuilds is checked against the original.

Matching binaries establish identical output, not the original source text or build settings.

## GCC port

The GCC port builds the same game sources for the Dreamcast with GCC 16.2 and SH4ZAM, on x86-64 Linux or macOS. Use the Japanese `.gdi` and tracks as above, or set `DISC_FILES=/path/to/files` to a complete extraction including `IP.BIN`. The first build fetches SH4ZAM and builds the [cross-toolchain](PORTS.md#toolchain). You need `c++` for the toolchain build, plus `xz` if `tar` can't unpack `.xz` itself.

```sh
make dc-disc    # build the port's GDI in build/dc/disc/
```

The port was smoke-tested on a stock Dreamcast at the release commit.

It adds a performance readout, a 16:9 view and universal 60 fps ([controls](ENGINE.md#features)). Most of the game already ran at 60 fps, and the option brings two-player games and some cutscenes up to it. Knuckles' and Rouge's [radar](ENGINE.md#the-radar) follows all three pieces at once, and more cutscenes can be [skipped](ENGINE.md#skips), including stage openings. It also adds a [crash screen](ENGINE.md#crash-screen) and repairs many of the shipped game's bugs ([Switches](PORTS.md#switches)).

## Layout

```text
src/       the main executable
modules/   stages, bosses, the Chao world and the other overlays
sdk/       the SDK libraries the binaries link
engine/    C added by the GCC port
config/    each binary's tables, compiler settings, and the GCC port's composition and link scripts
offsets/   offsets of generated data in each object and the targets of its pointers
layouts/   typed data layouts and shared pointer bindings
tools/     the build's tools: its own, vendored and downloaded
tests/     tests for the tools, and for game and engine code on the host and SH-4
```

## Contributing

Contributions are welcome. [CONTRIBUTING.md](CONTRIBUTING.md) says what helps, and follows one source from its placement table to the checked binary.

## Read on

- [TOOLCHAIN.md](TOOLCHAIN.md) documents the matching build, compiler releases, disc data and accounting.
- [TAGS.md](TAGS.md) defines the source annotations and their evidence requirements.
- [PORTS.md](PORTS.md) describes the GCC build, from compiling the sources to building the disc.
- [ENGINE.md](ENGINE.md) describes the code the port adds and its features.

[prskit](https://github.com/reshaped/prskit), [gdi](https://github.com/reshaped/gdi) and [shckit](https://github.com/reshaped/shckit) are companion tools for PRS compression, disc extraction, and SHC assembly analysis and patching. The matching build follows [sonic-adventure](https://github.com/reshaped/sonic-adventure), the decompilation of the first game.

## Acknowledgements

- [wibo](https://github.com/decompals/wibo), by decompals, runs Hitachi's Windows toolchain on Linux. It makes the matching build possible.
- [decomp.me](https://decomp.me) maintains the [compiler archive](https://github.com/decompme/compilers) used by decompilation projects, including the five SHC releases this build downloads.
- [SH4ZAM](https://github.com/gyrovorbis/sh4zam), by Falco Girgis and contributors, supplies the GCC port's SH-4 math routines.
- Stefanos Kornilios Mitsis Poiitidis wrote [`sh4_fpu_approx.h`](https://gist.github.com/skmp/4bd7b38149fc455978b5104201573236), the bit-exact model of the SH-4's FIPR, FTRV, FSCA and FSRRA that the engine tests use. The tree keeps [a copy](tests/kernels/sh4_fpu_approx.h).

## License

The Makefile, `dc.mk`, the make and link scripts in `config/` (`config/**/*.mk` and `config/**/*.ld*`) and the files committed to `engine/`, `tools/` and `tests/` are under the [MIT license](tools/LICENSE), except where a file carries its own notice. This project grants no license for the rest of the repository.

This project isn't affiliated with or endorsed by SEGA.
