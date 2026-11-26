/* Kamui's writes to its vertex lists: a strip's global parameter, a vertex
 * and a user clip. each goes to its list through the store queues */
#include <kamui.h>
#include <kamui_buffer.h>
#include <kamui_internal.h>
#include <kamui_vertex.h>
#include <machine.h>

/* the store queue address control registers, one for each queue */
#define QACR0 (*(volatile KMDWORD *)0xff000038)
#define QACR1 (*(volatile KMDWORD *)0xff00003c)
/* a store to an address in SQ_AREA goes to a store queue. a prefetch of the
 * address writes the queue to memory. the queue lands at the address's
 * SQ_OFFSET bits, within the 64 MB area its QACR register names. an address
 * shifted right by QACR_AREA_SHIFT holds the area's bits 26 to 28 where the
 * register takes them */
#define SQ_AREA 0xe0000000
#define SQ_OFFSET 0x03ffffff
#define QACR_AREA_SHIFT 24
/* the words a store queue holds, and the bytes both queues hold */
#define SQ_WORDS 8
#define TWO_QUEUES 64
/* a byte count shifted right by WORD_SHIFT counts words */
#define WORD_SHIFT 2

/* the words a list holds with only its opening header */
#define LIST_HEADER_WORDS 8

/* a parameter opens with its global word. the word holds the parameter's type
 * from GLOBAL_PARA_SHIFT up, and its list from GLOBAL_LIST_SHIFT up.
 * GLOBAL_GROUP_EN is its group bit, and GLOBAL_USER_CLIP its user clip mode.
 * on a modifier volume's parameter, GLOBAL_VOLUME marks the volume's last
 * triangle */
#define GLOBAL_PARA_SHIFT 29
#define GLOBAL_LIST_SHIFT 24
#define GLOBAL_GROUP_EN 0x00800000
#define GLOBAL_USER_CLIP 0x00030000
#define GLOBAL_VOLUME 0x00000040
/* the type of a user clip's parameter, and the words of it that hold its
 * bounds */
#define PARA_USER_CLIP 1
#define CLIP_XMIN 4
#define CLIP_YMIN 5
#define CLIP_XMAX 6
#define CLIP_YMAX 7
/* a modifier volume's type and list. it names the opaque modifier list, even
 * where the header goes to the translucent one */
#define GLOBAL_MODIFIER                                                        \
    (((KMDWORD)KM_PARAM_MODIFIERVOLUME << GLOBAL_PARA_SHIFT) |                 \
     ((KMDWORD)KM_OPAQUE_MODIFIER << GLOBAL_LIST_SHIFT))

/* the ListType bit of the two modifier volume lists, KM_OPAQUE_MODIFIER and
 * KM_TRANS_MODIFIER */
#define LIST_MODIFIER 1

/* the ISP word's offset color bit */
#define ISP_OFFSET 0x01000000
/* the shifts of the TSP word's fog mode, mipmap D adjust and texture
 * shading */
#define TSP_FOG_SHIFT 22
#define TSP_MIPMAP_SHIFT 8
#define TSP_SHADING_SHIFT 6
/* blank_header's TSP word */
#define BLANK_TSP                                                              \
    (((KMDWORD)KM_NOFOG << TSP_FOG_SHIFT) |                                    \
     ((KMDWORD)KM_MIPMAP_D_ADJUST_1_00 << TSP_MIPMAP_SHIFT) |                  \
     ((KMDWORD)KM_MODULATE << TSP_SHADING_SHIFT))

/* send size bytes from source to list's write pointer through the store
 * queues. the pointer then moves past them. SEND writes out the second queue
 * only where size is TWO_QUEUES. so size must be that, or one queue's 32 */
#define SEND(desc, list, source, size)                                         \
    do {                                                                       \
        KMDWORD *sq = (desc)->pCurrentPtr[list];                               \
        /* @hack spelling: from, the source read into a local before the       \
         * queues are mapped, where the copy's argument would do */            \
        KMDWORD *from = (KMDWORD *)(source);                                   \
        QACR0 = (KMDWORD)sq >> QACR_AREA_SHIFT;                                \
        QACR1 = (KMDWORD)sq >> QACR_AREA_SHIFT;                                \
        sq = (KMDWORD *)(((KMDWORD)sq & SQ_OFFSET) | SQ_AREA);                 \
        kmiMemCopy8(sq, from, size);                                           \
        prefetch(sq);                                                          \
        if ((size) == TWO_QUEUES) {                                            \
            prefetch(sq + SQ_WORDS);                                           \
        }                                                                      \
        (desc)->pCurrentPtr[list] += (size) >> WORD_SHIFT;                     \
    } while (0)

/* @hack spelling: SEND for TWO_QUEUES bytes without SEND's test of the size,
 * where SEND(..., TWO_QUEUES) would do */
#define SEND64(desc, list, source)                                             \
    do {                                                                       \
        KMDWORD *sq = (desc)->pCurrentPtr[list];                               \
        /* @hack spelling: from, the source read into a local before the       \
         * queues are mapped, where the copy's argument would do */            \
        KMDWORD *from = (KMDWORD *)(source);                                   \
        QACR0 = (KMDWORD)sq >> QACR_AREA_SHIFT;                                \
        QACR1 = (KMDWORD)sq >> QACR_AREA_SHIFT;                                \
        sq = (KMDWORD *)(((KMDWORD)sq & SQ_OFFSET) | SQ_AREA);                 \
        kmiMemCopy8(sq, from, TWO_QUEUES);                                     \
        prefetch(sq);                                                          \
        prefetch(sq + SQ_WORDS);                                               \
        (desc)->pCurrentPtr[list] += TWO_QUEUES >> WORD_SHIFT;                 \
    } while (0)

/* how many copies of blank_sprite kmSetUserClipping sends after
 * blank_header */
#define BLANK_SPRITES 2

/* the header kmSetUserClipping sends ahead of the blank sprites. the call sets
 * its global word for the list */
static KMDWORD blank_header[SQ_WORDS] = {
    0xffffffff, ISP_OFFSET, BLANK_TSP, 0, 0, 0, 0xffffffff, 0xffffffff,
};
/* a sprite off the screen, above and left of the screen's top left corner */
static KMVERTEX16 blank_sprite = {
#include "ninja/kmvtxlst_kamui_lib/blank_sprite.inc"
};

KMSTATUS kmStartVertexStrip(PKMVERTEXBUFFDESC pBufferDesc)
{
    KMDWORD list = pBufferDesc->pCurrentListState->ListType;
    KMDWORD size = pBufferDesc->pCurrentListState->GlobalParamSize;

    SEND(pBufferDesc, list, pBufferDesc->pGlobalParam, size);
    return KMSTATUS_SUCCESS;
}

KMSTATUS kmSetVertex(PKMVERTEXBUFFDESC pBufferDesc, void *pVertex,
                     KMINT32 VertexType, KMINT32 StructSize)
{
    KMDWORD list;

    kmiCurrentListState.Reserved0 = VertexType;
    list = pBufferDesc->pCurrentListState->ListType;
    SEND(pBufferDesc, list, pVertex, StructSize);
    return KMSTATUS_SUCCESS;
}

KMSTATUS kmSetUserClipping(PKMVERTEXBUFFDESC pBufferDesc, KMINT32 ListType,
                           KMINT32 Xmin, KMINT32 Ymin, KMINT32 Xmax,
                           KMINT32 Ymax)
{
    /* the user clip's parameter. its global word comes first, then three words
     * left unwritten, then its bounds in tiles */
    KMDWORD clip[SQ_WORDS];
    int i;

    if (kmGetCurrentVertexOffset(ListType) != LIST_HEADER_WORDS) {
        /* @hack spelling: the global parameter's first word and the mask
         * each in a local, where one expression would do */
        KMDWORD global = pBufferDesc->pGlobalParam[0];
        /* @hack spelling: the header written through a pointer of its own,
         * where blank_header[0] would do */
        KMDWORD *header = blank_header;
        KMDWORD control = GLOBAL_GROUP_EN | GLOBAL_USER_CLIP;

        control &= global;
        if (ListType & LIST_MODIFIER) {
            header[0] = control | (GLOBAL_MODIFIER | GLOBAL_VOLUME);
        } else {
            header[0] =
                control | ((KMDWORD)KM_PARAM_SPRITE << GLOBAL_PARA_SHIFT);
        }
        SEND(pBufferDesc, ListType, blank_header, sizeof(blank_header));
        for (i = 0; i < BLANK_SPRITES; i++) {
            SEND64(pBufferDesc, ListType, &blank_sprite);
        }
    }
    clip[0] = (KMDWORD)PARA_USER_CLIP << GLOBAL_PARA_SHIFT;
    clip[CLIP_XMIN] = Xmin;
    clip[CLIP_YMIN] = Ymin;
    clip[CLIP_XMAX] = Xmax;
    clip[CLIP_YMAX] = Ymax;
    /* @bug uninitialized: the user clip goes out with clip[1] to clip[3],
     * which no path writes: the copy called at 0x8c1526b6 reads all 32
     * bytes of clip */
    SEND(pBufferDesc, ListType, clip, sizeof(clip));
    return KMSTATUS_SUCCESS;
}
