/* 24 bytes of data between settings.c's and holder.c's data. nothing in this
 * image or 1ST_READ.BIN points into them, so their object is unknown. Uint32
 * is only the widest element they allow */
#include <sg_xpt.h>

static Uint32 dat_8c585ef0[6] = {
#include "boss_bigbogy/dat_8c585ef0/dat_8c585ef0.inc"
};
