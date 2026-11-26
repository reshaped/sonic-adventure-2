#include <kamui.h>
#include <machine.h>
#include <ninja.h>
#include <nj_draw3d.h>
#include <nj_global.h>
#include <nj_km.h>
#include <sg_xpt.h>

/* the member exports these functions, though no header of the SDK declares
 * them */
/* NOLINTBEGIN(misc-use-internal-linkage) */
/* project count vertices of p, 1 or more, into buf as njDCalcPoint3DP2
 * projects points. scale_x and scale_y stand for its sx and sy. the call
 * returns as njDCalcPoint3DP2 does, and each vertex keeps its own col */
Int njCalcPoint3DP(NJS_POLYGON_VTX *p, Int count, struct draw3d_vertex *buf,
                   Float scale_x, Float scale_y, Float cx, Float cy);
/* send the strip of vtx's count vertices from sq on. the call returns the
 * address after the last vertex sent. a vertex is in front where its ooz lies
 * between 0 and 1 over _nj_near_clip_. a triangle with its three vertices in
 * front joins a strip of such triangles. one with one or two in front goes to
 * njPolygonNearClip3DP. one with none is dropped */
Uint32 *njPolygonClip3DP(struct draw3d_vertex *vtx, Int count, Uint32 *sq);
/* cut the triangle of a, b and c at _nj_near_clip_. the part beyond goes from
 * sq on as a strip of 3 or 4 vertices. the call returns the address after the
 * strip. at least one of the three must lie beyond. a vertex the cut adds
 * takes 1 over _nj_near_clip_ as its ooz. each byte of its color lies between
 * its edge's ends as the vertex does */
Uint32 *njPolygonNearClip3DP(struct draw3d_vertex *a, struct draw3d_vertex *b,
                             struct draw3d_vertex *c, Uint32 *sq);
/* NOLINTEND(misc-use-internal-linkage) */

/* clang-format off */
#ifdef DREAMCAST
/* njCalcPoint3DP projects the vertices into _nj_vertex_buf_. nothing is
 * drawn where one lies beyond the far clip. any trans but 0 draws
 * translucent, where njDrawPolygon3DExStart takes only 1. ?ex_trans_mode and
 * ?ex_opaque_mode hold NJD_COLOR_MODE_PACKED_NORMAL_TRANS and
 * NJD_COLOR_MODE_PACKED_NORMAL.
 *
 * where a vertex lies at or nearer than the near clip, njPolygonClip3DP sends
 * the strip. otherwise each vertex goes out as a 32-byte block. its word 0 is
 * KM_VERTEXPARAM_NORMAL, or KM_VERTEXPARAM_ENDOFSTRIP for the last vertex,
 * which ?ex_cmd_vertex and ?ex_cmd_end hold. words 1 to 3 hold the vertex's
 * sx, sy and ooz, and word 6 its col. words 4 and 5, u and v, stay
 * unwritten. the color's pair store fills word 7, the offset color, with
 * whatever FR5 holds */
/* @asm epilogue: dead-return, SHC's RTS at 0x8c12b270 follows the body's own
 * return at 0x8c12b22a and its pool */
#pragma inline_asm(njDrawPolygon3DEx)
void njDrawPolygon3DEx(NJS_POLYGON_VTX *p, Int count, Int trans)
{
        .IMPORT     __nj_vertex_buf_
        MOV.L       R11,@-R15
        MOV.L       R10,@-R15
        MOV.L       R9,@-R15
        MOV.L       R8,@-R15
        STS.L       PR,@-R15
        MOV         R6,R8
        MOV         R5,R11
        MOV.L       ?ex_screen,R1
        MOV.L       ?ex_aspect_x,R2
        MOV.L       ?ex_aspect_y,R3
        FMOV.S      @R1,FR3
        FMOV.S      @R2,FR4
        ADD         #12,R1
        FMOV.S      @R3,FR5
        FMUL        FR3,FR4
        FMOV.S      @R1+,FR6
        FMUL        FR3,FR5
        MOV.L       ?ex_vertex_buf,R6
        MOV.L       @R6,R6
        MOV.L       ?ex_calc_point,R3
        JSR         @R3
        FMOV.S      @R1+,FR7
        SHLL        R0
        BT          ?ex_return
        MOV.L       ?ex_get_kmp,R3
        JSR         @R3
        MOV         R0,R9
        TST         R8,R8
        MOV.L       ?ex_color_mode,R3
        MOV.L       ?ex_opaque_mode,R4
        BT          ?ex_opaque
        MOV.L       ?ex_trans_mode,R4
        MOV.L       ?ex_start_trans,R10
        JSR         @R3
        NOP
        BRA         ?ex_start
        NOP
?ex_opaque:
        MOV.L       ?ex_start_opaque,R10
        JSR         @R3
        NOP
?ex_start:
        JSR         @R10
        NOP
        TST         R9,R9
        MOV.L       ?ex_vertex_buf,R4
        MOV.L       @R4,R4
        MOV         R11,R5
        BT          ?ex_strip
        MOV.L       ?ex_clip,R3
        JSR         @R3
        MOV         R0,R6
        BRA         ?ex_end
        MOV         R0,R1
?ex_strip:
        MOV         R0,R1
        ADD         #32,R1
        MOVA        ?ex_cmd_vertex,R0
        FMOV.S      @R0,FR0
?ex_loop:
        FMOV.S      @R4+,FR4
        FMOV.S      @R4+,FR3
        FMOV.S      @R4+,FR2
        DT          R5
        FMOV.S      @R4+,FR1
        BT          ?ex_last
        FSCHG
        FMOV        DR4,@-R1
        ADD         #-8,R1
        FMOV        DR2,@-R1
        FMOV        DR0,@-R1
        FSCHG
        PREF        @R1
        BRA         ?ex_loop
        ADD         #64,R1
?ex_last:
        MOVA        ?ex_cmd_end,R0
        FMOV.S      @R0,FR0
        FSCHG
        FMOV        DR4,@-R1
        ADD         #-8,R1
        FMOV        DR2,@-R1
        FMOV        DR0,@-R1
        FSCHG
        PREF        @R1
        ADD         #32,R1
?ex_end:
        MOV.L       ?ex_end_vertex,R3
        JSR         @R3
        MOV         R1,R4
        MOV.L       ?ex_set_kmp,R3
        JSR         @R3
        NOP
        LDS.L       @R15+,PR
        MOV.L       @R15+,R8
        MOV.L       @R15+,R9
        MOV.L       @R15+,R10
        RTS
        MOV.L       @R15+,R11
?ex_return:
        LDS.L       @R15+,PR
        MOV.L       @R15+,R8
        MOV.L       @R15+,R9
        MOV.L       @R15+,R10
        RTS
        MOV.L       @R15+,R11
        .ALIGN      4
?ex_vertex_buf:
        .DATA.L     __nj_vertex_buf_
?ex_screen:
        .DATA.L     __nj_screen_
?ex_aspect_x:
        .DATA.L     __nj_scr_aspect_x_
?ex_aspect_y:
        .DATA.L     __nj_scr_aspect_y_
?ex_get_kmp:
        .DATA.L     _njGetkmPointer
?ex_trans_mode:
        .DATA.L     H'02100002
?ex_color_mode:
        .DATA.L     _njSetkmColorMode
?ex_start_trans:
        .DATA.L     _njStartVertexNonTexTrans
?ex_opaque_mode:
        .DATA.L     H'00080002
?ex_start_opaque:
        .DATA.L     _njStartVertexNonTexOpaque
?ex_clip:
        .DATA.L     _njPolygonClip3DP
?ex_end_vertex:
        .DATA.L     _njEndVertex
?ex_set_kmp:
        .DATA.L     _njSetkmPointer
?ex_cmd_vertex:
        .DATA.L     H'E0000000
?ex_cmd_end:
        .DATA.L     H'F0000000
?ex_calc_point:
        .DATA.L     _njCalcPoint3DP
}
#endif
/* clang-format on */

/* clang-format off */
#ifdef DREAMCAST
/* @asm epilogue: dead-return, SHC's RTS at 0x8c12b2cc follows the body's own
 * return at 0x8c12b2be and its pool */
#pragma inline_asm(njCalcPoint3DP)
Int njCalcPoint3DP(NJS_POLYGON_VTX *p, Int count, struct draw3d_vertex *buf,
                   Float scale_x, Float scale_y, Float cx, Float cy)
{
        .IMPORT     __nj_far_clip_
        MOV         #0,R0
        ADD         #16,R6
        MOV.L       ?cp_near_clip,R1
        MOV.L       ?cp_far_clip,R2
        FMOV.S      @R1,FR10
        FMOV.S      @R2,FR11
?cp_loop:
        FMOV.S      @R4+,FR0
        FLDI1       FR3
        FMOV.S      @R4+,FR1
        FMOV.S      @R4+,FR2
        FTRV        XMTRX,FV0
        FCMP/GT     FR11,FR2
        FDIV        FR2,FR3
        FMOV.S      @R4+,FR9
        BT          ?cp_far
        FCMP/GT     FR10,FR2
        FMUL        FR4,FR0
        FMOV.S      FR0,FR2
        FMUL        FR5,FR1
        FMOV.S      FR1,FR0
        FMUL        FR3,FR0
        FMOV.S      FR2,FR1
        FMUL        FR3,FR1
        FADD        FR7,FR0
        FADD        FR6,FR1
        FMOV.S      FR9,FR2
        FSCHG
        FMOV        DR0,@-R6
        FMOV        DR2,@-R6
        FSCHG
        ADD         #32,R6
        BT          ?cp_next
        MOV         #1,R0
?cp_next:
        DT          R5
        BF          ?cp_loop
        RTS
        NOP
?cp_far:
        RTS
        MOV         #-1,R0
        .ALIGN      4
?cp_near_clip:
        .DATA.L     __nj_near_clip_
?cp_far_clip:
        .DATA.L     __nj_far_clip_
}
#endif
/* clang-format on */

/* send vtx as a block, and step sq past it. the block holds param, then vtx's
 * fields in reverse order, sx first. words 4 and 5, u and v, stay unwritten.
 * so does word 7, the offset color */
/* @hack spelling: a vertex's words stored as sq steps back from its end,
 * where indexed stores from its start would do */
#define SEND_VERTEX(vtx, param)                                                \
    {                                                                          \
        sq += SQ_WORDS - 1;                                                    \
        *--sq = (vtx)->col;                                                    \
        sq -= UV_WORDS;                                                        \
        *(Float *)--sq = (vtx)->ooz;                                           \
        *(Float *)--sq = (vtx)->sy;                                            \
        *(Float *)--sq = (vtx)->sx;                                            \
        *--sq = (param);                                                       \
        prefetch(sq);                                                          \
        sq += SQ_WORDS;                                                        \
    }

Uint32 *njPolygonClip3DP(struct draw3d_vertex *vtx, Int count, Uint32 *sq)
{
    Int strip = 0;
    Int i;
    Int clipped;
    Float inv_near;
    /* @hack redundant: register on a, b and c */
    register struct draw3d_vertex *a;
    register struct draw3d_vertex *b;
    register struct draw3d_vertex *c;

    a = vtx;
    b = &vtx[1];
    inv_near = 1.0F / _nj_near_clip_;
    for (i = 2; i < count; i++) {
        c = &vtx[i];
        /* @hack spelling: clipped, set where the test of the three vertices
         * fails, where that test in the if below would do */
        clipped = 0;
        if (!(a->ooz > 0.0F && a->ooz < inv_near && b->ooz > 0.0F &&
              b->ooz < inv_near && c->ooz > 0.0F && c->ooz < inv_near)) {
            clipped = 1;
        }
        if (!clipped) {
            if (!strip) {
                SEND_VERTEX(a, KM_VERTEXPARAM_NORMAL)
            }
            SEND_VERTEX(b, KM_VERTEXPARAM_NORMAL)
            strip++;
        } else {
            if (strip) {
                SEND_VERTEX(b, KM_VERTEXPARAM_ENDOFSTRIP)
                strip = 0;
            }
            if ((a->ooz > 0.0F && a->ooz < inv_near) ||
                (b->ooz > 0.0F && b->ooz < inv_near) ||
                (c->ooz > 0.0F && c->ooz < inv_near)) {
                sq = njPolygonNearClip3DP(a, b, c, sq);
            }
        }
        a = b;
        b = c;
    }
    if (strip) {
        SEND_VERTEX(c, KM_VERTEXPARAM_ENDOFSTRIP)
    }
    return sq;
}

/* the triangle's corners, and the words of each as a union corner holds it */
#define CORNERS 3
#define CORNER_WORDS 7
/* a corner of the triangle njPolygonNearClip3DP cuts. sx, sy and ooz are its
 * vertex's own. z is 1 over ooz, and x and y follow from z, sx and sy. for an
 * affine current matrix, (x, y, z) is the vertex's point through that matrix.
 * words views the same record */
union corner {
    Float words[CORNER_WORDS];
    struct {
        Float sx;
        Float sy;
        Float ooz;
        Float x;
        Float y;
        Float z;
        Uint32 col;
    } f;
};

#define STRIP_VERTEX_WORDS (sizeof(struct draw3d_vertex) / sizeof(Float))
/* a vertex of the strip njPolygonNearClip3DP sends. words views the same
 * record */
union strip_vertex {
    Float words[STRIP_VERTEX_WORDS];
    struct draw3d_vertex v;
};

/* fill corner r from vertex p through w, which must stand at r's first word.
 * the macro leaves w at the next corner's first word. x and y take the
 * caller's cx, cy, inv_x and inv_y */
/* @hack spelling: one word cursor that fills the three corners, each from
 * its end down, where stores to each corner's fields would do */
#define FILL_CORNER(r, p)                                                      \
    {                                                                          \
        w += CORNER_WORDS;                                                     \
        *(Uint32 *)--w = (p)->col;                                             \
        *--w = 1.0F / (p)->ooz;                                                \
        *--w = ((p)->sy - cy) * (r)->f.z * inv_y;                              \
        *--w = ((p)->sx - cx) * (r)->f.z * inv_x;                              \
        *--w = (p)->ooz;                                                       \
        *--w = (p)->sy;                                                        \
        *--w = (p)->sx;                                                        \
        w += CORNER_WORDS;                                                     \
    }

Uint32 *njPolygonNearClip3DP(struct draw3d_vertex *a, struct draw3d_vertex *b,
                             struct draw3d_vertex *c, Uint32 *sq)
{
    union corner corners[CORNERS + 1];
    union strip_vertex out[CORNERS + 1];
    Float *w;
    Float *s;
    Float *o;
    /* @hack redundant: register on every local but corners, out and the
     * three cursors */
    register Float scale_x;
    register Float scale_y;
    register Float cx;
    register Float cy;
    register Float inv_x;
    register Float inv_y;
    register Float near;
    register Float inv_near;
    register Float t;
    register Float x;
    register Float y;
    register Float z;
    register Float px;
    register Float py;
    register Float pz;
    register Float nx;
    register Float ny;
    register Float a0;
    register Float r0;
    register Float g0;
    register Float b0;
    register Float a1;
    register Float r1;
    register Float g1;
    register Float b1;
    register Float ca;
    register Float cr;
    register Float cg;
    register Float cb;
    register Uint32 col;
    register Uint32 pcol;
    register Int i;
    register Int n;

    scale_x = _nj_screen_.dist * _nj_scr_aspect_x_;
    scale_y = _nj_screen_.dist * _nj_scr_aspect_y_;
    inv_x = 1.0F / scale_x;
    inv_y = 1.0F / scale_y;
    cx = _nj_screen_.cx;
    cy = _nj_screen_.cy;
    w = corners[0].words;
    FILL_CORNER(&corners[0], a)
    FILL_CORNER(&corners[1], b)
    FILL_CORNER(&corners[2], c)
    /* corners[CORNERS] repeats corners[0], so the last edge runs from c to a */
    /* @hack spelling: the copy through two word cursors, s stepping down
     * from corners[1]'s first word, where corners[3] = corners[0] would do */
    w += CORNER_WORDS;
    s = corners[1].words;
    *(Uint32 *)--w = *(Uint32 *)--s;
    *--w = *--s;
    *--w = *--s;
    *--w = *--s;
    *--w = *--s;
    *--w = *--s;
    *--w = *--s;
    n = 0;
    pcol = corners[0].f.col;
    pz = corners[0].f.z;
    py = corners[0].f.y;
    px = corners[0].f.x;
    /* @hack spelling: one word cursor that writes the strip's vertices,
     * each from its end down, where stores to out[n]'s fields would do */
    o = out[0].words + STRIP_VERTEX_WORDS;
    near = _nj_near_clip_;
    /* @hack spelling: _nj_near_clip_ read again, where near would do */
    inv_near = 1.0F / _nj_near_clip_;
    scale_x *= inv_near;
    scale_y *= inv_near;
    for (i = 1; i < CORNERS + 1; i++) {
        col = corners[i].f.col;
        z = corners[i].f.z;
        y = corners[i].f.y;
        x = corners[i].f.x;
        /* an edge that crosses the near clip adds a vertex where it crosses.
         * the vertex lies t of the way from the edge's start to its end */
        if ((pz <= near) ^ (z <= near)) {
            t = (near - pz) / (z - pz);
            nx = px + (t * (x - px));
            ny = py + (t * (y - py));
            *--o = cx + (nx * scale_x);
            *--o = cy + (ny * scale_y);
            *--o = inv_near;
            a0 = (Float)((pcol >> ALPHA_SHIFT) & CHANNEL_MASK);
            r0 = (Float)((pcol >> RED_SHIFT) & CHANNEL_MASK);
            g0 = (Float)((pcol >> GREEN_SHIFT) & CHANNEL_MASK);
            b0 = (Float)(pcol & CHANNEL_MASK);
            a1 = (Float)((col >> ALPHA_SHIFT) & CHANNEL_MASK);
            r1 = (Float)((col >> RED_SHIFT) & CHANNEL_MASK);
            g1 = (Float)((col >> GREEN_SHIFT) & CHANNEL_MASK);
            b1 = (Float)(col & CHANNEL_MASK);
            ca = a0 + (t * (a1 - a0));
            cr = r0 + (t * (r1 - r0));
            cg = g0 + (t * (g1 - g0));
            cb = b0 + (t * (b1 - b0));
            pcol = ((Uint32)ca << ALPHA_SHIFT) | ((Uint32)cr << RED_SHIFT) |
                   ((Uint32)cg << GREEN_SHIFT) | (Uint32)cb;
            n++;
            *(Uint32 *)--o = pcol;
            o += 2 * STRIP_VERTEX_WORDS;
        }
        pcol = col;
        if (z > near) {
            n++;
            *--o = corners[i].f.sx;
            *--o = corners[i].f.sy;
            *--o = corners[i].f.ooz;
            *(Uint32 *)--o = col;
            o += 2 * STRIP_VERTEX_WORDS;
        }
        pz = z;
        py = y;
        px = x;
    }
    /* out holds the cut's n vertices in order around it. n is CORNERS for a
     * triangle, and one more for a quad. a strip zigzags, so a quad sends
     * out[3] before out[2] */
    SEND_VERTEX(&out[0].v, KM_VERTEXPARAM_NORMAL)
    SEND_VERTEX(&out[1].v, KM_VERTEXPARAM_NORMAL)
    if (n != CORNERS) {
        SEND_VERTEX(&out[3].v, KM_VERTEXPARAM_NORMAL)
    }
    SEND_VERTEX(&out[2].v, KM_VERTEXPARAM_ENDOFSTRIP)
    return sq;
}

void njDrawPolygon3DExStart(Int trans)
{
    njGetkmPointer();
    if (trans == 1) {
        njSetkmColorMode(NJD_COLOR_MODE_PACKED_NORMAL_TRANS);
        _nj_d3p_sq_ptr_ = njStartVertexNonTexTrans();
    } else {
        njSetkmColorMode(NJD_COLOR_MODE_PACKED_NORMAL);
        _nj_d3p_sq_ptr_ = njStartVertexNonTexOpaque();
    }
}

/* clang-format off */
#ifdef DREAMCAST
/* the vertices go out as njDrawPolygon3DEx sends them, from _nj_d3p_sq_ptr_
 * on. the call leaves _nj_d3p_sq_ptr_ past the last */
/* @asm epilogue: dead-return, SHC's RTS at 0x8c12b99c follows the body's own
 * return at 0x8c12b972 and its pool */
#pragma inline_asm(njDrawPolygon3DExSetData)
void njDrawPolygon3DExSetData(NJS_POLYGON_VTX *p, Int count)
{
        MOV.L       R8,@-R15
        STS.L       PR,@-R15
        MOV         R5,R8
        MOV.L       ?sd_screen,R1
        MOV.L       ?sd_aspect_x,R2
        MOV.L       ?sd_aspect_y,R3
        FMOV.S      @R1,FR3
        FMOV.S      @R2,FR4
        ADD         #12,R1
        FMOV.S      @R3,FR5
        FMUL        FR3,FR4
        FMOV.S      @R1+,FR6
        FMUL        FR3,FR5
        MOV.L       ?sd_vertex_buf,R6
        MOV.L       @R6,R6
        MOV.L       ?sd_calc_point,R3
        JSR         @R3
        FMOV.S      @R1+,FR7
        SHLL        R0
        BT          ?sd_return
        TST         R0,R0
        MOV.L       ?sd_vertex_buf,R4
        MOV.L       @R4,R4
        MOV         R8,R5
        MOV.L       ?sd_sq_ptr,R1
        MOV.L       @R1,R1
        BT          ?sd_strip
        MOV.L       ?sd_clip,R3
        JSR         @R3
        MOV         R1,R6
        BRA         ?sd_end
        MOV         R0,R1
?sd_strip:
        ADD         #32,R1
        MOVA        ?sd_cmd_vertex,R0
        FMOV.S      @R0,FR0
?sd_loop:
        FMOV.S      @R4+,FR4
        FMOV.S      @R4+,FR3
        FMOV.S      @R4+,FR2
        DT          R5
        FMOV.S      @R4+,FR1
        BT          ?sd_last
        FSCHG
        FMOV        DR4,@-R1
        ADD         #-8,R1
        FMOV        DR2,@-R1
        FMOV        DR0,@-R1
        FSCHG
        PREF        @R1
        BRA         ?sd_loop
        ADD         #64,R1
?sd_last:
        MOVA        ?sd_cmd_end,R0
        FMOV.S      @R0,FR0
        FSCHG
        FMOV        DR4,@-R1
        ADD         #-8,R1
        FMOV        DR2,@-R1
        FMOV        DR0,@-R1
        FSCHG
        PREF        @R1
        ADD         #32,R1
?sd_end:
        MOV.L       ?sd_sq_ptr,R3
        MOV.L       R1,@R3
        MOV         R1,R4
        LDS.L       @R15+,PR
        RTS
        MOV.L       @R15+,R8
?sd_return:
        LDS.L       @R15+,PR
        RTS
        MOV.L       @R15+,R8
        .ALIGN      4
?sd_vertex_buf:
        .DATA.L     __nj_vertex_buf_
?sd_screen:
        .DATA.L     __nj_screen_
?sd_aspect_x:
        .DATA.L     __nj_scr_aspect_x_
?sd_aspect_y:
        .DATA.L     __nj_scr_aspect_y_
?sd_clip:
        .DATA.L     _njPolygonClip3DP
?sd_cmd_vertex:
        .DATA.L     H'E0000000
?sd_cmd_end:
        .DATA.L     H'F0000000
?sd_calc_point:
        .DATA.L     _njCalcPoint3DP
?sd_sq_ptr:
        .DATA.L     __nj_d3p_sq_ptr_
}
#endif
/* clang-format on */

void njDrawPolygon3DExEnd(void)
{
    njEndVertex(_nj_d3p_sq_ptr_);
    njSetkmPointer();
}
