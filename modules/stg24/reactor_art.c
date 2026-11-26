/* Eternal Engine's reactor: its body, lid, rim and panels, and the fragments
 * it breaks into. its cannon and the cannon's shot follow */
#include <ninja.h>
#include <sg_xpt.h>
#include "draw/texture_anim.h"
#include "stg24/reactor_art.h"
#include "shared/cannon_parts.h"
#include "stg24/reactor_art/text.inc"

/* the body and its parts, the coils and guns among them. their texture list
 * comes first, and the body's collision object last */
static NJS_TEXNAME reactor_textures_names[18] = {
#include "stg24/reactor_art/reactor_textures_names.inc"
};

NJS_TEXLIST reactor_textures = {
#include "stg24/reactor_art/reactor_textures.inc"
};

static Sint16 reactor_body_part_18_polygons[57] = {
#include "stg24/reactor_art/reactor_body_part_18_polygons.inc"
};

static Sint32 reactor_body_part_18_vertices[39] = {
#include "stg24/reactor_art/reactor_body_part_18_vertices.inc"
};

static NJS_CNK_MODEL reactor_body_part_18_model = {
#include "stg24/reactor_art/reactor_body_part_18_model.inc"
};

static NJS_CNK_OBJECT reactor_body_part_18 = {
#include "stg24/reactor_art/reactor_body_part_18.inc"
};

static Sint16 reactor_body_part_17_polygons[57] = {
#include "stg24/reactor_art/reactor_body_part_17_polygons.inc"
};

static Sint32 reactor_body_part_17_vertices[39] = {
#include "stg24/reactor_art/reactor_body_part_17_vertices.inc"
};

static NJS_CNK_MODEL reactor_body_part_17_model = {
#include "stg24/reactor_art/reactor_body_part_17_model.inc"
};

static NJS_CNK_OBJECT reactor_body_part_17 = {
#include "stg24/reactor_art/reactor_body_part_17.inc"
};

static Sint16 reactor_body_part_16_polygons[57] = {
#include "stg24/reactor_art/reactor_body_part_16_polygons.inc"
};

static Sint32 reactor_body_part_16_vertices[39] = {
#include "stg24/reactor_art/reactor_body_part_16_vertices.inc"
};

static NJS_CNK_MODEL reactor_body_part_16_model = {
#include "stg24/reactor_art/reactor_body_part_16_model.inc"
};

static NJS_CNK_OBJECT reactor_body_part_16 = {
#include "stg24/reactor_art/reactor_body_part_16.inc"
};

static Sint16 reactor_body_part_15_polygons[57] = {
#include "stg24/reactor_art/reactor_body_part_15_polygons.inc"
};

static Sint32 reactor_body_part_15_vertices[39] = {
#include "stg24/reactor_art/reactor_body_part_15_vertices.inc"
};

static NJS_CNK_MODEL reactor_body_part_15_model = {
#include "stg24/reactor_art/reactor_body_part_15_model.inc"
};

static NJS_CNK_OBJECT reactor_body_part_15 = {
#include "stg24/reactor_art/reactor_body_part_15.inc"
};

static Sint16 reactor_body_part_14_polygons[57] = {
#include "stg24/reactor_art/reactor_body_part_14_polygons.inc"
};

static Sint32 reactor_body_part_14_vertices[39] = {
#include "stg24/reactor_art/reactor_body_part_14_vertices.inc"
};

static NJS_CNK_MODEL reactor_body_part_14_model = {
#include "stg24/reactor_art/reactor_body_part_14_model.inc"
};

static NJS_CNK_OBJECT reactor_body_part_14 = {
#include "stg24/reactor_art/reactor_body_part_14.inc"
};

static Sint16 reactor_body_part_13_polygons[57] = {
#include "stg24/reactor_art/reactor_body_part_13_polygons.inc"
};

static Sint32 reactor_body_part_13_vertices[39] = {
#include "stg24/reactor_art/reactor_body_part_13_vertices.inc"
};

static NJS_CNK_MODEL reactor_body_part_13_model = {
#include "stg24/reactor_art/reactor_body_part_13_model.inc"
};

static NJS_CNK_OBJECT reactor_body_part_13 = {
#include "stg24/reactor_art/reactor_body_part_13.inc"
};

static Sint16 reactor_body_part_12_polygons[57] = {
#include "stg24/reactor_art/reactor_body_part_12_polygons.inc"
};

static Sint32 reactor_body_part_12_vertices[39] = {
#include "stg24/reactor_art/reactor_body_part_12_vertices.inc"
};

static NJS_CNK_MODEL reactor_body_part_12_model = {
#include "stg24/reactor_art/reactor_body_part_12_model.inc"
};

static NJS_CNK_OBJECT reactor_body_part_12 = {
#include "stg24/reactor_art/reactor_body_part_12.inc"
};

static Sint16 reactor_body_part_11_polygons[57] = {
#include "stg24/reactor_art/reactor_body_part_11_polygons.inc"
};

static Sint32 reactor_body_part_11_vertices[39] = {
#include "stg24/reactor_art/reactor_body_part_11_vertices.inc"
};

static NJS_CNK_MODEL reactor_body_part_11_model = {
#include "stg24/reactor_art/reactor_body_part_11_model.inc"
};

static NJS_CNK_OBJECT reactor_body_part_11 = {
#include "stg24/reactor_art/reactor_body_part_11.inc"
};

static Sint16 reactor_left_gun_polygons[325] = {
#include "stg24/reactor_art/reactor_left_gun_polygons.inc"
};

static Sint32 reactor_left_gun_vertices[215] = {
#include "stg24/reactor_art/reactor_left_gun_vertices.inc"
};

static NJS_CNK_MODEL reactor_left_gun_model = {
#include "stg24/reactor_art/reactor_left_gun_model.inc"
};

NJS_CNK_OBJECT reactor_left_gun[1] = {
#include "stg24/reactor_art/reactor_left_gun.inc"
};

static Sint16 reactor_left_support_polygons[171] = {
#include "stg24/reactor_art/reactor_left_support_polygons.inc"
};

static Sint32 reactor_left_support_vertices[147] = {
#include "stg24/reactor_art/reactor_left_support_vertices.inc"
};

static NJS_CNK_MODEL reactor_left_support_model = {
#include "stg24/reactor_art/reactor_left_support_model.inc"
};

NJS_CNK_OBJECT reactor_left_support[1] = {
#include "stg24/reactor_art/reactor_left_support.inc"
};

static Sint16 reactor_first_coil_polygons[627] = {
#include "stg24/reactor_art/reactor_first_coil_polygons.inc"
};

static Sint32 reactor_first_coil_vertices[395] = {
#include "stg24/reactor_art/reactor_first_coil_vertices.inc"
};

static NJS_CNK_MODEL reactor_first_coil_model = {
#include "stg24/reactor_art/reactor_first_coil_model.inc"
};

NJS_CNK_OBJECT reactor_first_coil[1] = {
#include "stg24/reactor_art/reactor_first_coil.inc"
};

static Sint16 reactor_body_part_7_polygons[767] = {
#include "stg24/reactor_art/reactor_body_part_7_polygons.inc"
};

static Sint32 reactor_body_part_7_vertices[515] = {
#include "stg24/reactor_art/reactor_body_part_7_vertices.inc"
};

static NJS_CNK_MODEL reactor_body_part_7_model = {
#include "stg24/reactor_art/reactor_body_part_7_model.inc"
};

static NJS_CNK_OBJECT reactor_body_part_7 = {
#include "stg24/reactor_art/reactor_body_part_7.inc"
};

static Sint16 reactor_second_coil_polygons[627] = {
#include "stg24/reactor_art/reactor_second_coil_polygons.inc"
};

static Sint32 reactor_second_coil_vertices[395] = {
#include "stg24/reactor_art/reactor_second_coil_vertices.inc"
};

static NJS_CNK_MODEL reactor_second_coil_model = {
#include "stg24/reactor_art/reactor_second_coil_model.inc"
};

NJS_CNK_OBJECT reactor_second_coil[1] = {
#include "stg24/reactor_art/reactor_second_coil.inc"
};

static Sint16 reactor_body_part_5_polygons[101] = {
#include "stg24/reactor_art/reactor_body_part_5_polygons.inc"
};

static Sint32 reactor_body_part_5_vertices[107] = {
#include "stg24/reactor_art/reactor_body_part_5_vertices.inc"
};

static NJS_CNK_MODEL reactor_body_part_5_model = {
#include "stg24/reactor_art/reactor_body_part_5_model.inc"
};

static NJS_CNK_OBJECT reactor_body_part_5 = {
#include "stg24/reactor_art/reactor_body_part_5.inc"
};

static Sint16 reactor_right_gun_polygons[325] = {
#include "stg24/reactor_art/reactor_right_gun_polygons.inc"
};

static Sint32 reactor_right_gun_vertices[215] = {
#include "stg24/reactor_art/reactor_right_gun_vertices.inc"
};

static NJS_CNK_MODEL reactor_right_gun_model = {
#include "stg24/reactor_art/reactor_right_gun_model.inc"
};

NJS_CNK_OBJECT reactor_right_gun[1] = {
#include "stg24/reactor_art/reactor_right_gun.inc"
};

static Sint16 reactor_right_support_polygons[171] = {
#include "stg24/reactor_art/reactor_right_support_polygons.inc"
};

static Sint32 reactor_right_support_vertices[147] = {
#include "stg24/reactor_art/reactor_right_support_vertices.inc"
};

static NJS_CNK_MODEL reactor_right_support_model = {
#include "stg24/reactor_art/reactor_right_support_model.inc"
};

NJS_CNK_OBJECT reactor_right_support[1] = {
#include "stg24/reactor_art/reactor_right_support.inc"
};

static Sint16 reactor_body_part_2_polygons[57] = {
#include "stg24/reactor_art/reactor_body_part_2_polygons.inc"
};

static Sint32 reactor_body_part_2_vertices[39] = {
#include "stg24/reactor_art/reactor_body_part_2_vertices.inc"
};

static NJS_CNK_MODEL reactor_body_part_2_model = {
#include "stg24/reactor_art/reactor_body_part_2_model.inc"
};

static NJS_CNK_OBJECT reactor_body_part_2 = {
#include "stg24/reactor_art/reactor_body_part_2.inc"
};

static Sint16 reactor_body_polygons[2007] = {
#include "stg24/reactor_art/reactor_body_polygons.inc"
};

static Sint32 reactor_body_vertices[1767] = {
#include "stg24/reactor_art/reactor_body_vertices.inc"
};

static NJS_CNK_MODEL reactor_body_model = {
#include "stg24/reactor_art/reactor_body_model.inc"
};

NJS_CNK_OBJECT reactor_body[1] = {
#include "stg24/reactor_art/reactor_body.inc"
};

static NJS_MATERIAL reactor_body_collision_materials[1] = {
#include "stg24/reactor_art/reactor_body_collision_materials.inc"
};

static Sint16 reactor_body_collision_indices[167] = {
#include "stg24/reactor_art/reactor_body_collision_indices.inc"
};

static NJS_MESHSET reactor_body_collision_meshsets[1] = {
#include "stg24/reactor_art/reactor_body_collision_meshsets.inc"
};

static NJS_POINT3 reactor_body_collision_points[72] = {
#include "stg24/reactor_art/reactor_body_collision_points.inc"
};

static NJS_VECTOR reactor_body_collision_normals[72] = {
#include "stg24/reactor_art/reactor_body_collision_normals.inc"
};

static NJS_MODEL reactor_body_collision_model = {
#include "stg24/reactor_art/reactor_body_collision_model.inc"
};

NJS_OBJECT reactor_body_collision[1] = {
#include "stg24/reactor_art/reactor_body_collision.inc"
};

static NJS_TEXNAME reactor_lid_textures_names[7] = {
#include "stg24/reactor_art/reactor_lid_textures_names.inc"
};

NJS_TEXLIST reactor_lid_textures = {
#include "stg24/reactor_art/reactor_lid_textures.inc"
};

static Sint16 reactor_lid_model_polygons[767] = {
#include "stg24/reactor_art/reactor_lid_model_polygons.inc"
};

static Sint32 reactor_lid_model_vertices[515] = {
#include "stg24/reactor_art/reactor_lid_model_vertices.inc"
};

static NJS_CNK_MODEL reactor_lid_model_model = {
#include "stg24/reactor_art/reactor_lid_model_model.inc"
};

NJS_CNK_OBJECT reactor_lid_model[1] = {
#include "stg24/reactor_art/reactor_lid_model.inc"
};

static NJS_MATERIAL reactor_lid_collision_materials[1] = {
#include "stg24/reactor_art/reactor_lid_collision_materials.inc"
};

static Sint16 reactor_lid_collision_indices[80] = {
#include "stg24/reactor_art/reactor_lid_collision_indices.inc"
};

static NJS_MESHSET reactor_lid_collision_meshsets[1] = {
#include "stg24/reactor_art/reactor_lid_collision_meshsets.inc"
};

static NJS_POINT3 reactor_lid_collision_points[40] = {
#include "stg24/reactor_art/reactor_lid_collision_points.inc"
};

static NJS_VECTOR reactor_lid_collision_normals[40] = {
#include "stg24/reactor_art/reactor_lid_collision_normals.inc"
};

static NJS_MODEL reactor_lid_collision_model = {
#include "stg24/reactor_art/reactor_lid_collision_model.inc"
};

NJS_OBJECT reactor_lid_collision[1] = {
#include "stg24/reactor_art/reactor_lid_collision.inc"
};

static NJS_TEXNAME reactor_rim_textures_names[1] = {
#include "stg24/reactor_art/reactor_rim_textures_names.inc"
};

NJS_TEXLIST reactor_rim_textures = {
#include "stg24/reactor_art/reactor_rim_textures.inc"
};

static Sint16 reactor_rim_polygons[101] = {
#include "stg24/reactor_art/reactor_rim_polygons.inc"
};

static Sint32 reactor_rim_vertices[107] = {
#include "stg24/reactor_art/reactor_rim_vertices.inc"
};

static NJS_CNK_MODEL reactor_rim_model = {
#include "stg24/reactor_art/reactor_rim_model.inc"
};

NJS_CNK_OBJECT reactor_rim[1] = {
#include "stg24/reactor_art/reactor_rim.inc"
};

static NJS_TEXNAME reactor_fragment_1_texlist_names[2] = {
#include "stg24/reactor_art/reactor_fragment_1_texlist_names.inc"
};

NJS_TEXLIST reactor_fragment_1_texlist = {
#include "stg24/reactor_art/reactor_fragment_1_texlist.inc"
};

static Sint16 reactor_fragment_1_polygons[47] = {
#include "stg24/reactor_art/reactor_fragment_1_polygons.inc"
};

static Sint32 reactor_fragment_1_vertices[35] = {
#include "stg24/reactor_art/reactor_fragment_1_vertices.inc"
};

static NJS_CNK_MODEL reactor_fragment_1_model = {
#include "stg24/reactor_art/reactor_fragment_1_model.inc"
};

NJS_CNK_OBJECT reactor_fragment_1 = {
#include "stg24/reactor_art/reactor_fragment_1.inc"
};

static NJS_TEXNAME reactor_fragment_2_texlist_names[2] = {
#include "stg24/reactor_art/reactor_fragment_2_texlist_names.inc"
};

NJS_TEXLIST reactor_fragment_2_texlist = {
#include "stg24/reactor_art/reactor_fragment_2_texlist.inc"
};

static Sint16 reactor_fragment_2_polygons[47] = {
#include "stg24/reactor_art/reactor_fragment_2_polygons.inc"
};

static Sint32 reactor_fragment_2_vertices[35] = {
#include "stg24/reactor_art/reactor_fragment_2_vertices.inc"
};

static NJS_CNK_MODEL reactor_fragment_2_model = {
#include "stg24/reactor_art/reactor_fragment_2_model.inc"
};

NJS_CNK_OBJECT reactor_fragment_2 = {
#include "stg24/reactor_art/reactor_fragment_2.inc"
};

static NJS_TEXNAME reactor_fragment_3_texlist_names[1] = {
#include "stg24/reactor_art/reactor_fragment_3_texlist_names.inc"
};

NJS_TEXLIST reactor_fragment_3_texlist = {
#include "stg24/reactor_art/reactor_fragment_3_texlist.inc"
};

static Sint16 reactor_fragment_3_polygons[155] = {
#include "stg24/reactor_art/reactor_fragment_3_polygons.inc"
};

static Sint32 reactor_fragment_3_vertices[103] = {
#include "stg24/reactor_art/reactor_fragment_3_vertices.inc"
};

static NJS_CNK_MODEL reactor_fragment_3_model = {
#include "stg24/reactor_art/reactor_fragment_3_model.inc"
};

NJS_CNK_OBJECT reactor_fragment_3 = {
#include "stg24/reactor_art/reactor_fragment_3.inc"
};

static NJS_TEXNAME reactor_fragment_4_texlist_names[2] = {
#include "stg24/reactor_art/reactor_fragment_4_texlist_names.inc"
};

NJS_TEXLIST reactor_fragment_4_texlist = {
#include "stg24/reactor_art/reactor_fragment_4_texlist.inc"
};

static Sint16 reactor_fragment_4_polygons[41] = {
#include "stg24/reactor_art/reactor_fragment_4_polygons.inc"
};

static Sint32 reactor_fragment_4_vertices[23] = {
#include "stg24/reactor_art/reactor_fragment_4_vertices.inc"
};

static NJS_CNK_MODEL reactor_fragment_4_model = {
#include "stg24/reactor_art/reactor_fragment_4_model.inc"
};

NJS_CNK_OBJECT reactor_fragment_4 = {
#include "stg24/reactor_art/reactor_fragment_4.inc"
};

static NJS_TEXNAME reactor_fragment_5_texlist_names[3] = {
#include "stg24/reactor_art/reactor_fragment_5_texlist_names.inc"
};

NJS_TEXLIST reactor_fragment_5_texlist = {
#include "stg24/reactor_art/reactor_fragment_5_texlist.inc"
};

static Sint16 reactor_fragment_5_polygons[253] = {
#include "stg24/reactor_art/reactor_fragment_5_polygons.inc"
};

static Sint32 reactor_fragment_5_vertices[139] = {
#include "stg24/reactor_art/reactor_fragment_5_vertices.inc"
};

static NJS_CNK_MODEL reactor_fragment_5_model = {
#include "stg24/reactor_art/reactor_fragment_5_model.inc"
};

NJS_CNK_OBJECT reactor_fragment_5 = {
#include "stg24/reactor_art/reactor_fragment_5.inc"
};

static NJS_TEXNAME reactor_fragment_6_texlist_names[3] = {
#include "stg24/reactor_art/reactor_fragment_6_texlist_names.inc"
};

NJS_TEXLIST reactor_fragment_6_texlist = {
#include "stg24/reactor_art/reactor_fragment_6_texlist.inc"
};

static Sint16 reactor_fragment_6_polygons[73] = {
#include "stg24/reactor_art/reactor_fragment_6_polygons.inc"
};

static Sint32 reactor_fragment_6_vertices[51] = {
#include "stg24/reactor_art/reactor_fragment_6_vertices.inc"
};

static NJS_CNK_MODEL reactor_fragment_6_model = {
#include "stg24/reactor_art/reactor_fragment_6_model.inc"
};

NJS_CNK_OBJECT reactor_fragment_6 = {
#include "stg24/reactor_art/reactor_fragment_6.inc"
};

static NJS_TEXNAME reactor_fragment_7_texlist_names[1] = {
#include "stg24/reactor_art/reactor_fragment_7_texlist_names.inc"
};

NJS_TEXLIST reactor_fragment_7_texlist = {
#include "stg24/reactor_art/reactor_fragment_7_texlist.inc"
};

static Sint16 reactor_fragment_7_polygons[197] = {
#include "stg24/reactor_art/reactor_fragment_7_polygons.inc"
};

static Sint32 reactor_fragment_7_vertices[99] = {
#include "stg24/reactor_art/reactor_fragment_7_vertices.inc"
};

static NJS_CNK_MODEL reactor_fragment_7_model = {
#include "stg24/reactor_art/reactor_fragment_7_model.inc"
};

NJS_CNK_OBJECT reactor_fragment_7 = {
#include "stg24/reactor_art/reactor_fragment_7.inc"
};

static NJS_TEXNAME reactor_fragment_8_texlist_names[4] = {
#include "stg24/reactor_art/reactor_fragment_8_texlist_names.inc"
};

NJS_TEXLIST reactor_fragment_8_texlist = {
#include "stg24/reactor_art/reactor_fragment_8_texlist.inc"
};

static Sint16 reactor_fragment_8_polygons[81] = {
#include "stg24/reactor_art/reactor_fragment_8_polygons.inc"
};

static Sint32 reactor_fragment_8_vertices[55] = {
#include "stg24/reactor_art/reactor_fragment_8_vertices.inc"
};

static NJS_CNK_MODEL reactor_fragment_8_model = {
#include "stg24/reactor_art/reactor_fragment_8_model.inc"
};

NJS_CNK_OBJECT reactor_fragment_8 = {
#include "stg24/reactor_art/reactor_fragment_8.inc"
};

static NJS_TEXNAME reactor_panel_bright_names[1] = {
#include "stg24/reactor_art/reactor_panel_bright_names.inc"
};

NJS_TEXLIST reactor_panel_bright = {
#include "stg24/reactor_art/reactor_panel_bright.inc"
};

static NJS_TEXNAME reactor_panel_dim_names[1] = {
#include "stg24/reactor_art/reactor_panel_dim_names.inc"
};

NJS_TEXLIST reactor_panel_dim = {
#include "stg24/reactor_art/reactor_panel_dim.inc"
};

static Sint16 reactor_panel_polygons[57] = {
#include "stg24/reactor_art/reactor_panel_polygons.inc"
};

static Sint32 reactor_panel_vertices[39] = {
#include "stg24/reactor_art/reactor_panel_vertices.inc"
};

static NJS_CNK_MODEL reactor_panel_model = {
#include "stg24/reactor_art/reactor_panel_model.inc"
};

NJS_CNK_OBJECT reactor_panel[1] = {
#include "stg24/reactor_art/reactor_panel.inc"
};

/* the cannon's shot: a center and four sparks about it, with a bright and a
 * dim texture list */
static NJS_TEXNAME cannon_shot_bright_names[2] = {
#include "stg24/reactor_art/cannon_shot_bright_names.inc"
};

NJS_TEXLIST cannon_shot_bright = {
#include "stg24/reactor_art/cannon_shot_bright.inc"
};

static NJS_TEXNAME cannon_shot_dim_names[2] = {
#include "stg24/reactor_art/cannon_shot_dim_names.inc"
};

NJS_TEXLIST cannon_shot_dim = {
#include "stg24/reactor_art/cannon_shot_dim.inc"
};

static Sint16 shot_spark_down_polygons[29] = {
#include "stg24/reactor_art/shot_spark_down_polygons.inc"
};

static Sint32 shot_spark_down_vertices[19] = {
#include "stg24/reactor_art/shot_spark_down_vertices.inc"
};

static NJS_CNK_MODEL shot_spark_down_model = {
#include "stg24/reactor_art/shot_spark_down_model.inc"
};

static NJS_CNK_OBJECT shot_spark_down = {
#include "stg24/reactor_art/shot_spark_down.inc"
};

static Sint16 shot_spark_back_polygons[29] = {
#include "stg24/reactor_art/shot_spark_back_polygons.inc"
};

static Sint32 shot_spark_back_vertices[19] = {
#include "stg24/reactor_art/shot_spark_back_vertices.inc"
};

static NJS_CNK_MODEL shot_spark_back_model = {
#include "stg24/reactor_art/shot_spark_back_model.inc"
};

static NJS_CNK_OBJECT shot_spark_back = {
#include "stg24/reactor_art/shot_spark_back.inc"
};

static Sint16 shot_spark_front_polygons[29] = {
#include "stg24/reactor_art/shot_spark_front_polygons.inc"
};

static Sint32 shot_spark_front_vertices[19] = {
#include "stg24/reactor_art/shot_spark_front_vertices.inc"
};

static NJS_CNK_MODEL shot_spark_front_model = {
#include "stg24/reactor_art/shot_spark_front_model.inc"
};

static NJS_CNK_OBJECT shot_spark_front = {
#include "stg24/reactor_art/shot_spark_front.inc"
};

static Sint16 shot_spark_up_polygons[29] = {
#include "stg24/reactor_art/shot_spark_up_polygons.inc"
};

static Sint32 shot_spark_up_vertices[19] = {
#include "stg24/reactor_art/shot_spark_up_vertices.inc"
};

static NJS_CNK_MODEL shot_spark_up_model = {
#include "stg24/reactor_art/shot_spark_up_model.inc"
};

static NJS_CNK_OBJECT shot_spark_up = {
#include "stg24/reactor_art/shot_spark_up.inc"
};

static Sint16 cannon_shot_model_polygons[57] = {
#include "stg24/reactor_art/cannon_shot_model_polygons.inc"
};

static Sint32 cannon_shot_model_vertices[27] = {
#include "stg24/reactor_art/cannon_shot_model_vertices.inc"
};

static NJS_CNK_MODEL shot_center_model = {
#include "stg24/reactor_art/shot_center_model.inc"
};

NJS_CNK_OBJECT cannon_shot_model = {
#include "stg24/reactor_art/cannon_shot_model.inc"
};

static NJS_TEXNAME cannon_texlist_names[6] = {
#include "stg24/reactor_art/cannon_texlist_names.inc"
};

NJS_TEXLIST cannon_texlist = {
#include "stg24/reactor_art/cannon_texlist.inc"
};

static Sint16 cannon_mount_polygons[639] = {
#include "stg24/reactor_art/cannon_mount_polygons.inc"
};

static Sint32 cannon_mount_vertices[399] = {
#include "stg24/reactor_art/cannon_mount_vertices.inc"
};

NJS_CNK_MODEL cannon_barrel = {
#include "stg24/reactor_art/cannon_barrel.inc"
};

static NJS_CNK_OBJECT cannon_mount = {
#include "stg24/reactor_art/cannon_mount.inc"
};

static NJS_TEXNAME reactor_smoke_textures_names[10] = {
#include "stg24/reactor_art/reactor_smoke_textures_names.inc"
};

NJS_TEXLIST reactor_smoke_textures = {
#include "stg24/reactor_art/reactor_smoke_textures.inc"
};

static Sint16 reactor_rim_scroll_keys[66] = {
#include "stg24/reactor_art/reactor_rim_scroll_keys.inc"
};

struct texture_anim reactor_rim_anim[1] = {
#include "stg24/reactor_art/reactor_rim_anim.inc"
};
