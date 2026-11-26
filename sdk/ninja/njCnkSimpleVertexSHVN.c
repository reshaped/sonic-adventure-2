#include <nj_cnk.h>
#include <sg_xpt.h>

/* the member exports this function, though no header of the SDK declares
 * it */
/* NOLINTBEGIN(misc-use-internal-linkage) */
/* njCnkSimpleVertexSHVN takes NJD_CV_VN_SH chunks, whose vertex is a point and
 * a normal of four floats each. the current matrix takes the fourth float of
 * each as its w */
Sint32 njCnkSimpleVertexSHVN(struct nj_cnk_simple_work *work);
/* NOLINTEND(misc-use-internal-linkage) */

/* clang-format off */
#ifdef DREAMCAST
/* the body hard-codes the offsets of all of *work's fields but ambient, which
 * it skips */
/* @asm opcode: cache-line, OCBI @R7 at 0x8c1659e4 drops the line of an entry
 * the store queue writes */
#pragma inline_asm(njCnkSimpleVertexSHVN)
Sint32 njCnkSimpleVertexSHVN(struct nj_cnk_simple_work *work)
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
        ADD         #32,R5
        FMOV.S      @R4+,FR0
        FMOV.S      @R4+,FR1
        FMOV.S      @R4+,FR2
        FMOV.S      @R4+,FR3
        PREF        @R5
        MOV         #0,R6
?vertex:
        FSCHG
        FMOV        @R3+,DR4
        FMOV        @R3+,DR6
        FTRV        XMTRX,FV4
        FMOV        @R3+,DR8
        FMOV        @R3+,DR10
        FTRV        XMTRX,FV8
        FSCHG
        FLDI1       FR7
        ADD         #32,R5
        FDIV        FR6,FR7
        PREF        @R5
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
        FMOV.S      FR11,@-R2
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
        FMUL        FR10,FR11
        ADD         #16,R2
        DT          R1
        FMOV.S      FR11,@R2
        PREF        @R2
        BF/S        ?vertex
        ADD         #12,R2
        FMOV.S      @R15+,FR12
        FMOV.S      @R15+,FR13
        FMOV.S      @R15+,FR14
        FMOV.S      @R15+,FR15
        RTS
        MOV         R6,R0
}
#endif
/* clang-format on */
