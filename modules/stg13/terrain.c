/* STG13.PRS's motions, their registry, land, texture lists, Basic models,
 * chunk models and texture names, from the disc. its C lies between
 * city_escape.c and objects.c, and its D lies between city_escape.c and
 * objects.c */
#include <ninja.h>
#include <sg_xpt.h>

#include "collision/world.h"
#include "player/player.h"
#include "shared/motions.h"
#include "stg13/city_escape.h"

#include "stg13/terrain/text.inc"

/* the data up to the last that a pointer names ahead of its definition, as
 * tentative definitions in the disc's order: GCC places each datum at its
 * first definition */
static NJS_MKEY_A upgrade_city_escape_motion_joint_4_angles[16];
static NJS_MKEY_A upgrade_city_escape_motion_joint_7_angles[16];
static NJS_MKEY_A upgrade_city_escape_motion_joint_8_angles[14];
static NJS_MKEY_A upgrade_city_escape_motion_joint_9_angles[16];
static NJS_MKEY_A upgrade_city_escape_motion_joint_13_angles[16];
static NJS_MKEY_A upgrade_city_escape_motion_joint_14_angles[2];
static NJS_MKEY_A upgrade_city_escape_motion_joint_17_angles[16];
static NJS_MKEY_A upgrade_city_escape_motion_joint_18_angles[2];
static NJS_MKEY_A upgrade_city_escape_motion_joint_19_angles[8];
static NJS_MKEY_A upgrade_city_escape_motion_joint_23_angles[16];
static NJS_MKEY_A upgrade_city_escape_motion_joint_24_angles[2];
static NJS_MKEY_A upgrade_city_escape_motion_joint_27_angles[16];
static NJS_MKEY_A upgrade_city_escape_motion_joint_31_angles[2];
static NJS_MKEY_A upgrade_city_escape_motion_joint_32_angles[16];
static NJS_MKEY_A upgrade_city_escape_motion_joint_33_angles[12];
static NJS_MKEY_A upgrade_city_escape_motion_joint_37_angles[16];
static NJS_MKEY_A upgrade_city_escape_motion_joint_38_angles[16];
static NJS_MKEY_A upgrade_city_escape_motion_joint_39_angles[16];
static NJS_MKEY_A upgrade_city_escape_motion_joint_41_angles[16];
static NJS_MKEY_A upgrade_city_escape_motion_joint_44_angles[16];
static NJS_MKEY_A upgrade_city_escape_motion_joint_51_angles[16];
static NJS_MKEY_A upgrade_city_escape_motion_joint_53_angles[16];
static NJS_MKEY_A upgrade_city_escape_motion_joint_56_angles[16];
static NJS_MKEY_A upgrade_city_escape_motion_joint_57_angles[16];
static NJS_MKEY_A upgrade_city_escape_motion_joint_58_angles[16];
static NJS_MKEY_A upgrade_city_escape_motion_joint_60_angles[16];
static NJS_MDATA2 upgrade_city_escape_motion_tracks[62];
static NJS_MOTION upgrade_city_escape_motion;
static NJS_MKEY_F bar_throw_back_motion_joint_0_position[2];
static NJS_MKEY_F bar_throw_back_motion_joint_2_position[2];
static NJS_MKEY_A bar_throw_back_motion_joint_4_angles[2];
static NJS_MKEY_A bar_throw_back_motion_joint_7_angles[32];
static NJS_MKEY_A bar_throw_back_motion_joint_8_angles[32];
static NJS_MKEY_A bar_throw_back_motion_joint_9_angles[32];
static NJS_MKEY_A bar_throw_back_motion_joint_13_angles[32];
static NJS_MKEY_A bar_throw_back_motion_joint_14_angles[32];
static NJS_MKEY_A bar_throw_back_motion_joint_17_angles[32];
static NJS_MKEY_A bar_throw_back_motion_joint_18_angles[32];
static NJS_MKEY_A bar_throw_back_motion_joint_19_angles[32];
static NJS_MKEY_A bar_throw_back_motion_joint_23_angles[32];
static NJS_MKEY_A bar_throw_back_motion_joint_24_angles[32];
static NJS_MKEY_A bar_throw_back_motion_joint_27_angles[2];
static NJS_MKEY_A bar_throw_back_motion_joint_28_angles[2];
static NJS_MKEY_A bar_throw_back_motion_joint_29_angles[2];
static NJS_MKEY_A bar_throw_back_motion_joint_32_angles[32];
static NJS_MKEY_A bar_throw_back_motion_joint_33_angles[25];
static NJS_MKEY_A bar_throw_back_motion_joint_37_angles[32];
static NJS_MKEY_A bar_throw_back_motion_joint_38_angles[32];
static NJS_MKEY_A bar_throw_back_motion_joint_39_angles[32];
static NJS_MKEY_A bar_throw_back_motion_joint_41_angles[17];
static NJS_MKEY_A bar_throw_back_motion_joint_44_angles[32];
static NJS_MKEY_A bar_throw_back_motion_joint_51_angles[32];
static NJS_MKEY_A bar_throw_back_motion_joint_53_angles[32];
static NJS_MKEY_A bar_throw_back_motion_joint_56_angles[32];
static NJS_MKEY_A bar_throw_back_motion_joint_57_angles[32];
static NJS_MKEY_A bar_throw_back_motion_joint_58_angles[32];
static NJS_MKEY_A bar_throw_back_motion_joint_60_angles[17];
static NJS_MDATA2 bar_throw_back_motion_tracks[62];
static NJS_MOTION bar_throw_back_motion;
static NJS_MKEY_F bar_throw_motion_joint_0_position[2];
static NJS_MKEY_F bar_throw_motion_joint_2_position[2];
static NJS_MKEY_A bar_throw_motion_joint_4_angles[33];
static NJS_MKEY_A bar_throw_motion_joint_7_angles[33];
static NJS_MKEY_A bar_throw_motion_joint_8_angles[33];
static NJS_MKEY_A bar_throw_motion_joint_9_angles[33];
static NJS_MKEY_A bar_throw_motion_joint_13_angles[33];
static NJS_MKEY_A bar_throw_motion_joint_14_angles[33];
static NJS_MKEY_A bar_throw_motion_joint_17_angles[33];
static NJS_MKEY_A bar_throw_motion_joint_18_angles[33];
static NJS_MKEY_A bar_throw_motion_joint_19_angles[33];
static NJS_MKEY_A bar_throw_motion_joint_23_angles[33];
static NJS_MKEY_A bar_throw_motion_joint_24_angles[33];
static NJS_MKEY_A bar_throw_motion_joint_27_angles[33];
static NJS_MKEY_A bar_throw_motion_joint_28_angles[15];
static NJS_MKEY_A bar_throw_motion_joint_29_angles[15];
static NJS_MKEY_A bar_throw_motion_joint_32_angles[33];
static NJS_MKEY_A bar_throw_motion_joint_33_angles[26];
static NJS_MKEY_A bar_throw_motion_joint_37_angles[33];
static NJS_MKEY_A bar_throw_motion_joint_38_angles[33];
static NJS_MKEY_A bar_throw_motion_joint_39_angles[33];
static NJS_MKEY_A bar_throw_motion_joint_41_angles[2];
static NJS_MKEY_A bar_throw_motion_joint_44_angles[33];
static NJS_MKEY_A bar_throw_motion_joint_51_angles[33];
static NJS_MKEY_A bar_throw_motion_joint_53_angles[33];
static NJS_MKEY_A bar_throw_motion_joint_56_angles[33];
static NJS_MKEY_A bar_throw_motion_joint_57_angles[33];
static NJS_MKEY_A bar_throw_motion_joint_58_angles[33];
static NJS_MKEY_A bar_throw_motion_joint_60_angles[2];
static NJS_MDATA2 bar_throw_motion_tracks[62];
static NJS_MOTION bar_throw_motion;
static NJS_MKEY_F bar_fly_back_motion_joint_0_position[2];
static NJS_MKEY_F bar_fly_back_motion_joint_2_position[2];
static NJS_MKEY_A bar_fly_back_motion_joint_4_angles[2];
static NJS_MKEY_A bar_fly_back_motion_joint_7_angles[2];
static NJS_MKEY_A bar_fly_back_motion_joint_8_angles[2];
static NJS_MKEY_A bar_fly_back_motion_joint_9_angles[2];
static NJS_MKEY_A bar_fly_back_motion_joint_13_angles[2];
static NJS_MKEY_A bar_fly_back_motion_joint_14_angles[2];
static NJS_MKEY_A bar_fly_back_motion_joint_17_angles[2];
static NJS_MKEY_A bar_fly_back_motion_joint_18_angles[2];
static NJS_MKEY_A bar_fly_back_motion_joint_19_angles[2];
static NJS_MKEY_A bar_fly_back_motion_joint_23_angles[2];
static NJS_MKEY_A bar_fly_back_motion_joint_24_angles[2];
static NJS_MKEY_A bar_fly_back_motion_joint_27_angles[2];
static NJS_MKEY_A bar_fly_back_motion_joint_32_angles[2];
static NJS_MKEY_A bar_fly_back_motion_joint_37_angles[2];
static NJS_MKEY_A bar_fly_back_motion_joint_38_angles[2];
static NJS_MKEY_A bar_fly_back_motion_joint_39_angles[2];
static NJS_MKEY_A bar_fly_back_motion_joint_41_angles[2];
static NJS_MKEY_A bar_fly_back_motion_joint_44_angles[2];
static NJS_MKEY_A bar_fly_back_motion_joint_51_angles[2];
static NJS_MKEY_A bar_fly_back_motion_joint_53_angles[2];
static NJS_MKEY_A bar_fly_back_motion_joint_56_angles[6];
static NJS_MKEY_A bar_fly_back_motion_joint_57_angles[2];
static NJS_MKEY_A bar_fly_back_motion_joint_58_angles[3];
static NJS_MKEY_A bar_fly_back_motion_joint_60_angles[2];
static NJS_MDATA2 bar_fly_back_motion_tracks[62];
static NJS_MOTION bar_fly_back_motion;
static NJS_MKEY_F bar_fly_motion_joint_0_position[2];
static NJS_MKEY_F bar_fly_motion_joint_2_position[2];
static NJS_MKEY_A bar_fly_motion_joint_4_angles[2];
static NJS_MKEY_A bar_fly_motion_joint_7_angles[2];
static NJS_MKEY_A bar_fly_motion_joint_8_angles[2];
static NJS_MKEY_A bar_fly_motion_joint_9_angles[2];
static NJS_MKEY_A bar_fly_motion_joint_13_angles[2];
static NJS_MKEY_A bar_fly_motion_joint_14_angles[2];
static NJS_MKEY_A bar_fly_motion_joint_17_angles[2];
static NJS_MKEY_A bar_fly_motion_joint_18_angles[2];
static NJS_MKEY_A bar_fly_motion_joint_19_angles[2];
static NJS_MKEY_A bar_fly_motion_joint_23_angles[2];
static NJS_MKEY_A bar_fly_motion_joint_24_angles[2];
static NJS_MKEY_A bar_fly_motion_joint_27_angles[2];
static NJS_MKEY_A bar_fly_motion_joint_32_angles[2];
static NJS_MKEY_A bar_fly_motion_joint_37_angles[2];
static NJS_MKEY_A bar_fly_motion_joint_38_angles[2];
static NJS_MKEY_A bar_fly_motion_joint_39_angles[2];
static NJS_MKEY_A bar_fly_motion_joint_41_angles[2];
static NJS_MKEY_A bar_fly_motion_joint_44_angles[2];
static NJS_MKEY_A bar_fly_motion_joint_51_angles[2];
static NJS_MKEY_A bar_fly_motion_joint_53_angles[2];
static NJS_MKEY_A bar_fly_motion_joint_56_angles[3];
static NJS_MKEY_A bar_fly_motion_joint_57_angles[2];
static NJS_MKEY_A bar_fly_motion_joint_58_angles[2];
static NJS_MKEY_A bar_fly_motion_joint_60_angles[2];
static NJS_MDATA2 bar_fly_motion_tracks[62];
static NJS_MOTION bar_fly_motion;
static NJS_MKEY_F bar_swing_back_motion_joint_0_position[2];
static NJS_MKEY_F bar_swing_back_motion_joint_2_position[2];
static NJS_MKEY_A bar_swing_back_motion_joint_4_angles[32];
static NJS_MKEY_A bar_swing_back_motion_joint_7_angles[32];
static NJS_MKEY_A bar_swing_back_motion_joint_8_angles[32];
static NJS_MKEY_A bar_swing_back_motion_joint_9_angles[32];
static NJS_MKEY_A bar_swing_back_motion_joint_13_angles[32];
static NJS_MKEY_A bar_swing_back_motion_joint_14_angles[25];
static NJS_MKEY_A bar_swing_back_motion_joint_17_angles[32];
static NJS_MKEY_A bar_swing_back_motion_joint_18_angles[32];
static NJS_MKEY_A bar_swing_back_motion_joint_19_angles[32];
static NJS_MKEY_A bar_swing_back_motion_joint_23_angles[32];
static NJS_MKEY_A bar_swing_back_motion_joint_24_angles[32];
static NJS_MKEY_A bar_swing_back_motion_joint_27_angles[32];
static NJS_MKEY_A bar_swing_back_motion_joint_28_angles[28];
static NJS_MKEY_A bar_swing_back_motion_joint_29_angles[29];
static NJS_MKEY_A bar_swing_back_motion_joint_31_angles[2];
static NJS_MKEY_A bar_swing_back_motion_joint_32_angles[2];
static NJS_MKEY_A bar_swing_back_motion_joint_33_angles[2];
static NJS_MKEY_A bar_swing_back_motion_joint_37_angles[2];
static NJS_MKEY_A bar_swing_back_motion_joint_38_angles[2];
static NJS_MKEY_A bar_swing_back_motion_joint_39_angles[2];
static NJS_MKEY_A bar_swing_back_motion_joint_41_angles[2];
static NJS_MKEY_A bar_swing_back_motion_joint_44_angles[32];
static NJS_MKEY_A bar_swing_back_motion_joint_51_angles[32];
static NJS_MKEY_A bar_swing_back_motion_joint_53_angles[32];
static NJS_MKEY_A bar_swing_back_motion_joint_56_angles[2];
static NJS_MKEY_A bar_swing_back_motion_joint_57_angles[2];
static NJS_MKEY_A bar_swing_back_motion_joint_58_angles[2];
static NJS_MKEY_A bar_swing_back_motion_joint_60_angles[2];
static NJS_MDATA2 bar_swing_back_motion_tracks[62];
static NJS_MOTION bar_swing_back_motion;
static NJS_MKEY_F bar_swing_motion_joint_0_position[2];
static NJS_MKEY_F bar_swing_motion_joint_2_position[2];
static NJS_MKEY_A bar_swing_motion_joint_4_angles[32];
static NJS_MKEY_A bar_swing_motion_joint_7_angles[32];
static NJS_MKEY_A bar_swing_motion_joint_8_angles[32];
static NJS_MKEY_A bar_swing_motion_joint_9_angles[32];
static NJS_MKEY_A bar_swing_motion_joint_13_angles[32];
static NJS_MKEY_A bar_swing_motion_joint_14_angles[28];
static NJS_MKEY_A bar_swing_motion_joint_17_angles[32];
static NJS_MKEY_A bar_swing_motion_joint_18_angles[32];
static NJS_MKEY_A bar_swing_motion_joint_19_angles[32];
static NJS_MKEY_A bar_swing_motion_joint_23_angles[32];
static NJS_MKEY_A bar_swing_motion_joint_24_angles[28];
static NJS_MKEY_A bar_swing_motion_joint_27_angles[32];
static NJS_MKEY_A bar_swing_motion_joint_28_angles[32];
static NJS_MKEY_A bar_swing_motion_joint_29_angles[32];
static NJS_MKEY_A bar_swing_motion_joint_31_angles[2];
static NJS_MKEY_A bar_swing_motion_joint_32_angles[2];
static NJS_MKEY_A bar_swing_motion_joint_33_angles[2];
static NJS_MKEY_A bar_swing_motion_joint_37_angles[2];
static NJS_MKEY_A bar_swing_motion_joint_38_angles[2];
static NJS_MKEY_A bar_swing_motion_joint_39_angles[2];
static NJS_MKEY_A bar_swing_motion_joint_41_angles[2];
static NJS_MKEY_A bar_swing_motion_joint_44_angles[32];
static NJS_MKEY_A bar_swing_motion_joint_51_angles[32];
static NJS_MKEY_A bar_swing_motion_joint_53_angles[32];
static NJS_MKEY_A bar_swing_motion_joint_56_angles[2];
static NJS_MKEY_A bar_swing_motion_joint_57_angles[2];
static NJS_MKEY_A bar_swing_motion_joint_58_angles[2];
static NJS_MKEY_A bar_swing_motion_joint_60_angles[2];
static NJS_MDATA2 bar_swing_motion_tracks[62];
static NJS_MOTION bar_swing_motion;
static NJS_MKEY_F bar_land_motion_joint_2_position[9];
static NJS_MKEY_A bar_land_motion_joint_2_angles[2];
static NJS_MKEY_A bar_land_motion_joint_4_angles[2];
static NJS_MKEY_A bar_land_motion_joint_7_angles[9];
static NJS_MKEY_A bar_land_motion_joint_8_angles[9];
static NJS_MKEY_A bar_land_motion_joint_9_angles[9];
static NJS_MKEY_A bar_land_motion_joint_13_angles[9];
static NJS_MKEY_A bar_land_motion_joint_14_angles[2];
static NJS_MKEY_A bar_land_motion_joint_17_angles[9];
static NJS_MKEY_A bar_land_motion_joint_18_angles[9];
static NJS_MKEY_A bar_land_motion_joint_19_angles[9];
static NJS_MKEY_A bar_land_motion_joint_23_angles[9];
static NJS_MKEY_A bar_land_motion_joint_24_angles[4];
static NJS_MKEY_A bar_land_motion_joint_27_angles[9];
static NJS_MKEY_A bar_land_motion_joint_28_angles[9];
static NJS_MKEY_A bar_land_motion_joint_29_angles[9];
static NJS_MKEY_A bar_land_motion_joint_32_angles[6];
static NJS_MKEY_A bar_land_motion_joint_33_angles[6];
static NJS_MKEY_A bar_land_motion_joint_37_angles[5];
static NJS_MKEY_A bar_land_motion_joint_38_angles[5];
static NJS_MKEY_A bar_land_motion_joint_39_angles[5];
static NJS_MKEY_A bar_land_motion_joint_41_angles[9];
static NJS_MKEY_A bar_land_motion_joint_44_angles[9];
static NJS_MKEY_A bar_land_motion_joint_51_angles[9];
static NJS_MKEY_A bar_land_motion_joint_53_angles[9];
static NJS_MKEY_A bar_land_motion_joint_56_angles[8];
static NJS_MKEY_A bar_land_motion_joint_57_angles[5];
static NJS_MKEY_A bar_land_motion_joint_58_angles[5];
static NJS_MKEY_A bar_land_motion_joint_60_angles[9];
static NJS_MDATA2 bar_land_motion_tracks[62];
static NJS_MOTION bar_land_motion;
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
static NJS_MKEY_F rider_trick_city_a_motion_joint_2_position[146];
static NJS_MKEY_A rider_trick_city_a_motion_joint_4_angles[146];
static NJS_MKEY_A rider_trick_city_a_motion_joint_7_angles[146];
static NJS_MKEY_A rider_trick_city_a_motion_joint_8_angles[137];
static NJS_MKEY_A rider_trick_city_a_motion_joint_9_angles[145];
static NJS_MKEY_A rider_trick_city_a_motion_joint_13_angles[146];
static NJS_MKEY_A rider_trick_city_a_motion_joint_14_angles[138];
static NJS_MKEY_A rider_trick_city_a_motion_joint_17_angles[146];
static NJS_MKEY_A rider_trick_city_a_motion_joint_18_angles[142];
static NJS_MKEY_A rider_trick_city_a_motion_joint_19_angles[146];
static NJS_MKEY_A rider_trick_city_a_motion_joint_23_angles[146];
static NJS_MKEY_A rider_trick_city_a_motion_joint_24_angles[143];
static NJS_MKEY_A rider_trick_city_a_motion_joint_27_angles[146];
static NJS_MKEY_A rider_trick_city_a_motion_joint_28_angles[40];
static NJS_MKEY_A rider_trick_city_a_motion_joint_29_angles[40];
static NJS_MKEY_A rider_trick_city_a_motion_joint_31_angles[146];
static NJS_MKEY_A rider_trick_city_a_motion_joint_32_angles[146];
static NJS_MKEY_A rider_trick_city_a_motion_joint_33_angles[107];
static NJS_MKEY_A rider_trick_city_a_motion_joint_37_angles[146];
static NJS_MKEY_A rider_trick_city_a_motion_joint_38_angles[99];
static NJS_MKEY_A rider_trick_city_a_motion_joint_39_angles[99];
static NJS_MKEY_A rider_trick_city_a_motion_joint_41_angles[146];
static NJS_MKEY_A rider_trick_city_a_motion_joint_44_angles[146];
static NJS_MKEY_A rider_trick_city_a_motion_joint_51_angles[2];
static NJS_MKEY_A rider_trick_city_a_motion_joint_53_angles[2];
static NJS_MKEY_A rider_trick_city_a_motion_joint_56_angles[146];
static NJS_MKEY_A rider_trick_city_a_motion_joint_57_angles[37];
static NJS_MKEY_A rider_trick_city_a_motion_joint_58_angles[37];
static NJS_MKEY_A rider_trick_city_a_motion_joint_60_angles[146];
static NJS_MDATA2 rider_trick_city_a_motion_tracks[62];
static NJS_MOTION rider_trick_city_a_motion;
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
static NJS_MKEY_F board_trick_city_a_motion_joint_0_position[146];
static NJS_MKEY_A board_trick_city_a_motion_joint_0_angles[38];
static NJS_MKEY_A board_trick_city_a_motion_joint_1_angles[146];
static NJS_MDATA2 board_trick_city_a_motion_tracks[2];
static NJS_MOTION board_trick_city_a_motion;
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
struct motion_entry module_motions[39];
static struct land_entry entries[2739];
struct land_table city_escape_land;
static NJS_TEXNAME textures[124];
NJS_TEXLIST city_escape_land_texlist;
static NJS_MATERIAL collision_0_materials[1];
static Sint16 collision_0_faces[9];
static NJS_MESHSET collision_0_meshsets[1];
static NJS_POINT3 collision_0_points[8];
static NJS_MODEL collision_0_model;
static NJS_OBJECT collision_0;
static NJS_MATERIAL collision_1_materials[1];
static Sint16 collision_1_faces[57];
static NJS_MESHSET collision_1_meshsets[1];
static NJS_POINT3 collision_1_points[28];
static NJS_MODEL collision_1_model;
static NJS_OBJECT collision_1;
static NJS_MATERIAL collision_2_materials[1];
static Sint16 collision_2_faces[17];
static NJS_MESHSET collision_2_meshsets[1];
static NJS_POINT3 collision_2_points[16];
static NJS_MODEL collision_2_model;
static NJS_OBJECT collision_2;
static NJS_MATERIAL collision_3_materials[1];
static Sint16 collision_3_faces[9];
static NJS_MESHSET collision_3_meshsets[1];
static NJS_POINT3 collision_3_points[8];
static NJS_MODEL collision_3_model;
static NJS_OBJECT collision_3;
static NJS_MATERIAL collision_4_materials[1];
static Sint16 collision_4_faces[41];
static NJS_MESHSET collision_4_meshsets[1];
static NJS_POINT3 collision_4_points[27];
static NJS_MODEL collision_4_model;
static NJS_OBJECT collision_4;
static NJS_MATERIAL collision_5_materials[1];
static Sint16 collision_5_faces[18];
static NJS_MESHSET collision_5_meshsets[1];
static NJS_POINT3 collision_5_points[14];
static NJS_MODEL collision_5_model;
static NJS_OBJECT collision_5;
static NJS_MATERIAL collision_6_materials[1];
static Sint16 collision_6_faces[23];
static NJS_MESHSET collision_6_meshsets[1];
static NJS_POINT3 collision_6_points[16];
static NJS_MODEL collision_6_model;
static NJS_OBJECT collision_6;
static NJS_MATERIAL collision_7_materials[1];
static Sint16 collision_7_faces[64];
static NJS_MESHSET collision_7_meshsets[1];
static NJS_POINT3 collision_7_points[35];
static NJS_MODEL collision_7_model;
static NJS_OBJECT collision_7;
static NJS_MATERIAL collision_8_materials[1];
static Sint16 collision_8_faces[22];
static NJS_MESHSET collision_8_meshsets[1];
static NJS_POINT3 collision_8_points[17];
static NJS_MODEL collision_8_model;
static NJS_OBJECT collision_8;
static NJS_MATERIAL collision_9_materials[1];
static Sint16 collision_9_faces[76];
static NJS_MESHSET collision_9_meshsets[1];
static NJS_POINT3 collision_9_points[45];
static NJS_MODEL collision_9_model;
static NJS_OBJECT collision_9;
static NJS_MATERIAL collision_10_materials[1];
static Sint16 collision_10_faces[22];
static NJS_MESHSET collision_10_meshsets[1];
static NJS_POINT3 collision_10_points[13];
static NJS_MODEL collision_10_model;
static NJS_OBJECT collision_10;
static NJS_MATERIAL collision_11_materials[1];
static Sint16 collision_11_faces[102];
static NJS_MESHSET collision_11_meshsets[1];
static NJS_POINT3 collision_11_points[46];
static NJS_MODEL collision_11_model;
static NJS_OBJECT collision_11;
static NJS_MATERIAL collision_12_materials[1];
static Sint16 collision_12_faces[18];
static NJS_MESHSET collision_12_meshsets[1];
static NJS_POINT3 collision_12_points[12];
static NJS_MODEL collision_12_model;
static NJS_OBJECT collision_12;
static NJS_MATERIAL collision_13_materials[1];
static Sint16 collision_13_faces[9];
static NJS_MESHSET collision_13_meshsets[1];
static NJS_POINT3 collision_13_points[8];
static NJS_MODEL collision_13_model;
static NJS_OBJECT collision_13;
static NJS_MATERIAL collision_14_materials[1];
static Sint16 collision_14_faces[7];
static NJS_MESHSET collision_14_meshsets[1];
static NJS_POINT3 collision_14_points[6];
static NJS_MODEL collision_14_model;
static NJS_OBJECT collision_14;
static NJS_MATERIAL collision_15_materials[1];
static Sint16 collision_15_faces[13];
static NJS_MESHSET collision_15_meshsets[1];
static NJS_POINT3 collision_15_points[12];
static NJS_MODEL collision_15_model;
static NJS_OBJECT collision_15;
static NJS_MATERIAL collision_16_materials[1];
static Sint16 collision_16_faces[35];
static NJS_MESHSET collision_16_meshsets[1];
static NJS_POINT3 collision_16_points[30];
static NJS_MODEL collision_16_model;
static NJS_OBJECT collision_16;
static NJS_MATERIAL collision_17_materials[1];
static Sint16 collision_17_faces[35];
static NJS_MESHSET collision_17_meshsets[1];
static NJS_POINT3 collision_17_points[30];
static NJS_MODEL collision_17_model;
static NJS_OBJECT collision_17;
static NJS_MATERIAL collision_18_materials[1];
static Sint16 collision_18_faces[32];
static NJS_MESHSET collision_18_meshsets[1];
static NJS_POINT3 collision_18_points[30];
static NJS_MODEL collision_18_model;
static NJS_OBJECT collision_18;
static NJS_MATERIAL collision_20_materials[1];
static Sint16 collision_20_faces[30];
static NJS_MESHSET collision_20_meshsets[1];
static NJS_POINT3 collision_20_points[28];
static NJS_MODEL collision_20_model;
static NJS_OBJECT collision_20;
static NJS_MATERIAL collision_21_materials[1];
static Sint16 collision_21_faces[60];
static NJS_MESHSET collision_21_meshsets[1];
static NJS_POINT3 collision_21_points[35];
static NJS_MODEL collision_21_model;
static NJS_OBJECT collision_21;
static NJS_MATERIAL collision_22_materials[1];
static Sint16 collision_22_faces[44];
static NJS_MESHSET collision_22_meshsets[1];
static NJS_POINT3 collision_22_points[36];
static NJS_MODEL collision_22_model;
static NJS_OBJECT collision_22;
static NJS_MATERIAL collision_24_materials[1];
static Sint16 collision_24_faces[85];
static NJS_MESHSET collision_24_meshsets[1];
static NJS_POINT3 collision_24_points[45];
static NJS_MODEL collision_24_model;
static NJS_OBJECT collision_24;
static NJS_MATERIAL collision_28_materials[1];
static Sint16 collision_28_faces[44];
static NJS_MESHSET collision_28_meshsets[1];
static NJS_POINT3 collision_28_points[36];
static NJS_MODEL collision_28_model;
static NJS_OBJECT collision_28;
static NJS_MATERIAL collision_29_materials[1];
static Sint16 collision_29_faces[66];
static NJS_MESHSET collision_29_meshsets[1];
static NJS_POINT3 collision_29_points[36];
static NJS_MODEL collision_29_model;
static NJS_OBJECT collision_29;
static NJS_MATERIAL collision_30_materials[1];
static Sint16 collision_30_faces[85];
static NJS_MESHSET collision_30_meshsets[1];
static NJS_POINT3 collision_30_points[45];
static NJS_MODEL collision_30_model;
static NJS_OBJECT collision_30;
static NJS_MATERIAL collision_32_materials[1];
static Sint16 collision_32_faces[38];
static NJS_MESHSET collision_32_meshsets[1];
static NJS_POINT3 collision_32_points[36];
static NJS_MODEL collision_32_model;
static NJS_OBJECT collision_32;
static NJS_MATERIAL collision_33_materials[1];
static Sint16 collision_33_faces[30];
static NJS_MESHSET collision_33_meshsets[1];
static NJS_POINT3 collision_33_points[28];
static NJS_MODEL collision_33_model;
static NJS_OBJECT collision_33;
static NJS_MATERIAL collision_34_materials[1];
static Sint16 collision_34_faces[60];
static NJS_MESHSET collision_34_meshsets[1];
static NJS_POINT3 collision_34_points[41];
static NJS_MODEL collision_34_model;
static NJS_OBJECT collision_34;
static NJS_MATERIAL collision_35_materials[1];
static Sint16 collision_35_faces[60];
static NJS_MESHSET collision_35_meshsets[1];
static NJS_POINT3 collision_35_points[35];
static NJS_MODEL collision_35_model;
static NJS_OBJECT collision_35;
static NJS_MATERIAL collision_36_materials[1];
static Sint16 collision_36_faces[48];
static NJS_MESHSET collision_36_meshsets[1];
static NJS_POINT3 collision_36_points[40];
static NJS_MODEL collision_36_model;
static NJS_OBJECT collision_36;
static NJS_MATERIAL collision_37_materials[1];
static Sint16 collision_37_faces[48];
static NJS_MESHSET collision_37_meshsets[1];
static NJS_POINT3 collision_37_points[40];
static NJS_MODEL collision_37_model;
static NJS_OBJECT collision_37;
static NJS_MATERIAL collision_38_materials[1];
static Sint16 collision_38_faces[48];
static NJS_MESHSET collision_38_meshsets[1];
static NJS_POINT3 collision_38_points[40];
static NJS_MODEL collision_38_model;
static NJS_OBJECT collision_38;
static NJS_MATERIAL collision_39_materials[1];
static Sint16 collision_39_faces[38];
static NJS_MESHSET collision_39_meshsets[1];
static NJS_POINT3 collision_39_points[36];
static NJS_MODEL collision_39_model;
static NJS_OBJECT collision_39;
static NJS_MATERIAL collision_40_materials[1];
static Sint16 collision_40_faces[48];
static NJS_MESHSET collision_40_meshsets[1];
static NJS_POINT3 collision_40_points[40];
static NJS_MODEL collision_40_model;
static NJS_OBJECT collision_40;
static NJS_MATERIAL collision_41_materials[1];
static Sint16 collision_41_faces[38];
static NJS_MESHSET collision_41_meshsets[1];
static NJS_POINT3 collision_41_points[36];
static NJS_MODEL collision_41_model;
static NJS_OBJECT collision_41;
static NJS_MATERIAL collision_42_materials[1];
static Sint16 collision_42_faces[33];
static NJS_MESHSET collision_42_meshsets[1];
static NJS_POINT3 collision_42_points[21];
static NJS_MODEL collision_42_model;
static NJS_OBJECT collision_42;
static NJS_MATERIAL collision_43_materials[1];
static Sint16 collision_43_faces[44];
static NJS_MESHSET collision_43_meshsets[1];
static NJS_POINT3 collision_43_points[36];
static NJS_MODEL collision_43_model;
static NJS_OBJECT collision_43;
static NJS_MATERIAL collision_45_materials[1];
static Sint16 collision_45_faces[42];
static NJS_MESHSET collision_45_meshsets[1];
static NJS_POINT3 collision_45_points[40];
static NJS_MODEL collision_45_model;
static NJS_OBJECT collision_45;
static NJS_MATERIAL collision_47_materials[1];
static Sint16 collision_47_faces[44];
static NJS_MESHSET collision_47_meshsets[1];
static NJS_POINT3 collision_47_points[36];
static NJS_MODEL collision_47_model;
static NJS_OBJECT collision_47;
static NJS_MATERIAL collision_48_materials[1];
static Sint16 collision_48_faces[42];
static NJS_MESHSET collision_48_meshsets[1];
static NJS_POINT3 collision_48_points[40];
static NJS_MODEL collision_48_model;
static NJS_OBJECT collision_48;
static NJS_MATERIAL collision_49_materials[1];
static Sint16 collision_49_faces[66];
static NJS_MESHSET collision_49_meshsets[1];
static NJS_POINT3 collision_49_points[36];
static NJS_MODEL collision_49_model;
static NJS_OBJECT collision_49;
static NJS_MATERIAL collision_50_materials[1];
static Sint16 collision_50_faces[88];
static NJS_MESHSET collision_50_meshsets[1];
static NJS_POINT3 collision_50_points[45];
static NJS_MODEL collision_50_model;
static NJS_OBJECT collision_50;
static NJS_MATERIAL collision_52_materials[1];
static Sint16 collision_52_faces[63];
static NJS_MESHSET collision_52_meshsets[1];
static NJS_POINT3 collision_52_points[40];
static NJS_MODEL collision_52_model;
static NJS_OBJECT collision_52;
static NJS_MATERIAL collision_53_materials[1];
static Sint16 collision_53_faces[84];
static NJS_MESHSET collision_53_meshsets[1];
static NJS_POINT3 collision_53_points[50];
static NJS_MODEL collision_53_model;
static NJS_OBJECT collision_53;
static NJS_MATERIAL collision_55_materials[1];
static Sint16 collision_55_faces[31];
static NJS_MESHSET collision_55_meshsets[1];
static NJS_POINT3 collision_55_points[28];
static NJS_MODEL collision_55_model;
static NJS_OBJECT collision_55;
static NJS_MATERIAL collision_56_materials[1];
static Sint16 collision_56_faces[44];
static NJS_MESHSET collision_56_meshsets[1];
static NJS_POINT3 collision_56_points[36];
static NJS_MODEL collision_56_model;
static NJS_OBJECT collision_56;
static NJS_MATERIAL collision_57_materials[1];
static Sint16 collision_57_faces[66];
static NJS_MESHSET collision_57_meshsets[1];
static NJS_POINT3 collision_57_points[36];
static NJS_MODEL collision_57_model;
static NJS_OBJECT collision_57;
static NJS_MATERIAL collision_58_materials[1];
static Sint16 collision_58_faces[85];
static NJS_MESHSET collision_58_meshsets[1];
static NJS_POINT3 collision_58_points[45];
static NJS_MODEL collision_58_model;
static NJS_OBJECT collision_58;
static NJS_MATERIAL collision_60_materials[1];
static Sint16 collision_60_faces[92];
static NJS_MESHSET collision_60_meshsets[1];
static NJS_POINT3 collision_60_points[45];
static NJS_MODEL collision_60_model;
static NJS_OBJECT collision_60;
static NJS_MATERIAL collision_62_materials[1];
static Sint16 collision_62_faces[10];
static NJS_MESHSET collision_62_meshsets[1];
static NJS_POINT3 collision_62_points[6];
static NJS_MODEL collision_62_model;
static NJS_OBJECT collision_62;
static NJS_MATERIAL collision_63_materials[1];
static Sint16 collision_63_faces[5];
static NJS_MESHSET collision_63_meshsets[1];
static NJS_POINT3 collision_63_points[4];
static NJS_MODEL collision_63_model;
static NJS_OBJECT collision_63;
static NJS_MATERIAL collision_66_materials[1];
static Sint16 collision_66_faces[10];
static NJS_MESHSET collision_66_meshsets[1];
static NJS_POINT3 collision_66_points[6];
static NJS_MODEL collision_66_model;
static NJS_OBJECT collision_66;
static NJS_MATERIAL collision_69_materials[1];
static Sint16 collision_69_faces[5];
static NJS_MESHSET collision_69_meshsets[1];
static NJS_POINT3 collision_69_points[4];
static NJS_MODEL collision_69_model;
static NJS_OBJECT collision_69;
static NJS_MATERIAL collision_70_materials[1];
static Sint16 collision_70_faces[10];
static NJS_MESHSET collision_70_meshsets[1];
static NJS_POINT3 collision_70_points[6];
static NJS_MODEL collision_70_model;
static NJS_OBJECT collision_70;
static NJS_MATERIAL collision_71_materials[1];
static Sint16 collision_71_faces[57];
static NJS_MESHSET collision_71_meshsets[1];
static NJS_POINT3 collision_71_points[35];
static NJS_MODEL collision_71_model;
static NJS_OBJECT collision_71;
static NJS_MATERIAL collision_72_materials[1];
static Sint16 collision_72_faces[76];
static NJS_MESHSET collision_72_meshsets[1];
static NJS_POINT3 collision_72_points[54];
static NJS_MODEL collision_72_model;
static NJS_OBJECT collision_72;
static NJS_MATERIAL collision_73_materials[1];
static Sint16 collision_73_faces[76];
static NJS_MESHSET collision_73_meshsets[1];
static NJS_POINT3 collision_73_points[54];
static NJS_MODEL collision_73_model;
static NJS_OBJECT collision_73;
static NJS_MATERIAL collision_74_materials[1];
static Sint16 collision_74_faces[60];
static NJS_MESHSET collision_74_meshsets[1];
static NJS_POINT3 collision_74_points[42];
static NJS_MODEL collision_74_model;
static NJS_OBJECT collision_74;
static NJS_MATERIAL collision_75_materials[1];
static Sint16 collision_75_faces[60];
static NJS_MESHSET collision_75_meshsets[1];
static NJS_POINT3 collision_75_points[42];
static NJS_MODEL collision_75_model;
static NJS_OBJECT collision_75;
static NJS_MATERIAL collision_76_materials[1];
static Sint16 collision_76_faces[32];
static NJS_MESHSET collision_76_meshsets[1];
static NJS_POINT3 collision_76_points[30];
static NJS_MODEL collision_76_model;
static NJS_OBJECT collision_76;
static NJS_MATERIAL collision_78_materials[1];
static Sint16 collision_78_faces[17];
static NJS_MESHSET collision_78_meshsets[1];
static NJS_POINT3 collision_78_points[12];
static NJS_MODEL collision_78_model;
static NJS_OBJECT collision_78;
static NJS_MATERIAL collision_79_materials[1];
static Sint16 collision_79_faces[84];
static NJS_MESHSET collision_79_meshsets[1];
static NJS_POINT3 collision_79_points[60];
static NJS_MODEL collision_79_model;
static NJS_OBJECT collision_79;
static NJS_MATERIAL collision_80_materials[1];
static Sint16 collision_80_faces[10];
static NJS_MESHSET collision_80_meshsets[1];
static NJS_POINT3 collision_80_points[6];
static NJS_MODEL collision_80_model;
static NJS_OBJECT collision_80;
static NJS_MATERIAL collision_81_materials[1];
static Sint16 collision_81_faces[84];
static NJS_MESHSET collision_81_meshsets[1];
static NJS_POINT3 collision_81_points[60];
static NJS_MODEL collision_81_model;
static NJS_OBJECT collision_81;
static NJS_MATERIAL collision_82_materials[1];
static Sint16 collision_82_faces[10];
static NJS_MESHSET collision_82_meshsets[1];
static NJS_POINT3 collision_82_points[6];
static NJS_MODEL collision_82_model;
static NJS_OBJECT collision_82;
static NJS_MATERIAL collision_83_materials[1];
static Sint16 collision_83_faces[32];
static NJS_MESHSET collision_83_meshsets[1];
static NJS_POINT3 collision_83_points[30];
static NJS_MODEL collision_83_model;
static NJS_OBJECT collision_83;
static NJS_MATERIAL collision_84_materials[1];
static Sint16 collision_84_faces[73];
static NJS_MESHSET collision_84_meshsets[1];
static NJS_POINT3 collision_84_points[43];
static NJS_MODEL collision_84_model;
static NJS_OBJECT collision_84;
static NJS_MATERIAL collision_85_materials[1];
static Sint16 collision_85_faces[99];
static NJS_MESHSET collision_85_meshsets[1];
static NJS_POINT3 collision_85_points[60];
static NJS_MODEL collision_85_model;
static NJS_OBJECT collision_85;
static NJS_MATERIAL collision_86_materials[1];
static Sint16 collision_86_faces[39];
static NJS_MESHSET collision_86_meshsets[1];
static NJS_POINT3 collision_86_points[38];
static NJS_MODEL collision_86_model;
static NJS_OBJECT collision_86;
static NJS_MATERIAL collision_87_materials[1];
static Sint16 collision_87_faces[99];
static NJS_MESHSET collision_87_meshsets[1];
static NJS_POINT3 collision_87_points[60];
static NJS_MODEL collision_87_model;
static NJS_OBJECT collision_87;
static NJS_MATERIAL collision_88_materials[1];
static Sint16 collision_88_faces[76];
static NJS_MESHSET collision_88_meshsets[1];
static NJS_POINT3 collision_88_points[54];
static NJS_MODEL collision_88_model;
static NJS_OBJECT collision_88;
static NJS_MATERIAL collision_89_materials[1];
static Sint16 collision_89_faces[76];
static NJS_MESHSET collision_89_meshsets[1];
static NJS_POINT3 collision_89_points[54];
static NJS_MODEL collision_89_model;
static NJS_OBJECT collision_89;
static NJS_MATERIAL collision_90_materials[1];
static Sint16 collision_90_faces[32];
static NJS_MESHSET collision_90_meshsets[1];
static NJS_POINT3 collision_90_points[30];
static NJS_MODEL collision_90_model;
static NJS_OBJECT collision_90;
static NJS_MATERIAL collision_91_materials[1];
static Sint16 collision_91_faces[34];
static NJS_MESHSET collision_91_meshsets[1];
static NJS_POINT3 collision_91_points[26];
static NJS_MODEL collision_91_model;
static NJS_OBJECT collision_91;
static NJS_MATERIAL collision_92_materials[1];
static Sint16 collision_92_faces[60];
static NJS_MESHSET collision_92_meshsets[1];
static NJS_POINT3 collision_92_points[42];
static NJS_MODEL collision_92_model;
static NJS_OBJECT collision_92;
static NJS_MATERIAL collision_93_materials[1];
static Sint16 collision_93_faces[7];
static NJS_MESHSET collision_93_meshsets[1];
static NJS_POINT3 collision_93_points[6];
static NJS_MODEL collision_93_model;
static NJS_OBJECT collision_93;
static NJS_MATERIAL collision_101_materials[1];
static Sint16 collision_101_faces[60];
static NJS_MESHSET collision_101_meshsets[1];
static NJS_POINT3 collision_101_points[42];
static NJS_MODEL collision_101_model;
static NJS_OBJECT collision_101;
static NJS_MATERIAL collision_102_materials[1];
static Sint16 collision_102_faces[73];
static NJS_MESHSET collision_102_meshsets[1];
static NJS_POINT3 collision_102_points[46];
static NJS_MODEL collision_102_model;
static NJS_OBJECT collision_102;
static NJS_MATERIAL collision_105_materials[1];
static Sint16 collision_105_faces[84];
static NJS_MESHSET collision_105_meshsets[1];
static NJS_POINT3 collision_105_points[60];
static NJS_MODEL collision_105_model;
static NJS_OBJECT collision_105;
static NJS_MATERIAL collision_108_materials[1];
static Sint16 collision_108_faces[66];
static NJS_MESHSET collision_108_meshsets[1];
static NJS_POINT3 collision_108_points[42];
static NJS_MODEL collision_108_model;
static NJS_OBJECT collision_108;
static NJS_MATERIAL collision_111_materials[1];
static Sint16 collision_111_faces[66];
static NJS_MESHSET collision_111_meshsets[1];
static NJS_POINT3 collision_111_points[35];
static NJS_MODEL collision_111_model;
static NJS_OBJECT collision_111;
static NJS_MATERIAL collision_113_materials[1];
static Sint16 collision_113_faces[84];
static NJS_MESHSET collision_113_meshsets[1];
static NJS_POINT3 collision_113_points[50];
static NJS_MODEL collision_113_model;
static NJS_OBJECT collision_113;
static NJS_MATERIAL collision_115_materials[1];
static Sint16 collision_115_faces[63];
static NJS_MESHSET collision_115_meshsets[1];
static NJS_POINT3 collision_115_points[35];
static NJS_MODEL collision_115_model;
static NJS_OBJECT collision_115;
static NJS_MATERIAL collision_118_materials[1];
static Sint16 collision_118_faces[44];
static NJS_MESHSET collision_118_meshsets[1];
static NJS_POINT3 collision_118_points[36];
static NJS_MODEL collision_118_model;
static NJS_OBJECT collision_118;
static NJS_MATERIAL collision_120_materials[1];
static Sint16 collision_120_faces[63];
static NJS_MESHSET collision_120_meshsets[1];
static NJS_POINT3 collision_120_points[44];
static NJS_MODEL collision_120_model;
static NJS_OBJECT collision_120;
static NJS_MATERIAL collision_121_materials[1];
static Sint16 collision_121_faces[67];
static NJS_MESHSET collision_121_meshsets[1];
static NJS_POINT3 collision_121_points[40];
static NJS_MODEL collision_121_model;
static NJS_OBJECT collision_121;
static NJS_MATERIAL collision_122_materials[1];
static Sint16 collision_122_faces[88];
static NJS_MESHSET collision_122_meshsets[1];
static NJS_POINT3 collision_122_points[54];
static NJS_MODEL collision_122_model;
static NJS_OBJECT collision_122;
static NJS_MATERIAL collision_125_materials[1];
static Sint16 collision_125_faces[88];
static NJS_MESHSET collision_125_meshsets[1];
static NJS_POINT3 collision_125_points[54];
static NJS_MODEL collision_125_model;
static NJS_OBJECT collision_125;
static NJS_MATERIAL collision_128_materials[1];
static Sint16 collision_128_faces[84];
static NJS_MESHSET collision_128_meshsets[1];
static NJS_POINT3 collision_128_points[60];
static NJS_MODEL collision_128_model;
static NJS_OBJECT collision_128;
static NJS_MATERIAL collision_131_materials[1];
static Sint16 collision_131_faces[84];
static NJS_MESHSET collision_131_meshsets[1];
static NJS_POINT3 collision_131_points[60];
static NJS_MODEL collision_131_model;
static NJS_OBJECT collision_131;
static NJS_MATERIAL collision_134_materials[1];
static Sint16 collision_134_faces[88];
static NJS_MESHSET collision_134_meshsets[1];
static NJS_POINT3 collision_134_points[54];
static NJS_MODEL collision_134_model;
static NJS_OBJECT collision_134;
static NJS_MATERIAL collision_137_materials[1];
static Sint16 collision_137_faces[23];
static NJS_MESHSET collision_137_meshsets[1];
static NJS_POINT3 collision_137_points[22];
static NJS_MODEL collision_137_model;
static NJS_OBJECT collision_137;
static NJS_MATERIAL collision_141_materials[1];
static Sint16 collision_141_faces[88];
static NJS_MESHSET collision_141_meshsets[1];
static NJS_POINT3 collision_141_points[54];
static NJS_MODEL collision_141_model;
static NJS_OBJECT collision_141;
static NJS_MATERIAL collision_144_materials[1];
static Sint16 collision_144_faces[73];
static NJS_MESHSET collision_144_meshsets[1];
static NJS_POINT3 collision_144_points[46];
static NJS_MODEL collision_144_model;
static NJS_OBJECT collision_144;
static NJS_MATERIAL collision_146_materials[1];
static Sint16 collision_146_faces[66];
static NJS_MESHSET collision_146_meshsets[1];
static NJS_POINT3 collision_146_points[42];
static NJS_MODEL collision_146_model;
static NJS_OBJECT collision_146;
static NJS_MATERIAL collision_148_materials[1];
static Sint16 collision_148_faces[28];
static NJS_MESHSET collision_148_meshsets[1];
static NJS_POINT3 collision_148_points[18];
static NJS_MODEL collision_148_model;
static NJS_OBJECT collision_148;
static NJS_MATERIAL collision_150_materials[1];
static Sint16 collision_150_faces[43];
static NJS_MESHSET collision_150_meshsets[1];
static NJS_POINT3 collision_150_points[25];
static NJS_MODEL collision_150_model;
static NJS_OBJECT collision_150;
static NJS_MATERIAL collision_152_materials[1];
static Sint16 collision_152_faces[63];
static NJS_MESHSET collision_152_meshsets[1];
static NJS_POINT3 collision_152_points[35];
static NJS_MODEL collision_152_model;
static NJS_OBJECT collision_152;
static NJS_MATERIAL collision_154_materials[1];
static Sint16 collision_154_faces[64];
static NJS_MESHSET collision_154_meshsets[1];
static NJS_POINT3 collision_154_points[36];
static NJS_MODEL collision_154_model;
static NJS_OBJECT collision_154;
static NJS_MATERIAL collision_156_materials[1];
static Sint16 collision_156_faces[12];
static NJS_MESHSET collision_156_meshsets[1];
static NJS_POINT3 collision_156_points[10];
static NJS_MODEL collision_156_model;
static NJS_OBJECT collision_156;
static NJS_MATERIAL collision_157_materials[1];
static Sint16 collision_157_faces[21];
static NJS_MESHSET collision_157_meshsets[1];
static NJS_POINT3 collision_157_points[15];
static NJS_MODEL collision_157_model;
static NJS_OBJECT collision_157;
static NJS_MATERIAL collision_158_materials[1];
static Sint16 collision_158_faces[21];
static NJS_MESHSET collision_158_meshsets[1];
static NJS_POINT3 collision_158_points[15];
static NJS_MODEL collision_158_model;
static NJS_OBJECT collision_158;
static NJS_MATERIAL collision_159_materials[1];
static Sint16 collision_159_faces[37];
static NJS_MESHSET collision_159_meshsets[1];
static NJS_POINT3 collision_159_points[24];
static NJS_MODEL collision_159_model;
static NJS_OBJECT collision_159;
static NJS_MATERIAL collision_161_materials[1];
static Sint16 collision_161_faces[12];
static NJS_MESHSET collision_161_meshsets[1];
static NJS_POINT3 collision_161_points[10];
static NJS_MODEL collision_161_model;
static NJS_OBJECT collision_161;
static NJS_MATERIAL collision_162_materials[1];
static Sint16 collision_162_faces[37];
static NJS_MESHSET collision_162_meshsets[1];
static NJS_POINT3 collision_162_points[24];
static NJS_MODEL collision_162_model;
static NJS_OBJECT collision_162;
static NJS_MATERIAL collision_163_materials[1];
static Sint16 collision_163_faces[37];
static NJS_MESHSET collision_163_meshsets[1];
static NJS_POINT3 collision_163_points[24];
static NJS_MODEL collision_163_model;
static NJS_OBJECT collision_163;
static NJS_MATERIAL collision_164_materials[1];
static Sint16 collision_164_faces[28];
static NJS_MESHSET collision_164_meshsets[1];
static NJS_POINT3 collision_164_points[18];
static NJS_MODEL collision_164_model;
static NJS_OBJECT collision_164;
static NJS_MATERIAL collision_166_materials[1];
static Sint16 collision_166_faces[28];
static NJS_MESHSET collision_166_meshsets[1];
static NJS_POINT3 collision_166_points[18];
static NJS_MODEL collision_166_model;
static NJS_OBJECT collision_166;
static NJS_MATERIAL collision_172_materials[1];
static Sint16 collision_172_faces[91];
static NJS_MESHSET collision_172_meshsets[1];
static NJS_POINT3 collision_172_points[54];
static NJS_MODEL collision_172_model;
static NJS_OBJECT collision_172;
static NJS_MATERIAL collision_174_materials[1];
static Sint16 collision_174_faces[40];
static NJS_MESHSET collision_174_meshsets[1];
static NJS_POINT3 collision_174_points[21];
static NJS_MODEL collision_174_model;
static NJS_OBJECT collision_174;
static NJS_MATERIAL collision_175_materials[1];
static Sint16 collision_175_faces[91];
static NJS_MESHSET collision_175_meshsets[1];
static NJS_POINT3 collision_175_points[54];
static NJS_MODEL collision_175_model;
static NJS_OBJECT collision_175;
static NJS_MATERIAL collision_177_materials[1];
static Sint16 collision_177_faces[44];
static NJS_MESHSET collision_177_meshsets[1];
static NJS_POINT3 collision_177_points[27];
static NJS_MODEL collision_177_model;
static NJS_OBJECT collision_177;
static NJS_MATERIAL collision_178_materials[1];
static Sint16 collision_178_faces[21];
static NJS_MESHSET collision_178_meshsets[1];
static NJS_POINT3 collision_178_points[15];
static NJS_MODEL collision_178_model;
static NJS_OBJECT collision_178;
static NJS_MATERIAL collision_179_materials[1];
static Sint16 collision_179_faces[91];
static NJS_MESHSET collision_179_meshsets[1];
static NJS_POINT3 collision_179_points[54];
static NJS_MODEL collision_179_model;
static NJS_OBJECT collision_179;
static NJS_MATERIAL collision_181_materials[1];
static Sint16 collision_181_faces[23];
static NJS_MESHSET collision_181_meshsets[1];
static NJS_POINT3 collision_181_points[22];
static NJS_MODEL collision_181_model;
static NJS_OBJECT collision_181;
static NJS_MATERIAL collision_185_materials[1];
static Sint16 collision_185_faces[91];
static NJS_MESHSET collision_185_meshsets[1];
static NJS_POINT3 collision_185_points[54];
static NJS_MODEL collision_185_model;
static NJS_OBJECT collision_185;
static NJS_MATERIAL collision_187_materials[1];
static Sint16 collision_187_faces[53];
static NJS_MESHSET collision_187_meshsets[1];
static NJS_POINT3 collision_187_points[26];
static NJS_MODEL collision_187_model;
static NJS_OBJECT collision_187;
static NJS_MATERIAL collision_188_materials[1];
static Sint16 collision_188_faces[28];
static NJS_MESHSET collision_188_meshsets[1];
static NJS_POINT3 collision_188_points[18];
static NJS_MODEL collision_188_model;
static NJS_OBJECT collision_188;
static NJS_MATERIAL collision_189_materials[1];
static Sint16 collision_189_faces[12];
static NJS_MESHSET collision_189_meshsets[1];
static NJS_POINT3 collision_189_points[10];
static NJS_MODEL collision_189_model;
static NJS_OBJECT collision_189;
static NJS_MATERIAL collision_190_materials[1];
static Sint16 collision_190_faces[28];
static NJS_MESHSET collision_190_meshsets[1];
static NJS_POINT3 collision_190_points[18];
static NJS_MODEL collision_190_model;
static NJS_OBJECT collision_190;
static NJS_MATERIAL collision_194_materials[1];
static Sint16 collision_194_faces[28];
static NJS_MESHSET collision_194_meshsets[1];
static NJS_POINT3 collision_194_points[18];
static NJS_MODEL collision_194_model;
static NJS_OBJECT collision_194;
static NJS_MATERIAL collision_196_materials[1];
static Sint16 collision_196_faces[28];
static NJS_MESHSET collision_196_meshsets[1];
static NJS_POINT3 collision_196_points[18];
static NJS_MODEL collision_196_model;
static NJS_OBJECT collision_196;
static NJS_MATERIAL collision_197_materials[1];
static Sint16 collision_197_faces[14];
static NJS_MESHSET collision_197_meshsets[1];
static NJS_POINT3 collision_197_points[12];
static NJS_MODEL collision_197_model;
static NJS_OBJECT collision_197;
static NJS_MATERIAL collision_198_materials[1];
static Sint16 collision_198_faces[36];
static NJS_MESHSET collision_198_meshsets[1];
static NJS_POINT3 collision_198_points[24];
static NJS_MODEL collision_198_model;
static NJS_OBJECT collision_198;
static NJS_MATERIAL collision_199_materials[1];
static Sint16 collision_199_faces[36];
static NJS_MESHSET collision_199_meshsets[1];
static NJS_POINT3 collision_199_points[24];
static NJS_MODEL collision_199_model;
static NJS_OBJECT collision_199;
static NJS_MATERIAL collision_200_materials[1];
static Sint16 collision_200_faces[36];
static NJS_MESHSET collision_200_meshsets[1];
static NJS_POINT3 collision_200_points[24];
static NJS_MODEL collision_200_model;
static NJS_OBJECT collision_200;
static NJS_MATERIAL collision_201_materials[1];
static Sint16 collision_201_faces[33];
static NJS_MESHSET collision_201_meshsets[1];
static NJS_POINT3 collision_201_points[17];
static NJS_MODEL collision_201_model;
static NJS_OBJECT collision_201;
static NJS_MATERIAL collision_202_materials[1];
static Sint16 collision_202_faces[36];
static NJS_MESHSET collision_202_meshsets[1];
static NJS_POINT3 collision_202_points[25];
static NJS_MODEL collision_202_model;
static NJS_OBJECT collision_202;
static NJS_MATERIAL collision_203_materials[1];
static Sint16 collision_203_faces[23];
static NJS_MESHSET collision_203_meshsets[1];
static NJS_POINT3 collision_203_points[11];
static NJS_MODEL collision_203_model;
static NJS_OBJECT collision_203;
static NJS_MATERIAL collision_204_materials[1];
static Sint16 collision_204_faces[28];
static NJS_MESHSET collision_204_meshsets[1];
static NJS_POINT3 collision_204_points[18];
static NJS_MODEL collision_204_model;
static NJS_OBJECT collision_204;
static NJS_MATERIAL collision_205_materials[1];
static Sint16 collision_205_faces[26];
static NJS_MESHSET collision_205_meshsets[1];
static NJS_POINT3 collision_205_points[20];
static NJS_MODEL collision_205_model;
static NJS_OBJECT collision_205;
static NJS_MATERIAL collision_206_materials[1];
static Sint16 collision_206_faces[52];
static NJS_MESHSET collision_206_meshsets[1];
static NJS_POINT3 collision_206_points[35];
static NJS_MODEL collision_206_model;
static NJS_OBJECT collision_206;
static NJS_MATERIAL collision_207_materials[1];
static Sint16 collision_207_faces[37];
static NJS_MESHSET collision_207_meshsets[1];
static NJS_POINT3 collision_207_points[24];
static NJS_MODEL collision_207_model;
static NJS_OBJECT collision_207;
static NJS_MATERIAL collision_223_materials[1];
static Sint16 collision_223_faces[33];
static NJS_MESHSET collision_223_meshsets[1];
static NJS_POINT3 collision_223_points[16];
static NJS_MODEL collision_223_model;
static NJS_OBJECT collision_223;
static NJS_MATERIAL collision_227_materials[1];
static Sint16 collision_227_faces[24];
static NJS_MESHSET collision_227_meshsets[1];
static NJS_POINT3 collision_227_points[16];
static NJS_MODEL collision_227_model;
static NJS_OBJECT collision_227;
static NJS_MATERIAL collision_228_materials[1];
static Sint16 collision_228_faces[14];
static NJS_MESHSET collision_228_meshsets[1];
static NJS_POINT3 collision_228_points[12];
static NJS_MODEL collision_228_model;
static NJS_OBJECT collision_228;
static NJS_MATERIAL collision_229_materials[1];
static Sint16 collision_229_faces[29];
static NJS_MESHSET collision_229_meshsets[1];
static NJS_POINT3 collision_229_points[18];
static NJS_MODEL collision_229_model;
static NJS_OBJECT collision_229;
static NJS_MATERIAL collision_230_materials[1];
static Sint16 collision_230_faces[37];
static NJS_MESHSET collision_230_meshsets[1];
static NJS_POINT3 collision_230_points[24];
static NJS_MODEL collision_230_model;
static NJS_OBJECT collision_230;
static NJS_MATERIAL collision_231_materials[1];
static Sint16 collision_231_faces[62];
static NJS_MESHSET collision_231_meshsets[1];
static NJS_POINT3 collision_231_points[35];
static NJS_MODEL collision_231_model;
static NJS_OBJECT collision_231;
static NJS_MATERIAL collision_232_materials[1];
static Sint16 collision_232_faces[56];
static NJS_MESHSET collision_232_meshsets[1];
static NJS_POINT3 collision_232_points[35];
static NJS_MODEL collision_232_model;
static NJS_OBJECT collision_232;
static NJS_MATERIAL collision_233_materials[1];
static Sint16 collision_233_faces[34];
static NJS_MESHSET collision_233_meshsets[1];
static NJS_POINT3 collision_233_points[28];
static NJS_MODEL collision_233_model;
static NJS_OBJECT collision_233;
static NJS_MATERIAL collision_234_materials[1];
static Sint16 collision_234_faces[52];
static NJS_MESHSET collision_234_meshsets[1];
static NJS_POINT3 collision_234_points[25];
static NJS_MODEL collision_234_model;
static NJS_OBJECT collision_234;
static NJS_MATERIAL collision_235_materials[1];
static Sint16 collision_235_faces[9];
static NJS_MESHSET collision_235_meshsets[1];
static NJS_POINT3 collision_235_points[8];
static NJS_MODEL collision_235_model;
static NJS_OBJECT collision_235;
static NJS_MATERIAL collision_236_materials[1];
static Sint16 collision_236_faces[9];
static NJS_MESHSET collision_236_meshsets[1];
static NJS_POINT3 collision_236_points[8];
static NJS_MODEL collision_236_model;
static NJS_OBJECT collision_236;
static NJS_MATERIAL collision_237_materials[1];
static Sint16 collision_237_faces[18];
static NJS_MESHSET collision_237_meshsets[1];
static NJS_POINT3 collision_237_points[10];
static NJS_MODEL collision_237_model;
static NJS_OBJECT collision_237;
static NJS_MATERIAL collision_238_materials[1];
static Sint16 collision_238_faces[57];
static NJS_MESHSET collision_238_meshsets[1];
static NJS_POINT3 collision_238_points[41];
static NJS_MODEL collision_238_model;
static NJS_OBJECT collision_238;
static NJS_MATERIAL collision_239_materials[1];
static Sint16 collision_239_faces[78];
static NJS_MESHSET collision_239_meshsets[1];
static NJS_POINT3 collision_239_points[45];
static NJS_MODEL collision_239_model;
static NJS_OBJECT collision_239;
static NJS_MATERIAL collision_240_materials[1];
static Sint16 collision_240_faces[10];
static NJS_MESHSET collision_240_meshsets[1];
static NJS_POINT3 collision_240_points[8];
static NJS_MODEL collision_240_model;
static NJS_OBJECT collision_240;
static NJS_MATERIAL collision_241_materials[1];
static Sint16 collision_241_faces[24];
static NJS_MESHSET collision_241_meshsets[1];
static NJS_POINT3 collision_241_points[16];
static NJS_MODEL collision_241_model;
static NJS_OBJECT collision_241;
static NJS_MATERIAL collision_242_materials[1];
static Sint16 collision_242_faces[10];
static NJS_MESHSET collision_242_meshsets[1];
static NJS_POINT3 collision_242_points[8];
static NJS_MODEL collision_242_model;
static NJS_OBJECT collision_242;
static NJS_MATERIAL collision_245_materials[1];
static Sint16 collision_245_faces[94];
static NJS_MESHSET collision_245_meshsets[1];
static NJS_POINT3 collision_245_points[48];
static NJS_MODEL collision_245_model;
static NJS_OBJECT collision_245;
static NJS_MATERIAL collision_253_materials[1];
static Sint16 collision_253_faces[24];
static NJS_MESHSET collision_253_meshsets[1];
static NJS_POINT3 collision_253_points[14];
static NJS_MODEL collision_253_model;
static NJS_OBJECT collision_253;
static NJS_MATERIAL collision_254_materials[1];
static Sint16 collision_254_faces[49];
static NJS_MESHSET collision_254_meshsets[1];
static NJS_POINT3 collision_254_points[26];
static NJS_MODEL collision_254_model;
static NJS_OBJECT collision_254;
static NJS_MATERIAL collision_255_materials[1];
static Sint16 collision_255_faces[94];
static NJS_MESHSET collision_255_meshsets[1];
static NJS_POINT3 collision_255_points[48];
static NJS_MODEL collision_255_model;
static NJS_OBJECT collision_255;
static NJS_MATERIAL collision_263_materials[1];
static Sint16 collision_263_faces[23];
static NJS_MESHSET collision_263_meshsets[1];
static NJS_POINT3 collision_263_points[14];
static NJS_MODEL collision_263_model;
static NJS_OBJECT collision_263;
static NJS_MATERIAL collision_264_materials[1];
static Sint16 collision_264_faces[36];
static NJS_MESHSET collision_264_meshsets[1];
static NJS_POINT3 collision_264_points[24];
static NJS_MODEL collision_264_model;
static NJS_OBJECT collision_264;
static NJS_MATERIAL collision_265_materials[1];
static Sint16 collision_265_faces[24];
static NJS_MESHSET collision_265_meshsets[1];
static NJS_POINT3 collision_265_points[16];
static NJS_MODEL collision_265_model;
static NJS_OBJECT collision_265;
static NJS_MATERIAL collision_266_materials[1];
static Sint16 collision_266_faces[24];
static NJS_MESHSET collision_266_meshsets[1];
static NJS_POINT3 collision_266_points[17];
static NJS_MODEL collision_266_model;
static NJS_OBJECT collision_266;
static NJS_MATERIAL collision_267_materials[1];
static Sint16 collision_267_faces[22];
static NJS_MESHSET collision_267_meshsets[1];
static NJS_POINT3 collision_267_points[13];
static NJS_MODEL collision_267_model;
static NJS_OBJECT collision_267;
static NJS_MATERIAL collision_268_materials[1];
static Sint16 collision_268_faces[15];
static NJS_MESHSET collision_268_meshsets[1];
static NJS_POINT3 collision_268_points[8];
static NJS_MODEL collision_268_model;
static NJS_OBJECT collision_268;
static NJS_MATERIAL collision_269_materials[1];
static Sint16 collision_269_faces[24];
static NJS_MESHSET collision_269_meshsets[1];
static NJS_POINT3 collision_269_points[17];
static NJS_MODEL collision_269_model;
static NJS_OBJECT collision_269;
static NJS_MATERIAL collision_270_materials[1];
static Sint16 collision_270_faces[22];
static NJS_MESHSET collision_270_meshsets[1];
static NJS_POINT3 collision_270_points[13];
static NJS_MODEL collision_270_model;
static NJS_OBJECT collision_270;
static NJS_MATERIAL collision_271_materials[1];
static Sint16 collision_271_faces[11];
static NJS_MESHSET collision_271_meshsets[1];
static NJS_POINT3 collision_271_points[10];
static NJS_MODEL collision_271_model;
static NJS_OBJECT collision_271;
static NJS_MATERIAL collision_272_materials[1];
static Sint16 collision_272_faces[12];
static NJS_MESHSET collision_272_meshsets[1];
static NJS_POINT3 collision_272_points[8];
static NJS_MODEL collision_272_model;
static NJS_OBJECT collision_272;
static NJS_MATERIAL collision_273_materials[1];
static Sint16 collision_273_faces[11];
static NJS_MESHSET collision_273_meshsets[1];
static NJS_POINT3 collision_273_points[10];
static NJS_MODEL collision_273_model;
static NJS_OBJECT collision_273;
static NJS_MATERIAL collision_274_materials[1];
static Sint16 collision_274_faces[15];
static NJS_MESHSET collision_274_meshsets[1];
static NJS_POINT3 collision_274_points[8];
static NJS_MODEL collision_274_model;
static NJS_OBJECT collision_274;
static NJS_MATERIAL collision_276_materials[1];
static Sint16 collision_276_faces[17];
static NJS_MESHSET collision_276_meshsets[1];
static NJS_POINT3 collision_276_points[16];
static NJS_MODEL collision_276_model;
static NJS_OBJECT collision_276;
static NJS_MATERIAL collision_278_materials[1];
static Sint16 collision_278_faces[36];
static NJS_MESHSET collision_278_meshsets[1];
static NJS_POINT3 collision_278_points[24];
static NJS_MODEL collision_278_model;
static NJS_OBJECT collision_278;
static NJS_MATERIAL collision_279_materials[1];
static Sint16 collision_279_faces[36];
static NJS_MESHSET collision_279_meshsets[1];
static NJS_POINT3 collision_279_points[24];
static NJS_MODEL collision_279_model;
static NJS_OBJECT collision_279;
static NJS_MATERIAL collision_281_materials[1];
static Sint16 collision_281_faces[74];
static NJS_MESHSET collision_281_meshsets[1];
static NJS_POINT3 collision_281_points[48];
static NJS_MODEL collision_281_model;
static NJS_OBJECT collision_281;
static NJS_MATERIAL collision_282_materials[1];
static Sint16 collision_282_faces[19];
static NJS_MESHSET collision_282_meshsets[1];
static NJS_POINT3 collision_282_points[11];
static NJS_MODEL collision_282_model;
static NJS_OBJECT collision_282;
static NJS_MATERIAL collision_283_materials[1];
static Sint16 collision_283_faces[28];
static NJS_MESHSET collision_283_meshsets[1];
static NJS_POINT3 collision_283_points[18];
static NJS_MODEL collision_283_model;
static NJS_OBJECT collision_283;
static NJS_MATERIAL collision_284_materials[1];
static Sint16 collision_284_faces[10];
static NJS_MESHSET collision_284_meshsets[1];
static NJS_POINT3 collision_284_points[8];
static NJS_MODEL collision_284_model;
static NJS_OBJECT collision_284;
static NJS_MATERIAL collision_285_materials[1];
static Sint16 collision_285_faces[23];
static NJS_MESHSET collision_285_meshsets[1];
static NJS_POINT3 collision_285_points[13];
static NJS_MODEL collision_285_model;
static NJS_OBJECT collision_285;
static NJS_MATERIAL collision_286_materials[1];
static Sint16 collision_286_faces[19];
static NJS_MESHSET collision_286_meshsets[1];
static NJS_POINT3 collision_286_points[12];
static NJS_MODEL collision_286_model;
static NJS_OBJECT collision_286;
static NJS_MATERIAL collision_287_materials[1];
static Sint16 collision_287_faces[23];
static NJS_MESHSET collision_287_meshsets[1];
static NJS_POINT3 collision_287_points[13];
static NJS_MODEL collision_287_model;
static NJS_OBJECT collision_287;
static NJS_MATERIAL collision_288_materials[1];
static Sint16 collision_288_faces[11];
static NJS_MESHSET collision_288_meshsets[1];
static NJS_POINT3 collision_288_points[8];
static NJS_MODEL collision_288_model;
static NJS_OBJECT collision_288;
static NJS_MATERIAL collision_291_materials[1];
static Sint16 collision_291_faces[7];
static NJS_MESHSET collision_291_meshsets[1];
static NJS_POINT3 collision_291_points[6];
static NJS_MODEL collision_291_model;
static NJS_OBJECT collision_291;
static NJS_MATERIAL collision_296_materials[1];
static Sint16 collision_296_faces[19];
static NJS_MESHSET collision_296_meshsets[1];
static NJS_POINT3 collision_296_points[16];
static NJS_MODEL collision_296_model;
static NJS_OBJECT collision_296;
static NJS_MATERIAL collision_297_materials[1];
static Sint16 collision_297_faces[49];
static NJS_MESHSET collision_297_meshsets[1];
static NJS_POINT3 collision_297_points[25];
static NJS_MODEL collision_297_model;
static NJS_OBJECT collision_297;
static NJS_MATERIAL collision_298_materials[1];
static Sint16 collision_298_faces[29];
static NJS_MESHSET collision_298_meshsets[1];
static NJS_POINT3 collision_298_points[16];
static NJS_MODEL collision_298_model;
static NJS_OBJECT collision_298;
static NJS_MATERIAL collision_299_materials[1];
static Sint16 collision_299_faces[5];
static NJS_MESHSET collision_299_meshsets[1];
static NJS_POINT3 collision_299_points[4];
static NJS_MODEL collision_299_model;
static NJS_OBJECT collision_299;
static NJS_MATERIAL collision_300_materials[1];
static Sint16 collision_300_faces[27];
static NJS_MESHSET collision_300_meshsets[1];
static NJS_POINT3 collision_300_points[23];
static NJS_MODEL collision_300_model;
static NJS_OBJECT collision_300;
static NJS_MATERIAL collision_301_materials[1];
static Sint16 collision_301_faces[12];
static NJS_MESHSET collision_301_meshsets[1];
static NJS_POINT3 collision_301_points[5];
static NJS_MODEL collision_301_model;
static NJS_OBJECT collision_301;
static NJS_MATERIAL collision_306_materials[1];
static Sint16 collision_306_faces[12];
static NJS_MESHSET collision_306_meshsets[1];
static NJS_POINT3 collision_306_points[5];
static NJS_MODEL collision_306_model;
static NJS_OBJECT collision_306;
static NJS_MATERIAL collision_311_materials[1];
static Sint16 collision_311_faces[84];
static NJS_MESHSET collision_311_meshsets[1];
static NJS_POINT3 collision_311_points[48];
static NJS_MODEL collision_311_model;
static NJS_OBJECT collision_311;
static NJS_MATERIAL collision_312_materials[1];
static Sint16 collision_312_faces[7];
static NJS_MESHSET collision_312_meshsets[1];
static NJS_POINT3 collision_312_points[6];
static NJS_MODEL collision_312_model;
static NJS_OBJECT collision_312;
static NJS_MATERIAL collision_313_materials[1];
static Sint16 collision_313_faces[7];
static NJS_MESHSET collision_313_meshsets[1];
static NJS_POINT3 collision_313_points[6];
static NJS_MODEL collision_313_model;
static NJS_OBJECT collision_313;
static NJS_MATERIAL collision_316_materials[1];
static Sint16 collision_316_faces[25];
static NJS_MESHSET collision_316_meshsets[1];
static NJS_POINT3 collision_316_points[21];
static NJS_MODEL collision_316_model;
static NJS_OBJECT collision_316;
static NJS_MATERIAL collision_317_materials[1];
static Sint16 collision_317_faces[38];
static NJS_MESHSET collision_317_meshsets[1];
static NJS_POINT3 collision_317_points[23];
static NJS_MODEL collision_317_model;
static NJS_OBJECT collision_317;
static NJS_MATERIAL collision_318_materials[1];
static Sint16 collision_318_faces[81];
static NJS_MESHSET collision_318_meshsets[1];
static NJS_POINT3 collision_318_points[48];
static NJS_MODEL collision_318_model;
static NJS_OBJECT collision_318;
static NJS_MATERIAL collision_319_materials[1];
static Sint16 collision_319_faces[29];
static NJS_MESHSET collision_319_meshsets[1];
static NJS_POINT3 collision_319_points[14];
static NJS_MODEL collision_319_model;
static NJS_OBJECT collision_319;
static NJS_MATERIAL collision_320_materials[1];
static Sint16 collision_320_faces[7];
static NJS_MESHSET collision_320_meshsets[1];
static NJS_POINT3 collision_320_points[6];
static NJS_MODEL collision_320_model;
static NJS_OBJECT collision_320;
static NJS_MATERIAL collision_321_materials[1];
static Sint16 collision_321_faces[20];
static NJS_MESHSET collision_321_meshsets[1];
static NJS_POINT3 collision_321_points[12];
static NJS_MODEL collision_321_model;
static NJS_OBJECT collision_321;
static NJS_MATERIAL collision_322_materials[1];
static Sint16 collision_322_faces[9];
static NJS_MESHSET collision_322_meshsets[1];
static NJS_POINT3 collision_322_points[8];
static NJS_MODEL collision_322_model;
static NJS_OBJECT collision_322;
static NJS_MATERIAL collision_323_materials[1];
static Sint16 collision_323_faces[7];
static NJS_MESHSET collision_323_meshsets[1];
static NJS_POINT3 collision_323_points[6];
static NJS_MODEL collision_323_model;
static NJS_OBJECT collision_323;
static NJS_MATERIAL collision_324_materials[1];
static Sint16 collision_324_faces[35];
static NJS_MESHSET collision_324_meshsets[1];
static NJS_POINT3 collision_324_points[20];
static NJS_MODEL collision_324_model;
static NJS_OBJECT collision_324;
static NJS_MATERIAL collision_325_materials[1];
static Sint16 collision_325_faces[17];
static NJS_MESHSET collision_325_meshsets[1];
static NJS_POINT3 collision_325_points[11];
static NJS_MODEL collision_325_model;
static NJS_OBJECT collision_325;
static NJS_MATERIAL collision_326_materials[1];
static Sint16 collision_326_faces[35];
static NJS_MESHSET collision_326_meshsets[1];
static NJS_POINT3 collision_326_points[20];
static NJS_MODEL collision_326_model;
static NJS_OBJECT collision_326;
static NJS_MATERIAL collision_327_materials[1];
static Sint16 collision_327_faces[11];
static NJS_MESHSET collision_327_meshsets[1];
static NJS_POINT3 collision_327_points[10];
static NJS_MODEL collision_327_model;
static NJS_OBJECT collision_327;
static NJS_MATERIAL collision_328_materials[1];
static Sint16 collision_328_faces[25];
static NJS_MESHSET collision_328_meshsets[1];
static NJS_POINT3 collision_328_points[14];
static NJS_MODEL collision_328_model;
static NJS_OBJECT collision_328;
static NJS_MATERIAL collision_329_materials[1];
static Sint16 collision_329_faces[22];
static NJS_MESHSET collision_329_meshsets[1];
static NJS_POINT3 collision_329_points[15];
static NJS_MODEL collision_329_model;
static NJS_OBJECT collision_329;
static NJS_MATERIAL collision_330_materials[1];
static Sint16 collision_330_faces[9];
static NJS_MESHSET collision_330_meshsets[1];
static NJS_POINT3 collision_330_points[8];
static NJS_MODEL collision_330_model;
static NJS_OBJECT collision_330;
static NJS_MATERIAL collision_331_materials[1];
static Sint16 collision_331_faces[7];
static NJS_MESHSET collision_331_meshsets[1];
static NJS_POINT3 collision_331_points[6];
static NJS_MODEL collision_331_model;
static NJS_OBJECT collision_331;
static NJS_MATERIAL collision_332_materials[1];
static Sint16 collision_332_faces[18];
static NJS_MESHSET collision_332_meshsets[1];
static NJS_POINT3 collision_332_points[12];
static NJS_MODEL collision_332_model;
static NJS_OBJECT collision_332;
static NJS_MATERIAL collision_333_materials[1];
static Sint16 collision_333_faces[18];
static NJS_MESHSET collision_333_meshsets[1];
static NJS_POINT3 collision_333_points[14];
static NJS_MODEL collision_333_model;
static NJS_OBJECT collision_333;
static NJS_MATERIAL collision_334_materials[1];
static Sint16 collision_334_faces[40];
static NJS_MESHSET collision_334_meshsets[1];
static NJS_POINT3 collision_334_points[24];
static NJS_MODEL collision_334_model;
static NJS_OBJECT collision_334;
static NJS_MATERIAL collision_335_materials[1];
static Sint16 collision_335_faces[39];
static NJS_MESHSET collision_335_meshsets[1];
static NJS_POINT3 collision_335_points[21];
static NJS_MODEL collision_335_model;
static NJS_OBJECT collision_335;
static NJS_MATERIAL collision_336_materials[1];
static Sint16 collision_336_faces[9];
static NJS_MESHSET collision_336_meshsets[1];
static NJS_POINT3 collision_336_points[8];
static NJS_MODEL collision_336_model;
static NJS_OBJECT collision_336;
static NJS_MATERIAL collision_337_materials[1];
static Sint16 collision_337_faces[5];
static NJS_MESHSET collision_337_meshsets[1];
static NJS_POINT3 collision_337_points[4];
static NJS_MODEL collision_337_model;
static NJS_OBJECT collision_337;
static NJS_MATERIAL collision_341_materials[1];
static Sint16 collision_341_faces[117];
static NJS_MESHSET collision_341_meshsets[1];
static NJS_POINT3 collision_341_points[59];
static NJS_MODEL collision_341_model;
static NJS_OBJECT collision_341;
static NJS_MATERIAL collision_342_materials[1];
static Sint16 collision_342_faces[15];
static NJS_MESHSET collision_342_meshsets[1];
static NJS_POINT3 collision_342_points[8];
static NJS_MODEL collision_342_model;
static NJS_OBJECT collision_342;
static NJS_MATERIAL collision_343_materials[1];
static Sint16 collision_343_faces[11];
static NJS_MESHSET collision_343_meshsets[1];
static NJS_POINT3 collision_343_points[10];
static NJS_MODEL collision_343_model;
static NJS_OBJECT collision_343;
static NJS_MATERIAL collision_344_materials[1];
static Sint16 collision_344_faces[7];
static NJS_MESHSET collision_344_meshsets[1];
static NJS_POINT3 collision_344_points[6];
static NJS_MODEL collision_344_model;
static NJS_OBJECT collision_344;
static NJS_MATERIAL collision_345_materials[1];
static Sint16 collision_345_faces[22];
static NJS_MESHSET collision_345_meshsets[1];
static NJS_POINT3 collision_345_points[15];
static NJS_MODEL collision_345_model;
static NJS_OBJECT collision_345;
static NJS_MATERIAL collision_346_materials[1];
static Sint16 collision_346_faces[18];
static NJS_MESHSET collision_346_meshsets[1];
static NJS_POINT3 collision_346_points[14];
static NJS_MODEL collision_346_model;
static NJS_OBJECT collision_346;
static NJS_MATERIAL collision_347_materials[1];
static Sint16 collision_347_faces[13];
static NJS_MESHSET collision_347_meshsets[1];
static NJS_POINT3 collision_347_points[12];
static NJS_MODEL collision_347_model;
static NJS_OBJECT collision_347;
static NJS_MATERIAL collision_349_materials[1];
static Sint16 collision_349_faces[7];
static NJS_MESHSET collision_349_meshsets[1];
static NJS_POINT3 collision_349_points[6];
static NJS_MODEL collision_349_model;
static NJS_OBJECT collision_349;
static NJS_MATERIAL collision_350_materials[1];
static Sint16 collision_350_faces[39];
static NJS_MESHSET collision_350_meshsets[1];
static NJS_POINT3 collision_350_points[21];
static NJS_MODEL collision_350_model;
static NJS_OBJECT collision_350;
static NJS_MATERIAL collision_351_materials[1];
static Sint16 collision_351_faces[9];
static NJS_MESHSET collision_351_meshsets[1];
static NJS_POINT3 collision_351_points[8];
static NJS_MODEL collision_351_model;
static NJS_OBJECT collision_351;
static NJS_MATERIAL collision_352_materials[1];
static Sint16 collision_352_faces[71];
static NJS_MESHSET collision_352_meshsets[1];
static NJS_POINT3 collision_352_points[33];
static NJS_MODEL collision_352_model;
static NJS_OBJECT collision_352;
static NJS_MATERIAL collision_353_materials[1];
static Sint16 collision_353_faces[18];
static NJS_MESHSET collision_353_meshsets[1];
static NJS_POINT3 collision_353_points[12];
static NJS_MODEL collision_353_model;
static NJS_OBJECT collision_353;
static NJS_MATERIAL collision_354_materials[1];
static Sint16 collision_354_faces[18];
static NJS_MESHSET collision_354_meshsets[1];
static NJS_POINT3 collision_354_points[16];
static NJS_MODEL collision_354_model;
static NJS_OBJECT collision_354;
static NJS_MATERIAL collision_355_materials[1];
static Sint16 collision_355_faces[27];
static NJS_MESHSET collision_355_meshsets[1];
static NJS_POINT3 collision_355_points[14];
static NJS_MODEL collision_355_model;
static NJS_OBJECT collision_355;
static NJS_MATERIAL collision_357_materials[1];
static Sint16 collision_357_faces[71];
static NJS_MESHSET collision_357_meshsets[1];
static NJS_POINT3 collision_357_points[33];
static NJS_MODEL collision_357_model;
static NJS_OBJECT collision_357;
static NJS_MATERIAL collision_358_materials[1];
static Sint16 collision_358_faces[27];
static NJS_MESHSET collision_358_meshsets[1];
static NJS_POINT3 collision_358_points[26];
static NJS_MODEL collision_358_model;
static NJS_OBJECT collision_358;
static NJS_MATERIAL collision_359_materials[1];
static Sint16 collision_359_faces[24];
static NJS_MESHSET collision_359_meshsets[1];
static NJS_POINT3 collision_359_points[14];
static NJS_MODEL collision_359_model;
static NJS_OBJECT collision_359;
static NJS_MATERIAL collision_360_materials[1];
static Sint16 collision_360_faces[46];
static NJS_MESHSET collision_360_meshsets[1];
static NJS_POINT3 collision_360_points[20];
static NJS_MODEL collision_360_model;
static NJS_OBJECT collision_360;
static NJS_MATERIAL collision_361_materials[1];
static Sint16 collision_361_faces[18];
static NJS_MESHSET collision_361_meshsets[1];
static NJS_POINT3 collision_361_points[14];
static NJS_MODEL collision_361_model;
static NJS_OBJECT collision_361;
static NJS_MATERIAL collision_363_materials[1];
static Sint16 collision_363_faces[32];
static NJS_MESHSET collision_363_meshsets[1];
static NJS_POINT3 collision_363_points[15];
static NJS_MODEL collision_363_model;
static NJS_OBJECT collision_363;
static NJS_MATERIAL collision_364_materials[1];
static Sint16 collision_364_faces[39];
static NJS_MESHSET collision_364_meshsets[1];
static NJS_POINT3 collision_364_points[21];
static NJS_MODEL collision_364_model;
static NJS_OBJECT collision_364;
static NJS_MATERIAL collision_366_materials[1];
static Sint16 collision_366_faces[7];
static NJS_MESHSET collision_366_meshsets[1];
static NJS_POINT3 collision_366_points[6];
static NJS_MODEL collision_366_model;
static NJS_OBJECT collision_366;
static NJS_MATERIAL collision_367_materials[1];
static Sint16 collision_367_faces[22];
static NJS_MESHSET collision_367_meshsets[1];
static NJS_POINT3 collision_367_points[12];
static NJS_MODEL collision_367_model;
static NJS_OBJECT collision_367;
static NJS_MATERIAL collision_369_materials[1];
static Sint16 collision_369_faces[16];
static NJS_MESHSET collision_369_meshsets[1];
static NJS_POINT3 collision_369_points[12];
static NJS_MODEL collision_369_model;
static NJS_OBJECT collision_369;
static NJS_MATERIAL collision_371_materials[1];
static Sint16 collision_371_faces[35];
static NJS_MESHSET collision_371_meshsets[1];
static NJS_POINT3 collision_371_points[18];
static NJS_MODEL collision_371_model;
static NJS_OBJECT collision_371;
static NJS_MATERIAL collision_373_materials[1];
static Sint16 collision_373_faces[7];
static NJS_MESHSET collision_373_meshsets[1];
static NJS_POINT3 collision_373_points[6];
static NJS_MODEL collision_373_model;
static NJS_OBJECT collision_373;
static NJS_MATERIAL collision_374_materials[1];
static Sint16 collision_374_faces[26];
static NJS_MESHSET collision_374_meshsets[1];
static NJS_POINT3 collision_374_points[15];
static NJS_MODEL collision_374_model;
static NJS_OBJECT collision_374;
static NJS_MATERIAL collision_375_materials[1];
static Sint16 collision_375_faces[150];
static NJS_MESHSET collision_375_meshsets[1];
static NJS_POINT3 collision_375_points[82];
static NJS_MODEL collision_375_model;
static NJS_OBJECT collision_375;
static NJS_MATERIAL collision_376_materials[1];
static Sint16 collision_376_faces[23];
static NJS_MESHSET collision_376_meshsets[1];
static NJS_POINT3 collision_376_points[13];
static NJS_MODEL collision_376_model;
static NJS_OBJECT collision_376;
static NJS_MATERIAL collision_378_materials[1];
static Sint16 collision_378_faces[46];
static NJS_MESHSET collision_378_meshsets[1];
static NJS_POINT3 collision_378_points[24];
static NJS_MODEL collision_378_model;
static NJS_OBJECT collision_378;
static NJS_MATERIAL collision_379_materials[1];
static Sint16 collision_379_faces[269];
static NJS_MESHSET collision_379_meshsets[1];
static NJS_POINT3 collision_379_points[146];
static NJS_MODEL collision_379_model;
static NJS_OBJECT collision_379;
static NJS_MATERIAL collision_380_materials[1];
static Sint16 collision_380_faces[344];
static NJS_MESHSET collision_380_meshsets[1];
static NJS_POINT3 collision_380_points[170];
static NJS_MODEL collision_380_model;
static NJS_OBJECT collision_380;
static NJS_MATERIAL collision_381_materials[1];
static Sint16 collision_381_faces[27];
static NJS_MESHSET collision_381_meshsets[1];
static NJS_POINT3 collision_381_points[26];
static NJS_MODEL collision_381_model;
static NJS_OBJECT collision_381;
static NJS_MATERIAL collision_382_materials[1];
static Sint16 collision_382_faces[17];
static NJS_MESHSET collision_382_meshsets[1];
static NJS_POINT3 collision_382_points[16];
static NJS_MODEL collision_382_model;
static NJS_OBJECT collision_382;
static NJS_MATERIAL collision_383_materials[1];
static Sint16 collision_383_faces[26];
static NJS_MESHSET collision_383_meshsets[1];
static NJS_POINT3 collision_383_points[24];
static NJS_MODEL collision_383_model;
static NJS_OBJECT collision_383;
static NJS_MATERIAL collision_384_materials[1];
static Sint16 collision_384_faces[7];
static NJS_MESHSET collision_384_meshsets[1];
static NJS_POINT3 collision_384_points[6];
static NJS_MODEL collision_384_model;
static NJS_OBJECT collision_384;
static NJS_MATERIAL collision_385_materials[1];
static Sint16 collision_385_faces[61];
static NJS_MESHSET collision_385_meshsets[1];
static NJS_POINT3 collision_385_points[30];
static NJS_MODEL collision_385_model;
static NJS_OBJECT collision_385;
static NJS_MATERIAL collision_387_materials[1];
static Sint16 collision_387_faces[220];
static NJS_MESHSET collision_387_meshsets[1];
static NJS_POINT3 collision_387_points[114];
static NJS_MODEL collision_387_model;
static NJS_OBJECT collision_387;
static NJS_MATERIAL collision_388_materials[1];
static Sint16 collision_388_faces[30];
static NJS_MESHSET collision_388_meshsets[1];
static NJS_POINT3 collision_388_points[18];
static NJS_MODEL collision_388_model;
static NJS_OBJECT collision_388;
static NJS_MATERIAL collision_389_materials[1];
static Sint16 collision_389_faces[23];
static NJS_MESHSET collision_389_meshsets[1];
static NJS_POINT3 collision_389_points[12];
static NJS_MODEL collision_389_model;
static NJS_OBJECT collision_389;
static NJS_MATERIAL collision_390_materials[1];
static Sint16 collision_390_faces[41];
static NJS_MESHSET collision_390_meshsets[1];
static NJS_POINT3 collision_390_points[18];
static NJS_MODEL collision_390_model;
static NJS_OBJECT collision_390;
static NJS_MATERIAL collision_391_materials[1];
static Sint16 collision_391_faces[7];
static NJS_MESHSET collision_391_meshsets[1];
static NJS_POINT3 collision_391_points[6];
static NJS_MODEL collision_391_model;
static NJS_OBJECT collision_391;
static NJS_MATERIAL collision_393_materials[1];
static Sint16 collision_393_faces[5];
static NJS_MESHSET collision_393_meshsets[1];
static NJS_POINT3 collision_393_points[4];
static NJS_MODEL collision_393_model;
static NJS_OBJECT collision_393;
static NJS_MATERIAL collision_394_materials[1];
static Sint16 collision_394_faces[22];
static NJS_MESHSET collision_394_meshsets[1];
static NJS_POINT3 collision_394_points[15];
static NJS_MODEL collision_394_model;
static NJS_OBJECT collision_394;
static NJS_MATERIAL collision_397_materials[1];
static Sint16 collision_397_faces[22];
static NJS_MESHSET collision_397_meshsets[1];
static NJS_POINT3 collision_397_points[15];
static NJS_MODEL collision_397_model;
static NJS_OBJECT collision_397;
static NJS_MATERIAL collision_399_materials[1];
static Sint16 collision_399_faces[18];
static NJS_MESHSET collision_399_meshsets[1];
static NJS_POINT3 collision_399_points[14];
static NJS_MODEL collision_399_model;
static NJS_OBJECT collision_399;
static NJS_MATERIAL collision_402_materials[1];
static Sint16 collision_402_faces[39];
static NJS_MESHSET collision_402_meshsets[1];
static NJS_POINT3 collision_402_points[21];
static NJS_MODEL collision_402_model;
static NJS_OBJECT collision_402;
static NJS_MATERIAL collision_405_materials[1];
static Sint16 collision_405_faces[11];
static NJS_MESHSET collision_405_meshsets[1];
static NJS_POINT3 collision_405_points[10];
static NJS_MODEL collision_405_model;
static NJS_OBJECT collision_405;
static NJS_MATERIAL collision_406_materials[1];
static Sint16 collision_406_faces[18];
static NJS_MESHSET collision_406_meshsets[1];
static NJS_POINT3 collision_406_points[14];
static NJS_MODEL collision_406_model;
static NJS_OBJECT collision_406;
static NJS_MATERIAL collision_409_materials[1];
static Sint16 collision_409_faces[39];
static NJS_MESHSET collision_409_meshsets[1];
static NJS_POINT3 collision_409_points[21];
static NJS_MODEL collision_409_model;
static NJS_OBJECT collision_409;
static NJS_MATERIAL collision_412_materials[1];
static Sint16 collision_412_faces[32];
static NJS_MESHSET collision_412_meshsets[1];
static NJS_POINT3 collision_412_points[23];
static NJS_MODEL collision_412_model;
static NJS_OBJECT collision_412;
static NJS_MATERIAL collision_413_materials[1];
static Sint16 collision_413_faces[44];
static NJS_MESHSET collision_413_meshsets[1];
static NJS_POINT3 collision_413_points[25];
static NJS_MODEL collision_413_model;
static NJS_OBJECT collision_413;
static NJS_MATERIAL collision_416_materials[1];
static Sint16 collision_416_faces[20];
static NJS_MESHSET collision_416_meshsets[1];
static NJS_POINT3 collision_416_points[7];
static NJS_MODEL collision_416_model;
static NJS_OBJECT collision_416;
static NJS_MATERIAL collision_418_materials[1];
static Sint16 collision_418_faces[42];
static NJS_MESHSET collision_418_meshsets[1];
static NJS_POINT3 collision_418_points[25];
static NJS_MODEL collision_418_model;
static NJS_OBJECT collision_418;
static NJS_MATERIAL collision_419_materials[1];
static Sint16 collision_419_faces[78];
static NJS_MESHSET collision_419_meshsets[1];
static NJS_POINT3 collision_419_points[35];
static NJS_MODEL collision_419_model;
static NJS_OBJECT collision_419;
static NJS_MATERIAL collision_422_materials[1];
static Sint16 collision_422_faces[69];
static NJS_MESHSET collision_422_meshsets[1];
static NJS_POINT3 collision_422_points[35];
static NJS_MODEL collision_422_model;
static NJS_OBJECT collision_422;
static NJS_MATERIAL collision_425_materials[1];
static Sint16 collision_425_faces[19];
static NJS_MESHSET collision_425_meshsets[1];
static NJS_POINT3 collision_425_points[16];
static NJS_MODEL collision_425_model;
static NJS_OBJECT collision_425;
static NJS_MATERIAL collision_426_materials[1];
static Sint16 collision_426_faces[249];
static NJS_MESHSET collision_426_meshsets[1];
static NJS_POINT3 collision_426_points[125];
static NJS_MODEL collision_426_model;
static NJS_OBJECT collision_426;
static NJS_MATERIAL collision_427_materials[1];
static Sint16 collision_427_faces[16];
static NJS_MESHSET collision_427_meshsets[1];
static NJS_POINT3 collision_427_points[6];
static NJS_MODEL collision_427_model;
static NJS_OBJECT collision_427;
static NJS_MATERIAL collision_428_materials[1];
static Sint16 collision_428_faces[21];
static NJS_MESHSET collision_428_meshsets[1];
static NJS_POINT3 collision_428_points[18];
static NJS_MODEL collision_428_model;
static NJS_OBJECT collision_428;
static NJS_MATERIAL collision_429_materials[1];
static Sint16 collision_429_faces[16];
static NJS_MESHSET collision_429_meshsets[1];
static NJS_POINT3 collision_429_points[6];
static NJS_MODEL collision_429_model;
static NJS_OBJECT collision_429;
static NJS_MATERIAL collision_430_materials[1];
static Sint16 collision_430_faces[11];
static NJS_MESHSET collision_430_meshsets[1];
static NJS_POINT3 collision_430_points[10];
static NJS_MODEL collision_430_model;
static NJS_OBJECT collision_430;
static NJS_MATERIAL collision_431_materials[1];
static Sint16 collision_431_faces[72];
static NJS_MESHSET collision_431_meshsets[1];
static NJS_POINT3 collision_431_points[35];
static NJS_MODEL collision_431_model;
static NJS_OBJECT collision_431;
static NJS_MATERIAL collision_433_materials[1];
static Sint16 collision_433_faces[12];
static NJS_MESHSET collision_433_meshsets[1];
static NJS_POINT3 collision_433_points[8];
static NJS_MODEL collision_433_model;
static NJS_OBJECT collision_433;
static NJS_MATERIAL collision_434_materials[1];
static Sint16 collision_434_faces[17];
static NJS_MESHSET collision_434_meshsets[1];
static NJS_POINT3 collision_434_points[13];
static NJS_MODEL collision_434_model;
static NJS_OBJECT collision_434;
static NJS_MATERIAL collision_436_materials[1];
static Sint16 collision_436_faces[55];
static NJS_MESHSET collision_436_meshsets[1];
static NJS_POINT3 collision_436_points[30];
static NJS_MODEL collision_436_model;
static NJS_OBJECT collision_436;
static NJS_MATERIAL collision_438_materials[1];
static Sint16 collision_438_faces[16];
static NJS_MESHSET collision_438_meshsets[1];
static NJS_POINT3 collision_438_points[12];
static NJS_MODEL collision_438_model;
static NJS_OBJECT collision_438;
static NJS_MATERIAL collision_439_materials[1];
static Sint16 collision_439_faces[7];
static NJS_MESHSET collision_439_meshsets[1];
static NJS_POINT3 collision_439_points[6];
static NJS_MODEL collision_439_model;
static NJS_OBJECT collision_439;
static NJS_MATERIAL collision_440_materials[1];
static Sint16 collision_440_faces[16];
static NJS_MESHSET collision_440_meshsets[1];
static NJS_POINT3 collision_440_points[12];
static NJS_MODEL collision_440_model;
static NJS_OBJECT collision_440;
static NJS_MATERIAL collision_441_materials[1];
static Sint16 collision_441_faces[16];
static NJS_MESHSET collision_441_meshsets[1];
static NJS_POINT3 collision_441_points[12];
static NJS_MODEL collision_441_model;
static NJS_OBJECT collision_441;
static NJS_MATERIAL collision_442_materials[1];
static Sint16 collision_442_faces[16];
static NJS_MESHSET collision_442_meshsets[1];
static NJS_POINT3 collision_442_points[12];
static NJS_MODEL collision_442_model;
static NJS_OBJECT collision_442;
static NJS_MATERIAL collision_443_materials[1];
static Sint16 collision_443_faces[7];
static NJS_MESHSET collision_443_meshsets[1];
static NJS_POINT3 collision_443_points[6];
static NJS_MODEL collision_443_model;
static NJS_OBJECT collision_443;
static NJS_MATERIAL collision_446_materials[1];
static Sint16 collision_446_faces[7];
static NJS_MESHSET collision_446_meshsets[1];
static NJS_POINT3 collision_446_points[6];
static NJS_MODEL collision_446_model;
static NJS_OBJECT collision_446;
static NJS_MATERIAL collision_447_materials[1];
static Sint16 collision_447_faces[7];
static NJS_MESHSET collision_447_meshsets[1];
static NJS_POINT3 collision_447_points[6];
static NJS_MODEL collision_447_model;
static NJS_OBJECT collision_447;
static NJS_MATERIAL collision_448_materials[1];
static Sint16 collision_448_faces[48];
static NJS_MESHSET collision_448_meshsets[1];
static NJS_POINT3 collision_448_points[26];
static NJS_MODEL collision_448_model;
static NJS_OBJECT collision_448;
static NJS_MATERIAL collision_449_materials[1];
static Sint16 collision_449_faces[14];
static NJS_MESHSET collision_449_meshsets[1];
static NJS_POINT3 collision_449_points[7];
static NJS_MODEL collision_449_model;
static NJS_OBJECT collision_449;
static NJS_MATERIAL collision_450_materials[1];
static Sint16 collision_450_faces[42];
static NJS_MESHSET collision_450_meshsets[1];
static NJS_POINT3 collision_450_points[25];
static NJS_MODEL collision_450_model;
static NJS_OBJECT collision_450;
static NJS_MATERIAL collision_451_materials[1];
static Sint16 collision_451_faces[6];
static NJS_MESHSET collision_451_meshsets[1];
static NJS_POINT3 collision_451_points[5];
static NJS_MODEL collision_451_model;
static NJS_OBJECT collision_451;
static NJS_MATERIAL collision_452_materials[1];
static Sint16 collision_452_faces[6];
static NJS_MESHSET collision_452_meshsets[1];
static NJS_POINT3 collision_452_points[5];
static NJS_MODEL collision_452_model;
static NJS_OBJECT collision_452;
static NJS_MATERIAL collision_453_materials[1];
static Sint16 collision_453_faces[6];
static NJS_MESHSET collision_453_meshsets[1];
static NJS_POINT3 collision_453_points[5];
static NJS_MODEL collision_453_model;
static NJS_OBJECT collision_453;
static NJS_MATERIAL collision_454_materials[1];
static Sint16 collision_454_faces[35];
static NJS_MESHSET collision_454_meshsets[1];
static NJS_POINT3 collision_454_points[14];
static NJS_MODEL collision_454_model;
static NJS_OBJECT collision_454;
static NJS_MATERIAL collision_455_materials[1];
static Sint16 collision_455_faces[19];
static NJS_MESHSET collision_455_meshsets[1];
static NJS_POINT3 collision_455_points[16];
static NJS_MODEL collision_455_model;
static NJS_OBJECT collision_455;
static NJS_MATERIAL collision_456_materials[1];
static Sint16 collision_456_faces[24];
static NJS_MESHSET collision_456_meshsets[1];
static NJS_POINT3 collision_456_points[14];
static NJS_MODEL collision_456_model;
static NJS_OBJECT collision_456;
static NJS_MATERIAL collision_457_materials[1];
static Sint16 collision_457_faces[7];
static NJS_MESHSET collision_457_meshsets[1];
static NJS_POINT3 collision_457_points[6];
static NJS_MODEL collision_457_model;
static NJS_OBJECT collision_457;
static NJS_MATERIAL collision_458_materials[1];
static Sint16 collision_458_faces[27];
static NJS_MESHSET collision_458_meshsets[1];
static NJS_POINT3 collision_458_points[14];
static NJS_MODEL collision_458_model;
static NJS_OBJECT collision_458;
static NJS_MATERIAL collision_459_materials[1];
static Sint16 collision_459_faces[53];
static NJS_MESHSET collision_459_meshsets[1];
static NJS_POINT3 collision_459_points[25];
static NJS_MODEL collision_459_model;
static NJS_OBJECT collision_459;
static NJS_MATERIAL collision_460_materials[1];
static Sint16 collision_460_faces[27];
static NJS_MESHSET collision_460_meshsets[1];
static NJS_POINT3 collision_460_points[16];
static NJS_MODEL collision_460_model;
static NJS_OBJECT collision_460;
static NJS_MATERIAL collision_461_materials[1];
static Sint16 collision_461_faces[5];
static NJS_MESHSET collision_461_meshsets[1];
static NJS_POINT3 collision_461_points[4];
static NJS_MODEL collision_461_model;
static NJS_OBJECT collision_461;
static NJS_MATERIAL collision_462_materials[1];
static Sint16 collision_462_faces[71];
static NJS_MESHSET collision_462_meshsets[1];
static NJS_POINT3 collision_462_points[38];
static NJS_MODEL collision_462_model;
static NJS_OBJECT collision_462;
static NJS_MATERIAL collision_463_materials[1];
static Sint16 collision_463_faces[61];
static NJS_MESHSET collision_463_meshsets[1];
static NJS_POINT3 collision_463_points[28];
static NJS_MODEL collision_463_model;
static NJS_OBJECT collision_463;
static NJS_MATERIAL collision_464_materials[1];
static Sint16 collision_464_faces[23];
static NJS_MESHSET collision_464_meshsets[1];
static NJS_POINT3 collision_464_points[22];
static NJS_MODEL collision_464_model;
static NJS_OBJECT collision_464;
static NJS_MATERIAL collision_465_materials[1];
static Sint16 collision_465_faces[35];
static NJS_MESHSET collision_465_meshsets[1];
static NJS_POINT3 collision_465_points[18];
static NJS_MODEL collision_465_model;
static NJS_OBJECT collision_465;
static NJS_MATERIAL collision_466_materials[1];
static Sint16 collision_466_faces[20];
static NJS_MESHSET collision_466_meshsets[1];
static NJS_POINT3 collision_466_points[16];
static NJS_MODEL collision_466_model;
static NJS_OBJECT collision_466;
static NJS_MATERIAL collision_467_materials[1];
static Sint16 collision_467_faces[6];
static NJS_MESHSET collision_467_meshsets[1];
static NJS_POINT3 collision_467_points[5];
static NJS_MODEL collision_467_model;
static NJS_OBJECT collision_467;
static NJS_MATERIAL collision_468_materials[1];
static Sint16 collision_468_faces[20];
static NJS_MESHSET collision_468_meshsets[1];
static NJS_POINT3 collision_468_points[16];
static NJS_MODEL collision_468_model;
static NJS_OBJECT collision_468;
static NJS_MATERIAL collision_469_materials[1];
static Sint16 collision_469_faces[6];
static NJS_MESHSET collision_469_meshsets[1];
static NJS_POINT3 collision_469_points[5];
static NJS_MODEL collision_469_model;
static NJS_OBJECT collision_469;
static NJS_MATERIAL collision_470_materials[1];
static Sint16 collision_470_faces[23];
static NJS_MESHSET collision_470_meshsets[1];
static NJS_POINT3 collision_470_points[22];
static NJS_MODEL collision_470_model;
static NJS_OBJECT collision_470;
static NJS_MATERIAL collision_472_materials[1];
static Sint16 collision_472_faces[5];
static NJS_MESHSET collision_472_meshsets[1];
static NJS_POINT3 collision_472_points[4];
static NJS_MODEL collision_472_model;
static NJS_OBJECT collision_472;
static NJS_MATERIAL collision_474_materials[1];
static Sint16 collision_474_faces[5];
static NJS_MESHSET collision_474_meshsets[1];
static NJS_POINT3 collision_474_points[4];
static NJS_MODEL collision_474_model;
static NJS_OBJECT collision_474;
static NJS_MATERIAL collision_476_materials[1];
static Sint16 collision_476_faces[23];
static NJS_MESHSET collision_476_meshsets[1];
static NJS_POINT3 collision_476_points[16];
static NJS_MODEL collision_476_model;
static NJS_OBJECT collision_476;
static NJS_MATERIAL collision_477_materials[1];
static Sint16 collision_477_faces[14];
static NJS_MESHSET collision_477_meshsets[1];
static NJS_POINT3 collision_477_points[9];
static NJS_MODEL collision_477_model;
static NJS_OBJECT collision_477;
static NJS_MATERIAL collision_478_materials[1];
static Sint16 collision_478_faces[15];
static NJS_MESHSET collision_478_meshsets[1];
static NJS_POINT3 collision_478_points[10];
static NJS_MODEL collision_478_model;
static NJS_OBJECT collision_478;
static NJS_MATERIAL collision_479_materials[1];
static Sint16 collision_479_faces[22];
static NJS_MESHSET collision_479_meshsets[1];
static NJS_POINT3 collision_479_points[15];
static NJS_MODEL collision_479_model;
static NJS_OBJECT collision_479;
static NJS_MATERIAL collision_481_materials[1];
static Sint16 collision_481_faces[22];
static NJS_MESHSET collision_481_meshsets[1];
static NJS_POINT3 collision_481_points[15];
static NJS_MODEL collision_481_model;
static NJS_OBJECT collision_481;
static NJS_MATERIAL collision_483_materials[1];
static Sint16 collision_483_faces[14];
static NJS_MESHSET collision_483_meshsets[1];
static NJS_POINT3 collision_483_points[9];
static NJS_MODEL collision_483_model;
static NJS_OBJECT collision_483;
static NJS_MATERIAL collision_484_materials[1];
static Sint16 collision_484_faces[25];
static NJS_MESHSET collision_484_meshsets[1];
static NJS_POINT3 collision_484_points[14];
static NJS_MODEL collision_484_model;
static NJS_OBJECT collision_484;
static NJS_MATERIAL collision_485_materials[1];
static Sint16 collision_485_faces[51];
static NJS_MESHSET collision_485_meshsets[1];
static NJS_POINT3 collision_485_points[50];
static NJS_MODEL collision_485_model;
static NJS_OBJECT collision_485;
static NJS_MATERIAL collision_486_materials[1];
static Sint16 collision_486_faces[8];
static NJS_MESHSET collision_486_meshsets[1];
static NJS_POINT3 collision_486_points[6];
static NJS_MODEL collision_486_model;
static NJS_OBJECT collision_486;
static NJS_MATERIAL collision_487_materials[1];
static Sint16 collision_487_faces[33];
static NJS_MESHSET collision_487_meshsets[1];
static NJS_POINT3 collision_487_points[18];
static NJS_MODEL collision_487_model;
static NJS_OBJECT collision_487;
static NJS_MATERIAL collision_488_materials[1];
static Sint16 collision_488_faces[10];
static NJS_MESHSET collision_488_meshsets[1];
static NJS_POINT3 collision_488_points[8];
static NJS_MODEL collision_488_model;
static NJS_OBJECT collision_488;
static NJS_MATERIAL collision_489_materials[1];
static Sint16 collision_489_faces[7];
static NJS_MESHSET collision_489_meshsets[1];
static NJS_POINT3 collision_489_points[6];
static NJS_MODEL collision_489_model;
static NJS_OBJECT collision_489;
static NJS_MATERIAL collision_490_materials[1];
static Sint16 collision_490_faces[24];
static NJS_MESHSET collision_490_meshsets[1];
static NJS_POINT3 collision_490_points[12];
static NJS_MODEL collision_490_model;
static NJS_OBJECT collision_490;
static NJS_MATERIAL collision_491_materials[1];
static Sint16 collision_491_faces[51];
static NJS_MESHSET collision_491_meshsets[1];
static NJS_POINT3 collision_491_points[22];
static NJS_MODEL collision_491_model;
static NJS_OBJECT collision_491;
static NJS_MATERIAL collision_492_materials[1];
static Sint16 collision_492_faces[26];
static NJS_MESHSET collision_492_meshsets[1];
static NJS_POINT3 collision_492_points[22];
static NJS_MODEL collision_492_model;
static NJS_OBJECT collision_492;
static NJS_MATERIAL collision_493_materials[1];
static Sint16 collision_493_faces[5];
static NJS_MESHSET collision_493_meshsets[1];
static NJS_POINT3 collision_493_points[4];
static NJS_MODEL collision_493_model;
static NJS_OBJECT collision_493;
static NJS_MATERIAL collision_496_materials[1];
static Sint16 collision_496_faces[5];
static NJS_MESHSET collision_496_meshsets[1];
static NJS_POINT3 collision_496_points[4];
static NJS_MODEL collision_496_model;
static NJS_OBJECT collision_496;
static NJS_MATERIAL collision_497_materials[1];
static Sint16 collision_497_faces[5];
static NJS_MESHSET collision_497_meshsets[1];
static NJS_POINT3 collision_497_points[4];
static NJS_MODEL collision_497_model;
static NJS_OBJECT collision_497;
static NJS_MATERIAL collision_499_materials[1];
static Sint16 collision_499_faces[5];
static NJS_MESHSET collision_499_meshsets[1];
static NJS_POINT3 collision_499_points[4];
static NJS_MODEL collision_499_model;
static NJS_OBJECT collision_499;
static NJS_MATERIAL collision_502_materials[1];
static Sint16 collision_502_faces[7];
static NJS_MESHSET collision_502_meshsets[1];
static NJS_POINT3 collision_502_points[6];
static NJS_MODEL collision_502_model;
static NJS_OBJECT collision_502;
static NJS_MATERIAL collision_509_materials[1];
static Sint16 collision_509_faces[13];
static NJS_MESHSET collision_509_meshsets[1];
static NJS_POINT3 collision_509_points[12];
static NJS_MODEL collision_509_model;
static NJS_OBJECT collision_509;
static NJS_MATERIAL collision_517_materials[1];
static Sint16 collision_517_faces[13];
static NJS_MESHSET collision_517_meshsets[1];
static NJS_POINT3 collision_517_points[12];
static NJS_MODEL collision_517_model;
static NJS_OBJECT collision_517;
static NJS_MATERIAL collision_525_materials[1];
static Sint16 collision_525_faces[5];
static NJS_MESHSET collision_525_meshsets[1];
static NJS_POINT3 collision_525_points[4];
static NJS_MODEL collision_525_model;
static NJS_OBJECT collision_525;
static NJS_MATERIAL collision_527_materials[1];
static Sint16 collision_527_faces[7];
static NJS_MESHSET collision_527_meshsets[1];
static NJS_POINT3 collision_527_points[6];
static NJS_MODEL collision_527_model;
static NJS_OBJECT collision_527;
static NJS_MATERIAL collision_529_materials[1];
static Sint16 collision_529_faces[9];
static NJS_MESHSET collision_529_meshsets[1];
static NJS_POINT3 collision_529_points[8];
static NJS_MODEL collision_529_model;
static NJS_OBJECT collision_529;
static NJS_MATERIAL collision_530_materials[1];
static Sint16 collision_530_faces[7];
static NJS_MESHSET collision_530_meshsets[1];
static NJS_POINT3 collision_530_points[6];
static NJS_MODEL collision_530_model;
static NJS_OBJECT collision_530;
static NJS_MATERIAL collision_531_materials[1];
static Sint16 collision_531_faces[7];
static NJS_MESHSET collision_531_meshsets[1];
static NJS_POINT3 collision_531_points[6];
static NJS_MODEL collision_531_model;
static NJS_OBJECT collision_531;
static NJS_MATERIAL collision_532_materials[1];
static Sint16 collision_532_faces[7];
static NJS_MESHSET collision_532_meshsets[1];
static NJS_POINT3 collision_532_points[6];
static NJS_MODEL collision_532_model;
static NJS_OBJECT collision_532;
static Sint16 draw_640_polygons[239];
static Sint32 draw_640_vertices[231];
static NJS_CNK_MODEL draw_640_model;
static NJS_CNK_OBJECT draw_2204;
static Sint16 draw_165_polygons[37];
static Sint32 draw_165_vertices[35];
static NJS_CNK_MODEL draw_165_model;
static NJS_CNK_OBJECT draw_2203;
static Sint16 draw_639_polygons[375];
static Sint32 draw_639_vertices[391];
static NJS_CNK_MODEL draw_639_model;
static NJS_CNK_OBJECT draw_2202;
static Sint16 draw_121_polygons[23];
static Sint32 draw_121_vertices[19];
static NJS_CNK_MODEL draw_121_model;
static NJS_CNK_OBJECT draw_2201;
static Sint16 draw_517_polygons[59];
static Sint32 draw_517_vertices[67];
static NJS_CNK_MODEL draw_517_model;
static NJS_CNK_OBJECT draw_2200;
static Sint16 draw_519_polygons[59];
static Sint32 draw_519_vertices[67];
static NJS_CNK_MODEL draw_519_model;
static NJS_CNK_OBJECT draw_2199;
static Sint16 draw_206_polygons[91];
static Sint32 draw_206_vertices[103];
static NJS_CNK_MODEL draw_206_model;
static NJS_CNK_OBJECT draw_2197;
static Sint16 draw_220_polygons[47];
static Sint32 draw_220_vertices[31];
static NJS_CNK_MODEL draw_220_model;
static NJS_CNK_OBJECT draw_2194;
static Sint16 draw_116_polygons[123];
static Sint32 draw_116_vertices[87];
static NJS_CNK_MODEL draw_116_model;
static NJS_CNK_OBJECT draw_2193;
static Sint16 draw_114_polygons[123];
static Sint32 draw_114_vertices[87];
static NJS_CNK_MODEL draw_114_model;
static NJS_CNK_OBJECT draw_2191;
static Sint16 draw_298_polygons[59];
static Sint32 draw_298_vertices[67];
static NJS_CNK_MODEL draw_298_model;
static NJS_CNK_OBJECT draw_2187;
static Sint16 draw_638_polygons[433];
static Sint32 draw_638_vertices[395];
static NJS_CNK_MODEL draw_638_model;
static NJS_CNK_OBJECT draw_2185;
static Sint16 draw_637_polygons[439];
static Sint32 draw_637_vertices[387];
static NJS_CNK_MODEL draw_637_model;
static NJS_CNK_OBJECT draw_2184;
static Sint16 draw_180_polygons[23];
static Sint32 draw_180_vertices[19];
static NJS_CNK_MODEL draw_180_model;
static NJS_CNK_OBJECT draw_2182;
static Sint16 draw_83_polygons[37];
static Sint32 draw_83_vertices[35];
static NJS_CNK_MODEL draw_83_model;
static NJS_CNK_OBJECT draw_2113;
static Sint16 draw_94_polygons[155];
static Sint32 draw_94_vertices[147];
static NJS_CNK_MODEL draw_94_model;
static NJS_CNK_OBJECT draw_2053;
static Sint16 draw_2047_polygons[109];
static Sint32 draw_2047_vertices[131];
static NJS_CNK_MODEL draw_2047_model;
static NJS_CNK_OBJECT draw_2047;
static Sint16 draw_2044_polygons[115];
static Sint32 draw_2044_vertices[135];
static NJS_CNK_MODEL draw_2044_model;
static NJS_CNK_OBJECT draw_2044;
static Sint16 draw_160_polygons[673];
static Sint32 draw_160_vertices[643];
static NJS_CNK_MODEL draw_160_model;
static NJS_CNK_OBJECT draw_2042;
static Sint16 draw_166_polygons[69];
static Sint32 draw_166_vertices[35];
static NJS_CNK_MODEL draw_166_model;
static NJS_CNK_OBJECT draw_2041;
static Sint16 draw_72_polygons[185];
static Sint32 draw_72_vertices[151];
static NJS_CNK_MODEL draw_72_model;
static NJS_CNK_OBJECT draw_2039;
static Sint16 draw_266_polygons[183];
static Sint32 draw_266_vertices[131];
static NJS_CNK_MODEL draw_266_model;
static NJS_CNK_OBJECT draw_2038;
static Sint16 draw_212_polygons[227];
static Sint32 draw_212_vertices[143];
static NJS_CNK_MODEL draw_212_model;
static NJS_CNK_OBJECT draw_2037;
static Sint16 draw_692_polygons[1229];
static Sint32 draw_692_vertices[1303];
static NJS_CNK_MODEL draw_692_model;
static NJS_CNK_OBJECT draw_2032;
static Sint16 draw_74_polygons[227];
static Sint32 draw_74_vertices[163];
static NJS_CNK_MODEL draw_74_model;
static NJS_CNK_OBJECT draw_2031;
static Sint16 draw_297_polygons[77];
static Sint32 draw_297_vertices[91];
static NJS_CNK_MODEL draw_297_model;
static NJS_CNK_OBJECT draw_2024;
static Sint16 draw_178_polygons[401];
static Sint32 draw_178_vertices[391];
static NJS_CNK_MODEL draw_178_model;
static NJS_CNK_OBJECT draw_2018;
static Sint16 draw_291_polygons[587];
static Sint32 draw_291_vertices[539];
static NJS_CNK_MODEL draw_291_model;
static NJS_CNK_OBJECT draw_2017;
static Sint16 draw_1356_polygons[413];
static Sint32 draw_1356_vertices[363];
static NJS_CNK_MODEL draw_1356_model;
static NJS_CNK_OBJECT draw_2016;
static Sint16 draw_109_polygons[215];
static Sint32 draw_109_vertices[155];
static NJS_CNK_MODEL draw_109_model;
static NJS_CNK_OBJECT draw_2012;
static Sint16 draw_674_polygons[1177];
static Sint32 draw_674_vertices[1235];
static NJS_CNK_MODEL draw_674_model;
static NJS_CNK_OBJECT draw_2011;
static Sint16 draw_633_polygons[113];
static Sint32 draw_633_vertices[75];
static NJS_CNK_MODEL draw_633_model;
static NJS_CNK_OBJECT draw_2009;
static Sint16 draw_287_polygons[449];
static Sint32 draw_287_vertices[443];
static NJS_CNK_MODEL draw_287_model;
static NJS_CNK_OBJECT draw_2006;
static Sint16 draw_286_polygons[61];
static Sint32 draw_286_vertices[47];
static NJS_CNK_MODEL draw_286_model;
static NJS_CNK_OBJECT draw_2000;
static Sint16 draw_1259_polygons[41];
static Sint32 draw_1259_vertices[35];
static NJS_CNK_MODEL draw_1259_model;
static NJS_CNK_OBJECT draw_1999;
static Sint16 draw_282_polygons[43];
static Sint32 draw_282_vertices[35];
static NJS_CNK_MODEL draw_282_model;
static NJS_CNK_OBJECT draw_1997;
static Sint16 draw_135_polygons[617];
static Sint32 draw_135_vertices[475];
static NJS_CNK_MODEL draw_135_model;
static NJS_CNK_OBJECT draw_1994;
static Sint16 draw_158_polygons[41];
static Sint32 draw_158_vertices[35];
static NJS_CNK_MODEL draw_158_model;
static NJS_CNK_OBJECT draw_1993;
static Sint16 draw_281_polygons[67];
static Sint32 draw_281_vertices[47];
static NJS_CNK_MODEL draw_281_model;
static NJS_CNK_OBJECT draw_1991;
static Sint16 draw_617_polygons[815];
static Sint32 draw_617_vertices[791];
static NJS_CNK_MODEL draw_617_model;
static NJS_CNK_OBJECT draw_1987;
static Sint16 draw_132_polygons[153];
static Sint32 draw_132_vertices[155];
static NJS_CNK_MODEL draw_132_model;
static NJS_CNK_OBJECT draw_1986;
static Sint16 draw_1341_polygons[435];
static Sint32 draw_1341_vertices[403];
static NJS_CNK_MODEL draw_1341_model;
static NJS_CNK_OBJECT draw_1985;
static Sint16 draw_615_polygons[59];
static Sint32 draw_615_vertices[51];
static NJS_CNK_MODEL draw_615_model;
static NJS_CNK_OBJECT draw_1984;
static Sint16 draw_279_polygons[1023];
static Sint32 draw_279_vertices[1019];
static NJS_CNK_MODEL draw_279_model;
static NJS_CNK_OBJECT draw_1982;
static Sint16 draw_110_polygons[141];
static Sint32 draw_110_vertices[111];
static NJS_CNK_MODEL draw_110_model;
static NJS_CNK_OBJECT draw_1981;
static Sint16 draw_277_polygons[59];
static Sint32 draw_277_vertices[51];
static NJS_CNK_MODEL draw_277_model;
static NJS_CNK_OBJECT draw_1980;
static Sint16 draw_205_polygons[293];
static Sint32 draw_205_vertices[267];
static NJS_CNK_MODEL draw_205_model;
static NJS_CNK_OBJECT draw_1979;
static Sint16 draw_272_polygons[547];
static Sint32 draw_272_vertices[563];
static NJS_CNK_MODEL draw_272_model;
static NJS_CNK_OBJECT draw_1978;
static Sint16 draw_107_polygons[215];
static Sint32 draw_107_vertices[211];
static NJS_CNK_MODEL draw_107_model;
static NJS_CNK_OBJECT draw_1977;
static Sint16 draw_673_polygons[43];
static Sint32 draw_673_vertices[27];
static NJS_CNK_MODEL draw_673_model;
static NJS_CNK_OBJECT draw_1976;
static Sint16 draw_270_polygons[1129];
static Sint32 draw_270_vertices[891];
static NJS_CNK_MODEL draw_270_model;
static NJS_CNK_OBJECT draw_1975;
static Sint16 draw_672_polygons[279];
static Sint32 draw_672_vertices[279];
static NJS_CNK_MODEL draw_672_model;
static NJS_CNK_OBJECT draw_1974;
static Sint16 draw_268_polygons[81];
static Sint32 draw_268_vertices[67];
static NJS_CNK_MODEL draw_268_model;
static NJS_CNK_OBJECT draw_1973;
static Sint16 draw_588_polygons[213];
static Sint32 draw_588_vertices[171];
static NJS_CNK_MODEL draw_588_model;
static NJS_CNK_OBJECT draw_1972;
static Sint16 draw_262_polygons[281];
static Sint32 draw_262_vertices[211];
static NJS_CNK_MODEL draw_262_model;
static NJS_CNK_OBJECT draw_1971;
static Sint16 draw_296_polygons[327];
static Sint32 draw_296_vertices[311];
static NJS_CNK_MODEL draw_296_model;
static NJS_CNK_OBJECT draw_1970;
static Sint16 draw_259_polygons[429];
static Sint32 draw_259_vertices[411];
static NJS_CNK_MODEL draw_259_model;
static NJS_CNK_OBJECT draw_1969;
static Sint16 draw_202_polygons[357];
static Sint32 draw_202_vertices[343];
static NJS_CNK_MODEL draw_202_model;
static NJS_CNK_OBJECT draw_1968;
static Sint16 draw_254_polygons[271];
static Sint32 draw_254_vertices[191];
static NJS_CNK_MODEL draw_254_model;
static NJS_CNK_OBJECT draw_1967;
static Sint16 draw_252_polygons[747];
static Sint32 draw_252_vertices[743];
static NJS_CNK_MODEL draw_252_model;
static NJS_CNK_OBJECT draw_1966;
static Sint16 draw_250_polygons[73];
static Sint32 draw_250_vertices[67];
static NJS_CNK_MODEL draw_250_model;
static NJS_CNK_OBJECT draw_1965;
static Sint16 draw_201_polygons[123];
static Sint32 draw_201_vertices[107];
static NJS_CNK_MODEL draw_201_model;
static NJS_CNK_OBJECT draw_1964;
static Sint16 draw_200_polygons[423];
static Sint32 draw_200_vertices[479];
static NJS_CNK_MODEL draw_200_model;
static NJS_CNK_OBJECT draw_1963;
static Sint16 draw_240_polygons[431];
static Sint32 draw_240_vertices[455];
static NJS_CNK_MODEL draw_240_model;
static NJS_CNK_OBJECT draw_1962;
static Sint16 draw_238_polygons[59];
static Sint32 draw_238_vertices[51];
static NJS_CNK_MODEL draw_238_model;
static NJS_CNK_OBJECT draw_1961;
static Sint16 draw_199_polygons[633];
static Sint32 draw_199_vertices[515];
static NJS_CNK_MODEL draw_199_model;
static NJS_CNK_OBJECT draw_1960;
static Sint16 draw_234_polygons[59];
static Sint32 draw_234_vertices[51];
static NJS_CNK_MODEL draw_234_model;
static NJS_CNK_OBJECT draw_1959;
static Sint16 draw_232_polygons[621];
static Sint32 draw_232_vertices[575];
static NJS_CNK_MODEL draw_232_model;
static NJS_CNK_OBJECT draw_1958;
static Sint16 draw_230_polygons[77];
static Sint32 draw_230_vertices[59];
static NJS_CNK_MODEL draw_230_model;
static NJS_CNK_OBJECT draw_1957;
static Sint16 draw_100_polygons[429];
static Sint32 draw_100_vertices[475];
static NJS_CNK_MODEL draw_100_model;
static NJS_CNK_OBJECT draw_1956;
static Sint16 draw_98_polygons[55];
static Sint32 draw_98_vertices[51];
static NJS_CNK_MODEL draw_98_model;
static NJS_CNK_OBJECT draw_1955;
static Sint16 draw_228_polygons[59];
static Sint32 draw_228_vertices[43];
static NJS_CNK_MODEL draw_228_model;
static NJS_CNK_OBJECT draw_1954;
static Sint16 draw_190_polygons[1079];
static Sint32 draw_190_vertices[1031];
static NJS_CNK_MODEL draw_190_model;
static NJS_CNK_OBJECT draw_1949;
static Sint16 draw_189_polygons[169];
static Sint32 draw_189_vertices[159];
static NJS_CNK_MODEL draw_189_model;
static NJS_CNK_OBJECT draw_1931;
static Sint16 draw_85_polygons[1465];
static Sint32 draw_85_vertices[1151];
static NJS_CNK_MODEL draw_85_model;
static NJS_CNK_OBJECT draw_1878;
static Sint16 draw_142_polygons[1353];
static Sint32 draw_142_vertices[1423];
static NJS_CNK_MODEL draw_142_model;
static NJS_CNK_OBJECT draw_1877;
static Sint16 draw_82_polygons[153];
static Sint32 draw_82_vertices[115];
static NJS_CNK_MODEL draw_82_model;
static NJS_CNK_OBJECT draw_1876;
static Sint16 draw_81_polygons[149];
static Sint32 draw_81_vertices[111];
static NJS_CNK_MODEL draw_81_model;
static NJS_CNK_OBJECT draw_1875;
static Sint16 draw_75_polygons[571];
static Sint32 draw_75_vertices[519];
static NJS_CNK_MODEL draw_75_model;
static NJS_CNK_OBJECT draw_1874;
static Sint16 draw_89_polygons[233];
static Sint32 draw_89_vertices[187];
static NJS_CNK_MODEL draw_89_model;
static NJS_CNK_OBJECT draw_1873;
static Sint16 draw_88_polygons[643];
static Sint32 draw_88_vertices[563];
static NJS_CNK_MODEL draw_88_model;
static NJS_CNK_OBJECT draw_1872;
static Sint16 draw_175_polygons[533];
static Sint32 draw_175_vertices[559];
static NJS_CNK_MODEL draw_175_model;
static NJS_CNK_OBJECT draw_1871;
static Sint16 draw_86_polygons[91];
static Sint32 draw_86_vertices[71];
static NJS_CNK_MODEL draw_86_model;
static NJS_CNK_OBJECT draw_1870;
static Sint16 draw_170_polygons[579];
static Sint32 draw_170_vertices[615];
static NJS_CNK_MODEL draw_170_model;
static NJS_CNK_OBJECT draw_1869;
static Sint16 draw_172_polygons[379];
static Sint32 draw_172_vertices[379];
static NJS_CNK_MODEL draw_172_model;
static NJS_CNK_OBJECT draw_1858;
static Sint16 draw_952_polygons[273];
static Sint32 draw_952_vertices[235];
static NJS_CNK_MODEL draw_952_model;
static NJS_CNK_OBJECT draw_1856;
static Sint16 draw_183_polygons[487];
static Sint32 draw_183_vertices[439];
static NJS_CNK_MODEL draw_183_model;
static NJS_CNK_OBJECT draw_1853;
static Sint16 draw_127_polygons[899];
static Sint32 draw_127_vertices[847];
static NJS_CNK_MODEL draw_127_model;
static NJS_CNK_OBJECT draw_1852;
static Sint16 draw_167_polygons[587];
static Sint32 draw_167_vertices[599];
static NJS_CNK_MODEL draw_167_model;
static NJS_CNK_OBJECT draw_1851;
static Sint16 draw_164_polygons[165];
static Sint32 draw_164_vertices[115];
static NJS_CNK_MODEL draw_164_model;
static NJS_CNK_OBJECT draw_1847;
static Sint16 draw_187_polygons[199];
static Sint32 draw_187_vertices[171];
static NJS_CNK_MODEL draw_187_model;
static NJS_CNK_OBJECT draw_1846;
static Sint16 draw_152_polygons[695];
static Sint32 draw_152_vertices[563];
static NJS_CNK_MODEL draw_152_model;
static NJS_CNK_OBJECT draw_1842;
static Sint16 draw_150_polygons[659];
static Sint32 draw_150_vertices[543];
static NJS_CNK_MODEL draw_150_model;
static NJS_CNK_OBJECT draw_1837;
static Sint16 draw_76_polygons[833];
static Sint32 draw_76_vertices[727];
static NJS_CNK_MODEL draw_76_model;
static NJS_CNK_OBJECT draw_1834;
static Sint16 draw_144_polygons[65];
static Sint32 draw_144_vertices[75];
static NJS_CNK_MODEL draw_144_model;
static NJS_CNK_OBJECT draw_1828;
static Sint16 draw_138_polygons[897];
static Sint32 draw_138_vertices[731];
static NJS_CNK_MODEL draw_138_model;
static NJS_CNK_OBJECT draw_1825;
static Sint16 draw_162_polygons[55];
static Sint32 draw_162_vertices[43];
static NJS_CNK_MODEL draw_162_model;
static NJS_CNK_OBJECT draw_1803;
static Sint16 draw_78_polygons[453];
static Sint32 draw_78_vertices[407];
static NJS_CNK_MODEL draw_78_model;
static NJS_CNK_OBJECT draw_1768;
static Sint16 draw_131_polygons[257];
static Sint32 draw_131_vertices[199];
static NJS_CNK_MODEL draw_131_model;
static NJS_CNK_OBJECT draw_1767;
static Sint16 draw_687_polygons[989];
static Sint32 draw_687_vertices[903];
static NJS_CNK_MODEL draw_687_model;
static NJS_CNK_OBJECT draw_1766;
static Sint16 draw_686_polygons[359];
static Sint32 draw_686_vertices[307];
static NJS_CNK_MODEL draw_686_model;
static NJS_CNK_OBJECT draw_1763;
static Sint16 draw_113_polygons[147];
static Sint32 draw_113_vertices[135];
static NJS_CNK_MODEL draw_113_model;
static NJS_CNK_OBJECT draw_1744;
static Sint16 draw_112_polygons[23];
static Sint32 draw_112_vertices[19];
static NJS_CNK_MODEL draw_112_model;
static NJS_CNK_OBJECT draw_1743;
static Sint16 draw_106_polygons[293];
static Sint32 draw_106_vertices[215];
static NJS_CNK_MODEL draw_106_model;
static NJS_CNK_OBJECT draw_1672;
static Sint16 draw_92_polygons[181];
static Sint32 draw_92_vertices[131];
static NJS_CNK_MODEL draw_92_model;
static NJS_CNK_OBJECT draw_1657;
static Sint16 draw_90_polygons[87];
static Sint32 draw_90_vertices[63];
static NJS_CNK_MODEL draw_90_model;
static NJS_CNK_OBJECT draw_1655;
static Sint16 draw_87_polygons[813];
static Sint32 draw_87_vertices[807];
static NJS_CNK_MODEL draw_87_model;
static NJS_CNK_OBJECT draw_1640;
static Sint16 draw_841_polygons[205];
static Sint32 draw_841_vertices[187];
static NJS_CNK_MODEL draw_841_model;
static NJS_CNK_OBJECT draw_1639;
static Sint16 draw_1637_polygons[33];
static Sint32 draw_1637_vertices[35];
static NJS_CNK_MODEL draw_1637_model;
static NJS_CNK_OBJECT draw_1637;
static Sint16 draw_1636_polygons[227];
static Sint32 draw_1636_vertices[143];
static NJS_CNK_MODEL draw_1636_model;
static NJS_CNK_OBJECT draw_1636;
static Sint16 draw_1634_polygons[381];
static Sint32 draw_1634_vertices[307];
static NJS_CNK_MODEL draw_1634_model;
static NJS_CNK_OBJECT draw_1634;
static Sint16 draw_1633_polygons[149];
static Sint32 draw_1633_vertices[131];
static NJS_CNK_MODEL draw_1633_model;
static NJS_CNK_OBJECT draw_1633;
static Sint16 draw_1632_polygons[217];
static Sint32 draw_1632_vertices[179];
static NJS_CNK_MODEL draw_1632_model;
static NJS_CNK_OBJECT draw_1632;
static Sint16 draw_1631_polygons[51];
static Sint32 draw_1631_vertices[43];
static NJS_CNK_MODEL draw_1631_model;
static NJS_CNK_OBJECT draw_1631;
static Sint16 draw_656_polygons[55];
static Sint32 draw_656_vertices[43];
static NJS_CNK_MODEL draw_656_model;
static NJS_CNK_OBJECT draw_1630;
static Sint16 draw_1629_polygons[147];
static Sint32 draw_1629_vertices[143];
static NJS_CNK_MODEL draw_1629_model;
static NJS_CNK_OBJECT draw_1629;
static Sint16 draw_1628_polygons[223];
static Sint32 draw_1628_vertices[207];
static NJS_CNK_MODEL draw_1628_model;
static NJS_CNK_OBJECT draw_1628;
static Sint16 draw_1627_polygons[139];
static Sint32 draw_1627_vertices[135];
static NJS_CNK_MODEL draw_1627_model;
static NJS_CNK_OBJECT draw_1627;
static Sint16 draw_1625_polygons[829];
static Sint32 draw_1625_vertices[483];
static NJS_CNK_MODEL draw_1625_model;
static NJS_CNK_OBJECT draw_1625;
static Sint16 draw_646_polygons[1123];
static Sint32 draw_646_vertices[1115];
static NJS_CNK_MODEL draw_646_model;
static NJS_CNK_OBJECT draw_1624;
static Sint16 draw_1623_polygons[123];
static Sint32 draw_1623_vertices[99];
static NJS_CNK_MODEL draw_1623_model;
static NJS_CNK_OBJECT draw_1623;
static Sint16 draw_1622_polygons[343];
static Sint32 draw_1622_vertices[347];
static NJS_CNK_MODEL draw_1622_model;
static NJS_CNK_OBJECT draw_1622;
static Sint16 draw_642_polygons[229];
static Sint32 draw_642_vertices[211];
static NJS_CNK_MODEL draw_642_model;
static NJS_CNK_OBJECT draw_1621;
static Sint16 draw_1620_polygons[33];
static Sint32 draw_1620_vertices[35];
static NJS_CNK_MODEL draw_1620_model;
static NJS_CNK_OBJECT draw_1620;
static Sint16 draw_644_polygons[159];
static Sint32 draw_644_vertices[143];
static NJS_CNK_MODEL draw_644_model;
static NJS_CNK_OBJECT draw_1619;
static Sint16 draw_1618_polygons[509];
static Sint32 draw_1618_vertices[435];
static NJS_CNK_MODEL draw_1618_model;
static NJS_CNK_OBJECT draw_1618;
static Sint16 draw_1615_polygons[333];
static Sint32 draw_1615_vertices[343];
static NJS_CNK_MODEL draw_1615_model;
static NJS_CNK_OBJECT draw_1615;
static Sint16 draw_903_polygons[341];
static Sint32 draw_903_vertices[307];
static NJS_CNK_MODEL draw_903_model;
static NJS_CNK_OBJECT draw_1614;
static Sint16 draw_805_polygons[337];
static Sint32 draw_805_vertices[279];
static NJS_CNK_MODEL draw_805_model;
static NJS_CNK_OBJECT draw_1613;
static Sint16 draw_69_polygons[155];
static Sint32 draw_69_vertices[155];
static NJS_CNK_MODEL draw_69_model;
static NJS_CNK_OBJECT draw_1606;
static Sint16 draw_740_polygons[673];
static Sint32 draw_740_vertices[603];
static NJS_CNK_MODEL draw_740_model;
static NJS_CNK_OBJECT draw_1603;
static Sint16 draw_67_polygons[301];
static Sint32 draw_67_vertices[251];
static NJS_CNK_MODEL draw_67_model;
static NJS_CNK_OBJECT draw_1602;
static Sint16 draw_66_polygons[65];
static Sint32 draw_66_vertices[51];
static NJS_CNK_MODEL draw_66_model;
static NJS_CNK_OBJECT draw_1600;
static Sint16 draw_732_polygons[533];
static Sint32 draw_732_vertices[643];
static NJS_CNK_MODEL draw_732_model;
static NJS_CNK_OBJECT draw_1598;
static Sint16 draw_1596_polygons[155];
static Sint32 draw_1596_vertices[151];
static NJS_CNK_MODEL draw_1596_model;
static NJS_CNK_OBJECT draw_1596;
static Sint16 draw_1595_polygons[147];
static Sint32 draw_1595_vertices[115];
static NJS_CNK_MODEL draw_1595_model;
static NJS_CNK_OBJECT draw_1595;
static Sint16 draw_719_polygons[153];
static Sint32 draw_719_vertices[123];
static NJS_CNK_MODEL draw_719_model;
static NJS_CNK_OBJECT draw_1594;
static Sint16 draw_713_polygons[75];
static Sint32 draw_713_vertices[83];
static NJS_CNK_MODEL draw_713_model;
static NJS_CNK_OBJECT draw_1593;
static Sint16 draw_707_polygons[75];
static Sint32 draw_707_vertices[83];
static NJS_CNK_MODEL draw_707_model;
static NJS_CNK_OBJECT draw_1590;
static Sint16 draw_1585_polygons[409];
static Sint32 draw_1585_vertices[279];
static NJS_CNK_MODEL draw_1585_model;
static NJS_CNK_OBJECT draw_1585;
static Sint16 draw_119_polygons[323];
static Sint32 draw_119_vertices[243];
static NJS_CNK_MODEL draw_119_model;
static NJS_CNK_OBJECT draw_1584;
static Sint16 draw_828_polygons[455];
static Sint32 draw_828_vertices[375];
static NJS_CNK_MODEL draw_828_model;
static NJS_CNK_OBJECT draw_1583;
static Sint16 draw_1581_polygons[123];
static Sint32 draw_1581_vertices[99];
static NJS_CNK_MODEL draw_1581_model;
static NJS_CNK_OBJECT draw_1581;
static Sint16 draw_1580_polygons[243];
static Sint32 draw_1580_vertices[227];
static NJS_CNK_MODEL draw_1580_model;
static NJS_CNK_OBJECT draw_1580;
static Sint16 draw_814_polygons[533];
static Sint32 draw_814_vertices[643];
static NJS_CNK_MODEL draw_814_model;
static NJS_CNK_OBJECT draw_1579;
static Sint16 draw_1577_polygons[87];
static Sint32 draw_1577_vertices[67];
static NJS_CNK_MODEL draw_1577_model;
static NJS_CNK_OBJECT draw_1577;
static Sint16 draw_1576_polygons[245];
static Sint32 draw_1576_vertices[179];
static NJS_CNK_MODEL draw_1576_model;
static NJS_CNK_OBJECT draw_1576;
static Sint16 draw_811_polygons[95];
static Sint32 draw_811_vertices[107];
static NJS_CNK_MODEL draw_811_model;
static NJS_CNK_OBJECT draw_1571;
static Sint16 draw_1570_polygons[83];
static Sint32 draw_1570_vertices[83];
static NJS_CNK_MODEL draw_1570_model;
static NJS_CNK_OBJECT draw_1570;
static Sint16 draw_128_polygons[197];
static Sint32 draw_128_vertices[127];
static NJS_CNK_MODEL draw_128_model;
static NJS_CNK_OBJECT draw_1554;
static Sint16 draw_125_polygons[197];
static Sint32 draw_125_vertices[127];
static NJS_CNK_MODEL draw_125_model;
static NJS_CNK_OBJECT draw_1552;
static Sint16 draw_647_polygons[685];
static Sint32 draw_647_vertices[607];
static NJS_CNK_MODEL draw_647_model;
static NJS_CNK_OBJECT draw_1401;
static Sint16 draw_648_polygons[105];
static Sint32 draw_648_vertices[83];
static NJS_CNK_MODEL draw_648_model;
static NJS_CNK_OBJECT draw_1400;
static Sint16 draw_1399_polygons[107];
static Sint32 draw_1399_vertices[83];
static NJS_CNK_MODEL draw_1399_model;
static NJS_CNK_OBJECT draw_1399;
static Sint16 draw_819_polygons[55];
static Sint32 draw_819_vertices[43];
static NJS_CNK_MODEL draw_819_model;
static NJS_CNK_OBJECT draw_1395;
static Sint16 draw_643_polygons[349];
static Sint32 draw_643_vertices[311];
static NJS_CNK_MODEL draw_643_model;
static NJS_CNK_OBJECT draw_1386;
static Sint16 draw_96_polygons[229];
static Sint32 draw_96_vertices[155];
static NJS_CNK_MODEL draw_96_model;
static NJS_CNK_OBJECT draw_1385;
static Sint16 draw_1384_polygons[385];
static Sint32 draw_1384_vertices[291];
static NJS_CNK_MODEL draw_1384_model;
static NJS_CNK_OBJECT draw_1384;
static Sint16 draw_1383_polygons[641];
static Sint32 draw_1383_vertices[483];
static NJS_CNK_MODEL draw_1383_model;
static NJS_CNK_OBJECT draw_1383;
static Sint16 draw_139_polygons[121];
static Sint32 draw_139_vertices[147];
static NJS_CNK_MODEL draw_139_model;
static NJS_CNK_OBJECT draw_1340;
static Sint16 draw_196_polygons[97];
static Sint32 draw_196_vertices[115];
static NJS_CNK_MODEL draw_196_model;
static NJS_CNK_OBJECT draw_1318;
static Sint16 draw_222_polygons[159];
static Sint32 draw_222_vertices[179];
static NJS_CNK_MODEL draw_222_model;
static NJS_CNK_OBJECT draw_1255;
static Sint16 draw_1247_polygons[41];
static Sint32 draw_1247_vertices[43];
static NJS_CNK_MODEL draw_1247_model;
static NJS_CNK_OBJECT draw_1248;
static Sint16 draw_1140_polygons[177];
static Sint32 draw_1140_vertices[163];
static NJS_CNK_MODEL draw_1140_model;
static NJS_CNK_OBJECT draw_1241;
static Sint16 draw_1139_polygons[177];
static Sint32 draw_1139_vertices[163];
static NJS_CNK_MODEL draw_1139_model;
static NJS_CNK_OBJECT draw_1238;
static Sint16 draw_1223_polygons[137];
static Sint32 draw_1223_vertices[99];
static NJS_CNK_MODEL draw_1223_model;
static NJS_CNK_OBJECT draw_1223;
static Sint16 draw_1222_polygons[137];
static Sint32 draw_1222_vertices[99];
static NJS_CNK_MODEL draw_1222_model;
static NJS_CNK_OBJECT draw_1222;
static Sint16 draw_1216_polygons[145];
static Sint32 draw_1216_vertices[163];
static NJS_CNK_MODEL draw_1216_model;
static NJS_CNK_OBJECT draw_1216;
static Sint16 draw_1141_polygons[227];
static Sint32 draw_1141_vertices[175];
static NJS_CNK_MODEL draw_1141_model;
static NJS_CNK_OBJECT draw_1207;
static Sint16 draw_1145_polygons[29];
static Sint32 draw_1145_vertices[27];
static NJS_CNK_MODEL draw_1145_model;
static NJS_CNK_OBJECT draw_1206;
static Sint16 draw_1168_polygons[55];
static Sint32 draw_1168_vertices[43];
static NJS_CNK_MODEL draw_1168_model;
static NJS_CNK_OBJECT draw_1205;
static Sint16 draw_1201_polygons[85];
static Sint32 draw_1201_vertices[67];
static NJS_CNK_MODEL draw_1201_model;
static NJS_CNK_OBJECT draw_1204;
static Sint16 draw_1200_polygons[185];
static Sint32 draw_1200_vertices[139];
static NJS_CNK_MODEL draw_1200_model;
static NJS_CNK_OBJECT draw_1200;
static Sint16 draw_1196_polygons[85];
static Sint32 draw_1196_vertices[67];
static NJS_CNK_MODEL draw_1196_model;
static NJS_CNK_OBJECT draw_1199;
static Sint16 draw_1138_polygons[209];
static Sint32 draw_1138_vertices[147];
static NJS_CNK_MODEL draw_1138_model;
static NJS_CNK_OBJECT draw_1194;
static Sint16 draw_1137_polygons[209];
static Sint32 draw_1137_vertices[147];
static NJS_CNK_MODEL draw_1137_model;
static NJS_CNK_OBJECT draw_1193;
static Sint16 draw_1147_polygons[149];
static Sint32 draw_1147_vertices[115];
static NJS_CNK_MODEL draw_1147_model;
static NJS_CNK_OBJECT draw_1192;
static Sint16 draw_1154_polygons[149];
static Sint32 draw_1154_vertices[115];
static NJS_CNK_MODEL draw_1154_model;
static NJS_CNK_OBJECT draw_1191;
static Sint16 draw_1149_polygons[227];
static Sint32 draw_1149_vertices[175];
static NJS_CNK_MODEL draw_1149_model;
static NJS_CNK_OBJECT draw_1190;
static Sint16 draw_1170_polygons[409];
static Sint32 draw_1170_vertices[367];
static NJS_CNK_MODEL draw_1170_model;
static NJS_CNK_OBJECT draw_1179;
static Sint16 draw_1171_polygons[233];
static Sint32 draw_1171_vertices[235];
static NJS_CNK_MODEL draw_1171_model;
static NJS_CNK_OBJECT draw_1171;
static Sint16 draw_1169_polygons[337];
static Sint32 draw_1169_vertices[207];
static NJS_CNK_MODEL draw_1169_model;
static NJS_CNK_OBJECT draw_1169;
static Sint16 draw_1167_polygons[191];
static Sint32 draw_1167_vertices[151];
static NJS_CNK_MODEL draw_1167_model;
static NJS_CNK_OBJECT draw_1167;
static Sint16 draw_1165_polygons[55];
static Sint32 draw_1165_vertices[43];
static NJS_CNK_MODEL draw_1165_model;
static NJS_CNK_OBJECT draw_1166;
static Sint16 draw_1159_polygons[185];
static Sint32 draw_1159_vertices[139];
static NJS_CNK_MODEL draw_1159_model;
static NJS_CNK_OBJECT draw_1159;
static Sint16 draw_215_polygons[113];
static Sint32 draw_215_vertices[99];
static NJS_CNK_MODEL draw_215_model;
static NJS_CNK_OBJECT draw_1048;
static Sint16 draw_1016_polygons[15];
static Sint32 draw_1016_vertices[19];
static NJS_CNK_MODEL draw_1016_model;
static NJS_CNK_OBJECT draw_1016;
static Sint16 draw_91_polygons[503];
static Sint32 draw_91_vertices[443];
static NJS_CNK_MODEL draw_91_model;
static NJS_CNK_OBJECT draw_1015;
static Sint16 draw_730_polygons[437];
static Sint32 draw_730_vertices[515];
static NJS_CNK_MODEL draw_730_model;
static NJS_CNK_OBJECT draw_1003;
static Sint16 draw_731_polygons[865];
static Sint32 draw_731_vertices[831];
static NJS_CNK_MODEL draw_731_model;
static NJS_CNK_OBJECT draw_982;
static Sint16 draw_728_polygons[169];
static Sint32 draw_728_vertices[151];
static NJS_CNK_MODEL draw_728_model;
static NJS_CNK_OBJECT draw_978;
static Sint16 draw_726_polygons[409];
static Sint32 draw_726_vertices[415];
static NJS_CNK_MODEL draw_726_model;
static NJS_CNK_OBJECT draw_974;
static Sint16 draw_104_polygons[423];
static Sint32 draw_104_vertices[335];
static NJS_CNK_MODEL draw_104_model;
static NJS_CNK_OBJECT draw_970;
static Sint16 draw_904_polygons[533];
static Sint32 draw_904_vertices[487];
static NJS_CNK_MODEL draw_904_model;
static NJS_CNK_OBJECT draw_915;
static Sint16 draw_914_polygons[785];
static Sint32 draw_914_vertices[595];
static NJS_CNK_MODEL draw_914_model;
static NJS_CNK_OBJECT draw_914;
static Sint16 draw_913_polygons[789];
static Sint32 draw_913_vertices[711];
static NJS_CNK_MODEL draw_913_model;
static NJS_CNK_OBJECT draw_913;
static Sint16 draw_912_polygons[449];
static Sint32 draw_912_vertices[391];
static NJS_CNK_MODEL draw_912_model;
static NJS_CNK_OBJECT draw_912;
static Sint16 draw_909_polygons[485];
static Sint32 draw_909_vertices[419];
static NJS_CNK_MODEL draw_909_model;
static NJS_CNK_OBJECT draw_909;
static Sint16 draw_907_polygons[171];
static Sint32 draw_907_vertices[131];
static NJS_CNK_MODEL draw_907_model;
static NJS_CNK_OBJECT draw_907;
static Sint16 draw_906_polygons[95];
static Sint32 draw_906_vertices[75];
static NJS_CNK_MODEL draw_906_model;
static NJS_CNK_OBJECT draw_906;
static Sint16 draw_899_polygons[639];
static Sint32 draw_899_vertices[503];
static NJS_CNK_MODEL draw_899_model;
static NJS_CNK_OBJECT draw_899;
static Sint16 draw_749_polygons[837];
static Sint32 draw_749_vertices[819];
static NJS_CNK_MODEL draw_749_model;
static NJS_CNK_OBJECT draw_898;
static Sint16 draw_652_polygons[197];
static Sint32 draw_652_vertices[199];
static NJS_CNK_MODEL draw_652_model;
static NJS_CNK_OBJECT draw_897;
static Sint16 draw_724_polygons[401];
static Sint32 draw_724_vertices[411];
static NJS_CNK_MODEL draw_724_model;
static NJS_CNK_OBJECT draw_896;
static Sint16 draw_895_polygons[695];
static Sint32 draw_895_vertices[535];
static NJS_CNK_MODEL draw_895_model;
static NJS_CNK_OBJECT draw_895;
static Sint16 draw_722_polygons[445];
static Sint32 draw_722_vertices[519];
static NJS_CNK_MODEL draw_722_model;
static NJS_CNK_OBJECT draw_894;
static Sint16 draw_720_polygons[177];
static Sint32 draw_720_vertices[151];
static NJS_CNK_MODEL draw_720_model;
static NJS_CNK_OBJECT draw_892;
static Sint16 draw_651_polygons[267];
static Sint32 draw_651_vertices[291];
static NJS_CNK_MODEL draw_651_model;
static NJS_CNK_OBJECT draw_890;
static Sint16 draw_836_polygons[257];
static Sint32 draw_836_vertices[171];
static NJS_CNK_MODEL draw_836_model;
static NJS_CNK_OBJECT draw_889;
static Sint16 draw_888_polygons[171];
static Sint32 draw_888_vertices[119];
static NJS_CNK_MODEL draw_888_model;
static NJS_CNK_OBJECT draw_888;
static Sint16 draw_887_polygons[149];
static Sint32 draw_887_vertices[131];
static NJS_CNK_MODEL draw_887_model;
static NJS_CNK_OBJECT draw_887;
static Sint16 draw_886_polygons[353];
static Sint32 draw_886_vertices[247];
static NJS_CNK_MODEL draw_886_model;
static NJS_CNK_OBJECT draw_886;
static Sint16 draw_884_polygons[249];
static Sint32 draw_884_vertices[315];
static NJS_CNK_MODEL draw_884_model;
static NJS_CNK_OBJECT draw_884;
static Sint16 draw_883_polygons[249];
static Sint32 draw_883_vertices[287];
static NJS_CNK_MODEL draw_883_model;
static NJS_CNK_OBJECT draw_883;
static Sint16 draw_882_polygons[65];
static Sint32 draw_882_vertices[67];
static NJS_CNK_MODEL draw_882_model;
static NJS_CNK_OBJECT draw_882;
static Sint16 draw_881_polygons[249];
static Sint32 draw_881_vertices[311];
static NJS_CNK_MODEL draw_881_model;
static NJS_CNK_OBJECT draw_881;
static Sint16 draw_880_polygons[1011];
static Sint32 draw_880_vertices[711];
static NJS_CNK_MODEL draw_880_model;
static NJS_CNK_OBJECT draw_880;
static Sint16 draw_879_polygons[407];
static Sint32 draw_879_vertices[443];
static NJS_CNK_MODEL draw_879_model;
static NJS_CNK_OBJECT draw_879;
static Sint16 draw_877_polygons[101];
static Sint32 draw_877_vertices[87];
static NJS_CNK_MODEL draw_877_model;
static NJS_CNK_OBJECT draw_877;
static Sint16 draw_876_polygons[331];
static Sint32 draw_876_vertices[323];
static NJS_CNK_MODEL draw_876_model;
static NJS_CNK_OBJECT draw_876;
static Sint16 draw_869_polygons[69];
static Sint32 draw_869_vertices[59];
static NJS_CNK_MODEL draw_869_model;
static NJS_CNK_OBJECT draw_875;
static Sint16 draw_864_polygons[69];
static Sint32 draw_864_vertices[67];
static NJS_CNK_MODEL draw_864_model;
static NJS_CNK_OBJECT draw_868;
static Sint16 draw_866_polygons[211];
static Sint32 draw_866_vertices[199];
static NJS_CNK_MODEL draw_866_model;
static NJS_CNK_OBJECT draw_866;
static Sint16 draw_863_polygons[435];
static Sint32 draw_863_vertices[447];
static NJS_CNK_MODEL draw_863_model;
static NJS_CNK_OBJECT draw_863;
static Sint16 draw_809_polygons[73];
static Sint32 draw_809_vertices[51];
static NJS_CNK_MODEL draw_809_model;
static NJS_CNK_OBJECT draw_862;
static Sint16 draw_860_polygons[641];
static Sint32 draw_860_vertices[623];
static NJS_CNK_MODEL draw_860_model;
static NJS_CNK_OBJECT draw_860;
static Sint16 draw_859_polygons[709];
static Sint32 draw_859_vertices[671];
static NJS_CNK_MODEL draw_859_model;
static NJS_CNK_OBJECT draw_859;
static Sint16 draw_858_polygons[37];
static Sint32 draw_858_vertices[43];
static NJS_CNK_MODEL draw_858_model;
static NJS_CNK_OBJECT draw_858;
static Sint16 draw_857_polygons[249];
static Sint32 draw_857_vertices[231];
static NJS_CNK_MODEL draw_857_model;
static NJS_CNK_OBJECT draw_857;
static Sint16 draw_856_polygons[73];
static Sint32 draw_856_vertices[91];
static NJS_CNK_MODEL draw_856_model;
static NJS_CNK_OBJECT draw_856;
static Sint16 draw_855_polygons[73];
static Sint32 draw_855_vertices[91];
static NJS_CNK_MODEL draw_855_model;
static NJS_CNK_OBJECT draw_855;
static Sint16 draw_854_polygons[87];
static Sint32 draw_854_vertices[99];
static NJS_CNK_MODEL draw_854_model;
static NJS_CNK_OBJECT draw_854;
static Sint16 draw_851_polygons[619];
static Sint32 draw_851_vertices[427];
static NJS_CNK_MODEL draw_851_model;
static NJS_CNK_OBJECT draw_851;
static Sint16 draw_838_polygons[201];
static Sint32 draw_838_vertices[167];
static NJS_CNK_MODEL draw_838_model;
static NJS_CNK_OBJECT draw_838;
static Sint16 draw_835_polygons[269];
static Sint32 draw_835_vertices[171];
static NJS_CNK_MODEL draw_835_model;
static NJS_CNK_OBJECT draw_835;
static Sint16 draw_832_polygons[229];
static Sint32 draw_832_vertices[147];
static NJS_CNK_MODEL draw_832_model;
static NJS_CNK_OBJECT draw_832;
static Sint16 draw_831_polygons[809];
static Sint32 draw_831_vertices[631];
static NJS_CNK_MODEL draw_831_model;
static NJS_CNK_OBJECT draw_831;
static Sint16 draw_829_polygons[325];
static Sint32 draw_829_vertices[319];
static NJS_CNK_MODEL draw_829_model;
static NJS_CNK_OBJECT draw_829;
static Sint16 draw_827_polygons[323];
static Sint32 draw_827_vertices[195];
static NJS_CNK_MODEL draw_827_model;
static NJS_CNK_OBJECT draw_827;
static Sint16 draw_818_polygons[253];
static Sint32 draw_818_vertices[179];
static NJS_CNK_MODEL draw_818_model;
static NJS_CNK_OBJECT draw_818;
static Sint16 draw_817_polygons[107];
static Sint32 draw_817_vertices[107];
static NJS_CNK_MODEL draw_817_model;
static NJS_CNK_OBJECT draw_817;
static Sint16 draw_813_polygons[435];
static Sint32 draw_813_vertices[395];
static NJS_CNK_MODEL draw_813_model;
static NJS_CNK_OBJECT draw_813;
static Sint16 draw_812_polygons[495];
static Sint32 draw_812_vertices[447];
static NJS_CNK_MODEL draw_812_model;
static NJS_CNK_OBJECT draw_812;
static Sint16 draw_804_polygons[197];
static Sint32 draw_804_vertices[131];
static NJS_CNK_MODEL draw_804_model;
static NJS_CNK_OBJECT draw_804;
static Sint16 draw_653_polygons[105];
static Sint32 draw_653_vertices[83];
static NJS_CNK_MODEL draw_653_model;
static NJS_CNK_OBJECT draw_653;
static Sint16 draw_650_polygons[189];
static Sint32 draw_650_vertices[139];
static NJS_CNK_MODEL draw_650_model;
static NJS_CNK_OBJECT draw_650;
static Sint16 draw_649_polygons[667];
static Sint32 draw_649_vertices[463];
static NJS_CNK_MODEL draw_649_model;
static NJS_CNK_OBJECT draw_649;
static Sint16 draw_218_polygons[551];
static Sint32 draw_218_vertices[539];
static NJS_CNK_MODEL draw_218_model;
static NJS_CNK_OBJECT draw_299;
static Sint16 draw_225_polygons[103];
static Sint32 draw_225_vertices[115];
static NJS_CNK_MODEL draw_225_model;
static NJS_CNK_OBJECT draw_225;
static Sint16 draw_217_polygons[23];
static Sint32 draw_217_vertices[19];
static NJS_CNK_MODEL draw_217_model;
static NJS_CNK_OBJECT draw_217;
static Sint16 draw_195_polygons[43];
static Sint32 draw_195_vertices[35];
static NJS_CNK_MODEL draw_195_model;
static NJS_CNK_OBJECT draw_195;
static Sint16 draw_133_polygons[187];
static Sint32 draw_133_vertices[179];
static NJS_CNK_MODEL draw_133_model;
static NJS_CNK_OBJECT draw_133;
static Sint16 draw_80_polygons[247];
static Sint32 draw_80_vertices[235];
static NJS_CNK_MODEL draw_80_model;
static NJS_CNK_OBJECT draw_80;
static Sint16 draw_73_polygons[265];
static Sint32 draw_73_vertices[255];
static NJS_CNK_MODEL draw_73_model;
static NJS_CNK_OBJECT draw_73;
static Sint16 draw_51_polygons[87];
static Sint32 draw_51_vertices[103];
static NJS_CNK_MODEL draw_51_model;
static NJS_CNK_OBJECT draw_65;
static Sint16 draw_36_polygons[55];
static Sint32 draw_36_vertices[67];
static NJS_CNK_MODEL draw_36_model;
static NJS_CNK_OBJECT draw_64;
static Sint16 draw_35_polygons[19];
static Sint32 draw_35_vertices[19];
static NJS_CNK_MODEL draw_35_model;
static NJS_CNK_OBJECT draw_62;
static Sint16 draw_58_polygons[69];
static Sint32 draw_58_vertices[79];
static NJS_CNK_MODEL draw_58_model;
static NJS_CNK_OBJECT draw_59;
static Sint16 draw_39_polygons[19];
static Sint32 draw_39_vertices[19];
static NJS_CNK_MODEL draw_39_model;
static NJS_CNK_OBJECT draw_57;
static Sint16 draw_38_polygons[55];
static Sint32 draw_38_vertices[67];
static NJS_CNK_MODEL draw_38_model;
static NJS_CNK_OBJECT draw_38;
static Sint16 draw_37_polygons[55];
static Sint32 draw_37_vertices[67];
static NJS_CNK_MODEL draw_37_model;
static NJS_CNK_OBJECT draw_37;
static NJS_MATERIAL collision_533_materials[1];
static Sint16 collision_533_faces[39];
static NJS_MESHSET collision_533_meshsets[1];
static NJS_POINT3 collision_533_points[22];
static NJS_MODEL collision_533_model;
static NJS_OBJECT collision_533;
static Sint16 draw_34_polygons[195];
static Sint32 draw_34_vertices[187];
static NJS_CNK_MODEL draw_34_model;
static NJS_CNK_OBJECT draw_34;
static Sint16 draw_23_polygons[1371];
static Sint32 draw_23_vertices[1023];
static NJS_CNK_MODEL draw_23_model;
static NJS_CNK_OBJECT draw_33;
static Sint16 draw_11_polygons[1343];
static Sint32 draw_11_vertices[987];
static NJS_CNK_MODEL draw_11_model;
static NJS_CNK_OBJECT draw_22;
static Sint16 draw_3_polygons[1541];
static Sint32 draw_3_vertices[1067];
static NJS_CNK_MODEL draw_3_model;
static NJS_CNK_OBJECT draw_10;
static Sint16 draw_2_polygons[1211];
static Sint32 draw_2_vertices[927];
static NJS_CNK_MODEL draw_2_model;
static NJS_CNK_OBJECT draw_2;
static Sint16 draw_0_polygons[419];
static Sint32 draw_0_vertices[435];
static NJS_CNK_MODEL draw_0_model;
static NJS_CNK_OBJECT draw_1;
static NJS_OBJECT collision_19;
static NJS_OBJECT collision_23;
static NJS_OBJECT collision_25;
static NJS_OBJECT collision_26;
static NJS_OBJECT collision_27;
static NJS_OBJECT collision_31;
static NJS_OBJECT collision_44;
static NJS_OBJECT collision_46;
static NJS_OBJECT collision_51;
static NJS_OBJECT collision_54;
static NJS_OBJECT collision_59;
static NJS_OBJECT collision_61;
static NJS_OBJECT collision_64;
static NJS_OBJECT collision_65;
static NJS_OBJECT collision_67;
static NJS_OBJECT collision_68;
static NJS_OBJECT collision_77;
static NJS_OBJECT collision_94;
static NJS_OBJECT collision_95;
static NJS_OBJECT collision_96;
static NJS_OBJECT collision_97;
static NJS_OBJECT collision_98;
static NJS_OBJECT collision_99;
static NJS_OBJECT collision_100;
static NJS_OBJECT collision_103;
static NJS_OBJECT collision_104;
static NJS_OBJECT collision_106;
static NJS_OBJECT collision_107;
static NJS_OBJECT collision_109;
static NJS_OBJECT collision_110;
static NJS_OBJECT collision_112;
static NJS_OBJECT collision_114;
static NJS_OBJECT collision_116;
static NJS_OBJECT collision_117;
static NJS_OBJECT collision_119;
static NJS_OBJECT collision_123;
static NJS_OBJECT collision_124;
static NJS_OBJECT collision_126;
static NJS_OBJECT collision_127;
static NJS_OBJECT collision_129;
static NJS_OBJECT collision_130;
static NJS_OBJECT collision_132;
static NJS_OBJECT collision_133;
static NJS_OBJECT collision_135;
static NJS_OBJECT collision_136;
static NJS_OBJECT collision_138;
static NJS_OBJECT collision_139;
static NJS_OBJECT collision_140;
static NJS_OBJECT collision_142;
static NJS_OBJECT collision_143;
static NJS_OBJECT collision_145;
static NJS_OBJECT collision_147;
static NJS_OBJECT collision_149;
static NJS_OBJECT collision_151;
static NJS_OBJECT collision_153;
static NJS_OBJECT collision_155;
static NJS_OBJECT collision_160;
static NJS_OBJECT collision_165;
static NJS_OBJECT collision_167;
static NJS_OBJECT collision_168;
static NJS_OBJECT collision_169;
static NJS_OBJECT collision_170;
static NJS_OBJECT collision_171;
static NJS_OBJECT collision_173;
static NJS_OBJECT collision_176;
static NJS_OBJECT collision_180;
static NJS_OBJECT collision_182;
static NJS_OBJECT collision_183;
static NJS_OBJECT collision_184;
static NJS_OBJECT collision_186;
static NJS_OBJECT collision_191;
static NJS_OBJECT collision_192;
static NJS_OBJECT collision_193;
static NJS_OBJECT collision_195;
static NJS_OBJECT collision_208;
static NJS_OBJECT collision_209;
static NJS_OBJECT collision_210;
static NJS_OBJECT collision_211;
static NJS_OBJECT collision_212;
static NJS_OBJECT collision_213;
static NJS_OBJECT collision_214;
static NJS_OBJECT collision_215;
static NJS_OBJECT collision_216;
static NJS_OBJECT collision_217;
static NJS_OBJECT collision_218;
static NJS_OBJECT collision_219;
static NJS_OBJECT collision_220;
static NJS_OBJECT collision_221;
static NJS_OBJECT collision_222;
static NJS_OBJECT collision_224;
static NJS_OBJECT collision_225;
static NJS_OBJECT collision_226;
static NJS_OBJECT collision_243;
static NJS_OBJECT collision_244;
static NJS_OBJECT collision_246;
static NJS_OBJECT collision_247;
static NJS_OBJECT collision_248;
static NJS_OBJECT collision_249;
static NJS_OBJECT collision_250;
static NJS_OBJECT collision_251;
static NJS_OBJECT collision_252;
static NJS_OBJECT collision_256;
static NJS_OBJECT collision_257;
static NJS_OBJECT collision_258;
static NJS_OBJECT collision_259;
static NJS_OBJECT collision_260;
static NJS_OBJECT collision_261;
static NJS_OBJECT collision_262;
static NJS_OBJECT collision_275;
static NJS_OBJECT collision_277;
static NJS_OBJECT collision_280;
static NJS_OBJECT collision_289;
static NJS_OBJECT collision_290;
static NJS_OBJECT collision_292;
static NJS_OBJECT collision_293;
static NJS_OBJECT collision_294;
static NJS_OBJECT collision_295;
static NJS_OBJECT collision_302;
static NJS_OBJECT collision_303;
static NJS_OBJECT collision_304;
static NJS_OBJECT collision_305;
static NJS_OBJECT collision_307;
static NJS_OBJECT collision_308;
static NJS_OBJECT collision_309;
static NJS_OBJECT collision_310;
static NJS_OBJECT collision_314;
static NJS_OBJECT collision_315;
static NJS_OBJECT collision_338;
static NJS_OBJECT collision_339;
static NJS_OBJECT collision_340;
static NJS_OBJECT collision_348;
static NJS_OBJECT collision_356;
static NJS_OBJECT collision_362;
static NJS_OBJECT collision_365;
static NJS_OBJECT collision_368;
static NJS_OBJECT collision_370;
static NJS_OBJECT collision_372;
static NJS_OBJECT collision_377;
static NJS_OBJECT collision_386;
static NJS_OBJECT collision_392;
static NJS_OBJECT collision_395;
static NJS_OBJECT collision_396;
static NJS_OBJECT collision_398;
static NJS_OBJECT collision_400;
static NJS_OBJECT collision_401;
static NJS_OBJECT collision_403;
static NJS_OBJECT collision_404;
static NJS_OBJECT collision_407;
static NJS_OBJECT collision_408;
static NJS_OBJECT collision_410;
static NJS_OBJECT collision_411;
static NJS_OBJECT collision_414;
static NJS_OBJECT collision_415;
static NJS_OBJECT collision_417;
static NJS_OBJECT collision_420;
static NJS_OBJECT collision_421;
static NJS_OBJECT collision_423;
static NJS_OBJECT collision_424;
static NJS_OBJECT collision_432;
static NJS_OBJECT collision_435;
static NJS_OBJECT collision_437;
static NJS_OBJECT collision_444;
static NJS_OBJECT collision_445;
static NJS_OBJECT collision_471;
static NJS_OBJECT collision_473;
static NJS_OBJECT collision_475;
static NJS_OBJECT collision_480;
static NJS_OBJECT collision_482;
static NJS_OBJECT collision_494;
static NJS_OBJECT collision_495;
static NJS_OBJECT collision_498;
static NJS_OBJECT collision_500;
static NJS_OBJECT collision_501;
static NJS_OBJECT collision_503;
static NJS_OBJECT collision_504;
static NJS_OBJECT collision_505;
static NJS_OBJECT collision_506;
static NJS_OBJECT collision_507;
static NJS_OBJECT collision_508;
static NJS_OBJECT collision_510;
static NJS_OBJECT collision_511;
static NJS_OBJECT collision_512;
static NJS_OBJECT collision_513;
static NJS_OBJECT collision_514;
static NJS_OBJECT collision_515;
static NJS_OBJECT collision_516;
static NJS_OBJECT collision_518;
static NJS_OBJECT collision_519;
static NJS_OBJECT collision_520;
static NJS_OBJECT collision_521;
static NJS_OBJECT collision_522;
static NJS_OBJECT collision_523;
static NJS_OBJECT collision_524;
static NJS_OBJECT collision_526;
static NJS_OBJECT collision_528;
static NJS_CNK_OBJECT draw_2198;
static NJS_CNK_OBJECT draw_2196;
static NJS_CNK_OBJECT draw_2195;
static NJS_CNK_OBJECT draw_2192;
static NJS_CNK_OBJECT draw_2190;
static NJS_CNK_OBJECT draw_2189;
static NJS_CNK_OBJECT draw_2188;
static NJS_CNK_OBJECT draw_2186;
static NJS_CNK_OBJECT draw_2183;
static NJS_CNK_OBJECT draw_2181;
static NJS_CNK_OBJECT draw_2180;
static NJS_CNK_OBJECT draw_2179;
static NJS_CNK_OBJECT draw_2178;
static NJS_CNK_OBJECT draw_2177;
static NJS_CNK_OBJECT draw_2176;
static NJS_CNK_OBJECT draw_2175;
static NJS_CNK_OBJECT draw_2174;
static NJS_CNK_OBJECT draw_2173;
static NJS_CNK_OBJECT draw_2172;
static NJS_CNK_OBJECT draw_2171;
static NJS_CNK_OBJECT draw_2170;
static NJS_CNK_OBJECT draw_2169;
static NJS_CNK_OBJECT draw_2168;
static NJS_CNK_OBJECT draw_2167;
static NJS_CNK_OBJECT draw_2166;
static NJS_CNK_OBJECT draw_2165;
static NJS_CNK_OBJECT draw_2164;
static NJS_CNK_OBJECT draw_2163;
static NJS_CNK_OBJECT draw_2162;
static NJS_CNK_OBJECT draw_2161;
static NJS_CNK_OBJECT draw_2160;
static NJS_CNK_OBJECT draw_2159;
static NJS_CNK_OBJECT draw_2158;
static NJS_CNK_OBJECT draw_2157;
static NJS_CNK_OBJECT draw_2156;
static NJS_CNK_OBJECT draw_2155;
static NJS_CNK_OBJECT draw_2154;
static NJS_CNK_OBJECT draw_2153;
static NJS_CNK_OBJECT draw_2152;
static NJS_CNK_OBJECT draw_2151;
static NJS_CNK_OBJECT draw_2150;
static NJS_CNK_OBJECT draw_2149;
static NJS_CNK_OBJECT draw_2148;
static NJS_CNK_OBJECT draw_2147;
static NJS_CNK_OBJECT draw_2146;
static NJS_CNK_OBJECT draw_2145;
static NJS_CNK_OBJECT draw_2144;
static NJS_CNK_OBJECT draw_2143;
static NJS_CNK_OBJECT draw_2142;
static NJS_CNK_OBJECT draw_2141;
static NJS_CNK_OBJECT draw_2140;
static NJS_CNK_OBJECT draw_2139;
static NJS_CNK_OBJECT draw_2138;
static NJS_CNK_OBJECT draw_2137;
static NJS_CNK_OBJECT draw_2136;
static NJS_CNK_OBJECT draw_2135;
static NJS_CNK_OBJECT draw_2134;
static NJS_CNK_OBJECT draw_2133;
static NJS_CNK_OBJECT draw_2132;
static NJS_CNK_OBJECT draw_2131;
static NJS_CNK_OBJECT draw_2130;
static NJS_CNK_OBJECT draw_2129;
static NJS_CNK_OBJECT draw_2128;
static NJS_CNK_OBJECT draw_2127;
static NJS_CNK_OBJECT draw_2126;
static NJS_CNK_OBJECT draw_2125;
static NJS_CNK_OBJECT draw_2124;
static NJS_CNK_OBJECT draw_2123;
static NJS_CNK_OBJECT draw_2122;
static NJS_CNK_OBJECT draw_2121;
static NJS_CNK_OBJECT draw_2120;
static NJS_CNK_OBJECT draw_2119;
static NJS_CNK_OBJECT draw_2118;
static NJS_CNK_OBJECT draw_2117;
static NJS_CNK_OBJECT draw_2116;
static NJS_CNK_OBJECT draw_2115;
static NJS_CNK_OBJECT draw_2114;
static NJS_CNK_OBJECT draw_2112;
static NJS_CNK_OBJECT draw_2111;
static NJS_CNK_OBJECT draw_2110;
static NJS_CNK_OBJECT draw_2109;
static NJS_CNK_OBJECT draw_2108;
static NJS_CNK_OBJECT draw_2107;
static NJS_CNK_OBJECT draw_2106;
static NJS_CNK_OBJECT draw_2105;
static NJS_CNK_OBJECT draw_2104;
static NJS_CNK_OBJECT draw_2103;
static NJS_CNK_OBJECT draw_2102;
static NJS_CNK_OBJECT draw_2101;
static NJS_CNK_OBJECT draw_2100;
static NJS_CNK_OBJECT draw_2099;
static NJS_CNK_OBJECT draw_2098;
static NJS_CNK_OBJECT draw_2097;
static NJS_CNK_OBJECT draw_2096;
static NJS_CNK_OBJECT draw_2095;
static NJS_CNK_OBJECT draw_2094;
static NJS_CNK_OBJECT draw_2093;
static NJS_CNK_OBJECT draw_2092;
static NJS_CNK_OBJECT draw_2091;
static NJS_CNK_OBJECT draw_2090;
static NJS_CNK_OBJECT draw_2089;
static NJS_CNK_OBJECT draw_2088;
static NJS_CNK_OBJECT draw_2087;
static NJS_CNK_OBJECT draw_2086;
static NJS_CNK_OBJECT draw_2085;
static NJS_CNK_OBJECT draw_2084;
static NJS_CNK_OBJECT draw_2083;
static NJS_CNK_OBJECT draw_2082;
static NJS_CNK_OBJECT draw_2081;
static NJS_CNK_OBJECT draw_2080;
static NJS_CNK_OBJECT draw_2079;
static NJS_CNK_OBJECT draw_2078;
static NJS_CNK_OBJECT draw_2077;
static NJS_CNK_OBJECT draw_2076;
static NJS_CNK_OBJECT draw_2075;
static NJS_CNK_OBJECT draw_2074;
static NJS_CNK_OBJECT draw_2073;
static NJS_CNK_OBJECT draw_2072;
static NJS_CNK_OBJECT draw_2071;
static NJS_CNK_OBJECT draw_2070;
static NJS_CNK_OBJECT draw_2069;
static NJS_CNK_OBJECT draw_2068;
static NJS_CNK_OBJECT draw_2067;
static NJS_CNK_OBJECT draw_2066;
static NJS_CNK_OBJECT draw_2065;
static NJS_CNK_OBJECT draw_2064;
static NJS_CNK_OBJECT draw_2063;
static NJS_CNK_OBJECT draw_2062;
static NJS_CNK_OBJECT draw_2061;
static NJS_CNK_OBJECT draw_2060;
static NJS_CNK_OBJECT draw_2059;
static NJS_CNK_OBJECT draw_2058;
static NJS_CNK_OBJECT draw_2057;
static NJS_CNK_OBJECT draw_2056;
static NJS_CNK_OBJECT draw_2055;
static NJS_CNK_OBJECT draw_2054;
static NJS_CNK_OBJECT draw_2052;
static NJS_CNK_OBJECT draw_2051;
static NJS_CNK_OBJECT draw_2050;
static NJS_CNK_OBJECT draw_2049;
static NJS_CNK_OBJECT draw_2048;
static NJS_CNK_OBJECT draw_2046;
static NJS_CNK_OBJECT draw_2045;
static NJS_CNK_OBJECT draw_2043;
static NJS_CNK_OBJECT draw_2040;
static NJS_CNK_OBJECT draw_2036;
static NJS_CNK_OBJECT draw_2035;
static NJS_CNK_OBJECT draw_2034;
static NJS_CNK_OBJECT draw_2033;
static NJS_CNK_OBJECT draw_2030;
static NJS_CNK_OBJECT draw_2029;
static NJS_CNK_OBJECT draw_2028;
static NJS_CNK_OBJECT draw_2027;
static NJS_CNK_OBJECT draw_2026;
static NJS_CNK_OBJECT draw_2025;
static NJS_CNK_OBJECT draw_2023;
static NJS_CNK_OBJECT draw_2022;
static NJS_CNK_OBJECT draw_2021;
static NJS_CNK_OBJECT draw_2020;
static NJS_CNK_OBJECT draw_2019;
static NJS_CNK_OBJECT draw_2015;
static NJS_CNK_OBJECT draw_2014;
static NJS_CNK_OBJECT draw_2013;
static NJS_CNK_OBJECT draw_2010;
static NJS_CNK_OBJECT draw_2008;
static NJS_CNK_OBJECT draw_2007;
static NJS_CNK_OBJECT draw_2005;
static NJS_CNK_OBJECT draw_2004;
static NJS_CNK_OBJECT draw_2003;
static NJS_CNK_OBJECT draw_2002;
static NJS_CNK_OBJECT draw_2001;
static NJS_CNK_OBJECT draw_1998;
static NJS_CNK_OBJECT draw_1996;
static NJS_CNK_OBJECT draw_1995;
static NJS_CNK_OBJECT draw_1992;
static NJS_CNK_OBJECT draw_1990;
static NJS_CNK_OBJECT draw_1989;
static NJS_CNK_OBJECT draw_1988;
static NJS_CNK_OBJECT draw_1983;
static NJS_CNK_OBJECT draw_1953;
static NJS_CNK_OBJECT draw_1952;
static NJS_CNK_OBJECT draw_1951;
static NJS_CNK_OBJECT draw_1950;
static NJS_CNK_OBJECT draw_1948;
static NJS_CNK_OBJECT draw_1947;
static NJS_CNK_OBJECT draw_1946;
static NJS_CNK_OBJECT draw_1945;
static NJS_CNK_OBJECT draw_1944;
static NJS_CNK_OBJECT draw_1943;
static NJS_CNK_OBJECT draw_1942;
static NJS_CNK_OBJECT draw_1941;
static NJS_CNK_OBJECT draw_1940;
static NJS_CNK_OBJECT draw_1939;
static NJS_CNK_OBJECT draw_1938;
static NJS_CNK_OBJECT draw_1937;
static NJS_CNK_OBJECT draw_1936;
static NJS_CNK_OBJECT draw_1935;
static NJS_CNK_OBJECT draw_1934;
static NJS_CNK_OBJECT draw_1933;
static NJS_CNK_OBJECT draw_1932;
static NJS_CNK_OBJECT draw_1930;
static NJS_CNK_OBJECT draw_1929;
static NJS_CNK_OBJECT draw_1928;
static NJS_CNK_OBJECT draw_1927;
static NJS_CNK_OBJECT draw_1926;
static NJS_CNK_OBJECT draw_1925;
static NJS_CNK_OBJECT draw_1924;
static NJS_CNK_OBJECT draw_1923;
static NJS_CNK_OBJECT draw_1922;
static NJS_CNK_OBJECT draw_1921;
static NJS_CNK_OBJECT draw_1920;
static NJS_CNK_OBJECT draw_1919;
static NJS_CNK_OBJECT draw_1918;
static NJS_CNK_OBJECT draw_1917;
static NJS_CNK_OBJECT draw_1916;
static NJS_CNK_OBJECT draw_1915;
static NJS_CNK_OBJECT draw_1914;
static NJS_CNK_OBJECT draw_1913;
static NJS_CNK_OBJECT draw_1912;
static NJS_CNK_OBJECT draw_1911;
static NJS_CNK_OBJECT draw_1910;
static NJS_CNK_OBJECT draw_1909;
static NJS_CNK_OBJECT draw_1908;
static NJS_CNK_OBJECT draw_1907;
static NJS_CNK_OBJECT draw_1906;
static NJS_CNK_OBJECT draw_1905;
static NJS_CNK_OBJECT draw_1904;
static NJS_CNK_OBJECT draw_1903;
static NJS_CNK_OBJECT draw_1902;
static NJS_CNK_OBJECT draw_1901;
static NJS_CNK_OBJECT draw_1900;
static NJS_CNK_OBJECT draw_1899;
static NJS_CNK_OBJECT draw_1898;
static NJS_CNK_OBJECT draw_1897;
static NJS_CNK_OBJECT draw_1896;
static NJS_CNK_OBJECT draw_1895;
static NJS_CNK_OBJECT draw_1894;
static NJS_CNK_OBJECT draw_1893;
static NJS_CNK_OBJECT draw_1892;
static NJS_CNK_OBJECT draw_1891;
static NJS_CNK_OBJECT draw_1890;
static NJS_CNK_OBJECT draw_1889;
static NJS_CNK_OBJECT draw_1888;
static NJS_CNK_OBJECT draw_1887;
static NJS_CNK_OBJECT draw_1886;
static NJS_CNK_OBJECT draw_1885;
static NJS_CNK_OBJECT draw_1884;
static NJS_CNK_OBJECT draw_1883;
static NJS_CNK_OBJECT draw_1882;
static NJS_CNK_OBJECT draw_1881;
static NJS_CNK_OBJECT draw_1880;
static NJS_CNK_OBJECT draw_1879;
static NJS_CNK_OBJECT draw_1868;
static NJS_CNK_OBJECT draw_1867;
static NJS_CNK_OBJECT draw_1866;
static NJS_CNK_OBJECT draw_1865;
static NJS_CNK_OBJECT draw_1864;
static NJS_CNK_OBJECT draw_1863;
static NJS_CNK_OBJECT draw_1862;
static NJS_CNK_OBJECT draw_1861;
static NJS_CNK_OBJECT draw_1860;
static NJS_CNK_OBJECT draw_1859;
static NJS_CNK_OBJECT draw_1857;
static NJS_CNK_OBJECT draw_1855;
static NJS_CNK_OBJECT draw_1854;
static NJS_CNK_OBJECT draw_1850;
static NJS_CNK_OBJECT draw_1849;
static NJS_CNK_OBJECT draw_1848;
static NJS_CNK_OBJECT draw_1845;
static NJS_CNK_OBJECT draw_1844;
static NJS_CNK_OBJECT draw_1843;
static NJS_CNK_OBJECT draw_1841;
static NJS_CNK_OBJECT draw_1840;
static NJS_CNK_OBJECT draw_1839;
static NJS_CNK_OBJECT draw_1838;
static NJS_CNK_OBJECT draw_1836;
static NJS_CNK_OBJECT draw_1835;
static NJS_CNK_OBJECT draw_1833;
static NJS_CNK_OBJECT draw_1832;
static NJS_CNK_OBJECT draw_1831;
static NJS_CNK_OBJECT draw_1830;
static NJS_CNK_OBJECT draw_1829;
static NJS_CNK_OBJECT draw_1827;
static NJS_CNK_OBJECT draw_1826;
static NJS_CNK_OBJECT draw_1824;
static NJS_CNK_OBJECT draw_1823;
static NJS_CNK_OBJECT draw_1822;
static NJS_CNK_OBJECT draw_1821;
static NJS_CNK_OBJECT draw_1820;
static NJS_CNK_OBJECT draw_1819;
static NJS_CNK_OBJECT draw_1818;
static NJS_CNK_OBJECT draw_1817;
static NJS_CNK_OBJECT draw_1816;
static NJS_CNK_OBJECT draw_1815;
static NJS_CNK_OBJECT draw_1814;
static NJS_CNK_OBJECT draw_1813;
static NJS_CNK_OBJECT draw_1812;
static NJS_CNK_OBJECT draw_1811;
static NJS_CNK_OBJECT draw_1810;
static NJS_CNK_OBJECT draw_1809;
static NJS_CNK_OBJECT draw_1808;
static NJS_CNK_OBJECT draw_1807;
static NJS_CNK_OBJECT draw_1806;
static NJS_CNK_OBJECT draw_1805;
static NJS_CNK_OBJECT draw_1804;
static NJS_CNK_OBJECT draw_1802;
static NJS_CNK_OBJECT draw_1801;
static NJS_CNK_OBJECT draw_1800;
static NJS_CNK_OBJECT draw_1799;
static NJS_CNK_OBJECT draw_1798;
static NJS_CNK_OBJECT draw_1797;
static NJS_CNK_OBJECT draw_1796;
static NJS_CNK_OBJECT draw_1795;
static NJS_CNK_OBJECT draw_1794;
static NJS_CNK_OBJECT draw_1793;
static NJS_CNK_OBJECT draw_1792;
static NJS_CNK_OBJECT draw_1791;
static NJS_CNK_OBJECT draw_1790;
static NJS_CNK_OBJECT draw_1789;
static NJS_CNK_OBJECT draw_1788;
static NJS_CNK_OBJECT draw_1787;
static NJS_CNK_OBJECT draw_1786;
static NJS_CNK_OBJECT draw_1785;
static NJS_CNK_OBJECT draw_1784;
static NJS_CNK_OBJECT draw_1783;
static NJS_CNK_OBJECT draw_1782;
static NJS_CNK_OBJECT draw_1781;
static NJS_CNK_OBJECT draw_1780;
static NJS_CNK_OBJECT draw_1779;
static NJS_CNK_OBJECT draw_1778;
static NJS_CNK_OBJECT draw_1777;
static NJS_CNK_OBJECT draw_1776;
static NJS_CNK_OBJECT draw_1775;
static NJS_CNK_OBJECT draw_1774;
static NJS_CNK_OBJECT draw_1773;
static NJS_CNK_OBJECT draw_1772;
static NJS_CNK_OBJECT draw_1771;
static NJS_CNK_OBJECT draw_1770;
static NJS_CNK_OBJECT draw_1769;
static NJS_CNK_OBJECT draw_1765;
static NJS_CNK_OBJECT draw_1764;
static NJS_CNK_OBJECT draw_1762;
static NJS_CNK_OBJECT draw_1761;
static NJS_CNK_OBJECT draw_1760;
static NJS_CNK_OBJECT draw_1759;
static NJS_CNK_OBJECT draw_1758;
static NJS_CNK_OBJECT draw_1757;
static NJS_CNK_OBJECT draw_1756;
static NJS_CNK_OBJECT draw_1755;
static NJS_CNK_OBJECT draw_1754;
static NJS_CNK_OBJECT draw_1753;
static NJS_CNK_OBJECT draw_1752;
static NJS_CNK_OBJECT draw_1751;
static NJS_CNK_OBJECT draw_1750;
static NJS_CNK_OBJECT draw_1749;
static NJS_CNK_OBJECT draw_1748;
static NJS_CNK_OBJECT draw_1747;
static NJS_CNK_OBJECT draw_1746;
static NJS_CNK_OBJECT draw_1745;
static NJS_CNK_OBJECT draw_1742;
static NJS_CNK_OBJECT draw_1741;
static NJS_CNK_OBJECT draw_1740;
static NJS_CNK_OBJECT draw_1739;
static NJS_CNK_OBJECT draw_1738;
static NJS_CNK_OBJECT draw_1737;
static NJS_CNK_OBJECT draw_1736;
static NJS_CNK_OBJECT draw_1735;
static NJS_CNK_OBJECT draw_1734;
static NJS_CNK_OBJECT draw_1733;
static NJS_CNK_OBJECT draw_1732;
static NJS_CNK_OBJECT draw_1731;
static NJS_CNK_OBJECT draw_1730;
static NJS_CNK_OBJECT draw_1729;
static NJS_CNK_OBJECT draw_1728;
static NJS_CNK_OBJECT draw_1727;
static NJS_CNK_OBJECT draw_1726;
static NJS_CNK_OBJECT draw_1725;
static NJS_CNK_OBJECT draw_1724;
static NJS_CNK_OBJECT draw_1723;
static NJS_CNK_OBJECT draw_1722;
static NJS_CNK_OBJECT draw_1721;
static NJS_CNK_OBJECT draw_1720;
static NJS_CNK_OBJECT draw_1719;
static NJS_CNK_OBJECT draw_1718;
static NJS_CNK_OBJECT draw_1717;
static NJS_CNK_OBJECT draw_1716;
static NJS_CNK_OBJECT draw_1715;
static NJS_CNK_OBJECT draw_1714;
static NJS_CNK_OBJECT draw_1713;
static NJS_CNK_OBJECT draw_1712;
static NJS_CNK_OBJECT draw_1711;
static NJS_CNK_OBJECT draw_1710;
static NJS_CNK_OBJECT draw_1709;
static NJS_CNK_OBJECT draw_1708;
static NJS_CNK_OBJECT draw_1707;
static NJS_CNK_OBJECT draw_1706;
static NJS_CNK_OBJECT draw_1705;
static NJS_CNK_OBJECT draw_1704;
static NJS_CNK_OBJECT draw_1703;
static NJS_CNK_OBJECT draw_1702;
static NJS_CNK_OBJECT draw_1701;
static NJS_CNK_OBJECT draw_1700;
static NJS_CNK_OBJECT draw_1699;
static NJS_CNK_OBJECT draw_1698;
static NJS_CNK_OBJECT draw_1697;
static NJS_CNK_OBJECT draw_1696;
static NJS_CNK_OBJECT draw_1695;
static NJS_CNK_OBJECT draw_1694;
static NJS_CNK_OBJECT draw_1693;
static NJS_CNK_OBJECT draw_1692;
static NJS_CNK_OBJECT draw_1691;
static NJS_CNK_OBJECT draw_1690;
static NJS_CNK_OBJECT draw_1689;
static NJS_CNK_OBJECT draw_1688;
static NJS_CNK_OBJECT draw_1687;
static NJS_CNK_OBJECT draw_1686;
static NJS_CNK_OBJECT draw_1685;
static NJS_CNK_OBJECT draw_1684;
static NJS_CNK_OBJECT draw_1683;
static NJS_CNK_OBJECT draw_1682;
static NJS_CNK_OBJECT draw_1681;
static NJS_CNK_OBJECT draw_1680;
static NJS_CNK_OBJECT draw_1679;
static NJS_CNK_OBJECT draw_1678;
static NJS_CNK_OBJECT draw_1677;
static NJS_CNK_OBJECT draw_1676;
static NJS_CNK_OBJECT draw_1675;
static NJS_CNK_OBJECT draw_1674;
static NJS_CNK_OBJECT draw_1673;
static NJS_CNK_OBJECT draw_1671;
static NJS_CNK_OBJECT draw_1670;
static NJS_CNK_OBJECT draw_1669;
static NJS_CNK_OBJECT draw_1668;
static NJS_CNK_OBJECT draw_1667;
static NJS_CNK_OBJECT draw_1666;
static NJS_CNK_OBJECT draw_1665;
static NJS_CNK_OBJECT draw_1664;
static NJS_CNK_OBJECT draw_1663;
static NJS_CNK_OBJECT draw_1662;
static NJS_CNK_OBJECT draw_1661;
static NJS_CNK_OBJECT draw_1660;
static NJS_CNK_OBJECT draw_1659;
static NJS_CNK_OBJECT draw_1658;
static NJS_CNK_OBJECT draw_1656;
static NJS_CNK_OBJECT draw_1654;
static NJS_CNK_OBJECT draw_1653;
static NJS_CNK_OBJECT draw_1652;
static NJS_CNK_OBJECT draw_1651;
static NJS_CNK_OBJECT draw_1650;
static NJS_CNK_OBJECT draw_1649;
static NJS_CNK_OBJECT draw_1648;
static NJS_CNK_OBJECT draw_1647;
static NJS_CNK_OBJECT draw_1646;
static NJS_CNK_OBJECT draw_1645;
static NJS_CNK_OBJECT draw_1644;
static NJS_CNK_OBJECT draw_1643;
static NJS_CNK_OBJECT draw_1642;
static NJS_CNK_OBJECT draw_1641;
static NJS_CNK_OBJECT draw_1638;
static NJS_CNK_OBJECT draw_1635;
static NJS_CNK_OBJECT draw_1626;
static NJS_CNK_OBJECT draw_1617;
static NJS_CNK_OBJECT draw_1616;
static NJS_CNK_OBJECT draw_1612;
static NJS_CNK_OBJECT draw_1611;
static NJS_CNK_OBJECT draw_1610;
static NJS_CNK_OBJECT draw_1609;
static NJS_CNK_OBJECT draw_1608;
static NJS_CNK_OBJECT draw_1607;
static NJS_CNK_OBJECT draw_1605;
static NJS_CNK_OBJECT draw_1604;
static NJS_CNK_OBJECT draw_1601;
static NJS_CNK_OBJECT draw_1599;
static NJS_CNK_OBJECT draw_1597;
static NJS_CNK_OBJECT draw_1592;
static NJS_CNK_OBJECT draw_1591;
static NJS_CNK_OBJECT draw_1589;
static NJS_CNK_OBJECT draw_1588;
static NJS_CNK_OBJECT draw_1587;
static NJS_CNK_OBJECT draw_1586;
static NJS_CNK_OBJECT draw_1582;
static NJS_CNK_OBJECT draw_1578;
static NJS_CNK_OBJECT draw_1575;
static NJS_CNK_OBJECT draw_1574;
static NJS_CNK_OBJECT draw_1573;
static NJS_CNK_OBJECT draw_1572;
static NJS_CNK_OBJECT draw_1569;
static NJS_CNK_OBJECT draw_1568;
static NJS_CNK_OBJECT draw_1567;
static NJS_CNK_OBJECT draw_1566;
static NJS_CNK_OBJECT draw_1565;
static NJS_CNK_OBJECT draw_1564;
static NJS_CNK_OBJECT draw_1563;
static NJS_CNK_OBJECT draw_1562;
static NJS_CNK_OBJECT draw_1561;
static NJS_CNK_OBJECT draw_1560;
static NJS_CNK_OBJECT draw_1559;
static NJS_CNK_OBJECT draw_1558;
static NJS_CNK_OBJECT draw_1557;
static NJS_CNK_OBJECT draw_1556;
static NJS_CNK_OBJECT draw_1555;
static NJS_CNK_OBJECT draw_1553;
static NJS_CNK_OBJECT draw_1551;
static NJS_CNK_OBJECT draw_1550;
static NJS_CNK_OBJECT draw_1549;
static NJS_CNK_OBJECT draw_1548;
static NJS_CNK_OBJECT draw_1547;
static NJS_CNK_OBJECT draw_1546;
static NJS_CNK_OBJECT draw_1545;
static NJS_CNK_OBJECT draw_1544;
static NJS_CNK_OBJECT draw_1543;
static NJS_CNK_OBJECT draw_1542;
static NJS_CNK_OBJECT draw_1541;
static NJS_CNK_OBJECT draw_1540;
static NJS_CNK_OBJECT draw_1539;
static NJS_CNK_OBJECT draw_1538;
static NJS_CNK_OBJECT draw_1537;
static NJS_CNK_OBJECT draw_1536;
static NJS_CNK_OBJECT draw_1535;
static NJS_CNK_OBJECT draw_1534;
static NJS_CNK_OBJECT draw_1533;
static NJS_CNK_OBJECT draw_1532;
static NJS_CNK_OBJECT draw_1531;
static NJS_CNK_OBJECT draw_1530;
static NJS_CNK_OBJECT draw_1529;
static NJS_CNK_OBJECT draw_1528;
static NJS_CNK_OBJECT draw_1527;
static NJS_CNK_OBJECT draw_1526;
static NJS_CNK_OBJECT draw_1525;
static NJS_CNK_OBJECT draw_1524;
static NJS_CNK_OBJECT draw_1523;
static NJS_CNK_OBJECT draw_1522;
static NJS_CNK_OBJECT draw_1521;
static NJS_CNK_OBJECT draw_1520;
static NJS_CNK_OBJECT draw_1519;
static NJS_CNK_OBJECT draw_1518;
static NJS_CNK_OBJECT draw_1517;
static NJS_CNK_OBJECT draw_1516;
static NJS_CNK_OBJECT draw_1515;
static NJS_CNK_OBJECT draw_1514;
static NJS_CNK_OBJECT draw_1513;
static NJS_CNK_OBJECT draw_1512;
static NJS_CNK_OBJECT draw_1511;
static NJS_CNK_OBJECT draw_1510;
static NJS_CNK_OBJECT draw_1509;
static NJS_CNK_OBJECT draw_1508;
static NJS_CNK_OBJECT draw_1507;
static NJS_CNK_OBJECT draw_1506;
static NJS_CNK_OBJECT draw_1505;
static NJS_CNK_OBJECT draw_1504;
static NJS_CNK_OBJECT draw_1503;
static NJS_CNK_OBJECT draw_1502;
static NJS_CNK_OBJECT draw_1501;
static NJS_CNK_OBJECT draw_1500;
static NJS_CNK_OBJECT draw_1499;
static NJS_CNK_OBJECT draw_1498;
static NJS_CNK_OBJECT draw_1497;
static NJS_CNK_OBJECT draw_1496;
static NJS_CNK_OBJECT draw_1495;
static NJS_CNK_OBJECT draw_1494;
static NJS_CNK_OBJECT draw_1493;
static NJS_CNK_OBJECT draw_1492;
static NJS_CNK_OBJECT draw_1491;
static NJS_CNK_OBJECT draw_1490;
static NJS_CNK_OBJECT draw_1489;
static NJS_CNK_OBJECT draw_1488;
static NJS_CNK_OBJECT draw_1487;
static NJS_CNK_OBJECT draw_1486;
static NJS_CNK_OBJECT draw_1485;
static NJS_CNK_OBJECT draw_1484;
static NJS_CNK_OBJECT draw_1483;
static NJS_CNK_OBJECT draw_1482;
static NJS_CNK_OBJECT draw_1481;
static NJS_CNK_OBJECT draw_1480;
static NJS_CNK_OBJECT draw_1479;
static NJS_CNK_OBJECT draw_1478;
static NJS_CNK_OBJECT draw_1477;
static NJS_CNK_OBJECT draw_1476;
static NJS_CNK_OBJECT draw_1475;
static NJS_CNK_OBJECT draw_1474;
static NJS_CNK_OBJECT draw_1473;
static NJS_CNK_OBJECT draw_1472;
static NJS_CNK_OBJECT draw_1471;
static NJS_CNK_OBJECT draw_1470;
static NJS_CNK_OBJECT draw_1469;
static NJS_CNK_OBJECT draw_1468;
static NJS_CNK_OBJECT draw_1467;
static NJS_CNK_OBJECT draw_1466;
static NJS_CNK_OBJECT draw_1465;
static NJS_CNK_OBJECT draw_1464;
static NJS_CNK_OBJECT draw_1463;
static NJS_CNK_OBJECT draw_1462;
static NJS_CNK_OBJECT draw_1461;
static NJS_CNK_OBJECT draw_1460;
static NJS_CNK_OBJECT draw_1459;
static NJS_CNK_OBJECT draw_1458;
static NJS_CNK_OBJECT draw_1457;
static NJS_CNK_OBJECT draw_1456;
static NJS_CNK_OBJECT draw_1455;
static NJS_CNK_OBJECT draw_1454;
static NJS_CNK_OBJECT draw_1453;
static NJS_CNK_OBJECT draw_1452;
static NJS_CNK_OBJECT draw_1451;
static NJS_CNK_OBJECT draw_1450;
static NJS_CNK_OBJECT draw_1449;
static NJS_CNK_OBJECT draw_1448;
static NJS_CNK_OBJECT draw_1447;
static NJS_CNK_OBJECT draw_1446;
static NJS_CNK_OBJECT draw_1445;
static NJS_CNK_OBJECT draw_1444;
static NJS_CNK_OBJECT draw_1443;
static NJS_CNK_OBJECT draw_1442;
static NJS_CNK_OBJECT draw_1441;
static NJS_CNK_OBJECT draw_1440;
static NJS_CNK_OBJECT draw_1439;
static NJS_CNK_OBJECT draw_1438;
static NJS_CNK_OBJECT draw_1437;
static NJS_CNK_OBJECT draw_1436;
static NJS_CNK_OBJECT draw_1435;
static NJS_CNK_OBJECT draw_1434;
static NJS_CNK_OBJECT draw_1433;
static NJS_CNK_OBJECT draw_1432;
static NJS_CNK_OBJECT draw_1431;
static NJS_CNK_OBJECT draw_1430;
static NJS_CNK_OBJECT draw_1429;
static NJS_CNK_OBJECT draw_1428;
static NJS_CNK_OBJECT draw_1427;
static NJS_CNK_OBJECT draw_1426;
static NJS_CNK_OBJECT draw_1425;
static NJS_CNK_OBJECT draw_1424;
static NJS_CNK_OBJECT draw_1423;
static NJS_CNK_OBJECT draw_1422;
static NJS_CNK_OBJECT draw_1421;
static NJS_CNK_OBJECT draw_1420;
static NJS_CNK_OBJECT draw_1419;
static NJS_CNK_OBJECT draw_1418;
static NJS_CNK_OBJECT draw_1417;
static NJS_CNK_OBJECT draw_1416;
static NJS_CNK_OBJECT draw_1415;
static NJS_CNK_OBJECT draw_1414;
static NJS_CNK_OBJECT draw_1413;
static NJS_CNK_OBJECT draw_1412;
static NJS_CNK_OBJECT draw_1411;
static NJS_CNK_OBJECT draw_1410;
static NJS_CNK_OBJECT draw_1409;
static NJS_CNK_OBJECT draw_1408;
static NJS_CNK_OBJECT draw_1407;
static NJS_CNK_OBJECT draw_1406;
static NJS_CNK_OBJECT draw_1405;
static NJS_CNK_OBJECT draw_1404;
static NJS_CNK_OBJECT draw_1403;
static NJS_CNK_OBJECT draw_1402;
static NJS_CNK_OBJECT draw_1398;
static NJS_CNK_OBJECT draw_1397;
static NJS_CNK_OBJECT draw_1396;
static NJS_CNK_OBJECT draw_1394;
static NJS_CNK_OBJECT draw_1393;
static NJS_CNK_OBJECT draw_1392;
static NJS_CNK_OBJECT draw_1391;
static NJS_CNK_OBJECT draw_1390;
static NJS_CNK_OBJECT draw_1389;
static NJS_CNK_OBJECT draw_1388;
static NJS_CNK_OBJECT draw_1387;
static NJS_CNK_OBJECT draw_1382;
static NJS_CNK_OBJECT draw_1381;
static NJS_CNK_OBJECT draw_1380;
static NJS_CNK_OBJECT draw_1379;
static NJS_CNK_OBJECT draw_1378;
static NJS_CNK_OBJECT draw_1377;
static NJS_CNK_OBJECT draw_1376;
static NJS_CNK_OBJECT draw_1375;
static NJS_CNK_OBJECT draw_1374;
static NJS_CNK_OBJECT draw_1373;
static NJS_CNK_OBJECT draw_1372;
static NJS_CNK_OBJECT draw_1371;
static NJS_CNK_OBJECT draw_1370;
static NJS_CNK_OBJECT draw_1369;
static NJS_CNK_OBJECT draw_1368;
static NJS_CNK_OBJECT draw_1367;
static NJS_CNK_OBJECT draw_1366;
static NJS_CNK_OBJECT draw_1365;
static NJS_CNK_OBJECT draw_1364;
static NJS_CNK_OBJECT draw_1363;
static NJS_CNK_OBJECT draw_1362;
static NJS_CNK_OBJECT draw_1361;
static NJS_CNK_OBJECT draw_1360;
static NJS_CNK_OBJECT draw_1359;
static NJS_CNK_OBJECT draw_1358;
static NJS_CNK_OBJECT draw_1357;
static NJS_CNK_OBJECT draw_1356;
static NJS_CNK_OBJECT draw_1355;
static NJS_CNK_OBJECT draw_1354;
static NJS_CNK_OBJECT draw_1353;
static NJS_CNK_OBJECT draw_1352;
static NJS_CNK_OBJECT draw_1351;
static NJS_CNK_OBJECT draw_1350;
static NJS_CNK_OBJECT draw_1349;
static NJS_CNK_OBJECT draw_1348;
static NJS_CNK_OBJECT draw_1347;
static NJS_CNK_OBJECT draw_1346;
static NJS_CNK_OBJECT draw_1345;
static NJS_CNK_OBJECT draw_1344;
static NJS_CNK_OBJECT draw_1343;
static NJS_CNK_OBJECT draw_1342;
static NJS_CNK_OBJECT draw_1341;
static NJS_CNK_OBJECT draw_1339;
static NJS_CNK_OBJECT draw_1338;
static NJS_CNK_OBJECT draw_1337;
static NJS_CNK_OBJECT draw_1336;
static NJS_CNK_OBJECT draw_1335;
static NJS_CNK_OBJECT draw_1334;
static NJS_CNK_OBJECT draw_1333;
static NJS_CNK_OBJECT draw_1332;
static NJS_CNK_OBJECT draw_1331;
static NJS_CNK_OBJECT draw_1330;
static NJS_CNK_OBJECT draw_1329;
static NJS_CNK_OBJECT draw_1328;
static NJS_CNK_OBJECT draw_1327;
static NJS_CNK_OBJECT draw_1326;
static NJS_CNK_OBJECT draw_1325;
static NJS_CNK_OBJECT draw_1324;
static NJS_CNK_OBJECT draw_1323;
static NJS_CNK_OBJECT draw_1322;
static NJS_CNK_OBJECT draw_1321;
static NJS_CNK_OBJECT draw_1320;
static NJS_CNK_OBJECT draw_1319;
static NJS_CNK_OBJECT draw_1317;
static NJS_CNK_OBJECT draw_1316;
static NJS_CNK_OBJECT draw_1315;
static NJS_CNK_OBJECT draw_1314;
static NJS_CNK_OBJECT draw_1313;
static NJS_CNK_OBJECT draw_1312;
static NJS_CNK_OBJECT draw_1311;
static NJS_CNK_OBJECT draw_1310;
static NJS_CNK_OBJECT draw_1309;
static NJS_CNK_OBJECT draw_1308;
static NJS_CNK_OBJECT draw_1307;
static NJS_CNK_OBJECT draw_1306;
static NJS_CNK_OBJECT draw_1305;
static NJS_CNK_OBJECT draw_1304;
static NJS_CNK_OBJECT draw_1303;
static NJS_CNK_OBJECT draw_1302;
static NJS_CNK_OBJECT draw_1301;
static NJS_CNK_OBJECT draw_1300;
static NJS_CNK_OBJECT draw_1299;
static NJS_CNK_OBJECT draw_1298;
static NJS_CNK_OBJECT draw_1297;
static NJS_CNK_OBJECT draw_1296;
static NJS_CNK_OBJECT draw_1295;
static NJS_CNK_OBJECT draw_1294;
static NJS_CNK_OBJECT draw_1293;
static NJS_CNK_OBJECT draw_1292;
static NJS_CNK_OBJECT draw_1291;
static NJS_CNK_OBJECT draw_1290;
static NJS_CNK_OBJECT draw_1289;
static NJS_CNK_OBJECT draw_1288;
static NJS_CNK_OBJECT draw_1287;
static NJS_CNK_OBJECT draw_1286;
static NJS_CNK_OBJECT draw_1285;
static NJS_CNK_OBJECT draw_1284;
static NJS_CNK_OBJECT draw_1283;
static NJS_CNK_OBJECT draw_1282;
static NJS_CNK_OBJECT draw_1281;
static NJS_CNK_OBJECT draw_1280;
static NJS_CNK_OBJECT draw_1279;
static NJS_CNK_OBJECT draw_1278;
static NJS_CNK_OBJECT draw_1277;
static NJS_CNK_OBJECT draw_1276;
static NJS_CNK_OBJECT draw_1275;
static NJS_CNK_OBJECT draw_1274;
static NJS_CNK_OBJECT draw_1273;
static NJS_CNK_OBJECT draw_1272;
static NJS_CNK_OBJECT draw_1271;
static NJS_CNK_OBJECT draw_1270;
static NJS_CNK_OBJECT draw_1269;
static NJS_CNK_OBJECT draw_1268;
static NJS_CNK_OBJECT draw_1267;
static NJS_CNK_OBJECT draw_1266;
static NJS_CNK_OBJECT draw_1265;
static NJS_CNK_OBJECT draw_1264;
static NJS_CNK_OBJECT draw_1263;
static NJS_CNK_OBJECT draw_1262;
static NJS_CNK_OBJECT draw_1261;
static NJS_CNK_OBJECT draw_1260;
static NJS_CNK_OBJECT draw_1259;
static NJS_CNK_OBJECT draw_1258;
static NJS_CNK_OBJECT draw_1257;
static NJS_CNK_OBJECT draw_1256;
static NJS_CNK_OBJECT draw_1254;
static NJS_CNK_OBJECT draw_1253;
static NJS_CNK_OBJECT draw_1252;
static NJS_CNK_OBJECT draw_1251;
static NJS_CNK_OBJECT draw_1250;
static NJS_CNK_OBJECT draw_1249;
static NJS_CNK_OBJECT draw_1247;
static NJS_CNK_OBJECT draw_1246;
static NJS_CNK_OBJECT draw_1245;
static NJS_CNK_OBJECT draw_1244;
static NJS_CNK_OBJECT draw_1243;
static NJS_CNK_OBJECT draw_1242;
static NJS_CNK_OBJECT draw_1240;
static NJS_CNK_OBJECT draw_1239;
static NJS_CNK_OBJECT draw_1237;
static NJS_CNK_OBJECT draw_1236;
static NJS_CNK_OBJECT draw_1235;
static NJS_CNK_OBJECT draw_1234;
static NJS_CNK_OBJECT draw_1233;
static NJS_CNK_OBJECT draw_1232;
static NJS_CNK_OBJECT draw_1231;
static NJS_CNK_OBJECT draw_1230;
static NJS_CNK_OBJECT draw_1229;
static NJS_CNK_OBJECT draw_1228;
static NJS_CNK_OBJECT draw_1227;
static NJS_CNK_OBJECT draw_1226;
static NJS_CNK_OBJECT draw_1225;
static NJS_CNK_OBJECT draw_1224;
static NJS_CNK_OBJECT draw_1221;
static NJS_CNK_OBJECT draw_1220;
static NJS_CNK_OBJECT draw_1219;
static NJS_CNK_OBJECT draw_1218;
static NJS_CNK_OBJECT draw_1217;
static NJS_CNK_OBJECT draw_1215;
static NJS_CNK_OBJECT draw_1214;
static NJS_CNK_OBJECT draw_1213;
static NJS_CNK_OBJECT draw_1212;
static NJS_CNK_OBJECT draw_1211;
static NJS_CNK_OBJECT draw_1210;
static NJS_CNK_OBJECT draw_1209;
static NJS_CNK_OBJECT draw_1208;
static NJS_CNK_OBJECT draw_1203;
static NJS_CNK_OBJECT draw_1202;
static NJS_CNK_OBJECT draw_1201;
static NJS_CNK_OBJECT draw_1198;
static NJS_CNK_OBJECT draw_1197;
static NJS_CNK_OBJECT draw_1196;
static NJS_CNK_OBJECT draw_1195;
static NJS_CNK_OBJECT draw_1189;
static NJS_CNK_OBJECT draw_1188;
static NJS_CNK_OBJECT draw_1187;
static NJS_CNK_OBJECT draw_1186;
static NJS_CNK_OBJECT draw_1185;
static NJS_CNK_OBJECT draw_1184;
static NJS_CNK_OBJECT draw_1183;
static NJS_CNK_OBJECT draw_1182;
static NJS_CNK_OBJECT draw_1181;
static NJS_CNK_OBJECT draw_1180;
static NJS_CNK_OBJECT draw_1178;
static NJS_CNK_OBJECT draw_1177;
static NJS_CNK_OBJECT draw_1176;
static NJS_CNK_OBJECT draw_1175;
static NJS_CNK_OBJECT draw_1174;
static NJS_CNK_OBJECT draw_1173;
static NJS_CNK_OBJECT draw_1172;
static NJS_CNK_OBJECT draw_1170;
static NJS_CNK_OBJECT draw_1168;
static NJS_CNK_OBJECT draw_1165;
static NJS_CNK_OBJECT draw_1164;
static NJS_CNK_OBJECT draw_1163;
static NJS_CNK_OBJECT draw_1162;
static NJS_CNK_OBJECT draw_1161;
static NJS_CNK_OBJECT draw_1160;
static NJS_CNK_OBJECT draw_1158;
static NJS_CNK_OBJECT draw_1157;
static NJS_CNK_OBJECT draw_1156;
static NJS_CNK_OBJECT draw_1155;
static NJS_CNK_OBJECT draw_1154;
static NJS_CNK_OBJECT draw_1153;
static NJS_CNK_OBJECT draw_1152;
static NJS_CNK_OBJECT draw_1151;
static NJS_CNK_OBJECT draw_1150;
static NJS_CNK_OBJECT draw_1149;
static NJS_CNK_OBJECT draw_1148;
static NJS_CNK_OBJECT draw_1147;
static NJS_CNK_OBJECT draw_1146;
static NJS_CNK_OBJECT draw_1145;
static NJS_CNK_OBJECT draw_1144;
static NJS_CNK_OBJECT draw_1143;
static NJS_CNK_OBJECT draw_1142;
static NJS_CNK_OBJECT draw_1141;
static NJS_CNK_OBJECT draw_1140;
static NJS_CNK_OBJECT draw_1139;
static NJS_CNK_OBJECT draw_1138;
static NJS_CNK_OBJECT draw_1137;
static NJS_CNK_OBJECT draw_1136;
static NJS_CNK_OBJECT draw_1135;
static NJS_CNK_OBJECT draw_1134;
static NJS_CNK_OBJECT draw_1133;
static NJS_CNK_OBJECT draw_1132;
static NJS_CNK_OBJECT draw_1131;
static NJS_CNK_OBJECT draw_1130;
static NJS_CNK_OBJECT draw_1129;
static NJS_CNK_OBJECT draw_1128;
static NJS_CNK_OBJECT draw_1127;
static NJS_CNK_OBJECT draw_1126;
static NJS_CNK_OBJECT draw_1125;
static NJS_CNK_OBJECT draw_1124;
static NJS_CNK_OBJECT draw_1123;
static NJS_CNK_OBJECT draw_1122;
static NJS_CNK_OBJECT draw_1121;
static NJS_CNK_OBJECT draw_1120;
static NJS_CNK_OBJECT draw_1119;
static NJS_CNK_OBJECT draw_1118;
static NJS_CNK_OBJECT draw_1117;
static NJS_CNK_OBJECT draw_1116;
static NJS_CNK_OBJECT draw_1115;
static NJS_CNK_OBJECT draw_1114;
static NJS_CNK_OBJECT draw_1113;
static NJS_CNK_OBJECT draw_1112;
static NJS_CNK_OBJECT draw_1111;
static NJS_CNK_OBJECT draw_1110;
static NJS_CNK_OBJECT draw_1109;
static NJS_CNK_OBJECT draw_1108;
static NJS_CNK_OBJECT draw_1107;
static NJS_CNK_OBJECT draw_1106;
static NJS_CNK_OBJECT draw_1105;
static NJS_CNK_OBJECT draw_1104;
static NJS_CNK_OBJECT draw_1103;
static NJS_CNK_OBJECT draw_1102;
static NJS_CNK_OBJECT draw_1101;
static NJS_CNK_OBJECT draw_1100;
static NJS_CNK_OBJECT draw_1099;
static NJS_CNK_OBJECT draw_1098;
static NJS_CNK_OBJECT draw_1097;
static NJS_CNK_OBJECT draw_1096;
static NJS_CNK_OBJECT draw_1095;
static NJS_CNK_OBJECT draw_1094;
static NJS_CNK_OBJECT draw_1093;
static NJS_CNK_OBJECT draw_1092;
static NJS_CNK_OBJECT draw_1091;
static NJS_CNK_OBJECT draw_1090;
static NJS_CNK_OBJECT draw_1089;
static NJS_CNK_OBJECT draw_1088;
static NJS_CNK_OBJECT draw_1087;
static NJS_CNK_OBJECT draw_1086;
static NJS_CNK_OBJECT draw_1085;
static NJS_CNK_OBJECT draw_1084;
static NJS_CNK_OBJECT draw_1083;
static NJS_CNK_OBJECT draw_1082;
static NJS_CNK_OBJECT draw_1081;
static NJS_CNK_OBJECT draw_1080;
static NJS_CNK_OBJECT draw_1079;
static NJS_CNK_OBJECT draw_1078;
static NJS_CNK_OBJECT draw_1077;
static NJS_CNK_OBJECT draw_1076;
static NJS_CNK_OBJECT draw_1075;
static NJS_CNK_OBJECT draw_1074;
static NJS_CNK_OBJECT draw_1073;
static NJS_CNK_OBJECT draw_1072;
static NJS_CNK_OBJECT draw_1071;
static NJS_CNK_OBJECT draw_1070;
static NJS_CNK_OBJECT draw_1069;
static NJS_CNK_OBJECT draw_1068;
static NJS_CNK_OBJECT draw_1067;
static NJS_CNK_OBJECT draw_1066;
static NJS_CNK_OBJECT draw_1065;
static NJS_CNK_OBJECT draw_1064;
static NJS_CNK_OBJECT draw_1063;
static NJS_CNK_OBJECT draw_1062;
static NJS_CNK_OBJECT draw_1061;
static NJS_CNK_OBJECT draw_1060;
static NJS_CNK_OBJECT draw_1059;
static NJS_CNK_OBJECT draw_1058;
static NJS_CNK_OBJECT draw_1057;
static NJS_CNK_OBJECT draw_1056;
static NJS_CNK_OBJECT draw_1055;
static NJS_CNK_OBJECT draw_1054;
static NJS_CNK_OBJECT draw_1053;
static NJS_CNK_OBJECT draw_1052;
static NJS_CNK_OBJECT draw_1051;
static NJS_CNK_OBJECT draw_1050;
static NJS_CNK_OBJECT draw_1049;
static NJS_CNK_OBJECT draw_1047;
static NJS_CNK_OBJECT draw_1046;
static NJS_CNK_OBJECT draw_1045;
static NJS_CNK_OBJECT draw_1044;
static NJS_CNK_OBJECT draw_1043;
static NJS_CNK_OBJECT draw_1042;
static NJS_CNK_OBJECT draw_1041;
static NJS_CNK_OBJECT draw_1040;
static NJS_CNK_OBJECT draw_1039;
static NJS_CNK_OBJECT draw_1038;
static NJS_CNK_OBJECT draw_1037;
static NJS_CNK_OBJECT draw_1036;
static NJS_CNK_OBJECT draw_1035;
static NJS_CNK_OBJECT draw_1034;
static NJS_CNK_OBJECT draw_1033;
static NJS_CNK_OBJECT draw_1032;
static NJS_CNK_OBJECT draw_1031;
static NJS_CNK_OBJECT draw_1030;
static NJS_CNK_OBJECT draw_1029;
static NJS_CNK_OBJECT draw_1028;
static NJS_CNK_OBJECT draw_1027;
static NJS_CNK_OBJECT draw_1026;
static NJS_CNK_OBJECT draw_1025;
static NJS_CNK_OBJECT draw_1024;
static NJS_CNK_OBJECT draw_1023;
static NJS_CNK_OBJECT draw_1022;
static NJS_CNK_OBJECT draw_1021;
static NJS_CNK_OBJECT draw_1020;
static NJS_CNK_OBJECT draw_1019;
static NJS_CNK_OBJECT draw_1018;
static NJS_CNK_OBJECT draw_1017;
static NJS_CNK_OBJECT draw_1014;
static NJS_CNK_OBJECT draw_1013;
static NJS_CNK_OBJECT draw_1012;
static NJS_CNK_OBJECT draw_1011;
static NJS_CNK_OBJECT draw_1010;
static NJS_CNK_OBJECT draw_1009;
static NJS_CNK_OBJECT draw_1008;
static NJS_CNK_OBJECT draw_1007;
static NJS_CNK_OBJECT draw_1006;
static NJS_CNK_OBJECT draw_1005;
static NJS_CNK_OBJECT draw_1004;
static NJS_CNK_OBJECT draw_1002;
static NJS_CNK_OBJECT draw_1001;
static NJS_CNK_OBJECT draw_1000;
static NJS_CNK_OBJECT draw_999;
static NJS_CNK_OBJECT draw_998;
static NJS_CNK_OBJECT draw_997;
static NJS_CNK_OBJECT draw_996;
static NJS_CNK_OBJECT draw_995;
static NJS_CNK_OBJECT draw_994;
static NJS_CNK_OBJECT draw_993;
static NJS_CNK_OBJECT draw_992;
static NJS_CNK_OBJECT draw_991;
static NJS_CNK_OBJECT draw_990;
static NJS_CNK_OBJECT draw_989;
static NJS_CNK_OBJECT draw_988;
static NJS_CNK_OBJECT draw_987;
static NJS_CNK_OBJECT draw_986;
static NJS_CNK_OBJECT draw_985;
static NJS_CNK_OBJECT draw_984;
static NJS_CNK_OBJECT draw_983;
static NJS_CNK_OBJECT draw_981;
static NJS_CNK_OBJECT draw_980;
static NJS_CNK_OBJECT draw_979;
static NJS_CNK_OBJECT draw_977;
static NJS_CNK_OBJECT draw_976;
static NJS_CNK_OBJECT draw_975;
static NJS_CNK_OBJECT draw_973;
static NJS_CNK_OBJECT draw_972;
static NJS_CNK_OBJECT draw_971;
static NJS_CNK_OBJECT draw_969;
static NJS_CNK_OBJECT draw_968;
static NJS_CNK_OBJECT draw_967;
static NJS_CNK_OBJECT draw_966;
static NJS_CNK_OBJECT draw_965;
static NJS_CNK_OBJECT draw_964;
static NJS_CNK_OBJECT draw_963;
static NJS_CNK_OBJECT draw_962;
static NJS_CNK_OBJECT draw_961;
static NJS_CNK_OBJECT draw_960;
static NJS_CNK_OBJECT draw_959;
static NJS_CNK_OBJECT draw_958;
static NJS_CNK_OBJECT draw_957;
static NJS_CNK_OBJECT draw_956;
static NJS_CNK_OBJECT draw_955;
static NJS_CNK_OBJECT draw_954;
static NJS_CNK_OBJECT draw_953;
static NJS_CNK_OBJECT draw_952;
static NJS_CNK_OBJECT draw_951;
static NJS_CNK_OBJECT draw_950;
static NJS_CNK_OBJECT draw_949;
static NJS_CNK_OBJECT draw_948;
static NJS_CNK_OBJECT draw_947;
static NJS_CNK_OBJECT draw_946;
static NJS_CNK_OBJECT draw_945;
static NJS_CNK_OBJECT draw_944;
static NJS_CNK_OBJECT draw_943;
static NJS_CNK_OBJECT draw_942;
static NJS_CNK_OBJECT draw_941;
static NJS_CNK_OBJECT draw_940;
static NJS_CNK_OBJECT draw_939;
static NJS_CNK_OBJECT draw_938;
static NJS_CNK_OBJECT draw_937;
static NJS_CNK_OBJECT draw_936;
static NJS_CNK_OBJECT draw_935;
static NJS_CNK_OBJECT draw_934;
static NJS_CNK_OBJECT draw_933;
static NJS_CNK_OBJECT draw_932;
static NJS_CNK_OBJECT draw_931;
static NJS_CNK_OBJECT draw_930;
static NJS_CNK_OBJECT draw_929;
static NJS_CNK_OBJECT draw_928;
static NJS_CNK_OBJECT draw_927;
static NJS_CNK_OBJECT draw_926;
static NJS_CNK_OBJECT draw_925;
static NJS_CNK_OBJECT draw_924;
static NJS_CNK_OBJECT draw_923;
static NJS_CNK_OBJECT draw_922;
static NJS_CNK_OBJECT draw_921;
static NJS_CNK_OBJECT draw_920;
static NJS_CNK_OBJECT draw_919;
static NJS_CNK_OBJECT draw_918;
static NJS_CNK_OBJECT draw_917;
static NJS_CNK_OBJECT draw_916;
static NJS_CNK_OBJECT draw_911;
static NJS_CNK_OBJECT draw_910;
static NJS_CNK_OBJECT draw_908;
static NJS_CNK_OBJECT draw_905;
static NJS_CNK_OBJECT draw_904;
static NJS_CNK_OBJECT draw_903;
static NJS_CNK_OBJECT draw_902;
static NJS_CNK_OBJECT draw_901;
static NJS_CNK_OBJECT draw_900;
static NJS_CNK_OBJECT draw_893;
static NJS_CNK_OBJECT draw_891;
static NJS_CNK_OBJECT draw_885;
static NJS_CNK_OBJECT draw_878;
static NJS_CNK_OBJECT draw_874;
static NJS_CNK_OBJECT draw_873;
static NJS_CNK_OBJECT draw_872;
static NJS_CNK_OBJECT draw_871;
static NJS_CNK_OBJECT draw_870;
static NJS_CNK_OBJECT draw_869;
static NJS_CNK_OBJECT draw_867;
static NJS_CNK_OBJECT draw_865;
static NJS_CNK_OBJECT draw_864;
static NJS_CNK_OBJECT draw_861;
static NJS_CNK_OBJECT draw_853;
static NJS_CNK_OBJECT draw_852;
static NJS_CNK_OBJECT draw_850;
static NJS_CNK_OBJECT draw_849;
static NJS_CNK_OBJECT draw_848;
static NJS_CNK_OBJECT draw_847;
static NJS_CNK_OBJECT draw_846;
static NJS_CNK_OBJECT draw_845;
static NJS_CNK_OBJECT draw_844;
static NJS_CNK_OBJECT draw_843;
static NJS_CNK_OBJECT draw_842;
static NJS_CNK_OBJECT draw_841;
static NJS_CNK_OBJECT draw_840;
static NJS_CNK_OBJECT draw_839;
static NJS_CNK_OBJECT draw_837;
static NJS_CNK_OBJECT draw_836;
static NJS_CNK_OBJECT draw_834;
static NJS_CNK_OBJECT draw_833;
static NJS_CNK_OBJECT draw_830;
static NJS_CNK_OBJECT draw_828;
static NJS_CNK_OBJECT draw_826;
static NJS_CNK_OBJECT draw_825;
static NJS_CNK_OBJECT draw_824;
static NJS_CNK_OBJECT draw_823;
static NJS_CNK_OBJECT draw_822;
static NJS_CNK_OBJECT draw_821;
static NJS_CNK_OBJECT draw_820;
static NJS_CNK_OBJECT draw_819;
static NJS_CNK_OBJECT draw_816;
static NJS_CNK_OBJECT draw_815;
static NJS_CNK_OBJECT draw_814;
static NJS_CNK_OBJECT draw_811;
static NJS_CNK_OBJECT draw_810;
static NJS_CNK_OBJECT draw_809;
static NJS_CNK_OBJECT draw_808;
static NJS_CNK_OBJECT draw_807;
static NJS_CNK_OBJECT draw_806;
static NJS_CNK_OBJECT draw_805;
static NJS_CNK_OBJECT draw_803;
static NJS_CNK_OBJECT draw_802;
static NJS_CNK_OBJECT draw_801;
static NJS_CNK_OBJECT draw_800;
static NJS_CNK_OBJECT draw_799;
static NJS_CNK_OBJECT draw_798;
static NJS_CNK_OBJECT draw_797;
static NJS_CNK_OBJECT draw_796;
static NJS_CNK_OBJECT draw_795;
static NJS_CNK_OBJECT draw_794;
static NJS_CNK_OBJECT draw_793;
static NJS_CNK_OBJECT draw_792;
static NJS_CNK_OBJECT draw_791;
static NJS_CNK_OBJECT draw_790;
static NJS_CNK_OBJECT draw_789;
static NJS_CNK_OBJECT draw_788;
static NJS_CNK_OBJECT draw_787;
static NJS_CNK_OBJECT draw_786;
static NJS_CNK_OBJECT draw_785;
static NJS_CNK_OBJECT draw_784;
static NJS_CNK_OBJECT draw_783;
static NJS_CNK_OBJECT draw_782;
static NJS_CNK_OBJECT draw_781;
static NJS_CNK_OBJECT draw_780;
static NJS_CNK_OBJECT draw_779;
static NJS_CNK_OBJECT draw_778;
static NJS_CNK_OBJECT draw_777;
static NJS_CNK_OBJECT draw_776;
static NJS_CNK_OBJECT draw_775;
static NJS_CNK_OBJECT draw_774;
static NJS_CNK_OBJECT draw_773;
static NJS_CNK_OBJECT draw_772;
static NJS_CNK_OBJECT draw_771;
static NJS_CNK_OBJECT draw_770;
static NJS_CNK_OBJECT draw_769;
static NJS_CNK_OBJECT draw_768;
static NJS_CNK_OBJECT draw_767;
static NJS_CNK_OBJECT draw_766;
static NJS_CNK_OBJECT draw_765;
static NJS_CNK_OBJECT draw_764;
static NJS_CNK_OBJECT draw_763;
static NJS_CNK_OBJECT draw_762;
static NJS_CNK_OBJECT draw_761;
static NJS_CNK_OBJECT draw_760;
static NJS_CNK_OBJECT draw_759;
static NJS_CNK_OBJECT draw_758;
static NJS_CNK_OBJECT draw_757;
static NJS_CNK_OBJECT draw_756;
static NJS_CNK_OBJECT draw_755;
static NJS_CNK_OBJECT draw_754;
static NJS_CNK_OBJECT draw_753;
static NJS_CNK_OBJECT draw_752;
static NJS_CNK_OBJECT draw_751;
static NJS_CNK_OBJECT draw_750;
static NJS_CNK_OBJECT draw_749;
static NJS_CNK_OBJECT draw_748;
static NJS_CNK_OBJECT draw_747;
static NJS_CNK_OBJECT draw_746;
static NJS_CNK_OBJECT draw_745;
static NJS_CNK_OBJECT draw_744;
static NJS_CNK_OBJECT draw_743;
static NJS_CNK_OBJECT draw_742;
static NJS_CNK_OBJECT draw_741;
static NJS_CNK_OBJECT draw_740;
static NJS_CNK_OBJECT draw_739;
static NJS_CNK_OBJECT draw_738;
static NJS_CNK_OBJECT draw_737;
static NJS_CNK_OBJECT draw_736;
static NJS_CNK_OBJECT draw_735;
static NJS_CNK_OBJECT draw_734;
static NJS_CNK_OBJECT draw_733;
static NJS_CNK_OBJECT draw_732;
static NJS_CNK_OBJECT draw_731;
static NJS_CNK_OBJECT draw_730;
static NJS_CNK_OBJECT draw_729;
static NJS_CNK_OBJECT draw_728;
static NJS_CNK_OBJECT draw_727;
static NJS_CNK_OBJECT draw_726;
static NJS_CNK_OBJECT draw_725;
static NJS_CNK_OBJECT draw_724;
static NJS_CNK_OBJECT draw_723;
static NJS_CNK_OBJECT draw_722;
static NJS_CNK_OBJECT draw_721;
static NJS_CNK_OBJECT draw_720;
static NJS_CNK_OBJECT draw_719;
static NJS_CNK_OBJECT draw_718;
static NJS_CNK_OBJECT draw_717;
static NJS_CNK_OBJECT draw_716;
static NJS_CNK_OBJECT draw_715;
static NJS_CNK_OBJECT draw_714;
static NJS_CNK_OBJECT draw_713;
static NJS_CNK_OBJECT draw_712;
static NJS_CNK_OBJECT draw_711;
static NJS_CNK_OBJECT draw_710;
static NJS_CNK_OBJECT draw_709;
static NJS_CNK_OBJECT draw_708;
static NJS_CNK_OBJECT draw_707;
static NJS_CNK_OBJECT draw_706;
static NJS_CNK_OBJECT draw_705;
static NJS_CNK_OBJECT draw_704;
static NJS_CNK_OBJECT draw_703;
static NJS_CNK_OBJECT draw_702;
static NJS_CNK_OBJECT draw_701;
static NJS_CNK_OBJECT draw_700;
static NJS_CNK_OBJECT draw_699;
static NJS_CNK_OBJECT draw_698;
static NJS_CNK_OBJECT draw_697;
static NJS_CNK_OBJECT draw_696;
static NJS_CNK_OBJECT draw_695;
static NJS_CNK_OBJECT draw_694;
static NJS_CNK_OBJECT draw_693;
static NJS_CNK_OBJECT draw_692;
static NJS_CNK_OBJECT draw_691;
static NJS_CNK_OBJECT draw_690;
static NJS_CNK_OBJECT draw_689;
static NJS_CNK_OBJECT draw_688;
static NJS_CNK_OBJECT draw_687;
static NJS_CNK_OBJECT draw_686;
static NJS_CNK_OBJECT draw_685;
static NJS_CNK_OBJECT draw_684;
static NJS_CNK_OBJECT draw_683;
static NJS_CNK_OBJECT draw_682;
static NJS_CNK_OBJECT draw_681;
static NJS_CNK_OBJECT draw_680;
static NJS_CNK_OBJECT draw_679;
static NJS_CNK_OBJECT draw_678;
static NJS_CNK_OBJECT draw_677;
static NJS_CNK_OBJECT draw_676;
static NJS_CNK_OBJECT draw_675;
static NJS_CNK_OBJECT draw_674;
static NJS_CNK_OBJECT draw_673;
static NJS_CNK_OBJECT draw_672;
static NJS_CNK_OBJECT draw_671;
static NJS_CNK_OBJECT draw_670;
static NJS_CNK_OBJECT draw_669;
static NJS_CNK_OBJECT draw_668;
static NJS_CNK_OBJECT draw_667;
static NJS_CNK_OBJECT draw_666;
static NJS_CNK_OBJECT draw_665;
static NJS_CNK_OBJECT draw_664;
static NJS_CNK_OBJECT draw_663;
static NJS_CNK_OBJECT draw_662;
static NJS_CNK_OBJECT draw_661;
static NJS_CNK_OBJECT draw_660;
static NJS_CNK_OBJECT draw_659;
static NJS_CNK_OBJECT draw_658;
static NJS_CNK_OBJECT draw_657;
static NJS_CNK_OBJECT draw_656;
static NJS_CNK_OBJECT draw_655;
static NJS_CNK_OBJECT draw_654;
static NJS_CNK_OBJECT draw_652;
static NJS_CNK_OBJECT draw_651;
static NJS_CNK_OBJECT draw_648;
static NJS_CNK_OBJECT draw_647;
static NJS_CNK_OBJECT draw_646;
static NJS_CNK_OBJECT draw_645;
static NJS_CNK_OBJECT draw_644;
static NJS_CNK_OBJECT draw_643;
static NJS_CNK_OBJECT draw_642;
static NJS_CNK_OBJECT draw_641;
static NJS_CNK_OBJECT draw_640;
static NJS_CNK_OBJECT draw_639;
static NJS_CNK_OBJECT draw_638;
static NJS_CNK_OBJECT draw_637;
static NJS_CNK_OBJECT draw_636;
static NJS_CNK_OBJECT draw_635;
static NJS_CNK_OBJECT draw_634;
static NJS_CNK_OBJECT draw_633;
static NJS_CNK_OBJECT draw_632;
static NJS_CNK_OBJECT draw_631;
static NJS_CNK_OBJECT draw_630;
static NJS_CNK_OBJECT draw_629;
static NJS_CNK_OBJECT draw_628;
static NJS_CNK_OBJECT draw_627;
static NJS_CNK_OBJECT draw_626;
static NJS_CNK_OBJECT draw_625;
static NJS_CNK_OBJECT draw_624;
static NJS_CNK_OBJECT draw_623;
static NJS_CNK_OBJECT draw_622;
static NJS_CNK_OBJECT draw_621;
static NJS_CNK_OBJECT draw_620;
static NJS_CNK_OBJECT draw_619;
static NJS_CNK_OBJECT draw_618;
static NJS_CNK_OBJECT draw_617;
static NJS_CNK_OBJECT draw_616;
static NJS_CNK_OBJECT draw_615;
static NJS_CNK_OBJECT draw_614;
static NJS_CNK_OBJECT draw_613;
static NJS_CNK_OBJECT draw_612;
static NJS_CNK_OBJECT draw_611;
static NJS_CNK_OBJECT draw_610;
static NJS_CNK_OBJECT draw_609;
static NJS_CNK_OBJECT draw_608;
static NJS_CNK_OBJECT draw_607;
static NJS_CNK_OBJECT draw_606;
static NJS_CNK_OBJECT draw_605;
static NJS_CNK_OBJECT draw_604;
static NJS_CNK_OBJECT draw_603;
static NJS_CNK_OBJECT draw_602;
static NJS_CNK_OBJECT draw_601;
static NJS_CNK_OBJECT draw_600;
static NJS_CNK_OBJECT draw_599;
static NJS_CNK_OBJECT draw_598;
static NJS_CNK_OBJECT draw_597;
static NJS_CNK_OBJECT draw_596;
static NJS_CNK_OBJECT draw_595;
static NJS_CNK_OBJECT draw_594;
static NJS_CNK_OBJECT draw_593;
static NJS_CNK_OBJECT draw_592;
static NJS_CNK_OBJECT draw_591;
static NJS_CNK_OBJECT draw_590;
static NJS_CNK_OBJECT draw_589;
static NJS_CNK_OBJECT draw_588;
static NJS_CNK_OBJECT draw_587;
static NJS_CNK_OBJECT draw_586;
static NJS_CNK_OBJECT draw_585;
static NJS_CNK_OBJECT draw_584;
static NJS_CNK_OBJECT draw_583;
static NJS_CNK_OBJECT draw_582;
static NJS_CNK_OBJECT draw_581;
static NJS_CNK_OBJECT draw_580;
static NJS_CNK_OBJECT draw_579;
static NJS_CNK_OBJECT draw_578;
static NJS_CNK_OBJECT draw_577;
static NJS_CNK_OBJECT draw_576;
static NJS_CNK_OBJECT draw_575;
static NJS_CNK_OBJECT draw_574;
static NJS_CNK_OBJECT draw_573;
static NJS_CNK_OBJECT draw_572;
static NJS_CNK_OBJECT draw_571;
static NJS_CNK_OBJECT draw_570;
static NJS_CNK_OBJECT draw_569;
static NJS_CNK_OBJECT draw_568;
static NJS_CNK_OBJECT draw_567;
static NJS_CNK_OBJECT draw_566;
static NJS_CNK_OBJECT draw_565;
static NJS_CNK_OBJECT draw_564;
static NJS_CNK_OBJECT draw_563;
static NJS_CNK_OBJECT draw_562;
static NJS_CNK_OBJECT draw_561;
static NJS_CNK_OBJECT draw_560;
static NJS_CNK_OBJECT draw_559;
static NJS_CNK_OBJECT draw_558;
static NJS_CNK_OBJECT draw_557;
static NJS_CNK_OBJECT draw_556;
static NJS_CNK_OBJECT draw_555;
static NJS_CNK_OBJECT draw_554;
static NJS_CNK_OBJECT draw_553;
static NJS_CNK_OBJECT draw_552;
static NJS_CNK_OBJECT draw_551;
static NJS_CNK_OBJECT draw_550;
static NJS_CNK_OBJECT draw_549;
static NJS_CNK_OBJECT draw_548;
static NJS_CNK_OBJECT draw_547;
static NJS_CNK_OBJECT draw_546;
static NJS_CNK_OBJECT draw_545;
static NJS_CNK_OBJECT draw_544;
static NJS_CNK_OBJECT draw_543;
static NJS_CNK_OBJECT draw_542;
static NJS_CNK_OBJECT draw_541;
static NJS_CNK_OBJECT draw_540;
static NJS_CNK_OBJECT draw_539;
static NJS_CNK_OBJECT draw_538;
static NJS_CNK_OBJECT draw_537;
static NJS_CNK_OBJECT draw_536;
static NJS_CNK_OBJECT draw_535;
static NJS_CNK_OBJECT draw_534;
static NJS_CNK_OBJECT draw_533;
static NJS_CNK_OBJECT draw_532;
static NJS_CNK_OBJECT draw_531;
static NJS_CNK_OBJECT draw_530;
static NJS_CNK_OBJECT draw_529;
static NJS_CNK_OBJECT draw_528;
static NJS_CNK_OBJECT draw_527;
static NJS_CNK_OBJECT draw_526;
static NJS_CNK_OBJECT draw_525;
static NJS_CNK_OBJECT draw_524;
static NJS_CNK_OBJECT draw_523;
static NJS_CNK_OBJECT draw_522;
static NJS_CNK_OBJECT draw_521;
static NJS_CNK_OBJECT draw_520;
static NJS_CNK_OBJECT draw_519;
static NJS_CNK_OBJECT draw_518;
static NJS_CNK_OBJECT draw_517;
static NJS_CNK_OBJECT draw_516;
static NJS_CNK_OBJECT draw_515;
static NJS_CNK_OBJECT draw_514;
static NJS_CNK_OBJECT draw_513;
static NJS_CNK_OBJECT draw_512;
static NJS_CNK_OBJECT draw_511;
static NJS_CNK_OBJECT draw_510;
static NJS_CNK_OBJECT draw_509;
static NJS_CNK_OBJECT draw_508;
static NJS_CNK_OBJECT draw_507;
static NJS_CNK_OBJECT draw_506;
static NJS_CNK_OBJECT draw_505;
static NJS_CNK_OBJECT draw_504;
static NJS_CNK_OBJECT draw_503;
static NJS_CNK_OBJECT draw_502;
static NJS_CNK_OBJECT draw_501;
static NJS_CNK_OBJECT draw_500;
static NJS_CNK_OBJECT draw_499;
static NJS_CNK_OBJECT draw_498;
static NJS_CNK_OBJECT draw_497;
static NJS_CNK_OBJECT draw_496;
static NJS_CNK_OBJECT draw_495;
static NJS_CNK_OBJECT draw_494;
static NJS_CNK_OBJECT draw_493;
static NJS_CNK_OBJECT draw_492;
static NJS_CNK_OBJECT draw_491;
static NJS_CNK_OBJECT draw_490;
static NJS_CNK_OBJECT draw_489;
static NJS_CNK_OBJECT draw_488;
static NJS_CNK_OBJECT draw_487;
static NJS_CNK_OBJECT draw_486;
static NJS_CNK_OBJECT draw_485;
static NJS_CNK_OBJECT draw_484;
static NJS_CNK_OBJECT draw_483;
static NJS_CNK_OBJECT draw_482;
static NJS_CNK_OBJECT draw_481;
static NJS_CNK_OBJECT draw_480;
static NJS_CNK_OBJECT draw_479;
static NJS_CNK_OBJECT draw_478;
static NJS_CNK_OBJECT draw_477;
static NJS_CNK_OBJECT draw_476;
static NJS_CNK_OBJECT draw_475;
static NJS_CNK_OBJECT draw_474;
static NJS_CNK_OBJECT draw_473;
static NJS_CNK_OBJECT draw_472;
static NJS_CNK_OBJECT draw_471;
static NJS_CNK_OBJECT draw_470;
static NJS_CNK_OBJECT draw_469;
static NJS_CNK_OBJECT draw_468;
static NJS_CNK_OBJECT draw_467;
static NJS_CNK_OBJECT draw_466;
static NJS_CNK_OBJECT draw_465;
static NJS_CNK_OBJECT draw_464;
static NJS_CNK_OBJECT draw_463;
static NJS_CNK_OBJECT draw_462;
static NJS_CNK_OBJECT draw_461;
static NJS_CNK_OBJECT draw_460;
static NJS_CNK_OBJECT draw_459;
static NJS_CNK_OBJECT draw_458;
static NJS_CNK_OBJECT draw_457;
static NJS_CNK_OBJECT draw_456;
static NJS_CNK_OBJECT draw_455;
static NJS_CNK_OBJECT draw_454;
static NJS_CNK_OBJECT draw_453;
static NJS_CNK_OBJECT draw_452;
static NJS_CNK_OBJECT draw_451;
static NJS_CNK_OBJECT draw_450;
static NJS_CNK_OBJECT draw_449;
static NJS_CNK_OBJECT draw_448;
static NJS_CNK_OBJECT draw_447;
static NJS_CNK_OBJECT draw_446;
static NJS_CNK_OBJECT draw_445;
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
static NJS_CNK_OBJECT draw_419;
static NJS_CNK_OBJECT draw_418;
static NJS_CNK_OBJECT draw_417;
static NJS_CNK_OBJECT draw_416;
static NJS_CNK_OBJECT draw_415;
static NJS_CNK_OBJECT draw_414;
static NJS_CNK_OBJECT draw_413;
static NJS_CNK_OBJECT draw_412;
static NJS_CNK_OBJECT draw_411;
static NJS_CNK_OBJECT draw_410;
static NJS_CNK_OBJECT draw_409;
static NJS_CNK_OBJECT draw_408;
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
static NJS_CNK_OBJECT draw_375;
static NJS_CNK_OBJECT draw_374;
static NJS_CNK_OBJECT draw_373;
static NJS_CNK_OBJECT draw_372;
static NJS_CNK_OBJECT draw_371;
static NJS_CNK_OBJECT draw_370;
static NJS_CNK_OBJECT draw_369;
static NJS_CNK_OBJECT draw_368;
static NJS_CNK_OBJECT draw_367;
static NJS_CNK_OBJECT draw_366;
static NJS_CNK_OBJECT draw_365;
static NJS_CNK_OBJECT draw_364;
static NJS_CNK_OBJECT draw_363;
static NJS_CNK_OBJECT draw_362;
static NJS_CNK_OBJECT draw_361;
static NJS_CNK_OBJECT draw_360;
static NJS_CNK_OBJECT draw_359;
static NJS_CNK_OBJECT draw_358;
static NJS_CNK_OBJECT draw_357;
static NJS_CNK_OBJECT draw_356;
static NJS_CNK_OBJECT draw_355;
static NJS_CNK_OBJECT draw_354;
static NJS_CNK_OBJECT draw_353;
static NJS_CNK_OBJECT draw_352;
static NJS_CNK_OBJECT draw_351;
static NJS_CNK_OBJECT draw_350;
static NJS_CNK_OBJECT draw_349;
static NJS_CNK_OBJECT draw_348;
static NJS_CNK_OBJECT draw_347;
static NJS_CNK_OBJECT draw_346;
static NJS_CNK_OBJECT draw_345;
static NJS_CNK_OBJECT draw_344;
static NJS_CNK_OBJECT draw_343;
static NJS_CNK_OBJECT draw_342;
static NJS_CNK_OBJECT draw_341;
static NJS_CNK_OBJECT draw_340;
static NJS_CNK_OBJECT draw_339;
static NJS_CNK_OBJECT draw_338;
static NJS_CNK_OBJECT draw_337;
static NJS_CNK_OBJECT draw_336;
static NJS_CNK_OBJECT draw_335;
static NJS_CNK_OBJECT draw_334;
static NJS_CNK_OBJECT draw_333;
static NJS_CNK_OBJECT draw_332;
static NJS_CNK_OBJECT draw_331;
static NJS_CNK_OBJECT draw_330;
static NJS_CNK_OBJECT draw_329;
static NJS_CNK_OBJECT draw_328;
static NJS_CNK_OBJECT draw_327;
static NJS_CNK_OBJECT draw_326;
static NJS_CNK_OBJECT draw_325;
static NJS_CNK_OBJECT draw_324;
static NJS_CNK_OBJECT draw_323;
static NJS_CNK_OBJECT draw_322;
static NJS_CNK_OBJECT draw_321;
static NJS_CNK_OBJECT draw_320;
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
static NJS_CNK_OBJECT draw_301;
static NJS_CNK_OBJECT draw_300;
static NJS_CNK_OBJECT draw_298;
static NJS_CNK_OBJECT draw_297;
static NJS_CNK_OBJECT draw_296;
static NJS_CNK_OBJECT draw_295;
static NJS_CNK_OBJECT draw_294;
static NJS_CNK_OBJECT draw_293;
static NJS_CNK_OBJECT draw_292;
static NJS_CNK_OBJECT draw_291;
static NJS_CNK_OBJECT draw_290;
static NJS_CNK_OBJECT draw_289;
static NJS_CNK_OBJECT draw_288;
static NJS_CNK_OBJECT draw_287;
static NJS_CNK_OBJECT draw_286;
static NJS_CNK_OBJECT draw_285;
static NJS_CNK_OBJECT draw_284;
static NJS_CNK_OBJECT draw_283;
static NJS_CNK_OBJECT draw_282;
static NJS_CNK_OBJECT draw_281;
static NJS_CNK_OBJECT draw_280;
static NJS_CNK_OBJECT draw_279;
static NJS_CNK_OBJECT draw_278;
static NJS_CNK_OBJECT draw_277;
static NJS_CNK_OBJECT draw_276;
static NJS_CNK_OBJECT draw_275;
static NJS_CNK_OBJECT draw_274;
static NJS_CNK_OBJECT draw_273;
static NJS_CNK_OBJECT draw_272;
static NJS_CNK_OBJECT draw_271;
static NJS_CNK_OBJECT draw_270;
static NJS_CNK_OBJECT draw_269;
static NJS_CNK_OBJECT draw_268;
static NJS_CNK_OBJECT draw_267;
static NJS_CNK_OBJECT draw_266;
static NJS_CNK_OBJECT draw_265;
static NJS_CNK_OBJECT draw_264;
static NJS_CNK_OBJECT draw_263;
static NJS_CNK_OBJECT draw_262;
static NJS_CNK_OBJECT draw_261;
static NJS_CNK_OBJECT draw_260;
static NJS_CNK_OBJECT draw_259;
static NJS_CNK_OBJECT draw_258;
static NJS_CNK_OBJECT draw_257;
static NJS_CNK_OBJECT draw_256;
static NJS_CNK_OBJECT draw_255;
static NJS_CNK_OBJECT draw_254;
static NJS_CNK_OBJECT draw_253;
static NJS_CNK_OBJECT draw_252;
static NJS_CNK_OBJECT draw_251;
static NJS_CNK_OBJECT draw_250;
static NJS_CNK_OBJECT draw_249;
static NJS_CNK_OBJECT draw_248;
static NJS_CNK_OBJECT draw_247;
static NJS_CNK_OBJECT draw_246;
static NJS_CNK_OBJECT draw_245;
static NJS_CNK_OBJECT draw_244;
static NJS_CNK_OBJECT draw_243;
static NJS_CNK_OBJECT draw_242;
static NJS_CNK_OBJECT draw_241;
static NJS_CNK_OBJECT draw_240;
static NJS_CNK_OBJECT draw_239;
static NJS_CNK_OBJECT draw_238;
static NJS_CNK_OBJECT draw_237;
static NJS_CNK_OBJECT draw_236;
static NJS_CNK_OBJECT draw_235;
static NJS_CNK_OBJECT draw_234;
static NJS_CNK_OBJECT draw_233;
static NJS_CNK_OBJECT draw_232;
static NJS_CNK_OBJECT draw_231;
static NJS_CNK_OBJECT draw_230;
static NJS_CNK_OBJECT draw_229;
static NJS_CNK_OBJECT draw_228;
static NJS_CNK_OBJECT draw_227;
static NJS_CNK_OBJECT draw_226;
static NJS_CNK_OBJECT draw_224;
static NJS_CNK_OBJECT draw_223;
static NJS_CNK_OBJECT draw_222;
static NJS_CNK_OBJECT draw_221;
static NJS_CNK_OBJECT draw_220;
static NJS_CNK_OBJECT draw_219;
static NJS_CNK_OBJECT draw_218;
static NJS_CNK_OBJECT draw_216;
static NJS_CNK_OBJECT draw_215;
static NJS_CNK_OBJECT draw_214;
static NJS_CNK_OBJECT draw_213;
static NJS_CNK_OBJECT draw_212;
static NJS_CNK_OBJECT draw_211;
static NJS_CNK_OBJECT draw_210;
static NJS_CNK_OBJECT draw_209;
static NJS_CNK_OBJECT draw_208;
static NJS_CNK_OBJECT draw_207;
static NJS_CNK_OBJECT draw_206;
static NJS_CNK_OBJECT draw_205;
static NJS_CNK_OBJECT draw_204;
static NJS_CNK_OBJECT draw_203;
static NJS_CNK_OBJECT draw_202;
static NJS_CNK_OBJECT draw_201;
static NJS_CNK_OBJECT draw_200;
static NJS_CNK_OBJECT draw_199;
static NJS_CNK_OBJECT draw_198;
static NJS_CNK_OBJECT draw_197;
static NJS_CNK_OBJECT draw_196;
static NJS_CNK_OBJECT draw_194;
static NJS_CNK_OBJECT draw_193;
static NJS_CNK_OBJECT draw_192;
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
static NJS_CNK_OBJECT draw_178;
static NJS_CNK_OBJECT draw_177;
static NJS_CNK_OBJECT draw_176;
static NJS_CNK_OBJECT draw_175;
static NJS_CNK_OBJECT draw_174;
static NJS_CNK_OBJECT draw_173;
static NJS_CNK_OBJECT draw_172;
static NJS_CNK_OBJECT draw_171;
static NJS_CNK_OBJECT draw_170;
static NJS_CNK_OBJECT draw_169;
static NJS_CNK_OBJECT draw_168;
static NJS_CNK_OBJECT draw_167;
static NJS_CNK_OBJECT draw_166;
static NJS_CNK_OBJECT draw_165;
static NJS_CNK_OBJECT draw_164;
static NJS_CNK_OBJECT draw_163;
static NJS_CNK_OBJECT draw_162;
static NJS_CNK_OBJECT draw_161;
static NJS_CNK_OBJECT draw_160;
static NJS_CNK_OBJECT draw_159;
static NJS_CNK_OBJECT draw_158;
static NJS_CNK_OBJECT draw_157;
static NJS_CNK_OBJECT draw_156;
static NJS_CNK_OBJECT draw_155;
static NJS_CNK_OBJECT draw_154;
static NJS_CNK_OBJECT draw_153;
static NJS_CNK_OBJECT draw_152;
static NJS_CNK_OBJECT draw_151;
static NJS_CNK_OBJECT draw_150;
static NJS_CNK_OBJECT draw_149;
static NJS_CNK_OBJECT draw_148;
static NJS_CNK_OBJECT draw_147;
static NJS_CNK_OBJECT draw_146;
static NJS_CNK_OBJECT draw_145;
static NJS_CNK_OBJECT draw_144;
static NJS_CNK_OBJECT draw_143;
static NJS_CNK_OBJECT draw_142;
static NJS_CNK_OBJECT draw_141;
static NJS_CNK_OBJECT draw_140;
static NJS_CNK_OBJECT draw_139;
static NJS_CNK_OBJECT draw_138;
static NJS_CNK_OBJECT draw_137;
static NJS_CNK_OBJECT draw_136;
static NJS_CNK_OBJECT draw_135;
static NJS_CNK_OBJECT draw_134;
static NJS_CNK_OBJECT draw_132;
static NJS_CNK_OBJECT draw_131;
static NJS_CNK_OBJECT draw_130;
static NJS_CNK_OBJECT draw_129;
static NJS_CNK_OBJECT draw_128;
static NJS_CNK_OBJECT draw_127;
static NJS_CNK_OBJECT draw_126;
static NJS_CNK_OBJECT draw_125;
static NJS_CNK_OBJECT draw_124;
static NJS_CNK_OBJECT draw_123;
static NJS_CNK_OBJECT draw_122;
static NJS_CNK_OBJECT draw_121;
static NJS_CNK_OBJECT draw_120;
static NJS_CNK_OBJECT draw_119;
static NJS_CNK_OBJECT draw_118;
static NJS_CNK_OBJECT draw_117;
static NJS_CNK_OBJECT draw_116;
static NJS_CNK_OBJECT draw_115;
static NJS_CNK_OBJECT draw_114;
static NJS_CNK_OBJECT draw_113;
static NJS_CNK_OBJECT draw_112;
static NJS_CNK_OBJECT draw_111;
static NJS_CNK_OBJECT draw_110;
static NJS_CNK_OBJECT draw_109;
static NJS_CNK_OBJECT draw_108;
static NJS_CNK_OBJECT draw_107;
static NJS_CNK_OBJECT draw_106;
static NJS_CNK_OBJECT draw_105;
static NJS_CNK_OBJECT draw_104;
static NJS_CNK_OBJECT draw_103;
static NJS_CNK_OBJECT draw_102;
static NJS_CNK_OBJECT draw_101;
static NJS_CNK_OBJECT draw_100;
static NJS_CNK_OBJECT draw_99;
static NJS_CNK_OBJECT draw_98;
static NJS_CNK_OBJECT draw_97;
static NJS_CNK_OBJECT draw_96;
static NJS_CNK_OBJECT draw_95;
static NJS_CNK_OBJECT draw_94;
static NJS_CNK_OBJECT draw_93;
static NJS_CNK_OBJECT draw_92;
static NJS_CNK_OBJECT draw_91;
static NJS_CNK_OBJECT draw_90;
static NJS_CNK_OBJECT draw_89;
static NJS_CNK_OBJECT draw_88;
static NJS_CNK_OBJECT draw_87;
static NJS_CNK_OBJECT draw_86;
static NJS_CNK_OBJECT draw_85;
static NJS_CNK_OBJECT draw_84;
static NJS_CNK_OBJECT draw_83;
static NJS_CNK_OBJECT draw_82;
static NJS_CNK_OBJECT draw_81;
static NJS_CNK_OBJECT draw_79;
static NJS_CNK_OBJECT draw_78;
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
static NJS_CNK_OBJECT draw_63;
static NJS_CNK_OBJECT draw_61;
static NJS_CNK_OBJECT draw_60;
static NJS_CNK_OBJECT draw_58;
static NJS_CNK_OBJECT draw_56;
static NJS_CNK_OBJECT draw_55;
static NJS_CNK_OBJECT draw_54;
static NJS_CNK_OBJECT draw_53;
static NJS_CNK_OBJECT draw_52;
static NJS_CNK_OBJECT draw_51;
static NJS_CNK_OBJECT draw_50;
static NJS_CNK_OBJECT draw_49;
static NJS_CNK_OBJECT draw_48;
static NJS_CNK_OBJECT draw_47;
static NJS_CNK_OBJECT draw_46;
static NJS_CNK_OBJECT draw_45;
static NJS_CNK_OBJECT draw_44;
static NJS_CNK_OBJECT draw_43;
static NJS_CNK_OBJECT draw_42;
static NJS_CNK_OBJECT draw_41;
static NJS_CNK_OBJECT draw_40;
static NJS_CNK_OBJECT draw_39;
static NJS_CNK_OBJECT draw_36;
static NJS_CNK_OBJECT draw_35;
static NJS_CNK_OBJECT draw_32;
static NJS_CNK_OBJECT draw_31;
static NJS_CNK_OBJECT draw_30;
static NJS_CNK_OBJECT draw_29;
static NJS_CNK_OBJECT draw_28;
static NJS_CNK_OBJECT draw_27;
static NJS_CNK_OBJECT draw_26;
static NJS_CNK_OBJECT draw_25;
static NJS_CNK_OBJECT draw_24;
static NJS_CNK_OBJECT draw_23;
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
static NJS_CNK_OBJECT draw_9;
static NJS_CNK_OBJECT draw_8;
static NJS_CNK_OBJECT draw_7;
static NJS_CNK_OBJECT draw_6;
static NJS_CNK_OBJECT draw_5;
static NJS_CNK_OBJECT draw_4;
static NJS_CNK_OBJECT draw_3;
static NJS_CNK_OBJECT draw_0;

static NJS_MKEY_A upgrade_city_escape_motion_joint_4_angles[16] = {
#include "stg13/terrain/upgrade_city_escape_motion_joint_4_angles.inc"
};
static NJS_MKEY_A upgrade_city_escape_motion_joint_7_angles[16] = {
#include "stg13/terrain/upgrade_city_escape_motion_joint_7_angles.inc"
};
static NJS_MKEY_A upgrade_city_escape_motion_joint_8_angles[14] = {
#include "stg13/terrain/upgrade_city_escape_motion_joint_8_angles.inc"
};
static NJS_MKEY_A upgrade_city_escape_motion_joint_9_angles[16] = {
#include "stg13/terrain/upgrade_city_escape_motion_joint_9_angles.inc"
};
static NJS_MKEY_A upgrade_city_escape_motion_joint_13_angles[16] = {
#include "stg13/terrain/upgrade_city_escape_motion_joint_13_angles.inc"
};
static NJS_MKEY_A upgrade_city_escape_motion_joint_14_angles[2] = {
#include "stg13/terrain/upgrade_city_escape_motion_joint_14_angles.inc"
};
static NJS_MKEY_A upgrade_city_escape_motion_joint_17_angles[16] = {
#include "stg13/terrain/upgrade_city_escape_motion_joint_17_angles.inc"
};
static NJS_MKEY_A upgrade_city_escape_motion_joint_18_angles[2] = {
#include "stg13/terrain/upgrade_city_escape_motion_joint_18_angles.inc"
};
static NJS_MKEY_A upgrade_city_escape_motion_joint_19_angles[8] = {
#include "stg13/terrain/upgrade_city_escape_motion_joint_19_angles.inc"
};
static NJS_MKEY_A upgrade_city_escape_motion_joint_23_angles[16] = {
#include "stg13/terrain/upgrade_city_escape_motion_joint_23_angles.inc"
};
static NJS_MKEY_A upgrade_city_escape_motion_joint_24_angles[2] = {
#include "stg13/terrain/upgrade_city_escape_motion_joint_24_angles.inc"
};
static NJS_MKEY_A upgrade_city_escape_motion_joint_27_angles[16] = {
#include "stg13/terrain/upgrade_city_escape_motion_joint_27_angles.inc"
};
static NJS_MKEY_A upgrade_city_escape_motion_joint_31_angles[2] = {
#include "stg13/terrain/upgrade_city_escape_motion_joint_31_angles.inc"
};
static NJS_MKEY_A upgrade_city_escape_motion_joint_32_angles[16] = {
#include "stg13/terrain/upgrade_city_escape_motion_joint_32_angles.inc"
};
static NJS_MKEY_A upgrade_city_escape_motion_joint_33_angles[12] = {
#include "stg13/terrain/upgrade_city_escape_motion_joint_33_angles.inc"
};
static NJS_MKEY_A upgrade_city_escape_motion_joint_37_angles[16] = {
#include "stg13/terrain/upgrade_city_escape_motion_joint_37_angles.inc"
};
static NJS_MKEY_A upgrade_city_escape_motion_joint_38_angles[16] = {
#include "stg13/terrain/upgrade_city_escape_motion_joint_38_angles.inc"
};
static NJS_MKEY_A upgrade_city_escape_motion_joint_39_angles[16] = {
#include "stg13/terrain/upgrade_city_escape_motion_joint_39_angles.inc"
};
static NJS_MKEY_A upgrade_city_escape_motion_joint_41_angles[16] = {
#include "stg13/terrain/upgrade_city_escape_motion_joint_41_angles.inc"
};
static NJS_MKEY_A upgrade_city_escape_motion_joint_44_angles[16] = {
#include "stg13/terrain/upgrade_city_escape_motion_joint_44_angles.inc"
};
static NJS_MKEY_A upgrade_city_escape_motion_joint_51_angles[16] = {
#include "stg13/terrain/upgrade_city_escape_motion_joint_51_angles.inc"
};
static NJS_MKEY_A upgrade_city_escape_motion_joint_53_angles[16] = {
#include "stg13/terrain/upgrade_city_escape_motion_joint_53_angles.inc"
};
static NJS_MKEY_A upgrade_city_escape_motion_joint_56_angles[16] = {
#include "stg13/terrain/upgrade_city_escape_motion_joint_56_angles.inc"
};
static NJS_MKEY_A upgrade_city_escape_motion_joint_57_angles[16] = {
#include "stg13/terrain/upgrade_city_escape_motion_joint_57_angles.inc"
};
static NJS_MKEY_A upgrade_city_escape_motion_joint_58_angles[16] = {
#include "stg13/terrain/upgrade_city_escape_motion_joint_58_angles.inc"
};
static NJS_MKEY_A upgrade_city_escape_motion_joint_60_angles[16] = {
#include "stg13/terrain/upgrade_city_escape_motion_joint_60_angles.inc"
};
static NJS_MDATA2 upgrade_city_escape_motion_tracks[62] = {
#include "stg13/terrain/upgrade_city_escape_motion_tracks.inc"
};
static NJS_MOTION upgrade_city_escape_motion = {
#include "stg13/terrain/upgrade_city_escape_motion.inc"
};
static NJS_MKEY_F bar_throw_back_motion_joint_0_position[2] = {
#include "stg13/terrain/bar_throw_back_motion_joint_0_position.inc"
};
static NJS_MKEY_F bar_throw_back_motion_joint_2_position[2] = {
#include "stg13/terrain/bar_throw_back_motion_joint_2_position.inc"
};
static NJS_MKEY_A bar_throw_back_motion_joint_4_angles[2] = {
#include "stg13/terrain/bar_throw_back_motion_joint_4_angles.inc"
};
static NJS_MKEY_A bar_throw_back_motion_joint_7_angles[32] = {
#include "stg13/terrain/bar_throw_back_motion_joint_7_angles.inc"
};
static NJS_MKEY_A bar_throw_back_motion_joint_8_angles[32] = {
#include "stg13/terrain/bar_throw_back_motion_joint_8_angles.inc"
};
static NJS_MKEY_A bar_throw_back_motion_joint_9_angles[32] = {
#include "stg13/terrain/bar_throw_back_motion_joint_9_angles.inc"
};
static NJS_MKEY_A bar_throw_back_motion_joint_13_angles[32] = {
#include "stg13/terrain/bar_throw_back_motion_joint_13_angles.inc"
};
static NJS_MKEY_A bar_throw_back_motion_joint_14_angles[32] = {
#include "stg13/terrain/bar_throw_back_motion_joint_14_angles.inc"
};
static NJS_MKEY_A bar_throw_back_motion_joint_17_angles[32] = {
#include "stg13/terrain/bar_throw_back_motion_joint_17_angles.inc"
};
static NJS_MKEY_A bar_throw_back_motion_joint_18_angles[32] = {
#include "stg13/terrain/bar_throw_back_motion_joint_18_angles.inc"
};
static NJS_MKEY_A bar_throw_back_motion_joint_19_angles[32] = {
#include "stg13/terrain/bar_throw_back_motion_joint_19_angles.inc"
};
static NJS_MKEY_A bar_throw_back_motion_joint_23_angles[32] = {
#include "stg13/terrain/bar_throw_back_motion_joint_23_angles.inc"
};
static NJS_MKEY_A bar_throw_back_motion_joint_24_angles[32] = {
#include "stg13/terrain/bar_throw_back_motion_joint_24_angles.inc"
};
static NJS_MKEY_A bar_throw_back_motion_joint_27_angles[2] = {
#include "stg13/terrain/bar_throw_back_motion_joint_27_angles.inc"
};
static NJS_MKEY_A bar_throw_back_motion_joint_28_angles[2] = {
#include "stg13/terrain/bar_throw_back_motion_joint_28_angles.inc"
};
static NJS_MKEY_A bar_throw_back_motion_joint_29_angles[2] = {
#include "stg13/terrain/bar_throw_back_motion_joint_29_angles.inc"
};
static NJS_MKEY_A bar_throw_back_motion_joint_32_angles[32] = {
#include "stg13/terrain/bar_throw_back_motion_joint_32_angles.inc"
};
static NJS_MKEY_A bar_throw_back_motion_joint_33_angles[25] = {
#include "stg13/terrain/bar_throw_back_motion_joint_33_angles.inc"
};
static NJS_MKEY_A bar_throw_back_motion_joint_37_angles[32] = {
#include "stg13/terrain/bar_throw_back_motion_joint_37_angles.inc"
};
static NJS_MKEY_A bar_throw_back_motion_joint_38_angles[32] = {
#include "stg13/terrain/bar_throw_back_motion_joint_38_angles.inc"
};
static NJS_MKEY_A bar_throw_back_motion_joint_39_angles[32] = {
#include "stg13/terrain/bar_throw_back_motion_joint_39_angles.inc"
};
static NJS_MKEY_A bar_throw_back_motion_joint_41_angles[17] = {
#include "stg13/terrain/bar_throw_back_motion_joint_41_angles.inc"
};
static NJS_MKEY_A bar_throw_back_motion_joint_44_angles[32] = {
#include "stg13/terrain/bar_throw_back_motion_joint_44_angles.inc"
};
static NJS_MKEY_A bar_throw_back_motion_joint_51_angles[32] = {
#include "stg13/terrain/bar_throw_back_motion_joint_51_angles.inc"
};
static NJS_MKEY_A bar_throw_back_motion_joint_53_angles[32] = {
#include "stg13/terrain/bar_throw_back_motion_joint_53_angles.inc"
};
static NJS_MKEY_A bar_throw_back_motion_joint_56_angles[32] = {
#include "stg13/terrain/bar_throw_back_motion_joint_56_angles.inc"
};
static NJS_MKEY_A bar_throw_back_motion_joint_57_angles[32] = {
#include "stg13/terrain/bar_throw_back_motion_joint_57_angles.inc"
};
static NJS_MKEY_A bar_throw_back_motion_joint_58_angles[32] = {
#include "stg13/terrain/bar_throw_back_motion_joint_58_angles.inc"
};
static NJS_MKEY_A bar_throw_back_motion_joint_60_angles[17] = {
#include "stg13/terrain/bar_throw_back_motion_joint_60_angles.inc"
};
static NJS_MDATA2 bar_throw_back_motion_tracks[62] = {
#include "stg13/terrain/bar_throw_back_motion_tracks.inc"
};
static NJS_MOTION bar_throw_back_motion = {
#include "stg13/terrain/bar_throw_back_motion.inc"
};
static NJS_MKEY_F bar_throw_motion_joint_0_position[2] = {
#include "stg13/terrain/bar_throw_motion_joint_0_position.inc"
};
static NJS_MKEY_F bar_throw_motion_joint_2_position[2] = {
#include "stg13/terrain/bar_throw_motion_joint_2_position.inc"
};
static NJS_MKEY_A bar_throw_motion_joint_4_angles[33] = {
#include "stg13/terrain/bar_throw_motion_joint_4_angles.inc"
};
static NJS_MKEY_A bar_throw_motion_joint_7_angles[33] = {
#include "stg13/terrain/bar_throw_motion_joint_7_angles.inc"
};
static NJS_MKEY_A bar_throw_motion_joint_8_angles[33] = {
#include "stg13/terrain/bar_throw_motion_joint_8_angles.inc"
};
static NJS_MKEY_A bar_throw_motion_joint_9_angles[33] = {
#include "stg13/terrain/bar_throw_motion_joint_9_angles.inc"
};
static NJS_MKEY_A bar_throw_motion_joint_13_angles[33] = {
#include "stg13/terrain/bar_throw_motion_joint_13_angles.inc"
};
static NJS_MKEY_A bar_throw_motion_joint_14_angles[33] = {
#include "stg13/terrain/bar_throw_motion_joint_14_angles.inc"
};
static NJS_MKEY_A bar_throw_motion_joint_17_angles[33] = {
#include "stg13/terrain/bar_throw_motion_joint_17_angles.inc"
};
static NJS_MKEY_A bar_throw_motion_joint_18_angles[33] = {
#include "stg13/terrain/bar_throw_motion_joint_18_angles.inc"
};
static NJS_MKEY_A bar_throw_motion_joint_19_angles[33] = {
#include "stg13/terrain/bar_throw_motion_joint_19_angles.inc"
};
static NJS_MKEY_A bar_throw_motion_joint_23_angles[33] = {
#include "stg13/terrain/bar_throw_motion_joint_23_angles.inc"
};
static NJS_MKEY_A bar_throw_motion_joint_24_angles[33] = {
#include "stg13/terrain/bar_throw_motion_joint_24_angles.inc"
};
static NJS_MKEY_A bar_throw_motion_joint_27_angles[33] = {
#include "stg13/terrain/bar_throw_motion_joint_27_angles.inc"
};
static NJS_MKEY_A bar_throw_motion_joint_28_angles[15] = {
#include "stg13/terrain/bar_throw_motion_joint_28_angles.inc"
};
static NJS_MKEY_A bar_throw_motion_joint_29_angles[15] = {
#include "stg13/terrain/bar_throw_motion_joint_29_angles.inc"
};
static NJS_MKEY_A bar_throw_motion_joint_32_angles[33] = {
#include "stg13/terrain/bar_throw_motion_joint_32_angles.inc"
};
static NJS_MKEY_A bar_throw_motion_joint_33_angles[26] = {
#include "stg13/terrain/bar_throw_motion_joint_33_angles.inc"
};
static NJS_MKEY_A bar_throw_motion_joint_37_angles[33] = {
#include "stg13/terrain/bar_throw_motion_joint_37_angles.inc"
};
static NJS_MKEY_A bar_throw_motion_joint_38_angles[33] = {
#include "stg13/terrain/bar_throw_motion_joint_38_angles.inc"
};
static NJS_MKEY_A bar_throw_motion_joint_39_angles[33] = {
#include "stg13/terrain/bar_throw_motion_joint_39_angles.inc"
};
static NJS_MKEY_A bar_throw_motion_joint_41_angles[2] = {
#include "stg13/terrain/bar_throw_motion_joint_41_angles.inc"
};
static NJS_MKEY_A bar_throw_motion_joint_44_angles[33] = {
#include "stg13/terrain/bar_throw_motion_joint_44_angles.inc"
};
static NJS_MKEY_A bar_throw_motion_joint_51_angles[33] = {
#include "stg13/terrain/bar_throw_motion_joint_51_angles.inc"
};
static NJS_MKEY_A bar_throw_motion_joint_53_angles[33] = {
#include "stg13/terrain/bar_throw_motion_joint_53_angles.inc"
};
static NJS_MKEY_A bar_throw_motion_joint_56_angles[33] = {
#include "stg13/terrain/bar_throw_motion_joint_56_angles.inc"
};
static NJS_MKEY_A bar_throw_motion_joint_57_angles[33] = {
#include "stg13/terrain/bar_throw_motion_joint_57_angles.inc"
};
static NJS_MKEY_A bar_throw_motion_joint_58_angles[33] = {
#include "stg13/terrain/bar_throw_motion_joint_58_angles.inc"
};
static NJS_MKEY_A bar_throw_motion_joint_60_angles[2] = {
#include "stg13/terrain/bar_throw_motion_joint_60_angles.inc"
};
static NJS_MDATA2 bar_throw_motion_tracks[62] = {
#include "stg13/terrain/bar_throw_motion_tracks.inc"
};
static NJS_MOTION bar_throw_motion = {
#include "stg13/terrain/bar_throw_motion.inc"
};
static NJS_MKEY_F bar_fly_back_motion_joint_0_position[2] = {
#include "stg13/terrain/bar_fly_back_motion_joint_0_position.inc"
};
static NJS_MKEY_F bar_fly_back_motion_joint_2_position[2] = {
#include "stg13/terrain/bar_fly_back_motion_joint_2_position.inc"
};
static NJS_MKEY_A bar_fly_back_motion_joint_4_angles[2] = {
#include "stg13/terrain/bar_fly_back_motion_joint_4_angles.inc"
};
static NJS_MKEY_A bar_fly_back_motion_joint_7_angles[2] = {
#include "stg13/terrain/bar_fly_back_motion_joint_7_angles.inc"
};
static NJS_MKEY_A bar_fly_back_motion_joint_8_angles[2] = {
#include "stg13/terrain/bar_fly_back_motion_joint_8_angles.inc"
};
static NJS_MKEY_A bar_fly_back_motion_joint_9_angles[2] = {
#include "stg13/terrain/bar_fly_back_motion_joint_9_angles.inc"
};
static NJS_MKEY_A bar_fly_back_motion_joint_13_angles[2] = {
#include "stg13/terrain/bar_fly_back_motion_joint_13_angles.inc"
};
static NJS_MKEY_A bar_fly_back_motion_joint_14_angles[2] = {
#include "stg13/terrain/bar_fly_back_motion_joint_14_angles.inc"
};
static NJS_MKEY_A bar_fly_back_motion_joint_17_angles[2] = {
#include "stg13/terrain/bar_fly_back_motion_joint_17_angles.inc"
};
static NJS_MKEY_A bar_fly_back_motion_joint_18_angles[2] = {
#include "stg13/terrain/bar_fly_back_motion_joint_18_angles.inc"
};
static NJS_MKEY_A bar_fly_back_motion_joint_19_angles[2] = {
#include "stg13/terrain/bar_fly_back_motion_joint_19_angles.inc"
};
static NJS_MKEY_A bar_fly_back_motion_joint_23_angles[2] = {
#include "stg13/terrain/bar_fly_back_motion_joint_23_angles.inc"
};
static NJS_MKEY_A bar_fly_back_motion_joint_24_angles[2] = {
#include "stg13/terrain/bar_fly_back_motion_joint_24_angles.inc"
};
static NJS_MKEY_A bar_fly_back_motion_joint_27_angles[2] = {
#include "stg13/terrain/bar_fly_back_motion_joint_27_angles.inc"
};
static NJS_MKEY_A bar_fly_back_motion_joint_32_angles[2] = {
#include "stg13/terrain/bar_fly_back_motion_joint_32_angles.inc"
};
static NJS_MKEY_A bar_fly_back_motion_joint_37_angles[2] = {
#include "stg13/terrain/bar_fly_back_motion_joint_37_angles.inc"
};
static NJS_MKEY_A bar_fly_back_motion_joint_38_angles[2] = {
#include "stg13/terrain/bar_fly_back_motion_joint_38_angles.inc"
};
static NJS_MKEY_A bar_fly_back_motion_joint_39_angles[2] = {
#include "stg13/terrain/bar_fly_back_motion_joint_39_angles.inc"
};
static NJS_MKEY_A bar_fly_back_motion_joint_41_angles[2] = {
#include "stg13/terrain/bar_fly_back_motion_joint_41_angles.inc"
};
static NJS_MKEY_A bar_fly_back_motion_joint_44_angles[2] = {
#include "stg13/terrain/bar_fly_back_motion_joint_44_angles.inc"
};
static NJS_MKEY_A bar_fly_back_motion_joint_51_angles[2] = {
#include "stg13/terrain/bar_fly_back_motion_joint_51_angles.inc"
};
static NJS_MKEY_A bar_fly_back_motion_joint_53_angles[2] = {
#include "stg13/terrain/bar_fly_back_motion_joint_53_angles.inc"
};
static NJS_MKEY_A bar_fly_back_motion_joint_56_angles[6] = {
#include "stg13/terrain/bar_fly_back_motion_joint_56_angles.inc"
};
static NJS_MKEY_A bar_fly_back_motion_joint_57_angles[2] = {
#include "stg13/terrain/bar_fly_back_motion_joint_57_angles.inc"
};
static NJS_MKEY_A bar_fly_back_motion_joint_58_angles[3] = {
#include "stg13/terrain/bar_fly_back_motion_joint_58_angles.inc"
};
static NJS_MKEY_A bar_fly_back_motion_joint_60_angles[2] = {
#include "stg13/terrain/bar_fly_back_motion_joint_60_angles.inc"
};
static NJS_MDATA2 bar_fly_back_motion_tracks[62] = {
#include "stg13/terrain/bar_fly_back_motion_tracks.inc"
};
static NJS_MOTION bar_fly_back_motion = {
#include "stg13/terrain/bar_fly_back_motion.inc"
};
static NJS_MKEY_F bar_fly_motion_joint_0_position[2] = {
#include "stg13/terrain/bar_fly_motion_joint_0_position.inc"
};
static NJS_MKEY_F bar_fly_motion_joint_2_position[2] = {
#include "stg13/terrain/bar_fly_motion_joint_2_position.inc"
};
static NJS_MKEY_A bar_fly_motion_joint_4_angles[2] = {
#include "stg13/terrain/bar_fly_motion_joint_4_angles.inc"
};
static NJS_MKEY_A bar_fly_motion_joint_7_angles[2] = {
#include "stg13/terrain/bar_fly_motion_joint_7_angles.inc"
};
static NJS_MKEY_A bar_fly_motion_joint_8_angles[2] = {
#include "stg13/terrain/bar_fly_motion_joint_8_angles.inc"
};
static NJS_MKEY_A bar_fly_motion_joint_9_angles[2] = {
#include "stg13/terrain/bar_fly_motion_joint_9_angles.inc"
};
static NJS_MKEY_A bar_fly_motion_joint_13_angles[2] = {
#include "stg13/terrain/bar_fly_motion_joint_13_angles.inc"
};
static NJS_MKEY_A bar_fly_motion_joint_14_angles[2] = {
#include "stg13/terrain/bar_fly_motion_joint_14_angles.inc"
};
static NJS_MKEY_A bar_fly_motion_joint_17_angles[2] = {
#include "stg13/terrain/bar_fly_motion_joint_17_angles.inc"
};
static NJS_MKEY_A bar_fly_motion_joint_18_angles[2] = {
#include "stg13/terrain/bar_fly_motion_joint_18_angles.inc"
};
static NJS_MKEY_A bar_fly_motion_joint_19_angles[2] = {
#include "stg13/terrain/bar_fly_motion_joint_19_angles.inc"
};
static NJS_MKEY_A bar_fly_motion_joint_23_angles[2] = {
#include "stg13/terrain/bar_fly_motion_joint_23_angles.inc"
};
static NJS_MKEY_A bar_fly_motion_joint_24_angles[2] = {
#include "stg13/terrain/bar_fly_motion_joint_24_angles.inc"
};
static NJS_MKEY_A bar_fly_motion_joint_27_angles[2] = {
#include "stg13/terrain/bar_fly_motion_joint_27_angles.inc"
};
static NJS_MKEY_A bar_fly_motion_joint_32_angles[2] = {
#include "stg13/terrain/bar_fly_motion_joint_32_angles.inc"
};
static NJS_MKEY_A bar_fly_motion_joint_37_angles[2] = {
#include "stg13/terrain/bar_fly_motion_joint_37_angles.inc"
};
static NJS_MKEY_A bar_fly_motion_joint_38_angles[2] = {
#include "stg13/terrain/bar_fly_motion_joint_38_angles.inc"
};
static NJS_MKEY_A bar_fly_motion_joint_39_angles[2] = {
#include "stg13/terrain/bar_fly_motion_joint_39_angles.inc"
};
static NJS_MKEY_A bar_fly_motion_joint_41_angles[2] = {
#include "stg13/terrain/bar_fly_motion_joint_41_angles.inc"
};
static NJS_MKEY_A bar_fly_motion_joint_44_angles[2] = {
#include "stg13/terrain/bar_fly_motion_joint_44_angles.inc"
};
static NJS_MKEY_A bar_fly_motion_joint_51_angles[2] = {
#include "stg13/terrain/bar_fly_motion_joint_51_angles.inc"
};
static NJS_MKEY_A bar_fly_motion_joint_53_angles[2] = {
#include "stg13/terrain/bar_fly_motion_joint_53_angles.inc"
};
static NJS_MKEY_A bar_fly_motion_joint_56_angles[3] = {
#include "stg13/terrain/bar_fly_motion_joint_56_angles.inc"
};
static NJS_MKEY_A bar_fly_motion_joint_57_angles[2] = {
#include "stg13/terrain/bar_fly_motion_joint_57_angles.inc"
};
static NJS_MKEY_A bar_fly_motion_joint_58_angles[2] = {
#include "stg13/terrain/bar_fly_motion_joint_58_angles.inc"
};
static NJS_MKEY_A bar_fly_motion_joint_60_angles[2] = {
#include "stg13/terrain/bar_fly_motion_joint_60_angles.inc"
};
static NJS_MDATA2 bar_fly_motion_tracks[62] = {
#include "stg13/terrain/bar_fly_motion_tracks.inc"
};
static NJS_MOTION bar_fly_motion = {
#include "stg13/terrain/bar_fly_motion.inc"
};
static NJS_MKEY_F bar_swing_back_motion_joint_0_position[2] = {
#include "stg13/terrain/bar_swing_back_motion_joint_0_position.inc"
};
static NJS_MKEY_F bar_swing_back_motion_joint_2_position[2] = {
#include "stg13/terrain/bar_swing_back_motion_joint_2_position.inc"
};
static NJS_MKEY_A bar_swing_back_motion_joint_4_angles[32] = {
#include "stg13/terrain/bar_swing_back_motion_joint_4_angles.inc"
};
static NJS_MKEY_A bar_swing_back_motion_joint_7_angles[32] = {
#include "stg13/terrain/bar_swing_back_motion_joint_7_angles.inc"
};
static NJS_MKEY_A bar_swing_back_motion_joint_8_angles[32] = {
#include "stg13/terrain/bar_swing_back_motion_joint_8_angles.inc"
};
static NJS_MKEY_A bar_swing_back_motion_joint_9_angles[32] = {
#include "stg13/terrain/bar_swing_back_motion_joint_9_angles.inc"
};
static NJS_MKEY_A bar_swing_back_motion_joint_13_angles[32] = {
#include "stg13/terrain/bar_swing_back_motion_joint_13_angles.inc"
};
static NJS_MKEY_A bar_swing_back_motion_joint_14_angles[25] = {
#include "stg13/terrain/bar_swing_back_motion_joint_14_angles.inc"
};
static NJS_MKEY_A bar_swing_back_motion_joint_17_angles[32] = {
#include "stg13/terrain/bar_swing_back_motion_joint_17_angles.inc"
};
static NJS_MKEY_A bar_swing_back_motion_joint_18_angles[32] = {
#include "stg13/terrain/bar_swing_back_motion_joint_18_angles.inc"
};
static NJS_MKEY_A bar_swing_back_motion_joint_19_angles[32] = {
#include "stg13/terrain/bar_swing_back_motion_joint_19_angles.inc"
};
static NJS_MKEY_A bar_swing_back_motion_joint_23_angles[32] = {
#include "stg13/terrain/bar_swing_back_motion_joint_23_angles.inc"
};
static NJS_MKEY_A bar_swing_back_motion_joint_24_angles[32] = {
#include "stg13/terrain/bar_swing_back_motion_joint_24_angles.inc"
};
static NJS_MKEY_A bar_swing_back_motion_joint_27_angles[32] = {
#include "stg13/terrain/bar_swing_back_motion_joint_27_angles.inc"
};
static NJS_MKEY_A bar_swing_back_motion_joint_28_angles[28] = {
#include "stg13/terrain/bar_swing_back_motion_joint_28_angles.inc"
};
static NJS_MKEY_A bar_swing_back_motion_joint_29_angles[29] = {
#include "stg13/terrain/bar_swing_back_motion_joint_29_angles.inc"
};
static NJS_MKEY_A bar_swing_back_motion_joint_31_angles[2] = {
#include "stg13/terrain/bar_swing_back_motion_joint_31_angles.inc"
};
static NJS_MKEY_A bar_swing_back_motion_joint_32_angles[2] = {
#include "stg13/terrain/bar_swing_back_motion_joint_32_angles.inc"
};
static NJS_MKEY_A bar_swing_back_motion_joint_33_angles[2] = {
#include "stg13/terrain/bar_swing_back_motion_joint_33_angles.inc"
};
static NJS_MKEY_A bar_swing_back_motion_joint_37_angles[2] = {
#include "stg13/terrain/bar_swing_back_motion_joint_37_angles.inc"
};
static NJS_MKEY_A bar_swing_back_motion_joint_38_angles[2] = {
#include "stg13/terrain/bar_swing_back_motion_joint_38_angles.inc"
};
static NJS_MKEY_A bar_swing_back_motion_joint_39_angles[2] = {
#include "stg13/terrain/bar_swing_back_motion_joint_39_angles.inc"
};
static NJS_MKEY_A bar_swing_back_motion_joint_41_angles[2] = {
#include "stg13/terrain/bar_swing_back_motion_joint_41_angles.inc"
};
static NJS_MKEY_A bar_swing_back_motion_joint_44_angles[32] = {
#include "stg13/terrain/bar_swing_back_motion_joint_44_angles.inc"
};
static NJS_MKEY_A bar_swing_back_motion_joint_51_angles[32] = {
#include "stg13/terrain/bar_swing_back_motion_joint_51_angles.inc"
};
static NJS_MKEY_A bar_swing_back_motion_joint_53_angles[32] = {
#include "stg13/terrain/bar_swing_back_motion_joint_53_angles.inc"
};
static NJS_MKEY_A bar_swing_back_motion_joint_56_angles[2] = {
#include "stg13/terrain/bar_swing_back_motion_joint_56_angles.inc"
};
static NJS_MKEY_A bar_swing_back_motion_joint_57_angles[2] = {
#include "stg13/terrain/bar_swing_back_motion_joint_57_angles.inc"
};
static NJS_MKEY_A bar_swing_back_motion_joint_58_angles[2] = {
#include "stg13/terrain/bar_swing_back_motion_joint_58_angles.inc"
};
static NJS_MKEY_A bar_swing_back_motion_joint_60_angles[2] = {
#include "stg13/terrain/bar_swing_back_motion_joint_60_angles.inc"
};
static NJS_MDATA2 bar_swing_back_motion_tracks[62] = {
#include "stg13/terrain/bar_swing_back_motion_tracks.inc"
};
static NJS_MOTION bar_swing_back_motion = {
#include "stg13/terrain/bar_swing_back_motion.inc"
};
static NJS_MKEY_F bar_swing_motion_joint_0_position[2] = {
#include "stg13/terrain/bar_swing_motion_joint_0_position.inc"
};
static NJS_MKEY_F bar_swing_motion_joint_2_position[2] = {
#include "stg13/terrain/bar_swing_motion_joint_2_position.inc"
};
static NJS_MKEY_A bar_swing_motion_joint_4_angles[32] = {
#include "stg13/terrain/bar_swing_motion_joint_4_angles.inc"
};
static NJS_MKEY_A bar_swing_motion_joint_7_angles[32] = {
#include "stg13/terrain/bar_swing_motion_joint_7_angles.inc"
};
static NJS_MKEY_A bar_swing_motion_joint_8_angles[32] = {
#include "stg13/terrain/bar_swing_motion_joint_8_angles.inc"
};
static NJS_MKEY_A bar_swing_motion_joint_9_angles[32] = {
#include "stg13/terrain/bar_swing_motion_joint_9_angles.inc"
};
static NJS_MKEY_A bar_swing_motion_joint_13_angles[32] = {
#include "stg13/terrain/bar_swing_motion_joint_13_angles.inc"
};
static NJS_MKEY_A bar_swing_motion_joint_14_angles[28] = {
#include "stg13/terrain/bar_swing_motion_joint_14_angles.inc"
};
static NJS_MKEY_A bar_swing_motion_joint_17_angles[32] = {
#include "stg13/terrain/bar_swing_motion_joint_17_angles.inc"
};
static NJS_MKEY_A bar_swing_motion_joint_18_angles[32] = {
#include "stg13/terrain/bar_swing_motion_joint_18_angles.inc"
};
static NJS_MKEY_A bar_swing_motion_joint_19_angles[32] = {
#include "stg13/terrain/bar_swing_motion_joint_19_angles.inc"
};
static NJS_MKEY_A bar_swing_motion_joint_23_angles[32] = {
#include "stg13/terrain/bar_swing_motion_joint_23_angles.inc"
};
static NJS_MKEY_A bar_swing_motion_joint_24_angles[28] = {
#include "stg13/terrain/bar_swing_motion_joint_24_angles.inc"
};
static NJS_MKEY_A bar_swing_motion_joint_27_angles[32] = {
#include "stg13/terrain/bar_swing_motion_joint_27_angles.inc"
};
static NJS_MKEY_A bar_swing_motion_joint_28_angles[32] = {
#include "stg13/terrain/bar_swing_motion_joint_28_angles.inc"
};
static NJS_MKEY_A bar_swing_motion_joint_29_angles[32] = {
#include "stg13/terrain/bar_swing_motion_joint_29_angles.inc"
};
static NJS_MKEY_A bar_swing_motion_joint_31_angles[2] = {
#include "stg13/terrain/bar_swing_motion_joint_31_angles.inc"
};
static NJS_MKEY_A bar_swing_motion_joint_32_angles[2] = {
#include "stg13/terrain/bar_swing_motion_joint_32_angles.inc"
};
static NJS_MKEY_A bar_swing_motion_joint_33_angles[2] = {
#include "stg13/terrain/bar_swing_motion_joint_33_angles.inc"
};
static NJS_MKEY_A bar_swing_motion_joint_37_angles[2] = {
#include "stg13/terrain/bar_swing_motion_joint_37_angles.inc"
};
static NJS_MKEY_A bar_swing_motion_joint_38_angles[2] = {
#include "stg13/terrain/bar_swing_motion_joint_38_angles.inc"
};
static NJS_MKEY_A bar_swing_motion_joint_39_angles[2] = {
#include "stg13/terrain/bar_swing_motion_joint_39_angles.inc"
};
static NJS_MKEY_A bar_swing_motion_joint_41_angles[2] = {
#include "stg13/terrain/bar_swing_motion_joint_41_angles.inc"
};
static NJS_MKEY_A bar_swing_motion_joint_44_angles[32] = {
#include "stg13/terrain/bar_swing_motion_joint_44_angles.inc"
};
static NJS_MKEY_A bar_swing_motion_joint_51_angles[32] = {
#include "stg13/terrain/bar_swing_motion_joint_51_angles.inc"
};
static NJS_MKEY_A bar_swing_motion_joint_53_angles[32] = {
#include "stg13/terrain/bar_swing_motion_joint_53_angles.inc"
};
static NJS_MKEY_A bar_swing_motion_joint_56_angles[2] = {
#include "stg13/terrain/bar_swing_motion_joint_56_angles.inc"
};
static NJS_MKEY_A bar_swing_motion_joint_57_angles[2] = {
#include "stg13/terrain/bar_swing_motion_joint_57_angles.inc"
};
static NJS_MKEY_A bar_swing_motion_joint_58_angles[2] = {
#include "stg13/terrain/bar_swing_motion_joint_58_angles.inc"
};
static NJS_MKEY_A bar_swing_motion_joint_60_angles[2] = {
#include "stg13/terrain/bar_swing_motion_joint_60_angles.inc"
};
static NJS_MDATA2 bar_swing_motion_tracks[62] = {
#include "stg13/terrain/bar_swing_motion_tracks.inc"
};
static NJS_MOTION bar_swing_motion = {
#include "stg13/terrain/bar_swing_motion.inc"
};
static NJS_MKEY_F bar_land_motion_joint_2_position[9] = {
#include "stg13/terrain/bar_land_motion_joint_2_position.inc"
};
static NJS_MKEY_A bar_land_motion_joint_2_angles[2] = {
#include "stg13/terrain/bar_land_motion_joint_2_angles.inc"
};
static NJS_MKEY_A bar_land_motion_joint_4_angles[2] = {
#include "stg13/terrain/bar_land_motion_joint_4_angles.inc"
};
static NJS_MKEY_A bar_land_motion_joint_7_angles[9] = {
#include "stg13/terrain/bar_land_motion_joint_7_angles.inc"
};
static NJS_MKEY_A bar_land_motion_joint_8_angles[9] = {
#include "stg13/terrain/bar_land_motion_joint_8_angles.inc"
};
static NJS_MKEY_A bar_land_motion_joint_9_angles[9] = {
#include "stg13/terrain/bar_land_motion_joint_9_angles.inc"
};
static NJS_MKEY_A bar_land_motion_joint_13_angles[9] = {
#include "stg13/terrain/bar_land_motion_joint_13_angles.inc"
};
static NJS_MKEY_A bar_land_motion_joint_14_angles[2] = {
#include "stg13/terrain/bar_land_motion_joint_14_angles.inc"
};
static NJS_MKEY_A bar_land_motion_joint_17_angles[9] = {
#include "stg13/terrain/bar_land_motion_joint_17_angles.inc"
};
static NJS_MKEY_A bar_land_motion_joint_18_angles[9] = {
#include "stg13/terrain/bar_land_motion_joint_18_angles.inc"
};
static NJS_MKEY_A bar_land_motion_joint_19_angles[9] = {
#include "stg13/terrain/bar_land_motion_joint_19_angles.inc"
};
static NJS_MKEY_A bar_land_motion_joint_23_angles[9] = {
#include "stg13/terrain/bar_land_motion_joint_23_angles.inc"
};
static NJS_MKEY_A bar_land_motion_joint_24_angles[4] = {
#include "stg13/terrain/bar_land_motion_joint_24_angles.inc"
};
static NJS_MKEY_A bar_land_motion_joint_27_angles[9] = {
#include "stg13/terrain/bar_land_motion_joint_27_angles.inc"
};
static NJS_MKEY_A bar_land_motion_joint_28_angles[9] = {
#include "stg13/terrain/bar_land_motion_joint_28_angles.inc"
};
static NJS_MKEY_A bar_land_motion_joint_29_angles[9] = {
#include "stg13/terrain/bar_land_motion_joint_29_angles.inc"
};
static NJS_MKEY_A bar_land_motion_joint_32_angles[6] = {
#include "stg13/terrain/bar_land_motion_joint_32_angles.inc"
};
static NJS_MKEY_A bar_land_motion_joint_33_angles[6] = {
#include "stg13/terrain/bar_land_motion_joint_33_angles.inc"
};
static NJS_MKEY_A bar_land_motion_joint_37_angles[5] = {
#include "stg13/terrain/bar_land_motion_joint_37_angles.inc"
};
static NJS_MKEY_A bar_land_motion_joint_38_angles[5] = {
#include "stg13/terrain/bar_land_motion_joint_38_angles.inc"
};
static NJS_MKEY_A bar_land_motion_joint_39_angles[5] = {
#include "stg13/terrain/bar_land_motion_joint_39_angles.inc"
};
static NJS_MKEY_A bar_land_motion_joint_41_angles[9] = {
#include "stg13/terrain/bar_land_motion_joint_41_angles.inc"
};
static NJS_MKEY_A bar_land_motion_joint_44_angles[9] = {
#include "stg13/terrain/bar_land_motion_joint_44_angles.inc"
};
static NJS_MKEY_A bar_land_motion_joint_51_angles[9] = {
#include "stg13/terrain/bar_land_motion_joint_51_angles.inc"
};
static NJS_MKEY_A bar_land_motion_joint_53_angles[9] = {
#include "stg13/terrain/bar_land_motion_joint_53_angles.inc"
};
static NJS_MKEY_A bar_land_motion_joint_56_angles[8] = {
#include "stg13/terrain/bar_land_motion_joint_56_angles.inc"
};
static NJS_MKEY_A bar_land_motion_joint_57_angles[5] = {
#include "stg13/terrain/bar_land_motion_joint_57_angles.inc"
};
static NJS_MKEY_A bar_land_motion_joint_58_angles[5] = {
#include "stg13/terrain/bar_land_motion_joint_58_angles.inc"
};
static NJS_MKEY_A bar_land_motion_joint_60_angles[9] = {
#include "stg13/terrain/bar_land_motion_joint_60_angles.inc"
};
static NJS_MDATA2 bar_land_motion_tracks[62] = {
#include "stg13/terrain/bar_land_motion_tracks.inc"
};
static NJS_MOTION bar_land_motion = {
#include "stg13/terrain/bar_land_motion.inc"
};
static NJS_MKEY_F rider_trick_harbor_a_motion_joint_2_position[140] = {
#include "stg13/terrain/rider_trick_harbor_a_motion_joint_2_position.inc"
};
static NJS_MKEY_A rider_trick_harbor_a_motion_joint_4_angles[140] = {
#include "stg13/terrain/rider_trick_harbor_a_motion_joint_4_angles.inc"
};
static NJS_MKEY_A rider_trick_harbor_a_motion_joint_7_angles[140] = {
#include "stg13/terrain/rider_trick_harbor_a_motion_joint_7_angles.inc"
};
static NJS_MKEY_A rider_trick_harbor_a_motion_joint_8_angles[130] = {
#include "stg13/terrain/rider_trick_harbor_a_motion_joint_8_angles.inc"
};
static NJS_MKEY_A rider_trick_harbor_a_motion_joint_9_angles[131] = {
#include "stg13/terrain/rider_trick_harbor_a_motion_joint_9_angles.inc"
};
static NJS_MKEY_A rider_trick_harbor_a_motion_joint_13_angles[140] = {
#include "stg13/terrain/rider_trick_harbor_a_motion_joint_13_angles.inc"
};
static NJS_MKEY_A rider_trick_harbor_a_motion_joint_14_angles[113] = {
#include "stg13/terrain/rider_trick_harbor_a_motion_joint_14_angles.inc"
};
static NJS_MKEY_A rider_trick_harbor_a_motion_joint_17_angles[140] = {
#include "stg13/terrain/rider_trick_harbor_a_motion_joint_17_angles.inc"
};
static NJS_MKEY_A rider_trick_harbor_a_motion_joint_18_angles[132] = {
#include "stg13/terrain/rider_trick_harbor_a_motion_joint_18_angles.inc"
};
static NJS_MKEY_A rider_trick_harbor_a_motion_joint_19_angles[140] = {
#include "stg13/terrain/rider_trick_harbor_a_motion_joint_19_angles.inc"
};
static NJS_MKEY_A rider_trick_harbor_a_motion_joint_23_angles[140] = {
#include "stg13/terrain/rider_trick_harbor_a_motion_joint_23_angles.inc"
};
static NJS_MKEY_A rider_trick_harbor_a_motion_joint_24_angles[107] = {
#include "stg13/terrain/rider_trick_harbor_a_motion_joint_24_angles.inc"
};
static NJS_MKEY_A rider_trick_harbor_a_motion_joint_27_angles[140] = {
#include "stg13/terrain/rider_trick_harbor_a_motion_joint_27_angles.inc"
};
static NJS_MKEY_A rider_trick_harbor_a_motion_joint_28_angles[101] = {
#include "stg13/terrain/rider_trick_harbor_a_motion_joint_28_angles.inc"
};
static NJS_MKEY_A rider_trick_harbor_a_motion_joint_29_angles[105] = {
#include "stg13/terrain/rider_trick_harbor_a_motion_joint_29_angles.inc"
};
static NJS_MKEY_A rider_trick_harbor_a_motion_joint_31_angles[140] = {
#include "stg13/terrain/rider_trick_harbor_a_motion_joint_31_angles.inc"
};
static NJS_MKEY_A rider_trick_harbor_a_motion_joint_32_angles[140] = {
#include "stg13/terrain/rider_trick_harbor_a_motion_joint_32_angles.inc"
};
static NJS_MKEY_A rider_trick_harbor_a_motion_joint_33_angles[100] = {
#include "stg13/terrain/rider_trick_harbor_a_motion_joint_33_angles.inc"
};
static NJS_MKEY_A rider_trick_harbor_a_motion_joint_37_angles[140] = {
#include "stg13/terrain/rider_trick_harbor_a_motion_joint_37_angles.inc"
};
static NJS_MKEY_A rider_trick_harbor_a_motion_joint_38_angles[99] = {
#include "stg13/terrain/rider_trick_harbor_a_motion_joint_38_angles.inc"
};
static NJS_MKEY_A rider_trick_harbor_a_motion_joint_39_angles[99] = {
#include "stg13/terrain/rider_trick_harbor_a_motion_joint_39_angles.inc"
};
static NJS_MKEY_A rider_trick_harbor_a_motion_joint_41_angles[140] = {
#include "stg13/terrain/rider_trick_harbor_a_motion_joint_41_angles.inc"
};
static NJS_MKEY_A rider_trick_harbor_a_motion_joint_44_angles[140] = {
#include "stg13/terrain/rider_trick_harbor_a_motion_joint_44_angles.inc"
};
static NJS_MKEY_A rider_trick_harbor_a_motion_joint_51_angles[2] = {
#include "stg13/terrain/rider_trick_harbor_a_motion_joint_51_angles.inc"
};
static NJS_MKEY_A rider_trick_harbor_a_motion_joint_53_angles[2] = {
#include "stg13/terrain/rider_trick_harbor_a_motion_joint_53_angles.inc"
};
static NJS_MKEY_A rider_trick_harbor_a_motion_joint_56_angles[140] = {
#include "stg13/terrain/rider_trick_harbor_a_motion_joint_56_angles.inc"
};
static NJS_MKEY_A rider_trick_harbor_a_motion_joint_57_angles[86] = {
#include "stg13/terrain/rider_trick_harbor_a_motion_joint_57_angles.inc"
};
static NJS_MKEY_A rider_trick_harbor_a_motion_joint_58_angles[86] = {
#include "stg13/terrain/rider_trick_harbor_a_motion_joint_58_angles.inc"
};
static NJS_MKEY_A rider_trick_harbor_a_motion_joint_60_angles[140] = {
#include "stg13/terrain/rider_trick_harbor_a_motion_joint_60_angles.inc"
};
static NJS_MDATA2 rider_trick_harbor_a_motion_tracks[62] = {
#include "stg13/terrain/rider_trick_harbor_a_motion_tracks.inc"
};
static NJS_MOTION rider_trick_harbor_a_motion = {
#include "stg13/terrain/rider_trick_harbor_a_motion.inc"
};
static NJS_MKEY_F rider_trick_harbor_b_motion_joint_2_position[121] = {
#include "stg13/terrain/rider_trick_harbor_b_motion_joint_2_position.inc"
};
static NJS_MKEY_A rider_trick_harbor_b_motion_joint_4_angles[121] = {
#include "stg13/terrain/rider_trick_harbor_b_motion_joint_4_angles.inc"
};
static NJS_MKEY_A rider_trick_harbor_b_motion_joint_7_angles[121] = {
#include "stg13/terrain/rider_trick_harbor_b_motion_joint_7_angles.inc"
};
static NJS_MKEY_A rider_trick_harbor_b_motion_joint_8_angles[106] = {
#include "stg13/terrain/rider_trick_harbor_b_motion_joint_8_angles.inc"
};
static NJS_MKEY_A rider_trick_harbor_b_motion_joint_9_angles[121] = {
#include "stg13/terrain/rider_trick_harbor_b_motion_joint_9_angles.inc"
};
static NJS_MKEY_A rider_trick_harbor_b_motion_joint_13_angles[121] = {
#include "stg13/terrain/rider_trick_harbor_b_motion_joint_13_angles.inc"
};
static NJS_MKEY_A rider_trick_harbor_b_motion_joint_14_angles[105] = {
#include "stg13/terrain/rider_trick_harbor_b_motion_joint_14_angles.inc"
};
static NJS_MKEY_A rider_trick_harbor_b_motion_joint_17_angles[121] = {
#include "stg13/terrain/rider_trick_harbor_b_motion_joint_17_angles.inc"
};
static NJS_MKEY_A rider_trick_harbor_b_motion_joint_18_angles[94] = {
#include "stg13/terrain/rider_trick_harbor_b_motion_joint_18_angles.inc"
};
static NJS_MKEY_A rider_trick_harbor_b_motion_joint_19_angles[94] = {
#include "stg13/terrain/rider_trick_harbor_b_motion_joint_19_angles.inc"
};
static NJS_MKEY_A rider_trick_harbor_b_motion_joint_23_angles[121] = {
#include "stg13/terrain/rider_trick_harbor_b_motion_joint_23_angles.inc"
};
static NJS_MKEY_A rider_trick_harbor_b_motion_joint_24_angles[106] = {
#include "stg13/terrain/rider_trick_harbor_b_motion_joint_24_angles.inc"
};
static NJS_MKEY_A rider_trick_harbor_b_motion_joint_27_angles[121] = {
#include "stg13/terrain/rider_trick_harbor_b_motion_joint_27_angles.inc"
};
static NJS_MKEY_A rider_trick_harbor_b_motion_joint_28_angles[84] = {
#include "stg13/terrain/rider_trick_harbor_b_motion_joint_28_angles.inc"
};
static NJS_MKEY_A rider_trick_harbor_b_motion_joint_29_angles[91] = {
#include "stg13/terrain/rider_trick_harbor_b_motion_joint_29_angles.inc"
};
static NJS_MKEY_A rider_trick_harbor_b_motion_joint_31_angles[121] = {
#include "stg13/terrain/rider_trick_harbor_b_motion_joint_31_angles.inc"
};
static NJS_MKEY_A rider_trick_harbor_b_motion_joint_32_angles[121] = {
#include "stg13/terrain/rider_trick_harbor_b_motion_joint_32_angles.inc"
};
static NJS_MKEY_A rider_trick_harbor_b_motion_joint_33_angles[85] = {
#include "stg13/terrain/rider_trick_harbor_b_motion_joint_33_angles.inc"
};
static NJS_MKEY_A rider_trick_harbor_b_motion_joint_37_angles[121] = {
#include "stg13/terrain/rider_trick_harbor_b_motion_joint_37_angles.inc"
};
static NJS_MKEY_A rider_trick_harbor_b_motion_joint_38_angles[105] = {
#include "stg13/terrain/rider_trick_harbor_b_motion_joint_38_angles.inc"
};
static NJS_MKEY_A rider_trick_harbor_b_motion_joint_39_angles[105] = {
#include "stg13/terrain/rider_trick_harbor_b_motion_joint_39_angles.inc"
};
static NJS_MKEY_A rider_trick_harbor_b_motion_joint_41_angles[121] = {
#include "stg13/terrain/rider_trick_harbor_b_motion_joint_41_angles.inc"
};
static NJS_MKEY_A rider_trick_harbor_b_motion_joint_44_angles[121] = {
#include "stg13/terrain/rider_trick_harbor_b_motion_joint_44_angles.inc"
};
static NJS_MKEY_A rider_trick_harbor_b_motion_joint_51_angles[2] = {
#include "stg13/terrain/rider_trick_harbor_b_motion_joint_51_angles.inc"
};
static NJS_MKEY_A rider_trick_harbor_b_motion_joint_53_angles[2] = {
#include "stg13/terrain/rider_trick_harbor_b_motion_joint_53_angles.inc"
};
static NJS_MKEY_A rider_trick_harbor_b_motion_joint_56_angles[121] = {
#include "stg13/terrain/rider_trick_harbor_b_motion_joint_56_angles.inc"
};
static NJS_MKEY_A rider_trick_harbor_b_motion_joint_57_angles[118] = {
#include "stg13/terrain/rider_trick_harbor_b_motion_joint_57_angles.inc"
};
static NJS_MKEY_A rider_trick_harbor_b_motion_joint_58_angles[118] = {
#include "stg13/terrain/rider_trick_harbor_b_motion_joint_58_angles.inc"
};
static NJS_MKEY_A rider_trick_harbor_b_motion_joint_60_angles[121] = {
#include "stg13/terrain/rider_trick_harbor_b_motion_joint_60_angles.inc"
};
static NJS_MDATA2 rider_trick_harbor_b_motion_tracks[62] = {
#include "stg13/terrain/rider_trick_harbor_b_motion_tracks.inc"
};
static NJS_MOTION rider_trick_harbor_b_motion = {
#include "stg13/terrain/rider_trick_harbor_b_motion.inc"
};
static NJS_MKEY_F rider_trick_city_a_motion_joint_2_position[146] = {
#include "stg13/terrain/rider_trick_city_a_motion_joint_2_position.inc"
};
static NJS_MKEY_A rider_trick_city_a_motion_joint_4_angles[146] = {
#include "stg13/terrain/rider_trick_city_a_motion_joint_4_angles.inc"
};
static NJS_MKEY_A rider_trick_city_a_motion_joint_7_angles[146] = {
#include "stg13/terrain/rider_trick_city_a_motion_joint_7_angles.inc"
};
static NJS_MKEY_A rider_trick_city_a_motion_joint_8_angles[137] = {
#include "stg13/terrain/rider_trick_city_a_motion_joint_8_angles.inc"
};
static NJS_MKEY_A rider_trick_city_a_motion_joint_9_angles[145] = {
#include "stg13/terrain/rider_trick_city_a_motion_joint_9_angles.inc"
};
static NJS_MKEY_A rider_trick_city_a_motion_joint_13_angles[146] = {
#include "stg13/terrain/rider_trick_city_a_motion_joint_13_angles.inc"
};
static NJS_MKEY_A rider_trick_city_a_motion_joint_14_angles[138] = {
#include "stg13/terrain/rider_trick_city_a_motion_joint_14_angles.inc"
};
static NJS_MKEY_A rider_trick_city_a_motion_joint_17_angles[146] = {
#include "stg13/terrain/rider_trick_city_a_motion_joint_17_angles.inc"
};
static NJS_MKEY_A rider_trick_city_a_motion_joint_18_angles[142] = {
#include "stg13/terrain/rider_trick_city_a_motion_joint_18_angles.inc"
};
static NJS_MKEY_A rider_trick_city_a_motion_joint_19_angles[146] = {
#include "stg13/terrain/rider_trick_city_a_motion_joint_19_angles.inc"
};
static NJS_MKEY_A rider_trick_city_a_motion_joint_23_angles[146] = {
#include "stg13/terrain/rider_trick_city_a_motion_joint_23_angles.inc"
};
static NJS_MKEY_A rider_trick_city_a_motion_joint_24_angles[143] = {
#include "stg13/terrain/rider_trick_city_a_motion_joint_24_angles.inc"
};
static NJS_MKEY_A rider_trick_city_a_motion_joint_27_angles[146] = {
#include "stg13/terrain/rider_trick_city_a_motion_joint_27_angles.inc"
};
static NJS_MKEY_A rider_trick_city_a_motion_joint_28_angles[40] = {
#include "stg13/terrain/rider_trick_city_a_motion_joint_28_angles.inc"
};
static NJS_MKEY_A rider_trick_city_a_motion_joint_29_angles[40] = {
#include "stg13/terrain/rider_trick_city_a_motion_joint_29_angles.inc"
};
static NJS_MKEY_A rider_trick_city_a_motion_joint_31_angles[146] = {
#include "stg13/terrain/rider_trick_city_a_motion_joint_31_angles.inc"
};
static NJS_MKEY_A rider_trick_city_a_motion_joint_32_angles[146] = {
#include "stg13/terrain/rider_trick_city_a_motion_joint_32_angles.inc"
};
static NJS_MKEY_A rider_trick_city_a_motion_joint_33_angles[107] = {
#include "stg13/terrain/rider_trick_city_a_motion_joint_33_angles.inc"
};
static NJS_MKEY_A rider_trick_city_a_motion_joint_37_angles[146] = {
#include "stg13/terrain/rider_trick_city_a_motion_joint_37_angles.inc"
};
static NJS_MKEY_A rider_trick_city_a_motion_joint_38_angles[99] = {
#include "stg13/terrain/rider_trick_city_a_motion_joint_38_angles.inc"
};
static NJS_MKEY_A rider_trick_city_a_motion_joint_39_angles[99] = {
#include "stg13/terrain/rider_trick_city_a_motion_joint_39_angles.inc"
};
static NJS_MKEY_A rider_trick_city_a_motion_joint_41_angles[146] = {
#include "stg13/terrain/rider_trick_city_a_motion_joint_41_angles.inc"
};
static NJS_MKEY_A rider_trick_city_a_motion_joint_44_angles[146] = {
#include "stg13/terrain/rider_trick_city_a_motion_joint_44_angles.inc"
};
static NJS_MKEY_A rider_trick_city_a_motion_joint_51_angles[2] = {
#include "stg13/terrain/rider_trick_city_a_motion_joint_51_angles.inc"
};
static NJS_MKEY_A rider_trick_city_a_motion_joint_53_angles[2] = {
#include "stg13/terrain/rider_trick_city_a_motion_joint_53_angles.inc"
};
static NJS_MKEY_A rider_trick_city_a_motion_joint_56_angles[146] = {
#include "stg13/terrain/rider_trick_city_a_motion_joint_56_angles.inc"
};
static NJS_MKEY_A rider_trick_city_a_motion_joint_57_angles[37] = {
#include "stg13/terrain/rider_trick_city_a_motion_joint_57_angles.inc"
};
static NJS_MKEY_A rider_trick_city_a_motion_joint_58_angles[37] = {
#include "stg13/terrain/rider_trick_city_a_motion_joint_58_angles.inc"
};
static NJS_MKEY_A rider_trick_city_a_motion_joint_60_angles[146] = {
#include "stg13/terrain/rider_trick_city_a_motion_joint_60_angles.inc"
};
static NJS_MDATA2 rider_trick_city_a_motion_tracks[62] = {
#include "stg13/terrain/rider_trick_city_a_motion_tracks.inc"
};
static NJS_MOTION rider_trick_city_a_motion = {
#include "stg13/terrain/rider_trick_city_a_motion.inc"
};
static NJS_MKEY_F rider_lean_positive_motion_joint_2_position[17] = {
#include "stg13/terrain/rider_lean_positive_motion_joint_2_position.inc"
};
static NJS_MKEY_A rider_lean_positive_motion_joint_4_angles[17] = {
#include "stg13/terrain/rider_lean_positive_motion_joint_4_angles.inc"
};
static NJS_MKEY_A rider_lean_positive_motion_joint_7_angles[17] = {
#include "stg13/terrain/rider_lean_positive_motion_joint_7_angles.inc"
};
static NJS_MKEY_A rider_lean_positive_motion_joint_8_angles[17] = {
#include "stg13/terrain/rider_lean_positive_motion_joint_8_angles.inc"
};
static NJS_MKEY_A rider_lean_positive_motion_joint_9_angles[17] = {
#include "stg13/terrain/rider_lean_positive_motion_joint_9_angles.inc"
};
static NJS_MKEY_A rider_lean_positive_motion_joint_13_angles[17] = {
#include "stg13/terrain/rider_lean_positive_motion_joint_13_angles.inc"
};
static NJS_MKEY_A rider_lean_positive_motion_joint_14_angles[8] = {
#include "stg13/terrain/rider_lean_positive_motion_joint_14_angles.inc"
};
static NJS_MKEY_A rider_lean_positive_motion_joint_17_angles[17] = {
#include "stg13/terrain/rider_lean_positive_motion_joint_17_angles.inc"
};
static NJS_MKEY_A rider_lean_positive_motion_joint_18_angles[15] = {
#include "stg13/terrain/rider_lean_positive_motion_joint_18_angles.inc"
};
static NJS_MKEY_A rider_lean_positive_motion_joint_19_angles[17] = {
#include "stg13/terrain/rider_lean_positive_motion_joint_19_angles.inc"
};
static NJS_MKEY_A rider_lean_positive_motion_joint_23_angles[17] = {
#include "stg13/terrain/rider_lean_positive_motion_joint_23_angles.inc"
};
static NJS_MKEY_A rider_lean_positive_motion_joint_24_angles[10] = {
#include "stg13/terrain/rider_lean_positive_motion_joint_24_angles.inc"
};
static NJS_MKEY_A rider_lean_positive_motion_joint_27_angles[17] = {
#include "stg13/terrain/rider_lean_positive_motion_joint_27_angles.inc"
};
static NJS_MKEY_A rider_lean_positive_motion_joint_31_angles[17] = {
#include "stg13/terrain/rider_lean_positive_motion_joint_31_angles.inc"
};
static NJS_MKEY_A rider_lean_positive_motion_joint_32_angles[17] = {
#include "stg13/terrain/rider_lean_positive_motion_joint_32_angles.inc"
};
static NJS_MKEY_A rider_lean_positive_motion_joint_33_angles[12] = {
#include "stg13/terrain/rider_lean_positive_motion_joint_33_angles.inc"
};
static NJS_MKEY_A rider_lean_positive_motion_joint_37_angles[17] = {
#include "stg13/terrain/rider_lean_positive_motion_joint_37_angles.inc"
};
static NJS_MKEY_A rider_lean_positive_motion_joint_38_angles[17] = {
#include "stg13/terrain/rider_lean_positive_motion_joint_38_angles.inc"
};
static NJS_MKEY_A rider_lean_positive_motion_joint_39_angles[17] = {
#include "stg13/terrain/rider_lean_positive_motion_joint_39_angles.inc"
};
static NJS_MKEY_A rider_lean_positive_motion_joint_41_angles[17] = {
#include "stg13/terrain/rider_lean_positive_motion_joint_41_angles.inc"
};
static NJS_MKEY_A rider_lean_positive_motion_joint_44_angles[17] = {
#include "stg13/terrain/rider_lean_positive_motion_joint_44_angles.inc"
};
static NJS_MKEY_A rider_lean_positive_motion_joint_51_angles[2] = {
#include "stg13/terrain/rider_lean_positive_motion_joint_51_angles.inc"
};
static NJS_MKEY_A rider_lean_positive_motion_joint_53_angles[2] = {
#include "stg13/terrain/rider_lean_positive_motion_joint_53_angles.inc"
};
static NJS_MKEY_A rider_lean_positive_motion_joint_56_angles[17] = {
#include "stg13/terrain/rider_lean_positive_motion_joint_56_angles.inc"
};
static NJS_MKEY_A rider_lean_positive_motion_joint_57_angles[17] = {
#include "stg13/terrain/rider_lean_positive_motion_joint_57_angles.inc"
};
static NJS_MKEY_A rider_lean_positive_motion_joint_58_angles[17] = {
#include "stg13/terrain/rider_lean_positive_motion_joint_58_angles.inc"
};
static NJS_MKEY_A rider_lean_positive_motion_joint_60_angles[17] = {
#include "stg13/terrain/rider_lean_positive_motion_joint_60_angles.inc"
};
static NJS_MDATA2 rider_lean_positive_motion_tracks[62] = {
#include "stg13/terrain/rider_lean_positive_motion_tracks.inc"
};
static NJS_MOTION rider_lean_positive_motion = {
#include "stg13/terrain/rider_lean_positive_motion.inc"
};
static NJS_MKEY_F rider_lean_negative_motion_joint_2_position[2] = {
#include "stg13/terrain/rider_lean_negative_motion_joint_2_position.inc"
};
static NJS_MKEY_A rider_lean_negative_motion_joint_4_angles[17] = {
#include "stg13/terrain/rider_lean_negative_motion_joint_4_angles.inc"
};
static NJS_MKEY_A rider_lean_negative_motion_joint_7_angles[17] = {
#include "stg13/terrain/rider_lean_negative_motion_joint_7_angles.inc"
};
static NJS_MKEY_A rider_lean_negative_motion_joint_8_angles[17] = {
#include "stg13/terrain/rider_lean_negative_motion_joint_8_angles.inc"
};
static NJS_MKEY_A rider_lean_negative_motion_joint_9_angles[17] = {
#include "stg13/terrain/rider_lean_negative_motion_joint_9_angles.inc"
};
static NJS_MKEY_A rider_lean_negative_motion_joint_13_angles[17] = {
#include "stg13/terrain/rider_lean_negative_motion_joint_13_angles.inc"
};
static NJS_MKEY_A rider_lean_negative_motion_joint_14_angles[9] = {
#include "stg13/terrain/rider_lean_negative_motion_joint_14_angles.inc"
};
static NJS_MKEY_A rider_lean_negative_motion_joint_17_angles[17] = {
#include "stg13/terrain/rider_lean_negative_motion_joint_17_angles.inc"
};
static NJS_MKEY_A rider_lean_negative_motion_joint_18_angles[17] = {
#include "stg13/terrain/rider_lean_negative_motion_joint_18_angles.inc"
};
static NJS_MKEY_A rider_lean_negative_motion_joint_19_angles[17] = {
#include "stg13/terrain/rider_lean_negative_motion_joint_19_angles.inc"
};
static NJS_MKEY_A rider_lean_negative_motion_joint_23_angles[17] = {
#include "stg13/terrain/rider_lean_negative_motion_joint_23_angles.inc"
};
static NJS_MKEY_A rider_lean_negative_motion_joint_24_angles[12] = {
#include "stg13/terrain/rider_lean_negative_motion_joint_24_angles.inc"
};
static NJS_MKEY_A rider_lean_negative_motion_joint_27_angles[17] = {
#include "stg13/terrain/rider_lean_negative_motion_joint_27_angles.inc"
};
static NJS_MKEY_A rider_lean_negative_motion_joint_28_angles[5] = {
#include "stg13/terrain/rider_lean_negative_motion_joint_28_angles.inc"
};
static NJS_MKEY_A rider_lean_negative_motion_joint_29_angles[5] = {
#include "stg13/terrain/rider_lean_negative_motion_joint_29_angles.inc"
};
static NJS_MKEY_A rider_lean_negative_motion_joint_31_angles[17] = {
#include "stg13/terrain/rider_lean_negative_motion_joint_31_angles.inc"
};
static NJS_MKEY_A rider_lean_negative_motion_joint_32_angles[17] = {
#include "stg13/terrain/rider_lean_negative_motion_joint_32_angles.inc"
};
static NJS_MKEY_A rider_lean_negative_motion_joint_33_angles[13] = {
#include "stg13/terrain/rider_lean_negative_motion_joint_33_angles.inc"
};
static NJS_MKEY_A rider_lean_negative_motion_joint_37_angles[17] = {
#include "stg13/terrain/rider_lean_negative_motion_joint_37_angles.inc"
};
static NJS_MKEY_A rider_lean_negative_motion_joint_38_angles[17] = {
#include "stg13/terrain/rider_lean_negative_motion_joint_38_angles.inc"
};
static NJS_MKEY_A rider_lean_negative_motion_joint_39_angles[17] = {
#include "stg13/terrain/rider_lean_negative_motion_joint_39_angles.inc"
};
static NJS_MKEY_A rider_lean_negative_motion_joint_41_angles[17] = {
#include "stg13/terrain/rider_lean_negative_motion_joint_41_angles.inc"
};
static NJS_MKEY_A rider_lean_negative_motion_joint_44_angles[17] = {
#include "stg13/terrain/rider_lean_negative_motion_joint_44_angles.inc"
};
static NJS_MKEY_A rider_lean_negative_motion_joint_51_angles[2] = {
#include "stg13/terrain/rider_lean_negative_motion_joint_51_angles.inc"
};
static NJS_MKEY_A rider_lean_negative_motion_joint_53_angles[2] = {
#include "stg13/terrain/rider_lean_negative_motion_joint_53_angles.inc"
};
static NJS_MKEY_A rider_lean_negative_motion_joint_56_angles[17] = {
#include "stg13/terrain/rider_lean_negative_motion_joint_56_angles.inc"
};
static NJS_MKEY_A rider_lean_negative_motion_joint_57_angles[17] = {
#include "stg13/terrain/rider_lean_negative_motion_joint_57_angles.inc"
};
static NJS_MKEY_A rider_lean_negative_motion_joint_58_angles[17] = {
#include "stg13/terrain/rider_lean_negative_motion_joint_58_angles.inc"
};
static NJS_MKEY_A rider_lean_negative_motion_joint_60_angles[17] = {
#include "stg13/terrain/rider_lean_negative_motion_joint_60_angles.inc"
};
static NJS_MDATA2 rider_lean_negative_motion_tracks[62] = {
#include "stg13/terrain/rider_lean_negative_motion_tracks.inc"
};
static NJS_MOTION rider_lean_negative_motion = {
#include "stg13/terrain/rider_lean_negative_motion.inc"
};
static NJS_MKEY_F rider_turn_positive_motion_joint_2_position[9] = {
#include "stg13/terrain/rider_turn_positive_motion_joint_2_position.inc"
};
static NJS_MKEY_A rider_turn_positive_motion_joint_4_angles[9] = {
#include "stg13/terrain/rider_turn_positive_motion_joint_4_angles.inc"
};
static NJS_MKEY_A rider_turn_positive_motion_joint_7_angles[9] = {
#include "stg13/terrain/rider_turn_positive_motion_joint_7_angles.inc"
};
static NJS_MKEY_A rider_turn_positive_motion_joint_8_angles[2] = {
#include "stg13/terrain/rider_turn_positive_motion_joint_8_angles.inc"
};
static NJS_MKEY_A rider_turn_positive_motion_joint_9_angles[2] = {
#include "stg13/terrain/rider_turn_positive_motion_joint_9_angles.inc"
};
static NJS_MKEY_A rider_turn_positive_motion_joint_13_angles[5] = {
#include "stg13/terrain/rider_turn_positive_motion_joint_13_angles.inc"
};
static NJS_MKEY_A rider_turn_positive_motion_joint_17_angles[9] = {
#include "stg13/terrain/rider_turn_positive_motion_joint_17_angles.inc"
};
static NJS_MKEY_A rider_turn_positive_motion_joint_18_angles[2] = {
#include "stg13/terrain/rider_turn_positive_motion_joint_18_angles.inc"
};
static NJS_MKEY_A rider_turn_positive_motion_joint_19_angles[8] = {
#include "stg13/terrain/rider_turn_positive_motion_joint_19_angles.inc"
};
static NJS_MKEY_A rider_turn_positive_motion_joint_23_angles[9] = {
#include "stg13/terrain/rider_turn_positive_motion_joint_23_angles.inc"
};
static NJS_MKEY_A rider_turn_positive_motion_joint_27_angles[9] = {
#include "stg13/terrain/rider_turn_positive_motion_joint_27_angles.inc"
};
static NJS_MKEY_A rider_turn_positive_motion_joint_31_angles[9] = {
#include "stg13/terrain/rider_turn_positive_motion_joint_31_angles.inc"
};
static NJS_MKEY_A rider_turn_positive_motion_joint_32_angles[9] = {
#include "stg13/terrain/rider_turn_positive_motion_joint_32_angles.inc"
};
static NJS_MKEY_A rider_turn_positive_motion_joint_33_angles[9] = {
#include "stg13/terrain/rider_turn_positive_motion_joint_33_angles.inc"
};
static NJS_MKEY_A rider_turn_positive_motion_joint_37_angles[9] = {
#include "stg13/terrain/rider_turn_positive_motion_joint_37_angles.inc"
};
static NJS_MKEY_A rider_turn_positive_motion_joint_38_angles[2] = {
#include "stg13/terrain/rider_turn_positive_motion_joint_38_angles.inc"
};
static NJS_MKEY_A rider_turn_positive_motion_joint_39_angles[2] = {
#include "stg13/terrain/rider_turn_positive_motion_joint_39_angles.inc"
};
static NJS_MKEY_A rider_turn_positive_motion_joint_41_angles[9] = {
#include "stg13/terrain/rider_turn_positive_motion_joint_41_angles.inc"
};
static NJS_MKEY_A rider_turn_positive_motion_joint_44_angles[9] = {
#include "stg13/terrain/rider_turn_positive_motion_joint_44_angles.inc"
};
static NJS_MKEY_A rider_turn_positive_motion_joint_51_angles[2] = {
#include "stg13/terrain/rider_turn_positive_motion_joint_51_angles.inc"
};
static NJS_MKEY_A rider_turn_positive_motion_joint_53_angles[2] = {
#include "stg13/terrain/rider_turn_positive_motion_joint_53_angles.inc"
};
static NJS_MKEY_A rider_turn_positive_motion_joint_56_angles[9] = {
#include "stg13/terrain/rider_turn_positive_motion_joint_56_angles.inc"
};
static NJS_MKEY_A rider_turn_positive_motion_joint_57_angles[8] = {
#include "stg13/terrain/rider_turn_positive_motion_joint_57_angles.inc"
};
static NJS_MKEY_A rider_turn_positive_motion_joint_58_angles[8] = {
#include "stg13/terrain/rider_turn_positive_motion_joint_58_angles.inc"
};
static NJS_MKEY_A rider_turn_positive_motion_joint_60_angles[9] = {
#include "stg13/terrain/rider_turn_positive_motion_joint_60_angles.inc"
};
static NJS_MDATA2 rider_turn_positive_motion_tracks[62] = {
#include "stg13/terrain/rider_turn_positive_motion_tracks.inc"
};
static NJS_MOTION rider_turn_positive_motion = {
#include "stg13/terrain/rider_turn_positive_motion.inc"
};
static NJS_MKEY_F rider_turn_negative_motion_joint_2_position[8] = {
#include "stg13/terrain/rider_turn_negative_motion_joint_2_position.inc"
};
static NJS_MKEY_A rider_turn_negative_motion_joint_4_angles[8] = {
#include "stg13/terrain/rider_turn_negative_motion_joint_4_angles.inc"
};
static NJS_MKEY_A rider_turn_negative_motion_joint_7_angles[8] = {
#include "stg13/terrain/rider_turn_negative_motion_joint_7_angles.inc"
};
static NJS_MKEY_A rider_turn_negative_motion_joint_8_angles[8] = {
#include "stg13/terrain/rider_turn_negative_motion_joint_8_angles.inc"
};
static NJS_MKEY_A rider_turn_negative_motion_joint_9_angles[8] = {
#include "stg13/terrain/rider_turn_negative_motion_joint_9_angles.inc"
};
static NJS_MKEY_A rider_turn_negative_motion_joint_13_angles[8] = {
#include "stg13/terrain/rider_turn_negative_motion_joint_13_angles.inc"
};
static NJS_MKEY_A rider_turn_negative_motion_joint_14_angles[6] = {
#include "stg13/terrain/rider_turn_negative_motion_joint_14_angles.inc"
};
static NJS_MKEY_A rider_turn_negative_motion_joint_17_angles[8] = {
#include "stg13/terrain/rider_turn_negative_motion_joint_17_angles.inc"
};
static NJS_MKEY_A rider_turn_negative_motion_joint_18_angles[8] = {
#include "stg13/terrain/rider_turn_negative_motion_joint_18_angles.inc"
};
static NJS_MKEY_A rider_turn_negative_motion_joint_19_angles[8] = {
#include "stg13/terrain/rider_turn_negative_motion_joint_19_angles.inc"
};
static NJS_MKEY_A rider_turn_negative_motion_joint_23_angles[8] = {
#include "stg13/terrain/rider_turn_negative_motion_joint_23_angles.inc"
};
static NJS_MKEY_A rider_turn_negative_motion_joint_27_angles[8] = {
#include "stg13/terrain/rider_turn_negative_motion_joint_27_angles.inc"
};
static NJS_MKEY_A rider_turn_negative_motion_joint_28_angles[8] = {
#include "stg13/terrain/rider_turn_negative_motion_joint_28_angles.inc"
};
static NJS_MKEY_A rider_turn_negative_motion_joint_29_angles[8] = {
#include "stg13/terrain/rider_turn_negative_motion_joint_29_angles.inc"
};
static NJS_MKEY_A rider_turn_negative_motion_joint_31_angles[8] = {
#include "stg13/terrain/rider_turn_negative_motion_joint_31_angles.inc"
};
static NJS_MKEY_A rider_turn_negative_motion_joint_32_angles[8] = {
#include "stg13/terrain/rider_turn_negative_motion_joint_32_angles.inc"
};
static NJS_MKEY_A rider_turn_negative_motion_joint_33_angles[6] = {
#include "stg13/terrain/rider_turn_negative_motion_joint_33_angles.inc"
};
static NJS_MKEY_A rider_turn_negative_motion_joint_37_angles[8] = {
#include "stg13/terrain/rider_turn_negative_motion_joint_37_angles.inc"
};
static NJS_MKEY_A rider_turn_negative_motion_joint_38_angles[8] = {
#include "stg13/terrain/rider_turn_negative_motion_joint_38_angles.inc"
};
static NJS_MKEY_A rider_turn_negative_motion_joint_39_angles[8] = {
#include "stg13/terrain/rider_turn_negative_motion_joint_39_angles.inc"
};
static NJS_MKEY_A rider_turn_negative_motion_joint_41_angles[8] = {
#include "stg13/terrain/rider_turn_negative_motion_joint_41_angles.inc"
};
static NJS_MKEY_A rider_turn_negative_motion_joint_44_angles[8] = {
#include "stg13/terrain/rider_turn_negative_motion_joint_44_angles.inc"
};
static NJS_MKEY_A rider_turn_negative_motion_joint_51_angles[2] = {
#include "stg13/terrain/rider_turn_negative_motion_joint_51_angles.inc"
};
static NJS_MKEY_A rider_turn_negative_motion_joint_53_angles[2] = {
#include "stg13/terrain/rider_turn_negative_motion_joint_53_angles.inc"
};
static NJS_MKEY_A rider_turn_negative_motion_joint_56_angles[8] = {
#include "stg13/terrain/rider_turn_negative_motion_joint_56_angles.inc"
};
static NJS_MKEY_A rider_turn_negative_motion_joint_57_angles[8] = {
#include "stg13/terrain/rider_turn_negative_motion_joint_57_angles.inc"
};
static NJS_MKEY_A rider_turn_negative_motion_joint_58_angles[8] = {
#include "stg13/terrain/rider_turn_negative_motion_joint_58_angles.inc"
};
static NJS_MKEY_A rider_turn_negative_motion_joint_60_angles[8] = {
#include "stg13/terrain/rider_turn_negative_motion_joint_60_angles.inc"
};
static NJS_MDATA2 rider_turn_negative_motion_tracks[62] = {
#include "stg13/terrain/rider_turn_negative_motion_tracks.inc"
};
static NJS_MOTION rider_turn_negative_motion = {
#include "stg13/terrain/rider_turn_negative_motion.inc"
};
static NJS_MKEY_F rider_jump_motion_joint_2_position[8] = {
#include "stg13/terrain/rider_jump_motion_joint_2_position.inc"
};
static NJS_MKEY_A rider_jump_motion_joint_4_angles[2] = {
#include "stg13/terrain/rider_jump_motion_joint_4_angles.inc"
};
static NJS_MKEY_A rider_jump_motion_joint_7_angles[8] = {
#include "stg13/terrain/rider_jump_motion_joint_7_angles.inc"
};
static NJS_MKEY_A rider_jump_motion_joint_8_angles[8] = {
#include "stg13/terrain/rider_jump_motion_joint_8_angles.inc"
};
static NJS_MKEY_A rider_jump_motion_joint_9_angles[8] = {
#include "stg13/terrain/rider_jump_motion_joint_9_angles.inc"
};
static NJS_MKEY_A rider_jump_motion_joint_13_angles[8] = {
#include "stg13/terrain/rider_jump_motion_joint_13_angles.inc"
};
static NJS_MKEY_A rider_jump_motion_joint_14_angles[2] = {
#include "stg13/terrain/rider_jump_motion_joint_14_angles.inc"
};
static NJS_MKEY_A rider_jump_motion_joint_17_angles[8] = {
#include "stg13/terrain/rider_jump_motion_joint_17_angles.inc"
};
static NJS_MKEY_A rider_jump_motion_joint_18_angles[8] = {
#include "stg13/terrain/rider_jump_motion_joint_18_angles.inc"
};
static NJS_MKEY_A rider_jump_motion_joint_19_angles[8] = {
#include "stg13/terrain/rider_jump_motion_joint_19_angles.inc"
};
static NJS_MKEY_A rider_jump_motion_joint_23_angles[8] = {
#include "stg13/terrain/rider_jump_motion_joint_23_angles.inc"
};
static NJS_MKEY_A rider_jump_motion_joint_24_angles[2] = {
#include "stg13/terrain/rider_jump_motion_joint_24_angles.inc"
};
static NJS_MKEY_A rider_jump_motion_joint_27_angles[2] = {
#include "stg13/terrain/rider_jump_motion_joint_27_angles.inc"
};
static NJS_MKEY_A rider_jump_motion_joint_28_angles[2] = {
#include "stg13/terrain/rider_jump_motion_joint_28_angles.inc"
};
static NJS_MKEY_A rider_jump_motion_joint_29_angles[2] = {
#include "stg13/terrain/rider_jump_motion_joint_29_angles.inc"
};
static NJS_MKEY_A rider_jump_motion_joint_31_angles[8] = {
#include "stg13/terrain/rider_jump_motion_joint_31_angles.inc"
};
static NJS_MKEY_A rider_jump_motion_joint_32_angles[8] = {
#include "stg13/terrain/rider_jump_motion_joint_32_angles.inc"
};
static NJS_MKEY_A rider_jump_motion_joint_33_angles[8] = {
#include "stg13/terrain/rider_jump_motion_joint_33_angles.inc"
};
static NJS_MKEY_A rider_jump_motion_joint_37_angles[8] = {
#include "stg13/terrain/rider_jump_motion_joint_37_angles.inc"
};
static NJS_MKEY_A rider_jump_motion_joint_38_angles[8] = {
#include "stg13/terrain/rider_jump_motion_joint_38_angles.inc"
};
static NJS_MKEY_A rider_jump_motion_joint_39_angles[8] = {
#include "stg13/terrain/rider_jump_motion_joint_39_angles.inc"
};
static NJS_MKEY_A rider_jump_motion_joint_41_angles[8] = {
#include "stg13/terrain/rider_jump_motion_joint_41_angles.inc"
};
static NJS_MKEY_A rider_jump_motion_joint_44_angles[8] = {
#include "stg13/terrain/rider_jump_motion_joint_44_angles.inc"
};
static NJS_MKEY_A rider_jump_motion_joint_51_angles[2] = {
#include "stg13/terrain/rider_jump_motion_joint_51_angles.inc"
};
static NJS_MKEY_A rider_jump_motion_joint_53_angles[2] = {
#include "stg13/terrain/rider_jump_motion_joint_53_angles.inc"
};
static NJS_MKEY_A rider_jump_motion_joint_56_angles[8] = {
#include "stg13/terrain/rider_jump_motion_joint_56_angles.inc"
};
static NJS_MKEY_A rider_jump_motion_joint_57_angles[8] = {
#include "stg13/terrain/rider_jump_motion_joint_57_angles.inc"
};
static NJS_MKEY_A rider_jump_motion_joint_58_angles[8] = {
#include "stg13/terrain/rider_jump_motion_joint_58_angles.inc"
};
static NJS_MKEY_A rider_jump_motion_joint_60_angles[8] = {
#include "stg13/terrain/rider_jump_motion_joint_60_angles.inc"
};
static NJS_MDATA2 rider_jump_motion_tracks[62] = {
#include "stg13/terrain/rider_jump_motion_tracks.inc"
};
static NJS_MOTION rider_jump_motion = {
#include "stg13/terrain/rider_jump_motion.inc"
};
static NJS_MKEY_F rider_apex_motion_joint_2_position[12] = {
#include "stg13/terrain/rider_apex_motion_joint_2_position.inc"
};
static NJS_MKEY_A rider_apex_motion_joint_4_angles[12] = {
#include "stg13/terrain/rider_apex_motion_joint_4_angles.inc"
};
static NJS_MKEY_A rider_apex_motion_joint_7_angles[12] = {
#include "stg13/terrain/rider_apex_motion_joint_7_angles.inc"
};
static NJS_MKEY_A rider_apex_motion_joint_8_angles[12] = {
#include "stg13/terrain/rider_apex_motion_joint_8_angles.inc"
};
static NJS_MKEY_A rider_apex_motion_joint_9_angles[12] = {
#include "stg13/terrain/rider_apex_motion_joint_9_angles.inc"
};
static NJS_MKEY_A rider_apex_motion_joint_13_angles[12] = {
#include "stg13/terrain/rider_apex_motion_joint_13_angles.inc"
};
static NJS_MKEY_A rider_apex_motion_joint_14_angles[12] = {
#include "stg13/terrain/rider_apex_motion_joint_14_angles.inc"
};
static NJS_MKEY_A rider_apex_motion_joint_17_angles[12] = {
#include "stg13/terrain/rider_apex_motion_joint_17_angles.inc"
};
static NJS_MKEY_A rider_apex_motion_joint_18_angles[11] = {
#include "stg13/terrain/rider_apex_motion_joint_18_angles.inc"
};
static NJS_MKEY_A rider_apex_motion_joint_19_angles[12] = {
#include "stg13/terrain/rider_apex_motion_joint_19_angles.inc"
};
static NJS_MKEY_A rider_apex_motion_joint_23_angles[12] = {
#include "stg13/terrain/rider_apex_motion_joint_23_angles.inc"
};
static NJS_MKEY_A rider_apex_motion_joint_24_angles[12] = {
#include "stg13/terrain/rider_apex_motion_joint_24_angles.inc"
};
static NJS_MKEY_A rider_apex_motion_joint_27_angles[12] = {
#include "stg13/terrain/rider_apex_motion_joint_27_angles.inc"
};
static NJS_MKEY_A rider_apex_motion_joint_28_angles[12] = {
#include "stg13/terrain/rider_apex_motion_joint_28_angles.inc"
};
static NJS_MKEY_A rider_apex_motion_joint_29_angles[12] = {
#include "stg13/terrain/rider_apex_motion_joint_29_angles.inc"
};
static NJS_MKEY_A rider_apex_motion_joint_31_angles[12] = {
#include "stg13/terrain/rider_apex_motion_joint_31_angles.inc"
};
static NJS_MKEY_A rider_apex_motion_joint_32_angles[12] = {
#include "stg13/terrain/rider_apex_motion_joint_32_angles.inc"
};
static NJS_MKEY_A rider_apex_motion_joint_33_angles[9] = {
#include "stg13/terrain/rider_apex_motion_joint_33_angles.inc"
};
static NJS_MKEY_A rider_apex_motion_joint_37_angles[12] = {
#include "stg13/terrain/rider_apex_motion_joint_37_angles.inc"
};
static NJS_MKEY_A rider_apex_motion_joint_38_angles[12] = {
#include "stg13/terrain/rider_apex_motion_joint_38_angles.inc"
};
static NJS_MKEY_A rider_apex_motion_joint_39_angles[12] = {
#include "stg13/terrain/rider_apex_motion_joint_39_angles.inc"
};
static NJS_MKEY_A rider_apex_motion_joint_41_angles[12] = {
#include "stg13/terrain/rider_apex_motion_joint_41_angles.inc"
};
static NJS_MKEY_A rider_apex_motion_joint_44_angles[12] = {
#include "stg13/terrain/rider_apex_motion_joint_44_angles.inc"
};
static NJS_MKEY_A rider_apex_motion_joint_51_angles[2] = {
#include "stg13/terrain/rider_apex_motion_joint_51_angles.inc"
};
static NJS_MKEY_A rider_apex_motion_joint_53_angles[2] = {
#include "stg13/terrain/rider_apex_motion_joint_53_angles.inc"
};
static NJS_MKEY_A rider_apex_motion_joint_56_angles[12] = {
#include "stg13/terrain/rider_apex_motion_joint_56_angles.inc"
};
static NJS_MKEY_A rider_apex_motion_joint_57_angles[12] = {
#include "stg13/terrain/rider_apex_motion_joint_57_angles.inc"
};
static NJS_MKEY_A rider_apex_motion_joint_58_angles[12] = {
#include "stg13/terrain/rider_apex_motion_joint_58_angles.inc"
};
static NJS_MKEY_A rider_apex_motion_joint_60_angles[12] = {
#include "stg13/terrain/rider_apex_motion_joint_60_angles.inc"
};
static NJS_MDATA2 rider_apex_motion_tracks[62] = {
#include "stg13/terrain/rider_apex_motion_tracks.inc"
};
static NJS_MOTION rider_apex_motion = {
#include "stg13/terrain/rider_apex_motion.inc"
};
static NJS_MKEY_F rider_fall_motion_joint_2_position[8] = {
#include "stg13/terrain/rider_fall_motion_joint_2_position.inc"
};
static NJS_MKEY_A rider_fall_motion_joint_4_angles[8] = {
#include "stg13/terrain/rider_fall_motion_joint_4_angles.inc"
};
static NJS_MKEY_A rider_fall_motion_joint_7_angles[8] = {
#include "stg13/terrain/rider_fall_motion_joint_7_angles.inc"
};
static NJS_MKEY_A rider_fall_motion_joint_8_angles[8] = {
#include "stg13/terrain/rider_fall_motion_joint_8_angles.inc"
};
static NJS_MKEY_A rider_fall_motion_joint_9_angles[8] = {
#include "stg13/terrain/rider_fall_motion_joint_9_angles.inc"
};
static NJS_MKEY_A rider_fall_motion_joint_13_angles[8] = {
#include "stg13/terrain/rider_fall_motion_joint_13_angles.inc"
};
static NJS_MKEY_A rider_fall_motion_joint_14_angles[3] = {
#include "stg13/terrain/rider_fall_motion_joint_14_angles.inc"
};
static NJS_MKEY_A rider_fall_motion_joint_17_angles[8] = {
#include "stg13/terrain/rider_fall_motion_joint_17_angles.inc"
};
static NJS_MKEY_A rider_fall_motion_joint_18_angles[8] = {
#include "stg13/terrain/rider_fall_motion_joint_18_angles.inc"
};
static NJS_MKEY_A rider_fall_motion_joint_19_angles[8] = {
#include "stg13/terrain/rider_fall_motion_joint_19_angles.inc"
};
static NJS_MKEY_A rider_fall_motion_joint_23_angles[8] = {
#include "stg13/terrain/rider_fall_motion_joint_23_angles.inc"
};
static NJS_MKEY_A rider_fall_motion_joint_24_angles[8] = {
#include "stg13/terrain/rider_fall_motion_joint_24_angles.inc"
};
static NJS_MKEY_A rider_fall_motion_joint_27_angles[8] = {
#include "stg13/terrain/rider_fall_motion_joint_27_angles.inc"
};
static NJS_MKEY_A rider_fall_motion_joint_28_angles[8] = {
#include "stg13/terrain/rider_fall_motion_joint_28_angles.inc"
};
static NJS_MKEY_A rider_fall_motion_joint_29_angles[8] = {
#include "stg13/terrain/rider_fall_motion_joint_29_angles.inc"
};
static NJS_MKEY_A rider_fall_motion_joint_31_angles[8] = {
#include "stg13/terrain/rider_fall_motion_joint_31_angles.inc"
};
static NJS_MKEY_A rider_fall_motion_joint_32_angles[8] = {
#include "stg13/terrain/rider_fall_motion_joint_32_angles.inc"
};
static NJS_MKEY_A rider_fall_motion_joint_33_angles[7] = {
#include "stg13/terrain/rider_fall_motion_joint_33_angles.inc"
};
static NJS_MKEY_A rider_fall_motion_joint_37_angles[8] = {
#include "stg13/terrain/rider_fall_motion_joint_37_angles.inc"
};
static NJS_MKEY_A rider_fall_motion_joint_38_angles[8] = {
#include "stg13/terrain/rider_fall_motion_joint_38_angles.inc"
};
static NJS_MKEY_A rider_fall_motion_joint_39_angles[8] = {
#include "stg13/terrain/rider_fall_motion_joint_39_angles.inc"
};
static NJS_MKEY_A rider_fall_motion_joint_41_angles[8] = {
#include "stg13/terrain/rider_fall_motion_joint_41_angles.inc"
};
static NJS_MKEY_A rider_fall_motion_joint_44_angles[8] = {
#include "stg13/terrain/rider_fall_motion_joint_44_angles.inc"
};
static NJS_MKEY_A rider_fall_motion_joint_51_angles[2] = {
#include "stg13/terrain/rider_fall_motion_joint_51_angles.inc"
};
static NJS_MKEY_A rider_fall_motion_joint_53_angles[2] = {
#include "stg13/terrain/rider_fall_motion_joint_53_angles.inc"
};
static NJS_MKEY_A rider_fall_motion_joint_56_angles[8] = {
#include "stg13/terrain/rider_fall_motion_joint_56_angles.inc"
};
static NJS_MKEY_A rider_fall_motion_joint_57_angles[8] = {
#include "stg13/terrain/rider_fall_motion_joint_57_angles.inc"
};
static NJS_MKEY_A rider_fall_motion_joint_58_angles[8] = {
#include "stg13/terrain/rider_fall_motion_joint_58_angles.inc"
};
static NJS_MKEY_A rider_fall_motion_joint_60_angles[8] = {
#include "stg13/terrain/rider_fall_motion_joint_60_angles.inc"
};
static NJS_MDATA2 rider_fall_motion_tracks[62] = {
#include "stg13/terrain/rider_fall_motion_tracks.inc"
};
static NJS_MOTION rider_fall_motion = {
#include "stg13/terrain/rider_fall_motion.inc"
};
static NJS_MKEY_F rider_crash_motion_joint_2_position[20] = {
#include "stg13/terrain/rider_crash_motion_joint_2_position.inc"
};
static NJS_MKEY_A rider_crash_motion_joint_4_angles[20] = {
#include "stg13/terrain/rider_crash_motion_joint_4_angles.inc"
};
static NJS_MKEY_A rider_crash_motion_joint_7_angles[20] = {
#include "stg13/terrain/rider_crash_motion_joint_7_angles.inc"
};
static NJS_MKEY_A rider_crash_motion_joint_8_angles[20] = {
#include "stg13/terrain/rider_crash_motion_joint_8_angles.inc"
};
static NJS_MKEY_A rider_crash_motion_joint_9_angles[20] = {
#include "stg13/terrain/rider_crash_motion_joint_9_angles.inc"
};
static NJS_MKEY_A rider_crash_motion_joint_13_angles[20] = {
#include "stg13/terrain/rider_crash_motion_joint_13_angles.inc"
};
static NJS_MKEY_A rider_crash_motion_joint_14_angles[20] = {
#include "stg13/terrain/rider_crash_motion_joint_14_angles.inc"
};
static NJS_MKEY_A rider_crash_motion_joint_17_angles[20] = {
#include "stg13/terrain/rider_crash_motion_joint_17_angles.inc"
};
static NJS_MKEY_A rider_crash_motion_joint_18_angles[20] = {
#include "stg13/terrain/rider_crash_motion_joint_18_angles.inc"
};
static NJS_MKEY_A rider_crash_motion_joint_19_angles[20] = {
#include "stg13/terrain/rider_crash_motion_joint_19_angles.inc"
};
static NJS_MKEY_A rider_crash_motion_joint_23_angles[20] = {
#include "stg13/terrain/rider_crash_motion_joint_23_angles.inc"
};
static NJS_MKEY_A rider_crash_motion_joint_24_angles[20] = {
#include "stg13/terrain/rider_crash_motion_joint_24_angles.inc"
};
static NJS_MKEY_A rider_crash_motion_joint_27_angles[20] = {
#include "stg13/terrain/rider_crash_motion_joint_27_angles.inc"
};
static NJS_MKEY_A rider_crash_motion_joint_28_angles[14] = {
#include "stg13/terrain/rider_crash_motion_joint_28_angles.inc"
};
static NJS_MKEY_A rider_crash_motion_joint_29_angles[14] = {
#include "stg13/terrain/rider_crash_motion_joint_29_angles.inc"
};
static NJS_MKEY_A rider_crash_motion_joint_31_angles[20] = {
#include "stg13/terrain/rider_crash_motion_joint_31_angles.inc"
};
static NJS_MKEY_A rider_crash_motion_joint_32_angles[20] = {
#include "stg13/terrain/rider_crash_motion_joint_32_angles.inc"
};
static NJS_MKEY_A rider_crash_motion_joint_33_angles[17] = {
#include "stg13/terrain/rider_crash_motion_joint_33_angles.inc"
};
static NJS_MKEY_A rider_crash_motion_joint_37_angles[20] = {
#include "stg13/terrain/rider_crash_motion_joint_37_angles.inc"
};
static NJS_MKEY_A rider_crash_motion_joint_38_angles[20] = {
#include "stg13/terrain/rider_crash_motion_joint_38_angles.inc"
};
static NJS_MKEY_A rider_crash_motion_joint_39_angles[20] = {
#include "stg13/terrain/rider_crash_motion_joint_39_angles.inc"
};
static NJS_MKEY_A rider_crash_motion_joint_41_angles[20] = {
#include "stg13/terrain/rider_crash_motion_joint_41_angles.inc"
};
static NJS_MKEY_A rider_crash_motion_joint_44_angles[20] = {
#include "stg13/terrain/rider_crash_motion_joint_44_angles.inc"
};
static NJS_MKEY_A rider_crash_motion_joint_51_angles[2] = {
#include "stg13/terrain/rider_crash_motion_joint_51_angles.inc"
};
static NJS_MKEY_A rider_crash_motion_joint_53_angles[2] = {
#include "stg13/terrain/rider_crash_motion_joint_53_angles.inc"
};
static NJS_MKEY_A rider_crash_motion_joint_56_angles[20] = {
#include "stg13/terrain/rider_crash_motion_joint_56_angles.inc"
};
static NJS_MKEY_A rider_crash_motion_joint_57_angles[19] = {
#include "stg13/terrain/rider_crash_motion_joint_57_angles.inc"
};
static NJS_MKEY_A rider_crash_motion_joint_58_angles[19] = {
#include "stg13/terrain/rider_crash_motion_joint_58_angles.inc"
};
static NJS_MKEY_A rider_crash_motion_joint_60_angles[20] = {
#include "stg13/terrain/rider_crash_motion_joint_60_angles.inc"
};
static NJS_MDATA2 rider_crash_motion_tracks[62] = {
#include "stg13/terrain/rider_crash_motion_tracks.inc"
};
static NJS_MOTION rider_crash_motion = {
#include "stg13/terrain/rider_crash_motion.inc"
};
static NJS_MKEY_F rider_land_motion_joint_2_position[8] = {
#include "stg13/terrain/rider_land_motion_joint_2_position.inc"
};
static NJS_MKEY_A rider_land_motion_joint_4_angles[2] = {
#include "stg13/terrain/rider_land_motion_joint_4_angles.inc"
};
static NJS_MKEY_A rider_land_motion_joint_7_angles[8] = {
#include "stg13/terrain/rider_land_motion_joint_7_angles.inc"
};
static NJS_MKEY_A rider_land_motion_joint_8_angles[8] = {
#include "stg13/terrain/rider_land_motion_joint_8_angles.inc"
};
static NJS_MKEY_A rider_land_motion_joint_9_angles[8] = {
#include "stg13/terrain/rider_land_motion_joint_9_angles.inc"
};
static NJS_MKEY_A rider_land_motion_joint_13_angles[8] = {
#include "stg13/terrain/rider_land_motion_joint_13_angles.inc"
};
static NJS_MKEY_A rider_land_motion_joint_14_angles[2] = {
#include "stg13/terrain/rider_land_motion_joint_14_angles.inc"
};
static NJS_MKEY_A rider_land_motion_joint_17_angles[8] = {
#include "stg13/terrain/rider_land_motion_joint_17_angles.inc"
};
static NJS_MKEY_A rider_land_motion_joint_18_angles[8] = {
#include "stg13/terrain/rider_land_motion_joint_18_angles.inc"
};
static NJS_MKEY_A rider_land_motion_joint_19_angles[8] = {
#include "stg13/terrain/rider_land_motion_joint_19_angles.inc"
};
static NJS_MKEY_A rider_land_motion_joint_23_angles[8] = {
#include "stg13/terrain/rider_land_motion_joint_23_angles.inc"
};
static NJS_MKEY_A rider_land_motion_joint_24_angles[2] = {
#include "stg13/terrain/rider_land_motion_joint_24_angles.inc"
};
static NJS_MKEY_A rider_land_motion_joint_27_angles[8] = {
#include "stg13/terrain/rider_land_motion_joint_27_angles.inc"
};
static NJS_MKEY_A rider_land_motion_joint_28_angles[8] = {
#include "stg13/terrain/rider_land_motion_joint_28_angles.inc"
};
static NJS_MKEY_A rider_land_motion_joint_29_angles[8] = {
#include "stg13/terrain/rider_land_motion_joint_29_angles.inc"
};
static NJS_MKEY_A rider_land_motion_joint_31_angles[2] = {
#include "stg13/terrain/rider_land_motion_joint_31_angles.inc"
};
static NJS_MKEY_A rider_land_motion_joint_32_angles[6] = {
#include "stg13/terrain/rider_land_motion_joint_32_angles.inc"
};
static NJS_MKEY_A rider_land_motion_joint_33_angles[6] = {
#include "stg13/terrain/rider_land_motion_joint_33_angles.inc"
};
static NJS_MKEY_A rider_land_motion_joint_37_angles[8] = {
#include "stg13/terrain/rider_land_motion_joint_37_angles.inc"
};
static NJS_MKEY_A rider_land_motion_joint_38_angles[8] = {
#include "stg13/terrain/rider_land_motion_joint_38_angles.inc"
};
static NJS_MKEY_A rider_land_motion_joint_39_angles[8] = {
#include "stg13/terrain/rider_land_motion_joint_39_angles.inc"
};
static NJS_MKEY_A rider_land_motion_joint_41_angles[8] = {
#include "stg13/terrain/rider_land_motion_joint_41_angles.inc"
};
static NJS_MKEY_A rider_land_motion_joint_44_angles[8] = {
#include "stg13/terrain/rider_land_motion_joint_44_angles.inc"
};
static NJS_MKEY_A rider_land_motion_joint_51_angles[2] = {
#include "stg13/terrain/rider_land_motion_joint_51_angles.inc"
};
static NJS_MKEY_A rider_land_motion_joint_53_angles[2] = {
#include "stg13/terrain/rider_land_motion_joint_53_angles.inc"
};
static NJS_MKEY_A rider_land_motion_joint_56_angles[8] = {
#include "stg13/terrain/rider_land_motion_joint_56_angles.inc"
};
static NJS_MKEY_A rider_land_motion_joint_57_angles[8] = {
#include "stg13/terrain/rider_land_motion_joint_57_angles.inc"
};
static NJS_MKEY_A rider_land_motion_joint_58_angles[8] = {
#include "stg13/terrain/rider_land_motion_joint_58_angles.inc"
};
static NJS_MKEY_A rider_land_motion_joint_60_angles[8] = {
#include "stg13/terrain/rider_land_motion_joint_60_angles.inc"
};
static NJS_MDATA2 rider_land_motion_tracks[62] = {
#include "stg13/terrain/rider_land_motion_tracks.inc"
};
static NJS_MOTION rider_land_motion = {
#include "stg13/terrain/rider_land_motion.inc"
};
static NJS_MKEY_F board_trick_harbor_a_motion_joint_0_position[140] = {
#include "stg13/terrain/board_trick_harbor_a_motion_joint_0_position.inc"
};
static NJS_MKEY_A board_trick_harbor_a_motion_joint_1_angles[140] = {
#include "stg13/terrain/board_trick_harbor_a_motion_joint_1_angles.inc"
};
static NJS_MDATA2 board_trick_harbor_a_motion_tracks[2] = {
#include "stg13/terrain/board_trick_harbor_a_motion_tracks.inc"
};
static NJS_MOTION board_trick_harbor_a_motion = {
#include "stg13/terrain/board_trick_harbor_a_motion.inc"
};
static NJS_MKEY_F board_trick_harbor_b_motion_joint_0_position[121] = {
#include "stg13/terrain/board_trick_harbor_b_motion_joint_0_position.inc"
};
static NJS_MKEY_A board_trick_harbor_b_motion_joint_0_angles[121] = {
#include "stg13/terrain/board_trick_harbor_b_motion_joint_0_angles.inc"
};
static NJS_MDATA2 board_trick_harbor_b_motion_tracks[2] = {
#include "stg13/terrain/board_trick_harbor_b_motion_tracks.inc"
};
static NJS_MOTION board_trick_harbor_b_motion = {
#include "stg13/terrain/board_trick_harbor_b_motion.inc"
};
static NJS_MKEY_F board_trick_city_a_motion_joint_0_position[146] = {
#include "stg13/terrain/board_trick_city_a_motion_joint_0_position.inc"
};
static NJS_MKEY_A board_trick_city_a_motion_joint_0_angles[38] = {
#include "stg13/terrain/board_trick_city_a_motion_joint_0_angles.inc"
};
static NJS_MKEY_A board_trick_city_a_motion_joint_1_angles[146] = {
#include "stg13/terrain/board_trick_city_a_motion_joint_1_angles.inc"
};
static NJS_MDATA2 board_trick_city_a_motion_tracks[2] = {
#include "stg13/terrain/board_trick_city_a_motion_tracks.inc"
};
static NJS_MOTION board_trick_city_a_motion = {
#include "stg13/terrain/board_trick_city_a_motion.inc"
};
static NJS_MKEY_F board_lean_positive_motion_joint_0_position[17] = {
#include "stg13/terrain/board_lean_positive_motion_joint_0_position.inc"
};
static NJS_MKEY_A board_lean_positive_motion_joint_1_angles[17] = {
#include "stg13/terrain/board_lean_positive_motion_joint_1_angles.inc"
};
static NJS_MDATA2 board_lean_positive_motion_tracks[2] = {
#include "stg13/terrain/board_lean_positive_motion_tracks.inc"
};
static NJS_MOTION board_lean_positive_motion = {
#include "stg13/terrain/board_lean_positive_motion.inc"
};
static NJS_MKEY_F board_lean_negative_motion_joint_0_position[17] = {
#include "stg13/terrain/board_lean_negative_motion_joint_0_position.inc"
};
static NJS_MKEY_F board_lean_negative_motion_joint_1_position[17] = {
#include "stg13/terrain/board_lean_negative_motion_joint_1_position.inc"
};
static NJS_MKEY_A board_lean_negative_motion_joint_1_angles[17] = {
#include "stg13/terrain/board_lean_negative_motion_joint_1_angles.inc"
};
static NJS_MDATA2 board_lean_negative_motion_tracks[2] = {
#include "stg13/terrain/board_lean_negative_motion_tracks.inc"
};
static NJS_MOTION board_lean_negative_motion = {
#include "stg13/terrain/board_lean_negative_motion.inc"
};
static NJS_MKEY_F board_turn_positive_motion_joint_0_position[9] = {
#include "stg13/terrain/board_turn_positive_motion_joint_0_position.inc"
};
static NJS_MKEY_A board_turn_positive_motion_joint_1_angles[3] = {
#include "stg13/terrain/board_turn_positive_motion_joint_1_angles.inc"
};
static NJS_MDATA2 board_turn_positive_motion_tracks[2] = {
#include "stg13/terrain/board_turn_positive_motion_tracks.inc"
};
static NJS_MOTION board_turn_positive_motion = {
#include "stg13/terrain/board_turn_positive_motion.inc"
};
static NJS_MKEY_F board_turn_negative_motion_joint_0_position[8] = {
#include "stg13/terrain/board_turn_negative_motion_joint_0_position.inc"
};
static NJS_MKEY_A board_turn_negative_motion_joint_1_angles[8] = {
#include "stg13/terrain/board_turn_negative_motion_joint_1_angles.inc"
};
static NJS_MDATA2 board_turn_negative_motion_tracks[2] = {
#include "stg13/terrain/board_turn_negative_motion_tracks.inc"
};
static NJS_MOTION board_turn_negative_motion = {
#include "stg13/terrain/board_turn_negative_motion.inc"
};
static NJS_MKEY_F board_jump_motion_joint_0_position[8] = {
#include "stg13/terrain/board_jump_motion_joint_0_position.inc"
};
static NJS_MKEY_A board_jump_motion_joint_1_angles[8] = {
#include "stg13/terrain/board_jump_motion_joint_1_angles.inc"
};
static NJS_MDATA2 board_jump_motion_tracks[2] = {
#include "stg13/terrain/board_jump_motion_tracks.inc"
};
static NJS_MOTION board_jump_motion = {
#include "stg13/terrain/board_jump_motion.inc"
};
static NJS_MKEY_F board_apex_motion_joint_0_position[12] = {
#include "stg13/terrain/board_apex_motion_joint_0_position.inc"
};
static NJS_MKEY_A board_apex_motion_joint_0_angles[11] = {
#include "stg13/terrain/board_apex_motion_joint_0_angles.inc"
};
static NJS_MKEY_A board_apex_motion_joint_1_angles[12] = {
#include "stg13/terrain/board_apex_motion_joint_1_angles.inc"
};
static NJS_MDATA2 board_apex_motion_tracks[2] = {
#include "stg13/terrain/board_apex_motion_tracks.inc"
};
static NJS_MOTION board_apex_motion = {
#include "stg13/terrain/board_apex_motion.inc"
};
static NJS_MKEY_F board_fall_motion_joint_0_position[8] = {
#include "stg13/terrain/board_fall_motion_joint_0_position.inc"
};
static NJS_MKEY_A board_fall_motion_joint_0_angles[8] = {
#include "stg13/terrain/board_fall_motion_joint_0_angles.inc"
};
static NJS_MKEY_A board_fall_motion_joint_1_angles[8] = {
#include "stg13/terrain/board_fall_motion_joint_1_angles.inc"
};
static NJS_MDATA2 board_fall_motion_tracks[2] = {
#include "stg13/terrain/board_fall_motion_tracks.inc"
};
static NJS_MOTION board_fall_motion = {
#include "stg13/terrain/board_fall_motion.inc"
};
static NJS_MKEY_F board_crash_motion_joint_0_position[20] = {
#include "stg13/terrain/board_crash_motion_joint_0_position.inc"
};
static NJS_MKEY_A board_crash_motion_joint_1_angles[20] = {
#include "stg13/terrain/board_crash_motion_joint_1_angles.inc"
};
static NJS_MDATA2 board_crash_motion_tracks[2] = {
#include "stg13/terrain/board_crash_motion_tracks.inc"
};
static NJS_MOTION board_crash_motion = {
#include "stg13/terrain/board_crash_motion.inc"
};
static NJS_MDATA2 board_land_motion_tracks[2] = {
#include "stg13/terrain/board_land_motion_tracks.inc"
};
static NJS_MOTION board_land_motion = {
#include "stg13/terrain/board_land_motion.inc"
};
struct motion_entry module_motions[39] = {
#include "stg13/terrain/module_motions.inc"
};
static struct land_entry entries[2739] = {
#include "stg13/terrain/entries.inc"
};
struct land_table city_escape_land = {
#include "stg13/terrain/city_escape_land.inc"
};
static NJS_TEXNAME textures[124] = {
#include "stg13/terrain/textures.inc"
};
NJS_TEXLIST city_escape_land_texlist = {
#include "stg13/terrain/city_escape_land_texlist.inc"
};
static NJS_MATERIAL collision_0_materials[1] = {
#include "stg13/terrain/collision_0_materials.inc"
};
static Sint16 collision_0_faces[9] = {
#include "stg13/terrain/collision_0_faces.inc"
};
static NJS_MESHSET collision_0_meshsets[1] = {
#include "stg13/terrain/collision_0_meshsets.inc"
};
static NJS_POINT3 collision_0_points[8] = {
#include "stg13/terrain/collision_0_points.inc"
};
static NJS_MODEL collision_0_model = {
#include "stg13/terrain/collision_0_model.inc"
};
static NJS_OBJECT collision_0 = {
#include "stg13/terrain/collision_0.inc"
};
static NJS_MATERIAL collision_1_materials[1] = {
#include "stg13/terrain/collision_1_materials.inc"
};
static Sint16 collision_1_faces[57] = {
#include "stg13/terrain/collision_1_faces.inc"
};
static NJS_MESHSET collision_1_meshsets[1] = {
#include "stg13/terrain/collision_1_meshsets.inc"
};
static NJS_POINT3 collision_1_points[28] = {
#include "stg13/terrain/collision_1_points.inc"
};
static NJS_MODEL collision_1_model = {
#include "stg13/terrain/collision_1_model.inc"
};
static NJS_OBJECT collision_1 = {
#include "stg13/terrain/collision_1.inc"
};
static NJS_MATERIAL collision_2_materials[1] = {
#include "stg13/terrain/collision_2_materials.inc"
};
static Sint16 collision_2_faces[17] = {
#include "stg13/terrain/collision_2_faces.inc"
};
static NJS_MESHSET collision_2_meshsets[1] = {
#include "stg13/terrain/collision_2_meshsets.inc"
};
static NJS_POINT3 collision_2_points[16] = {
#include "stg13/terrain/collision_2_points.inc"
};
static NJS_MODEL collision_2_model = {
#include "stg13/terrain/collision_2_model.inc"
};
static NJS_OBJECT collision_2 = {
#include "stg13/terrain/collision_2.inc"
};
static NJS_MATERIAL collision_3_materials[1] = {
#include "stg13/terrain/collision_3_materials.inc"
};
static Sint16 collision_3_faces[9] = {
#include "stg13/terrain/collision_3_faces.inc"
};
static NJS_MESHSET collision_3_meshsets[1] = {
#include "stg13/terrain/collision_3_meshsets.inc"
};
static NJS_POINT3 collision_3_points[8] = {
#include "stg13/terrain/collision_3_points.inc"
};
static NJS_MODEL collision_3_model = {
#include "stg13/terrain/collision_3_model.inc"
};
static NJS_OBJECT collision_3 = {
#include "stg13/terrain/collision_3.inc"
};
static NJS_MATERIAL collision_4_materials[1] = {
#include "stg13/terrain/collision_4_materials.inc"
};
static Sint16 collision_4_faces[41] = {
#include "stg13/terrain/collision_4_faces.inc"
};
static NJS_MESHSET collision_4_meshsets[1] = {
#include "stg13/terrain/collision_4_meshsets.inc"
};
static NJS_POINT3 collision_4_points[27] = {
#include "stg13/terrain/collision_4_points.inc"
};
static NJS_MODEL collision_4_model = {
#include "stg13/terrain/collision_4_model.inc"
};
static NJS_OBJECT collision_4 = {
#include "stg13/terrain/collision_4.inc"
};
static NJS_MATERIAL collision_5_materials[1] = {
#include "stg13/terrain/collision_5_materials.inc"
};
static Sint16 collision_5_faces[18] = {
#include "stg13/terrain/collision_5_faces.inc"
};
static NJS_MESHSET collision_5_meshsets[1] = {
#include "stg13/terrain/collision_5_meshsets.inc"
};
static NJS_POINT3 collision_5_points[14] = {
#include "stg13/terrain/collision_5_points.inc"
};
static NJS_MODEL collision_5_model = {
#include "stg13/terrain/collision_5_model.inc"
};
static NJS_OBJECT collision_5 = {
#include "stg13/terrain/collision_5.inc"
};
static NJS_MATERIAL collision_6_materials[1] = {
#include "stg13/terrain/collision_6_materials.inc"
};
static Sint16 collision_6_faces[23] = {
#include "stg13/terrain/collision_6_faces.inc"
};
static NJS_MESHSET collision_6_meshsets[1] = {
#include "stg13/terrain/collision_6_meshsets.inc"
};
static NJS_POINT3 collision_6_points[16] = {
#include "stg13/terrain/collision_6_points.inc"
};
static NJS_MODEL collision_6_model = {
#include "stg13/terrain/collision_6_model.inc"
};
static NJS_OBJECT collision_6 = {
#include "stg13/terrain/collision_6.inc"
};
static NJS_MATERIAL collision_7_materials[1] = {
#include "stg13/terrain/collision_7_materials.inc"
};
static Sint16 collision_7_faces[64] = {
#include "stg13/terrain/collision_7_faces.inc"
};
static NJS_MESHSET collision_7_meshsets[1] = {
#include "stg13/terrain/collision_7_meshsets.inc"
};
static NJS_POINT3 collision_7_points[35] = {
#include "stg13/terrain/collision_7_points.inc"
};
static NJS_MODEL collision_7_model = {
#include "stg13/terrain/collision_7_model.inc"
};
static NJS_OBJECT collision_7 = {
#include "stg13/terrain/collision_7.inc"
};
static NJS_MATERIAL collision_8_materials[1] = {
#include "stg13/terrain/collision_8_materials.inc"
};
static Sint16 collision_8_faces[22] = {
#include "stg13/terrain/collision_8_faces.inc"
};
static NJS_MESHSET collision_8_meshsets[1] = {
#include "stg13/terrain/collision_8_meshsets.inc"
};
static NJS_POINT3 collision_8_points[17] = {
#include "stg13/terrain/collision_8_points.inc"
};
static NJS_MODEL collision_8_model = {
#include "stg13/terrain/collision_8_model.inc"
};
static NJS_OBJECT collision_8 = {
#include "stg13/terrain/collision_8.inc"
};
static NJS_MATERIAL collision_9_materials[1] = {
#include "stg13/terrain/collision_9_materials.inc"
};
static Sint16 collision_9_faces[76] = {
#include "stg13/terrain/collision_9_faces.inc"
};
static NJS_MESHSET collision_9_meshsets[1] = {
#include "stg13/terrain/collision_9_meshsets.inc"
};
static NJS_POINT3 collision_9_points[45] = {
#include "stg13/terrain/collision_9_points.inc"
};
static NJS_MODEL collision_9_model = {
#include "stg13/terrain/collision_9_model.inc"
};
static NJS_OBJECT collision_9 = {
#include "stg13/terrain/collision_9.inc"
};
static NJS_MATERIAL collision_10_materials[1] = {
#include "stg13/terrain/collision_10_materials.inc"
};
static Sint16 collision_10_faces[22] = {
#include "stg13/terrain/collision_10_faces.inc"
};
static NJS_MESHSET collision_10_meshsets[1] = {
#include "stg13/terrain/collision_10_meshsets.inc"
};
static NJS_POINT3 collision_10_points[13] = {
#include "stg13/terrain/collision_10_points.inc"
};
static NJS_MODEL collision_10_model = {
#include "stg13/terrain/collision_10_model.inc"
};
static NJS_OBJECT collision_10 = {
#include "stg13/terrain/collision_10.inc"
};
static NJS_MATERIAL collision_11_materials[1] = {
#include "stg13/terrain/collision_11_materials.inc"
};
static Sint16 collision_11_faces[102] = {
#include "stg13/terrain/collision_11_faces.inc"
};
static NJS_MESHSET collision_11_meshsets[1] = {
#include "stg13/terrain/collision_11_meshsets.inc"
};
static NJS_POINT3 collision_11_points[46] = {
#include "stg13/terrain/collision_11_points.inc"
};
static NJS_MODEL collision_11_model = {
#include "stg13/terrain/collision_11_model.inc"
};
static NJS_OBJECT collision_11 = {
#include "stg13/terrain/collision_11.inc"
};
static NJS_MATERIAL collision_12_materials[1] = {
#include "stg13/terrain/collision_12_materials.inc"
};
static Sint16 collision_12_faces[18] = {
#include "stg13/terrain/collision_12_faces.inc"
};
static NJS_MESHSET collision_12_meshsets[1] = {
#include "stg13/terrain/collision_12_meshsets.inc"
};
static NJS_POINT3 collision_12_points[12] = {
#include "stg13/terrain/collision_12_points.inc"
};
static NJS_MODEL collision_12_model = {
#include "stg13/terrain/collision_12_model.inc"
};
static NJS_OBJECT collision_12 = {
#include "stg13/terrain/collision_12.inc"
};
static NJS_MATERIAL collision_13_materials[1] = {
#include "stg13/terrain/collision_13_materials.inc"
};
static Sint16 collision_13_faces[9] = {
#include "stg13/terrain/collision_13_faces.inc"
};
static NJS_MESHSET collision_13_meshsets[1] = {
#include "stg13/terrain/collision_13_meshsets.inc"
};
static NJS_POINT3 collision_13_points[8] = {
#include "stg13/terrain/collision_13_points.inc"
};
static NJS_MODEL collision_13_model = {
#include "stg13/terrain/collision_13_model.inc"
};
static NJS_OBJECT collision_13 = {
#include "stg13/terrain/collision_13.inc"
};
static NJS_MATERIAL collision_14_materials[1] = {
#include "stg13/terrain/collision_14_materials.inc"
};
static Sint16 collision_14_faces[7] = {
#include "stg13/terrain/collision_14_faces.inc"
};
static NJS_MESHSET collision_14_meshsets[1] = {
#include "stg13/terrain/collision_14_meshsets.inc"
};
static NJS_POINT3 collision_14_points[6] = {
#include "stg13/terrain/collision_14_points.inc"
};
static NJS_MODEL collision_14_model = {
#include "stg13/terrain/collision_14_model.inc"
};
static NJS_OBJECT collision_14 = {
#include "stg13/terrain/collision_14.inc"
};
static NJS_MATERIAL collision_15_materials[1] = {
#include "stg13/terrain/collision_15_materials.inc"
};
static Sint16 collision_15_faces[13] = {
#include "stg13/terrain/collision_15_faces.inc"
};
static NJS_MESHSET collision_15_meshsets[1] = {
#include "stg13/terrain/collision_15_meshsets.inc"
};
static NJS_POINT3 collision_15_points[12] = {
#include "stg13/terrain/collision_15_points.inc"
};
static NJS_MODEL collision_15_model = {
#include "stg13/terrain/collision_15_model.inc"
};
static NJS_OBJECT collision_15 = {
#include "stg13/terrain/collision_15.inc"
};
static NJS_MATERIAL collision_16_materials[1] = {
#include "stg13/terrain/collision_16_materials.inc"
};
static Sint16 collision_16_faces[35] = {
#include "stg13/terrain/collision_16_faces.inc"
};
static NJS_MESHSET collision_16_meshsets[1] = {
#include "stg13/terrain/collision_16_meshsets.inc"
};
static NJS_POINT3 collision_16_points[30] = {
#include "stg13/terrain/collision_16_points.inc"
};
static NJS_MODEL collision_16_model = {
#include "stg13/terrain/collision_16_model.inc"
};
static NJS_OBJECT collision_16 = {
#include "stg13/terrain/collision_16.inc"
};
static NJS_MATERIAL collision_17_materials[1] = {
#include "stg13/terrain/collision_17_materials.inc"
};
static Sint16 collision_17_faces[35] = {
#include "stg13/terrain/collision_17_faces.inc"
};
static NJS_MESHSET collision_17_meshsets[1] = {
#include "stg13/terrain/collision_17_meshsets.inc"
};
static NJS_POINT3 collision_17_points[30] = {
#include "stg13/terrain/collision_17_points.inc"
};
static NJS_MODEL collision_17_model = {
#include "stg13/terrain/collision_17_model.inc"
};
static NJS_OBJECT collision_17 = {
#include "stg13/terrain/collision_17.inc"
};
static NJS_MATERIAL collision_18_materials[1] = {
#include "stg13/terrain/collision_18_materials.inc"
};
static Sint16 collision_18_faces[32] = {
#include "stg13/terrain/collision_18_faces.inc"
};
static NJS_MESHSET collision_18_meshsets[1] = {
#include "stg13/terrain/collision_18_meshsets.inc"
};
static NJS_POINT3 collision_18_points[30] = {
#include "stg13/terrain/collision_18_points.inc"
};
static NJS_MODEL collision_18_model = {
#include "stg13/terrain/collision_18_model.inc"
};
static NJS_OBJECT collision_18 = {
#include "stg13/terrain/collision_18.inc"
};
static NJS_MATERIAL collision_20_materials[1] = {
#include "stg13/terrain/collision_20_materials.inc"
};
static Sint16 collision_20_faces[30] = {
#include "stg13/terrain/collision_20_faces.inc"
};
static NJS_MESHSET collision_20_meshsets[1] = {
#include "stg13/terrain/collision_20_meshsets.inc"
};
static NJS_POINT3 collision_20_points[28] = {
#include "stg13/terrain/collision_20_points.inc"
};
static NJS_MODEL collision_20_model = {
#include "stg13/terrain/collision_20_model.inc"
};
static NJS_OBJECT collision_20 = {
#include "stg13/terrain/collision_20.inc"
};
static NJS_MATERIAL collision_21_materials[1] = {
#include "stg13/terrain/collision_21_materials.inc"
};
static Sint16 collision_21_faces[60] = {
#include "stg13/terrain/collision_21_faces.inc"
};
static NJS_MESHSET collision_21_meshsets[1] = {
#include "stg13/terrain/collision_21_meshsets.inc"
};
static NJS_POINT3 collision_21_points[35] = {
#include "stg13/terrain/collision_21_points.inc"
};
static NJS_MODEL collision_21_model = {
#include "stg13/terrain/collision_21_model.inc"
};
static NJS_OBJECT collision_21 = {
#include "stg13/terrain/collision_21.inc"
};
static NJS_MATERIAL collision_22_materials[1] = {
#include "stg13/terrain/collision_22_materials.inc"
};
static Sint16 collision_22_faces[44] = {
#include "stg13/terrain/collision_22_faces.inc"
};
static NJS_MESHSET collision_22_meshsets[1] = {
#include "stg13/terrain/collision_22_meshsets.inc"
};
static NJS_POINT3 collision_22_points[36] = {
#include "stg13/terrain/collision_22_points.inc"
};
static NJS_MODEL collision_22_model = {
#include "stg13/terrain/collision_22_model.inc"
};
static NJS_OBJECT collision_22 = {
#include "stg13/terrain/collision_22.inc"
};
static NJS_MATERIAL collision_24_materials[1] = {
#include "stg13/terrain/collision_24_materials.inc"
};
static Sint16 collision_24_faces[85] = {
#include "stg13/terrain/collision_24_faces.inc"
};
static NJS_MESHSET collision_24_meshsets[1] = {
#include "stg13/terrain/collision_24_meshsets.inc"
};
static NJS_POINT3 collision_24_points[45] = {
#include "stg13/terrain/collision_24_points.inc"
};
static NJS_MODEL collision_24_model = {
#include "stg13/terrain/collision_24_model.inc"
};
static NJS_OBJECT collision_24 = {
#include "stg13/terrain/collision_24.inc"
};
static NJS_MATERIAL collision_28_materials[1] = {
#include "stg13/terrain/collision_28_materials.inc"
};
static Sint16 collision_28_faces[44] = {
#include "stg13/terrain/collision_28_faces.inc"
};
static NJS_MESHSET collision_28_meshsets[1] = {
#include "stg13/terrain/collision_28_meshsets.inc"
};
static NJS_POINT3 collision_28_points[36] = {
#include "stg13/terrain/collision_28_points.inc"
};
static NJS_MODEL collision_28_model = {
#include "stg13/terrain/collision_28_model.inc"
};
static NJS_OBJECT collision_28 = {
#include "stg13/terrain/collision_28.inc"
};
static NJS_MATERIAL collision_29_materials[1] = {
#include "stg13/terrain/collision_29_materials.inc"
};
static Sint16 collision_29_faces[66] = {
#include "stg13/terrain/collision_29_faces.inc"
};
static NJS_MESHSET collision_29_meshsets[1] = {
#include "stg13/terrain/collision_29_meshsets.inc"
};
static NJS_POINT3 collision_29_points[36] = {
#include "stg13/terrain/collision_29_points.inc"
};
static NJS_MODEL collision_29_model = {
#include "stg13/terrain/collision_29_model.inc"
};
static NJS_OBJECT collision_29 = {
#include "stg13/terrain/collision_29.inc"
};
static NJS_MATERIAL collision_30_materials[1] = {
#include "stg13/terrain/collision_30_materials.inc"
};
static Sint16 collision_30_faces[85] = {
#include "stg13/terrain/collision_30_faces.inc"
};
static NJS_MESHSET collision_30_meshsets[1] = {
#include "stg13/terrain/collision_30_meshsets.inc"
};
static NJS_POINT3 collision_30_points[45] = {
#include "stg13/terrain/collision_30_points.inc"
};
static NJS_MODEL collision_30_model = {
#include "stg13/terrain/collision_30_model.inc"
};
static NJS_OBJECT collision_30 = {
#include "stg13/terrain/collision_30.inc"
};
static NJS_MATERIAL collision_32_materials[1] = {
#include "stg13/terrain/collision_32_materials.inc"
};
static Sint16 collision_32_faces[38] = {
#include "stg13/terrain/collision_32_faces.inc"
};
static NJS_MESHSET collision_32_meshsets[1] = {
#include "stg13/terrain/collision_32_meshsets.inc"
};
static NJS_POINT3 collision_32_points[36] = {
#include "stg13/terrain/collision_32_points.inc"
};
static NJS_MODEL collision_32_model = {
#include "stg13/terrain/collision_32_model.inc"
};
static NJS_OBJECT collision_32 = {
#include "stg13/terrain/collision_32.inc"
};
static NJS_MATERIAL collision_33_materials[1] = {
#include "stg13/terrain/collision_33_materials.inc"
};
static Sint16 collision_33_faces[30] = {
#include "stg13/terrain/collision_33_faces.inc"
};
static NJS_MESHSET collision_33_meshsets[1] = {
#include "stg13/terrain/collision_33_meshsets.inc"
};
static NJS_POINT3 collision_33_points[28] = {
#include "stg13/terrain/collision_33_points.inc"
};
static NJS_MODEL collision_33_model = {
#include "stg13/terrain/collision_33_model.inc"
};
static NJS_OBJECT collision_33 = {
#include "stg13/terrain/collision_33.inc"
};
static NJS_MATERIAL collision_34_materials[1] = {
#include "stg13/terrain/collision_34_materials.inc"
};
static Sint16 collision_34_faces[60] = {
#include "stg13/terrain/collision_34_faces.inc"
};
static NJS_MESHSET collision_34_meshsets[1] = {
#include "stg13/terrain/collision_34_meshsets.inc"
};
static NJS_POINT3 collision_34_points[41] = {
#include "stg13/terrain/collision_34_points.inc"
};
static NJS_MODEL collision_34_model = {
#include "stg13/terrain/collision_34_model.inc"
};
static NJS_OBJECT collision_34 = {
#include "stg13/terrain/collision_34.inc"
};
static NJS_MATERIAL collision_35_materials[1] = {
#include "stg13/terrain/collision_35_materials.inc"
};
static Sint16 collision_35_faces[60] = {
#include "stg13/terrain/collision_35_faces.inc"
};
static NJS_MESHSET collision_35_meshsets[1] = {
#include "stg13/terrain/collision_35_meshsets.inc"
};
static NJS_POINT3 collision_35_points[35] = {
#include "stg13/terrain/collision_35_points.inc"
};
static NJS_MODEL collision_35_model = {
#include "stg13/terrain/collision_35_model.inc"
};
static NJS_OBJECT collision_35 = {
#include "stg13/terrain/collision_35.inc"
};
static NJS_MATERIAL collision_36_materials[1] = {
#include "stg13/terrain/collision_36_materials.inc"
};
static Sint16 collision_36_faces[48] = {
#include "stg13/terrain/collision_36_faces.inc"
};
static NJS_MESHSET collision_36_meshsets[1] = {
#include "stg13/terrain/collision_36_meshsets.inc"
};
static NJS_POINT3 collision_36_points[40] = {
#include "stg13/terrain/collision_36_points.inc"
};
static NJS_MODEL collision_36_model = {
#include "stg13/terrain/collision_36_model.inc"
};
static NJS_OBJECT collision_36 = {
#include "stg13/terrain/collision_36.inc"
};
static NJS_MATERIAL collision_37_materials[1] = {
#include "stg13/terrain/collision_37_materials.inc"
};
static Sint16 collision_37_faces[48] = {
#include "stg13/terrain/collision_37_faces.inc"
};
static NJS_MESHSET collision_37_meshsets[1] = {
#include "stg13/terrain/collision_37_meshsets.inc"
};
static NJS_POINT3 collision_37_points[40] = {
#include "stg13/terrain/collision_37_points.inc"
};
static NJS_MODEL collision_37_model = {
#include "stg13/terrain/collision_37_model.inc"
};
static NJS_OBJECT collision_37 = {
#include "stg13/terrain/collision_37.inc"
};
static NJS_MATERIAL collision_38_materials[1] = {
#include "stg13/terrain/collision_38_materials.inc"
};
static Sint16 collision_38_faces[48] = {
#include "stg13/terrain/collision_38_faces.inc"
};
static NJS_MESHSET collision_38_meshsets[1] = {
#include "stg13/terrain/collision_38_meshsets.inc"
};
static NJS_POINT3 collision_38_points[40] = {
#include "stg13/terrain/collision_38_points.inc"
};
static NJS_MODEL collision_38_model = {
#include "stg13/terrain/collision_38_model.inc"
};
static NJS_OBJECT collision_38 = {
#include "stg13/terrain/collision_38.inc"
};
static NJS_MATERIAL collision_39_materials[1] = {
#include "stg13/terrain/collision_39_materials.inc"
};
static Sint16 collision_39_faces[38] = {
#include "stg13/terrain/collision_39_faces.inc"
};
static NJS_MESHSET collision_39_meshsets[1] = {
#include "stg13/terrain/collision_39_meshsets.inc"
};
static NJS_POINT3 collision_39_points[36] = {
#include "stg13/terrain/collision_39_points.inc"
};
static NJS_MODEL collision_39_model = {
#include "stg13/terrain/collision_39_model.inc"
};
static NJS_OBJECT collision_39 = {
#include "stg13/terrain/collision_39.inc"
};
static NJS_MATERIAL collision_40_materials[1] = {
#include "stg13/terrain/collision_40_materials.inc"
};
static Sint16 collision_40_faces[48] = {
#include "stg13/terrain/collision_40_faces.inc"
};
static NJS_MESHSET collision_40_meshsets[1] = {
#include "stg13/terrain/collision_40_meshsets.inc"
};
static NJS_POINT3 collision_40_points[40] = {
#include "stg13/terrain/collision_40_points.inc"
};
static NJS_MODEL collision_40_model = {
#include "stg13/terrain/collision_40_model.inc"
};
static NJS_OBJECT collision_40 = {
#include "stg13/terrain/collision_40.inc"
};
static NJS_MATERIAL collision_41_materials[1] = {
#include "stg13/terrain/collision_41_materials.inc"
};
static Sint16 collision_41_faces[38] = {
#include "stg13/terrain/collision_41_faces.inc"
};
static NJS_MESHSET collision_41_meshsets[1] = {
#include "stg13/terrain/collision_41_meshsets.inc"
};
static NJS_POINT3 collision_41_points[36] = {
#include "stg13/terrain/collision_41_points.inc"
};
static NJS_MODEL collision_41_model = {
#include "stg13/terrain/collision_41_model.inc"
};
static NJS_OBJECT collision_41 = {
#include "stg13/terrain/collision_41.inc"
};
static NJS_MATERIAL collision_42_materials[1] = {
#include "stg13/terrain/collision_42_materials.inc"
};
static Sint16 collision_42_faces[33] = {
#include "stg13/terrain/collision_42_faces.inc"
};
static NJS_MESHSET collision_42_meshsets[1] = {
#include "stg13/terrain/collision_42_meshsets.inc"
};
static NJS_POINT3 collision_42_points[21] = {
#include "stg13/terrain/collision_42_points.inc"
};
static NJS_MODEL collision_42_model = {
#include "stg13/terrain/collision_42_model.inc"
};
static NJS_OBJECT collision_42 = {
#include "stg13/terrain/collision_42.inc"
};
static NJS_MATERIAL collision_43_materials[1] = {
#include "stg13/terrain/collision_43_materials.inc"
};
static Sint16 collision_43_faces[44] = {
#include "stg13/terrain/collision_43_faces.inc"
};
static NJS_MESHSET collision_43_meshsets[1] = {
#include "stg13/terrain/collision_43_meshsets.inc"
};
static NJS_POINT3 collision_43_points[36] = {
#include "stg13/terrain/collision_43_points.inc"
};
static NJS_MODEL collision_43_model = {
#include "stg13/terrain/collision_43_model.inc"
};
static NJS_OBJECT collision_43 = {
#include "stg13/terrain/collision_43.inc"
};
static NJS_MATERIAL collision_45_materials[1] = {
#include "stg13/terrain/collision_45_materials.inc"
};
static Sint16 collision_45_faces[42] = {
#include "stg13/terrain/collision_45_faces.inc"
};
static NJS_MESHSET collision_45_meshsets[1] = {
#include "stg13/terrain/collision_45_meshsets.inc"
};
static NJS_POINT3 collision_45_points[40] = {
#include "stg13/terrain/collision_45_points.inc"
};
static NJS_MODEL collision_45_model = {
#include "stg13/terrain/collision_45_model.inc"
};
static NJS_OBJECT collision_45 = {
#include "stg13/terrain/collision_45.inc"
};
static NJS_MATERIAL collision_47_materials[1] = {
#include "stg13/terrain/collision_47_materials.inc"
};
static Sint16 collision_47_faces[44] = {
#include "stg13/terrain/collision_47_faces.inc"
};
static NJS_MESHSET collision_47_meshsets[1] = {
#include "stg13/terrain/collision_47_meshsets.inc"
};
static NJS_POINT3 collision_47_points[36] = {
#include "stg13/terrain/collision_47_points.inc"
};
static NJS_MODEL collision_47_model = {
#include "stg13/terrain/collision_47_model.inc"
};
static NJS_OBJECT collision_47 = {
#include "stg13/terrain/collision_47.inc"
};
static NJS_MATERIAL collision_48_materials[1] = {
#include "stg13/terrain/collision_48_materials.inc"
};
static Sint16 collision_48_faces[42] = {
#include "stg13/terrain/collision_48_faces.inc"
};
static NJS_MESHSET collision_48_meshsets[1] = {
#include "stg13/terrain/collision_48_meshsets.inc"
};
static NJS_POINT3 collision_48_points[40] = {
#include "stg13/terrain/collision_48_points.inc"
};
static NJS_MODEL collision_48_model = {
#include "stg13/terrain/collision_48_model.inc"
};
static NJS_OBJECT collision_48 = {
#include "stg13/terrain/collision_48.inc"
};
static NJS_MATERIAL collision_49_materials[1] = {
#include "stg13/terrain/collision_49_materials.inc"
};
static Sint16 collision_49_faces[66] = {
#include "stg13/terrain/collision_49_faces.inc"
};
static NJS_MESHSET collision_49_meshsets[1] = {
#include "stg13/terrain/collision_49_meshsets.inc"
};
static NJS_POINT3 collision_49_points[36] = {
#include "stg13/terrain/collision_49_points.inc"
};
static NJS_MODEL collision_49_model = {
#include "stg13/terrain/collision_49_model.inc"
};
static NJS_OBJECT collision_49 = {
#include "stg13/terrain/collision_49.inc"
};
static NJS_MATERIAL collision_50_materials[1] = {
#include "stg13/terrain/collision_50_materials.inc"
};
static Sint16 collision_50_faces[88] = {
#include "stg13/terrain/collision_50_faces.inc"
};
static NJS_MESHSET collision_50_meshsets[1] = {
#include "stg13/terrain/collision_50_meshsets.inc"
};
static NJS_POINT3 collision_50_points[45] = {
#include "stg13/terrain/collision_50_points.inc"
};
static NJS_MODEL collision_50_model = {
#include "stg13/terrain/collision_50_model.inc"
};
static NJS_OBJECT collision_50 = {
#include "stg13/terrain/collision_50.inc"
};
static NJS_MATERIAL collision_52_materials[1] = {
#include "stg13/terrain/collision_52_materials.inc"
};
static Sint16 collision_52_faces[63] = {
#include "stg13/terrain/collision_52_faces.inc"
};
static NJS_MESHSET collision_52_meshsets[1] = {
#include "stg13/terrain/collision_52_meshsets.inc"
};
static NJS_POINT3 collision_52_points[40] = {
#include "stg13/terrain/collision_52_points.inc"
};
static NJS_MODEL collision_52_model = {
#include "stg13/terrain/collision_52_model.inc"
};
static NJS_OBJECT collision_52 = {
#include "stg13/terrain/collision_52.inc"
};
static NJS_MATERIAL collision_53_materials[1] = {
#include "stg13/terrain/collision_53_materials.inc"
};
static Sint16 collision_53_faces[84] = {
#include "stg13/terrain/collision_53_faces.inc"
};
static NJS_MESHSET collision_53_meshsets[1] = {
#include "stg13/terrain/collision_53_meshsets.inc"
};
static NJS_POINT3 collision_53_points[50] = {
#include "stg13/terrain/collision_53_points.inc"
};
static NJS_MODEL collision_53_model = {
#include "stg13/terrain/collision_53_model.inc"
};
static NJS_OBJECT collision_53 = {
#include "stg13/terrain/collision_53.inc"
};
static NJS_MATERIAL collision_55_materials[1] = {
#include "stg13/terrain/collision_55_materials.inc"
};
static Sint16 collision_55_faces[31] = {
#include "stg13/terrain/collision_55_faces.inc"
};
static NJS_MESHSET collision_55_meshsets[1] = {
#include "stg13/terrain/collision_55_meshsets.inc"
};
static NJS_POINT3 collision_55_points[28] = {
#include "stg13/terrain/collision_55_points.inc"
};
static NJS_MODEL collision_55_model = {
#include "stg13/terrain/collision_55_model.inc"
};
static NJS_OBJECT collision_55 = {
#include "stg13/terrain/collision_55.inc"
};
static NJS_MATERIAL collision_56_materials[1] = {
#include "stg13/terrain/collision_56_materials.inc"
};
static Sint16 collision_56_faces[44] = {
#include "stg13/terrain/collision_56_faces.inc"
};
static NJS_MESHSET collision_56_meshsets[1] = {
#include "stg13/terrain/collision_56_meshsets.inc"
};
static NJS_POINT3 collision_56_points[36] = {
#include "stg13/terrain/collision_56_points.inc"
};
static NJS_MODEL collision_56_model = {
#include "stg13/terrain/collision_56_model.inc"
};
static NJS_OBJECT collision_56 = {
#include "stg13/terrain/collision_56.inc"
};
static NJS_MATERIAL collision_57_materials[1] = {
#include "stg13/terrain/collision_57_materials.inc"
};
static Sint16 collision_57_faces[66] = {
#include "stg13/terrain/collision_57_faces.inc"
};
static NJS_MESHSET collision_57_meshsets[1] = {
#include "stg13/terrain/collision_57_meshsets.inc"
};
static NJS_POINT3 collision_57_points[36] = {
#include "stg13/terrain/collision_57_points.inc"
};
static NJS_MODEL collision_57_model = {
#include "stg13/terrain/collision_57_model.inc"
};
static NJS_OBJECT collision_57 = {
#include "stg13/terrain/collision_57.inc"
};
static NJS_MATERIAL collision_58_materials[1] = {
#include "stg13/terrain/collision_58_materials.inc"
};
static Sint16 collision_58_faces[85] = {
#include "stg13/terrain/collision_58_faces.inc"
};
static NJS_MESHSET collision_58_meshsets[1] = {
#include "stg13/terrain/collision_58_meshsets.inc"
};
static NJS_POINT3 collision_58_points[45] = {
#include "stg13/terrain/collision_58_points.inc"
};
static NJS_MODEL collision_58_model = {
#include "stg13/terrain/collision_58_model.inc"
};
static NJS_OBJECT collision_58 = {
#include "stg13/terrain/collision_58.inc"
};
static NJS_MATERIAL collision_60_materials[1] = {
#include "stg13/terrain/collision_60_materials.inc"
};
static Sint16 collision_60_faces[92] = {
#include "stg13/terrain/collision_60_faces.inc"
};
static NJS_MESHSET collision_60_meshsets[1] = {
#include "stg13/terrain/collision_60_meshsets.inc"
};
static NJS_POINT3 collision_60_points[45] = {
#include "stg13/terrain/collision_60_points.inc"
};
static NJS_MODEL collision_60_model = {
#include "stg13/terrain/collision_60_model.inc"
};
static NJS_OBJECT collision_60 = {
#include "stg13/terrain/collision_60.inc"
};
static NJS_MATERIAL collision_62_materials[1] = {
#include "stg13/terrain/collision_62_materials.inc"
};
static Sint16 collision_62_faces[10] = {
#include "stg13/terrain/collision_62_faces.inc"
};
static NJS_MESHSET collision_62_meshsets[1] = {
#include "stg13/terrain/collision_62_meshsets.inc"
};
static NJS_POINT3 collision_62_points[6] = {
#include "stg13/terrain/collision_62_points.inc"
};
static NJS_MODEL collision_62_model = {
#include "stg13/terrain/collision_62_model.inc"
};
static NJS_OBJECT collision_62 = {
#include "stg13/terrain/collision_62.inc"
};
static NJS_MATERIAL collision_63_materials[1] = {
#include "stg13/terrain/collision_63_materials.inc"
};
static Sint16 collision_63_faces[5] = {
#include "stg13/terrain/collision_63_faces.inc"
};
static NJS_MESHSET collision_63_meshsets[1] = {
#include "stg13/terrain/collision_63_meshsets.inc"
};
static NJS_POINT3 collision_63_points[4] = {
#include "stg13/terrain/collision_63_points.inc"
};
static NJS_MODEL collision_63_model = {
#include "stg13/terrain/collision_63_model.inc"
};
static NJS_OBJECT collision_63 = {
#include "stg13/terrain/collision_63.inc"
};
static NJS_MATERIAL collision_66_materials[1] = {
#include "stg13/terrain/collision_66_materials.inc"
};
static Sint16 collision_66_faces[10] = {
#include "stg13/terrain/collision_66_faces.inc"
};
static NJS_MESHSET collision_66_meshsets[1] = {
#include "stg13/terrain/collision_66_meshsets.inc"
};
static NJS_POINT3 collision_66_points[6] = {
#include "stg13/terrain/collision_66_points.inc"
};
static NJS_MODEL collision_66_model = {
#include "stg13/terrain/collision_66_model.inc"
};
static NJS_OBJECT collision_66 = {
#include "stg13/terrain/collision_66.inc"
};
static NJS_MATERIAL collision_69_materials[1] = {
#include "stg13/terrain/collision_69_materials.inc"
};
static Sint16 collision_69_faces[5] = {
#include "stg13/terrain/collision_69_faces.inc"
};
static NJS_MESHSET collision_69_meshsets[1] = {
#include "stg13/terrain/collision_69_meshsets.inc"
};
static NJS_POINT3 collision_69_points[4] = {
#include "stg13/terrain/collision_69_points.inc"
};
static NJS_MODEL collision_69_model = {
#include "stg13/terrain/collision_69_model.inc"
};
static NJS_OBJECT collision_69 = {
#include "stg13/terrain/collision_69.inc"
};
static NJS_MATERIAL collision_70_materials[1] = {
#include "stg13/terrain/collision_70_materials.inc"
};
static Sint16 collision_70_faces[10] = {
#include "stg13/terrain/collision_70_faces.inc"
};
static NJS_MESHSET collision_70_meshsets[1] = {
#include "stg13/terrain/collision_70_meshsets.inc"
};
static NJS_POINT3 collision_70_points[6] = {
#include "stg13/terrain/collision_70_points.inc"
};
static NJS_MODEL collision_70_model = {
#include "stg13/terrain/collision_70_model.inc"
};
static NJS_OBJECT collision_70 = {
#include "stg13/terrain/collision_70.inc"
};
static NJS_MATERIAL collision_71_materials[1] = {
#include "stg13/terrain/collision_71_materials.inc"
};
static Sint16 collision_71_faces[57] = {
#include "stg13/terrain/collision_71_faces.inc"
};
static NJS_MESHSET collision_71_meshsets[1] = {
#include "stg13/terrain/collision_71_meshsets.inc"
};
static NJS_POINT3 collision_71_points[35] = {
#include "stg13/terrain/collision_71_points.inc"
};
static NJS_MODEL collision_71_model = {
#include "stg13/terrain/collision_71_model.inc"
};
static NJS_OBJECT collision_71 = {
#include "stg13/terrain/collision_71.inc"
};
static NJS_MATERIAL collision_72_materials[1] = {
#include "stg13/terrain/collision_72_materials.inc"
};
static Sint16 collision_72_faces[76] = {
#include "stg13/terrain/collision_72_faces.inc"
};
static NJS_MESHSET collision_72_meshsets[1] = {
#include "stg13/terrain/collision_72_meshsets.inc"
};
static NJS_POINT3 collision_72_points[54] = {
#include "stg13/terrain/collision_72_points.inc"
};
static NJS_MODEL collision_72_model = {
#include "stg13/terrain/collision_72_model.inc"
};
static NJS_OBJECT collision_72 = {
#include "stg13/terrain/collision_72.inc"
};
static NJS_MATERIAL collision_73_materials[1] = {
#include "stg13/terrain/collision_73_materials.inc"
};
static Sint16 collision_73_faces[76] = {
#include "stg13/terrain/collision_73_faces.inc"
};
static NJS_MESHSET collision_73_meshsets[1] = {
#include "stg13/terrain/collision_73_meshsets.inc"
};
static NJS_POINT3 collision_73_points[54] = {
#include "stg13/terrain/collision_73_points.inc"
};
static NJS_MODEL collision_73_model = {
#include "stg13/terrain/collision_73_model.inc"
};
static NJS_OBJECT collision_73 = {
#include "stg13/terrain/collision_73.inc"
};
static NJS_MATERIAL collision_74_materials[1] = {
#include "stg13/terrain/collision_74_materials.inc"
};
static Sint16 collision_74_faces[60] = {
#include "stg13/terrain/collision_74_faces.inc"
};
static NJS_MESHSET collision_74_meshsets[1] = {
#include "stg13/terrain/collision_74_meshsets.inc"
};
static NJS_POINT3 collision_74_points[42] = {
#include "stg13/terrain/collision_74_points.inc"
};
static NJS_MODEL collision_74_model = {
#include "stg13/terrain/collision_74_model.inc"
};
static NJS_OBJECT collision_74 = {
#include "stg13/terrain/collision_74.inc"
};
static NJS_MATERIAL collision_75_materials[1] = {
#include "stg13/terrain/collision_75_materials.inc"
};
static Sint16 collision_75_faces[60] = {
#include "stg13/terrain/collision_75_faces.inc"
};
static NJS_MESHSET collision_75_meshsets[1] = {
#include "stg13/terrain/collision_75_meshsets.inc"
};
static NJS_POINT3 collision_75_points[42] = {
#include "stg13/terrain/collision_75_points.inc"
};
static NJS_MODEL collision_75_model = {
#include "stg13/terrain/collision_75_model.inc"
};
static NJS_OBJECT collision_75 = {
#include "stg13/terrain/collision_75.inc"
};
static NJS_MATERIAL collision_76_materials[1] = {
#include "stg13/terrain/collision_76_materials.inc"
};
static Sint16 collision_76_faces[32] = {
#include "stg13/terrain/collision_76_faces.inc"
};
static NJS_MESHSET collision_76_meshsets[1] = {
#include "stg13/terrain/collision_76_meshsets.inc"
};
static NJS_POINT3 collision_76_points[30] = {
#include "stg13/terrain/collision_76_points.inc"
};
static NJS_MODEL collision_76_model = {
#include "stg13/terrain/collision_76_model.inc"
};
static NJS_OBJECT collision_76 = {
#include "stg13/terrain/collision_76.inc"
};
static NJS_MATERIAL collision_78_materials[1] = {
#include "stg13/terrain/collision_78_materials.inc"
};
static Sint16 collision_78_faces[17] = {
#include "stg13/terrain/collision_78_faces.inc"
};
static NJS_MESHSET collision_78_meshsets[1] = {
#include "stg13/terrain/collision_78_meshsets.inc"
};
static NJS_POINT3 collision_78_points[12] = {
#include "stg13/terrain/collision_78_points.inc"
};
static NJS_MODEL collision_78_model = {
#include "stg13/terrain/collision_78_model.inc"
};
static NJS_OBJECT collision_78 = {
#include "stg13/terrain/collision_78.inc"
};
static NJS_MATERIAL collision_79_materials[1] = {
#include "stg13/terrain/collision_79_materials.inc"
};
static Sint16 collision_79_faces[84] = {
#include "stg13/terrain/collision_79_faces.inc"
};
static NJS_MESHSET collision_79_meshsets[1] = {
#include "stg13/terrain/collision_79_meshsets.inc"
};
static NJS_POINT3 collision_79_points[60] = {
#include "stg13/terrain/collision_79_points.inc"
};
static NJS_MODEL collision_79_model = {
#include "stg13/terrain/collision_79_model.inc"
};
static NJS_OBJECT collision_79 = {
#include "stg13/terrain/collision_79.inc"
};
static NJS_MATERIAL collision_80_materials[1] = {
#include "stg13/terrain/collision_80_materials.inc"
};
static Sint16 collision_80_faces[10] = {
#include "stg13/terrain/collision_80_faces.inc"
};
static NJS_MESHSET collision_80_meshsets[1] = {
#include "stg13/terrain/collision_80_meshsets.inc"
};
static NJS_POINT3 collision_80_points[6] = {
#include "stg13/terrain/collision_80_points.inc"
};
static NJS_MODEL collision_80_model = {
#include "stg13/terrain/collision_80_model.inc"
};
static NJS_OBJECT collision_80 = {
#include "stg13/terrain/collision_80.inc"
};
static NJS_MATERIAL collision_81_materials[1] = {
#include "stg13/terrain/collision_81_materials.inc"
};
static Sint16 collision_81_faces[84] = {
#include "stg13/terrain/collision_81_faces.inc"
};
static NJS_MESHSET collision_81_meshsets[1] = {
#include "stg13/terrain/collision_81_meshsets.inc"
};
static NJS_POINT3 collision_81_points[60] = {
#include "stg13/terrain/collision_81_points.inc"
};
static NJS_MODEL collision_81_model = {
#include "stg13/terrain/collision_81_model.inc"
};
static NJS_OBJECT collision_81 = {
#include "stg13/terrain/collision_81.inc"
};
static NJS_MATERIAL collision_82_materials[1] = {
#include "stg13/terrain/collision_82_materials.inc"
};
static Sint16 collision_82_faces[10] = {
#include "stg13/terrain/collision_82_faces.inc"
};
static NJS_MESHSET collision_82_meshsets[1] = {
#include "stg13/terrain/collision_82_meshsets.inc"
};
static NJS_POINT3 collision_82_points[6] = {
#include "stg13/terrain/collision_82_points.inc"
};
static NJS_MODEL collision_82_model = {
#include "stg13/terrain/collision_82_model.inc"
};
static NJS_OBJECT collision_82 = {
#include "stg13/terrain/collision_82.inc"
};
static NJS_MATERIAL collision_83_materials[1] = {
#include "stg13/terrain/collision_83_materials.inc"
};
static Sint16 collision_83_faces[32] = {
#include "stg13/terrain/collision_83_faces.inc"
};
static NJS_MESHSET collision_83_meshsets[1] = {
#include "stg13/terrain/collision_83_meshsets.inc"
};
static NJS_POINT3 collision_83_points[30] = {
#include "stg13/terrain/collision_83_points.inc"
};
static NJS_MODEL collision_83_model = {
#include "stg13/terrain/collision_83_model.inc"
};
static NJS_OBJECT collision_83 = {
#include "stg13/terrain/collision_83.inc"
};
static NJS_MATERIAL collision_84_materials[1] = {
#include "stg13/terrain/collision_84_materials.inc"
};
static Sint16 collision_84_faces[73] = {
#include "stg13/terrain/collision_84_faces.inc"
};
static NJS_MESHSET collision_84_meshsets[1] = {
#include "stg13/terrain/collision_84_meshsets.inc"
};
static NJS_POINT3 collision_84_points[43] = {
#include "stg13/terrain/collision_84_points.inc"
};
static NJS_MODEL collision_84_model = {
#include "stg13/terrain/collision_84_model.inc"
};
static NJS_OBJECT collision_84 = {
#include "stg13/terrain/collision_84.inc"
};
static NJS_MATERIAL collision_85_materials[1] = {
#include "stg13/terrain/collision_85_materials.inc"
};
static Sint16 collision_85_faces[99] = {
#include "stg13/terrain/collision_85_faces.inc"
};
static NJS_MESHSET collision_85_meshsets[1] = {
#include "stg13/terrain/collision_85_meshsets.inc"
};
static NJS_POINT3 collision_85_points[60] = {
#include "stg13/terrain/collision_85_points.inc"
};
static NJS_MODEL collision_85_model = {
#include "stg13/terrain/collision_85_model.inc"
};
static NJS_OBJECT collision_85 = {
#include "stg13/terrain/collision_85.inc"
};
static NJS_MATERIAL collision_86_materials[1] = {
#include "stg13/terrain/collision_86_materials.inc"
};
static Sint16 collision_86_faces[39] = {
#include "stg13/terrain/collision_86_faces.inc"
};
static NJS_MESHSET collision_86_meshsets[1] = {
#include "stg13/terrain/collision_86_meshsets.inc"
};
static NJS_POINT3 collision_86_points[38] = {
#include "stg13/terrain/collision_86_points.inc"
};
static NJS_MODEL collision_86_model = {
#include "stg13/terrain/collision_86_model.inc"
};
static NJS_OBJECT collision_86 = {
#include "stg13/terrain/collision_86.inc"
};
static NJS_MATERIAL collision_87_materials[1] = {
#include "stg13/terrain/collision_87_materials.inc"
};
static Sint16 collision_87_faces[99] = {
#include "stg13/terrain/collision_87_faces.inc"
};
static NJS_MESHSET collision_87_meshsets[1] = {
#include "stg13/terrain/collision_87_meshsets.inc"
};
static NJS_POINT3 collision_87_points[60] = {
#include "stg13/terrain/collision_87_points.inc"
};
static NJS_MODEL collision_87_model = {
#include "stg13/terrain/collision_87_model.inc"
};
static NJS_OBJECT collision_87 = {
#include "stg13/terrain/collision_87.inc"
};
static NJS_MATERIAL collision_88_materials[1] = {
#include "stg13/terrain/collision_88_materials.inc"
};
static Sint16 collision_88_faces[76] = {
#include "stg13/terrain/collision_88_faces.inc"
};
static NJS_MESHSET collision_88_meshsets[1] = {
#include "stg13/terrain/collision_88_meshsets.inc"
};
static NJS_POINT3 collision_88_points[54] = {
#include "stg13/terrain/collision_88_points.inc"
};
static NJS_MODEL collision_88_model = {
#include "stg13/terrain/collision_88_model.inc"
};
static NJS_OBJECT collision_88 = {
#include "stg13/terrain/collision_88.inc"
};
static NJS_MATERIAL collision_89_materials[1] = {
#include "stg13/terrain/collision_89_materials.inc"
};
static Sint16 collision_89_faces[76] = {
#include "stg13/terrain/collision_89_faces.inc"
};
static NJS_MESHSET collision_89_meshsets[1] = {
#include "stg13/terrain/collision_89_meshsets.inc"
};
static NJS_POINT3 collision_89_points[54] = {
#include "stg13/terrain/collision_89_points.inc"
};
static NJS_MODEL collision_89_model = {
#include "stg13/terrain/collision_89_model.inc"
};
static NJS_OBJECT collision_89 = {
#include "stg13/terrain/collision_89.inc"
};
static NJS_MATERIAL collision_90_materials[1] = {
#include "stg13/terrain/collision_90_materials.inc"
};
static Sint16 collision_90_faces[32] = {
#include "stg13/terrain/collision_90_faces.inc"
};
static NJS_MESHSET collision_90_meshsets[1] = {
#include "stg13/terrain/collision_90_meshsets.inc"
};
static NJS_POINT3 collision_90_points[30] = {
#include "stg13/terrain/collision_90_points.inc"
};
static NJS_MODEL collision_90_model = {
#include "stg13/terrain/collision_90_model.inc"
};
static NJS_OBJECT collision_90 = {
#include "stg13/terrain/collision_90.inc"
};
static NJS_MATERIAL collision_91_materials[1] = {
#include "stg13/terrain/collision_91_materials.inc"
};
static Sint16 collision_91_faces[34] = {
#include "stg13/terrain/collision_91_faces.inc"
};
static NJS_MESHSET collision_91_meshsets[1] = {
#include "stg13/terrain/collision_91_meshsets.inc"
};
static NJS_POINT3 collision_91_points[26] = {
#include "stg13/terrain/collision_91_points.inc"
};
static NJS_MODEL collision_91_model = {
#include "stg13/terrain/collision_91_model.inc"
};
static NJS_OBJECT collision_91 = {
#include "stg13/terrain/collision_91.inc"
};
static NJS_MATERIAL collision_92_materials[1] = {
#include "stg13/terrain/collision_92_materials.inc"
};
static Sint16 collision_92_faces[60] = {
#include "stg13/terrain/collision_92_faces.inc"
};
static NJS_MESHSET collision_92_meshsets[1] = {
#include "stg13/terrain/collision_92_meshsets.inc"
};
static NJS_POINT3 collision_92_points[42] = {
#include "stg13/terrain/collision_92_points.inc"
};
static NJS_MODEL collision_92_model = {
#include "stg13/terrain/collision_92_model.inc"
};
static NJS_OBJECT collision_92 = {
#include "stg13/terrain/collision_92.inc"
};
static NJS_MATERIAL collision_93_materials[1] = {
#include "stg13/terrain/collision_93_materials.inc"
};
static Sint16 collision_93_faces[7] = {
#include "stg13/terrain/collision_93_faces.inc"
};
static NJS_MESHSET collision_93_meshsets[1] = {
#include "stg13/terrain/collision_93_meshsets.inc"
};
static NJS_POINT3 collision_93_points[6] = {
#include "stg13/terrain/collision_93_points.inc"
};
static NJS_MODEL collision_93_model = {
#include "stg13/terrain/collision_93_model.inc"
};
static NJS_OBJECT collision_93 = {
#include "stg13/terrain/collision_93.inc"
};
static NJS_MATERIAL collision_101_materials[1] = {
#include "stg13/terrain/collision_101_materials.inc"
};
static Sint16 collision_101_faces[60] = {
#include "stg13/terrain/collision_101_faces.inc"
};
static NJS_MESHSET collision_101_meshsets[1] = {
#include "stg13/terrain/collision_101_meshsets.inc"
};
static NJS_POINT3 collision_101_points[42] = {
#include "stg13/terrain/collision_101_points.inc"
};
static NJS_MODEL collision_101_model = {
#include "stg13/terrain/collision_101_model.inc"
};
static NJS_OBJECT collision_101 = {
#include "stg13/terrain/collision_101.inc"
};
static NJS_MATERIAL collision_102_materials[1] = {
#include "stg13/terrain/collision_102_materials.inc"
};
static Sint16 collision_102_faces[73] = {
#include "stg13/terrain/collision_102_faces.inc"
};
static NJS_MESHSET collision_102_meshsets[1] = {
#include "stg13/terrain/collision_102_meshsets.inc"
};
static NJS_POINT3 collision_102_points[46] = {
#include "stg13/terrain/collision_102_points.inc"
};
static NJS_MODEL collision_102_model = {
#include "stg13/terrain/collision_102_model.inc"
};
static NJS_OBJECT collision_102 = {
#include "stg13/terrain/collision_102.inc"
};
static NJS_MATERIAL collision_105_materials[1] = {
#include "stg13/terrain/collision_105_materials.inc"
};
static Sint16 collision_105_faces[84] = {
#include "stg13/terrain/collision_105_faces.inc"
};
static NJS_MESHSET collision_105_meshsets[1] = {
#include "stg13/terrain/collision_105_meshsets.inc"
};
static NJS_POINT3 collision_105_points[60] = {
#include "stg13/terrain/collision_105_points.inc"
};
static NJS_MODEL collision_105_model = {
#include "stg13/terrain/collision_105_model.inc"
};
static NJS_OBJECT collision_105 = {
#include "stg13/terrain/collision_105.inc"
};
static NJS_MATERIAL collision_108_materials[1] = {
#include "stg13/terrain/collision_108_materials.inc"
};
static Sint16 collision_108_faces[66] = {
#include "stg13/terrain/collision_108_faces.inc"
};
static NJS_MESHSET collision_108_meshsets[1] = {
#include "stg13/terrain/collision_108_meshsets.inc"
};
static NJS_POINT3 collision_108_points[42] = {
#include "stg13/terrain/collision_108_points.inc"
};
static NJS_MODEL collision_108_model = {
#include "stg13/terrain/collision_108_model.inc"
};
static NJS_OBJECT collision_108 = {
#include "stg13/terrain/collision_108.inc"
};
static NJS_MATERIAL collision_111_materials[1] = {
#include "stg13/terrain/collision_111_materials.inc"
};
static Sint16 collision_111_faces[66] = {
#include "stg13/terrain/collision_111_faces.inc"
};
static NJS_MESHSET collision_111_meshsets[1] = {
#include "stg13/terrain/collision_111_meshsets.inc"
};
static NJS_POINT3 collision_111_points[35] = {
#include "stg13/terrain/collision_111_points.inc"
};
static NJS_MODEL collision_111_model = {
#include "stg13/terrain/collision_111_model.inc"
};
static NJS_OBJECT collision_111 = {
#include "stg13/terrain/collision_111.inc"
};
static NJS_MATERIAL collision_113_materials[1] = {
#include "stg13/terrain/collision_113_materials.inc"
};
static Sint16 collision_113_faces[84] = {
#include "stg13/terrain/collision_113_faces.inc"
};
static NJS_MESHSET collision_113_meshsets[1] = {
#include "stg13/terrain/collision_113_meshsets.inc"
};
static NJS_POINT3 collision_113_points[50] = {
#include "stg13/terrain/collision_113_points.inc"
};
static NJS_MODEL collision_113_model = {
#include "stg13/terrain/collision_113_model.inc"
};
static NJS_OBJECT collision_113 = {
#include "stg13/terrain/collision_113.inc"
};
static NJS_MATERIAL collision_115_materials[1] = {
#include "stg13/terrain/collision_115_materials.inc"
};
static Sint16 collision_115_faces[63] = {
#include "stg13/terrain/collision_115_faces.inc"
};
static NJS_MESHSET collision_115_meshsets[1] = {
#include "stg13/terrain/collision_115_meshsets.inc"
};
static NJS_POINT3 collision_115_points[35] = {
#include "stg13/terrain/collision_115_points.inc"
};
static NJS_MODEL collision_115_model = {
#include "stg13/terrain/collision_115_model.inc"
};
static NJS_OBJECT collision_115 = {
#include "stg13/terrain/collision_115.inc"
};
static NJS_MATERIAL collision_118_materials[1] = {
#include "stg13/terrain/collision_118_materials.inc"
};
static Sint16 collision_118_faces[44] = {
#include "stg13/terrain/collision_118_faces.inc"
};
static NJS_MESHSET collision_118_meshsets[1] = {
#include "stg13/terrain/collision_118_meshsets.inc"
};
static NJS_POINT3 collision_118_points[36] = {
#include "stg13/terrain/collision_118_points.inc"
};
static NJS_MODEL collision_118_model = {
#include "stg13/terrain/collision_118_model.inc"
};
static NJS_OBJECT collision_118 = {
#include "stg13/terrain/collision_118.inc"
};
static NJS_MATERIAL collision_120_materials[1] = {
#include "stg13/terrain/collision_120_materials.inc"
};
static Sint16 collision_120_faces[63] = {
#include "stg13/terrain/collision_120_faces.inc"
};
static NJS_MESHSET collision_120_meshsets[1] = {
#include "stg13/terrain/collision_120_meshsets.inc"
};
static NJS_POINT3 collision_120_points[44] = {
#include "stg13/terrain/collision_120_points.inc"
};
static NJS_MODEL collision_120_model = {
#include "stg13/terrain/collision_120_model.inc"
};
static NJS_OBJECT collision_120 = {
#include "stg13/terrain/collision_120.inc"
};
static NJS_MATERIAL collision_121_materials[1] = {
#include "stg13/terrain/collision_121_materials.inc"
};
static Sint16 collision_121_faces[67] = {
#include "stg13/terrain/collision_121_faces.inc"
};
static NJS_MESHSET collision_121_meshsets[1] = {
#include "stg13/terrain/collision_121_meshsets.inc"
};
static NJS_POINT3 collision_121_points[40] = {
#include "stg13/terrain/collision_121_points.inc"
};
static NJS_MODEL collision_121_model = {
#include "stg13/terrain/collision_121_model.inc"
};
static NJS_OBJECT collision_121 = {
#include "stg13/terrain/collision_121.inc"
};
static NJS_MATERIAL collision_122_materials[1] = {
#include "stg13/terrain/collision_122_materials.inc"
};
static Sint16 collision_122_faces[88] = {
#include "stg13/terrain/collision_122_faces.inc"
};
static NJS_MESHSET collision_122_meshsets[1] = {
#include "stg13/terrain/collision_122_meshsets.inc"
};
static NJS_POINT3 collision_122_points[54] = {
#include "stg13/terrain/collision_122_points.inc"
};
static NJS_MODEL collision_122_model = {
#include "stg13/terrain/collision_122_model.inc"
};
static NJS_OBJECT collision_122 = {
#include "stg13/terrain/collision_122.inc"
};
static NJS_MATERIAL collision_125_materials[1] = {
#include "stg13/terrain/collision_125_materials.inc"
};
static Sint16 collision_125_faces[88] = {
#include "stg13/terrain/collision_125_faces.inc"
};
static NJS_MESHSET collision_125_meshsets[1] = {
#include "stg13/terrain/collision_125_meshsets.inc"
};
static NJS_POINT3 collision_125_points[54] = {
#include "stg13/terrain/collision_125_points.inc"
};
static NJS_MODEL collision_125_model = {
#include "stg13/terrain/collision_125_model.inc"
};
static NJS_OBJECT collision_125 = {
#include "stg13/terrain/collision_125.inc"
};
static NJS_MATERIAL collision_128_materials[1] = {
#include "stg13/terrain/collision_128_materials.inc"
};
static Sint16 collision_128_faces[84] = {
#include "stg13/terrain/collision_128_faces.inc"
};
static NJS_MESHSET collision_128_meshsets[1] = {
#include "stg13/terrain/collision_128_meshsets.inc"
};
static NJS_POINT3 collision_128_points[60] = {
#include "stg13/terrain/collision_128_points.inc"
};
static NJS_MODEL collision_128_model = {
#include "stg13/terrain/collision_128_model.inc"
};
static NJS_OBJECT collision_128 = {
#include "stg13/terrain/collision_128.inc"
};
static NJS_MATERIAL collision_131_materials[1] = {
#include "stg13/terrain/collision_131_materials.inc"
};
static Sint16 collision_131_faces[84] = {
#include "stg13/terrain/collision_131_faces.inc"
};
static NJS_MESHSET collision_131_meshsets[1] = {
#include "stg13/terrain/collision_131_meshsets.inc"
};
static NJS_POINT3 collision_131_points[60] = {
#include "stg13/terrain/collision_131_points.inc"
};
static NJS_MODEL collision_131_model = {
#include "stg13/terrain/collision_131_model.inc"
};
static NJS_OBJECT collision_131 = {
#include "stg13/terrain/collision_131.inc"
};
static NJS_MATERIAL collision_134_materials[1] = {
#include "stg13/terrain/collision_134_materials.inc"
};
static Sint16 collision_134_faces[88] = {
#include "stg13/terrain/collision_134_faces.inc"
};
static NJS_MESHSET collision_134_meshsets[1] = {
#include "stg13/terrain/collision_134_meshsets.inc"
};
static NJS_POINT3 collision_134_points[54] = {
#include "stg13/terrain/collision_134_points.inc"
};
static NJS_MODEL collision_134_model = {
#include "stg13/terrain/collision_134_model.inc"
};
static NJS_OBJECT collision_134 = {
#include "stg13/terrain/collision_134.inc"
};
static NJS_MATERIAL collision_137_materials[1] = {
#include "stg13/terrain/collision_137_materials.inc"
};
static Sint16 collision_137_faces[23] = {
#include "stg13/terrain/collision_137_faces.inc"
};
static NJS_MESHSET collision_137_meshsets[1] = {
#include "stg13/terrain/collision_137_meshsets.inc"
};
static NJS_POINT3 collision_137_points[22] = {
#include "stg13/terrain/collision_137_points.inc"
};
static NJS_MODEL collision_137_model = {
#include "stg13/terrain/collision_137_model.inc"
};
static NJS_OBJECT collision_137 = {
#include "stg13/terrain/collision_137.inc"
};
static NJS_MATERIAL collision_141_materials[1] = {
#include "stg13/terrain/collision_141_materials.inc"
};
static Sint16 collision_141_faces[88] = {
#include "stg13/terrain/collision_141_faces.inc"
};
static NJS_MESHSET collision_141_meshsets[1] = {
#include "stg13/terrain/collision_141_meshsets.inc"
};
static NJS_POINT3 collision_141_points[54] = {
#include "stg13/terrain/collision_141_points.inc"
};
static NJS_MODEL collision_141_model = {
#include "stg13/terrain/collision_141_model.inc"
};
static NJS_OBJECT collision_141 = {
#include "stg13/terrain/collision_141.inc"
};
static NJS_MATERIAL collision_144_materials[1] = {
#include "stg13/terrain/collision_144_materials.inc"
};
static Sint16 collision_144_faces[73] = {
#include "stg13/terrain/collision_144_faces.inc"
};
static NJS_MESHSET collision_144_meshsets[1] = {
#include "stg13/terrain/collision_144_meshsets.inc"
};
static NJS_POINT3 collision_144_points[46] = {
#include "stg13/terrain/collision_144_points.inc"
};
static NJS_MODEL collision_144_model = {
#include "stg13/terrain/collision_144_model.inc"
};
static NJS_OBJECT collision_144 = {
#include "stg13/terrain/collision_144.inc"
};
static NJS_MATERIAL collision_146_materials[1] = {
#include "stg13/terrain/collision_146_materials.inc"
};
static Sint16 collision_146_faces[66] = {
#include "stg13/terrain/collision_146_faces.inc"
};
static NJS_MESHSET collision_146_meshsets[1] = {
#include "stg13/terrain/collision_146_meshsets.inc"
};
static NJS_POINT3 collision_146_points[42] = {
#include "stg13/terrain/collision_146_points.inc"
};
static NJS_MODEL collision_146_model = {
#include "stg13/terrain/collision_146_model.inc"
};
static NJS_OBJECT collision_146 = {
#include "stg13/terrain/collision_146.inc"
};
static NJS_MATERIAL collision_148_materials[1] = {
#include "stg13/terrain/collision_148_materials.inc"
};
static Sint16 collision_148_faces[28] = {
#include "stg13/terrain/collision_148_faces.inc"
};
static NJS_MESHSET collision_148_meshsets[1] = {
#include "stg13/terrain/collision_148_meshsets.inc"
};
static NJS_POINT3 collision_148_points[18] = {
#include "stg13/terrain/collision_148_points.inc"
};
static NJS_MODEL collision_148_model = {
#include "stg13/terrain/collision_148_model.inc"
};
static NJS_OBJECT collision_148 = {
#include "stg13/terrain/collision_148.inc"
};
static NJS_MATERIAL collision_150_materials[1] = {
#include "stg13/terrain/collision_150_materials.inc"
};
static Sint16 collision_150_faces[43] = {
#include "stg13/terrain/collision_150_faces.inc"
};
static NJS_MESHSET collision_150_meshsets[1] = {
#include "stg13/terrain/collision_150_meshsets.inc"
};
static NJS_POINT3 collision_150_points[25] = {
#include "stg13/terrain/collision_150_points.inc"
};
static NJS_MODEL collision_150_model = {
#include "stg13/terrain/collision_150_model.inc"
};
static NJS_OBJECT collision_150 = {
#include "stg13/terrain/collision_150.inc"
};
static NJS_MATERIAL collision_152_materials[1] = {
#include "stg13/terrain/collision_152_materials.inc"
};
static Sint16 collision_152_faces[63] = {
#include "stg13/terrain/collision_152_faces.inc"
};
static NJS_MESHSET collision_152_meshsets[1] = {
#include "stg13/terrain/collision_152_meshsets.inc"
};
static NJS_POINT3 collision_152_points[35] = {
#include "stg13/terrain/collision_152_points.inc"
};
static NJS_MODEL collision_152_model = {
#include "stg13/terrain/collision_152_model.inc"
};
static NJS_OBJECT collision_152 = {
#include "stg13/terrain/collision_152.inc"
};
static NJS_MATERIAL collision_154_materials[1] = {
#include "stg13/terrain/collision_154_materials.inc"
};
static Sint16 collision_154_faces[64] = {
#include "stg13/terrain/collision_154_faces.inc"
};
static NJS_MESHSET collision_154_meshsets[1] = {
#include "stg13/terrain/collision_154_meshsets.inc"
};
static NJS_POINT3 collision_154_points[36] = {
#include "stg13/terrain/collision_154_points.inc"
};
static NJS_MODEL collision_154_model = {
#include "stg13/terrain/collision_154_model.inc"
};
static NJS_OBJECT collision_154 = {
#include "stg13/terrain/collision_154.inc"
};
static NJS_MATERIAL collision_156_materials[1] = {
#include "stg13/terrain/collision_156_materials.inc"
};
static Sint16 collision_156_faces[12] = {
#include "stg13/terrain/collision_156_faces.inc"
};
static NJS_MESHSET collision_156_meshsets[1] = {
#include "stg13/terrain/collision_156_meshsets.inc"
};
static NJS_POINT3 collision_156_points[10] = {
#include "stg13/terrain/collision_156_points.inc"
};
static NJS_MODEL collision_156_model = {
#include "stg13/terrain/collision_156_model.inc"
};
static NJS_OBJECT collision_156 = {
#include "stg13/terrain/collision_156.inc"
};
static NJS_MATERIAL collision_157_materials[1] = {
#include "stg13/terrain/collision_157_materials.inc"
};
static Sint16 collision_157_faces[21] = {
#include "stg13/terrain/collision_157_faces.inc"
};
static NJS_MESHSET collision_157_meshsets[1] = {
#include "stg13/terrain/collision_157_meshsets.inc"
};
static NJS_POINT3 collision_157_points[15] = {
#include "stg13/terrain/collision_157_points.inc"
};
static NJS_MODEL collision_157_model = {
#include "stg13/terrain/collision_157_model.inc"
};
static NJS_OBJECT collision_157 = {
#include "stg13/terrain/collision_157.inc"
};
static NJS_MATERIAL collision_158_materials[1] = {
#include "stg13/terrain/collision_158_materials.inc"
};
static Sint16 collision_158_faces[21] = {
#include "stg13/terrain/collision_158_faces.inc"
};
static NJS_MESHSET collision_158_meshsets[1] = {
#include "stg13/terrain/collision_158_meshsets.inc"
};
static NJS_POINT3 collision_158_points[15] = {
#include "stg13/terrain/collision_158_points.inc"
};
static NJS_MODEL collision_158_model = {
#include "stg13/terrain/collision_158_model.inc"
};
static NJS_OBJECT collision_158 = {
#include "stg13/terrain/collision_158.inc"
};
static NJS_MATERIAL collision_159_materials[1] = {
#include "stg13/terrain/collision_159_materials.inc"
};
static Sint16 collision_159_faces[37] = {
#include "stg13/terrain/collision_159_faces.inc"
};
static NJS_MESHSET collision_159_meshsets[1] = {
#include "stg13/terrain/collision_159_meshsets.inc"
};
static NJS_POINT3 collision_159_points[24] = {
#include "stg13/terrain/collision_159_points.inc"
};
static NJS_MODEL collision_159_model = {
#include "stg13/terrain/collision_159_model.inc"
};
static NJS_OBJECT collision_159 = {
#include "stg13/terrain/collision_159.inc"
};
static NJS_MATERIAL collision_161_materials[1] = {
#include "stg13/terrain/collision_161_materials.inc"
};
static Sint16 collision_161_faces[12] = {
#include "stg13/terrain/collision_161_faces.inc"
};
static NJS_MESHSET collision_161_meshsets[1] = {
#include "stg13/terrain/collision_161_meshsets.inc"
};
static NJS_POINT3 collision_161_points[10] = {
#include "stg13/terrain/collision_161_points.inc"
};
static NJS_MODEL collision_161_model = {
#include "stg13/terrain/collision_161_model.inc"
};
static NJS_OBJECT collision_161 = {
#include "stg13/terrain/collision_161.inc"
};
static NJS_MATERIAL collision_162_materials[1] = {
#include "stg13/terrain/collision_162_materials.inc"
};
static Sint16 collision_162_faces[37] = {
#include "stg13/terrain/collision_162_faces.inc"
};
static NJS_MESHSET collision_162_meshsets[1] = {
#include "stg13/terrain/collision_162_meshsets.inc"
};
static NJS_POINT3 collision_162_points[24] = {
#include "stg13/terrain/collision_162_points.inc"
};
static NJS_MODEL collision_162_model = {
#include "stg13/terrain/collision_162_model.inc"
};
static NJS_OBJECT collision_162 = {
#include "stg13/terrain/collision_162.inc"
};
static NJS_MATERIAL collision_163_materials[1] = {
#include "stg13/terrain/collision_163_materials.inc"
};
static Sint16 collision_163_faces[37] = {
#include "stg13/terrain/collision_163_faces.inc"
};
static NJS_MESHSET collision_163_meshsets[1] = {
#include "stg13/terrain/collision_163_meshsets.inc"
};
static NJS_POINT3 collision_163_points[24] = {
#include "stg13/terrain/collision_163_points.inc"
};
static NJS_MODEL collision_163_model = {
#include "stg13/terrain/collision_163_model.inc"
};
static NJS_OBJECT collision_163 = {
#include "stg13/terrain/collision_163.inc"
};
static NJS_MATERIAL collision_164_materials[1] = {
#include "stg13/terrain/collision_164_materials.inc"
};
static Sint16 collision_164_faces[28] = {
#include "stg13/terrain/collision_164_faces.inc"
};
static NJS_MESHSET collision_164_meshsets[1] = {
#include "stg13/terrain/collision_164_meshsets.inc"
};
static NJS_POINT3 collision_164_points[18] = {
#include "stg13/terrain/collision_164_points.inc"
};
static NJS_MODEL collision_164_model = {
#include "stg13/terrain/collision_164_model.inc"
};
static NJS_OBJECT collision_164 = {
#include "stg13/terrain/collision_164.inc"
};
static NJS_MATERIAL collision_166_materials[1] = {
#include "stg13/terrain/collision_166_materials.inc"
};
static Sint16 collision_166_faces[28] = {
#include "stg13/terrain/collision_166_faces.inc"
};
static NJS_MESHSET collision_166_meshsets[1] = {
#include "stg13/terrain/collision_166_meshsets.inc"
};
static NJS_POINT3 collision_166_points[18] = {
#include "stg13/terrain/collision_166_points.inc"
};
static NJS_MODEL collision_166_model = {
#include "stg13/terrain/collision_166_model.inc"
};
static NJS_OBJECT collision_166 = {
#include "stg13/terrain/collision_166.inc"
};
static NJS_MATERIAL collision_172_materials[1] = {
#include "stg13/terrain/collision_172_materials.inc"
};
static Sint16 collision_172_faces[91] = {
#include "stg13/terrain/collision_172_faces.inc"
};
static NJS_MESHSET collision_172_meshsets[1] = {
#include "stg13/terrain/collision_172_meshsets.inc"
};
static NJS_POINT3 collision_172_points[54] = {
#include "stg13/terrain/collision_172_points.inc"
};
static NJS_MODEL collision_172_model = {
#include "stg13/terrain/collision_172_model.inc"
};
static NJS_OBJECT collision_172 = {
#include "stg13/terrain/collision_172.inc"
};
static NJS_MATERIAL collision_174_materials[1] = {
#include "stg13/terrain/collision_174_materials.inc"
};
static Sint16 collision_174_faces[40] = {
#include "stg13/terrain/collision_174_faces.inc"
};
static NJS_MESHSET collision_174_meshsets[1] = {
#include "stg13/terrain/collision_174_meshsets.inc"
};
static NJS_POINT3 collision_174_points[21] = {
#include "stg13/terrain/collision_174_points.inc"
};
static NJS_MODEL collision_174_model = {
#include "stg13/terrain/collision_174_model.inc"
};
static NJS_OBJECT collision_174 = {
#include "stg13/terrain/collision_174.inc"
};
static NJS_MATERIAL collision_175_materials[1] = {
#include "stg13/terrain/collision_175_materials.inc"
};
static Sint16 collision_175_faces[91] = {
#include "stg13/terrain/collision_175_faces.inc"
};
static NJS_MESHSET collision_175_meshsets[1] = {
#include "stg13/terrain/collision_175_meshsets.inc"
};
static NJS_POINT3 collision_175_points[54] = {
#include "stg13/terrain/collision_175_points.inc"
};
static NJS_MODEL collision_175_model = {
#include "stg13/terrain/collision_175_model.inc"
};
static NJS_OBJECT collision_175 = {
#include "stg13/terrain/collision_175.inc"
};
static NJS_MATERIAL collision_177_materials[1] = {
#include "stg13/terrain/collision_177_materials.inc"
};
static Sint16 collision_177_faces[44] = {
#include "stg13/terrain/collision_177_faces.inc"
};
static NJS_MESHSET collision_177_meshsets[1] = {
#include "stg13/terrain/collision_177_meshsets.inc"
};
static NJS_POINT3 collision_177_points[27] = {
#include "stg13/terrain/collision_177_points.inc"
};
static NJS_MODEL collision_177_model = {
#include "stg13/terrain/collision_177_model.inc"
};
static NJS_OBJECT collision_177 = {
#include "stg13/terrain/collision_177.inc"
};
static NJS_MATERIAL collision_178_materials[1] = {
#include "stg13/terrain/collision_178_materials.inc"
};
static Sint16 collision_178_faces[21] = {
#include "stg13/terrain/collision_178_faces.inc"
};
static NJS_MESHSET collision_178_meshsets[1] = {
#include "stg13/terrain/collision_178_meshsets.inc"
};
static NJS_POINT3 collision_178_points[15] = {
#include "stg13/terrain/collision_178_points.inc"
};
static NJS_MODEL collision_178_model = {
#include "stg13/terrain/collision_178_model.inc"
};
static NJS_OBJECT collision_178 = {
#include "stg13/terrain/collision_178.inc"
};
static NJS_MATERIAL collision_179_materials[1] = {
#include "stg13/terrain/collision_179_materials.inc"
};
static Sint16 collision_179_faces[91] = {
#include "stg13/terrain/collision_179_faces.inc"
};
static NJS_MESHSET collision_179_meshsets[1] = {
#include "stg13/terrain/collision_179_meshsets.inc"
};
static NJS_POINT3 collision_179_points[54] = {
#include "stg13/terrain/collision_179_points.inc"
};
static NJS_MODEL collision_179_model = {
#include "stg13/terrain/collision_179_model.inc"
};
static NJS_OBJECT collision_179 = {
#include "stg13/terrain/collision_179.inc"
};
static NJS_MATERIAL collision_181_materials[1] = {
#include "stg13/terrain/collision_181_materials.inc"
};
static Sint16 collision_181_faces[23] = {
#include "stg13/terrain/collision_181_faces.inc"
};
static NJS_MESHSET collision_181_meshsets[1] = {
#include "stg13/terrain/collision_181_meshsets.inc"
};
static NJS_POINT3 collision_181_points[22] = {
#include "stg13/terrain/collision_181_points.inc"
};
static NJS_MODEL collision_181_model = {
#include "stg13/terrain/collision_181_model.inc"
};
static NJS_OBJECT collision_181 = {
#include "stg13/terrain/collision_181.inc"
};
static NJS_MATERIAL collision_185_materials[1] = {
#include "stg13/terrain/collision_185_materials.inc"
};
static Sint16 collision_185_faces[91] = {
#include "stg13/terrain/collision_185_faces.inc"
};
static NJS_MESHSET collision_185_meshsets[1] = {
#include "stg13/terrain/collision_185_meshsets.inc"
};
static NJS_POINT3 collision_185_points[54] = {
#include "stg13/terrain/collision_185_points.inc"
};
static NJS_MODEL collision_185_model = {
#include "stg13/terrain/collision_185_model.inc"
};
static NJS_OBJECT collision_185 = {
#include "stg13/terrain/collision_185.inc"
};
static NJS_MATERIAL collision_187_materials[1] = {
#include "stg13/terrain/collision_187_materials.inc"
};
static Sint16 collision_187_faces[53] = {
#include "stg13/terrain/collision_187_faces.inc"
};
static NJS_MESHSET collision_187_meshsets[1] = {
#include "stg13/terrain/collision_187_meshsets.inc"
};
static NJS_POINT3 collision_187_points[26] = {
#include "stg13/terrain/collision_187_points.inc"
};
static NJS_MODEL collision_187_model = {
#include "stg13/terrain/collision_187_model.inc"
};
static NJS_OBJECT collision_187 = {
#include "stg13/terrain/collision_187.inc"
};
static NJS_MATERIAL collision_188_materials[1] = {
#include "stg13/terrain/collision_188_materials.inc"
};
static Sint16 collision_188_faces[28] = {
#include "stg13/terrain/collision_188_faces.inc"
};
static NJS_MESHSET collision_188_meshsets[1] = {
#include "stg13/terrain/collision_188_meshsets.inc"
};
static NJS_POINT3 collision_188_points[18] = {
#include "stg13/terrain/collision_188_points.inc"
};
static NJS_MODEL collision_188_model = {
#include "stg13/terrain/collision_188_model.inc"
};
static NJS_OBJECT collision_188 = {
#include "stg13/terrain/collision_188.inc"
};
static NJS_MATERIAL collision_189_materials[1] = {
#include "stg13/terrain/collision_189_materials.inc"
};
static Sint16 collision_189_faces[12] = {
#include "stg13/terrain/collision_189_faces.inc"
};
static NJS_MESHSET collision_189_meshsets[1] = {
#include "stg13/terrain/collision_189_meshsets.inc"
};
static NJS_POINT3 collision_189_points[10] = {
#include "stg13/terrain/collision_189_points.inc"
};
static NJS_MODEL collision_189_model = {
#include "stg13/terrain/collision_189_model.inc"
};
static NJS_OBJECT collision_189 = {
#include "stg13/terrain/collision_189.inc"
};
static NJS_MATERIAL collision_190_materials[1] = {
#include "stg13/terrain/collision_190_materials.inc"
};
static Sint16 collision_190_faces[28] = {
#include "stg13/terrain/collision_190_faces.inc"
};
static NJS_MESHSET collision_190_meshsets[1] = {
#include "stg13/terrain/collision_190_meshsets.inc"
};
static NJS_POINT3 collision_190_points[18] = {
#include "stg13/terrain/collision_190_points.inc"
};
static NJS_MODEL collision_190_model = {
#include "stg13/terrain/collision_190_model.inc"
};
static NJS_OBJECT collision_190 = {
#include "stg13/terrain/collision_190.inc"
};
static NJS_MATERIAL collision_194_materials[1] = {
#include "stg13/terrain/collision_194_materials.inc"
};
static Sint16 collision_194_faces[28] = {
#include "stg13/terrain/collision_194_faces.inc"
};
static NJS_MESHSET collision_194_meshsets[1] = {
#include "stg13/terrain/collision_194_meshsets.inc"
};
static NJS_POINT3 collision_194_points[18] = {
#include "stg13/terrain/collision_194_points.inc"
};
static NJS_MODEL collision_194_model = {
#include "stg13/terrain/collision_194_model.inc"
};
static NJS_OBJECT collision_194 = {
#include "stg13/terrain/collision_194.inc"
};
static NJS_MATERIAL collision_196_materials[1] = {
#include "stg13/terrain/collision_196_materials.inc"
};
static Sint16 collision_196_faces[28] = {
#include "stg13/terrain/collision_196_faces.inc"
};
static NJS_MESHSET collision_196_meshsets[1] = {
#include "stg13/terrain/collision_196_meshsets.inc"
};
static NJS_POINT3 collision_196_points[18] = {
#include "stg13/terrain/collision_196_points.inc"
};
static NJS_MODEL collision_196_model = {
#include "stg13/terrain/collision_196_model.inc"
};
static NJS_OBJECT collision_196 = {
#include "stg13/terrain/collision_196.inc"
};
static NJS_MATERIAL collision_197_materials[1] = {
#include "stg13/terrain/collision_197_materials.inc"
};
static Sint16 collision_197_faces[14] = {
#include "stg13/terrain/collision_197_faces.inc"
};
static NJS_MESHSET collision_197_meshsets[1] = {
#include "stg13/terrain/collision_197_meshsets.inc"
};
static NJS_POINT3 collision_197_points[12] = {
#include "stg13/terrain/collision_197_points.inc"
};
static NJS_MODEL collision_197_model = {
#include "stg13/terrain/collision_197_model.inc"
};
static NJS_OBJECT collision_197 = {
#include "stg13/terrain/collision_197.inc"
};
static NJS_MATERIAL collision_198_materials[1] = {
#include "stg13/terrain/collision_198_materials.inc"
};
static Sint16 collision_198_faces[36] = {
#include "stg13/terrain/collision_198_faces.inc"
};
static NJS_MESHSET collision_198_meshsets[1] = {
#include "stg13/terrain/collision_198_meshsets.inc"
};
static NJS_POINT3 collision_198_points[24] = {
#include "stg13/terrain/collision_198_points.inc"
};
static NJS_MODEL collision_198_model = {
#include "stg13/terrain/collision_198_model.inc"
};
static NJS_OBJECT collision_198 = {
#include "stg13/terrain/collision_198.inc"
};
static NJS_MATERIAL collision_199_materials[1] = {
#include "stg13/terrain/collision_199_materials.inc"
};
static Sint16 collision_199_faces[36] = {
#include "stg13/terrain/collision_199_faces.inc"
};
static NJS_MESHSET collision_199_meshsets[1] = {
#include "stg13/terrain/collision_199_meshsets.inc"
};
static NJS_POINT3 collision_199_points[24] = {
#include "stg13/terrain/collision_199_points.inc"
};
static NJS_MODEL collision_199_model = {
#include "stg13/terrain/collision_199_model.inc"
};
static NJS_OBJECT collision_199 = {
#include "stg13/terrain/collision_199.inc"
};
static NJS_MATERIAL collision_200_materials[1] = {
#include "stg13/terrain/collision_200_materials.inc"
};
static Sint16 collision_200_faces[36] = {
#include "stg13/terrain/collision_200_faces.inc"
};
static NJS_MESHSET collision_200_meshsets[1] = {
#include "stg13/terrain/collision_200_meshsets.inc"
};
static NJS_POINT3 collision_200_points[24] = {
#include "stg13/terrain/collision_200_points.inc"
};
static NJS_MODEL collision_200_model = {
#include "stg13/terrain/collision_200_model.inc"
};
static NJS_OBJECT collision_200 = {
#include "stg13/terrain/collision_200.inc"
};
static NJS_MATERIAL collision_201_materials[1] = {
#include "stg13/terrain/collision_201_materials.inc"
};
static Sint16 collision_201_faces[33] = {
#include "stg13/terrain/collision_201_faces.inc"
};
static NJS_MESHSET collision_201_meshsets[1] = {
#include "stg13/terrain/collision_201_meshsets.inc"
};
static NJS_POINT3 collision_201_points[17] = {
#include "stg13/terrain/collision_201_points.inc"
};
static NJS_MODEL collision_201_model = {
#include "stg13/terrain/collision_201_model.inc"
};
static NJS_OBJECT collision_201 = {
#include "stg13/terrain/collision_201.inc"
};
static NJS_MATERIAL collision_202_materials[1] = {
#include "stg13/terrain/collision_202_materials.inc"
};
static Sint16 collision_202_faces[36] = {
#include "stg13/terrain/collision_202_faces.inc"
};
static NJS_MESHSET collision_202_meshsets[1] = {
#include "stg13/terrain/collision_202_meshsets.inc"
};
static NJS_POINT3 collision_202_points[25] = {
#include "stg13/terrain/collision_202_points.inc"
};
static NJS_MODEL collision_202_model = {
#include "stg13/terrain/collision_202_model.inc"
};
static NJS_OBJECT collision_202 = {
#include "stg13/terrain/collision_202.inc"
};
static NJS_MATERIAL collision_203_materials[1] = {
#include "stg13/terrain/collision_203_materials.inc"
};
static Sint16 collision_203_faces[23] = {
#include "stg13/terrain/collision_203_faces.inc"
};
static NJS_MESHSET collision_203_meshsets[1] = {
#include "stg13/terrain/collision_203_meshsets.inc"
};
static NJS_POINT3 collision_203_points[11] = {
#include "stg13/terrain/collision_203_points.inc"
};
static NJS_MODEL collision_203_model = {
#include "stg13/terrain/collision_203_model.inc"
};
static NJS_OBJECT collision_203 = {
#include "stg13/terrain/collision_203.inc"
};
static NJS_MATERIAL collision_204_materials[1] = {
#include "stg13/terrain/collision_204_materials.inc"
};
static Sint16 collision_204_faces[28] = {
#include "stg13/terrain/collision_204_faces.inc"
};
static NJS_MESHSET collision_204_meshsets[1] = {
#include "stg13/terrain/collision_204_meshsets.inc"
};
static NJS_POINT3 collision_204_points[18] = {
#include "stg13/terrain/collision_204_points.inc"
};
static NJS_MODEL collision_204_model = {
#include "stg13/terrain/collision_204_model.inc"
};
static NJS_OBJECT collision_204 = {
#include "stg13/terrain/collision_204.inc"
};
static NJS_MATERIAL collision_205_materials[1] = {
#include "stg13/terrain/collision_205_materials.inc"
};
static Sint16 collision_205_faces[26] = {
#include "stg13/terrain/collision_205_faces.inc"
};
static NJS_MESHSET collision_205_meshsets[1] = {
#include "stg13/terrain/collision_205_meshsets.inc"
};
static NJS_POINT3 collision_205_points[20] = {
#include "stg13/terrain/collision_205_points.inc"
};
static NJS_MODEL collision_205_model = {
#include "stg13/terrain/collision_205_model.inc"
};
static NJS_OBJECT collision_205 = {
#include "stg13/terrain/collision_205.inc"
};
static NJS_MATERIAL collision_206_materials[1] = {
#include "stg13/terrain/collision_206_materials.inc"
};
static Sint16 collision_206_faces[52] = {
#include "stg13/terrain/collision_206_faces.inc"
};
static NJS_MESHSET collision_206_meshsets[1] = {
#include "stg13/terrain/collision_206_meshsets.inc"
};
static NJS_POINT3 collision_206_points[35] = {
#include "stg13/terrain/collision_206_points.inc"
};
static NJS_MODEL collision_206_model = {
#include "stg13/terrain/collision_206_model.inc"
};
static NJS_OBJECT collision_206 = {
#include "stg13/terrain/collision_206.inc"
};
static NJS_MATERIAL collision_207_materials[1] = {
#include "stg13/terrain/collision_207_materials.inc"
};
static Sint16 collision_207_faces[37] = {
#include "stg13/terrain/collision_207_faces.inc"
};
static NJS_MESHSET collision_207_meshsets[1] = {
#include "stg13/terrain/collision_207_meshsets.inc"
};
static NJS_POINT3 collision_207_points[24] = {
#include "stg13/terrain/collision_207_points.inc"
};
static NJS_MODEL collision_207_model = {
#include "stg13/terrain/collision_207_model.inc"
};
static NJS_OBJECT collision_207 = {
#include "stg13/terrain/collision_207.inc"
};
static NJS_MATERIAL collision_223_materials[1] = {
#include "stg13/terrain/collision_223_materials.inc"
};
static Sint16 collision_223_faces[33] = {
#include "stg13/terrain/collision_223_faces.inc"
};
static NJS_MESHSET collision_223_meshsets[1] = {
#include "stg13/terrain/collision_223_meshsets.inc"
};
static NJS_POINT3 collision_223_points[16] = {
#include "stg13/terrain/collision_223_points.inc"
};
static NJS_MODEL collision_223_model = {
#include "stg13/terrain/collision_223_model.inc"
};
static NJS_OBJECT collision_223 = {
#include "stg13/terrain/collision_223.inc"
};
static NJS_MATERIAL collision_227_materials[1] = {
#include "stg13/terrain/collision_227_materials.inc"
};
static Sint16 collision_227_faces[24] = {
#include "stg13/terrain/collision_227_faces.inc"
};
static NJS_MESHSET collision_227_meshsets[1] = {
#include "stg13/terrain/collision_227_meshsets.inc"
};
static NJS_POINT3 collision_227_points[16] = {
#include "stg13/terrain/collision_227_points.inc"
};
static NJS_MODEL collision_227_model = {
#include "stg13/terrain/collision_227_model.inc"
};
static NJS_OBJECT collision_227 = {
#include "stg13/terrain/collision_227.inc"
};
static NJS_MATERIAL collision_228_materials[1] = {
#include "stg13/terrain/collision_228_materials.inc"
};
static Sint16 collision_228_faces[14] = {
#include "stg13/terrain/collision_228_faces.inc"
};
static NJS_MESHSET collision_228_meshsets[1] = {
#include "stg13/terrain/collision_228_meshsets.inc"
};
static NJS_POINT3 collision_228_points[12] = {
#include "stg13/terrain/collision_228_points.inc"
};
static NJS_MODEL collision_228_model = {
#include "stg13/terrain/collision_228_model.inc"
};
static NJS_OBJECT collision_228 = {
#include "stg13/terrain/collision_228.inc"
};
static NJS_MATERIAL collision_229_materials[1] = {
#include "stg13/terrain/collision_229_materials.inc"
};
static Sint16 collision_229_faces[29] = {
#include "stg13/terrain/collision_229_faces.inc"
};
static NJS_MESHSET collision_229_meshsets[1] = {
#include "stg13/terrain/collision_229_meshsets.inc"
};
static NJS_POINT3 collision_229_points[18] = {
#include "stg13/terrain/collision_229_points.inc"
};
static NJS_MODEL collision_229_model = {
#include "stg13/terrain/collision_229_model.inc"
};
static NJS_OBJECT collision_229 = {
#include "stg13/terrain/collision_229.inc"
};
static NJS_MATERIAL collision_230_materials[1] = {
#include "stg13/terrain/collision_230_materials.inc"
};
static Sint16 collision_230_faces[37] = {
#include "stg13/terrain/collision_230_faces.inc"
};
static NJS_MESHSET collision_230_meshsets[1] = {
#include "stg13/terrain/collision_230_meshsets.inc"
};
static NJS_POINT3 collision_230_points[24] = {
#include "stg13/terrain/collision_230_points.inc"
};
static NJS_MODEL collision_230_model = {
#include "stg13/terrain/collision_230_model.inc"
};
static NJS_OBJECT collision_230 = {
#include "stg13/terrain/collision_230.inc"
};
static NJS_MATERIAL collision_231_materials[1] = {
#include "stg13/terrain/collision_231_materials.inc"
};
static Sint16 collision_231_faces[62] = {
#include "stg13/terrain/collision_231_faces.inc"
};
static NJS_MESHSET collision_231_meshsets[1] = {
#include "stg13/terrain/collision_231_meshsets.inc"
};
static NJS_POINT3 collision_231_points[35] = {
#include "stg13/terrain/collision_231_points.inc"
};
static NJS_MODEL collision_231_model = {
#include "stg13/terrain/collision_231_model.inc"
};
static NJS_OBJECT collision_231 = {
#include "stg13/terrain/collision_231.inc"
};
static NJS_MATERIAL collision_232_materials[1] = {
#include "stg13/terrain/collision_232_materials.inc"
};
static Sint16 collision_232_faces[56] = {
#include "stg13/terrain/collision_232_faces.inc"
};
static NJS_MESHSET collision_232_meshsets[1] = {
#include "stg13/terrain/collision_232_meshsets.inc"
};
static NJS_POINT3 collision_232_points[35] = {
#include "stg13/terrain/collision_232_points.inc"
};
static NJS_MODEL collision_232_model = {
#include "stg13/terrain/collision_232_model.inc"
};
static NJS_OBJECT collision_232 = {
#include "stg13/terrain/collision_232.inc"
};
static NJS_MATERIAL collision_233_materials[1] = {
#include "stg13/terrain/collision_233_materials.inc"
};
static Sint16 collision_233_faces[34] = {
#include "stg13/terrain/collision_233_faces.inc"
};
static NJS_MESHSET collision_233_meshsets[1] = {
#include "stg13/terrain/collision_233_meshsets.inc"
};
static NJS_POINT3 collision_233_points[28] = {
#include "stg13/terrain/collision_233_points.inc"
};
static NJS_MODEL collision_233_model = {
#include "stg13/terrain/collision_233_model.inc"
};
static NJS_OBJECT collision_233 = {
#include "stg13/terrain/collision_233.inc"
};
static NJS_MATERIAL collision_234_materials[1] = {
#include "stg13/terrain/collision_234_materials.inc"
};
static Sint16 collision_234_faces[52] = {
#include "stg13/terrain/collision_234_faces.inc"
};
static NJS_MESHSET collision_234_meshsets[1] = {
#include "stg13/terrain/collision_234_meshsets.inc"
};
static NJS_POINT3 collision_234_points[25] = {
#include "stg13/terrain/collision_234_points.inc"
};
static NJS_MODEL collision_234_model = {
#include "stg13/terrain/collision_234_model.inc"
};
static NJS_OBJECT collision_234 = {
#include "stg13/terrain/collision_234.inc"
};
static NJS_MATERIAL collision_235_materials[1] = {
#include "stg13/terrain/collision_235_materials.inc"
};
static Sint16 collision_235_faces[9] = {
#include "stg13/terrain/collision_235_faces.inc"
};
static NJS_MESHSET collision_235_meshsets[1] = {
#include "stg13/terrain/collision_235_meshsets.inc"
};
static NJS_POINT3 collision_235_points[8] = {
#include "stg13/terrain/collision_235_points.inc"
};
static NJS_MODEL collision_235_model = {
#include "stg13/terrain/collision_235_model.inc"
};
static NJS_OBJECT collision_235 = {
#include "stg13/terrain/collision_235.inc"
};
static NJS_MATERIAL collision_236_materials[1] = {
#include "stg13/terrain/collision_236_materials.inc"
};
static Sint16 collision_236_faces[9] = {
#include "stg13/terrain/collision_236_faces.inc"
};
static NJS_MESHSET collision_236_meshsets[1] = {
#include "stg13/terrain/collision_236_meshsets.inc"
};
static NJS_POINT3 collision_236_points[8] = {
#include "stg13/terrain/collision_236_points.inc"
};
static NJS_MODEL collision_236_model = {
#include "stg13/terrain/collision_236_model.inc"
};
static NJS_OBJECT collision_236 = {
#include "stg13/terrain/collision_236.inc"
};
static NJS_MATERIAL collision_237_materials[1] = {
#include "stg13/terrain/collision_237_materials.inc"
};
static Sint16 collision_237_faces[18] = {
#include "stg13/terrain/collision_237_faces.inc"
};
static NJS_MESHSET collision_237_meshsets[1] = {
#include "stg13/terrain/collision_237_meshsets.inc"
};
static NJS_POINT3 collision_237_points[10] = {
#include "stg13/terrain/collision_237_points.inc"
};
static NJS_MODEL collision_237_model = {
#include "stg13/terrain/collision_237_model.inc"
};
static NJS_OBJECT collision_237 = {
#include "stg13/terrain/collision_237.inc"
};
static NJS_MATERIAL collision_238_materials[1] = {
#include "stg13/terrain/collision_238_materials.inc"
};
static Sint16 collision_238_faces[57] = {
#include "stg13/terrain/collision_238_faces.inc"
};
static NJS_MESHSET collision_238_meshsets[1] = {
#include "stg13/terrain/collision_238_meshsets.inc"
};
static NJS_POINT3 collision_238_points[41] = {
#include "stg13/terrain/collision_238_points.inc"
};
static NJS_MODEL collision_238_model = {
#include "stg13/terrain/collision_238_model.inc"
};
static NJS_OBJECT collision_238 = {
#include "stg13/terrain/collision_238.inc"
};
static NJS_MATERIAL collision_239_materials[1] = {
#include "stg13/terrain/collision_239_materials.inc"
};
static Sint16 collision_239_faces[78] = {
#include "stg13/terrain/collision_239_faces.inc"
};
static NJS_MESHSET collision_239_meshsets[1] = {
#include "stg13/terrain/collision_239_meshsets.inc"
};
static NJS_POINT3 collision_239_points[45] = {
#include "stg13/terrain/collision_239_points.inc"
};
static NJS_MODEL collision_239_model = {
#include "stg13/terrain/collision_239_model.inc"
};
static NJS_OBJECT collision_239 = {
#include "stg13/terrain/collision_239.inc"
};
static NJS_MATERIAL collision_240_materials[1] = {
#include "stg13/terrain/collision_240_materials.inc"
};
static Sint16 collision_240_faces[10] = {
#include "stg13/terrain/collision_240_faces.inc"
};
static NJS_MESHSET collision_240_meshsets[1] = {
#include "stg13/terrain/collision_240_meshsets.inc"
};
static NJS_POINT3 collision_240_points[8] = {
#include "stg13/terrain/collision_240_points.inc"
};
static NJS_MODEL collision_240_model = {
#include "stg13/terrain/collision_240_model.inc"
};
static NJS_OBJECT collision_240 = {
#include "stg13/terrain/collision_240.inc"
};
static NJS_MATERIAL collision_241_materials[1] = {
#include "stg13/terrain/collision_241_materials.inc"
};
static Sint16 collision_241_faces[24] = {
#include "stg13/terrain/collision_241_faces.inc"
};
static NJS_MESHSET collision_241_meshsets[1] = {
#include "stg13/terrain/collision_241_meshsets.inc"
};
static NJS_POINT3 collision_241_points[16] = {
#include "stg13/terrain/collision_241_points.inc"
};
static NJS_MODEL collision_241_model = {
#include "stg13/terrain/collision_241_model.inc"
};
static NJS_OBJECT collision_241 = {
#include "stg13/terrain/collision_241.inc"
};
static NJS_MATERIAL collision_242_materials[1] = {
#include "stg13/terrain/collision_242_materials.inc"
};
static Sint16 collision_242_faces[10] = {
#include "stg13/terrain/collision_242_faces.inc"
};
static NJS_MESHSET collision_242_meshsets[1] = {
#include "stg13/terrain/collision_242_meshsets.inc"
};
static NJS_POINT3 collision_242_points[8] = {
#include "stg13/terrain/collision_242_points.inc"
};
static NJS_MODEL collision_242_model = {
#include "stg13/terrain/collision_242_model.inc"
};
static NJS_OBJECT collision_242 = {
#include "stg13/terrain/collision_242.inc"
};
static NJS_MATERIAL collision_245_materials[1] = {
#include "stg13/terrain/collision_245_materials.inc"
};
static Sint16 collision_245_faces[94] = {
#include "stg13/terrain/collision_245_faces.inc"
};
static NJS_MESHSET collision_245_meshsets[1] = {
#include "stg13/terrain/collision_245_meshsets.inc"
};
static NJS_POINT3 collision_245_points[48] = {
#include "stg13/terrain/collision_245_points.inc"
};
static NJS_MODEL collision_245_model = {
#include "stg13/terrain/collision_245_model.inc"
};
static NJS_OBJECT collision_245 = {
#include "stg13/terrain/collision_245.inc"
};
static NJS_MATERIAL collision_253_materials[1] = {
#include "stg13/terrain/collision_253_materials.inc"
};
static Sint16 collision_253_faces[24] = {
#include "stg13/terrain/collision_253_faces.inc"
};
static NJS_MESHSET collision_253_meshsets[1] = {
#include "stg13/terrain/collision_253_meshsets.inc"
};
static NJS_POINT3 collision_253_points[14] = {
#include "stg13/terrain/collision_253_points.inc"
};
static NJS_MODEL collision_253_model = {
#include "stg13/terrain/collision_253_model.inc"
};
static NJS_OBJECT collision_253 = {
#include "stg13/terrain/collision_253.inc"
};
static NJS_MATERIAL collision_254_materials[1] = {
#include "stg13/terrain/collision_254_materials.inc"
};
static Sint16 collision_254_faces[49] = {
#include "stg13/terrain/collision_254_faces.inc"
};
static NJS_MESHSET collision_254_meshsets[1] = {
#include "stg13/terrain/collision_254_meshsets.inc"
};
static NJS_POINT3 collision_254_points[26] = {
#include "stg13/terrain/collision_254_points.inc"
};
static NJS_MODEL collision_254_model = {
#include "stg13/terrain/collision_254_model.inc"
};
static NJS_OBJECT collision_254 = {
#include "stg13/terrain/collision_254.inc"
};
static NJS_MATERIAL collision_255_materials[1] = {
#include "stg13/terrain/collision_255_materials.inc"
};
static Sint16 collision_255_faces[94] = {
#include "stg13/terrain/collision_255_faces.inc"
};
static NJS_MESHSET collision_255_meshsets[1] = {
#include "stg13/terrain/collision_255_meshsets.inc"
};
static NJS_POINT3 collision_255_points[48] = {
#include "stg13/terrain/collision_255_points.inc"
};
static NJS_MODEL collision_255_model = {
#include "stg13/terrain/collision_255_model.inc"
};
static NJS_OBJECT collision_255 = {
#include "stg13/terrain/collision_255.inc"
};
static NJS_MATERIAL collision_263_materials[1] = {
#include "stg13/terrain/collision_263_materials.inc"
};
static Sint16 collision_263_faces[23] = {
#include "stg13/terrain/collision_263_faces.inc"
};
static NJS_MESHSET collision_263_meshsets[1] = {
#include "stg13/terrain/collision_263_meshsets.inc"
};
static NJS_POINT3 collision_263_points[14] = {
#include "stg13/terrain/collision_263_points.inc"
};
static NJS_MODEL collision_263_model = {
#include "stg13/terrain/collision_263_model.inc"
};
static NJS_OBJECT collision_263 = {
#include "stg13/terrain/collision_263.inc"
};
static NJS_MATERIAL collision_264_materials[1] = {
#include "stg13/terrain/collision_264_materials.inc"
};
static Sint16 collision_264_faces[36] = {
#include "stg13/terrain/collision_264_faces.inc"
};
static NJS_MESHSET collision_264_meshsets[1] = {
#include "stg13/terrain/collision_264_meshsets.inc"
};
static NJS_POINT3 collision_264_points[24] = {
#include "stg13/terrain/collision_264_points.inc"
};
static NJS_MODEL collision_264_model = {
#include "stg13/terrain/collision_264_model.inc"
};
static NJS_OBJECT collision_264 = {
#include "stg13/terrain/collision_264.inc"
};
static NJS_MATERIAL collision_265_materials[1] = {
#include "stg13/terrain/collision_265_materials.inc"
};
static Sint16 collision_265_faces[24] = {
#include "stg13/terrain/collision_265_faces.inc"
};
static NJS_MESHSET collision_265_meshsets[1] = {
#include "stg13/terrain/collision_265_meshsets.inc"
};
static NJS_POINT3 collision_265_points[16] = {
#include "stg13/terrain/collision_265_points.inc"
};
static NJS_MODEL collision_265_model = {
#include "stg13/terrain/collision_265_model.inc"
};
static NJS_OBJECT collision_265 = {
#include "stg13/terrain/collision_265.inc"
};
static NJS_MATERIAL collision_266_materials[1] = {
#include "stg13/terrain/collision_266_materials.inc"
};
static Sint16 collision_266_faces[24] = {
#include "stg13/terrain/collision_266_faces.inc"
};
static NJS_MESHSET collision_266_meshsets[1] = {
#include "stg13/terrain/collision_266_meshsets.inc"
};
static NJS_POINT3 collision_266_points[17] = {
#include "stg13/terrain/collision_266_points.inc"
};
static NJS_MODEL collision_266_model = {
#include "stg13/terrain/collision_266_model.inc"
};
static NJS_OBJECT collision_266 = {
#include "stg13/terrain/collision_266.inc"
};
static NJS_MATERIAL collision_267_materials[1] = {
#include "stg13/terrain/collision_267_materials.inc"
};
static Sint16 collision_267_faces[22] = {
#include "stg13/terrain/collision_267_faces.inc"
};
static NJS_MESHSET collision_267_meshsets[1] = {
#include "stg13/terrain/collision_267_meshsets.inc"
};
static NJS_POINT3 collision_267_points[13] = {
#include "stg13/terrain/collision_267_points.inc"
};
static NJS_MODEL collision_267_model = {
#include "stg13/terrain/collision_267_model.inc"
};
static NJS_OBJECT collision_267 = {
#include "stg13/terrain/collision_267.inc"
};
static NJS_MATERIAL collision_268_materials[1] = {
#include "stg13/terrain/collision_268_materials.inc"
};
static Sint16 collision_268_faces[15] = {
#include "stg13/terrain/collision_268_faces.inc"
};
static NJS_MESHSET collision_268_meshsets[1] = {
#include "stg13/terrain/collision_268_meshsets.inc"
};
static NJS_POINT3 collision_268_points[8] = {
#include "stg13/terrain/collision_268_points.inc"
};
static NJS_MODEL collision_268_model = {
#include "stg13/terrain/collision_268_model.inc"
};
static NJS_OBJECT collision_268 = {
#include "stg13/terrain/collision_268.inc"
};
static NJS_MATERIAL collision_269_materials[1] = {
#include "stg13/terrain/collision_269_materials.inc"
};
static Sint16 collision_269_faces[24] = {
#include "stg13/terrain/collision_269_faces.inc"
};
static NJS_MESHSET collision_269_meshsets[1] = {
#include "stg13/terrain/collision_269_meshsets.inc"
};
static NJS_POINT3 collision_269_points[17] = {
#include "stg13/terrain/collision_269_points.inc"
};
static NJS_MODEL collision_269_model = {
#include "stg13/terrain/collision_269_model.inc"
};
static NJS_OBJECT collision_269 = {
#include "stg13/terrain/collision_269.inc"
};
static NJS_MATERIAL collision_270_materials[1] = {
#include "stg13/terrain/collision_270_materials.inc"
};
static Sint16 collision_270_faces[22] = {
#include "stg13/terrain/collision_270_faces.inc"
};
static NJS_MESHSET collision_270_meshsets[1] = {
#include "stg13/terrain/collision_270_meshsets.inc"
};
static NJS_POINT3 collision_270_points[13] = {
#include "stg13/terrain/collision_270_points.inc"
};
static NJS_MODEL collision_270_model = {
#include "stg13/terrain/collision_270_model.inc"
};
static NJS_OBJECT collision_270 = {
#include "stg13/terrain/collision_270.inc"
};
static NJS_MATERIAL collision_271_materials[1] = {
#include "stg13/terrain/collision_271_materials.inc"
};
static Sint16 collision_271_faces[11] = {
#include "stg13/terrain/collision_271_faces.inc"
};
static NJS_MESHSET collision_271_meshsets[1] = {
#include "stg13/terrain/collision_271_meshsets.inc"
};
static NJS_POINT3 collision_271_points[10] = {
#include "stg13/terrain/collision_271_points.inc"
};
static NJS_MODEL collision_271_model = {
#include "stg13/terrain/collision_271_model.inc"
};
static NJS_OBJECT collision_271 = {
#include "stg13/terrain/collision_271.inc"
};
static NJS_MATERIAL collision_272_materials[1] = {
#include "stg13/terrain/collision_272_materials.inc"
};
static Sint16 collision_272_faces[12] = {
#include "stg13/terrain/collision_272_faces.inc"
};
static NJS_MESHSET collision_272_meshsets[1] = {
#include "stg13/terrain/collision_272_meshsets.inc"
};
static NJS_POINT3 collision_272_points[8] = {
#include "stg13/terrain/collision_272_points.inc"
};
static NJS_MODEL collision_272_model = {
#include "stg13/terrain/collision_272_model.inc"
};
static NJS_OBJECT collision_272 = {
#include "stg13/terrain/collision_272.inc"
};
static NJS_MATERIAL collision_273_materials[1] = {
#include "stg13/terrain/collision_273_materials.inc"
};
static Sint16 collision_273_faces[11] = {
#include "stg13/terrain/collision_273_faces.inc"
};
static NJS_MESHSET collision_273_meshsets[1] = {
#include "stg13/terrain/collision_273_meshsets.inc"
};
static NJS_POINT3 collision_273_points[10] = {
#include "stg13/terrain/collision_273_points.inc"
};
static NJS_MODEL collision_273_model = {
#include "stg13/terrain/collision_273_model.inc"
};
static NJS_OBJECT collision_273 = {
#include "stg13/terrain/collision_273.inc"
};
static NJS_MATERIAL collision_274_materials[1] = {
#include "stg13/terrain/collision_274_materials.inc"
};
static Sint16 collision_274_faces[15] = {
#include "stg13/terrain/collision_274_faces.inc"
};
static NJS_MESHSET collision_274_meshsets[1] = {
#include "stg13/terrain/collision_274_meshsets.inc"
};
static NJS_POINT3 collision_274_points[8] = {
#include "stg13/terrain/collision_274_points.inc"
};
static NJS_MODEL collision_274_model = {
#include "stg13/terrain/collision_274_model.inc"
};
static NJS_OBJECT collision_274 = {
#include "stg13/terrain/collision_274.inc"
};
static NJS_MATERIAL collision_276_materials[1] = {
#include "stg13/terrain/collision_276_materials.inc"
};
static Sint16 collision_276_faces[17] = {
#include "stg13/terrain/collision_276_faces.inc"
};
static NJS_MESHSET collision_276_meshsets[1] = {
#include "stg13/terrain/collision_276_meshsets.inc"
};
static NJS_POINT3 collision_276_points[16] = {
#include "stg13/terrain/collision_276_points.inc"
};
static NJS_MODEL collision_276_model = {
#include "stg13/terrain/collision_276_model.inc"
};
static NJS_OBJECT collision_276 = {
#include "stg13/terrain/collision_276.inc"
};
static NJS_MATERIAL collision_278_materials[1] = {
#include "stg13/terrain/collision_278_materials.inc"
};
static Sint16 collision_278_faces[36] = {
#include "stg13/terrain/collision_278_faces.inc"
};
static NJS_MESHSET collision_278_meshsets[1] = {
#include "stg13/terrain/collision_278_meshsets.inc"
};
static NJS_POINT3 collision_278_points[24] = {
#include "stg13/terrain/collision_278_points.inc"
};
static NJS_MODEL collision_278_model = {
#include "stg13/terrain/collision_278_model.inc"
};
static NJS_OBJECT collision_278 = {
#include "stg13/terrain/collision_278.inc"
};
static NJS_MATERIAL collision_279_materials[1] = {
#include "stg13/terrain/collision_279_materials.inc"
};
static Sint16 collision_279_faces[36] = {
#include "stg13/terrain/collision_279_faces.inc"
};
static NJS_MESHSET collision_279_meshsets[1] = {
#include "stg13/terrain/collision_279_meshsets.inc"
};
static NJS_POINT3 collision_279_points[24] = {
#include "stg13/terrain/collision_279_points.inc"
};
static NJS_MODEL collision_279_model = {
#include "stg13/terrain/collision_279_model.inc"
};
static NJS_OBJECT collision_279 = {
#include "stg13/terrain/collision_279.inc"
};
static NJS_MATERIAL collision_281_materials[1] = {
#include "stg13/terrain/collision_281_materials.inc"
};
static Sint16 collision_281_faces[74] = {
#include "stg13/terrain/collision_281_faces.inc"
};
static NJS_MESHSET collision_281_meshsets[1] = {
#include "stg13/terrain/collision_281_meshsets.inc"
};
static NJS_POINT3 collision_281_points[48] = {
#include "stg13/terrain/collision_281_points.inc"
};
static NJS_MODEL collision_281_model = {
#include "stg13/terrain/collision_281_model.inc"
};
static NJS_OBJECT collision_281 = {
#include "stg13/terrain/collision_281.inc"
};
static NJS_MATERIAL collision_282_materials[1] = {
#include "stg13/terrain/collision_282_materials.inc"
};
static Sint16 collision_282_faces[19] = {
#include "stg13/terrain/collision_282_faces.inc"
};
static NJS_MESHSET collision_282_meshsets[1] = {
#include "stg13/terrain/collision_282_meshsets.inc"
};
static NJS_POINT3 collision_282_points[11] = {
#include "stg13/terrain/collision_282_points.inc"
};
static NJS_MODEL collision_282_model = {
#include "stg13/terrain/collision_282_model.inc"
};
static NJS_OBJECT collision_282 = {
#include "stg13/terrain/collision_282.inc"
};
static NJS_MATERIAL collision_283_materials[1] = {
#include "stg13/terrain/collision_283_materials.inc"
};
static Sint16 collision_283_faces[28] = {
#include "stg13/terrain/collision_283_faces.inc"
};
static NJS_MESHSET collision_283_meshsets[1] = {
#include "stg13/terrain/collision_283_meshsets.inc"
};
static NJS_POINT3 collision_283_points[18] = {
#include "stg13/terrain/collision_283_points.inc"
};
static NJS_MODEL collision_283_model = {
#include "stg13/terrain/collision_283_model.inc"
};
static NJS_OBJECT collision_283 = {
#include "stg13/terrain/collision_283.inc"
};
static NJS_MATERIAL collision_284_materials[1] = {
#include "stg13/terrain/collision_284_materials.inc"
};
static Sint16 collision_284_faces[10] = {
#include "stg13/terrain/collision_284_faces.inc"
};
static NJS_MESHSET collision_284_meshsets[1] = {
#include "stg13/terrain/collision_284_meshsets.inc"
};
static NJS_POINT3 collision_284_points[8] = {
#include "stg13/terrain/collision_284_points.inc"
};
static NJS_MODEL collision_284_model = {
#include "stg13/terrain/collision_284_model.inc"
};
static NJS_OBJECT collision_284 = {
#include "stg13/terrain/collision_284.inc"
};
static NJS_MATERIAL collision_285_materials[1] = {
#include "stg13/terrain/collision_285_materials.inc"
};
static Sint16 collision_285_faces[23] = {
#include "stg13/terrain/collision_285_faces.inc"
};
static NJS_MESHSET collision_285_meshsets[1] = {
#include "stg13/terrain/collision_285_meshsets.inc"
};
static NJS_POINT3 collision_285_points[13] = {
#include "stg13/terrain/collision_285_points.inc"
};
static NJS_MODEL collision_285_model = {
#include "stg13/terrain/collision_285_model.inc"
};
static NJS_OBJECT collision_285 = {
#include "stg13/terrain/collision_285.inc"
};
static NJS_MATERIAL collision_286_materials[1] = {
#include "stg13/terrain/collision_286_materials.inc"
};
static Sint16 collision_286_faces[19] = {
#include "stg13/terrain/collision_286_faces.inc"
};
static NJS_MESHSET collision_286_meshsets[1] = {
#include "stg13/terrain/collision_286_meshsets.inc"
};
static NJS_POINT3 collision_286_points[12] = {
#include "stg13/terrain/collision_286_points.inc"
};
static NJS_MODEL collision_286_model = {
#include "stg13/terrain/collision_286_model.inc"
};
static NJS_OBJECT collision_286 = {
#include "stg13/terrain/collision_286.inc"
};
static NJS_MATERIAL collision_287_materials[1] = {
#include "stg13/terrain/collision_287_materials.inc"
};
static Sint16 collision_287_faces[23] = {
#include "stg13/terrain/collision_287_faces.inc"
};
static NJS_MESHSET collision_287_meshsets[1] = {
#include "stg13/terrain/collision_287_meshsets.inc"
};
static NJS_POINT3 collision_287_points[13] = {
#include "stg13/terrain/collision_287_points.inc"
};
static NJS_MODEL collision_287_model = {
#include "stg13/terrain/collision_287_model.inc"
};
static NJS_OBJECT collision_287 = {
#include "stg13/terrain/collision_287.inc"
};
static NJS_MATERIAL collision_288_materials[1] = {
#include "stg13/terrain/collision_288_materials.inc"
};
static Sint16 collision_288_faces[11] = {
#include "stg13/terrain/collision_288_faces.inc"
};
static NJS_MESHSET collision_288_meshsets[1] = {
#include "stg13/terrain/collision_288_meshsets.inc"
};
static NJS_POINT3 collision_288_points[8] = {
#include "stg13/terrain/collision_288_points.inc"
};
static NJS_MODEL collision_288_model = {
#include "stg13/terrain/collision_288_model.inc"
};
static NJS_OBJECT collision_288 = {
#include "stg13/terrain/collision_288.inc"
};
static NJS_MATERIAL collision_291_materials[1] = {
#include "stg13/terrain/collision_291_materials.inc"
};
static Sint16 collision_291_faces[7] = {
#include "stg13/terrain/collision_291_faces.inc"
};
static NJS_MESHSET collision_291_meshsets[1] = {
#include "stg13/terrain/collision_291_meshsets.inc"
};
static NJS_POINT3 collision_291_points[6] = {
#include "stg13/terrain/collision_291_points.inc"
};
static NJS_MODEL collision_291_model = {
#include "stg13/terrain/collision_291_model.inc"
};
static NJS_OBJECT collision_291 = {
#include "stg13/terrain/collision_291.inc"
};
static NJS_MATERIAL collision_296_materials[1] = {
#include "stg13/terrain/collision_296_materials.inc"
};
static Sint16 collision_296_faces[19] = {
#include "stg13/terrain/collision_296_faces.inc"
};
static NJS_MESHSET collision_296_meshsets[1] = {
#include "stg13/terrain/collision_296_meshsets.inc"
};
static NJS_POINT3 collision_296_points[16] = {
#include "stg13/terrain/collision_296_points.inc"
};
static NJS_MODEL collision_296_model = {
#include "stg13/terrain/collision_296_model.inc"
};
static NJS_OBJECT collision_296 = {
#include "stg13/terrain/collision_296.inc"
};
static NJS_MATERIAL collision_297_materials[1] = {
#include "stg13/terrain/collision_297_materials.inc"
};
static Sint16 collision_297_faces[49] = {
#include "stg13/terrain/collision_297_faces.inc"
};
static NJS_MESHSET collision_297_meshsets[1] = {
#include "stg13/terrain/collision_297_meshsets.inc"
};
static NJS_POINT3 collision_297_points[25] = {
#include "stg13/terrain/collision_297_points.inc"
};
static NJS_MODEL collision_297_model = {
#include "stg13/terrain/collision_297_model.inc"
};
static NJS_OBJECT collision_297 = {
#include "stg13/terrain/collision_297.inc"
};
static NJS_MATERIAL collision_298_materials[1] = {
#include "stg13/terrain/collision_298_materials.inc"
};
static Sint16 collision_298_faces[29] = {
#include "stg13/terrain/collision_298_faces.inc"
};
static NJS_MESHSET collision_298_meshsets[1] = {
#include "stg13/terrain/collision_298_meshsets.inc"
};
static NJS_POINT3 collision_298_points[16] = {
#include "stg13/terrain/collision_298_points.inc"
};
static NJS_MODEL collision_298_model = {
#include "stg13/terrain/collision_298_model.inc"
};
static NJS_OBJECT collision_298 = {
#include "stg13/terrain/collision_298.inc"
};
static NJS_MATERIAL collision_299_materials[1] = {
#include "stg13/terrain/collision_299_materials.inc"
};
static Sint16 collision_299_faces[5] = {
#include "stg13/terrain/collision_299_faces.inc"
};
static NJS_MESHSET collision_299_meshsets[1] = {
#include "stg13/terrain/collision_299_meshsets.inc"
};
static NJS_POINT3 collision_299_points[4] = {
#include "stg13/terrain/collision_299_points.inc"
};
static NJS_MODEL collision_299_model = {
#include "stg13/terrain/collision_299_model.inc"
};
static NJS_OBJECT collision_299 = {
#include "stg13/terrain/collision_299.inc"
};
static NJS_MATERIAL collision_300_materials[1] = {
#include "stg13/terrain/collision_300_materials.inc"
};
static Sint16 collision_300_faces[27] = {
#include "stg13/terrain/collision_300_faces.inc"
};
static NJS_MESHSET collision_300_meshsets[1] = {
#include "stg13/terrain/collision_300_meshsets.inc"
};
static NJS_POINT3 collision_300_points[23] = {
#include "stg13/terrain/collision_300_points.inc"
};
static NJS_MODEL collision_300_model = {
#include "stg13/terrain/collision_300_model.inc"
};
static NJS_OBJECT collision_300 = {
#include "stg13/terrain/collision_300.inc"
};
static NJS_MATERIAL collision_301_materials[1] = {
#include "stg13/terrain/collision_301_materials.inc"
};
static Sint16 collision_301_faces[12] = {
#include "stg13/terrain/collision_301_faces.inc"
};
static NJS_MESHSET collision_301_meshsets[1] = {
#include "stg13/terrain/collision_301_meshsets.inc"
};
static NJS_POINT3 collision_301_points[5] = {
#include "stg13/terrain/collision_301_points.inc"
};
static NJS_MODEL collision_301_model = {
#include "stg13/terrain/collision_301_model.inc"
};
static NJS_OBJECT collision_301 = {
#include "stg13/terrain/collision_301.inc"
};
static NJS_MATERIAL collision_306_materials[1] = {
#include "stg13/terrain/collision_306_materials.inc"
};
static Sint16 collision_306_faces[12] = {
#include "stg13/terrain/collision_306_faces.inc"
};
static NJS_MESHSET collision_306_meshsets[1] = {
#include "stg13/terrain/collision_306_meshsets.inc"
};
static NJS_POINT3 collision_306_points[5] = {
#include "stg13/terrain/collision_306_points.inc"
};
static NJS_MODEL collision_306_model = {
#include "stg13/terrain/collision_306_model.inc"
};
static NJS_OBJECT collision_306 = {
#include "stg13/terrain/collision_306.inc"
};
static NJS_MATERIAL collision_311_materials[1] = {
#include "stg13/terrain/collision_311_materials.inc"
};
static Sint16 collision_311_faces[84] = {
#include "stg13/terrain/collision_311_faces.inc"
};
static NJS_MESHSET collision_311_meshsets[1] = {
#include "stg13/terrain/collision_311_meshsets.inc"
};
static NJS_POINT3 collision_311_points[48] = {
#include "stg13/terrain/collision_311_points.inc"
};
static NJS_MODEL collision_311_model = {
#include "stg13/terrain/collision_311_model.inc"
};
static NJS_OBJECT collision_311 = {
#include "stg13/terrain/collision_311.inc"
};
static NJS_MATERIAL collision_312_materials[1] = {
#include "stg13/terrain/collision_312_materials.inc"
};
static Sint16 collision_312_faces[7] = {
#include "stg13/terrain/collision_312_faces.inc"
};
static NJS_MESHSET collision_312_meshsets[1] = {
#include "stg13/terrain/collision_312_meshsets.inc"
};
static NJS_POINT3 collision_312_points[6] = {
#include "stg13/terrain/collision_312_points.inc"
};
static NJS_MODEL collision_312_model = {
#include "stg13/terrain/collision_312_model.inc"
};
static NJS_OBJECT collision_312 = {
#include "stg13/terrain/collision_312.inc"
};
static NJS_MATERIAL collision_313_materials[1] = {
#include "stg13/terrain/collision_313_materials.inc"
};
static Sint16 collision_313_faces[7] = {
#include "stg13/terrain/collision_313_faces.inc"
};
static NJS_MESHSET collision_313_meshsets[1] = {
#include "stg13/terrain/collision_313_meshsets.inc"
};
static NJS_POINT3 collision_313_points[6] = {
#include "stg13/terrain/collision_313_points.inc"
};
static NJS_MODEL collision_313_model = {
#include "stg13/terrain/collision_313_model.inc"
};
static NJS_OBJECT collision_313 = {
#include "stg13/terrain/collision_313.inc"
};
static NJS_MATERIAL collision_316_materials[1] = {
#include "stg13/terrain/collision_316_materials.inc"
};
static Sint16 collision_316_faces[25] = {
#include "stg13/terrain/collision_316_faces.inc"
};
static NJS_MESHSET collision_316_meshsets[1] = {
#include "stg13/terrain/collision_316_meshsets.inc"
};
static NJS_POINT3 collision_316_points[21] = {
#include "stg13/terrain/collision_316_points.inc"
};
static NJS_MODEL collision_316_model = {
#include "stg13/terrain/collision_316_model.inc"
};
static NJS_OBJECT collision_316 = {
#include "stg13/terrain/collision_316.inc"
};
static NJS_MATERIAL collision_317_materials[1] = {
#include "stg13/terrain/collision_317_materials.inc"
};
static Sint16 collision_317_faces[38] = {
#include "stg13/terrain/collision_317_faces.inc"
};
static NJS_MESHSET collision_317_meshsets[1] = {
#include "stg13/terrain/collision_317_meshsets.inc"
};
static NJS_POINT3 collision_317_points[23] = {
#include "stg13/terrain/collision_317_points.inc"
};
static NJS_MODEL collision_317_model = {
#include "stg13/terrain/collision_317_model.inc"
};
static NJS_OBJECT collision_317 = {
#include "stg13/terrain/collision_317.inc"
};
static NJS_MATERIAL collision_318_materials[1] = {
#include "stg13/terrain/collision_318_materials.inc"
};
static Sint16 collision_318_faces[81] = {
#include "stg13/terrain/collision_318_faces.inc"
};
static NJS_MESHSET collision_318_meshsets[1] = {
#include "stg13/terrain/collision_318_meshsets.inc"
};
static NJS_POINT3 collision_318_points[48] = {
#include "stg13/terrain/collision_318_points.inc"
};
static NJS_MODEL collision_318_model = {
#include "stg13/terrain/collision_318_model.inc"
};
static NJS_OBJECT collision_318 = {
#include "stg13/terrain/collision_318.inc"
};
static NJS_MATERIAL collision_319_materials[1] = {
#include "stg13/terrain/collision_319_materials.inc"
};
static Sint16 collision_319_faces[29] = {
#include "stg13/terrain/collision_319_faces.inc"
};
static NJS_MESHSET collision_319_meshsets[1] = {
#include "stg13/terrain/collision_319_meshsets.inc"
};
static NJS_POINT3 collision_319_points[14] = {
#include "stg13/terrain/collision_319_points.inc"
};
static NJS_MODEL collision_319_model = {
#include "stg13/terrain/collision_319_model.inc"
};
static NJS_OBJECT collision_319 = {
#include "stg13/terrain/collision_319.inc"
};
static NJS_MATERIAL collision_320_materials[1] = {
#include "stg13/terrain/collision_320_materials.inc"
};
static Sint16 collision_320_faces[7] = {
#include "stg13/terrain/collision_320_faces.inc"
};
static NJS_MESHSET collision_320_meshsets[1] = {
#include "stg13/terrain/collision_320_meshsets.inc"
};
static NJS_POINT3 collision_320_points[6] = {
#include "stg13/terrain/collision_320_points.inc"
};
static NJS_MODEL collision_320_model = {
#include "stg13/terrain/collision_320_model.inc"
};
static NJS_OBJECT collision_320 = {
#include "stg13/terrain/collision_320.inc"
};
static NJS_MATERIAL collision_321_materials[1] = {
#include "stg13/terrain/collision_321_materials.inc"
};
static Sint16 collision_321_faces[20] = {
#include "stg13/terrain/collision_321_faces.inc"
};
static NJS_MESHSET collision_321_meshsets[1] = {
#include "stg13/terrain/collision_321_meshsets.inc"
};
static NJS_POINT3 collision_321_points[12] = {
#include "stg13/terrain/collision_321_points.inc"
};
static NJS_MODEL collision_321_model = {
#include "stg13/terrain/collision_321_model.inc"
};
static NJS_OBJECT collision_321 = {
#include "stg13/terrain/collision_321.inc"
};
static NJS_MATERIAL collision_322_materials[1] = {
#include "stg13/terrain/collision_322_materials.inc"
};
static Sint16 collision_322_faces[9] = {
#include "stg13/terrain/collision_322_faces.inc"
};
static NJS_MESHSET collision_322_meshsets[1] = {
#include "stg13/terrain/collision_322_meshsets.inc"
};
static NJS_POINT3 collision_322_points[8] = {
#include "stg13/terrain/collision_322_points.inc"
};
static NJS_MODEL collision_322_model = {
#include "stg13/terrain/collision_322_model.inc"
};
static NJS_OBJECT collision_322 = {
#include "stg13/terrain/collision_322.inc"
};
static NJS_MATERIAL collision_323_materials[1] = {
#include "stg13/terrain/collision_323_materials.inc"
};
static Sint16 collision_323_faces[7] = {
#include "stg13/terrain/collision_323_faces.inc"
};
static NJS_MESHSET collision_323_meshsets[1] = {
#include "stg13/terrain/collision_323_meshsets.inc"
};
static NJS_POINT3 collision_323_points[6] = {
#include "stg13/terrain/collision_323_points.inc"
};
static NJS_MODEL collision_323_model = {
#include "stg13/terrain/collision_323_model.inc"
};
static NJS_OBJECT collision_323 = {
#include "stg13/terrain/collision_323.inc"
};
static NJS_MATERIAL collision_324_materials[1] = {
#include "stg13/terrain/collision_324_materials.inc"
};
static Sint16 collision_324_faces[35] = {
#include "stg13/terrain/collision_324_faces.inc"
};
static NJS_MESHSET collision_324_meshsets[1] = {
#include "stg13/terrain/collision_324_meshsets.inc"
};
static NJS_POINT3 collision_324_points[20] = {
#include "stg13/terrain/collision_324_points.inc"
};
static NJS_MODEL collision_324_model = {
#include "stg13/terrain/collision_324_model.inc"
};
static NJS_OBJECT collision_324 = {
#include "stg13/terrain/collision_324.inc"
};
static NJS_MATERIAL collision_325_materials[1] = {
#include "stg13/terrain/collision_325_materials.inc"
};
static Sint16 collision_325_faces[17] = {
#include "stg13/terrain/collision_325_faces.inc"
};
static NJS_MESHSET collision_325_meshsets[1] = {
#include "stg13/terrain/collision_325_meshsets.inc"
};
static NJS_POINT3 collision_325_points[11] = {
#include "stg13/terrain/collision_325_points.inc"
};
static NJS_MODEL collision_325_model = {
#include "stg13/terrain/collision_325_model.inc"
};
static NJS_OBJECT collision_325 = {
#include "stg13/terrain/collision_325.inc"
};
static NJS_MATERIAL collision_326_materials[1] = {
#include "stg13/terrain/collision_326_materials.inc"
};
static Sint16 collision_326_faces[35] = {
#include "stg13/terrain/collision_326_faces.inc"
};
static NJS_MESHSET collision_326_meshsets[1] = {
#include "stg13/terrain/collision_326_meshsets.inc"
};
static NJS_POINT3 collision_326_points[20] = {
#include "stg13/terrain/collision_326_points.inc"
};
static NJS_MODEL collision_326_model = {
#include "stg13/terrain/collision_326_model.inc"
};
static NJS_OBJECT collision_326 = {
#include "stg13/terrain/collision_326.inc"
};
static NJS_MATERIAL collision_327_materials[1] = {
#include "stg13/terrain/collision_327_materials.inc"
};
static Sint16 collision_327_faces[11] = {
#include "stg13/terrain/collision_327_faces.inc"
};
static NJS_MESHSET collision_327_meshsets[1] = {
#include "stg13/terrain/collision_327_meshsets.inc"
};
static NJS_POINT3 collision_327_points[10] = {
#include "stg13/terrain/collision_327_points.inc"
};
static NJS_MODEL collision_327_model = {
#include "stg13/terrain/collision_327_model.inc"
};
static NJS_OBJECT collision_327 = {
#include "stg13/terrain/collision_327.inc"
};
static NJS_MATERIAL collision_328_materials[1] = {
#include "stg13/terrain/collision_328_materials.inc"
};
static Sint16 collision_328_faces[25] = {
#include "stg13/terrain/collision_328_faces.inc"
};
static NJS_MESHSET collision_328_meshsets[1] = {
#include "stg13/terrain/collision_328_meshsets.inc"
};
static NJS_POINT3 collision_328_points[14] = {
#include "stg13/terrain/collision_328_points.inc"
};
static NJS_MODEL collision_328_model = {
#include "stg13/terrain/collision_328_model.inc"
};
static NJS_OBJECT collision_328 = {
#include "stg13/terrain/collision_328.inc"
};
static NJS_MATERIAL collision_329_materials[1] = {
#include "stg13/terrain/collision_329_materials.inc"
};
static Sint16 collision_329_faces[22] = {
#include "stg13/terrain/collision_329_faces.inc"
};
static NJS_MESHSET collision_329_meshsets[1] = {
#include "stg13/terrain/collision_329_meshsets.inc"
};
static NJS_POINT3 collision_329_points[15] = {
#include "stg13/terrain/collision_329_points.inc"
};
static NJS_MODEL collision_329_model = {
#include "stg13/terrain/collision_329_model.inc"
};
static NJS_OBJECT collision_329 = {
#include "stg13/terrain/collision_329.inc"
};
static NJS_MATERIAL collision_330_materials[1] = {
#include "stg13/terrain/collision_330_materials.inc"
};
static Sint16 collision_330_faces[9] = {
#include "stg13/terrain/collision_330_faces.inc"
};
static NJS_MESHSET collision_330_meshsets[1] = {
#include "stg13/terrain/collision_330_meshsets.inc"
};
static NJS_POINT3 collision_330_points[8] = {
#include "stg13/terrain/collision_330_points.inc"
};
static NJS_MODEL collision_330_model = {
#include "stg13/terrain/collision_330_model.inc"
};
static NJS_OBJECT collision_330 = {
#include "stg13/terrain/collision_330.inc"
};
static NJS_MATERIAL collision_331_materials[1] = {
#include "stg13/terrain/collision_331_materials.inc"
};
static Sint16 collision_331_faces[7] = {
#include "stg13/terrain/collision_331_faces.inc"
};
static NJS_MESHSET collision_331_meshsets[1] = {
#include "stg13/terrain/collision_331_meshsets.inc"
};
static NJS_POINT3 collision_331_points[6] = {
#include "stg13/terrain/collision_331_points.inc"
};
static NJS_MODEL collision_331_model = {
#include "stg13/terrain/collision_331_model.inc"
};
static NJS_OBJECT collision_331 = {
#include "stg13/terrain/collision_331.inc"
};
static NJS_MATERIAL collision_332_materials[1] = {
#include "stg13/terrain/collision_332_materials.inc"
};
static Sint16 collision_332_faces[18] = {
#include "stg13/terrain/collision_332_faces.inc"
};
static NJS_MESHSET collision_332_meshsets[1] = {
#include "stg13/terrain/collision_332_meshsets.inc"
};
static NJS_POINT3 collision_332_points[12] = {
#include "stg13/terrain/collision_332_points.inc"
};
static NJS_MODEL collision_332_model = {
#include "stg13/terrain/collision_332_model.inc"
};
static NJS_OBJECT collision_332 = {
#include "stg13/terrain/collision_332.inc"
};
static NJS_MATERIAL collision_333_materials[1] = {
#include "stg13/terrain/collision_333_materials.inc"
};
static Sint16 collision_333_faces[18] = {
#include "stg13/terrain/collision_333_faces.inc"
};
static NJS_MESHSET collision_333_meshsets[1] = {
#include "stg13/terrain/collision_333_meshsets.inc"
};
static NJS_POINT3 collision_333_points[14] = {
#include "stg13/terrain/collision_333_points.inc"
};
static NJS_MODEL collision_333_model = {
#include "stg13/terrain/collision_333_model.inc"
};
static NJS_OBJECT collision_333 = {
#include "stg13/terrain/collision_333.inc"
};
static NJS_MATERIAL collision_334_materials[1] = {
#include "stg13/terrain/collision_334_materials.inc"
};
static Sint16 collision_334_faces[40] = {
#include "stg13/terrain/collision_334_faces.inc"
};
static NJS_MESHSET collision_334_meshsets[1] = {
#include "stg13/terrain/collision_334_meshsets.inc"
};
static NJS_POINT3 collision_334_points[24] = {
#include "stg13/terrain/collision_334_points.inc"
};
static NJS_MODEL collision_334_model = {
#include "stg13/terrain/collision_334_model.inc"
};
static NJS_OBJECT collision_334 = {
#include "stg13/terrain/collision_334.inc"
};
static NJS_MATERIAL collision_335_materials[1] = {
#include "stg13/terrain/collision_335_materials.inc"
};
static Sint16 collision_335_faces[39] = {
#include "stg13/terrain/collision_335_faces.inc"
};
static NJS_MESHSET collision_335_meshsets[1] = {
#include "stg13/terrain/collision_335_meshsets.inc"
};
static NJS_POINT3 collision_335_points[21] = {
#include "stg13/terrain/collision_335_points.inc"
};
static NJS_MODEL collision_335_model = {
#include "stg13/terrain/collision_335_model.inc"
};
static NJS_OBJECT collision_335 = {
#include "stg13/terrain/collision_335.inc"
};
static NJS_MATERIAL collision_336_materials[1] = {
#include "stg13/terrain/collision_336_materials.inc"
};
static Sint16 collision_336_faces[9] = {
#include "stg13/terrain/collision_336_faces.inc"
};
static NJS_MESHSET collision_336_meshsets[1] = {
#include "stg13/terrain/collision_336_meshsets.inc"
};
static NJS_POINT3 collision_336_points[8] = {
#include "stg13/terrain/collision_336_points.inc"
};
static NJS_MODEL collision_336_model = {
#include "stg13/terrain/collision_336_model.inc"
};
static NJS_OBJECT collision_336 = {
#include "stg13/terrain/collision_336.inc"
};
static NJS_MATERIAL collision_337_materials[1] = {
#include "stg13/terrain/collision_337_materials.inc"
};
static Sint16 collision_337_faces[5] = {
#include "stg13/terrain/collision_337_faces.inc"
};
static NJS_MESHSET collision_337_meshsets[1] = {
#include "stg13/terrain/collision_337_meshsets.inc"
};
static NJS_POINT3 collision_337_points[4] = {
#include "stg13/terrain/collision_337_points.inc"
};
static NJS_MODEL collision_337_model = {
#include "stg13/terrain/collision_337_model.inc"
};
static NJS_OBJECT collision_337 = {
#include "stg13/terrain/collision_337.inc"
};
static NJS_MATERIAL collision_341_materials[1] = {
#include "stg13/terrain/collision_341_materials.inc"
};
static Sint16 collision_341_faces[117] = {
#include "stg13/terrain/collision_341_faces.inc"
};
static NJS_MESHSET collision_341_meshsets[1] = {
#include "stg13/terrain/collision_341_meshsets.inc"
};
static NJS_POINT3 collision_341_points[59] = {
#include "stg13/terrain/collision_341_points.inc"
};
static NJS_MODEL collision_341_model = {
#include "stg13/terrain/collision_341_model.inc"
};
static NJS_OBJECT collision_341 = {
#include "stg13/terrain/collision_341.inc"
};
static NJS_MATERIAL collision_342_materials[1] = {
#include "stg13/terrain/collision_342_materials.inc"
};
static Sint16 collision_342_faces[15] = {
#include "stg13/terrain/collision_342_faces.inc"
};
static NJS_MESHSET collision_342_meshsets[1] = {
#include "stg13/terrain/collision_342_meshsets.inc"
};
static NJS_POINT3 collision_342_points[8] = {
#include "stg13/terrain/collision_342_points.inc"
};
static NJS_MODEL collision_342_model = {
#include "stg13/terrain/collision_342_model.inc"
};
static NJS_OBJECT collision_342 = {
#include "stg13/terrain/collision_342.inc"
};
static NJS_MATERIAL collision_343_materials[1] = {
#include "stg13/terrain/collision_343_materials.inc"
};
static Sint16 collision_343_faces[11] = {
#include "stg13/terrain/collision_343_faces.inc"
};
static NJS_MESHSET collision_343_meshsets[1] = {
#include "stg13/terrain/collision_343_meshsets.inc"
};
static NJS_POINT3 collision_343_points[10] = {
#include "stg13/terrain/collision_343_points.inc"
};
static NJS_MODEL collision_343_model = {
#include "stg13/terrain/collision_343_model.inc"
};
static NJS_OBJECT collision_343 = {
#include "stg13/terrain/collision_343.inc"
};
static NJS_MATERIAL collision_344_materials[1] = {
#include "stg13/terrain/collision_344_materials.inc"
};
static Sint16 collision_344_faces[7] = {
#include "stg13/terrain/collision_344_faces.inc"
};
static NJS_MESHSET collision_344_meshsets[1] = {
#include "stg13/terrain/collision_344_meshsets.inc"
};
static NJS_POINT3 collision_344_points[6] = {
#include "stg13/terrain/collision_344_points.inc"
};
static NJS_MODEL collision_344_model = {
#include "stg13/terrain/collision_344_model.inc"
};
static NJS_OBJECT collision_344 = {
#include "stg13/terrain/collision_344.inc"
};
static NJS_MATERIAL collision_345_materials[1] = {
#include "stg13/terrain/collision_345_materials.inc"
};
static Sint16 collision_345_faces[22] = {
#include "stg13/terrain/collision_345_faces.inc"
};
static NJS_MESHSET collision_345_meshsets[1] = {
#include "stg13/terrain/collision_345_meshsets.inc"
};
static NJS_POINT3 collision_345_points[15] = {
#include "stg13/terrain/collision_345_points.inc"
};
static NJS_MODEL collision_345_model = {
#include "stg13/terrain/collision_345_model.inc"
};
static NJS_OBJECT collision_345 = {
#include "stg13/terrain/collision_345.inc"
};
static NJS_MATERIAL collision_346_materials[1] = {
#include "stg13/terrain/collision_346_materials.inc"
};
static Sint16 collision_346_faces[18] = {
#include "stg13/terrain/collision_346_faces.inc"
};
static NJS_MESHSET collision_346_meshsets[1] = {
#include "stg13/terrain/collision_346_meshsets.inc"
};
static NJS_POINT3 collision_346_points[14] = {
#include "stg13/terrain/collision_346_points.inc"
};
static NJS_MODEL collision_346_model = {
#include "stg13/terrain/collision_346_model.inc"
};
static NJS_OBJECT collision_346 = {
#include "stg13/terrain/collision_346.inc"
};
static NJS_MATERIAL collision_347_materials[1] = {
#include "stg13/terrain/collision_347_materials.inc"
};
static Sint16 collision_347_faces[13] = {
#include "stg13/terrain/collision_347_faces.inc"
};
static NJS_MESHSET collision_347_meshsets[1] = {
#include "stg13/terrain/collision_347_meshsets.inc"
};
static NJS_POINT3 collision_347_points[12] = {
#include "stg13/terrain/collision_347_points.inc"
};
static NJS_MODEL collision_347_model = {
#include "stg13/terrain/collision_347_model.inc"
};
static NJS_OBJECT collision_347 = {
#include "stg13/terrain/collision_347.inc"
};
static NJS_MATERIAL collision_349_materials[1] = {
#include "stg13/terrain/collision_349_materials.inc"
};
static Sint16 collision_349_faces[7] = {
#include "stg13/terrain/collision_349_faces.inc"
};
static NJS_MESHSET collision_349_meshsets[1] = {
#include "stg13/terrain/collision_349_meshsets.inc"
};
static NJS_POINT3 collision_349_points[6] = {
#include "stg13/terrain/collision_349_points.inc"
};
static NJS_MODEL collision_349_model = {
#include "stg13/terrain/collision_349_model.inc"
};
static NJS_OBJECT collision_349 = {
#include "stg13/terrain/collision_349.inc"
};
static NJS_MATERIAL collision_350_materials[1] = {
#include "stg13/terrain/collision_350_materials.inc"
};
static Sint16 collision_350_faces[39] = {
#include "stg13/terrain/collision_350_faces.inc"
};
static NJS_MESHSET collision_350_meshsets[1] = {
#include "stg13/terrain/collision_350_meshsets.inc"
};
static NJS_POINT3 collision_350_points[21] = {
#include "stg13/terrain/collision_350_points.inc"
};
static NJS_MODEL collision_350_model = {
#include "stg13/terrain/collision_350_model.inc"
};
static NJS_OBJECT collision_350 = {
#include "stg13/terrain/collision_350.inc"
};
static NJS_MATERIAL collision_351_materials[1] = {
#include "stg13/terrain/collision_351_materials.inc"
};
static Sint16 collision_351_faces[9] = {
#include "stg13/terrain/collision_351_faces.inc"
};
static NJS_MESHSET collision_351_meshsets[1] = {
#include "stg13/terrain/collision_351_meshsets.inc"
};
static NJS_POINT3 collision_351_points[8] = {
#include "stg13/terrain/collision_351_points.inc"
};
static NJS_MODEL collision_351_model = {
#include "stg13/terrain/collision_351_model.inc"
};
static NJS_OBJECT collision_351 = {
#include "stg13/terrain/collision_351.inc"
};
static NJS_MATERIAL collision_352_materials[1] = {
#include "stg13/terrain/collision_352_materials.inc"
};
static Sint16 collision_352_faces[71] = {
#include "stg13/terrain/collision_352_faces.inc"
};
static NJS_MESHSET collision_352_meshsets[1] = {
#include "stg13/terrain/collision_352_meshsets.inc"
};
static NJS_POINT3 collision_352_points[33] = {
#include "stg13/terrain/collision_352_points.inc"
};
static NJS_MODEL collision_352_model = {
#include "stg13/terrain/collision_352_model.inc"
};
static NJS_OBJECT collision_352 = {
#include "stg13/terrain/collision_352.inc"
};
static NJS_MATERIAL collision_353_materials[1] = {
#include "stg13/terrain/collision_353_materials.inc"
};
static Sint16 collision_353_faces[18] = {
#include "stg13/terrain/collision_353_faces.inc"
};
static NJS_MESHSET collision_353_meshsets[1] = {
#include "stg13/terrain/collision_353_meshsets.inc"
};
static NJS_POINT3 collision_353_points[12] = {
#include "stg13/terrain/collision_353_points.inc"
};
static NJS_MODEL collision_353_model = {
#include "stg13/terrain/collision_353_model.inc"
};
static NJS_OBJECT collision_353 = {
#include "stg13/terrain/collision_353.inc"
};
static NJS_MATERIAL collision_354_materials[1] = {
#include "stg13/terrain/collision_354_materials.inc"
};
static Sint16 collision_354_faces[18] = {
#include "stg13/terrain/collision_354_faces.inc"
};
static NJS_MESHSET collision_354_meshsets[1] = {
#include "stg13/terrain/collision_354_meshsets.inc"
};
static NJS_POINT3 collision_354_points[16] = {
#include "stg13/terrain/collision_354_points.inc"
};
static NJS_MODEL collision_354_model = {
#include "stg13/terrain/collision_354_model.inc"
};
static NJS_OBJECT collision_354 = {
#include "stg13/terrain/collision_354.inc"
};
static NJS_MATERIAL collision_355_materials[1] = {
#include "stg13/terrain/collision_355_materials.inc"
};
static Sint16 collision_355_faces[27] = {
#include "stg13/terrain/collision_355_faces.inc"
};
static NJS_MESHSET collision_355_meshsets[1] = {
#include "stg13/terrain/collision_355_meshsets.inc"
};
static NJS_POINT3 collision_355_points[14] = {
#include "stg13/terrain/collision_355_points.inc"
};
static NJS_MODEL collision_355_model = {
#include "stg13/terrain/collision_355_model.inc"
};
static NJS_OBJECT collision_355 = {
#include "stg13/terrain/collision_355.inc"
};
static NJS_MATERIAL collision_357_materials[1] = {
#include "stg13/terrain/collision_357_materials.inc"
};
static Sint16 collision_357_faces[71] = {
#include "stg13/terrain/collision_357_faces.inc"
};
static NJS_MESHSET collision_357_meshsets[1] = {
#include "stg13/terrain/collision_357_meshsets.inc"
};
static NJS_POINT3 collision_357_points[33] = {
#include "stg13/terrain/collision_357_points.inc"
};
static NJS_MODEL collision_357_model = {
#include "stg13/terrain/collision_357_model.inc"
};
static NJS_OBJECT collision_357 = {
#include "stg13/terrain/collision_357.inc"
};
static NJS_MATERIAL collision_358_materials[1] = {
#include "stg13/terrain/collision_358_materials.inc"
};
static Sint16 collision_358_faces[27] = {
#include "stg13/terrain/collision_358_faces.inc"
};
static NJS_MESHSET collision_358_meshsets[1] = {
#include "stg13/terrain/collision_358_meshsets.inc"
};
static NJS_POINT3 collision_358_points[26] = {
#include "stg13/terrain/collision_358_points.inc"
};
static NJS_MODEL collision_358_model = {
#include "stg13/terrain/collision_358_model.inc"
};
static NJS_OBJECT collision_358 = {
#include "stg13/terrain/collision_358.inc"
};
static NJS_MATERIAL collision_359_materials[1] = {
#include "stg13/terrain/collision_359_materials.inc"
};
static Sint16 collision_359_faces[24] = {
#include "stg13/terrain/collision_359_faces.inc"
};
static NJS_MESHSET collision_359_meshsets[1] = {
#include "stg13/terrain/collision_359_meshsets.inc"
};
static NJS_POINT3 collision_359_points[14] = {
#include "stg13/terrain/collision_359_points.inc"
};
static NJS_MODEL collision_359_model = {
#include "stg13/terrain/collision_359_model.inc"
};
static NJS_OBJECT collision_359 = {
#include "stg13/terrain/collision_359.inc"
};
static NJS_MATERIAL collision_360_materials[1] = {
#include "stg13/terrain/collision_360_materials.inc"
};
static Sint16 collision_360_faces[46] = {
#include "stg13/terrain/collision_360_faces.inc"
};
static NJS_MESHSET collision_360_meshsets[1] = {
#include "stg13/terrain/collision_360_meshsets.inc"
};
static NJS_POINT3 collision_360_points[20] = {
#include "stg13/terrain/collision_360_points.inc"
};
static NJS_MODEL collision_360_model = {
#include "stg13/terrain/collision_360_model.inc"
};
static NJS_OBJECT collision_360 = {
#include "stg13/terrain/collision_360.inc"
};
static NJS_MATERIAL collision_361_materials[1] = {
#include "stg13/terrain/collision_361_materials.inc"
};
static Sint16 collision_361_faces[18] = {
#include "stg13/terrain/collision_361_faces.inc"
};
static NJS_MESHSET collision_361_meshsets[1] = {
#include "stg13/terrain/collision_361_meshsets.inc"
};
static NJS_POINT3 collision_361_points[14] = {
#include "stg13/terrain/collision_361_points.inc"
};
static NJS_MODEL collision_361_model = {
#include "stg13/terrain/collision_361_model.inc"
};
static NJS_OBJECT collision_361 = {
#include "stg13/terrain/collision_361.inc"
};
static NJS_MATERIAL collision_363_materials[1] = {
#include "stg13/terrain/collision_363_materials.inc"
};
static Sint16 collision_363_faces[32] = {
#include "stg13/terrain/collision_363_faces.inc"
};
static NJS_MESHSET collision_363_meshsets[1] = {
#include "stg13/terrain/collision_363_meshsets.inc"
};
static NJS_POINT3 collision_363_points[15] = {
#include "stg13/terrain/collision_363_points.inc"
};
static NJS_MODEL collision_363_model = {
#include "stg13/terrain/collision_363_model.inc"
};
static NJS_OBJECT collision_363 = {
#include "stg13/terrain/collision_363.inc"
};
static NJS_MATERIAL collision_364_materials[1] = {
#include "stg13/terrain/collision_364_materials.inc"
};
static Sint16 collision_364_faces[39] = {
#include "stg13/terrain/collision_364_faces.inc"
};
static NJS_MESHSET collision_364_meshsets[1] = {
#include "stg13/terrain/collision_364_meshsets.inc"
};
static NJS_POINT3 collision_364_points[21] = {
#include "stg13/terrain/collision_364_points.inc"
};
static NJS_MODEL collision_364_model = {
#include "stg13/terrain/collision_364_model.inc"
};
static NJS_OBJECT collision_364 = {
#include "stg13/terrain/collision_364.inc"
};
static NJS_MATERIAL collision_366_materials[1] = {
#include "stg13/terrain/collision_366_materials.inc"
};
static Sint16 collision_366_faces[7] = {
#include "stg13/terrain/collision_366_faces.inc"
};
static NJS_MESHSET collision_366_meshsets[1] = {
#include "stg13/terrain/collision_366_meshsets.inc"
};
static NJS_POINT3 collision_366_points[6] = {
#include "stg13/terrain/collision_366_points.inc"
};
static NJS_MODEL collision_366_model = {
#include "stg13/terrain/collision_366_model.inc"
};
static NJS_OBJECT collision_366 = {
#include "stg13/terrain/collision_366.inc"
};
static NJS_MATERIAL collision_367_materials[1] = {
#include "stg13/terrain/collision_367_materials.inc"
};
static Sint16 collision_367_faces[22] = {
#include "stg13/terrain/collision_367_faces.inc"
};
static NJS_MESHSET collision_367_meshsets[1] = {
#include "stg13/terrain/collision_367_meshsets.inc"
};
static NJS_POINT3 collision_367_points[12] = {
#include "stg13/terrain/collision_367_points.inc"
};
static NJS_MODEL collision_367_model = {
#include "stg13/terrain/collision_367_model.inc"
};
static NJS_OBJECT collision_367 = {
#include "stg13/terrain/collision_367.inc"
};
static NJS_MATERIAL collision_369_materials[1] = {
#include "stg13/terrain/collision_369_materials.inc"
};
static Sint16 collision_369_faces[16] = {
#include "stg13/terrain/collision_369_faces.inc"
};
static NJS_MESHSET collision_369_meshsets[1] = {
#include "stg13/terrain/collision_369_meshsets.inc"
};
static NJS_POINT3 collision_369_points[12] = {
#include "stg13/terrain/collision_369_points.inc"
};
static NJS_MODEL collision_369_model = {
#include "stg13/terrain/collision_369_model.inc"
};
static NJS_OBJECT collision_369 = {
#include "stg13/terrain/collision_369.inc"
};
static NJS_MATERIAL collision_371_materials[1] = {
#include "stg13/terrain/collision_371_materials.inc"
};
static Sint16 collision_371_faces[35] = {
#include "stg13/terrain/collision_371_faces.inc"
};
static NJS_MESHSET collision_371_meshsets[1] = {
#include "stg13/terrain/collision_371_meshsets.inc"
};
static NJS_POINT3 collision_371_points[18] = {
#include "stg13/terrain/collision_371_points.inc"
};
static NJS_MODEL collision_371_model = {
#include "stg13/terrain/collision_371_model.inc"
};
static NJS_OBJECT collision_371 = {
#include "stg13/terrain/collision_371.inc"
};
static NJS_MATERIAL collision_373_materials[1] = {
#include "stg13/terrain/collision_373_materials.inc"
};
static Sint16 collision_373_faces[7] = {
#include "stg13/terrain/collision_373_faces.inc"
};
static NJS_MESHSET collision_373_meshsets[1] = {
#include "stg13/terrain/collision_373_meshsets.inc"
};
static NJS_POINT3 collision_373_points[6] = {
#include "stg13/terrain/collision_373_points.inc"
};
static NJS_MODEL collision_373_model = {
#include "stg13/terrain/collision_373_model.inc"
};
static NJS_OBJECT collision_373 = {
#include "stg13/terrain/collision_373.inc"
};
static NJS_MATERIAL collision_374_materials[1] = {
#include "stg13/terrain/collision_374_materials.inc"
};
static Sint16 collision_374_faces[26] = {
#include "stg13/terrain/collision_374_faces.inc"
};
static NJS_MESHSET collision_374_meshsets[1] = {
#include "stg13/terrain/collision_374_meshsets.inc"
};
static NJS_POINT3 collision_374_points[15] = {
#include "stg13/terrain/collision_374_points.inc"
};
static NJS_MODEL collision_374_model = {
#include "stg13/terrain/collision_374_model.inc"
};
static NJS_OBJECT collision_374 = {
#include "stg13/terrain/collision_374.inc"
};
static NJS_MATERIAL collision_375_materials[1] = {
#include "stg13/terrain/collision_375_materials.inc"
};
static Sint16 collision_375_faces[150] = {
#include "stg13/terrain/collision_375_faces.inc"
};
static NJS_MESHSET collision_375_meshsets[1] = {
#include "stg13/terrain/collision_375_meshsets.inc"
};
static NJS_POINT3 collision_375_points[82] = {
#include "stg13/terrain/collision_375_points.inc"
};
static NJS_MODEL collision_375_model = {
#include "stg13/terrain/collision_375_model.inc"
};
static NJS_OBJECT collision_375 = {
#include "stg13/terrain/collision_375.inc"
};
static NJS_MATERIAL collision_376_materials[1] = {
#include "stg13/terrain/collision_376_materials.inc"
};
static Sint16 collision_376_faces[23] = {
#include "stg13/terrain/collision_376_faces.inc"
};
static NJS_MESHSET collision_376_meshsets[1] = {
#include "stg13/terrain/collision_376_meshsets.inc"
};
static NJS_POINT3 collision_376_points[13] = {
#include "stg13/terrain/collision_376_points.inc"
};
static NJS_MODEL collision_376_model = {
#include "stg13/terrain/collision_376_model.inc"
};
static NJS_OBJECT collision_376 = {
#include "stg13/terrain/collision_376.inc"
};
static NJS_MATERIAL collision_378_materials[1] = {
#include "stg13/terrain/collision_378_materials.inc"
};
static Sint16 collision_378_faces[46] = {
#include "stg13/terrain/collision_378_faces.inc"
};
static NJS_MESHSET collision_378_meshsets[1] = {
#include "stg13/terrain/collision_378_meshsets.inc"
};
static NJS_POINT3 collision_378_points[24] = {
#include "stg13/terrain/collision_378_points.inc"
};
static NJS_MODEL collision_378_model = {
#include "stg13/terrain/collision_378_model.inc"
};
static NJS_OBJECT collision_378 = {
#include "stg13/terrain/collision_378.inc"
};
static NJS_MATERIAL collision_379_materials[1] = {
#include "stg13/terrain/collision_379_materials.inc"
};
static Sint16 collision_379_faces[269] = {
#include "stg13/terrain/collision_379_faces.inc"
};
static NJS_MESHSET collision_379_meshsets[1] = {
#include "stg13/terrain/collision_379_meshsets.inc"
};
static NJS_POINT3 collision_379_points[146] = {
#include "stg13/terrain/collision_379_points.inc"
};
static NJS_MODEL collision_379_model = {
#include "stg13/terrain/collision_379_model.inc"
};
static NJS_OBJECT collision_379 = {
#include "stg13/terrain/collision_379.inc"
};
static NJS_MATERIAL collision_380_materials[1] = {
#include "stg13/terrain/collision_380_materials.inc"
};
static Sint16 collision_380_faces[344] = {
#include "stg13/terrain/collision_380_faces.inc"
};
static NJS_MESHSET collision_380_meshsets[1] = {
#include "stg13/terrain/collision_380_meshsets.inc"
};
static NJS_POINT3 collision_380_points[170] = {
#include "stg13/terrain/collision_380_points.inc"
};
static NJS_MODEL collision_380_model = {
#include "stg13/terrain/collision_380_model.inc"
};
static NJS_OBJECT collision_380 = {
#include "stg13/terrain/collision_380.inc"
};
static NJS_MATERIAL collision_381_materials[1] = {
#include "stg13/terrain/collision_381_materials.inc"
};
static Sint16 collision_381_faces[27] = {
#include "stg13/terrain/collision_381_faces.inc"
};
static NJS_MESHSET collision_381_meshsets[1] = {
#include "stg13/terrain/collision_381_meshsets.inc"
};
static NJS_POINT3 collision_381_points[26] = {
#include "stg13/terrain/collision_381_points.inc"
};
static NJS_MODEL collision_381_model = {
#include "stg13/terrain/collision_381_model.inc"
};
static NJS_OBJECT collision_381 = {
#include "stg13/terrain/collision_381.inc"
};
static NJS_MATERIAL collision_382_materials[1] = {
#include "stg13/terrain/collision_382_materials.inc"
};
static Sint16 collision_382_faces[17] = {
#include "stg13/terrain/collision_382_faces.inc"
};
static NJS_MESHSET collision_382_meshsets[1] = {
#include "stg13/terrain/collision_382_meshsets.inc"
};
static NJS_POINT3 collision_382_points[16] = {
#include "stg13/terrain/collision_382_points.inc"
};
static NJS_MODEL collision_382_model = {
#include "stg13/terrain/collision_382_model.inc"
};
static NJS_OBJECT collision_382 = {
#include "stg13/terrain/collision_382.inc"
};
static NJS_MATERIAL collision_383_materials[1] = {
#include "stg13/terrain/collision_383_materials.inc"
};
static Sint16 collision_383_faces[26] = {
#include "stg13/terrain/collision_383_faces.inc"
};
static NJS_MESHSET collision_383_meshsets[1] = {
#include "stg13/terrain/collision_383_meshsets.inc"
};
static NJS_POINT3 collision_383_points[24] = {
#include "stg13/terrain/collision_383_points.inc"
};
static NJS_MODEL collision_383_model = {
#include "stg13/terrain/collision_383_model.inc"
};
static NJS_OBJECT collision_383 = {
#include "stg13/terrain/collision_383.inc"
};
static NJS_MATERIAL collision_384_materials[1] = {
#include "stg13/terrain/collision_384_materials.inc"
};
static Sint16 collision_384_faces[7] = {
#include "stg13/terrain/collision_384_faces.inc"
};
static NJS_MESHSET collision_384_meshsets[1] = {
#include "stg13/terrain/collision_384_meshsets.inc"
};
static NJS_POINT3 collision_384_points[6] = {
#include "stg13/terrain/collision_384_points.inc"
};
static NJS_MODEL collision_384_model = {
#include "stg13/terrain/collision_384_model.inc"
};
static NJS_OBJECT collision_384 = {
#include "stg13/terrain/collision_384.inc"
};
static NJS_MATERIAL collision_385_materials[1] = {
#include "stg13/terrain/collision_385_materials.inc"
};
static Sint16 collision_385_faces[61] = {
#include "stg13/terrain/collision_385_faces.inc"
};
static NJS_MESHSET collision_385_meshsets[1] = {
#include "stg13/terrain/collision_385_meshsets.inc"
};
static NJS_POINT3 collision_385_points[30] = {
#include "stg13/terrain/collision_385_points.inc"
};
static NJS_MODEL collision_385_model = {
#include "stg13/terrain/collision_385_model.inc"
};
static NJS_OBJECT collision_385 = {
#include "stg13/terrain/collision_385.inc"
};
static NJS_MATERIAL collision_387_materials[1] = {
#include "stg13/terrain/collision_387_materials.inc"
};
static Sint16 collision_387_faces[220] = {
#include "stg13/terrain/collision_387_faces.inc"
};
static NJS_MESHSET collision_387_meshsets[1] = {
#include "stg13/terrain/collision_387_meshsets.inc"
};
static NJS_POINT3 collision_387_points[114] = {
#include "stg13/terrain/collision_387_points.inc"
};
static NJS_MODEL collision_387_model = {
#include "stg13/terrain/collision_387_model.inc"
};
static NJS_OBJECT collision_387 = {
#include "stg13/terrain/collision_387.inc"
};
static NJS_MATERIAL collision_388_materials[1] = {
#include "stg13/terrain/collision_388_materials.inc"
};
static Sint16 collision_388_faces[30] = {
#include "stg13/terrain/collision_388_faces.inc"
};
static NJS_MESHSET collision_388_meshsets[1] = {
#include "stg13/terrain/collision_388_meshsets.inc"
};
static NJS_POINT3 collision_388_points[18] = {
#include "stg13/terrain/collision_388_points.inc"
};
static NJS_MODEL collision_388_model = {
#include "stg13/terrain/collision_388_model.inc"
};
static NJS_OBJECT collision_388 = {
#include "stg13/terrain/collision_388.inc"
};
static NJS_MATERIAL collision_389_materials[1] = {
#include "stg13/terrain/collision_389_materials.inc"
};
static Sint16 collision_389_faces[23] = {
#include "stg13/terrain/collision_389_faces.inc"
};
static NJS_MESHSET collision_389_meshsets[1] = {
#include "stg13/terrain/collision_389_meshsets.inc"
};
static NJS_POINT3 collision_389_points[12] = {
#include "stg13/terrain/collision_389_points.inc"
};
static NJS_MODEL collision_389_model = {
#include "stg13/terrain/collision_389_model.inc"
};
static NJS_OBJECT collision_389 = {
#include "stg13/terrain/collision_389.inc"
};
static NJS_MATERIAL collision_390_materials[1] = {
#include "stg13/terrain/collision_390_materials.inc"
};
static Sint16 collision_390_faces[41] = {
#include "stg13/terrain/collision_390_faces.inc"
};
static NJS_MESHSET collision_390_meshsets[1] = {
#include "stg13/terrain/collision_390_meshsets.inc"
};
static NJS_POINT3 collision_390_points[18] = {
#include "stg13/terrain/collision_390_points.inc"
};
static NJS_MODEL collision_390_model = {
#include "stg13/terrain/collision_390_model.inc"
};
static NJS_OBJECT collision_390 = {
#include "stg13/terrain/collision_390.inc"
};
static NJS_MATERIAL collision_391_materials[1] = {
#include "stg13/terrain/collision_391_materials.inc"
};
static Sint16 collision_391_faces[7] = {
#include "stg13/terrain/collision_391_faces.inc"
};
static NJS_MESHSET collision_391_meshsets[1] = {
#include "stg13/terrain/collision_391_meshsets.inc"
};
static NJS_POINT3 collision_391_points[6] = {
#include "stg13/terrain/collision_391_points.inc"
};
static NJS_MODEL collision_391_model = {
#include "stg13/terrain/collision_391_model.inc"
};
static NJS_OBJECT collision_391 = {
#include "stg13/terrain/collision_391.inc"
};
static NJS_MATERIAL collision_393_materials[1] = {
#include "stg13/terrain/collision_393_materials.inc"
};
static Sint16 collision_393_faces[5] = {
#include "stg13/terrain/collision_393_faces.inc"
};
static NJS_MESHSET collision_393_meshsets[1] = {
#include "stg13/terrain/collision_393_meshsets.inc"
};
static NJS_POINT3 collision_393_points[4] = {
#include "stg13/terrain/collision_393_points.inc"
};
static NJS_MODEL collision_393_model = {
#include "stg13/terrain/collision_393_model.inc"
};
static NJS_OBJECT collision_393 = {
#include "stg13/terrain/collision_393.inc"
};
static NJS_MATERIAL collision_394_materials[1] = {
#include "stg13/terrain/collision_394_materials.inc"
};
static Sint16 collision_394_faces[22] = {
#include "stg13/terrain/collision_394_faces.inc"
};
static NJS_MESHSET collision_394_meshsets[1] = {
#include "stg13/terrain/collision_394_meshsets.inc"
};
static NJS_POINT3 collision_394_points[15] = {
#include "stg13/terrain/collision_394_points.inc"
};
static NJS_MODEL collision_394_model = {
#include "stg13/terrain/collision_394_model.inc"
};
static NJS_OBJECT collision_394 = {
#include "stg13/terrain/collision_394.inc"
};
static NJS_MATERIAL collision_397_materials[1] = {
#include "stg13/terrain/collision_397_materials.inc"
};
static Sint16 collision_397_faces[22] = {
#include "stg13/terrain/collision_397_faces.inc"
};
static NJS_MESHSET collision_397_meshsets[1] = {
#include "stg13/terrain/collision_397_meshsets.inc"
};
static NJS_POINT3 collision_397_points[15] = {
#include "stg13/terrain/collision_397_points.inc"
};
static NJS_MODEL collision_397_model = {
#include "stg13/terrain/collision_397_model.inc"
};
static NJS_OBJECT collision_397 = {
#include "stg13/terrain/collision_397.inc"
};
static NJS_MATERIAL collision_399_materials[1] = {
#include "stg13/terrain/collision_399_materials.inc"
};
static Sint16 collision_399_faces[18] = {
#include "stg13/terrain/collision_399_faces.inc"
};
static NJS_MESHSET collision_399_meshsets[1] = {
#include "stg13/terrain/collision_399_meshsets.inc"
};
static NJS_POINT3 collision_399_points[14] = {
#include "stg13/terrain/collision_399_points.inc"
};
static NJS_MODEL collision_399_model = {
#include "stg13/terrain/collision_399_model.inc"
};
static NJS_OBJECT collision_399 = {
#include "stg13/terrain/collision_399.inc"
};
static NJS_MATERIAL collision_402_materials[1] = {
#include "stg13/terrain/collision_402_materials.inc"
};
static Sint16 collision_402_faces[39] = {
#include "stg13/terrain/collision_402_faces.inc"
};
static NJS_MESHSET collision_402_meshsets[1] = {
#include "stg13/terrain/collision_402_meshsets.inc"
};
static NJS_POINT3 collision_402_points[21] = {
#include "stg13/terrain/collision_402_points.inc"
};
static NJS_MODEL collision_402_model = {
#include "stg13/terrain/collision_402_model.inc"
};
static NJS_OBJECT collision_402 = {
#include "stg13/terrain/collision_402.inc"
};
static NJS_MATERIAL collision_405_materials[1] = {
#include "stg13/terrain/collision_405_materials.inc"
};
static Sint16 collision_405_faces[11] = {
#include "stg13/terrain/collision_405_faces.inc"
};
static NJS_MESHSET collision_405_meshsets[1] = {
#include "stg13/terrain/collision_405_meshsets.inc"
};
static NJS_POINT3 collision_405_points[10] = {
#include "stg13/terrain/collision_405_points.inc"
};
static NJS_MODEL collision_405_model = {
#include "stg13/terrain/collision_405_model.inc"
};
static NJS_OBJECT collision_405 = {
#include "stg13/terrain/collision_405.inc"
};
static NJS_MATERIAL collision_406_materials[1] = {
#include "stg13/terrain/collision_406_materials.inc"
};
static Sint16 collision_406_faces[18] = {
#include "stg13/terrain/collision_406_faces.inc"
};
static NJS_MESHSET collision_406_meshsets[1] = {
#include "stg13/terrain/collision_406_meshsets.inc"
};
static NJS_POINT3 collision_406_points[14] = {
#include "stg13/terrain/collision_406_points.inc"
};
static NJS_MODEL collision_406_model = {
#include "stg13/terrain/collision_406_model.inc"
};
static NJS_OBJECT collision_406 = {
#include "stg13/terrain/collision_406.inc"
};
static NJS_MATERIAL collision_409_materials[1] = {
#include "stg13/terrain/collision_409_materials.inc"
};
static Sint16 collision_409_faces[39] = {
#include "stg13/terrain/collision_409_faces.inc"
};
static NJS_MESHSET collision_409_meshsets[1] = {
#include "stg13/terrain/collision_409_meshsets.inc"
};
static NJS_POINT3 collision_409_points[21] = {
#include "stg13/terrain/collision_409_points.inc"
};
static NJS_MODEL collision_409_model = {
#include "stg13/terrain/collision_409_model.inc"
};
static NJS_OBJECT collision_409 = {
#include "stg13/terrain/collision_409.inc"
};
static NJS_MATERIAL collision_412_materials[1] = {
#include "stg13/terrain/collision_412_materials.inc"
};
static Sint16 collision_412_faces[32] = {
#include "stg13/terrain/collision_412_faces.inc"
};
static NJS_MESHSET collision_412_meshsets[1] = {
#include "stg13/terrain/collision_412_meshsets.inc"
};
static NJS_POINT3 collision_412_points[23] = {
#include "stg13/terrain/collision_412_points.inc"
};
static NJS_MODEL collision_412_model = {
#include "stg13/terrain/collision_412_model.inc"
};
static NJS_OBJECT collision_412 = {
#include "stg13/terrain/collision_412.inc"
};
static NJS_MATERIAL collision_413_materials[1] = {
#include "stg13/terrain/collision_413_materials.inc"
};
static Sint16 collision_413_faces[44] = {
#include "stg13/terrain/collision_413_faces.inc"
};
static NJS_MESHSET collision_413_meshsets[1] = {
#include "stg13/terrain/collision_413_meshsets.inc"
};
static NJS_POINT3 collision_413_points[25] = {
#include "stg13/terrain/collision_413_points.inc"
};
static NJS_MODEL collision_413_model = {
#include "stg13/terrain/collision_413_model.inc"
};
static NJS_OBJECT collision_413 = {
#include "stg13/terrain/collision_413.inc"
};
static NJS_MATERIAL collision_416_materials[1] = {
#include "stg13/terrain/collision_416_materials.inc"
};
static Sint16 collision_416_faces[20] = {
#include "stg13/terrain/collision_416_faces.inc"
};
static NJS_MESHSET collision_416_meshsets[1] = {
#include "stg13/terrain/collision_416_meshsets.inc"
};
static NJS_POINT3 collision_416_points[7] = {
#include "stg13/terrain/collision_416_points.inc"
};
static NJS_MODEL collision_416_model = {
#include "stg13/terrain/collision_416_model.inc"
};
static NJS_OBJECT collision_416 = {
#include "stg13/terrain/collision_416.inc"
};
static NJS_MATERIAL collision_418_materials[1] = {
#include "stg13/terrain/collision_418_materials.inc"
};
static Sint16 collision_418_faces[42] = {
#include "stg13/terrain/collision_418_faces.inc"
};
static NJS_MESHSET collision_418_meshsets[1] = {
#include "stg13/terrain/collision_418_meshsets.inc"
};
static NJS_POINT3 collision_418_points[25] = {
#include "stg13/terrain/collision_418_points.inc"
};
static NJS_MODEL collision_418_model = {
#include "stg13/terrain/collision_418_model.inc"
};
static NJS_OBJECT collision_418 = {
#include "stg13/terrain/collision_418.inc"
};
static NJS_MATERIAL collision_419_materials[1] = {
#include "stg13/terrain/collision_419_materials.inc"
};
static Sint16 collision_419_faces[78] = {
#include "stg13/terrain/collision_419_faces.inc"
};
static NJS_MESHSET collision_419_meshsets[1] = {
#include "stg13/terrain/collision_419_meshsets.inc"
};
static NJS_POINT3 collision_419_points[35] = {
#include "stg13/terrain/collision_419_points.inc"
};
static NJS_MODEL collision_419_model = {
#include "stg13/terrain/collision_419_model.inc"
};
static NJS_OBJECT collision_419 = {
#include "stg13/terrain/collision_419.inc"
};
static NJS_MATERIAL collision_422_materials[1] = {
#include "stg13/terrain/collision_422_materials.inc"
};
static Sint16 collision_422_faces[69] = {
#include "stg13/terrain/collision_422_faces.inc"
};
static NJS_MESHSET collision_422_meshsets[1] = {
#include "stg13/terrain/collision_422_meshsets.inc"
};
static NJS_POINT3 collision_422_points[35] = {
#include "stg13/terrain/collision_422_points.inc"
};
static NJS_MODEL collision_422_model = {
#include "stg13/terrain/collision_422_model.inc"
};
static NJS_OBJECT collision_422 = {
#include "stg13/terrain/collision_422.inc"
};
static NJS_MATERIAL collision_425_materials[1] = {
#include "stg13/terrain/collision_425_materials.inc"
};
static Sint16 collision_425_faces[19] = {
#include "stg13/terrain/collision_425_faces.inc"
};
static NJS_MESHSET collision_425_meshsets[1] = {
#include "stg13/terrain/collision_425_meshsets.inc"
};
static NJS_POINT3 collision_425_points[16] = {
#include "stg13/terrain/collision_425_points.inc"
};
static NJS_MODEL collision_425_model = {
#include "stg13/terrain/collision_425_model.inc"
};
static NJS_OBJECT collision_425 = {
#include "stg13/terrain/collision_425.inc"
};
static NJS_MATERIAL collision_426_materials[1] = {
#include "stg13/terrain/collision_426_materials.inc"
};
static Sint16 collision_426_faces[249] = {
#include "stg13/terrain/collision_426_faces.inc"
};
static NJS_MESHSET collision_426_meshsets[1] = {
#include "stg13/terrain/collision_426_meshsets.inc"
};
static NJS_POINT3 collision_426_points[125] = {
#include "stg13/terrain/collision_426_points.inc"
};
static NJS_MODEL collision_426_model = {
#include "stg13/terrain/collision_426_model.inc"
};
static NJS_OBJECT collision_426 = {
#include "stg13/terrain/collision_426.inc"
};
static NJS_MATERIAL collision_427_materials[1] = {
#include "stg13/terrain/collision_427_materials.inc"
};
static Sint16 collision_427_faces[16] = {
#include "stg13/terrain/collision_427_faces.inc"
};
static NJS_MESHSET collision_427_meshsets[1] = {
#include "stg13/terrain/collision_427_meshsets.inc"
};
static NJS_POINT3 collision_427_points[6] = {
#include "stg13/terrain/collision_427_points.inc"
};
static NJS_MODEL collision_427_model = {
#include "stg13/terrain/collision_427_model.inc"
};
static NJS_OBJECT collision_427 = {
#include "stg13/terrain/collision_427.inc"
};
static NJS_MATERIAL collision_428_materials[1] = {
#include "stg13/terrain/collision_428_materials.inc"
};
static Sint16 collision_428_faces[21] = {
#include "stg13/terrain/collision_428_faces.inc"
};
static NJS_MESHSET collision_428_meshsets[1] = {
#include "stg13/terrain/collision_428_meshsets.inc"
};
static NJS_POINT3 collision_428_points[18] = {
#include "stg13/terrain/collision_428_points.inc"
};
static NJS_MODEL collision_428_model = {
#include "stg13/terrain/collision_428_model.inc"
};
static NJS_OBJECT collision_428 = {
#include "stg13/terrain/collision_428.inc"
};
static NJS_MATERIAL collision_429_materials[1] = {
#include "stg13/terrain/collision_429_materials.inc"
};
static Sint16 collision_429_faces[16] = {
#include "stg13/terrain/collision_429_faces.inc"
};
static NJS_MESHSET collision_429_meshsets[1] = {
#include "stg13/terrain/collision_429_meshsets.inc"
};
static NJS_POINT3 collision_429_points[6] = {
#include "stg13/terrain/collision_429_points.inc"
};
static NJS_MODEL collision_429_model = {
#include "stg13/terrain/collision_429_model.inc"
};
static NJS_OBJECT collision_429 = {
#include "stg13/terrain/collision_429.inc"
};
static NJS_MATERIAL collision_430_materials[1] = {
#include "stg13/terrain/collision_430_materials.inc"
};
static Sint16 collision_430_faces[11] = {
#include "stg13/terrain/collision_430_faces.inc"
};
static NJS_MESHSET collision_430_meshsets[1] = {
#include "stg13/terrain/collision_430_meshsets.inc"
};
static NJS_POINT3 collision_430_points[10] = {
#include "stg13/terrain/collision_430_points.inc"
};
static NJS_MODEL collision_430_model = {
#include "stg13/terrain/collision_430_model.inc"
};
static NJS_OBJECT collision_430 = {
#include "stg13/terrain/collision_430.inc"
};
static NJS_MATERIAL collision_431_materials[1] = {
#include "stg13/terrain/collision_431_materials.inc"
};
static Sint16 collision_431_faces[72] = {
#include "stg13/terrain/collision_431_faces.inc"
};
static NJS_MESHSET collision_431_meshsets[1] = {
#include "stg13/terrain/collision_431_meshsets.inc"
};
static NJS_POINT3 collision_431_points[35] = {
#include "stg13/terrain/collision_431_points.inc"
};
static NJS_MODEL collision_431_model = {
#include "stg13/terrain/collision_431_model.inc"
};
static NJS_OBJECT collision_431 = {
#include "stg13/terrain/collision_431.inc"
};
static NJS_MATERIAL collision_433_materials[1] = {
#include "stg13/terrain/collision_433_materials.inc"
};
static Sint16 collision_433_faces[12] = {
#include "stg13/terrain/collision_433_faces.inc"
};
static NJS_MESHSET collision_433_meshsets[1] = {
#include "stg13/terrain/collision_433_meshsets.inc"
};
static NJS_POINT3 collision_433_points[8] = {
#include "stg13/terrain/collision_433_points.inc"
};
static NJS_MODEL collision_433_model = {
#include "stg13/terrain/collision_433_model.inc"
};
static NJS_OBJECT collision_433 = {
#include "stg13/terrain/collision_433.inc"
};
static NJS_MATERIAL collision_434_materials[1] = {
#include "stg13/terrain/collision_434_materials.inc"
};
static Sint16 collision_434_faces[17] = {
#include "stg13/terrain/collision_434_faces.inc"
};
static NJS_MESHSET collision_434_meshsets[1] = {
#include "stg13/terrain/collision_434_meshsets.inc"
};
static NJS_POINT3 collision_434_points[13] = {
#include "stg13/terrain/collision_434_points.inc"
};
static NJS_MODEL collision_434_model = {
#include "stg13/terrain/collision_434_model.inc"
};
static NJS_OBJECT collision_434 = {
#include "stg13/terrain/collision_434.inc"
};
static NJS_MATERIAL collision_436_materials[1] = {
#include "stg13/terrain/collision_436_materials.inc"
};
static Sint16 collision_436_faces[55] = {
#include "stg13/terrain/collision_436_faces.inc"
};
static NJS_MESHSET collision_436_meshsets[1] = {
#include "stg13/terrain/collision_436_meshsets.inc"
};
static NJS_POINT3 collision_436_points[30] = {
#include "stg13/terrain/collision_436_points.inc"
};
static NJS_MODEL collision_436_model = {
#include "stg13/terrain/collision_436_model.inc"
};
static NJS_OBJECT collision_436 = {
#include "stg13/terrain/collision_436.inc"
};
static NJS_MATERIAL collision_438_materials[1] = {
#include "stg13/terrain/collision_438_materials.inc"
};
static Sint16 collision_438_faces[16] = {
#include "stg13/terrain/collision_438_faces.inc"
};
static NJS_MESHSET collision_438_meshsets[1] = {
#include "stg13/terrain/collision_438_meshsets.inc"
};
static NJS_POINT3 collision_438_points[12] = {
#include "stg13/terrain/collision_438_points.inc"
};
static NJS_MODEL collision_438_model = {
#include "stg13/terrain/collision_438_model.inc"
};
static NJS_OBJECT collision_438 = {
#include "stg13/terrain/collision_438.inc"
};
static NJS_MATERIAL collision_439_materials[1] = {
#include "stg13/terrain/collision_439_materials.inc"
};
static Sint16 collision_439_faces[7] = {
#include "stg13/terrain/collision_439_faces.inc"
};
static NJS_MESHSET collision_439_meshsets[1] = {
#include "stg13/terrain/collision_439_meshsets.inc"
};
static NJS_POINT3 collision_439_points[6] = {
#include "stg13/terrain/collision_439_points.inc"
};
static NJS_MODEL collision_439_model = {
#include "stg13/terrain/collision_439_model.inc"
};
static NJS_OBJECT collision_439 = {
#include "stg13/terrain/collision_439.inc"
};
static NJS_MATERIAL collision_440_materials[1] = {
#include "stg13/terrain/collision_440_materials.inc"
};
static Sint16 collision_440_faces[16] = {
#include "stg13/terrain/collision_440_faces.inc"
};
static NJS_MESHSET collision_440_meshsets[1] = {
#include "stg13/terrain/collision_440_meshsets.inc"
};
static NJS_POINT3 collision_440_points[12] = {
#include "stg13/terrain/collision_440_points.inc"
};
static NJS_MODEL collision_440_model = {
#include "stg13/terrain/collision_440_model.inc"
};
static NJS_OBJECT collision_440 = {
#include "stg13/terrain/collision_440.inc"
};
static NJS_MATERIAL collision_441_materials[1] = {
#include "stg13/terrain/collision_441_materials.inc"
};
static Sint16 collision_441_faces[16] = {
#include "stg13/terrain/collision_441_faces.inc"
};
static NJS_MESHSET collision_441_meshsets[1] = {
#include "stg13/terrain/collision_441_meshsets.inc"
};
static NJS_POINT3 collision_441_points[12] = {
#include "stg13/terrain/collision_441_points.inc"
};
static NJS_MODEL collision_441_model = {
#include "stg13/terrain/collision_441_model.inc"
};
static NJS_OBJECT collision_441 = {
#include "stg13/terrain/collision_441.inc"
};
static NJS_MATERIAL collision_442_materials[1] = {
#include "stg13/terrain/collision_442_materials.inc"
};
static Sint16 collision_442_faces[16] = {
#include "stg13/terrain/collision_442_faces.inc"
};
static NJS_MESHSET collision_442_meshsets[1] = {
#include "stg13/terrain/collision_442_meshsets.inc"
};
static NJS_POINT3 collision_442_points[12] = {
#include "stg13/terrain/collision_442_points.inc"
};
static NJS_MODEL collision_442_model = {
#include "stg13/terrain/collision_442_model.inc"
};
static NJS_OBJECT collision_442 = {
#include "stg13/terrain/collision_442.inc"
};
static NJS_MATERIAL collision_443_materials[1] = {
#include "stg13/terrain/collision_443_materials.inc"
};
static Sint16 collision_443_faces[7] = {
#include "stg13/terrain/collision_443_faces.inc"
};
static NJS_MESHSET collision_443_meshsets[1] = {
#include "stg13/terrain/collision_443_meshsets.inc"
};
static NJS_POINT3 collision_443_points[6] = {
#include "stg13/terrain/collision_443_points.inc"
};
static NJS_MODEL collision_443_model = {
#include "stg13/terrain/collision_443_model.inc"
};
static NJS_OBJECT collision_443 = {
#include "stg13/terrain/collision_443.inc"
};
static NJS_MATERIAL collision_446_materials[1] = {
#include "stg13/terrain/collision_446_materials.inc"
};
static Sint16 collision_446_faces[7] = {
#include "stg13/terrain/collision_446_faces.inc"
};
static NJS_MESHSET collision_446_meshsets[1] = {
#include "stg13/terrain/collision_446_meshsets.inc"
};
static NJS_POINT3 collision_446_points[6] = {
#include "stg13/terrain/collision_446_points.inc"
};
static NJS_MODEL collision_446_model = {
#include "stg13/terrain/collision_446_model.inc"
};
static NJS_OBJECT collision_446 = {
#include "stg13/terrain/collision_446.inc"
};
static NJS_MATERIAL collision_447_materials[1] = {
#include "stg13/terrain/collision_447_materials.inc"
};
static Sint16 collision_447_faces[7] = {
#include "stg13/terrain/collision_447_faces.inc"
};
static NJS_MESHSET collision_447_meshsets[1] = {
#include "stg13/terrain/collision_447_meshsets.inc"
};
static NJS_POINT3 collision_447_points[6] = {
#include "stg13/terrain/collision_447_points.inc"
};
static NJS_MODEL collision_447_model = {
#include "stg13/terrain/collision_447_model.inc"
};
static NJS_OBJECT collision_447 = {
#include "stg13/terrain/collision_447.inc"
};
static NJS_MATERIAL collision_448_materials[1] = {
#include "stg13/terrain/collision_448_materials.inc"
};
static Sint16 collision_448_faces[48] = {
#include "stg13/terrain/collision_448_faces.inc"
};
static NJS_MESHSET collision_448_meshsets[1] = {
#include "stg13/terrain/collision_448_meshsets.inc"
};
static NJS_POINT3 collision_448_points[26] = {
#include "stg13/terrain/collision_448_points.inc"
};
static NJS_MODEL collision_448_model = {
#include "stg13/terrain/collision_448_model.inc"
};
static NJS_OBJECT collision_448 = {
#include "stg13/terrain/collision_448.inc"
};
static NJS_MATERIAL collision_449_materials[1] = {
#include "stg13/terrain/collision_449_materials.inc"
};
static Sint16 collision_449_faces[14] = {
#include "stg13/terrain/collision_449_faces.inc"
};
static NJS_MESHSET collision_449_meshsets[1] = {
#include "stg13/terrain/collision_449_meshsets.inc"
};
static NJS_POINT3 collision_449_points[7] = {
#include "stg13/terrain/collision_449_points.inc"
};
static NJS_MODEL collision_449_model = {
#include "stg13/terrain/collision_449_model.inc"
};
static NJS_OBJECT collision_449 = {
#include "stg13/terrain/collision_449.inc"
};
static NJS_MATERIAL collision_450_materials[1] = {
#include "stg13/terrain/collision_450_materials.inc"
};
static Sint16 collision_450_faces[42] = {
#include "stg13/terrain/collision_450_faces.inc"
};
static NJS_MESHSET collision_450_meshsets[1] = {
#include "stg13/terrain/collision_450_meshsets.inc"
};
static NJS_POINT3 collision_450_points[25] = {
#include "stg13/terrain/collision_450_points.inc"
};
static NJS_MODEL collision_450_model = {
#include "stg13/terrain/collision_450_model.inc"
};
static NJS_OBJECT collision_450 = {
#include "stg13/terrain/collision_450.inc"
};
static NJS_MATERIAL collision_451_materials[1] = {
#include "stg13/terrain/collision_451_materials.inc"
};
static Sint16 collision_451_faces[6] = {
#include "stg13/terrain/collision_451_faces.inc"
};
static NJS_MESHSET collision_451_meshsets[1] = {
#include "stg13/terrain/collision_451_meshsets.inc"
};
static NJS_POINT3 collision_451_points[5] = {
#include "stg13/terrain/collision_451_points.inc"
};
static NJS_MODEL collision_451_model = {
#include "stg13/terrain/collision_451_model.inc"
};
static NJS_OBJECT collision_451 = {
#include "stg13/terrain/collision_451.inc"
};
static NJS_MATERIAL collision_452_materials[1] = {
#include "stg13/terrain/collision_452_materials.inc"
};
static Sint16 collision_452_faces[6] = {
#include "stg13/terrain/collision_452_faces.inc"
};
static NJS_MESHSET collision_452_meshsets[1] = {
#include "stg13/terrain/collision_452_meshsets.inc"
};
static NJS_POINT3 collision_452_points[5] = {
#include "stg13/terrain/collision_452_points.inc"
};
static NJS_MODEL collision_452_model = {
#include "stg13/terrain/collision_452_model.inc"
};
static NJS_OBJECT collision_452 = {
#include "stg13/terrain/collision_452.inc"
};
static NJS_MATERIAL collision_453_materials[1] = {
#include "stg13/terrain/collision_453_materials.inc"
};
static Sint16 collision_453_faces[6] = {
#include "stg13/terrain/collision_453_faces.inc"
};
static NJS_MESHSET collision_453_meshsets[1] = {
#include "stg13/terrain/collision_453_meshsets.inc"
};
static NJS_POINT3 collision_453_points[5] = {
#include "stg13/terrain/collision_453_points.inc"
};
static NJS_MODEL collision_453_model = {
#include "stg13/terrain/collision_453_model.inc"
};
static NJS_OBJECT collision_453 = {
#include "stg13/terrain/collision_453.inc"
};
static NJS_MATERIAL collision_454_materials[1] = {
#include "stg13/terrain/collision_454_materials.inc"
};
static Sint16 collision_454_faces[35] = {
#include "stg13/terrain/collision_454_faces.inc"
};
static NJS_MESHSET collision_454_meshsets[1] = {
#include "stg13/terrain/collision_454_meshsets.inc"
};
static NJS_POINT3 collision_454_points[14] = {
#include "stg13/terrain/collision_454_points.inc"
};
static NJS_MODEL collision_454_model = {
#include "stg13/terrain/collision_454_model.inc"
};
static NJS_OBJECT collision_454 = {
#include "stg13/terrain/collision_454.inc"
};
static NJS_MATERIAL collision_455_materials[1] = {
#include "stg13/terrain/collision_455_materials.inc"
};
static Sint16 collision_455_faces[19] = {
#include "stg13/terrain/collision_455_faces.inc"
};
static NJS_MESHSET collision_455_meshsets[1] = {
#include "stg13/terrain/collision_455_meshsets.inc"
};
static NJS_POINT3 collision_455_points[16] = {
#include "stg13/terrain/collision_455_points.inc"
};
static NJS_MODEL collision_455_model = {
#include "stg13/terrain/collision_455_model.inc"
};
static NJS_OBJECT collision_455 = {
#include "stg13/terrain/collision_455.inc"
};
static NJS_MATERIAL collision_456_materials[1] = {
#include "stg13/terrain/collision_456_materials.inc"
};
static Sint16 collision_456_faces[24] = {
#include "stg13/terrain/collision_456_faces.inc"
};
static NJS_MESHSET collision_456_meshsets[1] = {
#include "stg13/terrain/collision_456_meshsets.inc"
};
static NJS_POINT3 collision_456_points[14] = {
#include "stg13/terrain/collision_456_points.inc"
};
static NJS_MODEL collision_456_model = {
#include "stg13/terrain/collision_456_model.inc"
};
static NJS_OBJECT collision_456 = {
#include "stg13/terrain/collision_456.inc"
};
static NJS_MATERIAL collision_457_materials[1] = {
#include "stg13/terrain/collision_457_materials.inc"
};
static Sint16 collision_457_faces[7] = {
#include "stg13/terrain/collision_457_faces.inc"
};
static NJS_MESHSET collision_457_meshsets[1] = {
#include "stg13/terrain/collision_457_meshsets.inc"
};
static NJS_POINT3 collision_457_points[6] = {
#include "stg13/terrain/collision_457_points.inc"
};
static NJS_MODEL collision_457_model = {
#include "stg13/terrain/collision_457_model.inc"
};
static NJS_OBJECT collision_457 = {
#include "stg13/terrain/collision_457.inc"
};
static NJS_MATERIAL collision_458_materials[1] = {
#include "stg13/terrain/collision_458_materials.inc"
};
static Sint16 collision_458_faces[27] = {
#include "stg13/terrain/collision_458_faces.inc"
};
static NJS_MESHSET collision_458_meshsets[1] = {
#include "stg13/terrain/collision_458_meshsets.inc"
};
static NJS_POINT3 collision_458_points[14] = {
#include "stg13/terrain/collision_458_points.inc"
};
static NJS_MODEL collision_458_model = {
#include "stg13/terrain/collision_458_model.inc"
};
static NJS_OBJECT collision_458 = {
#include "stg13/terrain/collision_458.inc"
};
static NJS_MATERIAL collision_459_materials[1] = {
#include "stg13/terrain/collision_459_materials.inc"
};
static Sint16 collision_459_faces[53] = {
#include "stg13/terrain/collision_459_faces.inc"
};
static NJS_MESHSET collision_459_meshsets[1] = {
#include "stg13/terrain/collision_459_meshsets.inc"
};
static NJS_POINT3 collision_459_points[25] = {
#include "stg13/terrain/collision_459_points.inc"
};
static NJS_MODEL collision_459_model = {
#include "stg13/terrain/collision_459_model.inc"
};
static NJS_OBJECT collision_459 = {
#include "stg13/terrain/collision_459.inc"
};
static NJS_MATERIAL collision_460_materials[1] = {
#include "stg13/terrain/collision_460_materials.inc"
};
static Sint16 collision_460_faces[27] = {
#include "stg13/terrain/collision_460_faces.inc"
};
static NJS_MESHSET collision_460_meshsets[1] = {
#include "stg13/terrain/collision_460_meshsets.inc"
};
static NJS_POINT3 collision_460_points[16] = {
#include "stg13/terrain/collision_460_points.inc"
};
static NJS_MODEL collision_460_model = {
#include "stg13/terrain/collision_460_model.inc"
};
static NJS_OBJECT collision_460 = {
#include "stg13/terrain/collision_460.inc"
};
static NJS_MATERIAL collision_461_materials[1] = {
#include "stg13/terrain/collision_461_materials.inc"
};
static Sint16 collision_461_faces[5] = {
#include "stg13/terrain/collision_461_faces.inc"
};
static NJS_MESHSET collision_461_meshsets[1] = {
#include "stg13/terrain/collision_461_meshsets.inc"
};
static NJS_POINT3 collision_461_points[4] = {
#include "stg13/terrain/collision_461_points.inc"
};
static NJS_MODEL collision_461_model = {
#include "stg13/terrain/collision_461_model.inc"
};
static NJS_OBJECT collision_461 = {
#include "stg13/terrain/collision_461.inc"
};
static NJS_MATERIAL collision_462_materials[1] = {
#include "stg13/terrain/collision_462_materials.inc"
};
static Sint16 collision_462_faces[71] = {
#include "stg13/terrain/collision_462_faces.inc"
};
static NJS_MESHSET collision_462_meshsets[1] = {
#include "stg13/terrain/collision_462_meshsets.inc"
};
static NJS_POINT3 collision_462_points[38] = {
#include "stg13/terrain/collision_462_points.inc"
};
static NJS_MODEL collision_462_model = {
#include "stg13/terrain/collision_462_model.inc"
};
static NJS_OBJECT collision_462 = {
#include "stg13/terrain/collision_462.inc"
};
static NJS_MATERIAL collision_463_materials[1] = {
#include "stg13/terrain/collision_463_materials.inc"
};
static Sint16 collision_463_faces[61] = {
#include "stg13/terrain/collision_463_faces.inc"
};
static NJS_MESHSET collision_463_meshsets[1] = {
#include "stg13/terrain/collision_463_meshsets.inc"
};
static NJS_POINT3 collision_463_points[28] = {
#include "stg13/terrain/collision_463_points.inc"
};
static NJS_MODEL collision_463_model = {
#include "stg13/terrain/collision_463_model.inc"
};
static NJS_OBJECT collision_463 = {
#include "stg13/terrain/collision_463.inc"
};
static NJS_MATERIAL collision_464_materials[1] = {
#include "stg13/terrain/collision_464_materials.inc"
};
static Sint16 collision_464_faces[23] = {
#include "stg13/terrain/collision_464_faces.inc"
};
static NJS_MESHSET collision_464_meshsets[1] = {
#include "stg13/terrain/collision_464_meshsets.inc"
};
static NJS_POINT3 collision_464_points[22] = {
#include "stg13/terrain/collision_464_points.inc"
};
static NJS_MODEL collision_464_model = {
#include "stg13/terrain/collision_464_model.inc"
};
static NJS_OBJECT collision_464 = {
#include "stg13/terrain/collision_464.inc"
};
static NJS_MATERIAL collision_465_materials[1] = {
#include "stg13/terrain/collision_465_materials.inc"
};
static Sint16 collision_465_faces[35] = {
#include "stg13/terrain/collision_465_faces.inc"
};
static NJS_MESHSET collision_465_meshsets[1] = {
#include "stg13/terrain/collision_465_meshsets.inc"
};
static NJS_POINT3 collision_465_points[18] = {
#include "stg13/terrain/collision_465_points.inc"
};
static NJS_MODEL collision_465_model = {
#include "stg13/terrain/collision_465_model.inc"
};
static NJS_OBJECT collision_465 = {
#include "stg13/terrain/collision_465.inc"
};
static NJS_MATERIAL collision_466_materials[1] = {
#include "stg13/terrain/collision_466_materials.inc"
};
static Sint16 collision_466_faces[20] = {
#include "stg13/terrain/collision_466_faces.inc"
};
static NJS_MESHSET collision_466_meshsets[1] = {
#include "stg13/terrain/collision_466_meshsets.inc"
};
static NJS_POINT3 collision_466_points[16] = {
#include "stg13/terrain/collision_466_points.inc"
};
static NJS_MODEL collision_466_model = {
#include "stg13/terrain/collision_466_model.inc"
};
static NJS_OBJECT collision_466 = {
#include "stg13/terrain/collision_466.inc"
};
static NJS_MATERIAL collision_467_materials[1] = {
#include "stg13/terrain/collision_467_materials.inc"
};
static Sint16 collision_467_faces[6] = {
#include "stg13/terrain/collision_467_faces.inc"
};
static NJS_MESHSET collision_467_meshsets[1] = {
#include "stg13/terrain/collision_467_meshsets.inc"
};
static NJS_POINT3 collision_467_points[5] = {
#include "stg13/terrain/collision_467_points.inc"
};
static NJS_MODEL collision_467_model = {
#include "stg13/terrain/collision_467_model.inc"
};
static NJS_OBJECT collision_467 = {
#include "stg13/terrain/collision_467.inc"
};
static NJS_MATERIAL collision_468_materials[1] = {
#include "stg13/terrain/collision_468_materials.inc"
};
static Sint16 collision_468_faces[20] = {
#include "stg13/terrain/collision_468_faces.inc"
};
static NJS_MESHSET collision_468_meshsets[1] = {
#include "stg13/terrain/collision_468_meshsets.inc"
};
static NJS_POINT3 collision_468_points[16] = {
#include "stg13/terrain/collision_468_points.inc"
};
static NJS_MODEL collision_468_model = {
#include "stg13/terrain/collision_468_model.inc"
};
static NJS_OBJECT collision_468 = {
#include "stg13/terrain/collision_468.inc"
};
static NJS_MATERIAL collision_469_materials[1] = {
#include "stg13/terrain/collision_469_materials.inc"
};
static Sint16 collision_469_faces[6] = {
#include "stg13/terrain/collision_469_faces.inc"
};
static NJS_MESHSET collision_469_meshsets[1] = {
#include "stg13/terrain/collision_469_meshsets.inc"
};
static NJS_POINT3 collision_469_points[5] = {
#include "stg13/terrain/collision_469_points.inc"
};
static NJS_MODEL collision_469_model = {
#include "stg13/terrain/collision_469_model.inc"
};
static NJS_OBJECT collision_469 = {
#include "stg13/terrain/collision_469.inc"
};
static NJS_MATERIAL collision_470_materials[1] = {
#include "stg13/terrain/collision_470_materials.inc"
};
static Sint16 collision_470_faces[23] = {
#include "stg13/terrain/collision_470_faces.inc"
};
static NJS_MESHSET collision_470_meshsets[1] = {
#include "stg13/terrain/collision_470_meshsets.inc"
};
static NJS_POINT3 collision_470_points[22] = {
#include "stg13/terrain/collision_470_points.inc"
};
static NJS_MODEL collision_470_model = {
#include "stg13/terrain/collision_470_model.inc"
};
static NJS_OBJECT collision_470 = {
#include "stg13/terrain/collision_470.inc"
};
static NJS_MATERIAL collision_472_materials[1] = {
#include "stg13/terrain/collision_472_materials.inc"
};
static Sint16 collision_472_faces[5] = {
#include "stg13/terrain/collision_472_faces.inc"
};
static NJS_MESHSET collision_472_meshsets[1] = {
#include "stg13/terrain/collision_472_meshsets.inc"
};
static NJS_POINT3 collision_472_points[4] = {
#include "stg13/terrain/collision_472_points.inc"
};
static NJS_MODEL collision_472_model = {
#include "stg13/terrain/collision_472_model.inc"
};
static NJS_OBJECT collision_472 = {
#include "stg13/terrain/collision_472.inc"
};
static NJS_MATERIAL collision_474_materials[1] = {
#include "stg13/terrain/collision_474_materials.inc"
};
static Sint16 collision_474_faces[5] = {
#include "stg13/terrain/collision_474_faces.inc"
};
static NJS_MESHSET collision_474_meshsets[1] = {
#include "stg13/terrain/collision_474_meshsets.inc"
};
static NJS_POINT3 collision_474_points[4] = {
#include "stg13/terrain/collision_474_points.inc"
};
static NJS_MODEL collision_474_model = {
#include "stg13/terrain/collision_474_model.inc"
};
static NJS_OBJECT collision_474 = {
#include "stg13/terrain/collision_474.inc"
};
static NJS_MATERIAL collision_476_materials[1] = {
#include "stg13/terrain/collision_476_materials.inc"
};
static Sint16 collision_476_faces[23] = {
#include "stg13/terrain/collision_476_faces.inc"
};
static NJS_MESHSET collision_476_meshsets[1] = {
#include "stg13/terrain/collision_476_meshsets.inc"
};
static NJS_POINT3 collision_476_points[16] = {
#include "stg13/terrain/collision_476_points.inc"
};
static NJS_MODEL collision_476_model = {
#include "stg13/terrain/collision_476_model.inc"
};
static NJS_OBJECT collision_476 = {
#include "stg13/terrain/collision_476.inc"
};
static NJS_MATERIAL collision_477_materials[1] = {
#include "stg13/terrain/collision_477_materials.inc"
};
static Sint16 collision_477_faces[14] = {
#include "stg13/terrain/collision_477_faces.inc"
};
static NJS_MESHSET collision_477_meshsets[1] = {
#include "stg13/terrain/collision_477_meshsets.inc"
};
static NJS_POINT3 collision_477_points[9] = {
#include "stg13/terrain/collision_477_points.inc"
};
static NJS_MODEL collision_477_model = {
#include "stg13/terrain/collision_477_model.inc"
};
static NJS_OBJECT collision_477 = {
#include "stg13/terrain/collision_477.inc"
};
static NJS_MATERIAL collision_478_materials[1] = {
#include "stg13/terrain/collision_478_materials.inc"
};
static Sint16 collision_478_faces[15] = {
#include "stg13/terrain/collision_478_faces.inc"
};
static NJS_MESHSET collision_478_meshsets[1] = {
#include "stg13/terrain/collision_478_meshsets.inc"
};
static NJS_POINT3 collision_478_points[10] = {
#include "stg13/terrain/collision_478_points.inc"
};
static NJS_MODEL collision_478_model = {
#include "stg13/terrain/collision_478_model.inc"
};
static NJS_OBJECT collision_478 = {
#include "stg13/terrain/collision_478.inc"
};
static NJS_MATERIAL collision_479_materials[1] = {
#include "stg13/terrain/collision_479_materials.inc"
};
static Sint16 collision_479_faces[22] = {
#include "stg13/terrain/collision_479_faces.inc"
};
static NJS_MESHSET collision_479_meshsets[1] = {
#include "stg13/terrain/collision_479_meshsets.inc"
};
static NJS_POINT3 collision_479_points[15] = {
#include "stg13/terrain/collision_479_points.inc"
};
static NJS_MODEL collision_479_model = {
#include "stg13/terrain/collision_479_model.inc"
};
static NJS_OBJECT collision_479 = {
#include "stg13/terrain/collision_479.inc"
};
static NJS_MATERIAL collision_481_materials[1] = {
#include "stg13/terrain/collision_481_materials.inc"
};
static Sint16 collision_481_faces[22] = {
#include "stg13/terrain/collision_481_faces.inc"
};
static NJS_MESHSET collision_481_meshsets[1] = {
#include "stg13/terrain/collision_481_meshsets.inc"
};
static NJS_POINT3 collision_481_points[15] = {
#include "stg13/terrain/collision_481_points.inc"
};
static NJS_MODEL collision_481_model = {
#include "stg13/terrain/collision_481_model.inc"
};
static NJS_OBJECT collision_481 = {
#include "stg13/terrain/collision_481.inc"
};
static NJS_MATERIAL collision_483_materials[1] = {
#include "stg13/terrain/collision_483_materials.inc"
};
static Sint16 collision_483_faces[14] = {
#include "stg13/terrain/collision_483_faces.inc"
};
static NJS_MESHSET collision_483_meshsets[1] = {
#include "stg13/terrain/collision_483_meshsets.inc"
};
static NJS_POINT3 collision_483_points[9] = {
#include "stg13/terrain/collision_483_points.inc"
};
static NJS_MODEL collision_483_model = {
#include "stg13/terrain/collision_483_model.inc"
};
static NJS_OBJECT collision_483 = {
#include "stg13/terrain/collision_483.inc"
};
static NJS_MATERIAL collision_484_materials[1] = {
#include "stg13/terrain/collision_484_materials.inc"
};
static Sint16 collision_484_faces[25] = {
#include "stg13/terrain/collision_484_faces.inc"
};
static NJS_MESHSET collision_484_meshsets[1] = {
#include "stg13/terrain/collision_484_meshsets.inc"
};
static NJS_POINT3 collision_484_points[14] = {
#include "stg13/terrain/collision_484_points.inc"
};
static NJS_MODEL collision_484_model = {
#include "stg13/terrain/collision_484_model.inc"
};
static NJS_OBJECT collision_484 = {
#include "stg13/terrain/collision_484.inc"
};
static NJS_MATERIAL collision_485_materials[1] = {
#include "stg13/terrain/collision_485_materials.inc"
};
static Sint16 collision_485_faces[51] = {
#include "stg13/terrain/collision_485_faces.inc"
};
static NJS_MESHSET collision_485_meshsets[1] = {
#include "stg13/terrain/collision_485_meshsets.inc"
};
static NJS_POINT3 collision_485_points[50] = {
#include "stg13/terrain/collision_485_points.inc"
};
static NJS_MODEL collision_485_model = {
#include "stg13/terrain/collision_485_model.inc"
};
static NJS_OBJECT collision_485 = {
#include "stg13/terrain/collision_485.inc"
};
static NJS_MATERIAL collision_486_materials[1] = {
#include "stg13/terrain/collision_486_materials.inc"
};
static Sint16 collision_486_faces[8] = {
#include "stg13/terrain/collision_486_faces.inc"
};
static NJS_MESHSET collision_486_meshsets[1] = {
#include "stg13/terrain/collision_486_meshsets.inc"
};
static NJS_POINT3 collision_486_points[6] = {
#include "stg13/terrain/collision_486_points.inc"
};
static NJS_MODEL collision_486_model = {
#include "stg13/terrain/collision_486_model.inc"
};
static NJS_OBJECT collision_486 = {
#include "stg13/terrain/collision_486.inc"
};
static NJS_MATERIAL collision_487_materials[1] = {
#include "stg13/terrain/collision_487_materials.inc"
};
static Sint16 collision_487_faces[33] = {
#include "stg13/terrain/collision_487_faces.inc"
};
static NJS_MESHSET collision_487_meshsets[1] = {
#include "stg13/terrain/collision_487_meshsets.inc"
};
static NJS_POINT3 collision_487_points[18] = {
#include "stg13/terrain/collision_487_points.inc"
};
static NJS_MODEL collision_487_model = {
#include "stg13/terrain/collision_487_model.inc"
};
static NJS_OBJECT collision_487 = {
#include "stg13/terrain/collision_487.inc"
};
static NJS_MATERIAL collision_488_materials[1] = {
#include "stg13/terrain/collision_488_materials.inc"
};
static Sint16 collision_488_faces[10] = {
#include "stg13/terrain/collision_488_faces.inc"
};
static NJS_MESHSET collision_488_meshsets[1] = {
#include "stg13/terrain/collision_488_meshsets.inc"
};
static NJS_POINT3 collision_488_points[8] = {
#include "stg13/terrain/collision_488_points.inc"
};
static NJS_MODEL collision_488_model = {
#include "stg13/terrain/collision_488_model.inc"
};
static NJS_OBJECT collision_488 = {
#include "stg13/terrain/collision_488.inc"
};
static NJS_MATERIAL collision_489_materials[1] = {
#include "stg13/terrain/collision_489_materials.inc"
};
static Sint16 collision_489_faces[7] = {
#include "stg13/terrain/collision_489_faces.inc"
};
static NJS_MESHSET collision_489_meshsets[1] = {
#include "stg13/terrain/collision_489_meshsets.inc"
};
static NJS_POINT3 collision_489_points[6] = {
#include "stg13/terrain/collision_489_points.inc"
};
static NJS_MODEL collision_489_model = {
#include "stg13/terrain/collision_489_model.inc"
};
static NJS_OBJECT collision_489 = {
#include "stg13/terrain/collision_489.inc"
};
static NJS_MATERIAL collision_490_materials[1] = {
#include "stg13/terrain/collision_490_materials.inc"
};
static Sint16 collision_490_faces[24] = {
#include "stg13/terrain/collision_490_faces.inc"
};
static NJS_MESHSET collision_490_meshsets[1] = {
#include "stg13/terrain/collision_490_meshsets.inc"
};
static NJS_POINT3 collision_490_points[12] = {
#include "stg13/terrain/collision_490_points.inc"
};
static NJS_MODEL collision_490_model = {
#include "stg13/terrain/collision_490_model.inc"
};
static NJS_OBJECT collision_490 = {
#include "stg13/terrain/collision_490.inc"
};
static NJS_MATERIAL collision_491_materials[1] = {
#include "stg13/terrain/collision_491_materials.inc"
};
static Sint16 collision_491_faces[51] = {
#include "stg13/terrain/collision_491_faces.inc"
};
static NJS_MESHSET collision_491_meshsets[1] = {
#include "stg13/terrain/collision_491_meshsets.inc"
};
static NJS_POINT3 collision_491_points[22] = {
#include "stg13/terrain/collision_491_points.inc"
};
static NJS_MODEL collision_491_model = {
#include "stg13/terrain/collision_491_model.inc"
};
static NJS_OBJECT collision_491 = {
#include "stg13/terrain/collision_491.inc"
};
static NJS_MATERIAL collision_492_materials[1] = {
#include "stg13/terrain/collision_492_materials.inc"
};
static Sint16 collision_492_faces[26] = {
#include "stg13/terrain/collision_492_faces.inc"
};
static NJS_MESHSET collision_492_meshsets[1] = {
#include "stg13/terrain/collision_492_meshsets.inc"
};
static NJS_POINT3 collision_492_points[22] = {
#include "stg13/terrain/collision_492_points.inc"
};
static NJS_MODEL collision_492_model = {
#include "stg13/terrain/collision_492_model.inc"
};
static NJS_OBJECT collision_492 = {
#include "stg13/terrain/collision_492.inc"
};
static NJS_MATERIAL collision_493_materials[1] = {
#include "stg13/terrain/collision_493_materials.inc"
};
static Sint16 collision_493_faces[5] = {
#include "stg13/terrain/collision_493_faces.inc"
};
static NJS_MESHSET collision_493_meshsets[1] = {
#include "stg13/terrain/collision_493_meshsets.inc"
};
static NJS_POINT3 collision_493_points[4] = {
#include "stg13/terrain/collision_493_points.inc"
};
static NJS_MODEL collision_493_model = {
#include "stg13/terrain/collision_493_model.inc"
};
static NJS_OBJECT collision_493 = {
#include "stg13/terrain/collision_493.inc"
};
static NJS_MATERIAL collision_496_materials[1] = {
#include "stg13/terrain/collision_496_materials.inc"
};
static Sint16 collision_496_faces[5] = {
#include "stg13/terrain/collision_496_faces.inc"
};
static NJS_MESHSET collision_496_meshsets[1] = {
#include "stg13/terrain/collision_496_meshsets.inc"
};
static NJS_POINT3 collision_496_points[4] = {
#include "stg13/terrain/collision_496_points.inc"
};
static NJS_MODEL collision_496_model = {
#include "stg13/terrain/collision_496_model.inc"
};
static NJS_OBJECT collision_496 = {
#include "stg13/terrain/collision_496.inc"
};
static NJS_MATERIAL collision_497_materials[1] = {
#include "stg13/terrain/collision_497_materials.inc"
};
static Sint16 collision_497_faces[5] = {
#include "stg13/terrain/collision_497_faces.inc"
};
static NJS_MESHSET collision_497_meshsets[1] = {
#include "stg13/terrain/collision_497_meshsets.inc"
};
static NJS_POINT3 collision_497_points[4] = {
#include "stg13/terrain/collision_497_points.inc"
};
static NJS_MODEL collision_497_model = {
#include "stg13/terrain/collision_497_model.inc"
};
static NJS_OBJECT collision_497 = {
#include "stg13/terrain/collision_497.inc"
};
static NJS_MATERIAL collision_499_materials[1] = {
#include "stg13/terrain/collision_499_materials.inc"
};
static Sint16 collision_499_faces[5] = {
#include "stg13/terrain/collision_499_faces.inc"
};
static NJS_MESHSET collision_499_meshsets[1] = {
#include "stg13/terrain/collision_499_meshsets.inc"
};
static NJS_POINT3 collision_499_points[4] = {
#include "stg13/terrain/collision_499_points.inc"
};
static NJS_MODEL collision_499_model = {
#include "stg13/terrain/collision_499_model.inc"
};
static NJS_OBJECT collision_499 = {
#include "stg13/terrain/collision_499.inc"
};
static NJS_MATERIAL collision_502_materials[1] = {
#include "stg13/terrain/collision_502_materials.inc"
};
static Sint16 collision_502_faces[7] = {
#include "stg13/terrain/collision_502_faces.inc"
};
static NJS_MESHSET collision_502_meshsets[1] = {
#include "stg13/terrain/collision_502_meshsets.inc"
};
static NJS_POINT3 collision_502_points[6] = {
#include "stg13/terrain/collision_502_points.inc"
};
static NJS_MODEL collision_502_model = {
#include "stg13/terrain/collision_502_model.inc"
};
static NJS_OBJECT collision_502 = {
#include "stg13/terrain/collision_502.inc"
};
static NJS_MATERIAL collision_509_materials[1] = {
#include "stg13/terrain/collision_509_materials.inc"
};
static Sint16 collision_509_faces[13] = {
#include "stg13/terrain/collision_509_faces.inc"
};
static NJS_MESHSET collision_509_meshsets[1] = {
#include "stg13/terrain/collision_509_meshsets.inc"
};
static NJS_POINT3 collision_509_points[12] = {
#include "stg13/terrain/collision_509_points.inc"
};
static NJS_MODEL collision_509_model = {
#include "stg13/terrain/collision_509_model.inc"
};
static NJS_OBJECT collision_509 = {
#include "stg13/terrain/collision_509.inc"
};
static NJS_MATERIAL collision_517_materials[1] = {
#include "stg13/terrain/collision_517_materials.inc"
};
static Sint16 collision_517_faces[13] = {
#include "stg13/terrain/collision_517_faces.inc"
};
static NJS_MESHSET collision_517_meshsets[1] = {
#include "stg13/terrain/collision_517_meshsets.inc"
};
static NJS_POINT3 collision_517_points[12] = {
#include "stg13/terrain/collision_517_points.inc"
};
static NJS_MODEL collision_517_model = {
#include "stg13/terrain/collision_517_model.inc"
};
static NJS_OBJECT collision_517 = {
#include "stg13/terrain/collision_517.inc"
};
static NJS_MATERIAL collision_525_materials[1] = {
#include "stg13/terrain/collision_525_materials.inc"
};
static Sint16 collision_525_faces[5] = {
#include "stg13/terrain/collision_525_faces.inc"
};
static NJS_MESHSET collision_525_meshsets[1] = {
#include "stg13/terrain/collision_525_meshsets.inc"
};
static NJS_POINT3 collision_525_points[4] = {
#include "stg13/terrain/collision_525_points.inc"
};
static NJS_MODEL collision_525_model = {
#include "stg13/terrain/collision_525_model.inc"
};
static NJS_OBJECT collision_525 = {
#include "stg13/terrain/collision_525.inc"
};
static NJS_MATERIAL collision_527_materials[1] = {
#include "stg13/terrain/collision_527_materials.inc"
};
static Sint16 collision_527_faces[7] = {
#include "stg13/terrain/collision_527_faces.inc"
};
static NJS_MESHSET collision_527_meshsets[1] = {
#include "stg13/terrain/collision_527_meshsets.inc"
};
static NJS_POINT3 collision_527_points[6] = {
#include "stg13/terrain/collision_527_points.inc"
};
static NJS_MODEL collision_527_model = {
#include "stg13/terrain/collision_527_model.inc"
};
static NJS_OBJECT collision_527 = {
#include "stg13/terrain/collision_527.inc"
};
static NJS_MATERIAL collision_529_materials[1] = {
#include "stg13/terrain/collision_529_materials.inc"
};
static Sint16 collision_529_faces[9] = {
#include "stg13/terrain/collision_529_faces.inc"
};
static NJS_MESHSET collision_529_meshsets[1] = {
#include "stg13/terrain/collision_529_meshsets.inc"
};
static NJS_POINT3 collision_529_points[8] = {
#include "stg13/terrain/collision_529_points.inc"
};
static NJS_MODEL collision_529_model = {
#include "stg13/terrain/collision_529_model.inc"
};
static NJS_OBJECT collision_529 = {
#include "stg13/terrain/collision_529.inc"
};
static NJS_MATERIAL collision_530_materials[1] = {
#include "stg13/terrain/collision_530_materials.inc"
};
static Sint16 collision_530_faces[7] = {
#include "stg13/terrain/collision_530_faces.inc"
};
static NJS_MESHSET collision_530_meshsets[1] = {
#include "stg13/terrain/collision_530_meshsets.inc"
};
static NJS_POINT3 collision_530_points[6] = {
#include "stg13/terrain/collision_530_points.inc"
};
static NJS_MODEL collision_530_model = {
#include "stg13/terrain/collision_530_model.inc"
};
static NJS_OBJECT collision_530 = {
#include "stg13/terrain/collision_530.inc"
};
static NJS_MATERIAL collision_531_materials[1] = {
#include "stg13/terrain/collision_531_materials.inc"
};
static Sint16 collision_531_faces[7] = {
#include "stg13/terrain/collision_531_faces.inc"
};
static NJS_MESHSET collision_531_meshsets[1] = {
#include "stg13/terrain/collision_531_meshsets.inc"
};
static NJS_POINT3 collision_531_points[6] = {
#include "stg13/terrain/collision_531_points.inc"
};
static NJS_MODEL collision_531_model = {
#include "stg13/terrain/collision_531_model.inc"
};
static NJS_OBJECT collision_531 = {
#include "stg13/terrain/collision_531.inc"
};
static NJS_MATERIAL collision_532_materials[1] = {
#include "stg13/terrain/collision_532_materials.inc"
};
static Sint16 collision_532_faces[7] = {
#include "stg13/terrain/collision_532_faces.inc"
};
static NJS_MESHSET collision_532_meshsets[1] = {
#include "stg13/terrain/collision_532_meshsets.inc"
};
static NJS_POINT3 collision_532_points[6] = {
#include "stg13/terrain/collision_532_points.inc"
};
static NJS_MODEL collision_532_model = {
#include "stg13/terrain/collision_532_model.inc"
};
static NJS_OBJECT collision_532 = {
#include "stg13/terrain/collision_532.inc"
};
static Sint16 draw_640_polygons[239] = {
#include "stg13/terrain/draw_640_polygons.inc"
};
static Sint32 draw_640_vertices[231] = {
#include "stg13/terrain/draw_640_vertices.inc"
};
static NJS_CNK_MODEL draw_640_model = {
#include "stg13/terrain/draw_640_model.inc"
};
static NJS_CNK_OBJECT draw_2204 = {
#include "stg13/terrain/draw_2204.inc"
};
static Sint16 draw_165_polygons[37] = {
#include "stg13/terrain/draw_165_polygons.inc"
};
static Sint32 draw_165_vertices[35] = {
#include "stg13/terrain/draw_165_vertices.inc"
};
static NJS_CNK_MODEL draw_165_model = {
#include "stg13/terrain/draw_165_model.inc"
};
static NJS_CNK_OBJECT draw_2203 = {
#include "stg13/terrain/draw_2203.inc"
};
static Sint16 draw_639_polygons[375] = {
#include "stg13/terrain/draw_639_polygons.inc"
};
static Sint32 draw_639_vertices[391] = {
#include "stg13/terrain/draw_639_vertices.inc"
};
static NJS_CNK_MODEL draw_639_model = {
#include "stg13/terrain/draw_639_model.inc"
};
static NJS_CNK_OBJECT draw_2202 = {
#include "stg13/terrain/draw_2202.inc"
};
static Sint16 draw_121_polygons[23] = {
#include "stg13/terrain/draw_121_polygons.inc"
};
static Sint32 draw_121_vertices[19] = {
#include "stg13/terrain/draw_121_vertices.inc"
};
static NJS_CNK_MODEL draw_121_model = {
#include "stg13/terrain/draw_121_model.inc"
};
static NJS_CNK_OBJECT draw_2201 = {
#include "stg13/terrain/draw_2201.inc"
};
static Sint16 draw_517_polygons[59] = {
#include "stg13/terrain/draw_517_polygons.inc"
};
static Sint32 draw_517_vertices[67] = {
#include "stg13/terrain/draw_517_vertices.inc"
};
static NJS_CNK_MODEL draw_517_model = {
#include "stg13/terrain/draw_517_model.inc"
};
static NJS_CNK_OBJECT draw_2200 = {
#include "stg13/terrain/draw_2200.inc"
};
static Sint16 draw_519_polygons[59] = {
#include "stg13/terrain/draw_519_polygons.inc"
};
static Sint32 draw_519_vertices[67] = {
#include "stg13/terrain/draw_519_vertices.inc"
};
static NJS_CNK_MODEL draw_519_model = {
#include "stg13/terrain/draw_519_model.inc"
};
static NJS_CNK_OBJECT draw_2199 = {
#include "stg13/terrain/draw_2199.inc"
};
static Sint16 draw_206_polygons[91] = {
#include "stg13/terrain/draw_206_polygons.inc"
};
static Sint32 draw_206_vertices[103] = {
#include "stg13/terrain/draw_206_vertices.inc"
};
static NJS_CNK_MODEL draw_206_model = {
#include "stg13/terrain/draw_206_model.inc"
};
static NJS_CNK_OBJECT draw_2197 = {
#include "stg13/terrain/draw_2197.inc"
};
static Sint16 draw_220_polygons[47] = {
#include "stg13/terrain/draw_220_polygons.inc"
};
static Sint32 draw_220_vertices[31] = {
#include "stg13/terrain/draw_220_vertices.inc"
};
static NJS_CNK_MODEL draw_220_model = {
#include "stg13/terrain/draw_220_model.inc"
};
static NJS_CNK_OBJECT draw_2194 = {
#include "stg13/terrain/draw_2194.inc"
};
static Sint16 draw_116_polygons[123] = {
#include "stg13/terrain/draw_116_polygons.inc"
};
static Sint32 draw_116_vertices[87] = {
#include "stg13/terrain/draw_116_vertices.inc"
};
static NJS_CNK_MODEL draw_116_model = {
#include "stg13/terrain/draw_116_model.inc"
};
static NJS_CNK_OBJECT draw_2193 = {
#include "stg13/terrain/draw_2193.inc"
};
static Sint16 draw_114_polygons[123] = {
#include "stg13/terrain/draw_114_polygons.inc"
};
static Sint32 draw_114_vertices[87] = {
#include "stg13/terrain/draw_114_vertices.inc"
};
static NJS_CNK_MODEL draw_114_model = {
#include "stg13/terrain/draw_114_model.inc"
};
static NJS_CNK_OBJECT draw_2191 = {
#include "stg13/terrain/draw_2191.inc"
};
static Sint16 draw_298_polygons[59] = {
#include "stg13/terrain/draw_298_polygons.inc"
};
static Sint32 draw_298_vertices[67] = {
#include "stg13/terrain/draw_298_vertices.inc"
};
static NJS_CNK_MODEL draw_298_model = {
#include "stg13/terrain/draw_298_model.inc"
};
static NJS_CNK_OBJECT draw_2187 = {
#include "stg13/terrain/draw_2187.inc"
};
static Sint16 draw_638_polygons[433] = {
#include "stg13/terrain/draw_638_polygons.inc"
};
static Sint32 draw_638_vertices[395] = {
#include "stg13/terrain/draw_638_vertices.inc"
};
static NJS_CNK_MODEL draw_638_model = {
#include "stg13/terrain/draw_638_model.inc"
};
static NJS_CNK_OBJECT draw_2185 = {
#include "stg13/terrain/draw_2185.inc"
};
static Sint16 draw_637_polygons[439] = {
#include "stg13/terrain/draw_637_polygons.inc"
};
static Sint32 draw_637_vertices[387] = {
#include "stg13/terrain/draw_637_vertices.inc"
};
static NJS_CNK_MODEL draw_637_model = {
#include "stg13/terrain/draw_637_model.inc"
};
static NJS_CNK_OBJECT draw_2184 = {
#include "stg13/terrain/draw_2184.inc"
};
static Sint16 draw_180_polygons[23] = {
#include "stg13/terrain/draw_180_polygons.inc"
};
static Sint32 draw_180_vertices[19] = {
#include "stg13/terrain/draw_180_vertices.inc"
};
static NJS_CNK_MODEL draw_180_model = {
#include "stg13/terrain/draw_180_model.inc"
};
static NJS_CNK_OBJECT draw_2182 = {
#include "stg13/terrain/draw_2182.inc"
};
static Sint16 draw_83_polygons[37] = {
#include "stg13/terrain/draw_83_polygons.inc"
};
static Sint32 draw_83_vertices[35] = {
#include "stg13/terrain/draw_83_vertices.inc"
};
static NJS_CNK_MODEL draw_83_model = {
#include "stg13/terrain/draw_83_model.inc"
};
static NJS_CNK_OBJECT draw_2113 = {
#include "stg13/terrain/draw_2113.inc"
};
static Sint16 draw_94_polygons[155] = {
#include "stg13/terrain/draw_94_polygons.inc"
};
static Sint32 draw_94_vertices[147] = {
#include "stg13/terrain/draw_94_vertices.inc"
};
static NJS_CNK_MODEL draw_94_model = {
#include "stg13/terrain/draw_94_model.inc"
};
static NJS_CNK_OBJECT draw_2053 = {
#include "stg13/terrain/draw_2053.inc"
};
static Sint16 draw_2047_polygons[109] = {
#include "stg13/terrain/draw_2047_polygons.inc"
};
static Sint32 draw_2047_vertices[131] = {
#include "stg13/terrain/draw_2047_vertices.inc"
};
static NJS_CNK_MODEL draw_2047_model = {
#include "stg13/terrain/draw_2047_model.inc"
};
static NJS_CNK_OBJECT draw_2047 = {
#include "stg13/terrain/draw_2047.inc"
};
static Sint16 draw_2044_polygons[115] = {
#include "stg13/terrain/draw_2044_polygons.inc"
};
static Sint32 draw_2044_vertices[135] = {
#include "stg13/terrain/draw_2044_vertices.inc"
};
static NJS_CNK_MODEL draw_2044_model = {
#include "stg13/terrain/draw_2044_model.inc"
};
static NJS_CNK_OBJECT draw_2044 = {
#include "stg13/terrain/draw_2044.inc"
};
static Sint16 draw_160_polygons[673] = {
#include "stg13/terrain/draw_160_polygons.inc"
};
static Sint32 draw_160_vertices[643] = {
#include "stg13/terrain/draw_160_vertices.inc"
};
static NJS_CNK_MODEL draw_160_model = {
#include "stg13/terrain/draw_160_model.inc"
};
static NJS_CNK_OBJECT draw_2042 = {
#include "stg13/terrain/draw_2042.inc"
};
static Sint16 draw_166_polygons[69] = {
#include "stg13/terrain/draw_166_polygons.inc"
};
static Sint32 draw_166_vertices[35] = {
#include "stg13/terrain/draw_166_vertices.inc"
};
static NJS_CNK_MODEL draw_166_model = {
#include "stg13/terrain/draw_166_model.inc"
};
static NJS_CNK_OBJECT draw_2041 = {
#include "stg13/terrain/draw_2041.inc"
};
static Sint16 draw_72_polygons[185] = {
#include "stg13/terrain/draw_72_polygons.inc"
};
static Sint32 draw_72_vertices[151] = {
#include "stg13/terrain/draw_72_vertices.inc"
};
static NJS_CNK_MODEL draw_72_model = {
#include "stg13/terrain/draw_72_model.inc"
};
static NJS_CNK_OBJECT draw_2039 = {
#include "stg13/terrain/draw_2039.inc"
};
static Sint16 draw_266_polygons[183] = {
#include "stg13/terrain/draw_266_polygons.inc"
};
static Sint32 draw_266_vertices[131] = {
#include "stg13/terrain/draw_266_vertices.inc"
};
static NJS_CNK_MODEL draw_266_model = {
#include "stg13/terrain/draw_266_model.inc"
};
static NJS_CNK_OBJECT draw_2038 = {
#include "stg13/terrain/draw_2038.inc"
};
static Sint16 draw_212_polygons[227] = {
#include "stg13/terrain/draw_212_polygons.inc"
};
static Sint32 draw_212_vertices[143] = {
#include "stg13/terrain/draw_212_vertices.inc"
};
static NJS_CNK_MODEL draw_212_model = {
#include "stg13/terrain/draw_212_model.inc"
};
static NJS_CNK_OBJECT draw_2037 = {
#include "stg13/terrain/draw_2037.inc"
};
static Sint16 draw_692_polygons[1229] = {
#include "stg13/terrain/draw_692_polygons.inc"
};
static Sint32 draw_692_vertices[1303] = {
#include "stg13/terrain/draw_692_vertices.inc"
};
static NJS_CNK_MODEL draw_692_model = {
#include "stg13/terrain/draw_692_model.inc"
};
static NJS_CNK_OBJECT draw_2032 = {
#include "stg13/terrain/draw_2032.inc"
};
static Sint16 draw_74_polygons[227] = {
#include "stg13/terrain/draw_74_polygons.inc"
};
static Sint32 draw_74_vertices[163] = {
#include "stg13/terrain/draw_74_vertices.inc"
};
static NJS_CNK_MODEL draw_74_model = {
#include "stg13/terrain/draw_74_model.inc"
};
static NJS_CNK_OBJECT draw_2031 = {
#include "stg13/terrain/draw_2031.inc"
};
static Sint16 draw_297_polygons[77] = {
#include "stg13/terrain/draw_297_polygons.inc"
};
static Sint32 draw_297_vertices[91] = {
#include "stg13/terrain/draw_297_vertices.inc"
};
static NJS_CNK_MODEL draw_297_model = {
#include "stg13/terrain/draw_297_model.inc"
};
static NJS_CNK_OBJECT draw_2024 = {
#include "stg13/terrain/draw_2024.inc"
};
static Sint16 draw_178_polygons[401] = {
#include "stg13/terrain/draw_178_polygons.inc"
};
static Sint32 draw_178_vertices[391] = {
#include "stg13/terrain/draw_178_vertices.inc"
};
static NJS_CNK_MODEL draw_178_model = {
#include "stg13/terrain/draw_178_model.inc"
};
static NJS_CNK_OBJECT draw_2018 = {
#include "stg13/terrain/draw_2018.inc"
};
static Sint16 draw_291_polygons[587] = {
#include "stg13/terrain/draw_291_polygons.inc"
};
static Sint32 draw_291_vertices[539] = {
#include "stg13/terrain/draw_291_vertices.inc"
};
static NJS_CNK_MODEL draw_291_model = {
#include "stg13/terrain/draw_291_model.inc"
};
static NJS_CNK_OBJECT draw_2017 = {
#include "stg13/terrain/draw_2017.inc"
};
static Sint16 draw_1356_polygons[413] = {
#include "stg13/terrain/draw_1356_polygons.inc"
};
static Sint32 draw_1356_vertices[363] = {
#include "stg13/terrain/draw_1356_vertices.inc"
};
static NJS_CNK_MODEL draw_1356_model = {
#include "stg13/terrain/draw_1356_model.inc"
};
static NJS_CNK_OBJECT draw_2016 = {
#include "stg13/terrain/draw_2016.inc"
};
static Sint16 draw_109_polygons[215] = {
#include "stg13/terrain/draw_109_polygons.inc"
};
static Sint32 draw_109_vertices[155] = {
#include "stg13/terrain/draw_109_vertices.inc"
};
static NJS_CNK_MODEL draw_109_model = {
#include "stg13/terrain/draw_109_model.inc"
};
static NJS_CNK_OBJECT draw_2012 = {
#include "stg13/terrain/draw_2012.inc"
};
static Sint16 draw_674_polygons[1177] = {
#include "stg13/terrain/draw_674_polygons.inc"
};
static Sint32 draw_674_vertices[1235] = {
#include "stg13/terrain/draw_674_vertices.inc"
};
static NJS_CNK_MODEL draw_674_model = {
#include "stg13/terrain/draw_674_model.inc"
};
static NJS_CNK_OBJECT draw_2011 = {
#include "stg13/terrain/draw_2011.inc"
};
static Sint16 draw_633_polygons[113] = {
#include "stg13/terrain/draw_633_polygons.inc"
};
static Sint32 draw_633_vertices[75] = {
#include "stg13/terrain/draw_633_vertices.inc"
};
static NJS_CNK_MODEL draw_633_model = {
#include "stg13/terrain/draw_633_model.inc"
};
static NJS_CNK_OBJECT draw_2009 = {
#include "stg13/terrain/draw_2009.inc"
};
static Sint16 draw_287_polygons[449] = {
#include "stg13/terrain/draw_287_polygons.inc"
};
static Sint32 draw_287_vertices[443] = {
#include "stg13/terrain/draw_287_vertices.inc"
};
static NJS_CNK_MODEL draw_287_model = {
#include "stg13/terrain/draw_287_model.inc"
};
static NJS_CNK_OBJECT draw_2006 = {
#include "stg13/terrain/draw_2006.inc"
};
static Sint16 draw_286_polygons[61] = {
#include "stg13/terrain/draw_286_polygons.inc"
};
static Sint32 draw_286_vertices[47] = {
#include "stg13/terrain/draw_286_vertices.inc"
};
static NJS_CNK_MODEL draw_286_model = {
#include "stg13/terrain/draw_286_model.inc"
};
static NJS_CNK_OBJECT draw_2000 = {
#include "stg13/terrain/draw_2000.inc"
};
static Sint16 draw_1259_polygons[41] = {
#include "stg13/terrain/draw_1259_polygons.inc"
};
static Sint32 draw_1259_vertices[35] = {
#include "stg13/terrain/draw_1259_vertices.inc"
};
static NJS_CNK_MODEL draw_1259_model = {
#include "stg13/terrain/draw_1259_model.inc"
};
static NJS_CNK_OBJECT draw_1999 = {
#include "stg13/terrain/draw_1999.inc"
};
static Sint16 draw_282_polygons[43] = {
#include "stg13/terrain/draw_282_polygons.inc"
};
static Sint32 draw_282_vertices[35] = {
#include "stg13/terrain/draw_282_vertices.inc"
};
static NJS_CNK_MODEL draw_282_model = {
#include "stg13/terrain/draw_282_model.inc"
};
static NJS_CNK_OBJECT draw_1997 = {
#include "stg13/terrain/draw_1997.inc"
};
static Sint16 draw_135_polygons[617] = {
#include "stg13/terrain/draw_135_polygons.inc"
};
static Sint32 draw_135_vertices[475] = {
#include "stg13/terrain/draw_135_vertices.inc"
};
static NJS_CNK_MODEL draw_135_model = {
#include "stg13/terrain/draw_135_model.inc"
};
static NJS_CNK_OBJECT draw_1994 = {
#include "stg13/terrain/draw_1994.inc"
};
static Sint16 draw_158_polygons[41] = {
#include "stg13/terrain/draw_158_polygons.inc"
};
static Sint32 draw_158_vertices[35] = {
#include "stg13/terrain/draw_158_vertices.inc"
};
static NJS_CNK_MODEL draw_158_model = {
#include "stg13/terrain/draw_158_model.inc"
};
static NJS_CNK_OBJECT draw_1993 = {
#include "stg13/terrain/draw_1993.inc"
};
static Sint16 draw_281_polygons[67] = {
#include "stg13/terrain/draw_281_polygons.inc"
};
static Sint32 draw_281_vertices[47] = {
#include "stg13/terrain/draw_281_vertices.inc"
};
static NJS_CNK_MODEL draw_281_model = {
#include "stg13/terrain/draw_281_model.inc"
};
static NJS_CNK_OBJECT draw_1991 = {
#include "stg13/terrain/draw_1991.inc"
};
static Sint16 draw_617_polygons[815] = {
#include "stg13/terrain/draw_617_polygons.inc"
};
static Sint32 draw_617_vertices[791] = {
#include "stg13/terrain/draw_617_vertices.inc"
};
static NJS_CNK_MODEL draw_617_model = {
#include "stg13/terrain/draw_617_model.inc"
};
static NJS_CNK_OBJECT draw_1987 = {
#include "stg13/terrain/draw_1987.inc"
};
static Sint16 draw_132_polygons[153] = {
#include "stg13/terrain/draw_132_polygons.inc"
};
static Sint32 draw_132_vertices[155] = {
#include "stg13/terrain/draw_132_vertices.inc"
};
static NJS_CNK_MODEL draw_132_model = {
#include "stg13/terrain/draw_132_model.inc"
};
static NJS_CNK_OBJECT draw_1986 = {
#include "stg13/terrain/draw_1986.inc"
};
static Sint16 draw_1341_polygons[435] = {
#include "stg13/terrain/draw_1341_polygons.inc"
};
static Sint32 draw_1341_vertices[403] = {
#include "stg13/terrain/draw_1341_vertices.inc"
};
static NJS_CNK_MODEL draw_1341_model = {
#include "stg13/terrain/draw_1341_model.inc"
};
static NJS_CNK_OBJECT draw_1985 = {
#include "stg13/terrain/draw_1985.inc"
};
static Sint16 draw_615_polygons[59] = {
#include "stg13/terrain/draw_615_polygons.inc"
};
static Sint32 draw_615_vertices[51] = {
#include "stg13/terrain/draw_615_vertices.inc"
};
static NJS_CNK_MODEL draw_615_model = {
#include "stg13/terrain/draw_615_model.inc"
};
static NJS_CNK_OBJECT draw_1984 = {
#include "stg13/terrain/draw_1984.inc"
};
static Sint16 draw_279_polygons[1023] = {
#include "stg13/terrain/draw_279_polygons.inc"
};
static Sint32 draw_279_vertices[1019] = {
#include "stg13/terrain/draw_279_vertices.inc"
};
static NJS_CNK_MODEL draw_279_model = {
#include "stg13/terrain/draw_279_model.inc"
};
static NJS_CNK_OBJECT draw_1982 = {
#include "stg13/terrain/draw_1982.inc"
};
static Sint16 draw_110_polygons[141] = {
#include "stg13/terrain/draw_110_polygons.inc"
};
static Sint32 draw_110_vertices[111] = {
#include "stg13/terrain/draw_110_vertices.inc"
};
static NJS_CNK_MODEL draw_110_model = {
#include "stg13/terrain/draw_110_model.inc"
};
static NJS_CNK_OBJECT draw_1981 = {
#include "stg13/terrain/draw_1981.inc"
};
static Sint16 draw_277_polygons[59] = {
#include "stg13/terrain/draw_277_polygons.inc"
};
static Sint32 draw_277_vertices[51] = {
#include "stg13/terrain/draw_277_vertices.inc"
};
static NJS_CNK_MODEL draw_277_model = {
#include "stg13/terrain/draw_277_model.inc"
};
static NJS_CNK_OBJECT draw_1980 = {
#include "stg13/terrain/draw_1980.inc"
};
static Sint16 draw_205_polygons[293] = {
#include "stg13/terrain/draw_205_polygons.inc"
};
static Sint32 draw_205_vertices[267] = {
#include "stg13/terrain/draw_205_vertices.inc"
};
static NJS_CNK_MODEL draw_205_model = {
#include "stg13/terrain/draw_205_model.inc"
};
static NJS_CNK_OBJECT draw_1979 = {
#include "stg13/terrain/draw_1979.inc"
};
static Sint16 draw_272_polygons[547] = {
#include "stg13/terrain/draw_272_polygons.inc"
};
static Sint32 draw_272_vertices[563] = {
#include "stg13/terrain/draw_272_vertices.inc"
};
static NJS_CNK_MODEL draw_272_model = {
#include "stg13/terrain/draw_272_model.inc"
};
static NJS_CNK_OBJECT draw_1978 = {
#include "stg13/terrain/draw_1978.inc"
};
static Sint16 draw_107_polygons[215] = {
#include "stg13/terrain/draw_107_polygons.inc"
};
static Sint32 draw_107_vertices[211] = {
#include "stg13/terrain/draw_107_vertices.inc"
};
static NJS_CNK_MODEL draw_107_model = {
#include "stg13/terrain/draw_107_model.inc"
};
static NJS_CNK_OBJECT draw_1977 = {
#include "stg13/terrain/draw_1977.inc"
};
static Sint16 draw_673_polygons[43] = {
#include "stg13/terrain/draw_673_polygons.inc"
};
static Sint32 draw_673_vertices[27] = {
#include "stg13/terrain/draw_673_vertices.inc"
};
static NJS_CNK_MODEL draw_673_model = {
#include "stg13/terrain/draw_673_model.inc"
};
static NJS_CNK_OBJECT draw_1976 = {
#include "stg13/terrain/draw_1976.inc"
};
static Sint16 draw_270_polygons[1129] = {
#include "stg13/terrain/draw_270_polygons.inc"
};
static Sint32 draw_270_vertices[891] = {
#include "stg13/terrain/draw_270_vertices.inc"
};
static NJS_CNK_MODEL draw_270_model = {
#include "stg13/terrain/draw_270_model.inc"
};
static NJS_CNK_OBJECT draw_1975 = {
#include "stg13/terrain/draw_1975.inc"
};
static Sint16 draw_672_polygons[279] = {
#include "stg13/terrain/draw_672_polygons.inc"
};
static Sint32 draw_672_vertices[279] = {
#include "stg13/terrain/draw_672_vertices.inc"
};
static NJS_CNK_MODEL draw_672_model = {
#include "stg13/terrain/draw_672_model.inc"
};
static NJS_CNK_OBJECT draw_1974 = {
#include "stg13/terrain/draw_1974.inc"
};
static Sint16 draw_268_polygons[81] = {
#include "stg13/terrain/draw_268_polygons.inc"
};
static Sint32 draw_268_vertices[67] = {
#include "stg13/terrain/draw_268_vertices.inc"
};
static NJS_CNK_MODEL draw_268_model = {
#include "stg13/terrain/draw_268_model.inc"
};
static NJS_CNK_OBJECT draw_1973 = {
#include "stg13/terrain/draw_1973.inc"
};
static Sint16 draw_588_polygons[213] = {
#include "stg13/terrain/draw_588_polygons.inc"
};
static Sint32 draw_588_vertices[171] = {
#include "stg13/terrain/draw_588_vertices.inc"
};
static NJS_CNK_MODEL draw_588_model = {
#include "stg13/terrain/draw_588_model.inc"
};
static NJS_CNK_OBJECT draw_1972 = {
#include "stg13/terrain/draw_1972.inc"
};
static Sint16 draw_262_polygons[281] = {
#include "stg13/terrain/draw_262_polygons.inc"
};
static Sint32 draw_262_vertices[211] = {
#include "stg13/terrain/draw_262_vertices.inc"
};
static NJS_CNK_MODEL draw_262_model = {
#include "stg13/terrain/draw_262_model.inc"
};
static NJS_CNK_OBJECT draw_1971 = {
#include "stg13/terrain/draw_1971.inc"
};
static Sint16 draw_296_polygons[327] = {
#include "stg13/terrain/draw_296_polygons.inc"
};
static Sint32 draw_296_vertices[311] = {
#include "stg13/terrain/draw_296_vertices.inc"
};
static NJS_CNK_MODEL draw_296_model = {
#include "stg13/terrain/draw_296_model.inc"
};
static NJS_CNK_OBJECT draw_1970 = {
#include "stg13/terrain/draw_1970.inc"
};
static Sint16 draw_259_polygons[429] = {
#include "stg13/terrain/draw_259_polygons.inc"
};
static Sint32 draw_259_vertices[411] = {
#include "stg13/terrain/draw_259_vertices.inc"
};
static NJS_CNK_MODEL draw_259_model = {
#include "stg13/terrain/draw_259_model.inc"
};
static NJS_CNK_OBJECT draw_1969 = {
#include "stg13/terrain/draw_1969.inc"
};
static Sint16 draw_202_polygons[357] = {
#include "stg13/terrain/draw_202_polygons.inc"
};
static Sint32 draw_202_vertices[343] = {
#include "stg13/terrain/draw_202_vertices.inc"
};
static NJS_CNK_MODEL draw_202_model = {
#include "stg13/terrain/draw_202_model.inc"
};
static NJS_CNK_OBJECT draw_1968 = {
#include "stg13/terrain/draw_1968.inc"
};
static Sint16 draw_254_polygons[271] = {
#include "stg13/terrain/draw_254_polygons.inc"
};
static Sint32 draw_254_vertices[191] = {
#include "stg13/terrain/draw_254_vertices.inc"
};
static NJS_CNK_MODEL draw_254_model = {
#include "stg13/terrain/draw_254_model.inc"
};
static NJS_CNK_OBJECT draw_1967 = {
#include "stg13/terrain/draw_1967.inc"
};
static Sint16 draw_252_polygons[747] = {
#include "stg13/terrain/draw_252_polygons.inc"
};
static Sint32 draw_252_vertices[743] = {
#include "stg13/terrain/draw_252_vertices.inc"
};
static NJS_CNK_MODEL draw_252_model = {
#include "stg13/terrain/draw_252_model.inc"
};
static NJS_CNK_OBJECT draw_1966 = {
#include "stg13/terrain/draw_1966.inc"
};
static Sint16 draw_250_polygons[73] = {
#include "stg13/terrain/draw_250_polygons.inc"
};
static Sint32 draw_250_vertices[67] = {
#include "stg13/terrain/draw_250_vertices.inc"
};
static NJS_CNK_MODEL draw_250_model = {
#include "stg13/terrain/draw_250_model.inc"
};
static NJS_CNK_OBJECT draw_1965 = {
#include "stg13/terrain/draw_1965.inc"
};
static Sint16 draw_201_polygons[123] = {
#include "stg13/terrain/draw_201_polygons.inc"
};
static Sint32 draw_201_vertices[107] = {
#include "stg13/terrain/draw_201_vertices.inc"
};
static NJS_CNK_MODEL draw_201_model = {
#include "stg13/terrain/draw_201_model.inc"
};
static NJS_CNK_OBJECT draw_1964 = {
#include "stg13/terrain/draw_1964.inc"
};
static Sint16 draw_200_polygons[423] = {
#include "stg13/terrain/draw_200_polygons.inc"
};
static Sint32 draw_200_vertices[479] = {
#include "stg13/terrain/draw_200_vertices.inc"
};
static NJS_CNK_MODEL draw_200_model = {
#include "stg13/terrain/draw_200_model.inc"
};
static NJS_CNK_OBJECT draw_1963 = {
#include "stg13/terrain/draw_1963.inc"
};
static Sint16 draw_240_polygons[431] = {
#include "stg13/terrain/draw_240_polygons.inc"
};
static Sint32 draw_240_vertices[455] = {
#include "stg13/terrain/draw_240_vertices.inc"
};
static NJS_CNK_MODEL draw_240_model = {
#include "stg13/terrain/draw_240_model.inc"
};
static NJS_CNK_OBJECT draw_1962 = {
#include "stg13/terrain/draw_1962.inc"
};
static Sint16 draw_238_polygons[59] = {
#include "stg13/terrain/draw_238_polygons.inc"
};
static Sint32 draw_238_vertices[51] = {
#include "stg13/terrain/draw_238_vertices.inc"
};
static NJS_CNK_MODEL draw_238_model = {
#include "stg13/terrain/draw_238_model.inc"
};
static NJS_CNK_OBJECT draw_1961 = {
#include "stg13/terrain/draw_1961.inc"
};
static Sint16 draw_199_polygons[633] = {
#include "stg13/terrain/draw_199_polygons.inc"
};
static Sint32 draw_199_vertices[515] = {
#include "stg13/terrain/draw_199_vertices.inc"
};
static NJS_CNK_MODEL draw_199_model = {
#include "stg13/terrain/draw_199_model.inc"
};
static NJS_CNK_OBJECT draw_1960 = {
#include "stg13/terrain/draw_1960.inc"
};
static Sint16 draw_234_polygons[59] = {
#include "stg13/terrain/draw_234_polygons.inc"
};
static Sint32 draw_234_vertices[51] = {
#include "stg13/terrain/draw_234_vertices.inc"
};
static NJS_CNK_MODEL draw_234_model = {
#include "stg13/terrain/draw_234_model.inc"
};
static NJS_CNK_OBJECT draw_1959 = {
#include "stg13/terrain/draw_1959.inc"
};
static Sint16 draw_232_polygons[621] = {
#include "stg13/terrain/draw_232_polygons.inc"
};
static Sint32 draw_232_vertices[575] = {
#include "stg13/terrain/draw_232_vertices.inc"
};
static NJS_CNK_MODEL draw_232_model = {
#include "stg13/terrain/draw_232_model.inc"
};
static NJS_CNK_OBJECT draw_1958 = {
#include "stg13/terrain/draw_1958.inc"
};
static Sint16 draw_230_polygons[77] = {
#include "stg13/terrain/draw_230_polygons.inc"
};
static Sint32 draw_230_vertices[59] = {
#include "stg13/terrain/draw_230_vertices.inc"
};
static NJS_CNK_MODEL draw_230_model = {
#include "stg13/terrain/draw_230_model.inc"
};
static NJS_CNK_OBJECT draw_1957 = {
#include "stg13/terrain/draw_1957.inc"
};
static Sint16 draw_100_polygons[429] = {
#include "stg13/terrain/draw_100_polygons.inc"
};
static Sint32 draw_100_vertices[475] = {
#include "stg13/terrain/draw_100_vertices.inc"
};
static NJS_CNK_MODEL draw_100_model = {
#include "stg13/terrain/draw_100_model.inc"
};
static NJS_CNK_OBJECT draw_1956 = {
#include "stg13/terrain/draw_1956.inc"
};
static Sint16 draw_98_polygons[55] = {
#include "stg13/terrain/draw_98_polygons.inc"
};
static Sint32 draw_98_vertices[51] = {
#include "stg13/terrain/draw_98_vertices.inc"
};
static NJS_CNK_MODEL draw_98_model = {
#include "stg13/terrain/draw_98_model.inc"
};
static NJS_CNK_OBJECT draw_1955 = {
#include "stg13/terrain/draw_1955.inc"
};
static Sint16 draw_228_polygons[59] = {
#include "stg13/terrain/draw_228_polygons.inc"
};
static Sint32 draw_228_vertices[43] = {
#include "stg13/terrain/draw_228_vertices.inc"
};
static NJS_CNK_MODEL draw_228_model = {
#include "stg13/terrain/draw_228_model.inc"
};
static NJS_CNK_OBJECT draw_1954 = {
#include "stg13/terrain/draw_1954.inc"
};
static Sint16 draw_190_polygons[1079] = {
#include "stg13/terrain/draw_190_polygons.inc"
};
static Sint32 draw_190_vertices[1031] = {
#include "stg13/terrain/draw_190_vertices.inc"
};
static NJS_CNK_MODEL draw_190_model = {
#include "stg13/terrain/draw_190_model.inc"
};
static NJS_CNK_OBJECT draw_1949 = {
#include "stg13/terrain/draw_1949.inc"
};
static Sint16 draw_189_polygons[169] = {
#include "stg13/terrain/draw_189_polygons.inc"
};
static Sint32 draw_189_vertices[159] = {
#include "stg13/terrain/draw_189_vertices.inc"
};
static NJS_CNK_MODEL draw_189_model = {
#include "stg13/terrain/draw_189_model.inc"
};
static NJS_CNK_OBJECT draw_1931 = {
#include "stg13/terrain/draw_1931.inc"
};
static Sint16 draw_85_polygons[1465] = {
#include "stg13/terrain/draw_85_polygons.inc"
};
static Sint32 draw_85_vertices[1151] = {
#include "stg13/terrain/draw_85_vertices.inc"
};
static NJS_CNK_MODEL draw_85_model = {
#include "stg13/terrain/draw_85_model.inc"
};
static NJS_CNK_OBJECT draw_1878 = {
#include "stg13/terrain/draw_1878.inc"
};
static Sint16 draw_142_polygons[1353] = {
#include "stg13/terrain/draw_142_polygons.inc"
};
static Sint32 draw_142_vertices[1423] = {
#include "stg13/terrain/draw_142_vertices.inc"
};
static NJS_CNK_MODEL draw_142_model = {
#include "stg13/terrain/draw_142_model.inc"
};
static NJS_CNK_OBJECT draw_1877 = {
#include "stg13/terrain/draw_1877.inc"
};
static Sint16 draw_82_polygons[153] = {
#include "stg13/terrain/draw_82_polygons.inc"
};
static Sint32 draw_82_vertices[115] = {
#include "stg13/terrain/draw_82_vertices.inc"
};
static NJS_CNK_MODEL draw_82_model = {
#include "stg13/terrain/draw_82_model.inc"
};
static NJS_CNK_OBJECT draw_1876 = {
#include "stg13/terrain/draw_1876.inc"
};
static Sint16 draw_81_polygons[149] = {
#include "stg13/terrain/draw_81_polygons.inc"
};
static Sint32 draw_81_vertices[111] = {
#include "stg13/terrain/draw_81_vertices.inc"
};
static NJS_CNK_MODEL draw_81_model = {
#include "stg13/terrain/draw_81_model.inc"
};
static NJS_CNK_OBJECT draw_1875 = {
#include "stg13/terrain/draw_1875.inc"
};
static Sint16 draw_75_polygons[571] = {
#include "stg13/terrain/draw_75_polygons.inc"
};
static Sint32 draw_75_vertices[519] = {
#include "stg13/terrain/draw_75_vertices.inc"
};
static NJS_CNK_MODEL draw_75_model = {
#include "stg13/terrain/draw_75_model.inc"
};
static NJS_CNK_OBJECT draw_1874 = {
#include "stg13/terrain/draw_1874.inc"
};
static Sint16 draw_89_polygons[233] = {
#include "stg13/terrain/draw_89_polygons.inc"
};
static Sint32 draw_89_vertices[187] = {
#include "stg13/terrain/draw_89_vertices.inc"
};
static NJS_CNK_MODEL draw_89_model = {
#include "stg13/terrain/draw_89_model.inc"
};
static NJS_CNK_OBJECT draw_1873 = {
#include "stg13/terrain/draw_1873.inc"
};
static Sint16 draw_88_polygons[643] = {
#include "stg13/terrain/draw_88_polygons.inc"
};
static Sint32 draw_88_vertices[563] = {
#include "stg13/terrain/draw_88_vertices.inc"
};
static NJS_CNK_MODEL draw_88_model = {
#include "stg13/terrain/draw_88_model.inc"
};
static NJS_CNK_OBJECT draw_1872 = {
#include "stg13/terrain/draw_1872.inc"
};
static Sint16 draw_175_polygons[533] = {
#include "stg13/terrain/draw_175_polygons.inc"
};
static Sint32 draw_175_vertices[559] = {
#include "stg13/terrain/draw_175_vertices.inc"
};
static NJS_CNK_MODEL draw_175_model = {
#include "stg13/terrain/draw_175_model.inc"
};
static NJS_CNK_OBJECT draw_1871 = {
#include "stg13/terrain/draw_1871.inc"
};
static Sint16 draw_86_polygons[91] = {
#include "stg13/terrain/draw_86_polygons.inc"
};
static Sint32 draw_86_vertices[71] = {
#include "stg13/terrain/draw_86_vertices.inc"
};
static NJS_CNK_MODEL draw_86_model = {
#include "stg13/terrain/draw_86_model.inc"
};
static NJS_CNK_OBJECT draw_1870 = {
#include "stg13/terrain/draw_1870.inc"
};
static Sint16 draw_170_polygons[579] = {
#include "stg13/terrain/draw_170_polygons.inc"
};
static Sint32 draw_170_vertices[615] = {
#include "stg13/terrain/draw_170_vertices.inc"
};
static NJS_CNK_MODEL draw_170_model = {
#include "stg13/terrain/draw_170_model.inc"
};
static NJS_CNK_OBJECT draw_1869 = {
#include "stg13/terrain/draw_1869.inc"
};
static Sint16 draw_172_polygons[379] = {
#include "stg13/terrain/draw_172_polygons.inc"
};
static Sint32 draw_172_vertices[379] = {
#include "stg13/terrain/draw_172_vertices.inc"
};
static NJS_CNK_MODEL draw_172_model = {
#include "stg13/terrain/draw_172_model.inc"
};
static NJS_CNK_OBJECT draw_1858 = {
#include "stg13/terrain/draw_1858.inc"
};
static Sint16 draw_952_polygons[273] = {
#include "stg13/terrain/draw_952_polygons.inc"
};
static Sint32 draw_952_vertices[235] = {
#include "stg13/terrain/draw_952_vertices.inc"
};
static NJS_CNK_MODEL draw_952_model = {
#include "stg13/terrain/draw_952_model.inc"
};
static NJS_CNK_OBJECT draw_1856 = {
#include "stg13/terrain/draw_1856.inc"
};
static Sint16 draw_183_polygons[487] = {
#include "stg13/terrain/draw_183_polygons.inc"
};
static Sint32 draw_183_vertices[439] = {
#include "stg13/terrain/draw_183_vertices.inc"
};
static NJS_CNK_MODEL draw_183_model = {
#include "stg13/terrain/draw_183_model.inc"
};
static NJS_CNK_OBJECT draw_1853 = {
#include "stg13/terrain/draw_1853.inc"
};
static Sint16 draw_127_polygons[899] = {
#include "stg13/terrain/draw_127_polygons.inc"
};
static Sint32 draw_127_vertices[847] = {
#include "stg13/terrain/draw_127_vertices.inc"
};
static NJS_CNK_MODEL draw_127_model = {
#include "stg13/terrain/draw_127_model.inc"
};
static NJS_CNK_OBJECT draw_1852 = {
#include "stg13/terrain/draw_1852.inc"
};
static Sint16 draw_167_polygons[587] = {
#include "stg13/terrain/draw_167_polygons.inc"
};
static Sint32 draw_167_vertices[599] = {
#include "stg13/terrain/draw_167_vertices.inc"
};
static NJS_CNK_MODEL draw_167_model = {
#include "stg13/terrain/draw_167_model.inc"
};
static NJS_CNK_OBJECT draw_1851 = {
#include "stg13/terrain/draw_1851.inc"
};
static Sint16 draw_164_polygons[165] = {
#include "stg13/terrain/draw_164_polygons.inc"
};
static Sint32 draw_164_vertices[115] = {
#include "stg13/terrain/draw_164_vertices.inc"
};
static NJS_CNK_MODEL draw_164_model = {
#include "stg13/terrain/draw_164_model.inc"
};
static NJS_CNK_OBJECT draw_1847 = {
#include "stg13/terrain/draw_1847.inc"
};
static Sint16 draw_187_polygons[199] = {
#include "stg13/terrain/draw_187_polygons.inc"
};
static Sint32 draw_187_vertices[171] = {
#include "stg13/terrain/draw_187_vertices.inc"
};
static NJS_CNK_MODEL draw_187_model = {
#include "stg13/terrain/draw_187_model.inc"
};
static NJS_CNK_OBJECT draw_1846 = {
#include "stg13/terrain/draw_1846.inc"
};
static Sint16 draw_152_polygons[695] = {
#include "stg13/terrain/draw_152_polygons.inc"
};
static Sint32 draw_152_vertices[563] = {
#include "stg13/terrain/draw_152_vertices.inc"
};
static NJS_CNK_MODEL draw_152_model = {
#include "stg13/terrain/draw_152_model.inc"
};
static NJS_CNK_OBJECT draw_1842 = {
#include "stg13/terrain/draw_1842.inc"
};
static Sint16 draw_150_polygons[659] = {
#include "stg13/terrain/draw_150_polygons.inc"
};
static Sint32 draw_150_vertices[543] = {
#include "stg13/terrain/draw_150_vertices.inc"
};
static NJS_CNK_MODEL draw_150_model = {
#include "stg13/terrain/draw_150_model.inc"
};
static NJS_CNK_OBJECT draw_1837 = {
#include "stg13/terrain/draw_1837.inc"
};
static Sint16 draw_76_polygons[833] = {
#include "stg13/terrain/draw_76_polygons.inc"
};
static Sint32 draw_76_vertices[727] = {
#include "stg13/terrain/draw_76_vertices.inc"
};
static NJS_CNK_MODEL draw_76_model = {
#include "stg13/terrain/draw_76_model.inc"
};
static NJS_CNK_OBJECT draw_1834 = {
#include "stg13/terrain/draw_1834.inc"
};
static Sint16 draw_144_polygons[65] = {
#include "stg13/terrain/draw_144_polygons.inc"
};
static Sint32 draw_144_vertices[75] = {
#include "stg13/terrain/draw_144_vertices.inc"
};
static NJS_CNK_MODEL draw_144_model = {
#include "stg13/terrain/draw_144_model.inc"
};
static NJS_CNK_OBJECT draw_1828 = {
#include "stg13/terrain/draw_1828.inc"
};
static Sint16 draw_138_polygons[897] = {
#include "stg13/terrain/draw_138_polygons.inc"
};
static Sint32 draw_138_vertices[731] = {
#include "stg13/terrain/draw_138_vertices.inc"
};
static NJS_CNK_MODEL draw_138_model = {
#include "stg13/terrain/draw_138_model.inc"
};
static NJS_CNK_OBJECT draw_1825 = {
#include "stg13/terrain/draw_1825.inc"
};
static Sint16 draw_162_polygons[55] = {
#include "stg13/terrain/draw_162_polygons.inc"
};
static Sint32 draw_162_vertices[43] = {
#include "stg13/terrain/draw_162_vertices.inc"
};
static NJS_CNK_MODEL draw_162_model = {
#include "stg13/terrain/draw_162_model.inc"
};
static NJS_CNK_OBJECT draw_1803 = {
#include "stg13/terrain/draw_1803.inc"
};
static Sint16 draw_78_polygons[453] = {
#include "stg13/terrain/draw_78_polygons.inc"
};
static Sint32 draw_78_vertices[407] = {
#include "stg13/terrain/draw_78_vertices.inc"
};
static NJS_CNK_MODEL draw_78_model = {
#include "stg13/terrain/draw_78_model.inc"
};
static NJS_CNK_OBJECT draw_1768 = {
#include "stg13/terrain/draw_1768.inc"
};
static Sint16 draw_131_polygons[257] = {
#include "stg13/terrain/draw_131_polygons.inc"
};
static Sint32 draw_131_vertices[199] = {
#include "stg13/terrain/draw_131_vertices.inc"
};
static NJS_CNK_MODEL draw_131_model = {
#include "stg13/terrain/draw_131_model.inc"
};
static NJS_CNK_OBJECT draw_1767 = {
#include "stg13/terrain/draw_1767.inc"
};
static Sint16 draw_687_polygons[989] = {
#include "stg13/terrain/draw_687_polygons.inc"
};
static Sint32 draw_687_vertices[903] = {
#include "stg13/terrain/draw_687_vertices.inc"
};
static NJS_CNK_MODEL draw_687_model = {
#include "stg13/terrain/draw_687_model.inc"
};
static NJS_CNK_OBJECT draw_1766 = {
#include "stg13/terrain/draw_1766.inc"
};
static Sint16 draw_686_polygons[359] = {
#include "stg13/terrain/draw_686_polygons.inc"
};
static Sint32 draw_686_vertices[307] = {
#include "stg13/terrain/draw_686_vertices.inc"
};
static NJS_CNK_MODEL draw_686_model = {
#include "stg13/terrain/draw_686_model.inc"
};
static NJS_CNK_OBJECT draw_1763 = {
#include "stg13/terrain/draw_1763.inc"
};
static Sint16 draw_113_polygons[147] = {
#include "stg13/terrain/draw_113_polygons.inc"
};
static Sint32 draw_113_vertices[135] = {
#include "stg13/terrain/draw_113_vertices.inc"
};
static NJS_CNK_MODEL draw_113_model = {
#include "stg13/terrain/draw_113_model.inc"
};
static NJS_CNK_OBJECT draw_1744 = {
#include "stg13/terrain/draw_1744.inc"
};
static Sint16 draw_112_polygons[23] = {
#include "stg13/terrain/draw_112_polygons.inc"
};
static Sint32 draw_112_vertices[19] = {
#include "stg13/terrain/draw_112_vertices.inc"
};
static NJS_CNK_MODEL draw_112_model = {
#include "stg13/terrain/draw_112_model.inc"
};
static NJS_CNK_OBJECT draw_1743 = {
#include "stg13/terrain/draw_1743.inc"
};
static Sint16 draw_106_polygons[293] = {
#include "stg13/terrain/draw_106_polygons.inc"
};
static Sint32 draw_106_vertices[215] = {
#include "stg13/terrain/draw_106_vertices.inc"
};
static NJS_CNK_MODEL draw_106_model = {
#include "stg13/terrain/draw_106_model.inc"
};
static NJS_CNK_OBJECT draw_1672 = {
#include "stg13/terrain/draw_1672.inc"
};
static Sint16 draw_92_polygons[181] = {
#include "stg13/terrain/draw_92_polygons.inc"
};
static Sint32 draw_92_vertices[131] = {
#include "stg13/terrain/draw_92_vertices.inc"
};
static NJS_CNK_MODEL draw_92_model = {
#include "stg13/terrain/draw_92_model.inc"
};
static NJS_CNK_OBJECT draw_1657 = {
#include "stg13/terrain/draw_1657.inc"
};
static Sint16 draw_90_polygons[87] = {
#include "stg13/terrain/draw_90_polygons.inc"
};
static Sint32 draw_90_vertices[63] = {
#include "stg13/terrain/draw_90_vertices.inc"
};
static NJS_CNK_MODEL draw_90_model = {
#include "stg13/terrain/draw_90_model.inc"
};
static NJS_CNK_OBJECT draw_1655 = {
#include "stg13/terrain/draw_1655.inc"
};
static Sint16 draw_87_polygons[813] = {
#include "stg13/terrain/draw_87_polygons.inc"
};
static Sint32 draw_87_vertices[807] = {
#include "stg13/terrain/draw_87_vertices.inc"
};
static NJS_CNK_MODEL draw_87_model = {
#include "stg13/terrain/draw_87_model.inc"
};
static NJS_CNK_OBJECT draw_1640 = {
#include "stg13/terrain/draw_1640.inc"
};
static Sint16 draw_841_polygons[205] = {
#include "stg13/terrain/draw_841_polygons.inc"
};
static Sint32 draw_841_vertices[187] = {
#include "stg13/terrain/draw_841_vertices.inc"
};
static NJS_CNK_MODEL draw_841_model = {
#include "stg13/terrain/draw_841_model.inc"
};
static NJS_CNK_OBJECT draw_1639 = {
#include "stg13/terrain/draw_1639.inc"
};
static Sint16 draw_1637_polygons[33] = {
#include "stg13/terrain/draw_1637_polygons.inc"
};
static Sint32 draw_1637_vertices[35] = {
#include "stg13/terrain/draw_1637_vertices.inc"
};
static NJS_CNK_MODEL draw_1637_model = {
#include "stg13/terrain/draw_1637_model.inc"
};
static NJS_CNK_OBJECT draw_1637 = {
#include "stg13/terrain/draw_1637.inc"
};
static Sint16 draw_1636_polygons[227] = {
#include "stg13/terrain/draw_1636_polygons.inc"
};
static Sint32 draw_1636_vertices[143] = {
#include "stg13/terrain/draw_1636_vertices.inc"
};
static NJS_CNK_MODEL draw_1636_model = {
#include "stg13/terrain/draw_1636_model.inc"
};
static NJS_CNK_OBJECT draw_1636 = {
#include "stg13/terrain/draw_1636.inc"
};
static Sint16 draw_1634_polygons[381] = {
#include "stg13/terrain/draw_1634_polygons.inc"
};
static Sint32 draw_1634_vertices[307] = {
#include "stg13/terrain/draw_1634_vertices.inc"
};
static NJS_CNK_MODEL draw_1634_model = {
#include "stg13/terrain/draw_1634_model.inc"
};
static NJS_CNK_OBJECT draw_1634 = {
#include "stg13/terrain/draw_1634.inc"
};
static Sint16 draw_1633_polygons[149] = {
#include "stg13/terrain/draw_1633_polygons.inc"
};
static Sint32 draw_1633_vertices[131] = {
#include "stg13/terrain/draw_1633_vertices.inc"
};
static NJS_CNK_MODEL draw_1633_model = {
#include "stg13/terrain/draw_1633_model.inc"
};
static NJS_CNK_OBJECT draw_1633 = {
#include "stg13/terrain/draw_1633.inc"
};
static Sint16 draw_1632_polygons[217] = {
#include "stg13/terrain/draw_1632_polygons.inc"
};
static Sint32 draw_1632_vertices[179] = {
#include "stg13/terrain/draw_1632_vertices.inc"
};
static NJS_CNK_MODEL draw_1632_model = {
#include "stg13/terrain/draw_1632_model.inc"
};
static NJS_CNK_OBJECT draw_1632 = {
#include "stg13/terrain/draw_1632.inc"
};
static Sint16 draw_1631_polygons[51] = {
#include "stg13/terrain/draw_1631_polygons.inc"
};
static Sint32 draw_1631_vertices[43] = {
#include "stg13/terrain/draw_1631_vertices.inc"
};
static NJS_CNK_MODEL draw_1631_model = {
#include "stg13/terrain/draw_1631_model.inc"
};
static NJS_CNK_OBJECT draw_1631 = {
#include "stg13/terrain/draw_1631.inc"
};
static Sint16 draw_656_polygons[55] = {
#include "stg13/terrain/draw_656_polygons.inc"
};
static Sint32 draw_656_vertices[43] = {
#include "stg13/terrain/draw_656_vertices.inc"
};
static NJS_CNK_MODEL draw_656_model = {
#include "stg13/terrain/draw_656_model.inc"
};
static NJS_CNK_OBJECT draw_1630 = {
#include "stg13/terrain/draw_1630.inc"
};
static Sint16 draw_1629_polygons[147] = {
#include "stg13/terrain/draw_1629_polygons.inc"
};
static Sint32 draw_1629_vertices[143] = {
#include "stg13/terrain/draw_1629_vertices.inc"
};
static NJS_CNK_MODEL draw_1629_model = {
#include "stg13/terrain/draw_1629_model.inc"
};
static NJS_CNK_OBJECT draw_1629 = {
#include "stg13/terrain/draw_1629.inc"
};
static Sint16 draw_1628_polygons[223] = {
#include "stg13/terrain/draw_1628_polygons.inc"
};
static Sint32 draw_1628_vertices[207] = {
#include "stg13/terrain/draw_1628_vertices.inc"
};
static NJS_CNK_MODEL draw_1628_model = {
#include "stg13/terrain/draw_1628_model.inc"
};
static NJS_CNK_OBJECT draw_1628 = {
#include "stg13/terrain/draw_1628.inc"
};
static Sint16 draw_1627_polygons[139] = {
#include "stg13/terrain/draw_1627_polygons.inc"
};
static Sint32 draw_1627_vertices[135] = {
#include "stg13/terrain/draw_1627_vertices.inc"
};
static NJS_CNK_MODEL draw_1627_model = {
#include "stg13/terrain/draw_1627_model.inc"
};
static NJS_CNK_OBJECT draw_1627 = {
#include "stg13/terrain/draw_1627.inc"
};
static Sint16 draw_1625_polygons[829] = {
#include "stg13/terrain/draw_1625_polygons.inc"
};
static Sint32 draw_1625_vertices[483] = {
#include "stg13/terrain/draw_1625_vertices.inc"
};
static NJS_CNK_MODEL draw_1625_model = {
#include "stg13/terrain/draw_1625_model.inc"
};
static NJS_CNK_OBJECT draw_1625 = {
#include "stg13/terrain/draw_1625.inc"
};
static Sint16 draw_646_polygons[1123] = {
#include "stg13/terrain/draw_646_polygons.inc"
};
static Sint32 draw_646_vertices[1115] = {
#include "stg13/terrain/draw_646_vertices.inc"
};
static NJS_CNK_MODEL draw_646_model = {
#include "stg13/terrain/draw_646_model.inc"
};
static NJS_CNK_OBJECT draw_1624 = {
#include "stg13/terrain/draw_1624.inc"
};
static Sint16 draw_1623_polygons[123] = {
#include "stg13/terrain/draw_1623_polygons.inc"
};
static Sint32 draw_1623_vertices[99] = {
#include "stg13/terrain/draw_1623_vertices.inc"
};
static NJS_CNK_MODEL draw_1623_model = {
#include "stg13/terrain/draw_1623_model.inc"
};
static NJS_CNK_OBJECT draw_1623 = {
#include "stg13/terrain/draw_1623.inc"
};
static Sint16 draw_1622_polygons[343] = {
#include "stg13/terrain/draw_1622_polygons.inc"
};
static Sint32 draw_1622_vertices[347] = {
#include "stg13/terrain/draw_1622_vertices.inc"
};
static NJS_CNK_MODEL draw_1622_model = {
#include "stg13/terrain/draw_1622_model.inc"
};
static NJS_CNK_OBJECT draw_1622 = {
#include "stg13/terrain/draw_1622.inc"
};
static Sint16 draw_642_polygons[229] = {
#include "stg13/terrain/draw_642_polygons.inc"
};
static Sint32 draw_642_vertices[211] = {
#include "stg13/terrain/draw_642_vertices.inc"
};
static NJS_CNK_MODEL draw_642_model = {
#include "stg13/terrain/draw_642_model.inc"
};
static NJS_CNK_OBJECT draw_1621 = {
#include "stg13/terrain/draw_1621.inc"
};
static Sint16 draw_1620_polygons[33] = {
#include "stg13/terrain/draw_1620_polygons.inc"
};
static Sint32 draw_1620_vertices[35] = {
#include "stg13/terrain/draw_1620_vertices.inc"
};
static NJS_CNK_MODEL draw_1620_model = {
#include "stg13/terrain/draw_1620_model.inc"
};
static NJS_CNK_OBJECT draw_1620 = {
#include "stg13/terrain/draw_1620.inc"
};
static Sint16 draw_644_polygons[159] = {
#include "stg13/terrain/draw_644_polygons.inc"
};
static Sint32 draw_644_vertices[143] = {
#include "stg13/terrain/draw_644_vertices.inc"
};
static NJS_CNK_MODEL draw_644_model = {
#include "stg13/terrain/draw_644_model.inc"
};
static NJS_CNK_OBJECT draw_1619 = {
#include "stg13/terrain/draw_1619.inc"
};
static Sint16 draw_1618_polygons[509] = {
#include "stg13/terrain/draw_1618_polygons.inc"
};
static Sint32 draw_1618_vertices[435] = {
#include "stg13/terrain/draw_1618_vertices.inc"
};
static NJS_CNK_MODEL draw_1618_model = {
#include "stg13/terrain/draw_1618_model.inc"
};
static NJS_CNK_OBJECT draw_1618 = {
#include "stg13/terrain/draw_1618.inc"
};
static Sint16 draw_1615_polygons[333] = {
#include "stg13/terrain/draw_1615_polygons.inc"
};
static Sint32 draw_1615_vertices[343] = {
#include "stg13/terrain/draw_1615_vertices.inc"
};
static NJS_CNK_MODEL draw_1615_model = {
#include "stg13/terrain/draw_1615_model.inc"
};
static NJS_CNK_OBJECT draw_1615 = {
#include "stg13/terrain/draw_1615.inc"
};
static Sint16 draw_903_polygons[341] = {
#include "stg13/terrain/draw_903_polygons.inc"
};
static Sint32 draw_903_vertices[307] = {
#include "stg13/terrain/draw_903_vertices.inc"
};
static NJS_CNK_MODEL draw_903_model = {
#include "stg13/terrain/draw_903_model.inc"
};
static NJS_CNK_OBJECT draw_1614 = {
#include "stg13/terrain/draw_1614.inc"
};
static Sint16 draw_805_polygons[337] = {
#include "stg13/terrain/draw_805_polygons.inc"
};
static Sint32 draw_805_vertices[279] = {
#include "stg13/terrain/draw_805_vertices.inc"
};
static NJS_CNK_MODEL draw_805_model = {
#include "stg13/terrain/draw_805_model.inc"
};
static NJS_CNK_OBJECT draw_1613 = {
#include "stg13/terrain/draw_1613.inc"
};
static Sint16 draw_69_polygons[155] = {
#include "stg13/terrain/draw_69_polygons.inc"
};
static Sint32 draw_69_vertices[155] = {
#include "stg13/terrain/draw_69_vertices.inc"
};
static NJS_CNK_MODEL draw_69_model = {
#include "stg13/terrain/draw_69_model.inc"
};
static NJS_CNK_OBJECT draw_1606 = {
#include "stg13/terrain/draw_1606.inc"
};
static Sint16 draw_740_polygons[673] = {
#include "stg13/terrain/draw_740_polygons.inc"
};
static Sint32 draw_740_vertices[603] = {
#include "stg13/terrain/draw_740_vertices.inc"
};
static NJS_CNK_MODEL draw_740_model = {
#include "stg13/terrain/draw_740_model.inc"
};
static NJS_CNK_OBJECT draw_1603 = {
#include "stg13/terrain/draw_1603.inc"
};
static Sint16 draw_67_polygons[301] = {
#include "stg13/terrain/draw_67_polygons.inc"
};
static Sint32 draw_67_vertices[251] = {
#include "stg13/terrain/draw_67_vertices.inc"
};
static NJS_CNK_MODEL draw_67_model = {
#include "stg13/terrain/draw_67_model.inc"
};
static NJS_CNK_OBJECT draw_1602 = {
#include "stg13/terrain/draw_1602.inc"
};
static Sint16 draw_66_polygons[65] = {
#include "stg13/terrain/draw_66_polygons.inc"
};
static Sint32 draw_66_vertices[51] = {
#include "stg13/terrain/draw_66_vertices.inc"
};
static NJS_CNK_MODEL draw_66_model = {
#include "stg13/terrain/draw_66_model.inc"
};
static NJS_CNK_OBJECT draw_1600 = {
#include "stg13/terrain/draw_1600.inc"
};
static Sint16 draw_732_polygons[533] = {
#include "stg13/terrain/draw_732_polygons.inc"
};
static Sint32 draw_732_vertices[643] = {
#include "stg13/terrain/draw_732_vertices.inc"
};
static NJS_CNK_MODEL draw_732_model = {
#include "stg13/terrain/draw_732_model.inc"
};
static NJS_CNK_OBJECT draw_1598 = {
#include "stg13/terrain/draw_1598.inc"
};
static Sint16 draw_1596_polygons[155] = {
#include "stg13/terrain/draw_1596_polygons.inc"
};
static Sint32 draw_1596_vertices[151] = {
#include "stg13/terrain/draw_1596_vertices.inc"
};
static NJS_CNK_MODEL draw_1596_model = {
#include "stg13/terrain/draw_1596_model.inc"
};
static NJS_CNK_OBJECT draw_1596 = {
#include "stg13/terrain/draw_1596.inc"
};
static Sint16 draw_1595_polygons[147] = {
#include "stg13/terrain/draw_1595_polygons.inc"
};
static Sint32 draw_1595_vertices[115] = {
#include "stg13/terrain/draw_1595_vertices.inc"
};
static NJS_CNK_MODEL draw_1595_model = {
#include "stg13/terrain/draw_1595_model.inc"
};
static NJS_CNK_OBJECT draw_1595 = {
#include "stg13/terrain/draw_1595.inc"
};
static Sint16 draw_719_polygons[153] = {
#include "stg13/terrain/draw_719_polygons.inc"
};
static Sint32 draw_719_vertices[123] = {
#include "stg13/terrain/draw_719_vertices.inc"
};
static NJS_CNK_MODEL draw_719_model = {
#include "stg13/terrain/draw_719_model.inc"
};
static NJS_CNK_OBJECT draw_1594 = {
#include "stg13/terrain/draw_1594.inc"
};
static Sint16 draw_713_polygons[75] = {
#include "stg13/terrain/draw_713_polygons.inc"
};
static Sint32 draw_713_vertices[83] = {
#include "stg13/terrain/draw_713_vertices.inc"
};
static NJS_CNK_MODEL draw_713_model = {
#include "stg13/terrain/draw_713_model.inc"
};
static NJS_CNK_OBJECT draw_1593 = {
#include "stg13/terrain/draw_1593.inc"
};
static Sint16 draw_707_polygons[75] = {
#include "stg13/terrain/draw_707_polygons.inc"
};
static Sint32 draw_707_vertices[83] = {
#include "stg13/terrain/draw_707_vertices.inc"
};
static NJS_CNK_MODEL draw_707_model = {
#include "stg13/terrain/draw_707_model.inc"
};
static NJS_CNK_OBJECT draw_1590 = {
#include "stg13/terrain/draw_1590.inc"
};
static Sint16 draw_1585_polygons[409] = {
#include "stg13/terrain/draw_1585_polygons.inc"
};
static Sint32 draw_1585_vertices[279] = {
#include "stg13/terrain/draw_1585_vertices.inc"
};
static NJS_CNK_MODEL draw_1585_model = {
#include "stg13/terrain/draw_1585_model.inc"
};
static NJS_CNK_OBJECT draw_1585 = {
#include "stg13/terrain/draw_1585.inc"
};
static Sint16 draw_119_polygons[323] = {
#include "stg13/terrain/draw_119_polygons.inc"
};
static Sint32 draw_119_vertices[243] = {
#include "stg13/terrain/draw_119_vertices.inc"
};
static NJS_CNK_MODEL draw_119_model = {
#include "stg13/terrain/draw_119_model.inc"
};
static NJS_CNK_OBJECT draw_1584 = {
#include "stg13/terrain/draw_1584.inc"
};
static Sint16 draw_828_polygons[455] = {
#include "stg13/terrain/draw_828_polygons.inc"
};
static Sint32 draw_828_vertices[375] = {
#include "stg13/terrain/draw_828_vertices.inc"
};
static NJS_CNK_MODEL draw_828_model = {
#include "stg13/terrain/draw_828_model.inc"
};
static NJS_CNK_OBJECT draw_1583 = {
#include "stg13/terrain/draw_1583.inc"
};
static Sint16 draw_1581_polygons[123] = {
#include "stg13/terrain/draw_1581_polygons.inc"
};
static Sint32 draw_1581_vertices[99] = {
#include "stg13/terrain/draw_1581_vertices.inc"
};
static NJS_CNK_MODEL draw_1581_model = {
#include "stg13/terrain/draw_1581_model.inc"
};
static NJS_CNK_OBJECT draw_1581 = {
#include "stg13/terrain/draw_1581.inc"
};
static Sint16 draw_1580_polygons[243] = {
#include "stg13/terrain/draw_1580_polygons.inc"
};
static Sint32 draw_1580_vertices[227] = {
#include "stg13/terrain/draw_1580_vertices.inc"
};
static NJS_CNK_MODEL draw_1580_model = {
#include "stg13/terrain/draw_1580_model.inc"
};
static NJS_CNK_OBJECT draw_1580 = {
#include "stg13/terrain/draw_1580.inc"
};
static Sint16 draw_814_polygons[533] = {
#include "stg13/terrain/draw_814_polygons.inc"
};
static Sint32 draw_814_vertices[643] = {
#include "stg13/terrain/draw_814_vertices.inc"
};
static NJS_CNK_MODEL draw_814_model = {
#include "stg13/terrain/draw_814_model.inc"
};
static NJS_CNK_OBJECT draw_1579 = {
#include "stg13/terrain/draw_1579.inc"
};
static Sint16 draw_1577_polygons[87] = {
#include "stg13/terrain/draw_1577_polygons.inc"
};
static Sint32 draw_1577_vertices[67] = {
#include "stg13/terrain/draw_1577_vertices.inc"
};
static NJS_CNK_MODEL draw_1577_model = {
#include "stg13/terrain/draw_1577_model.inc"
};
static NJS_CNK_OBJECT draw_1577 = {
#include "stg13/terrain/draw_1577.inc"
};
static Sint16 draw_1576_polygons[245] = {
#include "stg13/terrain/draw_1576_polygons.inc"
};
static Sint32 draw_1576_vertices[179] = {
#include "stg13/terrain/draw_1576_vertices.inc"
};
static NJS_CNK_MODEL draw_1576_model = {
#include "stg13/terrain/draw_1576_model.inc"
};
static NJS_CNK_OBJECT draw_1576 = {
#include "stg13/terrain/draw_1576.inc"
};
static Sint16 draw_811_polygons[95] = {
#include "stg13/terrain/draw_811_polygons.inc"
};
static Sint32 draw_811_vertices[107] = {
#include "stg13/terrain/draw_811_vertices.inc"
};
static NJS_CNK_MODEL draw_811_model = {
#include "stg13/terrain/draw_811_model.inc"
};
static NJS_CNK_OBJECT draw_1571 = {
#include "stg13/terrain/draw_1571.inc"
};
static Sint16 draw_1570_polygons[83] = {
#include "stg13/terrain/draw_1570_polygons.inc"
};
static Sint32 draw_1570_vertices[83] = {
#include "stg13/terrain/draw_1570_vertices.inc"
};
static NJS_CNK_MODEL draw_1570_model = {
#include "stg13/terrain/draw_1570_model.inc"
};
static NJS_CNK_OBJECT draw_1570 = {
#include "stg13/terrain/draw_1570.inc"
};
static Sint16 draw_128_polygons[197] = {
#include "stg13/terrain/draw_128_polygons.inc"
};
static Sint32 draw_128_vertices[127] = {
#include "stg13/terrain/draw_128_vertices.inc"
};
static NJS_CNK_MODEL draw_128_model = {
#include "stg13/terrain/draw_128_model.inc"
};
static NJS_CNK_OBJECT draw_1554 = {
#include "stg13/terrain/draw_1554.inc"
};
static Sint16 draw_125_polygons[197] = {
#include "stg13/terrain/draw_125_polygons.inc"
};
static Sint32 draw_125_vertices[127] = {
#include "stg13/terrain/draw_125_vertices.inc"
};
static NJS_CNK_MODEL draw_125_model = {
#include "stg13/terrain/draw_125_model.inc"
};
static NJS_CNK_OBJECT draw_1552 = {
#include "stg13/terrain/draw_1552.inc"
};
static Sint16 draw_647_polygons[685] = {
#include "stg13/terrain/draw_647_polygons.inc"
};
static Sint32 draw_647_vertices[607] = {
#include "stg13/terrain/draw_647_vertices.inc"
};
static NJS_CNK_MODEL draw_647_model = {
#include "stg13/terrain/draw_647_model.inc"
};
static NJS_CNK_OBJECT draw_1401 = {
#include "stg13/terrain/draw_1401.inc"
};
static Sint16 draw_648_polygons[105] = {
#include "stg13/terrain/draw_648_polygons.inc"
};
static Sint32 draw_648_vertices[83] = {
#include "stg13/terrain/draw_648_vertices.inc"
};
static NJS_CNK_MODEL draw_648_model = {
#include "stg13/terrain/draw_648_model.inc"
};
static NJS_CNK_OBJECT draw_1400 = {
#include "stg13/terrain/draw_1400.inc"
};
static Sint16 draw_1399_polygons[107] = {
#include "stg13/terrain/draw_1399_polygons.inc"
};
static Sint32 draw_1399_vertices[83] = {
#include "stg13/terrain/draw_1399_vertices.inc"
};
static NJS_CNK_MODEL draw_1399_model = {
#include "stg13/terrain/draw_1399_model.inc"
};
static NJS_CNK_OBJECT draw_1399 = {
#include "stg13/terrain/draw_1399.inc"
};
static Sint16 draw_819_polygons[55] = {
#include "stg13/terrain/draw_819_polygons.inc"
};
static Sint32 draw_819_vertices[43] = {
#include "stg13/terrain/draw_819_vertices.inc"
};
static NJS_CNK_MODEL draw_819_model = {
#include "stg13/terrain/draw_819_model.inc"
};
static NJS_CNK_OBJECT draw_1395 = {
#include "stg13/terrain/draw_1395.inc"
};
static Sint16 draw_643_polygons[349] = {
#include "stg13/terrain/draw_643_polygons.inc"
};
static Sint32 draw_643_vertices[311] = {
#include "stg13/terrain/draw_643_vertices.inc"
};
static NJS_CNK_MODEL draw_643_model = {
#include "stg13/terrain/draw_643_model.inc"
};
static NJS_CNK_OBJECT draw_1386 = {
#include "stg13/terrain/draw_1386.inc"
};
static Sint16 draw_96_polygons[229] = {
#include "stg13/terrain/draw_96_polygons.inc"
};
static Sint32 draw_96_vertices[155] = {
#include "stg13/terrain/draw_96_vertices.inc"
};
static NJS_CNK_MODEL draw_96_model = {
#include "stg13/terrain/draw_96_model.inc"
};
static NJS_CNK_OBJECT draw_1385 = {
#include "stg13/terrain/draw_1385.inc"
};
static Sint16 draw_1384_polygons[385] = {
#include "stg13/terrain/draw_1384_polygons.inc"
};
static Sint32 draw_1384_vertices[291] = {
#include "stg13/terrain/draw_1384_vertices.inc"
};
static NJS_CNK_MODEL draw_1384_model = {
#include "stg13/terrain/draw_1384_model.inc"
};
static NJS_CNK_OBJECT draw_1384 = {
#include "stg13/terrain/draw_1384.inc"
};
static Sint16 draw_1383_polygons[641] = {
#include "stg13/terrain/draw_1383_polygons.inc"
};
static Sint32 draw_1383_vertices[483] = {
#include "stg13/terrain/draw_1383_vertices.inc"
};
static NJS_CNK_MODEL draw_1383_model = {
#include "stg13/terrain/draw_1383_model.inc"
};
static NJS_CNK_OBJECT draw_1383 = {
#include "stg13/terrain/draw_1383.inc"
};
static Sint16 draw_139_polygons[121] = {
#include "stg13/terrain/draw_139_polygons.inc"
};
static Sint32 draw_139_vertices[147] = {
#include "stg13/terrain/draw_139_vertices.inc"
};
static NJS_CNK_MODEL draw_139_model = {
#include "stg13/terrain/draw_139_model.inc"
};
static NJS_CNK_OBJECT draw_1340 = {
#include "stg13/terrain/draw_1340.inc"
};
static Sint16 draw_196_polygons[97] = {
#include "stg13/terrain/draw_196_polygons.inc"
};
static Sint32 draw_196_vertices[115] = {
#include "stg13/terrain/draw_196_vertices.inc"
};
static NJS_CNK_MODEL draw_196_model = {
#include "stg13/terrain/draw_196_model.inc"
};
static NJS_CNK_OBJECT draw_1318 = {
#include "stg13/terrain/draw_1318.inc"
};
static Sint16 draw_222_polygons[159] = {
#include "stg13/terrain/draw_222_polygons.inc"
};
static Sint32 draw_222_vertices[179] = {
#include "stg13/terrain/draw_222_vertices.inc"
};
static NJS_CNK_MODEL draw_222_model = {
#include "stg13/terrain/draw_222_model.inc"
};
static NJS_CNK_OBJECT draw_1255 = {
#include "stg13/terrain/draw_1255.inc"
};
static Sint16 draw_1247_polygons[41] = {
#include "stg13/terrain/draw_1247_polygons.inc"
};
static Sint32 draw_1247_vertices[43] = {
#include "stg13/terrain/draw_1247_vertices.inc"
};
static NJS_CNK_MODEL draw_1247_model = {
#include "stg13/terrain/draw_1247_model.inc"
};
static NJS_CNK_OBJECT draw_1248 = {
#include "stg13/terrain/draw_1248.inc"
};
static Sint16 draw_1140_polygons[177] = {
#include "stg13/terrain/draw_1140_polygons.inc"
};
static Sint32 draw_1140_vertices[163] = {
#include "stg13/terrain/draw_1140_vertices.inc"
};
static NJS_CNK_MODEL draw_1140_model = {
#include "stg13/terrain/draw_1140_model.inc"
};
static NJS_CNK_OBJECT draw_1241 = {
#include "stg13/terrain/draw_1241.inc"
};
static Sint16 draw_1139_polygons[177] = {
#include "stg13/terrain/draw_1139_polygons.inc"
};
static Sint32 draw_1139_vertices[163] = {
#include "stg13/terrain/draw_1139_vertices.inc"
};
static NJS_CNK_MODEL draw_1139_model = {
#include "stg13/terrain/draw_1139_model.inc"
};
static NJS_CNK_OBJECT draw_1238 = {
#include "stg13/terrain/draw_1238.inc"
};
static Sint16 draw_1223_polygons[137] = {
#include "stg13/terrain/draw_1223_polygons.inc"
};
static Sint32 draw_1223_vertices[99] = {
#include "stg13/terrain/draw_1223_vertices.inc"
};
static NJS_CNK_MODEL draw_1223_model = {
#include "stg13/terrain/draw_1223_model.inc"
};
static NJS_CNK_OBJECT draw_1223 = {
#include "stg13/terrain/draw_1223.inc"
};
static Sint16 draw_1222_polygons[137] = {
#include "stg13/terrain/draw_1222_polygons.inc"
};
static Sint32 draw_1222_vertices[99] = {
#include "stg13/terrain/draw_1222_vertices.inc"
};
static NJS_CNK_MODEL draw_1222_model = {
#include "stg13/terrain/draw_1222_model.inc"
};
static NJS_CNK_OBJECT draw_1222 = {
#include "stg13/terrain/draw_1222.inc"
};
static Sint16 draw_1216_polygons[145] = {
#include "stg13/terrain/draw_1216_polygons.inc"
};
static Sint32 draw_1216_vertices[163] = {
#include "stg13/terrain/draw_1216_vertices.inc"
};
static NJS_CNK_MODEL draw_1216_model = {
#include "stg13/terrain/draw_1216_model.inc"
};
static NJS_CNK_OBJECT draw_1216 = {
#include "stg13/terrain/draw_1216.inc"
};
static Sint16 draw_1141_polygons[227] = {
#include "stg13/terrain/draw_1141_polygons.inc"
};
static Sint32 draw_1141_vertices[175] = {
#include "stg13/terrain/draw_1141_vertices.inc"
};
static NJS_CNK_MODEL draw_1141_model = {
#include "stg13/terrain/draw_1141_model.inc"
};
static NJS_CNK_OBJECT draw_1207 = {
#include "stg13/terrain/draw_1207.inc"
};
static Sint16 draw_1145_polygons[29] = {
#include "stg13/terrain/draw_1145_polygons.inc"
};
static Sint32 draw_1145_vertices[27] = {
#include "stg13/terrain/draw_1145_vertices.inc"
};
static NJS_CNK_MODEL draw_1145_model = {
#include "stg13/terrain/draw_1145_model.inc"
};
static NJS_CNK_OBJECT draw_1206 = {
#include "stg13/terrain/draw_1206.inc"
};
static Sint16 draw_1168_polygons[55] = {
#include "stg13/terrain/draw_1168_polygons.inc"
};
static Sint32 draw_1168_vertices[43] = {
#include "stg13/terrain/draw_1168_vertices.inc"
};
static NJS_CNK_MODEL draw_1168_model = {
#include "stg13/terrain/draw_1168_model.inc"
};
static NJS_CNK_OBJECT draw_1205 = {
#include "stg13/terrain/draw_1205.inc"
};
static Sint16 draw_1201_polygons[85] = {
#include "stg13/terrain/draw_1201_polygons.inc"
};
static Sint32 draw_1201_vertices[67] = {
#include "stg13/terrain/draw_1201_vertices.inc"
};
static NJS_CNK_MODEL draw_1201_model = {
#include "stg13/terrain/draw_1201_model.inc"
};
static NJS_CNK_OBJECT draw_1204 = {
#include "stg13/terrain/draw_1204.inc"
};
static Sint16 draw_1200_polygons[185] = {
#include "stg13/terrain/draw_1200_polygons.inc"
};
static Sint32 draw_1200_vertices[139] = {
#include "stg13/terrain/draw_1200_vertices.inc"
};
static NJS_CNK_MODEL draw_1200_model = {
#include "stg13/terrain/draw_1200_model.inc"
};
static NJS_CNK_OBJECT draw_1200 = {
#include "stg13/terrain/draw_1200.inc"
};
static Sint16 draw_1196_polygons[85] = {
#include "stg13/terrain/draw_1196_polygons.inc"
};
static Sint32 draw_1196_vertices[67] = {
#include "stg13/terrain/draw_1196_vertices.inc"
};
static NJS_CNK_MODEL draw_1196_model = {
#include "stg13/terrain/draw_1196_model.inc"
};
static NJS_CNK_OBJECT draw_1199 = {
#include "stg13/terrain/draw_1199.inc"
};
static Sint16 draw_1138_polygons[209] = {
#include "stg13/terrain/draw_1138_polygons.inc"
};
static Sint32 draw_1138_vertices[147] = {
#include "stg13/terrain/draw_1138_vertices.inc"
};
static NJS_CNK_MODEL draw_1138_model = {
#include "stg13/terrain/draw_1138_model.inc"
};
static NJS_CNK_OBJECT draw_1194 = {
#include "stg13/terrain/draw_1194.inc"
};
static Sint16 draw_1137_polygons[209] = {
#include "stg13/terrain/draw_1137_polygons.inc"
};
static Sint32 draw_1137_vertices[147] = {
#include "stg13/terrain/draw_1137_vertices.inc"
};
static NJS_CNK_MODEL draw_1137_model = {
#include "stg13/terrain/draw_1137_model.inc"
};
static NJS_CNK_OBJECT draw_1193 = {
#include "stg13/terrain/draw_1193.inc"
};
static Sint16 draw_1147_polygons[149] = {
#include "stg13/terrain/draw_1147_polygons.inc"
};
static Sint32 draw_1147_vertices[115] = {
#include "stg13/terrain/draw_1147_vertices.inc"
};
static NJS_CNK_MODEL draw_1147_model = {
#include "stg13/terrain/draw_1147_model.inc"
};
static NJS_CNK_OBJECT draw_1192 = {
#include "stg13/terrain/draw_1192.inc"
};
static Sint16 draw_1154_polygons[149] = {
#include "stg13/terrain/draw_1154_polygons.inc"
};
static Sint32 draw_1154_vertices[115] = {
#include "stg13/terrain/draw_1154_vertices.inc"
};
static NJS_CNK_MODEL draw_1154_model = {
#include "stg13/terrain/draw_1154_model.inc"
};
static NJS_CNK_OBJECT draw_1191 = {
#include "stg13/terrain/draw_1191.inc"
};
static Sint16 draw_1149_polygons[227] = {
#include "stg13/terrain/draw_1149_polygons.inc"
};
static Sint32 draw_1149_vertices[175] = {
#include "stg13/terrain/draw_1149_vertices.inc"
};
static NJS_CNK_MODEL draw_1149_model = {
#include "stg13/terrain/draw_1149_model.inc"
};
static NJS_CNK_OBJECT draw_1190 = {
#include "stg13/terrain/draw_1190.inc"
};
static Sint16 draw_1170_polygons[409] = {
#include "stg13/terrain/draw_1170_polygons.inc"
};
static Sint32 draw_1170_vertices[367] = {
#include "stg13/terrain/draw_1170_vertices.inc"
};
static NJS_CNK_MODEL draw_1170_model = {
#include "stg13/terrain/draw_1170_model.inc"
};
static NJS_CNK_OBJECT draw_1179 = {
#include "stg13/terrain/draw_1179.inc"
};
static Sint16 draw_1171_polygons[233] = {
#include "stg13/terrain/draw_1171_polygons.inc"
};
static Sint32 draw_1171_vertices[235] = {
#include "stg13/terrain/draw_1171_vertices.inc"
};
static NJS_CNK_MODEL draw_1171_model = {
#include "stg13/terrain/draw_1171_model.inc"
};
static NJS_CNK_OBJECT draw_1171 = {
#include "stg13/terrain/draw_1171.inc"
};
static Sint16 draw_1169_polygons[337] = {
#include "stg13/terrain/draw_1169_polygons.inc"
};
static Sint32 draw_1169_vertices[207] = {
#include "stg13/terrain/draw_1169_vertices.inc"
};
static NJS_CNK_MODEL draw_1169_model = {
#include "stg13/terrain/draw_1169_model.inc"
};
static NJS_CNK_OBJECT draw_1169 = {
#include "stg13/terrain/draw_1169.inc"
};
static Sint16 draw_1167_polygons[191] = {
#include "stg13/terrain/draw_1167_polygons.inc"
};
static Sint32 draw_1167_vertices[151] = {
#include "stg13/terrain/draw_1167_vertices.inc"
};
static NJS_CNK_MODEL draw_1167_model = {
#include "stg13/terrain/draw_1167_model.inc"
};
static NJS_CNK_OBJECT draw_1167 = {
#include "stg13/terrain/draw_1167.inc"
};
static Sint16 draw_1165_polygons[55] = {
#include "stg13/terrain/draw_1165_polygons.inc"
};
static Sint32 draw_1165_vertices[43] = {
#include "stg13/terrain/draw_1165_vertices.inc"
};
static NJS_CNK_MODEL draw_1165_model = {
#include "stg13/terrain/draw_1165_model.inc"
};
static NJS_CNK_OBJECT draw_1166 = {
#include "stg13/terrain/draw_1166.inc"
};
static Sint16 draw_1159_polygons[185] = {
#include "stg13/terrain/draw_1159_polygons.inc"
};
static Sint32 draw_1159_vertices[139] = {
#include "stg13/terrain/draw_1159_vertices.inc"
};
static NJS_CNK_MODEL draw_1159_model = {
#include "stg13/terrain/draw_1159_model.inc"
};
static NJS_CNK_OBJECT draw_1159 = {
#include "stg13/terrain/draw_1159.inc"
};
static Sint16 draw_215_polygons[113] = {
#include "stg13/terrain/draw_215_polygons.inc"
};
static Sint32 draw_215_vertices[99] = {
#include "stg13/terrain/draw_215_vertices.inc"
};
static NJS_CNK_MODEL draw_215_model = {
#include "stg13/terrain/draw_215_model.inc"
};
static NJS_CNK_OBJECT draw_1048 = {
#include "stg13/terrain/draw_1048.inc"
};
static Sint16 draw_1016_polygons[15] = {
#include "stg13/terrain/draw_1016_polygons.inc"
};
static Sint32 draw_1016_vertices[19] = {
#include "stg13/terrain/draw_1016_vertices.inc"
};
static NJS_CNK_MODEL draw_1016_model = {
#include "stg13/terrain/draw_1016_model.inc"
};
static NJS_CNK_OBJECT draw_1016 = {
#include "stg13/terrain/draw_1016.inc"
};
static Sint16 draw_91_polygons[503] = {
#include "stg13/terrain/draw_91_polygons.inc"
};
static Sint32 draw_91_vertices[443] = {
#include "stg13/terrain/draw_91_vertices.inc"
};
static NJS_CNK_MODEL draw_91_model = {
#include "stg13/terrain/draw_91_model.inc"
};
static NJS_CNK_OBJECT draw_1015 = {
#include "stg13/terrain/draw_1015.inc"
};
static Sint16 draw_730_polygons[437] = {
#include "stg13/terrain/draw_730_polygons.inc"
};
static Sint32 draw_730_vertices[515] = {
#include "stg13/terrain/draw_730_vertices.inc"
};
static NJS_CNK_MODEL draw_730_model = {
#include "stg13/terrain/draw_730_model.inc"
};
static NJS_CNK_OBJECT draw_1003 = {
#include "stg13/terrain/draw_1003.inc"
};
static Sint16 draw_731_polygons[865] = {
#include "stg13/terrain/draw_731_polygons.inc"
};
static Sint32 draw_731_vertices[831] = {
#include "stg13/terrain/draw_731_vertices.inc"
};
static NJS_CNK_MODEL draw_731_model = {
#include "stg13/terrain/draw_731_model.inc"
};
static NJS_CNK_OBJECT draw_982 = {
#include "stg13/terrain/draw_982.inc"
};
static Sint16 draw_728_polygons[169] = {
#include "stg13/terrain/draw_728_polygons.inc"
};
static Sint32 draw_728_vertices[151] = {
#include "stg13/terrain/draw_728_vertices.inc"
};
static NJS_CNK_MODEL draw_728_model = {
#include "stg13/terrain/draw_728_model.inc"
};
static NJS_CNK_OBJECT draw_978 = {
#include "stg13/terrain/draw_978.inc"
};
static Sint16 draw_726_polygons[409] = {
#include "stg13/terrain/draw_726_polygons.inc"
};
static Sint32 draw_726_vertices[415] = {
#include "stg13/terrain/draw_726_vertices.inc"
};
static NJS_CNK_MODEL draw_726_model = {
#include "stg13/terrain/draw_726_model.inc"
};
static NJS_CNK_OBJECT draw_974 = {
#include "stg13/terrain/draw_974.inc"
};
static Sint16 draw_104_polygons[423] = {
#include "stg13/terrain/draw_104_polygons.inc"
};
static Sint32 draw_104_vertices[335] = {
#include "stg13/terrain/draw_104_vertices.inc"
};
static NJS_CNK_MODEL draw_104_model = {
#include "stg13/terrain/draw_104_model.inc"
};
static NJS_CNK_OBJECT draw_970 = {
#include "stg13/terrain/draw_970.inc"
};
static Sint16 draw_904_polygons[533] = {
#include "stg13/terrain/draw_904_polygons.inc"
};
static Sint32 draw_904_vertices[487] = {
#include "stg13/terrain/draw_904_vertices.inc"
};
static NJS_CNK_MODEL draw_904_model = {
#include "stg13/terrain/draw_904_model.inc"
};
static NJS_CNK_OBJECT draw_915 = {
#include "stg13/terrain/draw_915.inc"
};
static Sint16 draw_914_polygons[785] = {
#include "stg13/terrain/draw_914_polygons.inc"
};
static Sint32 draw_914_vertices[595] = {
#include "stg13/terrain/draw_914_vertices.inc"
};
static NJS_CNK_MODEL draw_914_model = {
#include "stg13/terrain/draw_914_model.inc"
};
static NJS_CNK_OBJECT draw_914 = {
#include "stg13/terrain/draw_914.inc"
};
static Sint16 draw_913_polygons[789] = {
#include "stg13/terrain/draw_913_polygons.inc"
};
static Sint32 draw_913_vertices[711] = {
#include "stg13/terrain/draw_913_vertices.inc"
};
static NJS_CNK_MODEL draw_913_model = {
#include "stg13/terrain/draw_913_model.inc"
};
static NJS_CNK_OBJECT draw_913 = {
#include "stg13/terrain/draw_913.inc"
};
static Sint16 draw_912_polygons[449] = {
#include "stg13/terrain/draw_912_polygons.inc"
};
static Sint32 draw_912_vertices[391] = {
#include "stg13/terrain/draw_912_vertices.inc"
};
static NJS_CNK_MODEL draw_912_model = {
#include "stg13/terrain/draw_912_model.inc"
};
static NJS_CNK_OBJECT draw_912 = {
#include "stg13/terrain/draw_912.inc"
};
static Sint16 draw_909_polygons[485] = {
#include "stg13/terrain/draw_909_polygons.inc"
};
static Sint32 draw_909_vertices[419] = {
#include "stg13/terrain/draw_909_vertices.inc"
};
static NJS_CNK_MODEL draw_909_model = {
#include "stg13/terrain/draw_909_model.inc"
};
static NJS_CNK_OBJECT draw_909 = {
#include "stg13/terrain/draw_909.inc"
};
static Sint16 draw_907_polygons[171] = {
#include "stg13/terrain/draw_907_polygons.inc"
};
static Sint32 draw_907_vertices[131] = {
#include "stg13/terrain/draw_907_vertices.inc"
};
static NJS_CNK_MODEL draw_907_model = {
#include "stg13/terrain/draw_907_model.inc"
};
static NJS_CNK_OBJECT draw_907 = {
#include "stg13/terrain/draw_907.inc"
};
static Sint16 draw_906_polygons[95] = {
#include "stg13/terrain/draw_906_polygons.inc"
};
static Sint32 draw_906_vertices[75] = {
#include "stg13/terrain/draw_906_vertices.inc"
};
static NJS_CNK_MODEL draw_906_model = {
#include "stg13/terrain/draw_906_model.inc"
};
static NJS_CNK_OBJECT draw_906 = {
#include "stg13/terrain/draw_906.inc"
};
static Sint16 draw_899_polygons[639] = {
#include "stg13/terrain/draw_899_polygons.inc"
};
static Sint32 draw_899_vertices[503] = {
#include "stg13/terrain/draw_899_vertices.inc"
};
static NJS_CNK_MODEL draw_899_model = {
#include "stg13/terrain/draw_899_model.inc"
};
static NJS_CNK_OBJECT draw_899 = {
#include "stg13/terrain/draw_899.inc"
};
static Sint16 draw_749_polygons[837] = {
#include "stg13/terrain/draw_749_polygons.inc"
};
static Sint32 draw_749_vertices[819] = {
#include "stg13/terrain/draw_749_vertices.inc"
};
static NJS_CNK_MODEL draw_749_model = {
#include "stg13/terrain/draw_749_model.inc"
};
static NJS_CNK_OBJECT draw_898 = {
#include "stg13/terrain/draw_898.inc"
};
static Sint16 draw_652_polygons[197] = {
#include "stg13/terrain/draw_652_polygons.inc"
};
static Sint32 draw_652_vertices[199] = {
#include "stg13/terrain/draw_652_vertices.inc"
};
static NJS_CNK_MODEL draw_652_model = {
#include "stg13/terrain/draw_652_model.inc"
};
static NJS_CNK_OBJECT draw_897 = {
#include "stg13/terrain/draw_897.inc"
};
static Sint16 draw_724_polygons[401] = {
#include "stg13/terrain/draw_724_polygons.inc"
};
static Sint32 draw_724_vertices[411] = {
#include "stg13/terrain/draw_724_vertices.inc"
};
static NJS_CNK_MODEL draw_724_model = {
#include "stg13/terrain/draw_724_model.inc"
};
static NJS_CNK_OBJECT draw_896 = {
#include "stg13/terrain/draw_896.inc"
};
static Sint16 draw_895_polygons[695] = {
#include "stg13/terrain/draw_895_polygons.inc"
};
static Sint32 draw_895_vertices[535] = {
#include "stg13/terrain/draw_895_vertices.inc"
};
static NJS_CNK_MODEL draw_895_model = {
#include "stg13/terrain/draw_895_model.inc"
};
static NJS_CNK_OBJECT draw_895 = {
#include "stg13/terrain/draw_895.inc"
};
static Sint16 draw_722_polygons[445] = {
#include "stg13/terrain/draw_722_polygons.inc"
};
static Sint32 draw_722_vertices[519] = {
#include "stg13/terrain/draw_722_vertices.inc"
};
static NJS_CNK_MODEL draw_722_model = {
#include "stg13/terrain/draw_722_model.inc"
};
static NJS_CNK_OBJECT draw_894 = {
#include "stg13/terrain/draw_894.inc"
};
static Sint16 draw_720_polygons[177] = {
#include "stg13/terrain/draw_720_polygons.inc"
};
static Sint32 draw_720_vertices[151] = {
#include "stg13/terrain/draw_720_vertices.inc"
};
static NJS_CNK_MODEL draw_720_model = {
#include "stg13/terrain/draw_720_model.inc"
};
static NJS_CNK_OBJECT draw_892 = {
#include "stg13/terrain/draw_892.inc"
};
static Sint16 draw_651_polygons[267] = {
#include "stg13/terrain/draw_651_polygons.inc"
};
static Sint32 draw_651_vertices[291] = {
#include "stg13/terrain/draw_651_vertices.inc"
};
static NJS_CNK_MODEL draw_651_model = {
#include "stg13/terrain/draw_651_model.inc"
};
static NJS_CNK_OBJECT draw_890 = {
#include "stg13/terrain/draw_890.inc"
};
static Sint16 draw_836_polygons[257] = {
#include "stg13/terrain/draw_836_polygons.inc"
};
static Sint32 draw_836_vertices[171] = {
#include "stg13/terrain/draw_836_vertices.inc"
};
static NJS_CNK_MODEL draw_836_model = {
#include "stg13/terrain/draw_836_model.inc"
};
static NJS_CNK_OBJECT draw_889 = {
#include "stg13/terrain/draw_889.inc"
};
static Sint16 draw_888_polygons[171] = {
#include "stg13/terrain/draw_888_polygons.inc"
};
static Sint32 draw_888_vertices[119] = {
#include "stg13/terrain/draw_888_vertices.inc"
};
static NJS_CNK_MODEL draw_888_model = {
#include "stg13/terrain/draw_888_model.inc"
};
static NJS_CNK_OBJECT draw_888 = {
#include "stg13/terrain/draw_888.inc"
};
static Sint16 draw_887_polygons[149] = {
#include "stg13/terrain/draw_887_polygons.inc"
};
static Sint32 draw_887_vertices[131] = {
#include "stg13/terrain/draw_887_vertices.inc"
};
static NJS_CNK_MODEL draw_887_model = {
#include "stg13/terrain/draw_887_model.inc"
};
static NJS_CNK_OBJECT draw_887 = {
#include "stg13/terrain/draw_887.inc"
};
static Sint16 draw_886_polygons[353] = {
#include "stg13/terrain/draw_886_polygons.inc"
};
static Sint32 draw_886_vertices[247] = {
#include "stg13/terrain/draw_886_vertices.inc"
};
static NJS_CNK_MODEL draw_886_model = {
#include "stg13/terrain/draw_886_model.inc"
};
static NJS_CNK_OBJECT draw_886 = {
#include "stg13/terrain/draw_886.inc"
};
static Sint16 draw_884_polygons[249] = {
#include "stg13/terrain/draw_884_polygons.inc"
};
static Sint32 draw_884_vertices[315] = {
#include "stg13/terrain/draw_884_vertices.inc"
};
static NJS_CNK_MODEL draw_884_model = {
#include "stg13/terrain/draw_884_model.inc"
};
static NJS_CNK_OBJECT draw_884 = {
#include "stg13/terrain/draw_884.inc"
};
static Sint16 draw_883_polygons[249] = {
#include "stg13/terrain/draw_883_polygons.inc"
};
static Sint32 draw_883_vertices[287] = {
#include "stg13/terrain/draw_883_vertices.inc"
};
static NJS_CNK_MODEL draw_883_model = {
#include "stg13/terrain/draw_883_model.inc"
};
static NJS_CNK_OBJECT draw_883 = {
#include "stg13/terrain/draw_883.inc"
};
static Sint16 draw_882_polygons[65] = {
#include "stg13/terrain/draw_882_polygons.inc"
};
static Sint32 draw_882_vertices[67] = {
#include "stg13/terrain/draw_882_vertices.inc"
};
static NJS_CNK_MODEL draw_882_model = {
#include "stg13/terrain/draw_882_model.inc"
};
static NJS_CNK_OBJECT draw_882 = {
#include "stg13/terrain/draw_882.inc"
};
static Sint16 draw_881_polygons[249] = {
#include "stg13/terrain/draw_881_polygons.inc"
};
static Sint32 draw_881_vertices[311] = {
#include "stg13/terrain/draw_881_vertices.inc"
};
static NJS_CNK_MODEL draw_881_model = {
#include "stg13/terrain/draw_881_model.inc"
};
static NJS_CNK_OBJECT draw_881 = {
#include "stg13/terrain/draw_881.inc"
};
static Sint16 draw_880_polygons[1011] = {
#include "stg13/terrain/draw_880_polygons.inc"
};
static Sint32 draw_880_vertices[711] = {
#include "stg13/terrain/draw_880_vertices.inc"
};
static NJS_CNK_MODEL draw_880_model = {
#include "stg13/terrain/draw_880_model.inc"
};
static NJS_CNK_OBJECT draw_880 = {
#include "stg13/terrain/draw_880.inc"
};
static Sint16 draw_879_polygons[407] = {
#include "stg13/terrain/draw_879_polygons.inc"
};
static Sint32 draw_879_vertices[443] = {
#include "stg13/terrain/draw_879_vertices.inc"
};
static NJS_CNK_MODEL draw_879_model = {
#include "stg13/terrain/draw_879_model.inc"
};
static NJS_CNK_OBJECT draw_879 = {
#include "stg13/terrain/draw_879.inc"
};
static Sint16 draw_877_polygons[101] = {
#include "stg13/terrain/draw_877_polygons.inc"
};
static Sint32 draw_877_vertices[87] = {
#include "stg13/terrain/draw_877_vertices.inc"
};
static NJS_CNK_MODEL draw_877_model = {
#include "stg13/terrain/draw_877_model.inc"
};
static NJS_CNK_OBJECT draw_877 = {
#include "stg13/terrain/draw_877.inc"
};
static Sint16 draw_876_polygons[331] = {
#include "stg13/terrain/draw_876_polygons.inc"
};
static Sint32 draw_876_vertices[323] = {
#include "stg13/terrain/draw_876_vertices.inc"
};
static NJS_CNK_MODEL draw_876_model = {
#include "stg13/terrain/draw_876_model.inc"
};
static NJS_CNK_OBJECT draw_876 = {
#include "stg13/terrain/draw_876.inc"
};
static Sint16 draw_869_polygons[69] = {
#include "stg13/terrain/draw_869_polygons.inc"
};
static Sint32 draw_869_vertices[59] = {
#include "stg13/terrain/draw_869_vertices.inc"
};
static NJS_CNK_MODEL draw_869_model = {
#include "stg13/terrain/draw_869_model.inc"
};
static NJS_CNK_OBJECT draw_875 = {
#include "stg13/terrain/draw_875.inc"
};
static Sint16 draw_864_polygons[69] = {
#include "stg13/terrain/draw_864_polygons.inc"
};
static Sint32 draw_864_vertices[67] = {
#include "stg13/terrain/draw_864_vertices.inc"
};
static NJS_CNK_MODEL draw_864_model = {
#include "stg13/terrain/draw_864_model.inc"
};
static NJS_CNK_OBJECT draw_868 = {
#include "stg13/terrain/draw_868.inc"
};
static Sint16 draw_866_polygons[211] = {
#include "stg13/terrain/draw_866_polygons.inc"
};
static Sint32 draw_866_vertices[199] = {
#include "stg13/terrain/draw_866_vertices.inc"
};
static NJS_CNK_MODEL draw_866_model = {
#include "stg13/terrain/draw_866_model.inc"
};
static NJS_CNK_OBJECT draw_866 = {
#include "stg13/terrain/draw_866.inc"
};
static Sint16 draw_863_polygons[435] = {
#include "stg13/terrain/draw_863_polygons.inc"
};
static Sint32 draw_863_vertices[447] = {
#include "stg13/terrain/draw_863_vertices.inc"
};
static NJS_CNK_MODEL draw_863_model = {
#include "stg13/terrain/draw_863_model.inc"
};
static NJS_CNK_OBJECT draw_863 = {
#include "stg13/terrain/draw_863.inc"
};
static Sint16 draw_809_polygons[73] = {
#include "stg13/terrain/draw_809_polygons.inc"
};
static Sint32 draw_809_vertices[51] = {
#include "stg13/terrain/draw_809_vertices.inc"
};
static NJS_CNK_MODEL draw_809_model = {
#include "stg13/terrain/draw_809_model.inc"
};
static NJS_CNK_OBJECT draw_862 = {
#include "stg13/terrain/draw_862.inc"
};
static Sint16 draw_860_polygons[641] = {
#include "stg13/terrain/draw_860_polygons.inc"
};
static Sint32 draw_860_vertices[623] = {
#include "stg13/terrain/draw_860_vertices.inc"
};
static NJS_CNK_MODEL draw_860_model = {
#include "stg13/terrain/draw_860_model.inc"
};
static NJS_CNK_OBJECT draw_860 = {
#include "stg13/terrain/draw_860.inc"
};
static Sint16 draw_859_polygons[709] = {
#include "stg13/terrain/draw_859_polygons.inc"
};
static Sint32 draw_859_vertices[671] = {
#include "stg13/terrain/draw_859_vertices.inc"
};
static NJS_CNK_MODEL draw_859_model = {
#include "stg13/terrain/draw_859_model.inc"
};
static NJS_CNK_OBJECT draw_859 = {
#include "stg13/terrain/draw_859.inc"
};
static Sint16 draw_858_polygons[37] = {
#include "stg13/terrain/draw_858_polygons.inc"
};
static Sint32 draw_858_vertices[43] = {
#include "stg13/terrain/draw_858_vertices.inc"
};
static NJS_CNK_MODEL draw_858_model = {
#include "stg13/terrain/draw_858_model.inc"
};
static NJS_CNK_OBJECT draw_858 = {
#include "stg13/terrain/draw_858.inc"
};
static Sint16 draw_857_polygons[249] = {
#include "stg13/terrain/draw_857_polygons.inc"
};
static Sint32 draw_857_vertices[231] = {
#include "stg13/terrain/draw_857_vertices.inc"
};
static NJS_CNK_MODEL draw_857_model = {
#include "stg13/terrain/draw_857_model.inc"
};
static NJS_CNK_OBJECT draw_857 = {
#include "stg13/terrain/draw_857.inc"
};
static Sint16 draw_856_polygons[73] = {
#include "stg13/terrain/draw_856_polygons.inc"
};
static Sint32 draw_856_vertices[91] = {
#include "stg13/terrain/draw_856_vertices.inc"
};
static NJS_CNK_MODEL draw_856_model = {
#include "stg13/terrain/draw_856_model.inc"
};
static NJS_CNK_OBJECT draw_856 = {
#include "stg13/terrain/draw_856.inc"
};
static Sint16 draw_855_polygons[73] = {
#include "stg13/terrain/draw_855_polygons.inc"
};
static Sint32 draw_855_vertices[91] = {
#include "stg13/terrain/draw_855_vertices.inc"
};
static NJS_CNK_MODEL draw_855_model = {
#include "stg13/terrain/draw_855_model.inc"
};
static NJS_CNK_OBJECT draw_855 = {
#include "stg13/terrain/draw_855.inc"
};
static Sint16 draw_854_polygons[87] = {
#include "stg13/terrain/draw_854_polygons.inc"
};
static Sint32 draw_854_vertices[99] = {
#include "stg13/terrain/draw_854_vertices.inc"
};
static NJS_CNK_MODEL draw_854_model = {
#include "stg13/terrain/draw_854_model.inc"
};
static NJS_CNK_OBJECT draw_854 = {
#include "stg13/terrain/draw_854.inc"
};
static Sint16 draw_851_polygons[619] = {
#include "stg13/terrain/draw_851_polygons.inc"
};
static Sint32 draw_851_vertices[427] = {
#include "stg13/terrain/draw_851_vertices.inc"
};
static NJS_CNK_MODEL draw_851_model = {
#include "stg13/terrain/draw_851_model.inc"
};
static NJS_CNK_OBJECT draw_851 = {
#include "stg13/terrain/draw_851.inc"
};
static Sint16 draw_838_polygons[201] = {
#include "stg13/terrain/draw_838_polygons.inc"
};
static Sint32 draw_838_vertices[167] = {
#include "stg13/terrain/draw_838_vertices.inc"
};
static NJS_CNK_MODEL draw_838_model = {
#include "stg13/terrain/draw_838_model.inc"
};
static NJS_CNK_OBJECT draw_838 = {
#include "stg13/terrain/draw_838.inc"
};
static Sint16 draw_835_polygons[269] = {
#include "stg13/terrain/draw_835_polygons.inc"
};
static Sint32 draw_835_vertices[171] = {
#include "stg13/terrain/draw_835_vertices.inc"
};
static NJS_CNK_MODEL draw_835_model = {
#include "stg13/terrain/draw_835_model.inc"
};
static NJS_CNK_OBJECT draw_835 = {
#include "stg13/terrain/draw_835.inc"
};
static Sint16 draw_832_polygons[229] = {
#include "stg13/terrain/draw_832_polygons.inc"
};
static Sint32 draw_832_vertices[147] = {
#include "stg13/terrain/draw_832_vertices.inc"
};
static NJS_CNK_MODEL draw_832_model = {
#include "stg13/terrain/draw_832_model.inc"
};
static NJS_CNK_OBJECT draw_832 = {
#include "stg13/terrain/draw_832.inc"
};
static Sint16 draw_831_polygons[809] = {
#include "stg13/terrain/draw_831_polygons.inc"
};
static Sint32 draw_831_vertices[631] = {
#include "stg13/terrain/draw_831_vertices.inc"
};
static NJS_CNK_MODEL draw_831_model = {
#include "stg13/terrain/draw_831_model.inc"
};
static NJS_CNK_OBJECT draw_831 = {
#include "stg13/terrain/draw_831.inc"
};
static Sint16 draw_829_polygons[325] = {
#include "stg13/terrain/draw_829_polygons.inc"
};
static Sint32 draw_829_vertices[319] = {
#include "stg13/terrain/draw_829_vertices.inc"
};
static NJS_CNK_MODEL draw_829_model = {
#include "stg13/terrain/draw_829_model.inc"
};
static NJS_CNK_OBJECT draw_829 = {
#include "stg13/terrain/draw_829.inc"
};
static Sint16 draw_827_polygons[323] = {
#include "stg13/terrain/draw_827_polygons.inc"
};
static Sint32 draw_827_vertices[195] = {
#include "stg13/terrain/draw_827_vertices.inc"
};
static NJS_CNK_MODEL draw_827_model = {
#include "stg13/terrain/draw_827_model.inc"
};
static NJS_CNK_OBJECT draw_827 = {
#include "stg13/terrain/draw_827.inc"
};
static Sint16 draw_818_polygons[253] = {
#include "stg13/terrain/draw_818_polygons.inc"
};
static Sint32 draw_818_vertices[179] = {
#include "stg13/terrain/draw_818_vertices.inc"
};
static NJS_CNK_MODEL draw_818_model = {
#include "stg13/terrain/draw_818_model.inc"
};
static NJS_CNK_OBJECT draw_818 = {
#include "stg13/terrain/draw_818.inc"
};
static Sint16 draw_817_polygons[107] = {
#include "stg13/terrain/draw_817_polygons.inc"
};
static Sint32 draw_817_vertices[107] = {
#include "stg13/terrain/draw_817_vertices.inc"
};
static NJS_CNK_MODEL draw_817_model = {
#include "stg13/terrain/draw_817_model.inc"
};
static NJS_CNK_OBJECT draw_817 = {
#include "stg13/terrain/draw_817.inc"
};
static Sint16 draw_813_polygons[435] = {
#include "stg13/terrain/draw_813_polygons.inc"
};
static Sint32 draw_813_vertices[395] = {
#include "stg13/terrain/draw_813_vertices.inc"
};
static NJS_CNK_MODEL draw_813_model = {
#include "stg13/terrain/draw_813_model.inc"
};
static NJS_CNK_OBJECT draw_813 = {
#include "stg13/terrain/draw_813.inc"
};
static Sint16 draw_812_polygons[495] = {
#include "stg13/terrain/draw_812_polygons.inc"
};
static Sint32 draw_812_vertices[447] = {
#include "stg13/terrain/draw_812_vertices.inc"
};
static NJS_CNK_MODEL draw_812_model = {
#include "stg13/terrain/draw_812_model.inc"
};
static NJS_CNK_OBJECT draw_812 = {
#include "stg13/terrain/draw_812.inc"
};
static Sint16 draw_804_polygons[197] = {
#include "stg13/terrain/draw_804_polygons.inc"
};
static Sint32 draw_804_vertices[131] = {
#include "stg13/terrain/draw_804_vertices.inc"
};
static NJS_CNK_MODEL draw_804_model = {
#include "stg13/terrain/draw_804_model.inc"
};
static NJS_CNK_OBJECT draw_804 = {
#include "stg13/terrain/draw_804.inc"
};
static Sint16 draw_653_polygons[105] = {
#include "stg13/terrain/draw_653_polygons.inc"
};
static Sint32 draw_653_vertices[83] = {
#include "stg13/terrain/draw_653_vertices.inc"
};
static NJS_CNK_MODEL draw_653_model = {
#include "stg13/terrain/draw_653_model.inc"
};
static NJS_CNK_OBJECT draw_653 = {
#include "stg13/terrain/draw_653.inc"
};
static Sint16 draw_650_polygons[189] = {
#include "stg13/terrain/draw_650_polygons.inc"
};
static Sint32 draw_650_vertices[139] = {
#include "stg13/terrain/draw_650_vertices.inc"
};
static NJS_CNK_MODEL draw_650_model = {
#include "stg13/terrain/draw_650_model.inc"
};
static NJS_CNK_OBJECT draw_650 = {
#include "stg13/terrain/draw_650.inc"
};
static Sint16 draw_649_polygons[667] = {
#include "stg13/terrain/draw_649_polygons.inc"
};
static Sint32 draw_649_vertices[463] = {
#include "stg13/terrain/draw_649_vertices.inc"
};
static NJS_CNK_MODEL draw_649_model = {
#include "stg13/terrain/draw_649_model.inc"
};
static NJS_CNK_OBJECT draw_649 = {
#include "stg13/terrain/draw_649.inc"
};
static Sint16 draw_218_polygons[551] = {
#include "stg13/terrain/draw_218_polygons.inc"
};
static Sint32 draw_218_vertices[539] = {
#include "stg13/terrain/draw_218_vertices.inc"
};
static NJS_CNK_MODEL draw_218_model = {
#include "stg13/terrain/draw_218_model.inc"
};
static NJS_CNK_OBJECT draw_299 = {
#include "stg13/terrain/draw_299.inc"
};
static Sint16 draw_225_polygons[103] = {
#include "stg13/terrain/draw_225_polygons.inc"
};
static Sint32 draw_225_vertices[115] = {
#include "stg13/terrain/draw_225_vertices.inc"
};
static NJS_CNK_MODEL draw_225_model = {
#include "stg13/terrain/draw_225_model.inc"
};
static NJS_CNK_OBJECT draw_225 = {
#include "stg13/terrain/draw_225.inc"
};
static Sint16 draw_217_polygons[23] = {
#include "stg13/terrain/draw_217_polygons.inc"
};
static Sint32 draw_217_vertices[19] = {
#include "stg13/terrain/draw_217_vertices.inc"
};
static NJS_CNK_MODEL draw_217_model = {
#include "stg13/terrain/draw_217_model.inc"
};
static NJS_CNK_OBJECT draw_217 = {
#include "stg13/terrain/draw_217.inc"
};
static Sint16 draw_195_polygons[43] = {
#include "stg13/terrain/draw_195_polygons.inc"
};
static Sint32 draw_195_vertices[35] = {
#include "stg13/terrain/draw_195_vertices.inc"
};
static NJS_CNK_MODEL draw_195_model = {
#include "stg13/terrain/draw_195_model.inc"
};
static NJS_CNK_OBJECT draw_195 = {
#include "stg13/terrain/draw_195.inc"
};
static Sint16 draw_133_polygons[187] = {
#include "stg13/terrain/draw_133_polygons.inc"
};
static Sint32 draw_133_vertices[179] = {
#include "stg13/terrain/draw_133_vertices.inc"
};
static NJS_CNK_MODEL draw_133_model = {
#include "stg13/terrain/draw_133_model.inc"
};
static NJS_CNK_OBJECT draw_133 = {
#include "stg13/terrain/draw_133.inc"
};
static Sint16 draw_80_polygons[247] = {
#include "stg13/terrain/draw_80_polygons.inc"
};
static Sint32 draw_80_vertices[235] = {
#include "stg13/terrain/draw_80_vertices.inc"
};
static NJS_CNK_MODEL draw_80_model = {
#include "stg13/terrain/draw_80_model.inc"
};
static NJS_CNK_OBJECT draw_80 = {
#include "stg13/terrain/draw_80.inc"
};
static Sint16 draw_73_polygons[265] = {
#include "stg13/terrain/draw_73_polygons.inc"
};
static Sint32 draw_73_vertices[255] = {
#include "stg13/terrain/draw_73_vertices.inc"
};
static NJS_CNK_MODEL draw_73_model = {
#include "stg13/terrain/draw_73_model.inc"
};
static NJS_CNK_OBJECT draw_73 = {
#include "stg13/terrain/draw_73.inc"
};
static Sint16 draw_51_polygons[87] = {
#include "stg13/terrain/draw_51_polygons.inc"
};
static Sint32 draw_51_vertices[103] = {
#include "stg13/terrain/draw_51_vertices.inc"
};
static NJS_CNK_MODEL draw_51_model = {
#include "stg13/terrain/draw_51_model.inc"
};
static NJS_CNK_OBJECT draw_65 = {
#include "stg13/terrain/draw_65.inc"
};
static Sint16 draw_36_polygons[55] = {
#include "stg13/terrain/draw_36_polygons.inc"
};
static Sint32 draw_36_vertices[67] = {
#include "stg13/terrain/draw_36_vertices.inc"
};
static NJS_CNK_MODEL draw_36_model = {
#include "stg13/terrain/draw_36_model.inc"
};
static NJS_CNK_OBJECT draw_64 = {
#include "stg13/terrain/draw_64.inc"
};
static Sint16 draw_35_polygons[19] = {
#include "stg13/terrain/draw_35_polygons.inc"
};
static Sint32 draw_35_vertices[19] = {
#include "stg13/terrain/draw_35_vertices.inc"
};
static NJS_CNK_MODEL draw_35_model = {
#include "stg13/terrain/draw_35_model.inc"
};
static NJS_CNK_OBJECT draw_62 = {
#include "stg13/terrain/draw_62.inc"
};
static Sint16 draw_58_polygons[69] = {
#include "stg13/terrain/draw_58_polygons.inc"
};
static Sint32 draw_58_vertices[79] = {
#include "stg13/terrain/draw_58_vertices.inc"
};
static NJS_CNK_MODEL draw_58_model = {
#include "stg13/terrain/draw_58_model.inc"
};
static NJS_CNK_OBJECT draw_59 = {
#include "stg13/terrain/draw_59.inc"
};
static Sint16 draw_39_polygons[19] = {
#include "stg13/terrain/draw_39_polygons.inc"
};
static Sint32 draw_39_vertices[19] = {
#include "stg13/terrain/draw_39_vertices.inc"
};
static NJS_CNK_MODEL draw_39_model = {
#include "stg13/terrain/draw_39_model.inc"
};
static NJS_CNK_OBJECT draw_57 = {
#include "stg13/terrain/draw_57.inc"
};
static Sint16 draw_38_polygons[55] = {
#include "stg13/terrain/draw_38_polygons.inc"
};
static Sint32 draw_38_vertices[67] = {
#include "stg13/terrain/draw_38_vertices.inc"
};
static NJS_CNK_MODEL draw_38_model = {
#include "stg13/terrain/draw_38_model.inc"
};
static NJS_CNK_OBJECT draw_38 = {
#include "stg13/terrain/draw_38.inc"
};
static Sint16 draw_37_polygons[55] = {
#include "stg13/terrain/draw_37_polygons.inc"
};
static Sint32 draw_37_vertices[67] = {
#include "stg13/terrain/draw_37_vertices.inc"
};
static NJS_CNK_MODEL draw_37_model = {
#include "stg13/terrain/draw_37_model.inc"
};
static NJS_CNK_OBJECT draw_37 = {
#include "stg13/terrain/draw_37.inc"
};
static NJS_MATERIAL collision_533_materials[1] = {
#include "stg13/terrain/collision_533_materials.inc"
};
static Sint16 collision_533_faces[39] = {
#include "stg13/terrain/collision_533_faces.inc"
};
static NJS_MESHSET collision_533_meshsets[1] = {
#include "stg13/terrain/collision_533_meshsets.inc"
};
static NJS_POINT3 collision_533_points[22] = {
#include "stg13/terrain/collision_533_points.inc"
};
static NJS_MODEL collision_533_model = {
#include "stg13/terrain/collision_533_model.inc"
};
static NJS_OBJECT collision_533 = {
#include "stg13/terrain/collision_533.inc"
};
static Sint16 draw_34_polygons[195] = {
#include "stg13/terrain/draw_34_polygons.inc"
};
static Sint32 draw_34_vertices[187] = {
#include "stg13/terrain/draw_34_vertices.inc"
};
static NJS_CNK_MODEL draw_34_model = {
#include "stg13/terrain/draw_34_model.inc"
};
static NJS_CNK_OBJECT draw_34 = {
#include "stg13/terrain/draw_34.inc"
};
static Sint16 draw_23_polygons[1371] = {
#include "stg13/terrain/draw_23_polygons.inc"
};
static Sint32 draw_23_vertices[1023] = {
#include "stg13/terrain/draw_23_vertices.inc"
};
static NJS_CNK_MODEL draw_23_model = {
#include "stg13/terrain/draw_23_model.inc"
};
static NJS_CNK_OBJECT draw_33 = {
#include "stg13/terrain/draw_33.inc"
};
static Sint16 draw_11_polygons[1343] = {
#include "stg13/terrain/draw_11_polygons.inc"
};
static Sint32 draw_11_vertices[987] = {
#include "stg13/terrain/draw_11_vertices.inc"
};
static NJS_CNK_MODEL draw_11_model = {
#include "stg13/terrain/draw_11_model.inc"
};
static NJS_CNK_OBJECT draw_22 = {
#include "stg13/terrain/draw_22.inc"
};
static Sint16 draw_3_polygons[1541] = {
#include "stg13/terrain/draw_3_polygons.inc"
};
static Sint32 draw_3_vertices[1067] = {
#include "stg13/terrain/draw_3_vertices.inc"
};
static NJS_CNK_MODEL draw_3_model = {
#include "stg13/terrain/draw_3_model.inc"
};
static NJS_CNK_OBJECT draw_10 = {
#include "stg13/terrain/draw_10.inc"
};
static Sint16 draw_2_polygons[1211] = {
#include "stg13/terrain/draw_2_polygons.inc"
};
static Sint32 draw_2_vertices[927] = {
#include "stg13/terrain/draw_2_vertices.inc"
};
static NJS_CNK_MODEL draw_2_model = {
#include "stg13/terrain/draw_2_model.inc"
};
static NJS_CNK_OBJECT draw_2 = {
#include "stg13/terrain/draw_2.inc"
};
static Sint16 draw_0_polygons[419] = {
#include "stg13/terrain/draw_0_polygons.inc"
};
static Sint32 draw_0_vertices[435] = {
#include "stg13/terrain/draw_0_vertices.inc"
};
static NJS_CNK_MODEL draw_0_model = {
#include "stg13/terrain/draw_0_model.inc"
};
static NJS_CNK_OBJECT draw_1 = {
#include "stg13/terrain/draw_1.inc"
};
static NJS_OBJECT collision_19 = {
#include "stg13/terrain/collision_19.inc"
};
static NJS_OBJECT collision_23 = {
#include "stg13/terrain/collision_23.inc"
};
static NJS_OBJECT collision_25 = {
#include "stg13/terrain/collision_25.inc"
};
static NJS_OBJECT collision_26 = {
#include "stg13/terrain/collision_26.inc"
};
static NJS_OBJECT collision_27 = {
#include "stg13/terrain/collision_27.inc"
};
static NJS_OBJECT collision_31 = {
#include "stg13/terrain/collision_31.inc"
};
static NJS_OBJECT collision_44 = {
#include "stg13/terrain/collision_44.inc"
};
static NJS_OBJECT collision_46 = {
#include "stg13/terrain/collision_46.inc"
};
static NJS_OBJECT collision_51 = {
#include "stg13/terrain/collision_51.inc"
};
static NJS_OBJECT collision_54 = {
#include "stg13/terrain/collision_54.inc"
};
static NJS_OBJECT collision_59 = {
#include "stg13/terrain/collision_59.inc"
};
static NJS_OBJECT collision_61 = {
#include "stg13/terrain/collision_61.inc"
};
static NJS_OBJECT collision_64 = {
#include "stg13/terrain/collision_64.inc"
};
static NJS_OBJECT collision_65 = {
#include "stg13/terrain/collision_65.inc"
};
static NJS_OBJECT collision_67 = {
#include "stg13/terrain/collision_67.inc"
};
static NJS_OBJECT collision_68 = {
#include "stg13/terrain/collision_68.inc"
};
static NJS_OBJECT collision_77 = {
#include "stg13/terrain/collision_77.inc"
};
static NJS_OBJECT collision_94 = {
#include "stg13/terrain/collision_94.inc"
};
static NJS_OBJECT collision_95 = {
#include "stg13/terrain/collision_95.inc"
};
static NJS_OBJECT collision_96 = {
#include "stg13/terrain/collision_96.inc"
};
static NJS_OBJECT collision_97 = {
#include "stg13/terrain/collision_97.inc"
};
static NJS_OBJECT collision_98 = {
#include "stg13/terrain/collision_98.inc"
};
static NJS_OBJECT collision_99 = {
#include "stg13/terrain/collision_99.inc"
};
static NJS_OBJECT collision_100 = {
#include "stg13/terrain/collision_100.inc"
};
static NJS_OBJECT collision_103 = {
#include "stg13/terrain/collision_103.inc"
};
static NJS_OBJECT collision_104 = {
#include "stg13/terrain/collision_104.inc"
};
static NJS_OBJECT collision_106 = {
#include "stg13/terrain/collision_106.inc"
};
static NJS_OBJECT collision_107 = {
#include "stg13/terrain/collision_107.inc"
};
static NJS_OBJECT collision_109 = {
#include "stg13/terrain/collision_109.inc"
};
static NJS_OBJECT collision_110 = {
#include "stg13/terrain/collision_110.inc"
};
static NJS_OBJECT collision_112 = {
#include "stg13/terrain/collision_112.inc"
};
static NJS_OBJECT collision_114 = {
#include "stg13/terrain/collision_114.inc"
};
static NJS_OBJECT collision_116 = {
#include "stg13/terrain/collision_116.inc"
};
static NJS_OBJECT collision_117 = {
#include "stg13/terrain/collision_117.inc"
};
static NJS_OBJECT collision_119 = {
#include "stg13/terrain/collision_119.inc"
};
static NJS_OBJECT collision_123 = {
#include "stg13/terrain/collision_123.inc"
};
static NJS_OBJECT collision_124 = {
#include "stg13/terrain/collision_124.inc"
};
static NJS_OBJECT collision_126 = {
#include "stg13/terrain/collision_126.inc"
};
static NJS_OBJECT collision_127 = {
#include "stg13/terrain/collision_127.inc"
};
static NJS_OBJECT collision_129 = {
#include "stg13/terrain/collision_129.inc"
};
static NJS_OBJECT collision_130 = {
#include "stg13/terrain/collision_130.inc"
};
static NJS_OBJECT collision_132 = {
#include "stg13/terrain/collision_132.inc"
};
static NJS_OBJECT collision_133 = {
#include "stg13/terrain/collision_133.inc"
};
static NJS_OBJECT collision_135 = {
#include "stg13/terrain/collision_135.inc"
};
static NJS_OBJECT collision_136 = {
#include "stg13/terrain/collision_136.inc"
};
static NJS_OBJECT collision_138 = {
#include "stg13/terrain/collision_138.inc"
};
static NJS_OBJECT collision_139 = {
#include "stg13/terrain/collision_139.inc"
};
static NJS_OBJECT collision_140 = {
#include "stg13/terrain/collision_140.inc"
};
static NJS_OBJECT collision_142 = {
#include "stg13/terrain/collision_142.inc"
};
static NJS_OBJECT collision_143 = {
#include "stg13/terrain/collision_143.inc"
};
static NJS_OBJECT collision_145 = {
#include "stg13/terrain/collision_145.inc"
};
static NJS_OBJECT collision_147 = {
#include "stg13/terrain/collision_147.inc"
};
static NJS_OBJECT collision_149 = {
#include "stg13/terrain/collision_149.inc"
};
static NJS_OBJECT collision_151 = {
#include "stg13/terrain/collision_151.inc"
};
static NJS_OBJECT collision_153 = {
#include "stg13/terrain/collision_153.inc"
};
static NJS_OBJECT collision_155 = {
#include "stg13/terrain/collision_155.inc"
};
static NJS_OBJECT collision_160 = {
#include "stg13/terrain/collision_160.inc"
};
static NJS_OBJECT collision_165 = {
#include "stg13/terrain/collision_165.inc"
};
static NJS_OBJECT collision_167 = {
#include "stg13/terrain/collision_167.inc"
};
static NJS_OBJECT collision_168 = {
#include "stg13/terrain/collision_168.inc"
};
static NJS_OBJECT collision_169 = {
#include "stg13/terrain/collision_169.inc"
};
static NJS_OBJECT collision_170 = {
#include "stg13/terrain/collision_170.inc"
};
static NJS_OBJECT collision_171 = {
#include "stg13/terrain/collision_171.inc"
};
static NJS_OBJECT collision_173 = {
#include "stg13/terrain/collision_173.inc"
};
static NJS_OBJECT collision_176 = {
#include "stg13/terrain/collision_176.inc"
};
static NJS_OBJECT collision_180 = {
#include "stg13/terrain/collision_180.inc"
};
static NJS_OBJECT collision_182 = {
#include "stg13/terrain/collision_182.inc"
};
static NJS_OBJECT collision_183 = {
#include "stg13/terrain/collision_183.inc"
};
static NJS_OBJECT collision_184 = {
#include "stg13/terrain/collision_184.inc"
};
static NJS_OBJECT collision_186 = {
#include "stg13/terrain/collision_186.inc"
};
static NJS_OBJECT collision_191 = {
#include "stg13/terrain/collision_191.inc"
};
static NJS_OBJECT collision_192 = {
#include "stg13/terrain/collision_192.inc"
};
static NJS_OBJECT collision_193 = {
#include "stg13/terrain/collision_193.inc"
};
static NJS_OBJECT collision_195 = {
#include "stg13/terrain/collision_195.inc"
};
static NJS_OBJECT collision_208 = {
#include "stg13/terrain/collision_208.inc"
};
static NJS_OBJECT collision_209 = {
#include "stg13/terrain/collision_209.inc"
};
static NJS_OBJECT collision_210 = {
#include "stg13/terrain/collision_210.inc"
};
static NJS_OBJECT collision_211 = {
#include "stg13/terrain/collision_211.inc"
};
static NJS_OBJECT collision_212 = {
#include "stg13/terrain/collision_212.inc"
};
static NJS_OBJECT collision_213 = {
#include "stg13/terrain/collision_213.inc"
};
static NJS_OBJECT collision_214 = {
#include "stg13/terrain/collision_214.inc"
};
static NJS_OBJECT collision_215 = {
#include "stg13/terrain/collision_215.inc"
};
static NJS_OBJECT collision_216 = {
#include "stg13/terrain/collision_216.inc"
};
static NJS_OBJECT collision_217 = {
#include "stg13/terrain/collision_217.inc"
};
static NJS_OBJECT collision_218 = {
#include "stg13/terrain/collision_218.inc"
};
static NJS_OBJECT collision_219 = {
#include "stg13/terrain/collision_219.inc"
};
static NJS_OBJECT collision_220 = {
#include "stg13/terrain/collision_220.inc"
};
static NJS_OBJECT collision_221 = {
#include "stg13/terrain/collision_221.inc"
};
static NJS_OBJECT collision_222 = {
#include "stg13/terrain/collision_222.inc"
};
static NJS_OBJECT collision_224 = {
#include "stg13/terrain/collision_224.inc"
};
static NJS_OBJECT collision_225 = {
#include "stg13/terrain/collision_225.inc"
};
static NJS_OBJECT collision_226 = {
#include "stg13/terrain/collision_226.inc"
};
static NJS_OBJECT collision_243 = {
#include "stg13/terrain/collision_243.inc"
};
static NJS_OBJECT collision_244 = {
#include "stg13/terrain/collision_244.inc"
};
static NJS_OBJECT collision_246 = {
#include "stg13/terrain/collision_246.inc"
};
static NJS_OBJECT collision_247 = {
#include "stg13/terrain/collision_247.inc"
};
static NJS_OBJECT collision_248 = {
#include "stg13/terrain/collision_248.inc"
};
static NJS_OBJECT collision_249 = {
#include "stg13/terrain/collision_249.inc"
};
static NJS_OBJECT collision_250 = {
#include "stg13/terrain/collision_250.inc"
};
static NJS_OBJECT collision_251 = {
#include "stg13/terrain/collision_251.inc"
};
static NJS_OBJECT collision_252 = {
#include "stg13/terrain/collision_252.inc"
};
static NJS_OBJECT collision_256 = {
#include "stg13/terrain/collision_256.inc"
};
static NJS_OBJECT collision_257 = {
#include "stg13/terrain/collision_257.inc"
};
static NJS_OBJECT collision_258 = {
#include "stg13/terrain/collision_258.inc"
};
static NJS_OBJECT collision_259 = {
#include "stg13/terrain/collision_259.inc"
};
static NJS_OBJECT collision_260 = {
#include "stg13/terrain/collision_260.inc"
};
static NJS_OBJECT collision_261 = {
#include "stg13/terrain/collision_261.inc"
};
static NJS_OBJECT collision_262 = {
#include "stg13/terrain/collision_262.inc"
};
static NJS_OBJECT collision_275 = {
#include "stg13/terrain/collision_275.inc"
};
static NJS_OBJECT collision_277 = {
#include "stg13/terrain/collision_277.inc"
};
static NJS_OBJECT collision_280 = {
#include "stg13/terrain/collision_280.inc"
};
static NJS_OBJECT collision_289 = {
#include "stg13/terrain/collision_289.inc"
};
static NJS_OBJECT collision_290 = {
#include "stg13/terrain/collision_290.inc"
};
static NJS_OBJECT collision_292 = {
#include "stg13/terrain/collision_292.inc"
};
static NJS_OBJECT collision_293 = {
#include "stg13/terrain/collision_293.inc"
};
static NJS_OBJECT collision_294 = {
#include "stg13/terrain/collision_294.inc"
};
static NJS_OBJECT collision_295 = {
#include "stg13/terrain/collision_295.inc"
};
static NJS_OBJECT collision_302 = {
#include "stg13/terrain/collision_302.inc"
};
static NJS_OBJECT collision_303 = {
#include "stg13/terrain/collision_303.inc"
};
static NJS_OBJECT collision_304 = {
#include "stg13/terrain/collision_304.inc"
};
static NJS_OBJECT collision_305 = {
#include "stg13/terrain/collision_305.inc"
};
static NJS_OBJECT collision_307 = {
#include "stg13/terrain/collision_307.inc"
};
static NJS_OBJECT collision_308 = {
#include "stg13/terrain/collision_308.inc"
};
static NJS_OBJECT collision_309 = {
#include "stg13/terrain/collision_309.inc"
};
static NJS_OBJECT collision_310 = {
#include "stg13/terrain/collision_310.inc"
};
static NJS_OBJECT collision_314 = {
#include "stg13/terrain/collision_314.inc"
};
static NJS_OBJECT collision_315 = {
#include "stg13/terrain/collision_315.inc"
};
static NJS_OBJECT collision_338 = {
#include "stg13/terrain/collision_338.inc"
};
static NJS_OBJECT collision_339 = {
#include "stg13/terrain/collision_339.inc"
};
static NJS_OBJECT collision_340 = {
#include "stg13/terrain/collision_340.inc"
};
static NJS_OBJECT collision_348 = {
#include "stg13/terrain/collision_348.inc"
};
static NJS_OBJECT collision_356 = {
#include "stg13/terrain/collision_356.inc"
};
static NJS_OBJECT collision_362 = {
#include "stg13/terrain/collision_362.inc"
};
static NJS_OBJECT collision_365 = {
#include "stg13/terrain/collision_365.inc"
};
static NJS_OBJECT collision_368 = {
#include "stg13/terrain/collision_368.inc"
};
static NJS_OBJECT collision_370 = {
#include "stg13/terrain/collision_370.inc"
};
static NJS_OBJECT collision_372 = {
#include "stg13/terrain/collision_372.inc"
};
static NJS_OBJECT collision_377 = {
#include "stg13/terrain/collision_377.inc"
};
static NJS_OBJECT collision_386 = {
#include "stg13/terrain/collision_386.inc"
};
static NJS_OBJECT collision_392 = {
#include "stg13/terrain/collision_392.inc"
};
static NJS_OBJECT collision_395 = {
#include "stg13/terrain/collision_395.inc"
};
static NJS_OBJECT collision_396 = {
#include "stg13/terrain/collision_396.inc"
};
static NJS_OBJECT collision_398 = {
#include "stg13/terrain/collision_398.inc"
};
static NJS_OBJECT collision_400 = {
#include "stg13/terrain/collision_400.inc"
};
static NJS_OBJECT collision_401 = {
#include "stg13/terrain/collision_401.inc"
};
static NJS_OBJECT collision_403 = {
#include "stg13/terrain/collision_403.inc"
};
static NJS_OBJECT collision_404 = {
#include "stg13/terrain/collision_404.inc"
};
static NJS_OBJECT collision_407 = {
#include "stg13/terrain/collision_407.inc"
};
static NJS_OBJECT collision_408 = {
#include "stg13/terrain/collision_408.inc"
};
static NJS_OBJECT collision_410 = {
#include "stg13/terrain/collision_410.inc"
};
static NJS_OBJECT collision_411 = {
#include "stg13/terrain/collision_411.inc"
};
static NJS_OBJECT collision_414 = {
#include "stg13/terrain/collision_414.inc"
};
static NJS_OBJECT collision_415 = {
#include "stg13/terrain/collision_415.inc"
};
static NJS_OBJECT collision_417 = {
#include "stg13/terrain/collision_417.inc"
};
static NJS_OBJECT collision_420 = {
#include "stg13/terrain/collision_420.inc"
};
static NJS_OBJECT collision_421 = {
#include "stg13/terrain/collision_421.inc"
};
static NJS_OBJECT collision_423 = {
#include "stg13/terrain/collision_423.inc"
};
static NJS_OBJECT collision_424 = {
#include "stg13/terrain/collision_424.inc"
};
static NJS_OBJECT collision_432 = {
#include "stg13/terrain/collision_432.inc"
};
static NJS_OBJECT collision_435 = {
#include "stg13/terrain/collision_435.inc"
};
static NJS_OBJECT collision_437 = {
#include "stg13/terrain/collision_437.inc"
};
static NJS_OBJECT collision_444 = {
#include "stg13/terrain/collision_444.inc"
};
static NJS_OBJECT collision_445 = {
#include "stg13/terrain/collision_445.inc"
};
static NJS_OBJECT collision_471 = {
#include "stg13/terrain/collision_471.inc"
};
static NJS_OBJECT collision_473 = {
#include "stg13/terrain/collision_473.inc"
};
static NJS_OBJECT collision_475 = {
#include "stg13/terrain/collision_475.inc"
};
static NJS_OBJECT collision_480 = {
#include "stg13/terrain/collision_480.inc"
};
static NJS_OBJECT collision_482 = {
#include "stg13/terrain/collision_482.inc"
};
static NJS_OBJECT collision_494 = {
#include "stg13/terrain/collision_494.inc"
};
static NJS_OBJECT collision_495 = {
#include "stg13/terrain/collision_495.inc"
};
static NJS_OBJECT collision_498 = {
#include "stg13/terrain/collision_498.inc"
};
static NJS_OBJECT collision_500 = {
#include "stg13/terrain/collision_500.inc"
};
static NJS_OBJECT collision_501 = {
#include "stg13/terrain/collision_501.inc"
};
static NJS_OBJECT collision_503 = {
#include "stg13/terrain/collision_503.inc"
};
static NJS_OBJECT collision_504 = {
#include "stg13/terrain/collision_504.inc"
};
static NJS_OBJECT collision_505 = {
#include "stg13/terrain/collision_505.inc"
};
static NJS_OBJECT collision_506 = {
#include "stg13/terrain/collision_506.inc"
};
static NJS_OBJECT collision_507 = {
#include "stg13/terrain/collision_507.inc"
};
static NJS_OBJECT collision_508 = {
#include "stg13/terrain/collision_508.inc"
};
static NJS_OBJECT collision_510 = {
#include "stg13/terrain/collision_510.inc"
};
static NJS_OBJECT collision_511 = {
#include "stg13/terrain/collision_511.inc"
};
static NJS_OBJECT collision_512 = {
#include "stg13/terrain/collision_512.inc"
};
static NJS_OBJECT collision_513 = {
#include "stg13/terrain/collision_513.inc"
};
static NJS_OBJECT collision_514 = {
#include "stg13/terrain/collision_514.inc"
};
static NJS_OBJECT collision_515 = {
#include "stg13/terrain/collision_515.inc"
};
static NJS_OBJECT collision_516 = {
#include "stg13/terrain/collision_516.inc"
};
static NJS_OBJECT collision_518 = {
#include "stg13/terrain/collision_518.inc"
};
static NJS_OBJECT collision_519 = {
#include "stg13/terrain/collision_519.inc"
};
static NJS_OBJECT collision_520 = {
#include "stg13/terrain/collision_520.inc"
};
static NJS_OBJECT collision_521 = {
#include "stg13/terrain/collision_521.inc"
};
static NJS_OBJECT collision_522 = {
#include "stg13/terrain/collision_522.inc"
};
static NJS_OBJECT collision_523 = {
#include "stg13/terrain/collision_523.inc"
};
static NJS_OBJECT collision_524 = {
#include "stg13/terrain/collision_524.inc"
};
static NJS_OBJECT collision_526 = {
#include "stg13/terrain/collision_526.inc"
};
static NJS_OBJECT collision_528 = {
#include "stg13/terrain/collision_528.inc"
};
static NJS_CNK_OBJECT draw_2198 = {
#include "stg13/terrain/draw_2198.inc"
};
static NJS_CNK_OBJECT draw_2196 = {
#include "stg13/terrain/draw_2196.inc"
};
static NJS_CNK_OBJECT draw_2195 = {
#include "stg13/terrain/draw_2195.inc"
};
static NJS_CNK_OBJECT draw_2192 = {
#include "stg13/terrain/draw_2192.inc"
};
static NJS_CNK_OBJECT draw_2190 = {
#include "stg13/terrain/draw_2190.inc"
};
static NJS_CNK_OBJECT draw_2189 = {
#include "stg13/terrain/draw_2189.inc"
};
static NJS_CNK_OBJECT draw_2188 = {
#include "stg13/terrain/draw_2188.inc"
};
static NJS_CNK_OBJECT draw_2186 = {
#include "stg13/terrain/draw_2186.inc"
};
static NJS_CNK_OBJECT draw_2183 = {
#include "stg13/terrain/draw_2183.inc"
};
static NJS_CNK_OBJECT draw_2181 = {
#include "stg13/terrain/draw_2181.inc"
};
static NJS_CNK_OBJECT draw_2180 = {
#include "stg13/terrain/draw_2180.inc"
};
static NJS_CNK_OBJECT draw_2179 = {
#include "stg13/terrain/draw_2179.inc"
};
static NJS_CNK_OBJECT draw_2178 = {
#include "stg13/terrain/draw_2178.inc"
};
static NJS_CNK_OBJECT draw_2177 = {
#include "stg13/terrain/draw_2177.inc"
};
static NJS_CNK_OBJECT draw_2176 = {
#include "stg13/terrain/draw_2176.inc"
};
static NJS_CNK_OBJECT draw_2175 = {
#include "stg13/terrain/draw_2175.inc"
};
static NJS_CNK_OBJECT draw_2174 = {
#include "stg13/terrain/draw_2174.inc"
};
static NJS_CNK_OBJECT draw_2173 = {
#include "stg13/terrain/draw_2173.inc"
};
static NJS_CNK_OBJECT draw_2172 = {
#include "stg13/terrain/draw_2172.inc"
};
static NJS_CNK_OBJECT draw_2171 = {
#include "stg13/terrain/draw_2171.inc"
};
static NJS_CNK_OBJECT draw_2170 = {
#include "stg13/terrain/draw_2170.inc"
};
static NJS_CNK_OBJECT draw_2169 = {
#include "stg13/terrain/draw_2169.inc"
};
static NJS_CNK_OBJECT draw_2168 = {
#include "stg13/terrain/draw_2168.inc"
};
static NJS_CNK_OBJECT draw_2167 = {
#include "stg13/terrain/draw_2167.inc"
};
static NJS_CNK_OBJECT draw_2166 = {
#include "stg13/terrain/draw_2166.inc"
};
static NJS_CNK_OBJECT draw_2165 = {
#include "stg13/terrain/draw_2165.inc"
};
static NJS_CNK_OBJECT draw_2164 = {
#include "stg13/terrain/draw_2164.inc"
};
static NJS_CNK_OBJECT draw_2163 = {
#include "stg13/terrain/draw_2163.inc"
};
static NJS_CNK_OBJECT draw_2162 = {
#include "stg13/terrain/draw_2162.inc"
};
static NJS_CNK_OBJECT draw_2161 = {
#include "stg13/terrain/draw_2161.inc"
};
static NJS_CNK_OBJECT draw_2160 = {
#include "stg13/terrain/draw_2160.inc"
};
static NJS_CNK_OBJECT draw_2159 = {
#include "stg13/terrain/draw_2159.inc"
};
static NJS_CNK_OBJECT draw_2158 = {
#include "stg13/terrain/draw_2158.inc"
};
static NJS_CNK_OBJECT draw_2157 = {
#include "stg13/terrain/draw_2157.inc"
};
static NJS_CNK_OBJECT draw_2156 = {
#include "stg13/terrain/draw_2156.inc"
};
static NJS_CNK_OBJECT draw_2155 = {
#include "stg13/terrain/draw_2155.inc"
};
static NJS_CNK_OBJECT draw_2154 = {
#include "stg13/terrain/draw_2154.inc"
};
static NJS_CNK_OBJECT draw_2153 = {
#include "stg13/terrain/draw_2153.inc"
};
static NJS_CNK_OBJECT draw_2152 = {
#include "stg13/terrain/draw_2152.inc"
};
static NJS_CNK_OBJECT draw_2151 = {
#include "stg13/terrain/draw_2151.inc"
};
static NJS_CNK_OBJECT draw_2150 = {
#include "stg13/terrain/draw_2150.inc"
};
static NJS_CNK_OBJECT draw_2149 = {
#include "stg13/terrain/draw_2149.inc"
};
static NJS_CNK_OBJECT draw_2148 = {
#include "stg13/terrain/draw_2148.inc"
};
static NJS_CNK_OBJECT draw_2147 = {
#include "stg13/terrain/draw_2147.inc"
};
static NJS_CNK_OBJECT draw_2146 = {
#include "stg13/terrain/draw_2146.inc"
};
static NJS_CNK_OBJECT draw_2145 = {
#include "stg13/terrain/draw_2145.inc"
};
static NJS_CNK_OBJECT draw_2144 = {
#include "stg13/terrain/draw_2144.inc"
};
static NJS_CNK_OBJECT draw_2143 = {
#include "stg13/terrain/draw_2143.inc"
};
static NJS_CNK_OBJECT draw_2142 = {
#include "stg13/terrain/draw_2142.inc"
};
static NJS_CNK_OBJECT draw_2141 = {
#include "stg13/terrain/draw_2141.inc"
};
static NJS_CNK_OBJECT draw_2140 = {
#include "stg13/terrain/draw_2140.inc"
};
static NJS_CNK_OBJECT draw_2139 = {
#include "stg13/terrain/draw_2139.inc"
};
static NJS_CNK_OBJECT draw_2138 = {
#include "stg13/terrain/draw_2138.inc"
};
static NJS_CNK_OBJECT draw_2137 = {
#include "stg13/terrain/draw_2137.inc"
};
static NJS_CNK_OBJECT draw_2136 = {
#include "stg13/terrain/draw_2136.inc"
};
static NJS_CNK_OBJECT draw_2135 = {
#include "stg13/terrain/draw_2135.inc"
};
static NJS_CNK_OBJECT draw_2134 = {
#include "stg13/terrain/draw_2134.inc"
};
static NJS_CNK_OBJECT draw_2133 = {
#include "stg13/terrain/draw_2133.inc"
};
static NJS_CNK_OBJECT draw_2132 = {
#include "stg13/terrain/draw_2132.inc"
};
static NJS_CNK_OBJECT draw_2131 = {
#include "stg13/terrain/draw_2131.inc"
};
static NJS_CNK_OBJECT draw_2130 = {
#include "stg13/terrain/draw_2130.inc"
};
static NJS_CNK_OBJECT draw_2129 = {
#include "stg13/terrain/draw_2129.inc"
};
static NJS_CNK_OBJECT draw_2128 = {
#include "stg13/terrain/draw_2128.inc"
};
static NJS_CNK_OBJECT draw_2127 = {
#include "stg13/terrain/draw_2127.inc"
};
static NJS_CNK_OBJECT draw_2126 = {
#include "stg13/terrain/draw_2126.inc"
};
static NJS_CNK_OBJECT draw_2125 = {
#include "stg13/terrain/draw_2125.inc"
};
static NJS_CNK_OBJECT draw_2124 = {
#include "stg13/terrain/draw_2124.inc"
};
static NJS_CNK_OBJECT draw_2123 = {
#include "stg13/terrain/draw_2123.inc"
};
static NJS_CNK_OBJECT draw_2122 = {
#include "stg13/terrain/draw_2122.inc"
};
static NJS_CNK_OBJECT draw_2121 = {
#include "stg13/terrain/draw_2121.inc"
};
static NJS_CNK_OBJECT draw_2120 = {
#include "stg13/terrain/draw_2120.inc"
};
static NJS_CNK_OBJECT draw_2119 = {
#include "stg13/terrain/draw_2119.inc"
};
static NJS_CNK_OBJECT draw_2118 = {
#include "stg13/terrain/draw_2118.inc"
};
static NJS_CNK_OBJECT draw_2117 = {
#include "stg13/terrain/draw_2117.inc"
};
static NJS_CNK_OBJECT draw_2116 = {
#include "stg13/terrain/draw_2116.inc"
};
static NJS_CNK_OBJECT draw_2115 = {
#include "stg13/terrain/draw_2115.inc"
};
static NJS_CNK_OBJECT draw_2114 = {
#include "stg13/terrain/draw_2114.inc"
};
static NJS_CNK_OBJECT draw_2112 = {
#include "stg13/terrain/draw_2112.inc"
};
static NJS_CNK_OBJECT draw_2111 = {
#include "stg13/terrain/draw_2111.inc"
};
static NJS_CNK_OBJECT draw_2110 = {
#include "stg13/terrain/draw_2110.inc"
};
static NJS_CNK_OBJECT draw_2109 = {
#include "stg13/terrain/draw_2109.inc"
};
static NJS_CNK_OBJECT draw_2108 = {
#include "stg13/terrain/draw_2108.inc"
};
static NJS_CNK_OBJECT draw_2107 = {
#include "stg13/terrain/draw_2107.inc"
};
static NJS_CNK_OBJECT draw_2106 = {
#include "stg13/terrain/draw_2106.inc"
};
static NJS_CNK_OBJECT draw_2105 = {
#include "stg13/terrain/draw_2105.inc"
};
static NJS_CNK_OBJECT draw_2104 = {
#include "stg13/terrain/draw_2104.inc"
};
static NJS_CNK_OBJECT draw_2103 = {
#include "stg13/terrain/draw_2103.inc"
};
static NJS_CNK_OBJECT draw_2102 = {
#include "stg13/terrain/draw_2102.inc"
};
static NJS_CNK_OBJECT draw_2101 = {
#include "stg13/terrain/draw_2101.inc"
};
static NJS_CNK_OBJECT draw_2100 = {
#include "stg13/terrain/draw_2100.inc"
};
static NJS_CNK_OBJECT draw_2099 = {
#include "stg13/terrain/draw_2099.inc"
};
static NJS_CNK_OBJECT draw_2098 = {
#include "stg13/terrain/draw_2098.inc"
};
static NJS_CNK_OBJECT draw_2097 = {
#include "stg13/terrain/draw_2097.inc"
};
static NJS_CNK_OBJECT draw_2096 = {
#include "stg13/terrain/draw_2096.inc"
};
static NJS_CNK_OBJECT draw_2095 = {
#include "stg13/terrain/draw_2095.inc"
};
static NJS_CNK_OBJECT draw_2094 = {
#include "stg13/terrain/draw_2094.inc"
};
static NJS_CNK_OBJECT draw_2093 = {
#include "stg13/terrain/draw_2093.inc"
};
static NJS_CNK_OBJECT draw_2092 = {
#include "stg13/terrain/draw_2092.inc"
};
static NJS_CNK_OBJECT draw_2091 = {
#include "stg13/terrain/draw_2091.inc"
};
static NJS_CNK_OBJECT draw_2090 = {
#include "stg13/terrain/draw_2090.inc"
};
static NJS_CNK_OBJECT draw_2089 = {
#include "stg13/terrain/draw_2089.inc"
};
static NJS_CNK_OBJECT draw_2088 = {
#include "stg13/terrain/draw_2088.inc"
};
static NJS_CNK_OBJECT draw_2087 = {
#include "stg13/terrain/draw_2087.inc"
};
static NJS_CNK_OBJECT draw_2086 = {
#include "stg13/terrain/draw_2086.inc"
};
static NJS_CNK_OBJECT draw_2085 = {
#include "stg13/terrain/draw_2085.inc"
};
static NJS_CNK_OBJECT draw_2084 = {
#include "stg13/terrain/draw_2084.inc"
};
static NJS_CNK_OBJECT draw_2083 = {
#include "stg13/terrain/draw_2083.inc"
};
static NJS_CNK_OBJECT draw_2082 = {
#include "stg13/terrain/draw_2082.inc"
};
static NJS_CNK_OBJECT draw_2081 = {
#include "stg13/terrain/draw_2081.inc"
};
static NJS_CNK_OBJECT draw_2080 = {
#include "stg13/terrain/draw_2080.inc"
};
static NJS_CNK_OBJECT draw_2079 = {
#include "stg13/terrain/draw_2079.inc"
};
static NJS_CNK_OBJECT draw_2078 = {
#include "stg13/terrain/draw_2078.inc"
};
static NJS_CNK_OBJECT draw_2077 = {
#include "stg13/terrain/draw_2077.inc"
};
static NJS_CNK_OBJECT draw_2076 = {
#include "stg13/terrain/draw_2076.inc"
};
static NJS_CNK_OBJECT draw_2075 = {
#include "stg13/terrain/draw_2075.inc"
};
static NJS_CNK_OBJECT draw_2074 = {
#include "stg13/terrain/draw_2074.inc"
};
static NJS_CNK_OBJECT draw_2073 = {
#include "stg13/terrain/draw_2073.inc"
};
static NJS_CNK_OBJECT draw_2072 = {
#include "stg13/terrain/draw_2072.inc"
};
static NJS_CNK_OBJECT draw_2071 = {
#include "stg13/terrain/draw_2071.inc"
};
static NJS_CNK_OBJECT draw_2070 = {
#include "stg13/terrain/draw_2070.inc"
};
static NJS_CNK_OBJECT draw_2069 = {
#include "stg13/terrain/draw_2069.inc"
};
static NJS_CNK_OBJECT draw_2068 = {
#include "stg13/terrain/draw_2068.inc"
};
static NJS_CNK_OBJECT draw_2067 = {
#include "stg13/terrain/draw_2067.inc"
};
static NJS_CNK_OBJECT draw_2066 = {
#include "stg13/terrain/draw_2066.inc"
};
static NJS_CNK_OBJECT draw_2065 = {
#include "stg13/terrain/draw_2065.inc"
};
static NJS_CNK_OBJECT draw_2064 = {
#include "stg13/terrain/draw_2064.inc"
};
static NJS_CNK_OBJECT draw_2063 = {
#include "stg13/terrain/draw_2063.inc"
};
static NJS_CNK_OBJECT draw_2062 = {
#include "stg13/terrain/draw_2062.inc"
};
static NJS_CNK_OBJECT draw_2061 = {
#include "stg13/terrain/draw_2061.inc"
};
static NJS_CNK_OBJECT draw_2060 = {
#include "stg13/terrain/draw_2060.inc"
};
static NJS_CNK_OBJECT draw_2059 = {
#include "stg13/terrain/draw_2059.inc"
};
static NJS_CNK_OBJECT draw_2058 = {
#include "stg13/terrain/draw_2058.inc"
};
static NJS_CNK_OBJECT draw_2057 = {
#include "stg13/terrain/draw_2057.inc"
};
static NJS_CNK_OBJECT draw_2056 = {
#include "stg13/terrain/draw_2056.inc"
};
static NJS_CNK_OBJECT draw_2055 = {
#include "stg13/terrain/draw_2055.inc"
};
static NJS_CNK_OBJECT draw_2054 = {
#include "stg13/terrain/draw_2054.inc"
};
static NJS_CNK_OBJECT draw_2052 = {
#include "stg13/terrain/draw_2052.inc"
};
static NJS_CNK_OBJECT draw_2051 = {
#include "stg13/terrain/draw_2051.inc"
};
static NJS_CNK_OBJECT draw_2050 = {
#include "stg13/terrain/draw_2050.inc"
};
static NJS_CNK_OBJECT draw_2049 = {
#include "stg13/terrain/draw_2049.inc"
};
static NJS_CNK_OBJECT draw_2048 = {
#include "stg13/terrain/draw_2048.inc"
};
static NJS_CNK_OBJECT draw_2046 = {
#include "stg13/terrain/draw_2046.inc"
};
static NJS_CNK_OBJECT draw_2045 = {
#include "stg13/terrain/draw_2045.inc"
};
static NJS_CNK_OBJECT draw_2043 = {
#include "stg13/terrain/draw_2043.inc"
};
static NJS_CNK_OBJECT draw_2040 = {
#include "stg13/terrain/draw_2040.inc"
};
static NJS_CNK_OBJECT draw_2036 = {
#include "stg13/terrain/draw_2036.inc"
};
static NJS_CNK_OBJECT draw_2035 = {
#include "stg13/terrain/draw_2035.inc"
};
static NJS_CNK_OBJECT draw_2034 = {
#include "stg13/terrain/draw_2034.inc"
};
static NJS_CNK_OBJECT draw_2033 = {
#include "stg13/terrain/draw_2033.inc"
};
static NJS_CNK_OBJECT draw_2030 = {
#include "stg13/terrain/draw_2030.inc"
};
static NJS_CNK_OBJECT draw_2029 = {
#include "stg13/terrain/draw_2029.inc"
};
static NJS_CNK_OBJECT draw_2028 = {
#include "stg13/terrain/draw_2028.inc"
};
static NJS_CNK_OBJECT draw_2027 = {
#include "stg13/terrain/draw_2027.inc"
};
static NJS_CNK_OBJECT draw_2026 = {
#include "stg13/terrain/draw_2026.inc"
};
static NJS_CNK_OBJECT draw_2025 = {
#include "stg13/terrain/draw_2025.inc"
};
static NJS_CNK_OBJECT draw_2023 = {
#include "stg13/terrain/draw_2023.inc"
};
static NJS_CNK_OBJECT draw_2022 = {
#include "stg13/terrain/draw_2022.inc"
};
static NJS_CNK_OBJECT draw_2021 = {
#include "stg13/terrain/draw_2021.inc"
};
static NJS_CNK_OBJECT draw_2020 = {
#include "stg13/terrain/draw_2020.inc"
};
static NJS_CNK_OBJECT draw_2019 = {
#include "stg13/terrain/draw_2019.inc"
};
static NJS_CNK_OBJECT draw_2015 = {
#include "stg13/terrain/draw_2015.inc"
};
static NJS_CNK_OBJECT draw_2014 = {
#include "stg13/terrain/draw_2014.inc"
};
static NJS_CNK_OBJECT draw_2013 = {
#include "stg13/terrain/draw_2013.inc"
};
static NJS_CNK_OBJECT draw_2010 = {
#include "stg13/terrain/draw_2010.inc"
};
static NJS_CNK_OBJECT draw_2008 = {
#include "stg13/terrain/draw_2008.inc"
};
static NJS_CNK_OBJECT draw_2007 = {
#include "stg13/terrain/draw_2007.inc"
};
static NJS_CNK_OBJECT draw_2005 = {
#include "stg13/terrain/draw_2005.inc"
};
static NJS_CNK_OBJECT draw_2004 = {
#include "stg13/terrain/draw_2004.inc"
};
static NJS_CNK_OBJECT draw_2003 = {
#include "stg13/terrain/draw_2003.inc"
};
static NJS_CNK_OBJECT draw_2002 = {
#include "stg13/terrain/draw_2002.inc"
};
static NJS_CNK_OBJECT draw_2001 = {
#include "stg13/terrain/draw_2001.inc"
};
static NJS_CNK_OBJECT draw_1998 = {
#include "stg13/terrain/draw_1998.inc"
};
static NJS_CNK_OBJECT draw_1996 = {
#include "stg13/terrain/draw_1996.inc"
};
static NJS_CNK_OBJECT draw_1995 = {
#include "stg13/terrain/draw_1995.inc"
};
static NJS_CNK_OBJECT draw_1992 = {
#include "stg13/terrain/draw_1992.inc"
};
static NJS_CNK_OBJECT draw_1990 = {
#include "stg13/terrain/draw_1990.inc"
};
static NJS_CNK_OBJECT draw_1989 = {
#include "stg13/terrain/draw_1989.inc"
};
static NJS_CNK_OBJECT draw_1988 = {
#include "stg13/terrain/draw_1988.inc"
};
static NJS_CNK_OBJECT draw_1983 = {
#include "stg13/terrain/draw_1983.inc"
};
static NJS_CNK_OBJECT draw_1953 = {
#include "stg13/terrain/draw_1953.inc"
};
static NJS_CNK_OBJECT draw_1952 = {
#include "stg13/terrain/draw_1952.inc"
};
static NJS_CNK_OBJECT draw_1951 = {
#include "stg13/terrain/draw_1951.inc"
};
static NJS_CNK_OBJECT draw_1950 = {
#include "stg13/terrain/draw_1950.inc"
};
static NJS_CNK_OBJECT draw_1948 = {
#include "stg13/terrain/draw_1948.inc"
};
static NJS_CNK_OBJECT draw_1947 = {
#include "stg13/terrain/draw_1947.inc"
};
static NJS_CNK_OBJECT draw_1946 = {
#include "stg13/terrain/draw_1946.inc"
};
static NJS_CNK_OBJECT draw_1945 = {
#include "stg13/terrain/draw_1945.inc"
};
static NJS_CNK_OBJECT draw_1944 = {
#include "stg13/terrain/draw_1944.inc"
};
static NJS_CNK_OBJECT draw_1943 = {
#include "stg13/terrain/draw_1943.inc"
};
static NJS_CNK_OBJECT draw_1942 = {
#include "stg13/terrain/draw_1942.inc"
};
static NJS_CNK_OBJECT draw_1941 = {
#include "stg13/terrain/draw_1941.inc"
};
static NJS_CNK_OBJECT draw_1940 = {
#include "stg13/terrain/draw_1940.inc"
};
static NJS_CNK_OBJECT draw_1939 = {
#include "stg13/terrain/draw_1939.inc"
};
static NJS_CNK_OBJECT draw_1938 = {
#include "stg13/terrain/draw_1938.inc"
};
static NJS_CNK_OBJECT draw_1937 = {
#include "stg13/terrain/draw_1937.inc"
};
static NJS_CNK_OBJECT draw_1936 = {
#include "stg13/terrain/draw_1936.inc"
};
static NJS_CNK_OBJECT draw_1935 = {
#include "stg13/terrain/draw_1935.inc"
};
static NJS_CNK_OBJECT draw_1934 = {
#include "stg13/terrain/draw_1934.inc"
};
static NJS_CNK_OBJECT draw_1933 = {
#include "stg13/terrain/draw_1933.inc"
};
static NJS_CNK_OBJECT draw_1932 = {
#include "stg13/terrain/draw_1932.inc"
};
static NJS_CNK_OBJECT draw_1930 = {
#include "stg13/terrain/draw_1930.inc"
};
static NJS_CNK_OBJECT draw_1929 = {
#include "stg13/terrain/draw_1929.inc"
};
static NJS_CNK_OBJECT draw_1928 = {
#include "stg13/terrain/draw_1928.inc"
};
static NJS_CNK_OBJECT draw_1927 = {
#include "stg13/terrain/draw_1927.inc"
};
static NJS_CNK_OBJECT draw_1926 = {
#include "stg13/terrain/draw_1926.inc"
};
static NJS_CNK_OBJECT draw_1925 = {
#include "stg13/terrain/draw_1925.inc"
};
static NJS_CNK_OBJECT draw_1924 = {
#include "stg13/terrain/draw_1924.inc"
};
static NJS_CNK_OBJECT draw_1923 = {
#include "stg13/terrain/draw_1923.inc"
};
static NJS_CNK_OBJECT draw_1922 = {
#include "stg13/terrain/draw_1922.inc"
};
static NJS_CNK_OBJECT draw_1921 = {
#include "stg13/terrain/draw_1921.inc"
};
static NJS_CNK_OBJECT draw_1920 = {
#include "stg13/terrain/draw_1920.inc"
};
static NJS_CNK_OBJECT draw_1919 = {
#include "stg13/terrain/draw_1919.inc"
};
static NJS_CNK_OBJECT draw_1918 = {
#include "stg13/terrain/draw_1918.inc"
};
static NJS_CNK_OBJECT draw_1917 = {
#include "stg13/terrain/draw_1917.inc"
};
static NJS_CNK_OBJECT draw_1916 = {
#include "stg13/terrain/draw_1916.inc"
};
static NJS_CNK_OBJECT draw_1915 = {
#include "stg13/terrain/draw_1915.inc"
};
static NJS_CNK_OBJECT draw_1914 = {
#include "stg13/terrain/draw_1914.inc"
};
static NJS_CNK_OBJECT draw_1913 = {
#include "stg13/terrain/draw_1913.inc"
};
static NJS_CNK_OBJECT draw_1912 = {
#include "stg13/terrain/draw_1912.inc"
};
static NJS_CNK_OBJECT draw_1911 = {
#include "stg13/terrain/draw_1911.inc"
};
static NJS_CNK_OBJECT draw_1910 = {
#include "stg13/terrain/draw_1910.inc"
};
static NJS_CNK_OBJECT draw_1909 = {
#include "stg13/terrain/draw_1909.inc"
};
static NJS_CNK_OBJECT draw_1908 = {
#include "stg13/terrain/draw_1908.inc"
};
static NJS_CNK_OBJECT draw_1907 = {
#include "stg13/terrain/draw_1907.inc"
};
static NJS_CNK_OBJECT draw_1906 = {
#include "stg13/terrain/draw_1906.inc"
};
static NJS_CNK_OBJECT draw_1905 = {
#include "stg13/terrain/draw_1905.inc"
};
static NJS_CNK_OBJECT draw_1904 = {
#include "stg13/terrain/draw_1904.inc"
};
static NJS_CNK_OBJECT draw_1903 = {
#include "stg13/terrain/draw_1903.inc"
};
static NJS_CNK_OBJECT draw_1902 = {
#include "stg13/terrain/draw_1902.inc"
};
static NJS_CNK_OBJECT draw_1901 = {
#include "stg13/terrain/draw_1901.inc"
};
static NJS_CNK_OBJECT draw_1900 = {
#include "stg13/terrain/draw_1900.inc"
};
static NJS_CNK_OBJECT draw_1899 = {
#include "stg13/terrain/draw_1899.inc"
};
static NJS_CNK_OBJECT draw_1898 = {
#include "stg13/terrain/draw_1898.inc"
};
static NJS_CNK_OBJECT draw_1897 = {
#include "stg13/terrain/draw_1897.inc"
};
static NJS_CNK_OBJECT draw_1896 = {
#include "stg13/terrain/draw_1896.inc"
};
static NJS_CNK_OBJECT draw_1895 = {
#include "stg13/terrain/draw_1895.inc"
};
static NJS_CNK_OBJECT draw_1894 = {
#include "stg13/terrain/draw_1894.inc"
};
static NJS_CNK_OBJECT draw_1893 = {
#include "stg13/terrain/draw_1893.inc"
};
static NJS_CNK_OBJECT draw_1892 = {
#include "stg13/terrain/draw_1892.inc"
};
static NJS_CNK_OBJECT draw_1891 = {
#include "stg13/terrain/draw_1891.inc"
};
static NJS_CNK_OBJECT draw_1890 = {
#include "stg13/terrain/draw_1890.inc"
};
static NJS_CNK_OBJECT draw_1889 = {
#include "stg13/terrain/draw_1889.inc"
};
static NJS_CNK_OBJECT draw_1888 = {
#include "stg13/terrain/draw_1888.inc"
};
static NJS_CNK_OBJECT draw_1887 = {
#include "stg13/terrain/draw_1887.inc"
};
static NJS_CNK_OBJECT draw_1886 = {
#include "stg13/terrain/draw_1886.inc"
};
static NJS_CNK_OBJECT draw_1885 = {
#include "stg13/terrain/draw_1885.inc"
};
static NJS_CNK_OBJECT draw_1884 = {
#include "stg13/terrain/draw_1884.inc"
};
static NJS_CNK_OBJECT draw_1883 = {
#include "stg13/terrain/draw_1883.inc"
};
static NJS_CNK_OBJECT draw_1882 = {
#include "stg13/terrain/draw_1882.inc"
};
static NJS_CNK_OBJECT draw_1881 = {
#include "stg13/terrain/draw_1881.inc"
};
static NJS_CNK_OBJECT draw_1880 = {
#include "stg13/terrain/draw_1880.inc"
};
static NJS_CNK_OBJECT draw_1879 = {
#include "stg13/terrain/draw_1879.inc"
};
static NJS_CNK_OBJECT draw_1868 = {
#include "stg13/terrain/draw_1868.inc"
};
static NJS_CNK_OBJECT draw_1867 = {
#include "stg13/terrain/draw_1867.inc"
};
static NJS_CNK_OBJECT draw_1866 = {
#include "stg13/terrain/draw_1866.inc"
};
static NJS_CNK_OBJECT draw_1865 = {
#include "stg13/terrain/draw_1865.inc"
};
static NJS_CNK_OBJECT draw_1864 = {
#include "stg13/terrain/draw_1864.inc"
};
static NJS_CNK_OBJECT draw_1863 = {
#include "stg13/terrain/draw_1863.inc"
};
static NJS_CNK_OBJECT draw_1862 = {
#include "stg13/terrain/draw_1862.inc"
};
static NJS_CNK_OBJECT draw_1861 = {
#include "stg13/terrain/draw_1861.inc"
};
static NJS_CNK_OBJECT draw_1860 = {
#include "stg13/terrain/draw_1860.inc"
};
static NJS_CNK_OBJECT draw_1859 = {
#include "stg13/terrain/draw_1859.inc"
};
static NJS_CNK_OBJECT draw_1857 = {
#include "stg13/terrain/draw_1857.inc"
};
static NJS_CNK_OBJECT draw_1855 = {
#include "stg13/terrain/draw_1855.inc"
};
static NJS_CNK_OBJECT draw_1854 = {
#include "stg13/terrain/draw_1854.inc"
};
static NJS_CNK_OBJECT draw_1850 = {
#include "stg13/terrain/draw_1850.inc"
};
static NJS_CNK_OBJECT draw_1849 = {
#include "stg13/terrain/draw_1849.inc"
};
static NJS_CNK_OBJECT draw_1848 = {
#include "stg13/terrain/draw_1848.inc"
};
static NJS_CNK_OBJECT draw_1845 = {
#include "stg13/terrain/draw_1845.inc"
};
static NJS_CNK_OBJECT draw_1844 = {
#include "stg13/terrain/draw_1844.inc"
};
static NJS_CNK_OBJECT draw_1843 = {
#include "stg13/terrain/draw_1843.inc"
};
static NJS_CNK_OBJECT draw_1841 = {
#include "stg13/terrain/draw_1841.inc"
};
static NJS_CNK_OBJECT draw_1840 = {
#include "stg13/terrain/draw_1840.inc"
};
static NJS_CNK_OBJECT draw_1839 = {
#include "stg13/terrain/draw_1839.inc"
};
static NJS_CNK_OBJECT draw_1838 = {
#include "stg13/terrain/draw_1838.inc"
};
static NJS_CNK_OBJECT draw_1836 = {
#include "stg13/terrain/draw_1836.inc"
};
static NJS_CNK_OBJECT draw_1835 = {
#include "stg13/terrain/draw_1835.inc"
};
static NJS_CNK_OBJECT draw_1833 = {
#include "stg13/terrain/draw_1833.inc"
};
static NJS_CNK_OBJECT draw_1832 = {
#include "stg13/terrain/draw_1832.inc"
};
static NJS_CNK_OBJECT draw_1831 = {
#include "stg13/terrain/draw_1831.inc"
};
static NJS_CNK_OBJECT draw_1830 = {
#include "stg13/terrain/draw_1830.inc"
};
static NJS_CNK_OBJECT draw_1829 = {
#include "stg13/terrain/draw_1829.inc"
};
static NJS_CNK_OBJECT draw_1827 = {
#include "stg13/terrain/draw_1827.inc"
};
static NJS_CNK_OBJECT draw_1826 = {
#include "stg13/terrain/draw_1826.inc"
};
static NJS_CNK_OBJECT draw_1824 = {
#include "stg13/terrain/draw_1824.inc"
};
static NJS_CNK_OBJECT draw_1823 = {
#include "stg13/terrain/draw_1823.inc"
};
static NJS_CNK_OBJECT draw_1822 = {
#include "stg13/terrain/draw_1822.inc"
};
static NJS_CNK_OBJECT draw_1821 = {
#include "stg13/terrain/draw_1821.inc"
};
static NJS_CNK_OBJECT draw_1820 = {
#include "stg13/terrain/draw_1820.inc"
};
static NJS_CNK_OBJECT draw_1819 = {
#include "stg13/terrain/draw_1819.inc"
};
static NJS_CNK_OBJECT draw_1818 = {
#include "stg13/terrain/draw_1818.inc"
};
static NJS_CNK_OBJECT draw_1817 = {
#include "stg13/terrain/draw_1817.inc"
};
static NJS_CNK_OBJECT draw_1816 = {
#include "stg13/terrain/draw_1816.inc"
};
static NJS_CNK_OBJECT draw_1815 = {
#include "stg13/terrain/draw_1815.inc"
};
static NJS_CNK_OBJECT draw_1814 = {
#include "stg13/terrain/draw_1814.inc"
};
static NJS_CNK_OBJECT draw_1813 = {
#include "stg13/terrain/draw_1813.inc"
};
static NJS_CNK_OBJECT draw_1812 = {
#include "stg13/terrain/draw_1812.inc"
};
static NJS_CNK_OBJECT draw_1811 = {
#include "stg13/terrain/draw_1811.inc"
};
static NJS_CNK_OBJECT draw_1810 = {
#include "stg13/terrain/draw_1810.inc"
};
static NJS_CNK_OBJECT draw_1809 = {
#include "stg13/terrain/draw_1809.inc"
};
static NJS_CNK_OBJECT draw_1808 = {
#include "stg13/terrain/draw_1808.inc"
};
static NJS_CNK_OBJECT draw_1807 = {
#include "stg13/terrain/draw_1807.inc"
};
static NJS_CNK_OBJECT draw_1806 = {
#include "stg13/terrain/draw_1806.inc"
};
static NJS_CNK_OBJECT draw_1805 = {
#include "stg13/terrain/draw_1805.inc"
};
static NJS_CNK_OBJECT draw_1804 = {
#include "stg13/terrain/draw_1804.inc"
};
static NJS_CNK_OBJECT draw_1802 = {
#include "stg13/terrain/draw_1802.inc"
};
static NJS_CNK_OBJECT draw_1801 = {
#include "stg13/terrain/draw_1801.inc"
};
static NJS_CNK_OBJECT draw_1800 = {
#include "stg13/terrain/draw_1800.inc"
};
static NJS_CNK_OBJECT draw_1799 = {
#include "stg13/terrain/draw_1799.inc"
};
static NJS_CNK_OBJECT draw_1798 = {
#include "stg13/terrain/draw_1798.inc"
};
static NJS_CNK_OBJECT draw_1797 = {
#include "stg13/terrain/draw_1797.inc"
};
static NJS_CNK_OBJECT draw_1796 = {
#include "stg13/terrain/draw_1796.inc"
};
static NJS_CNK_OBJECT draw_1795 = {
#include "stg13/terrain/draw_1795.inc"
};
static NJS_CNK_OBJECT draw_1794 = {
#include "stg13/terrain/draw_1794.inc"
};
static NJS_CNK_OBJECT draw_1793 = {
#include "stg13/terrain/draw_1793.inc"
};
static NJS_CNK_OBJECT draw_1792 = {
#include "stg13/terrain/draw_1792.inc"
};
static NJS_CNK_OBJECT draw_1791 = {
#include "stg13/terrain/draw_1791.inc"
};
static NJS_CNK_OBJECT draw_1790 = {
#include "stg13/terrain/draw_1790.inc"
};
static NJS_CNK_OBJECT draw_1789 = {
#include "stg13/terrain/draw_1789.inc"
};
static NJS_CNK_OBJECT draw_1788 = {
#include "stg13/terrain/draw_1788.inc"
};
static NJS_CNK_OBJECT draw_1787 = {
#include "stg13/terrain/draw_1787.inc"
};
static NJS_CNK_OBJECT draw_1786 = {
#include "stg13/terrain/draw_1786.inc"
};
static NJS_CNK_OBJECT draw_1785 = {
#include "stg13/terrain/draw_1785.inc"
};
static NJS_CNK_OBJECT draw_1784 = {
#include "stg13/terrain/draw_1784.inc"
};
static NJS_CNK_OBJECT draw_1783 = {
#include "stg13/terrain/draw_1783.inc"
};
static NJS_CNK_OBJECT draw_1782 = {
#include "stg13/terrain/draw_1782.inc"
};
static NJS_CNK_OBJECT draw_1781 = {
#include "stg13/terrain/draw_1781.inc"
};
static NJS_CNK_OBJECT draw_1780 = {
#include "stg13/terrain/draw_1780.inc"
};
static NJS_CNK_OBJECT draw_1779 = {
#include "stg13/terrain/draw_1779.inc"
};
static NJS_CNK_OBJECT draw_1778 = {
#include "stg13/terrain/draw_1778.inc"
};
static NJS_CNK_OBJECT draw_1777 = {
#include "stg13/terrain/draw_1777.inc"
};
static NJS_CNK_OBJECT draw_1776 = {
#include "stg13/terrain/draw_1776.inc"
};
static NJS_CNK_OBJECT draw_1775 = {
#include "stg13/terrain/draw_1775.inc"
};
static NJS_CNK_OBJECT draw_1774 = {
#include "stg13/terrain/draw_1774.inc"
};
static NJS_CNK_OBJECT draw_1773 = {
#include "stg13/terrain/draw_1773.inc"
};
static NJS_CNK_OBJECT draw_1772 = {
#include "stg13/terrain/draw_1772.inc"
};
static NJS_CNK_OBJECT draw_1771 = {
#include "stg13/terrain/draw_1771.inc"
};
static NJS_CNK_OBJECT draw_1770 = {
#include "stg13/terrain/draw_1770.inc"
};
static NJS_CNK_OBJECT draw_1769 = {
#include "stg13/terrain/draw_1769.inc"
};
static NJS_CNK_OBJECT draw_1765 = {
#include "stg13/terrain/draw_1765.inc"
};
static NJS_CNK_OBJECT draw_1764 = {
#include "stg13/terrain/draw_1764.inc"
};
static NJS_CNK_OBJECT draw_1762 = {
#include "stg13/terrain/draw_1762.inc"
};
static NJS_CNK_OBJECT draw_1761 = {
#include "stg13/terrain/draw_1761.inc"
};
static NJS_CNK_OBJECT draw_1760 = {
#include "stg13/terrain/draw_1760.inc"
};
static NJS_CNK_OBJECT draw_1759 = {
#include "stg13/terrain/draw_1759.inc"
};
static NJS_CNK_OBJECT draw_1758 = {
#include "stg13/terrain/draw_1758.inc"
};
static NJS_CNK_OBJECT draw_1757 = {
#include "stg13/terrain/draw_1757.inc"
};
static NJS_CNK_OBJECT draw_1756 = {
#include "stg13/terrain/draw_1756.inc"
};
static NJS_CNK_OBJECT draw_1755 = {
#include "stg13/terrain/draw_1755.inc"
};
static NJS_CNK_OBJECT draw_1754 = {
#include "stg13/terrain/draw_1754.inc"
};
static NJS_CNK_OBJECT draw_1753 = {
#include "stg13/terrain/draw_1753.inc"
};
static NJS_CNK_OBJECT draw_1752 = {
#include "stg13/terrain/draw_1752.inc"
};
static NJS_CNK_OBJECT draw_1751 = {
#include "stg13/terrain/draw_1751.inc"
};
static NJS_CNK_OBJECT draw_1750 = {
#include "stg13/terrain/draw_1750.inc"
};
static NJS_CNK_OBJECT draw_1749 = {
#include "stg13/terrain/draw_1749.inc"
};
static NJS_CNK_OBJECT draw_1748 = {
#include "stg13/terrain/draw_1748.inc"
};
static NJS_CNK_OBJECT draw_1747 = {
#include "stg13/terrain/draw_1747.inc"
};
static NJS_CNK_OBJECT draw_1746 = {
#include "stg13/terrain/draw_1746.inc"
};
static NJS_CNK_OBJECT draw_1745 = {
#include "stg13/terrain/draw_1745.inc"
};
static NJS_CNK_OBJECT draw_1742 = {
#include "stg13/terrain/draw_1742.inc"
};
static NJS_CNK_OBJECT draw_1741 = {
#include "stg13/terrain/draw_1741.inc"
};
static NJS_CNK_OBJECT draw_1740 = {
#include "stg13/terrain/draw_1740.inc"
};
static NJS_CNK_OBJECT draw_1739 = {
#include "stg13/terrain/draw_1739.inc"
};
static NJS_CNK_OBJECT draw_1738 = {
#include "stg13/terrain/draw_1738.inc"
};
static NJS_CNK_OBJECT draw_1737 = {
#include "stg13/terrain/draw_1737.inc"
};
static NJS_CNK_OBJECT draw_1736 = {
#include "stg13/terrain/draw_1736.inc"
};
static NJS_CNK_OBJECT draw_1735 = {
#include "stg13/terrain/draw_1735.inc"
};
static NJS_CNK_OBJECT draw_1734 = {
#include "stg13/terrain/draw_1734.inc"
};
static NJS_CNK_OBJECT draw_1733 = {
#include "stg13/terrain/draw_1733.inc"
};
static NJS_CNK_OBJECT draw_1732 = {
#include "stg13/terrain/draw_1732.inc"
};
static NJS_CNK_OBJECT draw_1731 = {
#include "stg13/terrain/draw_1731.inc"
};
static NJS_CNK_OBJECT draw_1730 = {
#include "stg13/terrain/draw_1730.inc"
};
static NJS_CNK_OBJECT draw_1729 = {
#include "stg13/terrain/draw_1729.inc"
};
static NJS_CNK_OBJECT draw_1728 = {
#include "stg13/terrain/draw_1728.inc"
};
static NJS_CNK_OBJECT draw_1727 = {
#include "stg13/terrain/draw_1727.inc"
};
static NJS_CNK_OBJECT draw_1726 = {
#include "stg13/terrain/draw_1726.inc"
};
static NJS_CNK_OBJECT draw_1725 = {
#include "stg13/terrain/draw_1725.inc"
};
static NJS_CNK_OBJECT draw_1724 = {
#include "stg13/terrain/draw_1724.inc"
};
static NJS_CNK_OBJECT draw_1723 = {
#include "stg13/terrain/draw_1723.inc"
};
static NJS_CNK_OBJECT draw_1722 = {
#include "stg13/terrain/draw_1722.inc"
};
static NJS_CNK_OBJECT draw_1721 = {
#include "stg13/terrain/draw_1721.inc"
};
static NJS_CNK_OBJECT draw_1720 = {
#include "stg13/terrain/draw_1720.inc"
};
static NJS_CNK_OBJECT draw_1719 = {
#include "stg13/terrain/draw_1719.inc"
};
static NJS_CNK_OBJECT draw_1718 = {
#include "stg13/terrain/draw_1718.inc"
};
static NJS_CNK_OBJECT draw_1717 = {
#include "stg13/terrain/draw_1717.inc"
};
static NJS_CNK_OBJECT draw_1716 = {
#include "stg13/terrain/draw_1716.inc"
};
static NJS_CNK_OBJECT draw_1715 = {
#include "stg13/terrain/draw_1715.inc"
};
static NJS_CNK_OBJECT draw_1714 = {
#include "stg13/terrain/draw_1714.inc"
};
static NJS_CNK_OBJECT draw_1713 = {
#include "stg13/terrain/draw_1713.inc"
};
static NJS_CNK_OBJECT draw_1712 = {
#include "stg13/terrain/draw_1712.inc"
};
static NJS_CNK_OBJECT draw_1711 = {
#include "stg13/terrain/draw_1711.inc"
};
static NJS_CNK_OBJECT draw_1710 = {
#include "stg13/terrain/draw_1710.inc"
};
static NJS_CNK_OBJECT draw_1709 = {
#include "stg13/terrain/draw_1709.inc"
};
static NJS_CNK_OBJECT draw_1708 = {
#include "stg13/terrain/draw_1708.inc"
};
static NJS_CNK_OBJECT draw_1707 = {
#include "stg13/terrain/draw_1707.inc"
};
static NJS_CNK_OBJECT draw_1706 = {
#include "stg13/terrain/draw_1706.inc"
};
static NJS_CNK_OBJECT draw_1705 = {
#include "stg13/terrain/draw_1705.inc"
};
static NJS_CNK_OBJECT draw_1704 = {
#include "stg13/terrain/draw_1704.inc"
};
static NJS_CNK_OBJECT draw_1703 = {
#include "stg13/terrain/draw_1703.inc"
};
static NJS_CNK_OBJECT draw_1702 = {
#include "stg13/terrain/draw_1702.inc"
};
static NJS_CNK_OBJECT draw_1701 = {
#include "stg13/terrain/draw_1701.inc"
};
static NJS_CNK_OBJECT draw_1700 = {
#include "stg13/terrain/draw_1700.inc"
};
static NJS_CNK_OBJECT draw_1699 = {
#include "stg13/terrain/draw_1699.inc"
};
static NJS_CNK_OBJECT draw_1698 = {
#include "stg13/terrain/draw_1698.inc"
};
static NJS_CNK_OBJECT draw_1697 = {
#include "stg13/terrain/draw_1697.inc"
};
static NJS_CNK_OBJECT draw_1696 = {
#include "stg13/terrain/draw_1696.inc"
};
static NJS_CNK_OBJECT draw_1695 = {
#include "stg13/terrain/draw_1695.inc"
};
static NJS_CNK_OBJECT draw_1694 = {
#include "stg13/terrain/draw_1694.inc"
};
static NJS_CNK_OBJECT draw_1693 = {
#include "stg13/terrain/draw_1693.inc"
};
static NJS_CNK_OBJECT draw_1692 = {
#include "stg13/terrain/draw_1692.inc"
};
static NJS_CNK_OBJECT draw_1691 = {
#include "stg13/terrain/draw_1691.inc"
};
static NJS_CNK_OBJECT draw_1690 = {
#include "stg13/terrain/draw_1690.inc"
};
static NJS_CNK_OBJECT draw_1689 = {
#include "stg13/terrain/draw_1689.inc"
};
static NJS_CNK_OBJECT draw_1688 = {
#include "stg13/terrain/draw_1688.inc"
};
static NJS_CNK_OBJECT draw_1687 = {
#include "stg13/terrain/draw_1687.inc"
};
static NJS_CNK_OBJECT draw_1686 = {
#include "stg13/terrain/draw_1686.inc"
};
static NJS_CNK_OBJECT draw_1685 = {
#include "stg13/terrain/draw_1685.inc"
};
static NJS_CNK_OBJECT draw_1684 = {
#include "stg13/terrain/draw_1684.inc"
};
static NJS_CNK_OBJECT draw_1683 = {
#include "stg13/terrain/draw_1683.inc"
};
static NJS_CNK_OBJECT draw_1682 = {
#include "stg13/terrain/draw_1682.inc"
};
static NJS_CNK_OBJECT draw_1681 = {
#include "stg13/terrain/draw_1681.inc"
};
static NJS_CNK_OBJECT draw_1680 = {
#include "stg13/terrain/draw_1680.inc"
};
static NJS_CNK_OBJECT draw_1679 = {
#include "stg13/terrain/draw_1679.inc"
};
static NJS_CNK_OBJECT draw_1678 = {
#include "stg13/terrain/draw_1678.inc"
};
static NJS_CNK_OBJECT draw_1677 = {
#include "stg13/terrain/draw_1677.inc"
};
static NJS_CNK_OBJECT draw_1676 = {
#include "stg13/terrain/draw_1676.inc"
};
static NJS_CNK_OBJECT draw_1675 = {
#include "stg13/terrain/draw_1675.inc"
};
static NJS_CNK_OBJECT draw_1674 = {
#include "stg13/terrain/draw_1674.inc"
};
static NJS_CNK_OBJECT draw_1673 = {
#include "stg13/terrain/draw_1673.inc"
};
static NJS_CNK_OBJECT draw_1671 = {
#include "stg13/terrain/draw_1671.inc"
};
static NJS_CNK_OBJECT draw_1670 = {
#include "stg13/terrain/draw_1670.inc"
};
static NJS_CNK_OBJECT draw_1669 = {
#include "stg13/terrain/draw_1669.inc"
};
static NJS_CNK_OBJECT draw_1668 = {
#include "stg13/terrain/draw_1668.inc"
};
static NJS_CNK_OBJECT draw_1667 = {
#include "stg13/terrain/draw_1667.inc"
};
static NJS_CNK_OBJECT draw_1666 = {
#include "stg13/terrain/draw_1666.inc"
};
static NJS_CNK_OBJECT draw_1665 = {
#include "stg13/terrain/draw_1665.inc"
};
static NJS_CNK_OBJECT draw_1664 = {
#include "stg13/terrain/draw_1664.inc"
};
static NJS_CNK_OBJECT draw_1663 = {
#include "stg13/terrain/draw_1663.inc"
};
static NJS_CNK_OBJECT draw_1662 = {
#include "stg13/terrain/draw_1662.inc"
};
static NJS_CNK_OBJECT draw_1661 = {
#include "stg13/terrain/draw_1661.inc"
};
static NJS_CNK_OBJECT draw_1660 = {
#include "stg13/terrain/draw_1660.inc"
};
static NJS_CNK_OBJECT draw_1659 = {
#include "stg13/terrain/draw_1659.inc"
};
static NJS_CNK_OBJECT draw_1658 = {
#include "stg13/terrain/draw_1658.inc"
};
static NJS_CNK_OBJECT draw_1656 = {
#include "stg13/terrain/draw_1656.inc"
};
static NJS_CNK_OBJECT draw_1654 = {
#include "stg13/terrain/draw_1654.inc"
};
static NJS_CNK_OBJECT draw_1653 = {
#include "stg13/terrain/draw_1653.inc"
};
static NJS_CNK_OBJECT draw_1652 = {
#include "stg13/terrain/draw_1652.inc"
};
static NJS_CNK_OBJECT draw_1651 = {
#include "stg13/terrain/draw_1651.inc"
};
static NJS_CNK_OBJECT draw_1650 = {
#include "stg13/terrain/draw_1650.inc"
};
static NJS_CNK_OBJECT draw_1649 = {
#include "stg13/terrain/draw_1649.inc"
};
static NJS_CNK_OBJECT draw_1648 = {
#include "stg13/terrain/draw_1648.inc"
};
static NJS_CNK_OBJECT draw_1647 = {
#include "stg13/terrain/draw_1647.inc"
};
static NJS_CNK_OBJECT draw_1646 = {
#include "stg13/terrain/draw_1646.inc"
};
static NJS_CNK_OBJECT draw_1645 = {
#include "stg13/terrain/draw_1645.inc"
};
static NJS_CNK_OBJECT draw_1644 = {
#include "stg13/terrain/draw_1644.inc"
};
static NJS_CNK_OBJECT draw_1643 = {
#include "stg13/terrain/draw_1643.inc"
};
static NJS_CNK_OBJECT draw_1642 = {
#include "stg13/terrain/draw_1642.inc"
};
static NJS_CNK_OBJECT draw_1641 = {
#include "stg13/terrain/draw_1641.inc"
};
static NJS_CNK_OBJECT draw_1638 = {
#include "stg13/terrain/draw_1638.inc"
};
static NJS_CNK_OBJECT draw_1635 = {
#include "stg13/terrain/draw_1635.inc"
};
static NJS_CNK_OBJECT draw_1626 = {
#include "stg13/terrain/draw_1626.inc"
};
static NJS_CNK_OBJECT draw_1617 = {
#include "stg13/terrain/draw_1617.inc"
};
static NJS_CNK_OBJECT draw_1616 = {
#include "stg13/terrain/draw_1616.inc"
};
static NJS_CNK_OBJECT draw_1612 = {
#include "stg13/terrain/draw_1612.inc"
};
static NJS_CNK_OBJECT draw_1611 = {
#include "stg13/terrain/draw_1611.inc"
};
static NJS_CNK_OBJECT draw_1610 = {
#include "stg13/terrain/draw_1610.inc"
};
static NJS_CNK_OBJECT draw_1609 = {
#include "stg13/terrain/draw_1609.inc"
};
static NJS_CNK_OBJECT draw_1608 = {
#include "stg13/terrain/draw_1608.inc"
};
static NJS_CNK_OBJECT draw_1607 = {
#include "stg13/terrain/draw_1607.inc"
};
static NJS_CNK_OBJECT draw_1605 = {
#include "stg13/terrain/draw_1605.inc"
};
static NJS_CNK_OBJECT draw_1604 = {
#include "stg13/terrain/draw_1604.inc"
};
static NJS_CNK_OBJECT draw_1601 = {
#include "stg13/terrain/draw_1601.inc"
};
static NJS_CNK_OBJECT draw_1599 = {
#include "stg13/terrain/draw_1599.inc"
};
static NJS_CNK_OBJECT draw_1597 = {
#include "stg13/terrain/draw_1597.inc"
};
static NJS_CNK_OBJECT draw_1592 = {
#include "stg13/terrain/draw_1592.inc"
};
static NJS_CNK_OBJECT draw_1591 = {
#include "stg13/terrain/draw_1591.inc"
};
static NJS_CNK_OBJECT draw_1589 = {
#include "stg13/terrain/draw_1589.inc"
};
static NJS_CNK_OBJECT draw_1588 = {
#include "stg13/terrain/draw_1588.inc"
};
static NJS_CNK_OBJECT draw_1587 = {
#include "stg13/terrain/draw_1587.inc"
};
static NJS_CNK_OBJECT draw_1586 = {
#include "stg13/terrain/draw_1586.inc"
};
static NJS_CNK_OBJECT draw_1582 = {
#include "stg13/terrain/draw_1582.inc"
};
static NJS_CNK_OBJECT draw_1578 = {
#include "stg13/terrain/draw_1578.inc"
};
static NJS_CNK_OBJECT draw_1575 = {
#include "stg13/terrain/draw_1575.inc"
};
static NJS_CNK_OBJECT draw_1574 = {
#include "stg13/terrain/draw_1574.inc"
};
static NJS_CNK_OBJECT draw_1573 = {
#include "stg13/terrain/draw_1573.inc"
};
static NJS_CNK_OBJECT draw_1572 = {
#include "stg13/terrain/draw_1572.inc"
};
static NJS_CNK_OBJECT draw_1569 = {
#include "stg13/terrain/draw_1569.inc"
};
static NJS_CNK_OBJECT draw_1568 = {
#include "stg13/terrain/draw_1568.inc"
};
static NJS_CNK_OBJECT draw_1567 = {
#include "stg13/terrain/draw_1567.inc"
};
static NJS_CNK_OBJECT draw_1566 = {
#include "stg13/terrain/draw_1566.inc"
};
static NJS_CNK_OBJECT draw_1565 = {
#include "stg13/terrain/draw_1565.inc"
};
static NJS_CNK_OBJECT draw_1564 = {
#include "stg13/terrain/draw_1564.inc"
};
static NJS_CNK_OBJECT draw_1563 = {
#include "stg13/terrain/draw_1563.inc"
};
static NJS_CNK_OBJECT draw_1562 = {
#include "stg13/terrain/draw_1562.inc"
};
static NJS_CNK_OBJECT draw_1561 = {
#include "stg13/terrain/draw_1561.inc"
};
static NJS_CNK_OBJECT draw_1560 = {
#include "stg13/terrain/draw_1560.inc"
};
static NJS_CNK_OBJECT draw_1559 = {
#include "stg13/terrain/draw_1559.inc"
};
static NJS_CNK_OBJECT draw_1558 = {
#include "stg13/terrain/draw_1558.inc"
};
static NJS_CNK_OBJECT draw_1557 = {
#include "stg13/terrain/draw_1557.inc"
};
static NJS_CNK_OBJECT draw_1556 = {
#include "stg13/terrain/draw_1556.inc"
};
static NJS_CNK_OBJECT draw_1555 = {
#include "stg13/terrain/draw_1555.inc"
};
static NJS_CNK_OBJECT draw_1553 = {
#include "stg13/terrain/draw_1553.inc"
};
static NJS_CNK_OBJECT draw_1551 = {
#include "stg13/terrain/draw_1551.inc"
};
static NJS_CNK_OBJECT draw_1550 = {
#include "stg13/terrain/draw_1550.inc"
};
static NJS_CNK_OBJECT draw_1549 = {
#include "stg13/terrain/draw_1549.inc"
};
static NJS_CNK_OBJECT draw_1548 = {
#include "stg13/terrain/draw_1548.inc"
};
static NJS_CNK_OBJECT draw_1547 = {
#include "stg13/terrain/draw_1547.inc"
};
static NJS_CNK_OBJECT draw_1546 = {
#include "stg13/terrain/draw_1546.inc"
};
static NJS_CNK_OBJECT draw_1545 = {
#include "stg13/terrain/draw_1545.inc"
};
static NJS_CNK_OBJECT draw_1544 = {
#include "stg13/terrain/draw_1544.inc"
};
static NJS_CNK_OBJECT draw_1543 = {
#include "stg13/terrain/draw_1543.inc"
};
static NJS_CNK_OBJECT draw_1542 = {
#include "stg13/terrain/draw_1542.inc"
};
static NJS_CNK_OBJECT draw_1541 = {
#include "stg13/terrain/draw_1541.inc"
};
static NJS_CNK_OBJECT draw_1540 = {
#include "stg13/terrain/draw_1540.inc"
};
static NJS_CNK_OBJECT draw_1539 = {
#include "stg13/terrain/draw_1539.inc"
};
static NJS_CNK_OBJECT draw_1538 = {
#include "stg13/terrain/draw_1538.inc"
};
static NJS_CNK_OBJECT draw_1537 = {
#include "stg13/terrain/draw_1537.inc"
};
static NJS_CNK_OBJECT draw_1536 = {
#include "stg13/terrain/draw_1536.inc"
};
static NJS_CNK_OBJECT draw_1535 = {
#include "stg13/terrain/draw_1535.inc"
};
static NJS_CNK_OBJECT draw_1534 = {
#include "stg13/terrain/draw_1534.inc"
};
static NJS_CNK_OBJECT draw_1533 = {
#include "stg13/terrain/draw_1533.inc"
};
static NJS_CNK_OBJECT draw_1532 = {
#include "stg13/terrain/draw_1532.inc"
};
static NJS_CNK_OBJECT draw_1531 = {
#include "stg13/terrain/draw_1531.inc"
};
static NJS_CNK_OBJECT draw_1530 = {
#include "stg13/terrain/draw_1530.inc"
};
static NJS_CNK_OBJECT draw_1529 = {
#include "stg13/terrain/draw_1529.inc"
};
static NJS_CNK_OBJECT draw_1528 = {
#include "stg13/terrain/draw_1528.inc"
};
static NJS_CNK_OBJECT draw_1527 = {
#include "stg13/terrain/draw_1527.inc"
};
static NJS_CNK_OBJECT draw_1526 = {
#include "stg13/terrain/draw_1526.inc"
};
static NJS_CNK_OBJECT draw_1525 = {
#include "stg13/terrain/draw_1525.inc"
};
static NJS_CNK_OBJECT draw_1524 = {
#include "stg13/terrain/draw_1524.inc"
};
static NJS_CNK_OBJECT draw_1523 = {
#include "stg13/terrain/draw_1523.inc"
};
static NJS_CNK_OBJECT draw_1522 = {
#include "stg13/terrain/draw_1522.inc"
};
static NJS_CNK_OBJECT draw_1521 = {
#include "stg13/terrain/draw_1521.inc"
};
static NJS_CNK_OBJECT draw_1520 = {
#include "stg13/terrain/draw_1520.inc"
};
static NJS_CNK_OBJECT draw_1519 = {
#include "stg13/terrain/draw_1519.inc"
};
static NJS_CNK_OBJECT draw_1518 = {
#include "stg13/terrain/draw_1518.inc"
};
static NJS_CNK_OBJECT draw_1517 = {
#include "stg13/terrain/draw_1517.inc"
};
static NJS_CNK_OBJECT draw_1516 = {
#include "stg13/terrain/draw_1516.inc"
};
static NJS_CNK_OBJECT draw_1515 = {
#include "stg13/terrain/draw_1515.inc"
};
static NJS_CNK_OBJECT draw_1514 = {
#include "stg13/terrain/draw_1514.inc"
};
static NJS_CNK_OBJECT draw_1513 = {
#include "stg13/terrain/draw_1513.inc"
};
static NJS_CNK_OBJECT draw_1512 = {
#include "stg13/terrain/draw_1512.inc"
};
static NJS_CNK_OBJECT draw_1511 = {
#include "stg13/terrain/draw_1511.inc"
};
static NJS_CNK_OBJECT draw_1510 = {
#include "stg13/terrain/draw_1510.inc"
};
static NJS_CNK_OBJECT draw_1509 = {
#include "stg13/terrain/draw_1509.inc"
};
static NJS_CNK_OBJECT draw_1508 = {
#include "stg13/terrain/draw_1508.inc"
};
static NJS_CNK_OBJECT draw_1507 = {
#include "stg13/terrain/draw_1507.inc"
};
static NJS_CNK_OBJECT draw_1506 = {
#include "stg13/terrain/draw_1506.inc"
};
static NJS_CNK_OBJECT draw_1505 = {
#include "stg13/terrain/draw_1505.inc"
};
static NJS_CNK_OBJECT draw_1504 = {
#include "stg13/terrain/draw_1504.inc"
};
static NJS_CNK_OBJECT draw_1503 = {
#include "stg13/terrain/draw_1503.inc"
};
static NJS_CNK_OBJECT draw_1502 = {
#include "stg13/terrain/draw_1502.inc"
};
static NJS_CNK_OBJECT draw_1501 = {
#include "stg13/terrain/draw_1501.inc"
};
static NJS_CNK_OBJECT draw_1500 = {
#include "stg13/terrain/draw_1500.inc"
};
static NJS_CNK_OBJECT draw_1499 = {
#include "stg13/terrain/draw_1499.inc"
};
static NJS_CNK_OBJECT draw_1498 = {
#include "stg13/terrain/draw_1498.inc"
};
static NJS_CNK_OBJECT draw_1497 = {
#include "stg13/terrain/draw_1497.inc"
};
static NJS_CNK_OBJECT draw_1496 = {
#include "stg13/terrain/draw_1496.inc"
};
static NJS_CNK_OBJECT draw_1495 = {
#include "stg13/terrain/draw_1495.inc"
};
static NJS_CNK_OBJECT draw_1494 = {
#include "stg13/terrain/draw_1494.inc"
};
static NJS_CNK_OBJECT draw_1493 = {
#include "stg13/terrain/draw_1493.inc"
};
static NJS_CNK_OBJECT draw_1492 = {
#include "stg13/terrain/draw_1492.inc"
};
static NJS_CNK_OBJECT draw_1491 = {
#include "stg13/terrain/draw_1491.inc"
};
static NJS_CNK_OBJECT draw_1490 = {
#include "stg13/terrain/draw_1490.inc"
};
static NJS_CNK_OBJECT draw_1489 = {
#include "stg13/terrain/draw_1489.inc"
};
static NJS_CNK_OBJECT draw_1488 = {
#include "stg13/terrain/draw_1488.inc"
};
static NJS_CNK_OBJECT draw_1487 = {
#include "stg13/terrain/draw_1487.inc"
};
static NJS_CNK_OBJECT draw_1486 = {
#include "stg13/terrain/draw_1486.inc"
};
static NJS_CNK_OBJECT draw_1485 = {
#include "stg13/terrain/draw_1485.inc"
};
static NJS_CNK_OBJECT draw_1484 = {
#include "stg13/terrain/draw_1484.inc"
};
static NJS_CNK_OBJECT draw_1483 = {
#include "stg13/terrain/draw_1483.inc"
};
static NJS_CNK_OBJECT draw_1482 = {
#include "stg13/terrain/draw_1482.inc"
};
static NJS_CNK_OBJECT draw_1481 = {
#include "stg13/terrain/draw_1481.inc"
};
static NJS_CNK_OBJECT draw_1480 = {
#include "stg13/terrain/draw_1480.inc"
};
static NJS_CNK_OBJECT draw_1479 = {
#include "stg13/terrain/draw_1479.inc"
};
static NJS_CNK_OBJECT draw_1478 = {
#include "stg13/terrain/draw_1478.inc"
};
static NJS_CNK_OBJECT draw_1477 = {
#include "stg13/terrain/draw_1477.inc"
};
static NJS_CNK_OBJECT draw_1476 = {
#include "stg13/terrain/draw_1476.inc"
};
static NJS_CNK_OBJECT draw_1475 = {
#include "stg13/terrain/draw_1475.inc"
};
static NJS_CNK_OBJECT draw_1474 = {
#include "stg13/terrain/draw_1474.inc"
};
static NJS_CNK_OBJECT draw_1473 = {
#include "stg13/terrain/draw_1473.inc"
};
static NJS_CNK_OBJECT draw_1472 = {
#include "stg13/terrain/draw_1472.inc"
};
static NJS_CNK_OBJECT draw_1471 = {
#include "stg13/terrain/draw_1471.inc"
};
static NJS_CNK_OBJECT draw_1470 = {
#include "stg13/terrain/draw_1470.inc"
};
static NJS_CNK_OBJECT draw_1469 = {
#include "stg13/terrain/draw_1469.inc"
};
static NJS_CNK_OBJECT draw_1468 = {
#include "stg13/terrain/draw_1468.inc"
};
static NJS_CNK_OBJECT draw_1467 = {
#include "stg13/terrain/draw_1467.inc"
};
static NJS_CNK_OBJECT draw_1466 = {
#include "stg13/terrain/draw_1466.inc"
};
static NJS_CNK_OBJECT draw_1465 = {
#include "stg13/terrain/draw_1465.inc"
};
static NJS_CNK_OBJECT draw_1464 = {
#include "stg13/terrain/draw_1464.inc"
};
static NJS_CNK_OBJECT draw_1463 = {
#include "stg13/terrain/draw_1463.inc"
};
static NJS_CNK_OBJECT draw_1462 = {
#include "stg13/terrain/draw_1462.inc"
};
static NJS_CNK_OBJECT draw_1461 = {
#include "stg13/terrain/draw_1461.inc"
};
static NJS_CNK_OBJECT draw_1460 = {
#include "stg13/terrain/draw_1460.inc"
};
static NJS_CNK_OBJECT draw_1459 = {
#include "stg13/terrain/draw_1459.inc"
};
static NJS_CNK_OBJECT draw_1458 = {
#include "stg13/terrain/draw_1458.inc"
};
static NJS_CNK_OBJECT draw_1457 = {
#include "stg13/terrain/draw_1457.inc"
};
static NJS_CNK_OBJECT draw_1456 = {
#include "stg13/terrain/draw_1456.inc"
};
static NJS_CNK_OBJECT draw_1455 = {
#include "stg13/terrain/draw_1455.inc"
};
static NJS_CNK_OBJECT draw_1454 = {
#include "stg13/terrain/draw_1454.inc"
};
static NJS_CNK_OBJECT draw_1453 = {
#include "stg13/terrain/draw_1453.inc"
};
static NJS_CNK_OBJECT draw_1452 = {
#include "stg13/terrain/draw_1452.inc"
};
static NJS_CNK_OBJECT draw_1451 = {
#include "stg13/terrain/draw_1451.inc"
};
static NJS_CNK_OBJECT draw_1450 = {
#include "stg13/terrain/draw_1450.inc"
};
static NJS_CNK_OBJECT draw_1449 = {
#include "stg13/terrain/draw_1449.inc"
};
static NJS_CNK_OBJECT draw_1448 = {
#include "stg13/terrain/draw_1448.inc"
};
static NJS_CNK_OBJECT draw_1447 = {
#include "stg13/terrain/draw_1447.inc"
};
static NJS_CNK_OBJECT draw_1446 = {
#include "stg13/terrain/draw_1446.inc"
};
static NJS_CNK_OBJECT draw_1445 = {
#include "stg13/terrain/draw_1445.inc"
};
static NJS_CNK_OBJECT draw_1444 = {
#include "stg13/terrain/draw_1444.inc"
};
static NJS_CNK_OBJECT draw_1443 = {
#include "stg13/terrain/draw_1443.inc"
};
static NJS_CNK_OBJECT draw_1442 = {
#include "stg13/terrain/draw_1442.inc"
};
static NJS_CNK_OBJECT draw_1441 = {
#include "stg13/terrain/draw_1441.inc"
};
static NJS_CNK_OBJECT draw_1440 = {
#include "stg13/terrain/draw_1440.inc"
};
static NJS_CNK_OBJECT draw_1439 = {
#include "stg13/terrain/draw_1439.inc"
};
static NJS_CNK_OBJECT draw_1438 = {
#include "stg13/terrain/draw_1438.inc"
};
static NJS_CNK_OBJECT draw_1437 = {
#include "stg13/terrain/draw_1437.inc"
};
static NJS_CNK_OBJECT draw_1436 = {
#include "stg13/terrain/draw_1436.inc"
};
static NJS_CNK_OBJECT draw_1435 = {
#include "stg13/terrain/draw_1435.inc"
};
static NJS_CNK_OBJECT draw_1434 = {
#include "stg13/terrain/draw_1434.inc"
};
static NJS_CNK_OBJECT draw_1433 = {
#include "stg13/terrain/draw_1433.inc"
};
static NJS_CNK_OBJECT draw_1432 = {
#include "stg13/terrain/draw_1432.inc"
};
static NJS_CNK_OBJECT draw_1431 = {
#include "stg13/terrain/draw_1431.inc"
};
static NJS_CNK_OBJECT draw_1430 = {
#include "stg13/terrain/draw_1430.inc"
};
static NJS_CNK_OBJECT draw_1429 = {
#include "stg13/terrain/draw_1429.inc"
};
static NJS_CNK_OBJECT draw_1428 = {
#include "stg13/terrain/draw_1428.inc"
};
static NJS_CNK_OBJECT draw_1427 = {
#include "stg13/terrain/draw_1427.inc"
};
static NJS_CNK_OBJECT draw_1426 = {
#include "stg13/terrain/draw_1426.inc"
};
static NJS_CNK_OBJECT draw_1425 = {
#include "stg13/terrain/draw_1425.inc"
};
static NJS_CNK_OBJECT draw_1424 = {
#include "stg13/terrain/draw_1424.inc"
};
static NJS_CNK_OBJECT draw_1423 = {
#include "stg13/terrain/draw_1423.inc"
};
static NJS_CNK_OBJECT draw_1422 = {
#include "stg13/terrain/draw_1422.inc"
};
static NJS_CNK_OBJECT draw_1421 = {
#include "stg13/terrain/draw_1421.inc"
};
static NJS_CNK_OBJECT draw_1420 = {
#include "stg13/terrain/draw_1420.inc"
};
static NJS_CNK_OBJECT draw_1419 = {
#include "stg13/terrain/draw_1419.inc"
};
static NJS_CNK_OBJECT draw_1418 = {
#include "stg13/terrain/draw_1418.inc"
};
static NJS_CNK_OBJECT draw_1417 = {
#include "stg13/terrain/draw_1417.inc"
};
static NJS_CNK_OBJECT draw_1416 = {
#include "stg13/terrain/draw_1416.inc"
};
static NJS_CNK_OBJECT draw_1415 = {
#include "stg13/terrain/draw_1415.inc"
};
static NJS_CNK_OBJECT draw_1414 = {
#include "stg13/terrain/draw_1414.inc"
};
static NJS_CNK_OBJECT draw_1413 = {
#include "stg13/terrain/draw_1413.inc"
};
static NJS_CNK_OBJECT draw_1412 = {
#include "stg13/terrain/draw_1412.inc"
};
static NJS_CNK_OBJECT draw_1411 = {
#include "stg13/terrain/draw_1411.inc"
};
static NJS_CNK_OBJECT draw_1410 = {
#include "stg13/terrain/draw_1410.inc"
};
static NJS_CNK_OBJECT draw_1409 = {
#include "stg13/terrain/draw_1409.inc"
};
static NJS_CNK_OBJECT draw_1408 = {
#include "stg13/terrain/draw_1408.inc"
};
static NJS_CNK_OBJECT draw_1407 = {
#include "stg13/terrain/draw_1407.inc"
};
static NJS_CNK_OBJECT draw_1406 = {
#include "stg13/terrain/draw_1406.inc"
};
static NJS_CNK_OBJECT draw_1405 = {
#include "stg13/terrain/draw_1405.inc"
};
static NJS_CNK_OBJECT draw_1404 = {
#include "stg13/terrain/draw_1404.inc"
};
static NJS_CNK_OBJECT draw_1403 = {
#include "stg13/terrain/draw_1403.inc"
};
static NJS_CNK_OBJECT draw_1402 = {
#include "stg13/terrain/draw_1402.inc"
};
static NJS_CNK_OBJECT draw_1398 = {
#include "stg13/terrain/draw_1398.inc"
};
static NJS_CNK_OBJECT draw_1397 = {
#include "stg13/terrain/draw_1397.inc"
};
static NJS_CNK_OBJECT draw_1396 = {
#include "stg13/terrain/draw_1396.inc"
};
static NJS_CNK_OBJECT draw_1394 = {
#include "stg13/terrain/draw_1394.inc"
};
static NJS_CNK_OBJECT draw_1393 = {
#include "stg13/terrain/draw_1393.inc"
};
static NJS_CNK_OBJECT draw_1392 = {
#include "stg13/terrain/draw_1392.inc"
};
static NJS_CNK_OBJECT draw_1391 = {
#include "stg13/terrain/draw_1391.inc"
};
static NJS_CNK_OBJECT draw_1390 = {
#include "stg13/terrain/draw_1390.inc"
};
static NJS_CNK_OBJECT draw_1389 = {
#include "stg13/terrain/draw_1389.inc"
};
static NJS_CNK_OBJECT draw_1388 = {
#include "stg13/terrain/draw_1388.inc"
};
static NJS_CNK_OBJECT draw_1387 = {
#include "stg13/terrain/draw_1387.inc"
};
static NJS_CNK_OBJECT draw_1382 = {
#include "stg13/terrain/draw_1382.inc"
};
static NJS_CNK_OBJECT draw_1381 = {
#include "stg13/terrain/draw_1381.inc"
};
static NJS_CNK_OBJECT draw_1380 = {
#include "stg13/terrain/draw_1380.inc"
};
static NJS_CNK_OBJECT draw_1379 = {
#include "stg13/terrain/draw_1379.inc"
};
static NJS_CNK_OBJECT draw_1378 = {
#include "stg13/terrain/draw_1378.inc"
};
static NJS_CNK_OBJECT draw_1377 = {
#include "stg13/terrain/draw_1377.inc"
};
static NJS_CNK_OBJECT draw_1376 = {
#include "stg13/terrain/draw_1376.inc"
};
static NJS_CNK_OBJECT draw_1375 = {
#include "stg13/terrain/draw_1375.inc"
};
static NJS_CNK_OBJECT draw_1374 = {
#include "stg13/terrain/draw_1374.inc"
};
static NJS_CNK_OBJECT draw_1373 = {
#include "stg13/terrain/draw_1373.inc"
};
static NJS_CNK_OBJECT draw_1372 = {
#include "stg13/terrain/draw_1372.inc"
};
static NJS_CNK_OBJECT draw_1371 = {
#include "stg13/terrain/draw_1371.inc"
};
static NJS_CNK_OBJECT draw_1370 = {
#include "stg13/terrain/draw_1370.inc"
};
static NJS_CNK_OBJECT draw_1369 = {
#include "stg13/terrain/draw_1369.inc"
};
static NJS_CNK_OBJECT draw_1368 = {
#include "stg13/terrain/draw_1368.inc"
};
static NJS_CNK_OBJECT draw_1367 = {
#include "stg13/terrain/draw_1367.inc"
};
static NJS_CNK_OBJECT draw_1366 = {
#include "stg13/terrain/draw_1366.inc"
};
static NJS_CNK_OBJECT draw_1365 = {
#include "stg13/terrain/draw_1365.inc"
};
static NJS_CNK_OBJECT draw_1364 = {
#include "stg13/terrain/draw_1364.inc"
};
static NJS_CNK_OBJECT draw_1363 = {
#include "stg13/terrain/draw_1363.inc"
};
static NJS_CNK_OBJECT draw_1362 = {
#include "stg13/terrain/draw_1362.inc"
};
static NJS_CNK_OBJECT draw_1361 = {
#include "stg13/terrain/draw_1361.inc"
};
static NJS_CNK_OBJECT draw_1360 = {
#include "stg13/terrain/draw_1360.inc"
};
static NJS_CNK_OBJECT draw_1359 = {
#include "stg13/terrain/draw_1359.inc"
};
static NJS_CNK_OBJECT draw_1358 = {
#include "stg13/terrain/draw_1358.inc"
};
static NJS_CNK_OBJECT draw_1357 = {
#include "stg13/terrain/draw_1357.inc"
};
static NJS_CNK_OBJECT draw_1356 = {
#include "stg13/terrain/draw_1356.inc"
};
static NJS_CNK_OBJECT draw_1355 = {
#include "stg13/terrain/draw_1355.inc"
};
static NJS_CNK_OBJECT draw_1354 = {
#include "stg13/terrain/draw_1354.inc"
};
static NJS_CNK_OBJECT draw_1353 = {
#include "stg13/terrain/draw_1353.inc"
};
static NJS_CNK_OBJECT draw_1352 = {
#include "stg13/terrain/draw_1352.inc"
};
static NJS_CNK_OBJECT draw_1351 = {
#include "stg13/terrain/draw_1351.inc"
};
static NJS_CNK_OBJECT draw_1350 = {
#include "stg13/terrain/draw_1350.inc"
};
static NJS_CNK_OBJECT draw_1349 = {
#include "stg13/terrain/draw_1349.inc"
};
static NJS_CNK_OBJECT draw_1348 = {
#include "stg13/terrain/draw_1348.inc"
};
static NJS_CNK_OBJECT draw_1347 = {
#include "stg13/terrain/draw_1347.inc"
};
static NJS_CNK_OBJECT draw_1346 = {
#include "stg13/terrain/draw_1346.inc"
};
static NJS_CNK_OBJECT draw_1345 = {
#include "stg13/terrain/draw_1345.inc"
};
static NJS_CNK_OBJECT draw_1344 = {
#include "stg13/terrain/draw_1344.inc"
};
static NJS_CNK_OBJECT draw_1343 = {
#include "stg13/terrain/draw_1343.inc"
};
static NJS_CNK_OBJECT draw_1342 = {
#include "stg13/terrain/draw_1342.inc"
};
static NJS_CNK_OBJECT draw_1341 = {
#include "stg13/terrain/draw_1341.inc"
};
static NJS_CNK_OBJECT draw_1339 = {
#include "stg13/terrain/draw_1339.inc"
};
static NJS_CNK_OBJECT draw_1338 = {
#include "stg13/terrain/draw_1338.inc"
};
static NJS_CNK_OBJECT draw_1337 = {
#include "stg13/terrain/draw_1337.inc"
};
static NJS_CNK_OBJECT draw_1336 = {
#include "stg13/terrain/draw_1336.inc"
};
static NJS_CNK_OBJECT draw_1335 = {
#include "stg13/terrain/draw_1335.inc"
};
static NJS_CNK_OBJECT draw_1334 = {
#include "stg13/terrain/draw_1334.inc"
};
static NJS_CNK_OBJECT draw_1333 = {
#include "stg13/terrain/draw_1333.inc"
};
static NJS_CNK_OBJECT draw_1332 = {
#include "stg13/terrain/draw_1332.inc"
};
static NJS_CNK_OBJECT draw_1331 = {
#include "stg13/terrain/draw_1331.inc"
};
static NJS_CNK_OBJECT draw_1330 = {
#include "stg13/terrain/draw_1330.inc"
};
static NJS_CNK_OBJECT draw_1329 = {
#include "stg13/terrain/draw_1329.inc"
};
static NJS_CNK_OBJECT draw_1328 = {
#include "stg13/terrain/draw_1328.inc"
};
static NJS_CNK_OBJECT draw_1327 = {
#include "stg13/terrain/draw_1327.inc"
};
static NJS_CNK_OBJECT draw_1326 = {
#include "stg13/terrain/draw_1326.inc"
};
static NJS_CNK_OBJECT draw_1325 = {
#include "stg13/terrain/draw_1325.inc"
};
static NJS_CNK_OBJECT draw_1324 = {
#include "stg13/terrain/draw_1324.inc"
};
static NJS_CNK_OBJECT draw_1323 = {
#include "stg13/terrain/draw_1323.inc"
};
static NJS_CNK_OBJECT draw_1322 = {
#include "stg13/terrain/draw_1322.inc"
};
static NJS_CNK_OBJECT draw_1321 = {
#include "stg13/terrain/draw_1321.inc"
};
static NJS_CNK_OBJECT draw_1320 = {
#include "stg13/terrain/draw_1320.inc"
};
static NJS_CNK_OBJECT draw_1319 = {
#include "stg13/terrain/draw_1319.inc"
};
static NJS_CNK_OBJECT draw_1317 = {
#include "stg13/terrain/draw_1317.inc"
};
static NJS_CNK_OBJECT draw_1316 = {
#include "stg13/terrain/draw_1316.inc"
};
static NJS_CNK_OBJECT draw_1315 = {
#include "stg13/terrain/draw_1315.inc"
};
static NJS_CNK_OBJECT draw_1314 = {
#include "stg13/terrain/draw_1314.inc"
};
static NJS_CNK_OBJECT draw_1313 = {
#include "stg13/terrain/draw_1313.inc"
};
static NJS_CNK_OBJECT draw_1312 = {
#include "stg13/terrain/draw_1312.inc"
};
static NJS_CNK_OBJECT draw_1311 = {
#include "stg13/terrain/draw_1311.inc"
};
static NJS_CNK_OBJECT draw_1310 = {
#include "stg13/terrain/draw_1310.inc"
};
static NJS_CNK_OBJECT draw_1309 = {
#include "stg13/terrain/draw_1309.inc"
};
static NJS_CNK_OBJECT draw_1308 = {
#include "stg13/terrain/draw_1308.inc"
};
static NJS_CNK_OBJECT draw_1307 = {
#include "stg13/terrain/draw_1307.inc"
};
static NJS_CNK_OBJECT draw_1306 = {
#include "stg13/terrain/draw_1306.inc"
};
static NJS_CNK_OBJECT draw_1305 = {
#include "stg13/terrain/draw_1305.inc"
};
static NJS_CNK_OBJECT draw_1304 = {
#include "stg13/terrain/draw_1304.inc"
};
static NJS_CNK_OBJECT draw_1303 = {
#include "stg13/terrain/draw_1303.inc"
};
static NJS_CNK_OBJECT draw_1302 = {
#include "stg13/terrain/draw_1302.inc"
};
static NJS_CNK_OBJECT draw_1301 = {
#include "stg13/terrain/draw_1301.inc"
};
static NJS_CNK_OBJECT draw_1300 = {
#include "stg13/terrain/draw_1300.inc"
};
static NJS_CNK_OBJECT draw_1299 = {
#include "stg13/terrain/draw_1299.inc"
};
static NJS_CNK_OBJECT draw_1298 = {
#include "stg13/terrain/draw_1298.inc"
};
static NJS_CNK_OBJECT draw_1297 = {
#include "stg13/terrain/draw_1297.inc"
};
static NJS_CNK_OBJECT draw_1296 = {
#include "stg13/terrain/draw_1296.inc"
};
static NJS_CNK_OBJECT draw_1295 = {
#include "stg13/terrain/draw_1295.inc"
};
static NJS_CNK_OBJECT draw_1294 = {
#include "stg13/terrain/draw_1294.inc"
};
static NJS_CNK_OBJECT draw_1293 = {
#include "stg13/terrain/draw_1293.inc"
};
static NJS_CNK_OBJECT draw_1292 = {
#include "stg13/terrain/draw_1292.inc"
};
static NJS_CNK_OBJECT draw_1291 = {
#include "stg13/terrain/draw_1291.inc"
};
static NJS_CNK_OBJECT draw_1290 = {
#include "stg13/terrain/draw_1290.inc"
};
static NJS_CNK_OBJECT draw_1289 = {
#include "stg13/terrain/draw_1289.inc"
};
static NJS_CNK_OBJECT draw_1288 = {
#include "stg13/terrain/draw_1288.inc"
};
static NJS_CNK_OBJECT draw_1287 = {
#include "stg13/terrain/draw_1287.inc"
};
static NJS_CNK_OBJECT draw_1286 = {
#include "stg13/terrain/draw_1286.inc"
};
static NJS_CNK_OBJECT draw_1285 = {
#include "stg13/terrain/draw_1285.inc"
};
static NJS_CNK_OBJECT draw_1284 = {
#include "stg13/terrain/draw_1284.inc"
};
static NJS_CNK_OBJECT draw_1283 = {
#include "stg13/terrain/draw_1283.inc"
};
static NJS_CNK_OBJECT draw_1282 = {
#include "stg13/terrain/draw_1282.inc"
};
static NJS_CNK_OBJECT draw_1281 = {
#include "stg13/terrain/draw_1281.inc"
};
static NJS_CNK_OBJECT draw_1280 = {
#include "stg13/terrain/draw_1280.inc"
};
static NJS_CNK_OBJECT draw_1279 = {
#include "stg13/terrain/draw_1279.inc"
};
static NJS_CNK_OBJECT draw_1278 = {
#include "stg13/terrain/draw_1278.inc"
};
static NJS_CNK_OBJECT draw_1277 = {
#include "stg13/terrain/draw_1277.inc"
};
static NJS_CNK_OBJECT draw_1276 = {
#include "stg13/terrain/draw_1276.inc"
};
static NJS_CNK_OBJECT draw_1275 = {
#include "stg13/terrain/draw_1275.inc"
};
static NJS_CNK_OBJECT draw_1274 = {
#include "stg13/terrain/draw_1274.inc"
};
static NJS_CNK_OBJECT draw_1273 = {
#include "stg13/terrain/draw_1273.inc"
};
static NJS_CNK_OBJECT draw_1272 = {
#include "stg13/terrain/draw_1272.inc"
};
static NJS_CNK_OBJECT draw_1271 = {
#include "stg13/terrain/draw_1271.inc"
};
static NJS_CNK_OBJECT draw_1270 = {
#include "stg13/terrain/draw_1270.inc"
};
static NJS_CNK_OBJECT draw_1269 = {
#include "stg13/terrain/draw_1269.inc"
};
static NJS_CNK_OBJECT draw_1268 = {
#include "stg13/terrain/draw_1268.inc"
};
static NJS_CNK_OBJECT draw_1267 = {
#include "stg13/terrain/draw_1267.inc"
};
static NJS_CNK_OBJECT draw_1266 = {
#include "stg13/terrain/draw_1266.inc"
};
static NJS_CNK_OBJECT draw_1265 = {
#include "stg13/terrain/draw_1265.inc"
};
static NJS_CNK_OBJECT draw_1264 = {
#include "stg13/terrain/draw_1264.inc"
};
static NJS_CNK_OBJECT draw_1263 = {
#include "stg13/terrain/draw_1263.inc"
};
static NJS_CNK_OBJECT draw_1262 = {
#include "stg13/terrain/draw_1262.inc"
};
static NJS_CNK_OBJECT draw_1261 = {
#include "stg13/terrain/draw_1261.inc"
};
static NJS_CNK_OBJECT draw_1260 = {
#include "stg13/terrain/draw_1260.inc"
};
static NJS_CNK_OBJECT draw_1259 = {
#include "stg13/terrain/draw_1259.inc"
};
static NJS_CNK_OBJECT draw_1258 = {
#include "stg13/terrain/draw_1258.inc"
};
static NJS_CNK_OBJECT draw_1257 = {
#include "stg13/terrain/draw_1257.inc"
};
static NJS_CNK_OBJECT draw_1256 = {
#include "stg13/terrain/draw_1256.inc"
};
static NJS_CNK_OBJECT draw_1254 = {
#include "stg13/terrain/draw_1254.inc"
};
static NJS_CNK_OBJECT draw_1253 = {
#include "stg13/terrain/draw_1253.inc"
};
static NJS_CNK_OBJECT draw_1252 = {
#include "stg13/terrain/draw_1252.inc"
};
static NJS_CNK_OBJECT draw_1251 = {
#include "stg13/terrain/draw_1251.inc"
};
static NJS_CNK_OBJECT draw_1250 = {
#include "stg13/terrain/draw_1250.inc"
};
static NJS_CNK_OBJECT draw_1249 = {
#include "stg13/terrain/draw_1249.inc"
};
static NJS_CNK_OBJECT draw_1247 = {
#include "stg13/terrain/draw_1247.inc"
};
static NJS_CNK_OBJECT draw_1246 = {
#include "stg13/terrain/draw_1246.inc"
};
static NJS_CNK_OBJECT draw_1245 = {
#include "stg13/terrain/draw_1245.inc"
};
static NJS_CNK_OBJECT draw_1244 = {
#include "stg13/terrain/draw_1244.inc"
};
static NJS_CNK_OBJECT draw_1243 = {
#include "stg13/terrain/draw_1243.inc"
};
static NJS_CNK_OBJECT draw_1242 = {
#include "stg13/terrain/draw_1242.inc"
};
static NJS_CNK_OBJECT draw_1240 = {
#include "stg13/terrain/draw_1240.inc"
};
static NJS_CNK_OBJECT draw_1239 = {
#include "stg13/terrain/draw_1239.inc"
};
static NJS_CNK_OBJECT draw_1237 = {
#include "stg13/terrain/draw_1237.inc"
};
static NJS_CNK_OBJECT draw_1236 = {
#include "stg13/terrain/draw_1236.inc"
};
static NJS_CNK_OBJECT draw_1235 = {
#include "stg13/terrain/draw_1235.inc"
};
static NJS_CNK_OBJECT draw_1234 = {
#include "stg13/terrain/draw_1234.inc"
};
static NJS_CNK_OBJECT draw_1233 = {
#include "stg13/terrain/draw_1233.inc"
};
static NJS_CNK_OBJECT draw_1232 = {
#include "stg13/terrain/draw_1232.inc"
};
static NJS_CNK_OBJECT draw_1231 = {
#include "stg13/terrain/draw_1231.inc"
};
static NJS_CNK_OBJECT draw_1230 = {
#include "stg13/terrain/draw_1230.inc"
};
static NJS_CNK_OBJECT draw_1229 = {
#include "stg13/terrain/draw_1229.inc"
};
static NJS_CNK_OBJECT draw_1228 = {
#include "stg13/terrain/draw_1228.inc"
};
static NJS_CNK_OBJECT draw_1227 = {
#include "stg13/terrain/draw_1227.inc"
};
static NJS_CNK_OBJECT draw_1226 = {
#include "stg13/terrain/draw_1226.inc"
};
static NJS_CNK_OBJECT draw_1225 = {
#include "stg13/terrain/draw_1225.inc"
};
static NJS_CNK_OBJECT draw_1224 = {
#include "stg13/terrain/draw_1224.inc"
};
static NJS_CNK_OBJECT draw_1221 = {
#include "stg13/terrain/draw_1221.inc"
};
static NJS_CNK_OBJECT draw_1220 = {
#include "stg13/terrain/draw_1220.inc"
};
static NJS_CNK_OBJECT draw_1219 = {
#include "stg13/terrain/draw_1219.inc"
};
static NJS_CNK_OBJECT draw_1218 = {
#include "stg13/terrain/draw_1218.inc"
};
static NJS_CNK_OBJECT draw_1217 = {
#include "stg13/terrain/draw_1217.inc"
};
static NJS_CNK_OBJECT draw_1215 = {
#include "stg13/terrain/draw_1215.inc"
};
static NJS_CNK_OBJECT draw_1214 = {
#include "stg13/terrain/draw_1214.inc"
};
static NJS_CNK_OBJECT draw_1213 = {
#include "stg13/terrain/draw_1213.inc"
};
static NJS_CNK_OBJECT draw_1212 = {
#include "stg13/terrain/draw_1212.inc"
};
static NJS_CNK_OBJECT draw_1211 = {
#include "stg13/terrain/draw_1211.inc"
};
static NJS_CNK_OBJECT draw_1210 = {
#include "stg13/terrain/draw_1210.inc"
};
static NJS_CNK_OBJECT draw_1209 = {
#include "stg13/terrain/draw_1209.inc"
};
static NJS_CNK_OBJECT draw_1208 = {
#include "stg13/terrain/draw_1208.inc"
};
static NJS_CNK_OBJECT draw_1203 = {
#include "stg13/terrain/draw_1203.inc"
};
static NJS_CNK_OBJECT draw_1202 = {
#include "stg13/terrain/draw_1202.inc"
};
static NJS_CNK_OBJECT draw_1201 = {
#include "stg13/terrain/draw_1201.inc"
};
static NJS_CNK_OBJECT draw_1198 = {
#include "stg13/terrain/draw_1198.inc"
};
static NJS_CNK_OBJECT draw_1197 = {
#include "stg13/terrain/draw_1197.inc"
};
static NJS_CNK_OBJECT draw_1196 = {
#include "stg13/terrain/draw_1196.inc"
};
static NJS_CNK_OBJECT draw_1195 = {
#include "stg13/terrain/draw_1195.inc"
};
static NJS_CNK_OBJECT draw_1189 = {
#include "stg13/terrain/draw_1189.inc"
};
static NJS_CNK_OBJECT draw_1188 = {
#include "stg13/terrain/draw_1188.inc"
};
static NJS_CNK_OBJECT draw_1187 = {
#include "stg13/terrain/draw_1187.inc"
};
static NJS_CNK_OBJECT draw_1186 = {
#include "stg13/terrain/draw_1186.inc"
};
static NJS_CNK_OBJECT draw_1185 = {
#include "stg13/terrain/draw_1185.inc"
};
static NJS_CNK_OBJECT draw_1184 = {
#include "stg13/terrain/draw_1184.inc"
};
static NJS_CNK_OBJECT draw_1183 = {
#include "stg13/terrain/draw_1183.inc"
};
static NJS_CNK_OBJECT draw_1182 = {
#include "stg13/terrain/draw_1182.inc"
};
static NJS_CNK_OBJECT draw_1181 = {
#include "stg13/terrain/draw_1181.inc"
};
static NJS_CNK_OBJECT draw_1180 = {
#include "stg13/terrain/draw_1180.inc"
};
static NJS_CNK_OBJECT draw_1178 = {
#include "stg13/terrain/draw_1178.inc"
};
static NJS_CNK_OBJECT draw_1177 = {
#include "stg13/terrain/draw_1177.inc"
};
static NJS_CNK_OBJECT draw_1176 = {
#include "stg13/terrain/draw_1176.inc"
};
static NJS_CNK_OBJECT draw_1175 = {
#include "stg13/terrain/draw_1175.inc"
};
static NJS_CNK_OBJECT draw_1174 = {
#include "stg13/terrain/draw_1174.inc"
};
static NJS_CNK_OBJECT draw_1173 = {
#include "stg13/terrain/draw_1173.inc"
};
static NJS_CNK_OBJECT draw_1172 = {
#include "stg13/terrain/draw_1172.inc"
};
static NJS_CNK_OBJECT draw_1170 = {
#include "stg13/terrain/draw_1170.inc"
};
static NJS_CNK_OBJECT draw_1168 = {
#include "stg13/terrain/draw_1168.inc"
};
static NJS_CNK_OBJECT draw_1165 = {
#include "stg13/terrain/draw_1165.inc"
};
static NJS_CNK_OBJECT draw_1164 = {
#include "stg13/terrain/draw_1164.inc"
};
static NJS_CNK_OBJECT draw_1163 = {
#include "stg13/terrain/draw_1163.inc"
};
static NJS_CNK_OBJECT draw_1162 = {
#include "stg13/terrain/draw_1162.inc"
};
static NJS_CNK_OBJECT draw_1161 = {
#include "stg13/terrain/draw_1161.inc"
};
static NJS_CNK_OBJECT draw_1160 = {
#include "stg13/terrain/draw_1160.inc"
};
static NJS_CNK_OBJECT draw_1158 = {
#include "stg13/terrain/draw_1158.inc"
};
static NJS_CNK_OBJECT draw_1157 = {
#include "stg13/terrain/draw_1157.inc"
};
static NJS_CNK_OBJECT draw_1156 = {
#include "stg13/terrain/draw_1156.inc"
};
static NJS_CNK_OBJECT draw_1155 = {
#include "stg13/terrain/draw_1155.inc"
};
static NJS_CNK_OBJECT draw_1154 = {
#include "stg13/terrain/draw_1154.inc"
};
static NJS_CNK_OBJECT draw_1153 = {
#include "stg13/terrain/draw_1153.inc"
};
static NJS_CNK_OBJECT draw_1152 = {
#include "stg13/terrain/draw_1152.inc"
};
static NJS_CNK_OBJECT draw_1151 = {
#include "stg13/terrain/draw_1151.inc"
};
static NJS_CNK_OBJECT draw_1150 = {
#include "stg13/terrain/draw_1150.inc"
};
static NJS_CNK_OBJECT draw_1149 = {
#include "stg13/terrain/draw_1149.inc"
};
static NJS_CNK_OBJECT draw_1148 = {
#include "stg13/terrain/draw_1148.inc"
};
static NJS_CNK_OBJECT draw_1147 = {
#include "stg13/terrain/draw_1147.inc"
};
static NJS_CNK_OBJECT draw_1146 = {
#include "stg13/terrain/draw_1146.inc"
};
static NJS_CNK_OBJECT draw_1145 = {
#include "stg13/terrain/draw_1145.inc"
};
static NJS_CNK_OBJECT draw_1144 = {
#include "stg13/terrain/draw_1144.inc"
};
static NJS_CNK_OBJECT draw_1143 = {
#include "stg13/terrain/draw_1143.inc"
};
static NJS_CNK_OBJECT draw_1142 = {
#include "stg13/terrain/draw_1142.inc"
};
static NJS_CNK_OBJECT draw_1141 = {
#include "stg13/terrain/draw_1141.inc"
};
static NJS_CNK_OBJECT draw_1140 = {
#include "stg13/terrain/draw_1140.inc"
};
static NJS_CNK_OBJECT draw_1139 = {
#include "stg13/terrain/draw_1139.inc"
};
static NJS_CNK_OBJECT draw_1138 = {
#include "stg13/terrain/draw_1138.inc"
};
static NJS_CNK_OBJECT draw_1137 = {
#include "stg13/terrain/draw_1137.inc"
};
static NJS_CNK_OBJECT draw_1136 = {
#include "stg13/terrain/draw_1136.inc"
};
static NJS_CNK_OBJECT draw_1135 = {
#include "stg13/terrain/draw_1135.inc"
};
static NJS_CNK_OBJECT draw_1134 = {
#include "stg13/terrain/draw_1134.inc"
};
static NJS_CNK_OBJECT draw_1133 = {
#include "stg13/terrain/draw_1133.inc"
};
static NJS_CNK_OBJECT draw_1132 = {
#include "stg13/terrain/draw_1132.inc"
};
static NJS_CNK_OBJECT draw_1131 = {
#include "stg13/terrain/draw_1131.inc"
};
static NJS_CNK_OBJECT draw_1130 = {
#include "stg13/terrain/draw_1130.inc"
};
static NJS_CNK_OBJECT draw_1129 = {
#include "stg13/terrain/draw_1129.inc"
};
static NJS_CNK_OBJECT draw_1128 = {
#include "stg13/terrain/draw_1128.inc"
};
static NJS_CNK_OBJECT draw_1127 = {
#include "stg13/terrain/draw_1127.inc"
};
static NJS_CNK_OBJECT draw_1126 = {
#include "stg13/terrain/draw_1126.inc"
};
static NJS_CNK_OBJECT draw_1125 = {
#include "stg13/terrain/draw_1125.inc"
};
static NJS_CNK_OBJECT draw_1124 = {
#include "stg13/terrain/draw_1124.inc"
};
static NJS_CNK_OBJECT draw_1123 = {
#include "stg13/terrain/draw_1123.inc"
};
static NJS_CNK_OBJECT draw_1122 = {
#include "stg13/terrain/draw_1122.inc"
};
static NJS_CNK_OBJECT draw_1121 = {
#include "stg13/terrain/draw_1121.inc"
};
static NJS_CNK_OBJECT draw_1120 = {
#include "stg13/terrain/draw_1120.inc"
};
static NJS_CNK_OBJECT draw_1119 = {
#include "stg13/terrain/draw_1119.inc"
};
static NJS_CNK_OBJECT draw_1118 = {
#include "stg13/terrain/draw_1118.inc"
};
static NJS_CNK_OBJECT draw_1117 = {
#include "stg13/terrain/draw_1117.inc"
};
static NJS_CNK_OBJECT draw_1116 = {
#include "stg13/terrain/draw_1116.inc"
};
static NJS_CNK_OBJECT draw_1115 = {
#include "stg13/terrain/draw_1115.inc"
};
static NJS_CNK_OBJECT draw_1114 = {
#include "stg13/terrain/draw_1114.inc"
};
static NJS_CNK_OBJECT draw_1113 = {
#include "stg13/terrain/draw_1113.inc"
};
static NJS_CNK_OBJECT draw_1112 = {
#include "stg13/terrain/draw_1112.inc"
};
static NJS_CNK_OBJECT draw_1111 = {
#include "stg13/terrain/draw_1111.inc"
};
static NJS_CNK_OBJECT draw_1110 = {
#include "stg13/terrain/draw_1110.inc"
};
static NJS_CNK_OBJECT draw_1109 = {
#include "stg13/terrain/draw_1109.inc"
};
static NJS_CNK_OBJECT draw_1108 = {
#include "stg13/terrain/draw_1108.inc"
};
static NJS_CNK_OBJECT draw_1107 = {
#include "stg13/terrain/draw_1107.inc"
};
static NJS_CNK_OBJECT draw_1106 = {
#include "stg13/terrain/draw_1106.inc"
};
static NJS_CNK_OBJECT draw_1105 = {
#include "stg13/terrain/draw_1105.inc"
};
static NJS_CNK_OBJECT draw_1104 = {
#include "stg13/terrain/draw_1104.inc"
};
static NJS_CNK_OBJECT draw_1103 = {
#include "stg13/terrain/draw_1103.inc"
};
static NJS_CNK_OBJECT draw_1102 = {
#include "stg13/terrain/draw_1102.inc"
};
static NJS_CNK_OBJECT draw_1101 = {
#include "stg13/terrain/draw_1101.inc"
};
static NJS_CNK_OBJECT draw_1100 = {
#include "stg13/terrain/draw_1100.inc"
};
static NJS_CNK_OBJECT draw_1099 = {
#include "stg13/terrain/draw_1099.inc"
};
static NJS_CNK_OBJECT draw_1098 = {
#include "stg13/terrain/draw_1098.inc"
};
static NJS_CNK_OBJECT draw_1097 = {
#include "stg13/terrain/draw_1097.inc"
};
static NJS_CNK_OBJECT draw_1096 = {
#include "stg13/terrain/draw_1096.inc"
};
static NJS_CNK_OBJECT draw_1095 = {
#include "stg13/terrain/draw_1095.inc"
};
static NJS_CNK_OBJECT draw_1094 = {
#include "stg13/terrain/draw_1094.inc"
};
static NJS_CNK_OBJECT draw_1093 = {
#include "stg13/terrain/draw_1093.inc"
};
static NJS_CNK_OBJECT draw_1092 = {
#include "stg13/terrain/draw_1092.inc"
};
static NJS_CNK_OBJECT draw_1091 = {
#include "stg13/terrain/draw_1091.inc"
};
static NJS_CNK_OBJECT draw_1090 = {
#include "stg13/terrain/draw_1090.inc"
};
static NJS_CNK_OBJECT draw_1089 = {
#include "stg13/terrain/draw_1089.inc"
};
static NJS_CNK_OBJECT draw_1088 = {
#include "stg13/terrain/draw_1088.inc"
};
static NJS_CNK_OBJECT draw_1087 = {
#include "stg13/terrain/draw_1087.inc"
};
static NJS_CNK_OBJECT draw_1086 = {
#include "stg13/terrain/draw_1086.inc"
};
static NJS_CNK_OBJECT draw_1085 = {
#include "stg13/terrain/draw_1085.inc"
};
static NJS_CNK_OBJECT draw_1084 = {
#include "stg13/terrain/draw_1084.inc"
};
static NJS_CNK_OBJECT draw_1083 = {
#include "stg13/terrain/draw_1083.inc"
};
static NJS_CNK_OBJECT draw_1082 = {
#include "stg13/terrain/draw_1082.inc"
};
static NJS_CNK_OBJECT draw_1081 = {
#include "stg13/terrain/draw_1081.inc"
};
static NJS_CNK_OBJECT draw_1080 = {
#include "stg13/terrain/draw_1080.inc"
};
static NJS_CNK_OBJECT draw_1079 = {
#include "stg13/terrain/draw_1079.inc"
};
static NJS_CNK_OBJECT draw_1078 = {
#include "stg13/terrain/draw_1078.inc"
};
static NJS_CNK_OBJECT draw_1077 = {
#include "stg13/terrain/draw_1077.inc"
};
static NJS_CNK_OBJECT draw_1076 = {
#include "stg13/terrain/draw_1076.inc"
};
static NJS_CNK_OBJECT draw_1075 = {
#include "stg13/terrain/draw_1075.inc"
};
static NJS_CNK_OBJECT draw_1074 = {
#include "stg13/terrain/draw_1074.inc"
};
static NJS_CNK_OBJECT draw_1073 = {
#include "stg13/terrain/draw_1073.inc"
};
static NJS_CNK_OBJECT draw_1072 = {
#include "stg13/terrain/draw_1072.inc"
};
static NJS_CNK_OBJECT draw_1071 = {
#include "stg13/terrain/draw_1071.inc"
};
static NJS_CNK_OBJECT draw_1070 = {
#include "stg13/terrain/draw_1070.inc"
};
static NJS_CNK_OBJECT draw_1069 = {
#include "stg13/terrain/draw_1069.inc"
};
static NJS_CNK_OBJECT draw_1068 = {
#include "stg13/terrain/draw_1068.inc"
};
static NJS_CNK_OBJECT draw_1067 = {
#include "stg13/terrain/draw_1067.inc"
};
static NJS_CNK_OBJECT draw_1066 = {
#include "stg13/terrain/draw_1066.inc"
};
static NJS_CNK_OBJECT draw_1065 = {
#include "stg13/terrain/draw_1065.inc"
};
static NJS_CNK_OBJECT draw_1064 = {
#include "stg13/terrain/draw_1064.inc"
};
static NJS_CNK_OBJECT draw_1063 = {
#include "stg13/terrain/draw_1063.inc"
};
static NJS_CNK_OBJECT draw_1062 = {
#include "stg13/terrain/draw_1062.inc"
};
static NJS_CNK_OBJECT draw_1061 = {
#include "stg13/terrain/draw_1061.inc"
};
static NJS_CNK_OBJECT draw_1060 = {
#include "stg13/terrain/draw_1060.inc"
};
static NJS_CNK_OBJECT draw_1059 = {
#include "stg13/terrain/draw_1059.inc"
};
static NJS_CNK_OBJECT draw_1058 = {
#include "stg13/terrain/draw_1058.inc"
};
static NJS_CNK_OBJECT draw_1057 = {
#include "stg13/terrain/draw_1057.inc"
};
static NJS_CNK_OBJECT draw_1056 = {
#include "stg13/terrain/draw_1056.inc"
};
static NJS_CNK_OBJECT draw_1055 = {
#include "stg13/terrain/draw_1055.inc"
};
static NJS_CNK_OBJECT draw_1054 = {
#include "stg13/terrain/draw_1054.inc"
};
static NJS_CNK_OBJECT draw_1053 = {
#include "stg13/terrain/draw_1053.inc"
};
static NJS_CNK_OBJECT draw_1052 = {
#include "stg13/terrain/draw_1052.inc"
};
static NJS_CNK_OBJECT draw_1051 = {
#include "stg13/terrain/draw_1051.inc"
};
static NJS_CNK_OBJECT draw_1050 = {
#include "stg13/terrain/draw_1050.inc"
};
static NJS_CNK_OBJECT draw_1049 = {
#include "stg13/terrain/draw_1049.inc"
};
static NJS_CNK_OBJECT draw_1047 = {
#include "stg13/terrain/draw_1047.inc"
};
static NJS_CNK_OBJECT draw_1046 = {
#include "stg13/terrain/draw_1046.inc"
};
static NJS_CNK_OBJECT draw_1045 = {
#include "stg13/terrain/draw_1045.inc"
};
static NJS_CNK_OBJECT draw_1044 = {
#include "stg13/terrain/draw_1044.inc"
};
static NJS_CNK_OBJECT draw_1043 = {
#include "stg13/terrain/draw_1043.inc"
};
static NJS_CNK_OBJECT draw_1042 = {
#include "stg13/terrain/draw_1042.inc"
};
static NJS_CNK_OBJECT draw_1041 = {
#include "stg13/terrain/draw_1041.inc"
};
static NJS_CNK_OBJECT draw_1040 = {
#include "stg13/terrain/draw_1040.inc"
};
static NJS_CNK_OBJECT draw_1039 = {
#include "stg13/terrain/draw_1039.inc"
};
static NJS_CNK_OBJECT draw_1038 = {
#include "stg13/terrain/draw_1038.inc"
};
static NJS_CNK_OBJECT draw_1037 = {
#include "stg13/terrain/draw_1037.inc"
};
static NJS_CNK_OBJECT draw_1036 = {
#include "stg13/terrain/draw_1036.inc"
};
static NJS_CNK_OBJECT draw_1035 = {
#include "stg13/terrain/draw_1035.inc"
};
static NJS_CNK_OBJECT draw_1034 = {
#include "stg13/terrain/draw_1034.inc"
};
static NJS_CNK_OBJECT draw_1033 = {
#include "stg13/terrain/draw_1033.inc"
};
static NJS_CNK_OBJECT draw_1032 = {
#include "stg13/terrain/draw_1032.inc"
};
static NJS_CNK_OBJECT draw_1031 = {
#include "stg13/terrain/draw_1031.inc"
};
static NJS_CNK_OBJECT draw_1030 = {
#include "stg13/terrain/draw_1030.inc"
};
static NJS_CNK_OBJECT draw_1029 = {
#include "stg13/terrain/draw_1029.inc"
};
static NJS_CNK_OBJECT draw_1028 = {
#include "stg13/terrain/draw_1028.inc"
};
static NJS_CNK_OBJECT draw_1027 = {
#include "stg13/terrain/draw_1027.inc"
};
static NJS_CNK_OBJECT draw_1026 = {
#include "stg13/terrain/draw_1026.inc"
};
static NJS_CNK_OBJECT draw_1025 = {
#include "stg13/terrain/draw_1025.inc"
};
static NJS_CNK_OBJECT draw_1024 = {
#include "stg13/terrain/draw_1024.inc"
};
static NJS_CNK_OBJECT draw_1023 = {
#include "stg13/terrain/draw_1023.inc"
};
static NJS_CNK_OBJECT draw_1022 = {
#include "stg13/terrain/draw_1022.inc"
};
static NJS_CNK_OBJECT draw_1021 = {
#include "stg13/terrain/draw_1021.inc"
};
static NJS_CNK_OBJECT draw_1020 = {
#include "stg13/terrain/draw_1020.inc"
};
static NJS_CNK_OBJECT draw_1019 = {
#include "stg13/terrain/draw_1019.inc"
};
static NJS_CNK_OBJECT draw_1018 = {
#include "stg13/terrain/draw_1018.inc"
};
static NJS_CNK_OBJECT draw_1017 = {
#include "stg13/terrain/draw_1017.inc"
};
static NJS_CNK_OBJECT draw_1014 = {
#include "stg13/terrain/draw_1014.inc"
};
static NJS_CNK_OBJECT draw_1013 = {
#include "stg13/terrain/draw_1013.inc"
};
static NJS_CNK_OBJECT draw_1012 = {
#include "stg13/terrain/draw_1012.inc"
};
static NJS_CNK_OBJECT draw_1011 = {
#include "stg13/terrain/draw_1011.inc"
};
static NJS_CNK_OBJECT draw_1010 = {
#include "stg13/terrain/draw_1010.inc"
};
static NJS_CNK_OBJECT draw_1009 = {
#include "stg13/terrain/draw_1009.inc"
};
static NJS_CNK_OBJECT draw_1008 = {
#include "stg13/terrain/draw_1008.inc"
};
static NJS_CNK_OBJECT draw_1007 = {
#include "stg13/terrain/draw_1007.inc"
};
static NJS_CNK_OBJECT draw_1006 = {
#include "stg13/terrain/draw_1006.inc"
};
static NJS_CNK_OBJECT draw_1005 = {
#include "stg13/terrain/draw_1005.inc"
};
static NJS_CNK_OBJECT draw_1004 = {
#include "stg13/terrain/draw_1004.inc"
};
static NJS_CNK_OBJECT draw_1002 = {
#include "stg13/terrain/draw_1002.inc"
};
static NJS_CNK_OBJECT draw_1001 = {
#include "stg13/terrain/draw_1001.inc"
};
static NJS_CNK_OBJECT draw_1000 = {
#include "stg13/terrain/draw_1000.inc"
};
static NJS_CNK_OBJECT draw_999 = {
#include "stg13/terrain/draw_999.inc"
};
static NJS_CNK_OBJECT draw_998 = {
#include "stg13/terrain/draw_998.inc"
};
static NJS_CNK_OBJECT draw_997 = {
#include "stg13/terrain/draw_997.inc"
};
static NJS_CNK_OBJECT draw_996 = {
#include "stg13/terrain/draw_996.inc"
};
static NJS_CNK_OBJECT draw_995 = {
#include "stg13/terrain/draw_995.inc"
};
static NJS_CNK_OBJECT draw_994 = {
#include "stg13/terrain/draw_994.inc"
};
static NJS_CNK_OBJECT draw_993 = {
#include "stg13/terrain/draw_993.inc"
};
static NJS_CNK_OBJECT draw_992 = {
#include "stg13/terrain/draw_992.inc"
};
static NJS_CNK_OBJECT draw_991 = {
#include "stg13/terrain/draw_991.inc"
};
static NJS_CNK_OBJECT draw_990 = {
#include "stg13/terrain/draw_990.inc"
};
static NJS_CNK_OBJECT draw_989 = {
#include "stg13/terrain/draw_989.inc"
};
static NJS_CNK_OBJECT draw_988 = {
#include "stg13/terrain/draw_988.inc"
};
static NJS_CNK_OBJECT draw_987 = {
#include "stg13/terrain/draw_987.inc"
};
static NJS_CNK_OBJECT draw_986 = {
#include "stg13/terrain/draw_986.inc"
};
static NJS_CNK_OBJECT draw_985 = {
#include "stg13/terrain/draw_985.inc"
};
static NJS_CNK_OBJECT draw_984 = {
#include "stg13/terrain/draw_984.inc"
};
static NJS_CNK_OBJECT draw_983 = {
#include "stg13/terrain/draw_983.inc"
};
static NJS_CNK_OBJECT draw_981 = {
#include "stg13/terrain/draw_981.inc"
};
static NJS_CNK_OBJECT draw_980 = {
#include "stg13/terrain/draw_980.inc"
};
static NJS_CNK_OBJECT draw_979 = {
#include "stg13/terrain/draw_979.inc"
};
static NJS_CNK_OBJECT draw_977 = {
#include "stg13/terrain/draw_977.inc"
};
static NJS_CNK_OBJECT draw_976 = {
#include "stg13/terrain/draw_976.inc"
};
static NJS_CNK_OBJECT draw_975 = {
#include "stg13/terrain/draw_975.inc"
};
static NJS_CNK_OBJECT draw_973 = {
#include "stg13/terrain/draw_973.inc"
};
static NJS_CNK_OBJECT draw_972 = {
#include "stg13/terrain/draw_972.inc"
};
static NJS_CNK_OBJECT draw_971 = {
#include "stg13/terrain/draw_971.inc"
};
static NJS_CNK_OBJECT draw_969 = {
#include "stg13/terrain/draw_969.inc"
};
static NJS_CNK_OBJECT draw_968 = {
#include "stg13/terrain/draw_968.inc"
};
static NJS_CNK_OBJECT draw_967 = {
#include "stg13/terrain/draw_967.inc"
};
static NJS_CNK_OBJECT draw_966 = {
#include "stg13/terrain/draw_966.inc"
};
static NJS_CNK_OBJECT draw_965 = {
#include "stg13/terrain/draw_965.inc"
};
static NJS_CNK_OBJECT draw_964 = {
#include "stg13/terrain/draw_964.inc"
};
static NJS_CNK_OBJECT draw_963 = {
#include "stg13/terrain/draw_963.inc"
};
static NJS_CNK_OBJECT draw_962 = {
#include "stg13/terrain/draw_962.inc"
};
static NJS_CNK_OBJECT draw_961 = {
#include "stg13/terrain/draw_961.inc"
};
static NJS_CNK_OBJECT draw_960 = {
#include "stg13/terrain/draw_960.inc"
};
static NJS_CNK_OBJECT draw_959 = {
#include "stg13/terrain/draw_959.inc"
};
static NJS_CNK_OBJECT draw_958 = {
#include "stg13/terrain/draw_958.inc"
};
static NJS_CNK_OBJECT draw_957 = {
#include "stg13/terrain/draw_957.inc"
};
static NJS_CNK_OBJECT draw_956 = {
#include "stg13/terrain/draw_956.inc"
};
static NJS_CNK_OBJECT draw_955 = {
#include "stg13/terrain/draw_955.inc"
};
static NJS_CNK_OBJECT draw_954 = {
#include "stg13/terrain/draw_954.inc"
};
static NJS_CNK_OBJECT draw_953 = {
#include "stg13/terrain/draw_953.inc"
};
static NJS_CNK_OBJECT draw_952 = {
#include "stg13/terrain/draw_952.inc"
};
static NJS_CNK_OBJECT draw_951 = {
#include "stg13/terrain/draw_951.inc"
};
static NJS_CNK_OBJECT draw_950 = {
#include "stg13/terrain/draw_950.inc"
};
static NJS_CNK_OBJECT draw_949 = {
#include "stg13/terrain/draw_949.inc"
};
static NJS_CNK_OBJECT draw_948 = {
#include "stg13/terrain/draw_948.inc"
};
static NJS_CNK_OBJECT draw_947 = {
#include "stg13/terrain/draw_947.inc"
};
static NJS_CNK_OBJECT draw_946 = {
#include "stg13/terrain/draw_946.inc"
};
static NJS_CNK_OBJECT draw_945 = {
#include "stg13/terrain/draw_945.inc"
};
static NJS_CNK_OBJECT draw_944 = {
#include "stg13/terrain/draw_944.inc"
};
static NJS_CNK_OBJECT draw_943 = {
#include "stg13/terrain/draw_943.inc"
};
static NJS_CNK_OBJECT draw_942 = {
#include "stg13/terrain/draw_942.inc"
};
static NJS_CNK_OBJECT draw_941 = {
#include "stg13/terrain/draw_941.inc"
};
static NJS_CNK_OBJECT draw_940 = {
#include "stg13/terrain/draw_940.inc"
};
static NJS_CNK_OBJECT draw_939 = {
#include "stg13/terrain/draw_939.inc"
};
static NJS_CNK_OBJECT draw_938 = {
#include "stg13/terrain/draw_938.inc"
};
static NJS_CNK_OBJECT draw_937 = {
#include "stg13/terrain/draw_937.inc"
};
static NJS_CNK_OBJECT draw_936 = {
#include "stg13/terrain/draw_936.inc"
};
static NJS_CNK_OBJECT draw_935 = {
#include "stg13/terrain/draw_935.inc"
};
static NJS_CNK_OBJECT draw_934 = {
#include "stg13/terrain/draw_934.inc"
};
static NJS_CNK_OBJECT draw_933 = {
#include "stg13/terrain/draw_933.inc"
};
static NJS_CNK_OBJECT draw_932 = {
#include "stg13/terrain/draw_932.inc"
};
static NJS_CNK_OBJECT draw_931 = {
#include "stg13/terrain/draw_931.inc"
};
static NJS_CNK_OBJECT draw_930 = {
#include "stg13/terrain/draw_930.inc"
};
static NJS_CNK_OBJECT draw_929 = {
#include "stg13/terrain/draw_929.inc"
};
static NJS_CNK_OBJECT draw_928 = {
#include "stg13/terrain/draw_928.inc"
};
static NJS_CNK_OBJECT draw_927 = {
#include "stg13/terrain/draw_927.inc"
};
static NJS_CNK_OBJECT draw_926 = {
#include "stg13/terrain/draw_926.inc"
};
static NJS_CNK_OBJECT draw_925 = {
#include "stg13/terrain/draw_925.inc"
};
static NJS_CNK_OBJECT draw_924 = {
#include "stg13/terrain/draw_924.inc"
};
static NJS_CNK_OBJECT draw_923 = {
#include "stg13/terrain/draw_923.inc"
};
static NJS_CNK_OBJECT draw_922 = {
#include "stg13/terrain/draw_922.inc"
};
static NJS_CNK_OBJECT draw_921 = {
#include "stg13/terrain/draw_921.inc"
};
static NJS_CNK_OBJECT draw_920 = {
#include "stg13/terrain/draw_920.inc"
};
static NJS_CNK_OBJECT draw_919 = {
#include "stg13/terrain/draw_919.inc"
};
static NJS_CNK_OBJECT draw_918 = {
#include "stg13/terrain/draw_918.inc"
};
static NJS_CNK_OBJECT draw_917 = {
#include "stg13/terrain/draw_917.inc"
};
static NJS_CNK_OBJECT draw_916 = {
#include "stg13/terrain/draw_916.inc"
};
static NJS_CNK_OBJECT draw_911 = {
#include "stg13/terrain/draw_911.inc"
};
static NJS_CNK_OBJECT draw_910 = {
#include "stg13/terrain/draw_910.inc"
};
static NJS_CNK_OBJECT draw_908 = {
#include "stg13/terrain/draw_908.inc"
};
static NJS_CNK_OBJECT draw_905 = {
#include "stg13/terrain/draw_905.inc"
};
static NJS_CNK_OBJECT draw_904 = {
#include "stg13/terrain/draw_904.inc"
};
static NJS_CNK_OBJECT draw_903 = {
#include "stg13/terrain/draw_903.inc"
};
static NJS_CNK_OBJECT draw_902 = {
#include "stg13/terrain/draw_902.inc"
};
static NJS_CNK_OBJECT draw_901 = {
#include "stg13/terrain/draw_901.inc"
};
static NJS_CNK_OBJECT draw_900 = {
#include "stg13/terrain/draw_900.inc"
};
static NJS_CNK_OBJECT draw_893 = {
#include "stg13/terrain/draw_893.inc"
};
static NJS_CNK_OBJECT draw_891 = {
#include "stg13/terrain/draw_891.inc"
};
static NJS_CNK_OBJECT draw_885 = {
#include "stg13/terrain/draw_885.inc"
};
static NJS_CNK_OBJECT draw_878 = {
#include "stg13/terrain/draw_878.inc"
};
static NJS_CNK_OBJECT draw_874 = {
#include "stg13/terrain/draw_874.inc"
};
static NJS_CNK_OBJECT draw_873 = {
#include "stg13/terrain/draw_873.inc"
};
static NJS_CNK_OBJECT draw_872 = {
#include "stg13/terrain/draw_872.inc"
};
static NJS_CNK_OBJECT draw_871 = {
#include "stg13/terrain/draw_871.inc"
};
static NJS_CNK_OBJECT draw_870 = {
#include "stg13/terrain/draw_870.inc"
};
static NJS_CNK_OBJECT draw_869 = {
#include "stg13/terrain/draw_869.inc"
};
static NJS_CNK_OBJECT draw_867 = {
#include "stg13/terrain/draw_867.inc"
};
static NJS_CNK_OBJECT draw_865 = {
#include "stg13/terrain/draw_865.inc"
};
static NJS_CNK_OBJECT draw_864 = {
#include "stg13/terrain/draw_864.inc"
};
static NJS_CNK_OBJECT draw_861 = {
#include "stg13/terrain/draw_861.inc"
};
static NJS_CNK_OBJECT draw_853 = {
#include "stg13/terrain/draw_853.inc"
};
static NJS_CNK_OBJECT draw_852 = {
#include "stg13/terrain/draw_852.inc"
};
static NJS_CNK_OBJECT draw_850 = {
#include "stg13/terrain/draw_850.inc"
};
static NJS_CNK_OBJECT draw_849 = {
#include "stg13/terrain/draw_849.inc"
};
static NJS_CNK_OBJECT draw_848 = {
#include "stg13/terrain/draw_848.inc"
};
static NJS_CNK_OBJECT draw_847 = {
#include "stg13/terrain/draw_847.inc"
};
static NJS_CNK_OBJECT draw_846 = {
#include "stg13/terrain/draw_846.inc"
};
static NJS_CNK_OBJECT draw_845 = {
#include "stg13/terrain/draw_845.inc"
};
static NJS_CNK_OBJECT draw_844 = {
#include "stg13/terrain/draw_844.inc"
};
static NJS_CNK_OBJECT draw_843 = {
#include "stg13/terrain/draw_843.inc"
};
static NJS_CNK_OBJECT draw_842 = {
#include "stg13/terrain/draw_842.inc"
};
static NJS_CNK_OBJECT draw_841 = {
#include "stg13/terrain/draw_841.inc"
};
static NJS_CNK_OBJECT draw_840 = {
#include "stg13/terrain/draw_840.inc"
};
static NJS_CNK_OBJECT draw_839 = {
#include "stg13/terrain/draw_839.inc"
};
static NJS_CNK_OBJECT draw_837 = {
#include "stg13/terrain/draw_837.inc"
};
static NJS_CNK_OBJECT draw_836 = {
#include "stg13/terrain/draw_836.inc"
};
static NJS_CNK_OBJECT draw_834 = {
#include "stg13/terrain/draw_834.inc"
};
static NJS_CNK_OBJECT draw_833 = {
#include "stg13/terrain/draw_833.inc"
};
static NJS_CNK_OBJECT draw_830 = {
#include "stg13/terrain/draw_830.inc"
};
static NJS_CNK_OBJECT draw_828 = {
#include "stg13/terrain/draw_828.inc"
};
static NJS_CNK_OBJECT draw_826 = {
#include "stg13/terrain/draw_826.inc"
};
static NJS_CNK_OBJECT draw_825 = {
#include "stg13/terrain/draw_825.inc"
};
static NJS_CNK_OBJECT draw_824 = {
#include "stg13/terrain/draw_824.inc"
};
static NJS_CNK_OBJECT draw_823 = {
#include "stg13/terrain/draw_823.inc"
};
static NJS_CNK_OBJECT draw_822 = {
#include "stg13/terrain/draw_822.inc"
};
static NJS_CNK_OBJECT draw_821 = {
#include "stg13/terrain/draw_821.inc"
};
static NJS_CNK_OBJECT draw_820 = {
#include "stg13/terrain/draw_820.inc"
};
static NJS_CNK_OBJECT draw_819 = {
#include "stg13/terrain/draw_819.inc"
};
static NJS_CNK_OBJECT draw_816 = {
#include "stg13/terrain/draw_816.inc"
};
static NJS_CNK_OBJECT draw_815 = {
#include "stg13/terrain/draw_815.inc"
};
static NJS_CNK_OBJECT draw_814 = {
#include "stg13/terrain/draw_814.inc"
};
static NJS_CNK_OBJECT draw_811 = {
#include "stg13/terrain/draw_811.inc"
};
static NJS_CNK_OBJECT draw_810 = {
#include "stg13/terrain/draw_810.inc"
};
static NJS_CNK_OBJECT draw_809 = {
#include "stg13/terrain/draw_809.inc"
};
static NJS_CNK_OBJECT draw_808 = {
#include "stg13/terrain/draw_808.inc"
};
static NJS_CNK_OBJECT draw_807 = {
#include "stg13/terrain/draw_807.inc"
};
static NJS_CNK_OBJECT draw_806 = {
#include "stg13/terrain/draw_806.inc"
};
static NJS_CNK_OBJECT draw_805 = {
#include "stg13/terrain/draw_805.inc"
};
static NJS_CNK_OBJECT draw_803 = {
#include "stg13/terrain/draw_803.inc"
};
static NJS_CNK_OBJECT draw_802 = {
#include "stg13/terrain/draw_802.inc"
};
static NJS_CNK_OBJECT draw_801 = {
#include "stg13/terrain/draw_801.inc"
};
static NJS_CNK_OBJECT draw_800 = {
#include "stg13/terrain/draw_800.inc"
};
static NJS_CNK_OBJECT draw_799 = {
#include "stg13/terrain/draw_799.inc"
};
static NJS_CNK_OBJECT draw_798 = {
#include "stg13/terrain/draw_798.inc"
};
static NJS_CNK_OBJECT draw_797 = {
#include "stg13/terrain/draw_797.inc"
};
static NJS_CNK_OBJECT draw_796 = {
#include "stg13/terrain/draw_796.inc"
};
static NJS_CNK_OBJECT draw_795 = {
#include "stg13/terrain/draw_795.inc"
};
static NJS_CNK_OBJECT draw_794 = {
#include "stg13/terrain/draw_794.inc"
};
static NJS_CNK_OBJECT draw_793 = {
#include "stg13/terrain/draw_793.inc"
};
static NJS_CNK_OBJECT draw_792 = {
#include "stg13/terrain/draw_792.inc"
};
static NJS_CNK_OBJECT draw_791 = {
#include "stg13/terrain/draw_791.inc"
};
static NJS_CNK_OBJECT draw_790 = {
#include "stg13/terrain/draw_790.inc"
};
static NJS_CNK_OBJECT draw_789 = {
#include "stg13/terrain/draw_789.inc"
};
static NJS_CNK_OBJECT draw_788 = {
#include "stg13/terrain/draw_788.inc"
};
static NJS_CNK_OBJECT draw_787 = {
#include "stg13/terrain/draw_787.inc"
};
static NJS_CNK_OBJECT draw_786 = {
#include "stg13/terrain/draw_786.inc"
};
static NJS_CNK_OBJECT draw_785 = {
#include "stg13/terrain/draw_785.inc"
};
static NJS_CNK_OBJECT draw_784 = {
#include "stg13/terrain/draw_784.inc"
};
static NJS_CNK_OBJECT draw_783 = {
#include "stg13/terrain/draw_783.inc"
};
static NJS_CNK_OBJECT draw_782 = {
#include "stg13/terrain/draw_782.inc"
};
static NJS_CNK_OBJECT draw_781 = {
#include "stg13/terrain/draw_781.inc"
};
static NJS_CNK_OBJECT draw_780 = {
#include "stg13/terrain/draw_780.inc"
};
static NJS_CNK_OBJECT draw_779 = {
#include "stg13/terrain/draw_779.inc"
};
static NJS_CNK_OBJECT draw_778 = {
#include "stg13/terrain/draw_778.inc"
};
static NJS_CNK_OBJECT draw_777 = {
#include "stg13/terrain/draw_777.inc"
};
static NJS_CNK_OBJECT draw_776 = {
#include "stg13/terrain/draw_776.inc"
};
static NJS_CNK_OBJECT draw_775 = {
#include "stg13/terrain/draw_775.inc"
};
static NJS_CNK_OBJECT draw_774 = {
#include "stg13/terrain/draw_774.inc"
};
static NJS_CNK_OBJECT draw_773 = {
#include "stg13/terrain/draw_773.inc"
};
static NJS_CNK_OBJECT draw_772 = {
#include "stg13/terrain/draw_772.inc"
};
static NJS_CNK_OBJECT draw_771 = {
#include "stg13/terrain/draw_771.inc"
};
static NJS_CNK_OBJECT draw_770 = {
#include "stg13/terrain/draw_770.inc"
};
static NJS_CNK_OBJECT draw_769 = {
#include "stg13/terrain/draw_769.inc"
};
static NJS_CNK_OBJECT draw_768 = {
#include "stg13/terrain/draw_768.inc"
};
static NJS_CNK_OBJECT draw_767 = {
#include "stg13/terrain/draw_767.inc"
};
static NJS_CNK_OBJECT draw_766 = {
#include "stg13/terrain/draw_766.inc"
};
static NJS_CNK_OBJECT draw_765 = {
#include "stg13/terrain/draw_765.inc"
};
static NJS_CNK_OBJECT draw_764 = {
#include "stg13/terrain/draw_764.inc"
};
static NJS_CNK_OBJECT draw_763 = {
#include "stg13/terrain/draw_763.inc"
};
static NJS_CNK_OBJECT draw_762 = {
#include "stg13/terrain/draw_762.inc"
};
static NJS_CNK_OBJECT draw_761 = {
#include "stg13/terrain/draw_761.inc"
};
static NJS_CNK_OBJECT draw_760 = {
#include "stg13/terrain/draw_760.inc"
};
static NJS_CNK_OBJECT draw_759 = {
#include "stg13/terrain/draw_759.inc"
};
static NJS_CNK_OBJECT draw_758 = {
#include "stg13/terrain/draw_758.inc"
};
static NJS_CNK_OBJECT draw_757 = {
#include "stg13/terrain/draw_757.inc"
};
static NJS_CNK_OBJECT draw_756 = {
#include "stg13/terrain/draw_756.inc"
};
static NJS_CNK_OBJECT draw_755 = {
#include "stg13/terrain/draw_755.inc"
};
static NJS_CNK_OBJECT draw_754 = {
#include "stg13/terrain/draw_754.inc"
};
static NJS_CNK_OBJECT draw_753 = {
#include "stg13/terrain/draw_753.inc"
};
static NJS_CNK_OBJECT draw_752 = {
#include "stg13/terrain/draw_752.inc"
};
static NJS_CNK_OBJECT draw_751 = {
#include "stg13/terrain/draw_751.inc"
};
static NJS_CNK_OBJECT draw_750 = {
#include "stg13/terrain/draw_750.inc"
};
static NJS_CNK_OBJECT draw_749 = {
#include "stg13/terrain/draw_749.inc"
};
static NJS_CNK_OBJECT draw_748 = {
#include "stg13/terrain/draw_748.inc"
};
static NJS_CNK_OBJECT draw_747 = {
#include "stg13/terrain/draw_747.inc"
};
static NJS_CNK_OBJECT draw_746 = {
#include "stg13/terrain/draw_746.inc"
};
static NJS_CNK_OBJECT draw_745 = {
#include "stg13/terrain/draw_745.inc"
};
static NJS_CNK_OBJECT draw_744 = {
#include "stg13/terrain/draw_744.inc"
};
static NJS_CNK_OBJECT draw_743 = {
#include "stg13/terrain/draw_743.inc"
};
static NJS_CNK_OBJECT draw_742 = {
#include "stg13/terrain/draw_742.inc"
};
static NJS_CNK_OBJECT draw_741 = {
#include "stg13/terrain/draw_741.inc"
};
static NJS_CNK_OBJECT draw_740 = {
#include "stg13/terrain/draw_740.inc"
};
static NJS_CNK_OBJECT draw_739 = {
#include "stg13/terrain/draw_739.inc"
};
static NJS_CNK_OBJECT draw_738 = {
#include "stg13/terrain/draw_738.inc"
};
static NJS_CNK_OBJECT draw_737 = {
#include "stg13/terrain/draw_737.inc"
};
static NJS_CNK_OBJECT draw_736 = {
#include "stg13/terrain/draw_736.inc"
};
static NJS_CNK_OBJECT draw_735 = {
#include "stg13/terrain/draw_735.inc"
};
static NJS_CNK_OBJECT draw_734 = {
#include "stg13/terrain/draw_734.inc"
};
static NJS_CNK_OBJECT draw_733 = {
#include "stg13/terrain/draw_733.inc"
};
static NJS_CNK_OBJECT draw_732 = {
#include "stg13/terrain/draw_732.inc"
};
static NJS_CNK_OBJECT draw_731 = {
#include "stg13/terrain/draw_731.inc"
};
static NJS_CNK_OBJECT draw_730 = {
#include "stg13/terrain/draw_730.inc"
};
static NJS_CNK_OBJECT draw_729 = {
#include "stg13/terrain/draw_729.inc"
};
static NJS_CNK_OBJECT draw_728 = {
#include "stg13/terrain/draw_728.inc"
};
static NJS_CNK_OBJECT draw_727 = {
#include "stg13/terrain/draw_727.inc"
};
static NJS_CNK_OBJECT draw_726 = {
#include "stg13/terrain/draw_726.inc"
};
static NJS_CNK_OBJECT draw_725 = {
#include "stg13/terrain/draw_725.inc"
};
static NJS_CNK_OBJECT draw_724 = {
#include "stg13/terrain/draw_724.inc"
};
static NJS_CNK_OBJECT draw_723 = {
#include "stg13/terrain/draw_723.inc"
};
static NJS_CNK_OBJECT draw_722 = {
#include "stg13/terrain/draw_722.inc"
};
static NJS_CNK_OBJECT draw_721 = {
#include "stg13/terrain/draw_721.inc"
};
static NJS_CNK_OBJECT draw_720 = {
#include "stg13/terrain/draw_720.inc"
};
static NJS_CNK_OBJECT draw_719 = {
#include "stg13/terrain/draw_719.inc"
};
static NJS_CNK_OBJECT draw_718 = {
#include "stg13/terrain/draw_718.inc"
};
static NJS_CNK_OBJECT draw_717 = {
#include "stg13/terrain/draw_717.inc"
};
static NJS_CNK_OBJECT draw_716 = {
#include "stg13/terrain/draw_716.inc"
};
static NJS_CNK_OBJECT draw_715 = {
#include "stg13/terrain/draw_715.inc"
};
static NJS_CNK_OBJECT draw_714 = {
#include "stg13/terrain/draw_714.inc"
};
static NJS_CNK_OBJECT draw_713 = {
#include "stg13/terrain/draw_713.inc"
};
static NJS_CNK_OBJECT draw_712 = {
#include "stg13/terrain/draw_712.inc"
};
static NJS_CNK_OBJECT draw_711 = {
#include "stg13/terrain/draw_711.inc"
};
static NJS_CNK_OBJECT draw_710 = {
#include "stg13/terrain/draw_710.inc"
};
static NJS_CNK_OBJECT draw_709 = {
#include "stg13/terrain/draw_709.inc"
};
static NJS_CNK_OBJECT draw_708 = {
#include "stg13/terrain/draw_708.inc"
};
static NJS_CNK_OBJECT draw_707 = {
#include "stg13/terrain/draw_707.inc"
};
static NJS_CNK_OBJECT draw_706 = {
#include "stg13/terrain/draw_706.inc"
};
static NJS_CNK_OBJECT draw_705 = {
#include "stg13/terrain/draw_705.inc"
};
static NJS_CNK_OBJECT draw_704 = {
#include "stg13/terrain/draw_704.inc"
};
static NJS_CNK_OBJECT draw_703 = {
#include "stg13/terrain/draw_703.inc"
};
static NJS_CNK_OBJECT draw_702 = {
#include "stg13/terrain/draw_702.inc"
};
static NJS_CNK_OBJECT draw_701 = {
#include "stg13/terrain/draw_701.inc"
};
static NJS_CNK_OBJECT draw_700 = {
#include "stg13/terrain/draw_700.inc"
};
static NJS_CNK_OBJECT draw_699 = {
#include "stg13/terrain/draw_699.inc"
};
static NJS_CNK_OBJECT draw_698 = {
#include "stg13/terrain/draw_698.inc"
};
static NJS_CNK_OBJECT draw_697 = {
#include "stg13/terrain/draw_697.inc"
};
static NJS_CNK_OBJECT draw_696 = {
#include "stg13/terrain/draw_696.inc"
};
static NJS_CNK_OBJECT draw_695 = {
#include "stg13/terrain/draw_695.inc"
};
static NJS_CNK_OBJECT draw_694 = {
#include "stg13/terrain/draw_694.inc"
};
static NJS_CNK_OBJECT draw_693 = {
#include "stg13/terrain/draw_693.inc"
};
static NJS_CNK_OBJECT draw_692 = {
#include "stg13/terrain/draw_692.inc"
};
static NJS_CNK_OBJECT draw_691 = {
#include "stg13/terrain/draw_691.inc"
};
static NJS_CNK_OBJECT draw_690 = {
#include "stg13/terrain/draw_690.inc"
};
static NJS_CNK_OBJECT draw_689 = {
#include "stg13/terrain/draw_689.inc"
};
static NJS_CNK_OBJECT draw_688 = {
#include "stg13/terrain/draw_688.inc"
};
static NJS_CNK_OBJECT draw_687 = {
#include "stg13/terrain/draw_687.inc"
};
static NJS_CNK_OBJECT draw_686 = {
#include "stg13/terrain/draw_686.inc"
};
static NJS_CNK_OBJECT draw_685 = {
#include "stg13/terrain/draw_685.inc"
};
static NJS_CNK_OBJECT draw_684 = {
#include "stg13/terrain/draw_684.inc"
};
static NJS_CNK_OBJECT draw_683 = {
#include "stg13/terrain/draw_683.inc"
};
static NJS_CNK_OBJECT draw_682 = {
#include "stg13/terrain/draw_682.inc"
};
static NJS_CNK_OBJECT draw_681 = {
#include "stg13/terrain/draw_681.inc"
};
static NJS_CNK_OBJECT draw_680 = {
#include "stg13/terrain/draw_680.inc"
};
static NJS_CNK_OBJECT draw_679 = {
#include "stg13/terrain/draw_679.inc"
};
static NJS_CNK_OBJECT draw_678 = {
#include "stg13/terrain/draw_678.inc"
};
static NJS_CNK_OBJECT draw_677 = {
#include "stg13/terrain/draw_677.inc"
};
static NJS_CNK_OBJECT draw_676 = {
#include "stg13/terrain/draw_676.inc"
};
static NJS_CNK_OBJECT draw_675 = {
#include "stg13/terrain/draw_675.inc"
};
static NJS_CNK_OBJECT draw_674 = {
#include "stg13/terrain/draw_674.inc"
};
static NJS_CNK_OBJECT draw_673 = {
#include "stg13/terrain/draw_673.inc"
};
static NJS_CNK_OBJECT draw_672 = {
#include "stg13/terrain/draw_672.inc"
};
static NJS_CNK_OBJECT draw_671 = {
#include "stg13/terrain/draw_671.inc"
};
static NJS_CNK_OBJECT draw_670 = {
#include "stg13/terrain/draw_670.inc"
};
static NJS_CNK_OBJECT draw_669 = {
#include "stg13/terrain/draw_669.inc"
};
static NJS_CNK_OBJECT draw_668 = {
#include "stg13/terrain/draw_668.inc"
};
static NJS_CNK_OBJECT draw_667 = {
#include "stg13/terrain/draw_667.inc"
};
static NJS_CNK_OBJECT draw_666 = {
#include "stg13/terrain/draw_666.inc"
};
static NJS_CNK_OBJECT draw_665 = {
#include "stg13/terrain/draw_665.inc"
};
static NJS_CNK_OBJECT draw_664 = {
#include "stg13/terrain/draw_664.inc"
};
static NJS_CNK_OBJECT draw_663 = {
#include "stg13/terrain/draw_663.inc"
};
static NJS_CNK_OBJECT draw_662 = {
#include "stg13/terrain/draw_662.inc"
};
static NJS_CNK_OBJECT draw_661 = {
#include "stg13/terrain/draw_661.inc"
};
static NJS_CNK_OBJECT draw_660 = {
#include "stg13/terrain/draw_660.inc"
};
static NJS_CNK_OBJECT draw_659 = {
#include "stg13/terrain/draw_659.inc"
};
static NJS_CNK_OBJECT draw_658 = {
#include "stg13/terrain/draw_658.inc"
};
static NJS_CNK_OBJECT draw_657 = {
#include "stg13/terrain/draw_657.inc"
};
static NJS_CNK_OBJECT draw_656 = {
#include "stg13/terrain/draw_656.inc"
};
static NJS_CNK_OBJECT draw_655 = {
#include "stg13/terrain/draw_655.inc"
};
static NJS_CNK_OBJECT draw_654 = {
#include "stg13/terrain/draw_654.inc"
};
static NJS_CNK_OBJECT draw_652 = {
#include "stg13/terrain/draw_652.inc"
};
static NJS_CNK_OBJECT draw_651 = {
#include "stg13/terrain/draw_651.inc"
};
static NJS_CNK_OBJECT draw_648 = {
#include "stg13/terrain/draw_648.inc"
};
static NJS_CNK_OBJECT draw_647 = {
#include "stg13/terrain/draw_647.inc"
};
static NJS_CNK_OBJECT draw_646 = {
#include "stg13/terrain/draw_646.inc"
};
static NJS_CNK_OBJECT draw_645 = {
#include "stg13/terrain/draw_645.inc"
};
static NJS_CNK_OBJECT draw_644 = {
#include "stg13/terrain/draw_644.inc"
};
static NJS_CNK_OBJECT draw_643 = {
#include "stg13/terrain/draw_643.inc"
};
static NJS_CNK_OBJECT draw_642 = {
#include "stg13/terrain/draw_642.inc"
};
static NJS_CNK_OBJECT draw_641 = {
#include "stg13/terrain/draw_641.inc"
};
static NJS_CNK_OBJECT draw_640 = {
#include "stg13/terrain/draw_640.inc"
};
static NJS_CNK_OBJECT draw_639 = {
#include "stg13/terrain/draw_639.inc"
};
static NJS_CNK_OBJECT draw_638 = {
#include "stg13/terrain/draw_638.inc"
};
static NJS_CNK_OBJECT draw_637 = {
#include "stg13/terrain/draw_637.inc"
};
static NJS_CNK_OBJECT draw_636 = {
#include "stg13/terrain/draw_636.inc"
};
static NJS_CNK_OBJECT draw_635 = {
#include "stg13/terrain/draw_635.inc"
};
static NJS_CNK_OBJECT draw_634 = {
#include "stg13/terrain/draw_634.inc"
};
static NJS_CNK_OBJECT draw_633 = {
#include "stg13/terrain/draw_633.inc"
};
static NJS_CNK_OBJECT draw_632 = {
#include "stg13/terrain/draw_632.inc"
};
static NJS_CNK_OBJECT draw_631 = {
#include "stg13/terrain/draw_631.inc"
};
static NJS_CNK_OBJECT draw_630 = {
#include "stg13/terrain/draw_630.inc"
};
static NJS_CNK_OBJECT draw_629 = {
#include "stg13/terrain/draw_629.inc"
};
static NJS_CNK_OBJECT draw_628 = {
#include "stg13/terrain/draw_628.inc"
};
static NJS_CNK_OBJECT draw_627 = {
#include "stg13/terrain/draw_627.inc"
};
static NJS_CNK_OBJECT draw_626 = {
#include "stg13/terrain/draw_626.inc"
};
static NJS_CNK_OBJECT draw_625 = {
#include "stg13/terrain/draw_625.inc"
};
static NJS_CNK_OBJECT draw_624 = {
#include "stg13/terrain/draw_624.inc"
};
static NJS_CNK_OBJECT draw_623 = {
#include "stg13/terrain/draw_623.inc"
};
static NJS_CNK_OBJECT draw_622 = {
#include "stg13/terrain/draw_622.inc"
};
static NJS_CNK_OBJECT draw_621 = {
#include "stg13/terrain/draw_621.inc"
};
static NJS_CNK_OBJECT draw_620 = {
#include "stg13/terrain/draw_620.inc"
};
static NJS_CNK_OBJECT draw_619 = {
#include "stg13/terrain/draw_619.inc"
};
static NJS_CNK_OBJECT draw_618 = {
#include "stg13/terrain/draw_618.inc"
};
static NJS_CNK_OBJECT draw_617 = {
#include "stg13/terrain/draw_617.inc"
};
static NJS_CNK_OBJECT draw_616 = {
#include "stg13/terrain/draw_616.inc"
};
static NJS_CNK_OBJECT draw_615 = {
#include "stg13/terrain/draw_615.inc"
};
static NJS_CNK_OBJECT draw_614 = {
#include "stg13/terrain/draw_614.inc"
};
static NJS_CNK_OBJECT draw_613 = {
#include "stg13/terrain/draw_613.inc"
};
static NJS_CNK_OBJECT draw_612 = {
#include "stg13/terrain/draw_612.inc"
};
static NJS_CNK_OBJECT draw_611 = {
#include "stg13/terrain/draw_611.inc"
};
static NJS_CNK_OBJECT draw_610 = {
#include "stg13/terrain/draw_610.inc"
};
static NJS_CNK_OBJECT draw_609 = {
#include "stg13/terrain/draw_609.inc"
};
static NJS_CNK_OBJECT draw_608 = {
#include "stg13/terrain/draw_608.inc"
};
static NJS_CNK_OBJECT draw_607 = {
#include "stg13/terrain/draw_607.inc"
};
static NJS_CNK_OBJECT draw_606 = {
#include "stg13/terrain/draw_606.inc"
};
static NJS_CNK_OBJECT draw_605 = {
#include "stg13/terrain/draw_605.inc"
};
static NJS_CNK_OBJECT draw_604 = {
#include "stg13/terrain/draw_604.inc"
};
static NJS_CNK_OBJECT draw_603 = {
#include "stg13/terrain/draw_603.inc"
};
static NJS_CNK_OBJECT draw_602 = {
#include "stg13/terrain/draw_602.inc"
};
static NJS_CNK_OBJECT draw_601 = {
#include "stg13/terrain/draw_601.inc"
};
static NJS_CNK_OBJECT draw_600 = {
#include "stg13/terrain/draw_600.inc"
};
static NJS_CNK_OBJECT draw_599 = {
#include "stg13/terrain/draw_599.inc"
};
static NJS_CNK_OBJECT draw_598 = {
#include "stg13/terrain/draw_598.inc"
};
static NJS_CNK_OBJECT draw_597 = {
#include "stg13/terrain/draw_597.inc"
};
static NJS_CNK_OBJECT draw_596 = {
#include "stg13/terrain/draw_596.inc"
};
static NJS_CNK_OBJECT draw_595 = {
#include "stg13/terrain/draw_595.inc"
};
static NJS_CNK_OBJECT draw_594 = {
#include "stg13/terrain/draw_594.inc"
};
static NJS_CNK_OBJECT draw_593 = {
#include "stg13/terrain/draw_593.inc"
};
static NJS_CNK_OBJECT draw_592 = {
#include "stg13/terrain/draw_592.inc"
};
static NJS_CNK_OBJECT draw_591 = {
#include "stg13/terrain/draw_591.inc"
};
static NJS_CNK_OBJECT draw_590 = {
#include "stg13/terrain/draw_590.inc"
};
static NJS_CNK_OBJECT draw_589 = {
#include "stg13/terrain/draw_589.inc"
};
static NJS_CNK_OBJECT draw_588 = {
#include "stg13/terrain/draw_588.inc"
};
static NJS_CNK_OBJECT draw_587 = {
#include "stg13/terrain/draw_587.inc"
};
static NJS_CNK_OBJECT draw_586 = {
#include "stg13/terrain/draw_586.inc"
};
static NJS_CNK_OBJECT draw_585 = {
#include "stg13/terrain/draw_585.inc"
};
static NJS_CNK_OBJECT draw_584 = {
#include "stg13/terrain/draw_584.inc"
};
static NJS_CNK_OBJECT draw_583 = {
#include "stg13/terrain/draw_583.inc"
};
static NJS_CNK_OBJECT draw_582 = {
#include "stg13/terrain/draw_582.inc"
};
static NJS_CNK_OBJECT draw_581 = {
#include "stg13/terrain/draw_581.inc"
};
static NJS_CNK_OBJECT draw_580 = {
#include "stg13/terrain/draw_580.inc"
};
static NJS_CNK_OBJECT draw_579 = {
#include "stg13/terrain/draw_579.inc"
};
static NJS_CNK_OBJECT draw_578 = {
#include "stg13/terrain/draw_578.inc"
};
static NJS_CNK_OBJECT draw_577 = {
#include "stg13/terrain/draw_577.inc"
};
static NJS_CNK_OBJECT draw_576 = {
#include "stg13/terrain/draw_576.inc"
};
static NJS_CNK_OBJECT draw_575 = {
#include "stg13/terrain/draw_575.inc"
};
static NJS_CNK_OBJECT draw_574 = {
#include "stg13/terrain/draw_574.inc"
};
static NJS_CNK_OBJECT draw_573 = {
#include "stg13/terrain/draw_573.inc"
};
static NJS_CNK_OBJECT draw_572 = {
#include "stg13/terrain/draw_572.inc"
};
static NJS_CNK_OBJECT draw_571 = {
#include "stg13/terrain/draw_571.inc"
};
static NJS_CNK_OBJECT draw_570 = {
#include "stg13/terrain/draw_570.inc"
};
static NJS_CNK_OBJECT draw_569 = {
#include "stg13/terrain/draw_569.inc"
};
static NJS_CNK_OBJECT draw_568 = {
#include "stg13/terrain/draw_568.inc"
};
static NJS_CNK_OBJECT draw_567 = {
#include "stg13/terrain/draw_567.inc"
};
static NJS_CNK_OBJECT draw_566 = {
#include "stg13/terrain/draw_566.inc"
};
static NJS_CNK_OBJECT draw_565 = {
#include "stg13/terrain/draw_565.inc"
};
static NJS_CNK_OBJECT draw_564 = {
#include "stg13/terrain/draw_564.inc"
};
static NJS_CNK_OBJECT draw_563 = {
#include "stg13/terrain/draw_563.inc"
};
static NJS_CNK_OBJECT draw_562 = {
#include "stg13/terrain/draw_562.inc"
};
static NJS_CNK_OBJECT draw_561 = {
#include "stg13/terrain/draw_561.inc"
};
static NJS_CNK_OBJECT draw_560 = {
#include "stg13/terrain/draw_560.inc"
};
static NJS_CNK_OBJECT draw_559 = {
#include "stg13/terrain/draw_559.inc"
};
static NJS_CNK_OBJECT draw_558 = {
#include "stg13/terrain/draw_558.inc"
};
static NJS_CNK_OBJECT draw_557 = {
#include "stg13/terrain/draw_557.inc"
};
static NJS_CNK_OBJECT draw_556 = {
#include "stg13/terrain/draw_556.inc"
};
static NJS_CNK_OBJECT draw_555 = {
#include "stg13/terrain/draw_555.inc"
};
static NJS_CNK_OBJECT draw_554 = {
#include "stg13/terrain/draw_554.inc"
};
static NJS_CNK_OBJECT draw_553 = {
#include "stg13/terrain/draw_553.inc"
};
static NJS_CNK_OBJECT draw_552 = {
#include "stg13/terrain/draw_552.inc"
};
static NJS_CNK_OBJECT draw_551 = {
#include "stg13/terrain/draw_551.inc"
};
static NJS_CNK_OBJECT draw_550 = {
#include "stg13/terrain/draw_550.inc"
};
static NJS_CNK_OBJECT draw_549 = {
#include "stg13/terrain/draw_549.inc"
};
static NJS_CNK_OBJECT draw_548 = {
#include "stg13/terrain/draw_548.inc"
};
static NJS_CNK_OBJECT draw_547 = {
#include "stg13/terrain/draw_547.inc"
};
static NJS_CNK_OBJECT draw_546 = {
#include "stg13/terrain/draw_546.inc"
};
static NJS_CNK_OBJECT draw_545 = {
#include "stg13/terrain/draw_545.inc"
};
static NJS_CNK_OBJECT draw_544 = {
#include "stg13/terrain/draw_544.inc"
};
static NJS_CNK_OBJECT draw_543 = {
#include "stg13/terrain/draw_543.inc"
};
static NJS_CNK_OBJECT draw_542 = {
#include "stg13/terrain/draw_542.inc"
};
static NJS_CNK_OBJECT draw_541 = {
#include "stg13/terrain/draw_541.inc"
};
static NJS_CNK_OBJECT draw_540 = {
#include "stg13/terrain/draw_540.inc"
};
static NJS_CNK_OBJECT draw_539 = {
#include "stg13/terrain/draw_539.inc"
};
static NJS_CNK_OBJECT draw_538 = {
#include "stg13/terrain/draw_538.inc"
};
static NJS_CNK_OBJECT draw_537 = {
#include "stg13/terrain/draw_537.inc"
};
static NJS_CNK_OBJECT draw_536 = {
#include "stg13/terrain/draw_536.inc"
};
static NJS_CNK_OBJECT draw_535 = {
#include "stg13/terrain/draw_535.inc"
};
static NJS_CNK_OBJECT draw_534 = {
#include "stg13/terrain/draw_534.inc"
};
static NJS_CNK_OBJECT draw_533 = {
#include "stg13/terrain/draw_533.inc"
};
static NJS_CNK_OBJECT draw_532 = {
#include "stg13/terrain/draw_532.inc"
};
static NJS_CNK_OBJECT draw_531 = {
#include "stg13/terrain/draw_531.inc"
};
static NJS_CNK_OBJECT draw_530 = {
#include "stg13/terrain/draw_530.inc"
};
static NJS_CNK_OBJECT draw_529 = {
#include "stg13/terrain/draw_529.inc"
};
static NJS_CNK_OBJECT draw_528 = {
#include "stg13/terrain/draw_528.inc"
};
static NJS_CNK_OBJECT draw_527 = {
#include "stg13/terrain/draw_527.inc"
};
static NJS_CNK_OBJECT draw_526 = {
#include "stg13/terrain/draw_526.inc"
};
static NJS_CNK_OBJECT draw_525 = {
#include "stg13/terrain/draw_525.inc"
};
static NJS_CNK_OBJECT draw_524 = {
#include "stg13/terrain/draw_524.inc"
};
static NJS_CNK_OBJECT draw_523 = {
#include "stg13/terrain/draw_523.inc"
};
static NJS_CNK_OBJECT draw_522 = {
#include "stg13/terrain/draw_522.inc"
};
static NJS_CNK_OBJECT draw_521 = {
#include "stg13/terrain/draw_521.inc"
};
static NJS_CNK_OBJECT draw_520 = {
#include "stg13/terrain/draw_520.inc"
};
static NJS_CNK_OBJECT draw_519 = {
#include "stg13/terrain/draw_519.inc"
};
static NJS_CNK_OBJECT draw_518 = {
#include "stg13/terrain/draw_518.inc"
};
static NJS_CNK_OBJECT draw_517 = {
#include "stg13/terrain/draw_517.inc"
};
static NJS_CNK_OBJECT draw_516 = {
#include "stg13/terrain/draw_516.inc"
};
static NJS_CNK_OBJECT draw_515 = {
#include "stg13/terrain/draw_515.inc"
};
static NJS_CNK_OBJECT draw_514 = {
#include "stg13/terrain/draw_514.inc"
};
static NJS_CNK_OBJECT draw_513 = {
#include "stg13/terrain/draw_513.inc"
};
static NJS_CNK_OBJECT draw_512 = {
#include "stg13/terrain/draw_512.inc"
};
static NJS_CNK_OBJECT draw_511 = {
#include "stg13/terrain/draw_511.inc"
};
static NJS_CNK_OBJECT draw_510 = {
#include "stg13/terrain/draw_510.inc"
};
static NJS_CNK_OBJECT draw_509 = {
#include "stg13/terrain/draw_509.inc"
};
static NJS_CNK_OBJECT draw_508 = {
#include "stg13/terrain/draw_508.inc"
};
static NJS_CNK_OBJECT draw_507 = {
#include "stg13/terrain/draw_507.inc"
};
static NJS_CNK_OBJECT draw_506 = {
#include "stg13/terrain/draw_506.inc"
};
static NJS_CNK_OBJECT draw_505 = {
#include "stg13/terrain/draw_505.inc"
};
static NJS_CNK_OBJECT draw_504 = {
#include "stg13/terrain/draw_504.inc"
};
static NJS_CNK_OBJECT draw_503 = {
#include "stg13/terrain/draw_503.inc"
};
static NJS_CNK_OBJECT draw_502 = {
#include "stg13/terrain/draw_502.inc"
};
static NJS_CNK_OBJECT draw_501 = {
#include "stg13/terrain/draw_501.inc"
};
static NJS_CNK_OBJECT draw_500 = {
#include "stg13/terrain/draw_500.inc"
};
static NJS_CNK_OBJECT draw_499 = {
#include "stg13/terrain/draw_499.inc"
};
static NJS_CNK_OBJECT draw_498 = {
#include "stg13/terrain/draw_498.inc"
};
static NJS_CNK_OBJECT draw_497 = {
#include "stg13/terrain/draw_497.inc"
};
static NJS_CNK_OBJECT draw_496 = {
#include "stg13/terrain/draw_496.inc"
};
static NJS_CNK_OBJECT draw_495 = {
#include "stg13/terrain/draw_495.inc"
};
static NJS_CNK_OBJECT draw_494 = {
#include "stg13/terrain/draw_494.inc"
};
static NJS_CNK_OBJECT draw_493 = {
#include "stg13/terrain/draw_493.inc"
};
static NJS_CNK_OBJECT draw_492 = {
#include "stg13/terrain/draw_492.inc"
};
static NJS_CNK_OBJECT draw_491 = {
#include "stg13/terrain/draw_491.inc"
};
static NJS_CNK_OBJECT draw_490 = {
#include "stg13/terrain/draw_490.inc"
};
static NJS_CNK_OBJECT draw_489 = {
#include "stg13/terrain/draw_489.inc"
};
static NJS_CNK_OBJECT draw_488 = {
#include "stg13/terrain/draw_488.inc"
};
static NJS_CNK_OBJECT draw_487 = {
#include "stg13/terrain/draw_487.inc"
};
static NJS_CNK_OBJECT draw_486 = {
#include "stg13/terrain/draw_486.inc"
};
static NJS_CNK_OBJECT draw_485 = {
#include "stg13/terrain/draw_485.inc"
};
static NJS_CNK_OBJECT draw_484 = {
#include "stg13/terrain/draw_484.inc"
};
static NJS_CNK_OBJECT draw_483 = {
#include "stg13/terrain/draw_483.inc"
};
static NJS_CNK_OBJECT draw_482 = {
#include "stg13/terrain/draw_482.inc"
};
static NJS_CNK_OBJECT draw_481 = {
#include "stg13/terrain/draw_481.inc"
};
static NJS_CNK_OBJECT draw_480 = {
#include "stg13/terrain/draw_480.inc"
};
static NJS_CNK_OBJECT draw_479 = {
#include "stg13/terrain/draw_479.inc"
};
static NJS_CNK_OBJECT draw_478 = {
#include "stg13/terrain/draw_478.inc"
};
static NJS_CNK_OBJECT draw_477 = {
#include "stg13/terrain/draw_477.inc"
};
static NJS_CNK_OBJECT draw_476 = {
#include "stg13/terrain/draw_476.inc"
};
static NJS_CNK_OBJECT draw_475 = {
#include "stg13/terrain/draw_475.inc"
};
static NJS_CNK_OBJECT draw_474 = {
#include "stg13/terrain/draw_474.inc"
};
static NJS_CNK_OBJECT draw_473 = {
#include "stg13/terrain/draw_473.inc"
};
static NJS_CNK_OBJECT draw_472 = {
#include "stg13/terrain/draw_472.inc"
};
static NJS_CNK_OBJECT draw_471 = {
#include "stg13/terrain/draw_471.inc"
};
static NJS_CNK_OBJECT draw_470 = {
#include "stg13/terrain/draw_470.inc"
};
static NJS_CNK_OBJECT draw_469 = {
#include "stg13/terrain/draw_469.inc"
};
static NJS_CNK_OBJECT draw_468 = {
#include "stg13/terrain/draw_468.inc"
};
static NJS_CNK_OBJECT draw_467 = {
#include "stg13/terrain/draw_467.inc"
};
static NJS_CNK_OBJECT draw_466 = {
#include "stg13/terrain/draw_466.inc"
};
static NJS_CNK_OBJECT draw_465 = {
#include "stg13/terrain/draw_465.inc"
};
static NJS_CNK_OBJECT draw_464 = {
#include "stg13/terrain/draw_464.inc"
};
static NJS_CNK_OBJECT draw_463 = {
#include "stg13/terrain/draw_463.inc"
};
static NJS_CNK_OBJECT draw_462 = {
#include "stg13/terrain/draw_462.inc"
};
static NJS_CNK_OBJECT draw_461 = {
#include "stg13/terrain/draw_461.inc"
};
static NJS_CNK_OBJECT draw_460 = {
#include "stg13/terrain/draw_460.inc"
};
static NJS_CNK_OBJECT draw_459 = {
#include "stg13/terrain/draw_459.inc"
};
static NJS_CNK_OBJECT draw_458 = {
#include "stg13/terrain/draw_458.inc"
};
static NJS_CNK_OBJECT draw_457 = {
#include "stg13/terrain/draw_457.inc"
};
static NJS_CNK_OBJECT draw_456 = {
#include "stg13/terrain/draw_456.inc"
};
static NJS_CNK_OBJECT draw_455 = {
#include "stg13/terrain/draw_455.inc"
};
static NJS_CNK_OBJECT draw_454 = {
#include "stg13/terrain/draw_454.inc"
};
static NJS_CNK_OBJECT draw_453 = {
#include "stg13/terrain/draw_453.inc"
};
static NJS_CNK_OBJECT draw_452 = {
#include "stg13/terrain/draw_452.inc"
};
static NJS_CNK_OBJECT draw_451 = {
#include "stg13/terrain/draw_451.inc"
};
static NJS_CNK_OBJECT draw_450 = {
#include "stg13/terrain/draw_450.inc"
};
static NJS_CNK_OBJECT draw_449 = {
#include "stg13/terrain/draw_449.inc"
};
static NJS_CNK_OBJECT draw_448 = {
#include "stg13/terrain/draw_448.inc"
};
static NJS_CNK_OBJECT draw_447 = {
#include "stg13/terrain/draw_447.inc"
};
static NJS_CNK_OBJECT draw_446 = {
#include "stg13/terrain/draw_446.inc"
};
static NJS_CNK_OBJECT draw_445 = {
#include "stg13/terrain/draw_445.inc"
};
static NJS_CNK_OBJECT draw_444 = {
#include "stg13/terrain/draw_444.inc"
};
static NJS_CNK_OBJECT draw_443 = {
#include "stg13/terrain/draw_443.inc"
};
static NJS_CNK_OBJECT draw_442 = {
#include "stg13/terrain/draw_442.inc"
};
static NJS_CNK_OBJECT draw_441 = {
#include "stg13/terrain/draw_441.inc"
};
static NJS_CNK_OBJECT draw_440 = {
#include "stg13/terrain/draw_440.inc"
};
static NJS_CNK_OBJECT draw_439 = {
#include "stg13/terrain/draw_439.inc"
};
static NJS_CNK_OBJECT draw_438 = {
#include "stg13/terrain/draw_438.inc"
};
static NJS_CNK_OBJECT draw_437 = {
#include "stg13/terrain/draw_437.inc"
};
static NJS_CNK_OBJECT draw_436 = {
#include "stg13/terrain/draw_436.inc"
};
static NJS_CNK_OBJECT draw_435 = {
#include "stg13/terrain/draw_435.inc"
};
static NJS_CNK_OBJECT draw_434 = {
#include "stg13/terrain/draw_434.inc"
};
static NJS_CNK_OBJECT draw_433 = {
#include "stg13/terrain/draw_433.inc"
};
static NJS_CNK_OBJECT draw_432 = {
#include "stg13/terrain/draw_432.inc"
};
static NJS_CNK_OBJECT draw_431 = {
#include "stg13/terrain/draw_431.inc"
};
static NJS_CNK_OBJECT draw_430 = {
#include "stg13/terrain/draw_430.inc"
};
static NJS_CNK_OBJECT draw_429 = {
#include "stg13/terrain/draw_429.inc"
};
static NJS_CNK_OBJECT draw_428 = {
#include "stg13/terrain/draw_428.inc"
};
static NJS_CNK_OBJECT draw_427 = {
#include "stg13/terrain/draw_427.inc"
};
static NJS_CNK_OBJECT draw_426 = {
#include "stg13/terrain/draw_426.inc"
};
static NJS_CNK_OBJECT draw_425 = {
#include "stg13/terrain/draw_425.inc"
};
static NJS_CNK_OBJECT draw_424 = {
#include "stg13/terrain/draw_424.inc"
};
static NJS_CNK_OBJECT draw_423 = {
#include "stg13/terrain/draw_423.inc"
};
static NJS_CNK_OBJECT draw_422 = {
#include "stg13/terrain/draw_422.inc"
};
static NJS_CNK_OBJECT draw_421 = {
#include "stg13/terrain/draw_421.inc"
};
static NJS_CNK_OBJECT draw_420 = {
#include "stg13/terrain/draw_420.inc"
};
static NJS_CNK_OBJECT draw_419 = {
#include "stg13/terrain/draw_419.inc"
};
static NJS_CNK_OBJECT draw_418 = {
#include "stg13/terrain/draw_418.inc"
};
static NJS_CNK_OBJECT draw_417 = {
#include "stg13/terrain/draw_417.inc"
};
static NJS_CNK_OBJECT draw_416 = {
#include "stg13/terrain/draw_416.inc"
};
static NJS_CNK_OBJECT draw_415 = {
#include "stg13/terrain/draw_415.inc"
};
static NJS_CNK_OBJECT draw_414 = {
#include "stg13/terrain/draw_414.inc"
};
static NJS_CNK_OBJECT draw_413 = {
#include "stg13/terrain/draw_413.inc"
};
static NJS_CNK_OBJECT draw_412 = {
#include "stg13/terrain/draw_412.inc"
};
static NJS_CNK_OBJECT draw_411 = {
#include "stg13/terrain/draw_411.inc"
};
static NJS_CNK_OBJECT draw_410 = {
#include "stg13/terrain/draw_410.inc"
};
static NJS_CNK_OBJECT draw_409 = {
#include "stg13/terrain/draw_409.inc"
};
static NJS_CNK_OBJECT draw_408 = {
#include "stg13/terrain/draw_408.inc"
};
static NJS_CNK_OBJECT draw_407 = {
#include "stg13/terrain/draw_407.inc"
};
static NJS_CNK_OBJECT draw_406 = {
#include "stg13/terrain/draw_406.inc"
};
static NJS_CNK_OBJECT draw_405 = {
#include "stg13/terrain/draw_405.inc"
};
static NJS_CNK_OBJECT draw_404 = {
#include "stg13/terrain/draw_404.inc"
};
static NJS_CNK_OBJECT draw_403 = {
#include "stg13/terrain/draw_403.inc"
};
static NJS_CNK_OBJECT draw_402 = {
#include "stg13/terrain/draw_402.inc"
};
static NJS_CNK_OBJECT draw_401 = {
#include "stg13/terrain/draw_401.inc"
};
static NJS_CNK_OBJECT draw_400 = {
#include "stg13/terrain/draw_400.inc"
};
static NJS_CNK_OBJECT draw_399 = {
#include "stg13/terrain/draw_399.inc"
};
static NJS_CNK_OBJECT draw_398 = {
#include "stg13/terrain/draw_398.inc"
};
static NJS_CNK_OBJECT draw_397 = {
#include "stg13/terrain/draw_397.inc"
};
static NJS_CNK_OBJECT draw_396 = {
#include "stg13/terrain/draw_396.inc"
};
static NJS_CNK_OBJECT draw_395 = {
#include "stg13/terrain/draw_395.inc"
};
static NJS_CNK_OBJECT draw_394 = {
#include "stg13/terrain/draw_394.inc"
};
static NJS_CNK_OBJECT draw_393 = {
#include "stg13/terrain/draw_393.inc"
};
static NJS_CNK_OBJECT draw_392 = {
#include "stg13/terrain/draw_392.inc"
};
static NJS_CNK_OBJECT draw_391 = {
#include "stg13/terrain/draw_391.inc"
};
static NJS_CNK_OBJECT draw_390 = {
#include "stg13/terrain/draw_390.inc"
};
static NJS_CNK_OBJECT draw_389 = {
#include "stg13/terrain/draw_389.inc"
};
static NJS_CNK_OBJECT draw_388 = {
#include "stg13/terrain/draw_388.inc"
};
static NJS_CNK_OBJECT draw_387 = {
#include "stg13/terrain/draw_387.inc"
};
static NJS_CNK_OBJECT draw_386 = {
#include "stg13/terrain/draw_386.inc"
};
static NJS_CNK_OBJECT draw_385 = {
#include "stg13/terrain/draw_385.inc"
};
static NJS_CNK_OBJECT draw_384 = {
#include "stg13/terrain/draw_384.inc"
};
static NJS_CNK_OBJECT draw_383 = {
#include "stg13/terrain/draw_383.inc"
};
static NJS_CNK_OBJECT draw_382 = {
#include "stg13/terrain/draw_382.inc"
};
static NJS_CNK_OBJECT draw_381 = {
#include "stg13/terrain/draw_381.inc"
};
static NJS_CNK_OBJECT draw_380 = {
#include "stg13/terrain/draw_380.inc"
};
static NJS_CNK_OBJECT draw_379 = {
#include "stg13/terrain/draw_379.inc"
};
static NJS_CNK_OBJECT draw_378 = {
#include "stg13/terrain/draw_378.inc"
};
static NJS_CNK_OBJECT draw_377 = {
#include "stg13/terrain/draw_377.inc"
};
static NJS_CNK_OBJECT draw_376 = {
#include "stg13/terrain/draw_376.inc"
};
static NJS_CNK_OBJECT draw_375 = {
#include "stg13/terrain/draw_375.inc"
};
static NJS_CNK_OBJECT draw_374 = {
#include "stg13/terrain/draw_374.inc"
};
static NJS_CNK_OBJECT draw_373 = {
#include "stg13/terrain/draw_373.inc"
};
static NJS_CNK_OBJECT draw_372 = {
#include "stg13/terrain/draw_372.inc"
};
static NJS_CNK_OBJECT draw_371 = {
#include "stg13/terrain/draw_371.inc"
};
static NJS_CNK_OBJECT draw_370 = {
#include "stg13/terrain/draw_370.inc"
};
static NJS_CNK_OBJECT draw_369 = {
#include "stg13/terrain/draw_369.inc"
};
static NJS_CNK_OBJECT draw_368 = {
#include "stg13/terrain/draw_368.inc"
};
static NJS_CNK_OBJECT draw_367 = {
#include "stg13/terrain/draw_367.inc"
};
static NJS_CNK_OBJECT draw_366 = {
#include "stg13/terrain/draw_366.inc"
};
static NJS_CNK_OBJECT draw_365 = {
#include "stg13/terrain/draw_365.inc"
};
static NJS_CNK_OBJECT draw_364 = {
#include "stg13/terrain/draw_364.inc"
};
static NJS_CNK_OBJECT draw_363 = {
#include "stg13/terrain/draw_363.inc"
};
static NJS_CNK_OBJECT draw_362 = {
#include "stg13/terrain/draw_362.inc"
};
static NJS_CNK_OBJECT draw_361 = {
#include "stg13/terrain/draw_361.inc"
};
static NJS_CNK_OBJECT draw_360 = {
#include "stg13/terrain/draw_360.inc"
};
static NJS_CNK_OBJECT draw_359 = {
#include "stg13/terrain/draw_359.inc"
};
static NJS_CNK_OBJECT draw_358 = {
#include "stg13/terrain/draw_358.inc"
};
static NJS_CNK_OBJECT draw_357 = {
#include "stg13/terrain/draw_357.inc"
};
static NJS_CNK_OBJECT draw_356 = {
#include "stg13/terrain/draw_356.inc"
};
static NJS_CNK_OBJECT draw_355 = {
#include "stg13/terrain/draw_355.inc"
};
static NJS_CNK_OBJECT draw_354 = {
#include "stg13/terrain/draw_354.inc"
};
static NJS_CNK_OBJECT draw_353 = {
#include "stg13/terrain/draw_353.inc"
};
static NJS_CNK_OBJECT draw_352 = {
#include "stg13/terrain/draw_352.inc"
};
static NJS_CNK_OBJECT draw_351 = {
#include "stg13/terrain/draw_351.inc"
};
static NJS_CNK_OBJECT draw_350 = {
#include "stg13/terrain/draw_350.inc"
};
static NJS_CNK_OBJECT draw_349 = {
#include "stg13/terrain/draw_349.inc"
};
static NJS_CNK_OBJECT draw_348 = {
#include "stg13/terrain/draw_348.inc"
};
static NJS_CNK_OBJECT draw_347 = {
#include "stg13/terrain/draw_347.inc"
};
static NJS_CNK_OBJECT draw_346 = {
#include "stg13/terrain/draw_346.inc"
};
static NJS_CNK_OBJECT draw_345 = {
#include "stg13/terrain/draw_345.inc"
};
static NJS_CNK_OBJECT draw_344 = {
#include "stg13/terrain/draw_344.inc"
};
static NJS_CNK_OBJECT draw_343 = {
#include "stg13/terrain/draw_343.inc"
};
static NJS_CNK_OBJECT draw_342 = {
#include "stg13/terrain/draw_342.inc"
};
static NJS_CNK_OBJECT draw_341 = {
#include "stg13/terrain/draw_341.inc"
};
static NJS_CNK_OBJECT draw_340 = {
#include "stg13/terrain/draw_340.inc"
};
static NJS_CNK_OBJECT draw_339 = {
#include "stg13/terrain/draw_339.inc"
};
static NJS_CNK_OBJECT draw_338 = {
#include "stg13/terrain/draw_338.inc"
};
static NJS_CNK_OBJECT draw_337 = {
#include "stg13/terrain/draw_337.inc"
};
static NJS_CNK_OBJECT draw_336 = {
#include "stg13/terrain/draw_336.inc"
};
static NJS_CNK_OBJECT draw_335 = {
#include "stg13/terrain/draw_335.inc"
};
static NJS_CNK_OBJECT draw_334 = {
#include "stg13/terrain/draw_334.inc"
};
static NJS_CNK_OBJECT draw_333 = {
#include "stg13/terrain/draw_333.inc"
};
static NJS_CNK_OBJECT draw_332 = {
#include "stg13/terrain/draw_332.inc"
};
static NJS_CNK_OBJECT draw_331 = {
#include "stg13/terrain/draw_331.inc"
};
static NJS_CNK_OBJECT draw_330 = {
#include "stg13/terrain/draw_330.inc"
};
static NJS_CNK_OBJECT draw_329 = {
#include "stg13/terrain/draw_329.inc"
};
static NJS_CNK_OBJECT draw_328 = {
#include "stg13/terrain/draw_328.inc"
};
static NJS_CNK_OBJECT draw_327 = {
#include "stg13/terrain/draw_327.inc"
};
static NJS_CNK_OBJECT draw_326 = {
#include "stg13/terrain/draw_326.inc"
};
static NJS_CNK_OBJECT draw_325 = {
#include "stg13/terrain/draw_325.inc"
};
static NJS_CNK_OBJECT draw_324 = {
#include "stg13/terrain/draw_324.inc"
};
static NJS_CNK_OBJECT draw_323 = {
#include "stg13/terrain/draw_323.inc"
};
static NJS_CNK_OBJECT draw_322 = {
#include "stg13/terrain/draw_322.inc"
};
static NJS_CNK_OBJECT draw_321 = {
#include "stg13/terrain/draw_321.inc"
};
static NJS_CNK_OBJECT draw_320 = {
#include "stg13/terrain/draw_320.inc"
};
static NJS_CNK_OBJECT draw_319 = {
#include "stg13/terrain/draw_319.inc"
};
static NJS_CNK_OBJECT draw_318 = {
#include "stg13/terrain/draw_318.inc"
};
static NJS_CNK_OBJECT draw_317 = {
#include "stg13/terrain/draw_317.inc"
};
static NJS_CNK_OBJECT draw_316 = {
#include "stg13/terrain/draw_316.inc"
};
static NJS_CNK_OBJECT draw_315 = {
#include "stg13/terrain/draw_315.inc"
};
static NJS_CNK_OBJECT draw_314 = {
#include "stg13/terrain/draw_314.inc"
};
static NJS_CNK_OBJECT draw_313 = {
#include "stg13/terrain/draw_313.inc"
};
static NJS_CNK_OBJECT draw_312 = {
#include "stg13/terrain/draw_312.inc"
};
static NJS_CNK_OBJECT draw_311 = {
#include "stg13/terrain/draw_311.inc"
};
static NJS_CNK_OBJECT draw_310 = {
#include "stg13/terrain/draw_310.inc"
};
static NJS_CNK_OBJECT draw_309 = {
#include "stg13/terrain/draw_309.inc"
};
static NJS_CNK_OBJECT draw_308 = {
#include "stg13/terrain/draw_308.inc"
};
static NJS_CNK_OBJECT draw_307 = {
#include "stg13/terrain/draw_307.inc"
};
static NJS_CNK_OBJECT draw_306 = {
#include "stg13/terrain/draw_306.inc"
};
static NJS_CNK_OBJECT draw_305 = {
#include "stg13/terrain/draw_305.inc"
};
static NJS_CNK_OBJECT draw_304 = {
#include "stg13/terrain/draw_304.inc"
};
static NJS_CNK_OBJECT draw_303 = {
#include "stg13/terrain/draw_303.inc"
};
static NJS_CNK_OBJECT draw_302 = {
#include "stg13/terrain/draw_302.inc"
};
static NJS_CNK_OBJECT draw_301 = {
#include "stg13/terrain/draw_301.inc"
};
static NJS_CNK_OBJECT draw_300 = {
#include "stg13/terrain/draw_300.inc"
};
static NJS_CNK_OBJECT draw_298 = {
#include "stg13/terrain/draw_298.inc"
};
static NJS_CNK_OBJECT draw_297 = {
#include "stg13/terrain/draw_297.inc"
};
static NJS_CNK_OBJECT draw_296 = {
#include "stg13/terrain/draw_296.inc"
};
static NJS_CNK_OBJECT draw_295 = {
#include "stg13/terrain/draw_295.inc"
};
static NJS_CNK_OBJECT draw_294 = {
#include "stg13/terrain/draw_294.inc"
};
static NJS_CNK_OBJECT draw_293 = {
#include "stg13/terrain/draw_293.inc"
};
static NJS_CNK_OBJECT draw_292 = {
#include "stg13/terrain/draw_292.inc"
};
static NJS_CNK_OBJECT draw_291 = {
#include "stg13/terrain/draw_291.inc"
};
static NJS_CNK_OBJECT draw_290 = {
#include "stg13/terrain/draw_290.inc"
};
static NJS_CNK_OBJECT draw_289 = {
#include "stg13/terrain/draw_289.inc"
};
static NJS_CNK_OBJECT draw_288 = {
#include "stg13/terrain/draw_288.inc"
};
static NJS_CNK_OBJECT draw_287 = {
#include "stg13/terrain/draw_287.inc"
};
static NJS_CNK_OBJECT draw_286 = {
#include "stg13/terrain/draw_286.inc"
};
static NJS_CNK_OBJECT draw_285 = {
#include "stg13/terrain/draw_285.inc"
};
static NJS_CNK_OBJECT draw_284 = {
#include "stg13/terrain/draw_284.inc"
};
static NJS_CNK_OBJECT draw_283 = {
#include "stg13/terrain/draw_283.inc"
};
static NJS_CNK_OBJECT draw_282 = {
#include "stg13/terrain/draw_282.inc"
};
static NJS_CNK_OBJECT draw_281 = {
#include "stg13/terrain/draw_281.inc"
};
static NJS_CNK_OBJECT draw_280 = {
#include "stg13/terrain/draw_280.inc"
};
static NJS_CNK_OBJECT draw_279 = {
#include "stg13/terrain/draw_279.inc"
};
static NJS_CNK_OBJECT draw_278 = {
#include "stg13/terrain/draw_278.inc"
};
static NJS_CNK_OBJECT draw_277 = {
#include "stg13/terrain/draw_277.inc"
};
static NJS_CNK_OBJECT draw_276 = {
#include "stg13/terrain/draw_276.inc"
};
static NJS_CNK_OBJECT draw_275 = {
#include "stg13/terrain/draw_275.inc"
};
static NJS_CNK_OBJECT draw_274 = {
#include "stg13/terrain/draw_274.inc"
};
static NJS_CNK_OBJECT draw_273 = {
#include "stg13/terrain/draw_273.inc"
};
static NJS_CNK_OBJECT draw_272 = {
#include "stg13/terrain/draw_272.inc"
};
static NJS_CNK_OBJECT draw_271 = {
#include "stg13/terrain/draw_271.inc"
};
static NJS_CNK_OBJECT draw_270 = {
#include "stg13/terrain/draw_270.inc"
};
static NJS_CNK_OBJECT draw_269 = {
#include "stg13/terrain/draw_269.inc"
};
static NJS_CNK_OBJECT draw_268 = {
#include "stg13/terrain/draw_268.inc"
};
static NJS_CNK_OBJECT draw_267 = {
#include "stg13/terrain/draw_267.inc"
};
static NJS_CNK_OBJECT draw_266 = {
#include "stg13/terrain/draw_266.inc"
};
static NJS_CNK_OBJECT draw_265 = {
#include "stg13/terrain/draw_265.inc"
};
static NJS_CNK_OBJECT draw_264 = {
#include "stg13/terrain/draw_264.inc"
};
static NJS_CNK_OBJECT draw_263 = {
#include "stg13/terrain/draw_263.inc"
};
static NJS_CNK_OBJECT draw_262 = {
#include "stg13/terrain/draw_262.inc"
};
static NJS_CNK_OBJECT draw_261 = {
#include "stg13/terrain/draw_261.inc"
};
static NJS_CNK_OBJECT draw_260 = {
#include "stg13/terrain/draw_260.inc"
};
static NJS_CNK_OBJECT draw_259 = {
#include "stg13/terrain/draw_259.inc"
};
static NJS_CNK_OBJECT draw_258 = {
#include "stg13/terrain/draw_258.inc"
};
static NJS_CNK_OBJECT draw_257 = {
#include "stg13/terrain/draw_257.inc"
};
static NJS_CNK_OBJECT draw_256 = {
#include "stg13/terrain/draw_256.inc"
};
static NJS_CNK_OBJECT draw_255 = {
#include "stg13/terrain/draw_255.inc"
};
static NJS_CNK_OBJECT draw_254 = {
#include "stg13/terrain/draw_254.inc"
};
static NJS_CNK_OBJECT draw_253 = {
#include "stg13/terrain/draw_253.inc"
};
static NJS_CNK_OBJECT draw_252 = {
#include "stg13/terrain/draw_252.inc"
};
static NJS_CNK_OBJECT draw_251 = {
#include "stg13/terrain/draw_251.inc"
};
static NJS_CNK_OBJECT draw_250 = {
#include "stg13/terrain/draw_250.inc"
};
static NJS_CNK_OBJECT draw_249 = {
#include "stg13/terrain/draw_249.inc"
};
static NJS_CNK_OBJECT draw_248 = {
#include "stg13/terrain/draw_248.inc"
};
static NJS_CNK_OBJECT draw_247 = {
#include "stg13/terrain/draw_247.inc"
};
static NJS_CNK_OBJECT draw_246 = {
#include "stg13/terrain/draw_246.inc"
};
static NJS_CNK_OBJECT draw_245 = {
#include "stg13/terrain/draw_245.inc"
};
static NJS_CNK_OBJECT draw_244 = {
#include "stg13/terrain/draw_244.inc"
};
static NJS_CNK_OBJECT draw_243 = {
#include "stg13/terrain/draw_243.inc"
};
static NJS_CNK_OBJECT draw_242 = {
#include "stg13/terrain/draw_242.inc"
};
static NJS_CNK_OBJECT draw_241 = {
#include "stg13/terrain/draw_241.inc"
};
static NJS_CNK_OBJECT draw_240 = {
#include "stg13/terrain/draw_240.inc"
};
static NJS_CNK_OBJECT draw_239 = {
#include "stg13/terrain/draw_239.inc"
};
static NJS_CNK_OBJECT draw_238 = {
#include "stg13/terrain/draw_238.inc"
};
static NJS_CNK_OBJECT draw_237 = {
#include "stg13/terrain/draw_237.inc"
};
static NJS_CNK_OBJECT draw_236 = {
#include "stg13/terrain/draw_236.inc"
};
static NJS_CNK_OBJECT draw_235 = {
#include "stg13/terrain/draw_235.inc"
};
static NJS_CNK_OBJECT draw_234 = {
#include "stg13/terrain/draw_234.inc"
};
static NJS_CNK_OBJECT draw_233 = {
#include "stg13/terrain/draw_233.inc"
};
static NJS_CNK_OBJECT draw_232 = {
#include "stg13/terrain/draw_232.inc"
};
static NJS_CNK_OBJECT draw_231 = {
#include "stg13/terrain/draw_231.inc"
};
static NJS_CNK_OBJECT draw_230 = {
#include "stg13/terrain/draw_230.inc"
};
static NJS_CNK_OBJECT draw_229 = {
#include "stg13/terrain/draw_229.inc"
};
static NJS_CNK_OBJECT draw_228 = {
#include "stg13/terrain/draw_228.inc"
};
static NJS_CNK_OBJECT draw_227 = {
#include "stg13/terrain/draw_227.inc"
};
static NJS_CNK_OBJECT draw_226 = {
#include "stg13/terrain/draw_226.inc"
};
static NJS_CNK_OBJECT draw_224 = {
#include "stg13/terrain/draw_224.inc"
};
static NJS_CNK_OBJECT draw_223 = {
#include "stg13/terrain/draw_223.inc"
};
static NJS_CNK_OBJECT draw_222 = {
#include "stg13/terrain/draw_222.inc"
};
static NJS_CNK_OBJECT draw_221 = {
#include "stg13/terrain/draw_221.inc"
};
static NJS_CNK_OBJECT draw_220 = {
#include "stg13/terrain/draw_220.inc"
};
static NJS_CNK_OBJECT draw_219 = {
#include "stg13/terrain/draw_219.inc"
};
static NJS_CNK_OBJECT draw_218 = {
#include "stg13/terrain/draw_218.inc"
};
static NJS_CNK_OBJECT draw_216 = {
#include "stg13/terrain/draw_216.inc"
};
static NJS_CNK_OBJECT draw_215 = {
#include "stg13/terrain/draw_215.inc"
};
static NJS_CNK_OBJECT draw_214 = {
#include "stg13/terrain/draw_214.inc"
};
static NJS_CNK_OBJECT draw_213 = {
#include "stg13/terrain/draw_213.inc"
};
static NJS_CNK_OBJECT draw_212 = {
#include "stg13/terrain/draw_212.inc"
};
static NJS_CNK_OBJECT draw_211 = {
#include "stg13/terrain/draw_211.inc"
};
static NJS_CNK_OBJECT draw_210 = {
#include "stg13/terrain/draw_210.inc"
};
static NJS_CNK_OBJECT draw_209 = {
#include "stg13/terrain/draw_209.inc"
};
static NJS_CNK_OBJECT draw_208 = {
#include "stg13/terrain/draw_208.inc"
};
static NJS_CNK_OBJECT draw_207 = {
#include "stg13/terrain/draw_207.inc"
};
static NJS_CNK_OBJECT draw_206 = {
#include "stg13/terrain/draw_206.inc"
};
static NJS_CNK_OBJECT draw_205 = {
#include "stg13/terrain/draw_205.inc"
};
static NJS_CNK_OBJECT draw_204 = {
#include "stg13/terrain/draw_204.inc"
};
static NJS_CNK_OBJECT draw_203 = {
#include "stg13/terrain/draw_203.inc"
};
static NJS_CNK_OBJECT draw_202 = {
#include "stg13/terrain/draw_202.inc"
};
static NJS_CNK_OBJECT draw_201 = {
#include "stg13/terrain/draw_201.inc"
};
static NJS_CNK_OBJECT draw_200 = {
#include "stg13/terrain/draw_200.inc"
};
static NJS_CNK_OBJECT draw_199 = {
#include "stg13/terrain/draw_199.inc"
};
static NJS_CNK_OBJECT draw_198 = {
#include "stg13/terrain/draw_198.inc"
};
static NJS_CNK_OBJECT draw_197 = {
#include "stg13/terrain/draw_197.inc"
};
static NJS_CNK_OBJECT draw_196 = {
#include "stg13/terrain/draw_196.inc"
};
static NJS_CNK_OBJECT draw_194 = {
#include "stg13/terrain/draw_194.inc"
};
static NJS_CNK_OBJECT draw_193 = {
#include "stg13/terrain/draw_193.inc"
};
static NJS_CNK_OBJECT draw_192 = {
#include "stg13/terrain/draw_192.inc"
};
static NJS_CNK_OBJECT draw_191 = {
#include "stg13/terrain/draw_191.inc"
};
static NJS_CNK_OBJECT draw_190 = {
#include "stg13/terrain/draw_190.inc"
};
static NJS_CNK_OBJECT draw_189 = {
#include "stg13/terrain/draw_189.inc"
};
static NJS_CNK_OBJECT draw_188 = {
#include "stg13/terrain/draw_188.inc"
};
static NJS_CNK_OBJECT draw_187 = {
#include "stg13/terrain/draw_187.inc"
};
static NJS_CNK_OBJECT draw_186 = {
#include "stg13/terrain/draw_186.inc"
};
static NJS_CNK_OBJECT draw_185 = {
#include "stg13/terrain/draw_185.inc"
};
static NJS_CNK_OBJECT draw_184 = {
#include "stg13/terrain/draw_184.inc"
};
static NJS_CNK_OBJECT draw_183 = {
#include "stg13/terrain/draw_183.inc"
};
static NJS_CNK_OBJECT draw_182 = {
#include "stg13/terrain/draw_182.inc"
};
static NJS_CNK_OBJECT draw_181 = {
#include "stg13/terrain/draw_181.inc"
};
static NJS_CNK_OBJECT draw_180 = {
#include "stg13/terrain/draw_180.inc"
};
static NJS_CNK_OBJECT draw_179 = {
#include "stg13/terrain/draw_179.inc"
};
static NJS_CNK_OBJECT draw_178 = {
#include "stg13/terrain/draw_178.inc"
};
static NJS_CNK_OBJECT draw_177 = {
#include "stg13/terrain/draw_177.inc"
};
static NJS_CNK_OBJECT draw_176 = {
#include "stg13/terrain/draw_176.inc"
};
static NJS_CNK_OBJECT draw_175 = {
#include "stg13/terrain/draw_175.inc"
};
static NJS_CNK_OBJECT draw_174 = {
#include "stg13/terrain/draw_174.inc"
};
static NJS_CNK_OBJECT draw_173 = {
#include "stg13/terrain/draw_173.inc"
};
static NJS_CNK_OBJECT draw_172 = {
#include "stg13/terrain/draw_172.inc"
};
static NJS_CNK_OBJECT draw_171 = {
#include "stg13/terrain/draw_171.inc"
};
static NJS_CNK_OBJECT draw_170 = {
#include "stg13/terrain/draw_170.inc"
};
static NJS_CNK_OBJECT draw_169 = {
#include "stg13/terrain/draw_169.inc"
};
static NJS_CNK_OBJECT draw_168 = {
#include "stg13/terrain/draw_168.inc"
};
static NJS_CNK_OBJECT draw_167 = {
#include "stg13/terrain/draw_167.inc"
};
static NJS_CNK_OBJECT draw_166 = {
#include "stg13/terrain/draw_166.inc"
};
static NJS_CNK_OBJECT draw_165 = {
#include "stg13/terrain/draw_165.inc"
};
static NJS_CNK_OBJECT draw_164 = {
#include "stg13/terrain/draw_164.inc"
};
static NJS_CNK_OBJECT draw_163 = {
#include "stg13/terrain/draw_163.inc"
};
static NJS_CNK_OBJECT draw_162 = {
#include "stg13/terrain/draw_162.inc"
};
static NJS_CNK_OBJECT draw_161 = {
#include "stg13/terrain/draw_161.inc"
};
static NJS_CNK_OBJECT draw_160 = {
#include "stg13/terrain/draw_160.inc"
};
static NJS_CNK_OBJECT draw_159 = {
#include "stg13/terrain/draw_159.inc"
};
static NJS_CNK_OBJECT draw_158 = {
#include "stg13/terrain/draw_158.inc"
};
static NJS_CNK_OBJECT draw_157 = {
#include "stg13/terrain/draw_157.inc"
};
static NJS_CNK_OBJECT draw_156 = {
#include "stg13/terrain/draw_156.inc"
};
static NJS_CNK_OBJECT draw_155 = {
#include "stg13/terrain/draw_155.inc"
};
static NJS_CNK_OBJECT draw_154 = {
#include "stg13/terrain/draw_154.inc"
};
static NJS_CNK_OBJECT draw_153 = {
#include "stg13/terrain/draw_153.inc"
};
static NJS_CNK_OBJECT draw_152 = {
#include "stg13/terrain/draw_152.inc"
};
static NJS_CNK_OBJECT draw_151 = {
#include "stg13/terrain/draw_151.inc"
};
static NJS_CNK_OBJECT draw_150 = {
#include "stg13/terrain/draw_150.inc"
};
static NJS_CNK_OBJECT draw_149 = {
#include "stg13/terrain/draw_149.inc"
};
static NJS_CNK_OBJECT draw_148 = {
#include "stg13/terrain/draw_148.inc"
};
static NJS_CNK_OBJECT draw_147 = {
#include "stg13/terrain/draw_147.inc"
};
static NJS_CNK_OBJECT draw_146 = {
#include "stg13/terrain/draw_146.inc"
};
static NJS_CNK_OBJECT draw_145 = {
#include "stg13/terrain/draw_145.inc"
};
static NJS_CNK_OBJECT draw_144 = {
#include "stg13/terrain/draw_144.inc"
};
static NJS_CNK_OBJECT draw_143 = {
#include "stg13/terrain/draw_143.inc"
};
static NJS_CNK_OBJECT draw_142 = {
#include "stg13/terrain/draw_142.inc"
};
static NJS_CNK_OBJECT draw_141 = {
#include "stg13/terrain/draw_141.inc"
};
static NJS_CNK_OBJECT draw_140 = {
#include "stg13/terrain/draw_140.inc"
};
static NJS_CNK_OBJECT draw_139 = {
#include "stg13/terrain/draw_139.inc"
};
static NJS_CNK_OBJECT draw_138 = {
#include "stg13/terrain/draw_138.inc"
};
static NJS_CNK_OBJECT draw_137 = {
#include "stg13/terrain/draw_137.inc"
};
static NJS_CNK_OBJECT draw_136 = {
#include "stg13/terrain/draw_136.inc"
};
static NJS_CNK_OBJECT draw_135 = {
#include "stg13/terrain/draw_135.inc"
};
static NJS_CNK_OBJECT draw_134 = {
#include "stg13/terrain/draw_134.inc"
};
static NJS_CNK_OBJECT draw_132 = {
#include "stg13/terrain/draw_132.inc"
};
static NJS_CNK_OBJECT draw_131 = {
#include "stg13/terrain/draw_131.inc"
};
static NJS_CNK_OBJECT draw_130 = {
#include "stg13/terrain/draw_130.inc"
};
static NJS_CNK_OBJECT draw_129 = {
#include "stg13/terrain/draw_129.inc"
};
static NJS_CNK_OBJECT draw_128 = {
#include "stg13/terrain/draw_128.inc"
};
static NJS_CNK_OBJECT draw_127 = {
#include "stg13/terrain/draw_127.inc"
};
static NJS_CNK_OBJECT draw_126 = {
#include "stg13/terrain/draw_126.inc"
};
static NJS_CNK_OBJECT draw_125 = {
#include "stg13/terrain/draw_125.inc"
};
static NJS_CNK_OBJECT draw_124 = {
#include "stg13/terrain/draw_124.inc"
};
static NJS_CNK_OBJECT draw_123 = {
#include "stg13/terrain/draw_123.inc"
};
static NJS_CNK_OBJECT draw_122 = {
#include "stg13/terrain/draw_122.inc"
};
static NJS_CNK_OBJECT draw_121 = {
#include "stg13/terrain/draw_121.inc"
};
static NJS_CNK_OBJECT draw_120 = {
#include "stg13/terrain/draw_120.inc"
};
static NJS_CNK_OBJECT draw_119 = {
#include "stg13/terrain/draw_119.inc"
};
static NJS_CNK_OBJECT draw_118 = {
#include "stg13/terrain/draw_118.inc"
};
static NJS_CNK_OBJECT draw_117 = {
#include "stg13/terrain/draw_117.inc"
};
static NJS_CNK_OBJECT draw_116 = {
#include "stg13/terrain/draw_116.inc"
};
static NJS_CNK_OBJECT draw_115 = {
#include "stg13/terrain/draw_115.inc"
};
static NJS_CNK_OBJECT draw_114 = {
#include "stg13/terrain/draw_114.inc"
};
static NJS_CNK_OBJECT draw_113 = {
#include "stg13/terrain/draw_113.inc"
};
static NJS_CNK_OBJECT draw_112 = {
#include "stg13/terrain/draw_112.inc"
};
static NJS_CNK_OBJECT draw_111 = {
#include "stg13/terrain/draw_111.inc"
};
static NJS_CNK_OBJECT draw_110 = {
#include "stg13/terrain/draw_110.inc"
};
static NJS_CNK_OBJECT draw_109 = {
#include "stg13/terrain/draw_109.inc"
};
static NJS_CNK_OBJECT draw_108 = {
#include "stg13/terrain/draw_108.inc"
};
static NJS_CNK_OBJECT draw_107 = {
#include "stg13/terrain/draw_107.inc"
};
static NJS_CNK_OBJECT draw_106 = {
#include "stg13/terrain/draw_106.inc"
};
static NJS_CNK_OBJECT draw_105 = {
#include "stg13/terrain/draw_105.inc"
};
static NJS_CNK_OBJECT draw_104 = {
#include "stg13/terrain/draw_104.inc"
};
static NJS_CNK_OBJECT draw_103 = {
#include "stg13/terrain/draw_103.inc"
};
static NJS_CNK_OBJECT draw_102 = {
#include "stg13/terrain/draw_102.inc"
};
static NJS_CNK_OBJECT draw_101 = {
#include "stg13/terrain/draw_101.inc"
};
static NJS_CNK_OBJECT draw_100 = {
#include "stg13/terrain/draw_100.inc"
};
static NJS_CNK_OBJECT draw_99 = {
#include "stg13/terrain/draw_99.inc"
};
static NJS_CNK_OBJECT draw_98 = {
#include "stg13/terrain/draw_98.inc"
};
static NJS_CNK_OBJECT draw_97 = {
#include "stg13/terrain/draw_97.inc"
};
static NJS_CNK_OBJECT draw_96 = {
#include "stg13/terrain/draw_96.inc"
};
static NJS_CNK_OBJECT draw_95 = {
#include "stg13/terrain/draw_95.inc"
};
static NJS_CNK_OBJECT draw_94 = {
#include "stg13/terrain/draw_94.inc"
};
static NJS_CNK_OBJECT draw_93 = {
#include "stg13/terrain/draw_93.inc"
};
static NJS_CNK_OBJECT draw_92 = {
#include "stg13/terrain/draw_92.inc"
};
static NJS_CNK_OBJECT draw_91 = {
#include "stg13/terrain/draw_91.inc"
};
static NJS_CNK_OBJECT draw_90 = {
#include "stg13/terrain/draw_90.inc"
};
static NJS_CNK_OBJECT draw_89 = {
#include "stg13/terrain/draw_89.inc"
};
static NJS_CNK_OBJECT draw_88 = {
#include "stg13/terrain/draw_88.inc"
};
static NJS_CNK_OBJECT draw_87 = {
#include "stg13/terrain/draw_87.inc"
};
static NJS_CNK_OBJECT draw_86 = {
#include "stg13/terrain/draw_86.inc"
};
static NJS_CNK_OBJECT draw_85 = {
#include "stg13/terrain/draw_85.inc"
};
static NJS_CNK_OBJECT draw_84 = {
#include "stg13/terrain/draw_84.inc"
};
static NJS_CNK_OBJECT draw_83 = {
#include "stg13/terrain/draw_83.inc"
};
static NJS_CNK_OBJECT draw_82 = {
#include "stg13/terrain/draw_82.inc"
};
static NJS_CNK_OBJECT draw_81 = {
#include "stg13/terrain/draw_81.inc"
};
static NJS_CNK_OBJECT draw_79 = {
#include "stg13/terrain/draw_79.inc"
};
static NJS_CNK_OBJECT draw_78 = {
#include "stg13/terrain/draw_78.inc"
};
static NJS_CNK_OBJECT draw_77 = {
#include "stg13/terrain/draw_77.inc"
};
static NJS_CNK_OBJECT draw_76 = {
#include "stg13/terrain/draw_76.inc"
};
static NJS_CNK_OBJECT draw_75 = {
#include "stg13/terrain/draw_75.inc"
};
static NJS_CNK_OBJECT draw_74 = {
#include "stg13/terrain/draw_74.inc"
};
static NJS_CNK_OBJECT draw_72 = {
#include "stg13/terrain/draw_72.inc"
};
static NJS_CNK_OBJECT draw_71 = {
#include "stg13/terrain/draw_71.inc"
};
static NJS_CNK_OBJECT draw_70 = {
#include "stg13/terrain/draw_70.inc"
};
static NJS_CNK_OBJECT draw_69 = {
#include "stg13/terrain/draw_69.inc"
};
static NJS_CNK_OBJECT draw_68 = {
#include "stg13/terrain/draw_68.inc"
};
static NJS_CNK_OBJECT draw_67 = {
#include "stg13/terrain/draw_67.inc"
};
static NJS_CNK_OBJECT draw_66 = {
#include "stg13/terrain/draw_66.inc"
};
static NJS_CNK_OBJECT draw_63 = {
#include "stg13/terrain/draw_63.inc"
};
static NJS_CNK_OBJECT draw_61 = {
#include "stg13/terrain/draw_61.inc"
};
static NJS_CNK_OBJECT draw_60 = {
#include "stg13/terrain/draw_60.inc"
};
static NJS_CNK_OBJECT draw_58 = {
#include "stg13/terrain/draw_58.inc"
};
static NJS_CNK_OBJECT draw_56 = {
#include "stg13/terrain/draw_56.inc"
};
static NJS_CNK_OBJECT draw_55 = {
#include "stg13/terrain/draw_55.inc"
};
static NJS_CNK_OBJECT draw_54 = {
#include "stg13/terrain/draw_54.inc"
};
static NJS_CNK_OBJECT draw_53 = {
#include "stg13/terrain/draw_53.inc"
};
static NJS_CNK_OBJECT draw_52 = {
#include "stg13/terrain/draw_52.inc"
};
static NJS_CNK_OBJECT draw_51 = {
#include "stg13/terrain/draw_51.inc"
};
static NJS_CNK_OBJECT draw_50 = {
#include "stg13/terrain/draw_50.inc"
};
static NJS_CNK_OBJECT draw_49 = {
#include "stg13/terrain/draw_49.inc"
};
static NJS_CNK_OBJECT draw_48 = {
#include "stg13/terrain/draw_48.inc"
};
static NJS_CNK_OBJECT draw_47 = {
#include "stg13/terrain/draw_47.inc"
};
static NJS_CNK_OBJECT draw_46 = {
#include "stg13/terrain/draw_46.inc"
};
static NJS_CNK_OBJECT draw_45 = {
#include "stg13/terrain/draw_45.inc"
};
static NJS_CNK_OBJECT draw_44 = {
#include "stg13/terrain/draw_44.inc"
};
static NJS_CNK_OBJECT draw_43 = {
#include "stg13/terrain/draw_43.inc"
};
static NJS_CNK_OBJECT draw_42 = {
#include "stg13/terrain/draw_42.inc"
};
static NJS_CNK_OBJECT draw_41 = {
#include "stg13/terrain/draw_41.inc"
};
static NJS_CNK_OBJECT draw_40 = {
#include "stg13/terrain/draw_40.inc"
};
static NJS_CNK_OBJECT draw_39 = {
#include "stg13/terrain/draw_39.inc"
};
static NJS_CNK_OBJECT draw_36 = {
#include "stg13/terrain/draw_36.inc"
};
static NJS_CNK_OBJECT draw_35 = {
#include "stg13/terrain/draw_35.inc"
};
static NJS_CNK_OBJECT draw_32 = {
#include "stg13/terrain/draw_32.inc"
};
static NJS_CNK_OBJECT draw_31 = {
#include "stg13/terrain/draw_31.inc"
};
static NJS_CNK_OBJECT draw_30 = {
#include "stg13/terrain/draw_30.inc"
};
static NJS_CNK_OBJECT draw_29 = {
#include "stg13/terrain/draw_29.inc"
};
static NJS_CNK_OBJECT draw_28 = {
#include "stg13/terrain/draw_28.inc"
};
static NJS_CNK_OBJECT draw_27 = {
#include "stg13/terrain/draw_27.inc"
};
static NJS_CNK_OBJECT draw_26 = {
#include "stg13/terrain/draw_26.inc"
};
static NJS_CNK_OBJECT draw_25 = {
#include "stg13/terrain/draw_25.inc"
};
static NJS_CNK_OBJECT draw_24 = {
#include "stg13/terrain/draw_24.inc"
};
static NJS_CNK_OBJECT draw_23 = {
#include "stg13/terrain/draw_23.inc"
};
static NJS_CNK_OBJECT draw_21 = {
#include "stg13/terrain/draw_21.inc"
};
static NJS_CNK_OBJECT draw_20 = {
#include "stg13/terrain/draw_20.inc"
};
static NJS_CNK_OBJECT draw_19 = {
#include "stg13/terrain/draw_19.inc"
};
static NJS_CNK_OBJECT draw_18 = {
#include "stg13/terrain/draw_18.inc"
};
static NJS_CNK_OBJECT draw_17 = {
#include "stg13/terrain/draw_17.inc"
};
static NJS_CNK_OBJECT draw_16 = {
#include "stg13/terrain/draw_16.inc"
};
static NJS_CNK_OBJECT draw_15 = {
#include "stg13/terrain/draw_15.inc"
};
static NJS_CNK_OBJECT draw_14 = {
#include "stg13/terrain/draw_14.inc"
};
static NJS_CNK_OBJECT draw_13 = {
#include "stg13/terrain/draw_13.inc"
};
static NJS_CNK_OBJECT draw_12 = {
#include "stg13/terrain/draw_12.inc"
};
static NJS_CNK_OBJECT draw_11 = {
#include "stg13/terrain/draw_11.inc"
};
static NJS_CNK_OBJECT draw_9 = {
#include "stg13/terrain/draw_9.inc"
};
static NJS_CNK_OBJECT draw_8 = {
#include "stg13/terrain/draw_8.inc"
};
static NJS_CNK_OBJECT draw_7 = {
#include "stg13/terrain/draw_7.inc"
};
static NJS_CNK_OBJECT draw_6 = {
#include "stg13/terrain/draw_6.inc"
};
static NJS_CNK_OBJECT draw_5 = {
#include "stg13/terrain/draw_5.inc"
};
static NJS_CNK_OBJECT draw_4 = {
#include "stg13/terrain/draw_4.inc"
};
static NJS_CNK_OBJECT draw_3 = {
#include "stg13/terrain/draw_3.inc"
};
static NJS_CNK_OBJECT draw_0 = {
#include "stg13/terrain/draw_0.inc"
};
