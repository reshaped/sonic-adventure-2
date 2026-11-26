#include <ninja.h>

#include "stg24/hatch_textures.h"

const char hatch_miu128_cg001[16] = {
#include "stg24/floor_hatch_tex/hatch_miu128_cg001.inc"
};
const char hatch_miu128_cg005[16] = {
#include "stg24/floor_hatch_tex/hatch_miu128_cg005.inc"
};
const char hatch_miu128_cg007[16] = {
#include "stg24/floor_hatch_tex/hatch_miu128_cg007.inc"
};
const char hatch_miu128_ee001[16] = {
#include "stg24/floor_hatch_tex/hatch_miu128_ee001.inc"
};
const char hatch_miu128_ee003[16] = {
#include "stg24/floor_hatch_tex/hatch_miu128_ee003.inc"
};
const char hatch_miu64_cg008[12] = {
#include "stg24/floor_hatch_tex/hatch_miu64_cg008.inc"
};
const char hatch_miu64_cg010[12] = {
#include "stg24/floor_hatch_tex/hatch_miu64_cg010.inc"
};

static NJS_TEXNAME textures[7] = {
#include "stg24/floor_hatch_tex/textures.inc"
};

NJS_TEXLIST floor_hatch_texlist = {textures,
                                   sizeof(textures) / sizeof(textures[0])};
