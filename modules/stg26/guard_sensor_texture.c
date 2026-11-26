#include <ninja.h>

#include "stg26/guard_patrol.h"

#include "stg26/guard_sensor_texture/text.inc"

static NJS_TEXNAME textures[1] = {
#include "stg26/guard_sensor_texture/textures.inc"
};
NJS_TEXLIST guard_sensor_texlist = {textures,
                                    sizeof(textures) / sizeof(textures[0])};
