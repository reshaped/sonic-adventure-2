#include <ninja.h>

#include "stg21/fire_fixture.h"
#include "stg21/chain_flame_texture/text.inc"

static NJS_TEXNAME texture = {
#include "stg21/chain_flame_texture/texture.inc"
};
NJS_TEXLIST chain_fire_flame_texlist = {
#include "stg21/chain_flame_texture/chain_fire_flame_texlist.inc"
};
