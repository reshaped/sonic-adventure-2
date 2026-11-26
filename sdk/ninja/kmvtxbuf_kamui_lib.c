/* Kamui's vertex buffers, and the system configuration that makes them and
 * the tile accelerator's buffer. a vertex buffer holds its lists in two
 * banks, or in one under KM_LATENCYMODEL_2V. a list can open with a header,
 * and its write pointer then starts past the header. list sizes count
 * words */
#include <kamui.h>
#include <kamui_buffer.h>
#include <kamui_internal.h>
#include <kamui_texture.h>

/* kmiGetSysConfigFlag's value once the system configuration is set up.
 * before then, CONFIG_VERTEX marks a vertex buffer requested, and
 * CONFIG_COMPLETE every part requested */
#define CONFIG_READY 0x80000000UL
#define CONFIG_VERTEX 0x100
#define CONFIG_COMPLETE 0xb00
/* the most bytes ta_bytes takes. kmCreateVertexBuffer and kmCreateTABuffer
 * take the frame buffers' bytes off this limit. under the profile's
 * EnableTexture16MB, kmSetSystemConfiguration doubles it */
#define TA_BYTES_LIMIT 0x200000
/* the fewest words kmCreateVertexBuffer sizes ta_bytes for */
#define VERTEX_WORDS_LEAST 0x10000
/* the tile accelerator's polygon input, as a DMA target. a list a NOBUF model
 * sends with no buffer takes it as its base */
#define TA_INPUT 0x10000000UL
/* the words a list holds with only its opening header */
#define LIST_HEADER_WORDS 8
/* the words each list's room in a bank keeps past its limit.
 * kmCreateVertexBuffer and kmSetSystemConfiguration take them off each size,
 * and kmiGetVertexBasePointer gives them back past each list */
#define LIST_SPARE_WORDS 16
/* the lists kmCreateVertexBuffer and kmCreateTABuffer take: all but
 * KM_PUNCHTHROUGH_POLYGON */
#define CREATE_LISTS 4
/* the NOBUF model that sends list with no buffer. a model's MODEL_LIST bits
 * name its list */
#define NOBUF_MODEL(list) (KM_VERTEXBUFMODEL_NOBUF_OPAQUE_POLYGON + (list))
#define MODEL_LIST 15
/* kmiDummyOpaque's words. on a chip with KMI_DUMMY_OPAQUE,
 * kmSetSystemConfiguration adds DUMMY_OPAQUE_SHARE words to an opaque share of
 * fewer */
#define DUMMY_OPAQUE_WORDS 32
#define DUMMY_OPAQUE_SHARE                                                     \
    (LIST_HEADER_WORDS + DUMMY_OPAQUE_WORDS + LIST_SPARE_WORDS)
/* kmiGetDisplayData's color for the display's own pixel format */
#define CURRENT_COLOR 255
/* a wait for the renderer or the tile accelerator gives up after this many
 * vertical blanks */
#define WAIT_VBLANKS 10

/* a list's object pointer block code. the code picks its blocks' size from
 * OPB_WORDS: none, 8, 16 or 32 words. each tile takes one block for each
 * list */
#define OPB_NONE 0
#define OPB_8 1
#define OPB_16 2
#define OPB_32 3
#define OPB_WORDS {0, 8, 16, 32}
/* the options word holds each list's code in the OPB_CODE bits from
 * OPB_SHIFT(list) up. OPB_DOWNWARD is its bit that sets opb_downward */
#define OPB_CODE 3
#define OPB_SHIFT(list) (17 + (2 * (list)))
#define OPB_DOWNWARD 0x10000
/* a word's bytes, and those each tile's region array entry takes */
#define WORD_BYTES 4
#define REGION_ENTRY_BYTES 24

/* the control word of a list's opening header: a polygon's parameter type,
 * and the list from LIST_TYPE_SHIFT up */
#define PARA_POLYGON_WORD 0x80000000UL
#define LIST_TYPE_SHIFT 24
#define HEADER_CONTROL(list)                                                   \
    (PARA_POLYGON_WORD | ((KMDWORD)(list) << LIST_TYPE_SHIFT))
/* the TSP word of a list's opening header: no fog, and a D adjust of
 * KM_MIPMAP_D_ADJUST_1_00 */
#define TSP_FOG_SHIFT 22
#define TSP_MIPMAP_SHIFT 8
#define HEADER_TSP                                                             \
    (((KMDWORD)KM_NOFOG << TSP_FOG_SHIFT) |                                    \
     ((KMDWORD)KM_MIPMAP_D_ADJUST_1_00 << TSP_MIPMAP_SHIFT))

/* KMI_SOFT_RESET's bit, and KMI_TA_LIST_INIT's bit that starts a frame's
 * lists */
#define RESET_TA 1
#define LIST_INIT_START 0x80000000

struct kmi_system_config kmiSystemConfig;
KMCURRENTLISTSTATE kmiCurrentListState;
struct kmi_list_bases *kmiVertexBuff;
KMDWORD **kmiCurrentVertexBuff;
KMDWORD **kmiDstVertexBuffer;
static struct kmi_list_bases *bank_pointers[2];
static struct kmi_list_bases bank_bases[2];
static KMDWORD *write_cursor[KMI_LISTS];
/* where each list starts in the current bank, its header included */
static KMDWORD *list_origin[KMI_LISTS];
/* SHC lays out BSG, the uninitialized data, in the order of first
 * declaration. kamui_vertex.h declares kmiCurrentListType, which follows the
 * cursors above */
#include <kamui_vertex.h>
KMDWORD kmiCurrentListType;
/* the tile accelerator's buffer, as kmCreateVertexBuffer or kmCreateTABuffer
 * allocated its two halves, for kmDiscardVertexBuffer to free */
static KMDWORD ta_buffers[2];
/* the words each list's header takes: LIST_HEADER_WORDS for a list
 * kmiCopyDummyGlobalData opens, else 0 */
static KMDWORD header_words[KMI_LISTS];
static KMDWORD config_flags = 0;
/* 1 while the lists go straight to the tile accelerator, as kmCreateTABuffer
 * and KM_CONFIGFLAG_USEDIRECTMODE send them, else 0 */
static KMDWORD direct_transfer = 0;

#ifdef NONMATCHING
#define EMPTY_LOOP() ((void)0)
#define DEAD_TEST(x) ((void)0)
#else
/* @hack redundant: an empty loop of one pass */
#define EMPTY_LOOP()                                                           \
    do {                                                                       \
    } while (0)
#define DEAD_TEST(x)                                                           \
    /* @hack redundant: a test of x in a block that never runs */              \
    if (0) {                                                                   \
        if (x) {                                                               \
        }                                                                      \
    }
#endif

#define SET_TA_LAYOUT(options)                                                 \
    kmiDeviceExtension.ta_buffer_size = kmiSystemConfig.ta_bytes;              \
    kmiSetTAEnvironment(                                                       \
        kmiDeviceExtension.ta_layout[0].region_array >> KMI_TA_ADDRESS_SHIFT,  \
        kmiDeviceExtension.ta_layout[0].object_list >> KMI_TA_ADDRESS_SHIFT,   \
        kmiDeviceExtension.ta_layout[0].isp_params >> KMI_TA_ADDRESS_SHIFT,    \
        kmiDeviceExtension.ta_layout[0].tsp_params >> KMI_TA_ADDRESS_SHIFT,    \
        (options), 0)
#define START_TA()                                                             \
    direct_transfer = 1;                                                       \
    kmiDeviceExtension.ta_slot = kmiDeviceExtension.write_slot;                \
    kmiDeviceExtension.slot_state[kmiDeviceExtension.write_slot] =             \
        KMI_FRAME_TA;                                                          \
    kmiDeviceExtension.ta_busy = 0;                                            \
    kmiDeviceExtension.flip_pending = 1

#define SET_STARTS()                                                           \
    write_cursor[0] = bank_bases[0].opaque;                                    \
    write_cursor[1] = bank_bases[0].opaque_modifier;                           \
    write_cursor[2] = bank_bases[0].translucent;                               \
    write_cursor[3] = bank_bases[0].translucent_modifier
#define INIT_CURSORS(list_count, banks)                                        \
    for (i = 0; i < (list_count); i++)                                         \
        list_origin[i] = write_cursor[i];                                      \
    kmiCurrentListState.ListType = 0;                                          \
    kmiCurrentListState.GlobalParamSize = 0;                                   \
    kmiCurrentListState.Reserved0 = 0;                                         \
    kmiCurrentListState.VertexBank = 0;                                        \
    kmiVertexBuff = bank_bases;                                                \
    kmiCurrentVertexBuff = write_cursor;                                       \
    for (i = 0; i < (banks); i++)                                              \
        bank_pointers[i] = &bank_bases[i];                                     \
    kmiCopyDummyGlobalData(banks);                                             \
    for (i = 0; i < (list_count); i++)                                         \
    kmiCurrentVertexBuff[i] += header_words[i]
#define SET_DESCRIPTOR(desc)                                                   \
    (desc)->pCurrentPtr = write_cursor;                                        \
    (desc)->pGlobalParam = (KMDWORD *)kmiGlobalWork;                           \
    (desc)->pCurrentListState = &kmiCurrentListState;                          \
    (desc)->ppBuffer = bank_pointers
#ifdef NONMATCHING
/* this side refuses a list of LIST_SPARE_WORDS words or fewer, whose size
 * the other side lets wrap */
#define RESERVE_SPARE(count)                                                   \
    for (i = 0; i < (count); i++) {                                            \
        if (sizes[i]) {                                                        \
            if (sizes[i] <= LIST_SPARE_WORDS)                                  \
                return KMSTATUS_NOT_ENOUGH_MEMORY;                             \
            sizes[i] -= LIST_SPARE_WORDS;                                      \
        }                                                                      \
    }
#else
/* @bug arithmetic: a nonempty size below LIST_SPARE_WORDS wraps when the
 * spare words come off it. in kmCreateVertexBuffer, a size of
 * LIST_SPARE_WORDS leaves a list in use with no room for its header (the
 * subtraction at 0x8c151682) */
#define RESERVE_SPARE(count)                                                   \
    /* @hack spelling: the first size's test peeled off the loop, where the    \
     * loop from 0 would do */                                                 \
    if (sizes[0])                                                              \
        sizes[0] -= LIST_SPARE_WORDS;                                          \
    for (i = 1; i < (count); i++) {                                            \
        if (sizes[i])                                                          \
            sizes[i] -= LIST_SPARE_WORDS;                                      \
    }
#endif
#define STORE_LIST_SIZES()                                                     \
    kmiSystemConfig.opaque_words = sizes[0];                                   \
    kmiSystemConfig.opaque_modifier_words = sizes[1];                          \
    kmiSystemConfig.translucent_words = sizes[2];                              \
    kmiSystemConfig.translucent_modifier_words = sizes[3]
/* each list's bit of desc's enabled-list mask, as its size. a list not in use
 * has size 0 */
#define TA_LIST_MASKS(desc, sizes)                                             \
    (sizes)[0] = (desc)->Reserved0 & (1 << KM_OPAQUE_POLYGON);                 \
    (sizes)[1] = (desc)->Reserved0 & (1 << KM_OPAQUE_MODIFIER);                \
    (sizes)[2] = (desc)->Reserved0 & (1 << KM_TRANS_POLYGON);                  \
    (sizes)[3] = (desc)->Reserved0 & (1 << KM_TRANS_MODIFIER)

KMDWORD **kmiGetDstVertexBuffer(void)
{
    return &kmiCurrentVertexBuff[kmiCurrentListType];
}
KMSTATUS kmCreateVertexBuffer(PKMVERTEXBUFFDESC desc, KMDWORD opaque,
                              KMDWORD opaque_modifier, KMDWORD translucent,
                              KMDWORD translucent_modifier)
{
    KMDWORD result = 0;
    KMDWORD total;
    KMDWORD sizes[CREATE_LISTS];
#ifdef NONMATCHING
#else
    /* @hack redundant: a copy of OPB_WORDS that nothing reads */
    KMDWORD unread_words[4] = OPB_WORDS;
#endif
    KMDWORD i;
    KMDWORD banks = 2;
    KMDWORD lists;
    KMDWORD words;
    KMDWORD options;
    KMSTATUS status;
    KMDWORD available;
    kmiSystemConfig.mode = KMI_VERTEX_BUFFER;
    if (kmiDeviceExtension.capabilities & KMI_DUMMY_OPAQUE) {
        opaque += DUMMY_OPAQUE_WORDS;
    }
    DEAD_TEST(opaque_modifier);
    kmiSystemConfig.buffer = desc;
    kmiSystemConfig.opaque_words = opaque;
    kmiSystemConfig.opaque_modifier_words = opaque_modifier;
    kmiSystemConfig.translucent_words = translucent;
    DEAD_TEST(translucent_modifier);
    kmiSystemConfig.translucent_modifier_words = translucent_modifier;
    if (kmiGetSysConfigFlag() == CONFIG_READY) {
        lists = kmiChkActiveListType(opaque, opaque_modifier, translucent,
                                     translucent_modifier, 0);
        desc->Reserved0 = kmiDeviceExtension.lists_used;
        direct_transfer = 0;
        for (i = 0; i < CREATE_LISTS; i++) {
            header_words[i] = 0;
        }
        options = kmiGetPackOBPParam(opaque, opaque_modifier, translucent,
                                     translucent_modifier, 0, 0);
        kmiGetNativeRegOBPSize(options);
        /* @hack spelling: the sum in two statements, where one would do */
        total = opaque + opaque_modifier + translucent;
        total += translucent_modifier;
        /* words goes to kmiGetVertexBasePointer's unread spare */
        words = total + (lists * LIST_HEADER_WORDS) + 160;
        sizes[0] = opaque;
        sizes[1] = opaque_modifier;
        sizes[2] = translucent;
        sizes[3] = translucent_modifier;
        RESERVE_SPARE(CREATE_LISTS);
        STORE_LIST_SIZES();
        opaque = sizes[0];
        opaque_modifier = sizes[1];
        translucent = sizes[2];
        translucent_modifier = sizes[3];
        if (kmiDeviceExtension.latency_model == KM_LATENCYMODEL_2V) {
            banks = 1;
        }
        status = kmiGetVertexBasePointer(opaque, opaque_modifier, translucent,
                                         translucent_modifier, 0, words, banks);
        if (status) {
            return status;
        }
        kmiGetDisplayData(CURRENT_COLOR, kmiSystemConfig.strip_buffer,
                          kmiSystemConfig.frame[0].height);
        if (total < VERTEX_WORDS_LEAST) {
            total = VERTEX_WORDS_LEAST;
        }
        kmiSystemConfig.ta_bytes =
            (total * WORD_BYTES) + kmiSystemConfig.object_bytes;
        available = TA_BYTES_LIMIT - kmiSystemConfig.frame_buffer_bytes;
        if (kmiSystemConfig.ta_bytes > available) {
            kmiSystemConfig.ta_bytes = available;
        }
        result = kmiTAallocEx(kmiSystemConfig.ta_bytes, &ta_buffers[0],
                              &ta_buffers[1]);
        if (result) {
            return result;
        }
        SET_TA_LAYOUT(options & ~kmiSystemConfig.strip_buffer);
        SET_STARTS();
        EMPTY_LOOP();
        INIT_CURSORS(CREATE_LISTS, banks);
        SET_DESCRIPTOR(desc);
    } else if (kmiAddSysConfigFlag(CONFIG_VERTEX) == CONFIG_COMPLETE) {
        kmiSetSysConfigFlag(CONFIG_READY);
        result = kmiAllocSysConfig(kmiSystemConfig.mode);
    }
    return result;
}

void kmiFlipVertexBuffer(KMDWORD bank)
{
    KMDWORD i;
    if (kmiDeviceExtension.latency_model != KM_LATENCYMODEL_2V) {
        kmiVertexBuff = &bank_bases[bank];
    } else {
        kmiVertexBuff = &bank_bases[0];
    }
    kmiCurrentVertexBuff[0] = kmiVertexBuff->opaque;
    kmiCurrentVertexBuff[1] = kmiVertexBuff->opaque_modifier;
    kmiCurrentVertexBuff[2] = kmiVertexBuff->translucent;
    kmiCurrentVertexBuff[3] = kmiVertexBuff->translucent_modifier;
    kmiCurrentVertexBuff[4] = kmiVertexBuff->punch_through;
    for (i = 0; i < KMI_LISTS; i++) {
        list_origin[i] = kmiCurrentVertexBuff[i];
    }
    kmiCurrentListState.VertexBank = bank & 1;
    for (i = 0; i < KMI_LISTS; i++) {
        kmiCurrentVertexBuff[i] += header_words[i];
    }
}

KMSTATUS kmFlushVertexBuffer(KMINT32 list)
{
    KMI_DMA_HANDLE *handle;
    KMUINT32 bytes =
        (kmiCurrentVertexBuff[list] - list_origin[list]) * sizeof(KMDWORD);
#ifdef NONMATCHING
    KMUINT32 result;
    if (!bytes) {
        return KMSTATUS_SUCCESS;
    }
    do {
        result = kmiDMAtoTARequest(TA_INPUT, list_origin[list], bytes,
                                   KMI_DMA_SOURCE_UP, &handle);
    } while (result == KMI_DMA_QUEUE_FULL);
    if (result) {
        return result;
    }
#else
    EMPTY_LOOP();
    while (kmiDMAtoTARequest(TA_INPUT, list_origin[list], bytes,
                             KMI_DMA_SOURCE_UP,
                             &handle) == KMI_DMA_QUEUE_FULL) {
    }
#endif
    kmiCurrentVertexBuff[list] = list_origin[list] + header_words[list];
    /* @bug memory: where the list holds no bytes, this reads through handle,
     * which kmiDMAtoTARequest leaves unset: it returns KMI_DMA_NO_BYTES for
     * no bytes (the read at 0x8c1510c4) */
    kmiDeviceExtension.flush_dma = *handle;
    return KMSTATUS_SUCCESS;
}
KMSTATUS kmQueryFinishFlushVertexBuffer(void)
{
    if (kmiQueryFinishDMA(kmiDeviceExtension.flush_dma) != 1) {
        return KMSTATUS_LAST_LIST_UNDER_DMA;
    }
    return KMSTATUS_SUCCESS;
}

KMSTATUS kmCreateTABuffer(PKMVERTEXBUFFDESC desc, KMDWORD ta_bytes)
{
    KMDWORD sizes[CREATE_LISTS];
    KMDWORD options;
    KMDWORD available;
#ifdef NONMATCHING
    /* this side repairs the tag below: a call that leaves the configuration
     * waiting returns KMSTATUS_SUCCESS */
    KMSTATUS result = KMSTATUS_SUCCESS;
#else
    /* @bug uninitialized: a call that leaves the configuration waiting
     * returns result unset: the caller's R12 (0x8c15129e) */
    KMSTATUS result;
#endif
    kmiSystemConfig.mode = KMI_TA_BUFFER;
    kmiSystemConfig.buffer = desc;
    kmiSystemConfig.ta_bytes = ta_bytes;
    if (kmiGetSysConfigFlag() == CONFIG_READY) {
        START_TA();
        TA_LIST_MASKS(desc, sizes);
        kmiChkActiveListType(sizes[0], sizes[1], sizes[2], sizes[3], 0);
        options =
            kmiGetPackOBPParam(sizes[0], sizes[1], sizes[2], sizes[3], 0, 1);
        kmiGetNativeRegOBPSize(options);
        /* @hack spelling: the halving as x = x >> 1, where x >>= 1 would do */
        kmiSystemConfig.ta_bytes = kmiSystemConfig.ta_bytes >> 1;
        kmiGetDisplayData(CURRENT_COLOR, kmiSystemConfig.strip_buffer,
                          kmiSystemConfig.frame[0].height);
        available = TA_BYTES_LIMIT - kmiSystemConfig.frame_buffer_bytes;
        if (kmiSystemConfig.ta_bytes > available) {
            kmiSystemConfig.ta_bytes = available;
        }
        result = kmiTAallocEx(kmiSystemConfig.ta_bytes, &ta_buffers[0],
                              &ta_buffers[1]);
        if (result) {
            return result;
        }
        SET_TA_LAYOUT(options & ~kmiSystemConfig.strip_buffer);
        kmiDeviceExtension.ta_busy = 1;
        /* NOLINTNEXTLINE(clang-diagnostic-sometimes-uninitialized) */
    } else if (kmiAddSysConfigFlag(CONFIG_VERTEX) == CONFIG_COMPLETE) {
        kmiSetSysConfigFlag(CONFIG_READY);
        result = kmiAllocSysConfig(kmiSystemConfig.mode);
    }
    return result;
}

KMSTATUS kmDiscardVertexBuffer(PKMVERTEXBUFFDESC pBufferDesc)
{
    KMSTATUS result;
    KMINT32 i;
    EMPTY_LOOP();
    result = kmiFBfreeEx(ta_buffers[0], ta_buffers[1]);
    if (result) {
        return result;
    }
    for (i = 0; i < KMI_LISTS; i++) {
        header_words[i] = 0;
    }
    return KMSTATUS_SUCCESS;
}

#define WAIT_IDLE(field)                                                       \
    kmiDeviceExtension.vsync_count = 0;                                        \
    while (kmiDeviceExtension.field) {                                         \
        kmiDeviceExtension.callback[WAIT_VSYNC_CALLBACK].function(             \
            kmiDeviceExtension.callback[WAIT_VSYNC_CALLBACK].argument);        \
        if (kmiDeviceExtension.vsync_count > WAIT_VBLANKS)                     \
            break;                                                             \
    }
KMSTATUS kmChangeLatencyModel(KMUINT32 latency, KMINT32 model)
{
    WAIT_IDLE(ta_busy);
    kmiDeviceExtension.latency_model = latency;
    kmiDeviceExtension.vertex_buffer_model = model;
    kmiDeviceExtension.vertex_buffer_list = model & MODEL_LIST;
    KMI_RESET_SLOTS();
    return KMSTATUS_SUCCESS;
}

KMDWORD kmGetCurrentVertexOffset(KMINT32 ListType)
{
    return kmiCurrentVertexBuff[ListType] - list_origin[ListType];
}
KMSTATUS kmChangeVertexOffset(KMINT32 list, KMDWORD offset)
{
    kmiCurrentVertexBuff[list] = list_origin[list] + offset;
    return KMSTATUS_SUCCESS;
}
KMSTATUS kmChangeVertexPCW(KMINT32 list, KMDWORD control, KMDWORD advance)
{
    kmiDstVertexBuffer = &kmiCurrentVertexBuff[list];
    kmiMemCopy8(*kmiDstVertexBuffer, &control, sizeof(control));
    *kmiDstVertexBuffer += advance;
    return KMSTATUS_SUCCESS;
}

/* the vertex buffer is pVertexBuffer, which the caller supplies. each bank
 * takes half of it, or all of it under KM_LATENCYMODEL_2V. fBufferSize's
 * percents share a bank among the lists. the tile accelerator's two
 * halves share the video memory the textures and both frame buffers leave.
 * each half takes at most TA_BYTES_LIMIT, or twice that under
 * EnableTexture16MB */
KMSTATUS kmSetSystemConfiguration(PKMSYSTEMCONFIGSTRUCT setup)
{
    KMFLOAT percent[KMI_LISTS];
    KMFLOAT total_percent = 0;
    KMDWORD banks = 2;
    KMDWORD clear;
    KMDWORD strips;
    KMDWORD sync;
    KMDWORD available;
    KMDWORD i;
    KMDWORD total;
#ifdef NONMATCHING
    KMDWORD largest_list = 0;
#else
    KMDWORD largest_list;
#endif
    KMDWORD sizes[KMI_LISTS];
    KMDWORD buffers[2];
    KMDWORD direct_sizes[KMI_LISTS];
    KMDWORD options;
    KMDWORD *opaque;
    KMDWORD *opaque_modifier;
    KMDWORD *translucent;
    KMDWORD *translucent_modifier;
    KMDWORD *punch_through;
    long largest;
    KMDWORD excess;
    KMDWORD *size;
    KMSTATUS result;

    kmiFBinit();
    kmiDiscardRenderPipeline();
    EMPTY_LOOP();
    kmiSystemConfig.mode = KMI_NO_BUFFER;
    if (setup->flags & KM_CONFIGFLAG_ENABLE_CLEAR_FRAMEBUFFER) {
        clear = 1;
    } else {
        clear = 0;
    }
    if (setup->flags & KM_CONFIGFLAG_NOWAITVSYNC) {
        sync = 0;
    } else {
        sync = 1;
    }
    if (setup->flags & KM_CONFIGFLAG_ENABLE_STRIPBUFFER) {
        strips = 1;
    } else {
        strips = 0;
    }
    if (setup->flags & KM_CONFIGFLAG_NOWAIT_FINISH_TEXTUREDMA) {
        kmiDeviceExtension.frame_dma_wait = 0;
    } else {
        kmiDeviceExtension.frame_dma_wait = 1;
    }
    kmiGetDisplayData(CURRENT_COLOR, strips, setup->nHeight);
    if (!(setup->flags & KM_CONFIGFLAG_USEDIRECTMODE)) {
        direct_transfer = 0;
        if (setup->flags & KM_CONFIGFLAG_ENABLE_2V_LATENCY) {
            kmiDeviceExtension.latency_model = KM_LATENCYMODEL_2V;
            kmiDeviceExtension.frame_dma_wait = 1;
        } else {
            kmiDeviceExtension.latency_model = KM_LATENCYMODEL_3V;
        }
        kmChangeLatencyModel(kmiDeviceExtension.latency_model,
                             setup->nVbufModel);
        available = setup->nVertexBufferSize;
        kmiDeviceExtension.vertex_base = setup->pVertexBuffer;
        if ((KMDWORD)kmiDeviceExtension.vertex_base & 31) {
#ifdef NONMATCHING
            if (available <
                32 - ((KMDWORD)kmiDeviceExtension.vertex_base & 31)) {
                return KMSTATUS_NOT_ENOUGH_MEMORY;
            }
#endif
            available -= 32 - ((KMDWORD)kmiDeviceExtension.vertex_base & 31);
            kmiDeviceExtension.vertex_base =
                (void *)(((KMDWORD)kmiDeviceExtension.vertex_base + 31) & ~31);
        }
        if (kmiDeviceExtension.latency_model == KM_LATENCYMODEL_3V) {
            available >>= 1;
        }
        for (i = 0; i < KMI_LISTS; i++) {
            sizes[i] = 0;
        }
        for (i = 0; i < KMI_LISTS; i++) {
            total_percent += setup->fBufferSize[i];
        }
        if (total_percent > 100.0F) {
            for (i = 0; i < KMI_LISTS; i++) {
                percent[i] = setup->fBufferSize[i] * 100.0F / total_percent;
            }
        } else {
            for (i = 0; i < KMI_LISTS; i++) {
                percent[i] = setup->fBufferSize[i];
            }
        }
        for (i = 0; i < KMI_LISTS; i++) {
            sizes[i] = (KMDWORD)(percent[i] * available / 100.0F) & ~7;
        }
        if (kmiDeviceExtension.capabilities & KMI_DUMMY_OPAQUE) {
            if (sizes[0] < DUMMY_OPAQUE_SHARE) {
                sizes[0] += DUMMY_OPAQUE_SHARE;
            }
        }
        total = 0;
        largest = -1;
        for (i = 0; i < KMI_LISTS; i++) {
            /* @hack type: each size reached through size, and compared signed
             * through a long view of it, where sizes[i] and (long)sizes[i]
             * would do */
            size = &sizes[i];
            total += *size;
            if (*(long *)size > largest) {
                largest_list = i;
                largest = sizes[i];
            }
        }
        if (total > available) {
            excess = total - available;
#ifdef NONMATCHING
            if (excess > sizes[largest_list]) {
                return KMSTATUS_NOT_ENOUGH_MEMORY;
            }
#endif
            /* @hack spelling: the subtraction as x = x - excess, where
             * x -= excess would do */
            sizes[largest_list] = sizes[largest_list] - excess;
            sizes[largest_list] &= ~31;
        }
        RESERVE_SPARE(KMI_LISTS);
        kmiSystemConfig.buffer = setup->pBufferDesc;
        STORE_LIST_SIZES();
        kmiSystemConfig.punch_through_words = sizes[4];
        /* @hack spelling: the sizes passed to kmiChkActiveListType through
         * pointers set in this order, where sizes[0] to sizes[4] would do */
        translucent_modifier = &sizes[3];
        opaque_modifier = &sizes[1];
        opaque = &sizes[0];
        translucent = &sizes[2];
        punch_through = &sizes[4];
        kmiChkActiveListType(*opaque, *opaque_modifier, *translucent,
                             *translucent_modifier, *punch_through);
        setup->pBufferDesc->Reserved0 = kmiDeviceExtension.lists_used;
        for (i = 0; i < KMI_LISTS; i++) {
            header_words[i] = 0;
        }
        options = kmiGetPackOBPParam(sizes[0], sizes[1], sizes[2], sizes[3],
                                     sizes[4], 0);
        kmiGetNativeRegOBPSize(options);
        if (kmiDeviceExtension.latency_model == KM_LATENCYMODEL_2V) {
            banks = 1;
        }
        kmiGetVertexBasePointer(sizes[0], sizes[1], sizes[2], sizes[3],
                                sizes[4], 0, banks);
        SET_STARTS();
        write_cursor[4] = bank_bases[0].punch_through;
        INIT_CURSORS(KMI_LISTS, banks);
    } else {
        kmiDeviceExtension.frame_dma_wait = 1;
        START_TA();
        TA_LIST_MASKS(setup->pBufferDesc, direct_sizes);
        direct_sizes[4] =
            setup->pBufferDesc->Reserved0 & (1 << KM_PUNCHTHROUGH_POLYGON);
        kmiChkActiveListType(direct_sizes[0], direct_sizes[1], direct_sizes[2],
                             direct_sizes[3], direct_sizes[4]);
        options = kmiGetPackOBPParam(direct_sizes[0], direct_sizes[1],
                                     direct_sizes[2], direct_sizes[3],
                                     direct_sizes[4], 1);
        kmiGetNativeRegOBPSize(options);
    }
#ifdef NONMATCHING
    if (setup->nTextureMemorySize >
        kmiDeviceExtension.texture_memory_size / sizeof(KMDWORD)) {
        return KMSTATUS_NOT_ENOUGH_MEMORY;
    }
#endif
    kmiSystemConfig.texture_words = setup->nTextureMemorySize;
    if (kmiSystemConfig.texture_words & 7) {
        /* @hack spelling: the rounding in two statements, where one would
         * do */
        kmiSystemConfig.texture_words += 7;
        kmiSystemConfig.texture_words &= ~7;
    }
#ifdef NONMATCHING
    if (kmiSystemConfig.texture_words >
            kmiDeviceExtension.texture_memory_size / sizeof(KMDWORD) ||
        kmiSystemConfig.frame_buffer_bytes >
            (kmiDeviceExtension.texture_memory_size -
             (kmiSystemConfig.texture_words * sizeof(KMDWORD))) /
                2) {
        return KMSTATUS_NOT_ENOUGH_MEMORY;
    }
#endif
    kmiSystemConfig.ta_bytes = (kmiDeviceExtension.texture_memory_size -
                                (kmiSystemConfig.texture_words * WORD_BYTES) -
                                (kmiSystemConfig.frame_buffer_bytes * 2)) /
                               2;
    if (kmiGetProfileInt("HARDWARE", "EnableTexture16MB", 0) == 1) {
        if (kmiSystemConfig.ta_bytes > 2 * TA_BYTES_LIMIT) {
            kmiSystemConfig.ta_bytes = 2 * TA_BYTES_LIMIT;
        }
    } else {
        if (kmiSystemConfig.ta_bytes > TA_BYTES_LIMIT) {
            kmiSystemConfig.ta_bytes = TA_BYTES_LIMIT;
        }
    }
    available = kmiSystemConfig.ta_bytes;
    if (available < kmiSystemConfig.object_bytes) {
        return KMSTATUS_NOT_ENOUGH_MEMORY;
    }
    result = kmiTAallocEx(available, &buffers[0], &buffers[1]);
    if (result) {
        return result;
    }
    SET_TA_LAYOUT(options & ~strips);
    if (setup->flags & KM_CONFIGFLAG_USEDIRECTMODE) {
        kmiDeviceExtension.ta_busy = 1;
    }
    SET_DESCRIPTOR(setup->pBufferDesc);
    kmiSystemConfig.strip_buffer = strips;
    kmiDeviceExtension.strip_buffer = strips;
    kmiSystemConfig.frame[0].surface = setup->ppSurfaceDescArray[0];
    kmiSystemConfig.frame[0].width = setup->nWidth;
    kmiSystemConfig.frame[0].height = setup->nHeight;
    kmiSystemConfig.frame[0].clear = clear;
    kmiSystemConfig.frame[1].surface = setup->ppSurfaceDescArray[1];
    kmiSystemConfig.frame[1].width = setup->nWidth;
    kmiSystemConfig.frame[1].height = setup->nHeight;
    kmiSystemConfig.frame[1].clear = clear;
    kmiSystemConfig.front = setup->ppSurfaceDescArray[0];
    kmiSystemConfig.back = setup->ppSurfaceDescArray[1];
    kmiSystemConfig.vsync = sync;
    DEAD_TEST(clear);
    kmiSetSysConfigFlag(CONFIG_READY);
    result = kmiAllocSysConfig(kmiSystemConfig.mode);
    return result;
}

#define MARK_UNBUFFERED(list)                                                  \
    if (kmiDeviceExtension.vertex_buffer_model == NOBUF_MODEL(list))           \
    unbuffered[list] = 1
#define COUNT_LIST(words, list)                                                \
    if (words) {                                                               \
        kmiDeviceExtension.lists_used |= 1UL << (list);                        \
        count++;                                                               \
    }
/* set lists_used to the bit of each list whose size isn't 0. the result counts
 * those lists. under KM_LATENCYMODEL_2V, the list a NOBUF model sends with no
 * buffer counts as in use, whatever its size */
KMDWORD kmiChkActiveListType(KMDWORD opaque, KMDWORD opaque_modifier,
                             KMDWORD translucent, KMDWORD translucent_modifier,
                             KMDWORD punch_through)
{
    KMDWORD unbuffered[KMI_LISTS];
    KMDWORD i;
    KMDWORD count = 0;
    for (i = 0; i < KMI_LISTS; i++) {
        unbuffered[i] = 0;
    }
    if (kmiDeviceExtension.latency_model == KM_LATENCYMODEL_2V) {
        MARK_UNBUFFERED(0);
        MARK_UNBUFFERED(1);
        MARK_UNBUFFERED(2);
        MARK_UNBUFFERED(3);
        MARK_UNBUFFERED(4);
    }
    opaque += unbuffered[0];
    opaque_modifier += unbuffered[1];
    translucent += unbuffered[2];
    translucent_modifier += unbuffered[3];
    punch_through += unbuffered[4];
    kmiDeviceExtension.lists_used = 0;
    COUNT_LIST(opaque, 0);
    COUNT_LIST(opaque_modifier, 1);
    COUNT_LIST(translucent, 2);
    COUNT_LIST(translucent_modifier, 3);
    COUNT_LIST(punch_through, 4);
    return count;
}

#define PACK_CODE(code, list)                                                  \
    (((code) << OPB_SHIFT(list)) & ((KMDWORD)OPB_CODE << OPB_SHIFT(list)))
/* the options word kmiSetTAEnvironment takes for lists of these sizes:
 * ta_options' low byte, OPB_DOWNWARD and each list's code. direct is 1 when
 * the lists go straight to the tile accelerator. on a chip with
 * KMI_FLUSH_ENTRY, dummy_translucent records whether the opaque modifier,
 * translucent and punch-through lists all take blocks */
KMDWORD kmiGetPackOBPParam(KMDWORD opaque, KMDWORD opaque_modifier,
                           KMDWORD translucent, KMDWORD translucent_modifier,
                           KMDWORD punch_through, KMDWORD direct)
{
#ifdef NONMATCHING
#else
    /* @hack redundant: a copy of OPB_WORDS that nothing reads */
    KMDWORD unread_words[4] = OPB_WORDS;
#endif
    KMDWORD packed = kmiDeviceExtension.ta_options & 0xff;
    KMDWORD opaque_code;
    KMDWORD opaque_modifier_code;
    KMDWORD translucent_code;
    KMDWORD translucent_modifier_code;
    KMDWORD punch_code;
    opaque_code = opaque_modifier_code = translucent_code =
        translucent_modifier_code = punch_code = OPB_NONE;
    if (opaque_modifier) {
        opaque_modifier_code = OPB_8;
    }
    if (translucent_modifier) {
        translucent_modifier_code = OPB_8;
    }
    if (punch_through) {
        punch_code = OPB_16;
    }
    if (direct == 1) {
        if (opaque) {
            opaque_code = OPB_16;
        }
        if (translucent) {
            translucent_code = OPB_16;
        }
    } else {
        if (opaque) {
            opaque_code = OPB_32;
        }
        if (translucent) {
            translucent_code = OPB_32;
        }
        if (opaque && translucent) {
            if (opaque < translucent) {
                opaque_code = OPB_16;
            } else {
                translucent_code = OPB_16;
            }
        }
    }
    if (kmiDeviceExtension.latency_model == KM_LATENCYMODEL_2V) {
        if (kmiDeviceExtension.vertex_buffer_model == NOBUF_MODEL(0)) {
            opaque_code = OPB_32;
        }
        if (kmiDeviceExtension.vertex_buffer_model == NOBUF_MODEL(1)) {
            opaque_modifier_code = OPB_16;
        }
        if (kmiDeviceExtension.vertex_buffer_model == NOBUF_MODEL(2)) {
            translucent_code = OPB_32;
        }
        if (kmiDeviceExtension.vertex_buffer_model == NOBUF_MODEL(3)) {
            translucent_modifier_code = OPB_16;
        }
        if (kmiDeviceExtension.vertex_buffer_model == NOBUF_MODEL(4)) {
            punch_code = OPB_32;
        }
    }
    packed |= PACK_CODE(opaque_code, 0) | PACK_CODE(opaque_modifier_code, 1) |
              PACK_CODE(translucent_code, 2) |
              PACK_CODE(translucent_modifier_code, 3) |
              PACK_CODE(punch_code, 4) | OPB_DOWNWARD;
    if (kmiDeviceExtension.capabilities & KMI_FLUSH_ENTRY) {
        kmiDeviceExtension.dummy_translucent =
            opaque_modifier_code && translucent_code && punch_code;
    }
    return packed;
}

#define FILL_HEADER(list, field)                                               \
    if (kmiDeviceExtension.latency_model == KM_LATENCYMODEL_3V ||              \
        kmiDeviceExtension.vertex_buffer_model != NOBUF_MODEL(list)) {         \
        for (i = 0; i < banks; i++)                                            \
            kmiMemCopy4(bank_bases[i].field, header, sizeof(header));          \
        header_words[list] = LIST_HEADER_WORDS;                                \
    }
#define FILL_NEXT(list, field)                                                 \
    if (kmiDeviceExtension.lists_used & (1 << (list))) {                       \
        header[0] = HEADER_CONTROL(list);                                      \
        FILL_HEADER(list, field);                                              \
    }
/* open each list lists_used marks with a header of its own, in each bank.
 * under KM_LATENCYMODEL_2V, the list a NOBUF model sends with no buffer gets
 * none */
void kmiCopyDummyGlobalData(KMDWORD banks)
{
    KMDWORD header[LIST_HEADER_WORDS] = {HEADER_CONTROL(KM_OPAQUE_POLYGON),
                                         0,
                                         HEADER_TSP,
                                         0,
                                         0xffffffff,
                                         0xffffffff,
                                         0xffffffff,
                                         0xffffffff};
    KMDWORD i;
#ifdef NONMATCHING
#else
    /* @hack redundant: unused, a copy of bank_bases' address that nothing
     * reads */
    struct kmi_list_bases *unused = bank_bases;
#endif
    if (kmiDeviceExtension.lists_used & (1 << KM_OPAQUE_POLYGON)) {
        FILL_HEADER(0, opaque);
    }
    FILL_NEXT(1, opaque_modifier);
    FILL_NEXT(2, translucent);
    FILL_NEXT(3, translucent_modifier);
    FILL_NEXT(4, punch_through);
}

#define CODE_AT(packed, list) (((packed) >> OPB_SHIFT(list)) & OPB_CODE)
/* size object_bytes for the options kmiGetPackOBPParam packs. each tile takes
 * a region array entry and a block for each list. a chip with
 * KMI_FLUSH_ENTRY takes every tile twice, or four times while
 * dummy_translucent is set */
void kmiGetNativeRegOBPSize(KMDWORD packed)
{
    KMDWORD opaque_code;
    KMDWORD opaque_modifier_code;
    KMDWORD translucent_code;
    KMDWORD translucent_modifier_code;
    KMDWORD punch_code;
    KMDWORD tiles;
    KMDWORD words[4] = OPB_WORDS;
    opaque_code = CODE_AT(packed, 0);
    opaque_modifier_code = CODE_AT(packed, 1);
    translucent_code = CODE_AT(packed, 2);
    translucent_modifier_code = CODE_AT(packed, 3);
    punch_code = CODE_AT(packed, 4);
    tiles = kmiDeviceExtension.tiles_x * kmiDeviceExtension.tiles_y;
    if (kmiDeviceExtension.capabilities & KMI_FLUSH_ENTRY) {
        if (kmiDeviceExtension.dummy_translucent) {
            tiles *= 4;
        } else {
            tiles *= 2;
        }
    }
    kmiSystemConfig.object_bytes =
        (((words[opaque_code] + words[opaque_modifier_code] +
           words[translucent_code] + words[translucent_modifier_code] +
           words[punch_code]) *
          WORD_BYTES) +
         REGION_ENTRY_BYTES) *
        tiles;
}

#define PLACE_LIST(list, field)                                                \
    if (kmiDeviceExtension.latency_model == KM_LATENCYMODEL_2V &&              \
        kmiDeviceExtension.vertex_buffer_model == NOBUF_MODEL(list)) {         \
        bank_bases[bank].field = (KMDWORD *)TA_INPUT;                          \
    } else if ((field) > 0) {                                                  \
        bank_bases[bank].field = (KMDWORD *)base[bank] + used;                 \
        used = used + (field);                                                 \
        used += LIST_SPARE_WORDS + 7;                                          \
        used &= ~7;                                                            \
    } else {                                                                   \
        bank_bases[bank].field = 0;                                            \
    }
/* lay the lists out in each bank, in ListType order. each list's list_limit
 * takes its size in bytes. a list of size 0 gets a null base. bank 0 starts at
 * vertex_base rounded up to 32 bytes, and bank 1 where bank 0's lists end.
 * spare isn't read */
KMSTATUS kmiGetVertexBasePointer(KMDWORD opaque, KMDWORD opaque_modifier,
                                 KMDWORD translucent,
                                 KMDWORD translucent_modifier,
                                 KMDWORD punch_through, KMDWORD spare,
                                 KMDWORD banks)
{
    long base[2];
    long used = 0;
    KMDWORD bank;
    kmiDeviceExtension.list_limit[0] = opaque * WORD_BYTES;
    kmiDeviceExtension.list_limit[1] = opaque_modifier * WORD_BYTES;
    kmiDeviceExtension.list_limit[2] = translucent * WORD_BYTES;
    kmiDeviceExtension.list_limit[3] = translucent_modifier * WORD_BYTES;
    kmiDeviceExtension.list_limit[4] = punch_through * WORD_BYTES;
    base[0] = (long)kmiDeviceExtension.vertex_base;
    for (bank = 0; bank < banks; bank++) {
        if (base[bank] & 31) {
            base[bank] = (base[bank] + 31) & ~31;
        }
        PLACE_LIST(0, opaque);
        PLACE_LIST(1, opaque_modifier);
        PLACE_LIST(2, translucent);
        PLACE_LIST(3, translucent_modifier);
        PLACE_LIST(4, punch_through);
        base[1] = base[0] + (used * WORD_BYTES);
        used = 0;
    }
    return KMSTATUS_SUCCESS;
}

KMSTATUS kmiAllocSysConfig(KMINT32 mode)
{
    KMSTATUS result;
    KMDWORD i;
    KMDWORD surfaces = 2;
    KMDWORD bank;
    EMPTY_LOOP();
    if (mode == KMI_VERTEX_BUFFER) {
        result = kmCreateVertexBuffer(
            kmiSystemConfig.buffer, kmiSystemConfig.opaque_words,
            kmiSystemConfig.opaque_modifier_words,
            kmiSystemConfig.translucent_words,
            kmiSystemConfig.translucent_modifier_words);
        if (result) {
            return result;
        }
    }
    if (mode == KMI_TA_BUFFER) {
        result =
            kmCreateTABuffer(kmiSystemConfig.buffer, kmiSystemConfig.ta_bytes);
        if (result) {
            return result;
        }
    }
    if (kmiSystemConfig.frame[0].surface == kmiSystemConfig.frame[1].surface) {
        surfaces = 1;
    }
    for (i = 0; i < surfaces; i++) {
        result = kmCreateFrameBufferSurface(
            kmiSystemConfig.frame[i].surface, kmiSystemConfig.frame[i].width,
            kmiSystemConfig.frame[i].height, kmiSystemConfig.strip_buffer,
            kmiSystemConfig.frame[i].clear);
        if (result) {
            return result;
        }
    }
    result = kmActivateFrameBuffer(kmiSystemConfig.front, kmiSystemConfig.back,
                                   kmiSystemConfig.strip_buffer,
                                   kmiSystemConfig.vsync);
    if (result) {
        return result;
    }
    if (kmiDeviceExtension.latency_model == KM_LATENCYMODEL_2V ||
        direct_transfer == 1) {
        kmiDeviceExtension.ta_slot = kmiDeviceExtension.write_slot;
        kmiDeviceExtension.slot_state[kmiDeviceExtension.write_slot] =
            KMI_FRAME_WRITING;
        kmiDeviceExtension.flip_pending = 1;
        bank = kmiDeviceExtension.write_slot & 1;
        kmiWriteRegister(KMI_SOFT_RESET, RESET_TA);
        kmiWriteRegister(KMI_SOFT_RESET, 0);
        kmiWriteRegister(KMI_TA_OL_BASE,
                         kmiDeviceExtension.ta_bank[bank].ol_base);
        kmiWriteRegister(KMI_TA_OL_LIMIT,
                         kmiDeviceExtension.ta_bank[bank].ol_limit);
        kmiWriteRegister(KMI_TA_ISP_BASE,
                         kmiDeviceExtension.ta_bank[bank].isp_base);
        kmiWriteRegister(KMI_TA_ISP_LIMIT,
                         kmiDeviceExtension.ta_bank[bank].isp_limit);
        kmiWriteRegister(KMI_TA_NEXT_OPB_INIT,
                         kmiDeviceExtension.ta_bank[bank].next_opb_init);
        kmiWriteRegister(KMI_TA_LIST_INIT, LIST_INIT_START);
        kmiReadRegister(KMI_TA_LIST_INIT);
        kmiDeviceExtension.lists_done = 0;
        kmiDeviceExtension.lists_sent = kmiDeviceExtension.lists_used;
    }
    return result;
}
KMDWORD kmiAddSysConfigFlag(KMDWORD flags)
{
    config_flags |= flags;
    return config_flags;
}
KMDWORD kmiSetSysConfigFlag(KMDWORD flags)
{
    config_flags = flags;
    return config_flags;
}
KMDWORD kmiGetSysConfigFlag(void)
{
    return config_flags;
}
void kmiDiscardRenderPipeline(void)
{
    KMI_RESET_SLOTS();
    WAIT_IDLE(render_busy);
    if (kmiDeviceExtension.latency_model == KM_LATENCYMODEL_3V) {
        WAIT_IDLE(ta_busy);
    }
    kmiDeviceExtension.ta_busy = 0;
    kmiDeviceExtension.render_busy = 0;
}
