#ifdef DREAMCAST
#include <ninja.h>

#ifdef NONMATCHING
/* this side keeps the console's bits. it copies ms as it is. XMTRX takes it
 * by pairs where ms lies on an 8-byte boundary, else by single floats. md
 * takes it by words */
void njSetMatrix(NJS_MATRIX *md, NJS_MATRIX *ms)
{
    if (md == NULL) {
        shz_xmtrx_load_unaligned_4x4(*ms);
    } else {
        zam_copy_matrix(*md, *ms);
    }
}
#else
/* clang-format off */
/* for a null md, FRCHG turns the registers' banks over. the sixteen loads of
 * ms then land in XMTRX. for any other md the copy goes eight floats at a
 * time through FR0 to FR7 */
/* @asm epilogue: dead-return, SHC's RTS at 0x8c11a140 follows the body's own
 * return at 0x8c11a13c */
#pragma inline_asm(njSetMatrix)
void njSetMatrix(NJS_MATRIX *md, NJS_MATRIX *ms)
{
        TST         R4,R4
        BF          ?memory
        FRCHG
        FMOV.S      @R5+,FR0
        FMOV.S      @R5+,FR1
        FMOV.S      @R5+,FR2
        FMOV.S      @R5+,FR3
        FMOV.S      @R5+,FR4
        FMOV.S      @R5+,FR5
        FMOV.S      @R5+,FR6
        FMOV.S      @R5+,FR7
        FMOV.S      @R5+,FR8
        FMOV.S      @R5+,FR9
        FMOV.S      @R5+,FR10
        FMOV.S      @R5+,FR11
        FMOV.S      @R5+,FR12
        FMOV.S      @R5+,FR13
        FMOV.S      @R5+,FR14
        FMOV.S      @R5+,FR15
        RTS
        FRCHG
?memory:
        ADD         #32,R4
        FMOV.S      @R5+,FR0
        FMOV.S      @R5+,FR1
        FMOV.S      @R5+,FR2
        FMOV.S      @R5+,FR3
        FMOV.S      @R5+,FR4
        FMOV.S      @R5+,FR5
        FMOV.S      @R5+,FR6
        FMOV.S      @R5+,FR7
        FMOV.S      FR7,@-R4
        FMOV.S      FR6,@-R4
        FMOV.S      FR5,@-R4
        FMOV.S      FR4,@-R4
        FMOV.S      FR3,@-R4
        FMOV.S      FR2,@-R4
        FMOV.S      FR1,@-R4
        FMOV.S      FR0,@-R4
        FMOV.S      @R5+,FR0
        FMOV.S      @R5+,FR1
        ADD         #64,R4
        FMOV.S      @R5+,FR2
        FMOV.S      @R5+,FR3
        FMOV.S      @R5+,FR4
        FMOV.S      @R5+,FR5
        FMOV.S      @R5+,FR6
        FMOV.S      @R5+,FR7
        FMOV.S      FR7,@-R4
        FMOV.S      FR6,@-R4
        FMOV.S      FR5,@-R4
        FMOV.S      FR4,@-R4
        FMOV.S      FR3,@-R4
        FMOV.S      FR2,@-R4
        FMOV.S      FR1,@-R4
        RTS
        FMOV.S      FR0,@-R4
}
/* clang-format on */
#endif
#endif
