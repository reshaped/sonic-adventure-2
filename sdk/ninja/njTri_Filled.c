#include <kamui.h>
#include <machine.h>
#include <ninja.h>
#include <nj_draw3d.h>
#include <nj_km.h>
#include <sg_xpt.h>

/* a word of a projected vertex, as its bits or as a float */
union tri_word {
    Uint32 bits;
    Float value;
};

#define STRIP_VERTEX_WORDS                                                     \
    (sizeof(struct draw3d_vertex) / sizeof(union tri_word))
/* a vertex of the strip njTri_Filled sends. words views the same
 * record */
union strip_vertex {
    union tri_word words[STRIP_VERTEX_WORDS];
    struct draw3d_vertex v;
};

/* CORNERS counts the triangle's corners, and CORNER_WORDS a union corner's
 * words */
#define CORNERS 3
#define CORNER_WORDS 7
/* a corner of the triangle njDPolygonNearClip3DP cuts. sx, sy and ooz are its
 * vertex's own. z is 1 over ooz, and x and y follow from z, sx and sy. for an
 * affine current matrix, (x, y, z) is the vertex's point through that matrix.
 * words views the same record */
union corner {
    Float words[CORNER_WORDS];
    struct {
        Float sx;
        Float sy;
        Float ooz;
        Float x;
        Float y;
        Float z;
        Uint32 col;
    } f;
};

/* the member exports these functions, though no header of the SDK declares
 * them */
/* NOLINTBEGIN(misc-use-internal-linkage) */
/* send the strip of vtx's count vertices as njTri_Filled does with clip
 * above 0, from sq on. the call returns the address after the last vertex
 * sent */
Uint32 *njDPolygonClip3DP(struct draw3d_vertex *vtx, Int count, Uint32 *sq);
/* cut the triangle of a, b and c at _nj_near_clip_. the part beyond goes from
 * sq on as a strip of 3 or 4 vertices. the call returns the address after the
 * strip. at least one of the three must lie beyond. a vertex the cut adds
 * takes 1 over _nj_near_clip_ as its ooz. each byte of its color lies between
 * its edge's ends as the vertex does */
Uint32 *njDPolygonNearClip3DP(struct draw3d_vertex *a, struct draw3d_vertex *b,
                              struct draw3d_vertex *c, Uint32 *sq);
/* NOLINTEND(misc-use-internal-linkage) */

/* send vtx as a block, and step sq past it. the block holds param, then vtx's
 * fields in reverse order, sx first. words 4 and 5, u and v, stay unwritten.
 * so does word 7, the offset color */
/* @hack spelling: a vertex's words stored as sq steps back from its end,
 * where indexed stores from its start would do */
#define SEND_VERTEX(vtx, param)                                                \
    {                                                                          \
        sq += SQ_WORDS - 1;                                                    \
        *--sq = (vtx)->col;                                                    \
        sq -= UV_WORDS;                                                        \
        *(Float *)--sq = (vtx)->ooz;                                           \
        *(Float *)--sq = (vtx)->sy;                                            \
        *(Float *)--sq = (vtx)->sx;                                            \
        *--sq = (param);                                                       \
        prefetch(sq);                                                          \
        sq += SQ_WORDS;                                                        \
    }

Uint32 *njTri_Filled(void *buf, Int count, Uint32 *sq, Sint32 clip)
{
    Int lo = 0;
    Int hi = count;
    Int i = 0;
    Int n = 0;
    Uint32 param;
    union tri_word *src;

    if (clip > 0) {
        return njDPolygonClip3DP((struct draw3d_vertex *)buf, count, sq);
    }
    param = KM_VERTEXPARAM_NORMAL;
    do {
        /* @hack spelling: each vertex read through src, a cursor over its
         * words, where reads of its fields would do */
        src = ((union strip_vertex *)buf)[i].words;
        /* @hack spelling: the vertex's words stored as sq steps back from the
         * block's end, where indexed stores from its start would do */
        sq += SQ_WORDS - 1;
        *--sq = src->bits;
        src++;
        sq -= UV_WORDS;
        *(Float *)--sq = src++->value;
        *(Float *)--sq = src++->value;
        *(Float *)--sq = src->value;
        *--sq = param;
        prefetch(sq);
        sq += SQ_WORDS;
        /* @hack spelling: n stepped in a statement of its own, where ++n in
         * the test would do */
        n++;
        if (n & 1) {
            i = --hi;
        } else {
            i = ++lo;
        }
        if (--count == 0) {
            param = KM_VERTEXPARAM_ENDOFSTRIP;
        }
    } while (count >= 0);
    return sq;
}

Uint32 *njDPolygonClip3DP(struct draw3d_vertex *vtx, Int count, Uint32 *sq)
{
    /* @hack spelling: lo declared ahead of strip, where any order would do */
    Int lo = 1;
    Int strip = 0;
    Int hi = count - 1;
    Int n = 0;
    Int i;
    Int clipped;
    Float inv_near;
    struct draw3d_vertex *a;
    /* @hack redundant: register on b and c */
    register struct draw3d_vertex *b;
    register struct draw3d_vertex *c;

    a = vtx;
    b = &vtx[hi];
    /* @hack spelling: i set ahead of count's drop by two, where any order
     * would do */
    i = lo;
    count -= 2;
    inv_near = 1.0F / _nj_near_clip_;
    do {
        /* @hack spelling: clipped, a flag for the test, where the test itself
         * would do */
        clipped = 0;
        c = &vtx[i];
        if (!(a->ooz > 0.0F && a->ooz < inv_near && b->ooz > 0.0F &&
              b->ooz < inv_near && c->ooz > 0.0F && c->ooz < inv_near)) {
            clipped = 1;
        }
        if (!clipped) {
            if (!strip) {
                SEND_VERTEX(a, KM_VERTEXPARAM_NORMAL)
                strip++;
            }
            SEND_VERTEX(b, KM_VERTEXPARAM_NORMAL)
        } else {
            if (strip) {
                SEND_VERTEX(b, KM_VERTEXPARAM_ENDOFSTRIP)
                strip = 0;
            }
            if ((a->ooz > 0.0F && a->ooz < inv_near) ||
                (b->ooz > 0.0F && b->ooz < inv_near) ||
                (c->ooz > 0.0F && c->ooz < inv_near)) {
                sq = njDPolygonNearClip3DP(a, b, c, sq);
            }
        }
        /* @hack spelling: n stepped in a statement of its own, where ++n in
         * the test would do */
        n++;
        if (n & 1) {
            i = --hi;
        } else {
            i = ++lo;
        }
        /* @hack spelling: count stepped ahead of the vertices' turn, where any
         * order would do */
        count--;
        a = b;
        b = c;
    } while (count > 0);
    if (strip) {
        SEND_VERTEX(c, KM_VERTEXPARAM_ENDOFSTRIP)
    }
    return sq;
}

/* fill corner r from vertex p through w, which must stand at r's first word.
 * the macro leaves w at the next corner's first word. x and y take the
 * caller's cx, cy, inv_x and inv_y */
/* @hack spelling: one word cursor that fills the three corners, each from
 * its end down, where stores to each corner's fields would do */
#define FILL_CORNER(r, p)                                                      \
    {                                                                          \
        w += CORNER_WORDS;                                                     \
        *(Uint32 *)--w = (p)->col;                                             \
        *--w = 1.0F / (p)->ooz;                                                \
        *--w = ((p)->sy - cy) * (r)->f.z * inv_y;                              \
        *--w = ((p)->sx - cx) * (r)->f.z * inv_x;                              \
        *--w = (p)->ooz;                                                       \
        *--w = (p)->sy;                                                        \
        *--w = (p)->sx;                                                        \
        w += CORNER_WORDS;                                                     \
    }

Uint32 *njDPolygonNearClip3DP(struct draw3d_vertex *a, struct draw3d_vertex *b,
                              struct draw3d_vertex *c, Uint32 *sq)
{
#ifdef NONMATCHING
    /* this side repairs the write past out's end. out has room here for four
     * vertices, and corners for the three it uses */
    union corner corners[CORNERS];
    struct draw3d_vertex out[CORNERS + 1];
#else
    /* @bug memory: a triangle cut to four vertices writes the fourth past
     * out's end, onto corners[0], which nothing reads after it. out has room
     * for three, and corners for four where three are used. ADD #28,R14 at
     * 0x8c13c9cc puts out 28 bytes into the frame, and ADD #76,R12 at
     * 0x8c13c8d2 puts corners 48 bytes past out. MOV.L R4,@R1 at 0x8c13cbe2
     * stores a new vertex's color at out + 16 * n. ADD #48,R4 at 0x8c13cc6c
     * sends the fourth from corners[0] */
    union corner corners[CORNERS + 1];
    struct draw3d_vertex out[CORNERS];
#endif
    Float *w;
    /* @hack redundant: register on every float, argb, i, prev and n */
    register Float scale_x;
    register Float scale_y;
    register Float cx;
    register Float cy;
    register Float inv_x;
    register Float inv_y;
    register Float near;
    register Float inv_near;
    register Float t;
    register Float x;
    register Float y;
    register Float a0;
    register Float r0;
    register Float g0;
    register Float b0;
    register Float a1;
    register Float r1;
    register Float g1;
    register Float b1;
    register Float ca;
    register Float cr;
    register Float cg;
    register Float cb;
    register Uint32 argb;
    register Int i;
    register Int prev;
    register Int n;

    scale_x = _nj_screen_.dist * _nj_scr_aspect_x_;
    scale_y = _nj_screen_.dist * _nj_scr_aspect_y_;
    inv_x = 1.0F / scale_x;
    inv_y = 1.0F / scale_y;
    cx = _nj_screen_.cx;
    cy = _nj_screen_.cy;
    w = corners[0].words;
    FILL_CORNER(&corners[0], a)
    FILL_CORNER(&corners[1], b)
    FILL_CORNER(&corners[2], c)
    /* @hack spelling: n and prev set after the corners, where any place
     * before the loop would do */
    n = 0;
    prev = CORNERS - 1;
    near = _nj_near_clip_;
    /* @hack spelling: _nj_near_clip_ read again, where near would do */
    inv_near = 1.0F / _nj_near_clip_;
    scale_x *= inv_near;
    scale_y *= inv_near;
    for (i = 0; i < CORNERS; i++) {
        /* an edge runs from corner prev to corner i, so the first runs from c
         * to a. one that crosses the near clip adds a vertex where it crosses.
         * the vertex lies t of the way from the edge's start to its end */
        if ((corners[prev].f.z <= near) ^ (corners[i].f.z <= near)) {
            t = (near - corners[prev].f.z) /
                (corners[i].f.z - corners[prev].f.z);
            x = corners[prev].f.x + (t * (corners[i].f.x - corners[prev].f.x));
            y = corners[prev].f.y + (t * (corners[i].f.y - corners[prev].f.y));
            out[n].sx = cx + (x * scale_x);
            out[n].sy = cy + (y * scale_y);
            out[n].ooz = inv_near;
            argb = corners[prev].f.col;
            a0 = (Float)((argb >> ALPHA_SHIFT) & CHANNEL_MASK);
            r0 = (Float)((argb >> RED_SHIFT) & CHANNEL_MASK);
            g0 = (Float)((argb >> GREEN_SHIFT) & CHANNEL_MASK);
            b0 = (Float)(argb & CHANNEL_MASK);
            argb = corners[i].f.col;
            a1 = (Float)((argb >> ALPHA_SHIFT) & CHANNEL_MASK);
            r1 = (Float)((argb >> RED_SHIFT) & CHANNEL_MASK);
            g1 = (Float)((argb >> GREEN_SHIFT) & CHANNEL_MASK);
            b1 = (Float)(argb & CHANNEL_MASK);
            /* @hack spelling: ca, cr, cg and cb, locals for the color's sums,
             * where the sums in the packing would do */
            ca = a0 + (t * (a1 - a0));
            cr = r0 + (t * (r1 - r0));
            cg = g0 + (t * (g1 - g0));
            cb = b0 + (t * (b1 - b0));
            argb = ((Uint32)ca << ALPHA_SHIFT) | ((Uint32)cr << RED_SHIFT) |
                   ((Uint32)cg << GREEN_SHIFT) | (Uint32)cb;
            out[n].col = argb;
            n++;
        }
        if (corners[i].f.z > near) {
            out[n].sx = corners[i].f.sx;
            out[n].sy = corners[i].f.sy;
            out[n].ooz = corners[i].f.ooz;
            out[n].col = corners[i].f.col;
            n++;
        }
        prev = i;
    }
    /* out holds the cut's n vertices in order around it. n is CORNERS for a
     * triangle, and one more for a quad. a strip zigzags, so a quad sends
     * out[3] before out[2] */
    SEND_VERTEX(&out[0], KM_VERTEXPARAM_NORMAL)
    SEND_VERTEX(&out[1], KM_VERTEXPARAM_NORMAL)
    if (n != CORNERS) {
        SEND_VERTEX(&out[3], KM_VERTEXPARAM_NORMAL)
    }
    SEND_VERTEX(&out[2], KM_VERTEXPARAM_ENDOFSTRIP)
    return sq;
}
