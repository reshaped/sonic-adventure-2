#include <ninja.h>

#include "stg32/sky/text.inc"

#include "shared/space_sky_textures.h"
#include "stg32/meteor_herd.h"
#include "stg32/sky.h"
#include "system/texture.h"

static NJS_TEXLIST *sky_textures[6] = {
#include "stg32/sky/sky_textures.inc"
};
NJS_TEXLIST **background_texture_groups[2] = {
#include "stg32/sky/background_texture_groups.inc"
};
struct texture_pvm background_pvms[3] = {
#include "stg32/sky/background_pvms.inc"
};
