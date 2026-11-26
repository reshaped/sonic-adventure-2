/* what the kernels' C models take from the SH-4 that C can't spell. these are
 * XMTRX, and FSCA, FIPR and FTRV, which read the FPU's own tables and
 * multiplier. a host build that links a model must define them.
 * tests/kernels/console.c does, from a model of the silicon.
 *
 * A model spells FNEG as C's -, and may spell FMUL, FADD and FMAC as *, + and
 * fmaf. The host tests select round toward zero and use inputs whose arithmetic
 * stays out of the denormal range. Compile with -ffp-contract=off so a separate
 * multiply and add each retain their own rounding. */
#ifndef DC_FPU_H
#define DC_FPU_H

#include <stdint.h>

/* XMTRX: dc_xmtrx[n] is XFn. FTRV adds XF(4j + i) times a vector's float j into
 * the result's float i. so dc_xmtrx[4j] to [4j + 3] hold row j of Ninja's
 * current matrix, as njGetMatrix stores it */
extern float dc_xmtrx[16];

/* FSCA FPUL,DRn: the sine and the cosine of angle's low 16 bits, 65536 to a
 * turn */
typedef struct dc_sincos {
    float sin;
    float cos;
} dc_sincos_t;
dc_sincos_t dc_fsca(uint32_t angle);

/* FIPR FVm,FVn: the sum of fvm[i] * fvn[i]. the multiplier recodes fvn's
 * floats, so swapping fvm and fvn can change the result's last bits */
float dc_fipr(const float fvm[4], const float fvn[4]);

/* FTRV XMTRX,FVn: vector[i] becomes the sum over j of xmtrx[4 * j + i] *
 * vector[j]. each float of the result is a FIPR of vector with a column of
 * xmtrx. the multiplier recodes the column's floats */
void dc_ftrv(const float xmtrx[16], float vector[4]);

#endif
