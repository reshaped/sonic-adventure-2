/* 4 bytes of data between rail_intake.c's and rail.c's data. nothing in this
 * image or 1ST_READ.BIN points into them, so their object is unknown. Uint32
 * is only the widest element they allow */
#include <sg_xpt.h>

static Uint32 dat_8c5cda74[1] = {
#include "stg22/dat_8c5cda74/dat_8c5cda74.inc"
};
