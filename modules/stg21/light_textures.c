#include <ninja.h>

#include "stg21/swaying_light.h"
#include "stg21/light_textures/text.inc"

static NJS_TEXNAME textures[1] = {
#include "stg21/light_textures/textures.inc"
};
NJS_TEXLIST swaying_light_texlist = {
#include "stg21/light_textures/swaying_light_texlist.inc"
};
