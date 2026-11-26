#include <ninja.h>

#include "stg26/guard_patrol.h"

#include "stg26/guard_textures/text.inc"

static NJS_TEXNAME textures[1] = {
#include "stg26/guard_textures/textures.inc"
};
NJS_TEXLIST guard_texlist = {textures, sizeof(textures) / sizeof(textures[0])};
