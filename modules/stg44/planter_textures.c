#include <ninja.h>
#include "stg44/planter.h"

#include "stg44/planter_textures/text.inc"

static NJS_TEXNAME textures[2] = {
#include "stg44/planter_textures/textures.inc"
};
NJS_TEXLIST planter_texlist = {textures,
                               sizeof(textures) / sizeof(textures[0])};
