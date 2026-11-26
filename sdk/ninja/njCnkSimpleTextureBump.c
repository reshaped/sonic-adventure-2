#include <kamui.h>
#include <machine.h>
#include <ninja.h>
#include <nj_cnk.h>
#include <nj_direct.h>
#include <nj_global.h>
#include <sg_xpt.h>

/* the member exports this function, though no header of the SDK declares
 * it */
/* NOLINTBEGIN(misc-use-internal-linkage) */
/* njCnkSimpleTextureBump serves bump-mapped NJD_CS_UVN and NJD_CS_UVH strips.
 * it drops each triangle that faces away, as njCnkSimpleTexture does, and
 * never reads ambient. every vertex goes with the same base and offset colors.
 * the base color is _nj_cnk_simple_bump_lt_color_. the offset color is what
 * njCnkSimpleBumpLight gives for the simple light's direction, turned into the
 * bump texture's space */
Uint32 *njCnkSimpleTextureBump(Sint16 *strips, NJS_CNK_SIMPLE_BUF *buf,
                               Uint32 nbstrip, Uint32 *sq, Float ambient,
                               Float uvscale);
/* NOLINTEND(misc-use-internal-linkage) */

/* send the entry whose x p points to as a vertex at sq. param is its control
 * word, and (u, v) its texture coordinate. sq is left past the vertex */
#define SEND_VERTEX(u, v, param)                                               \
    {                                                                          \
        /* @hack spelling: the fields read as p steps along the entry, where   \
         * p[0], p[1] and p[2] would do */                                     \
        x = *p++;                                                              \
        y = *p++;                                                              \
        ooz = *p;                                                              \
        /* @hack spelling: the words stored as sq steps down through the       \
         * vertex, where indexed stores from sq would do */                    \
        sq += CNK_BLOCK_WORDS;                                                 \
        *--sq = offset_color;                                                  \
        *--sq = base_color;                                                    \
        *(Float *)--sq = (v);                                                  \
        *(Float *)--sq = (u);                                                  \
        *(Float *)--sq = ooz;                                                  \
        *(Float *)--sq = y;                                                    \
        *(Float *)--sq = x;                                                    \
        *--sq = (param);                                                       \
        prefetch(sq);                                                          \
        sq += CNK_BLOCK_WORDS;                                                 \
    }

Uint32 *njCnkSimpleTextureBump(Sint16 *strips, NJS_CNK_SIMPLE_BUF *buf,
                               Uint32 nbstrip, Uint32 *sq, Float ambient,
                               Float uvscale)
{
    NJS_VECTOR light;
    /* @hack type: normal as three floats, where an NJS_VECTOR would do */
    Float normal[3];
    Float *s;
    Float *t;
    NJS_MATRIX frame;
    NJS_MATRIX view;
    /* @hack spelling: mp, a void pointer to view for st_ext, where view cast
     * in the call would do */
    void *mp;
    Uint32 base_color;
    Uint32 offset_color;
    /* @hack spelling: light written through lp, where light written in place
     * would do */
    NJS_VECTOR *lp;
    /* @hack redundant: register on every local from here on */
    register Sint32 user_halfwords;
    register Sint32 drawn;
    register Float *p;
    register NJS_CNK_SIMPLE_BUF *v0;
    register NJS_CNK_SIMPLE_BUF *v1;
    register NJS_CNK_SIMPLE_BUF *v2;
    register Sint32 len;
    register Sint32 calculated;
    register Sint32 sent;
    register Float model_side;
    register Float tv2;
    register Float side;
    register Float facing;
    register Float tu0;
    register Float tv0;
    register Float tu1;
    register Float tv1;
    register Float tu2;
    register Float x;
    register Float y;
    register Float ooz;

    /* s and t are the bump texture's two vectors. normal is their cross
     * product. frame takes the x axis to normal, y to -s and z to t. view is
     * the current matrix, which st_ext stores from XMTRX. njMultiMatrix puts
     * view before frame. light is the simple light's direction turned by the
     * transpose of the new frame, at unit length */
    s = _nj_cnk_simple_bump_tex_vector_;
    t = _nj_cnk_simple_bump_tex_vector_ + BUMP_T;
    mp = view;
    st_ext(mp);
    normal[0] = (s[1] * t[2]) - (s[2] * t[1]);
    normal[1] = (s[2] * t[0]) - (s[0] * t[2]);
    normal[2] = (s[0] * t[1]) - (s[1] * t[0]);
    frame[0] = normal[0];
    frame[1] = normal[1];
    frame[2] = normal[2];
    frame[3] = 0.0F;
    frame[4] = -s[0];
    frame[5] = -s[1];
    frame[6] = -s[2];
    frame[7] = 0.0F;
    frame[8] = t[0];
    frame[9] = t[1];
    frame[10] = t[2];
    frame[11] = 0.0F;
    /* @hack spelling: frame[12] to frame[14] zeroed in one chained
     * assignment, where a statement for each would do */
    frame[14] = frame[13] = frame[12] = 0.0F;
    frame[15] = 1.0F;
    njMultiMatrix(&frame, &view);
    lp = &light;
    lp->x = (frame[0] * _nj_vertex_simple.light.x) +
            (frame[1] * _nj_vertex_simple.light.y) +
            (frame[2] * _nj_vertex_simple.light.z);
    lp->y = (frame[4] * _nj_vertex_simple.light.x) +
            (frame[5] * _nj_vertex_simple.light.y) +
            (frame[6] * _nj_vertex_simple.light.z);
    lp->z = (frame[8] * _nj_vertex_simple.light.x) +
            (frame[9] * _nj_vertex_simple.light.y) +
            (frame[10] * _nj_vertex_simple.light.z);
    njUnitVector(lp);
    base_color = _nj_cnk_simple_bump_lt_color_;
    offset_color = njCnkSimpleBumpLight(lp);

    prefetch(strips + CNK_STRIP_AHEAD);
    user_halfwords = (nbstrip >> NJD_UFO_SHIFT) & STRIP_USER;
    nbstrip &= STRIP_COUNT;
    drawn = 0;
    calculated = 0;
    model_side = 1.0F;
    if (_nj_control_3d_flag_ & NJD_CONTROL_3D_MIRROR_MODEL) {
        model_side = -1.0F;
    }
    do {
        len = *strips++;
        /* @hack spelling: the assignment to v0 inside prefetch's argument,
         * where a statement of its own would do */
        prefetch(v0 = &buf[*strips++]);
        side = model_side;
        if (len < 0) {
            len = -len;
            side = -side;
        }
        calculated += len;
        len -= CNK_STRIP_LEAD;
        sent = 0;
        tu0 = *strips++ * uvscale;
        tv0 = *strips++ * uvscale;
        v1 = &buf[*strips++];
        prefetch(v1);
        tu1 = *strips++ * uvscale;
        tv1 = *strips++ * uvscale;
        do {
            v2 = &buf[*strips++];
            prefetch(v2);
            tu2 = *strips++ * uvscale;
            tv2 = *strips++ * uvscale;
            /* @hack spelling: the winding written as (v0 - v1) x (v2 - v1)
             * with its two products swapped, where (v1 - v0) x (v2 - v1)
             * would do */
            facing = (((v0->y - v1->y) * (v2->x - v1->x)) -
                      ((v0->x - v1->x) * (v2->y - v1->y))) *
                     side;
            if (facing < 0.0F) {
                if (sent == 0) {
                    p = &v0->x;
                    SEND_VERTEX(tu0, tv0, KM_VERTEXPARAM_NORMAL)
                    sent++;
                }
                p = &v1->x;
                /* @hack redundant: the step of p under if (0) */
                if (0) {
                    p++;
                }
                SEND_VERTEX(tu1, tv1, KM_VERTEXPARAM_NORMAL)
                sent++;
            } else if (sent != 0) {
                p = &v1->x;
                SEND_VERTEX(tu1, tv1, KM_VERTEXPARAM_ENDOFSTRIP)
                sent++;
                drawn += sent;
                sent = 0;
            }
            strips += user_halfwords;
            prefetch(strips + CNK_STRIP_AHEAD);
            /* @hack spelling: side negated among the moves, where after them
             * would do */
            v0 = v1;
            tu0 = tu1;
            v1 = v2;
            side = -side;
            tv0 = tv1;
            tu1 = tu2;
            tv1 = tv2;
        } while (--len);
        if (sent != 0) {
            p = &v2->x;
            SEND_VERTEX(tu2, tv2, KM_VERTEXPARAM_ENDOFSTRIP)
            sent++;
            drawn += sent;
        }
    } while (--nbstrip);
    _nj_draw_polygon_ += drawn;
    _nj_calc_polygon_ += calculated;
    return sq;
}
