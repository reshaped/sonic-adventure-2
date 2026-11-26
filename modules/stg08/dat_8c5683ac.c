/* 12 zero bytes of data between laser.c's and tank_art.c's data. nothing in
 * this image or 1ST_READ.BIN points into them, so their object is unknown.
 * Uint32 is only the widest element they allow */
#include <sg_xpt.h>

static Uint32 dat_8c5683ac[3] = {
#include "stg08/dat_8c5683ac/dat_8c5683ac.inc"
};
