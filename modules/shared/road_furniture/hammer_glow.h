/* Street hammer glow geometry. */
#ifndef SHARED_ROAD_FURNITURE_HAMMER_GLOW_H
#define SHARED_ROAD_FURNITURE_HAMMER_GLOW_H

#include <ninja.h>
#include <sg_xpt.h>

#include "shared/road_furniture/hammer.h"

static Sint16 street_hammer_glow_polygons[91] = {
#include "shared/road_furniture/hammer_glow/street_hammer_glow_polygons.inc"
};

static Sint32 street_hammer_glow_vertices[51] = {
#include "shared/road_furniture/hammer_glow/street_hammer_glow_vertices.inc"
};

#endif
