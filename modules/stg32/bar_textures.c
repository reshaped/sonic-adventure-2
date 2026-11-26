#include <ninja.h>
#include "stg32/bar.h"
#include "stg32/bar_textures/text.inc"

static NJS_TEXNAME textures[5] = {
#include "stg32/bar_textures/textures.inc"
};

NJS_TEXLIST meteo_bar_texlist = {
#include "stg32/bar_textures/meteo_bar_texlist.inc"
};
