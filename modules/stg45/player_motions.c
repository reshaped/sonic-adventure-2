/* STG45.PRS's motions and their registry, from the disc. its D lies between
 * object_kinds.c and ocean_fence_art.c */
#include <ninja.h>

#include "player/player.h"
#include "shared/motions.h"

static NJS_MKEY_F eggman_walker_sink_motion_joint_0_position[2] = {
#include "stg45/player_motions/eggman_walker_sink_motion_joint_0_position.inc"
};
static NJS_MKEY_F eggman_walker_sink_motion_joint_1_position[2] = {
#include "stg45/player_motions/eggman_walker_sink_motion_joint_1_position.inc"
};
static NJS_MKEY_A eggman_walker_sink_motion_joint_1_angles[64] = {
#include "stg45/player_motions/eggman_walker_sink_motion_joint_1_angles.inc"
};
static NJS_MKEY_A eggman_walker_sink_motion_joint_2_angles[64] = {
#include "stg45/player_motions/eggman_walker_sink_motion_joint_2_angles.inc"
};
static NJS_MKEY_A eggman_walker_sink_motion_joint_3_angles[64] = {
#include "stg45/player_motions/eggman_walker_sink_motion_joint_3_angles.inc"
};
static NJS_MKEY_A eggman_walker_sink_motion_joint_4_angles[64] = {
#include "stg45/player_motions/eggman_walker_sink_motion_joint_4_angles.inc"
};
static NJS_MKEY_A eggman_walker_sink_motion_joint_5_angles[63] = {
#include "stg45/player_motions/eggman_walker_sink_motion_joint_5_angles.inc"
};
static NJS_MKEY_A eggman_walker_sink_motion_joint_6_angles[64] = {
#include "stg45/player_motions/eggman_walker_sink_motion_joint_6_angles.inc"
};
static NJS_MKEY_A eggman_walker_sink_motion_joint_7_angles[64] = {
#include "stg45/player_motions/eggman_walker_sink_motion_joint_7_angles.inc"
};
static NJS_MKEY_A eggman_walker_sink_motion_joint_8_angles[64] = {
#include "stg45/player_motions/eggman_walker_sink_motion_joint_8_angles.inc"
};
static NJS_MKEY_A eggman_walker_sink_motion_joint_9_angles[51] = {
#include "stg45/player_motions/eggman_walker_sink_motion_joint_9_angles.inc"
};
static NJS_MKEY_F eggman_walker_sink_motion_joint_14_position[2] = {
#include "stg45/player_motions/eggman_walker_sink_motion_joint_14_position.inc"
};
static NJS_MKEY_A eggman_walker_sink_motion_joint_14_angles[2] = {
#include "stg45/player_motions/eggman_walker_sink_motion_joint_14_angles.inc"
};
static NJS_MKEY_F eggman_walker_sink_motion_joint_15_position[2] = {
#include "stg45/player_motions/eggman_walker_sink_motion_joint_15_position.inc"
};
static NJS_MKEY_A eggman_walker_sink_motion_joint_18_angles[64] = {
#include "stg45/player_motions/eggman_walker_sink_motion_joint_18_angles.inc"
};
static NJS_MKEY_A eggman_walker_sink_motion_joint_19_angles[64] = {
#include "stg45/player_motions/eggman_walker_sink_motion_joint_19_angles.inc"
};
static NJS_MKEY_A eggman_walker_sink_motion_joint_21_angles[64] = {
#include "stg45/player_motions/eggman_walker_sink_motion_joint_21_angles.inc"
};
static NJS_MKEY_A eggman_walker_sink_motion_joint_24_angles[64] = {
#include "stg45/player_motions/eggman_walker_sink_motion_joint_24_angles.inc"
};
static NJS_MKEY_A eggman_walker_sink_motion_joint_25_angles[59] = {
#include "stg45/player_motions/eggman_walker_sink_motion_joint_25_angles.inc"
};
static NJS_MKEY_A eggman_walker_sink_motion_joint_27_angles[64] = {
#include "stg45/player_motions/eggman_walker_sink_motion_joint_27_angles.inc"
};
static NJS_MKEY_A eggman_walker_sink_motion_joint_30_angles[64] = {
#include "stg45/player_motions/eggman_walker_sink_motion_joint_30_angles.inc"
};
static NJS_MDATA2 eggman_walker_sink_motion_tracks[33] = {
#include "stg45/player_motions/eggman_walker_sink_motion_tracks.inc"
};
static NJS_MOTION eggman_walker_sink_motion = {
#include "stg45/player_motions/eggman_walker_sink_motion.inc"
};
static NJS_MKEY_F tails_walker_sink_motion_joint_1_position[2] = {
#include "stg45/player_motions/tails_walker_sink_motion_joint_1_position.inc"
};
static NJS_MKEY_A tails_walker_sink_motion_joint_1_angles[63] = {
#include "stg45/player_motions/tails_walker_sink_motion_joint_1_angles.inc"
};
static NJS_MKEY_F tails_walker_sink_motion_joint_3_position[2] = {
#include "stg45/player_motions/tails_walker_sink_motion_joint_3_position.inc"
};
static NJS_MKEY_A tails_walker_sink_motion_joint_9_angles[63] = {
#include "stg45/player_motions/tails_walker_sink_motion_joint_9_angles.inc"
};
static NJS_MKEY_A tails_walker_sink_motion_joint_10_angles[63] = {
#include "stg45/player_motions/tails_walker_sink_motion_joint_10_angles.inc"
};
static NJS_MKEY_A tails_walker_sink_motion_joint_11_angles[63] = {
#include "stg45/player_motions/tails_walker_sink_motion_joint_11_angles.inc"
};
static NJS_MKEY_A tails_walker_sink_motion_joint_15_angles[63] = {
#include "stg45/player_motions/tails_walker_sink_motion_joint_15_angles.inc"
};
static NJS_MKEY_A tails_walker_sink_motion_joint_16_angles[63] = {
#include "stg45/player_motions/tails_walker_sink_motion_joint_16_angles.inc"
};
static NJS_MKEY_A tails_walker_sink_motion_joint_17_angles[63] = {
#include "stg45/player_motions/tails_walker_sink_motion_joint_17_angles.inc"
};
static NJS_MKEY_A tails_walker_sink_motion_joint_20_angles[63] = {
#include "stg45/player_motions/tails_walker_sink_motion_joint_20_angles.inc"
};
static NJS_MKEY_A tails_walker_sink_motion_joint_27_angles[63] = {
#include "stg45/player_motions/tails_walker_sink_motion_joint_27_angles.inc"
};
static NJS_MKEY_A tails_walker_sink_motion_joint_28_angles[63] = {
#include "stg45/player_motions/tails_walker_sink_motion_joint_28_angles.inc"
};
static NJS_MKEY_A tails_walker_sink_motion_joint_29_angles[63] = {
#include "stg45/player_motions/tails_walker_sink_motion_joint_29_angles.inc"
};
static NJS_MKEY_A tails_walker_sink_motion_joint_30_angles[63] = {
#include "stg45/player_motions/tails_walker_sink_motion_joint_30_angles.inc"
};
static NJS_MKEY_A tails_walker_sink_motion_joint_31_angles[63] = {
#include "stg45/player_motions/tails_walker_sink_motion_joint_31_angles.inc"
};
static NJS_MKEY_A tails_walker_sink_motion_joint_32_angles[63] = {
#include "stg45/player_motions/tails_walker_sink_motion_joint_32_angles.inc"
};
static NJS_MDATA2 tails_walker_sink_motion_tracks[33] = {
#include "stg45/player_motions/tails_walker_sink_motion_tracks.inc"
};
static NJS_MOTION tails_walker_sink_motion = {
#include "stg45/player_motions/tails_walker_sink_motion.inc"
};
struct motion_entry module_motions[3] = {
#include "stg45/player_motions/module_motions.inc"
};
