#include <ninja.h>

#include "shared/machinery_assets.h"

static NJS_TEXNAME textures[3] = {
#include "shared/machinery_straight/textures.inc"
};
NJS_TEXLIST machinery_straight_texlist = {
#include "shared/machinery_straight/machinery_straight_texlist.inc"
};
