/* clang-format off */
#ifdef DREAMCAST
#include <nj_cnk.h>

/* the body sets SZ by FSCHG, so each FMOV moves a pair of floats. a vertex is
 * its entry with the color lit, moved as four pairs. its control word then
 * replaces the entry's z in word 0 */
/* @asm operand: fschg-pairs, FMOV.S @R9+,FR12 at 0x8c1440f0 loads an
 * entry's a and r as a pair */
#pragma inline_asm(njCnkEasyMultiPolygon)
Uint32 *njCnkEasyMultiPolygon(Sint16 *strips, NJS_CNK_EASYMULTI_BUF *buf,
                              Sint32 nbstrip, Uint32 *sq, Float a, Float r,
                              Float g, Float b, Float ar, Float ag, Float ab)
{
        .IMPORT     __nj_calc_polygon_
        .IMPORT     __nj_draw_polygon_
        MOV.L       R8,@-R15
        MOV.L       R9,@-R15
        MOV.L       R10,@-R15
        MOV.L       R11,@-R15
        FMOV.S      FR12,@-R15
        FMOV.S      FR13,@-R15
        FMOV.S      FR14,@-R15
        FMOV.S      FR15,@-R15
        MOV.L       #H'00003FFF,R0
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
        SHLL        R6
        MOV.L       #H'E0000000,R10
        MOV.L       #H'F0000000,R11
        FSCHG
?strip:
        MOV.W       #H'0005,R0
        MOV.W       @R4+,R8
        MOV.W       @R4+,R9
        SHLD        R0,R9
        ADD         R5,R9
        PREF        @R9
        CMP/PZ      R8
        BT          ?forward
        NEG         R8,R8
?forward:
        ADD         R8,R3
        FMOV.S      @R9+,FR0
        ADD         R8,R2
        FMOV.S      @R9+,FR2
        ADD         #-2,R8
        FMOV.S      @R9+,FR12
        FMOV.S      @R9+,FR14
        FMUL        FR4,FR12
        MOV         #5,R0
        FMUL        FR5,FR13
        MOV.W       @R4+,R9
        FMUL        FR6,FR14
        SHLD        R0,R9
        FMUL        FR7,FR15
        ADD         R5,R9
        FADD        FR8,FR13
        PREF        @R9
        FADD        FR9,FR14
        FADD        FR10,FR15
        ADD         #32,R7
        FMOV.S      FR14,@-R7
        FMOV.S      FR12,@-R7
        FMOV.S      FR2,@-R7
        FMOV.S      FR0,@-R7
        MOV.L       R10,@R7
        PREF        @R7
        FMOV.S      @R9+,FR0
        FMOV.S      @R9+,FR2
        FMOV.S      @R9+,FR12
        FMOV.S      @R9+,FR14
        FMUL        FR4,FR12
        FMUL        FR5,FR13
        ADD         #32,R7
        FMUL        FR6,FR14
        FMUL        FR7,FR15
?triangle:
        MOV         #5,R0
        MOV.W       @R4+,R9
        FADD        FR8,FR13
        SHLD        R0,R9
        FADD        FR9,FR14
        ADD         R5,R9
        FADD        FR10,FR15
        PREF        @R9
        ADD         #32,R7
        FMOV.S      FR14,@-R7
        FMOV.S      FR12,@-R7
        ADD         R6,R4
        FMOV.S      FR2,@-R7
        FMOV.S      FR0,@-R7
        ADD         #16,R9
        MOV.L       R10,@R7
        PREF        @R7
        DT          R8
        FMOV.S      @R9+,FR12
        ADD         #32,R7
        FMUL        FR4,FR12
        FMOV.S      @R9+,FR14
        FMUL        FR5,FR13
        ADD         #-32,R9
        FMUL        FR6,FR14
        FMOV.S      @R9+,FR0
        FMUL        FR7,FR15
        BF/S        ?triangle
        FMOV.S      @R9+,FR2
        FADD        FR10,FR15
        ADD         #32,R7
        FADD        FR9,FR14
        FMOV.S      FR14,@-R7
        FADD        FR8,FR13
        FMOV.S      FR12,@-R7
        FMOV.S      FR2,@-R7
        DT          R1
        FMOV.S      FR0,@-R7
        MOV.L       R11,@R7
        PREF        @R7
        BF/S        ?strip
        ADD         #32,R7
        FSCHG
        MOV.L       #__nj_draw_polygon_,R4
        MOV         R7,R0
        MOV.L       #__nj_calc_polygon_,R5
        MOV.L       @R4,R6
        MOV.L       @R5,R7
        ADD         R2,R6
        ADD         R3,R7
        MOV.L       R6,@R4
        MOV.L       R7,@R5
        FMOV.S      @R15+,FR15
        FMOV.S      @R15+,FR14
        FMOV.S      @R15+,FR13
        FMOV.S      @R15+,FR12
        MOV.L       @R15+,R11
        MOV.L       @R15+,R10
        MOV.L       @R15+,R9
        RTS
        MOV.L       @R15+,R8
}
#endif
/* clang-format on */
