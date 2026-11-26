#include <kamui.h>
#include <machine.h>
#include <ninja.h>
#include <nj_cnk.h>
#include <nj_direct.h>
#include <nj_global.h>
#include <sg_xpt.h>

/* the offset and base colors that njCnkEasyTextureBumpClip sets on each
 * call. every vertex it or its near clip sends takes them */
static Uint32 offset_color;
static Uint32 base_color;

/* send entry as a vertex at sq, with param as its control word. (u, v) is
 * its texture coordinate. sq is left past the vertex */
#define SEND_VERTEX(entry, u, v, param)                                        \
    {                                                                          \
        /* @hack spelling: the entry's fields read as p steps along it, where  \
         * reads of entry's fields would do */                                 \
        p = &(entry)->x;                                                       \
        x = *p++;                                                              \
        y = *p++;                                                              \
        ooz = *p;                                                              \
        /* @hack spelling: the words stored as sq steps back from the          \
         * vertex's end, where indexed stores from sq would do */              \
        sq += CNK_BLOCK_WORDS;                                                 \
        *--sq = offset_color;                                                  \
        *--sq = base_color;                                                    \
        *(Float *)--sq = (v);                                                  \
        *(Float *)--sq = (u);                                                  \
        *(Float *)--sq = ooz;                                                  \
        *(Float *)--sq = y;                                                    \
        *(Float *)--sq = x;                                                    \
        *--sq = (param);                                                       \
        prefetch(sq);                                                          \
        sq += CNK_BLOCK_WORDS;                                                 \
    }

Uint32 *njCnkEasyTextureBumpClip(Sint16 *strips, NJS_CNK_EASY_BUF *buf,
                                 Uint32 nbstrip, Uint32 *sq, Float uvscale)
{
    /* @hack spelling: light declared ahead of view, not after it */
    NJS_VECTOR light;
    /* @hack type: normal as three floats, where an NJS_VECTOR would do */
    Float normal[3];
    /* @hack spelling: mp, a void pointer to view for st_ext, where view cast
     * in the call would do */
    void *mp;
    Float *s;
    Float *t;
    NJS_MATRIX frame;
    NJS_MATRIX view;
    /* @hack spelling: light written through lp, where light written in place
     * would do */
    NJS_VECTOR *lp;
    struct nj_cnk_easy_work *wp;
    /* @hack redundant: register on every local from v0 on */
    register NJS_CNK_EASY_BUF *v0;
    register NJS_CNK_EASY_BUF *v1;
    register NJS_CNK_EASY_BUF *v2;
    register Sint32 len;
    register Sint32 calculated;
    register Sint32 drawn;
    register Sint32 sent;
    register Float tu0;
    register Float tv0;
    register Float tu1;
    register Float tv1;
    register Float tu2;
    register Float tv2;
    register Float *p;
    register Float x;
    register Float y;
    register Float ooz;
    register Sint32 user_halfwords;
    register Bool clipped;

    /* the function makes view, frame and light as njCnkEasyTextureBump
     * does */
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
    frame[11] = 0.0F;
    frame[12] = 0.0F;
    frame[13] = 0.0F;
    frame[14] = 0.0F;
    frame[15] = 1.0F;
    njMultiMatrix(&frame, &view);
    lp = &light;
    /* @hack spelling: _nj_vertex_easy read through wp, set in the
     * expression, where _nj_vertex_easy read in place would do */
    lp->x = (wp = &_nj_vertex_easy, (frame[0] * wp->light.x) +
                                        (frame[1] * wp->light.y) +
                                        (frame[2] * wp->light.z));
    lp->y = (frame[4] * _nj_vertex_easy.light.x) +
            (frame[5] * _nj_vertex_easy.light.y) +
            (frame[6] * _nj_vertex_easy.light.z);
    /* @hack spelling: frame[9]'s term first, under a unary plus, where the
     * terms in the order of x, y and z would do */
    lp->z = +(frame[9] * _nj_vertex_easy.light.y) +
            (frame[8] * _nj_vertex_easy.light.x) +
            (frame[10] * _nj_vertex_easy.light.z);
    njUnitVector(lp);
    base_color = _nj_cnk_easy_bump_lt_color_;
    offset_color = njCnkEasyBumpLight(lp);

    prefetch(strips + CNK_STRIP_AHEAD);
    user_halfwords = (nbstrip >> NJD_UFO_SHIFT) & STRIP_USER;
    nbstrip &= STRIP_COUNT;
    calculated = drawn = 0;
    do {
        len = *strips++;
        /* @hack spelling: the assignment to v0 inside prefetch's argument,
         * where a statement of its own would do */
        prefetch(v0 = &buf[*strips++]);
        if (len < 0) {
            len = -len;
        }
        calculated += len;
        len -= CNK_STRIP_LEAD;
        /* sent counts the vertices the open run has sent, and is 0 while no
         * run is open */
        sent = 0;
        tu0 = *strips++ * uvscale;
        tv0 = *strips++ * uvscale;
        v1 = &buf[*strips++];
        prefetch(v1);
        tu1 = *strips++ * uvscale;
        tv1 = *strips++ * uvscale;
        /* @hack redundant: an increment of tu0 that never runs */
        if (0) {
            tu0++;
        }
        do {
            v2 = &buf[*strips++];
            prefetch(v2);
            tu2 = *strips++ * uvscale;
            tv2 = *strips++ * uvscale;
            /* @hack redundant: an increment of tu2 that never runs */
            if (0) {
                tu2++;
            }
            /* @hack spelling: clipped set FALSE, then TRUE under a test, where
             * clipped set to the test's result would do */
            clipped = FALSE;
            if (!(v0->ooz > 0.0F && v1->ooz > 0.0F && v2->ooz > 0.0F &&
                  v0->ooz < CNK_NEAR_OOZ && v1->ooz < CNK_NEAR_OOZ &&
                  v2->ooz < CNK_NEAR_OOZ)) {
                clipped = TRUE;
            }
            if (!clipped) {
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
            /* @hack spelling: the test of the oozs nested in the test of
             * clipped, where clipped && the oozs' test would do */
            if (clipped) {
                if ((v0->ooz > 0.0F && v0->ooz < CNK_NEAR_OOZ) ||
                    (v1->ooz > 0.0F && v1->ooz < CNK_NEAR_OOZ) ||
                    (v2->ooz > 0.0F && v2->ooz < CNK_NEAR_OOZ)) {
                    sq = njCnkEasyTextureBumpNearClip(v0, v1, v2, sq, tu0, tv0,
                                                      tu1, tv1, tu2, tv2);
                }
            }
            strips += user_halfwords;
            prefetch(strips + CNK_STRIP_AHEAD);
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

/* a vertex of the triangle the near clip cuts, or of the strip it sends. sx
 * and sy place it on the screen, as a buffer entry's x and y do. x, y and z,
 * its point before projection, are set only in a vertex of the triangle. the
 * fields from v to sx lie in the reverse of the order their words are sent
 * in. SEND_CLIP_VERTEX reads them as a pointer steps up from v. the near clip
 * writes them as one steps down from x. pad holds nothing. it is the sixth
 * of the six floats below x that the near clip copies from a corner */
struct clip_vertex {
    Float pad;
    Float v;
    Float u;
    Float ooz;
    Float sy;
    Float sx;
    Float x;
    Float y;
    Float z;
};

/* send the strip's vertex at sq, with param as its control word. sq is left
 * past the vertex */
#define SEND_CLIP_VERTEX(vertex, param)                                        \
    {                                                                          \
        /* @hack spelling: the fields read as p steps up from v, where reads   \
         * of vertex's fields would do */                                      \
        p = &(vertex)->v;                                                      \
        /* @hack spelling: the words stored as sq steps back from the          \
         * vertex's end, where indexed stores from sq would do */              \
        sq += CNK_BLOCK_WORDS;                                                 \
        *--sq = offset_color;                                                  \
        *--sq = base_color;                                                    \
        *(Float *)--sq = *p++;                                                 \
        *(Float *)--sq = *p++;                                                 \
        *(Float *)--sq = *p++;                                                 \
        *(Float *)--sq = *p++;                                                 \
        *(Float *)--sq = *p;                                                   \
        *--sq = (param);                                                       \
        prefetch(sq);                                                          \
        sq += CNK_BLOCK_WORDS;                                                 \
    }

Uint32 *njCnkEasyTextureBumpNearClip(NJS_CNK_EASY_BUF *v0, NJS_CNK_EASY_BUF *v1,
                                     NJS_CNK_EASY_BUF *v2, Uint32 *sq,
                                     Float tu0, Float tv0, Float tu1, Float tv1,
                                     Float tu2, Float tv2)
{
    struct clip_vertex tri[CNK_TRI_CORNERS];
    struct clip_vertex strip[CNK_CLIP_VERTICES];
    /* @hack redundant: register on every local but tri and strip */
    register Float dist_x;
    register Float dist_y;
    register Float inv_dist_x;
    register Float inv_dist_y;
    register Float cx;
    register Float cy;
    register Float t;
    register Float x;
    register Float y;
    register Float *p;
    register Float *q;
    register Int i;
    register Int prev;
    register Int n;

    if (_nj_control_3d_flag_ & NJD_CONTROL_3D_DISABLE_NEAR_CLIP) {
        return sq;
    }
    /* @hack spelling: cx and cy set ahead of inv_dist_y, not after it */
    dist_x = _nj_screen_.dist * _nj_scr_aspect_x_;
    dist_y = _nj_screen_.dist * _nj_scr_aspect_y_;
    inv_dist_x = 1.0F / dist_x;
    cx = _nj_screen_.cx;
    cy = _nj_screen_.cy;
    inv_dist_y = 1.0F / dist_y;
    tri[0].z = 1.0F / v0->ooz;
    tri[0].u = tu0;
    tri[0].v = tv0;
    tri[0].sx = v0->x;
    tri[0].sy = v0->y;
    tri[0].ooz = v0->ooz;
    tri[0].x = (v0->x - cx) * tri[0].z * inv_dist_x;
    tri[0].y = (v0->y - cy) * tri[0].z * inv_dist_y;
    tri[1].z = 1.0F / v1->ooz;
    tri[1].u = tu1;
    tri[1].v = tv1;
    tri[1].sx = v1->x;
    tri[1].sy = v1->y;
    tri[1].ooz = v1->ooz;
    tri[1].x = (v1->x - cx) * tri[1].z * inv_dist_x;
    tri[1].y = (v1->y - cy) * tri[1].z * inv_dist_y;
    tri[2].z = 1.0F / v2->ooz;
    tri[2].u = tu2;
    tri[2].v = tv2;
    tri[2].sx = v2->x;
    tri[2].sy = v2->y;
    tri[2].ooz = v2->ooz;
    tri[2].x = (v2->x - cx) * tri[2].z * inv_dist_x;
    tri[2].y = (v2->y - cy) * tri[2].z * inv_dist_y;
    n = 0;
    for (prev = CNK_TRI_CORNERS - 1, i = 0; i < CNK_TRI_CORNERS;
         prev = i, i++) {
        /* the strip takes a vertex where the edge from prev to i crosses a z
         * of 1. t places that vertex on the edge, from 0 at prev to 1 at i */
        /* @hack spelling: the two tests negated and XORed, where
         * (tri[prev].z > CNK_NEAR_Z) != (tri[i].z > CNK_NEAR_Z) would do */
        if (!(tri[prev].z > CNK_NEAR_Z) ^ !(tri[i].z > CNK_NEAR_Z)) {
            t = (CNK_NEAR_Z - tri[prev].z) / (tri[i].z - tri[prev].z);
            x = cx + ((tri[prev].x + ((tri[i].x - tri[prev].x) * t)) * dist_x);
            y = cy + ((tri[prev].y + ((tri[i].y - tri[prev].y) * t)) * dist_y);
            /* @hack spelling: the fields stored as q steps down from x,
             * where stores to strip[n]'s fields would do */
            q = &strip[n].x;
            *--q = x;
            *--q = y;
            *--q = CNK_NEAR_OOZ;
            *--q = tri[prev].u + ((tri[i].u - tri[prev].u) * t);
            *--q = tri[prev].v + ((tri[i].v - tri[prev].v) * t);
            n++;
        }
        if (tri[i].z > CNK_NEAR_Z) {
            /* @hack spelling: the six fields below x copied as p and q step
             * down, where a copy of each field would do */
            q = &strip[n].x;
            p = &tri[i].x;
            *--q = *--p;
            *--q = *--p;
            *--q = *--p;
            *--q = *--p;
            *--q = *--p;
            *--q = *--p;
            n++;
        }
    }
    SEND_CLIP_VERTEX(&strip[0], KM_VERTEXPARAM_NORMAL)
    SEND_CLIP_VERTEX(&strip[1], KM_VERTEXPARAM_NORMAL)
    if (n != CNK_TRI_CORNERS) {
        SEND_CLIP_VERTEX(&strip[3], KM_VERTEXPARAM_NORMAL)
    }
    SEND_CLIP_VERTEX(&strip[2], KM_VERTEXPARAM_ENDOFSTRIP)
    _nj_draw_polygon_ += n;
    return sq;
}
