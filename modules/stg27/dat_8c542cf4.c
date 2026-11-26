/* 4 zero bytes of constants between colony_spark.c's and spark_emitter.c's
 * constants. nothing in this image or 1ST_READ.BIN points into them, so
 * their object is unknown. Uint32 is only the widest element they allow */
#include <sg_xpt.h>

static const Uint32 dat_8c542cf4[1] = {
#include "stg27/dat_8c542cf4/dat_8c542cf4.inc"
};
