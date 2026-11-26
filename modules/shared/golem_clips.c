/* the motions of BOSS_GOLEM.PRS and BOSS_GOLEM_E.PRS, from the disc. its D's
 * neighbours differ by image */
#include <ninja.h>

#include "shared/golem_clips.h"

static NJS_MKEY_F golem_motion_slam_start_joint_0_position[2] = {
#include "shared/golem_clips/golem_motion_slam_start_joint_0_position.inc"
};
static NJS_MKEY_A golem_motion_slam_start_joint_0_angles[2] = {
#include "shared/golem_clips/golem_motion_slam_start_joint_0_angles.inc"
};
static NJS_MKEY_F golem_motion_slam_start_joint_1_position[2] = {
#include "shared/golem_clips/golem_motion_slam_start_joint_1_position.inc"
};
static NJS_MKEY_A golem_motion_slam_start_joint_1_angles[2] = {
#include "shared/golem_clips/golem_motion_slam_start_joint_1_angles.inc"
};
static NJS_MKEY_F golem_motion_slam_start_joint_2_position[2] = {
#include "shared/golem_clips/golem_motion_slam_start_joint_2_position.inc"
};
static NJS_MKEY_A golem_motion_slam_start_joint_2_angles[2] = {
#include "shared/golem_clips/golem_motion_slam_start_joint_2_angles.inc"
};
static NJS_MKEY_F golem_motion_slam_start_joint_3_position[2] = {
#include "shared/golem_clips/golem_motion_slam_start_joint_3_position.inc"
};
static NJS_MKEY_A golem_motion_slam_start_joint_3_angles[2] = {
#include "shared/golem_clips/golem_motion_slam_start_joint_3_angles.inc"
};
static NJS_MKEY_F golem_motion_slam_start_joint_4_position[2] = {
#include "shared/golem_clips/golem_motion_slam_start_joint_4_position.inc"
};
static NJS_MKEY_A golem_motion_slam_start_joint_4_angles[2] = {
#include "shared/golem_clips/golem_motion_slam_start_joint_4_angles.inc"
};
static NJS_MKEY_F golem_motion_slam_start_joint_5_position[2] = {
#include "shared/golem_clips/golem_motion_slam_start_joint_5_position.inc"
};
static NJS_MKEY_A golem_motion_slam_start_joint_5_angles[30] = {
#include "shared/golem_clips/golem_motion_slam_start_joint_5_angles.inc"
};
static NJS_MKEY_F golem_motion_slam_start_joint_6_position[2] = {
#include "shared/golem_clips/golem_motion_slam_start_joint_6_position.inc"
};
static NJS_MKEY_A golem_motion_slam_start_joint_6_angles[30] = {
#include "shared/golem_clips/golem_motion_slam_start_joint_6_angles.inc"
};
static NJS_MKEY_F golem_motion_slam_start_joint_7_position[2] = {
#include "shared/golem_clips/golem_motion_slam_start_joint_7_position.inc"
};
static NJS_MKEY_A golem_motion_slam_start_joint_7_angles[30] = {
#include "shared/golem_clips/golem_motion_slam_start_joint_7_angles.inc"
};
static NJS_MKEY_F golem_motion_slam_start_joint_8_position[2] = {
#include "shared/golem_clips/golem_motion_slam_start_joint_8_position.inc"
};
static NJS_MKEY_A golem_motion_slam_start_joint_8_angles[30] = {
#include "shared/golem_clips/golem_motion_slam_start_joint_8_angles.inc"
};
static NJS_MKEY_F golem_motion_slam_start_joint_9_position[2] = {
#include "shared/golem_clips/golem_motion_slam_start_joint_9_position.inc"
};
static NJS_MKEY_A golem_motion_slam_start_joint_9_angles[30] = {
#include "shared/golem_clips/golem_motion_slam_start_joint_9_angles.inc"
};
static NJS_MKEY_F golem_motion_slam_start_joint_10_position[2] = {
#include "shared/golem_clips/golem_motion_slam_start_joint_10_position.inc"
};
static NJS_MKEY_A golem_motion_slam_start_joint_10_angles[28] = {
#include "shared/golem_clips/golem_motion_slam_start_joint_10_angles.inc"
};
static NJS_MKEY_F golem_motion_slam_start_joint_11_position[2] = {
#include "shared/golem_clips/golem_motion_slam_start_joint_11_position.inc"
};
static NJS_MKEY_A golem_motion_slam_start_joint_11_angles[23] = {
#include "shared/golem_clips/golem_motion_slam_start_joint_11_angles.inc"
};
static NJS_MKEY_F golem_motion_slam_start_joint_12_position[2] = {
#include "shared/golem_clips/golem_motion_slam_start_joint_12_position.inc"
};
static NJS_MKEY_A golem_motion_slam_start_joint_12_angles[18] = {
#include "shared/golem_clips/golem_motion_slam_start_joint_12_angles.inc"
};
static NJS_MKEY_F golem_motion_slam_start_joint_13_position[2] = {
#include "shared/golem_clips/golem_motion_slam_start_joint_13_position.inc"
};
static NJS_MKEY_A golem_motion_slam_start_joint_13_angles[30] = {
#include "shared/golem_clips/golem_motion_slam_start_joint_13_angles.inc"
};
static NJS_MKEY_F golem_motion_slam_start_joint_14_position[2] = {
#include "shared/golem_clips/golem_motion_slam_start_joint_14_position.inc"
};
static NJS_MKEY_A golem_motion_slam_start_joint_14_angles[30] = {
#include "shared/golem_clips/golem_motion_slam_start_joint_14_angles.inc"
};
static NJS_MKEY_F golem_motion_slam_start_joint_15_position[2] = {
#include "shared/golem_clips/golem_motion_slam_start_joint_15_position.inc"
};
static NJS_MKEY_A golem_motion_slam_start_joint_15_angles[30] = {
#include "shared/golem_clips/golem_motion_slam_start_joint_15_angles.inc"
};
static NJS_MKEY_F golem_motion_slam_start_joint_16_position[2] = {
#include "shared/golem_clips/golem_motion_slam_start_joint_16_position.inc"
};
static NJS_MKEY_A golem_motion_slam_start_joint_16_angles[30] = {
#include "shared/golem_clips/golem_motion_slam_start_joint_16_angles.inc"
};
static NJS_MKEY_F golem_motion_slam_start_joint_17_position[2] = {
#include "shared/golem_clips/golem_motion_slam_start_joint_17_position.inc"
};
static NJS_MKEY_A golem_motion_slam_start_joint_17_angles[30] = {
#include "shared/golem_clips/golem_motion_slam_start_joint_17_angles.inc"
};
static NJS_MKEY_F golem_motion_slam_start_joint_18_position[2] = {
#include "shared/golem_clips/golem_motion_slam_start_joint_18_position.inc"
};
static NJS_MKEY_A golem_motion_slam_start_joint_18_angles[30] = {
#include "shared/golem_clips/golem_motion_slam_start_joint_18_angles.inc"
};
static NJS_MKEY_F golem_motion_slam_start_joint_19_position[2] = {
#include "shared/golem_clips/golem_motion_slam_start_joint_19_position.inc"
};
static NJS_MKEY_A golem_motion_slam_start_joint_19_angles[30] = {
#include "shared/golem_clips/golem_motion_slam_start_joint_19_angles.inc"
};
static NJS_MKEY_F golem_motion_slam_start_joint_20_position[2] = {
#include "shared/golem_clips/golem_motion_slam_start_joint_20_position.inc"
};
static NJS_MKEY_A golem_motion_slam_start_joint_20_angles[15] = {
#include "shared/golem_clips/golem_motion_slam_start_joint_20_angles.inc"
};
static NJS_MDATA3 golem_motion_slam_start_tracks[21] = {
#include "shared/golem_clips/golem_motion_slam_start_tracks.inc"
};
NJS_MOTION golem_motion_slam_start = {
#include "shared/golem_clips/golem_motion_slam_start.inc"
};
static NJS_MKEY_F golem_motion_slam_blows_whole_joint_0_position[2] = {
#include "shared/golem_clips/golem_motion_slam_blows_whole_joint_0_position.inc"
};
static NJS_MKEY_A golem_motion_slam_blows_whole_joint_0_angles[2] = {
#include "shared/golem_clips/golem_motion_slam_blows_whole_joint_0_angles.inc"
};
static NJS_MKEY_F golem_motion_slam_blows_whole_joint_1_position[2] = {
#include "shared/golem_clips/golem_motion_slam_blows_whole_joint_1_position.inc"
};
static NJS_MKEY_A golem_motion_slam_blows_whole_joint_1_angles[2] = {
#include "shared/golem_clips/golem_motion_slam_blows_whole_joint_1_angles.inc"
};
static NJS_MKEY_F golem_motion_slam_blows_whole_joint_2_position[2] = {
#include "shared/golem_clips/golem_motion_slam_blows_whole_joint_2_position.inc"
};
static NJS_MKEY_A golem_motion_slam_blows_whole_joint_2_angles[2] = {
#include "shared/golem_clips/golem_motion_slam_blows_whole_joint_2_angles.inc"
};
static NJS_MKEY_F golem_motion_slam_blows_whole_joint_3_position[2] = {
#include "shared/golem_clips/golem_motion_slam_blows_whole_joint_3_position.inc"
};
static NJS_MKEY_A golem_motion_slam_blows_whole_joint_3_angles[2] = {
#include "shared/golem_clips/golem_motion_slam_blows_whole_joint_3_angles.inc"
};
static NJS_MKEY_F golem_motion_slam_blows_whole_joint_4_position[2] = {
#include "shared/golem_clips/golem_motion_slam_blows_whole_joint_4_position.inc"
};
static NJS_MKEY_A golem_motion_slam_blows_whole_joint_4_angles[2] = {
#include "shared/golem_clips/golem_motion_slam_blows_whole_joint_4_angles.inc"
};
static NJS_MKEY_F golem_motion_slam_blows_whole_joint_5_position[2] = {
#include "shared/golem_clips/golem_motion_slam_blows_whole_joint_5_position.inc"
};
static NJS_MKEY_A golem_motion_slam_blows_whole_joint_5_angles[29] = {
#include "shared/golem_clips/golem_motion_slam_blows_whole_joint_5_angles.inc"
};
static NJS_MKEY_F golem_motion_slam_blows_whole_joint_6_position[2] = {
#include "shared/golem_clips/golem_motion_slam_blows_whole_joint_6_position.inc"
};
static NJS_MKEY_A golem_motion_slam_blows_whole_joint_6_angles[30] = {
#include "shared/golem_clips/golem_motion_slam_blows_whole_joint_6_angles.inc"
};
static NJS_MKEY_F golem_motion_slam_blows_whole_joint_7_position[2] = {
#include "shared/golem_clips/golem_motion_slam_blows_whole_joint_7_position.inc"
};
static NJS_MKEY_A golem_motion_slam_blows_whole_joint_7_angles[30] = {
#include "shared/golem_clips/golem_motion_slam_blows_whole_joint_7_angles.inc"
};
static NJS_MKEY_F golem_motion_slam_blows_whole_joint_8_position[2] = {
#include "shared/golem_clips/golem_motion_slam_blows_whole_joint_8_position.inc"
};
static NJS_MKEY_A golem_motion_slam_blows_whole_joint_8_angles[30] = {
#include "shared/golem_clips/golem_motion_slam_blows_whole_joint_8_angles.inc"
};
static NJS_MKEY_F golem_motion_slam_blows_whole_joint_9_position[2] = {
#include "shared/golem_clips/golem_motion_slam_blows_whole_joint_9_position.inc"
};
static NJS_MKEY_A golem_motion_slam_blows_whole_joint_9_angles[30] = {
#include "shared/golem_clips/golem_motion_slam_blows_whole_joint_9_angles.inc"
};
static NJS_MKEY_F golem_motion_slam_blows_whole_joint_10_position[2] = {
#include "shared/golem_clips/golem_motion_slam_blows_whole_joint_10_position.inc"
};
static NJS_MKEY_A golem_motion_slam_blows_whole_joint_10_angles[30] = {
#include "shared/golem_clips/golem_motion_slam_blows_whole_joint_10_angles.inc"
};
static NJS_MKEY_F golem_motion_slam_blows_whole_joint_11_position[2] = {
#include "shared/golem_clips/golem_motion_slam_blows_whole_joint_11_position.inc"
};
static NJS_MKEY_A golem_motion_slam_blows_whole_joint_11_angles[30] = {
#include "shared/golem_clips/golem_motion_slam_blows_whole_joint_11_angles.inc"
};
static NJS_MKEY_F golem_motion_slam_blows_whole_joint_12_position[2] = {
#include "shared/golem_clips/golem_motion_slam_blows_whole_joint_12_position.inc"
};
static NJS_MKEY_A golem_motion_slam_blows_whole_joint_12_angles[30] = {
#include "shared/golem_clips/golem_motion_slam_blows_whole_joint_12_angles.inc"
};
static NJS_MKEY_F golem_motion_slam_blows_whole_joint_13_position[2] = {
#include "shared/golem_clips/golem_motion_slam_blows_whole_joint_13_position.inc"
};
static NJS_MKEY_A golem_motion_slam_blows_whole_joint_13_angles[30] = {
#include "shared/golem_clips/golem_motion_slam_blows_whole_joint_13_angles.inc"
};
static NJS_MKEY_F golem_motion_slam_blows_whole_joint_14_position[2] = {
#include "shared/golem_clips/golem_motion_slam_blows_whole_joint_14_position.inc"
};
static NJS_MKEY_A golem_motion_slam_blows_whole_joint_14_angles[30] = {
#include "shared/golem_clips/golem_motion_slam_blows_whole_joint_14_angles.inc"
};
static NJS_MKEY_F golem_motion_slam_blows_whole_joint_15_position[2] = {
#include "shared/golem_clips/golem_motion_slam_blows_whole_joint_15_position.inc"
};
static NJS_MKEY_A golem_motion_slam_blows_whole_joint_15_angles[30] = {
#include "shared/golem_clips/golem_motion_slam_blows_whole_joint_15_angles.inc"
};
static NJS_MKEY_F golem_motion_slam_blows_whole_joint_16_position[2] = {
#include "shared/golem_clips/golem_motion_slam_blows_whole_joint_16_position.inc"
};
static NJS_MKEY_A golem_motion_slam_blows_whole_joint_16_angles[30] = {
#include "shared/golem_clips/golem_motion_slam_blows_whole_joint_16_angles.inc"
};
static NJS_MKEY_F golem_motion_slam_blows_whole_joint_17_position[2] = {
#include "shared/golem_clips/golem_motion_slam_blows_whole_joint_17_position.inc"
};
static NJS_MKEY_A golem_motion_slam_blows_whole_joint_17_angles[30] = {
#include "shared/golem_clips/golem_motion_slam_blows_whole_joint_17_angles.inc"
};
static NJS_MKEY_F golem_motion_slam_blows_whole_joint_18_position[2] = {
#include "shared/golem_clips/golem_motion_slam_blows_whole_joint_18_position.inc"
};
static NJS_MKEY_A golem_motion_slam_blows_whole_joint_18_angles[30] = {
#include "shared/golem_clips/golem_motion_slam_blows_whole_joint_18_angles.inc"
};
static NJS_MKEY_F golem_motion_slam_blows_whole_joint_19_position[2] = {
#include "shared/golem_clips/golem_motion_slam_blows_whole_joint_19_position.inc"
};
static NJS_MKEY_A golem_motion_slam_blows_whole_joint_19_angles[30] = {
#include "shared/golem_clips/golem_motion_slam_blows_whole_joint_19_angles.inc"
};
static NJS_MKEY_F golem_motion_slam_blows_whole_joint_20_position[2] = {
#include "shared/golem_clips/golem_motion_slam_blows_whole_joint_20_position.inc"
};
static NJS_MKEY_A golem_motion_slam_blows_whole_joint_20_angles[30] = {
#include "shared/golem_clips/golem_motion_slam_blows_whole_joint_20_angles.inc"
};
static NJS_MDATA3 golem_motion_slam_blows_whole_tracks[21] = {
#include "shared/golem_clips/golem_motion_slam_blows_whole_tracks.inc"
};
NJS_MOTION golem_motion_slam_blows_whole = {
#include "shared/golem_clips/golem_motion_slam_blows_whole.inc"
};
static NJS_MKEY_F golem_motion_slam_slam_whole_joint_0_position[25] = {
#include "shared/golem_clips/golem_motion_slam_slam_whole_joint_0_position.inc"
};
static NJS_MKEY_A golem_motion_slam_slam_whole_joint_0_angles[2] = {
#include "shared/golem_clips/golem_motion_slam_slam_whole_joint_0_angles.inc"
};
static NJS_MKEY_F golem_motion_slam_slam_whole_joint_1_position[2] = {
#include "shared/golem_clips/golem_motion_slam_slam_whole_joint_1_position.inc"
};
static NJS_MKEY_A golem_motion_slam_slam_whole_joint_1_angles[2] = {
#include "shared/golem_clips/golem_motion_slam_slam_whole_joint_1_angles.inc"
};
static NJS_MKEY_F golem_motion_slam_slam_whole_joint_2_position[2] = {
#include "shared/golem_clips/golem_motion_slam_slam_whole_joint_2_position.inc"
};
static NJS_MKEY_A golem_motion_slam_slam_whole_joint_2_angles[2] = {
#include "shared/golem_clips/golem_motion_slam_slam_whole_joint_2_angles.inc"
};
static NJS_MKEY_F golem_motion_slam_slam_whole_joint_3_position[2] = {
#include "shared/golem_clips/golem_motion_slam_slam_whole_joint_3_position.inc"
};
static NJS_MKEY_A golem_motion_slam_slam_whole_joint_3_angles[2] = {
#include "shared/golem_clips/golem_motion_slam_slam_whole_joint_3_angles.inc"
};
static NJS_MKEY_F golem_motion_slam_slam_whole_joint_4_position[2] = {
#include "shared/golem_clips/golem_motion_slam_slam_whole_joint_4_position.inc"
};
static NJS_MKEY_A golem_motion_slam_slam_whole_joint_4_angles[2] = {
#include "shared/golem_clips/golem_motion_slam_slam_whole_joint_4_angles.inc"
};
static NJS_MKEY_F golem_motion_slam_slam_whole_joint_5_position[2] = {
#include "shared/golem_clips/golem_motion_slam_slam_whole_joint_5_position.inc"
};
static NJS_MKEY_A golem_motion_slam_slam_whole_joint_5_angles[25] = {
#include "shared/golem_clips/golem_motion_slam_slam_whole_joint_5_angles.inc"
};
static NJS_MKEY_F golem_motion_slam_slam_whole_joint_6_position[2] = {
#include "shared/golem_clips/golem_motion_slam_slam_whole_joint_6_position.inc"
};
static NJS_MKEY_A golem_motion_slam_slam_whole_joint_6_angles[25] = {
#include "shared/golem_clips/golem_motion_slam_slam_whole_joint_6_angles.inc"
};
static NJS_MKEY_F golem_motion_slam_slam_whole_joint_7_position[2] = {
#include "shared/golem_clips/golem_motion_slam_slam_whole_joint_7_position.inc"
};
static NJS_MKEY_A golem_motion_slam_slam_whole_joint_7_angles[25] = {
#include "shared/golem_clips/golem_motion_slam_slam_whole_joint_7_angles.inc"
};
static NJS_MKEY_F golem_motion_slam_slam_whole_joint_8_position[2] = {
#include "shared/golem_clips/golem_motion_slam_slam_whole_joint_8_position.inc"
};
static NJS_MKEY_A golem_motion_slam_slam_whole_joint_8_angles[25] = {
#include "shared/golem_clips/golem_motion_slam_slam_whole_joint_8_angles.inc"
};
static NJS_MKEY_F golem_motion_slam_slam_whole_joint_9_position[2] = {
#include "shared/golem_clips/golem_motion_slam_slam_whole_joint_9_position.inc"
};
static NJS_MKEY_A golem_motion_slam_slam_whole_joint_9_angles[25] = {
#include "shared/golem_clips/golem_motion_slam_slam_whole_joint_9_angles.inc"
};
static NJS_MKEY_F golem_motion_slam_slam_whole_joint_10_position[2] = {
#include "shared/golem_clips/golem_motion_slam_slam_whole_joint_10_position.inc"
};
static NJS_MKEY_A golem_motion_slam_slam_whole_joint_10_angles[25] = {
#include "shared/golem_clips/golem_motion_slam_slam_whole_joint_10_angles.inc"
};
static NJS_MKEY_F golem_motion_slam_slam_whole_joint_11_position[2] = {
#include "shared/golem_clips/golem_motion_slam_slam_whole_joint_11_position.inc"
};
static NJS_MKEY_A golem_motion_slam_slam_whole_joint_11_angles[25] = {
#include "shared/golem_clips/golem_motion_slam_slam_whole_joint_11_angles.inc"
};
static NJS_MKEY_F golem_motion_slam_slam_whole_joint_12_position[2] = {
#include "shared/golem_clips/golem_motion_slam_slam_whole_joint_12_position.inc"
};
static NJS_MKEY_A golem_motion_slam_slam_whole_joint_12_angles[25] = {
#include "shared/golem_clips/golem_motion_slam_slam_whole_joint_12_angles.inc"
};
static NJS_MKEY_F golem_motion_slam_slam_whole_joint_13_position[2] = {
#include "shared/golem_clips/golem_motion_slam_slam_whole_joint_13_position.inc"
};
static NJS_MKEY_A golem_motion_slam_slam_whole_joint_13_angles[25] = {
#include "shared/golem_clips/golem_motion_slam_slam_whole_joint_13_angles.inc"
};
static NJS_MKEY_F golem_motion_slam_slam_whole_joint_14_position[2] = {
#include "shared/golem_clips/golem_motion_slam_slam_whole_joint_14_position.inc"
};
static NJS_MKEY_A golem_motion_slam_slam_whole_joint_14_angles[25] = {
#include "shared/golem_clips/golem_motion_slam_slam_whole_joint_14_angles.inc"
};
static NJS_MKEY_F golem_motion_slam_slam_whole_joint_15_position[2] = {
#include "shared/golem_clips/golem_motion_slam_slam_whole_joint_15_position.inc"
};
static NJS_MKEY_A golem_motion_slam_slam_whole_joint_15_angles[25] = {
#include "shared/golem_clips/golem_motion_slam_slam_whole_joint_15_angles.inc"
};
static NJS_MKEY_F golem_motion_slam_slam_whole_joint_16_position[2] = {
#include "shared/golem_clips/golem_motion_slam_slam_whole_joint_16_position.inc"
};
static NJS_MKEY_A golem_motion_slam_slam_whole_joint_16_angles[25] = {
#include "shared/golem_clips/golem_motion_slam_slam_whole_joint_16_angles.inc"
};
static NJS_MKEY_F golem_motion_slam_slam_whole_joint_17_position[2] = {
#include "shared/golem_clips/golem_motion_slam_slam_whole_joint_17_position.inc"
};
static NJS_MKEY_A golem_motion_slam_slam_whole_joint_17_angles[18] = {
#include "shared/golem_clips/golem_motion_slam_slam_whole_joint_17_angles.inc"
};
static NJS_MKEY_F golem_motion_slam_slam_whole_joint_18_position[2] = {
#include "shared/golem_clips/golem_motion_slam_slam_whole_joint_18_position.inc"
};
static NJS_MKEY_A golem_motion_slam_slam_whole_joint_18_angles[25] = {
#include "shared/golem_clips/golem_motion_slam_slam_whole_joint_18_angles.inc"
};
static NJS_MKEY_F golem_motion_slam_slam_whole_joint_19_position[2] = {
#include "shared/golem_clips/golem_motion_slam_slam_whole_joint_19_position.inc"
};
static NJS_MKEY_A golem_motion_slam_slam_whole_joint_19_angles[21] = {
#include "shared/golem_clips/golem_motion_slam_slam_whole_joint_19_angles.inc"
};
static NJS_MKEY_F golem_motion_slam_slam_whole_joint_20_position[2] = {
#include "shared/golem_clips/golem_motion_slam_slam_whole_joint_20_position.inc"
};
static NJS_MKEY_A golem_motion_slam_slam_whole_joint_20_angles[25] = {
#include "shared/golem_clips/golem_motion_slam_slam_whole_joint_20_angles.inc"
};
static NJS_MDATA3 golem_motion_slam_slam_whole_tracks[21] = {
#include "shared/golem_clips/golem_motion_slam_slam_whole_tracks.inc"
};
NJS_MOTION golem_motion_slam_slam_whole = {
#include "shared/golem_clips/golem_motion_slam_slam_whole.inc"
};
static NJS_MKEY_F golem_motion_slam_end_whole_joint_0_position[40] = {
#include "shared/golem_clips/golem_motion_slam_end_whole_joint_0_position.inc"
};
static NJS_MKEY_A golem_motion_slam_end_whole_joint_0_angles[2] = {
#include "shared/golem_clips/golem_motion_slam_end_whole_joint_0_angles.inc"
};
static NJS_MKEY_F golem_motion_slam_end_whole_joint_1_position[2] = {
#include "shared/golem_clips/golem_motion_slam_end_whole_joint_1_position.inc"
};
static NJS_MKEY_A golem_motion_slam_end_whole_joint_1_angles[2] = {
#include "shared/golem_clips/golem_motion_slam_end_whole_joint_1_angles.inc"
};
static NJS_MKEY_F golem_motion_slam_end_whole_joint_2_position[2] = {
#include "shared/golem_clips/golem_motion_slam_end_whole_joint_2_position.inc"
};
static NJS_MKEY_A golem_motion_slam_end_whole_joint_2_angles[2] = {
#include "shared/golem_clips/golem_motion_slam_end_whole_joint_2_angles.inc"
};
static NJS_MKEY_F golem_motion_slam_end_whole_joint_3_position[2] = {
#include "shared/golem_clips/golem_motion_slam_end_whole_joint_3_position.inc"
};
static NJS_MKEY_A golem_motion_slam_end_whole_joint_3_angles[2] = {
#include "shared/golem_clips/golem_motion_slam_end_whole_joint_3_angles.inc"
};
static NJS_MKEY_F golem_motion_slam_end_whole_joint_4_position[2] = {
#include "shared/golem_clips/golem_motion_slam_end_whole_joint_4_position.inc"
};
static NJS_MKEY_A golem_motion_slam_end_whole_joint_4_angles[2] = {
#include "shared/golem_clips/golem_motion_slam_end_whole_joint_4_angles.inc"
};
static NJS_MKEY_F golem_motion_slam_end_whole_joint_5_position[2] = {
#include "shared/golem_clips/golem_motion_slam_end_whole_joint_5_position.inc"
};
static NJS_MKEY_A golem_motion_slam_end_whole_joint_5_angles[40] = {
#include "shared/golem_clips/golem_motion_slam_end_whole_joint_5_angles.inc"
};
static NJS_MKEY_F golem_motion_slam_end_whole_joint_6_position[2] = {
#include "shared/golem_clips/golem_motion_slam_end_whole_joint_6_position.inc"
};
static NJS_MKEY_A golem_motion_slam_end_whole_joint_6_angles[40] = {
#include "shared/golem_clips/golem_motion_slam_end_whole_joint_6_angles.inc"
};
static NJS_MKEY_F golem_motion_slam_end_whole_joint_7_position[2] = {
#include "shared/golem_clips/golem_motion_slam_end_whole_joint_7_position.inc"
};
static NJS_MKEY_A golem_motion_slam_end_whole_joint_7_angles[40] = {
#include "shared/golem_clips/golem_motion_slam_end_whole_joint_7_angles.inc"
};
static NJS_MKEY_F golem_motion_slam_end_whole_joint_8_position[2] = {
#include "shared/golem_clips/golem_motion_slam_end_whole_joint_8_position.inc"
};
static NJS_MKEY_A golem_motion_slam_end_whole_joint_8_angles[40] = {
#include "shared/golem_clips/golem_motion_slam_end_whole_joint_8_angles.inc"
};
static NJS_MKEY_F golem_motion_slam_end_whole_joint_9_position[2] = {
#include "shared/golem_clips/golem_motion_slam_end_whole_joint_9_position.inc"
};
static NJS_MKEY_A golem_motion_slam_end_whole_joint_9_angles[40] = {
#include "shared/golem_clips/golem_motion_slam_end_whole_joint_9_angles.inc"
};
static NJS_MKEY_F golem_motion_slam_end_whole_joint_10_position[2] = {
#include "shared/golem_clips/golem_motion_slam_end_whole_joint_10_position.inc"
};
static NJS_MKEY_A golem_motion_slam_end_whole_joint_10_angles[40] = {
#include "shared/golem_clips/golem_motion_slam_end_whole_joint_10_angles.inc"
};
static NJS_MKEY_F golem_motion_slam_end_whole_joint_11_position[2] = {
#include "shared/golem_clips/golem_motion_slam_end_whole_joint_11_position.inc"
};
static NJS_MKEY_A golem_motion_slam_end_whole_joint_11_angles[40] = {
#include "shared/golem_clips/golem_motion_slam_end_whole_joint_11_angles.inc"
};
static NJS_MKEY_F golem_motion_slam_end_whole_joint_12_position[2] = {
#include "shared/golem_clips/golem_motion_slam_end_whole_joint_12_position.inc"
};
static NJS_MKEY_A golem_motion_slam_end_whole_joint_12_angles[39] = {
#include "shared/golem_clips/golem_motion_slam_end_whole_joint_12_angles.inc"
};
static NJS_MKEY_F golem_motion_slam_end_whole_joint_13_position[2] = {
#include "shared/golem_clips/golem_motion_slam_end_whole_joint_13_position.inc"
};
static NJS_MKEY_A golem_motion_slam_end_whole_joint_13_angles[40] = {
#include "shared/golem_clips/golem_motion_slam_end_whole_joint_13_angles.inc"
};
static NJS_MKEY_F golem_motion_slam_end_whole_joint_14_position[2] = {
#include "shared/golem_clips/golem_motion_slam_end_whole_joint_14_position.inc"
};
static NJS_MKEY_A golem_motion_slam_end_whole_joint_14_angles[40] = {
#include "shared/golem_clips/golem_motion_slam_end_whole_joint_14_angles.inc"
};
static NJS_MKEY_F golem_motion_slam_end_whole_joint_15_position[2] = {
#include "shared/golem_clips/golem_motion_slam_end_whole_joint_15_position.inc"
};
static NJS_MKEY_A golem_motion_slam_end_whole_joint_15_angles[40] = {
#include "shared/golem_clips/golem_motion_slam_end_whole_joint_15_angles.inc"
};
static NJS_MKEY_F golem_motion_slam_end_whole_joint_16_position[2] = {
#include "shared/golem_clips/golem_motion_slam_end_whole_joint_16_position.inc"
};
static NJS_MKEY_A golem_motion_slam_end_whole_joint_16_angles[39] = {
#include "shared/golem_clips/golem_motion_slam_end_whole_joint_16_angles.inc"
};
static NJS_MKEY_F golem_motion_slam_end_whole_joint_17_position[2] = {
#include "shared/golem_clips/golem_motion_slam_end_whole_joint_17_position.inc"
};
static NJS_MKEY_A golem_motion_slam_end_whole_joint_17_angles[40] = {
#include "shared/golem_clips/golem_motion_slam_end_whole_joint_17_angles.inc"
};
static NJS_MKEY_F golem_motion_slam_end_whole_joint_18_position[2] = {
#include "shared/golem_clips/golem_motion_slam_end_whole_joint_18_position.inc"
};
static NJS_MKEY_A golem_motion_slam_end_whole_joint_18_angles[40] = {
#include "shared/golem_clips/golem_motion_slam_end_whole_joint_18_angles.inc"
};
static NJS_MKEY_F golem_motion_slam_end_whole_joint_19_position[2] = {
#include "shared/golem_clips/golem_motion_slam_end_whole_joint_19_position.inc"
};
static NJS_MKEY_A golem_motion_slam_end_whole_joint_19_angles[40] = {
#include "shared/golem_clips/golem_motion_slam_end_whole_joint_19_angles.inc"
};
static NJS_MKEY_F golem_motion_slam_end_whole_joint_20_position[2] = {
#include "shared/golem_clips/golem_motion_slam_end_whole_joint_20_position.inc"
};
static NJS_MKEY_A golem_motion_slam_end_whole_joint_20_angles[40] = {
#include "shared/golem_clips/golem_motion_slam_end_whole_joint_20_angles.inc"
};
static NJS_MDATA3 golem_motion_slam_end_whole_tracks[21] = {
#include "shared/golem_clips/golem_motion_slam_end_whole_tracks.inc"
};
NJS_MOTION golem_motion_slam_end_whole = {
#include "shared/golem_clips/golem_motion_slam_end_whole.inc"
};
static NJS_MKEY_F golem_motion_slam_blows_sunk_joint_0_position[29] = {
#include "shared/golem_clips/golem_motion_slam_blows_sunk_joint_0_position.inc"
};
static NJS_MKEY_A golem_motion_slam_blows_sunk_joint_0_angles[2] = {
#include "shared/golem_clips/golem_motion_slam_blows_sunk_joint_0_angles.inc"
};
static NJS_MKEY_F golem_motion_slam_blows_sunk_joint_1_position[2] = {
#include "shared/golem_clips/golem_motion_slam_blows_sunk_joint_1_position.inc"
};
static NJS_MKEY_A golem_motion_slam_blows_sunk_joint_1_angles[2] = {
#include "shared/golem_clips/golem_motion_slam_blows_sunk_joint_1_angles.inc"
};
static NJS_MKEY_F golem_motion_slam_blows_sunk_joint_2_position[2] = {
#include "shared/golem_clips/golem_motion_slam_blows_sunk_joint_2_position.inc"
};
static NJS_MKEY_A golem_motion_slam_blows_sunk_joint_2_angles[2] = {
#include "shared/golem_clips/golem_motion_slam_blows_sunk_joint_2_angles.inc"
};
static NJS_MKEY_F golem_motion_slam_blows_sunk_joint_3_position[2] = {
#include "shared/golem_clips/golem_motion_slam_blows_sunk_joint_3_position.inc"
};
static NJS_MKEY_A golem_motion_slam_blows_sunk_joint_3_angles[2] = {
#include "shared/golem_clips/golem_motion_slam_blows_sunk_joint_3_angles.inc"
};
static NJS_MKEY_F golem_motion_slam_blows_sunk_joint_4_position[2] = {
#include "shared/golem_clips/golem_motion_slam_blows_sunk_joint_4_position.inc"
};
static NJS_MKEY_A golem_motion_slam_blows_sunk_joint_4_angles[2] = {
#include "shared/golem_clips/golem_motion_slam_blows_sunk_joint_4_angles.inc"
};
static NJS_MKEY_F golem_motion_slam_blows_sunk_joint_5_position[2] = {
#include "shared/golem_clips/golem_motion_slam_blows_sunk_joint_5_position.inc"
};
static NJS_MKEY_A golem_motion_slam_blows_sunk_joint_5_angles[29] = {
#include "shared/golem_clips/golem_motion_slam_blows_sunk_joint_5_angles.inc"
};
static NJS_MKEY_F golem_motion_slam_blows_sunk_joint_6_position[2] = {
#include "shared/golem_clips/golem_motion_slam_blows_sunk_joint_6_position.inc"
};
static NJS_MKEY_A golem_motion_slam_blows_sunk_joint_6_angles[30] = {
#include "shared/golem_clips/golem_motion_slam_blows_sunk_joint_6_angles.inc"
};
static NJS_MKEY_F golem_motion_slam_blows_sunk_joint_7_position[2] = {
#include "shared/golem_clips/golem_motion_slam_blows_sunk_joint_7_position.inc"
};
static NJS_MKEY_A golem_motion_slam_blows_sunk_joint_7_angles[30] = {
#include "shared/golem_clips/golem_motion_slam_blows_sunk_joint_7_angles.inc"
};
static NJS_MKEY_F golem_motion_slam_blows_sunk_joint_8_position[2] = {
#include "shared/golem_clips/golem_motion_slam_blows_sunk_joint_8_position.inc"
};
static NJS_MKEY_A golem_motion_slam_blows_sunk_joint_8_angles[30] = {
#include "shared/golem_clips/golem_motion_slam_blows_sunk_joint_8_angles.inc"
};
static NJS_MKEY_F golem_motion_slam_blows_sunk_joint_9_position[2] = {
#include "shared/golem_clips/golem_motion_slam_blows_sunk_joint_9_position.inc"
};
static NJS_MKEY_A golem_motion_slam_blows_sunk_joint_9_angles[30] = {
#include "shared/golem_clips/golem_motion_slam_blows_sunk_joint_9_angles.inc"
};
static NJS_MKEY_F golem_motion_slam_blows_sunk_joint_10_position[2] = {
#include "shared/golem_clips/golem_motion_slam_blows_sunk_joint_10_position.inc"
};
static NJS_MKEY_A golem_motion_slam_blows_sunk_joint_10_angles[30] = {
#include "shared/golem_clips/golem_motion_slam_blows_sunk_joint_10_angles.inc"
};
static NJS_MKEY_F golem_motion_slam_blows_sunk_joint_11_position[2] = {
#include "shared/golem_clips/golem_motion_slam_blows_sunk_joint_11_position.inc"
};
static NJS_MKEY_A golem_motion_slam_blows_sunk_joint_11_angles[30] = {
#include "shared/golem_clips/golem_motion_slam_blows_sunk_joint_11_angles.inc"
};
static NJS_MKEY_F golem_motion_slam_blows_sunk_joint_12_position[2] = {
#include "shared/golem_clips/golem_motion_slam_blows_sunk_joint_12_position.inc"
};
static NJS_MKEY_A golem_motion_slam_blows_sunk_joint_12_angles[30] = {
#include "shared/golem_clips/golem_motion_slam_blows_sunk_joint_12_angles.inc"
};
static NJS_MKEY_F golem_motion_slam_blows_sunk_joint_13_position[2] = {
#include "shared/golem_clips/golem_motion_slam_blows_sunk_joint_13_position.inc"
};
static NJS_MKEY_A golem_motion_slam_blows_sunk_joint_13_angles[30] = {
#include "shared/golem_clips/golem_motion_slam_blows_sunk_joint_13_angles.inc"
};
static NJS_MKEY_F golem_motion_slam_blows_sunk_joint_14_position[2] = {
#include "shared/golem_clips/golem_motion_slam_blows_sunk_joint_14_position.inc"
};
static NJS_MKEY_A golem_motion_slam_blows_sunk_joint_14_angles[30] = {
#include "shared/golem_clips/golem_motion_slam_blows_sunk_joint_14_angles.inc"
};
static NJS_MKEY_F golem_motion_slam_blows_sunk_joint_15_position[2] = {
#include "shared/golem_clips/golem_motion_slam_blows_sunk_joint_15_position.inc"
};
static NJS_MKEY_A golem_motion_slam_blows_sunk_joint_15_angles[30] = {
#include "shared/golem_clips/golem_motion_slam_blows_sunk_joint_15_angles.inc"
};
static NJS_MKEY_F golem_motion_slam_blows_sunk_joint_16_position[2] = {
#include "shared/golem_clips/golem_motion_slam_blows_sunk_joint_16_position.inc"
};
static NJS_MKEY_A golem_motion_slam_blows_sunk_joint_16_angles[30] = {
#include "shared/golem_clips/golem_motion_slam_blows_sunk_joint_16_angles.inc"
};
static NJS_MKEY_F golem_motion_slam_blows_sunk_joint_17_position[2] = {
#include "shared/golem_clips/golem_motion_slam_blows_sunk_joint_17_position.inc"
};
static NJS_MKEY_A golem_motion_slam_blows_sunk_joint_17_angles[30] = {
#include "shared/golem_clips/golem_motion_slam_blows_sunk_joint_17_angles.inc"
};
static NJS_MKEY_F golem_motion_slam_blows_sunk_joint_18_position[2] = {
#include "shared/golem_clips/golem_motion_slam_blows_sunk_joint_18_position.inc"
};
static NJS_MKEY_A golem_motion_slam_blows_sunk_joint_18_angles[30] = {
#include "shared/golem_clips/golem_motion_slam_blows_sunk_joint_18_angles.inc"
};
static NJS_MKEY_F golem_motion_slam_blows_sunk_joint_19_position[2] = {
#include "shared/golem_clips/golem_motion_slam_blows_sunk_joint_19_position.inc"
};
static NJS_MKEY_A golem_motion_slam_blows_sunk_joint_19_angles[30] = {
#include "shared/golem_clips/golem_motion_slam_blows_sunk_joint_19_angles.inc"
};
static NJS_MKEY_F golem_motion_slam_blows_sunk_joint_20_position[2] = {
#include "shared/golem_clips/golem_motion_slam_blows_sunk_joint_20_position.inc"
};
static NJS_MKEY_A golem_motion_slam_blows_sunk_joint_20_angles[30] = {
#include "shared/golem_clips/golem_motion_slam_blows_sunk_joint_20_angles.inc"
};
static NJS_MDATA3 golem_motion_slam_blows_sunk_tracks[21] = {
#include "shared/golem_clips/golem_motion_slam_blows_sunk_tracks.inc"
};
NJS_MOTION golem_motion_slam_blows_sunk = {
#include "shared/golem_clips/golem_motion_slam_blows_sunk.inc"
};
static NJS_MKEY_F golem_motion_slam_slam_sunk_joint_0_position[25] = {
#include "shared/golem_clips/golem_motion_slam_slam_sunk_joint_0_position.inc"
};
static NJS_MKEY_A golem_motion_slam_slam_sunk_joint_0_angles[2] = {
#include "shared/golem_clips/golem_motion_slam_slam_sunk_joint_0_angles.inc"
};
static NJS_MKEY_F golem_motion_slam_slam_sunk_joint_1_position[2] = {
#include "shared/golem_clips/golem_motion_slam_slam_sunk_joint_1_position.inc"
};
static NJS_MKEY_A golem_motion_slam_slam_sunk_joint_1_angles[2] = {
#include "shared/golem_clips/golem_motion_slam_slam_sunk_joint_1_angles.inc"
};
static NJS_MKEY_F golem_motion_slam_slam_sunk_joint_2_position[2] = {
#include "shared/golem_clips/golem_motion_slam_slam_sunk_joint_2_position.inc"
};
static NJS_MKEY_A golem_motion_slam_slam_sunk_joint_2_angles[2] = {
#include "shared/golem_clips/golem_motion_slam_slam_sunk_joint_2_angles.inc"
};
static NJS_MKEY_F golem_motion_slam_slam_sunk_joint_3_position[2] = {
#include "shared/golem_clips/golem_motion_slam_slam_sunk_joint_3_position.inc"
};
static NJS_MKEY_A golem_motion_slam_slam_sunk_joint_3_angles[2] = {
#include "shared/golem_clips/golem_motion_slam_slam_sunk_joint_3_angles.inc"
};
static NJS_MKEY_F golem_motion_slam_slam_sunk_joint_4_position[2] = {
#include "shared/golem_clips/golem_motion_slam_slam_sunk_joint_4_position.inc"
};
static NJS_MKEY_A golem_motion_slam_slam_sunk_joint_4_angles[2] = {
#include "shared/golem_clips/golem_motion_slam_slam_sunk_joint_4_angles.inc"
};
static NJS_MKEY_F golem_motion_slam_slam_sunk_joint_5_position[2] = {
#include "shared/golem_clips/golem_motion_slam_slam_sunk_joint_5_position.inc"
};
static NJS_MKEY_A golem_motion_slam_slam_sunk_joint_5_angles[25] = {
#include "shared/golem_clips/golem_motion_slam_slam_sunk_joint_5_angles.inc"
};
static NJS_MKEY_F golem_motion_slam_slam_sunk_joint_6_position[2] = {
#include "shared/golem_clips/golem_motion_slam_slam_sunk_joint_6_position.inc"
};
static NJS_MKEY_A golem_motion_slam_slam_sunk_joint_6_angles[25] = {
#include "shared/golem_clips/golem_motion_slam_slam_sunk_joint_6_angles.inc"
};
static NJS_MKEY_F golem_motion_slam_slam_sunk_joint_7_position[2] = {
#include "shared/golem_clips/golem_motion_slam_slam_sunk_joint_7_position.inc"
};
static NJS_MKEY_A golem_motion_slam_slam_sunk_joint_7_angles[25] = {
#include "shared/golem_clips/golem_motion_slam_slam_sunk_joint_7_angles.inc"
};
static NJS_MKEY_F golem_motion_slam_slam_sunk_joint_8_position[2] = {
#include "shared/golem_clips/golem_motion_slam_slam_sunk_joint_8_position.inc"
};
static NJS_MKEY_A golem_motion_slam_slam_sunk_joint_8_angles[25] = {
#include "shared/golem_clips/golem_motion_slam_slam_sunk_joint_8_angles.inc"
};
static NJS_MKEY_F golem_motion_slam_slam_sunk_joint_9_position[2] = {
#include "shared/golem_clips/golem_motion_slam_slam_sunk_joint_9_position.inc"
};
static NJS_MKEY_A golem_motion_slam_slam_sunk_joint_9_angles[25] = {
#include "shared/golem_clips/golem_motion_slam_slam_sunk_joint_9_angles.inc"
};
static NJS_MKEY_F golem_motion_slam_slam_sunk_joint_10_position[2] = {
#include "shared/golem_clips/golem_motion_slam_slam_sunk_joint_10_position.inc"
};
static NJS_MKEY_A golem_motion_slam_slam_sunk_joint_10_angles[25] = {
#include "shared/golem_clips/golem_motion_slam_slam_sunk_joint_10_angles.inc"
};
static NJS_MKEY_F golem_motion_slam_slam_sunk_joint_11_position[2] = {
#include "shared/golem_clips/golem_motion_slam_slam_sunk_joint_11_position.inc"
};
static NJS_MKEY_A golem_motion_slam_slam_sunk_joint_11_angles[25] = {
#include "shared/golem_clips/golem_motion_slam_slam_sunk_joint_11_angles.inc"
};
static NJS_MKEY_F golem_motion_slam_slam_sunk_joint_12_position[2] = {
#include "shared/golem_clips/golem_motion_slam_slam_sunk_joint_12_position.inc"
};
static NJS_MKEY_A golem_motion_slam_slam_sunk_joint_12_angles[25] = {
#include "shared/golem_clips/golem_motion_slam_slam_sunk_joint_12_angles.inc"
};
static NJS_MKEY_F golem_motion_slam_slam_sunk_joint_13_position[2] = {
#include "shared/golem_clips/golem_motion_slam_slam_sunk_joint_13_position.inc"
};
static NJS_MKEY_A golem_motion_slam_slam_sunk_joint_13_angles[25] = {
#include "shared/golem_clips/golem_motion_slam_slam_sunk_joint_13_angles.inc"
};
static NJS_MKEY_F golem_motion_slam_slam_sunk_joint_14_position[2] = {
#include "shared/golem_clips/golem_motion_slam_slam_sunk_joint_14_position.inc"
};
static NJS_MKEY_A golem_motion_slam_slam_sunk_joint_14_angles[25] = {
#include "shared/golem_clips/golem_motion_slam_slam_sunk_joint_14_angles.inc"
};
static NJS_MKEY_F golem_motion_slam_slam_sunk_joint_15_position[2] = {
#include "shared/golem_clips/golem_motion_slam_slam_sunk_joint_15_position.inc"
};
static NJS_MKEY_A golem_motion_slam_slam_sunk_joint_15_angles[25] = {
#include "shared/golem_clips/golem_motion_slam_slam_sunk_joint_15_angles.inc"
};
static NJS_MKEY_F golem_motion_slam_slam_sunk_joint_16_position[2] = {
#include "shared/golem_clips/golem_motion_slam_slam_sunk_joint_16_position.inc"
};
static NJS_MKEY_A golem_motion_slam_slam_sunk_joint_16_angles[25] = {
#include "shared/golem_clips/golem_motion_slam_slam_sunk_joint_16_angles.inc"
};
static NJS_MKEY_F golem_motion_slam_slam_sunk_joint_17_position[2] = {
#include "shared/golem_clips/golem_motion_slam_slam_sunk_joint_17_position.inc"
};
static NJS_MKEY_A golem_motion_slam_slam_sunk_joint_17_angles[18] = {
#include "shared/golem_clips/golem_motion_slam_slam_sunk_joint_17_angles.inc"
};
static NJS_MKEY_F golem_motion_slam_slam_sunk_joint_18_position[2] = {
#include "shared/golem_clips/golem_motion_slam_slam_sunk_joint_18_position.inc"
};
static NJS_MKEY_A golem_motion_slam_slam_sunk_joint_18_angles[25] = {
#include "shared/golem_clips/golem_motion_slam_slam_sunk_joint_18_angles.inc"
};
static NJS_MKEY_F golem_motion_slam_slam_sunk_joint_19_position[2] = {
#include "shared/golem_clips/golem_motion_slam_slam_sunk_joint_19_position.inc"
};
static NJS_MKEY_A golem_motion_slam_slam_sunk_joint_19_angles[21] = {
#include "shared/golem_clips/golem_motion_slam_slam_sunk_joint_19_angles.inc"
};
static NJS_MKEY_F golem_motion_slam_slam_sunk_joint_20_position[2] = {
#include "shared/golem_clips/golem_motion_slam_slam_sunk_joint_20_position.inc"
};
static NJS_MKEY_A golem_motion_slam_slam_sunk_joint_20_angles[25] = {
#include "shared/golem_clips/golem_motion_slam_slam_sunk_joint_20_angles.inc"
};
static NJS_MDATA3 golem_motion_slam_slam_sunk_tracks[21] = {
#include "shared/golem_clips/golem_motion_slam_slam_sunk_tracks.inc"
};
NJS_MOTION golem_motion_slam_slam_sunk = {
#include "shared/golem_clips/golem_motion_slam_slam_sunk.inc"
};
static NJS_MKEY_F golem_motion_slam_end_sunk_joint_0_position[40] = {
#include "shared/golem_clips/golem_motion_slam_end_sunk_joint_0_position.inc"
};
static NJS_MKEY_A golem_motion_slam_end_sunk_joint_0_angles[2] = {
#include "shared/golem_clips/golem_motion_slam_end_sunk_joint_0_angles.inc"
};
static NJS_MKEY_F golem_motion_slam_end_sunk_joint_1_position[2] = {
#include "shared/golem_clips/golem_motion_slam_end_sunk_joint_1_position.inc"
};
static NJS_MKEY_A golem_motion_slam_end_sunk_joint_1_angles[2] = {
#include "shared/golem_clips/golem_motion_slam_end_sunk_joint_1_angles.inc"
};
static NJS_MKEY_F golem_motion_slam_end_sunk_joint_2_position[2] = {
#include "shared/golem_clips/golem_motion_slam_end_sunk_joint_2_position.inc"
};
static NJS_MKEY_A golem_motion_slam_end_sunk_joint_2_angles[2] = {
#include "shared/golem_clips/golem_motion_slam_end_sunk_joint_2_angles.inc"
};
static NJS_MKEY_F golem_motion_slam_end_sunk_joint_3_position[2] = {
#include "shared/golem_clips/golem_motion_slam_end_sunk_joint_3_position.inc"
};
static NJS_MKEY_A golem_motion_slam_end_sunk_joint_3_angles[2] = {
#include "shared/golem_clips/golem_motion_slam_end_sunk_joint_3_angles.inc"
};
static NJS_MKEY_F golem_motion_slam_end_sunk_joint_4_position[2] = {
#include "shared/golem_clips/golem_motion_slam_end_sunk_joint_4_position.inc"
};
static NJS_MKEY_A golem_motion_slam_end_sunk_joint_4_angles[2] = {
#include "shared/golem_clips/golem_motion_slam_end_sunk_joint_4_angles.inc"
};
static NJS_MKEY_F golem_motion_slam_end_sunk_joint_5_position[2] = {
#include "shared/golem_clips/golem_motion_slam_end_sunk_joint_5_position.inc"
};
static NJS_MKEY_A golem_motion_slam_end_sunk_joint_5_angles[40] = {
#include "shared/golem_clips/golem_motion_slam_end_sunk_joint_5_angles.inc"
};
static NJS_MKEY_F golem_motion_slam_end_sunk_joint_6_position[2] = {
#include "shared/golem_clips/golem_motion_slam_end_sunk_joint_6_position.inc"
};
static NJS_MKEY_A golem_motion_slam_end_sunk_joint_6_angles[40] = {
#include "shared/golem_clips/golem_motion_slam_end_sunk_joint_6_angles.inc"
};
static NJS_MKEY_F golem_motion_slam_end_sunk_joint_7_position[2] = {
#include "shared/golem_clips/golem_motion_slam_end_sunk_joint_7_position.inc"
};
static NJS_MKEY_A golem_motion_slam_end_sunk_joint_7_angles[40] = {
#include "shared/golem_clips/golem_motion_slam_end_sunk_joint_7_angles.inc"
};
static NJS_MKEY_F golem_motion_slam_end_sunk_joint_8_position[2] = {
#include "shared/golem_clips/golem_motion_slam_end_sunk_joint_8_position.inc"
};
static NJS_MKEY_A golem_motion_slam_end_sunk_joint_8_angles[40] = {
#include "shared/golem_clips/golem_motion_slam_end_sunk_joint_8_angles.inc"
};
static NJS_MKEY_F golem_motion_slam_end_sunk_joint_9_position[2] = {
#include "shared/golem_clips/golem_motion_slam_end_sunk_joint_9_position.inc"
};
static NJS_MKEY_A golem_motion_slam_end_sunk_joint_9_angles[40] = {
#include "shared/golem_clips/golem_motion_slam_end_sunk_joint_9_angles.inc"
};
static NJS_MKEY_F golem_motion_slam_end_sunk_joint_10_position[2] = {
#include "shared/golem_clips/golem_motion_slam_end_sunk_joint_10_position.inc"
};
static NJS_MKEY_A golem_motion_slam_end_sunk_joint_10_angles[40] = {
#include "shared/golem_clips/golem_motion_slam_end_sunk_joint_10_angles.inc"
};
static NJS_MKEY_F golem_motion_slam_end_sunk_joint_11_position[2] = {
#include "shared/golem_clips/golem_motion_slam_end_sunk_joint_11_position.inc"
};
static NJS_MKEY_A golem_motion_slam_end_sunk_joint_11_angles[40] = {
#include "shared/golem_clips/golem_motion_slam_end_sunk_joint_11_angles.inc"
};
static NJS_MKEY_F golem_motion_slam_end_sunk_joint_12_position[2] = {
#include "shared/golem_clips/golem_motion_slam_end_sunk_joint_12_position.inc"
};
static NJS_MKEY_A golem_motion_slam_end_sunk_joint_12_angles[39] = {
#include "shared/golem_clips/golem_motion_slam_end_sunk_joint_12_angles.inc"
};
static NJS_MKEY_F golem_motion_slam_end_sunk_joint_13_position[2] = {
#include "shared/golem_clips/golem_motion_slam_end_sunk_joint_13_position.inc"
};
static NJS_MKEY_A golem_motion_slam_end_sunk_joint_13_angles[40] = {
#include "shared/golem_clips/golem_motion_slam_end_sunk_joint_13_angles.inc"
};
static NJS_MKEY_F golem_motion_slam_end_sunk_joint_14_position[2] = {
#include "shared/golem_clips/golem_motion_slam_end_sunk_joint_14_position.inc"
};
static NJS_MKEY_A golem_motion_slam_end_sunk_joint_14_angles[40] = {
#include "shared/golem_clips/golem_motion_slam_end_sunk_joint_14_angles.inc"
};
static NJS_MKEY_F golem_motion_slam_end_sunk_joint_15_position[2] = {
#include "shared/golem_clips/golem_motion_slam_end_sunk_joint_15_position.inc"
};
static NJS_MKEY_A golem_motion_slam_end_sunk_joint_15_angles[40] = {
#include "shared/golem_clips/golem_motion_slam_end_sunk_joint_15_angles.inc"
};
static NJS_MKEY_F golem_motion_slam_end_sunk_joint_16_position[2] = {
#include "shared/golem_clips/golem_motion_slam_end_sunk_joint_16_position.inc"
};
static NJS_MKEY_A golem_motion_slam_end_sunk_joint_16_angles[39] = {
#include "shared/golem_clips/golem_motion_slam_end_sunk_joint_16_angles.inc"
};
static NJS_MKEY_F golem_motion_slam_end_sunk_joint_17_position[2] = {
#include "shared/golem_clips/golem_motion_slam_end_sunk_joint_17_position.inc"
};
static NJS_MKEY_A golem_motion_slam_end_sunk_joint_17_angles[40] = {
#include "shared/golem_clips/golem_motion_slam_end_sunk_joint_17_angles.inc"
};
static NJS_MKEY_F golem_motion_slam_end_sunk_joint_18_position[2] = {
#include "shared/golem_clips/golem_motion_slam_end_sunk_joint_18_position.inc"
};
static NJS_MKEY_A golem_motion_slam_end_sunk_joint_18_angles[40] = {
#include "shared/golem_clips/golem_motion_slam_end_sunk_joint_18_angles.inc"
};
static NJS_MKEY_F golem_motion_slam_end_sunk_joint_19_position[2] = {
#include "shared/golem_clips/golem_motion_slam_end_sunk_joint_19_position.inc"
};
static NJS_MKEY_A golem_motion_slam_end_sunk_joint_19_angles[40] = {
#include "shared/golem_clips/golem_motion_slam_end_sunk_joint_19_angles.inc"
};
static NJS_MKEY_F golem_motion_slam_end_sunk_joint_20_position[2] = {
#include "shared/golem_clips/golem_motion_slam_end_sunk_joint_20_position.inc"
};
static NJS_MKEY_A golem_motion_slam_end_sunk_joint_20_angles[40] = {
#include "shared/golem_clips/golem_motion_slam_end_sunk_joint_20_angles.inc"
};
static NJS_MDATA3 golem_motion_slam_end_sunk_tracks[21] = {
#include "shared/golem_clips/golem_motion_slam_end_sunk_tracks.inc"
};
NJS_MOTION golem_motion_slam_end_sunk = {
#include "shared/golem_clips/golem_motion_slam_end_sunk.inc"
};
static NJS_MKEY_F golem_motion_slam_blows_sunk_twice_joint_0_position[29] = {
#include "shared/golem_clips/golem_motion_slam_blows_sunk_twice_joint_0_position.inc"
};
static NJS_MKEY_A golem_motion_slam_blows_sunk_twice_joint_0_angles[2] = {
#include "shared/golem_clips/golem_motion_slam_blows_sunk_twice_joint_0_angles.inc"
};
static NJS_MKEY_F golem_motion_slam_blows_sunk_twice_joint_1_position[2] = {
#include "shared/golem_clips/golem_motion_slam_blows_sunk_twice_joint_1_position.inc"
};
static NJS_MKEY_A golem_motion_slam_blows_sunk_twice_joint_1_angles[2] = {
#include "shared/golem_clips/golem_motion_slam_blows_sunk_twice_joint_1_angles.inc"
};
static NJS_MKEY_F golem_motion_slam_blows_sunk_twice_joint_2_position[2] = {
#include "shared/golem_clips/golem_motion_slam_blows_sunk_twice_joint_2_position.inc"
};
static NJS_MKEY_A golem_motion_slam_blows_sunk_twice_joint_2_angles[2] = {
#include "shared/golem_clips/golem_motion_slam_blows_sunk_twice_joint_2_angles.inc"
};
static NJS_MKEY_F golem_motion_slam_blows_sunk_twice_joint_3_position[2] = {
#include "shared/golem_clips/golem_motion_slam_blows_sunk_twice_joint_3_position.inc"
};
static NJS_MKEY_A golem_motion_slam_blows_sunk_twice_joint_3_angles[2] = {
#include "shared/golem_clips/golem_motion_slam_blows_sunk_twice_joint_3_angles.inc"
};
static NJS_MKEY_F golem_motion_slam_blows_sunk_twice_joint_4_position[2] = {
#include "shared/golem_clips/golem_motion_slam_blows_sunk_twice_joint_4_position.inc"
};
static NJS_MKEY_A golem_motion_slam_blows_sunk_twice_joint_4_angles[2] = {
#include "shared/golem_clips/golem_motion_slam_blows_sunk_twice_joint_4_angles.inc"
};
static NJS_MKEY_F golem_motion_slam_blows_sunk_twice_joint_5_position[2] = {
#include "shared/golem_clips/golem_motion_slam_blows_sunk_twice_joint_5_position.inc"
};
static NJS_MKEY_A golem_motion_slam_blows_sunk_twice_joint_5_angles[29] = {
#include "shared/golem_clips/golem_motion_slam_blows_sunk_twice_joint_5_angles.inc"
};
static NJS_MKEY_F golem_motion_slam_blows_sunk_twice_joint_6_position[2] = {
#include "shared/golem_clips/golem_motion_slam_blows_sunk_twice_joint_6_position.inc"
};
static NJS_MKEY_A golem_motion_slam_blows_sunk_twice_joint_6_angles[30] = {
#include "shared/golem_clips/golem_motion_slam_blows_sunk_twice_joint_6_angles.inc"
};
static NJS_MKEY_F golem_motion_slam_blows_sunk_twice_joint_7_position[2] = {
#include "shared/golem_clips/golem_motion_slam_blows_sunk_twice_joint_7_position.inc"
};
static NJS_MKEY_A golem_motion_slam_blows_sunk_twice_joint_7_angles[30] = {
#include "shared/golem_clips/golem_motion_slam_blows_sunk_twice_joint_7_angles.inc"
};
static NJS_MKEY_F golem_motion_slam_blows_sunk_twice_joint_8_position[2] = {
#include "shared/golem_clips/golem_motion_slam_blows_sunk_twice_joint_8_position.inc"
};
static NJS_MKEY_A golem_motion_slam_blows_sunk_twice_joint_8_angles[30] = {
#include "shared/golem_clips/golem_motion_slam_blows_sunk_twice_joint_8_angles.inc"
};
static NJS_MKEY_F golem_motion_slam_blows_sunk_twice_joint_9_position[2] = {
#include "shared/golem_clips/golem_motion_slam_blows_sunk_twice_joint_9_position.inc"
};
static NJS_MKEY_A golem_motion_slam_blows_sunk_twice_joint_9_angles[30] = {
#include "shared/golem_clips/golem_motion_slam_blows_sunk_twice_joint_9_angles.inc"
};
static NJS_MKEY_F golem_motion_slam_blows_sunk_twice_joint_10_position[2] = {
#include "shared/golem_clips/golem_motion_slam_blows_sunk_twice_joint_10_position.inc"
};
static NJS_MKEY_A golem_motion_slam_blows_sunk_twice_joint_10_angles[30] = {
#include "shared/golem_clips/golem_motion_slam_blows_sunk_twice_joint_10_angles.inc"
};
static NJS_MKEY_F golem_motion_slam_blows_sunk_twice_joint_11_position[2] = {
#include "shared/golem_clips/golem_motion_slam_blows_sunk_twice_joint_11_position.inc"
};
static NJS_MKEY_A golem_motion_slam_blows_sunk_twice_joint_11_angles[30] = {
#include "shared/golem_clips/golem_motion_slam_blows_sunk_twice_joint_11_angles.inc"
};
static NJS_MKEY_F golem_motion_slam_blows_sunk_twice_joint_12_position[2] = {
#include "shared/golem_clips/golem_motion_slam_blows_sunk_twice_joint_12_position.inc"
};
static NJS_MKEY_A golem_motion_slam_blows_sunk_twice_joint_12_angles[30] = {
#include "shared/golem_clips/golem_motion_slam_blows_sunk_twice_joint_12_angles.inc"
};
static NJS_MKEY_F golem_motion_slam_blows_sunk_twice_joint_13_position[2] = {
#include "shared/golem_clips/golem_motion_slam_blows_sunk_twice_joint_13_position.inc"
};
static NJS_MKEY_A golem_motion_slam_blows_sunk_twice_joint_13_angles[30] = {
#include "shared/golem_clips/golem_motion_slam_blows_sunk_twice_joint_13_angles.inc"
};
static NJS_MKEY_F golem_motion_slam_blows_sunk_twice_joint_14_position[2] = {
#include "shared/golem_clips/golem_motion_slam_blows_sunk_twice_joint_14_position.inc"
};
static NJS_MKEY_A golem_motion_slam_blows_sunk_twice_joint_14_angles[30] = {
#include "shared/golem_clips/golem_motion_slam_blows_sunk_twice_joint_14_angles.inc"
};
static NJS_MKEY_F golem_motion_slam_blows_sunk_twice_joint_15_position[2] = {
#include "shared/golem_clips/golem_motion_slam_blows_sunk_twice_joint_15_position.inc"
};
static NJS_MKEY_A golem_motion_slam_blows_sunk_twice_joint_15_angles[30] = {
#include "shared/golem_clips/golem_motion_slam_blows_sunk_twice_joint_15_angles.inc"
};
static NJS_MKEY_F golem_motion_slam_blows_sunk_twice_joint_16_position[2] = {
#include "shared/golem_clips/golem_motion_slam_blows_sunk_twice_joint_16_position.inc"
};
static NJS_MKEY_A golem_motion_slam_blows_sunk_twice_joint_16_angles[30] = {
#include "shared/golem_clips/golem_motion_slam_blows_sunk_twice_joint_16_angles.inc"
};
static NJS_MKEY_F golem_motion_slam_blows_sunk_twice_joint_17_position[2] = {
#include "shared/golem_clips/golem_motion_slam_blows_sunk_twice_joint_17_position.inc"
};
static NJS_MKEY_A golem_motion_slam_blows_sunk_twice_joint_17_angles[30] = {
#include "shared/golem_clips/golem_motion_slam_blows_sunk_twice_joint_17_angles.inc"
};
static NJS_MKEY_F golem_motion_slam_blows_sunk_twice_joint_18_position[2] = {
#include "shared/golem_clips/golem_motion_slam_blows_sunk_twice_joint_18_position.inc"
};
static NJS_MKEY_A golem_motion_slam_blows_sunk_twice_joint_18_angles[30] = {
#include "shared/golem_clips/golem_motion_slam_blows_sunk_twice_joint_18_angles.inc"
};
static NJS_MKEY_F golem_motion_slam_blows_sunk_twice_joint_19_position[2] = {
#include "shared/golem_clips/golem_motion_slam_blows_sunk_twice_joint_19_position.inc"
};
static NJS_MKEY_A golem_motion_slam_blows_sunk_twice_joint_19_angles[30] = {
#include "shared/golem_clips/golem_motion_slam_blows_sunk_twice_joint_19_angles.inc"
};
static NJS_MKEY_F golem_motion_slam_blows_sunk_twice_joint_20_position[2] = {
#include "shared/golem_clips/golem_motion_slam_blows_sunk_twice_joint_20_position.inc"
};
static NJS_MKEY_A golem_motion_slam_blows_sunk_twice_joint_20_angles[30] = {
#include "shared/golem_clips/golem_motion_slam_blows_sunk_twice_joint_20_angles.inc"
};
static NJS_MDATA3 golem_motion_slam_blows_sunk_twice_tracks[21] = {
#include "shared/golem_clips/golem_motion_slam_blows_sunk_twice_tracks.inc"
};
NJS_MOTION golem_motion_slam_blows_sunk_twice = {
#include "shared/golem_clips/golem_motion_slam_blows_sunk_twice.inc"
};
static NJS_MKEY_F golem_motion_slam_slam_sunk_twice_joint_0_position[25] = {
#include "shared/golem_clips/golem_motion_slam_slam_sunk_twice_joint_0_position.inc"
};
static NJS_MKEY_A golem_motion_slam_slam_sunk_twice_joint_0_angles[2] = {
#include "shared/golem_clips/golem_motion_slam_slam_sunk_twice_joint_0_angles.inc"
};
static NJS_MKEY_F golem_motion_slam_slam_sunk_twice_joint_1_position[2] = {
#include "shared/golem_clips/golem_motion_slam_slam_sunk_twice_joint_1_position.inc"
};
static NJS_MKEY_A golem_motion_slam_slam_sunk_twice_joint_1_angles[2] = {
#include "shared/golem_clips/golem_motion_slam_slam_sunk_twice_joint_1_angles.inc"
};
static NJS_MKEY_F golem_motion_slam_slam_sunk_twice_joint_2_position[2] = {
#include "shared/golem_clips/golem_motion_slam_slam_sunk_twice_joint_2_position.inc"
};
static NJS_MKEY_A golem_motion_slam_slam_sunk_twice_joint_2_angles[2] = {
#include "shared/golem_clips/golem_motion_slam_slam_sunk_twice_joint_2_angles.inc"
};
static NJS_MKEY_F golem_motion_slam_slam_sunk_twice_joint_3_position[2] = {
#include "shared/golem_clips/golem_motion_slam_slam_sunk_twice_joint_3_position.inc"
};
static NJS_MKEY_A golem_motion_slam_slam_sunk_twice_joint_3_angles[2] = {
#include "shared/golem_clips/golem_motion_slam_slam_sunk_twice_joint_3_angles.inc"
};
static NJS_MKEY_F golem_motion_slam_slam_sunk_twice_joint_4_position[2] = {
#include "shared/golem_clips/golem_motion_slam_slam_sunk_twice_joint_4_position.inc"
};
static NJS_MKEY_A golem_motion_slam_slam_sunk_twice_joint_4_angles[2] = {
#include "shared/golem_clips/golem_motion_slam_slam_sunk_twice_joint_4_angles.inc"
};
static NJS_MKEY_F golem_motion_slam_slam_sunk_twice_joint_5_position[2] = {
#include "shared/golem_clips/golem_motion_slam_slam_sunk_twice_joint_5_position.inc"
};
static NJS_MKEY_A golem_motion_slam_slam_sunk_twice_joint_5_angles[25] = {
#include "shared/golem_clips/golem_motion_slam_slam_sunk_twice_joint_5_angles.inc"
};
static NJS_MKEY_F golem_motion_slam_slam_sunk_twice_joint_6_position[2] = {
#include "shared/golem_clips/golem_motion_slam_slam_sunk_twice_joint_6_position.inc"
};
static NJS_MKEY_A golem_motion_slam_slam_sunk_twice_joint_6_angles[25] = {
#include "shared/golem_clips/golem_motion_slam_slam_sunk_twice_joint_6_angles.inc"
};
static NJS_MKEY_F golem_motion_slam_slam_sunk_twice_joint_7_position[2] = {
#include "shared/golem_clips/golem_motion_slam_slam_sunk_twice_joint_7_position.inc"
};
static NJS_MKEY_A golem_motion_slam_slam_sunk_twice_joint_7_angles[25] = {
#include "shared/golem_clips/golem_motion_slam_slam_sunk_twice_joint_7_angles.inc"
};
static NJS_MKEY_F golem_motion_slam_slam_sunk_twice_joint_8_position[2] = {
#include "shared/golem_clips/golem_motion_slam_slam_sunk_twice_joint_8_position.inc"
};
static NJS_MKEY_A golem_motion_slam_slam_sunk_twice_joint_8_angles[25] = {
#include "shared/golem_clips/golem_motion_slam_slam_sunk_twice_joint_8_angles.inc"
};
static NJS_MKEY_F golem_motion_slam_slam_sunk_twice_joint_9_position[2] = {
#include "shared/golem_clips/golem_motion_slam_slam_sunk_twice_joint_9_position.inc"
};
static NJS_MKEY_A golem_motion_slam_slam_sunk_twice_joint_9_angles[25] = {
#include "shared/golem_clips/golem_motion_slam_slam_sunk_twice_joint_9_angles.inc"
};
static NJS_MKEY_F golem_motion_slam_slam_sunk_twice_joint_10_position[2] = {
#include "shared/golem_clips/golem_motion_slam_slam_sunk_twice_joint_10_position.inc"
};
static NJS_MKEY_A golem_motion_slam_slam_sunk_twice_joint_10_angles[25] = {
#include "shared/golem_clips/golem_motion_slam_slam_sunk_twice_joint_10_angles.inc"
};
static NJS_MKEY_F golem_motion_slam_slam_sunk_twice_joint_11_position[2] = {
#include "shared/golem_clips/golem_motion_slam_slam_sunk_twice_joint_11_position.inc"
};
static NJS_MKEY_A golem_motion_slam_slam_sunk_twice_joint_11_angles[25] = {
#include "shared/golem_clips/golem_motion_slam_slam_sunk_twice_joint_11_angles.inc"
};
static NJS_MKEY_F golem_motion_slam_slam_sunk_twice_joint_12_position[2] = {
#include "shared/golem_clips/golem_motion_slam_slam_sunk_twice_joint_12_position.inc"
};
static NJS_MKEY_A golem_motion_slam_slam_sunk_twice_joint_12_angles[25] = {
#include "shared/golem_clips/golem_motion_slam_slam_sunk_twice_joint_12_angles.inc"
};
static NJS_MKEY_F golem_motion_slam_slam_sunk_twice_joint_13_position[2] = {
#include "shared/golem_clips/golem_motion_slam_slam_sunk_twice_joint_13_position.inc"
};
static NJS_MKEY_A golem_motion_slam_slam_sunk_twice_joint_13_angles[25] = {
#include "shared/golem_clips/golem_motion_slam_slam_sunk_twice_joint_13_angles.inc"
};
static NJS_MKEY_F golem_motion_slam_slam_sunk_twice_joint_14_position[2] = {
#include "shared/golem_clips/golem_motion_slam_slam_sunk_twice_joint_14_position.inc"
};
static NJS_MKEY_A golem_motion_slam_slam_sunk_twice_joint_14_angles[25] = {
#include "shared/golem_clips/golem_motion_slam_slam_sunk_twice_joint_14_angles.inc"
};
static NJS_MKEY_F golem_motion_slam_slam_sunk_twice_joint_15_position[2] = {
#include "shared/golem_clips/golem_motion_slam_slam_sunk_twice_joint_15_position.inc"
};
static NJS_MKEY_A golem_motion_slam_slam_sunk_twice_joint_15_angles[25] = {
#include "shared/golem_clips/golem_motion_slam_slam_sunk_twice_joint_15_angles.inc"
};
static NJS_MKEY_F golem_motion_slam_slam_sunk_twice_joint_16_position[2] = {
#include "shared/golem_clips/golem_motion_slam_slam_sunk_twice_joint_16_position.inc"
};
static NJS_MKEY_A golem_motion_slam_slam_sunk_twice_joint_16_angles[25] = {
#include "shared/golem_clips/golem_motion_slam_slam_sunk_twice_joint_16_angles.inc"
};
static NJS_MKEY_F golem_motion_slam_slam_sunk_twice_joint_17_position[2] = {
#include "shared/golem_clips/golem_motion_slam_slam_sunk_twice_joint_17_position.inc"
};
static NJS_MKEY_A golem_motion_slam_slam_sunk_twice_joint_17_angles[18] = {
#include "shared/golem_clips/golem_motion_slam_slam_sunk_twice_joint_17_angles.inc"
};
static NJS_MKEY_F golem_motion_slam_slam_sunk_twice_joint_18_position[2] = {
#include "shared/golem_clips/golem_motion_slam_slam_sunk_twice_joint_18_position.inc"
};
static NJS_MKEY_A golem_motion_slam_slam_sunk_twice_joint_18_angles[25] = {
#include "shared/golem_clips/golem_motion_slam_slam_sunk_twice_joint_18_angles.inc"
};
static NJS_MKEY_F golem_motion_slam_slam_sunk_twice_joint_19_position[2] = {
#include "shared/golem_clips/golem_motion_slam_slam_sunk_twice_joint_19_position.inc"
};
static NJS_MKEY_A golem_motion_slam_slam_sunk_twice_joint_19_angles[21] = {
#include "shared/golem_clips/golem_motion_slam_slam_sunk_twice_joint_19_angles.inc"
};
static NJS_MKEY_F golem_motion_slam_slam_sunk_twice_joint_20_position[2] = {
#include "shared/golem_clips/golem_motion_slam_slam_sunk_twice_joint_20_position.inc"
};
static NJS_MKEY_A golem_motion_slam_slam_sunk_twice_joint_20_angles[25] = {
#include "shared/golem_clips/golem_motion_slam_slam_sunk_twice_joint_20_angles.inc"
};
static NJS_MDATA3 golem_motion_slam_slam_sunk_twice_tracks[21] = {
#include "shared/golem_clips/golem_motion_slam_slam_sunk_twice_tracks.inc"
};
NJS_MOTION golem_motion_slam_slam_sunk_twice = {
#include "shared/golem_clips/golem_motion_slam_slam_sunk_twice.inc"
};
static NJS_MKEY_F golem_motion_slam_end_sunk_twice_joint_0_position[40] = {
#include "shared/golem_clips/golem_motion_slam_end_sunk_twice_joint_0_position.inc"
};
static NJS_MKEY_A golem_motion_slam_end_sunk_twice_joint_0_angles[2] = {
#include "shared/golem_clips/golem_motion_slam_end_sunk_twice_joint_0_angles.inc"
};
static NJS_MKEY_F golem_motion_slam_end_sunk_twice_joint_1_position[2] = {
#include "shared/golem_clips/golem_motion_slam_end_sunk_twice_joint_1_position.inc"
};
static NJS_MKEY_A golem_motion_slam_end_sunk_twice_joint_1_angles[2] = {
#include "shared/golem_clips/golem_motion_slam_end_sunk_twice_joint_1_angles.inc"
};
static NJS_MKEY_F golem_motion_slam_end_sunk_twice_joint_2_position[2] = {
#include "shared/golem_clips/golem_motion_slam_end_sunk_twice_joint_2_position.inc"
};
static NJS_MKEY_A golem_motion_slam_end_sunk_twice_joint_2_angles[2] = {
#include "shared/golem_clips/golem_motion_slam_end_sunk_twice_joint_2_angles.inc"
};
static NJS_MKEY_F golem_motion_slam_end_sunk_twice_joint_3_position[2] = {
#include "shared/golem_clips/golem_motion_slam_end_sunk_twice_joint_3_position.inc"
};
static NJS_MKEY_A golem_motion_slam_end_sunk_twice_joint_3_angles[2] = {
#include "shared/golem_clips/golem_motion_slam_end_sunk_twice_joint_3_angles.inc"
};
static NJS_MKEY_F golem_motion_slam_end_sunk_twice_joint_4_position[2] = {
#include "shared/golem_clips/golem_motion_slam_end_sunk_twice_joint_4_position.inc"
};
static NJS_MKEY_A golem_motion_slam_end_sunk_twice_joint_4_angles[2] = {
#include "shared/golem_clips/golem_motion_slam_end_sunk_twice_joint_4_angles.inc"
};
static NJS_MKEY_F golem_motion_slam_end_sunk_twice_joint_5_position[2] = {
#include "shared/golem_clips/golem_motion_slam_end_sunk_twice_joint_5_position.inc"
};
static NJS_MKEY_A golem_motion_slam_end_sunk_twice_joint_5_angles[40] = {
#include "shared/golem_clips/golem_motion_slam_end_sunk_twice_joint_5_angles.inc"
};
static NJS_MKEY_F golem_motion_slam_end_sunk_twice_joint_6_position[2] = {
#include "shared/golem_clips/golem_motion_slam_end_sunk_twice_joint_6_position.inc"
};
static NJS_MKEY_A golem_motion_slam_end_sunk_twice_joint_6_angles[40] = {
#include "shared/golem_clips/golem_motion_slam_end_sunk_twice_joint_6_angles.inc"
};
static NJS_MKEY_F golem_motion_slam_end_sunk_twice_joint_7_position[2] = {
#include "shared/golem_clips/golem_motion_slam_end_sunk_twice_joint_7_position.inc"
};
static NJS_MKEY_A golem_motion_slam_end_sunk_twice_joint_7_angles[40] = {
#include "shared/golem_clips/golem_motion_slam_end_sunk_twice_joint_7_angles.inc"
};
static NJS_MKEY_F golem_motion_slam_end_sunk_twice_joint_8_position[2] = {
#include "shared/golem_clips/golem_motion_slam_end_sunk_twice_joint_8_position.inc"
};
static NJS_MKEY_A golem_motion_slam_end_sunk_twice_joint_8_angles[40] = {
#include "shared/golem_clips/golem_motion_slam_end_sunk_twice_joint_8_angles.inc"
};
static NJS_MKEY_F golem_motion_slam_end_sunk_twice_joint_9_position[2] = {
#include "shared/golem_clips/golem_motion_slam_end_sunk_twice_joint_9_position.inc"
};
static NJS_MKEY_A golem_motion_slam_end_sunk_twice_joint_9_angles[40] = {
#include "shared/golem_clips/golem_motion_slam_end_sunk_twice_joint_9_angles.inc"
};
static NJS_MKEY_F golem_motion_slam_end_sunk_twice_joint_10_position[2] = {
#include "shared/golem_clips/golem_motion_slam_end_sunk_twice_joint_10_position.inc"
};
static NJS_MKEY_A golem_motion_slam_end_sunk_twice_joint_10_angles[40] = {
#include "shared/golem_clips/golem_motion_slam_end_sunk_twice_joint_10_angles.inc"
};
static NJS_MKEY_F golem_motion_slam_end_sunk_twice_joint_11_position[2] = {
#include "shared/golem_clips/golem_motion_slam_end_sunk_twice_joint_11_position.inc"
};
static NJS_MKEY_A golem_motion_slam_end_sunk_twice_joint_11_angles[40] = {
#include "shared/golem_clips/golem_motion_slam_end_sunk_twice_joint_11_angles.inc"
};
static NJS_MKEY_F golem_motion_slam_end_sunk_twice_joint_12_position[2] = {
#include "shared/golem_clips/golem_motion_slam_end_sunk_twice_joint_12_position.inc"
};
static NJS_MKEY_A golem_motion_slam_end_sunk_twice_joint_12_angles[39] = {
#include "shared/golem_clips/golem_motion_slam_end_sunk_twice_joint_12_angles.inc"
};
static NJS_MKEY_F golem_motion_slam_end_sunk_twice_joint_13_position[2] = {
#include "shared/golem_clips/golem_motion_slam_end_sunk_twice_joint_13_position.inc"
};
static NJS_MKEY_A golem_motion_slam_end_sunk_twice_joint_13_angles[40] = {
#include "shared/golem_clips/golem_motion_slam_end_sunk_twice_joint_13_angles.inc"
};
static NJS_MKEY_F golem_motion_slam_end_sunk_twice_joint_14_position[2] = {
#include "shared/golem_clips/golem_motion_slam_end_sunk_twice_joint_14_position.inc"
};
static NJS_MKEY_A golem_motion_slam_end_sunk_twice_joint_14_angles[40] = {
#include "shared/golem_clips/golem_motion_slam_end_sunk_twice_joint_14_angles.inc"
};
static NJS_MKEY_F golem_motion_slam_end_sunk_twice_joint_15_position[2] = {
#include "shared/golem_clips/golem_motion_slam_end_sunk_twice_joint_15_position.inc"
};
static NJS_MKEY_A golem_motion_slam_end_sunk_twice_joint_15_angles[40] = {
#include "shared/golem_clips/golem_motion_slam_end_sunk_twice_joint_15_angles.inc"
};
static NJS_MKEY_F golem_motion_slam_end_sunk_twice_joint_16_position[2] = {
#include "shared/golem_clips/golem_motion_slam_end_sunk_twice_joint_16_position.inc"
};
static NJS_MKEY_A golem_motion_slam_end_sunk_twice_joint_16_angles[39] = {
#include "shared/golem_clips/golem_motion_slam_end_sunk_twice_joint_16_angles.inc"
};
static NJS_MKEY_F golem_motion_slam_end_sunk_twice_joint_17_position[2] = {
#include "shared/golem_clips/golem_motion_slam_end_sunk_twice_joint_17_position.inc"
};
static NJS_MKEY_A golem_motion_slam_end_sunk_twice_joint_17_angles[40] = {
#include "shared/golem_clips/golem_motion_slam_end_sunk_twice_joint_17_angles.inc"
};
static NJS_MKEY_F golem_motion_slam_end_sunk_twice_joint_18_position[2] = {
#include "shared/golem_clips/golem_motion_slam_end_sunk_twice_joint_18_position.inc"
};
static NJS_MKEY_A golem_motion_slam_end_sunk_twice_joint_18_angles[40] = {
#include "shared/golem_clips/golem_motion_slam_end_sunk_twice_joint_18_angles.inc"
};
static NJS_MKEY_F golem_motion_slam_end_sunk_twice_joint_19_position[2] = {
#include "shared/golem_clips/golem_motion_slam_end_sunk_twice_joint_19_position.inc"
};
static NJS_MKEY_A golem_motion_slam_end_sunk_twice_joint_19_angles[40] = {
#include "shared/golem_clips/golem_motion_slam_end_sunk_twice_joint_19_angles.inc"
};
static NJS_MKEY_F golem_motion_slam_end_sunk_twice_joint_20_position[2] = {
#include "shared/golem_clips/golem_motion_slam_end_sunk_twice_joint_20_position.inc"
};
static NJS_MKEY_A golem_motion_slam_end_sunk_twice_joint_20_angles[40] = {
#include "shared/golem_clips/golem_motion_slam_end_sunk_twice_joint_20_angles.inc"
};
static NJS_MDATA3 golem_motion_slam_end_sunk_twice_tracks[21] = {
#include "shared/golem_clips/golem_motion_slam_end_sunk_twice_tracks.inc"
};
NJS_MOTION golem_motion_slam_end_sunk_twice = {
#include "shared/golem_clips/golem_motion_slam_end_sunk_twice.inc"
};
static NJS_MKEY_A golem_motion_strike_a_start_joint_0_angles[2] = {
#include "shared/golem_clips/golem_motion_strike_a_start_joint_0_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_start_joint_1_angles[51] = {
#include "shared/golem_clips/golem_motion_strike_a_start_joint_1_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_start_joint_2_angles[54] = {
#include "shared/golem_clips/golem_motion_strike_a_start_joint_2_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_start_joint_3_angles[51] = {
#include "shared/golem_clips/golem_motion_strike_a_start_joint_3_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_start_joint_4_angles[2] = {
#include "shared/golem_clips/golem_motion_strike_a_start_joint_4_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_start_joint_5_angles[54] = {
#include "shared/golem_clips/golem_motion_strike_a_start_joint_5_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_start_joint_6_angles[54] = {
#include "shared/golem_clips/golem_motion_strike_a_start_joint_6_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_start_joint_7_angles[54] = {
#include "shared/golem_clips/golem_motion_strike_a_start_joint_7_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_start_joint_8_angles[54] = {
#include "shared/golem_clips/golem_motion_strike_a_start_joint_8_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_start_joint_9_angles[54] = {
#include "shared/golem_clips/golem_motion_strike_a_start_joint_9_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_start_joint_10_angles[53] = {
#include "shared/golem_clips/golem_motion_strike_a_start_joint_10_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_start_joint_11_angles[54] = {
#include "shared/golem_clips/golem_motion_strike_a_start_joint_11_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_start_joint_12_angles[54] = {
#include "shared/golem_clips/golem_motion_strike_a_start_joint_12_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_start_joint_13_angles[54] = {
#include "shared/golem_clips/golem_motion_strike_a_start_joint_13_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_start_joint_14_angles[52] = {
#include "shared/golem_clips/golem_motion_strike_a_start_joint_14_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_start_joint_15_angles[46] = {
#include "shared/golem_clips/golem_motion_strike_a_start_joint_15_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_start_joint_16_angles[50] = {
#include "shared/golem_clips/golem_motion_strike_a_start_joint_16_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_start_joint_17_angles[50] = {
#include "shared/golem_clips/golem_motion_strike_a_start_joint_17_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_start_joint_18_angles[44] = {
#include "shared/golem_clips/golem_motion_strike_a_start_joint_18_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_start_joint_19_angles[22] = {
#include "shared/golem_clips/golem_motion_strike_a_start_joint_19_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_start_joint_20_angles[54] = {
#include "shared/golem_clips/golem_motion_strike_a_start_joint_20_angles.inc"
};
static NJS_MDATA3 golem_motion_strike_a_start_tracks[21] = {
#include "shared/golem_clips/golem_motion_strike_a_start_tracks.inc"
};
NJS_MOTION golem_motion_strike_a_start = {
#include "shared/golem_clips/golem_motion_strike_a_start.inc"
};
static NJS_MKEY_A golem_motion_strike_a_blow_whole_joint_0_angles[2] = {
#include "shared/golem_clips/golem_motion_strike_a_blow_whole_joint_0_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_blow_whole_joint_1_angles[23] = {
#include "shared/golem_clips/golem_motion_strike_a_blow_whole_joint_1_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_blow_whole_joint_2_angles[23] = {
#include "shared/golem_clips/golem_motion_strike_a_blow_whole_joint_2_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_blow_whole_joint_3_angles[23] = {
#include "shared/golem_clips/golem_motion_strike_a_blow_whole_joint_3_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_blow_whole_joint_4_angles[2] = {
#include "shared/golem_clips/golem_motion_strike_a_blow_whole_joint_4_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_blow_whole_joint_5_angles[23] = {
#include "shared/golem_clips/golem_motion_strike_a_blow_whole_joint_5_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_blow_whole_joint_6_angles[23] = {
#include "shared/golem_clips/golem_motion_strike_a_blow_whole_joint_6_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_blow_whole_joint_7_angles[23] = {
#include "shared/golem_clips/golem_motion_strike_a_blow_whole_joint_7_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_blow_whole_joint_8_angles[23] = {
#include "shared/golem_clips/golem_motion_strike_a_blow_whole_joint_8_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_blow_whole_joint_9_angles[23] = {
#include "shared/golem_clips/golem_motion_strike_a_blow_whole_joint_9_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_blow_whole_joint_10_angles[15] = {
#include "shared/golem_clips/golem_motion_strike_a_blow_whole_joint_10_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_blow_whole_joint_11_angles[16] = {
#include "shared/golem_clips/golem_motion_strike_a_blow_whole_joint_11_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_blow_whole_joint_12_angles[18] = {
#include "shared/golem_clips/golem_motion_strike_a_blow_whole_joint_12_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_blow_whole_joint_13_angles[23] = {
#include "shared/golem_clips/golem_motion_strike_a_blow_whole_joint_13_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_blow_whole_joint_14_angles[23] = {
#include "shared/golem_clips/golem_motion_strike_a_blow_whole_joint_14_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_blow_whole_joint_15_angles[23] = {
#include "shared/golem_clips/golem_motion_strike_a_blow_whole_joint_15_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_blow_whole_joint_16_angles[23] = {
#include "shared/golem_clips/golem_motion_strike_a_blow_whole_joint_16_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_blow_whole_joint_17_angles[23] = {
#include "shared/golem_clips/golem_motion_strike_a_blow_whole_joint_17_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_blow_whole_joint_18_angles[23] = {
#include "shared/golem_clips/golem_motion_strike_a_blow_whole_joint_18_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_blow_whole_joint_19_angles[23] = {
#include "shared/golem_clips/golem_motion_strike_a_blow_whole_joint_19_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_blow_whole_joint_20_angles[23] = {
#include "shared/golem_clips/golem_motion_strike_a_blow_whole_joint_20_angles.inc"
};
static NJS_MDATA3 golem_motion_strike_a_blow_whole_tracks[21] = {
#include "shared/golem_clips/golem_motion_strike_a_blow_whole_tracks.inc"
};
NJS_MOTION golem_motion_strike_a_blow_whole = {
#include "shared/golem_clips/golem_motion_strike_a_blow_whole.inc"
};
static NJS_MKEY_A golem_motion_strike_a_rise_whole_joint_0_angles[2] = {
#include "shared/golem_clips/golem_motion_strike_a_rise_whole_joint_0_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_rise_whole_joint_1_angles[34] = {
#include "shared/golem_clips/golem_motion_strike_a_rise_whole_joint_1_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_rise_whole_joint_2_angles[6] = {
#include "shared/golem_clips/golem_motion_strike_a_rise_whole_joint_2_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_rise_whole_joint_3_angles[15] = {
#include "shared/golem_clips/golem_motion_strike_a_rise_whole_joint_3_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_rise_whole_joint_4_angles[2] = {
#include "shared/golem_clips/golem_motion_strike_a_rise_whole_joint_4_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_rise_whole_joint_5_angles[46] = {
#include "shared/golem_clips/golem_motion_strike_a_rise_whole_joint_5_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_rise_whole_joint_6_angles[44] = {
#include "shared/golem_clips/golem_motion_strike_a_rise_whole_joint_6_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_rise_whole_joint_7_angles[46] = {
#include "shared/golem_clips/golem_motion_strike_a_rise_whole_joint_7_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_rise_whole_joint_8_angles[44] = {
#include "shared/golem_clips/golem_motion_strike_a_rise_whole_joint_8_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_rise_whole_joint_9_angles[46] = {
#include "shared/golem_clips/golem_motion_strike_a_rise_whole_joint_9_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_rise_whole_joint_10_angles[46] = {
#include "shared/golem_clips/golem_motion_strike_a_rise_whole_joint_10_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_rise_whole_joint_11_angles[42] = {
#include "shared/golem_clips/golem_motion_strike_a_rise_whole_joint_11_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_rise_whole_joint_12_angles[46] = {
#include "shared/golem_clips/golem_motion_strike_a_rise_whole_joint_12_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_rise_whole_joint_13_angles[46] = {
#include "shared/golem_clips/golem_motion_strike_a_rise_whole_joint_13_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_rise_whole_joint_14_angles[46] = {
#include "shared/golem_clips/golem_motion_strike_a_rise_whole_joint_14_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_rise_whole_joint_15_angles[46] = {
#include "shared/golem_clips/golem_motion_strike_a_rise_whole_joint_15_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_rise_whole_joint_16_angles[46] = {
#include "shared/golem_clips/golem_motion_strike_a_rise_whole_joint_16_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_rise_whole_joint_17_angles[46] = {
#include "shared/golem_clips/golem_motion_strike_a_rise_whole_joint_17_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_rise_whole_joint_18_angles[46] = {
#include "shared/golem_clips/golem_motion_strike_a_rise_whole_joint_18_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_rise_whole_joint_19_angles[46] = {
#include "shared/golem_clips/golem_motion_strike_a_rise_whole_joint_19_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_rise_whole_joint_20_angles[46] = {
#include "shared/golem_clips/golem_motion_strike_a_rise_whole_joint_20_angles.inc"
};
static NJS_MDATA3 golem_motion_strike_a_rise_whole_tracks[21] = {
#include "shared/golem_clips/golem_motion_strike_a_rise_whole_tracks.inc"
};
NJS_MOTION golem_motion_strike_a_rise_whole = {
#include "shared/golem_clips/golem_motion_strike_a_rise_whole.inc"
};
static NJS_MKEY_A golem_motion_strike_a_second_blow_whole_joint_0_angles[2] = {
#include "shared/golem_clips/golem_motion_strike_a_second_blow_whole_joint_0_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_second_blow_whole_joint_1_angles[23] = {
#include "shared/golem_clips/golem_motion_strike_a_second_blow_whole_joint_1_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_second_blow_whole_joint_2_angles[23] = {
#include "shared/golem_clips/golem_motion_strike_a_second_blow_whole_joint_2_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_second_blow_whole_joint_3_angles[23] = {
#include "shared/golem_clips/golem_motion_strike_a_second_blow_whole_joint_3_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_second_blow_whole_joint_4_angles[2] = {
#include "shared/golem_clips/golem_motion_strike_a_second_blow_whole_joint_4_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_second_blow_whole_joint_5_angles[23] = {
#include "shared/golem_clips/golem_motion_strike_a_second_blow_whole_joint_5_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_second_blow_whole_joint_6_angles[23] = {
#include "shared/golem_clips/golem_motion_strike_a_second_blow_whole_joint_6_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_second_blow_whole_joint_7_angles[23] = {
#include "shared/golem_clips/golem_motion_strike_a_second_blow_whole_joint_7_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_second_blow_whole_joint_8_angles[23] = {
#include "shared/golem_clips/golem_motion_strike_a_second_blow_whole_joint_8_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_second_blow_whole_joint_9_angles[23] = {
#include "shared/golem_clips/golem_motion_strike_a_second_blow_whole_joint_9_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_second_blow_whole_joint_10_angles[23] =
    {
#include "shared/golem_clips/golem_motion_strike_a_second_blow_whole_joint_10_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_second_blow_whole_joint_11_angles[23] =
    {
#include "shared/golem_clips/golem_motion_strike_a_second_blow_whole_joint_11_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_second_blow_whole_joint_12_angles[23] =
    {
#include "shared/golem_clips/golem_motion_strike_a_second_blow_whole_joint_12_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_second_blow_whole_joint_13_angles[21] =
    {
#include "shared/golem_clips/golem_motion_strike_a_second_blow_whole_joint_13_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_second_blow_whole_joint_14_angles[23] =
    {
#include "shared/golem_clips/golem_motion_strike_a_second_blow_whole_joint_14_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_second_blow_whole_joint_15_angles[23] =
    {
#include "shared/golem_clips/golem_motion_strike_a_second_blow_whole_joint_15_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_second_blow_whole_joint_16_angles[18] =
    {
#include "shared/golem_clips/golem_motion_strike_a_second_blow_whole_joint_16_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_second_blow_whole_joint_17_angles[20] =
    {
#include "shared/golem_clips/golem_motion_strike_a_second_blow_whole_joint_17_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_second_blow_whole_joint_18_angles[16] =
    {
#include "shared/golem_clips/golem_motion_strike_a_second_blow_whole_joint_18_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_second_blow_whole_joint_19_angles[16] =
    {
#include "shared/golem_clips/golem_motion_strike_a_second_blow_whole_joint_19_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_second_blow_whole_joint_20_angles[23] =
    {
#include "shared/golem_clips/golem_motion_strike_a_second_blow_whole_joint_20_angles.inc"
};
static NJS_MDATA3 golem_motion_strike_a_second_blow_whole_tracks[21] = {
#include "shared/golem_clips/golem_motion_strike_a_second_blow_whole_tracks.inc"
};
NJS_MOTION golem_motion_strike_a_second_blow_whole = {
#include "shared/golem_clips/golem_motion_strike_a_second_blow_whole.inc"
};
static NJS_MKEY_A golem_motion_strike_a_second_rise_whole_joint_0_angles[2] = {
#include "shared/golem_clips/golem_motion_strike_a_second_rise_whole_joint_0_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_second_rise_whole_joint_1_angles[18] = {
#include "shared/golem_clips/golem_motion_strike_a_second_rise_whole_joint_1_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_second_rise_whole_joint_2_angles[14] = {
#include "shared/golem_clips/golem_motion_strike_a_second_rise_whole_joint_2_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_second_rise_whole_joint_3_angles[23] = {
#include "shared/golem_clips/golem_motion_strike_a_second_rise_whole_joint_3_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_second_rise_whole_joint_4_angles[2] = {
#include "shared/golem_clips/golem_motion_strike_a_second_rise_whole_joint_4_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_second_rise_whole_joint_5_angles[30] = {
#include "shared/golem_clips/golem_motion_strike_a_second_rise_whole_joint_5_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_second_rise_whole_joint_6_angles[30] = {
#include "shared/golem_clips/golem_motion_strike_a_second_rise_whole_joint_6_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_second_rise_whole_joint_7_angles[30] = {
#include "shared/golem_clips/golem_motion_strike_a_second_rise_whole_joint_7_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_second_rise_whole_joint_8_angles[30] = {
#include "shared/golem_clips/golem_motion_strike_a_second_rise_whole_joint_8_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_second_rise_whole_joint_9_angles[27] = {
#include "shared/golem_clips/golem_motion_strike_a_second_rise_whole_joint_9_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_second_rise_whole_joint_10_angles[29] =
    {
#include "shared/golem_clips/golem_motion_strike_a_second_rise_whole_joint_10_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_second_rise_whole_joint_11_angles[29] =
    {
#include "shared/golem_clips/golem_motion_strike_a_second_rise_whole_joint_11_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_second_rise_whole_joint_12_angles[30] =
    {
#include "shared/golem_clips/golem_motion_strike_a_second_rise_whole_joint_12_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_second_rise_whole_joint_13_angles[30] =
    {
#include "shared/golem_clips/golem_motion_strike_a_second_rise_whole_joint_13_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_second_rise_whole_joint_14_angles[30] =
    {
#include "shared/golem_clips/golem_motion_strike_a_second_rise_whole_joint_14_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_second_rise_whole_joint_15_angles[30] =
    {
#include "shared/golem_clips/golem_motion_strike_a_second_rise_whole_joint_15_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_second_rise_whole_joint_16_angles[30] =
    {
#include "shared/golem_clips/golem_motion_strike_a_second_rise_whole_joint_16_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_second_rise_whole_joint_17_angles[30] =
    {
#include "shared/golem_clips/golem_motion_strike_a_second_rise_whole_joint_17_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_second_rise_whole_joint_18_angles[30] =
    {
#include "shared/golem_clips/golem_motion_strike_a_second_rise_whole_joint_18_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_second_rise_whole_joint_19_angles[30] =
    {
#include "shared/golem_clips/golem_motion_strike_a_second_rise_whole_joint_19_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_second_rise_whole_joint_20_angles[30] =
    {
#include "shared/golem_clips/golem_motion_strike_a_second_rise_whole_joint_20_angles.inc"
};
static NJS_MDATA3 golem_motion_strike_a_second_rise_whole_tracks[21] = {
#include "shared/golem_clips/golem_motion_strike_a_second_rise_whole_tracks.inc"
};
NJS_MOTION golem_motion_strike_a_second_rise_whole = {
#include "shared/golem_clips/golem_motion_strike_a_second_rise_whole.inc"
};
static NJS_MKEY_A golem_motion_strike_a_blow_sunk_joint_0_angles[2] = {
#include "shared/golem_clips/golem_motion_strike_a_blow_sunk_joint_0_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_blow_sunk_joint_1_angles[23] = {
#include "shared/golem_clips/golem_motion_strike_a_blow_sunk_joint_1_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_blow_sunk_joint_2_angles[23] = {
#include "shared/golem_clips/golem_motion_strike_a_blow_sunk_joint_2_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_blow_sunk_joint_3_angles[23] = {
#include "shared/golem_clips/golem_motion_strike_a_blow_sunk_joint_3_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_blow_sunk_joint_4_angles[2] = {
#include "shared/golem_clips/golem_motion_strike_a_blow_sunk_joint_4_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_blow_sunk_joint_5_angles[23] = {
#include "shared/golem_clips/golem_motion_strike_a_blow_sunk_joint_5_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_blow_sunk_joint_6_angles[23] = {
#include "shared/golem_clips/golem_motion_strike_a_blow_sunk_joint_6_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_blow_sunk_joint_7_angles[23] = {
#include "shared/golem_clips/golem_motion_strike_a_blow_sunk_joint_7_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_blow_sunk_joint_8_angles[23] = {
#include "shared/golem_clips/golem_motion_strike_a_blow_sunk_joint_8_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_blow_sunk_joint_9_angles[23] = {
#include "shared/golem_clips/golem_motion_strike_a_blow_sunk_joint_9_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_blow_sunk_joint_10_angles[15] = {
#include "shared/golem_clips/golem_motion_strike_a_blow_sunk_joint_10_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_blow_sunk_joint_11_angles[16] = {
#include "shared/golem_clips/golem_motion_strike_a_blow_sunk_joint_11_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_blow_sunk_joint_12_angles[18] = {
#include "shared/golem_clips/golem_motion_strike_a_blow_sunk_joint_12_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_blow_sunk_joint_13_angles[23] = {
#include "shared/golem_clips/golem_motion_strike_a_blow_sunk_joint_13_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_blow_sunk_joint_14_angles[23] = {
#include "shared/golem_clips/golem_motion_strike_a_blow_sunk_joint_14_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_blow_sunk_joint_15_angles[23] = {
#include "shared/golem_clips/golem_motion_strike_a_blow_sunk_joint_15_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_blow_sunk_joint_16_angles[23] = {
#include "shared/golem_clips/golem_motion_strike_a_blow_sunk_joint_16_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_blow_sunk_joint_17_angles[23] = {
#include "shared/golem_clips/golem_motion_strike_a_blow_sunk_joint_17_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_blow_sunk_joint_18_angles[23] = {
#include "shared/golem_clips/golem_motion_strike_a_blow_sunk_joint_18_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_blow_sunk_joint_19_angles[23] = {
#include "shared/golem_clips/golem_motion_strike_a_blow_sunk_joint_19_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_blow_sunk_joint_20_angles[23] = {
#include "shared/golem_clips/golem_motion_strike_a_blow_sunk_joint_20_angles.inc"
};
static NJS_MDATA3 golem_motion_strike_a_blow_sunk_tracks[21] = {
#include "shared/golem_clips/golem_motion_strike_a_blow_sunk_tracks.inc"
};
NJS_MOTION golem_motion_strike_a_blow_sunk = {
#include "shared/golem_clips/golem_motion_strike_a_blow_sunk.inc"
};
static NJS_MKEY_A golem_motion_strike_a_rise_sunk_joint_0_angles[2] = {
#include "shared/golem_clips/golem_motion_strike_a_rise_sunk_joint_0_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_rise_sunk_joint_1_angles[34] = {
#include "shared/golem_clips/golem_motion_strike_a_rise_sunk_joint_1_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_rise_sunk_joint_2_angles[6] = {
#include "shared/golem_clips/golem_motion_strike_a_rise_sunk_joint_2_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_rise_sunk_joint_3_angles[15] = {
#include "shared/golem_clips/golem_motion_strike_a_rise_sunk_joint_3_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_rise_sunk_joint_4_angles[2] = {
#include "shared/golem_clips/golem_motion_strike_a_rise_sunk_joint_4_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_rise_sunk_joint_5_angles[46] = {
#include "shared/golem_clips/golem_motion_strike_a_rise_sunk_joint_5_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_rise_sunk_joint_6_angles[44] = {
#include "shared/golem_clips/golem_motion_strike_a_rise_sunk_joint_6_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_rise_sunk_joint_7_angles[46] = {
#include "shared/golem_clips/golem_motion_strike_a_rise_sunk_joint_7_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_rise_sunk_joint_8_angles[46] = {
#include "shared/golem_clips/golem_motion_strike_a_rise_sunk_joint_8_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_rise_sunk_joint_9_angles[46] = {
#include "shared/golem_clips/golem_motion_strike_a_rise_sunk_joint_9_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_rise_sunk_joint_10_angles[46] = {
#include "shared/golem_clips/golem_motion_strike_a_rise_sunk_joint_10_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_rise_sunk_joint_11_angles[42] = {
#include "shared/golem_clips/golem_motion_strike_a_rise_sunk_joint_11_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_rise_sunk_joint_12_angles[46] = {
#include "shared/golem_clips/golem_motion_strike_a_rise_sunk_joint_12_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_rise_sunk_joint_13_angles[46] = {
#include "shared/golem_clips/golem_motion_strike_a_rise_sunk_joint_13_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_rise_sunk_joint_14_angles[46] = {
#include "shared/golem_clips/golem_motion_strike_a_rise_sunk_joint_14_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_rise_sunk_joint_15_angles[46] = {
#include "shared/golem_clips/golem_motion_strike_a_rise_sunk_joint_15_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_rise_sunk_joint_16_angles[46] = {
#include "shared/golem_clips/golem_motion_strike_a_rise_sunk_joint_16_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_rise_sunk_joint_17_angles[46] = {
#include "shared/golem_clips/golem_motion_strike_a_rise_sunk_joint_17_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_rise_sunk_joint_18_angles[46] = {
#include "shared/golem_clips/golem_motion_strike_a_rise_sunk_joint_18_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_rise_sunk_joint_19_angles[46] = {
#include "shared/golem_clips/golem_motion_strike_a_rise_sunk_joint_19_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_rise_sunk_joint_20_angles[46] = {
#include "shared/golem_clips/golem_motion_strike_a_rise_sunk_joint_20_angles.inc"
};
static NJS_MDATA3 golem_motion_strike_a_rise_sunk_tracks[21] = {
#include "shared/golem_clips/golem_motion_strike_a_rise_sunk_tracks.inc"
};
NJS_MOTION golem_motion_strike_a_rise_sunk = {
#include "shared/golem_clips/golem_motion_strike_a_rise_sunk.inc"
};
static NJS_MKEY_A golem_motion_strike_a_second_blow_sunk_joint_0_angles[2] = {
#include "shared/golem_clips/golem_motion_strike_a_second_blow_sunk_joint_0_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_second_blow_sunk_joint_1_angles[23] = {
#include "shared/golem_clips/golem_motion_strike_a_second_blow_sunk_joint_1_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_second_blow_sunk_joint_2_angles[23] = {
#include "shared/golem_clips/golem_motion_strike_a_second_blow_sunk_joint_2_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_second_blow_sunk_joint_3_angles[23] = {
#include "shared/golem_clips/golem_motion_strike_a_second_blow_sunk_joint_3_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_second_blow_sunk_joint_4_angles[2] = {
#include "shared/golem_clips/golem_motion_strike_a_second_blow_sunk_joint_4_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_second_blow_sunk_joint_5_angles[23] = {
#include "shared/golem_clips/golem_motion_strike_a_second_blow_sunk_joint_5_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_second_blow_sunk_joint_6_angles[23] = {
#include "shared/golem_clips/golem_motion_strike_a_second_blow_sunk_joint_6_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_second_blow_sunk_joint_7_angles[23] = {
#include "shared/golem_clips/golem_motion_strike_a_second_blow_sunk_joint_7_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_second_blow_sunk_joint_8_angles[23] = {
#include "shared/golem_clips/golem_motion_strike_a_second_blow_sunk_joint_8_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_second_blow_sunk_joint_9_angles[23] = {
#include "shared/golem_clips/golem_motion_strike_a_second_blow_sunk_joint_9_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_second_blow_sunk_joint_10_angles[23] = {
#include "shared/golem_clips/golem_motion_strike_a_second_blow_sunk_joint_10_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_second_blow_sunk_joint_11_angles[23] = {
#include "shared/golem_clips/golem_motion_strike_a_second_blow_sunk_joint_11_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_second_blow_sunk_joint_12_angles[23] = {
#include "shared/golem_clips/golem_motion_strike_a_second_blow_sunk_joint_12_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_second_blow_sunk_joint_13_angles[23] = {
#include "shared/golem_clips/golem_motion_strike_a_second_blow_sunk_joint_13_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_second_blow_sunk_joint_14_angles[23] = {
#include "shared/golem_clips/golem_motion_strike_a_second_blow_sunk_joint_14_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_second_blow_sunk_joint_15_angles[23] = {
#include "shared/golem_clips/golem_motion_strike_a_second_blow_sunk_joint_15_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_second_blow_sunk_joint_16_angles[18] = {
#include "shared/golem_clips/golem_motion_strike_a_second_blow_sunk_joint_16_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_second_blow_sunk_joint_17_angles[20] = {
#include "shared/golem_clips/golem_motion_strike_a_second_blow_sunk_joint_17_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_second_blow_sunk_joint_18_angles[16] = {
#include "shared/golem_clips/golem_motion_strike_a_second_blow_sunk_joint_18_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_second_blow_sunk_joint_19_angles[16] = {
#include "shared/golem_clips/golem_motion_strike_a_second_blow_sunk_joint_19_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_second_blow_sunk_joint_20_angles[23] = {
#include "shared/golem_clips/golem_motion_strike_a_second_blow_sunk_joint_20_angles.inc"
};
static NJS_MDATA3 golem_motion_strike_a_second_blow_sunk_tracks[21] = {
#include "shared/golem_clips/golem_motion_strike_a_second_blow_sunk_tracks.inc"
};
NJS_MOTION golem_motion_strike_a_second_blow_sunk = {
#include "shared/golem_clips/golem_motion_strike_a_second_blow_sunk.inc"
};
static NJS_MKEY_A golem_motion_strike_a_second_rise_sunk_joint_0_angles[2] = {
#include "shared/golem_clips/golem_motion_strike_a_second_rise_sunk_joint_0_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_second_rise_sunk_joint_1_angles[18] = {
#include "shared/golem_clips/golem_motion_strike_a_second_rise_sunk_joint_1_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_second_rise_sunk_joint_2_angles[14] = {
#include "shared/golem_clips/golem_motion_strike_a_second_rise_sunk_joint_2_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_second_rise_sunk_joint_3_angles[23] = {
#include "shared/golem_clips/golem_motion_strike_a_second_rise_sunk_joint_3_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_second_rise_sunk_joint_4_angles[2] = {
#include "shared/golem_clips/golem_motion_strike_a_second_rise_sunk_joint_4_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_second_rise_sunk_joint_5_angles[30] = {
#include "shared/golem_clips/golem_motion_strike_a_second_rise_sunk_joint_5_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_second_rise_sunk_joint_6_angles[30] = {
#include "shared/golem_clips/golem_motion_strike_a_second_rise_sunk_joint_6_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_second_rise_sunk_joint_7_angles[30] = {
#include "shared/golem_clips/golem_motion_strike_a_second_rise_sunk_joint_7_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_second_rise_sunk_joint_8_angles[30] = {
#include "shared/golem_clips/golem_motion_strike_a_second_rise_sunk_joint_8_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_second_rise_sunk_joint_9_angles[27] = {
#include "shared/golem_clips/golem_motion_strike_a_second_rise_sunk_joint_9_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_second_rise_sunk_joint_10_angles[29] = {
#include "shared/golem_clips/golem_motion_strike_a_second_rise_sunk_joint_10_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_second_rise_sunk_joint_11_angles[29] = {
#include "shared/golem_clips/golem_motion_strike_a_second_rise_sunk_joint_11_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_second_rise_sunk_joint_12_angles[30] = {
#include "shared/golem_clips/golem_motion_strike_a_second_rise_sunk_joint_12_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_second_rise_sunk_joint_13_angles[30] = {
#include "shared/golem_clips/golem_motion_strike_a_second_rise_sunk_joint_13_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_second_rise_sunk_joint_14_angles[30] = {
#include "shared/golem_clips/golem_motion_strike_a_second_rise_sunk_joint_14_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_second_rise_sunk_joint_15_angles[30] = {
#include "shared/golem_clips/golem_motion_strike_a_second_rise_sunk_joint_15_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_second_rise_sunk_joint_16_angles[30] = {
#include "shared/golem_clips/golem_motion_strike_a_second_rise_sunk_joint_16_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_second_rise_sunk_joint_17_angles[30] = {
#include "shared/golem_clips/golem_motion_strike_a_second_rise_sunk_joint_17_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_second_rise_sunk_joint_18_angles[30] = {
#include "shared/golem_clips/golem_motion_strike_a_second_rise_sunk_joint_18_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_second_rise_sunk_joint_19_angles[30] = {
#include "shared/golem_clips/golem_motion_strike_a_second_rise_sunk_joint_19_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_second_rise_sunk_joint_20_angles[30] = {
#include "shared/golem_clips/golem_motion_strike_a_second_rise_sunk_joint_20_angles.inc"
};
static NJS_MDATA3 golem_motion_strike_a_second_rise_sunk_tracks[21] = {
#include "shared/golem_clips/golem_motion_strike_a_second_rise_sunk_tracks.inc"
};
NJS_MOTION golem_motion_strike_a_second_rise_sunk = {
#include "shared/golem_clips/golem_motion_strike_a_second_rise_sunk.inc"
};
static NJS_MKEY_A golem_motion_strike_a_blow_sunk_twice_joint_0_angles[2] = {
#include "shared/golem_clips/golem_motion_strike_a_blow_sunk_twice_joint_0_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_blow_sunk_twice_joint_1_angles[23] = {
#include "shared/golem_clips/golem_motion_strike_a_blow_sunk_twice_joint_1_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_blow_sunk_twice_joint_2_angles[23] = {
#include "shared/golem_clips/golem_motion_strike_a_blow_sunk_twice_joint_2_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_blow_sunk_twice_joint_3_angles[23] = {
#include "shared/golem_clips/golem_motion_strike_a_blow_sunk_twice_joint_3_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_blow_sunk_twice_joint_4_angles[2] = {
#include "shared/golem_clips/golem_motion_strike_a_blow_sunk_twice_joint_4_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_blow_sunk_twice_joint_5_angles[23] = {
#include "shared/golem_clips/golem_motion_strike_a_blow_sunk_twice_joint_5_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_blow_sunk_twice_joint_6_angles[19] = {
#include "shared/golem_clips/golem_motion_strike_a_blow_sunk_twice_joint_6_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_blow_sunk_twice_joint_7_angles[23] = {
#include "shared/golem_clips/golem_motion_strike_a_blow_sunk_twice_joint_7_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_blow_sunk_twice_joint_8_angles[23] = {
#include "shared/golem_clips/golem_motion_strike_a_blow_sunk_twice_joint_8_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_blow_sunk_twice_joint_9_angles[23] = {
#include "shared/golem_clips/golem_motion_strike_a_blow_sunk_twice_joint_9_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_blow_sunk_twice_joint_10_angles[15] = {
#include "shared/golem_clips/golem_motion_strike_a_blow_sunk_twice_joint_10_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_blow_sunk_twice_joint_11_angles[16] = {
#include "shared/golem_clips/golem_motion_strike_a_blow_sunk_twice_joint_11_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_blow_sunk_twice_joint_12_angles[18] = {
#include "shared/golem_clips/golem_motion_strike_a_blow_sunk_twice_joint_12_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_blow_sunk_twice_joint_13_angles[23] = {
#include "shared/golem_clips/golem_motion_strike_a_blow_sunk_twice_joint_13_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_blow_sunk_twice_joint_14_angles[23] = {
#include "shared/golem_clips/golem_motion_strike_a_blow_sunk_twice_joint_14_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_blow_sunk_twice_joint_15_angles[23] = {
#include "shared/golem_clips/golem_motion_strike_a_blow_sunk_twice_joint_15_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_blow_sunk_twice_joint_16_angles[23] = {
#include "shared/golem_clips/golem_motion_strike_a_blow_sunk_twice_joint_16_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_blow_sunk_twice_joint_17_angles[23] = {
#include "shared/golem_clips/golem_motion_strike_a_blow_sunk_twice_joint_17_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_blow_sunk_twice_joint_18_angles[23] = {
#include "shared/golem_clips/golem_motion_strike_a_blow_sunk_twice_joint_18_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_blow_sunk_twice_joint_19_angles[23] = {
#include "shared/golem_clips/golem_motion_strike_a_blow_sunk_twice_joint_19_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_blow_sunk_twice_joint_20_angles[23] = {
#include "shared/golem_clips/golem_motion_strike_a_blow_sunk_twice_joint_20_angles.inc"
};
static NJS_MDATA3 golem_motion_strike_a_blow_sunk_twice_tracks[21] = {
#include "shared/golem_clips/golem_motion_strike_a_blow_sunk_twice_tracks.inc"
};
NJS_MOTION golem_motion_strike_a_blow_sunk_twice = {
#include "shared/golem_clips/golem_motion_strike_a_blow_sunk_twice.inc"
};
static NJS_MKEY_A golem_motion_strike_a_rise_sunk_twice_joint_0_angles[2] = {
#include "shared/golem_clips/golem_motion_strike_a_rise_sunk_twice_joint_0_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_rise_sunk_twice_joint_1_angles[34] = {
#include "shared/golem_clips/golem_motion_strike_a_rise_sunk_twice_joint_1_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_rise_sunk_twice_joint_2_angles[6] = {
#include "shared/golem_clips/golem_motion_strike_a_rise_sunk_twice_joint_2_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_rise_sunk_twice_joint_3_angles[15] = {
#include "shared/golem_clips/golem_motion_strike_a_rise_sunk_twice_joint_3_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_rise_sunk_twice_joint_4_angles[2] = {
#include "shared/golem_clips/golem_motion_strike_a_rise_sunk_twice_joint_4_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_rise_sunk_twice_joint_5_angles[46] = {
#include "shared/golem_clips/golem_motion_strike_a_rise_sunk_twice_joint_5_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_rise_sunk_twice_joint_6_angles[44] = {
#include "shared/golem_clips/golem_motion_strike_a_rise_sunk_twice_joint_6_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_rise_sunk_twice_joint_7_angles[46] = {
#include "shared/golem_clips/golem_motion_strike_a_rise_sunk_twice_joint_7_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_rise_sunk_twice_joint_8_angles[46] = {
#include "shared/golem_clips/golem_motion_strike_a_rise_sunk_twice_joint_8_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_rise_sunk_twice_joint_9_angles[46] = {
#include "shared/golem_clips/golem_motion_strike_a_rise_sunk_twice_joint_9_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_rise_sunk_twice_joint_10_angles[46] = {
#include "shared/golem_clips/golem_motion_strike_a_rise_sunk_twice_joint_10_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_rise_sunk_twice_joint_11_angles[42] = {
#include "shared/golem_clips/golem_motion_strike_a_rise_sunk_twice_joint_11_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_rise_sunk_twice_joint_12_angles[46] = {
#include "shared/golem_clips/golem_motion_strike_a_rise_sunk_twice_joint_12_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_rise_sunk_twice_joint_13_angles[46] = {
#include "shared/golem_clips/golem_motion_strike_a_rise_sunk_twice_joint_13_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_rise_sunk_twice_joint_14_angles[46] = {
#include "shared/golem_clips/golem_motion_strike_a_rise_sunk_twice_joint_14_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_rise_sunk_twice_joint_15_angles[46] = {
#include "shared/golem_clips/golem_motion_strike_a_rise_sunk_twice_joint_15_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_rise_sunk_twice_joint_16_angles[46] = {
#include "shared/golem_clips/golem_motion_strike_a_rise_sunk_twice_joint_16_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_rise_sunk_twice_joint_17_angles[46] = {
#include "shared/golem_clips/golem_motion_strike_a_rise_sunk_twice_joint_17_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_rise_sunk_twice_joint_18_angles[46] = {
#include "shared/golem_clips/golem_motion_strike_a_rise_sunk_twice_joint_18_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_rise_sunk_twice_joint_19_angles[46] = {
#include "shared/golem_clips/golem_motion_strike_a_rise_sunk_twice_joint_19_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_a_rise_sunk_twice_joint_20_angles[46] = {
#include "shared/golem_clips/golem_motion_strike_a_rise_sunk_twice_joint_20_angles.inc"
};
static NJS_MDATA3 golem_motion_strike_a_rise_sunk_twice_tracks[21] = {
#include "shared/golem_clips/golem_motion_strike_a_rise_sunk_twice_tracks.inc"
};
NJS_MOTION golem_motion_strike_a_rise_sunk_twice = {
#include "shared/golem_clips/golem_motion_strike_a_rise_sunk_twice.inc"
};
static NJS_MKEY_A
    golem_motion_strike_a_second_blow_sunk_twice_joint_0_angles[2] = {
#include "shared/golem_clips/golem_motion_strike_a_second_blow_sunk_twice_joint_0_angles.inc"
};
static NJS_MKEY_A
    golem_motion_strike_a_second_blow_sunk_twice_joint_1_angles[23] = {
#include "shared/golem_clips/golem_motion_strike_a_second_blow_sunk_twice_joint_1_angles.inc"
};
static NJS_MKEY_A
    golem_motion_strike_a_second_blow_sunk_twice_joint_2_angles[23] = {
#include "shared/golem_clips/golem_motion_strike_a_second_blow_sunk_twice_joint_2_angles.inc"
};
static NJS_MKEY_A
    golem_motion_strike_a_second_blow_sunk_twice_joint_3_angles[23] = {
#include "shared/golem_clips/golem_motion_strike_a_second_blow_sunk_twice_joint_3_angles.inc"
};
static NJS_MKEY_A
    golem_motion_strike_a_second_blow_sunk_twice_joint_4_angles[2] = {
#include "shared/golem_clips/golem_motion_strike_a_second_blow_sunk_twice_joint_4_angles.inc"
};
static NJS_MKEY_A
    golem_motion_strike_a_second_blow_sunk_twice_joint_5_angles[23] = {
#include "shared/golem_clips/golem_motion_strike_a_second_blow_sunk_twice_joint_5_angles.inc"
};
static NJS_MKEY_A
    golem_motion_strike_a_second_blow_sunk_twice_joint_6_angles[23] = {
#include "shared/golem_clips/golem_motion_strike_a_second_blow_sunk_twice_joint_6_angles.inc"
};
static NJS_MKEY_A
    golem_motion_strike_a_second_blow_sunk_twice_joint_7_angles[23] = {
#include "shared/golem_clips/golem_motion_strike_a_second_blow_sunk_twice_joint_7_angles.inc"
};
static NJS_MKEY_A
    golem_motion_strike_a_second_blow_sunk_twice_joint_8_angles[23] = {
#include "shared/golem_clips/golem_motion_strike_a_second_blow_sunk_twice_joint_8_angles.inc"
};
static NJS_MKEY_A
    golem_motion_strike_a_second_blow_sunk_twice_joint_9_angles[23] = {
#include "shared/golem_clips/golem_motion_strike_a_second_blow_sunk_twice_joint_9_angles.inc"
};
static NJS_MKEY_A
    golem_motion_strike_a_second_blow_sunk_twice_joint_10_angles[23] = {
#include "shared/golem_clips/golem_motion_strike_a_second_blow_sunk_twice_joint_10_angles.inc"
};
static NJS_MKEY_A
    golem_motion_strike_a_second_blow_sunk_twice_joint_11_angles[23] = {
#include "shared/golem_clips/golem_motion_strike_a_second_blow_sunk_twice_joint_11_angles.inc"
};
static NJS_MKEY_A
    golem_motion_strike_a_second_blow_sunk_twice_joint_12_angles[23] = {
#include "shared/golem_clips/golem_motion_strike_a_second_blow_sunk_twice_joint_12_angles.inc"
};
static NJS_MKEY_A
    golem_motion_strike_a_second_blow_sunk_twice_joint_13_angles[18] = {
#include "shared/golem_clips/golem_motion_strike_a_second_blow_sunk_twice_joint_13_angles.inc"
};
static NJS_MKEY_A
    golem_motion_strike_a_second_blow_sunk_twice_joint_14_angles[23] = {
#include "shared/golem_clips/golem_motion_strike_a_second_blow_sunk_twice_joint_14_angles.inc"
};
static NJS_MKEY_A
    golem_motion_strike_a_second_blow_sunk_twice_joint_15_angles[23] = {
#include "shared/golem_clips/golem_motion_strike_a_second_blow_sunk_twice_joint_15_angles.inc"
};
static NJS_MKEY_A
    golem_motion_strike_a_second_blow_sunk_twice_joint_16_angles[18] = {
#include "shared/golem_clips/golem_motion_strike_a_second_blow_sunk_twice_joint_16_angles.inc"
};
static NJS_MKEY_A
    golem_motion_strike_a_second_blow_sunk_twice_joint_17_angles[20] = {
#include "shared/golem_clips/golem_motion_strike_a_second_blow_sunk_twice_joint_17_angles.inc"
};
static NJS_MKEY_A
    golem_motion_strike_a_second_blow_sunk_twice_joint_18_angles[16] = {
#include "shared/golem_clips/golem_motion_strike_a_second_blow_sunk_twice_joint_18_angles.inc"
};
static NJS_MKEY_A
    golem_motion_strike_a_second_blow_sunk_twice_joint_19_angles[16] = {
#include "shared/golem_clips/golem_motion_strike_a_second_blow_sunk_twice_joint_19_angles.inc"
};
static NJS_MKEY_A
    golem_motion_strike_a_second_blow_sunk_twice_joint_20_angles[23] = {
#include "shared/golem_clips/golem_motion_strike_a_second_blow_sunk_twice_joint_20_angles.inc"
};
static NJS_MDATA3 golem_motion_strike_a_second_blow_sunk_twice_tracks[21] = {
#include "shared/golem_clips/golem_motion_strike_a_second_blow_sunk_twice_tracks.inc"
};
NJS_MOTION golem_motion_strike_a_second_blow_sunk_twice = {
#include "shared/golem_clips/golem_motion_strike_a_second_blow_sunk_twice.inc"
};
static NJS_MKEY_A
    golem_motion_strike_a_second_rise_sunk_twice_joint_0_angles[2] = {
#include "shared/golem_clips/golem_motion_strike_a_second_rise_sunk_twice_joint_0_angles.inc"
};
static NJS_MKEY_A
    golem_motion_strike_a_second_rise_sunk_twice_joint_1_angles[18] = {
#include "shared/golem_clips/golem_motion_strike_a_second_rise_sunk_twice_joint_1_angles.inc"
};
static NJS_MKEY_A
    golem_motion_strike_a_second_rise_sunk_twice_joint_2_angles[14] = {
#include "shared/golem_clips/golem_motion_strike_a_second_rise_sunk_twice_joint_2_angles.inc"
};
static NJS_MKEY_A
    golem_motion_strike_a_second_rise_sunk_twice_joint_3_angles[23] = {
#include "shared/golem_clips/golem_motion_strike_a_second_rise_sunk_twice_joint_3_angles.inc"
};
static NJS_MKEY_A
    golem_motion_strike_a_second_rise_sunk_twice_joint_4_angles[2] = {
#include "shared/golem_clips/golem_motion_strike_a_second_rise_sunk_twice_joint_4_angles.inc"
};
static NJS_MKEY_A
    golem_motion_strike_a_second_rise_sunk_twice_joint_5_angles[30] = {
#include "shared/golem_clips/golem_motion_strike_a_second_rise_sunk_twice_joint_5_angles.inc"
};
static NJS_MKEY_A
    golem_motion_strike_a_second_rise_sunk_twice_joint_6_angles[30] = {
#include "shared/golem_clips/golem_motion_strike_a_second_rise_sunk_twice_joint_6_angles.inc"
};
static NJS_MKEY_A
    golem_motion_strike_a_second_rise_sunk_twice_joint_7_angles[30] = {
#include "shared/golem_clips/golem_motion_strike_a_second_rise_sunk_twice_joint_7_angles.inc"
};
static NJS_MKEY_A
    golem_motion_strike_a_second_rise_sunk_twice_joint_8_angles[30] = {
#include "shared/golem_clips/golem_motion_strike_a_second_rise_sunk_twice_joint_8_angles.inc"
};
static NJS_MKEY_A
    golem_motion_strike_a_second_rise_sunk_twice_joint_9_angles[27] = {
#include "shared/golem_clips/golem_motion_strike_a_second_rise_sunk_twice_joint_9_angles.inc"
};
static NJS_MKEY_A
    golem_motion_strike_a_second_rise_sunk_twice_joint_10_angles[29] = {
#include "shared/golem_clips/golem_motion_strike_a_second_rise_sunk_twice_joint_10_angles.inc"
};
static NJS_MKEY_A
    golem_motion_strike_a_second_rise_sunk_twice_joint_11_angles[29] = {
#include "shared/golem_clips/golem_motion_strike_a_second_rise_sunk_twice_joint_11_angles.inc"
};
static NJS_MKEY_A
    golem_motion_strike_a_second_rise_sunk_twice_joint_12_angles[30] = {
#include "shared/golem_clips/golem_motion_strike_a_second_rise_sunk_twice_joint_12_angles.inc"
};
static NJS_MKEY_A
    golem_motion_strike_a_second_rise_sunk_twice_joint_13_angles[30] = {
#include "shared/golem_clips/golem_motion_strike_a_second_rise_sunk_twice_joint_13_angles.inc"
};
static NJS_MKEY_A
    golem_motion_strike_a_second_rise_sunk_twice_joint_14_angles[30] = {
#include "shared/golem_clips/golem_motion_strike_a_second_rise_sunk_twice_joint_14_angles.inc"
};
static NJS_MKEY_A
    golem_motion_strike_a_second_rise_sunk_twice_joint_15_angles[30] = {
#include "shared/golem_clips/golem_motion_strike_a_second_rise_sunk_twice_joint_15_angles.inc"
};
static NJS_MKEY_A
    golem_motion_strike_a_second_rise_sunk_twice_joint_16_angles[30] = {
#include "shared/golem_clips/golem_motion_strike_a_second_rise_sunk_twice_joint_16_angles.inc"
};
static NJS_MKEY_A
    golem_motion_strike_a_second_rise_sunk_twice_joint_17_angles[30] = {
#include "shared/golem_clips/golem_motion_strike_a_second_rise_sunk_twice_joint_17_angles.inc"
};
static NJS_MKEY_A
    golem_motion_strike_a_second_rise_sunk_twice_joint_18_angles[30] = {
#include "shared/golem_clips/golem_motion_strike_a_second_rise_sunk_twice_joint_18_angles.inc"
};
static NJS_MKEY_A
    golem_motion_strike_a_second_rise_sunk_twice_joint_19_angles[30] = {
#include "shared/golem_clips/golem_motion_strike_a_second_rise_sunk_twice_joint_19_angles.inc"
};
static NJS_MKEY_A
    golem_motion_strike_a_second_rise_sunk_twice_joint_20_angles[30] = {
#include "shared/golem_clips/golem_motion_strike_a_second_rise_sunk_twice_joint_20_angles.inc"
};
static NJS_MDATA3 golem_motion_strike_a_second_rise_sunk_twice_tracks[21] = {
#include "shared/golem_clips/golem_motion_strike_a_second_rise_sunk_twice_tracks.inc"
};
NJS_MOTION golem_motion_strike_a_second_rise_sunk_twice = {
#include "shared/golem_clips/golem_motion_strike_a_second_rise_sunk_twice.inc"
};
static NJS_MKEY_A golem_motion_strike_b_start_joint_0_angles[2] = {
#include "shared/golem_clips/golem_motion_strike_b_start_joint_0_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_start_joint_1_angles[51] = {
#include "shared/golem_clips/golem_motion_strike_b_start_joint_1_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_start_joint_2_angles[38] = {
#include "shared/golem_clips/golem_motion_strike_b_start_joint_2_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_start_joint_3_angles[47] = {
#include "shared/golem_clips/golem_motion_strike_b_start_joint_3_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_start_joint_4_angles[2] = {
#include "shared/golem_clips/golem_motion_strike_b_start_joint_4_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_start_joint_5_angles[54] = {
#include "shared/golem_clips/golem_motion_strike_b_start_joint_5_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_start_joint_6_angles[54] = {
#include "shared/golem_clips/golem_motion_strike_b_start_joint_6_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_start_joint_7_angles[54] = {
#include "shared/golem_clips/golem_motion_strike_b_start_joint_7_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_start_joint_8_angles[54] = {
#include "shared/golem_clips/golem_motion_strike_b_start_joint_8_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_start_joint_9_angles[54] = {
#include "shared/golem_clips/golem_motion_strike_b_start_joint_9_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_start_joint_10_angles[54] = {
#include "shared/golem_clips/golem_motion_strike_b_start_joint_10_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_start_joint_11_angles[52] = {
#include "shared/golem_clips/golem_motion_strike_b_start_joint_11_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_start_joint_12_angles[50] = {
#include "shared/golem_clips/golem_motion_strike_b_start_joint_12_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_start_joint_13_angles[54] = {
#include "shared/golem_clips/golem_motion_strike_b_start_joint_13_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_start_joint_14_angles[54] = {
#include "shared/golem_clips/golem_motion_strike_b_start_joint_14_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_start_joint_15_angles[44] = {
#include "shared/golem_clips/golem_motion_strike_b_start_joint_15_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_start_joint_16_angles[54] = {
#include "shared/golem_clips/golem_motion_strike_b_start_joint_16_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_start_joint_17_angles[54] = {
#include "shared/golem_clips/golem_motion_strike_b_start_joint_17_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_start_joint_18_angles[54] = {
#include "shared/golem_clips/golem_motion_strike_b_start_joint_18_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_start_joint_19_angles[54] = {
#include "shared/golem_clips/golem_motion_strike_b_start_joint_19_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_start_joint_20_angles[54] = {
#include "shared/golem_clips/golem_motion_strike_b_start_joint_20_angles.inc"
};
static NJS_MDATA3 golem_motion_strike_b_start_tracks[21] = {
#include "shared/golem_clips/golem_motion_strike_b_start_tracks.inc"
};
NJS_MOTION golem_motion_strike_b_start = {
#include "shared/golem_clips/golem_motion_strike_b_start.inc"
};
static NJS_MKEY_A golem_motion_strike_b_blow_whole_joint_0_angles[2] = {
#include "shared/golem_clips/golem_motion_strike_b_blow_whole_joint_0_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_blow_whole_joint_1_angles[23] = {
#include "shared/golem_clips/golem_motion_strike_b_blow_whole_joint_1_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_blow_whole_joint_2_angles[23] = {
#include "shared/golem_clips/golem_motion_strike_b_blow_whole_joint_2_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_blow_whole_joint_3_angles[23] = {
#include "shared/golem_clips/golem_motion_strike_b_blow_whole_joint_3_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_blow_whole_joint_4_angles[2] = {
#include "shared/golem_clips/golem_motion_strike_b_blow_whole_joint_4_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_blow_whole_joint_5_angles[23] = {
#include "shared/golem_clips/golem_motion_strike_b_blow_whole_joint_5_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_blow_whole_joint_6_angles[23] = {
#include "shared/golem_clips/golem_motion_strike_b_blow_whole_joint_6_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_blow_whole_joint_7_angles[23] = {
#include "shared/golem_clips/golem_motion_strike_b_blow_whole_joint_7_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_blow_whole_joint_8_angles[23] = {
#include "shared/golem_clips/golem_motion_strike_b_blow_whole_joint_8_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_blow_whole_joint_9_angles[23] = {
#include "shared/golem_clips/golem_motion_strike_b_blow_whole_joint_9_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_blow_whole_joint_10_angles[23] = {
#include "shared/golem_clips/golem_motion_strike_b_blow_whole_joint_10_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_blow_whole_joint_11_angles[23] = {
#include "shared/golem_clips/golem_motion_strike_b_blow_whole_joint_11_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_blow_whole_joint_12_angles[23] = {
#include "shared/golem_clips/golem_motion_strike_b_blow_whole_joint_12_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_blow_whole_joint_13_angles[23] = {
#include "shared/golem_clips/golem_motion_strike_b_blow_whole_joint_13_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_blow_whole_joint_14_angles[23] = {
#include "shared/golem_clips/golem_motion_strike_b_blow_whole_joint_14_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_blow_whole_joint_15_angles[23] = {
#include "shared/golem_clips/golem_motion_strike_b_blow_whole_joint_15_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_blow_whole_joint_16_angles[23] = {
#include "shared/golem_clips/golem_motion_strike_b_blow_whole_joint_16_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_blow_whole_joint_17_angles[19] = {
#include "shared/golem_clips/golem_motion_strike_b_blow_whole_joint_17_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_blow_whole_joint_18_angles[23] = {
#include "shared/golem_clips/golem_motion_strike_b_blow_whole_joint_18_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_blow_whole_joint_19_angles[17] = {
#include "shared/golem_clips/golem_motion_strike_b_blow_whole_joint_19_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_blow_whole_joint_20_angles[23] = {
#include "shared/golem_clips/golem_motion_strike_b_blow_whole_joint_20_angles.inc"
};
static NJS_MDATA3 golem_motion_strike_b_blow_whole_tracks[21] = {
#include "shared/golem_clips/golem_motion_strike_b_blow_whole_tracks.inc"
};
NJS_MOTION golem_motion_strike_b_blow_whole = {
#include "shared/golem_clips/golem_motion_strike_b_blow_whole.inc"
};
static NJS_MKEY_A golem_motion_strike_b_rise_whole_joint_0_angles[2] = {
#include "shared/golem_clips/golem_motion_strike_b_rise_whole_joint_0_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_rise_whole_joint_1_angles[40] = {
#include "shared/golem_clips/golem_motion_strike_b_rise_whole_joint_1_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_rise_whole_joint_2_angles[46] = {
#include "shared/golem_clips/golem_motion_strike_b_rise_whole_joint_2_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_rise_whole_joint_3_angles[41] = {
#include "shared/golem_clips/golem_motion_strike_b_rise_whole_joint_3_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_rise_whole_joint_4_angles[2] = {
#include "shared/golem_clips/golem_motion_strike_b_rise_whole_joint_4_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_rise_whole_joint_5_angles[46] = {
#include "shared/golem_clips/golem_motion_strike_b_rise_whole_joint_5_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_rise_whole_joint_6_angles[46] = {
#include "shared/golem_clips/golem_motion_strike_b_rise_whole_joint_6_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_rise_whole_joint_7_angles[46] = {
#include "shared/golem_clips/golem_motion_strike_b_rise_whole_joint_7_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_rise_whole_joint_8_angles[46] = {
#include "shared/golem_clips/golem_motion_strike_b_rise_whole_joint_8_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_rise_whole_joint_9_angles[46] = {
#include "shared/golem_clips/golem_motion_strike_b_rise_whole_joint_9_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_rise_whole_joint_10_angles[46] = {
#include "shared/golem_clips/golem_motion_strike_b_rise_whole_joint_10_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_rise_whole_joint_11_angles[46] = {
#include "shared/golem_clips/golem_motion_strike_b_rise_whole_joint_11_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_rise_whole_joint_12_angles[46] = {
#include "shared/golem_clips/golem_motion_strike_b_rise_whole_joint_12_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_rise_whole_joint_13_angles[46] = {
#include "shared/golem_clips/golem_motion_strike_b_rise_whole_joint_13_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_rise_whole_joint_14_angles[46] = {
#include "shared/golem_clips/golem_motion_strike_b_rise_whole_joint_14_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_rise_whole_joint_15_angles[46] = {
#include "shared/golem_clips/golem_motion_strike_b_rise_whole_joint_15_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_rise_whole_joint_16_angles[46] = {
#include "shared/golem_clips/golem_motion_strike_b_rise_whole_joint_16_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_rise_whole_joint_17_angles[46] = {
#include "shared/golem_clips/golem_motion_strike_b_rise_whole_joint_17_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_rise_whole_joint_18_angles[46] = {
#include "shared/golem_clips/golem_motion_strike_b_rise_whole_joint_18_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_rise_whole_joint_19_angles[46] = {
#include "shared/golem_clips/golem_motion_strike_b_rise_whole_joint_19_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_rise_whole_joint_20_angles[46] = {
#include "shared/golem_clips/golem_motion_strike_b_rise_whole_joint_20_angles.inc"
};
static NJS_MDATA3 golem_motion_strike_b_rise_whole_tracks[21] = {
#include "shared/golem_clips/golem_motion_strike_b_rise_whole_tracks.inc"
};
NJS_MOTION golem_motion_strike_b_rise_whole = {
#include "shared/golem_clips/golem_motion_strike_b_rise_whole.inc"
};
static NJS_MKEY_A golem_motion_strike_b_second_blow_whole_joint_0_angles[2] = {
#include "shared/golem_clips/golem_motion_strike_b_second_blow_whole_joint_0_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_second_blow_whole_joint_1_angles[23] = {
#include "shared/golem_clips/golem_motion_strike_b_second_blow_whole_joint_1_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_second_blow_whole_joint_2_angles[23] = {
#include "shared/golem_clips/golem_motion_strike_b_second_blow_whole_joint_2_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_second_blow_whole_joint_3_angles[23] = {
#include "shared/golem_clips/golem_motion_strike_b_second_blow_whole_joint_3_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_second_blow_whole_joint_4_angles[2] = {
#include "shared/golem_clips/golem_motion_strike_b_second_blow_whole_joint_4_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_second_blow_whole_joint_5_angles[23] = {
#include "shared/golem_clips/golem_motion_strike_b_second_blow_whole_joint_5_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_second_blow_whole_joint_6_angles[19] = {
#include "shared/golem_clips/golem_motion_strike_b_second_blow_whole_joint_6_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_second_blow_whole_joint_7_angles[23] = {
#include "shared/golem_clips/golem_motion_strike_b_second_blow_whole_joint_7_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_second_blow_whole_joint_8_angles[22] = {
#include "shared/golem_clips/golem_motion_strike_b_second_blow_whole_joint_8_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_second_blow_whole_joint_9_angles[23] = {
#include "shared/golem_clips/golem_motion_strike_b_second_blow_whole_joint_9_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_second_blow_whole_joint_10_angles[16] =
    {
#include "shared/golem_clips/golem_motion_strike_b_second_blow_whole_joint_10_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_second_blow_whole_joint_11_angles[16] =
    {
#include "shared/golem_clips/golem_motion_strike_b_second_blow_whole_joint_11_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_second_blow_whole_joint_12_angles[17] =
    {
#include "shared/golem_clips/golem_motion_strike_b_second_blow_whole_joint_12_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_second_blow_whole_joint_13_angles[23] =
    {
#include "shared/golem_clips/golem_motion_strike_b_second_blow_whole_joint_13_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_second_blow_whole_joint_14_angles[23] =
    {
#include "shared/golem_clips/golem_motion_strike_b_second_blow_whole_joint_14_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_second_blow_whole_joint_15_angles[23] =
    {
#include "shared/golem_clips/golem_motion_strike_b_second_blow_whole_joint_15_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_second_blow_whole_joint_16_angles[23] =
    {
#include "shared/golem_clips/golem_motion_strike_b_second_blow_whole_joint_16_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_second_blow_whole_joint_17_angles[23] =
    {
#include "shared/golem_clips/golem_motion_strike_b_second_blow_whole_joint_17_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_second_blow_whole_joint_18_angles[23] =
    {
#include "shared/golem_clips/golem_motion_strike_b_second_blow_whole_joint_18_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_second_blow_whole_joint_19_angles[23] =
    {
#include "shared/golem_clips/golem_motion_strike_b_second_blow_whole_joint_19_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_second_blow_whole_joint_20_angles[23] =
    {
#include "shared/golem_clips/golem_motion_strike_b_second_blow_whole_joint_20_angles.inc"
};
static NJS_MDATA3 golem_motion_strike_b_second_blow_whole_tracks[21] = {
#include "shared/golem_clips/golem_motion_strike_b_second_blow_whole_tracks.inc"
};
NJS_MOTION golem_motion_strike_b_second_blow_whole = {
#include "shared/golem_clips/golem_motion_strike_b_second_blow_whole.inc"
};
static NJS_MKEY_A golem_motion_strike_b_second_rise_whole_joint_0_angles[2] = {
#include "shared/golem_clips/golem_motion_strike_b_second_rise_whole_joint_0_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_second_rise_whole_joint_1_angles[19] = {
#include "shared/golem_clips/golem_motion_strike_b_second_rise_whole_joint_1_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_second_rise_whole_joint_2_angles[28] = {
#include "shared/golem_clips/golem_motion_strike_b_second_rise_whole_joint_2_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_second_rise_whole_joint_3_angles[29] = {
#include "shared/golem_clips/golem_motion_strike_b_second_rise_whole_joint_3_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_second_rise_whole_joint_4_angles[2] = {
#include "shared/golem_clips/golem_motion_strike_b_second_rise_whole_joint_4_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_second_rise_whole_joint_5_angles[30] = {
#include "shared/golem_clips/golem_motion_strike_b_second_rise_whole_joint_5_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_second_rise_whole_joint_6_angles[30] = {
#include "shared/golem_clips/golem_motion_strike_b_second_rise_whole_joint_6_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_second_rise_whole_joint_7_angles[30] = {
#include "shared/golem_clips/golem_motion_strike_b_second_rise_whole_joint_7_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_second_rise_whole_joint_8_angles[29] = {
#include "shared/golem_clips/golem_motion_strike_b_second_rise_whole_joint_8_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_second_rise_whole_joint_9_angles[30] = {
#include "shared/golem_clips/golem_motion_strike_b_second_rise_whole_joint_9_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_second_rise_whole_joint_10_angles[28] =
    {
#include "shared/golem_clips/golem_motion_strike_b_second_rise_whole_joint_10_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_second_rise_whole_joint_11_angles[30] =
    {
#include "shared/golem_clips/golem_motion_strike_b_second_rise_whole_joint_11_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_second_rise_whole_joint_12_angles[29] =
    {
#include "shared/golem_clips/golem_motion_strike_b_second_rise_whole_joint_12_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_second_rise_whole_joint_13_angles[30] =
    {
#include "shared/golem_clips/golem_motion_strike_b_second_rise_whole_joint_13_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_second_rise_whole_joint_14_angles[30] =
    {
#include "shared/golem_clips/golem_motion_strike_b_second_rise_whole_joint_14_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_second_rise_whole_joint_15_angles[30] =
    {
#include "shared/golem_clips/golem_motion_strike_b_second_rise_whole_joint_15_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_second_rise_whole_joint_16_angles[30] =
    {
#include "shared/golem_clips/golem_motion_strike_b_second_rise_whole_joint_16_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_second_rise_whole_joint_17_angles[30] =
    {
#include "shared/golem_clips/golem_motion_strike_b_second_rise_whole_joint_17_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_second_rise_whole_joint_18_angles[30] =
    {
#include "shared/golem_clips/golem_motion_strike_b_second_rise_whole_joint_18_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_second_rise_whole_joint_19_angles[30] =
    {
#include "shared/golem_clips/golem_motion_strike_b_second_rise_whole_joint_19_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_second_rise_whole_joint_20_angles[30] =
    {
#include "shared/golem_clips/golem_motion_strike_b_second_rise_whole_joint_20_angles.inc"
};
static NJS_MDATA3 golem_motion_strike_b_second_rise_whole_tracks[21] = {
#include "shared/golem_clips/golem_motion_strike_b_second_rise_whole_tracks.inc"
};
NJS_MOTION golem_motion_strike_b_second_rise_whole = {
#include "shared/golem_clips/golem_motion_strike_b_second_rise_whole.inc"
};
static NJS_MKEY_A golem_motion_strike_b_blow_sunk_joint_0_angles[2] = {
#include "shared/golem_clips/golem_motion_strike_b_blow_sunk_joint_0_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_blow_sunk_joint_1_angles[23] = {
#include "shared/golem_clips/golem_motion_strike_b_blow_sunk_joint_1_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_blow_sunk_joint_2_angles[23] = {
#include "shared/golem_clips/golem_motion_strike_b_blow_sunk_joint_2_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_blow_sunk_joint_3_angles[23] = {
#include "shared/golem_clips/golem_motion_strike_b_blow_sunk_joint_3_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_blow_sunk_joint_4_angles[2] = {
#include "shared/golem_clips/golem_motion_strike_b_blow_sunk_joint_4_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_blow_sunk_joint_5_angles[23] = {
#include "shared/golem_clips/golem_motion_strike_b_blow_sunk_joint_5_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_blow_sunk_joint_6_angles[23] = {
#include "shared/golem_clips/golem_motion_strike_b_blow_sunk_joint_6_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_blow_sunk_joint_7_angles[23] = {
#include "shared/golem_clips/golem_motion_strike_b_blow_sunk_joint_7_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_blow_sunk_joint_8_angles[23] = {
#include "shared/golem_clips/golem_motion_strike_b_blow_sunk_joint_8_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_blow_sunk_joint_9_angles[23] = {
#include "shared/golem_clips/golem_motion_strike_b_blow_sunk_joint_9_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_blow_sunk_joint_10_angles[23] = {
#include "shared/golem_clips/golem_motion_strike_b_blow_sunk_joint_10_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_blow_sunk_joint_11_angles[23] = {
#include "shared/golem_clips/golem_motion_strike_b_blow_sunk_joint_11_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_blow_sunk_joint_12_angles[23] = {
#include "shared/golem_clips/golem_motion_strike_b_blow_sunk_joint_12_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_blow_sunk_joint_13_angles[23] = {
#include "shared/golem_clips/golem_motion_strike_b_blow_sunk_joint_13_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_blow_sunk_joint_14_angles[23] = {
#include "shared/golem_clips/golem_motion_strike_b_blow_sunk_joint_14_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_blow_sunk_joint_15_angles[23] = {
#include "shared/golem_clips/golem_motion_strike_b_blow_sunk_joint_15_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_blow_sunk_joint_16_angles[23] = {
#include "shared/golem_clips/golem_motion_strike_b_blow_sunk_joint_16_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_blow_sunk_joint_17_angles[19] = {
#include "shared/golem_clips/golem_motion_strike_b_blow_sunk_joint_17_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_blow_sunk_joint_18_angles[23] = {
#include "shared/golem_clips/golem_motion_strike_b_blow_sunk_joint_18_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_blow_sunk_joint_19_angles[17] = {
#include "shared/golem_clips/golem_motion_strike_b_blow_sunk_joint_19_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_blow_sunk_joint_20_angles[23] = {
#include "shared/golem_clips/golem_motion_strike_b_blow_sunk_joint_20_angles.inc"
};
static NJS_MDATA3 golem_motion_strike_b_blow_sunk_tracks[21] = {
#include "shared/golem_clips/golem_motion_strike_b_blow_sunk_tracks.inc"
};
NJS_MOTION golem_motion_strike_b_blow_sunk = {
#include "shared/golem_clips/golem_motion_strike_b_blow_sunk.inc"
};
static NJS_MKEY_A golem_motion_strike_b_rise_sunk_joint_0_angles[2] = {
#include "shared/golem_clips/golem_motion_strike_b_rise_sunk_joint_0_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_rise_sunk_joint_1_angles[40] = {
#include "shared/golem_clips/golem_motion_strike_b_rise_sunk_joint_1_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_rise_sunk_joint_2_angles[46] = {
#include "shared/golem_clips/golem_motion_strike_b_rise_sunk_joint_2_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_rise_sunk_joint_3_angles[41] = {
#include "shared/golem_clips/golem_motion_strike_b_rise_sunk_joint_3_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_rise_sunk_joint_4_angles[2] = {
#include "shared/golem_clips/golem_motion_strike_b_rise_sunk_joint_4_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_rise_sunk_joint_5_angles[46] = {
#include "shared/golem_clips/golem_motion_strike_b_rise_sunk_joint_5_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_rise_sunk_joint_6_angles[46] = {
#include "shared/golem_clips/golem_motion_strike_b_rise_sunk_joint_6_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_rise_sunk_joint_7_angles[46] = {
#include "shared/golem_clips/golem_motion_strike_b_rise_sunk_joint_7_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_rise_sunk_joint_8_angles[46] = {
#include "shared/golem_clips/golem_motion_strike_b_rise_sunk_joint_8_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_rise_sunk_joint_9_angles[46] = {
#include "shared/golem_clips/golem_motion_strike_b_rise_sunk_joint_9_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_rise_sunk_joint_10_angles[46] = {
#include "shared/golem_clips/golem_motion_strike_b_rise_sunk_joint_10_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_rise_sunk_joint_11_angles[46] = {
#include "shared/golem_clips/golem_motion_strike_b_rise_sunk_joint_11_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_rise_sunk_joint_12_angles[46] = {
#include "shared/golem_clips/golem_motion_strike_b_rise_sunk_joint_12_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_rise_sunk_joint_13_angles[46] = {
#include "shared/golem_clips/golem_motion_strike_b_rise_sunk_joint_13_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_rise_sunk_joint_14_angles[46] = {
#include "shared/golem_clips/golem_motion_strike_b_rise_sunk_joint_14_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_rise_sunk_joint_15_angles[46] = {
#include "shared/golem_clips/golem_motion_strike_b_rise_sunk_joint_15_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_rise_sunk_joint_16_angles[46] = {
#include "shared/golem_clips/golem_motion_strike_b_rise_sunk_joint_16_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_rise_sunk_joint_17_angles[46] = {
#include "shared/golem_clips/golem_motion_strike_b_rise_sunk_joint_17_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_rise_sunk_joint_18_angles[46] = {
#include "shared/golem_clips/golem_motion_strike_b_rise_sunk_joint_18_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_rise_sunk_joint_19_angles[46] = {
#include "shared/golem_clips/golem_motion_strike_b_rise_sunk_joint_19_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_rise_sunk_joint_20_angles[46] = {
#include "shared/golem_clips/golem_motion_strike_b_rise_sunk_joint_20_angles.inc"
};
static NJS_MDATA3 golem_motion_strike_b_rise_sunk_tracks[21] = {
#include "shared/golem_clips/golem_motion_strike_b_rise_sunk_tracks.inc"
};
NJS_MOTION golem_motion_strike_b_rise_sunk = {
#include "shared/golem_clips/golem_motion_strike_b_rise_sunk.inc"
};
static NJS_MKEY_A golem_motion_strike_b_second_blow_sunk_joint_0_angles[2] = {
#include "shared/golem_clips/golem_motion_strike_b_second_blow_sunk_joint_0_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_second_blow_sunk_joint_1_angles[23] = {
#include "shared/golem_clips/golem_motion_strike_b_second_blow_sunk_joint_1_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_second_blow_sunk_joint_2_angles[23] = {
#include "shared/golem_clips/golem_motion_strike_b_second_blow_sunk_joint_2_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_second_blow_sunk_joint_3_angles[23] = {
#include "shared/golem_clips/golem_motion_strike_b_second_blow_sunk_joint_3_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_second_blow_sunk_joint_4_angles[2] = {
#include "shared/golem_clips/golem_motion_strike_b_second_blow_sunk_joint_4_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_second_blow_sunk_joint_5_angles[23] = {
#include "shared/golem_clips/golem_motion_strike_b_second_blow_sunk_joint_5_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_second_blow_sunk_joint_6_angles[19] = {
#include "shared/golem_clips/golem_motion_strike_b_second_blow_sunk_joint_6_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_second_blow_sunk_joint_7_angles[23] = {
#include "shared/golem_clips/golem_motion_strike_b_second_blow_sunk_joint_7_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_second_blow_sunk_joint_8_angles[20] = {
#include "shared/golem_clips/golem_motion_strike_b_second_blow_sunk_joint_8_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_second_blow_sunk_joint_9_angles[23] = {
#include "shared/golem_clips/golem_motion_strike_b_second_blow_sunk_joint_9_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_second_blow_sunk_joint_10_angles[16] = {
#include "shared/golem_clips/golem_motion_strike_b_second_blow_sunk_joint_10_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_second_blow_sunk_joint_11_angles[16] = {
#include "shared/golem_clips/golem_motion_strike_b_second_blow_sunk_joint_11_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_second_blow_sunk_joint_12_angles[17] = {
#include "shared/golem_clips/golem_motion_strike_b_second_blow_sunk_joint_12_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_second_blow_sunk_joint_13_angles[23] = {
#include "shared/golem_clips/golem_motion_strike_b_second_blow_sunk_joint_13_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_second_blow_sunk_joint_14_angles[23] = {
#include "shared/golem_clips/golem_motion_strike_b_second_blow_sunk_joint_14_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_second_blow_sunk_joint_15_angles[23] = {
#include "shared/golem_clips/golem_motion_strike_b_second_blow_sunk_joint_15_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_second_blow_sunk_joint_16_angles[23] = {
#include "shared/golem_clips/golem_motion_strike_b_second_blow_sunk_joint_16_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_second_blow_sunk_joint_17_angles[23] = {
#include "shared/golem_clips/golem_motion_strike_b_second_blow_sunk_joint_17_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_second_blow_sunk_joint_18_angles[23] = {
#include "shared/golem_clips/golem_motion_strike_b_second_blow_sunk_joint_18_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_second_blow_sunk_joint_19_angles[23] = {
#include "shared/golem_clips/golem_motion_strike_b_second_blow_sunk_joint_19_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_second_blow_sunk_joint_20_angles[23] = {
#include "shared/golem_clips/golem_motion_strike_b_second_blow_sunk_joint_20_angles.inc"
};
static NJS_MDATA3 golem_motion_strike_b_second_blow_sunk_tracks[21] = {
#include "shared/golem_clips/golem_motion_strike_b_second_blow_sunk_tracks.inc"
};
NJS_MOTION golem_motion_strike_b_second_blow_sunk = {
#include "shared/golem_clips/golem_motion_strike_b_second_blow_sunk.inc"
};
static NJS_MKEY_A golem_motion_strike_b_second_rise_sunk_joint_0_angles[2] = {
#include "shared/golem_clips/golem_motion_strike_b_second_rise_sunk_joint_0_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_second_rise_sunk_joint_1_angles[19] = {
#include "shared/golem_clips/golem_motion_strike_b_second_rise_sunk_joint_1_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_second_rise_sunk_joint_2_angles[28] = {
#include "shared/golem_clips/golem_motion_strike_b_second_rise_sunk_joint_2_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_second_rise_sunk_joint_3_angles[29] = {
#include "shared/golem_clips/golem_motion_strike_b_second_rise_sunk_joint_3_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_second_rise_sunk_joint_4_angles[2] = {
#include "shared/golem_clips/golem_motion_strike_b_second_rise_sunk_joint_4_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_second_rise_sunk_joint_5_angles[30] = {
#include "shared/golem_clips/golem_motion_strike_b_second_rise_sunk_joint_5_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_second_rise_sunk_joint_6_angles[30] = {
#include "shared/golem_clips/golem_motion_strike_b_second_rise_sunk_joint_6_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_second_rise_sunk_joint_7_angles[30] = {
#include "shared/golem_clips/golem_motion_strike_b_second_rise_sunk_joint_7_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_second_rise_sunk_joint_8_angles[30] = {
#include "shared/golem_clips/golem_motion_strike_b_second_rise_sunk_joint_8_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_second_rise_sunk_joint_9_angles[30] = {
#include "shared/golem_clips/golem_motion_strike_b_second_rise_sunk_joint_9_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_second_rise_sunk_joint_10_angles[28] = {
#include "shared/golem_clips/golem_motion_strike_b_second_rise_sunk_joint_10_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_second_rise_sunk_joint_11_angles[30] = {
#include "shared/golem_clips/golem_motion_strike_b_second_rise_sunk_joint_11_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_second_rise_sunk_joint_12_angles[29] = {
#include "shared/golem_clips/golem_motion_strike_b_second_rise_sunk_joint_12_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_second_rise_sunk_joint_13_angles[30] = {
#include "shared/golem_clips/golem_motion_strike_b_second_rise_sunk_joint_13_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_second_rise_sunk_joint_14_angles[30] = {
#include "shared/golem_clips/golem_motion_strike_b_second_rise_sunk_joint_14_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_second_rise_sunk_joint_15_angles[30] = {
#include "shared/golem_clips/golem_motion_strike_b_second_rise_sunk_joint_15_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_second_rise_sunk_joint_16_angles[30] = {
#include "shared/golem_clips/golem_motion_strike_b_second_rise_sunk_joint_16_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_second_rise_sunk_joint_17_angles[30] = {
#include "shared/golem_clips/golem_motion_strike_b_second_rise_sunk_joint_17_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_second_rise_sunk_joint_18_angles[30] = {
#include "shared/golem_clips/golem_motion_strike_b_second_rise_sunk_joint_18_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_second_rise_sunk_joint_19_angles[30] = {
#include "shared/golem_clips/golem_motion_strike_b_second_rise_sunk_joint_19_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_second_rise_sunk_joint_20_angles[30] = {
#include "shared/golem_clips/golem_motion_strike_b_second_rise_sunk_joint_20_angles.inc"
};
static NJS_MDATA3 golem_motion_strike_b_second_rise_sunk_tracks[21] = {
#include "shared/golem_clips/golem_motion_strike_b_second_rise_sunk_tracks.inc"
};
NJS_MOTION golem_motion_strike_b_second_rise_sunk = {
#include "shared/golem_clips/golem_motion_strike_b_second_rise_sunk.inc"
};
static NJS_MKEY_A golem_motion_strike_b_blow_sunk_twice_joint_0_angles[2] = {
#include "shared/golem_clips/golem_motion_strike_b_blow_sunk_twice_joint_0_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_blow_sunk_twice_joint_1_angles[23] = {
#include "shared/golem_clips/golem_motion_strike_b_blow_sunk_twice_joint_1_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_blow_sunk_twice_joint_2_angles[23] = {
#include "shared/golem_clips/golem_motion_strike_b_blow_sunk_twice_joint_2_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_blow_sunk_twice_joint_3_angles[23] = {
#include "shared/golem_clips/golem_motion_strike_b_blow_sunk_twice_joint_3_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_blow_sunk_twice_joint_4_angles[2] = {
#include "shared/golem_clips/golem_motion_strike_b_blow_sunk_twice_joint_4_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_blow_sunk_twice_joint_5_angles[23] = {
#include "shared/golem_clips/golem_motion_strike_b_blow_sunk_twice_joint_5_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_blow_sunk_twice_joint_6_angles[23] = {
#include "shared/golem_clips/golem_motion_strike_b_blow_sunk_twice_joint_6_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_blow_sunk_twice_joint_7_angles[23] = {
#include "shared/golem_clips/golem_motion_strike_b_blow_sunk_twice_joint_7_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_blow_sunk_twice_joint_8_angles[23] = {
#include "shared/golem_clips/golem_motion_strike_b_blow_sunk_twice_joint_8_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_blow_sunk_twice_joint_9_angles[23] = {
#include "shared/golem_clips/golem_motion_strike_b_blow_sunk_twice_joint_9_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_blow_sunk_twice_joint_10_angles[23] = {
#include "shared/golem_clips/golem_motion_strike_b_blow_sunk_twice_joint_10_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_blow_sunk_twice_joint_11_angles[23] = {
#include "shared/golem_clips/golem_motion_strike_b_blow_sunk_twice_joint_11_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_blow_sunk_twice_joint_12_angles[23] = {
#include "shared/golem_clips/golem_motion_strike_b_blow_sunk_twice_joint_12_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_blow_sunk_twice_joint_13_angles[23] = {
#include "shared/golem_clips/golem_motion_strike_b_blow_sunk_twice_joint_13_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_blow_sunk_twice_joint_14_angles[23] = {
#include "shared/golem_clips/golem_motion_strike_b_blow_sunk_twice_joint_14_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_blow_sunk_twice_joint_15_angles[19] = {
#include "shared/golem_clips/golem_motion_strike_b_blow_sunk_twice_joint_15_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_blow_sunk_twice_joint_16_angles[23] = {
#include "shared/golem_clips/golem_motion_strike_b_blow_sunk_twice_joint_16_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_blow_sunk_twice_joint_17_angles[19] = {
#include "shared/golem_clips/golem_motion_strike_b_blow_sunk_twice_joint_17_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_blow_sunk_twice_joint_18_angles[23] = {
#include "shared/golem_clips/golem_motion_strike_b_blow_sunk_twice_joint_18_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_blow_sunk_twice_joint_19_angles[17] = {
#include "shared/golem_clips/golem_motion_strike_b_blow_sunk_twice_joint_19_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_blow_sunk_twice_joint_20_angles[23] = {
#include "shared/golem_clips/golem_motion_strike_b_blow_sunk_twice_joint_20_angles.inc"
};
static NJS_MDATA3 golem_motion_strike_b_blow_sunk_twice_tracks[21] = {
#include "shared/golem_clips/golem_motion_strike_b_blow_sunk_twice_tracks.inc"
};
NJS_MOTION golem_motion_strike_b_blow_sunk_twice = {
#include "shared/golem_clips/golem_motion_strike_b_blow_sunk_twice.inc"
};
static NJS_MKEY_A golem_motion_strike_b_rise_sunk_twice_joint_0_angles[2] = {
#include "shared/golem_clips/golem_motion_strike_b_rise_sunk_twice_joint_0_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_rise_sunk_twice_joint_1_angles[40] = {
#include "shared/golem_clips/golem_motion_strike_b_rise_sunk_twice_joint_1_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_rise_sunk_twice_joint_2_angles[46] = {
#include "shared/golem_clips/golem_motion_strike_b_rise_sunk_twice_joint_2_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_rise_sunk_twice_joint_3_angles[41] = {
#include "shared/golem_clips/golem_motion_strike_b_rise_sunk_twice_joint_3_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_rise_sunk_twice_joint_4_angles[2] = {
#include "shared/golem_clips/golem_motion_strike_b_rise_sunk_twice_joint_4_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_rise_sunk_twice_joint_5_angles[46] = {
#include "shared/golem_clips/golem_motion_strike_b_rise_sunk_twice_joint_5_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_rise_sunk_twice_joint_6_angles[46] = {
#include "shared/golem_clips/golem_motion_strike_b_rise_sunk_twice_joint_6_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_rise_sunk_twice_joint_7_angles[46] = {
#include "shared/golem_clips/golem_motion_strike_b_rise_sunk_twice_joint_7_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_rise_sunk_twice_joint_8_angles[46] = {
#include "shared/golem_clips/golem_motion_strike_b_rise_sunk_twice_joint_8_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_rise_sunk_twice_joint_9_angles[46] = {
#include "shared/golem_clips/golem_motion_strike_b_rise_sunk_twice_joint_9_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_rise_sunk_twice_joint_10_angles[46] = {
#include "shared/golem_clips/golem_motion_strike_b_rise_sunk_twice_joint_10_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_rise_sunk_twice_joint_11_angles[46] = {
#include "shared/golem_clips/golem_motion_strike_b_rise_sunk_twice_joint_11_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_rise_sunk_twice_joint_12_angles[46] = {
#include "shared/golem_clips/golem_motion_strike_b_rise_sunk_twice_joint_12_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_rise_sunk_twice_joint_13_angles[46] = {
#include "shared/golem_clips/golem_motion_strike_b_rise_sunk_twice_joint_13_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_rise_sunk_twice_joint_14_angles[46] = {
#include "shared/golem_clips/golem_motion_strike_b_rise_sunk_twice_joint_14_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_rise_sunk_twice_joint_15_angles[46] = {
#include "shared/golem_clips/golem_motion_strike_b_rise_sunk_twice_joint_15_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_rise_sunk_twice_joint_16_angles[46] = {
#include "shared/golem_clips/golem_motion_strike_b_rise_sunk_twice_joint_16_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_rise_sunk_twice_joint_17_angles[46] = {
#include "shared/golem_clips/golem_motion_strike_b_rise_sunk_twice_joint_17_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_rise_sunk_twice_joint_18_angles[46] = {
#include "shared/golem_clips/golem_motion_strike_b_rise_sunk_twice_joint_18_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_rise_sunk_twice_joint_19_angles[46] = {
#include "shared/golem_clips/golem_motion_strike_b_rise_sunk_twice_joint_19_angles.inc"
};
static NJS_MKEY_A golem_motion_strike_b_rise_sunk_twice_joint_20_angles[46] = {
#include "shared/golem_clips/golem_motion_strike_b_rise_sunk_twice_joint_20_angles.inc"
};
static NJS_MDATA3 golem_motion_strike_b_rise_sunk_twice_tracks[21] = {
#include "shared/golem_clips/golem_motion_strike_b_rise_sunk_twice_tracks.inc"
};
NJS_MOTION golem_motion_strike_b_rise_sunk_twice = {
#include "shared/golem_clips/golem_motion_strike_b_rise_sunk_twice.inc"
};
static NJS_MKEY_A
    golem_motion_strike_b_second_blow_sunk_twice_joint_0_angles[2] = {
#include "shared/golem_clips/golem_motion_strike_b_second_blow_sunk_twice_joint_0_angles.inc"
};
static NJS_MKEY_A
    golem_motion_strike_b_second_blow_sunk_twice_joint_1_angles[23] = {
#include "shared/golem_clips/golem_motion_strike_b_second_blow_sunk_twice_joint_1_angles.inc"
};
static NJS_MKEY_A
    golem_motion_strike_b_second_blow_sunk_twice_joint_2_angles[23] = {
#include "shared/golem_clips/golem_motion_strike_b_second_blow_sunk_twice_joint_2_angles.inc"
};
static NJS_MKEY_A
    golem_motion_strike_b_second_blow_sunk_twice_joint_3_angles[23] = {
#include "shared/golem_clips/golem_motion_strike_b_second_blow_sunk_twice_joint_3_angles.inc"
};
static NJS_MKEY_A
    golem_motion_strike_b_second_blow_sunk_twice_joint_4_angles[2] = {
#include "shared/golem_clips/golem_motion_strike_b_second_blow_sunk_twice_joint_4_angles.inc"
};
static NJS_MKEY_A
    golem_motion_strike_b_second_blow_sunk_twice_joint_5_angles[23] = {
#include "shared/golem_clips/golem_motion_strike_b_second_blow_sunk_twice_joint_5_angles.inc"
};
static NJS_MKEY_A
    golem_motion_strike_b_second_blow_sunk_twice_joint_6_angles[19] = {
#include "shared/golem_clips/golem_motion_strike_b_second_blow_sunk_twice_joint_6_angles.inc"
};
static NJS_MKEY_A
    golem_motion_strike_b_second_blow_sunk_twice_joint_7_angles[23] = {
#include "shared/golem_clips/golem_motion_strike_b_second_blow_sunk_twice_joint_7_angles.inc"
};
static NJS_MKEY_A
    golem_motion_strike_b_second_blow_sunk_twice_joint_8_angles[18] = {
#include "shared/golem_clips/golem_motion_strike_b_second_blow_sunk_twice_joint_8_angles.inc"
};
static NJS_MKEY_A
    golem_motion_strike_b_second_blow_sunk_twice_joint_9_angles[23] = {
#include "shared/golem_clips/golem_motion_strike_b_second_blow_sunk_twice_joint_9_angles.inc"
};
static NJS_MKEY_A
    golem_motion_strike_b_second_blow_sunk_twice_joint_10_angles[16] = {
#include "shared/golem_clips/golem_motion_strike_b_second_blow_sunk_twice_joint_10_angles.inc"
};
static NJS_MKEY_A
    golem_motion_strike_b_second_blow_sunk_twice_joint_11_angles[16] = {
#include "shared/golem_clips/golem_motion_strike_b_second_blow_sunk_twice_joint_11_angles.inc"
};
static NJS_MKEY_A
    golem_motion_strike_b_second_blow_sunk_twice_joint_12_angles[17] = {
#include "shared/golem_clips/golem_motion_strike_b_second_blow_sunk_twice_joint_12_angles.inc"
};
static NJS_MKEY_A
    golem_motion_strike_b_second_blow_sunk_twice_joint_13_angles[23] = {
#include "shared/golem_clips/golem_motion_strike_b_second_blow_sunk_twice_joint_13_angles.inc"
};
static NJS_MKEY_A
    golem_motion_strike_b_second_blow_sunk_twice_joint_14_angles[23] = {
#include "shared/golem_clips/golem_motion_strike_b_second_blow_sunk_twice_joint_14_angles.inc"
};
static NJS_MKEY_A
    golem_motion_strike_b_second_blow_sunk_twice_joint_15_angles[23] = {
#include "shared/golem_clips/golem_motion_strike_b_second_blow_sunk_twice_joint_15_angles.inc"
};
static NJS_MKEY_A
    golem_motion_strike_b_second_blow_sunk_twice_joint_16_angles[23] = {
#include "shared/golem_clips/golem_motion_strike_b_second_blow_sunk_twice_joint_16_angles.inc"
};
static NJS_MKEY_A
    golem_motion_strike_b_second_blow_sunk_twice_joint_17_angles[23] = {
#include "shared/golem_clips/golem_motion_strike_b_second_blow_sunk_twice_joint_17_angles.inc"
};
static NJS_MKEY_A
    golem_motion_strike_b_second_blow_sunk_twice_joint_18_angles[23] = {
#include "shared/golem_clips/golem_motion_strike_b_second_blow_sunk_twice_joint_18_angles.inc"
};
static NJS_MKEY_A
    golem_motion_strike_b_second_blow_sunk_twice_joint_19_angles[23] = {
#include "shared/golem_clips/golem_motion_strike_b_second_blow_sunk_twice_joint_19_angles.inc"
};
static NJS_MKEY_A
    golem_motion_strike_b_second_blow_sunk_twice_joint_20_angles[23] = {
#include "shared/golem_clips/golem_motion_strike_b_second_blow_sunk_twice_joint_20_angles.inc"
};
static NJS_MDATA3 golem_motion_strike_b_second_blow_sunk_twice_tracks[21] = {
#include "shared/golem_clips/golem_motion_strike_b_second_blow_sunk_twice_tracks.inc"
};
NJS_MOTION golem_motion_strike_b_second_blow_sunk_twice = {
#include "shared/golem_clips/golem_motion_strike_b_second_blow_sunk_twice.inc"
};
static NJS_MKEY_A
    golem_motion_strike_b_second_rise_sunk_twice_joint_0_angles[2] = {
#include "shared/golem_clips/golem_motion_strike_b_second_rise_sunk_twice_joint_0_angles.inc"
};
static NJS_MKEY_A
    golem_motion_strike_b_second_rise_sunk_twice_joint_1_angles[19] = {
#include "shared/golem_clips/golem_motion_strike_b_second_rise_sunk_twice_joint_1_angles.inc"
};
static NJS_MKEY_A
    golem_motion_strike_b_second_rise_sunk_twice_joint_2_angles[28] = {
#include "shared/golem_clips/golem_motion_strike_b_second_rise_sunk_twice_joint_2_angles.inc"
};
static NJS_MKEY_A
    golem_motion_strike_b_second_rise_sunk_twice_joint_3_angles[29] = {
#include "shared/golem_clips/golem_motion_strike_b_second_rise_sunk_twice_joint_3_angles.inc"
};
static NJS_MKEY_A
    golem_motion_strike_b_second_rise_sunk_twice_joint_4_angles[2] = {
#include "shared/golem_clips/golem_motion_strike_b_second_rise_sunk_twice_joint_4_angles.inc"
};
static NJS_MKEY_A
    golem_motion_strike_b_second_rise_sunk_twice_joint_5_angles[30] = {
#include "shared/golem_clips/golem_motion_strike_b_second_rise_sunk_twice_joint_5_angles.inc"
};
static NJS_MKEY_A
    golem_motion_strike_b_second_rise_sunk_twice_joint_6_angles[30] = {
#include "shared/golem_clips/golem_motion_strike_b_second_rise_sunk_twice_joint_6_angles.inc"
};
static NJS_MKEY_A
    golem_motion_strike_b_second_rise_sunk_twice_joint_7_angles[30] = {
#include "shared/golem_clips/golem_motion_strike_b_second_rise_sunk_twice_joint_7_angles.inc"
};
static NJS_MKEY_A
    golem_motion_strike_b_second_rise_sunk_twice_joint_8_angles[30] = {
#include "shared/golem_clips/golem_motion_strike_b_second_rise_sunk_twice_joint_8_angles.inc"
};
static NJS_MKEY_A
    golem_motion_strike_b_second_rise_sunk_twice_joint_9_angles[30] = {
#include "shared/golem_clips/golem_motion_strike_b_second_rise_sunk_twice_joint_9_angles.inc"
};
static NJS_MKEY_A
    golem_motion_strike_b_second_rise_sunk_twice_joint_10_angles[28] = {
#include "shared/golem_clips/golem_motion_strike_b_second_rise_sunk_twice_joint_10_angles.inc"
};
static NJS_MKEY_A
    golem_motion_strike_b_second_rise_sunk_twice_joint_11_angles[30] = {
#include "shared/golem_clips/golem_motion_strike_b_second_rise_sunk_twice_joint_11_angles.inc"
};
static NJS_MKEY_A
    golem_motion_strike_b_second_rise_sunk_twice_joint_12_angles[29] = {
#include "shared/golem_clips/golem_motion_strike_b_second_rise_sunk_twice_joint_12_angles.inc"
};
static NJS_MKEY_A
    golem_motion_strike_b_second_rise_sunk_twice_joint_13_angles[30] = {
#include "shared/golem_clips/golem_motion_strike_b_second_rise_sunk_twice_joint_13_angles.inc"
};
static NJS_MKEY_A
    golem_motion_strike_b_second_rise_sunk_twice_joint_14_angles[30] = {
#include "shared/golem_clips/golem_motion_strike_b_second_rise_sunk_twice_joint_14_angles.inc"
};
static NJS_MKEY_A
    golem_motion_strike_b_second_rise_sunk_twice_joint_15_angles[30] = {
#include "shared/golem_clips/golem_motion_strike_b_second_rise_sunk_twice_joint_15_angles.inc"
};
static NJS_MKEY_A
    golem_motion_strike_b_second_rise_sunk_twice_joint_16_angles[30] = {
#include "shared/golem_clips/golem_motion_strike_b_second_rise_sunk_twice_joint_16_angles.inc"
};
static NJS_MKEY_A
    golem_motion_strike_b_second_rise_sunk_twice_joint_17_angles[30] = {
#include "shared/golem_clips/golem_motion_strike_b_second_rise_sunk_twice_joint_17_angles.inc"
};
static NJS_MKEY_A
    golem_motion_strike_b_second_rise_sunk_twice_joint_18_angles[30] = {
#include "shared/golem_clips/golem_motion_strike_b_second_rise_sunk_twice_joint_18_angles.inc"
};
static NJS_MKEY_A
    golem_motion_strike_b_second_rise_sunk_twice_joint_19_angles[30] = {
#include "shared/golem_clips/golem_motion_strike_b_second_rise_sunk_twice_joint_19_angles.inc"
};
static NJS_MKEY_A
    golem_motion_strike_b_second_rise_sunk_twice_joint_20_angles[30] = {
#include "shared/golem_clips/golem_motion_strike_b_second_rise_sunk_twice_joint_20_angles.inc"
};
static NJS_MDATA3 golem_motion_strike_b_second_rise_sunk_twice_tracks[21] = {
#include "shared/golem_clips/golem_motion_strike_b_second_rise_sunk_twice_tracks.inc"
};
NJS_MOTION golem_motion_strike_b_second_rise_sunk_twice = {
#include "shared/golem_clips/golem_motion_strike_b_second_rise_sunk_twice.inc"
};
static NJS_MKEY_A golem_motion_hands_down_a_lower_whole_joint_0_angles[2] = {
#include "shared/golem_clips/golem_motion_hands_down_a_lower_whole_joint_0_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_a_lower_whole_joint_1_angles[2] = {
#include "shared/golem_clips/golem_motion_hands_down_a_lower_whole_joint_1_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_a_lower_whole_joint_2_angles[2] = {
#include "shared/golem_clips/golem_motion_hands_down_a_lower_whole_joint_2_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_a_lower_whole_joint_3_angles[2] = {
#include "shared/golem_clips/golem_motion_hands_down_a_lower_whole_joint_3_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_a_lower_whole_joint_4_angles[26] = {
#include "shared/golem_clips/golem_motion_hands_down_a_lower_whole_joint_4_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_a_lower_whole_joint_5_angles[40] = {
#include "shared/golem_clips/golem_motion_hands_down_a_lower_whole_joint_5_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_a_lower_whole_joint_6_angles[40] = {
#include "shared/golem_clips/golem_motion_hands_down_a_lower_whole_joint_6_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_a_lower_whole_joint_7_angles[40] = {
#include "shared/golem_clips/golem_motion_hands_down_a_lower_whole_joint_7_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_a_lower_whole_joint_8_angles[40] = {
#include "shared/golem_clips/golem_motion_hands_down_a_lower_whole_joint_8_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_a_lower_whole_joint_9_angles[40] = {
#include "shared/golem_clips/golem_motion_hands_down_a_lower_whole_joint_9_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_a_lower_whole_joint_10_angles[40] = {
#include "shared/golem_clips/golem_motion_hands_down_a_lower_whole_joint_10_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_a_lower_whole_joint_11_angles[40] = {
#include "shared/golem_clips/golem_motion_hands_down_a_lower_whole_joint_11_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_a_lower_whole_joint_12_angles[40] = {
#include "shared/golem_clips/golem_motion_hands_down_a_lower_whole_joint_12_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_a_lower_whole_joint_13_angles[40] = {
#include "shared/golem_clips/golem_motion_hands_down_a_lower_whole_joint_13_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_a_lower_whole_joint_14_angles[40] = {
#include "shared/golem_clips/golem_motion_hands_down_a_lower_whole_joint_14_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_a_lower_whole_joint_15_angles[40] = {
#include "shared/golem_clips/golem_motion_hands_down_a_lower_whole_joint_15_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_a_lower_whole_joint_16_angles[40] = {
#include "shared/golem_clips/golem_motion_hands_down_a_lower_whole_joint_16_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_a_lower_whole_joint_17_angles[40] = {
#include "shared/golem_clips/golem_motion_hands_down_a_lower_whole_joint_17_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_a_lower_whole_joint_18_angles[40] = {
#include "shared/golem_clips/golem_motion_hands_down_a_lower_whole_joint_18_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_a_lower_whole_joint_19_angles[40] = {
#include "shared/golem_clips/golem_motion_hands_down_a_lower_whole_joint_19_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_a_lower_whole_joint_20_angles[40] = {
#include "shared/golem_clips/golem_motion_hands_down_a_lower_whole_joint_20_angles.inc"
};
static NJS_MDATA3 golem_motion_hands_down_a_lower_whole_tracks[21] = {
#include "shared/golem_clips/golem_motion_hands_down_a_lower_whole_tracks.inc"
};
NJS_MOTION golem_motion_hands_down_a_lower_whole = {
#include "shared/golem_clips/golem_motion_hands_down_a_lower_whole.inc"
};
static NJS_MKEY_A golem_motion_hands_down_a_hold_whole_joint_0_angles[2] = {
#include "shared/golem_clips/golem_motion_hands_down_a_hold_whole_joint_0_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_a_hold_whole_joint_1_angles[2] = {
#include "shared/golem_clips/golem_motion_hands_down_a_hold_whole_joint_1_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_a_hold_whole_joint_2_angles[2] = {
#include "shared/golem_clips/golem_motion_hands_down_a_hold_whole_joint_2_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_a_hold_whole_joint_3_angles[2] = {
#include "shared/golem_clips/golem_motion_hands_down_a_hold_whole_joint_3_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_a_hold_whole_joint_4_angles[140] = {
#include "shared/golem_clips/golem_motion_hands_down_a_hold_whole_joint_4_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_a_hold_whole_joint_5_angles[45] = {
#include "shared/golem_clips/golem_motion_hands_down_a_hold_whole_joint_5_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_a_hold_whole_joint_6_angles[140] = {
#include "shared/golem_clips/golem_motion_hands_down_a_hold_whole_joint_6_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_a_hold_whole_joint_7_angles[139] = {
#include "shared/golem_clips/golem_motion_hands_down_a_hold_whole_joint_7_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_a_hold_whole_joint_8_angles[140] = {
#include "shared/golem_clips/golem_motion_hands_down_a_hold_whole_joint_8_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_a_hold_whole_joint_9_angles[85] = {
#include "shared/golem_clips/golem_motion_hands_down_a_hold_whole_joint_9_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_a_hold_whole_joint_10_angles[87] = {
#include "shared/golem_clips/golem_motion_hands_down_a_hold_whole_joint_10_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_a_hold_whole_joint_11_angles[104] = {
#include "shared/golem_clips/golem_motion_hands_down_a_hold_whole_joint_11_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_a_hold_whole_joint_12_angles[92] = {
#include "shared/golem_clips/golem_motion_hands_down_a_hold_whole_joint_12_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_a_hold_whole_joint_13_angles[74] = {
#include "shared/golem_clips/golem_motion_hands_down_a_hold_whole_joint_13_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_a_hold_whole_joint_14_angles[94] = {
#include "shared/golem_clips/golem_motion_hands_down_a_hold_whole_joint_14_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_a_hold_whole_joint_15_angles[117] = {
#include "shared/golem_clips/golem_motion_hands_down_a_hold_whole_joint_15_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_a_hold_whole_joint_16_angles[42] = {
#include "shared/golem_clips/golem_motion_hands_down_a_hold_whole_joint_16_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_a_hold_whole_joint_17_angles[99] = {
#include "shared/golem_clips/golem_motion_hands_down_a_hold_whole_joint_17_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_a_hold_whole_joint_18_angles[21] = {
#include "shared/golem_clips/golem_motion_hands_down_a_hold_whole_joint_18_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_a_hold_whole_joint_19_angles[13] = {
#include "shared/golem_clips/golem_motion_hands_down_a_hold_whole_joint_19_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_a_hold_whole_joint_20_angles[116] = {
#include "shared/golem_clips/golem_motion_hands_down_a_hold_whole_joint_20_angles.inc"
};
static NJS_MDATA3 golem_motion_hands_down_a_hold_whole_tracks[21] = {
#include "shared/golem_clips/golem_motion_hands_down_a_hold_whole_tracks.inc"
};
NJS_MOTION golem_motion_hands_down_a_hold_whole = {
#include "shared/golem_clips/golem_motion_hands_down_a_hold_whole.inc"
};
static NJS_MKEY_A golem_motion_hands_down_a_rise_whole_joint_0_angles[2] = {
#include "shared/golem_clips/golem_motion_hands_down_a_rise_whole_joint_0_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_a_rise_whole_joint_1_angles[2] = {
#include "shared/golem_clips/golem_motion_hands_down_a_rise_whole_joint_1_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_a_rise_whole_joint_2_angles[2] = {
#include "shared/golem_clips/golem_motion_hands_down_a_rise_whole_joint_2_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_a_rise_whole_joint_3_angles[2] = {
#include "shared/golem_clips/golem_motion_hands_down_a_rise_whole_joint_3_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_a_rise_whole_joint_4_angles[2] = {
#include "shared/golem_clips/golem_motion_hands_down_a_rise_whole_joint_4_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_a_rise_whole_joint_5_angles[40] = {
#include "shared/golem_clips/golem_motion_hands_down_a_rise_whole_joint_5_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_a_rise_whole_joint_6_angles[40] = {
#include "shared/golem_clips/golem_motion_hands_down_a_rise_whole_joint_6_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_a_rise_whole_joint_7_angles[40] = {
#include "shared/golem_clips/golem_motion_hands_down_a_rise_whole_joint_7_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_a_rise_whole_joint_8_angles[40] = {
#include "shared/golem_clips/golem_motion_hands_down_a_rise_whole_joint_8_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_a_rise_whole_joint_9_angles[40] = {
#include "shared/golem_clips/golem_motion_hands_down_a_rise_whole_joint_9_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_a_rise_whole_joint_10_angles[40] = {
#include "shared/golem_clips/golem_motion_hands_down_a_rise_whole_joint_10_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_a_rise_whole_joint_11_angles[40] = {
#include "shared/golem_clips/golem_motion_hands_down_a_rise_whole_joint_11_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_a_rise_whole_joint_12_angles[40] = {
#include "shared/golem_clips/golem_motion_hands_down_a_rise_whole_joint_12_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_a_rise_whole_joint_13_angles[40] = {
#include "shared/golem_clips/golem_motion_hands_down_a_rise_whole_joint_13_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_a_rise_whole_joint_14_angles[40] = {
#include "shared/golem_clips/golem_motion_hands_down_a_rise_whole_joint_14_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_a_rise_whole_joint_15_angles[40] = {
#include "shared/golem_clips/golem_motion_hands_down_a_rise_whole_joint_15_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_a_rise_whole_joint_16_angles[40] = {
#include "shared/golem_clips/golem_motion_hands_down_a_rise_whole_joint_16_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_a_rise_whole_joint_17_angles[40] = {
#include "shared/golem_clips/golem_motion_hands_down_a_rise_whole_joint_17_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_a_rise_whole_joint_18_angles[40] = {
#include "shared/golem_clips/golem_motion_hands_down_a_rise_whole_joint_18_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_a_rise_whole_joint_19_angles[40] = {
#include "shared/golem_clips/golem_motion_hands_down_a_rise_whole_joint_19_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_a_rise_whole_joint_20_angles[40] = {
#include "shared/golem_clips/golem_motion_hands_down_a_rise_whole_joint_20_angles.inc"
};
static NJS_MDATA3 golem_motion_hands_down_a_rise_whole_tracks[21] = {
#include "shared/golem_clips/golem_motion_hands_down_a_rise_whole_tracks.inc"
};
NJS_MOTION golem_motion_hands_down_a_rise_whole = {
#include "shared/golem_clips/golem_motion_hands_down_a_rise_whole.inc"
};
static NJS_MKEY_A golem_motion_hands_down_a_lower_sunk_joint_0_angles[2] = {
#include "shared/golem_clips/golem_motion_hands_down_a_lower_sunk_joint_0_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_a_lower_sunk_joint_1_angles[2] = {
#include "shared/golem_clips/golem_motion_hands_down_a_lower_sunk_joint_1_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_a_lower_sunk_joint_2_angles[2] = {
#include "shared/golem_clips/golem_motion_hands_down_a_lower_sunk_joint_2_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_a_lower_sunk_joint_3_angles[2] = {
#include "shared/golem_clips/golem_motion_hands_down_a_lower_sunk_joint_3_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_a_lower_sunk_joint_4_angles[26] = {
#include "shared/golem_clips/golem_motion_hands_down_a_lower_sunk_joint_4_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_a_lower_sunk_joint_5_angles[40] = {
#include "shared/golem_clips/golem_motion_hands_down_a_lower_sunk_joint_5_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_a_lower_sunk_joint_6_angles[40] = {
#include "shared/golem_clips/golem_motion_hands_down_a_lower_sunk_joint_6_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_a_lower_sunk_joint_7_angles[40] = {
#include "shared/golem_clips/golem_motion_hands_down_a_lower_sunk_joint_7_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_a_lower_sunk_joint_8_angles[40] = {
#include "shared/golem_clips/golem_motion_hands_down_a_lower_sunk_joint_8_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_a_lower_sunk_joint_9_angles[40] = {
#include "shared/golem_clips/golem_motion_hands_down_a_lower_sunk_joint_9_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_a_lower_sunk_joint_10_angles[40] = {
#include "shared/golem_clips/golem_motion_hands_down_a_lower_sunk_joint_10_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_a_lower_sunk_joint_11_angles[40] = {
#include "shared/golem_clips/golem_motion_hands_down_a_lower_sunk_joint_11_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_a_lower_sunk_joint_12_angles[40] = {
#include "shared/golem_clips/golem_motion_hands_down_a_lower_sunk_joint_12_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_a_lower_sunk_joint_13_angles[40] = {
#include "shared/golem_clips/golem_motion_hands_down_a_lower_sunk_joint_13_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_a_lower_sunk_joint_14_angles[40] = {
#include "shared/golem_clips/golem_motion_hands_down_a_lower_sunk_joint_14_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_a_lower_sunk_joint_15_angles[40] = {
#include "shared/golem_clips/golem_motion_hands_down_a_lower_sunk_joint_15_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_a_lower_sunk_joint_16_angles[40] = {
#include "shared/golem_clips/golem_motion_hands_down_a_lower_sunk_joint_16_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_a_lower_sunk_joint_17_angles[40] = {
#include "shared/golem_clips/golem_motion_hands_down_a_lower_sunk_joint_17_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_a_lower_sunk_joint_18_angles[40] = {
#include "shared/golem_clips/golem_motion_hands_down_a_lower_sunk_joint_18_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_a_lower_sunk_joint_19_angles[40] = {
#include "shared/golem_clips/golem_motion_hands_down_a_lower_sunk_joint_19_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_a_lower_sunk_joint_20_angles[40] = {
#include "shared/golem_clips/golem_motion_hands_down_a_lower_sunk_joint_20_angles.inc"
};
static NJS_MDATA3 golem_motion_hands_down_a_lower_sunk_tracks[21] = {
#include "shared/golem_clips/golem_motion_hands_down_a_lower_sunk_tracks.inc"
};
NJS_MOTION golem_motion_hands_down_a_lower_sunk = {
#include "shared/golem_clips/golem_motion_hands_down_a_lower_sunk.inc"
};
static NJS_MKEY_A golem_motion_hands_down_a_hold_sunk_joint_0_angles[2] = {
#include "shared/golem_clips/golem_motion_hands_down_a_hold_sunk_joint_0_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_a_hold_sunk_joint_1_angles[2] = {
#include "shared/golem_clips/golem_motion_hands_down_a_hold_sunk_joint_1_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_a_hold_sunk_joint_2_angles[2] = {
#include "shared/golem_clips/golem_motion_hands_down_a_hold_sunk_joint_2_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_a_hold_sunk_joint_3_angles[2] = {
#include "shared/golem_clips/golem_motion_hands_down_a_hold_sunk_joint_3_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_a_hold_sunk_joint_4_angles[140] = {
#include "shared/golem_clips/golem_motion_hands_down_a_hold_sunk_joint_4_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_a_hold_sunk_joint_5_angles[45] = {
#include "shared/golem_clips/golem_motion_hands_down_a_hold_sunk_joint_5_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_a_hold_sunk_joint_6_angles[140] = {
#include "shared/golem_clips/golem_motion_hands_down_a_hold_sunk_joint_6_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_a_hold_sunk_joint_7_angles[139] = {
#include "shared/golem_clips/golem_motion_hands_down_a_hold_sunk_joint_7_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_a_hold_sunk_joint_8_angles[140] = {
#include "shared/golem_clips/golem_motion_hands_down_a_hold_sunk_joint_8_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_a_hold_sunk_joint_9_angles[85] = {
#include "shared/golem_clips/golem_motion_hands_down_a_hold_sunk_joint_9_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_a_hold_sunk_joint_10_angles[87] = {
#include "shared/golem_clips/golem_motion_hands_down_a_hold_sunk_joint_10_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_a_hold_sunk_joint_11_angles[104] = {
#include "shared/golem_clips/golem_motion_hands_down_a_hold_sunk_joint_11_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_a_hold_sunk_joint_12_angles[92] = {
#include "shared/golem_clips/golem_motion_hands_down_a_hold_sunk_joint_12_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_a_hold_sunk_joint_13_angles[67] = {
#include "shared/golem_clips/golem_motion_hands_down_a_hold_sunk_joint_13_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_a_hold_sunk_joint_14_angles[94] = {
#include "shared/golem_clips/golem_motion_hands_down_a_hold_sunk_joint_14_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_a_hold_sunk_joint_15_angles[124] = {
#include "shared/golem_clips/golem_motion_hands_down_a_hold_sunk_joint_15_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_a_hold_sunk_joint_16_angles[42] = {
#include "shared/golem_clips/golem_motion_hands_down_a_hold_sunk_joint_16_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_a_hold_sunk_joint_17_angles[99] = {
#include "shared/golem_clips/golem_motion_hands_down_a_hold_sunk_joint_17_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_a_hold_sunk_joint_18_angles[21] = {
#include "shared/golem_clips/golem_motion_hands_down_a_hold_sunk_joint_18_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_a_hold_sunk_joint_19_angles[13] = {
#include "shared/golem_clips/golem_motion_hands_down_a_hold_sunk_joint_19_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_a_hold_sunk_joint_20_angles[116] = {
#include "shared/golem_clips/golem_motion_hands_down_a_hold_sunk_joint_20_angles.inc"
};
static NJS_MDATA3 golem_motion_hands_down_a_hold_sunk_tracks[21] = {
#include "shared/golem_clips/golem_motion_hands_down_a_hold_sunk_tracks.inc"
};
NJS_MOTION golem_motion_hands_down_a_hold_sunk = {
#include "shared/golem_clips/golem_motion_hands_down_a_hold_sunk.inc"
};
static NJS_MKEY_A golem_motion_hands_down_a_rise_sunk_joint_0_angles[2] = {
#include "shared/golem_clips/golem_motion_hands_down_a_rise_sunk_joint_0_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_a_rise_sunk_joint_1_angles[2] = {
#include "shared/golem_clips/golem_motion_hands_down_a_rise_sunk_joint_1_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_a_rise_sunk_joint_2_angles[2] = {
#include "shared/golem_clips/golem_motion_hands_down_a_rise_sunk_joint_2_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_a_rise_sunk_joint_3_angles[2] = {
#include "shared/golem_clips/golem_motion_hands_down_a_rise_sunk_joint_3_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_a_rise_sunk_joint_4_angles[2] = {
#include "shared/golem_clips/golem_motion_hands_down_a_rise_sunk_joint_4_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_a_rise_sunk_joint_5_angles[40] = {
#include "shared/golem_clips/golem_motion_hands_down_a_rise_sunk_joint_5_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_a_rise_sunk_joint_6_angles[40] = {
#include "shared/golem_clips/golem_motion_hands_down_a_rise_sunk_joint_6_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_a_rise_sunk_joint_7_angles[40] = {
#include "shared/golem_clips/golem_motion_hands_down_a_rise_sunk_joint_7_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_a_rise_sunk_joint_8_angles[40] = {
#include "shared/golem_clips/golem_motion_hands_down_a_rise_sunk_joint_8_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_a_rise_sunk_joint_9_angles[40] = {
#include "shared/golem_clips/golem_motion_hands_down_a_rise_sunk_joint_9_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_a_rise_sunk_joint_10_angles[40] = {
#include "shared/golem_clips/golem_motion_hands_down_a_rise_sunk_joint_10_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_a_rise_sunk_joint_11_angles[40] = {
#include "shared/golem_clips/golem_motion_hands_down_a_rise_sunk_joint_11_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_a_rise_sunk_joint_12_angles[40] = {
#include "shared/golem_clips/golem_motion_hands_down_a_rise_sunk_joint_12_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_a_rise_sunk_joint_13_angles[40] = {
#include "shared/golem_clips/golem_motion_hands_down_a_rise_sunk_joint_13_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_a_rise_sunk_joint_14_angles[40] = {
#include "shared/golem_clips/golem_motion_hands_down_a_rise_sunk_joint_14_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_a_rise_sunk_joint_15_angles[40] = {
#include "shared/golem_clips/golem_motion_hands_down_a_rise_sunk_joint_15_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_a_rise_sunk_joint_16_angles[40] = {
#include "shared/golem_clips/golem_motion_hands_down_a_rise_sunk_joint_16_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_a_rise_sunk_joint_17_angles[40] = {
#include "shared/golem_clips/golem_motion_hands_down_a_rise_sunk_joint_17_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_a_rise_sunk_joint_18_angles[40] = {
#include "shared/golem_clips/golem_motion_hands_down_a_rise_sunk_joint_18_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_a_rise_sunk_joint_19_angles[40] = {
#include "shared/golem_clips/golem_motion_hands_down_a_rise_sunk_joint_19_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_a_rise_sunk_joint_20_angles[40] = {
#include "shared/golem_clips/golem_motion_hands_down_a_rise_sunk_joint_20_angles.inc"
};
static NJS_MDATA3 golem_motion_hands_down_a_rise_sunk_tracks[21] = {
#include "shared/golem_clips/golem_motion_hands_down_a_rise_sunk_tracks.inc"
};
NJS_MOTION golem_motion_hands_down_a_rise_sunk = {
#include "shared/golem_clips/golem_motion_hands_down_a_rise_sunk.inc"
};
static NJS_MKEY_A golem_motion_hands_down_a_lower_sunk_twice_joint_0_angles[2] =
    {
#include "shared/golem_clips/golem_motion_hands_down_a_lower_sunk_twice_joint_0_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_a_lower_sunk_twice_joint_1_angles[2] =
    {
#include "shared/golem_clips/golem_motion_hands_down_a_lower_sunk_twice_joint_1_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_a_lower_sunk_twice_joint_2_angles[2] =
    {
#include "shared/golem_clips/golem_motion_hands_down_a_lower_sunk_twice_joint_2_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_a_lower_sunk_twice_joint_3_angles[2] =
    {
#include "shared/golem_clips/golem_motion_hands_down_a_lower_sunk_twice_joint_3_angles.inc"
};
static NJS_MKEY_A
    golem_motion_hands_down_a_lower_sunk_twice_joint_4_angles[26] = {
#include "shared/golem_clips/golem_motion_hands_down_a_lower_sunk_twice_joint_4_angles.inc"
};
static NJS_MKEY_A
    golem_motion_hands_down_a_lower_sunk_twice_joint_5_angles[40] = {
#include "shared/golem_clips/golem_motion_hands_down_a_lower_sunk_twice_joint_5_angles.inc"
};
static NJS_MKEY_A
    golem_motion_hands_down_a_lower_sunk_twice_joint_6_angles[40] = {
#include "shared/golem_clips/golem_motion_hands_down_a_lower_sunk_twice_joint_6_angles.inc"
};
static NJS_MKEY_A
    golem_motion_hands_down_a_lower_sunk_twice_joint_7_angles[40] = {
#include "shared/golem_clips/golem_motion_hands_down_a_lower_sunk_twice_joint_7_angles.inc"
};
static NJS_MKEY_A
    golem_motion_hands_down_a_lower_sunk_twice_joint_8_angles[40] = {
#include "shared/golem_clips/golem_motion_hands_down_a_lower_sunk_twice_joint_8_angles.inc"
};
static NJS_MKEY_A
    golem_motion_hands_down_a_lower_sunk_twice_joint_9_angles[40] = {
#include "shared/golem_clips/golem_motion_hands_down_a_lower_sunk_twice_joint_9_angles.inc"
};
static NJS_MKEY_A
    golem_motion_hands_down_a_lower_sunk_twice_joint_10_angles[40] = {
#include "shared/golem_clips/golem_motion_hands_down_a_lower_sunk_twice_joint_10_angles.inc"
};
static NJS_MKEY_A
    golem_motion_hands_down_a_lower_sunk_twice_joint_11_angles[40] = {
#include "shared/golem_clips/golem_motion_hands_down_a_lower_sunk_twice_joint_11_angles.inc"
};
static NJS_MKEY_A
    golem_motion_hands_down_a_lower_sunk_twice_joint_12_angles[40] = {
#include "shared/golem_clips/golem_motion_hands_down_a_lower_sunk_twice_joint_12_angles.inc"
};
static NJS_MKEY_A
    golem_motion_hands_down_a_lower_sunk_twice_joint_13_angles[40] = {
#include "shared/golem_clips/golem_motion_hands_down_a_lower_sunk_twice_joint_13_angles.inc"
};
static NJS_MKEY_A
    golem_motion_hands_down_a_lower_sunk_twice_joint_14_angles[40] = {
#include "shared/golem_clips/golem_motion_hands_down_a_lower_sunk_twice_joint_14_angles.inc"
};
static NJS_MKEY_A
    golem_motion_hands_down_a_lower_sunk_twice_joint_15_angles[40] = {
#include "shared/golem_clips/golem_motion_hands_down_a_lower_sunk_twice_joint_15_angles.inc"
};
static NJS_MKEY_A
    golem_motion_hands_down_a_lower_sunk_twice_joint_16_angles[40] = {
#include "shared/golem_clips/golem_motion_hands_down_a_lower_sunk_twice_joint_16_angles.inc"
};
static NJS_MKEY_A
    golem_motion_hands_down_a_lower_sunk_twice_joint_17_angles[40] = {
#include "shared/golem_clips/golem_motion_hands_down_a_lower_sunk_twice_joint_17_angles.inc"
};
static NJS_MKEY_A
    golem_motion_hands_down_a_lower_sunk_twice_joint_18_angles[40] = {
#include "shared/golem_clips/golem_motion_hands_down_a_lower_sunk_twice_joint_18_angles.inc"
};
static NJS_MKEY_A
    golem_motion_hands_down_a_lower_sunk_twice_joint_19_angles[40] = {
#include "shared/golem_clips/golem_motion_hands_down_a_lower_sunk_twice_joint_19_angles.inc"
};
static NJS_MKEY_A
    golem_motion_hands_down_a_lower_sunk_twice_joint_20_angles[40] = {
#include "shared/golem_clips/golem_motion_hands_down_a_lower_sunk_twice_joint_20_angles.inc"
};
static NJS_MDATA3 golem_motion_hands_down_a_lower_sunk_twice_tracks[21] = {
#include "shared/golem_clips/golem_motion_hands_down_a_lower_sunk_twice_tracks.inc"
};
NJS_MOTION golem_motion_hands_down_a_lower_sunk_twice = {
#include "shared/golem_clips/golem_motion_hands_down_a_lower_sunk_twice.inc"
};
static NJS_MKEY_A golem_motion_hands_down_a_hold_sunk_twice_joint_0_angles[2] =
    {
#include "shared/golem_clips/golem_motion_hands_down_a_hold_sunk_twice_joint_0_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_a_hold_sunk_twice_joint_1_angles[2] =
    {
#include "shared/golem_clips/golem_motion_hands_down_a_hold_sunk_twice_joint_1_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_a_hold_sunk_twice_joint_2_angles[2] =
    {
#include "shared/golem_clips/golem_motion_hands_down_a_hold_sunk_twice_joint_2_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_a_hold_sunk_twice_joint_3_angles[2] =
    {
#include "shared/golem_clips/golem_motion_hands_down_a_hold_sunk_twice_joint_3_angles.inc"
};
static NJS_MKEY_A
    golem_motion_hands_down_a_hold_sunk_twice_joint_4_angles[140] = {
#include "shared/golem_clips/golem_motion_hands_down_a_hold_sunk_twice_joint_4_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_a_hold_sunk_twice_joint_5_angles[45] =
    {
#include "shared/golem_clips/golem_motion_hands_down_a_hold_sunk_twice_joint_5_angles.inc"
};
static NJS_MKEY_A
    golem_motion_hands_down_a_hold_sunk_twice_joint_6_angles[140] = {
#include "shared/golem_clips/golem_motion_hands_down_a_hold_sunk_twice_joint_6_angles.inc"
};
static NJS_MKEY_A
    golem_motion_hands_down_a_hold_sunk_twice_joint_7_angles[139] = {
#include "shared/golem_clips/golem_motion_hands_down_a_hold_sunk_twice_joint_7_angles.inc"
};
static NJS_MKEY_A
    golem_motion_hands_down_a_hold_sunk_twice_joint_8_angles[140] = {
#include "shared/golem_clips/golem_motion_hands_down_a_hold_sunk_twice_joint_8_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_a_hold_sunk_twice_joint_9_angles[85] =
    {
#include "shared/golem_clips/golem_motion_hands_down_a_hold_sunk_twice_joint_9_angles.inc"
};
static NJS_MKEY_A
    golem_motion_hands_down_a_hold_sunk_twice_joint_10_angles[87] = {
#include "shared/golem_clips/golem_motion_hands_down_a_hold_sunk_twice_joint_10_angles.inc"
};
static NJS_MKEY_A
    golem_motion_hands_down_a_hold_sunk_twice_joint_11_angles[104] = {
#include "shared/golem_clips/golem_motion_hands_down_a_hold_sunk_twice_joint_11_angles.inc"
};
static NJS_MKEY_A
    golem_motion_hands_down_a_hold_sunk_twice_joint_12_angles[92] = {
#include "shared/golem_clips/golem_motion_hands_down_a_hold_sunk_twice_joint_12_angles.inc"
};
static NJS_MKEY_A
    golem_motion_hands_down_a_hold_sunk_twice_joint_13_angles[74] = {
#include "shared/golem_clips/golem_motion_hands_down_a_hold_sunk_twice_joint_13_angles.inc"
};
static NJS_MKEY_A
    golem_motion_hands_down_a_hold_sunk_twice_joint_14_angles[94] = {
#include "shared/golem_clips/golem_motion_hands_down_a_hold_sunk_twice_joint_14_angles.inc"
};
static NJS_MKEY_A
    golem_motion_hands_down_a_hold_sunk_twice_joint_15_angles[137] = {
#include "shared/golem_clips/golem_motion_hands_down_a_hold_sunk_twice_joint_15_angles.inc"
};
static NJS_MKEY_A
    golem_motion_hands_down_a_hold_sunk_twice_joint_16_angles[42] = {
#include "shared/golem_clips/golem_motion_hands_down_a_hold_sunk_twice_joint_16_angles.inc"
};
static NJS_MKEY_A
    golem_motion_hands_down_a_hold_sunk_twice_joint_17_angles[99] = {
#include "shared/golem_clips/golem_motion_hands_down_a_hold_sunk_twice_joint_17_angles.inc"
};
static NJS_MKEY_A
    golem_motion_hands_down_a_hold_sunk_twice_joint_18_angles[21] = {
#include "shared/golem_clips/golem_motion_hands_down_a_hold_sunk_twice_joint_18_angles.inc"
};
static NJS_MKEY_A
    golem_motion_hands_down_a_hold_sunk_twice_joint_19_angles[13] = {
#include "shared/golem_clips/golem_motion_hands_down_a_hold_sunk_twice_joint_19_angles.inc"
};
static NJS_MKEY_A
    golem_motion_hands_down_a_hold_sunk_twice_joint_20_angles[116] = {
#include "shared/golem_clips/golem_motion_hands_down_a_hold_sunk_twice_joint_20_angles.inc"
};
static NJS_MDATA3 golem_motion_hands_down_a_hold_sunk_twice_tracks[21] = {
#include "shared/golem_clips/golem_motion_hands_down_a_hold_sunk_twice_tracks.inc"
};
NJS_MOTION golem_motion_hands_down_a_hold_sunk_twice = {
#include "shared/golem_clips/golem_motion_hands_down_a_hold_sunk_twice.inc"
};
static NJS_MKEY_A golem_motion_hands_down_a_rise_sunk_twice_joint_0_angles[2] =
    {
#include "shared/golem_clips/golem_motion_hands_down_a_rise_sunk_twice_joint_0_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_a_rise_sunk_twice_joint_1_angles[2] =
    {
#include "shared/golem_clips/golem_motion_hands_down_a_rise_sunk_twice_joint_1_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_a_rise_sunk_twice_joint_2_angles[2] =
    {
#include "shared/golem_clips/golem_motion_hands_down_a_rise_sunk_twice_joint_2_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_a_rise_sunk_twice_joint_3_angles[2] =
    {
#include "shared/golem_clips/golem_motion_hands_down_a_rise_sunk_twice_joint_3_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_a_rise_sunk_twice_joint_4_angles[2] =
    {
#include "shared/golem_clips/golem_motion_hands_down_a_rise_sunk_twice_joint_4_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_a_rise_sunk_twice_joint_5_angles[40] =
    {
#include "shared/golem_clips/golem_motion_hands_down_a_rise_sunk_twice_joint_5_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_a_rise_sunk_twice_joint_6_angles[40] =
    {
#include "shared/golem_clips/golem_motion_hands_down_a_rise_sunk_twice_joint_6_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_a_rise_sunk_twice_joint_7_angles[40] =
    {
#include "shared/golem_clips/golem_motion_hands_down_a_rise_sunk_twice_joint_7_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_a_rise_sunk_twice_joint_8_angles[40] =
    {
#include "shared/golem_clips/golem_motion_hands_down_a_rise_sunk_twice_joint_8_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_a_rise_sunk_twice_joint_9_angles[40] =
    {
#include "shared/golem_clips/golem_motion_hands_down_a_rise_sunk_twice_joint_9_angles.inc"
};
static NJS_MKEY_A
    golem_motion_hands_down_a_rise_sunk_twice_joint_10_angles[40] = {
#include "shared/golem_clips/golem_motion_hands_down_a_rise_sunk_twice_joint_10_angles.inc"
};
static NJS_MKEY_A
    golem_motion_hands_down_a_rise_sunk_twice_joint_11_angles[40] = {
#include "shared/golem_clips/golem_motion_hands_down_a_rise_sunk_twice_joint_11_angles.inc"
};
static NJS_MKEY_A
    golem_motion_hands_down_a_rise_sunk_twice_joint_12_angles[40] = {
#include "shared/golem_clips/golem_motion_hands_down_a_rise_sunk_twice_joint_12_angles.inc"
};
static NJS_MKEY_A
    golem_motion_hands_down_a_rise_sunk_twice_joint_13_angles[40] = {
#include "shared/golem_clips/golem_motion_hands_down_a_rise_sunk_twice_joint_13_angles.inc"
};
static NJS_MKEY_A
    golem_motion_hands_down_a_rise_sunk_twice_joint_14_angles[40] = {
#include "shared/golem_clips/golem_motion_hands_down_a_rise_sunk_twice_joint_14_angles.inc"
};
static NJS_MKEY_A
    golem_motion_hands_down_a_rise_sunk_twice_joint_15_angles[40] = {
#include "shared/golem_clips/golem_motion_hands_down_a_rise_sunk_twice_joint_15_angles.inc"
};
static NJS_MKEY_A
    golem_motion_hands_down_a_rise_sunk_twice_joint_16_angles[40] = {
#include "shared/golem_clips/golem_motion_hands_down_a_rise_sunk_twice_joint_16_angles.inc"
};
static NJS_MKEY_A
    golem_motion_hands_down_a_rise_sunk_twice_joint_17_angles[40] = {
#include "shared/golem_clips/golem_motion_hands_down_a_rise_sunk_twice_joint_17_angles.inc"
};
static NJS_MKEY_A
    golem_motion_hands_down_a_rise_sunk_twice_joint_18_angles[40] = {
#include "shared/golem_clips/golem_motion_hands_down_a_rise_sunk_twice_joint_18_angles.inc"
};
static NJS_MKEY_A
    golem_motion_hands_down_a_rise_sunk_twice_joint_19_angles[40] = {
#include "shared/golem_clips/golem_motion_hands_down_a_rise_sunk_twice_joint_19_angles.inc"
};
static NJS_MKEY_A
    golem_motion_hands_down_a_rise_sunk_twice_joint_20_angles[40] = {
#include "shared/golem_clips/golem_motion_hands_down_a_rise_sunk_twice_joint_20_angles.inc"
};
static NJS_MDATA3 golem_motion_hands_down_a_rise_sunk_twice_tracks[21] = {
#include "shared/golem_clips/golem_motion_hands_down_a_rise_sunk_twice_tracks.inc"
};
NJS_MOTION golem_motion_hands_down_a_rise_sunk_twice = {
#include "shared/golem_clips/golem_motion_hands_down_a_rise_sunk_twice.inc"
};
static NJS_MKEY_A golem_motion_hands_down_b_lower_whole_joint_0_angles[2] = {
#include "shared/golem_clips/golem_motion_hands_down_b_lower_whole_joint_0_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_b_lower_whole_joint_1_angles[2] = {
#include "shared/golem_clips/golem_motion_hands_down_b_lower_whole_joint_1_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_b_lower_whole_joint_2_angles[2] = {
#include "shared/golem_clips/golem_motion_hands_down_b_lower_whole_joint_2_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_b_lower_whole_joint_3_angles[2] = {
#include "shared/golem_clips/golem_motion_hands_down_b_lower_whole_joint_3_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_b_lower_whole_joint_4_angles[40] = {
#include "shared/golem_clips/golem_motion_hands_down_b_lower_whole_joint_4_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_b_lower_whole_joint_5_angles[40] = {
#include "shared/golem_clips/golem_motion_hands_down_b_lower_whole_joint_5_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_b_lower_whole_joint_6_angles[40] = {
#include "shared/golem_clips/golem_motion_hands_down_b_lower_whole_joint_6_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_b_lower_whole_joint_7_angles[40] = {
#include "shared/golem_clips/golem_motion_hands_down_b_lower_whole_joint_7_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_b_lower_whole_joint_8_angles[40] = {
#include "shared/golem_clips/golem_motion_hands_down_b_lower_whole_joint_8_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_b_lower_whole_joint_9_angles[40] = {
#include "shared/golem_clips/golem_motion_hands_down_b_lower_whole_joint_9_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_b_lower_whole_joint_10_angles[40] = {
#include "shared/golem_clips/golem_motion_hands_down_b_lower_whole_joint_10_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_b_lower_whole_joint_11_angles[40] = {
#include "shared/golem_clips/golem_motion_hands_down_b_lower_whole_joint_11_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_b_lower_whole_joint_12_angles[40] = {
#include "shared/golem_clips/golem_motion_hands_down_b_lower_whole_joint_12_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_b_lower_whole_joint_13_angles[40] = {
#include "shared/golem_clips/golem_motion_hands_down_b_lower_whole_joint_13_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_b_lower_whole_joint_14_angles[40] = {
#include "shared/golem_clips/golem_motion_hands_down_b_lower_whole_joint_14_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_b_lower_whole_joint_15_angles[40] = {
#include "shared/golem_clips/golem_motion_hands_down_b_lower_whole_joint_15_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_b_lower_whole_joint_16_angles[40] = {
#include "shared/golem_clips/golem_motion_hands_down_b_lower_whole_joint_16_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_b_lower_whole_joint_17_angles[40] = {
#include "shared/golem_clips/golem_motion_hands_down_b_lower_whole_joint_17_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_b_lower_whole_joint_18_angles[40] = {
#include "shared/golem_clips/golem_motion_hands_down_b_lower_whole_joint_18_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_b_lower_whole_joint_19_angles[40] = {
#include "shared/golem_clips/golem_motion_hands_down_b_lower_whole_joint_19_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_b_lower_whole_joint_20_angles[40] = {
#include "shared/golem_clips/golem_motion_hands_down_b_lower_whole_joint_20_angles.inc"
};
static NJS_MDATA3 golem_motion_hands_down_b_lower_whole_tracks[21] = {
#include "shared/golem_clips/golem_motion_hands_down_b_lower_whole_tracks.inc"
};
NJS_MOTION golem_motion_hands_down_b_lower_whole = {
#include "shared/golem_clips/golem_motion_hands_down_b_lower_whole.inc"
};
static NJS_MKEY_A golem_motion_hands_down_b_hold_whole_joint_0_angles[2] = {
#include "shared/golem_clips/golem_motion_hands_down_b_hold_whole_joint_0_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_b_hold_whole_joint_1_angles[2] = {
#include "shared/golem_clips/golem_motion_hands_down_b_hold_whole_joint_1_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_b_hold_whole_joint_2_angles[2] = {
#include "shared/golem_clips/golem_motion_hands_down_b_hold_whole_joint_2_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_b_hold_whole_joint_3_angles[2] = {
#include "shared/golem_clips/golem_motion_hands_down_b_hold_whole_joint_3_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_b_hold_whole_joint_4_angles[140] = {
#include "shared/golem_clips/golem_motion_hands_down_b_hold_whole_joint_4_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_b_hold_whole_joint_5_angles[73] = {
#include "shared/golem_clips/golem_motion_hands_down_b_hold_whole_joint_5_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_b_hold_whole_joint_6_angles[129] = {
#include "shared/golem_clips/golem_motion_hands_down_b_hold_whole_joint_6_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_b_hold_whole_joint_7_angles[135] = {
#include "shared/golem_clips/golem_motion_hands_down_b_hold_whole_joint_7_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_b_hold_whole_joint_8_angles[140] = {
#include "shared/golem_clips/golem_motion_hands_down_b_hold_whole_joint_8_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_b_hold_whole_joint_9_angles[122] = {
#include "shared/golem_clips/golem_motion_hands_down_b_hold_whole_joint_9_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_b_hold_whole_joint_10_angles[33] = {
#include "shared/golem_clips/golem_motion_hands_down_b_hold_whole_joint_10_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_b_hold_whole_joint_11_angles[131] = {
#include "shared/golem_clips/golem_motion_hands_down_b_hold_whole_joint_11_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_b_hold_whole_joint_12_angles[135] = {
#include "shared/golem_clips/golem_motion_hands_down_b_hold_whole_joint_12_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_b_hold_whole_joint_13_angles[73] = {
#include "shared/golem_clips/golem_motion_hands_down_b_hold_whole_joint_13_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_b_hold_whole_joint_14_angles[104] = {
#include "shared/golem_clips/golem_motion_hands_down_b_hold_whole_joint_14_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_b_hold_whole_joint_15_angles[136] = {
#include "shared/golem_clips/golem_motion_hands_down_b_hold_whole_joint_15_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_b_hold_whole_joint_16_angles[125] = {
#include "shared/golem_clips/golem_motion_hands_down_b_hold_whole_joint_16_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_b_hold_whole_joint_17_angles[2] = {
#include "shared/golem_clips/golem_motion_hands_down_b_hold_whole_joint_17_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_b_hold_whole_joint_18_angles[115] = {
#include "shared/golem_clips/golem_motion_hands_down_b_hold_whole_joint_18_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_b_hold_whole_joint_19_angles[123] = {
#include "shared/golem_clips/golem_motion_hands_down_b_hold_whole_joint_19_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_b_hold_whole_joint_20_angles[33] = {
#include "shared/golem_clips/golem_motion_hands_down_b_hold_whole_joint_20_angles.inc"
};
static NJS_MDATA3 golem_motion_hands_down_b_hold_whole_tracks[21] = {
#include "shared/golem_clips/golem_motion_hands_down_b_hold_whole_tracks.inc"
};
NJS_MOTION golem_motion_hands_down_b_hold_whole = {
#include "shared/golem_clips/golem_motion_hands_down_b_hold_whole.inc"
};
static NJS_MKEY_A golem_motion_hands_down_b_rise_whole_joint_0_angles[2] = {
#include "shared/golem_clips/golem_motion_hands_down_b_rise_whole_joint_0_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_b_rise_whole_joint_1_angles[2] = {
#include "shared/golem_clips/golem_motion_hands_down_b_rise_whole_joint_1_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_b_rise_whole_joint_2_angles[2] = {
#include "shared/golem_clips/golem_motion_hands_down_b_rise_whole_joint_2_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_b_rise_whole_joint_3_angles[2] = {
#include "shared/golem_clips/golem_motion_hands_down_b_rise_whole_joint_3_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_b_rise_whole_joint_4_angles[2] = {
#include "shared/golem_clips/golem_motion_hands_down_b_rise_whole_joint_4_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_b_rise_whole_joint_5_angles[40] = {
#include "shared/golem_clips/golem_motion_hands_down_b_rise_whole_joint_5_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_b_rise_whole_joint_6_angles[40] = {
#include "shared/golem_clips/golem_motion_hands_down_b_rise_whole_joint_6_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_b_rise_whole_joint_7_angles[40] = {
#include "shared/golem_clips/golem_motion_hands_down_b_rise_whole_joint_7_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_b_rise_whole_joint_8_angles[40] = {
#include "shared/golem_clips/golem_motion_hands_down_b_rise_whole_joint_8_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_b_rise_whole_joint_9_angles[40] = {
#include "shared/golem_clips/golem_motion_hands_down_b_rise_whole_joint_9_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_b_rise_whole_joint_10_angles[40] = {
#include "shared/golem_clips/golem_motion_hands_down_b_rise_whole_joint_10_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_b_rise_whole_joint_11_angles[40] = {
#include "shared/golem_clips/golem_motion_hands_down_b_rise_whole_joint_11_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_b_rise_whole_joint_12_angles[40] = {
#include "shared/golem_clips/golem_motion_hands_down_b_rise_whole_joint_12_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_b_rise_whole_joint_13_angles[40] = {
#include "shared/golem_clips/golem_motion_hands_down_b_rise_whole_joint_13_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_b_rise_whole_joint_14_angles[40] = {
#include "shared/golem_clips/golem_motion_hands_down_b_rise_whole_joint_14_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_b_rise_whole_joint_15_angles[40] = {
#include "shared/golem_clips/golem_motion_hands_down_b_rise_whole_joint_15_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_b_rise_whole_joint_16_angles[40] = {
#include "shared/golem_clips/golem_motion_hands_down_b_rise_whole_joint_16_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_b_rise_whole_joint_17_angles[34] = {
#include "shared/golem_clips/golem_motion_hands_down_b_rise_whole_joint_17_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_b_rise_whole_joint_18_angles[40] = {
#include "shared/golem_clips/golem_motion_hands_down_b_rise_whole_joint_18_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_b_rise_whole_joint_19_angles[40] = {
#include "shared/golem_clips/golem_motion_hands_down_b_rise_whole_joint_19_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_b_rise_whole_joint_20_angles[12] = {
#include "shared/golem_clips/golem_motion_hands_down_b_rise_whole_joint_20_angles.inc"
};
static NJS_MDATA3 golem_motion_hands_down_b_rise_whole_tracks[21] = {
#include "shared/golem_clips/golem_motion_hands_down_b_rise_whole_tracks.inc"
};
NJS_MOTION golem_motion_hands_down_b_rise_whole = {
#include "shared/golem_clips/golem_motion_hands_down_b_rise_whole.inc"
};
static NJS_MKEY_A golem_motion_hands_down_b_lower_sunk_joint_0_angles[2] = {
#include "shared/golem_clips/golem_motion_hands_down_b_lower_sunk_joint_0_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_b_lower_sunk_joint_1_angles[2] = {
#include "shared/golem_clips/golem_motion_hands_down_b_lower_sunk_joint_1_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_b_lower_sunk_joint_2_angles[2] = {
#include "shared/golem_clips/golem_motion_hands_down_b_lower_sunk_joint_2_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_b_lower_sunk_joint_3_angles[2] = {
#include "shared/golem_clips/golem_motion_hands_down_b_lower_sunk_joint_3_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_b_lower_sunk_joint_4_angles[40] = {
#include "shared/golem_clips/golem_motion_hands_down_b_lower_sunk_joint_4_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_b_lower_sunk_joint_5_angles[40] = {
#include "shared/golem_clips/golem_motion_hands_down_b_lower_sunk_joint_5_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_b_lower_sunk_joint_6_angles[40] = {
#include "shared/golem_clips/golem_motion_hands_down_b_lower_sunk_joint_6_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_b_lower_sunk_joint_7_angles[40] = {
#include "shared/golem_clips/golem_motion_hands_down_b_lower_sunk_joint_7_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_b_lower_sunk_joint_8_angles[40] = {
#include "shared/golem_clips/golem_motion_hands_down_b_lower_sunk_joint_8_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_b_lower_sunk_joint_9_angles[40] = {
#include "shared/golem_clips/golem_motion_hands_down_b_lower_sunk_joint_9_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_b_lower_sunk_joint_10_angles[40] = {
#include "shared/golem_clips/golem_motion_hands_down_b_lower_sunk_joint_10_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_b_lower_sunk_joint_11_angles[40] = {
#include "shared/golem_clips/golem_motion_hands_down_b_lower_sunk_joint_11_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_b_lower_sunk_joint_12_angles[40] = {
#include "shared/golem_clips/golem_motion_hands_down_b_lower_sunk_joint_12_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_b_lower_sunk_joint_13_angles[40] = {
#include "shared/golem_clips/golem_motion_hands_down_b_lower_sunk_joint_13_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_b_lower_sunk_joint_14_angles[40] = {
#include "shared/golem_clips/golem_motion_hands_down_b_lower_sunk_joint_14_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_b_lower_sunk_joint_15_angles[40] = {
#include "shared/golem_clips/golem_motion_hands_down_b_lower_sunk_joint_15_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_b_lower_sunk_joint_16_angles[40] = {
#include "shared/golem_clips/golem_motion_hands_down_b_lower_sunk_joint_16_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_b_lower_sunk_joint_17_angles[40] = {
#include "shared/golem_clips/golem_motion_hands_down_b_lower_sunk_joint_17_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_b_lower_sunk_joint_18_angles[40] = {
#include "shared/golem_clips/golem_motion_hands_down_b_lower_sunk_joint_18_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_b_lower_sunk_joint_19_angles[40] = {
#include "shared/golem_clips/golem_motion_hands_down_b_lower_sunk_joint_19_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_b_lower_sunk_joint_20_angles[40] = {
#include "shared/golem_clips/golem_motion_hands_down_b_lower_sunk_joint_20_angles.inc"
};
static NJS_MDATA3 golem_motion_hands_down_b_lower_sunk_tracks[21] = {
#include "shared/golem_clips/golem_motion_hands_down_b_lower_sunk_tracks.inc"
};
NJS_MOTION golem_motion_hands_down_b_lower_sunk = {
#include "shared/golem_clips/golem_motion_hands_down_b_lower_sunk.inc"
};
static NJS_MKEY_A golem_motion_hands_down_b_hold_sunk_joint_0_angles[2] = {
#include "shared/golem_clips/golem_motion_hands_down_b_hold_sunk_joint_0_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_b_hold_sunk_joint_1_angles[2] = {
#include "shared/golem_clips/golem_motion_hands_down_b_hold_sunk_joint_1_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_b_hold_sunk_joint_2_angles[2] = {
#include "shared/golem_clips/golem_motion_hands_down_b_hold_sunk_joint_2_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_b_hold_sunk_joint_3_angles[2] = {
#include "shared/golem_clips/golem_motion_hands_down_b_hold_sunk_joint_3_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_b_hold_sunk_joint_4_angles[140] = {
#include "shared/golem_clips/golem_motion_hands_down_b_hold_sunk_joint_4_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_b_hold_sunk_joint_5_angles[73] = {
#include "shared/golem_clips/golem_motion_hands_down_b_hold_sunk_joint_5_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_b_hold_sunk_joint_6_angles[129] = {
#include "shared/golem_clips/golem_motion_hands_down_b_hold_sunk_joint_6_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_b_hold_sunk_joint_7_angles[135] = {
#include "shared/golem_clips/golem_motion_hands_down_b_hold_sunk_joint_7_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_b_hold_sunk_joint_8_angles[126] = {
#include "shared/golem_clips/golem_motion_hands_down_b_hold_sunk_joint_8_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_b_hold_sunk_joint_9_angles[122] = {
#include "shared/golem_clips/golem_motion_hands_down_b_hold_sunk_joint_9_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_b_hold_sunk_joint_10_angles[33] = {
#include "shared/golem_clips/golem_motion_hands_down_b_hold_sunk_joint_10_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_b_hold_sunk_joint_11_angles[81] = {
#include "shared/golem_clips/golem_motion_hands_down_b_hold_sunk_joint_11_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_b_hold_sunk_joint_12_angles[135] = {
#include "shared/golem_clips/golem_motion_hands_down_b_hold_sunk_joint_12_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_b_hold_sunk_joint_13_angles[65] = {
#include "shared/golem_clips/golem_motion_hands_down_b_hold_sunk_joint_13_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_b_hold_sunk_joint_14_angles[104] = {
#include "shared/golem_clips/golem_motion_hands_down_b_hold_sunk_joint_14_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_b_hold_sunk_joint_15_angles[140] = {
#include "shared/golem_clips/golem_motion_hands_down_b_hold_sunk_joint_15_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_b_hold_sunk_joint_16_angles[125] = {
#include "shared/golem_clips/golem_motion_hands_down_b_hold_sunk_joint_16_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_b_hold_sunk_joint_17_angles[2] = {
#include "shared/golem_clips/golem_motion_hands_down_b_hold_sunk_joint_17_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_b_hold_sunk_joint_18_angles[115] = {
#include "shared/golem_clips/golem_motion_hands_down_b_hold_sunk_joint_18_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_b_hold_sunk_joint_19_angles[123] = {
#include "shared/golem_clips/golem_motion_hands_down_b_hold_sunk_joint_19_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_b_hold_sunk_joint_20_angles[33] = {
#include "shared/golem_clips/golem_motion_hands_down_b_hold_sunk_joint_20_angles.inc"
};
static NJS_MDATA3 golem_motion_hands_down_b_hold_sunk_tracks[21] = {
#include "shared/golem_clips/golem_motion_hands_down_b_hold_sunk_tracks.inc"
};
NJS_MOTION golem_motion_hands_down_b_hold_sunk = {
#include "shared/golem_clips/golem_motion_hands_down_b_hold_sunk.inc"
};
static NJS_MKEY_A golem_motion_hands_down_b_rise_sunk_joint_0_angles[2] = {
#include "shared/golem_clips/golem_motion_hands_down_b_rise_sunk_joint_0_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_b_rise_sunk_joint_1_angles[2] = {
#include "shared/golem_clips/golem_motion_hands_down_b_rise_sunk_joint_1_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_b_rise_sunk_joint_2_angles[2] = {
#include "shared/golem_clips/golem_motion_hands_down_b_rise_sunk_joint_2_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_b_rise_sunk_joint_3_angles[2] = {
#include "shared/golem_clips/golem_motion_hands_down_b_rise_sunk_joint_3_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_b_rise_sunk_joint_4_angles[2] = {
#include "shared/golem_clips/golem_motion_hands_down_b_rise_sunk_joint_4_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_b_rise_sunk_joint_5_angles[40] = {
#include "shared/golem_clips/golem_motion_hands_down_b_rise_sunk_joint_5_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_b_rise_sunk_joint_6_angles[40] = {
#include "shared/golem_clips/golem_motion_hands_down_b_rise_sunk_joint_6_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_b_rise_sunk_joint_7_angles[40] = {
#include "shared/golem_clips/golem_motion_hands_down_b_rise_sunk_joint_7_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_b_rise_sunk_joint_8_angles[40] = {
#include "shared/golem_clips/golem_motion_hands_down_b_rise_sunk_joint_8_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_b_rise_sunk_joint_9_angles[40] = {
#include "shared/golem_clips/golem_motion_hands_down_b_rise_sunk_joint_9_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_b_rise_sunk_joint_10_angles[40] = {
#include "shared/golem_clips/golem_motion_hands_down_b_rise_sunk_joint_10_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_b_rise_sunk_joint_11_angles[40] = {
#include "shared/golem_clips/golem_motion_hands_down_b_rise_sunk_joint_11_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_b_rise_sunk_joint_12_angles[40] = {
#include "shared/golem_clips/golem_motion_hands_down_b_rise_sunk_joint_12_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_b_rise_sunk_joint_13_angles[40] = {
#include "shared/golem_clips/golem_motion_hands_down_b_rise_sunk_joint_13_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_b_rise_sunk_joint_14_angles[40] = {
#include "shared/golem_clips/golem_motion_hands_down_b_rise_sunk_joint_14_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_b_rise_sunk_joint_15_angles[40] = {
#include "shared/golem_clips/golem_motion_hands_down_b_rise_sunk_joint_15_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_b_rise_sunk_joint_16_angles[40] = {
#include "shared/golem_clips/golem_motion_hands_down_b_rise_sunk_joint_16_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_b_rise_sunk_joint_17_angles[34] = {
#include "shared/golem_clips/golem_motion_hands_down_b_rise_sunk_joint_17_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_b_rise_sunk_joint_18_angles[40] = {
#include "shared/golem_clips/golem_motion_hands_down_b_rise_sunk_joint_18_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_b_rise_sunk_joint_19_angles[40] = {
#include "shared/golem_clips/golem_motion_hands_down_b_rise_sunk_joint_19_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_b_rise_sunk_joint_20_angles[12] = {
#include "shared/golem_clips/golem_motion_hands_down_b_rise_sunk_joint_20_angles.inc"
};
static NJS_MDATA3 golem_motion_hands_down_b_rise_sunk_tracks[21] = {
#include "shared/golem_clips/golem_motion_hands_down_b_rise_sunk_tracks.inc"
};
NJS_MOTION golem_motion_hands_down_b_rise_sunk = {
#include "shared/golem_clips/golem_motion_hands_down_b_rise_sunk.inc"
};
static NJS_MKEY_A golem_motion_hands_down_b_lower_sunk_twice_joint_0_angles[2] =
    {
#include "shared/golem_clips/golem_motion_hands_down_b_lower_sunk_twice_joint_0_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_b_lower_sunk_twice_joint_1_angles[2] =
    {
#include "shared/golem_clips/golem_motion_hands_down_b_lower_sunk_twice_joint_1_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_b_lower_sunk_twice_joint_2_angles[2] =
    {
#include "shared/golem_clips/golem_motion_hands_down_b_lower_sunk_twice_joint_2_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_b_lower_sunk_twice_joint_3_angles[2] =
    {
#include "shared/golem_clips/golem_motion_hands_down_b_lower_sunk_twice_joint_3_angles.inc"
};
static NJS_MKEY_A
    golem_motion_hands_down_b_lower_sunk_twice_joint_4_angles[40] = {
#include "shared/golem_clips/golem_motion_hands_down_b_lower_sunk_twice_joint_4_angles.inc"
};
static NJS_MKEY_A
    golem_motion_hands_down_b_lower_sunk_twice_joint_5_angles[40] = {
#include "shared/golem_clips/golem_motion_hands_down_b_lower_sunk_twice_joint_5_angles.inc"
};
static NJS_MKEY_A
    golem_motion_hands_down_b_lower_sunk_twice_joint_6_angles[40] = {
#include "shared/golem_clips/golem_motion_hands_down_b_lower_sunk_twice_joint_6_angles.inc"
};
static NJS_MKEY_A
    golem_motion_hands_down_b_lower_sunk_twice_joint_7_angles[40] = {
#include "shared/golem_clips/golem_motion_hands_down_b_lower_sunk_twice_joint_7_angles.inc"
};
static NJS_MKEY_A
    golem_motion_hands_down_b_lower_sunk_twice_joint_8_angles[40] = {
#include "shared/golem_clips/golem_motion_hands_down_b_lower_sunk_twice_joint_8_angles.inc"
};
static NJS_MKEY_A
    golem_motion_hands_down_b_lower_sunk_twice_joint_9_angles[40] = {
#include "shared/golem_clips/golem_motion_hands_down_b_lower_sunk_twice_joint_9_angles.inc"
};
static NJS_MKEY_A
    golem_motion_hands_down_b_lower_sunk_twice_joint_10_angles[40] = {
#include "shared/golem_clips/golem_motion_hands_down_b_lower_sunk_twice_joint_10_angles.inc"
};
static NJS_MKEY_A
    golem_motion_hands_down_b_lower_sunk_twice_joint_11_angles[40] = {
#include "shared/golem_clips/golem_motion_hands_down_b_lower_sunk_twice_joint_11_angles.inc"
};
static NJS_MKEY_A
    golem_motion_hands_down_b_lower_sunk_twice_joint_12_angles[40] = {
#include "shared/golem_clips/golem_motion_hands_down_b_lower_sunk_twice_joint_12_angles.inc"
};
static NJS_MKEY_A
    golem_motion_hands_down_b_lower_sunk_twice_joint_13_angles[40] = {
#include "shared/golem_clips/golem_motion_hands_down_b_lower_sunk_twice_joint_13_angles.inc"
};
static NJS_MKEY_A
    golem_motion_hands_down_b_lower_sunk_twice_joint_14_angles[40] = {
#include "shared/golem_clips/golem_motion_hands_down_b_lower_sunk_twice_joint_14_angles.inc"
};
static NJS_MKEY_A
    golem_motion_hands_down_b_lower_sunk_twice_joint_15_angles[40] = {
#include "shared/golem_clips/golem_motion_hands_down_b_lower_sunk_twice_joint_15_angles.inc"
};
static NJS_MKEY_A
    golem_motion_hands_down_b_lower_sunk_twice_joint_16_angles[40] = {
#include "shared/golem_clips/golem_motion_hands_down_b_lower_sunk_twice_joint_16_angles.inc"
};
static NJS_MKEY_A
    golem_motion_hands_down_b_lower_sunk_twice_joint_17_angles[40] = {
#include "shared/golem_clips/golem_motion_hands_down_b_lower_sunk_twice_joint_17_angles.inc"
};
static NJS_MKEY_A
    golem_motion_hands_down_b_lower_sunk_twice_joint_18_angles[40] = {
#include "shared/golem_clips/golem_motion_hands_down_b_lower_sunk_twice_joint_18_angles.inc"
};
static NJS_MKEY_A
    golem_motion_hands_down_b_lower_sunk_twice_joint_19_angles[40] = {
#include "shared/golem_clips/golem_motion_hands_down_b_lower_sunk_twice_joint_19_angles.inc"
};
static NJS_MKEY_A
    golem_motion_hands_down_b_lower_sunk_twice_joint_20_angles[40] = {
#include "shared/golem_clips/golem_motion_hands_down_b_lower_sunk_twice_joint_20_angles.inc"
};
static NJS_MDATA3 golem_motion_hands_down_b_lower_sunk_twice_tracks[21] = {
#include "shared/golem_clips/golem_motion_hands_down_b_lower_sunk_twice_tracks.inc"
};
NJS_MOTION golem_motion_hands_down_b_lower_sunk_twice = {
#include "shared/golem_clips/golem_motion_hands_down_b_lower_sunk_twice.inc"
};
static NJS_MKEY_A golem_motion_hands_down_b_hold_sunk_twice_joint_0_angles[2] =
    {
#include "shared/golem_clips/golem_motion_hands_down_b_hold_sunk_twice_joint_0_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_b_hold_sunk_twice_joint_1_angles[2] =
    {
#include "shared/golem_clips/golem_motion_hands_down_b_hold_sunk_twice_joint_1_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_b_hold_sunk_twice_joint_2_angles[2] =
    {
#include "shared/golem_clips/golem_motion_hands_down_b_hold_sunk_twice_joint_2_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_b_hold_sunk_twice_joint_3_angles[2] =
    {
#include "shared/golem_clips/golem_motion_hands_down_b_hold_sunk_twice_joint_3_angles.inc"
};
static NJS_MKEY_A
    golem_motion_hands_down_b_hold_sunk_twice_joint_4_angles[140] = {
#include "shared/golem_clips/golem_motion_hands_down_b_hold_sunk_twice_joint_4_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_b_hold_sunk_twice_joint_5_angles[73] =
    {
#include "shared/golem_clips/golem_motion_hands_down_b_hold_sunk_twice_joint_5_angles.inc"
};
static NJS_MKEY_A
    golem_motion_hands_down_b_hold_sunk_twice_joint_6_angles[138] = {
#include "shared/golem_clips/golem_motion_hands_down_b_hold_sunk_twice_joint_6_angles.inc"
};
static NJS_MKEY_A
    golem_motion_hands_down_b_hold_sunk_twice_joint_7_angles[135] = {
#include "shared/golem_clips/golem_motion_hands_down_b_hold_sunk_twice_joint_7_angles.inc"
};
static NJS_MKEY_A
    golem_motion_hands_down_b_hold_sunk_twice_joint_8_angles[137] = {
#include "shared/golem_clips/golem_motion_hands_down_b_hold_sunk_twice_joint_8_angles.inc"
};
static NJS_MKEY_A
    golem_motion_hands_down_b_hold_sunk_twice_joint_9_angles[122] = {
#include "shared/golem_clips/golem_motion_hands_down_b_hold_sunk_twice_joint_9_angles.inc"
};
static NJS_MKEY_A
    golem_motion_hands_down_b_hold_sunk_twice_joint_10_angles[33] = {
#include "shared/golem_clips/golem_motion_hands_down_b_hold_sunk_twice_joint_10_angles.inc"
};
static NJS_MKEY_A
    golem_motion_hands_down_b_hold_sunk_twice_joint_11_angles[139] = {
#include "shared/golem_clips/golem_motion_hands_down_b_hold_sunk_twice_joint_11_angles.inc"
};
static NJS_MKEY_A
    golem_motion_hands_down_b_hold_sunk_twice_joint_12_angles[136] = {
#include "shared/golem_clips/golem_motion_hands_down_b_hold_sunk_twice_joint_12_angles.inc"
};
static NJS_MKEY_A
    golem_motion_hands_down_b_hold_sunk_twice_joint_13_angles[63] = {
#include "shared/golem_clips/golem_motion_hands_down_b_hold_sunk_twice_joint_13_angles.inc"
};
static NJS_MKEY_A
    golem_motion_hands_down_b_hold_sunk_twice_joint_14_angles[104] = {
#include "shared/golem_clips/golem_motion_hands_down_b_hold_sunk_twice_joint_14_angles.inc"
};
static NJS_MKEY_A
    golem_motion_hands_down_b_hold_sunk_twice_joint_15_angles[133] = {
#include "shared/golem_clips/golem_motion_hands_down_b_hold_sunk_twice_joint_15_angles.inc"
};
static NJS_MKEY_A
    golem_motion_hands_down_b_hold_sunk_twice_joint_16_angles[125] = {
#include "shared/golem_clips/golem_motion_hands_down_b_hold_sunk_twice_joint_16_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_b_hold_sunk_twice_joint_17_angles[2] =
    {
#include "shared/golem_clips/golem_motion_hands_down_b_hold_sunk_twice_joint_17_angles.inc"
};
static NJS_MKEY_A
    golem_motion_hands_down_b_hold_sunk_twice_joint_18_angles[115] = {
#include "shared/golem_clips/golem_motion_hands_down_b_hold_sunk_twice_joint_18_angles.inc"
};
static NJS_MKEY_A
    golem_motion_hands_down_b_hold_sunk_twice_joint_19_angles[123] = {
#include "shared/golem_clips/golem_motion_hands_down_b_hold_sunk_twice_joint_19_angles.inc"
};
static NJS_MKEY_A
    golem_motion_hands_down_b_hold_sunk_twice_joint_20_angles[33] = {
#include "shared/golem_clips/golem_motion_hands_down_b_hold_sunk_twice_joint_20_angles.inc"
};
static NJS_MDATA3 golem_motion_hands_down_b_hold_sunk_twice_tracks[21] = {
#include "shared/golem_clips/golem_motion_hands_down_b_hold_sunk_twice_tracks.inc"
};
NJS_MOTION golem_motion_hands_down_b_hold_sunk_twice = {
#include "shared/golem_clips/golem_motion_hands_down_b_hold_sunk_twice.inc"
};
static NJS_MKEY_A golem_motion_hands_down_b_rise_sunk_twice_joint_0_angles[2] =
    {
#include "shared/golem_clips/golem_motion_hands_down_b_rise_sunk_twice_joint_0_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_b_rise_sunk_twice_joint_1_angles[2] =
    {
#include "shared/golem_clips/golem_motion_hands_down_b_rise_sunk_twice_joint_1_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_b_rise_sunk_twice_joint_2_angles[2] =
    {
#include "shared/golem_clips/golem_motion_hands_down_b_rise_sunk_twice_joint_2_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_b_rise_sunk_twice_joint_3_angles[2] =
    {
#include "shared/golem_clips/golem_motion_hands_down_b_rise_sunk_twice_joint_3_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_b_rise_sunk_twice_joint_4_angles[2] =
    {
#include "shared/golem_clips/golem_motion_hands_down_b_rise_sunk_twice_joint_4_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_b_rise_sunk_twice_joint_5_angles[40] =
    {
#include "shared/golem_clips/golem_motion_hands_down_b_rise_sunk_twice_joint_5_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_b_rise_sunk_twice_joint_6_angles[40] =
    {
#include "shared/golem_clips/golem_motion_hands_down_b_rise_sunk_twice_joint_6_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_b_rise_sunk_twice_joint_7_angles[40] =
    {
#include "shared/golem_clips/golem_motion_hands_down_b_rise_sunk_twice_joint_7_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_b_rise_sunk_twice_joint_8_angles[40] =
    {
#include "shared/golem_clips/golem_motion_hands_down_b_rise_sunk_twice_joint_8_angles.inc"
};
static NJS_MKEY_A golem_motion_hands_down_b_rise_sunk_twice_joint_9_angles[40] =
    {
#include "shared/golem_clips/golem_motion_hands_down_b_rise_sunk_twice_joint_9_angles.inc"
};
static NJS_MKEY_A
    golem_motion_hands_down_b_rise_sunk_twice_joint_10_angles[40] = {
#include "shared/golem_clips/golem_motion_hands_down_b_rise_sunk_twice_joint_10_angles.inc"
};
static NJS_MKEY_A
    golem_motion_hands_down_b_rise_sunk_twice_joint_11_angles[40] = {
#include "shared/golem_clips/golem_motion_hands_down_b_rise_sunk_twice_joint_11_angles.inc"
};
static NJS_MKEY_A
    golem_motion_hands_down_b_rise_sunk_twice_joint_12_angles[40] = {
#include "shared/golem_clips/golem_motion_hands_down_b_rise_sunk_twice_joint_12_angles.inc"
};
static NJS_MKEY_A
    golem_motion_hands_down_b_rise_sunk_twice_joint_13_angles[40] = {
#include "shared/golem_clips/golem_motion_hands_down_b_rise_sunk_twice_joint_13_angles.inc"
};
static NJS_MKEY_A
    golem_motion_hands_down_b_rise_sunk_twice_joint_14_angles[40] = {
#include "shared/golem_clips/golem_motion_hands_down_b_rise_sunk_twice_joint_14_angles.inc"
};
static NJS_MKEY_A
    golem_motion_hands_down_b_rise_sunk_twice_joint_15_angles[40] = {
#include "shared/golem_clips/golem_motion_hands_down_b_rise_sunk_twice_joint_15_angles.inc"
};
static NJS_MKEY_A
    golem_motion_hands_down_b_rise_sunk_twice_joint_16_angles[40] = {
#include "shared/golem_clips/golem_motion_hands_down_b_rise_sunk_twice_joint_16_angles.inc"
};
static NJS_MKEY_A
    golem_motion_hands_down_b_rise_sunk_twice_joint_17_angles[34] = {
#include "shared/golem_clips/golem_motion_hands_down_b_rise_sunk_twice_joint_17_angles.inc"
};
static NJS_MKEY_A
    golem_motion_hands_down_b_rise_sunk_twice_joint_18_angles[40] = {
#include "shared/golem_clips/golem_motion_hands_down_b_rise_sunk_twice_joint_18_angles.inc"
};
static NJS_MKEY_A
    golem_motion_hands_down_b_rise_sunk_twice_joint_19_angles[40] = {
#include "shared/golem_clips/golem_motion_hands_down_b_rise_sunk_twice_joint_19_angles.inc"
};
static NJS_MKEY_A
    golem_motion_hands_down_b_rise_sunk_twice_joint_20_angles[12] = {
#include "shared/golem_clips/golem_motion_hands_down_b_rise_sunk_twice_joint_20_angles.inc"
};
static NJS_MDATA3 golem_motion_hands_down_b_rise_sunk_twice_tracks[21] = {
#include "shared/golem_clips/golem_motion_hands_down_b_rise_sunk_twice_tracks.inc"
};
NJS_MOTION golem_motion_hands_down_b_rise_sunk_twice = {
#include "shared/golem_clips/golem_motion_hands_down_b_rise_sunk_twice.inc"
};
static NJS_MKEY_A golem_motion_idle_joint_5_angles[9] = {
#include "shared/golem_clips/golem_motion_idle_joint_5_angles.inc"
};
static NJS_MKEY_A golem_motion_idle_joint_6_angles[9] = {
#include "shared/golem_clips/golem_motion_idle_joint_6_angles.inc"
};
static NJS_MKEY_A golem_motion_idle_joint_7_angles[8] = {
#include "shared/golem_clips/golem_motion_idle_joint_7_angles.inc"
};
static NJS_MKEY_A golem_motion_idle_joint_8_angles[9] = {
#include "shared/golem_clips/golem_motion_idle_joint_8_angles.inc"
};
static NJS_MKEY_A golem_motion_idle_joint_9_angles[9] = {
#include "shared/golem_clips/golem_motion_idle_joint_9_angles.inc"
};
static NJS_MKEY_A golem_motion_idle_joint_12_angles[9] = {
#include "shared/golem_clips/golem_motion_idle_joint_12_angles.inc"
};
static NJS_MKEY_A golem_motion_idle_joint_13_angles[9] = {
#include "shared/golem_clips/golem_motion_idle_joint_13_angles.inc"
};
static NJS_MKEY_A golem_motion_idle_joint_14_angles[9] = {
#include "shared/golem_clips/golem_motion_idle_joint_14_angles.inc"
};
static NJS_MKEY_A golem_motion_idle_joint_15_angles[9] = {
#include "shared/golem_clips/golem_motion_idle_joint_15_angles.inc"
};
static NJS_MKEY_A golem_motion_idle_joint_16_angles[9] = {
#include "shared/golem_clips/golem_motion_idle_joint_16_angles.inc"
};
static NJS_MKEY_A golem_motion_idle_joint_18_angles[9] = {
#include "shared/golem_clips/golem_motion_idle_joint_18_angles.inc"
};
static NJS_MKEY_A golem_motion_idle_joint_19_angles[9] = {
#include "shared/golem_clips/golem_motion_idle_joint_19_angles.inc"
};
static NJS_MKEY_A golem_motion_idle_joint_20_angles[9] = {
#include "shared/golem_clips/golem_motion_idle_joint_20_angles.inc"
};
static NJS_MDATA2 golem_motion_idle_tracks[21] = {
#include "shared/golem_clips/golem_motion_idle_tracks.inc"
};
NJS_MOTION golem_motion_idle = {
#include "shared/golem_clips/golem_motion_idle.inc"
};
static NJS_MKEY_A golem_motion_death_joint_0_angles[2] = {
#include "shared/golem_clips/golem_motion_death_joint_0_angles.inc"
};
static NJS_MKEY_A golem_motion_death_joint_1_angles[2] = {
#include "shared/golem_clips/golem_motion_death_joint_1_angles.inc"
};
static NJS_MKEY_A golem_motion_death_joint_2_angles[2] = {
#include "shared/golem_clips/golem_motion_death_joint_2_angles.inc"
};
static NJS_MKEY_A golem_motion_death_joint_3_angles[2] = {
#include "shared/golem_clips/golem_motion_death_joint_3_angles.inc"
};
static NJS_MKEY_A golem_motion_death_joint_4_angles[2] = {
#include "shared/golem_clips/golem_motion_death_joint_4_angles.inc"
};
static NJS_MKEY_A golem_motion_death_joint_5_angles[112] = {
#include "shared/golem_clips/golem_motion_death_joint_5_angles.inc"
};
static NJS_MKEY_A golem_motion_death_joint_6_angles[113] = {
#include "shared/golem_clips/golem_motion_death_joint_6_angles.inc"
};
static NJS_MKEY_A golem_motion_death_joint_7_angles[118] = {
#include "shared/golem_clips/golem_motion_death_joint_7_angles.inc"
};
static NJS_MKEY_A golem_motion_death_joint_8_angles[82] = {
#include "shared/golem_clips/golem_motion_death_joint_8_angles.inc"
};
static NJS_MKEY_A golem_motion_death_joint_9_angles[84] = {
#include "shared/golem_clips/golem_motion_death_joint_9_angles.inc"
};
static NJS_MKEY_A golem_motion_death_joint_10_angles[2] = {
#include "shared/golem_clips/golem_motion_death_joint_10_angles.inc"
};
static NJS_MKEY_A golem_motion_death_joint_11_angles[101] = {
#include "shared/golem_clips/golem_motion_death_joint_11_angles.inc"
};
static NJS_MKEY_A golem_motion_death_joint_12_angles[59] = {
#include "shared/golem_clips/golem_motion_death_joint_12_angles.inc"
};
static NJS_MKEY_A golem_motion_death_joint_13_angles[128] = {
#include "shared/golem_clips/golem_motion_death_joint_13_angles.inc"
};
static NJS_MKEY_A golem_motion_death_joint_14_angles[114] = {
#include "shared/golem_clips/golem_motion_death_joint_14_angles.inc"
};
static NJS_MKEY_A golem_motion_death_joint_15_angles[2] = {
#include "shared/golem_clips/golem_motion_death_joint_15_angles.inc"
};
static NJS_MKEY_A golem_motion_death_joint_16_angles[2] = {
#include "shared/golem_clips/golem_motion_death_joint_16_angles.inc"
};
static NJS_MKEY_A golem_motion_death_joint_17_angles[2] = {
#include "shared/golem_clips/golem_motion_death_joint_17_angles.inc"
};
static NJS_MKEY_A golem_motion_death_joint_18_angles[2] = {
#include "shared/golem_clips/golem_motion_death_joint_18_angles.inc"
};
static NJS_MKEY_A golem_motion_death_joint_19_angles[2] = {
#include "shared/golem_clips/golem_motion_death_joint_19_angles.inc"
};
static NJS_MKEY_A golem_motion_death_joint_20_angles[124] = {
#include "shared/golem_clips/golem_motion_death_joint_20_angles.inc"
};
static NJS_MDATA3 golem_motion_death_tracks[21] = {
#include "shared/golem_clips/golem_motion_death_tracks.inc"
};
NJS_MOTION golem_motion_death = {
#include "shared/golem_clips/golem_motion_death.inc"
};
