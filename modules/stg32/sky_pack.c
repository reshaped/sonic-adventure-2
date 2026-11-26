#include <ninja.h>

#include "stg32/sky_pack/text.inc"
#include "stg32/sky.h"

static NJS_TEXNAME textures[24] = {
#include "stg32/sky_pack/textures.inc"
};
NJS_TEXLIST meteor_herd_sky_texlist = {
#include "stg32/sky_pack/meteor_herd_sky_texlist.inc"
};
