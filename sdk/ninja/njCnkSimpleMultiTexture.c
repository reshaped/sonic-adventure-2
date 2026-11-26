/* clang-format off */
#ifdef DREAMCAST
#include <nj_cnk.h>

/* the body sets SZ and moves pairs as njCnkSimpleMultiPolygon's does. it
 * borrows XF0 to XF3, XF10 and XF11 from XMTRX, the current matrix. it
 * restores the six before it returns. XD10 holds v0's u and v. XD0 and XD2
 * carry an entry's nz, sr, sg and sb to its vertex's words 12 to 15 */
/* @asm operand: fschg-pairs, FMOV DR12,@-R7 at 0x8c1486aa stores a vertex's
 * a and r as a pair */
#pragma inline_asm(njCnkSimpleMultiTexture)
Uint32 *njCnkSimpleMultiTexture(Sint16 *strips, NJS_CNK_SIMPLEMULTI_BUF *buf,
                                Uint32 nbstrip, Uint32 *sq, Float uvscale,
                                Float a, Float ar, Float ag, Float ab)
{
        .IMPORT     __nj_calc_polygon_
        .IMPORT     __nj_control_3d_flag_
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
        FRCHG
        FMOV.S      FR0,@-R15
        FMOV.S      FR1,@-R15
        FMOV.S      FR2,@-R15
        FMOV.S      FR3,@-R15
        FMOV.S      FR10,@-R15
        FMOV.S      FR11,@-R15
        FRCHG
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
        MOV         #6,R0
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
        MOV         #6,R0
        FLOAT       FPUL,FR11
        MOV.W       @R4+,R10
        SHLD        R0,R10
        FMUL        FR4,FR10
        ADD         R5,R10
        FMUL        FR4,FR11
        PREF        @R10
        MOV.W       @R4+,R0
        LDS         R0,FPUL
        MOV.W       @R4+,R0
        FLOAT       FPUL,FR12
        LDS         R0,FPUL
        FLOAT       FPUL,FR13
        FMUL        FR4,FR12
        FMOV        DR10,XD10
        FMUL        FR4,FR13
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
        MOV         #6,R0
        FMOV.S      @R9+,FR0
        SHLD        R0,R11
        FMOV.S      @R9+,FR2
        FLOAT       FPUL,FR15
        FMOV.S      @R10+,FR1
        ADD         R5,R11
        FMOV.S      @R10+,FR3
        FMUL        FR4,FR14
        PREF        @R11
        FMUL        FR4,FR15
        ADD         #-12,R9
        FSUB        FR1,FR0
        ADD         #-12,R10
        FSUB        FR3,FR2
        ADD         #4,R11
        FMOV.S      @R11+,FR10
        ADD         #20,R4
        FMOV.S      @R11+,FR11
        ADD         #-12,R11
        FSUB        FR1,FR10
        FSUB        FR3,FR11
        PREF        @R4
        FMUL        FR0,FR11
        FMUL        FR2,FR10
        FLDI0       FR0
        FSUB        FR10,FR11
        ADD         #-20,R4
        FMUL        FR9,FR11
        FSCHG
        FCMP/GT     FR0,FR11
        FNEG        FR9
        BF/S        ?back
        TST         R12,R12
        BF          ?open_v1
        ADD         #32,R9
        PREF        @R9
        ADD         #-32,R9
        FMOV        @R9+,DR0
        ADD         #1,R12
        FMOV        @R9+,DR2
        ADD         #24,R7
        FMOV        XD10,@-R7
        FMOV        DR2,@-R7
        FMOV        DR0,@-R7
        MOV.L       R13,@R7
        PREF        @R7
        FMOV        @R9+,DR0
        ADD         #64,R7
        FMOV        @R9+,DR2
        ADD         #16,R9
        FMUL        FR5,FR0
        FMOV        @R9+,XD0
        FADD        FR6,FR1
        FMOV        @R9+,XD2
        FADD        FR7,FR2
        FMOV        XD2,@-R7
        FADD        FR8,FR3
        FMOV        XD0,@-R7
        ADD         #-64,R9
        FMOV        DR2,@-R7
        FMOV        DR0,@-R7
        PREF        @R7
        ADD         #32,R7
?open_v1:
        ADD         #32,R10
        PREF        @R10
        ADD         #-32,R10
        FMOV        @R10+,DR0
        ADD         #1,R12
        FMOV        @R10+,DR2
        ADD         #24,R7
        FMOV        DR12,@-R7
        FMOV        DR2,@-R7
        FMOV        DR0,@-R7
        MOV.L       R13,@R7
        PREF        @R7
        FMOV        @R10+,DR0
        ADD         #64,R7
        FMOV        @R10+,DR2
        ADD         #16,R10
        FMUL        FR5,FR0
        FMOV        @R10+,XD0
        FADD        FR6,FR1
        FMOV        @R10+,XD2
        FADD        FR7,FR2
        FMOV        XD2,@-R7
        FADD        FR8,FR3
        FMOV        XD0,@-R7
        ADD         #-64,R10
        FMOV        DR2,@-R7
        FMOV        DR0,@-R7
        PREF        @R7
        BRA         ?next
        ADD         #32,R7
?back:
        BT          ?next
        ADD         #32,R10
        PREF        @R10
        ADD         #-32,R10
        FMOV        @R10+,DR0
        ADD         #1,R12
        FMOV        @R10+,DR2
        ADD         #24,R7
        FMOV        DR12,@-R7
        FMOV        DR2,@-R7
        FMOV        DR0,@-R7
        ADD         R12,R2
        MOV.L       R14,@R7
        MOV         #0,R12
        PREF        @R7
        FMOV        @R10+,DR0
        ADD         #64,R7
        FMOV        @R10+,DR2
        ADD         #16,R10
        FMUL        FR5,FR0
        FMOV        @R10+,XD0
        FADD        FR6,FR1
        FMOV        @R10+,XD2
        FADD        FR7,FR2
        FMOV        XD2,@-R7
        FADD        FR8,FR3
        FMOV        XD0,@-R7
        ADD         #-64,R10
        FMOV        DR2,@-R7
        FMOV        DR0,@-R7
        PREF        @R7
        ADD         #32,R7
?next:
        MOV         R10,R9
        DT          R8
        MOV         R11,R10
        ADD         R6,R4
        BT          ?strip_end
        FMOV        DR12,XD10
        BRA         ?triangle
        FMOV        DR14,DR12
?strip_end:
        TST         R12,R12
        BT          ?strip_done
        ADD         #32,R11
        PREF        @R11
        ADD         #-32,R11
        FMOV        @R11+,DR0
        ADD         R12,R2
        FMOV        @R11+,DR2
        ADD         #24,R7
        FMOV        DR14,@-R7
        FMOV        DR2,@-R7
        FMOV        DR0,@-R7
        ADD         #1,R2
        MOV.L       R14,@R7
        PREF        @R7
        FMOV        @R11+,DR0
        ADD         #64,R7
        FMOV        @R11+,DR2
        ADD         #16,R11
        FMUL        FR5,FR0
        FMOV        @R11+,XD0
        FADD        FR6,FR1
        FMOV        @R11+,XD2
        FADD        FR7,FR2
        FMOV        XD2,@-R7
        FADD        FR8,FR3
        FMOV        XD0,@-R7
        ADD         #-64,R11
        FMOV        DR2,@-R7
        FMOV        DR0,@-R7
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
        ADD         #4,R15
        FRCHG
        FMOV.S      @R15+,FR11
        FMOV.S      @R15+,FR10
        FMOV.S      @R15+,FR3
        FMOV.S      @R15+,FR2
        FMOV.S      @R15+,FR1
        FMOV.S      @R15+,FR0
        FRCHG
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
?flag:
        .DATA.L     __nj_control_3d_flag_
}
#endif
/* clang-format on */
