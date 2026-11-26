/* STG04.PRS's motions and their registry, from the disc. its D lies between
 * objects.c and slgate.c */
#include <ninja.h>

#include "player/player.h"
#include "shared/motions.h"

static NJS_MKEY_A upgrade_white_jungle_motion_joint_4_angles[16] = {
#include "stg04/player_motions/upgrade_white_jungle_motion_joint_4_angles.inc"
};
static NJS_MKEY_A upgrade_white_jungle_motion_joint_7_angles[16] = {
#include "stg04/player_motions/upgrade_white_jungle_motion_joint_7_angles.inc"
};
static NJS_MKEY_A upgrade_white_jungle_motion_joint_8_angles[10] = {
#include "stg04/player_motions/upgrade_white_jungle_motion_joint_8_angles.inc"
};
static NJS_MKEY_A upgrade_white_jungle_motion_joint_9_angles[14] = {
#include "stg04/player_motions/upgrade_white_jungle_motion_joint_9_angles.inc"
};
static NJS_MKEY_A upgrade_white_jungle_motion_joint_13_angles[16] = {
#include "stg04/player_motions/upgrade_white_jungle_motion_joint_13_angles.inc"
};
static NJS_MKEY_A upgrade_white_jungle_motion_joint_14_angles[2] = {
#include "stg04/player_motions/upgrade_white_jungle_motion_joint_14_angles.inc"
};
static NJS_MKEY_A upgrade_white_jungle_motion_joint_17_angles[16] = {
#include "stg04/player_motions/upgrade_white_jungle_motion_joint_17_angles.inc"
};
static NJS_MKEY_A upgrade_white_jungle_motion_joint_18_angles[16] = {
#include "stg04/player_motions/upgrade_white_jungle_motion_joint_18_angles.inc"
};
static NJS_MKEY_A upgrade_white_jungle_motion_joint_19_angles[16] = {
#include "stg04/player_motions/upgrade_white_jungle_motion_joint_19_angles.inc"
};
static NJS_MKEY_A upgrade_white_jungle_motion_joint_23_angles[16] = {
#include "stg04/player_motions/upgrade_white_jungle_motion_joint_23_angles.inc"
};
static NJS_MKEY_A upgrade_white_jungle_motion_joint_24_angles[16] = {
#include "stg04/player_motions/upgrade_white_jungle_motion_joint_24_angles.inc"
};
static NJS_MKEY_A upgrade_white_jungle_motion_joint_27_angles[16] = {
#include "stg04/player_motions/upgrade_white_jungle_motion_joint_27_angles.inc"
};
static NJS_MKEY_A upgrade_white_jungle_motion_joint_28_angles[16] = {
#include "stg04/player_motions/upgrade_white_jungle_motion_joint_28_angles.inc"
};
static NJS_MKEY_A upgrade_white_jungle_motion_joint_29_angles[16] = {
#include "stg04/player_motions/upgrade_white_jungle_motion_joint_29_angles.inc"
};
static NJS_MKEY_A upgrade_white_jungle_motion_joint_31_angles[2] = {
#include "stg04/player_motions/upgrade_white_jungle_motion_joint_31_angles.inc"
};
static NJS_MKEY_A upgrade_white_jungle_motion_joint_32_angles[16] = {
#include "stg04/player_motions/upgrade_white_jungle_motion_joint_32_angles.inc"
};
static NJS_MKEY_A upgrade_white_jungle_motion_joint_33_angles[10] = {
#include "stg04/player_motions/upgrade_white_jungle_motion_joint_33_angles.inc"
};
static NJS_MKEY_A upgrade_white_jungle_motion_joint_37_angles[16] = {
#include "stg04/player_motions/upgrade_white_jungle_motion_joint_37_angles.inc"
};
static NJS_MKEY_A upgrade_white_jungle_motion_joint_38_angles[16] = {
#include "stg04/player_motions/upgrade_white_jungle_motion_joint_38_angles.inc"
};
static NJS_MKEY_A upgrade_white_jungle_motion_joint_39_angles[16] = {
#include "stg04/player_motions/upgrade_white_jungle_motion_joint_39_angles.inc"
};
static NJS_MKEY_A upgrade_white_jungle_motion_joint_41_angles[16] = {
#include "stg04/player_motions/upgrade_white_jungle_motion_joint_41_angles.inc"
};
static NJS_MKEY_A upgrade_white_jungle_motion_joint_44_angles[16] = {
#include "stg04/player_motions/upgrade_white_jungle_motion_joint_44_angles.inc"
};
static NJS_MKEY_A upgrade_white_jungle_motion_joint_51_angles[16] = {
#include "stg04/player_motions/upgrade_white_jungle_motion_joint_51_angles.inc"
};
static NJS_MKEY_A upgrade_white_jungle_motion_joint_53_angles[16] = {
#include "stg04/player_motions/upgrade_white_jungle_motion_joint_53_angles.inc"
};
static NJS_MKEY_A upgrade_white_jungle_motion_joint_56_angles[16] = {
#include "stg04/player_motions/upgrade_white_jungle_motion_joint_56_angles.inc"
};
static NJS_MKEY_A upgrade_white_jungle_motion_joint_57_angles[16] = {
#include "stg04/player_motions/upgrade_white_jungle_motion_joint_57_angles.inc"
};
static NJS_MKEY_A upgrade_white_jungle_motion_joint_58_angles[16] = {
#include "stg04/player_motions/upgrade_white_jungle_motion_joint_58_angles.inc"
};
static NJS_MKEY_A upgrade_white_jungle_motion_joint_60_angles[16] = {
#include "stg04/player_motions/upgrade_white_jungle_motion_joint_60_angles.inc"
};
static NJS_MDATA2 upgrade_white_jungle_motion_tracks[62] = {
#include "stg04/player_motions/upgrade_white_jungle_motion_tracks.inc"
};
static NJS_MOTION upgrade_white_jungle_motion = {
#include "stg04/player_motions/upgrade_white_jungle_motion.inc"
};
static NJS_MKEY_F vine_catch_motion_joint_2_position[8] = {
#include "stg04/player_motions/vine_catch_motion_joint_2_position.inc"
};
static NJS_MKEY_A vine_catch_motion_joint_4_angles[8] = {
#include "stg04/player_motions/vine_catch_motion_joint_4_angles.inc"
};
static NJS_MKEY_A vine_catch_motion_joint_7_angles[8] = {
#include "stg04/player_motions/vine_catch_motion_joint_7_angles.inc"
};
static NJS_MKEY_A vine_catch_motion_joint_8_angles[7] = {
#include "stg04/player_motions/vine_catch_motion_joint_8_angles.inc"
};
static NJS_MKEY_A vine_catch_motion_joint_9_angles[7] = {
#include "stg04/player_motions/vine_catch_motion_joint_9_angles.inc"
};
static NJS_MKEY_A vine_catch_motion_joint_13_angles[8] = {
#include "stg04/player_motions/vine_catch_motion_joint_13_angles.inc"
};
static NJS_MKEY_A vine_catch_motion_joint_14_angles[8] = {
#include "stg04/player_motions/vine_catch_motion_joint_14_angles.inc"
};
static NJS_MKEY_A vine_catch_motion_joint_17_angles[8] = {
#include "stg04/player_motions/vine_catch_motion_joint_17_angles.inc"
};
static NJS_MKEY_A vine_catch_motion_joint_18_angles[8] = {
#include "stg04/player_motions/vine_catch_motion_joint_18_angles.inc"
};
static NJS_MKEY_A vine_catch_motion_joint_19_angles[8] = {
#include "stg04/player_motions/vine_catch_motion_joint_19_angles.inc"
};
static NJS_MKEY_A vine_catch_motion_joint_23_angles[8] = {
#include "stg04/player_motions/vine_catch_motion_joint_23_angles.inc"
};
static NJS_MKEY_A vine_catch_motion_joint_24_angles[7] = {
#include "stg04/player_motions/vine_catch_motion_joint_24_angles.inc"
};
static NJS_MKEY_A vine_catch_motion_joint_27_angles[8] = {
#include "stg04/player_motions/vine_catch_motion_joint_27_angles.inc"
};
static NJS_MKEY_A vine_catch_motion_joint_28_angles[4] = {
#include "stg04/player_motions/vine_catch_motion_joint_28_angles.inc"
};
static NJS_MKEY_A vine_catch_motion_joint_29_angles[4] = {
#include "stg04/player_motions/vine_catch_motion_joint_29_angles.inc"
};
static NJS_MKEY_A vine_catch_motion_joint_31_angles[8] = {
#include "stg04/player_motions/vine_catch_motion_joint_31_angles.inc"
};
static NJS_MKEY_A vine_catch_motion_joint_32_angles[7] = {
#include "stg04/player_motions/vine_catch_motion_joint_32_angles.inc"
};
static NJS_MKEY_A vine_catch_motion_joint_33_angles[7] = {
#include "stg04/player_motions/vine_catch_motion_joint_33_angles.inc"
};
static NJS_MKEY_A vine_catch_motion_joint_37_angles[8] = {
#include "stg04/player_motions/vine_catch_motion_joint_37_angles.inc"
};
static NJS_MKEY_A vine_catch_motion_joint_38_angles[8] = {
#include "stg04/player_motions/vine_catch_motion_joint_38_angles.inc"
};
static NJS_MKEY_A vine_catch_motion_joint_39_angles[8] = {
#include "stg04/player_motions/vine_catch_motion_joint_39_angles.inc"
};
static NJS_MKEY_A vine_catch_motion_joint_41_angles[8] = {
#include "stg04/player_motions/vine_catch_motion_joint_41_angles.inc"
};
static NJS_MKEY_A vine_catch_motion_joint_44_angles[8] = {
#include "stg04/player_motions/vine_catch_motion_joint_44_angles.inc"
};
static NJS_MKEY_A vine_catch_motion_joint_51_angles[8] = {
#include "stg04/player_motions/vine_catch_motion_joint_51_angles.inc"
};
static NJS_MKEY_A vine_catch_motion_joint_53_angles[8] = {
#include "stg04/player_motions/vine_catch_motion_joint_53_angles.inc"
};
static NJS_MKEY_A vine_catch_motion_joint_56_angles[8] = {
#include "stg04/player_motions/vine_catch_motion_joint_56_angles.inc"
};
static NJS_MKEY_A vine_catch_motion_joint_57_angles[2] = {
#include "stg04/player_motions/vine_catch_motion_joint_57_angles.inc"
};
static NJS_MKEY_A vine_catch_motion_joint_58_angles[2] = {
#include "stg04/player_motions/vine_catch_motion_joint_58_angles.inc"
};
static NJS_MKEY_A vine_catch_motion_joint_60_angles[8] = {
#include "stg04/player_motions/vine_catch_motion_joint_60_angles.inc"
};
static NJS_MDATA2 vine_catch_motion_tracks[62] = {
#include "stg04/player_motions/vine_catch_motion_tracks.inc"
};
static NJS_MOTION vine_catch_motion = {
#include "stg04/player_motions/vine_catch_motion.inc"
};
static NJS_MKEY_F vine_hang_motion_joint_2_position[2] = {
#include "stg04/player_motions/vine_hang_motion_joint_2_position.inc"
};
static NJS_MKEY_A vine_hang_motion_joint_4_angles[4] = {
#include "stg04/player_motions/vine_hang_motion_joint_4_angles.inc"
};
static NJS_MKEY_A vine_hang_motion_joint_7_angles[4] = {
#include "stg04/player_motions/vine_hang_motion_joint_7_angles.inc"
};
static NJS_MKEY_A vine_hang_motion_joint_8_angles[4] = {
#include "stg04/player_motions/vine_hang_motion_joint_8_angles.inc"
};
static NJS_MKEY_A vine_hang_motion_joint_9_angles[4] = {
#include "stg04/player_motions/vine_hang_motion_joint_9_angles.inc"
};
static NJS_MKEY_A vine_hang_motion_joint_13_angles[4] = {
#include "stg04/player_motions/vine_hang_motion_joint_13_angles.inc"
};
static NJS_MKEY_A vine_hang_motion_joint_14_angles[4] = {
#include "stg04/player_motions/vine_hang_motion_joint_14_angles.inc"
};
static NJS_MKEY_A vine_hang_motion_joint_17_angles[4] = {
#include "stg04/player_motions/vine_hang_motion_joint_17_angles.inc"
};
static NJS_MKEY_A vine_hang_motion_joint_18_angles[3] = {
#include "stg04/player_motions/vine_hang_motion_joint_18_angles.inc"
};
static NJS_MKEY_A vine_hang_motion_joint_19_angles[4] = {
#include "stg04/player_motions/vine_hang_motion_joint_19_angles.inc"
};
static NJS_MKEY_A vine_hang_motion_joint_23_angles[4] = {
#include "stg04/player_motions/vine_hang_motion_joint_23_angles.inc"
};
static NJS_MKEY_A vine_hang_motion_joint_24_angles[4] = {
#include "stg04/player_motions/vine_hang_motion_joint_24_angles.inc"
};
static NJS_MKEY_A vine_hang_motion_joint_27_angles[4] = {
#include "stg04/player_motions/vine_hang_motion_joint_27_angles.inc"
};
static NJS_MKEY_A vine_hang_motion_joint_31_angles[2] = {
#include "stg04/player_motions/vine_hang_motion_joint_31_angles.inc"
};
static NJS_MKEY_A vine_hang_motion_joint_32_angles[4] = {
#include "stg04/player_motions/vine_hang_motion_joint_32_angles.inc"
};
static NJS_MKEY_A vine_hang_motion_joint_33_angles[4] = {
#include "stg04/player_motions/vine_hang_motion_joint_33_angles.inc"
};
static NJS_MKEY_A vine_hang_motion_joint_37_angles[4] = {
#include "stg04/player_motions/vine_hang_motion_joint_37_angles.inc"
};
static NJS_MKEY_A vine_hang_motion_joint_38_angles[4] = {
#include "stg04/player_motions/vine_hang_motion_joint_38_angles.inc"
};
static NJS_MKEY_A vine_hang_motion_joint_39_angles[4] = {
#include "stg04/player_motions/vine_hang_motion_joint_39_angles.inc"
};
static NJS_MKEY_A vine_hang_motion_joint_41_angles[4] = {
#include "stg04/player_motions/vine_hang_motion_joint_41_angles.inc"
};
static NJS_MKEY_A vine_hang_motion_joint_44_angles[4] = {
#include "stg04/player_motions/vine_hang_motion_joint_44_angles.inc"
};
static NJS_MKEY_A vine_hang_motion_joint_51_angles[4] = {
#include "stg04/player_motions/vine_hang_motion_joint_51_angles.inc"
};
static NJS_MKEY_A vine_hang_motion_joint_53_angles[4] = {
#include "stg04/player_motions/vine_hang_motion_joint_53_angles.inc"
};
static NJS_MKEY_A vine_hang_motion_joint_56_angles[4] = {
#include "stg04/player_motions/vine_hang_motion_joint_56_angles.inc"
};
static NJS_MKEY_A vine_hang_motion_joint_57_angles[2] = {
#include "stg04/player_motions/vine_hang_motion_joint_57_angles.inc"
};
static NJS_MKEY_A vine_hang_motion_joint_58_angles[2] = {
#include "stg04/player_motions/vine_hang_motion_joint_58_angles.inc"
};
static NJS_MKEY_A vine_hang_motion_joint_60_angles[4] = {
#include "stg04/player_motions/vine_hang_motion_joint_60_angles.inc"
};
static NJS_MDATA2 vine_hang_motion_tracks[62] = {
#include "stg04/player_motions/vine_hang_motion_tracks.inc"
};
static NJS_MOTION vine_hang_motion = {
#include "stg04/player_motions/vine_hang_motion.inc"
};
static NJS_MKEY_A bungee_ride_motion_joint_4_angles[2] = {
#include "stg04/player_motions/bungee_ride_motion_joint_4_angles.inc"
};
static NJS_MKEY_A bungee_ride_motion_joint_7_angles[17] = {
#include "stg04/player_motions/bungee_ride_motion_joint_7_angles.inc"
};
static NJS_MKEY_A bungee_ride_motion_joint_8_angles[17] = {
#include "stg04/player_motions/bungee_ride_motion_joint_8_angles.inc"
};
static NJS_MKEY_A bungee_ride_motion_joint_9_angles[17] = {
#include "stg04/player_motions/bungee_ride_motion_joint_9_angles.inc"
};
static NJS_MKEY_A bungee_ride_motion_joint_13_angles[17] = {
#include "stg04/player_motions/bungee_ride_motion_joint_13_angles.inc"
};
static NJS_MKEY_A bungee_ride_motion_joint_14_angles[17] = {
#include "stg04/player_motions/bungee_ride_motion_joint_14_angles.inc"
};
static NJS_MKEY_A bungee_ride_motion_joint_17_angles[17] = {
#include "stg04/player_motions/bungee_ride_motion_joint_17_angles.inc"
};
static NJS_MKEY_A bungee_ride_motion_joint_18_angles[17] = {
#include "stg04/player_motions/bungee_ride_motion_joint_18_angles.inc"
};
static NJS_MKEY_A bungee_ride_motion_joint_19_angles[17] = {
#include "stg04/player_motions/bungee_ride_motion_joint_19_angles.inc"
};
static NJS_MKEY_A bungee_ride_motion_joint_23_angles[17] = {
#include "stg04/player_motions/bungee_ride_motion_joint_23_angles.inc"
};
static NJS_MKEY_A bungee_ride_motion_joint_24_angles[17] = {
#include "stg04/player_motions/bungee_ride_motion_joint_24_angles.inc"
};
static NJS_MKEY_A bungee_ride_motion_joint_27_angles[17] = {
#include "stg04/player_motions/bungee_ride_motion_joint_27_angles.inc"
};
static NJS_MKEY_A bungee_ride_motion_joint_31_angles[17] = {
#include "stg04/player_motions/bungee_ride_motion_joint_31_angles.inc"
};
static NJS_MKEY_A bungee_ride_motion_joint_32_angles[17] = {
#include "stg04/player_motions/bungee_ride_motion_joint_32_angles.inc"
};
static NJS_MKEY_A bungee_ride_motion_joint_33_angles[12] = {
#include "stg04/player_motions/bungee_ride_motion_joint_33_angles.inc"
};
static NJS_MKEY_A bungee_ride_motion_joint_37_angles[17] = {
#include "stg04/player_motions/bungee_ride_motion_joint_37_angles.inc"
};
static NJS_MKEY_A bungee_ride_motion_joint_38_angles[16] = {
#include "stg04/player_motions/bungee_ride_motion_joint_38_angles.inc"
};
static NJS_MKEY_A bungee_ride_motion_joint_39_angles[16] = {
#include "stg04/player_motions/bungee_ride_motion_joint_39_angles.inc"
};
static NJS_MKEY_A bungee_ride_motion_joint_41_angles[17] = {
#include "stg04/player_motions/bungee_ride_motion_joint_41_angles.inc"
};
static NJS_MKEY_A bungee_ride_motion_joint_44_angles[17] = {
#include "stg04/player_motions/bungee_ride_motion_joint_44_angles.inc"
};
static NJS_MKEY_A bungee_ride_motion_joint_51_angles[17] = {
#include "stg04/player_motions/bungee_ride_motion_joint_51_angles.inc"
};
static NJS_MKEY_A bungee_ride_motion_joint_53_angles[17] = {
#include "stg04/player_motions/bungee_ride_motion_joint_53_angles.inc"
};
static NJS_MKEY_A bungee_ride_motion_joint_56_angles[17] = {
#include "stg04/player_motions/bungee_ride_motion_joint_56_angles.inc"
};
static NJS_MKEY_A bungee_ride_motion_joint_57_angles[2] = {
#include "stg04/player_motions/bungee_ride_motion_joint_57_angles.inc"
};
static NJS_MKEY_A bungee_ride_motion_joint_58_angles[2] = {
#include "stg04/player_motions/bungee_ride_motion_joint_58_angles.inc"
};
static NJS_MKEY_A bungee_ride_motion_joint_60_angles[17] = {
#include "stg04/player_motions/bungee_ride_motion_joint_60_angles.inc"
};
static NJS_MDATA2 bungee_ride_motion_tracks[62] = {
#include "stg04/player_motions/bungee_ride_motion_tracks.inc"
};
static NJS_MOTION bungee_ride_motion = {
#include "stg04/player_motions/bungee_ride_motion.inc"
};
static NJS_MKEY_A bungee_ride_alternate_motion_joint_4_angles[16] = {
#include "stg04/player_motions/bungee_ride_alternate_motion_joint_4_angles.inc"
};
static NJS_MKEY_A bungee_ride_alternate_motion_joint_7_angles[16] = {
#include "stg04/player_motions/bungee_ride_alternate_motion_joint_7_angles.inc"
};
static NJS_MKEY_A bungee_ride_alternate_motion_joint_8_angles[16] = {
#include "stg04/player_motions/bungee_ride_alternate_motion_joint_8_angles.inc"
};
static NJS_MKEY_A bungee_ride_alternate_motion_joint_9_angles[16] = {
#include "stg04/player_motions/bungee_ride_alternate_motion_joint_9_angles.inc"
};
static NJS_MKEY_A bungee_ride_alternate_motion_joint_13_angles[16] = {
#include "stg04/player_motions/bungee_ride_alternate_motion_joint_13_angles.inc"
};
static NJS_MKEY_A bungee_ride_alternate_motion_joint_14_angles[11] = {
#include "stg04/player_motions/bungee_ride_alternate_motion_joint_14_angles.inc"
};
static NJS_MKEY_A bungee_ride_alternate_motion_joint_17_angles[16] = {
#include "stg04/player_motions/bungee_ride_alternate_motion_joint_17_angles.inc"
};
static NJS_MKEY_A bungee_ride_alternate_motion_joint_18_angles[2] = {
#include "stg04/player_motions/bungee_ride_alternate_motion_joint_18_angles.inc"
};
static NJS_MKEY_A bungee_ride_alternate_motion_joint_19_angles[2] = {
#include "stg04/player_motions/bungee_ride_alternate_motion_joint_19_angles.inc"
};
static NJS_MKEY_A bungee_ride_alternate_motion_joint_23_angles[16] = {
#include "stg04/player_motions/bungee_ride_alternate_motion_joint_23_angles.inc"
};
static NJS_MKEY_A bungee_ride_alternate_motion_joint_24_angles[15] = {
#include "stg04/player_motions/bungee_ride_alternate_motion_joint_24_angles.inc"
};
static NJS_MKEY_A bungee_ride_alternate_motion_joint_27_angles[16] = {
#include "stg04/player_motions/bungee_ride_alternate_motion_joint_27_angles.inc"
};
static NJS_MKEY_A bungee_ride_alternate_motion_joint_28_angles[16] = {
#include "stg04/player_motions/bungee_ride_alternate_motion_joint_28_angles.inc"
};
static NJS_MKEY_A bungee_ride_alternate_motion_joint_29_angles[16] = {
#include "stg04/player_motions/bungee_ride_alternate_motion_joint_29_angles.inc"
};
static NJS_MKEY_A bungee_ride_alternate_motion_joint_31_angles[2] = {
#include "stg04/player_motions/bungee_ride_alternate_motion_joint_31_angles.inc"
};
static NJS_MKEY_A bungee_ride_alternate_motion_joint_32_angles[16] = {
#include "stg04/player_motions/bungee_ride_alternate_motion_joint_32_angles.inc"
};
static NJS_MKEY_A bungee_ride_alternate_motion_joint_33_angles[13] = {
#include "stg04/player_motions/bungee_ride_alternate_motion_joint_33_angles.inc"
};
static NJS_MKEY_A bungee_ride_alternate_motion_joint_37_angles[16] = {
#include "stg04/player_motions/bungee_ride_alternate_motion_joint_37_angles.inc"
};
static NJS_MKEY_A bungee_ride_alternate_motion_joint_38_angles[2] = {
#include "stg04/player_motions/bungee_ride_alternate_motion_joint_38_angles.inc"
};
static NJS_MKEY_A bungee_ride_alternate_motion_joint_39_angles[2] = {
#include "stg04/player_motions/bungee_ride_alternate_motion_joint_39_angles.inc"
};
static NJS_MKEY_A bungee_ride_alternate_motion_joint_41_angles[16] = {
#include "stg04/player_motions/bungee_ride_alternate_motion_joint_41_angles.inc"
};
static NJS_MKEY_A bungee_ride_alternate_motion_joint_44_angles[16] = {
#include "stg04/player_motions/bungee_ride_alternate_motion_joint_44_angles.inc"
};
static NJS_MKEY_A bungee_ride_alternate_motion_joint_51_angles[16] = {
#include "stg04/player_motions/bungee_ride_alternate_motion_joint_51_angles.inc"
};
static NJS_MKEY_A bungee_ride_alternate_motion_joint_53_angles[16] = {
#include "stg04/player_motions/bungee_ride_alternate_motion_joint_53_angles.inc"
};
static NJS_MKEY_A bungee_ride_alternate_motion_joint_56_angles[16] = {
#include "stg04/player_motions/bungee_ride_alternate_motion_joint_56_angles.inc"
};
static NJS_MKEY_A bungee_ride_alternate_motion_joint_57_angles[4] = {
#include "stg04/player_motions/bungee_ride_alternate_motion_joint_57_angles.inc"
};
static NJS_MKEY_A bungee_ride_alternate_motion_joint_58_angles[4] = {
#include "stg04/player_motions/bungee_ride_alternate_motion_joint_58_angles.inc"
};
static NJS_MKEY_A bungee_ride_alternate_motion_joint_60_angles[16] = {
#include "stg04/player_motions/bungee_ride_alternate_motion_joint_60_angles.inc"
};
static NJS_MDATA2 bungee_ride_alternate_motion_tracks[62] = {
#include "stg04/player_motions/bungee_ride_alternate_motion_tracks.inc"
};
static NJS_MOTION bungee_ride_alternate_motion = {
#include "stg04/player_motions/bungee_ride_alternate_motion.inc"
};
static NJS_MKEY_A bungee_catch_motion_joint_4_angles[16] = {
#include "stg04/player_motions/bungee_catch_motion_joint_4_angles.inc"
};
static NJS_MKEY_A bungee_catch_motion_joint_7_angles[16] = {
#include "stg04/player_motions/bungee_catch_motion_joint_7_angles.inc"
};
static NJS_MKEY_A bungee_catch_motion_joint_8_angles[14] = {
#include "stg04/player_motions/bungee_catch_motion_joint_8_angles.inc"
};
static NJS_MKEY_A bungee_catch_motion_joint_9_angles[16] = {
#include "stg04/player_motions/bungee_catch_motion_joint_9_angles.inc"
};
static NJS_MKEY_A bungee_catch_motion_joint_13_angles[16] = {
#include "stg04/player_motions/bungee_catch_motion_joint_13_angles.inc"
};
static NJS_MKEY_A bungee_catch_motion_joint_14_angles[14] = {
#include "stg04/player_motions/bungee_catch_motion_joint_14_angles.inc"
};
static NJS_MKEY_A bungee_catch_motion_joint_17_angles[16] = {
#include "stg04/player_motions/bungee_catch_motion_joint_17_angles.inc"
};
static NJS_MKEY_A bungee_catch_motion_joint_18_angles[16] = {
#include "stg04/player_motions/bungee_catch_motion_joint_18_angles.inc"
};
static NJS_MKEY_A bungee_catch_motion_joint_19_angles[16] = {
#include "stg04/player_motions/bungee_catch_motion_joint_19_angles.inc"
};
static NJS_MKEY_A bungee_catch_motion_joint_23_angles[16] = {
#include "stg04/player_motions/bungee_catch_motion_joint_23_angles.inc"
};
static NJS_MKEY_A bungee_catch_motion_joint_24_angles[16] = {
#include "stg04/player_motions/bungee_catch_motion_joint_24_angles.inc"
};
static NJS_MKEY_A bungee_catch_motion_joint_27_angles[16] = {
#include "stg04/player_motions/bungee_catch_motion_joint_27_angles.inc"
};
static NJS_MKEY_A bungee_catch_motion_joint_31_angles[16] = {
#include "stg04/player_motions/bungee_catch_motion_joint_31_angles.inc"
};
static NJS_MKEY_A bungee_catch_motion_joint_32_angles[16] = {
#include "stg04/player_motions/bungee_catch_motion_joint_32_angles.inc"
};
static NJS_MKEY_A bungee_catch_motion_joint_33_angles[13] = {
#include "stg04/player_motions/bungee_catch_motion_joint_33_angles.inc"
};
static NJS_MKEY_A bungee_catch_motion_joint_37_angles[16] = {
#include "stg04/player_motions/bungee_catch_motion_joint_37_angles.inc"
};
static NJS_MKEY_A bungee_catch_motion_joint_38_angles[2] = {
#include "stg04/player_motions/bungee_catch_motion_joint_38_angles.inc"
};
static NJS_MKEY_A bungee_catch_motion_joint_39_angles[2] = {
#include "stg04/player_motions/bungee_catch_motion_joint_39_angles.inc"
};
static NJS_MKEY_A bungee_catch_motion_joint_41_angles[16] = {
#include "stg04/player_motions/bungee_catch_motion_joint_41_angles.inc"
};
static NJS_MKEY_A bungee_catch_motion_joint_44_angles[16] = {
#include "stg04/player_motions/bungee_catch_motion_joint_44_angles.inc"
};
static NJS_MKEY_A bungee_catch_motion_joint_51_angles[16] = {
#include "stg04/player_motions/bungee_catch_motion_joint_51_angles.inc"
};
static NJS_MKEY_A bungee_catch_motion_joint_53_angles[16] = {
#include "stg04/player_motions/bungee_catch_motion_joint_53_angles.inc"
};
static NJS_MKEY_A bungee_catch_motion_joint_56_angles[16] = {
#include "stg04/player_motions/bungee_catch_motion_joint_56_angles.inc"
};
static NJS_MKEY_A bungee_catch_motion_joint_57_angles[2] = {
#include "stg04/player_motions/bungee_catch_motion_joint_57_angles.inc"
};
static NJS_MKEY_A bungee_catch_motion_joint_58_angles[2] = {
#include "stg04/player_motions/bungee_catch_motion_joint_58_angles.inc"
};
static NJS_MKEY_A bungee_catch_motion_joint_60_angles[16] = {
#include "stg04/player_motions/bungee_catch_motion_joint_60_angles.inc"
};
static NJS_MDATA2 bungee_catch_motion_tracks[62] = {
#include "stg04/player_motions/bungee_catch_motion_tracks.inc"
};
static NJS_MOTION bungee_catch_motion = {
#include "stg04/player_motions/bungee_catch_motion.inc"
};
struct motion_entry module_motions[7] = {
#include "stg04/player_motions/module_motions.inc"
};
