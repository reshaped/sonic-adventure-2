/* 4 bytes of data between form_field.c's and form_button.c's data. nothing
 * in this image, 1ST_READ.BIN or CHAO.PRS points into them, so their object
 * is unknown. Uint32 is only the widest element they allow */
#include <sg_xpt.h>

static Uint32 dat_0cacc7a0[1] = {
#include "chaostgkinder/dat_0cacc7a0/dat_0cacc7a0.inc"
};
