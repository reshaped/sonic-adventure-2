#include <kamui.h>
#include <machine.h>
#include <ninja.h>
#include <nj_cnk.h>
#include <nj_global.h>
#include <sg_xpt.h>

/* a vertex of the near clip's strip. SEND_VERTEX sends its sx, sy and ooz as
 * words 1 to 3. it sends a, r, g and b as words 4 to 7 */
struct clip_vertex {
    Float sx;
    Float sy;
    Float ooz;
    Float r;
    Float g;
    Float b;
    Float a;
};
/* the Floats of struct clip_vertex before its color: sx, sy and ooz */
#define CLIP_COLOR 3

/* send vertex at sq, with param as its control word. sq is left past the
 * vertex. the macro overwrites the parameters a, r, g and b */
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
        r = *q++;                                                              \
        g = *q++;                                                              \
        b = *q++;                                                              \
        a = *q;                                                                \
        /* @hack spelling: the words stored as sq steps down through the       \
         * vertex, where indexed stores from sq would do */                    \
        sq += CNK_BLOCK_WORDS;                                                 \
        *(Float *)--sq = b;                                                    \
        *(Float *)--sq = g;                                                    \
        *(Float *)--sq = r;                                                    \
        *(Float *)--sq = a;                                                    \
        *(Float *)--sq = ooz;                                                  \
        *(Float *)--sq = y;                                                    \
        *(Float *)--sq = x;                                                    \
        *--sq = (param);                                                       \
        prefetch(sq);                                                          \
        sq += CNK_BLOCK_WORDS;                                                 \
    }

Uint32 *njCnkSimpleMultiPolygonNearClip(NJS_CNK_SIMPLEMULTI_BUF *v0,
                                        NJS_CNK_SIMPLEMULTI_BUF *v1,
                                        NJS_CNK_SIMPLEMULTI_BUF *v2, Uint32 *sq,
                                        Float a, Float r, Float g, Float b,
                                        Float ar, Float ag, Float ab)
{
    NJS_CNK_SIMPLEMULTI_BUF *tri[CNK_TRI_CORNERS];
    struct clip_vertex strip[CNK_CLIP_VERTICES];
    /* @hack redundant: register on every local but tri and strip */
    register Float dist_x;
    register Float dist_y;
    register Float cx;
    register Float cy;
    register Float t;
    register Float x;
    register Float y;
    register Float ooz;
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
            if (!_nj_flat_model_flag_) {
                *q++ = ((tri[prev]->r + ((tri[i]->r - tri[prev]->r) * t)) * r) +
                       ar;
                *q++ = ((tri[prev]->g + ((tri[i]->g - tri[prev]->g) * t)) * g) +
                       ag;
                *q++ = ((tri[prev]->b + ((tri[i]->b - tri[prev]->b) * t)) * b) +
                       ab;
                *q = (tri[prev]->a + ((tri[i]->a - tri[prev]->a) * t)) * a;
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
            if (!_nj_flat_model_flag_) {
                *q++ = (tri[i]->r * r) + ar;
                *q++ = (tri[i]->g * g) + ag;
                *q++ = (tri[i]->b * b) + ab;
                *q = tri[i]->a * a;
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
        *q++ = (tri[2]->r * r) + ar;
        *q++ = (tri[2]->g * g) + ag;
        *q++ = (tri[2]->b * b) + ab;
        *q = tri[2]->a * a;
        if (n != CNK_TRI_CORNERS) {
            /* @hack spelling: q walked from strip[3].sx to strip[3].r and
             * along the fields, where stores to strip[3]'s fields would do */
            q = &strip[3].sx;
            q += CLIP_COLOR;
            *q++ = (tri[2]->r * r) + ar;
            *q++ = (tri[2]->g * g) + ag;
            *q++ = (tri[2]->b * b) + ab;
            *q = tri[2]->a * a;
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
