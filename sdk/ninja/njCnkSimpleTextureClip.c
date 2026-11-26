#include <nj_cnk.h>
#include <sg_xpt.h>

/* the member exports this function, though no header of the SDK declares
 * it */
/* NOLINTBEGIN(misc-use-internal-linkage) */
/* njCnkSimpleTextureClip is njCnkSimpleTexture's Clip form */
Uint32 *njCnkSimpleTextureClip(Sint16 *strips, NJS_CNK_SIMPLE_BUF *buf,
                               Uint32 nbstrip, Uint32 *sq, Float ambient,
                               Float uvscale);
/* NOLINTEND(misc-use-internal-linkage) */

/* clang-format off */
#ifdef DREAMCAST
/* the body hard-codes NJS_CNK_SIMPLE_BUF's size, 32 bytes, and the offsets of
 * its fields from z to spec. it sets FPSCR.SZ and sends a vertex as
 * njCnkSimpleTexture's body does. the moves that test whether a triangle is
 * whole run with SZ clear too. SZ is also clear around its call of
 * njCnkSimpleTextureNearClip, as SHC's calling interface needs */
/* @asm operand: integer-constant, MOV.L ?mask,R0 at 0x8c162098 loads H'3FFF,
 * the mask of nbstrip's strip count, from the pool */
#pragma inline_asm(njCnkSimpleTextureClip)
Uint32 *njCnkSimpleTextureClip(Sint16 *strips, NJS_CNK_SIMPLE_BUF *buf,
                               Uint32 nbstrip, Uint32 *sq, Float ambient,
                               Float uvscale)
{
        .IMPORT     __nj_draw_polygon_
        .IMPORT     __nj_calc_polygon_
        .IMPORT     _njCnkSimpleTextureNearClip
        .IMPORT     __nj_control_3d_flag_
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
        MOV.L       ?vertex_control,R13
        MOV.L       ?end_control,R14
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
        MOV.W       @R4+,R0
        LDS         R0,FPUL
        MOV.W       @R4+,R0
        FLOAT       FPUL,FR10
        LDS         R0,FPUL
        MOV         #5,R0
        FLOAT       FPUL,FR11
        MOV.W       @R4+,R10
        SHLD        R0,R10
        FMUL        FR5,FR10
        ADD         R5,R10
        FMUL        FR5,FR11
        PREF        @R10
        MOV.W       @R4+,R0
        LDS         R0,FPUL
        MOV.W       @R4+,R0
        FLOAT       FPUL,FR12
        LDS         R0,FPUL
        FLOAT       FPUL,FR13
        FMUL        FR5,FR12
        FMUL        FR5,FR13
?triangle:
        MOV.W       @R4+,R11
        FSCHG
        MOV.W       @R4+,R0
        ADD         #4,R9
        LDS         R0,FPUL
        ADD         #4,R10
        MOV.W       @R4+,R0
        FLOAT       FPUL,FR14
        LDS         R0,FPUL
        MOV         #5,R0
        FMOV.S      @R9+,FR0
        SHLD        R0,R11
        FMOV.S      @R9+,FR6
        FLOAT       FPUL,FR15
        FMOV.S      @R10+,FR1
        ADD         R5,R11
        FMOV.S      @R10+,FR7
        FMUL        FR5,FR14
        PREF        @R11
        FMUL        FR5,FR15
        ADD         #-12,R9
        FSUB        FR1,FR0
        ADD         #-12,R10
        FSUB        FR7,FR6
        ADD         #4,R11
        FMOV.S      @R11+,FR2
        ADD         #20,R4
        FMOV.S      @R11+,FR8
        ADD         #-12,R11
        FLDI1       FR3
        FSUB        FR1,FR2
        FMOV.S      @R9,FR1
        FSUB        FR7,FR8
        FMOV.S      @R10,FR7
        PREF        @R4
        FMUL        FR0,FR8
        FMOV.S      @R11,FR0
        ADD         #-20,R4
        FCMP/GT     FR1,FR3
        FSCHG
        BT/S        ?hidden
        MOV         #1,R0
        FCMP/GT     FR7,FR3
        FMUL        FR2,FR6
        BT          ?hidden
        FLDI0       FR2
        FSUB        FR8,FR6
        FCMP/GT     FR0,FR3
        FMUL        FR9,FR6
        BT          ?hidden
        FCMP/GT     FR6,FR2
        MOV         #0,R0
        BF          ?hidden
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
        BF          ?clamp1
        FMOV.S      FR4,FR6
?clamp1:
        FSCHG
        MOV.L       R13,@R7
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
        BF          ?clamp2
        FMOV.S      FR4,FR6
?clamp2:
        FSCHG
        MOV.L       R13,@R7
        ADD         #24,R7
        FMOV        DR6,@R7
        PREF        @R7
        BRA         ?clip_test
        ADD         #8,R7
?hidden:
        TST         R12,R12
        BT          ?clip_test
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
        ADD         R12,R2
        MOV.L       R14,@R7
        FSCHG
        ADD         #24,R7
        MOV         #0,R12
        FMOV        DR6,@R7
        PREF        @R7
        ADD         #8,R7
?clip_test:
        TST         R0,R0
        BF          ?near
?next:
        FNEG        FR9
        MOV         R10,R9
        ADD         R6,R4
        DT          R8
        MOV         R11,R10
        BT          ?strip_end
        FMOV        DR12,DR10
        BRA         ?triangle
        FMOV        DR14,DR12
?strip_end:
        TST         R12,R12
        BT          ?strip_next
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
        BF          ?clamp4
        FMOV.S      FR4,FR6
?clamp4:
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
?near:
        FMOV        @R9,DR0
        FLDI1       FR3
        FCMP/GT     FR3,FR0
        FMOV        @R10,DR0
        BT          ?clip_call
        FCMP/GT     FR3,FR0
        FMOV        @R11,DR0
        BT          ?clip_call
        FCMP/GT     FR3,FR0
        BF          ?next
?clip_call:
        FSCHG
        MOV.L       R1,@-R15
        MOV.L       R2,@-R15
        MOV.L       R3,@-R15
        MOV.L       R4,@-R15
        MOV.L       R5,@-R15
        MOV.L       R6,@-R15
        FMOV.S      FR4,@-R15
        FMOV.S      FR5,@-R15
        FMOV.S      FR9,@-R15
        FMOV.S      FR10,@-R15
        FMOV.S      FR11,@-R15
        FMOV.S      FR4,FR0
        FMOV.S      FR9,FR1
        MOV.L       ?near_clip,R0
        MOV         R9,R4
        MOV         R10,R5
        MOV         R11,R6
        FMOV.S      FR10,FR4
        FMOV.S      FR11,FR5
        FMOV.S      FR12,FR6
        FMOV.S      FR13,FR7
        FMOV.S      FR14,FR8
        FMOV.S      FR15,FR9
        FMOV.S      FR0,FR10
        JSR         @R0
        FMOV.S      FR1,FR11
        MOV         R0,R7
        FMOV.S      @R15+,FR11
        FMOV.S      @R15+,FR10
        FMOV.S      @R15+,FR9
        FMOV.S      @R15+,FR5
        FMOV.S      @R15+,FR4
        MOV.L       @R15+,R6
        MOV.L       @R15+,R5
        MOV.L       @R15+,R4
        MOV.L       @R15+,R3
        MOV.L       @R15+,R2
        MOV.L       @R15+,R1
        BRA         ?next
        FSCHG
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
        .DATA.L     _njCnkSimpleTextureNearClip
?flag:
        .DATA.L     __nj_control_3d_flag_
}
#endif
/* clang-format on */
