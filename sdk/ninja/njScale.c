#ifdef DREAMCAST
#include <ninja.h>

#ifdef NONMATCHING
/* this side keeps the console's bits. ninja_zam.h's helper runs the same
 * twelve FMULs. ninja_zam.h has no njScale macro for an #undef to remove */
void njScale(NJS_MATRIX *m, Float sx, Float sy, Float sz)
{
    zam_njScale(m, sx, sy, sz);
}
#else
/* clang-format off */
/* the scale multiplies rows 0, 1 and 2 by sx, sy and sz. row 3 stays as it
 * is. the current matrix's rows pass through FR0 to FR3 and FR8 to FR11
 * between FSCHGs. a matrix in memory is scaled in place, a float at a time */
/* @asm epilogue: dead-return, SHC's RTS at 0x8c11a0a4 follows the body's own
 * return at 0x8c11a09e */
#pragma inline_asm(njScale)
void njScale(NJS_MATRIX *m, Float sx, Float sy, Float sz)
{
        TST         R4,R4
        BF          ?memory
        FSCHG
        FMOV        XD0,DR0
        FMOV        XD2,DR2
        FMOV        XD4,DR8
        FMOV        XD6,DR10
        FSCHG
        FMUL        FR4,FR0
        FMUL        FR4,FR1
        FMUL        FR4,FR2
        FMUL        FR4,FR3
        FMUL        FR5,FR8
        FMUL        FR5,FR9
        FMUL        FR5,FR10
        FMUL        FR5,FR11
        FSCHG
        FMOV        DR0,XD0
        FMOV        DR2,XD2
        FMOV        DR8,XD4
        FMOV        DR10,XD6
        FSCHG
        FSCHG
        FMOV        XD8,DR0
        FMOV        XD10,DR2
        FSCHG
        FMUL        FR6,FR0
        FMUL        FR6,FR1
        FMUL        FR6,FR2
        FMUL        FR6,FR3
        FSCHG
        FMOV        DR0,XD8
        FMOV        DR2,XD10
        RTS
        FSCHG
?memory:
        MOV         R4,R5
        FMOV.S      @R4+,FR3
        FMUL        FR4,FR3
        FMOV.S      @R4+,FR2
        MOV         #4,R0
        FMUL        FR4,FR2
        FMOV.S      FR3,@R5
        FMOV.S      @R4+,FR1
        FMOV.S      FR2,@(R0,R5)
        FMUL        FR4,FR1
        FMOV.S      @R4+,FR3
        MOV         #8,R0
        FMUL        FR4,FR3
        FMOV.S      FR1,@(R0,R5)
        FMOV.S      @R4+,FR2
        MOV         #12,R0
        FMUL        FR5,FR2
        FMOV.S      FR3,@(R0,R5)
        FMOV.S      @R4+,FR1
        MOV         #16,R0
        FMUL        FR5,FR1
        FMOV.S      FR2,@(R0,R5)
        FMOV.S      @R4+,FR3
        MOV         #20,R0
        FMUL        FR5,FR3
        FMOV.S      FR1,@(R0,R5)
        FMOV.S      @R4+,FR2
        MOV         #24,R0
        FMUL        FR5,FR2
        FMOV.S      FR3,@(R0,R5)
        FMOV.S      @R4+,FR1
        MOV         #28,R0
        FMUL        FR6,FR1
        FMOV.S      FR2,@(R0,R5)
        FMOV.S      @R4+,FR3
        MOV         #32,R0
        FMUL        FR6,FR3
        FMOV.S      FR1,@(R0,R5)
        FMOV.S      @R4+,FR2
        MOV         #36,R0
        FMUL        FR6,FR2
        FMOV.S      FR3,@(R0,R5)
        FMOV.S      @R4,FR1
        MOV         #40,R0
        FMUL        FR6,FR1
        FMOV.S      FR2,@(R0,R5)
        RTS
        FMOV.S      FR1,@R4
}
/* clang-format on */
#endif
#endif
