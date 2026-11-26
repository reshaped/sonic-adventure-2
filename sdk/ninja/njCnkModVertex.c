/* clang-format off */
#ifdef DREAMCAST
#include <nj_cnk.h>

/* ?near holds 1e-5, then 100000 */
/* @asm opcode: cache-line, OCBI @R7 at 0x8c145018 drops the line of a block
 * the store queue writes */
#pragma inline_asm(njCnkModVertex)
Sint32 njCnkModVertex(struct nj_cnk_mod_work *work)
{
        MOV.L       @R4+,R1
        MOV.L       @R4+,R3
        MOV.L       @R4+,R7
        PREF        @R3
        MOV.L       @R4+,R2
        MOV         R3,R5
        ADD         #12,R5
        FMOV.S      @R4+,FR0
        FMOV.S      @R4+,FR1
        FMOV.S      @R4+,FR2
        FMOV.S      @R4+,FR3
        MOVA        ?near,R0
        FMOV.S      @R0+,FR8
        FMOV.S      @R0,FR9
        PREF        @R5
        MOV         #0,R0
        MOV         #0,R6
?vertex:
        FMOV.S      @R3+,FR4
        FMOV.S      @R3+,FR5
        FMOV.S      @R3+,FR6
        FLDI1       FR7
        FTRV        XMTRX,FV4
        ADD         #12,R5
        FLDI1       FR7
        PREF        @R5
        FDIV        FR6,FR7
        ADD         #16,R2
        FMOV.S      FR6,@-R2
        FCMP/GT     FR8,FR6
        BT          ?far
        ADD         #1,R6
        FMOV        FR9,FR7
?far:
        FMUL        FR1,FR5
        FMUL        FR0,FR4
        FMUL        FR7,FR5
        FMOV.S      FR7,@-R2
        FMUL        FR7,FR4
        FADD        FR3,FR5
        ADD         #1,R0
        FMOV.S      FR5,@-R2
        TST         #1,R0
        FADD        FR2,FR4
        BF/S        ?half
        FMOV.S      FR4,@-R2
        OCBI        @R7
        ADD         #32,R7
        PREF        @R2
?half:
        DT          R1
        BF/S        ?vertex
        ADD         #16,R2
        TST         #1,R0
        BT          ?done
        OCBI        @R7
        PREF        @R2
?done:
        RTS
        MOV         R6,R0
        .ALIGN      4
?near:
        .DATA.L     H'3727C5AC
        .DATA.L     H'47C35000
}
#endif
/* clang-format on */
