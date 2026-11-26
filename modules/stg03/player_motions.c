/* STG03.PRS's motions and their registry, from the disc. its D lies between
 * object_kinds.c and fallen_tree.c */
#include <ninja.h>

#include "player/player.h"
#include "shared/motions.h"

static NJS_MKEY_A upgrade_green_forest_motion_joint_4_angles[16] = {
#include "stg03/player_motions/upgrade_green_forest_motion_joint_4_angles.inc"
};
static NJS_MKEY_A upgrade_green_forest_motion_joint_7_angles[16] = {
#include "stg03/player_motions/upgrade_green_forest_motion_joint_7_angles.inc"
};
static NJS_MKEY_A upgrade_green_forest_motion_joint_8_angles[16] = {
#include "stg03/player_motions/upgrade_green_forest_motion_joint_8_angles.inc"
};
static NJS_MKEY_A upgrade_green_forest_motion_joint_9_angles[16] = {
#include "stg03/player_motions/upgrade_green_forest_motion_joint_9_angles.inc"
};
static NJS_MKEY_A upgrade_green_forest_motion_joint_13_angles[16] = {
#include "stg03/player_motions/upgrade_green_forest_motion_joint_13_angles.inc"
};
static NJS_MKEY_A upgrade_green_forest_motion_joint_14_angles[16] = {
#include "stg03/player_motions/upgrade_green_forest_motion_joint_14_angles.inc"
};
static NJS_MKEY_A upgrade_green_forest_motion_joint_17_angles[16] = {
#include "stg03/player_motions/upgrade_green_forest_motion_joint_17_angles.inc"
};
static NJS_MKEY_A upgrade_green_forest_motion_joint_18_angles[14] = {
#include "stg03/player_motions/upgrade_green_forest_motion_joint_18_angles.inc"
};
static NJS_MKEY_A upgrade_green_forest_motion_joint_19_angles[16] = {
#include "stg03/player_motions/upgrade_green_forest_motion_joint_19_angles.inc"
};
static NJS_MKEY_A upgrade_green_forest_motion_joint_23_angles[16] = {
#include "stg03/player_motions/upgrade_green_forest_motion_joint_23_angles.inc"
};
static NJS_MKEY_A upgrade_green_forest_motion_joint_27_angles[16] = {
#include "stg03/player_motions/upgrade_green_forest_motion_joint_27_angles.inc"
};
static NJS_MKEY_A upgrade_green_forest_motion_joint_28_angles[13] = {
#include "stg03/player_motions/upgrade_green_forest_motion_joint_28_angles.inc"
};
static NJS_MKEY_A upgrade_green_forest_motion_joint_29_angles[16] = {
#include "stg03/player_motions/upgrade_green_forest_motion_joint_29_angles.inc"
};
static NJS_MKEY_A upgrade_green_forest_motion_joint_32_angles[16] = {
#include "stg03/player_motions/upgrade_green_forest_motion_joint_32_angles.inc"
};
static NJS_MKEY_A upgrade_green_forest_motion_joint_33_angles[13] = {
#include "stg03/player_motions/upgrade_green_forest_motion_joint_33_angles.inc"
};
static NJS_MKEY_A upgrade_green_forest_motion_joint_37_angles[16] = {
#include "stg03/player_motions/upgrade_green_forest_motion_joint_37_angles.inc"
};
static NJS_MKEY_A upgrade_green_forest_motion_joint_38_angles[16] = {
#include "stg03/player_motions/upgrade_green_forest_motion_joint_38_angles.inc"
};
static NJS_MKEY_A upgrade_green_forest_motion_joint_39_angles[16] = {
#include "stg03/player_motions/upgrade_green_forest_motion_joint_39_angles.inc"
};
static NJS_MKEY_A upgrade_green_forest_motion_joint_41_angles[16] = {
#include "stg03/player_motions/upgrade_green_forest_motion_joint_41_angles.inc"
};
static NJS_MKEY_A upgrade_green_forest_motion_joint_44_angles[16] = {
#include "stg03/player_motions/upgrade_green_forest_motion_joint_44_angles.inc"
};
static NJS_MKEY_A upgrade_green_forest_motion_joint_51_angles[16] = {
#include "stg03/player_motions/upgrade_green_forest_motion_joint_51_angles.inc"
};
static NJS_MKEY_A upgrade_green_forest_motion_joint_53_angles[16] = {
#include "stg03/player_motions/upgrade_green_forest_motion_joint_53_angles.inc"
};
static NJS_MKEY_A upgrade_green_forest_motion_joint_56_angles[16] = {
#include "stg03/player_motions/upgrade_green_forest_motion_joint_56_angles.inc"
};
static NJS_MKEY_A upgrade_green_forest_motion_joint_57_angles[13] = {
#include "stg03/player_motions/upgrade_green_forest_motion_joint_57_angles.inc"
};
static NJS_MKEY_A upgrade_green_forest_motion_joint_58_angles[13] = {
#include "stg03/player_motions/upgrade_green_forest_motion_joint_58_angles.inc"
};
static NJS_MKEY_A upgrade_green_forest_motion_joint_60_angles[16] = {
#include "stg03/player_motions/upgrade_green_forest_motion_joint_60_angles.inc"
};
static NJS_MDATA2 upgrade_green_forest_motion_tracks[62] = {
#include "stg03/player_motions/upgrade_green_forest_motion_tracks.inc"
};
static NJS_MOTION upgrade_green_forest_motion = {
#include "stg03/player_motions/upgrade_green_forest_motion.inc"
};
static NJS_MKEY_F vine_catch_motion_joint_2_position[8] = {
#include "stg03/player_motions/vine_catch_motion_joint_2_position.inc"
};
static NJS_MKEY_A vine_catch_motion_joint_4_angles[8] = {
#include "stg03/player_motions/vine_catch_motion_joint_4_angles.inc"
};
static NJS_MKEY_A vine_catch_motion_joint_7_angles[8] = {
#include "stg03/player_motions/vine_catch_motion_joint_7_angles.inc"
};
static NJS_MKEY_A vine_catch_motion_joint_8_angles[7] = {
#include "stg03/player_motions/vine_catch_motion_joint_8_angles.inc"
};
static NJS_MKEY_A vine_catch_motion_joint_9_angles[7] = {
#include "stg03/player_motions/vine_catch_motion_joint_9_angles.inc"
};
static NJS_MKEY_A vine_catch_motion_joint_13_angles[8] = {
#include "stg03/player_motions/vine_catch_motion_joint_13_angles.inc"
};
static NJS_MKEY_A vine_catch_motion_joint_14_angles[8] = {
#include "stg03/player_motions/vine_catch_motion_joint_14_angles.inc"
};
static NJS_MKEY_A vine_catch_motion_joint_17_angles[8] = {
#include "stg03/player_motions/vine_catch_motion_joint_17_angles.inc"
};
static NJS_MKEY_A vine_catch_motion_joint_18_angles[8] = {
#include "stg03/player_motions/vine_catch_motion_joint_18_angles.inc"
};
static NJS_MKEY_A vine_catch_motion_joint_19_angles[8] = {
#include "stg03/player_motions/vine_catch_motion_joint_19_angles.inc"
};
static NJS_MKEY_A vine_catch_motion_joint_23_angles[8] = {
#include "stg03/player_motions/vine_catch_motion_joint_23_angles.inc"
};
static NJS_MKEY_A vine_catch_motion_joint_24_angles[7] = {
#include "stg03/player_motions/vine_catch_motion_joint_24_angles.inc"
};
static NJS_MKEY_A vine_catch_motion_joint_27_angles[8] = {
#include "stg03/player_motions/vine_catch_motion_joint_27_angles.inc"
};
static NJS_MKEY_A vine_catch_motion_joint_28_angles[4] = {
#include "stg03/player_motions/vine_catch_motion_joint_28_angles.inc"
};
static NJS_MKEY_A vine_catch_motion_joint_29_angles[4] = {
#include "stg03/player_motions/vine_catch_motion_joint_29_angles.inc"
};
static NJS_MKEY_A vine_catch_motion_joint_31_angles[8] = {
#include "stg03/player_motions/vine_catch_motion_joint_31_angles.inc"
};
static NJS_MKEY_A vine_catch_motion_joint_32_angles[7] = {
#include "stg03/player_motions/vine_catch_motion_joint_32_angles.inc"
};
static NJS_MKEY_A vine_catch_motion_joint_33_angles[7] = {
#include "stg03/player_motions/vine_catch_motion_joint_33_angles.inc"
};
static NJS_MKEY_A vine_catch_motion_joint_37_angles[8] = {
#include "stg03/player_motions/vine_catch_motion_joint_37_angles.inc"
};
static NJS_MKEY_A vine_catch_motion_joint_38_angles[8] = {
#include "stg03/player_motions/vine_catch_motion_joint_38_angles.inc"
};
static NJS_MKEY_A vine_catch_motion_joint_39_angles[8] = {
#include "stg03/player_motions/vine_catch_motion_joint_39_angles.inc"
};
static NJS_MKEY_A vine_catch_motion_joint_41_angles[8] = {
#include "stg03/player_motions/vine_catch_motion_joint_41_angles.inc"
};
static NJS_MKEY_A vine_catch_motion_joint_44_angles[8] = {
#include "stg03/player_motions/vine_catch_motion_joint_44_angles.inc"
};
static NJS_MKEY_A vine_catch_motion_joint_51_angles[8] = {
#include "stg03/player_motions/vine_catch_motion_joint_51_angles.inc"
};
static NJS_MKEY_A vine_catch_motion_joint_53_angles[8] = {
#include "stg03/player_motions/vine_catch_motion_joint_53_angles.inc"
};
static NJS_MKEY_A vine_catch_motion_joint_56_angles[8] = {
#include "stg03/player_motions/vine_catch_motion_joint_56_angles.inc"
};
static NJS_MKEY_A vine_catch_motion_joint_57_angles[2] = {
#include "stg03/player_motions/vine_catch_motion_joint_57_angles.inc"
};
static NJS_MKEY_A vine_catch_motion_joint_58_angles[2] = {
#include "stg03/player_motions/vine_catch_motion_joint_58_angles.inc"
};
static NJS_MKEY_A vine_catch_motion_joint_60_angles[8] = {
#include "stg03/player_motions/vine_catch_motion_joint_60_angles.inc"
};
static NJS_MDATA2 vine_catch_motion_tracks[62] = {
#include "stg03/player_motions/vine_catch_motion_tracks.inc"
};
static NJS_MOTION vine_catch_motion = {
#include "stg03/player_motions/vine_catch_motion.inc"
};
static NJS_MKEY_F vine_hang_motion_joint_2_position[2] = {
#include "stg03/player_motions/vine_hang_motion_joint_2_position.inc"
};
static NJS_MKEY_A vine_hang_motion_joint_4_angles[4] = {
#include "stg03/player_motions/vine_hang_motion_joint_4_angles.inc"
};
static NJS_MKEY_A vine_hang_motion_joint_7_angles[4] = {
#include "stg03/player_motions/vine_hang_motion_joint_7_angles.inc"
};
static NJS_MKEY_A vine_hang_motion_joint_8_angles[4] = {
#include "stg03/player_motions/vine_hang_motion_joint_8_angles.inc"
};
static NJS_MKEY_A vine_hang_motion_joint_9_angles[4] = {
#include "stg03/player_motions/vine_hang_motion_joint_9_angles.inc"
};
static NJS_MKEY_A vine_hang_motion_joint_13_angles[4] = {
#include "stg03/player_motions/vine_hang_motion_joint_13_angles.inc"
};
static NJS_MKEY_A vine_hang_motion_joint_14_angles[4] = {
#include "stg03/player_motions/vine_hang_motion_joint_14_angles.inc"
};
static NJS_MKEY_A vine_hang_motion_joint_17_angles[4] = {
#include "stg03/player_motions/vine_hang_motion_joint_17_angles.inc"
};
static NJS_MKEY_A vine_hang_motion_joint_18_angles[3] = {
#include "stg03/player_motions/vine_hang_motion_joint_18_angles.inc"
};
static NJS_MKEY_A vine_hang_motion_joint_19_angles[4] = {
#include "stg03/player_motions/vine_hang_motion_joint_19_angles.inc"
};
static NJS_MKEY_A vine_hang_motion_joint_23_angles[4] = {
#include "stg03/player_motions/vine_hang_motion_joint_23_angles.inc"
};
static NJS_MKEY_A vine_hang_motion_joint_24_angles[4] = {
#include "stg03/player_motions/vine_hang_motion_joint_24_angles.inc"
};
static NJS_MKEY_A vine_hang_motion_joint_27_angles[4] = {
#include "stg03/player_motions/vine_hang_motion_joint_27_angles.inc"
};
static NJS_MKEY_A vine_hang_motion_joint_31_angles[2] = {
#include "stg03/player_motions/vine_hang_motion_joint_31_angles.inc"
};
static NJS_MKEY_A vine_hang_motion_joint_32_angles[4] = {
#include "stg03/player_motions/vine_hang_motion_joint_32_angles.inc"
};
static NJS_MKEY_A vine_hang_motion_joint_33_angles[4] = {
#include "stg03/player_motions/vine_hang_motion_joint_33_angles.inc"
};
static NJS_MKEY_A vine_hang_motion_joint_37_angles[4] = {
#include "stg03/player_motions/vine_hang_motion_joint_37_angles.inc"
};
static NJS_MKEY_A vine_hang_motion_joint_38_angles[4] = {
#include "stg03/player_motions/vine_hang_motion_joint_38_angles.inc"
};
static NJS_MKEY_A vine_hang_motion_joint_39_angles[4] = {
#include "stg03/player_motions/vine_hang_motion_joint_39_angles.inc"
};
static NJS_MKEY_A vine_hang_motion_joint_41_angles[4] = {
#include "stg03/player_motions/vine_hang_motion_joint_41_angles.inc"
};
static NJS_MKEY_A vine_hang_motion_joint_44_angles[4] = {
#include "stg03/player_motions/vine_hang_motion_joint_44_angles.inc"
};
static NJS_MKEY_A vine_hang_motion_joint_51_angles[4] = {
#include "stg03/player_motions/vine_hang_motion_joint_51_angles.inc"
};
static NJS_MKEY_A vine_hang_motion_joint_53_angles[4] = {
#include "stg03/player_motions/vine_hang_motion_joint_53_angles.inc"
};
static NJS_MKEY_A vine_hang_motion_joint_56_angles[4] = {
#include "stg03/player_motions/vine_hang_motion_joint_56_angles.inc"
};
static NJS_MKEY_A vine_hang_motion_joint_57_angles[2] = {
#include "stg03/player_motions/vine_hang_motion_joint_57_angles.inc"
};
static NJS_MKEY_A vine_hang_motion_joint_58_angles[2] = {
#include "stg03/player_motions/vine_hang_motion_joint_58_angles.inc"
};
static NJS_MKEY_A vine_hang_motion_joint_60_angles[4] = {
#include "stg03/player_motions/vine_hang_motion_joint_60_angles.inc"
};
static NJS_MDATA2 vine_hang_motion_tracks[62] = {
#include "stg03/player_motions/vine_hang_motion_tracks.inc"
};
static NJS_MOTION vine_hang_motion = {
#include "stg03/player_motions/vine_hang_motion.inc"
};
static NJS_MKEY_A bungee_ride_motion_joint_4_angles[2] = {
#include "stg03/player_motions/bungee_ride_motion_joint_4_angles.inc"
};
static NJS_MKEY_A bungee_ride_motion_joint_7_angles[17] = {
#include "stg03/player_motions/bungee_ride_motion_joint_7_angles.inc"
};
static NJS_MKEY_A bungee_ride_motion_joint_8_angles[17] = {
#include "stg03/player_motions/bungee_ride_motion_joint_8_angles.inc"
};
static NJS_MKEY_A bungee_ride_motion_joint_9_angles[17] = {
#include "stg03/player_motions/bungee_ride_motion_joint_9_angles.inc"
};
static NJS_MKEY_A bungee_ride_motion_joint_13_angles[17] = {
#include "stg03/player_motions/bungee_ride_motion_joint_13_angles.inc"
};
static NJS_MKEY_A bungee_ride_motion_joint_14_angles[17] = {
#include "stg03/player_motions/bungee_ride_motion_joint_14_angles.inc"
};
static NJS_MKEY_A bungee_ride_motion_joint_17_angles[17] = {
#include "stg03/player_motions/bungee_ride_motion_joint_17_angles.inc"
};
static NJS_MKEY_A bungee_ride_motion_joint_18_angles[17] = {
#include "stg03/player_motions/bungee_ride_motion_joint_18_angles.inc"
};
static NJS_MKEY_A bungee_ride_motion_joint_19_angles[17] = {
#include "stg03/player_motions/bungee_ride_motion_joint_19_angles.inc"
};
static NJS_MKEY_A bungee_ride_motion_joint_23_angles[17] = {
#include "stg03/player_motions/bungee_ride_motion_joint_23_angles.inc"
};
static NJS_MKEY_A bungee_ride_motion_joint_24_angles[17] = {
#include "stg03/player_motions/bungee_ride_motion_joint_24_angles.inc"
};
static NJS_MKEY_A bungee_ride_motion_joint_27_angles[17] = {
#include "stg03/player_motions/bungee_ride_motion_joint_27_angles.inc"
};
static NJS_MKEY_A bungee_ride_motion_joint_31_angles[17] = {
#include "stg03/player_motions/bungee_ride_motion_joint_31_angles.inc"
};
static NJS_MKEY_A bungee_ride_motion_joint_32_angles[17] = {
#include "stg03/player_motions/bungee_ride_motion_joint_32_angles.inc"
};
static NJS_MKEY_A bungee_ride_motion_joint_33_angles[12] = {
#include "stg03/player_motions/bungee_ride_motion_joint_33_angles.inc"
};
static NJS_MKEY_A bungee_ride_motion_joint_37_angles[17] = {
#include "stg03/player_motions/bungee_ride_motion_joint_37_angles.inc"
};
static NJS_MKEY_A bungee_ride_motion_joint_38_angles[16] = {
#include "stg03/player_motions/bungee_ride_motion_joint_38_angles.inc"
};
static NJS_MKEY_A bungee_ride_motion_joint_39_angles[16] = {
#include "stg03/player_motions/bungee_ride_motion_joint_39_angles.inc"
};
static NJS_MKEY_A bungee_ride_motion_joint_41_angles[17] = {
#include "stg03/player_motions/bungee_ride_motion_joint_41_angles.inc"
};
static NJS_MKEY_A bungee_ride_motion_joint_44_angles[17] = {
#include "stg03/player_motions/bungee_ride_motion_joint_44_angles.inc"
};
static NJS_MKEY_A bungee_ride_motion_joint_51_angles[17] = {
#include "stg03/player_motions/bungee_ride_motion_joint_51_angles.inc"
};
static NJS_MKEY_A bungee_ride_motion_joint_53_angles[17] = {
#include "stg03/player_motions/bungee_ride_motion_joint_53_angles.inc"
};
static NJS_MKEY_A bungee_ride_motion_joint_56_angles[17] = {
#include "stg03/player_motions/bungee_ride_motion_joint_56_angles.inc"
};
static NJS_MKEY_A bungee_ride_motion_joint_57_angles[2] = {
#include "stg03/player_motions/bungee_ride_motion_joint_57_angles.inc"
};
static NJS_MKEY_A bungee_ride_motion_joint_58_angles[2] = {
#include "stg03/player_motions/bungee_ride_motion_joint_58_angles.inc"
};
static NJS_MKEY_A bungee_ride_motion_joint_60_angles[17] = {
#include "stg03/player_motions/bungee_ride_motion_joint_60_angles.inc"
};
static NJS_MDATA2 bungee_ride_motion_tracks[62] = {
#include "stg03/player_motions/bungee_ride_motion_tracks.inc"
};
static NJS_MOTION bungee_ride_motion = {
#include "stg03/player_motions/bungee_ride_motion.inc"
};
static NJS_MKEY_A bungee_ride_alternate_motion_joint_4_angles[16] = {
#include "stg03/player_motions/bungee_ride_alternate_motion_joint_4_angles.inc"
};
static NJS_MKEY_A bungee_ride_alternate_motion_joint_7_angles[16] = {
#include "stg03/player_motions/bungee_ride_alternate_motion_joint_7_angles.inc"
};
static NJS_MKEY_A bungee_ride_alternate_motion_joint_8_angles[16] = {
#include "stg03/player_motions/bungee_ride_alternate_motion_joint_8_angles.inc"
};
static NJS_MKEY_A bungee_ride_alternate_motion_joint_9_angles[16] = {
#include "stg03/player_motions/bungee_ride_alternate_motion_joint_9_angles.inc"
};
static NJS_MKEY_A bungee_ride_alternate_motion_joint_13_angles[16] = {
#include "stg03/player_motions/bungee_ride_alternate_motion_joint_13_angles.inc"
};
static NJS_MKEY_A bungee_ride_alternate_motion_joint_14_angles[11] = {
#include "stg03/player_motions/bungee_ride_alternate_motion_joint_14_angles.inc"
};
static NJS_MKEY_A bungee_ride_alternate_motion_joint_17_angles[16] = {
#include "stg03/player_motions/bungee_ride_alternate_motion_joint_17_angles.inc"
};
static NJS_MKEY_A bungee_ride_alternate_motion_joint_18_angles[2] = {
#include "stg03/player_motions/bungee_ride_alternate_motion_joint_18_angles.inc"
};
static NJS_MKEY_A bungee_ride_alternate_motion_joint_19_angles[2] = {
#include "stg03/player_motions/bungee_ride_alternate_motion_joint_19_angles.inc"
};
static NJS_MKEY_A bungee_ride_alternate_motion_joint_23_angles[16] = {
#include "stg03/player_motions/bungee_ride_alternate_motion_joint_23_angles.inc"
};
static NJS_MKEY_A bungee_ride_alternate_motion_joint_24_angles[15] = {
#include "stg03/player_motions/bungee_ride_alternate_motion_joint_24_angles.inc"
};
static NJS_MKEY_A bungee_ride_alternate_motion_joint_27_angles[16] = {
#include "stg03/player_motions/bungee_ride_alternate_motion_joint_27_angles.inc"
};
static NJS_MKEY_A bungee_ride_alternate_motion_joint_28_angles[16] = {
#include "stg03/player_motions/bungee_ride_alternate_motion_joint_28_angles.inc"
};
static NJS_MKEY_A bungee_ride_alternate_motion_joint_29_angles[16] = {
#include "stg03/player_motions/bungee_ride_alternate_motion_joint_29_angles.inc"
};
static NJS_MKEY_A bungee_ride_alternate_motion_joint_31_angles[2] = {
#include "stg03/player_motions/bungee_ride_alternate_motion_joint_31_angles.inc"
};
static NJS_MKEY_A bungee_ride_alternate_motion_joint_32_angles[16] = {
#include "stg03/player_motions/bungee_ride_alternate_motion_joint_32_angles.inc"
};
static NJS_MKEY_A bungee_ride_alternate_motion_joint_33_angles[13] = {
#include "stg03/player_motions/bungee_ride_alternate_motion_joint_33_angles.inc"
};
static NJS_MKEY_A bungee_ride_alternate_motion_joint_37_angles[16] = {
#include "stg03/player_motions/bungee_ride_alternate_motion_joint_37_angles.inc"
};
static NJS_MKEY_A bungee_ride_alternate_motion_joint_38_angles[2] = {
#include "stg03/player_motions/bungee_ride_alternate_motion_joint_38_angles.inc"
};
static NJS_MKEY_A bungee_ride_alternate_motion_joint_39_angles[2] = {
#include "stg03/player_motions/bungee_ride_alternate_motion_joint_39_angles.inc"
};
static NJS_MKEY_A bungee_ride_alternate_motion_joint_41_angles[16] = {
#include "stg03/player_motions/bungee_ride_alternate_motion_joint_41_angles.inc"
};
static NJS_MKEY_A bungee_ride_alternate_motion_joint_44_angles[16] = {
#include "stg03/player_motions/bungee_ride_alternate_motion_joint_44_angles.inc"
};
static NJS_MKEY_A bungee_ride_alternate_motion_joint_51_angles[16] = {
#include "stg03/player_motions/bungee_ride_alternate_motion_joint_51_angles.inc"
};
static NJS_MKEY_A bungee_ride_alternate_motion_joint_53_angles[16] = {
#include "stg03/player_motions/bungee_ride_alternate_motion_joint_53_angles.inc"
};
static NJS_MKEY_A bungee_ride_alternate_motion_joint_56_angles[16] = {
#include "stg03/player_motions/bungee_ride_alternate_motion_joint_56_angles.inc"
};
static NJS_MKEY_A bungee_ride_alternate_motion_joint_57_angles[4] = {
#include "stg03/player_motions/bungee_ride_alternate_motion_joint_57_angles.inc"
};
static NJS_MKEY_A bungee_ride_alternate_motion_joint_58_angles[4] = {
#include "stg03/player_motions/bungee_ride_alternate_motion_joint_58_angles.inc"
};
static NJS_MKEY_A bungee_ride_alternate_motion_joint_60_angles[16] = {
#include "stg03/player_motions/bungee_ride_alternate_motion_joint_60_angles.inc"
};
static NJS_MDATA2 bungee_ride_alternate_motion_tracks[62] = {
#include "stg03/player_motions/bungee_ride_alternate_motion_tracks.inc"
};
static NJS_MOTION bungee_ride_alternate_motion = {
#include "stg03/player_motions/bungee_ride_alternate_motion.inc"
};
static NJS_MKEY_A bungee_catch_motion_joint_4_angles[16] = {
#include "stg03/player_motions/bungee_catch_motion_joint_4_angles.inc"
};
static NJS_MKEY_A bungee_catch_motion_joint_7_angles[16] = {
#include "stg03/player_motions/bungee_catch_motion_joint_7_angles.inc"
};
static NJS_MKEY_A bungee_catch_motion_joint_8_angles[14] = {
#include "stg03/player_motions/bungee_catch_motion_joint_8_angles.inc"
};
static NJS_MKEY_A bungee_catch_motion_joint_9_angles[16] = {
#include "stg03/player_motions/bungee_catch_motion_joint_9_angles.inc"
};
static NJS_MKEY_A bungee_catch_motion_joint_13_angles[16] = {
#include "stg03/player_motions/bungee_catch_motion_joint_13_angles.inc"
};
static NJS_MKEY_A bungee_catch_motion_joint_14_angles[14] = {
#include "stg03/player_motions/bungee_catch_motion_joint_14_angles.inc"
};
static NJS_MKEY_A bungee_catch_motion_joint_17_angles[16] = {
#include "stg03/player_motions/bungee_catch_motion_joint_17_angles.inc"
};
static NJS_MKEY_A bungee_catch_motion_joint_18_angles[16] = {
#include "stg03/player_motions/bungee_catch_motion_joint_18_angles.inc"
};
static NJS_MKEY_A bungee_catch_motion_joint_19_angles[16] = {
#include "stg03/player_motions/bungee_catch_motion_joint_19_angles.inc"
};
static NJS_MKEY_A bungee_catch_motion_joint_23_angles[16] = {
#include "stg03/player_motions/bungee_catch_motion_joint_23_angles.inc"
};
static NJS_MKEY_A bungee_catch_motion_joint_24_angles[16] = {
#include "stg03/player_motions/bungee_catch_motion_joint_24_angles.inc"
};
static NJS_MKEY_A bungee_catch_motion_joint_27_angles[16] = {
#include "stg03/player_motions/bungee_catch_motion_joint_27_angles.inc"
};
static NJS_MKEY_A bungee_catch_motion_joint_31_angles[16] = {
#include "stg03/player_motions/bungee_catch_motion_joint_31_angles.inc"
};
static NJS_MKEY_A bungee_catch_motion_joint_32_angles[16] = {
#include "stg03/player_motions/bungee_catch_motion_joint_32_angles.inc"
};
static NJS_MKEY_A bungee_catch_motion_joint_33_angles[13] = {
#include "stg03/player_motions/bungee_catch_motion_joint_33_angles.inc"
};
static NJS_MKEY_A bungee_catch_motion_joint_37_angles[16] = {
#include "stg03/player_motions/bungee_catch_motion_joint_37_angles.inc"
};
static NJS_MKEY_A bungee_catch_motion_joint_38_angles[2] = {
#include "stg03/player_motions/bungee_catch_motion_joint_38_angles.inc"
};
static NJS_MKEY_A bungee_catch_motion_joint_39_angles[2] = {
#include "stg03/player_motions/bungee_catch_motion_joint_39_angles.inc"
};
static NJS_MKEY_A bungee_catch_motion_joint_41_angles[16] = {
#include "stg03/player_motions/bungee_catch_motion_joint_41_angles.inc"
};
static NJS_MKEY_A bungee_catch_motion_joint_44_angles[16] = {
#include "stg03/player_motions/bungee_catch_motion_joint_44_angles.inc"
};
static NJS_MKEY_A bungee_catch_motion_joint_51_angles[16] = {
#include "stg03/player_motions/bungee_catch_motion_joint_51_angles.inc"
};
static NJS_MKEY_A bungee_catch_motion_joint_53_angles[16] = {
#include "stg03/player_motions/bungee_catch_motion_joint_53_angles.inc"
};
static NJS_MKEY_A bungee_catch_motion_joint_56_angles[16] = {
#include "stg03/player_motions/bungee_catch_motion_joint_56_angles.inc"
};
static NJS_MKEY_A bungee_catch_motion_joint_57_angles[2] = {
#include "stg03/player_motions/bungee_catch_motion_joint_57_angles.inc"
};
static NJS_MKEY_A bungee_catch_motion_joint_58_angles[2] = {
#include "stg03/player_motions/bungee_catch_motion_joint_58_angles.inc"
};
static NJS_MKEY_A bungee_catch_motion_joint_60_angles[16] = {
#include "stg03/player_motions/bungee_catch_motion_joint_60_angles.inc"
};
static NJS_MDATA2 bungee_catch_motion_tracks[62] = {
#include "stg03/player_motions/bungee_catch_motion_tracks.inc"
};
static NJS_MOTION bungee_catch_motion = {
#include "stg03/player_motions/bungee_catch_motion.inc"
};
static NJS_MKEY_F bar_throw_back_motion_joint_0_position[2] = {
#include "stg03/player_motions/bar_throw_back_motion_joint_0_position.inc"
};
static NJS_MKEY_F bar_throw_back_motion_joint_2_position[2] = {
#include "stg03/player_motions/bar_throw_back_motion_joint_2_position.inc"
};
static NJS_MKEY_A bar_throw_back_motion_joint_4_angles[2] = {
#include "stg03/player_motions/bar_throw_back_motion_joint_4_angles.inc"
};
static NJS_MKEY_A bar_throw_back_motion_joint_7_angles[32] = {
#include "stg03/player_motions/bar_throw_back_motion_joint_7_angles.inc"
};
static NJS_MKEY_A bar_throw_back_motion_joint_8_angles[32] = {
#include "stg03/player_motions/bar_throw_back_motion_joint_8_angles.inc"
};
static NJS_MKEY_A bar_throw_back_motion_joint_9_angles[32] = {
#include "stg03/player_motions/bar_throw_back_motion_joint_9_angles.inc"
};
static NJS_MKEY_A bar_throw_back_motion_joint_13_angles[32] = {
#include "stg03/player_motions/bar_throw_back_motion_joint_13_angles.inc"
};
static NJS_MKEY_A bar_throw_back_motion_joint_14_angles[32] = {
#include "stg03/player_motions/bar_throw_back_motion_joint_14_angles.inc"
};
static NJS_MKEY_A bar_throw_back_motion_joint_17_angles[32] = {
#include "stg03/player_motions/bar_throw_back_motion_joint_17_angles.inc"
};
static NJS_MKEY_A bar_throw_back_motion_joint_18_angles[32] = {
#include "stg03/player_motions/bar_throw_back_motion_joint_18_angles.inc"
};
static NJS_MKEY_A bar_throw_back_motion_joint_19_angles[32] = {
#include "stg03/player_motions/bar_throw_back_motion_joint_19_angles.inc"
};
static NJS_MKEY_A bar_throw_back_motion_joint_23_angles[32] = {
#include "stg03/player_motions/bar_throw_back_motion_joint_23_angles.inc"
};
static NJS_MKEY_A bar_throw_back_motion_joint_24_angles[32] = {
#include "stg03/player_motions/bar_throw_back_motion_joint_24_angles.inc"
};
static NJS_MKEY_A bar_throw_back_motion_joint_27_angles[2] = {
#include "stg03/player_motions/bar_throw_back_motion_joint_27_angles.inc"
};
static NJS_MKEY_A bar_throw_back_motion_joint_28_angles[2] = {
#include "stg03/player_motions/bar_throw_back_motion_joint_28_angles.inc"
};
static NJS_MKEY_A bar_throw_back_motion_joint_29_angles[2] = {
#include "stg03/player_motions/bar_throw_back_motion_joint_29_angles.inc"
};
static NJS_MKEY_A bar_throw_back_motion_joint_32_angles[32] = {
#include "stg03/player_motions/bar_throw_back_motion_joint_32_angles.inc"
};
static NJS_MKEY_A bar_throw_back_motion_joint_33_angles[25] = {
#include "stg03/player_motions/bar_throw_back_motion_joint_33_angles.inc"
};
static NJS_MKEY_A bar_throw_back_motion_joint_37_angles[32] = {
#include "stg03/player_motions/bar_throw_back_motion_joint_37_angles.inc"
};
static NJS_MKEY_A bar_throw_back_motion_joint_38_angles[32] = {
#include "stg03/player_motions/bar_throw_back_motion_joint_38_angles.inc"
};
static NJS_MKEY_A bar_throw_back_motion_joint_39_angles[32] = {
#include "stg03/player_motions/bar_throw_back_motion_joint_39_angles.inc"
};
static NJS_MKEY_A bar_throw_back_motion_joint_41_angles[17] = {
#include "stg03/player_motions/bar_throw_back_motion_joint_41_angles.inc"
};
static NJS_MKEY_A bar_throw_back_motion_joint_44_angles[32] = {
#include "stg03/player_motions/bar_throw_back_motion_joint_44_angles.inc"
};
static NJS_MKEY_A bar_throw_back_motion_joint_51_angles[32] = {
#include "stg03/player_motions/bar_throw_back_motion_joint_51_angles.inc"
};
static NJS_MKEY_A bar_throw_back_motion_joint_53_angles[32] = {
#include "stg03/player_motions/bar_throw_back_motion_joint_53_angles.inc"
};
static NJS_MKEY_A bar_throw_back_motion_joint_56_angles[32] = {
#include "stg03/player_motions/bar_throw_back_motion_joint_56_angles.inc"
};
static NJS_MKEY_A bar_throw_back_motion_joint_57_angles[32] = {
#include "stg03/player_motions/bar_throw_back_motion_joint_57_angles.inc"
};
static NJS_MKEY_A bar_throw_back_motion_joint_58_angles[32] = {
#include "stg03/player_motions/bar_throw_back_motion_joint_58_angles.inc"
};
static NJS_MKEY_A bar_throw_back_motion_joint_60_angles[17] = {
#include "stg03/player_motions/bar_throw_back_motion_joint_60_angles.inc"
};
static NJS_MDATA2 bar_throw_back_motion_tracks[62] = {
#include "stg03/player_motions/bar_throw_back_motion_tracks.inc"
};
static NJS_MOTION bar_throw_back_motion = {
#include "stg03/player_motions/bar_throw_back_motion.inc"
};
static NJS_MKEY_F bar_throw_motion_joint_0_position[2] = {
#include "stg03/player_motions/bar_throw_motion_joint_0_position.inc"
};
static NJS_MKEY_F bar_throw_motion_joint_2_position[2] = {
#include "stg03/player_motions/bar_throw_motion_joint_2_position.inc"
};
static NJS_MKEY_A bar_throw_motion_joint_4_angles[33] = {
#include "stg03/player_motions/bar_throw_motion_joint_4_angles.inc"
};
static NJS_MKEY_A bar_throw_motion_joint_7_angles[33] = {
#include "stg03/player_motions/bar_throw_motion_joint_7_angles.inc"
};
static NJS_MKEY_A bar_throw_motion_joint_8_angles[33] = {
#include "stg03/player_motions/bar_throw_motion_joint_8_angles.inc"
};
static NJS_MKEY_A bar_throw_motion_joint_9_angles[33] = {
#include "stg03/player_motions/bar_throw_motion_joint_9_angles.inc"
};
static NJS_MKEY_A bar_throw_motion_joint_13_angles[33] = {
#include "stg03/player_motions/bar_throw_motion_joint_13_angles.inc"
};
static NJS_MKEY_A bar_throw_motion_joint_14_angles[33] = {
#include "stg03/player_motions/bar_throw_motion_joint_14_angles.inc"
};
static NJS_MKEY_A bar_throw_motion_joint_17_angles[33] = {
#include "stg03/player_motions/bar_throw_motion_joint_17_angles.inc"
};
static NJS_MKEY_A bar_throw_motion_joint_18_angles[33] = {
#include "stg03/player_motions/bar_throw_motion_joint_18_angles.inc"
};
static NJS_MKEY_A bar_throw_motion_joint_19_angles[33] = {
#include "stg03/player_motions/bar_throw_motion_joint_19_angles.inc"
};
static NJS_MKEY_A bar_throw_motion_joint_23_angles[33] = {
#include "stg03/player_motions/bar_throw_motion_joint_23_angles.inc"
};
static NJS_MKEY_A bar_throw_motion_joint_24_angles[33] = {
#include "stg03/player_motions/bar_throw_motion_joint_24_angles.inc"
};
static NJS_MKEY_A bar_throw_motion_joint_27_angles[33] = {
#include "stg03/player_motions/bar_throw_motion_joint_27_angles.inc"
};
static NJS_MKEY_A bar_throw_motion_joint_28_angles[15] = {
#include "stg03/player_motions/bar_throw_motion_joint_28_angles.inc"
};
static NJS_MKEY_A bar_throw_motion_joint_29_angles[15] = {
#include "stg03/player_motions/bar_throw_motion_joint_29_angles.inc"
};
static NJS_MKEY_A bar_throw_motion_joint_32_angles[33] = {
#include "stg03/player_motions/bar_throw_motion_joint_32_angles.inc"
};
static NJS_MKEY_A bar_throw_motion_joint_33_angles[26] = {
#include "stg03/player_motions/bar_throw_motion_joint_33_angles.inc"
};
static NJS_MKEY_A bar_throw_motion_joint_37_angles[33] = {
#include "stg03/player_motions/bar_throw_motion_joint_37_angles.inc"
};
static NJS_MKEY_A bar_throw_motion_joint_38_angles[33] = {
#include "stg03/player_motions/bar_throw_motion_joint_38_angles.inc"
};
static NJS_MKEY_A bar_throw_motion_joint_39_angles[33] = {
#include "stg03/player_motions/bar_throw_motion_joint_39_angles.inc"
};
static NJS_MKEY_A bar_throw_motion_joint_41_angles[2] = {
#include "stg03/player_motions/bar_throw_motion_joint_41_angles.inc"
};
static NJS_MKEY_A bar_throw_motion_joint_44_angles[33] = {
#include "stg03/player_motions/bar_throw_motion_joint_44_angles.inc"
};
static NJS_MKEY_A bar_throw_motion_joint_51_angles[33] = {
#include "stg03/player_motions/bar_throw_motion_joint_51_angles.inc"
};
static NJS_MKEY_A bar_throw_motion_joint_53_angles[33] = {
#include "stg03/player_motions/bar_throw_motion_joint_53_angles.inc"
};
static NJS_MKEY_A bar_throw_motion_joint_56_angles[33] = {
#include "stg03/player_motions/bar_throw_motion_joint_56_angles.inc"
};
static NJS_MKEY_A bar_throw_motion_joint_57_angles[33] = {
#include "stg03/player_motions/bar_throw_motion_joint_57_angles.inc"
};
static NJS_MKEY_A bar_throw_motion_joint_58_angles[33] = {
#include "stg03/player_motions/bar_throw_motion_joint_58_angles.inc"
};
static NJS_MKEY_A bar_throw_motion_joint_60_angles[2] = {
#include "stg03/player_motions/bar_throw_motion_joint_60_angles.inc"
};
static NJS_MDATA2 bar_throw_motion_tracks[62] = {
#include "stg03/player_motions/bar_throw_motion_tracks.inc"
};
static NJS_MOTION bar_throw_motion = {
#include "stg03/player_motions/bar_throw_motion.inc"
};
static NJS_MKEY_F bar_fly_back_motion_joint_0_position[2] = {
#include "stg03/player_motions/bar_fly_back_motion_joint_0_position.inc"
};
static NJS_MKEY_F bar_fly_back_motion_joint_2_position[2] = {
#include "stg03/player_motions/bar_fly_back_motion_joint_2_position.inc"
};
static NJS_MKEY_A bar_fly_back_motion_joint_4_angles[2] = {
#include "stg03/player_motions/bar_fly_back_motion_joint_4_angles.inc"
};
static NJS_MKEY_A bar_fly_back_motion_joint_7_angles[2] = {
#include "stg03/player_motions/bar_fly_back_motion_joint_7_angles.inc"
};
static NJS_MKEY_A bar_fly_back_motion_joint_8_angles[2] = {
#include "stg03/player_motions/bar_fly_back_motion_joint_8_angles.inc"
};
static NJS_MKEY_A bar_fly_back_motion_joint_9_angles[2] = {
#include "stg03/player_motions/bar_fly_back_motion_joint_9_angles.inc"
};
static NJS_MKEY_A bar_fly_back_motion_joint_13_angles[2] = {
#include "stg03/player_motions/bar_fly_back_motion_joint_13_angles.inc"
};
static NJS_MKEY_A bar_fly_back_motion_joint_14_angles[2] = {
#include "stg03/player_motions/bar_fly_back_motion_joint_14_angles.inc"
};
static NJS_MKEY_A bar_fly_back_motion_joint_17_angles[2] = {
#include "stg03/player_motions/bar_fly_back_motion_joint_17_angles.inc"
};
static NJS_MKEY_A bar_fly_back_motion_joint_18_angles[2] = {
#include "stg03/player_motions/bar_fly_back_motion_joint_18_angles.inc"
};
static NJS_MKEY_A bar_fly_back_motion_joint_19_angles[2] = {
#include "stg03/player_motions/bar_fly_back_motion_joint_19_angles.inc"
};
static NJS_MKEY_A bar_fly_back_motion_joint_23_angles[2] = {
#include "stg03/player_motions/bar_fly_back_motion_joint_23_angles.inc"
};
static NJS_MKEY_A bar_fly_back_motion_joint_24_angles[2] = {
#include "stg03/player_motions/bar_fly_back_motion_joint_24_angles.inc"
};
static NJS_MKEY_A bar_fly_back_motion_joint_27_angles[2] = {
#include "stg03/player_motions/bar_fly_back_motion_joint_27_angles.inc"
};
static NJS_MKEY_A bar_fly_back_motion_joint_32_angles[2] = {
#include "stg03/player_motions/bar_fly_back_motion_joint_32_angles.inc"
};
static NJS_MKEY_A bar_fly_back_motion_joint_37_angles[2] = {
#include "stg03/player_motions/bar_fly_back_motion_joint_37_angles.inc"
};
static NJS_MKEY_A bar_fly_back_motion_joint_38_angles[2] = {
#include "stg03/player_motions/bar_fly_back_motion_joint_38_angles.inc"
};
static NJS_MKEY_A bar_fly_back_motion_joint_39_angles[2] = {
#include "stg03/player_motions/bar_fly_back_motion_joint_39_angles.inc"
};
static NJS_MKEY_A bar_fly_back_motion_joint_41_angles[2] = {
#include "stg03/player_motions/bar_fly_back_motion_joint_41_angles.inc"
};
static NJS_MKEY_A bar_fly_back_motion_joint_44_angles[2] = {
#include "stg03/player_motions/bar_fly_back_motion_joint_44_angles.inc"
};
static NJS_MKEY_A bar_fly_back_motion_joint_51_angles[2] = {
#include "stg03/player_motions/bar_fly_back_motion_joint_51_angles.inc"
};
static NJS_MKEY_A bar_fly_back_motion_joint_53_angles[2] = {
#include "stg03/player_motions/bar_fly_back_motion_joint_53_angles.inc"
};
static NJS_MKEY_A bar_fly_back_motion_joint_56_angles[6] = {
#include "stg03/player_motions/bar_fly_back_motion_joint_56_angles.inc"
};
static NJS_MKEY_A bar_fly_back_motion_joint_57_angles[2] = {
#include "stg03/player_motions/bar_fly_back_motion_joint_57_angles.inc"
};
static NJS_MKEY_A bar_fly_back_motion_joint_58_angles[3] = {
#include "stg03/player_motions/bar_fly_back_motion_joint_58_angles.inc"
};
static NJS_MKEY_A bar_fly_back_motion_joint_60_angles[2] = {
#include "stg03/player_motions/bar_fly_back_motion_joint_60_angles.inc"
};
static NJS_MDATA2 bar_fly_back_motion_tracks[62] = {
#include "stg03/player_motions/bar_fly_back_motion_tracks.inc"
};
static NJS_MOTION bar_fly_back_motion = {
#include "stg03/player_motions/bar_fly_back_motion.inc"
};
static NJS_MKEY_F bar_fly_motion_joint_0_position[2] = {
#include "stg03/player_motions/bar_fly_motion_joint_0_position.inc"
};
static NJS_MKEY_F bar_fly_motion_joint_2_position[2] = {
#include "stg03/player_motions/bar_fly_motion_joint_2_position.inc"
};
static NJS_MKEY_A bar_fly_motion_joint_4_angles[2] = {
#include "stg03/player_motions/bar_fly_motion_joint_4_angles.inc"
};
static NJS_MKEY_A bar_fly_motion_joint_7_angles[2] = {
#include "stg03/player_motions/bar_fly_motion_joint_7_angles.inc"
};
static NJS_MKEY_A bar_fly_motion_joint_8_angles[2] = {
#include "stg03/player_motions/bar_fly_motion_joint_8_angles.inc"
};
static NJS_MKEY_A bar_fly_motion_joint_9_angles[2] = {
#include "stg03/player_motions/bar_fly_motion_joint_9_angles.inc"
};
static NJS_MKEY_A bar_fly_motion_joint_13_angles[2] = {
#include "stg03/player_motions/bar_fly_motion_joint_13_angles.inc"
};
static NJS_MKEY_A bar_fly_motion_joint_14_angles[2] = {
#include "stg03/player_motions/bar_fly_motion_joint_14_angles.inc"
};
static NJS_MKEY_A bar_fly_motion_joint_17_angles[2] = {
#include "stg03/player_motions/bar_fly_motion_joint_17_angles.inc"
};
static NJS_MKEY_A bar_fly_motion_joint_18_angles[2] = {
#include "stg03/player_motions/bar_fly_motion_joint_18_angles.inc"
};
static NJS_MKEY_A bar_fly_motion_joint_19_angles[2] = {
#include "stg03/player_motions/bar_fly_motion_joint_19_angles.inc"
};
static NJS_MKEY_A bar_fly_motion_joint_23_angles[2] = {
#include "stg03/player_motions/bar_fly_motion_joint_23_angles.inc"
};
static NJS_MKEY_A bar_fly_motion_joint_24_angles[2] = {
#include "stg03/player_motions/bar_fly_motion_joint_24_angles.inc"
};
static NJS_MKEY_A bar_fly_motion_joint_27_angles[2] = {
#include "stg03/player_motions/bar_fly_motion_joint_27_angles.inc"
};
static NJS_MKEY_A bar_fly_motion_joint_32_angles[2] = {
#include "stg03/player_motions/bar_fly_motion_joint_32_angles.inc"
};
static NJS_MKEY_A bar_fly_motion_joint_37_angles[2] = {
#include "stg03/player_motions/bar_fly_motion_joint_37_angles.inc"
};
static NJS_MKEY_A bar_fly_motion_joint_38_angles[2] = {
#include "stg03/player_motions/bar_fly_motion_joint_38_angles.inc"
};
static NJS_MKEY_A bar_fly_motion_joint_39_angles[2] = {
#include "stg03/player_motions/bar_fly_motion_joint_39_angles.inc"
};
static NJS_MKEY_A bar_fly_motion_joint_41_angles[2] = {
#include "stg03/player_motions/bar_fly_motion_joint_41_angles.inc"
};
static NJS_MKEY_A bar_fly_motion_joint_44_angles[2] = {
#include "stg03/player_motions/bar_fly_motion_joint_44_angles.inc"
};
static NJS_MKEY_A bar_fly_motion_joint_51_angles[2] = {
#include "stg03/player_motions/bar_fly_motion_joint_51_angles.inc"
};
static NJS_MKEY_A bar_fly_motion_joint_53_angles[2] = {
#include "stg03/player_motions/bar_fly_motion_joint_53_angles.inc"
};
static NJS_MKEY_A bar_fly_motion_joint_56_angles[3] = {
#include "stg03/player_motions/bar_fly_motion_joint_56_angles.inc"
};
static NJS_MKEY_A bar_fly_motion_joint_57_angles[2] = {
#include "stg03/player_motions/bar_fly_motion_joint_57_angles.inc"
};
static NJS_MKEY_A bar_fly_motion_joint_58_angles[2] = {
#include "stg03/player_motions/bar_fly_motion_joint_58_angles.inc"
};
static NJS_MKEY_A bar_fly_motion_joint_60_angles[2] = {
#include "stg03/player_motions/bar_fly_motion_joint_60_angles.inc"
};
static NJS_MDATA2 bar_fly_motion_tracks[62] = {
#include "stg03/player_motions/bar_fly_motion_tracks.inc"
};
static NJS_MOTION bar_fly_motion = {
#include "stg03/player_motions/bar_fly_motion.inc"
};
static NJS_MKEY_F bar_land_motion_joint_2_position[9] = {
#include "stg03/player_motions/bar_land_motion_joint_2_position.inc"
};
static NJS_MKEY_A bar_land_motion_joint_2_angles[2] = {
#include "stg03/player_motions/bar_land_motion_joint_2_angles.inc"
};
static NJS_MKEY_A bar_land_motion_joint_4_angles[2] = {
#include "stg03/player_motions/bar_land_motion_joint_4_angles.inc"
};
static NJS_MKEY_A bar_land_motion_joint_7_angles[9] = {
#include "stg03/player_motions/bar_land_motion_joint_7_angles.inc"
};
static NJS_MKEY_A bar_land_motion_joint_8_angles[9] = {
#include "stg03/player_motions/bar_land_motion_joint_8_angles.inc"
};
static NJS_MKEY_A bar_land_motion_joint_9_angles[9] = {
#include "stg03/player_motions/bar_land_motion_joint_9_angles.inc"
};
static NJS_MKEY_A bar_land_motion_joint_13_angles[9] = {
#include "stg03/player_motions/bar_land_motion_joint_13_angles.inc"
};
static NJS_MKEY_A bar_land_motion_joint_14_angles[2] = {
#include "stg03/player_motions/bar_land_motion_joint_14_angles.inc"
};
static NJS_MKEY_A bar_land_motion_joint_17_angles[9] = {
#include "stg03/player_motions/bar_land_motion_joint_17_angles.inc"
};
static NJS_MKEY_A bar_land_motion_joint_18_angles[9] = {
#include "stg03/player_motions/bar_land_motion_joint_18_angles.inc"
};
static NJS_MKEY_A bar_land_motion_joint_19_angles[9] = {
#include "stg03/player_motions/bar_land_motion_joint_19_angles.inc"
};
static NJS_MKEY_A bar_land_motion_joint_23_angles[9] = {
#include "stg03/player_motions/bar_land_motion_joint_23_angles.inc"
};
static NJS_MKEY_A bar_land_motion_joint_24_angles[4] = {
#include "stg03/player_motions/bar_land_motion_joint_24_angles.inc"
};
static NJS_MKEY_A bar_land_motion_joint_27_angles[9] = {
#include "stg03/player_motions/bar_land_motion_joint_27_angles.inc"
};
static NJS_MKEY_A bar_land_motion_joint_28_angles[9] = {
#include "stg03/player_motions/bar_land_motion_joint_28_angles.inc"
};
static NJS_MKEY_A bar_land_motion_joint_29_angles[9] = {
#include "stg03/player_motions/bar_land_motion_joint_29_angles.inc"
};
static NJS_MKEY_A bar_land_motion_joint_32_angles[6] = {
#include "stg03/player_motions/bar_land_motion_joint_32_angles.inc"
};
static NJS_MKEY_A bar_land_motion_joint_33_angles[6] = {
#include "stg03/player_motions/bar_land_motion_joint_33_angles.inc"
};
static NJS_MKEY_A bar_land_motion_joint_37_angles[5] = {
#include "stg03/player_motions/bar_land_motion_joint_37_angles.inc"
};
static NJS_MKEY_A bar_land_motion_joint_38_angles[5] = {
#include "stg03/player_motions/bar_land_motion_joint_38_angles.inc"
};
static NJS_MKEY_A bar_land_motion_joint_39_angles[5] = {
#include "stg03/player_motions/bar_land_motion_joint_39_angles.inc"
};
static NJS_MKEY_A bar_land_motion_joint_41_angles[9] = {
#include "stg03/player_motions/bar_land_motion_joint_41_angles.inc"
};
static NJS_MKEY_A bar_land_motion_joint_44_angles[9] = {
#include "stg03/player_motions/bar_land_motion_joint_44_angles.inc"
};
static NJS_MKEY_A bar_land_motion_joint_51_angles[9] = {
#include "stg03/player_motions/bar_land_motion_joint_51_angles.inc"
};
static NJS_MKEY_A bar_land_motion_joint_53_angles[9] = {
#include "stg03/player_motions/bar_land_motion_joint_53_angles.inc"
};
static NJS_MKEY_A bar_land_motion_joint_56_angles[8] = {
#include "stg03/player_motions/bar_land_motion_joint_56_angles.inc"
};
static NJS_MKEY_A bar_land_motion_joint_57_angles[5] = {
#include "stg03/player_motions/bar_land_motion_joint_57_angles.inc"
};
static NJS_MKEY_A bar_land_motion_joint_58_angles[5] = {
#include "stg03/player_motions/bar_land_motion_joint_58_angles.inc"
};
static NJS_MKEY_A bar_land_motion_joint_60_angles[9] = {
#include "stg03/player_motions/bar_land_motion_joint_60_angles.inc"
};
static NJS_MDATA2 bar_land_motion_tracks[62] = {
#include "stg03/player_motions/bar_land_motion_tracks.inc"
};
static NJS_MOTION bar_land_motion = {
#include "stg03/player_motions/bar_land_motion.inc"
};
struct motion_entry module_motions[12] = {
#include "stg03/player_motions/module_motions.inc"
};
