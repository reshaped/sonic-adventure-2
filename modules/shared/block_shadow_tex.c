#include <ninja.h>

#include "shared/core_block.h"

#include "shared/block_shadow_tex/text.inc"

static NJS_TEXNAME textures[1] = {
#include "shared/block_shadow_tex/textures.inc"
};
NJS_TEXLIST core_block_shadow_texlist = {textures, sizeof(textures) /
                                                       sizeof(textures[0])};
