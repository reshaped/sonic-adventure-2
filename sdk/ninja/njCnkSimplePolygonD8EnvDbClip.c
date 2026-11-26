#include <kamui.h>
#include <machine.h>
#include <ninja.h>
#include <nj_cnk.h>
#include <nj_direct.h>
#include <nj_global.h>
#include <sg_xpt.h>

#include "../ninja/njCnkSimpleD8Env.h"

/* the text of njCnkSimplePolygonD8EnvDbClip and njCnkSimpleTextureD8EnvDbClip.
 * njCnkSimpleTextureD8EnvDbClip.c defines UV_HALFWORDS and UVSCALE_PARAM before
 * it includes this text. UV_HALFWORDS is how many halfwords of u and v follow
 * each index in the function's strips. it skips them. UVSCALE_PARAM is any
 * parameter after sq, with its comma. the values below are
 * njCnkSimplePolygonD8EnvDbClip's. its NJD_CS strips hold no u or v, and it
 * takes no uvscale. njCnkSimpleTextureD8EnvDbClip takes the uvscale every
 * texture function takes, and never reads it */
#ifndef UV_HALFWORDS
#define UV_HALFWORDS 0
#define UVSCALE_PARAM
#endif

/* the member exports this function, though no header of the SDK declares
 * it */
/* NOLINTBEGIN(misc-use-internal-linkage) */
/* njCnkSimplePolygonD8EnvDbClip is njCnkSimplePolygonD8EnvDb's Clip form */
Uint32 *njCnkSimplePolygonD8EnvDbClip(Sint16 *strips, NJS_CNK_SIMPLE_BUF *buf,
                                      Uint32 nbstrip, Uint32 *sq UVSCALE_PARAM);
/* NOLINTEND(misc-use-internal-linkage) */

Uint32 *njCnkSimplePolygonD8EnvDbClip(Sint16 *strips, NJS_CNK_SIMPLE_BUF *buf,
                                      Uint32 nbstrip, Uint32 *sq UVSCALE_PARAM)
{
    /* @hack redundant: register on every local but clipped */
    register NJS_CNK_SIMPLE_BUF *v0;
    register NJS_CNK_SIMPLE_BUF *v1;
    register NJS_CNK_SIMPLE_BUF *v2;
    register Sint32 len;
    register Sint32 user_halfwords;
    register Sint32 sent;
    register Sint32 drawn;
    register Sint32 calculated;
    Bool clipped;
    register Float uoff;
    register Float voff;

    prefetch(strips + CNK_STRIP_AHEAD);
    user_halfwords = (nbstrip >> NJD_UFO_SHIFT) & STRIP_USER;
    nbstrip &= STRIP_COUNT;
    if (_nj_control_3d_flag_ & NJD_CONTROL_3D_ENV_UV_SCROLL) {
        uoff = _nj_uv_scroll_u_ + CNK_ENV_CENTER;
        voff = _nj_uv_scroll_v_ + CNK_ENV_CENTER;
    } else {
        uoff = CNK_ENV_CENTER;
        voff = CNK_ENV_CENTER;
    }
    drawn = 0;
    calculated = 0;
    do {
        len = *strips++;
        v0 = &buf[*strips++];
        prefetch(v0);
        if (len < 0) {
            len = -len;
        }
        calculated += len;
        len -= CNK_STRIP_LEAD;
        strips += UV_HALFWORDS;
        sent = 0;
        v1 = &buf[*strips++];
        prefetch(v1);
        strips += UV_HALFWORDS;
        do {
            v2 = &buf[*strips++];
            prefetch(v2);
            strips += UV_HALFWORDS;
            /* @hack spelling: clipped set FALSE, then TRUE under a test, where
             * clipped set to the test's result would do */
            clipped = FALSE;
            if (v0->z <= CNK_NEAR_Z || v1->z <= CNK_NEAR_Z ||
                v2->z <= CNK_NEAR_Z) {
                clipped = TRUE;
            }
            if (!clipped) {
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
            /* @hack spelling: the test of the zs nested in the test of clipped,
             * where clipped && the zs' test would do */
            if (clipped) {
                if (v0->z > CNK_NEAR_Z || v1->z > CNK_NEAR_Z ||
                    v2->z > CNK_NEAR_Z) {
                    sq = njCnkSimpleTextureD8EnvDbNearClip(v0, v1, v2, sq);
                }
            }
            strips += user_halfwords;
            prefetch(strips + CNK_STRIP_AHEAD);
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
