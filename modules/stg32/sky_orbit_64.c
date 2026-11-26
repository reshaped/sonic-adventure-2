/* the object of the sky's second orbit, which draws with background_3_texlist's
 * one texture */
#include <ninja.h>

#include "stg32/sky.h"

#include "shared/celestial/orbit_64.h"

NJS_CNK_OBJECT meteor_sky_second_orbit[1] = {
#include "stg32/sky_orbit_64/meteor_sky_second_orbit.inc"
};
