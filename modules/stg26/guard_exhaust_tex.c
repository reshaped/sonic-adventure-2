#include <ninja.h>

#include "stg26/guard_patrol.h"

#include "stg26/guard_exhaust_tex/text.inc"

static NJS_TEXNAME textures[1] = {
#include "stg26/guard_exhaust_tex/textures.inc"
};
NJS_TEXLIST guard_exhaust_texlist = {textures,
                                     sizeof(textures) / sizeof(textures[0])};
