/* Kamui's system configuration, which its frame buffer and vertex buffer
 * members share */
#ifndef KAMUI_BUFFER_H
#define KAMUI_BUFFER_H
#include <kamui.h>
#include <kamui_internal.h>

struct kmi_frame_request {
    PKMSURFACEDESC surface;
    KMINT32 width;
    KMINT32 height;
    KMBOOLEAN clear;
};

struct kmi_system_config {
    PKMVERTEXBUFFDESC buffer;
    KMDWORD opaque_words;
    KMDWORD opaque_modifier_words;
    KMDWORD translucent_words;
    KMDWORD translucent_modifier_words;
    KMDWORD punch_through_words;
    KMBOOLEAN strip_buffer;
    struct kmi_frame_request frame[2];
    PKMSURFACEDESC front;
    PKMSURFACEDESC back;
    KMBOOLEAN vsync;
    KMINT32 width;
    KMINT32 height;
    KMINT32 bytes_per_pixel;
    /* the buffer kmiAllocSysConfig lays out: none yet, kmCreateVertexBuffer's,
     * or kmCreateTABuffer's */
    KMINT32 mode;
#define KMI_NO_BUFFER 0
#define KMI_VERTEX_BUFFER 1
#define KMI_TA_BUFFER 2
    KMDWORD ta_bytes;
    KMUINT32 texture_words;
    KMDWORD frame_buffer_bytes;
    KMDWORD object_bytes;
};

struct kmi_list_bases {
    KMDWORD *opaque;
    KMDWORD *opaque_modifier;
    KMDWORD *translucent;
    KMDWORD *translucent_modifier;
    KMDWORD *punch_through;
};
extern struct kmi_system_config kmiSystemConfig;
extern KMCURRENTLISTSTATE kmiCurrentListState;
extern struct kmi_list_bases *kmiVertexBuff;
extern KMDWORD **kmiCurrentVertexBuff;
extern KMDWORD **kmiDstVertexBuffer;
extern struct kmi_interrupt_statistics kmiIntStatisticData;

KMSTATUS kmiFBalloc(KMINT32 bytes, PKMSURFACEDESC surface);
void kmiGetDisplayData(KMDWORD color, KMDWORD strip_buffer, KMDWORD height);

KMSTATUS kmActivateFrameBuffer(PKMSURFACEDESC front, PKMSURFACEDESC back,
                               KMBOOLEAN strip_buffer, KMBOOLEAN vsync);
KMDWORD **kmiGetDstVertexBuffer(void);
KMSTATUS kmCreateVertexBuffer(PKMVERTEXBUFFDESC desc, KMDWORD opaque,
                              KMDWORD opaque_modifier, KMDWORD translucent,
                              KMDWORD translucent_modifier);
KMSTATUS kmCreateTABuffer(PKMVERTEXBUFFDESC desc, KMDWORD ta_bytes);
KMSTATUS kmFlushVertexBuffer(KMINT32 list);
/* kmChangeLatencyModel's latencies, as latency_model holds them */
#define KM_LATENCYMODEL_3V 0
#define KM_LATENCYMODEL_2V 1
KMSTATUS kmChangeLatencyModel(KMUINT32 latency, KMINT32 model);
KMSTATUS kmChangeVertexOffset(KMINT32 list, KMDWORD offset);
KMSTATUS kmChangeVertexPCW(KMINT32 list, KMDWORD control, KMDWORD advance);
KMDWORD kmiChkActiveListType(KMDWORD opaque, KMDWORD opaque_modifier,
                             KMDWORD translucent, KMDWORD translucent_modifier,
                             KMDWORD punch_through);
KMDWORD kmiGetPackOBPParam(KMDWORD opaque, KMDWORD opaque_modifier,
                           KMDWORD translucent, KMDWORD translucent_modifier,
                           KMDWORD punch_through, KMDWORD direct);
void kmiGetNativeRegOBPSize(KMDWORD packed);
void kmiCopyDummyGlobalData(KMDWORD banks);
KMSTATUS kmiGetVertexBasePointer(KMDWORD opaque, KMDWORD opaque_modifier,
                                 KMDWORD translucent,
                                 KMDWORD translucent_modifier,
                                 KMDWORD punch_through, KMDWORD spare,
                                 KMDWORD banks);
KMDWORD kmiGetSysConfigFlag(void);
KMDWORD kmiAddSysConfigFlag(KMDWORD flags);
KMDWORD kmiSetSysConfigFlag(KMDWORD flags);
KMSTATUS kmiAllocSysConfig(KMINT32 mode);
void kmiFlipVertexBuffer(KMDWORD bank);
KMSTATUS kmQueryFinishFlushVertexBuffer(void);

#endif
