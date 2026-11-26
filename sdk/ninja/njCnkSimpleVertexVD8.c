#include <nj_cnk.h>
#include <sg_xpt.h>

/* the member exports this function, though no header of the SDK declares
 * it */
/* NOLINTBEGIN(misc-use-internal-linkage) */
/* njCnkSimpleVertexVD8 takes NJD_CV_D8 chunks, whose vertex is a point and a
 * packed ARGB word. each entry's inten holds the word's bits unchanged */
Sint32 njCnkSimpleVertexVD8(struct nj_cnk_simple_work *work);
/* NOLINTEND(misc-use-internal-linkage) */

/* clang-format off */
#ifdef DREAMCAST
/* the body hard-codes the offsets of *work's fields from nbvertex, 20 bytes in,
 * to cy */
/* @asm opcode: cache-line, OCBI @R7 at 0x8c165b2e drops the line of an entry
 * the store queue writes */
#pragma inline_asm(njCnkSimpleVertexVD8)
Sint32 njCnkSimpleVertexVD8(struct nj_cnk_simple_work *work)
{
        ADD         #20,R4
        MOV.L       @R4+,R1
        MOV.L       @R4+,R3
        MOV.L       @R4+,R7
        PREF        @R3
        MOV.L       @R4+,R2
        MOV         R3,R5
        ADD         #16,R5
        FMOV.S      @R4+,FR0
        FMOV.S      @R4+,FR1
        FMOV.S      @R4+,FR2
        FMOV.S      @R4+,FR3
        FLDI1       FR10
        FLDI0       FR9
        PREF        @R5
        MOV         #0,R6
?vertex:
        FMOV.S      @R3+,FR4
        FMOV.S      @R3+,FR5
        FMOV.S      @R3+,FR6
        FLDI1       FR7
        FTRV        XMTRX,FV4
        MOV.L       @R3+,R4
        ADD         #32,R2
        FLDI1       FR8
        ADD         #16,R5
        PREF        @R5
        FDIV        FR6,FR8
        FMOV.S      FR5,@-R2
        FMUL        FR1,FR5
        FMOV.S      FR4,@-R2
        FMUL        FR0,FR4
        FMOV.S      FR9,@-R2
        FCMP/GT     FR10,FR6
        MOV.L       R4,@-R2
        BT          ?far
        ADD         #1,R6
?far:
        FMUL        FR8,FR5
        FMOV.S      FR8,@-R2
        FMUL        FR8,FR4
        OCBI        @R7
        FADD        FR3,FR5
        FMOV.S      FR5,@-R2
        FADD        FR2,FR4
        FMOV.S      FR4,@-R2
        ADD         #32,R7
        DT          R1
        FMOV.S      FR6,@-R2
        PREF        @R2
        BF/S        ?vertex
        ADD         #32,R2
        RTS
        MOV         R6,R0
}
#endif
/* clang-format on */
