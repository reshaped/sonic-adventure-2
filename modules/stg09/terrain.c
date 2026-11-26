#include <ninja.h>
#include <sg_xpt.h>

#include "collision/world.h"
#include "stg09/prison_lane.h"

/* the land's file names, which its land table and texture list point at. each
 * starts on a 4-byte boundary */
const char prison_lane_land_filenames[367] = {
#include "stg09/terrain/prison_lane_land_filenames.inc"
};

/* STG09.PRS's land, texture lists, chunk models and Basic models, from the
 * disc. its D lies between prison_lane.c and objects.c */

/* the data up to the last that a pointer names ahead of its definition, as
 * tentative definitions in the disc's order: GCC places each datum at its
 * first definition */
static struct land_entry entries[379];
struct land_table prison_lane_land;
static NJS_TEXNAME textures[38];
NJS_TEXLIST prison_lane_terrain_textures;
static Sint16 draw_232_polygons[465];
static Sint32 draw_232_vertices[339];
static NJS_CNK_MODEL draw_232_model;
static NJS_CNK_OBJECT draw_232;
static Sint16 draw_231_polygons[699];
static Sint32 draw_231_vertices[767];
static NJS_CNK_MODEL draw_231_model;
static NJS_CNK_OBJECT draw_231;
static Sint16 draw_230_polygons[375];
static Sint32 draw_230_vertices[387];
static NJS_CNK_MODEL draw_230_model;
static NJS_CNK_OBJECT draw_230;
static Sint16 draw_229_polygons[177];
static Sint32 draw_229_vertices[99];
static NJS_CNK_MODEL draw_229_model;
static NJS_CNK_OBJECT draw_229;
static Sint16 draw_225_polygons[133];
static Sint32 draw_225_vertices[67];
static NJS_CNK_MODEL draw_225_model;
static NJS_CNK_OBJECT draw_226;
static Sint16 draw_221_polygons[71];
static Sint32 draw_221_vertices[35];
static NJS_CNK_MODEL draw_221_model;
static NJS_CNK_OBJECT draw_222;
static Sint16 draw_11_polygons[359];
static Sint32 draw_11_vertices[351];
static NJS_CNK_MODEL draw_11_model;
static NJS_CNK_OBJECT draw_213;
static Sint16 draw_204_polygons[345];
static Sint32 draw_204_vertices[163];
static NJS_CNK_MODEL draw_204_model;
static NJS_CNK_OBJECT draw_204;
static Sint16 draw_85_polygons[229];
static Sint32 draw_85_vertices[107];
static NJS_CNK_MODEL draw_85_model;
static NJS_CNK_OBJECT draw_203;
static Sint16 draw_69_polygons[129];
static Sint32 draw_69_vertices[51];
static NJS_CNK_MODEL draw_69_model;
static NJS_CNK_OBJECT draw_196;
static Sint16 draw_55_polygons[105];
static Sint32 draw_55_vertices[51];
static NJS_CNK_MODEL draw_55_model;
static NJS_CNK_OBJECT draw_176;
static Sint16 draw_46_polygons[519];
static Sint32 draw_46_vertices[367];
static NJS_CNK_MODEL draw_46_model;
static NJS_CNK_OBJECT draw_168;
static Sint16 draw_44_polygons[105];
static Sint32 draw_44_vertices[59];
static NJS_CNK_MODEL draw_44_model;
static NJS_CNK_OBJECT draw_155;
static Sint16 draw_40_polygons[471];
static Sint32 draw_40_vertices[343];
static NJS_CNK_MODEL draw_40_model;
static NJS_CNK_OBJECT draw_154;
static Sint16 draw_37_polygons[23];
static Sint32 draw_37_vertices[19];
static NJS_CNK_MODEL draw_37_model;
static NJS_CNK_OBJECT draw_150;
static Sint16 draw_36_polygons[505];
static Sint32 draw_36_vertices[247];
static NJS_CNK_MODEL draw_36_model;
static NJS_CNK_OBJECT draw_148;
static Sint16 draw_34_polygons[129];
static Sint32 draw_34_vertices[115];
static NJS_CNK_MODEL draw_34_model;
static NJS_CNK_OBJECT draw_147;
static Sint16 draw_144_polygons[247];
static Sint32 draw_144_vertices[187];
static NJS_CNK_MODEL draw_144_model;
static NJS_CNK_OBJECT draw_144;
static Sint16 draw_143_polygons[95];
static Sint32 draw_143_vertices[59];
static NJS_CNK_MODEL draw_143_model;
static NJS_CNK_OBJECT draw_143;
static Sint16 draw_142_polygons[423];
static Sint32 draw_142_vertices[463];
static NJS_CNK_MODEL draw_142_model;
static NJS_CNK_OBJECT draw_142;
static Sint16 draw_141_polygons[277];
static Sint32 draw_141_vertices[179];
static NJS_CNK_MODEL draw_141_model;
static NJS_CNK_OBJECT draw_141;
static Sint16 draw_139_polygons[215];
static Sint32 draw_139_vertices[243];
static NJS_CNK_MODEL draw_139_model;
static NJS_CNK_OBJECT draw_140;
static Sint16 draw_138_polygons[121];
static Sint32 draw_138_vertices[115];
static NJS_CNK_MODEL draw_138_model;
static NJS_CNK_OBJECT draw_138;
static Sint16 draw_137_polygons[387];
static Sint32 draw_137_vertices[391];
static NJS_CNK_MODEL draw_137_model;
static NJS_CNK_OBJECT draw_137;
static Sint16 draw_136_polygons[431];
static Sint32 draw_136_vertices[327];
static NJS_CNK_MODEL draw_136_model;
static NJS_CNK_OBJECT draw_136;
static Sint16 draw_132_polygons[215];
static Sint32 draw_132_vertices[243];
static NJS_CNK_MODEL draw_132_model;
static NJS_CNK_OBJECT draw_135;
static Sint16 draw_130_polygons[121];
static Sint32 draw_130_vertices[115];
static NJS_CNK_MODEL draw_130_model;
static NJS_CNK_OBJECT draw_131;
static Sint16 draw_127_polygons[389];
static Sint32 draw_127_vertices[415];
static NJS_CNK_MODEL draw_127_model;
static NJS_CNK_OBJECT draw_129;
static Sint16 draw_126_polygons[121];
static Sint32 draw_126_vertices[115];
static NJS_CNK_MODEL draw_126_model;
static NJS_CNK_OBJECT draw_126;
static Sint16 draw_125_polygons[385];
static Sint32 draw_125_vertices[263];
static NJS_CNK_MODEL draw_125_model;
static NJS_CNK_OBJECT draw_125;
static Sint16 draw_124_polygons[167];
static Sint32 draw_124_vertices[107];
static NJS_CNK_MODEL draw_124_model;
static NJS_CNK_OBJECT draw_124;
static Sint16 draw_122_polygons[431];
static Sint32 draw_122_vertices[323];
static NJS_CNK_MODEL draw_122_model;
static NJS_CNK_OBJECT draw_123;
static Sint16 draw_121_polygons[175];
static Sint32 draw_121_vertices[107];
static NJS_CNK_MODEL draw_121_model;
static NJS_CNK_OBJECT draw_121;
static Sint16 draw_120_polygons[23];
static Sint32 draw_120_vertices[19];
static NJS_CNK_MODEL draw_120_model;
static NJS_CNK_OBJECT draw_120;
static Sint16 draw_1_polygons[571];
static Sint32 draw_1_vertices[495];
static NJS_CNK_MODEL draw_1_model;
static NJS_CNK_OBJECT draw_119;
static Sint16 draw_0_polygons[641];
static Sint32 draw_0_vertices[595];
static NJS_CNK_MODEL draw_0_model;
static NJS_CNK_OBJECT draw_117;
static Sint16 draw_18_polygons[329];
static Sint32 draw_18_vertices[303];
static NJS_CNK_MODEL draw_18_model;
static NJS_CNK_OBJECT draw_115;
static Sint16 draw_114_polygons[359];
static Sint32 draw_114_vertices[359];
static NJS_CNK_MODEL draw_114_model;
static NJS_CNK_OBJECT draw_114;
static Sint16 draw_4_polygons[85];
static Sint32 draw_4_vertices[59];
static NJS_CNK_MODEL draw_4_model;
static NJS_CNK_OBJECT draw_113;
static Sint16 draw_16_polygons[375];
static Sint32 draw_16_vertices[395];
static NJS_CNK_MODEL draw_16_model;
static NJS_CNK_OBJECT draw_112;
static Sint16 draw_15_polygons[281];
static Sint32 draw_15_vertices[187];
static NJS_CNK_MODEL draw_15_model;
static NJS_CNK_OBJECT draw_111;
static Sint16 draw_14_polygons[419];
static Sint32 draw_14_vertices[491];
static NJS_CNK_MODEL draw_14_model;
static NJS_CNK_OBJECT draw_110;
static Sint16 draw_13_polygons[147];
static Sint32 draw_13_vertices[83];
static NJS_CNK_MODEL draw_13_model;
static NJS_CNK_OBJECT draw_109;
static Sint16 draw_12_polygons[153];
static Sint32 draw_12_vertices[131];
static NJS_CNK_MODEL draw_12_model;
static NJS_CNK_OBJECT draw_108;
static Sint16 draw_99_polygons[255];
static Sint32 draw_99_vertices[147];
static NJS_CNK_MODEL draw_99_model;
static NJS_CNK_OBJECT draw_99;
static Sint16 draw_98_polygons[701];
static Sint32 draw_98_vertices[523];
static NJS_CNK_MODEL draw_98_model;
static NJS_CNK_OBJECT draw_98;
static Sint16 draw_82_polygons[1197];
static Sint32 draw_82_vertices[775];
static NJS_CNK_MODEL draw_82_model;
static NJS_CNK_OBJECT draw_84;
static Sint16 draw_66_polygons[497];
static Sint32 draw_66_vertices[315];
static NJS_CNK_MODEL draw_66_model;
static NJS_CNK_OBJECT draw_68;
static Sint16 draw_54_polygons[301];
static Sint32 draw_54_vertices[199];
static NJS_CNK_MODEL draw_54_model;
static NJS_CNK_OBJECT draw_54;
static Sint16 draw_45_polygons[507];
static Sint32 draw_45_vertices[259];
static NJS_CNK_MODEL draw_45_model;
static NJS_CNK_OBJECT draw_45;
static Sint16 draw_43_polygons[275];
static Sint32 draw_43_vertices[183];
static NJS_CNK_MODEL draw_43_model;
static NJS_CNK_OBJECT draw_43;
static Sint16 draw_39_polygons[319];
static Sint32 draw_39_vertices[251];
static NJS_CNK_MODEL draw_39_model;
static NJS_CNK_OBJECT draw_39;
static Sint16 draw_33_polygons[403];
static Sint32 draw_33_vertices[447];
static NJS_CNK_MODEL draw_33_model;
static NJS_CNK_OBJECT draw_33;
static Sint16 draw_31_polygons[189];
static Sint32 draw_31_vertices[91];
static NJS_CNK_MODEL draw_31_model;
static NJS_CNK_OBJECT draw_32;
static Sint16 draw_28_polygons[407];
static Sint32 draw_28_vertices[227];
static NJS_CNK_MODEL draw_28_model;
static NJS_CNK_OBJECT draw_28;
static Sint16 draw_27_polygons[173];
static Sint32 draw_27_vertices[71];
static NJS_CNK_MODEL draw_27_model;
static NJS_CNK_OBJECT draw_27;
static Sint16 draw_3_polygons[219];
static Sint32 draw_3_vertices[123];
static NJS_CNK_MODEL draw_3_model;
static NJS_CNK_OBJECT draw_3;
static Sint16 draw_2_polygons[403];
static Sint32 draw_2_vertices[155];
static NJS_CNK_MODEL draw_2_model;
static NJS_CNK_OBJECT draw_2;
static NJS_MATERIAL collision_0_materials[1];
static Sint16 collision_0_faces[138];
static NJS_MESHSET collision_0_meshsets[1];
static NJS_POINT3 collision_0_points[64];
static NJS_MODEL collision_0_model;
static NJS_OBJECT collision_0;
static NJS_MATERIAL collision_1_materials[1];
static Sint16 collision_1_faces[129];
static NJS_MESHSET collision_1_meshsets[1];
static NJS_POINT3 collision_1_points[66];
static NJS_MODEL collision_1_model;
static NJS_OBJECT collision_1;
static NJS_MATERIAL collision_2_materials[1];
static Sint16 collision_2_faces[375];
static NJS_MESHSET collision_2_meshsets[1];
static NJS_POINT3 collision_2_points[166];
static NJS_MODEL collision_2_model;
static NJS_OBJECT collision_2;
static NJS_MATERIAL collision_3_materials[1];
static Sint16 collision_3_faces[375];
static NJS_MESHSET collision_3_meshsets[1];
static NJS_POINT3 collision_3_points[166];
static NJS_MODEL collision_3_model;
static NJS_OBJECT collision_3;
static NJS_MATERIAL collision_4_materials[1];
static Sint16 collision_4_faces[7];
static NJS_MESHSET collision_4_meshsets[1];
static NJS_POINT3 collision_4_points[6];
static NJS_MODEL collision_4_model;
static NJS_OBJECT collision_4;
static NJS_MATERIAL collision_5_materials[1];
static Sint16 collision_5_faces[5];
static NJS_MESHSET collision_5_meshsets[1];
static NJS_POINT3 collision_5_points[4];
static NJS_MODEL collision_5_model;
static NJS_OBJECT collision_5;
static NJS_MATERIAL collision_6_materials[1];
static Sint16 collision_6_faces[12];
static NJS_MESHSET collision_6_meshsets[1];
static NJS_POINT3 collision_6_points[10];
static NJS_MODEL collision_6_model;
static NJS_OBJECT collision_6;
static NJS_MATERIAL collision_7_materials[1];
static Sint16 collision_7_faces[7];
static NJS_MESHSET collision_7_meshsets[1];
static NJS_POINT3 collision_7_points[6];
static NJS_MODEL collision_7_model;
static NJS_OBJECT collision_7;
static NJS_MATERIAL collision_8_materials[1];
static Sint16 collision_8_faces[7];
static NJS_MESHSET collision_8_meshsets[1];
static NJS_POINT3 collision_8_points[6];
static NJS_MODEL collision_8_model;
static NJS_OBJECT collision_8;
static NJS_MATERIAL collision_9_materials[1];
static Sint16 collision_9_faces[12];
static NJS_MESHSET collision_9_meshsets[1];
static NJS_POINT3 collision_9_points[10];
static NJS_MODEL collision_9_model;
static NJS_OBJECT collision_9;
static NJS_MATERIAL collision_10_materials[1];
static Sint16 collision_10_faces[10];
static NJS_MESHSET collision_10_meshsets[1];
static NJS_POINT3 collision_10_points[8];
static NJS_MODEL collision_10_model;
static NJS_OBJECT collision_10;
static NJS_MATERIAL collision_11_materials[1];
static Sint16 collision_11_faces[9];
static NJS_MESHSET collision_11_meshsets[1];
static NJS_POINT3 collision_11_points[8];
static NJS_MODEL collision_11_model;
static NJS_OBJECT collision_11;
static NJS_MATERIAL collision_12_materials[1];
static Sint16 collision_12_faces[10];
static NJS_MESHSET collision_12_meshsets[1];
static NJS_POINT3 collision_12_points[8];
static NJS_MODEL collision_12_model;
static NJS_OBJECT collision_12;
static NJS_MATERIAL collision_13_materials[1];
static Sint16 collision_13_faces[7];
static NJS_MESHSET collision_13_meshsets[1];
static NJS_POINT3 collision_13_points[6];
static NJS_MODEL collision_13_model;
static NJS_OBJECT collision_13;
static NJS_MATERIAL collision_14_materials[1];
static Sint16 collision_14_faces[5];
static NJS_MESHSET collision_14_meshsets[1];
static NJS_POINT3 collision_14_points[4];
static NJS_MODEL collision_14_model;
static NJS_OBJECT collision_14;
static NJS_MATERIAL collision_16_materials[1];
static Sint16 collision_16_faces[7];
static NJS_MESHSET collision_16_meshsets[1];
static NJS_POINT3 collision_16_points[6];
static NJS_MODEL collision_16_model;
static NJS_OBJECT collision_16;
static NJS_MATERIAL collision_17_materials[1];
static Sint16 collision_17_faces[7];
static NJS_MESHSET collision_17_meshsets[1];
static NJS_POINT3 collision_17_points[6];
static NJS_MODEL collision_17_model;
static NJS_OBJECT collision_17;
static NJS_MATERIAL collision_19_materials[1];
static Sint16 collision_19_faces[9];
static NJS_MESHSET collision_19_meshsets[1];
static NJS_POINT3 collision_19_points[8];
static NJS_MODEL collision_19_model;
static NJS_OBJECT collision_19;
static NJS_MATERIAL collision_21_materials[1];
static Sint16 collision_21_faces[43];
static NJS_MESHSET collision_21_meshsets[1];
static NJS_POINT3 collision_21_points[24];
static NJS_MODEL collision_21_model;
static NJS_OBJECT collision_21;
static NJS_MATERIAL collision_22_materials[1];
static Sint16 collision_22_faces[28];
static NJS_MESHSET collision_22_meshsets[1];
static NJS_POINT3 collision_22_points[18];
static NJS_MODEL collision_22_model;
static NJS_OBJECT collision_22;
static NJS_MATERIAL collision_23_materials[1];
static Sint16 collision_23_faces[28];
static NJS_MESHSET collision_23_meshsets[1];
static NJS_POINT3 collision_23_points[18];
static NJS_MODEL collision_23_model;
static NJS_OBJECT collision_23;
static NJS_MATERIAL collision_24_materials[1];
static Sint16 collision_24_faces[37];
static NJS_MESHSET collision_24_meshsets[1];
static NJS_POINT3 collision_24_points[21];
static NJS_MODEL collision_24_model;
static NJS_OBJECT collision_24;
static NJS_MATERIAL collision_25_materials[1];
static Sint16 collision_25_faces[37];
static NJS_MESHSET collision_25_meshsets[1];
static NJS_POINT3 collision_25_points[21];
static NJS_MODEL collision_25_model;
static NJS_OBJECT collision_25;
static NJS_MATERIAL collision_26_materials[1];
static Sint16 collision_26_faces[28];
static NJS_MESHSET collision_26_meshsets[1];
static NJS_POINT3 collision_26_points[18];
static NJS_MODEL collision_26_model;
static NJS_OBJECT collision_26;
static NJS_MATERIAL collision_27_materials[1];
static Sint16 collision_27_faces[11];
static NJS_MESHSET collision_27_meshsets[1];
static NJS_POINT3 collision_27_points[8];
static NJS_MODEL collision_27_model;
static NJS_OBJECT collision_27;
static NJS_MATERIAL collision_28_materials[1];
static Sint16 collision_28_faces[16];
static NJS_MESHSET collision_28_meshsets[1];
static NJS_POINT3 collision_28_points[8];
static NJS_MODEL collision_28_model;
static NJS_OBJECT collision_28;
static NJS_MATERIAL collision_29_materials[1];
static Sint16 collision_29_faces[21];
static NJS_MESHSET collision_29_meshsets[1];
static NJS_POINT3 collision_29_points[8];
static NJS_MODEL collision_29_model;
static NJS_OBJECT collision_29;
static NJS_MATERIAL collision_30_materials[1];
static Sint16 collision_30_faces[11];
static NJS_MESHSET collision_30_meshsets[1];
static NJS_POINT3 collision_30_points[8];
static NJS_MODEL collision_30_model;
static NJS_OBJECT collision_30;
static NJS_MATERIAL collision_33_materials[1];
static Sint16 collision_33_faces[27];
static NJS_MESHSET collision_33_meshsets[1];
static NJS_POINT3 collision_33_points[16];
static NJS_MODEL collision_33_model;
static NJS_OBJECT collision_33;
static NJS_MATERIAL collision_34_materials[1];
static Sint16 collision_34_faces[97];
static NJS_MESHSET collision_34_meshsets[1];
static NJS_POINT3 collision_34_points[36];
static NJS_MODEL collision_34_model;
static NJS_OBJECT collision_34;
static NJS_MATERIAL collision_37_materials[1];
static Sint16 collision_37_faces[33];
static NJS_MESHSET collision_37_meshsets[1];
static NJS_POINT3 collision_37_points[17];
static NJS_MODEL collision_37_model;
static NJS_OBJECT collision_37;
static NJS_MATERIAL collision_38_materials[1];
static Sint16 collision_38_faces[46];
static NJS_MESHSET collision_38_meshsets[1];
static NJS_POINT3 collision_38_points[22];
static NJS_MODEL collision_38_model;
static NJS_OBJECT collision_38;
static NJS_MATERIAL collision_39_materials[1];
static Sint16 collision_39_faces[16];
static NJS_MESHSET collision_39_meshsets[1];
static NJS_POINT3 collision_39_points[8];
static NJS_MODEL collision_39_model;
static NJS_OBJECT collision_39;
static NJS_MATERIAL collision_40_materials[1];
static Sint16 collision_40_faces[21];
static NJS_MESHSET collision_40_meshsets[1];
static NJS_POINT3 collision_40_points[8];
static NJS_MODEL collision_40_model;
static NJS_OBJECT collision_40;
static NJS_MATERIAL collision_41_materials[1];
static Sint16 collision_41_faces[19];
static NJS_MESHSET collision_41_meshsets[1];
static NJS_POINT3 collision_41_points[8];
static NJS_MODEL collision_41_model;
static NJS_OBJECT collision_41;
static NJS_MATERIAL collision_42_materials[1];
static Sint16 collision_42_faces[21];
static NJS_MESHSET collision_42_meshsets[1];
static NJS_POINT3 collision_42_points[12];
static NJS_MODEL collision_42_model;
static NJS_OBJECT collision_42;
static NJS_MATERIAL collision_46_materials[1];
static Sint16 collision_46_faces[104];
static NJS_MESHSET collision_46_meshsets[1];
static NJS_POINT3 collision_46_points[39];
static NJS_MODEL collision_46_model;
static NJS_OBJECT collision_46;
static NJS_MATERIAL collision_47_materials[1];
static Sint16 collision_47_faces[17];
static NJS_MESHSET collision_47_meshsets[1];
static NJS_POINT3 collision_47_points[8];
static NJS_MODEL collision_47_model;
static NJS_OBJECT collision_47;
static NJS_MATERIAL collision_48_materials[1];
static Sint16 collision_48_faces[16];
static NJS_MESHSET collision_48_meshsets[1];
static NJS_POINT3 collision_48_points[8];
static NJS_MODEL collision_48_model;
static NJS_OBJECT collision_48;
static NJS_MATERIAL collision_52_materials[1];
static Sint16 collision_52_faces[19];
static NJS_MESHSET collision_52_meshsets[1];
static NJS_POINT3 collision_52_points[8];
static NJS_MODEL collision_52_model;
static NJS_OBJECT collision_52;
static NJS_MATERIAL collision_53_materials[1];
static Sint16 collision_53_faces[33];
static NJS_MESHSET collision_53_meshsets[1];
static NJS_POINT3 collision_53_points[12];
static NJS_MODEL collision_53_model;
static NJS_OBJECT collision_53;
static NJS_MATERIAL collision_54_materials[1];
static Sint16 collision_54_faces[19];
static NJS_MESHSET collision_54_meshsets[1];
static NJS_POINT3 collision_54_points[8];
static NJS_MODEL collision_54_model;
static NJS_OBJECT collision_54;
static NJS_MATERIAL collision_55_materials[1];
static Sint16 collision_55_faces[19];
static NJS_MESHSET collision_55_meshsets[1];
static NJS_POINT3 collision_55_points[8];
static NJS_MODEL collision_55_model;
static NJS_OBJECT collision_55;
static NJS_MATERIAL collision_56_materials[1];
static Sint16 collision_56_faces[11];
static NJS_MESHSET collision_56_meshsets[1];
static NJS_POINT3 collision_56_points[8];
static NJS_MODEL collision_56_model;
static NJS_OBJECT collision_56;
static NJS_MATERIAL collision_58_materials[1];
static Sint16 collision_58_faces[14];
static NJS_MESHSET collision_58_meshsets[1];
static NJS_POINT3 collision_58_points[8];
static NJS_MODEL collision_58_model;
static NJS_OBJECT collision_58;
static NJS_MATERIAL collision_59_materials[1];
static Sint16 collision_59_faces[46];
static NJS_MESHSET collision_59_meshsets[1];
static NJS_POINT3 collision_59_points[21];
static NJS_MODEL collision_59_model;
static NJS_OBJECT collision_59;
static NJS_MATERIAL collision_64_materials[1];
static Sint16 collision_64_faces[11];
static NJS_MESHSET collision_64_meshsets[1];
static NJS_POINT3 collision_64_points[8];
static NJS_MODEL collision_64_model;
static NJS_OBJECT collision_64;
static NJS_MATERIAL collision_65_materials[1];
static Sint16 collision_65_faces[19];
static NJS_MESHSET collision_65_meshsets[1];
static NJS_POINT3 collision_65_points[8];
static NJS_MODEL collision_65_model;
static NJS_OBJECT collision_65;
static NJS_MATERIAL collision_66_materials[1];
static Sint16 collision_66_faces[11];
static NJS_MESHSET collision_66_meshsets[1];
static NJS_POINT3 collision_66_points[8];
static NJS_MODEL collision_66_model;
static NJS_OBJECT collision_66;
static NJS_MATERIAL collision_67_materials[1];
static Sint16 collision_67_faces[61];
static NJS_MESHSET collision_67_meshsets[1];
static NJS_POINT3 collision_67_points[27];
static NJS_MODEL collision_67_model;
static NJS_OBJECT collision_67;
static NJS_MATERIAL collision_68_materials[1];
static Sint16 collision_68_faces[27];
static NJS_MESHSET collision_68_meshsets[1];
static NJS_POINT3 collision_68_points[16];
static NJS_MODEL collision_68_model;
static NJS_OBJECT collision_68;
static NJS_MATERIAL collision_69_materials[1];
static Sint16 collision_69_faces[27];
static NJS_MESHSET collision_69_meshsets[1];
static NJS_POINT3 collision_69_points[14];
static NJS_MODEL collision_69_model;
static NJS_OBJECT collision_69;
static NJS_MATERIAL collision_70_materials[1];
static Sint16 collision_70_faces[27];
static NJS_MESHSET collision_70_meshsets[1];
static NJS_POINT3 collision_70_points[14];
static NJS_MODEL collision_70_model;
static NJS_OBJECT collision_70;
static NJS_MATERIAL collision_71_materials[1];
static Sint16 collision_71_faces[28];
static NJS_MESHSET collision_71_meshsets[1];
static NJS_POINT3 collision_71_points[17];
static NJS_MODEL collision_71_model;
static NJS_OBJECT collision_71;
static NJS_MATERIAL collision_72_materials[1];
static Sint16 collision_72_faces[23];
static NJS_MESHSET collision_72_meshsets[1];
static NJS_POINT3 collision_72_points[16];
static NJS_MODEL collision_72_model;
static NJS_OBJECT collision_72;
static NJS_MATERIAL collision_73_materials[1];
static Sint16 collision_73_faces[21];
static NJS_MESHSET collision_73_meshsets[1];
static NJS_POINT3 collision_73_points[8];
static NJS_MODEL collision_73_model;
static NJS_OBJECT collision_73;
static NJS_MATERIAL collision_74_materials[1];
static Sint16 collision_74_faces[33];
static NJS_MESHSET collision_74_meshsets[1];
static NJS_POINT3 collision_74_points[12];
static NJS_MODEL collision_74_model;
static NJS_OBJECT collision_74;
static NJS_MATERIAL collision_75_materials[1];
static Sint16 collision_75_faces[25];
static NJS_MESHSET collision_75_meshsets[1];
static NJS_POINT3 collision_75_points[15];
static NJS_MODEL collision_75_model;
static NJS_OBJECT collision_75;
static NJS_MATERIAL collision_76_materials[1];
static Sint16 collision_76_faces[18];
static NJS_MESHSET collision_76_meshsets[1];
static NJS_POINT3 collision_76_points[16];
static NJS_MODEL collision_76_model;
static NJS_OBJECT collision_76;
static NJS_MATERIAL collision_77_materials[1];
static Sint16 collision_77_faces[23];
static NJS_MESHSET collision_77_meshsets[1];
static NJS_POINT3 collision_77_points[12];
static NJS_MODEL collision_77_model;
static NJS_OBJECT collision_77;
static NJS_MATERIAL collision_78_materials[1];
static Sint16 collision_78_faces[33];
static NJS_MESHSET collision_78_meshsets[1];
static NJS_POINT3 collision_78_points[16];
static NJS_MODEL collision_78_model;
static NJS_OBJECT collision_78;
static NJS_MATERIAL collision_79_materials[1];
static Sint16 collision_79_faces[14];
static NJS_MESHSET collision_79_meshsets[1];
static NJS_POINT3 collision_79_points[8];
static NJS_MODEL collision_79_model;
static NJS_OBJECT collision_79;
static NJS_MATERIAL collision_80_materials[1];
static Sint16 collision_80_faces[20];
static NJS_MESHSET collision_80_meshsets[1];
static NJS_POINT3 collision_80_points[16];
static NJS_MODEL collision_80_model;
static NJS_OBJECT collision_80;
static NJS_MATERIAL collision_81_materials[1];
static Sint16 collision_81_faces[36];
static NJS_MESHSET collision_81_meshsets[1];
static NJS_POINT3 collision_81_points[16];
static NJS_MODEL collision_81_model;
static NJS_OBJECT collision_81;
static NJS_MATERIAL collision_82_materials[1];
static Sint16 collision_82_faces[11];
static NJS_MESHSET collision_82_meshsets[1];
static NJS_POINT3 collision_82_points[8];
static NJS_MODEL collision_82_model;
static NJS_OBJECT collision_82;
static NJS_MATERIAL collision_83_materials[1];
static Sint16 collision_83_faces[33];
static NJS_MESHSET collision_83_meshsets[1];
static NJS_POINT3 collision_83_points[16];
static NJS_MODEL collision_83_model;
static NJS_OBJECT collision_83;
static NJS_MATERIAL collision_84_materials[1];
static Sint16 collision_84_faces[21];
static NJS_MESHSET collision_84_meshsets[1];
static NJS_POINT3 collision_84_points[8];
static NJS_MODEL collision_84_model;
static NJS_OBJECT collision_84;
static NJS_MATERIAL collision_85_materials[1];
static Sint16 collision_85_faces[21];
static NJS_MESHSET collision_85_meshsets[1];
static NJS_POINT3 collision_85_points[8];
static NJS_MODEL collision_85_model;
static NJS_OBJECT collision_85;
static NJS_MATERIAL collision_86_materials[1];
static Sint16 collision_86_faces[29];
static NJS_MESHSET collision_86_meshsets[1];
static NJS_POINT3 collision_86_points[22];
static NJS_MODEL collision_86_model;
static NJS_OBJECT collision_86;
static NJS_MATERIAL collision_88_materials[1];
static Sint16 collision_88_faces[36];
static NJS_MESHSET collision_88_meshsets[1];
static NJS_POINT3 collision_88_points[16];
static NJS_MODEL collision_88_model;
static NJS_OBJECT collision_88;
static NJS_MATERIAL collision_89_materials[1];
static Sint16 collision_89_faces[11];
static NJS_MESHSET collision_89_meshsets[1];
static NJS_POINT3 collision_89_points[8];
static NJS_MODEL collision_89_model;
static NJS_OBJECT collision_89;
static NJS_MATERIAL collision_90_materials[1];
static Sint16 collision_90_faces[39];
static NJS_MESHSET collision_90_meshsets[1];
static NJS_POINT3 collision_90_points[18];
static NJS_MODEL collision_90_model;
static NJS_OBJECT collision_90;
static NJS_MATERIAL collision_91_materials[1];
static Sint16 collision_91_faces[19];
static NJS_MESHSET collision_91_meshsets[1];
static NJS_POINT3 collision_91_points[8];
static NJS_MODEL collision_91_model;
static NJS_OBJECT collision_91;
static NJS_MATERIAL collision_92_materials[1];
static Sint16 collision_92_faces[14];
static NJS_MESHSET collision_92_meshsets[1];
static NJS_POINT3 collision_92_points[8];
static NJS_MODEL collision_92_model;
static NJS_OBJECT collision_92;
static NJS_MATERIAL collision_93_materials[1];
static Sint16 collision_93_faces[27];
static NJS_MESHSET collision_93_meshsets[1];
static NJS_POINT3 collision_93_points[16];
static NJS_MODEL collision_93_model;
static NJS_OBJECT collision_93;
static NJS_MATERIAL collision_94_materials[1];
static Sint16 collision_94_faces[27];
static NJS_MESHSET collision_94_meshsets[1];
static NJS_POINT3 collision_94_points[16];
static NJS_MODEL collision_94_model;
static NJS_OBJECT collision_94;
static NJS_MATERIAL collision_95_materials[1];
static Sint16 collision_95_faces[32];
static NJS_MESHSET collision_95_meshsets[1];
static NJS_POINT3 collision_95_points[16];
static NJS_MODEL collision_95_model;
static NJS_OBJECT collision_95;
static NJS_MATERIAL collision_96_materials[1];
static Sint16 collision_96_faces[18];
static NJS_MESHSET collision_96_meshsets[1];
static NJS_POINT3 collision_96_points[16];
static NJS_MODEL collision_96_model;
static NJS_OBJECT collision_96;
static NJS_MATERIAL collision_97_materials[1];
static Sint16 collision_97_faces[25];
static NJS_MESHSET collision_97_meshsets[1];
static NJS_POINT3 collision_97_points[12];
static NJS_MODEL collision_97_model;
static NJS_OBJECT collision_97;
static NJS_MATERIAL collision_98_materials[1];
static Sint16 collision_98_faces[33];
static NJS_MESHSET collision_98_meshsets[1];
static NJS_POINT3 collision_98_points[16];
static NJS_MODEL collision_98_model;
static NJS_OBJECT collision_98;
static NJS_MATERIAL collision_99_materials[1];
static Sint16 collision_99_faces[22];
static NJS_MESHSET collision_99_meshsets[1];
static NJS_POINT3 collision_99_points[14];
static NJS_MODEL collision_99_model;
static NJS_OBJECT collision_99;
static NJS_MATERIAL collision_100_materials[1];
static Sint16 collision_100_faces[22];
static NJS_MESHSET collision_100_meshsets[1];
static NJS_POINT3 collision_100_points[14];
static NJS_MODEL collision_100_model;
static NJS_OBJECT collision_100;
static NJS_MATERIAL collision_101_materials[1];
static Sint16 collision_101_faces[29];
static NJS_MESHSET collision_101_meshsets[1];
static NJS_POINT3 collision_101_points[14];
static NJS_MODEL collision_101_model;
static NJS_OBJECT collision_101;
static NJS_MATERIAL collision_102_materials[1];
static Sint16 collision_102_faces[28];
static NJS_MESHSET collision_102_meshsets[1];
static NJS_POINT3 collision_102_points[14];
static NJS_MODEL collision_102_model;
static NJS_OBJECT collision_102;
static NJS_MATERIAL collision_103_materials[1];
static Sint16 collision_103_faces[22];
static NJS_MESHSET collision_103_meshsets[1];
static NJS_POINT3 collision_103_points[14];
static NJS_MODEL collision_103_model;
static NJS_OBJECT collision_103;
static NJS_MATERIAL collision_104_materials[1];
static Sint16 collision_104_faces[21];
static NJS_MESHSET collision_104_meshsets[1];
static NJS_POINT3 collision_104_points[8];
static NJS_MODEL collision_104_model;
static NJS_OBJECT collision_104;
static NJS_MATERIAL collision_105_materials[1];
static Sint16 collision_105_faces[21];
static NJS_MESHSET collision_105_meshsets[1];
static NJS_POINT3 collision_105_points[8];
static NJS_MODEL collision_105_model;
static NJS_OBJECT collision_105;
static NJS_MATERIAL collision_106_materials[1];
static Sint16 collision_106_faces[36];
static NJS_MESHSET collision_106_meshsets[1];
static NJS_POINT3 collision_106_points[16];
static NJS_MODEL collision_106_model;
static NJS_OBJECT collision_106;
static NJS_MATERIAL collision_108_materials[1];
static Sint16 collision_108_faces[11];
static NJS_MESHSET collision_108_meshsets[1];
static NJS_POINT3 collision_108_points[8];
static NJS_MODEL collision_108_model;
static NJS_OBJECT collision_108;
static NJS_MATERIAL collision_109_materials[1];
static Sint16 collision_109_faces[36];
static NJS_MESHSET collision_109_meshsets[1];
static NJS_POINT3 collision_109_points[20];
static NJS_MODEL collision_109_model;
static NJS_OBJECT collision_109;
static NJS_MATERIAL collision_110_materials[1];
static Sint16 collision_110_faces[78];
static NJS_MESHSET collision_110_meshsets[1];
static NJS_POINT3 collision_110_points[31];
static NJS_MODEL collision_110_model;
static NJS_OBJECT collision_110;
static NJS_MATERIAL collision_111_materials[1];
static Sint16 collision_111_faces[104];
static NJS_MESHSET collision_111_meshsets[1];
static NJS_POINT3 collision_111_points[40];
static NJS_MODEL collision_111_model;
static NJS_OBJECT collision_111;
static NJS_MATERIAL collision_112_materials[1];
static Sint16 collision_112_faces[14];
static NJS_MESHSET collision_112_meshsets[1];
static NJS_POINT3 collision_112_points[8];
static NJS_MODEL collision_112_model;
static NJS_OBJECT collision_112;
static NJS_MATERIAL collision_137_materials[1];
static Sint16 collision_137_faces[19];
static NJS_MESHSET collision_137_meshsets[1];
static NJS_POINT3 collision_137_points[8];
static NJS_MODEL collision_137_model;
static NJS_OBJECT collision_137;
static NJS_MATERIAL collision_138_materials[1];
static Sint16 collision_138_faces[21];
static NJS_MESHSET collision_138_meshsets[1];
static NJS_POINT3 collision_138_points[12];
static NJS_MODEL collision_138_model;
static NJS_OBJECT collision_138;
static NJS_CNK_OBJECT draw_239;
static NJS_CNK_OBJECT draw_238;
static NJS_CNK_OBJECT draw_237;
static NJS_CNK_OBJECT draw_236;
static NJS_CNK_OBJECT draw_235;
static NJS_CNK_OBJECT draw_234;
static NJS_CNK_OBJECT draw_233;
static NJS_CNK_OBJECT draw_228;
static NJS_CNK_OBJECT draw_227;
static NJS_CNK_OBJECT draw_225;
static NJS_CNK_OBJECT draw_224;
static NJS_CNK_OBJECT draw_223;
static NJS_CNK_OBJECT draw_221;
static NJS_CNK_OBJECT draw_220;
static NJS_CNK_OBJECT draw_219;
static NJS_CNK_OBJECT draw_218;
static NJS_CNK_OBJECT draw_217;
static NJS_CNK_OBJECT draw_216;
static NJS_CNK_OBJECT draw_215;
static NJS_CNK_OBJECT draw_214;
static NJS_CNK_OBJECT draw_212;
static NJS_CNK_OBJECT draw_211;
static NJS_CNK_OBJECT draw_210;
static NJS_CNK_OBJECT draw_209;
static NJS_CNK_OBJECT draw_208;
static NJS_CNK_OBJECT draw_207;
static NJS_CNK_OBJECT draw_206;
static NJS_CNK_OBJECT draw_205;
static NJS_CNK_OBJECT draw_202;
static NJS_CNK_OBJECT draw_201;
static NJS_CNK_OBJECT draw_200;
static NJS_CNK_OBJECT draw_199;
static NJS_CNK_OBJECT draw_198;
static NJS_CNK_OBJECT draw_197;
static NJS_CNK_OBJECT draw_195;
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
static NJS_CNK_OBJECT draw_175;
static NJS_CNK_OBJECT draw_174;
static NJS_CNK_OBJECT draw_173;
static NJS_CNK_OBJECT draw_172;
static NJS_CNK_OBJECT draw_171;
static NJS_CNK_OBJECT draw_170;
static NJS_CNK_OBJECT draw_169;
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
static NJS_CNK_OBJECT draw_153;
static NJS_CNK_OBJECT draw_152;
static NJS_CNK_OBJECT draw_151;
static NJS_CNK_OBJECT draw_149;
static NJS_CNK_OBJECT draw_146;
static NJS_CNK_OBJECT draw_145;
static NJS_CNK_OBJECT draw_139;
static NJS_CNK_OBJECT draw_134;
static NJS_CNK_OBJECT draw_133;
static NJS_CNK_OBJECT draw_132;
static NJS_CNK_OBJECT draw_130;
static NJS_CNK_OBJECT draw_128;
static NJS_CNK_OBJECT draw_127;
static NJS_CNK_OBJECT draw_122;
static NJS_CNK_OBJECT draw_118;
static NJS_CNK_OBJECT draw_116;
static NJS_CNK_OBJECT draw_107;
static NJS_CNK_OBJECT draw_106;
static NJS_CNK_OBJECT draw_105;
static NJS_CNK_OBJECT draw_104;
static NJS_CNK_OBJECT draw_103;
static NJS_CNK_OBJECT draw_102;
static NJS_CNK_OBJECT draw_101;
static NJS_CNK_OBJECT draw_100;
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
static NJS_CNK_OBJECT draw_72;
static NJS_CNK_OBJECT draw_71;
static NJS_CNK_OBJECT draw_70;
static NJS_CNK_OBJECT draw_69;
static NJS_CNK_OBJECT draw_67;
static NJS_CNK_OBJECT draw_66;
static NJS_CNK_OBJECT draw_65;
static NJS_CNK_OBJECT draw_64;
static NJS_CNK_OBJECT draw_63;
static NJS_CNK_OBJECT draw_62;
static NJS_CNK_OBJECT draw_61;
static NJS_CNK_OBJECT draw_60;
static NJS_CNK_OBJECT draw_59;
static NJS_CNK_OBJECT draw_58;
static NJS_CNK_OBJECT draw_57;
static NJS_CNK_OBJECT draw_56;
static NJS_CNK_OBJECT draw_55;
static NJS_CNK_OBJECT draw_53;
static NJS_CNK_OBJECT draw_52;
static NJS_CNK_OBJECT draw_51;
static NJS_CNK_OBJECT draw_50;
static NJS_CNK_OBJECT draw_49;
static NJS_CNK_OBJECT draw_48;
static NJS_CNK_OBJECT draw_47;
static NJS_CNK_OBJECT draw_46;
static NJS_CNK_OBJECT draw_44;
static NJS_CNK_OBJECT draw_42;
static NJS_CNK_OBJECT draw_41;
static NJS_CNK_OBJECT draw_40;
static NJS_CNK_OBJECT draw_38;
static NJS_CNK_OBJECT draw_37;
static NJS_CNK_OBJECT draw_36;
static NJS_CNK_OBJECT draw_35;
static NJS_CNK_OBJECT draw_34;
static NJS_CNK_OBJECT draw_31;
static NJS_CNK_OBJECT draw_30;
static NJS_CNK_OBJECT draw_29;
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
static NJS_CNK_OBJECT draw_1;
static NJS_CNK_OBJECT draw_0;
static NJS_OBJECT collision_15;
static NJS_OBJECT collision_18;
static NJS_OBJECT collision_20;
static NJS_OBJECT collision_31;
static NJS_OBJECT collision_32;
static NJS_OBJECT collision_35;
static NJS_OBJECT collision_36;
static NJS_OBJECT collision_43;
static NJS_OBJECT collision_44;
static NJS_OBJECT collision_45;
static NJS_OBJECT collision_49;
static NJS_OBJECT collision_50;
static NJS_OBJECT collision_51;
static NJS_OBJECT collision_57;
static NJS_OBJECT collision_60;
static NJS_OBJECT collision_61;
static NJS_OBJECT collision_62;
static NJS_OBJECT collision_63;
static NJS_OBJECT collision_87;
static NJS_OBJECT collision_107;
static NJS_OBJECT collision_113;
static NJS_OBJECT collision_114;
static NJS_OBJECT collision_115;
static NJS_OBJECT collision_116;
static NJS_OBJECT collision_117;
static NJS_OBJECT collision_118;
static NJS_OBJECT collision_119;
static NJS_OBJECT collision_120;
static NJS_OBJECT collision_121;
static NJS_OBJECT collision_122;
static NJS_OBJECT collision_123;
static NJS_OBJECT collision_124;
static NJS_OBJECT collision_125;
static NJS_OBJECT collision_126;
static NJS_OBJECT collision_127;
static NJS_OBJECT collision_128;
static NJS_OBJECT collision_129;
static NJS_OBJECT collision_130;
static NJS_OBJECT collision_131;
static NJS_OBJECT collision_132;
static NJS_OBJECT collision_133;
static NJS_OBJECT collision_134;
static NJS_OBJECT collision_135;
static NJS_OBJECT collision_136;

static struct land_entry entries[379] = {
#include "stg09/terrain/entries.inc"
};
struct land_table prison_lane_land = {
#include "stg09/terrain/prison_lane_land.inc"
};
static NJS_TEXNAME textures[38] = {
#include "stg09/terrain/textures.inc"
};
NJS_TEXLIST prison_lane_terrain_textures = {
#include "stg09/terrain/prison_lane_terrain_textures.inc"
};
static Sint16 draw_232_polygons[465] = {
#include "stg09/terrain/draw_232_polygons.inc"
};
static Sint32 draw_232_vertices[339] = {
#include "stg09/terrain/draw_232_vertices.inc"
};
static NJS_CNK_MODEL draw_232_model = {
#include "stg09/terrain/draw_232_model.inc"
};
static NJS_CNK_OBJECT draw_232 = {
#include "stg09/terrain/draw_232.inc"
};
static Sint16 draw_231_polygons[699] = {
#include "stg09/terrain/draw_231_polygons.inc"
};
static Sint32 draw_231_vertices[767] = {
#include "stg09/terrain/draw_231_vertices.inc"
};
static NJS_CNK_MODEL draw_231_model = {
#include "stg09/terrain/draw_231_model.inc"
};
static NJS_CNK_OBJECT draw_231 = {
#include "stg09/terrain/draw_231.inc"
};
static Sint16 draw_230_polygons[375] = {
#include "stg09/terrain/draw_230_polygons.inc"
};
static Sint32 draw_230_vertices[387] = {
#include "stg09/terrain/draw_230_vertices.inc"
};
static NJS_CNK_MODEL draw_230_model = {
#include "stg09/terrain/draw_230_model.inc"
};
static NJS_CNK_OBJECT draw_230 = {
#include "stg09/terrain/draw_230.inc"
};
static Sint16 draw_229_polygons[177] = {
#include "stg09/terrain/draw_229_polygons.inc"
};
static Sint32 draw_229_vertices[99] = {
#include "stg09/terrain/draw_229_vertices.inc"
};
static NJS_CNK_MODEL draw_229_model = {
#include "stg09/terrain/draw_229_model.inc"
};
static NJS_CNK_OBJECT draw_229 = {
#include "stg09/terrain/draw_229.inc"
};
static Sint16 draw_225_polygons[133] = {
#include "stg09/terrain/draw_225_polygons.inc"
};
static Sint32 draw_225_vertices[67] = {
#include "stg09/terrain/draw_225_vertices.inc"
};
static NJS_CNK_MODEL draw_225_model = {
#include "stg09/terrain/draw_225_model.inc"
};
static NJS_CNK_OBJECT draw_226 = {
#include "stg09/terrain/draw_226.inc"
};
static Sint16 draw_221_polygons[71] = {
#include "stg09/terrain/draw_221_polygons.inc"
};
static Sint32 draw_221_vertices[35] = {
#include "stg09/terrain/draw_221_vertices.inc"
};
static NJS_CNK_MODEL draw_221_model = {
#include "stg09/terrain/draw_221_model.inc"
};
static NJS_CNK_OBJECT draw_222 = {
#include "stg09/terrain/draw_222.inc"
};
static Sint16 draw_11_polygons[359] = {
#include "stg09/terrain/draw_11_polygons.inc"
};
static Sint32 draw_11_vertices[351] = {
#include "stg09/terrain/draw_11_vertices.inc"
};
static NJS_CNK_MODEL draw_11_model = {
#include "stg09/terrain/draw_11_model.inc"
};
static NJS_CNK_OBJECT draw_213 = {
#include "stg09/terrain/draw_213.inc"
};
static Sint16 draw_204_polygons[345] = {
#include "stg09/terrain/draw_204_polygons.inc"
};
static Sint32 draw_204_vertices[163] = {
#include "stg09/terrain/draw_204_vertices.inc"
};
static NJS_CNK_MODEL draw_204_model = {
#include "stg09/terrain/draw_204_model.inc"
};
static NJS_CNK_OBJECT draw_204 = {
#include "stg09/terrain/draw_204.inc"
};
static Sint16 draw_85_polygons[229] = {
#include "stg09/terrain/draw_85_polygons.inc"
};
static Sint32 draw_85_vertices[107] = {
#include "stg09/terrain/draw_85_vertices.inc"
};
static NJS_CNK_MODEL draw_85_model = {
#include "stg09/terrain/draw_85_model.inc"
};
static NJS_CNK_OBJECT draw_203 = {
#include "stg09/terrain/draw_203.inc"
};
static Sint16 draw_69_polygons[129] = {
#include "stg09/terrain/draw_69_polygons.inc"
};
static Sint32 draw_69_vertices[51] = {
#include "stg09/terrain/draw_69_vertices.inc"
};
static NJS_CNK_MODEL draw_69_model = {
#include "stg09/terrain/draw_69_model.inc"
};
static NJS_CNK_OBJECT draw_196 = {
#include "stg09/terrain/draw_196.inc"
};
static Sint16 draw_55_polygons[105] = {
#include "stg09/terrain/draw_55_polygons.inc"
};
static Sint32 draw_55_vertices[51] = {
#include "stg09/terrain/draw_55_vertices.inc"
};
static NJS_CNK_MODEL draw_55_model = {
#include "stg09/terrain/draw_55_model.inc"
};
static NJS_CNK_OBJECT draw_176 = {
#include "stg09/terrain/draw_176.inc"
};
static Sint16 draw_46_polygons[519] = {
#include "stg09/terrain/draw_46_polygons.inc"
};
static Sint32 draw_46_vertices[367] = {
#include "stg09/terrain/draw_46_vertices.inc"
};
static NJS_CNK_MODEL draw_46_model = {
#include "stg09/terrain/draw_46_model.inc"
};
static NJS_CNK_OBJECT draw_168 = {
#include "stg09/terrain/draw_168.inc"
};
static Sint16 draw_44_polygons[105] = {
#include "stg09/terrain/draw_44_polygons.inc"
};
static Sint32 draw_44_vertices[59] = {
#include "stg09/terrain/draw_44_vertices.inc"
};
static NJS_CNK_MODEL draw_44_model = {
#include "stg09/terrain/draw_44_model.inc"
};
static NJS_CNK_OBJECT draw_155 = {
#include "stg09/terrain/draw_155.inc"
};
static Sint16 draw_40_polygons[471] = {
#include "stg09/terrain/draw_40_polygons.inc"
};
static Sint32 draw_40_vertices[343] = {
#include "stg09/terrain/draw_40_vertices.inc"
};
static NJS_CNK_MODEL draw_40_model = {
#include "stg09/terrain/draw_40_model.inc"
};
static NJS_CNK_OBJECT draw_154 = {
#include "stg09/terrain/draw_154.inc"
};
static Sint16 draw_37_polygons[23] = {
#include "stg09/terrain/draw_37_polygons.inc"
};
static Sint32 draw_37_vertices[19] = {
#include "stg09/terrain/draw_37_vertices.inc"
};
static NJS_CNK_MODEL draw_37_model = {
#include "stg09/terrain/draw_37_model.inc"
};
static NJS_CNK_OBJECT draw_150 = {
#include "stg09/terrain/draw_150.inc"
};
static Sint16 draw_36_polygons[505] = {
#include "stg09/terrain/draw_36_polygons.inc"
};
static Sint32 draw_36_vertices[247] = {
#include "stg09/terrain/draw_36_vertices.inc"
};
static NJS_CNK_MODEL draw_36_model = {
#include "stg09/terrain/draw_36_model.inc"
};
static NJS_CNK_OBJECT draw_148 = {
#include "stg09/terrain/draw_148.inc"
};
static Sint16 draw_34_polygons[129] = {
#include "stg09/terrain/draw_34_polygons.inc"
};
static Sint32 draw_34_vertices[115] = {
#include "stg09/terrain/draw_34_vertices.inc"
};
static NJS_CNK_MODEL draw_34_model = {
#include "stg09/terrain/draw_34_model.inc"
};
static NJS_CNK_OBJECT draw_147 = {
#include "stg09/terrain/draw_147.inc"
};
static Sint16 draw_144_polygons[247] = {
#include "stg09/terrain/draw_144_polygons.inc"
};
static Sint32 draw_144_vertices[187] = {
#include "stg09/terrain/draw_144_vertices.inc"
};
static NJS_CNK_MODEL draw_144_model = {
#include "stg09/terrain/draw_144_model.inc"
};
static NJS_CNK_OBJECT draw_144 = {
#include "stg09/terrain/draw_144.inc"
};
static Sint16 draw_143_polygons[95] = {
#include "stg09/terrain/draw_143_polygons.inc"
};
static Sint32 draw_143_vertices[59] = {
#include "stg09/terrain/draw_143_vertices.inc"
};
static NJS_CNK_MODEL draw_143_model = {
#include "stg09/terrain/draw_143_model.inc"
};
static NJS_CNK_OBJECT draw_143 = {
#include "stg09/terrain/draw_143.inc"
};
static Sint16 draw_142_polygons[423] = {
#include "stg09/terrain/draw_142_polygons.inc"
};
static Sint32 draw_142_vertices[463] = {
#include "stg09/terrain/draw_142_vertices.inc"
};
static NJS_CNK_MODEL draw_142_model = {
#include "stg09/terrain/draw_142_model.inc"
};
static NJS_CNK_OBJECT draw_142 = {
#include "stg09/terrain/draw_142.inc"
};
static Sint16 draw_141_polygons[277] = {
#include "stg09/terrain/draw_141_polygons.inc"
};
static Sint32 draw_141_vertices[179] = {
#include "stg09/terrain/draw_141_vertices.inc"
};
static NJS_CNK_MODEL draw_141_model = {
#include "stg09/terrain/draw_141_model.inc"
};
static NJS_CNK_OBJECT draw_141 = {
#include "stg09/terrain/draw_141.inc"
};
static Sint16 draw_139_polygons[215] = {
#include "stg09/terrain/draw_139_polygons.inc"
};
static Sint32 draw_139_vertices[243] = {
#include "stg09/terrain/draw_139_vertices.inc"
};
static NJS_CNK_MODEL draw_139_model = {
#include "stg09/terrain/draw_139_model.inc"
};
static NJS_CNK_OBJECT draw_140 = {
#include "stg09/terrain/draw_140.inc"
};
static Sint16 draw_138_polygons[121] = {
#include "stg09/terrain/draw_138_polygons.inc"
};
static Sint32 draw_138_vertices[115] = {
#include "stg09/terrain/draw_138_vertices.inc"
};
static NJS_CNK_MODEL draw_138_model = {
#include "stg09/terrain/draw_138_model.inc"
};
static NJS_CNK_OBJECT draw_138 = {
#include "stg09/terrain/draw_138.inc"
};
static Sint16 draw_137_polygons[387] = {
#include "stg09/terrain/draw_137_polygons.inc"
};
static Sint32 draw_137_vertices[391] = {
#include "stg09/terrain/draw_137_vertices.inc"
};
static NJS_CNK_MODEL draw_137_model = {
#include "stg09/terrain/draw_137_model.inc"
};
static NJS_CNK_OBJECT draw_137 = {
#include "stg09/terrain/draw_137.inc"
};
static Sint16 draw_136_polygons[431] = {
#include "stg09/terrain/draw_136_polygons.inc"
};
static Sint32 draw_136_vertices[327] = {
#include "stg09/terrain/draw_136_vertices.inc"
};
static NJS_CNK_MODEL draw_136_model = {
#include "stg09/terrain/draw_136_model.inc"
};
static NJS_CNK_OBJECT draw_136 = {
#include "stg09/terrain/draw_136.inc"
};
static Sint16 draw_132_polygons[215] = {
#include "stg09/terrain/draw_132_polygons.inc"
};
static Sint32 draw_132_vertices[243] = {
#include "stg09/terrain/draw_132_vertices.inc"
};
static NJS_CNK_MODEL draw_132_model = {
#include "stg09/terrain/draw_132_model.inc"
};
static NJS_CNK_OBJECT draw_135 = {
#include "stg09/terrain/draw_135.inc"
};
static Sint16 draw_130_polygons[121] = {
#include "stg09/terrain/draw_130_polygons.inc"
};
static Sint32 draw_130_vertices[115] = {
#include "stg09/terrain/draw_130_vertices.inc"
};
static NJS_CNK_MODEL draw_130_model = {
#include "stg09/terrain/draw_130_model.inc"
};
static NJS_CNK_OBJECT draw_131 = {
#include "stg09/terrain/draw_131.inc"
};
static Sint16 draw_127_polygons[389] = {
#include "stg09/terrain/draw_127_polygons.inc"
};
static Sint32 draw_127_vertices[415] = {
#include "stg09/terrain/draw_127_vertices.inc"
};
static NJS_CNK_MODEL draw_127_model = {
#include "stg09/terrain/draw_127_model.inc"
};
static NJS_CNK_OBJECT draw_129 = {
#include "stg09/terrain/draw_129.inc"
};
static Sint16 draw_126_polygons[121] = {
#include "stg09/terrain/draw_126_polygons.inc"
};
static Sint32 draw_126_vertices[115] = {
#include "stg09/terrain/draw_126_vertices.inc"
};
static NJS_CNK_MODEL draw_126_model = {
#include "stg09/terrain/draw_126_model.inc"
};
static NJS_CNK_OBJECT draw_126 = {
#include "stg09/terrain/draw_126.inc"
};
static Sint16 draw_125_polygons[385] = {
#include "stg09/terrain/draw_125_polygons.inc"
};
static Sint32 draw_125_vertices[263] = {
#include "stg09/terrain/draw_125_vertices.inc"
};
static NJS_CNK_MODEL draw_125_model = {
#include "stg09/terrain/draw_125_model.inc"
};
static NJS_CNK_OBJECT draw_125 = {
#include "stg09/terrain/draw_125.inc"
};
static Sint16 draw_124_polygons[167] = {
#include "stg09/terrain/draw_124_polygons.inc"
};
static Sint32 draw_124_vertices[107] = {
#include "stg09/terrain/draw_124_vertices.inc"
};
static NJS_CNK_MODEL draw_124_model = {
#include "stg09/terrain/draw_124_model.inc"
};
static NJS_CNK_OBJECT draw_124 = {
#include "stg09/terrain/draw_124.inc"
};
static Sint16 draw_122_polygons[431] = {
#include "stg09/terrain/draw_122_polygons.inc"
};
static Sint32 draw_122_vertices[323] = {
#include "stg09/terrain/draw_122_vertices.inc"
};
static NJS_CNK_MODEL draw_122_model = {
#include "stg09/terrain/draw_122_model.inc"
};
static NJS_CNK_OBJECT draw_123 = {
#include "stg09/terrain/draw_123.inc"
};
static Sint16 draw_121_polygons[175] = {
#include "stg09/terrain/draw_121_polygons.inc"
};
static Sint32 draw_121_vertices[107] = {
#include "stg09/terrain/draw_121_vertices.inc"
};
static NJS_CNK_MODEL draw_121_model = {
#include "stg09/terrain/draw_121_model.inc"
};
static NJS_CNK_OBJECT draw_121 = {
#include "stg09/terrain/draw_121.inc"
};
static Sint16 draw_120_polygons[23] = {
#include "stg09/terrain/draw_120_polygons.inc"
};
static Sint32 draw_120_vertices[19] = {
#include "stg09/terrain/draw_120_vertices.inc"
};
static NJS_CNK_MODEL draw_120_model = {
#include "stg09/terrain/draw_120_model.inc"
};
static NJS_CNK_OBJECT draw_120 = {
#include "stg09/terrain/draw_120.inc"
};
static Sint16 draw_1_polygons[571] = {
#include "stg09/terrain/draw_1_polygons.inc"
};
static Sint32 draw_1_vertices[495] = {
#include "stg09/terrain/draw_1_vertices.inc"
};
static NJS_CNK_MODEL draw_1_model = {
#include "stg09/terrain/draw_1_model.inc"
};
static NJS_CNK_OBJECT draw_119 = {
#include "stg09/terrain/draw_119.inc"
};
static Sint16 draw_0_polygons[641] = {
#include "stg09/terrain/draw_0_polygons.inc"
};
static Sint32 draw_0_vertices[595] = {
#include "stg09/terrain/draw_0_vertices.inc"
};
static NJS_CNK_MODEL draw_0_model = {
#include "stg09/terrain/draw_0_model.inc"
};
static NJS_CNK_OBJECT draw_117 = {
#include "stg09/terrain/draw_117.inc"
};
static Sint16 draw_18_polygons[329] = {
#include "stg09/terrain/draw_18_polygons.inc"
};
static Sint32 draw_18_vertices[303] = {
#include "stg09/terrain/draw_18_vertices.inc"
};
static NJS_CNK_MODEL draw_18_model = {
#include "stg09/terrain/draw_18_model.inc"
};
static NJS_CNK_OBJECT draw_115 = {
#include "stg09/terrain/draw_115.inc"
};
static Sint16 draw_114_polygons[359] = {
#include "stg09/terrain/draw_114_polygons.inc"
};
static Sint32 draw_114_vertices[359] = {
#include "stg09/terrain/draw_114_vertices.inc"
};
static NJS_CNK_MODEL draw_114_model = {
#include "stg09/terrain/draw_114_model.inc"
};
static NJS_CNK_OBJECT draw_114 = {
#include "stg09/terrain/draw_114.inc"
};
static Sint16 draw_4_polygons[85] = {
#include "stg09/terrain/draw_4_polygons.inc"
};
static Sint32 draw_4_vertices[59] = {
#include "stg09/terrain/draw_4_vertices.inc"
};
static NJS_CNK_MODEL draw_4_model = {
#include "stg09/terrain/draw_4_model.inc"
};
static NJS_CNK_OBJECT draw_113 = {
#include "stg09/terrain/draw_113.inc"
};
static Sint16 draw_16_polygons[375] = {
#include "stg09/terrain/draw_16_polygons.inc"
};
static Sint32 draw_16_vertices[395] = {
#include "stg09/terrain/draw_16_vertices.inc"
};
static NJS_CNK_MODEL draw_16_model = {
#include "stg09/terrain/draw_16_model.inc"
};
static NJS_CNK_OBJECT draw_112 = {
#include "stg09/terrain/draw_112.inc"
};
static Sint16 draw_15_polygons[281] = {
#include "stg09/terrain/draw_15_polygons.inc"
};
static Sint32 draw_15_vertices[187] = {
#include "stg09/terrain/draw_15_vertices.inc"
};
static NJS_CNK_MODEL draw_15_model = {
#include "stg09/terrain/draw_15_model.inc"
};
static NJS_CNK_OBJECT draw_111 = {
#include "stg09/terrain/draw_111.inc"
};
static Sint16 draw_14_polygons[419] = {
#include "stg09/terrain/draw_14_polygons.inc"
};
static Sint32 draw_14_vertices[491] = {
#include "stg09/terrain/draw_14_vertices.inc"
};
static NJS_CNK_MODEL draw_14_model = {
#include "stg09/terrain/draw_14_model.inc"
};
static NJS_CNK_OBJECT draw_110 = {
#include "stg09/terrain/draw_110.inc"
};
static Sint16 draw_13_polygons[147] = {
#include "stg09/terrain/draw_13_polygons.inc"
};
static Sint32 draw_13_vertices[83] = {
#include "stg09/terrain/draw_13_vertices.inc"
};
static NJS_CNK_MODEL draw_13_model = {
#include "stg09/terrain/draw_13_model.inc"
};
static NJS_CNK_OBJECT draw_109 = {
#include "stg09/terrain/draw_109.inc"
};
static Sint16 draw_12_polygons[153] = {
#include "stg09/terrain/draw_12_polygons.inc"
};
static Sint32 draw_12_vertices[131] = {
#include "stg09/terrain/draw_12_vertices.inc"
};
static NJS_CNK_MODEL draw_12_model = {
#include "stg09/terrain/draw_12_model.inc"
};
static NJS_CNK_OBJECT draw_108 = {
#include "stg09/terrain/draw_108.inc"
};
static Sint16 draw_99_polygons[255] = {
#include "stg09/terrain/draw_99_polygons.inc"
};
static Sint32 draw_99_vertices[147] = {
#include "stg09/terrain/draw_99_vertices.inc"
};
static NJS_CNK_MODEL draw_99_model = {
#include "stg09/terrain/draw_99_model.inc"
};
static NJS_CNK_OBJECT draw_99 = {
#include "stg09/terrain/draw_99.inc"
};
static Sint16 draw_98_polygons[701] = {
#include "stg09/terrain/draw_98_polygons.inc"
};
static Sint32 draw_98_vertices[523] = {
#include "stg09/terrain/draw_98_vertices.inc"
};
static NJS_CNK_MODEL draw_98_model = {
#include "stg09/terrain/draw_98_model.inc"
};
static NJS_CNK_OBJECT draw_98 = {
#include "stg09/terrain/draw_98.inc"
};
static Sint16 draw_82_polygons[1197] = {
#include "stg09/terrain/draw_82_polygons.inc"
};
static Sint32 draw_82_vertices[775] = {
#include "stg09/terrain/draw_82_vertices.inc"
};
static NJS_CNK_MODEL draw_82_model = {
#include "stg09/terrain/draw_82_model.inc"
};
static NJS_CNK_OBJECT draw_84 = {
#include "stg09/terrain/draw_84.inc"
};
static Sint16 draw_66_polygons[497] = {
#include "stg09/terrain/draw_66_polygons.inc"
};
static Sint32 draw_66_vertices[315] = {
#include "stg09/terrain/draw_66_vertices.inc"
};
static NJS_CNK_MODEL draw_66_model = {
#include "stg09/terrain/draw_66_model.inc"
};
static NJS_CNK_OBJECT draw_68 = {
#include "stg09/terrain/draw_68.inc"
};
static Sint16 draw_54_polygons[301] = {
#include "stg09/terrain/draw_54_polygons.inc"
};
static Sint32 draw_54_vertices[199] = {
#include "stg09/terrain/draw_54_vertices.inc"
};
static NJS_CNK_MODEL draw_54_model = {
#include "stg09/terrain/draw_54_model.inc"
};
static NJS_CNK_OBJECT draw_54 = {
#include "stg09/terrain/draw_54.inc"
};
static Sint16 draw_45_polygons[507] = {
#include "stg09/terrain/draw_45_polygons.inc"
};
static Sint32 draw_45_vertices[259] = {
#include "stg09/terrain/draw_45_vertices.inc"
};
static NJS_CNK_MODEL draw_45_model = {
#include "stg09/terrain/draw_45_model.inc"
};
static NJS_CNK_OBJECT draw_45 = {
#include "stg09/terrain/draw_45.inc"
};
static Sint16 draw_43_polygons[275] = {
#include "stg09/terrain/draw_43_polygons.inc"
};
static Sint32 draw_43_vertices[183] = {
#include "stg09/terrain/draw_43_vertices.inc"
};
static NJS_CNK_MODEL draw_43_model = {
#include "stg09/terrain/draw_43_model.inc"
};
static NJS_CNK_OBJECT draw_43 = {
#include "stg09/terrain/draw_43.inc"
};
static Sint16 draw_39_polygons[319] = {
#include "stg09/terrain/draw_39_polygons.inc"
};
static Sint32 draw_39_vertices[251] = {
#include "stg09/terrain/draw_39_vertices.inc"
};
static NJS_CNK_MODEL draw_39_model = {
#include "stg09/terrain/draw_39_model.inc"
};
static NJS_CNK_OBJECT draw_39 = {
#include "stg09/terrain/draw_39.inc"
};
static Sint16 draw_33_polygons[403] = {
#include "stg09/terrain/draw_33_polygons.inc"
};
static Sint32 draw_33_vertices[447] = {
#include "stg09/terrain/draw_33_vertices.inc"
};
static NJS_CNK_MODEL draw_33_model = {
#include "stg09/terrain/draw_33_model.inc"
};
static NJS_CNK_OBJECT draw_33 = {
#include "stg09/terrain/draw_33.inc"
};
static Sint16 draw_31_polygons[189] = {
#include "stg09/terrain/draw_31_polygons.inc"
};
static Sint32 draw_31_vertices[91] = {
#include "stg09/terrain/draw_31_vertices.inc"
};
static NJS_CNK_MODEL draw_31_model = {
#include "stg09/terrain/draw_31_model.inc"
};
static NJS_CNK_OBJECT draw_32 = {
#include "stg09/terrain/draw_32.inc"
};
static Sint16 draw_28_polygons[407] = {
#include "stg09/terrain/draw_28_polygons.inc"
};
static Sint32 draw_28_vertices[227] = {
#include "stg09/terrain/draw_28_vertices.inc"
};
static NJS_CNK_MODEL draw_28_model = {
#include "stg09/terrain/draw_28_model.inc"
};
static NJS_CNK_OBJECT draw_28 = {
#include "stg09/terrain/draw_28.inc"
};
static Sint16 draw_27_polygons[173] = {
#include "stg09/terrain/draw_27_polygons.inc"
};
static Sint32 draw_27_vertices[71] = {
#include "stg09/terrain/draw_27_vertices.inc"
};
static NJS_CNK_MODEL draw_27_model = {
#include "stg09/terrain/draw_27_model.inc"
};
static NJS_CNK_OBJECT draw_27 = {
#include "stg09/terrain/draw_27.inc"
};
static Sint16 draw_3_polygons[219] = {
#include "stg09/terrain/draw_3_polygons.inc"
};
static Sint32 draw_3_vertices[123] = {
#include "stg09/terrain/draw_3_vertices.inc"
};
static NJS_CNK_MODEL draw_3_model = {
#include "stg09/terrain/draw_3_model.inc"
};
static NJS_CNK_OBJECT draw_3 = {
#include "stg09/terrain/draw_3.inc"
};
static Sint16 draw_2_polygons[403] = {
#include "stg09/terrain/draw_2_polygons.inc"
};
static Sint32 draw_2_vertices[155] = {
#include "stg09/terrain/draw_2_vertices.inc"
};
static NJS_CNK_MODEL draw_2_model = {
#include "stg09/terrain/draw_2_model.inc"
};
static NJS_CNK_OBJECT draw_2 = {
#include "stg09/terrain/draw_2.inc"
};
static NJS_MATERIAL collision_0_materials[1] = {
#include "stg09/terrain/collision_0_materials.inc"
};
static Sint16 collision_0_faces[138] = {
#include "stg09/terrain/collision_0_faces.inc"
};
static NJS_MESHSET collision_0_meshsets[1] = {
#include "stg09/terrain/collision_0_meshsets.inc"
};
static NJS_POINT3 collision_0_points[64] = {
#include "stg09/terrain/collision_0_points.inc"
};
static NJS_MODEL collision_0_model = {
#include "stg09/terrain/collision_0_model.inc"
};
static NJS_OBJECT collision_0 = {
#include "stg09/terrain/collision_0.inc"
};
static NJS_MATERIAL collision_1_materials[1] = {
#include "stg09/terrain/collision_1_materials.inc"
};
static Sint16 collision_1_faces[129] = {
#include "stg09/terrain/collision_1_faces.inc"
};
static NJS_MESHSET collision_1_meshsets[1] = {
#include "stg09/terrain/collision_1_meshsets.inc"
};
static NJS_POINT3 collision_1_points[66] = {
#include "stg09/terrain/collision_1_points.inc"
};
static NJS_MODEL collision_1_model = {
#include "stg09/terrain/collision_1_model.inc"
};
static NJS_OBJECT collision_1 = {
#include "stg09/terrain/collision_1.inc"
};
static NJS_MATERIAL collision_2_materials[1] = {
#include "stg09/terrain/collision_2_materials.inc"
};
static Sint16 collision_2_faces[375] = {
#include "stg09/terrain/collision_2_faces.inc"
};
static NJS_MESHSET collision_2_meshsets[1] = {
#include "stg09/terrain/collision_2_meshsets.inc"
};
static NJS_POINT3 collision_2_points[166] = {
#include "stg09/terrain/collision_2_points.inc"
};
static NJS_MODEL collision_2_model = {
#include "stg09/terrain/collision_2_model.inc"
};
static NJS_OBJECT collision_2 = {
#include "stg09/terrain/collision_2.inc"
};
static NJS_MATERIAL collision_3_materials[1] = {
#include "stg09/terrain/collision_3_materials.inc"
};
static Sint16 collision_3_faces[375] = {
#include "stg09/terrain/collision_3_faces.inc"
};
static NJS_MESHSET collision_3_meshsets[1] = {
#include "stg09/terrain/collision_3_meshsets.inc"
};
static NJS_POINT3 collision_3_points[166] = {
#include "stg09/terrain/collision_3_points.inc"
};
static NJS_MODEL collision_3_model = {
#include "stg09/terrain/collision_3_model.inc"
};
static NJS_OBJECT collision_3 = {
#include "stg09/terrain/collision_3.inc"
};
static NJS_MATERIAL collision_4_materials[1] = {
#include "stg09/terrain/collision_4_materials.inc"
};
static Sint16 collision_4_faces[7] = {
#include "stg09/terrain/collision_4_faces.inc"
};
static NJS_MESHSET collision_4_meshsets[1] = {
#include "stg09/terrain/collision_4_meshsets.inc"
};
static NJS_POINT3 collision_4_points[6] = {
#include "stg09/terrain/collision_4_points.inc"
};
static NJS_MODEL collision_4_model = {
#include "stg09/terrain/collision_4_model.inc"
};
static NJS_OBJECT collision_4 = {
#include "stg09/terrain/collision_4.inc"
};
static NJS_MATERIAL collision_5_materials[1] = {
#include "stg09/terrain/collision_5_materials.inc"
};
static Sint16 collision_5_faces[5] = {
#include "stg09/terrain/collision_5_faces.inc"
};
static NJS_MESHSET collision_5_meshsets[1] = {
#include "stg09/terrain/collision_5_meshsets.inc"
};
static NJS_POINT3 collision_5_points[4] = {
#include "stg09/terrain/collision_5_points.inc"
};
static NJS_MODEL collision_5_model = {
#include "stg09/terrain/collision_5_model.inc"
};
static NJS_OBJECT collision_5 = {
#include "stg09/terrain/collision_5.inc"
};
static NJS_MATERIAL collision_6_materials[1] = {
#include "stg09/terrain/collision_6_materials.inc"
};
static Sint16 collision_6_faces[12] = {
#include "stg09/terrain/collision_6_faces.inc"
};
static NJS_MESHSET collision_6_meshsets[1] = {
#include "stg09/terrain/collision_6_meshsets.inc"
};
static NJS_POINT3 collision_6_points[10] = {
#include "stg09/terrain/collision_6_points.inc"
};
static NJS_MODEL collision_6_model = {
#include "stg09/terrain/collision_6_model.inc"
};
static NJS_OBJECT collision_6 = {
#include "stg09/terrain/collision_6.inc"
};
static NJS_MATERIAL collision_7_materials[1] = {
#include "stg09/terrain/collision_7_materials.inc"
};
static Sint16 collision_7_faces[7] = {
#include "stg09/terrain/collision_7_faces.inc"
};
static NJS_MESHSET collision_7_meshsets[1] = {
#include "stg09/terrain/collision_7_meshsets.inc"
};
static NJS_POINT3 collision_7_points[6] = {
#include "stg09/terrain/collision_7_points.inc"
};
static NJS_MODEL collision_7_model = {
#include "stg09/terrain/collision_7_model.inc"
};
static NJS_OBJECT collision_7 = {
#include "stg09/terrain/collision_7.inc"
};
static NJS_MATERIAL collision_8_materials[1] = {
#include "stg09/terrain/collision_8_materials.inc"
};
static Sint16 collision_8_faces[7] = {
#include "stg09/terrain/collision_8_faces.inc"
};
static NJS_MESHSET collision_8_meshsets[1] = {
#include "stg09/terrain/collision_8_meshsets.inc"
};
static NJS_POINT3 collision_8_points[6] = {
#include "stg09/terrain/collision_8_points.inc"
};
static NJS_MODEL collision_8_model = {
#include "stg09/terrain/collision_8_model.inc"
};
static NJS_OBJECT collision_8 = {
#include "stg09/terrain/collision_8.inc"
};
static NJS_MATERIAL collision_9_materials[1] = {
#include "stg09/terrain/collision_9_materials.inc"
};
static Sint16 collision_9_faces[12] = {
#include "stg09/terrain/collision_9_faces.inc"
};
static NJS_MESHSET collision_9_meshsets[1] = {
#include "stg09/terrain/collision_9_meshsets.inc"
};
static NJS_POINT3 collision_9_points[10] = {
#include "stg09/terrain/collision_9_points.inc"
};
static NJS_MODEL collision_9_model = {
#include "stg09/terrain/collision_9_model.inc"
};
static NJS_OBJECT collision_9 = {
#include "stg09/terrain/collision_9.inc"
};
static NJS_MATERIAL collision_10_materials[1] = {
#include "stg09/terrain/collision_10_materials.inc"
};
static Sint16 collision_10_faces[10] = {
#include "stg09/terrain/collision_10_faces.inc"
};
static NJS_MESHSET collision_10_meshsets[1] = {
#include "stg09/terrain/collision_10_meshsets.inc"
};
static NJS_POINT3 collision_10_points[8] = {
#include "stg09/terrain/collision_10_points.inc"
};
static NJS_MODEL collision_10_model = {
#include "stg09/terrain/collision_10_model.inc"
};
static NJS_OBJECT collision_10 = {
#include "stg09/terrain/collision_10.inc"
};
static NJS_MATERIAL collision_11_materials[1] = {
#include "stg09/terrain/collision_11_materials.inc"
};
static Sint16 collision_11_faces[9] = {
#include "stg09/terrain/collision_11_faces.inc"
};
static NJS_MESHSET collision_11_meshsets[1] = {
#include "stg09/terrain/collision_11_meshsets.inc"
};
static NJS_POINT3 collision_11_points[8] = {
#include "stg09/terrain/collision_11_points.inc"
};
static NJS_MODEL collision_11_model = {
#include "stg09/terrain/collision_11_model.inc"
};
static NJS_OBJECT collision_11 = {
#include "stg09/terrain/collision_11.inc"
};
static NJS_MATERIAL collision_12_materials[1] = {
#include "stg09/terrain/collision_12_materials.inc"
};
static Sint16 collision_12_faces[10] = {
#include "stg09/terrain/collision_12_faces.inc"
};
static NJS_MESHSET collision_12_meshsets[1] = {
#include "stg09/terrain/collision_12_meshsets.inc"
};
static NJS_POINT3 collision_12_points[8] = {
#include "stg09/terrain/collision_12_points.inc"
};
static NJS_MODEL collision_12_model = {
#include "stg09/terrain/collision_12_model.inc"
};
static NJS_OBJECT collision_12 = {
#include "stg09/terrain/collision_12.inc"
};
static NJS_MATERIAL collision_13_materials[1] = {
#include "stg09/terrain/collision_13_materials.inc"
};
static Sint16 collision_13_faces[7] = {
#include "stg09/terrain/collision_13_faces.inc"
};
static NJS_MESHSET collision_13_meshsets[1] = {
#include "stg09/terrain/collision_13_meshsets.inc"
};
static NJS_POINT3 collision_13_points[6] = {
#include "stg09/terrain/collision_13_points.inc"
};
static NJS_MODEL collision_13_model = {
#include "stg09/terrain/collision_13_model.inc"
};
static NJS_OBJECT collision_13 = {
#include "stg09/terrain/collision_13.inc"
};
static NJS_MATERIAL collision_14_materials[1] = {
#include "stg09/terrain/collision_14_materials.inc"
};
static Sint16 collision_14_faces[5] = {
#include "stg09/terrain/collision_14_faces.inc"
};
static NJS_MESHSET collision_14_meshsets[1] = {
#include "stg09/terrain/collision_14_meshsets.inc"
};
static NJS_POINT3 collision_14_points[4] = {
#include "stg09/terrain/collision_14_points.inc"
};
static NJS_MODEL collision_14_model = {
#include "stg09/terrain/collision_14_model.inc"
};
static NJS_OBJECT collision_14 = {
#include "stg09/terrain/collision_14.inc"
};
static NJS_MATERIAL collision_16_materials[1] = {
#include "stg09/terrain/collision_16_materials.inc"
};
static Sint16 collision_16_faces[7] = {
#include "stg09/terrain/collision_16_faces.inc"
};
static NJS_MESHSET collision_16_meshsets[1] = {
#include "stg09/terrain/collision_16_meshsets.inc"
};
static NJS_POINT3 collision_16_points[6] = {
#include "stg09/terrain/collision_16_points.inc"
};
static NJS_MODEL collision_16_model = {
#include "stg09/terrain/collision_16_model.inc"
};
static NJS_OBJECT collision_16 = {
#include "stg09/terrain/collision_16.inc"
};
static NJS_MATERIAL collision_17_materials[1] = {
#include "stg09/terrain/collision_17_materials.inc"
};
static Sint16 collision_17_faces[7] = {
#include "stg09/terrain/collision_17_faces.inc"
};
static NJS_MESHSET collision_17_meshsets[1] = {
#include "stg09/terrain/collision_17_meshsets.inc"
};
static NJS_POINT3 collision_17_points[6] = {
#include "stg09/terrain/collision_17_points.inc"
};
static NJS_MODEL collision_17_model = {
#include "stg09/terrain/collision_17_model.inc"
};
static NJS_OBJECT collision_17 = {
#include "stg09/terrain/collision_17.inc"
};
static NJS_MATERIAL collision_19_materials[1] = {
#include "stg09/terrain/collision_19_materials.inc"
};
static Sint16 collision_19_faces[9] = {
#include "stg09/terrain/collision_19_faces.inc"
};
static NJS_MESHSET collision_19_meshsets[1] = {
#include "stg09/terrain/collision_19_meshsets.inc"
};
static NJS_POINT3 collision_19_points[8] = {
#include "stg09/terrain/collision_19_points.inc"
};
static NJS_MODEL collision_19_model = {
#include "stg09/terrain/collision_19_model.inc"
};
static NJS_OBJECT collision_19 = {
#include "stg09/terrain/collision_19.inc"
};
static NJS_MATERIAL collision_21_materials[1] = {
#include "stg09/terrain/collision_21_materials.inc"
};
static Sint16 collision_21_faces[43] = {
#include "stg09/terrain/collision_21_faces.inc"
};
static NJS_MESHSET collision_21_meshsets[1] = {
#include "stg09/terrain/collision_21_meshsets.inc"
};
static NJS_POINT3 collision_21_points[24] = {
#include "stg09/terrain/collision_21_points.inc"
};
static NJS_MODEL collision_21_model = {
#include "stg09/terrain/collision_21_model.inc"
};
static NJS_OBJECT collision_21 = {
#include "stg09/terrain/collision_21.inc"
};
static NJS_MATERIAL collision_22_materials[1] = {
#include "stg09/terrain/collision_22_materials.inc"
};
static Sint16 collision_22_faces[28] = {
#include "stg09/terrain/collision_22_faces.inc"
};
static NJS_MESHSET collision_22_meshsets[1] = {
#include "stg09/terrain/collision_22_meshsets.inc"
};
static NJS_POINT3 collision_22_points[18] = {
#include "stg09/terrain/collision_22_points.inc"
};
static NJS_MODEL collision_22_model = {
#include "stg09/terrain/collision_22_model.inc"
};
static NJS_OBJECT collision_22 = {
#include "stg09/terrain/collision_22.inc"
};
static NJS_MATERIAL collision_23_materials[1] = {
#include "stg09/terrain/collision_23_materials.inc"
};
static Sint16 collision_23_faces[28] = {
#include "stg09/terrain/collision_23_faces.inc"
};
static NJS_MESHSET collision_23_meshsets[1] = {
#include "stg09/terrain/collision_23_meshsets.inc"
};
static NJS_POINT3 collision_23_points[18] = {
#include "stg09/terrain/collision_23_points.inc"
};
static NJS_MODEL collision_23_model = {
#include "stg09/terrain/collision_23_model.inc"
};
static NJS_OBJECT collision_23 = {
#include "stg09/terrain/collision_23.inc"
};
static NJS_MATERIAL collision_24_materials[1] = {
#include "stg09/terrain/collision_24_materials.inc"
};
static Sint16 collision_24_faces[37] = {
#include "stg09/terrain/collision_24_faces.inc"
};
static NJS_MESHSET collision_24_meshsets[1] = {
#include "stg09/terrain/collision_24_meshsets.inc"
};
static NJS_POINT3 collision_24_points[21] = {
#include "stg09/terrain/collision_24_points.inc"
};
static NJS_MODEL collision_24_model = {
#include "stg09/terrain/collision_24_model.inc"
};
static NJS_OBJECT collision_24 = {
#include "stg09/terrain/collision_24.inc"
};
static NJS_MATERIAL collision_25_materials[1] = {
#include "stg09/terrain/collision_25_materials.inc"
};
static Sint16 collision_25_faces[37] = {
#include "stg09/terrain/collision_25_faces.inc"
};
static NJS_MESHSET collision_25_meshsets[1] = {
#include "stg09/terrain/collision_25_meshsets.inc"
};
static NJS_POINT3 collision_25_points[21] = {
#include "stg09/terrain/collision_25_points.inc"
};
static NJS_MODEL collision_25_model = {
#include "stg09/terrain/collision_25_model.inc"
};
static NJS_OBJECT collision_25 = {
#include "stg09/terrain/collision_25.inc"
};
static NJS_MATERIAL collision_26_materials[1] = {
#include "stg09/terrain/collision_26_materials.inc"
};
static Sint16 collision_26_faces[28] = {
#include "stg09/terrain/collision_26_faces.inc"
};
static NJS_MESHSET collision_26_meshsets[1] = {
#include "stg09/terrain/collision_26_meshsets.inc"
};
static NJS_POINT3 collision_26_points[18] = {
#include "stg09/terrain/collision_26_points.inc"
};
static NJS_MODEL collision_26_model = {
#include "stg09/terrain/collision_26_model.inc"
};
static NJS_OBJECT collision_26 = {
#include "stg09/terrain/collision_26.inc"
};
static NJS_MATERIAL collision_27_materials[1] = {
#include "stg09/terrain/collision_27_materials.inc"
};
static Sint16 collision_27_faces[11] = {
#include "stg09/terrain/collision_27_faces.inc"
};
static NJS_MESHSET collision_27_meshsets[1] = {
#include "stg09/terrain/collision_27_meshsets.inc"
};
static NJS_POINT3 collision_27_points[8] = {
#include "stg09/terrain/collision_27_points.inc"
};
static NJS_MODEL collision_27_model = {
#include "stg09/terrain/collision_27_model.inc"
};
static NJS_OBJECT collision_27 = {
#include "stg09/terrain/collision_27.inc"
};
static NJS_MATERIAL collision_28_materials[1] = {
#include "stg09/terrain/collision_28_materials.inc"
};
static Sint16 collision_28_faces[16] = {
#include "stg09/terrain/collision_28_faces.inc"
};
static NJS_MESHSET collision_28_meshsets[1] = {
#include "stg09/terrain/collision_28_meshsets.inc"
};
static NJS_POINT3 collision_28_points[8] = {
#include "stg09/terrain/collision_28_points.inc"
};
static NJS_MODEL collision_28_model = {
#include "stg09/terrain/collision_28_model.inc"
};
static NJS_OBJECT collision_28 = {
#include "stg09/terrain/collision_28.inc"
};
static NJS_MATERIAL collision_29_materials[1] = {
#include "stg09/terrain/collision_29_materials.inc"
};
static Sint16 collision_29_faces[21] = {
#include "stg09/terrain/collision_29_faces.inc"
};
static NJS_MESHSET collision_29_meshsets[1] = {
#include "stg09/terrain/collision_29_meshsets.inc"
};
static NJS_POINT3 collision_29_points[8] = {
#include "stg09/terrain/collision_29_points.inc"
};
static NJS_MODEL collision_29_model = {
#include "stg09/terrain/collision_29_model.inc"
};
static NJS_OBJECT collision_29 = {
#include "stg09/terrain/collision_29.inc"
};
static NJS_MATERIAL collision_30_materials[1] = {
#include "stg09/terrain/collision_30_materials.inc"
};
static Sint16 collision_30_faces[11] = {
#include "stg09/terrain/collision_30_faces.inc"
};
static NJS_MESHSET collision_30_meshsets[1] = {
#include "stg09/terrain/collision_30_meshsets.inc"
};
static NJS_POINT3 collision_30_points[8] = {
#include "stg09/terrain/collision_30_points.inc"
};
static NJS_MODEL collision_30_model = {
#include "stg09/terrain/collision_30_model.inc"
};
static NJS_OBJECT collision_30 = {
#include "stg09/terrain/collision_30.inc"
};
static NJS_MATERIAL collision_33_materials[1] = {
#include "stg09/terrain/collision_33_materials.inc"
};
static Sint16 collision_33_faces[27] = {
#include "stg09/terrain/collision_33_faces.inc"
};
static NJS_MESHSET collision_33_meshsets[1] = {
#include "stg09/terrain/collision_33_meshsets.inc"
};
static NJS_POINT3 collision_33_points[16] = {
#include "stg09/terrain/collision_33_points.inc"
};
static NJS_MODEL collision_33_model = {
#include "stg09/terrain/collision_33_model.inc"
};
static NJS_OBJECT collision_33 = {
#include "stg09/terrain/collision_33.inc"
};
static NJS_MATERIAL collision_34_materials[1] = {
#include "stg09/terrain/collision_34_materials.inc"
};
static Sint16 collision_34_faces[97] = {
#include "stg09/terrain/collision_34_faces.inc"
};
static NJS_MESHSET collision_34_meshsets[1] = {
#include "stg09/terrain/collision_34_meshsets.inc"
};
static NJS_POINT3 collision_34_points[36] = {
#include "stg09/terrain/collision_34_points.inc"
};
static NJS_MODEL collision_34_model = {
#include "stg09/terrain/collision_34_model.inc"
};
static NJS_OBJECT collision_34 = {
#include "stg09/terrain/collision_34.inc"
};
static NJS_MATERIAL collision_37_materials[1] = {
#include "stg09/terrain/collision_37_materials.inc"
};
static Sint16 collision_37_faces[33] = {
#include "stg09/terrain/collision_37_faces.inc"
};
static NJS_MESHSET collision_37_meshsets[1] = {
#include "stg09/terrain/collision_37_meshsets.inc"
};
static NJS_POINT3 collision_37_points[17] = {
#include "stg09/terrain/collision_37_points.inc"
};
static NJS_MODEL collision_37_model = {
#include "stg09/terrain/collision_37_model.inc"
};
static NJS_OBJECT collision_37 = {
#include "stg09/terrain/collision_37.inc"
};
static NJS_MATERIAL collision_38_materials[1] = {
#include "stg09/terrain/collision_38_materials.inc"
};
static Sint16 collision_38_faces[46] = {
#include "stg09/terrain/collision_38_faces.inc"
};
static NJS_MESHSET collision_38_meshsets[1] = {
#include "stg09/terrain/collision_38_meshsets.inc"
};
static NJS_POINT3 collision_38_points[22] = {
#include "stg09/terrain/collision_38_points.inc"
};
static NJS_MODEL collision_38_model = {
#include "stg09/terrain/collision_38_model.inc"
};
static NJS_OBJECT collision_38 = {
#include "stg09/terrain/collision_38.inc"
};
static NJS_MATERIAL collision_39_materials[1] = {
#include "stg09/terrain/collision_39_materials.inc"
};
static Sint16 collision_39_faces[16] = {
#include "stg09/terrain/collision_39_faces.inc"
};
static NJS_MESHSET collision_39_meshsets[1] = {
#include "stg09/terrain/collision_39_meshsets.inc"
};
static NJS_POINT3 collision_39_points[8] = {
#include "stg09/terrain/collision_39_points.inc"
};
static NJS_MODEL collision_39_model = {
#include "stg09/terrain/collision_39_model.inc"
};
static NJS_OBJECT collision_39 = {
#include "stg09/terrain/collision_39.inc"
};
static NJS_MATERIAL collision_40_materials[1] = {
#include "stg09/terrain/collision_40_materials.inc"
};
static Sint16 collision_40_faces[21] = {
#include "stg09/terrain/collision_40_faces.inc"
};
static NJS_MESHSET collision_40_meshsets[1] = {
#include "stg09/terrain/collision_40_meshsets.inc"
};
static NJS_POINT3 collision_40_points[8] = {
#include "stg09/terrain/collision_40_points.inc"
};
static NJS_MODEL collision_40_model = {
#include "stg09/terrain/collision_40_model.inc"
};
static NJS_OBJECT collision_40 = {
#include "stg09/terrain/collision_40.inc"
};
static NJS_MATERIAL collision_41_materials[1] = {
#include "stg09/terrain/collision_41_materials.inc"
};
static Sint16 collision_41_faces[19] = {
#include "stg09/terrain/collision_41_faces.inc"
};
static NJS_MESHSET collision_41_meshsets[1] = {
#include "stg09/terrain/collision_41_meshsets.inc"
};
static NJS_POINT3 collision_41_points[8] = {
#include "stg09/terrain/collision_41_points.inc"
};
static NJS_MODEL collision_41_model = {
#include "stg09/terrain/collision_41_model.inc"
};
static NJS_OBJECT collision_41 = {
#include "stg09/terrain/collision_41.inc"
};
static NJS_MATERIAL collision_42_materials[1] = {
#include "stg09/terrain/collision_42_materials.inc"
};
static Sint16 collision_42_faces[21] = {
#include "stg09/terrain/collision_42_faces.inc"
};
static NJS_MESHSET collision_42_meshsets[1] = {
#include "stg09/terrain/collision_42_meshsets.inc"
};
static NJS_POINT3 collision_42_points[12] = {
#include "stg09/terrain/collision_42_points.inc"
};
static NJS_MODEL collision_42_model = {
#include "stg09/terrain/collision_42_model.inc"
};
static NJS_OBJECT collision_42 = {
#include "stg09/terrain/collision_42.inc"
};
static NJS_MATERIAL collision_46_materials[1] = {
#include "stg09/terrain/collision_46_materials.inc"
};
static Sint16 collision_46_faces[104] = {
#include "stg09/terrain/collision_46_faces.inc"
};
static NJS_MESHSET collision_46_meshsets[1] = {
#include "stg09/terrain/collision_46_meshsets.inc"
};
static NJS_POINT3 collision_46_points[39] = {
#include "stg09/terrain/collision_46_points.inc"
};
static NJS_MODEL collision_46_model = {
#include "stg09/terrain/collision_46_model.inc"
};
static NJS_OBJECT collision_46 = {
#include "stg09/terrain/collision_46.inc"
};
static NJS_MATERIAL collision_47_materials[1] = {
#include "stg09/terrain/collision_47_materials.inc"
};
static Sint16 collision_47_faces[17] = {
#include "stg09/terrain/collision_47_faces.inc"
};
static NJS_MESHSET collision_47_meshsets[1] = {
#include "stg09/terrain/collision_47_meshsets.inc"
};
static NJS_POINT3 collision_47_points[8] = {
#include "stg09/terrain/collision_47_points.inc"
};
static NJS_MODEL collision_47_model = {
#include "stg09/terrain/collision_47_model.inc"
};
static NJS_OBJECT collision_47 = {
#include "stg09/terrain/collision_47.inc"
};
static NJS_MATERIAL collision_48_materials[1] = {
#include "stg09/terrain/collision_48_materials.inc"
};
static Sint16 collision_48_faces[16] = {
#include "stg09/terrain/collision_48_faces.inc"
};
static NJS_MESHSET collision_48_meshsets[1] = {
#include "stg09/terrain/collision_48_meshsets.inc"
};
static NJS_POINT3 collision_48_points[8] = {
#include "stg09/terrain/collision_48_points.inc"
};
static NJS_MODEL collision_48_model = {
#include "stg09/terrain/collision_48_model.inc"
};
static NJS_OBJECT collision_48 = {
#include "stg09/terrain/collision_48.inc"
};
static NJS_MATERIAL collision_52_materials[1] = {
#include "stg09/terrain/collision_52_materials.inc"
};
static Sint16 collision_52_faces[19] = {
#include "stg09/terrain/collision_52_faces.inc"
};
static NJS_MESHSET collision_52_meshsets[1] = {
#include "stg09/terrain/collision_52_meshsets.inc"
};
static NJS_POINT3 collision_52_points[8] = {
#include "stg09/terrain/collision_52_points.inc"
};
static NJS_MODEL collision_52_model = {
#include "stg09/terrain/collision_52_model.inc"
};
static NJS_OBJECT collision_52 = {
#include "stg09/terrain/collision_52.inc"
};
static NJS_MATERIAL collision_53_materials[1] = {
#include "stg09/terrain/collision_53_materials.inc"
};
static Sint16 collision_53_faces[33] = {
#include "stg09/terrain/collision_53_faces.inc"
};
static NJS_MESHSET collision_53_meshsets[1] = {
#include "stg09/terrain/collision_53_meshsets.inc"
};
static NJS_POINT3 collision_53_points[12] = {
#include "stg09/terrain/collision_53_points.inc"
};
static NJS_MODEL collision_53_model = {
#include "stg09/terrain/collision_53_model.inc"
};
static NJS_OBJECT collision_53 = {
#include "stg09/terrain/collision_53.inc"
};
static NJS_MATERIAL collision_54_materials[1] = {
#include "stg09/terrain/collision_54_materials.inc"
};
static Sint16 collision_54_faces[19] = {
#include "stg09/terrain/collision_54_faces.inc"
};
static NJS_MESHSET collision_54_meshsets[1] = {
#include "stg09/terrain/collision_54_meshsets.inc"
};
static NJS_POINT3 collision_54_points[8] = {
#include "stg09/terrain/collision_54_points.inc"
};
static NJS_MODEL collision_54_model = {
#include "stg09/terrain/collision_54_model.inc"
};
static NJS_OBJECT collision_54 = {
#include "stg09/terrain/collision_54.inc"
};
static NJS_MATERIAL collision_55_materials[1] = {
#include "stg09/terrain/collision_55_materials.inc"
};
static Sint16 collision_55_faces[19] = {
#include "stg09/terrain/collision_55_faces.inc"
};
static NJS_MESHSET collision_55_meshsets[1] = {
#include "stg09/terrain/collision_55_meshsets.inc"
};
static NJS_POINT3 collision_55_points[8] = {
#include "stg09/terrain/collision_55_points.inc"
};
static NJS_MODEL collision_55_model = {
#include "stg09/terrain/collision_55_model.inc"
};
static NJS_OBJECT collision_55 = {
#include "stg09/terrain/collision_55.inc"
};
static NJS_MATERIAL collision_56_materials[1] = {
#include "stg09/terrain/collision_56_materials.inc"
};
static Sint16 collision_56_faces[11] = {
#include "stg09/terrain/collision_56_faces.inc"
};
static NJS_MESHSET collision_56_meshsets[1] = {
#include "stg09/terrain/collision_56_meshsets.inc"
};
static NJS_POINT3 collision_56_points[8] = {
#include "stg09/terrain/collision_56_points.inc"
};
static NJS_MODEL collision_56_model = {
#include "stg09/terrain/collision_56_model.inc"
};
static NJS_OBJECT collision_56 = {
#include "stg09/terrain/collision_56.inc"
};
static NJS_MATERIAL collision_58_materials[1] = {
#include "stg09/terrain/collision_58_materials.inc"
};
static Sint16 collision_58_faces[14] = {
#include "stg09/terrain/collision_58_faces.inc"
};
static NJS_MESHSET collision_58_meshsets[1] = {
#include "stg09/terrain/collision_58_meshsets.inc"
};
static NJS_POINT3 collision_58_points[8] = {
#include "stg09/terrain/collision_58_points.inc"
};
static NJS_MODEL collision_58_model = {
#include "stg09/terrain/collision_58_model.inc"
};
static NJS_OBJECT collision_58 = {
#include "stg09/terrain/collision_58.inc"
};
static NJS_MATERIAL collision_59_materials[1] = {
#include "stg09/terrain/collision_59_materials.inc"
};
static Sint16 collision_59_faces[46] = {
#include "stg09/terrain/collision_59_faces.inc"
};
static NJS_MESHSET collision_59_meshsets[1] = {
#include "stg09/terrain/collision_59_meshsets.inc"
};
static NJS_POINT3 collision_59_points[21] = {
#include "stg09/terrain/collision_59_points.inc"
};
static NJS_MODEL collision_59_model = {
#include "stg09/terrain/collision_59_model.inc"
};
static NJS_OBJECT collision_59 = {
#include "stg09/terrain/collision_59.inc"
};
static NJS_MATERIAL collision_64_materials[1] = {
#include "stg09/terrain/collision_64_materials.inc"
};
static Sint16 collision_64_faces[11] = {
#include "stg09/terrain/collision_64_faces.inc"
};
static NJS_MESHSET collision_64_meshsets[1] = {
#include "stg09/terrain/collision_64_meshsets.inc"
};
static NJS_POINT3 collision_64_points[8] = {
#include "stg09/terrain/collision_64_points.inc"
};
static NJS_MODEL collision_64_model = {
#include "stg09/terrain/collision_64_model.inc"
};
static NJS_OBJECT collision_64 = {
#include "stg09/terrain/collision_64.inc"
};
static NJS_MATERIAL collision_65_materials[1] = {
#include "stg09/terrain/collision_65_materials.inc"
};
static Sint16 collision_65_faces[19] = {
#include "stg09/terrain/collision_65_faces.inc"
};
static NJS_MESHSET collision_65_meshsets[1] = {
#include "stg09/terrain/collision_65_meshsets.inc"
};
static NJS_POINT3 collision_65_points[8] = {
#include "stg09/terrain/collision_65_points.inc"
};
static NJS_MODEL collision_65_model = {
#include "stg09/terrain/collision_65_model.inc"
};
static NJS_OBJECT collision_65 = {
#include "stg09/terrain/collision_65.inc"
};
static NJS_MATERIAL collision_66_materials[1] = {
#include "stg09/terrain/collision_66_materials.inc"
};
static Sint16 collision_66_faces[11] = {
#include "stg09/terrain/collision_66_faces.inc"
};
static NJS_MESHSET collision_66_meshsets[1] = {
#include "stg09/terrain/collision_66_meshsets.inc"
};
static NJS_POINT3 collision_66_points[8] = {
#include "stg09/terrain/collision_66_points.inc"
};
static NJS_MODEL collision_66_model = {
#include "stg09/terrain/collision_66_model.inc"
};
static NJS_OBJECT collision_66 = {
#include "stg09/terrain/collision_66.inc"
};
static NJS_MATERIAL collision_67_materials[1] = {
#include "stg09/terrain/collision_67_materials.inc"
};
static Sint16 collision_67_faces[61] = {
#include "stg09/terrain/collision_67_faces.inc"
};
static NJS_MESHSET collision_67_meshsets[1] = {
#include "stg09/terrain/collision_67_meshsets.inc"
};
static NJS_POINT3 collision_67_points[27] = {
#include "stg09/terrain/collision_67_points.inc"
};
static NJS_MODEL collision_67_model = {
#include "stg09/terrain/collision_67_model.inc"
};
static NJS_OBJECT collision_67 = {
#include "stg09/terrain/collision_67.inc"
};
static NJS_MATERIAL collision_68_materials[1] = {
#include "stg09/terrain/collision_68_materials.inc"
};
static Sint16 collision_68_faces[27] = {
#include "stg09/terrain/collision_68_faces.inc"
};
static NJS_MESHSET collision_68_meshsets[1] = {
#include "stg09/terrain/collision_68_meshsets.inc"
};
static NJS_POINT3 collision_68_points[16] = {
#include "stg09/terrain/collision_68_points.inc"
};
static NJS_MODEL collision_68_model = {
#include "stg09/terrain/collision_68_model.inc"
};
static NJS_OBJECT collision_68 = {
#include "stg09/terrain/collision_68.inc"
};
static NJS_MATERIAL collision_69_materials[1] = {
#include "stg09/terrain/collision_69_materials.inc"
};
static Sint16 collision_69_faces[27] = {
#include "stg09/terrain/collision_69_faces.inc"
};
static NJS_MESHSET collision_69_meshsets[1] = {
#include "stg09/terrain/collision_69_meshsets.inc"
};
static NJS_POINT3 collision_69_points[14] = {
#include "stg09/terrain/collision_69_points.inc"
};
static NJS_MODEL collision_69_model = {
#include "stg09/terrain/collision_69_model.inc"
};
static NJS_OBJECT collision_69 = {
#include "stg09/terrain/collision_69.inc"
};
static NJS_MATERIAL collision_70_materials[1] = {
#include "stg09/terrain/collision_70_materials.inc"
};
static Sint16 collision_70_faces[27] = {
#include "stg09/terrain/collision_70_faces.inc"
};
static NJS_MESHSET collision_70_meshsets[1] = {
#include "stg09/terrain/collision_70_meshsets.inc"
};
static NJS_POINT3 collision_70_points[14] = {
#include "stg09/terrain/collision_70_points.inc"
};
static NJS_MODEL collision_70_model = {
#include "stg09/terrain/collision_70_model.inc"
};
static NJS_OBJECT collision_70 = {
#include "stg09/terrain/collision_70.inc"
};
static NJS_MATERIAL collision_71_materials[1] = {
#include "stg09/terrain/collision_71_materials.inc"
};
static Sint16 collision_71_faces[28] = {
#include "stg09/terrain/collision_71_faces.inc"
};
static NJS_MESHSET collision_71_meshsets[1] = {
#include "stg09/terrain/collision_71_meshsets.inc"
};
static NJS_POINT3 collision_71_points[17] = {
#include "stg09/terrain/collision_71_points.inc"
};
static NJS_MODEL collision_71_model = {
#include "stg09/terrain/collision_71_model.inc"
};
static NJS_OBJECT collision_71 = {
#include "stg09/terrain/collision_71.inc"
};
static NJS_MATERIAL collision_72_materials[1] = {
#include "stg09/terrain/collision_72_materials.inc"
};
static Sint16 collision_72_faces[23] = {
#include "stg09/terrain/collision_72_faces.inc"
};
static NJS_MESHSET collision_72_meshsets[1] = {
#include "stg09/terrain/collision_72_meshsets.inc"
};
static NJS_POINT3 collision_72_points[16] = {
#include "stg09/terrain/collision_72_points.inc"
};
static NJS_MODEL collision_72_model = {
#include "stg09/terrain/collision_72_model.inc"
};
static NJS_OBJECT collision_72 = {
#include "stg09/terrain/collision_72.inc"
};
static NJS_MATERIAL collision_73_materials[1] = {
#include "stg09/terrain/collision_73_materials.inc"
};
static Sint16 collision_73_faces[21] = {
#include "stg09/terrain/collision_73_faces.inc"
};
static NJS_MESHSET collision_73_meshsets[1] = {
#include "stg09/terrain/collision_73_meshsets.inc"
};
static NJS_POINT3 collision_73_points[8] = {
#include "stg09/terrain/collision_73_points.inc"
};
static NJS_MODEL collision_73_model = {
#include "stg09/terrain/collision_73_model.inc"
};
static NJS_OBJECT collision_73 = {
#include "stg09/terrain/collision_73.inc"
};
static NJS_MATERIAL collision_74_materials[1] = {
#include "stg09/terrain/collision_74_materials.inc"
};
static Sint16 collision_74_faces[33] = {
#include "stg09/terrain/collision_74_faces.inc"
};
static NJS_MESHSET collision_74_meshsets[1] = {
#include "stg09/terrain/collision_74_meshsets.inc"
};
static NJS_POINT3 collision_74_points[12] = {
#include "stg09/terrain/collision_74_points.inc"
};
static NJS_MODEL collision_74_model = {
#include "stg09/terrain/collision_74_model.inc"
};
static NJS_OBJECT collision_74 = {
#include "stg09/terrain/collision_74.inc"
};
static NJS_MATERIAL collision_75_materials[1] = {
#include "stg09/terrain/collision_75_materials.inc"
};
static Sint16 collision_75_faces[25] = {
#include "stg09/terrain/collision_75_faces.inc"
};
static NJS_MESHSET collision_75_meshsets[1] = {
#include "stg09/terrain/collision_75_meshsets.inc"
};
static NJS_POINT3 collision_75_points[15] = {
#include "stg09/terrain/collision_75_points.inc"
};
static NJS_MODEL collision_75_model = {
#include "stg09/terrain/collision_75_model.inc"
};
static NJS_OBJECT collision_75 = {
#include "stg09/terrain/collision_75.inc"
};
static NJS_MATERIAL collision_76_materials[1] = {
#include "stg09/terrain/collision_76_materials.inc"
};
static Sint16 collision_76_faces[18] = {
#include "stg09/terrain/collision_76_faces.inc"
};
static NJS_MESHSET collision_76_meshsets[1] = {
#include "stg09/terrain/collision_76_meshsets.inc"
};
static NJS_POINT3 collision_76_points[16] = {
#include "stg09/terrain/collision_76_points.inc"
};
static NJS_MODEL collision_76_model = {
#include "stg09/terrain/collision_76_model.inc"
};
static NJS_OBJECT collision_76 = {
#include "stg09/terrain/collision_76.inc"
};
static NJS_MATERIAL collision_77_materials[1] = {
#include "stg09/terrain/collision_77_materials.inc"
};
static Sint16 collision_77_faces[23] = {
#include "stg09/terrain/collision_77_faces.inc"
};
static NJS_MESHSET collision_77_meshsets[1] = {
#include "stg09/terrain/collision_77_meshsets.inc"
};
static NJS_POINT3 collision_77_points[12] = {
#include "stg09/terrain/collision_77_points.inc"
};
static NJS_MODEL collision_77_model = {
#include "stg09/terrain/collision_77_model.inc"
};
static NJS_OBJECT collision_77 = {
#include "stg09/terrain/collision_77.inc"
};
static NJS_MATERIAL collision_78_materials[1] = {
#include "stg09/terrain/collision_78_materials.inc"
};
static Sint16 collision_78_faces[33] = {
#include "stg09/terrain/collision_78_faces.inc"
};
static NJS_MESHSET collision_78_meshsets[1] = {
#include "stg09/terrain/collision_78_meshsets.inc"
};
static NJS_POINT3 collision_78_points[16] = {
#include "stg09/terrain/collision_78_points.inc"
};
static NJS_MODEL collision_78_model = {
#include "stg09/terrain/collision_78_model.inc"
};
static NJS_OBJECT collision_78 = {
#include "stg09/terrain/collision_78.inc"
};
static NJS_MATERIAL collision_79_materials[1] = {
#include "stg09/terrain/collision_79_materials.inc"
};
static Sint16 collision_79_faces[14] = {
#include "stg09/terrain/collision_79_faces.inc"
};
static NJS_MESHSET collision_79_meshsets[1] = {
#include "stg09/terrain/collision_79_meshsets.inc"
};
static NJS_POINT3 collision_79_points[8] = {
#include "stg09/terrain/collision_79_points.inc"
};
static NJS_MODEL collision_79_model = {
#include "stg09/terrain/collision_79_model.inc"
};
static NJS_OBJECT collision_79 = {
#include "stg09/terrain/collision_79.inc"
};
static NJS_MATERIAL collision_80_materials[1] = {
#include "stg09/terrain/collision_80_materials.inc"
};
static Sint16 collision_80_faces[20] = {
#include "stg09/terrain/collision_80_faces.inc"
};
static NJS_MESHSET collision_80_meshsets[1] = {
#include "stg09/terrain/collision_80_meshsets.inc"
};
static NJS_POINT3 collision_80_points[16] = {
#include "stg09/terrain/collision_80_points.inc"
};
static NJS_MODEL collision_80_model = {
#include "stg09/terrain/collision_80_model.inc"
};
static NJS_OBJECT collision_80 = {
#include "stg09/terrain/collision_80.inc"
};
static NJS_MATERIAL collision_81_materials[1] = {
#include "stg09/terrain/collision_81_materials.inc"
};
static Sint16 collision_81_faces[36] = {
#include "stg09/terrain/collision_81_faces.inc"
};
static NJS_MESHSET collision_81_meshsets[1] = {
#include "stg09/terrain/collision_81_meshsets.inc"
};
static NJS_POINT3 collision_81_points[16] = {
#include "stg09/terrain/collision_81_points.inc"
};
static NJS_MODEL collision_81_model = {
#include "stg09/terrain/collision_81_model.inc"
};
static NJS_OBJECT collision_81 = {
#include "stg09/terrain/collision_81.inc"
};
static NJS_MATERIAL collision_82_materials[1] = {
#include "stg09/terrain/collision_82_materials.inc"
};
static Sint16 collision_82_faces[11] = {
#include "stg09/terrain/collision_82_faces.inc"
};
static NJS_MESHSET collision_82_meshsets[1] = {
#include "stg09/terrain/collision_82_meshsets.inc"
};
static NJS_POINT3 collision_82_points[8] = {
#include "stg09/terrain/collision_82_points.inc"
};
static NJS_MODEL collision_82_model = {
#include "stg09/terrain/collision_82_model.inc"
};
static NJS_OBJECT collision_82 = {
#include "stg09/terrain/collision_82.inc"
};
static NJS_MATERIAL collision_83_materials[1] = {
#include "stg09/terrain/collision_83_materials.inc"
};
static Sint16 collision_83_faces[33] = {
#include "stg09/terrain/collision_83_faces.inc"
};
static NJS_MESHSET collision_83_meshsets[1] = {
#include "stg09/terrain/collision_83_meshsets.inc"
};
static NJS_POINT3 collision_83_points[16] = {
#include "stg09/terrain/collision_83_points.inc"
};
static NJS_MODEL collision_83_model = {
#include "stg09/terrain/collision_83_model.inc"
};
static NJS_OBJECT collision_83 = {
#include "stg09/terrain/collision_83.inc"
};
static NJS_MATERIAL collision_84_materials[1] = {
#include "stg09/terrain/collision_84_materials.inc"
};
static Sint16 collision_84_faces[21] = {
#include "stg09/terrain/collision_84_faces.inc"
};
static NJS_MESHSET collision_84_meshsets[1] = {
#include "stg09/terrain/collision_84_meshsets.inc"
};
static NJS_POINT3 collision_84_points[8] = {
#include "stg09/terrain/collision_84_points.inc"
};
static NJS_MODEL collision_84_model = {
#include "stg09/terrain/collision_84_model.inc"
};
static NJS_OBJECT collision_84 = {
#include "stg09/terrain/collision_84.inc"
};
static NJS_MATERIAL collision_85_materials[1] = {
#include "stg09/terrain/collision_85_materials.inc"
};
static Sint16 collision_85_faces[21] = {
#include "stg09/terrain/collision_85_faces.inc"
};
static NJS_MESHSET collision_85_meshsets[1] = {
#include "stg09/terrain/collision_85_meshsets.inc"
};
static NJS_POINT3 collision_85_points[8] = {
#include "stg09/terrain/collision_85_points.inc"
};
static NJS_MODEL collision_85_model = {
#include "stg09/terrain/collision_85_model.inc"
};
static NJS_OBJECT collision_85 = {
#include "stg09/terrain/collision_85.inc"
};
static NJS_MATERIAL collision_86_materials[1] = {
#include "stg09/terrain/collision_86_materials.inc"
};
static Sint16 collision_86_faces[29] = {
#include "stg09/terrain/collision_86_faces.inc"
};
static NJS_MESHSET collision_86_meshsets[1] = {
#include "stg09/terrain/collision_86_meshsets.inc"
};
static NJS_POINT3 collision_86_points[22] = {
#include "stg09/terrain/collision_86_points.inc"
};
static NJS_MODEL collision_86_model = {
#include "stg09/terrain/collision_86_model.inc"
};
static NJS_OBJECT collision_86 = {
#include "stg09/terrain/collision_86.inc"
};
static NJS_MATERIAL collision_88_materials[1] = {
#include "stg09/terrain/collision_88_materials.inc"
};
static Sint16 collision_88_faces[36] = {
#include "stg09/terrain/collision_88_faces.inc"
};
static NJS_MESHSET collision_88_meshsets[1] = {
#include "stg09/terrain/collision_88_meshsets.inc"
};
static NJS_POINT3 collision_88_points[16] = {
#include "stg09/terrain/collision_88_points.inc"
};
static NJS_MODEL collision_88_model = {
#include "stg09/terrain/collision_88_model.inc"
};
static NJS_OBJECT collision_88 = {
#include "stg09/terrain/collision_88.inc"
};
static NJS_MATERIAL collision_89_materials[1] = {
#include "stg09/terrain/collision_89_materials.inc"
};
static Sint16 collision_89_faces[11] = {
#include "stg09/terrain/collision_89_faces.inc"
};
static NJS_MESHSET collision_89_meshsets[1] = {
#include "stg09/terrain/collision_89_meshsets.inc"
};
static NJS_POINT3 collision_89_points[8] = {
#include "stg09/terrain/collision_89_points.inc"
};
static NJS_MODEL collision_89_model = {
#include "stg09/terrain/collision_89_model.inc"
};
static NJS_OBJECT collision_89 = {
#include "stg09/terrain/collision_89.inc"
};
static NJS_MATERIAL collision_90_materials[1] = {
#include "stg09/terrain/collision_90_materials.inc"
};
static Sint16 collision_90_faces[39] = {
#include "stg09/terrain/collision_90_faces.inc"
};
static NJS_MESHSET collision_90_meshsets[1] = {
#include "stg09/terrain/collision_90_meshsets.inc"
};
static NJS_POINT3 collision_90_points[18] = {
#include "stg09/terrain/collision_90_points.inc"
};
static NJS_MODEL collision_90_model = {
#include "stg09/terrain/collision_90_model.inc"
};
static NJS_OBJECT collision_90 = {
#include "stg09/terrain/collision_90.inc"
};
static NJS_MATERIAL collision_91_materials[1] = {
#include "stg09/terrain/collision_91_materials.inc"
};
static Sint16 collision_91_faces[19] = {
#include "stg09/terrain/collision_91_faces.inc"
};
static NJS_MESHSET collision_91_meshsets[1] = {
#include "stg09/terrain/collision_91_meshsets.inc"
};
static NJS_POINT3 collision_91_points[8] = {
#include "stg09/terrain/collision_91_points.inc"
};
static NJS_MODEL collision_91_model = {
#include "stg09/terrain/collision_91_model.inc"
};
static NJS_OBJECT collision_91 = {
#include "stg09/terrain/collision_91.inc"
};
static NJS_MATERIAL collision_92_materials[1] = {
#include "stg09/terrain/collision_92_materials.inc"
};
static Sint16 collision_92_faces[14] = {
#include "stg09/terrain/collision_92_faces.inc"
};
static NJS_MESHSET collision_92_meshsets[1] = {
#include "stg09/terrain/collision_92_meshsets.inc"
};
static NJS_POINT3 collision_92_points[8] = {
#include "stg09/terrain/collision_92_points.inc"
};
static NJS_MODEL collision_92_model = {
#include "stg09/terrain/collision_92_model.inc"
};
static NJS_OBJECT collision_92 = {
#include "stg09/terrain/collision_92.inc"
};
static NJS_MATERIAL collision_93_materials[1] = {
#include "stg09/terrain/collision_93_materials.inc"
};
static Sint16 collision_93_faces[27] = {
#include "stg09/terrain/collision_93_faces.inc"
};
static NJS_MESHSET collision_93_meshsets[1] = {
#include "stg09/terrain/collision_93_meshsets.inc"
};
static NJS_POINT3 collision_93_points[16] = {
#include "stg09/terrain/collision_93_points.inc"
};
static NJS_MODEL collision_93_model = {
#include "stg09/terrain/collision_93_model.inc"
};
static NJS_OBJECT collision_93 = {
#include "stg09/terrain/collision_93.inc"
};
static NJS_MATERIAL collision_94_materials[1] = {
#include "stg09/terrain/collision_94_materials.inc"
};
static Sint16 collision_94_faces[27] = {
#include "stg09/terrain/collision_94_faces.inc"
};
static NJS_MESHSET collision_94_meshsets[1] = {
#include "stg09/terrain/collision_94_meshsets.inc"
};
static NJS_POINT3 collision_94_points[16] = {
#include "stg09/terrain/collision_94_points.inc"
};
static NJS_MODEL collision_94_model = {
#include "stg09/terrain/collision_94_model.inc"
};
static NJS_OBJECT collision_94 = {
#include "stg09/terrain/collision_94.inc"
};
static NJS_MATERIAL collision_95_materials[1] = {
#include "stg09/terrain/collision_95_materials.inc"
};
static Sint16 collision_95_faces[32] = {
#include "stg09/terrain/collision_95_faces.inc"
};
static NJS_MESHSET collision_95_meshsets[1] = {
#include "stg09/terrain/collision_95_meshsets.inc"
};
static NJS_POINT3 collision_95_points[16] = {
#include "stg09/terrain/collision_95_points.inc"
};
static NJS_MODEL collision_95_model = {
#include "stg09/terrain/collision_95_model.inc"
};
static NJS_OBJECT collision_95 = {
#include "stg09/terrain/collision_95.inc"
};
static NJS_MATERIAL collision_96_materials[1] = {
#include "stg09/terrain/collision_96_materials.inc"
};
static Sint16 collision_96_faces[18] = {
#include "stg09/terrain/collision_96_faces.inc"
};
static NJS_MESHSET collision_96_meshsets[1] = {
#include "stg09/terrain/collision_96_meshsets.inc"
};
static NJS_POINT3 collision_96_points[16] = {
#include "stg09/terrain/collision_96_points.inc"
};
static NJS_MODEL collision_96_model = {
#include "stg09/terrain/collision_96_model.inc"
};
static NJS_OBJECT collision_96 = {
#include "stg09/terrain/collision_96.inc"
};
static NJS_MATERIAL collision_97_materials[1] = {
#include "stg09/terrain/collision_97_materials.inc"
};
static Sint16 collision_97_faces[25] = {
#include "stg09/terrain/collision_97_faces.inc"
};
static NJS_MESHSET collision_97_meshsets[1] = {
#include "stg09/terrain/collision_97_meshsets.inc"
};
static NJS_POINT3 collision_97_points[12] = {
#include "stg09/terrain/collision_97_points.inc"
};
static NJS_MODEL collision_97_model = {
#include "stg09/terrain/collision_97_model.inc"
};
static NJS_OBJECT collision_97 = {
#include "stg09/terrain/collision_97.inc"
};
static NJS_MATERIAL collision_98_materials[1] = {
#include "stg09/terrain/collision_98_materials.inc"
};
static Sint16 collision_98_faces[33] = {
#include "stg09/terrain/collision_98_faces.inc"
};
static NJS_MESHSET collision_98_meshsets[1] = {
#include "stg09/terrain/collision_98_meshsets.inc"
};
static NJS_POINT3 collision_98_points[16] = {
#include "stg09/terrain/collision_98_points.inc"
};
static NJS_MODEL collision_98_model = {
#include "stg09/terrain/collision_98_model.inc"
};
static NJS_OBJECT collision_98 = {
#include "stg09/terrain/collision_98.inc"
};
static NJS_MATERIAL collision_99_materials[1] = {
#include "stg09/terrain/collision_99_materials.inc"
};
static Sint16 collision_99_faces[22] = {
#include "stg09/terrain/collision_99_faces.inc"
};
static NJS_MESHSET collision_99_meshsets[1] = {
#include "stg09/terrain/collision_99_meshsets.inc"
};
static NJS_POINT3 collision_99_points[14] = {
#include "stg09/terrain/collision_99_points.inc"
};
static NJS_MODEL collision_99_model = {
#include "stg09/terrain/collision_99_model.inc"
};
static NJS_OBJECT collision_99 = {
#include "stg09/terrain/collision_99.inc"
};
static NJS_MATERIAL collision_100_materials[1] = {
#include "stg09/terrain/collision_100_materials.inc"
};
static Sint16 collision_100_faces[22] = {
#include "stg09/terrain/collision_100_faces.inc"
};
static NJS_MESHSET collision_100_meshsets[1] = {
#include "stg09/terrain/collision_100_meshsets.inc"
};
static NJS_POINT3 collision_100_points[14] = {
#include "stg09/terrain/collision_100_points.inc"
};
static NJS_MODEL collision_100_model = {
#include "stg09/terrain/collision_100_model.inc"
};
static NJS_OBJECT collision_100 = {
#include "stg09/terrain/collision_100.inc"
};
static NJS_MATERIAL collision_101_materials[1] = {
#include "stg09/terrain/collision_101_materials.inc"
};
static Sint16 collision_101_faces[29] = {
#include "stg09/terrain/collision_101_faces.inc"
};
static NJS_MESHSET collision_101_meshsets[1] = {
#include "stg09/terrain/collision_101_meshsets.inc"
};
static NJS_POINT3 collision_101_points[14] = {
#include "stg09/terrain/collision_101_points.inc"
};
static NJS_MODEL collision_101_model = {
#include "stg09/terrain/collision_101_model.inc"
};
static NJS_OBJECT collision_101 = {
#include "stg09/terrain/collision_101.inc"
};
static NJS_MATERIAL collision_102_materials[1] = {
#include "stg09/terrain/collision_102_materials.inc"
};
static Sint16 collision_102_faces[28] = {
#include "stg09/terrain/collision_102_faces.inc"
};
static NJS_MESHSET collision_102_meshsets[1] = {
#include "stg09/terrain/collision_102_meshsets.inc"
};
static NJS_POINT3 collision_102_points[14] = {
#include "stg09/terrain/collision_102_points.inc"
};
static NJS_MODEL collision_102_model = {
#include "stg09/terrain/collision_102_model.inc"
};
static NJS_OBJECT collision_102 = {
#include "stg09/terrain/collision_102.inc"
};
static NJS_MATERIAL collision_103_materials[1] = {
#include "stg09/terrain/collision_103_materials.inc"
};
static Sint16 collision_103_faces[22] = {
#include "stg09/terrain/collision_103_faces.inc"
};
static NJS_MESHSET collision_103_meshsets[1] = {
#include "stg09/terrain/collision_103_meshsets.inc"
};
static NJS_POINT3 collision_103_points[14] = {
#include "stg09/terrain/collision_103_points.inc"
};
static NJS_MODEL collision_103_model = {
#include "stg09/terrain/collision_103_model.inc"
};
static NJS_OBJECT collision_103 = {
#include "stg09/terrain/collision_103.inc"
};
static NJS_MATERIAL collision_104_materials[1] = {
#include "stg09/terrain/collision_104_materials.inc"
};
static Sint16 collision_104_faces[21] = {
#include "stg09/terrain/collision_104_faces.inc"
};
static NJS_MESHSET collision_104_meshsets[1] = {
#include "stg09/terrain/collision_104_meshsets.inc"
};
static NJS_POINT3 collision_104_points[8] = {
#include "stg09/terrain/collision_104_points.inc"
};
static NJS_MODEL collision_104_model = {
#include "stg09/terrain/collision_104_model.inc"
};
static NJS_OBJECT collision_104 = {
#include "stg09/terrain/collision_104.inc"
};
static NJS_MATERIAL collision_105_materials[1] = {
#include "stg09/terrain/collision_105_materials.inc"
};
static Sint16 collision_105_faces[21] = {
#include "stg09/terrain/collision_105_faces.inc"
};
static NJS_MESHSET collision_105_meshsets[1] = {
#include "stg09/terrain/collision_105_meshsets.inc"
};
static NJS_POINT3 collision_105_points[8] = {
#include "stg09/terrain/collision_105_points.inc"
};
static NJS_MODEL collision_105_model = {
#include "stg09/terrain/collision_105_model.inc"
};
static NJS_OBJECT collision_105 = {
#include "stg09/terrain/collision_105.inc"
};
static NJS_MATERIAL collision_106_materials[1] = {
#include "stg09/terrain/collision_106_materials.inc"
};
static Sint16 collision_106_faces[36] = {
#include "stg09/terrain/collision_106_faces.inc"
};
static NJS_MESHSET collision_106_meshsets[1] = {
#include "stg09/terrain/collision_106_meshsets.inc"
};
static NJS_POINT3 collision_106_points[16] = {
#include "stg09/terrain/collision_106_points.inc"
};
static NJS_MODEL collision_106_model = {
#include "stg09/terrain/collision_106_model.inc"
};
static NJS_OBJECT collision_106 = {
#include "stg09/terrain/collision_106.inc"
};
static NJS_MATERIAL collision_108_materials[1] = {
#include "stg09/terrain/collision_108_materials.inc"
};
static Sint16 collision_108_faces[11] = {
#include "stg09/terrain/collision_108_faces.inc"
};
static NJS_MESHSET collision_108_meshsets[1] = {
#include "stg09/terrain/collision_108_meshsets.inc"
};
static NJS_POINT3 collision_108_points[8] = {
#include "stg09/terrain/collision_108_points.inc"
};
static NJS_MODEL collision_108_model = {
#include "stg09/terrain/collision_108_model.inc"
};
static NJS_OBJECT collision_108 = {
#include "stg09/terrain/collision_108.inc"
};
static NJS_MATERIAL collision_109_materials[1] = {
#include "stg09/terrain/collision_109_materials.inc"
};
static Sint16 collision_109_faces[36] = {
#include "stg09/terrain/collision_109_faces.inc"
};
static NJS_MESHSET collision_109_meshsets[1] = {
#include "stg09/terrain/collision_109_meshsets.inc"
};
static NJS_POINT3 collision_109_points[20] = {
#include "stg09/terrain/collision_109_points.inc"
};
static NJS_MODEL collision_109_model = {
#include "stg09/terrain/collision_109_model.inc"
};
static NJS_OBJECT collision_109 = {
#include "stg09/terrain/collision_109.inc"
};
static NJS_MATERIAL collision_110_materials[1] = {
#include "stg09/terrain/collision_110_materials.inc"
};
static Sint16 collision_110_faces[78] = {
#include "stg09/terrain/collision_110_faces.inc"
};
static NJS_MESHSET collision_110_meshsets[1] = {
#include "stg09/terrain/collision_110_meshsets.inc"
};
static NJS_POINT3 collision_110_points[31] = {
#include "stg09/terrain/collision_110_points.inc"
};
static NJS_MODEL collision_110_model = {
#include "stg09/terrain/collision_110_model.inc"
};
static NJS_OBJECT collision_110 = {
#include "stg09/terrain/collision_110.inc"
};
static NJS_MATERIAL collision_111_materials[1] = {
#include "stg09/terrain/collision_111_materials.inc"
};
static Sint16 collision_111_faces[104] = {
#include "stg09/terrain/collision_111_faces.inc"
};
static NJS_MESHSET collision_111_meshsets[1] = {
#include "stg09/terrain/collision_111_meshsets.inc"
};
static NJS_POINT3 collision_111_points[40] = {
#include "stg09/terrain/collision_111_points.inc"
};
static NJS_MODEL collision_111_model = {
#include "stg09/terrain/collision_111_model.inc"
};
static NJS_OBJECT collision_111 = {
#include "stg09/terrain/collision_111.inc"
};
static NJS_MATERIAL collision_112_materials[1] = {
#include "stg09/terrain/collision_112_materials.inc"
};
static Sint16 collision_112_faces[14] = {
#include "stg09/terrain/collision_112_faces.inc"
};
static NJS_MESHSET collision_112_meshsets[1] = {
#include "stg09/terrain/collision_112_meshsets.inc"
};
static NJS_POINT3 collision_112_points[8] = {
#include "stg09/terrain/collision_112_points.inc"
};
static NJS_MODEL collision_112_model = {
#include "stg09/terrain/collision_112_model.inc"
};
static NJS_OBJECT collision_112 = {
#include "stg09/terrain/collision_112.inc"
};
static NJS_MATERIAL collision_137_materials[1] = {
#include "stg09/terrain/collision_137_materials.inc"
};
static Sint16 collision_137_faces[19] = {
#include "stg09/terrain/collision_137_faces.inc"
};
static NJS_MESHSET collision_137_meshsets[1] = {
#include "stg09/terrain/collision_137_meshsets.inc"
};
static NJS_POINT3 collision_137_points[8] = {
#include "stg09/terrain/collision_137_points.inc"
};
static NJS_MODEL collision_137_model = {
#include "stg09/terrain/collision_137_model.inc"
};
static NJS_OBJECT collision_137 = {
#include "stg09/terrain/collision_137.inc"
};
static NJS_MATERIAL collision_138_materials[1] = {
#include "stg09/terrain/collision_138_materials.inc"
};
static Sint16 collision_138_faces[21] = {
#include "stg09/terrain/collision_138_faces.inc"
};
static NJS_MESHSET collision_138_meshsets[1] = {
#include "stg09/terrain/collision_138_meshsets.inc"
};
static NJS_POINT3 collision_138_points[12] = {
#include "stg09/terrain/collision_138_points.inc"
};
static NJS_MODEL collision_138_model = {
#include "stg09/terrain/collision_138_model.inc"
};
static NJS_OBJECT collision_138 = {
#include "stg09/terrain/collision_138.inc"
};
static NJS_CNK_OBJECT draw_239 = {
#include "stg09/terrain/draw_239.inc"
};
static NJS_CNK_OBJECT draw_238 = {
#include "stg09/terrain/draw_238.inc"
};
static NJS_CNK_OBJECT draw_237 = {
#include "stg09/terrain/draw_237.inc"
};
static NJS_CNK_OBJECT draw_236 = {
#include "stg09/terrain/draw_236.inc"
};
static NJS_CNK_OBJECT draw_235 = {
#include "stg09/terrain/draw_235.inc"
};
static NJS_CNK_OBJECT draw_234 = {
#include "stg09/terrain/draw_234.inc"
};
static NJS_CNK_OBJECT draw_233 = {
#include "stg09/terrain/draw_233.inc"
};
static NJS_CNK_OBJECT draw_228 = {
#include "stg09/terrain/draw_228.inc"
};
static NJS_CNK_OBJECT draw_227 = {
#include "stg09/terrain/draw_227.inc"
};
static NJS_CNK_OBJECT draw_225 = {
#include "stg09/terrain/draw_225.inc"
};
static NJS_CNK_OBJECT draw_224 = {
#include "stg09/terrain/draw_224.inc"
};
static NJS_CNK_OBJECT draw_223 = {
#include "stg09/terrain/draw_223.inc"
};
static NJS_CNK_OBJECT draw_221 = {
#include "stg09/terrain/draw_221.inc"
};
static NJS_CNK_OBJECT draw_220 = {
#include "stg09/terrain/draw_220.inc"
};
static NJS_CNK_OBJECT draw_219 = {
#include "stg09/terrain/draw_219.inc"
};
static NJS_CNK_OBJECT draw_218 = {
#include "stg09/terrain/draw_218.inc"
};
static NJS_CNK_OBJECT draw_217 = {
#include "stg09/terrain/draw_217.inc"
};
static NJS_CNK_OBJECT draw_216 = {
#include "stg09/terrain/draw_216.inc"
};
static NJS_CNK_OBJECT draw_215 = {
#include "stg09/terrain/draw_215.inc"
};
static NJS_CNK_OBJECT draw_214 = {
#include "stg09/terrain/draw_214.inc"
};
static NJS_CNK_OBJECT draw_212 = {
#include "stg09/terrain/draw_212.inc"
};
static NJS_CNK_OBJECT draw_211 = {
#include "stg09/terrain/draw_211.inc"
};
static NJS_CNK_OBJECT draw_210 = {
#include "stg09/terrain/draw_210.inc"
};
static NJS_CNK_OBJECT draw_209 = {
#include "stg09/terrain/draw_209.inc"
};
static NJS_CNK_OBJECT draw_208 = {
#include "stg09/terrain/draw_208.inc"
};
static NJS_CNK_OBJECT draw_207 = {
#include "stg09/terrain/draw_207.inc"
};
static NJS_CNK_OBJECT draw_206 = {
#include "stg09/terrain/draw_206.inc"
};
static NJS_CNK_OBJECT draw_205 = {
#include "stg09/terrain/draw_205.inc"
};
static NJS_CNK_OBJECT draw_202 = {
#include "stg09/terrain/draw_202.inc"
};
static NJS_CNK_OBJECT draw_201 = {
#include "stg09/terrain/draw_201.inc"
};
static NJS_CNK_OBJECT draw_200 = {
#include "stg09/terrain/draw_200.inc"
};
static NJS_CNK_OBJECT draw_199 = {
#include "stg09/terrain/draw_199.inc"
};
static NJS_CNK_OBJECT draw_198 = {
#include "stg09/terrain/draw_198.inc"
};
static NJS_CNK_OBJECT draw_197 = {
#include "stg09/terrain/draw_197.inc"
};
static NJS_CNK_OBJECT draw_195 = {
#include "stg09/terrain/draw_195.inc"
};
static NJS_CNK_OBJECT draw_194 = {
#include "stg09/terrain/draw_194.inc"
};
static NJS_CNK_OBJECT draw_193 = {
#include "stg09/terrain/draw_193.inc"
};
static NJS_CNK_OBJECT draw_192 = {
#include "stg09/terrain/draw_192.inc"
};
static NJS_CNK_OBJECT draw_191 = {
#include "stg09/terrain/draw_191.inc"
};
static NJS_CNK_OBJECT draw_190 = {
#include "stg09/terrain/draw_190.inc"
};
static NJS_CNK_OBJECT draw_189 = {
#include "stg09/terrain/draw_189.inc"
};
static NJS_CNK_OBJECT draw_188 = {
#include "stg09/terrain/draw_188.inc"
};
static NJS_CNK_OBJECT draw_187 = {
#include "stg09/terrain/draw_187.inc"
};
static NJS_CNK_OBJECT draw_186 = {
#include "stg09/terrain/draw_186.inc"
};
static NJS_CNK_OBJECT draw_185 = {
#include "stg09/terrain/draw_185.inc"
};
static NJS_CNK_OBJECT draw_184 = {
#include "stg09/terrain/draw_184.inc"
};
static NJS_CNK_OBJECT draw_183 = {
#include "stg09/terrain/draw_183.inc"
};
static NJS_CNK_OBJECT draw_182 = {
#include "stg09/terrain/draw_182.inc"
};
static NJS_CNK_OBJECT draw_181 = {
#include "stg09/terrain/draw_181.inc"
};
static NJS_CNK_OBJECT draw_180 = {
#include "stg09/terrain/draw_180.inc"
};
static NJS_CNK_OBJECT draw_179 = {
#include "stg09/terrain/draw_179.inc"
};
static NJS_CNK_OBJECT draw_178 = {
#include "stg09/terrain/draw_178.inc"
};
static NJS_CNK_OBJECT draw_177 = {
#include "stg09/terrain/draw_177.inc"
};
static NJS_CNK_OBJECT draw_175 = {
#include "stg09/terrain/draw_175.inc"
};
static NJS_CNK_OBJECT draw_174 = {
#include "stg09/terrain/draw_174.inc"
};
static NJS_CNK_OBJECT draw_173 = {
#include "stg09/terrain/draw_173.inc"
};
static NJS_CNK_OBJECT draw_172 = {
#include "stg09/terrain/draw_172.inc"
};
static NJS_CNK_OBJECT draw_171 = {
#include "stg09/terrain/draw_171.inc"
};
static NJS_CNK_OBJECT draw_170 = {
#include "stg09/terrain/draw_170.inc"
};
static NJS_CNK_OBJECT draw_169 = {
#include "stg09/terrain/draw_169.inc"
};
static NJS_CNK_OBJECT draw_167 = {
#include "stg09/terrain/draw_167.inc"
};
static NJS_CNK_OBJECT draw_166 = {
#include "stg09/terrain/draw_166.inc"
};
static NJS_CNK_OBJECT draw_165 = {
#include "stg09/terrain/draw_165.inc"
};
static NJS_CNK_OBJECT draw_164 = {
#include "stg09/terrain/draw_164.inc"
};
static NJS_CNK_OBJECT draw_163 = {
#include "stg09/terrain/draw_163.inc"
};
static NJS_CNK_OBJECT draw_162 = {
#include "stg09/terrain/draw_162.inc"
};
static NJS_CNK_OBJECT draw_161 = {
#include "stg09/terrain/draw_161.inc"
};
static NJS_CNK_OBJECT draw_160 = {
#include "stg09/terrain/draw_160.inc"
};
static NJS_CNK_OBJECT draw_159 = {
#include "stg09/terrain/draw_159.inc"
};
static NJS_CNK_OBJECT draw_158 = {
#include "stg09/terrain/draw_158.inc"
};
static NJS_CNK_OBJECT draw_157 = {
#include "stg09/terrain/draw_157.inc"
};
static NJS_CNK_OBJECT draw_156 = {
#include "stg09/terrain/draw_156.inc"
};
static NJS_CNK_OBJECT draw_153 = {
#include "stg09/terrain/draw_153.inc"
};
static NJS_CNK_OBJECT draw_152 = {
#include "stg09/terrain/draw_152.inc"
};
static NJS_CNK_OBJECT draw_151 = {
#include "stg09/terrain/draw_151.inc"
};
static NJS_CNK_OBJECT draw_149 = {
#include "stg09/terrain/draw_149.inc"
};
static NJS_CNK_OBJECT draw_146 = {
#include "stg09/terrain/draw_146.inc"
};
static NJS_CNK_OBJECT draw_145 = {
#include "stg09/terrain/draw_145.inc"
};
static NJS_CNK_OBJECT draw_139 = {
#include "stg09/terrain/draw_139.inc"
};
static NJS_CNK_OBJECT draw_134 = {
#include "stg09/terrain/draw_134.inc"
};
static NJS_CNK_OBJECT draw_133 = {
#include "stg09/terrain/draw_133.inc"
};
static NJS_CNK_OBJECT draw_132 = {
#include "stg09/terrain/draw_132.inc"
};
static NJS_CNK_OBJECT draw_130 = {
#include "stg09/terrain/draw_130.inc"
};
static NJS_CNK_OBJECT draw_128 = {
#include "stg09/terrain/draw_128.inc"
};
static NJS_CNK_OBJECT draw_127 = {
#include "stg09/terrain/draw_127.inc"
};
static NJS_CNK_OBJECT draw_122 = {
#include "stg09/terrain/draw_122.inc"
};
static NJS_CNK_OBJECT draw_118 = {
#include "stg09/terrain/draw_118.inc"
};
static NJS_CNK_OBJECT draw_116 = {
#include "stg09/terrain/draw_116.inc"
};
static NJS_CNK_OBJECT draw_107 = {
#include "stg09/terrain/draw_107.inc"
};
static NJS_CNK_OBJECT draw_106 = {
#include "stg09/terrain/draw_106.inc"
};
static NJS_CNK_OBJECT draw_105 = {
#include "stg09/terrain/draw_105.inc"
};
static NJS_CNK_OBJECT draw_104 = {
#include "stg09/terrain/draw_104.inc"
};
static NJS_CNK_OBJECT draw_103 = {
#include "stg09/terrain/draw_103.inc"
};
static NJS_CNK_OBJECT draw_102 = {
#include "stg09/terrain/draw_102.inc"
};
static NJS_CNK_OBJECT draw_101 = {
#include "stg09/terrain/draw_101.inc"
};
static NJS_CNK_OBJECT draw_100 = {
#include "stg09/terrain/draw_100.inc"
};
static NJS_CNK_OBJECT draw_97 = {
#include "stg09/terrain/draw_97.inc"
};
static NJS_CNK_OBJECT draw_96 = {
#include "stg09/terrain/draw_96.inc"
};
static NJS_CNK_OBJECT draw_95 = {
#include "stg09/terrain/draw_95.inc"
};
static NJS_CNK_OBJECT draw_94 = {
#include "stg09/terrain/draw_94.inc"
};
static NJS_CNK_OBJECT draw_93 = {
#include "stg09/terrain/draw_93.inc"
};
static NJS_CNK_OBJECT draw_92 = {
#include "stg09/terrain/draw_92.inc"
};
static NJS_CNK_OBJECT draw_91 = {
#include "stg09/terrain/draw_91.inc"
};
static NJS_CNK_OBJECT draw_90 = {
#include "stg09/terrain/draw_90.inc"
};
static NJS_CNK_OBJECT draw_89 = {
#include "stg09/terrain/draw_89.inc"
};
static NJS_CNK_OBJECT draw_88 = {
#include "stg09/terrain/draw_88.inc"
};
static NJS_CNK_OBJECT draw_87 = {
#include "stg09/terrain/draw_87.inc"
};
static NJS_CNK_OBJECT draw_86 = {
#include "stg09/terrain/draw_86.inc"
};
static NJS_CNK_OBJECT draw_85 = {
#include "stg09/terrain/draw_85.inc"
};
static NJS_CNK_OBJECT draw_83 = {
#include "stg09/terrain/draw_83.inc"
};
static NJS_CNK_OBJECT draw_82 = {
#include "stg09/terrain/draw_82.inc"
};
static NJS_CNK_OBJECT draw_81 = {
#include "stg09/terrain/draw_81.inc"
};
static NJS_CNK_OBJECT draw_80 = {
#include "stg09/terrain/draw_80.inc"
};
static NJS_CNK_OBJECT draw_79 = {
#include "stg09/terrain/draw_79.inc"
};
static NJS_CNK_OBJECT draw_78 = {
#include "stg09/terrain/draw_78.inc"
};
static NJS_CNK_OBJECT draw_77 = {
#include "stg09/terrain/draw_77.inc"
};
static NJS_CNK_OBJECT draw_76 = {
#include "stg09/terrain/draw_76.inc"
};
static NJS_CNK_OBJECT draw_75 = {
#include "stg09/terrain/draw_75.inc"
};
static NJS_CNK_OBJECT draw_74 = {
#include "stg09/terrain/draw_74.inc"
};
static NJS_CNK_OBJECT draw_73 = {
#include "stg09/terrain/draw_73.inc"
};
static NJS_CNK_OBJECT draw_72 = {
#include "stg09/terrain/draw_72.inc"
};
static NJS_CNK_OBJECT draw_71 = {
#include "stg09/terrain/draw_71.inc"
};
static NJS_CNK_OBJECT draw_70 = {
#include "stg09/terrain/draw_70.inc"
};
static NJS_CNK_OBJECT draw_69 = {
#include "stg09/terrain/draw_69.inc"
};
static NJS_CNK_OBJECT draw_67 = {
#include "stg09/terrain/draw_67.inc"
};
static NJS_CNK_OBJECT draw_66 = {
#include "stg09/terrain/draw_66.inc"
};
static NJS_CNK_OBJECT draw_65 = {
#include "stg09/terrain/draw_65.inc"
};
static NJS_CNK_OBJECT draw_64 = {
#include "stg09/terrain/draw_64.inc"
};
static NJS_CNK_OBJECT draw_63 = {
#include "stg09/terrain/draw_63.inc"
};
static NJS_CNK_OBJECT draw_62 = {
#include "stg09/terrain/draw_62.inc"
};
static NJS_CNK_OBJECT draw_61 = {
#include "stg09/terrain/draw_61.inc"
};
static NJS_CNK_OBJECT draw_60 = {
#include "stg09/terrain/draw_60.inc"
};
static NJS_CNK_OBJECT draw_59 = {
#include "stg09/terrain/draw_59.inc"
};
static NJS_CNK_OBJECT draw_58 = {
#include "stg09/terrain/draw_58.inc"
};
static NJS_CNK_OBJECT draw_57 = {
#include "stg09/terrain/draw_57.inc"
};
static NJS_CNK_OBJECT draw_56 = {
#include "stg09/terrain/draw_56.inc"
};
static NJS_CNK_OBJECT draw_55 = {
#include "stg09/terrain/draw_55.inc"
};
static NJS_CNK_OBJECT draw_53 = {
#include "stg09/terrain/draw_53.inc"
};
static NJS_CNK_OBJECT draw_52 = {
#include "stg09/terrain/draw_52.inc"
};
static NJS_CNK_OBJECT draw_51 = {
#include "stg09/terrain/draw_51.inc"
};
static NJS_CNK_OBJECT draw_50 = {
#include "stg09/terrain/draw_50.inc"
};
static NJS_CNK_OBJECT draw_49 = {
#include "stg09/terrain/draw_49.inc"
};
static NJS_CNK_OBJECT draw_48 = {
#include "stg09/terrain/draw_48.inc"
};
static NJS_CNK_OBJECT draw_47 = {
#include "stg09/terrain/draw_47.inc"
};
static NJS_CNK_OBJECT draw_46 = {
#include "stg09/terrain/draw_46.inc"
};
static NJS_CNK_OBJECT draw_44 = {
#include "stg09/terrain/draw_44.inc"
};
static NJS_CNK_OBJECT draw_42 = {
#include "stg09/terrain/draw_42.inc"
};
static NJS_CNK_OBJECT draw_41 = {
#include "stg09/terrain/draw_41.inc"
};
static NJS_CNK_OBJECT draw_40 = {
#include "stg09/terrain/draw_40.inc"
};
static NJS_CNK_OBJECT draw_38 = {
#include "stg09/terrain/draw_38.inc"
};
static NJS_CNK_OBJECT draw_37 = {
#include "stg09/terrain/draw_37.inc"
};
static NJS_CNK_OBJECT draw_36 = {
#include "stg09/terrain/draw_36.inc"
};
static NJS_CNK_OBJECT draw_35 = {
#include "stg09/terrain/draw_35.inc"
};
static NJS_CNK_OBJECT draw_34 = {
#include "stg09/terrain/draw_34.inc"
};
static NJS_CNK_OBJECT draw_31 = {
#include "stg09/terrain/draw_31.inc"
};
static NJS_CNK_OBJECT draw_30 = {
#include "stg09/terrain/draw_30.inc"
};
static NJS_CNK_OBJECT draw_29 = {
#include "stg09/terrain/draw_29.inc"
};
static NJS_CNK_OBJECT draw_26 = {
#include "stg09/terrain/draw_26.inc"
};
static NJS_CNK_OBJECT draw_25 = {
#include "stg09/terrain/draw_25.inc"
};
static NJS_CNK_OBJECT draw_24 = {
#include "stg09/terrain/draw_24.inc"
};
static NJS_CNK_OBJECT draw_23 = {
#include "stg09/terrain/draw_23.inc"
};
static NJS_CNK_OBJECT draw_22 = {
#include "stg09/terrain/draw_22.inc"
};
static NJS_CNK_OBJECT draw_21 = {
#include "stg09/terrain/draw_21.inc"
};
static NJS_CNK_OBJECT draw_20 = {
#include "stg09/terrain/draw_20.inc"
};
static NJS_CNK_OBJECT draw_19 = {
#include "stg09/terrain/draw_19.inc"
};
static NJS_CNK_OBJECT draw_18 = {
#include "stg09/terrain/draw_18.inc"
};
static NJS_CNK_OBJECT draw_17 = {
#include "stg09/terrain/draw_17.inc"
};
static NJS_CNK_OBJECT draw_16 = {
#include "stg09/terrain/draw_16.inc"
};
static NJS_CNK_OBJECT draw_15 = {
#include "stg09/terrain/draw_15.inc"
};
static NJS_CNK_OBJECT draw_14 = {
#include "stg09/terrain/draw_14.inc"
};
static NJS_CNK_OBJECT draw_13 = {
#include "stg09/terrain/draw_13.inc"
};
static NJS_CNK_OBJECT draw_12 = {
#include "stg09/terrain/draw_12.inc"
};
static NJS_CNK_OBJECT draw_11 = {
#include "stg09/terrain/draw_11.inc"
};
static NJS_CNK_OBJECT draw_10 = {
#include "stg09/terrain/draw_10.inc"
};
static NJS_CNK_OBJECT draw_9 = {
#include "stg09/terrain/draw_9.inc"
};
static NJS_CNK_OBJECT draw_8 = {
#include "stg09/terrain/draw_8.inc"
};
static NJS_CNK_OBJECT draw_7 = {
#include "stg09/terrain/draw_7.inc"
};
static NJS_CNK_OBJECT draw_6 = {
#include "stg09/terrain/draw_6.inc"
};
static NJS_CNK_OBJECT draw_5 = {
#include "stg09/terrain/draw_5.inc"
};
static NJS_CNK_OBJECT draw_4 = {
#include "stg09/terrain/draw_4.inc"
};
static NJS_CNK_OBJECT draw_1 = {
#include "stg09/terrain/draw_1.inc"
};
static NJS_CNK_OBJECT draw_0 = {
#include "stg09/terrain/draw_0.inc"
};
static NJS_OBJECT collision_15 = {
#include "stg09/terrain/collision_15.inc"
};
static NJS_OBJECT collision_18 = {
#include "stg09/terrain/collision_18.inc"
};
static NJS_OBJECT collision_20 = {
#include "stg09/terrain/collision_20.inc"
};
static NJS_OBJECT collision_31 = {
#include "stg09/terrain/collision_31.inc"
};
static NJS_OBJECT collision_32 = {
#include "stg09/terrain/collision_32.inc"
};
static NJS_OBJECT collision_35 = {
#include "stg09/terrain/collision_35.inc"
};
static NJS_OBJECT collision_36 = {
#include "stg09/terrain/collision_36.inc"
};
static NJS_OBJECT collision_43 = {
#include "stg09/terrain/collision_43.inc"
};
static NJS_OBJECT collision_44 = {
#include "stg09/terrain/collision_44.inc"
};
static NJS_OBJECT collision_45 = {
#include "stg09/terrain/collision_45.inc"
};
static NJS_OBJECT collision_49 = {
#include "stg09/terrain/collision_49.inc"
};
static NJS_OBJECT collision_50 = {
#include "stg09/terrain/collision_50.inc"
};
static NJS_OBJECT collision_51 = {
#include "stg09/terrain/collision_51.inc"
};
static NJS_OBJECT collision_57 = {
#include "stg09/terrain/collision_57.inc"
};
static NJS_OBJECT collision_60 = {
#include "stg09/terrain/collision_60.inc"
};
static NJS_OBJECT collision_61 = {
#include "stg09/terrain/collision_61.inc"
};
static NJS_OBJECT collision_62 = {
#include "stg09/terrain/collision_62.inc"
};
static NJS_OBJECT collision_63 = {
#include "stg09/terrain/collision_63.inc"
};
static NJS_OBJECT collision_87 = {
#include "stg09/terrain/collision_87.inc"
};
static NJS_OBJECT collision_107 = {
#include "stg09/terrain/collision_107.inc"
};
static NJS_OBJECT collision_113 = {
#include "stg09/terrain/collision_113.inc"
};
static NJS_OBJECT collision_114 = {
#include "stg09/terrain/collision_114.inc"
};
static NJS_OBJECT collision_115 = {
#include "stg09/terrain/collision_115.inc"
};
static NJS_OBJECT collision_116 = {
#include "stg09/terrain/collision_116.inc"
};
static NJS_OBJECT collision_117 = {
#include "stg09/terrain/collision_117.inc"
};
static NJS_OBJECT collision_118 = {
#include "stg09/terrain/collision_118.inc"
};
static NJS_OBJECT collision_119 = {
#include "stg09/terrain/collision_119.inc"
};
static NJS_OBJECT collision_120 = {
#include "stg09/terrain/collision_120.inc"
};
static NJS_OBJECT collision_121 = {
#include "stg09/terrain/collision_121.inc"
};
static NJS_OBJECT collision_122 = {
#include "stg09/terrain/collision_122.inc"
};
static NJS_OBJECT collision_123 = {
#include "stg09/terrain/collision_123.inc"
};
static NJS_OBJECT collision_124 = {
#include "stg09/terrain/collision_124.inc"
};
static NJS_OBJECT collision_125 = {
#include "stg09/terrain/collision_125.inc"
};
static NJS_OBJECT collision_126 = {
#include "stg09/terrain/collision_126.inc"
};
static NJS_OBJECT collision_127 = {
#include "stg09/terrain/collision_127.inc"
};
static NJS_OBJECT collision_128 = {
#include "stg09/terrain/collision_128.inc"
};
static NJS_OBJECT collision_129 = {
#include "stg09/terrain/collision_129.inc"
};
static NJS_OBJECT collision_130 = {
#include "stg09/terrain/collision_130.inc"
};
static NJS_OBJECT collision_131 = {
#include "stg09/terrain/collision_131.inc"
};
static NJS_OBJECT collision_132 = {
#include "stg09/terrain/collision_132.inc"
};
static NJS_OBJECT collision_133 = {
#include "stg09/terrain/collision_133.inc"
};
static NJS_OBJECT collision_134 = {
#include "stg09/terrain/collision_134.inc"
};
static NJS_OBJECT collision_135 = {
#include "stg09/terrain/collision_135.inc"
};
static NJS_OBJECT collision_136 = {
#include "stg09/terrain/collision_136.inc"
};
