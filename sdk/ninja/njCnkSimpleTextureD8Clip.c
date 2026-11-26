#include <kamui.h>
#include <machine.h>
#include <ninja.h>
#include <nj_cnk.h>
#include <nj_direct.h>
#include <sg_xpt.h>

#include "../ninja/njCnkSimpleTextureD8Clip.h"

/* the member exports this function, though no header of the SDK declares
 * it */
/* NOLINTBEGIN(misc-use-internal-linkage) */
/* njCnkSimpleTextureD8Clip is njCnkSimpleTextureD8's Clip form */
Uint32 *njCnkSimpleTextureD8Clip(Sint16 *strips, NJS_CNK_SIMPLE_BUF *buf,
                                 Uint32 nbstrip, Uint32 *sq, Float uvscale);
/* NOLINTEND(misc-use-internal-linkage) */

Uint32 *njCnkSimpleTextureD8Clip(Sint16 *strips, NJS_CNK_SIMPLE_BUF *buf,
                                 Uint32 nbstrip, Uint32 *sq, Float uvscale)
{
    /* @hack redundant: register on every local */
    register NJS_CNK_SIMPLE_BUF *v0;
    register NJS_CNK_SIMPLE_BUF *v1;
    register NJS_CNK_SIMPLE_BUF *v2;
    register Sint32 len;
    register Sint32 sent;
    register Sint32 drawn;
    register Sint32 calculated;
    register Sint32 user_halfwords;
    register Bool clipped;
    register Float tu0;
    register Float tv0;
    register Float tu1;
    register Float tv1;
    register Float tu2;
    register Float tv2;
    register Float model_side;
    register Float side;
    register Float facing;

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
                facing = (((v1->x - v0->x) * (v2->y - v1->y)) -
                          ((v1->y - v0->y) * (v2->x - v1->x))) *
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
            /* @hack spelling: the test of the zs nested in the test of clipped,
             * where clipped && the zs' test would do */
            if (clipped) {
                if (v0->z > CNK_NEAR_Z || v1->z > CNK_NEAR_Z ||
                    v2->z > CNK_NEAR_Z) {
                    sq = njCnkSimpleTextureD8NearClip(v0, v1, v2, sq, tu0, tv0,
                                                      tu1, tv1, tu2, tv2, side);
                }
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
