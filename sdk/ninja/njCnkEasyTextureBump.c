#include <kamui.h>
#include <machine.h>
#include <ninja.h>
#include <nj_cnk.h>
#include <nj_direct.h>
#include <nj_global.h>
#include <sg_xpt.h>

/* send a vertex as the 32 bytes before sq, with param as its control word.
 * sq is left where it was, past the vertex */
#define SEND_VERTEX(param)                                                     \
    {                                                                          \
        /* @hack spelling: the words stored as sq steps back from the          \
         * vertex's end, where indexed stores from sq would do */              \
        *--sq = offset_color;                                                  \
        *--sq = base_color;                                                    \
        *(Float *)--sq = tv;                                                   \
        *(Float *)--sq = tu;                                                   \
        *(Float *)--sq = ooz;                                                  \
        *(Float *)--sq = y;                                                    \
        *(Float *)--sq = x;                                                    \
        *--sq = (param);                                                       \
        prefetch(sq);                                                          \
        sq += CNK_BLOCK_WORDS;                                                 \
    }

/* load tu and tv from the two halfwords after a vertex's index, each times
 * uvscale */
#define LOAD_UV()                                                              \
    {                                                                          \
        tu = *strips++ * uvscale;                                              \
        tv = *strips++ * uvscale;                                              \
    }

/* load the locals x, y and ooz from the entry whose x p points to */
#define LOAD_ENTRY()                                                           \
    {                                                                          \
        /* @hack spelling: the fields read as p steps along the entry, where   \
         * p[0], p[1] and p[2] would do */                                     \
        x = *p++;                                                              \
        y = *p++;                                                              \
        ooz = *p;                                                              \
    }

Uint32 *njCnkEasyTextureBump(Sint16 *strips, NJS_CNK_EASY_BUF *buf,
                             Uint32 nbstrip, Uint32 *sq, Float uvscale)
{
    /* @hack spelling: light declared ahead of view, not after it */
    NJS_VECTOR light;
    /* @hack type: normal as three floats, where an NJS_VECTOR would do */
    Float normal[3];
    Float *s;
    Float *t;
    NJS_MATRIX frame;
    NJS_MATRIX view;
    /* @hack spelling: mp, a void pointer to view for st_ext, where view cast
     * in the call would do */
    void *mp;
    /* @hack redundant: register on p */
    register Float *p;
    /* @hack spelling: light written through lp, where light written in place
     * would do */
    NJS_VECTOR *lp;
    /* @hack redundant: register on user_halfwords, drawn and calculated */
    register Sint32 user_halfwords;
    register Sint32 drawn;
    register Sint32 calculated;
    Sint32 len;
    Uint32 offset_color;
    Uint32 base_color;
    Float x;
    Float ooz;
    Float tv;
    /* @hack redundant: register on y and tu */
    register Float y;
    register Float tu;

    /* s and t are the bump texture's two vectors. normal is their cross
     * product. frame takes the x axis to normal, y to -s and z to t. view is
     * the current matrix, which st_ext stores from XMTRX. njMultiMatrix puts
     * view before frame. light is the easy light's direction turned by the
     * transpose of the new frame, at unit length */
    /* @hack spelling: s and t set ahead of st_ext, not after it */
    s = _nj_cnk_easy_bump_tex_vector_;
    t = _nj_cnk_easy_bump_tex_vector_ + BUMP_T;
    mp = view;
    st_ext(mp);
    normal[0] = (s[1] * t[2]) - (s[2] * t[1]);
    normal[1] = (s[2] * t[0]) - (s[0] * t[2]);
    normal[2] = (s[0] * t[1]) - (s[1] * t[0]);
    frame[0] = normal[0];
    frame[1] = normal[1];
    frame[2] = normal[2];
    frame[3] = 0.0F;
    frame[4] = -s[0];
    frame[5] = -s[1];
    frame[6] = -s[2];
    frame[7] = 0.0F;
    frame[8] = t[0];
    frame[9] = t[1];
    frame[10] = t[2];
    /* @hack spelling: frame[11] to frame[14] zeroed in one chained
     * assignment, where a statement for each would do */
    frame[14] = frame[13] = frame[12] = frame[11] = 0.0F;
    frame[15] = 1.0F;
    njMultiMatrix(&frame, &view);
    lp = &light;
    lp->x = (frame[0] * _nj_vertex_easy.light.x) +
            (frame[1] * _nj_vertex_easy.light.y) +
            (frame[2] * _nj_vertex_easy.light.z);
    lp->y = (frame[4] * _nj_vertex_easy.light.x) +
            (frame[5] * _nj_vertex_easy.light.y) +
            (frame[6] * _nj_vertex_easy.light.z);
    lp->z = (frame[8] * _nj_vertex_easy.light.x) +
            (frame[9] * _nj_vertex_easy.light.y) +
            (frame[10] * _nj_vertex_easy.light.z);
    njUnitVector(lp);
    base_color = _nj_cnk_easy_bump_lt_color_;
    offset_color = njCnkEasyBumpLight(lp);

    prefetch(strips + CNK_STRIP_AHEAD);
    user_halfwords = (nbstrip >> NJD_UFO_SHIFT) & STRIP_USER;
    /* @hack redundant: a test of user_halfwords that never runs */
    if (0) {
        if (user_halfwords) {
        }
    }
    nbstrip &= STRIP_COUNT;
    /* @hack spelling: calculated and drawn zeroed through len, where
     * calculated = drawn = 0 would do */
    len = 0;
    calculated = drawn = len;
    do {
        len = *strips++;
        p = &buf[*strips++].x;
        prefetch(p);
        if (len < 0) {
            len = -len;
        }
        {
            /* @hack redundant: the first vertex's own x, ooz and tu, where
             * the function's would do */
            /* @hack redundant: register on x and ooz */
            register Float x;
            register Float ooz;
            Float tu;

            LOAD_UV()
            prefetch(strips + CNK_STRIP_AHEAD);
            LOAD_ENTRY()
            drawn += len;
            calculated += len;
            len -= CNK_STRIP_LEAD;
            p = &buf[*strips++].x;
            prefetch(p);
            /* @hack redundant: a test of x * x that never runs */
            if (0) {
                if (x * x) {
                }
            }
            sq += CNK_BLOCK_WORDS;
            /* @hack redundant: a test of y that never runs */
            if (0) {
                if (y) {
                }
            }
            SEND_VERTEX(KM_VERTEXPARAM_NORMAL)
        }
        LOAD_UV()
        prefetch(strips + CNK_STRIP_AHEAD);
        LOAD_ENTRY()
        do {
            p = &buf[*strips++].x;
            prefetch(p);
            sq += CNK_BLOCK_WORDS;
            SEND_VERTEX(KM_VERTEXPARAM_NORMAL)
            LOAD_UV()
            strips += user_halfwords;
            prefetch(strips + CNK_STRIP_AHEAD);
            LOAD_ENTRY()
        } while (--len);
        sq += CNK_BLOCK_WORDS;
        SEND_VERTEX(KM_VERTEXPARAM_ENDOFSTRIP)
    } while (--nbstrip);
    _nj_draw_polygon_ += drawn;
    _nj_calc_polygon_ += calculated;
    return sq;
}
