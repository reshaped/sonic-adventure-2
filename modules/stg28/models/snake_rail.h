/* Pyramid Cave snake rail and its collision model. */
#ifndef STG28_MODELS_SNAKE_RAIL_H
#define STG28_MODELS_SNAKE_RAIL_H

#include <ninja.h>
#include <sg_xpt.h>

static NJS_TEXNAME cave_snake_rail_texture_names[2] = {
#include "stg28/models/snake_rail/cave_snake_rail_texture_names.inc"
};

static NJS_TEXLIST cave_snake_rail_texlist = {
#include "stg28/models/snake_rail/cave_snake_rail_texlist.inc"
};

static Sint16 cave_snake_rail_polygons[669] = {
#include "stg28/models/snake_rail/cave_snake_rail_polygons.inc"
};

static Sint32 cave_snake_rail_vertices[363] = {
#include "stg28/models/snake_rail/cave_snake_rail_vertices.inc"
};

static NJS_CNK_MODEL cave_snake_rail_model = {
#include "stg28/models/snake_rail/cave_snake_rail_model.inc"
};

static NJS_CNK_OBJECT cave_snake_rail_object = {
#include "stg28/models/snake_rail/cave_snake_rail_object.inc"
};

static NJS_MATERIAL cave_snake_rail_collision_materials[1] = {
#include "stg28/models/snake_rail/cave_snake_rail_collision_materials.inc"
};

static Sint16 cave_snake_rail_collision_indices[59] = {
#include "stg28/models/snake_rail/cave_snake_rail_collision_indices.inc"
};

static NJS_MESHSET cave_snake_rail_collision_meshsets[1] = {
#include "stg28/models/snake_rail/cave_snake_rail_collision_meshsets.inc"
};

static NJS_POINT3 cave_snake_rail_collision_points[18] = {
#include "stg28/models/snake_rail/cave_snake_rail_collision_points.inc"
};

static NJS_VECTOR cave_snake_rail_collision_normals[18] = {
#include "stg28/models/snake_rail/cave_snake_rail_collision_normals.inc"
};

static NJS_MODEL cave_snake_rail_collision_model = {
#include "stg28/models/snake_rail/cave_snake_rail_collision_model.inc"
};

static NJS_OBJECT cave_snake_rail_collision = {
#include "stg28/models/snake_rail/cave_snake_rail_collision.inc"
};

#endif
