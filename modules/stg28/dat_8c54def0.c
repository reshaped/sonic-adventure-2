/* 4 zero bytes of data between cave_scenery.c's and kill_planes.c's data.
 * nothing in this image or 1ST_READ.BIN points into them, so their object is
 * unknown. Uint32 is only the widest element they allow */
#include <sg_xpt.h>

#pragma section ATA
static Uint32 dat_8c54def0[1] = {
#include "stg28/dat_8c54def0/dat_8c54def0.inc"
};
#pragma section
