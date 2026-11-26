#include <ninja.h>

#include "stg21/fire_fixture.h"
#include "stg21/chain_fire_textures/text.inc"

static NJS_TEXNAME textures[2] = {
#include "stg21/chain_fire_textures/textures.inc"
};
NJS_TEXLIST chain_fire_texlist = {
#include "stg21/chain_fire_textures/chain_fire_texlist.inc"
};
