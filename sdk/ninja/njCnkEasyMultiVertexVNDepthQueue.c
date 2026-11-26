/* clang-format off */
#ifdef DREAMCAST
#include <nj_cnk.h>

/* the body is njCnkEasyMultiVertexVN's, but scales each entry's a by the depth
 * queue's fade. it reads _nj_depth_far_ and _nj_depth_range_ as the two floats
 * after _nj_depth_near_, through its address. the three must lie in that
 * order. its pool holds the addresses of _nj_depth_far_ and _nj_depth_range_
 * too, which nothing loads */
/* @asm opcode: rotcl, ROTCL R0 at 0x8c14495a gathers a point light's facing
 * and range tests as bits of R0 */
#pragma inline_asm(njCnkEasyMultiVertexVNDepthQueue)
Sint32 njCnkEasyMultiVertexVNDepthQueue(struct nj_cnk_easy_multi_work *work)
{
        .IMPORT     __nj_depth_far_
        .IMPORT     __nj_depth_near_
        .IMPORT     __nj_depth_range_
        .IMPORT     __nj_cnk_easy_multilight_buffer_mdst_
        FMOV.S      FR15,@-R15
        FMOV.S      FR14,@-R15
        FMOV.S      FR13,@-R15
        FMOV.S      FR12,@-R15
        MOV.L       R8,@-R15
        MOV.L       R9,@-R15
        MOV.L       @R4+,R3
        MOV.L       @R4+,R5
        MOV.L       @R4+,R6
        MOV.L       @R4+,R7
        MOV         #0,R8
?vertex:
        FMOV.S      @R5+,FR12
        FMOV.S      @R5+,FR13
        FMOV.S      @R5+,FR14
        FLDI1       FR15
        FTRV        XMTRX,FV12
        FMOV.S      @R5+,FR8
        FMOV.S      @R5+,FR9
        FMOV.S      @R5+,FR10
        FLDI0       FR11
        FTRV        XMTRX,FV8
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
        MOV.L       ?depth,R9
        ADD         #32,R7
        FMOV.S      @R9+,FR8
        FLDI1       FR0
        FCMP/GT     FR8,FR14
        FMOV.S      @R9+,FR9
        BF          ?near
        FCMP/GT     FR14,FR9
        FMOV.S      @R9,FR11
        BF          ?far
        FMOV.S      FR14,FR9
        FSUB        FR8,FR9
        FLDI1       FR15
        FMUL        FR11,FR9
        BRA         ?fogged
        FSUB        FR9,FR15
?far:
        BRA         ?fogged
        FLDI0       FR15
?near:
        FLDI1       FR15
?fogged:
        FCMP/GT     FR14,FR0
        FDIV        FR14,FR0
        FMOV.S      @R4+,FR8
        FMOV.S      @R4+,FR9
        ADD         #24,R5
        FMOV.S      @R4+,FR10
        MOVT        R0
        FMOV.S      @R4+,FR11
        PREF        @R5
        ADD         #-16,R4
        FMOV.S      FR3,@-R7
        FMUL        FR13,FR9
        FMOV.S      FR2,@-R7
        FMUL        FR12,FR8
        FMOV.S      FR1,@-R7
        FMOV.S      FR15,@-R7
        FMAC        FR0,FR9,FR11
        FMOV.S      FR0,@-R7
        FMAC        FR0,FR8,FR10
        FMOV.S      FR11,@-R7
        ADD         #-24,R5
        FMOV.S      FR10,@-R7
        DT          R3
        OCBI        @R6
        ADD         R0,R8
        FMOV.S      FR14,@-R7
        ADD         #32,R6
        PREF        @R7
        BT/S        ?done
        ADD         #32,R7
        BRA         ?vertex
?done:
        MOV         R8,R0
        MOV.L       @R15+,R9
        MOV.L       @R15+,R8
        FMOV.S      @R15+,FR12
        FMOV.S      @R15+,FR13
        FMOV.S      @R15+,FR14
        RTS
        FMOV.S      @R15+,FR15
?depth:
        .DATA.L     __nj_depth_near_
        .DATA.L     __nj_depth_far_
        .DATA.L     __nj_depth_range_
}
#endif
/* clang-format on */
