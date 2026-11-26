#include <kamui.h>
#include <machine.h>
#include <ninja.h>
#include <nj_cnk.h>
#include <nj_direct.h>
#include <nj_global.h>
#include <sg_xpt.h>

#include "../ninja/njCnkSimpleEnvDb.h"

/* the member exports these functions, though no header of the SDK declares
 * them */
/* NOLINTBEGIN(misc-use-internal-linkage) */
/* njCnkSimplePolygonEnvDb is njCnkSimplePolygonEnv for strips under
 * NJD_FST_DB */
Uint32 *njCnkSimplePolygonEnvDb(Sint16 *strips, NJS_CNK_SIMPLE_BUF *buf,
                                Uint32 nbstrip, Uint32 *sq, Float ambient);
/* send the strips as njCnkSimplePolygonEnvDb does while
 * NJD_CONTROL_3D_ENV_UV_SCROLL is clear. it never offsets u and v, whatever the
 * flag holds */
Uint32 *_njCnkSimplePolygonEnvDb(Sint16 *strips, NJS_CNK_SIMPLE_BUF *buf,
                                 Uint32 nbstrip, Uint32 *sq, Float ambient);
/* NOLINTEND(misc-use-internal-linkage) */

Uint32 *njCnkSimplePolygonEnvDb(Sint16 *strips, NJS_CNK_SIMPLE_BUF *buf,
                                Uint32 nbstrip, Uint32 *sq, Float ambient)
{
    /* @hack redundant: register on every local */
    register NJS_CNK_SIMPLE_BUF *v0;
    register NJS_CNK_SIMPLE_BUF *v1;
    register NJS_CNK_SIMPLE_BUF *v2;
    register Sint32 len;
    register Sint32 user_halfwords;
    register Sint32 front;
    register Sint32 back;
    register Sint32 drawn;
    register Sint32 calculated;
    register Float uoff;
    register Float voff;
    register Float model_side;
    register Float side;
    register Float facing;
    register Float tu0;
    register Float tv0;
    register Float tu1;
    register Float tv1;
    register Float tu2;
    register Float tv2;

    /* @hack spelling: the call of _njCnkSimplePolygonEnvDb in the test's
     * else, where a return under the negated test would do */
    if (_nj_control_3d_flag_ & NJD_CONTROL_3D_ENV_UV_SCROLL) {
        uoff = _nj_uv_scroll_u_ + CNK_ENV_CENTER;
        voff = _nj_uv_scroll_v_ + CNK_ENV_CENTER;
    } else {
        return _njCnkSimplePolygonEnvDb(strips, buf, nbstrip, sq, ambient);
    }
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
        /* front counts the vertices an open run that faces the viewer has
         * sent. back counts those of an open run that faces away. at most one
         * run is open, and a count is 0 while its run is closed */
        front = 0;
        back = 0;
        tu0 = (v0->nx * CNK_ENV_SCALE) + uoff;
        tv0 = (v0->ny * CNK_ENV_SCALE) + voff;
        v1 = &buf[*strips++];
        prefetch(v1);
        tu1 = (v1->nx * CNK_ENV_SCALE) + uoff;
        tv1 = (v1->ny * CNK_ENV_SCALE) + voff;
        do {
            v2 = &buf[*strips++];
            prefetch(v2);
            tu2 = (v2->nx * CNK_ENV_SCALE) + uoff;
            tv2 = (v2->ny * CNK_ENV_SCALE) + voff;
            /* @hack spelling: the winding written as (v0 - v1) x (v2 - v1)
             * with its two products swapped, where (v1 - v0) x (v2 - v1)
             * would do */
            facing = (((v0->y - v1->y) * (v2->x - v1->x)) -
                      ((v0->x - v1->x) * (v2->y - v1->y))) *
                     side;
            if (facing < 0.0F) {
                if (back != 0) {
                    SEND_BACK(v1, tu1, tv1, KM_VERTEXPARAM_ENDOFSTRIP)
                    drawn += back + 1;
                    back = 0;
                }
                if (front == 0) {
                    SEND_FRONT(v0, tu0, tv0, KM_VERTEXPARAM_NORMAL)
                    front++;
                }
                SEND_FRONT(v1, tu1, tv1, KM_VERTEXPARAM_NORMAL)
                front++;
            } else {
                if (front != 0) {
                    SEND_FRONT(v1, tu1, tv1, KM_VERTEXPARAM_ENDOFSTRIP)
                    drawn += front + 1;
                    front = 0;
                }
                if (back == 0) {
                    SEND_BACK(v0, tu0, tv0, KM_VERTEXPARAM_NORMAL)
                    back++;
                }
                SEND_BACK(v1, tu1, tv1, KM_VERTEXPARAM_NORMAL)
                back++;
            }
            strips += user_halfwords;
            side = -side;
            v0 = v1;
            v1 = v2;
            tu0 = tu1;
            tv0 = tv1;
            tu1 = tu2;
            tv1 = tv2;
        } while (--len);
        /* @hack spelling: v2's x, y and ooz read as p steps along it, ahead
         * of the test of front, where SEND_FRONT or SEND_BACK under the test
         * would do */
        if (front != 0 || back != 0) {
            register Float *p;
            register Float x;
            register Float y;
            register Float ooz;
            register Float inten;
            register Float spec;
            p = &v2->x;
            x = *p++;
            y = *p++;
            ooz = *p++;
            if (front != 0) {
                inten = *p++ + ambient;
                spec = *p;
                drawn += front + 1;
            } else {
                inten = ambient - *p++;
                spec = -*p;
                drawn += back + 1;
            }
            if (inten < ambient) {
                inten = ambient;
            }
            SEND_VERTEX(tu2, tv2, KM_VERTEXPARAM_ENDOFSTRIP)
        }
    } while (--nbstrip);
    _nj_draw_polygon_ += drawn;
    _nj_calc_polygon_ += calculated;
    return sq;
}

/* clang-format off */
#ifdef DREAMCAST
/* _njCnkSimplePolygonEnvDb's body hard-codes NJS_CNK_SIMPLE_BUF's size, 32
 * bytes, and the offsets of its fields from z to ny. it sets FPSCR.SZ by FSCHG,
 * so an FMOV moves a pair of floats. it clears SZ for the single moves that
 * load a facing test's x and y. it clears SZ too for the move that keeps a lit
 * inten from falling below ambient. a vertex's words 0 to 3 are its entry's z,
 * x, y and ooz, moved as two pairs. the control word then overwrites word 0,
 * z */
/* @asm operand: integer-constant, MOV.L ?mask,R0 at 0x8c160b88 loads H'3FFF,
 * the mask of nbstrip's strip count, from the pool */
#pragma inline_asm(send_simple_unscrolled)
static Uint32 *send_simple_unscrolled(Sint16 *strips, NJS_CNK_SIMPLE_BUF *buf,
                                      Uint32 nbstrip, Uint32 *sq,
                                      Float ambient)
{
        MOV.L       R8,@-R15
        MOV.L       R9,@-R15
        MOV.L       R10,@-R15
        MOV.L       R11,@-R15
        MOV.L       R12,@-R15
        MOV.L       R13,@-R15
        MOV.L       R14,@-R15
        FMOV.S      FR12,@-R15
        FMOV.S      FR13,@-R15
        FMOV.S      FR14,@-R15
        FMOV.S      FR15,@-R15
        STS.L       PR,@-R15
        ADD         #-4,R15
        MOV.L       ?flag,R0
        MOV.L       @R0,R0
        MOV.L       R0,@R15
        MOV.L       ?mask,R0
        MOV         R4,R2
        ADD         #6,R2
        PREF        @R2
        MOV         R6,R1
        AND         R0,R1
        MOV         #-14,R2
        SHLD        R2,R6
        MOV         #3,R3
        MOV         #0,R2
        AND         R3,R6
        FLDI1       FR9
        MOV         #0,R3
        SHLL        R6
        MOVA        ?half,R0
        FMOV.S      @R0,FR5
        FSCHG
?strip:
        MOV         #5,R0
        MOV.W       @R4+,R8
        MOV.W       @R4+,R9
        SHLD        R0,R9
        MOV.L       @R15,R0
        SHLL        R0
        FLDI1       FR9
        BF          ?unmirrored
        FNEG        FR9
?unmirrored:
        ADD         R5,R9
        CMP/PZ      R8
        PREF        @R9
        BT          ?forward
        NEG         R8,R8
        FNEG        FR9
?forward:
        ADD         R8,R3
        ADD         #-2,R8
        MOV         #0,R12
        MOV         #0,R13
        MOV         #5,R0
        MOV.W       @R4+,R10
        SHLD        R0,R10
        ADD         R5,R10
        PREF        @R10
        MOV         #24,R0
        FMOV        @(R0,R9),DR10
        FMUL        FR5,FR10
        FMUL        FR5,FR11
        FADD        FR5,FR10
        FADD        FR5,FR11
        FMOV        @(R0,R10),DR12
        FMUL        FR5,FR12
        FMUL        FR5,FR13
        FADD        FR5,FR12
        FADD        FR5,FR13
?triangle:
        MOV.W       @R4+,R11
        FSCHG
        ADD         #4,R9
        ADD         #4,R10
        FMOV.S      @R9+,FR0
        MOV         #5,R0
        FMOV.S      @R9+,FR6
        SHLD        R0,R11
        FMOV.S      @R10+,FR1
        ADD         R5,R11
        FMOV.S      @R10+,FR7
        PREF        @R11
        ADD         #-12,R9
        FSUB        FR1,FR0
        ADD         #-12,R10
        FSUB        FR7,FR6
        ADD         #4,R11
        FMOV.S      @R11+,FR2
        FMOV.S      @R11+,FR8
        FSUB        FR1,FR2
        ADD         #12,R11
        FSCHG
        FSUB        FR7,FR8
        FMUL        FR0,FR8
        FMOV        @R11,DR14
        FMUL        FR2,FR6
        FLDI0       FR7
        FSUB        FR8,FR6
        FMUL        FR9,FR6
        FCMP/GT     FR6,FR7
        FNEG        FR9
        BF/S        ?back
        ADD         #-24,R11
        TST         R13,R13
        BT          ?front_open
        FMOV        @R10+,DR0
        ADD         #1,R13
        FMOV        @R10+,DR2
        ADD         #24,R7
        FMOV        @R10+,DR6
        ADD         #-24,R10
        FNEG        FR6
        FMOV        DR12,@-R7
        FADD        FR4,FR6
        FMOV        DR2,@-R7
        FCMP/GT     FR6,FR4
        FMOV        DR0,@-R7
        FSCHG
        BF          ?clamp1
        FMOV.S      FR4,FR6
?clamp1:
        FNEG        FR7
        MOV.L       ?end_control,R14
        ADD         R13,R2
        MOV.L       R14,@R7
        FSCHG
        ADD         #24,R7
        MOV         #0,R13
        FMOV        DR6,@R7
        PREF        @R7
        ADD         #8,R7
?front_open:
        TST         R12,R12
        BF          ?front_v1
        FMOV        @R9+,DR0
        ADD         #1,R12
        FMOV        @R9+,DR2
        ADD         #24,R7
        FMOV        @R9+,DR6
        ADD         #-24,R9
        FMOV        DR10,@-R7
        FADD        FR4,FR6
        FMOV        DR2,@-R7
        FCMP/GT     FR6,FR4
        FMOV        DR0,@-R7
        FSCHG
        BF          ?clamp2
        FMOV.S      FR4,FR6
?clamp2:
        MOV.L       ?vertex_control,R14
        FSCHG
        MOV.L       R14,@R7
        ADD         #24,R7
        FMOV        DR6,@R7
        PREF        @R7
        ADD         #8,R7
?front_v1:
        FMOV        @R10+,DR0
        ADD         #1,R12
        FMOV        @R10+,DR2
        ADD         #24,R7
        FMOV        @R10+,DR6
        ADD         #-24,R10
        FMOV        DR12,@-R7
        FADD        FR4,FR6
        FMOV        DR2,@-R7
        FCMP/GT     FR6,FR4
        FMOV        DR0,@-R7
        FSCHG
        BF          ?clamp3
        FMOV.S      FR4,FR6
?clamp3:
        MOV.L       ?vertex_control,R14
        FSCHG
        MOV.L       R14,@R7
        ADD         #24,R7
        FMOV        DR6,@R7
        PREF        @R7
        BRA         ?next
        ADD         #8,R7
?back:
        TST         R12,R12
        BT          ?back_open
        FMOV        @R10+,DR0
        ADD         #1,R12
        FMOV        @R10+,DR2
        ADD         #24,R7
        FMOV        @R10+,DR6
        ADD         #-24,R10
        FMOV        DR12,@-R7
        FADD        FR4,FR6
        FMOV        DR2,@-R7
        FCMP/GT     FR6,FR4
        FMOV        DR0,@-R7
        FSCHG
        BF          ?clamp4
        FMOV.S      FR4,FR6
?clamp4:
        MOV.L       ?end_control,R14
        ADD         R12,R2
        MOV.L       R14,@R7
        FSCHG
        ADD         #24,R7
        MOV         #0,R12
        FMOV        DR6,@R7
        PREF        @R7
        ADD         #8,R7
?back_open:
        TST         R13,R13
        BF          ?back_v1
        FMOV        @R9+,DR0
        ADD         #1,R13
        FMOV        @R9+,DR2
        ADD         #24,R7
        FMOV        @R9+,DR6
        ADD         #-24,R9
        FNEG        FR6
        FMOV        DR10,@-R7
        FADD        FR4,FR6
        FMOV        DR2,@-R7
        FCMP/GT     FR6,FR4
        FMOV        DR0,@-R7
        FSCHG
        BF          ?clamp5
        FMOV.S      FR4,FR6
?clamp5:
        FNEG        FR7
        MOV.L       ?vertex_control,R14
        FSCHG
        MOV.L       R14,@R7
        ADD         #24,R7
        FMOV        DR6,@R7
        PREF        @R7
        ADD         #8,R7
?back_v1:
        FMOV        @R10+,DR0
        ADD         #1,R13
        FMOV        @R10+,DR2
        ADD         #24,R7
        FMOV        @R10+,DR6
        ADD         #-24,R10
        FNEG        FR6
        FMOV        DR12,@-R7
        FADD        FR4,FR6
        FMOV        DR2,@-R7
        FCMP/GT     FR6,FR4
        FMOV        DR0,@-R7
        FSCHG
        BF          ?clamp6
        FMOV.S      FR4,FR6
?clamp6:
        FNEG        FR7
        MOV.L       ?vertex_control,R14
        FSCHG
        MOV.L       R14,@R7
        ADD         #24,R7
        FMOV        DR6,@R7
        PREF        @R7
        ADD         #8,R7
?next:
        FMUL        FR5,FR14
        ADD         R6,R4
        FMUL        FR5,FR15
        MOV         R10,R9
        FADD        FR5,FR14
        DT          R8
        FADD        FR5,FR15
        FMOV        DR12,DR10
        BT/S        ?strip_end
        FMOV        DR14,DR12
        BRA         ?triangle
        MOV         R11,R10
?strip_end:
        TST         R12,R12
        BT          ?strip_end_back
        FMOV        @R11+,DR0
        ADD         R12,R2
        FMOV        @R11+,DR2
        ADD         #24,R7
        FMOV        @R11+,DR6
        ADD         #-24,R11
        FMOV        DR14,@-R7
        FADD        FR4,FR6
        FMOV        DR2,@-R7
        FCMP/GT     FR6,FR4
        FMOV        DR0,@-R7
        FSCHG
        BF          ?clamp7
        FMOV.S      FR4,FR6
?clamp7:
        MOV.L       ?end_control,R14
        ADD         #1,R2
        MOV.L       R14,@R7
        FSCHG
        ADD         #24,R7
        FMOV        DR6,@R7
        PREF        @R7
        BRA         ?strip_next
        ADD         #8,R7
?strip_end_back:
        TST         R13,R13
        BT          ?strip_next
        FMOV        @R11+,DR0
        ADD         R13,R2
        FMOV        @R11+,DR2
        ADD         #24,R7
        FMOV        @R11+,DR6
        FNEG        FR6
        ADD         #-24,R11
        FMOV        DR14,@-R7
        FADD        FR4,FR6
        FMOV        DR2,@-R7
        FCMP/GT     FR6,FR4
        FMOV        DR0,@-R7
        FSCHG
        BF          ?clamp8
        FMOV.S      FR4,FR6
?clamp8:
        FNEG        FR7
        MOV.L       ?end_control,R14
        ADD         #1,R2
        MOV.L       R14,@R7
        FSCHG
        ADD         #24,R7
        FMOV        DR6,@R7
        PREF        @R7
        ADD         #8,R7
?strip_next:
        DT          R1
        BT          ?done
        BRA         ?strip
        NOP
?done:
        FSCHG
        MOV.L       ?draw,R4
        MOV         R7,R0
        MOV.L       ?calc,R5
        MOV.L       @R4,R6
        MOV.L       @R5,R7
        ADD         R2,R6
        ADD         R3,R7
        MOV.L       R6,@R4
        MOV.L       R7,@R5
        ADD         #4,R15
        LDS.L       @R15+,PR
        FMOV.S      @R15+,FR15
        FMOV.S      @R15+,FR14
        FMOV.S      @R15+,FR13
        FMOV.S      @R15+,FR12
        MOV.L       @R15+,R14
        MOV.L       @R15+,R13
        MOV.L       @R15+,R12
        MOV.L       @R15+,R11
        MOV.L       @R15+,R10
        MOV.L       @R15+,R9
        RTS
        MOV.L       @R15+,R8
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
?half:
        .FDATA.S    F'0.5
?flag:
        .DATA.L     __nj_control_3d_flag_
}
#else
/* NOLINTBEGIN(clang-diagnostic-undefined-internal) */
static Uint32 *send_simple_unscrolled(Sint16 *strips, NJS_CNK_SIMPLE_BUF *buf,
                                      Uint32 nbstrip, Uint32 *sq,
                                      Float ambient);
/* NOLINTEND(clang-diagnostic-undefined-internal) */
#endif
/* clang-format on */

/* @hack spelling: a call of a static inline body, where an inline body named
 * _njCnkSimplePolygonEnvDb would do */
Uint32 *_njCnkSimplePolygonEnvDb(Sint16 *strips, NJS_CNK_SIMPLE_BUF *buf,
                                 Uint32 nbstrip, Uint32 *sq, Float ambient)
{
    return send_simple_unscrolled(strips, buf, nbstrip, sq, ambient);
}
