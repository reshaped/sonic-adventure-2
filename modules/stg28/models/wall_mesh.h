#ifndef STG28_WALL_MESH_H
#define STG28_WALL_MESH_H

#include <ninja.h>
#include <sg_xpt.h>

static NJS_TEXNAME cave_wall_texture_names_a[7] = {
#include "stg28/models/wall_mesh/cave_wall_texture_names_a.inc"
};

static NJS_TEXLIST cave_wall_texlist_a = {
#include "stg28/models/wall_mesh/cave_wall_texlist_a.inc"
};

static Sint16 cave_wall_model_a_polygons[1633] = {
#include "stg28/models/wall_mesh/cave_wall_model_a_polygons.inc"
};

static Sint32 cave_wall_model_a_vertices[1743] = {
#include "stg28/models/wall_mesh/cave_wall_model_a_vertices.inc"
};

static NJS_CNK_MODEL cave_wall_model_a = {
#include "stg28/models/wall_mesh/cave_wall_model_a.inc"
};

static NJS_CNK_OBJECT cave_wall_a_object = {
#include "stg28/models/wall_mesh/cave_wall_a_object.inc"
};

static Sint16 cave_wall_pulse_model_a_polygons[65] = {
#include "stg28/models/wall_mesh/cave_wall_pulse_model_a_polygons.inc"
};

static Sint32 cave_wall_pulse_model_a_vertices[67] = {
#include "stg28/models/wall_mesh/cave_wall_pulse_model_a_vertices.inc"
};

static NJS_CNK_MODEL cave_wall_pulse_model_a = {
#include "stg28/models/wall_mesh/cave_wall_pulse_model_a.inc"
};

static NJS_CNK_OBJECT cave_wall_pulse_a_object = {
#include "stg28/models/wall_mesh/cave_wall_pulse_a_object.inc"
};

static NJS_CNK_OBJECT cave_wall_pulse_a_assembly = {
#include "stg28/models/wall_mesh/cave_wall_pulse_a_assembly.inc"
};

static NJS_TEXNAME cave_wall_texture_names_b[7] = {
#include "stg28/models/wall_mesh/cave_wall_texture_names_b.inc"
};

static NJS_TEXLIST cave_wall_texlist_b = {
#include "stg28/models/wall_mesh/cave_wall_texlist_b.inc"
};

static Sint16 cave_wall_model_b_polygons[1633] = {
#include "stg28/models/wall_mesh/cave_wall_model_b_polygons.inc"
};

static Sint32 cave_wall_model_b_vertices[1691] = {
#include "stg28/models/wall_mesh/cave_wall_model_b_vertices.inc"
};

static NJS_CNK_MODEL cave_wall_model_b = {
#include "stg28/models/wall_mesh/cave_wall_model_b.inc"
};

static NJS_CNK_OBJECT cave_wall_b_object = {
#include "stg28/models/wall_mesh/cave_wall_b_object.inc"
};

static Sint16 cave_wall_pulse_model_b_polygons[65] = {
#include "stg28/models/wall_mesh/cave_wall_pulse_model_b_polygons.inc"
};

static Sint32 cave_wall_pulse_model_b_vertices[67] = {
#include "stg28/models/wall_mesh/cave_wall_pulse_model_b_vertices.inc"
};

static NJS_CNK_MODEL cave_wall_pulse_model_b = {
#include "stg28/models/wall_mesh/cave_wall_pulse_model_b.inc"
};

static NJS_CNK_OBJECT cave_wall_pulse_b_object = {
#include "stg28/models/wall_mesh/cave_wall_pulse_b_object.inc"
};

static NJS_CNK_OBJECT cave_wall_pulse_b_assembly = {
#include "stg28/models/wall_mesh/cave_wall_pulse_b_assembly.inc"
};

static NJS_MATERIAL cave_wall_collision_materials[1] = {
#include "stg28/models/wall_mesh/cave_wall_collision_materials.inc"
};

static Sint16 cave_wall_collision_indices[56] = {
#include "stg28/models/wall_mesh/cave_wall_collision_indices.inc"
};

static NJS_MESHSET cave_wall_collision_meshsets[1] = {
#include "stg28/models/wall_mesh/cave_wall_collision_meshsets.inc"
};

static NJS_POINT3 cave_wall_collision_points[24] = {
#include "stg28/models/wall_mesh/cave_wall_collision_points.inc"
};

static NJS_VECTOR cave_wall_collision_normals[24] = {
#include "stg28/models/wall_mesh/cave_wall_collision_normals.inc"
};

static NJS_MODEL cave_wall_collision_model = {
#include "stg28/models/wall_mesh/cave_wall_collision_model.inc"
};

static NJS_OBJECT cave_wall_collision = {
#include "stg28/models/wall_mesh/cave_wall_collision.inc"
};

#endif
