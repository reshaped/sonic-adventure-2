/* Kamui's internals, which Ninja's sources use too: its record of the video
 * chip, and its copies */
#ifndef KAMUI_INTERNAL_H
#define KAMUI_INTERNAL_H
#include <kamui.h>

/* an entry of the device record's callback table. Kamui calls function for
 * an event, and passes it argument */
struct kmi_callback {
    PKMCALLBACKFUNC function;
    void *argument;
};
#define KMI_CALLBACKS 10 /* the number of entries in the callback table */
#define END_RENDER_CALLBACK 0
#define VSYNC_CALLBACK 1
#define HSYNC_CALLBACK 2
#define TEXTURE_OVERFLOW_CALLBACK 3
#define STRIP_OVERRUN_CALLBACK 4
#define END_VERTEX_CALLBACK 5
#define END_YUV_CALLBACK 6
#define WAIT_VSYNC_CALLBACK 7
#define FATAL_ERROR_CALLBACK 8
#define PAL_EXTENSION_CALLBACK 9

/* The TA setup takes addresses in units of 128 bytes. */
#define KMI_TA_ADDRESS_SHIFT 7

/* Each bank has an object list, two parameter buffers and one region array. */
struct kmi_ta_layout {
    KMUINT32 region_array;
    KMUINT32 object_list;
    KMUINT32 isp_params;
    KMUINT32 tsp_params;
};

struct kmi_ta_bank {
    KMUINT32 start;
    KMUINT32 region_base;
    KMUINT32 unk_08;
    KMUINT32 ol_base;
    KMUINT32 ol_limit;
    KMUINT32 next_opb_init;
    KMUINT32 isp_base;
    KMUINT32 isp_limit;
};

struct kmi_version_info {
    KMUINT32 major;
    KMUINT32 minor;
    KMUINT32 revision;
    KMUINT32 build;
    const char *copyright; /* 621 bytes, without a terminator */
};

/* States of a frame slot as the game, TA, renderer and scanout hand it on. */
#define KMI_FRAME_FREE (-1) /* no frame, as after a reset or a TA timeout */
#define KMI_FRAME_OVERFLOW (-3)
#define KMI_FRAME_WRITING 0
#define KMI_FRAME_QUEUED 1
#define KMI_FRAME_TA 2
#define KMI_FRAME_TA_DONE 3
#define KMI_FRAME_RENDERING 4
#define KMI_FRAME_RENDERED 5
#define KMI_FRAME_SHOWN 7
/* the slots as a reset leaves them: slot 0 the write slot, the others free,
 * and the write, render and TA slots all slot 0 */
#define KMI_RESET_SLOTS()                                                      \
    kmiDeviceExtension.slot_state[0] = KMI_FRAME_WRITING;                      \
    kmiDeviceExtension.slot_state[1] = KMI_FRAME_FREE;                         \
    kmiDeviceExtension.slot_state[2] = KMI_FRAME_FREE;                         \
    kmiDeviceExtension.slot_state[3] = KMI_FRAME_FREE;                         \
    kmiDeviceExtension.write_slot = 0;                                         \
    kmiDeviceExtension.render_slot = 0;                                        \
    kmiDeviceExtension.ta_slot = 0

typedef KMUINT32 KMI_DMA_HANDLE;
#define KMI_DMA_SECOND_RING 8
#define KMI_DMA_SOURCE_UP 0x10
/* kmiDMAtoTARequest's results: the request queued, the queue full, and a
 * request of no bytes, which queues nothing. only a queued request sets
 * *handle, to the address of the handle kmiQueryFinishDMA takes */
#define KMI_DMA_QUEUED 0
#define KMI_DMA_QUEUE_FULL 0x100
#define KMI_DMA_NO_BYTES 0x103

#define KMI_FRAME_SLOTS 4
struct kmi_list_start_row {
    KMDWORD *slot[KMI_FRAME_SLOTS];
};
union kmi_list_starts {
    struct kmi_list_start_row row[KMI_LISTS];
    struct {
        struct kmi_list_start_row opaque;
        struct kmi_list_start_row opaque_modifier;
        struct kmi_list_start_row translucent;
        struct kmi_list_start_row translucent_modifier;
        struct kmi_list_start_row punch_through;
    } list;
    KMDWORD *word[KMI_LISTS * KMI_FRAME_SLOTS];
};
struct kmi_list_size_row {
    KMUINT32 slot[KMI_FRAME_SLOTS];
};
union kmi_list_sizes {
    struct kmi_list_size_row row[KMI_LISTS];
    struct {
        struct kmi_list_size_row opaque;
        struct kmi_list_size_row opaque_modifier;
        struct kmi_list_size_row translucent;
        struct kmi_list_size_row translucent_modifier;
        struct kmi_list_size_row punch_through;
    } list;
    KMUINT32 word[KMI_LISTS * KMI_FRAME_SLOTS];
};

/* The driver's setup, render pipeline and shadow copies of PowerVR registers.
 * One interrupt handler advances the four slots through two TA banks. */
struct kmi_device_extension {
    struct kmi_version_info *version;
    KMDWORD core; /* the chip's core, 5 or 6. Kamui runs only on 6 */
    /* bits that kmiInitializeDevice sets by the system bus's revision */
    KMDWORD capabilities;
/* capabilities' bits. KMI_CAN_READ_ITP_CURRENT marks a chip whose tile
 * accelerator reports where its ISP parameters end, and KMI_CAN_READ_SYNC one
 * whose KMI_SYNC_STATUS reports the line and the field on display */
#define KMI_CAN_READ_ITP_CURRENT 1
#define KMI_CAN_READ_SYNC 2
/* frame buffers and textures each go in the bank with more room. without the
 * bit, frame buffers go in bank 0 and textures in bank 1 */
#define KMI_CHOOSE_BANK 4
/* the renderer resets before each render. kmfSetTimeOutCount sets the bit
 * where render_count is 0, and else sets it back as kmiInitializeDevice did */
#define KMI_RESET_BEFORE_RENDER 0x10
/* each tile's last entry in the region array draws nothing and writes the
 * tile out. a tile then takes two entries, or four while dummy_translucent is
 * set */
#define KMI_FLUSH_ENTRY 0x20
/* kmRender can add kmiDummyOpaque to the opaque list */
#define KMI_DUMMY_OPAQUE 0x40
/* an entry that draws nothing comes before the first tile's.
 * kmiInitializeDevice never sets the bit with KMI_FLUSH_ENTRY */
#define KMI_EMPTY_FIRST_ENTRY 0x80
    KMBYTE unk_00c[8];
    KMUINT32 memory_alignment;
    KMUINT32 texture_memory_size;
    KMUINT32 unk_01c;
    KMVIDEOMODE video_mode; /* the display's video signal */
    KMINT32 display_mode;
    KMUINT32 bpp;
    KMUINT32 dither;
    KMUINT32 anti_alias;
    KMINT32 pal_selection;
    KMUINT32 strip_buffer;
    KMUINT32 level_adjust;
    KMINT32 skip_lines;
    KMUINT32 fb_width; /* line length in words, less one */
    KMUINT32 fb_lines; /* line count, less one */
    KMUINT32 display_width;
    KMUINT32 display_height;
    KMINT32 offset_x;
    KMINT32 offset_y;
    KMINT32 origin_x;
    KMINT32 origin_y1;
    KMINT32 origin_y2;
    KMUINT32 polling_ta;
    KMUINT32 ta_options;
    struct kmi_ta_layout ta_layout[2];
    KMUINT32 bank_limit[2];
    KMUINT32 isp_size;
    KMUINT32 tsp_size;
    KMUINT32 palette_mode;
    KMUINT32 alpha_threshold;
    KMUINT32 background_packet[62];
    KMUINT32 background_size;
    KMUINT32 background_isp;
    KMUINT32 background_tsp;
    KMUINT32 background_texture;
    KMUINT32 background_skip;
    KMUINT32 background_depth;
    KMUINT32 background_tag;
    KMUINT32 background_base_color;
    KMUINT32 background_offset_color;
    KMUINT32 regions_stale;
    KMUINT32 presort;
    KMUINT32 global_clip;
    KMUINT32 global_clip_width;
    KMUINT32 global_clip_height;
    KMUINT32 another_modifier;
    KMUINT32 another_modifier_list;
    KMUINT32 strip_length;
    KMINT32 vsync_remaining;
    KMINT32 vsync_wait;
    KMUINT32 wait_vsync;
    KMUINT32 unk_1f0;
    KMBYTE unk_1f4[0x18];
    KMUINT32 render_busy;
    KMINT32 ta_busy;
    KMBYTE unk_214[8];
    KMINT32 field_offset[2];
    KMBYTE unk_224[4];
    KMINT32 render_timeout;
    KMINT32 vertex_timeout;
    KMUINT32 vsync_timeout;
    struct kmi_callback callback[KMI_CALLBACKS];
    KMBYTE unk_284[4];
    void *stack_pointer;
    KMBYTE unk_28c[32];
    KMUINT32 display_changed;
    KMUINT32 unk_2b0;
    KMUINT32 callback_pending;
    KMDWORD fb_clip_x;
    KMDWORD fb_clip_y;
    KMUINT32 fb_burst_control;
    KMUINT32 fb_read_size;
    KMUINT32 fb_write_stride;
    KMUINT32 fb_read_control;
    KMUINT32 fb_write_control;
    KMDWORD fb_read_start1;
    KMDWORD fb_read_start2;
    KMBYTE unk_2dc[8];
    KMUINT32 sync_control;
    KMUINT32 hblank_interrupt;
    KMUINT32 vblank_interrupt;
    KMUINT32 hblank;
    KMUINT32 sync_load;
    KMUINT32 vblank;
    KMUINT32 sync_width;
    KMUINT32 y_coefficients;
    KMUINT32 scaler_control;
    KMUINT32 video_control;
    KMUINT32 video_start_x;
    KMUINT32 video_start_y;
    KMBYTE unk_314[4];
    /* the word kmiInitializeDevice writes to the chip's feed configuration
     * register on core 6. on core 5 the field stays 0, and the register goes
     * unwritten */
    KMDWORD feed_config;
    KMUINT32 picture_end;
    KMUINT32 picture_start;
    KMUINT32 picture_lines;
    KMBYTE unk_328[16];
    KMDWORD fb_start[2];
    KMUINT32 unk_340[2];
    KMUINT32 latency_model;
    KMBYTE unk_34c[4];
    KMINT32 slot_state[KMI_FRAME_SLOTS];
    KMINT32 write_slot;
    KMINT32 render_slot;
    KMINT32 ta_slot;
    KMDWORD lists_sent;
    KMDWORD lists_done;
    KMUINT32 vsync_count;
    KMINT32 flip_pending;
    KMUINT32 texture_render;
    KMUINT32 slots_held;
    union kmi_list_starts list_start;
    union kmi_list_sizes list_size;
    KMUINT32 vertex_buffer_model;
    KMDWORD vertex_buffer_list;
    KMI_DMA_HANDLE flush_dma;
    KMI_DMA_HANDLE frame_dma;
    KMUINT32 frame_dma_wait;
    KMBOOLEAN common_modifier;
    KMUINT32 *common_modifier_data;
    KMUINT32 common_modifier_size;
    void *vertex_base;
    KMUINT32 vertex_size;
    KMDWORD unk_44c;
    KMDWORD isp_bytes;
    KMDWORD unk_454;
    KMDWORD unk_458;
    KMDWORD ta_buffer_size;
    KMDWORD tiles_x;
    KMDWORD tiles_y;
    KMDWORD dummy_translucent;
    KMDWORD lists_used;
    struct kmi_ta_bank ta_bank[2];
    KMDWORD opb_downward;
    KMDWORD opaque_opb;
    KMDWORD opaque_modifier_opb;
    KMDWORD translucent_opb;
    KMDWORD translucent_modifier_opb;
    KMDWORD punch_through_opb;
    KMUINT32 list_limit[KMI_LISTS];
    KMINT32 pal_lines[5];
    KMUINT32 no_wait_display_change;
};

/* The interrupt handler counts waits and overflows, tracks the maximum
 * size of each vertex list, and times the last render and TA transfer. */
struct kmi_interrupt_statistics {
    KMDWORD render_wait;
    KMDWORD ta_wait;
    KMDWORD render_timeouts;
    KMDWORD ta_timeouts;
    KMDWORD ol_overflow_wait;
    KMDWORD isp_overflow_wait;
    KMDWORD hold_wait;
    KMDWORD ol_overflows;
    KMDWORD isp_overflows;
    KMDWORD isp_current[2];
    KMDWORD list_max[KMI_LISTS];
    KMDWORD render_start;
    KMDWORD render_time;
    KMDWORD ta_start;
    KMDWORD ta_time;
};

/* Interrupt handlers update the same record that frame and DMA clients poll. */
extern volatile struct kmi_device_extension kmiDeviceExtension;
/* The installed driver's device record, or null before setup and after release.
 */
extern volatile struct kmi_device_extension *volatile kmiHardwareResource;

/* Display mode flags for CRT output. */
#define KMI_INTERLACE 4
#define KMI_PSEUDO_NONINTERLACE 8
#define KMI_HEIGHT_480 0x20
/* display_mode's low bits, which hold its video output: KM_NTSC, KM_VGA or
 * KM_PAL */
#define KMI_VIDEO_MASK 3
/* display_mode's bit for 640 pixels across, where clear is 320 */
#define KMI_WIDTH_640 0x10
/* display_mode's bit that, with KM_PAL, takes PAL to one of the five heights
 * in pal_lines, which pal_selection picks among. KMI_PAL_EXTENDED_MODE tests
 * for both bits */
#define KMI_EXTENDED_LINES 0x40
#define KMI_PAL_EXTENDED (KM_PAL | KMI_EXTENDED_LINES)
#define KMI_PAL_EXTENDED_MODE                                                  \
    ((kmiDeviceExtension.display_mode & KMI_PAL_EXTENDED) == KMI_PAL_EXTENDED)
#define KMI_FIELD_MODES                                                        \
    (KMI_INTERLACE | KMI_PSEUDO_NONINTERLACE | KMI_HEIGHT_480)
/* @hack spelling: turn the VGA test into one or zero with a conditional. */
#define KMI_VGA_MODE                                                           \
    ((kmiDeviceExtension.display_mode & KM_VGA) == KM_VGA ? 1 : 0)
/* @hack spelling: nested conditionals for the two field-mode tests. */
#define KMI_TWO_FIELDS                                                         \
    ((kmiDeviceExtension.display_mode & KM_VGA) == KM_VGA  ? 0                 \
     : (kmiDeviceExtension.display_mode & KMI_FIELD_MODES) ? 1                 \
                                                           : 0)

/* A 480-line TV mode without either interlace flag uses half the lines. */
/* @hack spelling: nested conditionals for the three display-mode tests. */
#define KMI_FLICKER_FREE                                                       \
    ((kmiDeviceExtension.display_mode & KM_VGA) == KM_VGA ? 0                  \
     : (kmiDeviceExtension.display_mode & KMI_FIELD_MODES) == KMI_HEIGHT_480   \
         ? 1                                                                   \
         : 0)

typedef KMINT32 (*KMI_IRQ_HANDLER)(void);
void kmiRegisterPhIntHandler(KMI_IRQ_HANDLER handler);
typedef KMINT32 (*KMI_BACKGROUND_WRITER)(void *vertices[3]);
extern KMI_BACKGROUND_WRITER kmiBackGroundPlaneFuncs[18];
KMSTATUS kmiInitializeBackGroundPlane(void);
void kmiInitInterrupt(void);
void kmiInitRenderer(void);
/* kmiFatalErrorEntry's codes: a DMA request that found the queue full, a list
 * past its limit, a bank without room, and a flip that never comes */
#define KMI_FATAL_DMA_QUEUE_FULL 1
#define KMI_FATAL_LIST_OVERFLOW 2
#define KMI_FATAL_BANK_OVERFLOW 3
#define KMI_FATAL_FLIP_TIMEOUT 4
void kmiFatalErrorEntry(KMDWORD code);
void kmiExCallBackFunc(PKMCALLBACKFUNC callback, void *argument);
KMINT32 kmiDMAHazardReset(void);
KMINT32 kmiExecuteDMAChain(void);
/* the orders kmiCreateRegionArray writes a bank's tiles in */
#define KMI_BY_COLUMNS 0
#define KMI_BY_ROWS 1
/* write bank's region array for width by height tiles, taken in order,
 * KMI_BY_COLUMNS or KMI_BY_ROWS. the array starts slot * width * height
 * entries past the bank's region_base. unless ta_busy is set, the call first
 * loads the tile accelerator's registers for the bank. under KMI_FLUSH_ENTRY,
 * while dummy_translucent is set, the call also writes the dummy translucent
 * object. it goes 4 * width * height entries past region_base */
void kmiCreateRegionArray(KMDWORD width, KMDWORD height, KMDWORD slot,
                          KMINT32 unused, KMINT32 bank, KMDWORD order);
KMUINT32 kmiDMAtoTARequest(KMDWORD target, const KMDWORD *source, KMUINT32 size,
                           KMDWORD flags, KMI_DMA_HANDLE **handle);
KMINT32 kmiQueryFinishDMA(KMI_DMA_HANDLE handle);
KMINT32 kmiWaitForFinishDMA(KMI_DMA_HANDLE handle);
void kmiInitializeDriver(void);
void kmiInitializeHardware(void);
void kmiDiscardRenderPipeline(void);
void kmiInitializeTA(void);
void kmiInitializeDMA(void);
/* kmiSetTAEnvironment's options bit, which ta_options keeps, that orders each
 * region array's tiles by columns */
#define KMI_OPTION_BY_COLUMNS 1
void kmiSetTAEnvironment(KMUINT32 region, KMUINT32 objects, KMUINT32 isp,
                         KMUINT32 tsp, KMUINT32 options, KMUINT32 buffer);
void kmiWaitVlank(void);
void kmiResetBoard(void);
void kmiInitPAL(void);
void kmiInitNTSC(void);
void kmiInitVGA(void);
KMINT32 kmiFBControl(KMINT32 width, KMINT32 height, KMBPPMODE format,
                     KMBOOLEAN flicker_filter, KMBOOLEAN dither,
                     KMBOOLEAN anti_alias);
KMINT32 kmiEnableHSyncInterrupt(KMBOOLEAN enable);
KMINT32 kmiEnableVSyncInterrupt(KMBOOLEAN enable);
void kmiInitCallback(void);
void kmiDefaultInterruptHandler(KMINT32 slot);
void kmiModuleInitialize(void);
void kmiUnloadDevice(void);
/* Set up the video chip and clear video RAM. The board must have core 6
 * and the caller must already have selected a video mode. */
KMINT32 kmiInitializeDevice(void);
/* Set density and color and fill the 128 pairs of the fog ramp. */
void kmiInitializeFog(void);
/* the video chip's registers that several of Kamui's members use, as offsets
 * into its register block */
#define KMI_SOFT_RESET 0x008
#define KMI_FB_READ_CONTROL 0x044
#define KMI_FB_WRITE_CONTROL 0x048
#define KMI_FB_WRITE_STRIDE 0x04c
#define KMI_FB_READ_SIZE 0x05c
#define KMI_FB_CLIP_X 0x068
#define KMI_FB_CLIP_Y 0x06c
#define KMI_SHADOW_SCALE 0x074
#define KMI_CULL_VALUE 0x078
#define KMI_BACKGROUND_DEPTH 0x088
#define KMI_BACKGROUND_TAG 0x08c
#define KMI_FOG_TABLE_COLOR 0x0b0
#define KMI_FOG_VERTEX_COLOR 0x0b4
#define KMI_FOG_DENSITY 0x0b8
#define KMI_FOG_CLAMP_MAX 0x0bc
#define KMI_FOG_CLAMP_MIN 0x0c0
#define KMI_HBLANK_INTERRUPT 0x0c8
#define KMI_TEXTURE_CONTROL 0x0e4
#define KMI_VIDEO_CONTROL 0x0e8
#define KMI_VIDEO_START_X 0x0ec
#define KMI_VIDEO_START_Y 0x0f0
#define KMI_SCALER_CONTROL 0x0f4
#define KMI_SYNC_STATUS 0x10c
#define KMI_TA_OL_BASE 0x124
#define KMI_TA_ISP_BASE 0x128
#define KMI_TA_OL_LIMIT 0x12c
#define KMI_TA_ISP_LIMIT 0x130
#define KMI_TA_LIST_INIT 0x144
#define KMI_TA_YUV_BASE 0x148
#define KMI_TA_YUV_CONTROL 0x14c
#define KMI_TA_NEXT_OPB_INIT 0x164
#define KMI_FOG_TABLE 0x200
/* KMI_FB_READ_CONTROL's bit that turns the display's frame buffer reads on,
 * and KMI_VIDEO_CONTROL's that blanks the picture */
#define KMI_READ_ENABLE 1
#define KMI_VIDEO_BLANK 8
/* Write value to the register at byte offset reg and return TRUE. */
KMINT32 kmiWriteRegister(KMDWORD reg, KMDWORD value);
/* Read the register at byte offset reg. */
KMDWORD kmiReadRegister(KMDWORD reg);
/* Write data as kmiWriteRegister does and return TRUE. */
KMINT32 kmiVerifyWriteRegister(KMDWORD reg, KMDWORD data);
void kmiPurgeCache(void *address);
KMINT32 kmiCopyToPVRMemory(void *destination, void *source, KMDWORD size);
KMINT32 kmiCopyFromPVRMemory(void *destination, void *source, KMDWORD size);
KMINT32 kmiCopyFromFrameBuffer(KMDWORD *destination, KMDWORD offset,
                               KMDWORD size);
/* Copy size bytes from source into video memory at byte offset offset. */
KMINT32 kmiCopyToFrameBuffer(KMDWORD offset, KMDWORD *source, KMDWORD size);
/* key's value in section of Kamui's profile, which holds one setting:
 * "EnableTexture16MB" of "HARDWARE", 0. a key of any other section gives
 * fallback, and another key of "HARDWARE" no defined value */
KMDWORD kmiGetProfileInt(const char *section, const char *key,
                         KMDWORD fallback);

/* copy Size bytes from SrcAdr to dstPtr, by pairs of longs where all three
 * are multiples of 8. otherwise copy Size / 4 longs, as kmiMemCopy4 does.
 * dstPtr and SrcAdr must be 4-byte aligned, and Size at least 4. FPSCR's SZ
 * bit must be clear, and is clear again on return. a copy by pairs changes
 * FR0 and FR1 */
void kmiMemCopy8(KMDWORD *dstPtr, const KMDWORD *SrcAdr, KMDWORD Size);
/* copy size / 4 longs from src to dst. dst and src must be 4-byte aligned,
 * and size at least 4 */
void kmiMemCopy4(void *dst, const void *src, KMDWORD size);

#endif
