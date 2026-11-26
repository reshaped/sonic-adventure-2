#ifndef TESTS_KERNELS_DRAW_FIXTURE_H
#define TESTS_KERNELS_DRAW_FIXTURE_H

/* The host sets the FTRV model, and the SH-4 fixture loads XF0 through XF15. */
void load_test_xmtrx(const float *matrix);

#endif
