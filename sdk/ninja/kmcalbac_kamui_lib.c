/* Kamui's interrupt callbacks and their defaults, the horizontal blank
 * interrupt's line, and the scan line on display */
#include <kamui.h>
#include <kamui_internal.h>
#include <machine.h>
#include <sg_xpt.h>

/* a scan line, in KMI_SYNC_STATUS's low bits and in KMI_HBLANK_INTERRUPT's */
#define SCANLINE_MASK 0x03ff
/* KMI_HBLANK_INTERRUPT's other field, the start of the horizontal blank */
#define HBLANK_START 0x03ff0000
/* the bit of KMI_SYNC_STATUS that kmGetCurrentScanline takes for the second
 * field */
#define FIELD_TWO 0x40000
/* sync_load's lines a field, from bit 16, or from bit 17 for two fields */
#define LOAD_LINES_SHIFT 16
/* the PAL extension callback's default choice of pal_lines */
#define DEFAULT_PAL_SELECTION 1
/* the border's colors for kmiFatalErrorEntry's codes */
#define BLUE 0xff0000ff
#define RED 0xffff0000
#define GREEN 0xff00ff00
#define MAGENTA 0xffff00ff

/* names Kamui exports that no header declares */
/* NOLINTBEGIN(misc-use-internal-linkage) */
void kmiRegisterCallback(volatile struct kmi_device_extension *device,
                         KMINT32 slot, PKMCALLBACKFUNC callback,
                         void *argument);
KMSTATUS kmSetHSyncLine(KMINT32 line);
KMSTATUS kmGetCurrentScanline(KMINT32 *line);
KMSTATUS kmSetTexOverflowCallback(PKMCALLBACKFUNC callback, void *argument);
KMSTATUS kmSetStripOverRunCallback(PKMCALLBACKFUNC callback, void *argument);
KMSTATUS kmSetEndOfYUVCallback(PKMCALLBACKFUNC callback, void *argument);
/* NOLINTEND(misc-use-internal-linkage) */

static void default_end_of_render(void *argument);
static void default_vsync(void *argument);
static void default_hsync(void *argument);
static void default_texture_overflow(void *argument);
static void default_strip_overrun(void *argument);
static void default_end_of_vertex(void *argument);
static void default_end_of_yuv(void *argument);
static void default_wait_vsync(void *argument);
static void default_fatal_error(void *code);
static void default_pal_extension(void *argument);

void kmiInitCallback(void)
{
    kmiDeviceExtension.unk_2b0 = 0;
#define INIT_CALLBACK(slot, handler)                                           \
    kmiDeviceExtension.callback[slot].function = handler;                      \
    kmiDeviceExtension.callback[slot].argument = NULL
    INIT_CALLBACK(END_RENDER_CALLBACK, default_end_of_render);
    INIT_CALLBACK(VSYNC_CALLBACK, default_vsync);
    INIT_CALLBACK(HSYNC_CALLBACK, default_hsync);
    INIT_CALLBACK(TEXTURE_OVERFLOW_CALLBACK, default_texture_overflow);
    INIT_CALLBACK(STRIP_OVERRUN_CALLBACK, default_strip_overrun);
    INIT_CALLBACK(END_VERTEX_CALLBACK, default_end_of_vertex);
    INIT_CALLBACK(END_YUV_CALLBACK, default_end_of_yuv);
    INIT_CALLBACK(WAIT_VSYNC_CALLBACK, default_wait_vsync);
    INIT_CALLBACK(FATAL_ERROR_CALLBACK, default_fatal_error);
    INIT_CALLBACK(PAL_EXTENSION_CALLBACK, default_pal_extension);
#undef INIT_CALLBACK
}

/* restore slot's default handler, and keep the argument registered last. the
 * PAL extension callback has no default, so a null one changes nothing */
static void reset_callback(volatile struct kmi_device_extension *device,
                           KMINT32 slot)
{
#define RESET_CALLBACK(slot, handler)                                          \
    case slot:                                                                 \
        device->callback[slot].function = handler;                             \
        break
    switch (slot) {
        RESET_CALLBACK(END_RENDER_CALLBACK, default_end_of_render);
        RESET_CALLBACK(VSYNC_CALLBACK, default_vsync);
        RESET_CALLBACK(HSYNC_CALLBACK, default_hsync);
        RESET_CALLBACK(TEXTURE_OVERFLOW_CALLBACK, default_texture_overflow);
        RESET_CALLBACK(STRIP_OVERRUN_CALLBACK, default_strip_overrun);
        RESET_CALLBACK(END_VERTEX_CALLBACK, default_end_of_vertex);
        RESET_CALLBACK(END_YUV_CALLBACK, default_end_of_yuv);
        RESET_CALLBACK(WAIT_VSYNC_CALLBACK, default_wait_vsync);
        RESET_CALLBACK(FATAL_ERROR_CALLBACK, default_fatal_error);
    }
#undef RESET_CALLBACK
}

void kmiRegisterCallback(volatile struct kmi_device_extension *device,
                         KMINT32 slot, PKMCALLBACKFUNC callback, void *argument)
{
    KMINT32 mask = get_imask();

    set_imask(IMASK_ALL);
    if (callback) {
        /* @hack type: the handler stored through device without volatile,
         * where device would do */
        ((struct kmi_device_extension *)device)->callback[slot].function =
            callback;
        device->callback[slot].argument = argument;
    } else {
        reset_callback(device, slot);
    }
    set_imask(mask);
}

KMSTATUS kmSetEORCallback(PKMCALLBACKFUNC callback, void *argument)
{
    kmiRegisterCallback(kmiHardwareResource, END_RENDER_CALLBACK, callback,
                        argument);
    return KMSTATUS_SUCCESS;
}

KMSTATUS kmSetVSyncCallback(PKMCALLBACKFUNC callback, void *argument)
{
    kmiEnableVSyncInterrupt(TRUE);
    kmiRegisterCallback(kmiHardwareResource, VSYNC_CALLBACK, callback,
                        argument);
    return KMSTATUS_SUCCESS;
}

KMSTATUS kmSetHSyncCallback(PKMCALLBACKFUNC callback, void *argument)
{
    kmiEnableHSyncInterrupt(callback != NULL);
    kmiRegisterCallback(kmiHardwareResource, HSYNC_CALLBACK, callback,
                        argument);
    return KMSTATUS_SUCCESS;
}

KMSTATUS kmSetHSyncLine(KMINT32 line)
{
    KMINT32 origin;
    KMINT32 field_lines;
    KMINT32 position;
    KMINT32 hblank;

    origin = kmiDeviceExtension.origin_y1 + kmiDeviceExtension.offset_y;
    if (KMI_TWO_FIELDS) {
        field_lines = kmiDeviceExtension.sync_load >> (LOAD_LINES_SHIFT + 1);
    } else {
        field_lines = kmiDeviceExtension.sync_load >> LOAD_LINES_SHIFT;
    }
    if (KMI_VGA_MODE) {
        position = line + origin;
    } else if (kmiDeviceExtension.display_mode & KMI_HEIGHT_480) {
        position = (line / 2) + origin;
    } else {
        position = line + origin;
    }
    if (position < 0) {
        position += field_lines;
    }
    if (position > field_lines) {
        position = field_lines;
    }
    if (position < 0) {
        position = 0;
    }
    line = position & SCANLINE_MASK;
    hblank = kmiDeviceExtension.hblank_interrupt & HBLANK_START;
    hblank |= line;
    kmiDeviceExtension.hblank_interrupt = hblank;
    kmiWriteRegister(KMI_HBLANK_INTERRUPT, hblank);
    return KMSTATUS_SUCCESS;
}

KMSTATUS kmGetCurrentScanline(KMINT32 *line)
{
    /* @hack redundant: register on status */
    register KMDWORD status;
    KMINT32 position;
    KMINT32 origin;
    KMINT32 visible;
    KMINT32 field_lines;
    KMFLOAT scale;

#ifdef NONMATCHING
    /* where status is unset, the image's holds line, as the field test's
     * tag says */
    status = (KMDWORD)line;
#endif
    if (kmiDeviceExtension.capabilities & KMI_CAN_READ_SYNC) {
        status = kmiReadRegister(KMI_SYNC_STATUS);
        position = status & SCANLINE_MASK;
    } else {
        position = 0;
    }
    scale = 1.0F;
    origin = kmiDeviceExtension.origin_y1 + kmiDeviceExtension.offset_y;
    if (KMI_VGA_MODE) {
        visible = 480;
    } else if (KMI_TWO_FIELDS) {
        if (kmiDeviceExtension.display_mode & KMI_HEIGHT_480) {
            visible = 480;
            /* @hack redundant: scale, already 1.0F, set to it again */
            scale = 1.0F;
            origin += origin;
        } else {
            visible = 240;
            origin += origin;
            scale = 0.5F;
        }
    } else {
        visible = 240;
    }
    field_lines = kmiDeviceExtension.sync_load >> LOAD_LINES_SHIFT;
    if (KMI_TWO_FIELDS) {
        /* @bug uninitialized: on a chip without KMI_CAN_READ_SYNC, the field
         * test reads status unset. the branch at 0x8c12bd24 skips its read, and
         * the test at 0x8c12bdf2 reads R4, which still holds line */
        /* @bug logic: the field test reads bit 18 of KMI_SYNC_STATUS, where the
         * interrupt handler reads the display's field from bit 10 (the test at
         * 0x8c12bdf2) */
        /* NOLINTNEXTLINE(clang-diagnostic-sometimes-uninitialized) */
        if (status & FIELD_TWO) {
            position++;
        }
    }
    position -= origin;
    position = (KMINT32)((KMFLOAT)position * scale);
    if (position <= visible) {
        *line = position;
    } else {
        *line = position - field_lines;
    }
    return KMSTATUS_SUCCESS;
}

KMSTATUS kmSetTexOverflowCallback(PKMCALLBACKFUNC callback, void *argument)
{
    kmiRegisterCallback(kmiHardwareResource, TEXTURE_OVERFLOW_CALLBACK,
                        callback, argument);
    return KMSTATUS_SUCCESS;
}

KMSTATUS kmSetStripOverRunCallback(PKMCALLBACKFUNC callback, void *argument)
{
    kmiRegisterCallback(kmiHardwareResource, STRIP_OVERRUN_CALLBACK, callback,
                        argument);
    return KMSTATUS_SUCCESS;
}

KMSTATUS kmSetEndOfVertexCallback(PKMCALLBACKFUNC callback, void *argument)
{
    kmiRegisterCallback(kmiHardwareResource, END_VERTEX_CALLBACK, callback,
                        argument);
    return KMSTATUS_SUCCESS;
}

KMSTATUS kmSetEndOfYUVCallback(PKMCALLBACKFUNC callback, void *argument)
{
    kmiRegisterCallback(kmiHardwareResource, END_YUV_CALLBACK, callback,
                        argument);
    return KMSTATUS_SUCCESS;
}

KMSTATUS kmSetWaitVsyncCallback(PKMCALLBACKFUNC callback, void *argument)
{
    kmiRegisterCallback(kmiHardwareResource, WAIT_VSYNC_CALLBACK, callback,
                        argument);
    return KMSTATUS_SUCCESS;
}

KMSTATUS kmfSetFatalErrorCallback(PKMCALLBACKFUNC pFatalErrorCallback)
{
    kmiRegisterCallback(kmiHardwareResource, FATAL_ERROR_CALLBACK,
                        pFatalErrorCallback, NULL);
    return KMSTATUS_SUCCESS;
}

KMSTATUS kmSetPALEXTCallback(PKMCALLBACKFUNC callback, void *Reserved)
{
    kmiRegisterCallback(kmiHardwareResource, PAL_EXTENSION_CALLBACK, callback,
                        NULL);
    return KMSTATUS_SUCCESS;
}

static void default_end_of_render(void *argument)
{
}
static void default_vsync(void *argument)
{
}
static void default_hsync(void *argument)
{
}
static void default_texture_overflow(void *argument)
{
}
static void default_strip_overrun(void *argument)
{
}
static void default_end_of_vertex(void *argument)
{
}
static void default_end_of_yuv(void *argument)
{
}
static void default_wait_vsync(void *argument)
{
}

static void default_fatal_error(void *code)
{
    KMPACKEDARGB color;

#ifdef NONMATCHING
    color.dwPacked = 0xff000000; /* black, for a code the switch lacks */
#endif
    switch ((KMDWORD)code) {
    case KMI_FATAL_DMA_QUEUE_FULL:
        color.dwPacked = BLUE;
        break;
    case KMI_FATAL_LIST_OVERFLOW:
        color.dwPacked = RED;
        break;
    case KMI_FATAL_BANK_OVERFLOW:
        color.dwPacked = GREEN;
        break;
    case KMI_FATAL_FLIP_TIMEOUT:
        color.dwPacked = MAGENTA;
        break;
    }
    /* @bug uninitialized: a code other than the four sets no color, and
     * kmSetBorderColor gets color's stack word unwritten (the branch at
     * 0x8c12bf6a, the load at 0x8c12bf86) */
    kmSetBorderColor(color);
    for (;;) {
    }
}

static void default_pal_extension(void *argument)
{
    ((PKMPALEXTINFO)argument)->nPALExtMode = DEFAULT_PAL_SELECTION;
}
