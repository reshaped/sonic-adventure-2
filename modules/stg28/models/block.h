/* Pyramid Cave rising stone block and solid sides. */
#ifndef STG28_MODELS_BLOCK_H
#define STG28_MODELS_BLOCK_H

#include <ninja.h>
#include <sg_xpt.h>

static NJS_TEXNAME cave_block_texture_names[3] = {
#include "stg28/models/block/cave_block_texture_names.inc"
};

static NJS_TEXLIST cave_block_texlist = {
#include "stg28/models/block/cave_block_texlist.inc"
};

static Sint16 cave_block_polygons[525] = {
#include "stg28/models/block/cave_block_polygons.inc"
};

static Sint32 cave_block_vertices[307] = {
#include "stg28/models/block/cave_block_vertices.inc"
};

static NJS_CNK_MODEL cave_block_model = {
#include "stg28/models/block/cave_block_model.inc"
};

static NJS_CNK_OBJECT cave_block_object = {
#include "stg28/models/block/cave_block_object.inc"
};

static NJS_MATERIAL cave_block_collision_materials[1] = {
#include "stg28/models/block/cave_block_collision_materials.inc"
};

static Sint16 cave_block_collision_indices[19] = {
#include "stg28/models/block/cave_block_collision_indices.inc"
};

static NJS_MESHSET cave_block_collision_meshsets[1] = {
#include "stg28/models/block/cave_block_collision_meshsets.inc"
};

static NJS_POINT3 cave_block_collision_points[8] = {
#include "stg28/models/block/cave_block_collision_points.inc"
};

static NJS_VECTOR cave_block_collision_normals[8] = {
#include "stg28/models/block/cave_block_collision_normals.inc"
};

static NJS_MODEL cave_block_collision_model = {
#include "stg28/models/block/cave_block_collision_model.inc"
};

static NJS_OBJECT cave_block_collision = {
#include "stg28/models/block/cave_block_collision.inc"
};

#endif
