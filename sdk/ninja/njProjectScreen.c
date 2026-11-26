/* clang-format off */
#ifdef DREAMCAST
#include <ninja.h>

#ifdef NONMATCHING
/* the body keeps Ninja's name. engine/dc/port.h's macro gives the game's
 * calls the port's projection, which runs this one */
#undef njProjectScreen
#endif

/* the body reads _nj_screen_'s dist, cx and cy at byte offsets 0, 12 and 16 */
/* @asm operand: ftrv-registers, FTRV XMTRX,FV4 at 0x8c11971e transforms p3 by
 * the current matrix */
#pragma inline_asm(njProjectScreen)
void njProjectScreen(NJS_MATRIX *m, NJS_POINT3 *p3, NJS_POINT2 *p2)
{
        .IMPORT     __nj_screen_
        .IMPORT     __nj_scr_aspect_x_
        .IMPORT     __nj_scr_aspect_y_
        MOV.L       #__nj_screen_,R3
        TST         R4,R4
        BF          ?memory
        FMOV.S      @R5+,FR4
        FLDI1       FR7
        FMOV.S      @R5+,FR5
        FMOV.S      @R5,FR6
        FTRV        XMTRX,FV4
?divide:
        FLDI1       FR7
        FDIV        FR6,FR7
        FMOV.S      @R3,FR2
        FMUL        FR4,FR2
        FMUL        FR7,FR2
        MOV.L       #__nj_scr_aspect_x_,R2
        FMOV.S      @R2,FR1
        FMUL        FR1,FR2
        MOV         #12,R0
        FMOV.S      @(R0,R3),FR1
        FADD        FR1,FR2
        FMOV.S      @R3,FR3
        FMUL        FR5,FR3
        FMUL        FR7,FR3
        MOV.L       #__nj_scr_aspect_y_,R2
        FMOV.S      @R2,FR1
        FMUL        FR1,FR3
        MOV         #16,R0
        FMOV.S      @(R0,R3),FR1
        FADD        FR1,FR3
        MOV         #4,R0
        FMOV.S      FR2,@R6
        RTS
        FMOV.S      FR3,@(R0,R6)
?memory:
        FMOV.S      @R5+,FR8
        FLDI1       FR11
        FMOV.S      @R5+,FR9
        FMOV.S      @R5,FR10
        MOV         #16,R0
        FMOV.S      @R4,FR0
        FMOV.S      @(R0,R4),FR1
        MOV         #32,R0
        FMOV.S      @(R0,R4),FR2
        MOV         #48,R0
        FMOV.S      @(R0,R4),FR3
        FIPR        FV8,FV0
        FMOV.S      FR3,FR4
        MOV         #4,R0
        FMOV.S      @(R0,R4),FR0
        MOV         #20,R0
        FMOV.S      @(R0,R4),FR1
        MOV         #36,R0
        FMOV.S      @(R0,R4),FR2
        MOV         #52,R0
        FMOV.S      @(R0,R4),FR3
        FIPR        FV8,FV0
        FMOV.S      FR3,FR5
        MOV         #8,R0
        FMOV.S      @(R0,R4),FR0
        MOV         #24,R0
        FMOV.S      @(R0,R4),FR1
        MOV         #40,R0
        FMOV.S      @(R0,R4),FR2
        MOV         #56,R0
        FMOV.S      @(R0,R4),FR3
        FIPR        FV8,FV0
        BRA         ?divide
        FMOV.S      FR3,FR6
}
#endif
/* clang-format on */
