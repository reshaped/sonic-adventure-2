/* the console's FIPR, FTRV and FSCA as a float's bits, which console.c
 * gives tests/sh4.py: a host's float would quiet a signalling NaN, and
 * FIPR's NaN, H'7FBFFFFF, is one. the fixtures select round toward zero
 * and treat denormals as zero */
#ifndef CONSOLE_H
#define CONSOLE_H

#include <stdint.h>

uint32_t console_fipr(const uint32_t fvm[4], const uint32_t fvn[4]);
void console_ftrv(const uint32_t xmtrx[16], uint32_t vector[4]);
/* pair[0] takes the sine, and pair[1] the cosine */
void console_fsca(uint32_t angle, uint32_t pair[2]);

/* set the host's rounding toward zero, for a model's * and fmaf, and
 * return the rounding it replaced, for console_rounding to restore */
int console_toward_zero(void);
void console_rounding(int mode);

#endif
