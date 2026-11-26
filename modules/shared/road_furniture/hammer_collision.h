/* Street hammer solid geometry. */
#ifndef SHARED_ROAD_FURNITURE_HAMMER_COLLISION_H
#define SHARED_ROAD_FURNITURE_HAMMER_COLLISION_H

#include <ninja.h>
#include <sg_xpt.h>

#include "shared/road_furniture/hammer.h"

static NJS_CNK_OBJECT street_hammer_object = {
#include "shared/road_furniture/hammer_collision/street_hammer_object.inc"
};

static NJS_MATERIAL street_hammer_collision_materials[1] = {
#include "shared/road_furniture/hammer_collision/street_hammer_collision_materials.inc"
};

static Sint16 street_hammer_collision_indices_0[35] = {
#include "shared/road_furniture/hammer_collision/street_hammer_collision_indices_0.inc"
};

static NJS_MESHSET street_hammer_collision_meshsets[1] = {
#include "shared/road_furniture/hammer_collision/street_hammer_collision_meshsets.inc"
};

static NJS_POINT3 street_hammer_collision_points[12] = {
#include "shared/road_furniture/hammer_collision/street_hammer_collision_points.inc"
};

static NJS_VECTOR street_hammer_collision_normals[12] = {
#include "shared/road_furniture/hammer_collision/street_hammer_collision_normals.inc"
};

static NJS_MODEL street_hammer_collision_model = {
#include "shared/road_furniture/hammer_collision/street_hammer_collision_model.inc"
};

#endif
