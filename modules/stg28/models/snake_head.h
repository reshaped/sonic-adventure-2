/* Pyramid Cave snake head, flame and collision models. */
#ifndef STG28_MODELS_SNAKE_HEAD_H
#define STG28_MODELS_SNAKE_HEAD_H

#include <ninja.h>
#include <sg_xpt.h>

static NJS_TEXNAME cave_snake_head_flame_texture_names[1] = {
#include "stg28/models/snake_head/cave_snake_head_flame_texture_names.inc"
};

static NJS_TEXLIST cave_snake_head_flame_texlist = {
#include "stg28/models/snake_head/cave_snake_head_flame_texlist.inc"
};

static Sint16 cave_snake_head_flame_polygons[25] = {
#include "stg28/models/snake_head/cave_snake_head_flame_polygons.inc"
};

static Sint32 cave_snake_head_flame_vertices[19] = {
#include "stg28/models/snake_head/cave_snake_head_flame_vertices.inc"
};

static NJS_CNK_MODEL cave_snake_head_flame_model = {
#include "stg28/models/snake_head/cave_snake_head_flame_model.inc"
};

static NJS_CNK_OBJECT cave_snake_head_flame_object = {
#include "stg28/models/snake_head/cave_snake_head_flame_object.inc"
};

static NJS_TEXNAME cave_snake_head_texture_names[2] = {
#include "stg28/models/snake_head/cave_snake_head_texture_names.inc"
};

static NJS_TEXLIST cave_snake_head_texlist = {
#include "stg28/models/snake_head/cave_snake_head_texlist.inc"
};

static Sint16 cave_snake_head_polygons[851] = {
#include "stg28/models/snake_head/cave_snake_head_polygons.inc"
};

static Sint32 cave_snake_head_vertices[619] = {
#include "stg28/models/snake_head/cave_snake_head_vertices.inc"
};

static NJS_CNK_MODEL cave_snake_head_model = {
#include "stg28/models/snake_head/cave_snake_head_model.inc"
};

static NJS_CNK_OBJECT cave_snake_head_object = {
#include "stg28/models/snake_head/cave_snake_head_object.inc"
};

static NJS_TEXNAME cave_snake_head_collision_texture_names[1] = {
#include "stg28/models/snake_head/cave_snake_head_collision_texture_names.inc"
};

static NJS_TEXLIST cave_snake_head_collision_texlist = {
#include "stg28/models/snake_head/cave_snake_head_collision_texlist.inc"
};

static NJS_MATERIAL cave_snake_head_collision_materials[1] = {
#include "stg28/models/snake_head/cave_snake_head_collision_materials.inc"
};

static Sint16 cave_snake_head_collision_indices[116] = {
#include "stg28/models/snake_head/cave_snake_head_collision_indices.inc"
};

static NJS_COLOR cave_snake_head_collision_uvs[92] = {
#include "stg28/models/snake_head/cave_snake_head_collision_uvs.inc"
};

static NJS_MESHSET cave_snake_head_collision_meshsets[1] = {
#include "stg28/models/snake_head/cave_snake_head_collision_meshsets.inc"
};

static NJS_POINT3 cave_snake_head_collision_points[28] = {
#include "stg28/models/snake_head/cave_snake_head_collision_points.inc"
};

static NJS_VECTOR cave_snake_head_collision_normals[28] = {
#include "stg28/models/snake_head/cave_snake_head_collision_normals.inc"
};

static NJS_MODEL cave_snake_head_collision_model = {
#include "stg28/models/snake_head/cave_snake_head_collision_model.inc"
};

static NJS_OBJECT cave_snake_head_collision = {
#include "stg28/models/snake_head/cave_snake_head_collision.inc"
};

#endif
