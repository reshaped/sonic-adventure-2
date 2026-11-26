#include <nj_cnk.h>
#include <sg_xpt.h>

/* the member exports this function, though no header of the SDK declares
 * it */
/* NOLINTBEGIN(misc-use-internal-linkage) */
/* njCnkSimpleVertexVNXD8 takes NJD_CV_VNX_D8 chunks, whose vertex is an
 * NJD_CV_VNX vertex and a packed ARGB word. each entry's inten holds the word's
 * bits unchanged */
Sint32 njCnkSimpleVertexVNXD8(struct nj_cnk_simple_work *work);
/* NOLINTEND(misc-use-internal-linkage) */

/* clang-format off */
#ifdef DREAMCAST
/* the body hard-codes the offsets of all of *work's fields but ambient, which
 * it skips. the float at ?normal_scale is 1 / 512 */
/* @asm opcode: cache-line, OCBI @R7 at 0x8c16615a drops the line of an entry
 * the store queue writes */
#pragma inline_asm(njCnkSimpleVertexVNXD8)
Sint32 njCnkSimpleVertexVNXD8(struct nj_cnk_simple_work *work)
{
        FMOV.S      FR15,@-R15
        FMOV.S      FR14,@-R15
        FMOV.S      FR13,@-R15
        FMOV.S      FR12,@-R15
        MOV.L       R10,@-R15
        MOV.L       R9,@-R15
        MOV.L       R8,@-R15
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
        MOV.L       @R4+,R2
        MOV         R3,R5
        FMUL        FR0,FR14
        ADD         #20,R5
        FMOV.S      @R4+,FR0
        FMOV.S      @R4+,FR1
        FMOV.S      @R4+,FR2
        FMOV.S      @R4+,FR3
        PREF        @R5
        MOV         #0,R6
        MOVA        ?normal_scale,R0
        LDS.L       @R0+,FPUL
?vertex:
        FMOV.S      @R3+,FR4
        FMOV.S      @R3+,FR5
        FMOV.S      @R3+,FR6
        FLDI1       FR7
        FTRV        XMTRX,FV4
        MOV.L       @R3+,R4
        FSTS        FPUL,FR11
        MOV         R4,R8
        SHLL2       R4
        MOV         R8,R9
        SHLL8       R8
        SHLL2       R8
        MOV         #22,R10
        SHLL2       R8
        SHLD        R10,R9
        FLDI1       FR7
        FDIV        FR6,FR7
        NEG         R10,R10
        SHAD        R10,R4
        SHAD        R10,R8
        SHAD        R10,R9
        LDS         R4,FPUL
        FLOAT       FPUL,FR8
        LDS         R8,FPUL
        FLOAT       FPUL,FR9
        LDS         R9,FPUL
        FLOAT       FPUL,FR10
        FMUL        FR11,FR8
        FMUL        FR11,FR9
        FMUL        FR11,FR10
        FLDS        FR11,FPUL
        FLDI0       FR11
        FTRV        XMTRX,FV8
        ADD         #20,R5
        PREF        @R5
        MOV.L       @R3+,R4
        FMOV.S      FR6,@R2
        FIPR        FV12,FV8
        ADD         #32,R2
        FMUL        FR1,FR5
        FMOV.S      FR9,@-R2
        FMUL        FR0,FR4
        FMOV.S      FR8,@-R2
        FMUL        FR7,FR5
        FLDI1       FR10
        FMUL        FR7,FR4
        ADD         #-4,R2
        FCMP/GT     FR10,FR6
        MOV.L       R4,@-R2
        BT          ?far
        ADD         #1,R6
?far:
        FMOV.S      FR11,FR10
        FMOV.S      FR7,@-R2
        FADD        FR3,FR5
        FMUL        FR11,FR11
        FADD        FR2,FR4
        FMOV.S      FR5,@-R2
        FMUL        FR11,FR11
        FMOV.S      FR4,@-R2
        FMUL        FR11,FR11
        OCBI        @R7
        FMUL        FR11,FR11
        ADD         #32,R7
        ADD         #16,R2
        FMUL        FR10,FR11
        DT          R1
        FMOV.S      FR11,@R2
        PREF        @R2
        BF/S        ?vertex
        ADD         #12,R2
        MOV.L       @R15+,R8
        MOV.L       @R15+,R9
        MOV.L       @R15+,R10
        FMOV.S      @R15+,FR12
        FMOV.S      @R15+,FR13
        FMOV.S      @R15+,FR14
        FMOV.S      @R15+,FR15
        RTS
        MOV         R6,R0
        .ALIGN      4
?normal_scale:
        .DATA.L     H'3B000000
}
#endif
/* clang-format on */
