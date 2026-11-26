/* 8 zero bytes of data between layout.c's and race_setup.c's data. nothing
 * in this image or 1ST_READ.BIN points into them, so their object is
 * unknown. Uint32 is only the widest element they allow */
#include <sg_xpt.h>

static Uint32 dat_8c52e398[2] = {
#include "cart/dat_8c52e398/dat_8c52e398.inc"
};
