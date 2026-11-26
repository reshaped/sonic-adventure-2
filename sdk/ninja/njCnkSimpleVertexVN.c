#include <nj_cnk.h>
#include <sg_xpt.h>

/* the member exports this function, though no header of the SDK declares
 * it */
/* NOLINTBEGIN(misc-use-internal-linkage) */
/* njCnkSimpleVertexVN takes NJD_CV_VN chunks, whose vertex is a point and a
 * normal */
Sint32 njCnkSimpleVertexVN(struct nj_cnk_simple_work *work);
/* NOLINTEND(misc-use-internal-linkage) */

/* clang-format off */
#ifdef DREAMCAST
/* the body hard-codes the offsets of all of *work's fields but ambient, which
 * it skips */
/* @asm opcode: cache-line, OCBI @R7 at 0x8c1662f0 drops the line of an entry
 * the store queue writes */
#pragma inline_asm(njCnkSimpleVertexVN)
Sint32 njCnkSimpleVertexVN(struct nj_cnk_simple_work *work)
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
        MOV.L       @R4+,R2
        MOV         R3,R5
        FMUL        FR0,FR14
        ADD         #24,R5
        FMOV.S      @R4+,FR8
        FMOV.S      @R4+,FR9
        FMOV.S      @R4+,FR10
        FMOV.S      @R4+,FR11
        PREF        @R5
        MOV         #0,R6
        MOV         #0,R0
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
        ADD         #24,R5
        PREF        @R5
        FMOV.S      FR6,@R2
        ADD         #32,R2
        FMOV.S      FR1,@-R2
        FIPR        FV12,FV0
        FMOV.S      FR0,@-R2
        FLDI1       FR0
        FDIV        FR6,FR0
        FMOV.S      FR10,FR1
        ADD         #-4,R2
        FMOV.S      FR11,FR2
        FLDI1       FR7
        FMUL        FR9,FR5
        FCMP/GT     FR6,FR7
        FMOV.S      FR3,FR7
        FMUL        FR8,FR4
        FMOV.S      FR3,@-R2
        FMUL        FR7,FR7
        ADDC        R6,R0
        FMUL        FR7,FR7
        FMAC        FR0,FR5,FR2
        FMOV.S      FR0,@-R2
        FMUL        FR7,FR7
        FMOV.S      FR2,@-R2
        FMAC        FR0,FR4,FR1
        DT          R1
        FMOV.S      FR1,@-R2
        FMUL        FR7,FR7
        OCBI        @R7
        FMUL        FR3,FR7
        ADD         #16,R2
        FMOV.S      FR7,@R2
        ADD         #32,R7
        PREF        @R2
        BF/S        ?vertex
        ADD         #12,R2
        FMOV.S      @R15+,FR12
        FMOV.S      @R15+,FR13
        FMOV.S      @R15+,FR14
        RTS
        FMOV.S      @R15+,FR15
}
#endif
/* clang-format on */
