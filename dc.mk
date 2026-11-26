# make dc's rules: GCC's objects, each image's census, make dc-check's
# sums, and the toolchain and SH4ZAM's files. The Makefile reads this file
# for DC_MK_GOALS alone, and no SHC object depends on it

# PINNED_GCC is the toolchain's driver where make builds the toolchain:
# where SH_ELF is tools/sh-elf, its default. SH4ZAM_FILES is the files
# tools/SHA256SUMS lists under tools/sh4zam/, which make fetches. FETCHED
# holds their lines as of the last fetch that finished. Where they aren't
# the lines tools/SHA256SUMS gives now, every file of SH4ZAM's is UNFETCHED:
# make fetches every file again, and removes any other file under
# tools/sh4zam/, before GCC compiles any C, so that none stays from another
# pin. For make dc and dc-check, DC_DOWNLOADS takes each of these files that
# is missing or UNFETCHED
PINNED_GCC := $(filter tools/sh-elf/bin/sh-elf-gcc,$(SH_ELF)/bin/sh-elf-gcc)
sh4zam_sums = awk '$$2 ~ "^tools/sh4zam/"' tools/SHA256SUMS
SH4ZAM_FILES := $(shell awk '$$2 ~ "^tools/sh4zam/" { print $$2 }' tools/SHA256SUMS)
FETCHED    := tools/sh4zam/fetched.txt
ifneq ($(strip $(file <$(FETCHED))),$(strip $(shell $(sh4zam_sums))))
UNFETCHED  := $(SH4ZAM_FILES)
endif
DC_DOWNLOADS := $(filter-out $(wildcard $(PINNED_GCC) $(filter-out $(UNFETCHED),$(SH4ZAM_FILES))), \
	$(if $(DC_GOALS),$(PINNED_GCC) $(SH4ZAM_FILES)))

# Stop before a goal runs on a host that lacks what it needs, and name what
# it lacks, as the Makefile does for the matching build. make dc and
# dc-check need the C compiler, which builds build/shckit, and sha256sum.
# A download needs curl, but not one of SH4ZAM's files where SH4ZAM may
# give it: its fetch needs curl only where SH4ZAM's copy fails. The
# toolchain's sources unpack with tar, one that gzip compressed and four
# that xz did, and build with c++. tar unpacks a format through its library
# where it was built with one, as libarchive's tar is with zlib and
# liblzma, and through the format's program where it wasn't, as GNU tar
# runs gzip and xz. tar --version names each library tar was built with,
# as zlib/1.2.12, so $(call unpacker,LIBRARY,PROGRAM) is PROGRAM where it
# names no LIBRARY
unpacker = $(if $(findstring $1,$(shell tar --version 2> /dev/null)),,$2)
DC_NEEDS := $(if $(DC_GOALS),$(firstword $(CC)) sha256sum) \
	$(if $(filter-out $(if $(SH4ZAM),$(SH4ZAM_FILES)),$(DC_DOWNLOADS)),curl) \
	$(if $(filter %/sh-elf-gcc,$(DC_DOWNLOADS)),tar $(call unpacker,zlib,gzip) $(call unpacker,liblzma,xz) c++)
DC_LACKS := $(shell for c in $(DC_NEEDS); do command -v $$c > /dev/null || echo $$c; done)
ifneq ($(DC_LACKS),)
$(error $(DC_LACKS): not found; see README.md#build)
endif

# make reads config/gcc.mk, GCC's flags, and config/dreamcast.mk, the
# port's composition, for DC_GOALS alone, so a fault in either stops only a
# run that names one of them. For a goal under tools/sh-elf/ or
# tools/sh4zam/, make reads dc.mk and neither of them
ifneq ($(DC_GOALS),)
include config/gcc.mk config/dreamcast.mk
endif
# DC_IMAGES is each image config/ describes, and DC_MAIN the main
# executable's, which stays in memory beside every other
DC_IMAGES := $(foreach i,$(IMAGES),$(firstword $(subst :, ,$i)))
DC_MAIN := $(call from_config,[config.MAIN])

# The toolchain make builds has a receipt, request.txt, which its recipe
# writes last: the sources, their lines of tools/SHA256SUMS, the host and
# each configure's flags. SH_ELF_REQUEST is the receipt this file's recipe
# writes. The tool stamps hold what the binaries are, and the receipt what
# they were built from and on which host, so make refuses a toolchain whose
# receipt differs, as one copied from a host uname names otherwise does, and
# leaves it in place. It refuses before it refreshes a stamp. A toolchain
# another recipe built can lack a file the stamps hash, as one built without
# link-time optimization lacks lto1 and the linker's plugin, and the refresh
# would stop make on that file and name no remedy
SH_ELF_SOURCES := binutils/binutils-2.47.tar.xz gcc/gcc-16.2.0/gcc-16.2.0.tar.xz gmp/gmp-6.3.0.tar.xz \
	mpc/mpc-1.3.1.tar.gz mpfr/mpfr-4.2.1.tar.xz
# Each configure links the host's zstd where it finds one, and --without-zstd
# has it look for none, so the toolchain's programs need no library the host
# may lack. --enable-deterministic-archives has ar write each member's date,
# owner and group as zero and its mode as 644 unless a command says
# otherwise, and GCC's build archives libgcc.a with that ar, so libgcc.a
# holds no time or user of its build. make dc's C objects carry GCC's
# intermediate code, which each image's census compiles, as config/gcc.mk's
# GCC_LTO says: --enable-lto has GCC build what writes and compiles that
# code, lto1 and the linker's plugin among it, and --enable-plugins has the
# linker load the plugin, and fails binutils' configure on a host where it
# can't
SH_ELF_BINUTILS := --target=sh-elf --disable-nls --disable-werror --without-zstd --enable-deterministic-archives \
	--enable-plugins
SH_ELF_GCC := --target=sh-elf --enable-languages=c --with-newlib --without-headers --with-endian=little \
	--with-cpu=m4-single-only --disable-multilib --disable-nls --disable-shared --disable-threads --enable-lto \
	--without-isl --without-zstd --disable-libssp --disable-libquadmath --disable-libgomp
# GCC compiles the target's libraries with debugging information, which names
# the directory of each compile. $(call sh_elf_map,CHECKOUT) is the flag that
# has it compile them with -fdebug-prefix-map=CHECKOUT=., which writes . there
# wherever a path opens with CHECKOUT. The configure takes the flag with the
# checkout's path, so libgcc.a holds no path of the host, and is the same
# file wherever the checkout lies. The receipt takes it with the word CURDIR:
# it holds each flag that shapes libgcc.a, and is one text in every checkout
# of a host
sh_elf_map = --with-debug-prefix-map=$1=.
SH_ELF_REQUEST := $(strip $(SH_ELF_SOURCES) $(shell awk '$$2 ~ "^tools/sh-elf.src/"' tools/SHA256SUMS) \
	host $(HOST) binutils $(SH_ELF_BINUTILS) gcc $(SH_ELF_GCC) $(call sh_elf_map,CURDIR))
ifneq ($(wildcard $(PINNED_GCC)),)
ifneq ($(strip $(file <tools/sh-elf/request.txt)),$(SH_ELF_REQUEST))
$(error tools/sh-elf: its request.txt names another build than dc.mk's; remove tools/sh-elf, and make builds it again)
endif
endif

# GCC's stamp holds the hashes of its driver, its C compiler, cc1, and its
# assembler
GCC_ID := build/tools/gcc.id
$(eval $(call stamp,gcc,$(SH_ELF)/bin/sh-elf-gcc,$(SH_ELF)/bin/sh-elf-gcc $(SH_ELF)/libexec/gcc/sh-elf/*/cc1 \
	$(SH_ELF)/bin/sh-elf-as,$(DC_GOALS)))
# The linker, the archiver, objcopy and libgcc.a take a stamp of their own.
# So do LINK_TIME_TOOLS, the programs of GCC's that only the census's link
# runs, for its link-time compile: lto1, which compiles the intermediate
# code make dc's C objects carry, lto-wrapper and collect2, through which the
# driver runs lto1 and the linker, and the plugin by which the linker hands
# GCC that code. A change to one runs the archive and each census's link
# again, and make dc-check's sums. GCC's version is part of the paths of cc1, of
# LINK_TIME_TOOLS and of libgcc.a, so the stamps name them by a shell glob,
# and LIBGCC is libgcc.a's. A refresh expands the glob as it hashes, once
# the toolchain is in place, and fails where it matches no file. A
# toolchain built without link-time optimization holds no lto1 and no
# plugin. Where SH_ELF's driver is in place and a program of LINK_TIME_TOOLS
# is not, make stops ahead of the refresh, and names each such program and
# what settles it: for SH_ELF, a GCC configured with --enable-lto, and for
# tools/sh-elf, whose receipt is this file's and which has lost the program
# since, its removal, so that make builds it again
LD_ID  := build/tools/ld.id
LIBGCC := $(SH_ELF)/lib/gcc/sh-elf/*/libgcc.a
LINK_TIME_TOOLS := lto1 lto-wrapper collect2 liblto_plugin.so
ifneq ($(DC_GOALS),)
ifneq ($(wildcard $(SH_ELF)/bin/sh-elf-gcc),)
LINK_TIME_MISSING := $(strip $(foreach t,$(LINK_TIME_TOOLS),$(if $(wildcard $(SH_ELF)/libexec/gcc/sh-elf/*/$t),,$t)))
ifneq ($(LINK_TIME_MISSING),)
$(error $(SH_ELF): its libexec/gcc/sh-elf/*/ holds no $(LINK_TIME_MISSING), which make dc's link-time compile runs; $(if \
	$(PINNED_GCC),remove tools/sh-elf$(comma) and make builds it again,SH_ELF must name a toolchain whose GCC was \
	configured with --enable-lto))
endif
# GCC's version is part of the paths of its own headers, of cc1 and of
# libgcc.a, which the rules name by a glob that takes one version
ifneq ($(word 2,$(wildcard $(SH_ELF)/lib/gcc/sh-elf/*/include)),)
$(error $(SH_ELF): its lib/gcc/sh-elf/ holds more than one version of GCC, $(patsubst \
	$(SH_ELF)/lib/gcc/sh-elf/%/include,%,$(wildcard $(SH_ELF)/lib/gcc/sh-elf/*/include)), and make dc takes a \
	toolchain of one)
endif
endif
endif
$(eval $(call stamp,ld,$(SH_ELF)/bin/sh-elf-gcc,$(SH_ELF)/bin/sh-elf-ld $(SH_ELF)/bin/sh-elf-ar \
	$(SH_ELF)/bin/sh-elf-objcopy $(LIBGCC) $(LINK_TIME_TOOLS:%=$(SH_ELF)/libexec/gcc/sh-elf/*/%),$(DC_GOALS)))

# make dc's objects lie under build/dc/, each at its source's path: one for
# each C source of each route, which GCC compiles, and one for each .src
# file, which GNU as assembles. config/dreamcast.mk composes the rest. An
# entry P:C of DC_REPLACED compiles C to build/dc/P.o, the object P would
# compile or assemble to, so C replaces P. An entry IMAGE:C of DC_ADDED
# compiles C to build/dc/C.o, which IMAGE's census links after its sources'.
# A C that two images add compiles once, and both censuses take its object
DC_REPLACED_SRC := $(foreach r,$(DC_REPLACED),$(word 1,$(subst :, ,$r)))
DC_REPLACED_OBJ := $(addsuffix .o,$(addprefix build/dc/,$(basename $(DC_REPLACED_SRC))))
DC_ADDED_C := $(sort $(foreach a,$(DC_ADDED),$(word 2,$(subst :, ,$a))))
DC_ADDED_OBJ := $(DC_ADDED_C:%.c=build/dc/%.o)
DC_PAIRED_OBJ := $(DC_REPLACED_OBJ) $(DC_ADDED_OBJ)
# each object of DC_PAIRED_OBJ and its C, as OBJECT:C
DC_PAIRS := $(join $(DC_REPLACED_OBJ),$(patsubst %,:%,$(foreach r,$(DC_REPLACED),$(word 2,$(subst :, ,$r))))) \
	$(join $(DC_ADDED_OBJ),$(DC_ADDED_C:%=:%))
# DC_C_OBJ is the objects GCC compiles from a source's C, DC_INLINE_OBJ
# those it compiles from the C split from a source's inline bodies, and
# DC_ASM_OBJ those GNU as assembles, but each a replacement takes
DC_C_OBJ := $(filter-out $(DC_REPLACED_OBJ),$(patsubst %.c,build/dc/%.o,$(filter-out $(INLINE),$(C_SOURCES))))
DC_INLINE_OBJ := $(filter-out $(DC_REPLACED_OBJ),$(patsubst %.c,build/dc/%.o,$(INLINE)))
DC_ASM_OBJ := $(filter-out $(DC_REPLACED_OBJ),$(ASM_SOURCES:%.src=build/dc/%.o))
# SH4ZAM's library is its SH-4 back end, source/sh4/, which its headers take
# under -m4-single-only, as config/gcc.mk says. The back end defines each
# function the headers declare out of line under SHZ_BACKEND == SHZ_SH4, but
# shz_version.h's two, which no object calls. The software back end,
# source/sw/, stays out of the library: it calls sqrtf, fabsf, sinf and
# cosf, which the SDK's math.h doesn't declare. DC_SH4ZAM_C_OBJ is the back
# end's C objects, which alone of SH4ZAM's objects write .d files
SH4ZAM_SOURCES := $(filter tools/sh4zam/source/sh4/%,$(SH4ZAM_FILES))
DC_SH4ZAM_OBJ := $(patsubst tools/sh4zam/source/%,build/dc/sh4zam/%.o,$(basename $(SH4ZAM_SOURCES)))
DC_SH4ZAM_C_OBJ := $(patsubst tools/sh4zam/source/%.c,build/dc/sh4zam/%.o,$(filter %.c,$(SH4ZAM_SOURCES)))
DC_SH4ZAM_LIB := build/dc/sh4zam/libsh4zam.a
# The original translation unit owns its data, including private tables the
# compiler can fold. Only the disc-free census compiles it with layout bodies.
DC_BODY_OBJ := $(filter $(DC_C_OBJ) $(DC_INLINE_OBJ),$(BODY_C:%.c=build/dc/%.o))
dc_checked_objects = $(foreach obj,$1,$(if $(filter $(DC_BODY_OBJ),$(obj)),$(obj:build/dc/%=build/dc/check/%),$(obj)))
DC_BODY_CHECK := $(DC_BODY_OBJ:build/dc/%=build/dc/check/%)

# DC_OBJ is every object make dc builds, from the lists DC_OBJ_LISTS names
DC_OBJ_LISTS := DC_C_OBJ DC_INLINE_OBJ DC_ASM_OBJ DC_PAIRED_OBJ DC_SH4ZAM_OBJ
DC_OBJ := $(foreach l,$(DC_OBJ_LISTS),$($l))
# Stop on an entry of DC_REPLACED that isn't P:C, P a source an image places
# and C a C source, and on one of DC_ADDED that isn't IMAGE:C, IMAGE an image
# config/ describes. Stop too where an added C compiles to another source's
# object, and on an object two of the lists give, as two of SH4ZAM's sources
# of one stem would: make would build it by one rule alone
ifneq ($(DC_GOALS),)
# $(call paired,ENTRY) is ENTRY where it is two fields with one colon
# between them, as IMAGE::C and :C are not. An entry's first field is
# looked up among the names it may take, and never taken as a pattern of
# them, as % would be
paired = $(if $(filter 2,$(words $(subst :, ,$1))),$(if $(subst x$1,,x$(word 1,$(subst :, ,$1)):$(word 2,$(subst \
	:, ,$1))),,$1))
UNREPLACED := $(strip $(foreach r,$(DC_REPLACED),$(if $(and $(call paired,$r),$(filter \
	$(C_SOURCES) $(ASM_SOURCES),$(word 1,$(subst :, ,$r))),$(filter %.c,$(wildcard $(word 2,$(subst :, ,$r))))),,$r)))
ifneq ($(UNREPLACED),)
$(error $(UNREPLACED): an entry of DC_REPLACED, and not P:C, P a source an image places and C a C source)
endif
UNADDED := $(strip $(foreach a,$(DC_ADDED),$(if $(and $(call paired,$a),$(filter $(DC_IMAGES),$(word 1,$(subst \
	:, ,$a))),$(filter %.c,$(wildcard $(word 2,$(subst :, ,$a))))),,$a)))
ifneq ($(UNADDED),)
$(error $(UNADDED): an entry of DC_ADDED, and not IMAGE:C, IMAGE an image config/ describes and C a C source)
endif
# make counts the objects itself: one shell command of every path could pass
# the length Linux allows one argument
ifneq ($(words $(DC_OBJ)),$(words $(sort $(DC_OBJ))))
TWICE := $(foreach o,$(sort $(DC_OBJ)),$(if $(word 2,$(filter $o,$(DC_OBJ))),$o))
$(error $(foreach o,$(TWICE),$o: two of make dc's rules build it, by $(strip $(foreach l,$(DC_OBJ_LISTS),\
	$(if $(filter $o,$($l)),$l)))))
endif
endif

# DC_PLAY is the C sources tools/parts.py classes as play: those whose
# floats a game step reads, which keep the console's bits under GCC
# (PORTS.md#floats). FLOATS is the float pass, which each of their objects
# runs, and the modules it imports
ifneq ($(DC_GOALS),)
DC_PLAY := $(shell $(python) -c 'import sys; sys.path[:0] = ["tools"]; import parts; found = parts.rows(); \
	print(*(s for s in sys.argv[1:] if parts.part(found, s).bin == parts.PLAY))' $(C_SOURCES))
ifneq ($(.SHELLSTATUS),0)
$(error tools/parts.py: no bin for each C source of the routes)
endif
endif
FLOATS := tools/floats.py $(wildcard tools/float_*.py) tools/csource.py tools/layout.py tools/inline.py \
	tools/config.py

# $(call gcc_flags,SOURCE,FLAGS) is GCC's flags for SOURCE: GCCFLAGS, its
# level, -O0 where FLAGS, SHC's flags for it, hold -optimize=0, and
# gcc_link_time's, with SOURCE's path for the seed of any intermediate code
gcc_flags = $(GCCFLAGS) $(call gcc_optimize,$1) $(if $(filter -optimize=0,$2),-O0) $(call gcc_link_time,$1) \
	-frandom-seed=$1
# $(call compile,FLAGS,C) has GCC compile C to $@ under FLAGS. GCC runs in
# the C locale with plain diagnostics, so its log reads the same on any
# host. It writes the .d file as a .tmp, which replaces the last one once
# the compile passes, so make reads the last whole one after a compile that
# fails
compile = mkdir -p $(@D) && LC_ALL=C $(SH_ELF)/bin/sh-elf-gcc $1 -fdiagnostics-plain-output -MD -MP -MT $@ \
	-MF $(@:.o=.d).tmp -c -o $@ $2 $(gcc_quiet) && mv $(@:.o=.d).tmp $(@:.o=.d)
# Keep GCC's output in $@.log, and print it and fail on a nonzero status,
# which config/gcc.mk's -Werror gives each warning
gcc_quiet = > $@.log 2>&1 || (cat $@.log; false)
# $(call gcc,INCLUDES,FLAGS[,C]) compiles the source to $@ under
# gcc_flags. INCLUDES is the route's include directories, split by commas,
# which GCC searches as SHC does, and FLAGS SHC's flags for the source. C,
# where given, is the C split from the source, which compiles in its place
# and finds its quoted includes in the source's directory. GCC searches the
# root last, so that a non-matching side names a header of engine/ by its
# path from the root, as "engine/dc/turn.h" (PORTS.md#kernels)
gcc = $(call compile,$(call gcc_flags,$<,$2) $(if $3,-iquote $(<D)) $(addprefix -I,$(subst $(comma), ,$1) .),$(call \
	unit,$(or $3,$<)))
# $(call unit,C) is what GCC reads to compile C. For a body source it is
# the source's polygon list declarations, dc_lists, with C included ahead
# of them: each declaration gives a datum that holds a polygon list 4
# bytes' alignment, and follows the datum's definition, whose type it names
# (dc_body). For any other source it is C
unit = $(if $(dc_lists),-include $1 $(dc_lists),$1)
# A play source's declarations follow its .i instead, once the float pass
# has rewritten it: the pass leaves the lines of a file that -include names
# as they are, so play preprocesses the source itself
lists_follow = $(if $(dc_lists),&& cat $(dc_lists) >> $(@:.o=.i))
# $(call play,INCLUDES,FLAGS,RELEASE[,C]) compiles a play source as gcc
# does, through the float pass. GCC's preprocessor writes $@'s .i, which
# tools/floats.py rewrites for RELEASE, the SHC that builds the source, and
# the flags of FLAGS it reads, -fpu=single and -aggressive=2, so that GCC
# fuses the products SHC fuses and no others. It leaves the lines of
# SH4ZAM's headers, under SH4ZAM_INCLUDE, as they are. GCC then compiles
# the .i under GCC_FUSING. The pass's and GCC's output both go to $@.log.
# The .i goes once the .d file is in place, so it stays for a compile that
# fails
play_flags = $(call gcc_flags,$<,$2) $(if $4,-iquote $(<D)) $(addprefix -I,$(subst $(comma), ,$1) .)
play = mkdir -p $(@D) && (LC_ALL=C $(SH_ELF)/bin/sh-elf-gcc $(play_flags) -fdiagnostics-plain-output -MD -MP -MT $@ \
	-MF $(@:.o=.d).tmp -E -o $(@:.o=.i) $(or $4,$<) && python3 tools/floats.py $(strip --shc $(subst R,r,$3) \
	$(filter -fpu=single -aggressive=2,$2)) --system $(SH4ZAM_INCLUDE) $(@:.o=.i) $(lists_follow) && LC_ALL=C \
	$(SH_ELF)/bin/sh-elf-gcc $(play_flags) $(GCC_FUSING) -fdiagnostics-plain-output -c -o $@ $(@:.o=.i)) \
	$(gcc_quiet) && mv $(@:.o=.d).tmp $(@:.o=.d) && rm -f $(@:.o=.i)

# No SHC builds a source of DC_PAIRED_OBJ's, so it takes no route's flags.
# DC_INC, the include directories it searches before the root, are the
# game's route's
DC_INC := $(game.includes)

# DC_COMPILED is the objects GCC compiles, each from C. Each depends on its
# .d file, which keeps its header dependencies, as an SHC object's does
DC_COMPILED := $(DC_BODY_CHECK) $(DC_C_OBJ) $(DC_INLINE_OBJ) $(DC_PAIRED_OBJ) $(DC_SH4ZAM_C_OBJ)
$(DC_COMPILED): %.o: %.d
$(DC_COMPILED:.o=.d): ;
-include $(DC_COMPILED:.o=.d)

# Before GCC's code calls a source's assembly, tools/tags.py --abi holds
# each piece it may call to GCC's calling convention: each inline body GCC
# compiles, and each .src function tagged @asm abi. A piece that may break
# it fails unless config/gcc.mk's CONTRACTS names it, with how GCC's callers
# meet its contract (PORTS.md#inline-assembly). ABI_NEEDS is what the check
# reads
abi = python3 tools/tags.py --abi $(addprefix --contract ,$(call contracts,$<)) $<
ABI_NEEDS := tools/tags.py tools/split.py tools/inline.py tools/csource.py tools/guards.py tools/config.py

# GCC compiles each route's C sources with the route's include directories
# and its flags. Each waits for SH4ZAM's files, whose headers the
# non-matching code includes. A source with inline bodies, one of INLINE,
# passes the check above, then splits: tools/split.py splits it into C and
# its bodies' text, which build/shckit translates for the C's file-scope asm
# to include. Where every body lies on the matching side of a guard, the
# text is empty. shckit refuses an empty text, so the split translates none,
# and the C includes none
split = $(abi) && python3 tools/split.py $< $(@:.o=.c) $(@:.o=_inline.src) && \
	if [ -s $(@:.o=_inline.src) ]; then build/shckit translate -o $(@:.o=_inline.s) $(@:.o=_inline.src); \
	else rm -f $(@:.o=_inline.s); fi
SPLIT_NEEDS := $(ABI_NEEDS) build/shckit
# $2 is the output directory. $3 selects this route's objects, expressed
# as their normal build/dc paths so the same inline and float rules apply.
dc_compile_at = $(patsubst build/dc/%,$1/%,$(filter $2,$(patsubst %.c,build/dc/%.o,$3)))
define dc_route
$(call dc_compile_at,$2,$(filter $3,$(DC_C_OBJ)),$(filter-out $(DC_PLAY),$($1.sources))): $2/%.o: %.c \
		build/dc/%.cmd $$(GCC_ID) | $$(FETCHED)
	@$$(call status,gcc,$$<)
	$$(Q)$$(call gcc,$$($1.includes)$$(dc_body_search),$$($1.flags))
$(call dc_compile_at,$2,$(filter $3,$(DC_INLINE_OBJ)),$(filter-out $(DC_PLAY),$($1.sources))): $2/%.o: \
		%.c build/dc/%.cmd $$(SPLIT_NEEDS) $$(GCC_ID) | $$(FETCHED)
	@$$(call status,gcc,$$< (split))
	$$(Q)$$(split)
	$$(Q)$$(call gcc,$$($1.includes)$$(dc_body_search),$$($1.flags),$$(@:.o=.c))
$(call dc_compile_at,$2,$(filter $3,$(DC_C_OBJ)),$(filter $(DC_PLAY),$($1.sources))): $2/%.o: %.c \
		build/dc/%.cmd $$(FLOATS) $$(GCC_ID) | $$(FETCHED)
	@$$(call status,gcc,$$< (play))
	$$(Q)$$(call play,$$($1.includes)$$(dc_body_search),$$($1.flags),$$($1.release))
$(call dc_compile_at,$2,$(filter $3,$(DC_INLINE_OBJ)),$(filter $(DC_PLAY),$($1.sources))): $2/%.o: %.c \
		build/dc/%.cmd $$(SPLIT_NEEDS) $$(FLOATS) $$(GCC_ID) | $$(FETCHED)
	@$$(call status,gcc,$$< (split, play))
	$$(Q)$$(split)
	$$(Q)$$(call play,$$($1.includes)$$(dc_body_search),$$($1.flags),$$($1.release),$$(@:.o=.c))
endef
$(foreach r,$(ROUTES),$(eval $(call dc_route,$r,build/dc,%)))
ifneq ($(DC_BODY_OBJ),)
$(foreach r,$(ROUTES),$(eval $(call dc_route,$r,build/dc/check,$(DC_BODY_OBJ))))
# Generated headers are grouped by source. A deleted include regenerates before
# the compile even when the dependency file is missing or names the old body.
#
# njCnkSetMaterial reads a material chunk's colors from its polygon list by
# a 32-bit load, which faults on the SH-4 off a multiple of 4. SHC starts
# each datum that holds a polygon list on one. GCC starts a datum at its
# type's alignment, 2 for an array of Sint16 and for a struct of them. It
# takes that from the datum's declarations alone: no flag of GCC's raises
# it, and no link can, since the census joins the sections of one name, and
# a source of GCC_DATA keeps its data in one section. So
# tools/polygon_lists.py writes a body source's declarations,
# build/dc/SOURCE.lists.c: one for each datum that a chunk model's plist
# points into, which gives it 4 bytes' alignment. Both of the source's
# objects compile them after the source (unit, lists_follow). make dc-images then fails
# on a material chunk whose colors lie off 4 bytes (config/images.mk)
POLYGON_LISTS := tools/polygon_lists.py $(BODY_TOOLS)
define dc_body
build/dc/$(1:.c=.o): private dc_body_search := $(comma)$(call body_root,$1,build/gen)
build/dc/$(1:.c=.o): $(call body_includes,$1,build/gen) dc.mk
build/dc/check/$(1:.c=.o): private dc_body_search := $(comma)$(call body_root,$1,build/stub)
build/dc/check/$(1:.c=.o): $(call body_includes,$1,build/stub) dc.mk
build/dc/$(1:.c=.o) build/dc/check/$(1:.c=.o): private dc_lists := build/dc/$(1:.c=.lists.c)
build/dc/$(1:.c=.o) build/dc/check/$(1:.c=.o): build/dc/$(1:.c=.lists.c)
build/dc/$(1:.c=.lists.c): $1 $(wildcard offsets/$(basename $1).txt) $(call body_binding_inputs,$1) \
		$(body_headers.$1) $(POLYGON_LISTS)
	@$$(call status,lists,$1)
	$$(Q)python3 tools/polygon_lists.py $1 $$@
endef
$(foreach s,$(DC_BODY_OBJ:build/dc/%.o=%.c),$(eval $(call dc_body,$s)))
endif
# An empty recipe lets a split C go missing, though its object's .d file
# names it: the object then splits its source again
$(DC_INLINE_OBJ:.o=.c) $(patsubst build/dc/%.o,build/dc/check/%.c,$(filter $(DC_INLINE_OBJ),$(DC_BODY_OBJ))): ;

define dc_object
$1: $2 $(1:.o=.cmd) $$(GCC_ID) | $$(FETCHED)
	@$$(call status,gcc,$$<)
	$$(Q)$$(call gcc,$$(DC_INC))
endef
$(foreach p,$(DC_PAIRS),$(eval $(call dc_object,$(firstword $(subst :, ,$p)),$(lastword $(subst :, ,$p)))))

# Each .src file passes the check above. build/shckit then translates it for
# GNU as, which assembles it under GASFLAGS
dc_assemble = mkdir -p $(@D) && $(abi) && build/shckit translate -o build/dc/$*.s $< && \
	$(SH_ELF)/bin/sh-elf-as $(GASFLAGS) -o $@ build/dc/$*.s
$(DC_ASM_OBJ): build/dc/%.o: %.src build/dc/%.cmd $(ABI_NEEDS) build/shckit $(GCC_ID)
	@$(call status,gas,$<)
	$(Q)$(dc_assemble)

# Each object's .cmd file holds what builds it, as an SHC object's does: for
# a C source, its route's include directories and GCC's flags for it, and
# for a .src file and SH4ZAM's sources, GNU as's or GCC's flags, each with
# the text of the macros its rule runs, and for a source with assembly its
# names in config/gcc.mk's CONTRACTS. make rewrites a .cmd file only where
# that text changes, so an object builds again where what builds it
# changes. DC_COMPILE names the macros a C object's rule runs, and
# DC_ASSEMBLE an assembled one's. The files are written as make reads these
# lines, so they follow each macro they hold: one not yet defined holds no
# text
DC_COMPILE  := gcc unit gcc_flags gcc_optimize gcc_link_time compile gcc_quiet
DC_ASSEMBLE := dc_assemble abi
# A play source's object runs play in gcc's place. Its .cmd file holds too
# its route's release, the flags of its route the float pass reads,
# -fpu=single and -aggressive=2, and GCC_FUSING, so that the object builds
# again where its source moves to a route the pass reads otherwise
DC_PLAY_COMPILE := play lists_follow play_flags gcc_flags gcc_optimize gcc_link_time gcc_quiet
ifneq ($(DC_GOALS),)
$(shell mkdir -p $(sort $(dir $(DC_OBJ) $(DC_BODY_CHECK))) build/dc/census build/dc/check)
$(foreach r,$(ROUTES),$(foreach s,$($r.sources),$(if $(filter build/dc/$(s:.c=.o),$(DC_C_OBJ) $(DC_INLINE_OBJ)),$(call \
	keep,build/dc/$(s:.c=.cmd),$($r.includes) $(call gcc_flags,$s,$($r.flags)) $(if $(filter $s,$(DC_PLAY)),$(filter \
	-fpu=single -aggressive=2,$($r.flags)) $($r.release) $(GCC_FUSING) )$(call unexpanded,$(if $(filter \
	$s,$(DC_PLAY)),$(DC_PLAY_COMPILE),$(DC_COMPILE)) $(if $(filter $s,$(INLINE)),split abi)) $(if $(filter \
	$s,$(INLINE)),$(call contracts,$s))))))
$(foreach o,$(DC_ASM_OBJ),$(call keep,$(o:.o=.cmd),$(GASFLAGS) $(call unexpanded,$(DC_ASSEMBLE)) $(call \
	contracts,$(o:build/dc/%.o=%.src))))
$(foreach p,$(DC_PAIRS),$(call keep,$(patsubst %.o,%.cmd,$(firstword $(subst :, ,$p))),$(DC_INC) $(call \
	gcc_flags,$(lastword $(subst :, ,$p))) $(call unexpanded,$(DC_COMPILE))))
$(foreach o,$(DC_SH4ZAM_C_OBJ),$(call keep,$(o:.o=.cmd),$(GCC_SH4ZAM_FLAGS) $(call unexpanded,compile gcc_quiet)))
$(foreach o,$(filter-out $(DC_SH4ZAM_C_OBJ),$(DC_SH4ZAM_OBJ)),$(call keep,$(o:.o=.cmd),$(GAS_CHECKS)))
endif

# SH4ZAM's C compiles under GCC_SH4ZAM_FLAGS, which config/gcc.mk gives its
# objects. Each object takes its language from tools/SHA256SUMS, not from
# the files on disk: across a pin that turns a .c into a .s, the old .c
# stays until FETCHED's recipe removes it
$(DC_SH4ZAM_C_OBJ): build/dc/sh4zam/%.o: tools/sh4zam/source/%.c build/dc/sh4zam/%.cmd $(GCC_ID) | $(FETCHED)
	@$(call status,gcc,$<)
	$(Q)$(call compile,$(GCC_SH4ZAM_FLAGS),$<)

# GNU as assembles SH4ZAM's assembly as it is, little-endian under
# GAS_CHECKS, not GASFLAGS. It pads each section to its alignment, which
# GASFLAGS' --no-pad-sections would not. The assembly includes no file, so
# it waits for no fetch but its own
$(filter-out $(DC_SH4ZAM_C_OBJ),$(DC_SH4ZAM_OBJ)): build/dc/sh4zam/%.o: tools/sh4zam/source/%.s \
		build/dc/sh4zam/%.cmd $(GCC_ID)
	@$(call status,gas,$<)
	$(Q)mkdir -p $(@D) && $(SH_ELF)/bin/sh-elf-as $(GAS_CHECKS) --little -o $@ $<

# make removes the archive before each write, so it holds no member another
# pin left. ar's D records no dates or user ids, so the same objects make the
# same archive. Its members' inventory writes it again when a pin drops one
ifneq ($(DC_GOALS),)
$(call keep,build/dc/sh4zam/members.txt,$(sort $(DC_SH4ZAM_OBJ)))
endif
$(DC_SH4ZAM_LIB): $(DC_SH4ZAM_OBJ) build/dc/sh4zam/members.txt $(LD_ID)
	@$(call status,ar,$@)
	$(Q)mkdir -p $(@D) && rm -f $@ && $(SH_ELF)/bin/sh-elf-ar rcsD $@ $(DC_SH4ZAM_OBJ)

# Each image's census. GCC's link-time compile reads the intermediate code
# of the image's objects whole, as config/gcc.mk's GCC_LTO_LINK has it, with
# the archive and libgcc.a, which give the objects the members they call. -r
# links what it compiles into one relocatable object, which no address
# places and which may leave a name undefined, and -flinker-output=nolto-rel
# has lto1 compile the code, where -r alone would join the intermediate
# code into an object of the same. tools/census.py then writes each name the
# object leaves undefined. For an image other than the main executable, it
# leaves out what the image takes from the main executable: each name of
# the main executable's symbols.txt, as tools/config.py gives every other
# image the main executable's names, and each name the main executable's
# relocatable object defines.
#
# $(call dc_image_obj,IMAGE) is the GCC object of each source IMAGE's
# splits.txt places, and each DC_ADDED gives it. The link reads them from
# build/dc/census/IMAGE.in, as an argument of @IMAGE.in: 1ST_READ.BIN's paths
# would fill much of the 128 KB Linux allows one argument, and make hands
# the shell a command as one. The list is an inventory, which has the link
# run again once an object joins or leaves, and the .cmd file beside it
# holds the link's flags
# The SH-4 link gives CHAO typed storage for the two archives loaded at startup.
CHAO_BANKS := build/dc/census/chao-bank-objects
# Recover the actual object bytes for the production link. The census can
# check the same named ranges from their metadata without reading the disc.
# The playable object and the disc-free census read the same typed declarations.
# A bank written as C has no table: its source and headers are all it reads.
dc_filename_inputs = $1 $(if $(wildcard offsets/$(basename $1).txt),offsets/$(basename $1).txt \
    $(call body_binding_inputs,$1)) $(body_headers.$1)
# Imported objects use their provider's optical types and the consumer's native symbol address.
dc_import_providers = $(sort $(shell awk '{sub(/\043.*/, "")} $$1 == "import" {print $$2}' config/$1/data.txt))
dc_image_data = config/$1/data.txt $(call content_layouts,$1) config/$1/splits.txt config/$1/symbols.txt \
    $(foreach f,$(call content_filenames,$1),$(call dc_filename_inputs,$f))
# tools/content.py and the tools it imports. build/dc/content/IMAGE.in is
# the inventory of what its content reads, so that the content is generated
# again once an optional table leaves, though every input left is older
CONTENT_TOOLS := tools/content.py tools/cart_data.py tools/cart_polygons.py tools/land_data.py \
                    tools/chunk_poses.py tools/ninja_motions.py tools/stage_paths.py tools/texture_data.py tools/config.py $(BODY_TOOLS)
define dc_content
$(eval dc_content_inputs.$1 := config/$(DC_MAIN)/symbols.txt $(CONTENT_TOOLS) \
    $(foreach image,$1 $(call dc_import_providers,$1),$(call dc_image_data,$(image))))
$(if $(DC_GOALS),$(shell mkdir -p build/dc/content)$(call keep,build/dc/content/$1.in,$(dc_content_inputs.$1)))
build/dc/content/$1.s: $(call original,$2) build/dc/content/$1.in $(dc_content_inputs.$1)
	$$(Q)python3 tools/content.py $1 $$< $$@
build/dc/content/$1.o: build/dc/content/$1.s $(call original,$2) dc.mk config/gcc.mk $(GCC_ID)
	@$$(call status,as,$$@)
	$$(Q)$(SH_ELF)/bin/sh-elf-as $(GAS_CHECKS) --little -o $$@ $$<
endef
$(foreach i,$(CONTENT_IMAGES),$(eval $(call dc_content,$i,$(patsubst $i:%,%,$(filter $i:%,$(IMAGES))))))
dc_members = $(strip $(patsubst build/link/$1/%.bin,build/dc/%.o,$(filter build/link/$1/%,$(LINKS))) \
	$(patsubst $1:%.c,build/dc/%.o,$(filter $1:%,$(DC_ADDED))) $(if $(filter chao,$1),$(CHAO_BANKS).o) \
	$(if $(dc_checking),,$(if $(filter $1,$(CONTENT_IMAGES)),build/dc/content/$1.o)))
# strt1.src records all sixteen Sofdec groups, including the empty ones.
# Process the linker's section list once for the resident image.
ifneq ($(DC_SOFDEC_LAYOUT),)
DC_SCRIPT.$(DC_MAIN) := build/dc/census/sofdec.ld
$(DC_SCRIPT.$(DC_MAIN)): $(DC_SOFDEC_LAYOUT) $(GCC_ID)
	@mkdir -p $(@D)
	$(Q)$(SH_ELF)/bin/sh-elf-gcc -E -P -x c $< -o $@
endif
DC_RELOCATABLE := -r -nostdlib -flinker-output=nolto-rel
# A recursive recipe gives GCC the jobserver pipe on older GNU make, too.
# Keep link recipes ordinary under dry-run, question, and touch modes.
ifneq ($(DC_GOALS),)
ifeq ($(DC_LTO_JOBS),jobserver)
dc_make_mode = $(firstword -$(MAKEFLAGS))
dc_lto_prefix = $(if $(or $(findstring n,$(dc_make_mode)),$(findstring q,$(dc_make_mode)),$(findstring t,$(dc_make_mode))),,+)
else
ifeq ($(shell expr $(call quoted,$(DC_LTO_JOBS)) : '[1-9][0-9]*$$'),0)
$(error DC_LTO_JOBS must be jobserver or a positive decimal worker count)
endif
# GCC would use a live jobserver in preference to an explicit worker count.
dc_lto_env := MAKEFLAGS= MFLAGS=
endif
endif
dc_lto = $(dc_lto_prefix)$(dc_lto_env) MAKE=$(call quoted,$(MAKE)) LC_ALL=C $(SH_ELF)/bin/sh-elf-gcc $(GCC_LTO_LINK)
dc_census = $(dc_lto) $(DC_RELOCATABLE) \
	$(addprefix -Wl$(comma)-T$(comma),$(DC_SCRIPT.$1)) -o $@ @$(@:.o=.in) \
	$(DC_SH4ZAM_LIB) $(LIBGCC) > $(@:.o=.log) 2>&1 || (cat $(@:.o=.log); false)
# The public links use the original bodies. Only objects and links reached
# by a body source need a second output path for the disc-free check.
dc_image_obj = $(if $(dc_checking),$(call dc_checked_objects,$(call dc_members,$1)),$(call dc_members,$1))
dc_base = build/dc/$(if $(and $(dc_checking),$(filter $1,$(DC_BODY_IMAGES))),check/)census/$1
dc_bound = $(or $(area_bound.$(call dc_base,$1)),$(call dc_base,$1).o)
dc_terminal = $(if $(filter $1,$(DC_TERRAIN)),$(call dc_base,$1)-terrain.o,$(call dc_bound,$1))
DC_BODY_IMAGES := $(sort $(CONTENT_IMAGES) $(foreach i,$(DC_IMAGES),$(if $(filter $(DC_BODY_OBJ),$(call dc_members,$i)),$i)))
CHAO_AREAS := $(filter chaostgneut chaostghero chaostgdark chaostglobby chaostgentrance chaostgrace chaostgkinder,$(DC_IMAGES))
ifneq ($(filter chao,$(DC_BODY_IMAGES)),)
DC_BODY_IMAGES := $(sort $(DC_BODY_IMAGES) $(CHAO_AREAS))
endif
DC_TERRAIN := $(if $(DC_TERRAIN_LAYOUT),$(filter stg% boss_% chaostgentrance chaostghero chaostgneut,$(DC_IMAGES)))
# A change in the resident also affects reports for overlays whose own link
# can still be shared. The report names the exports of its matching resident.
dc_report = build/dc/$(if $(and $(dc_checking),$(filter $1 $(DC_MAIN),$(DC_BODY_IMAGES))),check/)census/$1.txt

ifneq ($(DC_GOALS),)
dc_scope := $(DC_IMAGES)
dc_reports := $(DC_IMAGES)
include config/dc_links.mk
ifneq ($(DC_BODY_IMAGES),)
dc_checking := 1
dc_scope := $(DC_BODY_IMAGES)
dc_reports := $(if $(filter $(DC_MAIN),$(DC_BODY_IMAGES)),$(DC_IMAGES),$(DC_BODY_IMAGES))
$(shell mkdir -p build/dc/check/census)
include config/dc_links.mk
endif
DC_CHECK_REPORTS := $(foreach i,$(DC_IMAGES),$(call dc_report,$i))
DC_CHECK_TERMINALS := $(foreach i,$(DC_IMAGES),$(call dc_terminal,$i))
DC_CHECK_COMMANDS := $(foreach i,$(DC_IMAGES),$(call dc_base,$i).cmd) $(KINDER_LAYOUT:.o=.cmd)
dc_checking :=
DC_CENSUS := $(foreach i,$(DC_IMAGES),$(call dc_report,$i))
endif
# The final-image rules always see the production resident.
ifneq ($(area_bound.build/dc/census/$(DC_MAIN)),)
MAIN_CENSUS := build/dc/census/$(DC_MAIN)
MAIN_LAYOUT := $(area_bound.$(MAIN_CENSUS))
endif

ifneq ($(filter dc-images dc-files dc-disc build/dc/images/% build/dc/memory/% build/dc/manifest.txt build/dc/receipts/% \
	build/dc/%.BIN build/dc/%.PRS,$(DC_GOALS)),)
include config/images.mk
endif

# CHAO loads two archives that no image holds. The GCC link places the model
# roots and motion headers the Chao images name at their addresses in those
# archives (tools/chao_banks.py). make dc holds each archive to the Japanese
# disc's hashes, packed and decoded, and follows the pointers from each root
# and header, failing where one leaves its archive
# (tools/check_chao_banks.py). Without the archives, in orig/ or from
# DISC_FILES or the .gdi, make dc says it skips the check. disc.txt names
# where make takes the archives from, and for DISC_FILES their checksums
# too, so an archive replaced in place is taken again. read.txt names the
# archives the check reads, so it runs again once make reads them from
# elsewhere, even from files older than its last result
CHAO_ARCHIVES := SONIC2/CHAO_MODEL.PRS SONIC2/CHAO_MOTION.PRS
ifneq ($(filter chao,$(DC_IMAGES)),)
ifeq ($(wildcard $(CHAO_ARCHIVES:%=orig/%)),$(CHAO_ARCHIVES:%=orig/%))
CHAO_READ := $(CHAO_ARCHIVES:%=orig/%)
else ifneq ($(or $(strip $(DISC_FILES)),$(filter 1,$(FOUND))),)
CHAO_READ := $(CHAO_ARCHIVES:%=build/dc/chao/%)
ifneq ($(DC_GOALS),)
$(shell mkdir -p build/dc/chao)
$(call keep,build/dc/chao/disc.txt,$(or $(strip $(DISC_FILES)),$(GDI_IMAGE))$(if $(strip $(DISC_FILES)), $(shell \
	cd $(call quoted,$(DISC_FILES)) 2> /dev/null && cksum $(CHAO_ARCHIVES) 2> /dev/null)))
endif
ifneq ($(strip $(DISC_FILES)),)
$(CHAO_READ) &: build/dc/chao/disc.txt
	$(Q)mkdir -p build/dc/chao/SONIC2 && \
		cp $(foreach f,$(CHAO_ARCHIVES),$(call quoted,$(DISC_FILES)/$f)) build/dc/chao/SONIC2
else
# build/gdi never replaces a file
$(CHAO_READ) &: build/dc/chao/disc.txt build/gdi
	@$(call status,gdi,$(GDI_IMAGE))
	$(Q)rm -rf build/dc/chao/SONIC2 && build/gdi extract $(call quoted,$(GDI_IMAGE)) build/dc/chao $(CHAO_ARCHIVES)
endif
else
CHAO_SKIPPED := skipped: orig/ lacks $(subst $(space), or ,$(CHAO_ARCHIVES)), and neither DISC_FILES nor one .gdi \
	gives them
dc:
	@$(call status,chao,$(CHAO_SKIPPED))
endif
endif
ifneq ($(CHAO_READ),)
CHAO_CHECK := build/dc/chao/banks.txt
ifneq ($(DC_GOALS),)
$(shell mkdir -p build/dc/chao)
$(call keep,build/dc/chao/read.txt,$(CHAO_READ))
endif
$(CHAO_CHECK): build/dc/chao/read.txt $(CHAO_READ) build/prskit tools/check_chao_banks.py tools/chao_banks.py \
		tools/config.py tools/tool_process.py config/chao/symbols.txt config/chaostgrace/symbols.txt
	$(Q)mkdir -p $(@D) && python3 tools/check_chao_banks.py $(CHAO_READ) --prskit build/prskit > $@
	@printf '  %-6s %s\n' chao "$$(cat $@)"
endif

# tools/addresses.py holds each number for RAM that the port's sources,
# tables and generated data hold to a row of config/addresses.txt
# (PORTS.md#addresses). make dc's pass reads the full build's bodies and the
# disc data each image's content keeps. make dc-check's reads the stub
# build's bodies and no disc. A pass's list names each body source and then
# its bodies, since tools/body_data.py reads a body's kind from its source.
# That list and sources.txt, the C and assembly whose numbers the check
# reads, are inventories, so that a pass runs again once a file joins or
# leaves
ifneq ($(DC_GOALS),)
ADDRESS_SOURCES := $(shell $(python) -c 'import sys; sys.path[:0] = ["tools"]; import addresses; \
	print(*addresses.sources())')
ifneq ($(.SHELLSTATUS),0)
$(error tools/addresses.py: no list of the sources it reads)
endif
endif
ADDRESS_NEEDS := build/dc/addresses/sources.txt $(ADDRESS_SOURCES) config/addresses.txt \
	$(wildcard config/*/splits.txt config/*/symbols.txt) tools/addresses.py tools/csource.py tools/inline.py \
	tools/layout.py tools/kinds.py $(CONTENT_TOOLS)
ADDRESS_STUB := $(foreach s,$(BODY_C),$s $(call body_includes,$s,build/stub))
ADDRESS_GEN := $(foreach s,$(BODY_C),$s $(call body_includes,$s,build/gen))
ifneq ($(DC_GOALS),)
$(shell mkdir -p build/dc/addresses)
$(call keep,build/dc/addresses/sources.txt,$(ADDRESS_SOURCES))
$(call keep,build/dc/addresses/stub.txt,$(ADDRESS_STUB))
$(call keep,build/dc/addresses/gen.txt,$(ADDRESS_GEN))
endif
build/dc/addresses/stub.ok: build/dc/addresses/stub.txt $(ADDRESS_STUB) $(ADDRESS_NEEDS)
	@$(call status,addrs,config/addresses.txt)
	$(Q)python3 tools/addresses.py --bodies $< && touch $@
build/dc/addresses/gen.ok: build/dc/addresses/gen.txt $(ADDRESS_GEN) $(ADDRESS_NEEDS) \
		$(CONTENT_IMAGES:%=build/dc/content/%.s)
	@$(call status,addrs,config/addresses.txt)
	$(Q)python3 tools/addresses.py --bodies $< $(CONTENT_IMAGES:%=build/dc/content/%.s) && touch $@

# make dc builds every object, the archive and each image's census, and
# checks the Chao archives and the addresses
dc: $(DC_OBJ) $(DC_SH4ZAM_LIB) $(DC_CENSUS) $(CHAO_CHECK) build/dc/addresses/gen.ok

# The disc-free receipt covers each input object, the archive, and the actual
# terminal object, link command and symbol report of each image. The resident,
# Chao areas and terrain have their own terminal layouts. Select them while
# dc_checking still names the stub build above. The inventory file keeps the
# list out of the shell command line and makes changes in its membership visible.
DC_SUMMED := $(sort $(call dc_checked_objects,$(DC_OBJ)) $(DC_CHECK_TERMINALS) $(DC_CHECK_COMMANDS) $(DC_CHECK_REPORTS))
ifneq ($(DC_GOALS),)
$(call keep,build/dc/check/objects.txt,$(DC_SUMMED))
endif
# Only fat input objects need their checkout-specific intermediate code removed.
# Sum the terminal ELF directly: copying it through GNU objcopy can retarget a
# relocation against a zero-valued absolute symbol to ELF's null symbol.
dc_sum = case $1 in build/dc/census/*.o|build/dc/check/census/*.o) sha256sum $1;; \
	*.o) $(SH_ELF)/bin/sh-elf-objcopy -R '.gnu.lto_*' $1 build/dc/check/lean.o && \
	sum=$$(sha256sum < build/dc/check/lean.o) && echo "$${sum%% *}  $1";; *) sha256sum $1;; esac
# Tool stamps already hold sha256sum's lines, including libgcc.a. Give each
# tool its installed path below sh-elf/ so the receipt can move with its prefix.
# A line whose path doesn't open with SH_ELF's stops the receipt. A stamp
# holds such a line where the shell expanded a glob in SH_ELF, or where GNU
# sha256sum escaped a name that holds a backslash
build/dc/check.txt: build/dc/check/objects.txt $(DC_SUMMED) $(DC_SH4ZAM_LIB) $(GCC_ID) $(LD_ID)
	@$(call status,sums,$@)
	$(Q)for f in $$(cat $<) $(DC_SH4ZAM_LIB); do $(call dc_sum,$$f) || exit 1; done > $@.tmp
	$(Q)awk -v root=$(call quoted,$(SH_ELF)/) '{ path = substr($$0,67) } index(path,root) != 1 { \
		print FILENAME ":" FNR ": not a hash and a path under " root > "/dev/stderr"; exit 1 } \
		{ print substr($$0,1,66) "sh-elf/" substr(path,length(root)+1) }' $(GCC_ID) $(LD_ID) >> $@.tmp
	$(Q)LC_ALL=C sort -k 2 $@.tmp > $@ && rm -f $@.tmp build/dc/check/lean.o
# make dc-check holds each census to config/imports.txt, whose header gives
# the rule. Given no original, tools/content.py checks an image's data.txt
# against its other tables and its imports' providers, and reads no disc.
# Nor does the check of the addresses, which reads the stub build's bodies
dc-check: build/dc/check.txt build/dc/addresses/stub.ok
	$(Q)awk 'FILENAME == ARGV[1] { rows = FNR; if (!NF || $$1 ~ /^#/) next; \
			if (NF < 3) { print ARGV[1] ":" FNR ": not a row: IMAGE NAME REASON"; failed = 1; next } \
			image[FNR] = $$1; name[FNR] = $$2; expected[$$1 " " $$2]; next } \
		FNR == 1 { census = FILENAME; sub(/.*\//, "", census); sub(/\.txt$$/, "", census) } \
		{ held[census " " $$1] } \
		!((census " " $$1) in expected) { print FILENAME ": " $$1 ": undefined, and no row of " ARGV[1] \
			" gives it for " census; failed = 1 } \
		END { for (n = 1; n <= rows; n++) if ((n in name) && !((image[n] " " name[n]) in held)) { \
			print ARGV[1] ":" n ": " name[n] ": the census of " image[n] " holds no such name"; failed = 1 } \
			exit failed }' config/imports.txt $(DC_CHECK_REPORTS)
	$(Q)$(foreach i,$(CONTENT_IMAGES),python3 tools/content.py $i &&) :
	@printf '  %-6s %s names in %s censuses, %s of them distinct\n' census \
		"$$(cat $(DC_CHECK_REPORTS) | wc -l | tr -d ' ')" $(words $(DC_IMAGES)) \
		"$$(cat $(DC_CHECK_REPORTS) | sort -u | wc -l | tr -d ' ')"

# The toolchain make builds where SH_ELF names tools/sh-elf: binutils, and
# GCC for C alone with the libgcc of its one multilib, whose division, copy
# and double routines GCC's code calls. GMP, MPFR and MPC build inside GCC's
# tree, which builds GMP for no CPU in particular, so GMP needs no m4. The
# recipe fetches the sources itself, as SHC's does. It builds under
# build/toolchain/ and installs in tools/sh-elf.part. It fails there where
# GCC's build installed no file of one of LINK_TIME_TOOLS' names, so no
# toolchain that can't run make dc's link-time compile takes tools/sh-elf's
# place. Then it writes the toolchain's receipt, request.txt, which is
# SH_ELF_REQUEST. tools/sh-elf.part then takes tools/sh-elf's place, whole,
# and the recipe removes the sources and the build
SH_ELF_PREFIX := $(call quoted,$(CURDIR)/tools/sh-elf.part)
# $(call sh_elf,PACKAGE,COMMAND) runs COMMAND in PACKAGE's build directory and
# adds its output to PACKAGE's log, which a failure names. It enters the
# directory with cd -P, which names it in PWD through no symlink, as CURDIR
# names the checkout: GCC takes the directory of a compile from PWD, and
# sh_elf_map's path must open it, where a symlink led to the checkout too. No
# make that COMMAND runs inherits this make's flags, its jobserver among them.
# SH_ELF_MAKE and SH_ELF_INSTALL run this make's own executable, $(MAKE). The
# first builds a package with this make's -j, if it has one, and the second
# installs it with one job. MAKEINFO=true keeps each from writing a manual,
# which would need makeinfo
sh_elf = (mkdir -p build/toolchain/$1.build && cd -P build/toolchain/$1.build && unset MAKEFLAGS MFLAGS MAKELEVEL && \
	$2) >> build/toolchain/$1.log 2>&1 || (echo 'build/toolchain/$1.log: $1 failed'; false)
SH_ELF_MAKE = $(MAKE) $(filter -j%,$(MAKEFLAGS))
SH_ELF_INSTALL = $(MAKE)
# libgcc_tm.h is made before libgcc: its t-sh doesn't make
# unwind-dw2-Os-4-200.o wait for it
tools/sh-elf/bin/sh-elf-gcc:
	@$(call status,fetch,tools/sh-elf.src)
	$(Q)rm -rf tools/sh-elf.src && mkdir -p tools/sh-elf.src$(foreach s,$(SH_ELF_SOURCES), && $(call \
		fetch,https://ftp.gnu.org/gnu/$s,tools/sh-elf.src/$(notdir $s)))
	@$(call status,build,tools/sh-elf)
	$(Q)rm -rf build/toolchain tools/sh-elf.part
	$(Q)for p in binutils gcc gcc/gmp gcc/mpfr gcc/mpc; do mkdir -p build/toolchain/$$p && \
		tar -xf tools/sh-elf.src/$${p#gcc/}-*.tar.* -C build/toolchain/$$p --strip-components=1 || exit 1; done
	$(Q)$(call sh_elf,binutils,../binutils/configure --prefix=$(SH_ELF_PREFIX) $(SH_ELF_BINUTILS))
	$(Q)$(call sh_elf,binutils,$(SH_ELF_MAKE) MAKEINFO=true && $(SH_ELF_INSTALL) install MAKEINFO=true)
	$(Q)$(call sh_elf,gcc,PATH=$(SH_ELF_PREFIX)/bin:$$PATH && export PATH && \
		../gcc/configure --prefix=$(SH_ELF_PREFIX) $(SH_ELF_GCC) $(call sh_elf_map,$(call quoted,$(CURDIR))) && \
		$(SH_ELF_MAKE) all-gcc configure-target-libgcc MAKEINFO=true && \
		$(SH_ELF_MAKE) -C sh-elf/libgcc libgcc_tm.h && $(SH_ELF_MAKE) all-target-libgcc MAKEINFO=true && \
		$(SH_ELF_INSTALL) install-gcc install-target-libgcc MAKEINFO=true)
	$(Q)for f in $(LINK_TIME_TOOLS); do ls tools/sh-elf.part/libexec/gcc/sh-elf/*/$$f > /dev/null 2>&1 || \
		{ echo "tools/sh-elf.part: GCC's build installed no $$f, which make dc's link-time compile runs"; exit 1; }; done
	$(Q)printf '%s\n' $(call quoted,$(SH_ELF_REQUEST)) > tools/sh-elf.part/request.txt
	$(Q)rm -rf tools/sh-elf && mv tools/sh-elf.part tools/sh-elf
	$(Q)rm -rf tools/sh-elf.src build/toolchain

# Pin the upstream headers, sources and license by their individual hashes.
# SH4ZAM may name a local checkout. Fetch any file it cannot supply.
SH4ZAM_AT := https://raw.githubusercontent.com/gyrovorbis/sh4zam/ff395477628e89bb403db16b17c4759cf4f788ac
# $(call copied,FROM,PATH) copies FROM to PATH where FROM holds the hash of
# PATH's one line in tools/SHA256SUMS, and fails, writing nothing, where it
# doesn't
copied = { sum=$$(awk '$$2 == "$2" { n++; sum = $$1 } END { if (n == 1) print sum }' tools/SHA256SUMS) && \
	[ -n "$$sum" ] && cp $(call quoted,$1) $2.part 2> /dev/null && \
	echo "$$sum  $2.part" | sha256sum -c --quiet --status - && mv $2.part $2; } || { rm -f $2.part; false; }
# DC_NEEDS asks no curl for a file SH4ZAM may give. Where SH4ZAM's copy of
# one fails, sh4zam_curl says that its fetch needs the curl the host lacks
sh4zam_curl = { command -v curl > /dev/null || { echo '$@: SH4ZAM holds no copy that matches its line of \
	tools/SHA256SUMS, and its fetch needs curl'; false; }; }
$(SH4ZAM_FILES): $(if $(UNFETCHED),FORCE)
	@$(call status,fetch,$@)
	$(Q)rm -f $(FETCHED) && mkdir -p $(@D) && $(if $(SH4ZAM),{ $(call copied,$(SH4ZAM)/$(@:tools/sh4zam/%=%),$@); } \
		|| ){ $(sh4zam_curl) && $(call fetch,$(SH4ZAM_AT)/$(@:tools/sh4zam/%=%),$@); }
# Each fetch removes FETCHED, which make writes only once every file is in
# place. After a run of fetches cut short, make fetches every file again.
# Once every file is in place, FETCHED's recipe checks each against its
# line, since one edited after the last fetch is newer than FETCHED and is
# no reason to write it. A file that fails leaves no FETCHED, so the next
# make fetches every file again. Then the recipe removes each other file or
# link under tools/sh4zam/, one an earlier pin listed, by its whole path,
# and writes FETCHED. A manifest that names no file of SH4ZAM's, as one
# that awk couldn't read gives, stops it before it removes any
$(FETCHED): $(SH4ZAM_FILES)
	$(if $(SH4ZAM_FILES),,$(error tools/SHA256SUMS: no line for a file under tools/sh4zam/, so make removes none))
	$(Q)rm -f $@ && { $(sh4zam_sums) | sha256sum -c --quiet - || \
		{ echo 'tools/sh4zam: a file differs from its line of tools/SHA256SUMS; make fetches every file again'; false; }; }
	$(Q)find tools/sh4zam ! -type d | awk 'NR == FNR { listed[$$2]; next } !($$0 in listed) && $$0 != "$@"' \
		tools/SHA256SUMS - | while IFS= read -r f; do rm -f "$$f" || exit 1; done
	$(Q)mkdir -p $(@D) && $(sh4zam_sums) > $@
