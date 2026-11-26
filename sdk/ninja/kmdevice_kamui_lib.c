/* Kamui's display setup: the device, the display mode and its filters,
 * blanking, and the picture's position */
#include <kamui.h>
#include <kamui_internal.h>
#include <sg_xpt.h>

/* the video chip's registers, as offsets into its register block */
#define Y_COEFFICIENTS 0x118
/* KMI_FB_READ_SIZE's line count, from bit 10 */
#define READ_LINES 0x3ff
#define READ_LINES_SHIFT 10
/* the last of pal_lines' entries a PAL extension callback may choose. a
 * choice past it, or below 0, gives way to DEFAULT_PAL_SELECTION */
#define LAST_PAL_SELECTION 4
#define DEFAULT_PAL_SELECTION 1
/* KMI_VIDEO_START_Y's second field, from bit 16 */
#define SECOND_FIELD_SHIFT 16
/* kmAdjustDisplayCenter's status for an adjustment out of range, as one that
 * leaves KMI_VIDEO_START_X at LEAST_START_X or less */
#define ADJUST_OUT_OF_RANGE 18
#define LEAST_START_X 10

/* @hack spelling: TRUE or FALSE stored by an if and its else, where
 * result = (value) != 0 would do */
#define STORE_BOOLEAN(result, value)                                           \
    do {                                                                       \
        if (value) {                                                           \
            (result) = TRUE;                                                   \
        } else {                                                               \
            (result) = FALSE;                                                  \
        }                                                                      \
    } while (0)

/* names Kamui exports that no header declares */
/* NOLINTBEGIN(misc-use-internal-linkage) */
KMSTATUS kmBlankScreen(KMBOOLEAN blank);
KMSTATUS kmChangeDisplayFilterMode(KMBOOLEAN dither, KMBOOLEAN anti_alias);
/* NOLINTEND(misc-use-internal-linkage) */

/* write the display registers from Kamui's copies, with frame buffer reads off
 * and the picture blanked */
static void replay_display_registers(void)
{
    kmiWriteRegister(KMI_FB_CLIP_X, kmiDeviceExtension.fb_clip_x);
    kmiWriteRegister(KMI_FB_CLIP_Y, kmiDeviceExtension.fb_clip_y);
    kmiWriteRegister(KMI_FB_READ_SIZE, kmiDeviceExtension.fb_read_size);
    kmiWriteRegister(KMI_FB_WRITE_STRIDE, kmiDeviceExtension.fb_write_stride);
    kmiWriteRegister(Y_COEFFICIENTS, kmiDeviceExtension.y_coefficients);
    kmiWriteRegister(KMI_SCALER_CONTROL, kmiDeviceExtension.scaler_control);
    kmiWriteRegister(KMI_FB_WRITE_CONTROL, kmiDeviceExtension.fb_write_control);
    kmiWriteRegister(KMI_FB_READ_CONTROL,
                     kmiDeviceExtension.fb_read_control & ~KMI_READ_ENABLE);
    kmiWriteRegister(KMI_VIDEO_CONTROL,
                     kmiDeviceExtension.video_control | KMI_VIDEO_BLANK);
}

void kmiWaitVlank(void)
{
    KMDWORD count = kmiDeviceExtension.vsync_count;
    while (count == kmiDeviceExtension.vsync_count) {
    }
}

KMSTATUS kmInitDevice(KMVIDEOMODE nVideoMode)
{
    kmiInitializeDriver();
    if (nVideoMode > KM_PAL || (KMINT32)nVideoMode < KM_NTSC) {
        return KMSTATUS_INVALID_VIDEO_MODE;
    }
    kmiDeviceExtension.video_mode =
        kmiGetProfileInt("KAMUI", "VideoMode", nVideoMode & KMI_VIDEO_MASK);
    kmiInitializeHardware();
    return KMSTATUS_SUCCESS;
}

KMSTATUS kmSetDisplayMode(KMINT32 nDisplayMode, KMBPPMODE nBpp,
                          KMBOOLEAN fDither, KMBOOLEAN fAntiAlias)
{
    KMSTATUS status = KMSTATUS_SUCCESS;
    KMBOOLEAN anti_alias;
    KMBOOLEAN dither;
    KMBOOLEAN flicker_filter;
    KMDWORD width;
    KMDWORD height;
    KMPALEXTINFO choice;

    kmiDiscardRenderPipeline();
    nDisplayMode = kmiGetProfileInt("KAMUI", "DisplayMode", nDisplayMode);
    fAntiAlias = kmiGetProfileInt("KAMUI", "AntiAlias", fAntiAlias);
    fDither = kmiGetProfileInt("KAMUI", "Dither", fDither);
    kmiDeviceExtension.video_mode = nDisplayMode & KMI_VIDEO_MASK;
    if ((nDisplayMode & KMI_PAL_EXTENDED) == KMI_PAL_EXTENDED) {
        choice.nDisplayMode = nDisplayMode;
        kmiDeviceExtension.callback[PAL_EXTENSION_CALLBACK].function(&choice);
        kmiDeviceExtension.pal_selection = choice.nPALExtMode;
        if (kmiDeviceExtension.pal_selection < 0 ||
            kmiDeviceExtension.pal_selection > LAST_PAL_SELECTION) {
            kmiDeviceExtension.pal_selection = DEFAULT_PAL_SELECTION;
        }
    }
    switch (nBpp) {
    case KM_DSPBPP_ARGB8888:
    case KM_DSPBPP_RGB888:
    case KM_DSPBPP_ARGB1555:
    case KM_DSPBPP_RGB555:
    case KM_DSPBPP_RGB565:
        break;
    default:
        status = KMSTATUS_INVALID_DISPLAY_MODE;
        break;
    }
    STORE_BOOLEAN(anti_alias, fAntiAlias);
    STORE_BOOLEAN(dither, fDither);
    if (status == KMSTATUS_SUCCESS) {
        kmiDeviceExtension.display_mode = nDisplayMode;
        kmiDeviceExtension.bpp = nBpp;
        kmiDeviceExtension.dither = dither;
        kmiDeviceExtension.anti_alias = anti_alias;
        if (kmiDeviceExtension.display_mode == KM_VGA) {
            width = 640;
            height = 480;
            flicker_filter = FALSE;
            kmiDeviceExtension.picture_lines = height;
        } else {
            STORE_BOOLEAN(flicker_filter, KMI_TWO_FIELDS);
            if (kmiDeviceExtension.display_mode & KMI_WIDTH_640) {
                width = 640;
            } else {
                width = 320;
            }
            if (kmiDeviceExtension.display_mode & KMI_HEIGHT_480) {
                if (KMI_PAL_EXTENDED_MODE) {
                    height = kmiDeviceExtension
                                 .pal_lines[kmiDeviceExtension.pal_selection];
                } else {
                    height = 480;
                }
                kmiDeviceExtension.picture_lines = (KMINT32)height / 2;
            } else {
                if (KMI_PAL_EXTENDED_MODE) {
                    height = kmiDeviceExtension
                                 .pal_lines[kmiDeviceExtension.pal_selection] /
                             2;
                } else {
                    height = 240;
                }
                kmiDeviceExtension.picture_lines = height;
            }
        }
        /* @hack redundant: each case's store of KMSTATUS_SUCCESS, which
         * status already holds */
        switch (kmiDeviceExtension.video_mode) {
        case KM_VGA:
            status = KMSTATUS_SUCCESS;
            kmiInitVGA();
            break;
        case KM_NTSC:
            status = KMSTATUS_SUCCESS;
            kmiInitNTSC();
            break;
        case KM_PAL:
            status = KMSTATUS_SUCCESS;
            kmiInitPAL();
            break;
        default:
            status = KMSTATUS_INVALID_DISPLAY_MODE;
            break;
        }
        kmiFBControl(width, height, nBpp, flicker_filter, dither, anti_alias);
        replay_display_registers();
    }
    return status;
}

KMSTATUS kmBlankScreen(KMBOOLEAN blank)
{
    if (blank == TRUE) {
        kmiWriteRegister(KMI_FB_READ_CONTROL,
                         kmiDeviceExtension.fb_read_control & ~KMI_READ_ENABLE);
        kmiWriteRegister(KMI_VIDEO_CONTROL,
                         kmiDeviceExtension.video_control | KMI_VIDEO_BLANK);
    } else {
        kmiWriteRegister(KMI_FB_READ_CONTROL,
                         kmiDeviceExtension.fb_read_control & ~KMI_READ_ENABLE);
        kmiWriteRegister(KMI_VIDEO_CONTROL, kmiDeviceExtension.video_control);
    }
    kmiWriteRegister(KMI_FB_READ_CONTROL, kmiDeviceExtension.fb_read_control);
    return KMSTATUS_SUCCESS;
}

KMSTATUS kmChangeDisplayFilterMode(KMBOOLEAN dither, KMBOOLEAN anti_alias)
{
    /* @hack redundant: a table of bits per pixel for each KMBPPMODE, which
     * nothing reads */
    KMDWORD pixel_bits[6] = {16, 16, 16, 16, 24, 32};
    KMBOOLEAN use_anti_alias;
    KMBOOLEAN use_dither;
    KMBOOLEAN flicker_filter;
    KMINT32 width;
    KMINT32 height;

    kmiDiscardRenderPipeline();
    STORE_BOOLEAN(use_anti_alias, anti_alias);
    STORE_BOOLEAN(use_dither, dither);
    kmiDeviceExtension.dither = use_dither;
    kmiDeviceExtension.anti_alias = use_anti_alias;
    if (kmiDeviceExtension.display_mode == KM_VGA) {
        width = 640;
        height = 480;
        flicker_filter = FALSE;
    } else {
        STORE_BOOLEAN(flicker_filter, KMI_TWO_FIELDS);
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
    kmiWaitVlank();
    kmiFBControl(width, height, kmiDeviceExtension.bpp, flicker_filter,
                 use_dither, use_anti_alias);
    replay_display_registers();
    kmiWriteRegister(KMI_FB_READ_CONTROL,
                     kmiDeviceExtension.fb_read_control & ~KMI_READ_ENABLE);
    kmiWriteRegister(KMI_VIDEO_CONTROL, kmiDeviceExtension.video_control);
    kmiWriteRegister(KMI_FB_READ_CONTROL, kmiDeviceExtension.fb_read_control);
    kmiResetBoard();
    kmiSetTAEnvironment(
        kmiDeviceExtension.ta_layout[0].region_array >> KMI_TA_ADDRESS_SHIFT,
        kmiDeviceExtension.ta_layout[0].object_list >> KMI_TA_ADDRESS_SHIFT,
        kmiDeviceExtension.ta_layout[0].isp_params >> KMI_TA_ADDRESS_SHIFT,
        kmiDeviceExtension.ta_layout[0].tsp_params >> KMI_TA_ADDRESS_SHIFT,
        kmiDeviceExtension.ta_options, 0);
    kmiDeviceExtension.display_changed = TRUE;
    kmiWaitVlank();
    return KMSTATUS_SUCCESS;
}

KMSTATUS kmStopDisplayFrameBuffer(void)
{
    kmiWriteRegister(KMI_FB_READ_CONTROL,
                     kmiDeviceExtension.fb_read_control & ~KMI_READ_ENABLE);
    kmiWriteRegister(KMI_VIDEO_CONTROL,
                     kmiDeviceExtension.video_control | KMI_VIDEO_BLANK);
    kmiDiscardRenderPipeline();
    kmiResetBoard();
    kmiWriteRegister(KMI_FB_READ_CONTROL,
                     kmiDeviceExtension.fb_read_control & ~KMI_READ_ENABLE);
    kmiWriteRegister(KMI_VIDEO_CONTROL,
                     kmiDeviceExtension.video_control | KMI_VIDEO_BLANK);
    kmiWaitVlank();
    kmiDeviceExtension.display_changed = TRUE;
    return KMSTATUS_SUCCESS;
}

KMSTATUS kmAdjustDisplayCenter(KMINT32 nXAdjust, KMINT32 nYAdjust)
{
    KMSTATUS status = KMSTATUS_SUCCESS;
    KMDWORD size;
    KMDWORD lines;

    /* @hack type: nXAdjust converted to long in the first test, where
     * KMINT32 would do */
    if (kmiDeviceExtension.origin_x + (long)nXAdjust > LEAST_START_X) {
        if (kmiDeviceExtension.offset_x != nXAdjust) {
            kmiDeviceExtension.offset_x = nXAdjust;
            kmiDeviceExtension.video_start_x =
                kmiDeviceExtension.origin_x + kmiDeviceExtension.offset_x;
            kmiWriteRegister(KMI_VIDEO_START_X,
                             kmiDeviceExtension.video_start_x);
        }
    } else {
        status = ADJUST_OUT_OF_RANGE;
    }
    if (kmiDeviceExtension.origin_y1 + nYAdjust > 0 ||
        kmiDeviceExtension.origin_y2 + nYAdjust > 0) {
        if (kmiDeviceExtension.offset_y != nYAdjust) {
            kmiDeviceExtension.video_start_y =
                (kmiDeviceExtension.origin_y1 + nYAdjust) |
                ((kmiDeviceExtension.origin_y2 + nYAdjust)
                 << SECOND_FIELD_SHIFT);
            kmiWriteRegister(KMI_VIDEO_START_Y,
                             kmiDeviceExtension.video_start_y);
            size = kmiDeviceExtension.fb_read_size &
                   ~(READ_LINES << READ_LINES_SHIFT);
            if (nYAdjust > 0) {
                lines = kmiDeviceExtension.fb_lines;
                if (kmiDeviceExtension.skip_lines < nYAdjust) {
                    lines -= nYAdjust;
                } else {
                    lines -= kmiDeviceExtension.skip_lines;
                }
                if (nYAdjust + lines > kmiDeviceExtension.fb_lines) {
                    lines = kmiDeviceExtension.fb_lines -
                            kmiDeviceExtension.skip_lines;
                }
                if (kmiDeviceExtension.fb_lines < lines) {
                    kmiDeviceExtension.fb_lines = lines;
                }
            } else {
                if (KMI_VGA_MODE) {
                    lines = 478;
                } else if (KMI_PAL_EXTENDED_MODE) {
                    lines = (kmiDeviceExtension
                                 .pal_lines[kmiDeviceExtension.pal_selection] /
                             2) -
                            2;
                } else {
                    lines = 238;
                }
            }
            kmiDeviceExtension.offset_y = nYAdjust;
            kmiDeviceExtension.fb_read_size =
                ((lines & READ_LINES) << READ_LINES_SHIFT) | size;
            kmiWriteRegister(KMI_FB_READ_CONTROL,
                             kmiDeviceExtension.fb_read_control &
                                 ~KMI_READ_ENABLE);
            kmiWriteRegister(KMI_FB_READ_SIZE, kmiDeviceExtension.fb_read_size);
            kmiWriteRegister(KMI_FB_READ_CONTROL,
                             kmiDeviceExtension.fb_read_control);
        }
    } else {
        status = ADJUST_OUT_OF_RANGE;
    }
    return status;
}
