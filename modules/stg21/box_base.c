#include <ninja.h>

#include "stg21/box_textures.h"
#include "stg21/box_base/text.inc"

static NJS_TEXNAME textures[3] = {
#include "stg21/box_base/textures.inc"
};
NJS_TEXLIST box_base_texlist = {
#include "stg21/box_base/box_base_texlist.inc"
};
