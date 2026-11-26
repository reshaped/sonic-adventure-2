#include <nj_cnk.h>
#include <sg_xpt.h>

/* the member exports this function, though no header of the SDK declares
 * it */
/* NOLINTBEGIN(misc-use-internal-linkage) */
/* njCnkSimpleVertexVS5 takes NJD_CV_S5 chunks, whose vertex is a point and a
 * word. the word's high halfword is a diffuse color. from the top, it holds 5
 * bits of red, 6 of green and 5 of blue. the function ignores the word's low
 * halfword, a specular color. each entry's inten is the diffuse color as a
 * packed ARGB word with an alpha of 255. each channel's bits fill the top of
 * its byte, and the bits below them are 0 */
Sint32 njCnkSimpleVertexVS5(struct nj_cnk_simple_work *work);
/* NOLINTEND(misc-use-internal-linkage) */

/* clang-format off */
#ifdef DREAMCAST
/* the body hard-codes the offsets of *work's fields from nbvertex, 20 bytes in,
 * to cy */
/* @asm opcode: cache-line, OCBI @R7 at 0x8c16645a drops the line of an entry
 * the store queue writes */
#pragma inline_asm(njCnkSimpleVertexVS5)
Sint32 njCnkSimpleVertexVS5(struct nj_cnk_simple_work *work)
{
        MOV.L       R8,@-R15
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
?vertex:
        FMOV.S      @R3+,FR4
        FMOV.S      @R3+,FR5
        FMOV.S      @R3+,FR6
        FLDI1       FR7
        FTRV        XMTRX,FV4
        ADD         #32,R2
        FLDI1       FR8
        ADD         #16,R5
        PREF        @R5
        FDIV        FR6,FR8
        MOV.L       @R3+,R4
        SHLR16      R4
        MOV.L       #H'0000FF00,R8
        MOV         R4,R0
        SHLR8       R0
        SHLR2       R0
        SHLR        R0
        AND         #H'1F,R0
        SHLL2       R0
        SHLL        R0
        ADD         R0,R8
        SHLL8       R8
        MOV         R4,R0
        SHLR2       R0
        SHLR2       R0
        SHLR        R0
        AND         #H'3F,R0
        SHLL2       R0
        ADD         R0,R8
        SHLL8       R8
        MOV         R4,R0
        AND         #H'1F,R0
        SHLL2       R0
        SHLL        R0
        ADD         R0,R8
        FLDI0       FR9
        MOV         R8,R4
        FMOV.S      FR5,@-R2
        FMUL        FR1,FR5
        FMOV.S      FR4,@-R2
        FMUL        FR0,FR4
        FMOV.S      FR9,@-R2
        FLDI1       FR10
        FCMP/GT     FR10,FR6
        MOV.L       R4,@-R2
        BT          ?far
        ADD         #1,R6
?far:
        FMOV.S      FR8,@-R2
        FMUL        FR8,FR5
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
        MOV.L       @R15+,R8
        RTS
        MOV         R6,R0
}
#endif
/* clang-format on */
