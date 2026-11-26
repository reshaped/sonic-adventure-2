#include <ninja.h>
#include <sg_xpt.h>

#include "collision/world.h"
#include "player/player.h"
#include "shared/motions.h"
#include "stg14/radical_highway.h"

/* the land's file names, which its land table and texture list point at. each
 * starts on a 4-byte boundary */
const char radical_highway_land_filenames[881] = {
#include "stg14/terrain/radical_highway_land_filenames.inc"
};

/* STG14.PRS's motions, their registry, land, texture lists, Basic models and
 * chunk models, from the disc. its D lies between radical_highway.c and
 * objects.c */

/* the data up to the last that a pointer names ahead of its definition, as
 * tentative definitions in the disc's order: GCC places each datum at its
 * first definition */
static NJS_MKEY_A upgrade_radical_highway_motion_joint_4_angles[16];
static NJS_MKEY_A upgrade_radical_highway_motion_joint_7_angles[16];
static NJS_MKEY_A upgrade_radical_highway_motion_joint_8_angles[7];
static NJS_MKEY_A upgrade_radical_highway_motion_joint_9_angles[12];
static NJS_MKEY_A upgrade_radical_highway_motion_joint_13_angles[16];
static NJS_MKEY_A upgrade_radical_highway_motion_joint_14_angles[2];
static NJS_MKEY_A upgrade_radical_highway_motion_joint_17_angles[16];
static NJS_MKEY_A upgrade_radical_highway_motion_joint_18_angles[8];
static NJS_MKEY_A upgrade_radical_highway_motion_joint_19_angles[14];
static NJS_MKEY_A upgrade_radical_highway_motion_joint_23_angles[16];
static NJS_MKEY_A upgrade_radical_highway_motion_joint_24_angles[2];
static NJS_MKEY_A upgrade_radical_highway_motion_joint_27_angles[16];
static NJS_MKEY_A upgrade_radical_highway_motion_joint_28_angles[16];
static NJS_MKEY_A upgrade_radical_highway_motion_joint_29_angles[16];
static NJS_MKEY_A upgrade_radical_highway_motion_joint_31_angles[2];
static NJS_MKEY_A upgrade_radical_highway_motion_joint_32_angles[16];
static NJS_MKEY_A upgrade_radical_highway_motion_joint_33_angles[13];
static NJS_MKEY_A upgrade_radical_highway_motion_joint_37_angles[16];
static NJS_MKEY_A upgrade_radical_highway_motion_joint_38_angles[16];
static NJS_MKEY_A upgrade_radical_highway_motion_joint_39_angles[16];
static NJS_MKEY_A upgrade_radical_highway_motion_joint_41_angles[16];
static NJS_MKEY_A upgrade_radical_highway_motion_joint_44_angles[16];
static NJS_MKEY_A upgrade_radical_highway_motion_joint_51_angles[16];
static NJS_MKEY_A upgrade_radical_highway_motion_joint_53_angles[16];
static NJS_MKEY_A upgrade_radical_highway_motion_joint_56_angles[16];
static NJS_MKEY_A upgrade_radical_highway_motion_joint_57_angles[16];
static NJS_MKEY_A upgrade_radical_highway_motion_joint_58_angles[16];
static NJS_MKEY_A upgrade_radical_highway_motion_joint_60_angles[16];
static NJS_MDATA2 upgrade_radical_highway_motion_tracks[62];
static NJS_MOTION upgrade_radical_highway_motion;
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
struct motion_entry module_motions[9];
static struct land_entry entries[1128];
struct land_table radical_highway_land;
static NJS_TEXNAME textures[64];
NJS_TEXLIST radical_highway_terrain_textures;
static NJS_MATERIAL collision_0_materials[1];
static Sint16 collision_0_faces[9];
static NJS_MESHSET collision_0_meshsets[1];
static NJS_POINT3 collision_0_points[8];
static NJS_MODEL collision_0_model;
static NJS_OBJECT collision_0;
static NJS_MATERIAL collision_6_materials[1];
static Sint16 collision_6_faces[21];
static NJS_MESHSET collision_6_meshsets[1];
static NJS_POINT3 collision_6_points[17];
static NJS_MODEL collision_6_model;
static NJS_OBJECT collision_6;
static NJS_MATERIAL collision_8_materials[1];
static Sint16 collision_8_faces[21];
static NJS_MESHSET collision_8_meshsets[1];
static NJS_POINT3 collision_8_points[17];
static NJS_MODEL collision_8_model;
static NJS_OBJECT collision_8;
static NJS_MATERIAL collision_10_materials[1];
static Sint16 collision_10_faces[102];
static NJS_MESHSET collision_10_meshsets[1];
static NJS_POINT3 collision_10_points[48];
static NJS_MODEL collision_10_model;
static NJS_OBJECT collision_10;
static NJS_MATERIAL collision_11_materials[1];
static Sint16 collision_11_faces[42];
static NJS_MESHSET collision_11_meshsets[1];
static NJS_POINT3 collision_11_points[24];
static NJS_MODEL collision_11_model;
static NJS_OBJECT collision_11;
static NJS_MATERIAL collision_15_materials[1];
static Sint16 collision_15_faces[30];
static NJS_MESHSET collision_15_meshsets[1];
static NJS_POINT3 collision_15_points[21];
static NJS_MODEL collision_15_model;
static NJS_OBJECT collision_15;
static NJS_MATERIAL collision_17_materials[1];
static Sint16 collision_17_faces[14];
static NJS_MESHSET collision_17_meshsets[1];
static NJS_POINT3 collision_17_points[9];
static NJS_MODEL collision_17_model;
static NJS_OBJECT collision_17;
static NJS_MATERIAL collision_18_materials[1];
static Sint16 collision_18_faces[62];
static NJS_MESHSET collision_18_meshsets[1];
static NJS_POINT3 collision_18_points[32];
static NJS_MODEL collision_18_model;
static NJS_OBJECT collision_18;
static NJS_MATERIAL collision_19_materials[1];
static Sint16 collision_19_faces[31];
static NJS_MESHSET collision_19_meshsets[1];
static NJS_POINT3 collision_19_points[16];
static NJS_MODEL collision_19_model;
static NJS_OBJECT collision_19;
static NJS_MATERIAL collision_20_materials[1];
static Sint16 collision_20_faces[28];
static NJS_COLOR collision_20_basic_uvs[24];
static NJS_MESHSET collision_20_meshsets[1];
static NJS_POINT3 collision_20_points[16];
static NJS_MODEL collision_20_model;
static NJS_OBJECT collision_20;
static NJS_MATERIAL collision_21_materials[1];
static Sint16 collision_21_faces[18];
static NJS_MESHSET collision_21_meshsets[1];
static NJS_POINT3 collision_21_points[12];
static NJS_MODEL collision_21_model;
static NJS_OBJECT collision_21;
static NJS_MATERIAL collision_22_materials[1];
static Sint16 collision_22_faces[88];
static NJS_COLOR collision_22_basic_uvs[80];
static NJS_MESHSET collision_22_meshsets[1];
static NJS_POINT3 collision_22_points[45];
static NJS_MODEL collision_22_model;
static NJS_OBJECT collision_22;
static NJS_MATERIAL collision_23_materials[1];
static Sint16 collision_23_faces[105];
static NJS_MESHSET collision_23_meshsets[1];
static NJS_POINT3 collision_23_points[56];
static NJS_MODEL collision_23_model;
static NJS_OBJECT collision_23;
static NJS_MATERIAL collision_25_materials[1];
static Sint16 collision_25_faces[14];
static NJS_MESHSET collision_25_meshsets[1];
static NJS_POINT3 collision_25_points[12];
static NJS_MODEL collision_25_model;
static NJS_OBJECT collision_25;
static NJS_MATERIAL collision_26_materials[1];
static Sint16 collision_26_faces[18];
static NJS_MESHSET collision_26_meshsets[1];
static NJS_POINT3 collision_26_points[16];
static NJS_MODEL collision_26_model;
static NJS_OBJECT collision_26;
static NJS_MATERIAL collision_27_materials[1];
static Sint16 collision_27_faces[14];
static NJS_MESHSET collision_27_meshsets[1];
static NJS_POINT3 collision_27_points[12];
static NJS_MODEL collision_27_model;
static NJS_OBJECT collision_27;
static NJS_MATERIAL collision_28_materials[1];
static Sint16 collision_28_faces[9];
static NJS_MESHSET collision_28_meshsets[1];
static NJS_POINT3 collision_28_points[8];
static NJS_MODEL collision_28_model;
static NJS_OBJECT collision_28;
static NJS_MATERIAL collision_29_materials[1];
static Sint16 collision_29_faces[5];
static NJS_MESHSET collision_29_meshsets[1];
static NJS_POINT3 collision_29_points[4];
static NJS_MODEL collision_29_model;
static NJS_OBJECT collision_29;
static NJS_MATERIAL collision_30_materials[1];
static Sint16 collision_30_faces[21];
static NJS_MESHSET collision_30_meshsets[1];
static NJS_POINT3 collision_30_points[11];
static NJS_MODEL collision_30_model;
static NJS_OBJECT collision_30;
static NJS_MATERIAL collision_31_materials[1];
static Sint16 collision_31_faces[14];
static NJS_MESHSET collision_31_meshsets[1];
static NJS_POINT3 collision_31_points[9];
static NJS_MODEL collision_31_model;
static NJS_OBJECT collision_31;
static NJS_MATERIAL collision_33_materials[1];
static Sint16 collision_33_faces[10];
static NJS_MESHSET collision_33_meshsets[1];
static NJS_POINT3 collision_33_points[8];
static NJS_MODEL collision_33_model;
static NJS_OBJECT collision_33;
static NJS_MATERIAL collision_43_materials[1];
static Sint16 collision_43_faces[7];
static NJS_MESHSET collision_43_meshsets[1];
static NJS_POINT3 collision_43_points[6];
static NJS_MODEL collision_43_model;
static NJS_OBJECT collision_43;
static NJS_MATERIAL collision_44_materials[1];
static Sint16 collision_44_faces[63];
static NJS_MESHSET collision_44_meshsets[1];
static NJS_POINT3 collision_44_points[40];
static NJS_MODEL collision_44_model;
static NJS_OBJECT collision_44;
static NJS_MATERIAL collision_45_materials[1];
static Sint16 collision_45_faces[10];
static NJS_MESHSET collision_45_meshsets[1];
static NJS_POINT3 collision_45_points[8];
static NJS_MODEL collision_45_model;
static NJS_OBJECT collision_45;
static NJS_MATERIAL collision_52_materials[1];
static Sint16 collision_52_faces[38];
static NJS_MESHSET collision_52_meshsets[1];
static NJS_POINT3 collision_52_points[36];
static NJS_MODEL collision_52_model;
static NJS_OBJECT collision_52;
static NJS_MATERIAL collision_57_materials[1];
static Sint16 collision_57_faces[14];
static NJS_MESHSET collision_57_meshsets[1];
static NJS_POINT3 collision_57_points[12];
static NJS_MODEL collision_57_model;
static NJS_OBJECT collision_57;
static NJS_MATERIAL collision_63_materials[1];
static Sint16 collision_63_faces[14];
static NJS_MESHSET collision_63_meshsets[1];
static NJS_POINT3 collision_63_points[12];
static NJS_MODEL collision_63_model;
static NJS_OBJECT collision_63;
static NJS_MATERIAL collision_68_materials[1];
static Sint16 collision_68_faces[14];
static NJS_MESHSET collision_68_meshsets[1];
static NJS_POINT3 collision_68_points[12];
static NJS_MODEL collision_68_model;
static NJS_OBJECT collision_68;
static NJS_MATERIAL collision_69_materials[1];
static Sint16 collision_69_faces[120];
static NJS_MESHSET collision_69_meshsets[1];
static NJS_POINT3 collision_69_points[81];
static NJS_MODEL collision_69_model;
static NJS_OBJECT collision_69;
static NJS_MATERIAL collision_70_materials[1];
static Sint16 collision_70_faces[31];
static NJS_MESHSET collision_70_meshsets[1];
static NJS_POINT3 collision_70_points[30];
static NJS_MODEL collision_70_model;
static NJS_OBJECT collision_70;
static NJS_MATERIAL collision_71_materials[1];
static Sint16 collision_71_faces[123];
static NJS_MESHSET collision_71_meshsets[1];
static NJS_POINT3 collision_71_points[81];
static NJS_MODEL collision_71_model;
static NJS_OBJECT collision_71;
static NJS_MATERIAL collision_72_materials[1];
static Sint16 collision_72_faces[20];
static NJS_MESHSET collision_72_meshsets[1];
static NJS_POINT3 collision_72_points[12];
static NJS_MODEL collision_72_model;
static NJS_OBJECT collision_72;
static NJS_MATERIAL collision_74_materials[1];
static Sint16 collision_74_faces[19];
static NJS_MESHSET collision_74_meshsets[1];
static NJS_POINT3 collision_74_points[16];
static NJS_MODEL collision_74_model;
static NJS_OBJECT collision_74;
static NJS_MATERIAL collision_76_materials[1];
static Sint16 collision_76_faces[60];
static NJS_MESHSET collision_76_meshsets[1];
static NJS_POINT3 collision_76_points[40];
static NJS_MODEL collision_76_model;
static NJS_OBJECT collision_76;
static NJS_MATERIAL collision_77_materials[1];
static Sint16 collision_77_faces[14];
static NJS_MESHSET collision_77_meshsets[1];
static NJS_POINT3 collision_77_points[12];
static NJS_MODEL collision_77_model;
static NJS_OBJECT collision_77;
static NJS_MATERIAL collision_78_materials[1];
static Sint16 collision_78_faces[10];
static NJS_MESHSET collision_78_meshsets[1];
static NJS_POINT3 collision_78_points[8];
static NJS_MODEL collision_78_model;
static NJS_OBJECT collision_78;
static NJS_MATERIAL collision_79_materials[1];
static Sint16 collision_79_faces[14];
static NJS_MESHSET collision_79_meshsets[1];
static NJS_POINT3 collision_79_points[12];
static NJS_MODEL collision_79_model;
static NJS_OBJECT collision_79;
static NJS_MATERIAL collision_80_materials[1];
static Sint16 collision_80_faces[14];
static NJS_MESHSET collision_80_meshsets[1];
static NJS_POINT3 collision_80_points[12];
static NJS_MODEL collision_80_model;
static NJS_OBJECT collision_80;
static NJS_MATERIAL collision_81_materials[1];
static Sint16 collision_81_faces[14];
static NJS_MESHSET collision_81_meshsets[1];
static NJS_POINT3 collision_81_points[12];
static NJS_MODEL collision_81_model;
static NJS_OBJECT collision_81;
static NJS_MATERIAL collision_82_materials[1];
static Sint16 collision_82_faces[14];
static NJS_MESHSET collision_82_meshsets[1];
static NJS_POINT3 collision_82_points[12];
static NJS_MODEL collision_82_model;
static NJS_OBJECT collision_82;
static NJS_MATERIAL collision_83_materials[1];
static Sint16 collision_83_faces[14];
static NJS_MESHSET collision_83_meshsets[1];
static NJS_POINT3 collision_83_points[12];
static NJS_MODEL collision_83_model;
static NJS_OBJECT collision_83;
static NJS_MATERIAL collision_85_materials[1];
static Sint16 collision_85_faces[27];
static NJS_MESHSET collision_85_meshsets[1];
static NJS_POINT3 collision_85_points[20];
static NJS_MODEL collision_85_model;
static NJS_OBJECT collision_85;
static NJS_MATERIAL collision_86_materials[1];
static Sint16 collision_86_faces[22];
static NJS_MESHSET collision_86_meshsets[1];
static NJS_POINT3 collision_86_points[20];
static NJS_MODEL collision_86_model;
static NJS_OBJECT collision_86;
static NJS_MATERIAL collision_87_materials[1];
static Sint16 collision_87_faces[18];
static NJS_MESHSET collision_87_meshsets[1];
static NJS_POINT3 collision_87_points[16];
static NJS_MODEL collision_87_model;
static NJS_OBJECT collision_87;
static NJS_MATERIAL collision_90_materials[1];
static Sint16 collision_90_faces[11];
static NJS_MESHSET collision_90_meshsets[1];
static NJS_POINT3 collision_90_points[10];
static NJS_MODEL collision_90_model;
static NJS_OBJECT collision_90;
static NJS_MATERIAL collision_92_materials[1];
static Sint16 collision_92_faces[18];
static NJS_MESHSET collision_92_meshsets[1];
static NJS_POINT3 collision_92_points[16];
static NJS_MODEL collision_92_model;
static NJS_OBJECT collision_92;
static NJS_MATERIAL collision_95_materials[1];
static Sint16 collision_95_faces[104];
static NJS_MESHSET collision_95_meshsets[1];
static NJS_POINT3 collision_95_points[51];
static NJS_MODEL collision_95_model;
static NJS_OBJECT collision_95;
static NJS_MATERIAL collision_97_materials[1];
static Sint16 collision_97_faces[31];
static NJS_MESHSET collision_97_meshsets[1];
static NJS_POINT3 collision_97_points[30];
static NJS_MODEL collision_97_model;
static NJS_OBJECT collision_97;
static NJS_MATERIAL collision_98_materials[1];
static Sint16 collision_98_faces[34];
static NJS_MESHSET collision_98_meshsets[1];
static NJS_POINT3 collision_98_points[32];
static NJS_MODEL collision_98_model;
static NJS_OBJECT collision_98;
static NJS_MATERIAL collision_99_materials[1];
static Sint16 collision_99_faces[10];
static NJS_MESHSET collision_99_meshsets[1];
static NJS_POINT3 collision_99_points[8];
static NJS_MODEL collision_99_model;
static NJS_OBJECT collision_99;
static NJS_MATERIAL collision_100_materials[1];
static Sint16 collision_100_faces[30];
static NJS_MESHSET collision_100_meshsets[1];
static NJS_POINT3 collision_100_points[28];
static NJS_MODEL collision_100_model;
static NJS_OBJECT collision_100;
static NJS_MATERIAL collision_104_materials[1];
static Sint16 collision_104_faces[38];
static NJS_MESHSET collision_104_meshsets[1];
static NJS_POINT3 collision_104_points[36];
static NJS_MODEL collision_104_model;
static NJS_OBJECT collision_104;
static NJS_MATERIAL collision_105_materials[1];
static Sint16 collision_105_faces[54];
static NJS_MESHSET collision_105_meshsets[1];
static NJS_POINT3 collision_105_points[52];
static NJS_MODEL collision_105_model;
static NJS_OBJECT collision_105;
static NJS_MATERIAL collision_107_materials[1];
static Sint16 collision_107_faces[38];
static NJS_MESHSET collision_107_meshsets[1];
static NJS_POINT3 collision_107_points[36];
static NJS_MODEL collision_107_model;
static NJS_OBJECT collision_107;
static NJS_MATERIAL collision_110_materials[1];
static Sint16 collision_110_faces[38];
static NJS_MESHSET collision_110_meshsets[1];
static NJS_POINT3 collision_110_points[36];
static NJS_MODEL collision_110_model;
static NJS_OBJECT collision_110;
static NJS_MATERIAL collision_112_materials[1];
static Sint16 collision_112_faces[66];
static NJS_MESHSET collision_112_meshsets[1];
static NJS_POINT3 collision_112_points[64];
static NJS_MODEL collision_112_model;
static NJS_OBJECT collision_112;
static NJS_MATERIAL collision_114_materials[1];
static Sint16 collision_114_faces[28];
static NJS_MESHSET collision_114_meshsets[1];
static NJS_POINT3 collision_114_points[18];
static NJS_MODEL collision_114_model;
static NJS_OBJECT collision_114;
static NJS_MATERIAL collision_115_materials[1];
static Sint16 collision_115_faces[66];
static NJS_MESHSET collision_115_meshsets[1];
static NJS_POINT3 collision_115_points[64];
static NJS_MODEL collision_115_model;
static NJS_OBJECT collision_115;
static NJS_MATERIAL collision_117_materials[1];
static Sint16 collision_117_faces[132];
static NJS_MESHSET collision_117_meshsets[1];
static NJS_POINT3 collision_117_points[78];
static NJS_MODEL collision_117_model;
static NJS_OBJECT collision_117;
static NJS_MATERIAL collision_119_materials[1];
static Sint16 collision_119_faces[91];
static NJS_COLOR collision_119_basic_uvs[81];
static NJS_MESHSET collision_119_meshsets[1];
static NJS_POINT3 collision_119_points[44];
static NJS_MODEL collision_119_model;
static NJS_OBJECT collision_119;
static NJS_MATERIAL collision_120_materials[1];
static Sint16 collision_120_faces[54];
static NJS_MESHSET collision_120_meshsets[1];
static NJS_POINT3 collision_120_points[52];
static NJS_MODEL collision_120_model;
static NJS_OBJECT collision_120;
static NJS_MATERIAL collision_122_materials[1];
static Sint16 collision_122_faces[65];
static NJS_MESHSET collision_122_meshsets[1];
static NJS_POINT3 collision_122_points[36];
static NJS_MODEL collision_122_model;
static NJS_OBJECT collision_122;
static NJS_MATERIAL collision_123_materials[1];
static Sint16 collision_123_faces[7];
static NJS_MESHSET collision_123_meshsets[1];
static NJS_POINT3 collision_123_points[6];
static NJS_MODEL collision_123_model;
static NJS_OBJECT collision_123;
static NJS_MATERIAL collision_124_materials[1];
static Sint16 collision_124_faces[17];
static NJS_MESHSET collision_124_meshsets[1];
static NJS_POINT3 collision_124_points[12];
static NJS_MODEL collision_124_model;
static NJS_OBJECT collision_124;
static NJS_MATERIAL collision_130_materials[1];
static Sint16 collision_130_faces[34];
static NJS_MESHSET collision_130_meshsets[1];
static NJS_POINT3 collision_130_points[32];
static NJS_MODEL collision_130_model;
static NJS_OBJECT collision_130;
static NJS_MATERIAL collision_134_materials[1];
static Sint16 collision_134_faces[18];
static NJS_MESHSET collision_134_meshsets[1];
static NJS_POINT3 collision_134_points[16];
static NJS_MODEL collision_134_model;
static NJS_OBJECT collision_134;
static NJS_MATERIAL collision_136_materials[1];
static Sint16 collision_136_faces[22];
static NJS_MESHSET collision_136_meshsets[1];
static NJS_POINT3 collision_136_points[20];
static NJS_MODEL collision_136_model;
static NJS_OBJECT collision_136;
static NJS_MATERIAL collision_139_materials[1];
static Sint16 collision_139_faces[22];
static NJS_MESHSET collision_139_meshsets[1];
static NJS_POINT3 collision_139_points[20];
static NJS_MODEL collision_139_model;
static NJS_OBJECT collision_139;
static NJS_MATERIAL collision_142_materials[1];
static Sint16 collision_142_faces[10];
static NJS_MESHSET collision_142_meshsets[1];
static NJS_POINT3 collision_142_points[8];
static NJS_MODEL collision_142_model;
static NJS_OBJECT collision_142;
static NJS_MATERIAL collision_143_materials[1];
static Sint16 collision_143_faces[10];
static NJS_MESHSET collision_143_meshsets[1];
static NJS_POINT3 collision_143_points[8];
static NJS_MODEL collision_143_model;
static NJS_OBJECT collision_143;
static NJS_MATERIAL collision_144_materials[1];
static Sint16 collision_144_faces[34];
static NJS_MESHSET collision_144_meshsets[1];
static NJS_POINT3 collision_144_points[32];
static NJS_MODEL collision_144_model;
static NJS_OBJECT collision_144;
static NJS_MATERIAL collision_146_materials[1];
static Sint16 collision_146_faces[34];
static NJS_MESHSET collision_146_meshsets[1];
static NJS_POINT3 collision_146_points[32];
static NJS_MODEL collision_146_model;
static NJS_OBJECT collision_146;
static NJS_MATERIAL collision_148_materials[1];
static Sint16 collision_148_faces[19];
static NJS_MESHSET collision_148_meshsets[1];
static NJS_POINT3 collision_148_points[16];
static NJS_MODEL collision_148_model;
static NJS_OBJECT collision_148;
static NJS_MATERIAL collision_149_materials[1];
static Sint16 collision_149_faces[19];
static NJS_MESHSET collision_149_meshsets[1];
static NJS_POINT3 collision_149_points[16];
static NJS_MODEL collision_149_model;
static NJS_OBJECT collision_149;
static NJS_MATERIAL collision_151_materials[1];
static Sint16 collision_151_faces[17];
static NJS_MESHSET collision_151_meshsets[1];
static NJS_POINT3 collision_151_points[8];
static NJS_MODEL collision_151_model;
static NJS_OBJECT collision_151;
static NJS_MATERIAL collision_152_materials[1];
static Sint16 collision_152_faces[7];
static NJS_MESHSET collision_152_meshsets[1];
static NJS_POINT3 collision_152_points[6];
static NJS_MODEL collision_152_model;
static NJS_OBJECT collision_152;
static NJS_MATERIAL collision_153_materials[1];
static Sint16 collision_153_faces[9];
static NJS_MESHSET collision_153_meshsets[1];
static NJS_POINT3 collision_153_points[8];
static NJS_MODEL collision_153_model;
static NJS_OBJECT collision_153;
static NJS_MATERIAL collision_155_materials[1];
static Sint16 collision_155_faces[10];
static NJS_MESHSET collision_155_meshsets[1];
static NJS_POINT3 collision_155_points[6];
static NJS_MODEL collision_155_model;
static NJS_OBJECT collision_155;
static NJS_MATERIAL collision_156_materials[1];
static Sint16 collision_156_faces[7];
static NJS_MESHSET collision_156_meshsets[1];
static NJS_POINT3 collision_156_points[6];
static NJS_MODEL collision_156_model;
static NJS_OBJECT collision_156;
static NJS_MATERIAL collision_157_materials[1];
static Sint16 collision_157_faces[45];
static NJS_MESHSET collision_157_meshsets[1];
static NJS_POINT3 collision_157_points[26];
static NJS_MODEL collision_157_model;
static NJS_OBJECT collision_157;
static NJS_MATERIAL collision_165_materials[1];
static Sint16 collision_165_faces[14];
static NJS_MESHSET collision_165_meshsets[1];
static NJS_POINT3 collision_165_points[8];
static NJS_MODEL collision_165_model;
static NJS_OBJECT collision_165;
static NJS_MATERIAL collision_167_materials[1];
static Sint16 collision_167_faces[15];
static NJS_MESHSET collision_167_meshsets[1];
static NJS_POINT3 collision_167_points[12];
static NJS_MODEL collision_167_model;
static NJS_OBJECT collision_167;
static NJS_MATERIAL collision_177_materials[1];
static Sint16 collision_177_faces[224];
static NJS_MESHSET collision_177_meshsets[1];
static NJS_POINT3 collision_177_points[100];
static NJS_MODEL collision_177_model;
static NJS_OBJECT collision_177;
static NJS_MATERIAL collision_179_materials[1];
static Sint16 collision_179_faces[39];
static NJS_MESHSET collision_179_meshsets[1];
static NJS_POINT3 collision_179_points[24];
static NJS_MODEL collision_179_model;
static NJS_OBJECT collision_179;
static NJS_MATERIAL collision_189_materials[1];
static Sint16 collision_189_faces[288];
static NJS_MESHSET collision_189_meshsets[1];
static NJS_POINT3 collision_189_points[152];
static NJS_MODEL collision_189_model;
static NJS_OBJECT collision_189;
static NJS_MATERIAL collision_190_materials[1];
static Sint16 collision_190_faces[7];
static NJS_MESHSET collision_190_meshsets[1];
static NJS_POINT3 collision_190_points[6];
static NJS_MODEL collision_190_model;
static NJS_OBJECT collision_190;
static NJS_MATERIAL collision_191_materials[1];
static Sint16 collision_191_faces[33];
static NJS_MESHSET collision_191_meshsets[1];
static NJS_POINT3 collision_191_points[26];
static NJS_MODEL collision_191_model;
static NJS_OBJECT collision_191;
static NJS_MATERIAL collision_193_materials[1];
static Sint16 collision_193_faces[17];
static NJS_MESHSET collision_193_meshsets[1];
static NJS_POINT3 collision_193_points[9];
static NJS_MODEL collision_193_model;
static NJS_OBJECT collision_193;
static NJS_MATERIAL collision_194_materials[1];
static Sint16 collision_194_faces[31];
static NJS_MESHSET collision_194_meshsets[1];
static NJS_POINT3 collision_194_points[16];
static NJS_MODEL collision_194_model;
static NJS_OBJECT collision_194;
static NJS_MATERIAL collision_195_materials[1];
static Sint16 collision_195_faces[31];
static NJS_MESHSET collision_195_meshsets[1];
static NJS_POINT3 collision_195_points[16];
static NJS_MODEL collision_195_model;
static NJS_OBJECT collision_195;
static NJS_MATERIAL collision_196_materials[1];
static Sint16 collision_196_faces[31];
static NJS_MESHSET collision_196_meshsets[1];
static NJS_POINT3 collision_196_points[16];
static NJS_MODEL collision_196_model;
static NJS_OBJECT collision_196;
static NJS_MATERIAL collision_197_materials[1];
static Sint16 collision_197_faces[31];
static NJS_MESHSET collision_197_meshsets[1];
static NJS_POINT3 collision_197_points[16];
static NJS_MODEL collision_197_model;
static NJS_OBJECT collision_197;
static NJS_MATERIAL collision_198_materials[1];
static Sint16 collision_198_faces[31];
static NJS_MESHSET collision_198_meshsets[1];
static NJS_POINT3 collision_198_points[16];
static NJS_MODEL collision_198_model;
static NJS_OBJECT collision_198;
static NJS_MATERIAL collision_199_materials[1];
static Sint16 collision_199_faces[31];
static NJS_MESHSET collision_199_meshsets[1];
static NJS_POINT3 collision_199_points[16];
static NJS_MODEL collision_199_model;
static NJS_OBJECT collision_199;
static NJS_MATERIAL collision_200_materials[1];
static Sint16 collision_200_faces[31];
static NJS_MESHSET collision_200_meshsets[1];
static NJS_POINT3 collision_200_points[16];
static NJS_MODEL collision_200_model;
static NJS_OBJECT collision_200;
static NJS_MATERIAL collision_201_materials[1];
static Sint16 collision_201_faces[31];
static NJS_MESHSET collision_201_meshsets[1];
static NJS_POINT3 collision_201_points[16];
static NJS_MODEL collision_201_model;
static NJS_OBJECT collision_201;
static NJS_MATERIAL collision_202_materials[1];
static Sint16 collision_202_faces[31];
static NJS_MESHSET collision_202_meshsets[1];
static NJS_POINT3 collision_202_points[16];
static NJS_MODEL collision_202_model;
static NJS_OBJECT collision_202;
static NJS_MATERIAL collision_203_materials[1];
static Sint16 collision_203_faces[31];
static NJS_MESHSET collision_203_meshsets[1];
static NJS_POINT3 collision_203_points[16];
static NJS_MODEL collision_203_model;
static NJS_OBJECT collision_203;
static NJS_MATERIAL collision_204_materials[2];
static Sint16 collision_204_faces[12];
static Sint16 collision_204_faces_1[23];
static NJS_MESHSET collision_204_meshsets[2];
static NJS_POINT3 collision_204_points[14];
static NJS_MODEL collision_204_model;
static NJS_OBJECT collision_204;
static NJS_MATERIAL collision_207_materials[1];
static Sint16 collision_207_faces[63];
static NJS_MESHSET collision_207_meshsets[1];
static NJS_POINT3 collision_207_points[62];
static NJS_MODEL collision_207_model;
static NJS_OBJECT collision_207;
static NJS_MATERIAL collision_208_materials[1];
static Sint16 collision_208_faces[28];
static NJS_MESHSET collision_208_meshsets[1];
static NJS_POINT3 collision_208_points[16];
static NJS_MODEL collision_208_model;
static NJS_OBJECT collision_208;
static NJS_MATERIAL collision_215_materials[2];
static Sint16 collision_215_faces[99];
static NJS_MESHSET collision_215_meshsets[1];
static NJS_POINT3 collision_215_points[44];
static NJS_MODEL collision_215_model;
static NJS_OBJECT collision_215;
static NJS_MATERIAL collision_218_materials[1];
static Sint16 collision_218_faces[22];
static NJS_MESHSET collision_218_meshsets[1];
static NJS_POINT3 collision_218_points[12];
static NJS_MODEL collision_218_model;
static NJS_OBJECT collision_218;
static NJS_MATERIAL collision_220_materials[1];
static Sint16 collision_220_faces[57];
static NJS_MESHSET collision_220_meshsets[1];
static NJS_POINT3 collision_220_points[54];
static NJS_MODEL collision_220_model;
static NJS_OBJECT collision_220;
static NJS_MATERIAL collision_225_materials[1];
static Sint16 collision_225_faces[35];
static NJS_MESHSET collision_225_meshsets[1];
static NJS_POINT3 collision_225_points[24];
static NJS_MODEL collision_225_model;
static NJS_OBJECT collision_225;
static NJS_MATERIAL collision_231_materials[1];
static Sint16 collision_231_faces[7];
static NJS_MESHSET collision_231_meshsets[1];
static NJS_POINT3 collision_231_points[6];
static NJS_MODEL collision_231_model;
static NJS_OBJECT collision_231;
static NJS_MATERIAL collision_232_materials[1];
static Sint16 collision_232_faces[7];
static NJS_MESHSET collision_232_meshsets[1];
static NJS_POINT3 collision_232_points[6];
static NJS_MODEL collision_232_model;
static NJS_OBJECT collision_232;
static NJS_MATERIAL collision_233_materials[1];
static Sint16 collision_233_faces[7];
static NJS_MESHSET collision_233_meshsets[1];
static NJS_POINT3 collision_233_points[6];
static NJS_MODEL collision_233_model;
static NJS_OBJECT collision_233;
static NJS_MATERIAL collision_234_materials[1];
static Sint16 collision_234_faces[7];
static NJS_MESHSET collision_234_meshsets[1];
static NJS_POINT3 collision_234_points[6];
static NJS_MODEL collision_234_model;
static NJS_OBJECT collision_234;
static NJS_MATERIAL collision_235_materials[1];
static Sint16 collision_235_faces[28];
static NJS_MESHSET collision_235_meshsets[1];
static NJS_POINT3 collision_235_points[21];
static NJS_MODEL collision_235_model;
static NJS_OBJECT collision_235;
static NJS_MATERIAL collision_236_materials[1];
static Sint16 collision_236_faces[136];
static NJS_MESHSET collision_236_meshsets[1];
static NJS_POINT3 collision_236_points[60];
static NJS_MODEL collision_236_model;
static NJS_OBJECT collision_236;
static NJS_MATERIAL collision_239_materials[1];
static Sint16 collision_239_faces[36];
static NJS_MESHSET collision_239_meshsets[1];
static NJS_POINT3 collision_239_points[19];
static NJS_MODEL collision_239_model;
static NJS_OBJECT collision_239;
static NJS_MATERIAL collision_246_materials[1];
static Sint16 collision_246_faces[10];
static NJS_MESHSET collision_246_meshsets[1];
static NJS_POINT3 collision_246_points[6];
static NJS_MODEL collision_246_model;
static NJS_OBJECT collision_246;
static NJS_MATERIAL collision_248_materials[1];
static Sint16 collision_248_faces[21];
static NJS_MESHSET collision_248_meshsets[1];
static NJS_POINT3 collision_248_points[12];
static NJS_MODEL collision_248_model;
static NJS_OBJECT collision_248;
static NJS_MATERIAL collision_249_materials[1];
static Sint16 collision_249_faces[81];
static NJS_MESHSET collision_249_meshsets[1];
static NJS_POINT3 collision_249_points[52];
static NJS_MODEL collision_249_model;
static NJS_OBJECT collision_249;
static NJS_MATERIAL collision_265_materials[1];
static Sint16 collision_265_faces[10];
static NJS_MESHSET collision_265_meshsets[1];
static NJS_POINT3 collision_265_points[8];
static NJS_MODEL collision_265_model;
static NJS_OBJECT collision_265;
static NJS_MATERIAL collision_267_materials[1];
static Sint16 collision_267_faces[61];
static NJS_MESHSET collision_267_meshsets[1];
static NJS_POINT3 collision_267_points[60];
static NJS_MODEL collision_267_model;
static NJS_OBJECT collision_267;
static NJS_MATERIAL collision_268_materials[1];
static Sint16 collision_268_faces[21];
static NJS_MESHSET collision_268_meshsets[1];
static NJS_POINT3 collision_268_points[18];
static NJS_MODEL collision_268_model;
static NJS_OBJECT collision_268;
static NJS_MATERIAL collision_271_materials[1];
static Sint16 collision_271_faces[28];
static NJS_MESHSET collision_271_meshsets[1];
static NJS_POINT3 collision_271_points[16];
static NJS_MODEL collision_271_model;
static NJS_OBJECT collision_271;
static NJS_MATERIAL collision_272_materials[1];
static Sint16 collision_272_faces[22];
static NJS_MESHSET collision_272_meshsets[1];
static NJS_POINT3 collision_272_points[12];
static NJS_MODEL collision_272_model;
static NJS_OBJECT collision_272;
static NJS_MATERIAL collision_274_materials[1];
static Sint16 collision_274_faces[35];
static NJS_MESHSET collision_274_meshsets[1];
static NJS_POINT3 collision_274_points[24];
static NJS_MODEL collision_274_model;
static NJS_OBJECT collision_274;
static NJS_MATERIAL collision_275_materials[1];
static Sint16 collision_275_faces[7];
static NJS_MESHSET collision_275_meshsets[1];
static NJS_POINT3 collision_275_points[6];
static NJS_MODEL collision_275_model;
static NJS_OBJECT collision_275;
static NJS_MATERIAL collision_276_materials[1];
static Sint16 collision_276_faces[21];
static NJS_MESHSET collision_276_meshsets[1];
static NJS_POINT3 collision_276_points[18];
static NJS_MODEL collision_276_model;
static NJS_OBJECT collision_276;
static NJS_MATERIAL collision_277_materials[1];
static Sint16 collision_277_faces[7];
static NJS_MESHSET collision_277_meshsets[1];
static NJS_POINT3 collision_277_points[6];
static NJS_MODEL collision_277_model;
static NJS_OBJECT collision_277;
static NJS_MATERIAL collision_278_materials[1];
static Sint16 collision_278_faces[28];
static NJS_MESHSET collision_278_meshsets[1];
static NJS_POINT3 collision_278_points[21];
static NJS_MODEL collision_278_model;
static NJS_OBJECT collision_278;
static NJS_MATERIAL collision_280_materials[1];
static Sint16 collision_280_faces[184];
static NJS_MESHSET collision_280_meshsets[1];
static NJS_POINT3 collision_280_points[74];
static NJS_MODEL collision_280_model;
static NJS_OBJECT collision_280;
static NJS_MATERIAL collision_281_materials[1];
static Sint16 collision_281_faces[294];
static NJS_MESHSET collision_281_meshsets[1];
static NJS_POINT3 collision_281_points[152];
static NJS_MODEL collision_281_model;
static NJS_OBJECT collision_281;
static NJS_MATERIAL collision_282_materials[1];
static Sint16 collision_282_faces[25];
static NJS_MESHSET collision_282_meshsets[1];
static NJS_POINT3 collision_282_points[15];
static NJS_MODEL collision_282_model;
static NJS_OBJECT collision_282;
static NJS_MATERIAL collision_283_materials[1];
static Sint16 collision_283_faces[27];
static NJS_MESHSET collision_283_meshsets[1];
static NJS_POINT3 collision_283_points[24];
static NJS_MODEL collision_283_model;
static NJS_OBJECT collision_283;
static NJS_MATERIAL collision_286_materials[1];
static Sint16 collision_286_faces[9];
static NJS_MESHSET collision_286_meshsets[1];
static NJS_POINT3 collision_286_points[8];
static NJS_MODEL collision_286_model;
static NJS_OBJECT collision_286;
static NJS_MATERIAL collision_287_materials[1];
static Sint16 collision_287_faces[17];
static NJS_MESHSET collision_287_meshsets[1];
static NJS_POINT3 collision_287_points[14];
static NJS_MODEL collision_287_model;
static NJS_OBJECT collision_287;
static NJS_MATERIAL collision_288_materials[1];
static Sint16 collision_288_faces[11];
static NJS_MESHSET collision_288_meshsets[1];
static NJS_POINT3 collision_288_points[8];
static NJS_MODEL collision_288_model;
static NJS_OBJECT collision_288;
static NJS_MATERIAL collision_289_materials[1];
static Sint16 collision_289_faces[304];
static NJS_MESHSET collision_289_meshsets[1];
static NJS_POINT3 collision_289_points[116];
static NJS_MODEL collision_289_model;
static NJS_OBJECT collision_289;
static NJS_MATERIAL collision_290_materials[1];
static Sint16 collision_290_faces[19];
static NJS_MESHSET collision_290_meshsets[1];
static NJS_POINT3 collision_290_points[18];
static NJS_MODEL collision_290_model;
static NJS_OBJECT collision_290;
static NJS_MATERIAL collision_291_materials[1];
static Sint16 collision_291_faces[7];
static NJS_MESHSET collision_291_meshsets[1];
static NJS_POINT3 collision_291_points[6];
static NJS_MODEL collision_291_model;
static NJS_OBJECT collision_291;
static NJS_MATERIAL collision_292_materials[1];
static Sint16 collision_292_faces[5];
static NJS_MESHSET collision_292_meshsets[1];
static NJS_POINT3 collision_292_points[4];
static NJS_MODEL collision_292_model;
static NJS_OBJECT collision_292;
static NJS_MATERIAL collision_294_materials[1];
static Sint16 collision_294_faces[22];
static NJS_MESHSET collision_294_meshsets[1];
static NJS_POINT3 collision_294_points[16];
static NJS_MODEL collision_294_model;
static NJS_OBJECT collision_294;
static NJS_MATERIAL collision_295_materials[1];
static Sint16 collision_295_faces[45];
static NJS_MESHSET collision_295_meshsets[1];
static NJS_POINT3 collision_295_points[42];
static NJS_MODEL collision_295_model;
static NJS_OBJECT collision_295;
static NJS_MATERIAL collision_299_materials[1];
static Sint16 collision_299_faces[57];
static NJS_MESHSET collision_299_meshsets[1];
static NJS_POINT3 collision_299_points[54];
static NJS_MODEL collision_299_model;
static NJS_OBJECT collision_299;
static NJS_MATERIAL collision_300_materials[1];
static Sint16 collision_300_faces[20];
static NJS_MESHSET collision_300_meshsets[1];
static NJS_POINT3 collision_300_points[16];
static NJS_MODEL collision_300_model;
static NJS_OBJECT collision_300;
static NJS_MATERIAL collision_301_materials[1];
static Sint16 collision_301_faces[20];
static NJS_MESHSET collision_301_meshsets[1];
static NJS_POINT3 collision_301_points[16];
static NJS_MODEL collision_301_model;
static NJS_OBJECT collision_301;
static NJS_MATERIAL collision_302_materials[1];
static Sint16 collision_302_faces[33];
static NJS_MESHSET collision_302_meshsets[1];
static NJS_POINT3 collision_302_points[26];
static NJS_MODEL collision_302_model;
static NJS_OBJECT collision_302;
static NJS_MATERIAL collision_303_materials[1];
static Sint16 collision_303_faces[33];
static NJS_MESHSET collision_303_meshsets[1];
static NJS_POINT3 collision_303_points[26];
static NJS_MODEL collision_303_model;
static NJS_OBJECT collision_303;
static NJS_MATERIAL collision_304_materials[1];
static Sint16 collision_304_faces[33];
static NJS_MESHSET collision_304_meshsets[1];
static NJS_POINT3 collision_304_points[26];
static NJS_MODEL collision_304_model;
static NJS_OBJECT collision_304;
static NJS_MATERIAL collision_305_materials[1];
static Sint16 collision_305_faces[33];
static NJS_MESHSET collision_305_meshsets[1];
static NJS_POINT3 collision_305_points[26];
static NJS_MODEL collision_305_model;
static NJS_OBJECT collision_305;
static NJS_MATERIAL collision_306_materials[1];
static Sint16 collision_306_faces[33];
static NJS_MESHSET collision_306_meshsets[1];
static NJS_POINT3 collision_306_points[26];
static NJS_MODEL collision_306_model;
static NJS_OBJECT collision_306;
static NJS_MATERIAL collision_307_materials[1];
static Sint16 collision_307_faces[33];
static NJS_MESHSET collision_307_meshsets[1];
static NJS_POINT3 collision_307_points[26];
static NJS_MODEL collision_307_model;
static NJS_OBJECT collision_307;
static NJS_MATERIAL collision_308_materials[1];
static Sint16 collision_308_faces[22];
static NJS_MESHSET collision_308_meshsets[1];
static NJS_POINT3 collision_308_points[12];
static NJS_MODEL collision_308_model;
static NJS_OBJECT collision_308;
static NJS_MATERIAL collision_309_materials[1];
static Sint16 collision_309_faces[75];
static NJS_MESHSET collision_309_meshsets[1];
static NJS_POINT3 collision_309_points[74];
static NJS_MODEL collision_309_model;
static NJS_OBJECT collision_309;
static NJS_MATERIAL collision_311_materials[1];
static Sint16 collision_311_faces[29];
static NJS_MESHSET collision_311_meshsets[1];
static NJS_POINT3 collision_311_points[18];
static NJS_MODEL collision_311_model;
static NJS_OBJECT collision_311;
static NJS_MATERIAL collision_315_materials[1];
static Sint16 collision_315_faces[57];
static NJS_MESHSET collision_315_meshsets[1];
static NJS_POINT3 collision_315_points[54];
static NJS_MODEL collision_315_model;
static NJS_OBJECT collision_315;
static NJS_MATERIAL collision_318_materials[1];
static Sint16 collision_318_faces[30];
static NJS_MESHSET collision_318_meshsets[1];
static NJS_POINT3 collision_318_points[18];
static NJS_MODEL collision_318_model;
static NJS_OBJECT collision_318;
static NJS_MATERIAL collision_320_materials[1];
static Sint16 collision_320_faces[57];
static NJS_MESHSET collision_320_meshsets[1];
static NJS_POINT3 collision_320_points[54];
static NJS_MODEL collision_320_model;
static NJS_OBJECT collision_320;
static NJS_MATERIAL collision_322_materials[1];
static Sint16 collision_322_faces[33];
static NJS_MESHSET collision_322_meshsets[1];
static NJS_POINT3 collision_322_points[32];
static NJS_MODEL collision_322_model;
static NJS_OBJECT collision_322;
static NJS_MATERIAL collision_324_materials[1];
static Sint16 collision_324_faces[11];
static NJS_MESHSET collision_324_meshsets[1];
static NJS_POINT3 collision_324_points[8];
static NJS_MODEL collision_324_model;
static NJS_OBJECT collision_324;
static NJS_MATERIAL collision_326_materials[1];
static Sint16 collision_326_faces[63];
static NJS_MESHSET collision_326_meshsets[1];
static NJS_POINT3 collision_326_points[32];
static NJS_MODEL collision_326_model;
static NJS_OBJECT collision_326;
static NJS_MATERIAL collision_327_materials[1];
static Sint16 collision_327_faces[91];
static NJS_MESHSET collision_327_meshsets[1];
static NJS_POINT3 collision_327_points[40];
static NJS_MODEL collision_327_model;
static NJS_OBJECT collision_327;
static NJS_MATERIAL collision_328_materials[1];
static Sint16 collision_328_faces[55];
static NJS_MESHSET collision_328_meshsets[1];
static NJS_POINT3 collision_328_points[30];
static NJS_MODEL collision_328_model;
static NJS_OBJECT collision_328;
static NJS_MATERIAL collision_330_materials[1];
static Sint16 collision_330_faces[16];
static NJS_MESHSET collision_330_meshsets[1];
static NJS_POINT3 collision_330_points[8];
static NJS_MODEL collision_330_model;
static NJS_OBJECT collision_330;
static NJS_MATERIAL collision_335_materials[1];
static Sint16 collision_335_faces[7];
static NJS_MESHSET collision_335_meshsets[1];
static NJS_POINT3 collision_335_points[6];
static NJS_MODEL collision_335_model;
static NJS_OBJECT collision_335;
static NJS_MATERIAL collision_336_materials[1];
static Sint16 collision_336_faces[88];
static NJS_MESHSET collision_336_meshsets[1];
static NJS_POINT3 collision_336_points[40];
static NJS_MODEL collision_336_model;
static NJS_OBJECT collision_336;
static NJS_MATERIAL collision_338_materials[1];
static Sint16 collision_338_faces[55];
static NJS_MESHSET collision_338_meshsets[1];
static NJS_POINT3 collision_338_points[30];
static NJS_MODEL collision_338_model;
static NJS_OBJECT collision_338;
static NJS_MATERIAL collision_340_materials[1];
static Sint16 collision_340_faces[19];
static NJS_MESHSET collision_340_meshsets[1];
static NJS_POINT3 collision_340_points[8];
static NJS_MODEL collision_340_model;
static NJS_OBJECT collision_340;
static NJS_MATERIAL collision_349_materials[1];
static Sint16 collision_349_faces[33];
static NJS_MESHSET collision_349_meshsets[1];
static NJS_POINT3 collision_349_points[32];
static NJS_MODEL collision_349_model;
static NJS_OBJECT collision_349;
static NJS_MATERIAL collision_351_materials[1];
static Sint16 collision_351_faces[33];
static NJS_MESHSET collision_351_meshsets[1];
static NJS_POINT3 collision_351_points[26];
static NJS_MODEL collision_351_model;
static NJS_OBJECT collision_351;
static NJS_MATERIAL collision_353_materials[1];
static Sint16 collision_353_faces[33];
static NJS_MESHSET collision_353_meshsets[1];
static NJS_POINT3 collision_353_points[26];
static NJS_MODEL collision_353_model;
static NJS_OBJECT collision_353;
static NJS_MATERIAL collision_354_materials[1];
static Sint16 collision_354_faces[33];
static NJS_MESHSET collision_354_meshsets[1];
static NJS_POINT3 collision_354_points[26];
static NJS_MODEL collision_354_model;
static NJS_OBJECT collision_354;
static NJS_MATERIAL collision_355_materials[1];
static Sint16 collision_355_faces[33];
static NJS_MESHSET collision_355_meshsets[1];
static NJS_POINT3 collision_355_points[26];
static NJS_MODEL collision_355_model;
static NJS_OBJECT collision_355;
static NJS_MATERIAL collision_356_materials[1];
static Sint16 collision_356_faces[33];
static NJS_MESHSET collision_356_meshsets[1];
static NJS_POINT3 collision_356_points[26];
static NJS_MODEL collision_356_model;
static NJS_OBJECT collision_356;
static NJS_MATERIAL collision_357_materials[1];
static Sint16 collision_357_faces[33];
static NJS_MESHSET collision_357_meshsets[1];
static NJS_POINT3 collision_357_points[26];
static NJS_MODEL collision_357_model;
static NJS_OBJECT collision_357;
static NJS_MATERIAL collision_358_materials[1];
static Sint16 collision_358_faces[33];
static NJS_MESHSET collision_358_meshsets[1];
static NJS_POINT3 collision_358_points[26];
static NJS_MODEL collision_358_model;
static NJS_OBJECT collision_358;
static NJS_MATERIAL collision_359_materials[1];
static Sint16 collision_359_faces[20];
static NJS_MESHSET collision_359_meshsets[1];
static NJS_POINT3 collision_359_points[16];
static NJS_MODEL collision_359_model;
static NJS_OBJECT collision_359;
static NJS_MATERIAL collision_360_materials[1];
static Sint16 collision_360_faces[20];
static NJS_MESHSET collision_360_meshsets[1];
static NJS_POINT3 collision_360_points[16];
static NJS_MODEL collision_360_model;
static NJS_OBJECT collision_360;
static NJS_MATERIAL collision_361_materials[1];
static Sint16 collision_361_faces[27];
static NJS_MESHSET collision_361_meshsets[1];
static NJS_POINT3 collision_361_points[12];
static NJS_MODEL collision_361_model;
static NJS_OBJECT collision_361;
static NJS_MATERIAL collision_362_materials[1];
static Sint16 collision_362_faces[75];
static NJS_MESHSET collision_362_meshsets[1];
static NJS_POINT3 collision_362_points[74];
static NJS_MODEL collision_362_model;
static NJS_OBJECT collision_362;
static NJS_MATERIAL collision_364_materials[1];
static Sint16 collision_364_faces[28];
static NJS_MESHSET collision_364_meshsets[1];
static NJS_POINT3 collision_364_points[21];
static NJS_MODEL collision_364_model;
static NJS_OBJECT collision_364;
static NJS_MATERIAL collision_370_materials[1];
static Sint16 collision_370_faces[21];
static NJS_MESHSET collision_370_meshsets[1];
static NJS_POINT3 collision_370_points[12];
static NJS_MODEL collision_370_model;
static NJS_OBJECT collision_370;
static NJS_MATERIAL collision_371_materials[1];
static Sint16 collision_371_faces[17];
static NJS_MESHSET collision_371_meshsets[1];
static NJS_POINT3 collision_371_points[16];
static NJS_MODEL collision_371_model;
static NJS_OBJECT collision_371;
static NJS_MATERIAL collision_375_materials[1];
static Sint16 collision_375_faces[27];
static NJS_MESHSET collision_375_meshsets[1];
static NJS_POINT3 collision_375_points[24];
static NJS_MODEL collision_375_model;
static NJS_OBJECT collision_375;
static NJS_MATERIAL collision_377_materials[1];
static Sint16 collision_377_faces[33];
static NJS_MESHSET collision_377_meshsets[1];
static NJS_POINT3 collision_377_points[30];
static NJS_MODEL collision_377_model;
static NJS_OBJECT collision_377;
static NJS_MATERIAL collision_380_materials[1];
static Sint16 collision_380_faces[33];
static NJS_MESHSET collision_380_meshsets[1];
static NJS_POINT3 collision_380_points[30];
static NJS_MODEL collision_380_model;
static NJS_OBJECT collision_380;
static NJS_MATERIAL collision_383_materials[1];
static Sint16 collision_383_faces[15];
static NJS_MESHSET collision_383_meshsets[1];
static NJS_POINT3 collision_383_points[12];
static NJS_MODEL collision_383_model;
static NJS_OBJECT collision_383;
static NJS_MATERIAL collision_384_materials[1];
static Sint16 collision_384_faces[15];
static NJS_MESHSET collision_384_meshsets[1];
static NJS_POINT3 collision_384_points[12];
static NJS_MODEL collision_384_model;
static NJS_OBJECT collision_384;
static NJS_MATERIAL collision_385_materials[1];
static Sint16 collision_385_faces[74];
static NJS_MESHSET collision_385_meshsets[1];
static NJS_POINT3 collision_385_points[48];
static NJS_MODEL collision_385_model;
static NJS_OBJECT collision_385;
static NJS_MATERIAL collision_387_materials[1];
static Sint16 collision_387_faces[51];
static NJS_MESHSET collision_387_meshsets[1];
static NJS_POINT3 collision_387_points[48];
static NJS_MODEL collision_387_model;
static NJS_OBJECT collision_387;
static NJS_MATERIAL collision_389_materials[1];
static Sint16 collision_389_faces[58];
static NJS_MESHSET collision_389_meshsets[1];
static NJS_POINT3 collision_389_points[24];
static NJS_MODEL collision_389_model;
static NJS_OBJECT collision_389;
static NJS_MATERIAL collision_392_materials[1];
static Sint16 collision_392_faces[10];
static NJS_MESHSET collision_392_meshsets[1];
static NJS_POINT3 collision_392_points[8];
static NJS_MODEL collision_392_model;
static NJS_OBJECT collision_392;
static NJS_MATERIAL collision_398_materials[1];
static Sint16 collision_398_faces[30];
static NJS_MESHSET collision_398_meshsets[1];
static NJS_POINT3 collision_398_points[20];
static NJS_MODEL collision_398_model;
static NJS_OBJECT collision_398;
static NJS_MATERIAL collision_399_materials[1];
static Sint16 collision_399_faces[10];
static NJS_MESHSET collision_399_meshsets[1];
static NJS_POINT3 collision_399_points[8];
static NJS_MODEL collision_399_model;
static NJS_OBJECT collision_399;
static NJS_MATERIAL collision_401_materials[1];
static Sint16 collision_401_faces[22];
static NJS_MESHSET collision_401_meshsets[1];
static NJS_POINT3 collision_401_points[12];
static NJS_MODEL collision_401_model;
static NJS_OBJECT collision_401;
static NJS_MATERIAL collision_402_materials[1];
static Sint16 collision_402_faces[44];
static NJS_MESHSET collision_402_meshsets[1];
static NJS_POINT3 collision_402_points[20];
static NJS_MODEL collision_402_model;
static NJS_OBJECT collision_402;
static NJS_MATERIAL collision_404_materials[1];
static Sint16 collision_404_faces[28];
static NJS_MESHSET collision_404_meshsets[1];
static NJS_POINT3 collision_404_points[15];
static NJS_MODEL collision_404_model;
static NJS_OBJECT collision_404;
static NJS_MATERIAL collision_405_materials[1];
static Sint16 collision_405_faces[22];
static NJS_MESHSET collision_405_meshsets[1];
static NJS_POINT3 collision_405_points[12];
static NJS_MODEL collision_405_model;
static NJS_OBJECT collision_405;
static NJS_MATERIAL collision_406_materials[1];
static Sint16 collision_406_faces[20];
static NJS_MESHSET collision_406_meshsets[1];
static NJS_POINT3 collision_406_points[10];
static NJS_MODEL collision_406_model;
static NJS_OBJECT collision_406;
static NJS_MATERIAL collision_407_materials[1];
static Sint16 collision_407_faces[32];
static NJS_MESHSET collision_407_meshsets[1];
static NJS_POINT3 collision_407_points[16];
static NJS_MODEL collision_407_model;
static NJS_OBJECT collision_407;
static NJS_MATERIAL collision_408_materials[1];
static Sint16 collision_408_faces[64];
static NJS_MESHSET collision_408_meshsets[1];
static NJS_POINT3 collision_408_points[29];
static NJS_MODEL collision_408_model;
static NJS_OBJECT collision_408;
static NJS_MATERIAL collision_409_materials[1];
static Sint16 collision_409_faces[41];
static NJS_MESHSET collision_409_meshsets[1];
static NJS_POINT3 collision_409_points[20];
static NJS_MODEL collision_409_model;
static NJS_OBJECT collision_409;
static NJS_MATERIAL collision_410_materials[1];
static Sint16 collision_410_faces[30];
static NJS_MESHSET collision_410_meshsets[1];
static NJS_POINT3 collision_410_points[16];
static NJS_MODEL collision_410_model;
static NJS_OBJECT collision_410;
static NJS_MATERIAL collision_411_materials[1];
static Sint16 collision_411_faces[21];
static NJS_MESHSET collision_411_meshsets[1];
static NJS_POINT3 collision_411_points[12];
static NJS_MODEL collision_411_model;
static NJS_OBJECT collision_411;
static NJS_MATERIAL collision_412_materials[1];
static Sint16 collision_412_faces[23];
static NJS_MESHSET collision_412_meshsets[1];
static NJS_POINT3 collision_412_points[12];
static NJS_MODEL collision_412_model;
static NJS_OBJECT collision_412;
static NJS_MATERIAL collision_413_materials[1];
static Sint16 collision_413_faces[20];
static NJS_MESHSET collision_413_meshsets[1];
static NJS_POINT3 collision_413_points[10];
static NJS_MODEL collision_413_model;
static NJS_OBJECT collision_413;
static NJS_MATERIAL collision_414_materials[1];
static Sint16 collision_414_faces[25];
static NJS_MESHSET collision_414_meshsets[1];
static NJS_POINT3 collision_414_points[13];
static NJS_MODEL collision_414_model;
static NJS_OBJECT collision_414;
static NJS_MATERIAL collision_416_materials[1];
static Sint16 collision_416_faces[22];
static NJS_MESHSET collision_416_meshsets[1];
static NJS_POINT3 collision_416_points[12];
static NJS_MODEL collision_416_model;
static NJS_OBJECT collision_416;
static Sint16 draw_5_polygons[219];
static Sint32 draw_5_vertices[103];
static NJS_CNK_MODEL draw_5_model;
static NJS_CNK_OBJECT draw_710;
static Sint16 draw_260_polygons[635];
static Sint32 draw_260_vertices[571];
static NJS_CNK_MODEL draw_260_model;
static NJS_CNK_OBJECT draw_709;
static Sint16 draw_101_polygons[191];
static Sint32 draw_101_vertices[203];
static NJS_CNK_MODEL draw_101_model;
static NJS_CNK_OBJECT draw_708;
static Sint16 draw_95_polygons[237];
static Sint32 draw_95_vertices[251];
static NJS_CNK_MODEL draw_95_model;
static NJS_CNK_OBJECT draw_707;
static Sint16 draw_382_polygons[23];
static Sint32 draw_382_vertices[19];
static NJS_CNK_MODEL draw_382_model;
static NJS_CNK_OBJECT draw_704;
static Sint16 draw_248_polygons[101];
static Sint32 draw_248_vertices[63];
static NJS_CNK_MODEL draw_248_model;
static NJS_CNK_OBJECT draw_695;
static Sint16 draw_246_polygons[429];
static Sint32 draw_246_vertices[515];
static NJS_CNK_MODEL draw_246_model;
static NJS_CNK_OBJECT draw_679;
static Sint16 draw_244_polygons[757];
static Sint32 draw_244_vertices[495];
static NJS_CNK_MODEL draw_244_model;
static NJS_CNK_OBJECT draw_675;
static Sint16 draw_237_polygons[433];
static Sint32 draw_237_vertices[515];
static NJS_CNK_MODEL draw_237_model;
static NJS_CNK_OBJECT draw_666;
static Sint16 draw_231_polygons[757];
static Sint32 draw_231_vertices[495];
static NJS_CNK_MODEL draw_231_model;
static NJS_CNK_OBJECT draw_657;
static Sint16 draw_277_polygons[81];
static Sint32 draw_277_vertices[51];
static NJS_CNK_MODEL draw_277_model;
static NJS_CNK_OBJECT draw_655;
static Sint16 draw_206_polygons[337];
static Sint32 draw_206_vertices[347];
static NJS_CNK_MODEL draw_206_model;
static NJS_CNK_OBJECT draw_654;
static Sint16 draw_201_polygons[1889];
static Sint32 draw_201_vertices[1539];
static NJS_CNK_MODEL draw_201_model;
static NJS_CNK_OBJECT draw_653;
static Sint16 draw_186_polygons[337];
static Sint32 draw_186_vertices[347];
static NJS_CNK_MODEL draw_186_model;
static NJS_CNK_OBJECT draw_652;
static Sint16 draw_158_polygons[53];
static Sint32 draw_158_vertices[51];
static NJS_CNK_MODEL draw_158_model;
static NJS_CNK_OBJECT draw_651;
static Sint16 draw_153_polygons[61];
static Sint32 draw_153_vertices[43];
static NJS_CNK_MODEL draw_153_model;
static NJS_CNK_OBJECT draw_650;
static Sint16 draw_150_polygons[667];
static Sint32 draw_150_vertices[555];
static NJS_CNK_MODEL draw_150_model;
static NJS_CNK_OBJECT draw_649;
static Sint16 draw_121_polygons[323];
static Sint32 draw_121_vertices[279];
static NJS_CNK_MODEL draw_121_model;
static NJS_CNK_OBJECT draw_645;
static Sint16 draw_122_polygons[873];
static Sint32 draw_122_vertices[707];
static NJS_CNK_MODEL draw_122_model;
static NJS_CNK_OBJECT draw_644;
static Sint16 draw_8_polygons[687];
static Sint32 draw_8_vertices[511];
static NJS_CNK_MODEL draw_8_model;
static NJS_CNK_OBJECT draw_643;
static Sint16 draw_19_polygons[687];
static Sint32 draw_19_vertices[519];
static NJS_CNK_MODEL draw_19_model;
static NJS_CNK_OBJECT draw_642;
static Sint16 draw_18_polygons[829];
static Sint32 draw_18_vertices[587];
static NJS_CNK_MODEL draw_18_model;
static NJS_CNK_OBJECT draw_641;
static Sint16 draw_107_polygons[531];
static Sint32 draw_107_vertices[295];
static NJS_CNK_MODEL draw_107_model;
static NJS_CNK_OBJECT draw_640;
static Sint16 draw_29_polygons[215];
static Sint32 draw_29_vertices[139];
static NJS_CNK_MODEL draw_29_model;
static NJS_CNK_OBJECT draw_626;
static Sint16 draw_263_polygons[429];
static Sint32 draw_263_vertices[307];
static NJS_CNK_MODEL draw_263_model;
static NJS_CNK_OBJECT draw_622;
static Sint16 draw_621_polygons[1037];
static Sint32 draw_621_vertices[707];
static NJS_CNK_MODEL draw_621_model;
static NJS_CNK_OBJECT draw_621;
static Sint16 draw_261_polygons[149];
static Sint32 draw_261_vertices[115];
static NJS_CNK_MODEL draw_261_model;
static NJS_CNK_OBJECT draw_620;
static Sint16 draw_57_polygons[215];
static Sint32 draw_57_vertices[139];
static NJS_CNK_MODEL draw_57_model;
static NJS_CNK_OBJECT draw_617;
static Sint16 draw_22_polygons[89];
static Sint32 draw_22_vertices[107];
static NJS_CNK_MODEL draw_22_model;
static NJS_CNK_OBJECT draw_608;
static Sint16 draw_40_polygons[697];
static Sint32 draw_40_vertices[427];
static NJS_CNK_MODEL draw_40_model;
static NJS_CNK_OBJECT draw_607;
static Sint16 draw_23_polygons[89];
static Sint32 draw_23_vertices[107];
static NJS_CNK_MODEL draw_23_model;
static NJS_CNK_OBJECT draw_604;
static Sint16 draw_33_polygons[695];
static Sint32 draw_33_vertices[423];
static NJS_CNK_MODEL draw_33_model;
static NJS_CNK_OBJECT draw_603;
static Sint16 draw_21_polygons[59];
static Sint32 draw_21_vertices[67];
static NJS_CNK_MODEL draw_21_model;
static NJS_CNK_OBJECT draw_600;
static Sint16 draw_20_polygons[59];
static Sint32 draw_20_vertices[67];
static NJS_CNK_MODEL draw_20_model;
static NJS_CNK_OBJECT draw_599;
static Sint16 draw_2_polygons[455];
static Sint32 draw_2_vertices[351];
static NJS_CNK_MODEL draw_2_model;
static NJS_CNK_OBJECT draw_598;
static Sint16 draw_38_polygons[877];
static Sint32 draw_38_vertices[587];
static NJS_CNK_MODEL draw_38_model;
static NJS_CNK_OBJECT draw_597;
static Sint16 draw_37_polygons[861];
static Sint32 draw_37_vertices[519];
static NJS_CNK_MODEL draw_37_model;
static NJS_CNK_OBJECT draw_596;
static Sint16 draw_24_polygons[29];
static Sint32 draw_24_vertices[27];
static NJS_CNK_MODEL draw_24_model;
static NJS_CNK_OBJECT draw_595;
static Sint16 draw_4_polygons[93];
static Sint32 draw_4_vertices[67];
static NJS_CNK_MODEL draw_4_model;
static NJS_CNK_OBJECT draw_594;
static Sint16 draw_10_polygons[437];
static Sint32 draw_10_vertices[355];
static NJS_CNK_MODEL draw_10_model;
static NJS_CNK_OBJECT draw_590;
static Sint16 draw_7_polygons[157];
static Sint32 draw_7_vertices[123];
static NJS_CNK_MODEL draw_7_model;
static NJS_CNK_OBJECT draw_585;
static Sint16 draw_11_polygons[79];
static Sint32 draw_11_vertices[51];
static NJS_CNK_MODEL draw_11_model;
static NJS_CNK_OBJECT draw_584;
static Sint16 draw_131_polygons[297];
static Sint32 draw_131_vertices[363];
static NJS_CNK_MODEL draw_131_model;
static NJS_CNK_OBJECT draw_583;
static Sint16 draw_1_polygons[125];
static Sint32 draw_1_vertices[75];
static NJS_CNK_MODEL draw_1_model;
static NJS_CNK_OBJECT draw_557;
static Sint16 draw_363_polygons[157];
static Sint32 draw_363_vertices[107];
static NJS_CNK_MODEL draw_363_model;
static NJS_CNK_OBJECT draw_554;
static Sint16 draw_13_polygons[161];
static Sint32 draw_13_vertices[95];
static NJS_CNK_MODEL draw_13_model;
static NJS_CNK_OBJECT draw_543;
static Sint16 draw_542_polygons[407];
static Sint32 draw_542_vertices[259];
static NJS_CNK_MODEL draw_542_model;
static NJS_CNK_OBJECT draw_542;
static Sint16 draw_369_polygons[713];
static Sint32 draw_369_vertices[531];
static NJS_CNK_MODEL draw_369_model;
static NJS_CNK_OBJECT draw_540;
static Sint16 draw_14_polygons[261];
static Sint32 draw_14_vertices[147];
static NJS_CNK_MODEL draw_14_model;
static NJS_CNK_OBJECT draw_539;
static Sint16 draw_0_polygons[1145];
static Sint32 draw_0_vertices[1055];
static NJS_CNK_MODEL draw_0_model;
static NJS_CNK_OBJECT draw_538;
static Sint16 draw_533_polygons[713];
static Sint32 draw_533_vertices[531];
static NJS_CNK_MODEL draw_533_model;
static NJS_CNK_OBJECT draw_533;
static Sint16 draw_16_polygons[91];
static Sint32 draw_16_vertices[67];
static NJS_CNK_MODEL draw_16_model;
static NJS_CNK_OBJECT draw_531;
static Sint16 draw_124_polygons[663];
static Sint32 draw_124_vertices[507];
static NJS_CNK_MODEL draw_124_model;
static NJS_CNK_OBJECT draw_530;
static Sint16 draw_123_polygons[429];
static Sint32 draw_123_vertices[295];
static NJS_CNK_MODEL draw_123_model;
static NJS_CNK_OBJECT draw_529;
static Sint16 draw_9_polygons[203];
static Sint32 draw_9_vertices[131];
static NJS_CNK_MODEL draw_9_model;
static NJS_CNK_OBJECT draw_526;
static Sint16 draw_409_polygons[37];
static Sint32 draw_409_vertices[35];
static NJS_CNK_MODEL draw_409_model;
static NJS_CNK_OBJECT draw_525;
static Sint16 draw_242_polygons[509];
static Sint32 draw_242_vertices[327];
static NJS_CNK_MODEL draw_242_model;
static NJS_CNK_OBJECT draw_519;
static Sint16 draw_143_polygons[99];
static Sint32 draw_143_vertices[91];
static NJS_CNK_MODEL draw_143_model;
static NJS_CNK_OBJECT draw_518;
static Sint16 draw_32_polygons[323];
static Sint32 draw_32_vertices[163];
static NJS_CNK_MODEL draw_32_model;
static NJS_CNK_OBJECT draw_517;
static Sint16 draw_210_polygons[173];
static Sint32 draw_210_vertices[171];
static NJS_CNK_MODEL draw_210_model;
static NJS_CNK_OBJECT draw_516;
static Sint16 draw_166_polygons[253];
static Sint32 draw_166_vertices[155];
static NJS_CNK_MODEL draw_166_model;
static NJS_CNK_OBJECT draw_515;
static Sint16 draw_144_polygons[87];
static Sint32 draw_144_vertices[67];
static NJS_CNK_MODEL draw_144_model;
static NJS_CNK_OBJECT draw_514;
static Sint16 draw_173_polygons[175];
static Sint32 draw_173_vertices[107];
static NJS_CNK_MODEL draw_173_model;
static NJS_CNK_OBJECT draw_513;
static Sint16 draw_15_polygons[125];
static Sint32 draw_15_vertices[91];
static NJS_CNK_MODEL draw_15_model;
static NJS_CNK_OBJECT draw_512;
static Sint16 draw_311_polygons[93];
static Sint32 draw_311_vertices[99];
static NJS_CNK_MODEL draw_311_model;
static NJS_CNK_OBJECT draw_505;
static Sint16 draw_487_polygons[517];
static Sint32 draw_487_vertices[351];
static NJS_CNK_MODEL draw_487_model;
static NJS_CNK_OBJECT draw_487;
static Sint16 draw_91_polygons[2209];
static Sint32 draw_91_vertices[1947];
static NJS_CNK_MODEL draw_91_model;
static NJS_CNK_OBJECT draw_475;
static Sint16 draw_102_polygons[209];
static Sint32 draw_102_vertices[163];
static NJS_CNK_MODEL draw_102_model;
static NJS_CNK_OBJECT draw_474;
static Sint16 draw_92_polygons[867];
static Sint32 draw_92_vertices[475];
static NJS_CNK_MODEL draw_92_model;
static NJS_CNK_OBJECT draw_473;
static Sint16 draw_468_polygons[529];
static Sint32 draw_468_vertices[323];
static NJS_CNK_MODEL draw_468_model;
static NJS_CNK_OBJECT draw_468;
static Sint16 draw_77_polygons[241];
static Sint32 draw_77_vertices[243];
static NJS_CNK_MODEL draw_77_model;
static NJS_CNK_OBJECT draw_462;
static Sint16 draw_76_polygons[125];
static Sint32 draw_76_vertices[91];
static NJS_CNK_MODEL draw_76_model;
static NJS_CNK_OBJECT draw_460;
static Sint16 draw_456_polygons[163];
static Sint32 draw_456_vertices[115];
static NJS_CNK_MODEL draw_456_model;
static NJS_CNK_OBJECT draw_456;
static Sint16 draw_455_polygons[525];
static Sint32 draw_455_vertices[451];
static NJS_CNK_MODEL draw_455_model;
static NJS_CNK_OBJECT draw_455;
static Sint16 draw_454_polygons[1235];
static Sint32 draw_454_vertices[831];
static NJS_CNK_MODEL draw_454_model;
static NJS_CNK_OBJECT draw_454;
static Sint16 draw_408_polygons[193];
static Sint32 draw_408_vertices[139];
static NJS_CNK_MODEL draw_408_model;
static NJS_CNK_OBJECT draw_446;
static Sint16 draw_430_polygons[229];
static Sint32 draw_430_vertices[195];
static NJS_CNK_MODEL draw_430_model;
static NJS_CNK_OBJECT draw_430;
static Sint16 draw_428_polygons[195];
static Sint32 draw_428_vertices[107];
static NJS_CNK_MODEL draw_428_model;
static NJS_CNK_OBJECT draw_429;
static Sint16 draw_421_polygons[151];
static Sint32 draw_421_vertices[111];
static NJS_CNK_MODEL draw_421_model;
static NJS_CNK_OBJECT draw_426;
static Sint16 draw_423_polygons[191];
static Sint32 draw_423_vertices[131];
static NJS_CNK_MODEL draw_423_model;
static NJS_CNK_OBJECT draw_423;
static Sint16 draw_422_polygons[329];
static Sint32 draw_422_vertices[131];
static NJS_CNK_MODEL draw_422_model;
static NJS_CNK_OBJECT draw_422;
static Sint16 draw_137_polygons[517];
static Sint32 draw_137_vertices[351];
static NJS_CNK_MODEL draw_137_model;
static NJS_CNK_OBJECT draw_404;
static Sint16 draw_31_polygons[313];
static Sint32 draw_31_vertices[187];
static NJS_CNK_MODEL draw_31_model;
static NJS_CNK_OBJECT draw_356;
static Sint16 draw_30_polygons[279];
static Sint32 draw_30_vertices[195];
static NJS_CNK_MODEL draw_30_model;
static NJS_CNK_OBJECT draw_355;
static Sint16 draw_265_polygons[89];
static Sint32 draw_265_vertices[107];
static NJS_CNK_MODEL draw_265_model;
static NJS_CNK_OBJECT draw_338;
static Sint16 draw_294_polygons[867];
static Sint32 draw_294_vertices[475];
static NJS_CNK_MODEL draw_294_model;
static NJS_CNK_OBJECT draw_313;
static Sint16 draw_259_polygons[521];
static Sint32 draw_259_vertices[435];
static NJS_CNK_MODEL draw_259_model;
static NJS_CNK_OBJECT draw_259;
static Sint16 draw_250_polygons[155];
static Sint32 draw_250_vertices[99];
static NJS_CNK_MODEL draw_250_model;
static NJS_CNK_OBJECT draw_250;
static Sint16 draw_218_polygons[129];
static Sint32 draw_218_vertices[71];
static NJS_CNK_MODEL draw_218_model;
static NJS_CNK_OBJECT draw_218;
static Sint16 draw_148_polygons[69];
static Sint32 draw_148_vertices[35];
static NJS_CNK_MODEL draw_148_model;
static NJS_CNK_OBJECT draw_194;
static Sint16 draw_185_polygons[41];
static Sint32 draw_185_vertices[43];
static NJS_CNK_MODEL draw_185_model;
static NJS_CNK_OBJECT draw_185;
static Sint16 draw_152_polygons[125];
static Sint32 draw_152_vertices[75];
static NJS_CNK_MODEL draw_152_model;
static NJS_CNK_OBJECT draw_152;
static Sint16 draw_151_polygons[125];
static Sint32 draw_151_vertices[75];
static NJS_CNK_MODEL draw_151_model;
static NJS_CNK_OBJECT draw_151;
static Sint16 draw_149_polygons[235];
static Sint32 draw_149_vertices[135];
static NJS_CNK_MODEL draw_149_model;
static NJS_CNK_OBJECT draw_149;
static Sint16 draw_142_polygons[509];
static Sint32 draw_142_vertices[327];
static NJS_CNK_MODEL draw_142_model;
static NJS_CNK_OBJECT draw_142;
static Sint16 draw_134_polygons[407];
static Sint32 draw_134_vertices[259];
static NJS_CNK_MODEL draw_134_model;
static NJS_CNK_OBJECT draw_134;
static Sint16 draw_72_polygons[447];
static Sint32 draw_72_vertices[287];
static NJS_CNK_MODEL draw_72_model;
static NJS_CNK_OBJECT draw_72;
static Sint16 draw_66_polygons[899];
static Sint32 draw_66_vertices[707];
static NJS_CNK_MODEL draw_66_model;
static NJS_CNK_OBJECT draw_66;
static Sint16 draw_65_polygons[943];
static Sint32 draw_65_vertices[611];
static NJS_CNK_MODEL draw_65_model;
static NJS_CNK_OBJECT draw_65;
static Sint16 draw_64_polygons[249];
static Sint32 draw_64_vertices[215];
static NJS_CNK_MODEL draw_64_model;
static NJS_CNK_OBJECT draw_64;
static Sint16 draw_58_polygons[547];
static Sint32 draw_58_vertices[427];
static NJS_CNK_MODEL draw_58_model;
static NJS_CNK_OBJECT draw_58;
static Sint16 draw_56_polygons[943];
static Sint32 draw_56_vertices[611];
static NJS_CNK_MODEL draw_56_model;
static NJS_CNK_OBJECT draw_56;
static Sint16 draw_55_polygons[547];
static Sint32 draw_55_vertices[427];
static NJS_CNK_MODEL draw_55_model;
static NJS_CNK_OBJECT draw_55;
static Sint16 draw_39_polygons[249];
static Sint32 draw_39_vertices[215];
static NJS_CNK_MODEL draw_39_model;
static NJS_CNK_OBJECT draw_39;
static Sint16 draw_17_polygons[59];
static Sint32 draw_17_vertices[51];
static NJS_CNK_MODEL draw_17_model;
static NJS_CNK_OBJECT draw_17;
static Sint16 draw_6_polygons[1157];
static Sint32 draw_6_vertices[963];
static NJS_CNK_MODEL draw_6_model;
static NJS_CNK_OBJECT draw_6;
static Sint16 draw_3_polygons[1267];
static Sint32 draw_3_vertices[991];
static NJS_CNK_MODEL draw_3_model;
static NJS_CNK_OBJECT draw_3;
static NJS_OBJECT collision_1;
static NJS_OBJECT collision_2;
static NJS_OBJECT collision_3;
static NJS_OBJECT collision_4;
static NJS_OBJECT collision_5;
static NJS_OBJECT collision_7;
static NJS_OBJECT collision_9;
static NJS_OBJECT collision_12;
static NJS_OBJECT collision_13;
static NJS_OBJECT collision_14;
static NJS_OBJECT collision_16;
static NJS_OBJECT collision_24;
static NJS_OBJECT collision_32;
static NJS_OBJECT collision_34;
static NJS_OBJECT collision_35;
static NJS_OBJECT collision_36;
static NJS_OBJECT collision_37;
static NJS_OBJECT collision_38;
static NJS_OBJECT collision_39;
static NJS_OBJECT collision_40;
static NJS_OBJECT collision_41;
static NJS_OBJECT collision_42;
static NJS_OBJECT collision_46;
static NJS_OBJECT collision_47;
static NJS_OBJECT collision_48;
static NJS_OBJECT collision_49;
static NJS_OBJECT collision_50;
static NJS_OBJECT collision_51;
static NJS_OBJECT collision_53;
static NJS_OBJECT collision_54;
static NJS_OBJECT collision_55;
static NJS_OBJECT collision_56;
static NJS_OBJECT collision_58;
static NJS_OBJECT collision_59;
static NJS_OBJECT collision_60;
static NJS_OBJECT collision_61;
static NJS_OBJECT collision_62;
static NJS_OBJECT collision_64;
static NJS_OBJECT collision_65;
static NJS_OBJECT collision_66;
static NJS_OBJECT collision_67;
static NJS_OBJECT collision_73;
static NJS_OBJECT collision_75;
static NJS_OBJECT collision_84;
static NJS_OBJECT collision_88;
static NJS_OBJECT collision_89;
static NJS_OBJECT collision_91;
static NJS_OBJECT collision_93;
static NJS_OBJECT collision_94;
static NJS_OBJECT collision_96;
static NJS_OBJECT collision_101;
static NJS_OBJECT collision_102;
static NJS_OBJECT collision_103;
static NJS_OBJECT collision_106;
static NJS_OBJECT collision_108;
static NJS_OBJECT collision_109;
static NJS_OBJECT collision_111;
static NJS_OBJECT collision_113;
static NJS_OBJECT collision_116;
static NJS_OBJECT collision_118;
static NJS_OBJECT collision_121;
static NJS_OBJECT collision_125;
static NJS_OBJECT collision_126;
static NJS_OBJECT collision_127;
static NJS_OBJECT collision_128;
static NJS_OBJECT collision_129;
static NJS_OBJECT collision_131;
static NJS_OBJECT collision_132;
static NJS_OBJECT collision_133;
static NJS_OBJECT collision_135;
static NJS_OBJECT collision_137;
static NJS_OBJECT collision_138;
static NJS_OBJECT collision_140;
static NJS_OBJECT collision_141;
static NJS_OBJECT collision_145;
static NJS_OBJECT collision_147;
static NJS_OBJECT collision_150;
static NJS_OBJECT collision_154;
static NJS_OBJECT collision_158;
static NJS_OBJECT collision_159;
static NJS_OBJECT collision_160;
static NJS_OBJECT collision_161;
static NJS_OBJECT collision_162;
static NJS_OBJECT collision_163;
static NJS_OBJECT collision_164;
static NJS_OBJECT collision_166;
static NJS_OBJECT collision_168;
static NJS_OBJECT collision_169;
static NJS_OBJECT collision_170;
static NJS_OBJECT collision_171;
static NJS_OBJECT collision_172;
static NJS_OBJECT collision_173;
static NJS_OBJECT collision_174;
static NJS_OBJECT collision_175;
static NJS_OBJECT collision_176;
static NJS_OBJECT collision_178;
static NJS_OBJECT collision_180;
static NJS_OBJECT collision_181;
static NJS_OBJECT collision_182;
static NJS_OBJECT collision_183;
static NJS_OBJECT collision_184;
static NJS_OBJECT collision_185;
static NJS_OBJECT collision_186;
static NJS_OBJECT collision_187;
static NJS_OBJECT collision_188;
static NJS_OBJECT collision_192;
static NJS_OBJECT collision_205;
static NJS_OBJECT collision_206;
static NJS_OBJECT collision_209;
static NJS_OBJECT collision_210;
static NJS_OBJECT collision_211;
static NJS_OBJECT collision_212;
static NJS_OBJECT collision_213;
static NJS_OBJECT collision_214;
static NJS_OBJECT collision_216;
static NJS_OBJECT collision_217;
static NJS_OBJECT collision_219;
static NJS_OBJECT collision_221;
static NJS_OBJECT collision_222;
static NJS_OBJECT collision_223;
static NJS_OBJECT collision_224;
static NJS_OBJECT collision_226;
static NJS_OBJECT collision_227;
static NJS_OBJECT collision_228;
static NJS_OBJECT collision_229;
static NJS_OBJECT collision_230;
static NJS_OBJECT collision_237;
static NJS_OBJECT collision_238;
static NJS_OBJECT collision_240;
static NJS_OBJECT collision_241;
static NJS_OBJECT collision_242;
static NJS_OBJECT collision_243;
static NJS_OBJECT collision_244;
static NJS_OBJECT collision_245;
static NJS_OBJECT collision_247;
static NJS_OBJECT collision_250;
static NJS_OBJECT collision_251;
static NJS_OBJECT collision_252;
static NJS_OBJECT collision_253;
static NJS_OBJECT collision_254;
static NJS_OBJECT collision_255;
static NJS_OBJECT collision_256;
static NJS_OBJECT collision_257;
static NJS_OBJECT collision_258;
static NJS_OBJECT collision_259;
static NJS_OBJECT collision_260;
static NJS_OBJECT collision_261;
static NJS_OBJECT collision_262;
static NJS_OBJECT collision_263;
static NJS_OBJECT collision_264;
static NJS_OBJECT collision_266;
static NJS_OBJECT collision_269;
static NJS_OBJECT collision_270;
static NJS_OBJECT collision_273;
static NJS_OBJECT collision_279;
static NJS_OBJECT collision_284;
static NJS_OBJECT collision_285;
static NJS_OBJECT collision_293;
static NJS_OBJECT collision_296;
static NJS_OBJECT collision_297;
static NJS_OBJECT collision_298;
static NJS_OBJECT collision_310;
static NJS_OBJECT collision_312;
static NJS_OBJECT collision_313;
static NJS_OBJECT collision_314;
static NJS_OBJECT collision_316;
static NJS_OBJECT collision_317;
static NJS_OBJECT collision_319;
static NJS_OBJECT collision_321;
static NJS_OBJECT collision_323;
static NJS_OBJECT collision_325;
static NJS_OBJECT collision_329;
static NJS_OBJECT collision_331;
static NJS_OBJECT collision_332;
static NJS_OBJECT collision_333;
static NJS_OBJECT collision_334;
static NJS_OBJECT collision_337;
static NJS_OBJECT collision_339;
static NJS_OBJECT collision_341;
static NJS_OBJECT collision_342;
static NJS_OBJECT collision_343;
static NJS_OBJECT collision_344;
static NJS_OBJECT collision_345;
static NJS_OBJECT collision_346;
static NJS_OBJECT collision_347;
static NJS_OBJECT collision_348;
static NJS_OBJECT collision_350;
static NJS_OBJECT collision_352;
static NJS_OBJECT collision_363;
static NJS_OBJECT collision_365;
static NJS_OBJECT collision_366;
static NJS_OBJECT collision_367;
static NJS_OBJECT collision_368;
static NJS_OBJECT collision_369;
static NJS_OBJECT collision_372;
static NJS_OBJECT collision_373;
static NJS_OBJECT collision_374;
static NJS_OBJECT collision_376;
static NJS_OBJECT collision_378;
static NJS_OBJECT collision_379;
static NJS_OBJECT collision_381;
static NJS_OBJECT collision_382;
static NJS_OBJECT collision_386;
static NJS_OBJECT collision_388;
static NJS_OBJECT collision_390;
static NJS_OBJECT collision_391;
static NJS_OBJECT collision_393;
static NJS_OBJECT collision_394;
static NJS_OBJECT collision_395;
static NJS_OBJECT collision_396;
static NJS_OBJECT collision_397;
static NJS_OBJECT collision_400;
static NJS_OBJECT collision_403;
static NJS_OBJECT collision_415;
static NJS_CNK_OBJECT draw_706;
static NJS_CNK_OBJECT draw_705;
static NJS_CNK_OBJECT draw_703;
static NJS_CNK_OBJECT draw_702;
static NJS_CNK_OBJECT draw_701;
static NJS_CNK_OBJECT draw_700;
static NJS_CNK_OBJECT draw_699;
static NJS_CNK_OBJECT draw_698;
static NJS_CNK_OBJECT draw_697;
static NJS_CNK_OBJECT draw_696;
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
static NJS_CNK_OBJECT draw_678;
static NJS_CNK_OBJECT draw_677;
static NJS_CNK_OBJECT draw_676;
static NJS_CNK_OBJECT draw_674;
static NJS_CNK_OBJECT draw_673;
static NJS_CNK_OBJECT draw_672;
static NJS_CNK_OBJECT draw_671;
static NJS_CNK_OBJECT draw_670;
static NJS_CNK_OBJECT draw_669;
static NJS_CNK_OBJECT draw_668;
static NJS_CNK_OBJECT draw_667;
static NJS_CNK_OBJECT draw_665;
static NJS_CNK_OBJECT draw_664;
static NJS_CNK_OBJECT draw_663;
static NJS_CNK_OBJECT draw_662;
static NJS_CNK_OBJECT draw_661;
static NJS_CNK_OBJECT draw_660;
static NJS_CNK_OBJECT draw_659;
static NJS_CNK_OBJECT draw_658;
static NJS_CNK_OBJECT draw_656;
static NJS_CNK_OBJECT draw_648;
static NJS_CNK_OBJECT draw_647;
static NJS_CNK_OBJECT draw_646;
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
static NJS_CNK_OBJECT draw_625;
static NJS_CNK_OBJECT draw_624;
static NJS_CNK_OBJECT draw_623;
static NJS_CNK_OBJECT draw_619;
static NJS_CNK_OBJECT draw_618;
static NJS_CNK_OBJECT draw_616;
static NJS_CNK_OBJECT draw_615;
static NJS_CNK_OBJECT draw_614;
static NJS_CNK_OBJECT draw_613;
static NJS_CNK_OBJECT draw_612;
static NJS_CNK_OBJECT draw_611;
static NJS_CNK_OBJECT draw_610;
static NJS_CNK_OBJECT draw_609;
static NJS_CNK_OBJECT draw_606;
static NJS_CNK_OBJECT draw_605;
static NJS_CNK_OBJECT draw_602;
static NJS_CNK_OBJECT draw_601;
static NJS_CNK_OBJECT draw_593;
static NJS_CNK_OBJECT draw_592;
static NJS_CNK_OBJECT draw_591;
static NJS_CNK_OBJECT draw_589;
static NJS_CNK_OBJECT draw_588;
static NJS_CNK_OBJECT draw_587;
static NJS_CNK_OBJECT draw_586;
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
static NJS_CNK_OBJECT draw_556;
static NJS_CNK_OBJECT draw_555;
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
static NJS_CNK_OBJECT draw_541;
static NJS_CNK_OBJECT draw_537;
static NJS_CNK_OBJECT draw_536;
static NJS_CNK_OBJECT draw_535;
static NJS_CNK_OBJECT draw_534;
static NJS_CNK_OBJECT draw_532;
static NJS_CNK_OBJECT draw_528;
static NJS_CNK_OBJECT draw_527;
static NJS_CNK_OBJECT draw_524;
static NJS_CNK_OBJECT draw_523;
static NJS_CNK_OBJECT draw_522;
static NJS_CNK_OBJECT draw_521;
static NJS_CNK_OBJECT draw_520;
static NJS_CNK_OBJECT draw_511;
static NJS_CNK_OBJECT draw_510;
static NJS_CNK_OBJECT draw_509;
static NJS_CNK_OBJECT draw_508;
static NJS_CNK_OBJECT draw_507;
static NJS_CNK_OBJECT draw_506;
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
static NJS_CNK_OBJECT draw_472;
static NJS_CNK_OBJECT draw_471;
static NJS_CNK_OBJECT draw_470;
static NJS_CNK_OBJECT draw_469;
static NJS_CNK_OBJECT draw_467;
static NJS_CNK_OBJECT draw_466;
static NJS_CNK_OBJECT draw_465;
static NJS_CNK_OBJECT draw_464;
static NJS_CNK_OBJECT draw_463;
static NJS_CNK_OBJECT draw_461;
static NJS_CNK_OBJECT draw_459;
static NJS_CNK_OBJECT draw_458;
static NJS_CNK_OBJECT draw_457;
static NJS_CNK_OBJECT draw_453;
static NJS_CNK_OBJECT draw_452;
static NJS_CNK_OBJECT draw_451;
static NJS_CNK_OBJECT draw_450;
static NJS_CNK_OBJECT draw_449;
static NJS_CNK_OBJECT draw_448;
static NJS_CNK_OBJECT draw_447;
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
static NJS_CNK_OBJECT draw_428;
static NJS_CNK_OBJECT draw_427;
static NJS_CNK_OBJECT draw_425;
static NJS_CNK_OBJECT draw_424;
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
static NJS_CNK_OBJECT draw_299;
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
static NJS_CNK_OBJECT draw_258;
static NJS_CNK_OBJECT draw_257;
static NJS_CNK_OBJECT draw_256;
static NJS_CNK_OBJECT draw_255;
static NJS_CNK_OBJECT draw_254;
static NJS_CNK_OBJECT draw_253;
static NJS_CNK_OBJECT draw_252;
static NJS_CNK_OBJECT draw_251;
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
static NJS_CNK_OBJECT draw_225;
static NJS_CNK_OBJECT draw_224;
static NJS_CNK_OBJECT draw_223;
static NJS_CNK_OBJECT draw_222;
static NJS_CNK_OBJECT draw_221;
static NJS_CNK_OBJECT draw_220;
static NJS_CNK_OBJECT draw_219;
static NJS_CNK_OBJECT draw_217;
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
static NJS_CNK_OBJECT draw_195;
static NJS_CNK_OBJECT draw_193;
static NJS_CNK_OBJECT draw_192;
static NJS_CNK_OBJECT draw_191;
static NJS_CNK_OBJECT draw_190;
static NJS_CNK_OBJECT draw_189;
static NJS_CNK_OBJECT draw_188;
static NJS_CNK_OBJECT draw_187;
static NJS_CNK_OBJECT draw_186;
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
static NJS_CNK_OBJECT draw_150;
static NJS_CNK_OBJECT draw_148;
static NJS_CNK_OBJECT draw_147;
static NJS_CNK_OBJECT draw_146;
static NJS_CNK_OBJECT draw_145;
static NJS_CNK_OBJECT draw_144;
static NJS_CNK_OBJECT draw_143;
static NJS_CNK_OBJECT draw_141;
static NJS_CNK_OBJECT draw_140;
static NJS_CNK_OBJECT draw_139;
static NJS_CNK_OBJECT draw_138;
static NJS_CNK_OBJECT draw_137;
static NJS_CNK_OBJECT draw_136;
static NJS_CNK_OBJECT draw_135;
static NJS_CNK_OBJECT draw_133;
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
static NJS_CNK_OBJECT draw_80;
static NJS_CNK_OBJECT draw_79;
static NJS_CNK_OBJECT draw_78;
static NJS_CNK_OBJECT draw_77;
static NJS_CNK_OBJECT draw_76;
static NJS_CNK_OBJECT draw_75;
static NJS_CNK_OBJECT draw_74;
static NJS_CNK_OBJECT draw_73;
static NJS_CNK_OBJECT draw_71;
static NJS_CNK_OBJECT draw_70;
static NJS_CNK_OBJECT draw_69;
static NJS_CNK_OBJECT draw_68;
static NJS_CNK_OBJECT draw_67;
static NJS_CNK_OBJECT draw_63;
static NJS_CNK_OBJECT draw_62;
static NJS_CNK_OBJECT draw_61;
static NJS_CNK_OBJECT draw_60;
static NJS_CNK_OBJECT draw_59;
static NJS_CNK_OBJECT draw_57;
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
static NJS_CNK_OBJECT draw_38;
static NJS_CNK_OBJECT draw_37;
static NJS_CNK_OBJECT draw_36;
static NJS_CNK_OBJECT draw_35;
static NJS_CNK_OBJECT draw_34;
static NJS_CNK_OBJECT draw_33;
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
static NJS_CNK_OBJECT draw_22;
static NJS_CNK_OBJECT draw_21;
static NJS_CNK_OBJECT draw_20;
static NJS_CNK_OBJECT draw_19;
static NJS_CNK_OBJECT draw_18;
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
static NJS_CNK_OBJECT draw_5;
static NJS_CNK_OBJECT draw_4;
static NJS_CNK_OBJECT draw_2;
static NJS_CNK_OBJECT draw_1;
static NJS_CNK_OBJECT draw_0;

static NJS_MKEY_A upgrade_radical_highway_motion_joint_4_angles[16] = {
#include "stg14/terrain/upgrade_radical_highway_motion_joint_4_angles.inc"
};
static NJS_MKEY_A upgrade_radical_highway_motion_joint_7_angles[16] = {
#include "stg14/terrain/upgrade_radical_highway_motion_joint_7_angles.inc"
};
static NJS_MKEY_A upgrade_radical_highway_motion_joint_8_angles[7] = {
#include "stg14/terrain/upgrade_radical_highway_motion_joint_8_angles.inc"
};
static NJS_MKEY_A upgrade_radical_highway_motion_joint_9_angles[12] = {
#include "stg14/terrain/upgrade_radical_highway_motion_joint_9_angles.inc"
};
static NJS_MKEY_A upgrade_radical_highway_motion_joint_13_angles[16] = {
#include "stg14/terrain/upgrade_radical_highway_motion_joint_13_angles.inc"
};
static NJS_MKEY_A upgrade_radical_highway_motion_joint_14_angles[2] = {
#include "stg14/terrain/upgrade_radical_highway_motion_joint_14_angles.inc"
};
static NJS_MKEY_A upgrade_radical_highway_motion_joint_17_angles[16] = {
#include "stg14/terrain/upgrade_radical_highway_motion_joint_17_angles.inc"
};
static NJS_MKEY_A upgrade_radical_highway_motion_joint_18_angles[8] = {
#include "stg14/terrain/upgrade_radical_highway_motion_joint_18_angles.inc"
};
static NJS_MKEY_A upgrade_radical_highway_motion_joint_19_angles[14] = {
#include "stg14/terrain/upgrade_radical_highway_motion_joint_19_angles.inc"
};
static NJS_MKEY_A upgrade_radical_highway_motion_joint_23_angles[16] = {
#include "stg14/terrain/upgrade_radical_highway_motion_joint_23_angles.inc"
};
static NJS_MKEY_A upgrade_radical_highway_motion_joint_24_angles[2] = {
#include "stg14/terrain/upgrade_radical_highway_motion_joint_24_angles.inc"
};
static NJS_MKEY_A upgrade_radical_highway_motion_joint_27_angles[16] = {
#include "stg14/terrain/upgrade_radical_highway_motion_joint_27_angles.inc"
};
static NJS_MKEY_A upgrade_radical_highway_motion_joint_28_angles[16] = {
#include "stg14/terrain/upgrade_radical_highway_motion_joint_28_angles.inc"
};
static NJS_MKEY_A upgrade_radical_highway_motion_joint_29_angles[16] = {
#include "stg14/terrain/upgrade_radical_highway_motion_joint_29_angles.inc"
};
static NJS_MKEY_A upgrade_radical_highway_motion_joint_31_angles[2] = {
#include "stg14/terrain/upgrade_radical_highway_motion_joint_31_angles.inc"
};
static NJS_MKEY_A upgrade_radical_highway_motion_joint_32_angles[16] = {
#include "stg14/terrain/upgrade_radical_highway_motion_joint_32_angles.inc"
};
static NJS_MKEY_A upgrade_radical_highway_motion_joint_33_angles[13] = {
#include "stg14/terrain/upgrade_radical_highway_motion_joint_33_angles.inc"
};
static NJS_MKEY_A upgrade_radical_highway_motion_joint_37_angles[16] = {
#include "stg14/terrain/upgrade_radical_highway_motion_joint_37_angles.inc"
};
static NJS_MKEY_A upgrade_radical_highway_motion_joint_38_angles[16] = {
#include "stg14/terrain/upgrade_radical_highway_motion_joint_38_angles.inc"
};
static NJS_MKEY_A upgrade_radical_highway_motion_joint_39_angles[16] = {
#include "stg14/terrain/upgrade_radical_highway_motion_joint_39_angles.inc"
};
static NJS_MKEY_A upgrade_radical_highway_motion_joint_41_angles[16] = {
#include "stg14/terrain/upgrade_radical_highway_motion_joint_41_angles.inc"
};
static NJS_MKEY_A upgrade_radical_highway_motion_joint_44_angles[16] = {
#include "stg14/terrain/upgrade_radical_highway_motion_joint_44_angles.inc"
};
static NJS_MKEY_A upgrade_radical_highway_motion_joint_51_angles[16] = {
#include "stg14/terrain/upgrade_radical_highway_motion_joint_51_angles.inc"
};
static NJS_MKEY_A upgrade_radical_highway_motion_joint_53_angles[16] = {
#include "stg14/terrain/upgrade_radical_highway_motion_joint_53_angles.inc"
};
static NJS_MKEY_A upgrade_radical_highway_motion_joint_56_angles[16] = {
#include "stg14/terrain/upgrade_radical_highway_motion_joint_56_angles.inc"
};
static NJS_MKEY_A upgrade_radical_highway_motion_joint_57_angles[16] = {
#include "stg14/terrain/upgrade_radical_highway_motion_joint_57_angles.inc"
};
static NJS_MKEY_A upgrade_radical_highway_motion_joint_58_angles[16] = {
#include "stg14/terrain/upgrade_radical_highway_motion_joint_58_angles.inc"
};
static NJS_MKEY_A upgrade_radical_highway_motion_joint_60_angles[16] = {
#include "stg14/terrain/upgrade_radical_highway_motion_joint_60_angles.inc"
};
static NJS_MDATA2 upgrade_radical_highway_motion_tracks[62] = {
#include "stg14/terrain/upgrade_radical_highway_motion_tracks.inc"
};
static NJS_MOTION upgrade_radical_highway_motion = {
#include "stg14/terrain/upgrade_radical_highway_motion.inc"
};
static NJS_MKEY_F bar_throw_back_motion_joint_0_position[2] = {
#include "stg14/terrain/bar_throw_back_motion_joint_0_position.inc"
};
static NJS_MKEY_F bar_throw_back_motion_joint_2_position[2] = {
#include "stg14/terrain/bar_throw_back_motion_joint_2_position.inc"
};
static NJS_MKEY_A bar_throw_back_motion_joint_4_angles[2] = {
#include "stg14/terrain/bar_throw_back_motion_joint_4_angles.inc"
};
static NJS_MKEY_A bar_throw_back_motion_joint_7_angles[32] = {
#include "stg14/terrain/bar_throw_back_motion_joint_7_angles.inc"
};
static NJS_MKEY_A bar_throw_back_motion_joint_8_angles[32] = {
#include "stg14/terrain/bar_throw_back_motion_joint_8_angles.inc"
};
static NJS_MKEY_A bar_throw_back_motion_joint_9_angles[32] = {
#include "stg14/terrain/bar_throw_back_motion_joint_9_angles.inc"
};
static NJS_MKEY_A bar_throw_back_motion_joint_13_angles[32] = {
#include "stg14/terrain/bar_throw_back_motion_joint_13_angles.inc"
};
static NJS_MKEY_A bar_throw_back_motion_joint_14_angles[32] = {
#include "stg14/terrain/bar_throw_back_motion_joint_14_angles.inc"
};
static NJS_MKEY_A bar_throw_back_motion_joint_17_angles[32] = {
#include "stg14/terrain/bar_throw_back_motion_joint_17_angles.inc"
};
static NJS_MKEY_A bar_throw_back_motion_joint_18_angles[32] = {
#include "stg14/terrain/bar_throw_back_motion_joint_18_angles.inc"
};
static NJS_MKEY_A bar_throw_back_motion_joint_19_angles[32] = {
#include "stg14/terrain/bar_throw_back_motion_joint_19_angles.inc"
};
static NJS_MKEY_A bar_throw_back_motion_joint_23_angles[32] = {
#include "stg14/terrain/bar_throw_back_motion_joint_23_angles.inc"
};
static NJS_MKEY_A bar_throw_back_motion_joint_24_angles[32] = {
#include "stg14/terrain/bar_throw_back_motion_joint_24_angles.inc"
};
static NJS_MKEY_A bar_throw_back_motion_joint_27_angles[2] = {
#include "stg14/terrain/bar_throw_back_motion_joint_27_angles.inc"
};
static NJS_MKEY_A bar_throw_back_motion_joint_28_angles[2] = {
#include "stg14/terrain/bar_throw_back_motion_joint_28_angles.inc"
};
static NJS_MKEY_A bar_throw_back_motion_joint_29_angles[2] = {
#include "stg14/terrain/bar_throw_back_motion_joint_29_angles.inc"
};
static NJS_MKEY_A bar_throw_back_motion_joint_32_angles[32] = {
#include "stg14/terrain/bar_throw_back_motion_joint_32_angles.inc"
};
static NJS_MKEY_A bar_throw_back_motion_joint_33_angles[25] = {
#include "stg14/terrain/bar_throw_back_motion_joint_33_angles.inc"
};
static NJS_MKEY_A bar_throw_back_motion_joint_37_angles[32] = {
#include "stg14/terrain/bar_throw_back_motion_joint_37_angles.inc"
};
static NJS_MKEY_A bar_throw_back_motion_joint_38_angles[32] = {
#include "stg14/terrain/bar_throw_back_motion_joint_38_angles.inc"
};
static NJS_MKEY_A bar_throw_back_motion_joint_39_angles[32] = {
#include "stg14/terrain/bar_throw_back_motion_joint_39_angles.inc"
};
static NJS_MKEY_A bar_throw_back_motion_joint_41_angles[17] = {
#include "stg14/terrain/bar_throw_back_motion_joint_41_angles.inc"
};
static NJS_MKEY_A bar_throw_back_motion_joint_44_angles[32] = {
#include "stg14/terrain/bar_throw_back_motion_joint_44_angles.inc"
};
static NJS_MKEY_A bar_throw_back_motion_joint_51_angles[32] = {
#include "stg14/terrain/bar_throw_back_motion_joint_51_angles.inc"
};
static NJS_MKEY_A bar_throw_back_motion_joint_53_angles[32] = {
#include "stg14/terrain/bar_throw_back_motion_joint_53_angles.inc"
};
static NJS_MKEY_A bar_throw_back_motion_joint_56_angles[32] = {
#include "stg14/terrain/bar_throw_back_motion_joint_56_angles.inc"
};
static NJS_MKEY_A bar_throw_back_motion_joint_57_angles[32] = {
#include "stg14/terrain/bar_throw_back_motion_joint_57_angles.inc"
};
static NJS_MKEY_A bar_throw_back_motion_joint_58_angles[32] = {
#include "stg14/terrain/bar_throw_back_motion_joint_58_angles.inc"
};
static NJS_MKEY_A bar_throw_back_motion_joint_60_angles[17] = {
#include "stg14/terrain/bar_throw_back_motion_joint_60_angles.inc"
};
static NJS_MDATA2 bar_throw_back_motion_tracks[62] = {
#include "stg14/terrain/bar_throw_back_motion_tracks.inc"
};
static NJS_MOTION bar_throw_back_motion = {
#include "stg14/terrain/bar_throw_back_motion.inc"
};
static NJS_MKEY_F bar_throw_motion_joint_0_position[2] = {
#include "stg14/terrain/bar_throw_motion_joint_0_position.inc"
};
static NJS_MKEY_F bar_throw_motion_joint_2_position[2] = {
#include "stg14/terrain/bar_throw_motion_joint_2_position.inc"
};
static NJS_MKEY_A bar_throw_motion_joint_4_angles[33] = {
#include "stg14/terrain/bar_throw_motion_joint_4_angles.inc"
};
static NJS_MKEY_A bar_throw_motion_joint_7_angles[33] = {
#include "stg14/terrain/bar_throw_motion_joint_7_angles.inc"
};
static NJS_MKEY_A bar_throw_motion_joint_8_angles[33] = {
#include "stg14/terrain/bar_throw_motion_joint_8_angles.inc"
};
static NJS_MKEY_A bar_throw_motion_joint_9_angles[33] = {
#include "stg14/terrain/bar_throw_motion_joint_9_angles.inc"
};
static NJS_MKEY_A bar_throw_motion_joint_13_angles[33] = {
#include "stg14/terrain/bar_throw_motion_joint_13_angles.inc"
};
static NJS_MKEY_A bar_throw_motion_joint_14_angles[33] = {
#include "stg14/terrain/bar_throw_motion_joint_14_angles.inc"
};
static NJS_MKEY_A bar_throw_motion_joint_17_angles[33] = {
#include "stg14/terrain/bar_throw_motion_joint_17_angles.inc"
};
static NJS_MKEY_A bar_throw_motion_joint_18_angles[33] = {
#include "stg14/terrain/bar_throw_motion_joint_18_angles.inc"
};
static NJS_MKEY_A bar_throw_motion_joint_19_angles[33] = {
#include "stg14/terrain/bar_throw_motion_joint_19_angles.inc"
};
static NJS_MKEY_A bar_throw_motion_joint_23_angles[33] = {
#include "stg14/terrain/bar_throw_motion_joint_23_angles.inc"
};
static NJS_MKEY_A bar_throw_motion_joint_24_angles[33] = {
#include "stg14/terrain/bar_throw_motion_joint_24_angles.inc"
};
static NJS_MKEY_A bar_throw_motion_joint_27_angles[33] = {
#include "stg14/terrain/bar_throw_motion_joint_27_angles.inc"
};
static NJS_MKEY_A bar_throw_motion_joint_28_angles[15] = {
#include "stg14/terrain/bar_throw_motion_joint_28_angles.inc"
};
static NJS_MKEY_A bar_throw_motion_joint_29_angles[15] = {
#include "stg14/terrain/bar_throw_motion_joint_29_angles.inc"
};
static NJS_MKEY_A bar_throw_motion_joint_32_angles[33] = {
#include "stg14/terrain/bar_throw_motion_joint_32_angles.inc"
};
static NJS_MKEY_A bar_throw_motion_joint_33_angles[26] = {
#include "stg14/terrain/bar_throw_motion_joint_33_angles.inc"
};
static NJS_MKEY_A bar_throw_motion_joint_37_angles[33] = {
#include "stg14/terrain/bar_throw_motion_joint_37_angles.inc"
};
static NJS_MKEY_A bar_throw_motion_joint_38_angles[33] = {
#include "stg14/terrain/bar_throw_motion_joint_38_angles.inc"
};
static NJS_MKEY_A bar_throw_motion_joint_39_angles[33] = {
#include "stg14/terrain/bar_throw_motion_joint_39_angles.inc"
};
static NJS_MKEY_A bar_throw_motion_joint_41_angles[2] = {
#include "stg14/terrain/bar_throw_motion_joint_41_angles.inc"
};
static NJS_MKEY_A bar_throw_motion_joint_44_angles[33] = {
#include "stg14/terrain/bar_throw_motion_joint_44_angles.inc"
};
static NJS_MKEY_A bar_throw_motion_joint_51_angles[33] = {
#include "stg14/terrain/bar_throw_motion_joint_51_angles.inc"
};
static NJS_MKEY_A bar_throw_motion_joint_53_angles[33] = {
#include "stg14/terrain/bar_throw_motion_joint_53_angles.inc"
};
static NJS_MKEY_A bar_throw_motion_joint_56_angles[33] = {
#include "stg14/terrain/bar_throw_motion_joint_56_angles.inc"
};
static NJS_MKEY_A bar_throw_motion_joint_57_angles[33] = {
#include "stg14/terrain/bar_throw_motion_joint_57_angles.inc"
};
static NJS_MKEY_A bar_throw_motion_joint_58_angles[33] = {
#include "stg14/terrain/bar_throw_motion_joint_58_angles.inc"
};
static NJS_MKEY_A bar_throw_motion_joint_60_angles[2] = {
#include "stg14/terrain/bar_throw_motion_joint_60_angles.inc"
};
static NJS_MDATA2 bar_throw_motion_tracks[62] = {
#include "stg14/terrain/bar_throw_motion_tracks.inc"
};
static NJS_MOTION bar_throw_motion = {
#include "stg14/terrain/bar_throw_motion.inc"
};
static NJS_MKEY_F bar_fly_back_motion_joint_0_position[2] = {
#include "stg14/terrain/bar_fly_back_motion_joint_0_position.inc"
};
static NJS_MKEY_F bar_fly_back_motion_joint_2_position[2] = {
#include "stg14/terrain/bar_fly_back_motion_joint_2_position.inc"
};
static NJS_MKEY_A bar_fly_back_motion_joint_4_angles[2] = {
#include "stg14/terrain/bar_fly_back_motion_joint_4_angles.inc"
};
static NJS_MKEY_A bar_fly_back_motion_joint_7_angles[2] = {
#include "stg14/terrain/bar_fly_back_motion_joint_7_angles.inc"
};
static NJS_MKEY_A bar_fly_back_motion_joint_8_angles[2] = {
#include "stg14/terrain/bar_fly_back_motion_joint_8_angles.inc"
};
static NJS_MKEY_A bar_fly_back_motion_joint_9_angles[2] = {
#include "stg14/terrain/bar_fly_back_motion_joint_9_angles.inc"
};
static NJS_MKEY_A bar_fly_back_motion_joint_13_angles[2] = {
#include "stg14/terrain/bar_fly_back_motion_joint_13_angles.inc"
};
static NJS_MKEY_A bar_fly_back_motion_joint_14_angles[2] = {
#include "stg14/terrain/bar_fly_back_motion_joint_14_angles.inc"
};
static NJS_MKEY_A bar_fly_back_motion_joint_17_angles[2] = {
#include "stg14/terrain/bar_fly_back_motion_joint_17_angles.inc"
};
static NJS_MKEY_A bar_fly_back_motion_joint_18_angles[2] = {
#include "stg14/terrain/bar_fly_back_motion_joint_18_angles.inc"
};
static NJS_MKEY_A bar_fly_back_motion_joint_19_angles[2] = {
#include "stg14/terrain/bar_fly_back_motion_joint_19_angles.inc"
};
static NJS_MKEY_A bar_fly_back_motion_joint_23_angles[2] = {
#include "stg14/terrain/bar_fly_back_motion_joint_23_angles.inc"
};
static NJS_MKEY_A bar_fly_back_motion_joint_24_angles[2] = {
#include "stg14/terrain/bar_fly_back_motion_joint_24_angles.inc"
};
static NJS_MKEY_A bar_fly_back_motion_joint_27_angles[2] = {
#include "stg14/terrain/bar_fly_back_motion_joint_27_angles.inc"
};
static NJS_MKEY_A bar_fly_back_motion_joint_32_angles[2] = {
#include "stg14/terrain/bar_fly_back_motion_joint_32_angles.inc"
};
static NJS_MKEY_A bar_fly_back_motion_joint_37_angles[2] = {
#include "stg14/terrain/bar_fly_back_motion_joint_37_angles.inc"
};
static NJS_MKEY_A bar_fly_back_motion_joint_38_angles[2] = {
#include "stg14/terrain/bar_fly_back_motion_joint_38_angles.inc"
};
static NJS_MKEY_A bar_fly_back_motion_joint_39_angles[2] = {
#include "stg14/terrain/bar_fly_back_motion_joint_39_angles.inc"
};
static NJS_MKEY_A bar_fly_back_motion_joint_41_angles[2] = {
#include "stg14/terrain/bar_fly_back_motion_joint_41_angles.inc"
};
static NJS_MKEY_A bar_fly_back_motion_joint_44_angles[2] = {
#include "stg14/terrain/bar_fly_back_motion_joint_44_angles.inc"
};
static NJS_MKEY_A bar_fly_back_motion_joint_51_angles[2] = {
#include "stg14/terrain/bar_fly_back_motion_joint_51_angles.inc"
};
static NJS_MKEY_A bar_fly_back_motion_joint_53_angles[2] = {
#include "stg14/terrain/bar_fly_back_motion_joint_53_angles.inc"
};
static NJS_MKEY_A bar_fly_back_motion_joint_56_angles[6] = {
#include "stg14/terrain/bar_fly_back_motion_joint_56_angles.inc"
};
static NJS_MKEY_A bar_fly_back_motion_joint_57_angles[2] = {
#include "stg14/terrain/bar_fly_back_motion_joint_57_angles.inc"
};
static NJS_MKEY_A bar_fly_back_motion_joint_58_angles[3] = {
#include "stg14/terrain/bar_fly_back_motion_joint_58_angles.inc"
};
static NJS_MKEY_A bar_fly_back_motion_joint_60_angles[2] = {
#include "stg14/terrain/bar_fly_back_motion_joint_60_angles.inc"
};
static NJS_MDATA2 bar_fly_back_motion_tracks[62] = {
#include "stg14/terrain/bar_fly_back_motion_tracks.inc"
};
static NJS_MOTION bar_fly_back_motion = {
#include "stg14/terrain/bar_fly_back_motion.inc"
};
static NJS_MKEY_F bar_fly_motion_joint_0_position[2] = {
#include "stg14/terrain/bar_fly_motion_joint_0_position.inc"
};
static NJS_MKEY_F bar_fly_motion_joint_2_position[2] = {
#include "stg14/terrain/bar_fly_motion_joint_2_position.inc"
};
static NJS_MKEY_A bar_fly_motion_joint_4_angles[2] = {
#include "stg14/terrain/bar_fly_motion_joint_4_angles.inc"
};
static NJS_MKEY_A bar_fly_motion_joint_7_angles[2] = {
#include "stg14/terrain/bar_fly_motion_joint_7_angles.inc"
};
static NJS_MKEY_A bar_fly_motion_joint_8_angles[2] = {
#include "stg14/terrain/bar_fly_motion_joint_8_angles.inc"
};
static NJS_MKEY_A bar_fly_motion_joint_9_angles[2] = {
#include "stg14/terrain/bar_fly_motion_joint_9_angles.inc"
};
static NJS_MKEY_A bar_fly_motion_joint_13_angles[2] = {
#include "stg14/terrain/bar_fly_motion_joint_13_angles.inc"
};
static NJS_MKEY_A bar_fly_motion_joint_14_angles[2] = {
#include "stg14/terrain/bar_fly_motion_joint_14_angles.inc"
};
static NJS_MKEY_A bar_fly_motion_joint_17_angles[2] = {
#include "stg14/terrain/bar_fly_motion_joint_17_angles.inc"
};
static NJS_MKEY_A bar_fly_motion_joint_18_angles[2] = {
#include "stg14/terrain/bar_fly_motion_joint_18_angles.inc"
};
static NJS_MKEY_A bar_fly_motion_joint_19_angles[2] = {
#include "stg14/terrain/bar_fly_motion_joint_19_angles.inc"
};
static NJS_MKEY_A bar_fly_motion_joint_23_angles[2] = {
#include "stg14/terrain/bar_fly_motion_joint_23_angles.inc"
};
static NJS_MKEY_A bar_fly_motion_joint_24_angles[2] = {
#include "stg14/terrain/bar_fly_motion_joint_24_angles.inc"
};
static NJS_MKEY_A bar_fly_motion_joint_27_angles[2] = {
#include "stg14/terrain/bar_fly_motion_joint_27_angles.inc"
};
static NJS_MKEY_A bar_fly_motion_joint_32_angles[2] = {
#include "stg14/terrain/bar_fly_motion_joint_32_angles.inc"
};
static NJS_MKEY_A bar_fly_motion_joint_37_angles[2] = {
#include "stg14/terrain/bar_fly_motion_joint_37_angles.inc"
};
static NJS_MKEY_A bar_fly_motion_joint_38_angles[2] = {
#include "stg14/terrain/bar_fly_motion_joint_38_angles.inc"
};
static NJS_MKEY_A bar_fly_motion_joint_39_angles[2] = {
#include "stg14/terrain/bar_fly_motion_joint_39_angles.inc"
};
static NJS_MKEY_A bar_fly_motion_joint_41_angles[2] = {
#include "stg14/terrain/bar_fly_motion_joint_41_angles.inc"
};
static NJS_MKEY_A bar_fly_motion_joint_44_angles[2] = {
#include "stg14/terrain/bar_fly_motion_joint_44_angles.inc"
};
static NJS_MKEY_A bar_fly_motion_joint_51_angles[2] = {
#include "stg14/terrain/bar_fly_motion_joint_51_angles.inc"
};
static NJS_MKEY_A bar_fly_motion_joint_53_angles[2] = {
#include "stg14/terrain/bar_fly_motion_joint_53_angles.inc"
};
static NJS_MKEY_A bar_fly_motion_joint_56_angles[3] = {
#include "stg14/terrain/bar_fly_motion_joint_56_angles.inc"
};
static NJS_MKEY_A bar_fly_motion_joint_57_angles[2] = {
#include "stg14/terrain/bar_fly_motion_joint_57_angles.inc"
};
static NJS_MKEY_A bar_fly_motion_joint_58_angles[2] = {
#include "stg14/terrain/bar_fly_motion_joint_58_angles.inc"
};
static NJS_MKEY_A bar_fly_motion_joint_60_angles[2] = {
#include "stg14/terrain/bar_fly_motion_joint_60_angles.inc"
};
static NJS_MDATA2 bar_fly_motion_tracks[62] = {
#include "stg14/terrain/bar_fly_motion_tracks.inc"
};
static NJS_MOTION bar_fly_motion = {
#include "stg14/terrain/bar_fly_motion.inc"
};
static NJS_MKEY_F bar_swing_back_motion_joint_0_position[2] = {
#include "stg14/terrain/bar_swing_back_motion_joint_0_position.inc"
};
static NJS_MKEY_F bar_swing_back_motion_joint_2_position[2] = {
#include "stg14/terrain/bar_swing_back_motion_joint_2_position.inc"
};
static NJS_MKEY_A bar_swing_back_motion_joint_4_angles[32] = {
#include "stg14/terrain/bar_swing_back_motion_joint_4_angles.inc"
};
static NJS_MKEY_A bar_swing_back_motion_joint_7_angles[32] = {
#include "stg14/terrain/bar_swing_back_motion_joint_7_angles.inc"
};
static NJS_MKEY_A bar_swing_back_motion_joint_8_angles[32] = {
#include "stg14/terrain/bar_swing_back_motion_joint_8_angles.inc"
};
static NJS_MKEY_A bar_swing_back_motion_joint_9_angles[32] = {
#include "stg14/terrain/bar_swing_back_motion_joint_9_angles.inc"
};
static NJS_MKEY_A bar_swing_back_motion_joint_13_angles[32] = {
#include "stg14/terrain/bar_swing_back_motion_joint_13_angles.inc"
};
static NJS_MKEY_A bar_swing_back_motion_joint_14_angles[25] = {
#include "stg14/terrain/bar_swing_back_motion_joint_14_angles.inc"
};
static NJS_MKEY_A bar_swing_back_motion_joint_17_angles[32] = {
#include "stg14/terrain/bar_swing_back_motion_joint_17_angles.inc"
};
static NJS_MKEY_A bar_swing_back_motion_joint_18_angles[32] = {
#include "stg14/terrain/bar_swing_back_motion_joint_18_angles.inc"
};
static NJS_MKEY_A bar_swing_back_motion_joint_19_angles[32] = {
#include "stg14/terrain/bar_swing_back_motion_joint_19_angles.inc"
};
static NJS_MKEY_A bar_swing_back_motion_joint_23_angles[32] = {
#include "stg14/terrain/bar_swing_back_motion_joint_23_angles.inc"
};
static NJS_MKEY_A bar_swing_back_motion_joint_24_angles[32] = {
#include "stg14/terrain/bar_swing_back_motion_joint_24_angles.inc"
};
static NJS_MKEY_A bar_swing_back_motion_joint_27_angles[32] = {
#include "stg14/terrain/bar_swing_back_motion_joint_27_angles.inc"
};
static NJS_MKEY_A bar_swing_back_motion_joint_28_angles[28] = {
#include "stg14/terrain/bar_swing_back_motion_joint_28_angles.inc"
};
static NJS_MKEY_A bar_swing_back_motion_joint_29_angles[29] = {
#include "stg14/terrain/bar_swing_back_motion_joint_29_angles.inc"
};
static NJS_MKEY_A bar_swing_back_motion_joint_31_angles[2] = {
#include "stg14/terrain/bar_swing_back_motion_joint_31_angles.inc"
};
static NJS_MKEY_A bar_swing_back_motion_joint_32_angles[2] = {
#include "stg14/terrain/bar_swing_back_motion_joint_32_angles.inc"
};
static NJS_MKEY_A bar_swing_back_motion_joint_33_angles[2] = {
#include "stg14/terrain/bar_swing_back_motion_joint_33_angles.inc"
};
static NJS_MKEY_A bar_swing_back_motion_joint_37_angles[2] = {
#include "stg14/terrain/bar_swing_back_motion_joint_37_angles.inc"
};
static NJS_MKEY_A bar_swing_back_motion_joint_38_angles[2] = {
#include "stg14/terrain/bar_swing_back_motion_joint_38_angles.inc"
};
static NJS_MKEY_A bar_swing_back_motion_joint_39_angles[2] = {
#include "stg14/terrain/bar_swing_back_motion_joint_39_angles.inc"
};
static NJS_MKEY_A bar_swing_back_motion_joint_41_angles[2] = {
#include "stg14/terrain/bar_swing_back_motion_joint_41_angles.inc"
};
static NJS_MKEY_A bar_swing_back_motion_joint_44_angles[32] = {
#include "stg14/terrain/bar_swing_back_motion_joint_44_angles.inc"
};
static NJS_MKEY_A bar_swing_back_motion_joint_51_angles[32] = {
#include "stg14/terrain/bar_swing_back_motion_joint_51_angles.inc"
};
static NJS_MKEY_A bar_swing_back_motion_joint_53_angles[32] = {
#include "stg14/terrain/bar_swing_back_motion_joint_53_angles.inc"
};
static NJS_MKEY_A bar_swing_back_motion_joint_56_angles[2] = {
#include "stg14/terrain/bar_swing_back_motion_joint_56_angles.inc"
};
static NJS_MKEY_A bar_swing_back_motion_joint_57_angles[2] = {
#include "stg14/terrain/bar_swing_back_motion_joint_57_angles.inc"
};
static NJS_MKEY_A bar_swing_back_motion_joint_58_angles[2] = {
#include "stg14/terrain/bar_swing_back_motion_joint_58_angles.inc"
};
static NJS_MKEY_A bar_swing_back_motion_joint_60_angles[2] = {
#include "stg14/terrain/bar_swing_back_motion_joint_60_angles.inc"
};
static NJS_MDATA2 bar_swing_back_motion_tracks[62] = {
#include "stg14/terrain/bar_swing_back_motion_tracks.inc"
};
static NJS_MOTION bar_swing_back_motion = {
#include "stg14/terrain/bar_swing_back_motion.inc"
};
static NJS_MKEY_F bar_swing_motion_joint_0_position[2] = {
#include "stg14/terrain/bar_swing_motion_joint_0_position.inc"
};
static NJS_MKEY_F bar_swing_motion_joint_2_position[2] = {
#include "stg14/terrain/bar_swing_motion_joint_2_position.inc"
};
static NJS_MKEY_A bar_swing_motion_joint_4_angles[32] = {
#include "stg14/terrain/bar_swing_motion_joint_4_angles.inc"
};
static NJS_MKEY_A bar_swing_motion_joint_7_angles[32] = {
#include "stg14/terrain/bar_swing_motion_joint_7_angles.inc"
};
static NJS_MKEY_A bar_swing_motion_joint_8_angles[32] = {
#include "stg14/terrain/bar_swing_motion_joint_8_angles.inc"
};
static NJS_MKEY_A bar_swing_motion_joint_9_angles[32] = {
#include "stg14/terrain/bar_swing_motion_joint_9_angles.inc"
};
static NJS_MKEY_A bar_swing_motion_joint_13_angles[32] = {
#include "stg14/terrain/bar_swing_motion_joint_13_angles.inc"
};
static NJS_MKEY_A bar_swing_motion_joint_14_angles[28] = {
#include "stg14/terrain/bar_swing_motion_joint_14_angles.inc"
};
static NJS_MKEY_A bar_swing_motion_joint_17_angles[32] = {
#include "stg14/terrain/bar_swing_motion_joint_17_angles.inc"
};
static NJS_MKEY_A bar_swing_motion_joint_18_angles[32] = {
#include "stg14/terrain/bar_swing_motion_joint_18_angles.inc"
};
static NJS_MKEY_A bar_swing_motion_joint_19_angles[32] = {
#include "stg14/terrain/bar_swing_motion_joint_19_angles.inc"
};
static NJS_MKEY_A bar_swing_motion_joint_23_angles[32] = {
#include "stg14/terrain/bar_swing_motion_joint_23_angles.inc"
};
static NJS_MKEY_A bar_swing_motion_joint_24_angles[28] = {
#include "stg14/terrain/bar_swing_motion_joint_24_angles.inc"
};
static NJS_MKEY_A bar_swing_motion_joint_27_angles[32] = {
#include "stg14/terrain/bar_swing_motion_joint_27_angles.inc"
};
static NJS_MKEY_A bar_swing_motion_joint_28_angles[32] = {
#include "stg14/terrain/bar_swing_motion_joint_28_angles.inc"
};
static NJS_MKEY_A bar_swing_motion_joint_29_angles[32] = {
#include "stg14/terrain/bar_swing_motion_joint_29_angles.inc"
};
static NJS_MKEY_A bar_swing_motion_joint_31_angles[2] = {
#include "stg14/terrain/bar_swing_motion_joint_31_angles.inc"
};
static NJS_MKEY_A bar_swing_motion_joint_32_angles[2] = {
#include "stg14/terrain/bar_swing_motion_joint_32_angles.inc"
};
static NJS_MKEY_A bar_swing_motion_joint_33_angles[2] = {
#include "stg14/terrain/bar_swing_motion_joint_33_angles.inc"
};
static NJS_MKEY_A bar_swing_motion_joint_37_angles[2] = {
#include "stg14/terrain/bar_swing_motion_joint_37_angles.inc"
};
static NJS_MKEY_A bar_swing_motion_joint_38_angles[2] = {
#include "stg14/terrain/bar_swing_motion_joint_38_angles.inc"
};
static NJS_MKEY_A bar_swing_motion_joint_39_angles[2] = {
#include "stg14/terrain/bar_swing_motion_joint_39_angles.inc"
};
static NJS_MKEY_A bar_swing_motion_joint_41_angles[2] = {
#include "stg14/terrain/bar_swing_motion_joint_41_angles.inc"
};
static NJS_MKEY_A bar_swing_motion_joint_44_angles[32] = {
#include "stg14/terrain/bar_swing_motion_joint_44_angles.inc"
};
static NJS_MKEY_A bar_swing_motion_joint_51_angles[32] = {
#include "stg14/terrain/bar_swing_motion_joint_51_angles.inc"
};
static NJS_MKEY_A bar_swing_motion_joint_53_angles[32] = {
#include "stg14/terrain/bar_swing_motion_joint_53_angles.inc"
};
static NJS_MKEY_A bar_swing_motion_joint_56_angles[2] = {
#include "stg14/terrain/bar_swing_motion_joint_56_angles.inc"
};
static NJS_MKEY_A bar_swing_motion_joint_57_angles[2] = {
#include "stg14/terrain/bar_swing_motion_joint_57_angles.inc"
};
static NJS_MKEY_A bar_swing_motion_joint_58_angles[2] = {
#include "stg14/terrain/bar_swing_motion_joint_58_angles.inc"
};
static NJS_MKEY_A bar_swing_motion_joint_60_angles[2] = {
#include "stg14/terrain/bar_swing_motion_joint_60_angles.inc"
};
static NJS_MDATA2 bar_swing_motion_tracks[62] = {
#include "stg14/terrain/bar_swing_motion_tracks.inc"
};
static NJS_MOTION bar_swing_motion = {
#include "stg14/terrain/bar_swing_motion.inc"
};
static NJS_MKEY_F bar_land_motion_joint_2_position[9] = {
#include "stg14/terrain/bar_land_motion_joint_2_position.inc"
};
static NJS_MKEY_A bar_land_motion_joint_2_angles[2] = {
#include "stg14/terrain/bar_land_motion_joint_2_angles.inc"
};
static NJS_MKEY_A bar_land_motion_joint_4_angles[2] = {
#include "stg14/terrain/bar_land_motion_joint_4_angles.inc"
};
static NJS_MKEY_A bar_land_motion_joint_7_angles[9] = {
#include "stg14/terrain/bar_land_motion_joint_7_angles.inc"
};
static NJS_MKEY_A bar_land_motion_joint_8_angles[9] = {
#include "stg14/terrain/bar_land_motion_joint_8_angles.inc"
};
static NJS_MKEY_A bar_land_motion_joint_9_angles[9] = {
#include "stg14/terrain/bar_land_motion_joint_9_angles.inc"
};
static NJS_MKEY_A bar_land_motion_joint_13_angles[9] = {
#include "stg14/terrain/bar_land_motion_joint_13_angles.inc"
};
static NJS_MKEY_A bar_land_motion_joint_14_angles[2] = {
#include "stg14/terrain/bar_land_motion_joint_14_angles.inc"
};
static NJS_MKEY_A bar_land_motion_joint_17_angles[9] = {
#include "stg14/terrain/bar_land_motion_joint_17_angles.inc"
};
static NJS_MKEY_A bar_land_motion_joint_18_angles[9] = {
#include "stg14/terrain/bar_land_motion_joint_18_angles.inc"
};
static NJS_MKEY_A bar_land_motion_joint_19_angles[9] = {
#include "stg14/terrain/bar_land_motion_joint_19_angles.inc"
};
static NJS_MKEY_A bar_land_motion_joint_23_angles[9] = {
#include "stg14/terrain/bar_land_motion_joint_23_angles.inc"
};
static NJS_MKEY_A bar_land_motion_joint_24_angles[4] = {
#include "stg14/terrain/bar_land_motion_joint_24_angles.inc"
};
static NJS_MKEY_A bar_land_motion_joint_27_angles[9] = {
#include "stg14/terrain/bar_land_motion_joint_27_angles.inc"
};
static NJS_MKEY_A bar_land_motion_joint_28_angles[9] = {
#include "stg14/terrain/bar_land_motion_joint_28_angles.inc"
};
static NJS_MKEY_A bar_land_motion_joint_29_angles[9] = {
#include "stg14/terrain/bar_land_motion_joint_29_angles.inc"
};
static NJS_MKEY_A bar_land_motion_joint_32_angles[6] = {
#include "stg14/terrain/bar_land_motion_joint_32_angles.inc"
};
static NJS_MKEY_A bar_land_motion_joint_33_angles[6] = {
#include "stg14/terrain/bar_land_motion_joint_33_angles.inc"
};
static NJS_MKEY_A bar_land_motion_joint_37_angles[5] = {
#include "stg14/terrain/bar_land_motion_joint_37_angles.inc"
};
static NJS_MKEY_A bar_land_motion_joint_38_angles[5] = {
#include "stg14/terrain/bar_land_motion_joint_38_angles.inc"
};
static NJS_MKEY_A bar_land_motion_joint_39_angles[5] = {
#include "stg14/terrain/bar_land_motion_joint_39_angles.inc"
};
static NJS_MKEY_A bar_land_motion_joint_41_angles[9] = {
#include "stg14/terrain/bar_land_motion_joint_41_angles.inc"
};
static NJS_MKEY_A bar_land_motion_joint_44_angles[9] = {
#include "stg14/terrain/bar_land_motion_joint_44_angles.inc"
};
static NJS_MKEY_A bar_land_motion_joint_51_angles[9] = {
#include "stg14/terrain/bar_land_motion_joint_51_angles.inc"
};
static NJS_MKEY_A bar_land_motion_joint_53_angles[9] = {
#include "stg14/terrain/bar_land_motion_joint_53_angles.inc"
};
static NJS_MKEY_A bar_land_motion_joint_56_angles[8] = {
#include "stg14/terrain/bar_land_motion_joint_56_angles.inc"
};
static NJS_MKEY_A bar_land_motion_joint_57_angles[5] = {
#include "stg14/terrain/bar_land_motion_joint_57_angles.inc"
};
static NJS_MKEY_A bar_land_motion_joint_58_angles[5] = {
#include "stg14/terrain/bar_land_motion_joint_58_angles.inc"
};
static NJS_MKEY_A bar_land_motion_joint_60_angles[9] = {
#include "stg14/terrain/bar_land_motion_joint_60_angles.inc"
};
static NJS_MDATA2 bar_land_motion_tracks[62] = {
#include "stg14/terrain/bar_land_motion_tracks.inc"
};
static NJS_MOTION bar_land_motion = {
#include "stg14/terrain/bar_land_motion.inc"
};
struct motion_entry module_motions[9] = {
#include "stg14/terrain/module_motions.inc"
};
static struct land_entry entries[1128] = {
#include "stg14/terrain/entries.inc"
};
struct land_table radical_highway_land = {
#include "stg14/terrain/radical_highway_land.inc"
};
static NJS_TEXNAME textures[64] = {
#include "stg14/terrain/textures.inc"
};
NJS_TEXLIST radical_highway_terrain_textures = {
#include "stg14/terrain/radical_highway_terrain_textures.inc"
};
static NJS_MATERIAL collision_0_materials[1] = {
#include "stg14/terrain/collision_0_materials.inc"
};
static Sint16 collision_0_faces[9] = {
#include "stg14/terrain/collision_0_faces.inc"
};
static NJS_MESHSET collision_0_meshsets[1] = {
#include "stg14/terrain/collision_0_meshsets.inc"
};
static NJS_POINT3 collision_0_points[8] = {
#include "stg14/terrain/collision_0_points.inc"
};
static NJS_MODEL collision_0_model = {
#include "stg14/terrain/collision_0_model.inc"
};
static NJS_OBJECT collision_0 = {
#include "stg14/terrain/collision_0.inc"
};
static NJS_MATERIAL collision_6_materials[1] = {
#include "stg14/terrain/collision_6_materials.inc"
};
static Sint16 collision_6_faces[21] = {
#include "stg14/terrain/collision_6_faces.inc"
};
static NJS_MESHSET collision_6_meshsets[1] = {
#include "stg14/terrain/collision_6_meshsets.inc"
};
static NJS_POINT3 collision_6_points[17] = {
#include "stg14/terrain/collision_6_points.inc"
};
static NJS_MODEL collision_6_model = {
#include "stg14/terrain/collision_6_model.inc"
};
static NJS_OBJECT collision_6 = {
#include "stg14/terrain/collision_6.inc"
};
static NJS_MATERIAL collision_8_materials[1] = {
#include "stg14/terrain/collision_8_materials.inc"
};
static Sint16 collision_8_faces[21] = {
#include "stg14/terrain/collision_8_faces.inc"
};
static NJS_MESHSET collision_8_meshsets[1] = {
#include "stg14/terrain/collision_8_meshsets.inc"
};
static NJS_POINT3 collision_8_points[17] = {
#include "stg14/terrain/collision_8_points.inc"
};
static NJS_MODEL collision_8_model = {
#include "stg14/terrain/collision_8_model.inc"
};
static NJS_OBJECT collision_8 = {
#include "stg14/terrain/collision_8.inc"
};
static NJS_MATERIAL collision_10_materials[1] = {
#include "stg14/terrain/collision_10_materials.inc"
};
static Sint16 collision_10_faces[102] = {
#include "stg14/terrain/collision_10_faces.inc"
};
static NJS_MESHSET collision_10_meshsets[1] = {
#include "stg14/terrain/collision_10_meshsets.inc"
};
static NJS_POINT3 collision_10_points[48] = {
#include "stg14/terrain/collision_10_points.inc"
};
static NJS_MODEL collision_10_model = {
#include "stg14/terrain/collision_10_model.inc"
};
static NJS_OBJECT collision_10 = {
#include "stg14/terrain/collision_10.inc"
};
static NJS_MATERIAL collision_11_materials[1] = {
#include "stg14/terrain/collision_11_materials.inc"
};
static Sint16 collision_11_faces[42] = {
#include "stg14/terrain/collision_11_faces.inc"
};
static NJS_MESHSET collision_11_meshsets[1] = {
#include "stg14/terrain/collision_11_meshsets.inc"
};
static NJS_POINT3 collision_11_points[24] = {
#include "stg14/terrain/collision_11_points.inc"
};
static NJS_MODEL collision_11_model = {
#include "stg14/terrain/collision_11_model.inc"
};
static NJS_OBJECT collision_11 = {
#include "stg14/terrain/collision_11.inc"
};
static NJS_MATERIAL collision_15_materials[1] = {
#include "stg14/terrain/collision_15_materials.inc"
};
static Sint16 collision_15_faces[30] = {
#include "stg14/terrain/collision_15_faces.inc"
};
static NJS_MESHSET collision_15_meshsets[1] = {
#include "stg14/terrain/collision_15_meshsets.inc"
};
static NJS_POINT3 collision_15_points[21] = {
#include "stg14/terrain/collision_15_points.inc"
};
static NJS_MODEL collision_15_model = {
#include "stg14/terrain/collision_15_model.inc"
};
static NJS_OBJECT collision_15 = {
#include "stg14/terrain/collision_15.inc"
};
static NJS_MATERIAL collision_17_materials[1] = {
#include "stg14/terrain/collision_17_materials.inc"
};
static Sint16 collision_17_faces[14] = {
#include "stg14/terrain/collision_17_faces.inc"
};
static NJS_MESHSET collision_17_meshsets[1] = {
#include "stg14/terrain/collision_17_meshsets.inc"
};
static NJS_POINT3 collision_17_points[9] = {
#include "stg14/terrain/collision_17_points.inc"
};
static NJS_MODEL collision_17_model = {
#include "stg14/terrain/collision_17_model.inc"
};
static NJS_OBJECT collision_17 = {
#include "stg14/terrain/collision_17.inc"
};
static NJS_MATERIAL collision_18_materials[1] = {
#include "stg14/terrain/collision_18_materials.inc"
};
static Sint16 collision_18_faces[62] = {
#include "stg14/terrain/collision_18_faces.inc"
};
static NJS_MESHSET collision_18_meshsets[1] = {
#include "stg14/terrain/collision_18_meshsets.inc"
};
static NJS_POINT3 collision_18_points[32] = {
#include "stg14/terrain/collision_18_points.inc"
};
static NJS_MODEL collision_18_model = {
#include "stg14/terrain/collision_18_model.inc"
};
static NJS_OBJECT collision_18 = {
#include "stg14/terrain/collision_18.inc"
};
static NJS_MATERIAL collision_19_materials[1] = {
#include "stg14/terrain/collision_19_materials.inc"
};
static Sint16 collision_19_faces[31] = {
#include "stg14/terrain/collision_19_faces.inc"
};
static NJS_MESHSET collision_19_meshsets[1] = {
#include "stg14/terrain/collision_19_meshsets.inc"
};
static NJS_POINT3 collision_19_points[16] = {
#include "stg14/terrain/collision_19_points.inc"
};
static NJS_MODEL collision_19_model = {
#include "stg14/terrain/collision_19_model.inc"
};
static NJS_OBJECT collision_19 = {
#include "stg14/terrain/collision_19.inc"
};
static NJS_MATERIAL collision_20_materials[1] = {
#include "stg14/terrain/collision_20_materials.inc"
};
static Sint16 collision_20_faces[28] = {
#include "stg14/terrain/collision_20_faces.inc"
};
static NJS_COLOR collision_20_basic_uvs[24] = {
#include "stg14/terrain/collision_20_basic_uvs.inc"
};
static NJS_MESHSET collision_20_meshsets[1] = {
#include "stg14/terrain/collision_20_meshsets.inc"
};
static NJS_POINT3 collision_20_points[16] = {
#include "stg14/terrain/collision_20_points.inc"
};
static NJS_MODEL collision_20_model = {
#include "stg14/terrain/collision_20_model.inc"
};
static NJS_OBJECT collision_20 = {
#include "stg14/terrain/collision_20.inc"
};
static NJS_MATERIAL collision_21_materials[1] = {
#include "stg14/terrain/collision_21_materials.inc"
};
static Sint16 collision_21_faces[18] = {
#include "stg14/terrain/collision_21_faces.inc"
};
static NJS_MESHSET collision_21_meshsets[1] = {
#include "stg14/terrain/collision_21_meshsets.inc"
};
static NJS_POINT3 collision_21_points[12] = {
#include "stg14/terrain/collision_21_points.inc"
};
static NJS_MODEL collision_21_model = {
#include "stg14/terrain/collision_21_model.inc"
};
static NJS_OBJECT collision_21 = {
#include "stg14/terrain/collision_21.inc"
};
static NJS_MATERIAL collision_22_materials[1] = {
#include "stg14/terrain/collision_22_materials.inc"
};
static Sint16 collision_22_faces[88] = {
#include "stg14/terrain/collision_22_faces.inc"
};
static NJS_COLOR collision_22_basic_uvs[80] = {
#include "stg14/terrain/collision_22_basic_uvs.inc"
};
static NJS_MESHSET collision_22_meshsets[1] = {
#include "stg14/terrain/collision_22_meshsets.inc"
};
static NJS_POINT3 collision_22_points[45] = {
#include "stg14/terrain/collision_22_points.inc"
};
static NJS_MODEL collision_22_model = {
#include "stg14/terrain/collision_22_model.inc"
};
static NJS_OBJECT collision_22 = {
#include "stg14/terrain/collision_22.inc"
};
static NJS_MATERIAL collision_23_materials[1] = {
#include "stg14/terrain/collision_23_materials.inc"
};
static Sint16 collision_23_faces[105] = {
#include "stg14/terrain/collision_23_faces.inc"
};
static NJS_MESHSET collision_23_meshsets[1] = {
#include "stg14/terrain/collision_23_meshsets.inc"
};
static NJS_POINT3 collision_23_points[56] = {
#include "stg14/terrain/collision_23_points.inc"
};
static NJS_MODEL collision_23_model = {
#include "stg14/terrain/collision_23_model.inc"
};
static NJS_OBJECT collision_23 = {
#include "stg14/terrain/collision_23.inc"
};
static NJS_MATERIAL collision_25_materials[1] = {
#include "stg14/terrain/collision_25_materials.inc"
};
static Sint16 collision_25_faces[14] = {
#include "stg14/terrain/collision_25_faces.inc"
};
static NJS_MESHSET collision_25_meshsets[1] = {
#include "stg14/terrain/collision_25_meshsets.inc"
};
static NJS_POINT3 collision_25_points[12] = {
#include "stg14/terrain/collision_25_points.inc"
};
static NJS_MODEL collision_25_model = {
#include "stg14/terrain/collision_25_model.inc"
};
static NJS_OBJECT collision_25 = {
#include "stg14/terrain/collision_25.inc"
};
static NJS_MATERIAL collision_26_materials[1] = {
#include "stg14/terrain/collision_26_materials.inc"
};
static Sint16 collision_26_faces[18] = {
#include "stg14/terrain/collision_26_faces.inc"
};
static NJS_MESHSET collision_26_meshsets[1] = {
#include "stg14/terrain/collision_26_meshsets.inc"
};
static NJS_POINT3 collision_26_points[16] = {
#include "stg14/terrain/collision_26_points.inc"
};
static NJS_MODEL collision_26_model = {
#include "stg14/terrain/collision_26_model.inc"
};
static NJS_OBJECT collision_26 = {
#include "stg14/terrain/collision_26.inc"
};
static NJS_MATERIAL collision_27_materials[1] = {
#include "stg14/terrain/collision_27_materials.inc"
};
static Sint16 collision_27_faces[14] = {
#include "stg14/terrain/collision_27_faces.inc"
};
static NJS_MESHSET collision_27_meshsets[1] = {
#include "stg14/terrain/collision_27_meshsets.inc"
};
static NJS_POINT3 collision_27_points[12] = {
#include "stg14/terrain/collision_27_points.inc"
};
static NJS_MODEL collision_27_model = {
#include "stg14/terrain/collision_27_model.inc"
};
static NJS_OBJECT collision_27 = {
#include "stg14/terrain/collision_27.inc"
};
static NJS_MATERIAL collision_28_materials[1] = {
#include "stg14/terrain/collision_28_materials.inc"
};
static Sint16 collision_28_faces[9] = {
#include "stg14/terrain/collision_28_faces.inc"
};
static NJS_MESHSET collision_28_meshsets[1] = {
#include "stg14/terrain/collision_28_meshsets.inc"
};
static NJS_POINT3 collision_28_points[8] = {
#include "stg14/terrain/collision_28_points.inc"
};
static NJS_MODEL collision_28_model = {
#include "stg14/terrain/collision_28_model.inc"
};
static NJS_OBJECT collision_28 = {
#include "stg14/terrain/collision_28.inc"
};
static NJS_MATERIAL collision_29_materials[1] = {
#include "stg14/terrain/collision_29_materials.inc"
};
static Sint16 collision_29_faces[5] = {
#include "stg14/terrain/collision_29_faces.inc"
};
static NJS_MESHSET collision_29_meshsets[1] = {
#include "stg14/terrain/collision_29_meshsets.inc"
};
static NJS_POINT3 collision_29_points[4] = {
#include "stg14/terrain/collision_29_points.inc"
};
static NJS_MODEL collision_29_model = {
#include "stg14/terrain/collision_29_model.inc"
};
static NJS_OBJECT collision_29 = {
#include "stg14/terrain/collision_29.inc"
};
static NJS_MATERIAL collision_30_materials[1] = {
#include "stg14/terrain/collision_30_materials.inc"
};
static Sint16 collision_30_faces[21] = {
#include "stg14/terrain/collision_30_faces.inc"
};
static NJS_MESHSET collision_30_meshsets[1] = {
#include "stg14/terrain/collision_30_meshsets.inc"
};
static NJS_POINT3 collision_30_points[11] = {
#include "stg14/terrain/collision_30_points.inc"
};
static NJS_MODEL collision_30_model = {
#include "stg14/terrain/collision_30_model.inc"
};
static NJS_OBJECT collision_30 = {
#include "stg14/terrain/collision_30.inc"
};
static NJS_MATERIAL collision_31_materials[1] = {
#include "stg14/terrain/collision_31_materials.inc"
};
static Sint16 collision_31_faces[14] = {
#include "stg14/terrain/collision_31_faces.inc"
};
static NJS_MESHSET collision_31_meshsets[1] = {
#include "stg14/terrain/collision_31_meshsets.inc"
};
static NJS_POINT3 collision_31_points[9] = {
#include "stg14/terrain/collision_31_points.inc"
};
static NJS_MODEL collision_31_model = {
#include "stg14/terrain/collision_31_model.inc"
};
static NJS_OBJECT collision_31 = {
#include "stg14/terrain/collision_31.inc"
};
static NJS_MATERIAL collision_33_materials[1] = {
#include "stg14/terrain/collision_33_materials.inc"
};
static Sint16 collision_33_faces[10] = {
#include "stg14/terrain/collision_33_faces.inc"
};
static NJS_MESHSET collision_33_meshsets[1] = {
#include "stg14/terrain/collision_33_meshsets.inc"
};
static NJS_POINT3 collision_33_points[8] = {
#include "stg14/terrain/collision_33_points.inc"
};
static NJS_MODEL collision_33_model = {
#include "stg14/terrain/collision_33_model.inc"
};
static NJS_OBJECT collision_33 = {
#include "stg14/terrain/collision_33.inc"
};
static NJS_MATERIAL collision_43_materials[1] = {
#include "stg14/terrain/collision_43_materials.inc"
};
static Sint16 collision_43_faces[7] = {
#include "stg14/terrain/collision_43_faces.inc"
};
static NJS_MESHSET collision_43_meshsets[1] = {
#include "stg14/terrain/collision_43_meshsets.inc"
};
static NJS_POINT3 collision_43_points[6] = {
#include "stg14/terrain/collision_43_points.inc"
};
static NJS_MODEL collision_43_model = {
#include "stg14/terrain/collision_43_model.inc"
};
static NJS_OBJECT collision_43 = {
#include "stg14/terrain/collision_43.inc"
};
static NJS_MATERIAL collision_44_materials[1] = {
#include "stg14/terrain/collision_44_materials.inc"
};
static Sint16 collision_44_faces[63] = {
#include "stg14/terrain/collision_44_faces.inc"
};
static NJS_MESHSET collision_44_meshsets[1] = {
#include "stg14/terrain/collision_44_meshsets.inc"
};
static NJS_POINT3 collision_44_points[40] = {
#include "stg14/terrain/collision_44_points.inc"
};
static NJS_MODEL collision_44_model = {
#include "stg14/terrain/collision_44_model.inc"
};
static NJS_OBJECT collision_44 = {
#include "stg14/terrain/collision_44.inc"
};
static NJS_MATERIAL collision_45_materials[1] = {
#include "stg14/terrain/collision_45_materials.inc"
};
static Sint16 collision_45_faces[10] = {
#include "stg14/terrain/collision_45_faces.inc"
};
static NJS_MESHSET collision_45_meshsets[1] = {
#include "stg14/terrain/collision_45_meshsets.inc"
};
static NJS_POINT3 collision_45_points[8] = {
#include "stg14/terrain/collision_45_points.inc"
};
static NJS_MODEL collision_45_model = {
#include "stg14/terrain/collision_45_model.inc"
};
static NJS_OBJECT collision_45 = {
#include "stg14/terrain/collision_45.inc"
};
static NJS_MATERIAL collision_52_materials[1] = {
#include "stg14/terrain/collision_52_materials.inc"
};
static Sint16 collision_52_faces[38] = {
#include "stg14/terrain/collision_52_faces.inc"
};
static NJS_MESHSET collision_52_meshsets[1] = {
#include "stg14/terrain/collision_52_meshsets.inc"
};
static NJS_POINT3 collision_52_points[36] = {
#include "stg14/terrain/collision_52_points.inc"
};
static NJS_MODEL collision_52_model = {
#include "stg14/terrain/collision_52_model.inc"
};
static NJS_OBJECT collision_52 = {
#include "stg14/terrain/collision_52.inc"
};
static NJS_MATERIAL collision_57_materials[1] = {
#include "stg14/terrain/collision_57_materials.inc"
};
static Sint16 collision_57_faces[14] = {
#include "stg14/terrain/collision_57_faces.inc"
};
static NJS_MESHSET collision_57_meshsets[1] = {
#include "stg14/terrain/collision_57_meshsets.inc"
};
static NJS_POINT3 collision_57_points[12] = {
#include "stg14/terrain/collision_57_points.inc"
};
static NJS_MODEL collision_57_model = {
#include "stg14/terrain/collision_57_model.inc"
};
static NJS_OBJECT collision_57 = {
#include "stg14/terrain/collision_57.inc"
};
static NJS_MATERIAL collision_63_materials[1] = {
#include "stg14/terrain/collision_63_materials.inc"
};
static Sint16 collision_63_faces[14] = {
#include "stg14/terrain/collision_63_faces.inc"
};
static NJS_MESHSET collision_63_meshsets[1] = {
#include "stg14/terrain/collision_63_meshsets.inc"
};
static NJS_POINT3 collision_63_points[12] = {
#include "stg14/terrain/collision_63_points.inc"
};
static NJS_MODEL collision_63_model = {
#include "stg14/terrain/collision_63_model.inc"
};
static NJS_OBJECT collision_63 = {
#include "stg14/terrain/collision_63.inc"
};
static NJS_MATERIAL collision_68_materials[1] = {
#include "stg14/terrain/collision_68_materials.inc"
};
static Sint16 collision_68_faces[14] = {
#include "stg14/terrain/collision_68_faces.inc"
};
static NJS_MESHSET collision_68_meshsets[1] = {
#include "stg14/terrain/collision_68_meshsets.inc"
};
static NJS_POINT3 collision_68_points[12] = {
#include "stg14/terrain/collision_68_points.inc"
};
static NJS_MODEL collision_68_model = {
#include "stg14/terrain/collision_68_model.inc"
};
static NJS_OBJECT collision_68 = {
#include "stg14/terrain/collision_68.inc"
};
static NJS_MATERIAL collision_69_materials[1] = {
#include "stg14/terrain/collision_69_materials.inc"
};
static Sint16 collision_69_faces[120] = {
#include "stg14/terrain/collision_69_faces.inc"
};
static NJS_MESHSET collision_69_meshsets[1] = {
#include "stg14/terrain/collision_69_meshsets.inc"
};
static NJS_POINT3 collision_69_points[81] = {
#include "stg14/terrain/collision_69_points.inc"
};
static NJS_MODEL collision_69_model = {
#include "stg14/terrain/collision_69_model.inc"
};
static NJS_OBJECT collision_69 = {
#include "stg14/terrain/collision_69.inc"
};
static NJS_MATERIAL collision_70_materials[1] = {
#include "stg14/terrain/collision_70_materials.inc"
};
static Sint16 collision_70_faces[31] = {
#include "stg14/terrain/collision_70_faces.inc"
};
static NJS_MESHSET collision_70_meshsets[1] = {
#include "stg14/terrain/collision_70_meshsets.inc"
};
static NJS_POINT3 collision_70_points[30] = {
#include "stg14/terrain/collision_70_points.inc"
};
static NJS_MODEL collision_70_model = {
#include "stg14/terrain/collision_70_model.inc"
};
static NJS_OBJECT collision_70 = {
#include "stg14/terrain/collision_70.inc"
};
static NJS_MATERIAL collision_71_materials[1] = {
#include "stg14/terrain/collision_71_materials.inc"
};
static Sint16 collision_71_faces[123] = {
#include "stg14/terrain/collision_71_faces.inc"
};
static NJS_MESHSET collision_71_meshsets[1] = {
#include "stg14/terrain/collision_71_meshsets.inc"
};
static NJS_POINT3 collision_71_points[81] = {
#include "stg14/terrain/collision_71_points.inc"
};
static NJS_MODEL collision_71_model = {
#include "stg14/terrain/collision_71_model.inc"
};
static NJS_OBJECT collision_71 = {
#include "stg14/terrain/collision_71.inc"
};
static NJS_MATERIAL collision_72_materials[1] = {
#include "stg14/terrain/collision_72_materials.inc"
};
static Sint16 collision_72_faces[20] = {
#include "stg14/terrain/collision_72_faces.inc"
};
static NJS_MESHSET collision_72_meshsets[1] = {
#include "stg14/terrain/collision_72_meshsets.inc"
};
static NJS_POINT3 collision_72_points[12] = {
#include "stg14/terrain/collision_72_points.inc"
};
static NJS_MODEL collision_72_model = {
#include "stg14/terrain/collision_72_model.inc"
};
static NJS_OBJECT collision_72 = {
#include "stg14/terrain/collision_72.inc"
};
static NJS_MATERIAL collision_74_materials[1] = {
#include "stg14/terrain/collision_74_materials.inc"
};
static Sint16 collision_74_faces[19] = {
#include "stg14/terrain/collision_74_faces.inc"
};
static NJS_MESHSET collision_74_meshsets[1] = {
#include "stg14/terrain/collision_74_meshsets.inc"
};
static NJS_POINT3 collision_74_points[16] = {
#include "stg14/terrain/collision_74_points.inc"
};
static NJS_MODEL collision_74_model = {
#include "stg14/terrain/collision_74_model.inc"
};
static NJS_OBJECT collision_74 = {
#include "stg14/terrain/collision_74.inc"
};
static NJS_MATERIAL collision_76_materials[1] = {
#include "stg14/terrain/collision_76_materials.inc"
};
static Sint16 collision_76_faces[60] = {
#include "stg14/terrain/collision_76_faces.inc"
};
static NJS_MESHSET collision_76_meshsets[1] = {
#include "stg14/terrain/collision_76_meshsets.inc"
};
static NJS_POINT3 collision_76_points[40] = {
#include "stg14/terrain/collision_76_points.inc"
};
static NJS_MODEL collision_76_model = {
#include "stg14/terrain/collision_76_model.inc"
};
static NJS_OBJECT collision_76 = {
#include "stg14/terrain/collision_76.inc"
};
static NJS_MATERIAL collision_77_materials[1] = {
#include "stg14/terrain/collision_77_materials.inc"
};
static Sint16 collision_77_faces[14] = {
#include "stg14/terrain/collision_77_faces.inc"
};
static NJS_MESHSET collision_77_meshsets[1] = {
#include "stg14/terrain/collision_77_meshsets.inc"
};
static NJS_POINT3 collision_77_points[12] = {
#include "stg14/terrain/collision_77_points.inc"
};
static NJS_MODEL collision_77_model = {
#include "stg14/terrain/collision_77_model.inc"
};
static NJS_OBJECT collision_77 = {
#include "stg14/terrain/collision_77.inc"
};
static NJS_MATERIAL collision_78_materials[1] = {
#include "stg14/terrain/collision_78_materials.inc"
};
static Sint16 collision_78_faces[10] = {
#include "stg14/terrain/collision_78_faces.inc"
};
static NJS_MESHSET collision_78_meshsets[1] = {
#include "stg14/terrain/collision_78_meshsets.inc"
};
static NJS_POINT3 collision_78_points[8] = {
#include "stg14/terrain/collision_78_points.inc"
};
static NJS_MODEL collision_78_model = {
#include "stg14/terrain/collision_78_model.inc"
};
static NJS_OBJECT collision_78 = {
#include "stg14/terrain/collision_78.inc"
};
static NJS_MATERIAL collision_79_materials[1] = {
#include "stg14/terrain/collision_79_materials.inc"
};
static Sint16 collision_79_faces[14] = {
#include "stg14/terrain/collision_79_faces.inc"
};
static NJS_MESHSET collision_79_meshsets[1] = {
#include "stg14/terrain/collision_79_meshsets.inc"
};
static NJS_POINT3 collision_79_points[12] = {
#include "stg14/terrain/collision_79_points.inc"
};
static NJS_MODEL collision_79_model = {
#include "stg14/terrain/collision_79_model.inc"
};
static NJS_OBJECT collision_79 = {
#include "stg14/terrain/collision_79.inc"
};
static NJS_MATERIAL collision_80_materials[1] = {
#include "stg14/terrain/collision_80_materials.inc"
};
static Sint16 collision_80_faces[14] = {
#include "stg14/terrain/collision_80_faces.inc"
};
static NJS_MESHSET collision_80_meshsets[1] = {
#include "stg14/terrain/collision_80_meshsets.inc"
};
static NJS_POINT3 collision_80_points[12] = {
#include "stg14/terrain/collision_80_points.inc"
};
static NJS_MODEL collision_80_model = {
#include "stg14/terrain/collision_80_model.inc"
};
static NJS_OBJECT collision_80 = {
#include "stg14/terrain/collision_80.inc"
};
static NJS_MATERIAL collision_81_materials[1] = {
#include "stg14/terrain/collision_81_materials.inc"
};
static Sint16 collision_81_faces[14] = {
#include "stg14/terrain/collision_81_faces.inc"
};
static NJS_MESHSET collision_81_meshsets[1] = {
#include "stg14/terrain/collision_81_meshsets.inc"
};
static NJS_POINT3 collision_81_points[12] = {
#include "stg14/terrain/collision_81_points.inc"
};
static NJS_MODEL collision_81_model = {
#include "stg14/terrain/collision_81_model.inc"
};
static NJS_OBJECT collision_81 = {
#include "stg14/terrain/collision_81.inc"
};
static NJS_MATERIAL collision_82_materials[1] = {
#include "stg14/terrain/collision_82_materials.inc"
};
static Sint16 collision_82_faces[14] = {
#include "stg14/terrain/collision_82_faces.inc"
};
static NJS_MESHSET collision_82_meshsets[1] = {
#include "stg14/terrain/collision_82_meshsets.inc"
};
static NJS_POINT3 collision_82_points[12] = {
#include "stg14/terrain/collision_82_points.inc"
};
static NJS_MODEL collision_82_model = {
#include "stg14/terrain/collision_82_model.inc"
};
static NJS_OBJECT collision_82 = {
#include "stg14/terrain/collision_82.inc"
};
static NJS_MATERIAL collision_83_materials[1] = {
#include "stg14/terrain/collision_83_materials.inc"
};
static Sint16 collision_83_faces[14] = {
#include "stg14/terrain/collision_83_faces.inc"
};
static NJS_MESHSET collision_83_meshsets[1] = {
#include "stg14/terrain/collision_83_meshsets.inc"
};
static NJS_POINT3 collision_83_points[12] = {
#include "stg14/terrain/collision_83_points.inc"
};
static NJS_MODEL collision_83_model = {
#include "stg14/terrain/collision_83_model.inc"
};
static NJS_OBJECT collision_83 = {
#include "stg14/terrain/collision_83.inc"
};
static NJS_MATERIAL collision_85_materials[1] = {
#include "stg14/terrain/collision_85_materials.inc"
};
static Sint16 collision_85_faces[27] = {
#include "stg14/terrain/collision_85_faces.inc"
};
static NJS_MESHSET collision_85_meshsets[1] = {
#include "stg14/terrain/collision_85_meshsets.inc"
};
static NJS_POINT3 collision_85_points[20] = {
#include "stg14/terrain/collision_85_points.inc"
};
static NJS_MODEL collision_85_model = {
#include "stg14/terrain/collision_85_model.inc"
};
static NJS_OBJECT collision_85 = {
#include "stg14/terrain/collision_85.inc"
};
static NJS_MATERIAL collision_86_materials[1] = {
#include "stg14/terrain/collision_86_materials.inc"
};
static Sint16 collision_86_faces[22] = {
#include "stg14/terrain/collision_86_faces.inc"
};
static NJS_MESHSET collision_86_meshsets[1] = {
#include "stg14/terrain/collision_86_meshsets.inc"
};
static NJS_POINT3 collision_86_points[20] = {
#include "stg14/terrain/collision_86_points.inc"
};
static NJS_MODEL collision_86_model = {
#include "stg14/terrain/collision_86_model.inc"
};
static NJS_OBJECT collision_86 = {
#include "stg14/terrain/collision_86.inc"
};
static NJS_MATERIAL collision_87_materials[1] = {
#include "stg14/terrain/collision_87_materials.inc"
};
static Sint16 collision_87_faces[18] = {
#include "stg14/terrain/collision_87_faces.inc"
};
static NJS_MESHSET collision_87_meshsets[1] = {
#include "stg14/terrain/collision_87_meshsets.inc"
};
static NJS_POINT3 collision_87_points[16] = {
#include "stg14/terrain/collision_87_points.inc"
};
static NJS_MODEL collision_87_model = {
#include "stg14/terrain/collision_87_model.inc"
};
static NJS_OBJECT collision_87 = {
#include "stg14/terrain/collision_87.inc"
};
static NJS_MATERIAL collision_90_materials[1] = {
#include "stg14/terrain/collision_90_materials.inc"
};
static Sint16 collision_90_faces[11] = {
#include "stg14/terrain/collision_90_faces.inc"
};
static NJS_MESHSET collision_90_meshsets[1] = {
#include "stg14/terrain/collision_90_meshsets.inc"
};
static NJS_POINT3 collision_90_points[10] = {
#include "stg14/terrain/collision_90_points.inc"
};
static NJS_MODEL collision_90_model = {
#include "stg14/terrain/collision_90_model.inc"
};
static NJS_OBJECT collision_90 = {
#include "stg14/terrain/collision_90.inc"
};
static NJS_MATERIAL collision_92_materials[1] = {
#include "stg14/terrain/collision_92_materials.inc"
};
static Sint16 collision_92_faces[18] = {
#include "stg14/terrain/collision_92_faces.inc"
};
static NJS_MESHSET collision_92_meshsets[1] = {
#include "stg14/terrain/collision_92_meshsets.inc"
};
static NJS_POINT3 collision_92_points[16] = {
#include "stg14/terrain/collision_92_points.inc"
};
static NJS_MODEL collision_92_model = {
#include "stg14/terrain/collision_92_model.inc"
};
static NJS_OBJECT collision_92 = {
#include "stg14/terrain/collision_92.inc"
};
static NJS_MATERIAL collision_95_materials[1] = {
#include "stg14/terrain/collision_95_materials.inc"
};
static Sint16 collision_95_faces[104] = {
#include "stg14/terrain/collision_95_faces.inc"
};
static NJS_MESHSET collision_95_meshsets[1] = {
#include "stg14/terrain/collision_95_meshsets.inc"
};
static NJS_POINT3 collision_95_points[51] = {
#include "stg14/terrain/collision_95_points.inc"
};
static NJS_MODEL collision_95_model = {
#include "stg14/terrain/collision_95_model.inc"
};
static NJS_OBJECT collision_95 = {
#include "stg14/terrain/collision_95.inc"
};
static NJS_MATERIAL collision_97_materials[1] = {
#include "stg14/terrain/collision_97_materials.inc"
};
static Sint16 collision_97_faces[31] = {
#include "stg14/terrain/collision_97_faces.inc"
};
static NJS_MESHSET collision_97_meshsets[1] = {
#include "stg14/terrain/collision_97_meshsets.inc"
};
static NJS_POINT3 collision_97_points[30] = {
#include "stg14/terrain/collision_97_points.inc"
};
static NJS_MODEL collision_97_model = {
#include "stg14/terrain/collision_97_model.inc"
};
static NJS_OBJECT collision_97 = {
#include "stg14/terrain/collision_97.inc"
};
static NJS_MATERIAL collision_98_materials[1] = {
#include "stg14/terrain/collision_98_materials.inc"
};
static Sint16 collision_98_faces[34] = {
#include "stg14/terrain/collision_98_faces.inc"
};
static NJS_MESHSET collision_98_meshsets[1] = {
#include "stg14/terrain/collision_98_meshsets.inc"
};
static NJS_POINT3 collision_98_points[32] = {
#include "stg14/terrain/collision_98_points.inc"
};
static NJS_MODEL collision_98_model = {
#include "stg14/terrain/collision_98_model.inc"
};
static NJS_OBJECT collision_98 = {
#include "stg14/terrain/collision_98.inc"
};
static NJS_MATERIAL collision_99_materials[1] = {
#include "stg14/terrain/collision_99_materials.inc"
};
static Sint16 collision_99_faces[10] = {
#include "stg14/terrain/collision_99_faces.inc"
};
static NJS_MESHSET collision_99_meshsets[1] = {
#include "stg14/terrain/collision_99_meshsets.inc"
};
static NJS_POINT3 collision_99_points[8] = {
#include "stg14/terrain/collision_99_points.inc"
};
static NJS_MODEL collision_99_model = {
#include "stg14/terrain/collision_99_model.inc"
};
static NJS_OBJECT collision_99 = {
#include "stg14/terrain/collision_99.inc"
};
static NJS_MATERIAL collision_100_materials[1] = {
#include "stg14/terrain/collision_100_materials.inc"
};
static Sint16 collision_100_faces[30] = {
#include "stg14/terrain/collision_100_faces.inc"
};
static NJS_MESHSET collision_100_meshsets[1] = {
#include "stg14/terrain/collision_100_meshsets.inc"
};
static NJS_POINT3 collision_100_points[28] = {
#include "stg14/terrain/collision_100_points.inc"
};
static NJS_MODEL collision_100_model = {
#include "stg14/terrain/collision_100_model.inc"
};
static NJS_OBJECT collision_100 = {
#include "stg14/terrain/collision_100.inc"
};
static NJS_MATERIAL collision_104_materials[1] = {
#include "stg14/terrain/collision_104_materials.inc"
};
static Sint16 collision_104_faces[38] = {
#include "stg14/terrain/collision_104_faces.inc"
};
static NJS_MESHSET collision_104_meshsets[1] = {
#include "stg14/terrain/collision_104_meshsets.inc"
};
static NJS_POINT3 collision_104_points[36] = {
#include "stg14/terrain/collision_104_points.inc"
};
static NJS_MODEL collision_104_model = {
#include "stg14/terrain/collision_104_model.inc"
};
static NJS_OBJECT collision_104 = {
#include "stg14/terrain/collision_104.inc"
};
static NJS_MATERIAL collision_105_materials[1] = {
#include "stg14/terrain/collision_105_materials.inc"
};
static Sint16 collision_105_faces[54] = {
#include "stg14/terrain/collision_105_faces.inc"
};
static NJS_MESHSET collision_105_meshsets[1] = {
#include "stg14/terrain/collision_105_meshsets.inc"
};
static NJS_POINT3 collision_105_points[52] = {
#include "stg14/terrain/collision_105_points.inc"
};
static NJS_MODEL collision_105_model = {
#include "stg14/terrain/collision_105_model.inc"
};
static NJS_OBJECT collision_105 = {
#include "stg14/terrain/collision_105.inc"
};
static NJS_MATERIAL collision_107_materials[1] = {
#include "stg14/terrain/collision_107_materials.inc"
};
static Sint16 collision_107_faces[38] = {
#include "stg14/terrain/collision_107_faces.inc"
};
static NJS_MESHSET collision_107_meshsets[1] = {
#include "stg14/terrain/collision_107_meshsets.inc"
};
static NJS_POINT3 collision_107_points[36] = {
#include "stg14/terrain/collision_107_points.inc"
};
static NJS_MODEL collision_107_model = {
#include "stg14/terrain/collision_107_model.inc"
};
static NJS_OBJECT collision_107 = {
#include "stg14/terrain/collision_107.inc"
};
static NJS_MATERIAL collision_110_materials[1] = {
#include "stg14/terrain/collision_110_materials.inc"
};
static Sint16 collision_110_faces[38] = {
#include "stg14/terrain/collision_110_faces.inc"
};
static NJS_MESHSET collision_110_meshsets[1] = {
#include "stg14/terrain/collision_110_meshsets.inc"
};
static NJS_POINT3 collision_110_points[36] = {
#include "stg14/terrain/collision_110_points.inc"
};
static NJS_MODEL collision_110_model = {
#include "stg14/terrain/collision_110_model.inc"
};
static NJS_OBJECT collision_110 = {
#include "stg14/terrain/collision_110.inc"
};
static NJS_MATERIAL collision_112_materials[1] = {
#include "stg14/terrain/collision_112_materials.inc"
};
static Sint16 collision_112_faces[66] = {
#include "stg14/terrain/collision_112_faces.inc"
};
static NJS_MESHSET collision_112_meshsets[1] = {
#include "stg14/terrain/collision_112_meshsets.inc"
};
static NJS_POINT3 collision_112_points[64] = {
#include "stg14/terrain/collision_112_points.inc"
};
static NJS_MODEL collision_112_model = {
#include "stg14/terrain/collision_112_model.inc"
};
static NJS_OBJECT collision_112 = {
#include "stg14/terrain/collision_112.inc"
};
static NJS_MATERIAL collision_114_materials[1] = {
#include "stg14/terrain/collision_114_materials.inc"
};
static Sint16 collision_114_faces[28] = {
#include "stg14/terrain/collision_114_faces.inc"
};
static NJS_MESHSET collision_114_meshsets[1] = {
#include "stg14/terrain/collision_114_meshsets.inc"
};
static NJS_POINT3 collision_114_points[18] = {
#include "stg14/terrain/collision_114_points.inc"
};
static NJS_MODEL collision_114_model = {
#include "stg14/terrain/collision_114_model.inc"
};
static NJS_OBJECT collision_114 = {
#include "stg14/terrain/collision_114.inc"
};
static NJS_MATERIAL collision_115_materials[1] = {
#include "stg14/terrain/collision_115_materials.inc"
};
static Sint16 collision_115_faces[66] = {
#include "stg14/terrain/collision_115_faces.inc"
};
static NJS_MESHSET collision_115_meshsets[1] = {
#include "stg14/terrain/collision_115_meshsets.inc"
};
static NJS_POINT3 collision_115_points[64] = {
#include "stg14/terrain/collision_115_points.inc"
};
static NJS_MODEL collision_115_model = {
#include "stg14/terrain/collision_115_model.inc"
};
static NJS_OBJECT collision_115 = {
#include "stg14/terrain/collision_115.inc"
};
static NJS_MATERIAL collision_117_materials[1] = {
#include "stg14/terrain/collision_117_materials.inc"
};
static Sint16 collision_117_faces[132] = {
#include "stg14/terrain/collision_117_faces.inc"
};
static NJS_MESHSET collision_117_meshsets[1] = {
#include "stg14/terrain/collision_117_meshsets.inc"
};
static NJS_POINT3 collision_117_points[78] = {
#include "stg14/terrain/collision_117_points.inc"
};
static NJS_MODEL collision_117_model = {
#include "stg14/terrain/collision_117_model.inc"
};
static NJS_OBJECT collision_117 = {
#include "stg14/terrain/collision_117.inc"
};
static NJS_MATERIAL collision_119_materials[1] = {
#include "stg14/terrain/collision_119_materials.inc"
};
static Sint16 collision_119_faces[91] = {
#include "stg14/terrain/collision_119_faces.inc"
};
static NJS_COLOR collision_119_basic_uvs[81] = {
#include "stg14/terrain/collision_119_basic_uvs.inc"
};
static NJS_MESHSET collision_119_meshsets[1] = {
#include "stg14/terrain/collision_119_meshsets.inc"
};
static NJS_POINT3 collision_119_points[44] = {
#include "stg14/terrain/collision_119_points.inc"
};
static NJS_MODEL collision_119_model = {
#include "stg14/terrain/collision_119_model.inc"
};
static NJS_OBJECT collision_119 = {
#include "stg14/terrain/collision_119.inc"
};
static NJS_MATERIAL collision_120_materials[1] = {
#include "stg14/terrain/collision_120_materials.inc"
};
static Sint16 collision_120_faces[54] = {
#include "stg14/terrain/collision_120_faces.inc"
};
static NJS_MESHSET collision_120_meshsets[1] = {
#include "stg14/terrain/collision_120_meshsets.inc"
};
static NJS_POINT3 collision_120_points[52] = {
#include "stg14/terrain/collision_120_points.inc"
};
static NJS_MODEL collision_120_model = {
#include "stg14/terrain/collision_120_model.inc"
};
static NJS_OBJECT collision_120 = {
#include "stg14/terrain/collision_120.inc"
};
static NJS_MATERIAL collision_122_materials[1] = {
#include "stg14/terrain/collision_122_materials.inc"
};
static Sint16 collision_122_faces[65] = {
#include "stg14/terrain/collision_122_faces.inc"
};
static NJS_MESHSET collision_122_meshsets[1] = {
#include "stg14/terrain/collision_122_meshsets.inc"
};
static NJS_POINT3 collision_122_points[36] = {
#include "stg14/terrain/collision_122_points.inc"
};
static NJS_MODEL collision_122_model = {
#include "stg14/terrain/collision_122_model.inc"
};
static NJS_OBJECT collision_122 = {
#include "stg14/terrain/collision_122.inc"
};
static NJS_MATERIAL collision_123_materials[1] = {
#include "stg14/terrain/collision_123_materials.inc"
};
static Sint16 collision_123_faces[7] = {
#include "stg14/terrain/collision_123_faces.inc"
};
static NJS_MESHSET collision_123_meshsets[1] = {
#include "stg14/terrain/collision_123_meshsets.inc"
};
static NJS_POINT3 collision_123_points[6] = {
#include "stg14/terrain/collision_123_points.inc"
};
static NJS_MODEL collision_123_model = {
#include "stg14/terrain/collision_123_model.inc"
};
static NJS_OBJECT collision_123 = {
#include "stg14/terrain/collision_123.inc"
};
static NJS_MATERIAL collision_124_materials[1] = {
#include "stg14/terrain/collision_124_materials.inc"
};
static Sint16 collision_124_faces[17] = {
#include "stg14/terrain/collision_124_faces.inc"
};
static NJS_MESHSET collision_124_meshsets[1] = {
#include "stg14/terrain/collision_124_meshsets.inc"
};
static NJS_POINT3 collision_124_points[12] = {
#include "stg14/terrain/collision_124_points.inc"
};
static NJS_MODEL collision_124_model = {
#include "stg14/terrain/collision_124_model.inc"
};
static NJS_OBJECT collision_124 = {
#include "stg14/terrain/collision_124.inc"
};
static NJS_MATERIAL collision_130_materials[1] = {
#include "stg14/terrain/collision_130_materials.inc"
};
static Sint16 collision_130_faces[34] = {
#include "stg14/terrain/collision_130_faces.inc"
};
static NJS_MESHSET collision_130_meshsets[1] = {
#include "stg14/terrain/collision_130_meshsets.inc"
};
static NJS_POINT3 collision_130_points[32] = {
#include "stg14/terrain/collision_130_points.inc"
};
static NJS_MODEL collision_130_model = {
#include "stg14/terrain/collision_130_model.inc"
};
static NJS_OBJECT collision_130 = {
#include "stg14/terrain/collision_130.inc"
};
static NJS_MATERIAL collision_134_materials[1] = {
#include "stg14/terrain/collision_134_materials.inc"
};
static Sint16 collision_134_faces[18] = {
#include "stg14/terrain/collision_134_faces.inc"
};
static NJS_MESHSET collision_134_meshsets[1] = {
#include "stg14/terrain/collision_134_meshsets.inc"
};
static NJS_POINT3 collision_134_points[16] = {
#include "stg14/terrain/collision_134_points.inc"
};
static NJS_MODEL collision_134_model = {
#include "stg14/terrain/collision_134_model.inc"
};
static NJS_OBJECT collision_134 = {
#include "stg14/terrain/collision_134.inc"
};
static NJS_MATERIAL collision_136_materials[1] = {
#include "stg14/terrain/collision_136_materials.inc"
};
static Sint16 collision_136_faces[22] = {
#include "stg14/terrain/collision_136_faces.inc"
};
static NJS_MESHSET collision_136_meshsets[1] = {
#include "stg14/terrain/collision_136_meshsets.inc"
};
static NJS_POINT3 collision_136_points[20] = {
#include "stg14/terrain/collision_136_points.inc"
};
static NJS_MODEL collision_136_model = {
#include "stg14/terrain/collision_136_model.inc"
};
static NJS_OBJECT collision_136 = {
#include "stg14/terrain/collision_136.inc"
};
static NJS_MATERIAL collision_139_materials[1] = {
#include "stg14/terrain/collision_139_materials.inc"
};
static Sint16 collision_139_faces[22] = {
#include "stg14/terrain/collision_139_faces.inc"
};
static NJS_MESHSET collision_139_meshsets[1] = {
#include "stg14/terrain/collision_139_meshsets.inc"
};
static NJS_POINT3 collision_139_points[20] = {
#include "stg14/terrain/collision_139_points.inc"
};
static NJS_MODEL collision_139_model = {
#include "stg14/terrain/collision_139_model.inc"
};
static NJS_OBJECT collision_139 = {
#include "stg14/terrain/collision_139.inc"
};
static NJS_MATERIAL collision_142_materials[1] = {
#include "stg14/terrain/collision_142_materials.inc"
};
static Sint16 collision_142_faces[10] = {
#include "stg14/terrain/collision_142_faces.inc"
};
static NJS_MESHSET collision_142_meshsets[1] = {
#include "stg14/terrain/collision_142_meshsets.inc"
};
static NJS_POINT3 collision_142_points[8] = {
#include "stg14/terrain/collision_142_points.inc"
};
static NJS_MODEL collision_142_model = {
#include "stg14/terrain/collision_142_model.inc"
};
static NJS_OBJECT collision_142 = {
#include "stg14/terrain/collision_142.inc"
};
static NJS_MATERIAL collision_143_materials[1] = {
#include "stg14/terrain/collision_143_materials.inc"
};
static Sint16 collision_143_faces[10] = {
#include "stg14/terrain/collision_143_faces.inc"
};
static NJS_MESHSET collision_143_meshsets[1] = {
#include "stg14/terrain/collision_143_meshsets.inc"
};
static NJS_POINT3 collision_143_points[8] = {
#include "stg14/terrain/collision_143_points.inc"
};
static NJS_MODEL collision_143_model = {
#include "stg14/terrain/collision_143_model.inc"
};
static NJS_OBJECT collision_143 = {
#include "stg14/terrain/collision_143.inc"
};
static NJS_MATERIAL collision_144_materials[1] = {
#include "stg14/terrain/collision_144_materials.inc"
};
static Sint16 collision_144_faces[34] = {
#include "stg14/terrain/collision_144_faces.inc"
};
static NJS_MESHSET collision_144_meshsets[1] = {
#include "stg14/terrain/collision_144_meshsets.inc"
};
static NJS_POINT3 collision_144_points[32] = {
#include "stg14/terrain/collision_144_points.inc"
};
static NJS_MODEL collision_144_model = {
#include "stg14/terrain/collision_144_model.inc"
};
static NJS_OBJECT collision_144 = {
#include "stg14/terrain/collision_144.inc"
};
static NJS_MATERIAL collision_146_materials[1] = {
#include "stg14/terrain/collision_146_materials.inc"
};
static Sint16 collision_146_faces[34] = {
#include "stg14/terrain/collision_146_faces.inc"
};
static NJS_MESHSET collision_146_meshsets[1] = {
#include "stg14/terrain/collision_146_meshsets.inc"
};
static NJS_POINT3 collision_146_points[32] = {
#include "stg14/terrain/collision_146_points.inc"
};
static NJS_MODEL collision_146_model = {
#include "stg14/terrain/collision_146_model.inc"
};
static NJS_OBJECT collision_146 = {
#include "stg14/terrain/collision_146.inc"
};
static NJS_MATERIAL collision_148_materials[1] = {
#include "stg14/terrain/collision_148_materials.inc"
};
static Sint16 collision_148_faces[19] = {
#include "stg14/terrain/collision_148_faces.inc"
};
static NJS_MESHSET collision_148_meshsets[1] = {
#include "stg14/terrain/collision_148_meshsets.inc"
};
static NJS_POINT3 collision_148_points[16] = {
#include "stg14/terrain/collision_148_points.inc"
};
static NJS_MODEL collision_148_model = {
#include "stg14/terrain/collision_148_model.inc"
};
static NJS_OBJECT collision_148 = {
#include "stg14/terrain/collision_148.inc"
};
static NJS_MATERIAL collision_149_materials[1] = {
#include "stg14/terrain/collision_149_materials.inc"
};
static Sint16 collision_149_faces[19] = {
#include "stg14/terrain/collision_149_faces.inc"
};
static NJS_MESHSET collision_149_meshsets[1] = {
#include "stg14/terrain/collision_149_meshsets.inc"
};
static NJS_POINT3 collision_149_points[16] = {
#include "stg14/terrain/collision_149_points.inc"
};
static NJS_MODEL collision_149_model = {
#include "stg14/terrain/collision_149_model.inc"
};
static NJS_OBJECT collision_149 = {
#include "stg14/terrain/collision_149.inc"
};
static NJS_MATERIAL collision_151_materials[1] = {
#include "stg14/terrain/collision_151_materials.inc"
};
static Sint16 collision_151_faces[17] = {
#include "stg14/terrain/collision_151_faces.inc"
};
static NJS_MESHSET collision_151_meshsets[1] = {
#include "stg14/terrain/collision_151_meshsets.inc"
};
static NJS_POINT3 collision_151_points[8] = {
#include "stg14/terrain/collision_151_points.inc"
};
static NJS_MODEL collision_151_model = {
#include "stg14/terrain/collision_151_model.inc"
};
static NJS_OBJECT collision_151 = {
#include "stg14/terrain/collision_151.inc"
};
static NJS_MATERIAL collision_152_materials[1] = {
#include "stg14/terrain/collision_152_materials.inc"
};
static Sint16 collision_152_faces[7] = {
#include "stg14/terrain/collision_152_faces.inc"
};
static NJS_MESHSET collision_152_meshsets[1] = {
#include "stg14/terrain/collision_152_meshsets.inc"
};
static NJS_POINT3 collision_152_points[6] = {
#include "stg14/terrain/collision_152_points.inc"
};
static NJS_MODEL collision_152_model = {
#include "stg14/terrain/collision_152_model.inc"
};
static NJS_OBJECT collision_152 = {
#include "stg14/terrain/collision_152.inc"
};
static NJS_MATERIAL collision_153_materials[1] = {
#include "stg14/terrain/collision_153_materials.inc"
};
static Sint16 collision_153_faces[9] = {
#include "stg14/terrain/collision_153_faces.inc"
};
static NJS_MESHSET collision_153_meshsets[1] = {
#include "stg14/terrain/collision_153_meshsets.inc"
};
static NJS_POINT3 collision_153_points[8] = {
#include "stg14/terrain/collision_153_points.inc"
};
static NJS_MODEL collision_153_model = {
#include "stg14/terrain/collision_153_model.inc"
};
static NJS_OBJECT collision_153 = {
#include "stg14/terrain/collision_153.inc"
};
static NJS_MATERIAL collision_155_materials[1] = {
#include "stg14/terrain/collision_155_materials.inc"
};
static Sint16 collision_155_faces[10] = {
#include "stg14/terrain/collision_155_faces.inc"
};
static NJS_MESHSET collision_155_meshsets[1] = {
#include "stg14/terrain/collision_155_meshsets.inc"
};
static NJS_POINT3 collision_155_points[6] = {
#include "stg14/terrain/collision_155_points.inc"
};
static NJS_MODEL collision_155_model = {
#include "stg14/terrain/collision_155_model.inc"
};
static NJS_OBJECT collision_155 = {
#include "stg14/terrain/collision_155.inc"
};
static NJS_MATERIAL collision_156_materials[1] = {
#include "stg14/terrain/collision_156_materials.inc"
};
static Sint16 collision_156_faces[7] = {
#include "stg14/terrain/collision_156_faces.inc"
};
static NJS_MESHSET collision_156_meshsets[1] = {
#include "stg14/terrain/collision_156_meshsets.inc"
};
static NJS_POINT3 collision_156_points[6] = {
#include "stg14/terrain/collision_156_points.inc"
};
static NJS_MODEL collision_156_model = {
#include "stg14/terrain/collision_156_model.inc"
};
static NJS_OBJECT collision_156 = {
#include "stg14/terrain/collision_156.inc"
};
static NJS_MATERIAL collision_157_materials[1] = {
#include "stg14/terrain/collision_157_materials.inc"
};
static Sint16 collision_157_faces[45] = {
#include "stg14/terrain/collision_157_faces.inc"
};
static NJS_MESHSET collision_157_meshsets[1] = {
#include "stg14/terrain/collision_157_meshsets.inc"
};
static NJS_POINT3 collision_157_points[26] = {
#include "stg14/terrain/collision_157_points.inc"
};
static NJS_MODEL collision_157_model = {
#include "stg14/terrain/collision_157_model.inc"
};
static NJS_OBJECT collision_157 = {
#include "stg14/terrain/collision_157.inc"
};
static NJS_MATERIAL collision_165_materials[1] = {
#include "stg14/terrain/collision_165_materials.inc"
};
static Sint16 collision_165_faces[14] = {
#include "stg14/terrain/collision_165_faces.inc"
};
static NJS_MESHSET collision_165_meshsets[1] = {
#include "stg14/terrain/collision_165_meshsets.inc"
};
static NJS_POINT3 collision_165_points[8] = {
#include "stg14/terrain/collision_165_points.inc"
};
static NJS_MODEL collision_165_model = {
#include "stg14/terrain/collision_165_model.inc"
};
static NJS_OBJECT collision_165 = {
#include "stg14/terrain/collision_165.inc"
};
static NJS_MATERIAL collision_167_materials[1] = {
#include "stg14/terrain/collision_167_materials.inc"
};
static Sint16 collision_167_faces[15] = {
#include "stg14/terrain/collision_167_faces.inc"
};
static NJS_MESHSET collision_167_meshsets[1] = {
#include "stg14/terrain/collision_167_meshsets.inc"
};
static NJS_POINT3 collision_167_points[12] = {
#include "stg14/terrain/collision_167_points.inc"
};
static NJS_MODEL collision_167_model = {
#include "stg14/terrain/collision_167_model.inc"
};
static NJS_OBJECT collision_167 = {
#include "stg14/terrain/collision_167.inc"
};
static NJS_MATERIAL collision_177_materials[1] = {
#include "stg14/terrain/collision_177_materials.inc"
};
static Sint16 collision_177_faces[224] = {
#include "stg14/terrain/collision_177_faces.inc"
};
static NJS_MESHSET collision_177_meshsets[1] = {
#include "stg14/terrain/collision_177_meshsets.inc"
};
static NJS_POINT3 collision_177_points[100] = {
#include "stg14/terrain/collision_177_points.inc"
};
static NJS_MODEL collision_177_model = {
#include "stg14/terrain/collision_177_model.inc"
};
static NJS_OBJECT collision_177 = {
#include "stg14/terrain/collision_177.inc"
};
static NJS_MATERIAL collision_179_materials[1] = {
#include "stg14/terrain/collision_179_materials.inc"
};
static Sint16 collision_179_faces[39] = {
#include "stg14/terrain/collision_179_faces.inc"
};
static NJS_MESHSET collision_179_meshsets[1] = {
#include "stg14/terrain/collision_179_meshsets.inc"
};
static NJS_POINT3 collision_179_points[24] = {
#include "stg14/terrain/collision_179_points.inc"
};
static NJS_MODEL collision_179_model = {
#include "stg14/terrain/collision_179_model.inc"
};
static NJS_OBJECT collision_179 = {
#include "stg14/terrain/collision_179.inc"
};
static NJS_MATERIAL collision_189_materials[1] = {
#include "stg14/terrain/collision_189_materials.inc"
};
static Sint16 collision_189_faces[288] = {
#include "stg14/terrain/collision_189_faces.inc"
};
static NJS_MESHSET collision_189_meshsets[1] = {
#include "stg14/terrain/collision_189_meshsets.inc"
};
static NJS_POINT3 collision_189_points[152] = {
#include "stg14/terrain/collision_189_points.inc"
};
static NJS_MODEL collision_189_model = {
#include "stg14/terrain/collision_189_model.inc"
};
static NJS_OBJECT collision_189 = {
#include "stg14/terrain/collision_189.inc"
};
static NJS_MATERIAL collision_190_materials[1] = {
#include "stg14/terrain/collision_190_materials.inc"
};
static Sint16 collision_190_faces[7] = {
#include "stg14/terrain/collision_190_faces.inc"
};
static NJS_MESHSET collision_190_meshsets[1] = {
#include "stg14/terrain/collision_190_meshsets.inc"
};
static NJS_POINT3 collision_190_points[6] = {
#include "stg14/terrain/collision_190_points.inc"
};
static NJS_MODEL collision_190_model = {
#include "stg14/terrain/collision_190_model.inc"
};
static NJS_OBJECT collision_190 = {
#include "stg14/terrain/collision_190.inc"
};
static NJS_MATERIAL collision_191_materials[1] = {
#include "stg14/terrain/collision_191_materials.inc"
};
static Sint16 collision_191_faces[33] = {
#include "stg14/terrain/collision_191_faces.inc"
};
static NJS_MESHSET collision_191_meshsets[1] = {
#include "stg14/terrain/collision_191_meshsets.inc"
};
static NJS_POINT3 collision_191_points[26] = {
#include "stg14/terrain/collision_191_points.inc"
};
static NJS_MODEL collision_191_model = {
#include "stg14/terrain/collision_191_model.inc"
};
static NJS_OBJECT collision_191 = {
#include "stg14/terrain/collision_191.inc"
};
static NJS_MATERIAL collision_193_materials[1] = {
#include "stg14/terrain/collision_193_materials.inc"
};
static Sint16 collision_193_faces[17] = {
#include "stg14/terrain/collision_193_faces.inc"
};
static NJS_MESHSET collision_193_meshsets[1] = {
#include "stg14/terrain/collision_193_meshsets.inc"
};
static NJS_POINT3 collision_193_points[9] = {
#include "stg14/terrain/collision_193_points.inc"
};
static NJS_MODEL collision_193_model = {
#include "stg14/terrain/collision_193_model.inc"
};
static NJS_OBJECT collision_193 = {
#include "stg14/terrain/collision_193.inc"
};
static NJS_MATERIAL collision_194_materials[1] = {
#include "stg14/terrain/collision_194_materials.inc"
};
static Sint16 collision_194_faces[31] = {
#include "stg14/terrain/collision_194_faces.inc"
};
static NJS_MESHSET collision_194_meshsets[1] = {
#include "stg14/terrain/collision_194_meshsets.inc"
};
static NJS_POINT3 collision_194_points[16] = {
#include "stg14/terrain/collision_194_points.inc"
};
static NJS_MODEL collision_194_model = {
#include "stg14/terrain/collision_194_model.inc"
};
static NJS_OBJECT collision_194 = {
#include "stg14/terrain/collision_194.inc"
};
static NJS_MATERIAL collision_195_materials[1] = {
#include "stg14/terrain/collision_195_materials.inc"
};
static Sint16 collision_195_faces[31] = {
#include "stg14/terrain/collision_195_faces.inc"
};
static NJS_MESHSET collision_195_meshsets[1] = {
#include "stg14/terrain/collision_195_meshsets.inc"
};
static NJS_POINT3 collision_195_points[16] = {
#include "stg14/terrain/collision_195_points.inc"
};
static NJS_MODEL collision_195_model = {
#include "stg14/terrain/collision_195_model.inc"
};
static NJS_OBJECT collision_195 = {
#include "stg14/terrain/collision_195.inc"
};
static NJS_MATERIAL collision_196_materials[1] = {
#include "stg14/terrain/collision_196_materials.inc"
};
static Sint16 collision_196_faces[31] = {
#include "stg14/terrain/collision_196_faces.inc"
};
static NJS_MESHSET collision_196_meshsets[1] = {
#include "stg14/terrain/collision_196_meshsets.inc"
};
static NJS_POINT3 collision_196_points[16] = {
#include "stg14/terrain/collision_196_points.inc"
};
static NJS_MODEL collision_196_model = {
#include "stg14/terrain/collision_196_model.inc"
};
static NJS_OBJECT collision_196 = {
#include "stg14/terrain/collision_196.inc"
};
static NJS_MATERIAL collision_197_materials[1] = {
#include "stg14/terrain/collision_197_materials.inc"
};
static Sint16 collision_197_faces[31] = {
#include "stg14/terrain/collision_197_faces.inc"
};
static NJS_MESHSET collision_197_meshsets[1] = {
#include "stg14/terrain/collision_197_meshsets.inc"
};
static NJS_POINT3 collision_197_points[16] = {
#include "stg14/terrain/collision_197_points.inc"
};
static NJS_MODEL collision_197_model = {
#include "stg14/terrain/collision_197_model.inc"
};
static NJS_OBJECT collision_197 = {
#include "stg14/terrain/collision_197.inc"
};
static NJS_MATERIAL collision_198_materials[1] = {
#include "stg14/terrain/collision_198_materials.inc"
};
static Sint16 collision_198_faces[31] = {
#include "stg14/terrain/collision_198_faces.inc"
};
static NJS_MESHSET collision_198_meshsets[1] = {
#include "stg14/terrain/collision_198_meshsets.inc"
};
static NJS_POINT3 collision_198_points[16] = {
#include "stg14/terrain/collision_198_points.inc"
};
static NJS_MODEL collision_198_model = {
#include "stg14/terrain/collision_198_model.inc"
};
static NJS_OBJECT collision_198 = {
#include "stg14/terrain/collision_198.inc"
};
static NJS_MATERIAL collision_199_materials[1] = {
#include "stg14/terrain/collision_199_materials.inc"
};
static Sint16 collision_199_faces[31] = {
#include "stg14/terrain/collision_199_faces.inc"
};
static NJS_MESHSET collision_199_meshsets[1] = {
#include "stg14/terrain/collision_199_meshsets.inc"
};
static NJS_POINT3 collision_199_points[16] = {
#include "stg14/terrain/collision_199_points.inc"
};
static NJS_MODEL collision_199_model = {
#include "stg14/terrain/collision_199_model.inc"
};
static NJS_OBJECT collision_199 = {
#include "stg14/terrain/collision_199.inc"
};
static NJS_MATERIAL collision_200_materials[1] = {
#include "stg14/terrain/collision_200_materials.inc"
};
static Sint16 collision_200_faces[31] = {
#include "stg14/terrain/collision_200_faces.inc"
};
static NJS_MESHSET collision_200_meshsets[1] = {
#include "stg14/terrain/collision_200_meshsets.inc"
};
static NJS_POINT3 collision_200_points[16] = {
#include "stg14/terrain/collision_200_points.inc"
};
static NJS_MODEL collision_200_model = {
#include "stg14/terrain/collision_200_model.inc"
};
static NJS_OBJECT collision_200 = {
#include "stg14/terrain/collision_200.inc"
};
static NJS_MATERIAL collision_201_materials[1] = {
#include "stg14/terrain/collision_201_materials.inc"
};
static Sint16 collision_201_faces[31] = {
#include "stg14/terrain/collision_201_faces.inc"
};
static NJS_MESHSET collision_201_meshsets[1] = {
#include "stg14/terrain/collision_201_meshsets.inc"
};
static NJS_POINT3 collision_201_points[16] = {
#include "stg14/terrain/collision_201_points.inc"
};
static NJS_MODEL collision_201_model = {
#include "stg14/terrain/collision_201_model.inc"
};
static NJS_OBJECT collision_201 = {
#include "stg14/terrain/collision_201.inc"
};
static NJS_MATERIAL collision_202_materials[1] = {
#include "stg14/terrain/collision_202_materials.inc"
};
static Sint16 collision_202_faces[31] = {
#include "stg14/terrain/collision_202_faces.inc"
};
static NJS_MESHSET collision_202_meshsets[1] = {
#include "stg14/terrain/collision_202_meshsets.inc"
};
static NJS_POINT3 collision_202_points[16] = {
#include "stg14/terrain/collision_202_points.inc"
};
static NJS_MODEL collision_202_model = {
#include "stg14/terrain/collision_202_model.inc"
};
static NJS_OBJECT collision_202 = {
#include "stg14/terrain/collision_202.inc"
};
static NJS_MATERIAL collision_203_materials[1] = {
#include "stg14/terrain/collision_203_materials.inc"
};
static Sint16 collision_203_faces[31] = {
#include "stg14/terrain/collision_203_faces.inc"
};
static NJS_MESHSET collision_203_meshsets[1] = {
#include "stg14/terrain/collision_203_meshsets.inc"
};
static NJS_POINT3 collision_203_points[16] = {
#include "stg14/terrain/collision_203_points.inc"
};
static NJS_MODEL collision_203_model = {
#include "stg14/terrain/collision_203_model.inc"
};
static NJS_OBJECT collision_203 = {
#include "stg14/terrain/collision_203.inc"
};
static NJS_MATERIAL collision_204_materials[2] = {
#include "stg14/terrain/collision_204_materials.inc"
};
static Sint16 collision_204_faces[12] = {
#include "stg14/terrain/collision_204_faces.inc"
};
static Sint16 collision_204_faces_1[23] = {
#include "stg14/terrain/collision_204_faces_1.inc"
};
static NJS_MESHSET collision_204_meshsets[2] = {
#include "stg14/terrain/collision_204_meshsets.inc"
};
static NJS_POINT3 collision_204_points[14] = {
#include "stg14/terrain/collision_204_points.inc"
};
static NJS_MODEL collision_204_model = {
#include "stg14/terrain/collision_204_model.inc"
};
static NJS_OBJECT collision_204 = {
#include "stg14/terrain/collision_204.inc"
};
static NJS_MATERIAL collision_207_materials[1] = {
#include "stg14/terrain/collision_207_materials.inc"
};
static Sint16 collision_207_faces[63] = {
#include "stg14/terrain/collision_207_faces.inc"
};
static NJS_MESHSET collision_207_meshsets[1] = {
#include "stg14/terrain/collision_207_meshsets.inc"
};
static NJS_POINT3 collision_207_points[62] = {
#include "stg14/terrain/collision_207_points.inc"
};
static NJS_MODEL collision_207_model = {
#include "stg14/terrain/collision_207_model.inc"
};
static NJS_OBJECT collision_207 = {
#include "stg14/terrain/collision_207.inc"
};
static NJS_MATERIAL collision_208_materials[1] = {
#include "stg14/terrain/collision_208_materials.inc"
};
static Sint16 collision_208_faces[28] = {
#include "stg14/terrain/collision_208_faces.inc"
};
static NJS_MESHSET collision_208_meshsets[1] = {
#include "stg14/terrain/collision_208_meshsets.inc"
};
static NJS_POINT3 collision_208_points[16] = {
#include "stg14/terrain/collision_208_points.inc"
};
static NJS_MODEL collision_208_model = {
#include "stg14/terrain/collision_208_model.inc"
};
static NJS_OBJECT collision_208 = {
#include "stg14/terrain/collision_208.inc"
};
static NJS_MATERIAL collision_215_materials[2] = {
#include "stg14/terrain/collision_215_materials.inc"
};
static Sint16 collision_215_faces[99] = {
#include "stg14/terrain/collision_215_faces.inc"
};
static NJS_MESHSET collision_215_meshsets[1] = {
#include "stg14/terrain/collision_215_meshsets.inc"
};
static NJS_POINT3 collision_215_points[44] = {
#include "stg14/terrain/collision_215_points.inc"
};
static NJS_MODEL collision_215_model = {
#include "stg14/terrain/collision_215_model.inc"
};
static NJS_OBJECT collision_215 = {
#include "stg14/terrain/collision_215.inc"
};
static NJS_MATERIAL collision_218_materials[1] = {
#include "stg14/terrain/collision_218_materials.inc"
};
static Sint16 collision_218_faces[22] = {
#include "stg14/terrain/collision_218_faces.inc"
};
static NJS_MESHSET collision_218_meshsets[1] = {
#include "stg14/terrain/collision_218_meshsets.inc"
};
static NJS_POINT3 collision_218_points[12] = {
#include "stg14/terrain/collision_218_points.inc"
};
static NJS_MODEL collision_218_model = {
#include "stg14/terrain/collision_218_model.inc"
};
static NJS_OBJECT collision_218 = {
#include "stg14/terrain/collision_218.inc"
};
static NJS_MATERIAL collision_220_materials[1] = {
#include "stg14/terrain/collision_220_materials.inc"
};
static Sint16 collision_220_faces[57] = {
#include "stg14/terrain/collision_220_faces.inc"
};
static NJS_MESHSET collision_220_meshsets[1] = {
#include "stg14/terrain/collision_220_meshsets.inc"
};
static NJS_POINT3 collision_220_points[54] = {
#include "stg14/terrain/collision_220_points.inc"
};
static NJS_MODEL collision_220_model = {
#include "stg14/terrain/collision_220_model.inc"
};
static NJS_OBJECT collision_220 = {
#include "stg14/terrain/collision_220.inc"
};
static NJS_MATERIAL collision_225_materials[1] = {
#include "stg14/terrain/collision_225_materials.inc"
};
static Sint16 collision_225_faces[35] = {
#include "stg14/terrain/collision_225_faces.inc"
};
static NJS_MESHSET collision_225_meshsets[1] = {
#include "stg14/terrain/collision_225_meshsets.inc"
};
static NJS_POINT3 collision_225_points[24] = {
#include "stg14/terrain/collision_225_points.inc"
};
static NJS_MODEL collision_225_model = {
#include "stg14/terrain/collision_225_model.inc"
};
static NJS_OBJECT collision_225 = {
#include "stg14/terrain/collision_225.inc"
};
static NJS_MATERIAL collision_231_materials[1] = {
#include "stg14/terrain/collision_231_materials.inc"
};
static Sint16 collision_231_faces[7] = {
#include "stg14/terrain/collision_231_faces.inc"
};
static NJS_MESHSET collision_231_meshsets[1] = {
#include "stg14/terrain/collision_231_meshsets.inc"
};
static NJS_POINT3 collision_231_points[6] = {
#include "stg14/terrain/collision_231_points.inc"
};
static NJS_MODEL collision_231_model = {
#include "stg14/terrain/collision_231_model.inc"
};
static NJS_OBJECT collision_231 = {
#include "stg14/terrain/collision_231.inc"
};
static NJS_MATERIAL collision_232_materials[1] = {
#include "stg14/terrain/collision_232_materials.inc"
};
static Sint16 collision_232_faces[7] = {
#include "stg14/terrain/collision_232_faces.inc"
};
static NJS_MESHSET collision_232_meshsets[1] = {
#include "stg14/terrain/collision_232_meshsets.inc"
};
static NJS_POINT3 collision_232_points[6] = {
#include "stg14/terrain/collision_232_points.inc"
};
static NJS_MODEL collision_232_model = {
#include "stg14/terrain/collision_232_model.inc"
};
static NJS_OBJECT collision_232 = {
#include "stg14/terrain/collision_232.inc"
};
static NJS_MATERIAL collision_233_materials[1] = {
#include "stg14/terrain/collision_233_materials.inc"
};
static Sint16 collision_233_faces[7] = {
#include "stg14/terrain/collision_233_faces.inc"
};
static NJS_MESHSET collision_233_meshsets[1] = {
#include "stg14/terrain/collision_233_meshsets.inc"
};
static NJS_POINT3 collision_233_points[6] = {
#include "stg14/terrain/collision_233_points.inc"
};
static NJS_MODEL collision_233_model = {
#include "stg14/terrain/collision_233_model.inc"
};
static NJS_OBJECT collision_233 = {
#include "stg14/terrain/collision_233.inc"
};
static NJS_MATERIAL collision_234_materials[1] = {
#include "stg14/terrain/collision_234_materials.inc"
};
static Sint16 collision_234_faces[7] = {
#include "stg14/terrain/collision_234_faces.inc"
};
static NJS_MESHSET collision_234_meshsets[1] = {
#include "stg14/terrain/collision_234_meshsets.inc"
};
static NJS_POINT3 collision_234_points[6] = {
#include "stg14/terrain/collision_234_points.inc"
};
static NJS_MODEL collision_234_model = {
#include "stg14/terrain/collision_234_model.inc"
};
static NJS_OBJECT collision_234 = {
#include "stg14/terrain/collision_234.inc"
};
static NJS_MATERIAL collision_235_materials[1] = {
#include "stg14/terrain/collision_235_materials.inc"
};
static Sint16 collision_235_faces[28] = {
#include "stg14/terrain/collision_235_faces.inc"
};
static NJS_MESHSET collision_235_meshsets[1] = {
#include "stg14/terrain/collision_235_meshsets.inc"
};
static NJS_POINT3 collision_235_points[21] = {
#include "stg14/terrain/collision_235_points.inc"
};
static NJS_MODEL collision_235_model = {
#include "stg14/terrain/collision_235_model.inc"
};
static NJS_OBJECT collision_235 = {
#include "stg14/terrain/collision_235.inc"
};
static NJS_MATERIAL collision_236_materials[1] = {
#include "stg14/terrain/collision_236_materials.inc"
};
static Sint16 collision_236_faces[136] = {
#include "stg14/terrain/collision_236_faces.inc"
};
static NJS_MESHSET collision_236_meshsets[1] = {
#include "stg14/terrain/collision_236_meshsets.inc"
};
static NJS_POINT3 collision_236_points[60] = {
#include "stg14/terrain/collision_236_points.inc"
};
static NJS_MODEL collision_236_model = {
#include "stg14/terrain/collision_236_model.inc"
};
static NJS_OBJECT collision_236 = {
#include "stg14/terrain/collision_236.inc"
};
static NJS_MATERIAL collision_239_materials[1] = {
#include "stg14/terrain/collision_239_materials.inc"
};
static Sint16 collision_239_faces[36] = {
#include "stg14/terrain/collision_239_faces.inc"
};
static NJS_MESHSET collision_239_meshsets[1] = {
#include "stg14/terrain/collision_239_meshsets.inc"
};
static NJS_POINT3 collision_239_points[19] = {
#include "stg14/terrain/collision_239_points.inc"
};
static NJS_MODEL collision_239_model = {
#include "stg14/terrain/collision_239_model.inc"
};
static NJS_OBJECT collision_239 = {
#include "stg14/terrain/collision_239.inc"
};
static NJS_MATERIAL collision_246_materials[1] = {
#include "stg14/terrain/collision_246_materials.inc"
};
static Sint16 collision_246_faces[10] = {
#include "stg14/terrain/collision_246_faces.inc"
};
static NJS_MESHSET collision_246_meshsets[1] = {
#include "stg14/terrain/collision_246_meshsets.inc"
};
static NJS_POINT3 collision_246_points[6] = {
#include "stg14/terrain/collision_246_points.inc"
};
static NJS_MODEL collision_246_model = {
#include "stg14/terrain/collision_246_model.inc"
};
static NJS_OBJECT collision_246 = {
#include "stg14/terrain/collision_246.inc"
};
static NJS_MATERIAL collision_248_materials[1] = {
#include "stg14/terrain/collision_248_materials.inc"
};
static Sint16 collision_248_faces[21] = {
#include "stg14/terrain/collision_248_faces.inc"
};
static NJS_MESHSET collision_248_meshsets[1] = {
#include "stg14/terrain/collision_248_meshsets.inc"
};
static NJS_POINT3 collision_248_points[12] = {
#include "stg14/terrain/collision_248_points.inc"
};
static NJS_MODEL collision_248_model = {
#include "stg14/terrain/collision_248_model.inc"
};
static NJS_OBJECT collision_248 = {
#include "stg14/terrain/collision_248.inc"
};
static NJS_MATERIAL collision_249_materials[1] = {
#include "stg14/terrain/collision_249_materials.inc"
};
static Sint16 collision_249_faces[81] = {
#include "stg14/terrain/collision_249_faces.inc"
};
static NJS_MESHSET collision_249_meshsets[1] = {
#include "stg14/terrain/collision_249_meshsets.inc"
};
static NJS_POINT3 collision_249_points[52] = {
#include "stg14/terrain/collision_249_points.inc"
};
static NJS_MODEL collision_249_model = {
#include "stg14/terrain/collision_249_model.inc"
};
static NJS_OBJECT collision_249 = {
#include "stg14/terrain/collision_249.inc"
};
static NJS_MATERIAL collision_265_materials[1] = {
#include "stg14/terrain/collision_265_materials.inc"
};
static Sint16 collision_265_faces[10] = {
#include "stg14/terrain/collision_265_faces.inc"
};
static NJS_MESHSET collision_265_meshsets[1] = {
#include "stg14/terrain/collision_265_meshsets.inc"
};
static NJS_POINT3 collision_265_points[8] = {
#include "stg14/terrain/collision_265_points.inc"
};
static NJS_MODEL collision_265_model = {
#include "stg14/terrain/collision_265_model.inc"
};
static NJS_OBJECT collision_265 = {
#include "stg14/terrain/collision_265.inc"
};
static NJS_MATERIAL collision_267_materials[1] = {
#include "stg14/terrain/collision_267_materials.inc"
};
static Sint16 collision_267_faces[61] = {
#include "stg14/terrain/collision_267_faces.inc"
};
static NJS_MESHSET collision_267_meshsets[1] = {
#include "stg14/terrain/collision_267_meshsets.inc"
};
static NJS_POINT3 collision_267_points[60] = {
#include "stg14/terrain/collision_267_points.inc"
};
static NJS_MODEL collision_267_model = {
#include "stg14/terrain/collision_267_model.inc"
};
static NJS_OBJECT collision_267 = {
#include "stg14/terrain/collision_267.inc"
};
static NJS_MATERIAL collision_268_materials[1] = {
#include "stg14/terrain/collision_268_materials.inc"
};
static Sint16 collision_268_faces[21] = {
#include "stg14/terrain/collision_268_faces.inc"
};
static NJS_MESHSET collision_268_meshsets[1] = {
#include "stg14/terrain/collision_268_meshsets.inc"
};
static NJS_POINT3 collision_268_points[18] = {
#include "stg14/terrain/collision_268_points.inc"
};
static NJS_MODEL collision_268_model = {
#include "stg14/terrain/collision_268_model.inc"
};
static NJS_OBJECT collision_268 = {
#include "stg14/terrain/collision_268.inc"
};
static NJS_MATERIAL collision_271_materials[1] = {
#include "stg14/terrain/collision_271_materials.inc"
};
static Sint16 collision_271_faces[28] = {
#include "stg14/terrain/collision_271_faces.inc"
};
static NJS_MESHSET collision_271_meshsets[1] = {
#include "stg14/terrain/collision_271_meshsets.inc"
};
static NJS_POINT3 collision_271_points[16] = {
#include "stg14/terrain/collision_271_points.inc"
};
static NJS_MODEL collision_271_model = {
#include "stg14/terrain/collision_271_model.inc"
};
static NJS_OBJECT collision_271 = {
#include "stg14/terrain/collision_271.inc"
};
static NJS_MATERIAL collision_272_materials[1] = {
#include "stg14/terrain/collision_272_materials.inc"
};
static Sint16 collision_272_faces[22] = {
#include "stg14/terrain/collision_272_faces.inc"
};
static NJS_MESHSET collision_272_meshsets[1] = {
#include "stg14/terrain/collision_272_meshsets.inc"
};
static NJS_POINT3 collision_272_points[12] = {
#include "stg14/terrain/collision_272_points.inc"
};
static NJS_MODEL collision_272_model = {
#include "stg14/terrain/collision_272_model.inc"
};
static NJS_OBJECT collision_272 = {
#include "stg14/terrain/collision_272.inc"
};
static NJS_MATERIAL collision_274_materials[1] = {
#include "stg14/terrain/collision_274_materials.inc"
};
static Sint16 collision_274_faces[35] = {
#include "stg14/terrain/collision_274_faces.inc"
};
static NJS_MESHSET collision_274_meshsets[1] = {
#include "stg14/terrain/collision_274_meshsets.inc"
};
static NJS_POINT3 collision_274_points[24] = {
#include "stg14/terrain/collision_274_points.inc"
};
static NJS_MODEL collision_274_model = {
#include "stg14/terrain/collision_274_model.inc"
};
static NJS_OBJECT collision_274 = {
#include "stg14/terrain/collision_274.inc"
};
static NJS_MATERIAL collision_275_materials[1] = {
#include "stg14/terrain/collision_275_materials.inc"
};
static Sint16 collision_275_faces[7] = {
#include "stg14/terrain/collision_275_faces.inc"
};
static NJS_MESHSET collision_275_meshsets[1] = {
#include "stg14/terrain/collision_275_meshsets.inc"
};
static NJS_POINT3 collision_275_points[6] = {
#include "stg14/terrain/collision_275_points.inc"
};
static NJS_MODEL collision_275_model = {
#include "stg14/terrain/collision_275_model.inc"
};
static NJS_OBJECT collision_275 = {
#include "stg14/terrain/collision_275.inc"
};
static NJS_MATERIAL collision_276_materials[1] = {
#include "stg14/terrain/collision_276_materials.inc"
};
static Sint16 collision_276_faces[21] = {
#include "stg14/terrain/collision_276_faces.inc"
};
static NJS_MESHSET collision_276_meshsets[1] = {
#include "stg14/terrain/collision_276_meshsets.inc"
};
static NJS_POINT3 collision_276_points[18] = {
#include "stg14/terrain/collision_276_points.inc"
};
static NJS_MODEL collision_276_model = {
#include "stg14/terrain/collision_276_model.inc"
};
static NJS_OBJECT collision_276 = {
#include "stg14/terrain/collision_276.inc"
};
static NJS_MATERIAL collision_277_materials[1] = {
#include "stg14/terrain/collision_277_materials.inc"
};
static Sint16 collision_277_faces[7] = {
#include "stg14/terrain/collision_277_faces.inc"
};
static NJS_MESHSET collision_277_meshsets[1] = {
#include "stg14/terrain/collision_277_meshsets.inc"
};
static NJS_POINT3 collision_277_points[6] = {
#include "stg14/terrain/collision_277_points.inc"
};
static NJS_MODEL collision_277_model = {
#include "stg14/terrain/collision_277_model.inc"
};
static NJS_OBJECT collision_277 = {
#include "stg14/terrain/collision_277.inc"
};
static NJS_MATERIAL collision_278_materials[1] = {
#include "stg14/terrain/collision_278_materials.inc"
};
static Sint16 collision_278_faces[28] = {
#include "stg14/terrain/collision_278_faces.inc"
};
static NJS_MESHSET collision_278_meshsets[1] = {
#include "stg14/terrain/collision_278_meshsets.inc"
};
static NJS_POINT3 collision_278_points[21] = {
#include "stg14/terrain/collision_278_points.inc"
};
static NJS_MODEL collision_278_model = {
#include "stg14/terrain/collision_278_model.inc"
};
static NJS_OBJECT collision_278 = {
#include "stg14/terrain/collision_278.inc"
};
static NJS_MATERIAL collision_280_materials[1] = {
#include "stg14/terrain/collision_280_materials.inc"
};
static Sint16 collision_280_faces[184] = {
#include "stg14/terrain/collision_280_faces.inc"
};
static NJS_MESHSET collision_280_meshsets[1] = {
#include "stg14/terrain/collision_280_meshsets.inc"
};
static NJS_POINT3 collision_280_points[74] = {
#include "stg14/terrain/collision_280_points.inc"
};
static NJS_MODEL collision_280_model = {
#include "stg14/terrain/collision_280_model.inc"
};
static NJS_OBJECT collision_280 = {
#include "stg14/terrain/collision_280.inc"
};
static NJS_MATERIAL collision_281_materials[1] = {
#include "stg14/terrain/collision_281_materials.inc"
};
static Sint16 collision_281_faces[294] = {
#include "stg14/terrain/collision_281_faces.inc"
};
static NJS_MESHSET collision_281_meshsets[1] = {
#include "stg14/terrain/collision_281_meshsets.inc"
};
static NJS_POINT3 collision_281_points[152] = {
#include "stg14/terrain/collision_281_points.inc"
};
static NJS_MODEL collision_281_model = {
#include "stg14/terrain/collision_281_model.inc"
};
static NJS_OBJECT collision_281 = {
#include "stg14/terrain/collision_281.inc"
};
static NJS_MATERIAL collision_282_materials[1] = {
#include "stg14/terrain/collision_282_materials.inc"
};
static Sint16 collision_282_faces[25] = {
#include "stg14/terrain/collision_282_faces.inc"
};
static NJS_MESHSET collision_282_meshsets[1] = {
#include "stg14/terrain/collision_282_meshsets.inc"
};
static NJS_POINT3 collision_282_points[15] = {
#include "stg14/terrain/collision_282_points.inc"
};
static NJS_MODEL collision_282_model = {
#include "stg14/terrain/collision_282_model.inc"
};
static NJS_OBJECT collision_282 = {
#include "stg14/terrain/collision_282.inc"
};
static NJS_MATERIAL collision_283_materials[1] = {
#include "stg14/terrain/collision_283_materials.inc"
};
static Sint16 collision_283_faces[27] = {
#include "stg14/terrain/collision_283_faces.inc"
};
static NJS_MESHSET collision_283_meshsets[1] = {
#include "stg14/terrain/collision_283_meshsets.inc"
};
static NJS_POINT3 collision_283_points[24] = {
#include "stg14/terrain/collision_283_points.inc"
};
static NJS_MODEL collision_283_model = {
#include "stg14/terrain/collision_283_model.inc"
};
static NJS_OBJECT collision_283 = {
#include "stg14/terrain/collision_283.inc"
};
static NJS_MATERIAL collision_286_materials[1] = {
#include "stg14/terrain/collision_286_materials.inc"
};
static Sint16 collision_286_faces[9] = {
#include "stg14/terrain/collision_286_faces.inc"
};
static NJS_MESHSET collision_286_meshsets[1] = {
#include "stg14/terrain/collision_286_meshsets.inc"
};
static NJS_POINT3 collision_286_points[8] = {
#include "stg14/terrain/collision_286_points.inc"
};
static NJS_MODEL collision_286_model = {
#include "stg14/terrain/collision_286_model.inc"
};
static NJS_OBJECT collision_286 = {
#include "stg14/terrain/collision_286.inc"
};
static NJS_MATERIAL collision_287_materials[1] = {
#include "stg14/terrain/collision_287_materials.inc"
};
static Sint16 collision_287_faces[17] = {
#include "stg14/terrain/collision_287_faces.inc"
};
static NJS_MESHSET collision_287_meshsets[1] = {
#include "stg14/terrain/collision_287_meshsets.inc"
};
static NJS_POINT3 collision_287_points[14] = {
#include "stg14/terrain/collision_287_points.inc"
};
static NJS_MODEL collision_287_model = {
#include "stg14/terrain/collision_287_model.inc"
};
static NJS_OBJECT collision_287 = {
#include "stg14/terrain/collision_287.inc"
};
static NJS_MATERIAL collision_288_materials[1] = {
#include "stg14/terrain/collision_288_materials.inc"
};
static Sint16 collision_288_faces[11] = {
#include "stg14/terrain/collision_288_faces.inc"
};
static NJS_MESHSET collision_288_meshsets[1] = {
#include "stg14/terrain/collision_288_meshsets.inc"
};
static NJS_POINT3 collision_288_points[8] = {
#include "stg14/terrain/collision_288_points.inc"
};
static NJS_MODEL collision_288_model = {
#include "stg14/terrain/collision_288_model.inc"
};
static NJS_OBJECT collision_288 = {
#include "stg14/terrain/collision_288.inc"
};
static NJS_MATERIAL collision_289_materials[1] = {
#include "stg14/terrain/collision_289_materials.inc"
};
static Sint16 collision_289_faces[304] = {
#include "stg14/terrain/collision_289_faces.inc"
};
static NJS_MESHSET collision_289_meshsets[1] = {
#include "stg14/terrain/collision_289_meshsets.inc"
};
static NJS_POINT3 collision_289_points[116] = {
#include "stg14/terrain/collision_289_points.inc"
};
static NJS_MODEL collision_289_model = {
#include "stg14/terrain/collision_289_model.inc"
};
static NJS_OBJECT collision_289 = {
#include "stg14/terrain/collision_289.inc"
};
static NJS_MATERIAL collision_290_materials[1] = {
#include "stg14/terrain/collision_290_materials.inc"
};
static Sint16 collision_290_faces[19] = {
#include "stg14/terrain/collision_290_faces.inc"
};
static NJS_MESHSET collision_290_meshsets[1] = {
#include "stg14/terrain/collision_290_meshsets.inc"
};
static NJS_POINT3 collision_290_points[18] = {
#include "stg14/terrain/collision_290_points.inc"
};
static NJS_MODEL collision_290_model = {
#include "stg14/terrain/collision_290_model.inc"
};
static NJS_OBJECT collision_290 = {
#include "stg14/terrain/collision_290.inc"
};
static NJS_MATERIAL collision_291_materials[1] = {
#include "stg14/terrain/collision_291_materials.inc"
};
static Sint16 collision_291_faces[7] = {
#include "stg14/terrain/collision_291_faces.inc"
};
static NJS_MESHSET collision_291_meshsets[1] = {
#include "stg14/terrain/collision_291_meshsets.inc"
};
static NJS_POINT3 collision_291_points[6] = {
#include "stg14/terrain/collision_291_points.inc"
};
static NJS_MODEL collision_291_model = {
#include "stg14/terrain/collision_291_model.inc"
};
static NJS_OBJECT collision_291 = {
#include "stg14/terrain/collision_291.inc"
};
static NJS_MATERIAL collision_292_materials[1] = {
#include "stg14/terrain/collision_292_materials.inc"
};
static Sint16 collision_292_faces[5] = {
#include "stg14/terrain/collision_292_faces.inc"
};
static NJS_MESHSET collision_292_meshsets[1] = {
#include "stg14/terrain/collision_292_meshsets.inc"
};
static NJS_POINT3 collision_292_points[4] = {
#include "stg14/terrain/collision_292_points.inc"
};
static NJS_MODEL collision_292_model = {
#include "stg14/terrain/collision_292_model.inc"
};
static NJS_OBJECT collision_292 = {
#include "stg14/terrain/collision_292.inc"
};
static NJS_MATERIAL collision_294_materials[1] = {
#include "stg14/terrain/collision_294_materials.inc"
};
static Sint16 collision_294_faces[22] = {
#include "stg14/terrain/collision_294_faces.inc"
};
static NJS_MESHSET collision_294_meshsets[1] = {
#include "stg14/terrain/collision_294_meshsets.inc"
};
static NJS_POINT3 collision_294_points[16] = {
#include "stg14/terrain/collision_294_points.inc"
};
static NJS_MODEL collision_294_model = {
#include "stg14/terrain/collision_294_model.inc"
};
static NJS_OBJECT collision_294 = {
#include "stg14/terrain/collision_294.inc"
};
static NJS_MATERIAL collision_295_materials[1] = {
#include "stg14/terrain/collision_295_materials.inc"
};
static Sint16 collision_295_faces[45] = {
#include "stg14/terrain/collision_295_faces.inc"
};
static NJS_MESHSET collision_295_meshsets[1] = {
#include "stg14/terrain/collision_295_meshsets.inc"
};
static NJS_POINT3 collision_295_points[42] = {
#include "stg14/terrain/collision_295_points.inc"
};
static NJS_MODEL collision_295_model = {
#include "stg14/terrain/collision_295_model.inc"
};
static NJS_OBJECT collision_295 = {
#include "stg14/terrain/collision_295.inc"
};
static NJS_MATERIAL collision_299_materials[1] = {
#include "stg14/terrain/collision_299_materials.inc"
};
static Sint16 collision_299_faces[57] = {
#include "stg14/terrain/collision_299_faces.inc"
};
static NJS_MESHSET collision_299_meshsets[1] = {
#include "stg14/terrain/collision_299_meshsets.inc"
};
static NJS_POINT3 collision_299_points[54] = {
#include "stg14/terrain/collision_299_points.inc"
};
static NJS_MODEL collision_299_model = {
#include "stg14/terrain/collision_299_model.inc"
};
static NJS_OBJECT collision_299 = {
#include "stg14/terrain/collision_299.inc"
};
static NJS_MATERIAL collision_300_materials[1] = {
#include "stg14/terrain/collision_300_materials.inc"
};
static Sint16 collision_300_faces[20] = {
#include "stg14/terrain/collision_300_faces.inc"
};
static NJS_MESHSET collision_300_meshsets[1] = {
#include "stg14/terrain/collision_300_meshsets.inc"
};
static NJS_POINT3 collision_300_points[16] = {
#include "stg14/terrain/collision_300_points.inc"
};
static NJS_MODEL collision_300_model = {
#include "stg14/terrain/collision_300_model.inc"
};
static NJS_OBJECT collision_300 = {
#include "stg14/terrain/collision_300.inc"
};
static NJS_MATERIAL collision_301_materials[1] = {
#include "stg14/terrain/collision_301_materials.inc"
};
static Sint16 collision_301_faces[20] = {
#include "stg14/terrain/collision_301_faces.inc"
};
static NJS_MESHSET collision_301_meshsets[1] = {
#include "stg14/terrain/collision_301_meshsets.inc"
};
static NJS_POINT3 collision_301_points[16] = {
#include "stg14/terrain/collision_301_points.inc"
};
static NJS_MODEL collision_301_model = {
#include "stg14/terrain/collision_301_model.inc"
};
static NJS_OBJECT collision_301 = {
#include "stg14/terrain/collision_301.inc"
};
static NJS_MATERIAL collision_302_materials[1] = {
#include "stg14/terrain/collision_302_materials.inc"
};
static Sint16 collision_302_faces[33] = {
#include "stg14/terrain/collision_302_faces.inc"
};
static NJS_MESHSET collision_302_meshsets[1] = {
#include "stg14/terrain/collision_302_meshsets.inc"
};
static NJS_POINT3 collision_302_points[26] = {
#include "stg14/terrain/collision_302_points.inc"
};
static NJS_MODEL collision_302_model = {
#include "stg14/terrain/collision_302_model.inc"
};
static NJS_OBJECT collision_302 = {
#include "stg14/terrain/collision_302.inc"
};
static NJS_MATERIAL collision_303_materials[1] = {
#include "stg14/terrain/collision_303_materials.inc"
};
static Sint16 collision_303_faces[33] = {
#include "stg14/terrain/collision_303_faces.inc"
};
static NJS_MESHSET collision_303_meshsets[1] = {
#include "stg14/terrain/collision_303_meshsets.inc"
};
static NJS_POINT3 collision_303_points[26] = {
#include "stg14/terrain/collision_303_points.inc"
};
static NJS_MODEL collision_303_model = {
#include "stg14/terrain/collision_303_model.inc"
};
static NJS_OBJECT collision_303 = {
#include "stg14/terrain/collision_303.inc"
};
static NJS_MATERIAL collision_304_materials[1] = {
#include "stg14/terrain/collision_304_materials.inc"
};
static Sint16 collision_304_faces[33] = {
#include "stg14/terrain/collision_304_faces.inc"
};
static NJS_MESHSET collision_304_meshsets[1] = {
#include "stg14/terrain/collision_304_meshsets.inc"
};
static NJS_POINT3 collision_304_points[26] = {
#include "stg14/terrain/collision_304_points.inc"
};
static NJS_MODEL collision_304_model = {
#include "stg14/terrain/collision_304_model.inc"
};
static NJS_OBJECT collision_304 = {
#include "stg14/terrain/collision_304.inc"
};
static NJS_MATERIAL collision_305_materials[1] = {
#include "stg14/terrain/collision_305_materials.inc"
};
static Sint16 collision_305_faces[33] = {
#include "stg14/terrain/collision_305_faces.inc"
};
static NJS_MESHSET collision_305_meshsets[1] = {
#include "stg14/terrain/collision_305_meshsets.inc"
};
static NJS_POINT3 collision_305_points[26] = {
#include "stg14/terrain/collision_305_points.inc"
};
static NJS_MODEL collision_305_model = {
#include "stg14/terrain/collision_305_model.inc"
};
static NJS_OBJECT collision_305 = {
#include "stg14/terrain/collision_305.inc"
};
static NJS_MATERIAL collision_306_materials[1] = {
#include "stg14/terrain/collision_306_materials.inc"
};
static Sint16 collision_306_faces[33] = {
#include "stg14/terrain/collision_306_faces.inc"
};
static NJS_MESHSET collision_306_meshsets[1] = {
#include "stg14/terrain/collision_306_meshsets.inc"
};
static NJS_POINT3 collision_306_points[26] = {
#include "stg14/terrain/collision_306_points.inc"
};
static NJS_MODEL collision_306_model = {
#include "stg14/terrain/collision_306_model.inc"
};
static NJS_OBJECT collision_306 = {
#include "stg14/terrain/collision_306.inc"
};
static NJS_MATERIAL collision_307_materials[1] = {
#include "stg14/terrain/collision_307_materials.inc"
};
static Sint16 collision_307_faces[33] = {
#include "stg14/terrain/collision_307_faces.inc"
};
static NJS_MESHSET collision_307_meshsets[1] = {
#include "stg14/terrain/collision_307_meshsets.inc"
};
static NJS_POINT3 collision_307_points[26] = {
#include "stg14/terrain/collision_307_points.inc"
};
static NJS_MODEL collision_307_model = {
#include "stg14/terrain/collision_307_model.inc"
};
static NJS_OBJECT collision_307 = {
#include "stg14/terrain/collision_307.inc"
};
static NJS_MATERIAL collision_308_materials[1] = {
#include "stg14/terrain/collision_308_materials.inc"
};
static Sint16 collision_308_faces[22] = {
#include "stg14/terrain/collision_308_faces.inc"
};
static NJS_MESHSET collision_308_meshsets[1] = {
#include "stg14/terrain/collision_308_meshsets.inc"
};
static NJS_POINT3 collision_308_points[12] = {
#include "stg14/terrain/collision_308_points.inc"
};
static NJS_MODEL collision_308_model = {
#include "stg14/terrain/collision_308_model.inc"
};
static NJS_OBJECT collision_308 = {
#include "stg14/terrain/collision_308.inc"
};
static NJS_MATERIAL collision_309_materials[1] = {
#include "stg14/terrain/collision_309_materials.inc"
};
static Sint16 collision_309_faces[75] = {
#include "stg14/terrain/collision_309_faces.inc"
};
static NJS_MESHSET collision_309_meshsets[1] = {
#include "stg14/terrain/collision_309_meshsets.inc"
};
static NJS_POINT3 collision_309_points[74] = {
#include "stg14/terrain/collision_309_points.inc"
};
static NJS_MODEL collision_309_model = {
#include "stg14/terrain/collision_309_model.inc"
};
static NJS_OBJECT collision_309 = {
#include "stg14/terrain/collision_309.inc"
};
static NJS_MATERIAL collision_311_materials[1] = {
#include "stg14/terrain/collision_311_materials.inc"
};
static Sint16 collision_311_faces[29] = {
#include "stg14/terrain/collision_311_faces.inc"
};
static NJS_MESHSET collision_311_meshsets[1] = {
#include "stg14/terrain/collision_311_meshsets.inc"
};
static NJS_POINT3 collision_311_points[18] = {
#include "stg14/terrain/collision_311_points.inc"
};
static NJS_MODEL collision_311_model = {
#include "stg14/terrain/collision_311_model.inc"
};
static NJS_OBJECT collision_311 = {
#include "stg14/terrain/collision_311.inc"
};
static NJS_MATERIAL collision_315_materials[1] = {
#include "stg14/terrain/collision_315_materials.inc"
};
static Sint16 collision_315_faces[57] = {
#include "stg14/terrain/collision_315_faces.inc"
};
static NJS_MESHSET collision_315_meshsets[1] = {
#include "stg14/terrain/collision_315_meshsets.inc"
};
static NJS_POINT3 collision_315_points[54] = {
#include "stg14/terrain/collision_315_points.inc"
};
static NJS_MODEL collision_315_model = {
#include "stg14/terrain/collision_315_model.inc"
};
static NJS_OBJECT collision_315 = {
#include "stg14/terrain/collision_315.inc"
};
static NJS_MATERIAL collision_318_materials[1] = {
#include "stg14/terrain/collision_318_materials.inc"
};
static Sint16 collision_318_faces[30] = {
#include "stg14/terrain/collision_318_faces.inc"
};
static NJS_MESHSET collision_318_meshsets[1] = {
#include "stg14/terrain/collision_318_meshsets.inc"
};
static NJS_POINT3 collision_318_points[18] = {
#include "stg14/terrain/collision_318_points.inc"
};
static NJS_MODEL collision_318_model = {
#include "stg14/terrain/collision_318_model.inc"
};
static NJS_OBJECT collision_318 = {
#include "stg14/terrain/collision_318.inc"
};
static NJS_MATERIAL collision_320_materials[1] = {
#include "stg14/terrain/collision_320_materials.inc"
};
static Sint16 collision_320_faces[57] = {
#include "stg14/terrain/collision_320_faces.inc"
};
static NJS_MESHSET collision_320_meshsets[1] = {
#include "stg14/terrain/collision_320_meshsets.inc"
};
static NJS_POINT3 collision_320_points[54] = {
#include "stg14/terrain/collision_320_points.inc"
};
static NJS_MODEL collision_320_model = {
#include "stg14/terrain/collision_320_model.inc"
};
static NJS_OBJECT collision_320 = {
#include "stg14/terrain/collision_320.inc"
};
static NJS_MATERIAL collision_322_materials[1] = {
#include "stg14/terrain/collision_322_materials.inc"
};
static Sint16 collision_322_faces[33] = {
#include "stg14/terrain/collision_322_faces.inc"
};
static NJS_MESHSET collision_322_meshsets[1] = {
#include "stg14/terrain/collision_322_meshsets.inc"
};
static NJS_POINT3 collision_322_points[32] = {
#include "stg14/terrain/collision_322_points.inc"
};
static NJS_MODEL collision_322_model = {
#include "stg14/terrain/collision_322_model.inc"
};
static NJS_OBJECT collision_322 = {
#include "stg14/terrain/collision_322.inc"
};
static NJS_MATERIAL collision_324_materials[1] = {
#include "stg14/terrain/collision_324_materials.inc"
};
static Sint16 collision_324_faces[11] = {
#include "stg14/terrain/collision_324_faces.inc"
};
static NJS_MESHSET collision_324_meshsets[1] = {
#include "stg14/terrain/collision_324_meshsets.inc"
};
static NJS_POINT3 collision_324_points[8] = {
#include "stg14/terrain/collision_324_points.inc"
};
static NJS_MODEL collision_324_model = {
#include "stg14/terrain/collision_324_model.inc"
};
static NJS_OBJECT collision_324 = {
#include "stg14/terrain/collision_324.inc"
};
static NJS_MATERIAL collision_326_materials[1] = {
#include "stg14/terrain/collision_326_materials.inc"
};
static Sint16 collision_326_faces[63] = {
#include "stg14/terrain/collision_326_faces.inc"
};
static NJS_MESHSET collision_326_meshsets[1] = {
#include "stg14/terrain/collision_326_meshsets.inc"
};
static NJS_POINT3 collision_326_points[32] = {
#include "stg14/terrain/collision_326_points.inc"
};
static NJS_MODEL collision_326_model = {
#include "stg14/terrain/collision_326_model.inc"
};
static NJS_OBJECT collision_326 = {
#include "stg14/terrain/collision_326.inc"
};
static NJS_MATERIAL collision_327_materials[1] = {
#include "stg14/terrain/collision_327_materials.inc"
};
static Sint16 collision_327_faces[91] = {
#include "stg14/terrain/collision_327_faces.inc"
};
static NJS_MESHSET collision_327_meshsets[1] = {
#include "stg14/terrain/collision_327_meshsets.inc"
};
static NJS_POINT3 collision_327_points[40] = {
#include "stg14/terrain/collision_327_points.inc"
};
static NJS_MODEL collision_327_model = {
#include "stg14/terrain/collision_327_model.inc"
};
static NJS_OBJECT collision_327 = {
#include "stg14/terrain/collision_327.inc"
};
static NJS_MATERIAL collision_328_materials[1] = {
#include "stg14/terrain/collision_328_materials.inc"
};
static Sint16 collision_328_faces[55] = {
#include "stg14/terrain/collision_328_faces.inc"
};
static NJS_MESHSET collision_328_meshsets[1] = {
#include "stg14/terrain/collision_328_meshsets.inc"
};
static NJS_POINT3 collision_328_points[30] = {
#include "stg14/terrain/collision_328_points.inc"
};
static NJS_MODEL collision_328_model = {
#include "stg14/terrain/collision_328_model.inc"
};
static NJS_OBJECT collision_328 = {
#include "stg14/terrain/collision_328.inc"
};
static NJS_MATERIAL collision_330_materials[1] = {
#include "stg14/terrain/collision_330_materials.inc"
};
static Sint16 collision_330_faces[16] = {
#include "stg14/terrain/collision_330_faces.inc"
};
static NJS_MESHSET collision_330_meshsets[1] = {
#include "stg14/terrain/collision_330_meshsets.inc"
};
static NJS_POINT3 collision_330_points[8] = {
#include "stg14/terrain/collision_330_points.inc"
};
static NJS_MODEL collision_330_model = {
#include "stg14/terrain/collision_330_model.inc"
};
static NJS_OBJECT collision_330 = {
#include "stg14/terrain/collision_330.inc"
};
static NJS_MATERIAL collision_335_materials[1] = {
#include "stg14/terrain/collision_335_materials.inc"
};
static Sint16 collision_335_faces[7] = {
#include "stg14/terrain/collision_335_faces.inc"
};
static NJS_MESHSET collision_335_meshsets[1] = {
#include "stg14/terrain/collision_335_meshsets.inc"
};
static NJS_POINT3 collision_335_points[6] = {
#include "stg14/terrain/collision_335_points.inc"
};
static NJS_MODEL collision_335_model = {
#include "stg14/terrain/collision_335_model.inc"
};
static NJS_OBJECT collision_335 = {
#include "stg14/terrain/collision_335.inc"
};
static NJS_MATERIAL collision_336_materials[1] = {
#include "stg14/terrain/collision_336_materials.inc"
};
static Sint16 collision_336_faces[88] = {
#include "stg14/terrain/collision_336_faces.inc"
};
static NJS_MESHSET collision_336_meshsets[1] = {
#include "stg14/terrain/collision_336_meshsets.inc"
};
static NJS_POINT3 collision_336_points[40] = {
#include "stg14/terrain/collision_336_points.inc"
};
static NJS_MODEL collision_336_model = {
#include "stg14/terrain/collision_336_model.inc"
};
static NJS_OBJECT collision_336 = {
#include "stg14/terrain/collision_336.inc"
};
static NJS_MATERIAL collision_338_materials[1] = {
#include "stg14/terrain/collision_338_materials.inc"
};
static Sint16 collision_338_faces[55] = {
#include "stg14/terrain/collision_338_faces.inc"
};
static NJS_MESHSET collision_338_meshsets[1] = {
#include "stg14/terrain/collision_338_meshsets.inc"
};
static NJS_POINT3 collision_338_points[30] = {
#include "stg14/terrain/collision_338_points.inc"
};
static NJS_MODEL collision_338_model = {
#include "stg14/terrain/collision_338_model.inc"
};
static NJS_OBJECT collision_338 = {
#include "stg14/terrain/collision_338.inc"
};
static NJS_MATERIAL collision_340_materials[1] = {
#include "stg14/terrain/collision_340_materials.inc"
};
static Sint16 collision_340_faces[19] = {
#include "stg14/terrain/collision_340_faces.inc"
};
static NJS_MESHSET collision_340_meshsets[1] = {
#include "stg14/terrain/collision_340_meshsets.inc"
};
static NJS_POINT3 collision_340_points[8] = {
#include "stg14/terrain/collision_340_points.inc"
};
static NJS_MODEL collision_340_model = {
#include "stg14/terrain/collision_340_model.inc"
};
static NJS_OBJECT collision_340 = {
#include "stg14/terrain/collision_340.inc"
};
static NJS_MATERIAL collision_349_materials[1] = {
#include "stg14/terrain/collision_349_materials.inc"
};
static Sint16 collision_349_faces[33] = {
#include "stg14/terrain/collision_349_faces.inc"
};
static NJS_MESHSET collision_349_meshsets[1] = {
#include "stg14/terrain/collision_349_meshsets.inc"
};
static NJS_POINT3 collision_349_points[32] = {
#include "stg14/terrain/collision_349_points.inc"
};
static NJS_MODEL collision_349_model = {
#include "stg14/terrain/collision_349_model.inc"
};
static NJS_OBJECT collision_349 = {
#include "stg14/terrain/collision_349.inc"
};
static NJS_MATERIAL collision_351_materials[1] = {
#include "stg14/terrain/collision_351_materials.inc"
};
static Sint16 collision_351_faces[33] = {
#include "stg14/terrain/collision_351_faces.inc"
};
static NJS_MESHSET collision_351_meshsets[1] = {
#include "stg14/terrain/collision_351_meshsets.inc"
};
static NJS_POINT3 collision_351_points[26] = {
#include "stg14/terrain/collision_351_points.inc"
};
static NJS_MODEL collision_351_model = {
#include "stg14/terrain/collision_351_model.inc"
};
static NJS_OBJECT collision_351 = {
#include "stg14/terrain/collision_351.inc"
};
static NJS_MATERIAL collision_353_materials[1] = {
#include "stg14/terrain/collision_353_materials.inc"
};
static Sint16 collision_353_faces[33] = {
#include "stg14/terrain/collision_353_faces.inc"
};
static NJS_MESHSET collision_353_meshsets[1] = {
#include "stg14/terrain/collision_353_meshsets.inc"
};
static NJS_POINT3 collision_353_points[26] = {
#include "stg14/terrain/collision_353_points.inc"
};
static NJS_MODEL collision_353_model = {
#include "stg14/terrain/collision_353_model.inc"
};
static NJS_OBJECT collision_353 = {
#include "stg14/terrain/collision_353.inc"
};
static NJS_MATERIAL collision_354_materials[1] = {
#include "stg14/terrain/collision_354_materials.inc"
};
static Sint16 collision_354_faces[33] = {
#include "stg14/terrain/collision_354_faces.inc"
};
static NJS_MESHSET collision_354_meshsets[1] = {
#include "stg14/terrain/collision_354_meshsets.inc"
};
static NJS_POINT3 collision_354_points[26] = {
#include "stg14/terrain/collision_354_points.inc"
};
static NJS_MODEL collision_354_model = {
#include "stg14/terrain/collision_354_model.inc"
};
static NJS_OBJECT collision_354 = {
#include "stg14/terrain/collision_354.inc"
};
static NJS_MATERIAL collision_355_materials[1] = {
#include "stg14/terrain/collision_355_materials.inc"
};
static Sint16 collision_355_faces[33] = {
#include "stg14/terrain/collision_355_faces.inc"
};
static NJS_MESHSET collision_355_meshsets[1] = {
#include "stg14/terrain/collision_355_meshsets.inc"
};
static NJS_POINT3 collision_355_points[26] = {
#include "stg14/terrain/collision_355_points.inc"
};
static NJS_MODEL collision_355_model = {
#include "stg14/terrain/collision_355_model.inc"
};
static NJS_OBJECT collision_355 = {
#include "stg14/terrain/collision_355.inc"
};
static NJS_MATERIAL collision_356_materials[1] = {
#include "stg14/terrain/collision_356_materials.inc"
};
static Sint16 collision_356_faces[33] = {
#include "stg14/terrain/collision_356_faces.inc"
};
static NJS_MESHSET collision_356_meshsets[1] = {
#include "stg14/terrain/collision_356_meshsets.inc"
};
static NJS_POINT3 collision_356_points[26] = {
#include "stg14/terrain/collision_356_points.inc"
};
static NJS_MODEL collision_356_model = {
#include "stg14/terrain/collision_356_model.inc"
};
static NJS_OBJECT collision_356 = {
#include "stg14/terrain/collision_356.inc"
};
static NJS_MATERIAL collision_357_materials[1] = {
#include "stg14/terrain/collision_357_materials.inc"
};
static Sint16 collision_357_faces[33] = {
#include "stg14/terrain/collision_357_faces.inc"
};
static NJS_MESHSET collision_357_meshsets[1] = {
#include "stg14/terrain/collision_357_meshsets.inc"
};
static NJS_POINT3 collision_357_points[26] = {
#include "stg14/terrain/collision_357_points.inc"
};
static NJS_MODEL collision_357_model = {
#include "stg14/terrain/collision_357_model.inc"
};
static NJS_OBJECT collision_357 = {
#include "stg14/terrain/collision_357.inc"
};
static NJS_MATERIAL collision_358_materials[1] = {
#include "stg14/terrain/collision_358_materials.inc"
};
static Sint16 collision_358_faces[33] = {
#include "stg14/terrain/collision_358_faces.inc"
};
static NJS_MESHSET collision_358_meshsets[1] = {
#include "stg14/terrain/collision_358_meshsets.inc"
};
static NJS_POINT3 collision_358_points[26] = {
#include "stg14/terrain/collision_358_points.inc"
};
static NJS_MODEL collision_358_model = {
#include "stg14/terrain/collision_358_model.inc"
};
static NJS_OBJECT collision_358 = {
#include "stg14/terrain/collision_358.inc"
};
static NJS_MATERIAL collision_359_materials[1] = {
#include "stg14/terrain/collision_359_materials.inc"
};
static Sint16 collision_359_faces[20] = {
#include "stg14/terrain/collision_359_faces.inc"
};
static NJS_MESHSET collision_359_meshsets[1] = {
#include "stg14/terrain/collision_359_meshsets.inc"
};
static NJS_POINT3 collision_359_points[16] = {
#include "stg14/terrain/collision_359_points.inc"
};
static NJS_MODEL collision_359_model = {
#include "stg14/terrain/collision_359_model.inc"
};
static NJS_OBJECT collision_359 = {
#include "stg14/terrain/collision_359.inc"
};
static NJS_MATERIAL collision_360_materials[1] = {
#include "stg14/terrain/collision_360_materials.inc"
};
static Sint16 collision_360_faces[20] = {
#include "stg14/terrain/collision_360_faces.inc"
};
static NJS_MESHSET collision_360_meshsets[1] = {
#include "stg14/terrain/collision_360_meshsets.inc"
};
static NJS_POINT3 collision_360_points[16] = {
#include "stg14/terrain/collision_360_points.inc"
};
static NJS_MODEL collision_360_model = {
#include "stg14/terrain/collision_360_model.inc"
};
static NJS_OBJECT collision_360 = {
#include "stg14/terrain/collision_360.inc"
};
static NJS_MATERIAL collision_361_materials[1] = {
#include "stg14/terrain/collision_361_materials.inc"
};
static Sint16 collision_361_faces[27] = {
#include "stg14/terrain/collision_361_faces.inc"
};
static NJS_MESHSET collision_361_meshsets[1] = {
#include "stg14/terrain/collision_361_meshsets.inc"
};
static NJS_POINT3 collision_361_points[12] = {
#include "stg14/terrain/collision_361_points.inc"
};
static NJS_MODEL collision_361_model = {
#include "stg14/terrain/collision_361_model.inc"
};
static NJS_OBJECT collision_361 = {
#include "stg14/terrain/collision_361.inc"
};
static NJS_MATERIAL collision_362_materials[1] = {
#include "stg14/terrain/collision_362_materials.inc"
};
static Sint16 collision_362_faces[75] = {
#include "stg14/terrain/collision_362_faces.inc"
};
static NJS_MESHSET collision_362_meshsets[1] = {
#include "stg14/terrain/collision_362_meshsets.inc"
};
static NJS_POINT3 collision_362_points[74] = {
#include "stg14/terrain/collision_362_points.inc"
};
static NJS_MODEL collision_362_model = {
#include "stg14/terrain/collision_362_model.inc"
};
static NJS_OBJECT collision_362 = {
#include "stg14/terrain/collision_362.inc"
};
static NJS_MATERIAL collision_364_materials[1] = {
#include "stg14/terrain/collision_364_materials.inc"
};
static Sint16 collision_364_faces[28] = {
#include "stg14/terrain/collision_364_faces.inc"
};
static NJS_MESHSET collision_364_meshsets[1] = {
#include "stg14/terrain/collision_364_meshsets.inc"
};
static NJS_POINT3 collision_364_points[21] = {
#include "stg14/terrain/collision_364_points.inc"
};
static NJS_MODEL collision_364_model = {
#include "stg14/terrain/collision_364_model.inc"
};
static NJS_OBJECT collision_364 = {
#include "stg14/terrain/collision_364.inc"
};
static NJS_MATERIAL collision_370_materials[1] = {
#include "stg14/terrain/collision_370_materials.inc"
};
static Sint16 collision_370_faces[21] = {
#include "stg14/terrain/collision_370_faces.inc"
};
static NJS_MESHSET collision_370_meshsets[1] = {
#include "stg14/terrain/collision_370_meshsets.inc"
};
static NJS_POINT3 collision_370_points[12] = {
#include "stg14/terrain/collision_370_points.inc"
};
static NJS_MODEL collision_370_model = {
#include "stg14/terrain/collision_370_model.inc"
};
static NJS_OBJECT collision_370 = {
#include "stg14/terrain/collision_370.inc"
};
static NJS_MATERIAL collision_371_materials[1] = {
#include "stg14/terrain/collision_371_materials.inc"
};
static Sint16 collision_371_faces[17] = {
#include "stg14/terrain/collision_371_faces.inc"
};
static NJS_MESHSET collision_371_meshsets[1] = {
#include "stg14/terrain/collision_371_meshsets.inc"
};
static NJS_POINT3 collision_371_points[16] = {
#include "stg14/terrain/collision_371_points.inc"
};
static NJS_MODEL collision_371_model = {
#include "stg14/terrain/collision_371_model.inc"
};
static NJS_OBJECT collision_371 = {
#include "stg14/terrain/collision_371.inc"
};
static NJS_MATERIAL collision_375_materials[1] = {
#include "stg14/terrain/collision_375_materials.inc"
};
static Sint16 collision_375_faces[27] = {
#include "stg14/terrain/collision_375_faces.inc"
};
static NJS_MESHSET collision_375_meshsets[1] = {
#include "stg14/terrain/collision_375_meshsets.inc"
};
static NJS_POINT3 collision_375_points[24] = {
#include "stg14/terrain/collision_375_points.inc"
};
static NJS_MODEL collision_375_model = {
#include "stg14/terrain/collision_375_model.inc"
};
static NJS_OBJECT collision_375 = {
#include "stg14/terrain/collision_375.inc"
};
static NJS_MATERIAL collision_377_materials[1] = {
#include "stg14/terrain/collision_377_materials.inc"
};
static Sint16 collision_377_faces[33] = {
#include "stg14/terrain/collision_377_faces.inc"
};
static NJS_MESHSET collision_377_meshsets[1] = {
#include "stg14/terrain/collision_377_meshsets.inc"
};
static NJS_POINT3 collision_377_points[30] = {
#include "stg14/terrain/collision_377_points.inc"
};
static NJS_MODEL collision_377_model = {
#include "stg14/terrain/collision_377_model.inc"
};
static NJS_OBJECT collision_377 = {
#include "stg14/terrain/collision_377.inc"
};
static NJS_MATERIAL collision_380_materials[1] = {
#include "stg14/terrain/collision_380_materials.inc"
};
static Sint16 collision_380_faces[33] = {
#include "stg14/terrain/collision_380_faces.inc"
};
static NJS_MESHSET collision_380_meshsets[1] = {
#include "stg14/terrain/collision_380_meshsets.inc"
};
static NJS_POINT3 collision_380_points[30] = {
#include "stg14/terrain/collision_380_points.inc"
};
static NJS_MODEL collision_380_model = {
#include "stg14/terrain/collision_380_model.inc"
};
static NJS_OBJECT collision_380 = {
#include "stg14/terrain/collision_380.inc"
};
static NJS_MATERIAL collision_383_materials[1] = {
#include "stg14/terrain/collision_383_materials.inc"
};
static Sint16 collision_383_faces[15] = {
#include "stg14/terrain/collision_383_faces.inc"
};
static NJS_MESHSET collision_383_meshsets[1] = {
#include "stg14/terrain/collision_383_meshsets.inc"
};
static NJS_POINT3 collision_383_points[12] = {
#include "stg14/terrain/collision_383_points.inc"
};
static NJS_MODEL collision_383_model = {
#include "stg14/terrain/collision_383_model.inc"
};
static NJS_OBJECT collision_383 = {
#include "stg14/terrain/collision_383.inc"
};
static NJS_MATERIAL collision_384_materials[1] = {
#include "stg14/terrain/collision_384_materials.inc"
};
static Sint16 collision_384_faces[15] = {
#include "stg14/terrain/collision_384_faces.inc"
};
static NJS_MESHSET collision_384_meshsets[1] = {
#include "stg14/terrain/collision_384_meshsets.inc"
};
static NJS_POINT3 collision_384_points[12] = {
#include "stg14/terrain/collision_384_points.inc"
};
static NJS_MODEL collision_384_model = {
#include "stg14/terrain/collision_384_model.inc"
};
static NJS_OBJECT collision_384 = {
#include "stg14/terrain/collision_384.inc"
};
static NJS_MATERIAL collision_385_materials[1] = {
#include "stg14/terrain/collision_385_materials.inc"
};
static Sint16 collision_385_faces[74] = {
#include "stg14/terrain/collision_385_faces.inc"
};
static NJS_MESHSET collision_385_meshsets[1] = {
#include "stg14/terrain/collision_385_meshsets.inc"
};
static NJS_POINT3 collision_385_points[48] = {
#include "stg14/terrain/collision_385_points.inc"
};
static NJS_MODEL collision_385_model = {
#include "stg14/terrain/collision_385_model.inc"
};
static NJS_OBJECT collision_385 = {
#include "stg14/terrain/collision_385.inc"
};
static NJS_MATERIAL collision_387_materials[1] = {
#include "stg14/terrain/collision_387_materials.inc"
};
static Sint16 collision_387_faces[51] = {
#include "stg14/terrain/collision_387_faces.inc"
};
static NJS_MESHSET collision_387_meshsets[1] = {
#include "stg14/terrain/collision_387_meshsets.inc"
};
static NJS_POINT3 collision_387_points[48] = {
#include "stg14/terrain/collision_387_points.inc"
};
static NJS_MODEL collision_387_model = {
#include "stg14/terrain/collision_387_model.inc"
};
static NJS_OBJECT collision_387 = {
#include "stg14/terrain/collision_387.inc"
};
static NJS_MATERIAL collision_389_materials[1] = {
#include "stg14/terrain/collision_389_materials.inc"
};
static Sint16 collision_389_faces[58] = {
#include "stg14/terrain/collision_389_faces.inc"
};
static NJS_MESHSET collision_389_meshsets[1] = {
#include "stg14/terrain/collision_389_meshsets.inc"
};
static NJS_POINT3 collision_389_points[24] = {
#include "stg14/terrain/collision_389_points.inc"
};
static NJS_MODEL collision_389_model = {
#include "stg14/terrain/collision_389_model.inc"
};
static NJS_OBJECT collision_389 = {
#include "stg14/terrain/collision_389.inc"
};
static NJS_MATERIAL collision_392_materials[1] = {
#include "stg14/terrain/collision_392_materials.inc"
};
static Sint16 collision_392_faces[10] = {
#include "stg14/terrain/collision_392_faces.inc"
};
static NJS_MESHSET collision_392_meshsets[1] = {
#include "stg14/terrain/collision_392_meshsets.inc"
};
static NJS_POINT3 collision_392_points[8] = {
#include "stg14/terrain/collision_392_points.inc"
};
static NJS_MODEL collision_392_model = {
#include "stg14/terrain/collision_392_model.inc"
};
static NJS_OBJECT collision_392 = {
#include "stg14/terrain/collision_392.inc"
};
static NJS_MATERIAL collision_398_materials[1] = {
#include "stg14/terrain/collision_398_materials.inc"
};
static Sint16 collision_398_faces[30] = {
#include "stg14/terrain/collision_398_faces.inc"
};
static NJS_MESHSET collision_398_meshsets[1] = {
#include "stg14/terrain/collision_398_meshsets.inc"
};
static NJS_POINT3 collision_398_points[20] = {
#include "stg14/terrain/collision_398_points.inc"
};
static NJS_MODEL collision_398_model = {
#include "stg14/terrain/collision_398_model.inc"
};
static NJS_OBJECT collision_398 = {
#include "stg14/terrain/collision_398.inc"
};
static NJS_MATERIAL collision_399_materials[1] = {
#include "stg14/terrain/collision_399_materials.inc"
};
static Sint16 collision_399_faces[10] = {
#include "stg14/terrain/collision_399_faces.inc"
};
static NJS_MESHSET collision_399_meshsets[1] = {
#include "stg14/terrain/collision_399_meshsets.inc"
};
static NJS_POINT3 collision_399_points[8] = {
#include "stg14/terrain/collision_399_points.inc"
};
static NJS_MODEL collision_399_model = {
#include "stg14/terrain/collision_399_model.inc"
};
static NJS_OBJECT collision_399 = {
#include "stg14/terrain/collision_399.inc"
};
static NJS_MATERIAL collision_401_materials[1] = {
#include "stg14/terrain/collision_401_materials.inc"
};
static Sint16 collision_401_faces[22] = {
#include "stg14/terrain/collision_401_faces.inc"
};
static NJS_MESHSET collision_401_meshsets[1] = {
#include "stg14/terrain/collision_401_meshsets.inc"
};
static NJS_POINT3 collision_401_points[12] = {
#include "stg14/terrain/collision_401_points.inc"
};
static NJS_MODEL collision_401_model = {
#include "stg14/terrain/collision_401_model.inc"
};
static NJS_OBJECT collision_401 = {
#include "stg14/terrain/collision_401.inc"
};
static NJS_MATERIAL collision_402_materials[1] = {
#include "stg14/terrain/collision_402_materials.inc"
};
static Sint16 collision_402_faces[44] = {
#include "stg14/terrain/collision_402_faces.inc"
};
static NJS_MESHSET collision_402_meshsets[1] = {
#include "stg14/terrain/collision_402_meshsets.inc"
};
static NJS_POINT3 collision_402_points[20] = {
#include "stg14/terrain/collision_402_points.inc"
};
static NJS_MODEL collision_402_model = {
#include "stg14/terrain/collision_402_model.inc"
};
static NJS_OBJECT collision_402 = {
#include "stg14/terrain/collision_402.inc"
};
static NJS_MATERIAL collision_404_materials[1] = {
#include "stg14/terrain/collision_404_materials.inc"
};
static Sint16 collision_404_faces[28] = {
#include "stg14/terrain/collision_404_faces.inc"
};
static NJS_MESHSET collision_404_meshsets[1] = {
#include "stg14/terrain/collision_404_meshsets.inc"
};
static NJS_POINT3 collision_404_points[15] = {
#include "stg14/terrain/collision_404_points.inc"
};
static NJS_MODEL collision_404_model = {
#include "stg14/terrain/collision_404_model.inc"
};
static NJS_OBJECT collision_404 = {
#include "stg14/terrain/collision_404.inc"
};
static NJS_MATERIAL collision_405_materials[1] = {
#include "stg14/terrain/collision_405_materials.inc"
};
static Sint16 collision_405_faces[22] = {
#include "stg14/terrain/collision_405_faces.inc"
};
static NJS_MESHSET collision_405_meshsets[1] = {
#include "stg14/terrain/collision_405_meshsets.inc"
};
static NJS_POINT3 collision_405_points[12] = {
#include "stg14/terrain/collision_405_points.inc"
};
static NJS_MODEL collision_405_model = {
#include "stg14/terrain/collision_405_model.inc"
};
static NJS_OBJECT collision_405 = {
#include "stg14/terrain/collision_405.inc"
};
static NJS_MATERIAL collision_406_materials[1] = {
#include "stg14/terrain/collision_406_materials.inc"
};
static Sint16 collision_406_faces[20] = {
#include "stg14/terrain/collision_406_faces.inc"
};
static NJS_MESHSET collision_406_meshsets[1] = {
#include "stg14/terrain/collision_406_meshsets.inc"
};
static NJS_POINT3 collision_406_points[10] = {
#include "stg14/terrain/collision_406_points.inc"
};
static NJS_MODEL collision_406_model = {
#include "stg14/terrain/collision_406_model.inc"
};
static NJS_OBJECT collision_406 = {
#include "stg14/terrain/collision_406.inc"
};
static NJS_MATERIAL collision_407_materials[1] = {
#include "stg14/terrain/collision_407_materials.inc"
};
static Sint16 collision_407_faces[32] = {
#include "stg14/terrain/collision_407_faces.inc"
};
static NJS_MESHSET collision_407_meshsets[1] = {
#include "stg14/terrain/collision_407_meshsets.inc"
};
static NJS_POINT3 collision_407_points[16] = {
#include "stg14/terrain/collision_407_points.inc"
};
static NJS_MODEL collision_407_model = {
#include "stg14/terrain/collision_407_model.inc"
};
static NJS_OBJECT collision_407 = {
#include "stg14/terrain/collision_407.inc"
};
static NJS_MATERIAL collision_408_materials[1] = {
#include "stg14/terrain/collision_408_materials.inc"
};
static Sint16 collision_408_faces[64] = {
#include "stg14/terrain/collision_408_faces.inc"
};
static NJS_MESHSET collision_408_meshsets[1] = {
#include "stg14/terrain/collision_408_meshsets.inc"
};
static NJS_POINT3 collision_408_points[29] = {
#include "stg14/terrain/collision_408_points.inc"
};
static NJS_MODEL collision_408_model = {
#include "stg14/terrain/collision_408_model.inc"
};
static NJS_OBJECT collision_408 = {
#include "stg14/terrain/collision_408.inc"
};
static NJS_MATERIAL collision_409_materials[1] = {
#include "stg14/terrain/collision_409_materials.inc"
};
static Sint16 collision_409_faces[41] = {
#include "stg14/terrain/collision_409_faces.inc"
};
static NJS_MESHSET collision_409_meshsets[1] = {
#include "stg14/terrain/collision_409_meshsets.inc"
};
static NJS_POINT3 collision_409_points[20] = {
#include "stg14/terrain/collision_409_points.inc"
};
static NJS_MODEL collision_409_model = {
#include "stg14/terrain/collision_409_model.inc"
};
static NJS_OBJECT collision_409 = {
#include "stg14/terrain/collision_409.inc"
};
static NJS_MATERIAL collision_410_materials[1] = {
#include "stg14/terrain/collision_410_materials.inc"
};
static Sint16 collision_410_faces[30] = {
#include "stg14/terrain/collision_410_faces.inc"
};
static NJS_MESHSET collision_410_meshsets[1] = {
#include "stg14/terrain/collision_410_meshsets.inc"
};
static NJS_POINT3 collision_410_points[16] = {
#include "stg14/terrain/collision_410_points.inc"
};
static NJS_MODEL collision_410_model = {
#include "stg14/terrain/collision_410_model.inc"
};
static NJS_OBJECT collision_410 = {
#include "stg14/terrain/collision_410.inc"
};
static NJS_MATERIAL collision_411_materials[1] = {
#include "stg14/terrain/collision_411_materials.inc"
};
static Sint16 collision_411_faces[21] = {
#include "stg14/terrain/collision_411_faces.inc"
};
static NJS_MESHSET collision_411_meshsets[1] = {
#include "stg14/terrain/collision_411_meshsets.inc"
};
static NJS_POINT3 collision_411_points[12] = {
#include "stg14/terrain/collision_411_points.inc"
};
static NJS_MODEL collision_411_model = {
#include "stg14/terrain/collision_411_model.inc"
};
static NJS_OBJECT collision_411 = {
#include "stg14/terrain/collision_411.inc"
};
static NJS_MATERIAL collision_412_materials[1] = {
#include "stg14/terrain/collision_412_materials.inc"
};
static Sint16 collision_412_faces[23] = {
#include "stg14/terrain/collision_412_faces.inc"
};
static NJS_MESHSET collision_412_meshsets[1] = {
#include "stg14/terrain/collision_412_meshsets.inc"
};
static NJS_POINT3 collision_412_points[12] = {
#include "stg14/terrain/collision_412_points.inc"
};
static NJS_MODEL collision_412_model = {
#include "stg14/terrain/collision_412_model.inc"
};
static NJS_OBJECT collision_412 = {
#include "stg14/terrain/collision_412.inc"
};
static NJS_MATERIAL collision_413_materials[1] = {
#include "stg14/terrain/collision_413_materials.inc"
};
static Sint16 collision_413_faces[20] = {
#include "stg14/terrain/collision_413_faces.inc"
};
static NJS_MESHSET collision_413_meshsets[1] = {
#include "stg14/terrain/collision_413_meshsets.inc"
};
static NJS_POINT3 collision_413_points[10] = {
#include "stg14/terrain/collision_413_points.inc"
};
static NJS_MODEL collision_413_model = {
#include "stg14/terrain/collision_413_model.inc"
};
static NJS_OBJECT collision_413 = {
#include "stg14/terrain/collision_413.inc"
};
static NJS_MATERIAL collision_414_materials[1] = {
#include "stg14/terrain/collision_414_materials.inc"
};
static Sint16 collision_414_faces[25] = {
#include "stg14/terrain/collision_414_faces.inc"
};
static NJS_MESHSET collision_414_meshsets[1] = {
#include "stg14/terrain/collision_414_meshsets.inc"
};
static NJS_POINT3 collision_414_points[13] = {
#include "stg14/terrain/collision_414_points.inc"
};
static NJS_MODEL collision_414_model = {
#include "stg14/terrain/collision_414_model.inc"
};
static NJS_OBJECT collision_414 = {
#include "stg14/terrain/collision_414.inc"
};
static NJS_MATERIAL collision_416_materials[1] = {
#include "stg14/terrain/collision_416_materials.inc"
};
static Sint16 collision_416_faces[22] = {
#include "stg14/terrain/collision_416_faces.inc"
};
static NJS_MESHSET collision_416_meshsets[1] = {
#include "stg14/terrain/collision_416_meshsets.inc"
};
static NJS_POINT3 collision_416_points[12] = {
#include "stg14/terrain/collision_416_points.inc"
};
static NJS_MODEL collision_416_model = {
#include "stg14/terrain/collision_416_model.inc"
};
static NJS_OBJECT collision_416 = {
#include "stg14/terrain/collision_416.inc"
};
static Sint16 draw_5_polygons[219] = {
#include "stg14/terrain/draw_5_polygons.inc"
};
static Sint32 draw_5_vertices[103] = {
#include "stg14/terrain/draw_5_vertices.inc"
};
static NJS_CNK_MODEL draw_5_model = {
#include "stg14/terrain/draw_5_model.inc"
};
static NJS_CNK_OBJECT draw_710 = {
#include "stg14/terrain/draw_710.inc"
};
static Sint16 draw_260_polygons[635] = {
#include "stg14/terrain/draw_260_polygons.inc"
};
static Sint32 draw_260_vertices[571] = {
#include "stg14/terrain/draw_260_vertices.inc"
};
static NJS_CNK_MODEL draw_260_model = {
#include "stg14/terrain/draw_260_model.inc"
};
static NJS_CNK_OBJECT draw_709 = {
#include "stg14/terrain/draw_709.inc"
};
static Sint16 draw_101_polygons[191] = {
#include "stg14/terrain/draw_101_polygons.inc"
};
static Sint32 draw_101_vertices[203] = {
#include "stg14/terrain/draw_101_vertices.inc"
};
static NJS_CNK_MODEL draw_101_model = {
#include "stg14/terrain/draw_101_model.inc"
};
static NJS_CNK_OBJECT draw_708 = {
#include "stg14/terrain/draw_708.inc"
};
static Sint16 draw_95_polygons[237] = {
#include "stg14/terrain/draw_95_polygons.inc"
};
static Sint32 draw_95_vertices[251] = {
#include "stg14/terrain/draw_95_vertices.inc"
};
static NJS_CNK_MODEL draw_95_model = {
#include "stg14/terrain/draw_95_model.inc"
};
static NJS_CNK_OBJECT draw_707 = {
#include "stg14/terrain/draw_707.inc"
};
static Sint16 draw_382_polygons[23] = {
#include "stg14/terrain/draw_382_polygons.inc"
};
static Sint32 draw_382_vertices[19] = {
#include "stg14/terrain/draw_382_vertices.inc"
};
static NJS_CNK_MODEL draw_382_model = {
#include "stg14/terrain/draw_382_model.inc"
};
static NJS_CNK_OBJECT draw_704 = {
#include "stg14/terrain/draw_704.inc"
};
static Sint16 draw_248_polygons[101] = {
#include "stg14/terrain/draw_248_polygons.inc"
};
static Sint32 draw_248_vertices[63] = {
#include "stg14/terrain/draw_248_vertices.inc"
};
static NJS_CNK_MODEL draw_248_model = {
#include "stg14/terrain/draw_248_model.inc"
};
static NJS_CNK_OBJECT draw_695 = {
#include "stg14/terrain/draw_695.inc"
};
static Sint16 draw_246_polygons[429] = {
#include "stg14/terrain/draw_246_polygons.inc"
};
static Sint32 draw_246_vertices[515] = {
#include "stg14/terrain/draw_246_vertices.inc"
};
static NJS_CNK_MODEL draw_246_model = {
#include "stg14/terrain/draw_246_model.inc"
};
static NJS_CNK_OBJECT draw_679 = {
#include "stg14/terrain/draw_679.inc"
};
static Sint16 draw_244_polygons[757] = {
#include "stg14/terrain/draw_244_polygons.inc"
};
static Sint32 draw_244_vertices[495] = {
#include "stg14/terrain/draw_244_vertices.inc"
};
static NJS_CNK_MODEL draw_244_model = {
#include "stg14/terrain/draw_244_model.inc"
};
static NJS_CNK_OBJECT draw_675 = {
#include "stg14/terrain/draw_675.inc"
};
static Sint16 draw_237_polygons[433] = {
#include "stg14/terrain/draw_237_polygons.inc"
};
static Sint32 draw_237_vertices[515] = {
#include "stg14/terrain/draw_237_vertices.inc"
};
static NJS_CNK_MODEL draw_237_model = {
#include "stg14/terrain/draw_237_model.inc"
};
static NJS_CNK_OBJECT draw_666 = {
#include "stg14/terrain/draw_666.inc"
};
static Sint16 draw_231_polygons[757] = {
#include "stg14/terrain/draw_231_polygons.inc"
};
static Sint32 draw_231_vertices[495] = {
#include "stg14/terrain/draw_231_vertices.inc"
};
static NJS_CNK_MODEL draw_231_model = {
#include "stg14/terrain/draw_231_model.inc"
};
static NJS_CNK_OBJECT draw_657 = {
#include "stg14/terrain/draw_657.inc"
};
static Sint16 draw_277_polygons[81] = {
#include "stg14/terrain/draw_277_polygons.inc"
};
static Sint32 draw_277_vertices[51] = {
#include "stg14/terrain/draw_277_vertices.inc"
};
static NJS_CNK_MODEL draw_277_model = {
#include "stg14/terrain/draw_277_model.inc"
};
static NJS_CNK_OBJECT draw_655 = {
#include "stg14/terrain/draw_655.inc"
};
static Sint16 draw_206_polygons[337] = {
#include "stg14/terrain/draw_206_polygons.inc"
};
static Sint32 draw_206_vertices[347] = {
#include "stg14/terrain/draw_206_vertices.inc"
};
static NJS_CNK_MODEL draw_206_model = {
#include "stg14/terrain/draw_206_model.inc"
};
static NJS_CNK_OBJECT draw_654 = {
#include "stg14/terrain/draw_654.inc"
};
static Sint16 draw_201_polygons[1889] = {
#include "stg14/terrain/draw_201_polygons.inc"
};
static Sint32 draw_201_vertices[1539] = {
#include "stg14/terrain/draw_201_vertices.inc"
};
static NJS_CNK_MODEL draw_201_model = {
#include "stg14/terrain/draw_201_model.inc"
};
static NJS_CNK_OBJECT draw_653 = {
#include "stg14/terrain/draw_653.inc"
};
static Sint16 draw_186_polygons[337] = {
#include "stg14/terrain/draw_186_polygons.inc"
};
static Sint32 draw_186_vertices[347] = {
#include "stg14/terrain/draw_186_vertices.inc"
};
static NJS_CNK_MODEL draw_186_model = {
#include "stg14/terrain/draw_186_model.inc"
};
static NJS_CNK_OBJECT draw_652 = {
#include "stg14/terrain/draw_652.inc"
};
static Sint16 draw_158_polygons[53] = {
#include "stg14/terrain/draw_158_polygons.inc"
};
static Sint32 draw_158_vertices[51] = {
#include "stg14/terrain/draw_158_vertices.inc"
};
static NJS_CNK_MODEL draw_158_model = {
#include "stg14/terrain/draw_158_model.inc"
};
static NJS_CNK_OBJECT draw_651 = {
#include "stg14/terrain/draw_651.inc"
};
static Sint16 draw_153_polygons[61] = {
#include "stg14/terrain/draw_153_polygons.inc"
};
static Sint32 draw_153_vertices[43] = {
#include "stg14/terrain/draw_153_vertices.inc"
};
static NJS_CNK_MODEL draw_153_model = {
#include "stg14/terrain/draw_153_model.inc"
};
static NJS_CNK_OBJECT draw_650 = {
#include "stg14/terrain/draw_650.inc"
};
static Sint16 draw_150_polygons[667] = {
#include "stg14/terrain/draw_150_polygons.inc"
};
static Sint32 draw_150_vertices[555] = {
#include "stg14/terrain/draw_150_vertices.inc"
};
static NJS_CNK_MODEL draw_150_model = {
#include "stg14/terrain/draw_150_model.inc"
};
static NJS_CNK_OBJECT draw_649 = {
#include "stg14/terrain/draw_649.inc"
};
static Sint16 draw_121_polygons[323] = {
#include "stg14/terrain/draw_121_polygons.inc"
};
static Sint32 draw_121_vertices[279] = {
#include "stg14/terrain/draw_121_vertices.inc"
};
static NJS_CNK_MODEL draw_121_model = {
#include "stg14/terrain/draw_121_model.inc"
};
static NJS_CNK_OBJECT draw_645 = {
#include "stg14/terrain/draw_645.inc"
};
static Sint16 draw_122_polygons[873] = {
#include "stg14/terrain/draw_122_polygons.inc"
};
static Sint32 draw_122_vertices[707] = {
#include "stg14/terrain/draw_122_vertices.inc"
};
static NJS_CNK_MODEL draw_122_model = {
#include "stg14/terrain/draw_122_model.inc"
};
static NJS_CNK_OBJECT draw_644 = {
#include "stg14/terrain/draw_644.inc"
};
static Sint16 draw_8_polygons[687] = {
#include "stg14/terrain/draw_8_polygons.inc"
};
static Sint32 draw_8_vertices[511] = {
#include "stg14/terrain/draw_8_vertices.inc"
};
static NJS_CNK_MODEL draw_8_model = {
#include "stg14/terrain/draw_8_model.inc"
};
static NJS_CNK_OBJECT draw_643 = {
#include "stg14/terrain/draw_643.inc"
};
static Sint16 draw_19_polygons[687] = {
#include "stg14/terrain/draw_19_polygons.inc"
};
static Sint32 draw_19_vertices[519] = {
#include "stg14/terrain/draw_19_vertices.inc"
};
static NJS_CNK_MODEL draw_19_model = {
#include "stg14/terrain/draw_19_model.inc"
};
static NJS_CNK_OBJECT draw_642 = {
#include "stg14/terrain/draw_642.inc"
};
static Sint16 draw_18_polygons[829] = {
#include "stg14/terrain/draw_18_polygons.inc"
};
static Sint32 draw_18_vertices[587] = {
#include "stg14/terrain/draw_18_vertices.inc"
};
static NJS_CNK_MODEL draw_18_model = {
#include "stg14/terrain/draw_18_model.inc"
};
static NJS_CNK_OBJECT draw_641 = {
#include "stg14/terrain/draw_641.inc"
};
static Sint16 draw_107_polygons[531] = {
#include "stg14/terrain/draw_107_polygons.inc"
};
static Sint32 draw_107_vertices[295] = {
#include "stg14/terrain/draw_107_vertices.inc"
};
static NJS_CNK_MODEL draw_107_model = {
#include "stg14/terrain/draw_107_model.inc"
};
static NJS_CNK_OBJECT draw_640 = {
#include "stg14/terrain/draw_640.inc"
};
static Sint16 draw_29_polygons[215] = {
#include "stg14/terrain/draw_29_polygons.inc"
};
static Sint32 draw_29_vertices[139] = {
#include "stg14/terrain/draw_29_vertices.inc"
};
static NJS_CNK_MODEL draw_29_model = {
#include "stg14/terrain/draw_29_model.inc"
};
static NJS_CNK_OBJECT draw_626 = {
#include "stg14/terrain/draw_626.inc"
};
static Sint16 draw_263_polygons[429] = {
#include "stg14/terrain/draw_263_polygons.inc"
};
static Sint32 draw_263_vertices[307] = {
#include "stg14/terrain/draw_263_vertices.inc"
};
static NJS_CNK_MODEL draw_263_model = {
#include "stg14/terrain/draw_263_model.inc"
};
static NJS_CNK_OBJECT draw_622 = {
#include "stg14/terrain/draw_622.inc"
};
static Sint16 draw_621_polygons[1037] = {
#include "stg14/terrain/draw_621_polygons.inc"
};
static Sint32 draw_621_vertices[707] = {
#include "stg14/terrain/draw_621_vertices.inc"
};
static NJS_CNK_MODEL draw_621_model = {
#include "stg14/terrain/draw_621_model.inc"
};
static NJS_CNK_OBJECT draw_621 = {
#include "stg14/terrain/draw_621.inc"
};
static Sint16 draw_261_polygons[149] = {
#include "stg14/terrain/draw_261_polygons.inc"
};
static Sint32 draw_261_vertices[115] = {
#include "stg14/terrain/draw_261_vertices.inc"
};
static NJS_CNK_MODEL draw_261_model = {
#include "stg14/terrain/draw_261_model.inc"
};
static NJS_CNK_OBJECT draw_620 = {
#include "stg14/terrain/draw_620.inc"
};
static Sint16 draw_57_polygons[215] = {
#include "stg14/terrain/draw_57_polygons.inc"
};
static Sint32 draw_57_vertices[139] = {
#include "stg14/terrain/draw_57_vertices.inc"
};
static NJS_CNK_MODEL draw_57_model = {
#include "stg14/terrain/draw_57_model.inc"
};
static NJS_CNK_OBJECT draw_617 = {
#include "stg14/terrain/draw_617.inc"
};
static Sint16 draw_22_polygons[89] = {
#include "stg14/terrain/draw_22_polygons.inc"
};
static Sint32 draw_22_vertices[107] = {
#include "stg14/terrain/draw_22_vertices.inc"
};
static NJS_CNK_MODEL draw_22_model = {
#include "stg14/terrain/draw_22_model.inc"
};
static NJS_CNK_OBJECT draw_608 = {
#include "stg14/terrain/draw_608.inc"
};
static Sint16 draw_40_polygons[697] = {
#include "stg14/terrain/draw_40_polygons.inc"
};
static Sint32 draw_40_vertices[427] = {
#include "stg14/terrain/draw_40_vertices.inc"
};
static NJS_CNK_MODEL draw_40_model = {
#include "stg14/terrain/draw_40_model.inc"
};
static NJS_CNK_OBJECT draw_607 = {
#include "stg14/terrain/draw_607.inc"
};
static Sint16 draw_23_polygons[89] = {
#include "stg14/terrain/draw_23_polygons.inc"
};
static Sint32 draw_23_vertices[107] = {
#include "stg14/terrain/draw_23_vertices.inc"
};
static NJS_CNK_MODEL draw_23_model = {
#include "stg14/terrain/draw_23_model.inc"
};
static NJS_CNK_OBJECT draw_604 = {
#include "stg14/terrain/draw_604.inc"
};
static Sint16 draw_33_polygons[695] = {
#include "stg14/terrain/draw_33_polygons.inc"
};
static Sint32 draw_33_vertices[423] = {
#include "stg14/terrain/draw_33_vertices.inc"
};
static NJS_CNK_MODEL draw_33_model = {
#include "stg14/terrain/draw_33_model.inc"
};
static NJS_CNK_OBJECT draw_603 = {
#include "stg14/terrain/draw_603.inc"
};
static Sint16 draw_21_polygons[59] = {
#include "stg14/terrain/draw_21_polygons.inc"
};
static Sint32 draw_21_vertices[67] = {
#include "stg14/terrain/draw_21_vertices.inc"
};
static NJS_CNK_MODEL draw_21_model = {
#include "stg14/terrain/draw_21_model.inc"
};
static NJS_CNK_OBJECT draw_600 = {
#include "stg14/terrain/draw_600.inc"
};
static Sint16 draw_20_polygons[59] = {
#include "stg14/terrain/draw_20_polygons.inc"
};
static Sint32 draw_20_vertices[67] = {
#include "stg14/terrain/draw_20_vertices.inc"
};
static NJS_CNK_MODEL draw_20_model = {
#include "stg14/terrain/draw_20_model.inc"
};
static NJS_CNK_OBJECT draw_599 = {
#include "stg14/terrain/draw_599.inc"
};
static Sint16 draw_2_polygons[455] = {
#include "stg14/terrain/draw_2_polygons.inc"
};
static Sint32 draw_2_vertices[351] = {
#include "stg14/terrain/draw_2_vertices.inc"
};
static NJS_CNK_MODEL draw_2_model = {
#include "stg14/terrain/draw_2_model.inc"
};
static NJS_CNK_OBJECT draw_598 = {
#include "stg14/terrain/draw_598.inc"
};
static Sint16 draw_38_polygons[877] = {
#include "stg14/terrain/draw_38_polygons.inc"
};
static Sint32 draw_38_vertices[587] = {
#include "stg14/terrain/draw_38_vertices.inc"
};
static NJS_CNK_MODEL draw_38_model = {
#include "stg14/terrain/draw_38_model.inc"
};
static NJS_CNK_OBJECT draw_597 = {
#include "stg14/terrain/draw_597.inc"
};
static Sint16 draw_37_polygons[861] = {
#include "stg14/terrain/draw_37_polygons.inc"
};
static Sint32 draw_37_vertices[519] = {
#include "stg14/terrain/draw_37_vertices.inc"
};
static NJS_CNK_MODEL draw_37_model = {
#include "stg14/terrain/draw_37_model.inc"
};
static NJS_CNK_OBJECT draw_596 = {
#include "stg14/terrain/draw_596.inc"
};
static Sint16 draw_24_polygons[29] = {
#include "stg14/terrain/draw_24_polygons.inc"
};
static Sint32 draw_24_vertices[27] = {
#include "stg14/terrain/draw_24_vertices.inc"
};
static NJS_CNK_MODEL draw_24_model = {
#include "stg14/terrain/draw_24_model.inc"
};
static NJS_CNK_OBJECT draw_595 = {
#include "stg14/terrain/draw_595.inc"
};
static Sint16 draw_4_polygons[93] = {
#include "stg14/terrain/draw_4_polygons.inc"
};
static Sint32 draw_4_vertices[67] = {
#include "stg14/terrain/draw_4_vertices.inc"
};
static NJS_CNK_MODEL draw_4_model = {
#include "stg14/terrain/draw_4_model.inc"
};
static NJS_CNK_OBJECT draw_594 = {
#include "stg14/terrain/draw_594.inc"
};
static Sint16 draw_10_polygons[437] = {
#include "stg14/terrain/draw_10_polygons.inc"
};
static Sint32 draw_10_vertices[355] = {
#include "stg14/terrain/draw_10_vertices.inc"
};
static NJS_CNK_MODEL draw_10_model = {
#include "stg14/terrain/draw_10_model.inc"
};
static NJS_CNK_OBJECT draw_590 = {
#include "stg14/terrain/draw_590.inc"
};
static Sint16 draw_7_polygons[157] = {
#include "stg14/terrain/draw_7_polygons.inc"
};
static Sint32 draw_7_vertices[123] = {
#include "stg14/terrain/draw_7_vertices.inc"
};
static NJS_CNK_MODEL draw_7_model = {
#include "stg14/terrain/draw_7_model.inc"
};
static NJS_CNK_OBJECT draw_585 = {
#include "stg14/terrain/draw_585.inc"
};
static Sint16 draw_11_polygons[79] = {
#include "stg14/terrain/draw_11_polygons.inc"
};
static Sint32 draw_11_vertices[51] = {
#include "stg14/terrain/draw_11_vertices.inc"
};
static NJS_CNK_MODEL draw_11_model = {
#include "stg14/terrain/draw_11_model.inc"
};
static NJS_CNK_OBJECT draw_584 = {
#include "stg14/terrain/draw_584.inc"
};
static Sint16 draw_131_polygons[297] = {
#include "stg14/terrain/draw_131_polygons.inc"
};
static Sint32 draw_131_vertices[363] = {
#include "stg14/terrain/draw_131_vertices.inc"
};
static NJS_CNK_MODEL draw_131_model = {
#include "stg14/terrain/draw_131_model.inc"
};
static NJS_CNK_OBJECT draw_583 = {
#include "stg14/terrain/draw_583.inc"
};
static Sint16 draw_1_polygons[125] = {
#include "stg14/terrain/draw_1_polygons.inc"
};
static Sint32 draw_1_vertices[75] = {
#include "stg14/terrain/draw_1_vertices.inc"
};
static NJS_CNK_MODEL draw_1_model = {
#include "stg14/terrain/draw_1_model.inc"
};
static NJS_CNK_OBJECT draw_557 = {
#include "stg14/terrain/draw_557.inc"
};
static Sint16 draw_363_polygons[157] = {
#include "stg14/terrain/draw_363_polygons.inc"
};
static Sint32 draw_363_vertices[107] = {
#include "stg14/terrain/draw_363_vertices.inc"
};
static NJS_CNK_MODEL draw_363_model = {
#include "stg14/terrain/draw_363_model.inc"
};
static NJS_CNK_OBJECT draw_554 = {
#include "stg14/terrain/draw_554.inc"
};
static Sint16 draw_13_polygons[161] = {
#include "stg14/terrain/draw_13_polygons.inc"
};
static Sint32 draw_13_vertices[95] = {
#include "stg14/terrain/draw_13_vertices.inc"
};
static NJS_CNK_MODEL draw_13_model = {
#include "stg14/terrain/draw_13_model.inc"
};
static NJS_CNK_OBJECT draw_543 = {
#include "stg14/terrain/draw_543.inc"
};
static Sint16 draw_542_polygons[407] = {
#include "stg14/terrain/draw_542_polygons.inc"
};
static Sint32 draw_542_vertices[259] = {
#include "stg14/terrain/draw_542_vertices.inc"
};
static NJS_CNK_MODEL draw_542_model = {
#include "stg14/terrain/draw_542_model.inc"
};
static NJS_CNK_OBJECT draw_542 = {
#include "stg14/terrain/draw_542.inc"
};
static Sint16 draw_369_polygons[713] = {
#include "stg14/terrain/draw_369_polygons.inc"
};
static Sint32 draw_369_vertices[531] = {
#include "stg14/terrain/draw_369_vertices.inc"
};
static NJS_CNK_MODEL draw_369_model = {
#include "stg14/terrain/draw_369_model.inc"
};
static NJS_CNK_OBJECT draw_540 = {
#include "stg14/terrain/draw_540.inc"
};
static Sint16 draw_14_polygons[261] = {
#include "stg14/terrain/draw_14_polygons.inc"
};
static Sint32 draw_14_vertices[147] = {
#include "stg14/terrain/draw_14_vertices.inc"
};
static NJS_CNK_MODEL draw_14_model = {
#include "stg14/terrain/draw_14_model.inc"
};
static NJS_CNK_OBJECT draw_539 = {
#include "stg14/terrain/draw_539.inc"
};
static Sint16 draw_0_polygons[1145] = {
#include "stg14/terrain/draw_0_polygons.inc"
};
static Sint32 draw_0_vertices[1055] = {
#include "stg14/terrain/draw_0_vertices.inc"
};
static NJS_CNK_MODEL draw_0_model = {
#include "stg14/terrain/draw_0_model.inc"
};
static NJS_CNK_OBJECT draw_538 = {
#include "stg14/terrain/draw_538.inc"
};
static Sint16 draw_533_polygons[713] = {
#include "stg14/terrain/draw_533_polygons.inc"
};
static Sint32 draw_533_vertices[531] = {
#include "stg14/terrain/draw_533_vertices.inc"
};
static NJS_CNK_MODEL draw_533_model = {
#include "stg14/terrain/draw_533_model.inc"
};
static NJS_CNK_OBJECT draw_533 = {
#include "stg14/terrain/draw_533.inc"
};
static Sint16 draw_16_polygons[91] = {
#include "stg14/terrain/draw_16_polygons.inc"
};
static Sint32 draw_16_vertices[67] = {
#include "stg14/terrain/draw_16_vertices.inc"
};
static NJS_CNK_MODEL draw_16_model = {
#include "stg14/terrain/draw_16_model.inc"
};
static NJS_CNK_OBJECT draw_531 = {
#include "stg14/terrain/draw_531.inc"
};
static Sint16 draw_124_polygons[663] = {
#include "stg14/terrain/draw_124_polygons.inc"
};
static Sint32 draw_124_vertices[507] = {
#include "stg14/terrain/draw_124_vertices.inc"
};
static NJS_CNK_MODEL draw_124_model = {
#include "stg14/terrain/draw_124_model.inc"
};
static NJS_CNK_OBJECT draw_530 = {
#include "stg14/terrain/draw_530.inc"
};
static Sint16 draw_123_polygons[429] = {
#include "stg14/terrain/draw_123_polygons.inc"
};
static Sint32 draw_123_vertices[295] = {
#include "stg14/terrain/draw_123_vertices.inc"
};
static NJS_CNK_MODEL draw_123_model = {
#include "stg14/terrain/draw_123_model.inc"
};
static NJS_CNK_OBJECT draw_529 = {
#include "stg14/terrain/draw_529.inc"
};
static Sint16 draw_9_polygons[203] = {
#include "stg14/terrain/draw_9_polygons.inc"
};
static Sint32 draw_9_vertices[131] = {
#include "stg14/terrain/draw_9_vertices.inc"
};
static NJS_CNK_MODEL draw_9_model = {
#include "stg14/terrain/draw_9_model.inc"
};
static NJS_CNK_OBJECT draw_526 = {
#include "stg14/terrain/draw_526.inc"
};
static Sint16 draw_409_polygons[37] = {
#include "stg14/terrain/draw_409_polygons.inc"
};
static Sint32 draw_409_vertices[35] = {
#include "stg14/terrain/draw_409_vertices.inc"
};
static NJS_CNK_MODEL draw_409_model = {
#include "stg14/terrain/draw_409_model.inc"
};
static NJS_CNK_OBJECT draw_525 = {
#include "stg14/terrain/draw_525.inc"
};
static Sint16 draw_242_polygons[509] = {
#include "stg14/terrain/draw_242_polygons.inc"
};
static Sint32 draw_242_vertices[327] = {
#include "stg14/terrain/draw_242_vertices.inc"
};
static NJS_CNK_MODEL draw_242_model = {
#include "stg14/terrain/draw_242_model.inc"
};
static NJS_CNK_OBJECT draw_519 = {
#include "stg14/terrain/draw_519.inc"
};
static Sint16 draw_143_polygons[99] = {
#include "stg14/terrain/draw_143_polygons.inc"
};
static Sint32 draw_143_vertices[91] = {
#include "stg14/terrain/draw_143_vertices.inc"
};
static NJS_CNK_MODEL draw_143_model = {
#include "stg14/terrain/draw_143_model.inc"
};
static NJS_CNK_OBJECT draw_518 = {
#include "stg14/terrain/draw_518.inc"
};
static Sint16 draw_32_polygons[323] = {
#include "stg14/terrain/draw_32_polygons.inc"
};
static Sint32 draw_32_vertices[163] = {
#include "stg14/terrain/draw_32_vertices.inc"
};
static NJS_CNK_MODEL draw_32_model = {
#include "stg14/terrain/draw_32_model.inc"
};
static NJS_CNK_OBJECT draw_517 = {
#include "stg14/terrain/draw_517.inc"
};
static Sint16 draw_210_polygons[173] = {
#include "stg14/terrain/draw_210_polygons.inc"
};
static Sint32 draw_210_vertices[171] = {
#include "stg14/terrain/draw_210_vertices.inc"
};
static NJS_CNK_MODEL draw_210_model = {
#include "stg14/terrain/draw_210_model.inc"
};
static NJS_CNK_OBJECT draw_516 = {
#include "stg14/terrain/draw_516.inc"
};
static Sint16 draw_166_polygons[253] = {
#include "stg14/terrain/draw_166_polygons.inc"
};
static Sint32 draw_166_vertices[155] = {
#include "stg14/terrain/draw_166_vertices.inc"
};
static NJS_CNK_MODEL draw_166_model = {
#include "stg14/terrain/draw_166_model.inc"
};
static NJS_CNK_OBJECT draw_515 = {
#include "stg14/terrain/draw_515.inc"
};
static Sint16 draw_144_polygons[87] = {
#include "stg14/terrain/draw_144_polygons.inc"
};
static Sint32 draw_144_vertices[67] = {
#include "stg14/terrain/draw_144_vertices.inc"
};
static NJS_CNK_MODEL draw_144_model = {
#include "stg14/terrain/draw_144_model.inc"
};
static NJS_CNK_OBJECT draw_514 = {
#include "stg14/terrain/draw_514.inc"
};
static Sint16 draw_173_polygons[175] = {
#include "stg14/terrain/draw_173_polygons.inc"
};
static Sint32 draw_173_vertices[107] = {
#include "stg14/terrain/draw_173_vertices.inc"
};
static NJS_CNK_MODEL draw_173_model = {
#include "stg14/terrain/draw_173_model.inc"
};
static NJS_CNK_OBJECT draw_513 = {
#include "stg14/terrain/draw_513.inc"
};
static Sint16 draw_15_polygons[125] = {
#include "stg14/terrain/draw_15_polygons.inc"
};
static Sint32 draw_15_vertices[91] = {
#include "stg14/terrain/draw_15_vertices.inc"
};
static NJS_CNK_MODEL draw_15_model = {
#include "stg14/terrain/draw_15_model.inc"
};
static NJS_CNK_OBJECT draw_512 = {
#include "stg14/terrain/draw_512.inc"
};
static Sint16 draw_311_polygons[93] = {
#include "stg14/terrain/draw_311_polygons.inc"
};
static Sint32 draw_311_vertices[99] = {
#include "stg14/terrain/draw_311_vertices.inc"
};
static NJS_CNK_MODEL draw_311_model = {
#include "stg14/terrain/draw_311_model.inc"
};
static NJS_CNK_OBJECT draw_505 = {
#include "stg14/terrain/draw_505.inc"
};
static Sint16 draw_487_polygons[517] = {
#include "stg14/terrain/draw_487_polygons.inc"
};
static Sint32 draw_487_vertices[351] = {
#include "stg14/terrain/draw_487_vertices.inc"
};
static NJS_CNK_MODEL draw_487_model = {
#include "stg14/terrain/draw_487_model.inc"
};
static NJS_CNK_OBJECT draw_487 = {
#include "stg14/terrain/draw_487.inc"
};
static Sint16 draw_91_polygons[2209] = {
#include "stg14/terrain/draw_91_polygons.inc"
};
static Sint32 draw_91_vertices[1947] = {
#include "stg14/terrain/draw_91_vertices.inc"
};
static NJS_CNK_MODEL draw_91_model = {
#include "stg14/terrain/draw_91_model.inc"
};
static NJS_CNK_OBJECT draw_475 = {
#include "stg14/terrain/draw_475.inc"
};
static Sint16 draw_102_polygons[209] = {
#include "stg14/terrain/draw_102_polygons.inc"
};
static Sint32 draw_102_vertices[163] = {
#include "stg14/terrain/draw_102_vertices.inc"
};
static NJS_CNK_MODEL draw_102_model = {
#include "stg14/terrain/draw_102_model.inc"
};
static NJS_CNK_OBJECT draw_474 = {
#include "stg14/terrain/draw_474.inc"
};
static Sint16 draw_92_polygons[867] = {
#include "stg14/terrain/draw_92_polygons.inc"
};
static Sint32 draw_92_vertices[475] = {
#include "stg14/terrain/draw_92_vertices.inc"
};
static NJS_CNK_MODEL draw_92_model = {
#include "stg14/terrain/draw_92_model.inc"
};
static NJS_CNK_OBJECT draw_473 = {
#include "stg14/terrain/draw_473.inc"
};
static Sint16 draw_468_polygons[529] = {
#include "stg14/terrain/draw_468_polygons.inc"
};
static Sint32 draw_468_vertices[323] = {
#include "stg14/terrain/draw_468_vertices.inc"
};
static NJS_CNK_MODEL draw_468_model = {
#include "stg14/terrain/draw_468_model.inc"
};
static NJS_CNK_OBJECT draw_468 = {
#include "stg14/terrain/draw_468.inc"
};
static Sint16 draw_77_polygons[241] = {
#include "stg14/terrain/draw_77_polygons.inc"
};
static Sint32 draw_77_vertices[243] = {
#include "stg14/terrain/draw_77_vertices.inc"
};
static NJS_CNK_MODEL draw_77_model = {
#include "stg14/terrain/draw_77_model.inc"
};
static NJS_CNK_OBJECT draw_462 = {
#include "stg14/terrain/draw_462.inc"
};
static Sint16 draw_76_polygons[125] = {
#include "stg14/terrain/draw_76_polygons.inc"
};
static Sint32 draw_76_vertices[91] = {
#include "stg14/terrain/draw_76_vertices.inc"
};
static NJS_CNK_MODEL draw_76_model = {
#include "stg14/terrain/draw_76_model.inc"
};
static NJS_CNK_OBJECT draw_460 = {
#include "stg14/terrain/draw_460.inc"
};
static Sint16 draw_456_polygons[163] = {
#include "stg14/terrain/draw_456_polygons.inc"
};
static Sint32 draw_456_vertices[115] = {
#include "stg14/terrain/draw_456_vertices.inc"
};
static NJS_CNK_MODEL draw_456_model = {
#include "stg14/terrain/draw_456_model.inc"
};
static NJS_CNK_OBJECT draw_456 = {
#include "stg14/terrain/draw_456.inc"
};
static Sint16 draw_455_polygons[525] = {
#include "stg14/terrain/draw_455_polygons.inc"
};
static Sint32 draw_455_vertices[451] = {
#include "stg14/terrain/draw_455_vertices.inc"
};
static NJS_CNK_MODEL draw_455_model = {
#include "stg14/terrain/draw_455_model.inc"
};
static NJS_CNK_OBJECT draw_455 = {
#include "stg14/terrain/draw_455.inc"
};
static Sint16 draw_454_polygons[1235] = {
#include "stg14/terrain/draw_454_polygons.inc"
};
static Sint32 draw_454_vertices[831] = {
#include "stg14/terrain/draw_454_vertices.inc"
};
static NJS_CNK_MODEL draw_454_model = {
#include "stg14/terrain/draw_454_model.inc"
};
static NJS_CNK_OBJECT draw_454 = {
#include "stg14/terrain/draw_454.inc"
};
static Sint16 draw_408_polygons[193] = {
#include "stg14/terrain/draw_408_polygons.inc"
};
static Sint32 draw_408_vertices[139] = {
#include "stg14/terrain/draw_408_vertices.inc"
};
static NJS_CNK_MODEL draw_408_model = {
#include "stg14/terrain/draw_408_model.inc"
};
static NJS_CNK_OBJECT draw_446 = {
#include "stg14/terrain/draw_446.inc"
};
static Sint16 draw_430_polygons[229] = {
#include "stg14/terrain/draw_430_polygons.inc"
};
static Sint32 draw_430_vertices[195] = {
#include "stg14/terrain/draw_430_vertices.inc"
};
static NJS_CNK_MODEL draw_430_model = {
#include "stg14/terrain/draw_430_model.inc"
};
static NJS_CNK_OBJECT draw_430 = {
#include "stg14/terrain/draw_430.inc"
};
static Sint16 draw_428_polygons[195] = {
#include "stg14/terrain/draw_428_polygons.inc"
};
static Sint32 draw_428_vertices[107] = {
#include "stg14/terrain/draw_428_vertices.inc"
};
static NJS_CNK_MODEL draw_428_model = {
#include "stg14/terrain/draw_428_model.inc"
};
static NJS_CNK_OBJECT draw_429 = {
#include "stg14/terrain/draw_429.inc"
};
static Sint16 draw_421_polygons[151] = {
#include "stg14/terrain/draw_421_polygons.inc"
};
static Sint32 draw_421_vertices[111] = {
#include "stg14/terrain/draw_421_vertices.inc"
};
static NJS_CNK_MODEL draw_421_model = {
#include "stg14/terrain/draw_421_model.inc"
};
static NJS_CNK_OBJECT draw_426 = {
#include "stg14/terrain/draw_426.inc"
};
static Sint16 draw_423_polygons[191] = {
#include "stg14/terrain/draw_423_polygons.inc"
};
static Sint32 draw_423_vertices[131] = {
#include "stg14/terrain/draw_423_vertices.inc"
};
static NJS_CNK_MODEL draw_423_model = {
#include "stg14/terrain/draw_423_model.inc"
};
static NJS_CNK_OBJECT draw_423 = {
#include "stg14/terrain/draw_423.inc"
};
static Sint16 draw_422_polygons[329] = {
#include "stg14/terrain/draw_422_polygons.inc"
};
static Sint32 draw_422_vertices[131] = {
#include "stg14/terrain/draw_422_vertices.inc"
};
static NJS_CNK_MODEL draw_422_model = {
#include "stg14/terrain/draw_422_model.inc"
};
static NJS_CNK_OBJECT draw_422 = {
#include "stg14/terrain/draw_422.inc"
};
static Sint16 draw_137_polygons[517] = {
#include "stg14/terrain/draw_137_polygons.inc"
};
static Sint32 draw_137_vertices[351] = {
#include "stg14/terrain/draw_137_vertices.inc"
};
static NJS_CNK_MODEL draw_137_model = {
#include "stg14/terrain/draw_137_model.inc"
};
static NJS_CNK_OBJECT draw_404 = {
#include "stg14/terrain/draw_404.inc"
};
static Sint16 draw_31_polygons[313] = {
#include "stg14/terrain/draw_31_polygons.inc"
};
static Sint32 draw_31_vertices[187] = {
#include "stg14/terrain/draw_31_vertices.inc"
};
static NJS_CNK_MODEL draw_31_model = {
#include "stg14/terrain/draw_31_model.inc"
};
static NJS_CNK_OBJECT draw_356 = {
#include "stg14/terrain/draw_356.inc"
};
static Sint16 draw_30_polygons[279] = {
#include "stg14/terrain/draw_30_polygons.inc"
};
static Sint32 draw_30_vertices[195] = {
#include "stg14/terrain/draw_30_vertices.inc"
};
static NJS_CNK_MODEL draw_30_model = {
#include "stg14/terrain/draw_30_model.inc"
};
static NJS_CNK_OBJECT draw_355 = {
#include "stg14/terrain/draw_355.inc"
};
static Sint16 draw_265_polygons[89] = {
#include "stg14/terrain/draw_265_polygons.inc"
};
static Sint32 draw_265_vertices[107] = {
#include "stg14/terrain/draw_265_vertices.inc"
};
static NJS_CNK_MODEL draw_265_model = {
#include "stg14/terrain/draw_265_model.inc"
};
static NJS_CNK_OBJECT draw_338 = {
#include "stg14/terrain/draw_338.inc"
};
static Sint16 draw_294_polygons[867] = {
#include "stg14/terrain/draw_294_polygons.inc"
};
static Sint32 draw_294_vertices[475] = {
#include "stg14/terrain/draw_294_vertices.inc"
};
static NJS_CNK_MODEL draw_294_model = {
#include "stg14/terrain/draw_294_model.inc"
};
static NJS_CNK_OBJECT draw_313 = {
#include "stg14/terrain/draw_313.inc"
};
static Sint16 draw_259_polygons[521] = {
#include "stg14/terrain/draw_259_polygons.inc"
};
static Sint32 draw_259_vertices[435] = {
#include "stg14/terrain/draw_259_vertices.inc"
};
static NJS_CNK_MODEL draw_259_model = {
#include "stg14/terrain/draw_259_model.inc"
};
static NJS_CNK_OBJECT draw_259 = {
#include "stg14/terrain/draw_259.inc"
};
static Sint16 draw_250_polygons[155] = {
#include "stg14/terrain/draw_250_polygons.inc"
};
static Sint32 draw_250_vertices[99] = {
#include "stg14/terrain/draw_250_vertices.inc"
};
static NJS_CNK_MODEL draw_250_model = {
#include "stg14/terrain/draw_250_model.inc"
};
static NJS_CNK_OBJECT draw_250 = {
#include "stg14/terrain/draw_250.inc"
};
static Sint16 draw_218_polygons[129] = {
#include "stg14/terrain/draw_218_polygons.inc"
};
static Sint32 draw_218_vertices[71] = {
#include "stg14/terrain/draw_218_vertices.inc"
};
static NJS_CNK_MODEL draw_218_model = {
#include "stg14/terrain/draw_218_model.inc"
};
static NJS_CNK_OBJECT draw_218 = {
#include "stg14/terrain/draw_218.inc"
};
static Sint16 draw_148_polygons[69] = {
#include "stg14/terrain/draw_148_polygons.inc"
};
static Sint32 draw_148_vertices[35] = {
#include "stg14/terrain/draw_148_vertices.inc"
};
static NJS_CNK_MODEL draw_148_model = {
#include "stg14/terrain/draw_148_model.inc"
};
static NJS_CNK_OBJECT draw_194 = {
#include "stg14/terrain/draw_194.inc"
};
static Sint16 draw_185_polygons[41] = {
#include "stg14/terrain/draw_185_polygons.inc"
};
static Sint32 draw_185_vertices[43] = {
#include "stg14/terrain/draw_185_vertices.inc"
};
static NJS_CNK_MODEL draw_185_model = {
#include "stg14/terrain/draw_185_model.inc"
};
static NJS_CNK_OBJECT draw_185 = {
#include "stg14/terrain/draw_185.inc"
};
static Sint16 draw_152_polygons[125] = {
#include "stg14/terrain/draw_152_polygons.inc"
};
static Sint32 draw_152_vertices[75] = {
#include "stg14/terrain/draw_152_vertices.inc"
};
static NJS_CNK_MODEL draw_152_model = {
#include "stg14/terrain/draw_152_model.inc"
};
static NJS_CNK_OBJECT draw_152 = {
#include "stg14/terrain/draw_152.inc"
};
static Sint16 draw_151_polygons[125] = {
#include "stg14/terrain/draw_151_polygons.inc"
};
static Sint32 draw_151_vertices[75] = {
#include "stg14/terrain/draw_151_vertices.inc"
};
static NJS_CNK_MODEL draw_151_model = {
#include "stg14/terrain/draw_151_model.inc"
};
static NJS_CNK_OBJECT draw_151 = {
#include "stg14/terrain/draw_151.inc"
};
static Sint16 draw_149_polygons[235] = {
#include "stg14/terrain/draw_149_polygons.inc"
};
static Sint32 draw_149_vertices[135] = {
#include "stg14/terrain/draw_149_vertices.inc"
};
static NJS_CNK_MODEL draw_149_model = {
#include "stg14/terrain/draw_149_model.inc"
};
static NJS_CNK_OBJECT draw_149 = {
#include "stg14/terrain/draw_149.inc"
};
static Sint16 draw_142_polygons[509] = {
#include "stg14/terrain/draw_142_polygons.inc"
};
static Sint32 draw_142_vertices[327] = {
#include "stg14/terrain/draw_142_vertices.inc"
};
static NJS_CNK_MODEL draw_142_model = {
#include "stg14/terrain/draw_142_model.inc"
};
static NJS_CNK_OBJECT draw_142 = {
#include "stg14/terrain/draw_142.inc"
};
static Sint16 draw_134_polygons[407] = {
#include "stg14/terrain/draw_134_polygons.inc"
};
static Sint32 draw_134_vertices[259] = {
#include "stg14/terrain/draw_134_vertices.inc"
};
static NJS_CNK_MODEL draw_134_model = {
#include "stg14/terrain/draw_134_model.inc"
};
static NJS_CNK_OBJECT draw_134 = {
#include "stg14/terrain/draw_134.inc"
};
static Sint16 draw_72_polygons[447] = {
#include "stg14/terrain/draw_72_polygons.inc"
};
static Sint32 draw_72_vertices[287] = {
#include "stg14/terrain/draw_72_vertices.inc"
};
static NJS_CNK_MODEL draw_72_model = {
#include "stg14/terrain/draw_72_model.inc"
};
static NJS_CNK_OBJECT draw_72 = {
#include "stg14/terrain/draw_72.inc"
};
static Sint16 draw_66_polygons[899] = {
#include "stg14/terrain/draw_66_polygons.inc"
};
static Sint32 draw_66_vertices[707] = {
#include "stg14/terrain/draw_66_vertices.inc"
};
static NJS_CNK_MODEL draw_66_model = {
#include "stg14/terrain/draw_66_model.inc"
};
static NJS_CNK_OBJECT draw_66 = {
#include "stg14/terrain/draw_66.inc"
};
static Sint16 draw_65_polygons[943] = {
#include "stg14/terrain/draw_65_polygons.inc"
};
static Sint32 draw_65_vertices[611] = {
#include "stg14/terrain/draw_65_vertices.inc"
};
static NJS_CNK_MODEL draw_65_model = {
#include "stg14/terrain/draw_65_model.inc"
};
static NJS_CNK_OBJECT draw_65 = {
#include "stg14/terrain/draw_65.inc"
};
static Sint16 draw_64_polygons[249] = {
#include "stg14/terrain/draw_64_polygons.inc"
};
static Sint32 draw_64_vertices[215] = {
#include "stg14/terrain/draw_64_vertices.inc"
};
static NJS_CNK_MODEL draw_64_model = {
#include "stg14/terrain/draw_64_model.inc"
};
static NJS_CNK_OBJECT draw_64 = {
#include "stg14/terrain/draw_64.inc"
};
static Sint16 draw_58_polygons[547] = {
#include "stg14/terrain/draw_58_polygons.inc"
};
static Sint32 draw_58_vertices[427] = {
#include "stg14/terrain/draw_58_vertices.inc"
};
static NJS_CNK_MODEL draw_58_model = {
#include "stg14/terrain/draw_58_model.inc"
};
static NJS_CNK_OBJECT draw_58 = {
#include "stg14/terrain/draw_58.inc"
};
static Sint16 draw_56_polygons[943] = {
#include "stg14/terrain/draw_56_polygons.inc"
};
static Sint32 draw_56_vertices[611] = {
#include "stg14/terrain/draw_56_vertices.inc"
};
static NJS_CNK_MODEL draw_56_model = {
#include "stg14/terrain/draw_56_model.inc"
};
static NJS_CNK_OBJECT draw_56 = {
#include "stg14/terrain/draw_56.inc"
};
static Sint16 draw_55_polygons[547] = {
#include "stg14/terrain/draw_55_polygons.inc"
};
static Sint32 draw_55_vertices[427] = {
#include "stg14/terrain/draw_55_vertices.inc"
};
static NJS_CNK_MODEL draw_55_model = {
#include "stg14/terrain/draw_55_model.inc"
};
static NJS_CNK_OBJECT draw_55 = {
#include "stg14/terrain/draw_55.inc"
};
static Sint16 draw_39_polygons[249] = {
#include "stg14/terrain/draw_39_polygons.inc"
};
static Sint32 draw_39_vertices[215] = {
#include "stg14/terrain/draw_39_vertices.inc"
};
static NJS_CNK_MODEL draw_39_model = {
#include "stg14/terrain/draw_39_model.inc"
};
static NJS_CNK_OBJECT draw_39 = {
#include "stg14/terrain/draw_39.inc"
};
static Sint16 draw_17_polygons[59] = {
#include "stg14/terrain/draw_17_polygons.inc"
};
static Sint32 draw_17_vertices[51] = {
#include "stg14/terrain/draw_17_vertices.inc"
};
static NJS_CNK_MODEL draw_17_model = {
#include "stg14/terrain/draw_17_model.inc"
};
static NJS_CNK_OBJECT draw_17 = {
#include "stg14/terrain/draw_17.inc"
};
static Sint16 draw_6_polygons[1157] = {
#include "stg14/terrain/draw_6_polygons.inc"
};
static Sint32 draw_6_vertices[963] = {
#include "stg14/terrain/draw_6_vertices.inc"
};
static NJS_CNK_MODEL draw_6_model = {
#include "stg14/terrain/draw_6_model.inc"
};
static NJS_CNK_OBJECT draw_6 = {
#include "stg14/terrain/draw_6.inc"
};
static Sint16 draw_3_polygons[1267] = {
#include "stg14/terrain/draw_3_polygons.inc"
};
static Sint32 draw_3_vertices[991] = {
#include "stg14/terrain/draw_3_vertices.inc"
};
static NJS_CNK_MODEL draw_3_model = {
#include "stg14/terrain/draw_3_model.inc"
};
static NJS_CNK_OBJECT draw_3 = {
#include "stg14/terrain/draw_3.inc"
};
static NJS_OBJECT collision_1 = {
#include "stg14/terrain/collision_1.inc"
};
static NJS_OBJECT collision_2 = {
#include "stg14/terrain/collision_2.inc"
};
static NJS_OBJECT collision_3 = {
#include "stg14/terrain/collision_3.inc"
};
static NJS_OBJECT collision_4 = {
#include "stg14/terrain/collision_4.inc"
};
static NJS_OBJECT collision_5 = {
#include "stg14/terrain/collision_5.inc"
};
static NJS_OBJECT collision_7 = {
#include "stg14/terrain/collision_7.inc"
};
static NJS_OBJECT collision_9 = {
#include "stg14/terrain/collision_9.inc"
};
static NJS_OBJECT collision_12 = {
#include "stg14/terrain/collision_12.inc"
};
static NJS_OBJECT collision_13 = {
#include "stg14/terrain/collision_13.inc"
};
static NJS_OBJECT collision_14 = {
#include "stg14/terrain/collision_14.inc"
};
static NJS_OBJECT collision_16 = {
#include "stg14/terrain/collision_16.inc"
};
static NJS_OBJECT collision_24 = {
#include "stg14/terrain/collision_24.inc"
};
static NJS_OBJECT collision_32 = {
#include "stg14/terrain/collision_32.inc"
};
static NJS_OBJECT collision_34 = {
#include "stg14/terrain/collision_34.inc"
};
static NJS_OBJECT collision_35 = {
#include "stg14/terrain/collision_35.inc"
};
static NJS_OBJECT collision_36 = {
#include "stg14/terrain/collision_36.inc"
};
static NJS_OBJECT collision_37 = {
#include "stg14/terrain/collision_37.inc"
};
static NJS_OBJECT collision_38 = {
#include "stg14/terrain/collision_38.inc"
};
static NJS_OBJECT collision_39 = {
#include "stg14/terrain/collision_39.inc"
};
static NJS_OBJECT collision_40 = {
#include "stg14/terrain/collision_40.inc"
};
static NJS_OBJECT collision_41 = {
#include "stg14/terrain/collision_41.inc"
};
static NJS_OBJECT collision_42 = {
#include "stg14/terrain/collision_42.inc"
};
static NJS_OBJECT collision_46 = {
#include "stg14/terrain/collision_46.inc"
};
static NJS_OBJECT collision_47 = {
#include "stg14/terrain/collision_47.inc"
};
static NJS_OBJECT collision_48 = {
#include "stg14/terrain/collision_48.inc"
};
static NJS_OBJECT collision_49 = {
#include "stg14/terrain/collision_49.inc"
};
static NJS_OBJECT collision_50 = {
#include "stg14/terrain/collision_50.inc"
};
static NJS_OBJECT collision_51 = {
#include "stg14/terrain/collision_51.inc"
};
static NJS_OBJECT collision_53 = {
#include "stg14/terrain/collision_53.inc"
};
static NJS_OBJECT collision_54 = {
#include "stg14/terrain/collision_54.inc"
};
static NJS_OBJECT collision_55 = {
#include "stg14/terrain/collision_55.inc"
};
static NJS_OBJECT collision_56 = {
#include "stg14/terrain/collision_56.inc"
};
static NJS_OBJECT collision_58 = {
#include "stg14/terrain/collision_58.inc"
};
static NJS_OBJECT collision_59 = {
#include "stg14/terrain/collision_59.inc"
};
static NJS_OBJECT collision_60 = {
#include "stg14/terrain/collision_60.inc"
};
static NJS_OBJECT collision_61 = {
#include "stg14/terrain/collision_61.inc"
};
static NJS_OBJECT collision_62 = {
#include "stg14/terrain/collision_62.inc"
};
static NJS_OBJECT collision_64 = {
#include "stg14/terrain/collision_64.inc"
};
static NJS_OBJECT collision_65 = {
#include "stg14/terrain/collision_65.inc"
};
static NJS_OBJECT collision_66 = {
#include "stg14/terrain/collision_66.inc"
};
static NJS_OBJECT collision_67 = {
#include "stg14/terrain/collision_67.inc"
};
static NJS_OBJECT collision_73 = {
#include "stg14/terrain/collision_73.inc"
};
static NJS_OBJECT collision_75 = {
#include "stg14/terrain/collision_75.inc"
};
static NJS_OBJECT collision_84 = {
#include "stg14/terrain/collision_84.inc"
};
static NJS_OBJECT collision_88 = {
#include "stg14/terrain/collision_88.inc"
};
static NJS_OBJECT collision_89 = {
#include "stg14/terrain/collision_89.inc"
};
static NJS_OBJECT collision_91 = {
#include "stg14/terrain/collision_91.inc"
};
static NJS_OBJECT collision_93 = {
#include "stg14/terrain/collision_93.inc"
};
static NJS_OBJECT collision_94 = {
#include "stg14/terrain/collision_94.inc"
};
static NJS_OBJECT collision_96 = {
#include "stg14/terrain/collision_96.inc"
};
static NJS_OBJECT collision_101 = {
#include "stg14/terrain/collision_101.inc"
};
static NJS_OBJECT collision_102 = {
#include "stg14/terrain/collision_102.inc"
};
static NJS_OBJECT collision_103 = {
#include "stg14/terrain/collision_103.inc"
};
static NJS_OBJECT collision_106 = {
#include "stg14/terrain/collision_106.inc"
};
static NJS_OBJECT collision_108 = {
#include "stg14/terrain/collision_108.inc"
};
static NJS_OBJECT collision_109 = {
#include "stg14/terrain/collision_109.inc"
};
static NJS_OBJECT collision_111 = {
#include "stg14/terrain/collision_111.inc"
};
static NJS_OBJECT collision_113 = {
#include "stg14/terrain/collision_113.inc"
};
static NJS_OBJECT collision_116 = {
#include "stg14/terrain/collision_116.inc"
};
static NJS_OBJECT collision_118 = {
#include "stg14/terrain/collision_118.inc"
};
static NJS_OBJECT collision_121 = {
#include "stg14/terrain/collision_121.inc"
};
static NJS_OBJECT collision_125 = {
#include "stg14/terrain/collision_125.inc"
};
static NJS_OBJECT collision_126 = {
#include "stg14/terrain/collision_126.inc"
};
static NJS_OBJECT collision_127 = {
#include "stg14/terrain/collision_127.inc"
};
static NJS_OBJECT collision_128 = {
#include "stg14/terrain/collision_128.inc"
};
static NJS_OBJECT collision_129 = {
#include "stg14/terrain/collision_129.inc"
};
static NJS_OBJECT collision_131 = {
#include "stg14/terrain/collision_131.inc"
};
static NJS_OBJECT collision_132 = {
#include "stg14/terrain/collision_132.inc"
};
static NJS_OBJECT collision_133 = {
#include "stg14/terrain/collision_133.inc"
};
static NJS_OBJECT collision_135 = {
#include "stg14/terrain/collision_135.inc"
};
static NJS_OBJECT collision_137 = {
#include "stg14/terrain/collision_137.inc"
};
static NJS_OBJECT collision_138 = {
#include "stg14/terrain/collision_138.inc"
};
static NJS_OBJECT collision_140 = {
#include "stg14/terrain/collision_140.inc"
};
static NJS_OBJECT collision_141 = {
#include "stg14/terrain/collision_141.inc"
};
static NJS_OBJECT collision_145 = {
#include "stg14/terrain/collision_145.inc"
};
static NJS_OBJECT collision_147 = {
#include "stg14/terrain/collision_147.inc"
};
static NJS_OBJECT collision_150 = {
#include "stg14/terrain/collision_150.inc"
};
static NJS_OBJECT collision_154 = {
#include "stg14/terrain/collision_154.inc"
};
static NJS_OBJECT collision_158 = {
#include "stg14/terrain/collision_158.inc"
};
static NJS_OBJECT collision_159 = {
#include "stg14/terrain/collision_159.inc"
};
static NJS_OBJECT collision_160 = {
#include "stg14/terrain/collision_160.inc"
};
static NJS_OBJECT collision_161 = {
#include "stg14/terrain/collision_161.inc"
};
static NJS_OBJECT collision_162 = {
#include "stg14/terrain/collision_162.inc"
};
static NJS_OBJECT collision_163 = {
#include "stg14/terrain/collision_163.inc"
};
static NJS_OBJECT collision_164 = {
#include "stg14/terrain/collision_164.inc"
};
static NJS_OBJECT collision_166 = {
#include "stg14/terrain/collision_166.inc"
};
static NJS_OBJECT collision_168 = {
#include "stg14/terrain/collision_168.inc"
};
static NJS_OBJECT collision_169 = {
#include "stg14/terrain/collision_169.inc"
};
static NJS_OBJECT collision_170 = {
#include "stg14/terrain/collision_170.inc"
};
static NJS_OBJECT collision_171 = {
#include "stg14/terrain/collision_171.inc"
};
static NJS_OBJECT collision_172 = {
#include "stg14/terrain/collision_172.inc"
};
static NJS_OBJECT collision_173 = {
#include "stg14/terrain/collision_173.inc"
};
static NJS_OBJECT collision_174 = {
#include "stg14/terrain/collision_174.inc"
};
static NJS_OBJECT collision_175 = {
#include "stg14/terrain/collision_175.inc"
};
static NJS_OBJECT collision_176 = {
#include "stg14/terrain/collision_176.inc"
};
static NJS_OBJECT collision_178 = {
#include "stg14/terrain/collision_178.inc"
};
static NJS_OBJECT collision_180 = {
#include "stg14/terrain/collision_180.inc"
};
static NJS_OBJECT collision_181 = {
#include "stg14/terrain/collision_181.inc"
};
static NJS_OBJECT collision_182 = {
#include "stg14/terrain/collision_182.inc"
};
static NJS_OBJECT collision_183 = {
#include "stg14/terrain/collision_183.inc"
};
static NJS_OBJECT collision_184 = {
#include "stg14/terrain/collision_184.inc"
};
static NJS_OBJECT collision_185 = {
#include "stg14/terrain/collision_185.inc"
};
static NJS_OBJECT collision_186 = {
#include "stg14/terrain/collision_186.inc"
};
static NJS_OBJECT collision_187 = {
#include "stg14/terrain/collision_187.inc"
};
static NJS_OBJECT collision_188 = {
#include "stg14/terrain/collision_188.inc"
};
static NJS_OBJECT collision_192 = {
#include "stg14/terrain/collision_192.inc"
};
static NJS_OBJECT collision_205 = {
#include "stg14/terrain/collision_205.inc"
};
static NJS_OBJECT collision_206 = {
#include "stg14/terrain/collision_206.inc"
};
static NJS_OBJECT collision_209 = {
#include "stg14/terrain/collision_209.inc"
};
static NJS_OBJECT collision_210 = {
#include "stg14/terrain/collision_210.inc"
};
static NJS_OBJECT collision_211 = {
#include "stg14/terrain/collision_211.inc"
};
static NJS_OBJECT collision_212 = {
#include "stg14/terrain/collision_212.inc"
};
static NJS_OBJECT collision_213 = {
#include "stg14/terrain/collision_213.inc"
};
static NJS_OBJECT collision_214 = {
#include "stg14/terrain/collision_214.inc"
};
static NJS_OBJECT collision_216 = {
#include "stg14/terrain/collision_216.inc"
};
static NJS_OBJECT collision_217 = {
#include "stg14/terrain/collision_217.inc"
};
static NJS_OBJECT collision_219 = {
#include "stg14/terrain/collision_219.inc"
};
static NJS_OBJECT collision_221 = {
#include "stg14/terrain/collision_221.inc"
};
static NJS_OBJECT collision_222 = {
#include "stg14/terrain/collision_222.inc"
};
static NJS_OBJECT collision_223 = {
#include "stg14/terrain/collision_223.inc"
};
static NJS_OBJECT collision_224 = {
#include "stg14/terrain/collision_224.inc"
};
static NJS_OBJECT collision_226 = {
#include "stg14/terrain/collision_226.inc"
};
static NJS_OBJECT collision_227 = {
#include "stg14/terrain/collision_227.inc"
};
static NJS_OBJECT collision_228 = {
#include "stg14/terrain/collision_228.inc"
};
static NJS_OBJECT collision_229 = {
#include "stg14/terrain/collision_229.inc"
};
static NJS_OBJECT collision_230 = {
#include "stg14/terrain/collision_230.inc"
};
static NJS_OBJECT collision_237 = {
#include "stg14/terrain/collision_237.inc"
};
static NJS_OBJECT collision_238 = {
#include "stg14/terrain/collision_238.inc"
};
static NJS_OBJECT collision_240 = {
#include "stg14/terrain/collision_240.inc"
};
static NJS_OBJECT collision_241 = {
#include "stg14/terrain/collision_241.inc"
};
static NJS_OBJECT collision_242 = {
#include "stg14/terrain/collision_242.inc"
};
static NJS_OBJECT collision_243 = {
#include "stg14/terrain/collision_243.inc"
};
static NJS_OBJECT collision_244 = {
#include "stg14/terrain/collision_244.inc"
};
static NJS_OBJECT collision_245 = {
#include "stg14/terrain/collision_245.inc"
};
static NJS_OBJECT collision_247 = {
#include "stg14/terrain/collision_247.inc"
};
static NJS_OBJECT collision_250 = {
#include "stg14/terrain/collision_250.inc"
};
static NJS_OBJECT collision_251 = {
#include "stg14/terrain/collision_251.inc"
};
static NJS_OBJECT collision_252 = {
#include "stg14/terrain/collision_252.inc"
};
static NJS_OBJECT collision_253 = {
#include "stg14/terrain/collision_253.inc"
};
static NJS_OBJECT collision_254 = {
#include "stg14/terrain/collision_254.inc"
};
static NJS_OBJECT collision_255 = {
#include "stg14/terrain/collision_255.inc"
};
static NJS_OBJECT collision_256 = {
#include "stg14/terrain/collision_256.inc"
};
static NJS_OBJECT collision_257 = {
#include "stg14/terrain/collision_257.inc"
};
static NJS_OBJECT collision_258 = {
#include "stg14/terrain/collision_258.inc"
};
static NJS_OBJECT collision_259 = {
#include "stg14/terrain/collision_259.inc"
};
static NJS_OBJECT collision_260 = {
#include "stg14/terrain/collision_260.inc"
};
static NJS_OBJECT collision_261 = {
#include "stg14/terrain/collision_261.inc"
};
static NJS_OBJECT collision_262 = {
#include "stg14/terrain/collision_262.inc"
};
static NJS_OBJECT collision_263 = {
#include "stg14/terrain/collision_263.inc"
};
static NJS_OBJECT collision_264 = {
#include "stg14/terrain/collision_264.inc"
};
static NJS_OBJECT collision_266 = {
#include "stg14/terrain/collision_266.inc"
};
static NJS_OBJECT collision_269 = {
#include "stg14/terrain/collision_269.inc"
};
static NJS_OBJECT collision_270 = {
#include "stg14/terrain/collision_270.inc"
};
static NJS_OBJECT collision_273 = {
#include "stg14/terrain/collision_273.inc"
};
static NJS_OBJECT collision_279 = {
#include "stg14/terrain/collision_279.inc"
};
static NJS_OBJECT collision_284 = {
#include "stg14/terrain/collision_284.inc"
};
static NJS_OBJECT collision_285 = {
#include "stg14/terrain/collision_285.inc"
};
static NJS_OBJECT collision_293 = {
#include "stg14/terrain/collision_293.inc"
};
static NJS_OBJECT collision_296 = {
#include "stg14/terrain/collision_296.inc"
};
static NJS_OBJECT collision_297 = {
#include "stg14/terrain/collision_297.inc"
};
static NJS_OBJECT collision_298 = {
#include "stg14/terrain/collision_298.inc"
};
static NJS_OBJECT collision_310 = {
#include "stg14/terrain/collision_310.inc"
};
static NJS_OBJECT collision_312 = {
#include "stg14/terrain/collision_312.inc"
};
static NJS_OBJECT collision_313 = {
#include "stg14/terrain/collision_313.inc"
};
static NJS_OBJECT collision_314 = {
#include "stg14/terrain/collision_314.inc"
};
static NJS_OBJECT collision_316 = {
#include "stg14/terrain/collision_316.inc"
};
static NJS_OBJECT collision_317 = {
#include "stg14/terrain/collision_317.inc"
};
static NJS_OBJECT collision_319 = {
#include "stg14/terrain/collision_319.inc"
};
static NJS_OBJECT collision_321 = {
#include "stg14/terrain/collision_321.inc"
};
static NJS_OBJECT collision_323 = {
#include "stg14/terrain/collision_323.inc"
};
static NJS_OBJECT collision_325 = {
#include "stg14/terrain/collision_325.inc"
};
static NJS_OBJECT collision_329 = {
#include "stg14/terrain/collision_329.inc"
};
static NJS_OBJECT collision_331 = {
#include "stg14/terrain/collision_331.inc"
};
static NJS_OBJECT collision_332 = {
#include "stg14/terrain/collision_332.inc"
};
static NJS_OBJECT collision_333 = {
#include "stg14/terrain/collision_333.inc"
};
static NJS_OBJECT collision_334 = {
#include "stg14/terrain/collision_334.inc"
};
static NJS_OBJECT collision_337 = {
#include "stg14/terrain/collision_337.inc"
};
static NJS_OBJECT collision_339 = {
#include "stg14/terrain/collision_339.inc"
};
static NJS_OBJECT collision_341 = {
#include "stg14/terrain/collision_341.inc"
};
static NJS_OBJECT collision_342 = {
#include "stg14/terrain/collision_342.inc"
};
static NJS_OBJECT collision_343 = {
#include "stg14/terrain/collision_343.inc"
};
static NJS_OBJECT collision_344 = {
#include "stg14/terrain/collision_344.inc"
};
static NJS_OBJECT collision_345 = {
#include "stg14/terrain/collision_345.inc"
};
static NJS_OBJECT collision_346 = {
#include "stg14/terrain/collision_346.inc"
};
static NJS_OBJECT collision_347 = {
#include "stg14/terrain/collision_347.inc"
};
static NJS_OBJECT collision_348 = {
#include "stg14/terrain/collision_348.inc"
};
static NJS_OBJECT collision_350 = {
#include "stg14/terrain/collision_350.inc"
};
static NJS_OBJECT collision_352 = {
#include "stg14/terrain/collision_352.inc"
};
static NJS_OBJECT collision_363 = {
#include "stg14/terrain/collision_363.inc"
};
static NJS_OBJECT collision_365 = {
#include "stg14/terrain/collision_365.inc"
};
static NJS_OBJECT collision_366 = {
#include "stg14/terrain/collision_366.inc"
};
static NJS_OBJECT collision_367 = {
#include "stg14/terrain/collision_367.inc"
};
static NJS_OBJECT collision_368 = {
#include "stg14/terrain/collision_368.inc"
};
static NJS_OBJECT collision_369 = {
#include "stg14/terrain/collision_369.inc"
};
static NJS_OBJECT collision_372 = {
#include "stg14/terrain/collision_372.inc"
};
static NJS_OBJECT collision_373 = {
#include "stg14/terrain/collision_373.inc"
};
static NJS_OBJECT collision_374 = {
#include "stg14/terrain/collision_374.inc"
};
static NJS_OBJECT collision_376 = {
#include "stg14/terrain/collision_376.inc"
};
static NJS_OBJECT collision_378 = {
#include "stg14/terrain/collision_378.inc"
};
static NJS_OBJECT collision_379 = {
#include "stg14/terrain/collision_379.inc"
};
static NJS_OBJECT collision_381 = {
#include "stg14/terrain/collision_381.inc"
};
static NJS_OBJECT collision_382 = {
#include "stg14/terrain/collision_382.inc"
};
static NJS_OBJECT collision_386 = {
#include "stg14/terrain/collision_386.inc"
};
static NJS_OBJECT collision_388 = {
#include "stg14/terrain/collision_388.inc"
};
static NJS_OBJECT collision_390 = {
#include "stg14/terrain/collision_390.inc"
};
static NJS_OBJECT collision_391 = {
#include "stg14/terrain/collision_391.inc"
};
static NJS_OBJECT collision_393 = {
#include "stg14/terrain/collision_393.inc"
};
static NJS_OBJECT collision_394 = {
#include "stg14/terrain/collision_394.inc"
};
static NJS_OBJECT collision_395 = {
#include "stg14/terrain/collision_395.inc"
};
static NJS_OBJECT collision_396 = {
#include "stg14/terrain/collision_396.inc"
};
static NJS_OBJECT collision_397 = {
#include "stg14/terrain/collision_397.inc"
};
static NJS_OBJECT collision_400 = {
#include "stg14/terrain/collision_400.inc"
};
static NJS_OBJECT collision_403 = {
#include "stg14/terrain/collision_403.inc"
};
static NJS_OBJECT collision_415 = {
#include "stg14/terrain/collision_415.inc"
};
static NJS_CNK_OBJECT draw_706 = {
#include "stg14/terrain/draw_706.inc"
};
static NJS_CNK_OBJECT draw_705 = {
#include "stg14/terrain/draw_705.inc"
};
static NJS_CNK_OBJECT draw_703 = {
#include "stg14/terrain/draw_703.inc"
};
static NJS_CNK_OBJECT draw_702 = {
#include "stg14/terrain/draw_702.inc"
};
static NJS_CNK_OBJECT draw_701 = {
#include "stg14/terrain/draw_701.inc"
};
static NJS_CNK_OBJECT draw_700 = {
#include "stg14/terrain/draw_700.inc"
};
static NJS_CNK_OBJECT draw_699 = {
#include "stg14/terrain/draw_699.inc"
};
static NJS_CNK_OBJECT draw_698 = {
#include "stg14/terrain/draw_698.inc"
};
static NJS_CNK_OBJECT draw_697 = {
#include "stg14/terrain/draw_697.inc"
};
static NJS_CNK_OBJECT draw_696 = {
#include "stg14/terrain/draw_696.inc"
};
static NJS_CNK_OBJECT draw_694 = {
#include "stg14/terrain/draw_694.inc"
};
static NJS_CNK_OBJECT draw_693 = {
#include "stg14/terrain/draw_693.inc"
};
static NJS_CNK_OBJECT draw_692 = {
#include "stg14/terrain/draw_692.inc"
};
static NJS_CNK_OBJECT draw_691 = {
#include "stg14/terrain/draw_691.inc"
};
static NJS_CNK_OBJECT draw_690 = {
#include "stg14/terrain/draw_690.inc"
};
static NJS_CNK_OBJECT draw_689 = {
#include "stg14/terrain/draw_689.inc"
};
static NJS_CNK_OBJECT draw_688 = {
#include "stg14/terrain/draw_688.inc"
};
static NJS_CNK_OBJECT draw_687 = {
#include "stg14/terrain/draw_687.inc"
};
static NJS_CNK_OBJECT draw_686 = {
#include "stg14/terrain/draw_686.inc"
};
static NJS_CNK_OBJECT draw_685 = {
#include "stg14/terrain/draw_685.inc"
};
static NJS_CNK_OBJECT draw_684 = {
#include "stg14/terrain/draw_684.inc"
};
static NJS_CNK_OBJECT draw_683 = {
#include "stg14/terrain/draw_683.inc"
};
static NJS_CNK_OBJECT draw_682 = {
#include "stg14/terrain/draw_682.inc"
};
static NJS_CNK_OBJECT draw_681 = {
#include "stg14/terrain/draw_681.inc"
};
static NJS_CNK_OBJECT draw_680 = {
#include "stg14/terrain/draw_680.inc"
};
static NJS_CNK_OBJECT draw_678 = {
#include "stg14/terrain/draw_678.inc"
};
static NJS_CNK_OBJECT draw_677 = {
#include "stg14/terrain/draw_677.inc"
};
static NJS_CNK_OBJECT draw_676 = {
#include "stg14/terrain/draw_676.inc"
};
static NJS_CNK_OBJECT draw_674 = {
#include "stg14/terrain/draw_674.inc"
};
static NJS_CNK_OBJECT draw_673 = {
#include "stg14/terrain/draw_673.inc"
};
static NJS_CNK_OBJECT draw_672 = {
#include "stg14/terrain/draw_672.inc"
};
static NJS_CNK_OBJECT draw_671 = {
#include "stg14/terrain/draw_671.inc"
};
static NJS_CNK_OBJECT draw_670 = {
#include "stg14/terrain/draw_670.inc"
};
static NJS_CNK_OBJECT draw_669 = {
#include "stg14/terrain/draw_669.inc"
};
static NJS_CNK_OBJECT draw_668 = {
#include "stg14/terrain/draw_668.inc"
};
static NJS_CNK_OBJECT draw_667 = {
#include "stg14/terrain/draw_667.inc"
};
static NJS_CNK_OBJECT draw_665 = {
#include "stg14/terrain/draw_665.inc"
};
static NJS_CNK_OBJECT draw_664 = {
#include "stg14/terrain/draw_664.inc"
};
static NJS_CNK_OBJECT draw_663 = {
#include "stg14/terrain/draw_663.inc"
};
static NJS_CNK_OBJECT draw_662 = {
#include "stg14/terrain/draw_662.inc"
};
static NJS_CNK_OBJECT draw_661 = {
#include "stg14/terrain/draw_661.inc"
};
static NJS_CNK_OBJECT draw_660 = {
#include "stg14/terrain/draw_660.inc"
};
static NJS_CNK_OBJECT draw_659 = {
#include "stg14/terrain/draw_659.inc"
};
static NJS_CNK_OBJECT draw_658 = {
#include "stg14/terrain/draw_658.inc"
};
static NJS_CNK_OBJECT draw_656 = {
#include "stg14/terrain/draw_656.inc"
};
static NJS_CNK_OBJECT draw_648 = {
#include "stg14/terrain/draw_648.inc"
};
static NJS_CNK_OBJECT draw_647 = {
#include "stg14/terrain/draw_647.inc"
};
static NJS_CNK_OBJECT draw_646 = {
#include "stg14/terrain/draw_646.inc"
};
static NJS_CNK_OBJECT draw_639 = {
#include "stg14/terrain/draw_639.inc"
};
static NJS_CNK_OBJECT draw_638 = {
#include "stg14/terrain/draw_638.inc"
};
static NJS_CNK_OBJECT draw_637 = {
#include "stg14/terrain/draw_637.inc"
};
static NJS_CNK_OBJECT draw_636 = {
#include "stg14/terrain/draw_636.inc"
};
static NJS_CNK_OBJECT draw_635 = {
#include "stg14/terrain/draw_635.inc"
};
static NJS_CNK_OBJECT draw_634 = {
#include "stg14/terrain/draw_634.inc"
};
static NJS_CNK_OBJECT draw_633 = {
#include "stg14/terrain/draw_633.inc"
};
static NJS_CNK_OBJECT draw_632 = {
#include "stg14/terrain/draw_632.inc"
};
static NJS_CNK_OBJECT draw_631 = {
#include "stg14/terrain/draw_631.inc"
};
static NJS_CNK_OBJECT draw_630 = {
#include "stg14/terrain/draw_630.inc"
};
static NJS_CNK_OBJECT draw_629 = {
#include "stg14/terrain/draw_629.inc"
};
static NJS_CNK_OBJECT draw_628 = {
#include "stg14/terrain/draw_628.inc"
};
static NJS_CNK_OBJECT draw_627 = {
#include "stg14/terrain/draw_627.inc"
};
static NJS_CNK_OBJECT draw_625 = {
#include "stg14/terrain/draw_625.inc"
};
static NJS_CNK_OBJECT draw_624 = {
#include "stg14/terrain/draw_624.inc"
};
static NJS_CNK_OBJECT draw_623 = {
#include "stg14/terrain/draw_623.inc"
};
static NJS_CNK_OBJECT draw_619 = {
#include "stg14/terrain/draw_619.inc"
};
static NJS_CNK_OBJECT draw_618 = {
#include "stg14/terrain/draw_618.inc"
};
static NJS_CNK_OBJECT draw_616 = {
#include "stg14/terrain/draw_616.inc"
};
static NJS_CNK_OBJECT draw_615 = {
#include "stg14/terrain/draw_615.inc"
};
static NJS_CNK_OBJECT draw_614 = {
#include "stg14/terrain/draw_614.inc"
};
static NJS_CNK_OBJECT draw_613 = {
#include "stg14/terrain/draw_613.inc"
};
static NJS_CNK_OBJECT draw_612 = {
#include "stg14/terrain/draw_612.inc"
};
static NJS_CNK_OBJECT draw_611 = {
#include "stg14/terrain/draw_611.inc"
};
static NJS_CNK_OBJECT draw_610 = {
#include "stg14/terrain/draw_610.inc"
};
static NJS_CNK_OBJECT draw_609 = {
#include "stg14/terrain/draw_609.inc"
};
static NJS_CNK_OBJECT draw_606 = {
#include "stg14/terrain/draw_606.inc"
};
static NJS_CNK_OBJECT draw_605 = {
#include "stg14/terrain/draw_605.inc"
};
static NJS_CNK_OBJECT draw_602 = {
#include "stg14/terrain/draw_602.inc"
};
static NJS_CNK_OBJECT draw_601 = {
#include "stg14/terrain/draw_601.inc"
};
static NJS_CNK_OBJECT draw_593 = {
#include "stg14/terrain/draw_593.inc"
};
static NJS_CNK_OBJECT draw_592 = {
#include "stg14/terrain/draw_592.inc"
};
static NJS_CNK_OBJECT draw_591 = {
#include "stg14/terrain/draw_591.inc"
};
static NJS_CNK_OBJECT draw_589 = {
#include "stg14/terrain/draw_589.inc"
};
static NJS_CNK_OBJECT draw_588 = {
#include "stg14/terrain/draw_588.inc"
};
static NJS_CNK_OBJECT draw_587 = {
#include "stg14/terrain/draw_587.inc"
};
static NJS_CNK_OBJECT draw_586 = {
#include "stg14/terrain/draw_586.inc"
};
static NJS_CNK_OBJECT draw_582 = {
#include "stg14/terrain/draw_582.inc"
};
static NJS_CNK_OBJECT draw_581 = {
#include "stg14/terrain/draw_581.inc"
};
static NJS_CNK_OBJECT draw_580 = {
#include "stg14/terrain/draw_580.inc"
};
static NJS_CNK_OBJECT draw_579 = {
#include "stg14/terrain/draw_579.inc"
};
static NJS_CNK_OBJECT draw_578 = {
#include "stg14/terrain/draw_578.inc"
};
static NJS_CNK_OBJECT draw_577 = {
#include "stg14/terrain/draw_577.inc"
};
static NJS_CNK_OBJECT draw_576 = {
#include "stg14/terrain/draw_576.inc"
};
static NJS_CNK_OBJECT draw_575 = {
#include "stg14/terrain/draw_575.inc"
};
static NJS_CNK_OBJECT draw_574 = {
#include "stg14/terrain/draw_574.inc"
};
static NJS_CNK_OBJECT draw_573 = {
#include "stg14/terrain/draw_573.inc"
};
static NJS_CNK_OBJECT draw_572 = {
#include "stg14/terrain/draw_572.inc"
};
static NJS_CNK_OBJECT draw_571 = {
#include "stg14/terrain/draw_571.inc"
};
static NJS_CNK_OBJECT draw_570 = {
#include "stg14/terrain/draw_570.inc"
};
static NJS_CNK_OBJECT draw_569 = {
#include "stg14/terrain/draw_569.inc"
};
static NJS_CNK_OBJECT draw_568 = {
#include "stg14/terrain/draw_568.inc"
};
static NJS_CNK_OBJECT draw_567 = {
#include "stg14/terrain/draw_567.inc"
};
static NJS_CNK_OBJECT draw_566 = {
#include "stg14/terrain/draw_566.inc"
};
static NJS_CNK_OBJECT draw_565 = {
#include "stg14/terrain/draw_565.inc"
};
static NJS_CNK_OBJECT draw_564 = {
#include "stg14/terrain/draw_564.inc"
};
static NJS_CNK_OBJECT draw_563 = {
#include "stg14/terrain/draw_563.inc"
};
static NJS_CNK_OBJECT draw_562 = {
#include "stg14/terrain/draw_562.inc"
};
static NJS_CNK_OBJECT draw_561 = {
#include "stg14/terrain/draw_561.inc"
};
static NJS_CNK_OBJECT draw_560 = {
#include "stg14/terrain/draw_560.inc"
};
static NJS_CNK_OBJECT draw_559 = {
#include "stg14/terrain/draw_559.inc"
};
static NJS_CNK_OBJECT draw_558 = {
#include "stg14/terrain/draw_558.inc"
};
static NJS_CNK_OBJECT draw_556 = {
#include "stg14/terrain/draw_556.inc"
};
static NJS_CNK_OBJECT draw_555 = {
#include "stg14/terrain/draw_555.inc"
};
static NJS_CNK_OBJECT draw_553 = {
#include "stg14/terrain/draw_553.inc"
};
static NJS_CNK_OBJECT draw_552 = {
#include "stg14/terrain/draw_552.inc"
};
static NJS_CNK_OBJECT draw_551 = {
#include "stg14/terrain/draw_551.inc"
};
static NJS_CNK_OBJECT draw_550 = {
#include "stg14/terrain/draw_550.inc"
};
static NJS_CNK_OBJECT draw_549 = {
#include "stg14/terrain/draw_549.inc"
};
static NJS_CNK_OBJECT draw_548 = {
#include "stg14/terrain/draw_548.inc"
};
static NJS_CNK_OBJECT draw_547 = {
#include "stg14/terrain/draw_547.inc"
};
static NJS_CNK_OBJECT draw_546 = {
#include "stg14/terrain/draw_546.inc"
};
static NJS_CNK_OBJECT draw_545 = {
#include "stg14/terrain/draw_545.inc"
};
static NJS_CNK_OBJECT draw_544 = {
#include "stg14/terrain/draw_544.inc"
};
static NJS_CNK_OBJECT draw_541 = {
#include "stg14/terrain/draw_541.inc"
};
static NJS_CNK_OBJECT draw_537 = {
#include "stg14/terrain/draw_537.inc"
};
static NJS_CNK_OBJECT draw_536 = {
#include "stg14/terrain/draw_536.inc"
};
static NJS_CNK_OBJECT draw_535 = {
#include "stg14/terrain/draw_535.inc"
};
static NJS_CNK_OBJECT draw_534 = {
#include "stg14/terrain/draw_534.inc"
};
static NJS_CNK_OBJECT draw_532 = {
#include "stg14/terrain/draw_532.inc"
};
static NJS_CNK_OBJECT draw_528 = {
#include "stg14/terrain/draw_528.inc"
};
static NJS_CNK_OBJECT draw_527 = {
#include "stg14/terrain/draw_527.inc"
};
static NJS_CNK_OBJECT draw_524 = {
#include "stg14/terrain/draw_524.inc"
};
static NJS_CNK_OBJECT draw_523 = {
#include "stg14/terrain/draw_523.inc"
};
static NJS_CNK_OBJECT draw_522 = {
#include "stg14/terrain/draw_522.inc"
};
static NJS_CNK_OBJECT draw_521 = {
#include "stg14/terrain/draw_521.inc"
};
static NJS_CNK_OBJECT draw_520 = {
#include "stg14/terrain/draw_520.inc"
};
static NJS_CNK_OBJECT draw_511 = {
#include "stg14/terrain/draw_511.inc"
};
static NJS_CNK_OBJECT draw_510 = {
#include "stg14/terrain/draw_510.inc"
};
static NJS_CNK_OBJECT draw_509 = {
#include "stg14/terrain/draw_509.inc"
};
static NJS_CNK_OBJECT draw_508 = {
#include "stg14/terrain/draw_508.inc"
};
static NJS_CNK_OBJECT draw_507 = {
#include "stg14/terrain/draw_507.inc"
};
static NJS_CNK_OBJECT draw_506 = {
#include "stg14/terrain/draw_506.inc"
};
static NJS_CNK_OBJECT draw_504 = {
#include "stg14/terrain/draw_504.inc"
};
static NJS_CNK_OBJECT draw_503 = {
#include "stg14/terrain/draw_503.inc"
};
static NJS_CNK_OBJECT draw_502 = {
#include "stg14/terrain/draw_502.inc"
};
static NJS_CNK_OBJECT draw_501 = {
#include "stg14/terrain/draw_501.inc"
};
static NJS_CNK_OBJECT draw_500 = {
#include "stg14/terrain/draw_500.inc"
};
static NJS_CNK_OBJECT draw_499 = {
#include "stg14/terrain/draw_499.inc"
};
static NJS_CNK_OBJECT draw_498 = {
#include "stg14/terrain/draw_498.inc"
};
static NJS_CNK_OBJECT draw_497 = {
#include "stg14/terrain/draw_497.inc"
};
static NJS_CNK_OBJECT draw_496 = {
#include "stg14/terrain/draw_496.inc"
};
static NJS_CNK_OBJECT draw_495 = {
#include "stg14/terrain/draw_495.inc"
};
static NJS_CNK_OBJECT draw_494 = {
#include "stg14/terrain/draw_494.inc"
};
static NJS_CNK_OBJECT draw_493 = {
#include "stg14/terrain/draw_493.inc"
};
static NJS_CNK_OBJECT draw_492 = {
#include "stg14/terrain/draw_492.inc"
};
static NJS_CNK_OBJECT draw_491 = {
#include "stg14/terrain/draw_491.inc"
};
static NJS_CNK_OBJECT draw_490 = {
#include "stg14/terrain/draw_490.inc"
};
static NJS_CNK_OBJECT draw_489 = {
#include "stg14/terrain/draw_489.inc"
};
static NJS_CNK_OBJECT draw_488 = {
#include "stg14/terrain/draw_488.inc"
};
static NJS_CNK_OBJECT draw_486 = {
#include "stg14/terrain/draw_486.inc"
};
static NJS_CNK_OBJECT draw_485 = {
#include "stg14/terrain/draw_485.inc"
};
static NJS_CNK_OBJECT draw_484 = {
#include "stg14/terrain/draw_484.inc"
};
static NJS_CNK_OBJECT draw_483 = {
#include "stg14/terrain/draw_483.inc"
};
static NJS_CNK_OBJECT draw_482 = {
#include "stg14/terrain/draw_482.inc"
};
static NJS_CNK_OBJECT draw_481 = {
#include "stg14/terrain/draw_481.inc"
};
static NJS_CNK_OBJECT draw_480 = {
#include "stg14/terrain/draw_480.inc"
};
static NJS_CNK_OBJECT draw_479 = {
#include "stg14/terrain/draw_479.inc"
};
static NJS_CNK_OBJECT draw_478 = {
#include "stg14/terrain/draw_478.inc"
};
static NJS_CNK_OBJECT draw_477 = {
#include "stg14/terrain/draw_477.inc"
};
static NJS_CNK_OBJECT draw_476 = {
#include "stg14/terrain/draw_476.inc"
};
static NJS_CNK_OBJECT draw_472 = {
#include "stg14/terrain/draw_472.inc"
};
static NJS_CNK_OBJECT draw_471 = {
#include "stg14/terrain/draw_471.inc"
};
static NJS_CNK_OBJECT draw_470 = {
#include "stg14/terrain/draw_470.inc"
};
static NJS_CNK_OBJECT draw_469 = {
#include "stg14/terrain/draw_469.inc"
};
static NJS_CNK_OBJECT draw_467 = {
#include "stg14/terrain/draw_467.inc"
};
static NJS_CNK_OBJECT draw_466 = {
#include "stg14/terrain/draw_466.inc"
};
static NJS_CNK_OBJECT draw_465 = {
#include "stg14/terrain/draw_465.inc"
};
static NJS_CNK_OBJECT draw_464 = {
#include "stg14/terrain/draw_464.inc"
};
static NJS_CNK_OBJECT draw_463 = {
#include "stg14/terrain/draw_463.inc"
};
static NJS_CNK_OBJECT draw_461 = {
#include "stg14/terrain/draw_461.inc"
};
static NJS_CNK_OBJECT draw_459 = {
#include "stg14/terrain/draw_459.inc"
};
static NJS_CNK_OBJECT draw_458 = {
#include "stg14/terrain/draw_458.inc"
};
static NJS_CNK_OBJECT draw_457 = {
#include "stg14/terrain/draw_457.inc"
};
static NJS_CNK_OBJECT draw_453 = {
#include "stg14/terrain/draw_453.inc"
};
static NJS_CNK_OBJECT draw_452 = {
#include "stg14/terrain/draw_452.inc"
};
static NJS_CNK_OBJECT draw_451 = {
#include "stg14/terrain/draw_451.inc"
};
static NJS_CNK_OBJECT draw_450 = {
#include "stg14/terrain/draw_450.inc"
};
static NJS_CNK_OBJECT draw_449 = {
#include "stg14/terrain/draw_449.inc"
};
static NJS_CNK_OBJECT draw_448 = {
#include "stg14/terrain/draw_448.inc"
};
static NJS_CNK_OBJECT draw_447 = {
#include "stg14/terrain/draw_447.inc"
};
static NJS_CNK_OBJECT draw_445 = {
#include "stg14/terrain/draw_445.inc"
};
static NJS_CNK_OBJECT draw_444 = {
#include "stg14/terrain/draw_444.inc"
};
static NJS_CNK_OBJECT draw_443 = {
#include "stg14/terrain/draw_443.inc"
};
static NJS_CNK_OBJECT draw_442 = {
#include "stg14/terrain/draw_442.inc"
};
static NJS_CNK_OBJECT draw_441 = {
#include "stg14/terrain/draw_441.inc"
};
static NJS_CNK_OBJECT draw_440 = {
#include "stg14/terrain/draw_440.inc"
};
static NJS_CNK_OBJECT draw_439 = {
#include "stg14/terrain/draw_439.inc"
};
static NJS_CNK_OBJECT draw_438 = {
#include "stg14/terrain/draw_438.inc"
};
static NJS_CNK_OBJECT draw_437 = {
#include "stg14/terrain/draw_437.inc"
};
static NJS_CNK_OBJECT draw_436 = {
#include "stg14/terrain/draw_436.inc"
};
static NJS_CNK_OBJECT draw_435 = {
#include "stg14/terrain/draw_435.inc"
};
static NJS_CNK_OBJECT draw_434 = {
#include "stg14/terrain/draw_434.inc"
};
static NJS_CNK_OBJECT draw_433 = {
#include "stg14/terrain/draw_433.inc"
};
static NJS_CNK_OBJECT draw_432 = {
#include "stg14/terrain/draw_432.inc"
};
static NJS_CNK_OBJECT draw_431 = {
#include "stg14/terrain/draw_431.inc"
};
static NJS_CNK_OBJECT draw_428 = {
#include "stg14/terrain/draw_428.inc"
};
static NJS_CNK_OBJECT draw_427 = {
#include "stg14/terrain/draw_427.inc"
};
static NJS_CNK_OBJECT draw_425 = {
#include "stg14/terrain/draw_425.inc"
};
static NJS_CNK_OBJECT draw_424 = {
#include "stg14/terrain/draw_424.inc"
};
static NJS_CNK_OBJECT draw_421 = {
#include "stg14/terrain/draw_421.inc"
};
static NJS_CNK_OBJECT draw_420 = {
#include "stg14/terrain/draw_420.inc"
};
static NJS_CNK_OBJECT draw_419 = {
#include "stg14/terrain/draw_419.inc"
};
static NJS_CNK_OBJECT draw_418 = {
#include "stg14/terrain/draw_418.inc"
};
static NJS_CNK_OBJECT draw_417 = {
#include "stg14/terrain/draw_417.inc"
};
static NJS_CNK_OBJECT draw_416 = {
#include "stg14/terrain/draw_416.inc"
};
static NJS_CNK_OBJECT draw_415 = {
#include "stg14/terrain/draw_415.inc"
};
static NJS_CNK_OBJECT draw_414 = {
#include "stg14/terrain/draw_414.inc"
};
static NJS_CNK_OBJECT draw_413 = {
#include "stg14/terrain/draw_413.inc"
};
static NJS_CNK_OBJECT draw_412 = {
#include "stg14/terrain/draw_412.inc"
};
static NJS_CNK_OBJECT draw_411 = {
#include "stg14/terrain/draw_411.inc"
};
static NJS_CNK_OBJECT draw_410 = {
#include "stg14/terrain/draw_410.inc"
};
static NJS_CNK_OBJECT draw_409 = {
#include "stg14/terrain/draw_409.inc"
};
static NJS_CNK_OBJECT draw_408 = {
#include "stg14/terrain/draw_408.inc"
};
static NJS_CNK_OBJECT draw_407 = {
#include "stg14/terrain/draw_407.inc"
};
static NJS_CNK_OBJECT draw_406 = {
#include "stg14/terrain/draw_406.inc"
};
static NJS_CNK_OBJECT draw_405 = {
#include "stg14/terrain/draw_405.inc"
};
static NJS_CNK_OBJECT draw_403 = {
#include "stg14/terrain/draw_403.inc"
};
static NJS_CNK_OBJECT draw_402 = {
#include "stg14/terrain/draw_402.inc"
};
static NJS_CNK_OBJECT draw_401 = {
#include "stg14/terrain/draw_401.inc"
};
static NJS_CNK_OBJECT draw_400 = {
#include "stg14/terrain/draw_400.inc"
};
static NJS_CNK_OBJECT draw_399 = {
#include "stg14/terrain/draw_399.inc"
};
static NJS_CNK_OBJECT draw_398 = {
#include "stg14/terrain/draw_398.inc"
};
static NJS_CNK_OBJECT draw_397 = {
#include "stg14/terrain/draw_397.inc"
};
static NJS_CNK_OBJECT draw_396 = {
#include "stg14/terrain/draw_396.inc"
};
static NJS_CNK_OBJECT draw_395 = {
#include "stg14/terrain/draw_395.inc"
};
static NJS_CNK_OBJECT draw_394 = {
#include "stg14/terrain/draw_394.inc"
};
static NJS_CNK_OBJECT draw_393 = {
#include "stg14/terrain/draw_393.inc"
};
static NJS_CNK_OBJECT draw_392 = {
#include "stg14/terrain/draw_392.inc"
};
static NJS_CNK_OBJECT draw_391 = {
#include "stg14/terrain/draw_391.inc"
};
static NJS_CNK_OBJECT draw_390 = {
#include "stg14/terrain/draw_390.inc"
};
static NJS_CNK_OBJECT draw_389 = {
#include "stg14/terrain/draw_389.inc"
};
static NJS_CNK_OBJECT draw_388 = {
#include "stg14/terrain/draw_388.inc"
};
static NJS_CNK_OBJECT draw_387 = {
#include "stg14/terrain/draw_387.inc"
};
static NJS_CNK_OBJECT draw_386 = {
#include "stg14/terrain/draw_386.inc"
};
static NJS_CNK_OBJECT draw_385 = {
#include "stg14/terrain/draw_385.inc"
};
static NJS_CNK_OBJECT draw_384 = {
#include "stg14/terrain/draw_384.inc"
};
static NJS_CNK_OBJECT draw_383 = {
#include "stg14/terrain/draw_383.inc"
};
static NJS_CNK_OBJECT draw_382 = {
#include "stg14/terrain/draw_382.inc"
};
static NJS_CNK_OBJECT draw_381 = {
#include "stg14/terrain/draw_381.inc"
};
static NJS_CNK_OBJECT draw_380 = {
#include "stg14/terrain/draw_380.inc"
};
static NJS_CNK_OBJECT draw_379 = {
#include "stg14/terrain/draw_379.inc"
};
static NJS_CNK_OBJECT draw_378 = {
#include "stg14/terrain/draw_378.inc"
};
static NJS_CNK_OBJECT draw_377 = {
#include "stg14/terrain/draw_377.inc"
};
static NJS_CNK_OBJECT draw_376 = {
#include "stg14/terrain/draw_376.inc"
};
static NJS_CNK_OBJECT draw_375 = {
#include "stg14/terrain/draw_375.inc"
};
static NJS_CNK_OBJECT draw_374 = {
#include "stg14/terrain/draw_374.inc"
};
static NJS_CNK_OBJECT draw_373 = {
#include "stg14/terrain/draw_373.inc"
};
static NJS_CNK_OBJECT draw_372 = {
#include "stg14/terrain/draw_372.inc"
};
static NJS_CNK_OBJECT draw_371 = {
#include "stg14/terrain/draw_371.inc"
};
static NJS_CNK_OBJECT draw_370 = {
#include "stg14/terrain/draw_370.inc"
};
static NJS_CNK_OBJECT draw_369 = {
#include "stg14/terrain/draw_369.inc"
};
static NJS_CNK_OBJECT draw_368 = {
#include "stg14/terrain/draw_368.inc"
};
static NJS_CNK_OBJECT draw_367 = {
#include "stg14/terrain/draw_367.inc"
};
static NJS_CNK_OBJECT draw_366 = {
#include "stg14/terrain/draw_366.inc"
};
static NJS_CNK_OBJECT draw_365 = {
#include "stg14/terrain/draw_365.inc"
};
static NJS_CNK_OBJECT draw_364 = {
#include "stg14/terrain/draw_364.inc"
};
static NJS_CNK_OBJECT draw_363 = {
#include "stg14/terrain/draw_363.inc"
};
static NJS_CNK_OBJECT draw_362 = {
#include "stg14/terrain/draw_362.inc"
};
static NJS_CNK_OBJECT draw_361 = {
#include "stg14/terrain/draw_361.inc"
};
static NJS_CNK_OBJECT draw_360 = {
#include "stg14/terrain/draw_360.inc"
};
static NJS_CNK_OBJECT draw_359 = {
#include "stg14/terrain/draw_359.inc"
};
static NJS_CNK_OBJECT draw_358 = {
#include "stg14/terrain/draw_358.inc"
};
static NJS_CNK_OBJECT draw_357 = {
#include "stg14/terrain/draw_357.inc"
};
static NJS_CNK_OBJECT draw_354 = {
#include "stg14/terrain/draw_354.inc"
};
static NJS_CNK_OBJECT draw_353 = {
#include "stg14/terrain/draw_353.inc"
};
static NJS_CNK_OBJECT draw_352 = {
#include "stg14/terrain/draw_352.inc"
};
static NJS_CNK_OBJECT draw_351 = {
#include "stg14/terrain/draw_351.inc"
};
static NJS_CNK_OBJECT draw_350 = {
#include "stg14/terrain/draw_350.inc"
};
static NJS_CNK_OBJECT draw_349 = {
#include "stg14/terrain/draw_349.inc"
};
static NJS_CNK_OBJECT draw_348 = {
#include "stg14/terrain/draw_348.inc"
};
static NJS_CNK_OBJECT draw_347 = {
#include "stg14/terrain/draw_347.inc"
};
static NJS_CNK_OBJECT draw_346 = {
#include "stg14/terrain/draw_346.inc"
};
static NJS_CNK_OBJECT draw_345 = {
#include "stg14/terrain/draw_345.inc"
};
static NJS_CNK_OBJECT draw_344 = {
#include "stg14/terrain/draw_344.inc"
};
static NJS_CNK_OBJECT draw_343 = {
#include "stg14/terrain/draw_343.inc"
};
static NJS_CNK_OBJECT draw_342 = {
#include "stg14/terrain/draw_342.inc"
};
static NJS_CNK_OBJECT draw_341 = {
#include "stg14/terrain/draw_341.inc"
};
static NJS_CNK_OBJECT draw_340 = {
#include "stg14/terrain/draw_340.inc"
};
static NJS_CNK_OBJECT draw_339 = {
#include "stg14/terrain/draw_339.inc"
};
static NJS_CNK_OBJECT draw_337 = {
#include "stg14/terrain/draw_337.inc"
};
static NJS_CNK_OBJECT draw_336 = {
#include "stg14/terrain/draw_336.inc"
};
static NJS_CNK_OBJECT draw_335 = {
#include "stg14/terrain/draw_335.inc"
};
static NJS_CNK_OBJECT draw_334 = {
#include "stg14/terrain/draw_334.inc"
};
static NJS_CNK_OBJECT draw_333 = {
#include "stg14/terrain/draw_333.inc"
};
static NJS_CNK_OBJECT draw_332 = {
#include "stg14/terrain/draw_332.inc"
};
static NJS_CNK_OBJECT draw_331 = {
#include "stg14/terrain/draw_331.inc"
};
static NJS_CNK_OBJECT draw_330 = {
#include "stg14/terrain/draw_330.inc"
};
static NJS_CNK_OBJECT draw_329 = {
#include "stg14/terrain/draw_329.inc"
};
static NJS_CNK_OBJECT draw_328 = {
#include "stg14/terrain/draw_328.inc"
};
static NJS_CNK_OBJECT draw_327 = {
#include "stg14/terrain/draw_327.inc"
};
static NJS_CNK_OBJECT draw_326 = {
#include "stg14/terrain/draw_326.inc"
};
static NJS_CNK_OBJECT draw_325 = {
#include "stg14/terrain/draw_325.inc"
};
static NJS_CNK_OBJECT draw_324 = {
#include "stg14/terrain/draw_324.inc"
};
static NJS_CNK_OBJECT draw_323 = {
#include "stg14/terrain/draw_323.inc"
};
static NJS_CNK_OBJECT draw_322 = {
#include "stg14/terrain/draw_322.inc"
};
static NJS_CNK_OBJECT draw_321 = {
#include "stg14/terrain/draw_321.inc"
};
static NJS_CNK_OBJECT draw_320 = {
#include "stg14/terrain/draw_320.inc"
};
static NJS_CNK_OBJECT draw_319 = {
#include "stg14/terrain/draw_319.inc"
};
static NJS_CNK_OBJECT draw_318 = {
#include "stg14/terrain/draw_318.inc"
};
static NJS_CNK_OBJECT draw_317 = {
#include "stg14/terrain/draw_317.inc"
};
static NJS_CNK_OBJECT draw_316 = {
#include "stg14/terrain/draw_316.inc"
};
static NJS_CNK_OBJECT draw_315 = {
#include "stg14/terrain/draw_315.inc"
};
static NJS_CNK_OBJECT draw_314 = {
#include "stg14/terrain/draw_314.inc"
};
static NJS_CNK_OBJECT draw_312 = {
#include "stg14/terrain/draw_312.inc"
};
static NJS_CNK_OBJECT draw_311 = {
#include "stg14/terrain/draw_311.inc"
};
static NJS_CNK_OBJECT draw_310 = {
#include "stg14/terrain/draw_310.inc"
};
static NJS_CNK_OBJECT draw_309 = {
#include "stg14/terrain/draw_309.inc"
};
static NJS_CNK_OBJECT draw_308 = {
#include "stg14/terrain/draw_308.inc"
};
static NJS_CNK_OBJECT draw_307 = {
#include "stg14/terrain/draw_307.inc"
};
static NJS_CNK_OBJECT draw_306 = {
#include "stg14/terrain/draw_306.inc"
};
static NJS_CNK_OBJECT draw_305 = {
#include "stg14/terrain/draw_305.inc"
};
static NJS_CNK_OBJECT draw_304 = {
#include "stg14/terrain/draw_304.inc"
};
static NJS_CNK_OBJECT draw_303 = {
#include "stg14/terrain/draw_303.inc"
};
static NJS_CNK_OBJECT draw_302 = {
#include "stg14/terrain/draw_302.inc"
};
static NJS_CNK_OBJECT draw_301 = {
#include "stg14/terrain/draw_301.inc"
};
static NJS_CNK_OBJECT draw_300 = {
#include "stg14/terrain/draw_300.inc"
};
static NJS_CNK_OBJECT draw_299 = {
#include "stg14/terrain/draw_299.inc"
};
static NJS_CNK_OBJECT draw_298 = {
#include "stg14/terrain/draw_298.inc"
};
static NJS_CNK_OBJECT draw_297 = {
#include "stg14/terrain/draw_297.inc"
};
static NJS_CNK_OBJECT draw_296 = {
#include "stg14/terrain/draw_296.inc"
};
static NJS_CNK_OBJECT draw_295 = {
#include "stg14/terrain/draw_295.inc"
};
static NJS_CNK_OBJECT draw_294 = {
#include "stg14/terrain/draw_294.inc"
};
static NJS_CNK_OBJECT draw_293 = {
#include "stg14/terrain/draw_293.inc"
};
static NJS_CNK_OBJECT draw_292 = {
#include "stg14/terrain/draw_292.inc"
};
static NJS_CNK_OBJECT draw_291 = {
#include "stg14/terrain/draw_291.inc"
};
static NJS_CNK_OBJECT draw_290 = {
#include "stg14/terrain/draw_290.inc"
};
static NJS_CNK_OBJECT draw_289 = {
#include "stg14/terrain/draw_289.inc"
};
static NJS_CNK_OBJECT draw_288 = {
#include "stg14/terrain/draw_288.inc"
};
static NJS_CNK_OBJECT draw_287 = {
#include "stg14/terrain/draw_287.inc"
};
static NJS_CNK_OBJECT draw_286 = {
#include "stg14/terrain/draw_286.inc"
};
static NJS_CNK_OBJECT draw_285 = {
#include "stg14/terrain/draw_285.inc"
};
static NJS_CNK_OBJECT draw_284 = {
#include "stg14/terrain/draw_284.inc"
};
static NJS_CNK_OBJECT draw_283 = {
#include "stg14/terrain/draw_283.inc"
};
static NJS_CNK_OBJECT draw_282 = {
#include "stg14/terrain/draw_282.inc"
};
static NJS_CNK_OBJECT draw_281 = {
#include "stg14/terrain/draw_281.inc"
};
static NJS_CNK_OBJECT draw_280 = {
#include "stg14/terrain/draw_280.inc"
};
static NJS_CNK_OBJECT draw_279 = {
#include "stg14/terrain/draw_279.inc"
};
static NJS_CNK_OBJECT draw_278 = {
#include "stg14/terrain/draw_278.inc"
};
static NJS_CNK_OBJECT draw_277 = {
#include "stg14/terrain/draw_277.inc"
};
static NJS_CNK_OBJECT draw_276 = {
#include "stg14/terrain/draw_276.inc"
};
static NJS_CNK_OBJECT draw_275 = {
#include "stg14/terrain/draw_275.inc"
};
static NJS_CNK_OBJECT draw_274 = {
#include "stg14/terrain/draw_274.inc"
};
static NJS_CNK_OBJECT draw_273 = {
#include "stg14/terrain/draw_273.inc"
};
static NJS_CNK_OBJECT draw_272 = {
#include "stg14/terrain/draw_272.inc"
};
static NJS_CNK_OBJECT draw_271 = {
#include "stg14/terrain/draw_271.inc"
};
static NJS_CNK_OBJECT draw_270 = {
#include "stg14/terrain/draw_270.inc"
};
static NJS_CNK_OBJECT draw_269 = {
#include "stg14/terrain/draw_269.inc"
};
static NJS_CNK_OBJECT draw_268 = {
#include "stg14/terrain/draw_268.inc"
};
static NJS_CNK_OBJECT draw_267 = {
#include "stg14/terrain/draw_267.inc"
};
static NJS_CNK_OBJECT draw_266 = {
#include "stg14/terrain/draw_266.inc"
};
static NJS_CNK_OBJECT draw_265 = {
#include "stg14/terrain/draw_265.inc"
};
static NJS_CNK_OBJECT draw_264 = {
#include "stg14/terrain/draw_264.inc"
};
static NJS_CNK_OBJECT draw_263 = {
#include "stg14/terrain/draw_263.inc"
};
static NJS_CNK_OBJECT draw_262 = {
#include "stg14/terrain/draw_262.inc"
};
static NJS_CNK_OBJECT draw_261 = {
#include "stg14/terrain/draw_261.inc"
};
static NJS_CNK_OBJECT draw_260 = {
#include "stg14/terrain/draw_260.inc"
};
static NJS_CNK_OBJECT draw_258 = {
#include "stg14/terrain/draw_258.inc"
};
static NJS_CNK_OBJECT draw_257 = {
#include "stg14/terrain/draw_257.inc"
};
static NJS_CNK_OBJECT draw_256 = {
#include "stg14/terrain/draw_256.inc"
};
static NJS_CNK_OBJECT draw_255 = {
#include "stg14/terrain/draw_255.inc"
};
static NJS_CNK_OBJECT draw_254 = {
#include "stg14/terrain/draw_254.inc"
};
static NJS_CNK_OBJECT draw_253 = {
#include "stg14/terrain/draw_253.inc"
};
static NJS_CNK_OBJECT draw_252 = {
#include "stg14/terrain/draw_252.inc"
};
static NJS_CNK_OBJECT draw_251 = {
#include "stg14/terrain/draw_251.inc"
};
static NJS_CNK_OBJECT draw_249 = {
#include "stg14/terrain/draw_249.inc"
};
static NJS_CNK_OBJECT draw_248 = {
#include "stg14/terrain/draw_248.inc"
};
static NJS_CNK_OBJECT draw_247 = {
#include "stg14/terrain/draw_247.inc"
};
static NJS_CNK_OBJECT draw_246 = {
#include "stg14/terrain/draw_246.inc"
};
static NJS_CNK_OBJECT draw_245 = {
#include "stg14/terrain/draw_245.inc"
};
static NJS_CNK_OBJECT draw_244 = {
#include "stg14/terrain/draw_244.inc"
};
static NJS_CNK_OBJECT draw_243 = {
#include "stg14/terrain/draw_243.inc"
};
static NJS_CNK_OBJECT draw_242 = {
#include "stg14/terrain/draw_242.inc"
};
static NJS_CNK_OBJECT draw_241 = {
#include "stg14/terrain/draw_241.inc"
};
static NJS_CNK_OBJECT draw_240 = {
#include "stg14/terrain/draw_240.inc"
};
static NJS_CNK_OBJECT draw_239 = {
#include "stg14/terrain/draw_239.inc"
};
static NJS_CNK_OBJECT draw_238 = {
#include "stg14/terrain/draw_238.inc"
};
static NJS_CNK_OBJECT draw_237 = {
#include "stg14/terrain/draw_237.inc"
};
static NJS_CNK_OBJECT draw_236 = {
#include "stg14/terrain/draw_236.inc"
};
static NJS_CNK_OBJECT draw_235 = {
#include "stg14/terrain/draw_235.inc"
};
static NJS_CNK_OBJECT draw_234 = {
#include "stg14/terrain/draw_234.inc"
};
static NJS_CNK_OBJECT draw_233 = {
#include "stg14/terrain/draw_233.inc"
};
static NJS_CNK_OBJECT draw_232 = {
#include "stg14/terrain/draw_232.inc"
};
static NJS_CNK_OBJECT draw_231 = {
#include "stg14/terrain/draw_231.inc"
};
static NJS_CNK_OBJECT draw_230 = {
#include "stg14/terrain/draw_230.inc"
};
static NJS_CNK_OBJECT draw_229 = {
#include "stg14/terrain/draw_229.inc"
};
static NJS_CNK_OBJECT draw_228 = {
#include "stg14/terrain/draw_228.inc"
};
static NJS_CNK_OBJECT draw_227 = {
#include "stg14/terrain/draw_227.inc"
};
static NJS_CNK_OBJECT draw_226 = {
#include "stg14/terrain/draw_226.inc"
};
static NJS_CNK_OBJECT draw_225 = {
#include "stg14/terrain/draw_225.inc"
};
static NJS_CNK_OBJECT draw_224 = {
#include "stg14/terrain/draw_224.inc"
};
static NJS_CNK_OBJECT draw_223 = {
#include "stg14/terrain/draw_223.inc"
};
static NJS_CNK_OBJECT draw_222 = {
#include "stg14/terrain/draw_222.inc"
};
static NJS_CNK_OBJECT draw_221 = {
#include "stg14/terrain/draw_221.inc"
};
static NJS_CNK_OBJECT draw_220 = {
#include "stg14/terrain/draw_220.inc"
};
static NJS_CNK_OBJECT draw_219 = {
#include "stg14/terrain/draw_219.inc"
};
static NJS_CNK_OBJECT draw_217 = {
#include "stg14/terrain/draw_217.inc"
};
static NJS_CNK_OBJECT draw_216 = {
#include "stg14/terrain/draw_216.inc"
};
static NJS_CNK_OBJECT draw_215 = {
#include "stg14/terrain/draw_215.inc"
};
static NJS_CNK_OBJECT draw_214 = {
#include "stg14/terrain/draw_214.inc"
};
static NJS_CNK_OBJECT draw_213 = {
#include "stg14/terrain/draw_213.inc"
};
static NJS_CNK_OBJECT draw_212 = {
#include "stg14/terrain/draw_212.inc"
};
static NJS_CNK_OBJECT draw_211 = {
#include "stg14/terrain/draw_211.inc"
};
static NJS_CNK_OBJECT draw_210 = {
#include "stg14/terrain/draw_210.inc"
};
static NJS_CNK_OBJECT draw_209 = {
#include "stg14/terrain/draw_209.inc"
};
static NJS_CNK_OBJECT draw_208 = {
#include "stg14/terrain/draw_208.inc"
};
static NJS_CNK_OBJECT draw_207 = {
#include "stg14/terrain/draw_207.inc"
};
static NJS_CNK_OBJECT draw_206 = {
#include "stg14/terrain/draw_206.inc"
};
static NJS_CNK_OBJECT draw_205 = {
#include "stg14/terrain/draw_205.inc"
};
static NJS_CNK_OBJECT draw_204 = {
#include "stg14/terrain/draw_204.inc"
};
static NJS_CNK_OBJECT draw_203 = {
#include "stg14/terrain/draw_203.inc"
};
static NJS_CNK_OBJECT draw_202 = {
#include "stg14/terrain/draw_202.inc"
};
static NJS_CNK_OBJECT draw_201 = {
#include "stg14/terrain/draw_201.inc"
};
static NJS_CNK_OBJECT draw_200 = {
#include "stg14/terrain/draw_200.inc"
};
static NJS_CNK_OBJECT draw_199 = {
#include "stg14/terrain/draw_199.inc"
};
static NJS_CNK_OBJECT draw_198 = {
#include "stg14/terrain/draw_198.inc"
};
static NJS_CNK_OBJECT draw_197 = {
#include "stg14/terrain/draw_197.inc"
};
static NJS_CNK_OBJECT draw_196 = {
#include "stg14/terrain/draw_196.inc"
};
static NJS_CNK_OBJECT draw_195 = {
#include "stg14/terrain/draw_195.inc"
};
static NJS_CNK_OBJECT draw_193 = {
#include "stg14/terrain/draw_193.inc"
};
static NJS_CNK_OBJECT draw_192 = {
#include "stg14/terrain/draw_192.inc"
};
static NJS_CNK_OBJECT draw_191 = {
#include "stg14/terrain/draw_191.inc"
};
static NJS_CNK_OBJECT draw_190 = {
#include "stg14/terrain/draw_190.inc"
};
static NJS_CNK_OBJECT draw_189 = {
#include "stg14/terrain/draw_189.inc"
};
static NJS_CNK_OBJECT draw_188 = {
#include "stg14/terrain/draw_188.inc"
};
static NJS_CNK_OBJECT draw_187 = {
#include "stg14/terrain/draw_187.inc"
};
static NJS_CNK_OBJECT draw_186 = {
#include "stg14/terrain/draw_186.inc"
};
static NJS_CNK_OBJECT draw_184 = {
#include "stg14/terrain/draw_184.inc"
};
static NJS_CNK_OBJECT draw_183 = {
#include "stg14/terrain/draw_183.inc"
};
static NJS_CNK_OBJECT draw_182 = {
#include "stg14/terrain/draw_182.inc"
};
static NJS_CNK_OBJECT draw_181 = {
#include "stg14/terrain/draw_181.inc"
};
static NJS_CNK_OBJECT draw_180 = {
#include "stg14/terrain/draw_180.inc"
};
static NJS_CNK_OBJECT draw_179 = {
#include "stg14/terrain/draw_179.inc"
};
static NJS_CNK_OBJECT draw_178 = {
#include "stg14/terrain/draw_178.inc"
};
static NJS_CNK_OBJECT draw_177 = {
#include "stg14/terrain/draw_177.inc"
};
static NJS_CNK_OBJECT draw_176 = {
#include "stg14/terrain/draw_176.inc"
};
static NJS_CNK_OBJECT draw_175 = {
#include "stg14/terrain/draw_175.inc"
};
static NJS_CNK_OBJECT draw_174 = {
#include "stg14/terrain/draw_174.inc"
};
static NJS_CNK_OBJECT draw_173 = {
#include "stg14/terrain/draw_173.inc"
};
static NJS_CNK_OBJECT draw_172 = {
#include "stg14/terrain/draw_172.inc"
};
static NJS_CNK_OBJECT draw_171 = {
#include "stg14/terrain/draw_171.inc"
};
static NJS_CNK_OBJECT draw_170 = {
#include "stg14/terrain/draw_170.inc"
};
static NJS_CNK_OBJECT draw_169 = {
#include "stg14/terrain/draw_169.inc"
};
static NJS_CNK_OBJECT draw_168 = {
#include "stg14/terrain/draw_168.inc"
};
static NJS_CNK_OBJECT draw_167 = {
#include "stg14/terrain/draw_167.inc"
};
static NJS_CNK_OBJECT draw_166 = {
#include "stg14/terrain/draw_166.inc"
};
static NJS_CNK_OBJECT draw_165 = {
#include "stg14/terrain/draw_165.inc"
};
static NJS_CNK_OBJECT draw_164 = {
#include "stg14/terrain/draw_164.inc"
};
static NJS_CNK_OBJECT draw_163 = {
#include "stg14/terrain/draw_163.inc"
};
static NJS_CNK_OBJECT draw_162 = {
#include "stg14/terrain/draw_162.inc"
};
static NJS_CNK_OBJECT draw_161 = {
#include "stg14/terrain/draw_161.inc"
};
static NJS_CNK_OBJECT draw_160 = {
#include "stg14/terrain/draw_160.inc"
};
static NJS_CNK_OBJECT draw_159 = {
#include "stg14/terrain/draw_159.inc"
};
static NJS_CNK_OBJECT draw_158 = {
#include "stg14/terrain/draw_158.inc"
};
static NJS_CNK_OBJECT draw_157 = {
#include "stg14/terrain/draw_157.inc"
};
static NJS_CNK_OBJECT draw_156 = {
#include "stg14/terrain/draw_156.inc"
};
static NJS_CNK_OBJECT draw_155 = {
#include "stg14/terrain/draw_155.inc"
};
static NJS_CNK_OBJECT draw_154 = {
#include "stg14/terrain/draw_154.inc"
};
static NJS_CNK_OBJECT draw_153 = {
#include "stg14/terrain/draw_153.inc"
};
static NJS_CNK_OBJECT draw_150 = {
#include "stg14/terrain/draw_150.inc"
};
static NJS_CNK_OBJECT draw_148 = {
#include "stg14/terrain/draw_148.inc"
};
static NJS_CNK_OBJECT draw_147 = {
#include "stg14/terrain/draw_147.inc"
};
static NJS_CNK_OBJECT draw_146 = {
#include "stg14/terrain/draw_146.inc"
};
static NJS_CNK_OBJECT draw_145 = {
#include "stg14/terrain/draw_145.inc"
};
static NJS_CNK_OBJECT draw_144 = {
#include "stg14/terrain/draw_144.inc"
};
static NJS_CNK_OBJECT draw_143 = {
#include "stg14/terrain/draw_143.inc"
};
static NJS_CNK_OBJECT draw_141 = {
#include "stg14/terrain/draw_141.inc"
};
static NJS_CNK_OBJECT draw_140 = {
#include "stg14/terrain/draw_140.inc"
};
static NJS_CNK_OBJECT draw_139 = {
#include "stg14/terrain/draw_139.inc"
};
static NJS_CNK_OBJECT draw_138 = {
#include "stg14/terrain/draw_138.inc"
};
static NJS_CNK_OBJECT draw_137 = {
#include "stg14/terrain/draw_137.inc"
};
static NJS_CNK_OBJECT draw_136 = {
#include "stg14/terrain/draw_136.inc"
};
static NJS_CNK_OBJECT draw_135 = {
#include "stg14/terrain/draw_135.inc"
};
static NJS_CNK_OBJECT draw_133 = {
#include "stg14/terrain/draw_133.inc"
};
static NJS_CNK_OBJECT draw_132 = {
#include "stg14/terrain/draw_132.inc"
};
static NJS_CNK_OBJECT draw_131 = {
#include "stg14/terrain/draw_131.inc"
};
static NJS_CNK_OBJECT draw_130 = {
#include "stg14/terrain/draw_130.inc"
};
static NJS_CNK_OBJECT draw_129 = {
#include "stg14/terrain/draw_129.inc"
};
static NJS_CNK_OBJECT draw_128 = {
#include "stg14/terrain/draw_128.inc"
};
static NJS_CNK_OBJECT draw_127 = {
#include "stg14/terrain/draw_127.inc"
};
static NJS_CNK_OBJECT draw_126 = {
#include "stg14/terrain/draw_126.inc"
};
static NJS_CNK_OBJECT draw_125 = {
#include "stg14/terrain/draw_125.inc"
};
static NJS_CNK_OBJECT draw_124 = {
#include "stg14/terrain/draw_124.inc"
};
static NJS_CNK_OBJECT draw_123 = {
#include "stg14/terrain/draw_123.inc"
};
static NJS_CNK_OBJECT draw_122 = {
#include "stg14/terrain/draw_122.inc"
};
static NJS_CNK_OBJECT draw_121 = {
#include "stg14/terrain/draw_121.inc"
};
static NJS_CNK_OBJECT draw_120 = {
#include "stg14/terrain/draw_120.inc"
};
static NJS_CNK_OBJECT draw_119 = {
#include "stg14/terrain/draw_119.inc"
};
static NJS_CNK_OBJECT draw_118 = {
#include "stg14/terrain/draw_118.inc"
};
static NJS_CNK_OBJECT draw_117 = {
#include "stg14/terrain/draw_117.inc"
};
static NJS_CNK_OBJECT draw_116 = {
#include "stg14/terrain/draw_116.inc"
};
static NJS_CNK_OBJECT draw_115 = {
#include "stg14/terrain/draw_115.inc"
};
static NJS_CNK_OBJECT draw_114 = {
#include "stg14/terrain/draw_114.inc"
};
static NJS_CNK_OBJECT draw_113 = {
#include "stg14/terrain/draw_113.inc"
};
static NJS_CNK_OBJECT draw_112 = {
#include "stg14/terrain/draw_112.inc"
};
static NJS_CNK_OBJECT draw_111 = {
#include "stg14/terrain/draw_111.inc"
};
static NJS_CNK_OBJECT draw_110 = {
#include "stg14/terrain/draw_110.inc"
};
static NJS_CNK_OBJECT draw_109 = {
#include "stg14/terrain/draw_109.inc"
};
static NJS_CNK_OBJECT draw_108 = {
#include "stg14/terrain/draw_108.inc"
};
static NJS_CNK_OBJECT draw_107 = {
#include "stg14/terrain/draw_107.inc"
};
static NJS_CNK_OBJECT draw_106 = {
#include "stg14/terrain/draw_106.inc"
};
static NJS_CNK_OBJECT draw_105 = {
#include "stg14/terrain/draw_105.inc"
};
static NJS_CNK_OBJECT draw_104 = {
#include "stg14/terrain/draw_104.inc"
};
static NJS_CNK_OBJECT draw_103 = {
#include "stg14/terrain/draw_103.inc"
};
static NJS_CNK_OBJECT draw_102 = {
#include "stg14/terrain/draw_102.inc"
};
static NJS_CNK_OBJECT draw_101 = {
#include "stg14/terrain/draw_101.inc"
};
static NJS_CNK_OBJECT draw_100 = {
#include "stg14/terrain/draw_100.inc"
};
static NJS_CNK_OBJECT draw_99 = {
#include "stg14/terrain/draw_99.inc"
};
static NJS_CNK_OBJECT draw_98 = {
#include "stg14/terrain/draw_98.inc"
};
static NJS_CNK_OBJECT draw_97 = {
#include "stg14/terrain/draw_97.inc"
};
static NJS_CNK_OBJECT draw_96 = {
#include "stg14/terrain/draw_96.inc"
};
static NJS_CNK_OBJECT draw_95 = {
#include "stg14/terrain/draw_95.inc"
};
static NJS_CNK_OBJECT draw_94 = {
#include "stg14/terrain/draw_94.inc"
};
static NJS_CNK_OBJECT draw_93 = {
#include "stg14/terrain/draw_93.inc"
};
static NJS_CNK_OBJECT draw_92 = {
#include "stg14/terrain/draw_92.inc"
};
static NJS_CNK_OBJECT draw_91 = {
#include "stg14/terrain/draw_91.inc"
};
static NJS_CNK_OBJECT draw_90 = {
#include "stg14/terrain/draw_90.inc"
};
static NJS_CNK_OBJECT draw_89 = {
#include "stg14/terrain/draw_89.inc"
};
static NJS_CNK_OBJECT draw_88 = {
#include "stg14/terrain/draw_88.inc"
};
static NJS_CNK_OBJECT draw_87 = {
#include "stg14/terrain/draw_87.inc"
};
static NJS_CNK_OBJECT draw_86 = {
#include "stg14/terrain/draw_86.inc"
};
static NJS_CNK_OBJECT draw_85 = {
#include "stg14/terrain/draw_85.inc"
};
static NJS_CNK_OBJECT draw_84 = {
#include "stg14/terrain/draw_84.inc"
};
static NJS_CNK_OBJECT draw_83 = {
#include "stg14/terrain/draw_83.inc"
};
static NJS_CNK_OBJECT draw_82 = {
#include "stg14/terrain/draw_82.inc"
};
static NJS_CNK_OBJECT draw_81 = {
#include "stg14/terrain/draw_81.inc"
};
static NJS_CNK_OBJECT draw_80 = {
#include "stg14/terrain/draw_80.inc"
};
static NJS_CNK_OBJECT draw_79 = {
#include "stg14/terrain/draw_79.inc"
};
static NJS_CNK_OBJECT draw_78 = {
#include "stg14/terrain/draw_78.inc"
};
static NJS_CNK_OBJECT draw_77 = {
#include "stg14/terrain/draw_77.inc"
};
static NJS_CNK_OBJECT draw_76 = {
#include "stg14/terrain/draw_76.inc"
};
static NJS_CNK_OBJECT draw_75 = {
#include "stg14/terrain/draw_75.inc"
};
static NJS_CNK_OBJECT draw_74 = {
#include "stg14/terrain/draw_74.inc"
};
static NJS_CNK_OBJECT draw_73 = {
#include "stg14/terrain/draw_73.inc"
};
static NJS_CNK_OBJECT draw_71 = {
#include "stg14/terrain/draw_71.inc"
};
static NJS_CNK_OBJECT draw_70 = {
#include "stg14/terrain/draw_70.inc"
};
static NJS_CNK_OBJECT draw_69 = {
#include "stg14/terrain/draw_69.inc"
};
static NJS_CNK_OBJECT draw_68 = {
#include "stg14/terrain/draw_68.inc"
};
static NJS_CNK_OBJECT draw_67 = {
#include "stg14/terrain/draw_67.inc"
};
static NJS_CNK_OBJECT draw_63 = {
#include "stg14/terrain/draw_63.inc"
};
static NJS_CNK_OBJECT draw_62 = {
#include "stg14/terrain/draw_62.inc"
};
static NJS_CNK_OBJECT draw_61 = {
#include "stg14/terrain/draw_61.inc"
};
static NJS_CNK_OBJECT draw_60 = {
#include "stg14/terrain/draw_60.inc"
};
static NJS_CNK_OBJECT draw_59 = {
#include "stg14/terrain/draw_59.inc"
};
static NJS_CNK_OBJECT draw_57 = {
#include "stg14/terrain/draw_57.inc"
};
static NJS_CNK_OBJECT draw_54 = {
#include "stg14/terrain/draw_54.inc"
};
static NJS_CNK_OBJECT draw_53 = {
#include "stg14/terrain/draw_53.inc"
};
static NJS_CNK_OBJECT draw_52 = {
#include "stg14/terrain/draw_52.inc"
};
static NJS_CNK_OBJECT draw_51 = {
#include "stg14/terrain/draw_51.inc"
};
static NJS_CNK_OBJECT draw_50 = {
#include "stg14/terrain/draw_50.inc"
};
static NJS_CNK_OBJECT draw_49 = {
#include "stg14/terrain/draw_49.inc"
};
static NJS_CNK_OBJECT draw_48 = {
#include "stg14/terrain/draw_48.inc"
};
static NJS_CNK_OBJECT draw_47 = {
#include "stg14/terrain/draw_47.inc"
};
static NJS_CNK_OBJECT draw_46 = {
#include "stg14/terrain/draw_46.inc"
};
static NJS_CNK_OBJECT draw_45 = {
#include "stg14/terrain/draw_45.inc"
};
static NJS_CNK_OBJECT draw_44 = {
#include "stg14/terrain/draw_44.inc"
};
static NJS_CNK_OBJECT draw_43 = {
#include "stg14/terrain/draw_43.inc"
};
static NJS_CNK_OBJECT draw_42 = {
#include "stg14/terrain/draw_42.inc"
};
static NJS_CNK_OBJECT draw_41 = {
#include "stg14/terrain/draw_41.inc"
};
static NJS_CNK_OBJECT draw_40 = {
#include "stg14/terrain/draw_40.inc"
};
static NJS_CNK_OBJECT draw_38 = {
#include "stg14/terrain/draw_38.inc"
};
static NJS_CNK_OBJECT draw_37 = {
#include "stg14/terrain/draw_37.inc"
};
static NJS_CNK_OBJECT draw_36 = {
#include "stg14/terrain/draw_36.inc"
};
static NJS_CNK_OBJECT draw_35 = {
#include "stg14/terrain/draw_35.inc"
};
static NJS_CNK_OBJECT draw_34 = {
#include "stg14/terrain/draw_34.inc"
};
static NJS_CNK_OBJECT draw_33 = {
#include "stg14/terrain/draw_33.inc"
};
static NJS_CNK_OBJECT draw_32 = {
#include "stg14/terrain/draw_32.inc"
};
static NJS_CNK_OBJECT draw_31 = {
#include "stg14/terrain/draw_31.inc"
};
static NJS_CNK_OBJECT draw_30 = {
#include "stg14/terrain/draw_30.inc"
};
static NJS_CNK_OBJECT draw_29 = {
#include "stg14/terrain/draw_29.inc"
};
static NJS_CNK_OBJECT draw_28 = {
#include "stg14/terrain/draw_28.inc"
};
static NJS_CNK_OBJECT draw_27 = {
#include "stg14/terrain/draw_27.inc"
};
static NJS_CNK_OBJECT draw_26 = {
#include "stg14/terrain/draw_26.inc"
};
static NJS_CNK_OBJECT draw_25 = {
#include "stg14/terrain/draw_25.inc"
};
static NJS_CNK_OBJECT draw_24 = {
#include "stg14/terrain/draw_24.inc"
};
static NJS_CNK_OBJECT draw_23 = {
#include "stg14/terrain/draw_23.inc"
};
static NJS_CNK_OBJECT draw_22 = {
#include "stg14/terrain/draw_22.inc"
};
static NJS_CNK_OBJECT draw_21 = {
#include "stg14/terrain/draw_21.inc"
};
static NJS_CNK_OBJECT draw_20 = {
#include "stg14/terrain/draw_20.inc"
};
static NJS_CNK_OBJECT draw_19 = {
#include "stg14/terrain/draw_19.inc"
};
static NJS_CNK_OBJECT draw_18 = {
#include "stg14/terrain/draw_18.inc"
};
static NJS_CNK_OBJECT draw_16 = {
#include "stg14/terrain/draw_16.inc"
};
static NJS_CNK_OBJECT draw_15 = {
#include "stg14/terrain/draw_15.inc"
};
static NJS_CNK_OBJECT draw_14 = {
#include "stg14/terrain/draw_14.inc"
};
static NJS_CNK_OBJECT draw_13 = {
#include "stg14/terrain/draw_13.inc"
};
static NJS_CNK_OBJECT draw_12 = {
#include "stg14/terrain/draw_12.inc"
};
static NJS_CNK_OBJECT draw_11 = {
#include "stg14/terrain/draw_11.inc"
};
static NJS_CNK_OBJECT draw_10 = {
#include "stg14/terrain/draw_10.inc"
};
static NJS_CNK_OBJECT draw_9 = {
#include "stg14/terrain/draw_9.inc"
};
static NJS_CNK_OBJECT draw_8 = {
#include "stg14/terrain/draw_8.inc"
};
static NJS_CNK_OBJECT draw_7 = {
#include "stg14/terrain/draw_7.inc"
};
static NJS_CNK_OBJECT draw_5 = {
#include "stg14/terrain/draw_5.inc"
};
static NJS_CNK_OBJECT draw_4 = {
#include "stg14/terrain/draw_4.inc"
};
static NJS_CNK_OBJECT draw_2 = {
#include "stg14/terrain/draw_2.inc"
};
static NJS_CNK_OBJECT draw_1 = {
#include "stg14/terrain/draw_1.inc"
};
static NJS_CNK_OBJECT draw_0 = {
#include "stg14/terrain/draw_0.inc"
};
