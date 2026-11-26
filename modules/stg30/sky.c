#include <ninja.h>
#include <sg_xpt.h>

#include "shared/common_object.h"
#include "shared/space_sky_textures.h"
#include "stg30/sky.h"
#include "system/texture.h"

#include "stg30/sky/text.inc"

Bool sky_built = FALSE;
/* sky_build compiles stars_builds' first with fog on, then orbit_builds'
 * three with fog off. sky_built says whether both succeeded */
struct model_build stars_builds[3] = {
    {MODEL_BUILD_TREE, &background_1_texlist, &rush_stars_object, NULL},
};
struct model_build orbit_builds[3] = {
    {MODEL_BUILD_TREE, &background_0_texlist, &rush_earth_object, NULL},
    {MODEL_BUILD_ALONE, &background_2_texlist, rush_orbit_128_object, NULL},
    {MODEL_BUILD_ALONE, &background_3_texlist, rush_orbit_64_object, NULL},
};
static NJS_TEXLIST *sky_textures[4] = {
#include "stg30/sky/sky_textures.inc"
};
NJS_TEXLIST **rush_sky_groups[2] = {
#include "stg30/sky/rush_sky_groups.inc"
};
struct texture_pvm rush_sky_pvms[2] = {
#include "stg30/sky/rush_sky_pvms.inc"
};
