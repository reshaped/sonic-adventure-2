/* clang-format off */
#ifdef DREAMCAST
#include <ninja.h>

/* the body reads _nj_screen_'s w at byte offset 4, and sets its dist at 0 */
/* @asm epilogue: dead-return, SHC's RTS at 0x8c12a67c follows the body's own
 * return at 0x8c12a66e and its pool */
#pragma inline_asm(njSetPerspective)
void njSetPerspective(Angle ang)
{
        .IMPORT     __nj_screen_
        .IMPORT     _njTan
        STS.L       PR,@-R15
        MOV.L       #_njTan,R3
        JSR         @R3
        SHAR        R4
        MOV.L       #__nj_screen_+4,R3
        FMOV        FR0,FR3
        FMOV.S      @R3,FR2
        FADD        FR3,FR0
        FDIV        FR0,FR2
        ADD         #-4,R3
        LDS.L       @R15+,PR
        RTS
        FMOV.S      FR2,@R3
}
#endif
/* clang-format on */
