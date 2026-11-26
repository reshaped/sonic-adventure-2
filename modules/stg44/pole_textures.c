#include <ninja.h>

#include "stg44/lights.h"

#include "stg44/pole_textures/text.inc"

static NJS_TEXNAME textures[5] = {
#include "stg44/pole_textures/textures.inc"
};
NJS_TEXLIST mad_space_pole_texlist = {textures,
                                      sizeof(textures) / sizeof(textures[0])};
