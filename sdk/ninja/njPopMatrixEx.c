#ifdef DREAMCAST
#include <ninja.h>

#ifdef NONMATCHING
/* this side keeps the shipped result. ninja_zam.h's helper makes the same
 * signed depth test, and loads the same slot */
#undef njPopMatrixEx
Bool njPopMatrixEx(void)
{
    return zam_njPopMatrixEx();
}
#else
/* SHC expands this static helper at its only call. It returns the depth in
 * R0 and leaves its address in R5 for the success path's store. */
/* clang-format off */
/* @asm open: SHC emits both loads for C with different registers. */
#pragma inline_asm(read_stack_depth)
static Uint32 read_stack_depth(void)
{
        .IMPORT     __nj_stack_size_
        MOV.L       #__nj_stack_size_,R5
        MOV.L       @R5,R0
}
/* clang-format on */

Bool njPopMatrixEx(void)
{
    /* The SH4 decrements a 32-bit word and tests its signed result. */
    if ((Sint32)(read_stack_depth() - 1) <= 0) {
        return FALSE;
    }

    /* Store the decremented depth from R0 and load the previous matrix into
     * XMTRX. SHC's return at 0x8c118964 is unreachable after this RTS. */
    /* clang-format off */
    /* @asm open: no inspect rule reads any of its instructions */
#pragma asm
        .IMPORT     __nj_current_matrix_ptr_
        MOV.L       R0,@R5
        MOV.L       #__nj_current_matrix_ptr_,R5
        MOV.L       @R5,R1
        ADD         #-64,R1
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
