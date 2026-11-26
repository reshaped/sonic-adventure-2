#include <ninja.h>

#include "shared/space_sky_textures.h"
#include "stg30/sky.h"

const char sky_earth_texture[12] = {
#include "stg30/sky_pack/sky_earth_texture.inc"
};
const char rush_orbit_128_texture[12] = {
#include "stg30/sky_pack/rush_orbit_128_texture.inc"
};
const char sky_orbit_texture[12] = {
#include "stg30/sky_pack/sky_orbit_texture.inc"
};
const char sky_cloud_texture[9] = {
#include "stg30/sky_pack/sky_cloud_texture.inc"
};

static NJS_TEXNAME textures[4] = {
#include "stg30/sky_pack/textures.inc"
};
NJS_TEXLIST rush_sky_pvm_texlist = {textures,
                                    sizeof(textures) / sizeof(textures[0])};
