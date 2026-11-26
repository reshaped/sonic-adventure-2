/* STG25.PRS's motions and their registry, from the disc. its D lies between
 * object_kinds.c and wood_box_e.c */
#include <ninja.h>

#include "player/player.h"
#include "shared/motions.h"

static NJS_MKEY_F upgrade_pumpkin_hill_motion_joint_2_position[2] = {
#include "stg25/player_motions/upgrade_pumpkin_hill_motion_joint_2_position.inc"
};
static NJS_MKEY_A upgrade_pumpkin_hill_motion_joint_4_angles[16] = {
#include "stg25/player_motions/upgrade_pumpkin_hill_motion_joint_4_angles.inc"
};
static NJS_MKEY_A upgrade_pumpkin_hill_motion_joint_7_angles[16] = {
#include "stg25/player_motions/upgrade_pumpkin_hill_motion_joint_7_angles.inc"
};
static NJS_MKEY_A upgrade_pumpkin_hill_motion_joint_8_angles[16] = {
#include "stg25/player_motions/upgrade_pumpkin_hill_motion_joint_8_angles.inc"
};
static NJS_MKEY_A upgrade_pumpkin_hill_motion_joint_9_angles[16] = {
#include "stg25/player_motions/upgrade_pumpkin_hill_motion_joint_9_angles.inc"
};
static NJS_MKEY_A upgrade_pumpkin_hill_motion_joint_13_angles[16] = {
#include "stg25/player_motions/upgrade_pumpkin_hill_motion_joint_13_angles.inc"
};
static NJS_MKEY_A upgrade_pumpkin_hill_motion_joint_17_angles[16] = {
#include "stg25/player_motions/upgrade_pumpkin_hill_motion_joint_17_angles.inc"
};
static NJS_MKEY_A upgrade_pumpkin_hill_motion_joint_18_angles[16] = {
#include "stg25/player_motions/upgrade_pumpkin_hill_motion_joint_18_angles.inc"
};
static NJS_MKEY_A upgrade_pumpkin_hill_motion_joint_19_angles[16] = {
#include "stg25/player_motions/upgrade_pumpkin_hill_motion_joint_19_angles.inc"
};
static NJS_MKEY_A upgrade_pumpkin_hill_motion_joint_23_angles[16] = {
#include "stg25/player_motions/upgrade_pumpkin_hill_motion_joint_23_angles.inc"
};
static NJS_MKEY_A upgrade_pumpkin_hill_motion_joint_24_angles[10] = {
#include "stg25/player_motions/upgrade_pumpkin_hill_motion_joint_24_angles.inc"
};
static NJS_MKEY_A upgrade_pumpkin_hill_motion_joint_27_angles[16] = {
#include "stg25/player_motions/upgrade_pumpkin_hill_motion_joint_27_angles.inc"
};
static NJS_MKEY_A upgrade_pumpkin_hill_motion_joint_28_angles[16] = {
#include "stg25/player_motions/upgrade_pumpkin_hill_motion_joint_28_angles.inc"
};
static NJS_MKEY_A upgrade_pumpkin_hill_motion_joint_29_angles[16] = {
#include "stg25/player_motions/upgrade_pumpkin_hill_motion_joint_29_angles.inc"
};
static NJS_MKEY_A upgrade_pumpkin_hill_motion_joint_32_angles[16] = {
#include "stg25/player_motions/upgrade_pumpkin_hill_motion_joint_32_angles.inc"
};
static NJS_MKEY_A upgrade_pumpkin_hill_motion_joint_33_angles[13] = {
#include "stg25/player_motions/upgrade_pumpkin_hill_motion_joint_33_angles.inc"
};
static NJS_MKEY_A upgrade_pumpkin_hill_motion_joint_37_angles[16] = {
#include "stg25/player_motions/upgrade_pumpkin_hill_motion_joint_37_angles.inc"
};
static NJS_MKEY_A upgrade_pumpkin_hill_motion_joint_38_angles[16] = {
#include "stg25/player_motions/upgrade_pumpkin_hill_motion_joint_38_angles.inc"
};
static NJS_MKEY_A upgrade_pumpkin_hill_motion_joint_39_angles[16] = {
#include "stg25/player_motions/upgrade_pumpkin_hill_motion_joint_39_angles.inc"
};
static NJS_MKEY_A upgrade_pumpkin_hill_motion_joint_41_angles[16] = {
#include "stg25/player_motions/upgrade_pumpkin_hill_motion_joint_41_angles.inc"
};
static NJS_MKEY_A upgrade_pumpkin_hill_motion_joint_44_angles[16] = {
#include "stg25/player_motions/upgrade_pumpkin_hill_motion_joint_44_angles.inc"
};
static NJS_MKEY_A upgrade_pumpkin_hill_motion_joint_51_angles[16] = {
#include "stg25/player_motions/upgrade_pumpkin_hill_motion_joint_51_angles.inc"
};
static NJS_MKEY_A upgrade_pumpkin_hill_motion_joint_53_angles[16] = {
#include "stg25/player_motions/upgrade_pumpkin_hill_motion_joint_53_angles.inc"
};
static NJS_MKEY_A upgrade_pumpkin_hill_motion_joint_56_angles[16] = {
#include "stg25/player_motions/upgrade_pumpkin_hill_motion_joint_56_angles.inc"
};
static NJS_MKEY_A upgrade_pumpkin_hill_motion_joint_57_angles[15] = {
#include "stg25/player_motions/upgrade_pumpkin_hill_motion_joint_57_angles.inc"
};
static NJS_MKEY_A upgrade_pumpkin_hill_motion_joint_58_angles[15] = {
#include "stg25/player_motions/upgrade_pumpkin_hill_motion_joint_58_angles.inc"
};
static NJS_MKEY_A upgrade_pumpkin_hill_motion_joint_60_angles[16] = {
#include "stg25/player_motions/upgrade_pumpkin_hill_motion_joint_60_angles.inc"
};
static NJS_MDATA2 upgrade_pumpkin_hill_motion_tracks[62] = {
#include "stg25/player_motions/upgrade_pumpkin_hill_motion_tracks.inc"
};
static NJS_MOTION upgrade_pumpkin_hill_motion = {
#include "stg25/player_motions/upgrade_pumpkin_hill_motion.inc"
};
static NJS_MKEY_F knuckles_float_motion_joint_2_position[9] = {
#include "stg25/player_motions/knuckles_float_motion_joint_2_position.inc"
};
static NJS_MKEY_A knuckles_float_motion_joint_4_angles[9] = {
#include "stg25/player_motions/knuckles_float_motion_joint_4_angles.inc"
};
static NJS_MKEY_A knuckles_float_motion_joint_7_angles[9] = {
#include "stg25/player_motions/knuckles_float_motion_joint_7_angles.inc"
};
static NJS_MKEY_A knuckles_float_motion_joint_8_angles[9] = {
#include "stg25/player_motions/knuckles_float_motion_joint_8_angles.inc"
};
static NJS_MKEY_A knuckles_float_motion_joint_9_angles[9] = {
#include "stg25/player_motions/knuckles_float_motion_joint_9_angles.inc"
};
static NJS_MKEY_A knuckles_float_motion_joint_13_angles[9] = {
#include "stg25/player_motions/knuckles_float_motion_joint_13_angles.inc"
};
static NJS_MKEY_A knuckles_float_motion_joint_14_angles[9] = {
#include "stg25/player_motions/knuckles_float_motion_joint_14_angles.inc"
};
static NJS_MKEY_A knuckles_float_motion_joint_17_angles[9] = {
#include "stg25/player_motions/knuckles_float_motion_joint_17_angles.inc"
};
static NJS_MKEY_A knuckles_float_motion_joint_18_angles[9] = {
#include "stg25/player_motions/knuckles_float_motion_joint_18_angles.inc"
};
static NJS_MKEY_A knuckles_float_motion_joint_19_angles[9] = {
#include "stg25/player_motions/knuckles_float_motion_joint_19_angles.inc"
};
static NJS_MKEY_A knuckles_float_motion_joint_23_angles[9] = {
#include "stg25/player_motions/knuckles_float_motion_joint_23_angles.inc"
};
static NJS_MKEY_A knuckles_float_motion_joint_24_angles[8] = {
#include "stg25/player_motions/knuckles_float_motion_joint_24_angles.inc"
};
static NJS_MKEY_A knuckles_float_motion_joint_27_angles[9] = {
#include "stg25/player_motions/knuckles_float_motion_joint_27_angles.inc"
};
static NJS_MKEY_A knuckles_float_motion_joint_32_angles[9] = {
#include "stg25/player_motions/knuckles_float_motion_joint_32_angles.inc"
};
static NJS_MKEY_A knuckles_float_motion_joint_33_angles[5] = {
#include "stg25/player_motions/knuckles_float_motion_joint_33_angles.inc"
};
static NJS_MKEY_A knuckles_float_motion_joint_37_angles[9] = {
#include "stg25/player_motions/knuckles_float_motion_joint_37_angles.inc"
};
static NJS_MKEY_A knuckles_float_motion_joint_38_angles[9] = {
#include "stg25/player_motions/knuckles_float_motion_joint_38_angles.inc"
};
static NJS_MKEY_A knuckles_float_motion_joint_39_angles[9] = {
#include "stg25/player_motions/knuckles_float_motion_joint_39_angles.inc"
};
static NJS_MKEY_A knuckles_float_motion_joint_41_angles[9] = {
#include "stg25/player_motions/knuckles_float_motion_joint_41_angles.inc"
};
static NJS_MKEY_A knuckles_float_motion_joint_44_angles[9] = {
#include "stg25/player_motions/knuckles_float_motion_joint_44_angles.inc"
};
static NJS_MKEY_A knuckles_float_motion_joint_56_angles[9] = {
#include "stg25/player_motions/knuckles_float_motion_joint_56_angles.inc"
};
static NJS_MKEY_A knuckles_float_motion_joint_57_angles[9] = {
#include "stg25/player_motions/knuckles_float_motion_joint_57_angles.inc"
};
static NJS_MKEY_A knuckles_float_motion_joint_58_angles[9] = {
#include "stg25/player_motions/knuckles_float_motion_joint_58_angles.inc"
};
static NJS_MKEY_A knuckles_float_motion_joint_60_angles[9] = {
#include "stg25/player_motions/knuckles_float_motion_joint_60_angles.inc"
};
static NJS_MDATA2 knuckles_float_motion_tracks[62] = {
#include "stg25/player_motions/knuckles_float_motion_tracks.inc"
};
static NJS_MOTION knuckles_float_motion = {
#include "stg25/player_motions/knuckles_float_motion.inc"
};
static NJS_MKEY_F knuckles_dive_start_motion_joint_2_position[8] = {
#include "stg25/player_motions/knuckles_dive_start_motion_joint_2_position.inc"
};
static NJS_MKEY_A knuckles_dive_start_motion_joint_4_angles[8] = {
#include "stg25/player_motions/knuckles_dive_start_motion_joint_4_angles.inc"
};
static NJS_MKEY_A knuckles_dive_start_motion_joint_7_angles[8] = {
#include "stg25/player_motions/knuckles_dive_start_motion_joint_7_angles.inc"
};
static NJS_MKEY_A knuckles_dive_start_motion_joint_8_angles[8] = {
#include "stg25/player_motions/knuckles_dive_start_motion_joint_8_angles.inc"
};
static NJS_MKEY_A knuckles_dive_start_motion_joint_9_angles[8] = {
#include "stg25/player_motions/knuckles_dive_start_motion_joint_9_angles.inc"
};
static NJS_MKEY_A knuckles_dive_start_motion_joint_13_angles[8] = {
#include "stg25/player_motions/knuckles_dive_start_motion_joint_13_angles.inc"
};
static NJS_MKEY_A knuckles_dive_start_motion_joint_14_angles[6] = {
#include "stg25/player_motions/knuckles_dive_start_motion_joint_14_angles.inc"
};
static NJS_MKEY_A knuckles_dive_start_motion_joint_17_angles[8] = {
#include "stg25/player_motions/knuckles_dive_start_motion_joint_17_angles.inc"
};
static NJS_MKEY_A knuckles_dive_start_motion_joint_18_angles[8] = {
#include "stg25/player_motions/knuckles_dive_start_motion_joint_18_angles.inc"
};
static NJS_MKEY_A knuckles_dive_start_motion_joint_19_angles[8] = {
#include "stg25/player_motions/knuckles_dive_start_motion_joint_19_angles.inc"
};
static NJS_MKEY_A knuckles_dive_start_motion_joint_23_angles[8] = {
#include "stg25/player_motions/knuckles_dive_start_motion_joint_23_angles.inc"
};
static NJS_MKEY_A knuckles_dive_start_motion_joint_24_angles[5] = {
#include "stg25/player_motions/knuckles_dive_start_motion_joint_24_angles.inc"
};
static NJS_MKEY_A knuckles_dive_start_motion_joint_27_angles[8] = {
#include "stg25/player_motions/knuckles_dive_start_motion_joint_27_angles.inc"
};
static NJS_MKEY_A knuckles_dive_start_motion_joint_32_angles[8] = {
#include "stg25/player_motions/knuckles_dive_start_motion_joint_32_angles.inc"
};
static NJS_MKEY_A knuckles_dive_start_motion_joint_33_angles[5] = {
#include "stg25/player_motions/knuckles_dive_start_motion_joint_33_angles.inc"
};
static NJS_MKEY_A knuckles_dive_start_motion_joint_37_angles[8] = {
#include "stg25/player_motions/knuckles_dive_start_motion_joint_37_angles.inc"
};
static NJS_MKEY_A knuckles_dive_start_motion_joint_38_angles[5] = {
#include "stg25/player_motions/knuckles_dive_start_motion_joint_38_angles.inc"
};
static NJS_MKEY_A knuckles_dive_start_motion_joint_39_angles[5] = {
#include "stg25/player_motions/knuckles_dive_start_motion_joint_39_angles.inc"
};
static NJS_MKEY_A knuckles_dive_start_motion_joint_41_angles[8] = {
#include "stg25/player_motions/knuckles_dive_start_motion_joint_41_angles.inc"
};
static NJS_MKEY_A knuckles_dive_start_motion_joint_44_angles[8] = {
#include "stg25/player_motions/knuckles_dive_start_motion_joint_44_angles.inc"
};
static NJS_MKEY_A knuckles_dive_start_motion_joint_56_angles[8] = {
#include "stg25/player_motions/knuckles_dive_start_motion_joint_56_angles.inc"
};
static NJS_MKEY_A knuckles_dive_start_motion_joint_57_angles[5] = {
#include "stg25/player_motions/knuckles_dive_start_motion_joint_57_angles.inc"
};
static NJS_MKEY_A knuckles_dive_start_motion_joint_58_angles[5] = {
#include "stg25/player_motions/knuckles_dive_start_motion_joint_58_angles.inc"
};
static NJS_MKEY_A knuckles_dive_start_motion_joint_60_angles[8] = {
#include "stg25/player_motions/knuckles_dive_start_motion_joint_60_angles.inc"
};
static NJS_MDATA2 knuckles_dive_start_motion_tracks[62] = {
#include "stg25/player_motions/knuckles_dive_start_motion_tracks.inc"
};
static NJS_MOTION knuckles_dive_start_motion = {
#include "stg25/player_motions/knuckles_dive_start_motion.inc"
};
static NJS_MKEY_F knuckles_dive_motion_joint_2_position[4] = {
#include "stg25/player_motions/knuckles_dive_motion_joint_2_position.inc"
};
static NJS_MKEY_A knuckles_dive_motion_joint_4_angles[4] = {
#include "stg25/player_motions/knuckles_dive_motion_joint_4_angles.inc"
};
static NJS_MKEY_A knuckles_dive_motion_joint_7_angles[4] = {
#include "stg25/player_motions/knuckles_dive_motion_joint_7_angles.inc"
};
static NJS_MKEY_A knuckles_dive_motion_joint_8_angles[4] = {
#include "stg25/player_motions/knuckles_dive_motion_joint_8_angles.inc"
};
static NJS_MKEY_A knuckles_dive_motion_joint_9_angles[4] = {
#include "stg25/player_motions/knuckles_dive_motion_joint_9_angles.inc"
};
static NJS_MKEY_A knuckles_dive_motion_joint_13_angles[4] = {
#include "stg25/player_motions/knuckles_dive_motion_joint_13_angles.inc"
};
static NJS_MKEY_A knuckles_dive_motion_joint_17_angles[4] = {
#include "stg25/player_motions/knuckles_dive_motion_joint_17_angles.inc"
};
static NJS_MKEY_A knuckles_dive_motion_joint_18_angles[4] = {
#include "stg25/player_motions/knuckles_dive_motion_joint_18_angles.inc"
};
static NJS_MKEY_A knuckles_dive_motion_joint_19_angles[4] = {
#include "stg25/player_motions/knuckles_dive_motion_joint_19_angles.inc"
};
static NJS_MKEY_A knuckles_dive_motion_joint_23_angles[4] = {
#include "stg25/player_motions/knuckles_dive_motion_joint_23_angles.inc"
};
static NJS_MKEY_A knuckles_dive_motion_joint_24_angles[4] = {
#include "stg25/player_motions/knuckles_dive_motion_joint_24_angles.inc"
};
static NJS_MKEY_A knuckles_dive_motion_joint_27_angles[4] = {
#include "stg25/player_motions/knuckles_dive_motion_joint_27_angles.inc"
};
static NJS_MKEY_A knuckles_dive_motion_joint_32_angles[4] = {
#include "stg25/player_motions/knuckles_dive_motion_joint_32_angles.inc"
};
static NJS_MKEY_A knuckles_dive_motion_joint_33_angles[4] = {
#include "stg25/player_motions/knuckles_dive_motion_joint_33_angles.inc"
};
static NJS_MKEY_A knuckles_dive_motion_joint_37_angles[4] = {
#include "stg25/player_motions/knuckles_dive_motion_joint_37_angles.inc"
};
static NJS_MKEY_A knuckles_dive_motion_joint_38_angles[4] = {
#include "stg25/player_motions/knuckles_dive_motion_joint_38_angles.inc"
};
static NJS_MKEY_A knuckles_dive_motion_joint_39_angles[4] = {
#include "stg25/player_motions/knuckles_dive_motion_joint_39_angles.inc"
};
static NJS_MKEY_A knuckles_dive_motion_joint_41_angles[4] = {
#include "stg25/player_motions/knuckles_dive_motion_joint_41_angles.inc"
};
static NJS_MKEY_A knuckles_dive_motion_joint_44_angles[4] = {
#include "stg25/player_motions/knuckles_dive_motion_joint_44_angles.inc"
};
static NJS_MKEY_A knuckles_dive_motion_joint_56_angles[4] = {
#include "stg25/player_motions/knuckles_dive_motion_joint_56_angles.inc"
};
static NJS_MKEY_A knuckles_dive_motion_joint_57_angles[2] = {
#include "stg25/player_motions/knuckles_dive_motion_joint_57_angles.inc"
};
static NJS_MKEY_A knuckles_dive_motion_joint_58_angles[2] = {
#include "stg25/player_motions/knuckles_dive_motion_joint_58_angles.inc"
};
static NJS_MKEY_A knuckles_dive_motion_joint_60_angles[4] = {
#include "stg25/player_motions/knuckles_dive_motion_joint_60_angles.inc"
};
static NJS_MDATA2 knuckles_dive_motion_tracks[62] = {
#include "stg25/player_motions/knuckles_dive_motion_tracks.inc"
};
static NJS_MOTION knuckles_dive_motion = {
#include "stg25/player_motions/knuckles_dive_motion.inc"
};
static NJS_MKEY_F knuckles_surface_swim_motion_joint_2_position[16] = {
#include "stg25/player_motions/knuckles_surface_swim_motion_joint_2_position.inc"
};
static NJS_MKEY_A knuckles_surface_swim_motion_joint_4_angles[16] = {
#include "stg25/player_motions/knuckles_surface_swim_motion_joint_4_angles.inc"
};
static NJS_MKEY_A knuckles_surface_swim_motion_joint_7_angles[16] = {
#include "stg25/player_motions/knuckles_surface_swim_motion_joint_7_angles.inc"
};
static NJS_MKEY_A knuckles_surface_swim_motion_joint_8_angles[16] = {
#include "stg25/player_motions/knuckles_surface_swim_motion_joint_8_angles.inc"
};
static NJS_MKEY_A knuckles_surface_swim_motion_joint_9_angles[16] = {
#include "stg25/player_motions/knuckles_surface_swim_motion_joint_9_angles.inc"
};
static NJS_MKEY_A knuckles_surface_swim_motion_joint_13_angles[16] = {
#include "stg25/player_motions/knuckles_surface_swim_motion_joint_13_angles.inc"
};
static NJS_MKEY_A knuckles_surface_swim_motion_joint_14_angles[12] = {
#include "stg25/player_motions/knuckles_surface_swim_motion_joint_14_angles.inc"
};
static NJS_MKEY_A knuckles_surface_swim_motion_joint_17_angles[16] = {
#include "stg25/player_motions/knuckles_surface_swim_motion_joint_17_angles.inc"
};
static NJS_MKEY_A knuckles_surface_swim_motion_joint_18_angles[16] = {
#include "stg25/player_motions/knuckles_surface_swim_motion_joint_18_angles.inc"
};
static NJS_MKEY_A knuckles_surface_swim_motion_joint_19_angles[16] = {
#include "stg25/player_motions/knuckles_surface_swim_motion_joint_19_angles.inc"
};
static NJS_MKEY_A knuckles_surface_swim_motion_joint_23_angles[16] = {
#include "stg25/player_motions/knuckles_surface_swim_motion_joint_23_angles.inc"
};
static NJS_MKEY_A knuckles_surface_swim_motion_joint_24_angles[12] = {
#include "stg25/player_motions/knuckles_surface_swim_motion_joint_24_angles.inc"
};
static NJS_MKEY_A knuckles_surface_swim_motion_joint_27_angles[16] = {
#include "stg25/player_motions/knuckles_surface_swim_motion_joint_27_angles.inc"
};
static NJS_MKEY_A knuckles_surface_swim_motion_joint_31_angles[16] = {
#include "stg25/player_motions/knuckles_surface_swim_motion_joint_31_angles.inc"
};
static NJS_MKEY_A knuckles_surface_swim_motion_joint_32_angles[16] = {
#include "stg25/player_motions/knuckles_surface_swim_motion_joint_32_angles.inc"
};
static NJS_MKEY_A knuckles_surface_swim_motion_joint_33_angles[12] = {
#include "stg25/player_motions/knuckles_surface_swim_motion_joint_33_angles.inc"
};
static NJS_MKEY_A knuckles_surface_swim_motion_joint_37_angles[16] = {
#include "stg25/player_motions/knuckles_surface_swim_motion_joint_37_angles.inc"
};
static NJS_MKEY_A knuckles_surface_swim_motion_joint_38_angles[16] = {
#include "stg25/player_motions/knuckles_surface_swim_motion_joint_38_angles.inc"
};
static NJS_MKEY_A knuckles_surface_swim_motion_joint_39_angles[16] = {
#include "stg25/player_motions/knuckles_surface_swim_motion_joint_39_angles.inc"
};
static NJS_MKEY_A knuckles_surface_swim_motion_joint_41_angles[16] = {
#include "stg25/player_motions/knuckles_surface_swim_motion_joint_41_angles.inc"
};
static NJS_MKEY_A knuckles_surface_swim_motion_joint_44_angles[16] = {
#include "stg25/player_motions/knuckles_surface_swim_motion_joint_44_angles.inc"
};
static NJS_MKEY_A knuckles_surface_swim_motion_joint_56_angles[16] = {
#include "stg25/player_motions/knuckles_surface_swim_motion_joint_56_angles.inc"
};
static NJS_MKEY_A knuckles_surface_swim_motion_joint_57_angles[16] = {
#include "stg25/player_motions/knuckles_surface_swim_motion_joint_57_angles.inc"
};
static NJS_MKEY_A knuckles_surface_swim_motion_joint_58_angles[16] = {
#include "stg25/player_motions/knuckles_surface_swim_motion_joint_58_angles.inc"
};
static NJS_MKEY_A knuckles_surface_swim_motion_joint_60_angles[16] = {
#include "stg25/player_motions/knuckles_surface_swim_motion_joint_60_angles.inc"
};
static NJS_MDATA2 knuckles_surface_swim_motion_tracks[62] = {
#include "stg25/player_motions/knuckles_surface_swim_motion_tracks.inc"
};
static NJS_MOTION knuckles_surface_swim_motion = {
#include "stg25/player_motions/knuckles_surface_swim_motion.inc"
};
static NJS_MKEY_F knuckles_surface_start_motion_joint_2_position[2] = {
#include "stg25/player_motions/knuckles_surface_start_motion_joint_2_position.inc"
};
static NJS_MKEY_A knuckles_surface_start_motion_joint_4_angles[8] = {
#include "stg25/player_motions/knuckles_surface_start_motion_joint_4_angles.inc"
};
static NJS_MKEY_A knuckles_surface_start_motion_joint_7_angles[8] = {
#include "stg25/player_motions/knuckles_surface_start_motion_joint_7_angles.inc"
};
static NJS_MKEY_A knuckles_surface_start_motion_joint_8_angles[8] = {
#include "stg25/player_motions/knuckles_surface_start_motion_joint_8_angles.inc"
};
static NJS_MKEY_A knuckles_surface_start_motion_joint_9_angles[8] = {
#include "stg25/player_motions/knuckles_surface_start_motion_joint_9_angles.inc"
};
static NJS_MKEY_A knuckles_surface_start_motion_joint_13_angles[8] = {
#include "stg25/player_motions/knuckles_surface_start_motion_joint_13_angles.inc"
};
static NJS_MKEY_A knuckles_surface_start_motion_joint_14_angles[8] = {
#include "stg25/player_motions/knuckles_surface_start_motion_joint_14_angles.inc"
};
static NJS_MKEY_A knuckles_surface_start_motion_joint_17_angles[8] = {
#include "stg25/player_motions/knuckles_surface_start_motion_joint_17_angles.inc"
};
static NJS_MKEY_A knuckles_surface_start_motion_joint_18_angles[8] = {
#include "stg25/player_motions/knuckles_surface_start_motion_joint_18_angles.inc"
};
static NJS_MKEY_A knuckles_surface_start_motion_joint_19_angles[8] = {
#include "stg25/player_motions/knuckles_surface_start_motion_joint_19_angles.inc"
};
static NJS_MKEY_A knuckles_surface_start_motion_joint_23_angles[8] = {
#include "stg25/player_motions/knuckles_surface_start_motion_joint_23_angles.inc"
};
static NJS_MKEY_A knuckles_surface_start_motion_joint_24_angles[8] = {
#include "stg25/player_motions/knuckles_surface_start_motion_joint_24_angles.inc"
};
static NJS_MKEY_A knuckles_surface_start_motion_joint_27_angles[8] = {
#include "stg25/player_motions/knuckles_surface_start_motion_joint_27_angles.inc"
};
static NJS_MKEY_A knuckles_surface_start_motion_joint_31_angles[8] = {
#include "stg25/player_motions/knuckles_surface_start_motion_joint_31_angles.inc"
};
static NJS_MKEY_A knuckles_surface_start_motion_joint_32_angles[8] = {
#include "stg25/player_motions/knuckles_surface_start_motion_joint_32_angles.inc"
};
static NJS_MKEY_A knuckles_surface_start_motion_joint_37_angles[8] = {
#include "stg25/player_motions/knuckles_surface_start_motion_joint_37_angles.inc"
};
static NJS_MKEY_A knuckles_surface_start_motion_joint_38_angles[8] = {
#include "stg25/player_motions/knuckles_surface_start_motion_joint_38_angles.inc"
};
static NJS_MKEY_A knuckles_surface_start_motion_joint_39_angles[8] = {
#include "stg25/player_motions/knuckles_surface_start_motion_joint_39_angles.inc"
};
static NJS_MKEY_A knuckles_surface_start_motion_joint_41_angles[8] = {
#include "stg25/player_motions/knuckles_surface_start_motion_joint_41_angles.inc"
};
static NJS_MKEY_A knuckles_surface_start_motion_joint_44_angles[8] = {
#include "stg25/player_motions/knuckles_surface_start_motion_joint_44_angles.inc"
};
static NJS_MKEY_A knuckles_surface_start_motion_joint_56_angles[8] = {
#include "stg25/player_motions/knuckles_surface_start_motion_joint_56_angles.inc"
};
static NJS_MKEY_A knuckles_surface_start_motion_joint_57_angles[8] = {
#include "stg25/player_motions/knuckles_surface_start_motion_joint_57_angles.inc"
};
static NJS_MKEY_A knuckles_surface_start_motion_joint_58_angles[8] = {
#include "stg25/player_motions/knuckles_surface_start_motion_joint_58_angles.inc"
};
static NJS_MKEY_A knuckles_surface_start_motion_joint_60_angles[8] = {
#include "stg25/player_motions/knuckles_surface_start_motion_joint_60_angles.inc"
};
static NJS_MDATA2 knuckles_surface_start_motion_tracks[62] = {
#include "stg25/player_motions/knuckles_surface_start_motion_tracks.inc"
};
static NJS_MOTION knuckles_surface_start_motion = {
#include "stg25/player_motions/knuckles_surface_start_motion.inc"
};
static NJS_MKEY_F knuckles_surface_motion_joint_2_position[8] = {
#include "stg25/player_motions/knuckles_surface_motion_joint_2_position.inc"
};
static NJS_MKEY_A knuckles_surface_motion_joint_4_angles[2] = {
#include "stg25/player_motions/knuckles_surface_motion_joint_4_angles.inc"
};
static NJS_MKEY_A knuckles_surface_motion_joint_7_angles[8] = {
#include "stg25/player_motions/knuckles_surface_motion_joint_7_angles.inc"
};
static NJS_MKEY_A knuckles_surface_motion_joint_8_angles[8] = {
#include "stg25/player_motions/knuckles_surface_motion_joint_8_angles.inc"
};
static NJS_MKEY_A knuckles_surface_motion_joint_9_angles[8] = {
#include "stg25/player_motions/knuckles_surface_motion_joint_9_angles.inc"
};
static NJS_MKEY_A knuckles_surface_motion_joint_13_angles[8] = {
#include "stg25/player_motions/knuckles_surface_motion_joint_13_angles.inc"
};
static NJS_MKEY_A knuckles_surface_motion_joint_17_angles[8] = {
#include "stg25/player_motions/knuckles_surface_motion_joint_17_angles.inc"
};
static NJS_MKEY_A knuckles_surface_motion_joint_18_angles[8] = {
#include "stg25/player_motions/knuckles_surface_motion_joint_18_angles.inc"
};
static NJS_MKEY_A knuckles_surface_motion_joint_19_angles[8] = {
#include "stg25/player_motions/knuckles_surface_motion_joint_19_angles.inc"
};
static NJS_MKEY_A knuckles_surface_motion_joint_23_angles[8] = {
#include "stg25/player_motions/knuckles_surface_motion_joint_23_angles.inc"
};
static NJS_MKEY_A knuckles_surface_motion_joint_27_angles[8] = {
#include "stg25/player_motions/knuckles_surface_motion_joint_27_angles.inc"
};
static NJS_MKEY_A knuckles_surface_motion_joint_31_angles[2] = {
#include "stg25/player_motions/knuckles_surface_motion_joint_31_angles.inc"
};
static NJS_MKEY_A knuckles_surface_motion_joint_32_angles[8] = {
#include "stg25/player_motions/knuckles_surface_motion_joint_32_angles.inc"
};
static NJS_MKEY_A knuckles_surface_motion_joint_33_angles[7] = {
#include "stg25/player_motions/knuckles_surface_motion_joint_33_angles.inc"
};
static NJS_MKEY_A knuckles_surface_motion_joint_37_angles[8] = {
#include "stg25/player_motions/knuckles_surface_motion_joint_37_angles.inc"
};
static NJS_MKEY_A knuckles_surface_motion_joint_38_angles[2] = {
#include "stg25/player_motions/knuckles_surface_motion_joint_38_angles.inc"
};
static NJS_MKEY_A knuckles_surface_motion_joint_39_angles[2] = {
#include "stg25/player_motions/knuckles_surface_motion_joint_39_angles.inc"
};
static NJS_MKEY_A knuckles_surface_motion_joint_41_angles[8] = {
#include "stg25/player_motions/knuckles_surface_motion_joint_41_angles.inc"
};
static NJS_MKEY_A knuckles_surface_motion_joint_44_angles[8] = {
#include "stg25/player_motions/knuckles_surface_motion_joint_44_angles.inc"
};
static NJS_MKEY_A knuckles_surface_motion_joint_56_angles[8] = {
#include "stg25/player_motions/knuckles_surface_motion_joint_56_angles.inc"
};
static NJS_MKEY_A knuckles_surface_motion_joint_57_angles[8] = {
#include "stg25/player_motions/knuckles_surface_motion_joint_57_angles.inc"
};
static NJS_MKEY_A knuckles_surface_motion_joint_58_angles[8] = {
#include "stg25/player_motions/knuckles_surface_motion_joint_58_angles.inc"
};
static NJS_MKEY_A knuckles_surface_motion_joint_60_angles[8] = {
#include "stg25/player_motions/knuckles_surface_motion_joint_60_angles.inc"
};
static NJS_MDATA2 knuckles_surface_motion_tracks[62] = {
#include "stg25/player_motions/knuckles_surface_motion_tracks.inc"
};
static NJS_MOTION knuckles_surface_motion = {
#include "stg25/player_motions/knuckles_surface_motion.inc"
};
struct motion_entry module_motions[8] = {
#include "stg25/player_motions/module_motions.inc"
};
