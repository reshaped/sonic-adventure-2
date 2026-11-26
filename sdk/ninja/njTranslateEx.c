#ifdef DREAMCAST
#include <ninja.h>

#ifdef NONMATCHING
/* this side keeps the console's bits. ninja_zam.h's helper runs the same
 * FTRV */
#undef njTranslateEx
void njTranslateEx(NJS_VECTOR *v)
{
    zam_njTranslateEx(v);
}
#else
/* clang-format off */
/* only the current matrix's row 3 changes, to (v->x, v->y, v->z, 1) times
 * the matrix. FTRV forms the product */
/* @asm operand: ftrv-registers, FTRV XMTRX,FV4 at 0x8c118a94 makes the
 * current matrix's new row 3 */
#pragma inline_asm(njTranslateEx)
void njTranslateEx(NJS_VECTOR *v)
{
        FMOV.S      @R4+,FR4
        FMOV.S      @R4+,FR5
        FMOV.S      @R4+,FR6
        FLDI1       FR7
        FTRV        XMTRX,FV4
        FSCHG
        FMOV        DR4,XD12
        FMOV        DR6,XD14
        RTS
        FSCHG
}
/* clang-format on */
#endif
#endif
