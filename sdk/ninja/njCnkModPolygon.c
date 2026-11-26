#include <kamui.h>
#include <machine.h>
#include <ninja.h>
#include <nj_cnk.h>
#include <nj_direct.h>
#include <nj_km.h>
#include <sg_xpt.h>

/* the vertices of a triangle, each an index in buf */
#define TRI_VERTICES 3
/* a sent triangle's words: its control word, then each vertex's, its entry's
 * x, y and ooz */
#define CONTROL_WORDS 1
#define VERTEX_WORDS 3
/* nbtri while the loop sends the next to last triangle, which
 * njEndModifier's parameter follows */
#define NEXT_TO_LAST 2

Uint32 *njCnkModPolygon(Sint16 *tris, NJS_CNK_MOD_BUF *buf, Uint32 nbtri,
                        Uint32 *sq)
{
    /* @hack spelling: each entry's fields read as v0, v1 or v2 steps along
     * it, where reads of the entry's fields would do */
    Float *v0;
    Float *v1;
    Float *v2;
    Uint32 user_halfwords;
    Float x;
    Float y;
    /* @hack redundant: register on ooz */
    register Float ooz;

    prefetch(tris + TRI_VERTICES);
    user_halfwords = (nbtri >> NJD_UFO_SHIFT) & STRIP_USER;
    nbtri &= STRIP_COUNT;
    _nj_draw_polygon_ += nbtri * TRI_VERTICES;
    _nj_calc_polygon_ += nbtri * TRI_VERTICES;
    do {
        v0 = &buf[*tris++].x;
        prefetch(v0);
        *sq = KM_VERTEXPARAM_ENDOFSTRIP;
        /* @hack spelling: the words stored as sq steps back from each
         * vertex's last, where indexed stores from sq would do */
        sq += CONTROL_WORDS + VERTEX_WORDS;
        v1 = &buf[*tris++].x;
        v2 = &buf[*tris++].x;
        prefetch(v1);
        x = *v0++;
        y = *v0++;
        ooz = *v0;
        /* @hack redundant: an assignment of y to x that never runs */
        if (0) {
            x = y;
        }
        *(Float *)--sq = ooz;
        *(Float *)--sq = y;
        *(Float *)--sq = x;
        sq += 2 * VERTEX_WORDS;
        prefetch(v2);
        x = *v1++;
        y = *v1++;
        ooz = *v1;
        *(Float *)--sq = ooz;
        *(Float *)--sq = y;
        *(Float *)--sq = x;
        x = *v2++;
        /* past v1's words and v2's x, the first block's last word */
        sq += VERTEX_WORDS + 1;
        *(Float *)--sq = x;
        prefetch(sq);
        y = *v2++;
        sq += VERTEX_WORDS;
        ooz = *v2;
        *(Float *)--sq = ooz;
        *(Float *)--sq = y;
        prefetch(sq);
        sq += CNK_BLOCK_WORDS;
        tris += user_halfwords;
        if (nbtri == NEXT_TO_LAST) {
            sq = njEndModifier(sq);
        }
        prefetch(tris + TRI_VERTICES);
    } while (--nbtri);
    return sq;
}
