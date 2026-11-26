/* the object of the sky's stars, which draw with background_1_texlist */
#include <ninja.h>

#include "stg32/sky.h"

#include "shared/celestial/stars_herd.h"

NJS_CNK_OBJECT meteor_sky_stars[1] = {
#include "stg32/sky_starfield/meteor_sky_stars.inc"
};
