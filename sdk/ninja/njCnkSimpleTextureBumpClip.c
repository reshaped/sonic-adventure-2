#include <kamui.h>
#include <machine.h>
#include <ninja.h>
#include <nj_cnk.h>
#include <nj_direct.h>
#include <nj_global.h>
#include <sg_xpt.h>

/* the member exports these functions, though no header of the SDK declares
 * them */
/* NOLINTBEGIN(misc-use-internal-linkage) */
/* njCnkSimpleTextureBumpClip is njCnkSimpleTextureBump's Clip form. a
 * triangle is whole where each of its vertices has a z above 1. its near clip
 * is njCnkSimpleTextureBumpNearClip */
Uint32 *njCnkSimpleTextureBumpClip(Sint16 *strips, NJS_CNK_SIMPLE_BUF *buf,
                                   Uint32 nbstrip, Uint32 *sq, Float ambient,
                                   Float uvscale);
/* njCnkSimpleTextureBumpClip's near clip. it works as
 * njCnkSimpleTextureNearClip does, but never reads ambient. every vertex it
 * sends holds base_color and offset_color as words 6 and 7. only
 * njCnkSimpleTextureBumpClip sets the two colors, so only that function may
 * call the near clip */
Uint32 *njCnkSimpleTextureBumpNearClip(NJS_CNK_SIMPLE_BUF *v0,
                                       NJS_CNK_SIMPLE_BUF *v1,
                                       NJS_CNK_SIMPLE_BUF *v2, Uint32 *sq,
                                       Float tu0, Float tv0, Float tu1,
                                       Float tv1, Float tu2, Float tv2,
                                       Float ambient, Float side);
/* NOLINTEND(misc-use-internal-linkage) */

/* the offset and base colors that njCnkSimpleTextureBumpClip sets on each
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
        /* @hack spelling: the words stored as sq steps down through the       \
         * vertex, where indexed stores from sq would do */                    \
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

Uint32 *njCnkSimpleTextureBumpClip(Sint16 *strips, NJS_CNK_SIMPLE_BUF *buf,
                                   Uint32 nbstrip, Uint32 *sq, Float ambient,
                                   Float uvscale)
{
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
    /* @hack spelling: light written through lp, where light written in place
     * would do */
    NJS_VECTOR *lp;
    /* @hack redundant: register on every local from user_halfwords to ooz */
    register Sint32 user_halfwords;
    register Float *p;
    register NJS_CNK_SIMPLE_BUF *v0;
    register NJS_CNK_SIMPLE_BUF *v1;
    register NJS_CNK_SIMPLE_BUF *v2;
    register Sint32 len;
    register Sint32 calculated;
    register Sint32 drawn;
    register Sint32 sent;
    register Bool clipped;
    register Float model_side;
    register Float tv2;
    register Float side;
    register Float tu1;
    register Float tv1;
    register Float x;
    register Float y;
    register Float ooz;
    Float facing;
    Float tu0;
    Float tv0;
    Float tu2;

    /* the function makes view, frame and light as njCnkSimpleTextureBump
     * does */
    s = _nj_cnk_simple_bump_tex_vector_;
    t = _nj_cnk_simple_bump_tex_vector_ + BUMP_T;
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
    lp->x = (frame[0] * _nj_vertex_simple.light.x) +
            (frame[1] * _nj_vertex_simple.light.y) +
            (frame[2] * _nj_vertex_simple.light.z);
    lp->y = (frame[4] * _nj_vertex_simple.light.x) +
            (frame[5] * _nj_vertex_simple.light.y) +
            (frame[6] * _nj_vertex_simple.light.z);
    lp->z = (frame[8] * _nj_vertex_simple.light.x) +
            (frame[9] * _nj_vertex_simple.light.y) +
            (frame[10] * _nj_vertex_simple.light.z);
    njUnitVector(lp);
    base_color = _nj_cnk_simple_bump_lt_color_;
    offset_color = njCnkSimpleBumpLight(lp);

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
        tu0 = *strips++ * uvscale;
        tv0 = *strips++ * uvscale;
        v1 = &buf[*strips++];
        prefetch(v1);
        tu1 = *strips++ * uvscale;
        tv1 = *strips++ * uvscale;
        do {
            v2 = &buf[*strips++];
            prefetch(v2);
            tu2 = *strips++ * uvscale;
            tv2 = *strips++ * uvscale;
            /* @hack spelling: clipped set FALSE, then TRUE under a test,
             * where clipped set to the test's result would do */
            clipped = FALSE;
            if (v0->z <= CNK_NEAR_Z || v1->z <= CNK_NEAR_Z ||
                v2->z <= CNK_NEAR_Z) {
                clipped = TRUE;
                /* a triangle that isn't whole takes a facing above 0, which
                 * ends the run as facing away does */
                facing = 1.0F;
            } else {
                /* @hack redundant: (void)v1->y, a read nothing uses */
                facing = (((v1->x - v0->x) * (v2->y - v1->y)) -
                          ((v1->y - v0->y) * (v2->x - v1->x))) *
                         side,
                (void)v1->y;
            }
            if (facing < 0.0F) {
                if (sent == 0) {
                    SEND_VERTEX(v0, tu0, tv0, KM_VERTEXPARAM_NORMAL)
                    sent++;
                }
                SEND_VERTEX(v1, tu1, tv1, KM_VERTEXPARAM_NORMAL)
                /* @hack redundant: the store of y into sent under if (0) */
                if (0) {
                    sent = y;
                }
                sent++;
            } else if (sent != 0) {
                SEND_VERTEX(v1, tu1, tv1, KM_VERTEXPARAM_ENDOFSTRIP)
                sent++;
                drawn += sent;
                sent = 0;
            }
            /* @hack spelling: the test of the zs nested in the test of
             * clipped, where clipped && the zs' test would do */
            if (clipped) {
                if (v0->z > CNK_NEAR_Z || v1->z > CNK_NEAR_Z ||
                    v2->z > CNK_NEAR_Z) {
                    sq = njCnkSimpleTextureBumpNearClip(v0, v1, v2, sq, tu0,
                                                        tv0, tu1, tv1, tu2, tv2,
                                                        ambient, side);
                }
            }
            strips += user_halfwords;
            prefetch(strips + CNK_STRIP_AHEAD);
            side = -side;
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

/* a vertex of the triangle the near clip cuts, or of the strip it sends. sx and
 * sy place it on the screen, as a buffer entry's x and y do. (u, v) is its
 * texture coordinate. x, y and z, its point before projection, are set only in
 * a vertex of the triangle */
struct clip_vertex {
    Float sx;
    Float sy;
    Float ooz;
    Float x;
    Float y;
    Float z;
    Float u;
    Float v;
    /* @hack redundant: pad, which nothing reads */
    Float pad[2];
};

/* send the strip's vertex at sq, with param as its control word. sq is left
 * past the vertex */
#define SEND_CLIP_VERTEX(vertex, param)                                        \
    {                                                                          \
        /* @hack spelling: the words stored as sq steps down through the       \
         * vertex, where indexed stores from sq would do */                    \
        sq += CNK_BLOCK_WORDS;                                                 \
        *--sq = offset_color;                                                  \
        *--sq = base_color;                                                    \
        *(Float *)--sq = (vertex)->v;                                          \
        *(Float *)--sq = (vertex)->u;                                          \
        *(Float *)--sq = (vertex)->ooz;                                        \
        *(Float *)--sq = (vertex)->sy;                                         \
        *(Float *)--sq = (vertex)->sx;                                         \
        *--sq = (param);                                                       \
        prefetch(sq);                                                          \
        sq += CNK_BLOCK_WORDS;                                                 \
    }

/* load tri[k] from entry, with (tu, tv) as its texture coordinate */
#define LOAD_VERTEX(k, entry, tu, tv)                                          \
    {                                                                          \
        tri[k].sx = (entry)->x;                                                \
        tri[k].sy = (entry)->y;                                                \
        tri[k].ooz = (entry)->ooz;                                             \
        tri[k].z = (entry)->z;                                                 \
        tri[k].x = ((entry)->x - cx) * (entry)->z * inv_dist_x;                \
        tri[k].y = ((entry)->y - cy) * (entry)->z * inv_dist_y;                \
        tri[k].u = (tu);                                                       \
        tri[k].v = (tv);                                                       \
    }

Uint32 *njCnkSimpleTextureBumpNearClip(NJS_CNK_SIMPLE_BUF *v0,
                                       NJS_CNK_SIMPLE_BUF *v1,
                                       NJS_CNK_SIMPLE_BUF *v2, Uint32 *sq,
                                       Float tu0, Float tv0, Float tu1,
                                       Float tv1, Float tu2, Float tv2,
                                       Float ambient, Float side)
{
    struct clip_vertex tri[CNK_TRI_CORNERS];
    struct clip_vertex strip[CNK_CLIP_VERTICES];
    /* @hack redundant: register on every local but tri and strip */
    register Float dist_x;
    register Float dist_y;
    register Float cx;
    register Float cy;
    register Float inv_dist_x;
    register Float inv_dist_y;
    register Float t;
    register Float x;
    register Float y;
    register Int i;
    register Int prev;
    register Int n;

    if (_nj_control_3d_flag_ & NJD_CONTROL_3D_DISABLE_NEAR_CLIP) {
        return sq;
    }
    dist_x = _nj_screen_.dist * _nj_scr_aspect_x_;
    dist_y = _nj_screen_.dist * _nj_scr_aspect_y_;
    inv_dist_x = 1.0F / dist_x;
    inv_dist_y = 1.0F / dist_y;
    cx = _nj_screen_.cx;
    cy = _nj_screen_.cy;
    LOAD_VERTEX(0, v0, tu0, tv0)
    LOAD_VERTEX(1, v1, tu1, tv1)
    LOAD_VERTEX(2, v2, tu2, tv2)
    if (CNK_POINT_WINDING(&tri[0], &tri[1], &tri[2]) * side >= 0.0F) {
        return sq;
    }

    n = 0;
    for (prev = CNK_TRI_CORNERS - 1, i = 0; i < CNK_TRI_CORNERS; prev = i++) {
        /* the strip takes a vertex where the edge from prev to i crosses a z
         * of 1. t places that vertex on the edge, from 0 at prev to 1 at i */
        /* @hack spelling: the two tests negated and XORed, where
         * (tri[prev].z > CNK_NEAR_Z) != (tri[i].z > CNK_NEAR_Z) would do */
        if (!(tri[prev].z > CNK_NEAR_Z) ^ !(tri[i].z > CNK_NEAR_Z)) {
            t = (CNK_NEAR_Z - tri[prev].z) / (tri[i].z - tri[prev].z);
            /* @hack spelling: the sx and sy of a vertex on the cut summed into
             * x and y first, where stores of the sums would do */
            x = cx + ((tri[prev].x + ((tri[i].x - tri[prev].x) * t)) * dist_x);
            y = cy + ((tri[prev].y + ((tri[i].y - tri[prev].y) * t)) * dist_y);
            strip[n].sx = x;
            strip[n].sy = y;
            strip[n].ooz = CNK_NEAR_OOZ;
            strip[n].u = tri[prev].u + ((tri[i].u - tri[prev].u) * t);
            strip[n].v = tri[prev].v + ((tri[i].v - tri[prev].v) * t);
            n++;
        }
        if (tri[i].z > CNK_NEAR_Z) {
            /* @hack spelling: the kept vertex's sx and sy copied through x
             * and y, where direct copies would do */
            x = tri[i].sx;
            y = tri[i].sy;
            strip[n].sx = x;
            strip[n].sy = y;
            strip[n].ooz = tri[i].ooz;
            strip[n].u = tri[i].u;
            strip[n].v = tri[i].v;
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
