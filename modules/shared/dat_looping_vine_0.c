/* 4 zero bytes of data that 2 images place between looping_vine.c's and
 * hover_lift.c's data. nothing in those images or 1ST_READ.BIN points into
 * them, so their object is unknown. Uint32 is only the widest element they
 * allow */
#include <sg_xpt.h>

static Uint32 dat_looping_vine_0[1] = {
#include "shared/dat_looping_vine_0/dat_looping_vine_0.inc"
};
