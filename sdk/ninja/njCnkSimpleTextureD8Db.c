#include <kamui.h>
#include <machine.h>
#include <ninja.h>
#include <nj_cnk.h>
#include <nj_direct.h>
#include <sg_xpt.h>

/* the member exports this function, though no header of the SDK declares
 * it */
/* NOLINTBEGIN(misc-use-internal-linkage) */
/* njCnkSimpleTextureD8Db is njCnkSimpleTextureD8 for strips under
 * NJD_FST_DB */
Uint32 *njCnkSimpleTextureD8Db(Sint16 *strips, NJS_CNK_SIMPLE_BUF *buf,
                               Uint32 nbstrip, Uint32 *sq, Float uvscale);
/* NOLINTEND(misc-use-internal-linkage) */

/* load u and v from the two halfwords after a vertex's index, each times
 * uvscale */
#define LOAD_UV()                                                              \
    {                                                                          \
        u = *strips++ * uvscale;                                               \
        v = *strips++ * uvscale;                                               \
    }

/* load the locals x, y, ooz, inten and spec from the entry whose z p points
 * to */
#define LOAD_ENTRY()                                                           \
    {                                                                          \
        p++;                                                                   \
        x = *p++;                                                              \
        y = *p++;                                                              \
        ooz = *p++;                                                            \
        inten = *p++;                                                          \
        spec = *p;                                                             \
    }

/* send the entry LOAD_ENTRY loaded last as a vertex at sq, with param as its
 * control word. its texture coordinate is the (u, v) LOAD_UV loaded last. sq is
 * left past the vertex */
#define SEND_VERTEX(param)                                                     \
    {                                                                          \
        /* @hack spelling: the words stored as sq steps down through the       \
         * vertex, where indexed stores from sq would do */                    \
        sq += CNK_BLOCK_WORDS;                                                 \
        *(Float *)--sq = spec;                                                 \
        *(Float *)--sq = inten;                                                \
        *(Float *)--sq = v;                                                    \
        *(Float *)--sq = u;                                                    \
        *(Float *)--sq = ooz;                                                  \
        *(Float *)--sq = y;                                                    \
        *(Float *)--sq = x;                                                    \
        *--sq = (param);                                                       \
        prefetch(sq);                                                          \
        sq += CNK_BLOCK_WORDS;                                                 \
    }

Uint32 *njCnkSimpleTextureD8Db(Sint16 *strips, NJS_CNK_SIMPLE_BUF *buf,
                               Uint32 nbstrip, Uint32 *sq, Float uvscale)
{
    /* @hack redundant: register on user_halfwords, p, x, y, ooz, inten, spec, u
     * and v */
    register Uint32 user_halfwords;
    Sint32 len;
    Sint32 drawn;
    Sint32 calculated;
    /* @hack spelling: each entry read through p, from the entry's z on,
     * where a pointer to the entry would do */
    register Float *p;
    register Float x;
    register Float y;
    register Float ooz;
    register Float inten;
    register Float spec;
    register Float u;
    register Float v;

    prefetch(strips + CNK_STRIP_AHEAD);
    user_halfwords = (nbstrip >> NJD_UFO_SHIFT) & STRIP_USER;
    nbstrip &= STRIP_COUNT;
    drawn = 0;
    calculated = 0;
    do {
        len = *strips++;
        p = &buf[*strips++].z;
        prefetch(p);
        if (len < 0) {
            len = -len;
        }
        LOAD_UV()
        prefetch(strips + CNK_STRIP_AHEAD);
        LOAD_ENTRY()
        drawn += len;
        calculated += len;
        len -= CNK_STRIP_LEAD;
        p = &buf[*strips++].z;
        prefetch(p);
        SEND_VERTEX(KM_VERTEXPARAM_NORMAL)
        LOAD_UV()
        prefetch(strips + CNK_STRIP_AHEAD);
        LOAD_ENTRY()
        do {
            p = &buf[*strips++].z;
            prefetch(p);
            /* @hack redundant: an empty test of inten, x and y under if (0) */
            if (0) {
                if (inten == x + y) {
                }
            }
            SEND_VERTEX(KM_VERTEXPARAM_NORMAL)
            LOAD_UV()
            strips += user_halfwords;
            prefetch(strips + CNK_STRIP_AHEAD);
            LOAD_ENTRY()
        } while (--len);
        SEND_VERTEX(KM_VERTEXPARAM_ENDOFSTRIP)
    } while (--nbstrip);
    _nj_draw_polygon_ += drawn;
    _nj_calc_polygon_ += calculated;
    return sq;
}
