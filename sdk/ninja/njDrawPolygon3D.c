#include <ninja.h>
#include <nj_draw3d.h>
#include <nj_km.h>
#include <sg_xpt.h>

/* the most points buf holds, and the fewest a filled polygon takes */
#define POLYGON_POINTS 32
#define MIN_FILL_POINTS 3

void njDrawPolygon3D(NJS_POINT3COL *p, Int n, Uint32 attr)
{
    Float sx;
    Float sy;
    Float cx;
    Float cy;
    Uint32 *sq;
    union {
        struct draw3d_vertex plain[POLYGON_POINTS];
        struct draw3d_tex_vertex textured[POLYGON_POINTS];
    } buf;
    Sint32 clip;

#ifdef NONMATCHING
    /* GCC's side returns for an n the call doesn't take. n must be 1 to
     * POLYGON_POINTS, since for any other a projector can write past buf.
     * under NJD_FILL it must be MIN_FILL_POINTS or more, as njTri_Filled
     * needs */
    if (n <= 0 || n > POLYGON_POINTS ||
        ((attr & NJD_FILL) && n < MIN_FILL_POINTS)) {
        return;
    }
#endif
    sx = _nj_screen_.dist * _nj_scr_aspect_x_;
    sy = _nj_screen_.dist * _nj_scr_aspect_y_;
    cx = _nj_screen_.cx;
    cy = _nj_screen_.cy;
    if (attr & NJD_USE_TEXTURE) {
        /* @hack spelling: the textured draw nested under the projection's
         * test, where an early return as below would do */
        if ((clip = njDCalcPoint3DT(p, n, buf.textured, sx, sy, cx, cy)) !=
            BEYOND_FAR_CLIP) {
            njGetkmPointer();
            if (attr & NJD_TRANSPARENT) {
                njSetkmColorMode(NJD_COLOR_MODE_PACKED_TEXTURE_TRANS);
                sq = njStartVertexTexTrans();
            } else {
                njSetkmColorMode(NJD_COLOR_MODE_PACKED_TEXTURE);
                sq = njStartVertexTexOpaque();
            }
            if (attr & NJD_FILL) {
                sq = njTri_Filled_With_Tex(buf.textured, n, sq, clip);
            } else {
                sq = njTri_Wired_With_Tex(buf.textured, n, sq, clip);
            }
        } else {
            return;
        }
    } else {
        /* @hack type: buf.plain cast to void * at its three calls, where
         * buf.plain itself would do */
        if ((clip = njDCalcPoint3DP(p, n, (void *)buf.plain, sx, sy, cx, cy)) ==
            BEYOND_FAR_CLIP) {
            return;
        }
        njGetkmPointer();
        if (attr & NJD_TRANSPARENT) {
            njSetkmColorMode(NJD_COLOR_MODE_PACKED_NORMAL_TRANS);
            sq = njStartVertexNonTexTrans();
        } else {
            njSetkmColorMode(NJD_COLOR_MODE_PACKED_NORMAL);
            sq = njStartVertexNonTexOpaque();
        }
        if (attr & NJD_FILL) {
            sq = njTri_Filled((void *)buf.plain, n, sq, clip);
        } else {
            sq = njTri_Wired((void *)buf.plain, n, sq, clip);
        }
    }
    njEndVertex(sq);
    njSetkmPointer();
}

#ifdef DREAMCAST
/* clang-format off */
/* @bug memory: reads colors from low memory where p->col is null. CMP/EQ
 * R1,R7 at 0x8c7face8 gives a point white only where R7 is 0. MOV.L @R7+,R9
 * at 0x8c7facf6, in the delay slot of the branch on the test, loads through
 * R7 even at 0 and steps it. so only point 0 takes white. each other point i
 * takes the word at address 4i */
/* @asm operand: small-constant, MOV.L at 0x8c7facf8 loads white, -1, from
 * the pool */
#pragma inline_asm(project_textured)
static Sint32 project_textured(NJS_POINT3COL *p, Int count, void *buf,
                       Float sx, Float sy, Float cx, Float cy)
{
        .IMPORT     __nj_near_clip_
        .IMPORT     __nj_far_clip_
        MOV.L       R9,@-R15
        MOV.L       R8,@-R15
        MOV.L       @R4+,R2
        MOV.L       @R4+,R7
        MOV.L       @R4+,R8
        ADD         #24,R6
        MOV         R2,R4
        MOVA        ?t_uv_scale,R0
        MOV.L       ?t_near,R2
        MOV.L       ?t_far,R3
        FMOV.S      @R0,FR8
        MOV         #0,R0
?t_loop:
        MOV         #0,R1
        FMOV.S      @R4+,FR0
        CMP/EQ      R1,R7
        FLDI1       FR3
        FMOV.S      @R4+,FR1
        FMOV.S      @R4+,FR2
        FTRV        XMTRX,FV0
        FMOV.S      @R3,FR11
        BF/S        ?t_color
        MOV.L       @R7+,R9
        MOV.L       ?t_white,R9
?t_color:
        FCMP/GT     FR11,FR2
        FDIV        FR2,FR3
        MOV.W       @R8+,R1
        FMOV.S      @R2,FR11
        BT          ?t_far_out
        FMUL        FR4,FR0
        FCMP/GT     FR11,FR2
        FMUL        FR5,FR1
        LDS         R1,FPUL
        FMUL        FR3,FR0
        FLOAT       FPUL,FR9
        MOV.W       @R8+,R1
        FMUL        FR3,FR1
        LDS         R1,FPUL
        FADD        FR6,FR0
        FLOAT       FPUL,FR2
        FADD        FR7,FR1
        FMOV.S      FR0,@-R6
        FMUL        FR8,FR9
        FMOV.S      FR1,@-R6
        FMOV.S      FR3,@-R6
        FMUL        FR8,FR2
        FMOV.S      FR9,@-R6
        FMOV.S      FR2,@-R6
        MOV.L       R9,@-R6
        ADD         #48,R6
        BT          ?t_next
        MOV         #1,R0
?t_next:
        DT          R5
        BF          ?t_loop
        MOV.L       @R15+,R8
        RTS
        MOV.L       @R15+,R9
?t_white:
        .DATA.L     H'FFFFFFFF
?t_far_out:
        MOV         #-1,R0
        MOV.L       @R15+,R8
        RTS
        MOV.L       @R15+,R9
        .DATA.L     H'FFFFFFFF
?t_near:
        .DATA.L     __nj_near_clip_
?t_far:
        .DATA.L     __nj_far_clip_
?t_uv_scale:
        .DATA.L     H'3B800000
}
/* clang-format on */
Sint32 njDCalcPoint3DT(NJS_POINT3COL *p, Int count, void *buf, Float sx,
                       Float sy, Float cx, Float cy)
{
#ifdef NONMATCHING
    /* for a null p->col, GCC's side projects one point a call, with p->col
     * pointing at one white. the body reads each point's color through
     * p->col, null or not */
    if (p->col == NULL) {
        NJS_POINT3COL one = *p;
        NJS_COLOR white;
        struct draw3d_tex_vertex *vertices = buf;
        Sint32 clip = 0;
        Int i;
        white.color = 0xffffffffU;
        one.col = &white;
        for (i = 0; i < count; i++) {
            Sint32 code;
            one.p = &p->p[i];
            one.tex = &p->tex[i];
            code = njDCalcPoint3DT(&one, 1, &vertices[i], sx, sy, cx, cy);
            if (code == BEYOND_FAR_CLIP) {
                return code;
            }
            clip |= code;
        }
        return clip;
    }
#endif
    return project_textured(p, count, buf, sx, sy, cx, cy);
}

/* clang-format off */
/* @asm epilogue: dead-return, SHC's RTS at 0x8c7fadc0 follows the body's own
 * returns at 0x8c7fadac and 0x8c7fadb0 and its pool */
#pragma inline_asm(project_plain)
static Sint32 project_plain(NJS_POINT3COL *p, Int count, void *buf,
                       Float sx, Float sy, Float cx, Float cy)
{
        MOV.L       @R4+,R2
        MOV.L       @R4+,R7
        ADD         #16,R6
        MOV         #0,R1
        MOV         R2,R4
        MOVA        ?p_white,R0
        FMOV.S      @R0,FR10
        MOV.L       ?p_near,R2
        MOV         #0,R0
        MOV.L       ?p_far,R3
?p_loop:
        CMP/EQ      R1,R7
        FMOV.S      @R4+,FR0
        FLDI1       FR3
        FMOV.S      @R4+,FR1
        FMOV.S      @R4+,FR2
        FTRV        XMTRX,FV0
        FMOV.S      @R3,FR11
        BT/S        ?p_color
        FMOV.S      FR10,FR9
        FMOV.S      @R7+,FR9
?p_color:
        FCMP/GT     FR11,FR2
        FDIV        FR2,FR3
        FMOV.S      @R2,FR8
        BT          ?p_far_out
        FMUL        FR4,FR0
        FCMP/GT     FR8,FR2
        FMUL        FR5,FR1
        FMUL        FR3,FR0
        FMUL        FR3,FR1
        FADD        FR6,FR0
        FADD        FR7,FR1
        FMOV.S      FR0,@-R6
        FMOV.S      FR1,@-R6
        FMOV.S      FR3,@-R6
        FMOV.S      FR9,@-R6
        ADD         #32,R6
        BT          ?p_next
        MOV         #1,R0
?p_next:
        DT          R5
        BF          ?p_loop
        RTS
        NOP
?p_far_out:
        RTS
        MOV         #-1,R0
?p_white:
        .DATA.L     H'FFFFFFFF
?p_near:
        .DATA.L     __nj_near_clip_
?p_far:
        .DATA.L     __nj_far_clip_
}
/* clang-format on */
Sint32 njDCalcPoint3DP(NJS_POINT3COL *p, Int count, void *buf, Float sx,
                       Float sy, Float cx, Float cy)
{
    return project_plain(p, count, buf, sx, sy, cx, cy);
}
#endif
