/* the sky's second orbit: its object, and background_3_texlist, whose one name
 * is sky_pack.c's */
#include <ninja.h>

#include "shared/space_sky_textures.h"
#include "stg44/sky.h"
#include "stg44/sky_textures.h"

static NJS_TEXNAME textures[1] = {
#include "stg44/sky_orbit/textures.inc"
};
NJS_TEXLIST background_3_texlist = {textures,
                                    sizeof(textures) / sizeof(textures[0])};

#include "shared/celestial/orbit_64_chase.h"

NJS_CNK_OBJECT background_3_object[1] = {
#include "stg44/sky_orbit/background_3_object.inc"
};
