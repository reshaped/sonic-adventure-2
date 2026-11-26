#include <kamui.h>
#include <machine.h>
#include <ninja.h>
#include <nj_cnk.h>
#include <nj_global.h>
#include <sg_xpt.h>

/* a vertex of the triangle the near clip cuts, or of the strip it sends. sx
 * and sy place it on the screen, as a buffer entry's x and y do. x, y and z,
 * its point before projection, are set only in a vertex of the triangle. the
 * fields from inten to sx lie in the reverse of the order their words are
 * sent in. SEND_VERTEX reads them as a pointer steps up from inten. the near
 * clip writes them as one steps down from x */
struct clip_vertex {
    Float inten;
    Float v;
    Float u;
    Float ooz;
    Float sy;
    Float sx;
    Float x;
    Float y;
    Float z;
};

/* the most the base intensity shows. a vertex's offset intensity carries any
 * part of inten above it */
#define BASE_MAX 1.0F

/* send vertex at sq, with param as its control word. sq is left past the
 * vertex */
#define SEND_VERTEX(vertex, param)                                             \
    {                                                                          \
        /* @hack spelling: the fields read as p steps up from inten, where     \
         * reads of vertex's fields would do */                                \
        p = &(vertex)->inten;                                                  \
        inten = *p++;                                                          \
        /* @hack spelling: the words stored as sq steps back from the          \
         * vertex's end, where indexed stores from sq would do */              \
        sq += CNK_BLOCK_WORDS;                                                 \
        *(Float *)--sq = inten - BASE_MAX;                                     \
        *(Float *)--sq = inten;                                                \
        *(Float *)--sq = *p++;                                                 \
        *(Float *)--sq = *p++;                                                 \
        *(Float *)--sq = *p++;                                                 \
        *(Float *)--sq = *p++;                                                 \
        *(Float *)--sq = *p;                                                   \
        *--sq = (param);                                                       \
        prefetch(sq);                                                          \
        sq += CNK_BLOCK_WORDS;                                                 \
    }

/* clang-format off */
#ifdef DREAMCAST
/* the body sends each vertex as njCnkEasyTexture does */
/* @asm operand: integer-constant, MOV.L ?mask,R1 at 0x8c142e48 loads the
 * mask of the strip count from the pool */
#pragma inline_asm(njCnkEasyTextureClip)
Uint32 *njCnkEasyTextureClip(Sint16 *strips, NJS_CNK_EASY_BUF *buf,
                             Uint32 nbstrip, Uint32 *sq, Float uvscale)
{
        .IMPORT     __nj_calc_polygon_
        MOV.L       ?mask,R1
        FMOV.S      FR15,@-R15
        FMOV.S      FR14,@-R15
        FMOV.S      FR13,@-R15
        FMOV.S      FR12,@-R15
        MOV.L       R14,@-R15
        MOV.L       R13,@-R15
        MOV.L       R12,@-R15
        MOV.L       R11,@-R15
        MOV.L       R10,@-R15
        MOV.L       R9,@-R15
        MOV.L       R8,@-R15
        STS.L       PR,@-R15
        MOV         R4,R2
        ADD         #6,R2
        PREF        @R2
        MOV         R6,R3
        AND         R1,R3
        MOV         #-14,R2
        SHLD        R2,R6
        MOV         #3,R2
        AND         R2,R6
        SHLL        R6
        MOV         #0,R1
        MOV         #0,R2
        MOV.L       ?vertex_control,R8
        MOV.L       ?end_control,R9
        FMOV.S      FR4,FR3
        FLDI1       FR2
?strip:
        MOV.W       @R4+,R10
        MOV.W       @R4+,R11
        SHLL2       R11
        MOV.W       @R4+,R0
        SHLL2       R11
        LDS         R0,FPUL
        ADD         R5,R11
        MOV.W       @R4+,R0
        PREF        @R11
        CMP/PZ      R10
        BT          ?forward
        NEG         R10,R10
?forward:
        ADD         R10,R2
        ADD         #-2,R10
        MOV         #0,R14
        FLOAT       FPUL,FR10
        FMUL        FR3,FR10
        LDS         R0,FPUL
        FLOAT       FPUL,FR11
        FMUL        FR3,FR11
        MOV         #8,R0
        FMOV.S      @(R0,R11),FR5
        MOV.W       @R4+,R12
        SHLL2       R12
        MOV.W       @R4+,R0
        SHLL2       R12
        LDS         R0,FPUL
        ADD         R5,R12
        MOV.W       @R4+,R0
        PREF        @R12
        FLOAT       FPUL,FR12
        FMUL        FR3,FR12
        LDS         R0,FPUL
        FLOAT       FPUL,FR13
        FMUL        FR3,FR13
        MOV         #8,R0
        FMOV.S      @(R0,R12),FR7
?triangle:
        MOV.W       @R4+,R13
        SHLL2       R13
        MOV.W       @R4+,R0
        SHLL2       R13
        LDS         R0,FPUL
        ADD         R5,R13
        MOV.W       @R4+,R0
        FLOAT       FPUL,FR14
        PREF        @R13
        FMUL        FR3,FR14
        FLDI0       FR0
        LDS         R0,FPUL
        FCMP/GT     FR0,FR5
        FLOAT       FPUL,FR15
        MOV         #8,R0
        FMUL        FR3,FR15
        BF/S        ?not_whole
        FMOV.S      @(R0,R13),FR9
        FCMP/GT     FR0,FR7
        BF          ?not_whole
        FCMP/GT     FR0,FR9
        BF          ?not_whole
        FCMP/GT     FR2,FR5
        BT          ?not_whole
        FCMP/GT     FR2,FR7
        BT          ?not_whole
        FCMP/GT     FR2,FR9
        BT          ?not_whole
        TST         R14,R14
        ADD         #32,R7
        BF          ?open_v1
        MOV         R11,R0
        FMOV.S      @R0+,FR6
        FMOV.S      @R0+,FR4
        ADD         #4,R0
        FMOV.S      @R0+,FR0
        FMOV.S      FR0,FR1
        FSCHG
        FSUB        FR2,FR1
        FMOV        DR0,@-R7
        FMOV        DR10,@-R7
        ADD         #1,R14
        FMOV        DR4,@-R7
        FSCHG
        FMOV.S      FR6,@-R7
        MOV.L       R8,@-R7
        PREF        @R7
        ADD         #64,R7
?open_v1:
        MOV         R12,R0
        FMOV.S      @R0+,FR4
        FMOV.S      @R0+,FR6
        ADD         #4,R0
        FMOV.S      @R0+,FR0
        FMOV.S      FR0,FR1
        FSCHG
        FSUB        FR2,FR1
        FMOV        DR0,@-R7
        FMOV        DR12,@-R7
        ADD         #1,R14
        FMOV        DR6,@-R7
        FSCHG
        FMOV.S      FR4,@-R7
        MOV.L       R8,@-R7
        PREF        @R7
        BRA         ?next
        ADD         #32,R7
?not_whole:
        TST         R14,R14
        BT          ?near_v0
        MOV         R12,R0
        FMOV.S      @R0+,FR4
        FMOV.S      @R0+,FR6
        ADD         #4,R0
        FMOV.S      @R0+,FR0
        ADD         #32,R7
        FMOV.S      FR0,FR1
        FSCHG
        FSUB        FR2,FR1
        FMOV        DR0,@-R7
        FMOV        DR12,@-R7
        ADD         R14,R1
        FMOV        DR6,@-R7
        MOV         #0,R14
        FSCHG
        ADD         #1,R1
        FMOV.S      FR4,@-R7
        MOV.L       R9,@-R7
        PREF        @R7
        ADD         #32,R7
?near_v0:
        FLDI0       FR0
        FCMP/GT     FR0,FR5
        BF          ?near_v1
        FCMP/GT     FR5,FR2
        BT          ?clip
?near_v1:
        FCMP/GT     FR0,FR7
        BF          ?near_v2
        FCMP/GT     FR7,FR2
        BT          ?clip
?near_v2:
        FCMP/GT     FR0,FR9
        BF          ?next
        FCMP/GT     FR9,FR2
        BT          ?clip
?next:
        MOV         R12,R11
        ADD         R6,R4
        MOV         R13,R12
        ADD         #6,R4
        FSCHG
        PREF        @R4
        FMOV        DR12,DR10
        ADD         #-6,R4
        FMOV        DR14,DR12
        FSCHG
        DT          R10
        FMOV.S      FR7,FR5
        BF/S        ?triangle
        FMOV.S      FR9,FR7
        TST         R14,R14
        BT          ?strip_done
        MOV         R13,R0
        FMOV.S      @R0+,FR4
        FMOV.S      @R0+,FR8
        ADD         #4,R0
        FMOV.S      @R0+,FR0
        ADD         #32,R7
        FMOV.S      FR0,FR1
        FSCHG
        FSUB        FR2,FR1
        FMOV        DR0,@-R7
        FMOV        DR14,@-R7
        ADD         R14,R1
        FMOV        DR8,@-R7
        FSCHG
        FMOV.S      FR4,@-R7
        MOV.L       R9,@-R7
        ADD         #1,R1
        PREF        @R7
        ADD         #32,R7
?strip_done:
        DT          R3
        BT          ?done
        BRA         ?strip
        NOP
?done:
        MOV.L       ?draw,R3
        MOV.L       ?calc,R4
        MOV.L       @R3,R5
        MOV.L       @R4,R6
        ADD         R1,R5
        ADD         R2,R6
        MOV.L       R5,@R3
        MOV.L       R6,@R4
        MOV         R7,R0
        LDS.L       @R15+,PR
        MOV.L       @R15+,R8
        MOV.L       @R15+,R9
        MOV.L       @R15+,R10
        MOV.L       @R15+,R11
        MOV.L       @R15+,R12
        MOV.L       @R15+,R13
        MOV.L       @R15+,R14
        FMOV.S      @R15+,FR12
        FMOV.S      @R15+,FR13
        FMOV.S      @R15+,FR14
        RTS
        FMOV.S      @R15+,FR15
?clip:
        MOV.L       R6,@-R15
        MOV.L       R5,@-R15
        MOV.L       R4,@-R15
        MOV.L       R3,@-R15
        MOV.L       R2,@-R15
        MOV.L       R1,@-R15
        FMOV.S      FR3,@-R15
        FMOV.S      FR7,@-R15
        FMOV.S      FR9,@-R15
        MOV.L       ?near_clip,R0
        FMOV.S      FR10,FR4
        FMOV.S      FR11,FR5
        FMOV.S      FR12,FR6
        FMOV.S      FR13,FR7
        FMOV.S      FR14,FR8
        FMOV.S      FR15,FR9
        MOV         R11,R4
        MOV         R12,R5
        JSR         @R0
        MOV         R13,R6
        FMOV.S      @R15+,FR9
        FMOV.S      @R15+,FR7
        FMOV.S      @R15+,FR3
        MOV.L       @R15+,R1
        MOV.L       @R15+,R2
        MOV.L       @R15+,R3
        MOV.L       @R15+,R4
        MOV.L       @R15+,R5
        MOV.L       @R15+,R6
        FLDI1       FR2
        BRA         ?next
        MOV         R0,R7
        .ALIGN      4
?mask:
        .DATA.L     H'00003FFF
?vertex_control:
        .DATA.L     H'E0000000
?end_control:
        .DATA.L     H'F0000000
?draw:
        .DATA.L     __nj_draw_polygon_
?calc:
        .DATA.L     __nj_calc_polygon_
?near_clip:
        .DATA.L     _njCnkEasyTextureNearClip
}
#endif
/* clang-format on */

Uint32 *njCnkEasyTextureNearClip(NJS_CNK_EASY_BUF *v0, NJS_CNK_EASY_BUF *v1,
                                 NJS_CNK_EASY_BUF *v2, Uint32 *sq, Float tu0,
                                 Float tv0, Float tu1, Float tv1, Float tu2,
                                 Float tv2)
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
    register Float inten;
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
    register Float *p;
    register Float *q;
    register Int i;
    register Int prev;
    register Int n;

    if (_nj_control_3d_flag_ & NJD_CONTROL_3D_DISABLE_NEAR_CLIP) {
        return sq;
    }
    /* @hack spelling: cx set ahead of inv_dist_x and cy ahead of
     * inv_dist_y, not both after them */
    dist_x = _nj_screen_.dist * _nj_scr_aspect_x_;
    dist_y = _nj_screen_.dist * _nj_scr_aspect_y_;
    cx = _nj_screen_.cx;
    inv_dist_x = 1.0F / dist_x;
    cy = _nj_screen_.cy;
    inv_dist_y = 1.0F / dist_y;
    tri[0].z = 1.0F / v0->ooz;
    tri[0].inten = v0->inten;
    tri[0].u = tu0;
    tri[0].v = tv0;
    tri[0].sx = v0->x;
    tri[0].sy = v0->y;
    tri[0].ooz = v0->ooz;
    tri[0].x = (v0->x - cx) * tri[0].z * inv_dist_x;
    tri[0].y = (v0->y - cy) * tri[0].z * inv_dist_y;
    tri[1].z = 1.0F / v1->ooz;
    tri[1].inten = v1->inten;
    tri[1].u = tu1;
    tri[1].v = tv1;
    tri[1].sx = v1->x;
    tri[1].sy = v1->y;
    tri[1].ooz = v1->ooz;
    tri[1].x = (v1->x - cx) * tri[1].z * inv_dist_x;
    tri[1].y = (v1->y - cy) * tri[1].z * inv_dist_y;
    tri[2].z = 1.0F / v2->ooz;
    tri[2].inten = v2->inten;
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
            if (_nj_cnk_easy_vtype_ != NJD_CV_D8) {
                *--q = tri[prev].inten + ((tri[i].inten - tri[prev].inten) * t);
            } else {
                /* each byte of strip[n]'s ARGB word lies t of the way from
                 * prev's byte to i's. the conversion drops the byte's
                 * fraction */
                /* @hack type: the words read and written through each
                 * inten's address, where a union of a float and a word would
                 * do */
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
                a = a0 + ((a1 - a0) * t);
                r = r0 + ((r1 - r0) * t);
                g = g0 + ((g1 - g0) * t);
                b = b0 + ((b1 - b0) * t);
                argb = ((Uint32)a << ARGB_ALPHA_SHIFT) |
                       ((Uint32)r << ARGB_RED_SHIFT) |
                       ((Uint32)g << ARGB_GREEN_SHIFT) | (Uint32)b;
                *(Uint32 *)--q = argb;
            }
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
    /* flat shading needs an inten only at strip[2] and strip[3], where the
     * strip's triangles end */
    if (_nj_flat_model_flag_) {
        strip[2].inten = tri[2].inten;
        strip[3].inten = tri[2].inten;
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
