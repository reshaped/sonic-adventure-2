#include <ninja.h>

#include "shared/turning_platform.h"

#include "shared/turning_textures/text.inc"

static NJS_TEXNAME textures[7] = {
#include "shared/turning_textures/textures.inc"
};
NJS_TEXLIST turning_platform_texlist = {textures,
                                        sizeof(textures) / sizeof(textures[0])};
