/* fpu.h's FIPR, FTRV and FSCA for the kernels' C models, and console.h's
 * for tests/sh4.py. the fixtures use sh4_fpu_approx.h with round toward zero
 * and denormals treated as zero (FPSCR.RM = 1, DN = 1) */
#include "console.h"

#include <fenv.h>
#include <stdint.h>
#include <string.h>

#include "fpu.h"
#include "sh4_fpu_approx.h"

/* FPSCR.RM's round toward zero, as sh4_fpu_approx.h takes it */
#define ROUND_TO_ZERO 1

float dc_xmtrx[16];

dc_sincos_t dc_fsca(uint32_t angle)
{
    dc_sincos_t pair;

    sh4_fsca_f(angle, ROUND_TO_ZERO, &pair.sin, &pair.cos);
    return pair;
}

float dc_fipr(const float fvm[4], const float fvn[4])
{
    return sh4_fipr_f(fvm, fvn, ROUND_TO_ZERO);
}

void dc_ftrv(const float xmtrx[16], float vector[4])
{
    uint32_t matrix[16];
    uint32_t words[4];

    memcpy(matrix, xmtrx, sizeof matrix);
    memcpy(words, vector, sizeof words);
    sh4_ftrv(matrix, words, ROUND_TO_ZERO);
    memcpy(vector, words, sizeof words);
}

uint32_t console_fipr(const uint32_t fvm[4], const uint32_t fvn[4])
{
    return sh4_fipr(fvm, fvn, ROUND_TO_ZERO);
}

void console_ftrv(const uint32_t xmtrx[16], uint32_t vector[4])
{
    sh4_ftrv(xmtrx, vector, ROUND_TO_ZERO);
}

void console_fsca(uint32_t angle, uint32_t pair[2])
{
    sh4_fsca(angle, ROUND_TO_ZERO, &pair[0], &pair[1]);
}

int console_toward_zero(void)
{
    int mode = fegetround();

    fesetround(FE_TOWARDZERO);
    return mode;
}

void console_rounding(int mode)
{
    fesetround(mode);
}
