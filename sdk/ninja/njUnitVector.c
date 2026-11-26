#ifdef DREAMCAST
#include <ninja.h>

#ifdef NONMATCHING
/* this side keeps the console's bits. ninja_zam.h's helper runs the same FIPR
 * and FSRRA, and the FMULs by the root */
#undef njUnitVector
Float njUnitVector(NJS_VECTOR *v)
{
    return zam_njUnitVector(v);
}
#else
/* clang-format off */
/* v is scaled by FSRRA's reciprocal root of its length squared, which isn't
 * tested for 0. the result, v's old length, is the length squared times that
 * root */
/* @asm operand: fipr-registers, FIPR FV0,FV0 at 0x8c11a2b4 takes v's length
 * squared */
#pragma inline_asm(njUnitVector)
Float njUnitVector(NJS_VECTOR *v)
{
        FMOV.S      @R4+,FR0
        FMOV.S      @R4+,FR1
        FMOV.S      @R4+,FR2
        FLDI0       FR3
        FIPR        FV0,FV0
        FMOV.S      FR0,FR4
        FMOV.S      FR3,FR0
        FSRRA       FR3
        FMUL        FR3,FR2
        FMUL        FR3,FR1
        FMUL        FR3,FR4
        FMOV.S      FR2,@-R4
        FMOV.S      FR1,@-R4
        FMOV.S      FR4,@-R4
        RTS
        FMUL        FR3,FR0
}
/* clang-format on */
#endif
#endif
