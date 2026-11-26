/* BOSS_GOLEM.PRS's motions, from the disc. its D lies between golem_clips.c
 * and golem.c */
#include <ninja.h>

#include "shared/golem_clips.h"

static NJS_MKEY_A golem_motion_opening_joint_0_angles[2] = {
#include "boss_golem/dat_8c606ba8/golem_motion_opening_joint_0_angles.inc"
};
static NJS_MKEY_A golem_motion_opening_joint_1_angles[2] = {
#include "boss_golem/dat_8c606ba8/golem_motion_opening_joint_1_angles.inc"
};
static NJS_MKEY_A golem_motion_opening_joint_2_angles[2] = {
#include "boss_golem/dat_8c606ba8/golem_motion_opening_joint_2_angles.inc"
};
static NJS_MKEY_A golem_motion_opening_joint_3_angles[2] = {
#include "boss_golem/dat_8c606ba8/golem_motion_opening_joint_3_angles.inc"
};
static NJS_MKEY_A golem_motion_opening_joint_4_angles[2] = {
#include "boss_golem/dat_8c606ba8/golem_motion_opening_joint_4_angles.inc"
};
static NJS_MKEY_A golem_motion_opening_joint_5_angles[105] = {
#include "boss_golem/dat_8c606ba8/golem_motion_opening_joint_5_angles.inc"
};
static NJS_MKEY_A golem_motion_opening_joint_6_angles[38] = {
#include "boss_golem/dat_8c606ba8/golem_motion_opening_joint_6_angles.inc"
};
static NJS_MKEY_A golem_motion_opening_joint_7_angles[26] = {
#include "boss_golem/dat_8c606ba8/golem_motion_opening_joint_7_angles.inc"
};
static NJS_MKEY_A golem_motion_opening_joint_8_angles[46] = {
#include "boss_golem/dat_8c606ba8/golem_motion_opening_joint_8_angles.inc"
};
static NJS_MKEY_A golem_motion_opening_joint_9_angles[84] = {
#include "boss_golem/dat_8c606ba8/golem_motion_opening_joint_9_angles.inc"
};
static NJS_MKEY_A golem_motion_opening_joint_10_angles[2] = {
#include "boss_golem/dat_8c606ba8/golem_motion_opening_joint_10_angles.inc"
};
static NJS_MKEY_A golem_motion_opening_joint_11_angles[2] = {
#include "boss_golem/dat_8c606ba8/golem_motion_opening_joint_11_angles.inc"
};
static NJS_MKEY_A golem_motion_opening_joint_12_angles[2] = {
#include "boss_golem/dat_8c606ba8/golem_motion_opening_joint_12_angles.inc"
};
static NJS_MKEY_A golem_motion_opening_joint_13_angles[32] = {
#include "boss_golem/dat_8c606ba8/golem_motion_opening_joint_13_angles.inc"
};
static NJS_MKEY_A golem_motion_opening_joint_14_angles[26] = {
#include "boss_golem/dat_8c606ba8/golem_motion_opening_joint_14_angles.inc"
};
static NJS_MKEY_A golem_motion_opening_joint_15_angles[2] = {
#include "boss_golem/dat_8c606ba8/golem_motion_opening_joint_15_angles.inc"
};
static NJS_MKEY_A golem_motion_opening_joint_16_angles[49] = {
#include "boss_golem/dat_8c606ba8/golem_motion_opening_joint_16_angles.inc"
};
static NJS_MKEY_A golem_motion_opening_joint_17_angles[2] = {
#include "boss_golem/dat_8c606ba8/golem_motion_opening_joint_17_angles.inc"
};
static NJS_MKEY_A golem_motion_opening_joint_18_angles[2] = {
#include "boss_golem/dat_8c606ba8/golem_motion_opening_joint_18_angles.inc"
};
static NJS_MKEY_A golem_motion_opening_joint_19_angles[2] = {
#include "boss_golem/dat_8c606ba8/golem_motion_opening_joint_19_angles.inc"
};
static NJS_MKEY_A golem_motion_opening_joint_20_angles[107] = {
#include "boss_golem/dat_8c606ba8/golem_motion_opening_joint_20_angles.inc"
};
static NJS_MDATA3 golem_motion_opening_tracks[21] = {
#include "boss_golem/dat_8c606ba8/golem_motion_opening_tracks.inc"
};
NJS_MOTION golem_motion_opening = {
#include "boss_golem/dat_8c606ba8/golem_motion_opening.inc"
};
static NJS_MKEY_F golem_motion_pose_62_joint_0_position[2] = {
#include "boss_golem/dat_8c606ba8/golem_motion_pose_62_joint_0_position.inc"
};
static NJS_MKEY_A golem_motion_pose_62_joint_0_angles[2] = {
#include "boss_golem/dat_8c606ba8/golem_motion_pose_62_joint_0_angles.inc"
};
static NJS_MKEY_F golem_motion_pose_62_joint_1_position[2] = {
#include "boss_golem/dat_8c606ba8/golem_motion_pose_62_joint_1_position.inc"
};
static NJS_MKEY_A golem_motion_pose_62_joint_1_angles[2] = {
#include "boss_golem/dat_8c606ba8/golem_motion_pose_62_joint_1_angles.inc"
};
static NJS_MKEY_F golem_motion_pose_62_joint_2_position[2] = {
#include "boss_golem/dat_8c606ba8/golem_motion_pose_62_joint_2_position.inc"
};
static NJS_MKEY_A golem_motion_pose_62_joint_2_angles[2] = {
#include "boss_golem/dat_8c606ba8/golem_motion_pose_62_joint_2_angles.inc"
};
static NJS_MKEY_F golem_motion_pose_62_joint_3_position[2] = {
#include "boss_golem/dat_8c606ba8/golem_motion_pose_62_joint_3_position.inc"
};
static NJS_MKEY_A golem_motion_pose_62_joint_3_angles[2] = {
#include "boss_golem/dat_8c606ba8/golem_motion_pose_62_joint_3_angles.inc"
};
static NJS_MKEY_F golem_motion_pose_62_joint_4_position[2] = {
#include "boss_golem/dat_8c606ba8/golem_motion_pose_62_joint_4_position.inc"
};
static NJS_MKEY_A golem_motion_pose_62_joint_4_angles[2] = {
#include "boss_golem/dat_8c606ba8/golem_motion_pose_62_joint_4_angles.inc"
};
static NJS_MKEY_F golem_motion_pose_62_joint_5_position[2] = {
#include "boss_golem/dat_8c606ba8/golem_motion_pose_62_joint_5_position.inc"
};
static NJS_MKEY_A golem_motion_pose_62_joint_5_angles[2] = {
#include "boss_golem/dat_8c606ba8/golem_motion_pose_62_joint_5_angles.inc"
};
static NJS_MKEY_F golem_motion_pose_62_joint_6_position[2] = {
#include "boss_golem/dat_8c606ba8/golem_motion_pose_62_joint_6_position.inc"
};
static NJS_MKEY_A golem_motion_pose_62_joint_6_angles[2] = {
#include "boss_golem/dat_8c606ba8/golem_motion_pose_62_joint_6_angles.inc"
};
static NJS_MKEY_F golem_motion_pose_62_joint_7_position[2] = {
#include "boss_golem/dat_8c606ba8/golem_motion_pose_62_joint_7_position.inc"
};
static NJS_MKEY_A golem_motion_pose_62_joint_7_angles[2] = {
#include "boss_golem/dat_8c606ba8/golem_motion_pose_62_joint_7_angles.inc"
};
static NJS_MKEY_F golem_motion_pose_62_joint_8_position[2] = {
#include "boss_golem/dat_8c606ba8/golem_motion_pose_62_joint_8_position.inc"
};
static NJS_MKEY_A golem_motion_pose_62_joint_8_angles[2] = {
#include "boss_golem/dat_8c606ba8/golem_motion_pose_62_joint_8_angles.inc"
};
static NJS_MKEY_F golem_motion_pose_62_joint_9_position[2] = {
#include "boss_golem/dat_8c606ba8/golem_motion_pose_62_joint_9_position.inc"
};
static NJS_MKEY_A golem_motion_pose_62_joint_9_angles[2] = {
#include "boss_golem/dat_8c606ba8/golem_motion_pose_62_joint_9_angles.inc"
};
static NJS_MKEY_F golem_motion_pose_62_joint_10_position[2] = {
#include "boss_golem/dat_8c606ba8/golem_motion_pose_62_joint_10_position.inc"
};
static NJS_MKEY_A golem_motion_pose_62_joint_10_angles[2] = {
#include "boss_golem/dat_8c606ba8/golem_motion_pose_62_joint_10_angles.inc"
};
static NJS_MKEY_F golem_motion_pose_62_joint_11_position[2] = {
#include "boss_golem/dat_8c606ba8/golem_motion_pose_62_joint_11_position.inc"
};
static NJS_MKEY_A golem_motion_pose_62_joint_11_angles[2] = {
#include "boss_golem/dat_8c606ba8/golem_motion_pose_62_joint_11_angles.inc"
};
static NJS_MKEY_F golem_motion_pose_62_joint_12_position[2] = {
#include "boss_golem/dat_8c606ba8/golem_motion_pose_62_joint_12_position.inc"
};
static NJS_MKEY_A golem_motion_pose_62_joint_12_angles[2] = {
#include "boss_golem/dat_8c606ba8/golem_motion_pose_62_joint_12_angles.inc"
};
static NJS_MKEY_F golem_motion_pose_62_joint_13_position[2] = {
#include "boss_golem/dat_8c606ba8/golem_motion_pose_62_joint_13_position.inc"
};
static NJS_MKEY_A golem_motion_pose_62_joint_13_angles[2] = {
#include "boss_golem/dat_8c606ba8/golem_motion_pose_62_joint_13_angles.inc"
};
static NJS_MKEY_F golem_motion_pose_62_joint_14_position[2] = {
#include "boss_golem/dat_8c606ba8/golem_motion_pose_62_joint_14_position.inc"
};
static NJS_MKEY_A golem_motion_pose_62_joint_14_angles[2] = {
#include "boss_golem/dat_8c606ba8/golem_motion_pose_62_joint_14_angles.inc"
};
static NJS_MKEY_F golem_motion_pose_62_joint_15_position[2] = {
#include "boss_golem/dat_8c606ba8/golem_motion_pose_62_joint_15_position.inc"
};
static NJS_MKEY_A golem_motion_pose_62_joint_15_angles[2] = {
#include "boss_golem/dat_8c606ba8/golem_motion_pose_62_joint_15_angles.inc"
};
static NJS_MKEY_F golem_motion_pose_62_joint_16_position[2] = {
#include "boss_golem/dat_8c606ba8/golem_motion_pose_62_joint_16_position.inc"
};
static NJS_MKEY_A golem_motion_pose_62_joint_16_angles[2] = {
#include "boss_golem/dat_8c606ba8/golem_motion_pose_62_joint_16_angles.inc"
};
static NJS_MKEY_F golem_motion_pose_62_joint_17_position[2] = {
#include "boss_golem/dat_8c606ba8/golem_motion_pose_62_joint_17_position.inc"
};
static NJS_MKEY_A golem_motion_pose_62_joint_17_angles[2] = {
#include "boss_golem/dat_8c606ba8/golem_motion_pose_62_joint_17_angles.inc"
};
static NJS_MKEY_F golem_motion_pose_62_joint_18_position[2] = {
#include "boss_golem/dat_8c606ba8/golem_motion_pose_62_joint_18_position.inc"
};
static NJS_MKEY_A golem_motion_pose_62_joint_18_angles[2] = {
#include "boss_golem/dat_8c606ba8/golem_motion_pose_62_joint_18_angles.inc"
};
static NJS_MKEY_F golem_motion_pose_62_joint_19_position[2] = {
#include "boss_golem/dat_8c606ba8/golem_motion_pose_62_joint_19_position.inc"
};
static NJS_MKEY_A golem_motion_pose_62_joint_19_angles[2] = {
#include "boss_golem/dat_8c606ba8/golem_motion_pose_62_joint_19_angles.inc"
};
static NJS_MKEY_F golem_motion_pose_62_joint_20_position[2] = {
#include "boss_golem/dat_8c606ba8/golem_motion_pose_62_joint_20_position.inc"
};
static NJS_MKEY_A golem_motion_pose_62_joint_20_angles[2] = {
#include "boss_golem/dat_8c606ba8/golem_motion_pose_62_joint_20_angles.inc"
};
static NJS_MDATA3 golem_motion_pose_62_tracks[21] = {
#include "boss_golem/dat_8c606ba8/golem_motion_pose_62_tracks.inc"
};
NJS_MOTION golem_motion_pose_62 = {
#include "boss_golem/dat_8c606ba8/golem_motion_pose_62.inc"
};
