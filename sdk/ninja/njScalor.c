#ifdef DREAMCAST
#include <ninja.h>

#ifdef NONMATCHING
/* this side keeps the console's bits. ninja_zam.h's helper runs the same
 * FIPR and FSQRT */
#undef njScalor
Float njScalor(NJS_VECTOR *v)
{
    return zam_njScalor(v);
}
#else
/* clang-format off */
/* @asm operand: fipr-registers, FIPR FV4,FV4 at 0x8c11a0c4 takes v's length
 * squared, which FSQRT then roots */
#pragma inline_asm(njScalor)
Float njScalor(NJS_VECTOR *v)
{
        FLDI0       FR7
        FMOV.S      @R4+,FR4
        FMOV.S      @R4+,FR5
        FMOV.S      @R4+,FR6
        FIPR        FV4,FV4
        FSQRT       FR7
        RTS
        FMOV.S      FR7,FR0
}
/* clang-format on */
#endif
#endif
