/* 12 zero bytes of data between model_objects.c's and crash_piece.c's data.
 * nothing in this image or 1ST_READ.BIN points into them, so their object is
 * unknown. Uint32 is only the widest element they allow */
#include <sg_xpt.h>

static Uint32 dat_8c691cb0[3] = {
#include "stg13/dat_8c691cb0/dat_8c691cb0.inc"
};
