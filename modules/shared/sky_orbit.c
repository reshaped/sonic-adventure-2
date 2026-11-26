#include <ninja.h>

#include "shared/space_sky_textures.h"
#include "shared/celestial/orbit_name.h"

const char sky_orbit_texture[11] = "spacenbg64";

static NJS_TEXNAME textures[1] = {
#include "shared/sky_orbit/textures.inc"
};
NJS_TEXLIST background_3_texlist = {textures,
                                    sizeof(textures) / sizeof(textures[0])};
