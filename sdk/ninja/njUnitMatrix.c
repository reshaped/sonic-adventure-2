#ifdef DREAMCAST
#include <ninja.h>

#ifdef NONMATCHING
/* this side keeps the console's bits: it copies _nj_unit_matrix_ as it is.
 * a null m loads _nj_unit_matrix_ into XMTRX by pairs, so _nj_unit_matrix_
 * must lie on 8 bytes */
void njUnitMatrix(NJS_MATRIX *m)
{
    if (m == NULL) {
        shz_xmtrx_load_4x4((const shz_mat4x4_t *)&_nj_unit_matrix_);
    } else {
        zam_copy_matrix(*m, _nj_unit_matrix_);
    }
}
#else
/* clang-format off */
/* @asm epilogue: dead-return, SHC's RTS at 0x8c11a2a8 follows the body's own
 * return at 0x8c11a2a2 */
#pragma inline_asm(njUnitMatrix)
void njUnitMatrix(NJS_MATRIX *m)
{
        .IMPORT     __nj_unit_matrix_
        TST         R4,R4
        MOV.L       #__nj_unit_matrix_,R5
        BF          ?memory
        FSCHG
        FMOV        @R5+,XD0
        FMOV        @R5+,XD2
        FMOV        @R5+,XD4
        FMOV        @R5+,XD6
        FMOV        @R5+,XD8
        FMOV        @R5+,XD10
        FMOV        @R5+,XD12
        FMOV        @R5+,XD14
        RTS
        FSCHG
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
