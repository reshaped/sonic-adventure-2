#include <ninja.h>
#include <sg_xpt.h>

#include "stg40/sky/text.inc"

#include "stg40/terrain.h"
#include "shared/common_object.h"
#include "shared/space_sky_textures.h"
#include "stg40/sky.h"
#include "system/texture.h"

Sint32 background_built = FALSE;
struct model_build background_builds[4] = {
#include "stg40/sky/background_builds.inc"
};
static NJS_TEXLIST *sky_textures[5] = {
#include "stg40/sky/sky_textures.inc"
};
NJS_TEXLIST **background_texture_groups[2] = {
#include "stg40/sky/background_texture_groups.inc"
};
struct texture_pvm background_pvms[3] = {
#include "stg40/sky/background_pvms.inc"
};
