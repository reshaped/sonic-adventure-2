/* 12 bytes of data between crash_piece.c's and stage_sounds.c's data.
 * nothing in this image or 1ST_READ.BIN points into them, so their object is
 * unknown. Uint32 is only the widest element they allow */
#include <sg_xpt.h>

static Uint32 dat_8c691d00[3] = {
#include "stg13/dat_8c691d00/dat_8c691d00.inc"
};
