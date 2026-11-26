#ifdef DREAMCAST
#include <ninja.h>

#ifdef NONMATCHING
/* -2ab, as the body's two FMULs make it: a times b, then times -2. off the
 * diagonal it is an element of the reflection, and on the diagonal the
 * element less 1 */
ZAM_INLINE Float minus_twice_product(Float a, Float b)
{
    Float product = a * b;

    return product * -2.0F;
}

/* this side keeps the console's bits. each element takes the body's FMULs
 * and FADDs, in the body's order. each product is a statement of its own, so
 * no build fuses it into a sum */
void njMirror(NJS_MATRIX *m, NJS_PLANE *pl)
{
    NJS_MATRIX mirror;
    Float pz_vz = pl->pz * pl->vz;
    Float px_vx = pl->px * pl->vx;
    Float py_vy = pl->py * pl->vy;
    Float distance = (pz_vz + px_vx) + py_vy;
    Float twice = distance * 2.0F;
    Float xx = minus_twice_product(pl->vx, pl->vx);
    Float yy = minus_twice_product(pl->vy, pl->vy);
    Float zz = minus_twice_product(pl->vz, pl->vz);

    mirror[0] = xx + 1.0F;
    mirror[1] = minus_twice_product(pl->vy, pl->vx);
    mirror[2] = minus_twice_product(pl->vx, pl->vz);
    mirror[3] = 0.0F;
    mirror[4] = mirror[1];
    mirror[5] = yy + 1.0F;
    mirror[6] = minus_twice_product(pl->vy, pl->vz);
    mirror[7] = 0.0F;
    mirror[8] = mirror[2];
    mirror[9] = mirror[6];
    mirror[10] = zz + 1.0F;
    mirror[11] = 0.0F;
    mirror[12] = pl->vx * twice;
    mirror[13] = pl->vy * twice;
    mirror[14] = pl->vz * twice;
    mirror[15] = 1.0F;
    njMultiMatrix(m, &mirror);
}
#else
/* clang-format off */
/* the body builds the reflection on its stack, last element first, and hands
 * it to njMultiMatrix with m. n stands for pl's normal (vx, vy, vz), and p for
 * its point (px, py, pz). element (i, j) of the reflection's upper-left 3 by 3
 * block is the unit matrix's less 2 * n[i] * n[j]. the reflection's
 * translation is 2 * (p . n) * n */
/* @asm epilogue: dead-return, SHC's RTS at 0x8c1042a8 follows the body's own
 * return at 0x8c10429e and its pool */
#pragma inline_asm(njMirror)
void njMirror(NJS_MATRIX *m, NJS_PLANE *pl)
{
        .IMPORT     _njMultiMatrix
        STS.L       PR,@-R15
        MOV         R15,R6
        ADD         #-64,R15
        FMOV.S      @R5+,FR0
        FMOV.S      @R5+,FR1
        FMOV.S      @R5+,FR2
        FMOV.S      @R5+,FR3
        FMOV.S      @R5+,FR4
        FMOV.S      @R5+,FR5
        FMUL        FR3,FR0
        MOV         R6,R5
        FLDI1       FR9
        FMUL        FR4,FR1
        FLDI1       FR10
        FMUL        FR5,FR2
        FMOV.S      FR3,FR6
        FADD        FR9,FR10
        FMOV.S      FR4,FR7
        FADD        FR0,FR2
        FMOV.S      FR5,FR8
        FADD        FR1,FR2
        FMUL        FR10,FR2
        FMOV.S      FR9,@-R5
        FMUL        FR2,FR8
        FNEG        FR10
        FMOV.S      FR5,FR11
        FMUL        FR2,FR7
        FMOV.S      FR8,@-R5
        FMUL        FR2,FR6
        FMOV.S      FR7,@-R5
        FMOV.S      FR6,@-R5
        FLDI0       FR7
        FMOV.S      FR7,@-R5
        FMUL        FR11,FR11
        FMUL        FR10,FR11
        FMOV.S      FR4,FR8
        FADD        FR9,FR11
        FMOV.S      FR11,@-R5
        FMUL        FR5,FR8
        FMOV.S      FR3,FR6
        FMUL        FR10,FR8
        FMOV.S      FR8,@-R5
        FMUL        FR5,FR6
        FMOV.S      FR4,FR0
        FMUL        FR10,FR6
        FMOV.S      FR6,@-R5
        FMOV.S      FR7,@-R5
        FMUL        FR0,FR0
        FMOV.S      FR8,@-R5
        FMUL        FR10,FR0
        FADD        FR9,FR0
        FMOV.S      FR0,@-R5
        FMUL        FR3,FR4
        FMUL        FR10,FR4
        FMOV.S      FR4,@-R5
        FMOV.S      FR7,@-R5
        FMUL        FR3,FR3
        FMOV.S      FR6,@-R5
        FMOV.S      FR4,@-R5
        FMUL        FR10,FR3
        MOV.L       #_njMultiMatrix,R3
        FADD        FR9,FR3
        FMOV.S      FR3,@-R5
        JSR         @R3
        NOP
        ADD         #64,R15
        LDS.L       @R15+,PR
        RTS
        NOP
}
/* clang-format on */
#endif
#endif
