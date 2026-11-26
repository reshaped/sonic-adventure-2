#include <ninja.h>

#include "stg21/box_textures.h"

/* Two trailing zeros align the wall filename to four bytes. */
const char box_pillar_texture[16] = {
#include "stg21/box_textures/box_pillar_texture.inc"
};
const char box_wall_texture[12] = {
#include "stg21/box_textures/box_wall_texture.inc"
};
const char box_shrine_texture[14] = {
#include "stg21/box_textures/box_shrine_texture.inc"
};

static NJS_TEXNAME textures[3] = {
#include "stg21/box_textures/textures.inc"
};
NJS_TEXLIST stone_box_texlist = {
#include "stg21/box_textures/stone_box_texlist.inc"
};
