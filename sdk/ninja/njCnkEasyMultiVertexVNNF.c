/* clang-format off */
#ifdef DREAMCAST
#include <nj_cnk.h>

/* the body turns and lights as njCnkEasyMultiVertexVN's does. where
 * CNK_FLAGS(head) is 0, the body writes all 32 bytes of the entry. MOVCA.L
 * then allocates the entry's cache line without reading memory. between the
 * FSCHGs SZ is set, so each FMOV.S there moves a pair of floats */
/* @asm opcode: rotcl, ROTCL R0 at 0x8c144d36 gathers a point light's facing
 * and range tests as bits of R0 */
#pragma inline_asm(njCnkEasyMultiVertexVNNF)
Sint32 njCnkEasyMultiVertexVNNF(struct nj_cnk_easy_multi_work *work, Sint32 head)
{
        .IMPORT     __nj_cnk_easy_multilight_buffer_mdst_
        FMOV.S      FR15,@-R15
        FMOV.S      FR14,@-R15
        FMOV.S      FR13,@-R15
        FMOV.S      FR12,@-R15
        MOV.L       R11,@-R15
        MOV.L       R10,@-R15
        MOV.L       R9,@-R15
        MOV.L       R8,@-R15
        MOV.L       @R4+,R3
        MOV.L       @R4+,R7
        MOV.L       @R4+,R6
        ADD         #4,R4
        MOVA        ?weight_scale,R0
        SHLR8       R5
        MOV         #0,R8
        EXTU.B      R5,R5
        MOV         R0,R11
?vertex:
        FMOV.S      @R7+,FR12
        FMOV.S      @R7+,FR13
        FMOV.S      @R7+,FR14
        FLDI1       FR15
        FTRV        XMTRX,FV12
        FMOV.S      @R7+,FR8
        FMOV.S      @R7+,FR9
        FMOV.S      @R7+,FR10
        FLDI0       FR11
        MOV.L       @R7+,R9
        FTRV        XMTRX,FV8
        EXTS.W      R9,R10
        SHLR16      R9
        SHLL2       R10
        LDS         R9,FPUL
        SHLL2       R10
        FLOAT       FPUL,FR0
        SHLL        R10
        ADD         R6,R10
        FMOV.S      @R11,FR1
        MOV         R5,R0
        FMUL        FR1,FR0
        CMP/EQ      #0,R0
        BF          ?blend
        MOVCA.L     R0,@R10
        FMUL        FR0,FR10
        FSCHG
        FMUL        FR0,FR9
        ADD         #32,R10
        FMUL        FR0,FR8
        FMOV.S      FR10,@-R10
        FMUL        FR0,FR14
        FMOV.S      FR8,@-R10
        FMUL        FR0,FR13
        FMOV.S      FR14,@-R10
        FMUL        FR0,FR12
        DT          R3
        FMOV.S      FR12,@-R10
        BRA         ?next
        FSCHG
?blend:
        PREF        @R10
        CMP/EQ      #1,R0
        BF          ?last
        FSCHG
        FMOV.S      @R10+,FR4
        FMOV.S      @R10+,FR6
        FMAC        FR0,FR12,FR4
        FMAC        FR0,FR13,FR5
        FMAC        FR0,FR14,FR6
        FMOV.S      FR6,@-R10
        FMOV.S      FR4,@-R10
        ADD         #16,R10
        FMOV.S      @R10+,FR4
        FMOV.S      @R10+,FR6
        FMAC        FR0,FR8,FR4
        FMAC        FR0,FR9,FR5
        FMAC        FR0,FR10,FR6
        FMOV.S      FR6,@-R10
        DT          R3
        FMOV.S      FR4,@-R10
        BRA         ?next
        FSCHG
?last:
        FMOV.S      @R10+,FR4
        FMAC        FR0,FR12,FR4
        FMOV.S      FR4,FR12
        FMOV.S      @R10+,FR4
        FMAC        FR0,FR13,FR4
        FMOV.S      FR4,FR13
        FMOV.S      @R10+,FR4
        FMAC        FR0,FR14,FR4
        FMOV.S      FR4,FR14
        ADD         #4,R10
        FMOV.S      @R10+,FR4
        FMAC        FR0,FR8,FR4
        FMOV.S      FR4,FR8
        FMOV.S      @R10+,FR4
        FMAC        FR0,FR9,FR4
        FMOV.S      FR4,FR9
        FMOV.S      @R10+,FR4
        FMAC        FR0,FR10,FR4
        FMOV.S      FR4,FR10
        MOV.L       #__nj_cnk_easy_multilight_buffer_mdst_,R1
        FLDI0       FR1
        MOV.L       @R1,R2
        FLDI0       FR2
        ADD         #16,R1
        FLDI0       FR3
?next_light:
        ROTR        R2
?light:
        PREF        @R1
        BT/S        ?light_on
        ROTR        R2
        ROTR        R2
        ADD         #32,R1
        BT          ?project
        BRA         ?light
        ROTR        R2
?light_on:
        FMOV.S      @R1+,FR4
        BT/S        ?point
        FMOV.S      @R1+,FR5
        FMOV.S      @R1+,FR6
        FLDI0       FR11
        FIPR        FV4,FV8
        FLDI0       FR7
        FCMP/GT     FR11,FR7
        ADD         #20,R1
        BT/S        ?lit
        ROTR        R2
        BT          ?project
        BRA         ?light
        ROTR        R2
?lit:
        ADD         #-12,R1
        FMOV.S      FR11,FR0
        FMOV.S      @R1+,FR4
        FNEG        FR0
        FMOV.S      @R1+,FR5
        FMAC        FR0,FR4,FR1
        FMOV.S      @R1+,FR6
        FMAC        FR0,FR5,FR2
        BT/S        ?project
        FMAC        FR0,FR6,FR3
        BRA         ?light
        ROTR        R2
?point:
        FSUB        FR12,FR4
        FMOV.S      @R1+,FR6
        FSUB        FR13,FR5
        FLDI0       FR7
        FSUB        FR14,FR6
        FLDI0       FR11
        FLDI0       FR0
        FIPR        FV4,FV8
        FMOV.S      @R1+,FR15
        FIPR        FV4,FV4
        MOV         #0,R0
        FCMP/GT     FR0,FR11
        FMOV.S      FR7,FR4
        ROTCL       R0
        FCMP/GT     FR7,FR15
        FSRRA       FR4
        ROTCL       R0
        CMP/EQ      #3,R0
        BT/S        ?in_range
        FMOV.S      @R1+,FR15
        ROTR        R2
        ADD         #12,R1
        BT          ?project
        BRA         ?light
        ROTR        R2
?in_range:
        FMUL        FR4,FR11
        FCMP/GT     FR15,FR7
        FMUL        FR4,FR4
        BF/S        ?shade
        FMOV.S      @R1+,FR5
        FMUL        FR15,FR11
        FMUL        FR4,FR11
?shade:
        FMOV.S      FR11,FR0
        ROTR        R2
        FMOV.S      @R1+,FR4
        FMAC        FR0,FR5,FR1
        FMOV.S      @R1+,FR6
        FMAC        FR0,FR4,FR2
        BF/S        ?next_light
        FMAC        FR0,FR6,FR3
?project:
        FLDI1       FR0
        FCMP/GT     FR14,FR0
        FDIV        FR14,FR0
        FMOV.S      @R4+,FR8
        ADD         #4,R10
        FMOV.S      @R4+,FR9
        FMOV.S      @R4+,FR10
        MOVT        R0
        FMOV.S      @R4+,FR11
        FLDI1       FR4
        ADD         #-16,R4
        FMOV.S      FR3,@-R10
        FMUL        FR13,FR9
        FMOV.S      FR2,@-R10
        FMUL        FR12,FR8
        FMOV.S      FR1,@-R10
        FMOV.S      FR4,@-R10
        FMAC        FR0,FR9,FR11
        FMOV.S      FR0,@-R10
        FMAC        FR0,FR8,FR10
        FMOV.S      FR11,@-R10
        DT          R3
        FMOV.S      FR10,@-R10
        ADD         R0,R8
        FMOV.S      FR14,@-R10
?next:
        BT          ?done
        BRA         ?vertex
        NOP
?done:
        MOV         R8,R0
        MOV.L       @R15+,R8
        MOV.L       @R15+,R9
        MOV.L       @R15+,R10
        MOV.L       @R15+,R11
        FMOV.S      @R15+,FR12
        FMOV.S      @R15+,FR13
        FMOV.S      @R15+,FR14
        RTS
        FMOV.S      @R15+,FR15
?weight_scale:
        .DATA.L     H'3B808080
}
#endif
/* clang-format on */
