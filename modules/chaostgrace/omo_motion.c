/* CHAOSTGRACE.PRS's motions, from the disc. its D lies between crab.c and
 * omochao.c */
#include <ninja.h>

#include "chaostgrace/omo_motion.h"

static NJS_MKEY_F omo_idle_joint_0_position[16] = {
#include "chaostgrace/omo_motion/omo_idle_joint_0_position.inc"
};
static NJS_MKEY_A omo_idle_joint_0_angles[16] = {
#include "chaostgrace/omo_motion/omo_idle_joint_0_angles.inc"
};
static NJS_MKEY_A omo_idle_joint_1_angles[16] = {
#include "chaostgrace/omo_motion/omo_idle_joint_1_angles.inc"
};
static NJS_MKEY_A omo_idle_joint_2_angles[16] = {
#include "chaostgrace/omo_motion/omo_idle_joint_2_angles.inc"
};
static NJS_MKEY_A omo_idle_joint_3_angles[16] = {
#include "chaostgrace/omo_motion/omo_idle_joint_3_angles.inc"
};
static NJS_MKEY_A omo_idle_joint_4_angles[16] = {
#include "chaostgrace/omo_motion/omo_idle_joint_4_angles.inc"
};
static NJS_MKEY_A omo_idle_joint_5_angles[16] = {
#include "chaostgrace/omo_motion/omo_idle_joint_5_angles.inc"
};
static NJS_MKEY_A omo_idle_joint_6_angles[16] = {
#include "chaostgrace/omo_motion/omo_idle_joint_6_angles.inc"
};
static NJS_MKEY_A omo_idle_joint_7_angles[16] = {
#include "chaostgrace/omo_motion/omo_idle_joint_7_angles.inc"
};
static NJS_MKEY_A omo_idle_joint_8_angles[16] = {
#include "chaostgrace/omo_motion/omo_idle_joint_8_angles.inc"
};
static NJS_MDATA2 omo_idle_tracks[9] = {
#include "chaostgrace/omo_motion/omo_idle_tracks.inc"
};
NJS_MOTION omo_idle = {
#include "chaostgrace/omo_motion/omo_idle.inc"
};
static NJS_MKEY_F omo_right_joint_0_position[8] = {
#include "chaostgrace/omo_motion/omo_right_joint_0_position.inc"
};
static NJS_MKEY_A omo_right_joint_0_angles[8] = {
#include "chaostgrace/omo_motion/omo_right_joint_0_angles.inc"
};
static NJS_MKEY_A omo_right_joint_1_angles[8] = {
#include "chaostgrace/omo_motion/omo_right_joint_1_angles.inc"
};
static NJS_MKEY_A omo_right_joint_2_angles[8] = {
#include "chaostgrace/omo_motion/omo_right_joint_2_angles.inc"
};
static NJS_MKEY_A omo_right_joint_4_angles[8] = {
#include "chaostgrace/omo_motion/omo_right_joint_4_angles.inc"
};
static NJS_MKEY_A omo_right_joint_5_angles[8] = {
#include "chaostgrace/omo_motion/omo_right_joint_5_angles.inc"
};
static NJS_MKEY_A omo_right_joint_7_angles[8] = {
#include "chaostgrace/omo_motion/omo_right_joint_7_angles.inc"
};
static NJS_MKEY_A omo_right_joint_8_angles[8] = {
#include "chaostgrace/omo_motion/omo_right_joint_8_angles.inc"
};
static NJS_MDATA2 omo_right_tracks[9] = {
#include "chaostgrace/omo_motion/omo_right_tracks.inc"
};
NJS_MOTION omo_right = {
#include "chaostgrace/omo_motion/omo_right.inc"
};
static NJS_MKEY_F omo_left_joint_0_position[8] = {
#include "chaostgrace/omo_motion/omo_left_joint_0_position.inc"
};
static NJS_MKEY_A omo_left_joint_0_angles[8] = {
#include "chaostgrace/omo_motion/omo_left_joint_0_angles.inc"
};
static NJS_MKEY_A omo_left_joint_1_angles[8] = {
#include "chaostgrace/omo_motion/omo_left_joint_1_angles.inc"
};
static NJS_MKEY_A omo_left_joint_2_angles[8] = {
#include "chaostgrace/omo_motion/omo_left_joint_2_angles.inc"
};
static NJS_MKEY_A omo_left_joint_4_angles[8] = {
#include "chaostgrace/omo_motion/omo_left_joint_4_angles.inc"
};
static NJS_MKEY_A omo_left_joint_5_angles[8] = {
#include "chaostgrace/omo_motion/omo_left_joint_5_angles.inc"
};
static NJS_MKEY_A omo_left_joint_6_angles[8] = {
#include "chaostgrace/omo_motion/omo_left_joint_6_angles.inc"
};
static NJS_MKEY_A omo_left_joint_7_angles[8] = {
#include "chaostgrace/omo_motion/omo_left_joint_7_angles.inc"
};
static NJS_MDATA2 omo_left_tracks[9] = {
#include "chaostgrace/omo_motion/omo_left_tracks.inc"
};
NJS_MOTION omo_left = {
#include "chaostgrace/omo_motion/omo_left.inc"
};
static NJS_MKEY_F omo_starter_idle_joint_0_position[8] = {
#include "chaostgrace/omo_motion/omo_starter_idle_joint_0_position.inc"
};
static NJS_MKEY_A omo_starter_idle_joint_1_angles[8] = {
#include "chaostgrace/omo_motion/omo_starter_idle_joint_1_angles.inc"
};
static NJS_MKEY_A omo_starter_idle_joint_2_angles[8] = {
#include "chaostgrace/omo_motion/omo_starter_idle_joint_2_angles.inc"
};
static NJS_MKEY_A omo_starter_idle_joint_3_angles[8] = {
#include "chaostgrace/omo_motion/omo_starter_idle_joint_3_angles.inc"
};
static NJS_MKEY_A omo_starter_idle_joint_4_angles[8] = {
#include "chaostgrace/omo_motion/omo_starter_idle_joint_4_angles.inc"
};
static NJS_MKEY_A omo_starter_idle_joint_5_angles[8] = {
#include "chaostgrace/omo_motion/omo_starter_idle_joint_5_angles.inc"
};
static NJS_MKEY_A omo_starter_idle_joint_7_angles[8] = {
#include "chaostgrace/omo_motion/omo_starter_idle_joint_7_angles.inc"
};
static NJS_MDATA2 omo_starter_idle_tracks[9] = {
#include "chaostgrace/omo_motion/omo_starter_idle_tracks.inc"
};
NJS_MOTION omo_starter_idle = {
#include "chaostgrace/omo_motion/omo_starter_idle.inc"
};
static NJS_MKEY_F omo_start_joint_0_position[17] = {
#include "chaostgrace/omo_motion/omo_start_joint_0_position.inc"
};
static NJS_MKEY_A omo_start_joint_0_angles[17] = {
#include "chaostgrace/omo_motion/omo_start_joint_0_angles.inc"
};
static NJS_MKEY_A omo_start_joint_1_angles[17] = {
#include "chaostgrace/omo_motion/omo_start_joint_1_angles.inc"
};
static NJS_MKEY_A omo_start_joint_2_angles[17] = {
#include "chaostgrace/omo_motion/omo_start_joint_2_angles.inc"
};
static NJS_MKEY_A omo_start_joint_3_angles[17] = {
#include "chaostgrace/omo_motion/omo_start_joint_3_angles.inc"
};
static NJS_MKEY_A omo_start_joint_4_angles[17] = {
#include "chaostgrace/omo_motion/omo_start_joint_4_angles.inc"
};
static NJS_MKEY_A omo_start_joint_5_angles[17] = {
#include "chaostgrace/omo_motion/omo_start_joint_5_angles.inc"
};
static NJS_MKEY_A omo_start_joint_6_angles[17] = {
#include "chaostgrace/omo_motion/omo_start_joint_6_angles.inc"
};
static NJS_MKEY_A omo_start_joint_7_angles[12] = {
#include "chaostgrace/omo_motion/omo_start_joint_7_angles.inc"
};
static NJS_MDATA2 omo_start_tracks[9] = {
#include "chaostgrace/omo_motion/omo_start_tracks.inc"
};
NJS_MOTION omo_start = {
#include "chaostgrace/omo_motion/omo_start.inc"
};
static NJS_MKEY_F omo_cheer_joint_0_position[7] = {
#include "chaostgrace/omo_motion/omo_cheer_joint_0_position.inc"
};
static NJS_MKEY_A omo_cheer_joint_1_angles[7] = {
#include "chaostgrace/omo_motion/omo_cheer_joint_1_angles.inc"
};
static NJS_MKEY_A omo_cheer_joint_2_angles[7] = {
#include "chaostgrace/omo_motion/omo_cheer_joint_2_angles.inc"
};
static NJS_MKEY_A omo_cheer_joint_3_angles[7] = {
#include "chaostgrace/omo_motion/omo_cheer_joint_3_angles.inc"
};
static NJS_MKEY_A omo_cheer_joint_4_angles[7] = {
#include "chaostgrace/omo_motion/omo_cheer_joint_4_angles.inc"
};
static NJS_MKEY_A omo_cheer_joint_5_angles[7] = {
#include "chaostgrace/omo_motion/omo_cheer_joint_5_angles.inc"
};
static NJS_MKEY_A omo_cheer_joint_6_angles[7] = {
#include "chaostgrace/omo_motion/omo_cheer_joint_6_angles.inc"
};
static NJS_MKEY_A omo_cheer_joint_7_angles[7] = {
#include "chaostgrace/omo_motion/omo_cheer_joint_7_angles.inc"
};
static NJS_MKEY_A omo_cheer_joint_8_angles[7] = {
#include "chaostgrace/omo_motion/omo_cheer_joint_8_angles.inc"
};
static NJS_MDATA2 omo_cheer_tracks[9] = {
#include "chaostgrace/omo_motion/omo_cheer_tracks.inc"
};
NJS_MOTION omo_cheer = {
#include "chaostgrace/omo_motion/omo_cheer.inc"
};
