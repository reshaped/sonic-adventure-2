#include <ninja.h>

#include "shared/space_sky_textures.h"
#include "stg30/sky.h"

static NJS_TEXNAME textures[1] = {
#include "stg30/sky_orbit/textures.inc"
};
NJS_TEXLIST background_3_texlist = {textures,
                                    sizeof(textures) / sizeof(textures[0])};
