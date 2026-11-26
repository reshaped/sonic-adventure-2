/* the sky's meteor and its texture list. the background turns the meteor about
 * its model's center */
#include <ninja.h>

#include "stg44/sky.h"
#include "stg44/sky_textures.h"

static NJS_TEXNAME textures[] = {{(void *)mad_space_meteor_128_3, 0, 0}};
NJS_TEXLIST background_4_texlist = {textures, 1};

#include "shared/celestial/meteor.h"

NJS_CNK_OBJECT background_4_object[1] = {
#include "stg44/sky_meteor/background_4_object.inc"
};
