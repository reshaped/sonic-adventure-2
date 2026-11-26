#ifdef DREAMCAST
#include <ninja.h>

#ifdef NONMATCHING
/* this side keeps the shipped result. ninja_zam.h's helper makes the same
 * depth test. it stores XMTRX by the same MOVCA.L and pair stores. it leaves
 * the slot the pointer moves to as it was */
#undef njPushMatrixEx
Bool njPushMatrixEx(void)
{
    return zam_njPushMatrixEx();
}
#else
/* clang-format off */
/* the push stores XMTRX, the current matrix, in the slot at
 * _nj_current_matrix_ptr_. it then moves the pointer up a slot. MOVCA.L
 * allocates each of the slot's two cache lines without reading memory. that
 * is why the stack njInitMatrix takes must start on a 32-byte boundary */
/* @asm opcode: movca, MOVCA.L R0,@R5 at 0x8c118988 takes the current slot's
 * first cache line */
#pragma inline_asm(njPushMatrixEx)
Bool njPushMatrixEx(void)
{
        .IMPORT     __nj_max_stack_size_
        .IMPORT     __nj_stack_size_
        .IMPORT     __nj_current_matrix_ptr_
        MOV.L       #__nj_max_stack_size_,R1
        MOV.L       #__nj_stack_size_,R6
        MOV.L       @R1,R3
        MOV.L       @R6,R2
        CMP/GE      R3,R2
        BF          ?room
        RTS
        MOV         #0,R0
?room:
        ADD         #1,R2
        MOV.L       #__nj_current_matrix_ptr_,R1
        MOV.L       R2,@R6
        MOV.L       @R1,R5
        MOVCA.L     R0,@R5
        ADD         #32,R5
        MOVCA.L     R0,@R5
        ADD         #32,R5
        FSCHG
        MOV.L       R5,@R1
        FMOV        XD14,@-R5
        FMOV        XD12,@-R5
        FMOV        XD10,@-R5
        FMOV        XD8,@-R5
        FMOV        XD6,@-R5
        FMOV        XD4,@-R5
        FMOV        XD2,@-R5
        FMOV        XD0,@-R5
        FSCHG
        RTS
        MOV         #1,R0
}
/* clang-format on */
#endif
#endif
