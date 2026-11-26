/* 12 zero bytes of constants between ground.c's and editor.c's constants.
 * nothing in this image, 1ST_READ.BIN, CHAOSTGDARK.PRS, CHAOSTGENTRANCE.PRS,
 * CHAOSTGHERO.PRS, CHAOSTGKINDER.PRS, CHAOSTGLOBBY.PRS, CHAOSTGNEUT.PRS or
 * CHAOSTGRACE.PRS points into them, so their object is unknown. Uint32 is
 * only the widest element they allow */
#include <sg_xpt.h>

static const Uint32 dat_0c55d1f8[3] = {
#include "chao/dat_0c55d1f8/dat_0c55d1f8.inc"
};
