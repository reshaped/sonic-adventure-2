#include <ninja.h>
#include <sg_xpt.h>

#include "stg21/pillar.h"

static const char material_sekihi1[16] = {
#include "stg21/ocean_pillar_art/material_sekihi1.inc"
};

static const char material_sekihi2[16] = {
#include "stg21/ocean_pillar_art/material_sekihi2.inc"
};

static const char material_sekihi3[16] = {
#include "stg21/ocean_pillar_art/material_sekihi3.inc"
};

static const char material_hasira5[16] = {
#include "stg21/ocean_pillar_art/material_hasira5.inc"
};

static const char material_kabe2[12] = {
#include "stg21/ocean_pillar_art/material_kabe2.inc"
};

static const char material_sekihi4[16] = {
#include "stg21/ocean_pillar_art/material_sekihi4.inc"
};

static const char material_sekihi5[14] = {
#include "stg21/ocean_pillar_art/material_sekihi5.inc"
};

static NJS_TEXNAME pillar_shaft_textures[3] = {
#include "stg21/ocean_pillar_art/pillar_shaft_textures.inc"
};

NJS_TEXLIST pillar_shaft_texlist = {
#include "stg21/ocean_pillar_art/pillar_shaft_texlist.inc"
};

static Sint16 pillar_shaft_polygons[121] = {
#include "stg21/ocean_pillar_art/pillar_shaft_polygons.inc"
};

static Sint32 pillar_shaft_vertices[115] = {
#include "stg21/ocean_pillar_art/pillar_shaft_vertices.inc"
};

NJS_CNK_MODEL pillar_shaft = {
#include "stg21/ocean_pillar_art/pillar_shaft.inc"
};

static NJS_CNK_OBJECT pillar_shaft_object = {
#include "stg21/ocean_pillar_art/pillar_shaft_object.inc"
};

static NJS_TEXNAME pillar_whole_base_textures[2] = {
#include "stg21/ocean_pillar_art/pillar_whole_base_textures.inc"
};

NJS_TEXLIST pillar_whole_base_texlist = {
#include "stg21/ocean_pillar_art/pillar_whole_base_texlist.inc"
};

static Sint16 pillar_whole_base_polygons[123] = {
#include "stg21/ocean_pillar_art/pillar_whole_base_polygons.inc"
};

static Sint32 pillar_whole_base_vertices[91] = {
#include "stg21/ocean_pillar_art/pillar_whole_base_vertices.inc"
};

NJS_CNK_MODEL pillar_whole_base = {
#include "stg21/ocean_pillar_art/pillar_whole_base.inc"
};

static NJS_CNK_OBJECT pillar_whole_base_object = {
#include "stg21/ocean_pillar_art/pillar_whole_base_object.inc"
};

static NJS_TEXNAME pillar_broken_base_textures[2] = {
#include "stg21/ocean_pillar_art/pillar_broken_base_textures.inc"
};

NJS_TEXLIST pillar_broken_base_texlist = {
#include "stg21/ocean_pillar_art/pillar_broken_base_texlist.inc"
};

static Sint16 pillar_broken_base_polygons[83] = {
#include "stg21/ocean_pillar_art/pillar_broken_base_polygons.inc"
};

static Sint32 pillar_broken_base_vertices[59] = {
#include "stg21/ocean_pillar_art/pillar_broken_base_vertices.inc"
};

NJS_CNK_MODEL pillar_broken_base = {
#include "stg21/ocean_pillar_art/pillar_broken_base.inc"
};

static NJS_CNK_OBJECT pillar_broken_base_object = {
#include "stg21/ocean_pillar_art/pillar_broken_base_object.inc"
};

static NJS_TEXNAME pillar_stone_textures[1] = {
#include "stg21/ocean_pillar_art/pillar_stone_textures.inc"
};

NJS_TEXLIST pillar_stone_texlist = {
#include "stg21/ocean_pillar_art/pillar_stone_texlist.inc"
};

static Sint16 pillar_stone_0_polygons[31] = {
#include "stg21/ocean_pillar_art/pillar_stone_0_polygons.inc"
};

static Sint32 pillar_stone_0_vertices[19] = {
#include "stg21/ocean_pillar_art/pillar_stone_0_vertices.inc"
};

NJS_CNK_MODEL pillar_stone_0 = {
#include "stg21/ocean_pillar_art/pillar_stone_0.inc"
};

static NJS_CNK_OBJECT pillar_stone_0_object = {
#include "stg21/ocean_pillar_art/pillar_stone_0_object.inc"
};

static NJS_TEXNAME pillar_material_4_textures[1] = {
#include "stg21/ocean_pillar_art/pillar_material_4_textures.inc"
};

static NJS_TEXLIST pillar_material_4_texlist = {
#include "stg21/ocean_pillar_art/pillar_material_4_texlist.inc"
};

static Sint16 pillar_stone_1_polygons[31] = {
#include "stg21/ocean_pillar_art/pillar_stone_1_polygons.inc"
};

static Sint32 pillar_stone_1_vertices[19] = {
#include "stg21/ocean_pillar_art/pillar_stone_1_vertices.inc"
};

NJS_CNK_MODEL pillar_stone_1 = {
#include "stg21/ocean_pillar_art/pillar_stone_1.inc"
};

static NJS_CNK_OBJECT pillar_stone_1_object = {
#include "stg21/ocean_pillar_art/pillar_stone_1_object.inc"
};

static NJS_TEXNAME pillar_material_5_textures[1] = {
#include "stg21/ocean_pillar_art/pillar_material_5_textures.inc"
};

static NJS_TEXLIST pillar_material_5_texlist = {
#include "stg21/ocean_pillar_art/pillar_material_5_texlist.inc"
};

static Sint16 pillar_stone_2_polygons[31] = {
#include "stg21/ocean_pillar_art/pillar_stone_2_polygons.inc"
};

static Sint32 pillar_stone_2_vertices[19] = {
#include "stg21/ocean_pillar_art/pillar_stone_2_vertices.inc"
};

NJS_CNK_MODEL pillar_stone_2 = {
#include "stg21/ocean_pillar_art/pillar_stone_2.inc"
};

static NJS_CNK_OBJECT pillar_stone_2_object = {
#include "stg21/ocean_pillar_art/pillar_stone_2_object.inc"
};

static NJS_MATERIAL pillar_upright_collision_materials[1] = {
#include "stg21/ocean_pillar_art/pillar_upright_collision_materials.inc"
};

static Sint16 pillar_upright_collision_faces[19] = {
#include "stg21/ocean_pillar_art/pillar_upright_collision_faces.inc"
};

static NJS_MESHSET pillar_upright_collision_meshsets[1] = {
#include "stg21/ocean_pillar_art/pillar_upright_collision_meshsets.inc"
};

static NJS_POINT3 pillar_upright_collision_points[8] = {
#include "stg21/ocean_pillar_art/pillar_upright_collision_points.inc"
};

static NJS_VECTOR pillar_upright_collision_normals[8] = {
#include "stg21/ocean_pillar_art/pillar_upright_collision_normals.inc"
};

static NJS_MODEL pillar_upright_collision_model = {
#include "stg21/ocean_pillar_art/pillar_upright_collision_model.inc"
};

NJS_OBJECT pillar_upright_collision[1] = {
#include "stg21/ocean_pillar_art/pillar_upright_collision.inc"
};

static NJS_MATERIAL pillar_side_collision_materials[1] = {
#include "stg21/ocean_pillar_art/pillar_side_collision_materials.inc"
};

static Sint16 pillar_side_collision_faces[78] = {
#include "stg21/ocean_pillar_art/pillar_side_collision_faces.inc"
};

static NJS_MESHSET pillar_side_collision_meshsets[1] = {
#include "stg21/ocean_pillar_art/pillar_side_collision_meshsets.inc"
};

static NJS_POINT3 pillar_side_collision_points[32] = {
#include "stg21/ocean_pillar_art/pillar_side_collision_points.inc"
};

static NJS_VECTOR pillar_side_collision_normals[32] = {
#include "stg21/ocean_pillar_art/pillar_side_collision_normals.inc"
};

static NJS_MODEL pillar_side_collision_model = {
#include "stg21/ocean_pillar_art/pillar_side_collision_model.inc"
};

static NJS_OBJECT pillar_side_collision = {
#include "stg21/ocean_pillar_art/pillar_side_collision.inc"
};

static NJS_MATERIAL pillar_fallen_collision_materials[1] = {
#include "stg21/ocean_pillar_art/pillar_fallen_collision_materials.inc"
};

static Sint16 pillar_fallen_collision_faces[62] = {
#include "stg21/ocean_pillar_art/pillar_fallen_collision_faces.inc"
};

static NJS_MESHSET pillar_fallen_collision_meshsets[1] = {
#include "stg21/ocean_pillar_art/pillar_fallen_collision_meshsets.inc"
};

static NJS_POINT3 pillar_fallen_collision_points[18] = {
#include "stg21/ocean_pillar_art/pillar_fallen_collision_points.inc"
};

static NJS_VECTOR pillar_fallen_collision_normals[18] = {
#include "stg21/ocean_pillar_art/pillar_fallen_collision_normals.inc"
};

static NJS_MODEL pillar_fallen_collision_model = {
#include "stg21/ocean_pillar_art/pillar_fallen_collision_model.inc"
};

NJS_OBJECT pillar_fallen_collision[1] = {
#include "stg21/ocean_pillar_art/pillar_fallen_collision.inc"
};

static NJS_TEXNAME pillar_rubble_textures[1] = {
#include "stg21/ocean_pillar_art/pillar_rubble_textures.inc"
};

NJS_TEXLIST pillar_rubble_texlist = {
#include "stg21/ocean_pillar_art/pillar_rubble_texlist.inc"
};

static Sint16 pillar_rubble_0_polygons[69] = {
#include "stg21/ocean_pillar_art/pillar_rubble_0_polygons.inc"
};

static Sint32 pillar_rubble_0_vertices[35] = {
#include "stg21/ocean_pillar_art/pillar_rubble_0_vertices.inc"
};

NJS_CNK_MODEL pillar_rubble_0 = {
#include "stg21/ocean_pillar_art/pillar_rubble_0.inc"
};

static NJS_CNK_OBJECT pillar_rubble_0_object = {
#include "stg21/ocean_pillar_art/pillar_rubble_0_object.inc"
};

static NJS_TEXNAME pillar_material_7_textures[1] = {
#include "stg21/ocean_pillar_art/pillar_material_7_textures.inc"
};

static NJS_TEXLIST pillar_material_7_texlist = {
#include "stg21/ocean_pillar_art/pillar_material_7_texlist.inc"
};

static Sint16 pillar_rubble_1_polygons[69] = {
#include "stg21/ocean_pillar_art/pillar_rubble_1_polygons.inc"
};

static Sint32 pillar_rubble_1_vertices[35] = {
#include "stg21/ocean_pillar_art/pillar_rubble_1_vertices.inc"
};

NJS_CNK_MODEL pillar_rubble_1 = {
#include "stg21/ocean_pillar_art/pillar_rubble_1.inc"
};

static NJS_CNK_OBJECT pillar_rubble_1_object = {
#include "stg21/ocean_pillar_art/pillar_rubble_1_object.inc"
};

static NJS_MKEY_F pillar_position_keys[104] = {
#include "stg21/ocean_pillar_art/pillar_position_keys.inc"
};

static NJS_MKEY_A pillar_angle_keys[123] = {
#include "stg21/ocean_pillar_art/pillar_angle_keys.inc"
};

static NJS_MDATA3 pillar_tracks[1] = {
#include "stg21/ocean_pillar_art/pillar_tracks.inc"
};

NJS_MOTION pillar_motion[1] = {
#include "stg21/ocean_pillar_art/pillar_motion.inc"
};
