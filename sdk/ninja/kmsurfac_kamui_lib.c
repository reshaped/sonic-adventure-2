/* Kamui's renders and frame flips. the write slot, write_slot, is the frame
 * slot the lists are written for. kmRender ends the lists written since the
 * last render. it records their starts and sizes in the write slot.
 * kmFlipFrameBuffer then queues the slot and makes the next slot the write
 * slot. under KM_LATENCYMODEL_3V, the interrupt handler sends a queued slot's
 * lists to the tile accelerator. under KM_LATENCYMODEL_2V, kmFlipFrameBuffer
 * sends them itself, by DMA. kmRenderTexture renders the lists into a texture
 * instead */
#include <kamui.h>
#include <kamui_buffer.h>
#include <kamui_internal.h>
#include <machine.h>
#include <sg_xpt.h>

/* the store queue address control registers. a store to SQ_ADDRESS(address)
 * goes to a store queue. a prefetch of that address then writes the queue's
 * SQ_BYTES to address. QACR0 and QACR1 must first hold address shifted right by
 * QACR_AREA_SHIFT */
#define QACR0 (*(volatile KMUINT32 *)0xff000038)
#define QACR1 (*(volatile KMUINT32 *)0xff00003c)
#define QACR_AREA_SHIFT 24
#define SQ_AREA 0xe0000000UL
#define SQ_OFFSET 0x03ffffff
#define SQ_BYTES 32
#define SQ_ADDRESS(address) (((KMUINT32)(address) & SQ_OFFSET) | SQ_AREA)
#define TCNT0 (*(volatile KMUINT32 *)0xffd8000c)

/* the video chip's registers, as offsets into its register block */
#define START_RENDER 0x014
#define PARAM_BASE 0x020
#define REGION_BASE 0x02c
#define FB_READ_START1 0x050
#define FB_READ_START2 0x054
#define FB_WRITE_START1 0x060
#define FB_WRITE_START2 0x064
/* KMI_SOFT_RESET's bits, and KMI_TA_LIST_INIT's bit that starts a frame's
 * lists */
#define RESET_TA 1
#define RESET_RENDERER 2
#define LIST_INIT_START 0x80000000
/* KMI_FB_WRITE_CONTROL's pack mode, and its values for the three formats a
 * render into a texture takes */
#define FB_PACK 7
#define FB_PACK_RGB565 1
#define FB_PACK_ARGB4444 2
#define FB_PACK_ARGB1555 3
/* FB_WRITE_START1's bit that a render into a texture sets above the texture's
 * offset */
#define TEXTURE_WRITE 0x01000000
/* KMI_TEXTURE_CONTROL's stride field: the width of every stride texture, in
 * units of 1 << STRIDE_SHIFT texels */
#define STRIDE 0x1f
#define STRIDE_SHIFT 5
/* a line of 16-bit texels in KMI_FB_WRITE_STRIDE's units of 64 bits */
#define WRITE_STRIDE(texels) ((texels) << 4 >> 6)

/* the tile accelerator's polygon input, as a DMA target */
#define TA_INPUT 0x10000000UL
/* the bytes and the words of the parameter that ends each list */
#define END_OF_LIST 32
#define END_OF_LIST_WORDS 8
/* a parameter control word's list type, LIST_TYPE_VALUE from LIST_TYPE_SHIFT.
 * a new type replaces LIST_TYPE_MASK, which takes bit 27 too */
#define LIST_TYPE_SHIFT 24
#define LIST_TYPE_MASK 0x0f000000
#define LIST_TYPE_VALUE 7
/* a polygon's parameter type, in its control word */
#define PARA_POLYGON_WORD 0x80000000UL
#define WORD_BYTES 4
#define LIST_USED(list) ((1 << (list)) & kmiDeviceExtension.lists_used)
/* vertex_buffer_model's bit that every NOBUF model sets. under
 * KM_LATENCYMODEL_2V, such a model's list, vertex_buffer_list, goes to the
 * tile accelerator as it is written */
#define NOBUF 0x20
#define PIXEL_BYTES 2 /* a 16-bit pixel's bytes */
/* KMI_BACKGROUND_DEPTH drops a depth's low four bits */
#define DEPTH_LOW_BITS 15
/* a write of any value to START_RENDER starts a render */
#define START_VALUE 0xffffffff

/* kmiGetSysConfigFlag's value once the system configuration is set up. before
 * then, CONFIG_ACTIVATED marks a frame buffer pair activated, and
 * CONFIG_COMPLETE every part requested */
#define CONFIG_READY 0x80000000UL
#define CONFIG_ACTIVATED 0x800
#define CONFIG_COMPLETE 0xb00
/* after a timeout, kmFlipFrameBuffer clears render_busy and waits this many
 * more vertical blanks for the flip */
#define RECOVERY_VBLANKS 20
/* a skip for kmiReqRestOfDMA that names no list, so that it sends them all */
#define NO_LIST 0xff
/* kmRenderTexture's results for a wait that times out. once it sets
 * texture_render, the slots are held. BEFORE_HOLD_TIMEOUT is a wait before
 * then, for the tile accelerator or the renderer. AFTER_HOLD_TIMEOUT is a wait
 * for the renderer after it. RENDER_TIMEOUT is a wait for kmRenderTexture's
 * own render, under KM_LATENCYMODEL_2V */
#define BEFORE_HOLD_TIMEOUT ((KMSTATUS)(-1))
#define AFTER_HOLD_TIMEOUT ((KMSTATUS)(-2))
#define RENDER_TIMEOUT ((KMSTATUS)(-3))

/* a polygon's global parameter, which its vertices follow */
struct global_parameter {
    KMUINT32 control;
    KMUINT32 isp;
    KMUINT32 tsp;
    KMUINT32 texture;
    KMUINT32 reserved[4];
};
struct dummy_polygon {
    struct global_parameter header;
    KMVERTEX1 vertex[3];
};
/* the frame buffer write registers of a render into a texture */
struct texture_target {
    KMUINT32 control;
    KMUINT32 stride;
    KMUINT32 address;
    KMDWORD clip_x;
    KMDWORD clip_y;
    KMUINT32 unused;
};
/* names Kamui exports that no header declares */
/* NOLINTBEGIN(misc-use-internal-linkage) */
extern struct dummy_polygon kmiDummyOpaque;
extern struct texture_target kmiTextureRenderParam;
/* until the next kmRender or kmRenderTexture, have each kmFlipFrameBuffer
 * send no lists. the result is direct_render's address, cast to a KMSTATUS */
KMSTATUS kmRenderDirect(void);
/* render the lists into surface, then start them over. surface must be an
 * ARGB1555, RGB565 or ARGB4444 texture of KM_BITDEPTH_16 that isn't twiddled.
 * a twiddled surface, or one of another BitDepth, returns
 * KMSTATUS_INVALID_TEXTURE_TYPE. a wait that times out returns one of the
 * timeout results above */
KMSTATUS kmRenderTexture(PKMSURFACEDESC surface);
/* render into surface, as kmRenderTexture does, what the tile accelerator
 * already holds. a wait that times out returns the address of the timeout
 * field it counted against, not a status */
KMSTATUS kmRenderTextureDirect(PKMSURFACEDESC surface);
KMSTATUS kmSetFramebufferTexture(PKMSURFACEDESC surface);
/* write kmiDummyOpaque at the opaque list's write pointer, through the store
 * queues, and move the pointer past it */
void kmiSetOpaqueDummy(void);
/* send by DMA each list of the write slot but list skip, with its end-of-list
 * parameter. a list that holds nothing isn't sent. while common_modifier is 1,
 * the common modifier volume goes ahead of each modifier list, with that list's
 * type */
void kmiReqRestOfDMA(KMUINT32 skip);
/* render ta_slot's bank into the frame buffer target describes, once the tile
 * accelerator is idle. render_busy is set first. where the tile accelerator
 * stays busy past vertex_timeout, no render starts. that path writes the
 * device record's values back to each write register but FB_WRITE_START1,
 * which keeps target's address. it clears ta_busy and texture_render, leaves
 * render_busy set, and flips the vertex buffer */
void kmiStartRenderTextureEntry(struct texture_target *target);
/* NOLINTEND(misc-use-internal-linkage) */

static long dat_8c17ee78 = 0;
/* the vertex buffer bank, 0 or 1, that this file's calls give
 * kmiFlipVertexBuffer. kmRender switches it before its call.
 * kmSetFramebufferTexture takes it as a frame buffer's index */
static long vertex_bank = 0;
/* 1 from kmRenderDirect until the next kmRender or kmRenderTexture. while it
 * is 1, kmFlipFrameBuffer sends no lists, and hands the new write slot to the
 * tile accelerator */
static long direct_render = 0;
/* list_end[0] is the end-of-list parameter's control word, the one word of
 * the parameter that a list's end writes */
static const KMUINT32 list_end[2] = {0, 0x40000000};
/* a polygon header with no vertices after it. a list cut back for running past
 * its limit holds one. so does a NOBUF model's list that holds nothing */
#define EMPTY_POLYGON                                                          \
    {PARA_POLYGON_WORD,                                                        \
     0,                                                                        \
     0x00800400,                                                               \
     0,                                                                        \
     {0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}}
/* the polygon kmiSetOpaqueDummy writes, an opaque triangle. one corner is at
 * the origin, and one 2048 pixels along each axis */
struct dummy_polygon kmiDummyOpaque = {
    {0x80040012,
     0x00800000,
     0x20800000,
     0,
     {0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},
    {{KM_VERTEXPARAM_NORMAL, 0.0F, 0.0F, 1.0e-10F, 1.0F, 1.0F, 0.0F, 0.0F},
     {KM_VERTEXPARAM_NORMAL, 2048.0F, 0.0F, 1.0e-10F, 1.0F, 0.0F, 1.0F, 0.0F},
     {KM_VERTEXPARAM_ENDOFSTRIP, 0.0F, 2048.0F, 1.0e-10F, 1.0F, 0.0F, 0.0F,
      1.0F}}};
struct texture_target kmiTextureRenderParam;

void kmiSetOpaqueDummy(void)
{
    KMUINT32 address = (KMUINT32)kmiCurrentVertexBuff[KM_OPAQUE_POLYGON];
    /* @hack redundant: register on source */
    register KMBYTE *source = (KMBYTE *)&kmiDummyOpaque;
    KMUINT32 queue;
    KMINT32 line;
    QACR0 = address >> QACR_AREA_SHIFT;
    QACR1 = address >> QACR_AREA_SHIFT;
    queue = SQ_ADDRESS(address);
    for (line = 0; line < (KMINT32)(sizeof(kmiDummyOpaque) / SQ_BYTES);
         line++) {
        kmiMemCopy8((KMDWORD *)queue, (const KMDWORD *)source, SQ_BYTES);
        prefetch((void *)queue);
        queue += SQ_BYTES;
        source += SQ_BYTES;
    }
    kmiCurrentVertexBuff[KM_OPAQUE_POLYGON] +=
        sizeof(struct dummy_polygon) / sizeof(KMDWORD);
}

KMSTATUS kmActivateFrameBuffer(PKMSURFACEDESC front, PKMSURFACEDESC back,
                               KMBOOLEAN strip_buffer, KMBOOLEAN vsync)
{
    KMSTATUS result = KMSTATUS_SUCCESS;
    if (front == NULL || back == NULL) {
        return KMSTATUS_INVALID_SURFACE;
    }
    kmiSystemConfig.front = front;
    kmiSystemConfig.back = back;
    kmiSystemConfig.vsync = vsync;
    kmiSystemConfig.strip_buffer = strip_buffer;
    kmiDeviceExtension.strip_buffer = strip_buffer;
    if (kmiGetSysConfigFlag() == CONFIG_READY) {
        if (front->fSurfaceFlags != back->fSurfaceFlags) {
            return KMSTATUS_INVALID_SURFACE_TYPE;
        }
        if (front->uSize.dwFrameBufferSize != back->uSize.dwFrameBufferSize) {
            return KMSTATUS_INVALID_SURFACE_SIZE;
        }
        kmiDeviceExtension.fb_start[0] = (KMUINT32)front->pSurface;
        kmiDeviceExtension.unk_340[0] = 0;
        kmiDeviceExtension.fb_start[1] = (KMUINT32)back->pSurface;
        kmiDeviceExtension.unk_340[1] = 0;
        kmiWriteRegister(FB_READ_START1, kmiDeviceExtension.fb_read_start1);
        kmiWriteRegister(FB_READ_START2, kmiDeviceExtension.fb_read_start1);
        kmiWriteRegister(FB_WRITE_START1, kmiDeviceExtension.fb_start[0]);
        kmiWriteRegister(FB_WRITE_START2, kmiDeviceExtension.fb_start[1]);
        kmiDeviceExtension.fb_read_start1 = kmiDeviceExtension.fb_start[0];
        kmiDeviceExtension.fb_read_start2 = kmiDeviceExtension.fb_start[1];
        if (strip_buffer == TRUE) {
            kmiDeviceExtension.field_offset[0] = 0;
            kmiDeviceExtension.field_offset[1] = 0;
            kmiWriteRegister(FB_READ_START1, kmiDeviceExtension.fb_read_start1);
            kmiWriteRegister(FB_READ_START2, kmiDeviceExtension.fb_read_start2);
        } else {
            kmiWriteRegister(FB_READ_START1, kmiDeviceExtension.fb_read_start1);
            kmiWriteRegister(FB_READ_START2, kmiDeviceExtension.fb_read_start1);
        }
        kmiDeviceExtension.display_changed = 1;
        kmiDeviceExtension.wait_vsync = vsync;
    } else if (kmiAddSysConfigFlag(CONFIG_ACTIVATED) == CONFIG_COMPLETE) {
        kmiSetSysConfigFlag(CONFIG_READY);
        result = kmiAllocSysConfig(kmiSystemConfig.mode);
    }
    return result;
}

#define WAIT_CALLBACK()                                                        \
    kmiDeviceExtension.callback[WAIT_VSYNC_CALLBACK].function(                 \
        kmiDeviceExtension.callback[WAIT_VSYNC_CALLBACK].argument)
/* wait for field to clear, running the wait callback, and set timeout once
 * vsync_count passes limit */
#define WAIT_FOR(field, limit)                                                 \
    while (kmiDeviceExtension.field != 0) {                                    \
        WAIT_CALLBACK();                                                       \
        if (kmiDeviceExtension.vsync_count > (KMUINT32)(limit)) {              \
            timeout = 1;                                                       \
            break;                                                             \
        }                                                                      \
    }
/* ask for a DMA of size bytes from source to the tile accelerator until the
 * queue takes it */
#define SEND_DMA(source, size)                                                 \
    while (kmiDMAtoTARequest(TA_INPUT, (source), (size), KMI_DMA_SOURCE_UP,    \
                             &handle) == KMI_DMA_QUEUE_FULL) {                 \
    }
/* set the tile accelerator up for bank's lists */
#define PROGRAM_TA(bank)                                                       \
    {                                                                          \
        kmiWriteRegister(KMI_SOFT_RESET, RESET_TA);                            \
        kmiWriteRegister(KMI_SOFT_RESET, 0);                                   \
        kmiWriteRegister(KMI_TA_OL_BASE,                                       \
                         kmiDeviceExtension.ta_bank[bank].ol_base);            \
        kmiWriteRegister(KMI_TA_OL_LIMIT,                                      \
                         kmiDeviceExtension.ta_bank[bank].ol_limit);           \
        kmiWriteRegister(KMI_TA_ISP_BASE,                                      \
                         kmiDeviceExtension.ta_bank[bank].isp_base);           \
        kmiWriteRegister(KMI_TA_ISP_LIMIT,                                     \
                         kmiDeviceExtension.ta_bank[bank].isp_limit);          \
        kmiWriteRegister(KMI_TA_NEXT_OPB_INIT,                                 \
                         kmiDeviceExtension.ta_bank[bank].next_opb_init);      \
        kmiWriteRegister(KMI_TA_LIST_INIT, LIST_INIT_START);                   \
        kmiReadRegister(KMI_TA_LIST_INIT);                                     \
    }
/* list 0's start and size in slot. each row of list_start and list_size is one
 * list's, with an entry a slot. the next list's entries are thus
 * KMI_FRAME_SLOTS on */
/* @hack type: the entries reached through plain views, where volatile ones
 * would do */
#define STARTS_AT(slot)                                                        \
    ((KMDWORD **)&kmiDeviceExtension.list_start.word[(slot)])
#define SIZES_AT(slot) ((KMUINT32 *)&kmiDeviceExtension.list_size.word[(slot)])

void kmiReqRestOfDMA(KMUINT32 skip)
{
    KMI_DMA_HANDLE *handle;
    KMUINT32 slot = kmiDeviceExtension.write_slot;
    KMDWORD **start;
    KMUINT32 *size;
    KMUINT32 list;
    start = STARTS_AT(slot);
    size = SIZES_AT(slot);
    for (list = 0; list < KMI_LISTS; list++) {
        if (*size != 0 && skip != list) {
            if (kmiDeviceExtension.common_modifier == TRUE &&
                (list == KM_OPAQUE_MODIFIER || list == KM_TRANS_MODIFIER)) {
                *kmiDeviceExtension.common_modifier_data =
                    (*kmiDeviceExtension.common_modifier_data &
                     ~LIST_TYPE_MASK) |
                    (list << LIST_TYPE_SHIFT);
                SEND_DMA(
                    (const KMDWORD *)kmiDeviceExtension.common_modifier_data,
                    kmiDeviceExtension.common_modifier_size);
            }
            SEND_DMA(*start, *size + END_OF_LIST);
        }
        size += KMI_FRAME_SLOTS;
        start += KMI_FRAME_SLOTS;
    }
}

KMSTATUS kmFlipFrameBuffer(void)
{
    KMI_DMA_HANDLE *handle;
    KMDWORD **start;
    KMUINT32 *size;
    KMUINT32 slot;
    KMBOOLEAN timeout;
    KMUINT32 bank;
    if (!direct_render) {
        if (kmiDeviceExtension.latency_model == KM_LATENCYMODEL_3V) {
            kmiDeviceExtension.slot_state[kmiDeviceExtension.write_slot] =
                KMI_FRAME_QUEUED;
            kmiDeviceExtension.flip_pending = 1;
        } else {
            kmiIntStatisticData.ta_start = TCNT0;
            slot = kmiDeviceExtension.write_slot;
            kmiDeviceExtension.ta_slot = slot;
            kmiDeviceExtension.ta_busy = 1;
            kmiDeviceExtension.lists_done = 0;
            kmiDeviceExtension.lists_sent = kmiDeviceExtension.lists_used;
            kmiDeviceExtension.flip_pending = 1;
            kmiIntStatisticData.ta_wait = 0;
            kmiDeviceExtension.slot_state[slot] = KMI_FRAME_TA;
            if (kmiDeviceExtension.vertex_buffer_model & NOBUF) {
                QACR0 = TA_INPUT >> QACR_AREA_SHIFT;
                QACR1 = TA_INPUT >> QACR_AREA_SHIFT;
                kmiMemCopy4((void *)SQ_ADDRESS(TA_INPUT), &list_end,
                            sizeof(list_end[0]));
                prefetch((void *)SQ_ADDRESS(TA_INPUT));
                kmiReqRestOfDMA(kmiDeviceExtension.vertex_buffer_list);
            } else {
                start = STARTS_AT(kmiDeviceExtension.write_slot);
                size = SIZES_AT(kmiDeviceExtension.write_slot);
                size += kmiDeviceExtension.vertex_buffer_list * KMI_FRAME_SLOTS;
                start +=
                    kmiDeviceExtension.vertex_buffer_list * KMI_FRAME_SLOTS;
                SEND_DMA(*start, *size + END_OF_LIST);
                kmiReqRestOfDMA(kmiDeviceExtension.vertex_buffer_list);
            }
        }
    }
    kmiDeviceExtension.vsync_count = 0;
    timeout = 0;
    WAIT_FOR(flip_pending, kmiDeviceExtension.vertex_timeout +
                               kmiDeviceExtension.render_timeout + 1);
    if (timeout) {
        kmiDeviceExtension.render_busy = 0;
        kmiDeviceExtension.vsync_count = 0;
        timeout = 0;
        /* @hack spelling: a test, then a do-while, where a while loop would
         * do */
        if (kmiDeviceExtension.flip_pending != 0) {
            do {
                WAIT_CALLBACK();
                if (kmiDeviceExtension.vsync_count > RECOVERY_VBLANKS) {
                    timeout = 1;
                    break;
                }
            } while (kmiDeviceExtension.flip_pending != 0);
        }
        if (timeout) {
            kmiFatalErrorEntry(KMI_FATAL_FLIP_TIMEOUT);
        }
    }
    kmiDeviceExtension.write_slot++;
    kmiDeviceExtension.write_slot &= KMI_FRAME_SLOTS - 1;
    kmiDeviceExtension.slot_state[kmiDeviceExtension.write_slot] =
        KMI_FRAME_WRITING;
    if (kmiDeviceExtension.latency_model == KM_LATENCYMODEL_2V ||
        direct_render == 1) {
        bank = kmiDeviceExtension.write_slot & 1;
        PROGRAM_TA(bank);
        kmiDeviceExtension.lists_done = 0;
        kmiDeviceExtension.lists_sent = kmiDeviceExtension.lists_used;
        if (direct_render != 0) {
            kmiDeviceExtension.ta_slot = kmiDeviceExtension.write_slot;
            kmiDeviceExtension.slot_state[kmiDeviceExtension.write_slot] =
                KMI_FRAME_TA;
            kmiDeviceExtension.ta_busy = 1;
            kmiDeviceExtension.slots_held = 1;
        }
    }
    return KMSTATUS_SUCCESS;
}

void kmiStartRenderTextureEntry(struct texture_target *target)
{
    KMINT32 bank;
    KMINT32 index;
    KMBOOLEAN timeout;
    kmiDeviceExtension.render_busy = 1;
    bank = kmiDeviceExtension.ta_slot & 1;
    if (kmiDeviceExtension.capabilities & KMI_RESET_BEFORE_RENDER) {
        kmiWriteRegister(KMI_SOFT_RESET, RESET_RENDERER);
        kmiWriteRegister(KMI_SOFT_RESET, 0);
    }
    /* @hack redundant: index, a copy of bank */
    index = bank;
    kmiWriteRegister(REGION_BASE,
                     kmiDeviceExtension.ta_bank[index].region_base);
    kmiWriteRegister(PARAM_BASE, kmiDeviceExtension.ta_bank[index].isp_base);
    kmiWriteRegister(FB_WRITE_START1, target->address);
    kmiWriteRegister(KMI_FB_WRITE_CONTROL, target->control);
    kmiWriteRegister(KMI_FB_WRITE_STRIDE, target->stride);
    kmiWriteRegister(KMI_FB_CLIP_X, target->clip_x);
    kmiWriteRegister(KMI_FB_CLIP_Y, target->clip_y);
    kmiDeviceExtension.vsync_count = 0;
    timeout = 0;
    WAIT_FOR(ta_busy, kmiDeviceExtension.vertex_timeout);
    if (timeout) {
        kmiWriteRegister(KMI_FB_WRITE_CONTROL,
                         kmiDeviceExtension.fb_write_control);
        kmiWriteRegister(KMI_FB_WRITE_STRIDE,
                         kmiDeviceExtension.fb_write_stride);
        kmiWriteRegister(KMI_FB_CLIP_X, kmiDeviceExtension.fb_clip_x);
        kmiWriteRegister(KMI_FB_CLIP_Y, kmiDeviceExtension.fb_clip_y);
        kmiDeviceExtension.ta_busy = 0;
        kmiDeviceExtension.texture_render = 0;
        kmiFlipVertexBuffer(vertex_bank);
        return;
    }
    kmiWriteRegister(KMI_BACKGROUND_TAG, kmiDeviceExtension.background_tag);
    kmiWriteRegister(KMI_BACKGROUND_DEPTH,
                     kmiDeviceExtension.background_depth & ~DEPTH_LOW_BITS);
    kmiWriteRegister(START_RENDER, START_VALUE);
}

/* set kmiTextureRenderParam.control to the device record's write control, with
 * the pack mode of surface's format. a twiddled surface, or one of another
 * BitDepth than KM_BITDEPTH_16, returns KMSTATUS_INVALID_TEXTURE_TYPE. a format
 * with no pack mode ORs surface's address into control instead */
static KMSTATUS set_render_format(PKMSURFACEDESC surface)
{
    if ((surface->fSurfaceFlags & KM_SURFACEFLAGS_TWIDDLED) ||
        surface->BitDepth != KM_BITDEPTH_16) {
        return KMSTATUS_INVALID_TEXTURE_TYPE;
    }
    /* @hack type: the pack mode held in surface, where a KMUINT32 that starts
     * as surface's address would do */
    switch (surface->PixelFormat & KM_PIXELFORMAT_MASK) {
    case KM_PIXELFORMAT_ARGB1555:
        surface = (PKMSURFACEDESC)FB_PACK_ARGB1555;
        break;
    case KM_PIXELFORMAT_RGB565:
        surface = (PKMSURFACEDESC)FB_PACK_RGB565;
        break;
    case KM_PIXELFORMAT_ARGB4444:
        surface = (PKMSURFACEDESC)FB_PACK_ARGB4444;
        break;
    }
    kmiTextureRenderParam.control =
        (kmiDeviceExtension.fb_write_control & ~FB_PACK) | (KMUINT32)surface;
    return KMSTATUS_SUCCESS;
}
void kmiInitRenderer(void)
{
}

/* record list's size in bytes, and end it with an end-of-list parameter, whose
 * control word alone is written */
#define CLOSE_LIST(list)                                                       \
    {                                                                          \
        *size[list] =                                                          \
            (kmiCurrentVertexBuff[list] - *start[list]) * WORD_BYTES;          \
        kmiMemCopy4(kmiCurrentVertexBuff[list], list_end,                      \
                    sizeof(list_end[0]));                                      \
        kmiCurrentVertexBuff[list] += END_OF_LIST_WORDS;                       \
    }
#define UPDATE_LIST_MAX(list)                                                  \
    if (*size[list] > kmiIntStatisticData.list_max[list]) {                    \
        kmiIntStatisticData.list_max[list] = *size[list];                      \
    }
/* leave list only header, then the end-of-list word. when one list runs past
 * its limit, kmRender empties every list it ends. before each, it sets
 * header's list type to that list's */
#define EMPTY_LIST(list)                                                       \
    {                                                                          \
        kmiMemCopy4(*start[list], &header, sizeof(header));                    \
        kmiMemCopy4(*start[list] + (sizeof(header) / sizeof(KMDWORD)),         \
                    list_end, sizeof(list_end[0]));                            \
        *size[list] = sizeof(header);                                          \
    }

KMSTATUS kmRender(void)
{
    /* @hack type: the entries reached through plain views, where volatile ones
     * would do */
    KMDWORD **start[KMI_LISTS];
    KMUINT32 *size[KMI_LISTS];
    struct global_parameter header = EMPTY_POLYGON;
    KMUINT32 *control;
    KMUINT32 *queue;
    /* @hack redundant: register on slot */
    register KMUINT32 slot;
    /* @hack spelling: opaque and translucent_modifier, through which the two
     * lists' starts are stored, where start[] would do */
    KMDWORD ***opaque;
    KMDWORD ***translucent_modifier;
    KMINT32 list;
    KMINT32 replacement;
    if ((kmiDeviceExtension.capabilities & KMI_DUMMY_OPAQUE) &&
        kmiDeviceExtension.opaque_opb && kmiDeviceExtension.translucent_opb &&
        kmiDeviceExtension.presort) {
        kmiSetOpaqueDummy();
    }
    slot = kmiDeviceExtension.write_slot;
    start[KM_OPAQUE_POLYGON] =
        (KMDWORD **)&kmiDeviceExtension.list_start.list.opaque.slot[slot];
    opaque = &start[KM_OPAQUE_POLYGON];
    start[KM_OPAQUE_MODIFIER] = (KMDWORD **)&kmiDeviceExtension.list_start.list
                                    .opaque_modifier.slot[slot];
    start[KM_TRANS_POLYGON] =
        (KMDWORD **)&kmiDeviceExtension.list_start.list.translucent.slot[slot];
    start[KM_TRANS_MODIFIER] = (KMDWORD **)&kmiDeviceExtension.list_start.list
                                   .translucent_modifier.slot[slot];
    start[KM_PUNCHTHROUGH_POLYGON] = (KMDWORD **)&kmiDeviceExtension.list_start
                                         .list.punch_through.slot[slot];
    size[KM_OPAQUE_POLYGON] =
        (KMUINT32 *)&kmiDeviceExtension.list_size.list.opaque.slot[slot];
    size[KM_OPAQUE_MODIFIER] = (KMUINT32 *)&kmiDeviceExtension.list_size.list
                                   .opaque_modifier.slot[slot];
    size[KM_TRANS_POLYGON] =
        (KMUINT32 *)&kmiDeviceExtension.list_size.list.translucent.slot[slot];
    size[KM_TRANS_MODIFIER] = (KMUINT32 *)&kmiDeviceExtension.list_size.list
                                  .translucent_modifier.slot[slot];
    size[KM_PUNCHTHROUGH_POLYGON] =
        (KMUINT32 *)&kmiDeviceExtension.list_size.list.punch_through.slot[slot];
    **opaque = kmiVertexBuff->opaque;
    *start[KM_OPAQUE_MODIFIER] = kmiVertexBuff->opaque_modifier;
    translucent_modifier = &start[KM_TRANS_MODIFIER];
    *start[KM_TRANS_POLYGON] = kmiVertexBuff->translucent;
    **translucent_modifier = kmiVertexBuff->translucent_modifier;
    *start[KM_PUNCHTHROUGH_POLYGON] = kmiVertexBuff->punch_through;
    if (kmiDeviceExtension.latency_model == KM_LATENCYMODEL_3V) {
        for (list = 0; list < KMI_LISTS; list++) {
            if (LIST_USED(list)) {
                CLOSE_LIST(list);
            }
        }
        for (list = 0; list < KMI_LISTS; list++) {
            if (!LIST_USED(list)) {
                *size[list] = 0;
            }
        }
        for (list = 0, control = &header.control; list < KMI_LISTS; list++) {
            if (LIST_USED(list)) {
                UPDATE_LIST_MAX(list);
                if (*size[list] > kmiDeviceExtension.list_limit[list]) {
                    for (replacement = 0; replacement < KMI_LISTS;
                         replacement++) {
                        if (LIST_USED(replacement)) {
                            *control = (*control & ~LIST_TYPE_MASK) |
                                       (replacement << LIST_TYPE_SHIFT);
                            /* @hack redundant: a store of ~LIST_TYPE_MASK
                             * that never runs */
                            if (0) {
                                header.control = ~LIST_TYPE_MASK;
                            }
                            EMPTY_LIST(replacement);
                        }
                    }
                    kmiFatalErrorEntry(KMI_FATAL_LIST_OVERFLOW);
                    break;
                }
            }
        }
    } else if (kmiDeviceExtension.vertex_buffer_model & NOBUF) {
        for (list = 0, control = &header.control; list < KMI_LISTS; list++) {
            if ((KMDWORD)list != kmiDeviceExtension.vertex_buffer_list) {
                if (LIST_USED(list)) {
                    CLOSE_LIST(list);
                    UPDATE_LIST_MAX(list);
                    if (*size[list] > kmiDeviceExtension.list_limit[list]) {
                        for (replacement = 0; replacement < KMI_LISTS;
                             replacement++) {
                            if ((KMDWORD)replacement !=
                                    kmiDeviceExtension.vertex_buffer_list &&
                                LIST_USED(replacement)) {
                                *control = (*control & ~LIST_TYPE_MASK) |
                                           (replacement << LIST_TYPE_SHIFT);
                                EMPTY_LIST(replacement);
                            } else {
                                *size[replacement] = 0;
                            }
                        }
                        kmiFatalErrorEntry(KMI_FATAL_LIST_OVERFLOW);
                        break;
                    }
                } else {
                    *size[list] = 0;
                }
            }
        }
        /* @hack redundant: a call of kmiMemCopy4 with control that never runs
         */
        if (0) {
            kmiMemCopy4(control, 0, 0);
        }
        if (kmiCurrentVertexBuff[kmiDeviceExtension.vertex_buffer_list] ==
            *start[kmiDeviceExtension.vertex_buffer_list]) {
            QACR0 = TA_INPUT >> QACR_AREA_SHIFT;
            QACR1 = TA_INPUT >> QACR_AREA_SHIFT;
            control = &header.control;
            *control =
                (*control & ~LIST_TYPE_MASK) |
                ((kmiDeviceExtension.vertex_buffer_list & LIST_TYPE_VALUE)
                 << LIST_TYPE_SHIFT);
            queue = (KMUINT32 *)SQ_ADDRESS(
                kmiCurrentVertexBuff[kmiDeviceExtension.vertex_buffer_list]);
            kmiMemCopy4(queue, &header, sizeof(header));
            prefetch(queue);
        }
    } else {
        kmQueryFinishFlushVertexBuffer();
        for (list = 0, control = &header.control; list < KMI_LISTS; list++) {
            CLOSE_LIST(list);
            if (!LIST_USED(list)) {
                *size[list] = 0;
            }
            if (LIST_USED(list)) {
                UPDATE_LIST_MAX(list);
                if (*size[list] > kmiDeviceExtension.list_limit[list]) {
                    for (replacement = 0; replacement < KMI_LISTS;
                         replacement++) {
                        if (LIST_USED(replacement)) {
                            *control = (*control & ~LIST_TYPE_MASK) |
                                       (replacement << LIST_TYPE_SHIFT);
                            EMPTY_LIST(replacement);
                        } else {
                            *size[replacement] = 0;
                        }
                    }
                    kmiFatalErrorEntry(KMI_FATAL_LIST_OVERFLOW);
                    break;
                }
            }
        }
    }
    direct_render = 0;
    vertex_bank = (vertex_bank + 1) & 1;
    kmiFlipVertexBuffer(vertex_bank);
    return KMSTATUS_SUCCESS;
}

KMSTATUS kmRenderDirect(void)
{
    kmiDeviceExtension.flip_pending = 1;
    kmiDeviceExtension.slots_held = 0;
    direct_render = 1;
    return (KMSTATUS)&direct_render;
}

/* kmRenderTexture's own end-of-list control word and empty polygon header */
static const KMUINT32 texture_list_end = 0;
static const struct global_parameter texture_header = EMPTY_POLYGON;

KMSTATUS kmRenderTexture(PKMSURFACEDESC surface)
{
    /* @hack redundant: unused, 32 bytes that only size the frame */
    KMBYTE unused[32];
    struct global_parameter header;
    KMDWORD saved_clip_y;
    KMDWORD saved_clip_x;
    KMUINT32 saved_tag;
    KMUINT32 *queue;
    /* @hack redundant: register on slot */
    register KMUINT32 slot;
    KMUINT32 bank;
    KMDWORD *volatile *start;
    KMUINT32 *size;
    KMUINT32 list;
    KMSTATUS result;
    KMBOOLEAN timeout;
    KMUINT32 *control;
    void *stack;
    /* @hack type: the header's address kept as a KMUINT32, where &header would
     * do */
    KMUINT32 header_address;
    header_address = (KMUINT32)&header;
    header = texture_header;
    if (kmiDeviceExtension.latency_model == KM_LATENCYMODEL_2V) {
        QACR0 = TA_INPUT >> QACR_AREA_SHIFT;
        QACR1 = TA_INPUT >> QACR_AREA_SHIFT;
        control = (KMUINT32 *)header_address;
        *control = (*control & ~LIST_TYPE_MASK) |
                   ((kmiDeviceExtension.vertex_buffer_list & LIST_TYPE_VALUE)
                    << LIST_TYPE_SHIFT);
        queue = (KMUINT32 *)SQ_ADDRESS(
            kmiCurrentVertexBuff[kmiDeviceExtension.vertex_buffer_list]);
        kmiMemCopy4(queue, (void *)header_address, sizeof(header));
        prefetch(queue);
    }
    result = set_render_format(surface);
    if (result != KMSTATUS_SUCCESS) {
        kmiFlipVertexBuffer(vertex_bank);
        return result;
    }
    if (surface->fSurfaceFlags & KM_SURFACEFLAGS_STRIDE) {
        kmiTextureRenderParam.stride = WRITE_STRIDE(
            (kmiReadRegister(KMI_TEXTURE_CONTROL) & STRIDE) << STRIDE_SHIFT);
    } else {
        kmiTextureRenderParam.stride = WRITE_STRIDE(surface->u0.Width);
    }
    kmiTextureRenderParam.address = (KMUINT32)surface->pSurface | TEXTURE_WRITE;
    slot = kmiDeviceExtension.write_slot;
    bank = slot & 1;
    start = &kmiDeviceExtension.list_start.word[slot];
    size = SIZES_AT(slot);
    if (kmiDeviceExtension.latency_model == KM_LATENCYMODEL_3V) {
        kmiDeviceExtension.vsync_count = 0;
        timeout = 0;
        while (kmiDeviceExtension.ta_busy != 0) {
            WAIT_CALLBACK();
            if (kmiDeviceExtension.vsync_count >
                (KMUINT32)kmiDeviceExtension.vertex_timeout) {
                timeout = 1;
                /* @hack redundant: a store of bank that never runs */
                if (0) {
                    kmiTextureRenderParam.unused = bank;
                }
                break;
            }
        }
        if (timeout) {
            kmiDeviceExtension.texture_render = 0;
            kmiDeviceExtension.ta_busy = 0;
            kmiFlipVertexBuffer(vertex_bank);
            return BEFORE_HOLD_TIMEOUT;
        }
    }
    kmiDeviceExtension.list_start.list.opaque.slot[slot] =
        kmiVertexBuff->opaque;
    kmiDeviceExtension.list_start.list.opaque_modifier.slot[slot] =
        kmiVertexBuff->opaque_modifier;
    kmiDeviceExtension.list_start.list.translucent.slot[slot] =
        kmiVertexBuff->translucent;
    kmiDeviceExtension.list_start.list.translucent_modifier.slot[slot] =
        kmiVertexBuff->translucent_modifier;
    kmiDeviceExtension.list_start.list.punch_through.slot[slot] =
        kmiVertexBuff->punch_through;
    for (list = 0; list < KMI_LISTS; list++) {
        *size = (kmiCurrentVertexBuff[list] - *start) * WORD_BYTES;
        if (kmiDeviceExtension.latency_model == KM_LATENCYMODEL_3V ||
            list != kmiDeviceExtension.vertex_buffer_list) {
            if (LIST_USED(list)) {
                kmiMemCopy4(kmiCurrentVertexBuff[list], &texture_list_end,
                            sizeof(texture_list_end));
                kmiCurrentVertexBuff[list] += END_OF_LIST_WORDS;
                if (*size > kmiDeviceExtension.list_limit[list]) {
                    kmiFatalErrorEntry(KMI_FATAL_LIST_OVERFLOW);
                }
            }
        }
        size += KMI_FRAME_SLOTS;
        start += KMI_FRAME_SLOTS;
    }
    saved_tag = kmiDeviceExtension.background_tag;
    kmiDeviceExtension.vsync_count = 0;
    timeout = 0;
    WAIT_FOR(render_busy, kmiDeviceExtension.render_timeout);
    if (timeout) {
        kmiDeviceExtension.ta_busy = 0;
        kmiFlipVertexBuffer(vertex_bank);
        return BEFORE_HOLD_TIMEOUT;
    }
    kmiDeviceExtension.texture_render = 1;
    kmiDeviceExtension.ta_busy = 1;
    if (kmiDeviceExtension.latency_model == KM_LATENCYMODEL_3V) {
        PROGRAM_TA(bank);
    }
    kmiDeviceExtension.vsync_count = 0;
    timeout = 0;
    WAIT_FOR(render_busy, kmiDeviceExtension.render_timeout);
    if (timeout) {
        kmiDeviceExtension.texture_render = 0;
        kmiFlipVertexBuffer(vertex_bank);
        return AFTER_HOLD_TIMEOUT;
    }
    kmiDeviceExtension.ta_slot = slot;
    kmiDeviceExtension.lists_sent = kmiDeviceExtension.lists_used;
    kmiDeviceExtension.lists_done = 0;
    if (kmiDeviceExtension.latency_model == KM_LATENCYMODEL_3V) {
        kmiReqRestOfDMA(NO_LIST);
    } else {
        QACR0 = TA_INPUT >> QACR_AREA_SHIFT;
        QACR1 = TA_INPUT >> QACR_AREA_SHIFT;
        queue = (KMUINT32 *)SQ_ADDRESS(
            kmiCurrentVertexBuff[kmiDeviceExtension.vertex_buffer_list]);
        kmiMemCopy4(queue, &texture_list_end, sizeof(texture_list_end));
        prefetch(queue);
        kmiReqRestOfDMA(kmiDeviceExtension.vertex_buffer_list);
    }
    saved_clip_x = kmiDeviceExtension.fb_clip_x;
    saved_clip_y = kmiDeviceExtension.fb_clip_y;
    kmSetPixelClipping(0, 0,
                       surface->u0.Width > kmiDeviceExtension.display_width
                           ? kmiDeviceExtension.display_width
                           : surface->u0.Width - 1,
                       surface->u1.Height > kmiDeviceExtension.display_height
                           ? kmiDeviceExtension.display_height
                           : surface->u1.Height - 1);
    kmiTextureRenderParam.clip_x = kmiDeviceExtension.fb_clip_x;
    kmiTextureRenderParam.clip_y = kmiDeviceExtension.fb_clip_y;
    kmiDeviceExtension.fb_clip_x = saved_clip_x;
    kmiDeviceExtension.fb_clip_y = saved_clip_y;
    kmiStartRenderTextureEntry(&kmiTextureRenderParam);
    kmiDeviceExtension.background_tag = saved_tag;
    kmiFlipVertexBuffer(vertex_bank);
    if (kmiDeviceExtension.latency_model == KM_LATENCYMODEL_2V) {
        kmiDeviceExtension.vsync_count = 0;
        timeout = 0;
        WAIT_FOR(render_busy, kmiDeviceExtension.render_timeout);
        if (timeout) {
            kmiDeviceExtension.texture_render = 0;
            return RENDER_TIMEOUT;
        }
        PROGRAM_TA(bank);
    }
    direct_render = 0;
#ifdef NONMATCHING
#ifdef DREAMCAST
    __asm__ volatile("mov r15,%0" : "=r"(stack));
#else
    stack = __builtin_frame_address(0);
#endif
#endif
    /* @hack implicit: stack stored unset, where SHC's code stores the stack
     * pointer. the NONMATCHING side reads it */
    /* NOLINTNEXTLINE(clang-diagnostic-uninitialized) */
    kmiDeviceExtension.stack_pointer = stack;
    return KMSTATUS_SUCCESS;
}

#ifdef NONMATCHING
#define RETURN_TIMEOUT(field) return (KMSTATUS) & kmiDeviceExtension.field
#else
/* @hack implicit: a bare return, whose result SHC's code leaves as field's
 * address, where returning that address would do */
#define RETURN_TIMEOUT(field) return
#endif
KMSTATUS kmRenderTextureDirect(PKMSURFACEDESC surface)
{
    KMDWORD saved_clip_y;
    KMDWORD saved_clip_x;
    /* @hack redundant: register on slot */
    register KMUINT32 slot;
    KMSTATUS result;
    KMBOOLEAN timeout;
    result = set_render_format(surface);
    if (result != KMSTATUS_SUCCESS) {
        kmiFlipVertexBuffer(vertex_bank);
        return result;
    }
    kmiTextureRenderParam.stride = WRITE_STRIDE((KMUINT32)surface->u0.Width);
    kmiTextureRenderParam.address = (KMUINT32)surface->pSurface | TEXTURE_WRITE;
    slot = kmiDeviceExtension.write_slot;
    kmiDeviceExtension.vsync_count = 0;
    timeout = 0;
    WAIT_FOR(ta_busy, kmiDeviceExtension.vertex_timeout);
    if (timeout) {
        /* NOLINTNEXTLINE(clang-diagnostic-return-mismatch) */
        RETURN_TIMEOUT(vertex_timeout);
    }
    kmiDeviceExtension.vsync_count = 0;
    timeout = 0;
    WAIT_FOR(render_busy, kmiDeviceExtension.render_timeout);
    if (timeout) {
        kmiDeviceExtension.render_busy = 0;
        /* NOLINTNEXTLINE(clang-diagnostic-return-mismatch) */
        RETURN_TIMEOUT(render_timeout);
    }
    kmiDeviceExtension.texture_render = 1;
    saved_clip_x = kmiDeviceExtension.fb_clip_x;
    saved_clip_y = kmiDeviceExtension.fb_clip_y;
    kmSetPixelClipping(
        0, 0,
        (KMUINT32)surface->u0.Width > kmiDeviceExtension.display_width
            ? kmiDeviceExtension.display_width
            : (KMUINT32)surface->u0.Width,
        (KMUINT32)surface->u1.Height > kmiDeviceExtension.display_height
            ? kmiDeviceExtension.display_height
            : (KMUINT32)surface->u1.Height);
    kmiTextureRenderParam.clip_x = kmiDeviceExtension.fb_clip_x;
    kmiTextureRenderParam.clip_y = kmiDeviceExtension.fb_clip_y;
    kmiDeviceExtension.fb_clip_x = saved_clip_x;
    kmiDeviceExtension.fb_clip_y = saved_clip_y;
    kmiStartRenderTextureEntry(&kmiTextureRenderParam);
    /* @hack spelling: timeout set from vsync_count's assignment, where a
     * statement of its own would do */
    timeout = kmiDeviceExtension.vsync_count = 0;
    WAIT_FOR(render_busy, kmiDeviceExtension.render_timeout);
    if (timeout) {
        kmiDeviceExtension.texture_render = 0;
        /* NOLINTNEXTLINE(clang-diagnostic-return-mismatch) */
        RETURN_TIMEOUT(render_timeout);
    }
    slot &= 1;
    PROGRAM_TA(slot);
    kmiDeviceExtension.lists_done = 0;
    kmiDeviceExtension.lists_sent = kmiDeviceExtension.lists_used;
    kmiDeviceExtension.ta_slot = kmiDeviceExtension.write_slot;
    /* @hack spelling: slots_held set from ta_busy's assignment, where a
     * statement of its own would do */
    kmiDeviceExtension.slots_held = kmiDeviceExtension.ta_busy = 1;
    kmiDeviceExtension.slot_state[kmiDeviceExtension.write_slot] = KMI_FRAME_TA;
    return KMSTATUS_SUCCESS;
}

/* describe frame buffer vertex_bank as a 1024 by 1024 stride texture of the
 * display's 16-bit format. a 24-bit or 32-bit display returns
 * KMSTATUS_INVALID_TEXTURE_TYPE, with surface left as it was */
KMSTATUS kmSetFramebufferTexture(PKMSURFACEDESC surface)
{
    KMINT32 width;
    KMINT32 height;
    if (kmiDeviceExtension.video_mode == KM_VGA) {
        width = 640;
        height = 480;
    } else {
        if (kmiDeviceExtension.display_mode & KMI_WIDTH_640) {
            width = 640;
        } else {
            width = 320;
        }
        if (kmiDeviceExtension.display_mode & KMI_HEIGHT_480) {
            height = 480;
        } else {
            height = 240;
        }
    }
    switch (kmiDeviceExtension.bpp) {
    case KM_DSPBPP_ARGB1555:
        surface->BitDepth = KM_BITDEPTH_16;
        surface->PixelFormat = KM_PIXELFORMAT_ARGB1555;
        break;
    case KM_DSPBPP_RGB565:
        surface->BitDepth = KM_BITDEPTH_16;
        surface->PixelFormat = KM_PIXELFORMAT_RGB565;
        break;
    case KM_DSPBPP_RGB555:
        surface->BitDepth = KM_BITDEPTH_16;
        surface->PixelFormat = KM_PIXELFORMAT_ARGB1555;
        break;
    /* @hack redundant: case labels for two formats the default takes */
    case KM_DSPBPP_ARGB8888:
    case KM_DSPBPP_RGB888:
    default:
        return KMSTATUS_INVALID_TEXTURE_TYPE;
        /* @hack redundant: a break after the return */
        break;
    }
    surface->SurfaceType = KM_SURFACETYPE_TEXTURE;
    surface->u0.USize = 1024;
    surface->u1.VSize = 1024;
    surface->uSize.dwTextureSize = width * height * PIXEL_BYTES;
    surface->fSurfaceFlags = KM_SURFACEFLAGS_STRIDE;
    surface->pSurface =
        (KMDWORD *)(kmiDeviceExtension.fb_start[vertex_bank] << 3);
    return KMSTATUS_SUCCESS;
}

void kmResetPointer(void)
{
    KMUINT32 bank;
    if (kmiDeviceExtension.latency_model == KM_LATENCYMODEL_2V) {
        bank = kmiDeviceExtension.write_slot & 1;
        PROGRAM_TA(bank);
        kmiDeviceExtension.lists_done = 0;
        kmiDeviceExtension.lists_sent = kmiDeviceExtension.lists_used;
    }
    kmiFlipVertexBuffer(vertex_bank);
}
