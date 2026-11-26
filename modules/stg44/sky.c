#include <ninja.h>

#include "stg44/sky/text.inc"

#include "shared/space_sky_textures.h"
#include "stg44/mad_space.h"
#include "stg44/sky.h"
#include "system/texture.h"

static NJS_TEXLIST *sky_textures[6] = {
#include "stg44/sky/sky_textures.inc"
};
NJS_TEXLIST **background_texture_groups[2] = {
#include "stg44/sky/background_texture_groups.inc"
};
struct texture_pvm background_pvms[3] = {
#include "stg44/sky/background_pvms.inc"
};
