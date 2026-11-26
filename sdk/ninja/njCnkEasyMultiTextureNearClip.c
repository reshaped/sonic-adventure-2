#include <kamui.h>
#include <machine.h>
#include <ninja.h>
#include <nj_cnk.h>
#include <nj_global.h>
#include <sg_xpt.h>

/* a corner of the triangle being cut. an entry holds its point's z but not
 * its x and y, nor a texture coordinate. the corner carries them: x and y
 * worked back from the entry, and the caller's (u, v) */
struct corner {
    NJS_CNK_EASYMULTI_BUF *entry;
    Float x;
    Float y;
    Float u;
    Float v;
};
/* a vertex of the near clip's strip. SEND_VERTEX sends its sx, sy, ooz, u and
 * v as words 1 to 5. it sends a, r, g and b as words 8 to 11, the base color.
 * it sends r, g and b, each less BASE_MAX, as words 13 to 15 */
struct clip_vertex {
    Float sx;
    Float sy;
    Float ooz;
    Float u;
    Float v;
    Float r;
    Float g;
    Float b;
    Float a;
};
/* the Floats of struct clip_vertex before its color: sx, sy, ooz, u and v */
#define CLIP_COLOR 5
/* the most a channel of the base color shows. a vertex's offset color carries
 * any part of r, g or b above it */
#define BASE_MAX 1.0F

/* send vertex at sq, with param as its control word. sq is left past the
 * vertex. the macro overwrites the parameters a, ar, ag and ab */
#define SEND_VERTEX(vertex, param)                                             \
    {                                                                          \
        /* @hack spelling: vertex's fields read as q steps along it, where     \
         * reads of vertex's fields would do */                                \
        q = &(vertex)->sx;                                                     \
        /* @hack spelling: each field copied through a variable of the         \
         * function, where a local of the macro would do */                    \
        x = *q++;                                                              \
        y = *q++;                                                              \
        ooz = *q++;                                                            \
        u = *q++;                                                              \
        v = *q++;                                                              \
        /* @hack spelling: the words stored as sq steps down through each      \
         * block, where indexed stores from sq would do */                     \
        sq += CNK_UV_END;                                                      \
        *(Float *)--sq = v;                                                    \
        *(Float *)--sq = u;                                                    \
        *(Float *)--sq = ooz;                                                  \
        *(Float *)--sq = y;                                                    \
        *(Float *)--sq = x;                                                    \
        *--sq = (param);                                                       \
        prefetch(sq);                                                          \
        ar = *q++;                                                             \
        ag = *q++;                                                             \
        ab = *q++;                                                             \
        a = *q;                                                                \
        sq += CNK_VERTEX64_WORDS;                                              \
        *(Float *)--sq = ab - BASE_MAX;                                        \
        *(Float *)--sq = ag - BASE_MAX;                                        \
        *(Float *)--sq = ar - BASE_MAX;                                        \
        sq--;                                                                  \
        *(Float *)--sq = ab;                                                   \
        *(Float *)--sq = ag;                                                   \
        *(Float *)--sq = ar;                                                   \
        *(Float *)--sq = a;                                                    \
        prefetch(sq);                                                          \
        sq += CNK_BLOCK_WORDS;                                                 \
    }

Uint32 *njCnkEasyMultiTextureNearClip(NJS_CNK_EASYMULTI_BUF *v0,
                                      NJS_CNK_EASYMULTI_BUF *v1,
                                      NJS_CNK_EASYMULTI_BUF *v2, Uint32 *sq,
                                      Float tu0, Float tv0, Float tu1,
                                      Float tv1, Float tu2, Float tv2, Float a,
                                      Float ar, Float ag, Float ab)
{
    struct corner tri[CNK_TRI_CORNERS];
    struct clip_vertex strip[CNK_CLIP_VERTICES];
    /* @hack redundant: register on every local but tri and strip */
    register Float dist_x;
    register Float dist_y;
    register Float inv_dist_x;
    register Float inv_dist_y;
    register Float cx;
    register Float cy;
    register Float t;
    register Float x;
    register Float y;
    register Float ooz;
    register Float u;
    register Float v;
    register Float *q;
    register Int i;
    register Int prev;
    register Int n;

    if (_nj_control_3d_flag_ & NJD_CONTROL_3D_DISABLE_NEAR_CLIP) {
        return sq;
    }
    /* @hack spelling: cx set ahead of inv_dist_x and cy ahead of
     * inv_dist_y, not both after them */
    dist_x = _nj_screen_.dist * _nj_scr_aspect_x_;
    dist_y = _nj_screen_.dist * _nj_scr_aspect_y_;
    cx = _nj_screen_.cx;
    inv_dist_x = 1.0F / dist_x;
    cy = _nj_screen_.cy;
    inv_dist_y = 1.0F / dist_y;
    tri[0].entry = v0;
    tri[0].x = (v0->x - cx) * v0->z * inv_dist_x;
    tri[0].y = (v0->y - cy) * v0->z * inv_dist_y;
    tri[0].u = tu0;
    tri[0].v = tv0;
    tri[1].entry = v1;
    tri[1].x = (v1->x - cx) * v1->z * inv_dist_x;
    tri[1].y = (v1->y - cy) * v1->z * inv_dist_y;
    tri[1].u = tu1;
    tri[1].v = tv1;
    tri[2].entry = v2;
    tri[2].x = (v2->x - cx) * v2->z * inv_dist_x;
    tri[2].y = (v2->y - cy) * v2->z * inv_dist_y;
    tri[2].u = tu2;
    tri[2].v = tv2;
    n = 0;
    for (prev = CNK_TRI_CORNERS - 1, i = 0; i < CNK_TRI_CORNERS;
         prev = i, i++) {
        /* the strip takes a vertex where the edge from prev to i crosses a z
         * of 1. t places that vertex on the edge, from 0 at prev to 1 at i */
        /* @hack spelling: the two tests negated and XORed, where
         * (tri[prev].entry->z > CNK_NEAR_Z) != (tri[i].entry->z > CNK_NEAR_Z)
         * would do */
        if (!(tri[prev].entry->z > CNK_NEAR_Z) ^
            !(tri[i].entry->z > CNK_NEAR_Z)) {
            t = (CNK_NEAR_Z - tri[prev].entry->z) /
                (tri[i].entry->z - tri[prev].entry->z);
            x = tri[prev].x + ((tri[i].x - tri[prev].x) * t);
            y = tri[prev].y + ((tri[i].y - tri[prev].y) * t);
            /* @hack spelling: the fields stored as q steps up from sx, where
             * stores to strip[n]'s fields would do */
            q = &strip[n].sx;
            *q++ = cx + (x * dist_x);
            *q++ = cy + (y * dist_y);
            *q++ = CNK_NEAR_OOZ;
            *q++ = tri[prev].u + ((tri[i].u - tri[prev].u) * t);
            *q++ = tri[prev].v + ((tri[i].v - tri[prev].v) * t);
            if (!_nj_flat_model_flag_) {
                *q++ = tri[prev].entry->r +
                       ((tri[i].entry->r - tri[prev].entry->r) * t) + ar;
                *q++ = tri[prev].entry->g +
                       ((tri[i].entry->g - tri[prev].entry->g) * t) + ag;
                *q++ = tri[prev].entry->b +
                       ((tri[i].entry->b - tri[prev].entry->b) * t) + ab;
                *q = (tri[prev].entry->a +
                      ((tri[i].entry->a - tri[prev].entry->a) * t)) *
                     a;
            }
            n++;
        }
        if (tri[i].entry->z > CNK_NEAR_Z) {
            /* @hack spelling: the fields stored as q steps up from sx, where
             * stores to strip[n]'s fields would do */
            q = &strip[n].sx;
            *q++ = tri[i].entry->x;
            *q++ = tri[i].entry->y;
            *q++ = tri[i].entry->ooz;
            *q++ = tri[i].u;
            *q++ = tri[i].v;
            if (!_nj_flat_model_flag_) {
                *q++ = tri[i].entry->r + ar;
                *q++ = tri[i].entry->g + ag;
                *q++ = tri[i].entry->b + ab;
                *q = tri[i].entry->a * a;
            }
            n++;
        }
    }
    /* flat shading needs a color only at strip[2] and strip[3], where the
     * strip's triangles end */
    if (_nj_flat_model_flag_) {
        /* @hack spelling: q walked from strip[2].sx to strip[2].r and along
         * the fields, where stores to strip[2]'s fields would do */
        q = &strip[2].sx;
        q += CLIP_COLOR;
        *q++ = tri[2].entry->r + ar;
        *q++ = tri[2].entry->g + ag;
        *q++ = tri[2].entry->b + ab;
        *q = tri[2].entry->a * a;
        if (n != CNK_TRI_CORNERS) {
            /* @hack spelling: q walked from strip[3].sx to strip[3].r and
             * along the fields, where stores to strip[3]'s fields would do */
            q = &strip[3].sx;
            q += CLIP_COLOR;
            *q++ = tri[2].entry->r + ar;
            *q++ = tri[2].entry->g + ag;
            *q++ = tri[2].entry->b + ab;
            *q = tri[2].entry->a * a;
        }
    }
    SEND_VERTEX(&strip[0], KM_VERTEXPARAM_NORMAL)
    SEND_VERTEX(&strip[1], KM_VERTEXPARAM_NORMAL)
    if (n != CNK_TRI_CORNERS) {
        SEND_VERTEX(&strip[3], KM_VERTEXPARAM_NORMAL)
    }
    SEND_VERTEX(&strip[2], KM_VERTEXPARAM_ENDOFSTRIP)
    _nj_draw_polygon_ += n;
    return sq;
}
