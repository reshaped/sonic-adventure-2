/* clang-format off */
#ifdef DREAMCAST
#include <nj_cnk.h>

/* njCnkEasyVertexSHV takes NJD_CV_SH chunks, whose vertex is a point of four
 * floats. the current matrix takes the fourth as the point's w. each entry's
 * inten is 1. the body reads *work's fields from nbvertex, 20 bytes in, to
 * cy */
/* @asm opcode: cache-line, OCBI @R7 at 0x8c1437b6 drops the line of a block
 * the store queue writes */
#pragma inline_asm(njCnkEasyVertexSHV)
Sint32 njCnkEasyVertexSHV(struct nj_cnk_easy_work *work)
{
        ADD         #20,R4
        MOV.L       @R4+,R1
        MOV.L       @R4+,R3
        MOV.L       @R4+,R7
        PREF        @R3
        MOV.L       @R4+,R2
        MOV         R3,R5
        ADD         #16,R5
        FMOV.S      @R4+,FR0
        FMOV.S      @R4+,FR1
        FMOV.S      @R4+,FR2
        FMOV.S      @R4+,FR3
        PREF        @R5
        MOV         #0,R0
        MOV         #0,R6
        FLDI1       FR11
        FLDI1       FR9
?vertex:
        FSCHG
        FMOV        @R3+,DR4
        FMOV        @R3+,DR6
        FTRV        XMTRX,FV4
        FLDI1       FR8
        ADD         #16,R5
        FDIV        FR6,FR8
        ADD         #16,R2
        PREF        @R5
        FSCHG
        FMOV.S      FR9,@-R2
        FCMP/GT     FR11,FR6
        BT          ?far
        ADD         #1,R6
?far:
        FMUL        FR1,FR5
        FMUL        FR0,FR4
        FMUL        FR8,FR5
        FMOV.S      FR8,@-R2
        FMUL        FR8,FR4
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
}
#endif
/* clang-format on */
