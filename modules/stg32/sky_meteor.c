/* the sky's meteor and its texture list. the background turns the meteor about
 * its model's center */
#include <ninja.h>

#include "stg32/sky.h"

#include "stg32/sky_meteor/text.inc"

static NJS_TEXNAME textures[1] = {
#include "stg32/sky_meteor/textures.inc"
};
NJS_TEXLIST meteor_sky_meteor_texlist = {textures, 1};

#include "shared/celestial/meteor.h"

NJS_CNK_OBJECT meteor_sky_meteor[1] = {
#include "stg32/sky_meteor/meteor_sky_meteor.inc"
};
