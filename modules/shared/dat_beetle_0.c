/* 4 zero bytes of data that 36 images place between beetle.c's and
 * beetle_status.c's data. nothing in those images or 1ST_READ.BIN points
 * into them, so their object is unknown. Uint32 is only the widest element
 * they allow */
#include <sg_xpt.h>

static Uint32 dat_beetle_0[1] = {
#include "shared/dat_beetle_0/dat_beetle_0.inc"
};
