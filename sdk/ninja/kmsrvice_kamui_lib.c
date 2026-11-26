/* Kamui's kmf calls, which set the driver's options and report its metrics */
#include <kamui.h>
#include <kamui_buffer.h>
#include <kamui_internal.h>
#include <sg_xpt.h>

#define BUS_REVISION (*(volatile KMDWORD *)0xa05f689c)
/* KMI_FB_READ_SIZE's line count, from bit 10 */
#define READ_LINES 0x3ff
#define READ_LINES_SHIFT 10
#define VERTICAL_SCALE 0xffff /* KMI_SCALER_CONTROL's vertical factor */
/* the bytes of a DMA transfer unit. the common modifier goes by DMA, so its
 * buffer and size are whole units */
#define TRANSFER_BYTES 32
/* KMI_TA_YUV_CONTROL's fields: the last macroblock column in its low bits, the
 * last row from YUV_ROWS_SHIFT, and two flags */
#define YUV_CONTROL_422 0x01000000
#define YUV_MULTIPLE_TEXTURES 0x10000
#define YUV_DIMENSION_MASK 0x3f
#define YUV_ROWS_SHIFT 8
#define MACROBLOCK_SIZE(across, down)                                          \
    ((((down) - 1) & YUV_DIMENSION_MASK) << YUV_ROWS_SHIFT |                   \
     (((across) - 1) & YUV_DIMENSION_MASK))
/* KMI_VIDEO_START_Y's second field, from bit 16 */
#define SECOND_FIELD_SHIFT 16
/* kmfSetVirticalFilterMode's settings for mode 2 on an NTSC display of 480
 * lines without the flicker filter. NTSC_FILTER_SCALE is the scale factor.
 * each field starts NTSC_FILTER_START lines lower, and NTSC_FILTER_LINES are
 * read */
#define NTSC_FILTER_SCALE 0x471
#define NTSC_FILTER_START 12
#define NTSC_FILTER_LINES 214

/* names Kamui exports that no header declares */
/* NOLINTBEGIN(misc-use-internal-linkage) */
KMSTATUS kmfSetVertexBufferBaseAddress(KMDWORD base, KMDWORD size);
KMSTATUS kmfSetTimeOutCount(KMDWORD vertex_count, KMDWORD render_count);
KMSTATUS kmfInitializeYUVConverter(PKMSURFACEDESC surface, KMDWORD format,
                                   KMDWORD multiple, KMDWORD width_blocks,
                                   KMDWORD height_blocks);
KMSTATUS kmfSetCommonModifier(KMDWORD *buffer, KMDWORD size);
KMSTATUS kmfSetVirticalFilterMode(KMDWORD mode);
/* NOLINTEND(misc-use-internal-linkage) */

KMSTATUS kmfSetVertexBufferBaseAddress(KMDWORD base, KMDWORD size)
{
    kmiDeviceExtension.vertex_base = (void *)base;
    kmiDeviceExtension.vertex_size = size;
    return KMSTATUS_SUCCESS;
}

KMSTATUS kmfGetInternalSystemMetrics(PKMISYSTEMMETRICS pSystemMetrics)
{
    KMINT32 i;

    for (i = 0; i < 2; i++) {
        pSystemMetrics->NativeParams[1 - i].NativeITPBase =
            kmiDeviceExtension.ta_bank[i].isp_base;
        pSystemMetrics->NativeParams[1 - i].NativeOBJBase =
            kmiDeviceExtension.ta_bank[i].ol_base;
        pSystemMetrics->NativeParams[1 - i].NativeRegBase =
            kmiDeviceExtension.ta_bank[i].region_base;
        pSystemMetrics->NativeParams[1 - i].NativeITPLimit =
            kmiDeviceExtension.ta_bank[i].isp_limit;
        pSystemMetrics->NativeParams[1 - i].NativeOBJLimit =
            kmiDeviceExtension.ta_bank[i].ol_limit;
        pSystemMetrics->NativeParams[1 - i].NativeLastITPCurrent =
            kmiIntStatisticData.isp_current[i];
        pSystemMetrics->FBParams.FBLineStride[1 - i] =
            kmiDeviceExtension.field_offset[i];
    }
    if (kmiDeviceExtension.latency_model == KM_LATENCYMODEL_3V) {
        pSystemMetrics->FBParams.FBStartAddr[0] =
            kmiDeviceExtension.fb_start[kmiDeviceExtension.write_slot & 1];
        pSystemMetrics->FBParams.FBStartAddr[1] =
            kmiDeviceExtension
                .fb_start[1 - (kmiDeviceExtension.write_slot & 1)];
    } else {
        pSystemMetrics->FBParams.FBStartAddr[1] =
            kmiDeviceExtension.fb_start[kmiDeviceExtension.write_slot & 1];
        pSystemMetrics->FBParams.FBStartAddr[0] =
            kmiDeviceExtension
                .fb_start[1 - (kmiDeviceExtension.write_slot & 1)];
    }
    pSystemMetrics->FBParams.XClip = kmiDeviceExtension.fb_clip_x;
    pSystemMetrics->FBParams.YClip = kmiDeviceExtension.fb_clip_y;
    pSystemMetrics->RenderTimeOutCount = kmiIntStatisticData.render_timeouts;
    pSystemMetrics->VertexTimeOutCount = kmiIntStatisticData.ta_timeouts;
    pSystemMetrics->OBJOverflowCount = kmiIntStatisticData.ol_overflows;
    pSystemMetrics->ITPOverflowCount = kmiIntStatisticData.isp_overflows;
    for (i = 0; i < KMI_LISTS; i++) {
        /* @hack type: slots 0 and 1 of list i read through the flat word
         * array, the second from word 1's address, where
         * list_start.row[i].slot[n] would do */
        pSystemMetrics->DMADataAddress[0][i] =
            (KMDWORD)kmiDeviceExtension.list_start.word[i * KMI_FRAME_SLOTS];
        /* NOLINTBEGIN(readability-redundant-casting) */
        pSystemMetrics->DMADataAddress[1][i] =
            (KMDWORD)((KMDWORD *volatile *)&kmiDeviceExtension.list_start
                          .word[1])[i * KMI_FRAME_SLOTS];
        /* NOLINTEND(readability-redundant-casting) */
        pSystemMetrics->InternalVertexBufSize[i] =
            kmiDeviceExtension.list_limit[i];
        pSystemMetrics->MaxVertexBufSize[i] = kmiIntStatisticData.list_max[i];
    }
    pSystemMetrics->LastTATime = kmiIntStatisticData.ta_time;
    pSystemMetrics->LastRenderTime = kmiIntStatisticData.render_time;
    return KMSTATUS_SUCCESS;
}

KMSTATUS kmfSetTimeOutCount(KMDWORD vertex_count, KMDWORD render_count)
{
    KMDWORD revision;

    kmiDeviceExtension.vertex_timeout = vertex_count;
    kmiDeviceExtension.render_timeout = render_count;
    if (render_count == 0) {
        kmiDeviceExtension.capabilities |= KMI_RESET_BEFORE_RENDER;
    } else {
        /* @hack type: a cast of BUS_REVISION to KMDWORD, which it is already,
         * where the bare read would do */
        revision = (KMDWORD)BUS_REVISION;
        switch (revision) {
        case 4:
        case 5:
        case 6:
        case 8:
            kmiDeviceExtension.capabilities |= KMI_RESET_BEFORE_RENDER;
            break;
        case 0:
        case 1:
        case 2:
        case 9:
        default:
            kmiDeviceExtension.capabilities &= ~KMI_RESET_BEFORE_RENDER;
            break;
        }
    }
    return KMSTATUS_SUCCESS;
}

KMSTATUS kmfInitializeYUVConverter(PKMSURFACEDESC surface, KMDWORD format,
                                   KMDWORD multiple, KMDWORD width_blocks,
                                   KMDWORD height_blocks)
{
    KMDWORD layout = 0;
    KMDWORD control;

    if (format == KM_TEXTURE_YUV422) {
        layout = YUV_CONTROL_422;
    }
    if (multiple == 0) {
        control = MACROBLOCK_SIZE(width_blocks, height_blocks) | layout;
    } else {
        control = MACROBLOCK_SIZE(width_blocks, height_blocks) | layout |
                  YUV_MULTIPLE_TEXTURES;
    }
    kmiWriteRegister(KMI_TA_YUV_CONTROL, control);
    kmiWriteRegister(KMI_TA_YUV_BASE, (KMDWORD)surface->pSurface);
    return KMSTATUS_SUCCESS;
}

KMSTATUS kmfSetCommonModifier(KMDWORD *buffer, KMDWORD size)
{
    if (size == 0) {
        kmiDeviceExtension.common_modifier = FALSE;
        return KMSTATUS_SUCCESS;
    } else {
        /* @hack spelling: the enable in an else after a return, where a
         * statement after the if would do */
        kmiDeviceExtension.common_modifier = TRUE;
    }
    if (((KMDWORD)buffer & (TRANSFER_BYTES - 1)) ||
        (size & (TRANSFER_BYTES - 1))) {
        kmiDeviceExtension.common_modifier = FALSE;
        return KMSTATUS_INVALID_ADDRESS;
    }
    kmiDeviceExtension.common_modifier_data = (KMUINT32 *)buffer;
    kmiDeviceExtension.common_modifier_size = size;
    return KMSTATUS_SUCCESS;
}

KMSTATUS kmfSetVirticalFilterMode(KMDWORD mode)
{
    KMUINT32 scaler;
    KMDWORD size;
    KMUINT32 lines;
    KMDWORD first_field;
    KMDWORD second_field;
    KMDWORD factor;
    KMDWORD control;
    KMDWORD start;

    first_field = kmiDeviceExtension.origin_y1;
    second_field = kmiDeviceExtension.origin_y2;
    lines = kmiDeviceExtension.fb_lines;
    size = kmiDeviceExtension.fb_read_size & ~(READ_LINES << READ_LINES_SHIFT);
    scaler = kmiDeviceExtension.scaler_control;
    factor = scaler & VERTICAL_SCALE;
    switch (mode) {
    case 0:
        /* @hack spelling: factor = factor & 0xff00, where &= would do */
        factor = factor & 0xff00;
        break;
    case 1:
        break;
    /* @hack redundant: an empty default, before case 2 */
    default:
        break;
    case 2:
        if ((kmiDeviceExtension.display_mode & KMI_VIDEO_MASK) == KM_NTSC) {
            if (kmiDeviceExtension.display_mode & KMI_HEIGHT_480) {
                if (KMI_FLICKER_FREE) {
                    break;
                }
                factor = NTSC_FILTER_SCALE;
                /* @hack spelling: first_field = first_field +
                 * NTSC_FILTER_START, and second_field likewise, where += would
                 * do */
                first_field = first_field + NTSC_FILTER_START;
                lines = NTSC_FILTER_LINES;
                second_field = second_field + NTSC_FILTER_START;
            }
        }
        break;
    }
    control = (scaler & ~VERTICAL_SCALE) | factor;
    start =
        (first_field + kmiDeviceExtension.offset_y) |
        ((second_field + kmiDeviceExtension.offset_y) << SECOND_FIELD_SHIFT);
    size |= (lines & READ_LINES) << READ_LINES_SHIFT;
    kmiWaitVlank();
    kmiWriteRegister(KMI_FB_READ_CONTROL,
                     kmiDeviceExtension.fb_read_control & ~KMI_READ_ENABLE);
    kmiWriteRegister(KMI_VIDEO_CONTROL,
                     kmiDeviceExtension.video_control | KMI_VIDEO_BLANK);
    kmiWriteRegister(KMI_VIDEO_START_Y, start);
    kmiWriteRegister(KMI_FB_READ_SIZE, size);
    kmiWriteRegister(KMI_SCALER_CONTROL, control);
    kmiDeviceExtension.display_changed = TRUE;
    return KMSTATUS_SUCCESS;
}

KMSTATUS kmfSetNoWaitVSyncAtDisplayModeChange(KMBOOLEAN flag)
{
    kmiDeviceExtension.no_wait_display_change = flag;
    return KMSTATUS_SUCCESS;
}
