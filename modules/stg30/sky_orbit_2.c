#include <ninja.h>

#include "shared/space_sky_textures.h"
#include "stg30/sky.h"

static NJS_TEXNAME textures[2] = {
#include "stg30/sky_orbit_2/textures.inc"
};
NJS_TEXLIST background_2_texlist = {textures,
                                    sizeof(textures) / sizeof(textures[0])};
