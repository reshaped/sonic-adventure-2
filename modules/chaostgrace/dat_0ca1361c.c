/* 40 bytes of data between camera.c's and script.c's data. nothing in this
 * image, 1ST_READ.BIN or CHAO.PRS points into them, so their object is
 * unknown. Uint32 is only the widest element they allow */
#include <sg_xpt.h>

static Uint32 dat_0ca1361c[10] = {
#include "chaostgrace/dat_0ca1361c/dat_0ca1361c.inc"
};
