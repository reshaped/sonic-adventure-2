#include <ninja.h>

#include "stg44/lights.h"

#include "stg44/line_textures/text.inc"

static NJS_TEXNAME textures[1] = {
#include "stg44/line_textures/textures.inc"
};
NJS_TEXLIST mad_space_line_texlist = {textures,
                                      sizeof(textures) / sizeof(textures[0])};
