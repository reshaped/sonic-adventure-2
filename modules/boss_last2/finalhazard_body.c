/* BOSS_LAST2.PRS's chunk models, texture lists, motions and texture names,
 * from the disc. its C lies between effect_textures.c and panels.c, and its
 * D lies between body.c and panels.c */
#include <ninja.h>
#include <sg_xpt.h>

#include "boss_last2/arena.h"
#include "boss_last2/attack_art.h"
#include "boss_last2/body_art.h"
#include "boss_last2/motion_art.h"
#include "boss_last2/texture_names.h"

#include "boss_last2/finalhazard_body/text.inc"

static Sint16 core_shell_polygons[4293] = {
#include "boss_last2/finalhazard_body/core_shell_polygons.inc"
};
static NJS_CNK_MODEL core_shell_mesh = {
#include "boss_last2/finalhazard_body/core_shell_mesh.inc"
};
static NJS_CNK_OBJECT core_shell = {
#include "boss_last2/finalhazard_body/core_shell.inc"
};
NJS_CNK_OBJECT finalhazard_node_44 = {
#include "boss_last2/finalhazard_body/finalhazard_node_44.inc"
};
static Sint16 lower_support_polygons[347] = {
#include "boss_last2/finalhazard_body/lower_support_polygons.inc"
};
static Sint32 lower_support_vertices[249] = {
#include "boss_last2/finalhazard_body/lower_support_vertices.inc"
};
static NJS_CNK_MODEL lower_support_mesh = {
#include "boss_last2/finalhazard_body/lower_support_mesh.inc"
};
static NJS_CNK_OBJECT lower_support = {
#include "boss_last2/finalhazard_body/lower_support.inc"
};
NJS_CNK_OBJECT finalhazard_node_42 = {
#include "boss_last2/finalhazard_body/finalhazard_node_42.inc"
};
static Sint16 upper_support_polygons[391] = {
#include "boss_last2/finalhazard_body/upper_support_polygons.inc"
};
static Sint32 upper_support_vertices[357] = {
#include "boss_last2/finalhazard_body/upper_support_vertices.inc"
};
static NJS_CNK_MODEL upper_support_mesh = {
#include "boss_last2/finalhazard_body/upper_support_mesh.inc"
};
NJS_CNK_OBJECT finalhazard_node_41 = {
#include "boss_last2/finalhazard_body/finalhazard_node_41.inc"
};
static Sint16 support_shell_polygons[249] = {
#include "boss_last2/finalhazard_body/support_shell_polygons.inc"
};
static Sint32 support_shell_vertices[237] = {
#include "boss_last2/finalhazard_body/support_shell_vertices.inc"
};
static NJS_CNK_MODEL support_shell_mesh = {
#include "boss_last2/finalhazard_body/support_shell_mesh.inc"
};
static NJS_CNK_OBJECT support_shell = {
#include "boss_last2/finalhazard_body/support_shell.inc"
};
static Sint16 lower_shell_polygons[177] = {
#include "boss_last2/finalhazard_body/lower_shell_polygons.inc"
};
static Sint32 lower_shell_vertices[165] = {
#include "boss_last2/finalhazard_body/lower_shell_vertices.inc"
};
static NJS_CNK_MODEL lower_shell_mesh = {
#include "boss_last2/finalhazard_body/lower_shell_mesh.inc"
};
static NJS_CNK_OBJECT lower_shell = {
#include "boss_last2/finalhazard_body/lower_shell.inc"
};
static Sint16 side_shell_polygons[87] = {
#include "boss_last2/finalhazard_body/side_shell_polygons.inc"
};
static Sint32 side_shell_vertices[75] = {
#include "boss_last2/finalhazard_body/side_shell_vertices.inc"
};
static NJS_CNK_MODEL side_shell_mesh = {
#include "boss_last2/finalhazard_body/side_shell_mesh.inc"
};
static NJS_CNK_OBJECT side_shell = {
#include "boss_last2/finalhazard_body/side_shell.inc"
};
static Sint16 neck_shell_polygons[2639] = {
#include "boss_last2/finalhazard_body/neck_shell_polygons.inc"
};
static NJS_CNK_MODEL neck_shell_mesh = {
#include "boss_last2/finalhazard_body/neck_shell_mesh.inc"
};
static NJS_CNK_OBJECT neck_shell = {
#include "boss_last2/finalhazard_body/neck_shell.inc"
};
static Sint16 rear_shell_polygons[191] = {
#include "boss_last2/finalhazard_body/rear_shell_polygons.inc"
};
static NJS_CNK_MODEL rear_shell_mesh = {
#include "boss_last2/finalhazard_body/rear_shell_mesh.inc"
};
static NJS_CNK_OBJECT rear_shell = {
#include "boss_last2/finalhazard_body/rear_shell.inc"
};
static Sint16 back_shell_polygons[321] = {
#include "boss_last2/finalhazard_body/back_shell_polygons.inc"
};
static NJS_CNK_MODEL back_shell_mesh = {
#include "boss_last2/finalhazard_body/back_shell_mesh.inc"
};
static NJS_CNK_OBJECT back_shell = {
#include "boss_last2/finalhazard_body/back_shell.inc"
};
static NJS_CNK_OBJECT upper_limb_tip = {
#include "boss_last2/finalhazard_body/upper_limb_tip.inc"
};
static Sint16 upper_limb_polygons[2] = {
#include "boss_last2/finalhazard_body/upper_limb_polygons.inc"
};
static Sint32 upper_limb_vertices[143] = {
#include "boss_last2/finalhazard_body/upper_limb_vertices.inc"
};
static NJS_CNK_MODEL upper_limb_mesh = {
#include "boss_last2/finalhazard_body/upper_limb_mesh.inc"
};
NJS_CNK_OBJECT finalhazard_node_33 = {
#include "boss_last2/finalhazard_body/finalhazard_node_33.inc"
};
static NJS_CNK_OBJECT upper_limb_mount = {
#include "boss_last2/finalhazard_body/upper_limb_mount.inc"
};
static NJS_CNK_OBJECT tail_tip = {
#include "boss_last2/finalhazard_body/tail_tip.inc"
};
static Sint32 tail_end_vertices[554] = {
#include "boss_last2/finalhazard_body/tail_end_vertices.inc"
};
static NJS_CNK_MODEL tail_end_mesh = {
#include "boss_last2/finalhazard_body/tail_end_mesh.inc"
};
NJS_CNK_OBJECT finalhazard_node_30 = {
#include "boss_last2/finalhazard_body/finalhazard_node_30.inc"
};
static Sint32 tail_middle_vertices[209] = {
#include "boss_last2/finalhazard_body/tail_middle_vertices.inc"
};
static NJS_CNK_MODEL tail_middle_mesh = {
#include "boss_last2/finalhazard_body/tail_middle_mesh.inc"
};
NJS_CNK_OBJECT finalhazard_node_29 = {
#include "boss_last2/finalhazard_body/finalhazard_node_29.inc"
};
static Sint32 tail_base_vertices[131] = {
#include "boss_last2/finalhazard_body/tail_base_vertices.inc"
};
static NJS_CNK_MODEL tail_base_mesh = {
#include "boss_last2/finalhazard_body/tail_base_mesh.inc"
};
NJS_CNK_OBJECT finalhazard_node_28 = {
#include "boss_last2/finalhazard_body/finalhazard_node_28.inc"
};
static NJS_CNK_OBJECT tail_mount = {
#include "boss_last2/finalhazard_body/tail_mount.inc"
};
static NJS_CNK_OBJECT lower_limb_tip = {
#include "boss_last2/finalhazard_body/lower_limb_tip.inc"
};
static Sint32 lower_limb_vertices[81] = {
#include "boss_last2/finalhazard_body/lower_limb_vertices.inc"
};
static NJS_CNK_MODEL lower_limb_mesh = {
#include "boss_last2/finalhazard_body/lower_limb_mesh.inc"
};
NJS_CNK_OBJECT finalhazard_node_25 = {
#include "boss_last2/finalhazard_body/finalhazard_node_25.inc"
};
static NJS_CNK_OBJECT lower_limb_mount = {
#include "boss_last2/finalhazard_body/lower_limb_mount.inc"
};
static NJS_CNK_OBJECT front_foot_tip = {
#include "boss_last2/finalhazard_body/front_foot_tip.inc"
};
static Sint32 front_foot_vertices[59] = {
#include "boss_last2/finalhazard_body/front_foot_vertices.inc"
};
static NJS_CNK_MODEL front_foot_mesh = {
#include "boss_last2/finalhazard_body/front_foot_mesh.inc"
};
NJS_CNK_OBJECT finalhazard_node_22 = {
#include "boss_last2/finalhazard_body/finalhazard_node_22.inc"
};
static Sint32 front_leg_vertices[82] = {
#include "boss_last2/finalhazard_body/front_leg_vertices.inc"
};
static NJS_CNK_MODEL front_leg_mesh = {
#include "boss_last2/finalhazard_body/front_leg_mesh.inc"
};
NJS_CNK_OBJECT finalhazard_node_21 = {
#include "boss_last2/finalhazard_body/finalhazard_node_21.inc"
};
static NJS_CNK_OBJECT front_leg_mount = {
#include "boss_last2/finalhazard_body/front_leg_mount.inc"
};
static NJS_CNK_OBJECT side_limb_tip = {
#include "boss_last2/finalhazard_body/side_limb_tip.inc"
};
static Sint32 side_limb_vertices[81] = {
#include "boss_last2/finalhazard_body/side_limb_vertices.inc"
};
static NJS_CNK_MODEL side_limb_mesh = {
#include "boss_last2/finalhazard_body/side_limb_mesh.inc"
};
NJS_CNK_OBJECT finalhazard_node_18 = {
#include "boss_last2/finalhazard_body/finalhazard_node_18.inc"
};
static NJS_CNK_OBJECT side_limb_mount = {
#include "boss_last2/finalhazard_body/side_limb_mount.inc"
};
static NJS_CNK_OBJECT rear_foot_tip = {
#include "boss_last2/finalhazard_body/rear_foot_tip.inc"
};
static Sint32 rear_foot_vertices[165] = {
#include "boss_last2/finalhazard_body/rear_foot_vertices.inc"
};
static NJS_CNK_MODEL rear_foot_mesh = {
#include "boss_last2/finalhazard_body/rear_foot_mesh.inc"
};
NJS_CNK_OBJECT finalhazard_node_15 = {
#include "boss_last2/finalhazard_body/finalhazard_node_15.inc"
};
static Sint32 rear_leg_vertices[157] = {
#include "boss_last2/finalhazard_body/rear_leg_vertices.inc"
};
static NJS_CNK_MODEL rear_leg_mesh = {
#include "boss_last2/finalhazard_body/rear_leg_mesh.inc"
};
NJS_CNK_OBJECT finalhazard_node_14 = {
#include "boss_last2/finalhazard_body/finalhazard_node_14.inc"
};
static NJS_CNK_OBJECT rear_leg_mount = {
#include "boss_last2/finalhazard_body/rear_leg_mount.inc"
};
static NJS_CNK_OBJECT neck_tip = {
#include "boss_last2/finalhazard_body/neck_tip.inc"
};
static NJS_CNK_OBJECT jaw_tip = {
#include "boss_last2/finalhazard_body/jaw_tip.inc"
};
static Sint32 jaw_vertices[1038] = {
#include "boss_last2/finalhazard_body/jaw_vertices.inc"
};
static NJS_CNK_MODEL jaw_mesh = {
#include "boss_last2/finalhazard_body/jaw_mesh.inc"
};
NJS_CNK_OBJECT finalhazard_node_10 = {
#include "boss_last2/finalhazard_body/finalhazard_node_10.inc"
};
static NJS_CNK_OBJECT jaw_mount = {
#include "boss_last2/finalhazard_body/jaw_mount.inc"
};
static Sint32 head_vertices[2792] = {
#include "boss_last2/finalhazard_body/head_vertices.inc"
};
static NJS_CNK_MODEL head_mesh = {
#include "boss_last2/finalhazard_body/head_mesh.inc"
};
NJS_CNK_OBJECT finalhazard_node_8 = {
#include "boss_last2/finalhazard_body/finalhazard_node_8.inc"
};
NJS_CNK_OBJECT finalhazard_node_7 = {
#include "boss_last2/finalhazard_body/finalhazard_node_7.inc"
};
static Sint32 neck_fourth_vertices[343] = {
#include "boss_last2/finalhazard_body/neck_fourth_vertices.inc"
};
static NJS_CNK_MODEL neck_fourth_mesh = {
#include "boss_last2/finalhazard_body/neck_fourth_mesh.inc"
};
NJS_CNK_OBJECT finalhazard_node_6 = {
#include "boss_last2/finalhazard_body/finalhazard_node_6.inc"
};
static Sint32 neck_third_vertices[222] = {
#include "boss_last2/finalhazard_body/neck_third_vertices.inc"
};
static NJS_CNK_MODEL neck_third_mesh = {
#include "boss_last2/finalhazard_body/neck_third_mesh.inc"
};
NJS_CNK_OBJECT finalhazard_node_5 = {
#include "boss_last2/finalhazard_body/finalhazard_node_5.inc"
};
static Sint32 neck_second_vertices[1617] = {
#include "boss_last2/finalhazard_body/neck_second_vertices.inc"
};
static NJS_CNK_MODEL neck_second_mesh = {
#include "boss_last2/finalhazard_body/neck_second_mesh.inc"
};
NJS_CNK_OBJECT finalhazard_node_4 = {
#include "boss_last2/finalhazard_body/finalhazard_node_4.inc"
};
static Sint32 neck_base_vertices[493] = {
#include "boss_last2/finalhazard_body/neck_base_vertices.inc"
};
static NJS_CNK_MODEL neck_base_mesh = {
#include "boss_last2/finalhazard_body/neck_base_mesh.inc"
};
NJS_CNK_OBJECT finalhazard_node_3 = {
#include "boss_last2/finalhazard_body/finalhazard_node_3.inc"
};
static NJS_CNK_OBJECT neck_mount = {
#include "boss_last2/finalhazard_body/neck_mount.inc"
};
static Sint32 torso_vertices[3661] = {
#include "boss_last2/finalhazard_body/torso_vertices.inc"
};
static NJS_CNK_MODEL torso_mesh = {
#include "boss_last2/finalhazard_body/torso_mesh.inc"
};
static NJS_CNK_OBJECT torso = {
#include "boss_last2/finalhazard_body/torso.inc"
};
static Sint16 root_polygons[5409] = {
#include "boss_last2/finalhazard_body/root_polygons.inc"
};
static NJS_CNK_MODEL root_mesh = {
#include "boss_last2/finalhazard_body/root_mesh.inc"
};
NJS_CNK_OBJECT finalhazard_root = {
#include "boss_last2/finalhazard_body/finalhazard_root.inc"
};
static NJS_TEXNAME names[16] = {
#include "boss_last2/finalhazard_body/names.inc"
};
NJS_TEXLIST finalhazard_model_texlist = {
#include "boss_last2/finalhazard_body/finalhazard_model_texlist.inc"
};
static NJS_MKEY_A finalhazard_idle_motion_joint_4_angles[64] = {
#include "boss_last2/finalhazard_body/finalhazard_idle_motion_joint_4_angles.inc"
};
static NJS_MKEY_A finalhazard_idle_motion_joint_5_angles[63] = {
#include "boss_last2/finalhazard_body/finalhazard_idle_motion_joint_5_angles.inc"
};
static NJS_MKEY_A finalhazard_idle_motion_joint_6_angles[62] = {
#include "boss_last2/finalhazard_body/finalhazard_idle_motion_joint_6_angles.inc"
};
static NJS_MKEY_A finalhazard_idle_motion_joint_7_angles[64] = {
#include "boss_last2/finalhazard_body/finalhazard_idle_motion_joint_7_angles.inc"
};
static NJS_MKEY_A finalhazard_idle_motion_joint_8_angles[64] = {
#include "boss_last2/finalhazard_body/finalhazard_idle_motion_joint_8_angles.inc"
};
static NJS_MKEY_A finalhazard_idle_motion_joint_10_angles[64] = {
#include "boss_last2/finalhazard_body/finalhazard_idle_motion_joint_10_angles.inc"
};
static NJS_MKEY_A finalhazard_idle_motion_joint_14_angles[64] = {
#include "boss_last2/finalhazard_body/finalhazard_idle_motion_joint_14_angles.inc"
};
static NJS_MKEY_A finalhazard_idle_motion_joint_15_angles[64] = {
#include "boss_last2/finalhazard_body/finalhazard_idle_motion_joint_15_angles.inc"
};
static NJS_MKEY_A finalhazard_idle_motion_joint_18_angles[64] = {
#include "boss_last2/finalhazard_body/finalhazard_idle_motion_joint_18_angles.inc"
};
static NJS_MKEY_A finalhazard_idle_motion_joint_21_angles[64] = {
#include "boss_last2/finalhazard_body/finalhazard_idle_motion_joint_21_angles.inc"
};
static NJS_MKEY_A finalhazard_idle_motion_joint_22_angles[64] = {
#include "boss_last2/finalhazard_body/finalhazard_idle_motion_joint_22_angles.inc"
};
static NJS_MKEY_A finalhazard_idle_motion_joint_25_angles[64] = {
#include "boss_last2/finalhazard_body/finalhazard_idle_motion_joint_25_angles.inc"
};
static NJS_MKEY_A finalhazard_idle_motion_joint_28_angles[64] = {
#include "boss_last2/finalhazard_body/finalhazard_idle_motion_joint_28_angles.inc"
};
static NJS_MKEY_A finalhazard_idle_motion_joint_29_angles[64] = {
#include "boss_last2/finalhazard_body/finalhazard_idle_motion_joint_29_angles.inc"
};
static NJS_MKEY_A finalhazard_idle_motion_joint_30_angles[64] = {
#include "boss_last2/finalhazard_body/finalhazard_idle_motion_joint_30_angles.inc"
};
static NJS_MKEY_A finalhazard_idle_motion_joint_33_angles[64] = {
#include "boss_last2/finalhazard_body/finalhazard_idle_motion_joint_33_angles.inc"
};
static NJS_MDATA2 finalhazard_idle_motion_tracks[46] = {
#include "boss_last2/finalhazard_body/finalhazard_idle_motion_tracks.inc"
};
NJS_MOTION finalhazard_idle_motion = {
#include "boss_last2/finalhazard_body/finalhazard_idle_motion.inc"
};
static NJS_MKEY_A finalhazard_defeat_motion_joint_4_angles[130] = {
#include "boss_last2/finalhazard_body/finalhazard_defeat_motion_joint_4_angles.inc"
};
static NJS_MKEY_A finalhazard_defeat_motion_joint_5_angles[129] = {
#include "boss_last2/finalhazard_body/finalhazard_defeat_motion_joint_5_angles.inc"
};
static NJS_MKEY_A finalhazard_defeat_motion_joint_6_angles[156] = {
#include "boss_last2/finalhazard_body/finalhazard_defeat_motion_joint_6_angles.inc"
};
static NJS_MKEY_A finalhazard_defeat_motion_joint_7_angles[141] = {
#include "boss_last2/finalhazard_body/finalhazard_defeat_motion_joint_7_angles.inc"
};
static NJS_MKEY_A finalhazard_defeat_motion_joint_8_angles[160] = {
#include "boss_last2/finalhazard_body/finalhazard_defeat_motion_joint_8_angles.inc"
};
static NJS_MKEY_A finalhazard_defeat_motion_joint_10_angles[159] = {
#include "boss_last2/finalhazard_body/finalhazard_defeat_motion_joint_10_angles.inc"
};
static NJS_MKEY_A finalhazard_defeat_motion_joint_14_angles[160] = {
#include "boss_last2/finalhazard_body/finalhazard_defeat_motion_joint_14_angles.inc"
};
static NJS_MKEY_A finalhazard_defeat_motion_joint_15_angles[159] = {
#include "boss_last2/finalhazard_body/finalhazard_defeat_motion_joint_15_angles.inc"
};
static NJS_MKEY_A finalhazard_defeat_motion_joint_18_angles[159] = {
#include "boss_last2/finalhazard_body/finalhazard_defeat_motion_joint_18_angles.inc"
};
static NJS_MKEY_A finalhazard_defeat_motion_joint_21_angles[157] = {
#include "boss_last2/finalhazard_body/finalhazard_defeat_motion_joint_21_angles.inc"
};
static NJS_MKEY_A finalhazard_defeat_motion_joint_22_angles[160] = {
#include "boss_last2/finalhazard_body/finalhazard_defeat_motion_joint_22_angles.inc"
};
static NJS_MKEY_A finalhazard_defeat_motion_joint_25_angles[160] = {
#include "boss_last2/finalhazard_body/finalhazard_defeat_motion_joint_25_angles.inc"
};
static NJS_MKEY_A finalhazard_defeat_motion_joint_28_angles[154] = {
#include "boss_last2/finalhazard_body/finalhazard_defeat_motion_joint_28_angles.inc"
};
static NJS_MKEY_A finalhazard_defeat_motion_joint_29_angles[154] = {
#include "boss_last2/finalhazard_body/finalhazard_defeat_motion_joint_29_angles.inc"
};
static NJS_MKEY_A finalhazard_defeat_motion_joint_30_angles[158] = {
#include "boss_last2/finalhazard_body/finalhazard_defeat_motion_joint_30_angles.inc"
};
static NJS_MKEY_A finalhazard_defeat_motion_joint_33_angles[160] = {
#include "boss_last2/finalhazard_body/finalhazard_defeat_motion_joint_33_angles.inc"
};
static NJS_MDATA2 finalhazard_defeat_motion_tracks[46] = {
#include "boss_last2/finalhazard_body/finalhazard_defeat_motion_tracks.inc"
};
NJS_MOTION finalhazard_defeat_motion = {
#include "boss_last2/finalhazard_body/finalhazard_defeat_motion.inc"
};
static NJS_MKEY_A finalhazard_intro_motion_joint_4_angles[137] = {
#include "boss_last2/finalhazard_body/finalhazard_intro_motion_joint_4_angles.inc"
};
static NJS_MKEY_A finalhazard_intro_motion_joint_5_angles[139] = {
#include "boss_last2/finalhazard_body/finalhazard_intro_motion_joint_5_angles.inc"
};
static NJS_MKEY_A finalhazard_intro_motion_joint_6_angles[140] = {
#include "boss_last2/finalhazard_body/finalhazard_intro_motion_joint_6_angles.inc"
};
static NJS_MKEY_A finalhazard_intro_motion_joint_7_angles[131] = {
#include "boss_last2/finalhazard_body/finalhazard_intro_motion_joint_7_angles.inc"
};
static NJS_MKEY_A finalhazard_intro_motion_joint_8_angles[140] = {
#include "boss_last2/finalhazard_body/finalhazard_intro_motion_joint_8_angles.inc"
};
static NJS_MKEY_A finalhazard_intro_motion_joint_10_angles[139] = {
#include "boss_last2/finalhazard_body/finalhazard_intro_motion_joint_10_angles.inc"
};
static NJS_MKEY_A finalhazard_intro_motion_joint_14_angles[140] = {
#include "boss_last2/finalhazard_body/finalhazard_intro_motion_joint_14_angles.inc"
};
static NJS_MKEY_A finalhazard_intro_motion_joint_15_angles[140] = {
#include "boss_last2/finalhazard_body/finalhazard_intro_motion_joint_15_angles.inc"
};
static NJS_MKEY_A finalhazard_intro_motion_joint_18_angles[140] = {
#include "boss_last2/finalhazard_body/finalhazard_intro_motion_joint_18_angles.inc"
};
static NJS_MKEY_A finalhazard_intro_motion_joint_21_angles[140] = {
#include "boss_last2/finalhazard_body/finalhazard_intro_motion_joint_21_angles.inc"
};
static NJS_MKEY_A finalhazard_intro_motion_joint_22_angles[140] = {
#include "boss_last2/finalhazard_body/finalhazard_intro_motion_joint_22_angles.inc"
};
static NJS_MKEY_A finalhazard_intro_motion_joint_25_angles[140] = {
#include "boss_last2/finalhazard_body/finalhazard_intro_motion_joint_25_angles.inc"
};
static NJS_MKEY_A finalhazard_intro_motion_joint_28_angles[138] = {
#include "boss_last2/finalhazard_body/finalhazard_intro_motion_joint_28_angles.inc"
};
static NJS_MKEY_A finalhazard_intro_motion_joint_29_angles[138] = {
#include "boss_last2/finalhazard_body/finalhazard_intro_motion_joint_29_angles.inc"
};
static NJS_MKEY_A finalhazard_intro_motion_joint_30_angles[139] = {
#include "boss_last2/finalhazard_body/finalhazard_intro_motion_joint_30_angles.inc"
};
static NJS_MKEY_A finalhazard_intro_motion_joint_33_angles[140] = {
#include "boss_last2/finalhazard_body/finalhazard_intro_motion_joint_33_angles.inc"
};
static NJS_MDATA2 finalhazard_intro_motion_tracks[46] = {
#include "boss_last2/finalhazard_body/finalhazard_intro_motion_tracks.inc"
};
NJS_MOTION finalhazard_intro_motion = {
#include "boss_last2/finalhazard_body/finalhazard_intro_motion.inc"
};
static NJS_MKEY_A finalhazard_hit_motion_joint_4_angles[65] = {
#include "boss_last2/finalhazard_body/finalhazard_hit_motion_joint_4_angles.inc"
};
static NJS_MKEY_A finalhazard_hit_motion_joint_5_angles[65] = {
#include "boss_last2/finalhazard_body/finalhazard_hit_motion_joint_5_angles.inc"
};
static NJS_MKEY_A finalhazard_hit_motion_joint_6_angles[65] = {
#include "boss_last2/finalhazard_body/finalhazard_hit_motion_joint_6_angles.inc"
};
static NJS_MKEY_A finalhazard_hit_motion_joint_7_angles[65] = {
#include "boss_last2/finalhazard_body/finalhazard_hit_motion_joint_7_angles.inc"
};
static NJS_MKEY_A finalhazard_hit_motion_joint_8_angles[65] = {
#include "boss_last2/finalhazard_body/finalhazard_hit_motion_joint_8_angles.inc"
};
static NJS_MKEY_A finalhazard_hit_motion_joint_10_angles[65] = {
#include "boss_last2/finalhazard_body/finalhazard_hit_motion_joint_10_angles.inc"
};
static NJS_MKEY_A finalhazard_hit_motion_joint_14_angles[65] = {
#include "boss_last2/finalhazard_body/finalhazard_hit_motion_joint_14_angles.inc"
};
static NJS_MKEY_A finalhazard_hit_motion_joint_15_angles[65] = {
#include "boss_last2/finalhazard_body/finalhazard_hit_motion_joint_15_angles.inc"
};
static NJS_MKEY_A finalhazard_hit_motion_joint_18_angles[65] = {
#include "boss_last2/finalhazard_body/finalhazard_hit_motion_joint_18_angles.inc"
};
static NJS_MKEY_A finalhazard_hit_motion_joint_21_angles[65] = {
#include "boss_last2/finalhazard_body/finalhazard_hit_motion_joint_21_angles.inc"
};
static NJS_MKEY_A finalhazard_hit_motion_joint_22_angles[65] = {
#include "boss_last2/finalhazard_body/finalhazard_hit_motion_joint_22_angles.inc"
};
static NJS_MKEY_A finalhazard_hit_motion_joint_25_angles[65] = {
#include "boss_last2/finalhazard_body/finalhazard_hit_motion_joint_25_angles.inc"
};
static NJS_MKEY_A finalhazard_hit_motion_joint_28_angles[63] = {
#include "boss_last2/finalhazard_body/finalhazard_hit_motion_joint_28_angles.inc"
};
static NJS_MKEY_A finalhazard_hit_motion_joint_29_angles[65] = {
#include "boss_last2/finalhazard_body/finalhazard_hit_motion_joint_29_angles.inc"
};
static NJS_MKEY_A finalhazard_hit_motion_joint_30_angles[65] = {
#include "boss_last2/finalhazard_body/finalhazard_hit_motion_joint_30_angles.inc"
};
static NJS_MKEY_A finalhazard_hit_motion_joint_33_angles[65] = {
#include "boss_last2/finalhazard_body/finalhazard_hit_motion_joint_33_angles.inc"
};
static NJS_MDATA2 finalhazard_hit_motion_tracks[46] = {
#include "boss_last2/finalhazard_body/finalhazard_hit_motion_tracks.inc"
};
NJS_MOTION finalhazard_hit_motion = {
#include "boss_last2/finalhazard_body/finalhazard_hit_motion.inc"
};
static Sint16 laser_cap_outer_polygons[229] = {
#include "boss_last2/finalhazard_body/laser_cap_outer_polygons.inc"
};
static Sint32 laser_cap_outer_vertices[135] = {
#include "boss_last2/finalhazard_body/laser_cap_outer_vertices.inc"
};
NJS_CNK_MODEL laser_cap_outer = {
#include "boss_last2/finalhazard_body/laser_cap_outer.inc"
};
static NJS_CNK_OBJECT outer_cap = {
#include "boss_last2/finalhazard_body/outer_cap.inc"
};
static Sint16 laser_cap_rotor_polygons[405] = {
#include "boss_last2/finalhazard_body/laser_cap_rotor_polygons.inc"
};
static Sint32 laser_cap_rotor_vertices[231] = {
#include "boss_last2/finalhazard_body/laser_cap_rotor_vertices.inc"
};
NJS_CNK_MODEL laser_cap_rotor = {
#include "boss_last2/finalhazard_body/laser_cap_rotor.inc"
};
static NJS_CNK_OBJECT rotor = {
#include "boss_last2/finalhazard_body/rotor.inc"
};
static Sint16 laser_cap_inner_polygons[257] = {
#include "boss_last2/finalhazard_body/laser_cap_inner_polygons.inc"
};
static Sint32 laser_cap_inner_vertices[135] = {
#include "boss_last2/finalhazard_body/laser_cap_inner_vertices.inc"
};
NJS_CNK_MODEL laser_cap_inner = {
#include "boss_last2/finalhazard_body/laser_cap_inner.inc"
};
static NJS_CNK_OBJECT inner_cap = {
#include "boss_last2/finalhazard_body/inner_cap.inc"
};
static NJS_CNK_OBJECT laser_cap = {
#include "boss_last2/finalhazard_body/laser_cap.inc"
};
static Sint16 finalhazard_ring_model_polygons[543] = {
#include "boss_last2/finalhazard_body/finalhazard_ring_model_polygons.inc"
};
static Sint32 finalhazard_ring_model_vertices[327] = {
#include "boss_last2/finalhazard_body/finalhazard_ring_model_vertices.inc"
};
NJS_CNK_MODEL finalhazard_ring_model = {
#include "boss_last2/finalhazard_body/finalhazard_ring_model.inc"
};
static NJS_CNK_OBJECT reentry_ring = {
#include "boss_last2/finalhazard_body/reentry_ring.inc"
};
static Sint16 sphere_far_core_polygons[45] = {
#include "boss_last2/finalhazard_body/sphere_far_core_polygons.inc"
};
static Sint32 sphere_far_core_vertices[99] = {
#include "boss_last2/finalhazard_body/sphere_far_core_vertices.inc"
};
static NJS_CNK_MODEL sphere_far_core_mesh = {
#include "boss_last2/finalhazard_body/sphere_far_core_mesh.inc"
};
static NJS_CNK_OBJECT far_orb_core = {
#include "boss_last2/finalhazard_body/far_orb_core.inc"
};
static Sint16 sphere_far_shell_polygons[115] = {
#include "boss_last2/finalhazard_body/sphere_far_shell_polygons.inc"
};
static Sint32 sphere_far_shell_vertices[99] = {
#include "boss_last2/finalhazard_body/sphere_far_shell_vertices.inc"
};
static NJS_CNK_MODEL sphere_far_shell_mesh = {
#include "boss_last2/finalhazard_body/sphere_far_shell_mesh.inc"
};
static NJS_CNK_OBJECT far_orb_shell = {
#include "boss_last2/finalhazard_body/far_orb_shell.inc"
};
static NJS_CNK_OBJECT far_orb = {
#include "boss_last2/finalhazard_body/far_orb.inc"
};
static Sint16 sphere_near_core_polygons[161] = {
#include "boss_last2/finalhazard_body/sphere_near_core_polygons.inc"
};
static Sint32 sphere_near_core_vertices[351] = {
#include "boss_last2/finalhazard_body/sphere_near_core_vertices.inc"
};
static NJS_CNK_MODEL sphere_near_core_mesh = {
#include "boss_last2/finalhazard_body/sphere_near_core_mesh.inc"
};
static NJS_CNK_OBJECT near_orb_core = {
#include "boss_last2/finalhazard_body/near_orb_core.inc"
};
static Sint16 sphere_near_shell_polygons[303] = {
#include "boss_last2/finalhazard_body/sphere_near_shell_polygons.inc"
};
static Sint32 sphere_near_shell_vertices[207] = {
#include "boss_last2/finalhazard_body/sphere_near_shell_vertices.inc"
};
static NJS_CNK_MODEL sphere_near_shell_mesh = {
#include "boss_last2/finalhazard_body/sphere_near_shell_mesh.inc"
};
static NJS_CNK_OBJECT near_orb_shell = {
#include "boss_last2/finalhazard_body/near_orb_shell.inc"
};
static NJS_CNK_OBJECT near_orb = {
#include "boss_last2/finalhazard_body/near_orb.inc"
};
static Sint16 sphere_middle_core_polygons[97] = {
#include "boss_last2/finalhazard_body/sphere_middle_core_polygons.inc"
};
static Sint32 sphere_middle_core_vertices[195] = {
#include "boss_last2/finalhazard_body/sphere_middle_core_vertices.inc"
};
static NJS_CNK_MODEL sphere_middle_core_mesh = {
#include "boss_last2/finalhazard_body/sphere_middle_core_mesh.inc"
};
static NJS_CNK_OBJECT middle_orb_core = {
#include "boss_last2/finalhazard_body/middle_orb_core.inc"
};
static Sint16 sphere_middle_shell_polygons[199] = {
#include "boss_last2/finalhazard_body/sphere_middle_shell_polygons.inc"
};
static Sint32 sphere_middle_shell_vertices[159] = {
#include "boss_last2/finalhazard_body/sphere_middle_shell_vertices.inc"
};
static NJS_CNK_MODEL sphere_middle_shell_mesh = {
#include "boss_last2/finalhazard_body/sphere_middle_shell_mesh.inc"
};
static NJS_CNK_OBJECT middle_orb_shell = {
#include "boss_last2/finalhazard_body/middle_orb_shell.inc"
};
static NJS_CNK_OBJECT middle_orb = {
#include "boss_last2/finalhazard_body/middle_orb.inc"
};
static Sint16 sphere_flash_polygons[159] = {
#include "boss_last2/finalhazard_body/sphere_flash_polygons.inc"
};
static Sint32 sphere_flash_vertices[351] = {
#include "boss_last2/finalhazard_body/sphere_flash_vertices.inc"
};
static NJS_CNK_MODEL sphere_flash_mesh = {
#include "boss_last2/finalhazard_body/sphere_flash_mesh.inc"
};
NJS_CNK_OBJECT finalhazard_sphere_flash = {
#include "boss_last2/finalhazard_body/finalhazard_sphere_flash.inc"
};
static NJS_TEXNAME effect_names[8] = {
#include "boss_last2/finalhazard_body/effect_names.inc"
};
NJS_TEXLIST finalhazard_effect_texlist = {
#include "boss_last2/finalhazard_body/finalhazard_effect_texlist.inc"
};
static NJS_MKEY_F rotor_motion_joint_0_position[2] = {
#include "boss_last2/finalhazard_body/rotor_motion_joint_0_position.inc"
};
static NJS_MKEY_A rotor_motion_joint_0_angles[2] = {
#include "boss_last2/finalhazard_body/rotor_motion_joint_0_angles.inc"
};
static NJS_MKEY_F rotor_motion_joint_1_position[2] = {
#include "boss_last2/finalhazard_body/rotor_motion_joint_1_position.inc"
};
static NJS_MKEY_A rotor_motion_joint_1_angles[2] = {
#include "boss_last2/finalhazard_body/rotor_motion_joint_1_angles.inc"
};
static NJS_MKEY_F rotor_motion_joint_2_position[2] = {
#include "boss_last2/finalhazard_body/rotor_motion_joint_2_position.inc"
};
static NJS_MKEY_A rotor_motion_joint_2_angles[9] = {
#include "boss_last2/finalhazard_body/rotor_motion_joint_2_angles.inc"
};
static NJS_MKEY_F rotor_motion_joint_3_position[2] = {
#include "boss_last2/finalhazard_body/rotor_motion_joint_3_position.inc"
};
static NJS_MKEY_A rotor_motion_joint_3_angles[2] = {
#include "boss_last2/finalhazard_body/rotor_motion_joint_3_angles.inc"
};
static NJS_MDATA2 rotor_motion_tracks[4] = {
#include "boss_last2/finalhazard_body/rotor_motion_tracks.inc"
};
static NJS_MOTION rotor_motion = {
#include "boss_last2/finalhazard_body/rotor_motion.inc"
};
static Sint16 laser_link_outer_polygons[67] = {
#include "boss_last2/finalhazard_body/laser_link_outer_polygons.inc"
};
static Sint32 laser_link_outer_vertices[99] = {
#include "boss_last2/finalhazard_body/laser_link_outer_vertices.inc"
};
NJS_CNK_MODEL laser_link_outer = {
#include "boss_last2/finalhazard_body/laser_link_outer.inc"
};
static NJS_CNK_OBJECT outer_link = {
#include "boss_last2/finalhazard_body/outer_link.inc"
};
static Sint16 laser_link_inner_polygons[67] = {
#include "boss_last2/finalhazard_body/laser_link_inner_polygons.inc"
};
static Sint32 laser_link_inner_vertices[99] = {
#include "boss_last2/finalhazard_body/laser_link_inner_vertices.inc"
};
NJS_CNK_MODEL laser_link_inner = {
#include "boss_last2/finalhazard_body/laser_link_inner.inc"
};
static NJS_CNK_OBJECT inner_link = {
#include "boss_last2/finalhazard_body/inner_link.inc"
};
static NJS_CNK_OBJECT laser_link = {
#include "boss_last2/finalhazard_body/laser_link.inc"
};
static NJS_TEXNAME title_names[24] = {
#include "boss_last2/finalhazard_body/title_names.inc"
};
NJS_TEXLIST finalhazard_title_texlist = {
#include "boss_last2/finalhazard_body/finalhazard_title_texlist.inc"
};
static Sint16 panel_polygons[515] = {
#include "boss_last2/finalhazard_body/panel_polygons.inc"
};
static Sint32 panel_vertices[489] = {
#include "boss_last2/finalhazard_body/panel_vertices.inc"
};
static NJS_CNK_MODEL panel_mesh = {
#include "boss_last2/finalhazard_body/panel_mesh.inc"
};
NJS_CNK_OBJECT finalhazard_panel_model = {
#include "boss_last2/finalhazard_body/finalhazard_panel_model.inc"
};
