#include <nj_cnk.h>
#include <sg_xpt.h>

/* the member exports this function, though no header of the SDK declares
 * it */
/* NOLINTBEGIN(misc-use-internal-linkage) */
/* njCnkSimplePolygonDbClip is njCnkSimplePolygonDb's Clip form */
Uint32 *njCnkSimplePolygonDbClip(Sint16 *strips, NJS_CNK_SIMPLE_BUF *buf,
                                 Uint32 nbstrip, Uint32 *sq, Float ambient);
/* NOLINTEND(misc-use-internal-linkage) */

/* clang-format off */
#ifdef DREAMCAST
/* the body hard-codes NJS_CNK_SIMPLE_BUF's size, 32 bytes, and the offsets of
 * its fields from z to spec. it sets FPSCR.SZ and sends a vertex as
 * njCnkSimplePolygonDb's body does. SZ is also clear around its call of
 * njCnkSimplePolygonDbNearClip, as SHC's calling interface needs */
/* @bug uninitialized: hands the near clip a triangle whose greatest z, v2's, is
 * exactly 1 (FCMP/GT FR0,FR15 at 0x8c15f49e). the near clip sends four vertices
 * from unset locals for it (from 0x8c171be8) */
/* @asm operand: integer-constant, MOV.L ?mask,R0 at 0x8c15f268 loads H'3FFF,
 * the mask of nbstrip's strip count, from the pool */
#pragma inline_asm(njCnkSimplePolygonDbClip)
Uint32 *njCnkSimplePolygonDbClip(Sint16 *strips, NJS_CNK_SIMPLE_BUF *buf,
                                 Uint32 nbstrip, Uint32 *sq, Float ambient)
{
        .IMPORT     __nj_control_3d_flag_
        .IMPORT     __nj_calc_polygon_
        .IMPORT     _njCnkSimplePolygonDbNearClip
        .IMPORT     __nj_draw_polygon_
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
        FLDI1       FR15
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
        ADD         #20,R4
        FMOV.S      @R11+,FR8
        FSUB        FR1,FR2
        PREF        @R4
        FSCHG
        ADD         #-20,R4
        ADD         #-12,R11
        FMOV        @R9,DR12
        FSUB        FR7,FR8
        FCMP/GT     FR12,FR15
        FMUL        FR0,FR8
        BT          ?near
        FMOV        @R10,DR12
        FMUL        FR2,FR6
        FCMP/GT     FR12,FR15
        FLDI0       FR7
        BT          ?near
        FMOV        @R11,DR12
        FSUB        FR8,FR6
        FCMP/GT     FR12,FR15
        FMUL        FR9,FR6
        BT          ?near
        FCMP/GT     FR6,FR7
        BF/S        ?back
        NOP
        TST         R13,R13
        BT          ?front_open
        FMOV        @R10+,DR0
        ADD         #1,R13
        FMOV        @R10+,DR2
        ADD         #16,R7
        FMOV        @R10+,DR6
        ADD         #-24,R10
        FNEG        FR6
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
        ADD         #16,R7
        FMOV        @R9+,DR6
        ADD         #-24,R9
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
        ADD         #16,R7
        FMOV        @R10+,DR6
        ADD         #-24,R10
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
?near:
        BRA         ?clip
        NOP
?back:
        TST         R12,R12
        BT          ?back_open
        FMOV        @R10+,DR0
        ADD         #1,R12
        FMOV        @R10+,DR2
        ADD         #16,R7
        FMOV        @R10+,DR6
        ADD         #-24,R10
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
        ADD         #16,R7
        FMOV        @R9+,DR6
        ADD         #-24,R9
        FNEG        FR6
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
        ADD         #16,R7
        FMOV        @R10+,DR6
        ADD         #-24,R10
        FNEG        FR6
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
        BRA         ?next
        ADD         #8,R7
?clip:
        TST         R12,R12
        BT          ?clip_back
        FMOV        @R10+,DR0
        ADD         R12,R2
        FMOV        @R10+,DR2
        ADD         #16,R7
        FMOV        @R10+,DR6
        ADD         #-24,R10
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
        BRA         ?clip_test
        ADD         #8,R7
?clip_back:
        TST         R13,R13
        BT          ?clip_test
        FMOV        @R10+,DR0
        ADD         R13,R2
        FMOV        @R10+,DR2
        ADD         #16,R7
        FMOV        @R10+,DR6
        FNEG        FR6
        ADD         #-24,R10
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
?clip_test:
        MOV         #0,R12
        FMOV        @R9,DR0
        MOV         #0,R13
        FCMP/GT     FR15,FR0
        FMOV        @R10,DR0
        BT          ?clip_call
        FCMP/GT     FR15,FR0
        FMOV        @R11,DR0
        BT          ?clip_call
        FCMP/GT     FR0,FR15
        BT          ?next
?clip_call:
        FSCHG
        MOV.L       R1,@-R15
        MOV.L       R2,@-R15
        MOV.L       R3,@-R15
        MOV.L       R4,@-R15
        MOV.L       R5,@-R15
        MOV.L       R6,@-R15
        FMOV.S      FR4,@-R15
        FMOV.S      FR9,@-R15
        MOV.L       ?near_clip,R0
        FMOV.S      FR9,FR5
        MOV         R9,R4
        MOV         R10,R5
        JSR         @R0
        MOV         R11,R6
        MOV         R0,R7
        FMOV.S      @R15+,FR9
        FMOV.S      @R15+,FR4
        MOV.L       @R15+,R6
        MOV.L       @R15+,R5
        MOV.L       @R15+,R4
        MOV.L       @R15+,R3
        MOV.L       @R15+,R2
        MOV.L       @R15+,R1
        FSCHG
?next:
        FNEG        FR9
        DT          R8
        ADD         R6,R4
        BT/S        ?strip_end
        MOV         R10,R9
        BRA         ?triangle
        MOV         R11,R10
?strip_end:
        TST         R12,R12
        BT          ?strip_end_back
        FMOV        @R11+,DR0
        ADD         R12,R2
        FMOV        @R11+,DR2
        ADD         #16,R7
        FMOV        @R11+,DR6
        ADD         #-24,R11
        FADD        FR4,FR6
        FMOV        DR2,@-R7
        FCMP/GT     FR6,FR4
        FMOV        DR0,@-R7
        FSCHG
        BF          ?clamp9
        FMOV.S      FR4,FR6
?clamp9:
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
        ADD         #16,R7
        FMOV        @R11+,DR6
        FNEG        FR6
        ADD         #-24,R11
        FADD        FR4,FR6
        FMOV        DR2,@-R7
        FCMP/GT     FR6,FR4
        FMOV        DR0,@-R7
        FSCHG
        BF          ?clamp10
        FMOV.S      FR4,FR6
?clamp10:
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
?near_clip:
        .DATA.L     _njCnkSimplePolygonDbNearClip
?flag:
        .DATA.L     __nj_control_3d_flag_
}
#endif
/* clang-format on */
