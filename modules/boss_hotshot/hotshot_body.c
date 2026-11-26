/* BOSS_HOTSHOT.PRS's chunk models and motions, from the disc. its D lies
 * between hotshot.c and object_archives.c */
#include <ninja.h>
#include <sg_xpt.h>

#include "boss_hotshot/model_art.h"
#include "boss_hotshot/motion_art.h"

static NJS_CNK_OBJECT muzzle = {
#include "boss_hotshot/hotshot_body/muzzle.inc"
};
static Sint16 cannon_polygons[609] = {
#include "boss_hotshot/hotshot_body/cannon_polygons.inc"
};
static Sint32 cannon_vertices[381] = {
#include "boss_hotshot/hotshot_body/cannon_vertices.inc"
};
static NJS_CNK_MODEL cannon_mesh = {
#include "boss_hotshot/hotshot_body/cannon_mesh.inc"
};
NJS_CNK_OBJECT hotshot_gun_node = {
#include "boss_hotshot/hotshot_body/hotshot_gun_node.inc"
};
static Sint16 rear_toe_b_polygons[135] = {
#include "boss_hotshot/hotshot_body/rear_toe_b_polygons.inc"
};
static Sint32 rear_toe_b_vertices[75] = {
#include "boss_hotshot/hotshot_body/rear_toe_b_vertices.inc"
};
static NJS_CNK_MODEL rear_toe_b_mesh = {
#include "boss_hotshot/hotshot_body/rear_toe_b_mesh.inc"
};
static NJS_CNK_OBJECT rear_toe_b = {
#include "boss_hotshot/hotshot_body/rear_toe_b.inc"
};
static Sint16 outer_toe_b_polygons[135] = {
#include "boss_hotshot/hotshot_body/outer_toe_b_polygons.inc"
};
static Sint32 outer_toe_b_vertices[75] = {
#include "boss_hotshot/hotshot_body/outer_toe_b_vertices.inc"
};
static NJS_CNK_MODEL outer_toe_b_mesh = {
#include "boss_hotshot/hotshot_body/outer_toe_b_mesh.inc"
};
static NJS_CNK_OBJECT outer_toe_b = {
#include "boss_hotshot/hotshot_body/outer_toe_b.inc"
};
static Sint16 inner_toe_b_polygons[135] = {
#include "boss_hotshot/hotshot_body/inner_toe_b_polygons.inc"
};
static Sint32 inner_toe_b_vertices[75] = {
#include "boss_hotshot/hotshot_body/inner_toe_b_vertices.inc"
};
static NJS_CNK_MODEL inner_toe_b_mesh = {
#include "boss_hotshot/hotshot_body/inner_toe_b_mesh.inc"
};
static NJS_CNK_OBJECT inner_toe_b = {
#include "boss_hotshot/hotshot_body/inner_toe_b.inc"
};
static Sint16 foot_b_polygons[281] = {
#include "boss_hotshot/hotshot_body/foot_b_polygons.inc"
};
static Sint32 foot_b_vertices[195] = {
#include "boss_hotshot/hotshot_body/foot_b_vertices.inc"
};
static NJS_CNK_MODEL foot_b_mesh = {
#include "boss_hotshot/hotshot_body/foot_b_mesh.inc"
};
NJS_CNK_OBJECT hotshot_foot_b_node = {
#include "boss_hotshot/hotshot_body/hotshot_foot_b_node.inc"
};
static Sint16 shin_b_polygons[387] = {
#include "boss_hotshot/hotshot_body/shin_b_polygons.inc"
};
static Sint32 shin_b_vertices[255] = {
#include "boss_hotshot/hotshot_body/shin_b_vertices.inc"
};
static NJS_CNK_MODEL shin_b_mesh = {
#include "boss_hotshot/hotshot_body/shin_b_mesh.inc"
};
static NJS_CNK_OBJECT shin_b = {
#include "boss_hotshot/hotshot_body/shin_b.inc"
};
static Sint16 thigh_b_polygons[181] = {
#include "boss_hotshot/hotshot_body/thigh_b_polygons.inc"
};
static Sint32 thigh_b_vertices[99] = {
#include "boss_hotshot/hotshot_body/thigh_b_vertices.inc"
};
static NJS_CNK_MODEL thigh_b_mesh = {
#include "boss_hotshot/hotshot_body/thigh_b_mesh.inc"
};
static NJS_CNK_OBJECT thigh_b = {
#include "boss_hotshot/hotshot_body/thigh_b.inc"
};
static Sint16 hip_b_polygons[303] = {
#include "boss_hotshot/hotshot_body/hip_b_polygons.inc"
};
static Sint32 hip_b_vertices[219] = {
#include "boss_hotshot/hotshot_body/hip_b_vertices.inc"
};
static NJS_CNK_MODEL hip_b_mesh = {
#include "boss_hotshot/hotshot_body/hip_b_mesh.inc"
};
static NJS_CNK_OBJECT hip_b = {
#include "boss_hotshot/hotshot_body/hip_b.inc"
};
static Sint16 rear_toe_a_polygons[135] = {
#include "boss_hotshot/hotshot_body/rear_toe_a_polygons.inc"
};
static Sint32 rear_toe_a_vertices[75] = {
#include "boss_hotshot/hotshot_body/rear_toe_a_vertices.inc"
};
static NJS_CNK_MODEL rear_toe_a_mesh = {
#include "boss_hotshot/hotshot_body/rear_toe_a_mesh.inc"
};
static NJS_CNK_OBJECT rear_toe_a = {
#include "boss_hotshot/hotshot_body/rear_toe_a.inc"
};
static Sint16 outer_toe_a_polygons[135] = {
#include "boss_hotshot/hotshot_body/outer_toe_a_polygons.inc"
};
static Sint32 outer_toe_a_vertices[75] = {
#include "boss_hotshot/hotshot_body/outer_toe_a_vertices.inc"
};
static NJS_CNK_MODEL outer_toe_a_mesh = {
#include "boss_hotshot/hotshot_body/outer_toe_a_mesh.inc"
};
static NJS_CNK_OBJECT outer_toe_a = {
#include "boss_hotshot/hotshot_body/outer_toe_a.inc"
};
static Sint16 inner_toe_a_polygons[135] = {
#include "boss_hotshot/hotshot_body/inner_toe_a_polygons.inc"
};
static Sint32 inner_toe_a_vertices[75] = {
#include "boss_hotshot/hotshot_body/inner_toe_a_vertices.inc"
};
static NJS_CNK_MODEL inner_toe_a_mesh = {
#include "boss_hotshot/hotshot_body/inner_toe_a_mesh.inc"
};
static NJS_CNK_OBJECT inner_toe_a = {
#include "boss_hotshot/hotshot_body/inner_toe_a.inc"
};
static Sint16 foot_a_polygons[281] = {
#include "boss_hotshot/hotshot_body/foot_a_polygons.inc"
};
static Sint32 foot_a_vertices[195] = {
#include "boss_hotshot/hotshot_body/foot_a_vertices.inc"
};
static NJS_CNK_MODEL foot_a_mesh = {
#include "boss_hotshot/hotshot_body/foot_a_mesh.inc"
};
NJS_CNK_OBJECT hotshot_foot_a_node = {
#include "boss_hotshot/hotshot_body/hotshot_foot_a_node.inc"
};
static Sint16 shin_a_polygons[387] = {
#include "boss_hotshot/hotshot_body/shin_a_polygons.inc"
};
static Sint32 shin_a_vertices[255] = {
#include "boss_hotshot/hotshot_body/shin_a_vertices.inc"
};
static NJS_CNK_MODEL shin_a_mesh = {
#include "boss_hotshot/hotshot_body/shin_a_mesh.inc"
};
static NJS_CNK_OBJECT shin_a = {
#include "boss_hotshot/hotshot_body/shin_a.inc"
};
static Sint16 thigh_a_polygons[181] = {
#include "boss_hotshot/hotshot_body/thigh_a_polygons.inc"
};
static Sint32 thigh_a_vertices[99] = {
#include "boss_hotshot/hotshot_body/thigh_a_vertices.inc"
};
static NJS_CNK_MODEL thigh_a_mesh = {
#include "boss_hotshot/hotshot_body/thigh_a_mesh.inc"
};
static NJS_CNK_OBJECT thigh_a = {
#include "boss_hotshot/hotshot_body/thigh_a.inc"
};
static Sint16 hip_a_polygons[303] = {
#include "boss_hotshot/hotshot_body/hip_a_polygons.inc"
};
static Sint32 hip_a_vertices[219] = {
#include "boss_hotshot/hotshot_body/hip_a_vertices.inc"
};
static NJS_CNK_MODEL hip_a_mesh = {
#include "boss_hotshot/hotshot_body/hip_a_mesh.inc"
};
static NJS_CNK_OBJECT hip_a = {
#include "boss_hotshot/hotshot_body/hip_a.inc"
};
static Sint16 pod_a_polygons[275] = {
#include "boss_hotshot/hotshot_body/pod_a_polygons.inc"
};
static Sint32 pod_a_vertices[147] = {
#include "boss_hotshot/hotshot_body/pod_a_vertices.inc"
};
static NJS_CNK_MODEL pod_a_mesh = {
#include "boss_hotshot/hotshot_body/pod_a_mesh.inc"
};
static NJS_CNK_OBJECT pod_a = {
#include "boss_hotshot/hotshot_body/pod_a.inc"
};
NJS_CNK_OBJECT hotshot_laser_node = {
#include "boss_hotshot/hotshot_body/hotshot_laser_node.inc"
};
static Sint16 turret_polygons[203] = {
#include "boss_hotshot/hotshot_body/turret_polygons.inc"
};
static Sint32 turret_vertices[147] = {
#include "boss_hotshot/hotshot_body/turret_vertices.inc"
};
static NJS_CNK_MODEL turret_mesh = {
#include "boss_hotshot/hotshot_body/turret_mesh.inc"
};
NJS_CNK_OBJECT hotshot_turret_node = {
#include "boss_hotshot/hotshot_body/hotshot_turret_node.inc"
};
static Sint16 pod_b_polygons[275] = {
#include "boss_hotshot/hotshot_body/pod_b_polygons.inc"
};
static Sint32 pod_b_vertices[147] = {
#include "boss_hotshot/hotshot_body/pod_b_vertices.inc"
};
static NJS_CNK_MODEL pod_b_mesh = {
#include "boss_hotshot/hotshot_body/pod_b_mesh.inc"
};
static NJS_CNK_OBJECT pod_b = {
#include "boss_hotshot/hotshot_body/pod_b.inc"
};
static Sint16 body_polygons[4253] = {
#include "boss_hotshot/hotshot_body/body_polygons.inc"
};
static Sint32 body_vertices[3141] = {
#include "boss_hotshot/hotshot_body/body_vertices.inc"
};
static NJS_CNK_MODEL body_mesh = {
#include "boss_hotshot/hotshot_body/body_mesh.inc"
};
NJS_CNK_OBJECT hotshot_body_node = {
#include "boss_hotshot/hotshot_body/hotshot_body_node.inc"
};
NJS_CNK_OBJECT hotshot_object = {
#include "boss_hotshot/hotshot_body/hotshot_object.inc"
};
static NJS_MKEY_F hotshot_hit_motion_joint_0_position[33] = {
#include "boss_hotshot/hotshot_body/hotshot_hit_motion_joint_0_position.inc"
};
static NJS_MKEY_F hotshot_hit_motion_joint_1_position[33] = {
#include "boss_hotshot/hotshot_body/hotshot_hit_motion_joint_1_position.inc"
};
static NJS_MKEY_A hotshot_hit_motion_joint_1_angles[28] = {
#include "boss_hotshot/hotshot_body/hotshot_hit_motion_joint_1_angles.inc"
};
static NJS_MKEY_A hotshot_hit_motion_joint_2_angles[2] = {
#include "boss_hotshot/hotshot_body/hotshot_hit_motion_joint_2_angles.inc"
};
static NJS_MKEY_A hotshot_hit_motion_joint_5_angles[2] = {
#include "boss_hotshot/hotshot_body/hotshot_hit_motion_joint_5_angles.inc"
};
static NJS_MKEY_A hotshot_hit_motion_joint_6_angles[31] = {
#include "boss_hotshot/hotshot_body/hotshot_hit_motion_joint_6_angles.inc"
};
static NJS_MKEY_A hotshot_hit_motion_joint_8_angles[32] = {
#include "boss_hotshot/hotshot_body/hotshot_hit_motion_joint_8_angles.inc"
};
static NJS_MKEY_A hotshot_hit_motion_joint_9_angles[33] = {
#include "boss_hotshot/hotshot_body/hotshot_hit_motion_joint_9_angles.inc"
};
static NJS_MKEY_A hotshot_hit_motion_joint_13_angles[32] = {
#include "boss_hotshot/hotshot_body/hotshot_hit_motion_joint_13_angles.inc"
};
static NJS_MKEY_A hotshot_hit_motion_joint_14_angles[18] = {
#include "boss_hotshot/hotshot_body/hotshot_hit_motion_joint_14_angles.inc"
};
static NJS_MKEY_A hotshot_hit_motion_joint_15_angles[18] = {
#include "boss_hotshot/hotshot_body/hotshot_hit_motion_joint_15_angles.inc"
};
static NJS_MKEY_A hotshot_hit_motion_joint_16_angles[18] = {
#include "boss_hotshot/hotshot_body/hotshot_hit_motion_joint_16_angles.inc"
};
static NJS_MKEY_A hotshot_hit_motion_joint_17_angles[18] = {
#include "boss_hotshot/hotshot_body/hotshot_hit_motion_joint_17_angles.inc"
};
static NJS_MKEY_A hotshot_hit_motion_joint_18_angles[18] = {
#include "boss_hotshot/hotshot_body/hotshot_hit_motion_joint_18_angles.inc"
};
static NJS_MKEY_A hotshot_hit_motion_joint_19_angles[18] = {
#include "boss_hotshot/hotshot_body/hotshot_hit_motion_joint_19_angles.inc"
};
static NJS_MDATA2 hotshot_hit_motion_tracks[22] = {
#include "boss_hotshot/hotshot_body/hotshot_hit_motion_tracks.inc"
};
NJS_MOTION hotshot_hit_motion = {
#include "boss_hotshot/hotshot_body/hotshot_hit_motion.inc"
};
static NJS_MKEY_F hotshot_collapse_motion_joint_0_position[17] = {
#include "boss_hotshot/hotshot_body/hotshot_collapse_motion_joint_0_position.inc"
};
static NJS_MKEY_F hotshot_collapse_motion_joint_1_position[17] = {
#include "boss_hotshot/hotshot_body/hotshot_collapse_motion_joint_1_position.inc"
};
static NJS_MKEY_A hotshot_collapse_motion_joint_1_angles[17] = {
#include "boss_hotshot/hotshot_body/hotshot_collapse_motion_joint_1_angles.inc"
};
static NJS_MKEY_A hotshot_collapse_motion_joint_2_angles[10] = {
#include "boss_hotshot/hotshot_body/hotshot_collapse_motion_joint_2_angles.inc"
};
static NJS_MKEY_A hotshot_collapse_motion_joint_5_angles[17] = {
#include "boss_hotshot/hotshot_body/hotshot_collapse_motion_joint_5_angles.inc"
};
static NJS_MKEY_A hotshot_collapse_motion_joint_6_angles[17] = {
#include "boss_hotshot/hotshot_body/hotshot_collapse_motion_joint_6_angles.inc"
};
static NJS_MKEY_A hotshot_collapse_motion_joint_7_angles[17] = {
#include "boss_hotshot/hotshot_body/hotshot_collapse_motion_joint_7_angles.inc"
};
static NJS_MKEY_A hotshot_collapse_motion_joint_8_angles[17] = {
#include "boss_hotshot/hotshot_body/hotshot_collapse_motion_joint_8_angles.inc"
};
static NJS_MKEY_A hotshot_collapse_motion_joint_9_angles[17] = {
#include "boss_hotshot/hotshot_body/hotshot_collapse_motion_joint_9_angles.inc"
};
static NJS_MKEY_A hotshot_collapse_motion_joint_10_angles[17] = {
#include "boss_hotshot/hotshot_body/hotshot_collapse_motion_joint_10_angles.inc"
};
static NJS_MKEY_A hotshot_collapse_motion_joint_11_angles[17] = {
#include "boss_hotshot/hotshot_body/hotshot_collapse_motion_joint_11_angles.inc"
};
static NJS_MKEY_A hotshot_collapse_motion_joint_12_angles[17] = {
#include "boss_hotshot/hotshot_body/hotshot_collapse_motion_joint_12_angles.inc"
};
static NJS_MKEY_A hotshot_collapse_motion_joint_13_angles[17] = {
#include "boss_hotshot/hotshot_body/hotshot_collapse_motion_joint_13_angles.inc"
};
static NJS_MKEY_A hotshot_collapse_motion_joint_14_angles[17] = {
#include "boss_hotshot/hotshot_body/hotshot_collapse_motion_joint_14_angles.inc"
};
static NJS_MKEY_A hotshot_collapse_motion_joint_15_angles[17] = {
#include "boss_hotshot/hotshot_body/hotshot_collapse_motion_joint_15_angles.inc"
};
static NJS_MKEY_A hotshot_collapse_motion_joint_16_angles[17] = {
#include "boss_hotshot/hotshot_body/hotshot_collapse_motion_joint_16_angles.inc"
};
static NJS_MKEY_A hotshot_collapse_motion_joint_17_angles[17] = {
#include "boss_hotshot/hotshot_body/hotshot_collapse_motion_joint_17_angles.inc"
};
static NJS_MKEY_A hotshot_collapse_motion_joint_20_angles[17] = {
#include "boss_hotshot/hotshot_body/hotshot_collapse_motion_joint_20_angles.inc"
};
static NJS_MDATA2 hotshot_collapse_motion_tracks[22] = {
#include "boss_hotshot/hotshot_body/hotshot_collapse_motion_tracks.inc"
};
NJS_MOTION hotshot_collapse_motion = {
#include "boss_hotshot/hotshot_body/hotshot_collapse_motion.inc"
};
static NJS_MKEY_F hotshot_lean_motion_joint_1_position[12] = {
#include "boss_hotshot/hotshot_body/hotshot_lean_motion_joint_1_position.inc"
};
static NJS_MKEY_A hotshot_lean_motion_joint_1_angles[12] = {
#include "boss_hotshot/hotshot_body/hotshot_lean_motion_joint_1_angles.inc"
};
static NJS_MKEY_A hotshot_lean_motion_joint_2_angles[2] = {
#include "boss_hotshot/hotshot_body/hotshot_lean_motion_joint_2_angles.inc"
};
static NJS_MKEY_A hotshot_lean_motion_joint_5_angles[2] = {
#include "boss_hotshot/hotshot_body/hotshot_lean_motion_joint_5_angles.inc"
};
static NJS_MKEY_A hotshot_lean_motion_joint_6_angles[12] = {
#include "boss_hotshot/hotshot_body/hotshot_lean_motion_joint_6_angles.inc"
};
static NJS_MKEY_A hotshot_lean_motion_joint_7_angles[12] = {
#include "boss_hotshot/hotshot_body/hotshot_lean_motion_joint_7_angles.inc"
};
static NJS_MKEY_A hotshot_lean_motion_joint_8_angles[12] = {
#include "boss_hotshot/hotshot_body/hotshot_lean_motion_joint_8_angles.inc"
};
static NJS_MKEY_A hotshot_lean_motion_joint_9_angles[12] = {
#include "boss_hotshot/hotshot_body/hotshot_lean_motion_joint_9_angles.inc"
};
static NJS_MKEY_A hotshot_lean_motion_joint_10_angles[12] = {
#include "boss_hotshot/hotshot_body/hotshot_lean_motion_joint_10_angles.inc"
};
static NJS_MKEY_A hotshot_lean_motion_joint_11_angles[12] = {
#include "boss_hotshot/hotshot_body/hotshot_lean_motion_joint_11_angles.inc"
};
static NJS_MKEY_A hotshot_lean_motion_joint_12_angles[12] = {
#include "boss_hotshot/hotshot_body/hotshot_lean_motion_joint_12_angles.inc"
};
static NJS_MKEY_A hotshot_lean_motion_joint_13_angles[12] = {
#include "boss_hotshot/hotshot_body/hotshot_lean_motion_joint_13_angles.inc"
};
static NJS_MKEY_A hotshot_lean_motion_joint_14_angles[12] = {
#include "boss_hotshot/hotshot_body/hotshot_lean_motion_joint_14_angles.inc"
};
static NJS_MKEY_A hotshot_lean_motion_joint_15_angles[12] = {
#include "boss_hotshot/hotshot_body/hotshot_lean_motion_joint_15_angles.inc"
};
static NJS_MKEY_A hotshot_lean_motion_joint_16_angles[12] = {
#include "boss_hotshot/hotshot_body/hotshot_lean_motion_joint_16_angles.inc"
};
static NJS_MKEY_A hotshot_lean_motion_joint_17_angles[12] = {
#include "boss_hotshot/hotshot_body/hotshot_lean_motion_joint_17_angles.inc"
};
static NJS_MKEY_A hotshot_lean_motion_joint_18_angles[12] = {
#include "boss_hotshot/hotshot_body/hotshot_lean_motion_joint_18_angles.inc"
};
static NJS_MKEY_A hotshot_lean_motion_joint_19_angles[12] = {
#include "boss_hotshot/hotshot_body/hotshot_lean_motion_joint_19_angles.inc"
};
static NJS_MDATA2 hotshot_lean_motion_tracks[22] = {
#include "boss_hotshot/hotshot_body/hotshot_lean_motion_tracks.inc"
};
NJS_MOTION hotshot_lean_motion = {
#include "boss_hotshot/hotshot_body/hotshot_lean_motion.inc"
};
static NJS_MKEY_F unused_a_joint_1_position[8] = {
#include "boss_hotshot/hotshot_body/unused_a_joint_1_position.inc"
};
static NJS_MKEY_A unused_a_joint_1_angles[8] = {
#include "boss_hotshot/hotshot_body/unused_a_joint_1_angles.inc"
};
static NJS_MKEY_A unused_a_joint_2_angles[2] = {
#include "boss_hotshot/hotshot_body/unused_a_joint_2_angles.inc"
};
static NJS_MKEY_A unused_a_joint_5_angles[2] = {
#include "boss_hotshot/hotshot_body/unused_a_joint_5_angles.inc"
};
static NJS_MKEY_A unused_a_joint_7_angles[2] = {
#include "boss_hotshot/hotshot_body/unused_a_joint_7_angles.inc"
};
static NJS_MKEY_A unused_a_joint_10_angles[8] = {
#include "boss_hotshot/hotshot_body/unused_a_joint_10_angles.inc"
};
static NJS_MKEY_A unused_a_joint_11_angles[8] = {
#include "boss_hotshot/hotshot_body/unused_a_joint_11_angles.inc"
};
static NJS_MKEY_A unused_a_joint_12_angles[8] = {
#include "boss_hotshot/hotshot_body/unused_a_joint_12_angles.inc"
};
static NJS_MKEY_A unused_a_joint_17_angles[8] = {
#include "boss_hotshot/hotshot_body/unused_a_joint_17_angles.inc"
};
static NJS_MKEY_A unused_a_joint_18_angles[8] = {
#include "boss_hotshot/hotshot_body/unused_a_joint_18_angles.inc"
};
static NJS_MKEY_A unused_a_joint_19_angles[8] = {
#include "boss_hotshot/hotshot_body/unused_a_joint_19_angles.inc"
};
static NJS_MDATA2 unused_a_tracks[22] = {
#include "boss_hotshot/hotshot_body/unused_a_tracks.inc"
};
static NJS_MOTION unused_a = {
#include "boss_hotshot/hotshot_body/unused_a.inc"
};
static NJS_MKEY_F unused_b_joint_1_position[8] = {
#include "boss_hotshot/hotshot_body/unused_b_joint_1_position.inc"
};
static NJS_MKEY_A unused_b_joint_1_angles[8] = {
#include "boss_hotshot/hotshot_body/unused_b_joint_1_angles.inc"
};
static NJS_MKEY_A unused_b_joint_6_angles[8] = {
#include "boss_hotshot/hotshot_body/unused_b_joint_6_angles.inc"
};
static NJS_MKEY_A unused_b_joint_7_angles[8] = {
#include "boss_hotshot/hotshot_body/unused_b_joint_7_angles.inc"
};
static NJS_MKEY_A unused_b_joint_8_angles[8] = {
#include "boss_hotshot/hotshot_body/unused_b_joint_8_angles.inc"
};
static NJS_MKEY_A unused_b_joint_9_angles[8] = {
#include "boss_hotshot/hotshot_body/unused_b_joint_9_angles.inc"
};
static NJS_MKEY_A unused_b_joint_10_angles[8] = {
#include "boss_hotshot/hotshot_body/unused_b_joint_10_angles.inc"
};
static NJS_MKEY_A unused_b_joint_11_angles[8] = {
#include "boss_hotshot/hotshot_body/unused_b_joint_11_angles.inc"
};
static NJS_MKEY_A unused_b_joint_12_angles[8] = {
#include "boss_hotshot/hotshot_body/unused_b_joint_12_angles.inc"
};
static NJS_MKEY_A unused_b_joint_13_angles[8] = {
#include "boss_hotshot/hotshot_body/unused_b_joint_13_angles.inc"
};
static NJS_MKEY_A unused_b_joint_14_angles[8] = {
#include "boss_hotshot/hotshot_body/unused_b_joint_14_angles.inc"
};
static NJS_MKEY_A unused_b_joint_15_angles[8] = {
#include "boss_hotshot/hotshot_body/unused_b_joint_15_angles.inc"
};
static NJS_MKEY_A unused_b_joint_16_angles[8] = {
#include "boss_hotshot/hotshot_body/unused_b_joint_16_angles.inc"
};
static NJS_MKEY_A unused_b_joint_17_angles[2] = {
#include "boss_hotshot/hotshot_body/unused_b_joint_17_angles.inc"
};
static NJS_MKEY_A unused_b_joint_18_angles[8] = {
#include "boss_hotshot/hotshot_body/unused_b_joint_18_angles.inc"
};
static NJS_MKEY_A unused_b_joint_19_angles[8] = {
#include "boss_hotshot/hotshot_body/unused_b_joint_19_angles.inc"
};
static NJS_MDATA2 unused_b_tracks[22] = {
#include "boss_hotshot/hotshot_body/unused_b_tracks.inc"
};
static NJS_MOTION unused_b = {
#include "boss_hotshot/hotshot_body/unused_b.inc"
};
static NJS_MKEY_F unused_c_joint_1_position[13] = {
#include "boss_hotshot/hotshot_body/unused_c_joint_1_position.inc"
};
static NJS_MKEY_A unused_c_joint_1_angles[10] = {
#include "boss_hotshot/hotshot_body/unused_c_joint_1_angles.inc"
};
static NJS_MKEY_A unused_c_joint_2_angles[10] = {
#include "boss_hotshot/hotshot_body/unused_c_joint_2_angles.inc"
};
static NJS_MKEY_A unused_c_joint_5_angles[10] = {
#include "boss_hotshot/hotshot_body/unused_c_joint_5_angles.inc"
};
static NJS_MKEY_A unused_c_joint_6_angles[10] = {
#include "boss_hotshot/hotshot_body/unused_c_joint_6_angles.inc"
};
static NJS_MKEY_A unused_c_joint_7_angles[11] = {
#include "boss_hotshot/hotshot_body/unused_c_joint_7_angles.inc"
};
static NJS_MKEY_A unused_c_joint_8_angles[10] = {
#include "boss_hotshot/hotshot_body/unused_c_joint_8_angles.inc"
};
static NJS_MKEY_A unused_c_joint_9_angles[10] = {
#include "boss_hotshot/hotshot_body/unused_c_joint_9_angles.inc"
};
static NJS_MKEY_A unused_c_joint_10_angles[6] = {
#include "boss_hotshot/hotshot_body/unused_c_joint_10_angles.inc"
};
static NJS_MKEY_A unused_c_joint_11_angles[6] = {
#include "boss_hotshot/hotshot_body/unused_c_joint_11_angles.inc"
};
static NJS_MKEY_A unused_c_joint_12_angles[6] = {
#include "boss_hotshot/hotshot_body/unused_c_joint_12_angles.inc"
};
static NJS_MKEY_A unused_c_joint_13_angles[10] = {
#include "boss_hotshot/hotshot_body/unused_c_joint_13_angles.inc"
};
static NJS_MKEY_A unused_c_joint_14_angles[10] = {
#include "boss_hotshot/hotshot_body/unused_c_joint_14_angles.inc"
};
static NJS_MKEY_A unused_c_joint_15_angles[10] = {
#include "boss_hotshot/hotshot_body/unused_c_joint_15_angles.inc"
};
static NJS_MKEY_A unused_c_joint_16_angles[10] = {
#include "boss_hotshot/hotshot_body/unused_c_joint_16_angles.inc"
};
static NJS_MKEY_A unused_c_joint_17_angles[6] = {
#include "boss_hotshot/hotshot_body/unused_c_joint_17_angles.inc"
};
static NJS_MKEY_A unused_c_joint_18_angles[6] = {
#include "boss_hotshot/hotshot_body/unused_c_joint_18_angles.inc"
};
static NJS_MKEY_A unused_c_joint_19_angles[6] = {
#include "boss_hotshot/hotshot_body/unused_c_joint_19_angles.inc"
};
static NJS_MDATA2 unused_c_tracks[22] = {
#include "boss_hotshot/hotshot_body/unused_c_tracks.inc"
};
static NJS_MOTION unused_c = {
#include "boss_hotshot/hotshot_body/unused_c.inc"
};
static NJS_MKEY_F hotshot_stomp_motion_joint_1_position[14] = {
#include "boss_hotshot/hotshot_body/hotshot_stomp_motion_joint_1_position.inc"
};
static NJS_MKEY_A hotshot_stomp_motion_joint_1_angles[14] = {
#include "boss_hotshot/hotshot_body/hotshot_stomp_motion_joint_1_angles.inc"
};
static NJS_MKEY_A hotshot_stomp_motion_joint_2_angles[10] = {
#include "boss_hotshot/hotshot_body/hotshot_stomp_motion_joint_2_angles.inc"
};
static NJS_MKEY_A hotshot_stomp_motion_joint_5_angles[10] = {
#include "boss_hotshot/hotshot_body/hotshot_stomp_motion_joint_5_angles.inc"
};
static NJS_MKEY_A hotshot_stomp_motion_joint_6_angles[14] = {
#include "boss_hotshot/hotshot_body/hotshot_stomp_motion_joint_6_angles.inc"
};
static NJS_MKEY_A hotshot_stomp_motion_joint_7_angles[14] = {
#include "boss_hotshot/hotshot_body/hotshot_stomp_motion_joint_7_angles.inc"
};
static NJS_MKEY_A hotshot_stomp_motion_joint_8_angles[14] = {
#include "boss_hotshot/hotshot_body/hotshot_stomp_motion_joint_8_angles.inc"
};
static NJS_MKEY_A hotshot_stomp_motion_joint_9_angles[14] = {
#include "boss_hotshot/hotshot_body/hotshot_stomp_motion_joint_9_angles.inc"
};
static NJS_MKEY_A hotshot_stomp_motion_joint_10_angles[14] = {
#include "boss_hotshot/hotshot_body/hotshot_stomp_motion_joint_10_angles.inc"
};
static NJS_MKEY_A hotshot_stomp_motion_joint_11_angles[14] = {
#include "boss_hotshot/hotshot_body/hotshot_stomp_motion_joint_11_angles.inc"
};
static NJS_MKEY_A hotshot_stomp_motion_joint_12_angles[14] = {
#include "boss_hotshot/hotshot_body/hotshot_stomp_motion_joint_12_angles.inc"
};
static NJS_MKEY_A hotshot_stomp_motion_joint_13_angles[14] = {
#include "boss_hotshot/hotshot_body/hotshot_stomp_motion_joint_13_angles.inc"
};
static NJS_MKEY_A hotshot_stomp_motion_joint_14_angles[14] = {
#include "boss_hotshot/hotshot_body/hotshot_stomp_motion_joint_14_angles.inc"
};
static NJS_MKEY_A hotshot_stomp_motion_joint_15_angles[14] = {
#include "boss_hotshot/hotshot_body/hotshot_stomp_motion_joint_15_angles.inc"
};
static NJS_MKEY_A hotshot_stomp_motion_joint_16_angles[14] = {
#include "boss_hotshot/hotshot_body/hotshot_stomp_motion_joint_16_angles.inc"
};
static NJS_MKEY_A hotshot_stomp_motion_joint_17_angles[14] = {
#include "boss_hotshot/hotshot_body/hotshot_stomp_motion_joint_17_angles.inc"
};
static NJS_MKEY_A hotshot_stomp_motion_joint_18_angles[14] = {
#include "boss_hotshot/hotshot_body/hotshot_stomp_motion_joint_18_angles.inc"
};
static NJS_MKEY_A hotshot_stomp_motion_joint_19_angles[14] = {
#include "boss_hotshot/hotshot_body/hotshot_stomp_motion_joint_19_angles.inc"
};
static NJS_MDATA2 hotshot_stomp_motion_tracks[22] = {
#include "boss_hotshot/hotshot_body/hotshot_stomp_motion_tracks.inc"
};
NJS_MOTION hotshot_stomp_motion = {
#include "boss_hotshot/hotshot_body/hotshot_stomp_motion.inc"
};
static NJS_MKEY_F hotshot_stand_motion_joint_1_position[4] = {
#include "boss_hotshot/hotshot_body/hotshot_stand_motion_joint_1_position.inc"
};
static NJS_MKEY_A hotshot_stand_motion_joint_6_angles[4] = {
#include "boss_hotshot/hotshot_body/hotshot_stand_motion_joint_6_angles.inc"
};
static NJS_MKEY_A hotshot_stand_motion_joint_7_angles[4] = {
#include "boss_hotshot/hotshot_body/hotshot_stand_motion_joint_7_angles.inc"
};
static NJS_MKEY_A hotshot_stand_motion_joint_13_angles[4] = {
#include "boss_hotshot/hotshot_body/hotshot_stand_motion_joint_13_angles.inc"
};
static NJS_MKEY_A hotshot_stand_motion_joint_14_angles[4] = {
#include "boss_hotshot/hotshot_body/hotshot_stand_motion_joint_14_angles.inc"
};
static NJS_MDATA2 hotshot_stand_motion_tracks[22] = {
#include "boss_hotshot/hotshot_body/hotshot_stand_motion_tracks.inc"
};
NJS_MOTION hotshot_stand_motion = {
#include "boss_hotshot/hotshot_body/hotshot_stand_motion.inc"
};
static NJS_MKEY_F hotshot_step_motion_joint_1_position[32] = {
#include "boss_hotshot/hotshot_body/hotshot_step_motion_joint_1_position.inc"
};
static NJS_MKEY_A hotshot_step_motion_joint_6_angles[32] = {
#include "boss_hotshot/hotshot_body/hotshot_step_motion_joint_6_angles.inc"
};
static NJS_MKEY_A hotshot_step_motion_joint_7_angles[32] = {
#include "boss_hotshot/hotshot_body/hotshot_step_motion_joint_7_angles.inc"
};
static NJS_MKEY_A hotshot_step_motion_joint_8_angles[32] = {
#include "boss_hotshot/hotshot_body/hotshot_step_motion_joint_8_angles.inc"
};
static NJS_MKEY_A hotshot_step_motion_joint_9_angles[32] = {
#include "boss_hotshot/hotshot_body/hotshot_step_motion_joint_9_angles.inc"
};
static NJS_MKEY_A hotshot_step_motion_joint_10_angles[32] = {
#include "boss_hotshot/hotshot_body/hotshot_step_motion_joint_10_angles.inc"
};
static NJS_MKEY_A hotshot_step_motion_joint_11_angles[32] = {
#include "boss_hotshot/hotshot_body/hotshot_step_motion_joint_11_angles.inc"
};
static NJS_MKEY_A hotshot_step_motion_joint_12_angles[32] = {
#include "boss_hotshot/hotshot_body/hotshot_step_motion_joint_12_angles.inc"
};
static NJS_MKEY_A hotshot_step_motion_joint_13_angles[32] = {
#include "boss_hotshot/hotshot_body/hotshot_step_motion_joint_13_angles.inc"
};
static NJS_MKEY_A hotshot_step_motion_joint_14_angles[32] = {
#include "boss_hotshot/hotshot_body/hotshot_step_motion_joint_14_angles.inc"
};
static NJS_MKEY_A hotshot_step_motion_joint_15_angles[32] = {
#include "boss_hotshot/hotshot_body/hotshot_step_motion_joint_15_angles.inc"
};
static NJS_MKEY_A hotshot_step_motion_joint_16_angles[32] = {
#include "boss_hotshot/hotshot_body/hotshot_step_motion_joint_16_angles.inc"
};
static NJS_MKEY_A hotshot_step_motion_joint_17_angles[26] = {
#include "boss_hotshot/hotshot_body/hotshot_step_motion_joint_17_angles.inc"
};
static NJS_MKEY_A hotshot_step_motion_joint_18_angles[26] = {
#include "boss_hotshot/hotshot_body/hotshot_step_motion_joint_18_angles.inc"
};
static NJS_MKEY_A hotshot_step_motion_joint_19_angles[26] = {
#include "boss_hotshot/hotshot_body/hotshot_step_motion_joint_19_angles.inc"
};
static NJS_MDATA2 hotshot_step_motion_tracks[22] = {
#include "boss_hotshot/hotshot_body/hotshot_step_motion_tracks.inc"
};
NJS_MOTION hotshot_step_motion = {
#include "boss_hotshot/hotshot_body/hotshot_step_motion.inc"
};
static NJS_MKEY_F hotshot_take_off_motion_joint_0_position[9] = {
#include "boss_hotshot/hotshot_body/hotshot_take_off_motion_joint_0_position.inc"
};
static NJS_MKEY_F hotshot_take_off_motion_joint_1_position[17] = {
#include "boss_hotshot/hotshot_body/hotshot_take_off_motion_joint_1_position.inc"
};
static NJS_MKEY_A hotshot_take_off_motion_joint_1_angles[32] = {
#include "boss_hotshot/hotshot_body/hotshot_take_off_motion_joint_1_angles.inc"
};
static NJS_MKEY_F hotshot_take_off_motion_joint_4_position[2] = {
#include "boss_hotshot/hotshot_body/hotshot_take_off_motion_joint_4_position.inc"
};
static NJS_MKEY_A hotshot_take_off_motion_joint_6_angles[32] = {
#include "boss_hotshot/hotshot_body/hotshot_take_off_motion_joint_6_angles.inc"
};
static NJS_MKEY_A hotshot_take_off_motion_joint_7_angles[32] = {
#include "boss_hotshot/hotshot_body/hotshot_take_off_motion_joint_7_angles.inc"
};
static NJS_MKEY_A hotshot_take_off_motion_joint_8_angles[32] = {
#include "boss_hotshot/hotshot_body/hotshot_take_off_motion_joint_8_angles.inc"
};
static NJS_MKEY_A hotshot_take_off_motion_joint_9_angles[21] = {
#include "boss_hotshot/hotshot_body/hotshot_take_off_motion_joint_9_angles.inc"
};
static NJS_MKEY_A hotshot_take_off_motion_joint_10_angles[15] = {
#include "boss_hotshot/hotshot_body/hotshot_take_off_motion_joint_10_angles.inc"
};
static NJS_MKEY_A hotshot_take_off_motion_joint_11_angles[15] = {
#include "boss_hotshot/hotshot_body/hotshot_take_off_motion_joint_11_angles.inc"
};
static NJS_MKEY_A hotshot_take_off_motion_joint_12_angles[15] = {
#include "boss_hotshot/hotshot_body/hotshot_take_off_motion_joint_12_angles.inc"
};
static NJS_MKEY_A hotshot_take_off_motion_joint_13_angles[32] = {
#include "boss_hotshot/hotshot_body/hotshot_take_off_motion_joint_13_angles.inc"
};
static NJS_MKEY_A hotshot_take_off_motion_joint_14_angles[32] = {
#include "boss_hotshot/hotshot_body/hotshot_take_off_motion_joint_14_angles.inc"
};
static NJS_MKEY_A hotshot_take_off_motion_joint_15_angles[32] = {
#include "boss_hotshot/hotshot_body/hotshot_take_off_motion_joint_15_angles.inc"
};
static NJS_MKEY_A hotshot_take_off_motion_joint_16_angles[21] = {
#include "boss_hotshot/hotshot_body/hotshot_take_off_motion_joint_16_angles.inc"
};
static NJS_MKEY_A hotshot_take_off_motion_joint_17_angles[15] = {
#include "boss_hotshot/hotshot_body/hotshot_take_off_motion_joint_17_angles.inc"
};
static NJS_MKEY_A hotshot_take_off_motion_joint_18_angles[15] = {
#include "boss_hotshot/hotshot_body/hotshot_take_off_motion_joint_18_angles.inc"
};
static NJS_MKEY_A hotshot_take_off_motion_joint_19_angles[15] = {
#include "boss_hotshot/hotshot_body/hotshot_take_off_motion_joint_19_angles.inc"
};
static NJS_MKEY_F hotshot_take_off_motion_joint_21_position[2] = {
#include "boss_hotshot/hotshot_body/hotshot_take_off_motion_joint_21_position.inc"
};
static NJS_MDATA2 hotshot_take_off_motion_tracks[22] = {
#include "boss_hotshot/hotshot_body/hotshot_take_off_motion_tracks.inc"
};
NJS_MOTION hotshot_take_off_motion = {
#include "boss_hotshot/hotshot_body/hotshot_take_off_motion.inc"
};
static NJS_MKEY_F hotshot_hit_empty_motion_joint_0_position[33] = {
#include "boss_hotshot/hotshot_body/hotshot_hit_empty_motion_joint_0_position.inc"
};
static NJS_MKEY_F hotshot_hit_empty_motion_joint_1_position[33] = {
#include "boss_hotshot/hotshot_body/hotshot_hit_empty_motion_joint_1_position.inc"
};
static NJS_MKEY_A hotshot_hit_empty_motion_joint_1_angles[28] = {
#include "boss_hotshot/hotshot_body/hotshot_hit_empty_motion_joint_1_angles.inc"
};
static NJS_MKEY_F hotshot_hit_empty_motion_joint_4_position[2] = {
#include "boss_hotshot/hotshot_body/hotshot_hit_empty_motion_joint_4_position.inc"
};
static NJS_MKEY_A hotshot_hit_empty_motion_joint_6_angles[31] = {
#include "boss_hotshot/hotshot_body/hotshot_hit_empty_motion_joint_6_angles.inc"
};
static NJS_MKEY_A hotshot_hit_empty_motion_joint_8_angles[32] = {
#include "boss_hotshot/hotshot_body/hotshot_hit_empty_motion_joint_8_angles.inc"
};
static NJS_MKEY_A hotshot_hit_empty_motion_joint_9_angles[33] = {
#include "boss_hotshot/hotshot_body/hotshot_hit_empty_motion_joint_9_angles.inc"
};
static NJS_MKEY_A hotshot_hit_empty_motion_joint_13_angles[32] = {
#include "boss_hotshot/hotshot_body/hotshot_hit_empty_motion_joint_13_angles.inc"
};
static NJS_MKEY_A hotshot_hit_empty_motion_joint_14_angles[18] = {
#include "boss_hotshot/hotshot_body/hotshot_hit_empty_motion_joint_14_angles.inc"
};
static NJS_MKEY_A hotshot_hit_empty_motion_joint_15_angles[18] = {
#include "boss_hotshot/hotshot_body/hotshot_hit_empty_motion_joint_15_angles.inc"
};
static NJS_MKEY_A hotshot_hit_empty_motion_joint_16_angles[18] = {
#include "boss_hotshot/hotshot_body/hotshot_hit_empty_motion_joint_16_angles.inc"
};
static NJS_MKEY_A hotshot_hit_empty_motion_joint_17_angles[18] = {
#include "boss_hotshot/hotshot_body/hotshot_hit_empty_motion_joint_17_angles.inc"
};
static NJS_MKEY_A hotshot_hit_empty_motion_joint_18_angles[18] = {
#include "boss_hotshot/hotshot_body/hotshot_hit_empty_motion_joint_18_angles.inc"
};
static NJS_MKEY_A hotshot_hit_empty_motion_joint_19_angles[18] = {
#include "boss_hotshot/hotshot_body/hotshot_hit_empty_motion_joint_19_angles.inc"
};
static NJS_MKEY_F hotshot_hit_empty_motion_joint_21_position[2] = {
#include "boss_hotshot/hotshot_body/hotshot_hit_empty_motion_joint_21_position.inc"
};
static NJS_MDATA2 hotshot_hit_empty_motion_tracks[22] = {
#include "boss_hotshot/hotshot_body/hotshot_hit_empty_motion_tracks.inc"
};
NJS_MOTION hotshot_hit_empty_motion = {
#include "boss_hotshot/hotshot_body/hotshot_hit_empty_motion.inc"
};
