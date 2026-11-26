# How each source compiles: its route. A route is a release of SHC, its
# flags, the directories it searches for headers and its sources, and ROUTES
# names them all. The flags reproduce the disc's bytes; a match doesn't show
# the options the game was built with. TOOLCHAIN.md#flags says what each flag
# does, and what shows it.

# The flags most routes open with. Every SHC compile defines DREAMCAST,
# one of the two switches a guard tests, and never NONMATCHING
# (PORTS.md#switches)
TARGET := -cpu=sh4 -endian=little -fpu=single -round=nearest -define=DREAMCAST

# The game's sources whose code the disc holds unoptimized
plain.release  := R13
plain.flags    := $(TARGET) -optimize=0 -size -macsave=0 -extra=a=1800
plain.includes := sdk/include,src,modules
plain.sources  := src/camera/mode_leave.c src/draw/debug_draw.c

# The unoptimized event model bursts multiply by 1/360 for NJM_DEG_ANG.
plain_aggressive.release  := R13
plain_aggressive.flags    := $(plain.flags) -aggressive=2
plain_aggressive.includes := $(plain.includes)
plain_aggressive.sources  := modules/event/model_burst.c

# The game's sources that match the disc only under -aggressive=2
aggressive.release  := R13
aggressive.flags    := $(TARGET) -speed -macsave=0 -extra=a=1800 -aggressive=2
aggressive.includes := sdk/include,src,modules
aggressive.sources  := src/system/util.c src/player/sonic.c

# The game's sources whose code the disc holds with no function inlined but
# those a #pragma inline names
noinline.release  := R13
noinline.flags    := $(TARGET) -speed -noinline -macsave=0 -extra=a=1800
noinline.includes := sdk/include,src,modules
noinline.sources  := src/sound/sound.c

# R13 schedules the setter's integer loop as on disc for SH-3E, whose
# integer instructions run unchanged on SH-4. asmsh takes the following
# three handwritten routines under the usual SH-4 options.
vertex_colors.release  := R13
vertex_colors.flags    := $(subst -cpu=sh4,-cpu=sh3e,$(TARGET)) -speed -macsave=0 -extra=a=1800
vertex_colors.includes := sdk/include,src,modules
vertex_colors.sources  := modules/shared/vertex_colors.c

# The rest of the game's own code: every other C source under src/ and
# modules/
game.release  := R13
game.flags    := $(TARGET) -speed -macsave=0 -extra=a=1800
game.includes := sdk/include,src,modules
game.sources  := $(filter-out $(plain.sources) $(plain_aggressive.sources) $(aggressive.sources) $(noinline.sources) $(vertex_colors.sources),$(shell find src modules -name '*.c' 2>/dev/null))

# Sofdec's video decoder members that match the disc under -speed, not the
# SDK's -size
mpvspeed.release  := R13
mpvspeed.flags    := $(TARGET) -speed -section=p=PSG,c=CSG,d=DSG,b=BSG
mpvspeed.includes := sdk/include
mpvspeed.sources  := sdk/sofdec/mpv_deli.c sdk/sofdec/mpv_err.c sdk/sofdec/mpv_frm.c sdk/sofdec/mpv_get.c

# The MPEG decoder library leaves its helpers out of line and its loops rolled.
mpvlib.release  := R13
mpvlib.flags    := $(mpvspeed.flags) -noinline -noloop -macsave=0
mpvlib.includes := $(mpvspeed.includes)
mpvlib.sources  := sdk/sofdec/mpv_lib.c sdk/sofdec/mpvadec.c

# The movie renderer's members, which the disc holds unoptimized
rnv2.release  := R13
rnv2.flags    := $(TARGET) -optimize=0 -macsave=0 -section=p=PSG,c=CSG,d=DSG,b=BSG
rnv2.includes := sdk/include
rnv2.sources  := sdk/middleware/rnv2mvf.c sdk/middleware/mwrnv2.c sdk/middleware/rnv2tex.c sdk/middleware/rnv2vctx.c sdk/middleware/rnv2vtx.c

# The SDK's libraries, whose code keeps the section names they give it
sdk.release  := R13
sdk.flags    := $(TARGET) -size -macsave=0 -section=p=PSG,c=CSG,d=DSG,b=BSG
sdk.includes := sdk/include
sdk.sources  := $(filter-out $(mpvspeed.sources) $(mpvlib.sources) $(rnv2.sources),$(shell find sdk -name '*.c' -not -path 'sdk/ninja/*' -not -path 'sdk/modem/*' -not -path 'sdk/runtime/*' -not -path 'sdk/backup/*' -not -path 'sdk/network/*' 2>/dev/null))

# The members of Kamui, the video driver that Ninja's library carries
kamui.release  := R13
kamui.flags    := $(TARGET) -speed -noloop -macsave=0 -extra=a=800 -section=p=PSG,c=CSG,d=DSG,b=BSG
kamui.includes := sdk/include
kamui.sources  := $(wildcard sdk/ninja/*_kamui_lib.c)

# The Ninja library's members, some of which R13 builds to other bytes than
# the disc's
ninja.release  := R11
ninja.flags    := $(sdk.flags)
ninja.includes := sdk/include
ninja.sources  := $(filter-out %_kamui_lib.c,$(shell find sdk/ninja -name '*.c' 2>/dev/null))

# The modem library's members, most of which R13 builds to other bytes than
# the disc's
modem.release  := R26
modem.flags    := $(sdk.flags) -extra=a=400
modem.includes := sdk/include,sdk/modem
modem.sources  := $(shell find sdk/modem -name '*.c' 2>/dev/null)

# The Network SDK's members, whose code keeps SHC's own section names
network.release  := R13
network.flags    := $(TARGET) -size -macsave=0
network.includes := sdk/include,sdk/network
# The Link Control Protocol member, which R08 builds whole to the disc's bytes
network_lcp.release  := R08
network_lcp.flags    := $(network.flags)
network_lcp.includes := $(network.includes)
network_lcp.sources  := sdk/network/ppp_lcp.c
# These ACCESS members place constant data beside the other SDK tables.
network_data.release  := $(network.release)
network_data.flags    := $(network.flags) -section=c=DATA
network_data.includes := $(network.includes)
network_data.sources  := sdk/network/ask_rom_data.c sdk/network/mdmapi.c sdk/network/ppp_fcs.c sdk/network/ppp_script.c
network.sources  := $(filter-out $(network_lcp.sources) $(network_data.sources),$(shell find sdk/network -name '*.c' 2>/dev/null))

# The backup library's members, unoptimized on the disc, whose code keeps
# SHC's own section names
backup.release  := R13
backup.flags    := $(TARGET) -optimize=0 -size -macsave=0
backup.includes := sdk/include,sdk/backup
backup.sources  := $(shell find sdk/backup -name '*.c' 2>/dev/null)

# The C runtime's members that work in double precision. _log10 and _xtostr
# set FPSCR's PR for their arithmetic
runtime64.release  := R13
runtime64.flags    := -cpu=sh4 -endian=little -define=DREAMCAST
runtime64.includes := sdk/include
runtime64.sources  := sdk/runtime/_duchek.c sdk/runtime/_log10.c sdk/runtime/_unpack.c sdk/runtime/_xti.c sdk/runtime/_xtostr.c

# The rest of the C runtime library, whose code lies among the game's in P
runtime.release  := R13
runtime.flags    := $(TARGET)
runtime.includes := sdk/include
runtime.sources  := $(filter-out $(runtime64.sources),$(shell find sdk/runtime -name '*.c' 2>/dev/null))

ROUTES := game vertex_colors plain plain_aggressive aggressive noinline sdk mpvspeed mpvlib rnv2 kamui ninja modem backup network network_lcp network_data runtime64 runtime

# The sources whose assembly build/shckit patches before asmsh assembles it,
# each with its patches, which shckit applies in its own order. Such a source
# compiles to assembly under its route. TOOLCHAIN.md#patched-sources says why
# each takes its patches
PATCHED := sdk/ninja/njControl.c
sdk/ninja/njControl.c.patches := align-pool

# The sources whose own text draws a warning from SHC, each with the
# warnings' numbers. Their compile passes those warnings, and fails on any
# other error or warning, and on a listed warning SHC no longer gives.
# TOOLCHAIN.md#warned-sources says why each text can't be written without its
# warnings
WARNED := sdk/ninja/njQuaternionEx.c
sdk/ninja/njQuaternionEx.c.warnings := 1016

# What R32's asmsh takes for every text it assembles: SHC's, for a C source
# that holds assembly, and each .src file. It takes the SH-4's FSCA and FSRRA
# only under -private, and -sjis lets a comment hold Shift JIS
ASMFLAGS := -cpu=sh4 -endian=little -sjis -private
