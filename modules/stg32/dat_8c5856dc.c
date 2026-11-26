/* 4 zero bytes of data between object_kinds.c's and land_draw.c's data.
 * nothing in this image or 1ST_READ.BIN points into them, so their object is
 * unknown. Uint32 is only the widest element they allow */
#include <sg_xpt.h>

static Uint32 dat_8c5856dc[1] = {
#include "stg32/dat_8c5856dc/dat_8c5856dc.inc"
};
