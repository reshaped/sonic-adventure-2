/* 4 zero bytes of data that 47 images place before collision_object.c's
 * data. nothing in those images, 1ST_READ.BIN or CHAO.PRS points into them,
 * so their object is unknown. Uint32 is only the widest element they allow */
#include <sg_xpt.h>

static Uint32 dat_collision_object_1[1] = {
#include "shared/dat_collision_object_1/dat_collision_object_1.inc"
};
