#include <ninja.h>

#include "shared/wall_fire.h"

#include "shared/wall_fire_textures/text.inc"

static NJS_TEXNAME textures[2] = {
#include "shared/wall_fire_textures/textures.inc"
};
NJS_TEXLIST wall_fire_texlist = {textures,
                                 sizeof(textures) / sizeof(textures[0])};
