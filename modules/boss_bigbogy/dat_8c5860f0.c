/* 4 zero bytes of data between sway.c's and door_art.c's data. nothing in
 * this image or 1ST_READ.BIN points into them, so their object is unknown.
 * Uint32 is only the widest element they allow */
#include <sg_xpt.h>

static Uint32 dat_8c5860f0[1] = {
#include "boss_bigbogy/dat_8c5860f0/dat_8c5860f0.inc"
};
