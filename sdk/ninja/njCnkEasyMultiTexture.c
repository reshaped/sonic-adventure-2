/* clang-format off */
#ifdef DREAMCAST
#include <nj_cnk.h>

/* the body sets SZ by FSCHG, so each FMOV moves a pair of floats. a vertex's
 * first block takes three pairs: z and x, y and ooz, then u and v. its control
 * word then replaces the entry's z in word 0 */
/* @asm operand: fschg-pairs, FMOV.S FR10,@-R7 at 0x8c1444a6 stores a
 * vertex's u and v as a pair */
#pragma inline_asm(njCnkEasyMultiTexture)
Uint32 *njCnkEasyMultiTexture(Sint16 *strips, NJS_CNK_EASYMULTI_BUF *buf,
                              Sint32 nbstrip, Uint32 *sq, Float a, Float ar,
                              Float ag, Float ab, Float uvscale)
{
        .IMPORT     __nj_calc_polygon_
        .IMPORT     __nj_draw_polygon_
        MOV.L       R8,@-R15
        MOV.L       R9,@-R15
        MOV.L       R10,@-R15
        MOV.L       R11,@-R15
        MOV.L       R12,@-R15
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
        MOV         #0,R3
        FLDI1       FR9
        SHLL        R6
        FSUB        FR9,FR5
        FSUB        FR9,FR6
        FSUB        FR9,FR7
        FSCHG
        MOV.L       ?vertex_control,R11
        MOV.L       ?end_control,R12
?strip:
        MOV.W       ?index_shift,R0
        MOV.W       @R4+,R8
        MOV.W       @R4+,R9
        SHLD        R0,R9
        ADD         R5,R9
        PREF        @R9
        CMP/PZ      R8
        BT          ?forward
        NEG         R8,R8
?forward:
        MOV.W       @R4+,R0
        MOV.W       @R4+,R10
        LDS         R0,FPUL
        MOV         R4,R0
        FLOAT       FPUL,FR10
        ADD         #6,R0
        LDS         R10,FPUL
        PREF        @R0
        FLOAT       FPUL,FR11
        ADD         R8,R3
        FMUL        FR8,FR10
        ADD         R8,R2
        FMUL        FR8,FR11
        FMOV.S      @R9+,FR0
        ADD         #-2,R8
        FMOV.S      @R9+,FR2
        ADD         #24,R7
        FMOV.S      FR10,@-R7
        FMOV.S      FR2,@-R7
        FMOV.S      FR0,@-R7
        MOV.L       R11,@R7
        PREF        @R7
        ADD         #32,R7
        FMOV.S      @R9+,FR0
        MOV         #5,R0
        FMOV.S      @R9+,FR2
        MOV.W       @R4+,R9
        FADD        FR5,FR1
        SHLD        R0,R9
        FADD        FR6,FR2
        ADD         R5,R9
        FADD        FR7,FR3
        PREF        @R9
        ADD         #32,R7
        FMUL        FR4,FR0
        FMOV.S      FR2,@-R7
        FADD        FR9,FR3
        FMOV.S      FR0,@-R7
        FADD        FR9,FR2
        FMOV.S      FR2,@-R7
        FADD        FR9,FR1
        FMOV.S      FR0,@-R7
        PREF        @R7
        ADD         #32,R7
        MOV.W       @R4+,R0
        MOV.W       @R4+,R10
        LDS         R0,FPUL
        FMOV.S      @R9+,FR0
        FLOAT       FPUL,FR10
        FMOV.S      @R9+,FR2
        LDS         R10,FPUL
        MOV         R4,R0
        FLOAT       FPUL,FR11
        ADD         #6,R0
        FMUL        FR8,FR10
        PREF        @R0
        FMUL        FR8,FR11
?triangle:
        ADD         #24,R7
        FMOV.S      FR10,@-R7
        FMOV.S      FR2,@-R7
        FMOV.S      FR0,@-R7
        MOV.L       R11,@R7
        PREF        @R7
        ADD         #32,R7
        FMOV.S      @R9+,FR0
        MOV         #5,R0
        FMOV.S      @R9+,FR2
        MOV.W       @R4+,R9
        FADD        FR5,FR1
        SHLD        R0,R9
        FADD        FR6,FR2
        ADD         R5,R9
        FADD        FR7,FR3
        PREF        @R9
        FMUL        FR4,FR0
        ADD         #32,R7
        FMOV.S      FR2,@-R7
        FADD        FR9,FR3
        FMOV.S      FR0,@-R7
        FADD        FR9,FR2
        FMOV.S      FR2,@-R7
        FADD        FR9,FR1
        FMOV.S      FR0,@-R7
        PREF        @R7
        ADD         #32,R7
        MOV.W       @R4+,R0
        DT          R8
        MOV.W       @R4+,R10
        LDS         R0,FPUL
        ADD         R6,R4
        FLOAT       FPUL,FR10
        MOV         R4,R0
        LDS         R10,FPUL
        FLOAT       FPUL,FR11
        ADD         #6,R0
        FMUL        FR8,FR10
        PREF        @R0
        FMUL        FR8,FR11
        FMOV.S      @R9+,FR0
        BF/S        ?triangle
        FMOV.S      @R9+,FR2
        ADD         #24,R7
        FMOV.S      FR10,@-R7
        FMOV.S      FR2,@-R7
        FMOV.S      FR0,@-R7
        MOV.L       R12,@R7
        PREF        @R7
        ADD         #32,R7
        FMOV.S      @R9+,FR0
        FMOV.S      @R9+,FR2
        FADD        FR5,FR1
        FADD        FR6,FR2
        FADD        FR7,FR3
        FMUL        FR4,FR0
        ADD         #32,R7
        FMOV.S      FR2,@-R7
        FADD        FR9,FR3
        FMOV.S      FR0,@-R7
        FADD        FR9,FR2
        DT          R1
        FMOV.S      FR2,@-R7
        FADD        FR9,FR1
        FMOV.S      FR0,@-R7
        PREF        @R7
        ADD         #32,R7
        BT          ?done
        BRA         ?strip
        NOP
?index_shift:
        .DATA.W     H'0005
?done:
        MOV.L       ?draw,R4
        MOV         R7,R0
        MOV.L       ?calc,R5
        MOV.L       @R4,R6
        MOV.L       @R5,R7
        ADD         R2,R6
        ADD         R3,R7
        MOV.L       R6,@R4
        MOV.L       R7,@R5
        FSCHG
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
}
#endif
/* clang-format on */
