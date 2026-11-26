#include <ninja.h>

#include "shared/machinery_assets.h"
#include "stg26/fan_texture/text.inc"

static NJS_TEXNAME textures[1] = {
#include "stg26/fan_texture/textures.inc"
};
NJS_TEXLIST fan_texlist = {
#include "stg26/fan_texture/fan_texlist.inc"
};
