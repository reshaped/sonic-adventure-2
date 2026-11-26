/* clang-format off */
#ifdef DREAMCAST
#include <nj_draw3d.h>

/* the float at ?t2_uv_scale is 1 / 256. no load reads the unlabeled
 * H'FFFFFFFF above ?t2_near */
/* @bug memory: reads colors from low memory where p->col is null. CMP/EQ
 * R1,R8 at 0x8c13c5b0 gives a point white only where R8 is 0, but ADD R4,R8 at
 * 0x8c13c59a has added 4 * index to it. MOV.L @R8+,R10 at 0x8c13c5be, in the
 * delay slot of the branch on the test, loads through R8 even at 0 and steps
 * it. so only point 0 of a call from index 0 takes white. each other point i
 * takes the word at address 4i */
/* @asm operand: small-constant, MOV.L at 0x8c13c5c0 loads -1 from the pool */
#pragma inline_asm(njDCalcPoint3DT2)
Sint32 njDCalcPoint3DT2(NJS_POINT3COL *p, Int index, Int count, void *buf,
                        Float sx, Float sy, Float cx, Float cy)
{
        .IMPORT     __nj_near_clip_
        .IMPORT     __nj_far_clip_
        MOV.L       R10,@-R15
        MOV.L       R9,@-R15
        MOV.L       R8,@-R15
        MOV.L       @R4+,R2
        MOV.L       @R4+,R8
        MOV.L       @R4+,R9
        ADD         #24,R7
        MOV         R5,R0
        MOV         R5,R4
        SHLL2       R0
        SHLL2       R4
        SHLL        R0
        ADD         R4,R0
        ADD         R4,R8
        ADD         R4,R9
        ADD         R0,R2
        MOVA        ?t2_uv_scale,R0
        MOV         R2,R4
        MOV.L       ?t2_near,R2
        MOV.L       ?t2_far,R3
        FMOV.S      @R0,FR8
        MOV         #0,R0
?t2_loop:
        MOV         #0,R1
        FMOV.S      @R4+,FR0
        CMP/EQ      R1,R8
        FLDI1       FR3
        FMOV.S      @R4+,FR1
        FMOV.S      @R4+,FR2
        FTRV        XMTRX,FV0
        FMOV.S      @R3,FR11
        BF/S        ?t2_color
        MOV.L       @R8+,R10
        MOV.L       ?t2_white,R10
?t2_color:
        FCMP/GT     FR11,FR2
        FDIV        FR2,FR3
        MOV.W       @R9+,R1
        FMOV.S      @R2,FR11
        BT          ?t2_far_out
        FMUL        FR4,FR0
        FCMP/GT     FR11,FR2
        FMUL        FR5,FR1
        LDS         R1,FPUL
        FMUL        FR3,FR0
        FLOAT       FPUL,FR9
        MOV.W       @R9+,R1
        FMUL        FR3,FR1
        LDS         R1,FPUL
        FADD        FR6,FR0
        FLOAT       FPUL,FR2
        FADD        FR7,FR1
        FMOV.S      FR0,@-R7
        FMUL        FR8,FR9
        FMOV.S      FR1,@-R7
        FMOV.S      FR3,@-R7
        FMUL        FR8,FR2
        FMOV.S      FR9,@-R7
        FMOV.S      FR2,@-R7
        MOV.L       R10,@-R7
        ADD         #48,R7
        BT          ?t2_next
        MOV         #1,R0
?t2_next:
        DT          R6
        BF          ?t2_loop
        MOV.L       @R15+,R8
        MOV.L       @R15+,R9
        RTS
        MOV.L       @R15+,R10
        .RES.W      1
?t2_white:
        .DATA.L     H'FFFFFFFF
?t2_far_out:
        MOV         #-1,R0
        MOV.L       @R15+,R8
        MOV.L       @R15+,R9
        RTS
        MOV.L       @R15+,R10
        .ALIGN      4
        .DATA.L     H'FFFFFFFF
?t2_near:
        .DATA.L     __nj_near_clip_
?t2_far:
        .DATA.L     __nj_far_clip_
?t2_uv_scale:
        .DATA.L     H'3B800000
}


/* @bug memory: reads each color from low memory where p->col is null and
 * index is above 0. CMP/EQ R1,R8 at 0x8c13c652 gives a point white only where
 * R8 is 0, but ADD R4,R8 at 0x8c13c640 has added 4 * index to it. FMOV.S
 * @R8+,FR9 at 0x8c13c664 then loads each point i's color from address 4i */
/* @asm epilogue: dead-return, SHC's RTS at 0x8c13c6a4 follows the body's own
 * returns at 0x8c13c68e and 0x8c13c694 and its pool */
#pragma inline_asm(njDCalcPoint3DP2)
Sint32 njDCalcPoint3DP2(NJS_POINT3COL *p, Int index, Int count, void *buf,
                        Float sx, Float sy, Float cx, Float cy)
{
        MOV.L       R8,@-R15
        MOV.L       @R4+,R2
        MOV.L       @R4+,R8
        ADD         #16,R7
        MOV         R5,R0
        MOV         R5,R4
        SHLL2       R0
        SHLL2       R4
        SHLL        R0
        ADD         R4,R0
        ADD         R4,R8
        ADD         R0,R2
        MOV         #0,R1
        MOV         R2,R4
        MOVA        ?p2_white,R0
        FMOV.S      @R0,FR10
        MOV.L       ?p2_near,R2
        MOV         #0,R0
        MOV.L       ?p2_far,R3
?p2_loop:
        CMP/EQ      R1,R8
        FMOV.S      @R4+,FR0
        FLDI1       FR3
        FMOV.S      @R4+,FR1
        FMOV.S      @R4+,FR2
        FTRV        XMTRX,FV0
        FMOV.S      @R3,FR11
        BT/S        ?p2_color
        FMOV.S      FR10,FR9
        FMOV.S      @R8+,FR9
?p2_color:
        FCMP/GT     FR11,FR2
        FDIV        FR2,FR3
        FMOV.S      @R2,FR8
        BT          ?p2_far_out
        FMUL        FR4,FR0
        FCMP/GT     FR8,FR2
        FMUL        FR5,FR1
        FMUL        FR3,FR0
        FMUL        FR3,FR1
        FADD        FR6,FR0
        FADD        FR7,FR1
        FMOV.S      FR0,@-R7
        FMOV.S      FR1,@-R7
        FMOV.S      FR3,@-R7
        FMOV.S      FR9,@-R7
        ADD         #32,R7
        BT          ?p2_next
        MOV         #1,R0
?p2_next:
        DT          R6
        BF          ?p2_loop
        RTS
        MOV.L       @R15+,R8
?p2_far_out:
        MOV         #-1,R0
        RTS
        MOV.L       @R15+,R8
?p2_white:
        .DATA.L     H'FFFFFFFF
?p2_near:
        .DATA.L     __nj_near_clip_
?p2_far:
        .DATA.L     __nj_far_clip_
}
#endif
/* clang-format on */
