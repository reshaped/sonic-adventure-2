#include <ninja.h>

#include "stg44/fence.h"

#include "stg44/fence_textures/text.inc"

static NJS_TEXNAME textures[7] = {
#include "stg44/fence_textures/textures.inc"
};
NJS_TEXLIST mad_space_fence_texlist = {textures,
                                       sizeof(textures) / sizeof(textures[0])};
