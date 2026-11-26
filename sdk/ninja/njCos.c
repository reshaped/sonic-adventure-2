#ifdef DREAMCAST
#include <ninja.h>

#ifdef NONMATCHING
/* this side keeps the console's bits. ninja_zam.h's helper runs the same
 * FSCA */
#undef njCos
Float njCos(Angle n)
{
    return zam_njCos(n);
}
#else
/* clang-format off */
/* @asm epilogue: dead-return, njCos's RTS at 0x8c11a3b8 follows
 * the hardware return at 0x8c11a3b2 */
#pragma inline_asm(cosine_from_phase)
static Float cosine_from_phase(Uint32 phase)
{
        LDS         R4,FPUL
        FSCA        FPUL,DR0
        RTS
        FMOV.S      FR1,FR0
}

/* clang-format on */

Float njCos(Angle angle)
{
    Uint32 phase = (Uint16)angle;

    return cosine_from_phase(phase);
}
#endif
#endif
