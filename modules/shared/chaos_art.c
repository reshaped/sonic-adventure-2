#include <ninja.h>
#include <sg_xpt.h>

#include "collision/collision.h"
#include "enemy/enemy.h"
#include "shared/chaos_art/assets.h"
#include "shared/chaos_art/materials.h"

/* the names that a texture list's textures point at, each starting on a 4-byte
 * boundary */
const char chaos_enemy_filenames[96] = {
#include "shared/chaos_art/chaos_enemy_filenames.inc"
};

/* the texture lists, chunk models, motions and enemy actions that 12 images
 * place, from the disc. its D precedes enemy_chaos.c */
static NJS_TEXNAME textures[11] = {
#include "shared/chaos_art/textures.inc"
};
NJS_TEXLIST chaos_texlist = {
#include "shared/chaos_art/chaos_texlist.inc"
};
static Sint16 normal_joint_10_polygons[439] = {
#include "shared/chaos_art/normal_joint_10_polygons.inc"
};
static Sint32 normal_joint_10_vertices[261] = {
#include "shared/chaos_art/normal_joint_10_vertices.inc"
};
static NJS_CNK_MODEL normal_joint_10_model = {
#include "shared/chaos_art/normal_joint_10_model.inc"
};
static NJS_CNK_OBJECT normal_joint_10 = {
#include "shared/chaos_art/normal_joint_10.inc"
};
static Sint16 normal_muzzle_node_polygons[495] = {
#include "shared/chaos_art/normal_muzzle_node_polygons.inc"
};
static Sint32 normal_muzzle_node_vertices[363] = {
#include "shared/chaos_art/normal_muzzle_node_vertices.inc"
};
static NJS_CNK_MODEL normal_muzzle_node_model = {
#include "shared/chaos_art/normal_muzzle_node_model.inc"
};
NJS_CNK_OBJECT normal_muzzle_node = {
#include "shared/chaos_art/normal_muzzle_node.inc"
};
static Sint16 normal_joint_8_polygons[307] = {
#include "shared/chaos_art/normal_joint_8_polygons.inc"
};
static Sint32 normal_joint_8_vertices[235] = {
#include "shared/chaos_art/normal_joint_8_vertices.inc"
};
static NJS_CNK_MODEL normal_joint_8_model = {
#include "shared/chaos_art/normal_joint_8_model.inc"
};
static NJS_CNK_OBJECT normal_joint_8 = {
#include "shared/chaos_art/normal_joint_8.inc"
};
static Sint16 normal_joint_7_polygons[113] = {
#include "shared/chaos_art/normal_joint_7_polygons.inc"
};
static Sint32 normal_joint_7_vertices[113] = {
#include "shared/chaos_art/normal_joint_7_vertices.inc"
};
static NJS_CNK_MODEL normal_joint_7_model = {
#include "shared/chaos_art/normal_joint_7_model.inc"
};
static NJS_CNK_OBJECT normal_joint_7 = {
#include "shared/chaos_art/normal_joint_7.inc"
};
static NJS_CNK_OBJECT normal_joint_6 = {
#include "shared/chaos_art/normal_joint_6.inc"
};
static Sint32 normal_joint_5_vertices[201] = {
#include "shared/chaos_art/normal_joint_5_vertices.inc"
};
static NJS_CNK_MODEL normal_joint_5_model = {
#include "shared/chaos_art/normal_joint_5_model.inc"
};
static NJS_CNK_OBJECT normal_joint_5 = {
#include "shared/chaos_art/normal_joint_5.inc"
};
static Sint32 normal_joint_4_vertices[257] = {
#include "shared/chaos_art/normal_joint_4_vertices.inc"
};
static NJS_CNK_MODEL normal_joint_4_model = {
#include "shared/chaos_art/normal_joint_4_model.inc"
};
static NJS_CNK_OBJECT normal_joint_4 = {
#include "shared/chaos_art/normal_joint_4.inc"
};
static Sint32 normal_joint_3_vertices[229] = {
#include "shared/chaos_art/normal_joint_3_vertices.inc"
};
static NJS_CNK_MODEL normal_joint_3_model = {
#include "shared/chaos_art/normal_joint_3_model.inc"
};
static NJS_CNK_OBJECT normal_joint_3 = {
#include "shared/chaos_art/normal_joint_3.inc"
};
static NJS_CNK_OBJECT normal_joint_2 = {
#include "shared/chaos_art/normal_joint_2.inc"
};
static Sint32 normal_joint_1_vertices[125] = {
#include "shared/chaos_art/normal_joint_1_vertices.inc"
};
static NJS_CNK_MODEL normal_joint_1_model = {
#include "shared/chaos_art/normal_joint_1_model.inc"
};
static NJS_CNK_OBJECT normal_joint_1 = {
#include "shared/chaos_art/normal_joint_1.inc"
};
struct normal_object_polygons_layout normal_object_polygons = {
#include "shared/chaos_art/normal_object_polygons.inc"
};
static NJS_CNK_MODEL normal_object_model = {
#include "shared/chaos_art/normal_object_model.inc"
};
NJS_CNK_OBJECT normal_object[1] = {
#include "shared/chaos_art/normal_object.inc"
};
static NJS_MKEY_A normal_motion_joint_3_angles[12] = {
#include "shared/chaos_art/normal_motion_joint_3_angles.inc"
};
static NJS_MKEY_A normal_motion_joint_4_angles[12] = {
#include "shared/chaos_art/normal_motion_joint_4_angles.inc"
};
static NJS_MKEY_A normal_motion_joint_5_angles[12] = {
#include "shared/chaos_art/normal_motion_joint_5_angles.inc"
};
static NJS_MKEY_A normal_motion_joint_8_angles[12] = {
#include "shared/chaos_art/normal_motion_joint_8_angles.inc"
};
static NJS_MKEY_A normal_motion_joint_9_angles[12] = {
#include "shared/chaos_art/normal_motion_joint_9_angles.inc"
};
static NJS_MKEY_A normal_motion_joint_10_angles[12] = {
#include "shared/chaos_art/normal_motion_joint_10_angles.inc"
};
static NJS_MDATA3 normal_motion_tracks[11] = {
#include "shared/chaos_art/normal_motion_tracks.inc"
};
NJS_MOTION normal_motion = {
#include "shared/chaos_art/normal_motion.inc"
};
static NJS_CNK_OBJECT normal_reach_joint_30 = {
#include "shared/chaos_art/normal_reach_joint_30.inc"
};
static Sint16 normal_reach_joint_29_polygons[2] = {
#include "shared/chaos_art/normal_reach_joint_29_polygons.inc"
};
static Sint32 normal_reach_joint_29_vertices[137] = {
#include "shared/chaos_art/normal_reach_joint_29_vertices.inc"
};
static NJS_CNK_MODEL normal_reach_joint_29_model = {
#include "shared/chaos_art/normal_reach_joint_29_model.inc"
};
static NJS_CNK_OBJECT normal_reach_joint_29 = {
#include "shared/chaos_art/normal_reach_joint_29.inc"
};
static Sint32 normal_reach_joint_28_vertices[173] = {
#include "shared/chaos_art/normal_reach_joint_28_vertices.inc"
};
static NJS_CNK_MODEL normal_reach_joint_28_model = {
#include "shared/chaos_art/normal_reach_joint_28_model.inc"
};
static NJS_CNK_OBJECT normal_reach_joint_28 = {
#include "shared/chaos_art/normal_reach_joint_28.inc"
};
static Sint32 normal_reach_joint_27_vertices[131] = {
#include "shared/chaos_art/normal_reach_joint_27_vertices.inc"
};
static NJS_CNK_MODEL normal_reach_joint_27_model = {
#include "shared/chaos_art/normal_reach_joint_27_model.inc"
};
static NJS_CNK_OBJECT normal_reach_joint_27 = {
#include "shared/chaos_art/normal_reach_joint_27.inc"
};
static Sint32 normal_reach_joint_26_vertices[131] = {
#include "shared/chaos_art/normal_reach_joint_26_vertices.inc"
};
static NJS_CNK_MODEL normal_reach_joint_26_model = {
#include "shared/chaos_art/normal_reach_joint_26_model.inc"
};
static NJS_CNK_OBJECT normal_reach_joint_26 = {
#include "shared/chaos_art/normal_reach_joint_26.inc"
};
static Sint32 normal_reach_joint_25_vertices[131] = {
#include "shared/chaos_art/normal_reach_joint_25_vertices.inc"
};
static NJS_CNK_MODEL normal_reach_joint_25_model = {
#include "shared/chaos_art/normal_reach_joint_25_model.inc"
};
static NJS_CNK_OBJECT normal_reach_joint_25 = {
#include "shared/chaos_art/normal_reach_joint_25.inc"
};
static Sint32 normal_reach_joint_24_vertices[89] = {
#include "shared/chaos_art/normal_reach_joint_24_vertices.inc"
};
static NJS_CNK_MODEL normal_reach_joint_24_model = {
#include "shared/chaos_art/normal_reach_joint_24_model.inc"
};
static NJS_CNK_OBJECT normal_reach_joint_24 = {
#include "shared/chaos_art/normal_reach_joint_24.inc"
};
static Sint32 normal_reach_joint_23_vertices[229] = {
#include "shared/chaos_art/normal_reach_joint_23_vertices.inc"
};
static NJS_CNK_MODEL normal_reach_joint_23_model = {
#include "shared/chaos_art/normal_reach_joint_23_model.inc"
};
static NJS_CNK_OBJECT normal_reach_joint_23 = {
#include "shared/chaos_art/normal_reach_joint_23.inc"
};
static Sint32 normal_reach_joint_22_vertices[250] = {
#include "shared/chaos_art/normal_reach_joint_22_vertices.inc"
};
static NJS_CNK_MODEL normal_reach_joint_22_model = {
#include "shared/chaos_art/normal_reach_joint_22_model.inc"
};
static NJS_CNK_OBJECT normal_reach_joint_22 = {
#include "shared/chaos_art/normal_reach_joint_22.inc"
};
NJS_CNK_OBJECT normal_arm_b = {
#include "shared/chaos_art/normal_arm_b.inc"
};
static NJS_CNK_OBJECT normal_reach_joint_20 = {
#include "shared/chaos_art/normal_reach_joint_20.inc"
};
static Sint32 normal_reach_joint_19_vertices[131] = {
#include "shared/chaos_art/normal_reach_joint_19_vertices.inc"
};
static NJS_CNK_MODEL normal_reach_joint_19_model = {
#include "shared/chaos_art/normal_reach_joint_19_model.inc"
};
static NJS_CNK_OBJECT normal_reach_joint_19 = {
#include "shared/chaos_art/normal_reach_joint_19.inc"
};
static Sint32 normal_reach_joint_18_vertices[173] = {
#include "shared/chaos_art/normal_reach_joint_18_vertices.inc"
};
static NJS_CNK_MODEL normal_reach_joint_18_model = {
#include "shared/chaos_art/normal_reach_joint_18_model.inc"
};
static NJS_CNK_OBJECT normal_reach_joint_18 = {
#include "shared/chaos_art/normal_reach_joint_18.inc"
};
static Sint32 normal_reach_joint_17_vertices[131] = {
#include "shared/chaos_art/normal_reach_joint_17_vertices.inc"
};
static NJS_CNK_MODEL normal_reach_joint_17_model = {
#include "shared/chaos_art/normal_reach_joint_17_model.inc"
};
static NJS_CNK_OBJECT normal_reach_joint_17 = {
#include "shared/chaos_art/normal_reach_joint_17.inc"
};
static Sint32 normal_reach_joint_16_vertices[131] = {
#include "shared/chaos_art/normal_reach_joint_16_vertices.inc"
};
static NJS_CNK_MODEL normal_reach_joint_16_model = {
#include "shared/chaos_art/normal_reach_joint_16_model.inc"
};
static NJS_CNK_OBJECT normal_reach_joint_16 = {
#include "shared/chaos_art/normal_reach_joint_16.inc"
};
static Sint32 normal_reach_joint_15_vertices[131] = {
#include "shared/chaos_art/normal_reach_joint_15_vertices.inc"
};
static NJS_CNK_MODEL normal_reach_joint_15_model = {
#include "shared/chaos_art/normal_reach_joint_15_model.inc"
};
static NJS_CNK_OBJECT normal_reach_joint_15 = {
#include "shared/chaos_art/normal_reach_joint_15.inc"
};
static Sint32 normal_reach_joint_14_vertices[89] = {
#include "shared/chaos_art/normal_reach_joint_14_vertices.inc"
};
static NJS_CNK_MODEL normal_reach_joint_14_model = {
#include "shared/chaos_art/normal_reach_joint_14_model.inc"
};
static NJS_CNK_OBJECT normal_reach_joint_14 = {
#include "shared/chaos_art/normal_reach_joint_14.inc"
};
static Sint32 normal_reach_joint_13_vertices[229] = {
#include "shared/chaos_art/normal_reach_joint_13_vertices.inc"
};
static NJS_CNK_MODEL normal_reach_joint_13_model = {
#include "shared/chaos_art/normal_reach_joint_13_model.inc"
};
static NJS_CNK_OBJECT normal_reach_joint_13 = {
#include "shared/chaos_art/normal_reach_joint_13.inc"
};
static Sint32 normal_reach_joint_12_vertices[278] = {
#include "shared/chaos_art/normal_reach_joint_12_vertices.inc"
};
static NJS_CNK_MODEL normal_reach_joint_12_model = {
#include "shared/chaos_art/normal_reach_joint_12_model.inc"
};
static NJS_CNK_OBJECT normal_reach_joint_12 = {
#include "shared/chaos_art/normal_reach_joint_12.inc"
};
NJS_CNK_OBJECT normal_arm_a = {
#include "shared/chaos_art/normal_arm_a.inc"
};
static Sint16 normal_reach_joint_10_polygons[439] = {
#include "shared/chaos_art/normal_reach_joint_10_polygons.inc"
};
static Sint32 normal_reach_joint_10_vertices[261] = {
#include "shared/chaos_art/normal_reach_joint_10_vertices.inc"
};
static NJS_CNK_MODEL normal_reach_joint_10_model = {
#include "shared/chaos_art/normal_reach_joint_10_model.inc"
};
static NJS_CNK_OBJECT normal_reach_joint_10 = {
#include "shared/chaos_art/normal_reach_joint_10.inc"
};
static Sint16 normal_reach_muzzle_node_polygons[493] = {
#include "shared/chaos_art/normal_reach_muzzle_node_polygons.inc"
};
static Sint32 normal_reach_muzzle_node_vertices[333] = {
#include "shared/chaos_art/normal_reach_muzzle_node_vertices.inc"
};
static NJS_CNK_MODEL normal_reach_muzzle_node_model = {
#include "shared/chaos_art/normal_reach_muzzle_node_model.inc"
};
NJS_CNK_OBJECT normal_reach_muzzle_node = {
#include "shared/chaos_art/normal_reach_muzzle_node.inc"
};
static Sint16 normal_reach_joint_8_polygons[307] = {
#include "shared/chaos_art/normal_reach_joint_8_polygons.inc"
};
static Sint32 normal_reach_joint_8_vertices[219] = {
#include "shared/chaos_art/normal_reach_joint_8_vertices.inc"
};
static NJS_CNK_MODEL normal_reach_joint_8_model = {
#include "shared/chaos_art/normal_reach_joint_8_model.inc"
};
static NJS_CNK_OBJECT normal_reach_joint_8 = {
#include "shared/chaos_art/normal_reach_joint_8.inc"
};
static Sint16 normal_reach_joint_7_polygons[113] = {
#include "shared/chaos_art/normal_reach_joint_7_polygons.inc"
};
static Sint32 normal_reach_joint_7_vertices[69] = {
#include "shared/chaos_art/normal_reach_joint_7_vertices.inc"
};
static NJS_CNK_MODEL normal_reach_joint_7_model = {
#include "shared/chaos_art/normal_reach_joint_7_model.inc"
};
static NJS_CNK_OBJECT normal_reach_joint_7 = {
#include "shared/chaos_art/normal_reach_joint_7.inc"
};
static NJS_CNK_OBJECT normal_reach_joint_6 = {
#include "shared/chaos_art/normal_reach_joint_6.inc"
};
static Sint32 normal_reach_joint_5_vertices[180] = {
#include "shared/chaos_art/normal_reach_joint_5_vertices.inc"
};
static NJS_CNK_MODEL normal_reach_joint_5_model = {
#include "shared/chaos_art/normal_reach_joint_5_model.inc"
};
static NJS_CNK_OBJECT normal_reach_joint_5 = {
#include "shared/chaos_art/normal_reach_joint_5.inc"
};
static Sint32 normal_reach_joint_4_vertices[257] = {
#include "shared/chaos_art/normal_reach_joint_4_vertices.inc"
};
static NJS_CNK_MODEL normal_reach_joint_4_model = {
#include "shared/chaos_art/normal_reach_joint_4_model.inc"
};
static NJS_CNK_OBJECT normal_reach_joint_4 = {
#include "shared/chaos_art/normal_reach_joint_4.inc"
};
static Sint32 normal_reach_joint_3_vertices[222] = {
#include "shared/chaos_art/normal_reach_joint_3_vertices.inc"
};
static NJS_CNK_MODEL normal_reach_joint_3_model = {
#include "shared/chaos_art/normal_reach_joint_3_model.inc"
};
static NJS_CNK_OBJECT normal_reach_joint_3 = {
#include "shared/chaos_art/normal_reach_joint_3.inc"
};
static NJS_CNK_OBJECT normal_reach_joint_2 = {
#include "shared/chaos_art/normal_reach_joint_2.inc"
};
static Sint32 normal_reach_joint_1_vertices[111] = {
#include "shared/chaos_art/normal_reach_joint_1_vertices.inc"
};
static NJS_CNK_MODEL normal_reach_joint_1_model = {
#include "shared/chaos_art/normal_reach_joint_1_model.inc"
};
static NJS_CNK_OBJECT normal_reach_joint_1 = {
#include "shared/chaos_art/normal_reach_joint_1.inc"
};
struct normal_reach_object_polygons_layout normal_reach_object_polygons = {
#include "shared/chaos_art/normal_reach_object_polygons.inc"
};
static Sint32 normal_reach_object_vertices[87] = {
#include "shared/chaos_art/normal_reach_object_vertices.inc"
};
static NJS_CNK_MODEL normal_reach_object_model = {
#include "shared/chaos_art/normal_reach_object_model.inc"
};
NJS_CNK_OBJECT normal_reach_object = {
#include "shared/chaos_art/normal_reach_object.inc"
};
static NJS_MKEY_A reach_fire_up_motion_joint_8_angles[4] = {
#include "shared/chaos_art/reach_fire_up_motion_joint_8_angles.inc"
};
static NJS_MKEY_A reach_fire_up_motion_joint_9_angles[4] = {
#include "shared/chaos_art/reach_fire_up_motion_joint_9_angles.inc"
};
static NJS_MKEY_A reach_fire_up_motion_joint_10_angles[4] = {
#include "shared/chaos_art/reach_fire_up_motion_joint_10_angles.inc"
};
static NJS_MDATA2 reach_fire_up_motion_tracks[31] = {
#include "shared/chaos_art/reach_fire_up_motion_tracks.inc"
};
static NJS_MOTION reach_fire_up_motion = {
#include "shared/chaos_art/reach_fire_up_motion.inc"
};
static NJS_MKEY_A reach_fire_down_motion_joint_9_angles[4] = {
#include "shared/chaos_art/reach_fire_down_motion_joint_9_angles.inc"
};
static NJS_MKEY_A reach_fire_down_motion_joint_10_angles[5] = {
#include "shared/chaos_art/reach_fire_down_motion_joint_10_angles.inc"
};
static NJS_MDATA2 reach_fire_down_motion_tracks[31] = {
#include "shared/chaos_art/reach_fire_down_motion_tracks.inc"
};
static NJS_MOTION reach_fire_down_motion = {
#include "shared/chaos_art/reach_fire_down_motion.inc"
};
static NJS_MKEY_A reach_motion_joint_8_angles[11] = {
#include "shared/chaos_art/reach_motion_joint_8_angles.inc"
};
static NJS_MKEY_A reach_motion_joint_9_angles[11] = {
#include "shared/chaos_art/reach_motion_joint_9_angles.inc"
};
static NJS_MKEY_A reach_motion_joint_10_angles[11] = {
#include "shared/chaos_art/reach_motion_joint_10_angles.inc"
};
static NJS_MDATA2 reach_motion_tracks[31] = {
#include "shared/chaos_art/reach_motion_tracks.inc"
};
static NJS_MOTION reach_motion = {
#include "shared/chaos_art/reach_motion.inc"
};
static Sint16 flash_a_object_polygons[51] = {
#include "shared/chaos_art/flash_a_object_polygons.inc"
};
static Sint32 flash_a_object_vertices[51] = {
#include "shared/chaos_art/flash_a_object_vertices.inc"
};
static NJS_CNK_MODEL flash_a_object_model = {
#include "shared/chaos_art/flash_a_object_model.inc"
};
NJS_CNK_OBJECT flash_a_object = {
#include "shared/chaos_art/flash_a_object.inc"
};
static NJS_MDATA2 flash_a_motion_tracks[1] = {
#include "shared/chaos_art/flash_a_motion_tracks.inc"
};
NJS_MOTION flash_a_motion = {
#include "shared/chaos_art/flash_a_motion.inc"
};
static NJS_POINT3 flash_a_shape_joint_0_points_frame_0[8] = {
#include "shared/chaos_art/flash_a_shape_joint_0_points_frame_0.inc"
};
static NJS_POINT3 flash_a_shape_joint_0_points_frame_3[8] = {
#include "shared/chaos_art/flash_a_shape_joint_0_points_frame_3.inc"
};
static NJS_POINT3 flash_a_shape_joint_0_points_frame_1[8] = {
#include "shared/chaos_art/flash_a_shape_joint_0_points_frame_1.inc"
};
static NJS_POINT3 flash_a_shape_joint_0_points_frame_9[8] = {
#include "shared/chaos_art/flash_a_shape_joint_0_points_frame_9.inc"
};
static NJS_MKEY_P flash_a_shape_joint_0_vertices[10] = {
#include "shared/chaos_art/flash_a_shape_joint_0_vertices.inc"
};
static NJS_MDATA2 flash_a_shape_tracks[1] = {
#include "shared/chaos_art/flash_a_shape_tracks.inc"
};
NJS_MOTION flash_a_shape = {
#include "shared/chaos_art/flash_a_shape.inc"
};
static Sint16 flash_b_object_polygons[51] = {
#include "shared/chaos_art/flash_b_object_polygons.inc"
};
static Sint32 flash_b_object_vertices[51] = {
#include "shared/chaos_art/flash_b_object_vertices.inc"
};
static NJS_CNK_MODEL flash_b_object_model = {
#include "shared/chaos_art/flash_b_object_model.inc"
};
NJS_CNK_OBJECT flash_b_object = {
#include "shared/chaos_art/flash_b_object.inc"
};
static NJS_MDATA2 flash_b_motion_tracks[1] = {
#include "shared/chaos_art/flash_b_motion_tracks.inc"
};
NJS_MOTION flash_b_motion = {
#include "shared/chaos_art/flash_b_motion.inc"
};
static NJS_POINT3 flash_b_shape_joint_0_points_frame_0[8] = {
#include "shared/chaos_art/flash_b_shape_joint_0_points_frame_0.inc"
};
static NJS_POINT3 flash_b_shape_joint_0_points_frame_3[8] = {
#include "shared/chaos_art/flash_b_shape_joint_0_points_frame_3.inc"
};
static NJS_POINT3 flash_b_shape_joint_0_points_frame_1[8] = {
#include "shared/chaos_art/flash_b_shape_joint_0_points_frame_1.inc"
};
static NJS_POINT3 flash_b_shape_joint_0_points_frame_9[8] = {
#include "shared/chaos_art/flash_b_shape_joint_0_points_frame_9.inc"
};
static NJS_MKEY_P flash_b_shape_joint_0_vertices[10] = {
#include "shared/chaos_art/flash_b_shape_joint_0_vertices.inc"
};
static NJS_MDATA2 flash_b_shape_tracks[1] = {
#include "shared/chaos_art/flash_b_shape_tracks.inc"
};
NJS_MOTION flash_b_shape = {
#include "shared/chaos_art/flash_b_shape.inc"
};
static Sint16 float_joint_4_polygons[439] = {
#include "shared/chaos_art/float_joint_4_polygons.inc"
};
static Sint32 float_joint_4_vertices[261] = {
#include "shared/chaos_art/float_joint_4_vertices.inc"
};
static NJS_CNK_MODEL float_joint_4_model = {
#include "shared/chaos_art/float_joint_4_model.inc"
};
static NJS_CNK_OBJECT float_joint_4 = {
#include "shared/chaos_art/float_joint_4.inc"
};
static Sint16 float_joint_3_polygons[493] = {
#include "shared/chaos_art/float_joint_3_polygons.inc"
};
static Sint32 float_joint_3_vertices[333] = {
#include "shared/chaos_art/float_joint_3_vertices.inc"
};
static NJS_CNK_MODEL float_joint_3_model = {
#include "shared/chaos_art/float_joint_3_model.inc"
};
static NJS_CNK_OBJECT float_joint_3 = {
#include "shared/chaos_art/float_joint_3.inc"
};
static Sint16 float_joint_2_polygons[307] = {
#include "shared/chaos_art/float_joint_2_polygons.inc"
};
static Sint32 float_joint_2_vertices[219] = {
#include "shared/chaos_art/float_joint_2_vertices.inc"
};
static NJS_CNK_MODEL float_joint_2_model = {
#include "shared/chaos_art/float_joint_2_model.inc"
};
static NJS_CNK_OBJECT float_joint_2 = {
#include "shared/chaos_art/float_joint_2.inc"
};
static Sint16 float_joint_1_polygons[113] = {
#include "shared/chaos_art/float_joint_1_polygons.inc"
};
static Sint32 float_joint_1_vertices[69] = {
#include "shared/chaos_art/float_joint_1_vertices.inc"
};
static NJS_CNK_MODEL float_joint_1_model = {
#include "shared/chaos_art/float_joint_1_model.inc"
};
static NJS_CNK_OBJECT float_joint_1 = {
#include "shared/chaos_art/float_joint_1.inc"
};
struct float_object_polygons_layout float_object_polygons = {
#include "shared/chaos_art/float_object_polygons.inc"
};
static Sint32 float_object_vertices[411] = {
#include "shared/chaos_art/float_object_vertices.inc"
};
static NJS_CNK_MODEL float_object_model = {
#include "shared/chaos_art/float_object_model.inc"
};
NJS_CNK_OBJECT float_object[1] = {
#include "shared/chaos_art/float_object.inc"
};
static NJS_POINT3 float_shape_joint_0_points_frame_0[68] = {
#include "shared/chaos_art/float_shape_joint_0_points_frame_0.inc"
};
static NJS_POINT3 float_shape_joint_0_points_frame_6[68] = {
#include "shared/chaos_art/float_shape_joint_0_points_frame_6.inc"
};
static NJS_POINT3 float_shape_joint_0_points_frame_7[68] = {
#include "shared/chaos_art/float_shape_joint_0_points_frame_7.inc"
};
static NJS_POINT3 float_shape_joint_0_points_frame_8[68] = {
#include "shared/chaos_art/float_shape_joint_0_points_frame_8.inc"
};
static NJS_POINT3 float_shape_joint_0_points_frame_9[68] = {
#include "shared/chaos_art/float_shape_joint_0_points_frame_9.inc"
};
static NJS_POINT3 float_shape_joint_0_points_frame_10[68] = {
#include "shared/chaos_art/float_shape_joint_0_points_frame_10.inc"
};
static NJS_MKEY_P float_shape_joint_0_vertices[6] = {
#include "shared/chaos_art/float_shape_joint_0_vertices.inc"
};
static NJS_MDATA2 float_shape_tracks[5] = {
#include "shared/chaos_art/float_shape_tracks.inc"
};
NJS_MOTION float_shape = {
#include "shared/chaos_art/float_shape.inc"
};
static NJS_MKEY_A float_motion_joint_2_angles[11] = {
#include "shared/chaos_art/float_motion_joint_2_angles.inc"
};
static NJS_MKEY_A float_motion_joint_3_angles[6] = {
#include "shared/chaos_art/float_motion_joint_3_angles.inc"
};
static NJS_MKEY_A float_motion_joint_4_angles[11] = {
#include "shared/chaos_art/float_motion_joint_4_angles.inc"
};
static NJS_MDATA2 float_motion_tracks[5] = {
#include "shared/chaos_art/float_motion_tracks.inc"
};
NJS_MOTION float_motion = {
#include "shared/chaos_art/float_motion.inc"
};
static Sint16 float_reach_joint_24_polygons[439] = {
#include "shared/chaos_art/float_reach_joint_24_polygons.inc"
};
static Sint32 float_reach_joint_24_vertices[261] = {
#include "shared/chaos_art/float_reach_joint_24_vertices.inc"
};
static NJS_CNK_MODEL float_reach_joint_24_model = {
#include "shared/chaos_art/float_reach_joint_24_model.inc"
};
static NJS_CNK_OBJECT float_reach_joint_24 = {
#include "shared/chaos_art/float_reach_joint_24.inc"
};
static Sint16 float_reach_muzzle_node_polygons[493] = {
#include "shared/chaos_art/float_reach_muzzle_node_polygons.inc"
};
static Sint32 float_reach_muzzle_node_vertices[333] = {
#include "shared/chaos_art/float_reach_muzzle_node_vertices.inc"
};
static NJS_CNK_MODEL float_reach_muzzle_node_model = {
#include "shared/chaos_art/float_reach_muzzle_node_model.inc"
};
NJS_CNK_OBJECT float_reach_muzzle_node = {
#include "shared/chaos_art/float_reach_muzzle_node.inc"
};
static Sint16 float_reach_joint_22_polygons[307] = {
#include "shared/chaos_art/float_reach_joint_22_polygons.inc"
};
static Sint32 float_reach_joint_22_vertices[219] = {
#include "shared/chaos_art/float_reach_joint_22_vertices.inc"
};
static NJS_CNK_MODEL float_reach_joint_22_model = {
#include "shared/chaos_art/float_reach_joint_22_model.inc"
};
static NJS_CNK_OBJECT float_reach_joint_22 = {
#include "shared/chaos_art/float_reach_joint_22.inc"
};
static Sint16 float_reach_joint_21_polygons[113] = {
#include "shared/chaos_art/float_reach_joint_21_polygons.inc"
};
static Sint32 float_reach_joint_21_vertices[69] = {
#include "shared/chaos_art/float_reach_joint_21_vertices.inc"
};
static NJS_CNK_MODEL float_reach_joint_21_model = {
#include "shared/chaos_art/float_reach_joint_21_model.inc"
};
static NJS_CNK_OBJECT float_reach_joint_21 = {
#include "shared/chaos_art/float_reach_joint_21.inc"
};
static NJS_CNK_OBJECT float_reach_joint_20 = {
#include "shared/chaos_art/float_reach_joint_20.inc"
};
static Sint16 float_reach_joint_19_polygons[2] = {
#include "shared/chaos_art/float_reach_joint_19_polygons.inc"
};
static Sint32 float_reach_joint_19_vertices[153] = {
#include "shared/chaos_art/float_reach_joint_19_vertices.inc"
};
static NJS_CNK_MODEL float_reach_joint_19_model = {
#include "shared/chaos_art/float_reach_joint_19_model.inc"
};
static NJS_CNK_OBJECT float_reach_joint_19 = {
#include "shared/chaos_art/float_reach_joint_19.inc"
};
static Sint32 float_reach_joint_18_vertices[201] = {
#include "shared/chaos_art/float_reach_joint_18_vertices.inc"
};
static NJS_CNK_MODEL float_reach_joint_18_model = {
#include "shared/chaos_art/float_reach_joint_18_model.inc"
};
static NJS_CNK_OBJECT float_reach_joint_18 = {
#include "shared/chaos_art/float_reach_joint_18.inc"
};
static Sint32 float_reach_joint_17_vertices[131] = {
#include "shared/chaos_art/float_reach_joint_17_vertices.inc"
};
static NJS_CNK_MODEL float_reach_joint_17_model = {
#include "shared/chaos_art/float_reach_joint_17_model.inc"
};
static NJS_CNK_OBJECT float_reach_joint_17 = {
#include "shared/chaos_art/float_reach_joint_17.inc"
};
static Sint32 float_reach_joint_16_vertices[131] = {
#include "shared/chaos_art/float_reach_joint_16_vertices.inc"
};
static NJS_CNK_MODEL float_reach_joint_16_model = {
#include "shared/chaos_art/float_reach_joint_16_model.inc"
};
static NJS_CNK_OBJECT float_reach_joint_16 = {
#include "shared/chaos_art/float_reach_joint_16.inc"
};
static Sint32 float_reach_joint_15_vertices[173] = {
#include "shared/chaos_art/float_reach_joint_15_vertices.inc"
};
static NJS_CNK_MODEL float_reach_joint_15_model = {
#include "shared/chaos_art/float_reach_joint_15_model.inc"
};
static NJS_CNK_OBJECT float_reach_joint_15 = {
#include "shared/chaos_art/float_reach_joint_15.inc"
};
static Sint32 float_reach_joint_14_vertices[187] = {
#include "shared/chaos_art/float_reach_joint_14_vertices.inc"
};
static NJS_CNK_MODEL float_reach_joint_14_model = {
#include "shared/chaos_art/float_reach_joint_14_model.inc"
};
static NJS_CNK_OBJECT float_reach_joint_14 = {
#include "shared/chaos_art/float_reach_joint_14.inc"
};
static Sint32 float_reach_joint_13_vertices[138] = {
#include "shared/chaos_art/float_reach_joint_13_vertices.inc"
};
static NJS_CNK_MODEL float_reach_joint_13_model = {
#include "shared/chaos_art/float_reach_joint_13_model.inc"
};
static NJS_CNK_OBJECT float_reach_joint_13 = {
#include "shared/chaos_art/float_reach_joint_13.inc"
};
static Sint32 float_reach_joint_12_vertices[299] = {
#include "shared/chaos_art/float_reach_joint_12_vertices.inc"
};
static NJS_CNK_MODEL float_reach_joint_12_model = {
#include "shared/chaos_art/float_reach_joint_12_model.inc"
};
static NJS_CNK_OBJECT float_reach_joint_12 = {
#include "shared/chaos_art/float_reach_joint_12.inc"
};
NJS_CNK_OBJECT float_arm_a[1] = {
#include "shared/chaos_art/float_arm_a.inc"
};
static NJS_CNK_OBJECT float_reach_joint_10 = {
#include "shared/chaos_art/float_reach_joint_10.inc"
};
static Sint32 float_reach_joint_9_vertices[153] = {
#include "shared/chaos_art/float_reach_joint_9_vertices.inc"
};
static NJS_CNK_MODEL float_reach_joint_9_model = {
#include "shared/chaos_art/float_reach_joint_9_model.inc"
};
static NJS_CNK_OBJECT float_reach_joint_9 = {
#include "shared/chaos_art/float_reach_joint_9.inc"
};
static Sint32 float_reach_joint_8_vertices[201] = {
#include "shared/chaos_art/float_reach_joint_8_vertices.inc"
};
static NJS_CNK_MODEL float_reach_joint_8_model = {
#include "shared/chaos_art/float_reach_joint_8_model.inc"
};
static NJS_CNK_OBJECT float_reach_joint_8 = {
#include "shared/chaos_art/float_reach_joint_8.inc"
};
static Sint32 float_reach_joint_7_vertices[131] = {
#include "shared/chaos_art/float_reach_joint_7_vertices.inc"
};
static NJS_CNK_MODEL float_reach_joint_7_model = {
#include "shared/chaos_art/float_reach_joint_7_model.inc"
};
static NJS_CNK_OBJECT float_reach_joint_7 = {
#include "shared/chaos_art/float_reach_joint_7.inc"
};
static Sint32 float_reach_joint_6_vertices[131] = {
#include "shared/chaos_art/float_reach_joint_6_vertices.inc"
};
static NJS_CNK_MODEL float_reach_joint_6_model = {
#include "shared/chaos_art/float_reach_joint_6_model.inc"
};
static NJS_CNK_OBJECT float_reach_joint_6 = {
#include "shared/chaos_art/float_reach_joint_6.inc"
};
static Sint32 float_reach_joint_5_vertices[173] = {
#include "shared/chaos_art/float_reach_joint_5_vertices.inc"
};
static NJS_CNK_MODEL float_reach_joint_5_model = {
#include "shared/chaos_art/float_reach_joint_5_model.inc"
};
static NJS_CNK_OBJECT float_reach_joint_5 = {
#include "shared/chaos_art/float_reach_joint_5.inc"
};
static Sint32 float_reach_joint_4_vertices[187] = {
#include "shared/chaos_art/float_reach_joint_4_vertices.inc"
};
static NJS_CNK_MODEL float_reach_joint_4_model = {
#include "shared/chaos_art/float_reach_joint_4_model.inc"
};
static NJS_CNK_OBJECT float_reach_joint_4 = {
#include "shared/chaos_art/float_reach_joint_4.inc"
};
static Sint32 float_reach_joint_3_vertices[138] = {
#include "shared/chaos_art/float_reach_joint_3_vertices.inc"
};
static NJS_CNK_MODEL float_reach_joint_3_model = {
#include "shared/chaos_art/float_reach_joint_3_model.inc"
};
static NJS_CNK_OBJECT float_reach_joint_3 = {
#include "shared/chaos_art/float_reach_joint_3.inc"
};
static Sint32 float_reach_joint_2_vertices[299] = {
#include "shared/chaos_art/float_reach_joint_2_vertices.inc"
};
static NJS_CNK_MODEL float_reach_joint_2_model = {
#include "shared/chaos_art/float_reach_joint_2_model.inc"
};
static NJS_CNK_OBJECT float_reach_joint_2 = {
#include "shared/chaos_art/float_reach_joint_2.inc"
};
NJS_CNK_OBJECT float_arm_b[1] = {
#include "shared/chaos_art/float_arm_b.inc"
};
struct float_reach_object_polygons_layout float_reach_object_polygons = {
#include "shared/chaos_art/float_reach_object_polygons.inc"
};
static Sint32 float_reach_object_vertices[679] = {
#include "shared/chaos_art/float_reach_object_vertices.inc"
};
static NJS_CNK_MODEL float_reach_object_model = {
#include "shared/chaos_art/float_reach_object_model.inc"
};
NJS_CNK_OBJECT float_reach_object = {
#include "shared/chaos_art/float_reach_object.inc"
};
static NJS_MDATA2 float_reach_fire_motion_tracks[25] = {
#include "shared/chaos_art/float_reach_fire_motion_tracks.inc"
};
static NJS_MOTION float_reach_fire_motion = {
#include "shared/chaos_art/float_reach_fire_motion.inc"
};
static NJS_MKEY_A float_reach_motion_joint_22_angles[7] = {
#include "shared/chaos_art/float_reach_motion_joint_22_angles.inc"
};
static NJS_MKEY_A float_reach_motion_joint_23_angles[6] = {
#include "shared/chaos_art/float_reach_motion_joint_23_angles.inc"
};
static NJS_MKEY_A float_reach_motion_joint_24_angles[7] = {
#include "shared/chaos_art/float_reach_motion_joint_24_angles.inc"
};
static NJS_MDATA2 float_reach_motion_tracks[25] = {
#include "shared/chaos_art/float_reach_motion_tracks.inc"
};
static NJS_MOTION float_reach_motion = {
#include "shared/chaos_art/float_reach_motion.inc"
};
static NJS_MKEY_A spin_motion_joint_3_angles[17] = {
#include "shared/chaos_art/spin_motion_joint_3_angles.inc"
};
static NJS_MKEY_A spin_motion_joint_4_angles[17] = {
#include "shared/chaos_art/spin_motion_joint_4_angles.inc"
};
static NJS_MKEY_A spin_motion_joint_5_angles[17] = {
#include "shared/chaos_art/spin_motion_joint_5_angles.inc"
};
static NJS_MKEY_A spin_motion_joint_8_angles[17] = {
#include "shared/chaos_art/spin_motion_joint_8_angles.inc"
};
static NJS_MKEY_A spin_motion_joint_9_angles[17] = {
#include "shared/chaos_art/spin_motion_joint_9_angles.inc"
};
static NJS_MKEY_A spin_motion_joint_10_angles[17] = {
#include "shared/chaos_art/spin_motion_joint_10_angles.inc"
};
static NJS_MDATA2 spin_motion_tracks[11] = {
#include "shared/chaos_art/spin_motion_tracks.inc"
};
static NJS_MOTION spin_motion = {
#include "shared/chaos_art/spin_motion.inc"
};
static NJS_MDATA2 spin_shape_tracks[11] = {
#include "shared/chaos_art/spin_shape_tracks.inc"
};
static NJS_MOTION spin_shape = {
#include "shared/chaos_art/spin_shape.inc"
};
struct piece_a_strips_layout piece_a_strips[1] = {
#include "shared/chaos_art/piece_a_strips.inc"
};
static Sint32 piece_a_object_vertices[291] = {
#include "shared/chaos_art/piece_a_object_vertices.inc"
};
static NJS_CNK_MODEL piece_a_object_model = {
#include "shared/chaos_art/piece_a_object_model.inc"
};
NJS_CNK_OBJECT piece_a_object = {
#include "shared/chaos_art/piece_a_object.inc"
};
struct piece_b_strips_layout piece_b_strips[1] = {
#include "shared/chaos_art/piece_b_strips.inc"
};
static Sint32 piece_b_object_vertices[299] = {
#include "shared/chaos_art/piece_b_object_vertices.inc"
};
static NJS_CNK_MODEL piece_b_object_model = {
#include "shared/chaos_art/piece_b_object_model.inc"
};
NJS_CNK_OBJECT piece_b_object = {
#include "shared/chaos_art/piece_b_object.inc"
};
struct piece_polygons_layout piece_polygons = {
#include "shared/chaos_art/piece_polygons.inc"
};
static Sint32 piece_vertices[235] = {
#include "shared/chaos_art/piece_vertices.inc"
};
NJS_CNK_MODEL piece_model = {
#include "shared/chaos_art/piece_model.inc"
};
static NJS_CNK_OBJECT piece_fallback_object = {
#include "shared/chaos_art/piece_fallback_object.inc"
};
static Sint16 shed_joint_4_polygons[439] = {
#include "shared/chaos_art/shed_joint_4_polygons.inc"
};
static Sint32 shed_joint_4_vertices[261] = {
#include "shared/chaos_art/shed_joint_4_vertices.inc"
};
static NJS_CNK_MODEL shed_joint_4_model = {
#include "shared/chaos_art/shed_joint_4_model.inc"
};
static NJS_CNK_OBJECT shed_joint_4 = {
#include "shared/chaos_art/shed_joint_4.inc"
};
static Sint16 shed_joint_3_polygons[493] = {
#include "shared/chaos_art/shed_joint_3_polygons.inc"
};
static Sint32 shed_joint_3_vertices[333] = {
#include "shared/chaos_art/shed_joint_3_vertices.inc"
};
static NJS_CNK_MODEL shed_joint_3_model = {
#include "shared/chaos_art/shed_joint_3_model.inc"
};
static NJS_CNK_OBJECT shed_joint_3 = {
#include "shared/chaos_art/shed_joint_3.inc"
};
static Sint16 shed_joint_2_polygons[307] = {
#include "shared/chaos_art/shed_joint_2_polygons.inc"
};
static Sint32 shed_joint_2_vertices[219] = {
#include "shared/chaos_art/shed_joint_2_vertices.inc"
};
static NJS_CNK_MODEL shed_joint_2_model = {
#include "shared/chaos_art/shed_joint_2_model.inc"
};
static NJS_CNK_OBJECT shed_joint_2 = {
#include "shared/chaos_art/shed_joint_2.inc"
};
static Sint16 shed_joint_1_polygons[113] = {
#include "shared/chaos_art/shed_joint_1_polygons.inc"
};
static Sint32 shed_joint_1_vertices[69] = {
#include "shared/chaos_art/shed_joint_1_vertices.inc"
};
static NJS_CNK_MODEL shed_joint_1_model = {
#include "shared/chaos_art/shed_joint_1_model.inc"
};
static NJS_CNK_OBJECT shed_joint_1 = {
#include "shared/chaos_art/shed_joint_1.inc"
};
struct shed_object_polygons_layout shed_object_polygons = {
#include "shared/chaos_art/shed_object_polygons.inc"
};
static Sint32 shed_object_vertices[411] = {
#include "shared/chaos_art/shed_object_vertices.inc"
};
static NJS_CNK_MODEL shed_object_model = {
#include "shared/chaos_art/shed_object_model.inc"
};
NJS_CNK_OBJECT shed_object = {
#include "shared/chaos_art/shed_object.inc"
};
static NJS_MKEY_A shed_start_motion_joint_2_angles[3] = {
#include "shared/chaos_art/shed_start_motion_joint_2_angles.inc"
};
static NJS_MKEY_A shed_start_motion_joint_3_angles[3] = {
#include "shared/chaos_art/shed_start_motion_joint_3_angles.inc"
};
static NJS_MKEY_A shed_start_motion_joint_4_angles[3] = {
#include "shared/chaos_art/shed_start_motion_joint_4_angles.inc"
};
static NJS_MDATA2 shed_start_motion_tracks[5] = {
#include "shared/chaos_art/shed_start_motion_tracks.inc"
};
static NJS_MOTION shed_start_motion = {
#include "shared/chaos_art/shed_start_motion.inc"
};
static NJS_POINT3 shed_shape_joint_0_points_frame_0[68] = {
#include "shared/chaos_art/shed_shape_joint_0_points_frame_0.inc"
};
static NJS_POINT3 shed_shape_joint_0_points_frame_1[68] = {
#include "shared/chaos_art/shed_shape_joint_0_points_frame_1.inc"
};
static NJS_POINT3 shed_shape_joint_0_points_frame_2[68] = {
#include "shared/chaos_art/shed_shape_joint_0_points_frame_2.inc"
};
static NJS_MKEY_P shed_shape_joint_0_vertices[3] = {
#include "shared/chaos_art/shed_shape_joint_0_vertices.inc"
};
static NJS_MDATA2 shed_shape_tracks[5] = {
#include "shared/chaos_art/shed_shape_tracks.inc"
};
NJS_MOTION shed_shape = {
#include "shared/chaos_art/shed_shape.inc"
};
static NJS_MKEY_A shed_end_motion_joint_2_angles[3] = {
#include "shared/chaos_art/shed_end_motion_joint_2_angles.inc"
};
static NJS_MKEY_A shed_end_motion_joint_4_angles[3] = {
#include "shared/chaos_art/shed_end_motion_joint_4_angles.inc"
};
static NJS_MDATA2 shed_end_motion_tracks[5] = {
#include "shared/chaos_art/shed_end_motion_tracks.inc"
};
static NJS_MOTION shed_end_motion = {
#include "shared/chaos_art/shed_end_motion.inc"
};
static NJS_POINT3 shed_end_shape_joint_0_points_frame_0[68] = {
#include "shared/chaos_art/shed_end_shape_joint_0_points_frame_0.inc"
};
static NJS_POINT3 shed_end_shape_joint_0_points_frame_1[68] = {
#include "shared/chaos_art/shed_end_shape_joint_0_points_frame_1.inc"
};
static NJS_POINT3 shed_end_shape_joint_0_points_frame_2[68] = {
#include "shared/chaos_art/shed_end_shape_joint_0_points_frame_2.inc"
};
static NJS_MKEY_P shed_end_shape_joint_0_vertices[3] = {
#include "shared/chaos_art/shed_end_shape_joint_0_vertices.inc"
};
static NJS_MDATA2 shed_end_shape_tracks[5] = {
#include "shared/chaos_art/shed_end_shape_tracks.inc"
};
NJS_MOTION shed_end_shape = {
#include "shared/chaos_art/shed_end_shape.inc"
};
static Sint16 spent_joint_3_polygons[439] = {
#include "shared/chaos_art/spent_joint_3_polygons.inc"
};
static Sint32 spent_joint_3_vertices[261] = {
#include "shared/chaos_art/spent_joint_3_vertices.inc"
};
static NJS_CNK_MODEL spent_joint_3_model = {
#include "shared/chaos_art/spent_joint_3_model.inc"
};
static NJS_CNK_OBJECT spent_joint_3 = {
#include "shared/chaos_art/spent_joint_3.inc"
};
static Sint16 spent_joint_2_polygons[493] = {
#include "shared/chaos_art/spent_joint_2_polygons.inc"
};
static Sint32 spent_joint_2_vertices[333] = {
#include "shared/chaos_art/spent_joint_2_vertices.inc"
};
static NJS_CNK_MODEL spent_joint_2_model = {
#include "shared/chaos_art/spent_joint_2_model.inc"
};
static NJS_CNK_OBJECT spent_joint_2 = {
#include "shared/chaos_art/spent_joint_2.inc"
};
static Sint16 spent_joint_1_polygons[307] = {
#include "shared/chaos_art/spent_joint_1_polygons.inc"
};
static Sint32 spent_joint_1_vertices[219] = {
#include "shared/chaos_art/spent_joint_1_vertices.inc"
};
static NJS_CNK_MODEL spent_joint_1_model = {
#include "shared/chaos_art/spent_joint_1_model.inc"
};
static NJS_CNK_OBJECT spent_joint_1 = {
#include "shared/chaos_art/spent_joint_1.inc"
};
static Sint16 spent_object_polygons[113] = {
#include "shared/chaos_art/spent_object_polygons.inc"
};
static Sint32 spent_object_vertices[69] = {
#include "shared/chaos_art/spent_object_vertices.inc"
};
static NJS_CNK_MODEL spent_object_model = {
#include "shared/chaos_art/spent_object_model.inc"
};
NJS_CNK_OBJECT spent_object = {
#include "shared/chaos_art/spent_object.inc"
};
static NJS_MKEY_A spent_motion_joint_1_angles[5] = {
#include "shared/chaos_art/spent_motion_joint_1_angles.inc"
};
static NJS_MKEY_A spent_motion_joint_2_angles[7] = {
#include "shared/chaos_art/spent_motion_joint_2_angles.inc"
};
static NJS_MKEY_A spent_motion_joint_3_angles[5] = {
#include "shared/chaos_art/spent_motion_joint_3_angles.inc"
};
static NJS_MDATA2 spent_motion_tracks[4] = {
#include "shared/chaos_art/spent_motion_tracks.inc"
};
NJS_MOTION spent_motion = {
#include "shared/chaos_art/spent_motion.inc"
};
static NJS_MKEY_A open_guard_motion_joint_3_angles[4] = {
#include "shared/chaos_art/open_guard_motion_joint_3_angles.inc"
};
static NJS_MKEY_A open_guard_motion_joint_4_angles[4] = {
#include "shared/chaos_art/open_guard_motion_joint_4_angles.inc"
};
static NJS_MKEY_A open_guard_motion_joint_5_angles[4] = {
#include "shared/chaos_art/open_guard_motion_joint_5_angles.inc"
};
static NJS_MKEY_A open_guard_motion_joint_8_angles[4] = {
#include "shared/chaos_art/open_guard_motion_joint_8_angles.inc"
};
static NJS_MKEY_A open_guard_motion_joint_9_angles[4] = {
#include "shared/chaos_art/open_guard_motion_joint_9_angles.inc"
};
static NJS_MKEY_A open_guard_motion_joint_10_angles[4] = {
#include "shared/chaos_art/open_guard_motion_joint_10_angles.inc"
};
static NJS_MDATA2 open_guard_motion_tracks[11] = {
#include "shared/chaos_art/open_guard_motion_tracks.inc"
};
static NJS_MOTION open_guard_motion = {
#include "shared/chaos_art/open_guard_motion.inc"
};
static Sint16 guard_joint_4_polygons[439] = {
#include "shared/chaos_art/guard_joint_4_polygons.inc"
};
static Sint32 guard_joint_4_vertices[261] = {
#include "shared/chaos_art/guard_joint_4_vertices.inc"
};
static NJS_CNK_MODEL guard_joint_4_model = {
#include "shared/chaos_art/guard_joint_4_model.inc"
};
static NJS_CNK_OBJECT guard_joint_4 = {
#include "shared/chaos_art/guard_joint_4.inc"
};
static Sint16 guard_joint_3_polygons[493] = {
#include "shared/chaos_art/guard_joint_3_polygons.inc"
};
static Sint32 guard_joint_3_vertices[333] = {
#include "shared/chaos_art/guard_joint_3_vertices.inc"
};
static NJS_CNK_MODEL guard_joint_3_model = {
#include "shared/chaos_art/guard_joint_3_model.inc"
};
static NJS_CNK_OBJECT guard_joint_3 = {
#include "shared/chaos_art/guard_joint_3.inc"
};
static Sint16 guard_joint_2_polygons[307] = {
#include "shared/chaos_art/guard_joint_2_polygons.inc"
};
static Sint32 guard_joint_2_vertices[219] = {
#include "shared/chaos_art/guard_joint_2_vertices.inc"
};
static NJS_CNK_MODEL guard_joint_2_model = {
#include "shared/chaos_art/guard_joint_2_model.inc"
};
static NJS_CNK_OBJECT guard_joint_2 = {
#include "shared/chaos_art/guard_joint_2.inc"
};
static Sint16 guard_joint_1_polygons[113] = {
#include "shared/chaos_art/guard_joint_1_polygons.inc"
};
static Sint32 guard_joint_1_vertices[69] = {
#include "shared/chaos_art/guard_joint_1_vertices.inc"
};
static NJS_CNK_MODEL guard_joint_1_model = {
#include "shared/chaos_art/guard_joint_1_model.inc"
};
static NJS_CNK_OBJECT guard_joint_1 = {
#include "shared/chaos_art/guard_joint_1.inc"
};
struct guard_object_polygons_layout guard_object_polygons = {
#include "shared/chaos_art/guard_object_polygons.inc"
};
static Sint32 guard_object_vertices[399] = {
#include "shared/chaos_art/guard_object_vertices.inc"
};
static NJS_CNK_MODEL guard_object_model = {
#include "shared/chaos_art/guard_object_model.inc"
};
NJS_CNK_OBJECT guard_object = {
#include "shared/chaos_art/guard_object.inc"
};
static NJS_MKEY_F close_shell_motion_joint_1_position[12] = {
#include "shared/chaos_art/close_shell_motion_joint_1_position.inc"
};
static NJS_MKEY_A close_shell_motion_joint_1_angles[12] = {
#include "shared/chaos_art/close_shell_motion_joint_1_angles.inc"
};
static NJS_MKEY_A close_shell_motion_joint_2_angles[12] = {
#include "shared/chaos_art/close_shell_motion_joint_2_angles.inc"
};
static NJS_MKEY_F close_shell_motion_joint_3_position[12] = {
#include "shared/chaos_art/close_shell_motion_joint_3_position.inc"
};
static NJS_MKEY_A close_shell_motion_joint_3_angles[12] = {
#include "shared/chaos_art/close_shell_motion_joint_3_angles.inc"
};
static NJS_MKEY_A close_shell_motion_joint_4_angles[11] = {
#include "shared/chaos_art/close_shell_motion_joint_4_angles.inc"
};
static NJS_MDATA2 close_shell_motion_tracks[5] = {
#include "shared/chaos_art/close_shell_motion_tracks.inc"
};
static NJS_MOTION close_shell_motion = {
#include "shared/chaos_art/close_shell_motion.inc"
};
static NJS_POINT3 guard_shape_joint_0_points_frame_0[66] = {
#include "shared/chaos_art/guard_shape_joint_0_points_frame_0.inc"
};
static NJS_POINT3 guard_shape_joint_0_points_frame_3[66] = {
#include "shared/chaos_art/guard_shape_joint_0_points_frame_3.inc"
};
static NJS_POINT3 guard_shape_joint_0_points_frame_6[66] = {
#include "shared/chaos_art/guard_shape_joint_0_points_frame_6.inc"
};
static NJS_POINT3 guard_shape_joint_0_points_frame_7[66] = {
#include "shared/chaos_art/guard_shape_joint_0_points_frame_7.inc"
};
static NJS_POINT3 guard_shape_joint_0_points_frame_9[66] = {
#include "shared/chaos_art/guard_shape_joint_0_points_frame_9.inc"
};
static NJS_POINT3 guard_shape_joint_0_points_frame_10[66] = {
#include "shared/chaos_art/guard_shape_joint_0_points_frame_10.inc"
};
static NJS_POINT3 guard_shape_joint_0_points_frame_11[66] = {
#include "shared/chaos_art/guard_shape_joint_0_points_frame_11.inc"
};
static NJS_MKEY_P guard_shape_joint_0_vertices[7] = {
#include "shared/chaos_art/guard_shape_joint_0_vertices.inc"
};
static NJS_MDATA2 guard_shape_tracks[5] = {
#include "shared/chaos_art/guard_shape_tracks.inc"
};
NJS_MOTION guard_shape = {
#include "shared/chaos_art/guard_shape.inc"
};
static NJS_MKEY_F guard_motion_joint_1_position[7] = {
#include "shared/chaos_art/guard_motion_joint_1_position.inc"
};
static NJS_MKEY_A guard_motion_joint_1_angles[2] = {
#include "shared/chaos_art/guard_motion_joint_1_angles.inc"
};
static NJS_MKEY_A guard_motion_joint_2_angles[7] = {
#include "shared/chaos_art/guard_motion_joint_2_angles.inc"
};
static NJS_MKEY_F guard_motion_joint_3_position[2] = {
#include "shared/chaos_art/guard_motion_joint_3_position.inc"
};
static NJS_MKEY_A guard_motion_joint_3_angles[7] = {
#include "shared/chaos_art/guard_motion_joint_3_angles.inc"
};
static NJS_MKEY_A guard_motion_joint_4_angles[7] = {
#include "shared/chaos_art/guard_motion_joint_4_angles.inc"
};
static NJS_MDATA2 guard_motion_tracks[5] = {
#include "shared/chaos_art/guard_motion_tracks.inc"
};
NJS_MOTION guard_motion = {
#include "shared/chaos_art/guard_motion.inc"
};
static NJS_POINT3 guard_shut_shape_joint_0_points_frame_0[66] = {
#include "shared/chaos_art/guard_shut_shape_joint_0_points_frame_0.inc"
};
static NJS_POINT3 guard_shut_shape_joint_0_points_frame_1[66] = {
#include "shared/chaos_art/guard_shut_shape_joint_0_points_frame_1.inc"
};
static NJS_POINT3 guard_shut_shape_joint_0_points_frame_2[66] = {
#include "shared/chaos_art/guard_shut_shape_joint_0_points_frame_2.inc"
};
static NJS_POINT3 guard_shut_shape_joint_0_points_frame_6[66] = {
#include "shared/chaos_art/guard_shut_shape_joint_0_points_frame_6.inc"
};
static NJS_MKEY_P guard_shut_shape_joint_0_vertices[4] = {
#include "shared/chaos_art/guard_shut_shape_joint_0_vertices.inc"
};
static NJS_MDATA2 guard_shut_shape_tracks[5] = {
#include "shared/chaos_art/guard_shut_shape_tracks.inc"
};
NJS_MOTION guard_shut_shape = {
#include "shared/chaos_art/guard_shut_shape.inc"
};
/* clang-format off */
/* an arm node's shape. enemy_chaos.c's arms_fit_shapes sets its center,
 * size_y and angles, and turns it on */
#define NODE_SHAPE                                                             \
    {0, SHAPE_CAPSULE, PUSH_FIRM(PUSH_ALL_BUT_PICKUPS),                        \
     SHAPE_ATTACK_LEVEL(2) | SHAPE_GUARD_LEVEL(2) | SHAPE_HURTS_PLAYERS,       \
     SHAPE_OFF | SHAPE_IN_WORLD | SHAPE_TURN_YZX | SHAPE_HURTABLE |            \
         SHAPE_IGNORE_ENEMIES | SHAPE_IGNORE_OBJECTS | SHAPE_IGNORE_UNK_9,     \
     {0.0F, 0.0F, 0.0F}, 3.0F, 0.0F, 0.0F, 0, {0, 0, 0}}
/* an arm's shapes, one for each of the eight nodes that arms_pose turns */
#define ARM_SHAPES                                                             \
    NODE_SHAPE, NODE_SHAPE, NODE_SHAPE, NODE_SHAPE,                            \
    NODE_SHAPE, NODE_SHAPE, NODE_SHAPE, NODE_SHAPE
struct collision_shape chaos_shapes_round_reverse[2] = {
    {0, SHAPE_SPHERE, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS),
     SHAPE_ATTACK_LEVEL(1) | SHAPE_HURTS_PLAYERS,
     SHAPE_HURTABLE | SHAPE_IGNORE_ENEMIES,
     {5.0F, -30.0F, 0.0F}, 7.0F, 0.0F, 0.0F, 0, {0, 0, 0}},
    {0, SHAPE_CYLINDER, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS),
     SHAPE_ATTACK_LEVEL(1) | SHAPE_GUARD_LEVEL(2) | SHAPE_HURTS_PLAYERS,
     SHAPE_HURTABLE | SHAPE_IGNORE_ENEMIES | SHAPE_IGNORE_OBJECTS |
         SHAPE_IGNORE_UNK_9,
     {10.0F, -14.0F, 0.0F}, 6.0F, 14.0F, 0.0F, 0, {0, 0, 0}},
};
struct collision_shape chaos_shapes_reverse[18] = {
    {0, SHAPE_SPHERE, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS),
     SHAPE_ATTACK_LEVEL(1) | SHAPE_HURTS_PLAYERS,
     SHAPE_HURTABLE | SHAPE_IGNORE_ENEMIES,
     {-5.0F, -30.0F, 0.0F}, 7.0F, 0.0F, 0.0F, 0, {0, 0, 0}},
    {0, SHAPE_CYLINDER, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS),
     SHAPE_ATTACK_LEVEL(1) | SHAPE_GUARD_LEVEL(2) | SHAPE_HURTS_PLAYERS,
     SHAPE_HURTABLE | SHAPE_IGNORE_ENEMIES | SHAPE_IGNORE_OBJECTS |
         SHAPE_IGNORE_UNK_9,
     {-10.0F, -14.0F, 0.0F}, 6.0F, 14.0F, 0.0F, 0, {0, 0, 0}},
    ARM_SHAPES,
    ARM_SHAPES,
};
struct collision_shape chaos_shapes[18] = {
    {0, SHAPE_SPHERE, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS),
     SHAPE_ATTACK_LEVEL(1) | SHAPE_HURTS_PLAYERS,
     SHAPE_HURTABLE | SHAPE_IGNORE_ENEMIES,
     {-5.0F, 30.0F, 0.0F}, 7.0F, 0.0F, 0.0F, 0, {0, 0, 0}},
    {0, SHAPE_CYLINDER, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS),
     SHAPE_ATTACK_LEVEL(1) | SHAPE_GUARD_LEVEL(2) | SHAPE_HURTS_PLAYERS,
     SHAPE_HURTABLE | SHAPE_IGNORE_ENEMIES | SHAPE_IGNORE_OBJECTS |
         SHAPE_IGNORE_UNK_9,
     {-10.0F, 14.0F, 0.0F}, 6.0F, 14.0F, 0.0F, 0, {0, 0, 0}},
    ARM_SHAPES,
    ARM_SHAPES,
};
struct collision_shape chaos_shapes_float[18] = {
    {0, SHAPE_SPHERE, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS),
     SHAPE_ATTACK_LEVEL(1) | SHAPE_HURTS_PLAYERS,
     SHAPE_HURTABLE | SHAPE_IGNORE_ENEMIES,
     {-2.0F, 25.0F, 0.0F}, 5.0F, 0.0F, 0.0F, 0, {0, 0, 0}},
    {0, SHAPE_SPHERE, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS),
     SHAPE_ATTACK_LEVEL(1) | SHAPE_GUARD_LEVEL(3) | SHAPE_HURTS_PLAYERS,
     SHAPE_HURTABLE | SHAPE_IGNORE_ENEMIES | SHAPE_IGNORE_OBJECTS |
         SHAPE_IGNORE_UNK_9,
     {-5.0F, 13.0F, 0.0F}, 13.0F, 0.0F, 0.0F, 0, {0, 0, 0}},
    ARM_SHAPES,
    ARM_SHAPES,
};
/* clang-format on */
/* no pieces: the list holds only the null that ends it */
NJS_CNK_MODEL *chaos_pieces[1] = {NULL};
struct enemy_action chaos_actions[17] = {
#include "shared/chaos_art/chaos_actions.inc"
};
