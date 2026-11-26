/* 4 zero bytes of data between garden_state.c's and player_state.c's data.
 * nothing in this image, 1ST_READ.BIN, CHAOSTGDARK.PRS, CHAOSTGENTRANCE.PRS,
 * CHAOSTGHERO.PRS, CHAOSTGKINDER.PRS, CHAOSTGLOBBY.PRS, CHAOSTGNEUT.PRS or
 * CHAOSTGRACE.PRS points into them, so their object is unknown. Uint32 is
 * only the widest element they allow */
#include <sg_xpt.h>

static Uint32 dat_0c568f7c[1] = {
#include "chao/dat_0c568f7c/dat_0c568f7c.inc"
};
