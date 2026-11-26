#include <ninja.h>

#include "shared/core_block.h"

#include "shared/block_body_textures/text.inc"

static NJS_TEXNAME textures[5] = {
#include "shared/block_body_textures/textures.inc"
};
NJS_TEXLIST core_block_body_texlist = {textures,
                                       sizeof(textures) / sizeof(textures[0])};
