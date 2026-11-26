#include <ninja.h>

#include "shared/space_sky_textures.h"
#include "shared/celestial/orbit_name.h"

#include "shared/sky_orbit_2/text.inc"

static NJS_TEXNAME textures[2] = {
#include "shared/sky_orbit_2/textures.inc"
};
NJS_TEXLIST background_2_texlist = {textures,
                                    sizeof(textures) / sizeof(textures[0])};
