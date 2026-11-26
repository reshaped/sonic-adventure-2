#include <kamui.h>
#include <machine.h>
#include <ninja.h>
#include <nj_cnk.h>
#include <nj_direct.h>
#include <sg_xpt.h>

/* send entry as a vertex at sq, with param as its control word. sq is left past
 * the vertex */
#define SEND_VERTEX(entry, param)                                              \
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
        /* @hack spelling: the entry's fields read as p steps along it, where  \
         * reads of entry's fields would do */                                 \
        p = &(entry)->sx;                                                      \
        sx = *p++;                                                             \
        sy = *p++;                                                             \
        ooz = *p++;                                                            \
        ca = a * *p++;                                                         \
        cr = (r * *p++) + ar;                                                  \
        cg = (g * *p++) + ag;                                                  \
        cb = (b * *p) + ab;                                                    \
        /* @hack spelling: the words stored as sq steps down through the       \
         * vertex, where indexed stores from sq would do */                    \
        sq += CNK_BLOCK_WORDS;                                                 \
        *(Float *)--sq = cb;                                                   \
        *(Float *)--sq = cg;                                                   \
        *(Float *)--sq = cr;                                                   \
        *(Float *)--sq = ca;                                                   \
        *(Float *)--sq = ooz;                                                  \
        *(Float *)--sq = sy;                                                   \
        *(Float *)--sq = sx;                                                   \
        *--sq = (param);                                                       \
        prefetch(sq);                                                          \
        sq += CNK_BLOCK_WORDS;                                                 \
    }

Uint32 *njCnkSimpleMultiPolygonClip(Sint16 *strips,
                                    NJS_CNK_SIMPLEMULTI_BUF *buf,
                                    Uint32 nbstrip, Uint32 *sq, Float a,
                                    Float r, Float g, Float b, Float ar,
                                    Float ag, Float ab)
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

    prefetch(strips);
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
        v1 = &buf[*strips++];
        prefetch(v1);
        do {
            v2 = &buf[*strips++];
            prefetch(v2);
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
                    SEND_VERTEX(v0, KM_VERTEXPARAM_NORMAL)
                    sent++;
                }
                SEND_VERTEX(v1, KM_VERTEXPARAM_NORMAL)
                sent++;
            } else if (sent != 0) {
                SEND_VERTEX(v1, KM_VERTEXPARAM_ENDOFSTRIP)
                sent++;
                drawn += sent;
                sent = 0;
            }
            if (clipped) {
                /* @bug uninitialized: hands the near clip a triangle whose
                 * greatest z is exactly 1 (FCMP/GT FR3,FR13 at 0x8c145394). the
                 * near clip sends four vertices from unset locals for it (from
                 * 0x8c1667c0) */
#ifdef NONMATCHING
                /* this side repairs the tag above: a triangle goes to the near
                 * clip only where a corner lies beyond CNK_NEAR_Z */
                if (v0->z > CNK_NEAR_Z || v1->z > CNK_NEAR_Z ||
                    v2->z > CNK_NEAR_Z) {
                    if (CNK_POINT_WINDING(v0, v1, v2) * side < 0.0F) {
                        sq = njCnkSimpleMultiPolygonNearClip(
                            v0, v1, v2, sq, a, r, g, b, ar, ag, ab);
                    }
                }
#else
                if (v0->z >= CNK_NEAR_Z || v1->z >= CNK_NEAR_Z ||
                    v2->z >= CNK_NEAR_Z) {
                    if (CNK_POINT_WINDING(v0, v1, v2) * side < 0.0F) {
                        sq = njCnkSimpleMultiPolygonNearClip(
                            v0, v1, v2, sq, a, r, g, b, ar, ag, ab);
                    }
                }
#endif
            }
            strips += user_halfwords;
            prefetch(strips + 1);
            side = -side;
            v0 = v1;
            v1 = v2;
        } while (--len);
        if (sent != 0) {
            SEND_VERTEX(v2, KM_VERTEXPARAM_ENDOFSTRIP)
            sent++;
            drawn += sent;
        }
    } while (--nbstrip);
    _nj_draw_polygon_ += drawn;
    _nj_calc_polygon_ += calculated;
    return sq;
}
