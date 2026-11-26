/* 4 zero bytes of data between patrol_camera.c's and kazobj.c's data.
 * nothing in this image or 1ST_READ.BIN points into them, so their object is
 * unknown. Uint32 is only the widest element they allow */
#include <sg_xpt.h>

static Uint32 dat_8c583338[1] = {
#include "stg27/dat_8c583338/dat_8c583338.inc"
};
