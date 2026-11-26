#include <ninja.h>

#include "stg21/box_textures.h"

const char box_stone_texture[14] = {
#include "stg21/box_part_0/box_stone_texture.inc"
};

static NJS_TEXNAME textures[3] = {
#include "stg21/box_part_0/textures.inc"
};
NJS_TEXLIST box_part_0_texlist = {
#include "stg21/box_part_0/box_part_0_texlist.inc"
};
