#ifdef DREAMCAST
#include <ninja.h>

#ifdef NONMATCHING
/* this side keeps the console's bits. ninja_zam.h's helpers test n's sign as
 * the bodies do, and run the same FSQRT and FSRRA */
#undef njSqrt
#undef njInvertSqrt
Float njSqrt(Float n)
{
    return zam_njSqrt(n);
}

Float njInvertSqrt(Float n)
{
    return zam_njInvertSqrt(n);
}
#else
/* clang-format off */
/* @asm epilogue: dead-return, SHC's RTS at 0x8c11a548 follows the body's own
 * returns at 0x8c11a540 and 0x8c11a544 */
#pragma inline_asm(njSqrt)
Float njSqrt(Float n)
{
        FLDI0       FR0
        FCMP/GT     FR4,FR0
        BF/S        ?nonnegative
        FMOV        FR4,FR0
        FABS        FR0
        FSQRT       FR0
        RTS
        FNEG        FR0
?nonnegative:
        RTS
        FSQRT       FR0
}

/* @asm epilogue: dead-return, SHC's RTS at 0x8c11a560 follows the body's own
 * returns at 0x8c11a558 and 0x8c11a55c */
#pragma inline_asm(njInvertSqrt)
Float njInvertSqrt(Float n)
{
        FLDI0       FR0
        FCMP/GT     FR4,FR0
        BF/S        ?nonnegative
        FMOV        FR4,FR0
        FABS        FR0
        FSRRA       FR0
        RTS
        FNEG        FR0
?nonnegative:
        RTS
        FSRRA       FR0
}
/* clang-format on */
#endif
#endif
