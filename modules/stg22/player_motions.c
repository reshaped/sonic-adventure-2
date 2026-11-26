/* STG22.PRS's motions and their registry, from the disc. its D lies between
 * rolling_drum.c and dash_panel_art.c */
#include <ninja.h>

#include "player/player.h"
#include "shared/motions.h"

static NJS_MKEY_A upgrade_radical_highway_motion_joint_4_angles[16] = {
#include "stg22/player_motions/upgrade_radical_highway_motion_joint_4_angles.inc"
};
static NJS_MKEY_A upgrade_radical_highway_motion_joint_7_angles[16] = {
#include "stg22/player_motions/upgrade_radical_highway_motion_joint_7_angles.inc"
};
static NJS_MKEY_A upgrade_radical_highway_motion_joint_8_angles[7] = {
#include "stg22/player_motions/upgrade_radical_highway_motion_joint_8_angles.inc"
};
static NJS_MKEY_A upgrade_radical_highway_motion_joint_9_angles[12] = {
#include "stg22/player_motions/upgrade_radical_highway_motion_joint_9_angles.inc"
};
static NJS_MKEY_A upgrade_radical_highway_motion_joint_13_angles[16] = {
#include "stg22/player_motions/upgrade_radical_highway_motion_joint_13_angles.inc"
};
static NJS_MKEY_A upgrade_radical_highway_motion_joint_14_angles[2] = {
#include "stg22/player_motions/upgrade_radical_highway_motion_joint_14_angles.inc"
};
static NJS_MKEY_A upgrade_radical_highway_motion_joint_17_angles[16] = {
#include "stg22/player_motions/upgrade_radical_highway_motion_joint_17_angles.inc"
};
static NJS_MKEY_A upgrade_radical_highway_motion_joint_18_angles[8] = {
#include "stg22/player_motions/upgrade_radical_highway_motion_joint_18_angles.inc"
};
static NJS_MKEY_A upgrade_radical_highway_motion_joint_19_angles[14] = {
#include "stg22/player_motions/upgrade_radical_highway_motion_joint_19_angles.inc"
};
static NJS_MKEY_A upgrade_radical_highway_motion_joint_23_angles[16] = {
#include "stg22/player_motions/upgrade_radical_highway_motion_joint_23_angles.inc"
};
static NJS_MKEY_A upgrade_radical_highway_motion_joint_24_angles[2] = {
#include "stg22/player_motions/upgrade_radical_highway_motion_joint_24_angles.inc"
};
static NJS_MKEY_A upgrade_radical_highway_motion_joint_27_angles[16] = {
#include "stg22/player_motions/upgrade_radical_highway_motion_joint_27_angles.inc"
};
static NJS_MKEY_A upgrade_radical_highway_motion_joint_28_angles[16] = {
#include "stg22/player_motions/upgrade_radical_highway_motion_joint_28_angles.inc"
};
static NJS_MKEY_A upgrade_radical_highway_motion_joint_29_angles[16] = {
#include "stg22/player_motions/upgrade_radical_highway_motion_joint_29_angles.inc"
};
static NJS_MKEY_A upgrade_radical_highway_motion_joint_31_angles[2] = {
#include "stg22/player_motions/upgrade_radical_highway_motion_joint_31_angles.inc"
};
static NJS_MKEY_A upgrade_radical_highway_motion_joint_32_angles[16] = {
#include "stg22/player_motions/upgrade_radical_highway_motion_joint_32_angles.inc"
};
static NJS_MKEY_A upgrade_radical_highway_motion_joint_33_angles[13] = {
#include "stg22/player_motions/upgrade_radical_highway_motion_joint_33_angles.inc"
};
static NJS_MKEY_A upgrade_radical_highway_motion_joint_37_angles[16] = {
#include "stg22/player_motions/upgrade_radical_highway_motion_joint_37_angles.inc"
};
static NJS_MKEY_A upgrade_radical_highway_motion_joint_38_angles[16] = {
#include "stg22/player_motions/upgrade_radical_highway_motion_joint_38_angles.inc"
};
static NJS_MKEY_A upgrade_radical_highway_motion_joint_39_angles[16] = {
#include "stg22/player_motions/upgrade_radical_highway_motion_joint_39_angles.inc"
};
static NJS_MKEY_A upgrade_radical_highway_motion_joint_41_angles[16] = {
#include "stg22/player_motions/upgrade_radical_highway_motion_joint_41_angles.inc"
};
static NJS_MKEY_A upgrade_radical_highway_motion_joint_44_angles[16] = {
#include "stg22/player_motions/upgrade_radical_highway_motion_joint_44_angles.inc"
};
static NJS_MKEY_A upgrade_radical_highway_motion_joint_51_angles[16] = {
#include "stg22/player_motions/upgrade_radical_highway_motion_joint_51_angles.inc"
};
static NJS_MKEY_A upgrade_radical_highway_motion_joint_53_angles[16] = {
#include "stg22/player_motions/upgrade_radical_highway_motion_joint_53_angles.inc"
};
static NJS_MKEY_A upgrade_radical_highway_motion_joint_56_angles[16] = {
#include "stg22/player_motions/upgrade_radical_highway_motion_joint_56_angles.inc"
};
static NJS_MKEY_A upgrade_radical_highway_motion_joint_57_angles[16] = {
#include "stg22/player_motions/upgrade_radical_highway_motion_joint_57_angles.inc"
};
static NJS_MKEY_A upgrade_radical_highway_motion_joint_58_angles[16] = {
#include "stg22/player_motions/upgrade_radical_highway_motion_joint_58_angles.inc"
};
static NJS_MKEY_A upgrade_radical_highway_motion_joint_60_angles[16] = {
#include "stg22/player_motions/upgrade_radical_highway_motion_joint_60_angles.inc"
};
static NJS_MDATA2 upgrade_radical_highway_motion_tracks[62] = {
#include "stg22/player_motions/upgrade_radical_highway_motion_tracks.inc"
};
static NJS_MOTION upgrade_radical_highway_motion = {
#include "stg22/player_motions/upgrade_radical_highway_motion.inc"
};
static NJS_MKEY_F rail_hang_motion_joint_0_position[2] = {
#include "stg22/player_motions/rail_hang_motion_joint_0_position.inc"
};
static NJS_MKEY_F rail_hang_motion_joint_2_position[16] = {
#include "stg22/player_motions/rail_hang_motion_joint_2_position.inc"
};
static NJS_MKEY_A rail_hang_motion_joint_4_angles[16] = {
#include "stg22/player_motions/rail_hang_motion_joint_4_angles.inc"
};
static NJS_MKEY_A rail_hang_motion_joint_7_angles[16] = {
#include "stg22/player_motions/rail_hang_motion_joint_7_angles.inc"
};
static NJS_MKEY_A rail_hang_motion_joint_8_angles[16] = {
#include "stg22/player_motions/rail_hang_motion_joint_8_angles.inc"
};
static NJS_MKEY_A rail_hang_motion_joint_9_angles[16] = {
#include "stg22/player_motions/rail_hang_motion_joint_9_angles.inc"
};
static NJS_MKEY_A rail_hang_motion_joint_13_angles[16] = {
#include "stg22/player_motions/rail_hang_motion_joint_13_angles.inc"
};
static NJS_MKEY_A rail_hang_motion_joint_14_angles[2] = {
#include "stg22/player_motions/rail_hang_motion_joint_14_angles.inc"
};
static NJS_MKEY_A rail_hang_motion_joint_17_angles[16] = {
#include "stg22/player_motions/rail_hang_motion_joint_17_angles.inc"
};
static NJS_MKEY_A rail_hang_motion_joint_18_angles[10] = {
#include "stg22/player_motions/rail_hang_motion_joint_18_angles.inc"
};
static NJS_MKEY_A rail_hang_motion_joint_19_angles[16] = {
#include "stg22/player_motions/rail_hang_motion_joint_19_angles.inc"
};
static NJS_MKEY_A rail_hang_motion_joint_23_angles[16] = {
#include "stg22/player_motions/rail_hang_motion_joint_23_angles.inc"
};
static NJS_MKEY_A rail_hang_motion_joint_24_angles[2] = {
#include "stg22/player_motions/rail_hang_motion_joint_24_angles.inc"
};
static NJS_MKEY_A rail_hang_motion_joint_27_angles[16] = {
#include "stg22/player_motions/rail_hang_motion_joint_27_angles.inc"
};
static NJS_MKEY_A rail_hang_motion_joint_31_angles[16] = {
#include "stg22/player_motions/rail_hang_motion_joint_31_angles.inc"
};
static NJS_MKEY_A rail_hang_motion_joint_32_angles[12] = {
#include "stg22/player_motions/rail_hang_motion_joint_32_angles.inc"
};
static NJS_MKEY_A rail_hang_motion_joint_33_angles[12] = {
#include "stg22/player_motions/rail_hang_motion_joint_33_angles.inc"
};
static NJS_MKEY_A rail_hang_motion_joint_37_angles[12] = {
#include "stg22/player_motions/rail_hang_motion_joint_37_angles.inc"
};
static NJS_MKEY_A rail_hang_motion_joint_38_angles[12] = {
#include "stg22/player_motions/rail_hang_motion_joint_38_angles.inc"
};
static NJS_MKEY_A rail_hang_motion_joint_39_angles[12] = {
#include "stg22/player_motions/rail_hang_motion_joint_39_angles.inc"
};
static NJS_MKEY_A rail_hang_motion_joint_41_angles[2] = {
#include "stg22/player_motions/rail_hang_motion_joint_41_angles.inc"
};
static NJS_MKEY_A rail_hang_motion_joint_44_angles[12] = {
#include "stg22/player_motions/rail_hang_motion_joint_44_angles.inc"
};
static NJS_MKEY_A rail_hang_motion_joint_51_angles[16] = {
#include "stg22/player_motions/rail_hang_motion_joint_51_angles.inc"
};
static NJS_MKEY_A rail_hang_motion_joint_53_angles[16] = {
#include "stg22/player_motions/rail_hang_motion_joint_53_angles.inc"
};
static NJS_MKEY_A rail_hang_motion_joint_56_angles[12] = {
#include "stg22/player_motions/rail_hang_motion_joint_56_angles.inc"
};
static NJS_MKEY_A rail_hang_motion_joint_57_angles[2] = {
#include "stg22/player_motions/rail_hang_motion_joint_57_angles.inc"
};
static NJS_MKEY_A rail_hang_motion_joint_58_angles[2] = {
#include "stg22/player_motions/rail_hang_motion_joint_58_angles.inc"
};
static NJS_MKEY_A rail_hang_motion_joint_60_angles[12] = {
#include "stg22/player_motions/rail_hang_motion_joint_60_angles.inc"
};
static NJS_MDATA2 rail_hang_motion_tracks[62] = {
#include "stg22/player_motions/rail_hang_motion_tracks.inc"
};
static NJS_MOTION rail_hang_motion = {
#include "stg22/player_motions/rail_hang_motion.inc"
};
static NJS_MKEY_F crazy_gadget_pose_183_motion_joint_2_position[32] = {
#include "stg22/player_motions/crazy_gadget_pose_183_motion_joint_2_position.inc"
};
static NJS_MKEY_A crazy_gadget_pose_183_motion_joint_4_angles[30] = {
#include "stg22/player_motions/crazy_gadget_pose_183_motion_joint_4_angles.inc"
};
static NJS_MKEY_A crazy_gadget_pose_183_motion_joint_7_angles[32] = {
#include "stg22/player_motions/crazy_gadget_pose_183_motion_joint_7_angles.inc"
};
static NJS_MKEY_A crazy_gadget_pose_183_motion_joint_8_angles[31] = {
#include "stg22/player_motions/crazy_gadget_pose_183_motion_joint_8_angles.inc"
};
static NJS_MKEY_A crazy_gadget_pose_183_motion_joint_9_angles[32] = {
#include "stg22/player_motions/crazy_gadget_pose_183_motion_joint_9_angles.inc"
};
static NJS_MKEY_A crazy_gadget_pose_183_motion_joint_13_angles[32] = {
#include "stg22/player_motions/crazy_gadget_pose_183_motion_joint_13_angles.inc"
};
static NJS_MKEY_A crazy_gadget_pose_183_motion_joint_14_angles[32] = {
#include "stg22/player_motions/crazy_gadget_pose_183_motion_joint_14_angles.inc"
};
static NJS_MKEY_A crazy_gadget_pose_183_motion_joint_17_angles[32] = {
#include "stg22/player_motions/crazy_gadget_pose_183_motion_joint_17_angles.inc"
};
static NJS_MKEY_A crazy_gadget_pose_183_motion_joint_18_angles[32] = {
#include "stg22/player_motions/crazy_gadget_pose_183_motion_joint_18_angles.inc"
};
static NJS_MKEY_A crazy_gadget_pose_183_motion_joint_19_angles[32] = {
#include "stg22/player_motions/crazy_gadget_pose_183_motion_joint_19_angles.inc"
};
static NJS_MKEY_A crazy_gadget_pose_183_motion_joint_23_angles[32] = {
#include "stg22/player_motions/crazy_gadget_pose_183_motion_joint_23_angles.inc"
};
static NJS_MKEY_A crazy_gadget_pose_183_motion_joint_24_angles[32] = {
#include "stg22/player_motions/crazy_gadget_pose_183_motion_joint_24_angles.inc"
};
static NJS_MKEY_A crazy_gadget_pose_183_motion_joint_27_angles[31] = {
#include "stg22/player_motions/crazy_gadget_pose_183_motion_joint_27_angles.inc"
};
static NJS_MKEY_A crazy_gadget_pose_183_motion_joint_31_angles[32] = {
#include "stg22/player_motions/crazy_gadget_pose_183_motion_joint_31_angles.inc"
};
static NJS_MKEY_A crazy_gadget_pose_183_motion_joint_32_angles[32] = {
#include "stg22/player_motions/crazy_gadget_pose_183_motion_joint_32_angles.inc"
};
static NJS_MKEY_A crazy_gadget_pose_183_motion_joint_33_angles[26] = {
#include "stg22/player_motions/crazy_gadget_pose_183_motion_joint_33_angles.inc"
};
static NJS_MKEY_A crazy_gadget_pose_183_motion_joint_37_angles[32] = {
#include "stg22/player_motions/crazy_gadget_pose_183_motion_joint_37_angles.inc"
};
static NJS_MKEY_A crazy_gadget_pose_183_motion_joint_38_angles[32] = {
#include "stg22/player_motions/crazy_gadget_pose_183_motion_joint_38_angles.inc"
};
static NJS_MKEY_A crazy_gadget_pose_183_motion_joint_39_angles[32] = {
#include "stg22/player_motions/crazy_gadget_pose_183_motion_joint_39_angles.inc"
};
static NJS_MKEY_A crazy_gadget_pose_183_motion_joint_41_angles[32] = {
#include "stg22/player_motions/crazy_gadget_pose_183_motion_joint_41_angles.inc"
};
static NJS_MKEY_A crazy_gadget_pose_183_motion_joint_44_angles[32] = {
#include "stg22/player_motions/crazy_gadget_pose_183_motion_joint_44_angles.inc"
};
static NJS_MKEY_A crazy_gadget_pose_183_motion_joint_51_angles[32] = {
#include "stg22/player_motions/crazy_gadget_pose_183_motion_joint_51_angles.inc"
};
static NJS_MKEY_A crazy_gadget_pose_183_motion_joint_53_angles[32] = {
#include "stg22/player_motions/crazy_gadget_pose_183_motion_joint_53_angles.inc"
};
static NJS_MKEY_A crazy_gadget_pose_183_motion_joint_56_angles[32] = {
#include "stg22/player_motions/crazy_gadget_pose_183_motion_joint_56_angles.inc"
};
static NJS_MKEY_A crazy_gadget_pose_183_motion_joint_57_angles[32] = {
#include "stg22/player_motions/crazy_gadget_pose_183_motion_joint_57_angles.inc"
};
static NJS_MKEY_A crazy_gadget_pose_183_motion_joint_58_angles[32] = {
#include "stg22/player_motions/crazy_gadget_pose_183_motion_joint_58_angles.inc"
};
static NJS_MKEY_A crazy_gadget_pose_183_motion_joint_60_angles[32] = {
#include "stg22/player_motions/crazy_gadget_pose_183_motion_joint_60_angles.inc"
};
static NJS_MDATA2 crazy_gadget_pose_183_motion_tracks[62] = {
#include "stg22/player_motions/crazy_gadget_pose_183_motion_tracks.inc"
};
static NJS_MOTION crazy_gadget_pose_183_motion = {
#include "stg22/player_motions/crazy_gadget_pose_183_motion.inc"
};
struct motion_entry module_motions[4] = {
#include "stg22/player_motions/module_motions.inc"
};
