#include <ninja.h>

#include "stg31/floor_textures.h"
#include "stg31/platforms.h"

static NJS_TEXNAME textures[5] = {
#include "stg31/shard_textures/textures.inc"
};
NJS_TEXLIST green_hill_floor_pieces_texlist = {
    textures, sizeof(textures) / sizeof(textures[0])};
