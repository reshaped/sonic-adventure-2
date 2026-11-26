#include <ninja.h>

#include "stg24/hatch_textures.h"

static const char miu128_ee002[16] = {
#include "stg24/wall_hatch_tex/miu128_ee002.inc"
};

static NJS_TEXNAME textures[8] = {
#include "stg24/wall_hatch_tex/textures.inc"
};

NJS_TEXLIST wall_hatch_texlist = {textures,
                                  sizeof(textures) / sizeof(textures[0])};
