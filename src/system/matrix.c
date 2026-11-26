/* clang-format off */
#ifdef DREAMCAST
#include <ninja.h>

#include "system/matrix.h"

/* Ninja's current matrix lives in XMTRX, the FPU's back bank of sixteen
 * floats, a row to each four. FTRV XMTRX,FVn turns the row vector FVn into
 * FVn times that matrix. FRCHG swaps the back bank with the front. between
 * two FSCHGs, an FMOV moves two floats at once */

/* the rows of matrix load as pairs into FV0, FV4, FV8 and FV12. FTRV
 * takes each row times the current matrix. FRCHG then makes the four
 * products XMTRX */
/* @asm operand: ftrv-registers, FTRV XMTRX,FV4 at 0x8c02d258 transforms
 * matrix's second row by the current matrix */
#pragma inline_asm(matrix_multiply_before)
void matrix_multiply_before(const NJS_MATRIX *matrix)
{
        FMOV.S      FR15,@-R15
        FMOV.S      FR14,@-R15
        FMOV.S      FR13,@-R15
        FMOV.S      FR12,@-R15
        FSCHG
        FMOV        @R4+,DR0
        FMOV        @R4+,DR2
        FMOV        @R4+,DR4
        FMOV        @R4+,DR6
        FTRV        XMTRX,FV0
        FMOV        @R4+,DR8
        FMOV        @R4+,DR10
        FTRV        XMTRX,FV4
        FMOV        @R4+,DR12
        FMOV        @R4+,DR14
        FTRV        XMTRX,FV8
        FTRV        XMTRX,FV12
        FSCHG
        FRCHG
        FMOV.S      @R15+,FR12
        FMOV.S      @R15+,FR13
        FMOV.S      @R15+,FR14
        RTS
        FMOV.S      @R15+,FR15
}

/* @asm epilogue: dead-return, SHC's RTS at 0x8c02d28c follows the body's own
 * return at 0x8c02d286 */
#pragma inline_asm(matrix_set)
void matrix_set(const NJS_MATRIX *matrix)
{
        FSCHG
        FMOV        @R4+,XD0
        FMOV        @R4+,XD2
        FMOV        @R4+,XD4
        FMOV        @R4+,XD6
        FMOV        @R4+,XD8
        FMOV        @R4+,XD10
        FMOV        @R4+,XD12
        FMOV        @R4+,XD14
        RTS
        FSCHG
}

#ifdef NONMATCHING
/* SH4ZAM's store of XMTRX: the 16 floats the pairs below store */
void matrix_get(NJS_MATRIX *matrix)
{
    shz_xmtrx_store_4x4((shz_mat4x4_t *)matrix);
}
#else
/* an FMOV store has no @Rn+ form, so the pairs go down from matrix's end */
/* @asm epilogue: dead-return, SHC's RTS at 0x8c02d2a8 follows the body's own
 * return at 0x8c02d2a4 */
#pragma inline_asm(store_matrix_pairs(size=22))
static void store_matrix_pairs(Float *end)
{
        FSCHG
        FMOV        XD14,@-R4
        FMOV        XD12,@-R4
        FMOV        XD10,@-R4
        FMOV        XD8,@-R4
        FMOV        XD6,@-R4
        FMOV        XD4,@-R4
        FMOV        XD2,@-R4
        FMOV        XD0,@-R4
        RTS
        FSCHG
}

void matrix_get(NJS_MATRIX *matrix)
{
    Float *end = *matrix + 16;

    store_matrix_pairs(end);
}
#endif

/* matrix loads a float at a time into the front bank. the first FRCHG swaps
 * it into XMTRX, and the current matrix's rows into FV0, FV4, FV8 and FV12.
 * FTRV takes each row times matrix, and the second FRCHG makes the four
 * products XMTRX */
/* @asm operand: ftrv-registers, FTRV XMTRX,FV4 at 0x8c02d2d8 transforms the
 * current matrix's second row by matrix */
#pragma inline_asm(matrix_multiply_after)
void matrix_multiply_after(const NJS_MATRIX *matrix)
{
        FMOV.S      FR15,@-R15
        FMOV.S      FR14,@-R15
        FMOV.S      FR13,@-R15
        FMOV.S      FR12,@-R15
        FMOV.S      @R4+,FR0
        FMOV.S      @R4+,FR1
        FMOV.S      @R4+,FR2
        FMOV.S      @R4+,FR3
        FMOV.S      @R4+,FR4
        FMOV.S      @R4+,FR5
        FMOV.S      @R4+,FR6
        FMOV.S      @R4+,FR7
        FMOV.S      @R4+,FR8
        FMOV.S      @R4+,FR9
        FMOV.S      @R4+,FR10
        FMOV.S      @R4+,FR11
        FMOV.S      @R4+,FR12
        FMOV.S      @R4+,FR13
        FMOV.S      @R4+,FR14
        FMOV.S      @R4+,FR15
        FRCHG
        FTRV        XMTRX,FV0
        FTRV        XMTRX,FV4
        FTRV        XMTRX,FV8
        FTRV        XMTRX,FV12
        FRCHG
        FMOV.S      @R15+,FR12
        FMOV.S      @R15+,FR13
        FMOV.S      @R15+,FR14
        RTS
        FMOV.S      @R15+,FR15
}

/* matrix_multiply_after, with matrix loaded as pairs */
/* @asm operand: ftrv-registers, FTRV XMTRX,FV4 at 0x8c02d310 transforms the
 * current matrix's second row by matrix */
#pragma inline_asm(matrix_multiply_after_pairs)
void matrix_multiply_after_pairs(const NJS_MATRIX *matrix)
{
        FMOV.S      FR15,@-R15
        FMOV.S      FR14,@-R15
        FMOV.S      FR13,@-R15
        FMOV.S      FR12,@-R15
        FSCHG
        FMOV        @R4+,DR0
        FMOV        @R4+,DR2
        FMOV        @R4+,DR4
        FMOV        @R4+,DR6
        FMOV        @R4+,DR8
        FMOV        @R4+,DR10
        FMOV        @R4+,DR12
        FMOV        @R4+,DR14
        FRCHG
        FSCHG
        FTRV        XMTRX,FV0
        FTRV        XMTRX,FV4
        FTRV        XMTRX,FV8
        FTRV        XMTRX,FV12
        FRCHG
        FMOV.S      @R15+,FR12
        FMOV.S      @R15+,FR13
        FMOV.S      @R15+,FR14
        RTS
        FMOV.S      @R15+,FR15
}
#endif
/* clang-format on */
