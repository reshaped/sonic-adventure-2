#ifdef DREAMCAST
#include <ninja.h>

#ifdef NONMATCHING
/* this side keeps the console's bits. it copies XMTRX as it is. m takes it by
 * pairs where m lies on an 8-byte boundary, else by single floats */
void njGetMatrix(NJS_MATRIX *m)
{
    shz_xmtrx_store_unaligned_4x4(*m);
}
#else
/* clang-format off */
/* FRCHG exposes XMTRX. The sixteen predecrement stores write the matrix
 * from its last float to its first. */
/* @asm epilogue: fill, the body has no return of its own. it runs through
 * SHC's alignment NOP at 0x8c118bb2 into its RTS at 0x8c118bb4 */
#pragma inline_asm(store_xmtrx_backwards)
static void store_xmtrx_backwards(NJS_MATRIX *end)
{
        FRCHG
        FMOV.S      FR15,@-R4
        FMOV.S      FR14,@-R4
        FMOV.S      FR13,@-R4
        FMOV.S      FR12,@-R4
        FMOV.S      FR11,@-R4
        FMOV.S      FR10,@-R4
        FMOV.S      FR9,@-R4
        FMOV.S      FR8,@-R4
        FMOV.S      FR7,@-R4
        FMOV.S      FR6,@-R4
        FMOV.S      FR5,@-R4
        FMOV.S      FR4,@-R4
        FMOV.S      FR3,@-R4
        FMOV.S      FR2,@-R4
        FMOV.S      FR1,@-R4
        FMOV.S      FR0,@-R4
        FRCHG
}

/* clang-format on */

void njGetMatrix(NJS_MATRIX *matrix)
{
    NJS_MATRIX *end = matrix + 1;

    store_xmtrx_backwards(end);
}
#endif
#endif
