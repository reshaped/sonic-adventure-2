/* ASHIBA's model and its collision object */

#include "stg31/prop_art.h"
#include <ninja.h>
#include <sg_xpt.h>

static Sint16 platform_polygons[301] = {
#include "stg31/platform_art/platform_polygons.inc"
};

static Sint32 platform_vertices[291] = {
#include "stg31/platform_art/platform_vertices.inc"
};

static NJS_CNK_MODEL platform_model = {
#include "stg31/platform_art/platform_model.inc"
};

NJS_CNK_OBJECT green_hill_platform_object[1] = {
#include "stg31/platform_art/green_hill_platform_object.inc"
};
static NJS_MATERIAL platform_collision_materials[1] = {
#include "stg31/platform_art/platform_collision_materials.inc"
};

static Sint16 platform_collision_faces[21] = {
#include "stg31/platform_art/platform_collision_faces.inc"
};

static NJS_MESHSET platform_collision_meshsets[1] = {
#include "stg31/platform_art/platform_collision_meshsets.inc"
};

static NJS_POINT3 platform_collision_points[8] = {
#include "stg31/platform_art/platform_collision_points.inc"
};

static NJS_VECTOR platform_collision_normals[8] = {
#include "stg31/platform_art/platform_collision_normals.inc"
};

static NJS_MODEL platform_collision_model = {
#include "stg31/platform_art/platform_collision_model.inc"
};

NJS_OBJECT green_hill_platform_collision = {
#include "stg31/platform_art/green_hill_platform_collision.inc"
};
