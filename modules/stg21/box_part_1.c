#include <ninja.h>

#include "stg21/box_textures.h"

static NJS_TEXNAME textures[3] = {
#include "stg21/box_part_1/textures.inc"
};
NJS_TEXLIST box_part_1_texlist = {
#include "stg21/box_part_1/box_part_1_texlist.inc"
};
