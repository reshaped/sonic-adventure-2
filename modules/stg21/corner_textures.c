#include <ninja.h>

#include "stg21/corner.h"

#include "stg21/corner_textures/text.inc"

static NJS_TEXNAME textures[1] = {
#include "stg21/corner_textures/textures.inc"
};
NJS_TEXLIST corner_texlist = {textures, sizeof(textures) / sizeof(textures[0])};
