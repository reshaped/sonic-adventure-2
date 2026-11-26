/* Kamui's interrupt handler. a frame slot passes from the tile accelerator to
 * the renderer, then to the display. slot n uses bank n & 1. the handler marks
 * a stage done at the interrupt that ends it. it moves the waiting slots on
 * at a vertical blank, unless vsync_wait or frames_busy holds them there.
 * before it returns, it runs the callbacks its interrupts queued */
#include <kamui.h>
#include <kamui_internal.h>
#include <sg_xpt.h>

#define REG32(address) (*(volatile KMUINT32 *)(address))
/* @hack type: a register read or written as a KMDWORD, where REG32 would do */
#define REG_LONG(address) (*(volatile KMDWORD *)(address))

/* the system bus's interrupt status. a set bit marks an interrupt pending, and
 * writing 1 to the bit clears it */
#define NORMAL_STATUS REG32(0xa05f6900)
#define RENDER_END 4 /* the TSP's end of a render */
#define RENDER_END_ALL 7 /* the video's, the ISP's and the TSP's ends */
#define VBLANK 8
#define HBLANK 0x20
#define YUV_END 0x40
#define OPAQUE_END 0x80
#define OPAQUE_MODIFIER_END 0x100
#define TRANS_END 0x200
#define TRANS_MODIFIER_END 0x400
#define DMA_END 0x80000
#define PUNCHTHROUGH_END 0x200000
#define ERROR_PENDING REG32(0xa05f6908)
#define STRIP_OVERRUN 2
#define ISP_OVERFLOW 4
#define OL_OVERFLOW 8

#define SOFT_RESET REG32(0xa05f8008)
#define RESET_TA 1
#define RESET_RENDERER 2
#define RESET_SDRAM 4
#define START_RENDER REG32(0xa05f8014)
#define PARAM_BASE REG32(0xa05f8020)
#define REGION_BASE REG32(0xa05f802c)
#define FB_READ_CONTROL REG32(0xa05f8044)
#define FB_WRITE_CONTROL REG32(0xa05f8048)
#define FB_WRITE_STRIDE REG32(0xa05f804c)
#define FB_READ_START1 REG32(0xa05f8050)
#define FB_READ_START2 REG32(0xa05f8054)
#define FB_WRITE_START REG32(0xa05f8060)
#define FB_CLIP_X REG_LONG(0xa05f8068)
#define FB_CLIP_Y REG_LONG(0xa05f806c)
#define BACKGROUND_DEPTH REG32(0xa05f8088)
#define BACKGROUND_TAG REG32(0xa05f808c)
#define VIDEO_CONTROL REG32(0xa05f80e8)
#define SCALER_CONTROL REG32(0xa05f80f4)
#define SCALER_INTERLACE 0x20000
#define SCALER_FIELD 0x40000
#define SYNC_STATUS REG32(0xa05f810c)
#define SYNC_FIELD 0x400 /* the field the display is in */
#define SYNC_FIELD_SHIFT 10
#define TA_OL_BASE REG32(0xa05f8124)
#define TA_ISP_BASE REG32(0xa05f8128)
#define TA_OL_LIMIT REG32(0xa05f812c)
#define TA_ISP_LIMIT REG32(0xa05f8130)
#define TA_ITP_CURRENT REG_LONG(0xa05f8138)
#define TA_LIST_INIT REG32(0xa05f8144)
#define LIST_INIT_START 0x80000000
#define TA_NEXT_OPB_INIT REG32(0xa05f8164)
#define CHCR2 REG32(0xffa0002c)
#define CHCR_ENABLE 1
#define CHCR_TRANSFER_END 2
#ifdef NONMATCHING
#define TCNT0 (*(volatile KMDWORD *)0xffd8000c)
#else
/* @hack type: TCNT0 read through a plain word, where volatile would do */
#define TCNT0 (*(KMDWORD *)0xffd8000c)
#endif

/* video memory's uncached 32-bit area */
#define VIEW_32BIT 0xa5000000
/* the tile accelerator's polygon input, as a DMA target */
#define TA_INPUT 0x10000000
#define END_OF_LIST 32 /* the bytes of the parameter that ends each list */
/* a parameter control word's list type is three bits from LIST_TYPE_SHIFT.
 * SEND_MODIFIER clears LIST_TYPE_MASK, which takes bit 27 too, before it
 * writes one */
#define LIST_TYPE_SHIFT 24
#define LIST_TYPE_MASK 0x0f000000
/* a list type's bit in lists_sent and lists_done */
#define LIST_BIT(type) (1 << (type))
/* a callback's bit in callback_pending */
#define CALLBACK_BIT(slot) (1 << (slot))
/* the words of background_packet that kmiFinalizeRender copies */
#define PACKET_WORDS 48
#define WORD_BYTES 4
/* BACKGROUND_TAG's fields: background_skip's top byte, and from bit 3 the
 * packet's offset into the ISP parameters, in words. TAG_OFFSET takes the
 * offset in bytes */
#define TAG_SKIP 0xff000000
#define TAG_OFFSET_FIELD 0xfffff8
#define TAG_OFFSET(bytes) (((bytes) << 1) & TAG_OFFSET_FIELD)
/* regions_stale's bit for a bank, and for both */
#define STALE_BANK(bank) ((bank) + 1)
#define BOTH_BANKS 3
/* BACKGROUND_DEPTH drops a depth's low four bits */
#define DEPTH_LOW_BITS 15
/* a write of any value to START_RENDER starts a render */
#define START_VALUE 0xffffffff
#define BANKS 2
/* frames_busy counts its checks of a render into a texture. past
 * TEXTURE_TIMEOUT, it restores the frame buffer's write registers and clears
 * render_busy */
#define TEXTURE_TIMEOUT 16

/* set when frames_busy finds a slot under way. vsync_wait then lets the next
 * blank it counts through to frames_busy, whatever vsync_remaining holds. it
 * clears slot_busy at that blank, unless the blank takes vsync_remaining
 * below 0 */
static KMBOOLEAN slot_busy;
/* NOLINTNEXTLINE(misc-use-internal-linkage) */
struct kmi_interrupt_statistics kmiIntStatisticData;
/* the renders frames_busy has stopped for running past render_timeout since a
 * render last ended */
static KMDWORD renders_stopped;
/* TRUE from a render's start until the first strip overrun after it. that
 * overrun queues no callback */
static KMBOOLEAN render_started;
/* the field the last vertical blank began, 0 or 1 */
static KMDWORD current_field;
static KMDWORD field_toggle = 1;

/* names Kamui exports that no header declares */
/* NOLINTBEGIN(misc-use-internal-linkage) */
KMINT32 kmiInterruptHandlerSH(void);
void kmiFinalizeRender(void);
/* NOLINTEND(misc-use-internal-linkage) */

void kmiInitInterrupt(void)
{
    KMDWORD list;

    kmiRegisterPhIntHandler(kmiInterruptHandlerSH);
    kmiDeviceExtension.unk_1f0 = 0;
    kmiDeviceExtension.callback_pending = 0;
    kmiDeviceExtension.texture_render = 0;
    kmiDeviceExtension.slots_held = 0;
    kmiIntStatisticData.render_wait = 0;
    kmiIntStatisticData.ta_wait = 0;
    kmiIntStatisticData.render_timeouts = 0;
    kmiIntStatisticData.ta_timeouts = 0;
    kmiIntStatisticData.ol_overflow_wait = 0;
    kmiIntStatisticData.isp_overflow_wait = 0;
    kmiIntStatisticData.hold_wait = 0;
    kmiIntStatisticData.ol_overflows = 0;
    kmiIntStatisticData.isp_overflows = 0;
    renders_stopped = 0;
    slot_busy = 0;
    for (list = 0; list < KMI_LISTS; list++) {
        kmiIntStatisticData.list_max[list] = 0;
    }
}

static void show_rendered_frame(void)
{
    KMINT32 slot;
    for (slot = 0; slot < KMI_FRAME_SLOTS; slot++) {
        if (kmiDeviceExtension.slot_state[slot] == KMI_FRAME_RENDERED ||
            kmiDeviceExtension.slot_state[slot] == KMI_FRAME_OVERFLOW) {
            kmiDeviceExtension.slot_state[slot] = KMI_FRAME_SHOWN;
            if (KMI_FLICKER_FREE) {
                FB_READ_START1 = kmiDeviceExtension.fb_start[0] +
                                 kmiDeviceExtension.field_offset[0];
                FB_READ_START2 = kmiDeviceExtension.fb_start[1] +
                                 kmiDeviceExtension.field_offset[1];
            } else if (kmiDeviceExtension.strip_buffer != 1) {
                FB_READ_START1 = kmiDeviceExtension.fb_start[slot & 1] +
                                 kmiDeviceExtension.field_offset[0];
                FB_READ_START2 = kmiDeviceExtension.fb_start[slot & 1] +
                                 kmiDeviceExtension.field_offset[1];
            }
            if (kmiDeviceExtension.slot_state[slot] != KMI_FRAME_OVERFLOW &&
                kmiDeviceExtension.display_changed == 1) {
                VIDEO_CONTROL = kmiDeviceExtension.video_control;
                FB_READ_CONTROL = kmiDeviceExtension.fb_read_control;
                kmiDeviceExtension.display_changed = 0;
            }
            break;
        }
    }
}

/* while common_modifier is set, the common modifier volume goes ahead of
 * each modifier list, with that list's type */
#define SEND_MODIFIER(type)                                                    \
    if (kmiDeviceExtension.common_modifier == 1) {                             \
        *kmiDeviceExtension.common_modifier_data =                             \
            (*kmiDeviceExtension.common_modifier_data & ~LIST_TYPE_MASK) |     \
            ((type) << LIST_TYPE_SHIFT);                                       \
        result = kmiDMAtoTARequest(                                            \
            TA_INPUT,                                                          \
            (const KMDWORD *)kmiDeviceExtension.common_modifier_data,          \
            kmiDeviceExtension.common_modifier_size, KMI_DMA_SOURCE_UP,        \
            &handle);                                                          \
        if (result == KMI_DMA_QUEUE_FULL) {                                    \
            return 0;                                                          \
        }                                                                      \
    }
#define NO_MODIFIER
#define SEND_LIST(name, type, before)                                          \
    if (kmiDeviceExtension.list_size.list.name.slot[slot] != 0) {              \
        kmiDeviceExtension.lists_sent |= LIST_BIT(type);                       \
        before result = kmiDMAtoTARequest(                                     \
            TA_INPUT, kmiDeviceExtension.list_start.list.name.slot[slot],      \
            kmiDeviceExtension.list_size.list.name.slot[slot] + END_OF_LIST,   \
            KMI_DMA_SOURCE_UP, &handle);                                       \
        if (result == KMI_DMA_QUEUE_FULL) {                                    \
            return 0;                                                          \
        }                                                                      \
    }

/* if the tile accelerator is idle, send it the first queued slot's nonempty
 * lists. returns 0 where the DMA queue is full, with the slot's later lists
 * unsent, and else 1 */
static KMINT32 start_ta_frame(void)
{
    KMINT32 slot;
    KMINT32 bank;
    KMDWORD result;
    KMI_DMA_HANDLE *handle;
    for (slot = 0; slot < KMI_FRAME_SLOTS; slot++) {
        if (kmiDeviceExtension.slot_state[slot] == KMI_FRAME_QUEUED &&
            kmiDeviceExtension.ta_busy == 0) {
            kmiDeviceExtension.ta_busy = 1;
            kmiDeviceExtension.slot_state[slot] = KMI_FRAME_TA;
            kmiDeviceExtension.ta_slot = slot;
            bank = slot & 1;
            kmiIntStatisticData.ta_start = TCNT0;
            SOFT_RESET = RESET_TA;
            SOFT_RESET = 0;
            TA_OL_BASE = kmiDeviceExtension.ta_bank[bank].ol_base;
            TA_OL_LIMIT = kmiDeviceExtension.ta_bank[bank].ol_limit;
            TA_ISP_BASE = kmiDeviceExtension.ta_bank[bank].isp_base;
            TA_ISP_LIMIT = kmiDeviceExtension.ta_bank[bank].isp_limit;
            TA_NEXT_OPB_INIT = kmiDeviceExtension.ta_bank[bank].next_opb_init;
            TA_LIST_INIT = LIST_INIT_START;
            (void)TA_LIST_INIT;
            kmiDeviceExtension.lists_sent = 0;
            kmiDeviceExtension.lists_done = 0;
            kmiIntStatisticData.ta_wait = 0;
            SEND_LIST(opaque, KM_OPAQUE_POLYGON, NO_MODIFIER);
            SEND_LIST(opaque_modifier, KM_OPAQUE_MODIFIER,
                      SEND_MODIFIER(KM_OPAQUE_MODIFIER));
            SEND_LIST(translucent, KM_TRANS_POLYGON, NO_MODIFIER);
            SEND_LIST(translucent_modifier, KM_TRANS_MODIFIER,
                      SEND_MODIFIER(KM_TRANS_MODIFIER));
            SEND_LIST(punch_through, KM_PUNCHTHROUGH_POLYGON, NO_MODIFIER);
            return 1;
        }
    }
    return 1;
}

/* rebuild bank's region array, in kmiSetTAEnvironment's order */
/* @hack spelling: the order as ta_options' KMI_OPTION_BY_COLUMNS bit
 * inverted, where KMI_BY_COLUMNS or KMI_BY_ROWS chosen by the bit would do */
#define REBUILD_BANK(bank)                                                     \
    kmiCreateRegionArray(                                                      \
        kmiDeviceExtension.tiles_x, kmiDeviceExtension.tiles_y, 0, 1, (bank),  \
        (~kmiDeviceExtension.ta_options & KMI_OPTION_BY_COLUMNS))

static void start_render_frame(void)
{
    KMINT32 slot;
    KMINT32 bank;
    for (slot = 0; slot < KMI_FRAME_SLOTS; slot++) {
        if (kmiDeviceExtension.slot_state[slot] == KMI_FRAME_TA_DONE &&
            kmiDeviceExtension.render_busy == 0) {
            bank = slot & 1;
            if (kmiDeviceExtension.regions_stale & STALE_BANK(bank)) {
                REBUILD_BANK(bank);
                kmiDeviceExtension.regions_stale =
                    ~STALE_BANK(bank) & kmiDeviceExtension.regions_stale &
                    BOTH_BANKS;
            }
            kmiDeviceExtension.render_busy = 1;
            kmiDeviceExtension.slot_state[slot] = KMI_FRAME_RENDERING;
            if (kmiDeviceExtension.capabilities & KMI_RESET_BEFORE_RENDER) {
                SOFT_RESET = RESET_RENDERER;
                SOFT_RESET = 0;
            }
            REGION_BASE = kmiDeviceExtension.ta_bank[bank].region_base;
            PARAM_BASE = kmiDeviceExtension.ta_bank[bank].isp_base;
            if (kmiDeviceExtension.strip_buffer != 1) {
                if (KMI_FLICKER_FREE) {
                    FB_WRITE_START = kmiDeviceExtension.fb_start[current_field];
                } else {
                    FB_WRITE_START = kmiDeviceExtension.fb_start[bank];
                }
            }
            BACKGROUND_TAG = kmiDeviceExtension.background_tag;
            BACKGROUND_DEPTH =
                kmiDeviceExtension.background_depth & ~DEPTH_LOW_BITS;
            FB_CLIP_X = kmiDeviceExtension.fb_clip_x;
            FB_CLIP_Y = kmiDeviceExtension.fb_clip_y;
            FB_WRITE_STRIDE = kmiDeviceExtension.fb_write_stride;
            FB_WRITE_CONTROL = kmiDeviceExtension.fb_write_control;
            kmiIntStatisticData.render_wait = 0;
            if ((kmiDeviceExtension.display_mode & ~(KMI_WIDTH_640 | KM_PAL)) ==
                KMI_HEIGHT_480) {
                if (current_field == 0) {
                    SCALER_CONTROL =
                        kmiDeviceExtension.scaler_control | SCALER_INTERLACE;
                } else {
                    SCALER_CONTROL = kmiDeviceExtension.scaler_control |
                                     (SCALER_INTERLACE | SCALER_FIELD);
                }
            }
            kmiIntStatisticData.render_start = TCNT0;
            kmiDeviceExtension.render_slot = slot;
            render_started = TRUE;
            START_RENDER = START_VALUE;
            break;
        }
    }
}

#pragma inline(vsync_wait, frames_busy)
/* 1 to hold the slots at this vertical blank, and 0 to let it through to
 * frames_busy. in an interlaced mode, a blank that begins field 1 holds them,
 * and vsync_wait doesn't count it */
static KMINT32 vsync_wait(void)
{
    KMINT32 wait;
    /* @hack spelling: the field's test nested in the mode's, and 1 returned
     * through wait, where one test and return 1 would do */
    if ((kmiDeviceExtension.display_mode & KMI_INTERLACE) == KMI_INTERLACE) {
        if (current_field == 1) {
            wait = 1;
            return wait;
        }
    }
    kmiDeviceExtension.vsync_remaining--;
    if (kmiDeviceExtension.vsync_remaining >= 0) {
        if (slot_busy != 0) {
            wait = 0;
            slot_busy = 0;
        } else {
            wait = 1;
        }
    } else {
        kmiDeviceExtension.vsync_remaining = kmiDeviceExtension.vsync_wait;
        wait = 0;
    }
    return wait;
}

#define RESTORE_WRITE_REGISTERS()                                              \
    {                                                                          \
        FB_WRITE_CONTROL = kmiDeviceExtension.fb_write_control;                \
        FB_WRITE_STRIDE = kmiDeviceExtension.fb_write_stride;                  \
        FB_CLIP_X = kmiDeviceExtension.fb_clip_x;                              \
        FB_CLIP_Y = kmiDeviceExtension.fb_clip_y;                              \
    }
#define CHECK_TA_TIMEOUT(slot)                                                 \
    if (kmiDeviceExtension.slot_state[(slot)] == KMI_FRAME_TA) {               \
        kmiIntStatisticData.ta_wait++;                                         \
        if (kmiIntStatisticData.ta_wait >                                      \
            (KMDWORD)kmiDeviceExtension.vertex_timeout) {                      \
            kmiIntStatisticData.ta_timeouts++;                                 \
            kmiDMAHazardReset();                                               \
            kmiDeviceExtension.slot_state[(slot)] = KMI_FRAME_FREE;            \
            kmiDeviceExtension.ta_busy = 0;                                    \
            kmiIntStatisticData.ta_wait = 0;                                   \
        }                                                                      \
    }

/* 1 to hold the slots at this blank, and 0 to let them move. a TA transfer is
 * stopped once more than vertex_timeout checks find it running. a render is
 * stopped once more than render_timeout checks find it running. while
 * render_timeout is 0, the first check stops it */
static KMINT32 frames_busy(void)
{
    KMINT32 slot;
    KMINT32 busy;
    busy = 0;
    if (kmiDeviceExtension.slots_held != 0) {
        return 1;
    }
    if (kmiDeviceExtension.texture_render != 0) {
        kmiIntStatisticData.hold_wait++;
        if (kmiIntStatisticData.hold_wait > TEXTURE_TIMEOUT) {
            RESTORE_WRITE_REGISTERS();
            kmiDeviceExtension.render_busy = 0;
            kmiIntStatisticData.hold_wait = 0;
        }
        /* @hack spelling: 1 returned through busy, where return 1 would do */
        busy = 1;
        return busy;
    }
    /* @hack redundant: an empty loop of one pass */
    do {
    } while (0);
    for (slot = 0; slot < KMI_FRAME_SLOTS; slot++) {
        /* @hack redundant: the else after the continue */
        if (kmiDeviceExtension.slot_state[slot] < 0) {
            continue;
        } else if (!(kmiDeviceExtension.slot_state[slot] & 1)) {
            /* an even state is a stage under way */
            if (kmiDeviceExtension.render_timeout == 0) {
                if (kmiDeviceExtension.slot_state[slot] ==
                    KMI_FRAME_RENDERING) {
                    kmiDeviceExtension.slot_state[slot] = KMI_FRAME_RENDERED;
                    if (kmiDeviceExtension.render_busy != 0) {
                        kmiIntStatisticData.render_timeouts++;
                    }
                    kmiDeviceExtension.render_busy = 0;
                    SOFT_RESET = RESET_RENDERER;
                    SOFT_RESET = 0;
                } else {
                    slot_busy = 1;
                    busy = 1;
                    CHECK_TA_TIMEOUT(slot);
                }
            } else {
                slot_busy = 1;
                busy = 1;
                if (kmiDeviceExtension.slot_state[slot] ==
                    KMI_FRAME_RENDERING) {
                    kmiIntStatisticData.render_wait++;
                    if (kmiIntStatisticData.render_wait >
                        (KMDWORD)kmiDeviceExtension.render_timeout) {
                        kmiIntStatisticData.render_timeouts++;
                        FB_READ_CONTROL = kmiDeviceExtension.fb_read_control &
                                          ~KMI_READ_ENABLE;
                        SOFT_RESET = RESET_RENDERER | RESET_SDRAM;
                        SOFT_RESET = 0;
                        FB_READ_CONTROL = kmiDeviceExtension.fb_read_control;
                        REBUILD_BANK(0);
                        REBUILD_BANK(1);
                        kmiDeviceExtension.slot_state[slot] =
                            KMI_FRAME_RENDERED;
                        kmiDeviceExtension.render_busy = 0;
                        renders_stopped++;
                        kmiIntStatisticData.render_wait = 0;
                    }
                }
                CHECK_TA_TIMEOUT(slot);
            }
        }
    }
    busy |= kmiDeviceExtension.render_busy;
    return busy;
}

/* returns KMI_FATAL_DMA_QUEUE_FULL where start_ta_frame finds the DMA queue
 * full, and else 0 */
static KMINT32 handle_vblank(void)
{
    NORMAL_STATUS = VBLANK;
    kmiDeviceExtension.callback_pending |= CALLBACK_BIT(VSYNC_CALLBACK);
    kmiDeviceExtension.vsync_count++;
    if (kmiDeviceExtension.capabilities & KMI_CAN_READ_SYNC) {
        current_field = (SYNC_STATUS & SYNC_FIELD) >> SYNC_FIELD_SHIFT;
    } else {
        field_toggle = (field_toggle + 1) & 1;
        current_field = field_toggle;
    }
    if (!vsync_wait()) {
        if (!frames_busy()) {
            show_rendered_frame();
            if (!start_ta_frame()) {
                return KMI_FATAL_DMA_QUEUE_FULL;
            }
            start_render_frame();
            kmiDeviceExtension.flip_pending = 0;
        }
    }
    return 0;
}

/* end the TA transfer of ta_slot. kmiInterruptHandlerSH calls it once each
 * list sent is done. unless a TA timeout freed the slot, the background packet
 * goes where the bank's ISP parameters end */
void kmiFinalizeRender(void)
{
    /* @hack type: end holds the ISP parameters' end as an offset, then the
     * address the packet goes to, where a KMDWORD and a pointer would do */
    KMDWORD *end;
    volatile KMUINT32 *from;
    KMINT32 word;
    if (kmiDeviceExtension.slot_state[kmiDeviceExtension.ta_slot] !=
        KMI_FRAME_FREE) {
        if (kmiDeviceExtension.texture_render == 0) {
            kmiDeviceExtension.callback_pending |=
                CALLBACK_BIT(END_VERTEX_CALLBACK);
            kmiDeviceExtension.slot_state[kmiDeviceExtension.ta_slot] =
                KMI_FRAME_TA_DONE;
        }
        if (kmiDeviceExtension.capabilities & KMI_CAN_READ_ITP_CURRENT) {
            end = (KMDWORD *)TA_ITP_CURRENT;
        } else {
            end = (KMDWORD *)kmiDeviceExtension
                      .ta_bank[kmiDeviceExtension.ta_slot & 1]
                      .isp_limit;
        }
        kmiIntStatisticData.isp_current[kmiDeviceExtension.ta_slot & 1] =
            TA_ITP_CURRENT;
        kmiDeviceExtension.background_tag =
            (kmiDeviceExtension.background_skip & TAG_SKIP) |
            TAG_OFFSET(
                (KMDWORD)end -
                kmiDeviceExtension.ta_bank[kmiDeviceExtension.ta_slot & 1]
                    .isp_base);
        from = kmiDeviceExtension.background_packet;
        end = (KMDWORD *)((KMDWORD)end + VIEW_32BIT);
        for (word = 0; word < PACKET_WORDS; word++) {
            /* @hack spelling: from stepped in a statement of its own, where
             * *end++ = *from++ would do */
            *end++ = *from;
            from++;
        }
    }
    kmiDeviceExtension.ta_busy = 0;
    kmiIntStatisticData.ta_time = kmiIntStatisticData.ta_start - TCNT0;
}

/* grow_ta_area's areas and the bytes it moves an area's limit by. it keeps
 * ol_limit more than OL_FLOOR bytes above isp_base */
#define ISP_AREA 0
#define OL_AREA 1
#define ISP_GROWTH 0x2000
#define OL_GROWTH 0x800
#define OL_FLOOR 0x140
#define OL_OVERFLOW_LIMIT 10
/* a bank's isp_limit stays this far below its ol_limit. without
 * KMI_CAN_READ_ITP_CURRENT, kmiFinalizeRender copies the background packet to
 * isp_limit */
#define BACKGROUND_BYTES (PACKET_WORDS * WORD_BYTES)

/* give area room in each bank by moving its limit. a bank without room is a
 * fatal error once isp_overflow_wait passes 0, or ol_overflow_wait passes
 * OL_OVERFLOW_LIMIT. the result is always 0 */
static KMINT32 grow_ta_area(KMINT32 area)
{
    KMINT32 bank;
    switch (area) {
    case ISP_AREA:
        for (bank = 0; bank < BANKS; bank++) {
            if (kmiDeviceExtension.ta_bank[bank].isp_limit + ISP_GROWTH <
                kmiDeviceExtension.ta_bank[bank].ol_base - BACKGROUND_BYTES) {
                kmiDeviceExtension.ta_bank[bank].isp_limit += ISP_GROWTH;
                kmiDeviceExtension.ta_bank[bank].ol_limit =
                    kmiDeviceExtension.ta_bank[bank].isp_limit +
                    BACKGROUND_BYTES;
                kmiIntStatisticData.isp_overflow_wait = 0;
            } else if (kmiIntStatisticData.isp_overflow_wait > 0) {
                kmiFatalErrorEntry(KMI_FATAL_BANK_OVERFLOW);
            }
        }
        break;
    case OL_AREA:
        for (bank = 0; bank < BANKS; bank++) {
            if (kmiDeviceExtension.ta_bank[bank].ol_limit - OL_GROWTH >
                kmiDeviceExtension.ta_bank[bank].isp_base + OL_FLOOR) {
                if (kmiIntStatisticData.isp_current[bank] <
                    kmiDeviceExtension.ta_bank[bank].ol_limit - OL_GROWTH) {
                    kmiDeviceExtension.ta_bank[bank].ol_limit -= OL_GROWTH;
                    kmiDeviceExtension.ta_bank[bank].isp_limit =
                        kmiDeviceExtension.ta_bank[bank].ol_limit -
                        BACKGROUND_BYTES;
                }
                kmiIntStatisticData.ol_overflow_wait = 0;
            } else if (kmiIntStatisticData.ol_overflow_wait >
                       OL_OVERFLOW_LIMIT) {
                kmiFatalErrorEntry(KMI_FATAL_BANK_OVERFLOW);
            }
        }
        break;
    }
    return 0;
}

static KMINT32 isp_overflow(void)
{
    ERROR_PENDING = ISP_OVERFLOW;
    kmiIntStatisticData.isp_overflows++;
    kmiDeviceExtension.slot_state[kmiDeviceExtension.ta_slot] =
        KMI_FRAME_OVERFLOW;
    if (kmiDeviceExtension.opb_downward == 1) {
        kmiIntStatisticData.isp_overflow_wait++;
        grow_ta_area(ISP_AREA);
    }
    return 0;
}

static KMINT32 ol_overflow(void)
{
    ERROR_PENDING = OL_OVERFLOW;
    kmiIntStatisticData.ol_overflows++;
    if (kmiDeviceExtension.opb_downward == 1) {
        kmiIntStatisticData.ol_overflow_wait++;
        grow_ta_area(OL_AREA);
    }
    return 0;
}

static KMINT32 strip_overrun(void)
{
    ERROR_PENDING = STRIP_OVERRUN;
    if (!render_started) {
        kmiDeviceExtension.callback_pending |=
            CALLBACK_BIT(STRIP_OVERRUN_CALLBACK);
    }
    render_started = FALSE;
    return 0;
}

/* @hack spelling: the call to kmiFatalErrorEntry in a loop of one pass, where
 * if (error) would do */
#define HANDLE_EVENT(call)                                                     \
    for (;;) {                                                                 \
        error = (call);                                                        \
        if (!error) {                                                          \
            break;                                                             \
        }                                                                      \
        kmiFatalErrorEntry(error);                                             \
        break;                                                                 \
    }

#define FINISH_LIST(event, type)                                               \
    if (status & (event)) {                                                    \
        NORMAL_STATUS = (event);                                               \
        kmiDeviceExtension.lists_done |= LIST_BIT(type);                       \
        if (kmiDeviceExtension.lists_sent == kmiDeviceExtension.lists_done) {  \
            kmiFinalizeRender();                                               \
        }                                                                      \
    }

KMINT32 kmiInterruptHandlerSH(void)
{
    KMUINT32 status;
    KMINT32 slot;
    KMDWORD error;
    KMDWORD control;
    status = NORMAL_STATUS;
    if (status & VBLANK) {
        HANDLE_EVENT(handle_vblank());
    }
    if (status & HBLANK) {
        NORMAL_STATUS = HBLANK;
        kmiDeviceExtension.callback_pending |= CALLBACK_BIT(HSYNC_CALLBACK);
    }
    if (status & RENDER_END) {
        NORMAL_STATUS = RENDER_END_ALL;
        kmiDeviceExtension.render_busy = 0;
        kmiIntStatisticData.render_wait = 0;
        kmiIntStatisticData.render_time =
            kmiIntStatisticData.render_start - TCNT0;
        if (kmiDeviceExtension.texture_render != 0) {
            kmiDeviceExtension.texture_render = 0;
            kmiIntStatisticData.hold_wait = 0;
            RESTORE_WRITE_REGISTERS();
        } else {
            kmiDeviceExtension.slot_state[kmiDeviceExtension.render_slot] =
                KMI_FRAME_RENDERED;
        }
        kmiDeviceExtension.callback_pending |=
            CALLBACK_BIT(END_RENDER_CALLBACK);
        renders_stopped = 0;
    }
    /* @hack redundant: an empty if (0) */
    if (0) {
    }
    if (status & DMA_END) {
        control = CHCR2 & ~(CHCR_ENABLE | CHCR_TRANSFER_END);
        CHCR2 = control;
        NORMAL_STATUS = DMA_END;
        /* @hack redundant: calls that never run, which pass control and status
         * to kmiFatalErrorEntry, and an empty else */
        if (0) {
            kmiFatalErrorEntry(control);
            kmiFatalErrorEntry(status);
        } else {
        }
        kmiExecuteDMAChain();
    }
    if (status & YUV_END) {
        NORMAL_STATUS = YUV_END;
        kmiDeviceExtension.callback_pending |= CALLBACK_BIT(END_YUV_CALLBACK);
    }
    FINISH_LIST(OPAQUE_END, KM_OPAQUE_POLYGON);
    FINISH_LIST(OPAQUE_MODIFIER_END, KM_OPAQUE_MODIFIER);
    FINISH_LIST(TRANS_END, KM_TRANS_POLYGON);
    FINISH_LIST(TRANS_MODIFIER_END, KM_TRANS_MODIFIER);
    FINISH_LIST(PUNCHTHROUGH_END, KM_PUNCHTHROUGH_POLYGON);
    status = ERROR_PENDING;
    if (status & ISP_OVERFLOW) {
        error = isp_overflow();
        /* @hack spelling: the loop of one pass begun after the call, not
         * around it as in HANDLE_EVENT, where if (error) would do */
        for (;;) {
            if (!error) {
                break;
            }
            kmiFatalErrorEntry(error);
            break;
        }
    }
    if (status & OL_OVERFLOW) {
        HANDLE_EVENT(ol_overflow());
    }
    if (status & STRIP_OVERRUN) {
        HANDLE_EVENT(strip_overrun());
    }
    for (slot = 0; slot < KMI_CALLBACKS; slot++) {
        if (kmiDeviceExtension.callback_pending & CALLBACK_BIT(slot)) {
            kmiDeviceExtension.callback_pending &= ~CALLBACK_BIT(slot);
            kmiExCallBackFunc(kmiDeviceExtension.callback[slot].function,
                              kmiDeviceExtension.callback[slot].argument);
        }
        /* @hack redundant: a continue that ends the loop's body */
        /* NOLINTNEXTLINE(readability-redundant-control-flow) */
        continue;
    }
    return 0;
}

void kmiDefaultInterruptHandler(KMINT32 slot)
{
    kmiDeviceExtension.callback[slot].function(
        kmiDeviceExtension.callback[slot].argument);
}
