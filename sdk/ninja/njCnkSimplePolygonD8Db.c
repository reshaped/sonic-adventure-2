#include <kamui.h>
#include <machine.h>
#include <ninja.h>
#include <nj_cnk.h>
#include <nj_direct.h>
#include <sg_xpt.h>

/* the member exports this function, though no header of the SDK declares
 * it */
/* NOLINTBEGIN(misc-use-internal-linkage) */
/* njCnkSimplePolygonD8Db is njCnkSimplePolygonD8 for strips under
 * NJD_FST_DB */
Uint32 *njCnkSimplePolygonD8Db(Sint16 *strips, NJS_CNK_SIMPLE_BUF *buf,
                               Uint32 nbstrip, Uint32 *sq);
/* NOLINTEND(misc-use-internal-linkage) */

/* load entry's x, y, ooz and inten into the locals of those names */
#define LOAD_ENTRY(entry)                                                      \
    {                                                                          \
        /* @hack spelling: the entry's fields read as p steps along it, where  \
         * reads of entry's fields would do */                                 \
        p = &(entry)->x;                                                       \
        x = *p++;                                                              \
        y = *p++;                                                              \
        ooz = *p++;                                                            \
        inten = *p;                                                            \
    }

/* send the entry LOAD_ENTRY loaded last as a vertex at sq, with param as its
 * control word. sq is left past the vertex */
#define SEND_VERTEX(param)                                                     \
    {                                                                          \
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

/* @hack redundant: register on sq */
Uint32 *njCnkSimplePolygonD8Db(Sint16 *strips, NJS_CNK_SIMPLE_BUF *buf,
                               Uint32 nbstrip, register Uint32 *sq)
{
    /* @hack redundant: register on user_halfwords, p, x, y, ooz and inten */
    register Sint32 user_halfwords;
    Sint32 len;
    Sint32 drawn;
    Sint32 calculated;
    NJS_CNK_SIMPLE_BUF *entry;
    register Float *p;
    register Float x;
    register Float y;
    register Float ooz;
    register Float inten;

    prefetch(strips + CNK_STRIP_AHEAD);
    user_halfwords = (nbstrip >> NJD_UFO_SHIFT) & STRIP_USER;
    nbstrip &= STRIP_COUNT;
    /* @hack spelling: calculated and drawn zeroed in one chained assignment,
     * where a statement for each would do */
    calculated = drawn = 0;
    do {
        len = *strips++;
        entry = &buf[*strips++];
        prefetch(entry);
        if (len < 0) {
            len = -len;
        }
        prefetch(strips + CNK_STRIP_AHEAD);
        LOAD_ENTRY(entry)
        calculated += len;
        drawn += len;
        len -= CNK_STRIP_LEAD;
        entry = &buf[*strips++];
        prefetch(entry);
        /* SEND_VERTEX(KM_VERTEXPARAM_NORMAL), written out around the test
         * below */
        sq += CNK_SPEC_WORD;
        *(Float *)--sq = inten;
        sq -= CNK_UV_WORDS;
        /* @hack redundant: an empty test of ooz under if (0), where the macro
         * alone would do */
        if (0) {
            if (ooz) {
            }
        }
        *(Float *)--sq = ooz;
        *(Float *)--sq = y;
        *(Float *)--sq = x;
        *--sq = KM_VERTEXPARAM_NORMAL;
        prefetch(sq);
        sq += CNK_BLOCK_WORDS;
        prefetch(strips + CNK_STRIP_AHEAD);
        LOAD_ENTRY(entry)
        do {
            entry = &buf[*strips++];
            prefetch(entry);
            SEND_VERTEX(KM_VERTEXPARAM_NORMAL)
            strips += user_halfwords;
            prefetch(strips + CNK_STRIP_AHEAD);
            LOAD_ENTRY(entry)
        } while (--len);
        SEND_VERTEX(KM_VERTEXPARAM_ENDOFSTRIP)
    } while (--nbstrip);
    _nj_draw_polygon_ += drawn;
    _nj_calc_polygon_ += calculated;
    return sq;
}
