/* 4 bytes of data between foothold_art.c's and footholds.c's data. nothing
 * in this image or 1ST_READ.BIN points into them, so their object is
 * unknown. Uint32 is only the widest element they allow */
#include <sg_xpt.h>

static Uint32 dat_8c60edf8[1] = {
#include "boss_golem_e/dat_8c60edf8/dat_8c60edf8.inc"
};
