#include <ninja.h>

#include "stg21/box_textures.h"

static NJS_TEXNAME textures[2] = {
#include "stg21/box_part_2/textures.inc"
};
NJS_TEXLIST box_part_2_texlist = {
#include "stg21/box_part_2/box_part_2_texlist.inc"
};
