/* Street hammer render geometry. */
#ifndef SHARED_ROAD_FURNITURE_HAMMER_COLUMN_H
#define SHARED_ROAD_FURNITURE_HAMMER_COLUMN_H

#include <ninja.h>
#include <sg_xpt.h>

#include "shared/road_furniture/hammer.h"

static NJS_CNK_OBJECT street_hammer_glow_object = {
#include "shared/road_furniture/hammer_column/street_hammer_glow_object.inc"
};

static Sint16 street_hammer_polygons[387] = {
#include "shared/road_furniture/hammer_column/street_hammer_polygons.inc"
};

static Sint32 street_hammer_vertices[295] = {
#include "shared/road_furniture/hammer_column/street_hammer_vertices.inc"
};

#endif
