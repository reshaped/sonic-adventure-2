# Sonic Adventure 2 (Japan) build. See README.md and TOOLCHAIN.md

.DEFAULT_GOAL := all

## CC=command: use this C compiler instead of cc
## WIBO=path: use this wibo instead of downloading one
WIBO ?= tools/wibo-i686
## R08=dir: use this SHC v5.1r08 instead of downloading it
R08  ?= tools/shc-v5.1r08
## R11=dir: use this SHC v5.1r11 instead of downloading it
R11  ?= tools/shc-v5.1r11
## R13=dir: use this SHC v5.1r13 instead of downloading it
R13  ?= tools/shc-v5.1r13
## R26=dir: use this SHC v5.0r26 instead of downloading it
R26  ?= tools/shc-v5.0r26
## R32=dir: use this SHC v5.0r32 instead of downloading it
R32  ?= tools/shc-v5.0r32
## SH_ELF=dir: use this sh-elf toolchain instead of building one
SH_ELF ?= tools/sh-elf
## DC_LTO_JOBS=mode: use jobserver or set N LTO workers per SH-4 link
## SH4ZAM=dir: take SH4ZAM's files from this copy where their hashes match
## GDI=path: use this original disc descriptor and its tracks
## DISC_FILES=dir: build a disc from a complete Japanese file extraction
# tools/link.py runs R32's asmsh, lnk and cnvs under wibo
export WIBO R32
# The releases the build runs: each route's, and the linker's
RELEASES := R08 R11 R13 R26 R32
## V=1: print commands, and each image's bytes by what writes them
VERBOSE := $(filter-out 0,$(V))
Q := $(if $(VERBOSE),,@)
# Python bytecode for tools/ goes under build/, not beside the sources. make
# before 4.4 passes no exported variable to $(shell), so a $(shell) that runs
# Python sets it there too
export PYTHONPYCACHEPREFIX := build/pycache
python := PYTHONPYCACHEPREFIX=$(PYTHONPYCACHEPREFIX) python3

# Stop before a goal runs on a host that lacks what it needs, and name what
# it lacks; README.md#build lists it. A goal that names a file gets no check
# of the commands it needs. macOS's own make is 3.81, so the stop on make's
# version says how a Mac gets a later one
ifeq ($(filter grouped-target,$(.FEATURES)),)
$(error the build needs GNU make 4.3 or later, and this is $(MAKE_VERSION); on macOS, brew install make and run gmake; see README.md#build)
endif
GOALS      := $(or $(MAKECMDGOALS),all)
# The host, as uname's system and machine name it. SHC's tools, SHC and
# R32's asmsh, lnk and cnvs, are 32-bit Windows programs, and the build runs
# them under wibo, which runs on Linux on x86 alone. So on macOS make
# refuses MATCHING_GOALS, the goals a person types that run SHC's tools,
# before it runs Python or fetches a tool, under make -n too
HOST       := $(shell uname -sm)
MACOS      := $(filter Darwin,$(firstword $(HOST)))
MATCHING_GOALS := all check offsets progress relink shift source accept
# $(call no_shc,NAMES) is the stop for NAMES, the goals or the file that
# need SHC's tools on macOS. A # outside a function call would open a
# comment, so $(strip) keeps README.md#build whole
no_shc     = $(strip $1: the matching build needs Linux on x86, where wibo runs SHC's tools, and this is macOS; see README.md#build)
ifneq ($(MACOS),)
# On Apple silicon, macOS runs an x86-64 make under Rosetta 2, where uname
# names an Intel Mac. Such a make would install an Intel Mac's wheels and
# build make dc's toolchain for one, and each would run under Rosetta 2 in
# turn. sysctl.proc_translated is 1 in a process Rosetta 2 translates, and
# there make stops, whatever its goal
ifeq ($(shell sysctl -n sysctl.proc_translated 2> /dev/null),1)
$(error the build needs a native make, and this one runs under Rosetta 2; see README.md#build)
endif
ifneq ($(filter $(MATCHING_GOALS),$(GOALS)),)
$(error $(call no_shc,$(filter $(MATCHING_GOALS),$(GOALS))))
endif
endif
ifneq ($(shell $(python) -c 'import sys; print(sys.version_info >= (3, 8))'),True)
$(error the build needs Python 3.8 or later, as python3)
endif
# On its command line make takes the settings make help lists, and no other
# variable. Each setting changes no output, or make stamps or sums what it
# changes. A variable given on the command line overrides the makefiles' own
# assignments of it, so any other could change what make builds with no
# stamp of the change. A misspelt setting would change nothing, and make
# would not say so
SETTINGS := CC WIBO R08 R11 R13 R26 R32 SH_ELF DC_LTO_JOBS SH4ZAM GDI DISC_FILES V HUNG GRACE SOURCE FETCH_TIMEOUT
UNLISTED := $(filter-out $(SETTINGS),$(foreach v,$(.VARIABLES),$(if $(filter command line,$(origin $v)),$v)))
ifneq ($(UNLISTED),)
$(error $(UNLISTED): make takes no such setting; make help lists those it takes)
endif
# The matching goals that build the tree's sources. accept runs the matching
# build in its gates, each a make of its own
BUILDING   := $(filter-out accept,$(filter $(MATCHING_GOALS),$(GOALS)))
# These goals need no per-source build rules, dependency files or tool stamps
COMPILING  := $(filter-out accept clean format help lint review stub-includes test,$(GOALS))
# The goals that build with GCC, and the goals for which make reads dc.mk,
# make dc's rules: those, and a file under tools/sh-elf/ or tools/sh4zam/,
# which dc.mk alone builds or fetches. A fault in dc.mk stops only a run
# that names one of these goals. SHC_GOALS is the rest of this run's goals
# that compile, a file among them: those that may run SHC's tools
DC_GOALS   := $(filter dc dc-check dc-images dc-files dc-disc build/dc/%,$(MAKECMDGOALS))
DC_MK_GOALS := $(strip $(DC_GOALS) $(filter tools/sh-elf/% tools/sh4zam/%,$(MAKECMDGOALS)))
SHC_GOALS  := $(filter-out dc dc-check dc-images dc-files dc-disc build/dc/% tools/sh-elf/% tools/sh4zam/%,$(COMPILING))
PINNED     := $(filter tools/%,$(WIBO) $(foreach r,$(RELEASES),$($r)/shc.exe))
LLVM       := tools/llvm/bin
CLANG_TOOLS := $(LLVM)/clang-format $(LLVM)/clang-tidy
# clang-format's output and clang-tidy's checks change between releases, so
# both are pinned: to one release's wheels for the host, and to the lines
# tools/SHA256SUMS gives them, which an install writes to INSTALLED once both
# tools are in place. Unless both tools are there and INSTALLED holds those
# lines, make installs the wheels before it runs either tool. WHEEL is the
# wheels' platform tag: x86-64 Linux's, or macOS's on either processor. Any
# other host has no WHEEL, and there make lint and make format stop before
# they read a table or fetch a wheel
ifeq ($(HOST),Linux x86_64)
WHEEL      := manylinux_2_27_x86_64.manylinux_2_28_x86_64
else ifeq ($(HOST),Darwin arm64)
WHEEL      := macosx_11_0_arm64
else ifeq ($(HOST),Darwin x86_64)
WHEEL      := macosx_10_9_x86_64
endif
no_wheel   = $(strip clang-format and clang-tidy need x86-64 Linux or macOS, whose wheels make pins, and this is $(HOST); see README.md#build)
ifeq ($(WHEEL),)
ifneq ($(filter lint format,$(GOALS)),)
$(error $(no_wheel))
endif
endif
CLANG_FORMAT_WHEEL := tools/llvm/clang_format-22.1.8-py2.py3-none-$(WHEEL).whl
CLANG_TIDY_WHEEL   := tools/llvm/clang_tidy-22.1.8-py2.py3-none-$(WHEEL).whl
wheel_sums = awk '$$2 == "$(CLANG_FORMAT_WHEEL)" || $$2 == "$(CLANG_TIDY_WHEEL)"' tools/SHA256SUMS
INSTALLED  := tools/llvm/wheels.txt
ifneq ($(wildcard $(CLANG_TOOLS) $(INSTALLED)) $(strip $(file <$(INSTALLED))),$(CLANG_TOOLS) $(INSTALLED) $(strip $(shell $(wheel_sums))))
UNINSTALLED := $(CLANG_TOOLS)
endif
DOWNLOADS  := $(strip $(if $(BUILDING),$(filter-out $(wildcard $(PINNED)),$(PINNED))) \
	$(if $(filter lint format,$(GOALS)),$(UNINSTALLED)))
HOST_NEEDS := $(if $(BUILDING)$(filter lint,$(GOALS)),$(firstword $(CC))) \
	$(if $(BUILDING)$(DOWNLOADS)$(filter accept,$(GOALS)),sha256sum) $(if $(BUILDING),timeout) $(if $(DOWNLOADS),curl) \
	$(if $(filter %/shc.exe,$(DOWNLOADS)),tar gzip) $(if $(filter accept,$(GOALS)),git)
# Installing clang-format and clang-tidy runs python3 -m venv, which needs
# Python's ensurepip; Debian and Ubuntu ship it apart, in python3-venv.
# macOS's own sha256sum reads a list from its standard input only where the
# list is named -, and passes an empty list, which GNU's fails. So each list
# piped to sha256sum -c is named -, and a rule whose list can be empty stops
# first where its file has no line
HOST_LACKS := $(shell for c in $(HOST_NEEDS); do command -v $$c > /dev/null || echo $$c; done; \
	$(if $(filter $(CLANG_TOOLS),$(DOWNLOADS)),$(python) -c 'import ensurepip' 2> /dev/null || echo python3-venv))
ifneq ($(HOST_LACKS),)
$(error $(HOST_LACKS): not found; see README.md#build)
endif

include config/compilers.mk

# Stop on a route whose release the build doesn't run, and on a source a
# route names that isn't there: a misspelt name in a list leaves the source
# it means to a route that finds sources by directory
UNRELEASED := $(filter-out $(RELEASES),$(foreach r,$(ROUTES),$($r.release)))
ifneq ($(UNRELEASED),)
$(error config/compilers.mk: no release in RELEASES for $(UNRELEASED))
endif
ABSENT := $(strip $(foreach r,$(ROUTES),$(filter-out $(wildcard $($r.sources)),$($r.sources))))
ifneq ($(ABSENT),)
$(error config/compilers.mk: $(ABSENT): no such source)
endif
# Normalize dot and parent-directory components before collecting object
# names. Both the rule generator and the image placements use these paths.
$(foreach r,$(ROUTES),$(eval override $r.sources := $(subst $(CURDIR)/,,$(abspath $($r.sources)))))
# Every route's C sources, and their objects
C_SOURCES := $(foreach r,$(ROUTES),$($r.sources))
ifneq ($(words $(C_SOURCES)),$(words $(sort $(C_SOURCES))))
REPEATED := $(sort $(foreach s,$(C_SOURCES),$(if $(word 2,$(filter $s,$(C_SOURCES))),$s)))
$(error config/compilers.mk: $(foreach s,$(REPEATED),$s listed more than once in routes $(strip \
	$(foreach r,$(ROUTES),$(if $(filter $s,$($r.sources)),$r)))))
endif
C_OBJ    := $(C_SOURCES:%.c=build/obj/%.obj)
# Stop on a patched source that no route compiles, and on one that names no
# patch
UNROUTED := $(filter-out $(C_SOURCES),$(PATCHED))
ifneq ($(UNROUTED),)
$(error config/compilers.mk: $(UNROUTED): patched, but no route compiles it)
endif
UNPATCHED := $(strip $(foreach s,$(PATCHED),$(if $($s.patches),,$s)))
ifneq ($(UNPATCHED),)
$(error config/compilers.mk: $(UNPATCHED): patched, but its .patches names no patch)
endif
# Stop the same way on a source of WARNED that no route compiles, and on one
# that names no warning
UNROUTED := $(filter-out $(C_SOURCES),$(WARNED))
ifneq ($(UNROUTED),)
$(error config/compilers.mk: $(UNROUTED): warned, but no route compiles it)
endif
UNWARNED := $(strip $(foreach s,$(WARNED),$(if $($s.warnings),,$s)))
ifneq ($(UNWARNED),)
$(error config/compilers.mk: $(UNWARNED): warned, but its .warnings names no warning)
endif
# The C sources that hold assembly: an inline body or a #pragma asm block.
# SHC takes either only where it writes assembly text, so such a source
# compiles to text, which R32's asmsh assembles
INLINE   := $(shell grep -lE '^#pragma (inline_asm|asm)' $(C_SOURCES) /dev/null)
# The assembly sources, which R32's asmsh assembles as they stand
ASM_SOURCES := $(shell find src sdk modules -name '*.src' 2>/dev/null)
ASM_OBJ  := $(ASM_SOURCES:%.src=build/obj/%.obj)
# Stop on a C source and an assembly source that would build one object
SHARED_OBJ := $(filter $(ASM_OBJ),$(C_OBJ))
ifneq ($(SHARED_OBJ),)
$(error $(SHARED_OBJ:build/obj/%.obj=%) has a .c and a .src, which would build one object)
endif
# The C files lint and format read: the routes' sources and headers, and
# where they are, engine/'s, which make dc alone compiles, and those of
# tests/kernels/, which the kernels' test compiles for the host. clang-format
# leaves tests/kernels/sh4_fpu_approx.h in its author's form, under its own
# notice. clang-tidy reads the routes' sources alone: compile_flags.txt has
# it read C89 against the tree's own headers, and engine/'s C models and
# tests/kernels/ include the host's C library
LINTED   := src sdk modules $(wildcard engine tests/kernels)
C_FILES  := $(shell find $(LINTED) -name '*.[ch]' 2>/dev/null)
# Only src, sdk and modules produce original compiler objects. The port and
# host fixtures have their own inventories.
UNROUTED := $(filter-out $(C_SOURCES) engine/% tests/%,$(filter %.c,$(C_FILES)))
ifneq ($(UNROUTED),)
$(error config/compilers.mk: no matching compiler route for $(UNROUTED))
endif
FORMATTED := $(filter-out tests/kernels/sh4_fpu_approx.h,$(C_FILES))
TIDIED   := $(patsubst %,build/lint/%.tidy,$(filter-out engine/% tests/%,$(filter %.c,$(C_FILES))))
# clang-tidy takes a source's settings from the nearest .clang-tidy in its
# directory or above
TIDY_CONFIGS := .clang-tidy $(sort $(shell find src sdk modules -name .clang-tidy 2>/dev/null))

# $(call from_config,ITEMS) gives each item of ITEMS, a Python iterable in
# which config names tools/config.py, and stops make where the query fails. A
# comma outside parentheses in ITEMS would end make's argument, and a single
# quote the shell's word
from_config = $(shell $(python) -c 'import sys; sys.path[:0] = ["tools"]; import config; print(*$1)')$(if $(filter-out 0,$(.SHELLSTATUS)),$(error tools/config.py: $1 failed))
# image and object take a link's stem, <image>/<source less its extension>
image    = $(firstword $(subst /, ,$1))
object   = build/obj/$(patsubst $(call image,$1)/%,%,$1).obj
# The original an image is checked against, by its file on the disc: the file
# in orig/, or for a .PRS file the .BIN build/orig/ decodes it to
original = $(if $(filter %.PRS,$1),build/orig/$(1:.PRS=.BIN),orig/$1 build/verified/$1.ok)

# make reads the tables only for a goal that compiles, so a table being
# edited stops no make test or make format. Lint's tag check reads them itself
ifneq ($(COMPILING),)
# Each image's name and its file on the disc, as NAME:FILE. Reading them
# checks every table
IMAGES   := $(call from_config,(f"{name}:{config.Image(name).file}" for name in config.images()))
FILES    := $(foreach i,$(IMAGES),$(lastword $(subst :, ,$i)))
CONTENT_TABLES := $(wildcard config/*/data.txt)
CONTENT_IMAGES := $(patsubst config/%/data.txt,%,$(CONTENT_TABLES))
# A layout shared by several images contributes a prerequisite to each reader.
content_layouts = $(addprefix layouts/,$(addsuffix .txt,$(shell awk '{sub(/\043.*/, "")} $$2 == "layout" {print $$3}' config/$1/data.txt)))
# A compiled filename bank supplies real C pointers to a stage's land geometry.
content_filenames = $(shell awk '{sub(/\043.*/, "")} ($$3 == "land_filenames" || $$3 == "text_bank") {print $$4}' config/$1/data.txt)
ORIG     := $(addprefix orig/,$(FILES))
REBUILT  := $(addprefix build/,$(FILES))
# Each image linked whole, which make relink compares with its splice
RELINKED := $(foreach i,$(IMAGES),build/relink/$(firstword $(subst :, ,$i)).bin)
# Each image's list of the words its shift test finds keeping a value in its
# span, which make shift joins
FIXED    := $(RELINKED:.bin=/fixed.txt)
# One link for each source in each image whose splits.txt places it, at that
# image's addresses. make dc's census reads which sources each image places
# from them too
LINKS    := $(sort $(shell awk '{ sub(/#.*/, "") } NF == 4 && $$1 ~ "/" { split(FILENAME, conf, "/"); \
	sub(/\.[a-z]+$$/, ".bin", $$1); print "build/link/" conf[2] "/" $$1 }' config/*/splits.txt))
# Stop on a source no splits.txt places: no goal would build it
UNPLACED := $(filter-out $(foreach l,$(LINKS),$(call object,$(l:build/link/%.bin=%))),$(C_OBJ) $(ASM_OBJ))
ifneq ($(UNPLACED),)
$(error $(UNPLACED:build/obj/%.obj=%) has no row in any splits.txt)
endif
# The files SHA256SUMS records, one line each, and no others: the disc's
# files, and each linked source
EXPECTED := $(ORIG) $(LINKS)
# Each link keeps beside it, as build/link/IMAGE/PATH.rows, the rows of the
# tables it read: its source's ranges, and the address of each name its
# object defines or takes. tools/link.py rewrites those the tables now give
# otherwise, so an edit of a table relinks only the links whose rows it moves
ifneq ($(filter-out offsets build/offsets/%,$(SHC_GOALS)),)
$(shell $(python) tools/link.py --rows)
ifneq ($(.SHELLSTATUS),0)
$(error tools/link.py --rows failed)
endif
endif
endif

# A tool's stamp holds the hashes of its files, so replacing one, or pointing
# at another copy, rebuilds what it built. SHC runs under wibo, so a release's
# stamp follows wibo's
WIBO_ID := build/tools/wibo.id
$(foreach r,$(RELEASES),$(eval $r_ID := build/tools/$r.id $(WIBO_ID)))

comma := ,
empty :=
space := $(empty) $(empty)
define newline


endef
# $1 as one shell word, whatever it holds, as a person's .gdi name may
quoted = '$(subst ','\'',$1)'
# Print a step's status line: its label, $1, padded to the longest label's
# six characters, then $2 as given. tools/splice.py prints its line in this
# column too
status = printf '  %-6s %s\n' '$1' $(call quoted,$2)
# SHC under wibo now and then deadlocks. A tool still running after HUNG
# seconds is taken for hung: timeout sends TERM to it and what it started, and
# KILL if it still runs GRACE seconds on, so make can be run again.
# tools/link.py stops the tools it runs by the same two
## HUNG=secs: send a tool under wibo TERM after this long, not 600
HUNG  := 600
## GRACE=secs: send a tool under wibo KILL this long after TERM, not 10
GRACE := 10
export HUNG GRACE
# CI retries only a tool whose supervisor actually timed out. Its wrapper writes
# the final attempt to the same log quiet and warned inspect on an ordinary run.
ifeq ($(RETRY_WIBO_TIMEOUT),1)
wibo = sh tools/retry_wibo.sh $@.log '$(subst $(space),|,$(strip $1))' $(WIBO)
wibo_log =
else
wibo = timeout --verbose -k $(GRACE) $(HUNG) $(WIBO)
wibo_log = > $@.log 2>&1
endif
# Keep SHC's output in $@.log, and print it and fail on a nonzero status or
# on any error or warning it reports, since some warnings come with status 0
quiet = $(wibo_log) && ! grep -q ' ([EWF]) ' $@.log || (cat $@.log; false)
# $(call warned,NUMBERS) takes quiet's place for a compile whose source
# WARNED lists with the warnings NUMBERS. It passes those warnings, and fails
# on any other error or warning, and on each of NUMBERS SHC no longer gives
warned = $(wibo_log) || { cat $@.log; false; } && \
	{ ! grep ' ([EWF]) ' $@.log | grep -qvE ': ($(subst $(space),|,$(strip $1))) \(W\) ' || \
	{ cat $@.log; echo '$<: SHC reports what its .warnings does not list'; false; }; } \
	$(foreach w,$1,&& { grep -q ': $w (W) ' $@.log || { cat $@.log; echo '$<: SHC no longer gives warning $w'; false; }; })
# SHC names its temporary files with a few digits, as SHC085b, so two compiles
# sharing SHC_TMP can clash; each object takes a directory of its own
shc_tmp = $(@:build/obj/%.obj=build/tmp/%)
# $(call shc,RELEASE,INCLUDES,SOURCE,OPTIONS) runs RELEASE's shc.exe on SOURCE
# with OPTIONS, searching INCLUDES, directories split by commas, for headers
shc = mkdir -p $(shc_tmp) && SHC_LIB=$1 SHC_TMP=$(shc_tmp) SHC_INC=$2 $(call wibo,$($3.warnings)) $1/shc.exe $3 $4 \
	$(if $(filter $3,$(WARNED)),$(call warned,$($3.warnings)),$(quiet))
# asmsh reads / as an option's prefix, so a path goes to it with \ for /
dos = '$(subst /,\,$1)'
# $(call asmsh,TEXT) has R32's asmsh assemble TEXT into $@, and write its
# listing beside it, which tools/splice.py and tools/progress.py read. asmsh
# closes the listing last, so the object is then dated after it
asmsh = $(call wibo,) $(R32)/asmsh.exe $(call dos,$1) -object=$(call dos,$@) -list=$(call dos,$(@:.obj=.lis)) $(ASMFLAGS) $(quiet) && \
	touch $@
# $(call patch,PATCHES) has build/shckit apply PATCHES to SHC's text for $@,
# and write the result beside that text, as PATH_patched.src. shckit names
# each line it refuses, and then writes nothing. Patches that leave the text
# as it was fail too, since they find nothing they are there for
patch = build/shckit apply $(foreach p,$1,-p $p) -o $(@:.obj=_patched.src) $(@:.obj=.src) && \
	{ ! cmp -s $(@:.obj=.src) $(@:.obj=_patched.src) || { echo '$<: shckit changes nothing in its assembly with $1'; false; }; }
# Write an object's header dependencies to its .d file. The host compiler's
# preprocessor searches the same directories SHC does, in the same order, and
# with DREAMCAST it reads what SHC reads: a source keeps under that switch
# the includes that only its inline bodies need
depend = $(CC) -MM -MP -nostdinc -DDREAMCAST $(addprefix -I,$(subst $(comma), ,$1)) -MT $@ \
	-MF $(@:.obj=.d).tmp $< && mv $(@:.obj=.d).tmp $(@:.obj=.d)

# A source with extracted initializers has one layout-only object for checks
# and a second object with its actual data for the retail image. Keep the
# products in separate trees so check and all can run together.
OFFSET_TABLES := $(if $(wildcard offsets),$(shell find offsets -name '*.txt' | sort))
BODY_C := $(patsubst offsets/%.txt,%.c,$(OFFSET_TABLES))
BODY_GOALS := $(strip $(filter-out tools/%,$(COMPILING)) $(filter lint stub-includes,$(GOALS)))
ifneq ($(BODY_GOALS),)
# Include wrappers have their own native placements. Discover them before any
# compiler starts, including for the disc and GCC goals that consume offsets.
PLACED_BODY_C := $(shell $(python) tools/body_offsets.py --sources)
ifneq ($(.SHELLSTATUS),0)
$(error tools/body_offsets.py: cannot find native body sources)
endif
BODY_C := $(PLACED_BODY_C)
ifneq ($(filter-out offsets build/offsets/%.txt,$(BODY_GOALS)),)
MISSING_OFFSETS := $(filter-out $(OFFSET_TABLES),$(PLACED_BODY_C:%.c=offsets/%.txt))
ifneq ($(MISSING_OFFSETS),)
$(error $(MISSING_OFFSETS): missing offsets for $(MISSING_OFFSETS:offsets/%.txt=%.c); run make offsets)
endif
endif
ORPHAN_OFFSETS := $(filter-out $(PLACED_BODY_C:%.c=offsets/%.txt),$(OFFSET_TABLES))
ORPHAN_BINDINGS := $(filter-out $(PLACED_BODY_C:%.c=offsets/%.bindings),$(if $(wildcard offsets),$(shell find offsets -name '*.bindings')))
endif
CHECK_LINKS := $(LINKS)
ifneq ($(BODY_C),)
ifneq ($(strip $(COMPILING) $(filter lint stub-includes,$(GOALS))),)
include config/bodies.mk
endif
endif

# Each file make rebuilds is checked against its original's line
## build every binary and check each against SHA256SUMS
all: $(REBUILT) $(STUB_LINKS)
ifneq ($(BODY_LINKS),)
	$(Q)python3 tools/body_links.py
	$(file >build/body-links.txt,$(BODY_LINKS) $(STUB_LINKS))
	$(Q)awk 'NR == FNR {for (i=1; i<=NF; i++) full[$$i]=1; next} $$2 in full' build/body-links.txt SHA256SUMS | sha256sum -c --quiet -
endif
	$(file >build/expected.txt,$(EXPECTED))
	$(Q)python3 tools/sums.py build/expected.txt
	$(Q)awk '$$2 ~ "^orig/" { sub("^orig", "build", $$2); print $$1 "  " $$2 }' SHA256SUMS | sha256sum -c --quiet -
	@$(call status,sums,$(words $(EXPECTED)) files match SHA256SUMS)

# Needs no disc: each linked source is checked against its own line. Its
# list of the expected files is its own, so make -j all check writes no
# list that the other goal's tools/sums.py reads
## check the linked sources, the tables and README.md's numbers
check: $(CHECK_LINKS) $(MEASURED:.txt=.ok)
	$(if $(ORPHAN_BINDINGS),$(error $(ORPHAN_BINDINGS): bindings without a native body source))
	$(if $(ORPHAN_OFFSETS),$(error $(ORPHAN_OFFSETS): offsets without a body source; run make offsets))
	$(file >build/check-expected.txt,$(EXPECTED))
	$(Q)python3 tools/sums.py build/check-expected.txt
	$(file >build/checked.txt,$(CHECK_LINKS))
	$(Q)awk 'NR == FNR {for (i=1; i<=NF; i++) check[$$i]=1; next} $$2 in check' build/checked.txt SHA256SUMS | sha256sum -c --quiet -
	@$(call status,sums,$(words $(CHECK_LINKS)) files match SHA256SUMS)
	$(Q)python3 tools/splice.py --check
	$(Q)python3 tools/names.py
	$(Q)python3 tools/progress.py --check

## link each binary whole, and compare it with its splice
relink: $(RELINKED)

## link each binary at two shifted bases, and check its words
shift: build/relink/fixed.txt

# Only this explicit goal updates the tracked placements. Normal builds and
# make check leave source metadata alone.
## regenerate C data placements from native compiler listings
offsets: $(MEASURED)
	$(Q)for table in $(MEASURED:build/%=%); do cmp -s build/$$table $$table || { \
		printf '  %-6s %s\n' offset $$table && mkdir -p $$(dirname $$table) && cp build/$$table $$table.part && mv -f $$table.part $$table; } || exit 1; done
	$(Q)for table in $(ORPHAN_OFFSETS); do printf '  %-6s %s\n' remove $$table && rm -- $$table || exit 1; done

# Needs neither the disc nor SHC
## run the tests, and check the vendored tools' files
test:
ifneq ($(origin SH_ELF),file)
	$(Q)for tool in gcc as ld nm objcopy objdump; do \
		test -x $(call quoted,$(SH_ELF))/bin/sh-elf-$$tool || { echo "SH_ELF lacks sh-elf-$$tool" >&2; exit 1; }; done
	$(Q)test -f $(call quoted,$(or $(SH4ZAM),tools/sh4zam))/include/sh4zam/shz_xmtrx.h || \
		{ echo "make test needs SH4ZAM headers. Run make tools/sh4zam/fetched.txt or set SH4ZAM." >&2; exit 1; }
endif
	$(Q)PYTHONPATH=tools:tests python3 -m unittest discover -s tests $(if $(VERBOSE),-v,-q)
	$(Q)python3 tools/vendor.py --check

# Lint what changed since the last run that passed: clang-format the files
# edited since, or all of them when the style or the tool changes, and
# clang-tidy each C source whose text, headers or settings changed. make -k
# lint goes on past the first failure
## check formatting, diagnostics, declarations, guards and tags
lint: build/lint/format.ok $(TIDIED) build/lint/decls.ok build/lint/guards.ok build/lint/tags.ok \
	$(if $(ASM_SOURCES),build/lint/asm.ok)

## format every C file in place
format: | $(LLVM)/clang-format
	$(Q)$(LLVM)/clang-format -i $(FORMATTED)

## list the open assembly, and count the @asm tags by kind
review:
	$(Q)python3 tools/tags.py --review $(C_FILES) $(ASM_SOURCES)

COMPILE_FLAGS := $(file <compile_flags.txt)
# $(call compile_arguments,FILE,SOURCES) is the command that compiles FILE,
# whose text includes that of the body sources SOURCES: clang with
# compile_flags.txt's flags, after the directory of each variant among
# SOURCES, which comes first for the reason TIDY_BODY_ROOT does in lint
compile_arguments = clang $(addprefix -I,$(filter build/stub/variants/%,$(foreach s,$2,$(call body_root,$s,build/stub)))) \
	$(COMPILE_FLAGS) -c $1
# $(call compile_command,FILE,SOURCES) is FILE's entry in
# build/compile_commands.json. The checkout's path is the one text in it
# that the tree doesn't choose, so only it has its \ and " escaped
compile_command = {"directory": "$(subst ",\",$(subst \,\\,$(CURDIR)))", "file": "$1", \
	"arguments": [$(subst $(space),$(comma) ,$(patsubst %,"%",$(call compile_arguments,$1,$2)))]}
# $(call included_bodies,FILE) is the body sources whose text FILE includes,
# as the preprocessor finds them under compile_flags.txt's directories. A
# failed scan stops make, since its empty output would name no body
included_bodies = $(filter $(BODY_C),$(patsubst $(CURDIR)/%,%,$(abspath $(shell $(CC) -MM -MG -nostdinc \
	$(filter -I%,$(COMPILE_FLAGS)) $1))))$(if $(filter-out 0,$(.SHELLSTATUS)),$(error $1: $(CC) -MM failed))
# The entries: each C source's, and each test's, since a test may include a
# variant's text. The goal writes them one to a line, breaking the list after
# the ]} that closes each
compile_commands = $(foreach s,$(C_SOURCES),$(call compile_command,$s,$s)) \
	$(foreach t,$(sort $(shell find tests -name '*.c' 2>/dev/null)),$(call compile_command,$t,$(call included_bodies,$t)))
## write the stub includes and compile commands clangd reads
stub-includes: $(foreach s,$(BODY_C),$(call body_includes,$s,build/stub))
	$(file >build/compile_commands.json,[$(subst ]} {,]}$(comma)$(newline){,$(strip $(compile_commands)))])
	@$(call status,json,build/compile_commands.json)

# tools/progress.py reads each object's section table, so it waits for the
# links make check builds
## report what writes the code, and write README.md's numbers
progress: $(CHECK_LINKS)
	$(Q)python3 tools/progress.py
	$(Q)python3 tools/progress.py --write

## remove build/
clean:
	$(Q)rm -rf build
# make -j runs the goals at once, and rm -rf build would race their writes
ifneq ($(and $(filter clean,$(MAKECMDGOALS)),$(filter-out clean,$(MAKECMDGOALS))),)
$(error make clean takes no other goal)
endif
# make -j runs the goals at once, and accept's gates, each a make of its own
# in this checkout, would race the other goals' writes
ifneq ($(and $(filter accept,$(MAKECMDGOALS)),$(filter-out accept,$(MAKECMDGOALS))),)
$(error make accept takes no other goal)
endif

# The ## line directly above a goal says what it does. A variable's opens
# with the form a person gives it
## list these goals and variables
help:
	$(Q)awk -F: '/^## [A-Z0-9_]+=/ { variables = variables sprintf("%-20s%s\n", substr($$1, 4), substr($$0, length($$1) + 3)); next } \
		/^## / { does = substr($$0, 4); getline; printf "%-20s%s\n", $$1, does } \
		END { printf "%s", variables }' $(MAKEFILE_LIST)

# An empty recipe lets an object's .d file, which keeps its header
# dependencies, go missing without stopping make: the object, which depends
# on it, then builds again and writes it
ifneq ($(COMPILING),)
$(C_OBJ) $(STUB_OBJ): %.obj: %.d
$(C_OBJ:.obj=.d) $(STUB_OBJ:.obj=.d): ;
-include $(C_OBJ:.obj=.d) $(STUB_OBJ:.obj=.d)
endif

# An assembled object's listing may go missing the same way: the object
# depends on it, and assembles again to write it. config/bodies.mk gives
# every stub object this rule, the assembled ones among them
ASSEMBLED := $(ASM_OBJ) $(patsubst %.c,build/obj/%.obj,$(INLINE) $(PATCHED))
ifneq ($(COMPILING),)
$(ASSEMBLED): %.obj: %.lis
$(ASSEMBLED:.obj=.lis): ;
endif

# $(call route,NAME) gives the rules that compile route NAME's sources with
# its release and flags: straight to an object, or for a source of INLINE to
# the text asmsh assembles, which stays beside the object. A source of
# PATCHED compiles to that text too, and asmsh assembles the text shckit
# patches from it, which stays beside it
define route
$$(patsubst %.c,build/obj/%.obj,$$(filter-out $$(INLINE) $$(PATCHED),$$($1.sources))): build/obj/%.obj: %.c \
		build/obj/%.cmd $$($$($1.release)_ID)
	@$$(call status,$$($1.release),$$<)
	$$(Q)mkdir -p $$(@D)
	$$(Q)$$(call depend,$$($1.includes))
	$$(Q)$$(call shc,$$($$($1.release)),$$($1.includes),$$<,-objectfile=$$@ -code=machinecode $$($1.flags))
$$(patsubst %.c,build/obj/%.obj,$$(filter-out $$(PATCHED),$$(filter $$(INLINE),$$($1.sources)))): build/obj/%.obj: \
		%.c build/obj/%.cmd $$($$($1.release)_ID) $$(R32_ID)
	@$$(call status,$$($1.release),$$<)
	$$(Q)mkdir -p $$(@D)
	$$(Q)$$(call depend,$$($1.includes))
	$$(Q)$$(call shc,$$($$($1.release)),$$($1.includes),$$<,-objectfile=$$(@:.obj=.src) -code=asmcode $$($1.flags))
	$$(Q)$$(call asmsh,$$(@:.obj=.src))
$$(patsubst %.c,build/obj/%.obj,$$(filter $$(PATCHED),$$($1.sources))): build/obj/%.obj: %.c build/obj/%.cmd \
		$$($$($1.release)_ID) $$(R32_ID) build/shckit
	@$$(call status,$$($1.release),$$< (shckit $$($$<.patches)))
	$$(Q)mkdir -p $$(@D)
	$$(Q)$$(call depend,$$($1.includes))
	$$(Q)$$(call shc,$$($$($1.release)),$$($1.includes),$$<,-objectfile=$$(@:.obj=.src) -code=asmcode $$($1.flags))
	$$(Q)$$(call patch,$$($$<.patches))
	$$(Q)$$(call asmsh,$$(@:.obj=_patched.src))
endef
$(foreach r,$(ROUTES),$(eval $(call route,$r)))
$(foreach r,$(ROUTES),$(if $(filter $(BODY_C),$($r.sources)),$(eval $(call stub_route,$r))))

# The rule that assembles each .src file, a macro so that its text can go
# into each one's .cmd file
define assemble
$$(ASM_OBJ): build/obj/%.obj: %.src build/obj/%.cmd $$(R32_ID)
	@$$(call status,asm,$$<)
	$$(Q)mkdir -p $$(@D)
	$$(Q)$$(call asmsh,$$<)
endef
$(eval $(assemble))

# Each object's .cmd file holds what builds it. For a C source that is its
# route's release, flags and search path and the text of the macros COMPILE
# names. For a .src file it is R32 and the text of assemble. Where asmsh
# assembles the object, asmsh's flags and the text of the macros ASSEMBLE
# names join it, and for a patched source its patches and the text of patch.
# As make reads this file it rewrites a .cmd file only where that text
# changes. So an object builds again where what builds it changes, and no
# other edit here or in config/compilers.mk changes its .cmd file. Its other
# prerequisites still build it: build/shckit builds again on any edit here,
# and each patched object after it. A macro the rules come to run joins
# COMPILE or ASSEMBLE
COMPILE  := route shc shc_tmp depend quiet warned wibo wibo_log
ASSEMBLE := asmsh dos quiet wibo wibo_log
# $(call unexpanded,MACROS) gives the text of each of MACROS as written
unexpanded = $(foreach m,$1,$(value $m))
# $(call keep,FILE,TEXT) writes TEXT to FILE unless FILE holds it already. A
# missing FILE holds no text, not even an empty one
ifneq ($(filter 4.3,$(MAKE_VERSION)),)
# GNU make 4.3 can retain the final newline of a long $(file <...) read
# (Savannah #58497). Compare proposed replacements byte for byte in one
# batch before updating targets. A make process owns its own temporary files.
define keep_changed
$(if $(stamp_batch),,$(eval stamp_batch := $(shell mktemp build/.stamps.XXXXXXXX)))
$(if $(stamp_batch),,$(error could not create the stamp batch))
$(file >$1$(notdir $(stamp_batch)),$2)$(file >>$(stamp_batch),$1)
endef
else
keep_changed = $(file >$1,$2)
endif
keep = $(if $(wildcard $1),$(if $(subst x$(file <$1),,x$2)$(subst x$2,,x$(file <$1)),$(call keep_changed,$1,$2)),$(file >$1,$2))
# Optional pointer and text bindings can be added, removed or renamed.
# Record their presence per source so only that source's includes regenerate.
ifneq ($(BINDING_IDS),)
$(shell mkdir -p $(sort $(dir $(BINDING_IDS))))
$(foreach s,$(BODY_C),$(call keep,build/body-inputs/$(basename $s).id,$(if $(wildcard offsets/$(basename $s).bindings),bindings,none)))
endif
# A header's directory can supply clang-tidy settings too. Track config file
# membership even when the removed or moved config never appears in a .d file.
# The checks of the whole tree keep the names of the files they read, so a
# file added with an old date, or removed, checks them again
ifneq ($(filter lint build/lint/%,$(GOALS)),)
$(shell mkdir -p build/lint)
$(call keep,build/lint/tidy-configs.txt,$(TIDY_CONFIGS))
$(call keep,build/lint/files.txt,$(sort $(C_FILES) $(ASM_SOURCES)))
endif
ifneq ($(SHC_GOALS),)
$(shell mkdir -p $(sort $(dir $(C_OBJ) $(ASM_OBJ))))
$(foreach r,$(ROUTES),$(foreach s,$($r.sources),$(call keep,build/obj/$(s:.c=.cmd),$($r.release) $($r.flags) \
	$($r.includes) $(call unexpanded,$(COMPILE)) $(if $(filter $s,$(INLINE) $(PATCHED)),$(ASMFLAGS) \
	$(call unexpanded,$(ASSEMBLE)))$(if $(filter $s,$(PATCHED)), $($s.patches) $(call unexpanded,patch))$(if \
	$(filter $s,$(WARNED)), $($s.warnings)))))
$(foreach s,$(ASM_SOURCES),$(call keep,build/obj/$(s:.src=.cmd),R32 $(ASMFLAGS) \
	$(call unexpanded,assemble $(ASSEMBLE))))
endif

.SECONDEXPANSION:
$(LINKS): build/link/%.bin: $$(call object,$$*) build/link/%.rows tools/link.py tools/sysrof.py tools/config.py tools/tool_process.py \
		$(R32_ID)
	@$(call status,link,$@)
	$(Q)python3 tools/link.py $< $(call image,$*)
# An empty recipe lets a link's rows go missing without stopping make: the
# link, which depends on them, then runs again and writes them
$(LINKS:.bin=.rows): ;

# Build each image from its linked bytes, the disc's ranges and the linker's
# fill, which tools/splice.py checks against the image's original. A stored
# byte nothing writes fails the splice rather than fall back to the
# original's. Where fill crosses sections the splice reads the witnesses
# for the gap and any bounds the image records. Where a pool of filenames
# in C bounds the fill, the splice reads the pool from data.txt. Its count of
# each writer's bytes reads each source's offsets table through
# tools/body_data.py
SPLICE_TOOLS := tools/splice.py tools/body_data.py tools/body_bindings.py tools/csource.py tools/decls.py tools/guards.py \
	tools/inline.py tools/kinds.py tools/layout.py tools/split.py tools/sysrof.py tools/config.py
# The tables image $1's splice reads for its pools of filenames: data.txt,
# where a disc row in C starts on a word, and each layout data.txt places at
# such a row's start
pool_tables = $(shell awk '{sub(/\043.*/, "")} FNR == NR {if ($$1 == "disc" && $$2 == "C" && $$3 ~ /[048c]$$/) {start[$$3]; starts++}; next} \
    ($$1 in start) && $$2 == "layout" {print "layouts/" $$3 ".txt"} END {if (starts) print "config/$1/data.txt"}' config/$1/splits.txt config/$1/data.txt)
splice = python3 tools/splice.py $1 $< $@ $(if $(VERBOSE),--verbose)
# The tables image $1's splice reads where the image has them: its fill, and
# with it its bounds. Its whole link reads its names and its bounds
splice_tables = $(if $(wildcard config/$1/fill.txt),config/$1/fill.txt $(wildcard config/$1/bounds.txt))
relink_tables = $(wildcard config/$1/symbols.txt config/$1/bounds.txt)
# $(call spliced,NAME,FILE) gives the rules that rebuild image NAME's FILE
define spliced
build/$(2:.PRS=.BIN): $(call original,$2) $(filter build/link/$1/%,$(LINKS)) config/$1/splits.txt $(if $(filter $1,$(CONTENT_IMAGES)),$(call pool_tables,$1)) $(call splice_tables,$1) $(SPLICE_TOOLS)
	$$(Q)$$(call splice,$1)
endef
$(foreach i,$(IMAGES),$(eval $(call spliced,$(firstword $(subst :, ,$i)),$(lastword $(subst :, ,$i)))))

# Link each image's objects in one pass, apart from the splice, and compare
# the result with the spliced image, which the splicer has checked against
# the original. make relink runs it, and make doesn't. The objects come up
# to date with the spliced image, whose links read them
RELINK_TOOLS := tools/relink.py tools/link.py tools/tool_process.py $(SPLICE_TOOLS)
# $(call relinked,NAME,FILE) gives the rules that link image NAME's objects
# whole, and that run its shift test once that link matches the splice. The
# shift test links the image at its base again, and rewrites
# build/relink/NAME.bin before fixed.txt, so that fixed.txt stays the newer
define relinked
build/relink/$1.bin: build/$(2:.PRS=.BIN) $(call original,$2) config/$1/splits.txt $(call relink_tables,$1) \
		config/1st_read/symbols.txt $(RELINK_TOOLS) $(R32_ID)
	@$$(call status,relink,$$@)
	$$(Q)python3 tools/relink.py $1 $(firstword $(call original,$2)) build/$(2:.PRS=.BIN) $$(if $$(VERBOSE),--verbose)
build/relink/$1/fixed.txt: build/relink/$1.bin config/kept.txt tools/kept.py
	@$$(call status,shift,$$@)
	$$(Q)python3 tools/relink.py --shift $1 $(firstword $(call original,$2)) build/$(2:.PRS=.BIN) \
		$$(if $$(VERBOSE),--verbose)
endef
$(foreach i,$(IMAGES),$(eval $(call relinked,$(firstword $(subst :, ,$i)),$(lastword $(subst :, ,$i)))))

# A table that goes leaves no prerequisite newer than what read it. So
# $(call tabled,NAME,FILE) keeps the names of the tables image NAME's splice
# and whole link read, each in a stamp that step depends on, and removing
# one splices or links the image again
define tabled
$(call keep,build/tables/$1.splice,$(call splice_tables,$1))
$(call keep,build/tables/$1.relink,$(call relink_tables,$1))
build/$(2:.PRS=.BIN): build/tables/$1.splice
build/relink/$1.bin: build/tables/$1.relink
endef
ifneq ($(IMAGES),)
$(shell mkdir -p build/tables)
$(foreach i,$(IMAGES),$(eval $(call tabled,$(firstword $(subst :, ,$i)),$(lastword $(subst :, ,$i)))))
endif

build/relink/fixed.txt: $(FIXED)
	$(Q)cat $^ > $@
	@$(call status,fixed,$@)

build/orig/%.BIN: orig/%.PRS build/verified/%.PRS.ok build/prskit
	$(Q)mkdir -p $(@D)
	$(Q)build/prskit decompress $< $@

build/%.PRS: build/%.BIN build/prskit
	$(Q)build/prskit compress $< $@

# Check an original file against SHA256SUMS before anything reads it, so a
# wrong or partial one is named as such
.PRECIOUS: build/verified/%.ok
build/verified/%.ok: orig/% SHA256SUMS
	$(Q)mkdir -p $(@D)
	$(Q)awk '$$2 == "$<"' SHA256SUMS | grep -q . || (echo '$<: not in SHA256SUMS'; false)
	$(Q)awk '$$2 == "$<"' SHA256SUMS | sha256sum -c --quiet - || (echo '$<: not the original file; replace it'; false)
	$(Q)touch $@

# Take the original files from orig/, extracting those it lacks from a GDI
# image there. dc.mk takes the Chao archives from the same image
GDI_IMAGE := $(or $(GDI),$(shell find orig -maxdepth 1 -iname '*.gdi'))
# Count the words that open with orig/: a name can hold spaces, but no slash
FOUND := $(if $(GDI),1,$(words $(filter orig/%,$(GDI_IMAGE))))
MISSING := $(filter-out $(wildcard $(ORIG)),$(ORIG))
ifneq ($(MISSING),)
ifneq ($(strip $(DISC_FILES)),)
# Only materialize the original files requested by the build, as unpack
# does. With DISC_FILES, make unpack copies them and builds nothing
$(MISSING):
	$(Q)mkdir -p $(@D)
	$(Q)awk -v path='$@' '$$2 == path' SHA256SUMS | grep -q . || (echo '$@: not in SHA256SUMS'; false)
	$(Q)cp $(call quoted,$(DISC_FILES)/$(@:orig/%=%)) $@.part
	$(Q)awk -v path='$@' '$$2 == path { print $$1 "  " path ".part" }' SHA256SUMS | sha256sum -c --quiet -
	$(Q)mv $@.part $@
unpack: $(MISSING)
else
ifeq ($(FOUND),0)
# The stop for a missing disc ends with a goal that needs none: make
# dc-check for a port goal, or on macOS, which refuses make check. make
# unpack stops the same way: it has no .gdi to extract from
define ORIG_HELP
orig/ lacks $(words $(MISSING)) of the $(words $(ORIG)) files the build needs, $(firstword $(MISSING:orig/%=%)) among them.
  Put them in orig/ as the disc names them, or put the disc's .gdi and its tracks there.
  Without the disc, $(if $(MACOS)$(DC_GOALS),make dc-check builds and sums the port's objects,make check checks the linked sources).
endef
$(MISSING) unpack:
	$(info $(ORIG_HELP))
	$(error no disc in orig/)
else ifneq ($(FOUND),1)
$(MISSING) unpack:
	$(error orig/ has $(FOUND) .gdi files, and make takes one: $(GDI_IMAGE))
else
# Check every extracted file against SHA256SUMS before moving any into orig/,
# so a failed check leaves orig/ as it was
$(MISSING): unpack
unpack: build/gdi
	@$(call status,gdi,$(GDI_IMAGE))
	$(Q)rm -rf build/unpack
	$(Q)build/gdi extract $(call quoted,$(GDI_IMAGE)) build/unpack $(MISSING:orig/%=%)
	$(Q)for f in $(MISSING:orig/%=%); do \
		rows=$$(awk -v f="orig/$$f" '$$2 == f { print $$1 "  build/unpack/" substr(f, 6) }' SHA256SUMS); \
		[ -n "$$rows" ] || { echo "orig/$$f: not in SHA256SUMS"; exit 1; }; \
		echo "$$rows" | sha256sum -c --quiet --status - || { echo $(call quoted,$(GDI_IMAGE))": its $$f isn't the original"; exit 1; }; \
	done
	$(Q)for f in $(MISSING:orig/%=%); do \
		mkdir -p "$$(dirname "orig/$$f")" && mv "build/unpack/$$f" "orig/$$f" || exit 1; \
	done
endif
# Building the images, say what orig/ lacks before any compile
ifneq ($(filter all relink shift,$(GOALS)),)
$(C_OBJ) $(ASM_OBJ): | $(MISSING)
endif
endif
endif

# make source builds SOURCE's object and its link in each image that places
# it. With the disc, it counts in each the words of SOURCE's functions that
# differ from the original's, as a hack is measured before it takes its tag.
# tools/words.py exits 3 where a function differs, and make source goes on
# past it. Any other failure stops make source. Then it checks each link
# against its line in SHA256SUMS, and fails on a link without one, which
# tools/sums.py --add gives it
## SOURCE=path: the source make source builds and checks
ifneq ($(filter source,$(GOALS)),)
# SOURCE must be one of the tree's sources, each of which a splits.txt
# places. A link is named by its source's path less the extension, so the
# links alone would take src/a.typo for src/a.c
ifneq ($(words $(SOURCE)) $(filter $(SOURCE),$(C_SOURCES) $(ASM_SOURCES)),1 $(SOURCE))
$(error SOURCE=$(SOURCE): make source takes a source that a splits.txt places)
endif
SOURCE_LINKS  := $(filter $(foreach i,$(IMAGES),build/link/$(firstword $(subst :, ,$i))/$(basename $(SOURCE)).bin), \
	$(LINKS))
SOURCE_IMAGES := $(foreach l,$(SOURCE_LINKS),$(call image,$(l:build/link/%.bin=%)))
SOURCE_FILES  := $(foreach i,$(SOURCE_IMAGES),$(lastword $(subst :, ,$(filter $i:%,$(IMAGES)))))
# What orig/ lacks, a .gdi there gives, as for make
SOURCE_LACKS  := $(if $(or $(DISC_FILES),$(filter 1,$(FOUND))),,$(filter $(MISSING),$(addprefix orig/,$(SOURCE_FILES))))
SOURCE_LISTED := $(filter $(SOURCE_LINKS),$(shell awk '{ print $$2 }' SHA256SUMS))
SOURCE_NEW    := $(filter-out $(SOURCE_LISTED),$(SOURCE_LINKS))
endif
## build and check SOURCE: its links, its words and its hashes
source: $(SOURCE_LINKS) $(if $(SOURCE_LACKS),,$(foreach f,$(SOURCE_FILES),$(call original,$f)))
	$(Q)$(if $(SOURCE_LACKS),echo 'make source counts no words: orig/ lacks $(firstword $(SOURCE_LACKS:orig/%=%))', \
		for i in $(SOURCE_IMAGES); do python3 tools/words.py $$i $(SOURCE) || [ $$? = 3 ] || exit 1; done)
	$(Q)$(if $(SOURCE_LISTED),printf '%s\n' $(SOURCE_LISTED) | \
		awk 'NR == FNR { listed[$$1]; next } $$2 in listed' - SHA256SUMS | sha256sum -c --quiet -)
	@$(call status,sums,$(words $(SOURCE_LISTED)) of $(words $(SOURCE_LINKS)) links match SHA256SUMS)
	$(Q)$(if $(SOURCE_NEW),printf '%s: no line in SHA256SUMS; tools/sums.py --add gives it one\n' $(SOURCE_NEW); false)

# The host tools vendored under tools/, as tools/vendor.py's TOOLS names
# them. vendor.py --check reads each one's directory, so the directories are
# prerequisites too: adding or removing a file reruns it
VENDORED := gdi prskit shckit
build/tools/vendored.ok: tools/SHA256SUMS $(VENDORED:%=tools/%) $(wildcard $(VENDORED:%=tools/%/*)) tools/vendor.py tools/tool_process.py
	$(Q)python3 tools/vendor.py --check
	$(Q)mkdir -p $(@D) && touch $@

# CC builds the vendored tools, and writes each C object's and lint target's
# .d file in its recipe, so each of them depends on CC's stamp. The stamp
# holds CC's command, not a hash of the compiler: another compiler or another
# flag in CC makes each again, and a new release of the host's cc doesn't.
# make writes the stamp for each goal that may build one of them. A tree
# without the stamp is taken to have been built with this CC, so make dates
# a new stamp the epoch, and its first write rebuilds nothing
CC_ID := build/tools/cc.id
ifneq ($(filter-out accept clean format help review stub-includes test,$(GOALS)),)
ifneq ($(wildcard $(CC_ID)),)
$(call keep,$(CC_ID),$(CC))
else
$(shell mkdir -p $(dir $(CC_ID)))$(file >$(CC_ID),$(CC))$(shell TZ=UTC0 touch -t 197001010000 $(CC_ID))
endif
endif
$(C_OBJ) $(STUB_OBJ) $(TIDIED): $(CC_ID)

# The vendored tools build with their repositories' flags, once the check passes
HOSTFLAGS := -std=c99 -Wall -Wextra -Wpedantic -O2
$(VENDORED:%=build/%): build/%: tools/%/main.c $$(wildcard tools/$$*/*) Makefile $(CC_ID) | build/tools/vendored.ok
	@$(call status,cc,$@)
	$(Q)mkdir -p $(@D)
	$(Q)$(CC) $(HOSTFLAGS) -o $@ tools/$*/$*.c $<

# $(call refresh,NAME,FILES) writes sha256sum's lines for FILES to NAME's
# stamp unless it holds them already, so an unchanged tool in the same place
# rebuilds nothing. A glob in FILES sorts in the C locale, so a change of
# locale leaves a stamp alone
refresh = LC_ALL=C; export LC_ALL; mkdir -p build/tools && id="$$(sha256sum $2)" && \
	{ [ "$$(cat build/tools/$1.id 2>/dev/null)" = "$$id" ] || echo "$$id" > build/tools/$1.id; }
# $(call stamp,NAME,TOOL,FILES,GOALS) makes NAME's stamp of TOOL from its
# FILES. GOALS are this run's goals that may use TOOL, and for none make
# leaves the stamp alone. For any, make refreshes it as it reads the
# Makefile, so make -n shows what a changed tool rebuilds. A missing TOOL
# stops make, unless make fetches or builds it under tools/: then make
# refreshes the stamp once TOOL is in place
define stamp
build/tools/$1.id: | $(filter tools/%,$2)
	$$(Q)$$(call refresh,$1,$3)
ifneq ($4,)
ifneq ($$(wildcard $2),)
$$(shell $$(call refresh,$1,$3))
ifneq ($$(.SHELLSTATUS),0)
$$(error build/tools/$1.id: can't refresh it from $3)
endif
else ifneq ($(filter tools/%,$2),)
build/tools/$1.id: FORCE
else
$$(error $2: no such file)
endif
endif
endef
ifeq ($(MACOS),)
$(eval $(call stamp,wibo,$(WIBO),$(WIBO),$(SHC_GOALS)))
$(foreach r,$(RELEASES),$(eval $(call stamp,$r,$($r)/shc.exe,$($r)/*.*,$(SHC_GOALS))))
else
# macOS hashes none of SHC's tools. Each rule that runs one depends on the
# stamps of wibo and the tool's release, so a goal that comes to reach one
# stops make here, under make -n too, and the stop names the stamp
$(WIBO_ID) $(RELEASES:%=build/tools/%.id): FORCE
	$(error $(call no_shc,$@))
endif

# The largest pinned download is the GCC source archive. Give a progressing transfer
# an hour by default, but stop a stalled connection or body much sooner.
## FETCH_TIMEOUT=secs: total time allowed for one pinned download
FETCH_TIMEOUT ?= 3600
# Download $1 to $2 only if one line of tools/SHA256SUMS names $2 itself, and
# move it into place only once its hash matches that line's
fetch = sum=$$(awk '$$2 == "$2" { n++; sum = $$1 } END { if (n == 1) print sum }' tools/SHA256SUMS) && \
	{ [ -n "$$sum" ] || { echo '$2: not in tools/SHA256SUMS'; false; }; } && \
	(trap 'rm -f $2.part' EXIT; curl -fsSL --connect-timeout 30 --speed-limit 1 --speed-time 120 \
		--max-time $(FETCH_TIMEOUT) -o $2.part $1 && echo "$$sum  $2.part" | sha256sum -c --quiet - && mv $2.part $2)

# pip installs the wheels tools/SHA256SUMS lists, and no other package. It is
# forced, since it leaves alone a wheel it finds installed, even one whose
# tool was deleted. INSTALLED is removed just before pip and written once both
# tools are in place, so an install cut short is made again. make 4.3 deletes
# a grouped target whose recipe fails, so the tools are precious. A host with
# no WHEEL has no wheels to install, and the recipe stops make there, under
# make -n too, whatever goal reaches it. files.pythonhosted.org redirects
# packages/py2.py3/c/PROJECT/FILE to the path where it stores FILE, so one
# form of URL fetches each host's wheels
.PRECIOUS: $(CLANG_TOOLS)
$(CLANG_TOOLS) &: $(if $(UNINSTALLED),FORCE)
	$(if $(WHEEL),,$(error $(no_wheel)))
	@$(call status,fetch,$(CLANG_FORMAT_WHEEL))
	$(Q)mkdir -p tools/llvm && $(call fetch,https://files.pythonhosted.org/packages/py2.py3/c/clang-format/$(notdir $(CLANG_FORMAT_WHEEL)),$(CLANG_FORMAT_WHEEL))
	@$(call status,fetch,$(CLANG_TIDY_WHEEL))
	$(Q)$(call fetch,https://files.pythonhosted.org/packages/py2.py3/c/clang-tidy/$(notdir $(CLANG_TIDY_WHEEL)),$(CLANG_TIDY_WHEEL))
	@$(call status,venv,tools/llvm)
	$(Q)python3 -m venv tools/llvm
	$(Q)rm -f $(INSTALLED)
	$(Q)$(LLVM)/pip install -q --no-index --no-deps --force-reinstall $(CLANG_FORMAT_WHEEL) $(CLANG_TIDY_WHEEL)
	$(Q)rm $(CLANG_FORMAT_WHEEL) $(CLANG_TIDY_WHEEL)
	$(Q)for t in $(notdir $(CLANG_TOOLS)); do [ -x $(LLVM)/$$t ] || { echo "$(LLVM): no $$t"; exit 1; }; done
	$(Q)$(wheel_sums) > $(INSTALLED)

# the include directories of lint's scan for a source's headers:
# compile_flags.txt's, then the root, which make dc searches last too
TIDY_INC := $(filter -I%,$(COMPILE_FLAGS)) -I.
# What compile_flags.txt has clang-tidy read ahead of each source: SHC's
# intrinsics, declared for it
TIDY_PRELUDE := config/intrinsics.h config/intrinsics_machine.h

# The files format.ok checks: every C file when .clang-format or clang-format
# changed, else the C files edited since its last pass
formatted = $(if $(filter-out %.c %.h,$?),$(FORMATTED),$(filter %.c %.h,$?))
build/lint/format.ok: $(FORMATTED) .clang-format $(LLVM)/clang-format
	@$(call status,format,$(words $(formatted)) of $(words $(FORMATTED)) files)
	$(Q)$(LLVM)/clang-format --dry-run -Werror $(formatted)
	$(Q)mkdir -p $(@D) && touch $@

# $(call vfs,PATH,FILE) is the JSON that --vfsoverlay reads, under which
# clang reads FILE's text at PATH and names PATH in a diagnostic
vfs = {"version": 0, "use-external-names": false, \
	"roots": [{"name": "$1", "type": "file", "external-contents": "$2"}]}
# include-cleaner reads a source whose bodies lie under TIDY_BODY_ROOT as
# body_data.py --lint writes it (with_bodies). The overlay puts that copy
# at the source's path, so the source's quoted includes resolve from its
# own directory, and a warning names the source's path and line
tidy_includes = python3 tools/body_data.py $(TIDY_BODY_ROOT) $< $(@:.tidy=) --lint && \
	printf '%s\n' '$(call vfs,$<,$(@:.tidy=))' > $(@:.tidy=.vfs) && \
	$(LLVM)/clang-tidy --quiet --checks='-*,misc-include-cleaner' --vfsoverlay=$(@:.tidy=.vfs) $< \
	    --extra-arg-before=-I$(TIDY_BODY_ROOT)
# The scan and clang-tidy search TIDY_BODY_ROOT ahead of compile_flags.txt's
# directories: a variant's bodies take its provider's names, under which
# build/stub/ may hold the provider's own bodies. SHC searches a body root
# after its route's directories, among sdk/include, src and modules, and the
# two orders find the same files while no generated include shares its name
# with a file under those
$(TIDIED): build/lint/%.tidy: % compile_flags.txt $(TIDY_PRELUDE) $(TIDY_CONFIGS) build/lint/tidy-configs.txt $(LLVM)/clang-tidy
	@$(call status,tidy,$<)
	$(Q)mkdir -p $(@D)
	$(Q)$(CC) -MM -MP -nostdinc $(addprefix -I,$(TIDY_BODY_ROOT)) $(TIDY_INC) -MT $@ -MF $(@:.tidy=.d).tmp $<
	$(Q)mv $(@:.tidy=.d).tmp $(@:.tidy=.d)
	$(Q)$(LLVM)/clang-tidy --quiet $< $(if $(TIDY_BODY_ROOT),--extra-arg-before=-I$(TIDY_BODY_ROOT) --checks=-misc-include-cleaner)
	$(if $(TIDY_BODY_ROOT),$(Q)$(tidy_includes))
	$(Q)touch $@
# A lint target's .d file keeps its header dependencies, as an object's does
ifneq ($(filter lint build/lint/%,$(MAKECMDGOALS)),)
$(TIDIED): %.tidy: %.d
$(TIDIED:.tidy=.d): ;
-include $(TIDIED:.tidy=.d)
endif

build/lint/decls.ok: $(C_FILES) tools/decls.py tools/split.py tools/csource.py tools/inline.py tools/guards.py \
		tools/config.py tools/layout.py
	@$(call status,decls,$(words $(C_FILES)) files)
	$(Q)python3 tools/decls.py $(C_FILES)
	$(Q)mkdir -p $(@D) && touch $@

build/lint/guards.ok: $(C_FILES) tools/guards.py tools/csource.py tools/inline.py tools/config.py tools/layout.py
	@$(call status,guards,$(words $(C_FILES)) files)
	$(Q)python3 tools/guards.py $(C_FILES)
	$(Q)mkdir -p $(@D) && touch $@

# tools/tags.py reads every image's tables for the addresses a tag cites
build/lint/tags.ok: $(C_FILES) $(ASM_SOURCES) $(wildcard config/*/splits.txt config/*/symbols.txt) \
		tools/tags.py tools/inline.py tools/split.py tools/csource.py tools/guards.py tools/config.py tools/layout.py
	@$(call status,tags,$(words $(C_FILES) $(ASM_SOURCES)) files)
	$(Q)python3 tools/tags.py $(C_FILES) $(ASM_SOURCES)
	$(Q)mkdir -p $(@D) && touch $@

# shckit holds each assembly source to its check and to its lint
build/lint/asm.ok: $(ASM_SOURCES) build/shckit
	@$(call status,shckit,$(words $(ASM_SOURCES)) files)
	$(Q)build/shckit check $(ASM_SOURCES)
	$(Q)build/shckit lint $(ASM_SOURCES)
	$(Q)mkdir -p $(@D) && touch $@

# A cached check depends too on the recipe and options this file gives it,
# and a check of the whole tree on the names of the files it reads. A change
# of those names gives format.ok every file to check
build/lint/format.ok build/lint/decls.ok build/lint/guards.ok build/lint/tags.ok build/lint/asm.ok $(TIDIED): Makefile
build/lint/format.ok build/lint/decls.ok build/lint/guards.ok build/lint/tags.ok build/lint/asm.ok: build/lint/files.txt

ifeq ($(MACOS),)
tools/wibo-i686:
	@$(call status,fetch,$@)
	$(Q)$(call fetch,https://github.com/decompals/wibo/releases/download/1.2.0/wibo-i686,$@)
	$(Q)chmod +x $@

# Unpack beside the release, and put it in place only once whole. Its file
# names go to lowercase: the archives hold LNK.EXE and LBR.EXE
tools/shc-%/shc.exe:
	@$(call status,fetch,tools/shc-$*.tar.gz)
	$(Q)$(call fetch,https://github.com/decompme/compilers/releases/download/compilers/shc-$*.tar.gz,tools/shc-$*.tar.gz)
	@$(call status,tar,$(@D))
	$(Q)rm -rf $(@D).part && mkdir -p $(@D).part
	$(Q)tar -xzf tools/shc-$*.tar.gz -C $(@D).part --strip-components=1
	$(Q)cd $(@D).part && for f in *; do l=$$(echo "$$f" | tr A-Z a-z); [ "$$f" = "$$l" ] || mv "$$f" "$$l"; done
	$(Q)[ -f $(@D).part/shc.exe ] || { echo "$(@D): no shc.exe"; exit 1; }
	$(Q)rm -rf $(@D) && mv $(@D).part $(@D)
	$(Q)rm tools/shc-$*.tar.gz
else
# macOS fetches neither wibo nor a release of SHC's, and a goal that names
# one stops here, as at a stamp
tools/wibo-i686: FORCE
	$(error $(call no_shc,$@))
tools/shc-%/shc.exe: FORCE
	$(error $(call no_shc,$@))
endif

# dc.mk reads the lists, macros and stamps above, and gives make dc and
# dc-check their prerequisites and recipes. make help reads no dc.mk, so
# their lines stand here
ifneq ($(DC_MK_GOALS),)
include dc.mk
endif
## build GCC's objects and censuses; check the Chao archives
dc:

## sum make dc's objects and censuses; fail on an unlisted name
dc-check:

## link executable SH-4 images against the resident game
dc-images:

## pack the linked programs and write build/dc/manifest.txt
dc-files:

## build a GDI of the port in build/dc/disc/
dc-disc:

# HEAD's commit, then a line for each path where the tree differs from it.
# make's scans read untracked files too, so --untracked-files=normal lists
# them even where git's configuration sets status.showUntrackedFiles=no
ACCEPT_STATE = git rev-parse --verify HEAD && git status --porcelain --untracked-files=normal
# Each gate is a run of this make's executable with no flag but accept's -j.
# make hands a recipe its flags in MAKEFLAGS and MFLAGS and its depth in
# MAKELEVEL, and exports each variable its command line gives, so accept
# unsets them all for each gate. Each gate that uses SH_ELF's toolchain,
# dc-check to dc-disc and test, takes SH_ELF on its command line. test runs
# last, once the dc gates have built or found that toolchain and filled
# tools/sh4zam with the files tools/SHA256SUMS pins. test also takes
# SH4ZAM=tools/sh4zam: it checks no hash of the headers it reads, so the
# environment's SH4ZAM could give it other headers than the dc gates used
ACCEPT_MAKE = $(strip $(MAKE) $(filter -j%,$(MAKEFLAGS)))
ACCEPT_GATES = lint all check $(foreach g,dc-check dc dc-images dc-files dc-disc,$(call quoted,$g SH_ELF=$(SH_ELF))) \
	$(call quoted,test SH_ELF=$(SH_ELF) SH4ZAM=tools/sh4zam)
ACCEPT_UNSET = MAKEFLAGS MFLAGS MAKELEVEL $(foreach v,$(.VARIABLES),$(if $(filter command line,$(origin $v)),$v))
# The disc a gate reads is accept's, from its command line or its
# environment, so each gate takes GDI and DISC_FILES in its environment
ACCEPT_DISC = $(foreach v,GDI DISC_FILES,$(if $($v),$v=$(call quoted,$($v))))
# What the record sums once the gates pass: the manifest, which holds the
# hashes of each image's ELF, its decoded bytes and its packed file, and each
# file of the port's disc
ACCEPT_SUMS = build/dc/manifest.txt build/dc/disc/*
# accept writes its record to build/accept.txt: HEAD's commit, then each
# gate's status and command as the gate ends. Once every gate has passed, it
# adds sha256sum's line for each of ACCEPT_SUMS, then accepted. The record
# says accepted only where HEAD was the commit it names and the tree matched
# that commit, before the gates and after them. So sha256sum -c on the sum
# lines ties the port's programs and its disc to a commit whose gates all
# passed
## run every gate, lint to dc-disc and test, at HEAD
accept:
	$(Q)rm -f build/accept.txt && mkdir -p build && state=$$($(ACCEPT_STATE)) || exit 1; \
	commit=$$(echo "$$state" | head -n 1); echo "commit   $$commit" > build/accept.txt || exit 1; \
	if [ "$$state" != "$$commit" ]; then echo "$$state" | sed '1d; s/^/changed  /' | tee -a build/accept.txt; \
		echo "build/accept.txt: the tree differs from $$commit"; exit 1; fi; \
	for gate in $(ACCEPT_GATES); do \
		printf '  %-6s %s\n' gate "$(ACCEPT_MAKE) $$gate"; \
		env $(addprefix -u ,$(ACCEPT_UNSET)) $(ACCEPT_DISC) $(ACCEPT_MAKE) $$gate; status=$$?; \
		echo "gate     $$status  $(ACCEPT_MAKE) $$gate" >> build/accept.txt || exit 1; \
		[ $$status = 0 ] || exit $$status; \
	done; \
	state=$$($(ACCEPT_STATE)) || exit 1; [ "$$state" = "$$commit" ] || \
		{ echo 'changed  HEAD or the tree, as the gates ran' | tee -a build/accept.txt; exit 1; }; \
	sums=$$(sha256sum $(ACCEPT_SUMS)) || exit 1; echo "$$sums" | sed 's/^/sum      /' >> build/accept.txt && \
	echo accepted >> build/accept.txt && cat build/accept.txt

.PHONY: all check relink shift offsets test progress lint format review stub-includes source clean help unpack dc dc-check dc-images dc-files dc-disc accept FORCE
.DELETE_ON_ERROR:

ifneq ($(stamp_batch),)
$(shell $(python) tools/keep_stamps.py $(stamp_batch))
ifneq ($(.SHELLSTATUS),0)
$(error could not update build stamps)
endif
endif
