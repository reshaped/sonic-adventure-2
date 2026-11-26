/* STG24.PRS's chunk models and Basic models, from the disc. its D lies
 * between floor_hatch_tex.c and wall_hatch_tex.c */
#include <ninja.h>
#include <sg_xpt.h>

#include "stg24/hatch_art.h"

static Sint16 spring_polygons[243] = {
#include "stg24/hatch_art/spring_polygons.inc"
};
static Sint32 spring_vertices[275] = {
#include "stg24/hatch_art/spring_vertices.inc"
};
NJS_CNK_MODEL floor_hatch_spring_model = {
#include "stg24/hatch_art/floor_hatch_spring_model.inc"
};
NJS_CNK_OBJECT floor_hatch_spring = {
#include "stg24/hatch_art/floor_hatch_spring.inc"
};
static Sint16 open_lamp_polygons[65] = {
#include "stg24/hatch_art/open_lamp_polygons.inc"
};
static Sint32 open_lamp_vertices[35] = {
#include "stg24/hatch_art/open_lamp_vertices.inc"
};
NJS_CNK_MODEL floor_hatch_open_lamp_model = {
#include "stg24/hatch_art/floor_hatch_open_lamp_model.inc"
};
NJS_CNK_OBJECT floor_hatch_open_lamp = {
#include "stg24/hatch_art/floor_hatch_open_lamp.inc"
};
static Sint16 lamp_polygons[25] = {
#include "stg24/hatch_art/lamp_polygons.inc"
};
static Sint32 lamp_vertices[19] = {
#include "stg24/hatch_art/lamp_vertices.inc"
};
NJS_CNK_MODEL floor_hatch_lamp_model = {
#include "stg24/hatch_art/floor_hatch_lamp_model.inc"
};
NJS_CNK_OBJECT floor_hatch_lamp = {
#include "stg24/hatch_art/floor_hatch_lamp.inc"
};
static Sint16 lid_polygons[321] = {
#include "stg24/hatch_art/lid_polygons.inc"
};
static Sint32 lid_vertices[215] = {
#include "stg24/hatch_art/lid_vertices.inc"
};
NJS_CNK_MODEL floor_hatch_lid_model[1] = {
#include "stg24/hatch_art/floor_hatch_lid_model.inc"
};
NJS_CNK_OBJECT floor_hatch_lid = {
#include "stg24/hatch_art/floor_hatch_lid.inc"
};
static NJS_MATERIAL collision_materials[1] = {
#include "stg24/hatch_art/collision_materials.inc"
};
static Sint16 collision_indices[5] = {
#include "stg24/hatch_art/collision_indices.inc"
};
static NJS_MESHSET collision_meshsets[1] = {
#include "stg24/hatch_art/collision_meshsets.inc"
};
static NJS_POINT3 collision_points[4] = {
#include "stg24/hatch_art/collision_points.inc"
};
static NJS_POINT3 collision_normals[4] = {
#include "stg24/hatch_art/collision_normals.inc"
};
static NJS_MODEL collision_model = {
#include "stg24/hatch_art/collision_model.inc"
};
NJS_OBJECT floor_hatch_collision = {
#include "stg24/hatch_art/floor_hatch_collision.inc"
};
