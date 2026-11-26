/* 4 zero bytes of data between meet.c's and archives.c's data. nothing in
 * this image, 1ST_READ.BIN or CHAO.PRS points into them, so their object is
 * unknown. Uint32 is only the widest element they allow */
#include <sg_xpt.h>

static Uint32 dat_0c9f5ff0[1] = {
#include "chaostgrace/dat_0c9f5ff0/dat_0c9f5ff0.inc"
};
