#include <kamui.h>
#include <machine.h>
#include <ninja.h>
#include <nj_cnk.h>
#include <nj_direct.h>
#include <sg_xpt.h>

/* the member exports this function, though no header of the SDK declares
 * it */
/* NOLINTBEGIN(misc-use-internal-linkage) */
/* njCnkSimplePolygonD8DbClip is njCnkSimplePolygonD8Db's Clip form */
Uint32 *njCnkSimplePolygonD8DbClip(Sint16 *strips, NJS_CNK_SIMPLE_BUF *buf,
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

Uint32 *njCnkSimplePolygonD8DbClip(Sint16 *strips, NJS_CNK_SIMPLE_BUF *buf,
                                   Uint32 nbstrip, Uint32 *sq)
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

    prefetch(strips + CNK_STRIP_AHEAD);
    user_halfwords = (nbstrip >> NJD_UFO_SHIFT) & STRIP_USER;
    nbstrip &= STRIP_COUNT;
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
                    sq = njCnkSimplePolygonD8DbNearClip(v0, v1, v2, sq);
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
