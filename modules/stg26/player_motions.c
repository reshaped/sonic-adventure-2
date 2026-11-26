/* STG26.PRS's motions and their registry, from the disc. its D lies between
 * object_kinds.c and wood_box_e.c */
#include <ninja.h>

#include "player/player.h"
#include "shared/motions.h"

static NJS_MKEY_A upgrade_white_jungle_motion_joint_4_angles[16] = {
#include "stg26/player_motions/upgrade_white_jungle_motion_joint_4_angles.inc"
};
static NJS_MKEY_A upgrade_white_jungle_motion_joint_7_angles[16] = {
#include "stg26/player_motions/upgrade_white_jungle_motion_joint_7_angles.inc"
};
static NJS_MKEY_A upgrade_white_jungle_motion_joint_8_angles[10] = {
#include "stg26/player_motions/upgrade_white_jungle_motion_joint_8_angles.inc"
};
static NJS_MKEY_A upgrade_white_jungle_motion_joint_9_angles[14] = {
#include "stg26/player_motions/upgrade_white_jungle_motion_joint_9_angles.inc"
};
static NJS_MKEY_A upgrade_white_jungle_motion_joint_13_angles[16] = {
#include "stg26/player_motions/upgrade_white_jungle_motion_joint_13_angles.inc"
};
static NJS_MKEY_A upgrade_white_jungle_motion_joint_14_angles[2] = {
#include "stg26/player_motions/upgrade_white_jungle_motion_joint_14_angles.inc"
};
static NJS_MKEY_A upgrade_white_jungle_motion_joint_17_angles[16] = {
#include "stg26/player_motions/upgrade_white_jungle_motion_joint_17_angles.inc"
};
static NJS_MKEY_A upgrade_white_jungle_motion_joint_18_angles[16] = {
#include "stg26/player_motions/upgrade_white_jungle_motion_joint_18_angles.inc"
};
static NJS_MKEY_A upgrade_white_jungle_motion_joint_19_angles[16] = {
#include "stg26/player_motions/upgrade_white_jungle_motion_joint_19_angles.inc"
};
static NJS_MKEY_A upgrade_white_jungle_motion_joint_23_angles[16] = {
#include "stg26/player_motions/upgrade_white_jungle_motion_joint_23_angles.inc"
};
static NJS_MKEY_A upgrade_white_jungle_motion_joint_24_angles[16] = {
#include "stg26/player_motions/upgrade_white_jungle_motion_joint_24_angles.inc"
};
static NJS_MKEY_A upgrade_white_jungle_motion_joint_27_angles[16] = {
#include "stg26/player_motions/upgrade_white_jungle_motion_joint_27_angles.inc"
};
static NJS_MKEY_A upgrade_white_jungle_motion_joint_28_angles[16] = {
#include "stg26/player_motions/upgrade_white_jungle_motion_joint_28_angles.inc"
};
static NJS_MKEY_A upgrade_white_jungle_motion_joint_29_angles[16] = {
#include "stg26/player_motions/upgrade_white_jungle_motion_joint_29_angles.inc"
};
static NJS_MKEY_A upgrade_white_jungle_motion_joint_31_angles[2] = {
#include "stg26/player_motions/upgrade_white_jungle_motion_joint_31_angles.inc"
};
static NJS_MKEY_A upgrade_white_jungle_motion_joint_32_angles[16] = {
#include "stg26/player_motions/upgrade_white_jungle_motion_joint_32_angles.inc"
};
static NJS_MKEY_A upgrade_white_jungle_motion_joint_33_angles[10] = {
#include "stg26/player_motions/upgrade_white_jungle_motion_joint_33_angles.inc"
};
static NJS_MKEY_A upgrade_white_jungle_motion_joint_37_angles[16] = {
#include "stg26/player_motions/upgrade_white_jungle_motion_joint_37_angles.inc"
};
static NJS_MKEY_A upgrade_white_jungle_motion_joint_38_angles[16] = {
#include "stg26/player_motions/upgrade_white_jungle_motion_joint_38_angles.inc"
};
static NJS_MKEY_A upgrade_white_jungle_motion_joint_39_angles[16] = {
#include "stg26/player_motions/upgrade_white_jungle_motion_joint_39_angles.inc"
};
static NJS_MKEY_A upgrade_white_jungle_motion_joint_41_angles[16] = {
#include "stg26/player_motions/upgrade_white_jungle_motion_joint_41_angles.inc"
};
static NJS_MKEY_A upgrade_white_jungle_motion_joint_44_angles[16] = {
#include "stg26/player_motions/upgrade_white_jungle_motion_joint_44_angles.inc"
};
static NJS_MKEY_A upgrade_white_jungle_motion_joint_51_angles[16] = {
#include "stg26/player_motions/upgrade_white_jungle_motion_joint_51_angles.inc"
};
static NJS_MKEY_A upgrade_white_jungle_motion_joint_53_angles[16] = {
#include "stg26/player_motions/upgrade_white_jungle_motion_joint_53_angles.inc"
};
static NJS_MKEY_A upgrade_white_jungle_motion_joint_56_angles[16] = {
#include "stg26/player_motions/upgrade_white_jungle_motion_joint_56_angles.inc"
};
static NJS_MKEY_A upgrade_white_jungle_motion_joint_57_angles[16] = {
#include "stg26/player_motions/upgrade_white_jungle_motion_joint_57_angles.inc"
};
static NJS_MKEY_A upgrade_white_jungle_motion_joint_58_angles[16] = {
#include "stg26/player_motions/upgrade_white_jungle_motion_joint_58_angles.inc"
};
static NJS_MKEY_A upgrade_white_jungle_motion_joint_60_angles[16] = {
#include "stg26/player_motions/upgrade_white_jungle_motion_joint_60_angles.inc"
};
static NJS_MDATA2 upgrade_white_jungle_motion_tracks[62] = {
#include "stg26/player_motions/upgrade_white_jungle_motion_tracks.inc"
};
static NJS_MOTION upgrade_white_jungle_motion = {
#include "stg26/player_motions/upgrade_white_jungle_motion.inc"
};
static NJS_MKEY_F rouge_idle_motion_joint_2_position[2] = {
#include "stg26/player_motions/rouge_idle_motion_joint_2_position.inc"
};
static NJS_MKEY_A rouge_idle_motion_joint_4_angles[2] = {
#include "stg26/player_motions/rouge_idle_motion_joint_4_angles.inc"
};
static NJS_MKEY_A rouge_idle_motion_joint_7_angles[2] = {
#include "stg26/player_motions/rouge_idle_motion_joint_7_angles.inc"
};
static NJS_MKEY_A rouge_idle_motion_joint_8_angles[2] = {
#include "stg26/player_motions/rouge_idle_motion_joint_8_angles.inc"
};
static NJS_MKEY_A rouge_idle_motion_joint_9_angles[2] = {
#include "stg26/player_motions/rouge_idle_motion_joint_9_angles.inc"
};
static NJS_MKEY_A rouge_idle_motion_joint_13_angles[2] = {
#include "stg26/player_motions/rouge_idle_motion_joint_13_angles.inc"
};
static NJS_MKEY_A rouge_idle_motion_joint_14_angles[2] = {
#include "stg26/player_motions/rouge_idle_motion_joint_14_angles.inc"
};
static NJS_MKEY_A rouge_idle_motion_joint_17_angles[2] = {
#include "stg26/player_motions/rouge_idle_motion_joint_17_angles.inc"
};
static NJS_MKEY_A rouge_idle_motion_joint_18_angles[2] = {
#include "stg26/player_motions/rouge_idle_motion_joint_18_angles.inc"
};
static NJS_MKEY_A rouge_idle_motion_joint_19_angles[2] = {
#include "stg26/player_motions/rouge_idle_motion_joint_19_angles.inc"
};
static NJS_MKEY_A rouge_idle_motion_joint_23_angles[2] = {
#include "stg26/player_motions/rouge_idle_motion_joint_23_angles.inc"
};
static NJS_MKEY_A rouge_idle_motion_joint_24_angles[2] = {
#include "stg26/player_motions/rouge_idle_motion_joint_24_angles.inc"
};
static NJS_MKEY_A rouge_idle_motion_joint_27_angles[2] = {
#include "stg26/player_motions/rouge_idle_motion_joint_27_angles.inc"
};
static NJS_MKEY_A rouge_idle_motion_joint_28_angles[2] = {
#include "stg26/player_motions/rouge_idle_motion_joint_28_angles.inc"
};
static NJS_MKEY_A rouge_idle_motion_joint_29_angles[2] = {
#include "stg26/player_motions/rouge_idle_motion_joint_29_angles.inc"
};
static NJS_MKEY_A rouge_idle_motion_joint_31_angles[2] = {
#include "stg26/player_motions/rouge_idle_motion_joint_31_angles.inc"
};
static NJS_MKEY_A rouge_idle_motion_joint_32_angles[33] = {
#include "stg26/player_motions/rouge_idle_motion_joint_32_angles.inc"
};
static NJS_MKEY_A rouge_idle_motion_joint_33_angles[24] = {
#include "stg26/player_motions/rouge_idle_motion_joint_33_angles.inc"
};
static NJS_MKEY_A rouge_idle_motion_joint_37_angles[33] = {
#include "stg26/player_motions/rouge_idle_motion_joint_37_angles.inc"
};
static NJS_MKEY_A rouge_idle_motion_joint_38_angles[28] = {
#include "stg26/player_motions/rouge_idle_motion_joint_38_angles.inc"
};
static NJS_MKEY_A rouge_idle_motion_joint_39_angles[27] = {
#include "stg26/player_motions/rouge_idle_motion_joint_39_angles.inc"
};
static NJS_MKEY_A rouge_idle_motion_joint_41_angles[33] = {
#include "stg26/player_motions/rouge_idle_motion_joint_41_angles.inc"
};
static NJS_MKEY_A rouge_idle_motion_joint_44_angles[33] = {
#include "stg26/player_motions/rouge_idle_motion_joint_44_angles.inc"
};
static NJS_MKEY_A rouge_idle_motion_joint_51_angles[30] = {
#include "stg26/player_motions/rouge_idle_motion_joint_51_angles.inc"
};
static NJS_MKEY_A rouge_idle_motion_joint_53_angles[30] = {
#include "stg26/player_motions/rouge_idle_motion_joint_53_angles.inc"
};
static NJS_MKEY_A rouge_idle_motion_joint_56_angles[33] = {
#include "stg26/player_motions/rouge_idle_motion_joint_56_angles.inc"
};
static NJS_MKEY_A rouge_idle_motion_joint_57_angles[33] = {
#include "stg26/player_motions/rouge_idle_motion_joint_57_angles.inc"
};
static NJS_MKEY_A rouge_idle_motion_joint_58_angles[33] = {
#include "stg26/player_motions/rouge_idle_motion_joint_58_angles.inc"
};
static NJS_MKEY_A rouge_idle_motion_joint_60_angles[33] = {
#include "stg26/player_motions/rouge_idle_motion_joint_60_angles.inc"
};
static NJS_MDATA2 rouge_idle_motion_tracks[62] = {
#include "stg26/player_motions/rouge_idle_motion_tracks.inc"
};
static NJS_MOTION rouge_idle_motion = {
#include "stg26/player_motions/rouge_idle_motion.inc"
};
static NJS_MKEY_A turtle_stand_motion_joint_7_angles[8] = {
#include "stg26/player_motions/turtle_stand_motion_joint_7_angles.inc"
};
static NJS_MKEY_A turtle_stand_motion_joint_8_angles[8] = {
#include "stg26/player_motions/turtle_stand_motion_joint_8_angles.inc"
};
static NJS_MKEY_A turtle_stand_motion_joint_9_angles[8] = {
#include "stg26/player_motions/turtle_stand_motion_joint_9_angles.inc"
};
static NJS_MKEY_A turtle_stand_motion_joint_13_angles[7] = {
#include "stg26/player_motions/turtle_stand_motion_joint_13_angles.inc"
};
static NJS_MKEY_A turtle_stand_motion_joint_17_angles[8] = {
#include "stg26/player_motions/turtle_stand_motion_joint_17_angles.inc"
};
static NJS_MKEY_A turtle_stand_motion_joint_18_angles[8] = {
#include "stg26/player_motions/turtle_stand_motion_joint_18_angles.inc"
};
static NJS_MKEY_A turtle_stand_motion_joint_19_angles[8] = {
#include "stg26/player_motions/turtle_stand_motion_joint_19_angles.inc"
};
static NJS_MKEY_A turtle_stand_motion_joint_23_angles[5] = {
#include "stg26/player_motions/turtle_stand_motion_joint_23_angles.inc"
};
static NJS_MKEY_A turtle_stand_motion_joint_27_angles[8] = {
#include "stg26/player_motions/turtle_stand_motion_joint_27_angles.inc"
};
static NJS_MKEY_A turtle_stand_motion_joint_28_angles[8] = {
#include "stg26/player_motions/turtle_stand_motion_joint_28_angles.inc"
};
static NJS_MKEY_A turtle_stand_motion_joint_29_angles[8] = {
#include "stg26/player_motions/turtle_stand_motion_joint_29_angles.inc"
};
static NJS_MKEY_A turtle_stand_motion_joint_32_angles[8] = {
#include "stg26/player_motions/turtle_stand_motion_joint_32_angles.inc"
};
static NJS_MKEY_A turtle_stand_motion_joint_33_angles[5] = {
#include "stg26/player_motions/turtle_stand_motion_joint_33_angles.inc"
};
static NJS_MKEY_A turtle_stand_motion_joint_37_angles[8] = {
#include "stg26/player_motions/turtle_stand_motion_joint_37_angles.inc"
};
static NJS_MKEY_A turtle_stand_motion_joint_38_angles[8] = {
#include "stg26/player_motions/turtle_stand_motion_joint_38_angles.inc"
};
static NJS_MKEY_A turtle_stand_motion_joint_39_angles[8] = {
#include "stg26/player_motions/turtle_stand_motion_joint_39_angles.inc"
};
static NJS_MKEY_A turtle_stand_motion_joint_41_angles[8] = {
#include "stg26/player_motions/turtle_stand_motion_joint_41_angles.inc"
};
static NJS_MKEY_A turtle_stand_motion_joint_44_angles[8] = {
#include "stg26/player_motions/turtle_stand_motion_joint_44_angles.inc"
};
static NJS_MKEY_A turtle_stand_motion_joint_51_angles[8] = {
#include "stg26/player_motions/turtle_stand_motion_joint_51_angles.inc"
};
static NJS_MKEY_A turtle_stand_motion_joint_53_angles[8] = {
#include "stg26/player_motions/turtle_stand_motion_joint_53_angles.inc"
};
static NJS_MKEY_A turtle_stand_motion_joint_56_angles[8] = {
#include "stg26/player_motions/turtle_stand_motion_joint_56_angles.inc"
};
static NJS_MKEY_A turtle_stand_motion_joint_57_angles[8] = {
#include "stg26/player_motions/turtle_stand_motion_joint_57_angles.inc"
};
static NJS_MKEY_A turtle_stand_motion_joint_58_angles[8] = {
#include "stg26/player_motions/turtle_stand_motion_joint_58_angles.inc"
};
static NJS_MKEY_A turtle_stand_motion_joint_60_angles[8] = {
#include "stg26/player_motions/turtle_stand_motion_joint_60_angles.inc"
};
static NJS_MDATA2 turtle_stand_motion_tracks[62] = {
#include "stg26/player_motions/turtle_stand_motion_tracks.inc"
};
static NJS_MOTION turtle_stand_motion = {
#include "stg26/player_motions/turtle_stand_motion.inc"
};
static NJS_MKEY_A rouge_stopped_action_motion_joint_4_angles[32] = {
#include "stg26/player_motions/rouge_stopped_action_motion_joint_4_angles.inc"
};
static NJS_MKEY_A rouge_stopped_action_motion_joint_7_angles[32] = {
#include "stg26/player_motions/rouge_stopped_action_motion_joint_7_angles.inc"
};
static NJS_MKEY_A rouge_stopped_action_motion_joint_8_angles[32] = {
#include "stg26/player_motions/rouge_stopped_action_motion_joint_8_angles.inc"
};
static NJS_MKEY_A rouge_stopped_action_motion_joint_9_angles[32] = {
#include "stg26/player_motions/rouge_stopped_action_motion_joint_9_angles.inc"
};
static NJS_MKEY_A rouge_stopped_action_motion_joint_13_angles[32] = {
#include "stg26/player_motions/rouge_stopped_action_motion_joint_13_angles.inc"
};
static NJS_MKEY_A rouge_stopped_action_motion_joint_14_angles[26] = {
#include "stg26/player_motions/rouge_stopped_action_motion_joint_14_angles.inc"
};
static NJS_MKEY_A rouge_stopped_action_motion_joint_17_angles[32] = {
#include "stg26/player_motions/rouge_stopped_action_motion_joint_17_angles.inc"
};
static NJS_MKEY_A rouge_stopped_action_motion_joint_18_angles[27] = {
#include "stg26/player_motions/rouge_stopped_action_motion_joint_18_angles.inc"
};
static NJS_MKEY_A rouge_stopped_action_motion_joint_19_angles[32] = {
#include "stg26/player_motions/rouge_stopped_action_motion_joint_19_angles.inc"
};
static NJS_MKEY_A rouge_stopped_action_motion_joint_23_angles[32] = {
#include "stg26/player_motions/rouge_stopped_action_motion_joint_23_angles.inc"
};
static NJS_MKEY_A rouge_stopped_action_motion_joint_24_angles[13] = {
#include "stg26/player_motions/rouge_stopped_action_motion_joint_24_angles.inc"
};
static NJS_MKEY_A rouge_stopped_action_motion_joint_27_angles[32] = {
#include "stg26/player_motions/rouge_stopped_action_motion_joint_27_angles.inc"
};
static NJS_MKEY_A rouge_stopped_action_motion_joint_31_angles[17] = {
#include "stg26/player_motions/rouge_stopped_action_motion_joint_31_angles.inc"
};
static NJS_MKEY_A rouge_stopped_action_motion_joint_32_angles[17] = {
#include "stg26/player_motions/rouge_stopped_action_motion_joint_32_angles.inc"
};
static NJS_MKEY_A rouge_stopped_action_motion_joint_33_angles[12] = {
#include "stg26/player_motions/rouge_stopped_action_motion_joint_33_angles.inc"
};
static NJS_MKEY_A rouge_stopped_action_motion_joint_37_angles[32] = {
#include "stg26/player_motions/rouge_stopped_action_motion_joint_37_angles.inc"
};
static NJS_MKEY_A rouge_stopped_action_motion_joint_38_angles[27] = {
#include "stg26/player_motions/rouge_stopped_action_motion_joint_38_angles.inc"
};
static NJS_MKEY_A rouge_stopped_action_motion_joint_39_angles[27] = {
#include "stg26/player_motions/rouge_stopped_action_motion_joint_39_angles.inc"
};
static NJS_MKEY_A rouge_stopped_action_motion_joint_41_angles[32] = {
#include "stg26/player_motions/rouge_stopped_action_motion_joint_41_angles.inc"
};
static NJS_MKEY_A rouge_stopped_action_motion_joint_44_angles[32] = {
#include "stg26/player_motions/rouge_stopped_action_motion_joint_44_angles.inc"
};
static NJS_MKEY_A rouge_stopped_action_motion_joint_51_angles[32] = {
#include "stg26/player_motions/rouge_stopped_action_motion_joint_51_angles.inc"
};
static NJS_MKEY_A rouge_stopped_action_motion_joint_53_angles[32] = {
#include "stg26/player_motions/rouge_stopped_action_motion_joint_53_angles.inc"
};
static NJS_MKEY_A rouge_stopped_action_motion_joint_56_angles[17] = {
#include "stg26/player_motions/rouge_stopped_action_motion_joint_56_angles.inc"
};
static NJS_MKEY_A rouge_stopped_action_motion_joint_57_angles[17] = {
#include "stg26/player_motions/rouge_stopped_action_motion_joint_57_angles.inc"
};
static NJS_MKEY_A rouge_stopped_action_motion_joint_58_angles[17] = {
#include "stg26/player_motions/rouge_stopped_action_motion_joint_58_angles.inc"
};
static NJS_MKEY_A rouge_stopped_action_motion_joint_60_angles[17] = {
#include "stg26/player_motions/rouge_stopped_action_motion_joint_60_angles.inc"
};
static NJS_MDATA2 rouge_stopped_action_motion_tracks[62] = {
#include "stg26/player_motions/rouge_stopped_action_motion_tracks.inc"
};
static NJS_MOTION rouge_stopped_action_motion = {
#include "stg26/player_motions/rouge_stopped_action_motion.inc"
};
static NJS_MKEY_F turtle_hop_motion_joint_2_position[16] = {
#include "stg26/player_motions/turtle_hop_motion_joint_2_position.inc"
};
static NJS_MKEY_A turtle_hop_motion_joint_4_angles[16] = {
#include "stg26/player_motions/turtle_hop_motion_joint_4_angles.inc"
};
static NJS_MKEY_A turtle_hop_motion_joint_7_angles[16] = {
#include "stg26/player_motions/turtle_hop_motion_joint_7_angles.inc"
};
static NJS_MKEY_A turtle_hop_motion_joint_8_angles[16] = {
#include "stg26/player_motions/turtle_hop_motion_joint_8_angles.inc"
};
static NJS_MKEY_A turtle_hop_motion_joint_9_angles[16] = {
#include "stg26/player_motions/turtle_hop_motion_joint_9_angles.inc"
};
static NJS_MKEY_A turtle_hop_motion_joint_13_angles[16] = {
#include "stg26/player_motions/turtle_hop_motion_joint_13_angles.inc"
};
static NJS_MKEY_A turtle_hop_motion_joint_17_angles[16] = {
#include "stg26/player_motions/turtle_hop_motion_joint_17_angles.inc"
};
static NJS_MKEY_A turtle_hop_motion_joint_18_angles[16] = {
#include "stg26/player_motions/turtle_hop_motion_joint_18_angles.inc"
};
static NJS_MKEY_A turtle_hop_motion_joint_19_angles[16] = {
#include "stg26/player_motions/turtle_hop_motion_joint_19_angles.inc"
};
static NJS_MKEY_A turtle_hop_motion_joint_23_angles[16] = {
#include "stg26/player_motions/turtle_hop_motion_joint_23_angles.inc"
};
static NJS_MKEY_A turtle_hop_motion_joint_24_angles[16] = {
#include "stg26/player_motions/turtle_hop_motion_joint_24_angles.inc"
};
static NJS_MKEY_A turtle_hop_motion_joint_27_angles[16] = {
#include "stg26/player_motions/turtle_hop_motion_joint_27_angles.inc"
};
static NJS_MKEY_A turtle_hop_motion_joint_31_angles[16] = {
#include "stg26/player_motions/turtle_hop_motion_joint_31_angles.inc"
};
static NJS_MKEY_A turtle_hop_motion_joint_32_angles[16] = {
#include "stg26/player_motions/turtle_hop_motion_joint_32_angles.inc"
};
static NJS_MKEY_A turtle_hop_motion_joint_33_angles[11] = {
#include "stg26/player_motions/turtle_hop_motion_joint_33_angles.inc"
};
static NJS_MKEY_A turtle_hop_motion_joint_37_angles[16] = {
#include "stg26/player_motions/turtle_hop_motion_joint_37_angles.inc"
};
static NJS_MKEY_A turtle_hop_motion_joint_38_angles[13] = {
#include "stg26/player_motions/turtle_hop_motion_joint_38_angles.inc"
};
static NJS_MKEY_A turtle_hop_motion_joint_39_angles[13] = {
#include "stg26/player_motions/turtle_hop_motion_joint_39_angles.inc"
};
static NJS_MKEY_A turtle_hop_motion_joint_41_angles[16] = {
#include "stg26/player_motions/turtle_hop_motion_joint_41_angles.inc"
};
static NJS_MKEY_A turtle_hop_motion_joint_44_angles[16] = {
#include "stg26/player_motions/turtle_hop_motion_joint_44_angles.inc"
};
static NJS_MKEY_A turtle_hop_motion_joint_51_angles[16] = {
#include "stg26/player_motions/turtle_hop_motion_joint_51_angles.inc"
};
static NJS_MKEY_A turtle_hop_motion_joint_53_angles[16] = {
#include "stg26/player_motions/turtle_hop_motion_joint_53_angles.inc"
};
static NJS_MKEY_A turtle_hop_motion_joint_56_angles[16] = {
#include "stg26/player_motions/turtle_hop_motion_joint_56_angles.inc"
};
static NJS_MKEY_A turtle_hop_motion_joint_57_angles[16] = {
#include "stg26/player_motions/turtle_hop_motion_joint_57_angles.inc"
};
static NJS_MKEY_A turtle_hop_motion_joint_58_angles[16] = {
#include "stg26/player_motions/turtle_hop_motion_joint_58_angles.inc"
};
static NJS_MKEY_A turtle_hop_motion_joint_60_angles[16] = {
#include "stg26/player_motions/turtle_hop_motion_joint_60_angles.inc"
};
static NJS_MDATA2 turtle_hop_motion_tracks[62] = {
#include "stg26/player_motions/turtle_hop_motion_tracks.inc"
};
static NJS_MOTION turtle_hop_motion = {
#include "stg26/player_motions/turtle_hop_motion.inc"
};
static NJS_MKEY_F rouge_float_motion_joint_2_position[8] = {
#include "stg26/player_motions/rouge_float_motion_joint_2_position.inc"
};
static NJS_MKEY_A rouge_float_motion_joint_4_angles[8] = {
#include "stg26/player_motions/rouge_float_motion_joint_4_angles.inc"
};
static NJS_MKEY_A rouge_float_motion_joint_7_angles[8] = {
#include "stg26/player_motions/rouge_float_motion_joint_7_angles.inc"
};
static NJS_MKEY_A rouge_float_motion_joint_8_angles[8] = {
#include "stg26/player_motions/rouge_float_motion_joint_8_angles.inc"
};
static NJS_MKEY_A rouge_float_motion_joint_9_angles[8] = {
#include "stg26/player_motions/rouge_float_motion_joint_9_angles.inc"
};
static NJS_MKEY_A rouge_float_motion_joint_13_angles[8] = {
#include "stg26/player_motions/rouge_float_motion_joint_13_angles.inc"
};
static NJS_MKEY_A rouge_float_motion_joint_14_angles[7] = {
#include "stg26/player_motions/rouge_float_motion_joint_14_angles.inc"
};
static NJS_MKEY_A rouge_float_motion_joint_17_angles[8] = {
#include "stg26/player_motions/rouge_float_motion_joint_17_angles.inc"
};
static NJS_MKEY_A rouge_float_motion_joint_18_angles[8] = {
#include "stg26/player_motions/rouge_float_motion_joint_18_angles.inc"
};
static NJS_MKEY_A rouge_float_motion_joint_19_angles[8] = {
#include "stg26/player_motions/rouge_float_motion_joint_19_angles.inc"
};
static NJS_MKEY_A rouge_float_motion_joint_23_angles[8] = {
#include "stg26/player_motions/rouge_float_motion_joint_23_angles.inc"
};
static NJS_MKEY_A rouge_float_motion_joint_24_angles[8] = {
#include "stg26/player_motions/rouge_float_motion_joint_24_angles.inc"
};
static NJS_MKEY_A rouge_float_motion_joint_27_angles[8] = {
#include "stg26/player_motions/rouge_float_motion_joint_27_angles.inc"
};
static NJS_MKEY_A rouge_float_motion_joint_28_angles[8] = {
#include "stg26/player_motions/rouge_float_motion_joint_28_angles.inc"
};
static NJS_MKEY_A rouge_float_motion_joint_29_angles[8] = {
#include "stg26/player_motions/rouge_float_motion_joint_29_angles.inc"
};
static NJS_MKEY_A rouge_float_motion_joint_31_angles[8] = {
#include "stg26/player_motions/rouge_float_motion_joint_31_angles.inc"
};
static NJS_MKEY_A rouge_float_motion_joint_32_angles[8] = {
#include "stg26/player_motions/rouge_float_motion_joint_32_angles.inc"
};
static NJS_MKEY_A rouge_float_motion_joint_33_angles[6] = {
#include "stg26/player_motions/rouge_float_motion_joint_33_angles.inc"
};
static NJS_MKEY_A rouge_float_motion_joint_37_angles[8] = {
#include "stg26/player_motions/rouge_float_motion_joint_37_angles.inc"
};
static NJS_MKEY_A rouge_float_motion_joint_38_angles[8] = {
#include "stg26/player_motions/rouge_float_motion_joint_38_angles.inc"
};
static NJS_MKEY_A rouge_float_motion_joint_39_angles[8] = {
#include "stg26/player_motions/rouge_float_motion_joint_39_angles.inc"
};
static NJS_MKEY_A rouge_float_motion_joint_40_angles[8] = {
#include "stg26/player_motions/rouge_float_motion_joint_40_angles.inc"
};
static NJS_MKEY_A rouge_float_motion_joint_41_angles[8] = {
#include "stg26/player_motions/rouge_float_motion_joint_41_angles.inc"
};
static NJS_MKEY_A rouge_float_motion_joint_44_angles[8] = {
#include "stg26/player_motions/rouge_float_motion_joint_44_angles.inc"
};
static NJS_MKEY_A rouge_float_motion_joint_51_angles[8] = {
#include "stg26/player_motions/rouge_float_motion_joint_51_angles.inc"
};
static NJS_MKEY_A rouge_float_motion_joint_53_angles[8] = {
#include "stg26/player_motions/rouge_float_motion_joint_53_angles.inc"
};
static NJS_MKEY_A rouge_float_motion_joint_56_angles[8] = {
#include "stg26/player_motions/rouge_float_motion_joint_56_angles.inc"
};
static NJS_MKEY_A rouge_float_motion_joint_57_angles[8] = {
#include "stg26/player_motions/rouge_float_motion_joint_57_angles.inc"
};
static NJS_MKEY_A rouge_float_motion_joint_58_angles[8] = {
#include "stg26/player_motions/rouge_float_motion_joint_58_angles.inc"
};
static NJS_MKEY_A rouge_float_motion_joint_59_angles[8] = {
#include "stg26/player_motions/rouge_float_motion_joint_59_angles.inc"
};
static NJS_MKEY_F rouge_float_motion_joint_60_position[2] = {
#include "stg26/player_motions/rouge_float_motion_joint_60_position.inc"
};
static NJS_MKEY_A rouge_float_motion_joint_60_angles[8] = {
#include "stg26/player_motions/rouge_float_motion_joint_60_angles.inc"
};
static NJS_MDATA2 rouge_float_motion_tracks[62] = {
#include "stg26/player_motions/rouge_float_motion_tracks.inc"
};
static NJS_MOTION rouge_float_motion = {
#include "stg26/player_motions/rouge_float_motion.inc"
};
static NJS_MKEY_F rouge_dive_start_motion_joint_2_position[8] = {
#include "stg26/player_motions/rouge_dive_start_motion_joint_2_position.inc"
};
static NJS_MKEY_A rouge_dive_start_motion_joint_4_angles[8] = {
#include "stg26/player_motions/rouge_dive_start_motion_joint_4_angles.inc"
};
static NJS_MKEY_A rouge_dive_start_motion_joint_7_angles[8] = {
#include "stg26/player_motions/rouge_dive_start_motion_joint_7_angles.inc"
};
static NJS_MKEY_A rouge_dive_start_motion_joint_8_angles[8] = {
#include "stg26/player_motions/rouge_dive_start_motion_joint_8_angles.inc"
};
static NJS_MKEY_A rouge_dive_start_motion_joint_9_angles[8] = {
#include "stg26/player_motions/rouge_dive_start_motion_joint_9_angles.inc"
};
static NJS_MKEY_A rouge_dive_start_motion_joint_13_angles[8] = {
#include "stg26/player_motions/rouge_dive_start_motion_joint_13_angles.inc"
};
static NJS_MKEY_A rouge_dive_start_motion_joint_14_angles[8] = {
#include "stg26/player_motions/rouge_dive_start_motion_joint_14_angles.inc"
};
static NJS_MKEY_A rouge_dive_start_motion_joint_17_angles[8] = {
#include "stg26/player_motions/rouge_dive_start_motion_joint_17_angles.inc"
};
static NJS_MKEY_A rouge_dive_start_motion_joint_18_angles[8] = {
#include "stg26/player_motions/rouge_dive_start_motion_joint_18_angles.inc"
};
static NJS_MKEY_A rouge_dive_start_motion_joint_19_angles[8] = {
#include "stg26/player_motions/rouge_dive_start_motion_joint_19_angles.inc"
};
static NJS_MKEY_A rouge_dive_start_motion_joint_23_angles[8] = {
#include "stg26/player_motions/rouge_dive_start_motion_joint_23_angles.inc"
};
static NJS_MKEY_A rouge_dive_start_motion_joint_24_angles[8] = {
#include "stg26/player_motions/rouge_dive_start_motion_joint_24_angles.inc"
};
static NJS_MKEY_A rouge_dive_start_motion_joint_27_angles[8] = {
#include "stg26/player_motions/rouge_dive_start_motion_joint_27_angles.inc"
};
static NJS_MKEY_A rouge_dive_start_motion_joint_28_angles[3] = {
#include "stg26/player_motions/rouge_dive_start_motion_joint_28_angles.inc"
};
static NJS_MKEY_A rouge_dive_start_motion_joint_29_angles[3] = {
#include "stg26/player_motions/rouge_dive_start_motion_joint_29_angles.inc"
};
static NJS_MKEY_A rouge_dive_start_motion_joint_31_angles[8] = {
#include "stg26/player_motions/rouge_dive_start_motion_joint_31_angles.inc"
};
static NJS_MKEY_A rouge_dive_start_motion_joint_32_angles[8] = {
#include "stg26/player_motions/rouge_dive_start_motion_joint_32_angles.inc"
};
static NJS_MKEY_A rouge_dive_start_motion_joint_33_angles[8] = {
#include "stg26/player_motions/rouge_dive_start_motion_joint_33_angles.inc"
};
static NJS_MKEY_F rouge_dive_start_motion_joint_35_position[2] = {
#include "stg26/player_motions/rouge_dive_start_motion_joint_35_position.inc"
};
static NJS_MKEY_F rouge_dive_start_motion_joint_36_position[2] = {
#include "stg26/player_motions/rouge_dive_start_motion_joint_36_position.inc"
};
static NJS_MKEY_A rouge_dive_start_motion_joint_37_angles[8] = {
#include "stg26/player_motions/rouge_dive_start_motion_joint_37_angles.inc"
};
static NJS_MKEY_A rouge_dive_start_motion_joint_38_angles[8] = {
#include "stg26/player_motions/rouge_dive_start_motion_joint_38_angles.inc"
};
static NJS_MKEY_A rouge_dive_start_motion_joint_39_angles[8] = {
#include "stg26/player_motions/rouge_dive_start_motion_joint_39_angles.inc"
};
static NJS_MKEY_A rouge_dive_start_motion_joint_40_angles[8] = {
#include "stg26/player_motions/rouge_dive_start_motion_joint_40_angles.inc"
};
static NJS_MKEY_A rouge_dive_start_motion_joint_41_angles[8] = {
#include "stg26/player_motions/rouge_dive_start_motion_joint_41_angles.inc"
};
static NJS_MKEY_A rouge_dive_start_motion_joint_44_angles[8] = {
#include "stg26/player_motions/rouge_dive_start_motion_joint_44_angles.inc"
};
static NJS_MKEY_A rouge_dive_start_motion_joint_51_angles[8] = {
#include "stg26/player_motions/rouge_dive_start_motion_joint_51_angles.inc"
};
static NJS_MKEY_A rouge_dive_start_motion_joint_53_angles[8] = {
#include "stg26/player_motions/rouge_dive_start_motion_joint_53_angles.inc"
};
static NJS_MKEY_A rouge_dive_start_motion_joint_56_angles[8] = {
#include "stg26/player_motions/rouge_dive_start_motion_joint_56_angles.inc"
};
static NJS_MKEY_A rouge_dive_start_motion_joint_57_angles[8] = {
#include "stg26/player_motions/rouge_dive_start_motion_joint_57_angles.inc"
};
static NJS_MKEY_A rouge_dive_start_motion_joint_58_angles[8] = {
#include "stg26/player_motions/rouge_dive_start_motion_joint_58_angles.inc"
};
static NJS_MKEY_A rouge_dive_start_motion_joint_59_angles[8] = {
#include "stg26/player_motions/rouge_dive_start_motion_joint_59_angles.inc"
};
static NJS_MKEY_F rouge_dive_start_motion_joint_60_position[3] = {
#include "stg26/player_motions/rouge_dive_start_motion_joint_60_position.inc"
};
static NJS_MKEY_A rouge_dive_start_motion_joint_60_angles[8] = {
#include "stg26/player_motions/rouge_dive_start_motion_joint_60_angles.inc"
};
static NJS_MDATA2 rouge_dive_start_motion_tracks[62] = {
#include "stg26/player_motions/rouge_dive_start_motion_tracks.inc"
};
static NJS_MOTION rouge_dive_start_motion = {
#include "stg26/player_motions/rouge_dive_start_motion.inc"
};
static NJS_MKEY_F rouge_dive_motion_joint_2_position[2] = {
#include "stg26/player_motions/rouge_dive_motion_joint_2_position.inc"
};
static NJS_MKEY_A rouge_dive_motion_joint_4_angles[4] = {
#include "stg26/player_motions/rouge_dive_motion_joint_4_angles.inc"
};
static NJS_MKEY_A rouge_dive_motion_joint_7_angles[4] = {
#include "stg26/player_motions/rouge_dive_motion_joint_7_angles.inc"
};
static NJS_MKEY_A rouge_dive_motion_joint_8_angles[4] = {
#include "stg26/player_motions/rouge_dive_motion_joint_8_angles.inc"
};
static NJS_MKEY_A rouge_dive_motion_joint_9_angles[4] = {
#include "stg26/player_motions/rouge_dive_motion_joint_9_angles.inc"
};
static NJS_MKEY_A rouge_dive_motion_joint_13_angles[4] = {
#include "stg26/player_motions/rouge_dive_motion_joint_13_angles.inc"
};
static NJS_MKEY_A rouge_dive_motion_joint_14_angles[4] = {
#include "stg26/player_motions/rouge_dive_motion_joint_14_angles.inc"
};
static NJS_MKEY_A rouge_dive_motion_joint_17_angles[4] = {
#include "stg26/player_motions/rouge_dive_motion_joint_17_angles.inc"
};
static NJS_MKEY_A rouge_dive_motion_joint_18_angles[4] = {
#include "stg26/player_motions/rouge_dive_motion_joint_18_angles.inc"
};
static NJS_MKEY_A rouge_dive_motion_joint_19_angles[4] = {
#include "stg26/player_motions/rouge_dive_motion_joint_19_angles.inc"
};
static NJS_MKEY_A rouge_dive_motion_joint_23_angles[4] = {
#include "stg26/player_motions/rouge_dive_motion_joint_23_angles.inc"
};
static NJS_MKEY_A rouge_dive_motion_joint_24_angles[4] = {
#include "stg26/player_motions/rouge_dive_motion_joint_24_angles.inc"
};
static NJS_MKEY_A rouge_dive_motion_joint_27_angles[4] = {
#include "stg26/player_motions/rouge_dive_motion_joint_27_angles.inc"
};
static NJS_MKEY_A rouge_dive_motion_joint_28_angles[2] = {
#include "stg26/player_motions/rouge_dive_motion_joint_28_angles.inc"
};
static NJS_MKEY_A rouge_dive_motion_joint_29_angles[2] = {
#include "stg26/player_motions/rouge_dive_motion_joint_29_angles.inc"
};
static NJS_MKEY_A rouge_dive_motion_joint_31_angles[2] = {
#include "stg26/player_motions/rouge_dive_motion_joint_31_angles.inc"
};
static NJS_MKEY_A rouge_dive_motion_joint_32_angles[4] = {
#include "stg26/player_motions/rouge_dive_motion_joint_32_angles.inc"
};
static NJS_MKEY_A rouge_dive_motion_joint_33_angles[2] = {
#include "stg26/player_motions/rouge_dive_motion_joint_33_angles.inc"
};
static NJS_MKEY_A rouge_dive_motion_joint_37_angles[4] = {
#include "stg26/player_motions/rouge_dive_motion_joint_37_angles.inc"
};
static NJS_MKEY_A rouge_dive_motion_joint_38_angles[4] = {
#include "stg26/player_motions/rouge_dive_motion_joint_38_angles.inc"
};
static NJS_MKEY_A rouge_dive_motion_joint_39_angles[4] = {
#include "stg26/player_motions/rouge_dive_motion_joint_39_angles.inc"
};
static NJS_MKEY_A rouge_dive_motion_joint_40_angles[4] = {
#include "stg26/player_motions/rouge_dive_motion_joint_40_angles.inc"
};
static NJS_MKEY_A rouge_dive_motion_joint_41_angles[4] = {
#include "stg26/player_motions/rouge_dive_motion_joint_41_angles.inc"
};
static NJS_MKEY_A rouge_dive_motion_joint_44_angles[4] = {
#include "stg26/player_motions/rouge_dive_motion_joint_44_angles.inc"
};
static NJS_MKEY_A rouge_dive_motion_joint_51_angles[4] = {
#include "stg26/player_motions/rouge_dive_motion_joint_51_angles.inc"
};
static NJS_MKEY_A rouge_dive_motion_joint_53_angles[4] = {
#include "stg26/player_motions/rouge_dive_motion_joint_53_angles.inc"
};
static NJS_MKEY_A rouge_dive_motion_joint_56_angles[4] = {
#include "stg26/player_motions/rouge_dive_motion_joint_56_angles.inc"
};
static NJS_MKEY_A rouge_dive_motion_joint_57_angles[4] = {
#include "stg26/player_motions/rouge_dive_motion_joint_57_angles.inc"
};
static NJS_MKEY_A rouge_dive_motion_joint_58_angles[4] = {
#include "stg26/player_motions/rouge_dive_motion_joint_58_angles.inc"
};
static NJS_MKEY_A rouge_dive_motion_joint_59_angles[4] = {
#include "stg26/player_motions/rouge_dive_motion_joint_59_angles.inc"
};
static NJS_MKEY_F rouge_dive_motion_joint_60_position[2] = {
#include "stg26/player_motions/rouge_dive_motion_joint_60_position.inc"
};
static NJS_MKEY_A rouge_dive_motion_joint_60_angles[4] = {
#include "stg26/player_motions/rouge_dive_motion_joint_60_angles.inc"
};
static NJS_MDATA2 rouge_dive_motion_tracks[62] = {
#include "stg26/player_motions/rouge_dive_motion_tracks.inc"
};
static NJS_MOTION rouge_dive_motion = {
#include "stg26/player_motions/rouge_dive_motion.inc"
};
static NJS_MKEY_F rouge_surface_swim_motion_joint_2_position[2] = {
#include "stg26/player_motions/rouge_surface_swim_motion_joint_2_position.inc"
};
static NJS_MKEY_A rouge_surface_swim_motion_joint_4_angles[17] = {
#include "stg26/player_motions/rouge_surface_swim_motion_joint_4_angles.inc"
};
static NJS_MKEY_A rouge_surface_swim_motion_joint_7_angles[17] = {
#include "stg26/player_motions/rouge_surface_swim_motion_joint_7_angles.inc"
};
static NJS_MKEY_A rouge_surface_swim_motion_joint_8_angles[17] = {
#include "stg26/player_motions/rouge_surface_swim_motion_joint_8_angles.inc"
};
static NJS_MKEY_A rouge_surface_swim_motion_joint_9_angles[17] = {
#include "stg26/player_motions/rouge_surface_swim_motion_joint_9_angles.inc"
};
static NJS_MKEY_A rouge_surface_swim_motion_joint_13_angles[17] = {
#include "stg26/player_motions/rouge_surface_swim_motion_joint_13_angles.inc"
};
static NJS_MKEY_A rouge_surface_swim_motion_joint_14_angles[15] = {
#include "stg26/player_motions/rouge_surface_swim_motion_joint_14_angles.inc"
};
static NJS_MKEY_A rouge_surface_swim_motion_joint_17_angles[17] = {
#include "stg26/player_motions/rouge_surface_swim_motion_joint_17_angles.inc"
};
static NJS_MKEY_A rouge_surface_swim_motion_joint_18_angles[17] = {
#include "stg26/player_motions/rouge_surface_swim_motion_joint_18_angles.inc"
};
static NJS_MKEY_A rouge_surface_swim_motion_joint_19_angles[17] = {
#include "stg26/player_motions/rouge_surface_swim_motion_joint_19_angles.inc"
};
static NJS_MKEY_A rouge_surface_swim_motion_joint_23_angles[17] = {
#include "stg26/player_motions/rouge_surface_swim_motion_joint_23_angles.inc"
};
static NJS_MKEY_A rouge_surface_swim_motion_joint_24_angles[15] = {
#include "stg26/player_motions/rouge_surface_swim_motion_joint_24_angles.inc"
};
static NJS_MKEY_A rouge_surface_swim_motion_joint_27_angles[17] = {
#include "stg26/player_motions/rouge_surface_swim_motion_joint_27_angles.inc"
};
static NJS_MKEY_A rouge_surface_swim_motion_joint_28_angles[17] = {
#include "stg26/player_motions/rouge_surface_swim_motion_joint_28_angles.inc"
};
static NJS_MKEY_A rouge_surface_swim_motion_joint_29_angles[17] = {
#include "stg26/player_motions/rouge_surface_swim_motion_joint_29_angles.inc"
};
static NJS_MKEY_A rouge_surface_swim_motion_joint_31_angles[17] = {
#include "stg26/player_motions/rouge_surface_swim_motion_joint_31_angles.inc"
};
static NJS_MKEY_A rouge_surface_swim_motion_joint_32_angles[16] = {
#include "stg26/player_motions/rouge_surface_swim_motion_joint_32_angles.inc"
};
static NJS_MKEY_A rouge_surface_swim_motion_joint_33_angles[7] = {
#include "stg26/player_motions/rouge_surface_swim_motion_joint_33_angles.inc"
};
static NJS_MKEY_A rouge_surface_swim_motion_joint_37_angles[17] = {
#include "stg26/player_motions/rouge_surface_swim_motion_joint_37_angles.inc"
};
static NJS_MKEY_A rouge_surface_swim_motion_joint_38_angles[17] = {
#include "stg26/player_motions/rouge_surface_swim_motion_joint_38_angles.inc"
};
static NJS_MKEY_A rouge_surface_swim_motion_joint_39_angles[17] = {
#include "stg26/player_motions/rouge_surface_swim_motion_joint_39_angles.inc"
};
static NJS_MKEY_A rouge_surface_swim_motion_joint_40_angles[17] = {
#include "stg26/player_motions/rouge_surface_swim_motion_joint_40_angles.inc"
};
static NJS_MKEY_A rouge_surface_swim_motion_joint_41_angles[17] = {
#include "stg26/player_motions/rouge_surface_swim_motion_joint_41_angles.inc"
};
static NJS_MKEY_A rouge_surface_swim_motion_joint_44_angles[17] = {
#include "stg26/player_motions/rouge_surface_swim_motion_joint_44_angles.inc"
};
static NJS_MKEY_A rouge_surface_swim_motion_joint_51_angles[17] = {
#include "stg26/player_motions/rouge_surface_swim_motion_joint_51_angles.inc"
};
static NJS_MKEY_A rouge_surface_swim_motion_joint_53_angles[17] = {
#include "stg26/player_motions/rouge_surface_swim_motion_joint_53_angles.inc"
};
static NJS_MKEY_A rouge_surface_swim_motion_joint_56_angles[17] = {
#include "stg26/player_motions/rouge_surface_swim_motion_joint_56_angles.inc"
};
static NJS_MKEY_A rouge_surface_swim_motion_joint_57_angles[14] = {
#include "stg26/player_motions/rouge_surface_swim_motion_joint_57_angles.inc"
};
static NJS_MKEY_A rouge_surface_swim_motion_joint_58_angles[14] = {
#include "stg26/player_motions/rouge_surface_swim_motion_joint_58_angles.inc"
};
static NJS_MKEY_A rouge_surface_swim_motion_joint_59_angles[17] = {
#include "stg26/player_motions/rouge_surface_swim_motion_joint_59_angles.inc"
};
static NJS_MKEY_F rouge_surface_swim_motion_joint_60_position[9] = {
#include "stg26/player_motions/rouge_surface_swim_motion_joint_60_position.inc"
};
static NJS_MKEY_A rouge_surface_swim_motion_joint_60_angles[17] = {
#include "stg26/player_motions/rouge_surface_swim_motion_joint_60_angles.inc"
};
static NJS_MDATA2 rouge_surface_swim_motion_tracks[62] = {
#include "stg26/player_motions/rouge_surface_swim_motion_tracks.inc"
};
static NJS_MOTION rouge_surface_swim_motion = {
#include "stg26/player_motions/rouge_surface_swim_motion.inc"
};
static NJS_MKEY_F rouge_surface_start_motion_joint_2_position[8] = {
#include "stg26/player_motions/rouge_surface_start_motion_joint_2_position.inc"
};
static NJS_MKEY_A rouge_surface_start_motion_joint_4_angles[8] = {
#include "stg26/player_motions/rouge_surface_start_motion_joint_4_angles.inc"
};
static NJS_MKEY_A rouge_surface_start_motion_joint_7_angles[8] = {
#include "stg26/player_motions/rouge_surface_start_motion_joint_7_angles.inc"
};
static NJS_MKEY_A rouge_surface_start_motion_joint_8_angles[8] = {
#include "stg26/player_motions/rouge_surface_start_motion_joint_8_angles.inc"
};
static NJS_MKEY_A rouge_surface_start_motion_joint_9_angles[8] = {
#include "stg26/player_motions/rouge_surface_start_motion_joint_9_angles.inc"
};
static NJS_MKEY_A rouge_surface_start_motion_joint_13_angles[8] = {
#include "stg26/player_motions/rouge_surface_start_motion_joint_13_angles.inc"
};
static NJS_MKEY_A rouge_surface_start_motion_joint_14_angles[8] = {
#include "stg26/player_motions/rouge_surface_start_motion_joint_14_angles.inc"
};
static NJS_MKEY_A rouge_surface_start_motion_joint_17_angles[8] = {
#include "stg26/player_motions/rouge_surface_start_motion_joint_17_angles.inc"
};
static NJS_MKEY_A rouge_surface_start_motion_joint_18_angles[8] = {
#include "stg26/player_motions/rouge_surface_start_motion_joint_18_angles.inc"
};
static NJS_MKEY_A rouge_surface_start_motion_joint_19_angles[8] = {
#include "stg26/player_motions/rouge_surface_start_motion_joint_19_angles.inc"
};
static NJS_MKEY_A rouge_surface_start_motion_joint_23_angles[8] = {
#include "stg26/player_motions/rouge_surface_start_motion_joint_23_angles.inc"
};
static NJS_MKEY_A rouge_surface_start_motion_joint_24_angles[8] = {
#include "stg26/player_motions/rouge_surface_start_motion_joint_24_angles.inc"
};
static NJS_MKEY_A rouge_surface_start_motion_joint_27_angles[8] = {
#include "stg26/player_motions/rouge_surface_start_motion_joint_27_angles.inc"
};
static NJS_MKEY_A rouge_surface_start_motion_joint_28_angles[8] = {
#include "stg26/player_motions/rouge_surface_start_motion_joint_28_angles.inc"
};
static NJS_MKEY_A rouge_surface_start_motion_joint_29_angles[8] = {
#include "stg26/player_motions/rouge_surface_start_motion_joint_29_angles.inc"
};
static NJS_MKEY_A rouge_surface_start_motion_joint_31_angles[8] = {
#include "stg26/player_motions/rouge_surface_start_motion_joint_31_angles.inc"
};
static NJS_MKEY_A rouge_surface_start_motion_joint_32_angles[8] = {
#include "stg26/player_motions/rouge_surface_start_motion_joint_32_angles.inc"
};
static NJS_MKEY_A rouge_surface_start_motion_joint_33_angles[8] = {
#include "stg26/player_motions/rouge_surface_start_motion_joint_33_angles.inc"
};
static NJS_MKEY_A rouge_surface_start_motion_joint_37_angles[8] = {
#include "stg26/player_motions/rouge_surface_start_motion_joint_37_angles.inc"
};
static NJS_MKEY_A rouge_surface_start_motion_joint_38_angles[8] = {
#include "stg26/player_motions/rouge_surface_start_motion_joint_38_angles.inc"
};
static NJS_MKEY_A rouge_surface_start_motion_joint_39_angles[8] = {
#include "stg26/player_motions/rouge_surface_start_motion_joint_39_angles.inc"
};
static NJS_MKEY_A rouge_surface_start_motion_joint_40_angles[8] = {
#include "stg26/player_motions/rouge_surface_start_motion_joint_40_angles.inc"
};
static NJS_MKEY_A rouge_surface_start_motion_joint_41_angles[8] = {
#include "stg26/player_motions/rouge_surface_start_motion_joint_41_angles.inc"
};
static NJS_MKEY_A rouge_surface_start_motion_joint_44_angles[8] = {
#include "stg26/player_motions/rouge_surface_start_motion_joint_44_angles.inc"
};
static NJS_MKEY_A rouge_surface_start_motion_joint_51_angles[8] = {
#include "stg26/player_motions/rouge_surface_start_motion_joint_51_angles.inc"
};
static NJS_MKEY_A rouge_surface_start_motion_joint_53_angles[8] = {
#include "stg26/player_motions/rouge_surface_start_motion_joint_53_angles.inc"
};
static NJS_MKEY_A rouge_surface_start_motion_joint_56_angles[8] = {
#include "stg26/player_motions/rouge_surface_start_motion_joint_56_angles.inc"
};
static NJS_MKEY_A rouge_surface_start_motion_joint_57_angles[8] = {
#include "stg26/player_motions/rouge_surface_start_motion_joint_57_angles.inc"
};
static NJS_MKEY_A rouge_surface_start_motion_joint_58_angles[8] = {
#include "stg26/player_motions/rouge_surface_start_motion_joint_58_angles.inc"
};
static NJS_MKEY_A rouge_surface_start_motion_joint_59_angles[8] = {
#include "stg26/player_motions/rouge_surface_start_motion_joint_59_angles.inc"
};
static NJS_MKEY_F rouge_surface_start_motion_joint_60_position[3] = {
#include "stg26/player_motions/rouge_surface_start_motion_joint_60_position.inc"
};
static NJS_MKEY_A rouge_surface_start_motion_joint_60_angles[8] = {
#include "stg26/player_motions/rouge_surface_start_motion_joint_60_angles.inc"
};
static NJS_MDATA2 rouge_surface_start_motion_tracks[62] = {
#include "stg26/player_motions/rouge_surface_start_motion_tracks.inc"
};
static NJS_MOTION rouge_surface_start_motion = {
#include "stg26/player_motions/rouge_surface_start_motion.inc"
};
static NJS_MKEY_F rouge_surface_motion_joint_2_position[2] = {
#include "stg26/player_motions/rouge_surface_motion_joint_2_position.inc"
};
static NJS_MKEY_A rouge_surface_motion_joint_4_angles[4] = {
#include "stg26/player_motions/rouge_surface_motion_joint_4_angles.inc"
};
static NJS_MKEY_A rouge_surface_motion_joint_7_angles[4] = {
#include "stg26/player_motions/rouge_surface_motion_joint_7_angles.inc"
};
static NJS_MKEY_A rouge_surface_motion_joint_8_angles[4] = {
#include "stg26/player_motions/rouge_surface_motion_joint_8_angles.inc"
};
static NJS_MKEY_A rouge_surface_motion_joint_9_angles[4] = {
#include "stg26/player_motions/rouge_surface_motion_joint_9_angles.inc"
};
static NJS_MKEY_A rouge_surface_motion_joint_13_angles[4] = {
#include "stg26/player_motions/rouge_surface_motion_joint_13_angles.inc"
};
static NJS_MKEY_A rouge_surface_motion_joint_14_angles[4] = {
#include "stg26/player_motions/rouge_surface_motion_joint_14_angles.inc"
};
static NJS_MKEY_A rouge_surface_motion_joint_17_angles[4] = {
#include "stg26/player_motions/rouge_surface_motion_joint_17_angles.inc"
};
static NJS_MKEY_A rouge_surface_motion_joint_18_angles[4] = {
#include "stg26/player_motions/rouge_surface_motion_joint_18_angles.inc"
};
static NJS_MKEY_A rouge_surface_motion_joint_19_angles[4] = {
#include "stg26/player_motions/rouge_surface_motion_joint_19_angles.inc"
};
static NJS_MKEY_A rouge_surface_motion_joint_23_angles[4] = {
#include "stg26/player_motions/rouge_surface_motion_joint_23_angles.inc"
};
static NJS_MKEY_A rouge_surface_motion_joint_24_angles[4] = {
#include "stg26/player_motions/rouge_surface_motion_joint_24_angles.inc"
};
static NJS_MKEY_A rouge_surface_motion_joint_27_angles[4] = {
#include "stg26/player_motions/rouge_surface_motion_joint_27_angles.inc"
};
static NJS_MKEY_A rouge_surface_motion_joint_28_angles[4] = {
#include "stg26/player_motions/rouge_surface_motion_joint_28_angles.inc"
};
static NJS_MKEY_A rouge_surface_motion_joint_29_angles[4] = {
#include "stg26/player_motions/rouge_surface_motion_joint_29_angles.inc"
};
static NJS_MKEY_A rouge_surface_motion_joint_31_angles[2] = {
#include "stg26/player_motions/rouge_surface_motion_joint_31_angles.inc"
};
static NJS_MKEY_A rouge_surface_motion_joint_32_angles[4] = {
#include "stg26/player_motions/rouge_surface_motion_joint_32_angles.inc"
};
static NJS_MKEY_A rouge_surface_motion_joint_33_angles[3] = {
#include "stg26/player_motions/rouge_surface_motion_joint_33_angles.inc"
};
static NJS_MKEY_A rouge_surface_motion_joint_37_angles[4] = {
#include "stg26/player_motions/rouge_surface_motion_joint_37_angles.inc"
};
static NJS_MKEY_A rouge_surface_motion_joint_38_angles[4] = {
#include "stg26/player_motions/rouge_surface_motion_joint_38_angles.inc"
};
static NJS_MKEY_A rouge_surface_motion_joint_39_angles[4] = {
#include "stg26/player_motions/rouge_surface_motion_joint_39_angles.inc"
};
static NJS_MKEY_A rouge_surface_motion_joint_40_angles[4] = {
#include "stg26/player_motions/rouge_surface_motion_joint_40_angles.inc"
};
static NJS_MKEY_A rouge_surface_motion_joint_41_angles[4] = {
#include "stg26/player_motions/rouge_surface_motion_joint_41_angles.inc"
};
static NJS_MKEY_A rouge_surface_motion_joint_44_angles[4] = {
#include "stg26/player_motions/rouge_surface_motion_joint_44_angles.inc"
};
static NJS_MKEY_A rouge_surface_motion_joint_51_angles[4] = {
#include "stg26/player_motions/rouge_surface_motion_joint_51_angles.inc"
};
static NJS_MKEY_A rouge_surface_motion_joint_53_angles[4] = {
#include "stg26/player_motions/rouge_surface_motion_joint_53_angles.inc"
};
static NJS_MKEY_A rouge_surface_motion_joint_56_angles[4] = {
#include "stg26/player_motions/rouge_surface_motion_joint_56_angles.inc"
};
static NJS_MKEY_A rouge_surface_motion_joint_57_angles[4] = {
#include "stg26/player_motions/rouge_surface_motion_joint_57_angles.inc"
};
static NJS_MKEY_A rouge_surface_motion_joint_58_angles[4] = {
#include "stg26/player_motions/rouge_surface_motion_joint_58_angles.inc"
};
static NJS_MKEY_A rouge_surface_motion_joint_59_angles[4] = {
#include "stg26/player_motions/rouge_surface_motion_joint_59_angles.inc"
};
static NJS_MKEY_F rouge_surface_motion_joint_60_position[2] = {
#include "stg26/player_motions/rouge_surface_motion_joint_60_position.inc"
};
static NJS_MKEY_A rouge_surface_motion_joint_60_angles[4] = {
#include "stg26/player_motions/rouge_surface_motion_joint_60_angles.inc"
};
static NJS_MDATA2 rouge_surface_motion_tracks[62] = {
#include "stg26/player_motions/rouge_surface_motion_tracks.inc"
};
static NJS_MOTION rouge_surface_motion = {
#include "stg26/player_motions/rouge_surface_motion.inc"
};
struct motion_entry module_motions[9] = {
#include "stg26/player_motions/module_motions.inc"
};
