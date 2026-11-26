/* 8 zero bytes of data between flower_models.c's and tree_data.c's data.
 * nothing in this image, 1ST_READ.BIN, CHAOSTGDARK.PRS, CHAOSTGENTRANCE.PRS,
 * CHAOSTGHERO.PRS, CHAOSTGKINDER.PRS, CHAOSTGLOBBY.PRS, CHAOSTGNEUT.PRS or
 * CHAOSTGRACE.PRS points into them, so their object is unknown. Uint32 is
 * only the widest element they allow */
#include <sg_xpt.h>

static Uint32 dat_0c6191a4[2] = {
#include "chao/dat_0c6191a4/dat_0c6191a4.inc"
};
