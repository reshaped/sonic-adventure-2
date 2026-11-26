#include <kamui.h>
#include <ninja.h>
#include <nj_cnk.h>
#include <nj_global.h>
#include <sg_xpt.h>

/* the Ninja route searches sdk/include alone, so the path starts there */
#include "../ninja/njCnkSimpleMultiNearClip.h"

Uint32 *njCnkSimpleMultiTextureDiffEnvNearClip(NJS_CNK_SIMPLEMULTI_BUF *v0,
                                               NJS_CNK_SIMPLEMULTI_BUF *v1,
                                               NJS_CNK_SIMPLEMULTI_BUF *v2,
                                               Uint32 *sq, Float a, Float r,
                                               Float g, Float b, Float ar,
                                               Float ag, Float ab)
{
    NJS_CNK_SIMPLEMULTI_BUF *tri[CNK_TRI_CORNERS];
    struct clip_vertex strip[CNK_CLIP_VERTICES];
    /* @hack redundant: register on every local but tri and strip */
    register Float uoff;
    register Float voff;
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
    if (_nj_control_3d_flag_ & NJD_CONTROL_3D_ENV_UV_SCROLL) {
        uoff = _nj_uv_scroll_u_ + 0.5F;
        voff = _nj_uv_scroll_v_ + 0.5F;
    } else {
        uoff = 0.5F;
        voff = 0.5F;
    }
    dist_x = _nj_screen_.dist * _nj_scr_aspect_x_;
    dist_y = _nj_screen_.dist * _nj_scr_aspect_y_;
    cx = _nj_screen_.cx;
    cy = _nj_screen_.cy;
    tri[0] = v0;
    tri[1] = v1;
    tri[2] = v2;
    n = 0;
    for (prev = CNK_TRI_CORNERS - 1, i = 0; i < CNK_TRI_CORNERS;
         prev = i, i++) {
        /* the strip takes a vertex where the edge from prev to i crosses a z
         * of 1. t places that vertex on the edge, from 0 at prev to 1 at i */
        /* @hack spelling: the two tests negated and XORed, where
         * (tri[prev]->z > CNK_NEAR_Z) != (tri[i]->z > CNK_NEAR_Z) would do */
        if (!(tri[prev]->z > CNK_NEAR_Z) ^ !(tri[i]->z > CNK_NEAR_Z)) {
            t = (CNK_NEAR_Z - tri[prev]->z) / (tri[i]->z - tri[prev]->z);
            x = tri[prev]->x + ((tri[i]->x - tri[prev]->x) * t);
            y = tri[prev]->y + ((tri[i]->y - tri[prev]->y) * t);
            /* @hack spelling: the fields stored as q steps up from sx, where
             * stores to strip[n]'s fields would do */
            q = &strip[n].sx;
            *q++ = cx + (x * dist_x);
            *q++ = cy + (y * dist_y);
            *q++ = CNK_NEAR_OOZ;
            *q++ =
                ((tri[prev]->nx + ((tri[i]->nx - tri[prev]->nx) * t)) * 0.5F) +
                uoff;
            *q++ =
                ((tri[prev]->ny + ((tri[i]->ny - tri[prev]->ny) * t)) * 0.5F) +
                voff;
            if (!_nj_flat_model_flag_) {
                *q++ = (tri[prev]->a + ((tri[i]->a - tri[prev]->a) * t)) * a;
                *q++ = ((tri[prev]->r + ((tri[i]->r - tri[prev]->r) * t)) * r) +
                       ar;
                *q++ = ((tri[prev]->g + ((tri[i]->g - tri[prev]->g) * t)) * g) +
                       ag;
                *q++ = ((tri[prev]->b + ((tri[i]->b - tri[prev]->b) * t)) * b) +
                       ab;
                *q++ = tri[prev]->sr + ((tri[i]->sr - tri[prev]->sr) * t);
                *q++ = tri[prev]->sg + ((tri[i]->sg - tri[prev]->sg) * t);
                *q = tri[prev]->sb + ((tri[i]->sb - tri[prev]->sb) * t);
            }
            n++;
        }
        if (tri[i]->z > CNK_NEAR_Z) {
            /* @hack spelling: the fields stored as q steps up from sx, where
             * stores to strip[n]'s fields would do */
            q = &strip[n].sx;
            *q++ = tri[i]->sx;
            *q++ = tri[i]->sy;
            *q++ = tri[i]->ooz;
            *q++ = (tri[i]->nx * 0.5F) + uoff;
            *q++ = (tri[i]->ny * 0.5F) + voff;
            if (!_nj_flat_model_flag_) {
                *q++ = tri[i]->a * a;
                *q++ = (tri[i]->r * r) + ar;
                *q++ = (tri[i]->g * g) + ag;
                *q++ = (tri[i]->b * b) + ab;
                *q++ = tri[i]->sr;
                *q++ = tri[i]->sg;
                *q = tri[i]->sb;
            }
            n++;
        }
    }
    /* flat shading needs a color only at strip[2] and strip[3], where the
     * strip's triangles end */
    if (_nj_flat_model_flag_) {
        /* @hack spelling: q walked from strip[2].sx to strip[2].a and along the
         * fields, where stores to strip[2]'s fields would do */
        q = &strip[2].sx;
        q += CLIP_COLOR;
        *q++ = tri[2]->a * a;
        *q++ = (tri[2]->r * r) + ar;
        /* @bug logic: adds ab, not ag, to strip[2]'s g (FMOV FR10,FR3
         * at 0x8c166c4c takes ab) */
        *q++ = (tri[2]->g * g) + ab;
        *q++ = (tri[2]->b * b) + ab;
        *q++ = tri[2]->sr;
        *q++ = tri[2]->sg;
        *q = tri[2]->sb;
        if (n != CNK_TRI_CORNERS) {
            /* @hack spelling: q walked from (Float *)&strip[3] to strip[3].a
             * and along the fields, where stores to each field would do */
            q = (Float *)&strip[3];
            q += CLIP_COLOR;
            *q++ = tri[2]->a * a;
            *q++ = (tri[2]->r * r) + ar;
            *q++ = (tri[2]->g * g) + ag;
            *q++ = (tri[2]->b * b) + ab;
            *q++ = tri[2]->sr;
            *q++ = tri[2]->sg;
            *q = tri[2]->sb;
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
