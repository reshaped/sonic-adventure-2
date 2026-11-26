/* Right and left snake statues and their collision models. */
#ifndef STG28_MODELS_SNAKE_STATUE_H
#define STG28_MODELS_SNAKE_STATUE_H

#include <ninja.h>
#include <sg_xpt.h>

static NJS_TEXNAME cave_snake_statue_right_texture_names[5] = {
#include "stg28/models/snake_statue/cave_snake_statue_right_texture_names.inc"
};

static NJS_TEXLIST cave_snake_statue_right_texlist = {
#include "stg28/models/snake_statue/cave_snake_statue_right_texlist.inc"
};

static Sint16 cave_snake_statue_right_polygons[415] = {
#include "stg28/models/snake_statue/cave_snake_statue_right_polygons.inc"
};

static Sint32 cave_snake_statue_right_vertices[199] = {
#include "stg28/models/snake_statue/cave_snake_statue_right_vertices.inc"
};

static NJS_CNK_MODEL cave_snake_statue_right_model = {
#include "stg28/models/snake_statue/cave_snake_statue_right_model.inc"
};

static NJS_CNK_OBJECT cave_snake_statue_right_object = {
#include "stg28/models/snake_statue/cave_snake_statue_right_object.inc"
};

static NJS_MATERIAL cave_snake_statue_right_collision_materials[1] = {
#include "stg28/models/snake_statue/cave_snake_statue_right_collision_materials.inc"
};

static Sint16 cave_snake_statue_right_collision_indices[52] = {
#include "stg28/models/snake_statue/cave_snake_statue_right_collision_indices.inc"
};

static NJS_MESHSET cave_snake_statue_right_collision_meshsets[1] = {
#include "stg28/models/snake_statue/cave_snake_statue_right_collision_meshsets.inc"
};

static NJS_POINT3 cave_snake_statue_right_collision_points[20] = {
#include "stg28/models/snake_statue/cave_snake_statue_right_collision_points.inc"
};

static NJS_VECTOR cave_snake_statue_right_collision_normals[20] = {
#include "stg28/models/snake_statue/cave_snake_statue_right_collision_normals.inc"
};

static NJS_MODEL cave_snake_statue_right_collision_model = {
#include "stg28/models/snake_statue/cave_snake_statue_right_collision_model.inc"
};

static NJS_OBJECT cave_snake_statue_right_collision = {
#include "stg28/models/snake_statue/cave_snake_statue_right_collision.inc"
};

static NJS_TEXNAME cave_snake_statue_left_texture_names[5] = {
#include "stg28/models/snake_statue/cave_snake_statue_left_texture_names.inc"
};

static NJS_TEXLIST cave_snake_statue_left_texlist = {
#include "stg28/models/snake_statue/cave_snake_statue_left_texlist.inc"
};

static Sint16 cave_snake_statue_left_polygons[415] = {
#include "stg28/models/snake_statue/cave_snake_statue_left_polygons.inc"
};

static Sint32 cave_snake_statue_left_vertices[199] = {
#include "stg28/models/snake_statue/cave_snake_statue_left_vertices.inc"
};

static NJS_CNK_MODEL cave_snake_statue_left_model = {
#include "stg28/models/snake_statue/cave_snake_statue_left_model.inc"
};

static NJS_CNK_OBJECT cave_snake_statue_left_object = {
#include "stg28/models/snake_statue/cave_snake_statue_left_object.inc"
};

static NJS_MATERIAL cave_snake_statue_left_collision_materials[1] = {
#include "stg28/models/snake_statue/cave_snake_statue_left_collision_materials.inc"
};

static Sint16 cave_snake_statue_left_collision_indices[49] = {
#include "stg28/models/snake_statue/cave_snake_statue_left_collision_indices.inc"
};

static NJS_MESHSET cave_snake_statue_left_collision_meshsets[1] = {
#include "stg28/models/snake_statue/cave_snake_statue_left_collision_meshsets.inc"
};

static NJS_POINT3 cave_snake_statue_left_collision_points[20] = {
#include "stg28/models/snake_statue/cave_snake_statue_left_collision_points.inc"
};

static NJS_VECTOR cave_snake_statue_left_collision_normals[20] = {
#include "stg28/models/snake_statue/cave_snake_statue_left_collision_normals.inc"
};

static NJS_MODEL cave_snake_statue_left_collision_model = {
#include "stg28/models/snake_statue/cave_snake_statue_left_collision_model.inc"
};

static NJS_OBJECT cave_snake_statue_left_collision = {
#include "stg28/models/snake_statue/cave_snake_statue_left_collision.inc"
};

#endif
