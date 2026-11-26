/* Safe solid geometry. */
#ifndef STG08_SAFE_SAFE_SOLID_H
#define STG08_SAFE_SAFE_SOLID_H

#include <ninja.h>

static NJS_MATERIAL safe_collision_materials = {
#include "stg08/safe/safe_solid/safe_collision_materials.inc"
};

static Uint16 safe_collision_strips[5] = {
#include "stg08/safe/safe_solid/safe_collision_strips.inc"
};

static NJS_MESHSET safe_collision_faces = {
#include "stg08/safe/safe_solid/safe_collision_faces.inc"
};

static NJS_POINT3 safe_collision_points[4] = {
#include "stg08/safe/safe_solid/safe_collision_points.inc"
};

static NJS_VECTOR safe_collision_normals[4] = {
#include "stg08/safe/safe_solid/safe_collision_normals.inc"
};

static NJS_MODEL safe_collision_model = {
#include "stg08/safe/safe_solid/safe_collision_model.inc"
};

static NJS_OBJECT security_safe_collision = {
#include "stg08/safe/safe_solid/security_safe_collision.inc"
};

#endif
