/* Pyramid Cave firepot, flame and solid base. */
#ifndef STG28_MODELS_FIREPOT_H
#define STG28_MODELS_FIREPOT_H

#include <ninja.h>
#include <sg_xpt.h>

static NJS_TEXNAME cave_firepot_flame_texture_names[1] = {
#include "stg28/models/firepot/cave_firepot_flame_texture_names.inc"
};

static NJS_TEXLIST cave_firepot_flame_texlist = {
#include "stg28/models/firepot/cave_firepot_flame_texlist.inc"
};

static Sint16 cave_firepot_flame_polygons[25] = {
#include "stg28/models/firepot/cave_firepot_flame_polygons.inc"
};

static Sint32 cave_firepot_flame_vertices[19] = {
#include "stg28/models/firepot/cave_firepot_flame_vertices.inc"
};

static NJS_CNK_MODEL cave_firepot_flame_model = {
#include "stg28/models/firepot/cave_firepot_flame_model.inc"
};

static NJS_CNK_OBJECT cave_firepot_flame_object = {
#include "stg28/models/firepot/cave_firepot_flame_object.inc"
};

static NJS_TEXNAME cave_firepot_texture_names[4] = {
#include "stg28/models/firepot/cave_firepot_texture_names.inc"
};

static NJS_TEXLIST cave_firepot_texlist = {
#include "stg28/models/firepot/cave_firepot_texlist.inc"
};

static Sint16 cave_firepot_polygons[937] = {
#include "stg28/models/firepot/cave_firepot_polygons.inc"
};

static Sint32 cave_firepot_vertices[811] = {
#include "stg28/models/firepot/cave_firepot_vertices.inc"
};

static NJS_CNK_MODEL cave_firepot_model = {
#include "stg28/models/firepot/cave_firepot_model.inc"
};

static NJS_CNK_OBJECT cave_firepot_object = {
#include "stg28/models/firepot/cave_firepot_object.inc"
};

static NJS_MATERIAL cave_firepot_collision_materials[1] = {
#include "stg28/models/firepot/cave_firepot_collision_materials.inc"
};

static Sint16 cave_firepot_collision_indices[193] = {
#include "stg28/models/firepot/cave_firepot_collision_indices.inc"
};

static NJS_MESHSET cave_firepot_collision_meshsets[1] = {
#include "stg28/models/firepot/cave_firepot_collision_meshsets.inc"
};

static NJS_POINT3 cave_firepot_collision_points[48] = {
#include "stg28/models/firepot/cave_firepot_collision_points.inc"
};

static NJS_VECTOR cave_firepot_collision_normals[48] = {
#include "stg28/models/firepot/cave_firepot_collision_normals.inc"
};

static NJS_MODEL cave_firepot_collision_model = {
#include "stg28/models/firepot/cave_firepot_collision_model.inc"
};

static NJS_OBJECT cave_firepot_collision = {
#include "stg28/models/firepot/cave_firepot_collision.inc"
};

#endif
