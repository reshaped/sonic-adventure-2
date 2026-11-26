/* a chunk model's vertices, as a source that defines a model spells them */
#ifndef DRAW_CHUNK_H
#define DRAW_CHUNK_H
#include <ninja.h>

/* a vertex chunk opens with its head and its count word, before its first
 * vertex. the head holds the chunk's type, and in its high halfword the words
 * that follow the head. the count word holds the vertices' count in its high
 * halfword. its low halfword holds the first vertex's index */
#define CHUNK_HEAD_WORDS 2
/* the shift to a word's high halfword */
#define CHUNK_HIGH_SHIFT 16
/* the head of a chunk of type with count vertices of words each */
#define CHUNK_HEAD(type, words, count)                                         \
    ((type) |                                                                  \
     ((CHUNK_HEAD_WORDS - 1 + ((count) * (words))) << CHUNK_HIGH_SHIFT))
/* the count word of a chunk of count vertices whose first vertex's index is
 * first */
#define CHUNK_COUNT_AT(first, count) (((count) << CHUNK_HIGH_SHIFT) | (first))
/* the vertices' count, from the count word of the vertex chunk vlist opens */
#define CHUNK_VERTEX_COUNT(vlist) ((Uint16)((vlist)[1] >> CHUNK_HIGH_SHIFT))

/* a vertex of an NJD_CV_VN chunk */
struct chunk_vertex_vn {
    NJS_POINT3 point;
    NJS_VECTOR normal;
};
/* the words a struct chunk_vertex_vn takes */
#define CHUNK_VN_WORDS 6

/* a vertex of an NJD_CV_D8 chunk, whose color is a packed ARGB color */
struct chunk_vertex_d8 {
    NJS_POINT3 point;
    Uint32 color;
};
/* the words a struct chunk_vertex_d8 takes */
#define CHUNK_D8_WORDS 4
/* the head and the count word of an NJD_CV_D8 chunk of count vertices */
#define CHUNK_D8_HEAD(count) CHUNK_HEAD(NJD_CV_D8, CHUNK_D8_WORDS, count)
#define CHUNK_D8_COUNT(count) CHUNK_COUNT_AT(0, count)

/* the words a vertex of an NJD_CV chunk takes: its point, an NJS_POINT3 */
#define CHUNK_V_WORDS 3

/* a vertex of an NJD_CV_VN_NF chunk. its nf word holds an index in the vertex
 * buffer in its low halfword, and a weight in 255ths in its high halfword */
struct chunk_vertex_vn_nf {
    NJS_POINT3 point;
    NJS_VECTOR normal;
    Uint32 nf;
};

/* the bits of a strip chunk's header that hold its type. as the and_attr of
 * njSetConstantAttr, they keep the type and clear the flags above it */
#define CHUNK_TYPE_BITS 0xff

#endif
