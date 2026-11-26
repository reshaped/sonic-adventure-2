#include <ninja.h>
#include <sg_xpt.h>

#include "stg32/scenery_shapes.h"

static Sint16 meteor_platform_object_polygons[911] = {
#include "stg32/scenery/platform/meteor_platform_object_polygons.inc"
};

static Sint32 meteor_platform_object_vertices[603] = {
#include "stg32/scenery/platform/meteor_platform_object_vertices.inc"
};

static NJS_CNK_MODEL meteor_platform_object_model = {
#include "stg32/scenery/platform/meteor_platform_object_model.inc"
};

NJS_CNK_OBJECT meteor_platform_object[1] = {
#include "stg32/scenery/platform/meteor_platform_object.inc"
};

static NJS_MATERIAL meteor_platform_dusty_collision_materials[1] = {
#include "stg32/scenery/platform/meteor_platform_dusty_collision_materials.inc"
};

static Sint16 meteor_platform_dusty_collision_faces[52] = {
#include "stg32/scenery/platform/meteor_platform_dusty_collision_faces.inc"
};

static NJS_MESHSET meteor_platform_dusty_collision_meshsets[1] = {
#include "stg32/scenery/platform/meteor_platform_dusty_collision_meshsets.inc"
};

static NJS_POINT3 meteor_platform_dusty_collision_points[28] = {
#include "stg32/scenery/platform/meteor_platform_dusty_collision_points.inc"
};

static NJS_VECTOR meteor_platform_dusty_collision_normals[28] = {
#include "stg32/scenery/platform/meteor_platform_dusty_collision_normals.inc"
};

static NJS_MODEL meteor_platform_dusty_collision_model = {
#include "stg32/scenery/platform/meteor_platform_dusty_collision_model.inc"
};

NJS_OBJECT meteor_platform_dusty_collision[1] = {
#include "stg32/scenery/platform/meteor_platform_dusty_collision.inc"
};

static NJS_MATERIAL meteor_platform_collision_materials[1] = {
#include "stg32/scenery/platform/meteor_platform_collision_materials.inc"
};

static Sint16 meteor_platform_collision_faces[72] = {
#include "stg32/scenery/platform/meteor_platform_collision_faces.inc"
};

static NJS_MESHSET meteor_platform_collision_meshsets[1] = {
#include "stg32/scenery/platform/meteor_platform_collision_meshsets.inc"
};

static NJS_POINT3 meteor_platform_collision_points[34] = {
#include "stg32/scenery/platform/meteor_platform_collision_points.inc"
};

static NJS_VECTOR meteor_platform_collision_normals[34] = {
#include "stg32/scenery/platform/meteor_platform_collision_normals.inc"
};

static NJS_MODEL meteor_platform_collision_model = {
#include "stg32/scenery/platform/meteor_platform_collision_model.inc"
};

NJS_OBJECT meteor_platform_collision[1] = {
#include "stg32/scenery/platform/meteor_platform_collision.inc"
};
