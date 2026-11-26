/* Pyramid Cave moving eye, light and solid wall. */
#ifndef STG28_MODELS_EYE_H
#define STG28_MODELS_EYE_H

#include <ninja.h>
#include <sg_xpt.h>

static NJS_TEXNAME cave_eye_texture_names[2] = {
#include "stg28/models/eye/cave_eye_texture_names.inc"
};

static NJS_TEXLIST cave_eye_texlist = {
#include "stg28/models/eye/cave_eye_texlist.inc"
};

static Sint16 cave_eye_polygons[491] = {
#include "stg28/models/eye/cave_eye_polygons.inc"
};

static Sint32 cave_eye_vertices[287] = {
#include "stg28/models/eye/cave_eye_vertices.inc"
};

static NJS_CNK_MODEL cave_eye_model = {
#include "stg28/models/eye/cave_eye_model.inc"
};

static NJS_CNK_OBJECT cave_eye_object = {
#include "stg28/models/eye/cave_eye_object.inc"
};

static NJS_TEXNAME cave_eye_light_texture_names[1] = {
#include "stg28/models/eye/cave_eye_light_texture_names.inc"
};

static NJS_TEXLIST cave_eye_light_texlist = {
#include "stg28/models/eye/cave_eye_light_texlist.inc"
};

static Sint16 cave_eye_light_polygons[25] = {
#include "stg28/models/eye/cave_eye_light_polygons.inc"
};

static Sint32 cave_eye_light_vertices[19] = {
#include "stg28/models/eye/cave_eye_light_vertices.inc"
};

static NJS_CNK_MODEL cave_eye_light_model = {
#include "stg28/models/eye/cave_eye_light_model.inc"
};

static NJS_CNK_OBJECT cave_eye_light_object = {
#include "stg28/models/eye/cave_eye_light_object.inc"
};

static NJS_MATERIAL cave_eye_collision_materials[1] = {
#include "stg28/models/eye/cave_eye_collision_materials.inc"
};

static Sint16 cave_eye_collision_indices[16] = {
#include "stg28/models/eye/cave_eye_collision_indices.inc"
};

static NJS_MESHSET cave_eye_collision_meshsets[1] = {
#include "stg28/models/eye/cave_eye_collision_meshsets.inc"
};

static NJS_POINT3 cave_eye_collision_points[5] = {
#include "stg28/models/eye/cave_eye_collision_points.inc"
};

static NJS_VECTOR cave_eye_collision_normals[5] = {
#include "stg28/models/eye/cave_eye_collision_normals.inc"
};

static NJS_MODEL cave_eye_collision_model = {
#include "stg28/models/eye/cave_eye_collision_model.inc"
};

static NJS_OBJECT cave_eye_collision = {
#include "stg28/models/eye/cave_eye_collision.inc"
};

#endif
