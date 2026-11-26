#include <nj_cnk.h>
#include <sg_xpt.h>

/* the member exports this function, though no header of the SDK declares
 * it */
/* NOLINTBEGIN(misc-use-internal-linkage) */
/* njCnkSimpleVertexVI takes NJD_CV_IN chunks, whose vertex is a point and a
 * word. the word's high halfword is a diffuse intensity, and its low halfword a
 * signed specular intensity. each entry's inten is a packed ARGB word with an
 * alpha of 255. its red, green and blue are each the diffuse intensity's high
 * byte. each entry's spec is the specular intensity times the largest float
 * below 1 / 32768 */
Sint32 njCnkSimpleVertexVI(struct nj_cnk_simple_work *work);
/* NOLINTEND(misc-use-internal-linkage) */

/* clang-format off */
#ifdef DREAMCAST
/* the body hard-codes the offsets of *work's fields from nbvertex, 20 bytes in,
 * to cy */
/* @asm opcode: cache-line, OCBI @R7 at 0x8c165bae drops the line of an entry
 * the store queue writes */
#pragma inline_asm(njCnkSimpleVertexVI)
Sint32 njCnkSimpleVertexVI(struct nj_cnk_simple_work *work)
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
        PREF        @R5
        MOV         #0,R6
        MOVA        ?spec_scale,R0
        FMOV.S      @R0,FR9
?vertex:
        FMOV.S      @R3+,FR4
        FMOV.S      @R3+,FR5
        FMOV.S      @R3+,FR6
        FLDI1       FR7
        FTRV        XMTRX,FV4
        MOV.L       @R3,R4
        FLDI1       FR8
        ADD         #16,R5
        PREF        @R5
        FDIV        FR6,FR8
        FMOV.S      FR6,@R2
        ADD         #32,R2
        FMOV.S      FR5,@-R2
        FMUL        FR1,FR5
        FMOV.S      FR4,@-R2
        FMUL        FR0,FR4
        SHLR16      R4
        MOV.L       #H'00010101,R0
        SHLR8       R4
        MUL.L       R0,R4
        MOV.L       #H'FF000000,R0
        STS         MACL,R4
        ADD         #-4,R2
        OR          R0,R4
        MOV.L       R4,@-R2
        FLDI1       FR10
        FCMP/GT     FR10,FR6
        BT          ?far
        ADD         #1,R6
?far:
        FMOV.S      FR8,@-R2
        FMUL        FR8,FR5
        MOV.L       @R3+,R4
        FMUL        FR8,FR4
        OCBI        @R7
        FADD        FR3,FR5
        FMOV.S      FR5,@-R2
        FADD        FR2,FR4
        FMOV.S      FR4,@-R2
        ADD         #32,R7
        EXTS.W      R4,R4
        LDS         R4,FPUL
        FLOAT       FPUL,FR10
        ADD         #16,R2
        FMUL        FR9,FR10
        FMOV.S      FR10,@R2
        DT          R1
        PREF        @R2
        BF/S        ?vertex
        ADD         #12,R2
        RTS
        MOV         R6,R0
        .ALIGN      4
?spec_scale:
        .DATA.L     H'37FFFFFF
}
#endif
/* clang-format on */
