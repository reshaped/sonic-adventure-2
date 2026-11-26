#include <kamui.h>
#include <machine.h>
#include <ninja.h>
#include <nj_cnk.h>
#include <nj_direct.h>
#include <nj_global.h>
#include <sg_xpt.h>

/* the text of njCnkSimplePolygonD8EnvDb and njCnkSimpleTextureD8EnvDb.
 * njCnkSimpleTextureD8EnvDb.c defines UV_HALFWORDS and UVSCALE_PARAM before it
 * includes this text. UV_HALFWORDS is how many halfwords of u and v follow each
 * index in the function's strips. it skips them. UVSCALE_PARAM is any parameter
 * after sq, with its comma. the values below are njCnkSimplePolygonD8EnvDb's.
 * its NJD_CS strips hold no u or v, and it takes no uvscale.
 * njCnkSimpleTextureD8EnvDb takes the uvscale every texture function takes, and
 * never reads it */
#ifndef UV_HALFWORDS
#define UV_HALFWORDS 0
#define UVSCALE_PARAM
#endif

/* the member exports this function, though no header of the SDK declares
 * it */
/* NOLINTBEGIN(misc-use-internal-linkage) */
/* njCnkSimplePolygonD8EnvDb is njCnkSimplePolygonD8Env for strips under
 * NJD_FST_DB */
Uint32 *njCnkSimplePolygonD8EnvDb(Sint16 *strips, NJS_CNK_SIMPLE_BUF *buf,
                                  Uint32 nbstrip, Uint32 *sq UVSCALE_PARAM);
/* NOLINTEND(misc-use-internal-linkage) */

/* load entry's x, y, ooz, inten and spec into the locals of those names. u
 * and v get half of its nx and ny, plus uoff and voff */
#define LOAD_ENTRY(entry)                                                      \
    {                                                                          \
        /* @hack spelling: the entry's fields read as p steps along it, where  \
         * reads of entry's fields would do */                                 \
        p = &(entry)->x;                                                       \
        x = *p++;                                                              \
        y = *p++;                                                              \
        ooz = *p++;                                                            \
        inten = *p++;                                                          \
        spec = *p++;                                                           \
        /* @hack spelling: u and v each set, then added to, where one sum each \
         * would do */                                                         \
        u = uoff;                                                              \
        u += half * *p++;                                                      \
        v = voff;                                                              \
        v += half * *p;                                                        \
    }

/* send the entry LOAD_ENTRY loaded last as a vertex at sq, with param as its
 * control word. sq is left past the vertex */
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

Uint32 *njCnkSimplePolygonD8EnvDb(Sint16 *strips, NJS_CNK_SIMPLE_BUF *buf,
                                  Uint32 nbstrip, Uint32 *sq UVSCALE_PARAM)
{
    /* @hack redundant: register on user_halfwords, uoff, voff, entry, x, y,
     * ooz, inten, spec, u and v */
    register Sint32 user_halfwords;
    /* @hack spelling: the local half, where the constant 0.5F would do */
    Float half;
    register Float uoff;
    register Float voff;
    Sint32 len;
    Sint32 drawn;
    Sint32 calculated;
    register NJS_CNK_SIMPLE_BUF *entry;
    Float *p;
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
    half = 0.5F;
    if (_nj_control_3d_flag_ & NJD_CONTROL_3D_ENV_UV_SCROLL) {
        uoff = _nj_uv_scroll_u_ + half;
        voff = _nj_uv_scroll_v_ + half;
    } else {
        uoff = half;
        /* @hack spelling: voff copied from uoff, where voff = half would do */
        voff = uoff;
    }
    drawn = 0;
    calculated = 0;
    do {
        len = *strips++;
        entry = &buf[*strips++];
        prefetch(entry);
        strips += UV_HALFWORDS;
        if (len < 0) {
            len = -len;
        }
        prefetch(strips + CNK_STRIP_AHEAD);
        /* @hack spelling: LOAD_ENTRY(entry) written out, with u set in one
         * statement, where the macro would do */
        p = &entry->x;
        x = *p++;
        y = *p++;
        ooz = *p++;
        inten = *p++;
        spec = *p++;
        u = uoff + (half * *p++);
        v = voff;
        v += half * *p;
        drawn += len;
        calculated += len;
        len -= CNK_STRIP_LEAD;
        entry = &buf[*strips++];
        prefetch(entry);
        /* @hack redundant: an empty test of v, spec, inten and y under
         * if (0) */
        if (0) {
            if (v < spec && inten < y) {
            }
        }
        SEND_VERTEX(KM_VERTEXPARAM_NORMAL)
        strips += UV_HALFWORDS;
        prefetch(strips + CNK_STRIP_AHEAD);
        LOAD_ENTRY(entry)
        do {
            entry = &buf[*strips++];
            prefetch(entry);
            /* @hack redundant: an empty test of ooz, inten and spec under
             * if (0) */
            if (0) {
                if (ooz == inten + spec) {
                }
            }
            SEND_VERTEX(KM_VERTEXPARAM_NORMAL)
            strips += UV_HALFWORDS;
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
