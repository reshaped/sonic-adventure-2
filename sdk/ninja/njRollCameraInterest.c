/* clang-format off */
#ifdef DREAMCAST
#include <ninja.h>

/* the body reads c's vx to vz at byte offsets 12 to 20. it adds ang to roll,
 * at 24, and turns c's lx and ly, at 40 and 52 */
/* @asm epilogue: dead-return, SHC's RTS at 0x8c15d680 follows the body's own
 * return at 0x8c15d666 and its pool */
#pragma inline_asm(njRollCameraInterest)
void njRollCameraInterest(NJS_CAMERA *c, Angle ang)
{
        .IMPORT     __nj_camera_matrix_
        .IMPORT     _njUnitMatrix
        .IMPORT     _njRotate
        .IMPORT     _njCalcVector
        .IMPORT     _njUnitVector
        MOV.L       R14,@-R15
        MOV.L       R13,@-R15
        MOV         R4,R14
        MOV.L       R12,@-R15
        MOV         R5,R13
        STS.L       PR,@-R15
        MOV.L       #__nj_camera_matrix_,R12
        ADD         #-12,R15
        MOV.L       @(24,R14),R2
        ADD         R5,R2
        MOV.L       #_njUnitMatrix,R3
        MOV         R12,R4
        JSR         @R3
        MOV.L       R2,@(24,R14)
        MOV         R15,R5
        MOV         #20,R0
        ADD         #12,R15
        FMOV.S      @(R0,R14),FR3
        FMOV.S      FR3,@-R15
        MOV         #16,R0
        FMOV.S      @(R0,R14),FR2
        FMOV.S      FR2,@-R15
        MOV         #12,R0
        FMOV.S      @(R0,R14),FR3
        FMOV.S      FR3,@-R15
        MOV.L       #_njRotate,R3
        MOV         R12,R4
        JSR         @R3
        MOV         R13,R6
        MOV         R15,R5
        MOV         R14,R6
        ADD         #40,R6
        FMOV.S      @R6+,FR3
        FMOV.S      FR3,@R5
        MOV         #4,R0
        FMOV.S      @R6+,FR2
        FMOV.S      FR2,@(R0,R5)
        MOV         #8,R0
        FMOV.S      @R6+,FR3
        FMOV.S      FR3,@(R0,R5)
        MOV.L       #_njCalcVector,R3
        MOV         R12,R4
        JSR         @R3
        ADD         #-12,R6
        MOV.L       #_njUnitVector,R3
        MOV         R14,R4
        JSR         @R3
        ADD         #40,R4
        MOV         R15,R5
        MOV         R14,R6
        ADD         #52,R6
        FMOV.S      @R6+,FR3
        FMOV.S      FR3,@R5
        MOV         #4,R0
        FMOV.S      @R6+,FR2
        FMOV.S      FR2,@(R0,R5)
        MOV         #8,R0
        FMOV.S      @R6+,FR3
        FMOV.S      FR3,@(R0,R5)
        MOV.L       #_njCalcVector,R3
        MOV         R12,R4
        JSR         @R3
        ADD         #-12,R6
        MOV.L       #_njUnitVector,R3
        MOV         R14,R4
        JSR         @R3
        ADD         #52,R4
        ADD         #12,R15
        LDS.L       @R15+,PR
        MOV.L       @R15+,R12
        MOV.L       @R15+,R13
        RTS
        MOV.L       @R15+,R14
}
#endif
/* clang-format on */
