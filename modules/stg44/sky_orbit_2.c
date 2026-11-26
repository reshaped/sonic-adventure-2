/* the sky's first orbit: its object, and background_2_texlist, whose two names
 * are sky_pack.c's */
#include <ninja.h>

#include "shared/space_sky_textures.h"
#include "stg44/sky.h"
#include "stg44/sky_textures.h"

static NJS_TEXNAME textures[2] = {
#include "stg44/sky_orbit_2/textures.inc"
};
NJS_TEXLIST background_2_texlist = {textures,
                                    sizeof(textures) / sizeof(textures[0])};

#include "shared/celestial/orbit_128.h"

NJS_CNK_OBJECT background_2_object[1] = {
#include "stg44/sky_orbit_2/background_2_object.inc"
};
