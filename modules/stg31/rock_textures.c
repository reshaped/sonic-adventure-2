#include <ninja.h>

#include "stg31/scenery.h"

#include "stg31/rock_textures/text.inc"

static NJS_TEXNAME textures[1] = {
#include "stg31/rock_textures/textures.inc"
};

NJS_TEXLIST green_hill_rock_texlist = {textures,
                                       sizeof(textures) / sizeof(textures[0])};
