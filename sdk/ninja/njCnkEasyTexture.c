/* clang-format off */
#ifdef DREAMCAST
#include <nj_cnk.h>

/* the body holds the two control words in float registers. a vertex's control
 * word and x then go out as one pair */
/* @asm operand: fschg-pairs, FMOV DR8,@-R7 at 0x8c1435f6 stores a vertex's
 * base and offset intensities as a pair */
#pragma inline_asm(njCnkEasyTexture)
Uint32 *njCnkEasyTexture(Sint16 *strips, NJS_CNK_EASY_BUF *buf, Uint32 nbstrip,
                         Uint32 *sq, Float uvscale)
{
        .IMPORT     __nj_calc_polygon_
        .IMPORT     __nj_draw_polygon_
        MOV.L       R10,@-R15
        MOV.L       R9,@-R15
        MOV.L       R8,@-R15
        MOV         R6,R3
        MOVA        ?mask,R0
        MOV.L       @R0+,R1
        AND         R1,R3
        MOV         #-14,R2
        SHLD        R2,R6
        MOV         #3,R2
        MOV         #0,R1
        AND         R2,R6
        FMOV.S      @R0+,FR5
        SHLL        R6
        FMOV.S      @R0,FR10
        MOV         #0,R0
        FLDI1       FR11
?strip:
        MOV.W       @R4+,R2
        MOV.W       @R4+,R8
        SHLL2       R8
        MOV.W       @R4+,R9
        SHLL2       R8
        LDS         R9,FPUL
        CMP/PZ      R2
        FLOAT       FPUL,FR6
        ADD         R5,R8
        FMUL        FR4,FR6
        PREF        @R8
        BT          ?forward
        NEG         R2,R2
?forward:
        MOV.W       @R4+,R9
        ADD         R2,R0
        LDS         R9,FPUL
        FMOV.S      @R8+,FR1
        FLOAT       FPUL,FR7
        FMOV.S      @R8+,FR2
        FMUL        FR4,FR7
        ADD         R2,R1
        FMOV.S      @R8+,FR3
        ADD         #-2,R2
        FMOV.S      @R8+,FR8
        MOV.W       @R4+,R8
        SHLL2       R8
        FMOV.S      FR5,FR0
        SHLL2       R8
        FMOV.S      FR8,FR9
        FSCHG
        ADD         R5,R8
        PREF        @R8
        FSUB        FR11,FR9
        ADD         #32,R7
        FMOV        DR8,@-R7
        MOV.W       @R4+,R9
        FMOV        DR6,@-R7
        LDS         R9,FPUL
        FMOV        DR2,@-R7
        FLOAT       FPUL,FR6
        FMOV        DR0,@-R7
        FMUL        FR4,FR6
        PREF        @R7
        ADD         #32,R7
        FSCHG
        MOV.W       @R4+,R9
        FMOV.S      @R8+,FR1
        LDS         R9,FPUL
        FMOV.S      @R8+,FR2
        FLOAT       FPUL,FR7
        FMOV.S      @R8+,FR3
        FMUL        FR4,FR7
        FMOV.S      @R8+,FR8
        FMOV.S      FR8,FR9
?vertex:
        MOV.W       @R4+,R8
        SHLL2       R8
        MOV.W       @R4+,R9
        SHLL2       R8
        MOV.W       @R4+,R10
        ADD         R5,R8
        FSCHG
        PREF        @R8
        FSUB        FR11,FR9
        ADD         #32,R7
        FMOV        DR8,@-R7
        LDS         R9,FPUL
        FMOV        DR6,@-R7
        FLOAT       FPUL,FR6
        FMOV        DR2,@-R7
        FMUL        FR4,FR6
        FMOV        DR0,@-R7
        LDS         R10,FPUL
        FLOAT       FPUL,FR7
        PREF        @R7
        FSCHG
        ADD         R6,R4
        FMOV.S      @R8+,FR1
        ADD         #32,R7
        FMOV.S      @R8+,FR2
        FMUL        FR4,FR7
        FMOV.S      @R8+,FR3
        DT          R2
        FMOV.S      @R8+,FR8
        BF/S        ?vertex
        FMOV.S      FR8,FR9
        ADD         #32,R7
        FMOV.S      FR10,FR0
        FSUB        FR11,FR9
        FSCHG
        FMOV        DR8,@-R7
        FMOV        DR6,@-R7
        DT          R3
        FMOV        DR2,@-R7
        FMOV        DR0,@-R7
        PREF        @R7
        FSCHG
        BF/S        ?strip
        ADD         #32,R7
        MOV.L       ?draw,R2
        MOV.L       ?calc,R3
        MOV.L       @R2,R4
        MOV.L       @R3,R5
        ADD         R0,R4
        ADD         R1,R5
        MOV.L       R4,@R2
        MOV.L       R5,@R3
        MOV         R7,R0
        MOV.L       @R15+,R8
        MOV.L       @R15+,R9
        RTS
        MOV.L       @R15+,R10
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
