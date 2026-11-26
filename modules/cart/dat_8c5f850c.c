/* 24 zero bytes of data between models.c's and vehicle.c's data. nothing in
 * this image or 1ST_READ.BIN points into them, so their object is unknown.
 * Uint32 is only the widest element they allow */
#include <sg_xpt.h>

static Uint32 dat_8c5f850c[6] = {
#include "cart/dat_8c5f850c/dat_8c5f850c.inc"
};
