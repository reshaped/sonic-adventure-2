/* 28 bytes of data between weak_point.c's and upper_barrier.c's data.
 * nothing in this image or 1ST_READ.BIN points into them, so their object is
 * unknown. Uint32 is only the widest element they allow */
#include <sg_xpt.h>

static Uint32 dat_8c54fcdc[7] = {
#include "boss_last1/dat_8c54fcdc/dat_8c54fcdc.inc"
};
