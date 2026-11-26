#ifdef DREAMCAST
#include <ninja.h>

#ifdef NONMATCHING
/* Share the checked stack contract with inline callers. */
#undef njPopMatrix
Bool njPopMatrix(Uint32 n)
{
    return zam_njPopMatrix(n);
}
#else
Bool njPopMatrix(Uint32 count)
{
    Sint32 *depth = &_nj_stack_size_;

    /* The subtraction is unsigned, so only an equal depth and count fail. */
    /* @hack spelling: the comparison with one, where a zero test would do */
    if ((Uint32)*depth - count < 1) {
        return FALSE;
    }

    /* SHC enters this block with count in R4, depth in R5 and the
     * subtraction's result in R3. It stays in this function so the pop
     * can use all three registers. The RTS at 0x8c119704 returns TRUE.
     * SHC also emits its usual unreachable return at 0x8c11970c. */
    /* clang-format off */
    /* @asm open: no inspect rule reads any of its instructions */
#pragma asm
        .IMPORT     __nj_current_matrix_ptr_
        SHLL2       R4
        MOV.L       R3,@R5
        SHLL2       R4
        MOV.L       #__nj_current_matrix_ptr_,R5
        SHLL2       R4
        MOV.L       @R5,R1
        SUB         R4,R1
        MOV.L       R1,@R5
        FSCHG
        FMOV        @R1+,XD0
        FMOV        @R1+,XD2
        FMOV        @R1+,XD4
        FMOV        @R1+,XD6
        FMOV        @R1+,XD8
        FMOV        @R1+,XD10
        FMOV        @R1+,XD12
        FMOV        @R1+,XD14
        FSCHG
        RTS
        MOV         #1,R0
#pragma endasm
    /* clang-format on */
}
#endif
#endif
