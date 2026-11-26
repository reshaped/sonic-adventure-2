#include <ninja.h>

#include "shared/turning_platform.h"

#include "shared/turning_flame_tex/text.inc"

static NJS_TEXNAME texture = {
#include "shared/turning_flame_tex/texture.inc"
};
NJS_TEXLIST turning_platform_flame_texlist = {&texture, 1};
