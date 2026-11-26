/* BOSS_LAST1.PRS's texture lists, chunk models, motions and camera actions,
 * from the disc. its D lies between object_kinds.c and title_textures.c */
#include <ninja.h>
#include <sg_xpt.h>

#include "boss_last1/arena.h"
#include "boss_last1/model_art.h"
#include "boss_last1/motion_art.h"
#include "boss_last1/texture_names.h"

static NJS_TEXNAME model_textures[14] = {
#include "boss_last1/biolizard_model_textures/model_textures.inc"
};
NJS_TEXLIST biolizard_model_texlist = {
#include "boss_last1/biolizard_model_textures/biolizard_model_texlist.inc"
};
static Sint16 unused_surface_a_00_polygons[51] = {
#include "boss_last1/biolizard_model_textures/unused_surface_a_00_polygons.inc"
};
static Sint32 unused_surface_a_00_vertices[231] = {
#include "boss_last1/biolizard_model_textures/unused_surface_a_00_vertices.inc"
};
static NJS_CNK_MODEL unused_surface_a_00_mesh = {
#include "boss_last1/biolizard_model_textures/unused_surface_a_00_mesh.inc"
};
static NJS_CNK_OBJECT unused_surface_a = {
#include "boss_last1/biolizard_model_textures/unused_surface_a.inc"
};
static Sint16 unused_surface_b_00_polygons[51] = {
#include "boss_last1/biolizard_model_textures/unused_surface_b_00_polygons.inc"
};
static Sint32 unused_surface_b_00_vertices[231] = {
#include "boss_last1/biolizard_model_textures/unused_surface_b_00_vertices.inc"
};
static NJS_CNK_MODEL unused_surface_b_00_mesh = {
#include "boss_last1/biolizard_model_textures/unused_surface_b_00_mesh.inc"
};
static NJS_CNK_OBJECT unused_surface_b = {
#include "boss_last1/biolizard_model_textures/unused_surface_b.inc"
};
static Sint16 shell_6_polygons[4287] = {
#include "boss_last1/biolizard_model_textures/shell_6_polygons.inc"
};
static NJS_CNK_MODEL shell_6_mesh = {
#include "boss_last1/biolizard_model_textures/shell_6_mesh.inc"
};
static NJS_CNK_OBJECT shell_6 = {
#include "boss_last1/biolizard_model_textures/shell_6.inc"
};
static Sint16 shell_5_polygons[321] = {
#include "boss_last1/biolizard_model_textures/shell_5_polygons.inc"
};
static NJS_CNK_MODEL shell_5_mesh = {
#include "boss_last1/biolizard_model_textures/shell_5_mesh.inc"
};
static NJS_CNK_OBJECT shell_5 = {
#include "boss_last1/biolizard_model_textures/shell_5.inc"
};
static Sint16 shell_4_polygons[321] = {
#include "boss_last1/biolizard_model_textures/shell_4_polygons.inc"
};
static NJS_CNK_MODEL shell_4_mesh = {
#include "boss_last1/biolizard_model_textures/shell_4_mesh.inc"
};
static NJS_CNK_OBJECT shell_4 = {
#include "boss_last1/biolizard_model_textures/shell_4.inc"
};
static Sint16 support_center_polygons[389] = {
#include "boss_last1/biolizard_model_textures/support_center_polygons.inc"
};
static Sint32 support_center_vertices[351] = {
#include "boss_last1/biolizard_model_textures/support_center_vertices.inc"
};
static NJS_CNK_MODEL support_center_mesh = {
#include "boss_last1/biolizard_model_textures/support_center_mesh.inc"
};
NJS_CNK_OBJECT biolizard_node_40 = {
#include "boss_last1/biolizard_model_textures/biolizard_node_40.inc"
};
static Sint16 support_mount_polygons[2639] = {
#include "boss_last1/biolizard_model_textures/support_mount_polygons.inc"
};
static NJS_CNK_MODEL support_mount_mesh = {
#include "boss_last1/biolizard_model_textures/support_mount_mesh.inc"
};
static NJS_CNK_OBJECT support_mount = {
#include "boss_last1/biolizard_model_textures/support_mount.inc"
};
static Sint16 shell_3_polygons[719] = {
#include "boss_last1/biolizard_model_textures/shell_3_polygons.inc"
};
static NJS_CNK_MODEL shell_3_mesh = {
#include "boss_last1/biolizard_model_textures/shell_3_mesh.inc"
};
static NJS_CNK_OBJECT shell_3 = {
#include "boss_last1/biolizard_model_textures/shell_3.inc"
};
static Sint16 shell_2_polygons[1045] = {
#include "boss_last1/biolizard_model_textures/shell_2_polygons.inc"
};
static NJS_CNK_MODEL shell_2_mesh = {
#include "boss_last1/biolizard_model_textures/shell_2_mesh.inc"
};
static NJS_CNK_OBJECT shell_2 = {
#include "boss_last1/biolizard_model_textures/shell_2.inc"
};
static Sint16 shell_1_polygons[1051] = {
#include "boss_last1/biolizard_model_textures/shell_1_polygons.inc"
};
static NJS_CNK_MODEL shell_1_mesh = {
#include "boss_last1/biolizard_model_textures/shell_1_mesh.inc"
};
static NJS_CNK_OBJECT shell_1 = {
#include "boss_last1/biolizard_model_textures/shell_1.inc"
};
static NJS_CNK_OBJECT upper_attachment_tip = {
#include "boss_last1/biolizard_model_textures/upper_attachment_tip.inc"
};
static Sint16 upper_attachment_polygons[2] = {
#include "boss_last1/biolizard_model_textures/upper_attachment_polygons.inc"
};
static Sint32 upper_attachment_vertices[39] = {
#include "boss_last1/biolizard_model_textures/upper_attachment_vertices.inc"
};
static NJS_CNK_MODEL upper_attachment_mesh = {
#include "boss_last1/biolizard_model_textures/upper_attachment_mesh.inc"
};
NJS_CNK_OBJECT biolizard_node_34 = {
#include "boss_last1/biolizard_model_textures/biolizard_node_34.inc"
};
static NJS_CNK_OBJECT upper_attachment_mount = {
#include "boss_last1/biolizard_model_textures/upper_attachment_mount.inc"
};
static NJS_CNK_OBJECT side_attachment_tip = {
#include "boss_last1/biolizard_model_textures/side_attachment_tip.inc"
};
static Sint32 side_attachment_vertices[81] = {
#include "boss_last1/biolizard_model_textures/side_attachment_vertices.inc"
};
static NJS_CNK_MODEL side_attachment_mesh = {
#include "boss_last1/biolizard_model_textures/side_attachment_mesh.inc"
};
NJS_CNK_OBJECT biolizard_node_31 = {
#include "boss_last1/biolizard_model_textures/biolizard_node_31.inc"
};
static NJS_CNK_OBJECT side_attachment_mount = {
#include "boss_last1/biolizard_model_textures/side_attachment_mount.inc"
};
static NJS_CNK_OBJECT lower_attachment_tip = {
#include "boss_last1/biolizard_model_textures/lower_attachment_tip.inc"
};
static Sint32 lower_attachment_end_vertices[165] = {
#include "boss_last1/biolizard_model_textures/lower_attachment_end_vertices.inc"
};
static NJS_CNK_MODEL lower_attachment_end_mesh = {
#include "boss_last1/biolizard_model_textures/lower_attachment_end_mesh.inc"
};
static NJS_CNK_OBJECT lower_attachment_end = {
#include "boss_last1/biolizard_model_textures/lower_attachment_end.inc"
};
static Sint32 lower_attachment_vertices[231] = {
#include "boss_last1/biolizard_model_textures/lower_attachment_vertices.inc"
};
static NJS_CNK_MODEL lower_attachment_mesh = {
#include "boss_last1/biolizard_model_textures/lower_attachment_mesh.inc"
};
NJS_CNK_OBJECT biolizard_node_27 = {
#include "boss_last1/biolizard_model_textures/biolizard_node_27.inc"
};
static NJS_CNK_OBJECT lower_attachment_mount = {
#include "boss_last1/biolizard_model_textures/lower_attachment_mount.inc"
};
NJS_CNK_OBJECT biolizard_node_25 = {
#include "boss_last1/biolizard_model_textures/biolizard_node_25.inc"
};
static Sint32 tail_5_vertices[424] = {
#include "boss_last1/biolizard_model_textures/tail_5_vertices.inc"
};
static NJS_CNK_MODEL tail_5_mesh = {
#include "boss_last1/biolizard_model_textures/tail_5_mesh.inc"
};
NJS_CNK_OBJECT biolizard_node_24 = {
#include "boss_last1/biolizard_model_textures/biolizard_node_24.inc"
};
static Sint32 tail_4_vertices[490] = {
#include "boss_last1/biolizard_model_textures/tail_4_vertices.inc"
};
static NJS_CNK_MODEL tail_4_mesh = {
#include "boss_last1/biolizard_model_textures/tail_4_mesh.inc"
};
NJS_CNK_OBJECT biolizard_node_23 = {
#include "boss_last1/biolizard_model_textures/biolizard_node_23.inc"
};
static Sint32 tail_3_vertices[476] = {
#include "boss_last1/biolizard_model_textures/tail_3_vertices.inc"
};
static NJS_CNK_MODEL tail_3_mesh = {
#include "boss_last1/biolizard_model_textures/tail_3_mesh.inc"
};
NJS_CNK_OBJECT biolizard_node_22 = {
#include "boss_last1/biolizard_model_textures/biolizard_node_22.inc"
};
static Sint32 tail_2_vertices[336] = {
#include "boss_last1/biolizard_model_textures/tail_2_vertices.inc"
};
static NJS_CNK_MODEL tail_2_mesh = {
#include "boss_last1/biolizard_model_textures/tail_2_mesh.inc"
};
NJS_CNK_OBJECT biolizard_node_21 = {
#include "boss_last1/biolizard_model_textures/biolizard_node_21.inc"
};
static Sint32 tail_base_vertices[322] = {
#include "boss_last1/biolizard_model_textures/tail_base_vertices.inc"
};
static NJS_CNK_MODEL tail_base_mesh = {
#include "boss_last1/biolizard_model_textures/tail_base_mesh.inc"
};
NJS_CNK_OBJECT biolizard_node_20 = {
#include "boss_last1/biolizard_model_textures/biolizard_node_20.inc"
};
static NJS_CNK_OBJECT tail_mount = {
#include "boss_last1/biolizard_model_textures/tail_mount.inc"
};
static NJS_CNK_OBJECT negative_foot_tip = {
#include "boss_last1/biolizard_model_textures/negative_foot_tip.inc"
};
static Sint32 negative_foot_vertices[738] = {
#include "boss_last1/biolizard_model_textures/negative_foot_vertices.inc"
};
static NJS_CNK_MODEL negative_foot_mesh = {
#include "boss_last1/biolizard_model_textures/negative_foot_mesh.inc"
};
NJS_CNK_OBJECT biolizard_node_17 = {
#include "boss_last1/biolizard_model_textures/biolizard_node_17.inc"
};
static Sint32 negative_leg_vertices[1078] = {
#include "boss_last1/biolizard_model_textures/negative_leg_vertices.inc"
};
static NJS_CNK_MODEL negative_leg_mesh = {
#include "boss_last1/biolizard_model_textures/negative_leg_mesh.inc"
};
NJS_CNK_OBJECT biolizard_node_16 = {
#include "boss_last1/biolizard_model_textures/biolizard_node_16.inc"
};
static NJS_CNK_OBJECT negative_leg_mount = {
#include "boss_last1/biolizard_model_textures/negative_leg_mount.inc"
};
static NJS_CNK_OBJECT positive_foot_tip = {
#include "boss_last1/biolizard_model_textures/positive_foot_tip.inc"
};
static Sint32 positive_foot_vertices[738] = {
#include "boss_last1/biolizard_model_textures/positive_foot_vertices.inc"
};
static NJS_CNK_MODEL positive_foot_mesh = {
#include "boss_last1/biolizard_model_textures/positive_foot_mesh.inc"
};
NJS_CNK_OBJECT biolizard_node_13 = {
#include "boss_last1/biolizard_model_textures/biolizard_node_13.inc"
};
static Sint32 positive_leg_vertices[917] = {
#include "boss_last1/biolizard_model_textures/positive_leg_vertices.inc"
};
static NJS_CNK_MODEL positive_leg_mesh = {
#include "boss_last1/biolizard_model_textures/positive_leg_mesh.inc"
};
NJS_CNK_OBJECT biolizard_node_12 = {
#include "boss_last1/biolizard_model_textures/biolizard_node_12.inc"
};
static NJS_CNK_OBJECT positive_leg_mount = {
#include "boss_last1/biolizard_model_textures/positive_leg_mount.inc"
};
static NJS_CNK_OBJECT jaw_tip = {
#include "boss_last1/biolizard_model_textures/jaw_tip.inc"
};
static Sint32 jaw_vertices[1078] = {
#include "boss_last1/biolizard_model_textures/jaw_vertices.inc"
};
static NJS_CNK_MODEL jaw_mesh = {
#include "boss_last1/biolizard_model_textures/jaw_mesh.inc"
};
static NJS_CNK_OBJECT jaw = {
#include "boss_last1/biolizard_model_textures/jaw.inc"
};
static Sint32 head_vertices[2781] = {
#include "boss_last1/biolizard_model_textures/head_vertices.inc"
};
static NJS_CNK_MODEL head_mesh = {
#include "boss_last1/biolizard_model_textures/head_mesh.inc"
};
NJS_CNK_OBJECT biolizard_node_8 = {
#include "boss_last1/biolizard_model_textures/biolizard_node_8.inc"
};
static Sint32 neck_5_vertices[511] = {
#include "boss_last1/biolizard_model_textures/neck_5_vertices.inc"
};
static NJS_CNK_MODEL neck_5_mesh = {
#include "boss_last1/biolizard_model_textures/neck_5_mesh.inc"
};
NJS_CNK_OBJECT biolizard_node_7 = {
#include "boss_last1/biolizard_model_textures/biolizard_node_7.inc"
};
static Sint32 neck_4_vertices[728] = {
#include "boss_last1/biolizard_model_textures/neck_4_vertices.inc"
};
static NJS_CNK_MODEL neck_4_mesh = {
#include "boss_last1/biolizard_model_textures/neck_4_mesh.inc"
};
NJS_CNK_OBJECT biolizard_node_6 = {
#include "boss_last1/biolizard_model_textures/biolizard_node_6.inc"
};
static Sint32 neck_3_vertices[574] = {
#include "boss_last1/biolizard_model_textures/neck_3_vertices.inc"
};
static NJS_CNK_MODEL neck_3_mesh = {
#include "boss_last1/biolizard_model_textures/neck_3_mesh.inc"
};
NJS_CNK_OBJECT biolizard_node_5 = {
#include "boss_last1/biolizard_model_textures/biolizard_node_5.inc"
};
static Sint32 neck_2_vertices[1527] = {
#include "boss_last1/biolizard_model_textures/neck_2_vertices.inc"
};
static NJS_CNK_MODEL neck_2_mesh = {
#include "boss_last1/biolizard_model_textures/neck_2_mesh.inc"
};
NJS_CNK_OBJECT biolizard_node_4 = {
#include "boss_last1/biolizard_model_textures/biolizard_node_4.inc"
};
static Sint32 neck_base_vertices[1690] = {
#include "boss_last1/biolizard_model_textures/neck_base_vertices.inc"
};
static NJS_CNK_MODEL neck_base_mesh = {
#include "boss_last1/biolizard_model_textures/neck_base_mesh.inc"
};
NJS_CNK_OBJECT biolizard_node_3 = {
#include "boss_last1/biolizard_model_textures/biolizard_node_3.inc"
};
static NJS_CNK_OBJECT neck_mount = {
#include "boss_last1/biolizard_model_textures/neck_mount.inc"
};
static Sint32 torso_vertices[2820] = {
#include "boss_last1/biolizard_model_textures/torso_vertices.inc"
};
static NJS_CNK_MODEL torso_mesh = {
#include "boss_last1/biolizard_model_textures/torso_mesh.inc"
};
static NJS_CNK_OBJECT torso = {
#include "boss_last1/biolizard_model_textures/torso.inc"
};
static Sint16 root_polygons[3915] = {
#include "boss_last1/biolizard_model_textures/root_polygons.inc"
};
static Sint32 root_vertices[80] = {
#include "boss_last1/biolizard_model_textures/root_vertices.inc"
};
static NJS_CNK_MODEL root_mesh = {
#include "boss_last1/biolizard_model_textures/root_mesh.inc"
};
NJS_CNK_OBJECT biolizard_model = {
#include "boss_last1/biolizard_model_textures/biolizard_model.inc"
};
static NJS_MKEY_F biolizard_motion_16_joint_0_position[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_16_joint_0_position.inc"
};
static NJS_MKEY_A biolizard_motion_16_joint_0_angles[56] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_16_joint_0_angles.inc"
};
static NJS_MKEY_F biolizard_motion_16_joint_1_position[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_16_joint_1_position.inc"
};
static NJS_MKEY_A biolizard_motion_16_joint_3_angles[58] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_16_joint_3_angles.inc"
};
static NJS_MKEY_A biolizard_motion_16_joint_4_angles[60] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_16_joint_4_angles.inc"
};
static NJS_MKEY_A biolizard_motion_16_joint_5_angles[61] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_16_joint_5_angles.inc"
};
static NJS_MKEY_A biolizard_motion_16_joint_6_angles[64] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_16_joint_6_angles.inc"
};
static NJS_MKEY_A biolizard_motion_16_joint_7_angles[64] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_16_joint_7_angles.inc"
};
static NJS_MKEY_A biolizard_motion_16_joint_8_angles[64] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_16_joint_8_angles.inc"
};
static NJS_MKEY_A biolizard_motion_16_joint_9_angles[33] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_16_joint_9_angles.inc"
};
static NJS_MKEY_A biolizard_motion_16_joint_12_angles[64] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_16_joint_12_angles.inc"
};
static NJS_MKEY_A biolizard_motion_16_joint_13_angles[64] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_16_joint_13_angles.inc"
};
static NJS_MKEY_A biolizard_motion_16_joint_16_angles[64] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_16_joint_16_angles.inc"
};
static NJS_MKEY_A biolizard_motion_16_joint_17_angles[62] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_16_joint_17_angles.inc"
};
static NJS_MKEY_A biolizard_motion_16_joint_20_angles[64] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_16_joint_20_angles.inc"
};
static NJS_MKEY_A biolizard_motion_16_joint_21_angles[64] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_16_joint_21_angles.inc"
};
static NJS_MKEY_A biolizard_motion_16_joint_22_angles[64] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_16_joint_22_angles.inc"
};
static NJS_MKEY_A biolizard_motion_16_joint_23_angles[64] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_16_joint_23_angles.inc"
};
static NJS_MKEY_A biolizard_motion_16_joint_24_angles[64] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_16_joint_24_angles.inc"
};
static NJS_MKEY_A biolizard_motion_16_joint_27_angles[64] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_16_joint_27_angles.inc"
};
static NJS_MKEY_A biolizard_motion_16_joint_28_angles[64] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_16_joint_28_angles.inc"
};
static NJS_MKEY_A biolizard_motion_16_joint_31_angles[64] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_16_joint_31_angles.inc"
};
static NJS_MKEY_A biolizard_motion_16_joint_34_angles[64] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_16_joint_34_angles.inc"
};
static NJS_MKEY_F biolizard_motion_16_joint_36_position[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_16_joint_36_position.inc"
};
static NJS_MKEY_F biolizard_motion_16_joint_38_position[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_16_joint_38_position.inc"
};
static NJS_MKEY_F biolizard_motion_16_joint_39_position[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_16_joint_39_position.inc"
};
static NJS_MKEY_F biolizard_motion_16_joint_41_position[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_16_joint_41_position.inc"
};
static NJS_MKEY_F biolizard_motion_16_joint_42_position[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_16_joint_42_position.inc"
};
static NJS_MKEY_F biolizard_motion_16_joint_43_position[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_16_joint_43_position.inc"
};
static NJS_MKEY_A biolizard_motion_16_joint_43_angles[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_16_joint_43_angles.inc"
};
static NJS_MDATA2 biolizard_motion_16_tracks[44] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_16_tracks.inc"
};
NJS_MOTION biolizard_motion_16 = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_16.inc"
};
static NJS_MKEY_F biolizard_motion_9_joint_0_position[64] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_9_joint_0_position.inc"
};
static NJS_MKEY_A biolizard_motion_9_joint_3_angles[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_9_joint_3_angles.inc"
};
static NJS_MKEY_A biolizard_motion_9_joint_4_angles[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_9_joint_4_angles.inc"
};
static NJS_MKEY_A biolizard_motion_9_joint_5_angles[54] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_9_joint_5_angles.inc"
};
static NJS_MKEY_A biolizard_motion_9_joint_6_angles[64] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_9_joint_6_angles.inc"
};
static NJS_MKEY_A biolizard_motion_9_joint_7_angles[64] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_9_joint_7_angles.inc"
};
static NJS_MKEY_A biolizard_motion_9_joint_8_angles[64] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_9_joint_8_angles.inc"
};
static NJS_MKEY_A biolizard_motion_9_joint_9_angles[64] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_9_joint_9_angles.inc"
};
static NJS_MKEY_A biolizard_motion_9_joint_12_angles[55] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_9_joint_12_angles.inc"
};
static NJS_MKEY_A biolizard_motion_9_joint_13_angles[47] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_9_joint_13_angles.inc"
};
static NJS_MKEY_A biolizard_motion_9_joint_16_angles[55] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_9_joint_16_angles.inc"
};
static NJS_MKEY_A biolizard_motion_9_joint_17_angles[58] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_9_joint_17_angles.inc"
};
static NJS_MKEY_A biolizard_motion_9_joint_20_angles[61] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_9_joint_20_angles.inc"
};
static NJS_MKEY_A biolizard_motion_9_joint_21_angles[64] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_9_joint_21_angles.inc"
};
static NJS_MKEY_A biolizard_motion_9_joint_22_angles[64] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_9_joint_22_angles.inc"
};
static NJS_MKEY_A biolizard_motion_9_joint_23_angles[64] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_9_joint_23_angles.inc"
};
static NJS_MKEY_A biolizard_motion_9_joint_24_angles[64] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_9_joint_24_angles.inc"
};
static NJS_MKEY_A biolizard_motion_9_joint_27_angles[64] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_9_joint_27_angles.inc"
};
static NJS_MKEY_A biolizard_motion_9_joint_28_angles[64] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_9_joint_28_angles.inc"
};
static NJS_MKEY_A biolizard_motion_9_joint_31_angles[64] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_9_joint_31_angles.inc"
};
static NJS_MKEY_A biolizard_motion_9_joint_34_angles[64] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_9_joint_34_angles.inc"
};
static NJS_MDATA2 biolizard_motion_9_tracks[44] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_9_tracks.inc"
};
NJS_MOTION biolizard_motion_9 = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_9.inc"
};
static NJS_MKEY_F biolizard_motion_10_joint_0_position[64] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_10_joint_0_position.inc"
};
static NJS_MKEY_A biolizard_motion_10_joint_3_angles[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_10_joint_3_angles.inc"
};
static NJS_MKEY_A biolizard_motion_10_joint_4_angles[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_10_joint_4_angles.inc"
};
static NJS_MKEY_A biolizard_motion_10_joint_5_angles[53] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_10_joint_5_angles.inc"
};
static NJS_MKEY_A biolizard_motion_10_joint_6_angles[64] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_10_joint_6_angles.inc"
};
static NJS_MKEY_A biolizard_motion_10_joint_7_angles[61] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_10_joint_7_angles.inc"
};
static NJS_MKEY_A biolizard_motion_10_joint_8_angles[64] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_10_joint_8_angles.inc"
};
static NJS_MKEY_A biolizard_motion_10_joint_9_angles[64] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_10_joint_9_angles.inc"
};
static NJS_MKEY_A biolizard_motion_10_joint_12_angles[55] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_10_joint_12_angles.inc"
};
static NJS_MKEY_A biolizard_motion_10_joint_13_angles[47] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_10_joint_13_angles.inc"
};
static NJS_MKEY_A biolizard_motion_10_joint_16_angles[55] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_10_joint_16_angles.inc"
};
static NJS_MKEY_A biolizard_motion_10_joint_17_angles[58] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_10_joint_17_angles.inc"
};
static NJS_MKEY_A biolizard_motion_10_joint_20_angles[61] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_10_joint_20_angles.inc"
};
static NJS_MKEY_A biolizard_motion_10_joint_21_angles[64] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_10_joint_21_angles.inc"
};
static NJS_MKEY_A biolizard_motion_10_joint_22_angles[64] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_10_joint_22_angles.inc"
};
static NJS_MKEY_A biolizard_motion_10_joint_23_angles[64] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_10_joint_23_angles.inc"
};
static NJS_MKEY_A biolizard_motion_10_joint_24_angles[64] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_10_joint_24_angles.inc"
};
static NJS_MKEY_A biolizard_motion_10_joint_27_angles[64] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_10_joint_27_angles.inc"
};
static NJS_MKEY_A biolizard_motion_10_joint_28_angles[64] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_10_joint_28_angles.inc"
};
static NJS_MKEY_A biolizard_motion_10_joint_31_angles[64] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_10_joint_31_angles.inc"
};
static NJS_MKEY_A biolizard_motion_10_joint_34_angles[64] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_10_joint_34_angles.inc"
};
static NJS_MDATA2 biolizard_motion_10_tracks[44] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_10_tracks.inc"
};
NJS_MOTION biolizard_motion_10 = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_10.inc"
};
static NJS_MKEY_F biolizard_motion_7_joint_0_position[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_7_joint_0_position.inc"
};
static NJS_MKEY_A biolizard_motion_7_joint_0_angles[30] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_7_joint_0_angles.inc"
};
static NJS_MKEY_F biolizard_motion_7_joint_1_position[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_7_joint_1_position.inc"
};
static NJS_MKEY_A biolizard_motion_7_joint_3_angles[32] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_7_joint_3_angles.inc"
};
static NJS_MKEY_A biolizard_motion_7_joint_4_angles[32] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_7_joint_4_angles.inc"
};
static NJS_MKEY_A biolizard_motion_7_joint_5_angles[32] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_7_joint_5_angles.inc"
};
static NJS_MKEY_F biolizard_motion_7_joint_6_position[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_7_joint_6_position.inc"
};
static NJS_MKEY_A biolizard_motion_7_joint_6_angles[32] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_7_joint_6_angles.inc"
};
static NJS_MKEY_F biolizard_motion_7_joint_7_position[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_7_joint_7_position.inc"
};
static NJS_MKEY_A biolizard_motion_7_joint_7_angles[32] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_7_joint_7_angles.inc"
};
static NJS_MKEY_F biolizard_motion_7_joint_8_position[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_7_joint_8_position.inc"
};
static NJS_MKEY_A biolizard_motion_7_joint_8_angles[32] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_7_joint_8_angles.inc"
};
static NJS_MKEY_F biolizard_motion_7_joint_9_position[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_7_joint_9_position.inc"
};
static NJS_MKEY_A biolizard_motion_7_joint_9_angles[32] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_7_joint_9_angles.inc"
};
static NJS_MKEY_F biolizard_motion_7_joint_10_position[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_7_joint_10_position.inc"
};
static NJS_MKEY_A biolizard_motion_7_joint_12_angles[32] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_7_joint_12_angles.inc"
};
static NJS_MKEY_A biolizard_motion_7_joint_13_angles[31] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_7_joint_13_angles.inc"
};
static NJS_MKEY_A biolizard_motion_7_joint_16_angles[31] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_7_joint_16_angles.inc"
};
static NJS_MKEY_A biolizard_motion_7_joint_17_angles[31] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_7_joint_17_angles.inc"
};
static NJS_MKEY_A biolizard_motion_7_joint_20_angles[32] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_7_joint_20_angles.inc"
};
static NJS_MKEY_A biolizard_motion_7_joint_21_angles[32] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_7_joint_21_angles.inc"
};
static NJS_MKEY_A biolizard_motion_7_joint_22_angles[32] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_7_joint_22_angles.inc"
};
static NJS_MKEY_A biolizard_motion_7_joint_23_angles[32] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_7_joint_23_angles.inc"
};
static NJS_MKEY_A biolizard_motion_7_joint_24_angles[32] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_7_joint_24_angles.inc"
};
static NJS_MKEY_A biolizard_motion_7_joint_27_angles[32] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_7_joint_27_angles.inc"
};
static NJS_MKEY_A biolizard_motion_7_joint_28_angles[32] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_7_joint_28_angles.inc"
};
static NJS_MKEY_A biolizard_motion_7_joint_31_angles[32] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_7_joint_31_angles.inc"
};
static NJS_MKEY_A biolizard_motion_7_joint_34_angles[32] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_7_joint_34_angles.inc"
};
static NJS_MKEY_F biolizard_motion_7_joint_36_position[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_7_joint_36_position.inc"
};
static NJS_MKEY_F biolizard_motion_7_joint_38_position[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_7_joint_38_position.inc"
};
static NJS_MKEY_F biolizard_motion_7_joint_39_position[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_7_joint_39_position.inc"
};
static NJS_MKEY_F biolizard_motion_7_joint_41_position[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_7_joint_41_position.inc"
};
static NJS_MKEY_F biolizard_motion_7_joint_42_position[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_7_joint_42_position.inc"
};
static NJS_MKEY_F biolizard_motion_7_joint_43_position[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_7_joint_43_position.inc"
};
static NJS_MKEY_A biolizard_motion_7_joint_43_angles[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_7_joint_43_angles.inc"
};
static NJS_MDATA2 biolizard_motion_7_tracks[44] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_7_tracks.inc"
};
NJS_MOTION biolizard_motion_7 = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_7.inc"
};
static NJS_MKEY_F biolizard_motion_7_partner_joint_0_position[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_7_partner_joint_0_position.inc"
};
static NJS_MKEY_A biolizard_motion_7_partner_joint_0_angles[30] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_7_partner_joint_0_angles.inc"
};
static NJS_MKEY_F biolizard_motion_7_partner_joint_1_position[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_7_partner_joint_1_position.inc"
};
static NJS_MKEY_A biolizard_motion_7_partner_joint_3_angles[28] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_7_partner_joint_3_angles.inc"
};
static NJS_MKEY_A biolizard_motion_7_partner_joint_4_angles[28] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_7_partner_joint_4_angles.inc"
};
static NJS_MKEY_A biolizard_motion_7_partner_joint_5_angles[32] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_7_partner_joint_5_angles.inc"
};
static NJS_MKEY_F biolizard_motion_7_partner_joint_6_position[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_7_partner_joint_6_position.inc"
};
static NJS_MKEY_A biolizard_motion_7_partner_joint_6_angles[28] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_7_partner_joint_6_angles.inc"
};
static NJS_MKEY_F biolizard_motion_7_partner_joint_7_position[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_7_partner_joint_7_position.inc"
};
static NJS_MKEY_A biolizard_motion_7_partner_joint_7_angles[32] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_7_partner_joint_7_angles.inc"
};
static NJS_MKEY_F biolizard_motion_7_partner_joint_8_position[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_7_partner_joint_8_position.inc"
};
static NJS_MKEY_A biolizard_motion_7_partner_joint_8_angles[32] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_7_partner_joint_8_angles.inc"
};
static NJS_MKEY_F biolizard_motion_7_partner_joint_9_position[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_7_partner_joint_9_position.inc"
};
static NJS_MKEY_A biolizard_motion_7_partner_joint_9_angles[32] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_7_partner_joint_9_angles.inc"
};
static NJS_MKEY_F biolizard_motion_7_partner_joint_10_position[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_7_partner_joint_10_position.inc"
};
static NJS_MKEY_A biolizard_motion_7_partner_joint_12_angles[32] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_7_partner_joint_12_angles.inc"
};
static NJS_MKEY_A biolizard_motion_7_partner_joint_13_angles[31] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_7_partner_joint_13_angles.inc"
};
static NJS_MKEY_A biolizard_motion_7_partner_joint_16_angles[31] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_7_partner_joint_16_angles.inc"
};
static NJS_MKEY_A biolizard_motion_7_partner_joint_17_angles[31] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_7_partner_joint_17_angles.inc"
};
static NJS_MKEY_A biolizard_motion_7_partner_joint_20_angles[32] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_7_partner_joint_20_angles.inc"
};
static NJS_MKEY_A biolizard_motion_7_partner_joint_21_angles[32] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_7_partner_joint_21_angles.inc"
};
static NJS_MKEY_A biolizard_motion_7_partner_joint_22_angles[32] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_7_partner_joint_22_angles.inc"
};
static NJS_MKEY_A biolizard_motion_7_partner_joint_23_angles[32] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_7_partner_joint_23_angles.inc"
};
static NJS_MKEY_A biolizard_motion_7_partner_joint_24_angles[32] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_7_partner_joint_24_angles.inc"
};
static NJS_MKEY_A biolizard_motion_7_partner_joint_27_angles[32] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_7_partner_joint_27_angles.inc"
};
static NJS_MKEY_A biolizard_motion_7_partner_joint_28_angles[32] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_7_partner_joint_28_angles.inc"
};
static NJS_MKEY_A biolizard_motion_7_partner_joint_31_angles[32] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_7_partner_joint_31_angles.inc"
};
static NJS_MKEY_A biolizard_motion_7_partner_joint_34_angles[32] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_7_partner_joint_34_angles.inc"
};
static NJS_MKEY_F biolizard_motion_7_partner_joint_36_position[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_7_partner_joint_36_position.inc"
};
static NJS_MKEY_F biolizard_motion_7_partner_joint_38_position[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_7_partner_joint_38_position.inc"
};
static NJS_MKEY_F biolizard_motion_7_partner_joint_39_position[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_7_partner_joint_39_position.inc"
};
static NJS_MKEY_F biolizard_motion_7_partner_joint_41_position[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_7_partner_joint_41_position.inc"
};
static NJS_MKEY_F biolizard_motion_7_partner_joint_42_position[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_7_partner_joint_42_position.inc"
};
static NJS_MKEY_F biolizard_motion_7_partner_joint_43_position[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_7_partner_joint_43_position.inc"
};
static NJS_MKEY_A biolizard_motion_7_partner_joint_43_angles[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_7_partner_joint_43_angles.inc"
};
static NJS_MDATA2 biolizard_motion_7_partner_tracks[44] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_7_partner_tracks.inc"
};
NJS_MOTION biolizard_motion_7_partner = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_7_partner.inc"
};
static NJS_MKEY_F biolizard_motion_8_joint_0_position[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_8_joint_0_position.inc"
};
static NJS_MKEY_A biolizard_motion_8_joint_0_angles[30] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_8_joint_0_angles.inc"
};
static NJS_MKEY_F biolizard_motion_8_joint_1_position[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_8_joint_1_position.inc"
};
static NJS_MKEY_A biolizard_motion_8_joint_3_angles[32] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_8_joint_3_angles.inc"
};
static NJS_MKEY_A biolizard_motion_8_joint_4_angles[32] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_8_joint_4_angles.inc"
};
static NJS_MKEY_A biolizard_motion_8_joint_5_angles[32] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_8_joint_5_angles.inc"
};
static NJS_MKEY_F biolizard_motion_8_joint_6_position[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_8_joint_6_position.inc"
};
static NJS_MKEY_A biolizard_motion_8_joint_6_angles[32] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_8_joint_6_angles.inc"
};
static NJS_MKEY_F biolizard_motion_8_joint_7_position[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_8_joint_7_position.inc"
};
static NJS_MKEY_A biolizard_motion_8_joint_7_angles[32] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_8_joint_7_angles.inc"
};
static NJS_MKEY_F biolizard_motion_8_joint_8_position[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_8_joint_8_position.inc"
};
static NJS_MKEY_A biolizard_motion_8_joint_8_angles[32] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_8_joint_8_angles.inc"
};
static NJS_MKEY_F biolizard_motion_8_joint_9_position[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_8_joint_9_position.inc"
};
static NJS_MKEY_A biolizard_motion_8_joint_9_angles[32] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_8_joint_9_angles.inc"
};
static NJS_MKEY_F biolizard_motion_8_joint_10_position[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_8_joint_10_position.inc"
};
static NJS_MKEY_A biolizard_motion_8_joint_12_angles[32] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_8_joint_12_angles.inc"
};
static NJS_MKEY_A biolizard_motion_8_joint_13_angles[31] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_8_joint_13_angles.inc"
};
static NJS_MKEY_A biolizard_motion_8_joint_16_angles[31] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_8_joint_16_angles.inc"
};
static NJS_MKEY_A biolizard_motion_8_joint_17_angles[31] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_8_joint_17_angles.inc"
};
static NJS_MKEY_A biolizard_motion_8_joint_20_angles[32] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_8_joint_20_angles.inc"
};
static NJS_MKEY_A biolizard_motion_8_joint_21_angles[32] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_8_joint_21_angles.inc"
};
static NJS_MKEY_A biolizard_motion_8_joint_22_angles[32] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_8_joint_22_angles.inc"
};
static NJS_MKEY_A biolizard_motion_8_joint_23_angles[32] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_8_joint_23_angles.inc"
};
static NJS_MKEY_A biolizard_motion_8_joint_24_angles[32] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_8_joint_24_angles.inc"
};
static NJS_MKEY_A biolizard_motion_8_joint_27_angles[32] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_8_joint_27_angles.inc"
};
static NJS_MKEY_A biolizard_motion_8_joint_28_angles[32] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_8_joint_28_angles.inc"
};
static NJS_MKEY_A biolizard_motion_8_joint_31_angles[32] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_8_joint_31_angles.inc"
};
static NJS_MKEY_A biolizard_motion_8_joint_34_angles[32] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_8_joint_34_angles.inc"
};
static NJS_MKEY_F biolizard_motion_8_joint_36_position[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_8_joint_36_position.inc"
};
static NJS_MKEY_F biolizard_motion_8_joint_38_position[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_8_joint_38_position.inc"
};
static NJS_MKEY_F biolizard_motion_8_joint_39_position[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_8_joint_39_position.inc"
};
static NJS_MKEY_F biolizard_motion_8_joint_41_position[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_8_joint_41_position.inc"
};
static NJS_MKEY_F biolizard_motion_8_joint_42_position[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_8_joint_42_position.inc"
};
static NJS_MKEY_F biolizard_motion_8_joint_43_position[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_8_joint_43_position.inc"
};
static NJS_MKEY_A biolizard_motion_8_joint_43_angles[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_8_joint_43_angles.inc"
};
static NJS_MDATA2 biolizard_motion_8_tracks[44] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_8_tracks.inc"
};
NJS_MOTION biolizard_motion_8 = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_8.inc"
};
static NJS_MKEY_F biolizard_motion_8_partner_joint_0_position[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_8_partner_joint_0_position.inc"
};
static NJS_MKEY_A biolizard_motion_8_partner_joint_0_angles[30] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_8_partner_joint_0_angles.inc"
};
static NJS_MKEY_F biolizard_motion_8_partner_joint_1_position[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_8_partner_joint_1_position.inc"
};
static NJS_MKEY_A biolizard_motion_8_partner_joint_3_angles[29] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_8_partner_joint_3_angles.inc"
};
static NJS_MKEY_A biolizard_motion_8_partner_joint_4_angles[32] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_8_partner_joint_4_angles.inc"
};
static NJS_MKEY_A biolizard_motion_8_partner_joint_5_angles[32] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_8_partner_joint_5_angles.inc"
};
static NJS_MKEY_F biolizard_motion_8_partner_joint_6_position[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_8_partner_joint_6_position.inc"
};
static NJS_MKEY_A biolizard_motion_8_partner_joint_6_angles[32] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_8_partner_joint_6_angles.inc"
};
static NJS_MKEY_F biolizard_motion_8_partner_joint_7_position[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_8_partner_joint_7_position.inc"
};
static NJS_MKEY_A biolizard_motion_8_partner_joint_7_angles[32] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_8_partner_joint_7_angles.inc"
};
static NJS_MKEY_F biolizard_motion_8_partner_joint_8_position[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_8_partner_joint_8_position.inc"
};
static NJS_MKEY_A biolizard_motion_8_partner_joint_8_angles[32] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_8_partner_joint_8_angles.inc"
};
static NJS_MKEY_F biolizard_motion_8_partner_joint_9_position[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_8_partner_joint_9_position.inc"
};
static NJS_MKEY_A biolizard_motion_8_partner_joint_9_angles[32] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_8_partner_joint_9_angles.inc"
};
static NJS_MKEY_F biolizard_motion_8_partner_joint_10_position[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_8_partner_joint_10_position.inc"
};
static NJS_MKEY_A biolizard_motion_8_partner_joint_12_angles[32] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_8_partner_joint_12_angles.inc"
};
static NJS_MKEY_A biolizard_motion_8_partner_joint_13_angles[31] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_8_partner_joint_13_angles.inc"
};
static NJS_MKEY_A biolizard_motion_8_partner_joint_16_angles[31] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_8_partner_joint_16_angles.inc"
};
static NJS_MKEY_A biolizard_motion_8_partner_joint_17_angles[31] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_8_partner_joint_17_angles.inc"
};
static NJS_MKEY_A biolizard_motion_8_partner_joint_20_angles[32] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_8_partner_joint_20_angles.inc"
};
static NJS_MKEY_A biolizard_motion_8_partner_joint_21_angles[32] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_8_partner_joint_21_angles.inc"
};
static NJS_MKEY_A biolizard_motion_8_partner_joint_22_angles[32] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_8_partner_joint_22_angles.inc"
};
static NJS_MKEY_A biolizard_motion_8_partner_joint_23_angles[32] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_8_partner_joint_23_angles.inc"
};
static NJS_MKEY_A biolizard_motion_8_partner_joint_24_angles[32] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_8_partner_joint_24_angles.inc"
};
static NJS_MKEY_A biolizard_motion_8_partner_joint_27_angles[32] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_8_partner_joint_27_angles.inc"
};
static NJS_MKEY_A biolizard_motion_8_partner_joint_28_angles[32] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_8_partner_joint_28_angles.inc"
};
static NJS_MKEY_A biolizard_motion_8_partner_joint_31_angles[32] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_8_partner_joint_31_angles.inc"
};
static NJS_MKEY_A biolizard_motion_8_partner_joint_34_angles[32] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_8_partner_joint_34_angles.inc"
};
static NJS_MKEY_F biolizard_motion_8_partner_joint_36_position[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_8_partner_joint_36_position.inc"
};
static NJS_MKEY_F biolizard_motion_8_partner_joint_38_position[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_8_partner_joint_38_position.inc"
};
static NJS_MKEY_F biolizard_motion_8_partner_joint_39_position[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_8_partner_joint_39_position.inc"
};
static NJS_MKEY_F biolizard_motion_8_partner_joint_41_position[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_8_partner_joint_41_position.inc"
};
static NJS_MKEY_F biolizard_motion_8_partner_joint_42_position[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_8_partner_joint_42_position.inc"
};
static NJS_MKEY_F biolizard_motion_8_partner_joint_43_position[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_8_partner_joint_43_position.inc"
};
static NJS_MKEY_A biolizard_motion_8_partner_joint_43_angles[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_8_partner_joint_43_angles.inc"
};
static NJS_MDATA2 biolizard_motion_8_partner_tracks[44] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_8_partner_tracks.inc"
};
NJS_MOTION biolizard_motion_8_partner = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_8_partner.inc"
};
static NJS_MKEY_F biolizard_motion_13_joint_0_position[89] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_13_joint_0_position.inc"
};
static NJS_MKEY_A biolizard_motion_13_joint_0_angles[35] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_13_joint_0_angles.inc"
};
static NJS_MKEY_F biolizard_motion_13_joint_1_position[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_13_joint_1_position.inc"
};
static NJS_MKEY_A biolizard_motion_13_joint_3_angles[67] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_13_joint_3_angles.inc"
};
static NJS_MKEY_A biolizard_motion_13_joint_4_angles[90] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_13_joint_4_angles.inc"
};
static NJS_MKEY_A biolizard_motion_13_joint_5_angles[87] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_13_joint_5_angles.inc"
};
static NJS_MKEY_A biolizard_motion_13_joint_6_angles[90] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_13_joint_6_angles.inc"
};
static NJS_MKEY_A biolizard_motion_13_joint_7_angles[90] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_13_joint_7_angles.inc"
};
static NJS_MKEY_A biolizard_motion_13_joint_8_angles[90] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_13_joint_8_angles.inc"
};
static NJS_MKEY_A biolizard_motion_13_joint_9_angles[87] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_13_joint_9_angles.inc"
};
static NJS_MKEY_A biolizard_motion_13_joint_12_angles[54] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_13_joint_12_angles.inc"
};
static NJS_MKEY_A biolizard_motion_13_joint_13_angles[54] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_13_joint_13_angles.inc"
};
static NJS_MKEY_A biolizard_motion_13_joint_16_angles[58] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_13_joint_16_angles.inc"
};
static NJS_MKEY_A biolizard_motion_13_joint_17_angles[49] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_13_joint_17_angles.inc"
};
static NJS_MKEY_A biolizard_motion_13_joint_20_angles[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_13_joint_20_angles.inc"
};
static NJS_MKEY_A biolizard_motion_13_joint_21_angles[80] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_13_joint_21_angles.inc"
};
static NJS_MKEY_A biolizard_motion_13_joint_22_angles[77] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_13_joint_22_angles.inc"
};
static NJS_MKEY_A biolizard_motion_13_joint_23_angles[84] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_13_joint_23_angles.inc"
};
static NJS_MKEY_A biolizard_motion_13_joint_24_angles[87] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_13_joint_24_angles.inc"
};
static NJS_MKEY_A biolizard_motion_13_joint_27_angles[90] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_13_joint_27_angles.inc"
};
static NJS_MKEY_A biolizard_motion_13_joint_28_angles[90] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_13_joint_28_angles.inc"
};
static NJS_MKEY_A biolizard_motion_13_joint_31_angles[90] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_13_joint_31_angles.inc"
};
static NJS_MKEY_A biolizard_motion_13_joint_34_angles[90] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_13_joint_34_angles.inc"
};
static NJS_MKEY_F biolizard_motion_13_joint_36_position[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_13_joint_36_position.inc"
};
static NJS_MKEY_F biolizard_motion_13_joint_38_position[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_13_joint_38_position.inc"
};
static NJS_MKEY_F biolizard_motion_13_joint_39_position[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_13_joint_39_position.inc"
};
static NJS_MKEY_F biolizard_motion_13_joint_41_position[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_13_joint_41_position.inc"
};
static NJS_MKEY_F biolizard_motion_13_joint_42_position[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_13_joint_42_position.inc"
};
static NJS_MKEY_F biolizard_motion_13_joint_43_position[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_13_joint_43_position.inc"
};
static NJS_MKEY_A biolizard_motion_13_joint_43_angles[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_13_joint_43_angles.inc"
};
static NJS_MDATA2 biolizard_motion_13_tracks[44] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_13_tracks.inc"
};
NJS_MOTION biolizard_motion_13 = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_13.inc"
};
static NJS_MKEY_F biolizard_motion_15_joint_0_position[32] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_15_joint_0_position.inc"
};
static NJS_MKEY_A biolizard_motion_15_joint_0_angles[31] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_15_joint_0_angles.inc"
};
static NJS_MKEY_F biolizard_motion_15_joint_1_position[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_15_joint_1_position.inc"
};
static NJS_MKEY_A biolizard_motion_15_joint_3_angles[32] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_15_joint_3_angles.inc"
};
static NJS_MKEY_A biolizard_motion_15_joint_4_angles[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_15_joint_4_angles.inc"
};
static NJS_MKEY_A biolizard_motion_15_joint_5_angles[26] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_15_joint_5_angles.inc"
};
static NJS_MKEY_A biolizard_motion_15_joint_6_angles[31] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_15_joint_6_angles.inc"
};
static NJS_MKEY_A biolizard_motion_15_joint_7_angles[32] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_15_joint_7_angles.inc"
};
static NJS_MKEY_A biolizard_motion_15_joint_8_angles[32] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_15_joint_8_angles.inc"
};
static NJS_MKEY_A biolizard_motion_15_joint_9_angles[32] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_15_joint_9_angles.inc"
};
static NJS_MKEY_A biolizard_motion_15_joint_12_angles[32] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_15_joint_12_angles.inc"
};
static NJS_MKEY_A biolizard_motion_15_joint_13_angles[32] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_15_joint_13_angles.inc"
};
static NJS_MKEY_A biolizard_motion_15_joint_16_angles[32] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_15_joint_16_angles.inc"
};
static NJS_MKEY_A biolizard_motion_15_joint_17_angles[32] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_15_joint_17_angles.inc"
};
static NJS_MKEY_A biolizard_motion_15_joint_20_angles[32] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_15_joint_20_angles.inc"
};
static NJS_MKEY_A biolizard_motion_15_joint_21_angles[30] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_15_joint_21_angles.inc"
};
static NJS_MKEY_A biolizard_motion_15_joint_22_angles[32] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_15_joint_22_angles.inc"
};
static NJS_MKEY_A biolizard_motion_15_joint_23_angles[32] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_15_joint_23_angles.inc"
};
static NJS_MKEY_A biolizard_motion_15_joint_24_angles[32] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_15_joint_24_angles.inc"
};
static NJS_MKEY_A biolizard_motion_15_joint_27_angles[32] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_15_joint_27_angles.inc"
};
static NJS_MKEY_A biolizard_motion_15_joint_28_angles[32] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_15_joint_28_angles.inc"
};
static NJS_MKEY_A biolizard_motion_15_joint_31_angles[32] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_15_joint_31_angles.inc"
};
static NJS_MKEY_A biolizard_motion_15_joint_34_angles[32] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_15_joint_34_angles.inc"
};
static NJS_MKEY_F biolizard_motion_15_joint_36_position[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_15_joint_36_position.inc"
};
static NJS_MKEY_F biolizard_motion_15_joint_38_position[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_15_joint_38_position.inc"
};
static NJS_MKEY_F biolizard_motion_15_joint_39_position[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_15_joint_39_position.inc"
};
static NJS_MKEY_F biolizard_motion_15_joint_41_position[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_15_joint_41_position.inc"
};
static NJS_MKEY_F biolizard_motion_15_joint_42_position[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_15_joint_42_position.inc"
};
static NJS_MKEY_F biolizard_motion_15_joint_43_position[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_15_joint_43_position.inc"
};
static NJS_MKEY_A biolizard_motion_15_joint_43_angles[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_15_joint_43_angles.inc"
};
static NJS_MDATA2 biolizard_motion_15_tracks[44] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_15_tracks.inc"
};
NJS_MOTION biolizard_motion_15 = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_15.inc"
};
static NJS_MKEY_F biolizard_motion_3_joint_0_position[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_3_joint_0_position.inc"
};
static NJS_MKEY_A biolizard_motion_3_joint_0_angles[56] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_3_joint_0_angles.inc"
};
static NJS_MKEY_F biolizard_motion_3_joint_1_position[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_3_joint_1_position.inc"
};
static NJS_MKEY_A biolizard_motion_3_joint_3_angles[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_3_joint_3_angles.inc"
};
static NJS_MKEY_A biolizard_motion_3_joint_4_angles[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_3_joint_4_angles.inc"
};
static NJS_MKEY_A biolizard_motion_3_joint_5_angles[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_3_joint_5_angles.inc"
};
static NJS_MKEY_A biolizard_motion_3_joint_6_angles[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_3_joint_6_angles.inc"
};
static NJS_MKEY_A biolizard_motion_3_joint_7_angles[64] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_3_joint_7_angles.inc"
};
static NJS_MKEY_A biolizard_motion_3_joint_8_angles[64] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_3_joint_8_angles.inc"
};
static NJS_MKEY_A biolizard_motion_3_joint_9_angles[64] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_3_joint_9_angles.inc"
};
static NJS_MKEY_A biolizard_motion_3_joint_12_angles[64] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_3_joint_12_angles.inc"
};
static NJS_MKEY_A biolizard_motion_3_joint_13_angles[64] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_3_joint_13_angles.inc"
};
static NJS_MKEY_A biolizard_motion_3_joint_16_angles[64] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_3_joint_16_angles.inc"
};
static NJS_MKEY_A biolizard_motion_3_joint_17_angles[64] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_3_joint_17_angles.inc"
};
static NJS_MKEY_A biolizard_motion_3_joint_20_angles[63] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_3_joint_20_angles.inc"
};
static NJS_MKEY_A biolizard_motion_3_joint_21_angles[63] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_3_joint_21_angles.inc"
};
static NJS_MKEY_A biolizard_motion_3_joint_22_angles[64] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_3_joint_22_angles.inc"
};
static NJS_MKEY_A biolizard_motion_3_joint_23_angles[64] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_3_joint_23_angles.inc"
};
static NJS_MKEY_A biolizard_motion_3_joint_24_angles[64] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_3_joint_24_angles.inc"
};
static NJS_MKEY_A biolizard_motion_3_joint_27_angles[64] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_3_joint_27_angles.inc"
};
static NJS_MKEY_A biolizard_motion_3_joint_28_angles[64] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_3_joint_28_angles.inc"
};
static NJS_MKEY_A biolizard_motion_3_joint_31_angles[64] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_3_joint_31_angles.inc"
};
static NJS_MKEY_A biolizard_motion_3_joint_34_angles[64] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_3_joint_34_angles.inc"
};
static NJS_MKEY_F biolizard_motion_3_joint_36_position[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_3_joint_36_position.inc"
};
static NJS_MKEY_F biolizard_motion_3_joint_38_position[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_3_joint_38_position.inc"
};
static NJS_MKEY_F biolizard_motion_3_joint_39_position[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_3_joint_39_position.inc"
};
static NJS_MKEY_F biolizard_motion_3_joint_41_position[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_3_joint_41_position.inc"
};
static NJS_MKEY_F biolizard_motion_3_joint_42_position[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_3_joint_42_position.inc"
};
static NJS_MKEY_F biolizard_motion_3_joint_43_position[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_3_joint_43_position.inc"
};
static NJS_MDATA2 biolizard_motion_3_tracks[44] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_3_tracks.inc"
};
NJS_MOTION biolizard_motion_3 = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_3.inc"
};
static NJS_MKEY_F biolizard_motion_3_partner_joint_0_position[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_3_partner_joint_0_position.inc"
};
static NJS_MKEY_A biolizard_motion_3_partner_joint_0_angles[56] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_3_partner_joint_0_angles.inc"
};
static NJS_MKEY_F biolizard_motion_3_partner_joint_1_position[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_3_partner_joint_1_position.inc"
};
static NJS_MKEY_A biolizard_motion_3_partner_joint_3_angles[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_3_partner_joint_3_angles.inc"
};
static NJS_MKEY_A biolizard_motion_3_partner_joint_4_angles[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_3_partner_joint_4_angles.inc"
};
static NJS_MKEY_A biolizard_motion_3_partner_joint_5_angles[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_3_partner_joint_5_angles.inc"
};
static NJS_MKEY_F biolizard_motion_3_partner_joint_6_position[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_3_partner_joint_6_position.inc"
};
static NJS_MKEY_A biolizard_motion_3_partner_joint_6_angles[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_3_partner_joint_6_angles.inc"
};
static NJS_MKEY_F biolizard_motion_3_partner_joint_7_position[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_3_partner_joint_7_position.inc"
};
static NJS_MKEY_A biolizard_motion_3_partner_joint_7_angles[64] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_3_partner_joint_7_angles.inc"
};
static NJS_MKEY_F biolizard_motion_3_partner_joint_8_position[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_3_partner_joint_8_position.inc"
};
static NJS_MKEY_A biolizard_motion_3_partner_joint_8_angles[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_3_partner_joint_8_angles.inc"
};
static NJS_MKEY_F biolizard_motion_3_partner_joint_9_position[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_3_partner_joint_9_position.inc"
};
static NJS_MKEY_A biolizard_motion_3_partner_joint_9_angles[64] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_3_partner_joint_9_angles.inc"
};
static NJS_MKEY_F biolizard_motion_3_partner_joint_10_position[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_3_partner_joint_10_position.inc"
};
static NJS_MKEY_A biolizard_motion_3_partner_joint_12_angles[64] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_3_partner_joint_12_angles.inc"
};
static NJS_MKEY_A biolizard_motion_3_partner_joint_13_angles[64] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_3_partner_joint_13_angles.inc"
};
static NJS_MKEY_A biolizard_motion_3_partner_joint_16_angles[64] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_3_partner_joint_16_angles.inc"
};
static NJS_MKEY_A biolizard_motion_3_partner_joint_17_angles[64] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_3_partner_joint_17_angles.inc"
};
static NJS_MKEY_A biolizard_motion_3_partner_joint_20_angles[63] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_3_partner_joint_20_angles.inc"
};
static NJS_MKEY_A biolizard_motion_3_partner_joint_21_angles[63] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_3_partner_joint_21_angles.inc"
};
static NJS_MKEY_A biolizard_motion_3_partner_joint_22_angles[64] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_3_partner_joint_22_angles.inc"
};
static NJS_MKEY_A biolizard_motion_3_partner_joint_23_angles[64] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_3_partner_joint_23_angles.inc"
};
static NJS_MKEY_A biolizard_motion_3_partner_joint_24_angles[64] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_3_partner_joint_24_angles.inc"
};
static NJS_MKEY_A biolizard_motion_3_partner_joint_27_angles[64] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_3_partner_joint_27_angles.inc"
};
static NJS_MKEY_A biolizard_motion_3_partner_joint_28_angles[64] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_3_partner_joint_28_angles.inc"
};
static NJS_MKEY_A biolizard_motion_3_partner_joint_31_angles[64] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_3_partner_joint_31_angles.inc"
};
static NJS_MKEY_A biolizard_motion_3_partner_joint_34_angles[64] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_3_partner_joint_34_angles.inc"
};
static NJS_MKEY_F biolizard_motion_3_partner_joint_36_position[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_3_partner_joint_36_position.inc"
};
static NJS_MKEY_F biolizard_motion_3_partner_joint_38_position[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_3_partner_joint_38_position.inc"
};
static NJS_MKEY_F biolizard_motion_3_partner_joint_39_position[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_3_partner_joint_39_position.inc"
};
static NJS_MKEY_F biolizard_motion_3_partner_joint_41_position[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_3_partner_joint_41_position.inc"
};
static NJS_MKEY_F biolizard_motion_3_partner_joint_42_position[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_3_partner_joint_42_position.inc"
};
static NJS_MKEY_F biolizard_motion_3_partner_joint_43_position[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_3_partner_joint_43_position.inc"
};
static NJS_MDATA2 biolizard_motion_3_partner_tracks[44] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_3_partner_tracks.inc"
};
NJS_MOTION biolizard_motion_3_partner = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_3_partner.inc"
};
static NJS_MKEY_F biolizard_motion_5_joint_0_position[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_5_joint_0_position.inc"
};
static NJS_MKEY_A biolizard_motion_5_joint_0_angles[58] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_5_joint_0_angles.inc"
};
static NJS_MKEY_F biolizard_motion_5_joint_1_position[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_5_joint_1_position.inc"
};
static NJS_MKEY_A biolizard_motion_5_joint_3_angles[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_5_joint_3_angles.inc"
};
static NJS_MKEY_A biolizard_motion_5_joint_4_angles[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_5_joint_4_angles.inc"
};
static NJS_MKEY_A biolizard_motion_5_joint_5_angles[54] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_5_joint_5_angles.inc"
};
static NJS_MKEY_A biolizard_motion_5_joint_6_angles[3] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_5_joint_6_angles.inc"
};
static NJS_MKEY_A biolizard_motion_5_joint_7_angles[63] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_5_joint_7_angles.inc"
};
static NJS_MKEY_A biolizard_motion_5_joint_8_angles[61] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_5_joint_8_angles.inc"
};
static NJS_MKEY_A biolizard_motion_5_joint_9_angles[64] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_5_joint_9_angles.inc"
};
static NJS_MKEY_A biolizard_motion_5_joint_12_angles[64] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_5_joint_12_angles.inc"
};
static NJS_MKEY_A biolizard_motion_5_joint_13_angles[64] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_5_joint_13_angles.inc"
};
static NJS_MKEY_A biolizard_motion_5_joint_16_angles[64] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_5_joint_16_angles.inc"
};
static NJS_MKEY_A biolizard_motion_5_joint_17_angles[64] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_5_joint_17_angles.inc"
};
static NJS_MKEY_A biolizard_motion_5_joint_20_angles[63] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_5_joint_20_angles.inc"
};
static NJS_MKEY_A biolizard_motion_5_joint_21_angles[63] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_5_joint_21_angles.inc"
};
static NJS_MKEY_A biolizard_motion_5_joint_22_angles[64] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_5_joint_22_angles.inc"
};
static NJS_MKEY_A biolizard_motion_5_joint_23_angles[64] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_5_joint_23_angles.inc"
};
static NJS_MKEY_A biolizard_motion_5_joint_24_angles[64] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_5_joint_24_angles.inc"
};
static NJS_MKEY_A biolizard_motion_5_joint_27_angles[64] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_5_joint_27_angles.inc"
};
static NJS_MKEY_A biolizard_motion_5_joint_28_angles[64] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_5_joint_28_angles.inc"
};
static NJS_MKEY_A biolizard_motion_5_joint_31_angles[64] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_5_joint_31_angles.inc"
};
static NJS_MKEY_A biolizard_motion_5_joint_34_angles[64] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_5_joint_34_angles.inc"
};
static NJS_MKEY_F biolizard_motion_5_joint_36_position[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_5_joint_36_position.inc"
};
static NJS_MKEY_F biolizard_motion_5_joint_38_position[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_5_joint_38_position.inc"
};
static NJS_MKEY_F biolizard_motion_5_joint_39_position[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_5_joint_39_position.inc"
};
static NJS_MKEY_F biolizard_motion_5_joint_41_position[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_5_joint_41_position.inc"
};
static NJS_MKEY_F biolizard_motion_5_joint_42_position[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_5_joint_42_position.inc"
};
static NJS_MKEY_F biolizard_motion_5_joint_43_position[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_5_joint_43_position.inc"
};
static NJS_MDATA2 biolizard_motion_5_tracks[44] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_5_tracks.inc"
};
NJS_MOTION biolizard_motion_5 = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_5.inc"
};
static NJS_MKEY_F biolizard_motion_5_partner_joint_0_position[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_5_partner_joint_0_position.inc"
};
static NJS_MKEY_A biolizard_motion_5_partner_joint_0_angles[58] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_5_partner_joint_0_angles.inc"
};
static NJS_MKEY_F biolizard_motion_5_partner_joint_1_position[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_5_partner_joint_1_position.inc"
};
static NJS_MKEY_A biolizard_motion_5_partner_joint_3_angles[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_5_partner_joint_3_angles.inc"
};
static NJS_MKEY_A biolizard_motion_5_partner_joint_4_angles[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_5_partner_joint_4_angles.inc"
};
static NJS_MKEY_A biolizard_motion_5_partner_joint_5_angles[54] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_5_partner_joint_5_angles.inc"
};
static NJS_MKEY_F biolizard_motion_5_partner_joint_6_position[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_5_partner_joint_6_position.inc"
};
static NJS_MKEY_A biolizard_motion_5_partner_joint_6_angles[3] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_5_partner_joint_6_angles.inc"
};
static NJS_MKEY_F biolizard_motion_5_partner_joint_7_position[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_5_partner_joint_7_position.inc"
};
static NJS_MKEY_A biolizard_motion_5_partner_joint_7_angles[35] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_5_partner_joint_7_angles.inc"
};
static NJS_MKEY_F biolizard_motion_5_partner_joint_8_position[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_5_partner_joint_8_position.inc"
};
static NJS_MKEY_A biolizard_motion_5_partner_joint_8_angles[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_5_partner_joint_8_angles.inc"
};
static NJS_MKEY_F biolizard_motion_5_partner_joint_9_position[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_5_partner_joint_9_position.inc"
};
static NJS_MKEY_A biolizard_motion_5_partner_joint_9_angles[64] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_5_partner_joint_9_angles.inc"
};
static NJS_MKEY_F biolizard_motion_5_partner_joint_10_position[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_5_partner_joint_10_position.inc"
};
static NJS_MKEY_A biolizard_motion_5_partner_joint_12_angles[64] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_5_partner_joint_12_angles.inc"
};
static NJS_MKEY_A biolizard_motion_5_partner_joint_13_angles[64] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_5_partner_joint_13_angles.inc"
};
static NJS_MKEY_A biolizard_motion_5_partner_joint_16_angles[64] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_5_partner_joint_16_angles.inc"
};
static NJS_MKEY_A biolizard_motion_5_partner_joint_17_angles[64] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_5_partner_joint_17_angles.inc"
};
static NJS_MKEY_A biolizard_motion_5_partner_joint_20_angles[63] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_5_partner_joint_20_angles.inc"
};
static NJS_MKEY_A biolizard_motion_5_partner_joint_21_angles[63] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_5_partner_joint_21_angles.inc"
};
static NJS_MKEY_A biolizard_motion_5_partner_joint_22_angles[64] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_5_partner_joint_22_angles.inc"
};
static NJS_MKEY_A biolizard_motion_5_partner_joint_23_angles[64] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_5_partner_joint_23_angles.inc"
};
static NJS_MKEY_A biolizard_motion_5_partner_joint_24_angles[64] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_5_partner_joint_24_angles.inc"
};
static NJS_MKEY_A biolizard_motion_5_partner_joint_27_angles[64] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_5_partner_joint_27_angles.inc"
};
static NJS_MKEY_A biolizard_motion_5_partner_joint_28_angles[64] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_5_partner_joint_28_angles.inc"
};
static NJS_MKEY_A biolizard_motion_5_partner_joint_31_angles[64] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_5_partner_joint_31_angles.inc"
};
static NJS_MKEY_A biolizard_motion_5_partner_joint_34_angles[64] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_5_partner_joint_34_angles.inc"
};
static NJS_MKEY_F biolizard_motion_5_partner_joint_36_position[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_5_partner_joint_36_position.inc"
};
static NJS_MKEY_F biolizard_motion_5_partner_joint_38_position[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_5_partner_joint_38_position.inc"
};
static NJS_MKEY_F biolizard_motion_5_partner_joint_39_position[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_5_partner_joint_39_position.inc"
};
static NJS_MKEY_F biolizard_motion_5_partner_joint_41_position[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_5_partner_joint_41_position.inc"
};
static NJS_MKEY_F biolizard_motion_5_partner_joint_42_position[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_5_partner_joint_42_position.inc"
};
static NJS_MKEY_F biolizard_motion_5_partner_joint_43_position[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_5_partner_joint_43_position.inc"
};
static NJS_MDATA2 biolizard_motion_5_partner_tracks[44] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_5_partner_tracks.inc"
};
NJS_MOTION biolizard_motion_5_partner = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_5_partner.inc"
};
static NJS_MKEY_F biolizard_motion_11_joint_0_position[32] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_11_joint_0_position.inc"
};
static NJS_MKEY_F biolizard_motion_11_joint_1_position[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_11_joint_1_position.inc"
};
static NJS_MKEY_A biolizard_motion_11_joint_3_angles[17] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_11_joint_3_angles.inc"
};
static NJS_MKEY_A biolizard_motion_11_joint_4_angles[32] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_11_joint_4_angles.inc"
};
static NJS_MKEY_A biolizard_motion_11_joint_5_angles[32] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_11_joint_5_angles.inc"
};
static NJS_MKEY_F biolizard_motion_11_joint_6_position[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_11_joint_6_position.inc"
};
static NJS_MKEY_A biolizard_motion_11_joint_6_angles[32] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_11_joint_6_angles.inc"
};
static NJS_MKEY_F biolizard_motion_11_joint_7_position[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_11_joint_7_position.inc"
};
static NJS_MKEY_A biolizard_motion_11_joint_7_angles[32] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_11_joint_7_angles.inc"
};
static NJS_MKEY_F biolizard_motion_11_joint_8_position[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_11_joint_8_position.inc"
};
static NJS_MKEY_A biolizard_motion_11_joint_8_angles[32] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_11_joint_8_angles.inc"
};
static NJS_MKEY_F biolizard_motion_11_joint_9_position[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_11_joint_9_position.inc"
};
static NJS_MKEY_A biolizard_motion_11_joint_9_angles[32] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_11_joint_9_angles.inc"
};
static NJS_MKEY_F biolizard_motion_11_joint_10_position[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_11_joint_10_position.inc"
};
static NJS_MKEY_A biolizard_motion_11_joint_12_angles[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_11_joint_12_angles.inc"
};
static NJS_MKEY_A biolizard_motion_11_joint_13_angles[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_11_joint_13_angles.inc"
};
static NJS_MKEY_A biolizard_motion_11_joint_16_angles[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_11_joint_16_angles.inc"
};
static NJS_MKEY_A biolizard_motion_11_joint_17_angles[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_11_joint_17_angles.inc"
};
static NJS_MKEY_A biolizard_motion_11_joint_20_angles[32] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_11_joint_20_angles.inc"
};
static NJS_MKEY_A biolizard_motion_11_joint_21_angles[32] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_11_joint_21_angles.inc"
};
static NJS_MKEY_A biolizard_motion_11_joint_22_angles[32] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_11_joint_22_angles.inc"
};
static NJS_MKEY_A biolizard_motion_11_joint_23_angles[32] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_11_joint_23_angles.inc"
};
static NJS_MKEY_A biolizard_motion_11_joint_24_angles[32] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_11_joint_24_angles.inc"
};
static NJS_MKEY_A biolizard_motion_11_joint_27_angles[32] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_11_joint_27_angles.inc"
};
static NJS_MKEY_A biolizard_motion_11_joint_28_angles[32] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_11_joint_28_angles.inc"
};
static NJS_MKEY_A biolizard_motion_11_joint_31_angles[32] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_11_joint_31_angles.inc"
};
static NJS_MKEY_A biolizard_motion_11_joint_34_angles[32] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_11_joint_34_angles.inc"
};
static NJS_MKEY_F biolizard_motion_11_joint_36_position[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_11_joint_36_position.inc"
};
static NJS_MKEY_F biolizard_motion_11_joint_38_position[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_11_joint_38_position.inc"
};
static NJS_MKEY_F biolizard_motion_11_joint_39_position[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_11_joint_39_position.inc"
};
static NJS_MKEY_F biolizard_motion_11_joint_41_position[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_11_joint_41_position.inc"
};
static NJS_MKEY_F biolizard_motion_11_joint_42_position[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_11_joint_42_position.inc"
};
static NJS_MKEY_F biolizard_motion_11_joint_43_position[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_11_joint_43_position.inc"
};
static NJS_MDATA2 biolizard_motion_11_tracks[44] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_11_tracks.inc"
};
NJS_MOTION biolizard_motion_11 = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_11.inc"
};
static NJS_MKEY_F biolizard_motion_11_partner_joint_0_position[32] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_11_partner_joint_0_position.inc"
};
static NJS_MKEY_F biolizard_motion_11_partner_joint_1_position[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_11_partner_joint_1_position.inc"
};
static NJS_MKEY_A biolizard_motion_11_partner_joint_3_angles[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_11_partner_joint_3_angles.inc"
};
static NJS_MKEY_A biolizard_motion_11_partner_joint_4_angles[32] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_11_partner_joint_4_angles.inc"
};
static NJS_MKEY_A biolizard_motion_11_partner_joint_5_angles[32] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_11_partner_joint_5_angles.inc"
};
static NJS_MKEY_F biolizard_motion_11_partner_joint_6_position[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_11_partner_joint_6_position.inc"
};
static NJS_MKEY_A biolizard_motion_11_partner_joint_6_angles[32] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_11_partner_joint_6_angles.inc"
};
static NJS_MKEY_F biolizard_motion_11_partner_joint_7_position[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_11_partner_joint_7_position.inc"
};
static NJS_MKEY_A biolizard_motion_11_partner_joint_7_angles[32] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_11_partner_joint_7_angles.inc"
};
static NJS_MKEY_F biolizard_motion_11_partner_joint_8_position[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_11_partner_joint_8_position.inc"
};
static NJS_MKEY_A biolizard_motion_11_partner_joint_8_angles[32] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_11_partner_joint_8_angles.inc"
};
static NJS_MKEY_F biolizard_motion_11_partner_joint_9_position[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_11_partner_joint_9_position.inc"
};
static NJS_MKEY_A biolizard_motion_11_partner_joint_9_angles[32] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_11_partner_joint_9_angles.inc"
};
static NJS_MKEY_F biolizard_motion_11_partner_joint_10_position[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_11_partner_joint_10_position.inc"
};
static NJS_MKEY_A biolizard_motion_11_partner_joint_12_angles[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_11_partner_joint_12_angles.inc"
};
static NJS_MKEY_A biolizard_motion_11_partner_joint_13_angles[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_11_partner_joint_13_angles.inc"
};
static NJS_MKEY_A biolizard_motion_11_partner_joint_16_angles[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_11_partner_joint_16_angles.inc"
};
static NJS_MKEY_A biolizard_motion_11_partner_joint_17_angles[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_11_partner_joint_17_angles.inc"
};
static NJS_MKEY_A biolizard_motion_11_partner_joint_20_angles[32] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_11_partner_joint_20_angles.inc"
};
static NJS_MKEY_A biolizard_motion_11_partner_joint_21_angles[32] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_11_partner_joint_21_angles.inc"
};
static NJS_MKEY_A biolizard_motion_11_partner_joint_22_angles[32] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_11_partner_joint_22_angles.inc"
};
static NJS_MKEY_A biolizard_motion_11_partner_joint_23_angles[32] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_11_partner_joint_23_angles.inc"
};
static NJS_MKEY_A biolizard_motion_11_partner_joint_24_angles[32] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_11_partner_joint_24_angles.inc"
};
static NJS_MKEY_A biolizard_motion_11_partner_joint_27_angles[32] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_11_partner_joint_27_angles.inc"
};
static NJS_MKEY_A biolizard_motion_11_partner_joint_28_angles[32] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_11_partner_joint_28_angles.inc"
};
static NJS_MKEY_A biolizard_motion_11_partner_joint_31_angles[32] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_11_partner_joint_31_angles.inc"
};
static NJS_MKEY_A biolizard_motion_11_partner_joint_34_angles[32] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_11_partner_joint_34_angles.inc"
};
static NJS_MKEY_F biolizard_motion_11_partner_joint_36_position[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_11_partner_joint_36_position.inc"
};
static NJS_MKEY_F biolizard_motion_11_partner_joint_38_position[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_11_partner_joint_38_position.inc"
};
static NJS_MKEY_F biolizard_motion_11_partner_joint_39_position[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_11_partner_joint_39_position.inc"
};
static NJS_MKEY_F biolizard_motion_11_partner_joint_41_position[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_11_partner_joint_41_position.inc"
};
static NJS_MKEY_F biolizard_motion_11_partner_joint_42_position[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_11_partner_joint_42_position.inc"
};
static NJS_MKEY_F biolizard_motion_11_partner_joint_43_position[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_11_partner_joint_43_position.inc"
};
static NJS_MDATA2 biolizard_motion_11_partner_tracks[44] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_11_partner_tracks.inc"
};
NJS_MOTION biolizard_motion_11_partner = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_11_partner.inc"
};
static NJS_MKEY_F biolizard_motion_12_joint_0_position[32] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_12_joint_0_position.inc"
};
static NJS_MKEY_F biolizard_motion_12_joint_1_position[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_12_joint_1_position.inc"
};
static NJS_MKEY_A biolizard_motion_12_joint_3_angles[30] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_12_joint_3_angles.inc"
};
static NJS_MKEY_A biolizard_motion_12_joint_4_angles[32] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_12_joint_4_angles.inc"
};
static NJS_MKEY_A biolizard_motion_12_joint_5_angles[32] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_12_joint_5_angles.inc"
};
static NJS_MKEY_F biolizard_motion_12_joint_6_position[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_12_joint_6_position.inc"
};
static NJS_MKEY_A biolizard_motion_12_joint_6_angles[25] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_12_joint_6_angles.inc"
};
static NJS_MKEY_F biolizard_motion_12_joint_7_position[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_12_joint_7_position.inc"
};
static NJS_MKEY_A biolizard_motion_12_joint_7_angles[32] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_12_joint_7_angles.inc"
};
static NJS_MKEY_F biolizard_motion_12_joint_8_position[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_12_joint_8_position.inc"
};
static NJS_MKEY_A biolizard_motion_12_joint_8_angles[32] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_12_joint_8_angles.inc"
};
static NJS_MKEY_F biolizard_motion_12_joint_9_position[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_12_joint_9_position.inc"
};
static NJS_MKEY_A biolizard_motion_12_joint_9_angles[10] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_12_joint_9_angles.inc"
};
static NJS_MKEY_F biolizard_motion_12_joint_10_position[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_12_joint_10_position.inc"
};
static NJS_MKEY_A biolizard_motion_12_joint_12_angles[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_12_joint_12_angles.inc"
};
static NJS_MKEY_A biolizard_motion_12_joint_13_angles[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_12_joint_13_angles.inc"
};
static NJS_MKEY_A biolizard_motion_12_joint_16_angles[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_12_joint_16_angles.inc"
};
static NJS_MKEY_A biolizard_motion_12_joint_17_angles[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_12_joint_17_angles.inc"
};
static NJS_MKEY_A biolizard_motion_12_joint_20_angles[32] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_12_joint_20_angles.inc"
};
static NJS_MKEY_A biolizard_motion_12_joint_21_angles[32] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_12_joint_21_angles.inc"
};
static NJS_MKEY_A biolizard_motion_12_joint_22_angles[32] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_12_joint_22_angles.inc"
};
static NJS_MKEY_A biolizard_motion_12_joint_23_angles[32] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_12_joint_23_angles.inc"
};
static NJS_MKEY_A biolizard_motion_12_joint_24_angles[32] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_12_joint_24_angles.inc"
};
static NJS_MKEY_A biolizard_motion_12_joint_27_angles[32] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_12_joint_27_angles.inc"
};
static NJS_MKEY_A biolizard_motion_12_joint_28_angles[32] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_12_joint_28_angles.inc"
};
static NJS_MKEY_A biolizard_motion_12_joint_31_angles[32] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_12_joint_31_angles.inc"
};
static NJS_MKEY_A biolizard_motion_12_joint_34_angles[32] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_12_joint_34_angles.inc"
};
static NJS_MKEY_F biolizard_motion_12_joint_36_position[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_12_joint_36_position.inc"
};
static NJS_MKEY_F biolizard_motion_12_joint_38_position[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_12_joint_38_position.inc"
};
static NJS_MKEY_F biolizard_motion_12_joint_39_position[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_12_joint_39_position.inc"
};
static NJS_MKEY_F biolizard_motion_12_joint_41_position[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_12_joint_41_position.inc"
};
static NJS_MKEY_F biolizard_motion_12_joint_42_position[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_12_joint_42_position.inc"
};
static NJS_MKEY_F biolizard_motion_12_joint_43_position[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_12_joint_43_position.inc"
};
static NJS_MDATA2 biolizard_motion_12_tracks[44] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_12_tracks.inc"
};
NJS_MOTION biolizard_motion_12 = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_12.inc"
};
static NJS_MKEY_F biolizard_motion_12_partner_joint_0_position[32] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_12_partner_joint_0_position.inc"
};
static NJS_MKEY_F biolizard_motion_12_partner_joint_1_position[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_12_partner_joint_1_position.inc"
};
static NJS_MKEY_A biolizard_motion_12_partner_joint_3_angles[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_12_partner_joint_3_angles.inc"
};
static NJS_MKEY_A biolizard_motion_12_partner_joint_4_angles[31] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_12_partner_joint_4_angles.inc"
};
static NJS_MKEY_A biolizard_motion_12_partner_joint_5_angles[32] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_12_partner_joint_5_angles.inc"
};
static NJS_MKEY_F biolizard_motion_12_partner_joint_6_position[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_12_partner_joint_6_position.inc"
};
static NJS_MKEY_A biolizard_motion_12_partner_joint_6_angles[32] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_12_partner_joint_6_angles.inc"
};
static NJS_MKEY_F biolizard_motion_12_partner_joint_7_position[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_12_partner_joint_7_position.inc"
};
static NJS_MKEY_A biolizard_motion_12_partner_joint_7_angles[32] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_12_partner_joint_7_angles.inc"
};
static NJS_MKEY_F biolizard_motion_12_partner_joint_8_position[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_12_partner_joint_8_position.inc"
};
static NJS_MKEY_A biolizard_motion_12_partner_joint_8_angles[32] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_12_partner_joint_8_angles.inc"
};
static NJS_MKEY_F biolizard_motion_12_partner_joint_9_position[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_12_partner_joint_9_position.inc"
};
static NJS_MKEY_A biolizard_motion_12_partner_joint_9_angles[9] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_12_partner_joint_9_angles.inc"
};
static NJS_MKEY_F biolizard_motion_12_partner_joint_10_position[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_12_partner_joint_10_position.inc"
};
static NJS_MKEY_A biolizard_motion_12_partner_joint_12_angles[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_12_partner_joint_12_angles.inc"
};
static NJS_MKEY_A biolizard_motion_12_partner_joint_13_angles[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_12_partner_joint_13_angles.inc"
};
static NJS_MKEY_A biolizard_motion_12_partner_joint_16_angles[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_12_partner_joint_16_angles.inc"
};
static NJS_MKEY_A biolizard_motion_12_partner_joint_17_angles[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_12_partner_joint_17_angles.inc"
};
static NJS_MKEY_A biolizard_motion_12_partner_joint_20_angles[32] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_12_partner_joint_20_angles.inc"
};
static NJS_MKEY_A biolizard_motion_12_partner_joint_21_angles[32] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_12_partner_joint_21_angles.inc"
};
static NJS_MKEY_A biolizard_motion_12_partner_joint_22_angles[32] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_12_partner_joint_22_angles.inc"
};
static NJS_MKEY_A biolizard_motion_12_partner_joint_23_angles[32] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_12_partner_joint_23_angles.inc"
};
static NJS_MKEY_A biolizard_motion_12_partner_joint_24_angles[32] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_12_partner_joint_24_angles.inc"
};
static NJS_MKEY_A biolizard_motion_12_partner_joint_27_angles[32] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_12_partner_joint_27_angles.inc"
};
static NJS_MKEY_A biolizard_motion_12_partner_joint_28_angles[32] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_12_partner_joint_28_angles.inc"
};
static NJS_MKEY_A biolizard_motion_12_partner_joint_31_angles[32] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_12_partner_joint_31_angles.inc"
};
static NJS_MKEY_A biolizard_motion_12_partner_joint_34_angles[32] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_12_partner_joint_34_angles.inc"
};
static NJS_MKEY_F biolizard_motion_12_partner_joint_36_position[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_12_partner_joint_36_position.inc"
};
static NJS_MKEY_F biolizard_motion_12_partner_joint_38_position[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_12_partner_joint_38_position.inc"
};
static NJS_MKEY_F biolizard_motion_12_partner_joint_39_position[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_12_partner_joint_39_position.inc"
};
static NJS_MKEY_F biolizard_motion_12_partner_joint_41_position[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_12_partner_joint_41_position.inc"
};
static NJS_MKEY_F biolizard_motion_12_partner_joint_42_position[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_12_partner_joint_42_position.inc"
};
static NJS_MKEY_F biolizard_motion_12_partner_joint_43_position[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_12_partner_joint_43_position.inc"
};
static NJS_MDATA2 biolizard_motion_12_partner_tracks[44] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_12_partner_tracks.inc"
};
NJS_MOTION biolizard_motion_12_partner = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_12_partner.inc"
};
static NJS_MKEY_F unused_pose_a_joint_0_position[32] = {
#include "boss_last1/biolizard_model_textures/unused_pose_a_joint_0_position.inc"
};
static NJS_MKEY_F unused_pose_a_joint_1_position[2] = {
#include "boss_last1/biolizard_model_textures/unused_pose_a_joint_1_position.inc"
};
static NJS_MKEY_A unused_pose_a_joint_3_angles[3] = {
#include "boss_last1/biolizard_model_textures/unused_pose_a_joint_3_angles.inc"
};
static NJS_MKEY_A unused_pose_a_joint_4_angles[14] = {
#include "boss_last1/biolizard_model_textures/unused_pose_a_joint_4_angles.inc"
};
static NJS_MKEY_A unused_pose_a_joint_5_angles[5] = {
#include "boss_last1/biolizard_model_textures/unused_pose_a_joint_5_angles.inc"
};
static NJS_MKEY_A unused_pose_a_joint_6_angles[7] = {
#include "boss_last1/biolizard_model_textures/unused_pose_a_joint_6_angles.inc"
};
static NJS_MKEY_A unused_pose_a_joint_7_angles[5] = {
#include "boss_last1/biolizard_model_textures/unused_pose_a_joint_7_angles.inc"
};
static NJS_MKEY_A unused_pose_a_joint_8_angles[25] = {
#include "boss_last1/biolizard_model_textures/unused_pose_a_joint_8_angles.inc"
};
static NJS_MKEY_A unused_pose_a_joint_9_angles[2] = {
#include "boss_last1/biolizard_model_textures/unused_pose_a_joint_9_angles.inc"
};
static NJS_MKEY_A unused_pose_a_joint_12_angles[14] = {
#include "boss_last1/biolizard_model_textures/unused_pose_a_joint_12_angles.inc"
};
static NJS_MKEY_A unused_pose_a_joint_13_angles[2] = {
#include "boss_last1/biolizard_model_textures/unused_pose_a_joint_13_angles.inc"
};
static NJS_MKEY_A unused_pose_a_joint_16_angles[5] = {
#include "boss_last1/biolizard_model_textures/unused_pose_a_joint_16_angles.inc"
};
static NJS_MKEY_A unused_pose_a_joint_17_angles[2] = {
#include "boss_last1/biolizard_model_textures/unused_pose_a_joint_17_angles.inc"
};
static NJS_MKEY_A unused_pose_a_joint_20_angles[32] = {
#include "boss_last1/biolizard_model_textures/unused_pose_a_joint_20_angles.inc"
};
static NJS_MKEY_A unused_pose_a_joint_21_angles[32] = {
#include "boss_last1/biolizard_model_textures/unused_pose_a_joint_21_angles.inc"
};
static NJS_MKEY_A unused_pose_a_joint_22_angles[32] = {
#include "boss_last1/biolizard_model_textures/unused_pose_a_joint_22_angles.inc"
};
static NJS_MKEY_A unused_pose_a_joint_23_angles[32] = {
#include "boss_last1/biolizard_model_textures/unused_pose_a_joint_23_angles.inc"
};
static NJS_MKEY_A unused_pose_a_joint_24_angles[32] = {
#include "boss_last1/biolizard_model_textures/unused_pose_a_joint_24_angles.inc"
};
static NJS_MKEY_A unused_pose_a_joint_27_angles[32] = {
#include "boss_last1/biolizard_model_textures/unused_pose_a_joint_27_angles.inc"
};
static NJS_MKEY_A unused_pose_a_joint_28_angles[32] = {
#include "boss_last1/biolizard_model_textures/unused_pose_a_joint_28_angles.inc"
};
static NJS_MKEY_A unused_pose_a_joint_31_angles[32] = {
#include "boss_last1/biolizard_model_textures/unused_pose_a_joint_31_angles.inc"
};
static NJS_MKEY_A unused_pose_a_joint_34_angles[32] = {
#include "boss_last1/biolizard_model_textures/unused_pose_a_joint_34_angles.inc"
};
static NJS_MKEY_F unused_pose_a_joint_36_position[2] = {
#include "boss_last1/biolizard_model_textures/unused_pose_a_joint_36_position.inc"
};
static NJS_MKEY_F unused_pose_a_joint_38_position[2] = {
#include "boss_last1/biolizard_model_textures/unused_pose_a_joint_38_position.inc"
};
static NJS_MKEY_F unused_pose_a_joint_39_position[2] = {
#include "boss_last1/biolizard_model_textures/unused_pose_a_joint_39_position.inc"
};
static NJS_MKEY_F unused_pose_a_joint_41_position[2] = {
#include "boss_last1/biolizard_model_textures/unused_pose_a_joint_41_position.inc"
};
static NJS_MKEY_F unused_pose_a_joint_42_position[2] = {
#include "boss_last1/biolizard_model_textures/unused_pose_a_joint_42_position.inc"
};
static NJS_MKEY_F unused_pose_a_joint_43_position[2] = {
#include "boss_last1/biolizard_model_textures/unused_pose_a_joint_43_position.inc"
};
static NJS_MDATA2 unused_pose_a_tracks[44] = {
#include "boss_last1/biolizard_model_textures/unused_pose_a_tracks.inc"
};
static NJS_MOTION unused_pose_a = {
#include "boss_last1/biolizard_model_textures/unused_pose_a.inc"
};
static NJS_MKEY_F unused_pose_b_joint_0_position[32] = {
#include "boss_last1/biolizard_model_textures/unused_pose_b_joint_0_position.inc"
};
static NJS_MKEY_F unused_pose_b_joint_1_position[2] = {
#include "boss_last1/biolizard_model_textures/unused_pose_b_joint_1_position.inc"
};
static NJS_MKEY_A unused_pose_b_joint_3_angles[3] = {
#include "boss_last1/biolizard_model_textures/unused_pose_b_joint_3_angles.inc"
};
static NJS_MKEY_A unused_pose_b_joint_4_angles[14] = {
#include "boss_last1/biolizard_model_textures/unused_pose_b_joint_4_angles.inc"
};
static NJS_MKEY_A unused_pose_b_joint_5_angles[5] = {
#include "boss_last1/biolizard_model_textures/unused_pose_b_joint_5_angles.inc"
};
static NJS_MKEY_A unused_pose_b_joint_6_angles[7] = {
#include "boss_last1/biolizard_model_textures/unused_pose_b_joint_6_angles.inc"
};
static NJS_MKEY_A unused_pose_b_joint_7_angles[5] = {
#include "boss_last1/biolizard_model_textures/unused_pose_b_joint_7_angles.inc"
};
static NJS_MKEY_A unused_pose_b_joint_8_angles[25] = {
#include "boss_last1/biolizard_model_textures/unused_pose_b_joint_8_angles.inc"
};
static NJS_MKEY_A unused_pose_b_joint_9_angles[2] = {
#include "boss_last1/biolizard_model_textures/unused_pose_b_joint_9_angles.inc"
};
static NJS_MKEY_A unused_pose_b_joint_12_angles[14] = {
#include "boss_last1/biolizard_model_textures/unused_pose_b_joint_12_angles.inc"
};
static NJS_MKEY_A unused_pose_b_joint_13_angles[2] = {
#include "boss_last1/biolizard_model_textures/unused_pose_b_joint_13_angles.inc"
};
static NJS_MKEY_A unused_pose_b_joint_16_angles[5] = {
#include "boss_last1/biolizard_model_textures/unused_pose_b_joint_16_angles.inc"
};
static NJS_MKEY_A unused_pose_b_joint_17_angles[2] = {
#include "boss_last1/biolizard_model_textures/unused_pose_b_joint_17_angles.inc"
};
static NJS_MKEY_A unused_pose_b_joint_20_angles[32] = {
#include "boss_last1/biolizard_model_textures/unused_pose_b_joint_20_angles.inc"
};
static NJS_MKEY_A unused_pose_b_joint_21_angles[32] = {
#include "boss_last1/biolizard_model_textures/unused_pose_b_joint_21_angles.inc"
};
static NJS_MKEY_A unused_pose_b_joint_22_angles[32] = {
#include "boss_last1/biolizard_model_textures/unused_pose_b_joint_22_angles.inc"
};
static NJS_MKEY_A unused_pose_b_joint_23_angles[32] = {
#include "boss_last1/biolizard_model_textures/unused_pose_b_joint_23_angles.inc"
};
static NJS_MKEY_A unused_pose_b_joint_24_angles[32] = {
#include "boss_last1/biolizard_model_textures/unused_pose_b_joint_24_angles.inc"
};
static NJS_MKEY_A unused_pose_b_joint_27_angles[32] = {
#include "boss_last1/biolizard_model_textures/unused_pose_b_joint_27_angles.inc"
};
static NJS_MKEY_A unused_pose_b_joint_28_angles[32] = {
#include "boss_last1/biolizard_model_textures/unused_pose_b_joint_28_angles.inc"
};
static NJS_MKEY_A unused_pose_b_joint_31_angles[32] = {
#include "boss_last1/biolizard_model_textures/unused_pose_b_joint_31_angles.inc"
};
static NJS_MKEY_A unused_pose_b_joint_34_angles[32] = {
#include "boss_last1/biolizard_model_textures/unused_pose_b_joint_34_angles.inc"
};
static NJS_MKEY_F unused_pose_b_joint_36_position[2] = {
#include "boss_last1/biolizard_model_textures/unused_pose_b_joint_36_position.inc"
};
static NJS_MKEY_F unused_pose_b_joint_38_position[2] = {
#include "boss_last1/biolizard_model_textures/unused_pose_b_joint_38_position.inc"
};
static NJS_MKEY_F unused_pose_b_joint_39_position[2] = {
#include "boss_last1/biolizard_model_textures/unused_pose_b_joint_39_position.inc"
};
static NJS_MKEY_F unused_pose_b_joint_41_position[2] = {
#include "boss_last1/biolizard_model_textures/unused_pose_b_joint_41_position.inc"
};
static NJS_MKEY_F unused_pose_b_joint_42_position[2] = {
#include "boss_last1/biolizard_model_textures/unused_pose_b_joint_42_position.inc"
};
static NJS_MKEY_F unused_pose_b_joint_43_position[2] = {
#include "boss_last1/biolizard_model_textures/unused_pose_b_joint_43_position.inc"
};
static NJS_MDATA2 unused_pose_b_tracks[44] = {
#include "boss_last1/biolizard_model_textures/unused_pose_b_tracks.inc"
};
static NJS_MOTION unused_pose_b = {
#include "boss_last1/biolizard_model_textures/unused_pose_b.inc"
};
static NJS_MKEY_A biolizard_motion_17_joint_3_angles[58] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_17_joint_3_angles.inc"
};
static NJS_MKEY_A biolizard_motion_17_joint_4_angles[65] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_17_joint_4_angles.inc"
};
static NJS_MKEY_A biolizard_motion_17_joint_5_angles[65] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_17_joint_5_angles.inc"
};
static NJS_MKEY_A biolizard_motion_17_joint_6_angles[63] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_17_joint_6_angles.inc"
};
static NJS_MKEY_A biolizard_motion_17_joint_7_angles[64] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_17_joint_7_angles.inc"
};
static NJS_MKEY_A biolizard_motion_17_joint_8_angles[65] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_17_joint_8_angles.inc"
};
static NJS_MKEY_A biolizard_motion_17_joint_9_angles[65] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_17_joint_9_angles.inc"
};
static NJS_MKEY_A biolizard_motion_17_joint_20_angles[62] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_17_joint_20_angles.inc"
};
static NJS_MKEY_A biolizard_motion_17_joint_21_angles[65] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_17_joint_21_angles.inc"
};
static NJS_MKEY_A biolizard_motion_17_joint_22_angles[65] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_17_joint_22_angles.inc"
};
static NJS_MKEY_A biolizard_motion_17_joint_23_angles[65] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_17_joint_23_angles.inc"
};
static NJS_MKEY_A biolizard_motion_17_joint_24_angles[65] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_17_joint_24_angles.inc"
};
static NJS_MKEY_A biolizard_motion_17_joint_27_angles[65] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_17_joint_27_angles.inc"
};
static NJS_MKEY_A biolizard_motion_17_joint_28_angles[65] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_17_joint_28_angles.inc"
};
static NJS_MKEY_A biolizard_motion_17_joint_31_angles[65] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_17_joint_31_angles.inc"
};
static NJS_MKEY_A biolizard_motion_17_joint_34_angles[65] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_17_joint_34_angles.inc"
};
static NJS_MDATA2 biolizard_motion_17_tracks[44] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_17_tracks.inc"
};
NJS_MOTION biolizard_motion_17 = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_17.inc"
};
static NJS_MKEY_A biolizard_motion_18_joint_3_angles[58] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_18_joint_3_angles.inc"
};
static NJS_MKEY_A biolizard_motion_18_joint_4_angles[65] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_18_joint_4_angles.inc"
};
static NJS_MKEY_A biolizard_motion_18_joint_5_angles[65] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_18_joint_5_angles.inc"
};
static NJS_MKEY_A biolizard_motion_18_joint_6_angles[64] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_18_joint_6_angles.inc"
};
static NJS_MKEY_A biolizard_motion_18_joint_7_angles[64] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_18_joint_7_angles.inc"
};
static NJS_MKEY_A biolizard_motion_18_joint_8_angles[61] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_18_joint_8_angles.inc"
};
static NJS_MKEY_A biolizard_motion_18_joint_9_angles[65] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_18_joint_9_angles.inc"
};
static NJS_MKEY_A biolizard_motion_18_joint_20_angles[62] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_18_joint_20_angles.inc"
};
static NJS_MKEY_A biolizard_motion_18_joint_21_angles[65] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_18_joint_21_angles.inc"
};
static NJS_MKEY_A biolizard_motion_18_joint_22_angles[65] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_18_joint_22_angles.inc"
};
static NJS_MKEY_A biolizard_motion_18_joint_23_angles[65] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_18_joint_23_angles.inc"
};
static NJS_MKEY_A biolizard_motion_18_joint_24_angles[65] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_18_joint_24_angles.inc"
};
static NJS_MKEY_A biolizard_motion_18_joint_27_angles[65] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_18_joint_27_angles.inc"
};
static NJS_MKEY_A biolizard_motion_18_joint_28_angles[65] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_18_joint_28_angles.inc"
};
static NJS_MKEY_A biolizard_motion_18_joint_31_angles[65] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_18_joint_31_angles.inc"
};
static NJS_MKEY_A biolizard_motion_18_joint_34_angles[65] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_18_joint_34_angles.inc"
};
static NJS_MDATA2 biolizard_motion_18_tracks[44] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_18_tracks.inc"
};
NJS_MOTION biolizard_motion_18 = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_18.inc"
};
static NJS_MKEY_F biolizard_motion_0_joint_0_position[91] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_0_joint_0_position.inc"
};
static NJS_MKEY_A biolizard_motion_0_joint_0_angles[29] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_0_joint_0_angles.inc"
};
static NJS_MKEY_F biolizard_motion_0_joint_1_position[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_0_joint_1_position.inc"
};
static NJS_MKEY_A biolizard_motion_0_joint_3_angles[219] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_0_joint_3_angles.inc"
};
static NJS_MKEY_A biolizard_motion_0_joint_4_angles[222] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_0_joint_4_angles.inc"
};
static NJS_MKEY_A biolizard_motion_0_joint_5_angles[264] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_0_joint_5_angles.inc"
};
static NJS_MKEY_A biolizard_motion_0_joint_6_angles[260] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_0_joint_6_angles.inc"
};
static NJS_MKEY_A biolizard_motion_0_joint_7_angles[266] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_0_joint_7_angles.inc"
};
static NJS_MKEY_A biolizard_motion_0_joint_8_angles[270] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_0_joint_8_angles.inc"
};
static NJS_MKEY_A biolizard_motion_0_joint_9_angles[260] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_0_joint_9_angles.inc"
};
static NJS_MKEY_A biolizard_motion_0_joint_12_angles[69] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_0_joint_12_angles.inc"
};
static NJS_MKEY_A biolizard_motion_0_joint_13_angles[53] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_0_joint_13_angles.inc"
};
static NJS_MKEY_A biolizard_motion_0_joint_16_angles[50] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_0_joint_16_angles.inc"
};
static NJS_MKEY_A biolizard_motion_0_joint_17_angles[40] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_0_joint_17_angles.inc"
};
static NJS_MKEY_A biolizard_motion_0_joint_20_angles[255] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_0_joint_20_angles.inc"
};
static NJS_MKEY_A biolizard_motion_0_joint_21_angles[254] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_0_joint_21_angles.inc"
};
static NJS_MKEY_A biolizard_motion_0_joint_22_angles[253] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_0_joint_22_angles.inc"
};
static NJS_MKEY_A biolizard_motion_0_joint_23_angles[269] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_0_joint_23_angles.inc"
};
static NJS_MKEY_A biolizard_motion_0_joint_24_angles[269] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_0_joint_24_angles.inc"
};
static NJS_MKEY_A biolizard_motion_0_joint_27_angles[270] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_0_joint_27_angles.inc"
};
static NJS_MKEY_A biolizard_motion_0_joint_28_angles[270] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_0_joint_28_angles.inc"
};
static NJS_MKEY_A biolizard_motion_0_joint_31_angles[270] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_0_joint_31_angles.inc"
};
static NJS_MKEY_A biolizard_motion_0_joint_34_angles[270] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_0_joint_34_angles.inc"
};
static NJS_MKEY_F biolizard_motion_0_joint_36_position[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_0_joint_36_position.inc"
};
static NJS_MKEY_F biolizard_motion_0_joint_38_position[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_0_joint_38_position.inc"
};
static NJS_MKEY_F biolizard_motion_0_joint_39_position[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_0_joint_39_position.inc"
};
static NJS_MKEY_F biolizard_motion_0_joint_41_position[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_0_joint_41_position.inc"
};
static NJS_MKEY_F biolizard_motion_0_joint_42_position[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_0_joint_42_position.inc"
};
static NJS_MKEY_F biolizard_motion_0_joint_43_position[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_0_joint_43_position.inc"
};
static NJS_MKEY_A biolizard_motion_0_joint_43_angles[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_0_joint_43_angles.inc"
};
static NJS_MDATA2 biolizard_motion_0_tracks[44] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_0_tracks.inc"
};
NJS_MOTION biolizard_motion_0 = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_0.inc"
};
static NJS_MKEY_F biolizard_motion_19_joint_0_position[160] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_19_joint_0_position.inc"
};
static NJS_MKEY_A biolizard_motion_19_joint_0_angles[56] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_19_joint_0_angles.inc"
};
static NJS_MKEY_F biolizard_motion_19_joint_1_position[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_19_joint_1_position.inc"
};
static NJS_MKEY_A biolizard_motion_19_joint_3_angles[121] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_19_joint_3_angles.inc"
};
static NJS_MKEY_A biolizard_motion_19_joint_4_angles[131] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_19_joint_4_angles.inc"
};
static NJS_MKEY_A biolizard_motion_19_joint_5_angles[137] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_19_joint_5_angles.inc"
};
static NJS_MKEY_F biolizard_motion_19_joint_6_position[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_19_joint_6_position.inc"
};
static NJS_MKEY_A biolizard_motion_19_joint_6_angles[140] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_19_joint_6_angles.inc"
};
static NJS_MKEY_F biolizard_motion_19_joint_7_position[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_19_joint_7_position.inc"
};
static NJS_MKEY_A biolizard_motion_19_joint_7_angles[147] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_19_joint_7_angles.inc"
};
static NJS_MKEY_F biolizard_motion_19_joint_8_position[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_19_joint_8_position.inc"
};
static NJS_MKEY_A biolizard_motion_19_joint_8_angles[147] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_19_joint_8_angles.inc"
};
static NJS_MKEY_F biolizard_motion_19_joint_9_position[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_19_joint_9_position.inc"
};
static NJS_MKEY_A biolizard_motion_19_joint_9_angles[159] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_19_joint_9_angles.inc"
};
static NJS_MKEY_F biolizard_motion_19_joint_10_position[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_19_joint_10_position.inc"
};
static NJS_MKEY_A biolizard_motion_19_joint_12_angles[136] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_19_joint_12_angles.inc"
};
static NJS_MKEY_A biolizard_motion_19_joint_13_angles[140] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_19_joint_13_angles.inc"
};
static NJS_MKEY_A biolizard_motion_19_joint_16_angles[117] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_19_joint_16_angles.inc"
};
static NJS_MKEY_A biolizard_motion_19_joint_17_angles[140] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_19_joint_17_angles.inc"
};
static NJS_MKEY_A biolizard_motion_19_joint_20_angles[146] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_19_joint_20_angles.inc"
};
static NJS_MKEY_A biolizard_motion_19_joint_21_angles[126] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_19_joint_21_angles.inc"
};
static NJS_MKEY_A biolizard_motion_19_joint_22_angles[146] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_19_joint_22_angles.inc"
};
static NJS_MKEY_A biolizard_motion_19_joint_23_angles[149] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_19_joint_23_angles.inc"
};
static NJS_MKEY_A biolizard_motion_19_joint_24_angles[159] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_19_joint_24_angles.inc"
};
static NJS_MKEY_A biolizard_motion_19_joint_27_angles[159] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_19_joint_27_angles.inc"
};
static NJS_MKEY_A biolizard_motion_19_joint_28_angles[156] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_19_joint_28_angles.inc"
};
static NJS_MKEY_A biolizard_motion_19_joint_31_angles[160] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_19_joint_31_angles.inc"
};
static NJS_MKEY_A biolizard_motion_19_joint_34_angles[160] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_19_joint_34_angles.inc"
};
static NJS_MKEY_F biolizard_motion_19_joint_36_position[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_19_joint_36_position.inc"
};
static NJS_MKEY_F biolizard_motion_19_joint_38_position[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_19_joint_38_position.inc"
};
static NJS_MKEY_F biolizard_motion_19_joint_39_position[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_19_joint_39_position.inc"
};
static NJS_MKEY_F biolizard_motion_19_joint_41_position[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_19_joint_41_position.inc"
};
static NJS_MKEY_F biolizard_motion_19_joint_42_position[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_19_joint_42_position.inc"
};
static NJS_MKEY_F biolizard_motion_19_joint_43_position[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_19_joint_43_position.inc"
};
static NJS_MKEY_A biolizard_motion_19_joint_43_angles[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_19_joint_43_angles.inc"
};
static NJS_MDATA2 biolizard_motion_19_tracks[44] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_19_tracks.inc"
};
NJS_MOTION biolizard_motion_19 = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_19.inc"
};
static NJS_MKEY_F biolizard_motion_1_joint_0_position[32] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_1_joint_0_position.inc"
};
static NJS_MKEY_A biolizard_motion_1_joint_3_angles[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_1_joint_3_angles.inc"
};
static NJS_MKEY_A biolizard_motion_1_joint_4_angles[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_1_joint_4_angles.inc"
};
static NJS_MKEY_A biolizard_motion_1_joint_5_angles[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_1_joint_5_angles.inc"
};
static NJS_MKEY_A biolizard_motion_1_joint_6_angles[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_1_joint_6_angles.inc"
};
static NJS_MKEY_A biolizard_motion_1_joint_7_angles[32] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_1_joint_7_angles.inc"
};
static NJS_MKEY_A biolizard_motion_1_joint_8_angles[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_1_joint_8_angles.inc"
};
static NJS_MKEY_A biolizard_motion_1_joint_9_angles[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_1_joint_9_angles.inc"
};
static NJS_MKEY_A biolizard_motion_1_joint_12_angles[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_1_joint_12_angles.inc"
};
static NJS_MKEY_A biolizard_motion_1_joint_13_angles[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_1_joint_13_angles.inc"
};
static NJS_MKEY_A biolizard_motion_1_joint_16_angles[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_1_joint_16_angles.inc"
};
static NJS_MKEY_A biolizard_motion_1_joint_17_angles[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_1_joint_17_angles.inc"
};
static NJS_MKEY_A biolizard_motion_1_joint_20_angles[32] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_1_joint_20_angles.inc"
};
static NJS_MKEY_A biolizard_motion_1_joint_21_angles[32] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_1_joint_21_angles.inc"
};
static NJS_MKEY_A biolizard_motion_1_joint_22_angles[32] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_1_joint_22_angles.inc"
};
static NJS_MKEY_A biolizard_motion_1_joint_23_angles[32] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_1_joint_23_angles.inc"
};
static NJS_MKEY_A biolizard_motion_1_joint_24_angles[32] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_1_joint_24_angles.inc"
};
static NJS_MKEY_A biolizard_motion_1_joint_27_angles[32] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_1_joint_27_angles.inc"
};
static NJS_MKEY_A biolizard_motion_1_joint_28_angles[32] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_1_joint_28_angles.inc"
};
static NJS_MKEY_A biolizard_motion_1_joint_31_angles[32] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_1_joint_31_angles.inc"
};
static NJS_MKEY_A biolizard_motion_1_joint_34_angles[32] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_1_joint_34_angles.inc"
};
static NJS_MDATA2 biolizard_motion_1_tracks[44] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_1_tracks.inc"
};
NJS_MOTION biolizard_motion_1 = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_1.inc"
};
static NJS_MKEY_F biolizard_motion_1_partner_joint_0_position[32] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_1_partner_joint_0_position.inc"
};
static NJS_MKEY_A biolizard_motion_1_partner_joint_3_angles[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_1_partner_joint_3_angles.inc"
};
static NJS_MKEY_A biolizard_motion_1_partner_joint_4_angles[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_1_partner_joint_4_angles.inc"
};
static NJS_MKEY_A biolizard_motion_1_partner_joint_5_angles[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_1_partner_joint_5_angles.inc"
};
static NJS_MKEY_A biolizard_motion_1_partner_joint_6_angles[32] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_1_partner_joint_6_angles.inc"
};
static NJS_MKEY_A biolizard_motion_1_partner_joint_7_angles[32] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_1_partner_joint_7_angles.inc"
};
static NJS_MKEY_A biolizard_motion_1_partner_joint_8_angles[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_1_partner_joint_8_angles.inc"
};
static NJS_MKEY_A biolizard_motion_1_partner_joint_9_angles[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_1_partner_joint_9_angles.inc"
};
static NJS_MKEY_A biolizard_motion_1_partner_joint_12_angles[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_1_partner_joint_12_angles.inc"
};
static NJS_MKEY_A biolizard_motion_1_partner_joint_13_angles[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_1_partner_joint_13_angles.inc"
};
static NJS_MKEY_A biolizard_motion_1_partner_joint_16_angles[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_1_partner_joint_16_angles.inc"
};
static NJS_MKEY_A biolizard_motion_1_partner_joint_17_angles[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_1_partner_joint_17_angles.inc"
};
static NJS_MKEY_A biolizard_motion_1_partner_joint_20_angles[32] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_1_partner_joint_20_angles.inc"
};
static NJS_MKEY_A biolizard_motion_1_partner_joint_21_angles[32] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_1_partner_joint_21_angles.inc"
};
static NJS_MKEY_A biolizard_motion_1_partner_joint_22_angles[32] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_1_partner_joint_22_angles.inc"
};
static NJS_MKEY_A biolizard_motion_1_partner_joint_23_angles[32] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_1_partner_joint_23_angles.inc"
};
static NJS_MKEY_A biolizard_motion_1_partner_joint_24_angles[32] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_1_partner_joint_24_angles.inc"
};
static NJS_MKEY_A biolizard_motion_1_partner_joint_27_angles[32] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_1_partner_joint_27_angles.inc"
};
static NJS_MKEY_A biolizard_motion_1_partner_joint_28_angles[32] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_1_partner_joint_28_angles.inc"
};
static NJS_MKEY_A biolizard_motion_1_partner_joint_31_angles[32] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_1_partner_joint_31_angles.inc"
};
static NJS_MKEY_A biolizard_motion_1_partner_joint_34_angles[32] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_1_partner_joint_34_angles.inc"
};
static NJS_MDATA2 biolizard_motion_1_partner_tracks[44] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_1_partner_tracks.inc"
};
NJS_MOTION biolizard_motion_1_partner = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_1_partner.inc"
};
static NJS_MKEY_F biolizard_motion_2_joint_0_position[32] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_2_joint_0_position.inc"
};
static NJS_MKEY_A biolizard_motion_2_joint_3_angles[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_2_joint_3_angles.inc"
};
static NJS_MKEY_A biolizard_motion_2_joint_4_angles[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_2_joint_4_angles.inc"
};
static NJS_MKEY_A biolizard_motion_2_joint_5_angles[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_2_joint_5_angles.inc"
};
static NJS_MKEY_A biolizard_motion_2_joint_6_angles[15] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_2_joint_6_angles.inc"
};
static NJS_MKEY_A biolizard_motion_2_joint_7_angles[32] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_2_joint_7_angles.inc"
};
static NJS_MKEY_A biolizard_motion_2_joint_8_angles[32] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_2_joint_8_angles.inc"
};
static NJS_MKEY_A biolizard_motion_2_joint_9_angles[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_2_joint_9_angles.inc"
};
static NJS_MKEY_A biolizard_motion_2_joint_12_angles[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_2_joint_12_angles.inc"
};
static NJS_MKEY_A biolizard_motion_2_joint_13_angles[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_2_joint_13_angles.inc"
};
static NJS_MKEY_A biolizard_motion_2_joint_16_angles[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_2_joint_16_angles.inc"
};
static NJS_MKEY_A biolizard_motion_2_joint_17_angles[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_2_joint_17_angles.inc"
};
static NJS_MKEY_A biolizard_motion_2_joint_20_angles[32] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_2_joint_20_angles.inc"
};
static NJS_MKEY_A biolizard_motion_2_joint_21_angles[32] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_2_joint_21_angles.inc"
};
static NJS_MKEY_A biolizard_motion_2_joint_22_angles[32] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_2_joint_22_angles.inc"
};
static NJS_MKEY_A biolizard_motion_2_joint_23_angles[32] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_2_joint_23_angles.inc"
};
static NJS_MKEY_A biolizard_motion_2_joint_24_angles[32] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_2_joint_24_angles.inc"
};
static NJS_MKEY_A biolizard_motion_2_joint_27_angles[32] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_2_joint_27_angles.inc"
};
static NJS_MKEY_A biolizard_motion_2_joint_28_angles[32] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_2_joint_28_angles.inc"
};
static NJS_MKEY_A biolizard_motion_2_joint_31_angles[32] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_2_joint_31_angles.inc"
};
static NJS_MKEY_A biolizard_motion_2_joint_34_angles[32] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_2_joint_34_angles.inc"
};
static NJS_MDATA2 biolizard_motion_2_tracks[44] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_2_tracks.inc"
};
NJS_MOTION biolizard_motion_2 = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_2.inc"
};
static NJS_MKEY_F biolizard_motion_2_partner_joint_0_position[32] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_2_partner_joint_0_position.inc"
};
static NJS_MKEY_A biolizard_motion_2_partner_joint_3_angles[3] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_2_partner_joint_3_angles.inc"
};
static NJS_MKEY_A biolizard_motion_2_partner_joint_4_angles[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_2_partner_joint_4_angles.inc"
};
static NJS_MKEY_A biolizard_motion_2_partner_joint_5_angles[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_2_partner_joint_5_angles.inc"
};
static NJS_MKEY_A biolizard_motion_2_partner_joint_6_angles[31] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_2_partner_joint_6_angles.inc"
};
static NJS_MKEY_A biolizard_motion_2_partner_joint_7_angles[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_2_partner_joint_7_angles.inc"
};
static NJS_MKEY_A biolizard_motion_2_partner_joint_8_angles[31] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_2_partner_joint_8_angles.inc"
};
static NJS_MKEY_A biolizard_motion_2_partner_joint_9_angles[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_2_partner_joint_9_angles.inc"
};
static NJS_MKEY_A biolizard_motion_2_partner_joint_12_angles[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_2_partner_joint_12_angles.inc"
};
static NJS_MKEY_A biolizard_motion_2_partner_joint_13_angles[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_2_partner_joint_13_angles.inc"
};
static NJS_MKEY_A biolizard_motion_2_partner_joint_16_angles[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_2_partner_joint_16_angles.inc"
};
static NJS_MKEY_A biolizard_motion_2_partner_joint_17_angles[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_2_partner_joint_17_angles.inc"
};
static NJS_MKEY_A biolizard_motion_2_partner_joint_20_angles[32] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_2_partner_joint_20_angles.inc"
};
static NJS_MKEY_A biolizard_motion_2_partner_joint_21_angles[32] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_2_partner_joint_21_angles.inc"
};
static NJS_MKEY_A biolizard_motion_2_partner_joint_22_angles[32] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_2_partner_joint_22_angles.inc"
};
static NJS_MKEY_A biolizard_motion_2_partner_joint_23_angles[32] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_2_partner_joint_23_angles.inc"
};
static NJS_MKEY_A biolizard_motion_2_partner_joint_24_angles[32] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_2_partner_joint_24_angles.inc"
};
static NJS_MKEY_A biolizard_motion_2_partner_joint_27_angles[32] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_2_partner_joint_27_angles.inc"
};
static NJS_MKEY_A biolizard_motion_2_partner_joint_28_angles[32] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_2_partner_joint_28_angles.inc"
};
static NJS_MKEY_A biolizard_motion_2_partner_joint_31_angles[32] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_2_partner_joint_31_angles.inc"
};
static NJS_MKEY_A biolizard_motion_2_partner_joint_34_angles[32] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_2_partner_joint_34_angles.inc"
};
static NJS_MDATA2 biolizard_motion_2_partner_tracks[44] = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_2_partner_tracks.inc"
};
NJS_MOTION biolizard_motion_2_partner = {
#include "boss_last1/biolizard_model_textures/biolizard_motion_2_partner.inc"
};
static NJS_MKEY_F biolizard_intro_camera_motion_joint_0_position[270] = {
#include "boss_last1/biolizard_model_textures/biolizard_intro_camera_motion_joint_0_position.inc"
};
static NJS_MKEY_F biolizard_intro_camera_motion_joint_0_target[241] = {
#include "boss_last1/biolizard_model_textures/biolizard_intro_camera_motion_joint_0_target.inc"
};
static NJS_MKEY_A1 biolizard_intro_camera_motion_joint_0_roll[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_intro_camera_motion_joint_0_roll.inc"
};
static NJS_MKEY_A1 biolizard_intro_camera_motion_joint_0_fov[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_intro_camera_motion_joint_0_fov.inc"
};
static NJS_MDATA4 biolizard_intro_camera_motion_tracks[1] = {
#include "boss_last1/biolizard_model_textures/biolizard_intro_camera_motion_tracks.inc"
};
NJS_MOTION biolizard_intro_camera_motion = {
#include "boss_last1/biolizard_model_textures/biolizard_intro_camera_motion.inc"
};
static NJS_MKEY_F biolizard_defeat_camera_motion_joint_0_position[160] = {
#include "boss_last1/biolizard_model_textures/biolizard_defeat_camera_motion_joint_0_position.inc"
};
static NJS_MKEY_F biolizard_defeat_camera_motion_joint_0_target[141] = {
#include "boss_last1/biolizard_model_textures/biolizard_defeat_camera_motion_joint_0_target.inc"
};
static NJS_MKEY_A1 biolizard_defeat_camera_motion_joint_0_roll[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_defeat_camera_motion_joint_0_roll.inc"
};
static NJS_MKEY_A1 biolizard_defeat_camera_motion_joint_0_fov[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_defeat_camera_motion_joint_0_fov.inc"
};
static NJS_MDATA4 biolizard_defeat_camera_motion_tracks[1] = {
#include "boss_last1/biolizard_model_textures/biolizard_defeat_camera_motion_tracks.inc"
};
NJS_MOTION biolizard_defeat_camera_motion = {
#include "boss_last1/biolizard_model_textures/biolizard_defeat_camera_motion.inc"
};
static NJS_MKEY_F biolizard_charge_camera_motion_joint_0_position[71] = {
#include "boss_last1/biolizard_model_textures/biolizard_charge_camera_motion_joint_0_position.inc"
};
static NJS_MKEY_F biolizard_charge_camera_motion_joint_0_target[71] = {
#include "boss_last1/biolizard_model_textures/biolizard_charge_camera_motion_joint_0_target.inc"
};
static NJS_MKEY_A1 biolizard_charge_camera_motion_joint_0_roll[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_charge_camera_motion_joint_0_roll.inc"
};
static NJS_MKEY_A1 biolizard_charge_camera_motion_joint_0_fov[2] = {
#include "boss_last1/biolizard_model_textures/biolizard_charge_camera_motion_joint_0_fov.inc"
};
static NJS_MDATA4 biolizard_charge_camera_motion_tracks[1] = {
#include "boss_last1/biolizard_model_textures/biolizard_charge_camera_motion_tracks.inc"
};
NJS_MOTION biolizard_charge_camera_motion = {
#include "boss_last1/biolizard_model_textures/biolizard_charge_camera_motion.inc"
};
static NJS_CACTION charge_camera_action = {
#include "boss_last1/biolizard_model_textures/charge_camera_action.inc"
};
static NJS_TEXNAME water_textures[16] = {
#include "boss_last1/biolizard_model_textures/water_textures.inc"
};
NJS_TEXLIST biolizard_water_texlist = {
#include "boss_last1/biolizard_model_textures/biolizard_water_texlist.inc"
};
