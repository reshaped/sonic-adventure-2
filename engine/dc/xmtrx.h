/* a kernel on XMTRX, the bank of floats FTRV reads as a matrix, in SH4ZAM's
 * style. XMTRX holds Ninja's current matrix. a point goes into the view as the
 * row (x, y, z, 1) times XMTRX. a point is three floats.
 *
 * The kernel requires FPSCR.PR and SZ clear, as GCC leaves them, and preserves
 * FPSCR's PR, SZ and FR. Volatile asm orders accesses to XMTRX. The host model
 * uses fpu.h's dc_xmtrx. */
#ifndef DC_XMTRX_H
#define DC_XMTRX_H

#include <stdint.h>

/* Transform count points by XMTRX with w = 1. Each byte stride is a multiple
 * of 4 and at least 12. Overlapping buffers are safe when out starts at or
 * before in and out_stride <= in_stride. Reads stop at the last point.
 * The SH-4 kernel clobbers FR0 to FR7. */
void zam_xmtrx_transform_points(const float *in, uint32_t in_stride, float *out,
                                uint32_t out_stride, uint32_t count);

#endif
