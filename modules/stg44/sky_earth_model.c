/* the object of the sky's Earth, which draws with background_0_texlist's earth
 * and cloud textures */
#include <ninja.h>

#include "stg44/sky.h"

#include "shared/celestial/earth.h"

NJS_CNK_OBJECT background_0_object[1] = {
#include "stg44/sky_earth_model/background_0_object.inc"
};
