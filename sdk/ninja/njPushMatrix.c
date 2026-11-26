#ifdef DREAMCAST
#include <ninja.h>

#ifdef NONMATCHING
/* this side keeps the shipped result. ninja_zam.h's helper makes the same
 * depth test. it stores XMTRX by the same MOVCA.L and pair stores */
#undef njPushMatrix
Bool njPushMatrix(NJS_MATRIX *m)
{
    return zam_njPushMatrix(m);
}
#else
/* clang-format off */
/* the push stores XMTRX, the current matrix, in the slot at
 * _nj_current_matrix_ptr_. it then moves the pointer up a slot. a null m
 * stores XMTRX in the new slot too. any other m is loaded into XMTRX, and the
 * new slot is left unwritten. MOVCA.L allocates each of a slot's two cache
 * lines without reading memory. that is why the stack njInitMatrix takes must
 * start on a 32-byte boundary */
/* @asm opcode: movca, MOVCA.L R0,@R5 at 0x8c1197c6 takes the current slot's
 * first cache line */
#pragma inline_asm(njPushMatrix)
Bool njPushMatrix(NJS_MATRIX *m)
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
        TST         R4,R4
        MOVCA.L     R0,@R5
        ADD         #32,R5
        MOVCA.L     R0,@R5
        ADD         #32,R5
        FSCHG
        FMOV        XD14,@-R5
        FMOV        XD12,@-R5
        FMOV        XD10,@-R5
        FMOV        XD8,@-R5
        FMOV        XD6,@-R5
        FMOV        XD4,@-R5
        FMOV        XD2,@-R5
        FMOV        XD0,@-R5
        FSCHG
        ADD         #64,R5
        BF/S        ?load
        MOV.L       R5,@R1
        MOVCA.L     R0,@R5
        ADD         #32,R5
        MOVCA.L     R0,@R5
        ADD         #32,R5
        FSCHG
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
?load:
        FSCHG
        FMOV        @R4+,XD0
        FMOV        @R4+,XD2
        FMOV        @R4+,XD4
        FMOV        @R4+,XD6
        FMOV        @R4+,XD8
        FMOV        @R4+,XD10
        FMOV        @R4+,XD12
        FMOV        @R4+,XD14
        FSCHG
        RTS
        MOV         #1,R0
}
/* clang-format on */
#endif
#endif
