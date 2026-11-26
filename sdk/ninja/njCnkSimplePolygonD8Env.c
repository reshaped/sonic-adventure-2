#include <kamui.h>
#include <machine.h>
#include <ninja.h>
#include <nj_cnk.h>
#include <nj_direct.h>
#include <nj_global.h>
#include <sg_xpt.h>

#include "../ninja/njCnkSimpleD8Env.h"

/* the member exports this function, though no header of the SDK declares
 * it */
/* NOLINTBEGIN(misc-use-internal-linkage) */
/* njCnkSimplePolygonD8Env is njCnkSimplePolygonD8 for strips under
 * NJD_FST_ENV. a sent vertex's words 4 and 5 are its u and v. word 7 is its
 * entry's spec */
Uint32 *njCnkSimplePolygonD8Env(Sint16 *strips, NJS_CNK_SIMPLE_BUF *buf,
                                Uint32 nbstrip, Uint32 *sq);
/* NOLINTEND(misc-use-internal-linkage) */

Uint32 *njCnkSimplePolygonD8Env(Sint16 *strips, NJS_CNK_SIMPLE_BUF *buf,
                                Uint32 nbstrip, Uint32 *sq)
{
    /* @hack redundant: register on every local */
    register NJS_CNK_SIMPLE_BUF *v0;
    register NJS_CNK_SIMPLE_BUF *v1;
    register NJS_CNK_SIMPLE_BUF *v2;
    register Sint32 len;
    register Sint32 user_halfwords;
    register Sint32 sent;
    register Sint32 drawn;
    register Sint32 calculated;
    register Float uoff;
    register Float voff;
    register Float model_side;
    register Float side;
    register Float facing;

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
            /* @bug logic: prefetches v1 again, where v2, the entry just
             * read, is meant (PREF @R12 at 0x8c15ef2a) */
            prefetch(v1);
            facing = (((v1->x - v0->x) * (v2->y - v1->y)) -
                      ((v1->y - v0->y) * (v2->x - v1->x))) *
                     side;
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
            strips += user_halfwords;
            prefetch(strips + CNK_STRIP_AHEAD);
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
