#include <ninja.h>

#include "stg26/guard_patrol.h"

#include "stg26/guard_lens_texture/text.inc"

static NJS_TEXNAME textures[1] = {
#include "stg26/guard_lens_texture/textures.inc"
};
NJS_TEXLIST guard_lens_texlist = {textures,
                                  sizeof(textures) / sizeof(textures[0])};
