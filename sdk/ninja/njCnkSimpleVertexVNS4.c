#include <nj_cnk.h>
#include <sg_xpt.h>

/* the member exports this function, though no header of the SDK declares
 * it */
/* NOLINTBEGIN(misc-use-internal-linkage) */
/* njCnkSimpleVertexVNS4 takes NJD_CV_VN_S4 chunks, whose vertex is a point, a
 * normal and a word. the word's high halfword is a diffuse color. from the top,
 * it holds 4 bits each of alpha, red, green and blue. the function ignores the
 * word's low halfword, a specular color. each entry's inten is the diffuse
 * color as a packed ARGB word. each channel's byte holds its 4 bits twice, as
 * 0xa becomes 0xaa */
Sint32 njCnkSimpleVertexVNS4(struct nj_cnk_simple_work *work);
/* NOLINTEND(misc-use-internal-linkage) */

/* clang-format off */
#ifdef DREAMCAST
/* the body hard-codes the offsets of all of *work's fields but ambient, which
 * it skips */
/* @asm opcode: cache-line, OCBI @R7 at 0x8c165eb6 drops the line of an entry
 * the store queue writes */
#pragma inline_asm(njCnkSimpleVertexVNS4)
Sint32 njCnkSimpleVertexVNS4(struct nj_cnk_simple_work *work)
{
        FMOV.S      FR15,@-R15
        FMOV.S      FR14,@-R15
        FMOV.S      FR13,@-R15
        FMOV.S      FR12,@-R15
        MOV.L       R8,@-R15
        MOV.L       R9,@-R15
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
        MOV         #0,R8
        MOV         R4,R0
        SHLR8       R0
        SHLR2       R0
        SHLR2       R0
        AND         #H'0F,R0
        MOV         R0,R9
        SHLL2       R0
        SHLL2       R0
        ADD         R9,R0
        ADD         R0,R8
        SHLL8       R8
        MOV         R4,R0
        SHLR8       R0
        AND         #H'0F,R0
        MOV         R0,R9
        SHLL2       R0
        SHLL2       R0
        ADD         R9,R0
        ADD         R0,R8
        SHLL8       R8
        MOV         R4,R0
        SHLR2       R0
        SHLR2       R0
        AND         #H'0F,R0
        MOV         R0,R9
        SHLL2       R0
        SHLL2       R0
        ADD         R9,R0
        ADD         R0,R8
        SHLL8       R8
        MOV         R4,R0
        AND         #H'0F,R0
        MOV         R0,R9
        SHLL2       R0
        SHLL2       R0
        ADD         R9,R0
        ADD         R0,R8
        MOV         R8,R4
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
        MOV.L       @R15+,R9
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
