#include <ninja.h>

#include "stg32/fence.h"
#include "stg32/fence_textures/text.inc"

static NJS_TEXNAME textures[5] = {
#include "stg32/fence_textures/textures.inc"
};

NJS_TEXLIST herd_fence_texlist = {
#include "stg32/fence_textures/herd_fence_texlist.inc"
};
