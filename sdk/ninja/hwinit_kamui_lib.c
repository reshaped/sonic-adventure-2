/* Kamui's setup of the video chip: its reset, its registers' first values and
 * its fog table */
#include <kamui.h>
#include <kamui_internal.h>
#include <sg_xpt.h>

/* the video chip's registers, as offsets into its register block */
#define CHIP_ID 0x000
#define CHIP_REVISION 0x004
#define SPAN_SORT 0x030
#define PARAM_CONFIG 0x07c
#define HALF_OFFSET 0x080
#define PERP_VALUE 0x084
#define FEED_CONFIG 0x098
#define SDRAM_REFRESH 0x0a0
#define SDRAM_CONFIG 0x0a8
#define FB_BURST_CONTROL 0x110

/* KMI_SOFT_RESET's bits */
#define RESET_TA 1
#define RESET_RENDERER 2

/* a chip is core 6 when CHIP_ID reads CORE6_CHIP_ID and CHIP_REVISION reads
 * at least CORE6_REVISION */
#define CORE6_CHIP_ID 0x17fd11db
#define CORE6_REVISION 0x11

/* the system bus's revision register */
#define BUS_REVISION (*(KMDWORD *)0xa05f689c)

/* video memory's 32-bit view, through the uncached area */
#define VIEW_32BIT 0xa5000000UL
/* video memory's size in longs: 8 MB */
#define VIDEO_MEMORY_LONGS 0x200000

/* 1.0, as a float's bits */
#define FLOAT_ONE 0x3f800000

/* the frame buffer's first width and height, in pixels */
#define FB_WIDTH 640
#define FB_HEIGHT 480
/* KMI_FB_CLIP_X and KMI_FB_CLIP_Y hold a clip's start in their low half, and
 * its end from bit CLIP_END_SHIFT up */
#define CLIP_END_SHIFT 16
/* the pixels in one unit of KMI_FB_WRITE_STRIDE */
#define STRIDE_PIXELS 4

/* the background plane's first depth, a 1/w just under 0.00001, as a float's
 * bits */
#define BACKGROUND_INV_W 0x3727c5a0

/* both fog colors' first value, a gray of half intensity */
#define FOG_GRAY 0x007f7f7f
/* the fog density word whose value is 255. its high byte is a mantissa of 0xff
 * 128ths, and its low byte a power of 7 */
#define FOG_DENSITY_255 0xff07

/* The SDK exports this empty renderer hook. */
/* NOLINTNEXTLINE(misc-use-internal-linkage) */
void kmiCheckRenderer(void);

void kmiResetBoard(void)
{
    kmiWriteRegister(KMI_SOFT_RESET, RESET_TA | RESET_RENDERER);
    kmiWriteRegister(CHIP_ID, 0);
    kmiWriteRegister(KMI_SOFT_RESET, 0);
}

void kmiCheckRenderer(void)
{
}

KMINT32 kmiInitializeDevice(void)
{
    KMDWORD chip_id;
    KMDWORD chip_revision;
    KMDWORD bus_revision;
    KMDWORD sdram_config;
    KMDWORD span_sort;
    KMDWORD zero;
    int i;

    sdram_config = 0x0dd18957;
    kmiDeviceExtension.feed_config = 0;
    chip_id = kmiReadRegister(CHIP_ID);
    chip_revision = kmiReadRegister(CHIP_REVISION);
    kmiDeviceExtension.core = 5;
    /* @hack redundant: the arms for another CHIP_ID and for revision 1, which
     * store 5 in core again */
    if (chip_id != CORE6_CHIP_ID) {
        kmiDeviceExtension.core = 5;
    } else if (chip_revision == 1) {
        kmiDeviceExtension.core = 5;
    } else if (chip_revision >= CORE6_REVISION) {
        kmiDeviceExtension.core = 6;
        kmiDeviceExtension.feed_config = 0x00800408;
        sdram_config = 0x15d1c951;
    }
    /* @hack spelling: the switch on bus_revision, a copy of BUS_REVISION,
     * where a switch on BUS_REVISION would do */
    bus_revision = BUS_REVISION;
    switch (bus_revision) {
    case 0:
        kmiDeviceExtension.capabilities = 0;
        span_sort = 0;
        break;
    case 1:
        kmiDeviceExtension.capabilities = 1;
        span_sort = 0;
        break;
    case 2:
        kmiDeviceExtension.capabilities = 3;
        span_sort = 0x101;
        break;
    case 4:
    case 5:
    case 6:
        kmiDeviceExtension.capabilities = 0x7f;
        span_sort = 0x10101;
        break;
    case 8:
        kmiDeviceExtension.capabilities = 0x1f;
        span_sort = 0x101;
        break;
    case 9:
        kmiDeviceExtension.capabilities = 0xf;
        span_sort = 0x101;
        break;
    default:
        kmiDeviceExtension.capabilities = 0x8f;
        span_sort = 0x101;
        break;
    }
    kmiWriteRegister(SDRAM_CONFIG, sdram_config);
    kmiWriteRegister(SDRAM_REFRESH, 0x20);
    kmiWriteRegister(KMI_SOFT_RESET, 0);
    kmiWriteRegister(KMI_TEXTURE_CONTROL, 3);
    kmiWriteRegister(KMI_FB_WRITE_CONTROL, 9);
    if (kmiDeviceExtension.video_mode & KM_VGA) {
        kmiVerifyWriteRegister(KMI_FB_READ_CONTROL, 0x00800000);
    } else {
        kmiVerifyWriteRegister(KMI_FB_READ_CONTROL, 0);
    }
    kmiWriteRegister(KMI_FB_READ_SIZE, 0);
    kmiWriteRegister(KMI_FB_CLIP_X, FB_WIDTH << CLIP_END_SHIFT);
    kmiWriteRegister(KMI_FB_CLIP_Y, FB_HEIGHT << CLIP_END_SHIFT);
    if (kmiDeviceExtension.core == 6) {
        kmiWriteRegister(FB_BURST_CONTROL, 0x00093f39);
        kmiWriteRegister(FEED_CONFIG, kmiDeviceExtension.feed_config);
    } else {
        kmiWriteRegister(FB_BURST_CONTROL, 0x00092039);
    }
    kmiWriteRegister(KMI_FB_WRITE_STRIDE, FB_WIDTH / STRIDE_PIXELS);
    kmiWriteRegister(KMI_SHADOW_SCALE, 0);
    kmiWriteRegister(KMI_CULL_VALUE, FLOAT_ONE);
    kmiWriteRegister(PERP_VALUE, 0);
    kmiWriteRegister(SPAN_SORT, span_sort);
    kmiWriteRegister(KMI_FOG_TABLE_COLOR, FOG_GRAY);
    kmiWriteRegister(KMI_FOG_VERTEX_COLOR, FOG_GRAY);
    kmiWriteRegister(KMI_FOG_CLAMP_MIN, 0);
    kmiWriteRegister(KMI_FOG_CLAMP_MAX, 0xffffffff);
    kmiWriteRegister(HALF_OFFSET, 7);
    kmiWriteRegister(KMI_BACKGROUND_DEPTH, BACKGROUND_INV_W);
    kmiWriteRegister(KMI_BACKGROUND_TAG, 0x01203000);
    kmiWriteRegister(KMI_SHADOW_SCALE, 1);
    if (kmiDeviceExtension.core != 6) {
        while (1) {
        }
    }
    /* @hack redundant: the test of core and its else, which the loop above
     * leaves unreached */
    if (kmiDeviceExtension.core == 6) {
        kmiWriteRegister(PARAM_CONFIG, 0x0027df77);
    } else {
        kmiWriteRegister(PARAM_CONFIG, 0x0007df77);
    }
    zero = 0;
    for (i = 0; i < VIDEO_MEMORY_LONGS; i++) {
        kmiMemCopy4((void *)(VIEW_32BIT + (i * sizeof(KMDWORD))), &zero,
                    sizeof(zero));
    }
    return TRUE;
}

void kmiInitializeFog(void)
{
    int i;
    KMDWORD entry;

    kmiWriteRegister(KMI_FOG_DENSITY, FOG_DENSITY_255);
    kmiWriteRegister(KMI_FOG_VERTEX_COLOR, FOG_GRAY);
    kmiWriteRegister(KMI_FOG_TABLE_COLOR, FOG_GRAY);
    for (i = 0; i < KM_FOG_ENTRY; i++) {
        /* @hack spelling: each entry in a local of its own, where the call's
         * argument would do */
        entry = (((0xff - i) & 0xff) << 8 | ((0xfe - i) & 0xff)) & 0xffff;
        kmiWriteRegister(KMI_FOG_TABLE + (i * sizeof(KMDWORD)), entry);
    }
}

void kmiUnloadDevice(void)
{
}
