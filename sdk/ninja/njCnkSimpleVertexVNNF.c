/* clang-format off */
#ifdef DREAMCAST
#include <nj_cnk.h>

/* the body hard-codes the offsets of all of *work's fields but ambient and sq,
 * which it skips. ?weight_scale is the largest float below 1 / 255 */
/* @asm opcode: movca, MOVCA.L R0,@R5 at 0x8c1450a0 allocates an entry's
 * cache line without reading the entry */
#pragma inline_asm(njCnkSimpleVertexVNNF)
Sint32 njCnkSimpleVertexVNNF(struct nj_cnk_simple_work *work, Sint32 head)
{
        FMOV.S      FR15,@-R15
        FMOV.S      FR14,@-R15
        FMOV.S      FR13,@-R15
        FMOV.S      FR12,@-R15
        FMOV.S      @R4+,FR12
        FMOV.S      @R4+,FR13
        FMOV.S      @R4+,FR14
        FLDI0       FR15
        FMOV.S      @R4+,FR0
        ADD         #4,R4
        MOV.L       @R4+,R1
        FMUL        FR0,FR12
        MOV.L       @R4+,R3
        FMUL        FR0,FR13
        MOV.L       @R4+,R7
        PREF        @R3
        ADD         #4,R4
        FMUL        FR0,FR14
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
        FTRV        XMTRX,FV0
        MOV.L       @R3+,R4
        EXTS.W      R4,R5
        SHLR16      R4
        SHLL2       R5
        LDS         R4,FPUL
        SHLL2       R5
        FLOAT       FPUL,FR7
        SHLL        R5
        ADD         R7,R5
        FMOV.S      @R2,FR3
        CMP/EQ      #0,R0
        FMUL        FR3,FR7
        BF          ?add
        MOVCA.L     R0,@R5
        FMUL        FR7,FR2
        FSCHG
        FMUL        FR7,FR1
        ADD         #32,R5
        FMUL        FR7,FR0
        FMOV        DR2,@-R5
        FMUL        FR7,FR6
        FMOV        DR0,@-R5
        FMUL        FR7,FR5
        FMOV        DR6,@-R5
        FMUL        FR7,FR4
        FMOV        DR4,@-R5
        BRA         ?next
        FSCHG
?add:
        PREF        @R5
        CMP/EQ      #1,R0
        BF          ?last
        FMOV.S      FR0,FR3
        FMOV.S      FR7,FR0
        FMOV.S      @R5,FR7
        FMAC        FR0,FR4,FR7
        FMOV.S      FR7,@R5
        ADD         #4,R5
        FMOV.S      @R5,FR7
        FMAC        FR0,FR5,FR7
        FMOV.S      FR7,@R5
        ADD         #4,R5
        FMOV.S      @R5,FR7
        FMAC        FR0,FR6,FR7
        FMOV.S      FR7,@R5
        ADD         #8,R5
        FMOV.S      @R5,FR7
        FMAC        FR0,FR3,FR7
        FMOV.S      FR7,@R5
        ADD         #4,R5
        FMOV.S      @R5,FR7
        FMAC        FR0,FR1,FR7
        FMOV.S      FR7,@R5
        ADD         #4,R5
        FMOV.S      @R5,FR7
        FMAC        FR0,FR2,FR7
        BRA         ?next
        FMOV.S      FR7,@R5
?last:
        FMOV.S      FR0,FR3
        FMOV.S      FR7,FR0
        FMOV.S      @R5+,FR7
        FMAC        FR0,FR4,FR7
        FMOV.S      FR7,FR4
        FMOV.S      @R5+,FR7
        FMAC        FR0,FR5,FR7
        FMOV.S      FR7,FR5
        FMOV.S      @R5+,FR7
        FMAC        FR0,FR6,FR7
        FMOV.S      FR7,FR6
        ADD         #4,R5
        FMOV.S      @R5+,FR7
        FMAC        FR0,FR3,FR7
        FMOV.S      FR7,FR3
        FMOV.S      @R5+,FR7
        FMAC        FR0,FR1,FR7
        FMOV.S      FR7,FR1
        FMOV.S      @R5,FR7
        FMAC        FR0,FR2,FR7
        FMOV.S      FR7,FR2
        ADD         #-24,R5
        FMOV.S      FR3,FR0
        FLDI1       FR7
        FDIV        FR6,FR7
        FMOV.S      FR6,@R5
        FIPR        FV12,FV0
        ADD         #32,R5
        FMUL        FR9,FR5
        FMOV.S      FR1,@-R5
        FMUL        FR8,FR4
        FMOV.S      FR0,@-R5
        FMUL        FR7,FR5
        FLDI1       FR2
        FMUL        FR7,FR4
        ADD         #-4,R5
        FCMP/GT     FR2,FR6
        FMOV.S      FR3,@-R5
        BT          ?far
        ADD         #1,R6
?far:
        FMOV.S      FR3,FR2
        FMOV.S      FR7,@-R5
        FADD        FR11,FR5
        FMUL        FR3,FR3
        FADD        FR10,FR4
        FMOV.S      FR5,@-R5
        FMUL        FR3,FR3
        FMOV.S      FR4,@-R5
        FMUL        FR3,FR3
        ADD         #16,R5
        FMUL        FR3,FR3
        FMUL        FR2,FR3
        FMOV.S      FR3,@R5
?next:
        DT          R1
        BF/S        ?vertex
        NOP
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
