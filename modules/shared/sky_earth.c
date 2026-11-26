#include <ninja.h>

#include "shared/space_sky_textures.h"

static NJS_TEXNAME textures[2] = {
#include "shared/sky_earth/textures.inc"
};
NJS_TEXLIST background_0_texlist = {textures,
                                    sizeof(textures) / sizeof(textures[0])};
