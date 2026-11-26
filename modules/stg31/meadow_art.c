/* the models of the palm, the flowers and the grass, and the rock's collision
 * object. no code reads the palm's, the flowers' or the grass's collision
 * object, or green_hill_turning_flower_bounds */
#include <ninja.h>
#include <sg_xpt.h>

#include "stg31/prop_art.h"

static Sint16 palm_polygons[379] = {
#include "stg31/meadow_art/palm_polygons.inc"
};

static Sint32 palm_vertices[575] = {
#include "stg31/meadow_art/palm_vertices.inc"
};

static NJS_CNK_MODEL palm_model = {
#include "stg31/meadow_art/palm_model.inc"
};

NJS_CNK_OBJECT green_hill_palm_object[1] = {
#include "stg31/meadow_art/green_hill_palm_object.inc"
};

static NJS_MATERIAL palm_collision_materials[1] = {
#include "stg31/meadow_art/palm_collision_materials.inc"
};

static Sint16 palm_collision_faces[30] = {
#include "stg31/meadow_art/palm_collision_faces.inc"
};

static NJS_MESHSET palm_collision_meshsets[1] = {
#include "stg31/meadow_art/palm_collision_meshsets.inc"
};

static NJS_POINT3 palm_collision_points[16] = {
#include "stg31/meadow_art/palm_collision_points.inc"
};

static NJS_VECTOR palm_collision_normals[16] = {
#include "stg31/meadow_art/palm_collision_normals.inc"
};

static NJS_MODEL palm_collision_model = {
#include "stg31/meadow_art/palm_collision_model.inc"
};

NJS_OBJECT green_hill_palm_collision = {
#include "stg31/meadow_art/green_hill_palm_collision.inc"
};

static Sint16 turning_flower_a_head_polygons[213] = {
#include "stg31/meadow_art/turning_flower_a_head_polygons.inc"
};

static Sint32 turning_flower_a_head_vertices[467] = {
#include "stg31/meadow_art/turning_flower_a_head_vertices.inc"
};

static NJS_CNK_MODEL turning_flower_a_head_model = {
#include "stg31/meadow_art/turning_flower_a_head_model.inc"
};

static NJS_CNK_OBJECT turning_flower_a_head = {
#include "stg31/meadow_art/turning_flower_a_head.inc"
};

static Sint16 turning_flower_a_polygons[83] = {
#include "stg31/meadow_art/turning_flower_a_polygons.inc"
};

static Sint32 turning_flower_a_vertices[215] = {
#include "stg31/meadow_art/turning_flower_a_vertices.inc"
};

static NJS_CNK_MODEL turning_flower_a_model = {
#include "stg31/meadow_art/turning_flower_a_model.inc"
};

NJS_CNK_OBJECT green_hill_turning_flower_a = {
#include "stg31/meadow_art/green_hill_turning_flower_a.inc"
};

static Sint16 turning_flower_b_head_polygons[213] = {
#include "stg31/meadow_art/turning_flower_b_head_polygons.inc"
};

static Sint32 turning_flower_b_head_vertices[467] = {
#include "stg31/meadow_art/turning_flower_b_head_vertices.inc"
};

static NJS_CNK_MODEL turning_flower_b_head_model = {
#include "stg31/meadow_art/turning_flower_b_head_model.inc"
};

static NJS_CNK_OBJECT turning_flower_b_head = {
#include "stg31/meadow_art/turning_flower_b_head.inc"
};

static Sint16 turning_flower_b_polygons[115] = {
#include "stg31/meadow_art/turning_flower_b_polygons.inc"
};

static Sint32 turning_flower_b_vertices[311] = {
#include "stg31/meadow_art/turning_flower_b_vertices.inc"
};

static NJS_CNK_MODEL turning_flower_b_model = {
#include "stg31/meadow_art/turning_flower_b_model.inc"
};

NJS_CNK_OBJECT green_hill_turning_flower_b = {
#include "stg31/meadow_art/green_hill_turning_flower_b.inc"
};

static Sint16 turning_flower_bounds_polygons[29] = {
#include "stg31/meadow_art/turning_flower_bounds_polygons.inc"
};

static Sint32 turning_flower_bounds_vertices[35] = {
#include "stg31/meadow_art/turning_flower_bounds_vertices.inc"
};

static NJS_CNK_MODEL turning_flower_bounds_model = {
#include "stg31/meadow_art/turning_flower_bounds_model.inc"
};

NJS_CNK_OBJECT green_hill_turning_flower_bounds = {
#include "stg31/meadow_art/green_hill_turning_flower_bounds.inc"
};

static Sint16 bobbing_flower_head_polygons[135] = {
#include "stg31/meadow_art/bobbing_flower_head_polygons.inc"
};

static Sint32 bobbing_flower_head_vertices[363] = {
#include "stg31/meadow_art/bobbing_flower_head_vertices.inc"
};

static NJS_CNK_MODEL bobbing_flower_head_model = {
#include "stg31/meadow_art/bobbing_flower_head_model.inc"
};

static NJS_CNK_OBJECT bobbing_flower_head = {
#include "stg31/meadow_art/bobbing_flower_head.inc"
};

static Sint16 bobbing_flower_polygons[59] = {
#include "stg31/meadow_art/bobbing_flower_polygons.inc"
};

static Sint32 bobbing_flower_vertices[135] = {
#include "stg31/meadow_art/bobbing_flower_vertices.inc"
};

static NJS_CNK_MODEL bobbing_flower_model = {
#include "stg31/meadow_art/bobbing_flower_model.inc"
};

NJS_CNK_OBJECT green_hill_bobbing_flower = {
#include "stg31/meadow_art/green_hill_bobbing_flower.inc"
};

static NJS_MATERIAL flower_collision_materials[1] = {
#include "stg31/meadow_art/flower_collision_materials.inc"
};

static Sint16 flower_collision_faces[19] = {
#include "stg31/meadow_art/flower_collision_faces.inc"
};

static NJS_MESHSET flower_collision_meshsets[1] = {
#include "stg31/meadow_art/flower_collision_meshsets.inc"
};

static NJS_POINT3 flower_collision_points[8] = {
#include "stg31/meadow_art/flower_collision_points.inc"
};

static NJS_VECTOR flower_collision_normals[8] = {
#include "stg31/meadow_art/flower_collision_normals.inc"
};

static NJS_MODEL flower_collision_model = {
#include "stg31/meadow_art/flower_collision_model.inc"
};

NJS_OBJECT green_hill_flower_collision = {
#include "stg31/meadow_art/green_hill_flower_collision.inc"
};

static Sint16 grass_polygons[147] = {
#include "stg31/meadow_art/grass_polygons.inc"
};

static Sint32 grass_vertices[399] = {
#include "stg31/meadow_art/grass_vertices.inc"
};

static NJS_CNK_MODEL grass_model = {
#include "stg31/meadow_art/grass_model.inc"
};

NJS_CNK_OBJECT green_hill_grass_object[1] = {
#include "stg31/meadow_art/green_hill_grass_object.inc"
};

static NJS_MATERIAL grass_collision_materials[1] = {
#include "stg31/meadow_art/grass_collision_materials.inc"
};

static Sint16 grass_collision_faces[16] = {
#include "stg31/meadow_art/grass_collision_faces.inc"
};

static NJS_MESHSET grass_collision_meshsets[1] = {
#include "stg31/meadow_art/grass_collision_meshsets.inc"
};

static NJS_POINT3 grass_collision_points[8] = {
#include "stg31/meadow_art/grass_collision_points.inc"
};

static NJS_VECTOR grass_collision_normals[8] = {
#include "stg31/meadow_art/grass_collision_normals.inc"
};

static NJS_MODEL grass_collision_model = {
#include "stg31/meadow_art/grass_collision_model.inc"
};

NJS_OBJECT green_hill_grass_collision = {
#include "stg31/meadow_art/green_hill_grass_collision.inc"
};

static NJS_MATERIAL rock_collision_materials[1] = {
#include "stg31/meadow_art/rock_collision_materials.inc"
};

static Sint16 rock_collision_faces[19] = {
#include "stg31/meadow_art/rock_collision_faces.inc"
};

static NJS_MESHSET rock_collision_meshsets[1] = {
#include "stg31/meadow_art/rock_collision_meshsets.inc"
};

static NJS_POINT3 rock_collision_points[8] = {
#include "stg31/meadow_art/rock_collision_points.inc"
};

static NJS_VECTOR rock_collision_normals[8] = {
#include "stg31/meadow_art/rock_collision_normals.inc"
};

static NJS_MODEL rock_collision_model = {
#include "stg31/meadow_art/rock_collision_model.inc"
};

NJS_OBJECT green_hill_rock_collision = {
#include "stg31/meadow_art/green_hill_rock_collision.inc"
};
