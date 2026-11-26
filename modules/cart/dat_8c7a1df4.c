/* 4 bytes of data between traffic.c's and ramp_art.c's data. nothing in this
 * image or 1ST_READ.BIN points into them, so their object is unknown. Uint32
 * is only the widest element they allow */
#include <sg_xpt.h>

static Uint32 dat_8c7a1df4[1] = {
#include "cart/dat_8c7a1df4/dat_8c7a1df4.inc"
};
