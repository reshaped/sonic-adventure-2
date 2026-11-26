/* Kamui's frame buffer surfaces and the display's size.
 * kmCreateFrameBufferSurface fills kmiSystemConfig's two requests in turn.
 * before the system configuration is ready, it only records the request */
#include <kamui.h>
#include <kamui_buffer.h>
#include <kamui_internal.h>
#include <sg_xpt.h>

/* kmiGetSysConfigFlag's value once the system configuration is set up, and
 * the flag of a frame buffer requested before */
#define CONFIG_READY 0x80000000UL
#define CONFIG_FRAME 0x200
/* kmiGetDisplayData's color for the display's own pixel format */
#define CURRENT_COLOR 255
#define CLEAR_WORDS 640
/* fb_read_control's strip buffer fields. its height goes in STRIP_HEIGHT_FIELD
 * from STRIP_SHIFT, in units of STRIP_LINES lines. its enable is the bit at
 * STRIP_ENABLE_SHIFT */
#define STRIP_FIELDS 0x005f0000
#define STRIP_HEIGHT_FIELD 31
#define STRIP_SHIFT 16
#define STRIP_ENABLE_SHIFT 22
#define STRIP_LINES 32
/* a frame buffer surface's flag: a strip buffer */
#define STRIP_SURFACE 1

/* the kmiSystemConfig request kmCreateFrameBufferSurface fills next, 0 or 1 */
static KMDWORD next_request = 0;

KMSTATUS kmCreateFrameBufferSurface(PKMSURFACEDESC surface, KMINT32 width,
                                    KMINT32 height, KMBOOLEAN strip_buffer,
                                    KMBOOLEAN clear)
{
    KMDWORD row[CLEAR_WORDS];
    /* @hack type: height copied into a volatile local, where height would
     * do */
    volatile KMDWORD requested_height = height;
    KMDWORD depth_code[6] = {1, 1, 1, 1, 2, 3};
    KMSTATUS status;
    KMDWORD i;

    kmiSystemConfig.frame[next_request].surface = surface;
    kmiSystemConfig.frame[next_request].width = width;
    kmiSystemConfig.frame[next_request].height = requested_height;
    kmiSystemConfig.strip_buffer = strip_buffer;
    kmiSystemConfig.frame[next_request].clear = clear;
    next_request = (next_request + 1) & 1;
    kmiDeviceExtension.strip_buffer = strip_buffer;
    if (kmiSystemConfig.mode != KMI_NO_BUFFER) {
        kmiGetDisplayData(CURRENT_COLOR, strip_buffer, requested_height);
    }
    if (kmiGetSysConfigFlag() == CONFIG_READY) {
        status = kmiFBalloc(kmiSystemConfig.frame_buffer_bytes, surface);
        if (status != KMSTATUS_SUCCESS) {
            return status;
        }
        surface->SurfaceType = KM_SURFACETYPE_FRAMEBUFFER;
        surface->BitDepth = depth_code[kmiDeviceExtension.bpp];
        surface->PixelFormat = kmiDeviceExtension.bpp;
        surface->u0.Width = kmiSystemConfig.width;
        surface->u1.Height = kmiSystemConfig.height;
        if (strip_buffer == TRUE) {
            surface->fSurfaceFlags = STRIP_SURFACE;
        } else {
            surface->fSurfaceFlags = 0;
        }
        if (clear == TRUE) {
            for (i = 0; i < CLEAR_WORDS; i++) {
                row[i] = 0;
            }
            for (i = 0; i < surface->uSize.dwFrameBufferSize;
                 i += sizeof(row)) {
                /* @hack redundant: the call in a loop of one pass */
                do {
                    /* @hack type: &row, a pointer to the array, where row
                     * would do */
                    kmiCopyToPVRMemory((char *)surface->pSurface + i, &row,
                                       sizeof(row));
                } while (0);
            }
        }
    } else {
        kmiAddSysConfigFlag(CONFIG_FRAME);
    }
    return KMSTATUS_SUCCESS;
}

void kmiGetDisplayData(KMDWORD color, KMDWORD strip_buffer, KMDWORD height)
{
    KMDWORD bits_per_pixel[] = {16, 16, 16, 16, 24, 32};
    KMDWORD width;
    if (kmiDeviceExtension.video_mode == KM_VGA ||
        (kmiDeviceExtension.display_mode & KMI_WIDTH_640)) {
        width = 640;
    } else {
        width = 320;
    }
    if (strip_buffer == TRUE) {
        height = (height + (STRIP_LINES - 1)) & ~(STRIP_LINES - 1UL);
        kmiDeviceExtension.fb_read_control =
            (kmiDeviceExtension.fb_read_control & ~STRIP_FIELDS) |
            (strip_buffer << STRIP_ENABLE_SHIFT) |
            (((height / STRIP_LINES) & STRIP_HEIGHT_FIELD) << STRIP_SHIFT);
    } else if (kmiDeviceExtension.video_mode == KM_VGA) {
        height = 480;
    } else if (KMI_FLICKER_FREE) {
        height = 240;
    } else if (kmiDeviceExtension.display_mode & KMI_HEIGHT_480) {
        if (KMI_PAL_EXTENDED_MODE) {
            height =
                kmiDeviceExtension.pal_lines[kmiDeviceExtension.pal_selection];
        } else {
            height = 480;
        }
    } else if ((kmiDeviceExtension.display_mode &
                (KMI_PAL_EXTENDED | KMI_HEIGHT_480)) == KMI_PAL_EXTENDED) {
        height =
            kmiDeviceExtension.pal_lines[kmiDeviceExtension.pal_selection] / 2;
    } else {
        height = 240;
    }
    kmiSystemConfig.width = width;
    kmiSystemConfig.height = height;
    if (color != CURRENT_COLOR) {
        kmiSystemConfig.bytes_per_pixel = bits_per_pixel[color] >> 3;
    } else {
        kmiSystemConfig.bytes_per_pixel =
            bits_per_pixel[kmiDeviceExtension.bpp] >> 3;
    }
    kmiSystemConfig.frame_buffer_bytes = kmiSystemConfig.width *
                                         kmiSystemConfig.height *
                                         kmiSystemConfig.bytes_per_pixel;
}
