/* every texture near clip nj_cnk.h declares compiles from this source. built
 * alone, it is njCnkSimpleTextureNearClip. each other near clip is a variant.
 * its source renames that function, defines the NEAR_CLIP_ hooks and includes
 * this source. a variant whose parameters differ from this source's includes
 * nj_cnk.h before the rename. otherwise the header's declaration of
 * njCnkSimpleTextureNearClip would declare the variant a second time, with
 * other parameters */
#include <kamui.h>
#include <machine.h>
#include <ninja.h>
#include <nj_cnk.h>
/* only the Env variants' UV_OFFSETS reads nj_global.h's names */
#include <nj_global.h> /* IWYU pragma: keep -- the offsets UV_OFFSETS reads */
#include <sg_xpt.h>

/* the NEAR_CLIP_ hooks hold what the near clips don't share. tri[k] is corner
 * k of the triangle a near clip cuts. strip[n] is vertex n of the strip it
 * sends. the hooks below are njCnkSimpleTextureNearClip's */
#ifndef NEAR_CLIP_PARAMS
/* the parameters after sq, each led by its comma */
#define NEAR_CLIP_PARAMS UV_PARAMS, Float ambient, Float side
/* the locals only some near clips need */
#define NEAR_CLIP_LOCALS
/* set an Env near clip's uoff and voff, before tri is loaded */
#define NEAR_CLIP_START
/* set tri[k]'s inten and spec from its entry */
#define NEAR_CLIP_COLOR LIT_COLOR
/* set tri[k]'s texture coordinate */
#define NEAR_CLIP_UV PARAM_UV
/* drop the triangle, or light its corners, by its facing */
#define NEAR_CLIP_FACING DROP_BACK
/* set the inten and spec of strip[n], a vertex on the cut */
#define NEAR_CLIP_CUT_COLOR LERP_LIT
#endif

/* the texture coordinates of v0, v1 and v2, as the parameters tu0 to tv2 */
#define UV_PARAMS                                                              \
    , Float tu0, Float tv0, Float tu1, Float tv1, Float tu2, Float tv2

/* @hack redundant: register on uoff and voff, the offsets UV_OFFSETS sets */
#define ENV_LOCALS                                                             \
    register Float uoff;                                                       \
    register Float voff;
/* @hack redundant: register on LERP_ARGB's locals */
#define ARGB_LOCALS                                                            \
    register Float a0;                                                         \
    register Float r0;                                                         \
    register Float g0;                                                         \
    register Float b0;                                                         \
    register Float a1;                                                         \
    register Float r1;                                                         \
    register Float g1;                                                         \
    register Float b1;                                                         \
    register Float a;                                                          \
    register Float r;                                                          \
    register Float g;                                                          \
    register Float b;                                                          \
    register Uint32 argb;

/* set uoff and voff, the offsets NORMAL_UV adds */
#define UV_OFFSETS                                                             \
    if (_nj_control_3d_flag_ & NJD_CONTROL_3D_ENV_UV_SCROLL) {                 \
        uoff = _nj_uv_scroll_u_ + CNK_ENV_CENTER;                              \
        voff = _nj_uv_scroll_v_ + CNK_ENV_CENTER;                              \
    } else {                                                                   \
        uoff = CNK_ENV_CENTER;                                                 \
        voff = CNK_ENV_CENTER;                                                 \
    }

/* a vertex of the triangle the near clip cuts, or of the strip it sends. sx and
 * sy place it on the screen, as a buffer entry's x and y do. (u, v) is its
 * texture coordinate, and inten and spec are sent as its words 6 and 7. x, y
 * and z, its point before projection, are set only in a vertex of the
 * triangle */
struct clip_vertex {
    Float sx;
    Float sy;
    Float ooz;
    Float x;
    Float y;
    Float z;
    Float u;
    Float v;
    Float inten;
    Float spec;
};

/* NEAR_CLIP_COLOR's forms. LIT_COLOR sets the lit inten and copies the entry's
 * spec, as an inten function sends them. PACKED_COLOR copies a D8 entry's
 * packed ARGB words. njCnkSimpleTextureDbNearClip and
 * njCnkSimpleTextureEnvDbNearClip define NEAR_CLIP_COLOR empty. LIGHT_BY_FACING
 * sets their colors instead */
#define LIT_COLOR(k, entry)                                                    \
    {                                                                          \
        tri[k].inten = (entry)->inten + ambient;                               \
        if (tri[k].inten < ambient) {                                          \
            tri[k].inten = ambient;                                            \
        }                                                                      \
        tri[k].spec = (entry)->spec;                                           \
    }
#define PACKED_COLOR(k, entry)                                                 \
    {                                                                          \
        tri[k].inten = (entry)->inten;                                         \
        tri[k].spec = (entry)->spec;                                           \
    }
/* NEAR_CLIP_UV's forms. PARAM_UV sets tri[k]'s texture coordinate to (tu, tv).
 * NORMAL_UV ignores them. it sets the u and v an Env function sends, from the
 * entry's nx and ny */
#define PARAM_UV(k, entry, tu, tv)                                             \
    {                                                                          \
        tri[k].u = (tu);                                                       \
        tri[k].v = (tv);                                                       \
    }
#define NORMAL_UV(k, entry, tu, tv)                                            \
    {                                                                          \
        tri[k].u = uoff + ((entry)->nx * CNK_ENV_SCALE);                       \
        tri[k].v = voff + ((entry)->ny * CNK_ENV_SCALE);                       \
    }
/* load tri[k] from entry, with (tu, tv) as its texture coordinate where the
 * near clip takes one */
#define LOAD_VERTEX(k, entry, tu, tv)                                          \
    {                                                                          \
        tri[k].sx = (entry)->x;                                                \
        tri[k].sy = (entry)->y;                                                \
        tri[k].ooz = (entry)->ooz;                                             \
        tri[k].z = (entry)->z;                                                 \
        tri[k].x = ((entry)->x - cx) * (entry)->z * inv_dist_x;                \
        tri[k].y = ((entry)->y - cy) * (entry)->z * inv_dist_y;                \
        NEAR_CLIP_COLOR(k, entry)                                              \
        NEAR_CLIP_UV(k, entry, tu, tv)                                         \
    }

/* NEAR_CLIP_FACING's forms. DROP_BACK returns sq, having sent nothing, where
 * the triangle faces away. LIGHT_BY_FACING sets each tri[k]'s inten and spec as
 * an inten function for NJD_FST_DB strips sends them. where the triangle faces
 * away, it lights each corner from its entry's negated inten and spec */
#define DROP_BACK                                                              \
    if (CNK_POINT_WINDING(&tri[0], &tri[1], &tri[2]) * side >= 0.0F) {         \
        return sq;                                                             \
    }
#define LIGHT_BY_FACING                                                        \
    if (CNK_POINT_WINDING(&tri[0], &tri[1], &tri[2]) * side >= 0.0F) {         \
        tri[0].inten = ambient - v0->inten;                                    \
        tri[1].inten = ambient - v1->inten;                                    \
        tri[2].inten = ambient - v2->inten;                                    \
        tri[0].spec = -v0->spec;                                               \
        tri[1].spec = -v1->spec;                                               \
        tri[2].spec = -v2->spec;                                               \
    } else {                                                                   \
        tri[0].inten = ambient + v0->inten;                                    \
        tri[1].inten = ambient + v1->inten;                                    \
        tri[2].inten = ambient + v2->inten;                                    \
        tri[0].spec = v0->spec;                                                \
        tri[1].spec = v1->spec;                                                \
        tri[2].spec = v2->spec;                                                \
    }                                                                          \
    if (tri[0].inten < ambient) {                                              \
        tri[0].inten = ambient;                                                \
    }                                                                          \
    if (tri[1].inten < ambient) {                                              \
        tri[1].inten = ambient;                                                \
    }                                                                          \
    if (tri[2].inten < ambient) {                                              \
        tri[2].inten = ambient;                                                \
    }

/* dst, src0 and src1 are Floats that hold packed ARGB words' bits. set each
 * byte of dst t of the way from src0's byte to src1's, rounded toward 0 */
/* @hack type: the words read and written through the floats' addresses,
 * where a union of a float and a word would do */
#define LERP_ARGB(dst, src0, src1)                                             \
    {                                                                          \
        argb = *(Uint32 *)&(src0);                                             \
        a0 = (argb >> ARGB_ALPHA_SHIFT) & 0xff;                                \
        r0 = (argb >> ARGB_RED_SHIFT) & 0xff;                                  \
        g0 = (argb >> ARGB_GREEN_SHIFT) & 0xff;                                \
        b0 = argb & 0xff;                                                      \
        argb = *(Uint32 *)&(src1);                                             \
        a1 = (argb >> ARGB_ALPHA_SHIFT) & 0xff;                                \
        r1 = (argb >> ARGB_RED_SHIFT) & 0xff;                                  \
        g1 = (argb >> ARGB_GREEN_SHIFT) & 0xff;                                \
        b1 = argb & 0xff;                                                      \
        a = a0 + ((a1 - a0) * t);                                              \
        r = r0 + ((r1 - r0) * t);                                              \
        g = g0 + ((g1 - g0) * t);                                              \
        b = b0 + ((b1 - b0) * t);                                              \
        argb = ((Uint32)a << ARGB_ALPHA_SHIFT) |                               \
               ((Uint32)r << ARGB_RED_SHIFT) |                                 \
               ((Uint32)g << ARGB_GREEN_SHIFT) | (Uint32)b;                    \
        *(Uint32 *)&(dst) = argb;                                              \
    }
/* NEAR_CLIP_CUT_COLOR's forms, which set strip[n]'s inten and spec t of the way
 * from tri[prev]'s to tri[i]'s. LERP_LIT interpolates the lit values, and
 * LERP_PACKED each byte of a D8 near clip's packed ARGB words */
#define LERP_LIT                                                               \
    strip[n].inten = tri[prev].inten + ((tri[i].inten - tri[prev].inten) * t); \
    strip[n].spec = tri[prev].spec + ((tri[i].spec - tri[prev].spec) * t);
#define LERP_PACKED                                                            \
    LERP_ARGB(strip[n].inten, tri[prev].inten, tri[i].inten)                   \
    LERP_ARGB(strip[n].spec, tri[prev].spec, tri[i].spec)

/* send vertex at sq, with param as its control word. sq is left past the
 * vertex */
#define SEND_VERTEX(vertex, param)                                             \
    {                                                                          \
        /* @hack spelling: the words stored as sq steps down through the       \
         * vertex, where indexed stores from sq would do */                    \
        sq += CNK_BLOCK_WORDS;                                                 \
        *(Float *)--sq = (vertex)->spec;                                       \
        *(Float *)--sq = (vertex)->inten;                                      \
        *(Float *)--sq = (vertex)->v;                                          \
        *(Float *)--sq = (vertex)->u;                                          \
        *(Float *)--sq = (vertex)->ooz;                                        \
        *(Float *)--sq = (vertex)->sy;                                         \
        *(Float *)--sq = (vertex)->sx;                                         \
        *--sq = (param);                                                       \
        prefetch(sq);                                                          \
        sq += CNK_BLOCK_WORDS;                                                 \
    }

Uint32 *njCnkSimpleTextureNearClip(NJS_CNK_SIMPLE_BUF *v0,
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
    NEAR_CLIP_LOCALS
    register Int i;
    register Int prev;
    register Int n;

    if (_nj_control_3d_flag_ & NJD_CONTROL_3D_DISABLE_NEAR_CLIP) {
        return sq;
    }
    NEAR_CLIP_START
    dist_x = _nj_screen_.dist * _nj_scr_aspect_x_;
    dist_y = _nj_screen_.dist * _nj_scr_aspect_y_;
    inv_dist_x = 1.0F / dist_x;
    inv_dist_y = 1.0F / dist_y;
    cx = _nj_screen_.cx;
    cy = _nj_screen_.cy;
    LOAD_VERTEX(0, v0, tu0, tv0)
    LOAD_VERTEX(1, v1, tu1, tv1)
    LOAD_VERTEX(2, v2, tu2, tv2)
    NEAR_CLIP_FACING

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
            NEAR_CLIP_CUT_COLOR
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
            strip[n].inten = tri[i].inten;
            strip[n].spec = tri[i].spec;
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
