/* STG21.PRS's motions and their registry, from the disc. its D lies between
 * explosion.c and global_light_switch.c */
#include <ninja.h>

#include "player/player.h"
#include "shared/motions.h"

static NJS_MKEY_F upgrade_sand_ocean_motion_joint_0_position[16] = {
#include "stg21/player_motions/upgrade_sand_ocean_motion_joint_0_position.inc"
};
static NJS_MKEY_F upgrade_sand_ocean_motion_joint_1_position[2] = {
#include "stg21/player_motions/upgrade_sand_ocean_motion_joint_1_position.inc"
};
static NJS_MKEY_A upgrade_sand_ocean_motion_joint_1_angles[2] = {
#include "stg21/player_motions/upgrade_sand_ocean_motion_joint_1_angles.inc"
};
static NJS_MKEY_A upgrade_sand_ocean_motion_joint_2_angles[16] = {
#include "stg21/player_motions/upgrade_sand_ocean_motion_joint_2_angles.inc"
};
static NJS_MKEY_A upgrade_sand_ocean_motion_joint_3_angles[16] = {
#include "stg21/player_motions/upgrade_sand_ocean_motion_joint_3_angles.inc"
};
static NJS_MKEY_A upgrade_sand_ocean_motion_joint_4_angles[2] = {
#include "stg21/player_motions/upgrade_sand_ocean_motion_joint_4_angles.inc"
};
static NJS_MKEY_A upgrade_sand_ocean_motion_joint_6_angles[16] = {
#include "stg21/player_motions/upgrade_sand_ocean_motion_joint_6_angles.inc"
};
static NJS_MKEY_A upgrade_sand_ocean_motion_joint_7_angles[16] = {
#include "stg21/player_motions/upgrade_sand_ocean_motion_joint_7_angles.inc"
};
static NJS_MKEY_A upgrade_sand_ocean_motion_joint_8_angles[2] = {
#include "stg21/player_motions/upgrade_sand_ocean_motion_joint_8_angles.inc"
};
static NJS_MKEY_A upgrade_sand_ocean_motion_joint_10_angles[2] = {
#include "stg21/player_motions/upgrade_sand_ocean_motion_joint_10_angles.inc"
};
static NJS_MKEY_A upgrade_sand_ocean_motion_joint_14_angles[2] = {
#include "stg21/player_motions/upgrade_sand_ocean_motion_joint_14_angles.inc"
};
static NJS_MKEY_A upgrade_sand_ocean_motion_joint_18_angles[16] = {
#include "stg21/player_motions/upgrade_sand_ocean_motion_joint_18_angles.inc"
};
static NJS_MKEY_A upgrade_sand_ocean_motion_joint_19_angles[12] = {
#include "stg21/player_motions/upgrade_sand_ocean_motion_joint_19_angles.inc"
};
static NJS_MKEY_A upgrade_sand_ocean_motion_joint_21_angles[2] = {
#include "stg21/player_motions/upgrade_sand_ocean_motion_joint_21_angles.inc"
};
static NJS_MKEY_A upgrade_sand_ocean_motion_joint_24_angles[16] = {
#include "stg21/player_motions/upgrade_sand_ocean_motion_joint_24_angles.inc"
};
static NJS_MKEY_A upgrade_sand_ocean_motion_joint_25_angles[16] = {
#include "stg21/player_motions/upgrade_sand_ocean_motion_joint_25_angles.inc"
};
static NJS_MKEY_A upgrade_sand_ocean_motion_joint_27_angles[2] = {
#include "stg21/player_motions/upgrade_sand_ocean_motion_joint_27_angles.inc"
};
static NJS_MKEY_A upgrade_sand_ocean_motion_joint_30_angles[16] = {
#include "stg21/player_motions/upgrade_sand_ocean_motion_joint_30_angles.inc"
};
static NJS_MDATA2 upgrade_sand_ocean_motion_tracks[33] = {
#include "stg21/player_motions/upgrade_sand_ocean_motion_tracks.inc"
};
static NJS_MOTION upgrade_sand_ocean_motion = {
#include "stg21/player_motions/upgrade_sand_ocean_motion.inc"
};
static NJS_MKEY_F eggman_walker_sink_motion_joint_0_position[2] = {
#include "stg21/player_motions/eggman_walker_sink_motion_joint_0_position.inc"
};
static NJS_MKEY_F eggman_walker_sink_motion_joint_1_position[2] = {
#include "stg21/player_motions/eggman_walker_sink_motion_joint_1_position.inc"
};
static NJS_MKEY_A eggman_walker_sink_motion_joint_1_angles[64] = {
#include "stg21/player_motions/eggman_walker_sink_motion_joint_1_angles.inc"
};
static NJS_MKEY_A eggman_walker_sink_motion_joint_2_angles[64] = {
#include "stg21/player_motions/eggman_walker_sink_motion_joint_2_angles.inc"
};
static NJS_MKEY_A eggman_walker_sink_motion_joint_3_angles[64] = {
#include "stg21/player_motions/eggman_walker_sink_motion_joint_3_angles.inc"
};
static NJS_MKEY_A eggman_walker_sink_motion_joint_4_angles[64] = {
#include "stg21/player_motions/eggman_walker_sink_motion_joint_4_angles.inc"
};
static NJS_MKEY_A eggman_walker_sink_motion_joint_5_angles[63] = {
#include "stg21/player_motions/eggman_walker_sink_motion_joint_5_angles.inc"
};
static NJS_MKEY_A eggman_walker_sink_motion_joint_6_angles[64] = {
#include "stg21/player_motions/eggman_walker_sink_motion_joint_6_angles.inc"
};
static NJS_MKEY_A eggman_walker_sink_motion_joint_7_angles[64] = {
#include "stg21/player_motions/eggman_walker_sink_motion_joint_7_angles.inc"
};
static NJS_MKEY_A eggman_walker_sink_motion_joint_8_angles[64] = {
#include "stg21/player_motions/eggman_walker_sink_motion_joint_8_angles.inc"
};
static NJS_MKEY_A eggman_walker_sink_motion_joint_9_angles[51] = {
#include "stg21/player_motions/eggman_walker_sink_motion_joint_9_angles.inc"
};
static NJS_MKEY_F eggman_walker_sink_motion_joint_14_position[2] = {
#include "stg21/player_motions/eggman_walker_sink_motion_joint_14_position.inc"
};
static NJS_MKEY_A eggman_walker_sink_motion_joint_14_angles[2] = {
#include "stg21/player_motions/eggman_walker_sink_motion_joint_14_angles.inc"
};
static NJS_MKEY_F eggman_walker_sink_motion_joint_15_position[2] = {
#include "stg21/player_motions/eggman_walker_sink_motion_joint_15_position.inc"
};
static NJS_MKEY_A eggman_walker_sink_motion_joint_18_angles[64] = {
#include "stg21/player_motions/eggman_walker_sink_motion_joint_18_angles.inc"
};
static NJS_MKEY_A eggman_walker_sink_motion_joint_19_angles[64] = {
#include "stg21/player_motions/eggman_walker_sink_motion_joint_19_angles.inc"
};
static NJS_MKEY_A eggman_walker_sink_motion_joint_21_angles[64] = {
#include "stg21/player_motions/eggman_walker_sink_motion_joint_21_angles.inc"
};
static NJS_MKEY_A eggman_walker_sink_motion_joint_24_angles[64] = {
#include "stg21/player_motions/eggman_walker_sink_motion_joint_24_angles.inc"
};
static NJS_MKEY_A eggman_walker_sink_motion_joint_25_angles[59] = {
#include "stg21/player_motions/eggman_walker_sink_motion_joint_25_angles.inc"
};
static NJS_MKEY_A eggman_walker_sink_motion_joint_27_angles[64] = {
#include "stg21/player_motions/eggman_walker_sink_motion_joint_27_angles.inc"
};
static NJS_MKEY_A eggman_walker_sink_motion_joint_30_angles[64] = {
#include "stg21/player_motions/eggman_walker_sink_motion_joint_30_angles.inc"
};
static NJS_MDATA2 eggman_walker_sink_motion_tracks[33] = {
#include "stg21/player_motions/eggman_walker_sink_motion_tracks.inc"
};
static NJS_MOTION eggman_walker_sink_motion = {
#include "stg21/player_motions/eggman_walker_sink_motion.inc"
};
struct motion_entry module_motions[3] = {
#include "stg21/player_motions/module_motions.inc"
};
