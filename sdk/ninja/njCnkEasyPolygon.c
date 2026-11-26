/* clang-format off */
#ifdef DREAMCAST
#include <nj_cnk.h>

/* the body holds the two control words in float registers. a vertex's control
 * word and x go out as one pair. its inten and the end's control word go out
 * as another, which puts KM_VERTEXPARAM_ENDOFSTRIP in word 7 */
/* @asm operand: fschg-pairs, FMOV DR8,@-R7 at 0x8c1422c4 stores a vertex's
 * inten as one of a pair */
#pragma inline_asm(njCnkEasyPolygon)
Uint32 *njCnkEasyPolygon(Sint16 *strips, NJS_CNK_EASY_BUF *buf, Uint32 nbstrip,
                         Uint32 *sq)
{
        .IMPORT     __nj_calc_polygon_
        .IMPORT     __nj_draw_polygon_
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
        FMOV.S      @R0,FR9
        MOV         #0,R0
        MOV         #4,R9
?strip:
        MOV.W       @R4+,R2
        MOV.W       @R4+,R8
        SHLD        R9,R8
        CMP/PZ      R2
        ADD         R5,R8
        PREF        @R8
        BT          ?forward
        NEG         R2,R2
?forward:
        ADD         R2,R0
        FMOV.S      @R8+,FR1
        FMOV.S      @R8+,FR2
        ADD         R2,R1
        FMOV.S      @R8+,FR3
        ADD         #-2,R2
        FMOV.S      @R8+,FR8
        MOV.W       @R4+,R8
        FMOV.S      FR5,FR0
        SHLD        R9,R8
        FSCHG
        ADD         R5,R8
        PREF        @R8
        ADD         #32,R7
        FMOV        DR8,@-R7
        ADD         #-8,R7
        FMOV        DR2,@-R7
        FMOV        DR0,@-R7
        PREF        @R7
        ADD         #32,R7
        FSCHG
        FMOV.S      @R8+,FR1
        FMOV.S      @R8+,FR2
        FMOV.S      @R8+,FR3
        FMOV.S      @R8+,FR8
?vertex:
        MOV.W       @R4+,R8
        FSCHG
        SHLD        R9,R8
        ADD         R5,R8
        PREF        @R8
        ADD         #32,R7
        FMOV        DR8,@-R7
        ADD         #-8,R7
        FMOV        DR2,@-R7
        ADD         R6,R4
        FMOV        DR0,@-R7
        PREF        @R7
        FSCHG
        FMOV.S      @R8+,FR1
        ADD         #32,R7
        FMOV.S      @R8+,FR2
        DT          R2
        FMOV.S      @R8+,FR3
        BF/S        ?vertex
        FMOV.S      @R8+,FR8
        FMOV.S      FR9,FR0
        ADD         #32,R7
        FSCHG
        FMOV        DR8,@-R7
        ADD         #-8,R7
        FMOV        DR2,@-R7
        DT          R3
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
        RTS
        MOV.L       @R15+,R9
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
