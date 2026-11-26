#include <ninja.h>

#include "shared/machinery_assets.h"

/* a sized array pads its name, so the name after it starts on a 4-byte
 * boundary */
const char machinery_texture_128a[20] = {
#include "stg26/machinery_textures/machinery_texture_128a.inc"
};
const char machinery_texture_64a[20] = {
#include "stg26/machinery_textures/machinery_texture_64a.inc"
};
const char machinery_texture_64b[18] = {
#include "stg26/machinery_textures/machinery_texture_64b.inc"
};

static NJS_TEXNAME textures[3] = {
#include "stg26/machinery_textures/textures.inc"
};
NJS_TEXLIST machinery_curve_texlist = {
#include "stg26/machinery_textures/machinery_curve_texlist.inc"
};
