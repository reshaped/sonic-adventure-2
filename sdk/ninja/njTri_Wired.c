#include <kamui.h>
#include <machine.h>
#include <ninja.h>
#include <nj_draw3d.h>
#include <nj_km.h>
#include <sg_xpt.h>

/* a line's quad reaches LINE_SIDE pixels to one side of the line. it reaches
 * twice that to the other */
#define LINE_SIDE 0.525F
/* put a vertex's words in the block whose word 7 sq points at. sq is left at
 * the block's start. words 4 and 5, u and v, stay unwritten. so does word 7,
 * the offset color */
/* @hack spelling: a vertex's words stored as sq steps back from the block's
 * end, where indexed stores from its start would do */
#define PUT_VERTEX(col, ooz, sy, sx, param)                                    \
    {                                                                          \
        *--sq = (col);                                                         \
        sq -= UV_WORDS;                                                        \
        *(Float *)--sq = (ooz);                                                \
        *(Float *)--sq = (sy);                                                 \
        *(Float *)--sq = (sx);                                                 \
        *--sq = (param);                                                       \
        prefetch(sq);                                                          \
    }

/* a word of a projected vertex, as its bits or as a float */
union wire_word {
    Uint32 bits;
    Float value;
};

#define WIRE_VERTEX_WORDS                                                      \
    (sizeof(struct draw3d_vertex) / sizeof(union wire_word))
/* a vertex of the polygon whose edges njTri_Wired sends. words views the same
 * record */
union wire_vertex {
    union wire_word words[WIRE_VERTEX_WORDS];
    struct draw3d_vertex v;
};

/* the member exports this function, though no header of the SDK declares
 * it */
/* NOLINTBEGIN(misc-use-internal-linkage) */
/* send the edge from a to b as a quad, as njTri_Wired sends one, from sq on.
 * the call returns the address after the quad. one end's z, 1 over its ooz,
 * must lie below _nj_near_clip_. that end moves along the edge onto the near
 * clip, and takes its color from along the edge. the quad runs from the moved
 * end to the other. so the edges a to b and b to a send the same quad */
Uint32 *njDPolygonLineNearClip3DP(struct draw3d_vertex *a,
                                  struct draw3d_vertex *b, Uint32 *sq);
/* NOLINTEND(misc-use-internal-linkage) */

Uint32 *njTri_Wired(void *buf, Int count, Uint32 *sq, Sint32 clip)
{
    Float inv_near;
    union wire_vertex *a;
    union wire_vertex *b;
    union wire_word *p;
    union wire_word *q;
    Uint32 a_col;
    Uint32 b_col;
    Float a_ooz;
    Float a_sy;
    Float a_sx;
    Float b_ooz;
    Float b_sy;
    Float b_sx;

    /* @hack spelling: the vertices read through word cursors, where reads of
     * each vertex's fields would do */
    p = ((union wire_vertex *)buf)[count - 1].words;
    a = (union wire_vertex *)p;
    inv_near = 1.0F / _nj_near_clip_;
    a_col = p++->bits;
    a_ooz = p++->value;
    a_sy = p++->value;
    a_sx = p->value;
    /* @hack redundant: a store to a_ooz that never runs */
    if (0) {
        a_ooz = 0.0F;
    }
    q = ((union wire_vertex *)buf)->words;
    do {
        b = (union wire_vertex *)q;
        b_col = q++->bits;
        b_ooz = q++->value;
        b_sy = q++->value;
        b_sx = q++->value;
        if (!(b_ooz > 0.0F && b_ooz < inv_near && a_ooz > 0.0F &&
              a_ooz < inv_near)) {
            if ((a_ooz > 0.0F && a_ooz < inv_near) ||
                (b_ooz > 0.0F && b_ooz < inv_near)) {
                sq = njDPolygonLineNearClip3DP(&a->v, &b->v, sq);
            }
        } else {
            Float dx;
            Float dy;
            Float inv_length;
            Float side_x;
            Float side_y;
            Float wide_x;
            Float wide_y;

            dx = a_sx - b_sx;
            dy = a_sy - b_sy;
            inv_length = 1.0F / _builtin_sqrtf((dx * dx) + (dy * dy));
            side_x = dy * inv_length * LINE_SIDE;
            side_y = -(dx * inv_length * LINE_SIDE);
            wide_x = side_x + side_x;
            wide_y = side_y + side_y;
            sq += SQ_WORDS - 1;
            PUT_VERTEX(a_col, a_ooz, a_sy - side_y, a_sx - side_x,
                       KM_VERTEXPARAM_NORMAL)
            sq += (2 * SQ_WORDS) - 1;
            PUT_VERTEX(b_col, b_ooz, b_sy - side_y, b_sx - side_x,
                       KM_VERTEXPARAM_NORMAL)
            sq += (2 * SQ_WORDS) - 1;
            PUT_VERTEX(a_col, a_ooz, a_sy + wide_y, a_sx + wide_x,
                       KM_VERTEXPARAM_NORMAL)
            sq += (2 * SQ_WORDS) - 1;
            PUT_VERTEX(b_col, b_ooz, b_sy + wide_y, b_sx + wide_x,
                       KM_VERTEXPARAM_ENDOFSTRIP)
            sq += SQ_WORDS;
        }
        a = b;
        a_col = b_col;
        a_ooz = b_ooz;
        a_sy = b_sy;
        a_sx = b_sx;
    } while (--count);
    return sq;
}

Uint32 *njDPolygonLineNearClip3DP(struct draw3d_vertex *a,
                                  struct draw3d_vertex *b, Uint32 *sq)
{
    /* @hack redundant: register on scale_x, scale_y, cx, cy, inv_x,
     * inv_y, t, x, y and the twelve color values */
    register Float scale_x;
    register Float scale_y;
    register Float cx;
    register Float cy;
    register Float inv_x;
    register Float inv_y;
    Float z1;
    Float z2;
    register Float t;
    register Float x;
    register Float y;
    Float out_y;
    Float out_x;
    Float in_y;
    Float in_x;
    /* @hack spelling: the edge's terms declared before its ends' values,
     * where any order would do */
    Float dx;
    Float dy;
    Float inv_length;
    Float side_x;
    Float side_y;
    Float wide_x;
    Float wide_y;
    Float out_ooz;
    Float out_sy;
    Float out_sx;
    Float in_ooz;
    Float in_sy;
    Float in_sx;
    register Float out_a;
    register Float out_r;
    register Float out_g;
    register Float out_b;
    register Float in_a;
    register Float in_r;
    register Float in_g;
    register Float in_b;
    register Float ca;
    register Float cr;
    register Float cg;
    register Float cb;
    Uint32 out_col;
    Uint32 in_col;
    Int clipped;

    clipped = 0;
    scale_x = _nj_screen_.dist * _nj_scr_aspect_x_;
    scale_y = _nj_screen_.dist * _nj_scr_aspect_y_;
    inv_x = 1.0F / scale_x;
    inv_y = 1.0F / scale_y;
    cx = _nj_screen_.cx;
    cy = _nj_screen_.cy;
    z1 = 1.0F / a->ooz;
    z2 = 1.0F / b->ooz;
    /* out is the end nearer than _nj_near_clip_, and in the other. each end's
     * x and y come back from its place on the screen and its z */
    /* NOLINTBEGIN(clang-diagnostic-sometimes-uninitialized) */
    if (z1 < _nj_near_clip_) {
        /* @hack redundant: z1 and z2 worked out again */
        z1 = 1.0F / a->ooz;
        out_y = (a->sy - cy) * z1 * inv_y;
        out_x = (a->sx - cx) * z1 * inv_x;
        out_col = a->col;
        /* @hack redundant: copies of the out end's ooz and place, which the
         * move onto the near clip overwrites */
        out_ooz = a->ooz;
        out_sy = a->sy;
        out_sx = a->sx;
        in_col = b->col;
        z2 = 1.0F / b->ooz;
        in_y = (b->sy - cy) * z2 * inv_y;
        in_x = (b->sx - cx) * z2 * inv_x;
        in_ooz = b->ooz;
        in_sy = b->sy;
        in_sx = b->sx;
        clipped = 1;
    } else if (z2 < _nj_near_clip_) {
        z1 = 1.0F / b->ooz;
        out_y = (b->sy - cy) * z1 * inv_y;
        out_x = (b->sx - cx) * z1 * inv_x;
        out_col = b->col;
        /* @hack redundant: the same copies in this arm */
        out_ooz = b->ooz;
        out_sy = b->sy;
        out_sx = b->sx;
        in_col = a->col;
        z2 = 1.0F / a->ooz;
        in_y = (a->sy - cy) * z2 * inv_y;
        in_x = (a->sx - cx) * z2 * inv_x;
        in_ooz = a->ooz;
        in_sy = a->sy;
        in_sx = a->sx;
        clipped = 1;
    }
    /* NOLINTEND(clang-diagnostic-sometimes-uninitialized) */
#ifdef NONMATCHING
    /* this side repairs the read of unset values. it sends nothing where
     * neither end's z lies below _nj_near_clip_ */
    if (!clipped) {
        return sq;
    }
#endif
    if (clipped) {
        /* t is how far along the edge, from out to in, the near clip lies */
        t = (_nj_near_clip_ - z1) / (z2 - z1);
        y = out_y + (t * (in_y - out_y));
        x = out_x + (t * (in_x - out_x));
        out_ooz = 1.0F / _nj_near_clip_;
        out_sy = (scale_y * y * out_ooz) + cy;
        out_sx = (scale_x * x * out_ooz) + cx;
        out_a = (Float)((out_col >> ALPHA_SHIFT) & CHANNEL_MASK);
        out_r = (Float)((out_col >> RED_SHIFT) & CHANNEL_MASK);
        out_g = (Float)((out_col >> GREEN_SHIFT) & CHANNEL_MASK);
        out_b = (Float)(out_col & CHANNEL_MASK);
        in_a = (Float)((in_col >> ALPHA_SHIFT) & CHANNEL_MASK);
        in_r = (Float)((in_col >> RED_SHIFT) & CHANNEL_MASK);
        in_g = (Float)((in_col >> GREEN_SHIFT) & CHANNEL_MASK);
        in_b = (Float)(in_col & CHANNEL_MASK);
        ca = out_a + (t * (in_a - out_a));
        cr = out_r + (t * (in_r - out_r));
        cg = out_g + (t * (in_g - out_g));
        cb = out_b + (t * (in_b - out_b));
        out_col = ((Uint32)ca << ALPHA_SHIFT) | ((Uint32)cr << RED_SHIFT) |
                  ((Uint32)cg << GREEN_SHIFT) | (Uint32)cb;
    }
    /* @bug uninitialized: where neither end's z lies below _nj_near_clip_,
     * the quad's ends and colors are never set. clipped stays 0 from
     * 0x8c13d53e. the branch at 0x8c13d5e0 on the second test skips both
     * arms, and the branch at 0x8c13d658 skips the move. the quad then reads
     * the frame at 0x8c13d7f2. njTri_Wired reaches this with an end whose z
     * is _nj_near_clip_ exactly, or whose ooz is 0 */
    dx = out_sx - in_sx;
    dy = out_sy - in_sy;
    inv_length = 1.0F / _builtin_sqrtf((dx * dx) + (dy * dy));
    side_x = dy * inv_length * LINE_SIDE;
    side_y = -(dx * inv_length * LINE_SIDE);
    wide_x = side_x + side_x;
    wide_y = side_y + side_y;
    sq += SQ_WORDS - 1;
    PUT_VERTEX(out_col, out_ooz, out_sy - side_y, out_sx - side_x,
               KM_VERTEXPARAM_NORMAL)
    sq += (2 * SQ_WORDS) - 1;
    PUT_VERTEX(in_col, in_ooz, in_sy - side_y, in_sx - side_x,
               KM_VERTEXPARAM_NORMAL)
    sq += (2 * SQ_WORDS) - 1;
    PUT_VERTEX(out_col, out_ooz, out_sy + wide_y, out_sx + wide_x,
               KM_VERTEXPARAM_NORMAL)
    sq += (2 * SQ_WORDS) - 1;
    PUT_VERTEX(in_col, in_ooz, in_sy + wide_y, in_sx + wide_x,
               KM_VERTEXPARAM_ENDOFSTRIP)
    sq += SQ_WORDS;
    return sq;
}
