#include "stg30/frstage_textures/text.inc"
#include <ninja.h>
#include <sg_xpt.h>

#include "shared/frstage.h"
#include "shared/orbital/platform.h"

static NJS_TEXNAME frstage_textures[10] = {
#include "stg30/frstage_textures/frstage_textures.inc"
};
NJS_TEXLIST frstage_texlist = {
#include "stg30/frstage_textures/frstage_texlist.inc"
};

static Sint16 frstage_fence_3_polygons[325] = {
#include "stg30/frstage_textures/frstage_fence_3_polygons.inc"
};

static Sint32 frstage_fence_3_vertices[335] = {
#include "stg30/frstage_textures/frstage_fence_3_vertices.inc"
};

static NJS_CNK_MODEL frstage_fence_3_mesh = {
#include "stg30/frstage_textures/frstage_fence_3_mesh.inc"
};

NJS_CNK_OBJECT frstage_fence_3 = {
#include "stg30/frstage_textures/frstage_fence_3.inc"
};

static Sint16 frstage_fence_2_polygons[325] = {
#include "stg30/frstage_textures/frstage_fence_2_polygons.inc"
};

static Sint32 frstage_fence_2_vertices[335] = {
#include "stg30/frstage_textures/frstage_fence_2_vertices.inc"
};

static NJS_CNK_MODEL frstage_fence_2_mesh = {
#include "stg30/frstage_textures/frstage_fence_2_mesh.inc"
};

NJS_CNK_OBJECT frstage_fence_2 = {
#include "stg30/frstage_textures/frstage_fence_2.inc"
};

static Sint16 frstage_fence_1_polygons[325] = {
#include "stg30/frstage_textures/frstage_fence_1_polygons.inc"
};

static Sint32 frstage_fence_1_vertices[335] = {
#include "stg30/frstage_textures/frstage_fence_1_vertices.inc"
};

static NJS_CNK_MODEL frstage_fence_1_mesh = {
#include "stg30/frstage_textures/frstage_fence_1_mesh.inc"
};

NJS_CNK_OBJECT frstage_fence_1 = {
#include "stg30/frstage_textures/frstage_fence_1.inc"
};

static Sint16 frstage_fence_0_polygons[325] = {
#include "stg30/frstage_textures/frstage_fence_0_polygons.inc"
};

static Sint32 frstage_fence_0_vertices[335] = {
#include "stg30/frstage_textures/frstage_fence_0_vertices.inc"
};

static NJS_CNK_MODEL frstage_fence_0_mesh = {
#include "stg30/frstage_textures/frstage_fence_0_mesh.inc"
};

NJS_CNK_OBJECT frstage_fence_0[1] = {
#include "stg30/frstage_textures/frstage_fence_0.inc"
};

static Sint16 frstage_light_polygons[67] = {
#include "stg30/frstage_textures/frstage_light_polygons.inc"
};

static Sint32 frstage_light_vertices[67] = {
#include "stg30/frstage_textures/frstage_light_vertices.inc"
};

static NJS_CNK_MODEL frstage_light_mesh = {
#include "stg30/frstage_textures/frstage_light_mesh.inc"
};

NJS_CNK_OBJECT frstage_light[1] = {
#include "stg30/frstage_textures/frstage_light.inc"
};

static Sint16 frstage_floor_polygons[347] = {
#include "stg30/frstage_textures/frstage_floor_polygons.inc"
};

static Sint32 frstage_floor_vertices[323] = {
#include "stg30/frstage_textures/frstage_floor_vertices.inc"
};

static NJS_CNK_MODEL frstage_floor_mesh = {
#include "stg30/frstage_textures/frstage_floor_mesh.inc"
};

NJS_CNK_OBJECT frstage_floor[1] = {
#include "stg30/frstage_textures/frstage_floor.inc"
};

static NJS_MATERIAL frstage_fence_collision_3_materials[1] = {
#include "stg30/frstage_textures/frstage_fence_collision_3_materials.inc"
};

static Sint16 frstage_fence_collision_3_indices_0[16] = {
#include "stg30/frstage_textures/frstage_fence_collision_3_indices_0.inc"
};

static NJS_MESHSET frstage_fence_collision_3_meshsets[1] = {
#include "stg30/frstage_textures/frstage_fence_collision_3_meshsets.inc"
};

static NJS_POINT3 frstage_fence_collision_3_points[8] = {
#include "stg30/frstage_textures/frstage_fence_collision_3_points.inc"
};

static NJS_VECTOR frstage_fence_collision_3_normals[8] = {
#include "stg30/frstage_textures/frstage_fence_collision_3_normals.inc"
};

static NJS_MODEL frstage_fence_collision_3_model = {
#include "stg30/frstage_textures/frstage_fence_collision_3_model.inc"
};

NJS_OBJECT frstage_fence_collision_3 = {
#include "stg30/frstage_textures/frstage_fence_collision_3.inc"
};

static NJS_MATERIAL frstage_fence_collision_2_materials[1] = {
#include "stg30/frstage_textures/frstage_fence_collision_2_materials.inc"
};

static Sint16 frstage_fence_collision_2_indices_0[16] = {
#include "stg30/frstage_textures/frstage_fence_collision_2_indices_0.inc"
};

static NJS_MESHSET frstage_fence_collision_2_meshsets[1] = {
#include "stg30/frstage_textures/frstage_fence_collision_2_meshsets.inc"
};

static NJS_POINT3 frstage_fence_collision_2_points[8] = {
#include "stg30/frstage_textures/frstage_fence_collision_2_points.inc"
};

static NJS_VECTOR frstage_fence_collision_2_normals[8] = {
#include "stg30/frstage_textures/frstage_fence_collision_2_normals.inc"
};

static NJS_MODEL frstage_fence_collision_2_model = {
#include "stg30/frstage_textures/frstage_fence_collision_2_model.inc"
};

NJS_OBJECT frstage_fence_collision_2 = {
#include "stg30/frstage_textures/frstage_fence_collision_2.inc"
};

static NJS_MATERIAL frstage_fence_collision_1_materials[1] = {
#include "stg30/frstage_textures/frstage_fence_collision_1_materials.inc"
};

static Sint16 frstage_fence_collision_1_indices_0[16] = {
#include "stg30/frstage_textures/frstage_fence_collision_1_indices_0.inc"
};

static NJS_MESHSET frstage_fence_collision_1_meshsets[1] = {
#include "stg30/frstage_textures/frstage_fence_collision_1_meshsets.inc"
};

static NJS_POINT3 frstage_fence_collision_1_points[8] = {
#include "stg30/frstage_textures/frstage_fence_collision_1_points.inc"
};

static NJS_VECTOR frstage_fence_collision_1_normals[8] = {
#include "stg30/frstage_textures/frstage_fence_collision_1_normals.inc"
};

static NJS_MODEL frstage_fence_collision_1_model = {
#include "stg30/frstage_textures/frstage_fence_collision_1_model.inc"
};

NJS_OBJECT frstage_fence_collision_1 = {
#include "stg30/frstage_textures/frstage_fence_collision_1.inc"
};

static NJS_MATERIAL frstage_fence_collision_0_materials[1] = {
#include "stg30/frstage_textures/frstage_fence_collision_0_materials.inc"
};

static Sint16 frstage_fence_collision_0_indices_0[16] = {
#include "stg30/frstage_textures/frstage_fence_collision_0_indices_0.inc"
};

static NJS_MESHSET frstage_fence_collision_0_meshsets[1] = {
#include "stg30/frstage_textures/frstage_fence_collision_0_meshsets.inc"
};

static NJS_POINT3 frstage_fence_collision_0_points[8] = {
#include "stg30/frstage_textures/frstage_fence_collision_0_points.inc"
};

static NJS_VECTOR frstage_fence_collision_0_normals[8] = {
#include "stg30/frstage_textures/frstage_fence_collision_0_normals.inc"
};

static NJS_MODEL frstage_fence_collision_0_model = {
#include "stg30/frstage_textures/frstage_fence_collision_0_model.inc"
};

NJS_OBJECT frstage_fence_collision_0[1] = {
#include "stg30/frstage_textures/frstage_fence_collision_0.inc"
};

static NJS_MATERIAL frstage_collision_materials[1] = {
#include "stg30/frstage_textures/frstage_collision_materials.inc"
};

static Sint16 frstage_collision_indices_0[21] = {
#include "stg30/frstage_textures/frstage_collision_indices_0.inc"
};

static NJS_MESHSET frstage_collision_meshsets[1] = {
#include "stg30/frstage_textures/frstage_collision_meshsets.inc"
};

static NJS_POINT3 frstage_collision_points[8] = {
#include "stg30/frstage_textures/frstage_collision_points.inc"
};

static NJS_VECTOR frstage_collision_normals[8] = {
#include "stg30/frstage_textures/frstage_collision_normals.inc"
};

static NJS_MODEL frstage_collision_model = {
#include "stg30/frstage_textures/frstage_collision_model.inc"
};

NJS_OBJECT frstage_collision = {
#include "stg30/frstage_textures/frstage_collision.inc"
};
