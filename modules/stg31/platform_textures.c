#include <ninja.h>

#include "stg31/platforms.h"

/* the set editor's labels for ASHIBA's placement, which no code reads */
static const char editor_angle_y[12] = "<- ANGLE Y";
static const char editor_type[8] = "<- TYPE";
static const char editor_speed[8] = "<- SPD";
static const char editor_width[12] = "<- WIDTH";

#include "stg31/platform_textures/text.inc"

static NJS_TEXNAME textures[3] = {
#include "stg31/platform_textures/textures.inc"
};

NJS_TEXLIST green_hill_platform_texlist = {textures, sizeof(textures) /
                                                         sizeof(textures[0])};
