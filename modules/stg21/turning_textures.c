/* The one-player platform adds two pillar textures at the start. */
#include <ninja.h>

#include "shared/turning_platform.h"

#include "stg21/turning_textures/text.inc"

static NJS_TEXNAME textures[9] = {
#include "stg21/turning_textures/textures.inc"
};
NJS_TEXLIST turning_platform_texlist = {textures,
                                        sizeof(textures) / sizeof(textures[0])};
