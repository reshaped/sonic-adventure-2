#include <nj_cnk.h>
#include <sg_xpt.h>

/* the member exports this function, though no header of the SDK declares
 * it */
/* NOLINTBEGIN(misc-use-internal-linkage) */
/* njCnkSimpleVertexVNI takes NJD_CV_VN_IN chunks, whose vertex is a point, a
 * normal and a word. the word's high halfword is a diffuse intensity. the
 * function ignores its low halfword, a specular intensity. each entry's inten
 * is a packed ARGB word with an alpha of 255. its red, green and blue each take
 * the diffuse intensity's high byte */
Sint32 njCnkSimpleVertexVNI(struct nj_cnk_simple_work *work);
/* NOLINTEND(misc-use-internal-linkage) */

/* clang-format off */
#ifdef DREAMCAST
/* the body hard-codes the offsets of all of *work's fields but ambient, which
 * it skips */
/* @asm opcode: cache-line, OCBI @R7 at 0x8c165db2 drops the line of an entry
 * the store queue writes */
#pragma inline_asm(njCnkSimpleVertexVNI)
Sint32 njCnkSimpleVertexVNI(struct nj_cnk_simple_work *work)
{
        FMOV.S      FR15,@-R15
        FMOV.S      FR14,@-R15
        FMOV.S      FR13,@-R15
        FMOV.S      FR12,@-R15
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
        ADD         #28,R5
        FMOV.S      @R4+,FR0
        FMOV.S      @R4+,FR1
        FMOV.S      @R4+,FR2
        FMOV.S      @R4+,FR3
        PREF        @R5
        MOV         #0,R6
?vertex:
        FMOV.S      @R3+,FR4
        FMOV.S      @R3+,FR5
        FMOV.S      @R3+,FR6
        FLDI1       FR7
        FTRV        XMTRX,FV4
        FMOV.S      @R3+,FR8
        FMOV.S      @R3+,FR9
        FMOV.S      @R3+,FR10
        FLDI0       FR11
        FTRV        XMTRX,FV8
        MOV.L       @R3+,R4
        SHLR16      R4
        MOV.L       #H'00010101,R8
        SHLR8       R4
        MUL.L       R8,R4
        MOV.L       #H'FF000000,R8
        STS         MACL,R4
        OR          R8,R4
        FLDI1       FR7
        ADD         #28,R5
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
        ADD         #-4,R2
        FMUL        FR7,FR4
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
        ADD         #16,R2
        FMUL        FR11,FR11
        ADD         #32,R7
        FMUL        FR10,FR11
        DT          R1
        FMOV.S      FR11,@R2
        PREF        @R2
        BF/S        ?vertex
        ADD         #12,R2
        MOV.L       @R15+,R8
        FMOV.S      @R15+,FR12
        FMOV.S      @R15+,FR13
        FMOV.S      @R15+,FR14
        FMOV.S      @R15+,FR15
        RTS
        MOV         R6,R0
}
#endif
/* clang-format on */
