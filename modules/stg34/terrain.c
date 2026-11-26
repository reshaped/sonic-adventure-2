#include <ninja.h>
#include <sg_xpt.h>

#include "collision/world.h"
#include "player/player.h"
#include "shared/motions.h"
#include "stg34/core_sonic.h"

/* the land's file names, which its land table and texture list point at. each
 * starts on a 4-byte boundary */
const char core_sonic_land_filenames[690] = {
#include "stg34/terrain/core_sonic_land_filenames.inc"
};

/* STG34.PRS's motions, their registry, land, texture lists, Basic models and
 * chunk models, from the disc. its D lies between core_sonic.c and
 * object_archives.c */

/* the data up to the last that a pointer names ahead of its definition, as
 * tentative definitions in the disc's order: GCC places each datum at its
 * first definition */
static NJS_MKEY_F rail_hang_motion_joint_0_position[2];
static NJS_MKEY_F rail_hang_motion_joint_2_position[16];
static NJS_MKEY_A rail_hang_motion_joint_4_angles[16];
static NJS_MKEY_A rail_hang_motion_joint_7_angles[16];
static NJS_MKEY_A rail_hang_motion_joint_8_angles[16];
static NJS_MKEY_A rail_hang_motion_joint_9_angles[16];
static NJS_MKEY_A rail_hang_motion_joint_13_angles[16];
static NJS_MKEY_A rail_hang_motion_joint_14_angles[2];
static NJS_MKEY_A rail_hang_motion_joint_17_angles[16];
static NJS_MKEY_A rail_hang_motion_joint_18_angles[10];
static NJS_MKEY_A rail_hang_motion_joint_19_angles[16];
static NJS_MKEY_A rail_hang_motion_joint_23_angles[16];
static NJS_MKEY_A rail_hang_motion_joint_24_angles[2];
static NJS_MKEY_A rail_hang_motion_joint_27_angles[16];
static NJS_MKEY_A rail_hang_motion_joint_31_angles[16];
static NJS_MKEY_A rail_hang_motion_joint_32_angles[12];
static NJS_MKEY_A rail_hang_motion_joint_33_angles[12];
static NJS_MKEY_A rail_hang_motion_joint_37_angles[12];
static NJS_MKEY_A rail_hang_motion_joint_38_angles[12];
static NJS_MKEY_A rail_hang_motion_joint_39_angles[12];
static NJS_MKEY_A rail_hang_motion_joint_41_angles[2];
static NJS_MKEY_A rail_hang_motion_joint_44_angles[12];
static NJS_MKEY_A rail_hang_motion_joint_51_angles[16];
static NJS_MKEY_A rail_hang_motion_joint_53_angles[16];
static NJS_MKEY_A rail_hang_motion_joint_56_angles[12];
static NJS_MKEY_A rail_hang_motion_joint_57_angles[2];
static NJS_MKEY_A rail_hang_motion_joint_58_angles[2];
static NJS_MKEY_A rail_hang_motion_joint_60_angles[12];
static NJS_MDATA2 rail_hang_motion_tracks[62];
static NJS_MOTION rail_hang_motion;
static NJS_MKEY_F player_lift_motion_joint_2_position[32];
static NJS_MKEY_A player_lift_motion_joint_4_angles[32];
static NJS_MKEY_A player_lift_motion_joint_7_angles[32];
static NJS_MKEY_A player_lift_motion_joint_8_angles[32];
static NJS_MKEY_A player_lift_motion_joint_9_angles[32];
static NJS_MKEY_A player_lift_motion_joint_13_angles[32];
static NJS_MKEY_A player_lift_motion_joint_14_angles[32];
static NJS_MKEY_A player_lift_motion_joint_17_angles[32];
static NJS_MKEY_A player_lift_motion_joint_18_angles[32];
static NJS_MKEY_A player_lift_motion_joint_19_angles[32];
static NJS_MKEY_A player_lift_motion_joint_23_angles[32];
static NJS_MKEY_A player_lift_motion_joint_24_angles[32];
static NJS_MKEY_A player_lift_motion_joint_27_angles[32];
static NJS_MKEY_A player_lift_motion_joint_28_angles[2];
static NJS_MKEY_A player_lift_motion_joint_29_angles[2];
static NJS_MKEY_A player_lift_motion_joint_31_angles[32];
static NJS_MKEY_A player_lift_motion_joint_32_angles[32];
static NJS_MKEY_A player_lift_motion_joint_33_angles[32];
static NJS_MKEY_A player_lift_motion_joint_37_angles[32];
static NJS_MKEY_A player_lift_motion_joint_38_angles[32];
static NJS_MKEY_A player_lift_motion_joint_39_angles[32];
static NJS_MKEY_A player_lift_motion_joint_41_angles[32];
static NJS_MKEY_A player_lift_motion_joint_44_angles[32];
static NJS_MKEY_A player_lift_motion_joint_51_angles[32];
static NJS_MKEY_A player_lift_motion_joint_53_angles[32];
static NJS_MKEY_A player_lift_motion_joint_56_angles[32];
static NJS_MKEY_A player_lift_motion_joint_57_angles[32];
static NJS_MKEY_A player_lift_motion_joint_58_angles[32];
static NJS_MKEY_A player_lift_motion_joint_60_angles[32];
static NJS_MDATA2 player_lift_motion_tracks[62];
static NJS_MOTION player_lift_motion;
struct motion_entry module_motions[3];
static struct land_entry entries[282];
struct land_table core_sonic_land;
static NJS_TEXNAME textures[43];
NJS_TEXLIST core_sonic_land_texlist;
static NJS_MATERIAL collision_0_materials[1];
static Sint16 collision_0_faces[81];
static NJS_MESHSET collision_0_meshsets[1];
static NJS_POINT3 collision_0_points[37];
static NJS_MODEL collision_0_model;
static NJS_OBJECT collision_0;
static NJS_MATERIAL collision_1_materials[1];
static Sint16 collision_1_faces[19];
static NJS_MESHSET collision_1_meshsets[1];
static NJS_POINT3 collision_1_points[8];
static NJS_MODEL collision_1_model;
static NJS_OBJECT collision_1;
static NJS_MATERIAL collision_2_materials[1];
static Sint16 collision_2_faces[50];
static NJS_MESHSET collision_2_meshsets[1];
static NJS_POINT3 collision_2_points[22];
static NJS_MODEL collision_2_model;
static NJS_OBJECT collision_2;
static NJS_MATERIAL collision_3_materials[1];
static Sint16 collision_3_faces[19];
static NJS_MESHSET collision_3_meshsets[1];
static NJS_POINT3 collision_3_points[18];
static NJS_MODEL collision_3_model;
static NJS_OBJECT collision_3;
static NJS_MATERIAL collision_4_materials[1];
static Sint16 collision_4_faces[136];
static NJS_MESHSET collision_4_meshsets[1];
static NJS_POINT3 collision_4_points[62];
static NJS_MODEL collision_4_model;
static NJS_OBJECT collision_4;
static NJS_MATERIAL collision_5_materials[1];
static Sint16 collision_5_faces[47];
static NJS_MESHSET collision_5_meshsets[1];
static NJS_POINT3 collision_5_points[23];
static NJS_MODEL collision_5_model;
static NJS_OBJECT collision_5;
static NJS_MATERIAL collision_6_materials[1];
static Sint16 collision_6_faces[47];
static NJS_MESHSET collision_6_meshsets[1];
static NJS_POINT3 collision_6_points[23];
static NJS_MODEL collision_6_model;
static NJS_OBJECT collision_6;
static NJS_MATERIAL collision_7_materials[1];
static Sint16 collision_7_faces[45];
static NJS_MESHSET collision_7_meshsets[1];
static NJS_POINT3 collision_7_points[20];
static NJS_MODEL collision_7_model;
static NJS_OBJECT collision_7;
static NJS_MATERIAL collision_8_materials[1];
static Sint16 collision_8_faces[17];
static NJS_MESHSET collision_8_meshsets[1];
static NJS_POINT3 collision_8_points[12];
static NJS_MODEL collision_8_model;
static NJS_OBJECT collision_8;
static NJS_MATERIAL collision_12_materials[1];
static Sint16 collision_12_faces[48];
static NJS_MESHSET collision_12_meshsets[1];
static NJS_POINT3 collision_12_points[24];
static NJS_MODEL collision_12_model;
static NJS_OBJECT collision_12;
static NJS_MATERIAL collision_13_materials[1];
static Sint16 collision_13_faces[48];
static NJS_MESHSET collision_13_meshsets[1];
static NJS_POINT3 collision_13_points[26];
static NJS_MODEL collision_13_model;
static NJS_OBJECT collision_13;
static NJS_MATERIAL collision_14_materials[1];
static Sint16 collision_14_faces[17];
static NJS_MESHSET collision_14_meshsets[1];
static NJS_POINT3 collision_14_points[12];
static NJS_MODEL collision_14_model;
static NJS_OBJECT collision_14;
static NJS_MATERIAL collision_16_materials[1];
static Sint16 collision_16_faces[9];
static NJS_MESHSET collision_16_meshsets[1];
static NJS_POINT3 collision_16_points[8];
static NJS_MODEL collision_16_model;
static NJS_OBJECT collision_16;
static NJS_MATERIAL collision_17_materials[1];
static Sint16 collision_17_faces[5];
static NJS_MESHSET collision_17_meshsets[1];
static NJS_POINT3 collision_17_points[4];
static NJS_MODEL collision_17_model;
static NJS_OBJECT collision_17;
static NJS_MATERIAL collision_18_materials[1];
static Sint16 collision_18_faces[197];
static NJS_MESHSET collision_18_meshsets[1];
static NJS_POINT3 collision_18_points[80];
static NJS_MODEL collision_18_model;
static NJS_OBJECT collision_18;
static NJS_MATERIAL collision_19_materials[1];
static Sint16 collision_19_faces[104];
static NJS_MESHSET collision_19_meshsets[1];
static NJS_POINT3 collision_19_points[60];
static NJS_MODEL collision_19_model;
static NJS_OBJECT collision_19;
static NJS_MATERIAL collision_20_materials[1];
static Sint16 collision_20_faces[44];
static NJS_MESHSET collision_20_meshsets[1];
static NJS_POINT3 collision_20_points[24];
static NJS_MODEL collision_20_model;
static NJS_OBJECT collision_20;
static NJS_MATERIAL collision_23_materials[1];
static Sint16 collision_23_faces[107];
static NJS_MESHSET collision_23_meshsets[1];
static NJS_POINT3 collision_23_points[64];
static NJS_MODEL collision_23_model;
static NJS_OBJECT collision_23;
static NJS_MATERIAL collision_24_materials[1];
static Sint16 collision_24_faces[32];
static NJS_MESHSET collision_24_meshsets[1];
static NJS_POINT3 collision_24_points[16];
static NJS_MODEL collision_24_model;
static NJS_OBJECT collision_24;
static NJS_MATERIAL collision_25_materials[1];
static Sint16 collision_25_faces[33];
static NJS_MESHSET collision_25_meshsets[1];
static NJS_POINT3 collision_25_points[17];
static NJS_MODEL collision_25_model;
static NJS_OBJECT collision_25;
static NJS_MATERIAL collision_27_materials[1];
static Sint16 collision_27_faces[19];
static NJS_MESHSET collision_27_meshsets[1];
static NJS_POINT3 collision_27_points[10];
static NJS_MODEL collision_27_model;
static NJS_OBJECT collision_27;
static NJS_MATERIAL collision_28_materials[1];
static Sint16 collision_28_faces[96];
static NJS_MESHSET collision_28_meshsets[1];
static NJS_POINT3 collision_28_points[42];
static NJS_MODEL collision_28_model;
static NJS_OBJECT collision_28;
static NJS_MATERIAL collision_29_materials[1];
static Sint16 collision_29_faces[74];
static NJS_MESHSET collision_29_meshsets[1];
static NJS_POINT3 collision_29_points[28];
static NJS_MODEL collision_29_model;
static NJS_OBJECT collision_29;
static NJS_MATERIAL collision_30_materials[1];
static Sint16 collision_30_faces[19];
static NJS_MESHSET collision_30_meshsets[1];
static NJS_POINT3 collision_30_points[10];
static NJS_MODEL collision_30_model;
static NJS_OBJECT collision_30;
static NJS_MATERIAL collision_32_materials[1];
static Sint16 collision_32_faces[15];
static NJS_MESHSET collision_32_meshsets[1];
static NJS_POINT3 collision_32_points[11];
static NJS_MODEL collision_32_model;
static NJS_OBJECT collision_32;
static NJS_MATERIAL collision_34_materials[1];
static Sint16 collision_34_faces[16];
static NJS_MESHSET collision_34_meshsets[1];
static NJS_POINT3 collision_34_points[9];
static NJS_MODEL collision_34_model;
static NJS_OBJECT collision_34;
static NJS_MATERIAL collision_35_materials[1];
static Sint16 collision_35_faces[113];
static NJS_MESHSET collision_35_meshsets[1];
static NJS_POINT3 collision_35_points[45];
static NJS_MODEL collision_35_model;
static NJS_OBJECT collision_35;
static NJS_MATERIAL collision_36_materials[1];
static Sint16 collision_36_faces[178];
static NJS_MESHSET collision_36_meshsets[1];
static NJS_POINT3 collision_36_points[65];
static NJS_MODEL collision_36_model;
static NJS_OBJECT collision_36;
static NJS_MATERIAL collision_37_materials[1];
static Sint16 collision_37_faces[28];
static NJS_MESHSET collision_37_meshsets[1];
static NJS_POINT3 collision_37_points[12];
static NJS_MODEL collision_37_model;
static NJS_OBJECT collision_37;
static NJS_MATERIAL collision_38_materials[1];
static Sint16 collision_38_faces[26];
static NJS_MESHSET collision_38_meshsets[1];
static NJS_POINT3 collision_38_points[12];
static NJS_MODEL collision_38_model;
static NJS_OBJECT collision_38;
static NJS_MATERIAL collision_39_materials[1];
static Sint16 collision_39_faces[168];
static NJS_MESHSET collision_39_meshsets[1];
static NJS_POINT3 collision_39_points[45];
static NJS_MODEL collision_39_model;
static NJS_OBJECT collision_39;
static NJS_MATERIAL collision_40_materials[1];
static Sint16 collision_40_faces[157];
static NJS_MESHSET collision_40_meshsets[1];
static NJS_POINT3 collision_40_points[48];
static NJS_MODEL collision_40_model;
static NJS_OBJECT collision_40;
static NJS_MATERIAL collision_41_materials[1];
static Sint16 collision_41_faces[39];
static NJS_MESHSET collision_41_meshsets[1];
static NJS_POINT3 collision_41_points[18];
static NJS_MODEL collision_41_model;
static NJS_OBJECT collision_41;
static NJS_MATERIAL collision_43_materials[1];
static Sint16 collision_43_faces[125];
static NJS_MESHSET collision_43_meshsets[1];
static NJS_POINT3 collision_43_points[53];
static NJS_MODEL collision_43_model;
static NJS_OBJECT collision_43;
static NJS_MATERIAL collision_44_materials[1];
static Sint16 collision_44_faces[109];
static NJS_MESHSET collision_44_meshsets[1];
static NJS_POINT3 collision_44_points[46];
static NJS_MODEL collision_44_model;
static NJS_OBJECT collision_44;
static NJS_MATERIAL collision_45_materials[1];
static Sint16 collision_45_faces[173];
static NJS_MESHSET collision_45_meshsets[1];
static NJS_POINT3 collision_45_points[74];
static NJS_MODEL collision_45_model;
static NJS_OBJECT collision_45;
static NJS_MATERIAL collision_46_materials[1];
static Sint16 collision_46_faces[10];
static NJS_MESHSET collision_46_meshsets[1];
static NJS_POINT3 collision_46_points[5];
static NJS_MODEL collision_46_model;
static NJS_OBJECT collision_46;
static NJS_MATERIAL collision_50_materials[1];
static Sint16 collision_50_faces[47];
static NJS_MESHSET collision_50_meshsets[1];
static NJS_POINT3 collision_50_points[18];
static NJS_MODEL collision_50_model;
static NJS_OBJECT collision_50;
static NJS_MATERIAL collision_51_materials[1];
static Sint16 collision_51_faces[5];
static NJS_MESHSET collision_51_meshsets[1];
static NJS_POINT3 collision_51_points[4];
static NJS_MODEL collision_51_model;
static NJS_OBJECT collision_51;
static NJS_MATERIAL collision_52_materials[1];
static Sint16 collision_52_faces[10];
static NJS_MESHSET collision_52_meshsets[1];
static NJS_POINT3 collision_52_points[5];
static NJS_MODEL collision_52_model;
static NJS_OBJECT collision_52;
static NJS_MATERIAL collision_54_materials[1];
static Sint16 collision_54_faces[110];
static NJS_MESHSET collision_54_meshsets[1];
static NJS_POINT3 collision_54_points[40];
static NJS_MODEL collision_54_model;
static NJS_OBJECT collision_54;
static NJS_MATERIAL collision_62_materials[1];
static Sint16 collision_62_faces[71];
static NJS_MESHSET collision_62_meshsets[1];
static NJS_POINT3 collision_62_points[28];
static NJS_MODEL collision_62_model;
static NJS_OBJECT collision_62;
static NJS_MATERIAL collision_70_materials[1];
static Sint16 collision_70_faces[167];
static NJS_MESHSET collision_70_meshsets[1];
static NJS_POINT3 collision_70_points[70];
static NJS_MODEL collision_70_model;
static NJS_OBJECT collision_70;
static NJS_MATERIAL collision_74_materials[1];
static Sint16 collision_74_faces[164];
static NJS_MESHSET collision_74_meshsets[1];
static NJS_POINT3 collision_74_points[70];
static NJS_MODEL collision_74_model;
static NJS_OBJECT collision_74;
static NJS_MATERIAL collision_77_materials[1];
static Sint16 collision_77_faces[72];
static NJS_MESHSET collision_77_meshsets[1];
static NJS_POINT3 collision_77_points[24];
static NJS_MODEL collision_77_model;
static NJS_OBJECT collision_77;
static NJS_MATERIAL collision_80_materials[1];
static Sint16 collision_80_faces[172];
static NJS_MESHSET collision_80_meshsets[1];
static NJS_POINT3 collision_80_points[61];
static NJS_MODEL collision_80_model;
static NJS_OBJECT collision_80;
static NJS_MATERIAL collision_81_materials[1];
static Sint16 collision_81_faces[13];
static NJS_MESHSET collision_81_meshsets[1];
static NJS_POINT3 collision_81_points[12];
static NJS_MODEL collision_81_model;
static NJS_OBJECT collision_81;
static NJS_MATERIAL collision_82_materials[1];
static Sint16 collision_82_faces[75];
static NJS_MESHSET collision_82_meshsets[1];
static NJS_POINT3 collision_82_points[32];
static NJS_MODEL collision_82_model;
static NJS_OBJECT collision_82;
static NJS_MATERIAL collision_83_materials[1];
static Sint16 collision_83_faces[38];
static NJS_MESHSET collision_83_meshsets[1];
static NJS_POINT3 collision_83_points[24];
static NJS_MODEL collision_83_model;
static NJS_OBJECT collision_83;
static NJS_MATERIAL collision_88_materials[1];
static Sint16 collision_88_faces[72];
static NJS_MESHSET collision_88_meshsets[1];
static NJS_POINT3 collision_88_points[32];
static NJS_MODEL collision_88_model;
static NJS_OBJECT collision_88;
static NJS_MATERIAL collision_90_materials[1];
static Sint16 collision_90_faces[69];
static NJS_MESHSET collision_90_meshsets[1];
static NJS_POINT3 collision_90_points[32];
static NJS_MODEL collision_90_model;
static NJS_OBJECT collision_90;
static NJS_MATERIAL collision_91_materials[1];
static Sint16 collision_91_faces[21];
static NJS_MESHSET collision_91_meshsets[1];
static NJS_POINT3 collision_91_points[12];
static NJS_MODEL collision_91_model;
static NJS_OBJECT collision_91;
static NJS_MATERIAL collision_92_materials[1];
static Sint16 collision_92_faces[42];
static NJS_MESHSET collision_92_meshsets[1];
static NJS_POINT3 collision_92_points[21];
static NJS_MODEL collision_92_model;
static NJS_OBJECT collision_92;
static NJS_MATERIAL collision_93_materials[1];
static Sint16 collision_93_faces[31];
static NJS_MESHSET collision_93_meshsets[1];
static NJS_POINT3 collision_93_points[15];
static NJS_MODEL collision_93_model;
static NJS_OBJECT collision_93;
static NJS_MATERIAL collision_94_materials[1];
static Sint16 collision_94_faces[13];
static NJS_MESHSET collision_94_meshsets[1];
static NJS_POINT3 collision_94_points[12];
static NJS_MODEL collision_94_model;
static NJS_OBJECT collision_94;
static NJS_MATERIAL collision_102_materials[1];
static Sint16 collision_102_faces[9];
static NJS_MESHSET collision_102_meshsets[1];
static NJS_POINT3 collision_102_points[8];
static NJS_MODEL collision_102_model;
static NJS_OBJECT collision_102;
static NJS_MATERIAL collision_110_materials[1];
static Sint16 collision_110_faces[35];
static NJS_MESHSET collision_110_meshsets[1];
static NJS_POINT3 collision_110_points[22];
static NJS_MODEL collision_110_model;
static NJS_OBJECT collision_110;
static NJS_MATERIAL collision_114_materials[1];
static Sint16 collision_114_faces[23];
static NJS_MESHSET collision_114_meshsets[1];
static NJS_POINT3 collision_114_points[22];
static NJS_MODEL collision_114_model;
static NJS_OBJECT collision_114;
static NJS_MATERIAL collision_117_materials[1];
static Sint16 collision_117_faces[7];
static NJS_MESHSET collision_117_meshsets[1];
static NJS_POINT3 collision_117_points[6];
static NJS_MODEL collision_117_model;
static NJS_OBJECT collision_117;
static NJS_MATERIAL collision_120_materials[1];
static Sint16 collision_120_faces[13];
static NJS_MESHSET collision_120_meshsets[1];
static NJS_POINT3 collision_120_points[12];
static NJS_MODEL collision_120_model;
static NJS_OBJECT collision_120;
static Sint16 draw_160_polygons[477];
static Sint32 draw_160_vertices[371];
static NJS_CNK_MODEL draw_160_model;
static NJS_CNK_OBJECT draw_160;
static Sint16 draw_159_polygons[671];
static Sint32 draw_159_vertices[403];
static NJS_CNK_MODEL draw_159_model;
static NJS_CNK_OBJECT draw_159;
static Sint16 draw_158_polygons[671];
static Sint32 draw_158_vertices[403];
static NJS_CNK_MODEL draw_158_model;
static NJS_CNK_OBJECT draw_158;
static Sint16 draw_157_polygons[67];
static Sint32 draw_157_vertices[67];
static NJS_CNK_MODEL draw_157_model;
static NJS_CNK_OBJECT draw_157;
static Sint16 draw_153_polygons[1083];
static Sint32 draw_153_vertices[1135];
static NJS_CNK_MODEL draw_153_model;
static NJS_CNK_OBJECT draw_155;
static Sint16 draw_151_polygons[773];
static Sint32 draw_151_vertices[579];
static NJS_CNK_MODEL draw_151_model;
static NJS_CNK_OBJECT draw_152;
static Sint16 draw_147_polygons[153];
static Sint32 draw_147_vertices[91];
static NJS_CNK_MODEL draw_147_model;
static NJS_CNK_OBJECT draw_149;
static Sint16 draw_146_polygons[147];
static Sint32 draw_146_vertices[127];
static NJS_CNK_MODEL draw_146_model;
static NJS_CNK_OBJECT draw_146;
static Sint16 draw_142_polygons[535];
static Sint32 draw_142_vertices[543];
static NJS_CNK_MODEL draw_142_model;
static NJS_CNK_OBJECT draw_145;
static Sint16 draw_141_polygons[217];
static Sint32 draw_141_vertices[103];
static NJS_CNK_MODEL draw_141_model;
static NJS_CNK_OBJECT draw_141;
static Sint16 draw_140_polygons[369];
static Sint32 draw_140_vertices[263];
static NJS_CNK_MODEL draw_140_model;
static NJS_CNK_OBJECT draw_140;
static Sint16 draw_138_polygons[115];
static Sint32 draw_138_vertices[111];
static NJS_CNK_MODEL draw_138_model;
static NJS_CNK_OBJECT draw_138;
static Sint16 draw_136_polygons[1063];
static Sint32 draw_136_vertices[1143];
static NJS_CNK_MODEL draw_136_model;
static NJS_CNK_OBJECT draw_137;
static Sint16 draw_135_polygons[551];
static Sint32 draw_135_vertices[531];
static NJS_CNK_MODEL draw_135_model;
static NJS_CNK_OBJECT draw_135;
static Sint16 draw_134_polygons[343];
static Sint32 draw_134_vertices[323];
static NJS_CNK_MODEL draw_134_model;
static NJS_CNK_OBJECT draw_134;
static Sint16 draw_133_polygons[3789];
static Sint32 draw_133_vertices[3947];
static NJS_CNK_MODEL draw_133_model;
static NJS_CNK_OBJECT draw_133;
static Sint16 draw_132_polygons[677];
static Sint32 draw_132_vertices[507];
static NJS_CNK_MODEL draw_132_model;
static NJS_CNK_OBJECT draw_132;
static Sint16 draw_131_polygons[677];
static Sint32 draw_131_vertices[507];
static NJS_CNK_MODEL draw_131_model;
static NJS_CNK_OBJECT draw_131;
static Sint16 draw_130_polygons[269];
static Sint32 draw_130_vertices[243];
static NJS_CNK_MODEL draw_130_model;
static NJS_CNK_OBJECT draw_130;
static Sint16 draw_129_polygons[147];
static Sint32 draw_129_vertices[139];
static NJS_CNK_MODEL draw_129_model;
static NJS_CNK_OBJECT draw_129;
static Sint16 draw_128_polygons[325];
static Sint32 draw_128_vertices[227];
static NJS_CNK_MODEL draw_128_model;
static NJS_CNK_OBJECT draw_128;
static Sint16 draw_127_polygons[813];
static Sint32 draw_127_vertices[607];
static NJS_CNK_MODEL draw_127_model;
static NJS_CNK_OBJECT draw_127;
static Sint16 draw_126_polygons[169];
static Sint32 draw_126_vertices[179];
static NJS_CNK_MODEL draw_126_model;
static NJS_CNK_OBJECT draw_126;
static Sint16 draw_125_polygons[103];
static Sint32 draw_125_vertices[91];
static NJS_CNK_MODEL draw_125_model;
static NJS_CNK_OBJECT draw_125;
static Sint16 draw_124_polygons[179];
static Sint32 draw_124_vertices[131];
static NJS_CNK_MODEL draw_124_model;
static NJS_CNK_OBJECT draw_124;
static Sint16 draw_123_polygons[201];
static Sint32 draw_123_vertices[111];
static NJS_CNK_MODEL draw_123_model;
static NJS_CNK_OBJECT draw_123;
static Sint16 draw_122_polygons[145];
static Sint32 draw_122_vertices[123];
static NJS_CNK_MODEL draw_122_model;
static NJS_CNK_OBJECT draw_122;
static Sint16 draw_121_polygons[269];
static Sint32 draw_121_vertices[243];
static NJS_CNK_MODEL draw_121_model;
static NJS_CNK_OBJECT draw_121;
static Sint16 draw_120_polygons[891];
static Sint32 draw_120_vertices[479];
static NJS_CNK_MODEL draw_120_model;
static NJS_CNK_OBJECT draw_120;
static Sint16 draw_119_polygons[591];
static Sint32 draw_119_vertices[447];
static NJS_CNK_MODEL draw_119_model;
static NJS_CNK_OBJECT draw_119;
static Sint16 draw_101_polygons[547];
static Sint32 draw_101_vertices[239];
static NJS_CNK_MODEL draw_101_model;
static NJS_CNK_OBJECT draw_106;
static Sint16 draw_95_polygons[243];
static Sint32 draw_95_vertices[111];
static NJS_CNK_MODEL draw_95_model;
static NJS_CNK_OBJECT draw_98;
static Sint16 draw_85_polygons[319];
static Sint32 draw_85_vertices[143];
static NJS_CNK_MODEL draw_85_model;
static NJS_CNK_OBJECT draw_92;
static Sint16 draw_77_polygons[395];
static Sint32 draw_77_vertices[175];
static NJS_CNK_MODEL draw_77_model;
static NJS_CNK_OBJECT draw_84;
static Sint16 draw_76_polygons[207];
static Sint32 draw_76_vertices[107];
static NJS_CNK_MODEL draw_76_model;
static NJS_CNK_OBJECT draw_76;
static Sint16 draw_75_polygons[277];
static Sint32 draw_75_vertices[215];
static NJS_CNK_MODEL draw_75_model;
static NJS_CNK_OBJECT draw_75;
static Sint16 draw_74_polygons[351];
static Sint32 draw_74_vertices[299];
static NJS_CNK_MODEL draw_74_model;
static NJS_CNK_OBJECT draw_74;
static Sint16 draw_73_polygons[249];
static Sint32 draw_73_vertices[163];
static NJS_CNK_MODEL draw_73_model;
static NJS_CNK_OBJECT draw_73;
static Sint16 draw_72_polygons[169];
static Sint32 draw_72_vertices[99];
static NJS_CNK_MODEL draw_72_model;
static NJS_CNK_OBJECT draw_72;
static Sint16 draw_71_polygons[83];
static Sint32 draw_71_vertices[43];
static NJS_CNK_MODEL draw_71_model;
static NJS_CNK_OBJECT draw_71;
static Sint16 draw_70_polygons[249];
static Sint32 draw_70_vertices[163];
static NJS_CNK_MODEL draw_70_model;
static NJS_CNK_OBJECT draw_70;
static Sint16 draw_66_polygons[113];
static Sint32 draw_66_vertices[115];
static NJS_CNK_MODEL draw_66_model;
static NJS_CNK_OBJECT draw_67;
static Sint16 draw_65_polygons[281];
static Sint32 draw_65_vertices[243];
static NJS_CNK_MODEL draw_65_model;
static NJS_CNK_OBJECT draw_65;
static Sint16 draw_64_polygons[113];
static Sint32 draw_64_vertices[115];
static NJS_CNK_MODEL draw_64_model;
static NJS_CNK_OBJECT draw_64;
static Sint16 draw_62_polygons[131];
static Sint32 draw_62_vertices[107];
static NJS_CNK_MODEL draw_62_model;
static NJS_CNK_OBJECT draw_62;
static Sint16 draw_61_polygons[241];
static Sint32 draw_61_vertices[207];
static NJS_CNK_MODEL draw_61_model;
static NJS_CNK_OBJECT draw_61;
static Sint16 draw_54_polygons[667];
static Sint32 draw_54_vertices[363];
static NJS_CNK_MODEL draw_54_model;
static NJS_CNK_OBJECT draw_60;
static Sint16 draw_53_polygons[415];
static Sint32 draw_53_vertices[251];
static NJS_CNK_MODEL draw_53_model;
static NJS_CNK_OBJECT draw_53;
static Sint16 draw_45_polygons[689];
static Sint32 draw_45_vertices[391];
static NJS_CNK_MODEL draw_45_model;
static NJS_CNK_OBJECT draw_52;
static Sint16 draw_41_polygons[823];
static Sint32 draw_41_vertices[527];
static NJS_CNK_MODEL draw_41_model;
static NJS_CNK_OBJECT draw_44;
static Sint16 draw_38_polygons[823];
static Sint32 draw_38_vertices[527];
static NJS_CNK_MODEL draw_38_model;
static NJS_CNK_OBJECT draw_40;
static Sint16 draw_35_polygons[583];
static Sint32 draw_35_vertices[319];
static NJS_CNK_MODEL draw_35_model;
static NJS_CNK_OBJECT draw_37;
static Sint16 draw_34_polygons[269];
static Sint32 draw_34_vertices[227];
static NJS_CNK_MODEL draw_34_model;
static NJS_CNK_OBJECT draw_34;
static Sint16 draw_33_polygons[63];
static Sint32 draw_33_vertices[67];
static NJS_CNK_MODEL draw_33_model;
static NJS_CNK_OBJECT draw_33;
static Sint16 draw_32_polygons[745];
static Sint32 draw_32_vertices[483];
static NJS_CNK_MODEL draw_32_model;
static NJS_CNK_OBJECT draw_32;
static Sint16 draw_31_polygons[683];
static Sint32 draw_31_vertices[507];
static NJS_CNK_MODEL draw_31_model;
static NJS_CNK_OBJECT draw_31;
static Sint16 draw_30_polygons[117];
static Sint32 draw_30_vertices[71];
static NJS_CNK_MODEL draw_30_model;
static NJS_CNK_OBJECT draw_30;
static Sint16 draw_29_polygons[363];
static Sint32 draw_29_vertices[295];
static NJS_CNK_MODEL draw_29_model;
static NJS_CNK_OBJECT draw_29;
static Sint16 draw_28_polygons[511];
static Sint32 draw_28_vertices[475];
static NJS_CNK_MODEL draw_28_model;
static NJS_CNK_OBJECT draw_28;
static Sint16 draw_27_polygons[1171];
static Sint32 draw_27_vertices[611];
static NJS_CNK_MODEL draw_27_model;
static NJS_CNK_OBJECT draw_27;
static Sint16 draw_26_polygons[159];
static Sint32 draw_26_vertices[139];
static NJS_CNK_MODEL draw_26_model;
static NJS_CNK_OBJECT draw_26;
static Sint16 draw_25_polygons[213];
static Sint32 draw_25_vertices[179];
static NJS_CNK_MODEL draw_25_model;
static NJS_CNK_OBJECT draw_25;
static Sint16 draw_24_polygons[415];
static Sint32 draw_24_vertices[267];
static NJS_CNK_MODEL draw_24_model;
static NJS_CNK_OBJECT draw_24;
static Sint16 draw_23_polygons[415];
static Sint32 draw_23_vertices[267];
static NJS_CNK_MODEL draw_23_model;
static NJS_CNK_OBJECT draw_23;
static Sint16 draw_22_polygons[415];
static Sint32 draw_22_vertices[267];
static NJS_CNK_MODEL draw_22_model;
static NJS_CNK_OBJECT draw_22;
static Sint16 draw_21_polygons[347];
static Sint32 draw_21_vertices[267];
static NJS_CNK_MODEL draw_21_model;
static NJS_CNK_OBJECT draw_21;
static Sint16 draw_20_polygons[347];
static Sint32 draw_20_vertices[267];
static NJS_CNK_MODEL draw_20_model;
static NJS_CNK_OBJECT draw_20;
static Sint16 draw_19_polygons[415];
static Sint32 draw_19_vertices[267];
static NJS_CNK_MODEL draw_19_model;
static NJS_CNK_OBJECT draw_19;
static Sint16 draw_18_polygons[415];
static Sint32 draw_18_vertices[267];
static NJS_CNK_MODEL draw_18_model;
static NJS_CNK_OBJECT draw_18;
static Sint16 draw_17_polygons[261];
static Sint32 draw_17_vertices[207];
static NJS_CNK_MODEL draw_17_model;
static NJS_CNK_OBJECT draw_17;
static Sint16 draw_16_polygons[213];
static Sint32 draw_16_vertices[179];
static NJS_CNK_MODEL draw_16_model;
static NJS_CNK_OBJECT draw_16;
static Sint16 draw_15_polygons[213];
static Sint32 draw_15_vertices[179];
static NJS_CNK_MODEL draw_15_model;
static NJS_CNK_OBJECT draw_15;
static Sint16 draw_14_polygons[213];
static Sint32 draw_14_vertices[179];
static NJS_CNK_MODEL draw_14_model;
static NJS_CNK_OBJECT draw_14;
static Sint16 draw_13_polygons[213];
static Sint32 draw_13_vertices[179];
static NJS_CNK_MODEL draw_13_model;
static NJS_CNK_OBJECT draw_13;
static Sint16 draw_12_polygons[415];
static Sint32 draw_12_vertices[267];
static NJS_CNK_MODEL draw_12_model;
static NJS_CNK_OBJECT draw_12;
static Sint16 draw_11_polygons[415];
static Sint32 draw_11_vertices[267];
static NJS_CNK_MODEL draw_11_model;
static NJS_CNK_OBJECT draw_11;
static Sint16 draw_10_polygons[415];
static Sint32 draw_10_vertices[267];
static NJS_CNK_MODEL draw_10_model;
static NJS_CNK_OBJECT draw_10;
static Sint16 draw_9_polygons[287];
static Sint32 draw_9_vertices[239];
static NJS_CNK_MODEL draw_9_model;
static NJS_CNK_OBJECT draw_9;
static Sint16 draw_8_polygons[523];
static Sint32 draw_8_vertices[275];
static NJS_CNK_MODEL draw_8_model;
static NJS_CNK_OBJECT draw_8;
static Sint16 draw_7_polygons[385];
static Sint32 draw_7_vertices[255];
static NJS_CNK_MODEL draw_7_model;
static NJS_CNK_OBJECT draw_7;
static Sint16 draw_6_polygons[141];
static Sint32 draw_6_vertices[135];
static NJS_CNK_MODEL draw_6_model;
static NJS_CNK_OBJECT draw_6;
static Sint16 draw_5_polygons[319];
static Sint32 draw_5_vertices[283];
static NJS_CNK_MODEL draw_5_model;
static NJS_CNK_OBJECT draw_5;
static Sint16 draw_4_polygons[327];
static Sint32 draw_4_vertices[275];
static NJS_CNK_MODEL draw_4_model;
static NJS_CNK_OBJECT draw_4;
static Sint16 draw_3_polygons[327];
static Sint32 draw_3_vertices[259];
static NJS_CNK_MODEL draw_3_model;
static NJS_CNK_OBJECT draw_3;
static Sint16 draw_2_polygons[291];
static Sint32 draw_2_vertices[251];
static NJS_CNK_MODEL draw_2_model;
static NJS_CNK_OBJECT draw_2;
static Sint16 draw_1_polygons[585];
static Sint32 draw_1_vertices[451];
static NJS_CNK_MODEL draw_1_model;
static NJS_CNK_OBJECT draw_1;
static Sint16 draw_0_polygons[665];
static Sint32 draw_0_vertices[523];
static NJS_CNK_MODEL draw_0_model;
static NJS_CNK_OBJECT draw_0;
static NJS_OBJECT collision_9;
static NJS_OBJECT collision_10;
static NJS_OBJECT collision_11;
static NJS_OBJECT collision_15;
static NJS_OBJECT collision_21;
static NJS_OBJECT collision_22;
static NJS_OBJECT collision_26;
static NJS_OBJECT collision_31;
static NJS_OBJECT collision_33;
static NJS_OBJECT collision_42;
static NJS_OBJECT collision_47;
static NJS_OBJECT collision_48;
static NJS_OBJECT collision_49;
static NJS_OBJECT collision_53;
static NJS_OBJECT collision_55;
static NJS_OBJECT collision_56;
static NJS_OBJECT collision_57;
static NJS_OBJECT collision_58;
static NJS_OBJECT collision_59;
static NJS_OBJECT collision_60;
static NJS_OBJECT collision_61;
static NJS_OBJECT collision_63;
static NJS_OBJECT collision_64;
static NJS_OBJECT collision_65;
static NJS_OBJECT collision_66;
static NJS_OBJECT collision_67;
static NJS_OBJECT collision_68;
static NJS_OBJECT collision_69;
static NJS_OBJECT collision_71;
static NJS_OBJECT collision_72;
static NJS_OBJECT collision_73;
static NJS_OBJECT collision_75;
static NJS_OBJECT collision_76;
static NJS_OBJECT collision_78;
static NJS_OBJECT collision_79;
static NJS_OBJECT collision_84;
static NJS_OBJECT collision_85;
static NJS_OBJECT collision_86;
static NJS_OBJECT collision_87;
static NJS_OBJECT collision_89;
static NJS_OBJECT collision_95;
static NJS_OBJECT collision_96;
static NJS_OBJECT collision_97;
static NJS_OBJECT collision_98;
static NJS_OBJECT collision_99;
static NJS_OBJECT collision_100;
static NJS_OBJECT collision_101;
static NJS_OBJECT collision_103;
static NJS_OBJECT collision_104;
static NJS_OBJECT collision_105;
static NJS_OBJECT collision_106;
static NJS_OBJECT collision_107;
static NJS_OBJECT collision_108;
static NJS_OBJECT collision_109;
static NJS_OBJECT collision_111;
static NJS_OBJECT collision_112;
static NJS_OBJECT collision_113;
static NJS_OBJECT collision_115;
static NJS_OBJECT collision_116;
static NJS_OBJECT collision_118;
static NJS_OBJECT collision_119;
static NJS_CNK_OBJECT draw_156;
static NJS_CNK_OBJECT draw_154;
static NJS_CNK_OBJECT draw_153;
static NJS_CNK_OBJECT draw_151;
static NJS_CNK_OBJECT draw_150;
static NJS_CNK_OBJECT draw_148;
static NJS_CNK_OBJECT draw_147;
static NJS_CNK_OBJECT draw_144;
static NJS_CNK_OBJECT draw_143;
static NJS_CNK_OBJECT draw_142;
static NJS_CNK_OBJECT draw_139;
static NJS_CNK_OBJECT draw_136;
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
static NJS_CNK_OBJECT draw_105;
static NJS_CNK_OBJECT draw_104;
static NJS_CNK_OBJECT draw_103;
static NJS_CNK_OBJECT draw_102;
static NJS_CNK_OBJECT draw_101;
static NJS_CNK_OBJECT draw_100;
static NJS_CNK_OBJECT draw_99;
static NJS_CNK_OBJECT draw_97;
static NJS_CNK_OBJECT draw_96;
static NJS_CNK_OBJECT draw_95;
static NJS_CNK_OBJECT draw_94;
static NJS_CNK_OBJECT draw_93;
static NJS_CNK_OBJECT draw_91;
static NJS_CNK_OBJECT draw_90;
static NJS_CNK_OBJECT draw_89;
static NJS_CNK_OBJECT draw_88;
static NJS_CNK_OBJECT draw_87;
static NJS_CNK_OBJECT draw_86;
static NJS_CNK_OBJECT draw_85;
static NJS_CNK_OBJECT draw_83;
static NJS_CNK_OBJECT draw_82;
static NJS_CNK_OBJECT draw_81;
static NJS_CNK_OBJECT draw_80;
static NJS_CNK_OBJECT draw_79;
static NJS_CNK_OBJECT draw_78;
static NJS_CNK_OBJECT draw_77;
static NJS_CNK_OBJECT draw_69;
static NJS_CNK_OBJECT draw_68;
static NJS_CNK_OBJECT draw_66;
static NJS_CNK_OBJECT draw_63;
static NJS_CNK_OBJECT draw_59;
static NJS_CNK_OBJECT draw_58;
static NJS_CNK_OBJECT draw_57;
static NJS_CNK_OBJECT draw_56;
static NJS_CNK_OBJECT draw_55;
static NJS_CNK_OBJECT draw_54;
static NJS_CNK_OBJECT draw_51;
static NJS_CNK_OBJECT draw_50;
static NJS_CNK_OBJECT draw_49;
static NJS_CNK_OBJECT draw_48;
static NJS_CNK_OBJECT draw_47;
static NJS_CNK_OBJECT draw_46;
static NJS_CNK_OBJECT draw_45;
static NJS_CNK_OBJECT draw_43;
static NJS_CNK_OBJECT draw_42;
static NJS_CNK_OBJECT draw_41;
static NJS_CNK_OBJECT draw_39;
static NJS_CNK_OBJECT draw_38;
static NJS_CNK_OBJECT draw_36;
static NJS_CNK_OBJECT draw_35;

static NJS_MKEY_F rail_hang_motion_joint_0_position[2] = {
#include "stg34/terrain/rail_hang_motion_joint_0_position.inc"
};
static NJS_MKEY_F rail_hang_motion_joint_2_position[16] = {
#include "stg34/terrain/rail_hang_motion_joint_2_position.inc"
};
static NJS_MKEY_A rail_hang_motion_joint_4_angles[16] = {
#include "stg34/terrain/rail_hang_motion_joint_4_angles.inc"
};
static NJS_MKEY_A rail_hang_motion_joint_7_angles[16] = {
#include "stg34/terrain/rail_hang_motion_joint_7_angles.inc"
};
static NJS_MKEY_A rail_hang_motion_joint_8_angles[16] = {
#include "stg34/terrain/rail_hang_motion_joint_8_angles.inc"
};
static NJS_MKEY_A rail_hang_motion_joint_9_angles[16] = {
#include "stg34/terrain/rail_hang_motion_joint_9_angles.inc"
};
static NJS_MKEY_A rail_hang_motion_joint_13_angles[16] = {
#include "stg34/terrain/rail_hang_motion_joint_13_angles.inc"
};
static NJS_MKEY_A rail_hang_motion_joint_14_angles[2] = {
#include "stg34/terrain/rail_hang_motion_joint_14_angles.inc"
};
static NJS_MKEY_A rail_hang_motion_joint_17_angles[16] = {
#include "stg34/terrain/rail_hang_motion_joint_17_angles.inc"
};
static NJS_MKEY_A rail_hang_motion_joint_18_angles[10] = {
#include "stg34/terrain/rail_hang_motion_joint_18_angles.inc"
};
static NJS_MKEY_A rail_hang_motion_joint_19_angles[16] = {
#include "stg34/terrain/rail_hang_motion_joint_19_angles.inc"
};
static NJS_MKEY_A rail_hang_motion_joint_23_angles[16] = {
#include "stg34/terrain/rail_hang_motion_joint_23_angles.inc"
};
static NJS_MKEY_A rail_hang_motion_joint_24_angles[2] = {
#include "stg34/terrain/rail_hang_motion_joint_24_angles.inc"
};
static NJS_MKEY_A rail_hang_motion_joint_27_angles[16] = {
#include "stg34/terrain/rail_hang_motion_joint_27_angles.inc"
};
static NJS_MKEY_A rail_hang_motion_joint_31_angles[16] = {
#include "stg34/terrain/rail_hang_motion_joint_31_angles.inc"
};
static NJS_MKEY_A rail_hang_motion_joint_32_angles[12] = {
#include "stg34/terrain/rail_hang_motion_joint_32_angles.inc"
};
static NJS_MKEY_A rail_hang_motion_joint_33_angles[12] = {
#include "stg34/terrain/rail_hang_motion_joint_33_angles.inc"
};
static NJS_MKEY_A rail_hang_motion_joint_37_angles[12] = {
#include "stg34/terrain/rail_hang_motion_joint_37_angles.inc"
};
static NJS_MKEY_A rail_hang_motion_joint_38_angles[12] = {
#include "stg34/terrain/rail_hang_motion_joint_38_angles.inc"
};
static NJS_MKEY_A rail_hang_motion_joint_39_angles[12] = {
#include "stg34/terrain/rail_hang_motion_joint_39_angles.inc"
};
static NJS_MKEY_A rail_hang_motion_joint_41_angles[2] = {
#include "stg34/terrain/rail_hang_motion_joint_41_angles.inc"
};
static NJS_MKEY_A rail_hang_motion_joint_44_angles[12] = {
#include "stg34/terrain/rail_hang_motion_joint_44_angles.inc"
};
static NJS_MKEY_A rail_hang_motion_joint_51_angles[16] = {
#include "stg34/terrain/rail_hang_motion_joint_51_angles.inc"
};
static NJS_MKEY_A rail_hang_motion_joint_53_angles[16] = {
#include "stg34/terrain/rail_hang_motion_joint_53_angles.inc"
};
static NJS_MKEY_A rail_hang_motion_joint_56_angles[12] = {
#include "stg34/terrain/rail_hang_motion_joint_56_angles.inc"
};
static NJS_MKEY_A rail_hang_motion_joint_57_angles[2] = {
#include "stg34/terrain/rail_hang_motion_joint_57_angles.inc"
};
static NJS_MKEY_A rail_hang_motion_joint_58_angles[2] = {
#include "stg34/terrain/rail_hang_motion_joint_58_angles.inc"
};
static NJS_MKEY_A rail_hang_motion_joint_60_angles[12] = {
#include "stg34/terrain/rail_hang_motion_joint_60_angles.inc"
};
static NJS_MDATA2 rail_hang_motion_tracks[62] = {
#include "stg34/terrain/rail_hang_motion_tracks.inc"
};
static NJS_MOTION rail_hang_motion = {
#include "stg34/terrain/rail_hang_motion.inc"
};
static NJS_MKEY_F player_lift_motion_joint_2_position[32] = {
#include "stg34/terrain/player_lift_motion_joint_2_position.inc"
};
static NJS_MKEY_A player_lift_motion_joint_4_angles[32] = {
#include "stg34/terrain/player_lift_motion_joint_4_angles.inc"
};
static NJS_MKEY_A player_lift_motion_joint_7_angles[32] = {
#include "stg34/terrain/player_lift_motion_joint_7_angles.inc"
};
static NJS_MKEY_A player_lift_motion_joint_8_angles[32] = {
#include "stg34/terrain/player_lift_motion_joint_8_angles.inc"
};
static NJS_MKEY_A player_lift_motion_joint_9_angles[32] = {
#include "stg34/terrain/player_lift_motion_joint_9_angles.inc"
};
static NJS_MKEY_A player_lift_motion_joint_13_angles[32] = {
#include "stg34/terrain/player_lift_motion_joint_13_angles.inc"
};
static NJS_MKEY_A player_lift_motion_joint_14_angles[32] = {
#include "stg34/terrain/player_lift_motion_joint_14_angles.inc"
};
static NJS_MKEY_A player_lift_motion_joint_17_angles[32] = {
#include "stg34/terrain/player_lift_motion_joint_17_angles.inc"
};
static NJS_MKEY_A player_lift_motion_joint_18_angles[32] = {
#include "stg34/terrain/player_lift_motion_joint_18_angles.inc"
};
static NJS_MKEY_A player_lift_motion_joint_19_angles[32] = {
#include "stg34/terrain/player_lift_motion_joint_19_angles.inc"
};
static NJS_MKEY_A player_lift_motion_joint_23_angles[32] = {
#include "stg34/terrain/player_lift_motion_joint_23_angles.inc"
};
static NJS_MKEY_A player_lift_motion_joint_24_angles[32] = {
#include "stg34/terrain/player_lift_motion_joint_24_angles.inc"
};
static NJS_MKEY_A player_lift_motion_joint_27_angles[32] = {
#include "stg34/terrain/player_lift_motion_joint_27_angles.inc"
};
static NJS_MKEY_A player_lift_motion_joint_28_angles[2] = {
#include "stg34/terrain/player_lift_motion_joint_28_angles.inc"
};
static NJS_MKEY_A player_lift_motion_joint_29_angles[2] = {
#include "stg34/terrain/player_lift_motion_joint_29_angles.inc"
};
static NJS_MKEY_A player_lift_motion_joint_31_angles[32] = {
#include "stg34/terrain/player_lift_motion_joint_31_angles.inc"
};
static NJS_MKEY_A player_lift_motion_joint_32_angles[32] = {
#include "stg34/terrain/player_lift_motion_joint_32_angles.inc"
};
static NJS_MKEY_A player_lift_motion_joint_33_angles[32] = {
#include "stg34/terrain/player_lift_motion_joint_33_angles.inc"
};
static NJS_MKEY_A player_lift_motion_joint_37_angles[32] = {
#include "stg34/terrain/player_lift_motion_joint_37_angles.inc"
};
static NJS_MKEY_A player_lift_motion_joint_38_angles[32] = {
#include "stg34/terrain/player_lift_motion_joint_38_angles.inc"
};
static NJS_MKEY_A player_lift_motion_joint_39_angles[32] = {
#include "stg34/terrain/player_lift_motion_joint_39_angles.inc"
};
static NJS_MKEY_A player_lift_motion_joint_41_angles[32] = {
#include "stg34/terrain/player_lift_motion_joint_41_angles.inc"
};
static NJS_MKEY_A player_lift_motion_joint_44_angles[32] = {
#include "stg34/terrain/player_lift_motion_joint_44_angles.inc"
};
static NJS_MKEY_A player_lift_motion_joint_51_angles[32] = {
#include "stg34/terrain/player_lift_motion_joint_51_angles.inc"
};
static NJS_MKEY_A player_lift_motion_joint_53_angles[32] = {
#include "stg34/terrain/player_lift_motion_joint_53_angles.inc"
};
static NJS_MKEY_A player_lift_motion_joint_56_angles[32] = {
#include "stg34/terrain/player_lift_motion_joint_56_angles.inc"
};
static NJS_MKEY_A player_lift_motion_joint_57_angles[32] = {
#include "stg34/terrain/player_lift_motion_joint_57_angles.inc"
};
static NJS_MKEY_A player_lift_motion_joint_58_angles[32] = {
#include "stg34/terrain/player_lift_motion_joint_58_angles.inc"
};
static NJS_MKEY_A player_lift_motion_joint_60_angles[32] = {
#include "stg34/terrain/player_lift_motion_joint_60_angles.inc"
};
static NJS_MDATA2 player_lift_motion_tracks[62] = {
#include "stg34/terrain/player_lift_motion_tracks.inc"
};
static NJS_MOTION player_lift_motion = {
#include "stg34/terrain/player_lift_motion.inc"
};
struct motion_entry module_motions[3] = {
#include "stg34/terrain/module_motions.inc"
};
static struct land_entry entries[282] = {
#include "stg34/terrain/entries.inc"
};
struct land_table core_sonic_land = {
#include "stg34/terrain/core_sonic_land.inc"
};
static NJS_TEXNAME textures[43] = {
#include "stg34/terrain/textures.inc"
};
NJS_TEXLIST core_sonic_land_texlist = {
#include "stg34/terrain/core_sonic_land_texlist.inc"
};
static NJS_MATERIAL collision_0_materials[1] = {
#include "stg34/terrain/collision_0_materials.inc"
};
static Sint16 collision_0_faces[81] = {
#include "stg34/terrain/collision_0_faces.inc"
};
static NJS_MESHSET collision_0_meshsets[1] = {
#include "stg34/terrain/collision_0_meshsets.inc"
};
static NJS_POINT3 collision_0_points[37] = {
#include "stg34/terrain/collision_0_points.inc"
};
static NJS_MODEL collision_0_model = {
#include "stg34/terrain/collision_0_model.inc"
};
static NJS_OBJECT collision_0 = {
#include "stg34/terrain/collision_0.inc"
};
static NJS_MATERIAL collision_1_materials[1] = {
#include "stg34/terrain/collision_1_materials.inc"
};
static Sint16 collision_1_faces[19] = {
#include "stg34/terrain/collision_1_faces.inc"
};
static NJS_MESHSET collision_1_meshsets[1] = {
#include "stg34/terrain/collision_1_meshsets.inc"
};
static NJS_POINT3 collision_1_points[8] = {
#include "stg34/terrain/collision_1_points.inc"
};
static NJS_MODEL collision_1_model = {
#include "stg34/terrain/collision_1_model.inc"
};
static NJS_OBJECT collision_1 = {
#include "stg34/terrain/collision_1.inc"
};
static NJS_MATERIAL collision_2_materials[1] = {
#include "stg34/terrain/collision_2_materials.inc"
};
static Sint16 collision_2_faces[50] = {
#include "stg34/terrain/collision_2_faces.inc"
};
static NJS_MESHSET collision_2_meshsets[1] = {
#include "stg34/terrain/collision_2_meshsets.inc"
};
static NJS_POINT3 collision_2_points[22] = {
#include "stg34/terrain/collision_2_points.inc"
};
static NJS_MODEL collision_2_model = {
#include "stg34/terrain/collision_2_model.inc"
};
static NJS_OBJECT collision_2 = {
#include "stg34/terrain/collision_2.inc"
};
static NJS_MATERIAL collision_3_materials[1] = {
#include "stg34/terrain/collision_3_materials.inc"
};
static Sint16 collision_3_faces[19] = {
#include "stg34/terrain/collision_3_faces.inc"
};
static NJS_MESHSET collision_3_meshsets[1] = {
#include "stg34/terrain/collision_3_meshsets.inc"
};
static NJS_POINT3 collision_3_points[18] = {
#include "stg34/terrain/collision_3_points.inc"
};
static NJS_MODEL collision_3_model = {
#include "stg34/terrain/collision_3_model.inc"
};
static NJS_OBJECT collision_3 = {
#include "stg34/terrain/collision_3.inc"
};
static NJS_MATERIAL collision_4_materials[1] = {
#include "stg34/terrain/collision_4_materials.inc"
};
static Sint16 collision_4_faces[136] = {
#include "stg34/terrain/collision_4_faces.inc"
};
static NJS_MESHSET collision_4_meshsets[1] = {
#include "stg34/terrain/collision_4_meshsets.inc"
};
static NJS_POINT3 collision_4_points[62] = {
#include "stg34/terrain/collision_4_points.inc"
};
static NJS_MODEL collision_4_model = {
#include "stg34/terrain/collision_4_model.inc"
};
static NJS_OBJECT collision_4 = {
#include "stg34/terrain/collision_4.inc"
};
static NJS_MATERIAL collision_5_materials[1] = {
#include "stg34/terrain/collision_5_materials.inc"
};
static Sint16 collision_5_faces[47] = {
#include "stg34/terrain/collision_5_faces.inc"
};
static NJS_MESHSET collision_5_meshsets[1] = {
#include "stg34/terrain/collision_5_meshsets.inc"
};
static NJS_POINT3 collision_5_points[23] = {
#include "stg34/terrain/collision_5_points.inc"
};
static NJS_MODEL collision_5_model = {
#include "stg34/terrain/collision_5_model.inc"
};
static NJS_OBJECT collision_5 = {
#include "stg34/terrain/collision_5.inc"
};
static NJS_MATERIAL collision_6_materials[1] = {
#include "stg34/terrain/collision_6_materials.inc"
};
static Sint16 collision_6_faces[47] = {
#include "stg34/terrain/collision_6_faces.inc"
};
static NJS_MESHSET collision_6_meshsets[1] = {
#include "stg34/terrain/collision_6_meshsets.inc"
};
static NJS_POINT3 collision_6_points[23] = {
#include "stg34/terrain/collision_6_points.inc"
};
static NJS_MODEL collision_6_model = {
#include "stg34/terrain/collision_6_model.inc"
};
static NJS_OBJECT collision_6 = {
#include "stg34/terrain/collision_6.inc"
};
static NJS_MATERIAL collision_7_materials[1] = {
#include "stg34/terrain/collision_7_materials.inc"
};
static Sint16 collision_7_faces[45] = {
#include "stg34/terrain/collision_7_faces.inc"
};
static NJS_MESHSET collision_7_meshsets[1] = {
#include "stg34/terrain/collision_7_meshsets.inc"
};
static NJS_POINT3 collision_7_points[20] = {
#include "stg34/terrain/collision_7_points.inc"
};
static NJS_MODEL collision_7_model = {
#include "stg34/terrain/collision_7_model.inc"
};
static NJS_OBJECT collision_7 = {
#include "stg34/terrain/collision_7.inc"
};
static NJS_MATERIAL collision_8_materials[1] = {
#include "stg34/terrain/collision_8_materials.inc"
};
static Sint16 collision_8_faces[17] = {
#include "stg34/terrain/collision_8_faces.inc"
};
static NJS_MESHSET collision_8_meshsets[1] = {
#include "stg34/terrain/collision_8_meshsets.inc"
};
static NJS_POINT3 collision_8_points[12] = {
#include "stg34/terrain/collision_8_points.inc"
};
static NJS_MODEL collision_8_model = {
#include "stg34/terrain/collision_8_model.inc"
};
static NJS_OBJECT collision_8 = {
#include "stg34/terrain/collision_8.inc"
};
static NJS_MATERIAL collision_12_materials[1] = {
#include "stg34/terrain/collision_12_materials.inc"
};
static Sint16 collision_12_faces[48] = {
#include "stg34/terrain/collision_12_faces.inc"
};
static NJS_MESHSET collision_12_meshsets[1] = {
#include "stg34/terrain/collision_12_meshsets.inc"
};
static NJS_POINT3 collision_12_points[24] = {
#include "stg34/terrain/collision_12_points.inc"
};
static NJS_MODEL collision_12_model = {
#include "stg34/terrain/collision_12_model.inc"
};
static NJS_OBJECT collision_12 = {
#include "stg34/terrain/collision_12.inc"
};
static NJS_MATERIAL collision_13_materials[1] = {
#include "stg34/terrain/collision_13_materials.inc"
};
static Sint16 collision_13_faces[48] = {
#include "stg34/terrain/collision_13_faces.inc"
};
static NJS_MESHSET collision_13_meshsets[1] = {
#include "stg34/terrain/collision_13_meshsets.inc"
};
static NJS_POINT3 collision_13_points[26] = {
#include "stg34/terrain/collision_13_points.inc"
};
static NJS_MODEL collision_13_model = {
#include "stg34/terrain/collision_13_model.inc"
};
static NJS_OBJECT collision_13 = {
#include "stg34/terrain/collision_13.inc"
};
static NJS_MATERIAL collision_14_materials[1] = {
#include "stg34/terrain/collision_14_materials.inc"
};
static Sint16 collision_14_faces[17] = {
#include "stg34/terrain/collision_14_faces.inc"
};
static NJS_MESHSET collision_14_meshsets[1] = {
#include "stg34/terrain/collision_14_meshsets.inc"
};
static NJS_POINT3 collision_14_points[12] = {
#include "stg34/terrain/collision_14_points.inc"
};
static NJS_MODEL collision_14_model = {
#include "stg34/terrain/collision_14_model.inc"
};
static NJS_OBJECT collision_14 = {
#include "stg34/terrain/collision_14.inc"
};
static NJS_MATERIAL collision_16_materials[1] = {
#include "stg34/terrain/collision_16_materials.inc"
};
static Sint16 collision_16_faces[9] = {
#include "stg34/terrain/collision_16_faces.inc"
};
static NJS_MESHSET collision_16_meshsets[1] = {
#include "stg34/terrain/collision_16_meshsets.inc"
};
static NJS_POINT3 collision_16_points[8] = {
#include "stg34/terrain/collision_16_points.inc"
};
static NJS_MODEL collision_16_model = {
#include "stg34/terrain/collision_16_model.inc"
};
static NJS_OBJECT collision_16 = {
#include "stg34/terrain/collision_16.inc"
};
static NJS_MATERIAL collision_17_materials[1] = {
#include "stg34/terrain/collision_17_materials.inc"
};
static Sint16 collision_17_faces[5] = {
#include "stg34/terrain/collision_17_faces.inc"
};
static NJS_MESHSET collision_17_meshsets[1] = {
#include "stg34/terrain/collision_17_meshsets.inc"
};
static NJS_POINT3 collision_17_points[4] = {
#include "stg34/terrain/collision_17_points.inc"
};
static NJS_MODEL collision_17_model = {
#include "stg34/terrain/collision_17_model.inc"
};
static NJS_OBJECT collision_17 = {
#include "stg34/terrain/collision_17.inc"
};
static NJS_MATERIAL collision_18_materials[1] = {
#include "stg34/terrain/collision_18_materials.inc"
};
static Sint16 collision_18_faces[197] = {
#include "stg34/terrain/collision_18_faces.inc"
};
static NJS_MESHSET collision_18_meshsets[1] = {
#include "stg34/terrain/collision_18_meshsets.inc"
};
static NJS_POINT3 collision_18_points[80] = {
#include "stg34/terrain/collision_18_points.inc"
};
static NJS_MODEL collision_18_model = {
#include "stg34/terrain/collision_18_model.inc"
};
static NJS_OBJECT collision_18 = {
#include "stg34/terrain/collision_18.inc"
};
static NJS_MATERIAL collision_19_materials[1] = {
#include "stg34/terrain/collision_19_materials.inc"
};
static Sint16 collision_19_faces[104] = {
#include "stg34/terrain/collision_19_faces.inc"
};
static NJS_MESHSET collision_19_meshsets[1] = {
#include "stg34/terrain/collision_19_meshsets.inc"
};
static NJS_POINT3 collision_19_points[60] = {
#include "stg34/terrain/collision_19_points.inc"
};
static NJS_MODEL collision_19_model = {
#include "stg34/terrain/collision_19_model.inc"
};
static NJS_OBJECT collision_19 = {
#include "stg34/terrain/collision_19.inc"
};
static NJS_MATERIAL collision_20_materials[1] = {
#include "stg34/terrain/collision_20_materials.inc"
};
static Sint16 collision_20_faces[44] = {
#include "stg34/terrain/collision_20_faces.inc"
};
static NJS_MESHSET collision_20_meshsets[1] = {
#include "stg34/terrain/collision_20_meshsets.inc"
};
static NJS_POINT3 collision_20_points[24] = {
#include "stg34/terrain/collision_20_points.inc"
};
static NJS_MODEL collision_20_model = {
#include "stg34/terrain/collision_20_model.inc"
};
static NJS_OBJECT collision_20 = {
#include "stg34/terrain/collision_20.inc"
};
static NJS_MATERIAL collision_23_materials[1] = {
#include "stg34/terrain/collision_23_materials.inc"
};
static Sint16 collision_23_faces[107] = {
#include "stg34/terrain/collision_23_faces.inc"
};
static NJS_MESHSET collision_23_meshsets[1] = {
#include "stg34/terrain/collision_23_meshsets.inc"
};
static NJS_POINT3 collision_23_points[64] = {
#include "stg34/terrain/collision_23_points.inc"
};
static NJS_MODEL collision_23_model = {
#include "stg34/terrain/collision_23_model.inc"
};
static NJS_OBJECT collision_23 = {
#include "stg34/terrain/collision_23.inc"
};
static NJS_MATERIAL collision_24_materials[1] = {
#include "stg34/terrain/collision_24_materials.inc"
};
static Sint16 collision_24_faces[32] = {
#include "stg34/terrain/collision_24_faces.inc"
};
static NJS_MESHSET collision_24_meshsets[1] = {
#include "stg34/terrain/collision_24_meshsets.inc"
};
static NJS_POINT3 collision_24_points[16] = {
#include "stg34/terrain/collision_24_points.inc"
};
static NJS_MODEL collision_24_model = {
#include "stg34/terrain/collision_24_model.inc"
};
static NJS_OBJECT collision_24 = {
#include "stg34/terrain/collision_24.inc"
};
static NJS_MATERIAL collision_25_materials[1] = {
#include "stg34/terrain/collision_25_materials.inc"
};
static Sint16 collision_25_faces[33] = {
#include "stg34/terrain/collision_25_faces.inc"
};
static NJS_MESHSET collision_25_meshsets[1] = {
#include "stg34/terrain/collision_25_meshsets.inc"
};
static NJS_POINT3 collision_25_points[17] = {
#include "stg34/terrain/collision_25_points.inc"
};
static NJS_MODEL collision_25_model = {
#include "stg34/terrain/collision_25_model.inc"
};
static NJS_OBJECT collision_25 = {
#include "stg34/terrain/collision_25.inc"
};
static NJS_MATERIAL collision_27_materials[1] = {
#include "stg34/terrain/collision_27_materials.inc"
};
static Sint16 collision_27_faces[19] = {
#include "stg34/terrain/collision_27_faces.inc"
};
static NJS_MESHSET collision_27_meshsets[1] = {
#include "stg34/terrain/collision_27_meshsets.inc"
};
static NJS_POINT3 collision_27_points[10] = {
#include "stg34/terrain/collision_27_points.inc"
};
static NJS_MODEL collision_27_model = {
#include "stg34/terrain/collision_27_model.inc"
};
static NJS_OBJECT collision_27 = {
#include "stg34/terrain/collision_27.inc"
};
static NJS_MATERIAL collision_28_materials[1] = {
#include "stg34/terrain/collision_28_materials.inc"
};
static Sint16 collision_28_faces[96] = {
#include "stg34/terrain/collision_28_faces.inc"
};
static NJS_MESHSET collision_28_meshsets[1] = {
#include "stg34/terrain/collision_28_meshsets.inc"
};
static NJS_POINT3 collision_28_points[42] = {
#include "stg34/terrain/collision_28_points.inc"
};
static NJS_MODEL collision_28_model = {
#include "stg34/terrain/collision_28_model.inc"
};
static NJS_OBJECT collision_28 = {
#include "stg34/terrain/collision_28.inc"
};
static NJS_MATERIAL collision_29_materials[1] = {
#include "stg34/terrain/collision_29_materials.inc"
};
static Sint16 collision_29_faces[74] = {
#include "stg34/terrain/collision_29_faces.inc"
};
static NJS_MESHSET collision_29_meshsets[1] = {
#include "stg34/terrain/collision_29_meshsets.inc"
};
static NJS_POINT3 collision_29_points[28] = {
#include "stg34/terrain/collision_29_points.inc"
};
static NJS_MODEL collision_29_model = {
#include "stg34/terrain/collision_29_model.inc"
};
static NJS_OBJECT collision_29 = {
#include "stg34/terrain/collision_29.inc"
};
static NJS_MATERIAL collision_30_materials[1] = {
#include "stg34/terrain/collision_30_materials.inc"
};
static Sint16 collision_30_faces[19] = {
#include "stg34/terrain/collision_30_faces.inc"
};
static NJS_MESHSET collision_30_meshsets[1] = {
#include "stg34/terrain/collision_30_meshsets.inc"
};
static NJS_POINT3 collision_30_points[10] = {
#include "stg34/terrain/collision_30_points.inc"
};
static NJS_MODEL collision_30_model = {
#include "stg34/terrain/collision_30_model.inc"
};
static NJS_OBJECT collision_30 = {
#include "stg34/terrain/collision_30.inc"
};
static NJS_MATERIAL collision_32_materials[1] = {
#include "stg34/terrain/collision_32_materials.inc"
};
static Sint16 collision_32_faces[15] = {
#include "stg34/terrain/collision_32_faces.inc"
};
static NJS_MESHSET collision_32_meshsets[1] = {
#include "stg34/terrain/collision_32_meshsets.inc"
};
static NJS_POINT3 collision_32_points[11] = {
#include "stg34/terrain/collision_32_points.inc"
};
static NJS_MODEL collision_32_model = {
#include "stg34/terrain/collision_32_model.inc"
};
static NJS_OBJECT collision_32 = {
#include "stg34/terrain/collision_32.inc"
};
static NJS_MATERIAL collision_34_materials[1] = {
#include "stg34/terrain/collision_34_materials.inc"
};
static Sint16 collision_34_faces[16] = {
#include "stg34/terrain/collision_34_faces.inc"
};
static NJS_MESHSET collision_34_meshsets[1] = {
#include "stg34/terrain/collision_34_meshsets.inc"
};
static NJS_POINT3 collision_34_points[9] = {
#include "stg34/terrain/collision_34_points.inc"
};
static NJS_MODEL collision_34_model = {
#include "stg34/terrain/collision_34_model.inc"
};
static NJS_OBJECT collision_34 = {
#include "stg34/terrain/collision_34.inc"
};
static NJS_MATERIAL collision_35_materials[1] = {
#include "stg34/terrain/collision_35_materials.inc"
};
static Sint16 collision_35_faces[113] = {
#include "stg34/terrain/collision_35_faces.inc"
};
static NJS_MESHSET collision_35_meshsets[1] = {
#include "stg34/terrain/collision_35_meshsets.inc"
};
static NJS_POINT3 collision_35_points[45] = {
#include "stg34/terrain/collision_35_points.inc"
};
static NJS_MODEL collision_35_model = {
#include "stg34/terrain/collision_35_model.inc"
};
static NJS_OBJECT collision_35 = {
#include "stg34/terrain/collision_35.inc"
};
static NJS_MATERIAL collision_36_materials[1] = {
#include "stg34/terrain/collision_36_materials.inc"
};
static Sint16 collision_36_faces[178] = {
#include "stg34/terrain/collision_36_faces.inc"
};
static NJS_MESHSET collision_36_meshsets[1] = {
#include "stg34/terrain/collision_36_meshsets.inc"
};
static NJS_POINT3 collision_36_points[65] = {
#include "stg34/terrain/collision_36_points.inc"
};
static NJS_MODEL collision_36_model = {
#include "stg34/terrain/collision_36_model.inc"
};
static NJS_OBJECT collision_36 = {
#include "stg34/terrain/collision_36.inc"
};
static NJS_MATERIAL collision_37_materials[1] = {
#include "stg34/terrain/collision_37_materials.inc"
};
static Sint16 collision_37_faces[28] = {
#include "stg34/terrain/collision_37_faces.inc"
};
static NJS_MESHSET collision_37_meshsets[1] = {
#include "stg34/terrain/collision_37_meshsets.inc"
};
static NJS_POINT3 collision_37_points[12] = {
#include "stg34/terrain/collision_37_points.inc"
};
static NJS_MODEL collision_37_model = {
#include "stg34/terrain/collision_37_model.inc"
};
static NJS_OBJECT collision_37 = {
#include "stg34/terrain/collision_37.inc"
};
static NJS_MATERIAL collision_38_materials[1] = {
#include "stg34/terrain/collision_38_materials.inc"
};
static Sint16 collision_38_faces[26] = {
#include "stg34/terrain/collision_38_faces.inc"
};
static NJS_MESHSET collision_38_meshsets[1] = {
#include "stg34/terrain/collision_38_meshsets.inc"
};
static NJS_POINT3 collision_38_points[12] = {
#include "stg34/terrain/collision_38_points.inc"
};
static NJS_MODEL collision_38_model = {
#include "stg34/terrain/collision_38_model.inc"
};
static NJS_OBJECT collision_38 = {
#include "stg34/terrain/collision_38.inc"
};
static NJS_MATERIAL collision_39_materials[1] = {
#include "stg34/terrain/collision_39_materials.inc"
};
static Sint16 collision_39_faces[168] = {
#include "stg34/terrain/collision_39_faces.inc"
};
static NJS_MESHSET collision_39_meshsets[1] = {
#include "stg34/terrain/collision_39_meshsets.inc"
};
static NJS_POINT3 collision_39_points[45] = {
#include "stg34/terrain/collision_39_points.inc"
};
static NJS_MODEL collision_39_model = {
#include "stg34/terrain/collision_39_model.inc"
};
static NJS_OBJECT collision_39 = {
#include "stg34/terrain/collision_39.inc"
};
static NJS_MATERIAL collision_40_materials[1] = {
#include "stg34/terrain/collision_40_materials.inc"
};
static Sint16 collision_40_faces[157] = {
#include "stg34/terrain/collision_40_faces.inc"
};
static NJS_MESHSET collision_40_meshsets[1] = {
#include "stg34/terrain/collision_40_meshsets.inc"
};
static NJS_POINT3 collision_40_points[48] = {
#include "stg34/terrain/collision_40_points.inc"
};
static NJS_MODEL collision_40_model = {
#include "stg34/terrain/collision_40_model.inc"
};
static NJS_OBJECT collision_40 = {
#include "stg34/terrain/collision_40.inc"
};
static NJS_MATERIAL collision_41_materials[1] = {
#include "stg34/terrain/collision_41_materials.inc"
};
static Sint16 collision_41_faces[39] = {
#include "stg34/terrain/collision_41_faces.inc"
};
static NJS_MESHSET collision_41_meshsets[1] = {
#include "stg34/terrain/collision_41_meshsets.inc"
};
static NJS_POINT3 collision_41_points[18] = {
#include "stg34/terrain/collision_41_points.inc"
};
static NJS_MODEL collision_41_model = {
#include "stg34/terrain/collision_41_model.inc"
};
static NJS_OBJECT collision_41 = {
#include "stg34/terrain/collision_41.inc"
};
static NJS_MATERIAL collision_43_materials[1] = {
#include "stg34/terrain/collision_43_materials.inc"
};
static Sint16 collision_43_faces[125] = {
#include "stg34/terrain/collision_43_faces.inc"
};
static NJS_MESHSET collision_43_meshsets[1] = {
#include "stg34/terrain/collision_43_meshsets.inc"
};
static NJS_POINT3 collision_43_points[53] = {
#include "stg34/terrain/collision_43_points.inc"
};
static NJS_MODEL collision_43_model = {
#include "stg34/terrain/collision_43_model.inc"
};
static NJS_OBJECT collision_43 = {
#include "stg34/terrain/collision_43.inc"
};
static NJS_MATERIAL collision_44_materials[1] = {
#include "stg34/terrain/collision_44_materials.inc"
};
static Sint16 collision_44_faces[109] = {
#include "stg34/terrain/collision_44_faces.inc"
};
static NJS_MESHSET collision_44_meshsets[1] = {
#include "stg34/terrain/collision_44_meshsets.inc"
};
static NJS_POINT3 collision_44_points[46] = {
#include "stg34/terrain/collision_44_points.inc"
};
static NJS_MODEL collision_44_model = {
#include "stg34/terrain/collision_44_model.inc"
};
static NJS_OBJECT collision_44 = {
#include "stg34/terrain/collision_44.inc"
};
static NJS_MATERIAL collision_45_materials[1] = {
#include "stg34/terrain/collision_45_materials.inc"
};
static Sint16 collision_45_faces[173] = {
#include "stg34/terrain/collision_45_faces.inc"
};
static NJS_MESHSET collision_45_meshsets[1] = {
#include "stg34/terrain/collision_45_meshsets.inc"
};
static NJS_POINT3 collision_45_points[74] = {
#include "stg34/terrain/collision_45_points.inc"
};
static NJS_MODEL collision_45_model = {
#include "stg34/terrain/collision_45_model.inc"
};
static NJS_OBJECT collision_45 = {
#include "stg34/terrain/collision_45.inc"
};
static NJS_MATERIAL collision_46_materials[1] = {
#include "stg34/terrain/collision_46_materials.inc"
};
static Sint16 collision_46_faces[10] = {
#include "stg34/terrain/collision_46_faces.inc"
};
static NJS_MESHSET collision_46_meshsets[1] = {
#include "stg34/terrain/collision_46_meshsets.inc"
};
static NJS_POINT3 collision_46_points[5] = {
#include "stg34/terrain/collision_46_points.inc"
};
static NJS_MODEL collision_46_model = {
#include "stg34/terrain/collision_46_model.inc"
};
static NJS_OBJECT collision_46 = {
#include "stg34/terrain/collision_46.inc"
};
static NJS_MATERIAL collision_50_materials[1] = {
#include "stg34/terrain/collision_50_materials.inc"
};
static Sint16 collision_50_faces[47] = {
#include "stg34/terrain/collision_50_faces.inc"
};
static NJS_MESHSET collision_50_meshsets[1] = {
#include "stg34/terrain/collision_50_meshsets.inc"
};
static NJS_POINT3 collision_50_points[18] = {
#include "stg34/terrain/collision_50_points.inc"
};
static NJS_MODEL collision_50_model = {
#include "stg34/terrain/collision_50_model.inc"
};
static NJS_OBJECT collision_50 = {
#include "stg34/terrain/collision_50.inc"
};
static NJS_MATERIAL collision_51_materials[1] = {
#include "stg34/terrain/collision_51_materials.inc"
};
static Sint16 collision_51_faces[5] = {
#include "stg34/terrain/collision_51_faces.inc"
};
static NJS_MESHSET collision_51_meshsets[1] = {
#include "stg34/terrain/collision_51_meshsets.inc"
};
static NJS_POINT3 collision_51_points[4] = {
#include "stg34/terrain/collision_51_points.inc"
};
static NJS_MODEL collision_51_model = {
#include "stg34/terrain/collision_51_model.inc"
};
static NJS_OBJECT collision_51 = {
#include "stg34/terrain/collision_51.inc"
};
static NJS_MATERIAL collision_52_materials[1] = {
#include "stg34/terrain/collision_52_materials.inc"
};
static Sint16 collision_52_faces[10] = {
#include "stg34/terrain/collision_52_faces.inc"
};
static NJS_MESHSET collision_52_meshsets[1] = {
#include "stg34/terrain/collision_52_meshsets.inc"
};
static NJS_POINT3 collision_52_points[5] = {
#include "stg34/terrain/collision_52_points.inc"
};
static NJS_MODEL collision_52_model = {
#include "stg34/terrain/collision_52_model.inc"
};
static NJS_OBJECT collision_52 = {
#include "stg34/terrain/collision_52.inc"
};
static NJS_MATERIAL collision_54_materials[1] = {
#include "stg34/terrain/collision_54_materials.inc"
};
static Sint16 collision_54_faces[110] = {
#include "stg34/terrain/collision_54_faces.inc"
};
static NJS_MESHSET collision_54_meshsets[1] = {
#include "stg34/terrain/collision_54_meshsets.inc"
};
static NJS_POINT3 collision_54_points[40] = {
#include "stg34/terrain/collision_54_points.inc"
};
static NJS_MODEL collision_54_model = {
#include "stg34/terrain/collision_54_model.inc"
};
static NJS_OBJECT collision_54 = {
#include "stg34/terrain/collision_54.inc"
};
static NJS_MATERIAL collision_62_materials[1] = {
#include "stg34/terrain/collision_62_materials.inc"
};
static Sint16 collision_62_faces[71] = {
#include "stg34/terrain/collision_62_faces.inc"
};
static NJS_MESHSET collision_62_meshsets[1] = {
#include "stg34/terrain/collision_62_meshsets.inc"
};
static NJS_POINT3 collision_62_points[28] = {
#include "stg34/terrain/collision_62_points.inc"
};
static NJS_MODEL collision_62_model = {
#include "stg34/terrain/collision_62_model.inc"
};
static NJS_OBJECT collision_62 = {
#include "stg34/terrain/collision_62.inc"
};
static NJS_MATERIAL collision_70_materials[1] = {
#include "stg34/terrain/collision_70_materials.inc"
};
static Sint16 collision_70_faces[167] = {
#include "stg34/terrain/collision_70_faces.inc"
};
static NJS_MESHSET collision_70_meshsets[1] = {
#include "stg34/terrain/collision_70_meshsets.inc"
};
static NJS_POINT3 collision_70_points[70] = {
#include "stg34/terrain/collision_70_points.inc"
};
static NJS_MODEL collision_70_model = {
#include "stg34/terrain/collision_70_model.inc"
};
static NJS_OBJECT collision_70 = {
#include "stg34/terrain/collision_70.inc"
};
static NJS_MATERIAL collision_74_materials[1] = {
#include "stg34/terrain/collision_74_materials.inc"
};
static Sint16 collision_74_faces[164] = {
#include "stg34/terrain/collision_74_faces.inc"
};
static NJS_MESHSET collision_74_meshsets[1] = {
#include "stg34/terrain/collision_74_meshsets.inc"
};
static NJS_POINT3 collision_74_points[70] = {
#include "stg34/terrain/collision_74_points.inc"
};
static NJS_MODEL collision_74_model = {
#include "stg34/terrain/collision_74_model.inc"
};
static NJS_OBJECT collision_74 = {
#include "stg34/terrain/collision_74.inc"
};
static NJS_MATERIAL collision_77_materials[1] = {
#include "stg34/terrain/collision_77_materials.inc"
};
static Sint16 collision_77_faces[72] = {
#include "stg34/terrain/collision_77_faces.inc"
};
static NJS_MESHSET collision_77_meshsets[1] = {
#include "stg34/terrain/collision_77_meshsets.inc"
};
static NJS_POINT3 collision_77_points[24] = {
#include "stg34/terrain/collision_77_points.inc"
};
static NJS_MODEL collision_77_model = {
#include "stg34/terrain/collision_77_model.inc"
};
static NJS_OBJECT collision_77 = {
#include "stg34/terrain/collision_77.inc"
};
static NJS_MATERIAL collision_80_materials[1] = {
#include "stg34/terrain/collision_80_materials.inc"
};
static Sint16 collision_80_faces[172] = {
#include "stg34/terrain/collision_80_faces.inc"
};
static NJS_MESHSET collision_80_meshsets[1] = {
#include "stg34/terrain/collision_80_meshsets.inc"
};
static NJS_POINT3 collision_80_points[61] = {
#include "stg34/terrain/collision_80_points.inc"
};
static NJS_MODEL collision_80_model = {
#include "stg34/terrain/collision_80_model.inc"
};
static NJS_OBJECT collision_80 = {
#include "stg34/terrain/collision_80.inc"
};
static NJS_MATERIAL collision_81_materials[1] = {
#include "stg34/terrain/collision_81_materials.inc"
};
static Sint16 collision_81_faces[13] = {
#include "stg34/terrain/collision_81_faces.inc"
};
static NJS_MESHSET collision_81_meshsets[1] = {
#include "stg34/terrain/collision_81_meshsets.inc"
};
static NJS_POINT3 collision_81_points[12] = {
#include "stg34/terrain/collision_81_points.inc"
};
static NJS_MODEL collision_81_model = {
#include "stg34/terrain/collision_81_model.inc"
};
static NJS_OBJECT collision_81 = {
#include "stg34/terrain/collision_81.inc"
};
static NJS_MATERIAL collision_82_materials[1] = {
#include "stg34/terrain/collision_82_materials.inc"
};
static Sint16 collision_82_faces[75] = {
#include "stg34/terrain/collision_82_faces.inc"
};
static NJS_MESHSET collision_82_meshsets[1] = {
#include "stg34/terrain/collision_82_meshsets.inc"
};
static NJS_POINT3 collision_82_points[32] = {
#include "stg34/terrain/collision_82_points.inc"
};
static NJS_MODEL collision_82_model = {
#include "stg34/terrain/collision_82_model.inc"
};
static NJS_OBJECT collision_82 = {
#include "stg34/terrain/collision_82.inc"
};
static NJS_MATERIAL collision_83_materials[1] = {
#include "stg34/terrain/collision_83_materials.inc"
};
static Sint16 collision_83_faces[38] = {
#include "stg34/terrain/collision_83_faces.inc"
};
static NJS_MESHSET collision_83_meshsets[1] = {
#include "stg34/terrain/collision_83_meshsets.inc"
};
static NJS_POINT3 collision_83_points[24] = {
#include "stg34/terrain/collision_83_points.inc"
};
static NJS_MODEL collision_83_model = {
#include "stg34/terrain/collision_83_model.inc"
};
static NJS_OBJECT collision_83 = {
#include "stg34/terrain/collision_83.inc"
};
static NJS_MATERIAL collision_88_materials[1] = {
#include "stg34/terrain/collision_88_materials.inc"
};
static Sint16 collision_88_faces[72] = {
#include "stg34/terrain/collision_88_faces.inc"
};
static NJS_MESHSET collision_88_meshsets[1] = {
#include "stg34/terrain/collision_88_meshsets.inc"
};
static NJS_POINT3 collision_88_points[32] = {
#include "stg34/terrain/collision_88_points.inc"
};
static NJS_MODEL collision_88_model = {
#include "stg34/terrain/collision_88_model.inc"
};
static NJS_OBJECT collision_88 = {
#include "stg34/terrain/collision_88.inc"
};
static NJS_MATERIAL collision_90_materials[1] = {
#include "stg34/terrain/collision_90_materials.inc"
};
static Sint16 collision_90_faces[69] = {
#include "stg34/terrain/collision_90_faces.inc"
};
static NJS_MESHSET collision_90_meshsets[1] = {
#include "stg34/terrain/collision_90_meshsets.inc"
};
static NJS_POINT3 collision_90_points[32] = {
#include "stg34/terrain/collision_90_points.inc"
};
static NJS_MODEL collision_90_model = {
#include "stg34/terrain/collision_90_model.inc"
};
static NJS_OBJECT collision_90 = {
#include "stg34/terrain/collision_90.inc"
};
static NJS_MATERIAL collision_91_materials[1] = {
#include "stg34/terrain/collision_91_materials.inc"
};
static Sint16 collision_91_faces[21] = {
#include "stg34/terrain/collision_91_faces.inc"
};
static NJS_MESHSET collision_91_meshsets[1] = {
#include "stg34/terrain/collision_91_meshsets.inc"
};
static NJS_POINT3 collision_91_points[12] = {
#include "stg34/terrain/collision_91_points.inc"
};
static NJS_MODEL collision_91_model = {
#include "stg34/terrain/collision_91_model.inc"
};
static NJS_OBJECT collision_91 = {
#include "stg34/terrain/collision_91.inc"
};
static NJS_MATERIAL collision_92_materials[1] = {
#include "stg34/terrain/collision_92_materials.inc"
};
static Sint16 collision_92_faces[42] = {
#include "stg34/terrain/collision_92_faces.inc"
};
static NJS_MESHSET collision_92_meshsets[1] = {
#include "stg34/terrain/collision_92_meshsets.inc"
};
static NJS_POINT3 collision_92_points[21] = {
#include "stg34/terrain/collision_92_points.inc"
};
static NJS_MODEL collision_92_model = {
#include "stg34/terrain/collision_92_model.inc"
};
static NJS_OBJECT collision_92 = {
#include "stg34/terrain/collision_92.inc"
};
static NJS_MATERIAL collision_93_materials[1] = {
#include "stg34/terrain/collision_93_materials.inc"
};
static Sint16 collision_93_faces[31] = {
#include "stg34/terrain/collision_93_faces.inc"
};
static NJS_MESHSET collision_93_meshsets[1] = {
#include "stg34/terrain/collision_93_meshsets.inc"
};
static NJS_POINT3 collision_93_points[15] = {
#include "stg34/terrain/collision_93_points.inc"
};
static NJS_MODEL collision_93_model = {
#include "stg34/terrain/collision_93_model.inc"
};
static NJS_OBJECT collision_93 = {
#include "stg34/terrain/collision_93.inc"
};
static NJS_MATERIAL collision_94_materials[1] = {
#include "stg34/terrain/collision_94_materials.inc"
};
static Sint16 collision_94_faces[13] = {
#include "stg34/terrain/collision_94_faces.inc"
};
static NJS_MESHSET collision_94_meshsets[1] = {
#include "stg34/terrain/collision_94_meshsets.inc"
};
static NJS_POINT3 collision_94_points[12] = {
#include "stg34/terrain/collision_94_points.inc"
};
static NJS_MODEL collision_94_model = {
#include "stg34/terrain/collision_94_model.inc"
};
static NJS_OBJECT collision_94 = {
#include "stg34/terrain/collision_94.inc"
};
static NJS_MATERIAL collision_102_materials[1] = {
#include "stg34/terrain/collision_102_materials.inc"
};
static Sint16 collision_102_faces[9] = {
#include "stg34/terrain/collision_102_faces.inc"
};
static NJS_MESHSET collision_102_meshsets[1] = {
#include "stg34/terrain/collision_102_meshsets.inc"
};
static NJS_POINT3 collision_102_points[8] = {
#include "stg34/terrain/collision_102_points.inc"
};
static NJS_MODEL collision_102_model = {
#include "stg34/terrain/collision_102_model.inc"
};
static NJS_OBJECT collision_102 = {
#include "stg34/terrain/collision_102.inc"
};
static NJS_MATERIAL collision_110_materials[1] = {
#include "stg34/terrain/collision_110_materials.inc"
};
static Sint16 collision_110_faces[35] = {
#include "stg34/terrain/collision_110_faces.inc"
};
static NJS_MESHSET collision_110_meshsets[1] = {
#include "stg34/terrain/collision_110_meshsets.inc"
};
static NJS_POINT3 collision_110_points[22] = {
#include "stg34/terrain/collision_110_points.inc"
};
static NJS_MODEL collision_110_model = {
#include "stg34/terrain/collision_110_model.inc"
};
static NJS_OBJECT collision_110 = {
#include "stg34/terrain/collision_110.inc"
};
static NJS_MATERIAL collision_114_materials[1] = {
#include "stg34/terrain/collision_114_materials.inc"
};
static Sint16 collision_114_faces[23] = {
#include "stg34/terrain/collision_114_faces.inc"
};
static NJS_MESHSET collision_114_meshsets[1] = {
#include "stg34/terrain/collision_114_meshsets.inc"
};
static NJS_POINT3 collision_114_points[22] = {
#include "stg34/terrain/collision_114_points.inc"
};
static NJS_MODEL collision_114_model = {
#include "stg34/terrain/collision_114_model.inc"
};
static NJS_OBJECT collision_114 = {
#include "stg34/terrain/collision_114.inc"
};
static NJS_MATERIAL collision_117_materials[1] = {
#include "stg34/terrain/collision_117_materials.inc"
};
static Sint16 collision_117_faces[7] = {
#include "stg34/terrain/collision_117_faces.inc"
};
static NJS_MESHSET collision_117_meshsets[1] = {
#include "stg34/terrain/collision_117_meshsets.inc"
};
static NJS_POINT3 collision_117_points[6] = {
#include "stg34/terrain/collision_117_points.inc"
};
static NJS_MODEL collision_117_model = {
#include "stg34/terrain/collision_117_model.inc"
};
static NJS_OBJECT collision_117 = {
#include "stg34/terrain/collision_117.inc"
};
static NJS_MATERIAL collision_120_materials[1] = {
#include "stg34/terrain/collision_120_materials.inc"
};
static Sint16 collision_120_faces[13] = {
#include "stg34/terrain/collision_120_faces.inc"
};
static NJS_MESHSET collision_120_meshsets[1] = {
#include "stg34/terrain/collision_120_meshsets.inc"
};
static NJS_POINT3 collision_120_points[12] = {
#include "stg34/terrain/collision_120_points.inc"
};
static NJS_MODEL collision_120_model = {
#include "stg34/terrain/collision_120_model.inc"
};
static NJS_OBJECT collision_120 = {
#include "stg34/terrain/collision_120.inc"
};
static Sint16 draw_160_polygons[477] = {
#include "stg34/terrain/draw_160_polygons.inc"
};
static Sint32 draw_160_vertices[371] = {
#include "stg34/terrain/draw_160_vertices.inc"
};
static NJS_CNK_MODEL draw_160_model = {
#include "stg34/terrain/draw_160_model.inc"
};
static NJS_CNK_OBJECT draw_160 = {
#include "stg34/terrain/draw_160.inc"
};
static Sint16 draw_159_polygons[671] = {
#include "stg34/terrain/draw_159_polygons.inc"
};
static Sint32 draw_159_vertices[403] = {
#include "stg34/terrain/draw_159_vertices.inc"
};
static NJS_CNK_MODEL draw_159_model = {
#include "stg34/terrain/draw_159_model.inc"
};
static NJS_CNK_OBJECT draw_159 = {
#include "stg34/terrain/draw_159.inc"
};
static Sint16 draw_158_polygons[671] = {
#include "stg34/terrain/draw_158_polygons.inc"
};
static Sint32 draw_158_vertices[403] = {
#include "stg34/terrain/draw_158_vertices.inc"
};
static NJS_CNK_MODEL draw_158_model = {
#include "stg34/terrain/draw_158_model.inc"
};
static NJS_CNK_OBJECT draw_158 = {
#include "stg34/terrain/draw_158.inc"
};
static Sint16 draw_157_polygons[67] = {
#include "stg34/terrain/draw_157_polygons.inc"
};
static Sint32 draw_157_vertices[67] = {
#include "stg34/terrain/draw_157_vertices.inc"
};
static NJS_CNK_MODEL draw_157_model = {
#include "stg34/terrain/draw_157_model.inc"
};
static NJS_CNK_OBJECT draw_157 = {
#include "stg34/terrain/draw_157.inc"
};
static Sint16 draw_153_polygons[1083] = {
#include "stg34/terrain/draw_153_polygons.inc"
};
static Sint32 draw_153_vertices[1135] = {
#include "stg34/terrain/draw_153_vertices.inc"
};
static NJS_CNK_MODEL draw_153_model = {
#include "stg34/terrain/draw_153_model.inc"
};
static NJS_CNK_OBJECT draw_155 = {
#include "stg34/terrain/draw_155.inc"
};
static Sint16 draw_151_polygons[773] = {
#include "stg34/terrain/draw_151_polygons.inc"
};
static Sint32 draw_151_vertices[579] = {
#include "stg34/terrain/draw_151_vertices.inc"
};
static NJS_CNK_MODEL draw_151_model = {
#include "stg34/terrain/draw_151_model.inc"
};
static NJS_CNK_OBJECT draw_152 = {
#include "stg34/terrain/draw_152.inc"
};
static Sint16 draw_147_polygons[153] = {
#include "stg34/terrain/draw_147_polygons.inc"
};
static Sint32 draw_147_vertices[91] = {
#include "stg34/terrain/draw_147_vertices.inc"
};
static NJS_CNK_MODEL draw_147_model = {
#include "stg34/terrain/draw_147_model.inc"
};
static NJS_CNK_OBJECT draw_149 = {
#include "stg34/terrain/draw_149.inc"
};
static Sint16 draw_146_polygons[147] = {
#include "stg34/terrain/draw_146_polygons.inc"
};
static Sint32 draw_146_vertices[127] = {
#include "stg34/terrain/draw_146_vertices.inc"
};
static NJS_CNK_MODEL draw_146_model = {
#include "stg34/terrain/draw_146_model.inc"
};
static NJS_CNK_OBJECT draw_146 = {
#include "stg34/terrain/draw_146.inc"
};
static Sint16 draw_142_polygons[535] = {
#include "stg34/terrain/draw_142_polygons.inc"
};
static Sint32 draw_142_vertices[543] = {
#include "stg34/terrain/draw_142_vertices.inc"
};
static NJS_CNK_MODEL draw_142_model = {
#include "stg34/terrain/draw_142_model.inc"
};
static NJS_CNK_OBJECT draw_145 = {
#include "stg34/terrain/draw_145.inc"
};
static Sint16 draw_141_polygons[217] = {
#include "stg34/terrain/draw_141_polygons.inc"
};
static Sint32 draw_141_vertices[103] = {
#include "stg34/terrain/draw_141_vertices.inc"
};
static NJS_CNK_MODEL draw_141_model = {
#include "stg34/terrain/draw_141_model.inc"
};
static NJS_CNK_OBJECT draw_141 = {
#include "stg34/terrain/draw_141.inc"
};
static Sint16 draw_140_polygons[369] = {
#include "stg34/terrain/draw_140_polygons.inc"
};
static Sint32 draw_140_vertices[263] = {
#include "stg34/terrain/draw_140_vertices.inc"
};
static NJS_CNK_MODEL draw_140_model = {
#include "stg34/terrain/draw_140_model.inc"
};
static NJS_CNK_OBJECT draw_140 = {
#include "stg34/terrain/draw_140.inc"
};
static Sint16 draw_138_polygons[115] = {
#include "stg34/terrain/draw_138_polygons.inc"
};
static Sint32 draw_138_vertices[111] = {
#include "stg34/terrain/draw_138_vertices.inc"
};
static NJS_CNK_MODEL draw_138_model = {
#include "stg34/terrain/draw_138_model.inc"
};
static NJS_CNK_OBJECT draw_138 = {
#include "stg34/terrain/draw_138.inc"
};
static Sint16 draw_136_polygons[1063] = {
#include "stg34/terrain/draw_136_polygons.inc"
};
static Sint32 draw_136_vertices[1143] = {
#include "stg34/terrain/draw_136_vertices.inc"
};
static NJS_CNK_MODEL draw_136_model = {
#include "stg34/terrain/draw_136_model.inc"
};
static NJS_CNK_OBJECT draw_137 = {
#include "stg34/terrain/draw_137.inc"
};
static Sint16 draw_135_polygons[551] = {
#include "stg34/terrain/draw_135_polygons.inc"
};
static Sint32 draw_135_vertices[531] = {
#include "stg34/terrain/draw_135_vertices.inc"
};
static NJS_CNK_MODEL draw_135_model = {
#include "stg34/terrain/draw_135_model.inc"
};
static NJS_CNK_OBJECT draw_135 = {
#include "stg34/terrain/draw_135.inc"
};
static Sint16 draw_134_polygons[343] = {
#include "stg34/terrain/draw_134_polygons.inc"
};
static Sint32 draw_134_vertices[323] = {
#include "stg34/terrain/draw_134_vertices.inc"
};
static NJS_CNK_MODEL draw_134_model = {
#include "stg34/terrain/draw_134_model.inc"
};
static NJS_CNK_OBJECT draw_134 = {
#include "stg34/terrain/draw_134.inc"
};
static Sint16 draw_133_polygons[3789] = {
#include "stg34/terrain/draw_133_polygons.inc"
};
static Sint32 draw_133_vertices[3947] = {
#include "stg34/terrain/draw_133_vertices.inc"
};
static NJS_CNK_MODEL draw_133_model = {
#include "stg34/terrain/draw_133_model.inc"
};
static NJS_CNK_OBJECT draw_133 = {
#include "stg34/terrain/draw_133.inc"
};
static Sint16 draw_132_polygons[677] = {
#include "stg34/terrain/draw_132_polygons.inc"
};
static Sint32 draw_132_vertices[507] = {
#include "stg34/terrain/draw_132_vertices.inc"
};
static NJS_CNK_MODEL draw_132_model = {
#include "stg34/terrain/draw_132_model.inc"
};
static NJS_CNK_OBJECT draw_132 = {
#include "stg34/terrain/draw_132.inc"
};
static Sint16 draw_131_polygons[677] = {
#include "stg34/terrain/draw_131_polygons.inc"
};
static Sint32 draw_131_vertices[507] = {
#include "stg34/terrain/draw_131_vertices.inc"
};
static NJS_CNK_MODEL draw_131_model = {
#include "stg34/terrain/draw_131_model.inc"
};
static NJS_CNK_OBJECT draw_131 = {
#include "stg34/terrain/draw_131.inc"
};
static Sint16 draw_130_polygons[269] = {
#include "stg34/terrain/draw_130_polygons.inc"
};
static Sint32 draw_130_vertices[243] = {
#include "stg34/terrain/draw_130_vertices.inc"
};
static NJS_CNK_MODEL draw_130_model = {
#include "stg34/terrain/draw_130_model.inc"
};
static NJS_CNK_OBJECT draw_130 = {
#include "stg34/terrain/draw_130.inc"
};
static Sint16 draw_129_polygons[147] = {
#include "stg34/terrain/draw_129_polygons.inc"
};
static Sint32 draw_129_vertices[139] = {
#include "stg34/terrain/draw_129_vertices.inc"
};
static NJS_CNK_MODEL draw_129_model = {
#include "stg34/terrain/draw_129_model.inc"
};
static NJS_CNK_OBJECT draw_129 = {
#include "stg34/terrain/draw_129.inc"
};
static Sint16 draw_128_polygons[325] = {
#include "stg34/terrain/draw_128_polygons.inc"
};
static Sint32 draw_128_vertices[227] = {
#include "stg34/terrain/draw_128_vertices.inc"
};
static NJS_CNK_MODEL draw_128_model = {
#include "stg34/terrain/draw_128_model.inc"
};
static NJS_CNK_OBJECT draw_128 = {
#include "stg34/terrain/draw_128.inc"
};
static Sint16 draw_127_polygons[813] = {
#include "stg34/terrain/draw_127_polygons.inc"
};
static Sint32 draw_127_vertices[607] = {
#include "stg34/terrain/draw_127_vertices.inc"
};
static NJS_CNK_MODEL draw_127_model = {
#include "stg34/terrain/draw_127_model.inc"
};
static NJS_CNK_OBJECT draw_127 = {
#include "stg34/terrain/draw_127.inc"
};
static Sint16 draw_126_polygons[169] = {
#include "stg34/terrain/draw_126_polygons.inc"
};
static Sint32 draw_126_vertices[179] = {
#include "stg34/terrain/draw_126_vertices.inc"
};
static NJS_CNK_MODEL draw_126_model = {
#include "stg34/terrain/draw_126_model.inc"
};
static NJS_CNK_OBJECT draw_126 = {
#include "stg34/terrain/draw_126.inc"
};
static Sint16 draw_125_polygons[103] = {
#include "stg34/terrain/draw_125_polygons.inc"
};
static Sint32 draw_125_vertices[91] = {
#include "stg34/terrain/draw_125_vertices.inc"
};
static NJS_CNK_MODEL draw_125_model = {
#include "stg34/terrain/draw_125_model.inc"
};
static NJS_CNK_OBJECT draw_125 = {
#include "stg34/terrain/draw_125.inc"
};
static Sint16 draw_124_polygons[179] = {
#include "stg34/terrain/draw_124_polygons.inc"
};
static Sint32 draw_124_vertices[131] = {
#include "stg34/terrain/draw_124_vertices.inc"
};
static NJS_CNK_MODEL draw_124_model = {
#include "stg34/terrain/draw_124_model.inc"
};
static NJS_CNK_OBJECT draw_124 = {
#include "stg34/terrain/draw_124.inc"
};
static Sint16 draw_123_polygons[201] = {
#include "stg34/terrain/draw_123_polygons.inc"
};
static Sint32 draw_123_vertices[111] = {
#include "stg34/terrain/draw_123_vertices.inc"
};
static NJS_CNK_MODEL draw_123_model = {
#include "stg34/terrain/draw_123_model.inc"
};
static NJS_CNK_OBJECT draw_123 = {
#include "stg34/terrain/draw_123.inc"
};
static Sint16 draw_122_polygons[145] = {
#include "stg34/terrain/draw_122_polygons.inc"
};
static Sint32 draw_122_vertices[123] = {
#include "stg34/terrain/draw_122_vertices.inc"
};
static NJS_CNK_MODEL draw_122_model = {
#include "stg34/terrain/draw_122_model.inc"
};
static NJS_CNK_OBJECT draw_122 = {
#include "stg34/terrain/draw_122.inc"
};
static Sint16 draw_121_polygons[269] = {
#include "stg34/terrain/draw_121_polygons.inc"
};
static Sint32 draw_121_vertices[243] = {
#include "stg34/terrain/draw_121_vertices.inc"
};
static NJS_CNK_MODEL draw_121_model = {
#include "stg34/terrain/draw_121_model.inc"
};
static NJS_CNK_OBJECT draw_121 = {
#include "stg34/terrain/draw_121.inc"
};
static Sint16 draw_120_polygons[891] = {
#include "stg34/terrain/draw_120_polygons.inc"
};
static Sint32 draw_120_vertices[479] = {
#include "stg34/terrain/draw_120_vertices.inc"
};
static NJS_CNK_MODEL draw_120_model = {
#include "stg34/terrain/draw_120_model.inc"
};
static NJS_CNK_OBJECT draw_120 = {
#include "stg34/terrain/draw_120.inc"
};
static Sint16 draw_119_polygons[591] = {
#include "stg34/terrain/draw_119_polygons.inc"
};
static Sint32 draw_119_vertices[447] = {
#include "stg34/terrain/draw_119_vertices.inc"
};
static NJS_CNK_MODEL draw_119_model = {
#include "stg34/terrain/draw_119_model.inc"
};
static NJS_CNK_OBJECT draw_119 = {
#include "stg34/terrain/draw_119.inc"
};
static Sint16 draw_101_polygons[547] = {
#include "stg34/terrain/draw_101_polygons.inc"
};
static Sint32 draw_101_vertices[239] = {
#include "stg34/terrain/draw_101_vertices.inc"
};
static NJS_CNK_MODEL draw_101_model = {
#include "stg34/terrain/draw_101_model.inc"
};
static NJS_CNK_OBJECT draw_106 = {
#include "stg34/terrain/draw_106.inc"
};
static Sint16 draw_95_polygons[243] = {
#include "stg34/terrain/draw_95_polygons.inc"
};
static Sint32 draw_95_vertices[111] = {
#include "stg34/terrain/draw_95_vertices.inc"
};
static NJS_CNK_MODEL draw_95_model = {
#include "stg34/terrain/draw_95_model.inc"
};
static NJS_CNK_OBJECT draw_98 = {
#include "stg34/terrain/draw_98.inc"
};
static Sint16 draw_85_polygons[319] = {
#include "stg34/terrain/draw_85_polygons.inc"
};
static Sint32 draw_85_vertices[143] = {
#include "stg34/terrain/draw_85_vertices.inc"
};
static NJS_CNK_MODEL draw_85_model = {
#include "stg34/terrain/draw_85_model.inc"
};
static NJS_CNK_OBJECT draw_92 = {
#include "stg34/terrain/draw_92.inc"
};
static Sint16 draw_77_polygons[395] = {
#include "stg34/terrain/draw_77_polygons.inc"
};
static Sint32 draw_77_vertices[175] = {
#include "stg34/terrain/draw_77_vertices.inc"
};
static NJS_CNK_MODEL draw_77_model = {
#include "stg34/terrain/draw_77_model.inc"
};
static NJS_CNK_OBJECT draw_84 = {
#include "stg34/terrain/draw_84.inc"
};
static Sint16 draw_76_polygons[207] = {
#include "stg34/terrain/draw_76_polygons.inc"
};
static Sint32 draw_76_vertices[107] = {
#include "stg34/terrain/draw_76_vertices.inc"
};
static NJS_CNK_MODEL draw_76_model = {
#include "stg34/terrain/draw_76_model.inc"
};
static NJS_CNK_OBJECT draw_76 = {
#include "stg34/terrain/draw_76.inc"
};
static Sint16 draw_75_polygons[277] = {
#include "stg34/terrain/draw_75_polygons.inc"
};
static Sint32 draw_75_vertices[215] = {
#include "stg34/terrain/draw_75_vertices.inc"
};
static NJS_CNK_MODEL draw_75_model = {
#include "stg34/terrain/draw_75_model.inc"
};
static NJS_CNK_OBJECT draw_75 = {
#include "stg34/terrain/draw_75.inc"
};
static Sint16 draw_74_polygons[351] = {
#include "stg34/terrain/draw_74_polygons.inc"
};
static Sint32 draw_74_vertices[299] = {
#include "stg34/terrain/draw_74_vertices.inc"
};
static NJS_CNK_MODEL draw_74_model = {
#include "stg34/terrain/draw_74_model.inc"
};
static NJS_CNK_OBJECT draw_74 = {
#include "stg34/terrain/draw_74.inc"
};
static Sint16 draw_73_polygons[249] = {
#include "stg34/terrain/draw_73_polygons.inc"
};
static Sint32 draw_73_vertices[163] = {
#include "stg34/terrain/draw_73_vertices.inc"
};
static NJS_CNK_MODEL draw_73_model = {
#include "stg34/terrain/draw_73_model.inc"
};
static NJS_CNK_OBJECT draw_73 = {
#include "stg34/terrain/draw_73.inc"
};
static Sint16 draw_72_polygons[169] = {
#include "stg34/terrain/draw_72_polygons.inc"
};
static Sint32 draw_72_vertices[99] = {
#include "stg34/terrain/draw_72_vertices.inc"
};
static NJS_CNK_MODEL draw_72_model = {
#include "stg34/terrain/draw_72_model.inc"
};
static NJS_CNK_OBJECT draw_72 = {
#include "stg34/terrain/draw_72.inc"
};
static Sint16 draw_71_polygons[83] = {
#include "stg34/terrain/draw_71_polygons.inc"
};
static Sint32 draw_71_vertices[43] = {
#include "stg34/terrain/draw_71_vertices.inc"
};
static NJS_CNK_MODEL draw_71_model = {
#include "stg34/terrain/draw_71_model.inc"
};
static NJS_CNK_OBJECT draw_71 = {
#include "stg34/terrain/draw_71.inc"
};
static Sint16 draw_70_polygons[249] = {
#include "stg34/terrain/draw_70_polygons.inc"
};
static Sint32 draw_70_vertices[163] = {
#include "stg34/terrain/draw_70_vertices.inc"
};
static NJS_CNK_MODEL draw_70_model = {
#include "stg34/terrain/draw_70_model.inc"
};
static NJS_CNK_OBJECT draw_70 = {
#include "stg34/terrain/draw_70.inc"
};
static Sint16 draw_66_polygons[113] = {
#include "stg34/terrain/draw_66_polygons.inc"
};
static Sint32 draw_66_vertices[115] = {
#include "stg34/terrain/draw_66_vertices.inc"
};
static NJS_CNK_MODEL draw_66_model = {
#include "stg34/terrain/draw_66_model.inc"
};
static NJS_CNK_OBJECT draw_67 = {
#include "stg34/terrain/draw_67.inc"
};
static Sint16 draw_65_polygons[281] = {
#include "stg34/terrain/draw_65_polygons.inc"
};
static Sint32 draw_65_vertices[243] = {
#include "stg34/terrain/draw_65_vertices.inc"
};
static NJS_CNK_MODEL draw_65_model = {
#include "stg34/terrain/draw_65_model.inc"
};
static NJS_CNK_OBJECT draw_65 = {
#include "stg34/terrain/draw_65.inc"
};
static Sint16 draw_64_polygons[113] = {
#include "stg34/terrain/draw_64_polygons.inc"
};
static Sint32 draw_64_vertices[115] = {
#include "stg34/terrain/draw_64_vertices.inc"
};
static NJS_CNK_MODEL draw_64_model = {
#include "stg34/terrain/draw_64_model.inc"
};
static NJS_CNK_OBJECT draw_64 = {
#include "stg34/terrain/draw_64.inc"
};
static Sint16 draw_62_polygons[131] = {
#include "stg34/terrain/draw_62_polygons.inc"
};
static Sint32 draw_62_vertices[107] = {
#include "stg34/terrain/draw_62_vertices.inc"
};
static NJS_CNK_MODEL draw_62_model = {
#include "stg34/terrain/draw_62_model.inc"
};
static NJS_CNK_OBJECT draw_62 = {
#include "stg34/terrain/draw_62.inc"
};
static Sint16 draw_61_polygons[241] = {
#include "stg34/terrain/draw_61_polygons.inc"
};
static Sint32 draw_61_vertices[207] = {
#include "stg34/terrain/draw_61_vertices.inc"
};
static NJS_CNK_MODEL draw_61_model = {
#include "stg34/terrain/draw_61_model.inc"
};
static NJS_CNK_OBJECT draw_61 = {
#include "stg34/terrain/draw_61.inc"
};
static Sint16 draw_54_polygons[667] = {
#include "stg34/terrain/draw_54_polygons.inc"
};
static Sint32 draw_54_vertices[363] = {
#include "stg34/terrain/draw_54_vertices.inc"
};
static NJS_CNK_MODEL draw_54_model = {
#include "stg34/terrain/draw_54_model.inc"
};
static NJS_CNK_OBJECT draw_60 = {
#include "stg34/terrain/draw_60.inc"
};
static Sint16 draw_53_polygons[415] = {
#include "stg34/terrain/draw_53_polygons.inc"
};
static Sint32 draw_53_vertices[251] = {
#include "stg34/terrain/draw_53_vertices.inc"
};
static NJS_CNK_MODEL draw_53_model = {
#include "stg34/terrain/draw_53_model.inc"
};
static NJS_CNK_OBJECT draw_53 = {
#include "stg34/terrain/draw_53.inc"
};
static Sint16 draw_45_polygons[689] = {
#include "stg34/terrain/draw_45_polygons.inc"
};
static Sint32 draw_45_vertices[391] = {
#include "stg34/terrain/draw_45_vertices.inc"
};
static NJS_CNK_MODEL draw_45_model = {
#include "stg34/terrain/draw_45_model.inc"
};
static NJS_CNK_OBJECT draw_52 = {
#include "stg34/terrain/draw_52.inc"
};
static Sint16 draw_41_polygons[823] = {
#include "stg34/terrain/draw_41_polygons.inc"
};
static Sint32 draw_41_vertices[527] = {
#include "stg34/terrain/draw_41_vertices.inc"
};
static NJS_CNK_MODEL draw_41_model = {
#include "stg34/terrain/draw_41_model.inc"
};
static NJS_CNK_OBJECT draw_44 = {
#include "stg34/terrain/draw_44.inc"
};
static Sint16 draw_38_polygons[823] = {
#include "stg34/terrain/draw_38_polygons.inc"
};
static Sint32 draw_38_vertices[527] = {
#include "stg34/terrain/draw_38_vertices.inc"
};
static NJS_CNK_MODEL draw_38_model = {
#include "stg34/terrain/draw_38_model.inc"
};
static NJS_CNK_OBJECT draw_40 = {
#include "stg34/terrain/draw_40.inc"
};
static Sint16 draw_35_polygons[583] = {
#include "stg34/terrain/draw_35_polygons.inc"
};
static Sint32 draw_35_vertices[319] = {
#include "stg34/terrain/draw_35_vertices.inc"
};
static NJS_CNK_MODEL draw_35_model = {
#include "stg34/terrain/draw_35_model.inc"
};
static NJS_CNK_OBJECT draw_37 = {
#include "stg34/terrain/draw_37.inc"
};
static Sint16 draw_34_polygons[269] = {
#include "stg34/terrain/draw_34_polygons.inc"
};
static Sint32 draw_34_vertices[227] = {
#include "stg34/terrain/draw_34_vertices.inc"
};
static NJS_CNK_MODEL draw_34_model = {
#include "stg34/terrain/draw_34_model.inc"
};
static NJS_CNK_OBJECT draw_34 = {
#include "stg34/terrain/draw_34.inc"
};
static Sint16 draw_33_polygons[63] = {
#include "stg34/terrain/draw_33_polygons.inc"
};
static Sint32 draw_33_vertices[67] = {
#include "stg34/terrain/draw_33_vertices.inc"
};
static NJS_CNK_MODEL draw_33_model = {
#include "stg34/terrain/draw_33_model.inc"
};
static NJS_CNK_OBJECT draw_33 = {
#include "stg34/terrain/draw_33.inc"
};
static Sint16 draw_32_polygons[745] = {
#include "stg34/terrain/draw_32_polygons.inc"
};
static Sint32 draw_32_vertices[483] = {
#include "stg34/terrain/draw_32_vertices.inc"
};
static NJS_CNK_MODEL draw_32_model = {
#include "stg34/terrain/draw_32_model.inc"
};
static NJS_CNK_OBJECT draw_32 = {
#include "stg34/terrain/draw_32.inc"
};
static Sint16 draw_31_polygons[683] = {
#include "stg34/terrain/draw_31_polygons.inc"
};
static Sint32 draw_31_vertices[507] = {
#include "stg34/terrain/draw_31_vertices.inc"
};
static NJS_CNK_MODEL draw_31_model = {
#include "stg34/terrain/draw_31_model.inc"
};
static NJS_CNK_OBJECT draw_31 = {
#include "stg34/terrain/draw_31.inc"
};
static Sint16 draw_30_polygons[117] = {
#include "stg34/terrain/draw_30_polygons.inc"
};
static Sint32 draw_30_vertices[71] = {
#include "stg34/terrain/draw_30_vertices.inc"
};
static NJS_CNK_MODEL draw_30_model = {
#include "stg34/terrain/draw_30_model.inc"
};
static NJS_CNK_OBJECT draw_30 = {
#include "stg34/terrain/draw_30.inc"
};
static Sint16 draw_29_polygons[363] = {
#include "stg34/terrain/draw_29_polygons.inc"
};
static Sint32 draw_29_vertices[295] = {
#include "stg34/terrain/draw_29_vertices.inc"
};
static NJS_CNK_MODEL draw_29_model = {
#include "stg34/terrain/draw_29_model.inc"
};
static NJS_CNK_OBJECT draw_29 = {
#include "stg34/terrain/draw_29.inc"
};
static Sint16 draw_28_polygons[511] = {
#include "stg34/terrain/draw_28_polygons.inc"
};
static Sint32 draw_28_vertices[475] = {
#include "stg34/terrain/draw_28_vertices.inc"
};
static NJS_CNK_MODEL draw_28_model = {
#include "stg34/terrain/draw_28_model.inc"
};
static NJS_CNK_OBJECT draw_28 = {
#include "stg34/terrain/draw_28.inc"
};
static Sint16 draw_27_polygons[1171] = {
#include "stg34/terrain/draw_27_polygons.inc"
};
static Sint32 draw_27_vertices[611] = {
#include "stg34/terrain/draw_27_vertices.inc"
};
static NJS_CNK_MODEL draw_27_model = {
#include "stg34/terrain/draw_27_model.inc"
};
static NJS_CNK_OBJECT draw_27 = {
#include "stg34/terrain/draw_27.inc"
};
static Sint16 draw_26_polygons[159] = {
#include "stg34/terrain/draw_26_polygons.inc"
};
static Sint32 draw_26_vertices[139] = {
#include "stg34/terrain/draw_26_vertices.inc"
};
static NJS_CNK_MODEL draw_26_model = {
#include "stg34/terrain/draw_26_model.inc"
};
static NJS_CNK_OBJECT draw_26 = {
#include "stg34/terrain/draw_26.inc"
};
static Sint16 draw_25_polygons[213] = {
#include "stg34/terrain/draw_25_polygons.inc"
};
static Sint32 draw_25_vertices[179] = {
#include "stg34/terrain/draw_25_vertices.inc"
};
static NJS_CNK_MODEL draw_25_model = {
#include "stg34/terrain/draw_25_model.inc"
};
static NJS_CNK_OBJECT draw_25 = {
#include "stg34/terrain/draw_25.inc"
};
static Sint16 draw_24_polygons[415] = {
#include "stg34/terrain/draw_24_polygons.inc"
};
static Sint32 draw_24_vertices[267] = {
#include "stg34/terrain/draw_24_vertices.inc"
};
static NJS_CNK_MODEL draw_24_model = {
#include "stg34/terrain/draw_24_model.inc"
};
static NJS_CNK_OBJECT draw_24 = {
#include "stg34/terrain/draw_24.inc"
};
static Sint16 draw_23_polygons[415] = {
#include "stg34/terrain/draw_23_polygons.inc"
};
static Sint32 draw_23_vertices[267] = {
#include "stg34/terrain/draw_23_vertices.inc"
};
static NJS_CNK_MODEL draw_23_model = {
#include "stg34/terrain/draw_23_model.inc"
};
static NJS_CNK_OBJECT draw_23 = {
#include "stg34/terrain/draw_23.inc"
};
static Sint16 draw_22_polygons[415] = {
#include "stg34/terrain/draw_22_polygons.inc"
};
static Sint32 draw_22_vertices[267] = {
#include "stg34/terrain/draw_22_vertices.inc"
};
static NJS_CNK_MODEL draw_22_model = {
#include "stg34/terrain/draw_22_model.inc"
};
static NJS_CNK_OBJECT draw_22 = {
#include "stg34/terrain/draw_22.inc"
};
static Sint16 draw_21_polygons[347] = {
#include "stg34/terrain/draw_21_polygons.inc"
};
static Sint32 draw_21_vertices[267] = {
#include "stg34/terrain/draw_21_vertices.inc"
};
static NJS_CNK_MODEL draw_21_model = {
#include "stg34/terrain/draw_21_model.inc"
};
static NJS_CNK_OBJECT draw_21 = {
#include "stg34/terrain/draw_21.inc"
};
static Sint16 draw_20_polygons[347] = {
#include "stg34/terrain/draw_20_polygons.inc"
};
static Sint32 draw_20_vertices[267] = {
#include "stg34/terrain/draw_20_vertices.inc"
};
static NJS_CNK_MODEL draw_20_model = {
#include "stg34/terrain/draw_20_model.inc"
};
static NJS_CNK_OBJECT draw_20 = {
#include "stg34/terrain/draw_20.inc"
};
static Sint16 draw_19_polygons[415] = {
#include "stg34/terrain/draw_19_polygons.inc"
};
static Sint32 draw_19_vertices[267] = {
#include "stg34/terrain/draw_19_vertices.inc"
};
static NJS_CNK_MODEL draw_19_model = {
#include "stg34/terrain/draw_19_model.inc"
};
static NJS_CNK_OBJECT draw_19 = {
#include "stg34/terrain/draw_19.inc"
};
static Sint16 draw_18_polygons[415] = {
#include "stg34/terrain/draw_18_polygons.inc"
};
static Sint32 draw_18_vertices[267] = {
#include "stg34/terrain/draw_18_vertices.inc"
};
static NJS_CNK_MODEL draw_18_model = {
#include "stg34/terrain/draw_18_model.inc"
};
static NJS_CNK_OBJECT draw_18 = {
#include "stg34/terrain/draw_18.inc"
};
static Sint16 draw_17_polygons[261] = {
#include "stg34/terrain/draw_17_polygons.inc"
};
static Sint32 draw_17_vertices[207] = {
#include "stg34/terrain/draw_17_vertices.inc"
};
static NJS_CNK_MODEL draw_17_model = {
#include "stg34/terrain/draw_17_model.inc"
};
static NJS_CNK_OBJECT draw_17 = {
#include "stg34/terrain/draw_17.inc"
};
static Sint16 draw_16_polygons[213] = {
#include "stg34/terrain/draw_16_polygons.inc"
};
static Sint32 draw_16_vertices[179] = {
#include "stg34/terrain/draw_16_vertices.inc"
};
static NJS_CNK_MODEL draw_16_model = {
#include "stg34/terrain/draw_16_model.inc"
};
static NJS_CNK_OBJECT draw_16 = {
#include "stg34/terrain/draw_16.inc"
};
static Sint16 draw_15_polygons[213] = {
#include "stg34/terrain/draw_15_polygons.inc"
};
static Sint32 draw_15_vertices[179] = {
#include "stg34/terrain/draw_15_vertices.inc"
};
static NJS_CNK_MODEL draw_15_model = {
#include "stg34/terrain/draw_15_model.inc"
};
static NJS_CNK_OBJECT draw_15 = {
#include "stg34/terrain/draw_15.inc"
};
static Sint16 draw_14_polygons[213] = {
#include "stg34/terrain/draw_14_polygons.inc"
};
static Sint32 draw_14_vertices[179] = {
#include "stg34/terrain/draw_14_vertices.inc"
};
static NJS_CNK_MODEL draw_14_model = {
#include "stg34/terrain/draw_14_model.inc"
};
static NJS_CNK_OBJECT draw_14 = {
#include "stg34/terrain/draw_14.inc"
};
static Sint16 draw_13_polygons[213] = {
#include "stg34/terrain/draw_13_polygons.inc"
};
static Sint32 draw_13_vertices[179] = {
#include "stg34/terrain/draw_13_vertices.inc"
};
static NJS_CNK_MODEL draw_13_model = {
#include "stg34/terrain/draw_13_model.inc"
};
static NJS_CNK_OBJECT draw_13 = {
#include "stg34/terrain/draw_13.inc"
};
static Sint16 draw_12_polygons[415] = {
#include "stg34/terrain/draw_12_polygons.inc"
};
static Sint32 draw_12_vertices[267] = {
#include "stg34/terrain/draw_12_vertices.inc"
};
static NJS_CNK_MODEL draw_12_model = {
#include "stg34/terrain/draw_12_model.inc"
};
static NJS_CNK_OBJECT draw_12 = {
#include "stg34/terrain/draw_12.inc"
};
static Sint16 draw_11_polygons[415] = {
#include "stg34/terrain/draw_11_polygons.inc"
};
static Sint32 draw_11_vertices[267] = {
#include "stg34/terrain/draw_11_vertices.inc"
};
static NJS_CNK_MODEL draw_11_model = {
#include "stg34/terrain/draw_11_model.inc"
};
static NJS_CNK_OBJECT draw_11 = {
#include "stg34/terrain/draw_11.inc"
};
static Sint16 draw_10_polygons[415] = {
#include "stg34/terrain/draw_10_polygons.inc"
};
static Sint32 draw_10_vertices[267] = {
#include "stg34/terrain/draw_10_vertices.inc"
};
static NJS_CNK_MODEL draw_10_model = {
#include "stg34/terrain/draw_10_model.inc"
};
static NJS_CNK_OBJECT draw_10 = {
#include "stg34/terrain/draw_10.inc"
};
static Sint16 draw_9_polygons[287] = {
#include "stg34/terrain/draw_9_polygons.inc"
};
static Sint32 draw_9_vertices[239] = {
#include "stg34/terrain/draw_9_vertices.inc"
};
static NJS_CNK_MODEL draw_9_model = {
#include "stg34/terrain/draw_9_model.inc"
};
static NJS_CNK_OBJECT draw_9 = {
#include "stg34/terrain/draw_9.inc"
};
static Sint16 draw_8_polygons[523] = {
#include "stg34/terrain/draw_8_polygons.inc"
};
static Sint32 draw_8_vertices[275] = {
#include "stg34/terrain/draw_8_vertices.inc"
};
static NJS_CNK_MODEL draw_8_model = {
#include "stg34/terrain/draw_8_model.inc"
};
static NJS_CNK_OBJECT draw_8 = {
#include "stg34/terrain/draw_8.inc"
};
static Sint16 draw_7_polygons[385] = {
#include "stg34/terrain/draw_7_polygons.inc"
};
static Sint32 draw_7_vertices[255] = {
#include "stg34/terrain/draw_7_vertices.inc"
};
static NJS_CNK_MODEL draw_7_model = {
#include "stg34/terrain/draw_7_model.inc"
};
static NJS_CNK_OBJECT draw_7 = {
#include "stg34/terrain/draw_7.inc"
};
static Sint16 draw_6_polygons[141] = {
#include "stg34/terrain/draw_6_polygons.inc"
};
static Sint32 draw_6_vertices[135] = {
#include "stg34/terrain/draw_6_vertices.inc"
};
static NJS_CNK_MODEL draw_6_model = {
#include "stg34/terrain/draw_6_model.inc"
};
static NJS_CNK_OBJECT draw_6 = {
#include "stg34/terrain/draw_6.inc"
};
static Sint16 draw_5_polygons[319] = {
#include "stg34/terrain/draw_5_polygons.inc"
};
static Sint32 draw_5_vertices[283] = {
#include "stg34/terrain/draw_5_vertices.inc"
};
static NJS_CNK_MODEL draw_5_model = {
#include "stg34/terrain/draw_5_model.inc"
};
static NJS_CNK_OBJECT draw_5 = {
#include "stg34/terrain/draw_5.inc"
};
static Sint16 draw_4_polygons[327] = {
#include "stg34/terrain/draw_4_polygons.inc"
};
static Sint32 draw_4_vertices[275] = {
#include "stg34/terrain/draw_4_vertices.inc"
};
static NJS_CNK_MODEL draw_4_model = {
#include "stg34/terrain/draw_4_model.inc"
};
static NJS_CNK_OBJECT draw_4 = {
#include "stg34/terrain/draw_4.inc"
};
static Sint16 draw_3_polygons[327] = {
#include "stg34/terrain/draw_3_polygons.inc"
};
static Sint32 draw_3_vertices[259] = {
#include "stg34/terrain/draw_3_vertices.inc"
};
static NJS_CNK_MODEL draw_3_model = {
#include "stg34/terrain/draw_3_model.inc"
};
static NJS_CNK_OBJECT draw_3 = {
#include "stg34/terrain/draw_3.inc"
};
static Sint16 draw_2_polygons[291] = {
#include "stg34/terrain/draw_2_polygons.inc"
};
static Sint32 draw_2_vertices[251] = {
#include "stg34/terrain/draw_2_vertices.inc"
};
static NJS_CNK_MODEL draw_2_model = {
#include "stg34/terrain/draw_2_model.inc"
};
static NJS_CNK_OBJECT draw_2 = {
#include "stg34/terrain/draw_2.inc"
};
static Sint16 draw_1_polygons[585] = {
#include "stg34/terrain/draw_1_polygons.inc"
};
static Sint32 draw_1_vertices[451] = {
#include "stg34/terrain/draw_1_vertices.inc"
};
static NJS_CNK_MODEL draw_1_model = {
#include "stg34/terrain/draw_1_model.inc"
};
static NJS_CNK_OBJECT draw_1 = {
#include "stg34/terrain/draw_1.inc"
};
static Sint16 draw_0_polygons[665] = {
#include "stg34/terrain/draw_0_polygons.inc"
};
static Sint32 draw_0_vertices[523] = {
#include "stg34/terrain/draw_0_vertices.inc"
};
static NJS_CNK_MODEL draw_0_model = {
#include "stg34/terrain/draw_0_model.inc"
};
static NJS_CNK_OBJECT draw_0 = {
#include "stg34/terrain/draw_0.inc"
};
static NJS_OBJECT collision_9 = {
#include "stg34/terrain/collision_9.inc"
};
static NJS_OBJECT collision_10 = {
#include "stg34/terrain/collision_10.inc"
};
static NJS_OBJECT collision_11 = {
#include "stg34/terrain/collision_11.inc"
};
static NJS_OBJECT collision_15 = {
#include "stg34/terrain/collision_15.inc"
};
static NJS_OBJECT collision_21 = {
#include "stg34/terrain/collision_21.inc"
};
static NJS_OBJECT collision_22 = {
#include "stg34/terrain/collision_22.inc"
};
static NJS_OBJECT collision_26 = {
#include "stg34/terrain/collision_26.inc"
};
static NJS_OBJECT collision_31 = {
#include "stg34/terrain/collision_31.inc"
};
static NJS_OBJECT collision_33 = {
#include "stg34/terrain/collision_33.inc"
};
static NJS_OBJECT collision_42 = {
#include "stg34/terrain/collision_42.inc"
};
static NJS_OBJECT collision_47 = {
#include "stg34/terrain/collision_47.inc"
};
static NJS_OBJECT collision_48 = {
#include "stg34/terrain/collision_48.inc"
};
static NJS_OBJECT collision_49 = {
#include "stg34/terrain/collision_49.inc"
};
static NJS_OBJECT collision_53 = {
#include "stg34/terrain/collision_53.inc"
};
static NJS_OBJECT collision_55 = {
#include "stg34/terrain/collision_55.inc"
};
static NJS_OBJECT collision_56 = {
#include "stg34/terrain/collision_56.inc"
};
static NJS_OBJECT collision_57 = {
#include "stg34/terrain/collision_57.inc"
};
static NJS_OBJECT collision_58 = {
#include "stg34/terrain/collision_58.inc"
};
static NJS_OBJECT collision_59 = {
#include "stg34/terrain/collision_59.inc"
};
static NJS_OBJECT collision_60 = {
#include "stg34/terrain/collision_60.inc"
};
static NJS_OBJECT collision_61 = {
#include "stg34/terrain/collision_61.inc"
};
static NJS_OBJECT collision_63 = {
#include "stg34/terrain/collision_63.inc"
};
static NJS_OBJECT collision_64 = {
#include "stg34/terrain/collision_64.inc"
};
static NJS_OBJECT collision_65 = {
#include "stg34/terrain/collision_65.inc"
};
static NJS_OBJECT collision_66 = {
#include "stg34/terrain/collision_66.inc"
};
static NJS_OBJECT collision_67 = {
#include "stg34/terrain/collision_67.inc"
};
static NJS_OBJECT collision_68 = {
#include "stg34/terrain/collision_68.inc"
};
static NJS_OBJECT collision_69 = {
#include "stg34/terrain/collision_69.inc"
};
static NJS_OBJECT collision_71 = {
#include "stg34/terrain/collision_71.inc"
};
static NJS_OBJECT collision_72 = {
#include "stg34/terrain/collision_72.inc"
};
static NJS_OBJECT collision_73 = {
#include "stg34/terrain/collision_73.inc"
};
static NJS_OBJECT collision_75 = {
#include "stg34/terrain/collision_75.inc"
};
static NJS_OBJECT collision_76 = {
#include "stg34/terrain/collision_76.inc"
};
static NJS_OBJECT collision_78 = {
#include "stg34/terrain/collision_78.inc"
};
static NJS_OBJECT collision_79 = {
#include "stg34/terrain/collision_79.inc"
};
static NJS_OBJECT collision_84 = {
#include "stg34/terrain/collision_84.inc"
};
static NJS_OBJECT collision_85 = {
#include "stg34/terrain/collision_85.inc"
};
static NJS_OBJECT collision_86 = {
#include "stg34/terrain/collision_86.inc"
};
static NJS_OBJECT collision_87 = {
#include "stg34/terrain/collision_87.inc"
};
static NJS_OBJECT collision_89 = {
#include "stg34/terrain/collision_89.inc"
};
static NJS_OBJECT collision_95 = {
#include "stg34/terrain/collision_95.inc"
};
static NJS_OBJECT collision_96 = {
#include "stg34/terrain/collision_96.inc"
};
static NJS_OBJECT collision_97 = {
#include "stg34/terrain/collision_97.inc"
};
static NJS_OBJECT collision_98 = {
#include "stg34/terrain/collision_98.inc"
};
static NJS_OBJECT collision_99 = {
#include "stg34/terrain/collision_99.inc"
};
static NJS_OBJECT collision_100 = {
#include "stg34/terrain/collision_100.inc"
};
static NJS_OBJECT collision_101 = {
#include "stg34/terrain/collision_101.inc"
};
static NJS_OBJECT collision_103 = {
#include "stg34/terrain/collision_103.inc"
};
static NJS_OBJECT collision_104 = {
#include "stg34/terrain/collision_104.inc"
};
static NJS_OBJECT collision_105 = {
#include "stg34/terrain/collision_105.inc"
};
static NJS_OBJECT collision_106 = {
#include "stg34/terrain/collision_106.inc"
};
static NJS_OBJECT collision_107 = {
#include "stg34/terrain/collision_107.inc"
};
static NJS_OBJECT collision_108 = {
#include "stg34/terrain/collision_108.inc"
};
static NJS_OBJECT collision_109 = {
#include "stg34/terrain/collision_109.inc"
};
static NJS_OBJECT collision_111 = {
#include "stg34/terrain/collision_111.inc"
};
static NJS_OBJECT collision_112 = {
#include "stg34/terrain/collision_112.inc"
};
static NJS_OBJECT collision_113 = {
#include "stg34/terrain/collision_113.inc"
};
static NJS_OBJECT collision_115 = {
#include "stg34/terrain/collision_115.inc"
};
static NJS_OBJECT collision_116 = {
#include "stg34/terrain/collision_116.inc"
};
static NJS_OBJECT collision_118 = {
#include "stg34/terrain/collision_118.inc"
};
static NJS_OBJECT collision_119 = {
#include "stg34/terrain/collision_119.inc"
};
static NJS_CNK_OBJECT draw_156 = {
#include "stg34/terrain/draw_156.inc"
};
static NJS_CNK_OBJECT draw_154 = {
#include "stg34/terrain/draw_154.inc"
};
static NJS_CNK_OBJECT draw_153 = {
#include "stg34/terrain/draw_153.inc"
};
static NJS_CNK_OBJECT draw_151 = {
#include "stg34/terrain/draw_151.inc"
};
static NJS_CNK_OBJECT draw_150 = {
#include "stg34/terrain/draw_150.inc"
};
static NJS_CNK_OBJECT draw_148 = {
#include "stg34/terrain/draw_148.inc"
};
static NJS_CNK_OBJECT draw_147 = {
#include "stg34/terrain/draw_147.inc"
};
static NJS_CNK_OBJECT draw_144 = {
#include "stg34/terrain/draw_144.inc"
};
static NJS_CNK_OBJECT draw_143 = {
#include "stg34/terrain/draw_143.inc"
};
static NJS_CNK_OBJECT draw_142 = {
#include "stg34/terrain/draw_142.inc"
};
static NJS_CNK_OBJECT draw_139 = {
#include "stg34/terrain/draw_139.inc"
};
static NJS_CNK_OBJECT draw_136 = {
#include "stg34/terrain/draw_136.inc"
};
static NJS_CNK_OBJECT draw_118 = {
#include "stg34/terrain/draw_118.inc"
};
static NJS_CNK_OBJECT draw_117 = {
#include "stg34/terrain/draw_117.inc"
};
static NJS_CNK_OBJECT draw_116 = {
#include "stg34/terrain/draw_116.inc"
};
static NJS_CNK_OBJECT draw_115 = {
#include "stg34/terrain/draw_115.inc"
};
static NJS_CNK_OBJECT draw_114 = {
#include "stg34/terrain/draw_114.inc"
};
static NJS_CNK_OBJECT draw_113 = {
#include "stg34/terrain/draw_113.inc"
};
static NJS_CNK_OBJECT draw_112 = {
#include "stg34/terrain/draw_112.inc"
};
static NJS_CNK_OBJECT draw_111 = {
#include "stg34/terrain/draw_111.inc"
};
static NJS_CNK_OBJECT draw_110 = {
#include "stg34/terrain/draw_110.inc"
};
static NJS_CNK_OBJECT draw_109 = {
#include "stg34/terrain/draw_109.inc"
};
static NJS_CNK_OBJECT draw_108 = {
#include "stg34/terrain/draw_108.inc"
};
static NJS_CNK_OBJECT draw_107 = {
#include "stg34/terrain/draw_107.inc"
};
static NJS_CNK_OBJECT draw_105 = {
#include "stg34/terrain/draw_105.inc"
};
static NJS_CNK_OBJECT draw_104 = {
#include "stg34/terrain/draw_104.inc"
};
static NJS_CNK_OBJECT draw_103 = {
#include "stg34/terrain/draw_103.inc"
};
static NJS_CNK_OBJECT draw_102 = {
#include "stg34/terrain/draw_102.inc"
};
static NJS_CNK_OBJECT draw_101 = {
#include "stg34/terrain/draw_101.inc"
};
static NJS_CNK_OBJECT draw_100 = {
#include "stg34/terrain/draw_100.inc"
};
static NJS_CNK_OBJECT draw_99 = {
#include "stg34/terrain/draw_99.inc"
};
static NJS_CNK_OBJECT draw_97 = {
#include "stg34/terrain/draw_97.inc"
};
static NJS_CNK_OBJECT draw_96 = {
#include "stg34/terrain/draw_96.inc"
};
static NJS_CNK_OBJECT draw_95 = {
#include "stg34/terrain/draw_95.inc"
};
static NJS_CNK_OBJECT draw_94 = {
#include "stg34/terrain/draw_94.inc"
};
static NJS_CNK_OBJECT draw_93 = {
#include "stg34/terrain/draw_93.inc"
};
static NJS_CNK_OBJECT draw_91 = {
#include "stg34/terrain/draw_91.inc"
};
static NJS_CNK_OBJECT draw_90 = {
#include "stg34/terrain/draw_90.inc"
};
static NJS_CNK_OBJECT draw_89 = {
#include "stg34/terrain/draw_89.inc"
};
static NJS_CNK_OBJECT draw_88 = {
#include "stg34/terrain/draw_88.inc"
};
static NJS_CNK_OBJECT draw_87 = {
#include "stg34/terrain/draw_87.inc"
};
static NJS_CNK_OBJECT draw_86 = {
#include "stg34/terrain/draw_86.inc"
};
static NJS_CNK_OBJECT draw_85 = {
#include "stg34/terrain/draw_85.inc"
};
static NJS_CNK_OBJECT draw_83 = {
#include "stg34/terrain/draw_83.inc"
};
static NJS_CNK_OBJECT draw_82 = {
#include "stg34/terrain/draw_82.inc"
};
static NJS_CNK_OBJECT draw_81 = {
#include "stg34/terrain/draw_81.inc"
};
static NJS_CNK_OBJECT draw_80 = {
#include "stg34/terrain/draw_80.inc"
};
static NJS_CNK_OBJECT draw_79 = {
#include "stg34/terrain/draw_79.inc"
};
static NJS_CNK_OBJECT draw_78 = {
#include "stg34/terrain/draw_78.inc"
};
static NJS_CNK_OBJECT draw_77 = {
#include "stg34/terrain/draw_77.inc"
};
static NJS_CNK_OBJECT draw_69 = {
#include "stg34/terrain/draw_69.inc"
};
static NJS_CNK_OBJECT draw_68 = {
#include "stg34/terrain/draw_68.inc"
};
static NJS_CNK_OBJECT draw_66 = {
#include "stg34/terrain/draw_66.inc"
};
static NJS_CNK_OBJECT draw_63 = {
#include "stg34/terrain/draw_63.inc"
};
static NJS_CNK_OBJECT draw_59 = {
#include "stg34/terrain/draw_59.inc"
};
static NJS_CNK_OBJECT draw_58 = {
#include "stg34/terrain/draw_58.inc"
};
static NJS_CNK_OBJECT draw_57 = {
#include "stg34/terrain/draw_57.inc"
};
static NJS_CNK_OBJECT draw_56 = {
#include "stg34/terrain/draw_56.inc"
};
static NJS_CNK_OBJECT draw_55 = {
#include "stg34/terrain/draw_55.inc"
};
static NJS_CNK_OBJECT draw_54 = {
#include "stg34/terrain/draw_54.inc"
};
static NJS_CNK_OBJECT draw_51 = {
#include "stg34/terrain/draw_51.inc"
};
static NJS_CNK_OBJECT draw_50 = {
#include "stg34/terrain/draw_50.inc"
};
static NJS_CNK_OBJECT draw_49 = {
#include "stg34/terrain/draw_49.inc"
};
static NJS_CNK_OBJECT draw_48 = {
#include "stg34/terrain/draw_48.inc"
};
static NJS_CNK_OBJECT draw_47 = {
#include "stg34/terrain/draw_47.inc"
};
static NJS_CNK_OBJECT draw_46 = {
#include "stg34/terrain/draw_46.inc"
};
static NJS_CNK_OBJECT draw_45 = {
#include "stg34/terrain/draw_45.inc"
};
static NJS_CNK_OBJECT draw_43 = {
#include "stg34/terrain/draw_43.inc"
};
static NJS_CNK_OBJECT draw_42 = {
#include "stg34/terrain/draw_42.inc"
};
static NJS_CNK_OBJECT draw_41 = {
#include "stg34/terrain/draw_41.inc"
};
static NJS_CNK_OBJECT draw_39 = {
#include "stg34/terrain/draw_39.inc"
};
static NJS_CNK_OBJECT draw_38 = {
#include "stg34/terrain/draw_38.inc"
};
static NJS_CNK_OBJECT draw_36 = {
#include "stg34/terrain/draw_36.inc"
};
static NJS_CNK_OBJECT draw_35 = {
#include "stg34/terrain/draw_35.inc"
};
