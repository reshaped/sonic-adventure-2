#include <kamui.h>
#include <machine.h>
#include <ninja.h>
#include <nj_cnk.h>
#include <nj_direct.h>
#include <sg_xpt.h>

/* the member exports this function, though no header of the SDK declares
 * it */
/* NOLINTBEGIN(misc-use-internal-linkage) */
/* njCnkSimplePolygonD8 serves vertices that hold their color, in NJD_CS
 * strips without NJD_FST_DB or NJD_FST_ENV. a sent vertex's words 1 to 3 are
 * its entry's x, y and ooz. word 6 is its entry's inten, a packed ARGB word */
Uint32 *njCnkSimplePolygonD8(Sint16 *strips, NJS_CNK_SIMPLE_BUF *buf,
                             Uint32 nbstrip, Uint32 *sq);
/* NOLINTEND(misc-use-internal-linkage) */

/* send entry as a vertex at sq, with param as its control word. sq is left past
 * the vertex */
#define SEND_VERTEX(entry, param)                                              \
    {                                                                          \
        /* @hack redundant: register on the macro's locals */                  \
        register Float *p;                                                     \
        register Float x;                                                      \
        register Float y;                                                      \
        register Float ooz;                                                    \
        register Float inten;                                                  \
        /* @hack spelling: the entry's fields read as p steps along it, where  \
         * reads of entry's fields would do */                                 \
        p = &(entry)->x;                                                       \
        x = *p++;                                                              \
        y = *p++;                                                              \
        ooz = *p++;                                                            \
        inten = *p;                                                            \
        /* @hack spelling: the words stored as sq steps down through the       \
         * vertex, where indexed stores from sq would do */                    \
        sq += CNK_SPEC_WORD;                                                   \
        *(Float *)--sq = inten;                                                \
        sq -= CNK_UV_WORDS;                                                    \
        *(Float *)--sq = ooz;                                                  \
        *(Float *)--sq = y;                                                    \
        *(Float *)--sq = x;                                                    \
        *--sq = (param);                                                       \
        prefetch(sq);                                                          \
        sq += CNK_BLOCK_WORDS;                                                 \
    }

Uint32 *njCnkSimplePolygonD8(Sint16 *strips, NJS_CNK_SIMPLE_BUF *buf,
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
        v1 = &buf[*strips++];
        prefetch(v1);
        do {
            v2 = &buf[*strips++];
            prefetch(v2);
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
