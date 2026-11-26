#include <ninja.h>
#include <sg_xpt.h>

#include "collision/world.h"
#include "player/player.h"
#include "shared/motions.h"
#include "stg10/metal_harbor.h"

/* the land's file names, which its land table and texture list point at. each
 * starts on a 4-byte boundary */
const char metal_harbor_land_filenames[505] = {
#include "stg10/terrain/metal_harbor_land_filenames.inc"
};

/* STG10.PRS's motions, their registry, land, texture lists, chunk models and
 * Basic models, from the disc. its D lies between metal_harbor.c and
 * objects.c */

/* the data up to the last that a pointer names ahead of its definition, as
 * tentative definitions in the disc's order: GCC places each datum at its
 * first definition */
static NJS_MKEY_A upgrade_white_jungle_motion_joint_4_angles[16];
static NJS_MKEY_A upgrade_white_jungle_motion_joint_7_angles[16];
static NJS_MKEY_A upgrade_white_jungle_motion_joint_8_angles[10];
static NJS_MKEY_A upgrade_white_jungle_motion_joint_9_angles[14];
static NJS_MKEY_A upgrade_white_jungle_motion_joint_13_angles[16];
static NJS_MKEY_A upgrade_white_jungle_motion_joint_14_angles[2];
static NJS_MKEY_A upgrade_white_jungle_motion_joint_17_angles[16];
static NJS_MKEY_A upgrade_white_jungle_motion_joint_18_angles[16];
static NJS_MKEY_A upgrade_white_jungle_motion_joint_19_angles[16];
static NJS_MKEY_A upgrade_white_jungle_motion_joint_23_angles[16];
static NJS_MKEY_A upgrade_white_jungle_motion_joint_24_angles[16];
static NJS_MKEY_A upgrade_white_jungle_motion_joint_27_angles[16];
static NJS_MKEY_A upgrade_white_jungle_motion_joint_28_angles[16];
static NJS_MKEY_A upgrade_white_jungle_motion_joint_29_angles[16];
static NJS_MKEY_A upgrade_white_jungle_motion_joint_31_angles[2];
static NJS_MKEY_A upgrade_white_jungle_motion_joint_32_angles[16];
static NJS_MKEY_A upgrade_white_jungle_motion_joint_33_angles[10];
static NJS_MKEY_A upgrade_white_jungle_motion_joint_37_angles[16];
static NJS_MKEY_A upgrade_white_jungle_motion_joint_38_angles[16];
static NJS_MKEY_A upgrade_white_jungle_motion_joint_39_angles[16];
static NJS_MKEY_A upgrade_white_jungle_motion_joint_41_angles[16];
static NJS_MKEY_A upgrade_white_jungle_motion_joint_44_angles[16];
static NJS_MKEY_A upgrade_white_jungle_motion_joint_51_angles[16];
static NJS_MKEY_A upgrade_white_jungle_motion_joint_53_angles[16];
static NJS_MKEY_A upgrade_white_jungle_motion_joint_56_angles[16];
static NJS_MKEY_A upgrade_white_jungle_motion_joint_57_angles[16];
static NJS_MKEY_A upgrade_white_jungle_motion_joint_58_angles[16];
static NJS_MKEY_A upgrade_white_jungle_motion_joint_60_angles[16];
static NJS_MDATA2 upgrade_white_jungle_motion_tracks[62];
static NJS_MOTION upgrade_white_jungle_motion;
static NJS_MKEY_F missile_hold_motion_joint_2_position[8];
static NJS_MKEY_A missile_hold_motion_joint_4_angles[2];
static NJS_MKEY_A missile_hold_motion_joint_7_angles[8];
static NJS_MKEY_A missile_hold_motion_joint_8_angles[6];
static NJS_MKEY_A missile_hold_motion_joint_9_angles[6];
static NJS_MKEY_A missile_hold_motion_joint_13_angles[8];
static NJS_MKEY_A missile_hold_motion_joint_14_angles[8];
static NJS_MKEY_A missile_hold_motion_joint_17_angles[8];
static NJS_MKEY_A missile_hold_motion_joint_18_angles[8];
static NJS_MKEY_A missile_hold_motion_joint_19_angles[8];
static NJS_MKEY_A missile_hold_motion_joint_23_angles[8];
static NJS_MKEY_A missile_hold_motion_joint_24_angles[8];
static NJS_MKEY_A missile_hold_motion_joint_27_angles[2];
static NJS_MKEY_A missile_hold_motion_joint_31_angles[2];
static NJS_MKEY_A missile_hold_motion_joint_32_angles[2];
static NJS_MKEY_A missile_hold_motion_joint_37_angles[8];
static NJS_MKEY_A missile_hold_motion_joint_38_angles[8];
static NJS_MKEY_A missile_hold_motion_joint_39_angles[8];
static NJS_MKEY_A missile_hold_motion_joint_41_angles[8];
static NJS_MKEY_A missile_hold_motion_joint_44_angles[8];
static NJS_MKEY_A missile_hold_motion_joint_51_angles[8];
static NJS_MKEY_A missile_hold_motion_joint_53_angles[8];
static NJS_MKEY_A missile_hold_motion_joint_56_angles[8];
static NJS_MKEY_A missile_hold_motion_joint_57_angles[4];
static NJS_MKEY_A missile_hold_motion_joint_58_angles[4];
static NJS_MKEY_A missile_hold_motion_joint_60_angles[8];
static NJS_MDATA2 missile_hold_motion_tracks[62];
static NJS_MOTION missile_hold_motion;
static NJS_MKEY_F harbor_pose_173_motion_joint_2_position[32];
static NJS_MKEY_A harbor_pose_173_motion_joint_4_angles[32];
static NJS_MKEY_A harbor_pose_173_motion_joint_7_angles[32];
static NJS_MKEY_A harbor_pose_173_motion_joint_8_angles[30];
static NJS_MKEY_A harbor_pose_173_motion_joint_9_angles[30];
static NJS_MKEY_A harbor_pose_173_motion_joint_13_angles[32];
static NJS_MKEY_A harbor_pose_173_motion_joint_14_angles[20];
static NJS_MKEY_A harbor_pose_173_motion_joint_17_angles[32];
static NJS_MKEY_A harbor_pose_173_motion_joint_18_angles[25];
static NJS_MKEY_A harbor_pose_173_motion_joint_19_angles[25];
static NJS_MKEY_A harbor_pose_173_motion_joint_23_angles[32];
static NJS_MKEY_A harbor_pose_173_motion_joint_27_angles[5];
static NJS_MKEY_A harbor_pose_173_motion_joint_31_angles[32];
static NJS_MKEY_A harbor_pose_173_motion_joint_32_angles[32];
static NJS_MKEY_A harbor_pose_173_motion_joint_33_angles[25];
static NJS_MKEY_A harbor_pose_173_motion_joint_37_angles[32];
static NJS_MKEY_A harbor_pose_173_motion_joint_38_angles[32];
static NJS_MKEY_A harbor_pose_173_motion_joint_39_angles[32];
static NJS_MKEY_A harbor_pose_173_motion_joint_41_angles[32];
static NJS_MKEY_A harbor_pose_173_motion_joint_44_angles[32];
static NJS_MKEY_A harbor_pose_173_motion_joint_51_angles[2];
static NJS_MKEY_A harbor_pose_173_motion_joint_53_angles[2];
static NJS_MKEY_A harbor_pose_173_motion_joint_56_angles[32];
static NJS_MKEY_A harbor_pose_173_motion_joint_57_angles[32];
static NJS_MKEY_A harbor_pose_173_motion_joint_58_angles[32];
static NJS_MKEY_A harbor_pose_173_motion_joint_60_angles[32];
static NJS_MDATA2 harbor_pose_173_motion_tracks[62];
static NJS_MOTION harbor_pose_173_motion;
static NJS_MKEY_A unused_harbor_pose_motion_joint_4_angles[4];
static NJS_MKEY_A unused_harbor_pose_motion_joint_7_angles[4];
static NJS_MKEY_A unused_harbor_pose_motion_joint_8_angles[4];
static NJS_MKEY_A unused_harbor_pose_motion_joint_9_angles[4];
static NJS_MKEY_A unused_harbor_pose_motion_joint_13_angles[4];
static NJS_MKEY_A unused_harbor_pose_motion_joint_14_angles[4];
static NJS_MKEY_A unused_harbor_pose_motion_joint_17_angles[4];
static NJS_MKEY_A unused_harbor_pose_motion_joint_18_angles[4];
static NJS_MKEY_A unused_harbor_pose_motion_joint_19_angles[4];
static NJS_MKEY_A unused_harbor_pose_motion_joint_23_angles[4];
static NJS_MKEY_A unused_harbor_pose_motion_joint_24_angles[4];
static NJS_MKEY_A unused_harbor_pose_motion_joint_27_angles[4];
static NJS_MKEY_A unused_harbor_pose_motion_joint_31_angles[4];
static NJS_MKEY_A unused_harbor_pose_motion_joint_32_angles[4];
static NJS_MKEY_A unused_harbor_pose_motion_joint_33_angles[4];
static NJS_MKEY_A unused_harbor_pose_motion_joint_37_angles[4];
static NJS_MKEY_A unused_harbor_pose_motion_joint_38_angles[2];
static NJS_MKEY_A unused_harbor_pose_motion_joint_39_angles[2];
static NJS_MKEY_A unused_harbor_pose_motion_joint_41_angles[4];
static NJS_MKEY_A unused_harbor_pose_motion_joint_44_angles[4];
static NJS_MKEY_A unused_harbor_pose_motion_joint_51_angles[2];
static NJS_MKEY_A unused_harbor_pose_motion_joint_53_angles[2];
static NJS_MKEY_A unused_harbor_pose_motion_joint_56_angles[4];
static NJS_MKEY_A unused_harbor_pose_motion_joint_57_angles[2];
static NJS_MKEY_A unused_harbor_pose_motion_joint_58_angles[2];
static NJS_MKEY_A unused_harbor_pose_motion_joint_60_angles[4];
static NJS_MDATA2 unused_harbor_pose_motion_tracks[62];
static NJS_MOTION unused_harbor_pose_motion;
static NJS_MKEY_F rider_trick_harbor_a_motion_joint_2_position[140];
static NJS_MKEY_A rider_trick_harbor_a_motion_joint_4_angles[140];
static NJS_MKEY_A rider_trick_harbor_a_motion_joint_7_angles[140];
static NJS_MKEY_A rider_trick_harbor_a_motion_joint_8_angles[130];
static NJS_MKEY_A rider_trick_harbor_a_motion_joint_9_angles[131];
static NJS_MKEY_A rider_trick_harbor_a_motion_joint_13_angles[140];
static NJS_MKEY_A rider_trick_harbor_a_motion_joint_14_angles[113];
static NJS_MKEY_A rider_trick_harbor_a_motion_joint_17_angles[140];
static NJS_MKEY_A rider_trick_harbor_a_motion_joint_18_angles[132];
static NJS_MKEY_A rider_trick_harbor_a_motion_joint_19_angles[140];
static NJS_MKEY_A rider_trick_harbor_a_motion_joint_23_angles[140];
static NJS_MKEY_A rider_trick_harbor_a_motion_joint_24_angles[107];
static NJS_MKEY_A rider_trick_harbor_a_motion_joint_27_angles[140];
static NJS_MKEY_A rider_trick_harbor_a_motion_joint_28_angles[101];
static NJS_MKEY_A rider_trick_harbor_a_motion_joint_29_angles[105];
static NJS_MKEY_A rider_trick_harbor_a_motion_joint_31_angles[140];
static NJS_MKEY_A rider_trick_harbor_a_motion_joint_32_angles[140];
static NJS_MKEY_A rider_trick_harbor_a_motion_joint_33_angles[100];
static NJS_MKEY_A rider_trick_harbor_a_motion_joint_37_angles[140];
static NJS_MKEY_A rider_trick_harbor_a_motion_joint_38_angles[99];
static NJS_MKEY_A rider_trick_harbor_a_motion_joint_39_angles[99];
static NJS_MKEY_A rider_trick_harbor_a_motion_joint_41_angles[140];
static NJS_MKEY_A rider_trick_harbor_a_motion_joint_44_angles[140];
static NJS_MKEY_A rider_trick_harbor_a_motion_joint_51_angles[2];
static NJS_MKEY_A rider_trick_harbor_a_motion_joint_53_angles[2];
static NJS_MKEY_A rider_trick_harbor_a_motion_joint_56_angles[140];
static NJS_MKEY_A rider_trick_harbor_a_motion_joint_57_angles[86];
static NJS_MKEY_A rider_trick_harbor_a_motion_joint_58_angles[86];
static NJS_MKEY_A rider_trick_harbor_a_motion_joint_60_angles[140];
static NJS_MDATA2 rider_trick_harbor_a_motion_tracks[62];
static NJS_MOTION rider_trick_harbor_a_motion;
static NJS_MKEY_F rider_trick_harbor_b_motion_joint_2_position[121];
static NJS_MKEY_A rider_trick_harbor_b_motion_joint_4_angles[121];
static NJS_MKEY_A rider_trick_harbor_b_motion_joint_7_angles[121];
static NJS_MKEY_A rider_trick_harbor_b_motion_joint_8_angles[106];
static NJS_MKEY_A rider_trick_harbor_b_motion_joint_9_angles[121];
static NJS_MKEY_A rider_trick_harbor_b_motion_joint_13_angles[121];
static NJS_MKEY_A rider_trick_harbor_b_motion_joint_14_angles[105];
static NJS_MKEY_A rider_trick_harbor_b_motion_joint_17_angles[121];
static NJS_MKEY_A rider_trick_harbor_b_motion_joint_18_angles[94];
static NJS_MKEY_A rider_trick_harbor_b_motion_joint_19_angles[94];
static NJS_MKEY_A rider_trick_harbor_b_motion_joint_23_angles[121];
static NJS_MKEY_A rider_trick_harbor_b_motion_joint_24_angles[106];
static NJS_MKEY_A rider_trick_harbor_b_motion_joint_27_angles[121];
static NJS_MKEY_A rider_trick_harbor_b_motion_joint_28_angles[84];
static NJS_MKEY_A rider_trick_harbor_b_motion_joint_29_angles[91];
static NJS_MKEY_A rider_trick_harbor_b_motion_joint_31_angles[121];
static NJS_MKEY_A rider_trick_harbor_b_motion_joint_32_angles[121];
static NJS_MKEY_A rider_trick_harbor_b_motion_joint_33_angles[85];
static NJS_MKEY_A rider_trick_harbor_b_motion_joint_37_angles[121];
static NJS_MKEY_A rider_trick_harbor_b_motion_joint_38_angles[105];
static NJS_MKEY_A rider_trick_harbor_b_motion_joint_39_angles[105];
static NJS_MKEY_A rider_trick_harbor_b_motion_joint_41_angles[121];
static NJS_MKEY_A rider_trick_harbor_b_motion_joint_44_angles[121];
static NJS_MKEY_A rider_trick_harbor_b_motion_joint_51_angles[2];
static NJS_MKEY_A rider_trick_harbor_b_motion_joint_53_angles[2];
static NJS_MKEY_A rider_trick_harbor_b_motion_joint_56_angles[121];
static NJS_MKEY_A rider_trick_harbor_b_motion_joint_57_angles[118];
static NJS_MKEY_A rider_trick_harbor_b_motion_joint_58_angles[118];
static NJS_MKEY_A rider_trick_harbor_b_motion_joint_60_angles[121];
static NJS_MDATA2 rider_trick_harbor_b_motion_tracks[62];
static NJS_MOTION rider_trick_harbor_b_motion;
static NJS_MKEY_F rider_trick_harbor_c_motion_joint_2_position[90];
static NJS_MKEY_A rider_trick_harbor_c_motion_joint_4_angles[90];
static NJS_MKEY_A rider_trick_harbor_c_motion_joint_7_angles[90];
static NJS_MKEY_A rider_trick_harbor_c_motion_joint_8_angles[76];
static NJS_MKEY_A rider_trick_harbor_c_motion_joint_9_angles[87];
static NJS_MKEY_A rider_trick_harbor_c_motion_joint_13_angles[90];
static NJS_MKEY_A rider_trick_harbor_c_motion_joint_14_angles[81];
static NJS_MKEY_A rider_trick_harbor_c_motion_joint_17_angles[90];
static NJS_MKEY_A rider_trick_harbor_c_motion_joint_18_angles[90];
static NJS_MKEY_A rider_trick_harbor_c_motion_joint_19_angles[90];
static NJS_MKEY_A rider_trick_harbor_c_motion_joint_23_angles[90];
static NJS_MKEY_A rider_trick_harbor_c_motion_joint_24_angles[90];
static NJS_MKEY_A rider_trick_harbor_c_motion_joint_27_angles[90];
static NJS_MKEY_A rider_trick_harbor_c_motion_joint_28_angles[64];
static NJS_MKEY_A rider_trick_harbor_c_motion_joint_29_angles[64];
static NJS_MKEY_A rider_trick_harbor_c_motion_joint_31_angles[90];
static NJS_MKEY_A rider_trick_harbor_c_motion_joint_32_angles[90];
static NJS_MKEY_A rider_trick_harbor_c_motion_joint_33_angles[75];
static NJS_MKEY_A rider_trick_harbor_c_motion_joint_37_angles[90];
static NJS_MKEY_A rider_trick_harbor_c_motion_joint_38_angles[66];
static NJS_MKEY_A rider_trick_harbor_c_motion_joint_39_angles[66];
static NJS_MKEY_A rider_trick_harbor_c_motion_joint_41_angles[90];
static NJS_MKEY_A rider_trick_harbor_c_motion_joint_44_angles[90];
static NJS_MKEY_A rider_trick_harbor_c_motion_joint_51_angles[2];
static NJS_MKEY_A rider_trick_harbor_c_motion_joint_53_angles[2];
static NJS_MKEY_A rider_trick_harbor_c_motion_joint_56_angles[90];
static NJS_MKEY_A rider_trick_harbor_c_motion_joint_57_angles[64];
static NJS_MKEY_A rider_trick_harbor_c_motion_joint_58_angles[64];
static NJS_MKEY_A rider_trick_harbor_c_motion_joint_60_angles[90];
static NJS_MDATA2 rider_trick_harbor_c_motion_tracks[62];
static NJS_MOTION rider_trick_harbor_c_motion;
static NJS_MKEY_F rider_lean_positive_motion_joint_2_position[17];
static NJS_MKEY_A rider_lean_positive_motion_joint_4_angles[17];
static NJS_MKEY_A rider_lean_positive_motion_joint_7_angles[17];
static NJS_MKEY_A rider_lean_positive_motion_joint_8_angles[17];
static NJS_MKEY_A rider_lean_positive_motion_joint_9_angles[17];
static NJS_MKEY_A rider_lean_positive_motion_joint_13_angles[17];
static NJS_MKEY_A rider_lean_positive_motion_joint_14_angles[8];
static NJS_MKEY_A rider_lean_positive_motion_joint_17_angles[17];
static NJS_MKEY_A rider_lean_positive_motion_joint_18_angles[15];
static NJS_MKEY_A rider_lean_positive_motion_joint_19_angles[17];
static NJS_MKEY_A rider_lean_positive_motion_joint_23_angles[17];
static NJS_MKEY_A rider_lean_positive_motion_joint_24_angles[10];
static NJS_MKEY_A rider_lean_positive_motion_joint_27_angles[17];
static NJS_MKEY_A rider_lean_positive_motion_joint_31_angles[17];
static NJS_MKEY_A rider_lean_positive_motion_joint_32_angles[17];
static NJS_MKEY_A rider_lean_positive_motion_joint_33_angles[12];
static NJS_MKEY_A rider_lean_positive_motion_joint_37_angles[17];
static NJS_MKEY_A rider_lean_positive_motion_joint_38_angles[17];
static NJS_MKEY_A rider_lean_positive_motion_joint_39_angles[17];
static NJS_MKEY_A rider_lean_positive_motion_joint_41_angles[17];
static NJS_MKEY_A rider_lean_positive_motion_joint_44_angles[17];
static NJS_MKEY_A rider_lean_positive_motion_joint_51_angles[2];
static NJS_MKEY_A rider_lean_positive_motion_joint_53_angles[2];
static NJS_MKEY_A rider_lean_positive_motion_joint_56_angles[17];
static NJS_MKEY_A rider_lean_positive_motion_joint_57_angles[17];
static NJS_MKEY_A rider_lean_positive_motion_joint_58_angles[17];
static NJS_MKEY_A rider_lean_positive_motion_joint_60_angles[17];
static NJS_MDATA2 rider_lean_positive_motion_tracks[62];
static NJS_MOTION rider_lean_positive_motion;
static NJS_MKEY_F rider_lean_negative_motion_joint_2_position[2];
static NJS_MKEY_A rider_lean_negative_motion_joint_4_angles[17];
static NJS_MKEY_A rider_lean_negative_motion_joint_7_angles[17];
static NJS_MKEY_A rider_lean_negative_motion_joint_8_angles[17];
static NJS_MKEY_A rider_lean_negative_motion_joint_9_angles[17];
static NJS_MKEY_A rider_lean_negative_motion_joint_13_angles[17];
static NJS_MKEY_A rider_lean_negative_motion_joint_14_angles[9];
static NJS_MKEY_A rider_lean_negative_motion_joint_17_angles[17];
static NJS_MKEY_A rider_lean_negative_motion_joint_18_angles[17];
static NJS_MKEY_A rider_lean_negative_motion_joint_19_angles[17];
static NJS_MKEY_A rider_lean_negative_motion_joint_23_angles[17];
static NJS_MKEY_A rider_lean_negative_motion_joint_24_angles[12];
static NJS_MKEY_A rider_lean_negative_motion_joint_27_angles[17];
static NJS_MKEY_A rider_lean_negative_motion_joint_28_angles[5];
static NJS_MKEY_A rider_lean_negative_motion_joint_29_angles[5];
static NJS_MKEY_A rider_lean_negative_motion_joint_31_angles[17];
static NJS_MKEY_A rider_lean_negative_motion_joint_32_angles[17];
static NJS_MKEY_A rider_lean_negative_motion_joint_33_angles[13];
static NJS_MKEY_A rider_lean_negative_motion_joint_37_angles[17];
static NJS_MKEY_A rider_lean_negative_motion_joint_38_angles[17];
static NJS_MKEY_A rider_lean_negative_motion_joint_39_angles[17];
static NJS_MKEY_A rider_lean_negative_motion_joint_41_angles[17];
static NJS_MKEY_A rider_lean_negative_motion_joint_44_angles[17];
static NJS_MKEY_A rider_lean_negative_motion_joint_51_angles[2];
static NJS_MKEY_A rider_lean_negative_motion_joint_53_angles[2];
static NJS_MKEY_A rider_lean_negative_motion_joint_56_angles[17];
static NJS_MKEY_A rider_lean_negative_motion_joint_57_angles[17];
static NJS_MKEY_A rider_lean_negative_motion_joint_58_angles[17];
static NJS_MKEY_A rider_lean_negative_motion_joint_60_angles[17];
static NJS_MDATA2 rider_lean_negative_motion_tracks[62];
static NJS_MOTION rider_lean_negative_motion;
static NJS_MKEY_F rider_turn_positive_motion_joint_2_position[9];
static NJS_MKEY_A rider_turn_positive_motion_joint_4_angles[9];
static NJS_MKEY_A rider_turn_positive_motion_joint_7_angles[9];
static NJS_MKEY_A rider_turn_positive_motion_joint_8_angles[2];
static NJS_MKEY_A rider_turn_positive_motion_joint_9_angles[2];
static NJS_MKEY_A rider_turn_positive_motion_joint_13_angles[5];
static NJS_MKEY_A rider_turn_positive_motion_joint_17_angles[9];
static NJS_MKEY_A rider_turn_positive_motion_joint_18_angles[2];
static NJS_MKEY_A rider_turn_positive_motion_joint_19_angles[8];
static NJS_MKEY_A rider_turn_positive_motion_joint_23_angles[9];
static NJS_MKEY_A rider_turn_positive_motion_joint_27_angles[9];
static NJS_MKEY_A rider_turn_positive_motion_joint_31_angles[9];
static NJS_MKEY_A rider_turn_positive_motion_joint_32_angles[9];
static NJS_MKEY_A rider_turn_positive_motion_joint_33_angles[9];
static NJS_MKEY_A rider_turn_positive_motion_joint_37_angles[9];
static NJS_MKEY_A rider_turn_positive_motion_joint_38_angles[2];
static NJS_MKEY_A rider_turn_positive_motion_joint_39_angles[2];
static NJS_MKEY_A rider_turn_positive_motion_joint_41_angles[9];
static NJS_MKEY_A rider_turn_positive_motion_joint_44_angles[9];
static NJS_MKEY_A rider_turn_positive_motion_joint_51_angles[2];
static NJS_MKEY_A rider_turn_positive_motion_joint_53_angles[2];
static NJS_MKEY_A rider_turn_positive_motion_joint_56_angles[9];
static NJS_MKEY_A rider_turn_positive_motion_joint_57_angles[8];
static NJS_MKEY_A rider_turn_positive_motion_joint_58_angles[8];
static NJS_MKEY_A rider_turn_positive_motion_joint_60_angles[9];
static NJS_MDATA2 rider_turn_positive_motion_tracks[62];
static NJS_MOTION rider_turn_positive_motion;
static NJS_MKEY_F rider_turn_negative_motion_joint_2_position[8];
static NJS_MKEY_A rider_turn_negative_motion_joint_4_angles[8];
static NJS_MKEY_A rider_turn_negative_motion_joint_7_angles[8];
static NJS_MKEY_A rider_turn_negative_motion_joint_8_angles[8];
static NJS_MKEY_A rider_turn_negative_motion_joint_9_angles[8];
static NJS_MKEY_A rider_turn_negative_motion_joint_13_angles[8];
static NJS_MKEY_A rider_turn_negative_motion_joint_14_angles[6];
static NJS_MKEY_A rider_turn_negative_motion_joint_17_angles[8];
static NJS_MKEY_A rider_turn_negative_motion_joint_18_angles[8];
static NJS_MKEY_A rider_turn_negative_motion_joint_19_angles[8];
static NJS_MKEY_A rider_turn_negative_motion_joint_23_angles[8];
static NJS_MKEY_A rider_turn_negative_motion_joint_27_angles[8];
static NJS_MKEY_A rider_turn_negative_motion_joint_28_angles[8];
static NJS_MKEY_A rider_turn_negative_motion_joint_29_angles[8];
static NJS_MKEY_A rider_turn_negative_motion_joint_31_angles[8];
static NJS_MKEY_A rider_turn_negative_motion_joint_32_angles[8];
static NJS_MKEY_A rider_turn_negative_motion_joint_33_angles[6];
static NJS_MKEY_A rider_turn_negative_motion_joint_37_angles[8];
static NJS_MKEY_A rider_turn_negative_motion_joint_38_angles[8];
static NJS_MKEY_A rider_turn_negative_motion_joint_39_angles[8];
static NJS_MKEY_A rider_turn_negative_motion_joint_41_angles[8];
static NJS_MKEY_A rider_turn_negative_motion_joint_44_angles[8];
static NJS_MKEY_A rider_turn_negative_motion_joint_51_angles[2];
static NJS_MKEY_A rider_turn_negative_motion_joint_53_angles[2];
static NJS_MKEY_A rider_turn_negative_motion_joint_56_angles[8];
static NJS_MKEY_A rider_turn_negative_motion_joint_57_angles[8];
static NJS_MKEY_A rider_turn_negative_motion_joint_58_angles[8];
static NJS_MKEY_A rider_turn_negative_motion_joint_60_angles[8];
static NJS_MDATA2 rider_turn_negative_motion_tracks[62];
static NJS_MOTION rider_turn_negative_motion;
static NJS_MKEY_F rider_jump_motion_joint_2_position[8];
static NJS_MKEY_A rider_jump_motion_joint_4_angles[2];
static NJS_MKEY_A rider_jump_motion_joint_7_angles[8];
static NJS_MKEY_A rider_jump_motion_joint_8_angles[8];
static NJS_MKEY_A rider_jump_motion_joint_9_angles[8];
static NJS_MKEY_A rider_jump_motion_joint_13_angles[8];
static NJS_MKEY_A rider_jump_motion_joint_14_angles[2];
static NJS_MKEY_A rider_jump_motion_joint_17_angles[8];
static NJS_MKEY_A rider_jump_motion_joint_18_angles[8];
static NJS_MKEY_A rider_jump_motion_joint_19_angles[8];
static NJS_MKEY_A rider_jump_motion_joint_23_angles[8];
static NJS_MKEY_A rider_jump_motion_joint_24_angles[2];
static NJS_MKEY_A rider_jump_motion_joint_27_angles[2];
static NJS_MKEY_A rider_jump_motion_joint_28_angles[2];
static NJS_MKEY_A rider_jump_motion_joint_29_angles[2];
static NJS_MKEY_A rider_jump_motion_joint_31_angles[8];
static NJS_MKEY_A rider_jump_motion_joint_32_angles[8];
static NJS_MKEY_A rider_jump_motion_joint_33_angles[8];
static NJS_MKEY_A rider_jump_motion_joint_37_angles[8];
static NJS_MKEY_A rider_jump_motion_joint_38_angles[8];
static NJS_MKEY_A rider_jump_motion_joint_39_angles[8];
static NJS_MKEY_A rider_jump_motion_joint_41_angles[8];
static NJS_MKEY_A rider_jump_motion_joint_44_angles[8];
static NJS_MKEY_A rider_jump_motion_joint_51_angles[2];
static NJS_MKEY_A rider_jump_motion_joint_53_angles[2];
static NJS_MKEY_A rider_jump_motion_joint_56_angles[8];
static NJS_MKEY_A rider_jump_motion_joint_57_angles[8];
static NJS_MKEY_A rider_jump_motion_joint_58_angles[8];
static NJS_MKEY_A rider_jump_motion_joint_60_angles[8];
static NJS_MDATA2 rider_jump_motion_tracks[62];
static NJS_MOTION rider_jump_motion;
static NJS_MKEY_F rider_apex_motion_joint_2_position[12];
static NJS_MKEY_A rider_apex_motion_joint_4_angles[12];
static NJS_MKEY_A rider_apex_motion_joint_7_angles[12];
static NJS_MKEY_A rider_apex_motion_joint_8_angles[12];
static NJS_MKEY_A rider_apex_motion_joint_9_angles[12];
static NJS_MKEY_A rider_apex_motion_joint_13_angles[12];
static NJS_MKEY_A rider_apex_motion_joint_14_angles[12];
static NJS_MKEY_A rider_apex_motion_joint_17_angles[12];
static NJS_MKEY_A rider_apex_motion_joint_18_angles[11];
static NJS_MKEY_A rider_apex_motion_joint_19_angles[12];
static NJS_MKEY_A rider_apex_motion_joint_23_angles[12];
static NJS_MKEY_A rider_apex_motion_joint_24_angles[12];
static NJS_MKEY_A rider_apex_motion_joint_27_angles[12];
static NJS_MKEY_A rider_apex_motion_joint_28_angles[12];
static NJS_MKEY_A rider_apex_motion_joint_29_angles[12];
static NJS_MKEY_A rider_apex_motion_joint_31_angles[12];
static NJS_MKEY_A rider_apex_motion_joint_32_angles[12];
static NJS_MKEY_A rider_apex_motion_joint_33_angles[9];
static NJS_MKEY_A rider_apex_motion_joint_37_angles[12];
static NJS_MKEY_A rider_apex_motion_joint_38_angles[12];
static NJS_MKEY_A rider_apex_motion_joint_39_angles[12];
static NJS_MKEY_A rider_apex_motion_joint_41_angles[12];
static NJS_MKEY_A rider_apex_motion_joint_44_angles[12];
static NJS_MKEY_A rider_apex_motion_joint_51_angles[2];
static NJS_MKEY_A rider_apex_motion_joint_53_angles[2];
static NJS_MKEY_A rider_apex_motion_joint_56_angles[12];
static NJS_MKEY_A rider_apex_motion_joint_57_angles[12];
static NJS_MKEY_A rider_apex_motion_joint_58_angles[12];
static NJS_MKEY_A rider_apex_motion_joint_60_angles[12];
static NJS_MDATA2 rider_apex_motion_tracks[62];
static NJS_MOTION rider_apex_motion;
static NJS_MKEY_F rider_fall_motion_joint_2_position[8];
static NJS_MKEY_A rider_fall_motion_joint_4_angles[8];
static NJS_MKEY_A rider_fall_motion_joint_7_angles[8];
static NJS_MKEY_A rider_fall_motion_joint_8_angles[8];
static NJS_MKEY_A rider_fall_motion_joint_9_angles[8];
static NJS_MKEY_A rider_fall_motion_joint_13_angles[8];
static NJS_MKEY_A rider_fall_motion_joint_14_angles[3];
static NJS_MKEY_A rider_fall_motion_joint_17_angles[8];
static NJS_MKEY_A rider_fall_motion_joint_18_angles[8];
static NJS_MKEY_A rider_fall_motion_joint_19_angles[8];
static NJS_MKEY_A rider_fall_motion_joint_23_angles[8];
static NJS_MKEY_A rider_fall_motion_joint_24_angles[8];
static NJS_MKEY_A rider_fall_motion_joint_27_angles[8];
static NJS_MKEY_A rider_fall_motion_joint_28_angles[8];
static NJS_MKEY_A rider_fall_motion_joint_29_angles[8];
static NJS_MKEY_A rider_fall_motion_joint_31_angles[8];
static NJS_MKEY_A rider_fall_motion_joint_32_angles[8];
static NJS_MKEY_A rider_fall_motion_joint_33_angles[7];
static NJS_MKEY_A rider_fall_motion_joint_37_angles[8];
static NJS_MKEY_A rider_fall_motion_joint_38_angles[8];
static NJS_MKEY_A rider_fall_motion_joint_39_angles[8];
static NJS_MKEY_A rider_fall_motion_joint_41_angles[8];
static NJS_MKEY_A rider_fall_motion_joint_44_angles[8];
static NJS_MKEY_A rider_fall_motion_joint_51_angles[2];
static NJS_MKEY_A rider_fall_motion_joint_53_angles[2];
static NJS_MKEY_A rider_fall_motion_joint_56_angles[8];
static NJS_MKEY_A rider_fall_motion_joint_57_angles[8];
static NJS_MKEY_A rider_fall_motion_joint_58_angles[8];
static NJS_MKEY_A rider_fall_motion_joint_60_angles[8];
static NJS_MDATA2 rider_fall_motion_tracks[62];
static NJS_MOTION rider_fall_motion;
static NJS_MKEY_F rider_crash_motion_joint_2_position[20];
static NJS_MKEY_A rider_crash_motion_joint_4_angles[20];
static NJS_MKEY_A rider_crash_motion_joint_7_angles[20];
static NJS_MKEY_A rider_crash_motion_joint_8_angles[20];
static NJS_MKEY_A rider_crash_motion_joint_9_angles[20];
static NJS_MKEY_A rider_crash_motion_joint_13_angles[20];
static NJS_MKEY_A rider_crash_motion_joint_14_angles[20];
static NJS_MKEY_A rider_crash_motion_joint_17_angles[20];
static NJS_MKEY_A rider_crash_motion_joint_18_angles[20];
static NJS_MKEY_A rider_crash_motion_joint_19_angles[20];
static NJS_MKEY_A rider_crash_motion_joint_23_angles[20];
static NJS_MKEY_A rider_crash_motion_joint_24_angles[20];
static NJS_MKEY_A rider_crash_motion_joint_27_angles[20];
static NJS_MKEY_A rider_crash_motion_joint_28_angles[14];
static NJS_MKEY_A rider_crash_motion_joint_29_angles[14];
static NJS_MKEY_A rider_crash_motion_joint_31_angles[20];
static NJS_MKEY_A rider_crash_motion_joint_32_angles[20];
static NJS_MKEY_A rider_crash_motion_joint_33_angles[17];
static NJS_MKEY_A rider_crash_motion_joint_37_angles[20];
static NJS_MKEY_A rider_crash_motion_joint_38_angles[20];
static NJS_MKEY_A rider_crash_motion_joint_39_angles[20];
static NJS_MKEY_A rider_crash_motion_joint_41_angles[20];
static NJS_MKEY_A rider_crash_motion_joint_44_angles[20];
static NJS_MKEY_A rider_crash_motion_joint_51_angles[2];
static NJS_MKEY_A rider_crash_motion_joint_53_angles[2];
static NJS_MKEY_A rider_crash_motion_joint_56_angles[20];
static NJS_MKEY_A rider_crash_motion_joint_57_angles[19];
static NJS_MKEY_A rider_crash_motion_joint_58_angles[19];
static NJS_MKEY_A rider_crash_motion_joint_60_angles[20];
static NJS_MDATA2 rider_crash_motion_tracks[62];
static NJS_MOTION rider_crash_motion;
static NJS_MKEY_F rider_land_motion_joint_2_position[8];
static NJS_MKEY_A rider_land_motion_joint_4_angles[2];
static NJS_MKEY_A rider_land_motion_joint_7_angles[8];
static NJS_MKEY_A rider_land_motion_joint_8_angles[8];
static NJS_MKEY_A rider_land_motion_joint_9_angles[8];
static NJS_MKEY_A rider_land_motion_joint_13_angles[8];
static NJS_MKEY_A rider_land_motion_joint_14_angles[2];
static NJS_MKEY_A rider_land_motion_joint_17_angles[8];
static NJS_MKEY_A rider_land_motion_joint_18_angles[8];
static NJS_MKEY_A rider_land_motion_joint_19_angles[8];
static NJS_MKEY_A rider_land_motion_joint_23_angles[8];
static NJS_MKEY_A rider_land_motion_joint_24_angles[2];
static NJS_MKEY_A rider_land_motion_joint_27_angles[8];
static NJS_MKEY_A rider_land_motion_joint_28_angles[8];
static NJS_MKEY_A rider_land_motion_joint_29_angles[8];
static NJS_MKEY_A rider_land_motion_joint_31_angles[2];
static NJS_MKEY_A rider_land_motion_joint_32_angles[6];
static NJS_MKEY_A rider_land_motion_joint_33_angles[6];
static NJS_MKEY_A rider_land_motion_joint_37_angles[8];
static NJS_MKEY_A rider_land_motion_joint_38_angles[8];
static NJS_MKEY_A rider_land_motion_joint_39_angles[8];
static NJS_MKEY_A rider_land_motion_joint_41_angles[8];
static NJS_MKEY_A rider_land_motion_joint_44_angles[8];
static NJS_MKEY_A rider_land_motion_joint_51_angles[2];
static NJS_MKEY_A rider_land_motion_joint_53_angles[2];
static NJS_MKEY_A rider_land_motion_joint_56_angles[8];
static NJS_MKEY_A rider_land_motion_joint_57_angles[8];
static NJS_MKEY_A rider_land_motion_joint_58_angles[8];
static NJS_MKEY_A rider_land_motion_joint_60_angles[8];
static NJS_MDATA2 rider_land_motion_tracks[62];
static NJS_MOTION rider_land_motion;
static NJS_MKEY_F board_trick_harbor_a_motion_joint_0_position[140];
static NJS_MKEY_A board_trick_harbor_a_motion_joint_1_angles[140];
static NJS_MDATA2 board_trick_harbor_a_motion_tracks[2];
static NJS_MOTION board_trick_harbor_a_motion;
static NJS_MKEY_F board_trick_harbor_b_motion_joint_0_position[121];
static NJS_MKEY_A board_trick_harbor_b_motion_joint_0_angles[121];
static NJS_MDATA2 board_trick_harbor_b_motion_tracks[2];
static NJS_MOTION board_trick_harbor_b_motion;
static NJS_MKEY_F board_trick_harbor_c_motion_joint_0_position[90];
static NJS_MKEY_A board_trick_harbor_c_motion_joint_1_angles[90];
static NJS_MDATA2 board_trick_harbor_c_motion_tracks[2];
static NJS_MOTION board_trick_harbor_c_motion;
static NJS_MKEY_F board_lean_positive_motion_joint_0_position[17];
static NJS_MKEY_A board_lean_positive_motion_joint_1_angles[17];
static NJS_MDATA2 board_lean_positive_motion_tracks[2];
static NJS_MOTION board_lean_positive_motion;
static NJS_MKEY_F board_lean_negative_motion_joint_0_position[17];
static NJS_MKEY_F board_lean_negative_motion_joint_1_position[17];
static NJS_MKEY_A board_lean_negative_motion_joint_1_angles[17];
static NJS_MDATA2 board_lean_negative_motion_tracks[2];
static NJS_MOTION board_lean_negative_motion;
static NJS_MKEY_F board_turn_positive_motion_joint_0_position[9];
static NJS_MKEY_A board_turn_positive_motion_joint_1_angles[3];
static NJS_MDATA2 board_turn_positive_motion_tracks[2];
static NJS_MOTION board_turn_positive_motion;
static NJS_MKEY_F board_turn_negative_motion_joint_0_position[8];
static NJS_MKEY_A board_turn_negative_motion_joint_1_angles[8];
static NJS_MDATA2 board_turn_negative_motion_tracks[2];
static NJS_MOTION board_turn_negative_motion;
static NJS_MKEY_F board_jump_motion_joint_0_position[8];
static NJS_MKEY_A board_jump_motion_joint_1_angles[8];
static NJS_MDATA2 board_jump_motion_tracks[2];
static NJS_MOTION board_jump_motion;
static NJS_MKEY_F board_apex_motion_joint_0_position[12];
static NJS_MKEY_A board_apex_motion_joint_0_angles[11];
static NJS_MKEY_A board_apex_motion_joint_1_angles[12];
static NJS_MDATA2 board_apex_motion_tracks[2];
static NJS_MOTION board_apex_motion;
static NJS_MKEY_F board_fall_motion_joint_0_position[8];
static NJS_MKEY_A board_fall_motion_joint_0_angles[8];
static NJS_MKEY_A board_fall_motion_joint_1_angles[8];
static NJS_MDATA2 board_fall_motion_tracks[2];
static NJS_MOTION board_fall_motion;
static NJS_MKEY_F board_crash_motion_joint_0_position[20];
static NJS_MKEY_A board_crash_motion_joint_1_angles[20];
static NJS_MDATA2 board_crash_motion_tracks[2];
static NJS_MOTION board_crash_motion;
static NJS_MDATA2 board_land_motion_tracks[2];
static NJS_MOTION board_land_motion;
struct motion_entry module_motions[34];
static struct land_entry entries[769];
struct land_table metal_harbor_land;
static NJS_TEXNAME textures[50];
NJS_TEXLIST metal_harbor_terrain_textures;
static Sint16 draw_446_polygons[55];
static Sint32 draw_446_vertices[51];
static NJS_CNK_MODEL draw_446_model;
static NJS_CNK_OBJECT draw_446;
static Sint16 draw_420_polygons[217];
static Sint32 draw_420_vertices[235];
static NJS_CNK_MODEL draw_420_model;
static NJS_CNK_OBJECT draw_445;
static Sint16 draw_417_polygons[543];
static Sint32 draw_417_vertices[611];
static NJS_CNK_MODEL draw_417_model;
static NJS_CNK_OBJECT draw_419;
static Sint16 draw_414_polygons[115];
static Sint32 draw_414_vertices[107];
static NJS_CNK_MODEL draw_414_model;
static NJS_CNK_OBJECT draw_416;
static Sint16 draw_413_polygons[119];
static Sint32 draw_413_vertices[95];
static NJS_CNK_MODEL draw_413_model;
static NJS_CNK_OBJECT draw_413;
static Sint16 draw_412_polygons[119];
static Sint32 draw_412_vertices[99];
static NJS_CNK_MODEL draw_412_model;
static NJS_CNK_OBJECT draw_412;
static Sint16 draw_409_polygons[231];
static Sint32 draw_409_vertices[243];
static NJS_CNK_MODEL draw_409_model;
static NJS_CNK_OBJECT draw_411;
static Sint16 draw_376_polygons[1125];
static Sint32 draw_376_vertices[1023];
static NJS_CNK_MODEL draw_376_model;
static NJS_CNK_OBJECT draw_408;
static Sint16 draw_373_polygons[61];
static Sint32 draw_373_vertices[55];
static NJS_CNK_MODEL draw_373_model;
static NJS_CNK_OBJECT draw_375;
static Sint16 draw_369_polygons[549];
static Sint32 draw_369_vertices[467];
static NJS_CNK_MODEL draw_369_model;
static NJS_CNK_OBJECT draw_372;
static Sint16 draw_368_polygons[113];
static Sint32 draw_368_vertices[59];
static NJS_CNK_MODEL draw_368_model;
static NJS_CNK_OBJECT draw_368;
static Sint16 draw_364_polygons[119];
static Sint32 draw_364_vertices[87];
static NJS_CNK_MODEL draw_364_model;
static NJS_CNK_OBJECT draw_367;
static Sint16 draw_361_polygons[257];
static Sint32 draw_361_vertices[299];
static NJS_CNK_MODEL draw_361_model;
static NJS_CNK_OBJECT draw_363;
static Sint16 draw_359_polygons[37];
static Sint32 draw_359_vertices[35];
static NJS_CNK_MODEL draw_359_model;
static NJS_CNK_OBJECT draw_360;
static Sint16 draw_355_polygons[91];
static Sint32 draw_355_vertices[39];
static NJS_CNK_MODEL draw_355_model;
static NJS_CNK_OBJECT draw_358;
static Sint16 draw_352_polygons[23];
static Sint32 draw_352_vertices[19];
static NJS_CNK_MODEL draw_352_model;
static NJS_CNK_OBJECT draw_354;
static Sint16 draw_351_polygons[167];
static Sint32 draw_351_vertices[195];
static NJS_CNK_MODEL draw_351_model;
static NJS_CNK_OBJECT draw_351;
static Sint16 draw_350_polygons[181];
static Sint32 draw_350_vertices[159];
static NJS_CNK_MODEL draw_350_model;
static NJS_CNK_OBJECT draw_350;
static Sint16 draw_348_polygons[337];
static Sint32 draw_348_vertices[315];
static NJS_CNK_MODEL draw_348_model;
static NJS_CNK_OBJECT draw_349;
static Sint16 draw_346_polygons[399];
static Sint32 draw_346_vertices[247];
static NJS_CNK_MODEL draw_346_model;
static NJS_CNK_OBJECT draw_347;
static Sint16 draw_338_polygons[1075];
static Sint32 draw_338_vertices[1175];
static NJS_CNK_MODEL draw_338_model;
static NJS_CNK_OBJECT draw_345;
static Sint16 draw_337_polygons[139];
static Sint32 draw_337_vertices[123];
static NJS_CNK_MODEL draw_337_model;
static NJS_CNK_OBJECT draw_337;
static Sint16 draw_334_polygons[69];
static Sint32 draw_334_vertices[35];
static NJS_CNK_MODEL draw_334_model;
static NJS_CNK_OBJECT draw_336;
static Sint16 draw_331_polygons[55];
static Sint32 draw_331_vertices[51];
static NJS_CNK_MODEL draw_331_model;
static NJS_CNK_OBJECT draw_333;
static Sint16 draw_330_polygons[591];
static Sint32 draw_330_vertices[379];
static NJS_CNK_MODEL draw_330_model;
static NJS_CNK_OBJECT draw_330;
static Sint16 draw_327_polygons[155];
static Sint32 draw_327_vertices[67];
static NJS_CNK_MODEL draw_327_model;
static NJS_CNK_OBJECT draw_329;
static Sint16 draw_324_polygons[273];
static Sint32 draw_324_vertices[123];
static NJS_CNK_MODEL draw_324_model;
static NJS_CNK_OBJECT draw_326;
static Sint16 draw_323_polygons[613];
static Sint32 draw_323_vertices[307];
static NJS_CNK_MODEL draw_323_model;
static NJS_CNK_OBJECT draw_323;
static Sint16 draw_321_polygons[203];
static Sint32 draw_321_vertices[163];
static NJS_CNK_MODEL draw_321_model;
static NJS_CNK_OBJECT draw_322;
static Sint16 draw_302_polygons[551];
static Sint32 draw_302_vertices[567];
static NJS_CNK_MODEL draw_302_model;
static NJS_CNK_OBJECT draw_320;
static Sint16 draw_300_polygons[87];
static Sint32 draw_300_vertices[67];
static NJS_CNK_MODEL draw_300_model;
static NJS_CNK_OBJECT draw_301;
static Sint16 draw_299_polygons[165];
static Sint32 draw_299_vertices[87];
static NJS_CNK_MODEL draw_299_model;
static NJS_CNK_OBJECT draw_299;
static Sint16 draw_298_polygons[279];
static Sint32 draw_298_vertices[183];
static NJS_CNK_MODEL draw_298_model;
static NJS_CNK_OBJECT draw_298;
static Sint16 draw_295_polygons[297];
static Sint32 draw_295_vertices[347];
static NJS_CNK_MODEL draw_295_model;
static NJS_CNK_OBJECT draw_297;
static Sint16 draw_293_polygons[23];
static Sint32 draw_293_vertices[19];
static NJS_CNK_MODEL draw_293_model;
static NJS_CNK_OBJECT draw_294;
static Sint16 draw_287_polygons[23];
static Sint32 draw_287_vertices[19];
static NJS_CNK_MODEL draw_287_model;
static NJS_CNK_OBJECT draw_292;
static Sint16 draw_286_polygons[379];
static Sint32 draw_286_vertices[395];
static NJS_CNK_MODEL draw_286_model;
static NJS_CNK_OBJECT draw_286;
static Sint16 draw_285_polygons[47];
static Sint32 draw_285_vertices[51];
static NJS_CNK_MODEL draw_285_model;
static NJS_CNK_OBJECT draw_285;
static Sint16 draw_284_polygons[339];
static Sint32 draw_284_vertices[403];
static NJS_CNK_MODEL draw_284_model;
static NJS_CNK_OBJECT draw_284;
static Sint16 draw_283_polygons[121];
static Sint32 draw_283_vertices[71];
static NJS_CNK_MODEL draw_283_model;
static NJS_CNK_OBJECT draw_283;
static Sint16 draw_281_polygons[309];
static Sint32 draw_281_vertices[279];
static NJS_CNK_MODEL draw_281_model;
static NJS_CNK_OBJECT draw_282;
static Sint16 draw_279_polygons[167];
static Sint32 draw_279_vertices[127];
static NJS_CNK_MODEL draw_279_model;
static NJS_CNK_OBJECT draw_280;
static Sint16 draw_273_polygons[897];
static Sint32 draw_273_vertices[703];
static NJS_CNK_MODEL draw_273_model;
static NJS_CNK_OBJECT draw_278;
static Sint16 draw_270_polygons[613];
static Sint32 draw_270_vertices[311];
static NJS_CNK_MODEL draw_270_model;
static NJS_CNK_OBJECT draw_272;
static Sint16 draw_268_polygons[921];
static Sint32 draw_268_vertices[827];
static NJS_CNK_MODEL draw_268_model;
static NJS_CNK_OBJECT draw_269;
static Sint16 draw_267_polygons[177];
static Sint32 draw_267_vertices[163];
static NJS_CNK_MODEL draw_267_model;
static NJS_CNK_OBJECT draw_267;
static Sint16 draw_260_polygons[893];
static Sint32 draw_260_vertices[827];
static NJS_CNK_MODEL draw_260_model;
static NJS_CNK_OBJECT draw_266;
static Sint16 draw_259_polygons[61];
static Sint32 draw_259_vertices[55];
static NJS_CNK_MODEL draw_259_model;
static NJS_CNK_OBJECT draw_259;
static Sint16 draw_257_polygons[171];
static Sint32 draw_257_vertices[87];
static NJS_CNK_MODEL draw_257_model;
static NJS_CNK_OBJECT draw_258;
static Sint16 draw_251_polygons[101];
static Sint32 draw_251_vertices[91];
static NJS_CNK_MODEL draw_251_model;
static NJS_CNK_OBJECT draw_256;
static Sint16 draw_248_polygons[263];
static Sint32 draw_248_vertices[263];
static NJS_CNK_MODEL draw_248_model;
static NJS_CNK_OBJECT draw_250;
static Sint16 draw_247_polygons[37];
static Sint32 draw_247_vertices[35];
static NJS_CNK_MODEL draw_247_model;
static NJS_CNK_OBJECT draw_247;
static Sint16 draw_246_polygons[77];
static Sint32 draw_246_vertices[51];
static NJS_CNK_MODEL draw_246_model;
static NJS_CNK_OBJECT draw_246;
static Sint16 draw_245_polygons[65];
static Sint32 draw_245_vertices[47];
static NJS_CNK_MODEL draw_245_model;
static NJS_CNK_OBJECT draw_245;
static Sint16 draw_241_polygons[445];
static Sint32 draw_241_vertices[431];
static NJS_CNK_MODEL draw_241_model;
static NJS_CNK_OBJECT draw_244;
static Sint16 draw_240_polygons[777];
static Sint32 draw_240_vertices[759];
static NJS_CNK_MODEL draw_240_model;
static NJS_CNK_OBJECT draw_240;
static Sint16 draw_239_polygons[173];
static Sint32 draw_239_vertices[123];
static NJS_CNK_MODEL draw_239_model;
static NJS_CNK_OBJECT draw_239;
static Sint16 draw_236_polygons[171];
static Sint32 draw_236_vertices[151];
static NJS_CNK_MODEL draw_236_model;
static NJS_CNK_OBJECT draw_238;
static Sint16 draw_235_polygons[927];
static Sint32 draw_235_vertices[703];
static NJS_CNK_MODEL draw_235_model;
static NJS_CNK_OBJECT draw_235;
static Sint16 draw_234_polygons[43];
static Sint32 draw_234_vertices[35];
static NJS_CNK_MODEL draw_234_model;
static NJS_CNK_OBJECT draw_234;
static Sint16 draw_232_polygons[159];
static Sint32 draw_232_vertices[155];
static NJS_CNK_MODEL draw_232_model;
static NJS_CNK_OBJECT draw_233;
static Sint16 draw_231_polygons[229];
static Sint32 draw_231_vertices[159];
static NJS_CNK_MODEL draw_231_model;
static NJS_CNK_OBJECT draw_231;
static Sint16 draw_230_polygons[215];
static Sint32 draw_230_vertices[207];
static NJS_CNK_MODEL draw_230_model;
static NJS_CNK_OBJECT draw_230;
static Sint16 draw_229_polygons[87];
static Sint32 draw_229_vertices[83];
static NJS_CNK_MODEL draw_229_model;
static NJS_CNK_OBJECT draw_229;
static Sint16 draw_226_polygons[163];
static Sint32 draw_226_vertices[179];
static NJS_CNK_MODEL draw_226_model;
static NJS_CNK_OBJECT draw_228;
static Sint16 draw_225_polygons[121];
static Sint32 draw_225_vertices[75];
static NJS_CNK_MODEL draw_225_model;
static NJS_CNK_OBJECT draw_225;
static Sint16 draw_223_polygons[163];
static Sint32 draw_223_vertices[171];
static NJS_CNK_MODEL draw_223_model;
static NJS_CNK_OBJECT draw_224;
static Sint16 draw_220_polygons[139];
static Sint32 draw_220_vertices[147];
static NJS_CNK_MODEL draw_220_model;
static NJS_CNK_OBJECT draw_222;
static Sint16 draw_219_polygons[163];
static Sint32 draw_219_vertices[175];
static NJS_CNK_MODEL draw_219_model;
static NJS_CNK_OBJECT draw_219;
static Sint16 draw_218_polygons[171];
static Sint32 draw_218_vertices[207];
static NJS_CNK_MODEL draw_218_model;
static NJS_CNK_OBJECT draw_218;
static Sint16 draw_217_polygons[217];
static Sint32 draw_217_vertices[231];
static NJS_CNK_MODEL draw_217_model;
static NJS_CNK_OBJECT draw_217;
static Sint16 draw_216_polygons[167];
static Sint32 draw_216_vertices[135];
static NJS_CNK_MODEL draw_216_model;
static NJS_CNK_OBJECT draw_216;
static Sint16 draw_215_polygons[69];
static Sint32 draw_215_vertices[35];
static NJS_CNK_MODEL draw_215_model;
static NJS_CNK_OBJECT draw_215;
static Sint16 draw_205_polygons[171];
static Sint32 draw_205_vertices[179];
static NJS_CNK_MODEL draw_205_model;
static NJS_CNK_OBJECT draw_214;
static Sint16 draw_204_polygons[235];
static Sint32 draw_204_vertices[211];
static NJS_CNK_MODEL draw_204_model;
static NJS_CNK_OBJECT draw_204;
static Sint16 draw_203_polygons[277];
static Sint32 draw_203_vertices[315];
static NJS_CNK_MODEL draw_203_model;
static NJS_CNK_OBJECT draw_203;
static Sint16 draw_202_polygons[145];
static Sint32 draw_202_vertices[87];
static NJS_CNK_MODEL draw_202_model;
static NJS_CNK_OBJECT draw_202;
static Sint16 draw_201_polygons[1319];
static Sint32 draw_201_vertices[1159];
static NJS_CNK_MODEL draw_201_model;
static NJS_CNK_OBJECT draw_201;
static Sint16 draw_199_polygons[23];
static Sint32 draw_199_vertices[19];
static NJS_CNK_MODEL draw_199_model;
static NJS_CNK_OBJECT draw_200;
static Sint16 draw_198_polygons[69];
static Sint32 draw_198_vertices[55];
static NJS_CNK_MODEL draw_198_model;
static NJS_CNK_OBJECT draw_198;
static Sint16 draw_195_polygons[139];
static Sint32 draw_195_vertices[131];
static NJS_CNK_MODEL draw_195_model;
static NJS_CNK_OBJECT draw_197;
static Sint16 draw_193_polygons[163];
static Sint32 draw_193_vertices[143];
static NJS_CNK_MODEL draw_193_model;
static NJS_CNK_OBJECT draw_194;
static Sint16 draw_179_polygons[23];
static Sint32 draw_179_vertices[19];
static NJS_CNK_MODEL draw_179_model;
static NJS_CNK_OBJECT draw_192;
static Sint16 draw_178_polygons[23];
static Sint32 draw_178_vertices[19];
static NJS_CNK_MODEL draw_178_model;
static NJS_CNK_OBJECT draw_178;
static Sint16 draw_177_polygons[23];
static Sint32 draw_177_vertices[19];
static NJS_CNK_MODEL draw_177_model;
static NJS_CNK_OBJECT draw_177;
static Sint16 draw_174_polygons[23];
static Sint32 draw_174_vertices[19];
static NJS_CNK_MODEL draw_174_model;
static NJS_CNK_OBJECT draw_176;
static Sint16 draw_173_polygons[23];
static Sint32 draw_173_vertices[19];
static NJS_CNK_MODEL draw_173_model;
static NJS_CNK_OBJECT draw_173;
static Sint16 draw_172_polygons[23];
static Sint32 draw_172_vertices[19];
static NJS_CNK_MODEL draw_172_model;
static NJS_CNK_OBJECT draw_172;
static Sint16 draw_166_polygons[23];
static Sint32 draw_166_vertices[19];
static NJS_CNK_MODEL draw_166_model;
static NJS_CNK_OBJECT draw_171;
static Sint16 draw_163_polygons[71];
static Sint32 draw_163_vertices[35];
static NJS_CNK_MODEL draw_163_model;
static NJS_CNK_OBJECT draw_165;
static Sint16 draw_161_polygons[79];
static Sint32 draw_161_vertices[83];
static NJS_CNK_MODEL draw_161_model;
static NJS_CNK_OBJECT draw_162;
static Sint16 draw_160_polygons[47];
static Sint32 draw_160_vertices[51];
static NJS_CNK_MODEL draw_160_model;
static NJS_CNK_OBJECT draw_160;
static Sint16 draw_158_polygons[833];
static Sint32 draw_158_vertices[843];
static NJS_CNK_MODEL draw_158_model;
static NJS_CNK_OBJECT draw_159;
static Sint16 draw_156_polygons[23];
static Sint32 draw_156_vertices[19];
static NJS_CNK_MODEL draw_156_model;
static NJS_CNK_OBJECT draw_157;
static Sint16 draw_154_polygons[23];
static Sint32 draw_154_vertices[19];
static NJS_CNK_MODEL draw_154_model;
static NJS_CNK_OBJECT draw_155;
static Sint16 draw_153_polygons[23];
static Sint32 draw_153_vertices[19];
static NJS_CNK_MODEL draw_153_model;
static NJS_CNK_OBJECT draw_153;
static Sint16 draw_152_polygons[23];
static Sint32 draw_152_vertices[19];
static NJS_CNK_MODEL draw_152_model;
static NJS_CNK_OBJECT draw_152;
static Sint16 draw_148_polygons[23];
static Sint32 draw_148_vertices[19];
static NJS_CNK_MODEL draw_148_model;
static NJS_CNK_OBJECT draw_151;
static Sint16 draw_147_polygons[37];
static Sint32 draw_147_vertices[27];
static NJS_CNK_MODEL draw_147_model;
static NJS_CNK_OBJECT draw_147;
static Sint16 draw_146_polygons[23];
static Sint32 draw_146_vertices[19];
static NJS_CNK_MODEL draw_146_model;
static NJS_CNK_OBJECT draw_146;
static Sint16 draw_145_polygons[35];
static Sint32 draw_145_vertices[35];
static NJS_CNK_MODEL draw_145_model;
static NJS_CNK_OBJECT draw_145;
static Sint16 draw_144_polygons[55];
static Sint32 draw_144_vertices[51];
static NJS_CNK_MODEL draw_144_model;
static NJS_CNK_OBJECT draw_144;
static Sint16 draw_143_polygons[59];
static Sint32 draw_143_vertices[67];
static NJS_CNK_MODEL draw_143_model;
static NJS_CNK_OBJECT draw_143;
static Sint16 draw_140_polygons[853];
static Sint32 draw_140_vertices[863];
static NJS_CNK_MODEL draw_140_model;
static NJS_CNK_OBJECT draw_142;
static Sint16 draw_139_polygons[47];
static Sint32 draw_139_vertices[51];
static NJS_CNK_MODEL draw_139_model;
static NJS_CNK_OBJECT draw_139;
static Sint16 draw_137_polygons[65];
static Sint32 draw_137_vertices[59];
static NJS_CNK_MODEL draw_137_model;
static NJS_CNK_OBJECT draw_138;
static Sint16 draw_136_polygons[79];
static Sint32 draw_136_vertices[67];
static NJS_CNK_MODEL draw_136_model;
static NJS_CNK_OBJECT draw_136;
static Sint16 draw_135_polygons[47];
static Sint32 draw_135_vertices[51];
static NJS_CNK_MODEL draw_135_model;
static NJS_CNK_OBJECT draw_135;
static Sint16 draw_133_polygons[23];
static Sint32 draw_133_vertices[19];
static NJS_CNK_MODEL draw_133_model;
static NJS_CNK_OBJECT draw_134;
static Sint16 draw_130_polygons[23];
static Sint32 draw_130_vertices[19];
static NJS_CNK_MODEL draw_130_model;
static NJS_CNK_OBJECT draw_132;
static Sint16 draw_124_polygons[1137];
static Sint32 draw_124_vertices[1103];
static NJS_CNK_MODEL draw_124_model;
static NJS_CNK_OBJECT draw_129;
static Sint16 draw_123_polygons[23];
static Sint32 draw_123_vertices[19];
static NJS_CNK_MODEL draw_123_model;
static NJS_CNK_OBJECT draw_123;
static Sint16 draw_121_polygons[49];
static Sint32 draw_121_vertices[35];
static NJS_CNK_MODEL draw_121_model;
static NJS_CNK_OBJECT draw_122;
static Sint16 draw_120_polygons[83];
static Sint32 draw_120_vertices[47];
static NJS_CNK_MODEL draw_120_model;
static NJS_CNK_OBJECT draw_120;
static Sint16 draw_119_polygons[55];
static Sint32 draw_119_vertices[51];
static NJS_CNK_MODEL draw_119_model;
static NJS_CNK_OBJECT draw_119;
static Sint16 draw_118_polygons[1285];
static Sint32 draw_118_vertices[1291];
static NJS_CNK_MODEL draw_118_model;
static NJS_CNK_OBJECT draw_118;
static Sint16 draw_117_polygons[261];
static Sint32 draw_117_vertices[255];
static NJS_CNK_MODEL draw_117_model;
static NJS_CNK_OBJECT draw_117;
static Sint16 draw_115_polygons[267];
static Sint32 draw_115_vertices[259];
static NJS_CNK_MODEL draw_115_model;
static NJS_CNK_OBJECT draw_116;
static Sint16 draw_111_polygons[87];
static Sint32 draw_111_vertices[67];
static NJS_CNK_MODEL draw_111_model;
static NJS_CNK_OBJECT draw_114;
static Sint16 draw_110_polygons[185];
static Sint32 draw_110_vertices[91];
static NJS_CNK_MODEL draw_110_model;
static NJS_CNK_OBJECT draw_110;
static Sint16 draw_109_polygons[83];
static Sint32 draw_109_vertices[63];
static NJS_CNK_MODEL draw_109_model;
static NJS_CNK_OBJECT draw_109;
static Sint16 draw_107_polygons[1659];
static Sint32 draw_107_vertices[1707];
static NJS_CNK_MODEL draw_107_model;
static NJS_CNK_OBJECT draw_108;
static Sint16 draw_105_polygons[235];
static Sint32 draw_105_vertices[155];
static NJS_CNK_MODEL draw_105_model;
static NJS_CNK_OBJECT draw_106;
static Sint16 draw_104_polygons[269];
static Sint32 draw_104_vertices[187];
static NJS_CNK_MODEL draw_104_model;
static NJS_CNK_OBJECT draw_104;
static Sint16 draw_100_polygons[157];
static Sint32 draw_100_vertices[71];
static NJS_CNK_MODEL draw_100_model;
static NJS_CNK_OBJECT draw_103;
static Sint16 draw_99_polygons[853];
static Sint32 draw_99_vertices[779];
static NJS_CNK_MODEL draw_99_model;
static NJS_CNK_OBJECT draw_99;
static Sint16 draw_98_polygons[1033];
static Sint32 draw_98_vertices[699];
static NJS_CNK_MODEL draw_98_model;
static NJS_CNK_OBJECT draw_98;
static Sint16 draw_96_polygons[243];
static Sint32 draw_96_vertices[119];
static NJS_CNK_MODEL draw_96_model;
static NJS_CNK_OBJECT draw_97;
static Sint16 draw_95_polygons[281];
static Sint32 draw_95_vertices[207];
static NJS_CNK_MODEL draw_95_model;
static NJS_CNK_OBJECT draw_95;
static Sint16 draw_91_polygons[1229];
static Sint32 draw_91_vertices[1059];
static NJS_CNK_MODEL draw_91_model;
static NJS_CNK_OBJECT draw_94;
static Sint16 draw_90_polygons[61];
static Sint32 draw_90_vertices[35];
static NJS_CNK_MODEL draw_90_model;
static NJS_CNK_OBJECT draw_90;
static Sint16 draw_89_polygons[585];
static Sint32 draw_89_vertices[355];
static NJS_CNK_MODEL draw_89_model;
static NJS_CNK_OBJECT draw_89;
static Sint16 draw_86_polygons[1323];
static Sint32 draw_86_vertices[1459];
static NJS_CNK_MODEL draw_86_model;
static NJS_CNK_OBJECT draw_88;
static Sint16 draw_82_polygons[35];
static Sint32 draw_82_vertices[35];
static NJS_CNK_MODEL draw_82_model;
static NJS_CNK_OBJECT draw_85;
static Sint16 draw_81_polygons[485];
static Sint32 draw_81_vertices[435];
static NJS_CNK_MODEL draw_81_model;
static NJS_CNK_OBJECT draw_81;
static Sint16 draw_80_polygons[23];
static Sint32 draw_80_vertices[19];
static NJS_CNK_MODEL draw_80_model;
static NJS_CNK_OBJECT draw_80;
static Sint16 draw_79_polygons[463];
static Sint32 draw_79_vertices[267];
static NJS_CNK_MODEL draw_79_model;
static NJS_CNK_OBJECT draw_79;
static Sint16 draw_74_polygons[23];
static Sint32 draw_74_vertices[19];
static NJS_CNK_MODEL draw_74_model;
static NJS_CNK_OBJECT draw_78;
static Sint16 draw_62_polygons[247];
static Sint32 draw_62_vertices[95];
static NJS_CNK_MODEL draw_62_model;
static NJS_CNK_OBJECT draw_73;
static Sint16 draw_56_polygons[429];
static Sint32 draw_56_vertices[475];
static NJS_CNK_MODEL draw_56_model;
static NJS_CNK_OBJECT draw_61;
static Sint16 draw_52_polygons[209];
static Sint32 draw_52_vertices[155];
static NJS_CNK_MODEL draw_52_model;
static NJS_CNK_OBJECT draw_55;
static Sint16 draw_50_polygons[247];
static Sint32 draw_50_vertices[139];
static NJS_CNK_MODEL draw_50_model;
static NJS_CNK_OBJECT draw_51;
static Sint16 draw_48_polygons[257];
static Sint32 draw_48_vertices[211];
static NJS_CNK_MODEL draw_48_model;
static NJS_CNK_OBJECT draw_49;
static Sint16 draw_45_polygons[159];
static Sint32 draw_45_vertices[147];
static NJS_CNK_MODEL draw_45_model;
static NJS_CNK_OBJECT draw_47;
static Sint16 draw_33_polygons[1737];
static Sint32 draw_33_vertices[1843];
static NJS_CNK_MODEL draw_33_model;
static NJS_CNK_OBJECT draw_44;
static Sint16 draw_32_polygons[89];
static Sint32 draw_32_vertices[75];
static NJS_CNK_MODEL draw_32_model;
static NJS_CNK_OBJECT draw_32;
static Sint16 draw_31_polygons[89];
static Sint32 draw_31_vertices[75];
static NJS_CNK_MODEL draw_31_model;
static NJS_CNK_OBJECT draw_31;
static Sint16 draw_30_polygons[403];
static Sint32 draw_30_vertices[227];
static NJS_CNK_MODEL draw_30_model;
static NJS_CNK_OBJECT draw_30;
static Sint16 draw_29_polygons[63];
static Sint32 draw_29_vertices[67];
static NJS_CNK_MODEL draw_29_model;
static NJS_CNK_OBJECT draw_29;
static Sint16 draw_27_polygons[181];
static Sint32 draw_27_vertices[131];
static NJS_CNK_MODEL draw_27_model;
static NJS_CNK_OBJECT draw_28;
static Sint16 draw_2_polygons[259];
static Sint32 draw_2_vertices[263];
static NJS_CNK_MODEL draw_2_model;
static NJS_CNK_OBJECT draw_26;
static Sint16 draw_0_polygons[55];
static Sint32 draw_0_vertices[31];
static NJS_CNK_MODEL draw_0_model;
static NJS_CNK_OBJECT draw_1;
static NJS_MATERIAL collision_0_materials[1];
static Sint16 collision_0_faces[10];
static NJS_MESHSET collision_0_meshsets[1];
static NJS_POINT3 collision_0_points[8];
static NJS_MODEL collision_0_model;
static NJS_OBJECT collision_0;
static NJS_MATERIAL collision_1_materials[1];
static Sint16 collision_1_faces[19];
static NJS_MESHSET collision_1_meshsets[1];
static NJS_POINT3 collision_1_points[16];
static NJS_MODEL collision_1_model;
static NJS_OBJECT collision_1;
static NJS_MATERIAL collision_2_materials[1];
static Sint16 collision_2_faces[19];
static NJS_MESHSET collision_2_meshsets[1];
static NJS_POINT3 collision_2_points[16];
static NJS_MODEL collision_2_model;
static NJS_OBJECT collision_2;
static NJS_MATERIAL collision_3_materials[1];
static Sint16 collision_3_faces[10];
static NJS_MESHSET collision_3_meshsets[1];
static NJS_POINT3 collision_3_points[8];
static NJS_MODEL collision_3_model;
static NJS_OBJECT collision_3;
static NJS_MATERIAL collision_36_materials[1];
static Sint16 collision_36_faces[10];
static NJS_MESHSET collision_36_meshsets[1];
static NJS_POINT3 collision_36_points[8];
static NJS_MODEL collision_36_model;
static NJS_OBJECT collision_36;
static NJS_MATERIAL collision_42_materials[1];
static Sint16 collision_42_faces[19];
static NJS_MESHSET collision_42_meshsets[1];
static NJS_POINT3 collision_42_points[16];
static NJS_MODEL collision_42_model;
static NJS_OBJECT collision_42;
static NJS_MATERIAL collision_43_materials[1];
static Sint16 collision_43_faces[90];
static NJS_MESHSET collision_43_meshsets[1];
static NJS_POINT3 collision_43_points[60];
static NJS_MODEL collision_43_model;
static NJS_OBJECT collision_43;
static NJS_MATERIAL collision_49_materials[1];
static Sint16 collision_49_faces[10];
static NJS_MESHSET collision_49_meshsets[1];
static NJS_POINT3 collision_49_points[8];
static NJS_MODEL collision_49_model;
static NJS_OBJECT collision_49;
static NJS_MATERIAL collision_51_materials[1];
static Sint16 collision_51_faces[114];
static NJS_MESHSET collision_51_meshsets[1];
static NJS_POINT3 collision_51_points[76];
static NJS_MODEL collision_51_model;
static NJS_OBJECT collision_51;
static NJS_MATERIAL collision_52_materials[1];
static Sint16 collision_52_faces[114];
static NJS_MESHSET collision_52_meshsets[1];
static NJS_POINT3 collision_52_points[76];
static NJS_MODEL collision_52_model;
static NJS_OBJECT collision_52;
static NJS_MATERIAL collision_55_materials[1];
static Sint16 collision_55_faces[10];
static NJS_MESHSET collision_55_meshsets[1];
static NJS_POINT3 collision_55_points[8];
static NJS_MODEL collision_55_model;
static NJS_OBJECT collision_55;
static NJS_MATERIAL collision_63_materials[1];
static Sint16 collision_63_faces[10];
static NJS_MESHSET collision_63_meshsets[1];
static NJS_POINT3 collision_63_points[8];
static NJS_MODEL collision_63_model;
static NJS_OBJECT collision_63;
static NJS_MATERIAL collision_64_materials[1];
static Sint16 collision_64_faces[5];
static NJS_MESHSET collision_64_meshsets[1];
static NJS_POINT3 collision_64_points[4];
static NJS_MODEL collision_64_model;
static NJS_OBJECT collision_64;
static NJS_MATERIAL collision_71_materials[1];
static Sint16 collision_71_faces[18];
static NJS_MESHSET collision_71_meshsets[1];
static NJS_POINT3 collision_71_points[16];
static NJS_MODEL collision_71_model;
static NJS_OBJECT collision_71;
static NJS_MATERIAL collision_73_materials[1];
static Sint16 collision_73_faces[22];
static NJS_MESHSET collision_73_meshsets[1];
static NJS_POINT3 collision_73_points[20];
static NJS_MODEL collision_73_model;
static NJS_OBJECT collision_73;
static NJS_MATERIAL collision_79_materials[1];
static Sint16 collision_79_faces[85];
static NJS_MESHSET collision_79_meshsets[1];
static NJS_POINT3 collision_79_points[68];
static NJS_MODEL collision_79_model;
static NJS_OBJECT collision_79;
static NJS_MATERIAL collision_81_materials[1];
static Sint16 collision_81_faces[22];
static NJS_MESHSET collision_81_meshsets[1];
static NJS_POINT3 collision_81_points[20];
static NJS_MODEL collision_81_model;
static NJS_OBJECT collision_81;
static NJS_MATERIAL collision_85_materials[1];
static Sint16 collision_85_faces[49];
static NJS_MESHSET collision_85_meshsets[1];
static NJS_POINT3 collision_85_points[34];
static NJS_MODEL collision_85_model;
static NJS_OBJECT collision_85;
static NJS_MATERIAL collision_88_materials[1];
static Sint16 collision_88_faces[22];
static NJS_MESHSET collision_88_meshsets[1];
static NJS_POINT3 collision_88_points[12];
static NJS_MODEL collision_88_model;
static NJS_OBJECT collision_88;
static NJS_MATERIAL collision_89_materials[1];
static Sint16 collision_89_faces[5];
static NJS_MESHSET collision_89_meshsets[1];
static NJS_POINT3 collision_89_points[4];
static NJS_MODEL collision_89_model;
static NJS_OBJECT collision_89;
static NJS_MATERIAL collision_100_materials[1];
static Sint16 collision_100_faces[27];
static NJS_MESHSET collision_100_meshsets[1];
static NJS_POINT3 collision_100_points[18];
static NJS_MODEL collision_100_model;
static NJS_OBJECT collision_100;
static NJS_MATERIAL collision_102_materials[1];
static Sint16 collision_102_faces[38];
static NJS_MESHSET collision_102_meshsets[1];
static NJS_POINT3 collision_102_points[16];
static NJS_MODEL collision_102_model;
static NJS_OBJECT collision_102;
static NJS_MATERIAL collision_103_materials[1];
static Sint16 collision_103_faces[40];
static NJS_MESHSET collision_103_meshsets[1];
static NJS_POINT3 collision_103_points[16];
static NJS_MODEL collision_103_model;
static NJS_OBJECT collision_103;
static NJS_MATERIAL collision_104_materials[1];
static Sint16 collision_104_faces[40];
static NJS_MESHSET collision_104_meshsets[1];
static NJS_POINT3 collision_104_points[16];
static NJS_MODEL collision_104_model;
static NJS_OBJECT collision_104;
static NJS_MATERIAL collision_105_materials[1];
static Sint16 collision_105_faces[40];
static NJS_MESHSET collision_105_meshsets[1];
static NJS_POINT3 collision_105_points[16];
static NJS_MODEL collision_105_model;
static NJS_OBJECT collision_105;
static NJS_MATERIAL collision_106_materials[1];
static Sint16 collision_106_faces[40];
static NJS_MESHSET collision_106_meshsets[1];
static NJS_POINT3 collision_106_points[16];
static NJS_MODEL collision_106_model;
static NJS_OBJECT collision_106;
static NJS_MATERIAL collision_107_materials[1];
static Sint16 collision_107_faces[40];
static NJS_MESHSET collision_107_meshsets[1];
static NJS_POINT3 collision_107_points[16];
static NJS_MODEL collision_107_model;
static NJS_OBJECT collision_107;
static NJS_MATERIAL collision_108_materials[1];
static Sint16 collision_108_faces[40];
static NJS_MESHSET collision_108_meshsets[1];
static NJS_POINT3 collision_108_points[16];
static NJS_MODEL collision_108_model;
static NJS_OBJECT collision_108;
static NJS_MATERIAL collision_109_materials[1];
static Sint16 collision_109_faces[19];
static NJS_MESHSET collision_109_meshsets[1];
static NJS_POINT3 collision_109_points[16];
static NJS_MODEL collision_109_model;
static NJS_OBJECT collision_109;
static NJS_MATERIAL collision_110_materials[1];
static Sint16 collision_110_faces[22];
static NJS_MESHSET collision_110_meshsets[1];
static NJS_POINT3 collision_110_points[12];
static NJS_MODEL collision_110_model;
static NJS_OBJECT collision_110;
static NJS_MATERIAL collision_113_materials[1];
static Sint16 collision_113_faces[55];
static NJS_MESHSET collision_113_meshsets[1];
static NJS_POINT3 collision_113_points[30];
static NJS_MODEL collision_113_model;
static NJS_OBJECT collision_113;
static NJS_MATERIAL collision_116_materials[1];
static Sint16 collision_116_faces[38];
static NJS_MESHSET collision_116_meshsets[1];
static NJS_POINT3 collision_116_points[16];
static NJS_MODEL collision_116_model;
static NJS_OBJECT collision_116;
static NJS_MATERIAL collision_117_materials[1];
static Sint16 collision_117_faces[166];
static NJS_MESHSET collision_117_meshsets[1];
static NJS_POINT3 collision_117_points[112];
static NJS_MODEL collision_117_model;
static NJS_OBJECT collision_117;
static NJS_MATERIAL collision_118_materials[1];
static Sint16 collision_118_faces[36];
static NJS_MESHSET collision_118_meshsets[1];
static NJS_POINT3 collision_118_points[16];
static NJS_MODEL collision_118_model;
static NJS_OBJECT collision_118;
static NJS_MATERIAL collision_119_materials[1];
static Sint16 collision_119_faces[15];
static NJS_MESHSET collision_119_meshsets[1];
static NJS_POINT3 collision_119_points[8];
static NJS_MODEL collision_119_model;
static NJS_OBJECT collision_119;
static NJS_MATERIAL collision_122_materials[1];
static Sint16 collision_122_faces[166];
static NJS_MESHSET collision_122_meshsets[1];
static NJS_POINT3 collision_122_points[112];
static NJS_MODEL collision_122_model;
static NJS_OBJECT collision_122;
static NJS_MATERIAL collision_124_materials[1];
static Sint16 collision_124_faces[17];
static NJS_MESHSET collision_124_meshsets[1];
static NJS_POINT3 collision_124_points[8];
static NJS_MODEL collision_124_model;
static NJS_OBJECT collision_124;
static NJS_MATERIAL collision_127_materials[1];
static Sint16 collision_127_faces[166];
static NJS_MESHSET collision_127_meshsets[1];
static NJS_POINT3 collision_127_points[112];
static NJS_MODEL collision_127_model;
static NJS_OBJECT collision_127;
static NJS_MATERIAL collision_129_materials[1];
static Sint16 collision_129_faces[55];
static NJS_MESHSET collision_129_meshsets[1];
static NJS_POINT3 collision_129_points[32];
static NJS_MODEL collision_129_model;
static NJS_OBJECT collision_129;
static NJS_MATERIAL collision_131_materials[1];
static Sint16 collision_131_faces[47];
static NJS_MESHSET collision_131_meshsets[1];
static NJS_POINT3 collision_131_points[28];
static NJS_MODEL collision_131_model;
static NJS_OBJECT collision_131;
static NJS_MATERIAL collision_133_materials[1];
static Sint16 collision_133_faces[27];
static NJS_MESHSET collision_133_meshsets[1];
static NJS_POINT3 collision_133_points[21];
static NJS_MODEL collision_133_model;
static NJS_OBJECT collision_133;
static NJS_MATERIAL collision_134_materials[1];
static Sint16 collision_134_faces[19];
static NJS_MESHSET collision_134_meshsets[1];
static NJS_POINT3 collision_134_points[16];
static NJS_MODEL collision_134_model;
static NJS_OBJECT collision_134;
static NJS_MATERIAL collision_148_materials[1];
static Sint16 collision_148_faces[19];
static NJS_MESHSET collision_148_meshsets[1];
static NJS_POINT3 collision_148_points[8];
static NJS_MODEL collision_148_model;
static NJS_OBJECT collision_148;
static NJS_MATERIAL collision_149_materials[1];
static Sint16 collision_149_faces[31];
static NJS_MESHSET collision_149_meshsets[1];
static NJS_POINT3 collision_149_points[18];
static NJS_MODEL collision_149_model;
static NJS_OBJECT collision_149;
static NJS_MATERIAL collision_150_materials[1];
static Sint16 collision_150_faces[19];
static NJS_MESHSET collision_150_meshsets[1];
static NJS_POINT3 collision_150_points[16];
static NJS_MODEL collision_150_model;
static NJS_OBJECT collision_150;
static NJS_MATERIAL collision_154_materials[1];
static Sint16 collision_154_faces[65];
static NJS_MESHSET collision_154_meshsets[1];
static NJS_POINT3 collision_154_points[39];
static NJS_MODEL collision_154_model;
static NJS_OBJECT collision_154;
static NJS_MATERIAL collision_155_materials[1];
static Sint16 collision_155_faces[33];
static NJS_MESHSET collision_155_meshsets[1];
static NJS_POINT3 collision_155_points[16];
static NJS_MODEL collision_155_model;
static NJS_OBJECT collision_155;
static NJS_MATERIAL collision_156_materials[1];
static Sint16 collision_156_faces[166];
static NJS_MESHSET collision_156_meshsets[1];
static NJS_POINT3 collision_156_points[112];
static NJS_MODEL collision_156_model;
static NJS_OBJECT collision_156;
static NJS_MATERIAL collision_157_materials[1];
static Sint16 collision_157_faces[10];
static NJS_MESHSET collision_157_meshsets[1];
static NJS_POINT3 collision_157_points[8];
static NJS_MODEL collision_157_model;
static NJS_OBJECT collision_157;
static NJS_MATERIAL collision_159_materials[1];
static Sint16 collision_159_faces[15];
static NJS_MESHSET collision_159_meshsets[1];
static NJS_POINT3 collision_159_points[8];
static NJS_MODEL collision_159_model;
static NJS_OBJECT collision_159;
static NJS_MATERIAL collision_160_materials[1];
static Sint16 collision_160_faces[15];
static NJS_MESHSET collision_160_meshsets[1];
static NJS_POINT3 collision_160_points[8];
static NJS_MODEL collision_160_model;
static NJS_OBJECT collision_160;
static NJS_MATERIAL collision_161_materials[1];
static Sint16 collision_161_faces[15];
static NJS_MESHSET collision_161_meshsets[1];
static NJS_POINT3 collision_161_points[8];
static NJS_MODEL collision_161_model;
static NJS_OBJECT collision_161;
static NJS_MATERIAL collision_162_materials[1];
static Sint16 collision_162_faces[15];
static NJS_MESHSET collision_162_meshsets[1];
static NJS_POINT3 collision_162_points[8];
static NJS_MODEL collision_162_model;
static NJS_OBJECT collision_162;
static NJS_MATERIAL collision_163_materials[1];
static Sint16 collision_163_faces[15];
static NJS_MESHSET collision_163_meshsets[1];
static NJS_POINT3 collision_163_points[8];
static NJS_MODEL collision_163_model;
static NJS_OBJECT collision_163;
static NJS_MATERIAL collision_164_materials[1];
static Sint16 collision_164_faces[61];
static NJS_MESHSET collision_164_meshsets[1];
static NJS_POINT3 collision_164_points[16];
static NJS_MODEL collision_164_model;
static NJS_OBJECT collision_164;
static NJS_MATERIAL collision_165_materials[1];
static Sint16 collision_165_faces[19];
static NJS_MESHSET collision_165_meshsets[1];
static NJS_POINT3 collision_165_points[8];
static NJS_MODEL collision_165_model;
static NJS_OBJECT collision_165;
static NJS_MATERIAL collision_166_materials[1];
static Sint16 collision_166_faces[114];
static NJS_MESHSET collision_166_meshsets[1];
static NJS_POINT3 collision_166_points[76];
static NJS_MODEL collision_166_model;
static NJS_OBJECT collision_166;
static NJS_MATERIAL collision_167_materials[1];
static Sint16 collision_167_faces[19];
static NJS_MESHSET collision_167_meshsets[1];
static NJS_POINT3 collision_167_points[8];
static NJS_MODEL collision_167_model;
static NJS_OBJECT collision_167;
static NJS_MATERIAL collision_168_materials[1];
static Sint16 collision_168_faces[19];
static NJS_MESHSET collision_168_meshsets[1];
static NJS_POINT3 collision_168_points[8];
static NJS_MODEL collision_168_model;
static NJS_OBJECT collision_168;
static NJS_MATERIAL collision_169_materials[1];
static Sint16 collision_169_faces[14];
static NJS_MESHSET collision_169_meshsets[1];
static NJS_POINT3 collision_169_points[8];
static NJS_MODEL collision_169_model;
static NJS_OBJECT collision_169;
static NJS_MATERIAL collision_170_materials[1];
static Sint16 collision_170_faces[19];
static NJS_MESHSET collision_170_meshsets[1];
static NJS_POINT3 collision_170_points[8];
static NJS_MODEL collision_170_model;
static NJS_OBJECT collision_170;
static NJS_MATERIAL collision_171_materials[1];
static Sint16 collision_171_faces[19];
static NJS_MESHSET collision_171_meshsets[1];
static NJS_POINT3 collision_171_points[8];
static NJS_MODEL collision_171_model;
static NJS_OBJECT collision_171;
static NJS_MATERIAL collision_172_materials[1];
static Sint16 collision_172_faces[19];
static NJS_MESHSET collision_172_meshsets[1];
static NJS_POINT3 collision_172_points[8];
static NJS_MODEL collision_172_model;
static NJS_OBJECT collision_172;
static NJS_MATERIAL collision_173_materials[1];
static Sint16 collision_173_faces[19];
static NJS_MESHSET collision_173_meshsets[1];
static NJS_POINT3 collision_173_points[8];
static NJS_MODEL collision_173_model;
static NJS_OBJECT collision_173;
static NJS_MATERIAL collision_174_materials[1];
static Sint16 collision_174_faces[14];
static NJS_MESHSET collision_174_meshsets[1];
static NJS_POINT3 collision_174_points[8];
static NJS_MODEL collision_174_model;
static NJS_OBJECT collision_174;
static NJS_MATERIAL collision_175_materials[1];
static Sint16 collision_175_faces[19];
static NJS_MESHSET collision_175_meshsets[1];
static NJS_POINT3 collision_175_points[8];
static NJS_MODEL collision_175_model;
static NJS_OBJECT collision_175;
static NJS_MATERIAL collision_176_materials[1];
static Sint16 collision_176_faces[19];
static NJS_MESHSET collision_176_meshsets[1];
static NJS_POINT3 collision_176_points[8];
static NJS_MODEL collision_176_model;
static NJS_OBJECT collision_176;
static NJS_MATERIAL collision_177_materials[1];
static Sint16 collision_177_faces[19];
static NJS_MESHSET collision_177_meshsets[1];
static NJS_POINT3 collision_177_points[8];
static NJS_MODEL collision_177_model;
static NJS_OBJECT collision_177;
static NJS_MATERIAL collision_178_materials[1];
static Sint16 collision_178_faces[19];
static NJS_MESHSET collision_178_meshsets[1];
static NJS_POINT3 collision_178_points[8];
static NJS_MODEL collision_178_model;
static NJS_OBJECT collision_178;
static NJS_MATERIAL collision_179_materials[1];
static Sint16 collision_179_faces[15];
static NJS_MESHSET collision_179_meshsets[1];
static NJS_POINT3 collision_179_points[8];
static NJS_MODEL collision_179_model;
static NJS_OBJECT collision_179;
static NJS_MATERIAL collision_180_materials[1];
static Sint16 collision_180_faces[14];
static NJS_MESHSET collision_180_meshsets[1];
static NJS_POINT3 collision_180_points[8];
static NJS_MODEL collision_180_model;
static NJS_OBJECT collision_180;
static NJS_MATERIAL collision_181_materials[1];
static Sint16 collision_181_faces[33];
static NJS_MESHSET collision_181_meshsets[1];
static NJS_POINT3 collision_181_points[12];
static NJS_MODEL collision_181_model;
static NJS_OBJECT collision_181;
static NJS_MATERIAL collision_182_materials[1];
static Sint16 collision_182_faces[14];
static NJS_MESHSET collision_182_meshsets[1];
static NJS_POINT3 collision_182_points[8];
static NJS_MODEL collision_182_model;
static NJS_OBJECT collision_182;
static NJS_MATERIAL collision_183_materials[1];
static Sint16 collision_183_faces[63];
static NJS_MESHSET collision_183_meshsets[1];
static NJS_POINT3 collision_183_points[30];
static NJS_MODEL collision_183_model;
static NJS_OBJECT collision_183;
static NJS_MATERIAL collision_184_materials[1];
static Sint16 collision_184_faces[28];
static NJS_MESHSET collision_184_meshsets[1];
static NJS_POINT3 collision_184_points[16];
static NJS_MODEL collision_184_model;
static NJS_OBJECT collision_184;
static NJS_MATERIAL collision_186_materials[1];
static Sint16 collision_186_faces[15];
static NJS_MESHSET collision_186_meshsets[1];
static NJS_POINT3 collision_186_points[12];
static NJS_MODEL collision_186_model;
static NJS_OBJECT collision_186;
static NJS_MATERIAL collision_187_materials[1];
static Sint16 collision_187_faces[59];
static NJS_MESHSET collision_187_meshsets[1];
static NJS_POINT3 collision_187_points[34];
static NJS_MODEL collision_187_model;
static NJS_OBJECT collision_187;
static NJS_MATERIAL collision_188_materials[1];
static Sint16 collision_188_faces[42];
static NJS_MESHSET collision_188_meshsets[1];
static NJS_POINT3 collision_188_points[17];
static NJS_MODEL collision_188_model;
static NJS_OBJECT collision_188;
static NJS_MATERIAL collision_190_materials[1];
static Sint16 collision_190_faces[189];
static NJS_MESHSET collision_190_meshsets[1];
static NJS_POINT3 collision_190_points[72];
static NJS_MODEL collision_190_model;
static NJS_OBJECT collision_190;
static NJS_MATERIAL collision_196_materials[1];
static Sint16 collision_196_faces[114];
static NJS_MESHSET collision_196_meshsets[1];
static NJS_POINT3 collision_196_points[76];
static NJS_MODEL collision_196_model;
static NJS_OBJECT collision_196;
static NJS_MATERIAL collision_199_materials[1];
static Sint16 collision_199_faces[19];
static NJS_MESHSET collision_199_meshsets[1];
static NJS_POINT3 collision_199_points[16];
static NJS_MODEL collision_199_model;
static NJS_OBJECT collision_199;
static NJS_MATERIAL collision_200_materials[1];
static Sint16 collision_200_faces[33];
static NJS_MESHSET collision_200_meshsets[1];
static NJS_POINT3 collision_200_points[16];
static NJS_MODEL collision_200_model;
static NJS_OBJECT collision_200;
static NJS_MATERIAL collision_201_materials[1];
static Sint16 collision_201_faces[37];
static NJS_MESHSET collision_201_meshsets[1];
static NJS_POINT3 collision_201_points[16];
static NJS_MODEL collision_201_model;
static NJS_OBJECT collision_201;
static NJS_MATERIAL collision_202_materials[1];
static Sint16 collision_202_faces[37];
static NJS_MESHSET collision_202_meshsets[1];
static NJS_POINT3 collision_202_points[16];
static NJS_MODEL collision_202_model;
static NJS_OBJECT collision_202;
static NJS_MATERIAL collision_203_materials[1];
static Sint16 collision_203_faces[33];
static NJS_MESHSET collision_203_meshsets[1];
static NJS_POINT3 collision_203_points[16];
static NJS_MODEL collision_203_model;
static NJS_OBJECT collision_203;
static NJS_MATERIAL collision_204_materials[1];
static Sint16 collision_204_faces[14];
static NJS_MESHSET collision_204_meshsets[1];
static NJS_POINT3 collision_204_points[8];
static NJS_MODEL collision_204_model;
static NJS_OBJECT collision_204;
static NJS_MATERIAL collision_205_materials[1];
static Sint16 collision_205_faces[33];
static NJS_MESHSET collision_205_meshsets[1];
static NJS_POINT3 collision_205_points[16];
static NJS_MODEL collision_205_model;
static NJS_OBJECT collision_205;
static NJS_MATERIAL collision_206_materials[1];
static Sint16 collision_206_faces[33];
static NJS_MESHSET collision_206_meshsets[1];
static NJS_POINT3 collision_206_points[12];
static NJS_MODEL collision_206_model;
static NJS_OBJECT collision_206;
static NJS_MATERIAL collision_208_materials[1];
static Sint16 collision_208_faces[90];
static NJS_MESHSET collision_208_meshsets[1];
static NJS_POINT3 collision_208_points[40];
static NJS_MODEL collision_208_model;
static NJS_OBJECT collision_208;
static NJS_MATERIAL collision_210_materials[1];
static Sint16 collision_210_faces[48];
static NJS_MESHSET collision_210_meshsets[1];
static NJS_POINT3 collision_210_points[26];
static NJS_MODEL collision_210_model;
static NJS_OBJECT collision_210;
static NJS_MATERIAL collision_211_materials[1];
static Sint16 collision_211_faces[55];
static NJS_MESHSET collision_211_meshsets[1];
static NJS_POINT3 collision_211_points[40];
static NJS_MODEL collision_211_model;
static NJS_OBJECT collision_211;
static NJS_MATERIAL collision_215_materials[1];
static Sint16 collision_215_faces[28];
static NJS_MESHSET collision_215_meshsets[1];
static NJS_POINT3 collision_215_points[16];
static NJS_MODEL collision_215_model;
static NJS_OBJECT collision_215;
static NJS_MATERIAL collision_216_materials[1];
static Sint16 collision_216_faces[61];
static NJS_MESHSET collision_216_meshsets[1];
static NJS_POINT3 collision_216_points[38];
static NJS_MODEL collision_216_model;
static NJS_OBJECT collision_216;
static NJS_MATERIAL collision_217_materials[1];
static Sint16 collision_217_faces[113];
static NJS_MESHSET collision_217_meshsets[1];
static NJS_POINT3 collision_217_points[62];
static NJS_MODEL collision_217_model;
static NJS_OBJECT collision_217;
static NJS_MATERIAL collision_218_materials[1];
static Sint16 collision_218_faces[39];
static NJS_MESHSET collision_218_meshsets[1];
static NJS_POINT3 collision_218_points[16];
static NJS_MODEL collision_218_model;
static NJS_OBJECT collision_218;
static NJS_MATERIAL collision_219_materials[1];
static Sint16 collision_219_faces[103];
static NJS_MESHSET collision_219_meshsets[1];
static NJS_POINT3 collision_219_points[55];
static NJS_MODEL collision_219_model;
static NJS_OBJECT collision_219;
static NJS_MATERIAL collision_220_materials[1];
static Sint16 collision_220_faces[34];
static NJS_MESHSET collision_220_meshsets[1];
static NJS_POINT3 collision_220_points[16];
static NJS_MODEL collision_220_model;
static NJS_OBJECT collision_220;
static NJS_MATERIAL collision_224_materials[1];
static Sint16 collision_224_faces[106];
static NJS_MESHSET collision_224_meshsets[1];
static NJS_POINT3 collision_224_points[53];
static NJS_MODEL collision_224_model;
static NJS_OBJECT collision_224;
static NJS_MATERIAL collision_225_materials[1];
static Sint16 collision_225_faces[15];
static NJS_MESHSET collision_225_meshsets[1];
static NJS_POINT3 collision_225_points[8];
static NJS_MODEL collision_225_model;
static NJS_OBJECT collision_225;
static NJS_MATERIAL collision_226_materials[1];
static Sint16 collision_226_faces[45];
static NJS_MESHSET collision_226_meshsets[1];
static NJS_POINT3 collision_226_points[30];
static NJS_MODEL collision_226_model;
static NJS_OBJECT collision_226;
static NJS_MATERIAL collision_227_materials[1];
static Sint16 collision_227_faces[14];
static NJS_MESHSET collision_227_meshsets[1];
static NJS_POINT3 collision_227_points[8];
static NJS_MODEL collision_227_model;
static NJS_OBJECT collision_227;
static NJS_MATERIAL collision_228_materials[1];
static Sint16 collision_228_faces[18];
static NJS_MESHSET collision_228_meshsets[1];
static NJS_POINT3 collision_228_points[12];
static NJS_MODEL collision_228_model;
static NJS_OBJECT collision_228;
static NJS_MATERIAL collision_229_materials[1];
static Sint16 collision_229_faces[5];
static NJS_MESHSET collision_229_meshsets[1];
static NJS_POINT3 collision_229_points[4];
static NJS_MODEL collision_229_model;
static NJS_OBJECT collision_229;
static NJS_MATERIAL collision_230_materials[1];
static Sint16 collision_230_faces[62];
static NJS_MESHSET collision_230_meshsets[1];
static NJS_POINT3 collision_230_points[45];
static NJS_MODEL collision_230_model;
static NJS_OBJECT collision_230;
static NJS_MATERIAL collision_231_materials[1];
static Sint16 collision_231_faces[63];
static NJS_MESHSET collision_231_meshsets[1];
static NJS_POINT3 collision_231_points[40];
static NJS_MODEL collision_231_model;
static NJS_OBJECT collision_231;
static NJS_MATERIAL collision_233_materials[1];
static Sint16 collision_233_faces[21];
static NJS_MESHSET collision_233_meshsets[1];
static NJS_POINT3 collision_233_points[20];
static NJS_MODEL collision_233_model;
static NJS_OBJECT collision_233;
static NJS_MATERIAL collision_234_materials[1];
static Sint16 collision_234_faces[80];
static NJS_MESHSET collision_234_meshsets[1];
static NJS_POINT3 collision_234_points[35];
static NJS_MODEL collision_234_model;
static NJS_OBJECT collision_234;
static NJS_MATERIAL collision_235_materials[1];
static Sint16 collision_235_faces[9];
static NJS_MESHSET collision_235_meshsets[1];
static NJS_POINT3 collision_235_points[8];
static NJS_MODEL collision_235_model;
static NJS_OBJECT collision_235;
static NJS_MATERIAL collision_236_materials[1];
static Sint16 collision_236_faces[27];
static NJS_MESHSET collision_236_meshsets[1];
static NJS_POINT3 collision_236_points[16];
static NJS_MODEL collision_236_model;
static NJS_OBJECT collision_236;
static NJS_MATERIAL collision_237_materials[1];
static Sint16 collision_237_faces[9];
static NJS_MESHSET collision_237_meshsets[1];
static NJS_POINT3 collision_237_points[8];
static NJS_MODEL collision_237_model;
static NJS_OBJECT collision_237;
static NJS_MATERIAL collision_238_materials[1];
static Sint16 collision_238_faces[14];
static NJS_MESHSET collision_238_meshsets[1];
static NJS_POINT3 collision_238_points[12];
static NJS_MODEL collision_238_model;
static NJS_OBJECT collision_238;
static NJS_MATERIAL collision_239_materials[1];
static Sint16 collision_239_faces[14];
static NJS_MESHSET collision_239_meshsets[1];
static NJS_POINT3 collision_239_points[8];
static NJS_MODEL collision_239_model;
static NJS_OBJECT collision_239;
static NJS_MATERIAL collision_240_materials[1];
static Sint16 collision_240_faces[14];
static NJS_MESHSET collision_240_meshsets[1];
static NJS_POINT3 collision_240_points[8];
static NJS_MODEL collision_240_model;
static NJS_OBJECT collision_240;
static NJS_MATERIAL collision_241_materials[1];
static Sint16 collision_241_faces[14];
static NJS_MESHSET collision_241_meshsets[1];
static NJS_POINT3 collision_241_points[8];
static NJS_MODEL collision_241_model;
static NJS_OBJECT collision_241;
static NJS_MATERIAL collision_242_materials[1];
static Sint16 collision_242_faces[14];
static NJS_MESHSET collision_242_meshsets[1];
static NJS_POINT3 collision_242_points[8];
static NJS_MODEL collision_242_model;
static NJS_OBJECT collision_242;
static NJS_MATERIAL collision_243_materials[1];
static Sint16 collision_243_faces[14];
static NJS_MESHSET collision_243_meshsets[1];
static NJS_POINT3 collision_243_points[8];
static NJS_MODEL collision_243_model;
static NJS_OBJECT collision_243;
static NJS_MATERIAL collision_244_materials[1];
static Sint16 collision_244_faces[14];
static NJS_MESHSET collision_244_meshsets[1];
static NJS_POINT3 collision_244_points[8];
static NJS_MODEL collision_244_model;
static NJS_OBJECT collision_244;
static NJS_MATERIAL collision_245_materials[1];
static Sint16 collision_245_faces[45];
static NJS_MESHSET collision_245_meshsets[1];
static NJS_POINT3 collision_245_points[32];
static NJS_MODEL collision_245_model;
static NJS_OBJECT collision_245;
static NJS_MATERIAL collision_247_materials[1];
static Sint16 collision_247_faces[81];
static NJS_MESHSET collision_247_meshsets[1];
static NJS_POINT3 collision_247_points[52];
static NJS_MODEL collision_247_model;
static NJS_OBJECT collision_247;
static NJS_MATERIAL collision_248_materials[1];
static Sint16 collision_248_faces[81];
static NJS_MESHSET collision_248_meshsets[1];
static NJS_POINT3 collision_248_points[52];
static NJS_MODEL collision_248_model;
static NJS_OBJECT collision_248;
static NJS_MATERIAL collision_249_materials[1];
static Sint16 collision_249_faces[81];
static NJS_MESHSET collision_249_meshsets[1];
static NJS_POINT3 collision_249_points[52];
static NJS_MODEL collision_249_model;
static NJS_OBJECT collision_249;
static NJS_MATERIAL collision_250_materials[1];
static Sint16 collision_250_faces[64];
static NJS_MESHSET collision_250_meshsets[1];
static NJS_POINT3 collision_250_points[24];
static NJS_MODEL collision_250_model;
static NJS_OBJECT collision_250;
static NJS_MATERIAL collision_253_materials[1];
static Sint16 collision_253_faces[27];
static NJS_MESHSET collision_253_meshsets[1];
static NJS_POINT3 collision_253_points[16];
static NJS_MODEL collision_253_model;
static NJS_OBJECT collision_253;
static NJS_MATERIAL collision_254_materials[1];
static Sint16 collision_254_faces[81];
static NJS_MESHSET collision_254_meshsets[1];
static NJS_POINT3 collision_254_points[52];
static NJS_MODEL collision_254_model;
static NJS_OBJECT collision_254;
static NJS_MATERIAL collision_255_materials[1];
static Sint16 collision_255_faces[55];
static NJS_MESHSET collision_255_meshsets[1];
static NJS_POINT3 collision_255_points[40];
static NJS_MODEL collision_255_model;
static NJS_OBJECT collision_255;
static NJS_MATERIAL collision_261_materials[1];
static Sint16 collision_261_faces[5];
static NJS_MESHSET collision_261_meshsets[1];
static NJS_POINT3 collision_261_points[4];
static NJS_MODEL collision_261_model;
static NJS_OBJECT collision_261;
static NJS_MATERIAL collision_262_materials[1];
static Sint16 collision_262_faces[45];
static NJS_MESHSET collision_262_meshsets[1];
static NJS_POINT3 collision_262_points[24];
static NJS_MODEL collision_262_model;
static NJS_OBJECT collision_262;
static NJS_MATERIAL collision_263_materials[1];
static Sint16 collision_263_faces[18];
static NJS_MESHSET collision_263_meshsets[1];
static NJS_POINT3 collision_263_points[8];
static NJS_MODEL collision_263_model;
static NJS_OBJECT collision_263;
static NJS_MATERIAL collision_264_materials[1];
static Sint16 collision_264_faces[205];
static NJS_MESHSET collision_264_meshsets[1];
static NJS_POINT3 collision_264_points[89];
static NJS_MODEL collision_264_model;
static NJS_OBJECT collision_264;
static NJS_MATERIAL collision_266_materials[1];
static Sint16 collision_266_faces[17];
static NJS_MESHSET collision_266_meshsets[1];
static NJS_POINT3 collision_266_points[8];
static NJS_MODEL collision_266_model;
static NJS_OBJECT collision_266;
static NJS_MATERIAL collision_268_materials[1];
static Sint16 collision_268_faces[25];
static NJS_MESHSET collision_268_meshsets[1];
static NJS_POINT3 collision_268_points[15];
static NJS_MODEL collision_268_model;
static NJS_OBJECT collision_268;
static NJS_MATERIAL collision_269_materials[1];
static Sint16 collision_269_faces[60];
static NJS_MESHSET collision_269_meshsets[1];
static NJS_POINT3 collision_269_points[28];
static NJS_MODEL collision_269_model;
static NJS_OBJECT collision_269;
static NJS_MATERIAL collision_271_materials[1];
static Sint16 collision_271_faces[42];
static NJS_MESHSET collision_271_meshsets[1];
static NJS_POINT3 collision_271_points[20];
static NJS_MODEL collision_271_model;
static NJS_OBJECT collision_271;
static NJS_MATERIAL collision_272_materials[1];
static Sint16 collision_272_faces[66];
static NJS_MESHSET collision_272_meshsets[1];
static NJS_POINT3 collision_272_points[28];
static NJS_MODEL collision_272_model;
static NJS_OBJECT collision_272;
static NJS_MATERIAL collision_273_materials[1];
static Sint16 collision_273_faces[140];
static NJS_MESHSET collision_273_meshsets[1];
static NJS_POINT3 collision_273_points[80];
static NJS_MODEL collision_273_model;
static NJS_OBJECT collision_273;
static NJS_MATERIAL collision_276_materials[1];
static Sint16 collision_276_faces[49];
static NJS_MESHSET collision_276_meshsets[1];
static NJS_POINT3 collision_276_points[16];
static NJS_MODEL collision_276_model;
static NJS_OBJECT collision_276;
static NJS_MATERIAL collision_288_materials[1];
static Sint16 collision_288_faces[36];
static NJS_MESHSET collision_288_meshsets[1];
static NJS_POINT3 collision_288_points[17];
static NJS_MODEL collision_288_model;
static NJS_OBJECT collision_288;
static NJS_MATERIAL collision_289_materials[1];
static Sint16 collision_289_faces[22];
static NJS_MESHSET collision_289_meshsets[1];
static NJS_POINT3 collision_289_points[16];
static NJS_MODEL collision_289_model;
static NJS_OBJECT collision_289;
static NJS_MATERIAL collision_295_materials[1];
static Sint16 collision_295_faces[47];
static NJS_MESHSET collision_295_meshsets[1];
static NJS_POINT3 collision_295_points[16];
static NJS_MODEL collision_295_model;
static NJS_OBJECT collision_295;
static NJS_MATERIAL collision_297_materials[1];
static Sint16 collision_297_faces[11];
static NJS_MESHSET collision_297_meshsets[1];
static NJS_POINT3 collision_297_points[8];
static NJS_MODEL collision_297_model;
static NJS_OBJECT collision_297;
static NJS_MATERIAL collision_299_materials[1];
static Sint16 collision_299_faces[81];
static NJS_MESHSET collision_299_meshsets[1];
static NJS_POINT3 collision_299_points[52];
static NJS_MODEL collision_299_model;
static NJS_OBJECT collision_299;
static NJS_MATERIAL collision_300_materials[1];
static Sint16 collision_300_faces[81];
static NJS_MESHSET collision_300_meshsets[1];
static NJS_POINT3 collision_300_points[52];
static NJS_MODEL collision_300_model;
static NJS_OBJECT collision_300;
static NJS_MATERIAL collision_301_materials[1];
static Sint16 collision_301_faces[81];
static NJS_MESHSET collision_301_meshsets[1];
static NJS_POINT3 collision_301_points[52];
static NJS_MODEL collision_301_model;
static NJS_OBJECT collision_301;
static NJS_MATERIAL collision_302_materials[1];
static Sint16 collision_302_faces[81];
static NJS_MESHSET collision_302_meshsets[1];
static NJS_POINT3 collision_302_points[52];
static NJS_MODEL collision_302_model;
static NJS_OBJECT collision_302;
static NJS_MATERIAL collision_303_materials[1];
static Sint16 collision_303_faces[81];
static NJS_MESHSET collision_303_meshsets[1];
static NJS_POINT3 collision_303_points[52];
static NJS_MODEL collision_303_model;
static NJS_OBJECT collision_303;
static NJS_MATERIAL collision_304_materials[1];
static Sint16 collision_304_faces[81];
static NJS_MESHSET collision_304_meshsets[1];
static NJS_POINT3 collision_304_points[52];
static NJS_MODEL collision_304_model;
static NJS_OBJECT collision_304;
static NJS_MATERIAL collision_305_materials[1];
static Sint16 collision_305_faces[81];
static NJS_MESHSET collision_305_meshsets[1];
static NJS_POINT3 collision_305_points[52];
static NJS_MODEL collision_305_model;
static NJS_OBJECT collision_305;
static NJS_MATERIAL collision_306_materials[1];
static Sint16 collision_306_faces[81];
static NJS_MESHSET collision_306_meshsets[1];
static NJS_POINT3 collision_306_points[52];
static NJS_MODEL collision_306_model;
static NJS_OBJECT collision_306;
static NJS_MATERIAL collision_307_materials[1];
static Sint16 collision_307_faces[9];
static NJS_MESHSET collision_307_meshsets[1];
static NJS_POINT3 collision_307_points[8];
static NJS_MODEL collision_307_model;
static NJS_OBJECT collision_307;
static NJS_MATERIAL collision_308_materials[1];
static Sint16 collision_308_faces[9];
static NJS_MESHSET collision_308_meshsets[1];
static NJS_POINT3 collision_308_points[8];
static NJS_MODEL collision_308_model;
static NJS_OBJECT collision_308;
static NJS_MATERIAL collision_309_materials[1];
static Sint16 collision_309_faces[42];
static NJS_MESHSET collision_309_meshsets[1];
static NJS_POINT3 collision_309_points[17];
static NJS_MODEL collision_309_model;
static NJS_OBJECT collision_309;
static NJS_MATERIAL collision_311_materials[1];
static Sint16 collision_311_faces[14];
static NJS_MESHSET collision_311_meshsets[1];
static NJS_POINT3 collision_311_points[8];
static NJS_MODEL collision_311_model;
static NJS_OBJECT collision_311;
static NJS_CNK_OBJECT draw_444;
static NJS_CNK_OBJECT draw_443;
static NJS_CNK_OBJECT draw_442;
static NJS_CNK_OBJECT draw_441;
static NJS_CNK_OBJECT draw_440;
static NJS_CNK_OBJECT draw_439;
static NJS_CNK_OBJECT draw_438;
static NJS_CNK_OBJECT draw_437;
static NJS_CNK_OBJECT draw_436;
static NJS_CNK_OBJECT draw_435;
static NJS_CNK_OBJECT draw_434;
static NJS_CNK_OBJECT draw_433;
static NJS_CNK_OBJECT draw_432;
static NJS_CNK_OBJECT draw_431;
static NJS_CNK_OBJECT draw_430;
static NJS_CNK_OBJECT draw_429;
static NJS_CNK_OBJECT draw_428;
static NJS_CNK_OBJECT draw_427;
static NJS_CNK_OBJECT draw_426;
static NJS_CNK_OBJECT draw_425;
static NJS_CNK_OBJECT draw_424;
static NJS_CNK_OBJECT draw_423;
static NJS_CNK_OBJECT draw_422;
static NJS_CNK_OBJECT draw_421;
static NJS_CNK_OBJECT draw_420;
static NJS_CNK_OBJECT draw_418;
static NJS_CNK_OBJECT draw_417;
static NJS_CNK_OBJECT draw_415;
static NJS_CNK_OBJECT draw_414;
static NJS_CNK_OBJECT draw_410;
static NJS_CNK_OBJECT draw_409;
static NJS_CNK_OBJECT draw_407;
static NJS_CNK_OBJECT draw_406;
static NJS_CNK_OBJECT draw_405;
static NJS_CNK_OBJECT draw_404;
static NJS_CNK_OBJECT draw_403;
static NJS_CNK_OBJECT draw_402;
static NJS_CNK_OBJECT draw_401;
static NJS_CNK_OBJECT draw_400;
static NJS_CNK_OBJECT draw_399;
static NJS_CNK_OBJECT draw_398;
static NJS_CNK_OBJECT draw_397;
static NJS_CNK_OBJECT draw_396;
static NJS_CNK_OBJECT draw_395;
static NJS_CNK_OBJECT draw_394;
static NJS_CNK_OBJECT draw_393;
static NJS_CNK_OBJECT draw_392;
static NJS_CNK_OBJECT draw_391;
static NJS_CNK_OBJECT draw_390;
static NJS_CNK_OBJECT draw_389;
static NJS_CNK_OBJECT draw_388;
static NJS_CNK_OBJECT draw_387;
static NJS_CNK_OBJECT draw_386;
static NJS_CNK_OBJECT draw_385;
static NJS_CNK_OBJECT draw_384;
static NJS_CNK_OBJECT draw_383;
static NJS_CNK_OBJECT draw_382;
static NJS_CNK_OBJECT draw_381;
static NJS_CNK_OBJECT draw_380;
static NJS_CNK_OBJECT draw_379;
static NJS_CNK_OBJECT draw_378;
static NJS_CNK_OBJECT draw_377;
static NJS_CNK_OBJECT draw_376;
static NJS_CNK_OBJECT draw_374;
static NJS_CNK_OBJECT draw_373;
static NJS_CNK_OBJECT draw_371;
static NJS_CNK_OBJECT draw_370;
static NJS_CNK_OBJECT draw_369;
static NJS_CNK_OBJECT draw_366;
static NJS_CNK_OBJECT draw_365;
static NJS_CNK_OBJECT draw_364;
static NJS_CNK_OBJECT draw_362;
static NJS_CNK_OBJECT draw_361;
static NJS_CNK_OBJECT draw_359;
static NJS_CNK_OBJECT draw_357;
static NJS_CNK_OBJECT draw_356;
static NJS_CNK_OBJECT draw_355;
static NJS_CNK_OBJECT draw_353;
static NJS_CNK_OBJECT draw_352;
static NJS_CNK_OBJECT draw_348;
static NJS_CNK_OBJECT draw_346;
static NJS_CNK_OBJECT draw_344;
static NJS_CNK_OBJECT draw_343;
static NJS_CNK_OBJECT draw_342;
static NJS_CNK_OBJECT draw_341;
static NJS_CNK_OBJECT draw_340;
static NJS_CNK_OBJECT draw_339;
static NJS_CNK_OBJECT draw_338;
static NJS_CNK_OBJECT draw_335;
static NJS_CNK_OBJECT draw_334;
static NJS_CNK_OBJECT draw_332;
static NJS_CNK_OBJECT draw_331;
static NJS_CNK_OBJECT draw_328;
static NJS_CNK_OBJECT draw_327;
static NJS_CNK_OBJECT draw_325;
static NJS_CNK_OBJECT draw_324;
static NJS_CNK_OBJECT draw_321;
static NJS_CNK_OBJECT draw_319;
static NJS_CNK_OBJECT draw_318;
static NJS_CNK_OBJECT draw_317;
static NJS_CNK_OBJECT draw_316;
static NJS_CNK_OBJECT draw_315;
static NJS_CNK_OBJECT draw_314;
static NJS_CNK_OBJECT draw_313;
static NJS_CNK_OBJECT draw_312;
static NJS_CNK_OBJECT draw_311;
static NJS_CNK_OBJECT draw_310;
static NJS_CNK_OBJECT draw_309;
static NJS_CNK_OBJECT draw_308;
static NJS_CNK_OBJECT draw_307;
static NJS_CNK_OBJECT draw_306;
static NJS_CNK_OBJECT draw_305;
static NJS_CNK_OBJECT draw_304;
static NJS_CNK_OBJECT draw_303;
static NJS_CNK_OBJECT draw_302;
static NJS_CNK_OBJECT draw_300;
static NJS_CNK_OBJECT draw_296;
static NJS_CNK_OBJECT draw_295;
static NJS_CNK_OBJECT draw_293;
static NJS_CNK_OBJECT draw_291;
static NJS_CNK_OBJECT draw_290;
static NJS_CNK_OBJECT draw_289;
static NJS_CNK_OBJECT draw_288;
static NJS_CNK_OBJECT draw_287;
static NJS_CNK_OBJECT draw_281;
static NJS_CNK_OBJECT draw_279;
static NJS_CNK_OBJECT draw_277;
static NJS_CNK_OBJECT draw_276;
static NJS_CNK_OBJECT draw_275;
static NJS_CNK_OBJECT draw_274;
static NJS_CNK_OBJECT draw_273;
static NJS_CNK_OBJECT draw_271;
static NJS_CNK_OBJECT draw_270;
static NJS_CNK_OBJECT draw_268;
static NJS_CNK_OBJECT draw_265;
static NJS_CNK_OBJECT draw_264;
static NJS_CNK_OBJECT draw_263;
static NJS_CNK_OBJECT draw_262;
static NJS_CNK_OBJECT draw_261;
static NJS_CNK_OBJECT draw_260;
static NJS_CNK_OBJECT draw_257;
static NJS_CNK_OBJECT draw_255;
static NJS_CNK_OBJECT draw_254;
static NJS_CNK_OBJECT draw_253;
static NJS_CNK_OBJECT draw_252;
static NJS_CNK_OBJECT draw_251;
static NJS_CNK_OBJECT draw_249;
static NJS_CNK_OBJECT draw_248;
static NJS_CNK_OBJECT draw_243;
static NJS_CNK_OBJECT draw_242;
static NJS_CNK_OBJECT draw_241;
static NJS_CNK_OBJECT draw_237;
static NJS_CNK_OBJECT draw_236;
static NJS_CNK_OBJECT draw_232;
static NJS_CNK_OBJECT draw_227;
static NJS_CNK_OBJECT draw_226;
static NJS_CNK_OBJECT draw_223;
static NJS_CNK_OBJECT draw_221;
static NJS_CNK_OBJECT draw_220;
static NJS_CNK_OBJECT draw_213;
static NJS_CNK_OBJECT draw_212;
static NJS_CNK_OBJECT draw_211;
static NJS_CNK_OBJECT draw_210;
static NJS_CNK_OBJECT draw_209;
static NJS_CNK_OBJECT draw_208;
static NJS_CNK_OBJECT draw_207;
static NJS_CNK_OBJECT draw_206;
static NJS_CNK_OBJECT draw_205;
static NJS_CNK_OBJECT draw_199;
static NJS_CNK_OBJECT draw_196;
static NJS_CNK_OBJECT draw_195;
static NJS_CNK_OBJECT draw_193;
static NJS_CNK_OBJECT draw_191;
static NJS_CNK_OBJECT draw_190;
static NJS_CNK_OBJECT draw_189;
static NJS_CNK_OBJECT draw_188;
static NJS_CNK_OBJECT draw_187;
static NJS_CNK_OBJECT draw_186;
static NJS_CNK_OBJECT draw_185;
static NJS_CNK_OBJECT draw_184;
static NJS_CNK_OBJECT draw_183;
static NJS_CNK_OBJECT draw_182;
static NJS_CNK_OBJECT draw_181;
static NJS_CNK_OBJECT draw_180;
static NJS_CNK_OBJECT draw_179;
static NJS_CNK_OBJECT draw_175;
static NJS_CNK_OBJECT draw_174;
static NJS_CNK_OBJECT draw_170;
static NJS_CNK_OBJECT draw_169;
static NJS_CNK_OBJECT draw_168;
static NJS_CNK_OBJECT draw_167;
static NJS_CNK_OBJECT draw_166;
static NJS_CNK_OBJECT draw_164;
static NJS_CNK_OBJECT draw_163;
static NJS_CNK_OBJECT draw_161;
static NJS_CNK_OBJECT draw_158;
static NJS_CNK_OBJECT draw_156;
static NJS_CNK_OBJECT draw_154;
static NJS_CNK_OBJECT draw_150;
static NJS_CNK_OBJECT draw_149;
static NJS_CNK_OBJECT draw_148;
static NJS_CNK_OBJECT draw_141;
static NJS_CNK_OBJECT draw_140;
static NJS_CNK_OBJECT draw_137;
static NJS_CNK_OBJECT draw_133;
static NJS_CNK_OBJECT draw_131;
static NJS_CNK_OBJECT draw_130;
static NJS_CNK_OBJECT draw_128;
static NJS_CNK_OBJECT draw_127;
static NJS_CNK_OBJECT draw_126;
static NJS_CNK_OBJECT draw_125;
static NJS_CNK_OBJECT draw_124;
static NJS_CNK_OBJECT draw_121;
static NJS_CNK_OBJECT draw_115;
static NJS_CNK_OBJECT draw_113;
static NJS_CNK_OBJECT draw_112;
static NJS_CNK_OBJECT draw_111;
static NJS_CNK_OBJECT draw_107;
static NJS_CNK_OBJECT draw_105;
static NJS_CNK_OBJECT draw_102;
static NJS_CNK_OBJECT draw_101;
static NJS_CNK_OBJECT draw_100;
static NJS_CNK_OBJECT draw_96;
static NJS_CNK_OBJECT draw_93;
static NJS_CNK_OBJECT draw_92;
static NJS_CNK_OBJECT draw_91;
static NJS_CNK_OBJECT draw_87;
static NJS_CNK_OBJECT draw_86;
static NJS_CNK_OBJECT draw_84;
static NJS_CNK_OBJECT draw_83;
static NJS_CNK_OBJECT draw_82;
static NJS_CNK_OBJECT draw_77;
static NJS_CNK_OBJECT draw_76;
static NJS_CNK_OBJECT draw_75;
static NJS_CNK_OBJECT draw_74;
static NJS_CNK_OBJECT draw_72;
static NJS_CNK_OBJECT draw_71;
static NJS_CNK_OBJECT draw_70;
static NJS_CNK_OBJECT draw_69;
static NJS_CNK_OBJECT draw_68;
static NJS_CNK_OBJECT draw_67;
static NJS_CNK_OBJECT draw_66;
static NJS_CNK_OBJECT draw_65;
static NJS_CNK_OBJECT draw_64;
static NJS_CNK_OBJECT draw_63;
static NJS_CNK_OBJECT draw_62;
static NJS_CNK_OBJECT draw_60;
static NJS_CNK_OBJECT draw_59;
static NJS_CNK_OBJECT draw_58;
static NJS_CNK_OBJECT draw_57;
static NJS_CNK_OBJECT draw_56;
static NJS_CNK_OBJECT draw_54;
static NJS_CNK_OBJECT draw_53;
static NJS_CNK_OBJECT draw_52;
static NJS_CNK_OBJECT draw_50;
static NJS_CNK_OBJECT draw_48;
static NJS_CNK_OBJECT draw_46;
static NJS_CNK_OBJECT draw_45;
static NJS_CNK_OBJECT draw_43;
static NJS_CNK_OBJECT draw_42;
static NJS_CNK_OBJECT draw_41;
static NJS_CNK_OBJECT draw_40;
static NJS_CNK_OBJECT draw_39;
static NJS_CNK_OBJECT draw_38;
static NJS_CNK_OBJECT draw_37;
static NJS_CNK_OBJECT draw_36;
static NJS_CNK_OBJECT draw_35;
static NJS_CNK_OBJECT draw_34;
static NJS_CNK_OBJECT draw_33;
static NJS_CNK_OBJECT draw_27;
static NJS_CNK_OBJECT draw_25;
static NJS_CNK_OBJECT draw_24;
static NJS_CNK_OBJECT draw_23;
static NJS_CNK_OBJECT draw_22;
static NJS_CNK_OBJECT draw_21;
static NJS_CNK_OBJECT draw_20;
static NJS_CNK_OBJECT draw_19;
static NJS_CNK_OBJECT draw_18;
static NJS_CNK_OBJECT draw_17;
static NJS_CNK_OBJECT draw_16;
static NJS_CNK_OBJECT draw_15;
static NJS_CNK_OBJECT draw_14;
static NJS_CNK_OBJECT draw_13;
static NJS_CNK_OBJECT draw_12;
static NJS_CNK_OBJECT draw_11;
static NJS_CNK_OBJECT draw_10;
static NJS_CNK_OBJECT draw_9;
static NJS_CNK_OBJECT draw_8;
static NJS_CNK_OBJECT draw_7;
static NJS_CNK_OBJECT draw_6;
static NJS_CNK_OBJECT draw_5;
static NJS_CNK_OBJECT draw_4;
static NJS_CNK_OBJECT draw_3;
static NJS_CNK_OBJECT draw_2;
static NJS_CNK_OBJECT draw_0;
static NJS_OBJECT collision_4;
static NJS_OBJECT collision_5;
static NJS_OBJECT collision_6;
static NJS_OBJECT collision_7;
static NJS_OBJECT collision_8;
static NJS_OBJECT collision_9;
static NJS_OBJECT collision_10;
static NJS_OBJECT collision_11;
static NJS_OBJECT collision_12;
static NJS_OBJECT collision_13;
static NJS_OBJECT collision_14;
static NJS_OBJECT collision_15;
static NJS_OBJECT collision_16;
static NJS_OBJECT collision_17;
static NJS_OBJECT collision_18;
static NJS_OBJECT collision_19;
static NJS_OBJECT collision_20;
static NJS_OBJECT collision_21;
static NJS_OBJECT collision_22;
static NJS_OBJECT collision_23;
static NJS_OBJECT collision_24;
static NJS_OBJECT collision_25;
static NJS_OBJECT collision_26;
static NJS_OBJECT collision_27;
static NJS_OBJECT collision_28;
static NJS_OBJECT collision_29;
static NJS_OBJECT collision_30;
static NJS_OBJECT collision_31;
static NJS_OBJECT collision_32;
static NJS_OBJECT collision_33;
static NJS_OBJECT collision_34;
static NJS_OBJECT collision_35;
static NJS_OBJECT collision_37;
static NJS_OBJECT collision_38;
static NJS_OBJECT collision_39;
static NJS_OBJECT collision_40;
static NJS_OBJECT collision_41;
static NJS_OBJECT collision_44;
static NJS_OBJECT collision_45;
static NJS_OBJECT collision_46;
static NJS_OBJECT collision_47;
static NJS_OBJECT collision_48;
static NJS_OBJECT collision_50;
static NJS_OBJECT collision_53;
static NJS_OBJECT collision_54;
static NJS_OBJECT collision_56;
static NJS_OBJECT collision_57;
static NJS_OBJECT collision_58;
static NJS_OBJECT collision_59;
static NJS_OBJECT collision_60;
static NJS_OBJECT collision_61;
static NJS_OBJECT collision_62;
static NJS_OBJECT collision_65;
static NJS_OBJECT collision_66;
static NJS_OBJECT collision_67;
static NJS_OBJECT collision_68;
static NJS_OBJECT collision_69;
static NJS_OBJECT collision_70;
static NJS_OBJECT collision_72;
static NJS_OBJECT collision_74;
static NJS_OBJECT collision_75;
static NJS_OBJECT collision_76;
static NJS_OBJECT collision_77;
static NJS_OBJECT collision_78;
static NJS_OBJECT collision_80;
static NJS_OBJECT collision_82;
static NJS_OBJECT collision_83;
static NJS_OBJECT collision_84;
static NJS_OBJECT collision_86;
static NJS_OBJECT collision_87;
static NJS_OBJECT collision_90;
static NJS_OBJECT collision_91;
static NJS_OBJECT collision_92;
static NJS_OBJECT collision_93;
static NJS_OBJECT collision_94;
static NJS_OBJECT collision_95;
static NJS_OBJECT collision_96;
static NJS_OBJECT collision_97;
static NJS_OBJECT collision_98;
static NJS_OBJECT collision_99;
static NJS_OBJECT collision_101;
static NJS_OBJECT collision_111;
static NJS_OBJECT collision_112;
static NJS_OBJECT collision_114;
static NJS_OBJECT collision_115;
static NJS_OBJECT collision_120;
static NJS_OBJECT collision_121;
static NJS_OBJECT collision_123;
static NJS_OBJECT collision_125;
static NJS_OBJECT collision_126;
static NJS_OBJECT collision_128;
static NJS_OBJECT collision_130;
static NJS_OBJECT collision_132;
static NJS_OBJECT collision_135;
static NJS_OBJECT collision_136;
static NJS_OBJECT collision_137;
static NJS_OBJECT collision_138;
static NJS_OBJECT collision_139;
static NJS_OBJECT collision_140;
static NJS_OBJECT collision_141;
static NJS_OBJECT collision_142;
static NJS_OBJECT collision_143;
static NJS_OBJECT collision_144;
static NJS_OBJECT collision_145;
static NJS_OBJECT collision_146;
static NJS_OBJECT collision_147;
static NJS_OBJECT collision_151;
static NJS_OBJECT collision_152;
static NJS_OBJECT collision_153;
static NJS_OBJECT collision_158;
static NJS_OBJECT collision_185;
static NJS_OBJECT collision_189;
static NJS_OBJECT collision_191;
static NJS_OBJECT collision_192;
static NJS_OBJECT collision_193;
static NJS_OBJECT collision_194;
static NJS_OBJECT collision_195;
static NJS_OBJECT collision_197;
static NJS_OBJECT collision_198;
static NJS_OBJECT collision_207;
static NJS_OBJECT collision_209;
static NJS_OBJECT collision_212;
static NJS_OBJECT collision_213;
static NJS_OBJECT collision_214;
static NJS_OBJECT collision_221;
static NJS_OBJECT collision_222;
static NJS_OBJECT collision_223;
static NJS_OBJECT collision_232;
static NJS_OBJECT collision_246;
static NJS_OBJECT collision_251;
static NJS_OBJECT collision_252;
static NJS_OBJECT collision_256;
static NJS_OBJECT collision_257;
static NJS_OBJECT collision_258;
static NJS_OBJECT collision_259;
static NJS_OBJECT collision_260;
static NJS_OBJECT collision_265;
static NJS_OBJECT collision_267;
static NJS_OBJECT collision_270;
static NJS_OBJECT collision_274;
static NJS_OBJECT collision_275;
static NJS_OBJECT collision_277;
static NJS_OBJECT collision_278;
static NJS_OBJECT collision_279;
static NJS_OBJECT collision_280;
static NJS_OBJECT collision_281;
static NJS_OBJECT collision_282;
static NJS_OBJECT collision_283;
static NJS_OBJECT collision_284;
static NJS_OBJECT collision_285;
static NJS_OBJECT collision_286;
static NJS_OBJECT collision_287;
static NJS_OBJECT collision_290;
static NJS_OBJECT collision_291;
static NJS_OBJECT collision_292;
static NJS_OBJECT collision_293;
static NJS_OBJECT collision_294;
static NJS_OBJECT collision_296;
static NJS_OBJECT collision_298;
static NJS_OBJECT collision_310;
static NJS_OBJECT collision_312;
static NJS_OBJECT collision_313;
static NJS_OBJECT collision_314;
static NJS_OBJECT collision_315;
static NJS_OBJECT collision_316;
static NJS_OBJECT collision_317;
static NJS_OBJECT collision_318;
static NJS_OBJECT collision_319;
static NJS_OBJECT collision_320;
static NJS_OBJECT collision_321;

static NJS_MKEY_A upgrade_white_jungle_motion_joint_4_angles[16] = {
#include "stg10/terrain/upgrade_white_jungle_motion_joint_4_angles.inc"
};
static NJS_MKEY_A upgrade_white_jungle_motion_joint_7_angles[16] = {
#include "stg10/terrain/upgrade_white_jungle_motion_joint_7_angles.inc"
};
static NJS_MKEY_A upgrade_white_jungle_motion_joint_8_angles[10] = {
#include "stg10/terrain/upgrade_white_jungle_motion_joint_8_angles.inc"
};
static NJS_MKEY_A upgrade_white_jungle_motion_joint_9_angles[14] = {
#include "stg10/terrain/upgrade_white_jungle_motion_joint_9_angles.inc"
};
static NJS_MKEY_A upgrade_white_jungle_motion_joint_13_angles[16] = {
#include "stg10/terrain/upgrade_white_jungle_motion_joint_13_angles.inc"
};
static NJS_MKEY_A upgrade_white_jungle_motion_joint_14_angles[2] = {
#include "stg10/terrain/upgrade_white_jungle_motion_joint_14_angles.inc"
};
static NJS_MKEY_A upgrade_white_jungle_motion_joint_17_angles[16] = {
#include "stg10/terrain/upgrade_white_jungle_motion_joint_17_angles.inc"
};
static NJS_MKEY_A upgrade_white_jungle_motion_joint_18_angles[16] = {
#include "stg10/terrain/upgrade_white_jungle_motion_joint_18_angles.inc"
};
static NJS_MKEY_A upgrade_white_jungle_motion_joint_19_angles[16] = {
#include "stg10/terrain/upgrade_white_jungle_motion_joint_19_angles.inc"
};
static NJS_MKEY_A upgrade_white_jungle_motion_joint_23_angles[16] = {
#include "stg10/terrain/upgrade_white_jungle_motion_joint_23_angles.inc"
};
static NJS_MKEY_A upgrade_white_jungle_motion_joint_24_angles[16] = {
#include "stg10/terrain/upgrade_white_jungle_motion_joint_24_angles.inc"
};
static NJS_MKEY_A upgrade_white_jungle_motion_joint_27_angles[16] = {
#include "stg10/terrain/upgrade_white_jungle_motion_joint_27_angles.inc"
};
static NJS_MKEY_A upgrade_white_jungle_motion_joint_28_angles[16] = {
#include "stg10/terrain/upgrade_white_jungle_motion_joint_28_angles.inc"
};
static NJS_MKEY_A upgrade_white_jungle_motion_joint_29_angles[16] = {
#include "stg10/terrain/upgrade_white_jungle_motion_joint_29_angles.inc"
};
static NJS_MKEY_A upgrade_white_jungle_motion_joint_31_angles[2] = {
#include "stg10/terrain/upgrade_white_jungle_motion_joint_31_angles.inc"
};
static NJS_MKEY_A upgrade_white_jungle_motion_joint_32_angles[16] = {
#include "stg10/terrain/upgrade_white_jungle_motion_joint_32_angles.inc"
};
static NJS_MKEY_A upgrade_white_jungle_motion_joint_33_angles[10] = {
#include "stg10/terrain/upgrade_white_jungle_motion_joint_33_angles.inc"
};
static NJS_MKEY_A upgrade_white_jungle_motion_joint_37_angles[16] = {
#include "stg10/terrain/upgrade_white_jungle_motion_joint_37_angles.inc"
};
static NJS_MKEY_A upgrade_white_jungle_motion_joint_38_angles[16] = {
#include "stg10/terrain/upgrade_white_jungle_motion_joint_38_angles.inc"
};
static NJS_MKEY_A upgrade_white_jungle_motion_joint_39_angles[16] = {
#include "stg10/terrain/upgrade_white_jungle_motion_joint_39_angles.inc"
};
static NJS_MKEY_A upgrade_white_jungle_motion_joint_41_angles[16] = {
#include "stg10/terrain/upgrade_white_jungle_motion_joint_41_angles.inc"
};
static NJS_MKEY_A upgrade_white_jungle_motion_joint_44_angles[16] = {
#include "stg10/terrain/upgrade_white_jungle_motion_joint_44_angles.inc"
};
static NJS_MKEY_A upgrade_white_jungle_motion_joint_51_angles[16] = {
#include "stg10/terrain/upgrade_white_jungle_motion_joint_51_angles.inc"
};
static NJS_MKEY_A upgrade_white_jungle_motion_joint_53_angles[16] = {
#include "stg10/terrain/upgrade_white_jungle_motion_joint_53_angles.inc"
};
static NJS_MKEY_A upgrade_white_jungle_motion_joint_56_angles[16] = {
#include "stg10/terrain/upgrade_white_jungle_motion_joint_56_angles.inc"
};
static NJS_MKEY_A upgrade_white_jungle_motion_joint_57_angles[16] = {
#include "stg10/terrain/upgrade_white_jungle_motion_joint_57_angles.inc"
};
static NJS_MKEY_A upgrade_white_jungle_motion_joint_58_angles[16] = {
#include "stg10/terrain/upgrade_white_jungle_motion_joint_58_angles.inc"
};
static NJS_MKEY_A upgrade_white_jungle_motion_joint_60_angles[16] = {
#include "stg10/terrain/upgrade_white_jungle_motion_joint_60_angles.inc"
};
static NJS_MDATA2 upgrade_white_jungle_motion_tracks[62] = {
#include "stg10/terrain/upgrade_white_jungle_motion_tracks.inc"
};
static NJS_MOTION upgrade_white_jungle_motion = {
#include "stg10/terrain/upgrade_white_jungle_motion.inc"
};
static NJS_MKEY_F missile_hold_motion_joint_2_position[8] = {
#include "stg10/terrain/missile_hold_motion_joint_2_position.inc"
};
static NJS_MKEY_A missile_hold_motion_joint_4_angles[2] = {
#include "stg10/terrain/missile_hold_motion_joint_4_angles.inc"
};
static NJS_MKEY_A missile_hold_motion_joint_7_angles[8] = {
#include "stg10/terrain/missile_hold_motion_joint_7_angles.inc"
};
static NJS_MKEY_A missile_hold_motion_joint_8_angles[6] = {
#include "stg10/terrain/missile_hold_motion_joint_8_angles.inc"
};
static NJS_MKEY_A missile_hold_motion_joint_9_angles[6] = {
#include "stg10/terrain/missile_hold_motion_joint_9_angles.inc"
};
static NJS_MKEY_A missile_hold_motion_joint_13_angles[8] = {
#include "stg10/terrain/missile_hold_motion_joint_13_angles.inc"
};
static NJS_MKEY_A missile_hold_motion_joint_14_angles[8] = {
#include "stg10/terrain/missile_hold_motion_joint_14_angles.inc"
};
static NJS_MKEY_A missile_hold_motion_joint_17_angles[8] = {
#include "stg10/terrain/missile_hold_motion_joint_17_angles.inc"
};
static NJS_MKEY_A missile_hold_motion_joint_18_angles[8] = {
#include "stg10/terrain/missile_hold_motion_joint_18_angles.inc"
};
static NJS_MKEY_A missile_hold_motion_joint_19_angles[8] = {
#include "stg10/terrain/missile_hold_motion_joint_19_angles.inc"
};
static NJS_MKEY_A missile_hold_motion_joint_23_angles[8] = {
#include "stg10/terrain/missile_hold_motion_joint_23_angles.inc"
};
static NJS_MKEY_A missile_hold_motion_joint_24_angles[8] = {
#include "stg10/terrain/missile_hold_motion_joint_24_angles.inc"
};
static NJS_MKEY_A missile_hold_motion_joint_27_angles[2] = {
#include "stg10/terrain/missile_hold_motion_joint_27_angles.inc"
};
static NJS_MKEY_A missile_hold_motion_joint_31_angles[2] = {
#include "stg10/terrain/missile_hold_motion_joint_31_angles.inc"
};
static NJS_MKEY_A missile_hold_motion_joint_32_angles[2] = {
#include "stg10/terrain/missile_hold_motion_joint_32_angles.inc"
};
static NJS_MKEY_A missile_hold_motion_joint_37_angles[8] = {
#include "stg10/terrain/missile_hold_motion_joint_37_angles.inc"
};
static NJS_MKEY_A missile_hold_motion_joint_38_angles[8] = {
#include "stg10/terrain/missile_hold_motion_joint_38_angles.inc"
};
static NJS_MKEY_A missile_hold_motion_joint_39_angles[8] = {
#include "stg10/terrain/missile_hold_motion_joint_39_angles.inc"
};
static NJS_MKEY_A missile_hold_motion_joint_41_angles[8] = {
#include "stg10/terrain/missile_hold_motion_joint_41_angles.inc"
};
static NJS_MKEY_A missile_hold_motion_joint_44_angles[8] = {
#include "stg10/terrain/missile_hold_motion_joint_44_angles.inc"
};
static NJS_MKEY_A missile_hold_motion_joint_51_angles[8] = {
#include "stg10/terrain/missile_hold_motion_joint_51_angles.inc"
};
static NJS_MKEY_A missile_hold_motion_joint_53_angles[8] = {
#include "stg10/terrain/missile_hold_motion_joint_53_angles.inc"
};
static NJS_MKEY_A missile_hold_motion_joint_56_angles[8] = {
#include "stg10/terrain/missile_hold_motion_joint_56_angles.inc"
};
static NJS_MKEY_A missile_hold_motion_joint_57_angles[4] = {
#include "stg10/terrain/missile_hold_motion_joint_57_angles.inc"
};
static NJS_MKEY_A missile_hold_motion_joint_58_angles[4] = {
#include "stg10/terrain/missile_hold_motion_joint_58_angles.inc"
};
static NJS_MKEY_A missile_hold_motion_joint_60_angles[8] = {
#include "stg10/terrain/missile_hold_motion_joint_60_angles.inc"
};
static NJS_MDATA2 missile_hold_motion_tracks[62] = {
#include "stg10/terrain/missile_hold_motion_tracks.inc"
};
static NJS_MOTION missile_hold_motion = {
#include "stg10/terrain/missile_hold_motion.inc"
};
static NJS_MKEY_F harbor_pose_173_motion_joint_2_position[32] = {
#include "stg10/terrain/harbor_pose_173_motion_joint_2_position.inc"
};
static NJS_MKEY_A harbor_pose_173_motion_joint_4_angles[32] = {
#include "stg10/terrain/harbor_pose_173_motion_joint_4_angles.inc"
};
static NJS_MKEY_A harbor_pose_173_motion_joint_7_angles[32] = {
#include "stg10/terrain/harbor_pose_173_motion_joint_7_angles.inc"
};
static NJS_MKEY_A harbor_pose_173_motion_joint_8_angles[30] = {
#include "stg10/terrain/harbor_pose_173_motion_joint_8_angles.inc"
};
static NJS_MKEY_A harbor_pose_173_motion_joint_9_angles[30] = {
#include "stg10/terrain/harbor_pose_173_motion_joint_9_angles.inc"
};
static NJS_MKEY_A harbor_pose_173_motion_joint_13_angles[32] = {
#include "stg10/terrain/harbor_pose_173_motion_joint_13_angles.inc"
};
static NJS_MKEY_A harbor_pose_173_motion_joint_14_angles[20] = {
#include "stg10/terrain/harbor_pose_173_motion_joint_14_angles.inc"
};
static NJS_MKEY_A harbor_pose_173_motion_joint_17_angles[32] = {
#include "stg10/terrain/harbor_pose_173_motion_joint_17_angles.inc"
};
static NJS_MKEY_A harbor_pose_173_motion_joint_18_angles[25] = {
#include "stg10/terrain/harbor_pose_173_motion_joint_18_angles.inc"
};
static NJS_MKEY_A harbor_pose_173_motion_joint_19_angles[25] = {
#include "stg10/terrain/harbor_pose_173_motion_joint_19_angles.inc"
};
static NJS_MKEY_A harbor_pose_173_motion_joint_23_angles[32] = {
#include "stg10/terrain/harbor_pose_173_motion_joint_23_angles.inc"
};
static NJS_MKEY_A harbor_pose_173_motion_joint_27_angles[5] = {
#include "stg10/terrain/harbor_pose_173_motion_joint_27_angles.inc"
};
static NJS_MKEY_A harbor_pose_173_motion_joint_31_angles[32] = {
#include "stg10/terrain/harbor_pose_173_motion_joint_31_angles.inc"
};
static NJS_MKEY_A harbor_pose_173_motion_joint_32_angles[32] = {
#include "stg10/terrain/harbor_pose_173_motion_joint_32_angles.inc"
};
static NJS_MKEY_A harbor_pose_173_motion_joint_33_angles[25] = {
#include "stg10/terrain/harbor_pose_173_motion_joint_33_angles.inc"
};
static NJS_MKEY_A harbor_pose_173_motion_joint_37_angles[32] = {
#include "stg10/terrain/harbor_pose_173_motion_joint_37_angles.inc"
};
static NJS_MKEY_A harbor_pose_173_motion_joint_38_angles[32] = {
#include "stg10/terrain/harbor_pose_173_motion_joint_38_angles.inc"
};
static NJS_MKEY_A harbor_pose_173_motion_joint_39_angles[32] = {
#include "stg10/terrain/harbor_pose_173_motion_joint_39_angles.inc"
};
static NJS_MKEY_A harbor_pose_173_motion_joint_41_angles[32] = {
#include "stg10/terrain/harbor_pose_173_motion_joint_41_angles.inc"
};
static NJS_MKEY_A harbor_pose_173_motion_joint_44_angles[32] = {
#include "stg10/terrain/harbor_pose_173_motion_joint_44_angles.inc"
};
static NJS_MKEY_A harbor_pose_173_motion_joint_51_angles[2] = {
#include "stg10/terrain/harbor_pose_173_motion_joint_51_angles.inc"
};
static NJS_MKEY_A harbor_pose_173_motion_joint_53_angles[2] = {
#include "stg10/terrain/harbor_pose_173_motion_joint_53_angles.inc"
};
static NJS_MKEY_A harbor_pose_173_motion_joint_56_angles[32] = {
#include "stg10/terrain/harbor_pose_173_motion_joint_56_angles.inc"
};
static NJS_MKEY_A harbor_pose_173_motion_joint_57_angles[32] = {
#include "stg10/terrain/harbor_pose_173_motion_joint_57_angles.inc"
};
static NJS_MKEY_A harbor_pose_173_motion_joint_58_angles[32] = {
#include "stg10/terrain/harbor_pose_173_motion_joint_58_angles.inc"
};
static NJS_MKEY_A harbor_pose_173_motion_joint_60_angles[32] = {
#include "stg10/terrain/harbor_pose_173_motion_joint_60_angles.inc"
};
static NJS_MDATA2 harbor_pose_173_motion_tracks[62] = {
#include "stg10/terrain/harbor_pose_173_motion_tracks.inc"
};
static NJS_MOTION harbor_pose_173_motion = {
#include "stg10/terrain/harbor_pose_173_motion.inc"
};
static NJS_MKEY_A unused_harbor_pose_motion_joint_4_angles[4] = {
#include "stg10/terrain/unused_harbor_pose_motion_joint_4_angles.inc"
};
static NJS_MKEY_A unused_harbor_pose_motion_joint_7_angles[4] = {
#include "stg10/terrain/unused_harbor_pose_motion_joint_7_angles.inc"
};
static NJS_MKEY_A unused_harbor_pose_motion_joint_8_angles[4] = {
#include "stg10/terrain/unused_harbor_pose_motion_joint_8_angles.inc"
};
static NJS_MKEY_A unused_harbor_pose_motion_joint_9_angles[4] = {
#include "stg10/terrain/unused_harbor_pose_motion_joint_9_angles.inc"
};
static NJS_MKEY_A unused_harbor_pose_motion_joint_13_angles[4] = {
#include "stg10/terrain/unused_harbor_pose_motion_joint_13_angles.inc"
};
static NJS_MKEY_A unused_harbor_pose_motion_joint_14_angles[4] = {
#include "stg10/terrain/unused_harbor_pose_motion_joint_14_angles.inc"
};
static NJS_MKEY_A unused_harbor_pose_motion_joint_17_angles[4] = {
#include "stg10/terrain/unused_harbor_pose_motion_joint_17_angles.inc"
};
static NJS_MKEY_A unused_harbor_pose_motion_joint_18_angles[4] = {
#include "stg10/terrain/unused_harbor_pose_motion_joint_18_angles.inc"
};
static NJS_MKEY_A unused_harbor_pose_motion_joint_19_angles[4] = {
#include "stg10/terrain/unused_harbor_pose_motion_joint_19_angles.inc"
};
static NJS_MKEY_A unused_harbor_pose_motion_joint_23_angles[4] = {
#include "stg10/terrain/unused_harbor_pose_motion_joint_23_angles.inc"
};
static NJS_MKEY_A unused_harbor_pose_motion_joint_24_angles[4] = {
#include "stg10/terrain/unused_harbor_pose_motion_joint_24_angles.inc"
};
static NJS_MKEY_A unused_harbor_pose_motion_joint_27_angles[4] = {
#include "stg10/terrain/unused_harbor_pose_motion_joint_27_angles.inc"
};
static NJS_MKEY_A unused_harbor_pose_motion_joint_31_angles[4] = {
#include "stg10/terrain/unused_harbor_pose_motion_joint_31_angles.inc"
};
static NJS_MKEY_A unused_harbor_pose_motion_joint_32_angles[4] = {
#include "stg10/terrain/unused_harbor_pose_motion_joint_32_angles.inc"
};
static NJS_MKEY_A unused_harbor_pose_motion_joint_33_angles[4] = {
#include "stg10/terrain/unused_harbor_pose_motion_joint_33_angles.inc"
};
static NJS_MKEY_A unused_harbor_pose_motion_joint_37_angles[4] = {
#include "stg10/terrain/unused_harbor_pose_motion_joint_37_angles.inc"
};
static NJS_MKEY_A unused_harbor_pose_motion_joint_38_angles[2] = {
#include "stg10/terrain/unused_harbor_pose_motion_joint_38_angles.inc"
};
static NJS_MKEY_A unused_harbor_pose_motion_joint_39_angles[2] = {
#include "stg10/terrain/unused_harbor_pose_motion_joint_39_angles.inc"
};
static NJS_MKEY_A unused_harbor_pose_motion_joint_41_angles[4] = {
#include "stg10/terrain/unused_harbor_pose_motion_joint_41_angles.inc"
};
static NJS_MKEY_A unused_harbor_pose_motion_joint_44_angles[4] = {
#include "stg10/terrain/unused_harbor_pose_motion_joint_44_angles.inc"
};
static NJS_MKEY_A unused_harbor_pose_motion_joint_51_angles[2] = {
#include "stg10/terrain/unused_harbor_pose_motion_joint_51_angles.inc"
};
static NJS_MKEY_A unused_harbor_pose_motion_joint_53_angles[2] = {
#include "stg10/terrain/unused_harbor_pose_motion_joint_53_angles.inc"
};
static NJS_MKEY_A unused_harbor_pose_motion_joint_56_angles[4] = {
#include "stg10/terrain/unused_harbor_pose_motion_joint_56_angles.inc"
};
static NJS_MKEY_A unused_harbor_pose_motion_joint_57_angles[2] = {
#include "stg10/terrain/unused_harbor_pose_motion_joint_57_angles.inc"
};
static NJS_MKEY_A unused_harbor_pose_motion_joint_58_angles[2] = {
#include "stg10/terrain/unused_harbor_pose_motion_joint_58_angles.inc"
};
static NJS_MKEY_A unused_harbor_pose_motion_joint_60_angles[4] = {
#include "stg10/terrain/unused_harbor_pose_motion_joint_60_angles.inc"
};
static NJS_MDATA2 unused_harbor_pose_motion_tracks[62] = {
#include "stg10/terrain/unused_harbor_pose_motion_tracks.inc"
};
static NJS_MOTION unused_harbor_pose_motion = {
#include "stg10/terrain/unused_harbor_pose_motion.inc"
};
static NJS_MKEY_F rider_trick_harbor_a_motion_joint_2_position[140] = {
#include "stg10/terrain/rider_trick_harbor_a_motion_joint_2_position.inc"
};
static NJS_MKEY_A rider_trick_harbor_a_motion_joint_4_angles[140] = {
#include "stg10/terrain/rider_trick_harbor_a_motion_joint_4_angles.inc"
};
static NJS_MKEY_A rider_trick_harbor_a_motion_joint_7_angles[140] = {
#include "stg10/terrain/rider_trick_harbor_a_motion_joint_7_angles.inc"
};
static NJS_MKEY_A rider_trick_harbor_a_motion_joint_8_angles[130] = {
#include "stg10/terrain/rider_trick_harbor_a_motion_joint_8_angles.inc"
};
static NJS_MKEY_A rider_trick_harbor_a_motion_joint_9_angles[131] = {
#include "stg10/terrain/rider_trick_harbor_a_motion_joint_9_angles.inc"
};
static NJS_MKEY_A rider_trick_harbor_a_motion_joint_13_angles[140] = {
#include "stg10/terrain/rider_trick_harbor_a_motion_joint_13_angles.inc"
};
static NJS_MKEY_A rider_trick_harbor_a_motion_joint_14_angles[113] = {
#include "stg10/terrain/rider_trick_harbor_a_motion_joint_14_angles.inc"
};
static NJS_MKEY_A rider_trick_harbor_a_motion_joint_17_angles[140] = {
#include "stg10/terrain/rider_trick_harbor_a_motion_joint_17_angles.inc"
};
static NJS_MKEY_A rider_trick_harbor_a_motion_joint_18_angles[132] = {
#include "stg10/terrain/rider_trick_harbor_a_motion_joint_18_angles.inc"
};
static NJS_MKEY_A rider_trick_harbor_a_motion_joint_19_angles[140] = {
#include "stg10/terrain/rider_trick_harbor_a_motion_joint_19_angles.inc"
};
static NJS_MKEY_A rider_trick_harbor_a_motion_joint_23_angles[140] = {
#include "stg10/terrain/rider_trick_harbor_a_motion_joint_23_angles.inc"
};
static NJS_MKEY_A rider_trick_harbor_a_motion_joint_24_angles[107] = {
#include "stg10/terrain/rider_trick_harbor_a_motion_joint_24_angles.inc"
};
static NJS_MKEY_A rider_trick_harbor_a_motion_joint_27_angles[140] = {
#include "stg10/terrain/rider_trick_harbor_a_motion_joint_27_angles.inc"
};
static NJS_MKEY_A rider_trick_harbor_a_motion_joint_28_angles[101] = {
#include "stg10/terrain/rider_trick_harbor_a_motion_joint_28_angles.inc"
};
static NJS_MKEY_A rider_trick_harbor_a_motion_joint_29_angles[105] = {
#include "stg10/terrain/rider_trick_harbor_a_motion_joint_29_angles.inc"
};
static NJS_MKEY_A rider_trick_harbor_a_motion_joint_31_angles[140] = {
#include "stg10/terrain/rider_trick_harbor_a_motion_joint_31_angles.inc"
};
static NJS_MKEY_A rider_trick_harbor_a_motion_joint_32_angles[140] = {
#include "stg10/terrain/rider_trick_harbor_a_motion_joint_32_angles.inc"
};
static NJS_MKEY_A rider_trick_harbor_a_motion_joint_33_angles[100] = {
#include "stg10/terrain/rider_trick_harbor_a_motion_joint_33_angles.inc"
};
static NJS_MKEY_A rider_trick_harbor_a_motion_joint_37_angles[140] = {
#include "stg10/terrain/rider_trick_harbor_a_motion_joint_37_angles.inc"
};
static NJS_MKEY_A rider_trick_harbor_a_motion_joint_38_angles[99] = {
#include "stg10/terrain/rider_trick_harbor_a_motion_joint_38_angles.inc"
};
static NJS_MKEY_A rider_trick_harbor_a_motion_joint_39_angles[99] = {
#include "stg10/terrain/rider_trick_harbor_a_motion_joint_39_angles.inc"
};
static NJS_MKEY_A rider_trick_harbor_a_motion_joint_41_angles[140] = {
#include "stg10/terrain/rider_trick_harbor_a_motion_joint_41_angles.inc"
};
static NJS_MKEY_A rider_trick_harbor_a_motion_joint_44_angles[140] = {
#include "stg10/terrain/rider_trick_harbor_a_motion_joint_44_angles.inc"
};
static NJS_MKEY_A rider_trick_harbor_a_motion_joint_51_angles[2] = {
#include "stg10/terrain/rider_trick_harbor_a_motion_joint_51_angles.inc"
};
static NJS_MKEY_A rider_trick_harbor_a_motion_joint_53_angles[2] = {
#include "stg10/terrain/rider_trick_harbor_a_motion_joint_53_angles.inc"
};
static NJS_MKEY_A rider_trick_harbor_a_motion_joint_56_angles[140] = {
#include "stg10/terrain/rider_trick_harbor_a_motion_joint_56_angles.inc"
};
static NJS_MKEY_A rider_trick_harbor_a_motion_joint_57_angles[86] = {
#include "stg10/terrain/rider_trick_harbor_a_motion_joint_57_angles.inc"
};
static NJS_MKEY_A rider_trick_harbor_a_motion_joint_58_angles[86] = {
#include "stg10/terrain/rider_trick_harbor_a_motion_joint_58_angles.inc"
};
static NJS_MKEY_A rider_trick_harbor_a_motion_joint_60_angles[140] = {
#include "stg10/terrain/rider_trick_harbor_a_motion_joint_60_angles.inc"
};
static NJS_MDATA2 rider_trick_harbor_a_motion_tracks[62] = {
#include "stg10/terrain/rider_trick_harbor_a_motion_tracks.inc"
};
static NJS_MOTION rider_trick_harbor_a_motion = {
#include "stg10/terrain/rider_trick_harbor_a_motion.inc"
};
static NJS_MKEY_F rider_trick_harbor_b_motion_joint_2_position[121] = {
#include "stg10/terrain/rider_trick_harbor_b_motion_joint_2_position.inc"
};
static NJS_MKEY_A rider_trick_harbor_b_motion_joint_4_angles[121] = {
#include "stg10/terrain/rider_trick_harbor_b_motion_joint_4_angles.inc"
};
static NJS_MKEY_A rider_trick_harbor_b_motion_joint_7_angles[121] = {
#include "stg10/terrain/rider_trick_harbor_b_motion_joint_7_angles.inc"
};
static NJS_MKEY_A rider_trick_harbor_b_motion_joint_8_angles[106] = {
#include "stg10/terrain/rider_trick_harbor_b_motion_joint_8_angles.inc"
};
static NJS_MKEY_A rider_trick_harbor_b_motion_joint_9_angles[121] = {
#include "stg10/terrain/rider_trick_harbor_b_motion_joint_9_angles.inc"
};
static NJS_MKEY_A rider_trick_harbor_b_motion_joint_13_angles[121] = {
#include "stg10/terrain/rider_trick_harbor_b_motion_joint_13_angles.inc"
};
static NJS_MKEY_A rider_trick_harbor_b_motion_joint_14_angles[105] = {
#include "stg10/terrain/rider_trick_harbor_b_motion_joint_14_angles.inc"
};
static NJS_MKEY_A rider_trick_harbor_b_motion_joint_17_angles[121] = {
#include "stg10/terrain/rider_trick_harbor_b_motion_joint_17_angles.inc"
};
static NJS_MKEY_A rider_trick_harbor_b_motion_joint_18_angles[94] = {
#include "stg10/terrain/rider_trick_harbor_b_motion_joint_18_angles.inc"
};
static NJS_MKEY_A rider_trick_harbor_b_motion_joint_19_angles[94] = {
#include "stg10/terrain/rider_trick_harbor_b_motion_joint_19_angles.inc"
};
static NJS_MKEY_A rider_trick_harbor_b_motion_joint_23_angles[121] = {
#include "stg10/terrain/rider_trick_harbor_b_motion_joint_23_angles.inc"
};
static NJS_MKEY_A rider_trick_harbor_b_motion_joint_24_angles[106] = {
#include "stg10/terrain/rider_trick_harbor_b_motion_joint_24_angles.inc"
};
static NJS_MKEY_A rider_trick_harbor_b_motion_joint_27_angles[121] = {
#include "stg10/terrain/rider_trick_harbor_b_motion_joint_27_angles.inc"
};
static NJS_MKEY_A rider_trick_harbor_b_motion_joint_28_angles[84] = {
#include "stg10/terrain/rider_trick_harbor_b_motion_joint_28_angles.inc"
};
static NJS_MKEY_A rider_trick_harbor_b_motion_joint_29_angles[91] = {
#include "stg10/terrain/rider_trick_harbor_b_motion_joint_29_angles.inc"
};
static NJS_MKEY_A rider_trick_harbor_b_motion_joint_31_angles[121] = {
#include "stg10/terrain/rider_trick_harbor_b_motion_joint_31_angles.inc"
};
static NJS_MKEY_A rider_trick_harbor_b_motion_joint_32_angles[121] = {
#include "stg10/terrain/rider_trick_harbor_b_motion_joint_32_angles.inc"
};
static NJS_MKEY_A rider_trick_harbor_b_motion_joint_33_angles[85] = {
#include "stg10/terrain/rider_trick_harbor_b_motion_joint_33_angles.inc"
};
static NJS_MKEY_A rider_trick_harbor_b_motion_joint_37_angles[121] = {
#include "stg10/terrain/rider_trick_harbor_b_motion_joint_37_angles.inc"
};
static NJS_MKEY_A rider_trick_harbor_b_motion_joint_38_angles[105] = {
#include "stg10/terrain/rider_trick_harbor_b_motion_joint_38_angles.inc"
};
static NJS_MKEY_A rider_trick_harbor_b_motion_joint_39_angles[105] = {
#include "stg10/terrain/rider_trick_harbor_b_motion_joint_39_angles.inc"
};
static NJS_MKEY_A rider_trick_harbor_b_motion_joint_41_angles[121] = {
#include "stg10/terrain/rider_trick_harbor_b_motion_joint_41_angles.inc"
};
static NJS_MKEY_A rider_trick_harbor_b_motion_joint_44_angles[121] = {
#include "stg10/terrain/rider_trick_harbor_b_motion_joint_44_angles.inc"
};
static NJS_MKEY_A rider_trick_harbor_b_motion_joint_51_angles[2] = {
#include "stg10/terrain/rider_trick_harbor_b_motion_joint_51_angles.inc"
};
static NJS_MKEY_A rider_trick_harbor_b_motion_joint_53_angles[2] = {
#include "stg10/terrain/rider_trick_harbor_b_motion_joint_53_angles.inc"
};
static NJS_MKEY_A rider_trick_harbor_b_motion_joint_56_angles[121] = {
#include "stg10/terrain/rider_trick_harbor_b_motion_joint_56_angles.inc"
};
static NJS_MKEY_A rider_trick_harbor_b_motion_joint_57_angles[118] = {
#include "stg10/terrain/rider_trick_harbor_b_motion_joint_57_angles.inc"
};
static NJS_MKEY_A rider_trick_harbor_b_motion_joint_58_angles[118] = {
#include "stg10/terrain/rider_trick_harbor_b_motion_joint_58_angles.inc"
};
static NJS_MKEY_A rider_trick_harbor_b_motion_joint_60_angles[121] = {
#include "stg10/terrain/rider_trick_harbor_b_motion_joint_60_angles.inc"
};
static NJS_MDATA2 rider_trick_harbor_b_motion_tracks[62] = {
#include "stg10/terrain/rider_trick_harbor_b_motion_tracks.inc"
};
static NJS_MOTION rider_trick_harbor_b_motion = {
#include "stg10/terrain/rider_trick_harbor_b_motion.inc"
};
static NJS_MKEY_F rider_trick_harbor_c_motion_joint_2_position[90] = {
#include "stg10/terrain/rider_trick_harbor_c_motion_joint_2_position.inc"
};
static NJS_MKEY_A rider_trick_harbor_c_motion_joint_4_angles[90] = {
#include "stg10/terrain/rider_trick_harbor_c_motion_joint_4_angles.inc"
};
static NJS_MKEY_A rider_trick_harbor_c_motion_joint_7_angles[90] = {
#include "stg10/terrain/rider_trick_harbor_c_motion_joint_7_angles.inc"
};
static NJS_MKEY_A rider_trick_harbor_c_motion_joint_8_angles[76] = {
#include "stg10/terrain/rider_trick_harbor_c_motion_joint_8_angles.inc"
};
static NJS_MKEY_A rider_trick_harbor_c_motion_joint_9_angles[87] = {
#include "stg10/terrain/rider_trick_harbor_c_motion_joint_9_angles.inc"
};
static NJS_MKEY_A rider_trick_harbor_c_motion_joint_13_angles[90] = {
#include "stg10/terrain/rider_trick_harbor_c_motion_joint_13_angles.inc"
};
static NJS_MKEY_A rider_trick_harbor_c_motion_joint_14_angles[81] = {
#include "stg10/terrain/rider_trick_harbor_c_motion_joint_14_angles.inc"
};
static NJS_MKEY_A rider_trick_harbor_c_motion_joint_17_angles[90] = {
#include "stg10/terrain/rider_trick_harbor_c_motion_joint_17_angles.inc"
};
static NJS_MKEY_A rider_trick_harbor_c_motion_joint_18_angles[90] = {
#include "stg10/terrain/rider_trick_harbor_c_motion_joint_18_angles.inc"
};
static NJS_MKEY_A rider_trick_harbor_c_motion_joint_19_angles[90] = {
#include "stg10/terrain/rider_trick_harbor_c_motion_joint_19_angles.inc"
};
static NJS_MKEY_A rider_trick_harbor_c_motion_joint_23_angles[90] = {
#include "stg10/terrain/rider_trick_harbor_c_motion_joint_23_angles.inc"
};
static NJS_MKEY_A rider_trick_harbor_c_motion_joint_24_angles[90] = {
#include "stg10/terrain/rider_trick_harbor_c_motion_joint_24_angles.inc"
};
static NJS_MKEY_A rider_trick_harbor_c_motion_joint_27_angles[90] = {
#include "stg10/terrain/rider_trick_harbor_c_motion_joint_27_angles.inc"
};
static NJS_MKEY_A rider_trick_harbor_c_motion_joint_28_angles[64] = {
#include "stg10/terrain/rider_trick_harbor_c_motion_joint_28_angles.inc"
};
static NJS_MKEY_A rider_trick_harbor_c_motion_joint_29_angles[64] = {
#include "stg10/terrain/rider_trick_harbor_c_motion_joint_29_angles.inc"
};
static NJS_MKEY_A rider_trick_harbor_c_motion_joint_31_angles[90] = {
#include "stg10/terrain/rider_trick_harbor_c_motion_joint_31_angles.inc"
};
static NJS_MKEY_A rider_trick_harbor_c_motion_joint_32_angles[90] = {
#include "stg10/terrain/rider_trick_harbor_c_motion_joint_32_angles.inc"
};
static NJS_MKEY_A rider_trick_harbor_c_motion_joint_33_angles[75] = {
#include "stg10/terrain/rider_trick_harbor_c_motion_joint_33_angles.inc"
};
static NJS_MKEY_A rider_trick_harbor_c_motion_joint_37_angles[90] = {
#include "stg10/terrain/rider_trick_harbor_c_motion_joint_37_angles.inc"
};
static NJS_MKEY_A rider_trick_harbor_c_motion_joint_38_angles[66] = {
#include "stg10/terrain/rider_trick_harbor_c_motion_joint_38_angles.inc"
};
static NJS_MKEY_A rider_trick_harbor_c_motion_joint_39_angles[66] = {
#include "stg10/terrain/rider_trick_harbor_c_motion_joint_39_angles.inc"
};
static NJS_MKEY_A rider_trick_harbor_c_motion_joint_41_angles[90] = {
#include "stg10/terrain/rider_trick_harbor_c_motion_joint_41_angles.inc"
};
static NJS_MKEY_A rider_trick_harbor_c_motion_joint_44_angles[90] = {
#include "stg10/terrain/rider_trick_harbor_c_motion_joint_44_angles.inc"
};
static NJS_MKEY_A rider_trick_harbor_c_motion_joint_51_angles[2] = {
#include "stg10/terrain/rider_trick_harbor_c_motion_joint_51_angles.inc"
};
static NJS_MKEY_A rider_trick_harbor_c_motion_joint_53_angles[2] = {
#include "stg10/terrain/rider_trick_harbor_c_motion_joint_53_angles.inc"
};
static NJS_MKEY_A rider_trick_harbor_c_motion_joint_56_angles[90] = {
#include "stg10/terrain/rider_trick_harbor_c_motion_joint_56_angles.inc"
};
static NJS_MKEY_A rider_trick_harbor_c_motion_joint_57_angles[64] = {
#include "stg10/terrain/rider_trick_harbor_c_motion_joint_57_angles.inc"
};
static NJS_MKEY_A rider_trick_harbor_c_motion_joint_58_angles[64] = {
#include "stg10/terrain/rider_trick_harbor_c_motion_joint_58_angles.inc"
};
static NJS_MKEY_A rider_trick_harbor_c_motion_joint_60_angles[90] = {
#include "stg10/terrain/rider_trick_harbor_c_motion_joint_60_angles.inc"
};
static NJS_MDATA2 rider_trick_harbor_c_motion_tracks[62] = {
#include "stg10/terrain/rider_trick_harbor_c_motion_tracks.inc"
};
static NJS_MOTION rider_trick_harbor_c_motion = {
#include "stg10/terrain/rider_trick_harbor_c_motion.inc"
};
static NJS_MKEY_F rider_lean_positive_motion_joint_2_position[17] = {
#include "stg10/terrain/rider_lean_positive_motion_joint_2_position.inc"
};
static NJS_MKEY_A rider_lean_positive_motion_joint_4_angles[17] = {
#include "stg10/terrain/rider_lean_positive_motion_joint_4_angles.inc"
};
static NJS_MKEY_A rider_lean_positive_motion_joint_7_angles[17] = {
#include "stg10/terrain/rider_lean_positive_motion_joint_7_angles.inc"
};
static NJS_MKEY_A rider_lean_positive_motion_joint_8_angles[17] = {
#include "stg10/terrain/rider_lean_positive_motion_joint_8_angles.inc"
};
static NJS_MKEY_A rider_lean_positive_motion_joint_9_angles[17] = {
#include "stg10/terrain/rider_lean_positive_motion_joint_9_angles.inc"
};
static NJS_MKEY_A rider_lean_positive_motion_joint_13_angles[17] = {
#include "stg10/terrain/rider_lean_positive_motion_joint_13_angles.inc"
};
static NJS_MKEY_A rider_lean_positive_motion_joint_14_angles[8] = {
#include "stg10/terrain/rider_lean_positive_motion_joint_14_angles.inc"
};
static NJS_MKEY_A rider_lean_positive_motion_joint_17_angles[17] = {
#include "stg10/terrain/rider_lean_positive_motion_joint_17_angles.inc"
};
static NJS_MKEY_A rider_lean_positive_motion_joint_18_angles[15] = {
#include "stg10/terrain/rider_lean_positive_motion_joint_18_angles.inc"
};
static NJS_MKEY_A rider_lean_positive_motion_joint_19_angles[17] = {
#include "stg10/terrain/rider_lean_positive_motion_joint_19_angles.inc"
};
static NJS_MKEY_A rider_lean_positive_motion_joint_23_angles[17] = {
#include "stg10/terrain/rider_lean_positive_motion_joint_23_angles.inc"
};
static NJS_MKEY_A rider_lean_positive_motion_joint_24_angles[10] = {
#include "stg10/terrain/rider_lean_positive_motion_joint_24_angles.inc"
};
static NJS_MKEY_A rider_lean_positive_motion_joint_27_angles[17] = {
#include "stg10/terrain/rider_lean_positive_motion_joint_27_angles.inc"
};
static NJS_MKEY_A rider_lean_positive_motion_joint_31_angles[17] = {
#include "stg10/terrain/rider_lean_positive_motion_joint_31_angles.inc"
};
static NJS_MKEY_A rider_lean_positive_motion_joint_32_angles[17] = {
#include "stg10/terrain/rider_lean_positive_motion_joint_32_angles.inc"
};
static NJS_MKEY_A rider_lean_positive_motion_joint_33_angles[12] = {
#include "stg10/terrain/rider_lean_positive_motion_joint_33_angles.inc"
};
static NJS_MKEY_A rider_lean_positive_motion_joint_37_angles[17] = {
#include "stg10/terrain/rider_lean_positive_motion_joint_37_angles.inc"
};
static NJS_MKEY_A rider_lean_positive_motion_joint_38_angles[17] = {
#include "stg10/terrain/rider_lean_positive_motion_joint_38_angles.inc"
};
static NJS_MKEY_A rider_lean_positive_motion_joint_39_angles[17] = {
#include "stg10/terrain/rider_lean_positive_motion_joint_39_angles.inc"
};
static NJS_MKEY_A rider_lean_positive_motion_joint_41_angles[17] = {
#include "stg10/terrain/rider_lean_positive_motion_joint_41_angles.inc"
};
static NJS_MKEY_A rider_lean_positive_motion_joint_44_angles[17] = {
#include "stg10/terrain/rider_lean_positive_motion_joint_44_angles.inc"
};
static NJS_MKEY_A rider_lean_positive_motion_joint_51_angles[2] = {
#include "stg10/terrain/rider_lean_positive_motion_joint_51_angles.inc"
};
static NJS_MKEY_A rider_lean_positive_motion_joint_53_angles[2] = {
#include "stg10/terrain/rider_lean_positive_motion_joint_53_angles.inc"
};
static NJS_MKEY_A rider_lean_positive_motion_joint_56_angles[17] = {
#include "stg10/terrain/rider_lean_positive_motion_joint_56_angles.inc"
};
static NJS_MKEY_A rider_lean_positive_motion_joint_57_angles[17] = {
#include "stg10/terrain/rider_lean_positive_motion_joint_57_angles.inc"
};
static NJS_MKEY_A rider_lean_positive_motion_joint_58_angles[17] = {
#include "stg10/terrain/rider_lean_positive_motion_joint_58_angles.inc"
};
static NJS_MKEY_A rider_lean_positive_motion_joint_60_angles[17] = {
#include "stg10/terrain/rider_lean_positive_motion_joint_60_angles.inc"
};
static NJS_MDATA2 rider_lean_positive_motion_tracks[62] = {
#include "stg10/terrain/rider_lean_positive_motion_tracks.inc"
};
static NJS_MOTION rider_lean_positive_motion = {
#include "stg10/terrain/rider_lean_positive_motion.inc"
};
static NJS_MKEY_F rider_lean_negative_motion_joint_2_position[2] = {
#include "stg10/terrain/rider_lean_negative_motion_joint_2_position.inc"
};
static NJS_MKEY_A rider_lean_negative_motion_joint_4_angles[17] = {
#include "stg10/terrain/rider_lean_negative_motion_joint_4_angles.inc"
};
static NJS_MKEY_A rider_lean_negative_motion_joint_7_angles[17] = {
#include "stg10/terrain/rider_lean_negative_motion_joint_7_angles.inc"
};
static NJS_MKEY_A rider_lean_negative_motion_joint_8_angles[17] = {
#include "stg10/terrain/rider_lean_negative_motion_joint_8_angles.inc"
};
static NJS_MKEY_A rider_lean_negative_motion_joint_9_angles[17] = {
#include "stg10/terrain/rider_lean_negative_motion_joint_9_angles.inc"
};
static NJS_MKEY_A rider_lean_negative_motion_joint_13_angles[17] = {
#include "stg10/terrain/rider_lean_negative_motion_joint_13_angles.inc"
};
static NJS_MKEY_A rider_lean_negative_motion_joint_14_angles[9] = {
#include "stg10/terrain/rider_lean_negative_motion_joint_14_angles.inc"
};
static NJS_MKEY_A rider_lean_negative_motion_joint_17_angles[17] = {
#include "stg10/terrain/rider_lean_negative_motion_joint_17_angles.inc"
};
static NJS_MKEY_A rider_lean_negative_motion_joint_18_angles[17] = {
#include "stg10/terrain/rider_lean_negative_motion_joint_18_angles.inc"
};
static NJS_MKEY_A rider_lean_negative_motion_joint_19_angles[17] = {
#include "stg10/terrain/rider_lean_negative_motion_joint_19_angles.inc"
};
static NJS_MKEY_A rider_lean_negative_motion_joint_23_angles[17] = {
#include "stg10/terrain/rider_lean_negative_motion_joint_23_angles.inc"
};
static NJS_MKEY_A rider_lean_negative_motion_joint_24_angles[12] = {
#include "stg10/terrain/rider_lean_negative_motion_joint_24_angles.inc"
};
static NJS_MKEY_A rider_lean_negative_motion_joint_27_angles[17] = {
#include "stg10/terrain/rider_lean_negative_motion_joint_27_angles.inc"
};
static NJS_MKEY_A rider_lean_negative_motion_joint_28_angles[5] = {
#include "stg10/terrain/rider_lean_negative_motion_joint_28_angles.inc"
};
static NJS_MKEY_A rider_lean_negative_motion_joint_29_angles[5] = {
#include "stg10/terrain/rider_lean_negative_motion_joint_29_angles.inc"
};
static NJS_MKEY_A rider_lean_negative_motion_joint_31_angles[17] = {
#include "stg10/terrain/rider_lean_negative_motion_joint_31_angles.inc"
};
static NJS_MKEY_A rider_lean_negative_motion_joint_32_angles[17] = {
#include "stg10/terrain/rider_lean_negative_motion_joint_32_angles.inc"
};
static NJS_MKEY_A rider_lean_negative_motion_joint_33_angles[13] = {
#include "stg10/terrain/rider_lean_negative_motion_joint_33_angles.inc"
};
static NJS_MKEY_A rider_lean_negative_motion_joint_37_angles[17] = {
#include "stg10/terrain/rider_lean_negative_motion_joint_37_angles.inc"
};
static NJS_MKEY_A rider_lean_negative_motion_joint_38_angles[17] = {
#include "stg10/terrain/rider_lean_negative_motion_joint_38_angles.inc"
};
static NJS_MKEY_A rider_lean_negative_motion_joint_39_angles[17] = {
#include "stg10/terrain/rider_lean_negative_motion_joint_39_angles.inc"
};
static NJS_MKEY_A rider_lean_negative_motion_joint_41_angles[17] = {
#include "stg10/terrain/rider_lean_negative_motion_joint_41_angles.inc"
};
static NJS_MKEY_A rider_lean_negative_motion_joint_44_angles[17] = {
#include "stg10/terrain/rider_lean_negative_motion_joint_44_angles.inc"
};
static NJS_MKEY_A rider_lean_negative_motion_joint_51_angles[2] = {
#include "stg10/terrain/rider_lean_negative_motion_joint_51_angles.inc"
};
static NJS_MKEY_A rider_lean_negative_motion_joint_53_angles[2] = {
#include "stg10/terrain/rider_lean_negative_motion_joint_53_angles.inc"
};
static NJS_MKEY_A rider_lean_negative_motion_joint_56_angles[17] = {
#include "stg10/terrain/rider_lean_negative_motion_joint_56_angles.inc"
};
static NJS_MKEY_A rider_lean_negative_motion_joint_57_angles[17] = {
#include "stg10/terrain/rider_lean_negative_motion_joint_57_angles.inc"
};
static NJS_MKEY_A rider_lean_negative_motion_joint_58_angles[17] = {
#include "stg10/terrain/rider_lean_negative_motion_joint_58_angles.inc"
};
static NJS_MKEY_A rider_lean_negative_motion_joint_60_angles[17] = {
#include "stg10/terrain/rider_lean_negative_motion_joint_60_angles.inc"
};
static NJS_MDATA2 rider_lean_negative_motion_tracks[62] = {
#include "stg10/terrain/rider_lean_negative_motion_tracks.inc"
};
static NJS_MOTION rider_lean_negative_motion = {
#include "stg10/terrain/rider_lean_negative_motion.inc"
};
static NJS_MKEY_F rider_turn_positive_motion_joint_2_position[9] = {
#include "stg10/terrain/rider_turn_positive_motion_joint_2_position.inc"
};
static NJS_MKEY_A rider_turn_positive_motion_joint_4_angles[9] = {
#include "stg10/terrain/rider_turn_positive_motion_joint_4_angles.inc"
};
static NJS_MKEY_A rider_turn_positive_motion_joint_7_angles[9] = {
#include "stg10/terrain/rider_turn_positive_motion_joint_7_angles.inc"
};
static NJS_MKEY_A rider_turn_positive_motion_joint_8_angles[2] = {
#include "stg10/terrain/rider_turn_positive_motion_joint_8_angles.inc"
};
static NJS_MKEY_A rider_turn_positive_motion_joint_9_angles[2] = {
#include "stg10/terrain/rider_turn_positive_motion_joint_9_angles.inc"
};
static NJS_MKEY_A rider_turn_positive_motion_joint_13_angles[5] = {
#include "stg10/terrain/rider_turn_positive_motion_joint_13_angles.inc"
};
static NJS_MKEY_A rider_turn_positive_motion_joint_17_angles[9] = {
#include "stg10/terrain/rider_turn_positive_motion_joint_17_angles.inc"
};
static NJS_MKEY_A rider_turn_positive_motion_joint_18_angles[2] = {
#include "stg10/terrain/rider_turn_positive_motion_joint_18_angles.inc"
};
static NJS_MKEY_A rider_turn_positive_motion_joint_19_angles[8] = {
#include "stg10/terrain/rider_turn_positive_motion_joint_19_angles.inc"
};
static NJS_MKEY_A rider_turn_positive_motion_joint_23_angles[9] = {
#include "stg10/terrain/rider_turn_positive_motion_joint_23_angles.inc"
};
static NJS_MKEY_A rider_turn_positive_motion_joint_27_angles[9] = {
#include "stg10/terrain/rider_turn_positive_motion_joint_27_angles.inc"
};
static NJS_MKEY_A rider_turn_positive_motion_joint_31_angles[9] = {
#include "stg10/terrain/rider_turn_positive_motion_joint_31_angles.inc"
};
static NJS_MKEY_A rider_turn_positive_motion_joint_32_angles[9] = {
#include "stg10/terrain/rider_turn_positive_motion_joint_32_angles.inc"
};
static NJS_MKEY_A rider_turn_positive_motion_joint_33_angles[9] = {
#include "stg10/terrain/rider_turn_positive_motion_joint_33_angles.inc"
};
static NJS_MKEY_A rider_turn_positive_motion_joint_37_angles[9] = {
#include "stg10/terrain/rider_turn_positive_motion_joint_37_angles.inc"
};
static NJS_MKEY_A rider_turn_positive_motion_joint_38_angles[2] = {
#include "stg10/terrain/rider_turn_positive_motion_joint_38_angles.inc"
};
static NJS_MKEY_A rider_turn_positive_motion_joint_39_angles[2] = {
#include "stg10/terrain/rider_turn_positive_motion_joint_39_angles.inc"
};
static NJS_MKEY_A rider_turn_positive_motion_joint_41_angles[9] = {
#include "stg10/terrain/rider_turn_positive_motion_joint_41_angles.inc"
};
static NJS_MKEY_A rider_turn_positive_motion_joint_44_angles[9] = {
#include "stg10/terrain/rider_turn_positive_motion_joint_44_angles.inc"
};
static NJS_MKEY_A rider_turn_positive_motion_joint_51_angles[2] = {
#include "stg10/terrain/rider_turn_positive_motion_joint_51_angles.inc"
};
static NJS_MKEY_A rider_turn_positive_motion_joint_53_angles[2] = {
#include "stg10/terrain/rider_turn_positive_motion_joint_53_angles.inc"
};
static NJS_MKEY_A rider_turn_positive_motion_joint_56_angles[9] = {
#include "stg10/terrain/rider_turn_positive_motion_joint_56_angles.inc"
};
static NJS_MKEY_A rider_turn_positive_motion_joint_57_angles[8] = {
#include "stg10/terrain/rider_turn_positive_motion_joint_57_angles.inc"
};
static NJS_MKEY_A rider_turn_positive_motion_joint_58_angles[8] = {
#include "stg10/terrain/rider_turn_positive_motion_joint_58_angles.inc"
};
static NJS_MKEY_A rider_turn_positive_motion_joint_60_angles[9] = {
#include "stg10/terrain/rider_turn_positive_motion_joint_60_angles.inc"
};
static NJS_MDATA2 rider_turn_positive_motion_tracks[62] = {
#include "stg10/terrain/rider_turn_positive_motion_tracks.inc"
};
static NJS_MOTION rider_turn_positive_motion = {
#include "stg10/terrain/rider_turn_positive_motion.inc"
};
static NJS_MKEY_F rider_turn_negative_motion_joint_2_position[8] = {
#include "stg10/terrain/rider_turn_negative_motion_joint_2_position.inc"
};
static NJS_MKEY_A rider_turn_negative_motion_joint_4_angles[8] = {
#include "stg10/terrain/rider_turn_negative_motion_joint_4_angles.inc"
};
static NJS_MKEY_A rider_turn_negative_motion_joint_7_angles[8] = {
#include "stg10/terrain/rider_turn_negative_motion_joint_7_angles.inc"
};
static NJS_MKEY_A rider_turn_negative_motion_joint_8_angles[8] = {
#include "stg10/terrain/rider_turn_negative_motion_joint_8_angles.inc"
};
static NJS_MKEY_A rider_turn_negative_motion_joint_9_angles[8] = {
#include "stg10/terrain/rider_turn_negative_motion_joint_9_angles.inc"
};
static NJS_MKEY_A rider_turn_negative_motion_joint_13_angles[8] = {
#include "stg10/terrain/rider_turn_negative_motion_joint_13_angles.inc"
};
static NJS_MKEY_A rider_turn_negative_motion_joint_14_angles[6] = {
#include "stg10/terrain/rider_turn_negative_motion_joint_14_angles.inc"
};
static NJS_MKEY_A rider_turn_negative_motion_joint_17_angles[8] = {
#include "stg10/terrain/rider_turn_negative_motion_joint_17_angles.inc"
};
static NJS_MKEY_A rider_turn_negative_motion_joint_18_angles[8] = {
#include "stg10/terrain/rider_turn_negative_motion_joint_18_angles.inc"
};
static NJS_MKEY_A rider_turn_negative_motion_joint_19_angles[8] = {
#include "stg10/terrain/rider_turn_negative_motion_joint_19_angles.inc"
};
static NJS_MKEY_A rider_turn_negative_motion_joint_23_angles[8] = {
#include "stg10/terrain/rider_turn_negative_motion_joint_23_angles.inc"
};
static NJS_MKEY_A rider_turn_negative_motion_joint_27_angles[8] = {
#include "stg10/terrain/rider_turn_negative_motion_joint_27_angles.inc"
};
static NJS_MKEY_A rider_turn_negative_motion_joint_28_angles[8] = {
#include "stg10/terrain/rider_turn_negative_motion_joint_28_angles.inc"
};
static NJS_MKEY_A rider_turn_negative_motion_joint_29_angles[8] = {
#include "stg10/terrain/rider_turn_negative_motion_joint_29_angles.inc"
};
static NJS_MKEY_A rider_turn_negative_motion_joint_31_angles[8] = {
#include "stg10/terrain/rider_turn_negative_motion_joint_31_angles.inc"
};
static NJS_MKEY_A rider_turn_negative_motion_joint_32_angles[8] = {
#include "stg10/terrain/rider_turn_negative_motion_joint_32_angles.inc"
};
static NJS_MKEY_A rider_turn_negative_motion_joint_33_angles[6] = {
#include "stg10/terrain/rider_turn_negative_motion_joint_33_angles.inc"
};
static NJS_MKEY_A rider_turn_negative_motion_joint_37_angles[8] = {
#include "stg10/terrain/rider_turn_negative_motion_joint_37_angles.inc"
};
static NJS_MKEY_A rider_turn_negative_motion_joint_38_angles[8] = {
#include "stg10/terrain/rider_turn_negative_motion_joint_38_angles.inc"
};
static NJS_MKEY_A rider_turn_negative_motion_joint_39_angles[8] = {
#include "stg10/terrain/rider_turn_negative_motion_joint_39_angles.inc"
};
static NJS_MKEY_A rider_turn_negative_motion_joint_41_angles[8] = {
#include "stg10/terrain/rider_turn_negative_motion_joint_41_angles.inc"
};
static NJS_MKEY_A rider_turn_negative_motion_joint_44_angles[8] = {
#include "stg10/terrain/rider_turn_negative_motion_joint_44_angles.inc"
};
static NJS_MKEY_A rider_turn_negative_motion_joint_51_angles[2] = {
#include "stg10/terrain/rider_turn_negative_motion_joint_51_angles.inc"
};
static NJS_MKEY_A rider_turn_negative_motion_joint_53_angles[2] = {
#include "stg10/terrain/rider_turn_negative_motion_joint_53_angles.inc"
};
static NJS_MKEY_A rider_turn_negative_motion_joint_56_angles[8] = {
#include "stg10/terrain/rider_turn_negative_motion_joint_56_angles.inc"
};
static NJS_MKEY_A rider_turn_negative_motion_joint_57_angles[8] = {
#include "stg10/terrain/rider_turn_negative_motion_joint_57_angles.inc"
};
static NJS_MKEY_A rider_turn_negative_motion_joint_58_angles[8] = {
#include "stg10/terrain/rider_turn_negative_motion_joint_58_angles.inc"
};
static NJS_MKEY_A rider_turn_negative_motion_joint_60_angles[8] = {
#include "stg10/terrain/rider_turn_negative_motion_joint_60_angles.inc"
};
static NJS_MDATA2 rider_turn_negative_motion_tracks[62] = {
#include "stg10/terrain/rider_turn_negative_motion_tracks.inc"
};
static NJS_MOTION rider_turn_negative_motion = {
#include "stg10/terrain/rider_turn_negative_motion.inc"
};
static NJS_MKEY_F rider_jump_motion_joint_2_position[8] = {
#include "stg10/terrain/rider_jump_motion_joint_2_position.inc"
};
static NJS_MKEY_A rider_jump_motion_joint_4_angles[2] = {
#include "stg10/terrain/rider_jump_motion_joint_4_angles.inc"
};
static NJS_MKEY_A rider_jump_motion_joint_7_angles[8] = {
#include "stg10/terrain/rider_jump_motion_joint_7_angles.inc"
};
static NJS_MKEY_A rider_jump_motion_joint_8_angles[8] = {
#include "stg10/terrain/rider_jump_motion_joint_8_angles.inc"
};
static NJS_MKEY_A rider_jump_motion_joint_9_angles[8] = {
#include "stg10/terrain/rider_jump_motion_joint_9_angles.inc"
};
static NJS_MKEY_A rider_jump_motion_joint_13_angles[8] = {
#include "stg10/terrain/rider_jump_motion_joint_13_angles.inc"
};
static NJS_MKEY_A rider_jump_motion_joint_14_angles[2] = {
#include "stg10/terrain/rider_jump_motion_joint_14_angles.inc"
};
static NJS_MKEY_A rider_jump_motion_joint_17_angles[8] = {
#include "stg10/terrain/rider_jump_motion_joint_17_angles.inc"
};
static NJS_MKEY_A rider_jump_motion_joint_18_angles[8] = {
#include "stg10/terrain/rider_jump_motion_joint_18_angles.inc"
};
static NJS_MKEY_A rider_jump_motion_joint_19_angles[8] = {
#include "stg10/terrain/rider_jump_motion_joint_19_angles.inc"
};
static NJS_MKEY_A rider_jump_motion_joint_23_angles[8] = {
#include "stg10/terrain/rider_jump_motion_joint_23_angles.inc"
};
static NJS_MKEY_A rider_jump_motion_joint_24_angles[2] = {
#include "stg10/terrain/rider_jump_motion_joint_24_angles.inc"
};
static NJS_MKEY_A rider_jump_motion_joint_27_angles[2] = {
#include "stg10/terrain/rider_jump_motion_joint_27_angles.inc"
};
static NJS_MKEY_A rider_jump_motion_joint_28_angles[2] = {
#include "stg10/terrain/rider_jump_motion_joint_28_angles.inc"
};
static NJS_MKEY_A rider_jump_motion_joint_29_angles[2] = {
#include "stg10/terrain/rider_jump_motion_joint_29_angles.inc"
};
static NJS_MKEY_A rider_jump_motion_joint_31_angles[8] = {
#include "stg10/terrain/rider_jump_motion_joint_31_angles.inc"
};
static NJS_MKEY_A rider_jump_motion_joint_32_angles[8] = {
#include "stg10/terrain/rider_jump_motion_joint_32_angles.inc"
};
static NJS_MKEY_A rider_jump_motion_joint_33_angles[8] = {
#include "stg10/terrain/rider_jump_motion_joint_33_angles.inc"
};
static NJS_MKEY_A rider_jump_motion_joint_37_angles[8] = {
#include "stg10/terrain/rider_jump_motion_joint_37_angles.inc"
};
static NJS_MKEY_A rider_jump_motion_joint_38_angles[8] = {
#include "stg10/terrain/rider_jump_motion_joint_38_angles.inc"
};
static NJS_MKEY_A rider_jump_motion_joint_39_angles[8] = {
#include "stg10/terrain/rider_jump_motion_joint_39_angles.inc"
};
static NJS_MKEY_A rider_jump_motion_joint_41_angles[8] = {
#include "stg10/terrain/rider_jump_motion_joint_41_angles.inc"
};
static NJS_MKEY_A rider_jump_motion_joint_44_angles[8] = {
#include "stg10/terrain/rider_jump_motion_joint_44_angles.inc"
};
static NJS_MKEY_A rider_jump_motion_joint_51_angles[2] = {
#include "stg10/terrain/rider_jump_motion_joint_51_angles.inc"
};
static NJS_MKEY_A rider_jump_motion_joint_53_angles[2] = {
#include "stg10/terrain/rider_jump_motion_joint_53_angles.inc"
};
static NJS_MKEY_A rider_jump_motion_joint_56_angles[8] = {
#include "stg10/terrain/rider_jump_motion_joint_56_angles.inc"
};
static NJS_MKEY_A rider_jump_motion_joint_57_angles[8] = {
#include "stg10/terrain/rider_jump_motion_joint_57_angles.inc"
};
static NJS_MKEY_A rider_jump_motion_joint_58_angles[8] = {
#include "stg10/terrain/rider_jump_motion_joint_58_angles.inc"
};
static NJS_MKEY_A rider_jump_motion_joint_60_angles[8] = {
#include "stg10/terrain/rider_jump_motion_joint_60_angles.inc"
};
static NJS_MDATA2 rider_jump_motion_tracks[62] = {
#include "stg10/terrain/rider_jump_motion_tracks.inc"
};
static NJS_MOTION rider_jump_motion = {
#include "stg10/terrain/rider_jump_motion.inc"
};
static NJS_MKEY_F rider_apex_motion_joint_2_position[12] = {
#include "stg10/terrain/rider_apex_motion_joint_2_position.inc"
};
static NJS_MKEY_A rider_apex_motion_joint_4_angles[12] = {
#include "stg10/terrain/rider_apex_motion_joint_4_angles.inc"
};
static NJS_MKEY_A rider_apex_motion_joint_7_angles[12] = {
#include "stg10/terrain/rider_apex_motion_joint_7_angles.inc"
};
static NJS_MKEY_A rider_apex_motion_joint_8_angles[12] = {
#include "stg10/terrain/rider_apex_motion_joint_8_angles.inc"
};
static NJS_MKEY_A rider_apex_motion_joint_9_angles[12] = {
#include "stg10/terrain/rider_apex_motion_joint_9_angles.inc"
};
static NJS_MKEY_A rider_apex_motion_joint_13_angles[12] = {
#include "stg10/terrain/rider_apex_motion_joint_13_angles.inc"
};
static NJS_MKEY_A rider_apex_motion_joint_14_angles[12] = {
#include "stg10/terrain/rider_apex_motion_joint_14_angles.inc"
};
static NJS_MKEY_A rider_apex_motion_joint_17_angles[12] = {
#include "stg10/terrain/rider_apex_motion_joint_17_angles.inc"
};
static NJS_MKEY_A rider_apex_motion_joint_18_angles[11] = {
#include "stg10/terrain/rider_apex_motion_joint_18_angles.inc"
};
static NJS_MKEY_A rider_apex_motion_joint_19_angles[12] = {
#include "stg10/terrain/rider_apex_motion_joint_19_angles.inc"
};
static NJS_MKEY_A rider_apex_motion_joint_23_angles[12] = {
#include "stg10/terrain/rider_apex_motion_joint_23_angles.inc"
};
static NJS_MKEY_A rider_apex_motion_joint_24_angles[12] = {
#include "stg10/terrain/rider_apex_motion_joint_24_angles.inc"
};
static NJS_MKEY_A rider_apex_motion_joint_27_angles[12] = {
#include "stg10/terrain/rider_apex_motion_joint_27_angles.inc"
};
static NJS_MKEY_A rider_apex_motion_joint_28_angles[12] = {
#include "stg10/terrain/rider_apex_motion_joint_28_angles.inc"
};
static NJS_MKEY_A rider_apex_motion_joint_29_angles[12] = {
#include "stg10/terrain/rider_apex_motion_joint_29_angles.inc"
};
static NJS_MKEY_A rider_apex_motion_joint_31_angles[12] = {
#include "stg10/terrain/rider_apex_motion_joint_31_angles.inc"
};
static NJS_MKEY_A rider_apex_motion_joint_32_angles[12] = {
#include "stg10/terrain/rider_apex_motion_joint_32_angles.inc"
};
static NJS_MKEY_A rider_apex_motion_joint_33_angles[9] = {
#include "stg10/terrain/rider_apex_motion_joint_33_angles.inc"
};
static NJS_MKEY_A rider_apex_motion_joint_37_angles[12] = {
#include "stg10/terrain/rider_apex_motion_joint_37_angles.inc"
};
static NJS_MKEY_A rider_apex_motion_joint_38_angles[12] = {
#include "stg10/terrain/rider_apex_motion_joint_38_angles.inc"
};
static NJS_MKEY_A rider_apex_motion_joint_39_angles[12] = {
#include "stg10/terrain/rider_apex_motion_joint_39_angles.inc"
};
static NJS_MKEY_A rider_apex_motion_joint_41_angles[12] = {
#include "stg10/terrain/rider_apex_motion_joint_41_angles.inc"
};
static NJS_MKEY_A rider_apex_motion_joint_44_angles[12] = {
#include "stg10/terrain/rider_apex_motion_joint_44_angles.inc"
};
static NJS_MKEY_A rider_apex_motion_joint_51_angles[2] = {
#include "stg10/terrain/rider_apex_motion_joint_51_angles.inc"
};
static NJS_MKEY_A rider_apex_motion_joint_53_angles[2] = {
#include "stg10/terrain/rider_apex_motion_joint_53_angles.inc"
};
static NJS_MKEY_A rider_apex_motion_joint_56_angles[12] = {
#include "stg10/terrain/rider_apex_motion_joint_56_angles.inc"
};
static NJS_MKEY_A rider_apex_motion_joint_57_angles[12] = {
#include "stg10/terrain/rider_apex_motion_joint_57_angles.inc"
};
static NJS_MKEY_A rider_apex_motion_joint_58_angles[12] = {
#include "stg10/terrain/rider_apex_motion_joint_58_angles.inc"
};
static NJS_MKEY_A rider_apex_motion_joint_60_angles[12] = {
#include "stg10/terrain/rider_apex_motion_joint_60_angles.inc"
};
static NJS_MDATA2 rider_apex_motion_tracks[62] = {
#include "stg10/terrain/rider_apex_motion_tracks.inc"
};
static NJS_MOTION rider_apex_motion = {
#include "stg10/terrain/rider_apex_motion.inc"
};
static NJS_MKEY_F rider_fall_motion_joint_2_position[8] = {
#include "stg10/terrain/rider_fall_motion_joint_2_position.inc"
};
static NJS_MKEY_A rider_fall_motion_joint_4_angles[8] = {
#include "stg10/terrain/rider_fall_motion_joint_4_angles.inc"
};
static NJS_MKEY_A rider_fall_motion_joint_7_angles[8] = {
#include "stg10/terrain/rider_fall_motion_joint_7_angles.inc"
};
static NJS_MKEY_A rider_fall_motion_joint_8_angles[8] = {
#include "stg10/terrain/rider_fall_motion_joint_8_angles.inc"
};
static NJS_MKEY_A rider_fall_motion_joint_9_angles[8] = {
#include "stg10/terrain/rider_fall_motion_joint_9_angles.inc"
};
static NJS_MKEY_A rider_fall_motion_joint_13_angles[8] = {
#include "stg10/terrain/rider_fall_motion_joint_13_angles.inc"
};
static NJS_MKEY_A rider_fall_motion_joint_14_angles[3] = {
#include "stg10/terrain/rider_fall_motion_joint_14_angles.inc"
};
static NJS_MKEY_A rider_fall_motion_joint_17_angles[8] = {
#include "stg10/terrain/rider_fall_motion_joint_17_angles.inc"
};
static NJS_MKEY_A rider_fall_motion_joint_18_angles[8] = {
#include "stg10/terrain/rider_fall_motion_joint_18_angles.inc"
};
static NJS_MKEY_A rider_fall_motion_joint_19_angles[8] = {
#include "stg10/terrain/rider_fall_motion_joint_19_angles.inc"
};
static NJS_MKEY_A rider_fall_motion_joint_23_angles[8] = {
#include "stg10/terrain/rider_fall_motion_joint_23_angles.inc"
};
static NJS_MKEY_A rider_fall_motion_joint_24_angles[8] = {
#include "stg10/terrain/rider_fall_motion_joint_24_angles.inc"
};
static NJS_MKEY_A rider_fall_motion_joint_27_angles[8] = {
#include "stg10/terrain/rider_fall_motion_joint_27_angles.inc"
};
static NJS_MKEY_A rider_fall_motion_joint_28_angles[8] = {
#include "stg10/terrain/rider_fall_motion_joint_28_angles.inc"
};
static NJS_MKEY_A rider_fall_motion_joint_29_angles[8] = {
#include "stg10/terrain/rider_fall_motion_joint_29_angles.inc"
};
static NJS_MKEY_A rider_fall_motion_joint_31_angles[8] = {
#include "stg10/terrain/rider_fall_motion_joint_31_angles.inc"
};
static NJS_MKEY_A rider_fall_motion_joint_32_angles[8] = {
#include "stg10/terrain/rider_fall_motion_joint_32_angles.inc"
};
static NJS_MKEY_A rider_fall_motion_joint_33_angles[7] = {
#include "stg10/terrain/rider_fall_motion_joint_33_angles.inc"
};
static NJS_MKEY_A rider_fall_motion_joint_37_angles[8] = {
#include "stg10/terrain/rider_fall_motion_joint_37_angles.inc"
};
static NJS_MKEY_A rider_fall_motion_joint_38_angles[8] = {
#include "stg10/terrain/rider_fall_motion_joint_38_angles.inc"
};
static NJS_MKEY_A rider_fall_motion_joint_39_angles[8] = {
#include "stg10/terrain/rider_fall_motion_joint_39_angles.inc"
};
static NJS_MKEY_A rider_fall_motion_joint_41_angles[8] = {
#include "stg10/terrain/rider_fall_motion_joint_41_angles.inc"
};
static NJS_MKEY_A rider_fall_motion_joint_44_angles[8] = {
#include "stg10/terrain/rider_fall_motion_joint_44_angles.inc"
};
static NJS_MKEY_A rider_fall_motion_joint_51_angles[2] = {
#include "stg10/terrain/rider_fall_motion_joint_51_angles.inc"
};
static NJS_MKEY_A rider_fall_motion_joint_53_angles[2] = {
#include "stg10/terrain/rider_fall_motion_joint_53_angles.inc"
};
static NJS_MKEY_A rider_fall_motion_joint_56_angles[8] = {
#include "stg10/terrain/rider_fall_motion_joint_56_angles.inc"
};
static NJS_MKEY_A rider_fall_motion_joint_57_angles[8] = {
#include "stg10/terrain/rider_fall_motion_joint_57_angles.inc"
};
static NJS_MKEY_A rider_fall_motion_joint_58_angles[8] = {
#include "stg10/terrain/rider_fall_motion_joint_58_angles.inc"
};
static NJS_MKEY_A rider_fall_motion_joint_60_angles[8] = {
#include "stg10/terrain/rider_fall_motion_joint_60_angles.inc"
};
static NJS_MDATA2 rider_fall_motion_tracks[62] = {
#include "stg10/terrain/rider_fall_motion_tracks.inc"
};
static NJS_MOTION rider_fall_motion = {
#include "stg10/terrain/rider_fall_motion.inc"
};
static NJS_MKEY_F rider_crash_motion_joint_2_position[20] = {
#include "stg10/terrain/rider_crash_motion_joint_2_position.inc"
};
static NJS_MKEY_A rider_crash_motion_joint_4_angles[20] = {
#include "stg10/terrain/rider_crash_motion_joint_4_angles.inc"
};
static NJS_MKEY_A rider_crash_motion_joint_7_angles[20] = {
#include "stg10/terrain/rider_crash_motion_joint_7_angles.inc"
};
static NJS_MKEY_A rider_crash_motion_joint_8_angles[20] = {
#include "stg10/terrain/rider_crash_motion_joint_8_angles.inc"
};
static NJS_MKEY_A rider_crash_motion_joint_9_angles[20] = {
#include "stg10/terrain/rider_crash_motion_joint_9_angles.inc"
};
static NJS_MKEY_A rider_crash_motion_joint_13_angles[20] = {
#include "stg10/terrain/rider_crash_motion_joint_13_angles.inc"
};
static NJS_MKEY_A rider_crash_motion_joint_14_angles[20] = {
#include "stg10/terrain/rider_crash_motion_joint_14_angles.inc"
};
static NJS_MKEY_A rider_crash_motion_joint_17_angles[20] = {
#include "stg10/terrain/rider_crash_motion_joint_17_angles.inc"
};
static NJS_MKEY_A rider_crash_motion_joint_18_angles[20] = {
#include "stg10/terrain/rider_crash_motion_joint_18_angles.inc"
};
static NJS_MKEY_A rider_crash_motion_joint_19_angles[20] = {
#include "stg10/terrain/rider_crash_motion_joint_19_angles.inc"
};
static NJS_MKEY_A rider_crash_motion_joint_23_angles[20] = {
#include "stg10/terrain/rider_crash_motion_joint_23_angles.inc"
};
static NJS_MKEY_A rider_crash_motion_joint_24_angles[20] = {
#include "stg10/terrain/rider_crash_motion_joint_24_angles.inc"
};
static NJS_MKEY_A rider_crash_motion_joint_27_angles[20] = {
#include "stg10/terrain/rider_crash_motion_joint_27_angles.inc"
};
static NJS_MKEY_A rider_crash_motion_joint_28_angles[14] = {
#include "stg10/terrain/rider_crash_motion_joint_28_angles.inc"
};
static NJS_MKEY_A rider_crash_motion_joint_29_angles[14] = {
#include "stg10/terrain/rider_crash_motion_joint_29_angles.inc"
};
static NJS_MKEY_A rider_crash_motion_joint_31_angles[20] = {
#include "stg10/terrain/rider_crash_motion_joint_31_angles.inc"
};
static NJS_MKEY_A rider_crash_motion_joint_32_angles[20] = {
#include "stg10/terrain/rider_crash_motion_joint_32_angles.inc"
};
static NJS_MKEY_A rider_crash_motion_joint_33_angles[17] = {
#include "stg10/terrain/rider_crash_motion_joint_33_angles.inc"
};
static NJS_MKEY_A rider_crash_motion_joint_37_angles[20] = {
#include "stg10/terrain/rider_crash_motion_joint_37_angles.inc"
};
static NJS_MKEY_A rider_crash_motion_joint_38_angles[20] = {
#include "stg10/terrain/rider_crash_motion_joint_38_angles.inc"
};
static NJS_MKEY_A rider_crash_motion_joint_39_angles[20] = {
#include "stg10/terrain/rider_crash_motion_joint_39_angles.inc"
};
static NJS_MKEY_A rider_crash_motion_joint_41_angles[20] = {
#include "stg10/terrain/rider_crash_motion_joint_41_angles.inc"
};
static NJS_MKEY_A rider_crash_motion_joint_44_angles[20] = {
#include "stg10/terrain/rider_crash_motion_joint_44_angles.inc"
};
static NJS_MKEY_A rider_crash_motion_joint_51_angles[2] = {
#include "stg10/terrain/rider_crash_motion_joint_51_angles.inc"
};
static NJS_MKEY_A rider_crash_motion_joint_53_angles[2] = {
#include "stg10/terrain/rider_crash_motion_joint_53_angles.inc"
};
static NJS_MKEY_A rider_crash_motion_joint_56_angles[20] = {
#include "stg10/terrain/rider_crash_motion_joint_56_angles.inc"
};
static NJS_MKEY_A rider_crash_motion_joint_57_angles[19] = {
#include "stg10/terrain/rider_crash_motion_joint_57_angles.inc"
};
static NJS_MKEY_A rider_crash_motion_joint_58_angles[19] = {
#include "stg10/terrain/rider_crash_motion_joint_58_angles.inc"
};
static NJS_MKEY_A rider_crash_motion_joint_60_angles[20] = {
#include "stg10/terrain/rider_crash_motion_joint_60_angles.inc"
};
static NJS_MDATA2 rider_crash_motion_tracks[62] = {
#include "stg10/terrain/rider_crash_motion_tracks.inc"
};
static NJS_MOTION rider_crash_motion = {
#include "stg10/terrain/rider_crash_motion.inc"
};
static NJS_MKEY_F rider_land_motion_joint_2_position[8] = {
#include "stg10/terrain/rider_land_motion_joint_2_position.inc"
};
static NJS_MKEY_A rider_land_motion_joint_4_angles[2] = {
#include "stg10/terrain/rider_land_motion_joint_4_angles.inc"
};
static NJS_MKEY_A rider_land_motion_joint_7_angles[8] = {
#include "stg10/terrain/rider_land_motion_joint_7_angles.inc"
};
static NJS_MKEY_A rider_land_motion_joint_8_angles[8] = {
#include "stg10/terrain/rider_land_motion_joint_8_angles.inc"
};
static NJS_MKEY_A rider_land_motion_joint_9_angles[8] = {
#include "stg10/terrain/rider_land_motion_joint_9_angles.inc"
};
static NJS_MKEY_A rider_land_motion_joint_13_angles[8] = {
#include "stg10/terrain/rider_land_motion_joint_13_angles.inc"
};
static NJS_MKEY_A rider_land_motion_joint_14_angles[2] = {
#include "stg10/terrain/rider_land_motion_joint_14_angles.inc"
};
static NJS_MKEY_A rider_land_motion_joint_17_angles[8] = {
#include "stg10/terrain/rider_land_motion_joint_17_angles.inc"
};
static NJS_MKEY_A rider_land_motion_joint_18_angles[8] = {
#include "stg10/terrain/rider_land_motion_joint_18_angles.inc"
};
static NJS_MKEY_A rider_land_motion_joint_19_angles[8] = {
#include "stg10/terrain/rider_land_motion_joint_19_angles.inc"
};
static NJS_MKEY_A rider_land_motion_joint_23_angles[8] = {
#include "stg10/terrain/rider_land_motion_joint_23_angles.inc"
};
static NJS_MKEY_A rider_land_motion_joint_24_angles[2] = {
#include "stg10/terrain/rider_land_motion_joint_24_angles.inc"
};
static NJS_MKEY_A rider_land_motion_joint_27_angles[8] = {
#include "stg10/terrain/rider_land_motion_joint_27_angles.inc"
};
static NJS_MKEY_A rider_land_motion_joint_28_angles[8] = {
#include "stg10/terrain/rider_land_motion_joint_28_angles.inc"
};
static NJS_MKEY_A rider_land_motion_joint_29_angles[8] = {
#include "stg10/terrain/rider_land_motion_joint_29_angles.inc"
};
static NJS_MKEY_A rider_land_motion_joint_31_angles[2] = {
#include "stg10/terrain/rider_land_motion_joint_31_angles.inc"
};
static NJS_MKEY_A rider_land_motion_joint_32_angles[6] = {
#include "stg10/terrain/rider_land_motion_joint_32_angles.inc"
};
static NJS_MKEY_A rider_land_motion_joint_33_angles[6] = {
#include "stg10/terrain/rider_land_motion_joint_33_angles.inc"
};
static NJS_MKEY_A rider_land_motion_joint_37_angles[8] = {
#include "stg10/terrain/rider_land_motion_joint_37_angles.inc"
};
static NJS_MKEY_A rider_land_motion_joint_38_angles[8] = {
#include "stg10/terrain/rider_land_motion_joint_38_angles.inc"
};
static NJS_MKEY_A rider_land_motion_joint_39_angles[8] = {
#include "stg10/terrain/rider_land_motion_joint_39_angles.inc"
};
static NJS_MKEY_A rider_land_motion_joint_41_angles[8] = {
#include "stg10/terrain/rider_land_motion_joint_41_angles.inc"
};
static NJS_MKEY_A rider_land_motion_joint_44_angles[8] = {
#include "stg10/terrain/rider_land_motion_joint_44_angles.inc"
};
static NJS_MKEY_A rider_land_motion_joint_51_angles[2] = {
#include "stg10/terrain/rider_land_motion_joint_51_angles.inc"
};
static NJS_MKEY_A rider_land_motion_joint_53_angles[2] = {
#include "stg10/terrain/rider_land_motion_joint_53_angles.inc"
};
static NJS_MKEY_A rider_land_motion_joint_56_angles[8] = {
#include "stg10/terrain/rider_land_motion_joint_56_angles.inc"
};
static NJS_MKEY_A rider_land_motion_joint_57_angles[8] = {
#include "stg10/terrain/rider_land_motion_joint_57_angles.inc"
};
static NJS_MKEY_A rider_land_motion_joint_58_angles[8] = {
#include "stg10/terrain/rider_land_motion_joint_58_angles.inc"
};
static NJS_MKEY_A rider_land_motion_joint_60_angles[8] = {
#include "stg10/terrain/rider_land_motion_joint_60_angles.inc"
};
static NJS_MDATA2 rider_land_motion_tracks[62] = {
#include "stg10/terrain/rider_land_motion_tracks.inc"
};
static NJS_MOTION rider_land_motion = {
#include "stg10/terrain/rider_land_motion.inc"
};
static NJS_MKEY_F board_trick_harbor_a_motion_joint_0_position[140] = {
#include "stg10/terrain/board_trick_harbor_a_motion_joint_0_position.inc"
};
static NJS_MKEY_A board_trick_harbor_a_motion_joint_1_angles[140] = {
#include "stg10/terrain/board_trick_harbor_a_motion_joint_1_angles.inc"
};
static NJS_MDATA2 board_trick_harbor_a_motion_tracks[2] = {
#include "stg10/terrain/board_trick_harbor_a_motion_tracks.inc"
};
static NJS_MOTION board_trick_harbor_a_motion = {
#include "stg10/terrain/board_trick_harbor_a_motion.inc"
};
static NJS_MKEY_F board_trick_harbor_b_motion_joint_0_position[121] = {
#include "stg10/terrain/board_trick_harbor_b_motion_joint_0_position.inc"
};
static NJS_MKEY_A board_trick_harbor_b_motion_joint_0_angles[121] = {
#include "stg10/terrain/board_trick_harbor_b_motion_joint_0_angles.inc"
};
static NJS_MDATA2 board_trick_harbor_b_motion_tracks[2] = {
#include "stg10/terrain/board_trick_harbor_b_motion_tracks.inc"
};
static NJS_MOTION board_trick_harbor_b_motion = {
#include "stg10/terrain/board_trick_harbor_b_motion.inc"
};
static NJS_MKEY_F board_trick_harbor_c_motion_joint_0_position[90] = {
#include "stg10/terrain/board_trick_harbor_c_motion_joint_0_position.inc"
};
static NJS_MKEY_A board_trick_harbor_c_motion_joint_1_angles[90] = {
#include "stg10/terrain/board_trick_harbor_c_motion_joint_1_angles.inc"
};
static NJS_MDATA2 board_trick_harbor_c_motion_tracks[2] = {
#include "stg10/terrain/board_trick_harbor_c_motion_tracks.inc"
};
static NJS_MOTION board_trick_harbor_c_motion = {
#include "stg10/terrain/board_trick_harbor_c_motion.inc"
};
static NJS_MKEY_F board_lean_positive_motion_joint_0_position[17] = {
#include "stg10/terrain/board_lean_positive_motion_joint_0_position.inc"
};
static NJS_MKEY_A board_lean_positive_motion_joint_1_angles[17] = {
#include "stg10/terrain/board_lean_positive_motion_joint_1_angles.inc"
};
static NJS_MDATA2 board_lean_positive_motion_tracks[2] = {
#include "stg10/terrain/board_lean_positive_motion_tracks.inc"
};
static NJS_MOTION board_lean_positive_motion = {
#include "stg10/terrain/board_lean_positive_motion.inc"
};
static NJS_MKEY_F board_lean_negative_motion_joint_0_position[17] = {
#include "stg10/terrain/board_lean_negative_motion_joint_0_position.inc"
};
static NJS_MKEY_F board_lean_negative_motion_joint_1_position[17] = {
#include "stg10/terrain/board_lean_negative_motion_joint_1_position.inc"
};
static NJS_MKEY_A board_lean_negative_motion_joint_1_angles[17] = {
#include "stg10/terrain/board_lean_negative_motion_joint_1_angles.inc"
};
static NJS_MDATA2 board_lean_negative_motion_tracks[2] = {
#include "stg10/terrain/board_lean_negative_motion_tracks.inc"
};
static NJS_MOTION board_lean_negative_motion = {
#include "stg10/terrain/board_lean_negative_motion.inc"
};
static NJS_MKEY_F board_turn_positive_motion_joint_0_position[9] = {
#include "stg10/terrain/board_turn_positive_motion_joint_0_position.inc"
};
static NJS_MKEY_A board_turn_positive_motion_joint_1_angles[3] = {
#include "stg10/terrain/board_turn_positive_motion_joint_1_angles.inc"
};
static NJS_MDATA2 board_turn_positive_motion_tracks[2] = {
#include "stg10/terrain/board_turn_positive_motion_tracks.inc"
};
static NJS_MOTION board_turn_positive_motion = {
#include "stg10/terrain/board_turn_positive_motion.inc"
};
static NJS_MKEY_F board_turn_negative_motion_joint_0_position[8] = {
#include "stg10/terrain/board_turn_negative_motion_joint_0_position.inc"
};
static NJS_MKEY_A board_turn_negative_motion_joint_1_angles[8] = {
#include "stg10/terrain/board_turn_negative_motion_joint_1_angles.inc"
};
static NJS_MDATA2 board_turn_negative_motion_tracks[2] = {
#include "stg10/terrain/board_turn_negative_motion_tracks.inc"
};
static NJS_MOTION board_turn_negative_motion = {
#include "stg10/terrain/board_turn_negative_motion.inc"
};
static NJS_MKEY_F board_jump_motion_joint_0_position[8] = {
#include "stg10/terrain/board_jump_motion_joint_0_position.inc"
};
static NJS_MKEY_A board_jump_motion_joint_1_angles[8] = {
#include "stg10/terrain/board_jump_motion_joint_1_angles.inc"
};
static NJS_MDATA2 board_jump_motion_tracks[2] = {
#include "stg10/terrain/board_jump_motion_tracks.inc"
};
static NJS_MOTION board_jump_motion = {
#include "stg10/terrain/board_jump_motion.inc"
};
static NJS_MKEY_F board_apex_motion_joint_0_position[12] = {
#include "stg10/terrain/board_apex_motion_joint_0_position.inc"
};
static NJS_MKEY_A board_apex_motion_joint_0_angles[11] = {
#include "stg10/terrain/board_apex_motion_joint_0_angles.inc"
};
static NJS_MKEY_A board_apex_motion_joint_1_angles[12] = {
#include "stg10/terrain/board_apex_motion_joint_1_angles.inc"
};
static NJS_MDATA2 board_apex_motion_tracks[2] = {
#include "stg10/terrain/board_apex_motion_tracks.inc"
};
static NJS_MOTION board_apex_motion = {
#include "stg10/terrain/board_apex_motion.inc"
};
static NJS_MKEY_F board_fall_motion_joint_0_position[8] = {
#include "stg10/terrain/board_fall_motion_joint_0_position.inc"
};
static NJS_MKEY_A board_fall_motion_joint_0_angles[8] = {
#include "stg10/terrain/board_fall_motion_joint_0_angles.inc"
};
static NJS_MKEY_A board_fall_motion_joint_1_angles[8] = {
#include "stg10/terrain/board_fall_motion_joint_1_angles.inc"
};
static NJS_MDATA2 board_fall_motion_tracks[2] = {
#include "stg10/terrain/board_fall_motion_tracks.inc"
};
static NJS_MOTION board_fall_motion = {
#include "stg10/terrain/board_fall_motion.inc"
};
static NJS_MKEY_F board_crash_motion_joint_0_position[20] = {
#include "stg10/terrain/board_crash_motion_joint_0_position.inc"
};
static NJS_MKEY_A board_crash_motion_joint_1_angles[20] = {
#include "stg10/terrain/board_crash_motion_joint_1_angles.inc"
};
static NJS_MDATA2 board_crash_motion_tracks[2] = {
#include "stg10/terrain/board_crash_motion_tracks.inc"
};
static NJS_MOTION board_crash_motion = {
#include "stg10/terrain/board_crash_motion.inc"
};
static NJS_MDATA2 board_land_motion_tracks[2] = {
#include "stg10/terrain/board_land_motion_tracks.inc"
};
static NJS_MOTION board_land_motion = {
#include "stg10/terrain/board_land_motion.inc"
};
struct motion_entry module_motions[34] = {
#include "stg10/terrain/module_motions.inc"
};
static struct land_entry entries[769] = {
#include "stg10/terrain/entries.inc"
};
struct land_table metal_harbor_land = {
#include "stg10/terrain/metal_harbor_land.inc"
};
static NJS_TEXNAME textures[50] = {
#include "stg10/terrain/textures.inc"
};
NJS_TEXLIST metal_harbor_terrain_textures = {
#include "stg10/terrain/metal_harbor_terrain_textures.inc"
};
static Sint16 draw_446_polygons[55] = {
#include "stg10/terrain/draw_446_polygons.inc"
};
static Sint32 draw_446_vertices[51] = {
#include "stg10/terrain/draw_446_vertices.inc"
};
static NJS_CNK_MODEL draw_446_model = {
#include "stg10/terrain/draw_446_model.inc"
};
static NJS_CNK_OBJECT draw_446 = {
#include "stg10/terrain/draw_446.inc"
};
static Sint16 draw_420_polygons[217] = {
#include "stg10/terrain/draw_420_polygons.inc"
};
static Sint32 draw_420_vertices[235] = {
#include "stg10/terrain/draw_420_vertices.inc"
};
static NJS_CNK_MODEL draw_420_model = {
#include "stg10/terrain/draw_420_model.inc"
};
static NJS_CNK_OBJECT draw_445 = {
#include "stg10/terrain/draw_445.inc"
};
static Sint16 draw_417_polygons[543] = {
#include "stg10/terrain/draw_417_polygons.inc"
};
static Sint32 draw_417_vertices[611] = {
#include "stg10/terrain/draw_417_vertices.inc"
};
static NJS_CNK_MODEL draw_417_model = {
#include "stg10/terrain/draw_417_model.inc"
};
static NJS_CNK_OBJECT draw_419 = {
#include "stg10/terrain/draw_419.inc"
};
static Sint16 draw_414_polygons[115] = {
#include "stg10/terrain/draw_414_polygons.inc"
};
static Sint32 draw_414_vertices[107] = {
#include "stg10/terrain/draw_414_vertices.inc"
};
static NJS_CNK_MODEL draw_414_model = {
#include "stg10/terrain/draw_414_model.inc"
};
static NJS_CNK_OBJECT draw_416 = {
#include "stg10/terrain/draw_416.inc"
};
static Sint16 draw_413_polygons[119] = {
#include "stg10/terrain/draw_413_polygons.inc"
};
static Sint32 draw_413_vertices[95] = {
#include "stg10/terrain/draw_413_vertices.inc"
};
static NJS_CNK_MODEL draw_413_model = {
#include "stg10/terrain/draw_413_model.inc"
};
static NJS_CNK_OBJECT draw_413 = {
#include "stg10/terrain/draw_413.inc"
};
static Sint16 draw_412_polygons[119] = {
#include "stg10/terrain/draw_412_polygons.inc"
};
static Sint32 draw_412_vertices[99] = {
#include "stg10/terrain/draw_412_vertices.inc"
};
static NJS_CNK_MODEL draw_412_model = {
#include "stg10/terrain/draw_412_model.inc"
};
static NJS_CNK_OBJECT draw_412 = {
#include "stg10/terrain/draw_412.inc"
};
static Sint16 draw_409_polygons[231] = {
#include "stg10/terrain/draw_409_polygons.inc"
};
static Sint32 draw_409_vertices[243] = {
#include "stg10/terrain/draw_409_vertices.inc"
};
static NJS_CNK_MODEL draw_409_model = {
#include "stg10/terrain/draw_409_model.inc"
};
static NJS_CNK_OBJECT draw_411 = {
#include "stg10/terrain/draw_411.inc"
};
static Sint16 draw_376_polygons[1125] = {
#include "stg10/terrain/draw_376_polygons.inc"
};
static Sint32 draw_376_vertices[1023] = {
#include "stg10/terrain/draw_376_vertices.inc"
};
static NJS_CNK_MODEL draw_376_model = {
#include "stg10/terrain/draw_376_model.inc"
};
static NJS_CNK_OBJECT draw_408 = {
#include "stg10/terrain/draw_408.inc"
};
static Sint16 draw_373_polygons[61] = {
#include "stg10/terrain/draw_373_polygons.inc"
};
static Sint32 draw_373_vertices[55] = {
#include "stg10/terrain/draw_373_vertices.inc"
};
static NJS_CNK_MODEL draw_373_model = {
#include "stg10/terrain/draw_373_model.inc"
};
static NJS_CNK_OBJECT draw_375 = {
#include "stg10/terrain/draw_375.inc"
};
static Sint16 draw_369_polygons[549] = {
#include "stg10/terrain/draw_369_polygons.inc"
};
static Sint32 draw_369_vertices[467] = {
#include "stg10/terrain/draw_369_vertices.inc"
};
static NJS_CNK_MODEL draw_369_model = {
#include "stg10/terrain/draw_369_model.inc"
};
static NJS_CNK_OBJECT draw_372 = {
#include "stg10/terrain/draw_372.inc"
};
static Sint16 draw_368_polygons[113] = {
#include "stg10/terrain/draw_368_polygons.inc"
};
static Sint32 draw_368_vertices[59] = {
#include "stg10/terrain/draw_368_vertices.inc"
};
static NJS_CNK_MODEL draw_368_model = {
#include "stg10/terrain/draw_368_model.inc"
};
static NJS_CNK_OBJECT draw_368 = {
#include "stg10/terrain/draw_368.inc"
};
static Sint16 draw_364_polygons[119] = {
#include "stg10/terrain/draw_364_polygons.inc"
};
static Sint32 draw_364_vertices[87] = {
#include "stg10/terrain/draw_364_vertices.inc"
};
static NJS_CNK_MODEL draw_364_model = {
#include "stg10/terrain/draw_364_model.inc"
};
static NJS_CNK_OBJECT draw_367 = {
#include "stg10/terrain/draw_367.inc"
};
static Sint16 draw_361_polygons[257] = {
#include "stg10/terrain/draw_361_polygons.inc"
};
static Sint32 draw_361_vertices[299] = {
#include "stg10/terrain/draw_361_vertices.inc"
};
static NJS_CNK_MODEL draw_361_model = {
#include "stg10/terrain/draw_361_model.inc"
};
static NJS_CNK_OBJECT draw_363 = {
#include "stg10/terrain/draw_363.inc"
};
static Sint16 draw_359_polygons[37] = {
#include "stg10/terrain/draw_359_polygons.inc"
};
static Sint32 draw_359_vertices[35] = {
#include "stg10/terrain/draw_359_vertices.inc"
};
static NJS_CNK_MODEL draw_359_model = {
#include "stg10/terrain/draw_359_model.inc"
};
static NJS_CNK_OBJECT draw_360 = {
#include "stg10/terrain/draw_360.inc"
};
static Sint16 draw_355_polygons[91] = {
#include "stg10/terrain/draw_355_polygons.inc"
};
static Sint32 draw_355_vertices[39] = {
#include "stg10/terrain/draw_355_vertices.inc"
};
static NJS_CNK_MODEL draw_355_model = {
#include "stg10/terrain/draw_355_model.inc"
};
static NJS_CNK_OBJECT draw_358 = {
#include "stg10/terrain/draw_358.inc"
};
static Sint16 draw_352_polygons[23] = {
#include "stg10/terrain/draw_352_polygons.inc"
};
static Sint32 draw_352_vertices[19] = {
#include "stg10/terrain/draw_352_vertices.inc"
};
static NJS_CNK_MODEL draw_352_model = {
#include "stg10/terrain/draw_352_model.inc"
};
static NJS_CNK_OBJECT draw_354 = {
#include "stg10/terrain/draw_354.inc"
};
static Sint16 draw_351_polygons[167] = {
#include "stg10/terrain/draw_351_polygons.inc"
};
static Sint32 draw_351_vertices[195] = {
#include "stg10/terrain/draw_351_vertices.inc"
};
static NJS_CNK_MODEL draw_351_model = {
#include "stg10/terrain/draw_351_model.inc"
};
static NJS_CNK_OBJECT draw_351 = {
#include "stg10/terrain/draw_351.inc"
};
static Sint16 draw_350_polygons[181] = {
#include "stg10/terrain/draw_350_polygons.inc"
};
static Sint32 draw_350_vertices[159] = {
#include "stg10/terrain/draw_350_vertices.inc"
};
static NJS_CNK_MODEL draw_350_model = {
#include "stg10/terrain/draw_350_model.inc"
};
static NJS_CNK_OBJECT draw_350 = {
#include "stg10/terrain/draw_350.inc"
};
static Sint16 draw_348_polygons[337] = {
#include "stg10/terrain/draw_348_polygons.inc"
};
static Sint32 draw_348_vertices[315] = {
#include "stg10/terrain/draw_348_vertices.inc"
};
static NJS_CNK_MODEL draw_348_model = {
#include "stg10/terrain/draw_348_model.inc"
};
static NJS_CNK_OBJECT draw_349 = {
#include "stg10/terrain/draw_349.inc"
};
static Sint16 draw_346_polygons[399] = {
#include "stg10/terrain/draw_346_polygons.inc"
};
static Sint32 draw_346_vertices[247] = {
#include "stg10/terrain/draw_346_vertices.inc"
};
static NJS_CNK_MODEL draw_346_model = {
#include "stg10/terrain/draw_346_model.inc"
};
static NJS_CNK_OBJECT draw_347 = {
#include "stg10/terrain/draw_347.inc"
};
static Sint16 draw_338_polygons[1075] = {
#include "stg10/terrain/draw_338_polygons.inc"
};
static Sint32 draw_338_vertices[1175] = {
#include "stg10/terrain/draw_338_vertices.inc"
};
static NJS_CNK_MODEL draw_338_model = {
#include "stg10/terrain/draw_338_model.inc"
};
static NJS_CNK_OBJECT draw_345 = {
#include "stg10/terrain/draw_345.inc"
};
static Sint16 draw_337_polygons[139] = {
#include "stg10/terrain/draw_337_polygons.inc"
};
static Sint32 draw_337_vertices[123] = {
#include "stg10/terrain/draw_337_vertices.inc"
};
static NJS_CNK_MODEL draw_337_model = {
#include "stg10/terrain/draw_337_model.inc"
};
static NJS_CNK_OBJECT draw_337 = {
#include "stg10/terrain/draw_337.inc"
};
static Sint16 draw_334_polygons[69] = {
#include "stg10/terrain/draw_334_polygons.inc"
};
static Sint32 draw_334_vertices[35] = {
#include "stg10/terrain/draw_334_vertices.inc"
};
static NJS_CNK_MODEL draw_334_model = {
#include "stg10/terrain/draw_334_model.inc"
};
static NJS_CNK_OBJECT draw_336 = {
#include "stg10/terrain/draw_336.inc"
};
static Sint16 draw_331_polygons[55] = {
#include "stg10/terrain/draw_331_polygons.inc"
};
static Sint32 draw_331_vertices[51] = {
#include "stg10/terrain/draw_331_vertices.inc"
};
static NJS_CNK_MODEL draw_331_model = {
#include "stg10/terrain/draw_331_model.inc"
};
static NJS_CNK_OBJECT draw_333 = {
#include "stg10/terrain/draw_333.inc"
};
static Sint16 draw_330_polygons[591] = {
#include "stg10/terrain/draw_330_polygons.inc"
};
static Sint32 draw_330_vertices[379] = {
#include "stg10/terrain/draw_330_vertices.inc"
};
static NJS_CNK_MODEL draw_330_model = {
#include "stg10/terrain/draw_330_model.inc"
};
static NJS_CNK_OBJECT draw_330 = {
#include "stg10/terrain/draw_330.inc"
};
static Sint16 draw_327_polygons[155] = {
#include "stg10/terrain/draw_327_polygons.inc"
};
static Sint32 draw_327_vertices[67] = {
#include "stg10/terrain/draw_327_vertices.inc"
};
static NJS_CNK_MODEL draw_327_model = {
#include "stg10/terrain/draw_327_model.inc"
};
static NJS_CNK_OBJECT draw_329 = {
#include "stg10/terrain/draw_329.inc"
};
static Sint16 draw_324_polygons[273] = {
#include "stg10/terrain/draw_324_polygons.inc"
};
static Sint32 draw_324_vertices[123] = {
#include "stg10/terrain/draw_324_vertices.inc"
};
static NJS_CNK_MODEL draw_324_model = {
#include "stg10/terrain/draw_324_model.inc"
};
static NJS_CNK_OBJECT draw_326 = {
#include "stg10/terrain/draw_326.inc"
};
static Sint16 draw_323_polygons[613] = {
#include "stg10/terrain/draw_323_polygons.inc"
};
static Sint32 draw_323_vertices[307] = {
#include "stg10/terrain/draw_323_vertices.inc"
};
static NJS_CNK_MODEL draw_323_model = {
#include "stg10/terrain/draw_323_model.inc"
};
static NJS_CNK_OBJECT draw_323 = {
#include "stg10/terrain/draw_323.inc"
};
static Sint16 draw_321_polygons[203] = {
#include "stg10/terrain/draw_321_polygons.inc"
};
static Sint32 draw_321_vertices[163] = {
#include "stg10/terrain/draw_321_vertices.inc"
};
static NJS_CNK_MODEL draw_321_model = {
#include "stg10/terrain/draw_321_model.inc"
};
static NJS_CNK_OBJECT draw_322 = {
#include "stg10/terrain/draw_322.inc"
};
static Sint16 draw_302_polygons[551] = {
#include "stg10/terrain/draw_302_polygons.inc"
};
static Sint32 draw_302_vertices[567] = {
#include "stg10/terrain/draw_302_vertices.inc"
};
static NJS_CNK_MODEL draw_302_model = {
#include "stg10/terrain/draw_302_model.inc"
};
static NJS_CNK_OBJECT draw_320 = {
#include "stg10/terrain/draw_320.inc"
};
static Sint16 draw_300_polygons[87] = {
#include "stg10/terrain/draw_300_polygons.inc"
};
static Sint32 draw_300_vertices[67] = {
#include "stg10/terrain/draw_300_vertices.inc"
};
static NJS_CNK_MODEL draw_300_model = {
#include "stg10/terrain/draw_300_model.inc"
};
static NJS_CNK_OBJECT draw_301 = {
#include "stg10/terrain/draw_301.inc"
};
static Sint16 draw_299_polygons[165] = {
#include "stg10/terrain/draw_299_polygons.inc"
};
static Sint32 draw_299_vertices[87] = {
#include "stg10/terrain/draw_299_vertices.inc"
};
static NJS_CNK_MODEL draw_299_model = {
#include "stg10/terrain/draw_299_model.inc"
};
static NJS_CNK_OBJECT draw_299 = {
#include "stg10/terrain/draw_299.inc"
};
static Sint16 draw_298_polygons[279] = {
#include "stg10/terrain/draw_298_polygons.inc"
};
static Sint32 draw_298_vertices[183] = {
#include "stg10/terrain/draw_298_vertices.inc"
};
static NJS_CNK_MODEL draw_298_model = {
#include "stg10/terrain/draw_298_model.inc"
};
static NJS_CNK_OBJECT draw_298 = {
#include "stg10/terrain/draw_298.inc"
};
static Sint16 draw_295_polygons[297] = {
#include "stg10/terrain/draw_295_polygons.inc"
};
static Sint32 draw_295_vertices[347] = {
#include "stg10/terrain/draw_295_vertices.inc"
};
static NJS_CNK_MODEL draw_295_model = {
#include "stg10/terrain/draw_295_model.inc"
};
static NJS_CNK_OBJECT draw_297 = {
#include "stg10/terrain/draw_297.inc"
};
static Sint16 draw_293_polygons[23] = {
#include "stg10/terrain/draw_293_polygons.inc"
};
static Sint32 draw_293_vertices[19] = {
#include "stg10/terrain/draw_293_vertices.inc"
};
static NJS_CNK_MODEL draw_293_model = {
#include "stg10/terrain/draw_293_model.inc"
};
static NJS_CNK_OBJECT draw_294 = {
#include "stg10/terrain/draw_294.inc"
};
static Sint16 draw_287_polygons[23] = {
#include "stg10/terrain/draw_287_polygons.inc"
};
static Sint32 draw_287_vertices[19] = {
#include "stg10/terrain/draw_287_vertices.inc"
};
static NJS_CNK_MODEL draw_287_model = {
#include "stg10/terrain/draw_287_model.inc"
};
static NJS_CNK_OBJECT draw_292 = {
#include "stg10/terrain/draw_292.inc"
};
static Sint16 draw_286_polygons[379] = {
#include "stg10/terrain/draw_286_polygons.inc"
};
static Sint32 draw_286_vertices[395] = {
#include "stg10/terrain/draw_286_vertices.inc"
};
static NJS_CNK_MODEL draw_286_model = {
#include "stg10/terrain/draw_286_model.inc"
};
static NJS_CNK_OBJECT draw_286 = {
#include "stg10/terrain/draw_286.inc"
};
static Sint16 draw_285_polygons[47] = {
#include "stg10/terrain/draw_285_polygons.inc"
};
static Sint32 draw_285_vertices[51] = {
#include "stg10/terrain/draw_285_vertices.inc"
};
static NJS_CNK_MODEL draw_285_model = {
#include "stg10/terrain/draw_285_model.inc"
};
static NJS_CNK_OBJECT draw_285 = {
#include "stg10/terrain/draw_285.inc"
};
static Sint16 draw_284_polygons[339] = {
#include "stg10/terrain/draw_284_polygons.inc"
};
static Sint32 draw_284_vertices[403] = {
#include "stg10/terrain/draw_284_vertices.inc"
};
static NJS_CNK_MODEL draw_284_model = {
#include "stg10/terrain/draw_284_model.inc"
};
static NJS_CNK_OBJECT draw_284 = {
#include "stg10/terrain/draw_284.inc"
};
static Sint16 draw_283_polygons[121] = {
#include "stg10/terrain/draw_283_polygons.inc"
};
static Sint32 draw_283_vertices[71] = {
#include "stg10/terrain/draw_283_vertices.inc"
};
static NJS_CNK_MODEL draw_283_model = {
#include "stg10/terrain/draw_283_model.inc"
};
static NJS_CNK_OBJECT draw_283 = {
#include "stg10/terrain/draw_283.inc"
};
static Sint16 draw_281_polygons[309] = {
#include "stg10/terrain/draw_281_polygons.inc"
};
static Sint32 draw_281_vertices[279] = {
#include "stg10/terrain/draw_281_vertices.inc"
};
static NJS_CNK_MODEL draw_281_model = {
#include "stg10/terrain/draw_281_model.inc"
};
static NJS_CNK_OBJECT draw_282 = {
#include "stg10/terrain/draw_282.inc"
};
static Sint16 draw_279_polygons[167] = {
#include "stg10/terrain/draw_279_polygons.inc"
};
static Sint32 draw_279_vertices[127] = {
#include "stg10/terrain/draw_279_vertices.inc"
};
static NJS_CNK_MODEL draw_279_model = {
#include "stg10/terrain/draw_279_model.inc"
};
static NJS_CNK_OBJECT draw_280 = {
#include "stg10/terrain/draw_280.inc"
};
static Sint16 draw_273_polygons[897] = {
#include "stg10/terrain/draw_273_polygons.inc"
};
static Sint32 draw_273_vertices[703] = {
#include "stg10/terrain/draw_273_vertices.inc"
};
static NJS_CNK_MODEL draw_273_model = {
#include "stg10/terrain/draw_273_model.inc"
};
static NJS_CNK_OBJECT draw_278 = {
#include "stg10/terrain/draw_278.inc"
};
static Sint16 draw_270_polygons[613] = {
#include "stg10/terrain/draw_270_polygons.inc"
};
static Sint32 draw_270_vertices[311] = {
#include "stg10/terrain/draw_270_vertices.inc"
};
static NJS_CNK_MODEL draw_270_model = {
#include "stg10/terrain/draw_270_model.inc"
};
static NJS_CNK_OBJECT draw_272 = {
#include "stg10/terrain/draw_272.inc"
};
static Sint16 draw_268_polygons[921] = {
#include "stg10/terrain/draw_268_polygons.inc"
};
static Sint32 draw_268_vertices[827] = {
#include "stg10/terrain/draw_268_vertices.inc"
};
static NJS_CNK_MODEL draw_268_model = {
#include "stg10/terrain/draw_268_model.inc"
};
static NJS_CNK_OBJECT draw_269 = {
#include "stg10/terrain/draw_269.inc"
};
static Sint16 draw_267_polygons[177] = {
#include "stg10/terrain/draw_267_polygons.inc"
};
static Sint32 draw_267_vertices[163] = {
#include "stg10/terrain/draw_267_vertices.inc"
};
static NJS_CNK_MODEL draw_267_model = {
#include "stg10/terrain/draw_267_model.inc"
};
static NJS_CNK_OBJECT draw_267 = {
#include "stg10/terrain/draw_267.inc"
};
static Sint16 draw_260_polygons[893] = {
#include "stg10/terrain/draw_260_polygons.inc"
};
static Sint32 draw_260_vertices[827] = {
#include "stg10/terrain/draw_260_vertices.inc"
};
static NJS_CNK_MODEL draw_260_model = {
#include "stg10/terrain/draw_260_model.inc"
};
static NJS_CNK_OBJECT draw_266 = {
#include "stg10/terrain/draw_266.inc"
};
static Sint16 draw_259_polygons[61] = {
#include "stg10/terrain/draw_259_polygons.inc"
};
static Sint32 draw_259_vertices[55] = {
#include "stg10/terrain/draw_259_vertices.inc"
};
static NJS_CNK_MODEL draw_259_model = {
#include "stg10/terrain/draw_259_model.inc"
};
static NJS_CNK_OBJECT draw_259 = {
#include "stg10/terrain/draw_259.inc"
};
static Sint16 draw_257_polygons[171] = {
#include "stg10/terrain/draw_257_polygons.inc"
};
static Sint32 draw_257_vertices[87] = {
#include "stg10/terrain/draw_257_vertices.inc"
};
static NJS_CNK_MODEL draw_257_model = {
#include "stg10/terrain/draw_257_model.inc"
};
static NJS_CNK_OBJECT draw_258 = {
#include "stg10/terrain/draw_258.inc"
};
static Sint16 draw_251_polygons[101] = {
#include "stg10/terrain/draw_251_polygons.inc"
};
static Sint32 draw_251_vertices[91] = {
#include "stg10/terrain/draw_251_vertices.inc"
};
static NJS_CNK_MODEL draw_251_model = {
#include "stg10/terrain/draw_251_model.inc"
};
static NJS_CNK_OBJECT draw_256 = {
#include "stg10/terrain/draw_256.inc"
};
static Sint16 draw_248_polygons[263] = {
#include "stg10/terrain/draw_248_polygons.inc"
};
static Sint32 draw_248_vertices[263] = {
#include "stg10/terrain/draw_248_vertices.inc"
};
static NJS_CNK_MODEL draw_248_model = {
#include "stg10/terrain/draw_248_model.inc"
};
static NJS_CNK_OBJECT draw_250 = {
#include "stg10/terrain/draw_250.inc"
};
static Sint16 draw_247_polygons[37] = {
#include "stg10/terrain/draw_247_polygons.inc"
};
static Sint32 draw_247_vertices[35] = {
#include "stg10/terrain/draw_247_vertices.inc"
};
static NJS_CNK_MODEL draw_247_model = {
#include "stg10/terrain/draw_247_model.inc"
};
static NJS_CNK_OBJECT draw_247 = {
#include "stg10/terrain/draw_247.inc"
};
static Sint16 draw_246_polygons[77] = {
#include "stg10/terrain/draw_246_polygons.inc"
};
static Sint32 draw_246_vertices[51] = {
#include "stg10/terrain/draw_246_vertices.inc"
};
static NJS_CNK_MODEL draw_246_model = {
#include "stg10/terrain/draw_246_model.inc"
};
static NJS_CNK_OBJECT draw_246 = {
#include "stg10/terrain/draw_246.inc"
};
static Sint16 draw_245_polygons[65] = {
#include "stg10/terrain/draw_245_polygons.inc"
};
static Sint32 draw_245_vertices[47] = {
#include "stg10/terrain/draw_245_vertices.inc"
};
static NJS_CNK_MODEL draw_245_model = {
#include "stg10/terrain/draw_245_model.inc"
};
static NJS_CNK_OBJECT draw_245 = {
#include "stg10/terrain/draw_245.inc"
};
static Sint16 draw_241_polygons[445] = {
#include "stg10/terrain/draw_241_polygons.inc"
};
static Sint32 draw_241_vertices[431] = {
#include "stg10/terrain/draw_241_vertices.inc"
};
static NJS_CNK_MODEL draw_241_model = {
#include "stg10/terrain/draw_241_model.inc"
};
static NJS_CNK_OBJECT draw_244 = {
#include "stg10/terrain/draw_244.inc"
};
static Sint16 draw_240_polygons[777] = {
#include "stg10/terrain/draw_240_polygons.inc"
};
static Sint32 draw_240_vertices[759] = {
#include "stg10/terrain/draw_240_vertices.inc"
};
static NJS_CNK_MODEL draw_240_model = {
#include "stg10/terrain/draw_240_model.inc"
};
static NJS_CNK_OBJECT draw_240 = {
#include "stg10/terrain/draw_240.inc"
};
static Sint16 draw_239_polygons[173] = {
#include "stg10/terrain/draw_239_polygons.inc"
};
static Sint32 draw_239_vertices[123] = {
#include "stg10/terrain/draw_239_vertices.inc"
};
static NJS_CNK_MODEL draw_239_model = {
#include "stg10/terrain/draw_239_model.inc"
};
static NJS_CNK_OBJECT draw_239 = {
#include "stg10/terrain/draw_239.inc"
};
static Sint16 draw_236_polygons[171] = {
#include "stg10/terrain/draw_236_polygons.inc"
};
static Sint32 draw_236_vertices[151] = {
#include "stg10/terrain/draw_236_vertices.inc"
};
static NJS_CNK_MODEL draw_236_model = {
#include "stg10/terrain/draw_236_model.inc"
};
static NJS_CNK_OBJECT draw_238 = {
#include "stg10/terrain/draw_238.inc"
};
static Sint16 draw_235_polygons[927] = {
#include "stg10/terrain/draw_235_polygons.inc"
};
static Sint32 draw_235_vertices[703] = {
#include "stg10/terrain/draw_235_vertices.inc"
};
static NJS_CNK_MODEL draw_235_model = {
#include "stg10/terrain/draw_235_model.inc"
};
static NJS_CNK_OBJECT draw_235 = {
#include "stg10/terrain/draw_235.inc"
};
static Sint16 draw_234_polygons[43] = {
#include "stg10/terrain/draw_234_polygons.inc"
};
static Sint32 draw_234_vertices[35] = {
#include "stg10/terrain/draw_234_vertices.inc"
};
static NJS_CNK_MODEL draw_234_model = {
#include "stg10/terrain/draw_234_model.inc"
};
static NJS_CNK_OBJECT draw_234 = {
#include "stg10/terrain/draw_234.inc"
};
static Sint16 draw_232_polygons[159] = {
#include "stg10/terrain/draw_232_polygons.inc"
};
static Sint32 draw_232_vertices[155] = {
#include "stg10/terrain/draw_232_vertices.inc"
};
static NJS_CNK_MODEL draw_232_model = {
#include "stg10/terrain/draw_232_model.inc"
};
static NJS_CNK_OBJECT draw_233 = {
#include "stg10/terrain/draw_233.inc"
};
static Sint16 draw_231_polygons[229] = {
#include "stg10/terrain/draw_231_polygons.inc"
};
static Sint32 draw_231_vertices[159] = {
#include "stg10/terrain/draw_231_vertices.inc"
};
static NJS_CNK_MODEL draw_231_model = {
#include "stg10/terrain/draw_231_model.inc"
};
static NJS_CNK_OBJECT draw_231 = {
#include "stg10/terrain/draw_231.inc"
};
static Sint16 draw_230_polygons[215] = {
#include "stg10/terrain/draw_230_polygons.inc"
};
static Sint32 draw_230_vertices[207] = {
#include "stg10/terrain/draw_230_vertices.inc"
};
static NJS_CNK_MODEL draw_230_model = {
#include "stg10/terrain/draw_230_model.inc"
};
static NJS_CNK_OBJECT draw_230 = {
#include "stg10/terrain/draw_230.inc"
};
static Sint16 draw_229_polygons[87] = {
#include "stg10/terrain/draw_229_polygons.inc"
};
static Sint32 draw_229_vertices[83] = {
#include "stg10/terrain/draw_229_vertices.inc"
};
static NJS_CNK_MODEL draw_229_model = {
#include "stg10/terrain/draw_229_model.inc"
};
static NJS_CNK_OBJECT draw_229 = {
#include "stg10/terrain/draw_229.inc"
};
static Sint16 draw_226_polygons[163] = {
#include "stg10/terrain/draw_226_polygons.inc"
};
static Sint32 draw_226_vertices[179] = {
#include "stg10/terrain/draw_226_vertices.inc"
};
static NJS_CNK_MODEL draw_226_model = {
#include "stg10/terrain/draw_226_model.inc"
};
static NJS_CNK_OBJECT draw_228 = {
#include "stg10/terrain/draw_228.inc"
};
static Sint16 draw_225_polygons[121] = {
#include "stg10/terrain/draw_225_polygons.inc"
};
static Sint32 draw_225_vertices[75] = {
#include "stg10/terrain/draw_225_vertices.inc"
};
static NJS_CNK_MODEL draw_225_model = {
#include "stg10/terrain/draw_225_model.inc"
};
static NJS_CNK_OBJECT draw_225 = {
#include "stg10/terrain/draw_225.inc"
};
static Sint16 draw_223_polygons[163] = {
#include "stg10/terrain/draw_223_polygons.inc"
};
static Sint32 draw_223_vertices[171] = {
#include "stg10/terrain/draw_223_vertices.inc"
};
static NJS_CNK_MODEL draw_223_model = {
#include "stg10/terrain/draw_223_model.inc"
};
static NJS_CNK_OBJECT draw_224 = {
#include "stg10/terrain/draw_224.inc"
};
static Sint16 draw_220_polygons[139] = {
#include "stg10/terrain/draw_220_polygons.inc"
};
static Sint32 draw_220_vertices[147] = {
#include "stg10/terrain/draw_220_vertices.inc"
};
static NJS_CNK_MODEL draw_220_model = {
#include "stg10/terrain/draw_220_model.inc"
};
static NJS_CNK_OBJECT draw_222 = {
#include "stg10/terrain/draw_222.inc"
};
static Sint16 draw_219_polygons[163] = {
#include "stg10/terrain/draw_219_polygons.inc"
};
static Sint32 draw_219_vertices[175] = {
#include "stg10/terrain/draw_219_vertices.inc"
};
static NJS_CNK_MODEL draw_219_model = {
#include "stg10/terrain/draw_219_model.inc"
};
static NJS_CNK_OBJECT draw_219 = {
#include "stg10/terrain/draw_219.inc"
};
static Sint16 draw_218_polygons[171] = {
#include "stg10/terrain/draw_218_polygons.inc"
};
static Sint32 draw_218_vertices[207] = {
#include "stg10/terrain/draw_218_vertices.inc"
};
static NJS_CNK_MODEL draw_218_model = {
#include "stg10/terrain/draw_218_model.inc"
};
static NJS_CNK_OBJECT draw_218 = {
#include "stg10/terrain/draw_218.inc"
};
static Sint16 draw_217_polygons[217] = {
#include "stg10/terrain/draw_217_polygons.inc"
};
static Sint32 draw_217_vertices[231] = {
#include "stg10/terrain/draw_217_vertices.inc"
};
static NJS_CNK_MODEL draw_217_model = {
#include "stg10/terrain/draw_217_model.inc"
};
static NJS_CNK_OBJECT draw_217 = {
#include "stg10/terrain/draw_217.inc"
};
static Sint16 draw_216_polygons[167] = {
#include "stg10/terrain/draw_216_polygons.inc"
};
static Sint32 draw_216_vertices[135] = {
#include "stg10/terrain/draw_216_vertices.inc"
};
static NJS_CNK_MODEL draw_216_model = {
#include "stg10/terrain/draw_216_model.inc"
};
static NJS_CNK_OBJECT draw_216 = {
#include "stg10/terrain/draw_216.inc"
};
static Sint16 draw_215_polygons[69] = {
#include "stg10/terrain/draw_215_polygons.inc"
};
static Sint32 draw_215_vertices[35] = {
#include "stg10/terrain/draw_215_vertices.inc"
};
static NJS_CNK_MODEL draw_215_model = {
#include "stg10/terrain/draw_215_model.inc"
};
static NJS_CNK_OBJECT draw_215 = {
#include "stg10/terrain/draw_215.inc"
};
static Sint16 draw_205_polygons[171] = {
#include "stg10/terrain/draw_205_polygons.inc"
};
static Sint32 draw_205_vertices[179] = {
#include "stg10/terrain/draw_205_vertices.inc"
};
static NJS_CNK_MODEL draw_205_model = {
#include "stg10/terrain/draw_205_model.inc"
};
static NJS_CNK_OBJECT draw_214 = {
#include "stg10/terrain/draw_214.inc"
};
static Sint16 draw_204_polygons[235] = {
#include "stg10/terrain/draw_204_polygons.inc"
};
static Sint32 draw_204_vertices[211] = {
#include "stg10/terrain/draw_204_vertices.inc"
};
static NJS_CNK_MODEL draw_204_model = {
#include "stg10/terrain/draw_204_model.inc"
};
static NJS_CNK_OBJECT draw_204 = {
#include "stg10/terrain/draw_204.inc"
};
static Sint16 draw_203_polygons[277] = {
#include "stg10/terrain/draw_203_polygons.inc"
};
static Sint32 draw_203_vertices[315] = {
#include "stg10/terrain/draw_203_vertices.inc"
};
static NJS_CNK_MODEL draw_203_model = {
#include "stg10/terrain/draw_203_model.inc"
};
static NJS_CNK_OBJECT draw_203 = {
#include "stg10/terrain/draw_203.inc"
};
static Sint16 draw_202_polygons[145] = {
#include "stg10/terrain/draw_202_polygons.inc"
};
static Sint32 draw_202_vertices[87] = {
#include "stg10/terrain/draw_202_vertices.inc"
};
static NJS_CNK_MODEL draw_202_model = {
#include "stg10/terrain/draw_202_model.inc"
};
static NJS_CNK_OBJECT draw_202 = {
#include "stg10/terrain/draw_202.inc"
};
static Sint16 draw_201_polygons[1319] = {
#include "stg10/terrain/draw_201_polygons.inc"
};
static Sint32 draw_201_vertices[1159] = {
#include "stg10/terrain/draw_201_vertices.inc"
};
static NJS_CNK_MODEL draw_201_model = {
#include "stg10/terrain/draw_201_model.inc"
};
static NJS_CNK_OBJECT draw_201 = {
#include "stg10/terrain/draw_201.inc"
};
static Sint16 draw_199_polygons[23] = {
#include "stg10/terrain/draw_199_polygons.inc"
};
static Sint32 draw_199_vertices[19] = {
#include "stg10/terrain/draw_199_vertices.inc"
};
static NJS_CNK_MODEL draw_199_model = {
#include "stg10/terrain/draw_199_model.inc"
};
static NJS_CNK_OBJECT draw_200 = {
#include "stg10/terrain/draw_200.inc"
};
static Sint16 draw_198_polygons[69] = {
#include "stg10/terrain/draw_198_polygons.inc"
};
static Sint32 draw_198_vertices[55] = {
#include "stg10/terrain/draw_198_vertices.inc"
};
static NJS_CNK_MODEL draw_198_model = {
#include "stg10/terrain/draw_198_model.inc"
};
static NJS_CNK_OBJECT draw_198 = {
#include "stg10/terrain/draw_198.inc"
};
static Sint16 draw_195_polygons[139] = {
#include "stg10/terrain/draw_195_polygons.inc"
};
static Sint32 draw_195_vertices[131] = {
#include "stg10/terrain/draw_195_vertices.inc"
};
static NJS_CNK_MODEL draw_195_model = {
#include "stg10/terrain/draw_195_model.inc"
};
static NJS_CNK_OBJECT draw_197 = {
#include "stg10/terrain/draw_197.inc"
};
static Sint16 draw_193_polygons[163] = {
#include "stg10/terrain/draw_193_polygons.inc"
};
static Sint32 draw_193_vertices[143] = {
#include "stg10/terrain/draw_193_vertices.inc"
};
static NJS_CNK_MODEL draw_193_model = {
#include "stg10/terrain/draw_193_model.inc"
};
static NJS_CNK_OBJECT draw_194 = {
#include "stg10/terrain/draw_194.inc"
};
static Sint16 draw_179_polygons[23] = {
#include "stg10/terrain/draw_179_polygons.inc"
};
static Sint32 draw_179_vertices[19] = {
#include "stg10/terrain/draw_179_vertices.inc"
};
static NJS_CNK_MODEL draw_179_model = {
#include "stg10/terrain/draw_179_model.inc"
};
static NJS_CNK_OBJECT draw_192 = {
#include "stg10/terrain/draw_192.inc"
};
static Sint16 draw_178_polygons[23] = {
#include "stg10/terrain/draw_178_polygons.inc"
};
static Sint32 draw_178_vertices[19] = {
#include "stg10/terrain/draw_178_vertices.inc"
};
static NJS_CNK_MODEL draw_178_model = {
#include "stg10/terrain/draw_178_model.inc"
};
static NJS_CNK_OBJECT draw_178 = {
#include "stg10/terrain/draw_178.inc"
};
static Sint16 draw_177_polygons[23] = {
#include "stg10/terrain/draw_177_polygons.inc"
};
static Sint32 draw_177_vertices[19] = {
#include "stg10/terrain/draw_177_vertices.inc"
};
static NJS_CNK_MODEL draw_177_model = {
#include "stg10/terrain/draw_177_model.inc"
};
static NJS_CNK_OBJECT draw_177 = {
#include "stg10/terrain/draw_177.inc"
};
static Sint16 draw_174_polygons[23] = {
#include "stg10/terrain/draw_174_polygons.inc"
};
static Sint32 draw_174_vertices[19] = {
#include "stg10/terrain/draw_174_vertices.inc"
};
static NJS_CNK_MODEL draw_174_model = {
#include "stg10/terrain/draw_174_model.inc"
};
static NJS_CNK_OBJECT draw_176 = {
#include "stg10/terrain/draw_176.inc"
};
static Sint16 draw_173_polygons[23] = {
#include "stg10/terrain/draw_173_polygons.inc"
};
static Sint32 draw_173_vertices[19] = {
#include "stg10/terrain/draw_173_vertices.inc"
};
static NJS_CNK_MODEL draw_173_model = {
#include "stg10/terrain/draw_173_model.inc"
};
static NJS_CNK_OBJECT draw_173 = {
#include "stg10/terrain/draw_173.inc"
};
static Sint16 draw_172_polygons[23] = {
#include "stg10/terrain/draw_172_polygons.inc"
};
static Sint32 draw_172_vertices[19] = {
#include "stg10/terrain/draw_172_vertices.inc"
};
static NJS_CNK_MODEL draw_172_model = {
#include "stg10/terrain/draw_172_model.inc"
};
static NJS_CNK_OBJECT draw_172 = {
#include "stg10/terrain/draw_172.inc"
};
static Sint16 draw_166_polygons[23] = {
#include "stg10/terrain/draw_166_polygons.inc"
};
static Sint32 draw_166_vertices[19] = {
#include "stg10/terrain/draw_166_vertices.inc"
};
static NJS_CNK_MODEL draw_166_model = {
#include "stg10/terrain/draw_166_model.inc"
};
static NJS_CNK_OBJECT draw_171 = {
#include "stg10/terrain/draw_171.inc"
};
static Sint16 draw_163_polygons[71] = {
#include "stg10/terrain/draw_163_polygons.inc"
};
static Sint32 draw_163_vertices[35] = {
#include "stg10/terrain/draw_163_vertices.inc"
};
static NJS_CNK_MODEL draw_163_model = {
#include "stg10/terrain/draw_163_model.inc"
};
static NJS_CNK_OBJECT draw_165 = {
#include "stg10/terrain/draw_165.inc"
};
static Sint16 draw_161_polygons[79] = {
#include "stg10/terrain/draw_161_polygons.inc"
};
static Sint32 draw_161_vertices[83] = {
#include "stg10/terrain/draw_161_vertices.inc"
};
static NJS_CNK_MODEL draw_161_model = {
#include "stg10/terrain/draw_161_model.inc"
};
static NJS_CNK_OBJECT draw_162 = {
#include "stg10/terrain/draw_162.inc"
};
static Sint16 draw_160_polygons[47] = {
#include "stg10/terrain/draw_160_polygons.inc"
};
static Sint32 draw_160_vertices[51] = {
#include "stg10/terrain/draw_160_vertices.inc"
};
static NJS_CNK_MODEL draw_160_model = {
#include "stg10/terrain/draw_160_model.inc"
};
static NJS_CNK_OBJECT draw_160 = {
#include "stg10/terrain/draw_160.inc"
};
static Sint16 draw_158_polygons[833] = {
#include "stg10/terrain/draw_158_polygons.inc"
};
static Sint32 draw_158_vertices[843] = {
#include "stg10/terrain/draw_158_vertices.inc"
};
static NJS_CNK_MODEL draw_158_model = {
#include "stg10/terrain/draw_158_model.inc"
};
static NJS_CNK_OBJECT draw_159 = {
#include "stg10/terrain/draw_159.inc"
};
static Sint16 draw_156_polygons[23] = {
#include "stg10/terrain/draw_156_polygons.inc"
};
static Sint32 draw_156_vertices[19] = {
#include "stg10/terrain/draw_156_vertices.inc"
};
static NJS_CNK_MODEL draw_156_model = {
#include "stg10/terrain/draw_156_model.inc"
};
static NJS_CNK_OBJECT draw_157 = {
#include "stg10/terrain/draw_157.inc"
};
static Sint16 draw_154_polygons[23] = {
#include "stg10/terrain/draw_154_polygons.inc"
};
static Sint32 draw_154_vertices[19] = {
#include "stg10/terrain/draw_154_vertices.inc"
};
static NJS_CNK_MODEL draw_154_model = {
#include "stg10/terrain/draw_154_model.inc"
};
static NJS_CNK_OBJECT draw_155 = {
#include "stg10/terrain/draw_155.inc"
};
static Sint16 draw_153_polygons[23] = {
#include "stg10/terrain/draw_153_polygons.inc"
};
static Sint32 draw_153_vertices[19] = {
#include "stg10/terrain/draw_153_vertices.inc"
};
static NJS_CNK_MODEL draw_153_model = {
#include "stg10/terrain/draw_153_model.inc"
};
static NJS_CNK_OBJECT draw_153 = {
#include "stg10/terrain/draw_153.inc"
};
static Sint16 draw_152_polygons[23] = {
#include "stg10/terrain/draw_152_polygons.inc"
};
static Sint32 draw_152_vertices[19] = {
#include "stg10/terrain/draw_152_vertices.inc"
};
static NJS_CNK_MODEL draw_152_model = {
#include "stg10/terrain/draw_152_model.inc"
};
static NJS_CNK_OBJECT draw_152 = {
#include "stg10/terrain/draw_152.inc"
};
static Sint16 draw_148_polygons[23] = {
#include "stg10/terrain/draw_148_polygons.inc"
};
static Sint32 draw_148_vertices[19] = {
#include "stg10/terrain/draw_148_vertices.inc"
};
static NJS_CNK_MODEL draw_148_model = {
#include "stg10/terrain/draw_148_model.inc"
};
static NJS_CNK_OBJECT draw_151 = {
#include "stg10/terrain/draw_151.inc"
};
static Sint16 draw_147_polygons[37] = {
#include "stg10/terrain/draw_147_polygons.inc"
};
static Sint32 draw_147_vertices[27] = {
#include "stg10/terrain/draw_147_vertices.inc"
};
static NJS_CNK_MODEL draw_147_model = {
#include "stg10/terrain/draw_147_model.inc"
};
static NJS_CNK_OBJECT draw_147 = {
#include "stg10/terrain/draw_147.inc"
};
static Sint16 draw_146_polygons[23] = {
#include "stg10/terrain/draw_146_polygons.inc"
};
static Sint32 draw_146_vertices[19] = {
#include "stg10/terrain/draw_146_vertices.inc"
};
static NJS_CNK_MODEL draw_146_model = {
#include "stg10/terrain/draw_146_model.inc"
};
static NJS_CNK_OBJECT draw_146 = {
#include "stg10/terrain/draw_146.inc"
};
static Sint16 draw_145_polygons[35] = {
#include "stg10/terrain/draw_145_polygons.inc"
};
static Sint32 draw_145_vertices[35] = {
#include "stg10/terrain/draw_145_vertices.inc"
};
static NJS_CNK_MODEL draw_145_model = {
#include "stg10/terrain/draw_145_model.inc"
};
static NJS_CNK_OBJECT draw_145 = {
#include "stg10/terrain/draw_145.inc"
};
static Sint16 draw_144_polygons[55] = {
#include "stg10/terrain/draw_144_polygons.inc"
};
static Sint32 draw_144_vertices[51] = {
#include "stg10/terrain/draw_144_vertices.inc"
};
static NJS_CNK_MODEL draw_144_model = {
#include "stg10/terrain/draw_144_model.inc"
};
static NJS_CNK_OBJECT draw_144 = {
#include "stg10/terrain/draw_144.inc"
};
static Sint16 draw_143_polygons[59] = {
#include "stg10/terrain/draw_143_polygons.inc"
};
static Sint32 draw_143_vertices[67] = {
#include "stg10/terrain/draw_143_vertices.inc"
};
static NJS_CNK_MODEL draw_143_model = {
#include "stg10/terrain/draw_143_model.inc"
};
static NJS_CNK_OBJECT draw_143 = {
#include "stg10/terrain/draw_143.inc"
};
static Sint16 draw_140_polygons[853] = {
#include "stg10/terrain/draw_140_polygons.inc"
};
static Sint32 draw_140_vertices[863] = {
#include "stg10/terrain/draw_140_vertices.inc"
};
static NJS_CNK_MODEL draw_140_model = {
#include "stg10/terrain/draw_140_model.inc"
};
static NJS_CNK_OBJECT draw_142 = {
#include "stg10/terrain/draw_142.inc"
};
static Sint16 draw_139_polygons[47] = {
#include "stg10/terrain/draw_139_polygons.inc"
};
static Sint32 draw_139_vertices[51] = {
#include "stg10/terrain/draw_139_vertices.inc"
};
static NJS_CNK_MODEL draw_139_model = {
#include "stg10/terrain/draw_139_model.inc"
};
static NJS_CNK_OBJECT draw_139 = {
#include "stg10/terrain/draw_139.inc"
};
static Sint16 draw_137_polygons[65] = {
#include "stg10/terrain/draw_137_polygons.inc"
};
static Sint32 draw_137_vertices[59] = {
#include "stg10/terrain/draw_137_vertices.inc"
};
static NJS_CNK_MODEL draw_137_model = {
#include "stg10/terrain/draw_137_model.inc"
};
static NJS_CNK_OBJECT draw_138 = {
#include "stg10/terrain/draw_138.inc"
};
static Sint16 draw_136_polygons[79] = {
#include "stg10/terrain/draw_136_polygons.inc"
};
static Sint32 draw_136_vertices[67] = {
#include "stg10/terrain/draw_136_vertices.inc"
};
static NJS_CNK_MODEL draw_136_model = {
#include "stg10/terrain/draw_136_model.inc"
};
static NJS_CNK_OBJECT draw_136 = {
#include "stg10/terrain/draw_136.inc"
};
static Sint16 draw_135_polygons[47] = {
#include "stg10/terrain/draw_135_polygons.inc"
};
static Sint32 draw_135_vertices[51] = {
#include "stg10/terrain/draw_135_vertices.inc"
};
static NJS_CNK_MODEL draw_135_model = {
#include "stg10/terrain/draw_135_model.inc"
};
static NJS_CNK_OBJECT draw_135 = {
#include "stg10/terrain/draw_135.inc"
};
static Sint16 draw_133_polygons[23] = {
#include "stg10/terrain/draw_133_polygons.inc"
};
static Sint32 draw_133_vertices[19] = {
#include "stg10/terrain/draw_133_vertices.inc"
};
static NJS_CNK_MODEL draw_133_model = {
#include "stg10/terrain/draw_133_model.inc"
};
static NJS_CNK_OBJECT draw_134 = {
#include "stg10/terrain/draw_134.inc"
};
static Sint16 draw_130_polygons[23] = {
#include "stg10/terrain/draw_130_polygons.inc"
};
static Sint32 draw_130_vertices[19] = {
#include "stg10/terrain/draw_130_vertices.inc"
};
static NJS_CNK_MODEL draw_130_model = {
#include "stg10/terrain/draw_130_model.inc"
};
static NJS_CNK_OBJECT draw_132 = {
#include "stg10/terrain/draw_132.inc"
};
static Sint16 draw_124_polygons[1137] = {
#include "stg10/terrain/draw_124_polygons.inc"
};
static Sint32 draw_124_vertices[1103] = {
#include "stg10/terrain/draw_124_vertices.inc"
};
static NJS_CNK_MODEL draw_124_model = {
#include "stg10/terrain/draw_124_model.inc"
};
static NJS_CNK_OBJECT draw_129 = {
#include "stg10/terrain/draw_129.inc"
};
static Sint16 draw_123_polygons[23] = {
#include "stg10/terrain/draw_123_polygons.inc"
};
static Sint32 draw_123_vertices[19] = {
#include "stg10/terrain/draw_123_vertices.inc"
};
static NJS_CNK_MODEL draw_123_model = {
#include "stg10/terrain/draw_123_model.inc"
};
static NJS_CNK_OBJECT draw_123 = {
#include "stg10/terrain/draw_123.inc"
};
static Sint16 draw_121_polygons[49] = {
#include "stg10/terrain/draw_121_polygons.inc"
};
static Sint32 draw_121_vertices[35] = {
#include "stg10/terrain/draw_121_vertices.inc"
};
static NJS_CNK_MODEL draw_121_model = {
#include "stg10/terrain/draw_121_model.inc"
};
static NJS_CNK_OBJECT draw_122 = {
#include "stg10/terrain/draw_122.inc"
};
static Sint16 draw_120_polygons[83] = {
#include "stg10/terrain/draw_120_polygons.inc"
};
static Sint32 draw_120_vertices[47] = {
#include "stg10/terrain/draw_120_vertices.inc"
};
static NJS_CNK_MODEL draw_120_model = {
#include "stg10/terrain/draw_120_model.inc"
};
static NJS_CNK_OBJECT draw_120 = {
#include "stg10/terrain/draw_120.inc"
};
static Sint16 draw_119_polygons[55] = {
#include "stg10/terrain/draw_119_polygons.inc"
};
static Sint32 draw_119_vertices[51] = {
#include "stg10/terrain/draw_119_vertices.inc"
};
static NJS_CNK_MODEL draw_119_model = {
#include "stg10/terrain/draw_119_model.inc"
};
static NJS_CNK_OBJECT draw_119 = {
#include "stg10/terrain/draw_119.inc"
};
static Sint16 draw_118_polygons[1285] = {
#include "stg10/terrain/draw_118_polygons.inc"
};
static Sint32 draw_118_vertices[1291] = {
#include "stg10/terrain/draw_118_vertices.inc"
};
static NJS_CNK_MODEL draw_118_model = {
#include "stg10/terrain/draw_118_model.inc"
};
static NJS_CNK_OBJECT draw_118 = {
#include "stg10/terrain/draw_118.inc"
};
static Sint16 draw_117_polygons[261] = {
#include "stg10/terrain/draw_117_polygons.inc"
};
static Sint32 draw_117_vertices[255] = {
#include "stg10/terrain/draw_117_vertices.inc"
};
static NJS_CNK_MODEL draw_117_model = {
#include "stg10/terrain/draw_117_model.inc"
};
static NJS_CNK_OBJECT draw_117 = {
#include "stg10/terrain/draw_117.inc"
};
static Sint16 draw_115_polygons[267] = {
#include "stg10/terrain/draw_115_polygons.inc"
};
static Sint32 draw_115_vertices[259] = {
#include "stg10/terrain/draw_115_vertices.inc"
};
static NJS_CNK_MODEL draw_115_model = {
#include "stg10/terrain/draw_115_model.inc"
};
static NJS_CNK_OBJECT draw_116 = {
#include "stg10/terrain/draw_116.inc"
};
static Sint16 draw_111_polygons[87] = {
#include "stg10/terrain/draw_111_polygons.inc"
};
static Sint32 draw_111_vertices[67] = {
#include "stg10/terrain/draw_111_vertices.inc"
};
static NJS_CNK_MODEL draw_111_model = {
#include "stg10/terrain/draw_111_model.inc"
};
static NJS_CNK_OBJECT draw_114 = {
#include "stg10/terrain/draw_114.inc"
};
static Sint16 draw_110_polygons[185] = {
#include "stg10/terrain/draw_110_polygons.inc"
};
static Sint32 draw_110_vertices[91] = {
#include "stg10/terrain/draw_110_vertices.inc"
};
static NJS_CNK_MODEL draw_110_model = {
#include "stg10/terrain/draw_110_model.inc"
};
static NJS_CNK_OBJECT draw_110 = {
#include "stg10/terrain/draw_110.inc"
};
static Sint16 draw_109_polygons[83] = {
#include "stg10/terrain/draw_109_polygons.inc"
};
static Sint32 draw_109_vertices[63] = {
#include "stg10/terrain/draw_109_vertices.inc"
};
static NJS_CNK_MODEL draw_109_model = {
#include "stg10/terrain/draw_109_model.inc"
};
static NJS_CNK_OBJECT draw_109 = {
#include "stg10/terrain/draw_109.inc"
};
static Sint16 draw_107_polygons[1659] = {
#include "stg10/terrain/draw_107_polygons.inc"
};
static Sint32 draw_107_vertices[1707] = {
#include "stg10/terrain/draw_107_vertices.inc"
};
static NJS_CNK_MODEL draw_107_model = {
#include "stg10/terrain/draw_107_model.inc"
};
static NJS_CNK_OBJECT draw_108 = {
#include "stg10/terrain/draw_108.inc"
};
static Sint16 draw_105_polygons[235] = {
#include "stg10/terrain/draw_105_polygons.inc"
};
static Sint32 draw_105_vertices[155] = {
#include "stg10/terrain/draw_105_vertices.inc"
};
static NJS_CNK_MODEL draw_105_model = {
#include "stg10/terrain/draw_105_model.inc"
};
static NJS_CNK_OBJECT draw_106 = {
#include "stg10/terrain/draw_106.inc"
};
static Sint16 draw_104_polygons[269] = {
#include "stg10/terrain/draw_104_polygons.inc"
};
static Sint32 draw_104_vertices[187] = {
#include "stg10/terrain/draw_104_vertices.inc"
};
static NJS_CNK_MODEL draw_104_model = {
#include "stg10/terrain/draw_104_model.inc"
};
static NJS_CNK_OBJECT draw_104 = {
#include "stg10/terrain/draw_104.inc"
};
static Sint16 draw_100_polygons[157] = {
#include "stg10/terrain/draw_100_polygons.inc"
};
static Sint32 draw_100_vertices[71] = {
#include "stg10/terrain/draw_100_vertices.inc"
};
static NJS_CNK_MODEL draw_100_model = {
#include "stg10/terrain/draw_100_model.inc"
};
static NJS_CNK_OBJECT draw_103 = {
#include "stg10/terrain/draw_103.inc"
};
static Sint16 draw_99_polygons[853] = {
#include "stg10/terrain/draw_99_polygons.inc"
};
static Sint32 draw_99_vertices[779] = {
#include "stg10/terrain/draw_99_vertices.inc"
};
static NJS_CNK_MODEL draw_99_model = {
#include "stg10/terrain/draw_99_model.inc"
};
static NJS_CNK_OBJECT draw_99 = {
#include "stg10/terrain/draw_99.inc"
};
static Sint16 draw_98_polygons[1033] = {
#include "stg10/terrain/draw_98_polygons.inc"
};
static Sint32 draw_98_vertices[699] = {
#include "stg10/terrain/draw_98_vertices.inc"
};
static NJS_CNK_MODEL draw_98_model = {
#include "stg10/terrain/draw_98_model.inc"
};
static NJS_CNK_OBJECT draw_98 = {
#include "stg10/terrain/draw_98.inc"
};
static Sint16 draw_96_polygons[243] = {
#include "stg10/terrain/draw_96_polygons.inc"
};
static Sint32 draw_96_vertices[119] = {
#include "stg10/terrain/draw_96_vertices.inc"
};
static NJS_CNK_MODEL draw_96_model = {
#include "stg10/terrain/draw_96_model.inc"
};
static NJS_CNK_OBJECT draw_97 = {
#include "stg10/terrain/draw_97.inc"
};
static Sint16 draw_95_polygons[281] = {
#include "stg10/terrain/draw_95_polygons.inc"
};
static Sint32 draw_95_vertices[207] = {
#include "stg10/terrain/draw_95_vertices.inc"
};
static NJS_CNK_MODEL draw_95_model = {
#include "stg10/terrain/draw_95_model.inc"
};
static NJS_CNK_OBJECT draw_95 = {
#include "stg10/terrain/draw_95.inc"
};
static Sint16 draw_91_polygons[1229] = {
#include "stg10/terrain/draw_91_polygons.inc"
};
static Sint32 draw_91_vertices[1059] = {
#include "stg10/terrain/draw_91_vertices.inc"
};
static NJS_CNK_MODEL draw_91_model = {
#include "stg10/terrain/draw_91_model.inc"
};
static NJS_CNK_OBJECT draw_94 = {
#include "stg10/terrain/draw_94.inc"
};
static Sint16 draw_90_polygons[61] = {
#include "stg10/terrain/draw_90_polygons.inc"
};
static Sint32 draw_90_vertices[35] = {
#include "stg10/terrain/draw_90_vertices.inc"
};
static NJS_CNK_MODEL draw_90_model = {
#include "stg10/terrain/draw_90_model.inc"
};
static NJS_CNK_OBJECT draw_90 = {
#include "stg10/terrain/draw_90.inc"
};
static Sint16 draw_89_polygons[585] = {
#include "stg10/terrain/draw_89_polygons.inc"
};
static Sint32 draw_89_vertices[355] = {
#include "stg10/terrain/draw_89_vertices.inc"
};
static NJS_CNK_MODEL draw_89_model = {
#include "stg10/terrain/draw_89_model.inc"
};
static NJS_CNK_OBJECT draw_89 = {
#include "stg10/terrain/draw_89.inc"
};
static Sint16 draw_86_polygons[1323] = {
#include "stg10/terrain/draw_86_polygons.inc"
};
static Sint32 draw_86_vertices[1459] = {
#include "stg10/terrain/draw_86_vertices.inc"
};
static NJS_CNK_MODEL draw_86_model = {
#include "stg10/terrain/draw_86_model.inc"
};
static NJS_CNK_OBJECT draw_88 = {
#include "stg10/terrain/draw_88.inc"
};
static Sint16 draw_82_polygons[35] = {
#include "stg10/terrain/draw_82_polygons.inc"
};
static Sint32 draw_82_vertices[35] = {
#include "stg10/terrain/draw_82_vertices.inc"
};
static NJS_CNK_MODEL draw_82_model = {
#include "stg10/terrain/draw_82_model.inc"
};
static NJS_CNK_OBJECT draw_85 = {
#include "stg10/terrain/draw_85.inc"
};
static Sint16 draw_81_polygons[485] = {
#include "stg10/terrain/draw_81_polygons.inc"
};
static Sint32 draw_81_vertices[435] = {
#include "stg10/terrain/draw_81_vertices.inc"
};
static NJS_CNK_MODEL draw_81_model = {
#include "stg10/terrain/draw_81_model.inc"
};
static NJS_CNK_OBJECT draw_81 = {
#include "stg10/terrain/draw_81.inc"
};
static Sint16 draw_80_polygons[23] = {
#include "stg10/terrain/draw_80_polygons.inc"
};
static Sint32 draw_80_vertices[19] = {
#include "stg10/terrain/draw_80_vertices.inc"
};
static NJS_CNK_MODEL draw_80_model = {
#include "stg10/terrain/draw_80_model.inc"
};
static NJS_CNK_OBJECT draw_80 = {
#include "stg10/terrain/draw_80.inc"
};
static Sint16 draw_79_polygons[463] = {
#include "stg10/terrain/draw_79_polygons.inc"
};
static Sint32 draw_79_vertices[267] = {
#include "stg10/terrain/draw_79_vertices.inc"
};
static NJS_CNK_MODEL draw_79_model = {
#include "stg10/terrain/draw_79_model.inc"
};
static NJS_CNK_OBJECT draw_79 = {
#include "stg10/terrain/draw_79.inc"
};
static Sint16 draw_74_polygons[23] = {
#include "stg10/terrain/draw_74_polygons.inc"
};
static Sint32 draw_74_vertices[19] = {
#include "stg10/terrain/draw_74_vertices.inc"
};
static NJS_CNK_MODEL draw_74_model = {
#include "stg10/terrain/draw_74_model.inc"
};
static NJS_CNK_OBJECT draw_78 = {
#include "stg10/terrain/draw_78.inc"
};
static Sint16 draw_62_polygons[247] = {
#include "stg10/terrain/draw_62_polygons.inc"
};
static Sint32 draw_62_vertices[95] = {
#include "stg10/terrain/draw_62_vertices.inc"
};
static NJS_CNK_MODEL draw_62_model = {
#include "stg10/terrain/draw_62_model.inc"
};
static NJS_CNK_OBJECT draw_73 = {
#include "stg10/terrain/draw_73.inc"
};
static Sint16 draw_56_polygons[429] = {
#include "stg10/terrain/draw_56_polygons.inc"
};
static Sint32 draw_56_vertices[475] = {
#include "stg10/terrain/draw_56_vertices.inc"
};
static NJS_CNK_MODEL draw_56_model = {
#include "stg10/terrain/draw_56_model.inc"
};
static NJS_CNK_OBJECT draw_61 = {
#include "stg10/terrain/draw_61.inc"
};
static Sint16 draw_52_polygons[209] = {
#include "stg10/terrain/draw_52_polygons.inc"
};
static Sint32 draw_52_vertices[155] = {
#include "stg10/terrain/draw_52_vertices.inc"
};
static NJS_CNK_MODEL draw_52_model = {
#include "stg10/terrain/draw_52_model.inc"
};
static NJS_CNK_OBJECT draw_55 = {
#include "stg10/terrain/draw_55.inc"
};
static Sint16 draw_50_polygons[247] = {
#include "stg10/terrain/draw_50_polygons.inc"
};
static Sint32 draw_50_vertices[139] = {
#include "stg10/terrain/draw_50_vertices.inc"
};
static NJS_CNK_MODEL draw_50_model = {
#include "stg10/terrain/draw_50_model.inc"
};
static NJS_CNK_OBJECT draw_51 = {
#include "stg10/terrain/draw_51.inc"
};
static Sint16 draw_48_polygons[257] = {
#include "stg10/terrain/draw_48_polygons.inc"
};
static Sint32 draw_48_vertices[211] = {
#include "stg10/terrain/draw_48_vertices.inc"
};
static NJS_CNK_MODEL draw_48_model = {
#include "stg10/terrain/draw_48_model.inc"
};
static NJS_CNK_OBJECT draw_49 = {
#include "stg10/terrain/draw_49.inc"
};
static Sint16 draw_45_polygons[159] = {
#include "stg10/terrain/draw_45_polygons.inc"
};
static Sint32 draw_45_vertices[147] = {
#include "stg10/terrain/draw_45_vertices.inc"
};
static NJS_CNK_MODEL draw_45_model = {
#include "stg10/terrain/draw_45_model.inc"
};
static NJS_CNK_OBJECT draw_47 = {
#include "stg10/terrain/draw_47.inc"
};
static Sint16 draw_33_polygons[1737] = {
#include "stg10/terrain/draw_33_polygons.inc"
};
static Sint32 draw_33_vertices[1843] = {
#include "stg10/terrain/draw_33_vertices.inc"
};
static NJS_CNK_MODEL draw_33_model = {
#include "stg10/terrain/draw_33_model.inc"
};
static NJS_CNK_OBJECT draw_44 = {
#include "stg10/terrain/draw_44.inc"
};
static Sint16 draw_32_polygons[89] = {
#include "stg10/terrain/draw_32_polygons.inc"
};
static Sint32 draw_32_vertices[75] = {
#include "stg10/terrain/draw_32_vertices.inc"
};
static NJS_CNK_MODEL draw_32_model = {
#include "stg10/terrain/draw_32_model.inc"
};
static NJS_CNK_OBJECT draw_32 = {
#include "stg10/terrain/draw_32.inc"
};
static Sint16 draw_31_polygons[89] = {
#include "stg10/terrain/draw_31_polygons.inc"
};
static Sint32 draw_31_vertices[75] = {
#include "stg10/terrain/draw_31_vertices.inc"
};
static NJS_CNK_MODEL draw_31_model = {
#include "stg10/terrain/draw_31_model.inc"
};
static NJS_CNK_OBJECT draw_31 = {
#include "stg10/terrain/draw_31.inc"
};
static Sint16 draw_30_polygons[403] = {
#include "stg10/terrain/draw_30_polygons.inc"
};
static Sint32 draw_30_vertices[227] = {
#include "stg10/terrain/draw_30_vertices.inc"
};
static NJS_CNK_MODEL draw_30_model = {
#include "stg10/terrain/draw_30_model.inc"
};
static NJS_CNK_OBJECT draw_30 = {
#include "stg10/terrain/draw_30.inc"
};
static Sint16 draw_29_polygons[63] = {
#include "stg10/terrain/draw_29_polygons.inc"
};
static Sint32 draw_29_vertices[67] = {
#include "stg10/terrain/draw_29_vertices.inc"
};
static NJS_CNK_MODEL draw_29_model = {
#include "stg10/terrain/draw_29_model.inc"
};
static NJS_CNK_OBJECT draw_29 = {
#include "stg10/terrain/draw_29.inc"
};
static Sint16 draw_27_polygons[181] = {
#include "stg10/terrain/draw_27_polygons.inc"
};
static Sint32 draw_27_vertices[131] = {
#include "stg10/terrain/draw_27_vertices.inc"
};
static NJS_CNK_MODEL draw_27_model = {
#include "stg10/terrain/draw_27_model.inc"
};
static NJS_CNK_OBJECT draw_28 = {
#include "stg10/terrain/draw_28.inc"
};
static Sint16 draw_2_polygons[259] = {
#include "stg10/terrain/draw_2_polygons.inc"
};
static Sint32 draw_2_vertices[263] = {
#include "stg10/terrain/draw_2_vertices.inc"
};
static NJS_CNK_MODEL draw_2_model = {
#include "stg10/terrain/draw_2_model.inc"
};
static NJS_CNK_OBJECT draw_26 = {
#include "stg10/terrain/draw_26.inc"
};
static Sint16 draw_0_polygons[55] = {
#include "stg10/terrain/draw_0_polygons.inc"
};
static Sint32 draw_0_vertices[31] = {
#include "stg10/terrain/draw_0_vertices.inc"
};
static NJS_CNK_MODEL draw_0_model = {
#include "stg10/terrain/draw_0_model.inc"
};
static NJS_CNK_OBJECT draw_1 = {
#include "stg10/terrain/draw_1.inc"
};
static NJS_MATERIAL collision_0_materials[1] = {
#include "stg10/terrain/collision_0_materials.inc"
};
static Sint16 collision_0_faces[10] = {
#include "stg10/terrain/collision_0_faces.inc"
};
static NJS_MESHSET collision_0_meshsets[1] = {
#include "stg10/terrain/collision_0_meshsets.inc"
};
static NJS_POINT3 collision_0_points[8] = {
#include "stg10/terrain/collision_0_points.inc"
};
static NJS_MODEL collision_0_model = {
#include "stg10/terrain/collision_0_model.inc"
};
static NJS_OBJECT collision_0 = {
#include "stg10/terrain/collision_0.inc"
};
static NJS_MATERIAL collision_1_materials[1] = {
#include "stg10/terrain/collision_1_materials.inc"
};
static Sint16 collision_1_faces[19] = {
#include "stg10/terrain/collision_1_faces.inc"
};
static NJS_MESHSET collision_1_meshsets[1] = {
#include "stg10/terrain/collision_1_meshsets.inc"
};
static NJS_POINT3 collision_1_points[16] = {
#include "stg10/terrain/collision_1_points.inc"
};
static NJS_MODEL collision_1_model = {
#include "stg10/terrain/collision_1_model.inc"
};
static NJS_OBJECT collision_1 = {
#include "stg10/terrain/collision_1.inc"
};
static NJS_MATERIAL collision_2_materials[1] = {
#include "stg10/terrain/collision_2_materials.inc"
};
static Sint16 collision_2_faces[19] = {
#include "stg10/terrain/collision_2_faces.inc"
};
static NJS_MESHSET collision_2_meshsets[1] = {
#include "stg10/terrain/collision_2_meshsets.inc"
};
static NJS_POINT3 collision_2_points[16] = {
#include "stg10/terrain/collision_2_points.inc"
};
static NJS_MODEL collision_2_model = {
#include "stg10/terrain/collision_2_model.inc"
};
static NJS_OBJECT collision_2 = {
#include "stg10/terrain/collision_2.inc"
};
static NJS_MATERIAL collision_3_materials[1] = {
#include "stg10/terrain/collision_3_materials.inc"
};
static Sint16 collision_3_faces[10] = {
#include "stg10/terrain/collision_3_faces.inc"
};
static NJS_MESHSET collision_3_meshsets[1] = {
#include "stg10/terrain/collision_3_meshsets.inc"
};
static NJS_POINT3 collision_3_points[8] = {
#include "stg10/terrain/collision_3_points.inc"
};
static NJS_MODEL collision_3_model = {
#include "stg10/terrain/collision_3_model.inc"
};
static NJS_OBJECT collision_3 = {
#include "stg10/terrain/collision_3.inc"
};
static NJS_MATERIAL collision_36_materials[1] = {
#include "stg10/terrain/collision_36_materials.inc"
};
static Sint16 collision_36_faces[10] = {
#include "stg10/terrain/collision_36_faces.inc"
};
static NJS_MESHSET collision_36_meshsets[1] = {
#include "stg10/terrain/collision_36_meshsets.inc"
};
static NJS_POINT3 collision_36_points[8] = {
#include "stg10/terrain/collision_36_points.inc"
};
static NJS_MODEL collision_36_model = {
#include "stg10/terrain/collision_36_model.inc"
};
static NJS_OBJECT collision_36 = {
#include "stg10/terrain/collision_36.inc"
};
static NJS_MATERIAL collision_42_materials[1] = {
#include "stg10/terrain/collision_42_materials.inc"
};
static Sint16 collision_42_faces[19] = {
#include "stg10/terrain/collision_42_faces.inc"
};
static NJS_MESHSET collision_42_meshsets[1] = {
#include "stg10/terrain/collision_42_meshsets.inc"
};
static NJS_POINT3 collision_42_points[16] = {
#include "stg10/terrain/collision_42_points.inc"
};
static NJS_MODEL collision_42_model = {
#include "stg10/terrain/collision_42_model.inc"
};
static NJS_OBJECT collision_42 = {
#include "stg10/terrain/collision_42.inc"
};
static NJS_MATERIAL collision_43_materials[1] = {
#include "stg10/terrain/collision_43_materials.inc"
};
static Sint16 collision_43_faces[90] = {
#include "stg10/terrain/collision_43_faces.inc"
};
static NJS_MESHSET collision_43_meshsets[1] = {
#include "stg10/terrain/collision_43_meshsets.inc"
};
static NJS_POINT3 collision_43_points[60] = {
#include "stg10/terrain/collision_43_points.inc"
};
static NJS_MODEL collision_43_model = {
#include "stg10/terrain/collision_43_model.inc"
};
static NJS_OBJECT collision_43 = {
#include "stg10/terrain/collision_43.inc"
};
static NJS_MATERIAL collision_49_materials[1] = {
#include "stg10/terrain/collision_49_materials.inc"
};
static Sint16 collision_49_faces[10] = {
#include "stg10/terrain/collision_49_faces.inc"
};
static NJS_MESHSET collision_49_meshsets[1] = {
#include "stg10/terrain/collision_49_meshsets.inc"
};
static NJS_POINT3 collision_49_points[8] = {
#include "stg10/terrain/collision_49_points.inc"
};
static NJS_MODEL collision_49_model = {
#include "stg10/terrain/collision_49_model.inc"
};
static NJS_OBJECT collision_49 = {
#include "stg10/terrain/collision_49.inc"
};
static NJS_MATERIAL collision_51_materials[1] = {
#include "stg10/terrain/collision_51_materials.inc"
};
static Sint16 collision_51_faces[114] = {
#include "stg10/terrain/collision_51_faces.inc"
};
static NJS_MESHSET collision_51_meshsets[1] = {
#include "stg10/terrain/collision_51_meshsets.inc"
};
static NJS_POINT3 collision_51_points[76] = {
#include "stg10/terrain/collision_51_points.inc"
};
static NJS_MODEL collision_51_model = {
#include "stg10/terrain/collision_51_model.inc"
};
static NJS_OBJECT collision_51 = {
#include "stg10/terrain/collision_51.inc"
};
static NJS_MATERIAL collision_52_materials[1] = {
#include "stg10/terrain/collision_52_materials.inc"
};
static Sint16 collision_52_faces[114] = {
#include "stg10/terrain/collision_52_faces.inc"
};
static NJS_MESHSET collision_52_meshsets[1] = {
#include "stg10/terrain/collision_52_meshsets.inc"
};
static NJS_POINT3 collision_52_points[76] = {
#include "stg10/terrain/collision_52_points.inc"
};
static NJS_MODEL collision_52_model = {
#include "stg10/terrain/collision_52_model.inc"
};
static NJS_OBJECT collision_52 = {
#include "stg10/terrain/collision_52.inc"
};
static NJS_MATERIAL collision_55_materials[1] = {
#include "stg10/terrain/collision_55_materials.inc"
};
static Sint16 collision_55_faces[10] = {
#include "stg10/terrain/collision_55_faces.inc"
};
static NJS_MESHSET collision_55_meshsets[1] = {
#include "stg10/terrain/collision_55_meshsets.inc"
};
static NJS_POINT3 collision_55_points[8] = {
#include "stg10/terrain/collision_55_points.inc"
};
static NJS_MODEL collision_55_model = {
#include "stg10/terrain/collision_55_model.inc"
};
static NJS_OBJECT collision_55 = {
#include "stg10/terrain/collision_55.inc"
};
static NJS_MATERIAL collision_63_materials[1] = {
#include "stg10/terrain/collision_63_materials.inc"
};
static Sint16 collision_63_faces[10] = {
#include "stg10/terrain/collision_63_faces.inc"
};
static NJS_MESHSET collision_63_meshsets[1] = {
#include "stg10/terrain/collision_63_meshsets.inc"
};
static NJS_POINT3 collision_63_points[8] = {
#include "stg10/terrain/collision_63_points.inc"
};
static NJS_MODEL collision_63_model = {
#include "stg10/terrain/collision_63_model.inc"
};
static NJS_OBJECT collision_63 = {
#include "stg10/terrain/collision_63.inc"
};
static NJS_MATERIAL collision_64_materials[1] = {
#include "stg10/terrain/collision_64_materials.inc"
};
static Sint16 collision_64_faces[5] = {
#include "stg10/terrain/collision_64_faces.inc"
};
static NJS_MESHSET collision_64_meshsets[1] = {
#include "stg10/terrain/collision_64_meshsets.inc"
};
static NJS_POINT3 collision_64_points[4] = {
#include "stg10/terrain/collision_64_points.inc"
};
static NJS_MODEL collision_64_model = {
#include "stg10/terrain/collision_64_model.inc"
};
static NJS_OBJECT collision_64 = {
#include "stg10/terrain/collision_64.inc"
};
static NJS_MATERIAL collision_71_materials[1] = {
#include "stg10/terrain/collision_71_materials.inc"
};
static Sint16 collision_71_faces[18] = {
#include "stg10/terrain/collision_71_faces.inc"
};
static NJS_MESHSET collision_71_meshsets[1] = {
#include "stg10/terrain/collision_71_meshsets.inc"
};
static NJS_POINT3 collision_71_points[16] = {
#include "stg10/terrain/collision_71_points.inc"
};
static NJS_MODEL collision_71_model = {
#include "stg10/terrain/collision_71_model.inc"
};
static NJS_OBJECT collision_71 = {
#include "stg10/terrain/collision_71.inc"
};
static NJS_MATERIAL collision_73_materials[1] = {
#include "stg10/terrain/collision_73_materials.inc"
};
static Sint16 collision_73_faces[22] = {
#include "stg10/terrain/collision_73_faces.inc"
};
static NJS_MESHSET collision_73_meshsets[1] = {
#include "stg10/terrain/collision_73_meshsets.inc"
};
static NJS_POINT3 collision_73_points[20] = {
#include "stg10/terrain/collision_73_points.inc"
};
static NJS_MODEL collision_73_model = {
#include "stg10/terrain/collision_73_model.inc"
};
static NJS_OBJECT collision_73 = {
#include "stg10/terrain/collision_73.inc"
};
static NJS_MATERIAL collision_79_materials[1] = {
#include "stg10/terrain/collision_79_materials.inc"
};
static Sint16 collision_79_faces[85] = {
#include "stg10/terrain/collision_79_faces.inc"
};
static NJS_MESHSET collision_79_meshsets[1] = {
#include "stg10/terrain/collision_79_meshsets.inc"
};
static NJS_POINT3 collision_79_points[68] = {
#include "stg10/terrain/collision_79_points.inc"
};
static NJS_MODEL collision_79_model = {
#include "stg10/terrain/collision_79_model.inc"
};
static NJS_OBJECT collision_79 = {
#include "stg10/terrain/collision_79.inc"
};
static NJS_MATERIAL collision_81_materials[1] = {
#include "stg10/terrain/collision_81_materials.inc"
};
static Sint16 collision_81_faces[22] = {
#include "stg10/terrain/collision_81_faces.inc"
};
static NJS_MESHSET collision_81_meshsets[1] = {
#include "stg10/terrain/collision_81_meshsets.inc"
};
static NJS_POINT3 collision_81_points[20] = {
#include "stg10/terrain/collision_81_points.inc"
};
static NJS_MODEL collision_81_model = {
#include "stg10/terrain/collision_81_model.inc"
};
static NJS_OBJECT collision_81 = {
#include "stg10/terrain/collision_81.inc"
};
static NJS_MATERIAL collision_85_materials[1] = {
#include "stg10/terrain/collision_85_materials.inc"
};
static Sint16 collision_85_faces[49] = {
#include "stg10/terrain/collision_85_faces.inc"
};
static NJS_MESHSET collision_85_meshsets[1] = {
#include "stg10/terrain/collision_85_meshsets.inc"
};
static NJS_POINT3 collision_85_points[34] = {
#include "stg10/terrain/collision_85_points.inc"
};
static NJS_MODEL collision_85_model = {
#include "stg10/terrain/collision_85_model.inc"
};
static NJS_OBJECT collision_85 = {
#include "stg10/terrain/collision_85.inc"
};
static NJS_MATERIAL collision_88_materials[1] = {
#include "stg10/terrain/collision_88_materials.inc"
};
static Sint16 collision_88_faces[22] = {
#include "stg10/terrain/collision_88_faces.inc"
};
static NJS_MESHSET collision_88_meshsets[1] = {
#include "stg10/terrain/collision_88_meshsets.inc"
};
static NJS_POINT3 collision_88_points[12] = {
#include "stg10/terrain/collision_88_points.inc"
};
static NJS_MODEL collision_88_model = {
#include "stg10/terrain/collision_88_model.inc"
};
static NJS_OBJECT collision_88 = {
#include "stg10/terrain/collision_88.inc"
};
static NJS_MATERIAL collision_89_materials[1] = {
#include "stg10/terrain/collision_89_materials.inc"
};
static Sint16 collision_89_faces[5] = {
#include "stg10/terrain/collision_89_faces.inc"
};
static NJS_MESHSET collision_89_meshsets[1] = {
#include "stg10/terrain/collision_89_meshsets.inc"
};
static NJS_POINT3 collision_89_points[4] = {
#include "stg10/terrain/collision_89_points.inc"
};
static NJS_MODEL collision_89_model = {
#include "stg10/terrain/collision_89_model.inc"
};
static NJS_OBJECT collision_89 = {
#include "stg10/terrain/collision_89.inc"
};
static NJS_MATERIAL collision_100_materials[1] = {
#include "stg10/terrain/collision_100_materials.inc"
};
static Sint16 collision_100_faces[27] = {
#include "stg10/terrain/collision_100_faces.inc"
};
static NJS_MESHSET collision_100_meshsets[1] = {
#include "stg10/terrain/collision_100_meshsets.inc"
};
static NJS_POINT3 collision_100_points[18] = {
#include "stg10/terrain/collision_100_points.inc"
};
static NJS_MODEL collision_100_model = {
#include "stg10/terrain/collision_100_model.inc"
};
static NJS_OBJECT collision_100 = {
#include "stg10/terrain/collision_100.inc"
};
static NJS_MATERIAL collision_102_materials[1] = {
#include "stg10/terrain/collision_102_materials.inc"
};
static Sint16 collision_102_faces[38] = {
#include "stg10/terrain/collision_102_faces.inc"
};
static NJS_MESHSET collision_102_meshsets[1] = {
#include "stg10/terrain/collision_102_meshsets.inc"
};
static NJS_POINT3 collision_102_points[16] = {
#include "stg10/terrain/collision_102_points.inc"
};
static NJS_MODEL collision_102_model = {
#include "stg10/terrain/collision_102_model.inc"
};
static NJS_OBJECT collision_102 = {
#include "stg10/terrain/collision_102.inc"
};
static NJS_MATERIAL collision_103_materials[1] = {
#include "stg10/terrain/collision_103_materials.inc"
};
static Sint16 collision_103_faces[40] = {
#include "stg10/terrain/collision_103_faces.inc"
};
static NJS_MESHSET collision_103_meshsets[1] = {
#include "stg10/terrain/collision_103_meshsets.inc"
};
static NJS_POINT3 collision_103_points[16] = {
#include "stg10/terrain/collision_103_points.inc"
};
static NJS_MODEL collision_103_model = {
#include "stg10/terrain/collision_103_model.inc"
};
static NJS_OBJECT collision_103 = {
#include "stg10/terrain/collision_103.inc"
};
static NJS_MATERIAL collision_104_materials[1] = {
#include "stg10/terrain/collision_104_materials.inc"
};
static Sint16 collision_104_faces[40] = {
#include "stg10/terrain/collision_104_faces.inc"
};
static NJS_MESHSET collision_104_meshsets[1] = {
#include "stg10/terrain/collision_104_meshsets.inc"
};
static NJS_POINT3 collision_104_points[16] = {
#include "stg10/terrain/collision_104_points.inc"
};
static NJS_MODEL collision_104_model = {
#include "stg10/terrain/collision_104_model.inc"
};
static NJS_OBJECT collision_104 = {
#include "stg10/terrain/collision_104.inc"
};
static NJS_MATERIAL collision_105_materials[1] = {
#include "stg10/terrain/collision_105_materials.inc"
};
static Sint16 collision_105_faces[40] = {
#include "stg10/terrain/collision_105_faces.inc"
};
static NJS_MESHSET collision_105_meshsets[1] = {
#include "stg10/terrain/collision_105_meshsets.inc"
};
static NJS_POINT3 collision_105_points[16] = {
#include "stg10/terrain/collision_105_points.inc"
};
static NJS_MODEL collision_105_model = {
#include "stg10/terrain/collision_105_model.inc"
};
static NJS_OBJECT collision_105 = {
#include "stg10/terrain/collision_105.inc"
};
static NJS_MATERIAL collision_106_materials[1] = {
#include "stg10/terrain/collision_106_materials.inc"
};
static Sint16 collision_106_faces[40] = {
#include "stg10/terrain/collision_106_faces.inc"
};
static NJS_MESHSET collision_106_meshsets[1] = {
#include "stg10/terrain/collision_106_meshsets.inc"
};
static NJS_POINT3 collision_106_points[16] = {
#include "stg10/terrain/collision_106_points.inc"
};
static NJS_MODEL collision_106_model = {
#include "stg10/terrain/collision_106_model.inc"
};
static NJS_OBJECT collision_106 = {
#include "stg10/terrain/collision_106.inc"
};
static NJS_MATERIAL collision_107_materials[1] = {
#include "stg10/terrain/collision_107_materials.inc"
};
static Sint16 collision_107_faces[40] = {
#include "stg10/terrain/collision_107_faces.inc"
};
static NJS_MESHSET collision_107_meshsets[1] = {
#include "stg10/terrain/collision_107_meshsets.inc"
};
static NJS_POINT3 collision_107_points[16] = {
#include "stg10/terrain/collision_107_points.inc"
};
static NJS_MODEL collision_107_model = {
#include "stg10/terrain/collision_107_model.inc"
};
static NJS_OBJECT collision_107 = {
#include "stg10/terrain/collision_107.inc"
};
static NJS_MATERIAL collision_108_materials[1] = {
#include "stg10/terrain/collision_108_materials.inc"
};
static Sint16 collision_108_faces[40] = {
#include "stg10/terrain/collision_108_faces.inc"
};
static NJS_MESHSET collision_108_meshsets[1] = {
#include "stg10/terrain/collision_108_meshsets.inc"
};
static NJS_POINT3 collision_108_points[16] = {
#include "stg10/terrain/collision_108_points.inc"
};
static NJS_MODEL collision_108_model = {
#include "stg10/terrain/collision_108_model.inc"
};
static NJS_OBJECT collision_108 = {
#include "stg10/terrain/collision_108.inc"
};
static NJS_MATERIAL collision_109_materials[1] = {
#include "stg10/terrain/collision_109_materials.inc"
};
static Sint16 collision_109_faces[19] = {
#include "stg10/terrain/collision_109_faces.inc"
};
static NJS_MESHSET collision_109_meshsets[1] = {
#include "stg10/terrain/collision_109_meshsets.inc"
};
static NJS_POINT3 collision_109_points[16] = {
#include "stg10/terrain/collision_109_points.inc"
};
static NJS_MODEL collision_109_model = {
#include "stg10/terrain/collision_109_model.inc"
};
static NJS_OBJECT collision_109 = {
#include "stg10/terrain/collision_109.inc"
};
static NJS_MATERIAL collision_110_materials[1] = {
#include "stg10/terrain/collision_110_materials.inc"
};
static Sint16 collision_110_faces[22] = {
#include "stg10/terrain/collision_110_faces.inc"
};
static NJS_MESHSET collision_110_meshsets[1] = {
#include "stg10/terrain/collision_110_meshsets.inc"
};
static NJS_POINT3 collision_110_points[12] = {
#include "stg10/terrain/collision_110_points.inc"
};
static NJS_MODEL collision_110_model = {
#include "stg10/terrain/collision_110_model.inc"
};
static NJS_OBJECT collision_110 = {
#include "stg10/terrain/collision_110.inc"
};
static NJS_MATERIAL collision_113_materials[1] = {
#include "stg10/terrain/collision_113_materials.inc"
};
static Sint16 collision_113_faces[55] = {
#include "stg10/terrain/collision_113_faces.inc"
};
static NJS_MESHSET collision_113_meshsets[1] = {
#include "stg10/terrain/collision_113_meshsets.inc"
};
static NJS_POINT3 collision_113_points[30] = {
#include "stg10/terrain/collision_113_points.inc"
};
static NJS_MODEL collision_113_model = {
#include "stg10/terrain/collision_113_model.inc"
};
static NJS_OBJECT collision_113 = {
#include "stg10/terrain/collision_113.inc"
};
static NJS_MATERIAL collision_116_materials[1] = {
#include "stg10/terrain/collision_116_materials.inc"
};
static Sint16 collision_116_faces[38] = {
#include "stg10/terrain/collision_116_faces.inc"
};
static NJS_MESHSET collision_116_meshsets[1] = {
#include "stg10/terrain/collision_116_meshsets.inc"
};
static NJS_POINT3 collision_116_points[16] = {
#include "stg10/terrain/collision_116_points.inc"
};
static NJS_MODEL collision_116_model = {
#include "stg10/terrain/collision_116_model.inc"
};
static NJS_OBJECT collision_116 = {
#include "stg10/terrain/collision_116.inc"
};
static NJS_MATERIAL collision_117_materials[1] = {
#include "stg10/terrain/collision_117_materials.inc"
};
static Sint16 collision_117_faces[166] = {
#include "stg10/terrain/collision_117_faces.inc"
};
static NJS_MESHSET collision_117_meshsets[1] = {
#include "stg10/terrain/collision_117_meshsets.inc"
};
static NJS_POINT3 collision_117_points[112] = {
#include "stg10/terrain/collision_117_points.inc"
};
static NJS_MODEL collision_117_model = {
#include "stg10/terrain/collision_117_model.inc"
};
static NJS_OBJECT collision_117 = {
#include "stg10/terrain/collision_117.inc"
};
static NJS_MATERIAL collision_118_materials[1] = {
#include "stg10/terrain/collision_118_materials.inc"
};
static Sint16 collision_118_faces[36] = {
#include "stg10/terrain/collision_118_faces.inc"
};
static NJS_MESHSET collision_118_meshsets[1] = {
#include "stg10/terrain/collision_118_meshsets.inc"
};
static NJS_POINT3 collision_118_points[16] = {
#include "stg10/terrain/collision_118_points.inc"
};
static NJS_MODEL collision_118_model = {
#include "stg10/terrain/collision_118_model.inc"
};
static NJS_OBJECT collision_118 = {
#include "stg10/terrain/collision_118.inc"
};
static NJS_MATERIAL collision_119_materials[1] = {
#include "stg10/terrain/collision_119_materials.inc"
};
static Sint16 collision_119_faces[15] = {
#include "stg10/terrain/collision_119_faces.inc"
};
static NJS_MESHSET collision_119_meshsets[1] = {
#include "stg10/terrain/collision_119_meshsets.inc"
};
static NJS_POINT3 collision_119_points[8] = {
#include "stg10/terrain/collision_119_points.inc"
};
static NJS_MODEL collision_119_model = {
#include "stg10/terrain/collision_119_model.inc"
};
static NJS_OBJECT collision_119 = {
#include "stg10/terrain/collision_119.inc"
};
static NJS_MATERIAL collision_122_materials[1] = {
#include "stg10/terrain/collision_122_materials.inc"
};
static Sint16 collision_122_faces[166] = {
#include "stg10/terrain/collision_122_faces.inc"
};
static NJS_MESHSET collision_122_meshsets[1] = {
#include "stg10/terrain/collision_122_meshsets.inc"
};
static NJS_POINT3 collision_122_points[112] = {
#include "stg10/terrain/collision_122_points.inc"
};
static NJS_MODEL collision_122_model = {
#include "stg10/terrain/collision_122_model.inc"
};
static NJS_OBJECT collision_122 = {
#include "stg10/terrain/collision_122.inc"
};
static NJS_MATERIAL collision_124_materials[1] = {
#include "stg10/terrain/collision_124_materials.inc"
};
static Sint16 collision_124_faces[17] = {
#include "stg10/terrain/collision_124_faces.inc"
};
static NJS_MESHSET collision_124_meshsets[1] = {
#include "stg10/terrain/collision_124_meshsets.inc"
};
static NJS_POINT3 collision_124_points[8] = {
#include "stg10/terrain/collision_124_points.inc"
};
static NJS_MODEL collision_124_model = {
#include "stg10/terrain/collision_124_model.inc"
};
static NJS_OBJECT collision_124 = {
#include "stg10/terrain/collision_124.inc"
};
static NJS_MATERIAL collision_127_materials[1] = {
#include "stg10/terrain/collision_127_materials.inc"
};
static Sint16 collision_127_faces[166] = {
#include "stg10/terrain/collision_127_faces.inc"
};
static NJS_MESHSET collision_127_meshsets[1] = {
#include "stg10/terrain/collision_127_meshsets.inc"
};
static NJS_POINT3 collision_127_points[112] = {
#include "stg10/terrain/collision_127_points.inc"
};
static NJS_MODEL collision_127_model = {
#include "stg10/terrain/collision_127_model.inc"
};
static NJS_OBJECT collision_127 = {
#include "stg10/terrain/collision_127.inc"
};
static NJS_MATERIAL collision_129_materials[1] = {
#include "stg10/terrain/collision_129_materials.inc"
};
static Sint16 collision_129_faces[55] = {
#include "stg10/terrain/collision_129_faces.inc"
};
static NJS_MESHSET collision_129_meshsets[1] = {
#include "stg10/terrain/collision_129_meshsets.inc"
};
static NJS_POINT3 collision_129_points[32] = {
#include "stg10/terrain/collision_129_points.inc"
};
static NJS_MODEL collision_129_model = {
#include "stg10/terrain/collision_129_model.inc"
};
static NJS_OBJECT collision_129 = {
#include "stg10/terrain/collision_129.inc"
};
static NJS_MATERIAL collision_131_materials[1] = {
#include "stg10/terrain/collision_131_materials.inc"
};
static Sint16 collision_131_faces[47] = {
#include "stg10/terrain/collision_131_faces.inc"
};
static NJS_MESHSET collision_131_meshsets[1] = {
#include "stg10/terrain/collision_131_meshsets.inc"
};
static NJS_POINT3 collision_131_points[28] = {
#include "stg10/terrain/collision_131_points.inc"
};
static NJS_MODEL collision_131_model = {
#include "stg10/terrain/collision_131_model.inc"
};
static NJS_OBJECT collision_131 = {
#include "stg10/terrain/collision_131.inc"
};
static NJS_MATERIAL collision_133_materials[1] = {
#include "stg10/terrain/collision_133_materials.inc"
};
static Sint16 collision_133_faces[27] = {
#include "stg10/terrain/collision_133_faces.inc"
};
static NJS_MESHSET collision_133_meshsets[1] = {
#include "stg10/terrain/collision_133_meshsets.inc"
};
static NJS_POINT3 collision_133_points[21] = {
#include "stg10/terrain/collision_133_points.inc"
};
static NJS_MODEL collision_133_model = {
#include "stg10/terrain/collision_133_model.inc"
};
static NJS_OBJECT collision_133 = {
#include "stg10/terrain/collision_133.inc"
};
static NJS_MATERIAL collision_134_materials[1] = {
#include "stg10/terrain/collision_134_materials.inc"
};
static Sint16 collision_134_faces[19] = {
#include "stg10/terrain/collision_134_faces.inc"
};
static NJS_MESHSET collision_134_meshsets[1] = {
#include "stg10/terrain/collision_134_meshsets.inc"
};
static NJS_POINT3 collision_134_points[16] = {
#include "stg10/terrain/collision_134_points.inc"
};
static NJS_MODEL collision_134_model = {
#include "stg10/terrain/collision_134_model.inc"
};
static NJS_OBJECT collision_134 = {
#include "stg10/terrain/collision_134.inc"
};
static NJS_MATERIAL collision_148_materials[1] = {
#include "stg10/terrain/collision_148_materials.inc"
};
static Sint16 collision_148_faces[19] = {
#include "stg10/terrain/collision_148_faces.inc"
};
static NJS_MESHSET collision_148_meshsets[1] = {
#include "stg10/terrain/collision_148_meshsets.inc"
};
static NJS_POINT3 collision_148_points[8] = {
#include "stg10/terrain/collision_148_points.inc"
};
static NJS_MODEL collision_148_model = {
#include "stg10/terrain/collision_148_model.inc"
};
static NJS_OBJECT collision_148 = {
#include "stg10/terrain/collision_148.inc"
};
static NJS_MATERIAL collision_149_materials[1] = {
#include "stg10/terrain/collision_149_materials.inc"
};
static Sint16 collision_149_faces[31] = {
#include "stg10/terrain/collision_149_faces.inc"
};
static NJS_MESHSET collision_149_meshsets[1] = {
#include "stg10/terrain/collision_149_meshsets.inc"
};
static NJS_POINT3 collision_149_points[18] = {
#include "stg10/terrain/collision_149_points.inc"
};
static NJS_MODEL collision_149_model = {
#include "stg10/terrain/collision_149_model.inc"
};
static NJS_OBJECT collision_149 = {
#include "stg10/terrain/collision_149.inc"
};
static NJS_MATERIAL collision_150_materials[1] = {
#include "stg10/terrain/collision_150_materials.inc"
};
static Sint16 collision_150_faces[19] = {
#include "stg10/terrain/collision_150_faces.inc"
};
static NJS_MESHSET collision_150_meshsets[1] = {
#include "stg10/terrain/collision_150_meshsets.inc"
};
static NJS_POINT3 collision_150_points[16] = {
#include "stg10/terrain/collision_150_points.inc"
};
static NJS_MODEL collision_150_model = {
#include "stg10/terrain/collision_150_model.inc"
};
static NJS_OBJECT collision_150 = {
#include "stg10/terrain/collision_150.inc"
};
static NJS_MATERIAL collision_154_materials[1] = {
#include "stg10/terrain/collision_154_materials.inc"
};
static Sint16 collision_154_faces[65] = {
#include "stg10/terrain/collision_154_faces.inc"
};
static NJS_MESHSET collision_154_meshsets[1] = {
#include "stg10/terrain/collision_154_meshsets.inc"
};
static NJS_POINT3 collision_154_points[39] = {
#include "stg10/terrain/collision_154_points.inc"
};
static NJS_MODEL collision_154_model = {
#include "stg10/terrain/collision_154_model.inc"
};
static NJS_OBJECT collision_154 = {
#include "stg10/terrain/collision_154.inc"
};
static NJS_MATERIAL collision_155_materials[1] = {
#include "stg10/terrain/collision_155_materials.inc"
};
static Sint16 collision_155_faces[33] = {
#include "stg10/terrain/collision_155_faces.inc"
};
static NJS_MESHSET collision_155_meshsets[1] = {
#include "stg10/terrain/collision_155_meshsets.inc"
};
static NJS_POINT3 collision_155_points[16] = {
#include "stg10/terrain/collision_155_points.inc"
};
static NJS_MODEL collision_155_model = {
#include "stg10/terrain/collision_155_model.inc"
};
static NJS_OBJECT collision_155 = {
#include "stg10/terrain/collision_155.inc"
};
static NJS_MATERIAL collision_156_materials[1] = {
#include "stg10/terrain/collision_156_materials.inc"
};
static Sint16 collision_156_faces[166] = {
#include "stg10/terrain/collision_156_faces.inc"
};
static NJS_MESHSET collision_156_meshsets[1] = {
#include "stg10/terrain/collision_156_meshsets.inc"
};
static NJS_POINT3 collision_156_points[112] = {
#include "stg10/terrain/collision_156_points.inc"
};
static NJS_MODEL collision_156_model = {
#include "stg10/terrain/collision_156_model.inc"
};
static NJS_OBJECT collision_156 = {
#include "stg10/terrain/collision_156.inc"
};
static NJS_MATERIAL collision_157_materials[1] = {
#include "stg10/terrain/collision_157_materials.inc"
};
static Sint16 collision_157_faces[10] = {
#include "stg10/terrain/collision_157_faces.inc"
};
static NJS_MESHSET collision_157_meshsets[1] = {
#include "stg10/terrain/collision_157_meshsets.inc"
};
static NJS_POINT3 collision_157_points[8] = {
#include "stg10/terrain/collision_157_points.inc"
};
static NJS_MODEL collision_157_model = {
#include "stg10/terrain/collision_157_model.inc"
};
static NJS_OBJECT collision_157 = {
#include "stg10/terrain/collision_157.inc"
};
static NJS_MATERIAL collision_159_materials[1] = {
#include "stg10/terrain/collision_159_materials.inc"
};
static Sint16 collision_159_faces[15] = {
#include "stg10/terrain/collision_159_faces.inc"
};
static NJS_MESHSET collision_159_meshsets[1] = {
#include "stg10/terrain/collision_159_meshsets.inc"
};
static NJS_POINT3 collision_159_points[8] = {
#include "stg10/terrain/collision_159_points.inc"
};
static NJS_MODEL collision_159_model = {
#include "stg10/terrain/collision_159_model.inc"
};
static NJS_OBJECT collision_159 = {
#include "stg10/terrain/collision_159.inc"
};
static NJS_MATERIAL collision_160_materials[1] = {
#include "stg10/terrain/collision_160_materials.inc"
};
static Sint16 collision_160_faces[15] = {
#include "stg10/terrain/collision_160_faces.inc"
};
static NJS_MESHSET collision_160_meshsets[1] = {
#include "stg10/terrain/collision_160_meshsets.inc"
};
static NJS_POINT3 collision_160_points[8] = {
#include "stg10/terrain/collision_160_points.inc"
};
static NJS_MODEL collision_160_model = {
#include "stg10/terrain/collision_160_model.inc"
};
static NJS_OBJECT collision_160 = {
#include "stg10/terrain/collision_160.inc"
};
static NJS_MATERIAL collision_161_materials[1] = {
#include "stg10/terrain/collision_161_materials.inc"
};
static Sint16 collision_161_faces[15] = {
#include "stg10/terrain/collision_161_faces.inc"
};
static NJS_MESHSET collision_161_meshsets[1] = {
#include "stg10/terrain/collision_161_meshsets.inc"
};
static NJS_POINT3 collision_161_points[8] = {
#include "stg10/terrain/collision_161_points.inc"
};
static NJS_MODEL collision_161_model = {
#include "stg10/terrain/collision_161_model.inc"
};
static NJS_OBJECT collision_161 = {
#include "stg10/terrain/collision_161.inc"
};
static NJS_MATERIAL collision_162_materials[1] = {
#include "stg10/terrain/collision_162_materials.inc"
};
static Sint16 collision_162_faces[15] = {
#include "stg10/terrain/collision_162_faces.inc"
};
static NJS_MESHSET collision_162_meshsets[1] = {
#include "stg10/terrain/collision_162_meshsets.inc"
};
static NJS_POINT3 collision_162_points[8] = {
#include "stg10/terrain/collision_162_points.inc"
};
static NJS_MODEL collision_162_model = {
#include "stg10/terrain/collision_162_model.inc"
};
static NJS_OBJECT collision_162 = {
#include "stg10/terrain/collision_162.inc"
};
static NJS_MATERIAL collision_163_materials[1] = {
#include "stg10/terrain/collision_163_materials.inc"
};
static Sint16 collision_163_faces[15] = {
#include "stg10/terrain/collision_163_faces.inc"
};
static NJS_MESHSET collision_163_meshsets[1] = {
#include "stg10/terrain/collision_163_meshsets.inc"
};
static NJS_POINT3 collision_163_points[8] = {
#include "stg10/terrain/collision_163_points.inc"
};
static NJS_MODEL collision_163_model = {
#include "stg10/terrain/collision_163_model.inc"
};
static NJS_OBJECT collision_163 = {
#include "stg10/terrain/collision_163.inc"
};
static NJS_MATERIAL collision_164_materials[1] = {
#include "stg10/terrain/collision_164_materials.inc"
};
static Sint16 collision_164_faces[61] = {
#include "stg10/terrain/collision_164_faces.inc"
};
static NJS_MESHSET collision_164_meshsets[1] = {
#include "stg10/terrain/collision_164_meshsets.inc"
};
static NJS_POINT3 collision_164_points[16] = {
#include "stg10/terrain/collision_164_points.inc"
};
static NJS_MODEL collision_164_model = {
#include "stg10/terrain/collision_164_model.inc"
};
static NJS_OBJECT collision_164 = {
#include "stg10/terrain/collision_164.inc"
};
static NJS_MATERIAL collision_165_materials[1] = {
#include "stg10/terrain/collision_165_materials.inc"
};
static Sint16 collision_165_faces[19] = {
#include "stg10/terrain/collision_165_faces.inc"
};
static NJS_MESHSET collision_165_meshsets[1] = {
#include "stg10/terrain/collision_165_meshsets.inc"
};
static NJS_POINT3 collision_165_points[8] = {
#include "stg10/terrain/collision_165_points.inc"
};
static NJS_MODEL collision_165_model = {
#include "stg10/terrain/collision_165_model.inc"
};
static NJS_OBJECT collision_165 = {
#include "stg10/terrain/collision_165.inc"
};
static NJS_MATERIAL collision_166_materials[1] = {
#include "stg10/terrain/collision_166_materials.inc"
};
static Sint16 collision_166_faces[114] = {
#include "stg10/terrain/collision_166_faces.inc"
};
static NJS_MESHSET collision_166_meshsets[1] = {
#include "stg10/terrain/collision_166_meshsets.inc"
};
static NJS_POINT3 collision_166_points[76] = {
#include "stg10/terrain/collision_166_points.inc"
};
static NJS_MODEL collision_166_model = {
#include "stg10/terrain/collision_166_model.inc"
};
static NJS_OBJECT collision_166 = {
#include "stg10/terrain/collision_166.inc"
};
static NJS_MATERIAL collision_167_materials[1] = {
#include "stg10/terrain/collision_167_materials.inc"
};
static Sint16 collision_167_faces[19] = {
#include "stg10/terrain/collision_167_faces.inc"
};
static NJS_MESHSET collision_167_meshsets[1] = {
#include "stg10/terrain/collision_167_meshsets.inc"
};
static NJS_POINT3 collision_167_points[8] = {
#include "stg10/terrain/collision_167_points.inc"
};
static NJS_MODEL collision_167_model = {
#include "stg10/terrain/collision_167_model.inc"
};
static NJS_OBJECT collision_167 = {
#include "stg10/terrain/collision_167.inc"
};
static NJS_MATERIAL collision_168_materials[1] = {
#include "stg10/terrain/collision_168_materials.inc"
};
static Sint16 collision_168_faces[19] = {
#include "stg10/terrain/collision_168_faces.inc"
};
static NJS_MESHSET collision_168_meshsets[1] = {
#include "stg10/terrain/collision_168_meshsets.inc"
};
static NJS_POINT3 collision_168_points[8] = {
#include "stg10/terrain/collision_168_points.inc"
};
static NJS_MODEL collision_168_model = {
#include "stg10/terrain/collision_168_model.inc"
};
static NJS_OBJECT collision_168 = {
#include "stg10/terrain/collision_168.inc"
};
static NJS_MATERIAL collision_169_materials[1] = {
#include "stg10/terrain/collision_169_materials.inc"
};
static Sint16 collision_169_faces[14] = {
#include "stg10/terrain/collision_169_faces.inc"
};
static NJS_MESHSET collision_169_meshsets[1] = {
#include "stg10/terrain/collision_169_meshsets.inc"
};
static NJS_POINT3 collision_169_points[8] = {
#include "stg10/terrain/collision_169_points.inc"
};
static NJS_MODEL collision_169_model = {
#include "stg10/terrain/collision_169_model.inc"
};
static NJS_OBJECT collision_169 = {
#include "stg10/terrain/collision_169.inc"
};
static NJS_MATERIAL collision_170_materials[1] = {
#include "stg10/terrain/collision_170_materials.inc"
};
static Sint16 collision_170_faces[19] = {
#include "stg10/terrain/collision_170_faces.inc"
};
static NJS_MESHSET collision_170_meshsets[1] = {
#include "stg10/terrain/collision_170_meshsets.inc"
};
static NJS_POINT3 collision_170_points[8] = {
#include "stg10/terrain/collision_170_points.inc"
};
static NJS_MODEL collision_170_model = {
#include "stg10/terrain/collision_170_model.inc"
};
static NJS_OBJECT collision_170 = {
#include "stg10/terrain/collision_170.inc"
};
static NJS_MATERIAL collision_171_materials[1] = {
#include "stg10/terrain/collision_171_materials.inc"
};
static Sint16 collision_171_faces[19] = {
#include "stg10/terrain/collision_171_faces.inc"
};
static NJS_MESHSET collision_171_meshsets[1] = {
#include "stg10/terrain/collision_171_meshsets.inc"
};
static NJS_POINT3 collision_171_points[8] = {
#include "stg10/terrain/collision_171_points.inc"
};
static NJS_MODEL collision_171_model = {
#include "stg10/terrain/collision_171_model.inc"
};
static NJS_OBJECT collision_171 = {
#include "stg10/terrain/collision_171.inc"
};
static NJS_MATERIAL collision_172_materials[1] = {
#include "stg10/terrain/collision_172_materials.inc"
};
static Sint16 collision_172_faces[19] = {
#include "stg10/terrain/collision_172_faces.inc"
};
static NJS_MESHSET collision_172_meshsets[1] = {
#include "stg10/terrain/collision_172_meshsets.inc"
};
static NJS_POINT3 collision_172_points[8] = {
#include "stg10/terrain/collision_172_points.inc"
};
static NJS_MODEL collision_172_model = {
#include "stg10/terrain/collision_172_model.inc"
};
static NJS_OBJECT collision_172 = {
#include "stg10/terrain/collision_172.inc"
};
static NJS_MATERIAL collision_173_materials[1] = {
#include "stg10/terrain/collision_173_materials.inc"
};
static Sint16 collision_173_faces[19] = {
#include "stg10/terrain/collision_173_faces.inc"
};
static NJS_MESHSET collision_173_meshsets[1] = {
#include "stg10/terrain/collision_173_meshsets.inc"
};
static NJS_POINT3 collision_173_points[8] = {
#include "stg10/terrain/collision_173_points.inc"
};
static NJS_MODEL collision_173_model = {
#include "stg10/terrain/collision_173_model.inc"
};
static NJS_OBJECT collision_173 = {
#include "stg10/terrain/collision_173.inc"
};
static NJS_MATERIAL collision_174_materials[1] = {
#include "stg10/terrain/collision_174_materials.inc"
};
static Sint16 collision_174_faces[14] = {
#include "stg10/terrain/collision_174_faces.inc"
};
static NJS_MESHSET collision_174_meshsets[1] = {
#include "stg10/terrain/collision_174_meshsets.inc"
};
static NJS_POINT3 collision_174_points[8] = {
#include "stg10/terrain/collision_174_points.inc"
};
static NJS_MODEL collision_174_model = {
#include "stg10/terrain/collision_174_model.inc"
};
static NJS_OBJECT collision_174 = {
#include "stg10/terrain/collision_174.inc"
};
static NJS_MATERIAL collision_175_materials[1] = {
#include "stg10/terrain/collision_175_materials.inc"
};
static Sint16 collision_175_faces[19] = {
#include "stg10/terrain/collision_175_faces.inc"
};
static NJS_MESHSET collision_175_meshsets[1] = {
#include "stg10/terrain/collision_175_meshsets.inc"
};
static NJS_POINT3 collision_175_points[8] = {
#include "stg10/terrain/collision_175_points.inc"
};
static NJS_MODEL collision_175_model = {
#include "stg10/terrain/collision_175_model.inc"
};
static NJS_OBJECT collision_175 = {
#include "stg10/terrain/collision_175.inc"
};
static NJS_MATERIAL collision_176_materials[1] = {
#include "stg10/terrain/collision_176_materials.inc"
};
static Sint16 collision_176_faces[19] = {
#include "stg10/terrain/collision_176_faces.inc"
};
static NJS_MESHSET collision_176_meshsets[1] = {
#include "stg10/terrain/collision_176_meshsets.inc"
};
static NJS_POINT3 collision_176_points[8] = {
#include "stg10/terrain/collision_176_points.inc"
};
static NJS_MODEL collision_176_model = {
#include "stg10/terrain/collision_176_model.inc"
};
static NJS_OBJECT collision_176 = {
#include "stg10/terrain/collision_176.inc"
};
static NJS_MATERIAL collision_177_materials[1] = {
#include "stg10/terrain/collision_177_materials.inc"
};
static Sint16 collision_177_faces[19] = {
#include "stg10/terrain/collision_177_faces.inc"
};
static NJS_MESHSET collision_177_meshsets[1] = {
#include "stg10/terrain/collision_177_meshsets.inc"
};
static NJS_POINT3 collision_177_points[8] = {
#include "stg10/terrain/collision_177_points.inc"
};
static NJS_MODEL collision_177_model = {
#include "stg10/terrain/collision_177_model.inc"
};
static NJS_OBJECT collision_177 = {
#include "stg10/terrain/collision_177.inc"
};
static NJS_MATERIAL collision_178_materials[1] = {
#include "stg10/terrain/collision_178_materials.inc"
};
static Sint16 collision_178_faces[19] = {
#include "stg10/terrain/collision_178_faces.inc"
};
static NJS_MESHSET collision_178_meshsets[1] = {
#include "stg10/terrain/collision_178_meshsets.inc"
};
static NJS_POINT3 collision_178_points[8] = {
#include "stg10/terrain/collision_178_points.inc"
};
static NJS_MODEL collision_178_model = {
#include "stg10/terrain/collision_178_model.inc"
};
static NJS_OBJECT collision_178 = {
#include "stg10/terrain/collision_178.inc"
};
static NJS_MATERIAL collision_179_materials[1] = {
#include "stg10/terrain/collision_179_materials.inc"
};
static Sint16 collision_179_faces[15] = {
#include "stg10/terrain/collision_179_faces.inc"
};
static NJS_MESHSET collision_179_meshsets[1] = {
#include "stg10/terrain/collision_179_meshsets.inc"
};
static NJS_POINT3 collision_179_points[8] = {
#include "stg10/terrain/collision_179_points.inc"
};
static NJS_MODEL collision_179_model = {
#include "stg10/terrain/collision_179_model.inc"
};
static NJS_OBJECT collision_179 = {
#include "stg10/terrain/collision_179.inc"
};
static NJS_MATERIAL collision_180_materials[1] = {
#include "stg10/terrain/collision_180_materials.inc"
};
static Sint16 collision_180_faces[14] = {
#include "stg10/terrain/collision_180_faces.inc"
};
static NJS_MESHSET collision_180_meshsets[1] = {
#include "stg10/terrain/collision_180_meshsets.inc"
};
static NJS_POINT3 collision_180_points[8] = {
#include "stg10/terrain/collision_180_points.inc"
};
static NJS_MODEL collision_180_model = {
#include "stg10/terrain/collision_180_model.inc"
};
static NJS_OBJECT collision_180 = {
#include "stg10/terrain/collision_180.inc"
};
static NJS_MATERIAL collision_181_materials[1] = {
#include "stg10/terrain/collision_181_materials.inc"
};
static Sint16 collision_181_faces[33] = {
#include "stg10/terrain/collision_181_faces.inc"
};
static NJS_MESHSET collision_181_meshsets[1] = {
#include "stg10/terrain/collision_181_meshsets.inc"
};
static NJS_POINT3 collision_181_points[12] = {
#include "stg10/terrain/collision_181_points.inc"
};
static NJS_MODEL collision_181_model = {
#include "stg10/terrain/collision_181_model.inc"
};
static NJS_OBJECT collision_181 = {
#include "stg10/terrain/collision_181.inc"
};
static NJS_MATERIAL collision_182_materials[1] = {
#include "stg10/terrain/collision_182_materials.inc"
};
static Sint16 collision_182_faces[14] = {
#include "stg10/terrain/collision_182_faces.inc"
};
static NJS_MESHSET collision_182_meshsets[1] = {
#include "stg10/terrain/collision_182_meshsets.inc"
};
static NJS_POINT3 collision_182_points[8] = {
#include "stg10/terrain/collision_182_points.inc"
};
static NJS_MODEL collision_182_model = {
#include "stg10/terrain/collision_182_model.inc"
};
static NJS_OBJECT collision_182 = {
#include "stg10/terrain/collision_182.inc"
};
static NJS_MATERIAL collision_183_materials[1] = {
#include "stg10/terrain/collision_183_materials.inc"
};
static Sint16 collision_183_faces[63] = {
#include "stg10/terrain/collision_183_faces.inc"
};
static NJS_MESHSET collision_183_meshsets[1] = {
#include "stg10/terrain/collision_183_meshsets.inc"
};
static NJS_POINT3 collision_183_points[30] = {
#include "stg10/terrain/collision_183_points.inc"
};
static NJS_MODEL collision_183_model = {
#include "stg10/terrain/collision_183_model.inc"
};
static NJS_OBJECT collision_183 = {
#include "stg10/terrain/collision_183.inc"
};
static NJS_MATERIAL collision_184_materials[1] = {
#include "stg10/terrain/collision_184_materials.inc"
};
static Sint16 collision_184_faces[28] = {
#include "stg10/terrain/collision_184_faces.inc"
};
static NJS_MESHSET collision_184_meshsets[1] = {
#include "stg10/terrain/collision_184_meshsets.inc"
};
static NJS_POINT3 collision_184_points[16] = {
#include "stg10/terrain/collision_184_points.inc"
};
static NJS_MODEL collision_184_model = {
#include "stg10/terrain/collision_184_model.inc"
};
static NJS_OBJECT collision_184 = {
#include "stg10/terrain/collision_184.inc"
};
static NJS_MATERIAL collision_186_materials[1] = {
#include "stg10/terrain/collision_186_materials.inc"
};
static Sint16 collision_186_faces[15] = {
#include "stg10/terrain/collision_186_faces.inc"
};
static NJS_MESHSET collision_186_meshsets[1] = {
#include "stg10/terrain/collision_186_meshsets.inc"
};
static NJS_POINT3 collision_186_points[12] = {
#include "stg10/terrain/collision_186_points.inc"
};
static NJS_MODEL collision_186_model = {
#include "stg10/terrain/collision_186_model.inc"
};
static NJS_OBJECT collision_186 = {
#include "stg10/terrain/collision_186.inc"
};
static NJS_MATERIAL collision_187_materials[1] = {
#include "stg10/terrain/collision_187_materials.inc"
};
static Sint16 collision_187_faces[59] = {
#include "stg10/terrain/collision_187_faces.inc"
};
static NJS_MESHSET collision_187_meshsets[1] = {
#include "stg10/terrain/collision_187_meshsets.inc"
};
static NJS_POINT3 collision_187_points[34] = {
#include "stg10/terrain/collision_187_points.inc"
};
static NJS_MODEL collision_187_model = {
#include "stg10/terrain/collision_187_model.inc"
};
static NJS_OBJECT collision_187 = {
#include "stg10/terrain/collision_187.inc"
};
static NJS_MATERIAL collision_188_materials[1] = {
#include "stg10/terrain/collision_188_materials.inc"
};
static Sint16 collision_188_faces[42] = {
#include "stg10/terrain/collision_188_faces.inc"
};
static NJS_MESHSET collision_188_meshsets[1] = {
#include "stg10/terrain/collision_188_meshsets.inc"
};
static NJS_POINT3 collision_188_points[17] = {
#include "stg10/terrain/collision_188_points.inc"
};
static NJS_MODEL collision_188_model = {
#include "stg10/terrain/collision_188_model.inc"
};
static NJS_OBJECT collision_188 = {
#include "stg10/terrain/collision_188.inc"
};
static NJS_MATERIAL collision_190_materials[1] = {
#include "stg10/terrain/collision_190_materials.inc"
};
static Sint16 collision_190_faces[189] = {
#include "stg10/terrain/collision_190_faces.inc"
};
static NJS_MESHSET collision_190_meshsets[1] = {
#include "stg10/terrain/collision_190_meshsets.inc"
};
static NJS_POINT3 collision_190_points[72] = {
#include "stg10/terrain/collision_190_points.inc"
};
static NJS_MODEL collision_190_model = {
#include "stg10/terrain/collision_190_model.inc"
};
static NJS_OBJECT collision_190 = {
#include "stg10/terrain/collision_190.inc"
};
static NJS_MATERIAL collision_196_materials[1] = {
#include "stg10/terrain/collision_196_materials.inc"
};
static Sint16 collision_196_faces[114] = {
#include "stg10/terrain/collision_196_faces.inc"
};
static NJS_MESHSET collision_196_meshsets[1] = {
#include "stg10/terrain/collision_196_meshsets.inc"
};
static NJS_POINT3 collision_196_points[76] = {
#include "stg10/terrain/collision_196_points.inc"
};
static NJS_MODEL collision_196_model = {
#include "stg10/terrain/collision_196_model.inc"
};
static NJS_OBJECT collision_196 = {
#include "stg10/terrain/collision_196.inc"
};
static NJS_MATERIAL collision_199_materials[1] = {
#include "stg10/terrain/collision_199_materials.inc"
};
static Sint16 collision_199_faces[19] = {
#include "stg10/terrain/collision_199_faces.inc"
};
static NJS_MESHSET collision_199_meshsets[1] = {
#include "stg10/terrain/collision_199_meshsets.inc"
};
static NJS_POINT3 collision_199_points[16] = {
#include "stg10/terrain/collision_199_points.inc"
};
static NJS_MODEL collision_199_model = {
#include "stg10/terrain/collision_199_model.inc"
};
static NJS_OBJECT collision_199 = {
#include "stg10/terrain/collision_199.inc"
};
static NJS_MATERIAL collision_200_materials[1] = {
#include "stg10/terrain/collision_200_materials.inc"
};
static Sint16 collision_200_faces[33] = {
#include "stg10/terrain/collision_200_faces.inc"
};
static NJS_MESHSET collision_200_meshsets[1] = {
#include "stg10/terrain/collision_200_meshsets.inc"
};
static NJS_POINT3 collision_200_points[16] = {
#include "stg10/terrain/collision_200_points.inc"
};
static NJS_MODEL collision_200_model = {
#include "stg10/terrain/collision_200_model.inc"
};
static NJS_OBJECT collision_200 = {
#include "stg10/terrain/collision_200.inc"
};
static NJS_MATERIAL collision_201_materials[1] = {
#include "stg10/terrain/collision_201_materials.inc"
};
static Sint16 collision_201_faces[37] = {
#include "stg10/terrain/collision_201_faces.inc"
};
static NJS_MESHSET collision_201_meshsets[1] = {
#include "stg10/terrain/collision_201_meshsets.inc"
};
static NJS_POINT3 collision_201_points[16] = {
#include "stg10/terrain/collision_201_points.inc"
};
static NJS_MODEL collision_201_model = {
#include "stg10/terrain/collision_201_model.inc"
};
static NJS_OBJECT collision_201 = {
#include "stg10/terrain/collision_201.inc"
};
static NJS_MATERIAL collision_202_materials[1] = {
#include "stg10/terrain/collision_202_materials.inc"
};
static Sint16 collision_202_faces[37] = {
#include "stg10/terrain/collision_202_faces.inc"
};
static NJS_MESHSET collision_202_meshsets[1] = {
#include "stg10/terrain/collision_202_meshsets.inc"
};
static NJS_POINT3 collision_202_points[16] = {
#include "stg10/terrain/collision_202_points.inc"
};
static NJS_MODEL collision_202_model = {
#include "stg10/terrain/collision_202_model.inc"
};
static NJS_OBJECT collision_202 = {
#include "stg10/terrain/collision_202.inc"
};
static NJS_MATERIAL collision_203_materials[1] = {
#include "stg10/terrain/collision_203_materials.inc"
};
static Sint16 collision_203_faces[33] = {
#include "stg10/terrain/collision_203_faces.inc"
};
static NJS_MESHSET collision_203_meshsets[1] = {
#include "stg10/terrain/collision_203_meshsets.inc"
};
static NJS_POINT3 collision_203_points[16] = {
#include "stg10/terrain/collision_203_points.inc"
};
static NJS_MODEL collision_203_model = {
#include "stg10/terrain/collision_203_model.inc"
};
static NJS_OBJECT collision_203 = {
#include "stg10/terrain/collision_203.inc"
};
static NJS_MATERIAL collision_204_materials[1] = {
#include "stg10/terrain/collision_204_materials.inc"
};
static Sint16 collision_204_faces[14] = {
#include "stg10/terrain/collision_204_faces.inc"
};
static NJS_MESHSET collision_204_meshsets[1] = {
#include "stg10/terrain/collision_204_meshsets.inc"
};
static NJS_POINT3 collision_204_points[8] = {
#include "stg10/terrain/collision_204_points.inc"
};
static NJS_MODEL collision_204_model = {
#include "stg10/terrain/collision_204_model.inc"
};
static NJS_OBJECT collision_204 = {
#include "stg10/terrain/collision_204.inc"
};
static NJS_MATERIAL collision_205_materials[1] = {
#include "stg10/terrain/collision_205_materials.inc"
};
static Sint16 collision_205_faces[33] = {
#include "stg10/terrain/collision_205_faces.inc"
};
static NJS_MESHSET collision_205_meshsets[1] = {
#include "stg10/terrain/collision_205_meshsets.inc"
};
static NJS_POINT3 collision_205_points[16] = {
#include "stg10/terrain/collision_205_points.inc"
};
static NJS_MODEL collision_205_model = {
#include "stg10/terrain/collision_205_model.inc"
};
static NJS_OBJECT collision_205 = {
#include "stg10/terrain/collision_205.inc"
};
static NJS_MATERIAL collision_206_materials[1] = {
#include "stg10/terrain/collision_206_materials.inc"
};
static Sint16 collision_206_faces[33] = {
#include "stg10/terrain/collision_206_faces.inc"
};
static NJS_MESHSET collision_206_meshsets[1] = {
#include "stg10/terrain/collision_206_meshsets.inc"
};
static NJS_POINT3 collision_206_points[12] = {
#include "stg10/terrain/collision_206_points.inc"
};
static NJS_MODEL collision_206_model = {
#include "stg10/terrain/collision_206_model.inc"
};
static NJS_OBJECT collision_206 = {
#include "stg10/terrain/collision_206.inc"
};
static NJS_MATERIAL collision_208_materials[1] = {
#include "stg10/terrain/collision_208_materials.inc"
};
static Sint16 collision_208_faces[90] = {
#include "stg10/terrain/collision_208_faces.inc"
};
static NJS_MESHSET collision_208_meshsets[1] = {
#include "stg10/terrain/collision_208_meshsets.inc"
};
static NJS_POINT3 collision_208_points[40] = {
#include "stg10/terrain/collision_208_points.inc"
};
static NJS_MODEL collision_208_model = {
#include "stg10/terrain/collision_208_model.inc"
};
static NJS_OBJECT collision_208 = {
#include "stg10/terrain/collision_208.inc"
};
static NJS_MATERIAL collision_210_materials[1] = {
#include "stg10/terrain/collision_210_materials.inc"
};
static Sint16 collision_210_faces[48] = {
#include "stg10/terrain/collision_210_faces.inc"
};
static NJS_MESHSET collision_210_meshsets[1] = {
#include "stg10/terrain/collision_210_meshsets.inc"
};
static NJS_POINT3 collision_210_points[26] = {
#include "stg10/terrain/collision_210_points.inc"
};
static NJS_MODEL collision_210_model = {
#include "stg10/terrain/collision_210_model.inc"
};
static NJS_OBJECT collision_210 = {
#include "stg10/terrain/collision_210.inc"
};
static NJS_MATERIAL collision_211_materials[1] = {
#include "stg10/terrain/collision_211_materials.inc"
};
static Sint16 collision_211_faces[55] = {
#include "stg10/terrain/collision_211_faces.inc"
};
static NJS_MESHSET collision_211_meshsets[1] = {
#include "stg10/terrain/collision_211_meshsets.inc"
};
static NJS_POINT3 collision_211_points[40] = {
#include "stg10/terrain/collision_211_points.inc"
};
static NJS_MODEL collision_211_model = {
#include "stg10/terrain/collision_211_model.inc"
};
static NJS_OBJECT collision_211 = {
#include "stg10/terrain/collision_211.inc"
};
static NJS_MATERIAL collision_215_materials[1] = {
#include "stg10/terrain/collision_215_materials.inc"
};
static Sint16 collision_215_faces[28] = {
#include "stg10/terrain/collision_215_faces.inc"
};
static NJS_MESHSET collision_215_meshsets[1] = {
#include "stg10/terrain/collision_215_meshsets.inc"
};
static NJS_POINT3 collision_215_points[16] = {
#include "stg10/terrain/collision_215_points.inc"
};
static NJS_MODEL collision_215_model = {
#include "stg10/terrain/collision_215_model.inc"
};
static NJS_OBJECT collision_215 = {
#include "stg10/terrain/collision_215.inc"
};
static NJS_MATERIAL collision_216_materials[1] = {
#include "stg10/terrain/collision_216_materials.inc"
};
static Sint16 collision_216_faces[61] = {
#include "stg10/terrain/collision_216_faces.inc"
};
static NJS_MESHSET collision_216_meshsets[1] = {
#include "stg10/terrain/collision_216_meshsets.inc"
};
static NJS_POINT3 collision_216_points[38] = {
#include "stg10/terrain/collision_216_points.inc"
};
static NJS_MODEL collision_216_model = {
#include "stg10/terrain/collision_216_model.inc"
};
static NJS_OBJECT collision_216 = {
#include "stg10/terrain/collision_216.inc"
};
static NJS_MATERIAL collision_217_materials[1] = {
#include "stg10/terrain/collision_217_materials.inc"
};
static Sint16 collision_217_faces[113] = {
#include "stg10/terrain/collision_217_faces.inc"
};
static NJS_MESHSET collision_217_meshsets[1] = {
#include "stg10/terrain/collision_217_meshsets.inc"
};
static NJS_POINT3 collision_217_points[62] = {
#include "stg10/terrain/collision_217_points.inc"
};
static NJS_MODEL collision_217_model = {
#include "stg10/terrain/collision_217_model.inc"
};
static NJS_OBJECT collision_217 = {
#include "stg10/terrain/collision_217.inc"
};
static NJS_MATERIAL collision_218_materials[1] = {
#include "stg10/terrain/collision_218_materials.inc"
};
static Sint16 collision_218_faces[39] = {
#include "stg10/terrain/collision_218_faces.inc"
};
static NJS_MESHSET collision_218_meshsets[1] = {
#include "stg10/terrain/collision_218_meshsets.inc"
};
static NJS_POINT3 collision_218_points[16] = {
#include "stg10/terrain/collision_218_points.inc"
};
static NJS_MODEL collision_218_model = {
#include "stg10/terrain/collision_218_model.inc"
};
static NJS_OBJECT collision_218 = {
#include "stg10/terrain/collision_218.inc"
};
static NJS_MATERIAL collision_219_materials[1] = {
#include "stg10/terrain/collision_219_materials.inc"
};
static Sint16 collision_219_faces[103] = {
#include "stg10/terrain/collision_219_faces.inc"
};
static NJS_MESHSET collision_219_meshsets[1] = {
#include "stg10/terrain/collision_219_meshsets.inc"
};
static NJS_POINT3 collision_219_points[55] = {
#include "stg10/terrain/collision_219_points.inc"
};
static NJS_MODEL collision_219_model = {
#include "stg10/terrain/collision_219_model.inc"
};
static NJS_OBJECT collision_219 = {
#include "stg10/terrain/collision_219.inc"
};
static NJS_MATERIAL collision_220_materials[1] = {
#include "stg10/terrain/collision_220_materials.inc"
};
static Sint16 collision_220_faces[34] = {
#include "stg10/terrain/collision_220_faces.inc"
};
static NJS_MESHSET collision_220_meshsets[1] = {
#include "stg10/terrain/collision_220_meshsets.inc"
};
static NJS_POINT3 collision_220_points[16] = {
#include "stg10/terrain/collision_220_points.inc"
};
static NJS_MODEL collision_220_model = {
#include "stg10/terrain/collision_220_model.inc"
};
static NJS_OBJECT collision_220 = {
#include "stg10/terrain/collision_220.inc"
};
static NJS_MATERIAL collision_224_materials[1] = {
#include "stg10/terrain/collision_224_materials.inc"
};
static Sint16 collision_224_faces[106] = {
#include "stg10/terrain/collision_224_faces.inc"
};
static NJS_MESHSET collision_224_meshsets[1] = {
#include "stg10/terrain/collision_224_meshsets.inc"
};
static NJS_POINT3 collision_224_points[53] = {
#include "stg10/terrain/collision_224_points.inc"
};
static NJS_MODEL collision_224_model = {
#include "stg10/terrain/collision_224_model.inc"
};
static NJS_OBJECT collision_224 = {
#include "stg10/terrain/collision_224.inc"
};
static NJS_MATERIAL collision_225_materials[1] = {
#include "stg10/terrain/collision_225_materials.inc"
};
static Sint16 collision_225_faces[15] = {
#include "stg10/terrain/collision_225_faces.inc"
};
static NJS_MESHSET collision_225_meshsets[1] = {
#include "stg10/terrain/collision_225_meshsets.inc"
};
static NJS_POINT3 collision_225_points[8] = {
#include "stg10/terrain/collision_225_points.inc"
};
static NJS_MODEL collision_225_model = {
#include "stg10/terrain/collision_225_model.inc"
};
static NJS_OBJECT collision_225 = {
#include "stg10/terrain/collision_225.inc"
};
static NJS_MATERIAL collision_226_materials[1] = {
#include "stg10/terrain/collision_226_materials.inc"
};
static Sint16 collision_226_faces[45] = {
#include "stg10/terrain/collision_226_faces.inc"
};
static NJS_MESHSET collision_226_meshsets[1] = {
#include "stg10/terrain/collision_226_meshsets.inc"
};
static NJS_POINT3 collision_226_points[30] = {
#include "stg10/terrain/collision_226_points.inc"
};
static NJS_MODEL collision_226_model = {
#include "stg10/terrain/collision_226_model.inc"
};
static NJS_OBJECT collision_226 = {
#include "stg10/terrain/collision_226.inc"
};
static NJS_MATERIAL collision_227_materials[1] = {
#include "stg10/terrain/collision_227_materials.inc"
};
static Sint16 collision_227_faces[14] = {
#include "stg10/terrain/collision_227_faces.inc"
};
static NJS_MESHSET collision_227_meshsets[1] = {
#include "stg10/terrain/collision_227_meshsets.inc"
};
static NJS_POINT3 collision_227_points[8] = {
#include "stg10/terrain/collision_227_points.inc"
};
static NJS_MODEL collision_227_model = {
#include "stg10/terrain/collision_227_model.inc"
};
static NJS_OBJECT collision_227 = {
#include "stg10/terrain/collision_227.inc"
};
static NJS_MATERIAL collision_228_materials[1] = {
#include "stg10/terrain/collision_228_materials.inc"
};
static Sint16 collision_228_faces[18] = {
#include "stg10/terrain/collision_228_faces.inc"
};
static NJS_MESHSET collision_228_meshsets[1] = {
#include "stg10/terrain/collision_228_meshsets.inc"
};
static NJS_POINT3 collision_228_points[12] = {
#include "stg10/terrain/collision_228_points.inc"
};
static NJS_MODEL collision_228_model = {
#include "stg10/terrain/collision_228_model.inc"
};
static NJS_OBJECT collision_228 = {
#include "stg10/terrain/collision_228.inc"
};
static NJS_MATERIAL collision_229_materials[1] = {
#include "stg10/terrain/collision_229_materials.inc"
};
static Sint16 collision_229_faces[5] = {
#include "stg10/terrain/collision_229_faces.inc"
};
static NJS_MESHSET collision_229_meshsets[1] = {
#include "stg10/terrain/collision_229_meshsets.inc"
};
static NJS_POINT3 collision_229_points[4] = {
#include "stg10/terrain/collision_229_points.inc"
};
static NJS_MODEL collision_229_model = {
#include "stg10/terrain/collision_229_model.inc"
};
static NJS_OBJECT collision_229 = {
#include "stg10/terrain/collision_229.inc"
};
static NJS_MATERIAL collision_230_materials[1] = {
#include "stg10/terrain/collision_230_materials.inc"
};
static Sint16 collision_230_faces[62] = {
#include "stg10/terrain/collision_230_faces.inc"
};
static NJS_MESHSET collision_230_meshsets[1] = {
#include "stg10/terrain/collision_230_meshsets.inc"
};
static NJS_POINT3 collision_230_points[45] = {
#include "stg10/terrain/collision_230_points.inc"
};
static NJS_MODEL collision_230_model = {
#include "stg10/terrain/collision_230_model.inc"
};
static NJS_OBJECT collision_230 = {
#include "stg10/terrain/collision_230.inc"
};
static NJS_MATERIAL collision_231_materials[1] = {
#include "stg10/terrain/collision_231_materials.inc"
};
static Sint16 collision_231_faces[63] = {
#include "stg10/terrain/collision_231_faces.inc"
};
static NJS_MESHSET collision_231_meshsets[1] = {
#include "stg10/terrain/collision_231_meshsets.inc"
};
static NJS_POINT3 collision_231_points[40] = {
#include "stg10/terrain/collision_231_points.inc"
};
static NJS_MODEL collision_231_model = {
#include "stg10/terrain/collision_231_model.inc"
};
static NJS_OBJECT collision_231 = {
#include "stg10/terrain/collision_231.inc"
};
static NJS_MATERIAL collision_233_materials[1] = {
#include "stg10/terrain/collision_233_materials.inc"
};
static Sint16 collision_233_faces[21] = {
#include "stg10/terrain/collision_233_faces.inc"
};
static NJS_MESHSET collision_233_meshsets[1] = {
#include "stg10/terrain/collision_233_meshsets.inc"
};
static NJS_POINT3 collision_233_points[20] = {
#include "stg10/terrain/collision_233_points.inc"
};
static NJS_MODEL collision_233_model = {
#include "stg10/terrain/collision_233_model.inc"
};
static NJS_OBJECT collision_233 = {
#include "stg10/terrain/collision_233.inc"
};
static NJS_MATERIAL collision_234_materials[1] = {
#include "stg10/terrain/collision_234_materials.inc"
};
static Sint16 collision_234_faces[80] = {
#include "stg10/terrain/collision_234_faces.inc"
};
static NJS_MESHSET collision_234_meshsets[1] = {
#include "stg10/terrain/collision_234_meshsets.inc"
};
static NJS_POINT3 collision_234_points[35] = {
#include "stg10/terrain/collision_234_points.inc"
};
static NJS_MODEL collision_234_model = {
#include "stg10/terrain/collision_234_model.inc"
};
static NJS_OBJECT collision_234 = {
#include "stg10/terrain/collision_234.inc"
};
static NJS_MATERIAL collision_235_materials[1] = {
#include "stg10/terrain/collision_235_materials.inc"
};
static Sint16 collision_235_faces[9] = {
#include "stg10/terrain/collision_235_faces.inc"
};
static NJS_MESHSET collision_235_meshsets[1] = {
#include "stg10/terrain/collision_235_meshsets.inc"
};
static NJS_POINT3 collision_235_points[8] = {
#include "stg10/terrain/collision_235_points.inc"
};
static NJS_MODEL collision_235_model = {
#include "stg10/terrain/collision_235_model.inc"
};
static NJS_OBJECT collision_235 = {
#include "stg10/terrain/collision_235.inc"
};
static NJS_MATERIAL collision_236_materials[1] = {
#include "stg10/terrain/collision_236_materials.inc"
};
static Sint16 collision_236_faces[27] = {
#include "stg10/terrain/collision_236_faces.inc"
};
static NJS_MESHSET collision_236_meshsets[1] = {
#include "stg10/terrain/collision_236_meshsets.inc"
};
static NJS_POINT3 collision_236_points[16] = {
#include "stg10/terrain/collision_236_points.inc"
};
static NJS_MODEL collision_236_model = {
#include "stg10/terrain/collision_236_model.inc"
};
static NJS_OBJECT collision_236 = {
#include "stg10/terrain/collision_236.inc"
};
static NJS_MATERIAL collision_237_materials[1] = {
#include "stg10/terrain/collision_237_materials.inc"
};
static Sint16 collision_237_faces[9] = {
#include "stg10/terrain/collision_237_faces.inc"
};
static NJS_MESHSET collision_237_meshsets[1] = {
#include "stg10/terrain/collision_237_meshsets.inc"
};
static NJS_POINT3 collision_237_points[8] = {
#include "stg10/terrain/collision_237_points.inc"
};
static NJS_MODEL collision_237_model = {
#include "stg10/terrain/collision_237_model.inc"
};
static NJS_OBJECT collision_237 = {
#include "stg10/terrain/collision_237.inc"
};
static NJS_MATERIAL collision_238_materials[1] = {
#include "stg10/terrain/collision_238_materials.inc"
};
static Sint16 collision_238_faces[14] = {
#include "stg10/terrain/collision_238_faces.inc"
};
static NJS_MESHSET collision_238_meshsets[1] = {
#include "stg10/terrain/collision_238_meshsets.inc"
};
static NJS_POINT3 collision_238_points[12] = {
#include "stg10/terrain/collision_238_points.inc"
};
static NJS_MODEL collision_238_model = {
#include "stg10/terrain/collision_238_model.inc"
};
static NJS_OBJECT collision_238 = {
#include "stg10/terrain/collision_238.inc"
};
static NJS_MATERIAL collision_239_materials[1] = {
#include "stg10/terrain/collision_239_materials.inc"
};
static Sint16 collision_239_faces[14] = {
#include "stg10/terrain/collision_239_faces.inc"
};
static NJS_MESHSET collision_239_meshsets[1] = {
#include "stg10/terrain/collision_239_meshsets.inc"
};
static NJS_POINT3 collision_239_points[8] = {
#include "stg10/terrain/collision_239_points.inc"
};
static NJS_MODEL collision_239_model = {
#include "stg10/terrain/collision_239_model.inc"
};
static NJS_OBJECT collision_239 = {
#include "stg10/terrain/collision_239.inc"
};
static NJS_MATERIAL collision_240_materials[1] = {
#include "stg10/terrain/collision_240_materials.inc"
};
static Sint16 collision_240_faces[14] = {
#include "stg10/terrain/collision_240_faces.inc"
};
static NJS_MESHSET collision_240_meshsets[1] = {
#include "stg10/terrain/collision_240_meshsets.inc"
};
static NJS_POINT3 collision_240_points[8] = {
#include "stg10/terrain/collision_240_points.inc"
};
static NJS_MODEL collision_240_model = {
#include "stg10/terrain/collision_240_model.inc"
};
static NJS_OBJECT collision_240 = {
#include "stg10/terrain/collision_240.inc"
};
static NJS_MATERIAL collision_241_materials[1] = {
#include "stg10/terrain/collision_241_materials.inc"
};
static Sint16 collision_241_faces[14] = {
#include "stg10/terrain/collision_241_faces.inc"
};
static NJS_MESHSET collision_241_meshsets[1] = {
#include "stg10/terrain/collision_241_meshsets.inc"
};
static NJS_POINT3 collision_241_points[8] = {
#include "stg10/terrain/collision_241_points.inc"
};
static NJS_MODEL collision_241_model = {
#include "stg10/terrain/collision_241_model.inc"
};
static NJS_OBJECT collision_241 = {
#include "stg10/terrain/collision_241.inc"
};
static NJS_MATERIAL collision_242_materials[1] = {
#include "stg10/terrain/collision_242_materials.inc"
};
static Sint16 collision_242_faces[14] = {
#include "stg10/terrain/collision_242_faces.inc"
};
static NJS_MESHSET collision_242_meshsets[1] = {
#include "stg10/terrain/collision_242_meshsets.inc"
};
static NJS_POINT3 collision_242_points[8] = {
#include "stg10/terrain/collision_242_points.inc"
};
static NJS_MODEL collision_242_model = {
#include "stg10/terrain/collision_242_model.inc"
};
static NJS_OBJECT collision_242 = {
#include "stg10/terrain/collision_242.inc"
};
static NJS_MATERIAL collision_243_materials[1] = {
#include "stg10/terrain/collision_243_materials.inc"
};
static Sint16 collision_243_faces[14] = {
#include "stg10/terrain/collision_243_faces.inc"
};
static NJS_MESHSET collision_243_meshsets[1] = {
#include "stg10/terrain/collision_243_meshsets.inc"
};
static NJS_POINT3 collision_243_points[8] = {
#include "stg10/terrain/collision_243_points.inc"
};
static NJS_MODEL collision_243_model = {
#include "stg10/terrain/collision_243_model.inc"
};
static NJS_OBJECT collision_243 = {
#include "stg10/terrain/collision_243.inc"
};
static NJS_MATERIAL collision_244_materials[1] = {
#include "stg10/terrain/collision_244_materials.inc"
};
static Sint16 collision_244_faces[14] = {
#include "stg10/terrain/collision_244_faces.inc"
};
static NJS_MESHSET collision_244_meshsets[1] = {
#include "stg10/terrain/collision_244_meshsets.inc"
};
static NJS_POINT3 collision_244_points[8] = {
#include "stg10/terrain/collision_244_points.inc"
};
static NJS_MODEL collision_244_model = {
#include "stg10/terrain/collision_244_model.inc"
};
static NJS_OBJECT collision_244 = {
#include "stg10/terrain/collision_244.inc"
};
static NJS_MATERIAL collision_245_materials[1] = {
#include "stg10/terrain/collision_245_materials.inc"
};
static Sint16 collision_245_faces[45] = {
#include "stg10/terrain/collision_245_faces.inc"
};
static NJS_MESHSET collision_245_meshsets[1] = {
#include "stg10/terrain/collision_245_meshsets.inc"
};
static NJS_POINT3 collision_245_points[32] = {
#include "stg10/terrain/collision_245_points.inc"
};
static NJS_MODEL collision_245_model = {
#include "stg10/terrain/collision_245_model.inc"
};
static NJS_OBJECT collision_245 = {
#include "stg10/terrain/collision_245.inc"
};
static NJS_MATERIAL collision_247_materials[1] = {
#include "stg10/terrain/collision_247_materials.inc"
};
static Sint16 collision_247_faces[81] = {
#include "stg10/terrain/collision_247_faces.inc"
};
static NJS_MESHSET collision_247_meshsets[1] = {
#include "stg10/terrain/collision_247_meshsets.inc"
};
static NJS_POINT3 collision_247_points[52] = {
#include "stg10/terrain/collision_247_points.inc"
};
static NJS_MODEL collision_247_model = {
#include "stg10/terrain/collision_247_model.inc"
};
static NJS_OBJECT collision_247 = {
#include "stg10/terrain/collision_247.inc"
};
static NJS_MATERIAL collision_248_materials[1] = {
#include "stg10/terrain/collision_248_materials.inc"
};
static Sint16 collision_248_faces[81] = {
#include "stg10/terrain/collision_248_faces.inc"
};
static NJS_MESHSET collision_248_meshsets[1] = {
#include "stg10/terrain/collision_248_meshsets.inc"
};
static NJS_POINT3 collision_248_points[52] = {
#include "stg10/terrain/collision_248_points.inc"
};
static NJS_MODEL collision_248_model = {
#include "stg10/terrain/collision_248_model.inc"
};
static NJS_OBJECT collision_248 = {
#include "stg10/terrain/collision_248.inc"
};
static NJS_MATERIAL collision_249_materials[1] = {
#include "stg10/terrain/collision_249_materials.inc"
};
static Sint16 collision_249_faces[81] = {
#include "stg10/terrain/collision_249_faces.inc"
};
static NJS_MESHSET collision_249_meshsets[1] = {
#include "stg10/terrain/collision_249_meshsets.inc"
};
static NJS_POINT3 collision_249_points[52] = {
#include "stg10/terrain/collision_249_points.inc"
};
static NJS_MODEL collision_249_model = {
#include "stg10/terrain/collision_249_model.inc"
};
static NJS_OBJECT collision_249 = {
#include "stg10/terrain/collision_249.inc"
};
static NJS_MATERIAL collision_250_materials[1] = {
#include "stg10/terrain/collision_250_materials.inc"
};
static Sint16 collision_250_faces[64] = {
#include "stg10/terrain/collision_250_faces.inc"
};
static NJS_MESHSET collision_250_meshsets[1] = {
#include "stg10/terrain/collision_250_meshsets.inc"
};
static NJS_POINT3 collision_250_points[24] = {
#include "stg10/terrain/collision_250_points.inc"
};
static NJS_MODEL collision_250_model = {
#include "stg10/terrain/collision_250_model.inc"
};
static NJS_OBJECT collision_250 = {
#include "stg10/terrain/collision_250.inc"
};
static NJS_MATERIAL collision_253_materials[1] = {
#include "stg10/terrain/collision_253_materials.inc"
};
static Sint16 collision_253_faces[27] = {
#include "stg10/terrain/collision_253_faces.inc"
};
static NJS_MESHSET collision_253_meshsets[1] = {
#include "stg10/terrain/collision_253_meshsets.inc"
};
static NJS_POINT3 collision_253_points[16] = {
#include "stg10/terrain/collision_253_points.inc"
};
static NJS_MODEL collision_253_model = {
#include "stg10/terrain/collision_253_model.inc"
};
static NJS_OBJECT collision_253 = {
#include "stg10/terrain/collision_253.inc"
};
static NJS_MATERIAL collision_254_materials[1] = {
#include "stg10/terrain/collision_254_materials.inc"
};
static Sint16 collision_254_faces[81] = {
#include "stg10/terrain/collision_254_faces.inc"
};
static NJS_MESHSET collision_254_meshsets[1] = {
#include "stg10/terrain/collision_254_meshsets.inc"
};
static NJS_POINT3 collision_254_points[52] = {
#include "stg10/terrain/collision_254_points.inc"
};
static NJS_MODEL collision_254_model = {
#include "stg10/terrain/collision_254_model.inc"
};
static NJS_OBJECT collision_254 = {
#include "stg10/terrain/collision_254.inc"
};
static NJS_MATERIAL collision_255_materials[1] = {
#include "stg10/terrain/collision_255_materials.inc"
};
static Sint16 collision_255_faces[55] = {
#include "stg10/terrain/collision_255_faces.inc"
};
static NJS_MESHSET collision_255_meshsets[1] = {
#include "stg10/terrain/collision_255_meshsets.inc"
};
static NJS_POINT3 collision_255_points[40] = {
#include "stg10/terrain/collision_255_points.inc"
};
static NJS_MODEL collision_255_model = {
#include "stg10/terrain/collision_255_model.inc"
};
static NJS_OBJECT collision_255 = {
#include "stg10/terrain/collision_255.inc"
};
static NJS_MATERIAL collision_261_materials[1] = {
#include "stg10/terrain/collision_261_materials.inc"
};
static Sint16 collision_261_faces[5] = {
#include "stg10/terrain/collision_261_faces.inc"
};
static NJS_MESHSET collision_261_meshsets[1] = {
#include "stg10/terrain/collision_261_meshsets.inc"
};
static NJS_POINT3 collision_261_points[4] = {
#include "stg10/terrain/collision_261_points.inc"
};
static NJS_MODEL collision_261_model = {
#include "stg10/terrain/collision_261_model.inc"
};
static NJS_OBJECT collision_261 = {
#include "stg10/terrain/collision_261.inc"
};
static NJS_MATERIAL collision_262_materials[1] = {
#include "stg10/terrain/collision_262_materials.inc"
};
static Sint16 collision_262_faces[45] = {
#include "stg10/terrain/collision_262_faces.inc"
};
static NJS_MESHSET collision_262_meshsets[1] = {
#include "stg10/terrain/collision_262_meshsets.inc"
};
static NJS_POINT3 collision_262_points[24] = {
#include "stg10/terrain/collision_262_points.inc"
};
static NJS_MODEL collision_262_model = {
#include "stg10/terrain/collision_262_model.inc"
};
static NJS_OBJECT collision_262 = {
#include "stg10/terrain/collision_262.inc"
};
static NJS_MATERIAL collision_263_materials[1] = {
#include "stg10/terrain/collision_263_materials.inc"
};
static Sint16 collision_263_faces[18] = {
#include "stg10/terrain/collision_263_faces.inc"
};
static NJS_MESHSET collision_263_meshsets[1] = {
#include "stg10/terrain/collision_263_meshsets.inc"
};
static NJS_POINT3 collision_263_points[8] = {
#include "stg10/terrain/collision_263_points.inc"
};
static NJS_MODEL collision_263_model = {
#include "stg10/terrain/collision_263_model.inc"
};
static NJS_OBJECT collision_263 = {
#include "stg10/terrain/collision_263.inc"
};
static NJS_MATERIAL collision_264_materials[1] = {
#include "stg10/terrain/collision_264_materials.inc"
};
static Sint16 collision_264_faces[205] = {
#include "stg10/terrain/collision_264_faces.inc"
};
static NJS_MESHSET collision_264_meshsets[1] = {
#include "stg10/terrain/collision_264_meshsets.inc"
};
static NJS_POINT3 collision_264_points[89] = {
#include "stg10/terrain/collision_264_points.inc"
};
static NJS_MODEL collision_264_model = {
#include "stg10/terrain/collision_264_model.inc"
};
static NJS_OBJECT collision_264 = {
#include "stg10/terrain/collision_264.inc"
};
static NJS_MATERIAL collision_266_materials[1] = {
#include "stg10/terrain/collision_266_materials.inc"
};
static Sint16 collision_266_faces[17] = {
#include "stg10/terrain/collision_266_faces.inc"
};
static NJS_MESHSET collision_266_meshsets[1] = {
#include "stg10/terrain/collision_266_meshsets.inc"
};
static NJS_POINT3 collision_266_points[8] = {
#include "stg10/terrain/collision_266_points.inc"
};
static NJS_MODEL collision_266_model = {
#include "stg10/terrain/collision_266_model.inc"
};
static NJS_OBJECT collision_266 = {
#include "stg10/terrain/collision_266.inc"
};
static NJS_MATERIAL collision_268_materials[1] = {
#include "stg10/terrain/collision_268_materials.inc"
};
static Sint16 collision_268_faces[25] = {
#include "stg10/terrain/collision_268_faces.inc"
};
static NJS_MESHSET collision_268_meshsets[1] = {
#include "stg10/terrain/collision_268_meshsets.inc"
};
static NJS_POINT3 collision_268_points[15] = {
#include "stg10/terrain/collision_268_points.inc"
};
static NJS_MODEL collision_268_model = {
#include "stg10/terrain/collision_268_model.inc"
};
static NJS_OBJECT collision_268 = {
#include "stg10/terrain/collision_268.inc"
};
static NJS_MATERIAL collision_269_materials[1] = {
#include "stg10/terrain/collision_269_materials.inc"
};
static Sint16 collision_269_faces[60] = {
#include "stg10/terrain/collision_269_faces.inc"
};
static NJS_MESHSET collision_269_meshsets[1] = {
#include "stg10/terrain/collision_269_meshsets.inc"
};
static NJS_POINT3 collision_269_points[28] = {
#include "stg10/terrain/collision_269_points.inc"
};
static NJS_MODEL collision_269_model = {
#include "stg10/terrain/collision_269_model.inc"
};
static NJS_OBJECT collision_269 = {
#include "stg10/terrain/collision_269.inc"
};
static NJS_MATERIAL collision_271_materials[1] = {
#include "stg10/terrain/collision_271_materials.inc"
};
static Sint16 collision_271_faces[42] = {
#include "stg10/terrain/collision_271_faces.inc"
};
static NJS_MESHSET collision_271_meshsets[1] = {
#include "stg10/terrain/collision_271_meshsets.inc"
};
static NJS_POINT3 collision_271_points[20] = {
#include "stg10/terrain/collision_271_points.inc"
};
static NJS_MODEL collision_271_model = {
#include "stg10/terrain/collision_271_model.inc"
};
static NJS_OBJECT collision_271 = {
#include "stg10/terrain/collision_271.inc"
};
static NJS_MATERIAL collision_272_materials[1] = {
#include "stg10/terrain/collision_272_materials.inc"
};
static Sint16 collision_272_faces[66] = {
#include "stg10/terrain/collision_272_faces.inc"
};
static NJS_MESHSET collision_272_meshsets[1] = {
#include "stg10/terrain/collision_272_meshsets.inc"
};
static NJS_POINT3 collision_272_points[28] = {
#include "stg10/terrain/collision_272_points.inc"
};
static NJS_MODEL collision_272_model = {
#include "stg10/terrain/collision_272_model.inc"
};
static NJS_OBJECT collision_272 = {
#include "stg10/terrain/collision_272.inc"
};
static NJS_MATERIAL collision_273_materials[1] = {
#include "stg10/terrain/collision_273_materials.inc"
};
static Sint16 collision_273_faces[140] = {
#include "stg10/terrain/collision_273_faces.inc"
};
static NJS_MESHSET collision_273_meshsets[1] = {
#include "stg10/terrain/collision_273_meshsets.inc"
};
static NJS_POINT3 collision_273_points[80] = {
#include "stg10/terrain/collision_273_points.inc"
};
static NJS_MODEL collision_273_model = {
#include "stg10/terrain/collision_273_model.inc"
};
static NJS_OBJECT collision_273 = {
#include "stg10/terrain/collision_273.inc"
};
static NJS_MATERIAL collision_276_materials[1] = {
#include "stg10/terrain/collision_276_materials.inc"
};
static Sint16 collision_276_faces[49] = {
#include "stg10/terrain/collision_276_faces.inc"
};
static NJS_MESHSET collision_276_meshsets[1] = {
#include "stg10/terrain/collision_276_meshsets.inc"
};
static NJS_POINT3 collision_276_points[16] = {
#include "stg10/terrain/collision_276_points.inc"
};
static NJS_MODEL collision_276_model = {
#include "stg10/terrain/collision_276_model.inc"
};
static NJS_OBJECT collision_276 = {
#include "stg10/terrain/collision_276.inc"
};
static NJS_MATERIAL collision_288_materials[1] = {
#include "stg10/terrain/collision_288_materials.inc"
};
static Sint16 collision_288_faces[36] = {
#include "stg10/terrain/collision_288_faces.inc"
};
static NJS_MESHSET collision_288_meshsets[1] = {
#include "stg10/terrain/collision_288_meshsets.inc"
};
static NJS_POINT3 collision_288_points[17] = {
#include "stg10/terrain/collision_288_points.inc"
};
static NJS_MODEL collision_288_model = {
#include "stg10/terrain/collision_288_model.inc"
};
static NJS_OBJECT collision_288 = {
#include "stg10/terrain/collision_288.inc"
};
static NJS_MATERIAL collision_289_materials[1] = {
#include "stg10/terrain/collision_289_materials.inc"
};
static Sint16 collision_289_faces[22] = {
#include "stg10/terrain/collision_289_faces.inc"
};
static NJS_MESHSET collision_289_meshsets[1] = {
#include "stg10/terrain/collision_289_meshsets.inc"
};
static NJS_POINT3 collision_289_points[16] = {
#include "stg10/terrain/collision_289_points.inc"
};
static NJS_MODEL collision_289_model = {
#include "stg10/terrain/collision_289_model.inc"
};
static NJS_OBJECT collision_289 = {
#include "stg10/terrain/collision_289.inc"
};
static NJS_MATERIAL collision_295_materials[1] = {
#include "stg10/terrain/collision_295_materials.inc"
};
static Sint16 collision_295_faces[47] = {
#include "stg10/terrain/collision_295_faces.inc"
};
static NJS_MESHSET collision_295_meshsets[1] = {
#include "stg10/terrain/collision_295_meshsets.inc"
};
static NJS_POINT3 collision_295_points[16] = {
#include "stg10/terrain/collision_295_points.inc"
};
static NJS_MODEL collision_295_model = {
#include "stg10/terrain/collision_295_model.inc"
};
static NJS_OBJECT collision_295 = {
#include "stg10/terrain/collision_295.inc"
};
static NJS_MATERIAL collision_297_materials[1] = {
#include "stg10/terrain/collision_297_materials.inc"
};
static Sint16 collision_297_faces[11] = {
#include "stg10/terrain/collision_297_faces.inc"
};
static NJS_MESHSET collision_297_meshsets[1] = {
#include "stg10/terrain/collision_297_meshsets.inc"
};
static NJS_POINT3 collision_297_points[8] = {
#include "stg10/terrain/collision_297_points.inc"
};
static NJS_MODEL collision_297_model = {
#include "stg10/terrain/collision_297_model.inc"
};
static NJS_OBJECT collision_297 = {
#include "stg10/terrain/collision_297.inc"
};
static NJS_MATERIAL collision_299_materials[1] = {
#include "stg10/terrain/collision_299_materials.inc"
};
static Sint16 collision_299_faces[81] = {
#include "stg10/terrain/collision_299_faces.inc"
};
static NJS_MESHSET collision_299_meshsets[1] = {
#include "stg10/terrain/collision_299_meshsets.inc"
};
static NJS_POINT3 collision_299_points[52] = {
#include "stg10/terrain/collision_299_points.inc"
};
static NJS_MODEL collision_299_model = {
#include "stg10/terrain/collision_299_model.inc"
};
static NJS_OBJECT collision_299 = {
#include "stg10/terrain/collision_299.inc"
};
static NJS_MATERIAL collision_300_materials[1] = {
#include "stg10/terrain/collision_300_materials.inc"
};
static Sint16 collision_300_faces[81] = {
#include "stg10/terrain/collision_300_faces.inc"
};
static NJS_MESHSET collision_300_meshsets[1] = {
#include "stg10/terrain/collision_300_meshsets.inc"
};
static NJS_POINT3 collision_300_points[52] = {
#include "stg10/terrain/collision_300_points.inc"
};
static NJS_MODEL collision_300_model = {
#include "stg10/terrain/collision_300_model.inc"
};
static NJS_OBJECT collision_300 = {
#include "stg10/terrain/collision_300.inc"
};
static NJS_MATERIAL collision_301_materials[1] = {
#include "stg10/terrain/collision_301_materials.inc"
};
static Sint16 collision_301_faces[81] = {
#include "stg10/terrain/collision_301_faces.inc"
};
static NJS_MESHSET collision_301_meshsets[1] = {
#include "stg10/terrain/collision_301_meshsets.inc"
};
static NJS_POINT3 collision_301_points[52] = {
#include "stg10/terrain/collision_301_points.inc"
};
static NJS_MODEL collision_301_model = {
#include "stg10/terrain/collision_301_model.inc"
};
static NJS_OBJECT collision_301 = {
#include "stg10/terrain/collision_301.inc"
};
static NJS_MATERIAL collision_302_materials[1] = {
#include "stg10/terrain/collision_302_materials.inc"
};
static Sint16 collision_302_faces[81] = {
#include "stg10/terrain/collision_302_faces.inc"
};
static NJS_MESHSET collision_302_meshsets[1] = {
#include "stg10/terrain/collision_302_meshsets.inc"
};
static NJS_POINT3 collision_302_points[52] = {
#include "stg10/terrain/collision_302_points.inc"
};
static NJS_MODEL collision_302_model = {
#include "stg10/terrain/collision_302_model.inc"
};
static NJS_OBJECT collision_302 = {
#include "stg10/terrain/collision_302.inc"
};
static NJS_MATERIAL collision_303_materials[1] = {
#include "stg10/terrain/collision_303_materials.inc"
};
static Sint16 collision_303_faces[81] = {
#include "stg10/terrain/collision_303_faces.inc"
};
static NJS_MESHSET collision_303_meshsets[1] = {
#include "stg10/terrain/collision_303_meshsets.inc"
};
static NJS_POINT3 collision_303_points[52] = {
#include "stg10/terrain/collision_303_points.inc"
};
static NJS_MODEL collision_303_model = {
#include "stg10/terrain/collision_303_model.inc"
};
static NJS_OBJECT collision_303 = {
#include "stg10/terrain/collision_303.inc"
};
static NJS_MATERIAL collision_304_materials[1] = {
#include "stg10/terrain/collision_304_materials.inc"
};
static Sint16 collision_304_faces[81] = {
#include "stg10/terrain/collision_304_faces.inc"
};
static NJS_MESHSET collision_304_meshsets[1] = {
#include "stg10/terrain/collision_304_meshsets.inc"
};
static NJS_POINT3 collision_304_points[52] = {
#include "stg10/terrain/collision_304_points.inc"
};
static NJS_MODEL collision_304_model = {
#include "stg10/terrain/collision_304_model.inc"
};
static NJS_OBJECT collision_304 = {
#include "stg10/terrain/collision_304.inc"
};
static NJS_MATERIAL collision_305_materials[1] = {
#include "stg10/terrain/collision_305_materials.inc"
};
static Sint16 collision_305_faces[81] = {
#include "stg10/terrain/collision_305_faces.inc"
};
static NJS_MESHSET collision_305_meshsets[1] = {
#include "stg10/terrain/collision_305_meshsets.inc"
};
static NJS_POINT3 collision_305_points[52] = {
#include "stg10/terrain/collision_305_points.inc"
};
static NJS_MODEL collision_305_model = {
#include "stg10/terrain/collision_305_model.inc"
};
static NJS_OBJECT collision_305 = {
#include "stg10/terrain/collision_305.inc"
};
static NJS_MATERIAL collision_306_materials[1] = {
#include "stg10/terrain/collision_306_materials.inc"
};
static Sint16 collision_306_faces[81] = {
#include "stg10/terrain/collision_306_faces.inc"
};
static NJS_MESHSET collision_306_meshsets[1] = {
#include "stg10/terrain/collision_306_meshsets.inc"
};
static NJS_POINT3 collision_306_points[52] = {
#include "stg10/terrain/collision_306_points.inc"
};
static NJS_MODEL collision_306_model = {
#include "stg10/terrain/collision_306_model.inc"
};
static NJS_OBJECT collision_306 = {
#include "stg10/terrain/collision_306.inc"
};
static NJS_MATERIAL collision_307_materials[1] = {
#include "stg10/terrain/collision_307_materials.inc"
};
static Sint16 collision_307_faces[9] = {
#include "stg10/terrain/collision_307_faces.inc"
};
static NJS_MESHSET collision_307_meshsets[1] = {
#include "stg10/terrain/collision_307_meshsets.inc"
};
static NJS_POINT3 collision_307_points[8] = {
#include "stg10/terrain/collision_307_points.inc"
};
static NJS_MODEL collision_307_model = {
#include "stg10/terrain/collision_307_model.inc"
};
static NJS_OBJECT collision_307 = {
#include "stg10/terrain/collision_307.inc"
};
static NJS_MATERIAL collision_308_materials[1] = {
#include "stg10/terrain/collision_308_materials.inc"
};
static Sint16 collision_308_faces[9] = {
#include "stg10/terrain/collision_308_faces.inc"
};
static NJS_MESHSET collision_308_meshsets[1] = {
#include "stg10/terrain/collision_308_meshsets.inc"
};
static NJS_POINT3 collision_308_points[8] = {
#include "stg10/terrain/collision_308_points.inc"
};
static NJS_MODEL collision_308_model = {
#include "stg10/terrain/collision_308_model.inc"
};
static NJS_OBJECT collision_308 = {
#include "stg10/terrain/collision_308.inc"
};
static NJS_MATERIAL collision_309_materials[1] = {
#include "stg10/terrain/collision_309_materials.inc"
};
static Sint16 collision_309_faces[42] = {
#include "stg10/terrain/collision_309_faces.inc"
};
static NJS_MESHSET collision_309_meshsets[1] = {
#include "stg10/terrain/collision_309_meshsets.inc"
};
static NJS_POINT3 collision_309_points[17] = {
#include "stg10/terrain/collision_309_points.inc"
};
static NJS_MODEL collision_309_model = {
#include "stg10/terrain/collision_309_model.inc"
};
static NJS_OBJECT collision_309 = {
#include "stg10/terrain/collision_309.inc"
};
static NJS_MATERIAL collision_311_materials[1] = {
#include "stg10/terrain/collision_311_materials.inc"
};
static Sint16 collision_311_faces[14] = {
#include "stg10/terrain/collision_311_faces.inc"
};
static NJS_MESHSET collision_311_meshsets[1] = {
#include "stg10/terrain/collision_311_meshsets.inc"
};
static NJS_POINT3 collision_311_points[8] = {
#include "stg10/terrain/collision_311_points.inc"
};
static NJS_MODEL collision_311_model = {
#include "stg10/terrain/collision_311_model.inc"
};
static NJS_OBJECT collision_311 = {
#include "stg10/terrain/collision_311.inc"
};
static NJS_CNK_OBJECT draw_444 = {
#include "stg10/terrain/draw_444.inc"
};
static NJS_CNK_OBJECT draw_443 = {
#include "stg10/terrain/draw_443.inc"
};
static NJS_CNK_OBJECT draw_442 = {
#include "stg10/terrain/draw_442.inc"
};
static NJS_CNK_OBJECT draw_441 = {
#include "stg10/terrain/draw_441.inc"
};
static NJS_CNK_OBJECT draw_440 = {
#include "stg10/terrain/draw_440.inc"
};
static NJS_CNK_OBJECT draw_439 = {
#include "stg10/terrain/draw_439.inc"
};
static NJS_CNK_OBJECT draw_438 = {
#include "stg10/terrain/draw_438.inc"
};
static NJS_CNK_OBJECT draw_437 = {
#include "stg10/terrain/draw_437.inc"
};
static NJS_CNK_OBJECT draw_436 = {
#include "stg10/terrain/draw_436.inc"
};
static NJS_CNK_OBJECT draw_435 = {
#include "stg10/terrain/draw_435.inc"
};
static NJS_CNK_OBJECT draw_434 = {
#include "stg10/terrain/draw_434.inc"
};
static NJS_CNK_OBJECT draw_433 = {
#include "stg10/terrain/draw_433.inc"
};
static NJS_CNK_OBJECT draw_432 = {
#include "stg10/terrain/draw_432.inc"
};
static NJS_CNK_OBJECT draw_431 = {
#include "stg10/terrain/draw_431.inc"
};
static NJS_CNK_OBJECT draw_430 = {
#include "stg10/terrain/draw_430.inc"
};
static NJS_CNK_OBJECT draw_429 = {
#include "stg10/terrain/draw_429.inc"
};
static NJS_CNK_OBJECT draw_428 = {
#include "stg10/terrain/draw_428.inc"
};
static NJS_CNK_OBJECT draw_427 = {
#include "stg10/terrain/draw_427.inc"
};
static NJS_CNK_OBJECT draw_426 = {
#include "stg10/terrain/draw_426.inc"
};
static NJS_CNK_OBJECT draw_425 = {
#include "stg10/terrain/draw_425.inc"
};
static NJS_CNK_OBJECT draw_424 = {
#include "stg10/terrain/draw_424.inc"
};
static NJS_CNK_OBJECT draw_423 = {
#include "stg10/terrain/draw_423.inc"
};
static NJS_CNK_OBJECT draw_422 = {
#include "stg10/terrain/draw_422.inc"
};
static NJS_CNK_OBJECT draw_421 = {
#include "stg10/terrain/draw_421.inc"
};
static NJS_CNK_OBJECT draw_420 = {
#include "stg10/terrain/draw_420.inc"
};
static NJS_CNK_OBJECT draw_418 = {
#include "stg10/terrain/draw_418.inc"
};
static NJS_CNK_OBJECT draw_417 = {
#include "stg10/terrain/draw_417.inc"
};
static NJS_CNK_OBJECT draw_415 = {
#include "stg10/terrain/draw_415.inc"
};
static NJS_CNK_OBJECT draw_414 = {
#include "stg10/terrain/draw_414.inc"
};
static NJS_CNK_OBJECT draw_410 = {
#include "stg10/terrain/draw_410.inc"
};
static NJS_CNK_OBJECT draw_409 = {
#include "stg10/terrain/draw_409.inc"
};
static NJS_CNK_OBJECT draw_407 = {
#include "stg10/terrain/draw_407.inc"
};
static NJS_CNK_OBJECT draw_406 = {
#include "stg10/terrain/draw_406.inc"
};
static NJS_CNK_OBJECT draw_405 = {
#include "stg10/terrain/draw_405.inc"
};
static NJS_CNK_OBJECT draw_404 = {
#include "stg10/terrain/draw_404.inc"
};
static NJS_CNK_OBJECT draw_403 = {
#include "stg10/terrain/draw_403.inc"
};
static NJS_CNK_OBJECT draw_402 = {
#include "stg10/terrain/draw_402.inc"
};
static NJS_CNK_OBJECT draw_401 = {
#include "stg10/terrain/draw_401.inc"
};
static NJS_CNK_OBJECT draw_400 = {
#include "stg10/terrain/draw_400.inc"
};
static NJS_CNK_OBJECT draw_399 = {
#include "stg10/terrain/draw_399.inc"
};
static NJS_CNK_OBJECT draw_398 = {
#include "stg10/terrain/draw_398.inc"
};
static NJS_CNK_OBJECT draw_397 = {
#include "stg10/terrain/draw_397.inc"
};
static NJS_CNK_OBJECT draw_396 = {
#include "stg10/terrain/draw_396.inc"
};
static NJS_CNK_OBJECT draw_395 = {
#include "stg10/terrain/draw_395.inc"
};
static NJS_CNK_OBJECT draw_394 = {
#include "stg10/terrain/draw_394.inc"
};
static NJS_CNK_OBJECT draw_393 = {
#include "stg10/terrain/draw_393.inc"
};
static NJS_CNK_OBJECT draw_392 = {
#include "stg10/terrain/draw_392.inc"
};
static NJS_CNK_OBJECT draw_391 = {
#include "stg10/terrain/draw_391.inc"
};
static NJS_CNK_OBJECT draw_390 = {
#include "stg10/terrain/draw_390.inc"
};
static NJS_CNK_OBJECT draw_389 = {
#include "stg10/terrain/draw_389.inc"
};
static NJS_CNK_OBJECT draw_388 = {
#include "stg10/terrain/draw_388.inc"
};
static NJS_CNK_OBJECT draw_387 = {
#include "stg10/terrain/draw_387.inc"
};
static NJS_CNK_OBJECT draw_386 = {
#include "stg10/terrain/draw_386.inc"
};
static NJS_CNK_OBJECT draw_385 = {
#include "stg10/terrain/draw_385.inc"
};
static NJS_CNK_OBJECT draw_384 = {
#include "stg10/terrain/draw_384.inc"
};
static NJS_CNK_OBJECT draw_383 = {
#include "stg10/terrain/draw_383.inc"
};
static NJS_CNK_OBJECT draw_382 = {
#include "stg10/terrain/draw_382.inc"
};
static NJS_CNK_OBJECT draw_381 = {
#include "stg10/terrain/draw_381.inc"
};
static NJS_CNK_OBJECT draw_380 = {
#include "stg10/terrain/draw_380.inc"
};
static NJS_CNK_OBJECT draw_379 = {
#include "stg10/terrain/draw_379.inc"
};
static NJS_CNK_OBJECT draw_378 = {
#include "stg10/terrain/draw_378.inc"
};
static NJS_CNK_OBJECT draw_377 = {
#include "stg10/terrain/draw_377.inc"
};
static NJS_CNK_OBJECT draw_376 = {
#include "stg10/terrain/draw_376.inc"
};
static NJS_CNK_OBJECT draw_374 = {
#include "stg10/terrain/draw_374.inc"
};
static NJS_CNK_OBJECT draw_373 = {
#include "stg10/terrain/draw_373.inc"
};
static NJS_CNK_OBJECT draw_371 = {
#include "stg10/terrain/draw_371.inc"
};
static NJS_CNK_OBJECT draw_370 = {
#include "stg10/terrain/draw_370.inc"
};
static NJS_CNK_OBJECT draw_369 = {
#include "stg10/terrain/draw_369.inc"
};
static NJS_CNK_OBJECT draw_366 = {
#include "stg10/terrain/draw_366.inc"
};
static NJS_CNK_OBJECT draw_365 = {
#include "stg10/terrain/draw_365.inc"
};
static NJS_CNK_OBJECT draw_364 = {
#include "stg10/terrain/draw_364.inc"
};
static NJS_CNK_OBJECT draw_362 = {
#include "stg10/terrain/draw_362.inc"
};
static NJS_CNK_OBJECT draw_361 = {
#include "stg10/terrain/draw_361.inc"
};
static NJS_CNK_OBJECT draw_359 = {
#include "stg10/terrain/draw_359.inc"
};
static NJS_CNK_OBJECT draw_357 = {
#include "stg10/terrain/draw_357.inc"
};
static NJS_CNK_OBJECT draw_356 = {
#include "stg10/terrain/draw_356.inc"
};
static NJS_CNK_OBJECT draw_355 = {
#include "stg10/terrain/draw_355.inc"
};
static NJS_CNK_OBJECT draw_353 = {
#include "stg10/terrain/draw_353.inc"
};
static NJS_CNK_OBJECT draw_352 = {
#include "stg10/terrain/draw_352.inc"
};
static NJS_CNK_OBJECT draw_348 = {
#include "stg10/terrain/draw_348.inc"
};
static NJS_CNK_OBJECT draw_346 = {
#include "stg10/terrain/draw_346.inc"
};
static NJS_CNK_OBJECT draw_344 = {
#include "stg10/terrain/draw_344.inc"
};
static NJS_CNK_OBJECT draw_343 = {
#include "stg10/terrain/draw_343.inc"
};
static NJS_CNK_OBJECT draw_342 = {
#include "stg10/terrain/draw_342.inc"
};
static NJS_CNK_OBJECT draw_341 = {
#include "stg10/terrain/draw_341.inc"
};
static NJS_CNK_OBJECT draw_340 = {
#include "stg10/terrain/draw_340.inc"
};
static NJS_CNK_OBJECT draw_339 = {
#include "stg10/terrain/draw_339.inc"
};
static NJS_CNK_OBJECT draw_338 = {
#include "stg10/terrain/draw_338.inc"
};
static NJS_CNK_OBJECT draw_335 = {
#include "stg10/terrain/draw_335.inc"
};
static NJS_CNK_OBJECT draw_334 = {
#include "stg10/terrain/draw_334.inc"
};
static NJS_CNK_OBJECT draw_332 = {
#include "stg10/terrain/draw_332.inc"
};
static NJS_CNK_OBJECT draw_331 = {
#include "stg10/terrain/draw_331.inc"
};
static NJS_CNK_OBJECT draw_328 = {
#include "stg10/terrain/draw_328.inc"
};
static NJS_CNK_OBJECT draw_327 = {
#include "stg10/terrain/draw_327.inc"
};
static NJS_CNK_OBJECT draw_325 = {
#include "stg10/terrain/draw_325.inc"
};
static NJS_CNK_OBJECT draw_324 = {
#include "stg10/terrain/draw_324.inc"
};
static NJS_CNK_OBJECT draw_321 = {
#include "stg10/terrain/draw_321.inc"
};
static NJS_CNK_OBJECT draw_319 = {
#include "stg10/terrain/draw_319.inc"
};
static NJS_CNK_OBJECT draw_318 = {
#include "stg10/terrain/draw_318.inc"
};
static NJS_CNK_OBJECT draw_317 = {
#include "stg10/terrain/draw_317.inc"
};
static NJS_CNK_OBJECT draw_316 = {
#include "stg10/terrain/draw_316.inc"
};
static NJS_CNK_OBJECT draw_315 = {
#include "stg10/terrain/draw_315.inc"
};
static NJS_CNK_OBJECT draw_314 = {
#include "stg10/terrain/draw_314.inc"
};
static NJS_CNK_OBJECT draw_313 = {
#include "stg10/terrain/draw_313.inc"
};
static NJS_CNK_OBJECT draw_312 = {
#include "stg10/terrain/draw_312.inc"
};
static NJS_CNK_OBJECT draw_311 = {
#include "stg10/terrain/draw_311.inc"
};
static NJS_CNK_OBJECT draw_310 = {
#include "stg10/terrain/draw_310.inc"
};
static NJS_CNK_OBJECT draw_309 = {
#include "stg10/terrain/draw_309.inc"
};
static NJS_CNK_OBJECT draw_308 = {
#include "stg10/terrain/draw_308.inc"
};
static NJS_CNK_OBJECT draw_307 = {
#include "stg10/terrain/draw_307.inc"
};
static NJS_CNK_OBJECT draw_306 = {
#include "stg10/terrain/draw_306.inc"
};
static NJS_CNK_OBJECT draw_305 = {
#include "stg10/terrain/draw_305.inc"
};
static NJS_CNK_OBJECT draw_304 = {
#include "stg10/terrain/draw_304.inc"
};
static NJS_CNK_OBJECT draw_303 = {
#include "stg10/terrain/draw_303.inc"
};
static NJS_CNK_OBJECT draw_302 = {
#include "stg10/terrain/draw_302.inc"
};
static NJS_CNK_OBJECT draw_300 = {
#include "stg10/terrain/draw_300.inc"
};
static NJS_CNK_OBJECT draw_296 = {
#include "stg10/terrain/draw_296.inc"
};
static NJS_CNK_OBJECT draw_295 = {
#include "stg10/terrain/draw_295.inc"
};
static NJS_CNK_OBJECT draw_293 = {
#include "stg10/terrain/draw_293.inc"
};
static NJS_CNK_OBJECT draw_291 = {
#include "stg10/terrain/draw_291.inc"
};
static NJS_CNK_OBJECT draw_290 = {
#include "stg10/terrain/draw_290.inc"
};
static NJS_CNK_OBJECT draw_289 = {
#include "stg10/terrain/draw_289.inc"
};
static NJS_CNK_OBJECT draw_288 = {
#include "stg10/terrain/draw_288.inc"
};
static NJS_CNK_OBJECT draw_287 = {
#include "stg10/terrain/draw_287.inc"
};
static NJS_CNK_OBJECT draw_281 = {
#include "stg10/terrain/draw_281.inc"
};
static NJS_CNK_OBJECT draw_279 = {
#include "stg10/terrain/draw_279.inc"
};
static NJS_CNK_OBJECT draw_277 = {
#include "stg10/terrain/draw_277.inc"
};
static NJS_CNK_OBJECT draw_276 = {
#include "stg10/terrain/draw_276.inc"
};
static NJS_CNK_OBJECT draw_275 = {
#include "stg10/terrain/draw_275.inc"
};
static NJS_CNK_OBJECT draw_274 = {
#include "stg10/terrain/draw_274.inc"
};
static NJS_CNK_OBJECT draw_273 = {
#include "stg10/terrain/draw_273.inc"
};
static NJS_CNK_OBJECT draw_271 = {
#include "stg10/terrain/draw_271.inc"
};
static NJS_CNK_OBJECT draw_270 = {
#include "stg10/terrain/draw_270.inc"
};
static NJS_CNK_OBJECT draw_268 = {
#include "stg10/terrain/draw_268.inc"
};
static NJS_CNK_OBJECT draw_265 = {
#include "stg10/terrain/draw_265.inc"
};
static NJS_CNK_OBJECT draw_264 = {
#include "stg10/terrain/draw_264.inc"
};
static NJS_CNK_OBJECT draw_263 = {
#include "stg10/terrain/draw_263.inc"
};
static NJS_CNK_OBJECT draw_262 = {
#include "stg10/terrain/draw_262.inc"
};
static NJS_CNK_OBJECT draw_261 = {
#include "stg10/terrain/draw_261.inc"
};
static NJS_CNK_OBJECT draw_260 = {
#include "stg10/terrain/draw_260.inc"
};
static NJS_CNK_OBJECT draw_257 = {
#include "stg10/terrain/draw_257.inc"
};
static NJS_CNK_OBJECT draw_255 = {
#include "stg10/terrain/draw_255.inc"
};
static NJS_CNK_OBJECT draw_254 = {
#include "stg10/terrain/draw_254.inc"
};
static NJS_CNK_OBJECT draw_253 = {
#include "stg10/terrain/draw_253.inc"
};
static NJS_CNK_OBJECT draw_252 = {
#include "stg10/terrain/draw_252.inc"
};
static NJS_CNK_OBJECT draw_251 = {
#include "stg10/terrain/draw_251.inc"
};
static NJS_CNK_OBJECT draw_249 = {
#include "stg10/terrain/draw_249.inc"
};
static NJS_CNK_OBJECT draw_248 = {
#include "stg10/terrain/draw_248.inc"
};
static NJS_CNK_OBJECT draw_243 = {
#include "stg10/terrain/draw_243.inc"
};
static NJS_CNK_OBJECT draw_242 = {
#include "stg10/terrain/draw_242.inc"
};
static NJS_CNK_OBJECT draw_241 = {
#include "stg10/terrain/draw_241.inc"
};
static NJS_CNK_OBJECT draw_237 = {
#include "stg10/terrain/draw_237.inc"
};
static NJS_CNK_OBJECT draw_236 = {
#include "stg10/terrain/draw_236.inc"
};
static NJS_CNK_OBJECT draw_232 = {
#include "stg10/terrain/draw_232.inc"
};
static NJS_CNK_OBJECT draw_227 = {
#include "stg10/terrain/draw_227.inc"
};
static NJS_CNK_OBJECT draw_226 = {
#include "stg10/terrain/draw_226.inc"
};
static NJS_CNK_OBJECT draw_223 = {
#include "stg10/terrain/draw_223.inc"
};
static NJS_CNK_OBJECT draw_221 = {
#include "stg10/terrain/draw_221.inc"
};
static NJS_CNK_OBJECT draw_220 = {
#include "stg10/terrain/draw_220.inc"
};
static NJS_CNK_OBJECT draw_213 = {
#include "stg10/terrain/draw_213.inc"
};
static NJS_CNK_OBJECT draw_212 = {
#include "stg10/terrain/draw_212.inc"
};
static NJS_CNK_OBJECT draw_211 = {
#include "stg10/terrain/draw_211.inc"
};
static NJS_CNK_OBJECT draw_210 = {
#include "stg10/terrain/draw_210.inc"
};
static NJS_CNK_OBJECT draw_209 = {
#include "stg10/terrain/draw_209.inc"
};
static NJS_CNK_OBJECT draw_208 = {
#include "stg10/terrain/draw_208.inc"
};
static NJS_CNK_OBJECT draw_207 = {
#include "stg10/terrain/draw_207.inc"
};
static NJS_CNK_OBJECT draw_206 = {
#include "stg10/terrain/draw_206.inc"
};
static NJS_CNK_OBJECT draw_205 = {
#include "stg10/terrain/draw_205.inc"
};
static NJS_CNK_OBJECT draw_199 = {
#include "stg10/terrain/draw_199.inc"
};
static NJS_CNK_OBJECT draw_196 = {
#include "stg10/terrain/draw_196.inc"
};
static NJS_CNK_OBJECT draw_195 = {
#include "stg10/terrain/draw_195.inc"
};
static NJS_CNK_OBJECT draw_193 = {
#include "stg10/terrain/draw_193.inc"
};
static NJS_CNK_OBJECT draw_191 = {
#include "stg10/terrain/draw_191.inc"
};
static NJS_CNK_OBJECT draw_190 = {
#include "stg10/terrain/draw_190.inc"
};
static NJS_CNK_OBJECT draw_189 = {
#include "stg10/terrain/draw_189.inc"
};
static NJS_CNK_OBJECT draw_188 = {
#include "stg10/terrain/draw_188.inc"
};
static NJS_CNK_OBJECT draw_187 = {
#include "stg10/terrain/draw_187.inc"
};
static NJS_CNK_OBJECT draw_186 = {
#include "stg10/terrain/draw_186.inc"
};
static NJS_CNK_OBJECT draw_185 = {
#include "stg10/terrain/draw_185.inc"
};
static NJS_CNK_OBJECT draw_184 = {
#include "stg10/terrain/draw_184.inc"
};
static NJS_CNK_OBJECT draw_183 = {
#include "stg10/terrain/draw_183.inc"
};
static NJS_CNK_OBJECT draw_182 = {
#include "stg10/terrain/draw_182.inc"
};
static NJS_CNK_OBJECT draw_181 = {
#include "stg10/terrain/draw_181.inc"
};
static NJS_CNK_OBJECT draw_180 = {
#include "stg10/terrain/draw_180.inc"
};
static NJS_CNK_OBJECT draw_179 = {
#include "stg10/terrain/draw_179.inc"
};
static NJS_CNK_OBJECT draw_175 = {
#include "stg10/terrain/draw_175.inc"
};
static NJS_CNK_OBJECT draw_174 = {
#include "stg10/terrain/draw_174.inc"
};
static NJS_CNK_OBJECT draw_170 = {
#include "stg10/terrain/draw_170.inc"
};
static NJS_CNK_OBJECT draw_169 = {
#include "stg10/terrain/draw_169.inc"
};
static NJS_CNK_OBJECT draw_168 = {
#include "stg10/terrain/draw_168.inc"
};
static NJS_CNK_OBJECT draw_167 = {
#include "stg10/terrain/draw_167.inc"
};
static NJS_CNK_OBJECT draw_166 = {
#include "stg10/terrain/draw_166.inc"
};
static NJS_CNK_OBJECT draw_164 = {
#include "stg10/terrain/draw_164.inc"
};
static NJS_CNK_OBJECT draw_163 = {
#include "stg10/terrain/draw_163.inc"
};
static NJS_CNK_OBJECT draw_161 = {
#include "stg10/terrain/draw_161.inc"
};
static NJS_CNK_OBJECT draw_158 = {
#include "stg10/terrain/draw_158.inc"
};
static NJS_CNK_OBJECT draw_156 = {
#include "stg10/terrain/draw_156.inc"
};
static NJS_CNK_OBJECT draw_154 = {
#include "stg10/terrain/draw_154.inc"
};
static NJS_CNK_OBJECT draw_150 = {
#include "stg10/terrain/draw_150.inc"
};
static NJS_CNK_OBJECT draw_149 = {
#include "stg10/terrain/draw_149.inc"
};
static NJS_CNK_OBJECT draw_148 = {
#include "stg10/terrain/draw_148.inc"
};
static NJS_CNK_OBJECT draw_141 = {
#include "stg10/terrain/draw_141.inc"
};
static NJS_CNK_OBJECT draw_140 = {
#include "stg10/terrain/draw_140.inc"
};
static NJS_CNK_OBJECT draw_137 = {
#include "stg10/terrain/draw_137.inc"
};
static NJS_CNK_OBJECT draw_133 = {
#include "stg10/terrain/draw_133.inc"
};
static NJS_CNK_OBJECT draw_131 = {
#include "stg10/terrain/draw_131.inc"
};
static NJS_CNK_OBJECT draw_130 = {
#include "stg10/terrain/draw_130.inc"
};
static NJS_CNK_OBJECT draw_128 = {
#include "stg10/terrain/draw_128.inc"
};
static NJS_CNK_OBJECT draw_127 = {
#include "stg10/terrain/draw_127.inc"
};
static NJS_CNK_OBJECT draw_126 = {
#include "stg10/terrain/draw_126.inc"
};
static NJS_CNK_OBJECT draw_125 = {
#include "stg10/terrain/draw_125.inc"
};
static NJS_CNK_OBJECT draw_124 = {
#include "stg10/terrain/draw_124.inc"
};
static NJS_CNK_OBJECT draw_121 = {
#include "stg10/terrain/draw_121.inc"
};
static NJS_CNK_OBJECT draw_115 = {
#include "stg10/terrain/draw_115.inc"
};
static NJS_CNK_OBJECT draw_113 = {
#include "stg10/terrain/draw_113.inc"
};
static NJS_CNK_OBJECT draw_112 = {
#include "stg10/terrain/draw_112.inc"
};
static NJS_CNK_OBJECT draw_111 = {
#include "stg10/terrain/draw_111.inc"
};
static NJS_CNK_OBJECT draw_107 = {
#include "stg10/terrain/draw_107.inc"
};
static NJS_CNK_OBJECT draw_105 = {
#include "stg10/terrain/draw_105.inc"
};
static NJS_CNK_OBJECT draw_102 = {
#include "stg10/terrain/draw_102.inc"
};
static NJS_CNK_OBJECT draw_101 = {
#include "stg10/terrain/draw_101.inc"
};
static NJS_CNK_OBJECT draw_100 = {
#include "stg10/terrain/draw_100.inc"
};
static NJS_CNK_OBJECT draw_96 = {
#include "stg10/terrain/draw_96.inc"
};
static NJS_CNK_OBJECT draw_93 = {
#include "stg10/terrain/draw_93.inc"
};
static NJS_CNK_OBJECT draw_92 = {
#include "stg10/terrain/draw_92.inc"
};
static NJS_CNK_OBJECT draw_91 = {
#include "stg10/terrain/draw_91.inc"
};
static NJS_CNK_OBJECT draw_87 = {
#include "stg10/terrain/draw_87.inc"
};
static NJS_CNK_OBJECT draw_86 = {
#include "stg10/terrain/draw_86.inc"
};
static NJS_CNK_OBJECT draw_84 = {
#include "stg10/terrain/draw_84.inc"
};
static NJS_CNK_OBJECT draw_83 = {
#include "stg10/terrain/draw_83.inc"
};
static NJS_CNK_OBJECT draw_82 = {
#include "stg10/terrain/draw_82.inc"
};
static NJS_CNK_OBJECT draw_77 = {
#include "stg10/terrain/draw_77.inc"
};
static NJS_CNK_OBJECT draw_76 = {
#include "stg10/terrain/draw_76.inc"
};
static NJS_CNK_OBJECT draw_75 = {
#include "stg10/terrain/draw_75.inc"
};
static NJS_CNK_OBJECT draw_74 = {
#include "stg10/terrain/draw_74.inc"
};
static NJS_CNK_OBJECT draw_72 = {
#include "stg10/terrain/draw_72.inc"
};
static NJS_CNK_OBJECT draw_71 = {
#include "stg10/terrain/draw_71.inc"
};
static NJS_CNK_OBJECT draw_70 = {
#include "stg10/terrain/draw_70.inc"
};
static NJS_CNK_OBJECT draw_69 = {
#include "stg10/terrain/draw_69.inc"
};
static NJS_CNK_OBJECT draw_68 = {
#include "stg10/terrain/draw_68.inc"
};
static NJS_CNK_OBJECT draw_67 = {
#include "stg10/terrain/draw_67.inc"
};
static NJS_CNK_OBJECT draw_66 = {
#include "stg10/terrain/draw_66.inc"
};
static NJS_CNK_OBJECT draw_65 = {
#include "stg10/terrain/draw_65.inc"
};
static NJS_CNK_OBJECT draw_64 = {
#include "stg10/terrain/draw_64.inc"
};
static NJS_CNK_OBJECT draw_63 = {
#include "stg10/terrain/draw_63.inc"
};
static NJS_CNK_OBJECT draw_62 = {
#include "stg10/terrain/draw_62.inc"
};
static NJS_CNK_OBJECT draw_60 = {
#include "stg10/terrain/draw_60.inc"
};
static NJS_CNK_OBJECT draw_59 = {
#include "stg10/terrain/draw_59.inc"
};
static NJS_CNK_OBJECT draw_58 = {
#include "stg10/terrain/draw_58.inc"
};
static NJS_CNK_OBJECT draw_57 = {
#include "stg10/terrain/draw_57.inc"
};
static NJS_CNK_OBJECT draw_56 = {
#include "stg10/terrain/draw_56.inc"
};
static NJS_CNK_OBJECT draw_54 = {
#include "stg10/terrain/draw_54.inc"
};
static NJS_CNK_OBJECT draw_53 = {
#include "stg10/terrain/draw_53.inc"
};
static NJS_CNK_OBJECT draw_52 = {
#include "stg10/terrain/draw_52.inc"
};
static NJS_CNK_OBJECT draw_50 = {
#include "stg10/terrain/draw_50.inc"
};
static NJS_CNK_OBJECT draw_48 = {
#include "stg10/terrain/draw_48.inc"
};
static NJS_CNK_OBJECT draw_46 = {
#include "stg10/terrain/draw_46.inc"
};
static NJS_CNK_OBJECT draw_45 = {
#include "stg10/terrain/draw_45.inc"
};
static NJS_CNK_OBJECT draw_43 = {
#include "stg10/terrain/draw_43.inc"
};
static NJS_CNK_OBJECT draw_42 = {
#include "stg10/terrain/draw_42.inc"
};
static NJS_CNK_OBJECT draw_41 = {
#include "stg10/terrain/draw_41.inc"
};
static NJS_CNK_OBJECT draw_40 = {
#include "stg10/terrain/draw_40.inc"
};
static NJS_CNK_OBJECT draw_39 = {
#include "stg10/terrain/draw_39.inc"
};
static NJS_CNK_OBJECT draw_38 = {
#include "stg10/terrain/draw_38.inc"
};
static NJS_CNK_OBJECT draw_37 = {
#include "stg10/terrain/draw_37.inc"
};
static NJS_CNK_OBJECT draw_36 = {
#include "stg10/terrain/draw_36.inc"
};
static NJS_CNK_OBJECT draw_35 = {
#include "stg10/terrain/draw_35.inc"
};
static NJS_CNK_OBJECT draw_34 = {
#include "stg10/terrain/draw_34.inc"
};
static NJS_CNK_OBJECT draw_33 = {
#include "stg10/terrain/draw_33.inc"
};
static NJS_CNK_OBJECT draw_27 = {
#include "stg10/terrain/draw_27.inc"
};
static NJS_CNK_OBJECT draw_25 = {
#include "stg10/terrain/draw_25.inc"
};
static NJS_CNK_OBJECT draw_24 = {
#include "stg10/terrain/draw_24.inc"
};
static NJS_CNK_OBJECT draw_23 = {
#include "stg10/terrain/draw_23.inc"
};
static NJS_CNK_OBJECT draw_22 = {
#include "stg10/terrain/draw_22.inc"
};
static NJS_CNK_OBJECT draw_21 = {
#include "stg10/terrain/draw_21.inc"
};
static NJS_CNK_OBJECT draw_20 = {
#include "stg10/terrain/draw_20.inc"
};
static NJS_CNK_OBJECT draw_19 = {
#include "stg10/terrain/draw_19.inc"
};
static NJS_CNK_OBJECT draw_18 = {
#include "stg10/terrain/draw_18.inc"
};
static NJS_CNK_OBJECT draw_17 = {
#include "stg10/terrain/draw_17.inc"
};
static NJS_CNK_OBJECT draw_16 = {
#include "stg10/terrain/draw_16.inc"
};
static NJS_CNK_OBJECT draw_15 = {
#include "stg10/terrain/draw_15.inc"
};
static NJS_CNK_OBJECT draw_14 = {
#include "stg10/terrain/draw_14.inc"
};
static NJS_CNK_OBJECT draw_13 = {
#include "stg10/terrain/draw_13.inc"
};
static NJS_CNK_OBJECT draw_12 = {
#include "stg10/terrain/draw_12.inc"
};
static NJS_CNK_OBJECT draw_11 = {
#include "stg10/terrain/draw_11.inc"
};
static NJS_CNK_OBJECT draw_10 = {
#include "stg10/terrain/draw_10.inc"
};
static NJS_CNK_OBJECT draw_9 = {
#include "stg10/terrain/draw_9.inc"
};
static NJS_CNK_OBJECT draw_8 = {
#include "stg10/terrain/draw_8.inc"
};
static NJS_CNK_OBJECT draw_7 = {
#include "stg10/terrain/draw_7.inc"
};
static NJS_CNK_OBJECT draw_6 = {
#include "stg10/terrain/draw_6.inc"
};
static NJS_CNK_OBJECT draw_5 = {
#include "stg10/terrain/draw_5.inc"
};
static NJS_CNK_OBJECT draw_4 = {
#include "stg10/terrain/draw_4.inc"
};
static NJS_CNK_OBJECT draw_3 = {
#include "stg10/terrain/draw_3.inc"
};
static NJS_CNK_OBJECT draw_2 = {
#include "stg10/terrain/draw_2.inc"
};
static NJS_CNK_OBJECT draw_0 = {
#include "stg10/terrain/draw_0.inc"
};
static NJS_OBJECT collision_4 = {
#include "stg10/terrain/collision_4.inc"
};
static NJS_OBJECT collision_5 = {
#include "stg10/terrain/collision_5.inc"
};
static NJS_OBJECT collision_6 = {
#include "stg10/terrain/collision_6.inc"
};
static NJS_OBJECT collision_7 = {
#include "stg10/terrain/collision_7.inc"
};
static NJS_OBJECT collision_8 = {
#include "stg10/terrain/collision_8.inc"
};
static NJS_OBJECT collision_9 = {
#include "stg10/terrain/collision_9.inc"
};
static NJS_OBJECT collision_10 = {
#include "stg10/terrain/collision_10.inc"
};
static NJS_OBJECT collision_11 = {
#include "stg10/terrain/collision_11.inc"
};
static NJS_OBJECT collision_12 = {
#include "stg10/terrain/collision_12.inc"
};
static NJS_OBJECT collision_13 = {
#include "stg10/terrain/collision_13.inc"
};
static NJS_OBJECT collision_14 = {
#include "stg10/terrain/collision_14.inc"
};
static NJS_OBJECT collision_15 = {
#include "stg10/terrain/collision_15.inc"
};
static NJS_OBJECT collision_16 = {
#include "stg10/terrain/collision_16.inc"
};
static NJS_OBJECT collision_17 = {
#include "stg10/terrain/collision_17.inc"
};
static NJS_OBJECT collision_18 = {
#include "stg10/terrain/collision_18.inc"
};
static NJS_OBJECT collision_19 = {
#include "stg10/terrain/collision_19.inc"
};
static NJS_OBJECT collision_20 = {
#include "stg10/terrain/collision_20.inc"
};
static NJS_OBJECT collision_21 = {
#include "stg10/terrain/collision_21.inc"
};
static NJS_OBJECT collision_22 = {
#include "stg10/terrain/collision_22.inc"
};
static NJS_OBJECT collision_23 = {
#include "stg10/terrain/collision_23.inc"
};
static NJS_OBJECT collision_24 = {
#include "stg10/terrain/collision_24.inc"
};
static NJS_OBJECT collision_25 = {
#include "stg10/terrain/collision_25.inc"
};
static NJS_OBJECT collision_26 = {
#include "stg10/terrain/collision_26.inc"
};
static NJS_OBJECT collision_27 = {
#include "stg10/terrain/collision_27.inc"
};
static NJS_OBJECT collision_28 = {
#include "stg10/terrain/collision_28.inc"
};
static NJS_OBJECT collision_29 = {
#include "stg10/terrain/collision_29.inc"
};
static NJS_OBJECT collision_30 = {
#include "stg10/terrain/collision_30.inc"
};
static NJS_OBJECT collision_31 = {
#include "stg10/terrain/collision_31.inc"
};
static NJS_OBJECT collision_32 = {
#include "stg10/terrain/collision_32.inc"
};
static NJS_OBJECT collision_33 = {
#include "stg10/terrain/collision_33.inc"
};
static NJS_OBJECT collision_34 = {
#include "stg10/terrain/collision_34.inc"
};
static NJS_OBJECT collision_35 = {
#include "stg10/terrain/collision_35.inc"
};
static NJS_OBJECT collision_37 = {
#include "stg10/terrain/collision_37.inc"
};
static NJS_OBJECT collision_38 = {
#include "stg10/terrain/collision_38.inc"
};
static NJS_OBJECT collision_39 = {
#include "stg10/terrain/collision_39.inc"
};
static NJS_OBJECT collision_40 = {
#include "stg10/terrain/collision_40.inc"
};
static NJS_OBJECT collision_41 = {
#include "stg10/terrain/collision_41.inc"
};
static NJS_OBJECT collision_44 = {
#include "stg10/terrain/collision_44.inc"
};
static NJS_OBJECT collision_45 = {
#include "stg10/terrain/collision_45.inc"
};
static NJS_OBJECT collision_46 = {
#include "stg10/terrain/collision_46.inc"
};
static NJS_OBJECT collision_47 = {
#include "stg10/terrain/collision_47.inc"
};
static NJS_OBJECT collision_48 = {
#include "stg10/terrain/collision_48.inc"
};
static NJS_OBJECT collision_50 = {
#include "stg10/terrain/collision_50.inc"
};
static NJS_OBJECT collision_53 = {
#include "stg10/terrain/collision_53.inc"
};
static NJS_OBJECT collision_54 = {
#include "stg10/terrain/collision_54.inc"
};
static NJS_OBJECT collision_56 = {
#include "stg10/terrain/collision_56.inc"
};
static NJS_OBJECT collision_57 = {
#include "stg10/terrain/collision_57.inc"
};
static NJS_OBJECT collision_58 = {
#include "stg10/terrain/collision_58.inc"
};
static NJS_OBJECT collision_59 = {
#include "stg10/terrain/collision_59.inc"
};
static NJS_OBJECT collision_60 = {
#include "stg10/terrain/collision_60.inc"
};
static NJS_OBJECT collision_61 = {
#include "stg10/terrain/collision_61.inc"
};
static NJS_OBJECT collision_62 = {
#include "stg10/terrain/collision_62.inc"
};
static NJS_OBJECT collision_65 = {
#include "stg10/terrain/collision_65.inc"
};
static NJS_OBJECT collision_66 = {
#include "stg10/terrain/collision_66.inc"
};
static NJS_OBJECT collision_67 = {
#include "stg10/terrain/collision_67.inc"
};
static NJS_OBJECT collision_68 = {
#include "stg10/terrain/collision_68.inc"
};
static NJS_OBJECT collision_69 = {
#include "stg10/terrain/collision_69.inc"
};
static NJS_OBJECT collision_70 = {
#include "stg10/terrain/collision_70.inc"
};
static NJS_OBJECT collision_72 = {
#include "stg10/terrain/collision_72.inc"
};
static NJS_OBJECT collision_74 = {
#include "stg10/terrain/collision_74.inc"
};
static NJS_OBJECT collision_75 = {
#include "stg10/terrain/collision_75.inc"
};
static NJS_OBJECT collision_76 = {
#include "stg10/terrain/collision_76.inc"
};
static NJS_OBJECT collision_77 = {
#include "stg10/terrain/collision_77.inc"
};
static NJS_OBJECT collision_78 = {
#include "stg10/terrain/collision_78.inc"
};
static NJS_OBJECT collision_80 = {
#include "stg10/terrain/collision_80.inc"
};
static NJS_OBJECT collision_82 = {
#include "stg10/terrain/collision_82.inc"
};
static NJS_OBJECT collision_83 = {
#include "stg10/terrain/collision_83.inc"
};
static NJS_OBJECT collision_84 = {
#include "stg10/terrain/collision_84.inc"
};
static NJS_OBJECT collision_86 = {
#include "stg10/terrain/collision_86.inc"
};
static NJS_OBJECT collision_87 = {
#include "stg10/terrain/collision_87.inc"
};
static NJS_OBJECT collision_90 = {
#include "stg10/terrain/collision_90.inc"
};
static NJS_OBJECT collision_91 = {
#include "stg10/terrain/collision_91.inc"
};
static NJS_OBJECT collision_92 = {
#include "stg10/terrain/collision_92.inc"
};
static NJS_OBJECT collision_93 = {
#include "stg10/terrain/collision_93.inc"
};
static NJS_OBJECT collision_94 = {
#include "stg10/terrain/collision_94.inc"
};
static NJS_OBJECT collision_95 = {
#include "stg10/terrain/collision_95.inc"
};
static NJS_OBJECT collision_96 = {
#include "stg10/terrain/collision_96.inc"
};
static NJS_OBJECT collision_97 = {
#include "stg10/terrain/collision_97.inc"
};
static NJS_OBJECT collision_98 = {
#include "stg10/terrain/collision_98.inc"
};
static NJS_OBJECT collision_99 = {
#include "stg10/terrain/collision_99.inc"
};
static NJS_OBJECT collision_101 = {
#include "stg10/terrain/collision_101.inc"
};
static NJS_OBJECT collision_111 = {
#include "stg10/terrain/collision_111.inc"
};
static NJS_OBJECT collision_112 = {
#include "stg10/terrain/collision_112.inc"
};
static NJS_OBJECT collision_114 = {
#include "stg10/terrain/collision_114.inc"
};
static NJS_OBJECT collision_115 = {
#include "stg10/terrain/collision_115.inc"
};
static NJS_OBJECT collision_120 = {
#include "stg10/terrain/collision_120.inc"
};
static NJS_OBJECT collision_121 = {
#include "stg10/terrain/collision_121.inc"
};
static NJS_OBJECT collision_123 = {
#include "stg10/terrain/collision_123.inc"
};
static NJS_OBJECT collision_125 = {
#include "stg10/terrain/collision_125.inc"
};
static NJS_OBJECT collision_126 = {
#include "stg10/terrain/collision_126.inc"
};
static NJS_OBJECT collision_128 = {
#include "stg10/terrain/collision_128.inc"
};
static NJS_OBJECT collision_130 = {
#include "stg10/terrain/collision_130.inc"
};
static NJS_OBJECT collision_132 = {
#include "stg10/terrain/collision_132.inc"
};
static NJS_OBJECT collision_135 = {
#include "stg10/terrain/collision_135.inc"
};
static NJS_OBJECT collision_136 = {
#include "stg10/terrain/collision_136.inc"
};
static NJS_OBJECT collision_137 = {
#include "stg10/terrain/collision_137.inc"
};
static NJS_OBJECT collision_138 = {
#include "stg10/terrain/collision_138.inc"
};
static NJS_OBJECT collision_139 = {
#include "stg10/terrain/collision_139.inc"
};
static NJS_OBJECT collision_140 = {
#include "stg10/terrain/collision_140.inc"
};
static NJS_OBJECT collision_141 = {
#include "stg10/terrain/collision_141.inc"
};
static NJS_OBJECT collision_142 = {
#include "stg10/terrain/collision_142.inc"
};
static NJS_OBJECT collision_143 = {
#include "stg10/terrain/collision_143.inc"
};
static NJS_OBJECT collision_144 = {
#include "stg10/terrain/collision_144.inc"
};
static NJS_OBJECT collision_145 = {
#include "stg10/terrain/collision_145.inc"
};
static NJS_OBJECT collision_146 = {
#include "stg10/terrain/collision_146.inc"
};
static NJS_OBJECT collision_147 = {
#include "stg10/terrain/collision_147.inc"
};
static NJS_OBJECT collision_151 = {
#include "stg10/terrain/collision_151.inc"
};
static NJS_OBJECT collision_152 = {
#include "stg10/terrain/collision_152.inc"
};
static NJS_OBJECT collision_153 = {
#include "stg10/terrain/collision_153.inc"
};
static NJS_OBJECT collision_158 = {
#include "stg10/terrain/collision_158.inc"
};
static NJS_OBJECT collision_185 = {
#include "stg10/terrain/collision_185.inc"
};
static NJS_OBJECT collision_189 = {
#include "stg10/terrain/collision_189.inc"
};
static NJS_OBJECT collision_191 = {
#include "stg10/terrain/collision_191.inc"
};
static NJS_OBJECT collision_192 = {
#include "stg10/terrain/collision_192.inc"
};
static NJS_OBJECT collision_193 = {
#include "stg10/terrain/collision_193.inc"
};
static NJS_OBJECT collision_194 = {
#include "stg10/terrain/collision_194.inc"
};
static NJS_OBJECT collision_195 = {
#include "stg10/terrain/collision_195.inc"
};
static NJS_OBJECT collision_197 = {
#include "stg10/terrain/collision_197.inc"
};
static NJS_OBJECT collision_198 = {
#include "stg10/terrain/collision_198.inc"
};
static NJS_OBJECT collision_207 = {
#include "stg10/terrain/collision_207.inc"
};
static NJS_OBJECT collision_209 = {
#include "stg10/terrain/collision_209.inc"
};
static NJS_OBJECT collision_212 = {
#include "stg10/terrain/collision_212.inc"
};
static NJS_OBJECT collision_213 = {
#include "stg10/terrain/collision_213.inc"
};
static NJS_OBJECT collision_214 = {
#include "stg10/terrain/collision_214.inc"
};
static NJS_OBJECT collision_221 = {
#include "stg10/terrain/collision_221.inc"
};
static NJS_OBJECT collision_222 = {
#include "stg10/terrain/collision_222.inc"
};
static NJS_OBJECT collision_223 = {
#include "stg10/terrain/collision_223.inc"
};
static NJS_OBJECT collision_232 = {
#include "stg10/terrain/collision_232.inc"
};
static NJS_OBJECT collision_246 = {
#include "stg10/terrain/collision_246.inc"
};
static NJS_OBJECT collision_251 = {
#include "stg10/terrain/collision_251.inc"
};
static NJS_OBJECT collision_252 = {
#include "stg10/terrain/collision_252.inc"
};
static NJS_OBJECT collision_256 = {
#include "stg10/terrain/collision_256.inc"
};
static NJS_OBJECT collision_257 = {
#include "stg10/terrain/collision_257.inc"
};
static NJS_OBJECT collision_258 = {
#include "stg10/terrain/collision_258.inc"
};
static NJS_OBJECT collision_259 = {
#include "stg10/terrain/collision_259.inc"
};
static NJS_OBJECT collision_260 = {
#include "stg10/terrain/collision_260.inc"
};
static NJS_OBJECT collision_265 = {
#include "stg10/terrain/collision_265.inc"
};
static NJS_OBJECT collision_267 = {
#include "stg10/terrain/collision_267.inc"
};
static NJS_OBJECT collision_270 = {
#include "stg10/terrain/collision_270.inc"
};
static NJS_OBJECT collision_274 = {
#include "stg10/terrain/collision_274.inc"
};
static NJS_OBJECT collision_275 = {
#include "stg10/terrain/collision_275.inc"
};
static NJS_OBJECT collision_277 = {
#include "stg10/terrain/collision_277.inc"
};
static NJS_OBJECT collision_278 = {
#include "stg10/terrain/collision_278.inc"
};
static NJS_OBJECT collision_279 = {
#include "stg10/terrain/collision_279.inc"
};
static NJS_OBJECT collision_280 = {
#include "stg10/terrain/collision_280.inc"
};
static NJS_OBJECT collision_281 = {
#include "stg10/terrain/collision_281.inc"
};
static NJS_OBJECT collision_282 = {
#include "stg10/terrain/collision_282.inc"
};
static NJS_OBJECT collision_283 = {
#include "stg10/terrain/collision_283.inc"
};
static NJS_OBJECT collision_284 = {
#include "stg10/terrain/collision_284.inc"
};
static NJS_OBJECT collision_285 = {
#include "stg10/terrain/collision_285.inc"
};
static NJS_OBJECT collision_286 = {
#include "stg10/terrain/collision_286.inc"
};
static NJS_OBJECT collision_287 = {
#include "stg10/terrain/collision_287.inc"
};
static NJS_OBJECT collision_290 = {
#include "stg10/terrain/collision_290.inc"
};
static NJS_OBJECT collision_291 = {
#include "stg10/terrain/collision_291.inc"
};
static NJS_OBJECT collision_292 = {
#include "stg10/terrain/collision_292.inc"
};
static NJS_OBJECT collision_293 = {
#include "stg10/terrain/collision_293.inc"
};
static NJS_OBJECT collision_294 = {
#include "stg10/terrain/collision_294.inc"
};
static NJS_OBJECT collision_296 = {
#include "stg10/terrain/collision_296.inc"
};
static NJS_OBJECT collision_298 = {
#include "stg10/terrain/collision_298.inc"
};
static NJS_OBJECT collision_310 = {
#include "stg10/terrain/collision_310.inc"
};
static NJS_OBJECT collision_312 = {
#include "stg10/terrain/collision_312.inc"
};
static NJS_OBJECT collision_313 = {
#include "stg10/terrain/collision_313.inc"
};
static NJS_OBJECT collision_314 = {
#include "stg10/terrain/collision_314.inc"
};
static NJS_OBJECT collision_315 = {
#include "stg10/terrain/collision_315.inc"
};
static NJS_OBJECT collision_316 = {
#include "stg10/terrain/collision_316.inc"
};
static NJS_OBJECT collision_317 = {
#include "stg10/terrain/collision_317.inc"
};
static NJS_OBJECT collision_318 = {
#include "stg10/terrain/collision_318.inc"
};
static NJS_OBJECT collision_319 = {
#include "stg10/terrain/collision_319.inc"
};
static NJS_OBJECT collision_320 = {
#include "stg10/terrain/collision_320.inc"
};
static NJS_OBJECT collision_321 = {
#include "stg10/terrain/collision_321.inc"
};
