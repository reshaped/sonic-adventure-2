#include <ninja.h>

#include "stg24/machinery_textures.h"

static const char miu32_ee001[12] = {
#include "stg24/blue_lamp_tex/miu32_ee001.inc"
};
static NJS_TEXNAME textures[1] = {
#include "stg24/blue_lamp_tex/textures.inc"
};
NJS_TEXLIST blue_lamp_texlist = {textures, 1};
