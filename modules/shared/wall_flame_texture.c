#include <ninja.h>

#include "shared/wall_fire.h"

#include "shared/wall_flame_texture/text.inc"

static NJS_TEXNAME texture = {
#include "shared/wall_flame_texture/texture.inc"
};
NJS_TEXLIST wall_fire_flame_texlist = {&texture, 1};
