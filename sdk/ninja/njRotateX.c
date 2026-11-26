#ifdef DREAMCAST
#include <ninja.h>

#ifdef NONMATCHING
/* turn m in memory, out of line, so that the path for XMTRX needs no stack
 * frame */
static void __attribute__((noinline, cold)) turn_rows(NJS_MATRIX *m, Angle ang)
{
    zam_turn_rows(m, ZAM_X, ang);
}

/* this side keeps the console's bits. SH4ZAM's turn about x runs the same FSCA
 * and two FTRVs on the same rows */
void njRotateX(NJS_MATRIX *m, Angle ang)
{
    if (m == NULL) {
        zam_turn(ZAM_X, ang);
    } else {
        turn_rows(m, ang);
    }
}
#else
/* clang-format off */
/* the turn differs from the unit matrix in rows 1 and 2, made from FSCA's sine
 * and cosine. FTRV multiplies each by XMTRX, and the products replace XMTRX's
 * rows 1 and 2. for m in memory, m's rows 1 and 2 stand in for XMTRX's while
 * the FTRVs run. the products then go back to m. XMTRX's own rows 1 and 2
 * wait in FR8 to FR15. so the caller's FR12 to FR15, which the call keeps,
 * wait on the stack */
/* @asm operand: ftrv-registers, FTRV XMTRX,FV4 at 0x8c119a16 multiplies the
 * turn's row for z */
#pragma inline_asm(njRotateX)
void njRotateX(NJS_MATRIX *m, Angle ang)
{
        TST         R4,R4
        BF/S        ?memory
        LDS         R5,FPUL
        FLDI0       FR3
        FSCA        FPUL,DR0
        FLDI0       FR4
        FLDI0       FR7
        FMOV.S      FR0,FR2
        FLDI0       FR0
        FMOV.S      FR2,FR5
        FMOV.S      FR1,FR6
        FNEG        FR5
        FTRV        XMTRX,FV0
        FTRV        XMTRX,FV4
        FSCHG
        FMOV        DR0,XD4
        FMOV        DR2,XD6
        FMOV        DR4,XD8
        FMOV        DR6,XD10
        RTS
        FSCHG
?memory:
        FLDI0       FR3
        FSCA        FPUL,DR0
        FLDI0       FR4
        FLDI0       FR7
        FMOV.S      FR0,FR2
        FLDI0       FR0
        FMOV.S      FR2,FR5
        FMOV.S      FR1,FR6
        FNEG        FR5
        FMOV.S      FR12,@-R15
        FMOV.S      FR13,@-R15
        FMOV.S      FR14,@-R15
        FMOV.S      FR15,@-R15
        FSCHG
        FMOV        XD4,DR8
        FMOV        XD6,DR10
        FMOV        XD8,DR12
        FMOV        XD10,DR14
        FSCHG
        FRCHG
        ADD         #16,R4
        FMOV.S      @R4+,FR4
        FMOV.S      @R4+,FR5
        FMOV.S      @R4+,FR6
        FMOV.S      @R4+,FR7
        FMOV.S      @R4+,FR8
        FMOV.S      @R4+,FR9
        FMOV.S      @R4+,FR10
        FMOV.S      @R4+,FR11
        FRCHG
        FTRV        XMTRX,FV0
        FTRV        XMTRX,FV4
        FMOV.S      FR7,@-R4
        FMOV.S      FR6,@-R4
        FMOV.S      FR5,@-R4
        FMOV.S      FR4,@-R4
        FMOV.S      FR3,@-R4
        FMOV.S      FR2,@-R4
        FMOV.S      FR1,@-R4
        FMOV.S      FR0,@-R4
        FSCHG
        FMOV        DR8,XD4
        FMOV        DR10,XD6
        FMOV        DR12,XD8
        FMOV        DR14,XD10
        FSCHG
        FMOV.S      @R15+,FR15
        FMOV.S      @R15+,FR14
        FMOV.S      @R15+,FR13
        RTS
        FMOV.S      @R15+,FR12
}
/* clang-format on */
#endif
#endif
