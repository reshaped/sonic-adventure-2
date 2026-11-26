#ifdef DREAMCAST
#include <ninja.h>

#ifdef NONMATCHING
/* this side keeps the console's bits: ninja_zam.h's helper copies the same
 * three Floats, from XMTRX or from m */
#undef njGetTranslation
void njGetTranslation(NJS_MATRIX *m, NJS_POINT3 *p)
{
    zam_njGetTranslation(m, p);
}
#else
/* clang-format off */
/* The three predecrement stores write z, y and x. */
/* @asm epilogue: dead-return, njGetTranslation's RTS at 0x8c118bd8 follows
 * the hardware return at 0x8c118bd4. */
#pragma inline_asm(store_translation_backwards)
static void store_translation_backwards(NJS_MATRIX *matrix, NJS_POINT3 *end)
{
        TST         R4,R4
        BF          ?memory
        FSCHG
        FMOV        XD12,DR0
        FMOV        XD14,DR2
        BRA         ?store
        FSCHG
?memory:
        ADD         #48,R4
        FMOV.S      @R4+,FR0
        FMOV.S      @R4+,FR1
        FMOV.S      @R4+,FR2
?store:
        FMOV.S      FR2,@-R5
        FMOV.S      FR1,@-R5
        RTS
        FMOV.S      FR0,@-R5
}

/* clang-format on */

void njGetTranslation(NJS_MATRIX *matrix, NJS_POINT3 *point)
{
    ++point;
    store_translation_backwards(matrix, point);
}
#endif
#endif
