/* the motions and their registry of STG17.PRS and STG24.PRS, from the disc.
 * its D precedes dash_panel_art.c */
#include <ninja.h>

#include "player/player.h"
#include "shared/motions.h"

static NJS_MKEY_F upgrade_mission_street_motion_joint_1_position[16] = {
#include "shared/player_motions_upgrade_mission_street/upgrade_mission_street_motion_joint_1_position.inc"
};
static NJS_MKEY_A upgrade_mission_street_motion_joint_1_angles[2] = {
#include "shared/player_motions_upgrade_mission_street/upgrade_mission_street_motion_joint_1_angles.inc"
};
static NJS_MKEY_F upgrade_mission_street_motion_joint_2_position[2] = {
#include "shared/player_motions_upgrade_mission_street/upgrade_mission_street_motion_joint_2_position.inc"
};
static NJS_MKEY_A upgrade_mission_street_motion_joint_2_angles[2] = {
#include "shared/player_motions_upgrade_mission_street/upgrade_mission_street_motion_joint_2_angles.inc"
};
static NJS_MKEY_F upgrade_mission_street_motion_joint_3_position[2] = {
#include "shared/player_motions_upgrade_mission_street/upgrade_mission_street_motion_joint_3_position.inc"
};
static NJS_MKEY_F upgrade_mission_street_motion_joint_5_position[2] = {
#include "shared/player_motions_upgrade_mission_street/upgrade_mission_street_motion_joint_5_position.inc"
};
static NJS_MKEY_A upgrade_mission_street_motion_joint_5_angles[2] = {
#include "shared/player_motions_upgrade_mission_street/upgrade_mission_street_motion_joint_5_angles.inc"
};
static NJS_MKEY_A upgrade_mission_street_motion_joint_9_angles[16] = {
#include "shared/player_motions_upgrade_mission_street/upgrade_mission_street_motion_joint_9_angles.inc"
};
static NJS_MKEY_A upgrade_mission_street_motion_joint_10_angles[16] = {
#include "shared/player_motions_upgrade_mission_street/upgrade_mission_street_motion_joint_10_angles.inc"
};
static NJS_MKEY_A upgrade_mission_street_motion_joint_11_angles[2] = {
#include "shared/player_motions_upgrade_mission_street/upgrade_mission_street_motion_joint_11_angles.inc"
};
static NJS_MKEY_A upgrade_mission_street_motion_joint_12_angles[2] = {
#include "shared/player_motions_upgrade_mission_street/upgrade_mission_street_motion_joint_12_angles.inc"
};
static NJS_MKEY_A upgrade_mission_street_motion_joint_15_angles[12] = {
#include "shared/player_motions_upgrade_mission_street/upgrade_mission_street_motion_joint_15_angles.inc"
};
static NJS_MKEY_A upgrade_mission_street_motion_joint_16_angles[16] = {
#include "shared/player_motions_upgrade_mission_street/upgrade_mission_street_motion_joint_16_angles.inc"
};
static NJS_MKEY_A upgrade_mission_street_motion_joint_17_angles[2] = {
#include "shared/player_motions_upgrade_mission_street/upgrade_mission_street_motion_joint_17_angles.inc"
};
static NJS_MKEY_A upgrade_mission_street_motion_joint_18_angles[2] = {
#include "shared/player_motions_upgrade_mission_street/upgrade_mission_street_motion_joint_18_angles.inc"
};
static NJS_MKEY_A upgrade_mission_street_motion_joint_20_angles[16] = {
#include "shared/player_motions_upgrade_mission_street/upgrade_mission_street_motion_joint_20_angles.inc"
};
static NJS_MKEY_A upgrade_mission_street_motion_joint_22_angles[2] = {
#include "shared/player_motions_upgrade_mission_street/upgrade_mission_street_motion_joint_22_angles.inc"
};
static NJS_MKEY_A upgrade_mission_street_motion_joint_23_angles[2] = {
#include "shared/player_motions_upgrade_mission_street/upgrade_mission_street_motion_joint_23_angles.inc"
};
static NJS_MKEY_A upgrade_mission_street_motion_joint_27_angles[16] = {
#include "shared/player_motions_upgrade_mission_street/upgrade_mission_street_motion_joint_27_angles.inc"
};
static NJS_MKEY_A upgrade_mission_street_motion_joint_28_angles[16] = {
#include "shared/player_motions_upgrade_mission_street/upgrade_mission_street_motion_joint_28_angles.inc"
};
static NJS_MKEY_F upgrade_mission_street_motion_joint_29_position[2] = {
#include "shared/player_motions_upgrade_mission_street/upgrade_mission_street_motion_joint_29_position.inc"
};
static NJS_MKEY_A upgrade_mission_street_motion_joint_29_angles[2] = {
#include "shared/player_motions_upgrade_mission_street/upgrade_mission_street_motion_joint_29_angles.inc"
};
static NJS_MKEY_A upgrade_mission_street_motion_joint_30_angles[16] = {
#include "shared/player_motions_upgrade_mission_street/upgrade_mission_street_motion_joint_30_angles.inc"
};
static NJS_MKEY_A upgrade_mission_street_motion_joint_31_angles[16] = {
#include "shared/player_motions_upgrade_mission_street/upgrade_mission_street_motion_joint_31_angles.inc"
};
static NJS_MKEY_F upgrade_mission_street_motion_joint_32_position[2] = {
#include "shared/player_motions_upgrade_mission_street/upgrade_mission_street_motion_joint_32_position.inc"
};
static NJS_MKEY_A upgrade_mission_street_motion_joint_32_angles[2] = {
#include "shared/player_motions_upgrade_mission_street/upgrade_mission_street_motion_joint_32_angles.inc"
};
static NJS_MDATA2 upgrade_mission_street_motion_tracks[33] = {
#include "shared/player_motions_upgrade_mission_street/upgrade_mission_street_motion_tracks.inc"
};
static NJS_MOTION upgrade_mission_street_motion = {
#include "shared/player_motions_upgrade_mission_street/upgrade_mission_street_motion.inc"
};
struct motion_entry module_motions[2] = {
#include "shared/player_motions_upgrade_mission_street/module_motions.inc"
};
