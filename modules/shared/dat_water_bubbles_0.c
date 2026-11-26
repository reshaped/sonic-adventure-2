/* 16 zero bytes of data that 3 images place between water_bubbles.c's and
 * knudai_foothold.c's data. nothing in those images or 1ST_READ.BIN points
 * into them, so their object is unknown. Uint32 is only the widest element
 * they allow */
#include <sg_xpt.h>

static Uint32 dat_water_bubbles_0[4] = {
#include "shared/dat_water_bubbles_0/dat_water_bubbles_0.inc"
};
