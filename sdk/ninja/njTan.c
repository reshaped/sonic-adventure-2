#ifdef DREAMCAST
#include <ninja.h>

#ifdef NONMATCHING
/* this side keeps the console's bits. ninja_zam.h's helper runs the same FSCA
 * and FDIV */
#undef njTan
Float njTan(Angle n)
{
    return zam_njTan(n);
}
#else
/* clang-format off */
/* @asm epilogue: dead-return, njTan's RTS at 0x8c11a570 follows
 * the hardware return at 0x8c11a56a */
#pragma inline_asm(tangent_from_phase)
static Float tangent_from_phase(Uint32 phase)
{
        LDS         R4,FPUL
        FSCA        FPUL,DR0
        RTS
        FDIV        FR1,FR0
}

/* clang-format on */

Float njTan(Angle angle)
{
    Uint32 phase = (Uint16)angle;

    return tangent_from_phase(phase);
}
#endif
#endif
