/* Ninja's system: the display, Kamui's setup and its vertex buffer, and the
 * current vertex context's state. it also renders and shows each frame */
#include <kamui.h>
#include <kamui_internal.h>
#include <machine.h>
#include <ninja.h>
#include <nj_global.h>
#include <nj_km.h>
#include <nj_light_state.h>
#include <sg_chain.h>
#include <sg_syg2.h>
#include <sg_xpt.h>
#include <shinobi.h>
#ifdef NONMATCHING
/* this side's copy_light_defaults copies with memcpy */
#include <string.h>
#endif

/* how many vertex contexts _nj_context_ holds */
#define CONTEXT_COUNT 6

/* _nj_SystemConfigStruct_ is the setup Ninja gives kmSetSystemConfiguration,
 * and _nj_ppSurfaceArray points to its frame buffers' surfaces */
extern KMSYSTEMCONFIGSTRUCT _nj_SystemConfigStruct_;
extern PKMSURFACEDESC _nj_ppSurfaceArray[2];
extern NJS_TEXSURFACE _nj_framebuffer_surface0_;
extern NJS_TEXSURFACE _nj_framebuffer_surface1_;
/* the texture memory's size in bytes, which the setup takes */
extern Uint32 _nj_texture_memory_size;
/* the vertex buffer njSetVertexBuffer gave, by its P2 address, and its size
 * in bytes. the address is null before any call */
extern Uint32 *_nj_usr_vertex_buffer_ptr_;
extern Uint32 _nj_usr_vertex_buffer_size_;
/* the words njInitVertexBuffer last gave each list of the vertex buffer */
extern Sint32 _nj_vertex_buffer_op_; /* opaque polygons */
extern Sint32 _nj_vertex_buffer_om_; /* opaque modifiers */
extern Sint32 _nj_vertex_buffer_tp_; /* translucent polygons */
extern Sint32 _nj_vertex_buffer_tm_; /* translucent modifiers */
extern Sint32 _nj_vertex_buffer_pt_; /* punch-through polygons */
/* the five lists' words together, which njInitSystem zeroes */
extern Uint32 _nj_vertex_buffer_size_;
/* each list's pCurrentPtr entry in _nj_vertex_buffer_, as njWaitVSync or
 * njFlipFrameBuffer last left it, by the same suffixes. njGetSystemInfo
 * measures each list's bytes from there */
extern Uint32 _nj_start_vertex_op;
extern Uint32 _nj_start_vertex_om;
extern Uint32 _nj_start_vertex_tp;
extern Uint32 _nj_start_vertex_tm;
extern Uint32 _nj_start_vertex_pt;
/* the render's state. njInitSystem and njChangeSystem set _nj_eov_flag_ to
 * TRUE, the other two flags to FALSE and _nj_fatal_err_callback_ to 0 */
extern volatile Bool _nj_render_flag_; /* TRUE once a render has ended */
/* FALSE once Kamui's vertices have ended */
extern volatile Bool _nj_eov_flag_;
/* the argument of Kamui's last fatal error callback, or 0 */
extern volatile Int _nj_fatal_err_callback_;
/* the functions njSetEORFunction and njSetEORCallBackFunction set, or null */
extern void (*_nj_eor_function_)(void);
extern void (*_nj_eor_function2_)(void);
extern Int _nj_display_mode_; /* the display mode njInitSystem took */
extern Int _nj_frame_mode_; /* the frame buffer's format */
extern Int _nj_waitvsync_count; /* the vertical blanks to a frame */
/* the frame buffer's width and height in pixels */
extern Int _nj_frame_size_w;
extern Int _nj_frame_size_h;
/* the NJD_PALEXT_HEIGHT_RATIO_ field of the display mode, moved down to bit
 * 0, which njPalExtCallbackFunc gives Kamui */
/* @hack type: _nj_palext_height_ratio_ volatile, where a plain Int would do */
extern volatile Int _nj_palext_height_ratio_;
/* each vertex context's color mode */
extern Uint32 _nj_ctx_color_mode_[CONTEXT_COUNT];
/* the background plane's context */
extern KMVERTEXCONTEXT _nj_background_ctx_;
/* syTmrGetCount's count where njWaitVSync or njFlipFrameBuffer last began */
extern Uint32 _nj_wait_vsync_in_;
/* syTmrGetCount's count where the same call last ended */
extern Uint32 _nj_wait_vsync_out_;
/* the frames njWaitVSync and njFlipFrameBuffer have shown since
 * njInitSystem */
extern Uint32 _nj_vsync_count_;
/* the current texture and its bank, as njGetSystemAttr last saved them */
extern Uint32 _nj_current_texture_back_;
extern Uint32 _nj_current_texture_bank_back_;
/* the frame buffers' P2 addresses, as njGetSystemInfo last read them */
extern Uint32 _nj_frame_addr_0_;
extern Uint32 _nj_frame_addr_1_;

/* the member exports these functions and data, though no header of the SDK
 * declares them */
/* NOLINTBEGIN(misc-use-internal-linkage) */
/* Kamui's end of render, end of vertices and fatal error callbacks */
void njSetEORflag(void *arg);
void njSetEOVflag(void *arg);
void njSetERRflag(void *arg);
/* a vertical blank handler, which nothing in the image puts on the chain */
void njBlank(void *arg);
/* wait until the video output leaves scan line 0, then until it returns to
 * it */
void waitVsync(void);
/* set *empty to TRUE where FIFO_STATUS's FIFO_SH4 bit is clear, else FALSE */
void G2FifoIsEmpty(Bool *empty);
/* set AICA_VIDEO_CABLE to cable, a SYE_CBL_CABLE_ value shifted up by
 * CABLE_SHIFT. any other bit cable holds is set in the register too. the write
 * runs with every interrupt masked, once G2FifoIsEmpty finds the FIFO empty. a
 * G2 DMA channel that runs at the call is resumed before the wait. it is left
 * suspended after the write */
void njSetVideoMode(Uint32 cable);
/* set AICA_VIDEO_CABLE to cable, as njSetVideoMode does, but read and write
 * the register through syG2Read and syG2Write alone */
void njSetVideoModeChange(Uint32 cable);
/* Kamui's PAL stretch callback. arg is the KMPALEXTINFO Kamui passes */
void njPalExtCallbackFunc(void *arg);
/* set Ninja's drawing state to its defaults for the screen njInitSystem set
 * up, and make context 0 current */
void njInitGlobal(void);
/* zero the vertex and polygon counts. fit the screen's center and the 2D clip
 * to the screen njChangeSystem set up */
void njChangeGlobal(void);
/* make _nj_context_[n] the current context, with its color mode, and set it
 * through kmSetVertexRenderState */
void njSetCurrentContext(Int n);
/* give context Ninja's default state, and process it through
 * kmProcessVertexRenderState */
void njInitContext(KMVERTEXCONTEXT *context);
/* call the function njSetEORFunction set, if any */
void njExeEORFunction(void);
/* set *func and *arg to entry no of Kamui's callback table */
void njGetKmCallbackFunction(Int no, PKMCALLBACKFUNC *func, void **arg);
/* set *func and *arg to those njSetWaitVSyncFunction last set */
void njGetWaitVSyncFunction(PKMCALLBACKFUNC *func, void **arg);
/* make mode the current context's color mode, and set the context through
 * kmSetVertexRenderState. mode's COLOR_MODE_GLOBAL and COLOR_MODE_TSP bits go
 * into the context's global and TSP words. a mode the context has already
 * changes nothing */
void njSetColorMode(Uint32 mode);
/* the current context's color mode */
Uint32 njGetColorMode(void);
/* make the current context's face color face, and its offset color offset.
 * the call then sets the context through kmSetVertexRenderState */
void njSetIntensityColor(NJS_ARGB *face, NJS_ARGB *offset);
char *_nj_copyright = "SEGA Ninja Library";
char *version = "\nNinja Ver 01100114 Build:Jul 03 2000 18:12:08\n";
Uint32 _nj_version = 0x01100114;
char *_nj_mmu = "MMU OFF  ";
/* the vertical blanks njBlank has counted */
volatile Int _nj_blank_count_ = 0;
/* the list njInitVertexBuffer last left without a buffer. NOBUF_OPAQUE is the
 * opaque list, and NOBUF_TRANS the translucent or the punch-through list.
 * NOBUF_NONE says every list has a buffer */
#define NOBUF_NONE 0
#define NOBUF_OPAQUE 1
#define NOBUF_TRANS 2
Int _nj_vertex_2v_type_ = NOBUF_NONE;
/* the start of main memory, in P1 */
#define RAM_P1 0x8c000000
/* the code njInitSystem calls where the system bus's revision is below
 * BUS_REVISION_MIN */
void (*_NJ_GO_TO_LOOP)(void) = (void (*)(void))RAM_P1;
/* a G2 DMA channel's start register holds G2_DMA_START while the channel
 * runs. its suspend register takes G2_DMA_SUSPEND to suspend the channel, and
 * G2_DMA_RESUME to resume it */
#define G2_DMA_START 1
#define G2_DMA_SUSPEND 1
#define G2_DMA_RESUME 0
/* the SY_G2_DMA_NUM channels' registers, the first at G2_DMA_ADDRESS */
#define G2_DMA_ADDRESS 0xa05f7800
volatile struct sy_g2_dma_regs *_nj_img =
    (volatile struct sy_g2_dma_regs *)G2_DMA_ADDRESS;
/* NOLINTEND(misc-use-internal-linkage) */

/* the handle of njSetVSyncFunction's function on the vertical blank chain,
 * or 0 */
static SYCHAIN vsync_handle;
/* the handle njBlank deletes from the vertical blank chain. nothing sets
 * it */
static SYCHAIN blank_handle;
/* 32 KB of the member's bss, which nothing reads or writes */
/* NOLINTNEXTLINE(clang-diagnostic-unused-variable) */
static Uint8 unused[0x8000];
/* the width and height of the tiles njUserClipping takes, in pixels */
#define TILE_SIZE 32.0F
/* njUserClipping's last rectangle, as its first tile and its last. it starts
 * as the whole 640 by 480 screen */
static NJS_POINT2 last_rect[2] = {
#include "ninja/njSystem/last_rect.inc"
};

/* the video output's status. SPG_STATUS_LINE holds the scan line it draws */
#define SPG_STATUS (*(volatile Uint32 *)0xa05f810c)
#define SPG_STATUS_LINE 0x3ff
/* the system bus's FIFO status. FIFO_SH4 is the bit of the SH-4's FIFO onto
 * the bus, which G2FifoIsEmpty tests */
#define FIFO_STATUS (*(volatile Uint32 *)0xa05f688c)
#define FIFO_SH4 0x20
/* the system bus's revision, in BUS_REVISION_MASK. Ninja needs
 * BUS_REVISION_MIN or above */
#define BUS_REVISION (*(volatile Uint32 *)0xa05f689c)
#define BUS_REVISION_MASK 0xff
#define BUS_REVISION_MIN 10
/* the video output's control. setting KMI_VIDEO_BLANK blanks the picture */
#define VIDEO_CONTROL (*(volatile Uint32 *)0xa05f80e8)
/* the system bus's normal interrupt status. INT_VBLANK_IN is set as a
 * vertical blank starts, and writing the bit clears it */
#define INT_STATUS (*(volatile Uint32 *)0xa05f6900)
#define INT_VBLANK_IN 8
/* an AICA register on the G2 bus whose AICA_VIDEO_CABLE bits hold the video
 * cable's type. CABLE_SHIFT moves a SYE_CBL_CABLE_ value into them */
#define AICA_VIDEO_ADDRESS 0xa0702c00
#define AICA_VIDEO (*(volatile Uint32 *)AICA_VIDEO_ADDRESS)
#define AICA_VIDEO_CABLE 0x300
#define CABLE_SHIFT 8
/* DSP_VIDEO holds a display mode's video signal, a KMVIDEOMODE. NJD_DSP_MASK
 * holds the bits of a display mode that kmSetDisplayMode takes as its own */
#define DSP_VIDEO 3
#define NJD_DSP_MASK 0xfff
/* a display mode's PAL stretch, an NJD_PALEXT_HEIGHT_RATIO_ value. the shift
 * moves it down to bit 0 */
#define PALEXT_RATIO 0x000e0000
#define PALEXT_RATIO_SHIFT 17
/* the border njInitSystem shows before it calls _NJ_GO_TO_LOOP */
#define BORDER_ORANGE 0xffff6600
/* the priority of njSetVSyncFunction's function on the vertical blank
 * chain */
#define VBLANK_PRIORITY 88
/* the words njInitVertexBuffer counts for a list with no buffer */
#define NOBUF_WORDS 100
/* the words Kamui's vertex buffer takes beyond its lists' words */
#define BUFFER_EXTRA 88
/* ORing P2_BASE or P1_TO_P2 into a cached P1 address gives its P2 address.
 * ANDing a P2 address with P2_TO_P1 gives its P1 address */
#define P1_TO_P2 0x20000000
#define P2_TO_P1 0x8fffffff
/* video memory's 32-bit view, through P2 */
#define VIDEO_MEMORY_P2 0xa5000000
/* the limit on njWaitVSync's wait for the last render to end, in the counts
 * syTmrMicroToCount returns */
#define RENDER_WAIT_COUNT 166000
/* the microseconds in one of syTmrGetCount's counts */
#define MICROSECONDS_PER_COUNT 1.28F
/* the 1/w njSetBackColor gives the background plane's corners */
#define BACKGROUND_DEPTH 0.00001F
/* the culling register's value njInitGlobal sets */
#define CULLING_SIZE 0.01F
/* the floats njInitGlobal gives each multi drawer's light buffer and the two
 * arrays after it, _mdst_ and _dst_. the buffer takes MULTILIGHT_FLOATS. _mdst_
 * takes MDST_FLOATS, the mode word 0 and the ambient color 1.0. _dst_ takes
 * DST_FLOATS, lights 1 to 3 */
#define MDST_FLOATS 4
#define DST_FLOATS 24
#define LIGHT_DEFAULTS (MULTILIGHT_FLOATS + MDST_FLOATS + DST_FLOATS)
/* the float of a light buffer that holds light 6's y, which njInitGlobal
 * gives the integer 3 */
#define LIGHT_6_Y 16
#define LIGHT_VALUES                                                             \
    {                                                                            \
        0.0F, 0.0F, -1.0F, 0.0F, 0.0F,     0.0F, 0.0F,     0.0F, 0.0F,     0.0F, \
        0.0F, 0.0F, 0.0F,  0.2F, 0.2F,     0.2F, 0.0F,     0.0F, 0.0F,     1.0F, \
        0.0F, 1.0F, 1.0F,  1.0F, 0.0F,     0.0F, 0.0F,     1.0F, 65535.0F, 1.0F, \
        1.0F, 1.0F, 0.0F,  0.0F, 0.0F,     1.0F, 65535.0F, 1.0F, 1.0F,     1.0F, \
        0.0F, 0.0F, 0.0F,  1.0F, 65535.0F, 1.0F, 1.0F,     1.0F,                 \
    }

#ifdef NONMATCHING
/* The three SDK exports occupy one consecutive range in the shipped image.
 * Copy each object separately so the C accesses stay within its bounds. */
static void copy_light_defaults(Float *vectors,
                                struct nj_multilight_parameters *parameters,
                                Float *output)
{
    static const Float defaults[LIGHT_DEFAULTS] = LIGHT_VALUES;
    Uint32 mode = 3;
    memcpy(vectors, defaults, MULTILIGHT_FLOATS * sizeof(*vectors));
    memcpy(parameters, defaults + MULTILIGHT_FLOATS, sizeof(*parameters));
    memcpy(output, defaults + MULTILIGHT_FLOATS + MDST_FLOATS,
           DST_FLOATS * sizeof(*output));
    memcpy(&vectors[LIGHT_6_Y], &mode, sizeof(mode));
}
#define COPY_LIGHT_DEFAULTS(buffer, parameters, output)                        \
    copy_light_defaults(buffer, parameters, output)
#else
/* The loop starts at the vectors and ends in the third transformed light. */
#define COPY_LIGHT_DEFAULTS(buffer, parameters, output)                        \
    {                                                                          \
        Float lights[LIGHT_DEFAULTS] = LIGHT_VALUES;                           \
        for (i = 0; i < LIGHT_DEFAULTS; i++) {                                 \
            (buffer)[i] = lights[i];                                           \
        }                                                                      \
        *(Sint32 *)&(buffer)[LIGHT_6_Y] = 3;                                   \
    }
#endif

/* set bpp to the KMBPPMODE of frame, an NJD_FRAMEBUFFER_MODE_. a frame
 * buffer format no case lists leaves bpp as it was */
#define FRAME_BPP(bpp, frame)                                                  \
    switch (frame) {                                                           \
    case NJD_FRAMEBUFFER_MODE_RGB565:                                          \
        (bpp) = KM_DSPBPP_RGB565;                                              \
        break;                                                                 \
    case NJD_FRAMEBUFFER_MODE_RGB555:                                          \
        (bpp) = KM_DSPBPP_RGB555;                                              \
        break;                                                                 \
    case NJD_FRAMEBUFFER_MODE_ARGB1555:                                        \
        (bpp) = KM_DSPBPP_ARGB1555;                                            \
        break;                                                                 \
    case NJD_FRAMEBUFFER_MODE_RGB888:                                          \
        (bpp) = KM_DSPBPP_RGB888;                                              \
        break;                                                                 \
    case NJD_FRAMEBUFFER_MODE_ARGB8888:                                        \
        (bpp) = KM_DSPBPP_ARGB8888;                                            \
        break;                                                                 \
    }
/* stretch _nj_frame_size_h, 240 or 480 lines, by mode's PALEXT_RATIO. a
 * ratio no case lists leaves it as it was */
#define PALEXT_FRAME_HEIGHT(mode)                                              \
    if (_nj_frame_size_h == 240) {                                             \
        switch ((mode) & PALEXT_RATIO) {                                       \
        case NJD_PALEXT_HEIGHT_RATIO_1_033:                                    \
            _nj_frame_size_h = 248;                                            \
            break;                                                             \
        case NJD_PALEXT_HEIGHT_RATIO_1_066:                                    \
            _nj_frame_size_h = 256;                                            \
            break;                                                             \
        case NJD_PALEXT_HEIGHT_RATIO_1_100:                                    \
            _nj_frame_size_h = 264;                                            \
            break;                                                             \
        case NJD_PALEXT_HEIGHT_RATIO_1_133:                                    \
            _nj_frame_size_h = 272;                                            \
            break;                                                             \
        case NJD_PALEXT_HEIGHT_RATIO_1_166:                                    \
            _nj_frame_size_h = 280;                                            \
            break;                                                             \
        }                                                                      \
    } else {                                                                   \
        switch ((mode) & PALEXT_RATIO) {                                       \
        case NJD_PALEXT_HEIGHT_RATIO_1_033:                                    \
            _nj_frame_size_h = 496;                                            \
            break;                                                             \
        case NJD_PALEXT_HEIGHT_RATIO_1_066:                                    \
            _nj_frame_size_h = 512;                                            \
            break;                                                             \
        case NJD_PALEXT_HEIGHT_RATIO_1_100:                                    \
            _nj_frame_size_h = 528;                                            \
            break;                                                             \
        case NJD_PALEXT_HEIGHT_RATIO_1_133:                                    \
            _nj_frame_size_h = 544;                                            \
            break;                                                             \
        case NJD_PALEXT_HEIGHT_RATIO_1_166:                                    \
            _nj_frame_size_h = 560;                                            \
            break;                                                             \
        }                                                                      \
    }

void njSetTextureMemorySize(Uint32 size)
{
    _nj_texture_memory_size = size;
}

void njChangeTextureMemorySize(Uint32 size)
{
    _nj_texture_memory_size = size;
    /* @hack spelling: the size read back from _nj_texture_memory_size, where
     * size would do */
    _nj_SystemConfigStruct_.nTextureMemorySize = _nj_texture_memory_size;
    kmSetSystemConfiguration(&_nj_SystemConfigStruct_);
    _nj_tex_max_free_size = njCalcTexture(NJD_TEXMEM_FREESIZE);
}

/* NOLINTNEXTLINE(readability-non-const-parameter) */
void njSetVertexBuffer(Uint32 *buffer, Sint32 size)
{
    _nj_usr_vertex_buffer_ptr_ = (Uint32 *)((Uint32)buffer | P2_BASE);
    _nj_usr_vertex_buffer_size_ = size;
}

void njSetEORflag(void *arg)
{
    _nj_eor_flag_ = FALSE;
    _nj_render_flag_ = TRUE;
    if (_nj_eor_function2_) {
        _nj_eor_function2_();
    }
}

void njSetEOVflag(void *arg)
{
    _nj_eov_flag_ = FALSE;
}

void njSetERRflag(void *arg)
{
    _nj_fatal_err_callback_ = (Int)arg;
}

void njBlank(void *arg)
{
    if (_nj_render_flag_) {
        syChainDeleteHandler(blank_handle);
    }
    _nj_blank_count_++;
}

void waitVsync(void)
{
    while (!(SPG_STATUS & SPG_STATUS_LINE)) {
    }
    while (SPG_STATUS & SPG_STATUS_LINE) {
    }
}

void G2FifoIsEmpty(Bool *empty)
{
    /* @hack spelling: the test's result through ? FALSE : TRUE, where == 0
     * would do */
    *empty = (FIFO_STATUS & FIFO_SH4) ? FALSE : TRUE;
}

void njSetVideoMode(Uint32 cable)
{
    Bool empty;
    Int mask;
    Int i;
    Bool running[SY_G2_DMA_NUM];
    Uint32 value;

    empty = FALSE;
    mask = get_imask();
    set_imask(IMASK_ALL);
    for (i = 0; i < SY_G2_DMA_NUM; i++) {
        if (_nj_img[i].start & G2_DMA_START) {
            _nj_img[i].suspend = G2_DMA_RESUME;
            running[i] = TRUE;
        } else {
            running[i] = FALSE;
        }
    }
    while (empty != TRUE) {
        G2FifoIsEmpty(&empty);
    }
    /* @hack spelling: the register staged through value, where one
     * expression would do */
    value = AICA_VIDEO;
    value = (value & ~AICA_VIDEO_CABLE) | cable;
    AICA_VIDEO = value;
    for (i = 0; i < SY_G2_DMA_NUM; i++) {
        /* @hack spelling: the flag tested against TRUE, where the flag alone
         * would do */
        if (running[i] == TRUE) {
            /* @bug logic: a G2 DMA channel that runs at the call stays
             * suspended after it. the call resumes the channel before the
             * wait, MOV.L R13,@(28,R2) at 0x8c111aba, and suspends it here,
             * MOV.L R14,@(28,R3) at 0x8c111b50. nothing resumes it after */
            _nj_img[i].suspend = G2_DMA_SUSPEND;
        }
    }
    set_imask(mask);
}

void njSetVideoModeChange(Uint32 cable)
{
    Uint32 value;

    syG2Read((void *)AICA_VIDEO_ADDRESS, &value, SYE_G2_DATA_WIDTH_32BIT, 1,
             SYE_G2_ADR_MODE_FIX, SYE_G2_ADR_MODE_FIX);
    value = (value & ~AICA_VIDEO_CABLE) | cable;
    syG2Write(&value, (void *)AICA_VIDEO_ADDRESS, SYE_G2_DATA_WIDTH_32BIT, 1,
              SYE_G2_ADR_MODE_FIX, SYE_G2_ADR_MODE_FIX);
}

void njPalExtCallbackFunc(void *arg)
{
    ((PKMPALEXTINFO)arg)->nPALExtMode = _nj_palext_height_ratio_;
}

void njInitSystem(Int mode, Int frame, Int count)
{
    KMVIDEOMODE video;
    Int display;
    SYE_CBL_CABLE cable;
    KMBPPMODE bpp;
    KMBOOLEAN anti_alias;

    _nj_display_mode_ = mode;
    _nj_frame_mode_ = frame;
    _nj_waitvsync_count = count;
    video = mode & DSP_VIDEO;
    display = mode & NJD_DSP_MASK;
    if (mode & NJD_DSP_WIDTH_640) {
        _nj_screen_.w = 640.0F;
        _nj_frame_size_w = 640;
    } else {
        _nj_screen_.w = 320.0F;
        _nj_frame_size_w = 320;
    }
    if (mode & NJD_DSP_HEIGHT_480) {
        _nj_screen_.h = 480.0F;
        _nj_frame_size_h = 480;
    } else {
        _nj_screen_.h = 240.0F;
        _nj_frame_size_h = 240;
    }
    if (video == KM_VGA) {
        waitVsync();
        njSetVideoMode(SYE_CBL_CABLE_VGA << CABLE_SHIFT);
        _nj_screen_.w = 640.0F;
        _nj_frame_size_w = 640;
        _nj_screen_.h = 480.0F;
        _nj_frame_size_h = 480;
    } else {
        cable = syCblCheckCable();
        waitVsync();
        /* @hack spelling: SYE_CBL_CABLE_RESERVED's case last, where the cases
         * in their values' order would do */
        switch (cable) {
        case SYE_CBL_CABLE_RGB:
            njSetVideoMode(SYE_CBL_CABLE_RGB << CABLE_SHIFT);
            break;
        case SYE_CBL_CABLE_VBS:
            njSetVideoMode(SYE_CBL_CABLE_VBS << CABLE_SHIFT);
            break;
        case SYE_CBL_CABLE_RESERVED:
            njSetVideoMode(SYE_CBL_CABLE_RESERVED << CABLE_SHIFT);
            break;
        default:
            njSetVideoMode(SYE_CBL_CABLE_VBS << CABLE_SHIFT);
            break;
        }
    }
    if (mode & NJD_DSP_ANTI) {
        _nj_screen_.w *= 2.0F;
        anti_alias = TRUE;
        if (_nj_screen_.h == 240.0F) {
            _nj_screen_.h = 480.0F;
        }
    } else {
        anti_alias = FALSE;
    }
#ifdef NONMATCHING
    /* a frame buffer format no case lists leaves bpp unset, and the image
     * passes what a register held. this side passes -1 */
    bpp = (KMBPPMODE)-1;
#endif
    FRAME_BPP(bpp, frame)
    /* @hack spelling: the or written out, where |= would do */
    VIDEO_CONTROL = VIDEO_CONTROL | KMI_VIDEO_BLANK;
    INT_STATUS = INT_VBLANK_IN;
    while (!(INT_STATUS & INT_VBLANK_IN)) {
    }
    kmInitDevice(video);
    if (mode & NJD_DSP_HEIGHT_EXT) {
        _nj_palext_height_ratio_ = (mode & PALEXT_RATIO) >> PALEXT_RATIO_SHIFT;
        kmSetPALEXTCallback(njPalExtCallbackFunc, NULL);
        PALEXT_FRAME_HEIGHT(mode)
    }
    /* NOLINTNEXTLINE(clang-diagnostic-sometimes-uninitialized) */
    kmSetDisplayMode(display, bpp, TRUE, anti_alias);
    _nj_SystemConfigStruct_.dwSize = sizeof(KMSYSTEMCONFIGSTRUCT);
    _nj_SystemConfigStruct_.flags = KM_CONFIGFLAG_ENABLE_CLEAR_FRAMEBUFFER;
    _nj_ppSurfaceArray[0] = (PKMSURFACEDESC)&_nj_framebuffer_surface0_;
    _nj_ppSurfaceArray[1] = (PKMSURFACEDESC)&_nj_framebuffer_surface1_;
    _nj_SystemConfigStruct_.ppSurfaceDescArray = _nj_ppSurfaceArray;
    _nj_SystemConfigStruct_.nWidth = 0;
    _nj_SystemConfigStruct_.nHeight = 0;
    _nj_SystemConfigStruct_.nTextureMemorySize = _nj_texture_memory_size;
    _nj_vertex_buffer_size_ = 0;
    _nj_SystemConfigStruct_.pBufferDesc = &_nj_vertex_buffer_;
    _nj_SystemConfigStruct_.pVertexBuffer = NULL;
    _nj_SystemConfigStruct_.nVertexBufferSize = 0;
    _nj_SystemConfigStruct_.fBufferSize[KM_OPAQUE_POLYGON] = 0.0F;
    _nj_SystemConfigStruct_.fBufferSize[KM_OPAQUE_MODIFIER] = 0.0F;
    _nj_SystemConfigStruct_.fBufferSize[KM_TRANS_POLYGON] = 0.0F;
    _nj_SystemConfigStruct_.fBufferSize[KM_TRANS_MODIFIER] = 0.0F;
    _nj_SystemConfigStruct_.fBufferSize[KM_PUNCHTHROUGH_POLYGON] = 0.0F;
    _nj_render_flag_ = FALSE;
    _nj_eor_flag_ = FALSE;
    _nj_eov_flag_ = TRUE;
    _nj_fatal_err_callback_ = 0;
    kmSetEORCallback(njSetEORflag, NULL);
    kmSetEndOfVertexCallback(njSetEOVflag, NULL);
    kmfSetFatalErrorCallback(njSetERRflag);
    kmSetWaitVsyncCount(_nj_waitvsync_count);
    njInitGlobal();
    if ((BUS_REVISION & BUS_REVISION_MASK) < BUS_REVISION_MIN) {
        njSetBorderColor(BORDER_ORANGE);
        _NJ_GO_TO_LOOP();
    }
}

void njChangeSystem(Int mode, Int frame, Int count)
{
    KMVIDEOMODE video;
    Int display;
    SYE_CBL_CABLE cable;
    KMBPPMODE bpp;
    KMBOOLEAN anti_alias;

    njStopDisplay();
    kmfSetNoWaitVSyncAtDisplayModeChange(TRUE);
    _nj_frame_mode_ = frame;
    _nj_waitvsync_count = count;
    video = mode & DSP_VIDEO;
    display = mode & NJD_DSP_MASK;
    if (mode & NJD_DSP_WIDTH_640) {
        _nj_screen_.w = 640.0F;
        _nj_frame_size_w = 640;
    } else {
        _nj_screen_.w = 320.0F;
        _nj_frame_size_w = 320;
    }
    if (mode & NJD_DSP_HEIGHT_480) {
        _nj_screen_.h = 480.0F;
        _nj_frame_size_h = 480;
    } else {
        _nj_screen_.h = 240.0F;
        _nj_frame_size_h = 240;
    }
    if (video == KM_VGA) {
        njSetVideoMode(SYE_CBL_CABLE_VGA << CABLE_SHIFT);
        _nj_screen_.w = 640.0F;
        _nj_frame_size_w = 640;
        _nj_screen_.h = 480.0F;
        _nj_frame_size_h = 480;
    } else {
        cable = syCblCheckCable();
        /* @hack spelling: SYE_CBL_CABLE_RESERVED's case last, where the cases
         * in their values' order would do */
        switch (cable) {
        case SYE_CBL_CABLE_RGB:
            njSetVideoModeChange(SYE_CBL_CABLE_RGB << CABLE_SHIFT);
            break;
        case SYE_CBL_CABLE_VBS:
            njSetVideoModeChange(SYE_CBL_CABLE_VBS << CABLE_SHIFT);
            break;
        case SYE_CBL_CABLE_RESERVED:
            njSetVideoModeChange(SYE_CBL_CABLE_RESERVED << CABLE_SHIFT);
            break;
        default:
            njSetVideoModeChange(SYE_CBL_CABLE_VBS << CABLE_SHIFT);
            break;
        }
    }
    if (mode & NJD_DSP_ANTI) {
        _nj_screen_.w *= 2.0F;
        anti_alias = TRUE;
        if (_nj_screen_.h == 240.0F) {
            _nj_screen_.h = 480.0F;
        }
    } else {
        anti_alias = FALSE;
    }
#ifdef NONMATCHING
    /* a frame buffer format no case lists leaves bpp unset, and the image
     * passes what a register held. this side passes -1 */
    bpp = (KMBPPMODE)-1;
#endif
    FRAME_BPP(bpp, frame)
    _nj_SystemConfigStruct_.nTextureMemorySize = _nj_texture_memory_size;
    if (mode & NJD_DSP_HEIGHT_EXT) {
        _nj_palext_height_ratio_ = (mode & PALEXT_RATIO) >> PALEXT_RATIO_SHIFT;
        kmSetPALEXTCallback(njPalExtCallbackFunc, NULL);
        PALEXT_FRAME_HEIGHT(mode)
    }
    /* NOLINTNEXTLINE(clang-diagnostic-sometimes-uninitialized) */
    kmSetDisplayMode(display, bpp, TRUE, anti_alias);
    kmSetWaitVsyncCount(_nj_waitvsync_count);
    _nj_render_flag_ = FALSE;
    _nj_eor_flag_ = FALSE;
    _nj_eov_flag_ = TRUE;
    _nj_fatal_err_callback_ = 0;
    kmSetEORCallback(njSetEORflag, NULL);
    kmSetEndOfVertexCallback(njSetEOVflag, NULL);
    kmfSetFatalErrorCallback(njSetERRflag);
    njChangeGlobal();
    kmfSetNoWaitVSyncAtDisplayModeChange(FALSE);
}

void njInitGlobal(void)
{
    NJS_POINT2 clip[2];
    Int i;

    _nj_calc_vertex_ = 0;
    _nj_calc_polygon_ = 0;
    _nj_draw_polygon_ = 0;
    _nj_screen_.cx = _nj_screen_.w / 2.0F;
    _nj_screen_.cy = _nj_screen_.h / 2.0F;
    _nj_scr_aspect_x_ = 1.0F;
    _nj_scr_aspect_y_ = 1.0F;
    _nj_screen_.dist = 500.0F;
    _nj_near_clip_ = 1.0F;
    _nj_far_clip_ = 60000.0F;
    _nj_control_3d_flag_ = 0;
    _nj_vsync_count_ = 0;
    _nj_motion_callback_ = NULL;
    /* @hack spelling: the corners through clip, where stores straight into
     * _nj_2d_clip_ would do */
    clip[0].x = 0.0F;
    clip[0].y = 0.0F;
    clip[1].x = _nj_screen_.w - 1.0F;
    clip[1].y = _nj_screen_.h - 1.0F;
    _nj_2d_clip_[0].x = clip[0].x;
    _nj_2d_clip_[0].y = clip[0].y;
    _nj_2d_clip_[1].x = clip[1].x;
    _nj_2d_clip_[1].y = clip[1].y;
    njLightSystemInit();
    for (i = 0; i < CONTEXT_COUNT; i++) {
        njInitContext(&_nj_context_[i]);
        _nj_ctx_color_mode_[i] = NJD_COLOR_MODE_NORMAL;
    }
    njInitContext(&_nj_background_ctx_);
    _nj_background_ctx_.RenderState = KM_SRCBLENDINGMODE | KM_DSTBLENDINGMODE;
    _nj_background_ctx_.SRCBlendingMode = KM_ONE;
    _nj_background_ctx_.DSTBlendingMode = KM_ZERO;
    kmProcessVertexRenderState(&_nj_background_ctx_);
    kmSetBackGroundRenderState(&_nj_background_ctx_);
    njInitSprite();
    kmSetCullingRegister(CULLING_SIZE);
    njSetCurrentContext(0);
    COPY_LIGHT_DEFAULTS(_nj_cnk_easy_multilight_buffer_,
                        &_nj_cnk_easy_multilight_buffer_mdst_,
                        _nj_cnk_easy_multilight_buffer_dst_);
    COPY_LIGHT_DEFAULTS(_nj_cnk_simple_multilight_buffer_,
                        &_nj_cnk_simple_multilight_buffer_mdst_,
                        _nj_cnk_simple_multilight_buffer_dst_);
    vsync_handle = 0;
}

void njChangeGlobal(void)
{
    NJS_POINT2 clip[2];

    _nj_calc_vertex_ = 0;
    _nj_calc_polygon_ = 0;
    _nj_draw_polygon_ = 0;
    _nj_screen_.cx = _nj_screen_.w / 2.0F;
    _nj_screen_.cy = _nj_screen_.h / 2.0F;
    /* @hack spelling: the corners through clip, where stores straight into
     * _nj_2d_clip_ would do */
    clip[0].x = 0.0F;
    clip[0].y = 0.0F;
    clip[1].x = _nj_screen_.w - 1.0F;
    clip[1].y = _nj_screen_.h - 1.0F;
    _nj_2d_clip_[0].x = clip[0].x;
    _nj_2d_clip_[0].y = clip[0].y;
    _nj_2d_clip_[1].x = clip[1].x;
    _nj_2d_clip_[1].y = clip[1].y;
}

void njInitVertexBuffer(Sint32 op, Sint32 om, Sint32 tp, Sint32 tm, Sint32 pt)
{
    Int mask;
    Float total;
    Float op_percent;
    Float om_percent;
    Float tp_percent;
    Float tm_percent;
    Float pt_percent;

    _nj_vertex_2v_type_ = NOBUF_NONE;
    if (op < 0) {
        _nj_SystemConfigStruct_.flags = KM_CONFIGFLAG_NOWAIT_FINISH_TEXTUREDMA |
                                        KM_CONFIGFLAG_ENABLE_2V_LATENCY;
        _nj_SystemConfigStruct_.nVbufModel =
            KM_VERTEXBUFMODEL_NOBUF_OPAQUE_POLYGON;
        op = NOBUF_WORDS;
        _nj_vertex_2v_type_ = NOBUF_OPAQUE;
    } else if (tp < 0) {
        _nj_SystemConfigStruct_.flags = KM_CONFIGFLAG_NOWAIT_FINISH_TEXTUREDMA |
                                        KM_CONFIGFLAG_ENABLE_2V_LATENCY;
        _nj_SystemConfigStruct_.nVbufModel =
            KM_VERTEXBUFMODEL_NOBUF_TRANS_POLYGON;
        tp = NOBUF_WORDS;
        _nj_vertex_2v_type_ = NOBUF_TRANS;
    } else if (pt < 0) {
        _nj_SystemConfigStruct_.flags = KM_CONFIGFLAG_NOWAIT_FINISH_TEXTUREDMA |
                                        KM_CONFIGFLAG_ENABLE_2V_LATENCY;
        _nj_SystemConfigStruct_.nVbufModel =
            KM_VERTEXBUFMODEL_NOBUF_PUNCH_THROUGH;
        pt = NOBUF_WORDS;
        _nj_vertex_2v_type_ = NOBUF_TRANS;
    } else {
        _nj_SystemConfigStruct_.flags = KM_CONFIGFLAG_NOWAIT_FINISH_TEXTUREDMA;
        _nj_SystemConfigStruct_.nVbufModel = KM_VERTEXBUFMODEL_NORMAL;
    }
    _nj_vertex_buffer_op_ = op;
    _nj_vertex_buffer_om_ = om;
    _nj_vertex_buffer_tp_ = tp;
    _nj_vertex_buffer_tm_ = tm;
    _nj_vertex_buffer_pt_ = pt;
    _nj_vertex_buffer_size_ = op + om + tp + tm + pt;
    if (_nj_usr_vertex_buffer_ptr_ == NULL) {
        if (_nj_SystemConfigStruct_.pVertexBuffer) {
            syFree((void *)((Uint32)_nj_SystemConfigStruct_.pVertexBuffer &
                            P2_TO_P1));
        }
        /* @hack spelling: the total converted in each arm, where one
         * conversion after them would do */
        total = (Float)_nj_vertex_buffer_size_;
        _nj_SystemConfigStruct_.pVertexBuffer =
            (Uint32 *)((Uint32)syMalloc(
                           (_nj_vertex_buffer_size_ * sizeof(Uint32)) +
                           (BUFFER_EXTRA * sizeof(Uint32))) |
                       P1_TO_P2);
        _nj_SystemConfigStruct_.nVertexBufferSize =
            _nj_vertex_buffer_size_ + BUFFER_EXTRA;
    } else if (_nj_usr_vertex_buffer_size_ >=
               (_nj_vertex_buffer_size_ * sizeof(Uint32)) +
                   (BUFFER_EXTRA * sizeof(Uint32))) {
        total = (Float)_nj_vertex_buffer_size_;
        _nj_SystemConfigStruct_.pVertexBuffer = _nj_usr_vertex_buffer_ptr_;
        _nj_SystemConfigStruct_.nVertexBufferSize =
            _nj_vertex_buffer_size_ + BUFFER_EXTRA;
    } else {
        total = (Float)_nj_vertex_buffer_size_;
        _nj_SystemConfigStruct_.pVertexBuffer = NULL;
        _nj_SystemConfigStruct_.nVertexBufferSize = 0;
    }
    /* @hack spelling: each list's percent through a local, where a store as
     * each is made would do */
    op_percent = (Float)op * 100.0F / total;
    om_percent = (Float)om * 100.0F / total;
    tp_percent = (Float)tp * 100.0F / total;
    tm_percent = (Float)tm * 100.0F / total;
    pt_percent = (Float)pt * 100.0F / total;
    _nj_SystemConfigStruct_.fBufferSize[KM_OPAQUE_POLYGON] = op_percent;
    _nj_SystemConfigStruct_.fBufferSize[KM_OPAQUE_MODIFIER] = om_percent;
    _nj_SystemConfigStruct_.fBufferSize[KM_TRANS_POLYGON] = tp_percent;
    _nj_SystemConfigStruct_.fBufferSize[KM_TRANS_MODIFIER] = tm_percent;
    _nj_SystemConfigStruct_.fBufferSize[KM_PUNCHTHROUGH_POLYGON] = pt_percent;
    mask = get_imask();
    set_imask(IMASK_ALL);
    waitVsync();
    kmSetSystemConfiguration(&_nj_SystemConfigStruct_);
    set_imask(mask);
    _nj_tex_max_free_size = njCalcTexture(NJD_TEXMEM_FREESIZE);
}

void njSetCurrentContext(Int n)
{
    _nj_current_ctx_ = &_nj_context_[n];
    kmSetVertexRenderState(_nj_current_ctx_);
    _nj_color_mode_ = &_nj_ctx_color_mode_[n];
}

/* the RenderState bits njInitContext marks for kmProcessVertexRenderState:
 * KM_DEPTHMODE through KM_DSTSELECT, but for KM_COLORCLAMP and KM_PALETTEBANK.
 * the call sets bColorClamp and PaletteBank too, unmarked */
#define INIT_RENDER_STATE                                                      \
    (((KM_DSTSELECT << 1) - KM_DEPTHMODE) & ~(KM_COLORCLAMP | KM_PALETTEBANK))

void njInitContext(KMVERTEXCONTEXT *context)
{
    context->RenderState = INIT_RENDER_STATE;
    context->ParamType = KM_POLYGON;
    context->ListType = KM_OPAQUE_POLYGON;
    context->ColorType = KM_FLOATINGCOLOR;
    context->UVFormat = KM_32BITUV;
    context->DepthMode = KM_GREATEREQUAL;
    context->CullingMode = KM_CULLSMALL;
    context->reserved00 = 0;
    context->ShadingMode = KM_NOTEXTUREGOURAUD;
    context->SelectModifier = KM_NOMODIFIER;
    context->bZWriteDisable = FALSE;
    context->SRCBlendingMode = KM_SRCALPHA;
    context->DSTBlendingMode = KM_INVSRCALPHA;
    context->bSRCSel = FALSE;
    context->bDSTSel = FALSE;
    context->FogMode = KM_NOFOG;
    context->bUseSpecular = FALSE;
    context->bUseAlpha = FALSE;
    context->bIgnoreTextureAlpha = FALSE;
    context->FlipUV = KM_NOFLIP;
    context->ClampUV = KM_NOCLAMP;
    context->FilterMode = KM_POINT_SAMPLE;
    context->bSuperSample = FALSE;
    context->MipMapAdjust = KM_MIPMAP_D_ADJUST_1_00;
    context->TextureShadingMode = KM_MODULATE_ALPHA;
    context->pTextureSurfaceDesc = NULL;
    context->bColorClamp = FALSE;
    context->PaletteBank = 0;
    context->fFaceColorAlpha = 1.0F;
    context->fFaceColorRed = 1.0F;
    context->fFaceColorGreen = 1.0F;
    context->fFaceColorBlue = 1.0F;
    context->fOffsetColorAlpha = 1.0F;
    context->fOffsetColorRed = 0.0F;
    context->fOffsetColorGreen = 0.0F;
    context->fOffsetColorBlue = 0.0F;
    kmProcessVertexRenderState(context);
}

void njSetEORFunction(void (*func)(void))
{
    _nj_eor_function_ = func;
}

void njSetEORCallBackFunction(void (*func)(void))
{
    _nj_eor_function2_ = func;
}

void njExeEORFunction(void)
{
    if (_nj_eor_function_) {
        _nj_eor_function_();
    }
}

void njWaitVSync(void)
{
    Uint32 elapsed;
    Uint32 **next;

    elapsed = 0;
    _nj_wait_vsync_in_ = syTmrGetCount();
    if (_nj_render_flag_) {
        while (_nj_eor_flag_ && elapsed < RENDER_WAIT_COUNT) {
            elapsed = syTmrDiffCount(_nj_wait_vsync_in_, syTmrGetCount());
            /* @bug logic: the wait runs to 212480 counts, about 272 ms, where
             * RENDER_WAIT_COUNT is 166000. elapsed holds counts, which
             * syTmrMicroToCount converts as if they were microseconds, JSR
             * @R9 at 0x8c112802. so the limit stretches by
             * MICROSECONDS_PER_COUNT */
            elapsed = syTmrMicroToCount(elapsed);
        }
        if (elapsed < RENDER_WAIT_COUNT) {
            njExeEORFunction();
        }
    }
    _nj_eor_flag_ = TRUE;
    kmRender();
    _nj_calc_vertex_ = 0;
    _nj_calc_polygon_ = 0;
    _nj_draw_polygon_ = 0;
    _nj_vsync_count_++;
    kmFlipFrameBuffer();
    pdExecPeripheralServer();
    _nj_wait_vsync_out_ = syTmrGetCount();
    /* @hack spelling: next, a local for the lists' addresses, where
     * pCurrentPtr read for each would do */
    next = _nj_vertex_buffer_.pCurrentPtr;
    _nj_start_vertex_op = (Uint32)next[KM_OPAQUE_POLYGON];
    _nj_start_vertex_om = (Uint32)next[KM_OPAQUE_MODIFIER];
    _nj_start_vertex_tp = (Uint32)next[KM_TRANS_POLYGON];
    _nj_start_vertex_tm = (Uint32)next[KM_TRANS_MODIFIER];
    _nj_start_vertex_pt = (Uint32)next[KM_PUNCHTHROUGH_POLYGON];
}

void njRenderStart(void)
{
    _nj_eor_flag_ = TRUE;
    kmRender();
    _nj_calc_vertex_ = 0;
    _nj_calc_polygon_ = 0;
    _nj_draw_polygon_ = 0;
}

void njFlipFrameBuffer(void)
{
    Uint32 **next;

    _nj_wait_vsync_in_ = syTmrGetCount();
    kmFlipFrameBuffer();
    _nj_vsync_count_++;
    _nj_wait_vsync_out_ = syTmrGetCount();
    /* @hack spelling: next, a local for the lists' addresses, where
     * pCurrentPtr read for each would do */
    next = _nj_vertex_buffer_.pCurrentPtr;
    _nj_start_vertex_op = (Uint32)next[KM_OPAQUE_POLYGON];
    _nj_start_vertex_om = (Uint32)next[KM_OPAQUE_MODIFIER];
    _nj_start_vertex_tp = (Uint32)next[KM_TRANS_POLYGON];
    _nj_start_vertex_tm = (Uint32)next[KM_TRANS_MODIFIER];
    _nj_start_vertex_pt = (Uint32)next[KM_PUNCHTHROUGH_POLYGON];
}

void njSetVSyncFunction(void (*func)(void))
{
    if (vsync_handle) {
        syChainDeleteHandler(vsync_handle);
        vsync_handle = 0;
    }
    if (func) {
        vsync_handle =
            syChainAddHandler(SYD_CHAIN_EVENT_IML6_VBLANK,
                              (void (*)(void *))func, VBLANK_PRIORITY, NULL);
    }
}

void njSetWaitVSyncFunction(void (*func)(void *), void *arg)
{
    kmSetWaitVsyncCallback(func, arg);
}

void njGetKmCallbackFunction(Int no, PKMCALLBACKFUNC *func, void **arg)
{
    *func = kmiHardwareResource->callback[no].function;
    *arg = kmiHardwareResource->callback[no].argument;
}

void njGetWaitVSyncFunction(PKMCALLBACKFUNC *func, void **arg)
{
    njGetKmCallbackFunction(WAIT_VSYNC_CALLBACK, func, arg);
}

void njSetBackColor(Uint32 col1, Uint32 col2, Uint32 col3)
{
    KMVERTEX0 vertex[3];
    void *plane[3];

    vertex[0].ParamControlWord = KM_VERTEXPARAM_NORMAL;
    vertex[0].fX = 0.0F;
    vertex[0].fY = 0.0F;
    vertex[0].fZ = BACKGROUND_DEPTH;
    vertex[0].dwBaseRGB = col1;
    vertex[1].ParamControlWord = KM_VERTEXPARAM_NORMAL;
    vertex[1].fX = _nj_screen_.w;
    vertex[1].fY = 0.0F;
    vertex[1].fZ = BACKGROUND_DEPTH;
    vertex[1].dwBaseRGB = col2;
    vertex[2].ParamControlWord = KM_VERTEXPARAM_ENDOFSTRIP;
    vertex[2].fX = 0.0F;
    vertex[2].fY = _nj_screen_.h;
    vertex[2].fZ = BACKGROUND_DEPTH;
    vertex[2].dwBaseRGB = col3;
    plane[0] = &vertex[0];
    plane[1] = &vertex[1];
    plane[2] = &vertex[2];
    kmSetBackGroundPlane(plane, KM_VERTEXTYPE_00, sizeof(KMVERTEX0));
}

Uint32 njVersion(void)
{
    return _nj_version;
}

void njExitSystem(void)
{
    kmDiscardVertexBuffer(&_nj_vertex_buffer_);
    if (_nj_usr_vertex_buffer_ptr_ == NULL) {
        syFree(
            (void *)((Uint32)_nj_SystemConfigStruct_.pVertexBuffer & P2_TO_P1));
    }
}

void njSetColorMode(Uint32 mode)
{
    Uint32 global;
    Uint32 isp;
    Uint32 tsp;

    if (*_nj_color_mode_ != mode) {
        global = _nj_current_ctx_->GLOBALPARAMBUFFER & ~COLOR_MODE_GLOBAL;
        /* @hack redundant: the ISP word read and written back unchanged */
        isp = _nj_current_ctx_->ISPPARAMBUFFER;
        tsp = _nj_current_ctx_->TSPPARAMBUFFER & ~COLOR_MODE_TSP;
        global |= mode & COLOR_MODE_GLOBAL;
        tsp |= mode & COLOR_MODE_TSP;
        _nj_current_ctx_->GLOBALPARAMBUFFER = global;
        _nj_current_ctx_->ISPPARAMBUFFER = isp;
        _nj_current_ctx_->TSPPARAMBUFFER = tsp;
        kmSetVertexRenderState(_nj_current_ctx_);
        *_nj_color_mode_ = mode;
    }
}

Uint32 njGetColorMode(void)
{
    return *_nj_color_mode_;
}

void njSetIntensityColor(NJS_ARGB *face, NJS_ARGB *offset)
{
    _nj_current_ctx_->fFaceColorAlpha = face->a;
    _nj_current_ctx_->fFaceColorRed = face->r;
    _nj_current_ctx_->fFaceColorGreen = face->g;
    _nj_current_ctx_->fFaceColorBlue = face->b;
    _nj_current_ctx_->fOffsetColorAlpha = offset->a;
    _nj_current_ctx_->fOffsetColorRed = offset->r;
    _nj_current_ctx_->fOffsetColorGreen = offset->g;
    _nj_current_ctx_->fOffsetColorBlue = offset->b;
    kmSetVertexRenderState(_nj_current_ctx_);
}

void njZBufferMode(Int mode)
{
    _nj_current_ctx_->RenderState = KM_DEPTHMODE;
    /* @hack spelling: the ZBUFFER_ON case first, where the cases in their
     * values' order would do */
    switch (mode) {
    case ZBUFFER_ON:
        _nj_current_ctx_->DepthMode = KM_GREATEREQUAL;
        break;
    case ZBUFFER_OFF:
        _nj_current_ctx_->DepthMode = KM_ALWAYS;
        break;
    }
    kmProcessVertexRenderState(_nj_current_ctx_);
    kmSetVertexRenderState(_nj_current_ctx_);
}

void njMipmapAdjust(Int level)
{
    _nj_current_ctx_->RenderState = KM_MIPMAPDADJUST;
    _nj_current_ctx_->MipMapAdjust = level;
    kmProcessVertexRenderState(_nj_current_ctx_);
    kmSetVertexRenderState(_nj_current_ctx_);
}

void njTextureFilterMode(Int mode)
{
    _nj_current_ctx_->RenderState = KM_FILTERMODE;
    switch (mode) {
    case NJD_TEXTUREFILTER_POINT_SAMPLE:
        _nj_current_ctx_->FilterMode = KM_POINT_SAMPLE;
        break;
    case NJD_TEXTUREFILTER_BILINEAR:
        _nj_current_ctx_->FilterMode = KM_BILINEAR;
        break;
    case NJD_TEXTUREFILTER_TRILINEAR:
        _nj_current_ctx_->FilterMode = KM_TRILINEAR_A;
        break;
    }
    kmProcessVertexRenderState(_nj_current_ctx_);
    kmSetVertexRenderState(_nj_current_ctx_);
}

void njSuperSampleMode(Int mode)
{
    _nj_current_ctx_->RenderState = KM_SUPERSAMPLE;
    /* @hack spelling: the TRUE case first, where the cases in their values'
     * order would do */
    switch (mode) {
    case TRUE:
        _nj_current_ctx_->bSuperSample = TRUE;
        break;
    case FALSE:
        _nj_current_ctx_->bSuperSample = FALSE;
        break;
    }
    kmProcessVertexRenderState(_nj_current_ctx_);
    kmSetVertexRenderState(_nj_current_ctx_);
}

void njTextureClampMode(Int mode)
{
    _nj_current_ctx_->RenderState = KM_CLAMPUV;
    switch (mode) {
    case NJD_TEXTURECLAMP_NOCLAMP:
        _nj_current_ctx_->ClampUV = KM_NOCLAMP;
        break;
    case NJD_TEXTURECLAMP_CLAMP_V:
        _nj_current_ctx_->ClampUV = KM_CLAMP_V;
        break;
    case NJD_TEXTURECLAMP_CLAMP_U:
        _nj_current_ctx_->ClampUV = KM_CLAMP_U;
        break;
    case NJD_TEXTURECLAMP_CLAMP_UV:
        _nj_current_ctx_->ClampUV = KM_CLAMP_UV;
        break;
    }
    kmProcessVertexRenderState(_nj_current_ctx_);
    kmSetVertexRenderState(_nj_current_ctx_);
}

void njTextureFlipMode(Int mode)
{
    _nj_current_ctx_->RenderState = KM_FLIPUV;
    switch (mode) {
    case NJD_TEXTUREFLIP_NOFLIP:
        _nj_current_ctx_->FlipUV = KM_NOFLIP;
        break;
    case NJD_TEXTUREFLIP_FLIP_V:
        _nj_current_ctx_->FlipUV = KM_FLIP_V;
        break;
    case NJD_TEXTUREFLIP_FLIP_U:
        _nj_current_ctx_->FlipUV = KM_FLIP_U;
        break;
    case NJD_TEXTUREFLIP_FLIP_UV:
        _nj_current_ctx_->FlipUV = KM_FLIP_UV;
        break;
    }
    kmProcessVertexRenderState(_nj_current_ctx_);
    kmSetVertexRenderState(_nj_current_ctx_);
}

void njPolygonCullingSize(Float size)
{
    kmSetCullingRegister(size);
}

void njPolygonCullingMode(Int mode)
{
    _nj_current_ctx_->RenderState = KM_CULLINGMODE;
    switch (mode) {
    case KM_NOCULLING:
        _nj_current_ctx_->CullingMode = KM_NOCULLING;
        break;
    case KM_CULLSMALL:
        _nj_current_ctx_->CullingMode = KM_CULLSMALL;
        break;
    case KM_CULLCCW:
        _nj_current_ctx_->CullingMode = KM_CULLCCW;
        break;
    case KM_CULLCW:
        _nj_current_ctx_->CullingMode = KM_CULLCW;
        break;
    }
    kmProcessVertexRenderState(_nj_current_ctx_);
    kmSetVertexRenderState(_nj_current_ctx_);
}

void njColorBlendingMode(Int target, Int mode)
{
    switch (mode) {
    case NJD_COLOR_BLENDING_BOTHINVALPHA:
        mode = KM_BOTHINVSRCALPHA;
        break;
    case NJD_COLOR_BLENDING_BOTHSRCALPHA:
        mode = KM_BOTHSRCALPHA;
        break;
    case NJD_COLOR_BLENDING_DESTALPHA:
        mode = KM_DESTALPHA;
        break;
    case NJD_COLOR_BLENDING_DESTCOLOR:
        mode = KM_DESTCOLOR;
        break;
    case NJD_COLOR_BLENDING_INVDESTALPHA:
        mode = KM_INVDESTALPHA;
        break;
    case NJD_COLOR_BLENDING_INVDESTCOLOR:
        mode = KM_INVDESTCOLOR;
        break;
    case NJD_COLOR_BLENDING_INVSRCALPHA:
        mode = KM_INVSRCALPHA;
        break;
    case NJD_COLOR_BLENDING_INVSRCCOLOR:
        mode = KM_INVSRCCOLOR;
        break;
    case NJD_COLOR_BLENDING_SRCALPHA:
        mode = KM_SRCALPHA;
        break;
    case NJD_COLOR_BLENDING_SRCCOLOR:
        mode = KM_SRCCOLOR;
        break;
    case NJD_COLOR_BLENDING_ONE:
        mode = KM_ONE;
        break;
    case NJD_COLOR_BLENDING_ZERO:
        mode = KM_ZERO;
        break;
    }
    if (target == NJD_SOURCE_COLOR) {
        _nj_current_ctx_->RenderState = KM_SRCBLENDINGMODE;
        _nj_current_ctx_->SRCBlendingMode = mode;
    }
    if (target == NJD_DESTINATION_COLOR) {
        _nj_current_ctx_->RenderState = KM_DSTBLENDINGMODE;
        _nj_current_ctx_->DSTBlendingMode = mode;
    }
    kmProcessVertexRenderState(_nj_current_ctx_);
    kmSetVertexRenderState(_nj_current_ctx_);
}

void njSpecularMode(Int mode)
{
    _nj_current_ctx_->RenderState = KM_USESPECULAR;
    /* @hack spelling: the TRUE case first, where the cases in their values'
     * order would do */
    switch (mode) {
    case TRUE:
        _nj_current_ctx_->bUseSpecular = TRUE;
        break;
    case FALSE:
        _nj_current_ctx_->bUseSpecular = FALSE;
        break;
    }
    kmProcessVertexRenderState(_nj_current_ctx_);
    kmSetVertexRenderState(_nj_current_ctx_);
}

void njAlphaMode(Int mode)
{
    _nj_current_ctx_->RenderState = KM_USEALPHA;
    /* @hack spelling: the TRUE case first, where the cases in their values'
     * order would do */
    switch (mode) {
    case TRUE:
        _nj_current_ctx_->bUseAlpha = TRUE;
        break;
    case FALSE:
        _nj_current_ctx_->bUseAlpha = FALSE;
        break;
    }
    kmProcessVertexRenderState(_nj_current_ctx_);
    kmSetVertexRenderState(_nj_current_ctx_);
}

void njIgnoreTextureAlphaMode(Int mode)
{
    _nj_current_ctx_->RenderState = KM_IGNORETEXTUREALPHA;
    /* @hack spelling: the TRUE case first, where the cases in their values'
     * order would do */
    switch (mode) {
    case TRUE:
        _nj_current_ctx_->bIgnoreTextureAlpha = TRUE;
        break;
    case FALSE:
        _nj_current_ctx_->bIgnoreTextureAlpha = FALSE;
        break;
    }
    kmProcessVertexRenderState(_nj_current_ctx_);
    kmSetVertexRenderState(_nj_current_ctx_);
}

void njTextureShadingMode(Int mode)
{
    _nj_current_ctx_->RenderState = KM_TEXTURESHADINGMODE;
    /* @hack spelling: NJD_TEX_SHADING_MODE_DECAL's case first, where the
     * cases in their values' order would do */
    switch (mode) {
    case NJD_TEX_SHADING_MODE_DECAL:
        _nj_current_ctx_->TextureShadingMode = KM_DECAL;
        break;
    case NJD_TEX_SHADING_MODE_MODULATE:
        _nj_current_ctx_->TextureShadingMode = KM_MODULATE;
        break;
    case NJD_TEX_SHADING_MODE_DECALALPHA:
        _nj_current_ctx_->TextureShadingMode = KM_DECAL_ALPHA;
        break;
    case NJD_TEX_SHADING_MODE_MODULATEALPHA:
        _nj_current_ctx_->TextureShadingMode = KM_MODULATE_ALPHA;
        break;
    }
    kmProcessVertexRenderState(_nj_current_ctx_);
    kmSetVertexRenderState(_nj_current_ctx_);
}

void njSetCheapShadowMode(Int mode)
{
    kmSetCheapShadowMode(mode);
}

/* give list the clip rectangle of the tiles from p[0] to p[1] */
#define SET_USER_CLIPPING(list, p)                                             \
    kmSetUserClipping(&_nj_vertex_buffer_, (list), (KMINT32)(p)[0].x,          \
                      (KMINT32)(p)[0].y, (KMINT32)(p)[1].x, (KMINT32)(p)[1].y)

void njUserClipping(Int mode, NJS_POINT2 *p)
{
    Uint32 *sq;

    if (_nj_vertex_2v_type_ == NOBUF_OPAQUE) {
        njGetkmPointer();
        njSetkmColorMode(NJD_COLOR_MODE_NORMAL);
        /* @hack spelling: sq, a local for the starter's result, where the
         * call in njEndVertex's argument would do */
        sq = njStartVertexNonTexOpaque();
        njEndVertex(sq);
        njSetkmPointer();
    } else if (_nj_vertex_2v_type_ == NOBUF_TRANS) {
        njGetkmPointer();
        njSetkmColorMode(NJD_COLOR_MODE_NORMAL_TRANS);
        sq = njStartVertexNonTexTrans();
        njEndVertex(sq);
        njSetkmPointer();
    }
    if (_nj_vertex_buffer_op_) {
        SET_USER_CLIPPING(KM_OPAQUE_POLYGON, last_rect);
    }
    if (_nj_vertex_buffer_om_) {
        SET_USER_CLIPPING(KM_OPAQUE_MODIFIER, last_rect);
    }
    if (_nj_vertex_buffer_tp_) {
        SET_USER_CLIPPING(KM_TRANS_POLYGON, last_rect);
    }
    if (_nj_vertex_buffer_tm_) {
        SET_USER_CLIPPING(KM_TRANS_MODIFIER, last_rect);
    }
    if (_nj_vertex_buffer_pt_) {
        SET_USER_CLIPPING(KM_PUNCHTHROUGH_POLYGON, last_rect);
    }
    _nj_current_ctx_->RenderState = KM_USERCLIPMODE;
    _nj_current_ctx_->UserClipMode = mode;
    kmProcessVertexRenderState(_nj_current_ctx_);
    kmSetVertexRenderState(_nj_current_ctx_);
    if (_nj_vertex_buffer_op_) {
        SET_USER_CLIPPING(KM_OPAQUE_POLYGON, p);
    }
    if (_nj_vertex_buffer_om_) {
        SET_USER_CLIPPING(KM_OPAQUE_MODIFIER, p);
    }
    if (_nj_vertex_buffer_tp_) {
        SET_USER_CLIPPING(KM_TRANS_POLYGON, p);
    }
    if (_nj_vertex_buffer_tm_) {
        SET_USER_CLIPPING(KM_TRANS_MODIFIER, p);
    }
    if (_nj_vertex_buffer_pt_) {
        SET_USER_CLIPPING(KM_PUNCHTHROUGH_POLYGON, p);
    }
    last_rect[0] = p[0];
    last_rect[1] = p[1];
    if (mode == NJD_CLIP_INSIDE) {
        _nj_2d_clip_[0].x = p[0].x * TILE_SIZE;
        _nj_2d_clip_[0].y = p[0].y * TILE_SIZE;
        _nj_2d_clip_[1].x = (p[1].x * TILE_SIZE) + (TILE_SIZE - 1.0F);
        _nj_2d_clip_[1].y = (p[1].y * TILE_SIZE) + (TILE_SIZE - 1.0F);
    } else {
        _nj_2d_clip_[0].x = 0.0F;
        _nj_2d_clip_[0].y = 0.0F;
        _nj_2d_clip_[1].x = _nj_screen_.w - 1.0F;
        _nj_2d_clip_[1].y = _nj_screen_.h - 1.0F;
    }
}

void njGetSystemAttr(NJS_SYS_ATTR *attr)
{
    njMemCopy(attr, _nj_current_ctx_, sizeof(KMVERTEXCONTEXT));
    _nj_current_texture_back_ = _nj_current_texture;
    _nj_current_texture_bank_back_ = _nj_current_texture_bank;
}

void njSetSystemAttr(NJS_SYS_ATTR *attr)
{
    njMemCopy(_nj_current_ctx_, attr, sizeof(KMVERTEXCONTEXT));
    kmSetVertexRenderState(_nj_current_ctx_);
    _nj_current_texture = _nj_current_texture_back_;
    _nj_current_texture_bank = _nj_current_texture_bank_back_;
}

void njStopDisplay(void)
{
    kmStopDisplayFrameBuffer();
}

void njColorClampMode(Int mode)
{
    _nj_current_ctx_->RenderState = KM_COLORCLAMP;
    /* @hack spelling: the TRUE case first, where the cases in their values'
     * order would do */
    switch (mode) {
    case TRUE:
        _nj_current_ctx_->bColorClamp = TRUE;
        break;
    case FALSE:
        _nj_current_ctx_->bColorClamp = FALSE;
        break;
    }
    kmProcessVertexRenderState(_nj_current_ctx_);
    kmSetVertexRenderState(_nj_current_ctx_);
}

void njSetColorClamp(Uint32 MaxARGB, Uint32 MinARGB)
{
    KMPACKEDARGB high;
    KMPACKEDARGB low;

    high.dwPacked = MaxARGB;
    low.dwPacked = MinARGB;
    kmSetColorClampValue(high, low);
}

void njAdjustDisplay(Sint32 xadjust, Sint32 yadjust)
{
    kmAdjustDisplayCenter(xadjust, yadjust);
}

void njSetBorderColor(Uint32 color)
{
    /* @hack type: color passed through its own address as a KMPACKEDARGB,
     * where a KMPACKEDARGB local would do */
    kmSetBorderColor(*(KMPACKEDARGB *)&color);
}

void njGetSystemInfo(NJS_SYSTEMINFO *info)
{
    KMISYSTEMMETRICS metrics;

    kmfGetInternalSystemMetrics(&metrics);
    info->NativeLimit = metrics.NativeParams[0].NativeITPLimit;
    info->NativeCurrent = metrics.NativeParams[0].NativeLastITPCurrent;
    if (info->NativePeak < info->NativeCurrent) {
        /* @hack spelling: the peak taken from metrics again, where info's
         * copy would do */
        info->NativePeak = metrics.NativeParams[0].NativeLastITPCurrent;
    }
    njMemCopy(info->InternalVertexBufSize, metrics.InternalVertexBufSize,
              sizeof(info->InternalVertexBufSize));
    njMemCopy(info->MaxVertexBufSize, metrics.MaxVertexBufSize,
              sizeof(info->MaxVertexBufSize));
    info->LastTATime =
        (Uint32)((Float)metrics.LastTATime * MICROSECONDS_PER_COUNT);
    info->LastRenderTime =
        (Uint32)((Float)metrics.LastRenderTime * MICROSECONDS_PER_COUNT);
    _nj_frame_addr_0_ = metrics.FBParams.FBStartAddr[0] | VIDEO_MEMORY_P2;
    _nj_frame_addr_1_ = metrics.FBParams.FBStartAddr[1] | VIDEO_MEMORY_P2;
    info->CurrentVertexBufSize[KM_OPAQUE_POLYGON] =
        (Uint32)_nj_vertex_buffer_.pCurrentPtr[KM_OPAQUE_POLYGON] -
        _nj_start_vertex_op;
    info->CurrentVertexBufSize[KM_OPAQUE_MODIFIER] =
        (Uint32)_nj_vertex_buffer_.pCurrentPtr[KM_OPAQUE_MODIFIER] -
        _nj_start_vertex_om;
    info->CurrentVertexBufSize[KM_TRANS_POLYGON] =
        (Uint32)_nj_vertex_buffer_.pCurrentPtr[KM_TRANS_POLYGON] -
        _nj_start_vertex_tp;
    info->CurrentVertexBufSize[KM_TRANS_MODIFIER] =
        (Uint32)_nj_vertex_buffer_.pCurrentPtr[KM_TRANS_MODIFIER] -
        _nj_start_vertex_tm;
    info->CurrentVertexBufSize[KM_PUNCHTHROUGH_POLYGON] =
        (Uint32)_nj_vertex_buffer_.pCurrentPtr[KM_PUNCHTHROUGH_POLYGON] -
        _nj_start_vertex_pt;
}

void njSetWaitVsyncCount(Int count)
{
    _nj_waitvsync_count = count;
    kmSetWaitVsyncCount(count);
}
