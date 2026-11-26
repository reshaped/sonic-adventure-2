#ifndef SHARED_GOLEM_REEL_REEL_COLLISION_H
#define SHARED_GOLEM_REEL_REEL_COLLISION_H

#include <ninja.h>
#include <sg_xpt.h>

static NJS_MATERIAL reel_collision_materials[1] = {
#include "shared/golem_reel/reel_collision/reel_collision_materials.inc"
};

static Sint16 reel_collision_faces[24] = {
#include "shared/golem_reel/reel_collision/reel_collision_faces.inc"
};

static NJS_MESHSET reel_collision_meshes[1] = {
#include "shared/golem_reel/reel_collision/reel_collision_meshes.inc"
};

static NJS_POINT3 reel_collision_points[8] = {
#include "shared/golem_reel/reel_collision/reel_collision_points.inc"
};

static NJS_VECTOR reel_collision_normals[8] = {
#include "shared/golem_reel/reel_collision/reel_collision_normals.inc"
};

static NJS_MODEL reel_collision_model = {
#include "shared/golem_reel/reel_collision/reel_collision_model.inc"
};

static NJS_OBJECT reel_collision = {
#include "shared/golem_reel/reel_collision/reel_collision.inc"
};

#endif
