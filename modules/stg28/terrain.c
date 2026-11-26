/* STG28.PRS's motions, their registry, land, texture lists, Basic models,
 * chunk models and texture names, from the disc. its C lies between
 * cave_scenery.c and object_archives.c, and its D lies between
 * pyramid_cave.c and object_archives.c */
#include <ninja.h>
#include <sg_xpt.h>

#include "collision/world.h"
#include "player/player.h"
#include "shared/motions.h"
#include "stg28/pyramid_cave.h"

#include "stg28/terrain/text.inc"

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
struct motion_entry module_motions[9];
static struct land_entry entries[462];
struct land_table pyramid_cave_land;
static NJS_TEXNAME textures[33];
NJS_TEXLIST pyramid_cave_land_texlist;
static NJS_MATERIAL collision_0_materials[1];
static Sint16 collision_0_faces[198];
static NJS_MESHSET collision_0_meshsets[1];
static NJS_POINT3 collision_0_points[184];
static NJS_MODEL collision_0_model;
static NJS_OBJECT collision_0;
static NJS_MATERIAL collision_1_materials[1];
static Sint16 collision_1_faces[18];
static NJS_MESHSET collision_1_meshsets[1];
static NJS_POINT3 collision_1_points[16];
static NJS_MODEL collision_1_model;
static NJS_OBJECT collision_1;
static NJS_MATERIAL collision_2_materials[1];
static Sint16 collision_2_faces[14];
static NJS_MESHSET collision_2_meshsets[1];
static NJS_POINT3 collision_2_points[12];
static NJS_MODEL collision_2_model;
static NJS_OBJECT collision_2;
static NJS_MATERIAL collision_3_materials[1];
static Sint16 collision_3_faces[156];
static NJS_MESHSET collision_3_meshsets[1];
static NJS_POINT3 collision_3_points[114];
static NJS_MODEL collision_3_model;
static NJS_OBJECT collision_3;
static NJS_MATERIAL collision_4_materials[1];
static Sint16 collision_4_faces[198];
static NJS_MESHSET collision_4_meshsets[1];
static NJS_POINT3 collision_4_points[184];
static NJS_MODEL collision_4_model;
static NJS_OBJECT collision_4;
static NJS_MATERIAL collision_5_materials[1];
static Sint16 collision_5_faces[18];
static NJS_MESHSET collision_5_meshsets[1];
static NJS_POINT3 collision_5_points[16];
static NJS_MODEL collision_5_model;
static NJS_OBJECT collision_5;
static NJS_MATERIAL collision_6_materials[1];
static Sint16 collision_6_faces[58];
static NJS_MESHSET collision_6_meshsets[1];
static NJS_POINT3 collision_6_points[54];
static NJS_MODEL collision_6_model;
static NJS_OBJECT collision_6;
static NJS_MATERIAL collision_8_materials[1];
static Sint16 collision_8_faces[73];
static NJS_MESHSET collision_8_meshsets[1];
static NJS_POINT3 collision_8_points[72];
static NJS_MODEL collision_8_model;
static NJS_OBJECT collision_8;
static NJS_MATERIAL collision_10_materials[1];
static Sint16 collision_10_faces[79];
static NJS_MESHSET collision_10_meshsets[1];
static NJS_POINT3 collision_10_points[78];
static NJS_MODEL collision_10_model;
static NJS_OBJECT collision_10;
static NJS_MATERIAL collision_12_materials[1];
static Sint16 collision_12_faces[14];
static NJS_MESHSET collision_12_meshsets[1];
static NJS_POINT3 collision_12_points[12];
static NJS_MODEL collision_12_model;
static NJS_OBJECT collision_12;
static NJS_MATERIAL collision_18_materials[1];
static Sint16 collision_18_faces[14];
static NJS_MESHSET collision_18_meshsets[1];
static NJS_POINT3 collision_18_points[12];
static NJS_MODEL collision_18_model;
static NJS_OBJECT collision_18;
static NJS_MATERIAL collision_22_materials[1];
static Sint16 collision_22_faces[14];
static NJS_MESHSET collision_22_meshsets[1];
static NJS_POINT3 collision_22_points[12];
static NJS_MODEL collision_22_model;
static NJS_OBJECT collision_22;
static NJS_MATERIAL collision_26_materials[1];
static Sint16 collision_26_faces[14];
static NJS_MESHSET collision_26_meshsets[1];
static NJS_POINT3 collision_26_points[12];
static NJS_MODEL collision_26_model;
static NJS_OBJECT collision_26;
static NJS_MATERIAL collision_29_materials[1];
static Sint16 collision_29_faces[14];
static NJS_MESHSET collision_29_meshsets[1];
static NJS_POINT3 collision_29_points[12];
static NJS_MODEL collision_29_model;
static NJS_OBJECT collision_29;
static NJS_MATERIAL collision_36_materials[1];
static Sint16 collision_36_faces[10];
static NJS_MESHSET collision_36_meshsets[1];
static NJS_POINT3 collision_36_points[8];
static NJS_MODEL collision_36_model;
static NJS_OBJECT collision_36;
static NJS_MATERIAL collision_41_materials[1];
static Sint16 collision_41_faces[10];
static NJS_MESHSET collision_41_meshsets[1];
static NJS_POINT3 collision_41_points[8];
static NJS_MODEL collision_41_model;
static NJS_OBJECT collision_41;
static NJS_MATERIAL collision_49_materials[1];
static Sint16 collision_49_faces[10];
static NJS_MESHSET collision_49_meshsets[1];
static NJS_POINT3 collision_49_points[8];
static NJS_MODEL collision_49_model;
static NJS_OBJECT collision_49;
static NJS_MATERIAL collision_50_materials[1];
static Sint16 collision_50_faces[10];
static NJS_MESHSET collision_50_meshsets[1];
static NJS_POINT3 collision_50_points[8];
static NJS_MODEL collision_50_model;
static NJS_OBJECT collision_50;
static NJS_MATERIAL collision_51_materials[1];
static Sint16 collision_51_faces[14];
static NJS_MESHSET collision_51_meshsets[1];
static NJS_POINT3 collision_51_points[12];
static NJS_MODEL collision_51_model;
static NJS_OBJECT collision_51;
static NJS_MATERIAL collision_52_materials[1];
static Sint16 collision_52_faces[202];
static NJS_MESHSET collision_52_meshsets[1];
static NJS_POINT3 collision_52_points[188];
static NJS_MODEL collision_52_model;
static NJS_OBJECT collision_52;
static NJS_MATERIAL collision_53_materials[1];
static Sint16 collision_53_faces[36];
static NJS_MESHSET collision_53_meshsets[1];
static NJS_POINT3 collision_53_points[24];
static NJS_MODEL collision_53_model;
static NJS_OBJECT collision_53;
static NJS_MATERIAL collision_54_materials[1];
static Sint16 collision_54_faces[86];
static NJS_MESHSET collision_54_meshsets[1];
static NJS_POINT3 collision_54_points[40];
static NJS_MODEL collision_54_model;
static NJS_OBJECT collision_54;
static NJS_MATERIAL collision_55_materials[1];
static Sint16 collision_55_faces[238];
static NJS_MESHSET collision_55_meshsets[1];
static NJS_POINT3 collision_55_points[96];
static NJS_MODEL collision_55_model;
static NJS_OBJECT collision_55;
static NJS_MATERIAL collision_56_materials[1];
static Sint16 collision_56_faces[33];
static NJS_MESHSET collision_56_meshsets[1];
static NJS_POINT3 collision_56_points[18];
static NJS_MODEL collision_56_model;
static NJS_OBJECT collision_56;
static NJS_MATERIAL collision_57_materials[1];
static Sint16 collision_57_faces[329];
static NJS_MESHSET collision_57_meshsets[1];
static NJS_POINT3 collision_57_points[118];
static NJS_MODEL collision_57_model;
static NJS_OBJECT collision_57;
static NJS_MATERIAL collision_58_materials[1];
static Sint16 collision_58_faces[90];
static NJS_MESHSET collision_58_meshsets[1];
static NJS_POINT3 collision_58_points[32];
static NJS_MODEL collision_58_model;
static NJS_OBJECT collision_58;
static NJS_MATERIAL collision_59_materials[1];
static Sint16 collision_59_faces[201];
static NJS_MESHSET collision_59_meshsets[1];
static NJS_POINT3 collision_59_points[77];
static NJS_MODEL collision_59_model;
static NJS_OBJECT collision_59;
static NJS_MATERIAL collision_60_materials[1];
static Sint16 collision_60_faces[582];
static NJS_MESHSET collision_60_meshsets[1];
static NJS_POINT3 collision_60_points[368];
static NJS_MODEL collision_60_model;
static NJS_OBJECT collision_60;
static NJS_MATERIAL collision_61_materials[1];
static Sint16 collision_61_faces[62];
static NJS_MESHSET collision_61_meshsets[1];
static NJS_POINT3 collision_61_points[26];
static NJS_MODEL collision_61_model;
static NJS_OBJECT collision_61;
static NJS_MATERIAL collision_69_materials[1];
static Sint16 collision_69_faces[60];
static NJS_MESHSET collision_69_meshsets[1];
static NJS_POINT3 collision_69_points[32];
static NJS_MODEL collision_69_model;
static NJS_OBJECT collision_69;
static NJS_MATERIAL collision_70_materials[1];
static Sint16 collision_70_faces[36];
static NJS_MESHSET collision_70_meshsets[1];
static NJS_POINT3 collision_70_points[18];
static NJS_MODEL collision_70_model;
static NJS_OBJECT collision_70;
static NJS_MATERIAL collision_71_materials[1];
static Sint16 collision_71_faces[46];
static NJS_MESHSET collision_71_meshsets[1];
static NJS_POINT3 collision_71_points[22];
static NJS_MODEL collision_71_model;
static NJS_OBJECT collision_71;
static NJS_MATERIAL collision_72_materials[1];
static Sint16 collision_72_faces[31];
static NJS_MESHSET collision_72_meshsets[1];
static NJS_POINT3 collision_72_points[14];
static NJS_MODEL collision_72_model;
static NJS_OBJECT collision_72;
static NJS_MATERIAL collision_73_materials[1];
static Sint16 collision_73_faces[156];
static NJS_MESHSET collision_73_meshsets[1];
static NJS_POINT3 collision_73_points[63];
static NJS_MODEL collision_73_model;
static NJS_OBJECT collision_73;
static NJS_MATERIAL collision_74_materials[1];
static Sint16 collision_74_faces[195];
static NJS_MESHSET collision_74_meshsets[1];
static NJS_POINT3 collision_74_points[80];
static NJS_MODEL collision_74_model;
static NJS_OBJECT collision_74;
static NJS_MATERIAL collision_75_materials[1];
static Sint16 collision_75_faces[75];
static NJS_MESHSET collision_75_meshsets[1];
static NJS_POINT3 collision_75_points[33];
static NJS_MODEL collision_75_model;
static NJS_OBJECT collision_75;
static NJS_MATERIAL collision_76_materials[1];
static Sint16 collision_76_faces[99];
static NJS_MESHSET collision_76_meshsets[1];
static NJS_POINT3 collision_76_points[79];
static NJS_MODEL collision_76_model;
static NJS_OBJECT collision_76;
static NJS_MATERIAL collision_77_materials[1];
static Sint16 collision_77_faces[86];
static NJS_MESHSET collision_77_meshsets[1];
static NJS_POINT3 collision_77_points[58];
static NJS_MODEL collision_77_model;
static NJS_OBJECT collision_77;
static NJS_MATERIAL collision_78_materials[1];
static Sint16 collision_78_faces[22];
static NJS_MESHSET collision_78_meshsets[1];
static NJS_POINT3 collision_78_points[15];
static NJS_MODEL collision_78_model;
static NJS_OBJECT collision_78;
static NJS_MATERIAL collision_79_materials[1];
static Sint16 collision_79_faces[213];
static NJS_MESHSET collision_79_meshsets[1];
static NJS_POINT3 collision_79_points[82];
static NJS_MODEL collision_79_model;
static NJS_OBJECT collision_79;
static NJS_MATERIAL collision_80_materials[1];
static Sint16 collision_80_faces[175];
static NJS_MESHSET collision_80_meshsets[1];
static NJS_POINT3 collision_80_points[71];
static NJS_MODEL collision_80_model;
static NJS_OBJECT collision_80;
static NJS_MATERIAL collision_82_materials[1];
static Sint16 collision_82_faces[242];
static NJS_MESHSET collision_82_meshsets[1];
static NJS_POINT3 collision_82_points[89];
static NJS_MODEL collision_82_model;
static NJS_OBJECT collision_82;
static NJS_MATERIAL collision_83_materials[1];
static Sint16 collision_83_faces[585];
static NJS_MESHSET collision_83_meshsets[1];
static NJS_POINT3 collision_83_points[368];
static NJS_MODEL collision_83_model;
static NJS_OBJECT collision_83;
static NJS_MATERIAL collision_84_materials[1];
static Sint16 collision_84_faces[726];
static NJS_MESHSET collision_84_meshsets[1];
static NJS_POINT3 collision_84_points[424];
static NJS_MODEL collision_84_model;
static NJS_OBJECT collision_84;
static NJS_MATERIAL collision_86_materials[1];
static Sint16 collision_86_faces[226];
static NJS_MESHSET collision_86_meshsets[1];
static NJS_POINT3 collision_86_points[212];
static NJS_MODEL collision_86_model;
static NJS_OBJECT collision_86;
static NJS_MATERIAL collision_88_materials[1];
static Sint16 collision_88_faces[11];
static NJS_MESHSET collision_88_meshsets[1];
static NJS_POINT3 collision_88_points[8];
static NJS_MODEL collision_88_model;
static NJS_OBJECT collision_88;
static NJS_MATERIAL collision_89_materials[1];
static Sint16 collision_89_faces[45];
static NJS_MESHSET collision_89_meshsets[1];
static NJS_POINT3 collision_89_points[32];
static NJS_MODEL collision_89_model;
static NJS_OBJECT collision_89;
static NJS_MATERIAL collision_90_materials[1];
static Sint16 collision_90_faces[87];
static NJS_MESHSET collision_90_meshsets[1];
static NJS_POINT3 collision_90_points[32];
static NJS_MODEL collision_90_model;
static NJS_OBJECT collision_90;
static NJS_MATERIAL collision_92_materials[1];
static Sint16 collision_92_faces[84];
static NJS_MESHSET collision_92_meshsets[1];
static NJS_POINT3 collision_92_points[32];
static NJS_MODEL collision_92_model;
static NJS_OBJECT collision_92;
static NJS_MATERIAL collision_93_materials[1];
static Sint16 collision_93_faces[67];
static NJS_MESHSET collision_93_meshsets[1];
static NJS_POINT3 collision_93_points[25];
static NJS_MODEL collision_93_model;
static NJS_OBJECT collision_93;
static NJS_MATERIAL collision_97_materials[1];
static Sint16 collision_97_faces[11];
static NJS_MESHSET collision_97_meshsets[1];
static NJS_POINT3 collision_97_points[8];
static NJS_MODEL collision_97_model;
static NJS_OBJECT collision_97;
static NJS_MATERIAL collision_98_materials[1];
static Sint16 collision_98_faces[40];
static NJS_MESHSET collision_98_meshsets[1];
static NJS_POINT3 collision_98_points[16];
static NJS_MODEL collision_98_model;
static NJS_OBJECT collision_98;
static NJS_MATERIAL collision_99_materials[1];
static Sint16 collision_99_faces[50];
static NJS_MESHSET collision_99_meshsets[1];
static NJS_POINT3 collision_99_points[20];
static NJS_MODEL collision_99_model;
static NJS_OBJECT collision_99;
static NJS_MATERIAL collision_100_materials[1];
static Sint16 collision_100_faces[33];
static NJS_MESHSET collision_100_meshsets[1];
static NJS_POINT3 collision_100_points[12];
static NJS_MODEL collision_100_model;
static NJS_OBJECT collision_100;
static NJS_MATERIAL collision_121_materials[1];
static Sint16 collision_121_faces[73];
static NJS_MESHSET collision_121_meshsets[1];
static NJS_POINT3 collision_121_points[26];
static NJS_MODEL collision_121_model;
static NJS_OBJECT collision_121;
static NJS_MATERIAL collision_131_materials[1];
static Sint16 collision_131_faces[226];
static NJS_MESHSET collision_131_meshsets[1];
static NJS_POINT3 collision_131_points[93];
static NJS_MODEL collision_131_model;
static NJS_OBJECT collision_131;
static NJS_MATERIAL collision_133_materials[1];
static Sint16 collision_133_faces[330];
static NJS_MESHSET collision_133_meshsets[1];
static NJS_POINT3 collision_133_points[114];
static NJS_MODEL collision_133_model;
static NJS_OBJECT collision_133;
static NJS_MATERIAL collision_135_materials[1];
static Sint16 collision_135_faces[195];
static NJS_MESHSET collision_135_meshsets[1];
static NJS_POINT3 collision_135_points[76];
static NJS_MODEL collision_135_model;
static NJS_OBJECT collision_135;
static NJS_MATERIAL collision_136_materials[1];
static Sint16 collision_136_faces[189];
static NJS_MESHSET collision_136_meshsets[1];
static NJS_POINT3 collision_136_points[67];
static NJS_MODEL collision_136_model;
static NJS_OBJECT collision_136;
static NJS_MATERIAL collision_138_materials[1];
static Sint16 collision_138_faces[226];
static NJS_MESHSET collision_138_meshsets[1];
static NJS_POINT3 collision_138_points[85];
static NJS_MODEL collision_138_model;
static NJS_OBJECT collision_138;
static NJS_MATERIAL collision_140_materials[1];
static Sint16 collision_140_faces[226];
static NJS_MESHSET collision_140_meshsets[1];
static NJS_POINT3 collision_140_points[93];
static NJS_MODEL collision_140_model;
static NJS_OBJECT collision_140;
static NJS_MATERIAL collision_142_materials[1];
static Sint16 collision_142_faces[62];
static NJS_MESHSET collision_142_meshsets[1];
static NJS_POINT3 collision_142_points[26];
static NJS_MODEL collision_142_model;
static NJS_OBJECT collision_142;
static NJS_MATERIAL collision_156_materials[1];
static Sint16 collision_156_faces[56];
static NJS_MESHSET collision_156_meshsets[1];
static NJS_POINT3 collision_156_points[25];
static NJS_MODEL collision_156_model;
static NJS_OBJECT collision_156;
static NJS_MATERIAL collision_160_materials[1];
static Sint16 collision_160_faces[59];
static NJS_MESHSET collision_160_meshsets[1];
static NJS_POINT3 collision_160_points[27];
static NJS_MODEL collision_160_model;
static NJS_OBJECT collision_160;
static NJS_MATERIAL collision_163_materials[1];
static Sint16 collision_163_faces[36];
static NJS_MESHSET collision_163_meshsets[1];
static NJS_POINT3 collision_163_points[24];
static NJS_MODEL collision_163_model;
static NJS_OBJECT collision_163;
static NJS_MATERIAL collision_170_materials[1];
static Sint16 collision_170_faces[24];
static NJS_MESHSET collision_170_meshsets[1];
static NJS_POINT3 collision_170_points[16];
static NJS_MODEL collision_170_model;
static NJS_OBJECT collision_170;
static NJS_MATERIAL collision_175_materials[1];
static Sint16 collision_175_faces[27];
static NJS_MESHSET collision_175_meshsets[1];
static NJS_POINT3 collision_175_points[16];
static NJS_MODEL collision_175_model;
static NJS_OBJECT collision_175;
static NJS_MATERIAL collision_182_materials[1];
static Sint16 collision_182_faces[27];
static NJS_MESHSET collision_182_meshsets[1];
static NJS_POINT3 collision_182_points[16];
static NJS_MODEL collision_182_model;
static NJS_OBJECT collision_182;
static NJS_MATERIAL collision_183_materials[1];
static Sint16 collision_183_faces[79];
static NJS_MESHSET collision_183_meshsets[1];
static NJS_POINT3 collision_183_points[37];
static NJS_MODEL collision_183_model;
static NJS_OBJECT collision_183;
static NJS_MATERIAL collision_184_materials[1];
static Sint16 collision_184_faces[325];
static NJS_MESHSET collision_184_meshsets[1];
static NJS_POINT3 collision_184_points[129];
static NJS_MODEL collision_184_model;
static NJS_OBJECT collision_184;
static NJS_MATERIAL collision_185_materials[1];
static Sint16 collision_185_faces[40];
static NJS_MESHSET collision_185_meshsets[1];
static NJS_POINT3 collision_185_points[18];
static NJS_MODEL collision_185_model;
static NJS_OBJECT collision_185;
static NJS_MATERIAL collision_186_materials[1];
static Sint16 collision_186_faces[195];
static NJS_MESHSET collision_186_meshsets[1];
static NJS_POINT3 collision_186_points[90];
static NJS_MODEL collision_186_model;
static NJS_OBJECT collision_186;
static NJS_MATERIAL collision_187_materials[1];
static Sint16 collision_187_faces[110];
static NJS_MESHSET collision_187_meshsets[1];
static NJS_POINT3 collision_187_points[56];
static NJS_MODEL collision_187_model;
static NJS_OBJECT collision_187;
static NJS_MATERIAL collision_188_materials[1];
static Sint16 collision_188_faces[14];
static NJS_MESHSET collision_188_meshsets[1];
static NJS_POINT3 collision_188_points[9];
static NJS_MODEL collision_188_model;
static NJS_OBJECT collision_188;
static NJS_MATERIAL collision_189_materials[1];
static Sint16 collision_189_faces[64];
static NJS_MESHSET collision_189_meshsets[1];
static NJS_POINT3 collision_189_points[24];
static NJS_MODEL collision_189_model;
static NJS_OBJECT collision_189;
static NJS_MATERIAL collision_190_materials[1];
static Sint16 collision_190_faces[164];
static NJS_MESHSET collision_190_meshsets[1];
static NJS_POINT3 collision_190_points[40];
static NJS_MODEL collision_190_model;
static NJS_OBJECT collision_190;
static NJS_MATERIAL collision_191_materials[1];
static Sint16 collision_191_faces[249];
static NJS_MESHSET collision_191_meshsets[1];
static NJS_POINT3 collision_191_points[66];
static NJS_MODEL collision_191_model;
static NJS_OBJECT collision_191;
static NJS_MATERIAL collision_192_materials[1];
static Sint16 collision_192_faces[240];
static NJS_MESHSET collision_192_meshsets[1];
static NJS_POINT3 collision_192_points[96];
static NJS_MODEL collision_192_model;
static NJS_OBJECT collision_192;
static NJS_MATERIAL collision_193_materials[1];
static Sint16 collision_193_faces[231];
static NJS_MESHSET collision_193_meshsets[1];
static NJS_POINT3 collision_193_points[81];
static NJS_MODEL collision_193_model;
static NJS_OBJECT collision_193;
static NJS_MATERIAL collision_194_materials[1];
static Sint16 collision_194_faces[285];
static NJS_MESHSET collision_194_meshsets[1];
static NJS_POINT3 collision_194_points[176];
static NJS_MODEL collision_194_model;
static NJS_OBJECT collision_194;
static NJS_MATERIAL collision_195_materials[1];
static Sint16 collision_195_faces[333];
static NJS_MESHSET collision_195_meshsets[1];
static NJS_POINT3 collision_195_points[208];
static NJS_MODEL collision_195_model;
static NJS_OBJECT collision_195;
static NJS_MATERIAL collision_196_materials[1];
static Sint16 collision_196_faces[54];
static NJS_MESHSET collision_196_meshsets[1];
static NJS_POINT3 collision_196_points[32];
static NJS_MODEL collision_196_model;
static NJS_OBJECT collision_196;
static NJS_MATERIAL collision_197_materials[1];
static Sint16 collision_197_faces[259];
static NJS_MESHSET collision_197_meshsets[1];
static NJS_POINT3 collision_197_points[91];
static NJS_MODEL collision_197_model;
static NJS_OBJECT collision_197;
static NJS_MATERIAL collision_198_materials[1];
static Sint16 collision_198_faces[65];
static NJS_MESHSET collision_198_meshsets[1];
static NJS_POINT3 collision_198_points[24];
static NJS_MODEL collision_198_model;
static NJS_OBJECT collision_198;
static NJS_MATERIAL collision_199_materials[1];
static Sint16 collision_199_faces[59];
static NJS_MESHSET collision_199_meshsets[1];
static NJS_POINT3 collision_199_points[24];
static NJS_MODEL collision_199_model;
static NJS_OBJECT collision_199;
static NJS_MATERIAL collision_200_materials[1];
static Sint16 collision_200_faces[288];
static NJS_MESHSET collision_200_meshsets[1];
static NJS_POINT3 collision_200_points[140];
static NJS_MODEL collision_200_model;
static NJS_OBJECT collision_200;
static NJS_MATERIAL collision_201_materials[1];
static Sint16 collision_201_faces[279];
static NJS_MESHSET collision_201_meshsets[1];
static NJS_POINT3 collision_201_points[150];
static NJS_MODEL collision_201_model;
static NJS_OBJECT collision_201;
static NJS_MATERIAL collision_202_materials[1];
static Sint16 collision_202_faces[288];
static NJS_MESHSET collision_202_meshsets[1];
static NJS_POINT3 collision_202_points[140];
static NJS_MODEL collision_202_model;
static NJS_OBJECT collision_202;
static NJS_MATERIAL collision_203_materials[1];
static Sint16 collision_203_faces[279];
static NJS_MESHSET collision_203_meshsets[1];
static NJS_POINT3 collision_203_points[150];
static NJS_MODEL collision_203_model;
static NJS_OBJECT collision_203;
static NJS_MATERIAL collision_204_materials[1];
static Sint16 collision_204_faces[333];
static NJS_MESHSET collision_204_meshsets[1];
static NJS_POINT3 collision_204_points[180];
static NJS_MODEL collision_204_model;
static NJS_OBJECT collision_204;
static NJS_MATERIAL collision_205_materials[1];
static Sint16 collision_205_faces[351];
static NJS_MESHSET collision_205_meshsets[1];
static NJS_POINT3 collision_205_points[190];
static NJS_MODEL collision_205_model;
static NJS_OBJECT collision_205;
static NJS_MATERIAL collision_206_materials[1];
static Sint16 collision_206_faces[333];
static NJS_MESHSET collision_206_meshsets[1];
static NJS_POINT3 collision_206_points[180];
static NJS_MODEL collision_206_model;
static NJS_OBJECT collision_206;
static NJS_MATERIAL collision_207_materials[1];
static Sint16 collision_207_faces[351];
static NJS_MESHSET collision_207_meshsets[1];
static NJS_POINT3 collision_207_points[190];
static NJS_MODEL collision_207_model;
static NJS_OBJECT collision_207;
static NJS_MATERIAL collision_208_materials[1];
static Sint16 collision_208_faces[347];
static NJS_MESHSET collision_208_meshsets[1];
static NJS_POINT3 collision_208_points[178];
static NJS_MODEL collision_208_model;
static NJS_OBJECT collision_208;
static NJS_MATERIAL collision_209_materials[1];
static Sint16 collision_209_faces[423];
static NJS_MESHSET collision_209_meshsets[1];
static NJS_POINT3 collision_209_points[230];
static NJS_MODEL collision_209_model;
static NJS_OBJECT collision_209;
static NJS_MATERIAL collision_210_materials[1];
static Sint16 collision_210_faces[347];
static NJS_MESHSET collision_210_meshsets[1];
static NJS_POINT3 collision_210_points[178];
static NJS_MODEL collision_210_model;
static NJS_OBJECT collision_210;
static NJS_MATERIAL collision_211_materials[1];
static Sint16 collision_211_faces[423];
static NJS_MESHSET collision_211_meshsets[1];
static NJS_POINT3 collision_211_points[230];
static NJS_MODEL collision_211_model;
static NJS_OBJECT collision_211;
static Sint16 draw_249_polygons[589];
static Sint32 draw_249_vertices[423];
static NJS_CNK_MODEL draw_249_model;
static NJS_CNK_OBJECT draw_249;
static Sint16 draw_248_polygons[1549];
static Sint32 draw_248_vertices[1235];
static NJS_CNK_MODEL draw_248_model;
static NJS_CNK_OBJECT draw_248;
static Sint16 draw_247_polygons[65];
static Sint32 draw_247_vertices[35];
static NJS_CNK_MODEL draw_247_model;
static NJS_CNK_OBJECT draw_247;
static Sint16 draw_246_polygons[477];
static Sint32 draw_246_vertices[327];
static NJS_CNK_MODEL draw_246_model;
static NJS_CNK_OBJECT draw_246;
static Sint16 draw_238_polygons[437];
static Sint32 draw_238_vertices[275];
static NJS_CNK_MODEL draw_238_model;
static NJS_CNK_OBJECT draw_245;
static Sint16 draw_236_polygons[171];
static Sint32 draw_236_vertices[107];
static NJS_CNK_MODEL draw_236_model;
static NJS_CNK_OBJECT draw_237;
static Sint16 draw_235_polygons[495];
static Sint32 draw_235_vertices[323];
static NJS_CNK_MODEL draw_235_model;
static NJS_CNK_OBJECT draw_235;
static Sint16 draw_234_polygons[97];
static Sint32 draw_234_vertices[55];
static NJS_CNK_MODEL draw_234_model;
static NJS_CNK_OBJECT draw_234;
static Sint16 draw_233_polygons[243];
static Sint32 draw_233_vertices[159];
static NJS_CNK_MODEL draw_233_model;
static NJS_CNK_OBJECT draw_233;
static Sint16 draw_232_polygons[397];
static Sint32 draw_232_vertices[259];
static NJS_CNK_MODEL draw_232_model;
static NJS_CNK_OBJECT draw_232;
static Sint16 draw_231_polygons[121];
static Sint32 draw_231_vertices[107];
static NJS_CNK_MODEL draw_231_model;
static NJS_CNK_OBJECT draw_231;
static Sint16 draw_230_polygons[759];
static Sint32 draw_230_vertices[459];
static NJS_CNK_MODEL draw_230_model;
static NJS_CNK_OBJECT draw_230;
static Sint16 draw_229_polygons[549];
static Sint32 draw_229_vertices[335];
static NJS_CNK_MODEL draw_229_model;
static NJS_CNK_OBJECT draw_229;
static Sint16 draw_225_polygons[171];
static Sint32 draw_225_vertices[99];
static NJS_CNK_MODEL draw_225_model;
static NJS_CNK_OBJECT draw_228;
static Sint16 draw_224_polygons[1611];
static Sint32 draw_224_vertices[1083];
static NJS_CNK_MODEL draw_224_model;
static NJS_CNK_OBJECT draw_224;
static Sint16 draw_223_polygons[153];
static Sint32 draw_223_vertices[127];
static NJS_CNK_MODEL draw_223_model;
static NJS_CNK_OBJECT draw_223;
static Sint16 draw_220_polygons[57];
static Sint32 draw_220_vertices[51];
static NJS_CNK_MODEL draw_220_model;
static NJS_CNK_OBJECT draw_222;
static Sint16 draw_219_polygons[57];
static Sint32 draw_219_vertices[51];
static NJS_CNK_MODEL draw_219_model;
static NJS_CNK_OBJECT draw_219;
static Sint16 draw_218_polygons[613];
static Sint32 draw_218_vertices[591];
static NJS_CNK_MODEL draw_218_model;
static NJS_CNK_OBJECT draw_218;
static Sint16 draw_217_polygons[627];
static Sint32 draw_217_vertices[447];
static NJS_CNK_MODEL draw_217_model;
static NJS_CNK_OBJECT draw_217;
static Sint16 draw_216_polygons[117];
static Sint32 draw_216_vertices[67];
static NJS_CNK_MODEL draw_216_model;
static NJS_CNK_OBJECT draw_216;
static Sint16 draw_213_polygons[1153];
static Sint32 draw_213_vertices[751];
static NJS_CNK_MODEL draw_213_model;
static NJS_CNK_OBJECT draw_215;
static Sint16 draw_212_polygons[565];
static Sint32 draw_212_vertices[371];
static NJS_CNK_MODEL draw_212_model;
static NJS_CNK_OBJECT draw_212;
static Sint16 draw_210_polygons[739];
static Sint32 draw_210_vertices[483];
static NJS_CNK_MODEL draw_210_model;
static NJS_CNK_OBJECT draw_211;
static Sint16 draw_208_polygons[739];
static Sint32 draw_208_vertices[483];
static NJS_CNK_MODEL draw_208_model;
static NJS_CNK_OBJECT draw_209;
static Sint16 draw_207_polygons[65];
static Sint32 draw_207_vertices[67];
static NJS_CNK_MODEL draw_207_model;
static NJS_CNK_OBJECT draw_207;
static Sint16 draw_205_polygons[403];
static Sint32 draw_205_vertices[359];
static NJS_CNK_MODEL draw_205_model;
static NJS_CNK_OBJECT draw_206;
static Sint16 draw_204_polygons[489];
static Sint32 draw_204_vertices[371];
static NJS_CNK_MODEL draw_204_model;
static NJS_CNK_OBJECT draw_204;
static Sint16 draw_203_polygons[491];
static Sint32 draw_203_vertices[359];
static NJS_CNK_MODEL draw_203_model;
static NJS_CNK_OBJECT draw_203;
static Sint16 draw_192_polygons[419];
static Sint32 draw_192_vertices[335];
static NJS_CNK_MODEL draw_192_model;
static NJS_CNK_OBJECT draw_201;
static Sint16 draw_190_polygons[2287];
static Sint32 draw_190_vertices[1719];
static NJS_CNK_MODEL draw_190_model;
static NJS_CNK_OBJECT draw_191;
static Sint16 draw_188_polygons[2307];
static Sint32 draw_188_vertices[1707];
static NJS_CNK_MODEL draw_188_model;
static NJS_CNK_OBJECT draw_189;
static Sint16 draw_186_polygons[2327];
static Sint32 draw_186_vertices[1715];
static NJS_CNK_MODEL draw_186_model;
static NJS_CNK_OBJECT draw_187;
static Sint16 draw_185_polygons[65];
static Sint32 draw_185_vertices[67];
static NJS_CNK_MODEL draw_185_model;
static NJS_CNK_OBJECT draw_185;
static Sint16 draw_184_polygons[1101];
static Sint32 draw_184_vertices[699];
static NJS_CNK_MODEL draw_184_model;
static NJS_CNK_OBJECT draw_184;
static Sint16 draw_182_polygons[2395];
static Sint32 draw_182_vertices[2915];
static NJS_CNK_MODEL draw_182_model;
static NJS_CNK_OBJECT draw_183;
static Sint16 draw_180_polygons[2559];
static Sint32 draw_180_vertices[3203];
static NJS_CNK_MODEL draw_180_model;
static NJS_CNK_OBJECT draw_181;
static Sint16 draw_159_polygons[151];
static Sint32 draw_159_vertices[99];
static NJS_CNK_MODEL draw_159_model;
static NJS_CNK_OBJECT draw_179;
static Sint16 draw_149_polygons[199];
static Sint32 draw_149_vertices[179];
static NJS_CNK_MODEL draw_149_model;
static NJS_CNK_OBJECT draw_158;
static Sint16 draw_147_polygons[213];
static Sint32 draw_147_vertices[187];
static NJS_CNK_MODEL draw_147_model;
static NJS_CNK_OBJECT draw_148;
static Sint16 draw_137_polygons[545];
static Sint32 draw_137_vertices[379];
static NJS_CNK_MODEL draw_137_model;
static NJS_CNK_OBJECT draw_146;
static Sint16 draw_134_polygons[1141];
static Sint32 draw_134_vertices[735];
static NJS_CNK_MODEL draw_134_model;
static NJS_CNK_OBJECT draw_136;
static Sint16 draw_132_polygons[1621];
static Sint32 draw_132_vertices[1279];
static NJS_CNK_MODEL draw_132_model;
static NJS_CNK_OBJECT draw_133;
static Sint16 draw_128_polygons[773];
static Sint32 draw_128_vertices[599];
static NJS_CNK_MODEL draw_128_model;
static NJS_CNK_OBJECT draw_131;
static Sint16 draw_126_polygons[375];
static Sint32 draw_126_vertices[251];
static NJS_CNK_MODEL draw_126_model;
static NJS_CNK_OBJECT draw_127;
static Sint16 draw_124_polygons[347];
static Sint32 draw_124_vertices[243];
static NJS_CNK_MODEL draw_124_model;
static NJS_CNK_OBJECT draw_125;
static Sint16 draw_121_polygons[1141];
static Sint32 draw_121_vertices[715];
static NJS_CNK_MODEL draw_121_model;
static NJS_CNK_OBJECT draw_123;
static Sint16 draw_113_polygons[431];
static Sint32 draw_113_vertices[299];
static NJS_CNK_MODEL draw_113_model;
static NJS_CNK_OBJECT draw_120;
static Sint16 draw_107_polygons[439];
static Sint32 draw_107_vertices[291];
static NJS_CNK_MODEL draw_107_model;
static NJS_CNK_OBJECT draw_112;
static Sint16 draw_101_polygons[243];
static Sint32 draw_101_vertices[159];
static NJS_CNK_MODEL draw_101_model;
static NJS_CNK_OBJECT draw_106;
static Sint16 draw_89_polygons[243];
static Sint32 draw_89_vertices[163];
static NJS_CNK_MODEL draw_89_model;
static NJS_CNK_OBJECT draw_100;
static Sint16 draw_88_polygons[243];
static Sint32 draw_88_vertices[159];
static NJS_CNK_MODEL draw_88_model;
static NJS_CNK_OBJECT draw_88;
static Sint16 draw_80_polygons[141];
static Sint32 draw_80_vertices[91];
static NJS_CNK_MODEL draw_80_model;
static NJS_CNK_OBJECT draw_87;
static Sint16 draw_73_polygons[155];
static Sint32 draw_73_vertices[107];
static NJS_CNK_MODEL draw_73_model;
static NJS_CNK_OBJECT draw_79;
static Sint16 draw_68_polygons[43];
static Sint32 draw_68_vertices[43];
static NJS_CNK_MODEL draw_68_model;
static NJS_CNK_OBJECT draw_72;
static Sint16 draw_63_polygons[43];
static Sint32 draw_63_vertices[43];
static NJS_CNK_MODEL draw_63_model;
static NJS_CNK_OBJECT draw_67;
static Sint16 draw_62_polygons[409];
static Sint32 draw_62_vertices[291];
static NJS_CNK_MODEL draw_62_model;
static NJS_CNK_OBJECT draw_62;
static Sint16 draw_61_polygons[819];
static Sint32 draw_61_vertices[559];
static NJS_CNK_MODEL draw_61_model;
static NJS_CNK_OBJECT draw_61;
static Sint16 draw_60_polygons[243];
static Sint32 draw_60_vertices[131];
static NJS_CNK_MODEL draw_60_model;
static NJS_CNK_OBJECT draw_60;
static Sint16 draw_54_polygons[57];
static Sint32 draw_54_vertices[35];
static NJS_CNK_MODEL draw_54_model;
static NJS_CNK_OBJECT draw_59;
static Sint16 draw_48_polygons[57];
static Sint32 draw_48_vertices[35];
static NJS_CNK_MODEL draw_48_model;
static NJS_CNK_OBJECT draw_53;
static Sint16 draw_46_polygons[715];
static Sint32 draw_46_vertices[475];
static NJS_CNK_MODEL draw_46_model;
static NJS_CNK_OBJECT draw_47;
static Sint16 draw_45_polygons[889];
static Sint32 draw_45_vertices[607];
static NJS_CNK_MODEL draw_45_model;
static NJS_CNK_OBJECT draw_45;
static Sint16 draw_44_polygons[739];
static Sint32 draw_44_vertices[535];
static NJS_CNK_MODEL draw_44_model;
static NJS_CNK_OBJECT draw_44;
static Sint16 draw_43_polygons[121];
static Sint32 draw_43_vertices[67];
static NJS_CNK_MODEL draw_43_model;
static NJS_CNK_OBJECT draw_43;
static Sint16 draw_42_polygons[451];
static Sint32 draw_42_vertices[315];
static NJS_CNK_MODEL draw_42_model;
static NJS_CNK_OBJECT draw_42;
static Sint16 draw_41_polygons[1069];
static Sint32 draw_41_vertices[715];
static NJS_CNK_MODEL draw_41_model;
static NJS_CNK_OBJECT draw_41;
static Sint16 draw_40_polygons[779];
static Sint32 draw_40_vertices[647];
static NJS_CNK_MODEL draw_40_model;
static NJS_CNK_OBJECT draw_40;
static Sint16 draw_39_polygons[693];
static Sint32 draw_39_vertices[575];
static NJS_CNK_MODEL draw_39_model;
static NJS_CNK_OBJECT draw_39;
static Sint16 draw_38_polygons[201];
static Sint32 draw_38_vertices[139];
static NJS_CNK_MODEL draw_38_model;
static NJS_CNK_OBJECT draw_38;
static Sint16 draw_37_polygons[565];
static Sint32 draw_37_vertices[403];
static NJS_CNK_MODEL draw_37_model;
static NJS_CNK_OBJECT draw_37;
static Sint16 draw_34_polygons[163];
static Sint32 draw_34_vertices[127];
static NJS_CNK_MODEL draw_34_model;
static NJS_CNK_OBJECT draw_36;
static Sint16 draw_33_polygons[2395];
static Sint32 draw_33_vertices[2915];
static NJS_CNK_MODEL draw_33_model;
static NJS_CNK_OBJECT draw_33;
static Sint16 draw_32_polygons[245];
static Sint32 draw_32_vertices[203];
static NJS_CNK_MODEL draw_32_model;
static NJS_CNK_OBJECT draw_32;
static Sint16 draw_31_polygons[141];
static Sint32 draw_31_vertices[99];
static NJS_CNK_MODEL draw_31_model;
static NJS_CNK_OBJECT draw_31;
static Sint16 draw_30_polygons[243];
static Sint32 draw_30_vertices[155];
static NJS_CNK_MODEL draw_30_model;
static NJS_CNK_OBJECT draw_30;
static Sint16 draw_4_polygons[163];
static Sint32 draw_4_vertices[127];
static NJS_CNK_MODEL draw_4_model;
static NJS_CNK_OBJECT draw_29;
static Sint16 draw_3_polygons[169];
static Sint32 draw_3_vertices[123];
static NJS_CNK_MODEL draw_3_model;
static NJS_CNK_OBJECT draw_3;
static Sint16 draw_2_polygons[241];
static Sint32 draw_2_vertices[139];
static NJS_CNK_MODEL draw_2_model;
static NJS_CNK_OBJECT draw_2;
static Sint16 draw_1_polygons[317];
static Sint32 draw_1_vertices[195];
static NJS_CNK_MODEL draw_1_model;
static NJS_CNK_OBJECT draw_1;
static Sint16 draw_0_polygons[317];
static Sint32 draw_0_vertices[187];
static NJS_CNK_MODEL draw_0_model;
static NJS_CNK_OBJECT draw_0;
static NJS_OBJECT collision_7;
static NJS_OBJECT collision_9;
static NJS_OBJECT collision_11;
static NJS_OBJECT collision_13;
static NJS_OBJECT collision_14;
static NJS_OBJECT collision_15;
static NJS_OBJECT collision_16;
static NJS_OBJECT collision_17;
static NJS_OBJECT collision_19;
static NJS_OBJECT collision_20;
static NJS_OBJECT collision_21;
static NJS_OBJECT collision_23;
static NJS_OBJECT collision_24;
static NJS_OBJECT collision_25;
static NJS_OBJECT collision_27;
static NJS_OBJECT collision_28;
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
static NJS_OBJECT collision_42;
static NJS_OBJECT collision_43;
static NJS_OBJECT collision_44;
static NJS_OBJECT collision_45;
static NJS_OBJECT collision_46;
static NJS_OBJECT collision_47;
static NJS_OBJECT collision_48;
static NJS_OBJECT collision_62;
static NJS_OBJECT collision_63;
static NJS_OBJECT collision_64;
static NJS_OBJECT collision_65;
static NJS_OBJECT collision_66;
static NJS_OBJECT collision_67;
static NJS_OBJECT collision_68;
static NJS_OBJECT collision_81;
static NJS_OBJECT collision_85;
static NJS_OBJECT collision_87;
static NJS_OBJECT collision_91;
static NJS_OBJECT collision_94;
static NJS_OBJECT collision_95;
static NJS_OBJECT collision_96;
static NJS_OBJECT collision_101;
static NJS_OBJECT collision_102;
static NJS_OBJECT collision_103;
static NJS_OBJECT collision_104;
static NJS_OBJECT collision_105;
static NJS_OBJECT collision_106;
static NJS_OBJECT collision_107;
static NJS_OBJECT collision_108;
static NJS_OBJECT collision_109;
static NJS_OBJECT collision_110;
static NJS_OBJECT collision_111;
static NJS_OBJECT collision_112;
static NJS_OBJECT collision_113;
static NJS_OBJECT collision_114;
static NJS_OBJECT collision_115;
static NJS_OBJECT collision_116;
static NJS_OBJECT collision_117;
static NJS_OBJECT collision_118;
static NJS_OBJECT collision_119;
static NJS_OBJECT collision_120;
static NJS_OBJECT collision_122;
static NJS_OBJECT collision_123;
static NJS_OBJECT collision_124;
static NJS_OBJECT collision_125;
static NJS_OBJECT collision_126;
static NJS_OBJECT collision_127;
static NJS_OBJECT collision_128;
static NJS_OBJECT collision_129;
static NJS_OBJECT collision_130;
static NJS_OBJECT collision_132;
static NJS_OBJECT collision_134;
static NJS_OBJECT collision_137;
static NJS_OBJECT collision_139;
static NJS_OBJECT collision_141;
static NJS_OBJECT collision_143;
static NJS_OBJECT collision_144;
static NJS_OBJECT collision_145;
static NJS_OBJECT collision_146;
static NJS_OBJECT collision_147;
static NJS_OBJECT collision_148;
static NJS_OBJECT collision_149;
static NJS_OBJECT collision_150;
static NJS_OBJECT collision_151;
static NJS_OBJECT collision_152;
static NJS_OBJECT collision_153;
static NJS_OBJECT collision_154;
static NJS_OBJECT collision_155;
static NJS_OBJECT collision_157;
static NJS_OBJECT collision_158;
static NJS_OBJECT collision_159;
static NJS_OBJECT collision_161;
static NJS_OBJECT collision_162;
static NJS_OBJECT collision_164;
static NJS_OBJECT collision_165;
static NJS_OBJECT collision_166;
static NJS_OBJECT collision_167;
static NJS_OBJECT collision_168;
static NJS_OBJECT collision_169;
static NJS_OBJECT collision_171;
static NJS_OBJECT collision_172;
static NJS_OBJECT collision_173;
static NJS_OBJECT collision_174;
static NJS_OBJECT collision_176;
static NJS_OBJECT collision_177;
static NJS_OBJECT collision_178;
static NJS_OBJECT collision_179;
static NJS_OBJECT collision_180;
static NJS_OBJECT collision_181;
static NJS_CNK_OBJECT draw_244;
static NJS_CNK_OBJECT draw_243;
static NJS_CNK_OBJECT draw_242;
static NJS_CNK_OBJECT draw_241;
static NJS_CNK_OBJECT draw_240;
static NJS_CNK_OBJECT draw_239;
static NJS_CNK_OBJECT draw_238;
static NJS_CNK_OBJECT draw_236;
static NJS_CNK_OBJECT draw_227;
static NJS_CNK_OBJECT draw_226;
static NJS_CNK_OBJECT draw_225;
static NJS_CNK_OBJECT draw_221;
static NJS_CNK_OBJECT draw_220;
static NJS_CNK_OBJECT draw_214;
static NJS_CNK_OBJECT draw_213;
static NJS_CNK_OBJECT draw_210;
static NJS_CNK_OBJECT draw_208;
static NJS_CNK_OBJECT draw_205;
static NJS_CNK_OBJECT draw_202;
static NJS_CNK_OBJECT draw_200;
static NJS_CNK_OBJECT draw_199;
static NJS_CNK_OBJECT draw_198;
static NJS_CNK_OBJECT draw_197;
static NJS_CNK_OBJECT draw_196;
static NJS_CNK_OBJECT draw_195;
static NJS_CNK_OBJECT draw_194;
static NJS_CNK_OBJECT draw_193;
static NJS_CNK_OBJECT draw_192;
static NJS_CNK_OBJECT draw_190;
static NJS_CNK_OBJECT draw_188;
static NJS_CNK_OBJECT draw_186;
static NJS_CNK_OBJECT draw_182;
static NJS_CNK_OBJECT draw_180;
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
static NJS_CNK_OBJECT draw_157;
static NJS_CNK_OBJECT draw_156;
static NJS_CNK_OBJECT draw_155;
static NJS_CNK_OBJECT draw_154;
static NJS_CNK_OBJECT draw_153;
static NJS_CNK_OBJECT draw_152;
static NJS_CNK_OBJECT draw_151;
static NJS_CNK_OBJECT draw_150;
static NJS_CNK_OBJECT draw_149;
static NJS_CNK_OBJECT draw_147;
static NJS_CNK_OBJECT draw_145;
static NJS_CNK_OBJECT draw_144;
static NJS_CNK_OBJECT draw_143;
static NJS_CNK_OBJECT draw_142;
static NJS_CNK_OBJECT draw_141;
static NJS_CNK_OBJECT draw_140;
static NJS_CNK_OBJECT draw_139;
static NJS_CNK_OBJECT draw_138;
static NJS_CNK_OBJECT draw_137;
static NJS_CNK_OBJECT draw_135;
static NJS_CNK_OBJECT draw_134;
static NJS_CNK_OBJECT draw_132;
static NJS_CNK_OBJECT draw_130;
static NJS_CNK_OBJECT draw_129;
static NJS_CNK_OBJECT draw_128;
static NJS_CNK_OBJECT draw_126;
static NJS_CNK_OBJECT draw_124;
static NJS_CNK_OBJECT draw_122;
static NJS_CNK_OBJECT draw_121;
static NJS_CNK_OBJECT draw_119;
static NJS_CNK_OBJECT draw_118;
static NJS_CNK_OBJECT draw_117;
static NJS_CNK_OBJECT draw_116;
static NJS_CNK_OBJECT draw_115;
static NJS_CNK_OBJECT draw_114;
static NJS_CNK_OBJECT draw_113;
static NJS_CNK_OBJECT draw_111;
static NJS_CNK_OBJECT draw_110;
static NJS_CNK_OBJECT draw_109;
static NJS_CNK_OBJECT draw_108;
static NJS_CNK_OBJECT draw_107;
static NJS_CNK_OBJECT draw_105;
static NJS_CNK_OBJECT draw_104;
static NJS_CNK_OBJECT draw_103;
static NJS_CNK_OBJECT draw_102;
static NJS_CNK_OBJECT draw_101;
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
static NJS_CNK_OBJECT draw_86;
static NJS_CNK_OBJECT draw_85;
static NJS_CNK_OBJECT draw_84;
static NJS_CNK_OBJECT draw_83;
static NJS_CNK_OBJECT draw_82;
static NJS_CNK_OBJECT draw_81;
static NJS_CNK_OBJECT draw_80;
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
static NJS_CNK_OBJECT draw_66;
static NJS_CNK_OBJECT draw_65;
static NJS_CNK_OBJECT draw_64;
static NJS_CNK_OBJECT draw_63;
static NJS_CNK_OBJECT draw_58;
static NJS_CNK_OBJECT draw_57;
static NJS_CNK_OBJECT draw_56;
static NJS_CNK_OBJECT draw_55;
static NJS_CNK_OBJECT draw_54;
static NJS_CNK_OBJECT draw_52;
static NJS_CNK_OBJECT draw_51;
static NJS_CNK_OBJECT draw_50;
static NJS_CNK_OBJECT draw_49;
static NJS_CNK_OBJECT draw_48;
static NJS_CNK_OBJECT draw_46;
static NJS_CNK_OBJECT draw_35;
static NJS_CNK_OBJECT draw_34;
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

static NJS_MKEY_A upgrade_city_escape_motion_joint_4_angles[16] = {
#include "stg28/terrain/upgrade_city_escape_motion_joint_4_angles.inc"
};
static NJS_MKEY_A upgrade_city_escape_motion_joint_7_angles[16] = {
#include "stg28/terrain/upgrade_city_escape_motion_joint_7_angles.inc"
};
static NJS_MKEY_A upgrade_city_escape_motion_joint_8_angles[14] = {
#include "stg28/terrain/upgrade_city_escape_motion_joint_8_angles.inc"
};
static NJS_MKEY_A upgrade_city_escape_motion_joint_9_angles[16] = {
#include "stg28/terrain/upgrade_city_escape_motion_joint_9_angles.inc"
};
static NJS_MKEY_A upgrade_city_escape_motion_joint_13_angles[16] = {
#include "stg28/terrain/upgrade_city_escape_motion_joint_13_angles.inc"
};
static NJS_MKEY_A upgrade_city_escape_motion_joint_14_angles[2] = {
#include "stg28/terrain/upgrade_city_escape_motion_joint_14_angles.inc"
};
static NJS_MKEY_A upgrade_city_escape_motion_joint_17_angles[16] = {
#include "stg28/terrain/upgrade_city_escape_motion_joint_17_angles.inc"
};
static NJS_MKEY_A upgrade_city_escape_motion_joint_18_angles[2] = {
#include "stg28/terrain/upgrade_city_escape_motion_joint_18_angles.inc"
};
static NJS_MKEY_A upgrade_city_escape_motion_joint_19_angles[8] = {
#include "stg28/terrain/upgrade_city_escape_motion_joint_19_angles.inc"
};
static NJS_MKEY_A upgrade_city_escape_motion_joint_23_angles[16] = {
#include "stg28/terrain/upgrade_city_escape_motion_joint_23_angles.inc"
};
static NJS_MKEY_A upgrade_city_escape_motion_joint_24_angles[2] = {
#include "stg28/terrain/upgrade_city_escape_motion_joint_24_angles.inc"
};
static NJS_MKEY_A upgrade_city_escape_motion_joint_27_angles[16] = {
#include "stg28/terrain/upgrade_city_escape_motion_joint_27_angles.inc"
};
static NJS_MKEY_A upgrade_city_escape_motion_joint_31_angles[2] = {
#include "stg28/terrain/upgrade_city_escape_motion_joint_31_angles.inc"
};
static NJS_MKEY_A upgrade_city_escape_motion_joint_32_angles[16] = {
#include "stg28/terrain/upgrade_city_escape_motion_joint_32_angles.inc"
};
static NJS_MKEY_A upgrade_city_escape_motion_joint_33_angles[12] = {
#include "stg28/terrain/upgrade_city_escape_motion_joint_33_angles.inc"
};
static NJS_MKEY_A upgrade_city_escape_motion_joint_37_angles[16] = {
#include "stg28/terrain/upgrade_city_escape_motion_joint_37_angles.inc"
};
static NJS_MKEY_A upgrade_city_escape_motion_joint_38_angles[16] = {
#include "stg28/terrain/upgrade_city_escape_motion_joint_38_angles.inc"
};
static NJS_MKEY_A upgrade_city_escape_motion_joint_39_angles[16] = {
#include "stg28/terrain/upgrade_city_escape_motion_joint_39_angles.inc"
};
static NJS_MKEY_A upgrade_city_escape_motion_joint_41_angles[16] = {
#include "stg28/terrain/upgrade_city_escape_motion_joint_41_angles.inc"
};
static NJS_MKEY_A upgrade_city_escape_motion_joint_44_angles[16] = {
#include "stg28/terrain/upgrade_city_escape_motion_joint_44_angles.inc"
};
static NJS_MKEY_A upgrade_city_escape_motion_joint_51_angles[16] = {
#include "stg28/terrain/upgrade_city_escape_motion_joint_51_angles.inc"
};
static NJS_MKEY_A upgrade_city_escape_motion_joint_53_angles[16] = {
#include "stg28/terrain/upgrade_city_escape_motion_joint_53_angles.inc"
};
static NJS_MKEY_A upgrade_city_escape_motion_joint_56_angles[16] = {
#include "stg28/terrain/upgrade_city_escape_motion_joint_56_angles.inc"
};
static NJS_MKEY_A upgrade_city_escape_motion_joint_57_angles[16] = {
#include "stg28/terrain/upgrade_city_escape_motion_joint_57_angles.inc"
};
static NJS_MKEY_A upgrade_city_escape_motion_joint_58_angles[16] = {
#include "stg28/terrain/upgrade_city_escape_motion_joint_58_angles.inc"
};
static NJS_MKEY_A upgrade_city_escape_motion_joint_60_angles[16] = {
#include "stg28/terrain/upgrade_city_escape_motion_joint_60_angles.inc"
};
static NJS_MDATA2 upgrade_city_escape_motion_tracks[62] = {
#include "stg28/terrain/upgrade_city_escape_motion_tracks.inc"
};
static NJS_MOTION upgrade_city_escape_motion = {
#include "stg28/terrain/upgrade_city_escape_motion.inc"
};
static NJS_MKEY_F bar_throw_back_motion_joint_0_position[2] = {
#include "stg28/terrain/bar_throw_back_motion_joint_0_position.inc"
};
static NJS_MKEY_F bar_throw_back_motion_joint_2_position[2] = {
#include "stg28/terrain/bar_throw_back_motion_joint_2_position.inc"
};
static NJS_MKEY_A bar_throw_back_motion_joint_4_angles[2] = {
#include "stg28/terrain/bar_throw_back_motion_joint_4_angles.inc"
};
static NJS_MKEY_A bar_throw_back_motion_joint_7_angles[32] = {
#include "stg28/terrain/bar_throw_back_motion_joint_7_angles.inc"
};
static NJS_MKEY_A bar_throw_back_motion_joint_8_angles[32] = {
#include "stg28/terrain/bar_throw_back_motion_joint_8_angles.inc"
};
static NJS_MKEY_A bar_throw_back_motion_joint_9_angles[32] = {
#include "stg28/terrain/bar_throw_back_motion_joint_9_angles.inc"
};
static NJS_MKEY_A bar_throw_back_motion_joint_13_angles[32] = {
#include "stg28/terrain/bar_throw_back_motion_joint_13_angles.inc"
};
static NJS_MKEY_A bar_throw_back_motion_joint_14_angles[32] = {
#include "stg28/terrain/bar_throw_back_motion_joint_14_angles.inc"
};
static NJS_MKEY_A bar_throw_back_motion_joint_17_angles[32] = {
#include "stg28/terrain/bar_throw_back_motion_joint_17_angles.inc"
};
static NJS_MKEY_A bar_throw_back_motion_joint_18_angles[32] = {
#include "stg28/terrain/bar_throw_back_motion_joint_18_angles.inc"
};
static NJS_MKEY_A bar_throw_back_motion_joint_19_angles[32] = {
#include "stg28/terrain/bar_throw_back_motion_joint_19_angles.inc"
};
static NJS_MKEY_A bar_throw_back_motion_joint_23_angles[32] = {
#include "stg28/terrain/bar_throw_back_motion_joint_23_angles.inc"
};
static NJS_MKEY_A bar_throw_back_motion_joint_24_angles[32] = {
#include "stg28/terrain/bar_throw_back_motion_joint_24_angles.inc"
};
static NJS_MKEY_A bar_throw_back_motion_joint_27_angles[2] = {
#include "stg28/terrain/bar_throw_back_motion_joint_27_angles.inc"
};
static NJS_MKEY_A bar_throw_back_motion_joint_28_angles[2] = {
#include "stg28/terrain/bar_throw_back_motion_joint_28_angles.inc"
};
static NJS_MKEY_A bar_throw_back_motion_joint_29_angles[2] = {
#include "stg28/terrain/bar_throw_back_motion_joint_29_angles.inc"
};
static NJS_MKEY_A bar_throw_back_motion_joint_32_angles[32] = {
#include "stg28/terrain/bar_throw_back_motion_joint_32_angles.inc"
};
static NJS_MKEY_A bar_throw_back_motion_joint_33_angles[25] = {
#include "stg28/terrain/bar_throw_back_motion_joint_33_angles.inc"
};
static NJS_MKEY_A bar_throw_back_motion_joint_37_angles[32] = {
#include "stg28/terrain/bar_throw_back_motion_joint_37_angles.inc"
};
static NJS_MKEY_A bar_throw_back_motion_joint_38_angles[32] = {
#include "stg28/terrain/bar_throw_back_motion_joint_38_angles.inc"
};
static NJS_MKEY_A bar_throw_back_motion_joint_39_angles[32] = {
#include "stg28/terrain/bar_throw_back_motion_joint_39_angles.inc"
};
static NJS_MKEY_A bar_throw_back_motion_joint_41_angles[17] = {
#include "stg28/terrain/bar_throw_back_motion_joint_41_angles.inc"
};
static NJS_MKEY_A bar_throw_back_motion_joint_44_angles[32] = {
#include "stg28/terrain/bar_throw_back_motion_joint_44_angles.inc"
};
static NJS_MKEY_A bar_throw_back_motion_joint_51_angles[32] = {
#include "stg28/terrain/bar_throw_back_motion_joint_51_angles.inc"
};
static NJS_MKEY_A bar_throw_back_motion_joint_53_angles[32] = {
#include "stg28/terrain/bar_throw_back_motion_joint_53_angles.inc"
};
static NJS_MKEY_A bar_throw_back_motion_joint_56_angles[32] = {
#include "stg28/terrain/bar_throw_back_motion_joint_56_angles.inc"
};
static NJS_MKEY_A bar_throw_back_motion_joint_57_angles[32] = {
#include "stg28/terrain/bar_throw_back_motion_joint_57_angles.inc"
};
static NJS_MKEY_A bar_throw_back_motion_joint_58_angles[32] = {
#include "stg28/terrain/bar_throw_back_motion_joint_58_angles.inc"
};
static NJS_MKEY_A bar_throw_back_motion_joint_60_angles[17] = {
#include "stg28/terrain/bar_throw_back_motion_joint_60_angles.inc"
};
static NJS_MDATA2 bar_throw_back_motion_tracks[62] = {
#include "stg28/terrain/bar_throw_back_motion_tracks.inc"
};
static NJS_MOTION bar_throw_back_motion = {
#include "stg28/terrain/bar_throw_back_motion.inc"
};
static NJS_MKEY_F bar_throw_motion_joint_0_position[2] = {
#include "stg28/terrain/bar_throw_motion_joint_0_position.inc"
};
static NJS_MKEY_F bar_throw_motion_joint_2_position[2] = {
#include "stg28/terrain/bar_throw_motion_joint_2_position.inc"
};
static NJS_MKEY_A bar_throw_motion_joint_4_angles[33] = {
#include "stg28/terrain/bar_throw_motion_joint_4_angles.inc"
};
static NJS_MKEY_A bar_throw_motion_joint_7_angles[33] = {
#include "stg28/terrain/bar_throw_motion_joint_7_angles.inc"
};
static NJS_MKEY_A bar_throw_motion_joint_8_angles[33] = {
#include "stg28/terrain/bar_throw_motion_joint_8_angles.inc"
};
static NJS_MKEY_A bar_throw_motion_joint_9_angles[33] = {
#include "stg28/terrain/bar_throw_motion_joint_9_angles.inc"
};
static NJS_MKEY_A bar_throw_motion_joint_13_angles[33] = {
#include "stg28/terrain/bar_throw_motion_joint_13_angles.inc"
};
static NJS_MKEY_A bar_throw_motion_joint_14_angles[33] = {
#include "stg28/terrain/bar_throw_motion_joint_14_angles.inc"
};
static NJS_MKEY_A bar_throw_motion_joint_17_angles[33] = {
#include "stg28/terrain/bar_throw_motion_joint_17_angles.inc"
};
static NJS_MKEY_A bar_throw_motion_joint_18_angles[33] = {
#include "stg28/terrain/bar_throw_motion_joint_18_angles.inc"
};
static NJS_MKEY_A bar_throw_motion_joint_19_angles[33] = {
#include "stg28/terrain/bar_throw_motion_joint_19_angles.inc"
};
static NJS_MKEY_A bar_throw_motion_joint_23_angles[33] = {
#include "stg28/terrain/bar_throw_motion_joint_23_angles.inc"
};
static NJS_MKEY_A bar_throw_motion_joint_24_angles[33] = {
#include "stg28/terrain/bar_throw_motion_joint_24_angles.inc"
};
static NJS_MKEY_A bar_throw_motion_joint_27_angles[33] = {
#include "stg28/terrain/bar_throw_motion_joint_27_angles.inc"
};
static NJS_MKEY_A bar_throw_motion_joint_28_angles[15] = {
#include "stg28/terrain/bar_throw_motion_joint_28_angles.inc"
};
static NJS_MKEY_A bar_throw_motion_joint_29_angles[15] = {
#include "stg28/terrain/bar_throw_motion_joint_29_angles.inc"
};
static NJS_MKEY_A bar_throw_motion_joint_32_angles[33] = {
#include "stg28/terrain/bar_throw_motion_joint_32_angles.inc"
};
static NJS_MKEY_A bar_throw_motion_joint_33_angles[26] = {
#include "stg28/terrain/bar_throw_motion_joint_33_angles.inc"
};
static NJS_MKEY_A bar_throw_motion_joint_37_angles[33] = {
#include "stg28/terrain/bar_throw_motion_joint_37_angles.inc"
};
static NJS_MKEY_A bar_throw_motion_joint_38_angles[33] = {
#include "stg28/terrain/bar_throw_motion_joint_38_angles.inc"
};
static NJS_MKEY_A bar_throw_motion_joint_39_angles[33] = {
#include "stg28/terrain/bar_throw_motion_joint_39_angles.inc"
};
static NJS_MKEY_A bar_throw_motion_joint_41_angles[2] = {
#include "stg28/terrain/bar_throw_motion_joint_41_angles.inc"
};
static NJS_MKEY_A bar_throw_motion_joint_44_angles[33] = {
#include "stg28/terrain/bar_throw_motion_joint_44_angles.inc"
};
static NJS_MKEY_A bar_throw_motion_joint_51_angles[33] = {
#include "stg28/terrain/bar_throw_motion_joint_51_angles.inc"
};
static NJS_MKEY_A bar_throw_motion_joint_53_angles[33] = {
#include "stg28/terrain/bar_throw_motion_joint_53_angles.inc"
};
static NJS_MKEY_A bar_throw_motion_joint_56_angles[33] = {
#include "stg28/terrain/bar_throw_motion_joint_56_angles.inc"
};
static NJS_MKEY_A bar_throw_motion_joint_57_angles[33] = {
#include "stg28/terrain/bar_throw_motion_joint_57_angles.inc"
};
static NJS_MKEY_A bar_throw_motion_joint_58_angles[33] = {
#include "stg28/terrain/bar_throw_motion_joint_58_angles.inc"
};
static NJS_MKEY_A bar_throw_motion_joint_60_angles[2] = {
#include "stg28/terrain/bar_throw_motion_joint_60_angles.inc"
};
static NJS_MDATA2 bar_throw_motion_tracks[62] = {
#include "stg28/terrain/bar_throw_motion_tracks.inc"
};
static NJS_MOTION bar_throw_motion = {
#include "stg28/terrain/bar_throw_motion.inc"
};
static NJS_MKEY_F bar_fly_back_motion_joint_0_position[2] = {
#include "stg28/terrain/bar_fly_back_motion_joint_0_position.inc"
};
static NJS_MKEY_F bar_fly_back_motion_joint_2_position[2] = {
#include "stg28/terrain/bar_fly_back_motion_joint_2_position.inc"
};
static NJS_MKEY_A bar_fly_back_motion_joint_4_angles[2] = {
#include "stg28/terrain/bar_fly_back_motion_joint_4_angles.inc"
};
static NJS_MKEY_A bar_fly_back_motion_joint_7_angles[2] = {
#include "stg28/terrain/bar_fly_back_motion_joint_7_angles.inc"
};
static NJS_MKEY_A bar_fly_back_motion_joint_8_angles[2] = {
#include "stg28/terrain/bar_fly_back_motion_joint_8_angles.inc"
};
static NJS_MKEY_A bar_fly_back_motion_joint_9_angles[2] = {
#include "stg28/terrain/bar_fly_back_motion_joint_9_angles.inc"
};
static NJS_MKEY_A bar_fly_back_motion_joint_13_angles[2] = {
#include "stg28/terrain/bar_fly_back_motion_joint_13_angles.inc"
};
static NJS_MKEY_A bar_fly_back_motion_joint_14_angles[2] = {
#include "stg28/terrain/bar_fly_back_motion_joint_14_angles.inc"
};
static NJS_MKEY_A bar_fly_back_motion_joint_17_angles[2] = {
#include "stg28/terrain/bar_fly_back_motion_joint_17_angles.inc"
};
static NJS_MKEY_A bar_fly_back_motion_joint_18_angles[2] = {
#include "stg28/terrain/bar_fly_back_motion_joint_18_angles.inc"
};
static NJS_MKEY_A bar_fly_back_motion_joint_19_angles[2] = {
#include "stg28/terrain/bar_fly_back_motion_joint_19_angles.inc"
};
static NJS_MKEY_A bar_fly_back_motion_joint_23_angles[2] = {
#include "stg28/terrain/bar_fly_back_motion_joint_23_angles.inc"
};
static NJS_MKEY_A bar_fly_back_motion_joint_24_angles[2] = {
#include "stg28/terrain/bar_fly_back_motion_joint_24_angles.inc"
};
static NJS_MKEY_A bar_fly_back_motion_joint_27_angles[2] = {
#include "stg28/terrain/bar_fly_back_motion_joint_27_angles.inc"
};
static NJS_MKEY_A bar_fly_back_motion_joint_32_angles[2] = {
#include "stg28/terrain/bar_fly_back_motion_joint_32_angles.inc"
};
static NJS_MKEY_A bar_fly_back_motion_joint_37_angles[2] = {
#include "stg28/terrain/bar_fly_back_motion_joint_37_angles.inc"
};
static NJS_MKEY_A bar_fly_back_motion_joint_38_angles[2] = {
#include "stg28/terrain/bar_fly_back_motion_joint_38_angles.inc"
};
static NJS_MKEY_A bar_fly_back_motion_joint_39_angles[2] = {
#include "stg28/terrain/bar_fly_back_motion_joint_39_angles.inc"
};
static NJS_MKEY_A bar_fly_back_motion_joint_41_angles[2] = {
#include "stg28/terrain/bar_fly_back_motion_joint_41_angles.inc"
};
static NJS_MKEY_A bar_fly_back_motion_joint_44_angles[2] = {
#include "stg28/terrain/bar_fly_back_motion_joint_44_angles.inc"
};
static NJS_MKEY_A bar_fly_back_motion_joint_51_angles[2] = {
#include "stg28/terrain/bar_fly_back_motion_joint_51_angles.inc"
};
static NJS_MKEY_A bar_fly_back_motion_joint_53_angles[2] = {
#include "stg28/terrain/bar_fly_back_motion_joint_53_angles.inc"
};
static NJS_MKEY_A bar_fly_back_motion_joint_56_angles[6] = {
#include "stg28/terrain/bar_fly_back_motion_joint_56_angles.inc"
};
static NJS_MKEY_A bar_fly_back_motion_joint_57_angles[2] = {
#include "stg28/terrain/bar_fly_back_motion_joint_57_angles.inc"
};
static NJS_MKEY_A bar_fly_back_motion_joint_58_angles[3] = {
#include "stg28/terrain/bar_fly_back_motion_joint_58_angles.inc"
};
static NJS_MKEY_A bar_fly_back_motion_joint_60_angles[2] = {
#include "stg28/terrain/bar_fly_back_motion_joint_60_angles.inc"
};
static NJS_MDATA2 bar_fly_back_motion_tracks[62] = {
#include "stg28/terrain/bar_fly_back_motion_tracks.inc"
};
static NJS_MOTION bar_fly_back_motion = {
#include "stg28/terrain/bar_fly_back_motion.inc"
};
static NJS_MKEY_F bar_fly_motion_joint_0_position[2] = {
#include "stg28/terrain/bar_fly_motion_joint_0_position.inc"
};
static NJS_MKEY_F bar_fly_motion_joint_2_position[2] = {
#include "stg28/terrain/bar_fly_motion_joint_2_position.inc"
};
static NJS_MKEY_A bar_fly_motion_joint_4_angles[2] = {
#include "stg28/terrain/bar_fly_motion_joint_4_angles.inc"
};
static NJS_MKEY_A bar_fly_motion_joint_7_angles[2] = {
#include "stg28/terrain/bar_fly_motion_joint_7_angles.inc"
};
static NJS_MKEY_A bar_fly_motion_joint_8_angles[2] = {
#include "stg28/terrain/bar_fly_motion_joint_8_angles.inc"
};
static NJS_MKEY_A bar_fly_motion_joint_9_angles[2] = {
#include "stg28/terrain/bar_fly_motion_joint_9_angles.inc"
};
static NJS_MKEY_A bar_fly_motion_joint_13_angles[2] = {
#include "stg28/terrain/bar_fly_motion_joint_13_angles.inc"
};
static NJS_MKEY_A bar_fly_motion_joint_14_angles[2] = {
#include "stg28/terrain/bar_fly_motion_joint_14_angles.inc"
};
static NJS_MKEY_A bar_fly_motion_joint_17_angles[2] = {
#include "stg28/terrain/bar_fly_motion_joint_17_angles.inc"
};
static NJS_MKEY_A bar_fly_motion_joint_18_angles[2] = {
#include "stg28/terrain/bar_fly_motion_joint_18_angles.inc"
};
static NJS_MKEY_A bar_fly_motion_joint_19_angles[2] = {
#include "stg28/terrain/bar_fly_motion_joint_19_angles.inc"
};
static NJS_MKEY_A bar_fly_motion_joint_23_angles[2] = {
#include "stg28/terrain/bar_fly_motion_joint_23_angles.inc"
};
static NJS_MKEY_A bar_fly_motion_joint_24_angles[2] = {
#include "stg28/terrain/bar_fly_motion_joint_24_angles.inc"
};
static NJS_MKEY_A bar_fly_motion_joint_27_angles[2] = {
#include "stg28/terrain/bar_fly_motion_joint_27_angles.inc"
};
static NJS_MKEY_A bar_fly_motion_joint_32_angles[2] = {
#include "stg28/terrain/bar_fly_motion_joint_32_angles.inc"
};
static NJS_MKEY_A bar_fly_motion_joint_37_angles[2] = {
#include "stg28/terrain/bar_fly_motion_joint_37_angles.inc"
};
static NJS_MKEY_A bar_fly_motion_joint_38_angles[2] = {
#include "stg28/terrain/bar_fly_motion_joint_38_angles.inc"
};
static NJS_MKEY_A bar_fly_motion_joint_39_angles[2] = {
#include "stg28/terrain/bar_fly_motion_joint_39_angles.inc"
};
static NJS_MKEY_A bar_fly_motion_joint_41_angles[2] = {
#include "stg28/terrain/bar_fly_motion_joint_41_angles.inc"
};
static NJS_MKEY_A bar_fly_motion_joint_44_angles[2] = {
#include "stg28/terrain/bar_fly_motion_joint_44_angles.inc"
};
static NJS_MKEY_A bar_fly_motion_joint_51_angles[2] = {
#include "stg28/terrain/bar_fly_motion_joint_51_angles.inc"
};
static NJS_MKEY_A bar_fly_motion_joint_53_angles[2] = {
#include "stg28/terrain/bar_fly_motion_joint_53_angles.inc"
};
static NJS_MKEY_A bar_fly_motion_joint_56_angles[3] = {
#include "stg28/terrain/bar_fly_motion_joint_56_angles.inc"
};
static NJS_MKEY_A bar_fly_motion_joint_57_angles[2] = {
#include "stg28/terrain/bar_fly_motion_joint_57_angles.inc"
};
static NJS_MKEY_A bar_fly_motion_joint_58_angles[2] = {
#include "stg28/terrain/bar_fly_motion_joint_58_angles.inc"
};
static NJS_MKEY_A bar_fly_motion_joint_60_angles[2] = {
#include "stg28/terrain/bar_fly_motion_joint_60_angles.inc"
};
static NJS_MDATA2 bar_fly_motion_tracks[62] = {
#include "stg28/terrain/bar_fly_motion_tracks.inc"
};
static NJS_MOTION bar_fly_motion = {
#include "stg28/terrain/bar_fly_motion.inc"
};
static NJS_MKEY_F bar_swing_back_motion_joint_0_position[2] = {
#include "stg28/terrain/bar_swing_back_motion_joint_0_position.inc"
};
static NJS_MKEY_F bar_swing_back_motion_joint_2_position[2] = {
#include "stg28/terrain/bar_swing_back_motion_joint_2_position.inc"
};
static NJS_MKEY_A bar_swing_back_motion_joint_4_angles[32] = {
#include "stg28/terrain/bar_swing_back_motion_joint_4_angles.inc"
};
static NJS_MKEY_A bar_swing_back_motion_joint_7_angles[32] = {
#include "stg28/terrain/bar_swing_back_motion_joint_7_angles.inc"
};
static NJS_MKEY_A bar_swing_back_motion_joint_8_angles[32] = {
#include "stg28/terrain/bar_swing_back_motion_joint_8_angles.inc"
};
static NJS_MKEY_A bar_swing_back_motion_joint_9_angles[32] = {
#include "stg28/terrain/bar_swing_back_motion_joint_9_angles.inc"
};
static NJS_MKEY_A bar_swing_back_motion_joint_13_angles[32] = {
#include "stg28/terrain/bar_swing_back_motion_joint_13_angles.inc"
};
static NJS_MKEY_A bar_swing_back_motion_joint_14_angles[25] = {
#include "stg28/terrain/bar_swing_back_motion_joint_14_angles.inc"
};
static NJS_MKEY_A bar_swing_back_motion_joint_17_angles[32] = {
#include "stg28/terrain/bar_swing_back_motion_joint_17_angles.inc"
};
static NJS_MKEY_A bar_swing_back_motion_joint_18_angles[32] = {
#include "stg28/terrain/bar_swing_back_motion_joint_18_angles.inc"
};
static NJS_MKEY_A bar_swing_back_motion_joint_19_angles[32] = {
#include "stg28/terrain/bar_swing_back_motion_joint_19_angles.inc"
};
static NJS_MKEY_A bar_swing_back_motion_joint_23_angles[32] = {
#include "stg28/terrain/bar_swing_back_motion_joint_23_angles.inc"
};
static NJS_MKEY_A bar_swing_back_motion_joint_24_angles[32] = {
#include "stg28/terrain/bar_swing_back_motion_joint_24_angles.inc"
};
static NJS_MKEY_A bar_swing_back_motion_joint_27_angles[32] = {
#include "stg28/terrain/bar_swing_back_motion_joint_27_angles.inc"
};
static NJS_MKEY_A bar_swing_back_motion_joint_28_angles[28] = {
#include "stg28/terrain/bar_swing_back_motion_joint_28_angles.inc"
};
static NJS_MKEY_A bar_swing_back_motion_joint_29_angles[29] = {
#include "stg28/terrain/bar_swing_back_motion_joint_29_angles.inc"
};
static NJS_MKEY_A bar_swing_back_motion_joint_31_angles[2] = {
#include "stg28/terrain/bar_swing_back_motion_joint_31_angles.inc"
};
static NJS_MKEY_A bar_swing_back_motion_joint_32_angles[2] = {
#include "stg28/terrain/bar_swing_back_motion_joint_32_angles.inc"
};
static NJS_MKEY_A bar_swing_back_motion_joint_33_angles[2] = {
#include "stg28/terrain/bar_swing_back_motion_joint_33_angles.inc"
};
static NJS_MKEY_A bar_swing_back_motion_joint_37_angles[2] = {
#include "stg28/terrain/bar_swing_back_motion_joint_37_angles.inc"
};
static NJS_MKEY_A bar_swing_back_motion_joint_38_angles[2] = {
#include "stg28/terrain/bar_swing_back_motion_joint_38_angles.inc"
};
static NJS_MKEY_A bar_swing_back_motion_joint_39_angles[2] = {
#include "stg28/terrain/bar_swing_back_motion_joint_39_angles.inc"
};
static NJS_MKEY_A bar_swing_back_motion_joint_41_angles[2] = {
#include "stg28/terrain/bar_swing_back_motion_joint_41_angles.inc"
};
static NJS_MKEY_A bar_swing_back_motion_joint_44_angles[32] = {
#include "stg28/terrain/bar_swing_back_motion_joint_44_angles.inc"
};
static NJS_MKEY_A bar_swing_back_motion_joint_51_angles[32] = {
#include "stg28/terrain/bar_swing_back_motion_joint_51_angles.inc"
};
static NJS_MKEY_A bar_swing_back_motion_joint_53_angles[32] = {
#include "stg28/terrain/bar_swing_back_motion_joint_53_angles.inc"
};
static NJS_MKEY_A bar_swing_back_motion_joint_56_angles[2] = {
#include "stg28/terrain/bar_swing_back_motion_joint_56_angles.inc"
};
static NJS_MKEY_A bar_swing_back_motion_joint_57_angles[2] = {
#include "stg28/terrain/bar_swing_back_motion_joint_57_angles.inc"
};
static NJS_MKEY_A bar_swing_back_motion_joint_58_angles[2] = {
#include "stg28/terrain/bar_swing_back_motion_joint_58_angles.inc"
};
static NJS_MKEY_A bar_swing_back_motion_joint_60_angles[2] = {
#include "stg28/terrain/bar_swing_back_motion_joint_60_angles.inc"
};
static NJS_MDATA2 bar_swing_back_motion_tracks[62] = {
#include "stg28/terrain/bar_swing_back_motion_tracks.inc"
};
static NJS_MOTION bar_swing_back_motion = {
#include "stg28/terrain/bar_swing_back_motion.inc"
};
static NJS_MKEY_F bar_swing_motion_joint_0_position[2] = {
#include "stg28/terrain/bar_swing_motion_joint_0_position.inc"
};
static NJS_MKEY_F bar_swing_motion_joint_2_position[2] = {
#include "stg28/terrain/bar_swing_motion_joint_2_position.inc"
};
static NJS_MKEY_A bar_swing_motion_joint_4_angles[32] = {
#include "stg28/terrain/bar_swing_motion_joint_4_angles.inc"
};
static NJS_MKEY_A bar_swing_motion_joint_7_angles[32] = {
#include "stg28/terrain/bar_swing_motion_joint_7_angles.inc"
};
static NJS_MKEY_A bar_swing_motion_joint_8_angles[32] = {
#include "stg28/terrain/bar_swing_motion_joint_8_angles.inc"
};
static NJS_MKEY_A bar_swing_motion_joint_9_angles[32] = {
#include "stg28/terrain/bar_swing_motion_joint_9_angles.inc"
};
static NJS_MKEY_A bar_swing_motion_joint_13_angles[32] = {
#include "stg28/terrain/bar_swing_motion_joint_13_angles.inc"
};
static NJS_MKEY_A bar_swing_motion_joint_14_angles[28] = {
#include "stg28/terrain/bar_swing_motion_joint_14_angles.inc"
};
static NJS_MKEY_A bar_swing_motion_joint_17_angles[32] = {
#include "stg28/terrain/bar_swing_motion_joint_17_angles.inc"
};
static NJS_MKEY_A bar_swing_motion_joint_18_angles[32] = {
#include "stg28/terrain/bar_swing_motion_joint_18_angles.inc"
};
static NJS_MKEY_A bar_swing_motion_joint_19_angles[32] = {
#include "stg28/terrain/bar_swing_motion_joint_19_angles.inc"
};
static NJS_MKEY_A bar_swing_motion_joint_23_angles[32] = {
#include "stg28/terrain/bar_swing_motion_joint_23_angles.inc"
};
static NJS_MKEY_A bar_swing_motion_joint_24_angles[28] = {
#include "stg28/terrain/bar_swing_motion_joint_24_angles.inc"
};
static NJS_MKEY_A bar_swing_motion_joint_27_angles[32] = {
#include "stg28/terrain/bar_swing_motion_joint_27_angles.inc"
};
static NJS_MKEY_A bar_swing_motion_joint_28_angles[32] = {
#include "stg28/terrain/bar_swing_motion_joint_28_angles.inc"
};
static NJS_MKEY_A bar_swing_motion_joint_29_angles[32] = {
#include "stg28/terrain/bar_swing_motion_joint_29_angles.inc"
};
static NJS_MKEY_A bar_swing_motion_joint_31_angles[2] = {
#include "stg28/terrain/bar_swing_motion_joint_31_angles.inc"
};
static NJS_MKEY_A bar_swing_motion_joint_32_angles[2] = {
#include "stg28/terrain/bar_swing_motion_joint_32_angles.inc"
};
static NJS_MKEY_A bar_swing_motion_joint_33_angles[2] = {
#include "stg28/terrain/bar_swing_motion_joint_33_angles.inc"
};
static NJS_MKEY_A bar_swing_motion_joint_37_angles[2] = {
#include "stg28/terrain/bar_swing_motion_joint_37_angles.inc"
};
static NJS_MKEY_A bar_swing_motion_joint_38_angles[2] = {
#include "stg28/terrain/bar_swing_motion_joint_38_angles.inc"
};
static NJS_MKEY_A bar_swing_motion_joint_39_angles[2] = {
#include "stg28/terrain/bar_swing_motion_joint_39_angles.inc"
};
static NJS_MKEY_A bar_swing_motion_joint_41_angles[2] = {
#include "stg28/terrain/bar_swing_motion_joint_41_angles.inc"
};
static NJS_MKEY_A bar_swing_motion_joint_44_angles[32] = {
#include "stg28/terrain/bar_swing_motion_joint_44_angles.inc"
};
static NJS_MKEY_A bar_swing_motion_joint_51_angles[32] = {
#include "stg28/terrain/bar_swing_motion_joint_51_angles.inc"
};
static NJS_MKEY_A bar_swing_motion_joint_53_angles[32] = {
#include "stg28/terrain/bar_swing_motion_joint_53_angles.inc"
};
static NJS_MKEY_A bar_swing_motion_joint_56_angles[2] = {
#include "stg28/terrain/bar_swing_motion_joint_56_angles.inc"
};
static NJS_MKEY_A bar_swing_motion_joint_57_angles[2] = {
#include "stg28/terrain/bar_swing_motion_joint_57_angles.inc"
};
static NJS_MKEY_A bar_swing_motion_joint_58_angles[2] = {
#include "stg28/terrain/bar_swing_motion_joint_58_angles.inc"
};
static NJS_MKEY_A bar_swing_motion_joint_60_angles[2] = {
#include "stg28/terrain/bar_swing_motion_joint_60_angles.inc"
};
static NJS_MDATA2 bar_swing_motion_tracks[62] = {
#include "stg28/terrain/bar_swing_motion_tracks.inc"
};
static NJS_MOTION bar_swing_motion = {
#include "stg28/terrain/bar_swing_motion.inc"
};
static NJS_MKEY_F bar_land_motion_joint_2_position[9] = {
#include "stg28/terrain/bar_land_motion_joint_2_position.inc"
};
static NJS_MKEY_A bar_land_motion_joint_2_angles[2] = {
#include "stg28/terrain/bar_land_motion_joint_2_angles.inc"
};
static NJS_MKEY_A bar_land_motion_joint_4_angles[2] = {
#include "stg28/terrain/bar_land_motion_joint_4_angles.inc"
};
static NJS_MKEY_A bar_land_motion_joint_7_angles[9] = {
#include "stg28/terrain/bar_land_motion_joint_7_angles.inc"
};
static NJS_MKEY_A bar_land_motion_joint_8_angles[9] = {
#include "stg28/terrain/bar_land_motion_joint_8_angles.inc"
};
static NJS_MKEY_A bar_land_motion_joint_9_angles[9] = {
#include "stg28/terrain/bar_land_motion_joint_9_angles.inc"
};
static NJS_MKEY_A bar_land_motion_joint_13_angles[9] = {
#include "stg28/terrain/bar_land_motion_joint_13_angles.inc"
};
static NJS_MKEY_A bar_land_motion_joint_14_angles[2] = {
#include "stg28/terrain/bar_land_motion_joint_14_angles.inc"
};
static NJS_MKEY_A bar_land_motion_joint_17_angles[9] = {
#include "stg28/terrain/bar_land_motion_joint_17_angles.inc"
};
static NJS_MKEY_A bar_land_motion_joint_18_angles[9] = {
#include "stg28/terrain/bar_land_motion_joint_18_angles.inc"
};
static NJS_MKEY_A bar_land_motion_joint_19_angles[9] = {
#include "stg28/terrain/bar_land_motion_joint_19_angles.inc"
};
static NJS_MKEY_A bar_land_motion_joint_23_angles[9] = {
#include "stg28/terrain/bar_land_motion_joint_23_angles.inc"
};
static NJS_MKEY_A bar_land_motion_joint_24_angles[4] = {
#include "stg28/terrain/bar_land_motion_joint_24_angles.inc"
};
static NJS_MKEY_A bar_land_motion_joint_27_angles[9] = {
#include "stg28/terrain/bar_land_motion_joint_27_angles.inc"
};
static NJS_MKEY_A bar_land_motion_joint_28_angles[9] = {
#include "stg28/terrain/bar_land_motion_joint_28_angles.inc"
};
static NJS_MKEY_A bar_land_motion_joint_29_angles[9] = {
#include "stg28/terrain/bar_land_motion_joint_29_angles.inc"
};
static NJS_MKEY_A bar_land_motion_joint_32_angles[6] = {
#include "stg28/terrain/bar_land_motion_joint_32_angles.inc"
};
static NJS_MKEY_A bar_land_motion_joint_33_angles[6] = {
#include "stg28/terrain/bar_land_motion_joint_33_angles.inc"
};
static NJS_MKEY_A bar_land_motion_joint_37_angles[5] = {
#include "stg28/terrain/bar_land_motion_joint_37_angles.inc"
};
static NJS_MKEY_A bar_land_motion_joint_38_angles[5] = {
#include "stg28/terrain/bar_land_motion_joint_38_angles.inc"
};
static NJS_MKEY_A bar_land_motion_joint_39_angles[5] = {
#include "stg28/terrain/bar_land_motion_joint_39_angles.inc"
};
static NJS_MKEY_A bar_land_motion_joint_41_angles[9] = {
#include "stg28/terrain/bar_land_motion_joint_41_angles.inc"
};
static NJS_MKEY_A bar_land_motion_joint_44_angles[9] = {
#include "stg28/terrain/bar_land_motion_joint_44_angles.inc"
};
static NJS_MKEY_A bar_land_motion_joint_51_angles[9] = {
#include "stg28/terrain/bar_land_motion_joint_51_angles.inc"
};
static NJS_MKEY_A bar_land_motion_joint_53_angles[9] = {
#include "stg28/terrain/bar_land_motion_joint_53_angles.inc"
};
static NJS_MKEY_A bar_land_motion_joint_56_angles[8] = {
#include "stg28/terrain/bar_land_motion_joint_56_angles.inc"
};
static NJS_MKEY_A bar_land_motion_joint_57_angles[5] = {
#include "stg28/terrain/bar_land_motion_joint_57_angles.inc"
};
static NJS_MKEY_A bar_land_motion_joint_58_angles[5] = {
#include "stg28/terrain/bar_land_motion_joint_58_angles.inc"
};
static NJS_MKEY_A bar_land_motion_joint_60_angles[9] = {
#include "stg28/terrain/bar_land_motion_joint_60_angles.inc"
};
static NJS_MDATA2 bar_land_motion_tracks[62] = {
#include "stg28/terrain/bar_land_motion_tracks.inc"
};
static NJS_MOTION bar_land_motion = {
#include "stg28/terrain/bar_land_motion.inc"
};
struct motion_entry module_motions[9] = {
#include "stg28/terrain/module_motions.inc"
};
static struct land_entry entries[462] = {
#include "stg28/terrain/entries.inc"
};
struct land_table pyramid_cave_land = {
#include "stg28/terrain/pyramid_cave_land.inc"
};
static NJS_TEXNAME textures[33] = {
#include "stg28/terrain/textures.inc"
};
NJS_TEXLIST pyramid_cave_land_texlist = {
#include "stg28/terrain/pyramid_cave_land_texlist.inc"
};
static NJS_MATERIAL collision_0_materials[1] = {
#include "stg28/terrain/collision_0_materials.inc"
};
static Sint16 collision_0_faces[198] = {
#include "stg28/terrain/collision_0_faces.inc"
};
static NJS_MESHSET collision_0_meshsets[1] = {
#include "stg28/terrain/collision_0_meshsets.inc"
};
static NJS_POINT3 collision_0_points[184] = {
#include "stg28/terrain/collision_0_points.inc"
};
static NJS_MODEL collision_0_model = {
#include "stg28/terrain/collision_0_model.inc"
};
static NJS_OBJECT collision_0 = {
#include "stg28/terrain/collision_0.inc"
};
static NJS_MATERIAL collision_1_materials[1] = {
#include "stg28/terrain/collision_1_materials.inc"
};
static Sint16 collision_1_faces[18] = {
#include "stg28/terrain/collision_1_faces.inc"
};
static NJS_MESHSET collision_1_meshsets[1] = {
#include "stg28/terrain/collision_1_meshsets.inc"
};
static NJS_POINT3 collision_1_points[16] = {
#include "stg28/terrain/collision_1_points.inc"
};
static NJS_MODEL collision_1_model = {
#include "stg28/terrain/collision_1_model.inc"
};
static NJS_OBJECT collision_1 = {
#include "stg28/terrain/collision_1.inc"
};
static NJS_MATERIAL collision_2_materials[1] = {
#include "stg28/terrain/collision_2_materials.inc"
};
static Sint16 collision_2_faces[14] = {
#include "stg28/terrain/collision_2_faces.inc"
};
static NJS_MESHSET collision_2_meshsets[1] = {
#include "stg28/terrain/collision_2_meshsets.inc"
};
static NJS_POINT3 collision_2_points[12] = {
#include "stg28/terrain/collision_2_points.inc"
};
static NJS_MODEL collision_2_model = {
#include "stg28/terrain/collision_2_model.inc"
};
static NJS_OBJECT collision_2 = {
#include "stg28/terrain/collision_2.inc"
};
static NJS_MATERIAL collision_3_materials[1] = {
#include "stg28/terrain/collision_3_materials.inc"
};
static Sint16 collision_3_faces[156] = {
#include "stg28/terrain/collision_3_faces.inc"
};
static NJS_MESHSET collision_3_meshsets[1] = {
#include "stg28/terrain/collision_3_meshsets.inc"
};
static NJS_POINT3 collision_3_points[114] = {
#include "stg28/terrain/collision_3_points.inc"
};
static NJS_MODEL collision_3_model = {
#include "stg28/terrain/collision_3_model.inc"
};
static NJS_OBJECT collision_3 = {
#include "stg28/terrain/collision_3.inc"
};
static NJS_MATERIAL collision_4_materials[1] = {
#include "stg28/terrain/collision_4_materials.inc"
};
static Sint16 collision_4_faces[198] = {
#include "stg28/terrain/collision_4_faces.inc"
};
static NJS_MESHSET collision_4_meshsets[1] = {
#include "stg28/terrain/collision_4_meshsets.inc"
};
static NJS_POINT3 collision_4_points[184] = {
#include "stg28/terrain/collision_4_points.inc"
};
static NJS_MODEL collision_4_model = {
#include "stg28/terrain/collision_4_model.inc"
};
static NJS_OBJECT collision_4 = {
#include "stg28/terrain/collision_4.inc"
};
static NJS_MATERIAL collision_5_materials[1] = {
#include "stg28/terrain/collision_5_materials.inc"
};
static Sint16 collision_5_faces[18] = {
#include "stg28/terrain/collision_5_faces.inc"
};
static NJS_MESHSET collision_5_meshsets[1] = {
#include "stg28/terrain/collision_5_meshsets.inc"
};
static NJS_POINT3 collision_5_points[16] = {
#include "stg28/terrain/collision_5_points.inc"
};
static NJS_MODEL collision_5_model = {
#include "stg28/terrain/collision_5_model.inc"
};
static NJS_OBJECT collision_5 = {
#include "stg28/terrain/collision_5.inc"
};
static NJS_MATERIAL collision_6_materials[1] = {
#include "stg28/terrain/collision_6_materials.inc"
};
static Sint16 collision_6_faces[58] = {
#include "stg28/terrain/collision_6_faces.inc"
};
static NJS_MESHSET collision_6_meshsets[1] = {
#include "stg28/terrain/collision_6_meshsets.inc"
};
static NJS_POINT3 collision_6_points[54] = {
#include "stg28/terrain/collision_6_points.inc"
};
static NJS_MODEL collision_6_model = {
#include "stg28/terrain/collision_6_model.inc"
};
static NJS_OBJECT collision_6 = {
#include "stg28/terrain/collision_6.inc"
};
static NJS_MATERIAL collision_8_materials[1] = {
#include "stg28/terrain/collision_8_materials.inc"
};
static Sint16 collision_8_faces[73] = {
#include "stg28/terrain/collision_8_faces.inc"
};
static NJS_MESHSET collision_8_meshsets[1] = {
#include "stg28/terrain/collision_8_meshsets.inc"
};
static NJS_POINT3 collision_8_points[72] = {
#include "stg28/terrain/collision_8_points.inc"
};
static NJS_MODEL collision_8_model = {
#include "stg28/terrain/collision_8_model.inc"
};
static NJS_OBJECT collision_8 = {
#include "stg28/terrain/collision_8.inc"
};
static NJS_MATERIAL collision_10_materials[1] = {
#include "stg28/terrain/collision_10_materials.inc"
};
static Sint16 collision_10_faces[79] = {
#include "stg28/terrain/collision_10_faces.inc"
};
static NJS_MESHSET collision_10_meshsets[1] = {
#include "stg28/terrain/collision_10_meshsets.inc"
};
static NJS_POINT3 collision_10_points[78] = {
#include "stg28/terrain/collision_10_points.inc"
};
static NJS_MODEL collision_10_model = {
#include "stg28/terrain/collision_10_model.inc"
};
static NJS_OBJECT collision_10 = {
#include "stg28/terrain/collision_10.inc"
};
static NJS_MATERIAL collision_12_materials[1] = {
#include "stg28/terrain/collision_12_materials.inc"
};
static Sint16 collision_12_faces[14] = {
#include "stg28/terrain/collision_12_faces.inc"
};
static NJS_MESHSET collision_12_meshsets[1] = {
#include "stg28/terrain/collision_12_meshsets.inc"
};
static NJS_POINT3 collision_12_points[12] = {
#include "stg28/terrain/collision_12_points.inc"
};
static NJS_MODEL collision_12_model = {
#include "stg28/terrain/collision_12_model.inc"
};
static NJS_OBJECT collision_12 = {
#include "stg28/terrain/collision_12.inc"
};
static NJS_MATERIAL collision_18_materials[1] = {
#include "stg28/terrain/collision_18_materials.inc"
};
static Sint16 collision_18_faces[14] = {
#include "stg28/terrain/collision_18_faces.inc"
};
static NJS_MESHSET collision_18_meshsets[1] = {
#include "stg28/terrain/collision_18_meshsets.inc"
};
static NJS_POINT3 collision_18_points[12] = {
#include "stg28/terrain/collision_18_points.inc"
};
static NJS_MODEL collision_18_model = {
#include "stg28/terrain/collision_18_model.inc"
};
static NJS_OBJECT collision_18 = {
#include "stg28/terrain/collision_18.inc"
};
static NJS_MATERIAL collision_22_materials[1] = {
#include "stg28/terrain/collision_22_materials.inc"
};
static Sint16 collision_22_faces[14] = {
#include "stg28/terrain/collision_22_faces.inc"
};
static NJS_MESHSET collision_22_meshsets[1] = {
#include "stg28/terrain/collision_22_meshsets.inc"
};
static NJS_POINT3 collision_22_points[12] = {
#include "stg28/terrain/collision_22_points.inc"
};
static NJS_MODEL collision_22_model = {
#include "stg28/terrain/collision_22_model.inc"
};
static NJS_OBJECT collision_22 = {
#include "stg28/terrain/collision_22.inc"
};
static NJS_MATERIAL collision_26_materials[1] = {
#include "stg28/terrain/collision_26_materials.inc"
};
static Sint16 collision_26_faces[14] = {
#include "stg28/terrain/collision_26_faces.inc"
};
static NJS_MESHSET collision_26_meshsets[1] = {
#include "stg28/terrain/collision_26_meshsets.inc"
};
static NJS_POINT3 collision_26_points[12] = {
#include "stg28/terrain/collision_26_points.inc"
};
static NJS_MODEL collision_26_model = {
#include "stg28/terrain/collision_26_model.inc"
};
static NJS_OBJECT collision_26 = {
#include "stg28/terrain/collision_26.inc"
};
static NJS_MATERIAL collision_29_materials[1] = {
#include "stg28/terrain/collision_29_materials.inc"
};
static Sint16 collision_29_faces[14] = {
#include "stg28/terrain/collision_29_faces.inc"
};
static NJS_MESHSET collision_29_meshsets[1] = {
#include "stg28/terrain/collision_29_meshsets.inc"
};
static NJS_POINT3 collision_29_points[12] = {
#include "stg28/terrain/collision_29_points.inc"
};
static NJS_MODEL collision_29_model = {
#include "stg28/terrain/collision_29_model.inc"
};
static NJS_OBJECT collision_29 = {
#include "stg28/terrain/collision_29.inc"
};
static NJS_MATERIAL collision_36_materials[1] = {
#include "stg28/terrain/collision_36_materials.inc"
};
static Sint16 collision_36_faces[10] = {
#include "stg28/terrain/collision_36_faces.inc"
};
static NJS_MESHSET collision_36_meshsets[1] = {
#include "stg28/terrain/collision_36_meshsets.inc"
};
static NJS_POINT3 collision_36_points[8] = {
#include "stg28/terrain/collision_36_points.inc"
};
static NJS_MODEL collision_36_model = {
#include "stg28/terrain/collision_36_model.inc"
};
static NJS_OBJECT collision_36 = {
#include "stg28/terrain/collision_36.inc"
};
static NJS_MATERIAL collision_41_materials[1] = {
#include "stg28/terrain/collision_41_materials.inc"
};
static Sint16 collision_41_faces[10] = {
#include "stg28/terrain/collision_41_faces.inc"
};
static NJS_MESHSET collision_41_meshsets[1] = {
#include "stg28/terrain/collision_41_meshsets.inc"
};
static NJS_POINT3 collision_41_points[8] = {
#include "stg28/terrain/collision_41_points.inc"
};
static NJS_MODEL collision_41_model = {
#include "stg28/terrain/collision_41_model.inc"
};
static NJS_OBJECT collision_41 = {
#include "stg28/terrain/collision_41.inc"
};
static NJS_MATERIAL collision_49_materials[1] = {
#include "stg28/terrain/collision_49_materials.inc"
};
static Sint16 collision_49_faces[10] = {
#include "stg28/terrain/collision_49_faces.inc"
};
static NJS_MESHSET collision_49_meshsets[1] = {
#include "stg28/terrain/collision_49_meshsets.inc"
};
static NJS_POINT3 collision_49_points[8] = {
#include "stg28/terrain/collision_49_points.inc"
};
static NJS_MODEL collision_49_model = {
#include "stg28/terrain/collision_49_model.inc"
};
static NJS_OBJECT collision_49 = {
#include "stg28/terrain/collision_49.inc"
};
static NJS_MATERIAL collision_50_materials[1] = {
#include "stg28/terrain/collision_50_materials.inc"
};
static Sint16 collision_50_faces[10] = {
#include "stg28/terrain/collision_50_faces.inc"
};
static NJS_MESHSET collision_50_meshsets[1] = {
#include "stg28/terrain/collision_50_meshsets.inc"
};
static NJS_POINT3 collision_50_points[8] = {
#include "stg28/terrain/collision_50_points.inc"
};
static NJS_MODEL collision_50_model = {
#include "stg28/terrain/collision_50_model.inc"
};
static NJS_OBJECT collision_50 = {
#include "stg28/terrain/collision_50.inc"
};
static NJS_MATERIAL collision_51_materials[1] = {
#include "stg28/terrain/collision_51_materials.inc"
};
static Sint16 collision_51_faces[14] = {
#include "stg28/terrain/collision_51_faces.inc"
};
static NJS_MESHSET collision_51_meshsets[1] = {
#include "stg28/terrain/collision_51_meshsets.inc"
};
static NJS_POINT3 collision_51_points[12] = {
#include "stg28/terrain/collision_51_points.inc"
};
static NJS_MODEL collision_51_model = {
#include "stg28/terrain/collision_51_model.inc"
};
static NJS_OBJECT collision_51 = {
#include "stg28/terrain/collision_51.inc"
};
static NJS_MATERIAL collision_52_materials[1] = {
#include "stg28/terrain/collision_52_materials.inc"
};
static Sint16 collision_52_faces[202] = {
#include "stg28/terrain/collision_52_faces.inc"
};
static NJS_MESHSET collision_52_meshsets[1] = {
#include "stg28/terrain/collision_52_meshsets.inc"
};
static NJS_POINT3 collision_52_points[188] = {
#include "stg28/terrain/collision_52_points.inc"
};
static NJS_MODEL collision_52_model = {
#include "stg28/terrain/collision_52_model.inc"
};
static NJS_OBJECT collision_52 = {
#include "stg28/terrain/collision_52.inc"
};
static NJS_MATERIAL collision_53_materials[1] = {
#include "stg28/terrain/collision_53_materials.inc"
};
static Sint16 collision_53_faces[36] = {
#include "stg28/terrain/collision_53_faces.inc"
};
static NJS_MESHSET collision_53_meshsets[1] = {
#include "stg28/terrain/collision_53_meshsets.inc"
};
static NJS_POINT3 collision_53_points[24] = {
#include "stg28/terrain/collision_53_points.inc"
};
static NJS_MODEL collision_53_model = {
#include "stg28/terrain/collision_53_model.inc"
};
static NJS_OBJECT collision_53 = {
#include "stg28/terrain/collision_53.inc"
};
static NJS_MATERIAL collision_54_materials[1] = {
#include "stg28/terrain/collision_54_materials.inc"
};
static Sint16 collision_54_faces[86] = {
#include "stg28/terrain/collision_54_faces.inc"
};
static NJS_MESHSET collision_54_meshsets[1] = {
#include "stg28/terrain/collision_54_meshsets.inc"
};
static NJS_POINT3 collision_54_points[40] = {
#include "stg28/terrain/collision_54_points.inc"
};
static NJS_MODEL collision_54_model = {
#include "stg28/terrain/collision_54_model.inc"
};
static NJS_OBJECT collision_54 = {
#include "stg28/terrain/collision_54.inc"
};
static NJS_MATERIAL collision_55_materials[1] = {
#include "stg28/terrain/collision_55_materials.inc"
};
static Sint16 collision_55_faces[238] = {
#include "stg28/terrain/collision_55_faces.inc"
};
static NJS_MESHSET collision_55_meshsets[1] = {
#include "stg28/terrain/collision_55_meshsets.inc"
};
static NJS_POINT3 collision_55_points[96] = {
#include "stg28/terrain/collision_55_points.inc"
};
static NJS_MODEL collision_55_model = {
#include "stg28/terrain/collision_55_model.inc"
};
static NJS_OBJECT collision_55 = {
#include "stg28/terrain/collision_55.inc"
};
static NJS_MATERIAL collision_56_materials[1] = {
#include "stg28/terrain/collision_56_materials.inc"
};
static Sint16 collision_56_faces[33] = {
#include "stg28/terrain/collision_56_faces.inc"
};
static NJS_MESHSET collision_56_meshsets[1] = {
#include "stg28/terrain/collision_56_meshsets.inc"
};
static NJS_POINT3 collision_56_points[18] = {
#include "stg28/terrain/collision_56_points.inc"
};
static NJS_MODEL collision_56_model = {
#include "stg28/terrain/collision_56_model.inc"
};
static NJS_OBJECT collision_56 = {
#include "stg28/terrain/collision_56.inc"
};
static NJS_MATERIAL collision_57_materials[1] = {
#include "stg28/terrain/collision_57_materials.inc"
};
static Sint16 collision_57_faces[329] = {
#include "stg28/terrain/collision_57_faces.inc"
};
static NJS_MESHSET collision_57_meshsets[1] = {
#include "stg28/terrain/collision_57_meshsets.inc"
};
static NJS_POINT3 collision_57_points[118] = {
#include "stg28/terrain/collision_57_points.inc"
};
static NJS_MODEL collision_57_model = {
#include "stg28/terrain/collision_57_model.inc"
};
static NJS_OBJECT collision_57 = {
#include "stg28/terrain/collision_57.inc"
};
static NJS_MATERIAL collision_58_materials[1] = {
#include "stg28/terrain/collision_58_materials.inc"
};
static Sint16 collision_58_faces[90] = {
#include "stg28/terrain/collision_58_faces.inc"
};
static NJS_MESHSET collision_58_meshsets[1] = {
#include "stg28/terrain/collision_58_meshsets.inc"
};
static NJS_POINT3 collision_58_points[32] = {
#include "stg28/terrain/collision_58_points.inc"
};
static NJS_MODEL collision_58_model = {
#include "stg28/terrain/collision_58_model.inc"
};
static NJS_OBJECT collision_58 = {
#include "stg28/terrain/collision_58.inc"
};
static NJS_MATERIAL collision_59_materials[1] = {
#include "stg28/terrain/collision_59_materials.inc"
};
static Sint16 collision_59_faces[201] = {
#include "stg28/terrain/collision_59_faces.inc"
};
static NJS_MESHSET collision_59_meshsets[1] = {
#include "stg28/terrain/collision_59_meshsets.inc"
};
static NJS_POINT3 collision_59_points[77] = {
#include "stg28/terrain/collision_59_points.inc"
};
static NJS_MODEL collision_59_model = {
#include "stg28/terrain/collision_59_model.inc"
};
static NJS_OBJECT collision_59 = {
#include "stg28/terrain/collision_59.inc"
};
static NJS_MATERIAL collision_60_materials[1] = {
#include "stg28/terrain/collision_60_materials.inc"
};
static Sint16 collision_60_faces[582] = {
#include "stg28/terrain/collision_60_faces.inc"
};
static NJS_MESHSET collision_60_meshsets[1] = {
#include "stg28/terrain/collision_60_meshsets.inc"
};
static NJS_POINT3 collision_60_points[368] = {
#include "stg28/terrain/collision_60_points.inc"
};
static NJS_MODEL collision_60_model = {
#include "stg28/terrain/collision_60_model.inc"
};
static NJS_OBJECT collision_60 = {
#include "stg28/terrain/collision_60.inc"
};
static NJS_MATERIAL collision_61_materials[1] = {
#include "stg28/terrain/collision_61_materials.inc"
};
static Sint16 collision_61_faces[62] = {
#include "stg28/terrain/collision_61_faces.inc"
};
static NJS_MESHSET collision_61_meshsets[1] = {
#include "stg28/terrain/collision_61_meshsets.inc"
};
static NJS_POINT3 collision_61_points[26] = {
#include "stg28/terrain/collision_61_points.inc"
};
static NJS_MODEL collision_61_model = {
#include "stg28/terrain/collision_61_model.inc"
};
static NJS_OBJECT collision_61 = {
#include "stg28/terrain/collision_61.inc"
};
static NJS_MATERIAL collision_69_materials[1] = {
#include "stg28/terrain/collision_69_materials.inc"
};
static Sint16 collision_69_faces[60] = {
#include "stg28/terrain/collision_69_faces.inc"
};
static NJS_MESHSET collision_69_meshsets[1] = {
#include "stg28/terrain/collision_69_meshsets.inc"
};
static NJS_POINT3 collision_69_points[32] = {
#include "stg28/terrain/collision_69_points.inc"
};
static NJS_MODEL collision_69_model = {
#include "stg28/terrain/collision_69_model.inc"
};
static NJS_OBJECT collision_69 = {
#include "stg28/terrain/collision_69.inc"
};
static NJS_MATERIAL collision_70_materials[1] = {
#include "stg28/terrain/collision_70_materials.inc"
};
static Sint16 collision_70_faces[36] = {
#include "stg28/terrain/collision_70_faces.inc"
};
static NJS_MESHSET collision_70_meshsets[1] = {
#include "stg28/terrain/collision_70_meshsets.inc"
};
static NJS_POINT3 collision_70_points[18] = {
#include "stg28/terrain/collision_70_points.inc"
};
static NJS_MODEL collision_70_model = {
#include "stg28/terrain/collision_70_model.inc"
};
static NJS_OBJECT collision_70 = {
#include "stg28/terrain/collision_70.inc"
};
static NJS_MATERIAL collision_71_materials[1] = {
#include "stg28/terrain/collision_71_materials.inc"
};
static Sint16 collision_71_faces[46] = {
#include "stg28/terrain/collision_71_faces.inc"
};
static NJS_MESHSET collision_71_meshsets[1] = {
#include "stg28/terrain/collision_71_meshsets.inc"
};
static NJS_POINT3 collision_71_points[22] = {
#include "stg28/terrain/collision_71_points.inc"
};
static NJS_MODEL collision_71_model = {
#include "stg28/terrain/collision_71_model.inc"
};
static NJS_OBJECT collision_71 = {
#include "stg28/terrain/collision_71.inc"
};
static NJS_MATERIAL collision_72_materials[1] = {
#include "stg28/terrain/collision_72_materials.inc"
};
static Sint16 collision_72_faces[31] = {
#include "stg28/terrain/collision_72_faces.inc"
};
static NJS_MESHSET collision_72_meshsets[1] = {
#include "stg28/terrain/collision_72_meshsets.inc"
};
static NJS_POINT3 collision_72_points[14] = {
#include "stg28/terrain/collision_72_points.inc"
};
static NJS_MODEL collision_72_model = {
#include "stg28/terrain/collision_72_model.inc"
};
static NJS_OBJECT collision_72 = {
#include "stg28/terrain/collision_72.inc"
};
static NJS_MATERIAL collision_73_materials[1] = {
#include "stg28/terrain/collision_73_materials.inc"
};
static Sint16 collision_73_faces[156] = {
#include "stg28/terrain/collision_73_faces.inc"
};
static NJS_MESHSET collision_73_meshsets[1] = {
#include "stg28/terrain/collision_73_meshsets.inc"
};
static NJS_POINT3 collision_73_points[63] = {
#include "stg28/terrain/collision_73_points.inc"
};
static NJS_MODEL collision_73_model = {
#include "stg28/terrain/collision_73_model.inc"
};
static NJS_OBJECT collision_73 = {
#include "stg28/terrain/collision_73.inc"
};
static NJS_MATERIAL collision_74_materials[1] = {
#include "stg28/terrain/collision_74_materials.inc"
};
static Sint16 collision_74_faces[195] = {
#include "stg28/terrain/collision_74_faces.inc"
};
static NJS_MESHSET collision_74_meshsets[1] = {
#include "stg28/terrain/collision_74_meshsets.inc"
};
static NJS_POINT3 collision_74_points[80] = {
#include "stg28/terrain/collision_74_points.inc"
};
static NJS_MODEL collision_74_model = {
#include "stg28/terrain/collision_74_model.inc"
};
static NJS_OBJECT collision_74 = {
#include "stg28/terrain/collision_74.inc"
};
static NJS_MATERIAL collision_75_materials[1] = {
#include "stg28/terrain/collision_75_materials.inc"
};
static Sint16 collision_75_faces[75] = {
#include "stg28/terrain/collision_75_faces.inc"
};
static NJS_MESHSET collision_75_meshsets[1] = {
#include "stg28/terrain/collision_75_meshsets.inc"
};
static NJS_POINT3 collision_75_points[33] = {
#include "stg28/terrain/collision_75_points.inc"
};
static NJS_MODEL collision_75_model = {
#include "stg28/terrain/collision_75_model.inc"
};
static NJS_OBJECT collision_75 = {
#include "stg28/terrain/collision_75.inc"
};
static NJS_MATERIAL collision_76_materials[1] = {
#include "stg28/terrain/collision_76_materials.inc"
};
static Sint16 collision_76_faces[99] = {
#include "stg28/terrain/collision_76_faces.inc"
};
static NJS_MESHSET collision_76_meshsets[1] = {
#include "stg28/terrain/collision_76_meshsets.inc"
};
static NJS_POINT3 collision_76_points[79] = {
#include "stg28/terrain/collision_76_points.inc"
};
static NJS_MODEL collision_76_model = {
#include "stg28/terrain/collision_76_model.inc"
};
static NJS_OBJECT collision_76 = {
#include "stg28/terrain/collision_76.inc"
};
static NJS_MATERIAL collision_77_materials[1] = {
#include "stg28/terrain/collision_77_materials.inc"
};
static Sint16 collision_77_faces[86] = {
#include "stg28/terrain/collision_77_faces.inc"
};
static NJS_MESHSET collision_77_meshsets[1] = {
#include "stg28/terrain/collision_77_meshsets.inc"
};
static NJS_POINT3 collision_77_points[58] = {
#include "stg28/terrain/collision_77_points.inc"
};
static NJS_MODEL collision_77_model = {
#include "stg28/terrain/collision_77_model.inc"
};
static NJS_OBJECT collision_77 = {
#include "stg28/terrain/collision_77.inc"
};
static NJS_MATERIAL collision_78_materials[1] = {
#include "stg28/terrain/collision_78_materials.inc"
};
static Sint16 collision_78_faces[22] = {
#include "stg28/terrain/collision_78_faces.inc"
};
static NJS_MESHSET collision_78_meshsets[1] = {
#include "stg28/terrain/collision_78_meshsets.inc"
};
static NJS_POINT3 collision_78_points[15] = {
#include "stg28/terrain/collision_78_points.inc"
};
static NJS_MODEL collision_78_model = {
#include "stg28/terrain/collision_78_model.inc"
};
static NJS_OBJECT collision_78 = {
#include "stg28/terrain/collision_78.inc"
};
static NJS_MATERIAL collision_79_materials[1] = {
#include "stg28/terrain/collision_79_materials.inc"
};
static Sint16 collision_79_faces[213] = {
#include "stg28/terrain/collision_79_faces.inc"
};
static NJS_MESHSET collision_79_meshsets[1] = {
#include "stg28/terrain/collision_79_meshsets.inc"
};
static NJS_POINT3 collision_79_points[82] = {
#include "stg28/terrain/collision_79_points.inc"
};
static NJS_MODEL collision_79_model = {
#include "stg28/terrain/collision_79_model.inc"
};
static NJS_OBJECT collision_79 = {
#include "stg28/terrain/collision_79.inc"
};
static NJS_MATERIAL collision_80_materials[1] = {
#include "stg28/terrain/collision_80_materials.inc"
};
static Sint16 collision_80_faces[175] = {
#include "stg28/terrain/collision_80_faces.inc"
};
static NJS_MESHSET collision_80_meshsets[1] = {
#include "stg28/terrain/collision_80_meshsets.inc"
};
static NJS_POINT3 collision_80_points[71] = {
#include "stg28/terrain/collision_80_points.inc"
};
static NJS_MODEL collision_80_model = {
#include "stg28/terrain/collision_80_model.inc"
};
static NJS_OBJECT collision_80 = {
#include "stg28/terrain/collision_80.inc"
};
static NJS_MATERIAL collision_82_materials[1] = {
#include "stg28/terrain/collision_82_materials.inc"
};
static Sint16 collision_82_faces[242] = {
#include "stg28/terrain/collision_82_faces.inc"
};
static NJS_MESHSET collision_82_meshsets[1] = {
#include "stg28/terrain/collision_82_meshsets.inc"
};
static NJS_POINT3 collision_82_points[89] = {
#include "stg28/terrain/collision_82_points.inc"
};
static NJS_MODEL collision_82_model = {
#include "stg28/terrain/collision_82_model.inc"
};
static NJS_OBJECT collision_82 = {
#include "stg28/terrain/collision_82.inc"
};
static NJS_MATERIAL collision_83_materials[1] = {
#include "stg28/terrain/collision_83_materials.inc"
};
static Sint16 collision_83_faces[585] = {
#include "stg28/terrain/collision_83_faces.inc"
};
static NJS_MESHSET collision_83_meshsets[1] = {
#include "stg28/terrain/collision_83_meshsets.inc"
};
static NJS_POINT3 collision_83_points[368] = {
#include "stg28/terrain/collision_83_points.inc"
};
static NJS_MODEL collision_83_model = {
#include "stg28/terrain/collision_83_model.inc"
};
static NJS_OBJECT collision_83 = {
#include "stg28/terrain/collision_83.inc"
};
static NJS_MATERIAL collision_84_materials[1] = {
#include "stg28/terrain/collision_84_materials.inc"
};
static Sint16 collision_84_faces[726] = {
#include "stg28/terrain/collision_84_faces.inc"
};
static NJS_MESHSET collision_84_meshsets[1] = {
#include "stg28/terrain/collision_84_meshsets.inc"
};
static NJS_POINT3 collision_84_points[424] = {
#include "stg28/terrain/collision_84_points.inc"
};
static NJS_MODEL collision_84_model = {
#include "stg28/terrain/collision_84_model.inc"
};
static NJS_OBJECT collision_84 = {
#include "stg28/terrain/collision_84.inc"
};
static NJS_MATERIAL collision_86_materials[1] = {
#include "stg28/terrain/collision_86_materials.inc"
};
static Sint16 collision_86_faces[226] = {
#include "stg28/terrain/collision_86_faces.inc"
};
static NJS_MESHSET collision_86_meshsets[1] = {
#include "stg28/terrain/collision_86_meshsets.inc"
};
static NJS_POINT3 collision_86_points[212] = {
#include "stg28/terrain/collision_86_points.inc"
};
static NJS_MODEL collision_86_model = {
#include "stg28/terrain/collision_86_model.inc"
};
static NJS_OBJECT collision_86 = {
#include "stg28/terrain/collision_86.inc"
};
static NJS_MATERIAL collision_88_materials[1] = {
#include "stg28/terrain/collision_88_materials.inc"
};
static Sint16 collision_88_faces[11] = {
#include "stg28/terrain/collision_88_faces.inc"
};
static NJS_MESHSET collision_88_meshsets[1] = {
#include "stg28/terrain/collision_88_meshsets.inc"
};
static NJS_POINT3 collision_88_points[8] = {
#include "stg28/terrain/collision_88_points.inc"
};
static NJS_MODEL collision_88_model = {
#include "stg28/terrain/collision_88_model.inc"
};
static NJS_OBJECT collision_88 = {
#include "stg28/terrain/collision_88.inc"
};
static NJS_MATERIAL collision_89_materials[1] = {
#include "stg28/terrain/collision_89_materials.inc"
};
static Sint16 collision_89_faces[45] = {
#include "stg28/terrain/collision_89_faces.inc"
};
static NJS_MESHSET collision_89_meshsets[1] = {
#include "stg28/terrain/collision_89_meshsets.inc"
};
static NJS_POINT3 collision_89_points[32] = {
#include "stg28/terrain/collision_89_points.inc"
};
static NJS_MODEL collision_89_model = {
#include "stg28/terrain/collision_89_model.inc"
};
static NJS_OBJECT collision_89 = {
#include "stg28/terrain/collision_89.inc"
};
static NJS_MATERIAL collision_90_materials[1] = {
#include "stg28/terrain/collision_90_materials.inc"
};
static Sint16 collision_90_faces[87] = {
#include "stg28/terrain/collision_90_faces.inc"
};
static NJS_MESHSET collision_90_meshsets[1] = {
#include "stg28/terrain/collision_90_meshsets.inc"
};
static NJS_POINT3 collision_90_points[32] = {
#include "stg28/terrain/collision_90_points.inc"
};
static NJS_MODEL collision_90_model = {
#include "stg28/terrain/collision_90_model.inc"
};
static NJS_OBJECT collision_90 = {
#include "stg28/terrain/collision_90.inc"
};
static NJS_MATERIAL collision_92_materials[1] = {
#include "stg28/terrain/collision_92_materials.inc"
};
static Sint16 collision_92_faces[84] = {
#include "stg28/terrain/collision_92_faces.inc"
};
static NJS_MESHSET collision_92_meshsets[1] = {
#include "stg28/terrain/collision_92_meshsets.inc"
};
static NJS_POINT3 collision_92_points[32] = {
#include "stg28/terrain/collision_92_points.inc"
};
static NJS_MODEL collision_92_model = {
#include "stg28/terrain/collision_92_model.inc"
};
static NJS_OBJECT collision_92 = {
#include "stg28/terrain/collision_92.inc"
};
static NJS_MATERIAL collision_93_materials[1] = {
#include "stg28/terrain/collision_93_materials.inc"
};
static Sint16 collision_93_faces[67] = {
#include "stg28/terrain/collision_93_faces.inc"
};
static NJS_MESHSET collision_93_meshsets[1] = {
#include "stg28/terrain/collision_93_meshsets.inc"
};
static NJS_POINT3 collision_93_points[25] = {
#include "stg28/terrain/collision_93_points.inc"
};
static NJS_MODEL collision_93_model = {
#include "stg28/terrain/collision_93_model.inc"
};
static NJS_OBJECT collision_93 = {
#include "stg28/terrain/collision_93.inc"
};
static NJS_MATERIAL collision_97_materials[1] = {
#include "stg28/terrain/collision_97_materials.inc"
};
static Sint16 collision_97_faces[11] = {
#include "stg28/terrain/collision_97_faces.inc"
};
static NJS_MESHSET collision_97_meshsets[1] = {
#include "stg28/terrain/collision_97_meshsets.inc"
};
static NJS_POINT3 collision_97_points[8] = {
#include "stg28/terrain/collision_97_points.inc"
};
static NJS_MODEL collision_97_model = {
#include "stg28/terrain/collision_97_model.inc"
};
static NJS_OBJECT collision_97 = {
#include "stg28/terrain/collision_97.inc"
};
static NJS_MATERIAL collision_98_materials[1] = {
#include "stg28/terrain/collision_98_materials.inc"
};
static Sint16 collision_98_faces[40] = {
#include "stg28/terrain/collision_98_faces.inc"
};
static NJS_MESHSET collision_98_meshsets[1] = {
#include "stg28/terrain/collision_98_meshsets.inc"
};
static NJS_POINT3 collision_98_points[16] = {
#include "stg28/terrain/collision_98_points.inc"
};
static NJS_MODEL collision_98_model = {
#include "stg28/terrain/collision_98_model.inc"
};
static NJS_OBJECT collision_98 = {
#include "stg28/terrain/collision_98.inc"
};
static NJS_MATERIAL collision_99_materials[1] = {
#include "stg28/terrain/collision_99_materials.inc"
};
static Sint16 collision_99_faces[50] = {
#include "stg28/terrain/collision_99_faces.inc"
};
static NJS_MESHSET collision_99_meshsets[1] = {
#include "stg28/terrain/collision_99_meshsets.inc"
};
static NJS_POINT3 collision_99_points[20] = {
#include "stg28/terrain/collision_99_points.inc"
};
static NJS_MODEL collision_99_model = {
#include "stg28/terrain/collision_99_model.inc"
};
static NJS_OBJECT collision_99 = {
#include "stg28/terrain/collision_99.inc"
};
static NJS_MATERIAL collision_100_materials[1] = {
#include "stg28/terrain/collision_100_materials.inc"
};
static Sint16 collision_100_faces[33] = {
#include "stg28/terrain/collision_100_faces.inc"
};
static NJS_MESHSET collision_100_meshsets[1] = {
#include "stg28/terrain/collision_100_meshsets.inc"
};
static NJS_POINT3 collision_100_points[12] = {
#include "stg28/terrain/collision_100_points.inc"
};
static NJS_MODEL collision_100_model = {
#include "stg28/terrain/collision_100_model.inc"
};
static NJS_OBJECT collision_100 = {
#include "stg28/terrain/collision_100.inc"
};
static NJS_MATERIAL collision_121_materials[1] = {
#include "stg28/terrain/collision_121_materials.inc"
};
static Sint16 collision_121_faces[73] = {
#include "stg28/terrain/collision_121_faces.inc"
};
static NJS_MESHSET collision_121_meshsets[1] = {
#include "stg28/terrain/collision_121_meshsets.inc"
};
static NJS_POINT3 collision_121_points[26] = {
#include "stg28/terrain/collision_121_points.inc"
};
static NJS_MODEL collision_121_model = {
#include "stg28/terrain/collision_121_model.inc"
};
static NJS_OBJECT collision_121 = {
#include "stg28/terrain/collision_121.inc"
};
static NJS_MATERIAL collision_131_materials[1] = {
#include "stg28/terrain/collision_131_materials.inc"
};
static Sint16 collision_131_faces[226] = {
#include "stg28/terrain/collision_131_faces.inc"
};
static NJS_MESHSET collision_131_meshsets[1] = {
#include "stg28/terrain/collision_131_meshsets.inc"
};
static NJS_POINT3 collision_131_points[93] = {
#include "stg28/terrain/collision_131_points.inc"
};
static NJS_MODEL collision_131_model = {
#include "stg28/terrain/collision_131_model.inc"
};
static NJS_OBJECT collision_131 = {
#include "stg28/terrain/collision_131.inc"
};
static NJS_MATERIAL collision_133_materials[1] = {
#include "stg28/terrain/collision_133_materials.inc"
};
static Sint16 collision_133_faces[330] = {
#include "stg28/terrain/collision_133_faces.inc"
};
static NJS_MESHSET collision_133_meshsets[1] = {
#include "stg28/terrain/collision_133_meshsets.inc"
};
static NJS_POINT3 collision_133_points[114] = {
#include "stg28/terrain/collision_133_points.inc"
};
static NJS_MODEL collision_133_model = {
#include "stg28/terrain/collision_133_model.inc"
};
static NJS_OBJECT collision_133 = {
#include "stg28/terrain/collision_133.inc"
};
static NJS_MATERIAL collision_135_materials[1] = {
#include "stg28/terrain/collision_135_materials.inc"
};
static Sint16 collision_135_faces[195] = {
#include "stg28/terrain/collision_135_faces.inc"
};
static NJS_MESHSET collision_135_meshsets[1] = {
#include "stg28/terrain/collision_135_meshsets.inc"
};
static NJS_POINT3 collision_135_points[76] = {
#include "stg28/terrain/collision_135_points.inc"
};
static NJS_MODEL collision_135_model = {
#include "stg28/terrain/collision_135_model.inc"
};
static NJS_OBJECT collision_135 = {
#include "stg28/terrain/collision_135.inc"
};
static NJS_MATERIAL collision_136_materials[1] = {
#include "stg28/terrain/collision_136_materials.inc"
};
static Sint16 collision_136_faces[189] = {
#include "stg28/terrain/collision_136_faces.inc"
};
static NJS_MESHSET collision_136_meshsets[1] = {
#include "stg28/terrain/collision_136_meshsets.inc"
};
static NJS_POINT3 collision_136_points[67] = {
#include "stg28/terrain/collision_136_points.inc"
};
static NJS_MODEL collision_136_model = {
#include "stg28/terrain/collision_136_model.inc"
};
static NJS_OBJECT collision_136 = {
#include "stg28/terrain/collision_136.inc"
};
static NJS_MATERIAL collision_138_materials[1] = {
#include "stg28/terrain/collision_138_materials.inc"
};
static Sint16 collision_138_faces[226] = {
#include "stg28/terrain/collision_138_faces.inc"
};
static NJS_MESHSET collision_138_meshsets[1] = {
#include "stg28/terrain/collision_138_meshsets.inc"
};
static NJS_POINT3 collision_138_points[85] = {
#include "stg28/terrain/collision_138_points.inc"
};
static NJS_MODEL collision_138_model = {
#include "stg28/terrain/collision_138_model.inc"
};
static NJS_OBJECT collision_138 = {
#include "stg28/terrain/collision_138.inc"
};
static NJS_MATERIAL collision_140_materials[1] = {
#include "stg28/terrain/collision_140_materials.inc"
};
static Sint16 collision_140_faces[226] = {
#include "stg28/terrain/collision_140_faces.inc"
};
static NJS_MESHSET collision_140_meshsets[1] = {
#include "stg28/terrain/collision_140_meshsets.inc"
};
static NJS_POINT3 collision_140_points[93] = {
#include "stg28/terrain/collision_140_points.inc"
};
static NJS_MODEL collision_140_model = {
#include "stg28/terrain/collision_140_model.inc"
};
static NJS_OBJECT collision_140 = {
#include "stg28/terrain/collision_140.inc"
};
static NJS_MATERIAL collision_142_materials[1] = {
#include "stg28/terrain/collision_142_materials.inc"
};
static Sint16 collision_142_faces[62] = {
#include "stg28/terrain/collision_142_faces.inc"
};
static NJS_MESHSET collision_142_meshsets[1] = {
#include "stg28/terrain/collision_142_meshsets.inc"
};
static NJS_POINT3 collision_142_points[26] = {
#include "stg28/terrain/collision_142_points.inc"
};
static NJS_MODEL collision_142_model = {
#include "stg28/terrain/collision_142_model.inc"
};
static NJS_OBJECT collision_142 = {
#include "stg28/terrain/collision_142.inc"
};
static NJS_MATERIAL collision_156_materials[1] = {
#include "stg28/terrain/collision_156_materials.inc"
};
static Sint16 collision_156_faces[56] = {
#include "stg28/terrain/collision_156_faces.inc"
};
static NJS_MESHSET collision_156_meshsets[1] = {
#include "stg28/terrain/collision_156_meshsets.inc"
};
static NJS_POINT3 collision_156_points[25] = {
#include "stg28/terrain/collision_156_points.inc"
};
static NJS_MODEL collision_156_model = {
#include "stg28/terrain/collision_156_model.inc"
};
static NJS_OBJECT collision_156 = {
#include "stg28/terrain/collision_156.inc"
};
static NJS_MATERIAL collision_160_materials[1] = {
#include "stg28/terrain/collision_160_materials.inc"
};
static Sint16 collision_160_faces[59] = {
#include "stg28/terrain/collision_160_faces.inc"
};
static NJS_MESHSET collision_160_meshsets[1] = {
#include "stg28/terrain/collision_160_meshsets.inc"
};
static NJS_POINT3 collision_160_points[27] = {
#include "stg28/terrain/collision_160_points.inc"
};
static NJS_MODEL collision_160_model = {
#include "stg28/terrain/collision_160_model.inc"
};
static NJS_OBJECT collision_160 = {
#include "stg28/terrain/collision_160.inc"
};
static NJS_MATERIAL collision_163_materials[1] = {
#include "stg28/terrain/collision_163_materials.inc"
};
static Sint16 collision_163_faces[36] = {
#include "stg28/terrain/collision_163_faces.inc"
};
static NJS_MESHSET collision_163_meshsets[1] = {
#include "stg28/terrain/collision_163_meshsets.inc"
};
static NJS_POINT3 collision_163_points[24] = {
#include "stg28/terrain/collision_163_points.inc"
};
static NJS_MODEL collision_163_model = {
#include "stg28/terrain/collision_163_model.inc"
};
static NJS_OBJECT collision_163 = {
#include "stg28/terrain/collision_163.inc"
};
static NJS_MATERIAL collision_170_materials[1] = {
#include "stg28/terrain/collision_170_materials.inc"
};
static Sint16 collision_170_faces[24] = {
#include "stg28/terrain/collision_170_faces.inc"
};
static NJS_MESHSET collision_170_meshsets[1] = {
#include "stg28/terrain/collision_170_meshsets.inc"
};
static NJS_POINT3 collision_170_points[16] = {
#include "stg28/terrain/collision_170_points.inc"
};
static NJS_MODEL collision_170_model = {
#include "stg28/terrain/collision_170_model.inc"
};
static NJS_OBJECT collision_170 = {
#include "stg28/terrain/collision_170.inc"
};
static NJS_MATERIAL collision_175_materials[1] = {
#include "stg28/terrain/collision_175_materials.inc"
};
static Sint16 collision_175_faces[27] = {
#include "stg28/terrain/collision_175_faces.inc"
};
static NJS_MESHSET collision_175_meshsets[1] = {
#include "stg28/terrain/collision_175_meshsets.inc"
};
static NJS_POINT3 collision_175_points[16] = {
#include "stg28/terrain/collision_175_points.inc"
};
static NJS_MODEL collision_175_model = {
#include "stg28/terrain/collision_175_model.inc"
};
static NJS_OBJECT collision_175 = {
#include "stg28/terrain/collision_175.inc"
};
static NJS_MATERIAL collision_182_materials[1] = {
#include "stg28/terrain/collision_182_materials.inc"
};
static Sint16 collision_182_faces[27] = {
#include "stg28/terrain/collision_182_faces.inc"
};
static NJS_MESHSET collision_182_meshsets[1] = {
#include "stg28/terrain/collision_182_meshsets.inc"
};
static NJS_POINT3 collision_182_points[16] = {
#include "stg28/terrain/collision_182_points.inc"
};
static NJS_MODEL collision_182_model = {
#include "stg28/terrain/collision_182_model.inc"
};
static NJS_OBJECT collision_182 = {
#include "stg28/terrain/collision_182.inc"
};
static NJS_MATERIAL collision_183_materials[1] = {
#include "stg28/terrain/collision_183_materials.inc"
};
static Sint16 collision_183_faces[79] = {
#include "stg28/terrain/collision_183_faces.inc"
};
static NJS_MESHSET collision_183_meshsets[1] = {
#include "stg28/terrain/collision_183_meshsets.inc"
};
static NJS_POINT3 collision_183_points[37] = {
#include "stg28/terrain/collision_183_points.inc"
};
static NJS_MODEL collision_183_model = {
#include "stg28/terrain/collision_183_model.inc"
};
static NJS_OBJECT collision_183 = {
#include "stg28/terrain/collision_183.inc"
};
static NJS_MATERIAL collision_184_materials[1] = {
#include "stg28/terrain/collision_184_materials.inc"
};
static Sint16 collision_184_faces[325] = {
#include "stg28/terrain/collision_184_faces.inc"
};
static NJS_MESHSET collision_184_meshsets[1] = {
#include "stg28/terrain/collision_184_meshsets.inc"
};
static NJS_POINT3 collision_184_points[129] = {
#include "stg28/terrain/collision_184_points.inc"
};
static NJS_MODEL collision_184_model = {
#include "stg28/terrain/collision_184_model.inc"
};
static NJS_OBJECT collision_184 = {
#include "stg28/terrain/collision_184.inc"
};
static NJS_MATERIAL collision_185_materials[1] = {
#include "stg28/terrain/collision_185_materials.inc"
};
static Sint16 collision_185_faces[40] = {
#include "stg28/terrain/collision_185_faces.inc"
};
static NJS_MESHSET collision_185_meshsets[1] = {
#include "stg28/terrain/collision_185_meshsets.inc"
};
static NJS_POINT3 collision_185_points[18] = {
#include "stg28/terrain/collision_185_points.inc"
};
static NJS_MODEL collision_185_model = {
#include "stg28/terrain/collision_185_model.inc"
};
static NJS_OBJECT collision_185 = {
#include "stg28/terrain/collision_185.inc"
};
static NJS_MATERIAL collision_186_materials[1] = {
#include "stg28/terrain/collision_186_materials.inc"
};
static Sint16 collision_186_faces[195] = {
#include "stg28/terrain/collision_186_faces.inc"
};
static NJS_MESHSET collision_186_meshsets[1] = {
#include "stg28/terrain/collision_186_meshsets.inc"
};
static NJS_POINT3 collision_186_points[90] = {
#include "stg28/terrain/collision_186_points.inc"
};
static NJS_MODEL collision_186_model = {
#include "stg28/terrain/collision_186_model.inc"
};
static NJS_OBJECT collision_186 = {
#include "stg28/terrain/collision_186.inc"
};
static NJS_MATERIAL collision_187_materials[1] = {
#include "stg28/terrain/collision_187_materials.inc"
};
static Sint16 collision_187_faces[110] = {
#include "stg28/terrain/collision_187_faces.inc"
};
static NJS_MESHSET collision_187_meshsets[1] = {
#include "stg28/terrain/collision_187_meshsets.inc"
};
static NJS_POINT3 collision_187_points[56] = {
#include "stg28/terrain/collision_187_points.inc"
};
static NJS_MODEL collision_187_model = {
#include "stg28/terrain/collision_187_model.inc"
};
static NJS_OBJECT collision_187 = {
#include "stg28/terrain/collision_187.inc"
};
static NJS_MATERIAL collision_188_materials[1] = {
#include "stg28/terrain/collision_188_materials.inc"
};
static Sint16 collision_188_faces[14] = {
#include "stg28/terrain/collision_188_faces.inc"
};
static NJS_MESHSET collision_188_meshsets[1] = {
#include "stg28/terrain/collision_188_meshsets.inc"
};
static NJS_POINT3 collision_188_points[9] = {
#include "stg28/terrain/collision_188_points.inc"
};
static NJS_MODEL collision_188_model = {
#include "stg28/terrain/collision_188_model.inc"
};
static NJS_OBJECT collision_188 = {
#include "stg28/terrain/collision_188.inc"
};
static NJS_MATERIAL collision_189_materials[1] = {
#include "stg28/terrain/collision_189_materials.inc"
};
static Sint16 collision_189_faces[64] = {
#include "stg28/terrain/collision_189_faces.inc"
};
static NJS_MESHSET collision_189_meshsets[1] = {
#include "stg28/terrain/collision_189_meshsets.inc"
};
static NJS_POINT3 collision_189_points[24] = {
#include "stg28/terrain/collision_189_points.inc"
};
static NJS_MODEL collision_189_model = {
#include "stg28/terrain/collision_189_model.inc"
};
static NJS_OBJECT collision_189 = {
#include "stg28/terrain/collision_189.inc"
};
static NJS_MATERIAL collision_190_materials[1] = {
#include "stg28/terrain/collision_190_materials.inc"
};
static Sint16 collision_190_faces[164] = {
#include "stg28/terrain/collision_190_faces.inc"
};
static NJS_MESHSET collision_190_meshsets[1] = {
#include "stg28/terrain/collision_190_meshsets.inc"
};
static NJS_POINT3 collision_190_points[40] = {
#include "stg28/terrain/collision_190_points.inc"
};
static NJS_MODEL collision_190_model = {
#include "stg28/terrain/collision_190_model.inc"
};
static NJS_OBJECT collision_190 = {
#include "stg28/terrain/collision_190.inc"
};
static NJS_MATERIAL collision_191_materials[1] = {
#include "stg28/terrain/collision_191_materials.inc"
};
static Sint16 collision_191_faces[249] = {
#include "stg28/terrain/collision_191_faces.inc"
};
static NJS_MESHSET collision_191_meshsets[1] = {
#include "stg28/terrain/collision_191_meshsets.inc"
};
static NJS_POINT3 collision_191_points[66] = {
#include "stg28/terrain/collision_191_points.inc"
};
static NJS_MODEL collision_191_model = {
#include "stg28/terrain/collision_191_model.inc"
};
static NJS_OBJECT collision_191 = {
#include "stg28/terrain/collision_191.inc"
};
static NJS_MATERIAL collision_192_materials[1] = {
#include "stg28/terrain/collision_192_materials.inc"
};
static Sint16 collision_192_faces[240] = {
#include "stg28/terrain/collision_192_faces.inc"
};
static NJS_MESHSET collision_192_meshsets[1] = {
#include "stg28/terrain/collision_192_meshsets.inc"
};
static NJS_POINT3 collision_192_points[96] = {
#include "stg28/terrain/collision_192_points.inc"
};
static NJS_MODEL collision_192_model = {
#include "stg28/terrain/collision_192_model.inc"
};
static NJS_OBJECT collision_192 = {
#include "stg28/terrain/collision_192.inc"
};
static NJS_MATERIAL collision_193_materials[1] = {
#include "stg28/terrain/collision_193_materials.inc"
};
static Sint16 collision_193_faces[231] = {
#include "stg28/terrain/collision_193_faces.inc"
};
static NJS_MESHSET collision_193_meshsets[1] = {
#include "stg28/terrain/collision_193_meshsets.inc"
};
static NJS_POINT3 collision_193_points[81] = {
#include "stg28/terrain/collision_193_points.inc"
};
static NJS_MODEL collision_193_model = {
#include "stg28/terrain/collision_193_model.inc"
};
static NJS_OBJECT collision_193 = {
#include "stg28/terrain/collision_193.inc"
};
static NJS_MATERIAL collision_194_materials[1] = {
#include "stg28/terrain/collision_194_materials.inc"
};
static Sint16 collision_194_faces[285] = {
#include "stg28/terrain/collision_194_faces.inc"
};
static NJS_MESHSET collision_194_meshsets[1] = {
#include "stg28/terrain/collision_194_meshsets.inc"
};
static NJS_POINT3 collision_194_points[176] = {
#include "stg28/terrain/collision_194_points.inc"
};
static NJS_MODEL collision_194_model = {
#include "stg28/terrain/collision_194_model.inc"
};
static NJS_OBJECT collision_194 = {
#include "stg28/terrain/collision_194.inc"
};
static NJS_MATERIAL collision_195_materials[1] = {
#include "stg28/terrain/collision_195_materials.inc"
};
static Sint16 collision_195_faces[333] = {
#include "stg28/terrain/collision_195_faces.inc"
};
static NJS_MESHSET collision_195_meshsets[1] = {
#include "stg28/terrain/collision_195_meshsets.inc"
};
static NJS_POINT3 collision_195_points[208] = {
#include "stg28/terrain/collision_195_points.inc"
};
static NJS_MODEL collision_195_model = {
#include "stg28/terrain/collision_195_model.inc"
};
static NJS_OBJECT collision_195 = {
#include "stg28/terrain/collision_195.inc"
};
static NJS_MATERIAL collision_196_materials[1] = {
#include "stg28/terrain/collision_196_materials.inc"
};
static Sint16 collision_196_faces[54] = {
#include "stg28/terrain/collision_196_faces.inc"
};
static NJS_MESHSET collision_196_meshsets[1] = {
#include "stg28/terrain/collision_196_meshsets.inc"
};
static NJS_POINT3 collision_196_points[32] = {
#include "stg28/terrain/collision_196_points.inc"
};
static NJS_MODEL collision_196_model = {
#include "stg28/terrain/collision_196_model.inc"
};
static NJS_OBJECT collision_196 = {
#include "stg28/terrain/collision_196.inc"
};
static NJS_MATERIAL collision_197_materials[1] = {
#include "stg28/terrain/collision_197_materials.inc"
};
static Sint16 collision_197_faces[259] = {
#include "stg28/terrain/collision_197_faces.inc"
};
static NJS_MESHSET collision_197_meshsets[1] = {
#include "stg28/terrain/collision_197_meshsets.inc"
};
static NJS_POINT3 collision_197_points[91] = {
#include "stg28/terrain/collision_197_points.inc"
};
static NJS_MODEL collision_197_model = {
#include "stg28/terrain/collision_197_model.inc"
};
static NJS_OBJECT collision_197 = {
#include "stg28/terrain/collision_197.inc"
};
static NJS_MATERIAL collision_198_materials[1] = {
#include "stg28/terrain/collision_198_materials.inc"
};
static Sint16 collision_198_faces[65] = {
#include "stg28/terrain/collision_198_faces.inc"
};
static NJS_MESHSET collision_198_meshsets[1] = {
#include "stg28/terrain/collision_198_meshsets.inc"
};
static NJS_POINT3 collision_198_points[24] = {
#include "stg28/terrain/collision_198_points.inc"
};
static NJS_MODEL collision_198_model = {
#include "stg28/terrain/collision_198_model.inc"
};
static NJS_OBJECT collision_198 = {
#include "stg28/terrain/collision_198.inc"
};
static NJS_MATERIAL collision_199_materials[1] = {
#include "stg28/terrain/collision_199_materials.inc"
};
static Sint16 collision_199_faces[59] = {
#include "stg28/terrain/collision_199_faces.inc"
};
static NJS_MESHSET collision_199_meshsets[1] = {
#include "stg28/terrain/collision_199_meshsets.inc"
};
static NJS_POINT3 collision_199_points[24] = {
#include "stg28/terrain/collision_199_points.inc"
};
static NJS_MODEL collision_199_model = {
#include "stg28/terrain/collision_199_model.inc"
};
static NJS_OBJECT collision_199 = {
#include "stg28/terrain/collision_199.inc"
};
static NJS_MATERIAL collision_200_materials[1] = {
#include "stg28/terrain/collision_200_materials.inc"
};
static Sint16 collision_200_faces[288] = {
#include "stg28/terrain/collision_200_faces.inc"
};
static NJS_MESHSET collision_200_meshsets[1] = {
#include "stg28/terrain/collision_200_meshsets.inc"
};
static NJS_POINT3 collision_200_points[140] = {
#include "stg28/terrain/collision_200_points.inc"
};
static NJS_MODEL collision_200_model = {
#include "stg28/terrain/collision_200_model.inc"
};
static NJS_OBJECT collision_200 = {
#include "stg28/terrain/collision_200.inc"
};
static NJS_MATERIAL collision_201_materials[1] = {
#include "stg28/terrain/collision_201_materials.inc"
};
static Sint16 collision_201_faces[279] = {
#include "stg28/terrain/collision_201_faces.inc"
};
static NJS_MESHSET collision_201_meshsets[1] = {
#include "stg28/terrain/collision_201_meshsets.inc"
};
static NJS_POINT3 collision_201_points[150] = {
#include "stg28/terrain/collision_201_points.inc"
};
static NJS_MODEL collision_201_model = {
#include "stg28/terrain/collision_201_model.inc"
};
static NJS_OBJECT collision_201 = {
#include "stg28/terrain/collision_201.inc"
};
static NJS_MATERIAL collision_202_materials[1] = {
#include "stg28/terrain/collision_202_materials.inc"
};
static Sint16 collision_202_faces[288] = {
#include "stg28/terrain/collision_202_faces.inc"
};
static NJS_MESHSET collision_202_meshsets[1] = {
#include "stg28/terrain/collision_202_meshsets.inc"
};
static NJS_POINT3 collision_202_points[140] = {
#include "stg28/terrain/collision_202_points.inc"
};
static NJS_MODEL collision_202_model = {
#include "stg28/terrain/collision_202_model.inc"
};
static NJS_OBJECT collision_202 = {
#include "stg28/terrain/collision_202.inc"
};
static NJS_MATERIAL collision_203_materials[1] = {
#include "stg28/terrain/collision_203_materials.inc"
};
static Sint16 collision_203_faces[279] = {
#include "stg28/terrain/collision_203_faces.inc"
};
static NJS_MESHSET collision_203_meshsets[1] = {
#include "stg28/terrain/collision_203_meshsets.inc"
};
static NJS_POINT3 collision_203_points[150] = {
#include "stg28/terrain/collision_203_points.inc"
};
static NJS_MODEL collision_203_model = {
#include "stg28/terrain/collision_203_model.inc"
};
static NJS_OBJECT collision_203 = {
#include "stg28/terrain/collision_203.inc"
};
static NJS_MATERIAL collision_204_materials[1] = {
#include "stg28/terrain/collision_204_materials.inc"
};
static Sint16 collision_204_faces[333] = {
#include "stg28/terrain/collision_204_faces.inc"
};
static NJS_MESHSET collision_204_meshsets[1] = {
#include "stg28/terrain/collision_204_meshsets.inc"
};
static NJS_POINT3 collision_204_points[180] = {
#include "stg28/terrain/collision_204_points.inc"
};
static NJS_MODEL collision_204_model = {
#include "stg28/terrain/collision_204_model.inc"
};
static NJS_OBJECT collision_204 = {
#include "stg28/terrain/collision_204.inc"
};
static NJS_MATERIAL collision_205_materials[1] = {
#include "stg28/terrain/collision_205_materials.inc"
};
static Sint16 collision_205_faces[351] = {
#include "stg28/terrain/collision_205_faces.inc"
};
static NJS_MESHSET collision_205_meshsets[1] = {
#include "stg28/terrain/collision_205_meshsets.inc"
};
static NJS_POINT3 collision_205_points[190] = {
#include "stg28/terrain/collision_205_points.inc"
};
static NJS_MODEL collision_205_model = {
#include "stg28/terrain/collision_205_model.inc"
};
static NJS_OBJECT collision_205 = {
#include "stg28/terrain/collision_205.inc"
};
static NJS_MATERIAL collision_206_materials[1] = {
#include "stg28/terrain/collision_206_materials.inc"
};
static Sint16 collision_206_faces[333] = {
#include "stg28/terrain/collision_206_faces.inc"
};
static NJS_MESHSET collision_206_meshsets[1] = {
#include "stg28/terrain/collision_206_meshsets.inc"
};
static NJS_POINT3 collision_206_points[180] = {
#include "stg28/terrain/collision_206_points.inc"
};
static NJS_MODEL collision_206_model = {
#include "stg28/terrain/collision_206_model.inc"
};
static NJS_OBJECT collision_206 = {
#include "stg28/terrain/collision_206.inc"
};
static NJS_MATERIAL collision_207_materials[1] = {
#include "stg28/terrain/collision_207_materials.inc"
};
static Sint16 collision_207_faces[351] = {
#include "stg28/terrain/collision_207_faces.inc"
};
static NJS_MESHSET collision_207_meshsets[1] = {
#include "stg28/terrain/collision_207_meshsets.inc"
};
static NJS_POINT3 collision_207_points[190] = {
#include "stg28/terrain/collision_207_points.inc"
};
static NJS_MODEL collision_207_model = {
#include "stg28/terrain/collision_207_model.inc"
};
static NJS_OBJECT collision_207 = {
#include "stg28/terrain/collision_207.inc"
};
static NJS_MATERIAL collision_208_materials[1] = {
#include "stg28/terrain/collision_208_materials.inc"
};
static Sint16 collision_208_faces[347] = {
#include "stg28/terrain/collision_208_faces.inc"
};
static NJS_MESHSET collision_208_meshsets[1] = {
#include "stg28/terrain/collision_208_meshsets.inc"
};
static NJS_POINT3 collision_208_points[178] = {
#include "stg28/terrain/collision_208_points.inc"
};
static NJS_MODEL collision_208_model = {
#include "stg28/terrain/collision_208_model.inc"
};
static NJS_OBJECT collision_208 = {
#include "stg28/terrain/collision_208.inc"
};
static NJS_MATERIAL collision_209_materials[1] = {
#include "stg28/terrain/collision_209_materials.inc"
};
static Sint16 collision_209_faces[423] = {
#include "stg28/terrain/collision_209_faces.inc"
};
static NJS_MESHSET collision_209_meshsets[1] = {
#include "stg28/terrain/collision_209_meshsets.inc"
};
static NJS_POINT3 collision_209_points[230] = {
#include "stg28/terrain/collision_209_points.inc"
};
static NJS_MODEL collision_209_model = {
#include "stg28/terrain/collision_209_model.inc"
};
static NJS_OBJECT collision_209 = {
#include "stg28/terrain/collision_209.inc"
};
static NJS_MATERIAL collision_210_materials[1] = {
#include "stg28/terrain/collision_210_materials.inc"
};
static Sint16 collision_210_faces[347] = {
#include "stg28/terrain/collision_210_faces.inc"
};
static NJS_MESHSET collision_210_meshsets[1] = {
#include "stg28/terrain/collision_210_meshsets.inc"
};
static NJS_POINT3 collision_210_points[178] = {
#include "stg28/terrain/collision_210_points.inc"
};
static NJS_MODEL collision_210_model = {
#include "stg28/terrain/collision_210_model.inc"
};
static NJS_OBJECT collision_210 = {
#include "stg28/terrain/collision_210.inc"
};
static NJS_MATERIAL collision_211_materials[1] = {
#include "stg28/terrain/collision_211_materials.inc"
};
static Sint16 collision_211_faces[423] = {
#include "stg28/terrain/collision_211_faces.inc"
};
static NJS_MESHSET collision_211_meshsets[1] = {
#include "stg28/terrain/collision_211_meshsets.inc"
};
static NJS_POINT3 collision_211_points[230] = {
#include "stg28/terrain/collision_211_points.inc"
};
static NJS_MODEL collision_211_model = {
#include "stg28/terrain/collision_211_model.inc"
};
static NJS_OBJECT collision_211 = {
#include "stg28/terrain/collision_211.inc"
};
static Sint16 draw_249_polygons[589] = {
#include "stg28/terrain/draw_249_polygons.inc"
};
static Sint32 draw_249_vertices[423] = {
#include "stg28/terrain/draw_249_vertices.inc"
};
static NJS_CNK_MODEL draw_249_model = {
#include "stg28/terrain/draw_249_model.inc"
};
static NJS_CNK_OBJECT draw_249 = {
#include "stg28/terrain/draw_249.inc"
};
static Sint16 draw_248_polygons[1549] = {
#include "stg28/terrain/draw_248_polygons.inc"
};
static Sint32 draw_248_vertices[1235] = {
#include "stg28/terrain/draw_248_vertices.inc"
};
static NJS_CNK_MODEL draw_248_model = {
#include "stg28/terrain/draw_248_model.inc"
};
static NJS_CNK_OBJECT draw_248 = {
#include "stg28/terrain/draw_248.inc"
};
static Sint16 draw_247_polygons[65] = {
#include "stg28/terrain/draw_247_polygons.inc"
};
static Sint32 draw_247_vertices[35] = {
#include "stg28/terrain/draw_247_vertices.inc"
};
static NJS_CNK_MODEL draw_247_model = {
#include "stg28/terrain/draw_247_model.inc"
};
static NJS_CNK_OBJECT draw_247 = {
#include "stg28/terrain/draw_247.inc"
};
static Sint16 draw_246_polygons[477] = {
#include "stg28/terrain/draw_246_polygons.inc"
};
static Sint32 draw_246_vertices[327] = {
#include "stg28/terrain/draw_246_vertices.inc"
};
static NJS_CNK_MODEL draw_246_model = {
#include "stg28/terrain/draw_246_model.inc"
};
static NJS_CNK_OBJECT draw_246 = {
#include "stg28/terrain/draw_246.inc"
};
static Sint16 draw_238_polygons[437] = {
#include "stg28/terrain/draw_238_polygons.inc"
};
static Sint32 draw_238_vertices[275] = {
#include "stg28/terrain/draw_238_vertices.inc"
};
static NJS_CNK_MODEL draw_238_model = {
#include "stg28/terrain/draw_238_model.inc"
};
static NJS_CNK_OBJECT draw_245 = {
#include "stg28/terrain/draw_245.inc"
};
static Sint16 draw_236_polygons[171] = {
#include "stg28/terrain/draw_236_polygons.inc"
};
static Sint32 draw_236_vertices[107] = {
#include "stg28/terrain/draw_236_vertices.inc"
};
static NJS_CNK_MODEL draw_236_model = {
#include "stg28/terrain/draw_236_model.inc"
};
static NJS_CNK_OBJECT draw_237 = {
#include "stg28/terrain/draw_237.inc"
};
static Sint16 draw_235_polygons[495] = {
#include "stg28/terrain/draw_235_polygons.inc"
};
static Sint32 draw_235_vertices[323] = {
#include "stg28/terrain/draw_235_vertices.inc"
};
static NJS_CNK_MODEL draw_235_model = {
#include "stg28/terrain/draw_235_model.inc"
};
static NJS_CNK_OBJECT draw_235 = {
#include "stg28/terrain/draw_235.inc"
};
static Sint16 draw_234_polygons[97] = {
#include "stg28/terrain/draw_234_polygons.inc"
};
static Sint32 draw_234_vertices[55] = {
#include "stg28/terrain/draw_234_vertices.inc"
};
static NJS_CNK_MODEL draw_234_model = {
#include "stg28/terrain/draw_234_model.inc"
};
static NJS_CNK_OBJECT draw_234 = {
#include "stg28/terrain/draw_234.inc"
};
static Sint16 draw_233_polygons[243] = {
#include "stg28/terrain/draw_233_polygons.inc"
};
static Sint32 draw_233_vertices[159] = {
#include "stg28/terrain/draw_233_vertices.inc"
};
static NJS_CNK_MODEL draw_233_model = {
#include "stg28/terrain/draw_233_model.inc"
};
static NJS_CNK_OBJECT draw_233 = {
#include "stg28/terrain/draw_233.inc"
};
static Sint16 draw_232_polygons[397] = {
#include "stg28/terrain/draw_232_polygons.inc"
};
static Sint32 draw_232_vertices[259] = {
#include "stg28/terrain/draw_232_vertices.inc"
};
static NJS_CNK_MODEL draw_232_model = {
#include "stg28/terrain/draw_232_model.inc"
};
static NJS_CNK_OBJECT draw_232 = {
#include "stg28/terrain/draw_232.inc"
};
static Sint16 draw_231_polygons[121] = {
#include "stg28/terrain/draw_231_polygons.inc"
};
static Sint32 draw_231_vertices[107] = {
#include "stg28/terrain/draw_231_vertices.inc"
};
static NJS_CNK_MODEL draw_231_model = {
#include "stg28/terrain/draw_231_model.inc"
};
static NJS_CNK_OBJECT draw_231 = {
#include "stg28/terrain/draw_231.inc"
};
static Sint16 draw_230_polygons[759] = {
#include "stg28/terrain/draw_230_polygons.inc"
};
static Sint32 draw_230_vertices[459] = {
#include "stg28/terrain/draw_230_vertices.inc"
};
static NJS_CNK_MODEL draw_230_model = {
#include "stg28/terrain/draw_230_model.inc"
};
static NJS_CNK_OBJECT draw_230 = {
#include "stg28/terrain/draw_230.inc"
};
static Sint16 draw_229_polygons[549] = {
#include "stg28/terrain/draw_229_polygons.inc"
};
static Sint32 draw_229_vertices[335] = {
#include "stg28/terrain/draw_229_vertices.inc"
};
static NJS_CNK_MODEL draw_229_model = {
#include "stg28/terrain/draw_229_model.inc"
};
static NJS_CNK_OBJECT draw_229 = {
#include "stg28/terrain/draw_229.inc"
};
static Sint16 draw_225_polygons[171] = {
#include "stg28/terrain/draw_225_polygons.inc"
};
static Sint32 draw_225_vertices[99] = {
#include "stg28/terrain/draw_225_vertices.inc"
};
static NJS_CNK_MODEL draw_225_model = {
#include "stg28/terrain/draw_225_model.inc"
};
static NJS_CNK_OBJECT draw_228 = {
#include "stg28/terrain/draw_228.inc"
};
static Sint16 draw_224_polygons[1611] = {
#include "stg28/terrain/draw_224_polygons.inc"
};
static Sint32 draw_224_vertices[1083] = {
#include "stg28/terrain/draw_224_vertices.inc"
};
static NJS_CNK_MODEL draw_224_model = {
#include "stg28/terrain/draw_224_model.inc"
};
static NJS_CNK_OBJECT draw_224 = {
#include "stg28/terrain/draw_224.inc"
};
static Sint16 draw_223_polygons[153] = {
#include "stg28/terrain/draw_223_polygons.inc"
};
static Sint32 draw_223_vertices[127] = {
#include "stg28/terrain/draw_223_vertices.inc"
};
static NJS_CNK_MODEL draw_223_model = {
#include "stg28/terrain/draw_223_model.inc"
};
static NJS_CNK_OBJECT draw_223 = {
#include "stg28/terrain/draw_223.inc"
};
static Sint16 draw_220_polygons[57] = {
#include "stg28/terrain/draw_220_polygons.inc"
};
static Sint32 draw_220_vertices[51] = {
#include "stg28/terrain/draw_220_vertices.inc"
};
static NJS_CNK_MODEL draw_220_model = {
#include "stg28/terrain/draw_220_model.inc"
};
static NJS_CNK_OBJECT draw_222 = {
#include "stg28/terrain/draw_222.inc"
};
static Sint16 draw_219_polygons[57] = {
#include "stg28/terrain/draw_219_polygons.inc"
};
static Sint32 draw_219_vertices[51] = {
#include "stg28/terrain/draw_219_vertices.inc"
};
static NJS_CNK_MODEL draw_219_model = {
#include "stg28/terrain/draw_219_model.inc"
};
static NJS_CNK_OBJECT draw_219 = {
#include "stg28/terrain/draw_219.inc"
};
static Sint16 draw_218_polygons[613] = {
#include "stg28/terrain/draw_218_polygons.inc"
};
static Sint32 draw_218_vertices[591] = {
#include "stg28/terrain/draw_218_vertices.inc"
};
static NJS_CNK_MODEL draw_218_model = {
#include "stg28/terrain/draw_218_model.inc"
};
static NJS_CNK_OBJECT draw_218 = {
#include "stg28/terrain/draw_218.inc"
};
static Sint16 draw_217_polygons[627] = {
#include "stg28/terrain/draw_217_polygons.inc"
};
static Sint32 draw_217_vertices[447] = {
#include "stg28/terrain/draw_217_vertices.inc"
};
static NJS_CNK_MODEL draw_217_model = {
#include "stg28/terrain/draw_217_model.inc"
};
static NJS_CNK_OBJECT draw_217 = {
#include "stg28/terrain/draw_217.inc"
};
static Sint16 draw_216_polygons[117] = {
#include "stg28/terrain/draw_216_polygons.inc"
};
static Sint32 draw_216_vertices[67] = {
#include "stg28/terrain/draw_216_vertices.inc"
};
static NJS_CNK_MODEL draw_216_model = {
#include "stg28/terrain/draw_216_model.inc"
};
static NJS_CNK_OBJECT draw_216 = {
#include "stg28/terrain/draw_216.inc"
};
static Sint16 draw_213_polygons[1153] = {
#include "stg28/terrain/draw_213_polygons.inc"
};
static Sint32 draw_213_vertices[751] = {
#include "stg28/terrain/draw_213_vertices.inc"
};
static NJS_CNK_MODEL draw_213_model = {
#include "stg28/terrain/draw_213_model.inc"
};
static NJS_CNK_OBJECT draw_215 = {
#include "stg28/terrain/draw_215.inc"
};
static Sint16 draw_212_polygons[565] = {
#include "stg28/terrain/draw_212_polygons.inc"
};
static Sint32 draw_212_vertices[371] = {
#include "stg28/terrain/draw_212_vertices.inc"
};
static NJS_CNK_MODEL draw_212_model = {
#include "stg28/terrain/draw_212_model.inc"
};
static NJS_CNK_OBJECT draw_212 = {
#include "stg28/terrain/draw_212.inc"
};
static Sint16 draw_210_polygons[739] = {
#include "stg28/terrain/draw_210_polygons.inc"
};
static Sint32 draw_210_vertices[483] = {
#include "stg28/terrain/draw_210_vertices.inc"
};
static NJS_CNK_MODEL draw_210_model = {
#include "stg28/terrain/draw_210_model.inc"
};
static NJS_CNK_OBJECT draw_211 = {
#include "stg28/terrain/draw_211.inc"
};
static Sint16 draw_208_polygons[739] = {
#include "stg28/terrain/draw_208_polygons.inc"
};
static Sint32 draw_208_vertices[483] = {
#include "stg28/terrain/draw_208_vertices.inc"
};
static NJS_CNK_MODEL draw_208_model = {
#include "stg28/terrain/draw_208_model.inc"
};
static NJS_CNK_OBJECT draw_209 = {
#include "stg28/terrain/draw_209.inc"
};
static Sint16 draw_207_polygons[65] = {
#include "stg28/terrain/draw_207_polygons.inc"
};
static Sint32 draw_207_vertices[67] = {
#include "stg28/terrain/draw_207_vertices.inc"
};
static NJS_CNK_MODEL draw_207_model = {
#include "stg28/terrain/draw_207_model.inc"
};
static NJS_CNK_OBJECT draw_207 = {
#include "stg28/terrain/draw_207.inc"
};
static Sint16 draw_205_polygons[403] = {
#include "stg28/terrain/draw_205_polygons.inc"
};
static Sint32 draw_205_vertices[359] = {
#include "stg28/terrain/draw_205_vertices.inc"
};
static NJS_CNK_MODEL draw_205_model = {
#include "stg28/terrain/draw_205_model.inc"
};
static NJS_CNK_OBJECT draw_206 = {
#include "stg28/terrain/draw_206.inc"
};
static Sint16 draw_204_polygons[489] = {
#include "stg28/terrain/draw_204_polygons.inc"
};
static Sint32 draw_204_vertices[371] = {
#include "stg28/terrain/draw_204_vertices.inc"
};
static NJS_CNK_MODEL draw_204_model = {
#include "stg28/terrain/draw_204_model.inc"
};
static NJS_CNK_OBJECT draw_204 = {
#include "stg28/terrain/draw_204.inc"
};
static Sint16 draw_203_polygons[491] = {
#include "stg28/terrain/draw_203_polygons.inc"
};
static Sint32 draw_203_vertices[359] = {
#include "stg28/terrain/draw_203_vertices.inc"
};
static NJS_CNK_MODEL draw_203_model = {
#include "stg28/terrain/draw_203_model.inc"
};
static NJS_CNK_OBJECT draw_203 = {
#include "stg28/terrain/draw_203.inc"
};
static Sint16 draw_192_polygons[419] = {
#include "stg28/terrain/draw_192_polygons.inc"
};
static Sint32 draw_192_vertices[335] = {
#include "stg28/terrain/draw_192_vertices.inc"
};
static NJS_CNK_MODEL draw_192_model = {
#include "stg28/terrain/draw_192_model.inc"
};
static NJS_CNK_OBJECT draw_201 = {
#include "stg28/terrain/draw_201.inc"
};
static Sint16 draw_190_polygons[2287] = {
#include "stg28/terrain/draw_190_polygons.inc"
};
static Sint32 draw_190_vertices[1719] = {
#include "stg28/terrain/draw_190_vertices.inc"
};
static NJS_CNK_MODEL draw_190_model = {
#include "stg28/terrain/draw_190_model.inc"
};
static NJS_CNK_OBJECT draw_191 = {
#include "stg28/terrain/draw_191.inc"
};
static Sint16 draw_188_polygons[2307] = {
#include "stg28/terrain/draw_188_polygons.inc"
};
static Sint32 draw_188_vertices[1707] = {
#include "stg28/terrain/draw_188_vertices.inc"
};
static NJS_CNK_MODEL draw_188_model = {
#include "stg28/terrain/draw_188_model.inc"
};
static NJS_CNK_OBJECT draw_189 = {
#include "stg28/terrain/draw_189.inc"
};
static Sint16 draw_186_polygons[2327] = {
#include "stg28/terrain/draw_186_polygons.inc"
};
static Sint32 draw_186_vertices[1715] = {
#include "stg28/terrain/draw_186_vertices.inc"
};
static NJS_CNK_MODEL draw_186_model = {
#include "stg28/terrain/draw_186_model.inc"
};
static NJS_CNK_OBJECT draw_187 = {
#include "stg28/terrain/draw_187.inc"
};
static Sint16 draw_185_polygons[65] = {
#include "stg28/terrain/draw_185_polygons.inc"
};
static Sint32 draw_185_vertices[67] = {
#include "stg28/terrain/draw_185_vertices.inc"
};
static NJS_CNK_MODEL draw_185_model = {
#include "stg28/terrain/draw_185_model.inc"
};
static NJS_CNK_OBJECT draw_185 = {
#include "stg28/terrain/draw_185.inc"
};
static Sint16 draw_184_polygons[1101] = {
#include "stg28/terrain/draw_184_polygons.inc"
};
static Sint32 draw_184_vertices[699] = {
#include "stg28/terrain/draw_184_vertices.inc"
};
static NJS_CNK_MODEL draw_184_model = {
#include "stg28/terrain/draw_184_model.inc"
};
static NJS_CNK_OBJECT draw_184 = {
#include "stg28/terrain/draw_184.inc"
};
static Sint16 draw_182_polygons[2395] = {
#include "stg28/terrain/draw_182_polygons.inc"
};
static Sint32 draw_182_vertices[2915] = {
#include "stg28/terrain/draw_182_vertices.inc"
};
static NJS_CNK_MODEL draw_182_model = {
#include "stg28/terrain/draw_182_model.inc"
};
static NJS_CNK_OBJECT draw_183 = {
#include "stg28/terrain/draw_183.inc"
};
static Sint16 draw_180_polygons[2559] = {
#include "stg28/terrain/draw_180_polygons.inc"
};
static Sint32 draw_180_vertices[3203] = {
#include "stg28/terrain/draw_180_vertices.inc"
};
static NJS_CNK_MODEL draw_180_model = {
#include "stg28/terrain/draw_180_model.inc"
};
static NJS_CNK_OBJECT draw_181 = {
#include "stg28/terrain/draw_181.inc"
};
static Sint16 draw_159_polygons[151] = {
#include "stg28/terrain/draw_159_polygons.inc"
};
static Sint32 draw_159_vertices[99] = {
#include "stg28/terrain/draw_159_vertices.inc"
};
static NJS_CNK_MODEL draw_159_model = {
#include "stg28/terrain/draw_159_model.inc"
};
static NJS_CNK_OBJECT draw_179 = {
#include "stg28/terrain/draw_179.inc"
};
static Sint16 draw_149_polygons[199] = {
#include "stg28/terrain/draw_149_polygons.inc"
};
static Sint32 draw_149_vertices[179] = {
#include "stg28/terrain/draw_149_vertices.inc"
};
static NJS_CNK_MODEL draw_149_model = {
#include "stg28/terrain/draw_149_model.inc"
};
static NJS_CNK_OBJECT draw_158 = {
#include "stg28/terrain/draw_158.inc"
};
static Sint16 draw_147_polygons[213] = {
#include "stg28/terrain/draw_147_polygons.inc"
};
static Sint32 draw_147_vertices[187] = {
#include "stg28/terrain/draw_147_vertices.inc"
};
static NJS_CNK_MODEL draw_147_model = {
#include "stg28/terrain/draw_147_model.inc"
};
static NJS_CNK_OBJECT draw_148 = {
#include "stg28/terrain/draw_148.inc"
};
static Sint16 draw_137_polygons[545] = {
#include "stg28/terrain/draw_137_polygons.inc"
};
static Sint32 draw_137_vertices[379] = {
#include "stg28/terrain/draw_137_vertices.inc"
};
static NJS_CNK_MODEL draw_137_model = {
#include "stg28/terrain/draw_137_model.inc"
};
static NJS_CNK_OBJECT draw_146 = {
#include "stg28/terrain/draw_146.inc"
};
static Sint16 draw_134_polygons[1141] = {
#include "stg28/terrain/draw_134_polygons.inc"
};
static Sint32 draw_134_vertices[735] = {
#include "stg28/terrain/draw_134_vertices.inc"
};
static NJS_CNK_MODEL draw_134_model = {
#include "stg28/terrain/draw_134_model.inc"
};
static NJS_CNK_OBJECT draw_136 = {
#include "stg28/terrain/draw_136.inc"
};
static Sint16 draw_132_polygons[1621] = {
#include "stg28/terrain/draw_132_polygons.inc"
};
static Sint32 draw_132_vertices[1279] = {
#include "stg28/terrain/draw_132_vertices.inc"
};
static NJS_CNK_MODEL draw_132_model = {
#include "stg28/terrain/draw_132_model.inc"
};
static NJS_CNK_OBJECT draw_133 = {
#include "stg28/terrain/draw_133.inc"
};
static Sint16 draw_128_polygons[773] = {
#include "stg28/terrain/draw_128_polygons.inc"
};
static Sint32 draw_128_vertices[599] = {
#include "stg28/terrain/draw_128_vertices.inc"
};
static NJS_CNK_MODEL draw_128_model = {
#include "stg28/terrain/draw_128_model.inc"
};
static NJS_CNK_OBJECT draw_131 = {
#include "stg28/terrain/draw_131.inc"
};
static Sint16 draw_126_polygons[375] = {
#include "stg28/terrain/draw_126_polygons.inc"
};
static Sint32 draw_126_vertices[251] = {
#include "stg28/terrain/draw_126_vertices.inc"
};
static NJS_CNK_MODEL draw_126_model = {
#include "stg28/terrain/draw_126_model.inc"
};
static NJS_CNK_OBJECT draw_127 = {
#include "stg28/terrain/draw_127.inc"
};
static Sint16 draw_124_polygons[347] = {
#include "stg28/terrain/draw_124_polygons.inc"
};
static Sint32 draw_124_vertices[243] = {
#include "stg28/terrain/draw_124_vertices.inc"
};
static NJS_CNK_MODEL draw_124_model = {
#include "stg28/terrain/draw_124_model.inc"
};
static NJS_CNK_OBJECT draw_125 = {
#include "stg28/terrain/draw_125.inc"
};
static Sint16 draw_121_polygons[1141] = {
#include "stg28/terrain/draw_121_polygons.inc"
};
static Sint32 draw_121_vertices[715] = {
#include "stg28/terrain/draw_121_vertices.inc"
};
static NJS_CNK_MODEL draw_121_model = {
#include "stg28/terrain/draw_121_model.inc"
};
static NJS_CNK_OBJECT draw_123 = {
#include "stg28/terrain/draw_123.inc"
};
static Sint16 draw_113_polygons[431] = {
#include "stg28/terrain/draw_113_polygons.inc"
};
static Sint32 draw_113_vertices[299] = {
#include "stg28/terrain/draw_113_vertices.inc"
};
static NJS_CNK_MODEL draw_113_model = {
#include "stg28/terrain/draw_113_model.inc"
};
static NJS_CNK_OBJECT draw_120 = {
#include "stg28/terrain/draw_120.inc"
};
static Sint16 draw_107_polygons[439] = {
#include "stg28/terrain/draw_107_polygons.inc"
};
static Sint32 draw_107_vertices[291] = {
#include "stg28/terrain/draw_107_vertices.inc"
};
static NJS_CNK_MODEL draw_107_model = {
#include "stg28/terrain/draw_107_model.inc"
};
static NJS_CNK_OBJECT draw_112 = {
#include "stg28/terrain/draw_112.inc"
};
static Sint16 draw_101_polygons[243] = {
#include "stg28/terrain/draw_101_polygons.inc"
};
static Sint32 draw_101_vertices[159] = {
#include "stg28/terrain/draw_101_vertices.inc"
};
static NJS_CNK_MODEL draw_101_model = {
#include "stg28/terrain/draw_101_model.inc"
};
static NJS_CNK_OBJECT draw_106 = {
#include "stg28/terrain/draw_106.inc"
};
static Sint16 draw_89_polygons[243] = {
#include "stg28/terrain/draw_89_polygons.inc"
};
static Sint32 draw_89_vertices[163] = {
#include "stg28/terrain/draw_89_vertices.inc"
};
static NJS_CNK_MODEL draw_89_model = {
#include "stg28/terrain/draw_89_model.inc"
};
static NJS_CNK_OBJECT draw_100 = {
#include "stg28/terrain/draw_100.inc"
};
static Sint16 draw_88_polygons[243] = {
#include "stg28/terrain/draw_88_polygons.inc"
};
static Sint32 draw_88_vertices[159] = {
#include "stg28/terrain/draw_88_vertices.inc"
};
static NJS_CNK_MODEL draw_88_model = {
#include "stg28/terrain/draw_88_model.inc"
};
static NJS_CNK_OBJECT draw_88 = {
#include "stg28/terrain/draw_88.inc"
};
static Sint16 draw_80_polygons[141] = {
#include "stg28/terrain/draw_80_polygons.inc"
};
static Sint32 draw_80_vertices[91] = {
#include "stg28/terrain/draw_80_vertices.inc"
};
static NJS_CNK_MODEL draw_80_model = {
#include "stg28/terrain/draw_80_model.inc"
};
static NJS_CNK_OBJECT draw_87 = {
#include "stg28/terrain/draw_87.inc"
};
static Sint16 draw_73_polygons[155] = {
#include "stg28/terrain/draw_73_polygons.inc"
};
static Sint32 draw_73_vertices[107] = {
#include "stg28/terrain/draw_73_vertices.inc"
};
static NJS_CNK_MODEL draw_73_model = {
#include "stg28/terrain/draw_73_model.inc"
};
static NJS_CNK_OBJECT draw_79 = {
#include "stg28/terrain/draw_79.inc"
};
static Sint16 draw_68_polygons[43] = {
#include "stg28/terrain/draw_68_polygons.inc"
};
static Sint32 draw_68_vertices[43] = {
#include "stg28/terrain/draw_68_vertices.inc"
};
static NJS_CNK_MODEL draw_68_model = {
#include "stg28/terrain/draw_68_model.inc"
};
static NJS_CNK_OBJECT draw_72 = {
#include "stg28/terrain/draw_72.inc"
};
static Sint16 draw_63_polygons[43] = {
#include "stg28/terrain/draw_63_polygons.inc"
};
static Sint32 draw_63_vertices[43] = {
#include "stg28/terrain/draw_63_vertices.inc"
};
static NJS_CNK_MODEL draw_63_model = {
#include "stg28/terrain/draw_63_model.inc"
};
static NJS_CNK_OBJECT draw_67 = {
#include "stg28/terrain/draw_67.inc"
};
static Sint16 draw_62_polygons[409] = {
#include "stg28/terrain/draw_62_polygons.inc"
};
static Sint32 draw_62_vertices[291] = {
#include "stg28/terrain/draw_62_vertices.inc"
};
static NJS_CNK_MODEL draw_62_model = {
#include "stg28/terrain/draw_62_model.inc"
};
static NJS_CNK_OBJECT draw_62 = {
#include "stg28/terrain/draw_62.inc"
};
static Sint16 draw_61_polygons[819] = {
#include "stg28/terrain/draw_61_polygons.inc"
};
static Sint32 draw_61_vertices[559] = {
#include "stg28/terrain/draw_61_vertices.inc"
};
static NJS_CNK_MODEL draw_61_model = {
#include "stg28/terrain/draw_61_model.inc"
};
static NJS_CNK_OBJECT draw_61 = {
#include "stg28/terrain/draw_61.inc"
};
static Sint16 draw_60_polygons[243] = {
#include "stg28/terrain/draw_60_polygons.inc"
};
static Sint32 draw_60_vertices[131] = {
#include "stg28/terrain/draw_60_vertices.inc"
};
static NJS_CNK_MODEL draw_60_model = {
#include "stg28/terrain/draw_60_model.inc"
};
static NJS_CNK_OBJECT draw_60 = {
#include "stg28/terrain/draw_60.inc"
};
static Sint16 draw_54_polygons[57] = {
#include "stg28/terrain/draw_54_polygons.inc"
};
static Sint32 draw_54_vertices[35] = {
#include "stg28/terrain/draw_54_vertices.inc"
};
static NJS_CNK_MODEL draw_54_model = {
#include "stg28/terrain/draw_54_model.inc"
};
static NJS_CNK_OBJECT draw_59 = {
#include "stg28/terrain/draw_59.inc"
};
static Sint16 draw_48_polygons[57] = {
#include "stg28/terrain/draw_48_polygons.inc"
};
static Sint32 draw_48_vertices[35] = {
#include "stg28/terrain/draw_48_vertices.inc"
};
static NJS_CNK_MODEL draw_48_model = {
#include "stg28/terrain/draw_48_model.inc"
};
static NJS_CNK_OBJECT draw_53 = {
#include "stg28/terrain/draw_53.inc"
};
static Sint16 draw_46_polygons[715] = {
#include "stg28/terrain/draw_46_polygons.inc"
};
static Sint32 draw_46_vertices[475] = {
#include "stg28/terrain/draw_46_vertices.inc"
};
static NJS_CNK_MODEL draw_46_model = {
#include "stg28/terrain/draw_46_model.inc"
};
static NJS_CNK_OBJECT draw_47 = {
#include "stg28/terrain/draw_47.inc"
};
static Sint16 draw_45_polygons[889] = {
#include "stg28/terrain/draw_45_polygons.inc"
};
static Sint32 draw_45_vertices[607] = {
#include "stg28/terrain/draw_45_vertices.inc"
};
static NJS_CNK_MODEL draw_45_model = {
#include "stg28/terrain/draw_45_model.inc"
};
static NJS_CNK_OBJECT draw_45 = {
#include "stg28/terrain/draw_45.inc"
};
static Sint16 draw_44_polygons[739] = {
#include "stg28/terrain/draw_44_polygons.inc"
};
static Sint32 draw_44_vertices[535] = {
#include "stg28/terrain/draw_44_vertices.inc"
};
static NJS_CNK_MODEL draw_44_model = {
#include "stg28/terrain/draw_44_model.inc"
};
static NJS_CNK_OBJECT draw_44 = {
#include "stg28/terrain/draw_44.inc"
};
static Sint16 draw_43_polygons[121] = {
#include "stg28/terrain/draw_43_polygons.inc"
};
static Sint32 draw_43_vertices[67] = {
#include "stg28/terrain/draw_43_vertices.inc"
};
static NJS_CNK_MODEL draw_43_model = {
#include "stg28/terrain/draw_43_model.inc"
};
static NJS_CNK_OBJECT draw_43 = {
#include "stg28/terrain/draw_43.inc"
};
static Sint16 draw_42_polygons[451] = {
#include "stg28/terrain/draw_42_polygons.inc"
};
static Sint32 draw_42_vertices[315] = {
#include "stg28/terrain/draw_42_vertices.inc"
};
static NJS_CNK_MODEL draw_42_model = {
#include "stg28/terrain/draw_42_model.inc"
};
static NJS_CNK_OBJECT draw_42 = {
#include "stg28/terrain/draw_42.inc"
};
static Sint16 draw_41_polygons[1069] = {
#include "stg28/terrain/draw_41_polygons.inc"
};
static Sint32 draw_41_vertices[715] = {
#include "stg28/terrain/draw_41_vertices.inc"
};
static NJS_CNK_MODEL draw_41_model = {
#include "stg28/terrain/draw_41_model.inc"
};
static NJS_CNK_OBJECT draw_41 = {
#include "stg28/terrain/draw_41.inc"
};
static Sint16 draw_40_polygons[779] = {
#include "stg28/terrain/draw_40_polygons.inc"
};
static Sint32 draw_40_vertices[647] = {
#include "stg28/terrain/draw_40_vertices.inc"
};
static NJS_CNK_MODEL draw_40_model = {
#include "stg28/terrain/draw_40_model.inc"
};
static NJS_CNK_OBJECT draw_40 = {
#include "stg28/terrain/draw_40.inc"
};
static Sint16 draw_39_polygons[693] = {
#include "stg28/terrain/draw_39_polygons.inc"
};
static Sint32 draw_39_vertices[575] = {
#include "stg28/terrain/draw_39_vertices.inc"
};
static NJS_CNK_MODEL draw_39_model = {
#include "stg28/terrain/draw_39_model.inc"
};
static NJS_CNK_OBJECT draw_39 = {
#include "stg28/terrain/draw_39.inc"
};
static Sint16 draw_38_polygons[201] = {
#include "stg28/terrain/draw_38_polygons.inc"
};
static Sint32 draw_38_vertices[139] = {
#include "stg28/terrain/draw_38_vertices.inc"
};
static NJS_CNK_MODEL draw_38_model = {
#include "stg28/terrain/draw_38_model.inc"
};
static NJS_CNK_OBJECT draw_38 = {
#include "stg28/terrain/draw_38.inc"
};
static Sint16 draw_37_polygons[565] = {
#include "stg28/terrain/draw_37_polygons.inc"
};
static Sint32 draw_37_vertices[403] = {
#include "stg28/terrain/draw_37_vertices.inc"
};
static NJS_CNK_MODEL draw_37_model = {
#include "stg28/terrain/draw_37_model.inc"
};
static NJS_CNK_OBJECT draw_37 = {
#include "stg28/terrain/draw_37.inc"
};
static Sint16 draw_34_polygons[163] = {
#include "stg28/terrain/draw_34_polygons.inc"
};
static Sint32 draw_34_vertices[127] = {
#include "stg28/terrain/draw_34_vertices.inc"
};
static NJS_CNK_MODEL draw_34_model = {
#include "stg28/terrain/draw_34_model.inc"
};
static NJS_CNK_OBJECT draw_36 = {
#include "stg28/terrain/draw_36.inc"
};
static Sint16 draw_33_polygons[2395] = {
#include "stg28/terrain/draw_33_polygons.inc"
};
static Sint32 draw_33_vertices[2915] = {
#include "stg28/terrain/draw_33_vertices.inc"
};
static NJS_CNK_MODEL draw_33_model = {
#include "stg28/terrain/draw_33_model.inc"
};
static NJS_CNK_OBJECT draw_33 = {
#include "stg28/terrain/draw_33.inc"
};
static Sint16 draw_32_polygons[245] = {
#include "stg28/terrain/draw_32_polygons.inc"
};
static Sint32 draw_32_vertices[203] = {
#include "stg28/terrain/draw_32_vertices.inc"
};
static NJS_CNK_MODEL draw_32_model = {
#include "stg28/terrain/draw_32_model.inc"
};
static NJS_CNK_OBJECT draw_32 = {
#include "stg28/terrain/draw_32.inc"
};
static Sint16 draw_31_polygons[141] = {
#include "stg28/terrain/draw_31_polygons.inc"
};
static Sint32 draw_31_vertices[99] = {
#include "stg28/terrain/draw_31_vertices.inc"
};
static NJS_CNK_MODEL draw_31_model = {
#include "stg28/terrain/draw_31_model.inc"
};
static NJS_CNK_OBJECT draw_31 = {
#include "stg28/terrain/draw_31.inc"
};
static Sint16 draw_30_polygons[243] = {
#include "stg28/terrain/draw_30_polygons.inc"
};
static Sint32 draw_30_vertices[155] = {
#include "stg28/terrain/draw_30_vertices.inc"
};
static NJS_CNK_MODEL draw_30_model = {
#include "stg28/terrain/draw_30_model.inc"
};
static NJS_CNK_OBJECT draw_30 = {
#include "stg28/terrain/draw_30.inc"
};
static Sint16 draw_4_polygons[163] = {
#include "stg28/terrain/draw_4_polygons.inc"
};
static Sint32 draw_4_vertices[127] = {
#include "stg28/terrain/draw_4_vertices.inc"
};
static NJS_CNK_MODEL draw_4_model = {
#include "stg28/terrain/draw_4_model.inc"
};
static NJS_CNK_OBJECT draw_29 = {
#include "stg28/terrain/draw_29.inc"
};
static Sint16 draw_3_polygons[169] = {
#include "stg28/terrain/draw_3_polygons.inc"
};
static Sint32 draw_3_vertices[123] = {
#include "stg28/terrain/draw_3_vertices.inc"
};
static NJS_CNK_MODEL draw_3_model = {
#include "stg28/terrain/draw_3_model.inc"
};
static NJS_CNK_OBJECT draw_3 = {
#include "stg28/terrain/draw_3.inc"
};
static Sint16 draw_2_polygons[241] = {
#include "stg28/terrain/draw_2_polygons.inc"
};
static Sint32 draw_2_vertices[139] = {
#include "stg28/terrain/draw_2_vertices.inc"
};
static NJS_CNK_MODEL draw_2_model = {
#include "stg28/terrain/draw_2_model.inc"
};
static NJS_CNK_OBJECT draw_2 = {
#include "stg28/terrain/draw_2.inc"
};
static Sint16 draw_1_polygons[317] = {
#include "stg28/terrain/draw_1_polygons.inc"
};
static Sint32 draw_1_vertices[195] = {
#include "stg28/terrain/draw_1_vertices.inc"
};
static NJS_CNK_MODEL draw_1_model = {
#include "stg28/terrain/draw_1_model.inc"
};
static NJS_CNK_OBJECT draw_1 = {
#include "stg28/terrain/draw_1.inc"
};
static Sint16 draw_0_polygons[317] = {
#include "stg28/terrain/draw_0_polygons.inc"
};
static Sint32 draw_0_vertices[187] = {
#include "stg28/terrain/draw_0_vertices.inc"
};
static NJS_CNK_MODEL draw_0_model = {
#include "stg28/terrain/draw_0_model.inc"
};
static NJS_CNK_OBJECT draw_0 = {
#include "stg28/terrain/draw_0.inc"
};
static NJS_OBJECT collision_7 = {
#include "stg28/terrain/collision_7.inc"
};
static NJS_OBJECT collision_9 = {
#include "stg28/terrain/collision_9.inc"
};
static NJS_OBJECT collision_11 = {
#include "stg28/terrain/collision_11.inc"
};
static NJS_OBJECT collision_13 = {
#include "stg28/terrain/collision_13.inc"
};
static NJS_OBJECT collision_14 = {
#include "stg28/terrain/collision_14.inc"
};
static NJS_OBJECT collision_15 = {
#include "stg28/terrain/collision_15.inc"
};
static NJS_OBJECT collision_16 = {
#include "stg28/terrain/collision_16.inc"
};
static NJS_OBJECT collision_17 = {
#include "stg28/terrain/collision_17.inc"
};
static NJS_OBJECT collision_19 = {
#include "stg28/terrain/collision_19.inc"
};
static NJS_OBJECT collision_20 = {
#include "stg28/terrain/collision_20.inc"
};
static NJS_OBJECT collision_21 = {
#include "stg28/terrain/collision_21.inc"
};
static NJS_OBJECT collision_23 = {
#include "stg28/terrain/collision_23.inc"
};
static NJS_OBJECT collision_24 = {
#include "stg28/terrain/collision_24.inc"
};
static NJS_OBJECT collision_25 = {
#include "stg28/terrain/collision_25.inc"
};
static NJS_OBJECT collision_27 = {
#include "stg28/terrain/collision_27.inc"
};
static NJS_OBJECT collision_28 = {
#include "stg28/terrain/collision_28.inc"
};
static NJS_OBJECT collision_30 = {
#include "stg28/terrain/collision_30.inc"
};
static NJS_OBJECT collision_31 = {
#include "stg28/terrain/collision_31.inc"
};
static NJS_OBJECT collision_32 = {
#include "stg28/terrain/collision_32.inc"
};
static NJS_OBJECT collision_33 = {
#include "stg28/terrain/collision_33.inc"
};
static NJS_OBJECT collision_34 = {
#include "stg28/terrain/collision_34.inc"
};
static NJS_OBJECT collision_35 = {
#include "stg28/terrain/collision_35.inc"
};
static NJS_OBJECT collision_37 = {
#include "stg28/terrain/collision_37.inc"
};
static NJS_OBJECT collision_38 = {
#include "stg28/terrain/collision_38.inc"
};
static NJS_OBJECT collision_39 = {
#include "stg28/terrain/collision_39.inc"
};
static NJS_OBJECT collision_40 = {
#include "stg28/terrain/collision_40.inc"
};
static NJS_OBJECT collision_42 = {
#include "stg28/terrain/collision_42.inc"
};
static NJS_OBJECT collision_43 = {
#include "stg28/terrain/collision_43.inc"
};
static NJS_OBJECT collision_44 = {
#include "stg28/terrain/collision_44.inc"
};
static NJS_OBJECT collision_45 = {
#include "stg28/terrain/collision_45.inc"
};
static NJS_OBJECT collision_46 = {
#include "stg28/terrain/collision_46.inc"
};
static NJS_OBJECT collision_47 = {
#include "stg28/terrain/collision_47.inc"
};
static NJS_OBJECT collision_48 = {
#include "stg28/terrain/collision_48.inc"
};
static NJS_OBJECT collision_62 = {
#include "stg28/terrain/collision_62.inc"
};
static NJS_OBJECT collision_63 = {
#include "stg28/terrain/collision_63.inc"
};
static NJS_OBJECT collision_64 = {
#include "stg28/terrain/collision_64.inc"
};
static NJS_OBJECT collision_65 = {
#include "stg28/terrain/collision_65.inc"
};
static NJS_OBJECT collision_66 = {
#include "stg28/terrain/collision_66.inc"
};
static NJS_OBJECT collision_67 = {
#include "stg28/terrain/collision_67.inc"
};
static NJS_OBJECT collision_68 = {
#include "stg28/terrain/collision_68.inc"
};
static NJS_OBJECT collision_81 = {
#include "stg28/terrain/collision_81.inc"
};
static NJS_OBJECT collision_85 = {
#include "stg28/terrain/collision_85.inc"
};
static NJS_OBJECT collision_87 = {
#include "stg28/terrain/collision_87.inc"
};
static NJS_OBJECT collision_91 = {
#include "stg28/terrain/collision_91.inc"
};
static NJS_OBJECT collision_94 = {
#include "stg28/terrain/collision_94.inc"
};
static NJS_OBJECT collision_95 = {
#include "stg28/terrain/collision_95.inc"
};
static NJS_OBJECT collision_96 = {
#include "stg28/terrain/collision_96.inc"
};
static NJS_OBJECT collision_101 = {
#include "stg28/terrain/collision_101.inc"
};
static NJS_OBJECT collision_102 = {
#include "stg28/terrain/collision_102.inc"
};
static NJS_OBJECT collision_103 = {
#include "stg28/terrain/collision_103.inc"
};
static NJS_OBJECT collision_104 = {
#include "stg28/terrain/collision_104.inc"
};
static NJS_OBJECT collision_105 = {
#include "stg28/terrain/collision_105.inc"
};
static NJS_OBJECT collision_106 = {
#include "stg28/terrain/collision_106.inc"
};
static NJS_OBJECT collision_107 = {
#include "stg28/terrain/collision_107.inc"
};
static NJS_OBJECT collision_108 = {
#include "stg28/terrain/collision_108.inc"
};
static NJS_OBJECT collision_109 = {
#include "stg28/terrain/collision_109.inc"
};
static NJS_OBJECT collision_110 = {
#include "stg28/terrain/collision_110.inc"
};
static NJS_OBJECT collision_111 = {
#include "stg28/terrain/collision_111.inc"
};
static NJS_OBJECT collision_112 = {
#include "stg28/terrain/collision_112.inc"
};
static NJS_OBJECT collision_113 = {
#include "stg28/terrain/collision_113.inc"
};
static NJS_OBJECT collision_114 = {
#include "stg28/terrain/collision_114.inc"
};
static NJS_OBJECT collision_115 = {
#include "stg28/terrain/collision_115.inc"
};
static NJS_OBJECT collision_116 = {
#include "stg28/terrain/collision_116.inc"
};
static NJS_OBJECT collision_117 = {
#include "stg28/terrain/collision_117.inc"
};
static NJS_OBJECT collision_118 = {
#include "stg28/terrain/collision_118.inc"
};
static NJS_OBJECT collision_119 = {
#include "stg28/terrain/collision_119.inc"
};
static NJS_OBJECT collision_120 = {
#include "stg28/terrain/collision_120.inc"
};
static NJS_OBJECT collision_122 = {
#include "stg28/terrain/collision_122.inc"
};
static NJS_OBJECT collision_123 = {
#include "stg28/terrain/collision_123.inc"
};
static NJS_OBJECT collision_124 = {
#include "stg28/terrain/collision_124.inc"
};
static NJS_OBJECT collision_125 = {
#include "stg28/terrain/collision_125.inc"
};
static NJS_OBJECT collision_126 = {
#include "stg28/terrain/collision_126.inc"
};
static NJS_OBJECT collision_127 = {
#include "stg28/terrain/collision_127.inc"
};
static NJS_OBJECT collision_128 = {
#include "stg28/terrain/collision_128.inc"
};
static NJS_OBJECT collision_129 = {
#include "stg28/terrain/collision_129.inc"
};
static NJS_OBJECT collision_130 = {
#include "stg28/terrain/collision_130.inc"
};
static NJS_OBJECT collision_132 = {
#include "stg28/terrain/collision_132.inc"
};
static NJS_OBJECT collision_134 = {
#include "stg28/terrain/collision_134.inc"
};
static NJS_OBJECT collision_137 = {
#include "stg28/terrain/collision_137.inc"
};
static NJS_OBJECT collision_139 = {
#include "stg28/terrain/collision_139.inc"
};
static NJS_OBJECT collision_141 = {
#include "stg28/terrain/collision_141.inc"
};
static NJS_OBJECT collision_143 = {
#include "stg28/terrain/collision_143.inc"
};
static NJS_OBJECT collision_144 = {
#include "stg28/terrain/collision_144.inc"
};
static NJS_OBJECT collision_145 = {
#include "stg28/terrain/collision_145.inc"
};
static NJS_OBJECT collision_146 = {
#include "stg28/terrain/collision_146.inc"
};
static NJS_OBJECT collision_147 = {
#include "stg28/terrain/collision_147.inc"
};
static NJS_OBJECT collision_148 = {
#include "stg28/terrain/collision_148.inc"
};
static NJS_OBJECT collision_149 = {
#include "stg28/terrain/collision_149.inc"
};
static NJS_OBJECT collision_150 = {
#include "stg28/terrain/collision_150.inc"
};
static NJS_OBJECT collision_151 = {
#include "stg28/terrain/collision_151.inc"
};
static NJS_OBJECT collision_152 = {
#include "stg28/terrain/collision_152.inc"
};
static NJS_OBJECT collision_153 = {
#include "stg28/terrain/collision_153.inc"
};
static NJS_OBJECT collision_154 = {
#include "stg28/terrain/collision_154.inc"
};
static NJS_OBJECT collision_155 = {
#include "stg28/terrain/collision_155.inc"
};
static NJS_OBJECT collision_157 = {
#include "stg28/terrain/collision_157.inc"
};
static NJS_OBJECT collision_158 = {
#include "stg28/terrain/collision_158.inc"
};
static NJS_OBJECT collision_159 = {
#include "stg28/terrain/collision_159.inc"
};
static NJS_OBJECT collision_161 = {
#include "stg28/terrain/collision_161.inc"
};
static NJS_OBJECT collision_162 = {
#include "stg28/terrain/collision_162.inc"
};
static NJS_OBJECT collision_164 = {
#include "stg28/terrain/collision_164.inc"
};
static NJS_OBJECT collision_165 = {
#include "stg28/terrain/collision_165.inc"
};
static NJS_OBJECT collision_166 = {
#include "stg28/terrain/collision_166.inc"
};
static NJS_OBJECT collision_167 = {
#include "stg28/terrain/collision_167.inc"
};
static NJS_OBJECT collision_168 = {
#include "stg28/terrain/collision_168.inc"
};
static NJS_OBJECT collision_169 = {
#include "stg28/terrain/collision_169.inc"
};
static NJS_OBJECT collision_171 = {
#include "stg28/terrain/collision_171.inc"
};
static NJS_OBJECT collision_172 = {
#include "stg28/terrain/collision_172.inc"
};
static NJS_OBJECT collision_173 = {
#include "stg28/terrain/collision_173.inc"
};
static NJS_OBJECT collision_174 = {
#include "stg28/terrain/collision_174.inc"
};
static NJS_OBJECT collision_176 = {
#include "stg28/terrain/collision_176.inc"
};
static NJS_OBJECT collision_177 = {
#include "stg28/terrain/collision_177.inc"
};
static NJS_OBJECT collision_178 = {
#include "stg28/terrain/collision_178.inc"
};
static NJS_OBJECT collision_179 = {
#include "stg28/terrain/collision_179.inc"
};
static NJS_OBJECT collision_180 = {
#include "stg28/terrain/collision_180.inc"
};
static NJS_OBJECT collision_181 = {
#include "stg28/terrain/collision_181.inc"
};
static NJS_CNK_OBJECT draw_244 = {
#include "stg28/terrain/draw_244.inc"
};
static NJS_CNK_OBJECT draw_243 = {
#include "stg28/terrain/draw_243.inc"
};
static NJS_CNK_OBJECT draw_242 = {
#include "stg28/terrain/draw_242.inc"
};
static NJS_CNK_OBJECT draw_241 = {
#include "stg28/terrain/draw_241.inc"
};
static NJS_CNK_OBJECT draw_240 = {
#include "stg28/terrain/draw_240.inc"
};
static NJS_CNK_OBJECT draw_239 = {
#include "stg28/terrain/draw_239.inc"
};
static NJS_CNK_OBJECT draw_238 = {
#include "stg28/terrain/draw_238.inc"
};
static NJS_CNK_OBJECT draw_236 = {
#include "stg28/terrain/draw_236.inc"
};
static NJS_CNK_OBJECT draw_227 = {
#include "stg28/terrain/draw_227.inc"
};
static NJS_CNK_OBJECT draw_226 = {
#include "stg28/terrain/draw_226.inc"
};
static NJS_CNK_OBJECT draw_225 = {
#include "stg28/terrain/draw_225.inc"
};
static NJS_CNK_OBJECT draw_221 = {
#include "stg28/terrain/draw_221.inc"
};
static NJS_CNK_OBJECT draw_220 = {
#include "stg28/terrain/draw_220.inc"
};
static NJS_CNK_OBJECT draw_214 = {
#include "stg28/terrain/draw_214.inc"
};
static NJS_CNK_OBJECT draw_213 = {
#include "stg28/terrain/draw_213.inc"
};
static NJS_CNK_OBJECT draw_210 = {
#include "stg28/terrain/draw_210.inc"
};
static NJS_CNK_OBJECT draw_208 = {
#include "stg28/terrain/draw_208.inc"
};
static NJS_CNK_OBJECT draw_205 = {
#include "stg28/terrain/draw_205.inc"
};
static NJS_CNK_OBJECT draw_202 = {
#include "stg28/terrain/draw_202.inc"
};
static NJS_CNK_OBJECT draw_200 = {
#include "stg28/terrain/draw_200.inc"
};
static NJS_CNK_OBJECT draw_199 = {
#include "stg28/terrain/draw_199.inc"
};
static NJS_CNK_OBJECT draw_198 = {
#include "stg28/terrain/draw_198.inc"
};
static NJS_CNK_OBJECT draw_197 = {
#include "stg28/terrain/draw_197.inc"
};
static NJS_CNK_OBJECT draw_196 = {
#include "stg28/terrain/draw_196.inc"
};
static NJS_CNK_OBJECT draw_195 = {
#include "stg28/terrain/draw_195.inc"
};
static NJS_CNK_OBJECT draw_194 = {
#include "stg28/terrain/draw_194.inc"
};
static NJS_CNK_OBJECT draw_193 = {
#include "stg28/terrain/draw_193.inc"
};
static NJS_CNK_OBJECT draw_192 = {
#include "stg28/terrain/draw_192.inc"
};
static NJS_CNK_OBJECT draw_190 = {
#include "stg28/terrain/draw_190.inc"
};
static NJS_CNK_OBJECT draw_188 = {
#include "stg28/terrain/draw_188.inc"
};
static NJS_CNK_OBJECT draw_186 = {
#include "stg28/terrain/draw_186.inc"
};
static NJS_CNK_OBJECT draw_182 = {
#include "stg28/terrain/draw_182.inc"
};
static NJS_CNK_OBJECT draw_180 = {
#include "stg28/terrain/draw_180.inc"
};
static NJS_CNK_OBJECT draw_178 = {
#include "stg28/terrain/draw_178.inc"
};
static NJS_CNK_OBJECT draw_177 = {
#include "stg28/terrain/draw_177.inc"
};
static NJS_CNK_OBJECT draw_176 = {
#include "stg28/terrain/draw_176.inc"
};
static NJS_CNK_OBJECT draw_175 = {
#include "stg28/terrain/draw_175.inc"
};
static NJS_CNK_OBJECT draw_174 = {
#include "stg28/terrain/draw_174.inc"
};
static NJS_CNK_OBJECT draw_173 = {
#include "stg28/terrain/draw_173.inc"
};
static NJS_CNK_OBJECT draw_172 = {
#include "stg28/terrain/draw_172.inc"
};
static NJS_CNK_OBJECT draw_171 = {
#include "stg28/terrain/draw_171.inc"
};
static NJS_CNK_OBJECT draw_170 = {
#include "stg28/terrain/draw_170.inc"
};
static NJS_CNK_OBJECT draw_169 = {
#include "stg28/terrain/draw_169.inc"
};
static NJS_CNK_OBJECT draw_168 = {
#include "stg28/terrain/draw_168.inc"
};
static NJS_CNK_OBJECT draw_167 = {
#include "stg28/terrain/draw_167.inc"
};
static NJS_CNK_OBJECT draw_166 = {
#include "stg28/terrain/draw_166.inc"
};
static NJS_CNK_OBJECT draw_165 = {
#include "stg28/terrain/draw_165.inc"
};
static NJS_CNK_OBJECT draw_164 = {
#include "stg28/terrain/draw_164.inc"
};
static NJS_CNK_OBJECT draw_163 = {
#include "stg28/terrain/draw_163.inc"
};
static NJS_CNK_OBJECT draw_162 = {
#include "stg28/terrain/draw_162.inc"
};
static NJS_CNK_OBJECT draw_161 = {
#include "stg28/terrain/draw_161.inc"
};
static NJS_CNK_OBJECT draw_160 = {
#include "stg28/terrain/draw_160.inc"
};
static NJS_CNK_OBJECT draw_159 = {
#include "stg28/terrain/draw_159.inc"
};
static NJS_CNK_OBJECT draw_157 = {
#include "stg28/terrain/draw_157.inc"
};
static NJS_CNK_OBJECT draw_156 = {
#include "stg28/terrain/draw_156.inc"
};
static NJS_CNK_OBJECT draw_155 = {
#include "stg28/terrain/draw_155.inc"
};
static NJS_CNK_OBJECT draw_154 = {
#include "stg28/terrain/draw_154.inc"
};
static NJS_CNK_OBJECT draw_153 = {
#include "stg28/terrain/draw_153.inc"
};
static NJS_CNK_OBJECT draw_152 = {
#include "stg28/terrain/draw_152.inc"
};
static NJS_CNK_OBJECT draw_151 = {
#include "stg28/terrain/draw_151.inc"
};
static NJS_CNK_OBJECT draw_150 = {
#include "stg28/terrain/draw_150.inc"
};
static NJS_CNK_OBJECT draw_149 = {
#include "stg28/terrain/draw_149.inc"
};
static NJS_CNK_OBJECT draw_147 = {
#include "stg28/terrain/draw_147.inc"
};
static NJS_CNK_OBJECT draw_145 = {
#include "stg28/terrain/draw_145.inc"
};
static NJS_CNK_OBJECT draw_144 = {
#include "stg28/terrain/draw_144.inc"
};
static NJS_CNK_OBJECT draw_143 = {
#include "stg28/terrain/draw_143.inc"
};
static NJS_CNK_OBJECT draw_142 = {
#include "stg28/terrain/draw_142.inc"
};
static NJS_CNK_OBJECT draw_141 = {
#include "stg28/terrain/draw_141.inc"
};
static NJS_CNK_OBJECT draw_140 = {
#include "stg28/terrain/draw_140.inc"
};
static NJS_CNK_OBJECT draw_139 = {
#include "stg28/terrain/draw_139.inc"
};
static NJS_CNK_OBJECT draw_138 = {
#include "stg28/terrain/draw_138.inc"
};
static NJS_CNK_OBJECT draw_137 = {
#include "stg28/terrain/draw_137.inc"
};
static NJS_CNK_OBJECT draw_135 = {
#include "stg28/terrain/draw_135.inc"
};
static NJS_CNK_OBJECT draw_134 = {
#include "stg28/terrain/draw_134.inc"
};
static NJS_CNK_OBJECT draw_132 = {
#include "stg28/terrain/draw_132.inc"
};
static NJS_CNK_OBJECT draw_130 = {
#include "stg28/terrain/draw_130.inc"
};
static NJS_CNK_OBJECT draw_129 = {
#include "stg28/terrain/draw_129.inc"
};
static NJS_CNK_OBJECT draw_128 = {
#include "stg28/terrain/draw_128.inc"
};
static NJS_CNK_OBJECT draw_126 = {
#include "stg28/terrain/draw_126.inc"
};
static NJS_CNK_OBJECT draw_124 = {
#include "stg28/terrain/draw_124.inc"
};
static NJS_CNK_OBJECT draw_122 = {
#include "stg28/terrain/draw_122.inc"
};
static NJS_CNK_OBJECT draw_121 = {
#include "stg28/terrain/draw_121.inc"
};
static NJS_CNK_OBJECT draw_119 = {
#include "stg28/terrain/draw_119.inc"
};
static NJS_CNK_OBJECT draw_118 = {
#include "stg28/terrain/draw_118.inc"
};
static NJS_CNK_OBJECT draw_117 = {
#include "stg28/terrain/draw_117.inc"
};
static NJS_CNK_OBJECT draw_116 = {
#include "stg28/terrain/draw_116.inc"
};
static NJS_CNK_OBJECT draw_115 = {
#include "stg28/terrain/draw_115.inc"
};
static NJS_CNK_OBJECT draw_114 = {
#include "stg28/terrain/draw_114.inc"
};
static NJS_CNK_OBJECT draw_113 = {
#include "stg28/terrain/draw_113.inc"
};
static NJS_CNK_OBJECT draw_111 = {
#include "stg28/terrain/draw_111.inc"
};
static NJS_CNK_OBJECT draw_110 = {
#include "stg28/terrain/draw_110.inc"
};
static NJS_CNK_OBJECT draw_109 = {
#include "stg28/terrain/draw_109.inc"
};
static NJS_CNK_OBJECT draw_108 = {
#include "stg28/terrain/draw_108.inc"
};
static NJS_CNK_OBJECT draw_107 = {
#include "stg28/terrain/draw_107.inc"
};
static NJS_CNK_OBJECT draw_105 = {
#include "stg28/terrain/draw_105.inc"
};
static NJS_CNK_OBJECT draw_104 = {
#include "stg28/terrain/draw_104.inc"
};
static NJS_CNK_OBJECT draw_103 = {
#include "stg28/terrain/draw_103.inc"
};
static NJS_CNK_OBJECT draw_102 = {
#include "stg28/terrain/draw_102.inc"
};
static NJS_CNK_OBJECT draw_101 = {
#include "stg28/terrain/draw_101.inc"
};
static NJS_CNK_OBJECT draw_99 = {
#include "stg28/terrain/draw_99.inc"
};
static NJS_CNK_OBJECT draw_98 = {
#include "stg28/terrain/draw_98.inc"
};
static NJS_CNK_OBJECT draw_97 = {
#include "stg28/terrain/draw_97.inc"
};
static NJS_CNK_OBJECT draw_96 = {
#include "stg28/terrain/draw_96.inc"
};
static NJS_CNK_OBJECT draw_95 = {
#include "stg28/terrain/draw_95.inc"
};
static NJS_CNK_OBJECT draw_94 = {
#include "stg28/terrain/draw_94.inc"
};
static NJS_CNK_OBJECT draw_93 = {
#include "stg28/terrain/draw_93.inc"
};
static NJS_CNK_OBJECT draw_92 = {
#include "stg28/terrain/draw_92.inc"
};
static NJS_CNK_OBJECT draw_91 = {
#include "stg28/terrain/draw_91.inc"
};
static NJS_CNK_OBJECT draw_90 = {
#include "stg28/terrain/draw_90.inc"
};
static NJS_CNK_OBJECT draw_89 = {
#include "stg28/terrain/draw_89.inc"
};
static NJS_CNK_OBJECT draw_86 = {
#include "stg28/terrain/draw_86.inc"
};
static NJS_CNK_OBJECT draw_85 = {
#include "stg28/terrain/draw_85.inc"
};
static NJS_CNK_OBJECT draw_84 = {
#include "stg28/terrain/draw_84.inc"
};
static NJS_CNK_OBJECT draw_83 = {
#include "stg28/terrain/draw_83.inc"
};
static NJS_CNK_OBJECT draw_82 = {
#include "stg28/terrain/draw_82.inc"
};
static NJS_CNK_OBJECT draw_81 = {
#include "stg28/terrain/draw_81.inc"
};
static NJS_CNK_OBJECT draw_80 = {
#include "stg28/terrain/draw_80.inc"
};
static NJS_CNK_OBJECT draw_78 = {
#include "stg28/terrain/draw_78.inc"
};
static NJS_CNK_OBJECT draw_77 = {
#include "stg28/terrain/draw_77.inc"
};
static NJS_CNK_OBJECT draw_76 = {
#include "stg28/terrain/draw_76.inc"
};
static NJS_CNK_OBJECT draw_75 = {
#include "stg28/terrain/draw_75.inc"
};
static NJS_CNK_OBJECT draw_74 = {
#include "stg28/terrain/draw_74.inc"
};
static NJS_CNK_OBJECT draw_73 = {
#include "stg28/terrain/draw_73.inc"
};
static NJS_CNK_OBJECT draw_71 = {
#include "stg28/terrain/draw_71.inc"
};
static NJS_CNK_OBJECT draw_70 = {
#include "stg28/terrain/draw_70.inc"
};
static NJS_CNK_OBJECT draw_69 = {
#include "stg28/terrain/draw_69.inc"
};
static NJS_CNK_OBJECT draw_68 = {
#include "stg28/terrain/draw_68.inc"
};
static NJS_CNK_OBJECT draw_66 = {
#include "stg28/terrain/draw_66.inc"
};
static NJS_CNK_OBJECT draw_65 = {
#include "stg28/terrain/draw_65.inc"
};
static NJS_CNK_OBJECT draw_64 = {
#include "stg28/terrain/draw_64.inc"
};
static NJS_CNK_OBJECT draw_63 = {
#include "stg28/terrain/draw_63.inc"
};
static NJS_CNK_OBJECT draw_58 = {
#include "stg28/terrain/draw_58.inc"
};
static NJS_CNK_OBJECT draw_57 = {
#include "stg28/terrain/draw_57.inc"
};
static NJS_CNK_OBJECT draw_56 = {
#include "stg28/terrain/draw_56.inc"
};
static NJS_CNK_OBJECT draw_55 = {
#include "stg28/terrain/draw_55.inc"
};
static NJS_CNK_OBJECT draw_54 = {
#include "stg28/terrain/draw_54.inc"
};
static NJS_CNK_OBJECT draw_52 = {
#include "stg28/terrain/draw_52.inc"
};
static NJS_CNK_OBJECT draw_51 = {
#include "stg28/terrain/draw_51.inc"
};
static NJS_CNK_OBJECT draw_50 = {
#include "stg28/terrain/draw_50.inc"
};
static NJS_CNK_OBJECT draw_49 = {
#include "stg28/terrain/draw_49.inc"
};
static NJS_CNK_OBJECT draw_48 = {
#include "stg28/terrain/draw_48.inc"
};
static NJS_CNK_OBJECT draw_46 = {
#include "stg28/terrain/draw_46.inc"
};
static NJS_CNK_OBJECT draw_35 = {
#include "stg28/terrain/draw_35.inc"
};
static NJS_CNK_OBJECT draw_34 = {
#include "stg28/terrain/draw_34.inc"
};
static NJS_CNK_OBJECT draw_28 = {
#include "stg28/terrain/draw_28.inc"
};
static NJS_CNK_OBJECT draw_27 = {
#include "stg28/terrain/draw_27.inc"
};
static NJS_CNK_OBJECT draw_26 = {
#include "stg28/terrain/draw_26.inc"
};
static NJS_CNK_OBJECT draw_25 = {
#include "stg28/terrain/draw_25.inc"
};
static NJS_CNK_OBJECT draw_24 = {
#include "stg28/terrain/draw_24.inc"
};
static NJS_CNK_OBJECT draw_23 = {
#include "stg28/terrain/draw_23.inc"
};
static NJS_CNK_OBJECT draw_22 = {
#include "stg28/terrain/draw_22.inc"
};
static NJS_CNK_OBJECT draw_21 = {
#include "stg28/terrain/draw_21.inc"
};
static NJS_CNK_OBJECT draw_20 = {
#include "stg28/terrain/draw_20.inc"
};
static NJS_CNK_OBJECT draw_19 = {
#include "stg28/terrain/draw_19.inc"
};
static NJS_CNK_OBJECT draw_18 = {
#include "stg28/terrain/draw_18.inc"
};
static NJS_CNK_OBJECT draw_17 = {
#include "stg28/terrain/draw_17.inc"
};
static NJS_CNK_OBJECT draw_16 = {
#include "stg28/terrain/draw_16.inc"
};
static NJS_CNK_OBJECT draw_15 = {
#include "stg28/terrain/draw_15.inc"
};
static NJS_CNK_OBJECT draw_14 = {
#include "stg28/terrain/draw_14.inc"
};
static NJS_CNK_OBJECT draw_13 = {
#include "stg28/terrain/draw_13.inc"
};
static NJS_CNK_OBJECT draw_12 = {
#include "stg28/terrain/draw_12.inc"
};
static NJS_CNK_OBJECT draw_11 = {
#include "stg28/terrain/draw_11.inc"
};
static NJS_CNK_OBJECT draw_10 = {
#include "stg28/terrain/draw_10.inc"
};
static NJS_CNK_OBJECT draw_9 = {
#include "stg28/terrain/draw_9.inc"
};
static NJS_CNK_OBJECT draw_8 = {
#include "stg28/terrain/draw_8.inc"
};
static NJS_CNK_OBJECT draw_7 = {
#include "stg28/terrain/draw_7.inc"
};
static NJS_CNK_OBJECT draw_6 = {
#include "stg28/terrain/draw_6.inc"
};
static NJS_CNK_OBJECT draw_5 = {
#include "stg28/terrain/draw_5.inc"
};
static NJS_CNK_OBJECT draw_4 = {
#include "stg28/terrain/draw_4.inc"
};
