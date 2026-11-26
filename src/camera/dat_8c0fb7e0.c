/* 4 zero bytes of data between camera.c's and zone_text.c's data. nothing in
 * any image points into them, so their object is unknown. Uint32 is only the
 * widest element they allow */
#include <sg_xpt.h>

static Uint32 dat_8c0fb7e0[1] = {
#include "camera/dat_8c0fb7e0/dat_8c0fb7e0.inc"
};
