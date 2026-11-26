#ifdef DREAMCAST
#include <ninja.h>

#ifdef NONMATCHING
/* this side keeps the console's bits. ninja_zam.h's helper runs the same FTRV
 * for the current matrix, and the same four FIPRs, in the shipped operand
 * order, for a matrix in memory */
#undef njTranslate
void njTranslate(NJS_MATRIX *m, Float x, Float y, Float z)
{
    zam_njTranslate(m, x, y, z);
}
#else
/* clang-format off */
/* only the matrix's row 3 changes, to (x, y, z, 1) times the matrix. FTRV
 * forms the product for the current matrix. for a matrix in memory, one
 * FIPR forms each float */
/* @asm operand: ftrv-registers, FTRV XMTRX,FV4 at 0x8c11a1de makes the
 * current matrix's new row 3 */
#pragma inline_asm(njTranslate)
void njTranslate(NJS_MATRIX *m, Float x, Float y, Float z)
{
        TST         R4,R4
        BF/S        ?memory
        FLDI1       FR7
        FTRV        XMTRX,FV4
        FSCHG
        FMOV        DR4,XD12
        FMOV        DR6,XD14
        RTS
        FSCHG
?memory:
        FMOV.S      @R4,FR0
        MOV         #16,R0
        FMOV.S      @(R0,R4),FR1
        MOV         #32,R0
        FMOV.S      @(R0,R4),FR2
        MOV         #48,R0
        FMOV.S      @(R0,R4),FR3
        FIPR        FV4,FV0
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
        RTS
        FMOV.S      FR3,@(R0,R4)
}
/* clang-format on */
#endif
#endif
