/* clang-format off */
#ifdef DREAMCAST
#include <nj_cnk.h>

/* njCnkEasyVertexVI takes NJD_CV_IN chunks, whose vertex is a point and a
 * word. the word's high halfword is a signed diffuse intensity. its low
 * halfword is a specular intensity, which the function ignores. each entry's
 * inten is the high halfword times ?inten_scale. ?inten_scale is the largest
 * float below 1 / 32768. the body reads *work's fields from nbvertex, 20
 * bytes in, to cy */
/* @asm opcode: cache-line, OCBI @R7 at 0x8c143920 drops the line of a block
 * the store queue writes */
#pragma inline_asm(njCnkEasyVertexVI)
Sint32 njCnkEasyVertexVI(struct nj_cnk_easy_work *work)
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
        MOVA        ?inten_scale,R0
        FMOV.S      @R0,FR9
        PREF        @R5
        MOV         #0,R0
        MOV         #0,R6
        FLDI1       FR11
?vertex:
        FMOV.S      @R3+,FR4
        FMOV.S      @R3+,FR5
        FMOV.S      @R3+,FR6
        FLDI1       FR7
        FTRV        XMTRX,FV4
        MOV.L       @R3+,R4
        FLDI1       FR8
        ADD         #16,R5
        FDIV        FR6,FR8
        ADD         #16,R2
        PREF        @R5
        SHLR16      R4
        EXTS.W      R4,R4
        LDS         R4,FPUL
        FLOAT       FPUL,FR10
        FMUL        FR9,FR10
        FMOV.S      FR10,@-R2
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
        .ALIGN      4
?inten_scale:
        .DATA.L     H'37FFFFFF
}
#endif
/* clang-format on */
