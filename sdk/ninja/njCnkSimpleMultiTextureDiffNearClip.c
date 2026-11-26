#include <kamui.h>
#include <ninja.h>
#include <nj_cnk.h>
#include <nj_global.h>
#include <sg_xpt.h>

/* the Ninja route searches sdk/include alone, so the path starts there */
#include "../ninja/njCnkSimpleMultiNearClip.h"

/* a corner of the triangle being cut. a buffer entry holds no texture
 * coordinate, so the corner carries its own */
struct corner {
    NJS_CNK_SIMPLEMULTI_BUF *entry;
    Float u;
    Float v;
};

Uint32 *njCnkSimpleMultiTextureDiffNearClip(
    NJS_CNK_SIMPLEMULTI_BUF *v0, NJS_CNK_SIMPLEMULTI_BUF *v1,
    NJS_CNK_SIMPLEMULTI_BUF *v2, Uint32 *sq, Float tu0, Float tv0, Float tu1,
    Float tv1, Float tu2, Float tv2, Float a, Float r, Float g, Float b,
    Float ar, Float ag, Float ab)
{
    struct corner tri[CNK_TRI_CORNERS];
    struct clip_vertex strip[CNK_CLIP_VERTICES];
    /* @hack redundant: register on every local but tri and strip */
    register Float dist_x;
    register Float dist_y;
    register Float cx;
    register Float cy;
    register Float t;
    register Float x;
    register Float y;
    register Float z;
    register Float u;
    register Float v;
    register Float *q;
    register Int i;
    register Int prev;
    register Int n;

    if (_nj_control_3d_flag_ & NJD_CONTROL_3D_DISABLE_NEAR_CLIP) {
        return sq;
    }
    dist_x = _nj_screen_.dist * _nj_scr_aspect_x_;
    dist_y = _nj_screen_.dist * _nj_scr_aspect_y_;
    cx = _nj_screen_.cx;
    cy = _nj_screen_.cy;
    tri[0].entry = v0;
    tri[0].u = tu0;
    tri[0].v = tv0;
    tri[1].entry = v1;
    tri[1].u = tu1;
    tri[1].v = tv1;
    tri[2].entry = v2;
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
            x = tri[prev].entry->x +
                ((tri[i].entry->x - tri[prev].entry->x) * t);
            y = tri[prev].entry->y +
                ((tri[i].entry->y - tri[prev].entry->y) * t);
            /* @hack spelling: the fields stored as q steps up from sx, where
             * stores to strip[n]'s fields would do */
            q = &strip[n].sx;
            *q++ = cx + (x * dist_x);
            *q++ = cy + (y * dist_y);
            *q++ = CNK_NEAR_OOZ;
            *q++ = tri[prev].u + ((tri[i].u - tri[prev].u) * t);
            *q++ = tri[prev].v + ((tri[i].v - tri[prev].v) * t);
            if (!_nj_flat_model_flag_) {
                *q++ = (tri[prev].entry->a +
                        ((tri[i].entry->a - tri[prev].entry->a) * t)) *
                       a;
                *q++ = ((tri[prev].entry->r +
                         ((tri[i].entry->r - tri[prev].entry->r) * t)) *
                        r) +
                       ar;
                *q++ = ((tri[prev].entry->g +
                         ((tri[i].entry->g - tri[prev].entry->g) * t)) *
                        g) +
                       ag;
                *q++ = ((tri[prev].entry->b +
                         ((tri[i].entry->b - tri[prev].entry->b) * t)) *
                        b) +
                       ab;
                *q++ = tri[prev].entry->sr +
                       ((tri[i].entry->sr - tri[prev].entry->sr) * t);
                *q++ = tri[prev].entry->sg +
                       ((tri[i].entry->sg - tri[prev].entry->sg) * t);
                *q = tri[prev].entry->sb +
                     ((tri[i].entry->sb - tri[prev].entry->sb) * t);
            }
            n++;
        }
        if (tri[i].entry->z > CNK_NEAR_Z) {
            /* @hack spelling: the fields stored as q steps up from sx, where
             * stores to strip[n]'s fields would do */
            q = &strip[n].sx;
            *q++ = tri[i].entry->sx;
            *q++ = tri[i].entry->sy;
            *q++ = tri[i].entry->ooz;
            *q++ = tri[i].u;
            *q++ = tri[i].v;
            if (!_nj_flat_model_flag_) {
                *q++ = tri[i].entry->a * a;
                *q++ = (tri[i].entry->r * r) + ar;
                *q++ = (tri[i].entry->g * g) + ag;
                *q++ = (tri[i].entry->b * b) + ab;
                *q++ = tri[i].entry->sr;
                *q++ = tri[i].entry->sg;
                *q = tri[i].entry->sb;
            }
            /* @hack spelling: n = n + 1, where n++ would do */
            n = n + 1;
        }
    }
    /* flat shading needs a color only at strip[2] and strip[3], where the
     * strip's triangles end */
    if (_nj_flat_model_flag_) {
        /* @hack spelling: q walked from strip[2].sx to strip[2].a and along the
         * fields, where stores to strip[2]'s fields would do */
        q = &strip[2].sx;
        q += CLIP_COLOR;
        *q++ = tri[2].entry->a * a;
        *q++ = (tri[2].entry->r * r) + ar;
        /* @bug logic: adds ab, not ag, to strip[2]'s g (FMOV.S
         * @(R0,R15),FR3 at 0x8c16717c loads ab) */
        *q++ = (tri[2].entry->g * g) + ab;
        *q++ = (tri[2].entry->b * b) + ab;
        *q++ = tri[2].entry->sr;
        *q++ = tri[2].entry->sg;
        *q = tri[2].entry->sb;
        if (n != CNK_TRI_CORNERS) {
            /* @hack spelling: q walked from (Float *)&strip[3] to strip[3].a
             * and along the fields, where stores to each field would do */
            q = (Float *)&strip[3];
            q += CLIP_COLOR;
            *q++ = tri[2].entry->a * a;
            *q++ = (tri[2].entry->r * r) + ar;
            *q++ = (tri[2].entry->g * g) + ag;
            *q++ = (tri[2].entry->b * b) + ab;
            *q++ = tri[2].entry->sr;
            *q++ = tri[2].entry->sg;
            *q = tri[2].entry->sb;
        }
    }
    SEND_DIFF_VERTEX(&strip[0], KM_VERTEXPARAM_NORMAL)
    SEND_DIFF_VERTEX(&strip[1], KM_VERTEXPARAM_NORMAL)
    if (n != CNK_TRI_CORNERS) {
        SEND_DIFF_VERTEX(&strip[3], KM_VERTEXPARAM_NORMAL)
    }
    SEND_DIFF_VERTEX(&strip[2], KM_VERTEXPARAM_ENDOFSTRIP)
    _nj_draw_polygon_ += n;
    return sq;
}
