# make dc's compiler: GCC's and GNU as's flags for the tree's C, its
# assembly and SH4ZAM's library. make reads it through dc.mk, make dc's
# rules, for DC_GOALS alone, and no SHC object depends on it. Of SHC's flags
# for a source, config/compilers.mk's, GCC reads -optimize=0 alone

# GAS_CHECKS, which GNU as takes wherever it assembles: the .src files,
# SH4ZAM's assembly, and through GCC's -Wa, GCC's code and the inline
# bodies' text. GNU as takes SH-4's instructions alone, as asmsh does,
# where GCC's driver would ask it for SH-4A's, and fails on a warning, as
# the matching build fails asmsh on one
GAS_CHECKS := --isa=sh4 --fatal-warnings
# GNU as assembles the .src files little-endian, with no padding after a
# section's last byte, which asmsh doesn't write
GASFLAGS := $(GAS_CHECKS) --little --no-pad-sections
# SH4ZAM_INCLUDE, SH4ZAM's include directory, whose headers GCC reads as
# system headers
SH4ZAM_INCLUDE := tools/sh4zam/include

# GCC_TARGET, the target and the toolchain, which every compile takes. GCC
# writes code for little-endian SH-4 under one FPU model: a float passes in
# FR4 on, where SHC's code and the assembly take it, and a double is a
# float, as -fpu=single makes it for SHC on every route but runtime64. That
# route's five members work on a double of 8 bytes, which they spell long
# double, 8 bytes under GCC too (PORTS.md#flags-and-warnings). SH4ZAM's
# headers take its SH-4 back end, SHZ_SH4, only under a single-precision
# model, as this one is, and sdk/include/zam.h fails the compile otherwise.
# No -D pins SHZ_BACKEND: a pin would take that back end under any model
GCC_TARGET := -ml -m4-single-only
# GCC may compute a sine or cosine by FSCA and a reciprocal square root by
# FSRRA, the SH-4's approximations, only under unsafe math, which no compile
# takes: SH4ZAM's SHZ_FAST_MATH gives it to a function it marks
GCC_TARGET += -mfsca -mfsrra
# GCC reads the C as C17 with GNU's extensions. C23 reads a declaration
# with an empty list as one of no parameters: sdk/include/gdci.h's
# gdCiStopTr(), which gdci.c defines with one
GCC_TARGET += -std=gnu17
# GCC's own headers need no C library's, and GCC takes no library function
# for a builtin, so it folds no call of one. Its code may still call memcpy,
# memmove, memset or memcmp where the C doesn't: GCC requires those of a
# freestanding environment. SH4ZAM's headers include no threads.h, which
# neither the SDK nor GCC's own headers hold, and its one thread-local, the
# software back end's XMTRX, isn't built
GCC_TARGET += -ffreestanding
# GCC reads no host header: the route's include directories come first, as
# SHC searches them, then SH4ZAM's, which the non-matching code includes,
# and GCC's own. GCC's version is part of its own's path, so the shell
# expands a glob, as for dc.mk's LIBGCC. The glob names one directory:
# dc.mk stops make dc where SH_ELF holds more than one version of GCC
GCC_TARGET += -nostdinc -isystem $(SH4ZAM_INCLUDE) -idirafter $(SH_ELF)/lib/gcc/sh-elf/*/include
GCC_TARGET += $(GAS_CHECKS:%=-Wa,%)
# Each object keeps GCC's debugging information, DWARF, for a debugger. It
# writes no byte of the code or data
GCC_TARGET += -g
# DWARF names the checkout's directory as ., and a toolchain outside the
# checkout names its own include directory. The intermediate code GCC_LTO
# writes holds the working directory of the compile all the same, which no
# prefix map takes out, so make dc-check sums each object without it (dc.mk)
GCC_TARGET += $(call quoted,-fdebug-prefix-map=$(CURDIR)=.) -fno-working-directory
# make dc builds the non-matching code for the Dreamcast: it defines both
# switches a guard tests (PORTS.md#switches)
GCC_TARGET += -DNONMATCHING -DDREAMCAST
# The shipping build defines NDEBUG, so assert checks nothing, as SH4ZAM's
# documentation asks of a release build: its copies assert their arguments'
# alignment and size, and sdk/include/ninja_zam.h asserts that a matrix a
# caller hands SH4ZAM lies on 8. sdk/include/assert.h alone reads it
GCC_TARGET += -DNDEBUG
# GCC_ZEROS_IN_DATA, which each C object make dc compiles takes, keeps a
# datum the C initializes to zero among the data, as SHC keeps
# src/enemy/enemy.c's enemy_switch_bits in D
GCC_ZEROS_IN_DATA := -fno-zero-initialized-in-bss
# GCC_AS_SHC, the tree's C as SHC compiles it. An access may reach an object
# through another type, as sdk/runtime/_i_flors.c reads a float's sign
# through an int, and GCC, like SHC, doesn't assume that accesses of two
# types reach two objects
GCC_AS_SHC := -fno-strict-aliasing
# GCC takes no dereference as proof that its pointer isn't null, so it
# deletes no null test on the strength of one
GCC_AS_SHC += -fno-delete-null-pointer-checks
# Signed addition, subtraction and multiplication wrap, as SHC's do
GCC_AS_SHC += -fwrapv
# sqrtf, which sdk/include/mathf.h makes GCC's builtin, is FSQRT alone,
# with no call on its error path
GCC_AS_SHC += -fno-math-errno
# A product rounds before a sum takes it, but __builtin_fmaf's, which
# fuses. SHC fuses some products into sums by FMAC, and tools/fmacs.py
# lists each. GCC folds a constant expression to the float nearest its
# value, as SHC does under -round=nearest (TOOLCHAIN.md#c-for-shc), so no
# -frounding-math, which would leave 1.0F / 3.0F to an FDIV at run time
GCC_AS_SHC += -ffp-contract=off
# GCC_FUSING, which dc.mk gives a play source's compile after GCCFLAGS,
# once tools/floats.py has rewritten its preprocessed C. GCC then fuses a
# product into a sum by FMAC within one expression, where the product has
# the sum's type, and the pass leaves it only the products SHC fuses
# (PORTS.md#floats). The link-time compile keeps each function's own
# contraction
GCC_FUSING := -ffp-contract=on
# GCC_LAYOUT, how the tree's objects lay out what they hold. A structure's
# member aligns to at most 4, as under SHC. NJS_MATRIX aligns to 8 under
# GCC, as SH4ZAM's matrix does, so a structure that holds one keeps it on 8
# only where #pragma pack(8) wraps it (PORTS.md#shcs-c-under-gcc)
GCC_LAYOUT := -fpack-struct=4
# GCC_WARNINGS, which every compile takes. Each warning fails the compile.
# GCC warns, but not of these
# classes, which flag well-defined C, whose meaning is the same under either
# compiler: an unused name, braces left out, an address tested as true, a
# product's or a shift's truth, an enumerator no case names. Nor of a pragma
# GCC doesn't know: GCC may ignore SHC's #pragma inline, which steers SHC's
# inlining alone. #pragma section names the sections of the modem library's
# tables of constructors and destructors (TOOLCHAIN.md#the-modem-library),
# which GCC leaves in .data. test_make_dc.Pragmas holds that no other pragma
# GCC doesn't know reaches it
GCC_WARNINGS := -Wall -Wno-unused -Wno-missing-braces -Wno-address -Wno-int-in-bool-context -Wno-switch \
	-Wno-unknown-pragmas -Werror
# GCC_HOT, the sources GCC compiles at -O2: those whose game loops run each
# frame for each vertex or each particle. No profile of a frame ranks them,
# and one would add what it finds: the draws' share, and each scene's own.
# jiggle_step, util.c's, calls njSin for each vertex of a jiggle's wave
GCC_HOT := src/system/util.c
# particle_draw_all divides twice for each particle, and calls njCalcPoint,
# and njSin and njCos up to three times each
GCC_HOT += src/effect/particle.c
# $(call gcc_optimize,SOURCE) is SOURCE's level: -O2 for GCC_HOT's, -Os for
# the rest, whose smaller code leaves the images the most room in retail's
# memory map. Where SHC takes -optimize=0, dc.mk gives GCC -O0 after either
# level, so that GCC compiles that source unoptimized too
gcc_optimize = $(if $(filter $(GCC_HOT),$1),-O2,-Os)
# GCCFLAGS, GCC's flags for the tree's C, which dc.mk's gcc_flags follows
# with a source's level
GCCFLAGS = $(GCC_TARGET) $(GCC_ZEROS_IN_DATA) $(GCC_AS_SHC) $(GCC_LAYOUT) $(GCC_WARNINGS)
# GCC_SH4ZAM_FLAGS, SH4ZAM's library's flags, which take neither of the
# tree's sets: SHC never compiled the library, and its types keep the layout
# their headers declare. A warning on its sources' lines fails as on a
# game source's. Its C, complex and matrix math, compiles at -O2, as the hot
# set's does. Its C takes assert, memmove, INFINITY and isinf from the SDK's
# headers, which define each for GCC alone (PORTS.md#sh4zam-and-the-runtime)
GCC_SH4ZAM_FLAGS = $(GCC_TARGET) -O2 $(GCC_ZEROS_IN_DATA) $(GCC_WARNINGS) -Isdk/include
# GCC_LTO, which each C object make dc compiles takes, unless GCC_DATA
# lists its source: the object holds GCC's intermediate code beside its
# own code. Each image's census compiles that code whole
# (PORTS.md#link-time-optimization), and make dc-check sums the object
# without it. A split source's file-scope asm includes its bodies' text by
# a path from the root, where make runs every compile and link. GCC names
# the intermediate code's sections by a random seed, which dc.mk's
# gcc_flags gives as the source's path, so two builds in one checkout
# write one object
GCC_LTO := -flto -ffat-lto-objects
# GCC_DATA, the sources that hold a run of the disc's data alone, and the
# bank sources that a run joins, which hold data and no function: each in
# the disc's order. GCC compiles each outside the link-time compile, under
# GCC_DATA_FLAGS, so that an image keeps every datum of them, in the
# source's order within each section. Each datum still lies at its own
# alignment: a read past one's end meets the next only where GCC places no
# fill between them. The whole-program compile would drop each static that
# nothing names, and place the rest in an order of its own. A list names each
# source, a path a line: config/IMAGE/data_sources.txt where IMAGE alone
# places it, and config/data_sources.txt where several images do
GCC_DATA := $(foreach list,$(wildcard config/data_sources.txt config/*/data_sources.txt),$(file <$(list)))
# GCC_DATA_FLAGS, which a source of GCC_DATA takes in GCC_LTO's place.
# -fno-toplevel-reorder keeps each static that nothing names, which an
# optimizing compile drops otherwise, and emits each datum at its first
# definition, which each such source puts in the disc's order. Without
# -fdata-sections, the object keeps its data in one .data and its const
# data in one .rodata, and no link splits a section. That flag would give
# each datum a section named for it, and the census's relocatable link
# joins the sections of one name: STG12's static textures would lie with
# two other objects' data of that name
GCC_DATA_FLAGS := -fno-toplevel-reorder
# $(call gcc_link_time,SOURCE) is SOURCE's flags for the link-time compile:
# GCC_LTO, which takes its object in, or for a source of GCC_DATA,
# GCC_DATA_FLAGS, which keep it out
gcc_link_time = $(if $(filter $(GCC_DATA),$1),$(GCC_DATA_FLAGS),$(GCC_LTO))
# GCC_LTO_LINK, that compile's flags. GCC keeps each function's level, its
# float contraction and SHC's semantics from its object, and the target
# from every object, but takes these from the compile's command line alone:
# one object for each object's code, each function and each datum in a
# section of its own, which a link places by name, and the zeros in data,
# as GCC_ZEROS_IN_DATA keeps them. The level gives the whole program's
# passes, as inlining across objects, and the code holds no DWARF.
#
# GCC also runs passes over the whole program, and one of them folds
# arithmetic: where a function passes on an argument times a constant and
# every caller gives it a constant, GCC computes the product as it
# compiles. The line takes GCC_AS_SHC, so that those passes run under SHC's
# semantics whichever options GCC carries to them from the objects. The
# line's -m options are GCC_TARGET's, so the code is for the target each
# object's is. GNU as assembles the code under GAS_CHECKS, as it assembles
# each object's. -fuse-linker-plugin fails the compile in a toolchain that
# holds no plugin for the linker, through which the linker tells GCC which
# names the other inputs take. -Werror fails it on a warning of GCC's, as
# GCC_WARNINGS fails each object's compile, a declaration of a name with
# another type than its definition's among them (-Wlto-type-mismatch)
# DC_LTO_JOBS is jobserver, the default, or a count of the link's LTO
# workers. Under jobserver, dc.mk runs the link as a recursive recipe, and
# GCC 16 takes its workers from make's jobserver even with -flto=1. One
# worker is the fallback where make runs serially or has no jobserver. A
# count is the link's own: dc.mk clears MAKEFLAGS, so GCC sees no jobserver,
# and DC_LTO_JOBS=1 links with one worker under any -j
DC_LTO_JOBS ?= jobserver
GCC_LTO_WORKERS = $(if $(filter jobserver,$(DC_LTO_JOBS)),1,$(DC_LTO_JOBS))
GCC_LTO_LINK = $(filter -m%,$(GCC_TARGET)) -O2 -flto=$(GCC_LTO_WORKERS) -flto-partition=1to1 -fuse-linker-plugin \
	-ffunction-sections -fdata-sections $(GCC_ZEROS_IN_DATA) $(GCC_AS_SHC) $(GAS_CHECKS:%=-Wa,%) -Werror -g0

# CONTRACTS, the findings of tools/tags.py --abi that a contract answers,
# each as SOURCE:NAME:KEY: the source, the function as C spells it, and the
# finding's key, the register it reads or changes, FPSCR's bit, or abi for
# a .src function whose @asm tag is abi. --abi fails on each finding no row
# names. The reason beside each says how the function's GCC callers meet
# its contract, or that no GCC code calls it (PORTS.md#inline-assembly).
# $(call contracts,SOURCE) is SOURCE's NAME:KEY pairs, which dc.mk hands
# --abi.
#
# longjmp returns to no caller. It goes on past the setjmp that filled env,
# on that setjmp's stack, or in a new fiber's entry on the fiber's own
# stack, as buFbrCreateFiber writes its context. Its GCC callers, rfiber.c's,
# take setjmp as returning twice, so they keep what lives across it in
# their frames
CONTRACTS := sdk/runtime/longjmp.src:longjmp:abi
# quick_strcpy1 pops the R3 and R4 its two ways in pushed: __quick_strcpy,
# which SHC's code alone calls, and _slow_strcpy's jump. machine.h's strcpy
# calls _slow_strcpy by an asm that passes s1 in R0 and s2 in R1, and takes
# R0, R1, PR and T as changed. quick_strcpy1 keeps every other register
CONTRACTS += sdk/runtime/__q_scpy.src:quick_strcpy1:abi
# No GCC code calls these. syIntInit and syChainDeleteHandler store
# syIntBrkRteRte's address in the vector table, through which the masters
# call it with an event's code in R4, and it leaves the event itself.
# syIntInit stores the masters' addresses for the stubs it copies to VBR,
# which the processor enters
CONTRACTS += sdk/shinobi/syIntChkIsr.src:syIntBrkRteRte:abi sdk/shinobi/syIntIsr.src:syIntMasterISR:abi \
	sdk/shinobi/syIntIsr.src:syIntMasterISR_2:abi
# matrix_multiply_before returns with FPSCR.FR flipped, as SHC's expansion
# does: its FRCHG makes the four products XMTRX. It pops FR12 to FR15 into
# the bank FR then names, so a GCC caller finds what it kept there. GCC's
# code names its registers by FR's bank, and keeps FR as it finds it
CONTRACTS += $(foreach k,FR FR12 FR13 FR14 FR15,src/system/matrix.c:matrix_multiply_before:$k)
# Each multi-light vertex body's first FIPR, of a light's direction and the
# normal, takes FR7 times the 0 FLDI0 has put in FR11, which adds nothing
# for a finite FR7. No caller passes an argument there, under SHC's
# convention or GCC's. A call's first light reads the FR7 the code ahead of
# it left. njCnkEasyMultiVlist and njCnkSimpleMultiVlist compute no float,
# so that is the last body's 0 or squared distance to a point light, or
# what the drawing before the list computed. Each later light reads the
# body's own
CONTRACTS += $(foreach f,EasyMultiVertexVN EasyMultiVertexVND8 EasyMultiVertexVND8DepthQueue \
	EasyMultiVertexVNDepthQueue EasyMultiVertexVNNF EasyMultiVertexVNNFDepthQueue SimpleMultiVertexVN \
	SimpleMultiVertexVND8 SimpleMultiVertexVND8DepthQueue SimpleMultiVertexVNDepthQueue SimpleMultiVertexVNNF \
	SimpleMultiVertexVNNFDepthQueue,sdk/ninja/njCnk$f.c:njCnk$f:FR7)
# Each 3D drawer's pair store writes a register that its calls may change,
# under SHC's convention as under GCC's: FR5 as the last word of a polygon's
# vertex, and FR7 as a textured vertex's offset color. The shipped code
# writes either as its callees leave it, and each colors the picture alone
CONTRACTS += $(foreach f,njDrawPolygon3DEx njDrawPolygon3DExSetData,sdk/ninja/njDrawPolygon3.c:$f:FR5) \
	$(foreach f,njDrawTexture3DEx njDrawTexture3DExSetData,sdk/ninja/njDrawTexture3.c:$f:FR7)
# cmSwitchContext saves and restores no FPU register. Its three contexts,
# network_vblank's and the PPP and TCP/IP daemons', all run inside the
# vertical-blank interrupt, whose master saves the interrupted code's FPU
# registers, FPSCR and FPUL (syIntIsr.src). No frame of the three holds a
# float at a switch. network_vblank, syChain.c's chain that calls it, and
# the daemons' loops hold integers. The network, ACCESS and modem libraries
# compute floats only in
# uart_set_baud and scif_set_baud, and neither reaches a switch
CONTRACTS += sdk/network/cmswitch.src:cmSwitchContext:abi
contracts = $(patsubst $1:%,%,$(filter $1:%,$(CONTRACTS)))
