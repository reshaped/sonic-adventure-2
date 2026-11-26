/* 4 zero bytes of data between polygon.c's and shape_models.c's data.
 * nothing in any image points into them, so their object is unknown. Uint32
 * is only the widest element they allow */
#include <sg_xpt.h>

static Uint32 dat_8c0d1c48[1] = {
#include "collision/dat_8c0d1c48/dat_8c0d1c48.inc"
};
