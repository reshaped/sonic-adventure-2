/* clang-format off */
#ifdef DREAMCAST
#include <nj_cnk.h>

/* njCnkEasyVertexVN takes NJD_CV_VN chunks, whose vertex is a point and a
 * normal. the body turns work->light by the current matrix's transpose once,
 * and scales it by work->inten. work->ambient follows it as a fourth float. a
 * FIPR of the four with a vertex's normal and a 1 gives ambient plus the
 * light's part */
/* @asm opcode: cache-line, OCBI @R7 at 0x8c143e0c drops the line of a block
 * the store queue writes */
#pragma inline_asm(njCnkEasyVertexVN)
Sint32 njCnkEasyVertexVN(struct nj_cnk_easy_work *work)
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
        ADD         #24,R5
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
        FMOV.S      @R3+,FR6
        FLDI1       FR7
        FTRV        XMTRX,FV4
        FMOV.S      @R3+,FR0
        FMOV.S      @R3+,FR1
        FMOV.S      @R3+,FR2
        FLDI1       FR3
        FIPR        FV12,FV0
        FLDI1       FR0
        ADD         #24,R5
        FDIV        FR6,FR0
        PREF        @R5
        FLDI1       FR1
        FCMP/GT     FR15,FR3
        ADD         #16,R2
        BT          ?floor
        FMOV.S      FR15,FR3
?floor:
        FCMP/GT     FR1,FR6
        FMOV.S      FR3,FR1
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
        FMOV.S      @R15+,FR12
        FMOV.S      @R15+,FR13
        FMOV.S      @R15+,FR14
        FMOV.S      @R15+,FR15
        RTS
        MOV         R6,R0
}
#endif
/* clang-format on */
