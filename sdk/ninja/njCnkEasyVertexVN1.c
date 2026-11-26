/* clang-format off */
#ifdef DREAMCAST
#include <nj_cnk.h>

/* njCnkEasyVertexVN1 takes NJD_CV_VN_UF, NJD_CV_VN_S5, NJD_CV_VN_S4 and
 * NJD_CV_VN_IN chunks. their vertex is a point, a normal and a word. the
 * function skips the word. the body lights a vertex as njCnkEasyVertexVN's
 * does */
/* @asm opcode: cache-line, OCBI @R7 at 0x8c1439da drops the line of a block
 * the store queue writes */
#pragma inline_asm(njCnkEasyVertexVN1)
Sint32 njCnkEasyVertexVN1(struct nj_cnk_easy_work *work)
{
        FMOV.S      FR15,@-R15
        FMOV.S      FR14,@-R15
        FMOV.S      FR13,@-R15
        FMOV.S      FR12,@-R15
        FMOV.S      @R4+,FR12
        FMOV.S      @R4+,FR13
        FMOV.S      @R4+,FR14
        FLDI0       FR15
        FSCHG
        FMOV        XD0,DR0
        FMOV        XD2,DR2
        FIPR        FV12,FV0
        FMOV        XD4,DR4
        FMOV        XD6,DR6
        FIPR        FV12,FV4
        FMOV        XD8,DR8
        FMOV        XD10,DR10
        FIPR        FV12,FV8
        FSCHG
        FMOV.S      FR3,FR12
        FMOV.S      FR7,FR13
        FMOV.S      FR11,FR14
        FMOV.S      @R4+,FR0
        FMOV.S      @R4+,FR15
        MOV.L       @R4+,R1
        FMUL        FR0,FR12
        MOV.L       @R4+,R3
        FMUL        FR0,FR13
        MOV.L       @R4+,R7
        PREF        @R3
        MOV.L       @R4+,R2
        MOV         R3,R5
        FMUL        FR0,FR14
        ADD         #28,R5
        FMOV.S      @R4+,FR0
        FMOV.S      @R4+,FR1
        FMOV.S      @R4+,FR2
        FMOV.S      @R4+,FR3
        PREF        @R5
        MOV         #0,R0
        MOV         #0,R6
?vertex:
        FMOV.S      @R3+,FR4
        FMOV.S      @R3+,FR5
        FMOV.S      @R3+,FR6
        FLDI1       FR7
        FTRV        XMTRX,FV4
        FMOV.S      @R3+,FR8
        FMOV.S      @R3+,FR9
        FMOV.S      @R3+,FR10
        FLDI1       FR11
        FIPR        FV12,FV8
        FLDI1       FR7
        ADD         #28,R5
        FDIV        FR6,FR7
        PREF        @R5
        ADD         #4,R3
        FCMP/GT     FR15,FR11
        ADD         #16,R2
        BT          ?floor
        FMOV.S      FR15,FR11
?floor:
        FMOV.S      FR11,@-R2
        FLDI1       FR11
        FCMP/GT     FR11,FR6
        BT          ?far
        ADD         #1,R6
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
        FMOV.S      @R15+,FR12
        FMOV.S      @R15+,FR13
        FMOV.S      @R15+,FR14
        FMOV.S      @R15+,FR15
        RTS
        MOV         R6,R0
}
#endif
/* clang-format on */
