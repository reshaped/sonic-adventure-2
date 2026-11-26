/* 12 bytes of data between golem.c's and golem_camera.c's data. nothing in
 * this image or 1ST_READ.BIN points into them, so their object is unknown.
 * Uint32 is only the widest element they allow */
#include <sg_xpt.h>

static Uint32 dat_8c609a3c[3] = {
#include "boss_golem/dat_8c609a3c/dat_8c609a3c.inc"
};
