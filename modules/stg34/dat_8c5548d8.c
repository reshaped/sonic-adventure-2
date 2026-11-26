/* 4 zero bytes of data between background_archives.c's and core_sonic.c's
 * data. nothing in this image or 1ST_READ.BIN points into them, so their
 * object is unknown. Uint32 is only the widest element they allow */
#include <sg_xpt.h>

static Uint32 dat_8c5548d8[1] = {
#include "stg34/dat_8c5548d8/dat_8c5548d8.inc"
};
