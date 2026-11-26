/* clang-format off */
#ifdef DREAMCAST
#include <nj_cnk.h>

/* njCnkEasyVertexVND8 takes NJD_CV_VN_D8 chunks, whose vertex is a point, a
 * normal and a packed ARGB word. the function skips the normal. each entry's
 * inten holds the word's bits unchanged. the body reads *work's fields from
 * nbvertex, 20 bytes in, to cy */
/* @asm opcode: cache-line, OCBI @R7 at 0x8c143a56 drops the line of a block
 * the store queue writes */
#pragma inline_asm(njCnkEasyVertexVND8)
Sint32 njCnkEasyVertexVND8(struct nj_cnk_easy_work *work)
{
        ADD         #20,R4
        MOV.L       @R4+,R1
        MOV.L       @R4+,R3
        MOV.L       @R4+,R7
        PREF        @R3
        MOV.L       @R4+,R2
        MOV         R3,R5
        ADD         #16,R5
        FMOV.S      @R4+,FR8
        FMOV.S      @R4+,FR9
        FMOV.S      @R4+,FR10
        FMOV.S      @R4+,FR11
        PREF        @R5
        MOV         #0,R0
        MOV         #0,R6
?vertex:
        FMOV.S      @R3+,FR4
        FMOV.S      @R3+,FR5
        ADD         #28,R5
        FMOV.S      @R3+,FR6
        FLDI1       FR7
        FTRV        XMTRX,FV4
        PREF        @R5
        FLDI1       FR0
        ADD         #12,R3
        FDIV        FR6,FR0
        FLDI1       FR2
        ADD         #16,R2
        FCMP/GT     FR2,FR6
        FMOV.S      @R3+,FR1
        BT          ?far
        ADD         #1,R6
?far:
        FMOV.S      FR10,FR2
        FMUL        FR8,FR4
        FMOV.S      FR11,FR3
        FMUL        FR9,FR5
        FSCHG
        FMAC        FR0,FR4,FR2
        ADD         #1,R0
        FMAC        FR0,FR5,FR3
        TST         #1,R0
        FMOV        DR0,@-R2
        BF/S        ?half
        FMOV        DR2,@-R2
        OCBI        @R7
        ADD         #32,R7
        PREF        @R2
?half:
        DT          R1
        FSCHG
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
