/* Treasure chest collision. */
#ifndef SHARED_TREASURE_MESHES_CHEST_COLLISION_H
#define SHARED_TREASURE_MESHES_CHEST_COLLISION_H

#include <ninja.h>
#include <sg_xpt.h>

static NJS_MATERIAL collision_materials[1] = {
#include "shared/treasure_meshes/chest_collision/collision_materials.inc"
};

static Sint16 collision_faces[31] = {
#include "shared/treasure_meshes/chest_collision/collision_faces.inc"
};

static NJS_MESHSET collision_meshsets[1] = {
#include "shared/treasure_meshes/chest_collision/collision_meshsets.inc"
};

static NJS_POINT3 collision_points[14] = {
#include "shared/treasure_meshes/chest_collision/collision_points.inc"
};

static NJS_VECTOR collision_normals[14] = {
#include "shared/treasure_meshes/chest_collision/collision_normals.inc"
};

static NJS_MODEL collision_model = {
#include "shared/treasure_meshes/chest_collision/collision_model.inc"
};

static NJS_OBJECT collision = {
#include "shared/treasure_meshes/chest_collision/collision.inc"
};

#endif
