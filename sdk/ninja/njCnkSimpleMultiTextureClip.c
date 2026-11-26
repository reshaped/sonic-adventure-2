#include <kamui.h>
#include <machine.h>
#include <ninja.h>
#include <nj_cnk.h>
#include <nj_direct.h>
#include <sg_xpt.h>

/* send entry as a vertex at sq, with param as its control word. (u, v) is its
 * texture coordinate. sq is left past the vertex */
#define SEND_VERTEX(entry, u, v, param)                                        \
    {                                                                          \
        /* @hack redundant: register on the macro's locals */                  \
        register Float *p;                                                     \
        register Float sx;                                                     \
        register Float sy;                                                     \
        register Float ooz;                                                    \
        register Float ca;                                                     \
        register Float cr;                                                     \
        register Float cg;                                                     \
        register Float cb;                                                     \
        register Float sr;                                                     \
        register Float sg;                                                     \
        register Float sb;                                                     \
        /* @hack spelling: the entry's fields read as p steps along it, where  \
         * reads of entry's fields would do */                                 \
        p = &(entry)->sx;                                                      \
        sx = *p++;                                                             \
        sy = *p++;                                                             \
        ooz = *p++;                                                            \
        /* @hack spelling: the words stored as sq steps down through each      \
         * block, where indexed stores from sq would do */                     \
        sq += CNK_UV_END;                                                      \
        *(Float *)--sq = (v);                                                  \
        *(Float *)--sq = (u);                                                  \
        *(Float *)--sq = ooz;                                                  \
        *(Float *)--sq = sy;                                                   \
        *(Float *)--sq = sx;                                                   \
        *--sq = (param);                                                       \
        prefetch(sq);                                                          \
        ca = a * *p++;                                                         \
        cr = ar + *p++;                                                        \
        cg = ag + *p++;                                                        \
        cb = ab + *p++;                                                        \
        p += CNK_SIMPLEMULTI_SR - CNK_SIMPLEMULTI_X;                           \
        sr = *p++;                                                             \
        sg = *p++;                                                             \
        sb = *p;                                                               \
        sq += CNK_VERTEX64_WORDS;                                              \
        *(Float *)--sq = sb;                                                   \
        *(Float *)--sq = sg;                                                   \
        *(Float *)--sq = sr;                                                   \
        sq--;                                                                  \
        *(Float *)--sq = cb;                                                   \
        *(Float *)--sq = cg;                                                   \
        *(Float *)--sq = cr;                                                   \
        *(Float *)--sq = ca;                                                   \
        prefetch(sq);                                                          \
        sq += CNK_BLOCK_WORDS;                                                 \
    }

Uint32 *njCnkSimpleMultiTextureClip(Sint16 *strips,
                                    NJS_CNK_SIMPLEMULTI_BUF *buf,
                                    Uint32 nbstrip, Uint32 *sq, Float uvscale,
                                    Float a, Float ar, Float ag, Float ab)
{
    /* @hack redundant: register on every local */
    register NJS_CNK_SIMPLEMULTI_BUF *v0;
    register NJS_CNK_SIMPLEMULTI_BUF *v1;
    register NJS_CNK_SIMPLEMULTI_BUF *v2;
    register Sint32 len;
    register Sint32 user_halfwords;
    register Sint32 sent;
    register Sint32 drawn;
    register Sint32 calculated;
    register Bool clipped;
    register Float model_side;
    register Float side;
    register Float facing;
    register Float tu0;
    register Float tv0;
    register Float tu1;
    register Float tv1;
    register Float tu2;
    register Float tv2;

    prefetch(strips + CNK_STRIP_AHEAD);
    user_halfwords = (nbstrip >> NJD_UFO_SHIFT) & STRIP_USER;
    nbstrip &= STRIP_COUNT;
    /* @hack spelling: calculated and drawn zeroed in one chained assignment,
     * where a statement for each would do */
    calculated = drawn = 0;
    model_side = 1.0F;
    if (_nj_control_3d_flag_ & NJD_CONTROL_3D_MIRROR_MODEL) {
        model_side = -1.0F;
    }
    do {
        len = *strips++;
        v0 = &buf[*strips++];
        prefetch(v0);
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
            /* @hack spelling: clipped set FALSE, then TRUE under a test, where
             * clipped set to the test's result would do */
            clipped = FALSE;
            if (v0->z <= CNK_NEAR_Z || v1->z <= CNK_NEAR_Z ||
                v2->z <= CNK_NEAR_Z) {
                clipped = TRUE;
                /* a triangle that isn't whole takes a facing above 0, which
                 * ends the run as facing away does */
                facing = 1.0F;
            } else {
                facing = (((v1->sx - v0->sx) * (v2->sy - v1->sy)) -
                          ((v1->sy - v0->sy) * (v2->sx - v1->sx))) *
                         side;
            }
            if (facing < 0.0F) {
                if (sent == 0) {
                    SEND_VERTEX(v0, tu0, tv0, KM_VERTEXPARAM_NORMAL)
                    sent++;
                }
                SEND_VERTEX(v1, tu1, tv1, KM_VERTEXPARAM_NORMAL)
                sent++;
            } else if (sent != 0) {
                SEND_VERTEX(v1, tu1, tv1, KM_VERTEXPARAM_ENDOFSTRIP)
                sent++;
                drawn += sent;
                sent = 0;
            }
            if (clipped) {
                /* @bug uninitialized: hands the near clip a triangle whose
                 * greatest z is exactly 1 (FCMP/GT FR2,FR3 at 0x8c146aa4). the
                 * near clip sends four vertices from unset locals for it (from
                 * 0x8c167c4e) */
#ifdef NONMATCHING
                /* this side repairs the tag above: a triangle goes to the near
                 * clip only where a corner lies beyond CNK_NEAR_Z */
                if (v0->z > CNK_NEAR_Z || v1->z > CNK_NEAR_Z ||
                    v2->z > CNK_NEAR_Z) {
                    if (CNK_POINT_WINDING(v0, v1, v2) * side < 0.0F) {
                        sq = njCnkSimpleMultiTextureNearClip(
                            v0, v1, v2, sq, tu0, tv0, tu1, tv1, tu2, tv2, a, ar,
                            ag, ab);
                    }
                }
#else
                if (v0->z >= CNK_NEAR_Z || v1->z >= CNK_NEAR_Z ||
                    v2->z >= CNK_NEAR_Z) {
                    if (CNK_POINT_WINDING(v0, v1, v2) * side < 0.0F) {
                        sq = njCnkSimpleMultiTextureNearClip(
                            v0, v1, v2, sq, tu0, tv0, tu1, tv1, tu2, tv2, a, ar,
                            ag, ab);
                    }
                }
#endif
            }
            strips += user_halfwords;
            prefetch(strips + CNK_STRIP_AHEAD);
            side = -side;
            v0 = v1;
            v1 = v2;
            tu0 = tu1;
            tv0 = tv1;
            tu1 = tu2;
            tv1 = tv2;
        } while (--len);
        if (sent != 0) {
            SEND_VERTEX(v2, tu2, tv2, KM_VERTEXPARAM_ENDOFSTRIP)
            sent++;
            drawn += sent;
        }
    } while (--nbstrip);
    _nj_draw_polygon_ += drawn;
    _nj_calc_polygon_ += calculated;
    return sq;
}
