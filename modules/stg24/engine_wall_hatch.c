/* STG24.PRS's chunk models and Basic models, from the disc. its D lies
 * between wall_hatch_tex.c and hatch_textures.c */
#include <ninja.h>
#include <sg_xpt.h>

#include "stg24/hatch_art.h"

static Sint16 spring_polygons[243] = {
#include "stg24/engine_wall_hatch/spring_polygons.inc"
};
static Sint32 spring_vertices[243] = {
#include "stg24/engine_wall_hatch/spring_vertices.inc"
};
NJS_CNK_MODEL wall_hatch_spring_model = {
#include "stg24/engine_wall_hatch/wall_hatch_spring_model.inc"
};
NJS_CNK_OBJECT wall_hatch_spring[1] = {
#include "stg24/engine_wall_hatch/wall_hatch_spring.inc"
};
static Sint16 open_lamp_polygons[25] = {
#include "stg24/engine_wall_hatch/open_lamp_polygons.inc"
};
static Sint32 open_lamp_vertices[19] = {
#include "stg24/engine_wall_hatch/open_lamp_vertices.inc"
};
NJS_CNK_MODEL wall_hatch_open_lamp_model = {
#include "stg24/engine_wall_hatch/wall_hatch_open_lamp_model.inc"
};
NJS_CNK_OBJECT wall_hatch_open_lamp[1] = {
#include "stg24/engine_wall_hatch/wall_hatch_open_lamp.inc"
};
static Sint16 closed_lamp_polygons[25] = {
#include "stg24/engine_wall_hatch/closed_lamp_polygons.inc"
};
static Sint32 closed_lamp_vertices[19] = {
#include "stg24/engine_wall_hatch/closed_lamp_vertices.inc"
};
NJS_CNK_MODEL wall_hatch_closed_lamp_model = {
#include "stg24/engine_wall_hatch/wall_hatch_closed_lamp_model.inc"
};
NJS_CNK_OBJECT wall_hatch_closed_lamp[1] = {
#include "stg24/engine_wall_hatch/wall_hatch_closed_lamp.inc"
};
static Sint16 lamp_polygons[25] = {
#include "stg24/engine_wall_hatch/lamp_polygons.inc"
};
static Sint32 lamp_vertices[19] = {
#include "stg24/engine_wall_hatch/lamp_vertices.inc"
};
NJS_CNK_MODEL wall_hatch_lamp_model = {
#include "stg24/engine_wall_hatch/wall_hatch_lamp_model.inc"
};
NJS_CNK_OBJECT wall_hatch_lamp[1] = {
#include "stg24/engine_wall_hatch/wall_hatch_lamp.inc"
};
static Sint16 lid_polygons[409] = {
#include "stg24/engine_wall_hatch/lid_polygons.inc"
};
static Sint32 lid_vertices[291] = {
#include "stg24/engine_wall_hatch/lid_vertices.inc"
};
NJS_CNK_MODEL wall_hatch_lid_model[1] = {
#include "stg24/engine_wall_hatch/wall_hatch_lid_model.inc"
};
NJS_CNK_OBJECT wall_hatch_lid = {
#include "stg24/engine_wall_hatch/wall_hatch_lid.inc"
};
static NJS_MATERIAL collision_materials[1] = {
#include "stg24/engine_wall_hatch/collision_materials.inc"
};
static Sint16 collision_indices[9] = {
#include "stg24/engine_wall_hatch/collision_indices.inc"
};
static NJS_MESHSET collision_meshsets[1] = {
#include "stg24/engine_wall_hatch/collision_meshsets.inc"
};
static NJS_POINT3 collision_points[8] = {
#include "stg24/engine_wall_hatch/collision_points.inc"
};
static NJS_POINT3 collision_normals[8] = {
#include "stg24/engine_wall_hatch/collision_normals.inc"
};
static NJS_MODEL collision_model = {
#include "stg24/engine_wall_hatch/collision_model.inc"
};
NJS_OBJECT wall_hatch_collision = {
#include "stg24/engine_wall_hatch/wall_hatch_collision.inc"
};
