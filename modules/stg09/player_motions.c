/* STG09.PRS's motions and their registry, from the disc. its D lies between
 * sound_point.c and dash_panel_art.c */
#include <ninja.h>

#include "player/player.h"
#include "shared/motions.h"

static NJS_MKEY_F upgrade_prison_lane_motion_joint_1_position[16] = {
#include "stg09/player_motions/upgrade_prison_lane_motion_joint_1_position.inc"
};
static NJS_MKEY_A upgrade_prison_lane_motion_joint_1_angles[2] = {
#include "stg09/player_motions/upgrade_prison_lane_motion_joint_1_angles.inc"
};
static NJS_MKEY_F upgrade_prison_lane_motion_joint_2_position[2] = {
#include "stg09/player_motions/upgrade_prison_lane_motion_joint_2_position.inc"
};
static NJS_MKEY_A upgrade_prison_lane_motion_joint_2_angles[2] = {
#include "stg09/player_motions/upgrade_prison_lane_motion_joint_2_angles.inc"
};
static NJS_MKEY_F upgrade_prison_lane_motion_joint_3_position[2] = {
#include "stg09/player_motions/upgrade_prison_lane_motion_joint_3_position.inc"
};
static NJS_MKEY_A upgrade_prison_lane_motion_joint_9_angles[2] = {
#include "stg09/player_motions/upgrade_prison_lane_motion_joint_9_angles.inc"
};
static NJS_MKEY_A upgrade_prison_lane_motion_joint_10_angles[2] = {
#include "stg09/player_motions/upgrade_prison_lane_motion_joint_10_angles.inc"
};
static NJS_MKEY_A upgrade_prison_lane_motion_joint_11_angles[2] = {
#include "stg09/player_motions/upgrade_prison_lane_motion_joint_11_angles.inc"
};
static NJS_MKEY_A upgrade_prison_lane_motion_joint_15_angles[2] = {
#include "stg09/player_motions/upgrade_prison_lane_motion_joint_15_angles.inc"
};
static NJS_MKEY_A upgrade_prison_lane_motion_joint_16_angles[2] = {
#include "stg09/player_motions/upgrade_prison_lane_motion_joint_16_angles.inc"
};
static NJS_MKEY_A upgrade_prison_lane_motion_joint_17_angles[2] = {
#include "stg09/player_motions/upgrade_prison_lane_motion_joint_17_angles.inc"
};
static NJS_MKEY_A upgrade_prison_lane_motion_joint_20_angles[16] = {
#include "stg09/player_motions/upgrade_prison_lane_motion_joint_20_angles.inc"
};
static NJS_MKEY_A upgrade_prison_lane_motion_joint_22_angles[2] = {
#include "stg09/player_motions/upgrade_prison_lane_motion_joint_22_angles.inc"
};
static NJS_MKEY_A upgrade_prison_lane_motion_joint_23_angles[2] = {
#include "stg09/player_motions/upgrade_prison_lane_motion_joint_23_angles.inc"
};
static NJS_MKEY_A upgrade_prison_lane_motion_joint_27_angles[16] = {
#include "stg09/player_motions/upgrade_prison_lane_motion_joint_27_angles.inc"
};
static NJS_MKEY_A upgrade_prison_lane_motion_joint_28_angles[16] = {
#include "stg09/player_motions/upgrade_prison_lane_motion_joint_28_angles.inc"
};
static NJS_MKEY_F upgrade_prison_lane_motion_joint_29_position[2] = {
#include "stg09/player_motions/upgrade_prison_lane_motion_joint_29_position.inc"
};
static NJS_MKEY_A upgrade_prison_lane_motion_joint_29_angles[2] = {
#include "stg09/player_motions/upgrade_prison_lane_motion_joint_29_angles.inc"
};
static NJS_MKEY_A upgrade_prison_lane_motion_joint_30_angles[16] = {
#include "stg09/player_motions/upgrade_prison_lane_motion_joint_30_angles.inc"
};
static NJS_MKEY_A upgrade_prison_lane_motion_joint_31_angles[16] = {
#include "stg09/player_motions/upgrade_prison_lane_motion_joint_31_angles.inc"
};
static NJS_MKEY_F upgrade_prison_lane_motion_joint_32_position[2] = {
#include "stg09/player_motions/upgrade_prison_lane_motion_joint_32_position.inc"
};
static NJS_MKEY_A upgrade_prison_lane_motion_joint_32_angles[2] = {
#include "stg09/player_motions/upgrade_prison_lane_motion_joint_32_angles.inc"
};
static NJS_MDATA2 upgrade_prison_lane_motion_tracks[33] = {
#include "stg09/player_motions/upgrade_prison_lane_motion_tracks.inc"
};
static NJS_MOTION upgrade_prison_lane_motion = {
#include "stg09/player_motions/upgrade_prison_lane_motion.inc"
};
struct motion_entry module_motions[2] = {
#include "stg09/player_motions/module_motions.inc"
};
