# make dc's composition of the Dreamcast port: the C that takes a source's
# place, the C an image adds, and the layout scripts the port uses. make
# reads it through dc.mk for DC_GOALS alone, and no SHC object depends on
# it. GCC compiles each C here under config/gcc.mk's GCCFLAGS, with the
# game's include directories and the root (PORTS.md#build-model)

# DC_REPLACED, the C that replaces a source, each as P:C, P a C source or a
# .src file an image places. C compiles to build/dc/P.o, the object P would
# compile or assemble to, which each image's census takes in P's place
# The retail point kernels use BSR to an adjacent C routine. GCC may place
# the routine outside its reach. The direct drawer's D8 loop also writes
# the caller's R8. These kernels call C through GCC's ABI.
DC_REPLACED := sdk/ninja/njDirectPointLoops.src:engine/dc/direct_points.c \
               sdk/ninja/njLine3DKernels.src:engine/dc/line.c
# The string helpers keep the retail signed-byte order and stop at NUL.
DC_REPLACED += sdk/runtime/__strcmp.src:engine/dc/strcmp.c

# GCC's objects do not share the original texture-copy fallthroughs.
DC_REPLACED += sdk/ninja/njTexMemCopy.src:engine/dc/texture_copy.c

# DC_ADDED, the C an image adds, each as IMAGE:C. C compiles to
# build/dc/C.o, which IMAGE's census takes after its sources'. The main
# executable adds the Dreamcast engine's kernels: a batch of points, and
# the turns the game's windows call (PORTS.md#kernels)
DC_ADDED := 1st_read:engine/dc/xmtrx.c 1st_read:engine/dc/turn.c
DC_ADDED += 1st_read:engine/dc/prs_checked.c
DC_ADDED += 1st_read:engine/dc/pvm.c
# The port's features: the toggles and their chord, each frame's end and
# the split frame, the readout and 16:9 (engine/dc/port.h).
DC_ADDED += 1st_read:engine/dc/port.c 1st_read:engine/dc/pace.c
DC_ADDED += 1st_read:engine/dc/readout.c 1st_read:engine/dc/wide.c
# The resident installs the crash screen and its vblank watchdog.
dc_crash_sources := engine/dc/crash.c engine/dc/crash_report.c engine/dc/crash_watch.c
DC_ADDED += $(addprefix 1st_read:,$(dc_crash_sources))
# MCWARN.PRS links a separate copy so it also works under a retail resident.
DC_ADDED += $(addprefix mcwarn:,$(dc_crash_sources))

# Downloaded kart models are checked against the SimpleDraw chunk format.
DC_ADDED += cart:engine/dc/cart_chunk.c

# The startup logo overlay shows the SH4ZAM splash in the GCC build.
DC_ADDED += otherprint:engine/dc/sh4zam_splash.c

# The resident startup stores the bounds of Sofdec's instruction sections.
DC_SOFDEC_LAYOUT := config/1st_read/sofdec.ld.S

# The terrain compiler starts beyond its stage's last occupied ELF section.
DC_TERRAIN_LAYOUT := config/terrain.ld
