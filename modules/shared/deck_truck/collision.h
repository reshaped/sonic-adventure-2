#ifndef SHARED_DECK_TRUCK_COLLISION_H
#define SHARED_DECK_TRUCK_COLLISION_H

#include <ninja.h>

static NJS_MATERIAL collision_material[1] = {
#include "shared/deck_truck/collision/collision_material.inc"
};

static Sint16 collision_indices[16] = {
#include "shared/deck_truck/collision/collision_indices.inc"
};

static NJS_MESHSET collision_meshes[1] = {
#include "shared/deck_truck/collision/collision_meshes.inc"
};

static NJS_POINT3 collision_points[8] = {
#include "shared/deck_truck/collision/collision_points.inc"
};

static NJS_VECTOR collision_normals[8] = {
#include "shared/deck_truck/collision/collision_normals.inc"
};

static NJS_MODEL truck_collision_model = {
#include "shared/deck_truck/collision/truck_collision_model.inc"
};

static NJS_OBJECT deck_truck_collision = {
#include "shared/deck_truck/collision/deck_truck_collision.inc"
};

#endif
