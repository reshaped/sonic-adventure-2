#include <ninja.h>
#include <sg_xpt.h>

#include "stg44/scenery_shapes.h"

static Sint16 landing_model_polygons[917] = {
#include "stg44/scenery/landing/landing_model_polygons.inc"
};

static Sint32 landing_model_vertices[587] = {
#include "stg44/scenery/landing/landing_model_vertices.inc"
};

static NJS_CNK_MODEL landing_model_model = {
#include "stg44/scenery/landing/landing_model_model.inc"
};

NJS_CNK_OBJECT landing_model[1] = {
#include "stg44/scenery/landing/landing_model.inc"
};

static NJS_MATERIAL landing_dust_collision_materials[1] = {
#include "stg44/scenery/landing/landing_dust_collision_materials.inc"
};

static Sint16 landing_dust_collision_faces[52] = {
#include "stg44/scenery/landing/landing_dust_collision_faces.inc"
};

static NJS_MESHSET landing_dust_collision_meshsets[1] = {
#include "stg44/scenery/landing/landing_dust_collision_meshsets.inc"
};

static NJS_POINT3 landing_dust_collision_points[28] = {
#include "stg44/scenery/landing/landing_dust_collision_points.inc"
};

static NJS_VECTOR landing_dust_collision_normals[28] = {
#include "stg44/scenery/landing/landing_dust_collision_normals.inc"
};

static NJS_MODEL landing_dust_collision_model = {
#include "stg44/scenery/landing/landing_dust_collision_model.inc"
};

NJS_OBJECT landing_dust_collision[1] = {
#include "stg44/scenery/landing/landing_dust_collision.inc"
};

static NJS_MATERIAL landing_collision_materials[1] = {
#include "stg44/scenery/landing/landing_collision_materials.inc"
};

static Sint16 landing_collision_faces[72] = {
#include "stg44/scenery/landing/landing_collision_faces.inc"
};

static NJS_MESHSET landing_collision_meshsets[1] = {
#include "stg44/scenery/landing/landing_collision_meshsets.inc"
};

static NJS_POINT3 landing_collision_points[34] = {
#include "stg44/scenery/landing/landing_collision_points.inc"
};

static NJS_VECTOR landing_collision_normals[34] = {
#include "stg44/scenery/landing/landing_collision_normals.inc"
};

static NJS_MODEL landing_collision_model = {
#include "stg44/scenery/landing/landing_collision_model.inc"
};

NJS_OBJECT landing_collision[1] = {
#include "stg44/scenery/landing/landing_collision.inc"
};
