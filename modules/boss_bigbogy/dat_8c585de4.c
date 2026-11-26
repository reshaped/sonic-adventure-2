/* 4 zero bytes of data between title.c's and fight.c's data. nothing in this
 * image or 1ST_READ.BIN points into them, so their object is unknown. Uint32
 * is only the widest element they allow */
#include <sg_xpt.h>

static Uint32 dat_8c585de4[1] = {
#include "boss_bigbogy/dat_8c585de4/dat_8c585de4.inc"
};
