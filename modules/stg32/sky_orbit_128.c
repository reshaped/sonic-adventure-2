/* the object of the sky's first orbit, which draws with background_2_texlist's
 * two textures */
#include <ninja.h>

#include "stg32/sky.h"

#include "shared/celestial/orbit_128.h"

NJS_CNK_OBJECT meteor_sky_first_orbit[1] = {
#include "stg32/sky_orbit_128/meteor_sky_first_orbit.inc"
};
