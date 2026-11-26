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
 * the block's start. word 7, the offset color, stays unwritten */
/* @hack spelling: a vertex's words stored as sq steps back from the block's
 * end, where indexed stores from its start would do */
#define PUT_VERTEX(col, v, u, ooz, sy, sx, param)                              \
    {                                                                          \
        *--sq = (col);                                                         \
        *(Float *)--sq = (v);                                                  \
        *(Float *)--sq = (u);                                                  \
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
    (sizeof(struct draw3d_tex_vertex) / sizeof(union wire_word))
/* a vertex of the polygon whose edges njTri_Wired_With_Tex sends. words
 * views the same record */
union wire_vertex {
    union wire_word words[WIRE_VERTEX_WORDS];
    struct draw3d_tex_vertex v;
};

/* the member exports this function, though no header of the SDK declares
 * it */
/* NOLINTBEGIN(misc-use-internal-linkage) */
/* njDPolygonLineNearClip3DP for an edge of textured vertices. the end that
 * moves onto the near clip takes its color, u and v from along the edge */
Uint32 *njDTextureLineNearClip3DP(struct draw3d_tex_vertex *a,
                                  struct draw3d_tex_vertex *b, Uint32 *sq);
/* NOLINTEND(misc-use-internal-linkage) */

Uint32 *njTri_Wired_With_Tex(void *buf, Int count, Uint32 *sq, Sint32 clip)
{
    Float inv_near;
    union wire_vertex *a;
    union wire_vertex *b;
    union wire_word *p;
    union wire_word *q;
    Uint32 a_col;
    Uint32 b_col;
    Float a_v;
    Float a_u;
    Float a_ooz;
    Float a_sy;
    Float a_sx;
    Float b_v;
    Float b_u;
    Float b_ooz;
    Float b_sy;
    Float b_sx;

    /* @hack spelling: the vertices read through word cursors, where reads of
     * each vertex's fields would do */
    p = ((union wire_vertex *)buf)[count - 1].words;
    inv_near = 1.0F / _nj_near_clip_;
    a = (union wire_vertex *)p;
    a_col = p++->bits;
    a_v = p++->value;
    a_u = p++->value;
    a_ooz = p++->value;
    a_sy = p++->value;
    a_sx = p->value;
    q = ((union wire_vertex *)buf)->words;
    do {
        /* @hack redundant: q set to the words of b, which q already points
         * to */
        q = (b = (union wire_vertex *)q)->words;
        b_col = q++->bits;
        b_v = q++->value;
        b_u = q++->value;
        b_ooz = q++->value;
        b_sy = q++->value;
        b_sx = q++->value;
        if (!(b_ooz > 0.0F && b_ooz < inv_near && a_ooz > 0.0F &&
              a_ooz < inv_near)) {
            if ((a_ooz > 0.0F && a_ooz < inv_near) ||
                (b_ooz > 0.0F && b_ooz < inv_near)) {
                sq = njDTextureLineNearClip3DP(&a->v, &b->v, sq);
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
            /* @hack spelling: wide_y worked out before wide_x, where either
             * order would do */
            wide_y = side_y + side_y;
            wide_x = side_x + side_x;
            sq += SQ_WORDS - 1;
            PUT_VERTEX(a_col, a_v, a_u, a_ooz, a_sy - side_y, a_sx - side_x,
                       KM_VERTEXPARAM_NORMAL)
            sq += (2 * SQ_WORDS) - 1;
            PUT_VERTEX(b_col, b_v, b_u, b_ooz, b_sy - side_y, b_sx - side_x,
                       KM_VERTEXPARAM_NORMAL)
            sq += (2 * SQ_WORDS) - 1;
            PUT_VERTEX(a_col, a_v, a_u, a_ooz, a_sy + wide_y, a_sx + wide_x,
                       KM_VERTEXPARAM_NORMAL)
            sq += (2 * SQ_WORDS) - 1;
            PUT_VERTEX(b_col, b_v, b_u, b_ooz, b_sy + wide_y, b_sx + wide_x,
                       KM_VERTEXPARAM_ENDOFSTRIP)
            sq += SQ_WORDS;
        }
        a = b;
        a_col = b_col;
        a_v = b_v;
        a_u = b_u;
        a_ooz = b_ooz;
        a_sy = b_sy;
        a_sx = b_sx;
    } while (--count);
    return sq;
}

Uint32 *njDTextureLineNearClip3DP(struct draw3d_tex_vertex *a,
                                  struct draw3d_tex_vertex *b, Uint32 *sq)
{
    Float scale_x;
    Float scale_y;
    Float cx;
    Float cy;
    Float inv_x;
    Float inv_y;
    Float near;
    Float z1;
    Float z2;
    Float t;
    Float x;
    Float y;
    Float out_y;
    Float out_x;
    Float in_y;
    Float in_x;
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
    Float out_v;
    Float out_u;
    Float in_v;
    Float in_u;
    /* @hack redundant: register on the twelve color values */
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
    Uint32 moved_col;

    scale_x = _nj_screen_.dist * _nj_scr_aspect_x_;
    scale_y = _nj_screen_.dist * _nj_scr_aspect_y_;
    inv_x = 1.0F / scale_x;
    inv_y = 1.0F / scale_y;
    cx = _nj_screen_.cx;
    cy = _nj_screen_.cy;
    z1 = 1.0F / a->ooz;
    z2 = 1.0F / b->ooz;
    near = _nj_near_clip_;
    /* out is the end nearer than _nj_near_clip_, and in the other. each end's
     * x and y come back from its place on the screen and its z */
    /* NOLINTBEGIN(clang-diagnostic-sometimes-uninitialized) */
    if (z1 < near) {
        out_col = a->col;
        /* @hack redundant: z1 and z2 worked out again */
        z1 = 1.0F / a->ooz;
        out_y = (a->sy - cy) * z1 * inv_y;
        out_x = (a->sx - cx) * z1 * inv_x;
        out_v = a->v;
        out_u = a->u;
        /* @hack redundant: copies of the out end's ooz and place, which the
         * move onto the near clip overwrites */
        out_ooz = a->ooz;
        out_sy = a->sy;
        out_sx = a->sx;
        in_col = b->col;
        z2 = 1.0F / b->ooz;
        in_y = (b->sy - cy) * z2 * inv_y;
        in_x = (b->sx - cx) * z2 * inv_x;
        in_v = b->v;
        in_u = b->u;
        in_ooz = b->ooz;
        in_sy = b->sy;
        in_sx = b->sx;
    } else if (z2 < near) {
        out_col = b->col;
        z1 = 1.0F / b->ooz;
        out_y = (b->sy - cy) * z1 * inv_y;
        out_x = (b->sx - cx) * z1 * inv_x;
        out_v = b->v;
        out_u = b->u;
        /* @hack redundant: the same copies in this arm */
        out_ooz = b->ooz;
        out_sy = b->sy;
        out_sx = b->sx;
        in_col = a->col;
        z2 = 1.0F / a->ooz;
        in_y = (a->sy - cy) * z2 * inv_y;
        in_x = (a->sx - cx) * z2 * inv_x;
        in_v = a->v;
        in_u = a->u;
        in_ooz = a->ooz;
        in_sy = a->sy;
        in_sx = a->sx;
    }
    /* NOLINTEND(clang-diagnostic-sometimes-uninitialized) */
#ifdef NONMATCHING
    /* this side repairs the read of unset values. it sends nothing where
     * neither end's z lies below _nj_near_clip_ */
    if (!(z1 < near)) {
        return sq;
    }
#endif
    /* @bug uninitialized: where neither end's z lies below _nj_near_clip_, the
     * move and the quad read values no store on that path sets. the branch at
     * 0x8c13db1e, on the second test, skips both arms for the move at
     * 0x8c13dbaa. njTri_Wired_With_Tex reaches this with an end whose z is
     * _nj_near_clip_ exactly, or whose ooz is 0 */
    /* t is how far along the edge, from out to in, the near clip lies */
    t = (near - z1) / (z2 - z1);
    out_v = out_v + ((in_v - out_v) * t);
    out_u = out_u + ((in_u - out_u) * t);
    y = out_y + ((in_y - out_y) * t);
    x = out_x + ((in_x - out_x) * t);
    out_ooz = 1.0F / near;
    out_sy = (scale_y * y * out_ooz) + cy;
    out_sx = (scale_x * x * out_ooz) + cx;
    out_a = (out_col >> ALPHA_SHIFT) & CHANNEL_MASK;
    out_r = (out_col >> RED_SHIFT) & CHANNEL_MASK;
    out_g = (out_col >> GREEN_SHIFT) & CHANNEL_MASK;
    out_b = out_col & CHANNEL_MASK;
    in_a = (in_col >> ALPHA_SHIFT) & CHANNEL_MASK;
    in_r = (in_col >> RED_SHIFT) & CHANNEL_MASK;
    in_g = (in_col >> GREEN_SHIFT) & CHANNEL_MASK;
    in_b = in_col & CHANNEL_MASK;
    ca = out_a + ((in_a - out_a) * t);
    cr = out_r + ((in_r - out_r) * t);
    cg = out_g + ((in_g - out_g) * t);
    cb = out_b + ((in_b - out_b) * t);
    moved_col = ((Uint32)ca << ALPHA_SHIFT) | ((Uint32)cr << RED_SHIFT) |
                ((Uint32)cg << GREEN_SHIFT) | (Uint32)cb;
    dx = out_sx - in_sx;
    dy = out_sy - in_sy;
    inv_length = 1.0F / _builtin_sqrtf((dx * dx) + (dy * dy));
    side_x = dy * inv_length * LINE_SIDE;
    side_y = -(dx * inv_length * LINE_SIDE);
    wide_x = side_x + side_x;
    wide_y = side_y + side_y;
    sq += SQ_WORDS - 1;
    PUT_VERTEX(moved_col, out_v, out_u, out_ooz, out_sy - side_y,
               out_sx - side_x, KM_VERTEXPARAM_NORMAL)
    sq += (2 * SQ_WORDS) - 1;
    PUT_VERTEX(in_col, in_v, in_u, in_ooz, in_sy - side_y, in_sx - side_x,
               KM_VERTEXPARAM_NORMAL)
    sq += (2 * SQ_WORDS) - 1;
    PUT_VERTEX(moved_col, out_v, out_u, out_ooz, out_sy + wide_y,
               out_sx + wide_x, KM_VERTEXPARAM_NORMAL)
    sq += (2 * SQ_WORDS) - 1;
    PUT_VERTEX(in_col, in_v, in_u, in_ooz, in_sy + wide_y, in_sx + wide_x,
               KM_VERTEXPARAM_ENDOFSTRIP)
    sq += SQ_WORDS;
    return sq;
}
