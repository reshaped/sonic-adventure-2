/* the check model_archive.c makes of a downloaded kart model's chunk lists
 * before it relocates them. it accepts only the chunks njCnkSimpleVlist and
 * njCnkSimplePlist take, where every read stays inside the model. a strip may
 * name only a vertex the vertex list writes */
#include <nj_cnk.h>
#include <ninja.h>
#include <sg_xpt.h>
#include <string.h>

#include "cart/chunk_stream.h"

/* the vertices of Ninja's vertex buffer, as system_boot_init gives njInit3D */
#define CART_VERTEX_COUNT 4096
/* the bits of an NJD_CT_TID chunk's second halfword that index the texture
 * list, below its flags */
#define TEXTURE_INDEX_MASK 0x1fff
/* the bytes of a word and of a halfword. a vertex chunk's size counts words,
 * and a polygon chunk's halfwords */
#define WORD_BYTES 4
#define HALFWORD_BYTES 2
/* a strip vertex's bytes: an index for NJD_CS, and an index, u and v for
 * NJD_CS_UVN and NJD_CS_UVH */
#define INDEX_BYTES 2
#define UV_INDEX_BYTES 6
/* a vertex chunk's size counts the word of its first index and its count.
 * the vertices' words follow it */
#define RANGE_WORDS 1
/* the bits of a material chunk's type that name its colors: D, A and S */
#define MATERIAL_D (NJD_CM_D - NJD_MATOFF)
#define MATERIAL_A (NJD_CM_A - NJD_MATOFF)
#define MATERIAL_S (NJD_CM_S - NJD_MATOFF)
/* an NJD_CM_BU chunk's halfwords after its size: two vectors of three */
#define BUMP_HALFWORDS 6
/* njCnkSimplePlist sign-extends a strip chunk's header. it then indexes its
 * polygon functions, rows 0 to 127, by the header's flags. a header with
 * HEADER_SIGN set would index below row 0 */
#define HEADER_SIGN 0x8000

static Uint16 le16(const Uint8 *bytes)
{
    return (Uint16)(bytes[0] | (bytes[1] << 8));
}

/* the words each vertex takes in a vertex chunk with head, its NJD_FV_CONT
 * cleared. it returns 0 where the check refuses the chunk. it refuses a type
 * the switch doesn't name, and any other flag */
static int vertex_words(int head)
{
    switch (head) {
    case NJD_CV:
        return 3;
    case NJD_CV_SH:
    case NJD_CV_D8:
    case NJD_CV_UF:
    case NJD_CV_NF:
    case NJD_CV_S5:
    case NJD_CV_S4:
    case NJD_CV_IN:
    case NJD_CV_VNX:
        return 4;
    case NJD_CV_VNX_D8:
    case NJD_CV_VNX_UF:
        return 5;
    case NJD_CV_VN:
        return 6;
    case NJD_CV_VN_D8:
    case NJD_CV_VN_UF:
    case NJD_CV_VN_S5:
    case NJD_CV_VN_S4:
    case NJD_CV_VN_IN:
        return 7;
    case NJD_CV_VN_SH:
        return 8;
    default:
        /* njCnkSimpleVlist hands NJD_CV_VN_NF alone to
         * njCnkSimpleVertexVNNF. that function writes each vertex where its
         * NF word says. read_vertices marks the vertices from the chunk's
         * first index on */
        return 0;
    }
}

/* walk the vertex list at *cursor to its end chunk, and mark in written each
 * vertex it writes. a vertex chunk opens with a word of its head and its size.
 * a word of its first index and its count follows, then its vertices. on TRUE
 * *cursor is the offset past the end chunk */
static Bool read_vertices(const Uint8 *model, Uint32 size, Uint32 *cursor,
                          Uint8 *written)
{
    Uint32 at = *cursor;
    Uint32 first, count, words, i;
    Uint16 head;
    int stride;

    if (at % WORD_BYTES != 0) {
        return FALSE;
    }
    while (at <= size && size - at >= WORD_BYTES) {
        head = le16(model + at);
        words = le16(model + at + HALFWORD_BYTES);
        at += WORD_BYTES;
        if (head == NJD_CE && words == 0) {
            *cursor = at;
            return TRUE;
        }
        stride = vertex_words(head & ~NJD_FV_CONT);
        if (!stride || !words || words > (size - at) / WORD_BYTES) {
            return FALSE;
        }
        first = le16(model + at);
        count = le16(model + at + HALFWORD_BYTES);
        if (!count || first >= CART_VERTEX_COUNT ||
            count > CART_VERTEX_COUNT - first ||
            words != RANGE_WORDS + stride * count) {
            return FALSE;
        }
        for (i = first; i < first + count; i++) {
            written[i / 8] |= 1U << (i % 8);
        }
        at += words * WORD_BYTES;
    }
    return FALSE;
}

/* whether a strip chunk's strips fill it from at, the halfword that counts
 * them, to end. a vertex takes vertex_size bytes, then its user halfwords. it
 * may name only a vertex written marks */
static Bool read_strips(const Uint8 *model, Uint32 at, Uint32 end,
                        int vertex_size, const Uint8 *written)
{
    Uint32 count, user_bytes, strips, vertex;
    Sint16 length;
    Uint16 index;
    Uint32 bytes;

    if (end - at < HALFWORD_BYTES) {
        return FALSE;
    }
    strips = le16(model + at);
    at += HALFWORD_BYTES;
    user_bytes = (strips >> NJD_UFO_SHIFT) * HALFWORD_BYTES;
    strips &= (1U << NJD_UFO_SHIFT) - 1;
    if (!strips) {
        return FALSE;
    }
    while (strips--) {
        if (end - at < HALFWORD_BYTES) {
            return FALSE;
        }
        length = (Sint16)le16(model + at);
        at += HALFWORD_BYTES;
        /* a negative length marks a strip wound the other way. a strip holds
         * count - CNK_STRIP_LEAD triangles, and must hold one */
        count = length < 0 ? (Uint32)-length : (Uint32)length;
        if (count <= CNK_STRIP_LEAD) {
            return FALSE;
        }
        for (vertex = 0; vertex < count; vertex++) {
            bytes = vertex_size;
            /* a strip's first two vertices take no user halfwords */
            if (vertex >= CNK_STRIP_LEAD) {
                bytes += user_bytes;
            }
            if (bytes > end - at) {
                return FALSE;
            }
            index = le16(model + at);
            if (index >= CART_VERTEX_COUNT ||
                !(written[index / 8] & (1U << (index % 8)))) {
                return FALSE;
            }
            at += bytes;
        }
    }
    /* a halfword of 0 may pad the chunk to a word */
    return at == end || (at % WORD_BYTES != 0 && end - at == HALFWORD_BYTES &&
                         le16(model + at) == 0);
}

/* walk the polygon list at *cursor to its end chunk. a texture chunk's index
 * must lie below textures. on TRUE *cursor is the offset past the end chunk */
static Bool read_polygons(const Uint8 *model, Uint32 size, Uint32 *cursor,
                          Uint32 textures, const Uint8 *written)
{
    Uint32 at = *cursor;
    Uint32 end, words, colors;
    Uint16 head, type;
    Bool after_texture = FALSE;
    Bool texture_bound = FALSE;

    if (at % HALFWORD_BYTES != 0) {
        return FALSE;
    }
    while (at <= size && size - at >= HALFWORD_BYTES) {
        head = le16(model + at);
        at += HALFWORD_BYTES;
        type = CNK_TYPE(head);
        /* after a bump texture's chunk, njCnkSimplePlist reads a head with any
         * bit of NJD_CM_BU as an NJD_CM_BU chunk's. it then reads a size and
         * BUMP_HALFWORDS halfwords. only NJD_CM_BU and NJD_CM_DAS end where
         * that reading ends. the check can't see a texture's format. so after
         * any texture chunk, it refuses such a head of any other type */
        if (after_texture && (head & NJD_CM_BU) && type != NJD_CM_BU &&
            type != NJD_CM_DAS) {
            return FALSE;
        }
        after_texture = FALSE;
        if (head == NJD_CE) {
            *cursor = at;
            return TRUE;
        }
        if (head == NJD_CN || type == NJD_CB_BA || type == NJD_CB_DA ||
            type == NJD_CB_EXP) {
            continue;
        }
        if (size - at < HALFWORD_BYTES) {
            return FALSE;
        }
        words = le16(model + at);
        at += HALFWORD_BYTES;
        if (type == NJD_CT_TID) {
            if ((words & TEXTURE_INDEX_MASK) >= textures) {
                return FALSE;
            }
            after_texture = TRUE;
            texture_bound = TRUE;
            continue;
        }
        if (words > (size - at) / HALFWORD_BYTES) {
            return FALSE;
        }
        end = at + words * HALFWORD_BYTES;
        if (type >= NJD_CM_D && type <= NJD_CM_DAS) {
            /* a word for each color the type names */
            colors = ((type & MATERIAL_D) != 0) + ((type & MATERIAL_A) != 0) +
                     ((type & MATERIAL_S) != 0);
            if (at % WORD_BYTES != 0 || words != colors * CNK_WORD_HALFWORDS) {
                return FALSE;
            }
        } else if (type == NJD_CM_BU) {
            if (words != BUMP_HALFWORDS) {
                return FALSE;
            }
        } else if (type == NJD_CS || type == NJD_CS_UVN || type == NJD_CS_UVH) {
            /* a strip with u and v, or with NJD_FST_ENV, maps a texture. a
             * texture chunk must come before it */
            if ((head & HEADER_SIGN) ||
                ((type != NJD_CS || (head & NJD_FST_ENV)) && !texture_bound) ||
                !read_strips(model, at, end,
                             type == NJD_CS ? INDEX_BYTES : UV_INDEX_BYTES,
                             written)) {
                return FALSE;
            }
        } else {
            /* the check refuses any other chunk. among them, NJD_CB_CP and
             * NJD_CB_DP store or draw a list outside the model */
            return FALSE;
        }
        at = end;
    }
    return FALSE;
}

Bool cart_chunk_streams_valid(const Uint8 *model, Uint32 size, Uint32 vertices,
                              Uint32 polygons, Uint32 textures,
                              Uint32 *vertex_bytes, Uint32 *polygon_bytes)
{
    Uint8 written[CART_VERTEX_COUNT / 8];
    Uint32 vertex_end = vertices;
    Uint32 polygon_end = polygons;

    if (model == NULL || vertex_bytes == NULL || polygon_bytes == NULL) {
        return FALSE;
    }
    memset(written, 0, sizeof(written));
    if ((vertices && !read_vertices(model, size, &vertex_end, written)) ||
        (polygons &&
         !read_polygons(model, size, &polygon_end, textures, written))) {
        return FALSE;
    }
    *vertex_bytes = vertex_end - vertices;
    *polygon_bytes = polygon_end - polygons;
    return TRUE;
}
