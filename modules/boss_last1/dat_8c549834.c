/* 12 bytes of constants between arena.c's and energy.c's constants. nothing
 * in this image or 1ST_READ.BIN points into them, so their object is
 * unknown. Uint32 is only the widest element they allow */
#include <sg_xpt.h>

static const Uint32 dat_8c549834[3] = {
#include "boss_last1/dat_8c549834/dat_8c549834.inc"
};
