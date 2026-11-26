#include <ninja.h>
#include "stg44/landing.h"

#include "stg44/landing_textures/text.inc"

static NJS_TEXNAME textures[9] = {
#include "stg44/landing_textures/textures.inc"
};
NJS_TEXLIST landing_texlist = {textures,
                               sizeof(textures) / sizeof(textures[0])};
