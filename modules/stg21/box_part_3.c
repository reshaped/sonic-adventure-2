#include <ninja.h>

#include "stg21/box_textures.h"

static NJS_TEXNAME textures[2] = {
#include "stg21/box_part_3/textures.inc"
};
NJS_TEXLIST box_part_3_texlist = {
#include "stg21/box_part_3/box_part_3_texlist.inc"
};
