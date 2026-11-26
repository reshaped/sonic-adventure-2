/* 8 bytes of constants between garden_textures.c's and world_card_art.c's
 * constants. nothing in this image, 1ST_READ.BIN, CHAOSTGDARK.PRS,
 * CHAOSTGENTRANCE.PRS, CHAOSTGHERO.PRS, CHAOSTGKINDER.PRS, CHAOSTGLOBBY.PRS,
 * CHAOSTGNEUT.PRS or CHAOSTGRACE.PRS points into them, so their object is
 * unknown. Uint32 is only the widest element they allow */
#include <sg_xpt.h>

static const Uint32 dat_0c565fe8[2] = {
#include "chao/dat_0c565fe8/dat_0c565fe8.inc"
};
