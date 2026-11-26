/* Pyramid Cave stone awning, three flames and solid base. */
#ifndef STG28_MODELS_AWNING_H
#define STG28_MODELS_AWNING_H

#include <ninja.h>
#include <sg_xpt.h>

static NJS_TEXNAME cave_awning_side_flame_texture_names[1] = {
#include "stg28/models/awning/cave_awning_side_flame_texture_names.inc"
};

static NJS_TEXLIST cave_awning_side_flame_texlist = {
#include "stg28/models/awning/cave_awning_side_flame_texlist.inc"
};

static Sint16 cave_awning_side_flame_polygons[25] = {
#include "stg28/models/awning/cave_awning_side_flame_polygons.inc"
};

static Sint32 cave_awning_side_flame_vertices[19] = {
#include "stg28/models/awning/cave_awning_side_flame_vertices.inc"
};

static NJS_CNK_MODEL cave_awning_side_flame_model[1] = {
#include "stg28/models/awning/cave_awning_side_flame_model.inc"
};

static NJS_CNK_OBJECT cave_awning_side_flame_object = {
#include "stg28/models/awning/cave_awning_side_flame_object.inc"
};

static NJS_TEXNAME cave_awning_front_flame_texture_names[1] = {
#include "stg28/models/awning/cave_awning_front_flame_texture_names.inc"
};

static NJS_TEXLIST cave_awning_front_flame_texlist = {
#include "stg28/models/awning/cave_awning_front_flame_texlist.inc"
};

static Sint16 cave_awning_front_flame_polygons[25] = {
#include "stg28/models/awning/cave_awning_front_flame_polygons.inc"
};

static Sint32 cave_awning_front_flame_vertices[19] = {
#include "stg28/models/awning/cave_awning_front_flame_vertices.inc"
};

static NJS_CNK_MODEL cave_awning_front_flame_model[1] = {
#include "stg28/models/awning/cave_awning_front_flame_model.inc"
};

static NJS_CNK_OBJECT cave_awning_front_flame_object = {
#include "stg28/models/awning/cave_awning_front_flame_object.inc"
};

static NJS_TEXNAME cave_awning_texture_names[5] = {
#include "stg28/models/awning/cave_awning_texture_names.inc"
};

static NJS_TEXLIST cave_awning_texlist = {
#include "stg28/models/awning/cave_awning_texlist.inc"
};

static Sint16 cave_awning_polygons[319] = {
#include "stg28/models/awning/cave_awning_polygons.inc"
};

static Sint32 cave_awning_vertices[243] = {
#include "stg28/models/awning/cave_awning_vertices.inc"
};

static NJS_CNK_MODEL cave_awning_model = {
#include "stg28/models/awning/cave_awning_model.inc"
};

static NJS_CNK_OBJECT cave_awning_object = {
#include "stg28/models/awning/cave_awning_object.inc"
};

static NJS_MATERIAL cave_awning_collision_materials[1] = {
#include "stg28/models/awning/cave_awning_collision_materials.inc"
};

static Sint16 cave_awning_collision_indices[66] = {
#include "stg28/models/awning/cave_awning_collision_indices.inc"
};

static NJS_MESHSET cave_awning_collision_meshsets[1] = {
#include "stg28/models/awning/cave_awning_collision_meshsets.inc"
};

static NJS_POINT3 cave_awning_collision_points[16] = {
#include "stg28/models/awning/cave_awning_collision_points.inc"
};

static NJS_VECTOR cave_awning_collision_normals[16] = {
#include "stg28/models/awning/cave_awning_collision_normals.inc"
};

static NJS_MODEL cave_awning_collision_model = {
#include "stg28/models/awning/cave_awning_collision_model.inc"
};

static NJS_OBJECT cave_awning_collision = {
#include "stg28/models/awning/cave_awning_collision.inc"
};

#endif
