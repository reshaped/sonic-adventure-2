#include <ninja.h>

#include "shared/space_sky_textures.h"

#include "shared/sky_stars/text.inc"

static NJS_TEXNAME textures[17] = {
#include "shared/sky_stars/textures.inc"
};
NJS_TEXLIST background_1_texlist = {textures,
                                    sizeof(textures) / sizeof(textures[0])};
