#ifdef DREAMCAST
#include <ninja.h>

#ifdef NONMATCHING
/* this side keeps the console's bits. ninja_zam.h's helper runs the same FTRV
 * for the current matrix, and the same four FIPRs, in the shipped operand
 * order, for a matrix in memory */
#undef njTranslateV
void njTranslateV(NJS_MATRIX *m, NJS_VECTOR *v)
{
    zam_njTranslateV(m, v);
}
#else
/* clang-format off */
/* only the matrix's row 3 changes, to (v->x, v->y, v->z, 1) times the
 * matrix. FTRV forms the product for the current matrix. for a matrix in memory,
 * one FIPR forms each float */
/* @asm operand: ftrv-registers, FTRV XMTRX,FV4 at 0x8c11a1c8 makes the
 * current matrix's new row 3 */
#pragma inline_asm(njTranslateV)
void njTranslateV(NJS_MATRIX *m, NJS_VECTOR *v)
{
        FMOV.S      @R5+,FR4
        TST         R4,R4
        FMOV.S      @R5+,FR5
        FMOV.S      @R5+,FR6
        BT/S        ?current
        FLDI1       FR7
        MOV         #16,R0
        FMOV.S      @R4,FR0
        FMOV.S      @(R0,R4),FR1
        MOV         #32,R0
        FMOV.S      @(R0,R4),FR2
        MOV         #48,R0
        FMOV.S      @(R0,R4),FR3
        FIPR        FV4,FV0
        MOV         #48,R0
        FMOV.S      FR3,@(R0,R4)
        MOV         #4,R0
        FMOV.S      @(R0,R4),FR0
        MOV         #20,R0
        FMOV.S      @(R0,R4),FR1
        MOV         #36,R0
        FMOV.S      @(R0,R4),FR2
        MOV         #52,R0
        FMOV.S      @(R0,R4),FR3
        FIPR        FV4,FV0
        MOV         #52,R0
        FMOV.S      FR3,@(R0,R4)
        MOV         #8,R0
        FMOV.S      @(R0,R4),FR0
        MOV         #24,R0
        FMOV.S      @(R0,R4),FR1
        MOV         #40,R0
        FMOV.S      @(R0,R4),FR2
        MOV         #56,R0
        FMOV.S      @(R0,R4),FR3
        FIPR        FV4,FV0
        MOV         #56,R0
        FMOV.S      FR3,@(R0,R4)
        MOV         #12,R0
        FMOV.S      @(R0,R4),FR0
        MOV         #28,R0
        FMOV.S      @(R0,R4),FR1
        MOV         #44,R0
        FMOV.S      @(R0,R4),FR2
        MOV         #60,R0
        FMOV.S      @(R0,R4),FR3
        FIPR        FV4,FV0
        MOV         #60,R0
        RTS
        FMOV.S      FR3,@(R0,R4)
?current:
        FTRV        XMTRX,FV4
        FSCHG
        FMOV        DR4,XD12
        FMOV        DR6,XD14
        RTS
        FSCHG
}
/* clang-format on */
#endif
#endif
