#include <ninja.h>

#include "shared/core_block.h"

#include "shared/block_portal_tex/text.inc"

static NJS_TEXNAME textures[4] = {
#include "shared/block_portal_tex/textures.inc"
};
NJS_TEXLIST core_block_portal_texlist = {textures, sizeof(textures) /
                                                       sizeof(textures[0])};
