/* Kamui's driver setup and release, and the layout of the tile accelerator's
 * buffers in video memory's two banks */
#include <kamui.h>
#include <kamui_buffer.h>
#include <kamui_internal.h>
#include <kamui_texture.h>
#include <string.h>

/* each bank is half of the console's 8 MB of video memory */
#define BANKS 2
#define BANK_BYTES 0x400000
#define VIDEO_RAM_BYTES (BANKS * BANK_BYTES)
/* bank 0's bank_limit, 0x30000 below the end of video memory. bank 1's is a
 * bank lower */
#define BANK0_LIMIT 0x7d0000
#define SIZE_SHIFT 11 /* each buffer's size rounds down to whole 2 KB */
#define PASSES_PER_SECOND 2000.0F

/* the notice version.copyright points to: 621 bytes of text, with no
 * terminator */
static char driver_notice[621] = {
#include "ninja/internal_kamui_lib/driver_notice.inc"
};

/* names Kamui exports that no header declares */
/* NOLINTBEGIN(misc-use-internal-linkage) */
const char *const kmiBtBuild =
    "\nKAMUI Ver 1.11.0.0 Build:Jul 26 1999 19:23:10\n";
KMSTATUS kmiInitTimer(void);
void kmiReleaseHardware(void);
void kmiWaitAccurate(KMFLOAT seconds);
/* NOLINTEND(misc-use-internal-linkage) */

volatile struct kmi_device_extension *volatile kmiHardwareResource = NULL;
volatile struct kmi_device_extension kmiDeviceExtension;
static struct kmi_version_info version;

KMSTATUS kmiInitTimer(void)
{
    return KMSTATUS_SUCCESS;
}

void kmiInitializeDriver(void)
{
    KMDWORD object_offset;
    KMDWORD isp_offset;
    KMDWORD tsp_offset;
    KMDWORD region_size;
    KMDWORD object_size;
    KMDWORD isp_size;
    KMDWORD tsp_size;
    int bank;

    if (!kmiHardwareResource) {
        kmiHardwareResource = &kmiDeviceExtension;
        memset((void *)kmiHardwareResource, 0, sizeof(kmiDeviceExtension));
    } else {
        /* @hack redundant: an empty else */
    }
    version.major = 3;
    version.minor = 2;
    version.revision = 6;
    version.build = 0;
    version.copyright = driver_notice;
    kmiDeviceExtension.version = &version;
    kmiDeviceExtension.strip_buffer = 0;
    kmiDeviceExtension.level_adjust = 0;
    kmiDeviceExtension.skip_lines = 0;
    kmiDeviceExtension.offset_x = 0;
    kmiDeviceExtension.offset_y = 0;
    kmiDeviceExtension.pal_lines[0] = 496;
    kmiDeviceExtension.pal_lines[1] = 512;
    kmiDeviceExtension.pal_lines[2] = 528;
    kmiDeviceExtension.pal_lines[3] = 544;
    kmiDeviceExtension.pal_lines[4] = 560;
    kmiInitCallback();
    kmiDeviceExtension.render_busy = 0;
    kmiDeviceExtension.ta_busy = 0;
    kmiDeviceExtension.vsync_wait =
        kmiGetProfileInt("RENDER", "VSYNCCount", 1) - 1;
    kmiDeviceExtension.vsync_remaining = kmiDeviceExtension.vsync_wait;
    kmiDeviceExtension.memory_alignment = 0x10000;
    kmiDeviceExtension.texture_memory_size = VIDEO_RAM_BYTES;
    kmiDeviceExtension.unk_01c = VIDEO_RAM_BYTES;
    kmiDeviceExtension.global_clip = 0;
    kmiDeviceExtension.presort = 0;
    kmiDeviceExtension.regions_stale = 0;
    kmiDeviceExtension.strip_length = 3;
    kmiDeviceExtension.display_changed = 1;
    kmiInitializeBackGroundPlane();

    region_size = kmiGetProfileInt("RENDER", "RegionSize", 0x6000);
    object_size = kmiGetProfileInt("RENDER", "ObjListSize", 0x7a000);
    isp_size = kmiGetProfileInt("RENDER", "ISPParamSize", 0x180000);
    tsp_size = kmiGetProfileInt("RENDER", "TSPParamSize", 0x180000);
    region_size = (region_size >> SIZE_SHIFT) << SIZE_SHIFT;
    object_size = (object_size >> SIZE_SHIFT) << SIZE_SHIFT;
    isp_size = (isp_size >> SIZE_SHIFT) << SIZE_SHIFT;
    tsp_size = (tsp_size >> SIZE_SHIFT) << SIZE_SHIFT;
    kmiDeviceExtension.isp_size = isp_size;
    kmiDeviceExtension.tsp_size = tsp_size;
    object_offset = region_size;
    isp_offset = object_offset + object_size;
    tsp_offset = isp_offset + isp_size;
    for (bank = 0; bank < BANKS; bank++) {
        kmiDeviceExtension.ta_layout[bank].region_array = bank * BANK_BYTES;
        kmiDeviceExtension.ta_layout[bank].object_list =
            object_offset + (bank * BANK_BYTES);
        kmiDeviceExtension.ta_layout[bank].isp_params =
            isp_offset + (bank * BANK_BYTES);
        kmiDeviceExtension.ta_layout[bank].tsp_params =
            tsp_offset + (bank * BANK_BYTES);
        kmiDeviceExtension.bank_limit[bank] = BANK0_LIMIT - (bank * BANK_BYTES);
    }

    kmiDeviceExtension.latency_model = KM_LATENCYMODEL_3V;
    kmiDeviceExtension.vertex_buffer_model = 0;
    KMI_RESET_SLOTS();
    kmiDeviceExtension.frame_dma = 0;
    kmiDeviceExtension.common_modifier = 0;
    kmiDeviceExtension.frame_dma_wait = 1;
    kmiDeviceExtension.vertex_base =
        (void *)kmiGetProfileInt("VertexBuffer", "BufferBase", 0xacc00000);
    kmiDeviceExtension.vertex_size =
        kmiGetProfileInt("VertexBuffer", "BufferSize", BANK_BYTES);
    kmiDeviceExtension.polling_ta =
        kmiGetProfileInt("RENDER", "PoolingTAStatus", 0);
    kmiDeviceExtension.render_timeout =
        kmiGetProfileInt("RENDER", "RenderTimeOut", 10);
    kmiDeviceExtension.vertex_timeout =
        kmiGetProfileInt("RENDER", "VertexTimeOut", 10);
    kmiDeviceExtension.vsync_timeout =
        kmiGetProfileInt("RENDER", "VSYNCTimeOut", 10);
    kmiDeviceExtension.another_modifier = 0;
    kmiDeviceExtension.another_modifier_list = 1;
    kmiDeviceExtension.ta_options = 15;
    /* @hack redundant: an OR of 0 into ta_options, which reads the volatile
     * field back and stores it unchanged */
    kmiDeviceExtension.ta_options |= 0;
    kmiDeviceExtension.field_offset[0] = 0;
    kmiDeviceExtension.field_offset[1] = 0;
    kmiInitTimer();
    /* @hack redundant: the call in a loop of one pass */
    do {
        kmiInitRenderer();
    } while (0);
}

void kmiReleaseHardware(void)
{
    if (kmiHardwareResource) {
        kmiUnloadDevice();
        kmiHardwareResource = NULL;
    }
}

void kmiInitializeHardware(void)
{
    kmiModuleInitialize();
    kmiInitializeDevice();
    kmiInitializeTA();
    kmiFBinit();
    kmiInitInterrupt();
    kmiInitializeFog();
    kmiDeviceExtension.ta_buffer_size = 0x100000;
    kmiDeviceExtension.tiles_x = 0;
    kmiDeviceExtension.tiles_y = 0;
    kmiDeviceExtension.dummy_translucent = 0;
    kmiSetTAEnvironment(
        kmiDeviceExtension.ta_layout[0].region_array >> KMI_TA_ADDRESS_SHIFT,
        kmiDeviceExtension.ta_layout[0].object_list >> KMI_TA_ADDRESS_SHIFT,
        kmiDeviceExtension.ta_layout[0].isp_params >> KMI_TA_ADDRESS_SHIFT,
        kmiDeviceExtension.ta_layout[0].tsp_params >> KMI_TA_ADDRESS_SHIFT,
        kmiDeviceExtension.ta_options, 0);
}

KMINT32 kmiVerifyWriteRegister(KMDWORD reg, KMDWORD data)
{
    return kmiWriteRegister(reg, data);
}

void kmiWaitAccurate(KMFLOAT seconds)
{
    volatile KMDWORD tick;
    for (tick = 0; tick < (KMDWORD)(KMINT32)(seconds * PASSES_PER_SECOND);
         tick++) {
    }
}

void kmiFatalErrorEntry(KMDWORD code)
{
    kmiDeviceExtension.callback[FATAL_ERROR_CALLBACK].function((void *)code);
}
