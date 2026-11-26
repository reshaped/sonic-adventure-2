/* Pyramid Cave windmill rotor, base and collision mesh. */
#ifndef STG28_MODELS_WINDMILL_H
#define STG28_MODELS_WINDMILL_H

#include <ninja.h>
#include <sg_xpt.h>

static NJS_TEXNAME cave_windmill_rotor_texture_names[6] = {
#include "stg28/models/windmill/cave_windmill_rotor_texture_names.inc"
};

static NJS_TEXLIST cave_windmill_rotor_texlist = {
#include "stg28/models/windmill/cave_windmill_rotor_texlist.inc"
};

static Sint16 cave_windmill_rotor_polygons[1553] = {
#include "stg28/models/windmill/cave_windmill_rotor_polygons.inc"
};

static Sint32 cave_windmill_rotor_vertices[2535] = {
#include "stg28/models/windmill/cave_windmill_rotor_vertices.inc"
};

static NJS_CNK_MODEL cave_windmill_rotor_model = {
#include "stg28/models/windmill/cave_windmill_rotor_model.inc"
};

static NJS_CNK_OBJECT cave_windmill_rotor_object = {
#include "stg28/models/windmill/cave_windmill_rotor_object.inc"
};

static NJS_TEXNAME cave_windmill_base_texture_names[3] = {
#include "stg28/models/windmill/cave_windmill_base_texture_names.inc"
};

static NJS_TEXLIST cave_windmill_base_texlist = {
#include "stg28/models/windmill/cave_windmill_base_texlist.inc"
};

static Sint16 cave_windmill_base_polygons[379] = {
#include "stg28/models/windmill/cave_windmill_base_polygons.inc"
};

static Sint32 cave_windmill_base_vertices[291] = {
#include "stg28/models/windmill/cave_windmill_base_vertices.inc"
};

static NJS_CNK_MODEL cave_windmill_base_model = {
#include "stg28/models/windmill/cave_windmill_base_model.inc"
};

static NJS_CNK_OBJECT cave_windmill_base_object = {
#include "stg28/models/windmill/cave_windmill_base_object.inc"
};

static NJS_MATERIAL cave_windmill_rotor_collision_materials[1] = {
#include "stg28/models/windmill/cave_windmill_rotor_collision_materials.inc"
};

static Sint16 cave_windmill_rotor_collision_indices[87] = {
#include "stg28/models/windmill/cave_windmill_rotor_collision_indices.inc"
};

static NJS_MESHSET cave_windmill_rotor_collision_meshsets[1] = {
#include "stg28/models/windmill/cave_windmill_rotor_collision_meshsets.inc"
};

static NJS_POINT3 cave_windmill_rotor_collision_points[36] = {
#include "stg28/models/windmill/cave_windmill_rotor_collision_points.inc"
};

static NJS_VECTOR cave_windmill_rotor_collision_normals[36] = {
#include "stg28/models/windmill/cave_windmill_rotor_collision_normals.inc"
};

static NJS_MODEL cave_windmill_rotor_collision_model = {
#include "stg28/models/windmill/cave_windmill_rotor_collision_model.inc"
};

static NJS_OBJECT cave_windmill_rotor_collision = {
#include "stg28/models/windmill/cave_windmill_rotor_collision.inc"
};

#endif
