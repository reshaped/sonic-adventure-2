#include <ninja.h>

#include "stg24/machinery_textures.h"

static const char miu128_cg016[16] = {
#include "stg24/machine_tex/miu128_cg016.inc"
};
static const char miu32_ee001[12] = {
#include "stg24/machine_tex/miu32_ee001.inc"
};
static NJS_TEXNAME textures[2] = {
#include "stg24/machine_tex/textures.inc"
};

NJS_TEXLIST engine_machine_texlist = {textures,
                                      sizeof(textures) / sizeof(textures[0])};
