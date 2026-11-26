#ifdef DREAMCAST
#include <ninja.h>

#ifdef NONMATCHING
/* this side keeps the console's bits. ninja_zam.h's helper runs the same
 * FSCA */
#undef njSin
Float njSin(Angle n)
{
    return zam_njSin(n);
}
#else
/* clang-format off */
/* @asm epilogue: dead-return, njSin's RTS at 0x8c11a530 follows
 * the hardware return at 0x8c11a52c */
#pragma inline_asm(sine_from_phase)
static Float sine_from_phase(Uint32 phase)
{
        LDS         R4,FPUL
        RTS
        FSCA        FPUL,DR0
}

/* clang-format on */

Float njSin(Angle angle)
{
    Uint32 phase = (Uint16)angle;

    return sine_from_phase(phase);
}
#endif
#endif
