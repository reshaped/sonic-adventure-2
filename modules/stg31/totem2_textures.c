#include <ninja.h>

#include "stg31/scenery.h"

static NJS_TEXNAME textures[1] = {
#include "stg31/totem2_textures/textures.inc"
};

NJS_TEXLIST green_hill_short_totem_texlist = {
    textures, sizeof(textures) / sizeof(textures[0])};
