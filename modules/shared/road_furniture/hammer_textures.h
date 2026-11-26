/* Street hammer texture slots. */
#ifndef SHARED_ROAD_FURNITURE_HAMMER_TEXTURES_H
#define SHARED_ROAD_FURNITURE_HAMMER_TEXTURES_H

#include <ninja.h>
#include <sg_xpt.h>

#include "shared/road_furniture/hammer.h"

static NJS_TEXNAME street_hammer_textures[4] = {
#include "shared/road_furniture/hammer_textures/street_hammer_textures.inc"
};

#endif
