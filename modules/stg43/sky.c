#include <ninja.h>
#include <sg_xpt.h>

#include "stg43/terrain.h"
#include "shared/common_object.h"
#include "shared/space_sky_textures.h"
#include "stg43/sky.h"
#include "system/texture.h"

#include "stg43/sky/text.inc"

Sint32 background_built = FALSE;
struct model_build background_builds[4] = {
    {MODEL_BUILD_TREE, &background_0_texlist, &background_0_object, NULL},
    {MODEL_BUILD_TREE, &background_1_texlist, &background_1_object, NULL},
    {MODEL_BUILD_ALONE, &background_2_texlist, &background_2_object, NULL},
    {MODEL_BUILD_ALONE, &background_3_texlist, &background_3_object, NULL},
};
static NJS_TEXLIST *sky_textures[5] = {
#include "stg43/sky/sky_textures.inc"
};
NJS_TEXLIST **background_texture_groups[2] = {
#include "stg43/sky/background_texture_groups.inc"
};
struct texture_pvm background_pvms[3] = {
#include "stg43/sky/background_pvms.inc"
};
