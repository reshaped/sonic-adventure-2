/* clang-format off */
#ifdef DREAMCAST
#include <nj_cnk.h>

/* njCnkEasyVertexVNNF takes NJD_CV_VN_NF chunks. the body reads all of
 * *work's fields but sq. it turns and scales work->light as
 * njCnkEasyVertexVN's does. the loop's FIPR takes each normal's fourth float as
 * 0, so work->ambient adds nothing to a sum. ?weight_scale is the largest
 * float below 1 / 255 */
/* @asm operand: fipr-registers, FIPR FV12,FV0 at 0x8c143a8a takes the x of
 * work->light turned by the current matrix's transpose */
#pragma inline_asm(njCnkEasyVertexVNNF)
Sint32 njCnkEasyVertexVNNF(struct nj_cnk_easy_work *work, Sint32 head)
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
        FMUL        FR0,FR14
        ADD         #4,R4
        FMOV.S      @R4+,FR8
        FMOV.S      @R4+,FR9
        FMOV.S      @R4+,FR10
        FMOV.S      @R4+,FR11
        MOVA        ?weight_scale,R0
        SHLR8       R5
        MOV         R0,R2
        EXTU.B      R5,R0
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
        FLDI0       FR3
        FIPR        FV12,FV0
        MOV.L       @R3+,R4
        EXTS.W      R4,R5
        SHLR16      R4
        SHLL2       R5
        LDS         R4,FPUL
        SHLL2       R5
        FLOAT       FPUL,FR0
        ADD         R7,R5
        FMOV.S      @R2,FR1
        PREF        @R5
        FMUL        FR1,FR0
        CMP/EQ      #0,R0
        BF          ?add
        FMUL        FR0,FR3
        ADD         #16,R5
        FMUL        FR0,FR6
        FMOV.S      FR3,@-R5
        FMUL        FR0,FR5
        FMOV.S      FR6,@-R5
        FMUL        FR0,FR4
        FMOV.S      FR5,@-R5
        BRA         ?next
        FMOV.S      FR4,@-R5
?add:
        CMP/EQ      #1,R0
        BF          ?last
        FMOV.S      @R5+,FR1
        FMAC        FR0,FR4,FR1
        FMOV.S      @R5+,FR2
        FMAC        FR0,FR5,FR2
        FMOV.S      @R5+,FR8
        FMAC        FR0,FR6,FR8
        FMOV.S      @R5+,FR9
        FMAC        FR0,FR3,FR9
        FMOV.S      FR9,@-R5
        FMOV.S      FR8,@-R5
        FMOV.S      FR2,@-R5
        BRA         ?next
        FMOV.S      FR1,@-R5
?last:
        FMOV.S      @R5+,FR1
        FMAC        FR0,FR4,FR1
        FMOV.S      FR1,FR4
        FMOV.S      @R5+,FR1
        FMAC        FR0,FR5,FR1
        FMOV.S      FR1,FR5
        FMOV.S      @R5+,FR2
        FMAC        FR0,FR6,FR2
        FMOV.S      @R5+,FR1
        FMAC        FR0,FR3,FR1
        FLDI1       FR0
        FADD        FR15,FR1
        FDIV        FR2,FR0
        FCMP/GT     FR15,FR1
        BT          ?floor
        FMOV.S      FR15,FR1
?floor:
        FMOV.S      FR1,@-R5
        FLDI1       FR3
        FCMP/GT     FR3,FR2
        BT          ?far
        ADD         #1,R6
?far:
        FMUL        FR9,FR5
        FMUL        FR8,FR4
        FMUL        FR0,FR5
        FMOV.S      FR0,@-R5
        FMUL        FR0,FR4
        FADD        FR11,FR5
        FMOV.S      FR5,@-R5
        FADD        FR10,FR4
        FMOV.S      FR4,@-R5
?next:
        DT          R1
        BF          ?vertex
        FMOV.S      @R15+,FR12
        FMOV.S      @R15+,FR13
        FMOV.S      @R15+,FR14
        FMOV.S      @R15+,FR15
        RTS
        MOV         R6,R0
        .ALIGN      4
?weight_scale:
        .DATA.L     H'3B808080
}
#endif
/* clang-format on */
