#include <ninja.h>

#include "stg31/scenery.h"

const char green_hill_totem_texture_name[6] = {
#include "stg31/totem_textures/green_hill_totem_texture_name.inc"
};

static NJS_TEXNAME textures[1] = {
#include "stg31/totem_textures/textures.inc"
};

NJS_TEXLIST green_hill_tall_totem_texlist = {textures, sizeof(textures) /
                                                           sizeof(textures[0])};
