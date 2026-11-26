#ifdef DREAMCAST
#include <ninja.h>

#ifdef NONMATCHING
/* this side keeps the console's bits. ninja_zam.h's helper runs the same
 * FIPR */
#undef njScalor2
Float njScalor2(NJS_VECTOR *v)
{
    return zam_njScalor2(v);
}
#else
/* clang-format off */
/* @asm operand: fipr-registers, FIPR FV4,FV4 at 0x8c11a0b0 takes v's length
 * squared, with FR7 cleared for the fourth part */
#pragma inline_asm(njScalor2)
Float njScalor2(NJS_VECTOR *v)
{
        FLDI0       FR7
        FMOV.S      @R4+,FR4
        FMOV.S      @R4+,FR5
        FMOV.S      @R4+,FR6
        FIPR        FV4,FV4
        RTS
        FMOV.S      FR7,FR0
}
/* clang-format on */
#endif
#endif
