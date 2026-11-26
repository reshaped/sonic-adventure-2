/* the motions and their registry of STG27.PRS and STG43.PRS, from the disc.
 * its D follows object_kinds.c */
#include <ninja.h>

#include "player/player.h"
#include "shared/motions.h"

static NJS_MKEY_F upgrade_sand_ocean_motion_joint_0_position[16] = {
#include "shared/player_motions_upgrade_sand_ocean/upgrade_sand_ocean_motion_joint_0_position.inc"
};
static NJS_MKEY_F upgrade_sand_ocean_motion_joint_1_position[2] = {
#include "shared/player_motions_upgrade_sand_ocean/upgrade_sand_ocean_motion_joint_1_position.inc"
};
static NJS_MKEY_A upgrade_sand_ocean_motion_joint_1_angles[2] = {
#include "shared/player_motions_upgrade_sand_ocean/upgrade_sand_ocean_motion_joint_1_angles.inc"
};
static NJS_MKEY_A upgrade_sand_ocean_motion_joint_2_angles[16] = {
#include "shared/player_motions_upgrade_sand_ocean/upgrade_sand_ocean_motion_joint_2_angles.inc"
};
static NJS_MKEY_A upgrade_sand_ocean_motion_joint_3_angles[16] = {
#include "shared/player_motions_upgrade_sand_ocean/upgrade_sand_ocean_motion_joint_3_angles.inc"
};
static NJS_MKEY_A upgrade_sand_ocean_motion_joint_4_angles[2] = {
#include "shared/player_motions_upgrade_sand_ocean/upgrade_sand_ocean_motion_joint_4_angles.inc"
};
static NJS_MKEY_A upgrade_sand_ocean_motion_joint_6_angles[16] = {
#include "shared/player_motions_upgrade_sand_ocean/upgrade_sand_ocean_motion_joint_6_angles.inc"
};
static NJS_MKEY_A upgrade_sand_ocean_motion_joint_7_angles[16] = {
#include "shared/player_motions_upgrade_sand_ocean/upgrade_sand_ocean_motion_joint_7_angles.inc"
};
static NJS_MKEY_A upgrade_sand_ocean_motion_joint_8_angles[2] = {
#include "shared/player_motions_upgrade_sand_ocean/upgrade_sand_ocean_motion_joint_8_angles.inc"
};
static NJS_MKEY_A upgrade_sand_ocean_motion_joint_10_angles[2] = {
#include "shared/player_motions_upgrade_sand_ocean/upgrade_sand_ocean_motion_joint_10_angles.inc"
};
static NJS_MKEY_A upgrade_sand_ocean_motion_joint_14_angles[2] = {
#include "shared/player_motions_upgrade_sand_ocean/upgrade_sand_ocean_motion_joint_14_angles.inc"
};
static NJS_MKEY_A upgrade_sand_ocean_motion_joint_18_angles[16] = {
#include "shared/player_motions_upgrade_sand_ocean/upgrade_sand_ocean_motion_joint_18_angles.inc"
};
static NJS_MKEY_A upgrade_sand_ocean_motion_joint_19_angles[12] = {
#include "shared/player_motions_upgrade_sand_ocean/upgrade_sand_ocean_motion_joint_19_angles.inc"
};
static NJS_MKEY_A upgrade_sand_ocean_motion_joint_21_angles[2] = {
#include "shared/player_motions_upgrade_sand_ocean/upgrade_sand_ocean_motion_joint_21_angles.inc"
};
static NJS_MKEY_A upgrade_sand_ocean_motion_joint_24_angles[16] = {
#include "shared/player_motions_upgrade_sand_ocean/upgrade_sand_ocean_motion_joint_24_angles.inc"
};
static NJS_MKEY_A upgrade_sand_ocean_motion_joint_25_angles[16] = {
#include "shared/player_motions_upgrade_sand_ocean/upgrade_sand_ocean_motion_joint_25_angles.inc"
};
static NJS_MKEY_A upgrade_sand_ocean_motion_joint_27_angles[2] = {
#include "shared/player_motions_upgrade_sand_ocean/upgrade_sand_ocean_motion_joint_27_angles.inc"
};
static NJS_MKEY_A upgrade_sand_ocean_motion_joint_30_angles[16] = {
#include "shared/player_motions_upgrade_sand_ocean/upgrade_sand_ocean_motion_joint_30_angles.inc"
};
static NJS_MDATA2 upgrade_sand_ocean_motion_tracks[33] = {
#include "shared/player_motions_upgrade_sand_ocean/upgrade_sand_ocean_motion_tracks.inc"
};
static NJS_MOTION upgrade_sand_ocean_motion = {
#include "shared/player_motions_upgrade_sand_ocean/upgrade_sand_ocean_motion.inc"
};
struct motion_entry module_motions[2] = {
#include "shared/player_motions_upgrade_sand_ocean/module_motions.inc"
};
