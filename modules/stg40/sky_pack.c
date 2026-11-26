#include <ninja.h>

#include "shared/space_sky_textures.h"
#include "stg40/sky.h"

/* the texture list of bgtex40, the background's PVM. the sky's own lists share
 * the names this file exports. a sized array pads its name, so the name after
 * it starts on a 4-byte boundary */
const char sky_earth_texture[12] = {
#include "stg40/sky_pack/sky_earth_texture.inc"
};
static const char sky_32_4[12] = {
#include "stg40/sky_pack/sky_32_4.inc"
};
const char sky_stars_20_texture[12] = {
#include "stg40/sky_pack/sky_stars_20_texture.inc"
};
static const char sky_orbit_128[12] = {
#include "stg40/sky_pack/sky_orbit_128.inc"
};
static const char sky_orbit_64[12] = {
#include "stg40/sky_pack/sky_orbit_64.inc"
};
const char sky_cloud_texture[9] = {
#include "stg40/sky_pack/sky_cloud_texture.inc"
};

static NJS_TEXNAME textures[6] = {
#include "stg40/sky_pack/textures.inc"
};
NJS_TEXLIST final_chase_background_pvm_texlist = {
#include "stg40/sky_pack/final_chase_background_pvm_texlist.inc"
};
