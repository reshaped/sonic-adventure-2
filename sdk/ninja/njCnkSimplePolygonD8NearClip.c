#include <kamui.h>
#include <machine.h>
#include <ninja.h>
#include <nj_cnk.h>
#include <sg_xpt.h>

/* the text of njCnkSimplePolygonD8NearClip and njCnkSimplePolygonD8DbNearClip.
 * njCnkSimplePolygonD8DbNearClip.c includes nj_cnk.h, then renames the
 * function, defines NEAR_CLIP_PARAMS and NEAR_CLIP_FACING and includes this
 * text. nj_cnk.h comes first: renamed, its declaration of
 * njCnkSimplePolygonD8NearClip would declare njCnkSimplePolygonD8DbNearClip
 * again, with side among its parameters. NEAR_CLIP_PARAMS is any parameter
 * after sq, with its comma. NEAR_CLIP_FACING is the statement that runs once
 * the triangle's points are found. the values below are
 * njCnkSimplePolygonD8NearClip's */
#ifndef NEAR_CLIP_PARAMS
#define NEAR_CLIP_PARAMS , Float side
#define NEAR_CLIP_FACING DROP_BACK
#endif

/* return without sending anything where the triangle faces away */
#define DROP_BACK                                                              \
    if (CNK_POINT_WINDING(&tri[0], &tri[1], &tri[2]) * side >= 0.0F) {         \
        return sq;                                                             \
    }

/* a vertex of the triangle the near clip cuts, or of the strip it sends. sx and
 * sy place it on the screen, as a buffer entry's x and y do. inten holds a
 * packed ARGB word's bits. x, y and z, its point before projection, are set
 * only in a vertex of the triangle */
struct clip_vertex {
    Float sx;
    Float sy;
    Float ooz;
    Float x;
    Float y;
    Float z;
    Float inten;
};

/* send v at sq, with param as its control word. words 4, 5 and 7 keep whatever
 * the store queue held. sq is left past the vertex */
#define SEND_VERTEX(v, param)                                                  \
    {                                                                          \
        /* @hack spelling: the words stored as sq steps down through the       \
         * vertex, where indexed stores from sq would do */                    \
        sq += CNK_SPEC_WORD;                                                   \
        *(Float *)--sq = (v)->inten;                                           \
        sq -= CNK_UV_WORDS;                                                    \
        *(Float *)--sq = (v)->ooz;                                             \
        *(Float *)--sq = (v)->sy;                                              \
        *(Float *)--sq = (v)->sx;                                              \
        *--sq = (param);                                                       \
        prefetch(sq);                                                          \
        sq += CNK_BLOCK_WORDS;                                                 \
    }

Uint32 *njCnkSimplePolygonD8NearClip(NJS_CNK_SIMPLE_BUF *v0,
                                     NJS_CNK_SIMPLE_BUF *v1,
                                     NJS_CNK_SIMPLE_BUF *v2,
                                     Uint32 *sq NEAR_CLIP_PARAMS)
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
    register Float a0;
    register Float r0;
    register Float g0;
    register Float b0;
    register Float a1;
    register Float r1;
    register Float g1;
    register Float b1;
    register Float a;
    register Float r;
    register Float g;
    register Float b;
    register Uint32 argb;
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
    tri[0].sx = v0->x;
    tri[0].sy = v0->y;
    tri[0].ooz = v0->ooz;
    tri[0].z = v0->z;
    tri[0].x = (v0->x - cx) * v0->z * inv_dist_x;
    tri[0].y = (v0->y - cy) * v0->z * inv_dist_y;
    tri[0].inten = v0->inten;
    tri[1].sx = v1->x;
    tri[1].sy = v1->y;
    tri[1].ooz = v1->ooz;
    tri[1].z = v1->z;
    tri[1].x = (v1->x - cx) * v1->z * inv_dist_x;
    tri[1].y = (v1->y - cy) * v1->z * inv_dist_y;
    tri[1].inten = v1->inten;
    tri[2].sx = v2->x;
    tri[2].sy = v2->y;
    tri[2].ooz = v2->ooz;
    tri[2].z = v2->z;
    tri[2].x = (v2->x - cx) * v2->z * inv_dist_x;
    tri[2].y = (v2->y - cy) * v2->z * inv_dist_y;
    tri[2].inten = v2->inten;
    NEAR_CLIP_FACING

    n = 0;
    for (prev = CNK_TRI_CORNERS - 1, i = 0; i < CNK_TRI_CORNERS;
         prev = i, i++) {
        /* the strip takes a vertex where the edge from prev to i crosses a z
         * of 1. t places that vertex on the edge, from 0 at prev to 1 at i */
        /* @hack spelling: the two tests negated and XORed, where
         * (tri[prev].z > CNK_NEAR_Z) != (tri[i].z > CNK_NEAR_Z) would do */
        if (!(tri[prev].z > CNK_NEAR_Z) ^ !(tri[i].z > CNK_NEAR_Z)) {
            t = (CNK_NEAR_Z - tri[prev].z) / (tri[i].z - tri[prev].z);
            x = tri[prev].x + (t * (tri[i].x - tri[prev].x));
            y = tri[prev].y + (t * (tri[i].y - tri[prev].y));
            strip[n].sx = cx + (x * dist_x);
            strip[n].sy = cy + (y * dist_y);
            strip[n].ooz = CNK_NEAR_OOZ;
            /* each byte of strip[n]'s ARGB word lies t of the way from prev's
             * byte to i's. the conversion drops the byte's fraction */
            /* @hack type: the words read and written through each inten's
             * address, where a union of a float and a word would do */
            argb = *(Uint32 *)&tri[prev].inten;
            a0 = (argb >> ARGB_ALPHA_SHIFT) & 0xff;
            r0 = (argb >> ARGB_RED_SHIFT) & 0xff;
            g0 = (argb >> ARGB_GREEN_SHIFT) & 0xff;
            b0 = argb & 0xff;
            argb = *(Uint32 *)&tri[i].inten;
            a1 = (argb >> ARGB_ALPHA_SHIFT) & 0xff;
            r1 = (argb >> ARGB_RED_SHIFT) & 0xff;
            g1 = (argb >> ARGB_GREEN_SHIFT) & 0xff;
            b1 = argb & 0xff;
            a = a0 + (t * (a1 - a0));
            r = r0 + (t * (r1 - r0));
            g = g0 + (t * (g1 - g0));
            b = b0 + (t * (b1 - b0));
            argb = ((Uint32)a << ARGB_ALPHA_SHIFT) |
                   ((Uint32)r << ARGB_RED_SHIFT) |
                   ((Uint32)g << ARGB_GREEN_SHIFT) | (Uint32)b;
            *(Uint32 *)&strip[n].inten = argb;
            n++;
        }
        if (tri[i].z > CNK_NEAR_Z) {
            strip[n].sx = tri[i].sx;
            strip[n].sy = tri[i].sy;
            strip[n].ooz = tri[i].ooz;
            strip[n].inten = tri[i].inten;
            n++;
        }
    }
    SEND_VERTEX(&strip[0], KM_VERTEXPARAM_NORMAL)
    SEND_VERTEX(&strip[1], KM_VERTEXPARAM_NORMAL)
    if (n != CNK_TRI_CORNERS) {
        SEND_VERTEX(&strip[3], KM_VERTEXPARAM_NORMAL)
    }
    SEND_VERTEX(&strip[2], KM_VERTEXPARAM_ENDOFSTRIP)
    _nj_draw_polygon_ += n;
    return sq;
}
