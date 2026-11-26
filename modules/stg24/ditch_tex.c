#include <ninja.h>

#include "stg24/machinery_textures.h"

static const char miu32_cg001[12] = {
#include "stg24/ditch_tex/miu32_cg001.inc"
};
static const char miu64_cg009[12] = {
#include "stg24/ditch_tex/miu64_cg009.inc"
};
static const char miu64_cg020[12] = {
#include "stg24/ditch_tex/miu64_cg020.inc"
};
static NJS_TEXNAME textures[3] = {
#include "stg24/ditch_tex/textures.inc"
};

NJS_TEXLIST engine_ditch_texlist = {textures,
                                    sizeof(textures) / sizeof(textures[0])};
