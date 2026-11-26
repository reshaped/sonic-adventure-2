/* 4 zero bytes of constants that 51 images place before effect_object.c's
 * constants. nothing in those images or 1ST_READ.BIN points into them, so
 * their object is unknown. Uint32 is only the widest element they allow */
#include <sg_xpt.h>

static const Uint32 dat_effect_object_0[1] = {
#include "shared/dat_effect_object_0/dat_effect_object_0.inc"
};
