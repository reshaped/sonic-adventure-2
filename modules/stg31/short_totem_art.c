/* the short totem's model, then both totems' collision objects, which no code
 * reads */
#include "stg31/prop_art.h"
#include <ninja.h>
#include <sg_xpt.h>

static Sint16 short_totem_polygons[131] = {
#include "stg31/short_totem_art/short_totem_polygons.inc"
};

static Sint32 short_totem_vertices[95] = {
#include "stg31/short_totem_art/short_totem_vertices.inc"
};

static NJS_CNK_MODEL short_totem_model = {
#include "stg31/short_totem_art/short_totem_model.inc"
};

NJS_CNK_OBJECT green_hill_short_totem_object[1] = {
#include "stg31/short_totem_art/green_hill_short_totem_object.inc"
};
static NJS_MATERIAL tall_totem_collision_materials[1] = {
#include "stg31/short_totem_art/tall_totem_collision_materials.inc"
};

static Sint16 tall_totem_collision_faces[27] = {
#include "stg31/short_totem_art/tall_totem_collision_faces.inc"
};

static NJS_MESHSET tall_totem_collision_meshsets[1] = {
#include "stg31/short_totem_art/tall_totem_collision_meshsets.inc"
};

static NJS_POINT3 tall_totem_collision_points[17] = {
#include "stg31/short_totem_art/tall_totem_collision_points.inc"
};

static NJS_VECTOR tall_totem_collision_normals[17] = {
#include "stg31/short_totem_art/tall_totem_collision_normals.inc"
};

static NJS_MODEL tall_totem_collision_model = {
#include "stg31/short_totem_art/tall_totem_collision_model.inc"
};

NJS_OBJECT green_hill_tall_totem_collision = {
#include "stg31/short_totem_art/green_hill_tall_totem_collision.inc"
};
static NJS_MATERIAL short_totem_collision_materials[1] = {
#include "stg31/short_totem_art/short_totem_collision_materials.inc"
};

static Sint16 short_totem_collision_faces[27] = {
#include "stg31/short_totem_art/short_totem_collision_faces.inc"
};

static NJS_MESHSET short_totem_collision_meshsets[1] = {
#include "stg31/short_totem_art/short_totem_collision_meshsets.inc"
};

static NJS_POINT3 short_totem_collision_points[17] = {
#include "stg31/short_totem_art/short_totem_collision_points.inc"
};

static NJS_VECTOR short_totem_collision_normals[17] = {
#include "stg31/short_totem_art/short_totem_collision_normals.inc"
};

static NJS_MODEL short_totem_collision_model = {
#include "stg31/short_totem_art/short_totem_collision_model.inc"
};

NJS_OBJECT green_hill_short_totem_collision = {
#include "stg31/short_totem_art/green_hill_short_totem_collision.inc"
};
