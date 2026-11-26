/* Egg Quarters coffin collision. */
#ifndef STG26_COFFIN_MESHES_COLLISION_H
#define STG26_COFFIN_MESHES_COLLISION_H

#include <ninja.h>
#include <sg_xpt.h>

static NJS_MATERIAL collision_materials[1] = {
#include "stg26/coffin_meshes/collision/collision_materials.inc"
};

static Sint16 collision_faces[58] = {
#include "stg26/coffin_meshes/collision/collision_faces.inc"
};

static NJS_MESHSET collision_meshsets[1] = {
#include "stg26/coffin_meshes/collision/collision_meshsets.inc"
};

static NJS_POINT3 collision_points[14] = {
#include "stg26/coffin_meshes/collision/collision_points.inc"
};

static NJS_VECTOR collision_normals[14] = {
#include "stg26/coffin_meshes/collision/collision_normals.inc"
};

static NJS_MODEL collision_model = {
#include "stg26/coffin_meshes/collision/collision_model.inc"
};

static NJS_OBJECT collision = {
#include "stg26/coffin_meshes/collision/collision.inc"
};

#endif
