#include <ninja.h>

#include "stg21/fire_fixture.h"
#include "stg21/post_fire_textures/text.inc"

static NJS_TEXNAME textures[3] = {
#include "stg21/post_fire_textures/textures.inc"
};
NJS_TEXLIST post_fire_texlist = {
#include "stg21/post_fire_textures/post_fire_texlist.inc"
};
