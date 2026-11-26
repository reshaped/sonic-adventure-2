/* clang-format off */
#ifdef DREAMCAST
#include <nj_cnk.h>

/* the body sets SZ and sends a vertex as njCnkEasyMultiTexture's does. SZ is
 * clear around its call of njCnkEasyMultiTextureNearClip, as SHC's calling
 * interface needs */
/* @bug uninitialized: hands the near clip a triangle whose greatest z is
 * exactly 1 (FCMP/GT FR0,FR9 at 0x8c144320). the near clip sends four
 * vertices with unset positions for it (from 0x8c15df9a) */
/* @asm operand: fschg-pairs, FMOV.S FR10,@-R7 at 0x8c144278 stores a
 * vertex's u and v as a pair */
#pragma inline_asm(njCnkEasyMultiTextureClip)
Uint32 *njCnkEasyMultiTextureClip(Sint16 *strips, NJS_CNK_EASYMULTI_BUF *buf,
                                  Sint32 nbstrip, Uint32 *sq, Float a, Float ar,
                                  Float ag, Float ab, Float uvscale)
{
        .IMPORT     __nj_calc_polygon_
        .IMPORT     __nj_draw_polygon_
        .IMPORT     _njCnkEasyMultiTextureNearClip
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
        FSUB        FR9,FR5
        FSUB        FR9,FR6
        FSUB        FR9,FR7
        FSCHG
?strip:
        MOV         #5,R0
        MOV.W       @R4+,R8
        MOV.W       @R4+,R9
        SHLD        R0,R9
        ADD         R5,R9
        CMP/PZ      R8
        PREF        @R9
        BT          ?forward
        NEG         R8,R8
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
        FMUL        FR8,FR10
        ADD         R5,R10
        FMUL        FR8,FR11
        PREF        @R10
        MOV.W       @R4+,R0
        LDS         R0,FPUL
        MOV.W       @R4+,R0
        FLOAT       FPUL,FR12
        LDS         R0,FPUL
        FLOAT       FPUL,FR13
        FMUL        FR8,FR12
        FMUL        FR8,FR13
?triangle:
        MOV         #5,R0
        MOV.W       @R4+,R11
        SHLD        R0,R11
        MOV.W       @R4+,R0
        ADD         R5,R11
        FMOV.S      @R9,FR0
        LDS         R0,FPUL
        MOV.W       @R4+,R0
        PREF        @R11
        FLOAT       FPUL,FR14
        LDS         R0,FPUL
        FLOAT       FPUL,FR15
        FMUL        FR8,FR14
        FMUL        FR8,FR15
        FCMP/GT     FR0,FR9
        FMOV.S      @R10,FR2
        BT          ?near
        FCMP/GT     FR2,FR9
        FMOV.S      @R11,FR0
        BT          ?near
        FCMP/GT     FR0,FR9
        BT          ?near
        TST         R12,R12
        BF          ?continue_run
        FMOV.S      @R9+,FR0
        FMOV.S      @R9+,FR2
        ADD         #24,R7
        FMOV.S      FR10,@-R7
        FMOV.S      FR2,@-R7
        FMOV.S      FR0,@-R7
        MOV.L       R13,@R7
        PREF        @R7
        ADD         #32,R7
        FMOV.S      @R9+,FR0
        FMOV.S      @R9+,FR2
        FMUL        FR4,FR0
        FADD        FR5,FR1
        ADD         #-32,R9
        FADD        FR6,FR2
        FADD        FR7,FR3
        ADD         #32,R7
        FMOV.S      FR2,@-R7
        FMOV.S      FR0,@-R7
        FADD        FR9,FR1
        FADD        FR9,FR2
        FADD        FR9,FR3
        FMOV.S      FR2,@-R7
        ADD         #1,R12
        FMOV.S      FR0,@-R7
        PREF        @R7
        ADD         #32,R7
?continue_run:
        FMOV.S      @R10+,FR0
        FMOV.S      @R10+,FR2
        ADD         #24,R7
        FMOV.S      FR12,@-R7
        FMOV.S      FR2,@-R7
        FMOV.S      FR0,@-R7
        MOV.L       R13,@R7
        PREF        @R7
        ADD         #32,R7
        FMOV.S      @R10+,FR0
        FMOV.S      @R10+,FR2
        FMUL        FR4,FR0
        FADD        FR5,FR1
        ADD         #-32,R10
        FADD        FR6,FR2
        FADD        FR7,FR3
        ADD         #32,R7
        FMOV.S      FR2,@-R7
        FMOV.S      FR0,@-R7
        FADD        FR9,FR1
        FADD        FR9,FR2
        FADD        FR9,FR3
        FMOV.S      FR2,@-R7
        ADD         #1,R12
        FMOV.S      FR0,@-R7
        PREF        @R7
        BRA         ?next_triangle
        ADD         #32,R7
?near:
        TST         R12,R12
        BT          ?check_near
        FMOV.S      @R10+,FR0
        FMOV.S      @R10+,FR2
        ADD         #24,R7
        FMOV.S      FR12,@-R7
        FMOV.S      FR2,@-R7
        FMOV.S      FR0,@-R7
        MOV.L       R14,@R7
        PREF        @R7
        ADD         #32,R7
        FMOV.S      @R10+,FR0
        FMOV.S      @R10+,FR2
        FMUL        FR4,FR0
        FADD        FR5,FR1
        ADD         #-32,R10
        FADD        FR6,FR2
        FADD        FR7,FR3
        ADD         #32,R7
        FMOV.S      FR2,@-R7
        FMOV.S      FR0,@-R7
        FADD        FR9,FR1
        ADD         #1,R12
        FADD        FR9,FR2
        ADD         R12,R2
        FADD        FR9,FR3
        FMOV.S      FR2,@-R7
        MOV         #0,R12
        FMOV.S      FR0,@-R7
        PREF        @R7
        ADD         #32,R7
?check_near:
        FMOV.S      @R9,FR0
        FCMP/GT     FR0,FR9
        FMOV.S      @R10,FR2
        BF          ?clip
        FCMP/GT     FR2,FR9
        FMOV.S      @R11,FR0
        BF          ?clip
        FCMP/GT     FR0,FR9
        BF          ?clip
?next_triangle:
        ADD         R6,R4
        MOV         R4,R0
        MOV         R10,R9
        MOV         R11,R10
        DT          R8
        FMOV.S      FR12,FR10
        ADD         #6,R0
        FMOV.S      FR14,FR12
        PREF        @R0
        BT          ?strip_end
        BRA         ?triangle
        NOP
?strip_end:
        TST         R12,R12
        BT          ?strip_done
        FMOV.S      @R11+,FR0
        FMOV.S      @R11+,FR2
        ADD         #24,R7
        FMOV.S      FR14,@-R7
        FMOV.S      FR2,@-R7
        FMOV.S      FR0,@-R7
        MOV.L       R14,@R7
        PREF        @R7
        ADD         #32,R7
        FMOV.S      @R11+,FR0
        FMOV.S      @R11+,FR2
        FMUL        FR4,FR0
        FADD        FR5,FR1
        ADD         #-32,R11
        FADD        FR6,FR2
        FADD        FR7,FR3
        ADD         #32,R7
        FMOV.S      FR2,@-R7
        FMOV.S      FR0,@-R7
        FADD        FR9,FR1
        ADD         R12,R2
        FADD        FR9,FR2
        FADD        FR9,FR3
        FMOV.S      FR2,@-R7
        ADD         #1,R2
        FMOV.S      FR0,@-R7
        PREF        @R7
        ADD         #32,R7
?strip_done:
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
?clip:
        FSCHG
        MOV.L       R1,@-R15
        MOV.L       R2,@-R15
        MOV.L       R3,@-R15
        MOV.L       R4,@-R15
        MOV.L       R5,@-R15
        MOV.L       R6,@-R15
        FMOV.S      FR4,@-R15
        FMOV.S      FR5,@-R15
        FMOV.S      FR6,@-R15
        FMOV.S      FR7,@-R15
        FMOV.S      FR8,@-R15
        FADD        FR9,FR5
        FMOV.S      FR10,@-R15
        FADD        FR9,FR6
        FMOV.S      FR11,@-R15
        FADD        FR9,FR7
        MOV.L       ?near_clip,R1
        FMOV.S      FR4,FR1
        FMOV.S      FR5,FR2
        MOV         R9,R4
        MOV         R10,R5
        MOV         R11,R6
        FMOV.S      FR10,FR4
        FMOV.S      FR11,FR5
        FMOV.S      FR7,@-R15
        FMOV.S      FR6,@-R15
        FMOV.S      FR12,FR6
        FMOV.S      FR13,FR7
        FMOV.S      FR14,FR8
        FMOV.S      FR15,FR9
        FMOV.S      FR1,FR10
        JSR         @R1
        FMOV.S      FR2,FR11
        ADD         #8,R15
        MOV         R0,R7
        FMOV.S      @R15+,FR11
        FMOV.S      @R15+,FR10
        FMOV.S      @R15+,FR8
        FMOV.S      @R15+,FR7
        FMOV.S      @R15+,FR6
        FMOV.S      @R15+,FR5
        FMOV.S      @R15+,FR4
        MOV.L       @R15+,R6
        MOV.L       @R15+,R5
        MOV.L       @R15+,R4
        MOV.L       @R15+,R3
        MOV.L       @R15+,R2
        MOV.L       @R15+,R1
        FLDI1       FR9
        BRA         ?next_triangle
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
        .DATA.L     _njCnkEasyMultiTextureNearClip
}
#endif
/* clang-format on */
