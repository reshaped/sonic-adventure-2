#include <ninja.h>

#include "shared/core_solid.h"

#include "shared/core_solid_textures/text.inc"

static NJS_TEXNAME textures[7] = {
#include "shared/core_solid_textures/textures.inc"
};
NJS_TEXLIST core_solid_texlist = {textures,
                                  sizeof(textures) / sizeof(textures[0])};
