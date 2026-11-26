#include <ninja.h>
#include <sg_xpt.h>

#include "collision/world.h"
#include "stg24/eternal_engine.h"

/* the land's file names, which its land table and texture list point at. each
 * starts on a 4-byte boundary */
const char eternal_engine_land_filenames[541] = {
#include "stg24/terrain/eternal_engine_land_filenames.inc"
};

/* STG24.PRS's land, texture lists, Basic models and chunk models, from the
 * disc. its D lies between scenery.c and poison_tex.c */

/* the data up to the last that a pointer names ahead of its definition, as
 * tentative definitions in the disc's order: GCC places each datum at its
 * first definition */
static struct land_entry entries[698];
struct land_table eternal_engine_land;
static NJS_TEXNAME textures[41];
NJS_TEXLIST eternal_engine_land_texlist;
static NJS_MATERIAL collision_0_materials[1];
static Sint16 collision_0_faces[36];
static NJS_MESHSET collision_0_meshsets[1];
static NJS_POINT3 collision_0_points[24];
static NJS_MODEL collision_0_model;
static NJS_OBJECT collision_0;
static NJS_MATERIAL collision_1_materials[1];
static Sint16 collision_1_faces[54];
static NJS_MESHSET collision_1_meshsets[1];
static NJS_POINT3 collision_1_points[32];
static NJS_MODEL collision_1_model;
static NJS_OBJECT collision_1;
static NJS_MATERIAL collision_2_materials[1];
static Sint16 collision_2_faces[36];
static NJS_MESHSET collision_2_meshsets[1];
static NJS_POINT3 collision_2_points[24];
static NJS_MODEL collision_2_model;
static NJS_OBJECT collision_2;
static NJS_MATERIAL collision_3_materials[1];
static Sint16 collision_3_faces[9];
static NJS_MESHSET collision_3_meshsets[1];
static NJS_POINT3 collision_3_points[8];
static NJS_MODEL collision_3_model;
static NJS_OBJECT collision_3;
static NJS_MATERIAL collision_5_materials[1];
static Sint16 collision_5_faces[44];
static NJS_MESHSET collision_5_meshsets[1];
static NJS_POINT3 collision_5_points[30];
static NJS_MODEL collision_5_model;
static NJS_OBJECT collision_5;
static NJS_MATERIAL collision_6_materials[1];
static Sint16 collision_6_faces[9];
static NJS_MESHSET collision_6_meshsets[1];
static NJS_POINT3 collision_6_points[8];
static NJS_MODEL collision_6_model;
static NJS_OBJECT collision_6;
static NJS_MATERIAL collision_8_materials[1];
static Sint16 collision_8_faces[66];
static NJS_MESHSET collision_8_meshsets[1];
static NJS_POINT3 collision_8_points[40];
static NJS_MODEL collision_8_model;
static NJS_OBJECT collision_8;
static NJS_MATERIAL collision_9_materials[1];
static Sint16 collision_9_faces[44];
static NJS_MESHSET collision_9_meshsets[1];
static NJS_POINT3 collision_9_points[30];
static NJS_MODEL collision_9_model;
static NJS_OBJECT collision_9;
static NJS_MATERIAL collision_10_materials[1];
static Sint16 collision_10_faces[56];
static NJS_MESHSET collision_10_meshsets[1];
static NJS_POINT3 collision_10_points[24];
static NJS_MODEL collision_10_model;
static NJS_OBJECT collision_10;
static NJS_MATERIAL collision_12_materials[1];
static Sint16 collision_12_faces[72];
static NJS_MESHSET collision_12_meshsets[1];
static NJS_POINT3 collision_12_points[42];
static NJS_MODEL collision_12_model;
static NJS_OBJECT collision_12;
static NJS_MATERIAL collision_13_materials[1];
static Sint16 collision_13_faces[108];
static NJS_MESHSET collision_13_meshsets[1];
static NJS_POINT3 collision_13_points[56];
static NJS_MODEL collision_13_model;
static NJS_OBJECT collision_13;
static NJS_MATERIAL collision_14_materials[1];
static Sint16 collision_14_faces[75];
static NJS_MESHSET collision_14_meshsets[1];
static NJS_POINT3 collision_14_points[42];
static NJS_MODEL collision_14_model;
static NJS_OBJECT collision_14;
static NJS_MATERIAL collision_15_materials[1];
static Sint16 collision_15_faces[44];
static NJS_MESHSET collision_15_meshsets[1];
static NJS_POINT3 collision_15_points[30];
static NJS_MODEL collision_15_model;
static NJS_OBJECT collision_15;
static NJS_MATERIAL collision_16_materials[1];
static Sint16 collision_16_faces[45];
static NJS_MESHSET collision_16_meshsets[1];
static NJS_POINT3 collision_16_points[28];
static NJS_MODEL collision_16_model;
static NJS_OBJECT collision_16;
static NJS_MATERIAL collision_17_materials[1];
static Sint16 collision_17_faces[66];
static NJS_MESHSET collision_17_meshsets[1];
static NJS_POINT3 collision_17_points[40];
static NJS_MODEL collision_17_model;
static NJS_OBJECT collision_17;
static NJS_MATERIAL collision_18_materials[1];
static Sint16 collision_18_faces[28];
static NJS_MESHSET collision_18_meshsets[1];
static NJS_POINT3 collision_18_points[18];
static NJS_MODEL collision_18_model;
static NJS_OBJECT collision_18;
static NJS_MATERIAL collision_22_materials[1];
static Sint16 collision_22_faces[39];
static NJS_MESHSET collision_22_meshsets[1];
static NJS_POINT3 collision_22_points[20];
static NJS_MODEL collision_22_model;
static NJS_OBJECT collision_22;
static NJS_MATERIAL collision_27_materials[1];
static Sint16 collision_27_faces[30];
static NJS_MESHSET collision_27_meshsets[1];
static NJS_POINT3 collision_27_points[21];
static NJS_MODEL collision_27_model;
static NJS_OBJECT collision_27;
static NJS_MATERIAL collision_28_materials[1];
static Sint16 collision_28_faces[45];
static NJS_MESHSET collision_28_meshsets[1];
static NJS_POINT3 collision_28_points[28];
static NJS_MODEL collision_28_model;
static NJS_OBJECT collision_28;
static NJS_MATERIAL collision_30_materials[1];
static Sint16 collision_30_faces[60];
static NJS_MESHSET collision_30_meshsets[1];
static NJS_POINT3 collision_30_points[35];
static NJS_MODEL collision_30_model;
static NJS_OBJECT collision_30;
static NJS_MATERIAL collision_31_materials[1];
static Sint16 collision_31_faces[239];
static NJS_MESHSET collision_31_meshsets[1];
static NJS_POINT3 collision_31_points[110];
static NJS_MODEL collision_31_model;
static NJS_OBJECT collision_31;
static NJS_MATERIAL collision_33_materials[1];
static Sint16 collision_33_faces[44];
static NJS_MESHSET collision_33_meshsets[1];
static NJS_POINT3 collision_33_points[30];
static NJS_MODEL collision_33_model;
static NJS_OBJECT collision_33;
static NJS_MATERIAL collision_34_materials[1];
static Sint16 collision_34_faces[28];
static NJS_MESHSET collision_34_meshsets[1];
static NJS_POINT3 collision_34_points[18];
static NJS_MODEL collision_34_model;
static NJS_OBJECT collision_34;
static NJS_MATERIAL collision_38_materials[1];
static Sint16 collision_38_faces[30];
static NJS_MESHSET collision_38_meshsets[1];
static NJS_POINT3 collision_38_points[21];
static NJS_MODEL collision_38_model;
static NJS_OBJECT collision_38;
static NJS_MATERIAL collision_39_materials[1];
static Sint16 collision_39_faces[17];
static NJS_MESHSET collision_39_meshsets[1];
static NJS_POINT3 collision_39_points[14];
static NJS_MODEL collision_39_model;
static NJS_OBJECT collision_39;
static NJS_MATERIAL collision_42_materials[1];
static Sint16 collision_42_faces[17];
static NJS_MESHSET collision_42_meshsets[1];
static NJS_POINT3 collision_42_points[14];
static NJS_MODEL collision_42_model;
static NJS_OBJECT collision_42;
static NJS_MATERIAL collision_43_materials[1];
static Sint16 collision_43_faces[17];
static NJS_MESHSET collision_43_meshsets[1];
static NJS_POINT3 collision_43_points[10];
static NJS_MODEL collision_43_model;
static NJS_OBJECT collision_43;
static NJS_MATERIAL collision_44_materials[1];
static Sint16 collision_44_faces[17];
static NJS_MESHSET collision_44_meshsets[1];
static NJS_POINT3 collision_44_points[10];
static NJS_MODEL collision_44_model;
static NJS_OBJECT collision_44;
static NJS_MATERIAL collision_45_materials[1];
static Sint16 collision_45_faces[17];
static NJS_MESHSET collision_45_meshsets[1];
static NJS_POINT3 collision_45_points[10];
static NJS_MODEL collision_45_model;
static NJS_OBJECT collision_45;
static NJS_MATERIAL collision_46_materials[1];
static Sint16 collision_46_faces[36];
static NJS_MESHSET collision_46_meshsets[1];
static NJS_POINT3 collision_46_points[22];
static NJS_MODEL collision_46_model;
static NJS_OBJECT collision_46;
static NJS_MATERIAL collision_47_materials[1];
static Sint16 collision_47_faces[69];
static NJS_MESHSET collision_47_meshsets[1];
static NJS_POINT3 collision_47_points[40];
static NJS_MODEL collision_47_model;
static NJS_OBJECT collision_47;
static NJS_MATERIAL collision_48_materials[1];
static Sint16 collision_48_faces[36];
static NJS_MESHSET collision_48_meshsets[1];
static NJS_POINT3 collision_48_points[22];
static NJS_MODEL collision_48_model;
static NJS_OBJECT collision_48;
static NJS_MATERIAL collision_50_materials[1];
static Sint16 collision_50_faces[14];
static NJS_MESHSET collision_50_meshsets[1];
static NJS_POINT3 collision_50_points[12];
static NJS_MODEL collision_50_model;
static NJS_OBJECT collision_50;
static NJS_MATERIAL collision_64_materials[1];
static Sint16 collision_64_faces[69];
static NJS_MESHSET collision_64_meshsets[1];
static NJS_POINT3 collision_64_points[40];
static NJS_MODEL collision_64_model;
static NJS_OBJECT collision_64;
static NJS_MATERIAL collision_65_materials[1];
static Sint16 collision_65_faces[69];
static NJS_MESHSET collision_65_meshsets[1];
static NJS_POINT3 collision_65_points[40];
static NJS_MODEL collision_65_model;
static NJS_OBJECT collision_65;
static NJS_MATERIAL collision_66_materials[1];
static Sint16 collision_66_faces[38];
static NJS_MESHSET collision_66_meshsets[1];
static NJS_POINT3 collision_66_points[24];
static NJS_MODEL collision_66_model;
static NJS_OBJECT collision_66;
static NJS_MATERIAL collision_78_materials[1];
static Sint16 collision_78_faces[10];
static NJS_MESHSET collision_78_meshsets[1];
static NJS_POINT3 collision_78_points[8];
static NJS_MODEL collision_78_model;
static NJS_OBJECT collision_78;
static NJS_MATERIAL collision_120_materials[1];
static Sint16 collision_120_faces[46];
static NJS_MESHSET collision_120_meshsets[1];
static NJS_POINT3 collision_120_points[30];
static NJS_MODEL collision_120_model;
static NJS_OBJECT collision_120;
static NJS_MATERIAL collision_121_materials[1];
static Sint16 collision_121_faces[69];
static NJS_MESHSET collision_121_meshsets[1];
static NJS_POINT3 collision_121_points[40];
static NJS_MODEL collision_121_model;
static NJS_OBJECT collision_121;
static NJS_MATERIAL collision_122_materials[1];
static Sint16 collision_122_faces[11];
static NJS_MESHSET collision_122_meshsets[1];
static NJS_POINT3 collision_122_points[10];
static NJS_MODEL collision_122_model;
static NJS_OBJECT collision_122;
static NJS_MATERIAL collision_129_materials[1];
static Sint16 collision_129_faces[18];
static NJS_MESHSET collision_129_meshsets[1];
static NJS_POINT3 collision_129_points[16];
static NJS_MODEL collision_129_model;
static NJS_OBJECT collision_129;
static NJS_MATERIAL collision_137_materials[1];
static Sint16 collision_137_faces[23];
static NJS_MESHSET collision_137_meshsets[1];
static NJS_POINT3 collision_137_points[22];
static NJS_MODEL collision_137_model;
static NJS_OBJECT collision_137;
static NJS_MATERIAL collision_138_materials[1];
static Sint16 collision_138_faces[18];
static NJS_MESHSET collision_138_meshsets[1];
static NJS_POINT3 collision_138_points[16];
static NJS_MODEL collision_138_model;
static NJS_OBJECT collision_138;
static NJS_MATERIAL collision_149_materials[1];
static Sint16 collision_149_faces[92];
static NJS_MESHSET collision_149_meshsets[1];
static NJS_POINT3 collision_149_points[41];
static NJS_MODEL collision_149_model;
static NJS_OBJECT collision_149;
static NJS_MATERIAL collision_150_materials[1];
static Sint16 collision_150_faces[11];
static NJS_MESHSET collision_150_meshsets[1];
static NJS_POINT3 collision_150_points[8];
static NJS_MODEL collision_150_model;
static NJS_OBJECT collision_150;
static NJS_MATERIAL collision_151_materials[1];
static Sint16 collision_151_faces[11];
static NJS_MESHSET collision_151_meshsets[1];
static NJS_POINT3 collision_151_points[8];
static NJS_MODEL collision_151_model;
static NJS_OBJECT collision_151;
static NJS_MATERIAL collision_152_materials[1];
static Sint16 collision_152_faces[11];
static NJS_MESHSET collision_152_meshsets[1];
static NJS_POINT3 collision_152_points[8];
static NJS_MODEL collision_152_model;
static NJS_OBJECT collision_152;
static NJS_MATERIAL collision_153_materials[1];
static Sint16 collision_153_faces[11];
static NJS_MESHSET collision_153_meshsets[1];
static NJS_POINT3 collision_153_points[8];
static NJS_MODEL collision_153_model;
static NJS_OBJECT collision_153;
static NJS_MATERIAL collision_154_materials[1];
static Sint16 collision_154_faces[11];
static NJS_MESHSET collision_154_meshsets[1];
static NJS_POINT3 collision_154_points[8];
static NJS_MODEL collision_154_model;
static NJS_OBJECT collision_154;
static NJS_MATERIAL collision_155_materials[1];
static Sint16 collision_155_faces[11];
static NJS_MESHSET collision_155_meshsets[1];
static NJS_POINT3 collision_155_points[8];
static NJS_MODEL collision_155_model;
static NJS_OBJECT collision_155;
static NJS_MATERIAL collision_156_materials[1];
static Sint16 collision_156_faces[11];
static NJS_MESHSET collision_156_meshsets[1];
static NJS_POINT3 collision_156_points[8];
static NJS_MODEL collision_156_model;
static NJS_OBJECT collision_156;
static NJS_MATERIAL collision_157_materials[1];
static Sint16 collision_157_faces[11];
static NJS_MESHSET collision_157_meshsets[1];
static NJS_POINT3 collision_157_points[8];
static NJS_MODEL collision_157_model;
static NJS_OBJECT collision_157;
static NJS_MATERIAL collision_158_materials[1];
static Sint16 collision_158_faces[11];
static NJS_MESHSET collision_158_meshsets[1];
static NJS_POINT3 collision_158_points[8];
static NJS_MODEL collision_158_model;
static NJS_OBJECT collision_158;
static NJS_MATERIAL collision_159_materials[1];
static Sint16 collision_159_faces[11];
static NJS_MESHSET collision_159_meshsets[1];
static NJS_POINT3 collision_159_points[8];
static NJS_MODEL collision_159_model;
static NJS_OBJECT collision_159;
static NJS_MATERIAL collision_160_materials[1];
static Sint16 collision_160_faces[367];
static NJS_MESHSET collision_160_meshsets[1];
static NJS_POINT3 collision_160_points[135];
static NJS_MODEL collision_160_model;
static NJS_OBJECT collision_160;
static NJS_MATERIAL collision_167_materials[1];
static Sint16 collision_167_faces[50];
static NJS_MESHSET collision_167_meshsets[1];
static NJS_POINT3 collision_167_points[22];
static NJS_MODEL collision_167_model;
static NJS_OBJECT collision_167;
static NJS_MATERIAL collision_169_materials[1];
static Sint16 collision_169_faces[124];
static NJS_MESHSET collision_169_meshsets[1];
static NJS_POINT3 collision_169_points[48];
static NJS_MODEL collision_169_model;
static NJS_OBJECT collision_169;
static NJS_MATERIAL collision_171_materials[1];
static Sint16 collision_171_faces[61];
static NJS_MESHSET collision_171_meshsets[1];
static NJS_POINT3 collision_171_points[29];
static NJS_MODEL collision_171_model;
static NJS_OBJECT collision_171;
static NJS_MATERIAL collision_172_materials[1];
static Sint16 collision_172_faces[47];
static NJS_MESHSET collision_172_meshsets[1];
static NJS_POINT3 collision_172_points[30];
static NJS_MODEL collision_172_model;
static NJS_OBJECT collision_172;
static NJS_MATERIAL collision_173_materials[1];
static Sint16 collision_173_faces[52];
static NJS_MESHSET collision_173_meshsets[1];
static NJS_POINT3 collision_173_points[24];
static NJS_MODEL collision_173_model;
static NJS_OBJECT collision_173;
static NJS_MATERIAL collision_176_materials[1];
static Sint16 collision_176_faces[20];
static NJS_MESHSET collision_176_meshsets[1];
static NJS_POINT3 collision_176_points[12];
static NJS_MODEL collision_176_model;
static NJS_OBJECT collision_176;
static NJS_MATERIAL collision_190_materials[1];
static Sint16 collision_190_faces[54];
static NJS_MESHSET collision_190_meshsets[1];
static NJS_POINT3 collision_190_points[27];
static NJS_MODEL collision_190_model;
static NJS_OBJECT collision_190;
static NJS_MATERIAL collision_191_materials[1];
static Sint16 collision_191_faces[53];
static NJS_MESHSET collision_191_meshsets[1];
static NJS_POINT3 collision_191_points[22];
static NJS_MODEL collision_191_model;
static NJS_OBJECT collision_191;
static NJS_MATERIAL collision_192_materials[1];
static Sint16 collision_192_faces[40];
static NJS_MESHSET collision_192_meshsets[1];
static NJS_POINT3 collision_192_points[24];
static NJS_MODEL collision_192_model;
static NJS_OBJECT collision_192;
static NJS_MATERIAL collision_194_materials[1];
static Sint16 collision_194_faces[56];
static NJS_MESHSET collision_194_meshsets[1];
static NJS_POINT3 collision_194_points[25];
static NJS_MODEL collision_194_model;
static NJS_OBJECT collision_194;
static NJS_MATERIAL collision_198_materials[1];
static Sint16 collision_198_faces[20];
static NJS_MESHSET collision_198_meshsets[1];
static NJS_POINT3 collision_198_points[10];
static NJS_MODEL collision_198_model;
static NJS_OBJECT collision_198;
static NJS_MATERIAL collision_240_materials[1];
static Sint16 collision_240_faces[126];
static NJS_MESHSET collision_240_meshsets[1];
static NJS_POINT3 collision_240_points[46];
static NJS_MODEL collision_240_model;
static NJS_OBJECT collision_240;
static NJS_MATERIAL collision_242_materials[1];
static Sint16 collision_242_faces[72];
static NJS_MESHSET collision_242_meshsets[1];
static NJS_POINT3 collision_242_points[29];
static NJS_MODEL collision_242_model;
static NJS_OBJECT collision_242;
static NJS_MATERIAL collision_247_materials[1];
static Sint16 collision_247_faces[7];
static NJS_MESHSET collision_247_meshsets[1];
static NJS_POINT3 collision_247_points[6];
static NJS_MODEL collision_247_model;
static NJS_OBJECT collision_247;
static NJS_MATERIAL collision_254_materials[1];
static Sint16 collision_254_faces[33];
static NJS_MESHSET collision_254_meshsets[1];
static NJS_POINT3 collision_254_points[14];
static NJS_MODEL collision_254_model;
static NJS_OBJECT collision_254;
static NJS_MATERIAL collision_261_materials[1];
static Sint16 collision_261_faces[36];
static NJS_MESHSET collision_261_meshsets[1];
static NJS_POINT3 collision_261_points[24];
static NJS_MODEL collision_261_model;
static NJS_OBJECT collision_261;
static NJS_MATERIAL collision_262_materials[1];
static Sint16 collision_262_faces[10];
static NJS_MESHSET collision_262_meshsets[1];
static NJS_POINT3 collision_262_points[8];
static NJS_MODEL collision_262_model;
static NJS_OBJECT collision_262;
static NJS_MATERIAL collision_269_materials[1];
static Sint16 collision_269_faces[10];
static NJS_MESHSET collision_269_meshsets[1];
static NJS_POINT3 collision_269_points[8];
static NJS_MODEL collision_269_model;
static NJS_OBJECT collision_269;
static NJS_MATERIAL collision_270_materials[1];
static Sint16 collision_270_faces[21];
static NJS_MESHSET collision_270_meshsets[1];
static NJS_POINT3 collision_270_points[8];
static NJS_MODEL collision_270_model;
static NJS_OBJECT collision_270;
static NJS_MATERIAL collision_278_materials[1];
static Sint16 collision_278_faces[66];
static NJS_MESHSET collision_278_meshsets[1];
static NJS_POINT3 collision_278_points[28];
static NJS_MODEL collision_278_model;
static NJS_OBJECT collision_278;
static NJS_MATERIAL collision_279_materials[1];
static Sint16 collision_279_faces[19];
static NJS_MESHSET collision_279_meshsets[1];
static NJS_POINT3 collision_279_points[18];
static NJS_MODEL collision_279_model;
static NJS_OBJECT collision_279;
static NJS_MATERIAL collision_280_materials[1];
static Sint16 collision_280_faces[14];
static NJS_MESHSET collision_280_meshsets[1];
static NJS_POINT3 collision_280_points[12];
static NJS_MODEL collision_280_model;
static NJS_OBJECT collision_280;
static NJS_MATERIAL collision_291_materials[1];
static Sint16 collision_291_faces[166];
static NJS_MESHSET collision_291_meshsets[1];
static NJS_POINT3 collision_291_points[72];
static NJS_MODEL collision_291_model;
static NJS_OBJECT collision_291;
static NJS_MATERIAL collision_292_materials[1];
static Sint16 collision_292_faces[31];
static NJS_MESHSET collision_292_meshsets[1];
static NJS_POINT3 collision_292_points[28];
static NJS_MODEL collision_292_model;
static NJS_OBJECT collision_292;
static NJS_MATERIAL collision_293_materials[1];
static Sint16 collision_293_faces[28];
static NJS_MESHSET collision_293_meshsets[1];
static NJS_POINT3 collision_293_points[12];
static NJS_MODEL collision_293_model;
static NJS_OBJECT collision_293;
static NJS_MATERIAL collision_304_materials[1];
static Sint16 collision_304_faces[41];
static NJS_MESHSET collision_304_meshsets[1];
static NJS_POINT3 collision_304_points[18];
static NJS_MODEL collision_304_model;
static NJS_OBJECT collision_304;
static Sint16 draw_45_polygons[131];
static Sint32 draw_45_vertices[119];
static NJS_CNK_MODEL draw_45_model;
static NJS_CNK_OBJECT draw_390;
static Sint16 draw_65_polygons[127];
static Sint32 draw_65_vertices[67];
static NJS_CNK_MODEL draw_65_model;
static NJS_CNK_OBJECT draw_388;
static Sint16 draw_15_polygons[197];
static Sint32 draw_15_vertices[163];
static NJS_CNK_MODEL draw_15_model;
static NJS_CNK_OBJECT draw_364;
static Sint16 draw_14_polygons[1183];
static Sint32 draw_14_vertices[1271];
static NJS_CNK_MODEL draw_14_model;
static NJS_CNK_OBJECT draw_363;
static Sint16 draw_30_polygons[315];
static Sint32 draw_30_vertices[351];
static NJS_CNK_MODEL draw_30_model;
static NJS_CNK_OBJECT draw_362;
static Sint16 draw_36_polygons[235];
static Sint32 draw_36_vertices[131];
static NJS_CNK_MODEL draw_36_model;
static NJS_CNK_OBJECT draw_361;
static Sint16 draw_33_polygons[1271];
static Sint32 draw_33_vertices[1059];
static NJS_CNK_MODEL draw_33_model;
static NJS_CNK_OBJECT draw_355;
static Sint16 draw_28_polygons[83];
static Sint32 draw_28_vertices[83];
static NJS_CNK_MODEL draw_28_model;
static NJS_CNK_OBJECT draw_354;
static Sint16 draw_13_polygons[455];
static Sint32 draw_13_vertices[483];
static NJS_CNK_MODEL draw_13_model;
static NJS_CNK_OBJECT draw_353;
static Sint16 draw_69_polygons[141];
static Sint32 draw_69_vertices[83];
static NJS_CNK_MODEL draw_69_model;
static NJS_CNK_OBJECT draw_316;
static Sint16 draw_315_polygons[63];
static Sint32 draw_315_vertices[67];
static NJS_CNK_MODEL draw_315_model;
static NJS_CNK_OBJECT draw_315;
static Sint16 draw_44_polygons[421];
static Sint32 draw_44_vertices[415];
static NJS_CNK_MODEL draw_44_model;
static NJS_CNK_OBJECT draw_310;
static Sint16 draw_57_polygons[3869];
static Sint32 draw_57_vertices[2815];
static NJS_CNK_MODEL draw_57_model;
static NJS_CNK_OBJECT draw_309;
static Sint16 draw_148_polygons[207];
static Sint32 draw_148_vertices[151];
static NJS_CNK_MODEL draw_148_model;
static NJS_CNK_OBJECT draw_308;
static Sint16 draw_206_polygons[83];
static Sint32 draw_206_vertices[51];
static NJS_CNK_MODEL draw_206_model;
static NJS_CNK_OBJECT draw_307;
static Sint16 draw_204_polygons[681];
static Sint32 draw_204_vertices[699];
static NJS_CNK_MODEL draw_204_model;
static NJS_CNK_OBJECT draw_304;
static Sint16 draw_202_polygons[1179];
static Sint32 draw_202_vertices[835];
static NJS_CNK_MODEL draw_202_model;
static NJS_CNK_OBJECT draw_303;
static Sint16 draw_94_polygons[83];
static Sint32 draw_94_vertices[51];
static NJS_CNK_MODEL draw_94_model;
static NJS_CNK_OBJECT draw_296;
static Sint16 draw_159_polygons[65];
static Sint32 draw_159_vertices[43];
static NJS_CNK_MODEL draw_159_model;
static NJS_CNK_OBJECT draw_290;
static Sint16 draw_82_polygons[425];
static Sint32 draw_82_vertices[419];
static NJS_CNK_MODEL draw_82_model;
static NJS_CNK_OBJECT draw_289;
static Sint16 draw_154_polygons[1021];
static Sint32 draw_154_vertices[755];
static NJS_CNK_MODEL draw_154_model;
static NJS_CNK_OBJECT draw_288;
static Sint16 draw_91_polygons[915];
static Sint32 draw_91_vertices[527];
static NJS_CNK_MODEL draw_91_model;
static NJS_CNK_OBJECT draw_287;
static Sint16 draw_151_polygons[845];
static Sint32 draw_151_vertices[907];
static NJS_CNK_MODEL draw_151_model;
static NJS_CNK_OBJECT draw_286;
static Sint16 draw_75_polygons[319];
static Sint32 draw_75_vertices[327];
static NJS_CNK_MODEL draw_75_model;
static NJS_CNK_OBJECT draw_279;
static Sint16 draw_73_polygons[311];
static Sint32 draw_73_vertices[283];
static NJS_CNK_MODEL draw_73_model;
static NJS_CNK_OBJECT draw_278;
static Sint16 draw_116_polygons[235];
static Sint32 draw_116_vertices[211];
static NJS_CNK_MODEL draw_116_model;
static NJS_CNK_OBJECT draw_276;
static Sint16 draw_17_polygons[939];
static Sint32 draw_17_vertices[887];
static NJS_CNK_MODEL draw_17_model;
static NJS_CNK_OBJECT draw_274;
static Sint16 draw_9_polygons[711];
static Sint32 draw_9_vertices[619];
static NJS_CNK_MODEL draw_9_model;
static NJS_CNK_OBJECT draw_273;
static Sint16 draw_16_polygons[853];
static Sint32 draw_16_vertices[735];
static NJS_CNK_MODEL draw_16_model;
static NJS_CNK_OBJECT draw_267;
static Sint16 draw_257_polygons[141];
static Sint32 draw_257_vertices[83];
static NJS_CNK_MODEL draw_257_model;
static NJS_CNK_OBJECT draw_257;
static Sint16 draw_256_polygons[141];
static Sint32 draw_256_vertices[83];
static NJS_CNK_MODEL draw_256_model;
static NJS_CNK_OBJECT draw_256;
static Sint16 draw_243_polygons[59];
static Sint32 draw_243_vertices[35];
static NJS_CNK_MODEL draw_243_model;
static NJS_CNK_OBJECT draw_243;
static Sint16 draw_83_polygons[393];
static Sint32 draw_83_vertices[239];
static NJS_CNK_MODEL draw_83_model;
static NJS_CNK_OBJECT draw_242;
static Sint16 draw_71_polygons[515];
static Sint32 draw_71_vertices[291];
static NJS_CNK_MODEL draw_71_model;
static NJS_CNK_OBJECT draw_238;
static Sint16 draw_163_polygons[145];
static Sint32 draw_163_vertices[163];
static NJS_CNK_MODEL draw_163_model;
static NJS_CNK_OBJECT draw_195;
static Sint16 draw_168_polygons[315];
static Sint32 draw_168_vertices[347];
static NJS_CNK_MODEL draw_168_model;
static NJS_CNK_OBJECT draw_194;
static Sint16 draw_175_polygons[135];
static Sint32 draw_175_vertices[123];
static NJS_CNK_MODEL draw_175_model;
static NJS_CNK_OBJECT draw_193;
static Sint16 draw_162_polygons[217];
static Sint32 draw_162_vertices[243];
static NJS_CNK_MODEL draw_162_model;
static NJS_CNK_OBJECT draw_192;
static Sint16 draw_173_polygons[873];
static Sint32 draw_173_vertices[555];
static NJS_CNK_MODEL draw_173_model;
static NJS_CNK_OBJECT draw_191;
static Sint16 draw_190_polygons[707];
static Sint32 draw_190_vertices[523];
static NJS_CNK_MODEL draw_190_model;
static NJS_CNK_OBJECT draw_190;
static Sint16 draw_189_polygons[165];
static Sint32 draw_189_vertices[151];
static NJS_CNK_MODEL draw_189_model;
static NJS_CNK_OBJECT draw_189;
static Sint16 draw_185_polygons[53];
static Sint32 draw_185_vertices[51];
static NJS_CNK_MODEL draw_185_model;
static NJS_CNK_OBJECT draw_187;
static Sint16 draw_184_polygons[165];
static Sint32 draw_184_vertices[159];
static NJS_CNK_MODEL draw_184_model;
static NJS_CNK_OBJECT draw_184;
static Sint16 draw_174_polygons[497];
static Sint32 draw_174_vertices[579];
static NJS_CNK_MODEL draw_174_model;
static NJS_CNK_OBJECT draw_174;
static Sint16 draw_160_polygons[179];
static Sint32 draw_160_vertices[187];
static NJS_CNK_MODEL draw_160_model;
static NJS_CNK_OBJECT draw_161;
static Sint16 draw_121_polygons[611];
static Sint32 draw_121_vertices[467];
static NJS_CNK_MODEL draw_121_model;
static NJS_CNK_OBJECT draw_141;
static Sint16 draw_139_polygons[69];
static Sint32 draw_139_vertices[39];
static NJS_CNK_MODEL draw_139_model;
static NJS_CNK_OBJECT draw_140;
static Sint16 draw_99_polygons[63];
static Sint32 draw_99_vertices[67];
static NJS_CNK_MODEL draw_99_model;
static NJS_CNK_OBJECT draw_99;
static Sint16 draw_77_polygons[193];
static Sint32 draw_77_vertices[159];
static NJS_CNK_MODEL draw_77_model;
static NJS_CNK_OBJECT draw_78;
static Sint16 draw_70_polygons[415];
static Sint32 draw_70_vertices[251];
static NJS_CNK_MODEL draw_70_model;
static NJS_CNK_OBJECT draw_70;
static Sint16 draw_68_polygons[291];
static Sint32 draw_68_vertices[163];
static NJS_CNK_MODEL draw_68_model;
static NJS_CNK_OBJECT draw_68;
static Sint16 draw_2_polygons[677];
static Sint32 draw_2_vertices[483];
static NJS_CNK_MODEL draw_2_model;
static NJS_CNK_OBJECT draw_12;
static Sint16 draw_11_polygons[803];
static Sint32 draw_11_vertices[523];
static NJS_CNK_MODEL draw_11_model;
static NJS_CNK_OBJECT draw_11;
static Sint16 draw_10_polygons[941];
static Sint32 draw_10_vertices[551];
static NJS_CNK_MODEL draw_10_model;
static NJS_CNK_OBJECT draw_10;
static Sint16 draw_1_polygons[23];
static Sint32 draw_1_vertices[19];
static NJS_CNK_MODEL draw_1_model;
static NJS_CNK_OBJECT draw_1;
static Sint16 draw_0_polygons[139];
static Sint32 draw_0_vertices[139];
static NJS_CNK_MODEL draw_0_model;
static NJS_CNK_OBJECT draw_0;
static NJS_OBJECT collision_4;
static NJS_OBJECT collision_7;
static NJS_OBJECT collision_11;
static NJS_OBJECT collision_19;
static NJS_OBJECT collision_20;
static NJS_OBJECT collision_21;
static NJS_OBJECT collision_23;
static NJS_OBJECT collision_24;
static NJS_OBJECT collision_25;
static NJS_OBJECT collision_26;
static NJS_OBJECT collision_29;
static NJS_OBJECT collision_32;
static NJS_OBJECT collision_35;
static NJS_OBJECT collision_36;
static NJS_OBJECT collision_37;
static NJS_OBJECT collision_40;
static NJS_OBJECT collision_41;
static NJS_OBJECT collision_49;
static NJS_OBJECT collision_51;
static NJS_OBJECT collision_52;
static NJS_OBJECT collision_53;
static NJS_OBJECT collision_54;
static NJS_OBJECT collision_55;
static NJS_OBJECT collision_56;
static NJS_OBJECT collision_57;
static NJS_OBJECT collision_58;
static NJS_OBJECT collision_59;
static NJS_OBJECT collision_60;
static NJS_OBJECT collision_61;
static NJS_OBJECT collision_62;
static NJS_OBJECT collision_63;
static NJS_OBJECT collision_67;
static NJS_OBJECT collision_68;
static NJS_OBJECT collision_69;
static NJS_OBJECT collision_70;
static NJS_OBJECT collision_71;
static NJS_OBJECT collision_72;
static NJS_OBJECT collision_73;
static NJS_OBJECT collision_74;
static NJS_OBJECT collision_75;
static NJS_OBJECT collision_76;
static NJS_OBJECT collision_77;
static NJS_OBJECT collision_79;
static NJS_OBJECT collision_80;
static NJS_OBJECT collision_81;
static NJS_OBJECT collision_82;
static NJS_OBJECT collision_83;
static NJS_OBJECT collision_84;
static NJS_OBJECT collision_85;
static NJS_OBJECT collision_86;
static NJS_OBJECT collision_87;
static NJS_OBJECT collision_88;
static NJS_OBJECT collision_89;
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
static NJS_OBJECT collision_100;
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
static NJS_OBJECT collision_123;
static NJS_OBJECT collision_124;
static NJS_OBJECT collision_125;
static NJS_OBJECT collision_126;
static NJS_OBJECT collision_127;
static NJS_OBJECT collision_128;
static NJS_OBJECT collision_130;
static NJS_OBJECT collision_131;
static NJS_OBJECT collision_132;
static NJS_OBJECT collision_133;
static NJS_OBJECT collision_134;
static NJS_OBJECT collision_135;
static NJS_OBJECT collision_136;
static NJS_OBJECT collision_139;
static NJS_OBJECT collision_140;
static NJS_OBJECT collision_141;
static NJS_OBJECT collision_142;
static NJS_OBJECT collision_143;
static NJS_OBJECT collision_144;
static NJS_OBJECT collision_145;
static NJS_OBJECT collision_146;
static NJS_OBJECT collision_147;
static NJS_OBJECT collision_148;
static NJS_OBJECT collision_161;
static NJS_OBJECT collision_162;
static NJS_OBJECT collision_163;
static NJS_OBJECT collision_164;
static NJS_OBJECT collision_165;
static NJS_OBJECT collision_166;
static NJS_OBJECT collision_168;
static NJS_OBJECT collision_170;
static NJS_OBJECT collision_174;
static NJS_OBJECT collision_175;
static NJS_OBJECT collision_177;
static NJS_OBJECT collision_178;
static NJS_OBJECT collision_179;
static NJS_OBJECT collision_180;
static NJS_OBJECT collision_181;
static NJS_OBJECT collision_182;
static NJS_OBJECT collision_183;
static NJS_OBJECT collision_184;
static NJS_OBJECT collision_185;
static NJS_OBJECT collision_186;
static NJS_OBJECT collision_187;
static NJS_OBJECT collision_188;
static NJS_OBJECT collision_189;
static NJS_OBJECT collision_193;
static NJS_OBJECT collision_195;
static NJS_OBJECT collision_196;
static NJS_OBJECT collision_197;
static NJS_OBJECT collision_199;
static NJS_OBJECT collision_200;
static NJS_OBJECT collision_201;
static NJS_OBJECT collision_202;
static NJS_OBJECT collision_203;
static NJS_OBJECT collision_204;
static NJS_OBJECT collision_205;
static NJS_OBJECT collision_206;
static NJS_OBJECT collision_207;
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
static NJS_OBJECT collision_223;
static NJS_OBJECT collision_224;
static NJS_OBJECT collision_225;
static NJS_OBJECT collision_226;
static NJS_OBJECT collision_227;
static NJS_OBJECT collision_228;
static NJS_OBJECT collision_229;
static NJS_OBJECT collision_230;
static NJS_OBJECT collision_231;
static NJS_OBJECT collision_232;
static NJS_OBJECT collision_233;
static NJS_OBJECT collision_234;
static NJS_OBJECT collision_235;
static NJS_OBJECT collision_236;
static NJS_OBJECT collision_237;
static NJS_OBJECT collision_238;
static NJS_OBJECT collision_239;
static NJS_OBJECT collision_241;
static NJS_OBJECT collision_243;
static NJS_OBJECT collision_244;
static NJS_OBJECT collision_245;
static NJS_OBJECT collision_246;
static NJS_OBJECT collision_248;
static NJS_OBJECT collision_249;
static NJS_OBJECT collision_250;
static NJS_OBJECT collision_251;
static NJS_OBJECT collision_252;
static NJS_OBJECT collision_253;
static NJS_OBJECT collision_255;
static NJS_OBJECT collision_256;
static NJS_OBJECT collision_257;
static NJS_OBJECT collision_258;
static NJS_OBJECT collision_259;
static NJS_OBJECT collision_260;
static NJS_OBJECT collision_263;
static NJS_OBJECT collision_264;
static NJS_OBJECT collision_265;
static NJS_OBJECT collision_266;
static NJS_OBJECT collision_267;
static NJS_OBJECT collision_268;
static NJS_OBJECT collision_271;
static NJS_OBJECT collision_272;
static NJS_OBJECT collision_273;
static NJS_OBJECT collision_274;
static NJS_OBJECT collision_275;
static NJS_OBJECT collision_276;
static NJS_OBJECT collision_277;
static NJS_OBJECT collision_281;
static NJS_OBJECT collision_282;
static NJS_OBJECT collision_283;
static NJS_OBJECT collision_284;
static NJS_OBJECT collision_285;
static NJS_OBJECT collision_286;
static NJS_OBJECT collision_287;
static NJS_OBJECT collision_288;
static NJS_OBJECT collision_289;
static NJS_OBJECT collision_290;
static NJS_OBJECT collision_294;
static NJS_OBJECT collision_295;
static NJS_OBJECT collision_296;
static NJS_OBJECT collision_297;
static NJS_OBJECT collision_298;
static NJS_OBJECT collision_299;
static NJS_OBJECT collision_300;
static NJS_OBJECT collision_301;
static NJS_OBJECT collision_302;
static NJS_OBJECT collision_303;
static NJS_CNK_OBJECT draw_392;
static NJS_CNK_OBJECT draw_391;
static NJS_CNK_OBJECT draw_389;
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
static NJS_CNK_OBJECT draw_360;
static NJS_CNK_OBJECT draw_359;
static NJS_CNK_OBJECT draw_358;
static NJS_CNK_OBJECT draw_357;
static NJS_CNK_OBJECT draw_356;
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
static NJS_CNK_OBJECT draw_314;
static NJS_CNK_OBJECT draw_313;
static NJS_CNK_OBJECT draw_312;
static NJS_CNK_OBJECT draw_311;
static NJS_CNK_OBJECT draw_306;
static NJS_CNK_OBJECT draw_305;
static NJS_CNK_OBJECT draw_302;
static NJS_CNK_OBJECT draw_301;
static NJS_CNK_OBJECT draw_300;
static NJS_CNK_OBJECT draw_299;
static NJS_CNK_OBJECT draw_298;
static NJS_CNK_OBJECT draw_297;
static NJS_CNK_OBJECT draw_295;
static NJS_CNK_OBJECT draw_294;
static NJS_CNK_OBJECT draw_293;
static NJS_CNK_OBJECT draw_292;
static NJS_CNK_OBJECT draw_291;
static NJS_CNK_OBJECT draw_285;
static NJS_CNK_OBJECT draw_284;
static NJS_CNK_OBJECT draw_283;
static NJS_CNK_OBJECT draw_282;
static NJS_CNK_OBJECT draw_281;
static NJS_CNK_OBJECT draw_280;
static NJS_CNK_OBJECT draw_277;
static NJS_CNK_OBJECT draw_275;
static NJS_CNK_OBJECT draw_272;
static NJS_CNK_OBJECT draw_271;
static NJS_CNK_OBJECT draw_270;
static NJS_CNK_OBJECT draw_269;
static NJS_CNK_OBJECT draw_268;
static NJS_CNK_OBJECT draw_266;
static NJS_CNK_OBJECT draw_265;
static NJS_CNK_OBJECT draw_264;
static NJS_CNK_OBJECT draw_263;
static NJS_CNK_OBJECT draw_262;
static NJS_CNK_OBJECT draw_261;
static NJS_CNK_OBJECT draw_260;
static NJS_CNK_OBJECT draw_259;
static NJS_CNK_OBJECT draw_258;
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
static NJS_CNK_OBJECT draw_241;
static NJS_CNK_OBJECT draw_240;
static NJS_CNK_OBJECT draw_239;
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
static NJS_CNK_OBJECT draw_218;
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
static NJS_CNK_OBJECT draw_188;
static NJS_CNK_OBJECT draw_186;
static NJS_CNK_OBJECT draw_185;
static NJS_CNK_OBJECT draw_183;
static NJS_CNK_OBJECT draw_182;
static NJS_CNK_OBJECT draw_181;
static NJS_CNK_OBJECT draw_180;
static NJS_CNK_OBJECT draw_179;
static NJS_CNK_OBJECT draw_178;
static NJS_CNK_OBJECT draw_177;
static NJS_CNK_OBJECT draw_176;
static NJS_CNK_OBJECT draw_175;
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
static NJS_CNK_OBJECT draw_139;
static NJS_CNK_OBJECT draw_138;
static NJS_CNK_OBJECT draw_137;
static NJS_CNK_OBJECT draw_136;
static NJS_CNK_OBJECT draw_135;
static NJS_CNK_OBJECT draw_134;
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
static NJS_CNK_OBJECT draw_77;
static NJS_CNK_OBJECT draw_76;
static NJS_CNK_OBJECT draw_75;
static NJS_CNK_OBJECT draw_74;
static NJS_CNK_OBJECT draw_73;
static NJS_CNK_OBJECT draw_72;
static NJS_CNK_OBJECT draw_71;
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
static NJS_CNK_OBJECT draw_17;
static NJS_CNK_OBJECT draw_16;
static NJS_CNK_OBJECT draw_15;
static NJS_CNK_OBJECT draw_14;
static NJS_CNK_OBJECT draw_13;
static NJS_CNK_OBJECT draw_9;
static NJS_CNK_OBJECT draw_8;
static NJS_CNK_OBJECT draw_7;
static NJS_CNK_OBJECT draw_6;
static NJS_CNK_OBJECT draw_5;
static NJS_CNK_OBJECT draw_4;
static NJS_CNK_OBJECT draw_3;
static NJS_CNK_OBJECT draw_2;

static struct land_entry entries[698] = {
#include "stg24/terrain/entries.inc"
};
struct land_table eternal_engine_land = {
#include "stg24/terrain/eternal_engine_land.inc"
};
static NJS_TEXNAME textures[41] = {
#include "stg24/terrain/textures.inc"
};
NJS_TEXLIST eternal_engine_land_texlist = {
#include "stg24/terrain/eternal_engine_land_texlist.inc"
};
static NJS_MATERIAL collision_0_materials[1] = {
#include "stg24/terrain/collision_0_materials.inc"
};
static Sint16 collision_0_faces[36] = {
#include "stg24/terrain/collision_0_faces.inc"
};
static NJS_MESHSET collision_0_meshsets[1] = {
#include "stg24/terrain/collision_0_meshsets.inc"
};
static NJS_POINT3 collision_0_points[24] = {
#include "stg24/terrain/collision_0_points.inc"
};
static NJS_MODEL collision_0_model = {
#include "stg24/terrain/collision_0_model.inc"
};
static NJS_OBJECT collision_0 = {
#include "stg24/terrain/collision_0.inc"
};
static NJS_MATERIAL collision_1_materials[1] = {
#include "stg24/terrain/collision_1_materials.inc"
};
static Sint16 collision_1_faces[54] = {
#include "stg24/terrain/collision_1_faces.inc"
};
static NJS_MESHSET collision_1_meshsets[1] = {
#include "stg24/terrain/collision_1_meshsets.inc"
};
static NJS_POINT3 collision_1_points[32] = {
#include "stg24/terrain/collision_1_points.inc"
};
static NJS_MODEL collision_1_model = {
#include "stg24/terrain/collision_1_model.inc"
};
static NJS_OBJECT collision_1 = {
#include "stg24/terrain/collision_1.inc"
};
static NJS_MATERIAL collision_2_materials[1] = {
#include "stg24/terrain/collision_2_materials.inc"
};
static Sint16 collision_2_faces[36] = {
#include "stg24/terrain/collision_2_faces.inc"
};
static NJS_MESHSET collision_2_meshsets[1] = {
#include "stg24/terrain/collision_2_meshsets.inc"
};
static NJS_POINT3 collision_2_points[24] = {
#include "stg24/terrain/collision_2_points.inc"
};
static NJS_MODEL collision_2_model = {
#include "stg24/terrain/collision_2_model.inc"
};
static NJS_OBJECT collision_2 = {
#include "stg24/terrain/collision_2.inc"
};
static NJS_MATERIAL collision_3_materials[1] = {
#include "stg24/terrain/collision_3_materials.inc"
};
static Sint16 collision_3_faces[9] = {
#include "stg24/terrain/collision_3_faces.inc"
};
static NJS_MESHSET collision_3_meshsets[1] = {
#include "stg24/terrain/collision_3_meshsets.inc"
};
static NJS_POINT3 collision_3_points[8] = {
#include "stg24/terrain/collision_3_points.inc"
};
static NJS_MODEL collision_3_model = {
#include "stg24/terrain/collision_3_model.inc"
};
static NJS_OBJECT collision_3 = {
#include "stg24/terrain/collision_3.inc"
};
static NJS_MATERIAL collision_5_materials[1] = {
#include "stg24/terrain/collision_5_materials.inc"
};
static Sint16 collision_5_faces[44] = {
#include "stg24/terrain/collision_5_faces.inc"
};
static NJS_MESHSET collision_5_meshsets[1] = {
#include "stg24/terrain/collision_5_meshsets.inc"
};
static NJS_POINT3 collision_5_points[30] = {
#include "stg24/terrain/collision_5_points.inc"
};
static NJS_MODEL collision_5_model = {
#include "stg24/terrain/collision_5_model.inc"
};
static NJS_OBJECT collision_5 = {
#include "stg24/terrain/collision_5.inc"
};
static NJS_MATERIAL collision_6_materials[1] = {
#include "stg24/terrain/collision_6_materials.inc"
};
static Sint16 collision_6_faces[9] = {
#include "stg24/terrain/collision_6_faces.inc"
};
static NJS_MESHSET collision_6_meshsets[1] = {
#include "stg24/terrain/collision_6_meshsets.inc"
};
static NJS_POINT3 collision_6_points[8] = {
#include "stg24/terrain/collision_6_points.inc"
};
static NJS_MODEL collision_6_model = {
#include "stg24/terrain/collision_6_model.inc"
};
static NJS_OBJECT collision_6 = {
#include "stg24/terrain/collision_6.inc"
};
static NJS_MATERIAL collision_8_materials[1] = {
#include "stg24/terrain/collision_8_materials.inc"
};
static Sint16 collision_8_faces[66] = {
#include "stg24/terrain/collision_8_faces.inc"
};
static NJS_MESHSET collision_8_meshsets[1] = {
#include "stg24/terrain/collision_8_meshsets.inc"
};
static NJS_POINT3 collision_8_points[40] = {
#include "stg24/terrain/collision_8_points.inc"
};
static NJS_MODEL collision_8_model = {
#include "stg24/terrain/collision_8_model.inc"
};
static NJS_OBJECT collision_8 = {
#include "stg24/terrain/collision_8.inc"
};
static NJS_MATERIAL collision_9_materials[1] = {
#include "stg24/terrain/collision_9_materials.inc"
};
static Sint16 collision_9_faces[44] = {
#include "stg24/terrain/collision_9_faces.inc"
};
static NJS_MESHSET collision_9_meshsets[1] = {
#include "stg24/terrain/collision_9_meshsets.inc"
};
static NJS_POINT3 collision_9_points[30] = {
#include "stg24/terrain/collision_9_points.inc"
};
static NJS_MODEL collision_9_model = {
#include "stg24/terrain/collision_9_model.inc"
};
static NJS_OBJECT collision_9 = {
#include "stg24/terrain/collision_9.inc"
};
static NJS_MATERIAL collision_10_materials[1] = {
#include "stg24/terrain/collision_10_materials.inc"
};
static Sint16 collision_10_faces[56] = {
#include "stg24/terrain/collision_10_faces.inc"
};
static NJS_MESHSET collision_10_meshsets[1] = {
#include "stg24/terrain/collision_10_meshsets.inc"
};
static NJS_POINT3 collision_10_points[24] = {
#include "stg24/terrain/collision_10_points.inc"
};
static NJS_MODEL collision_10_model = {
#include "stg24/terrain/collision_10_model.inc"
};
static NJS_OBJECT collision_10 = {
#include "stg24/terrain/collision_10.inc"
};
static NJS_MATERIAL collision_12_materials[1] = {
#include "stg24/terrain/collision_12_materials.inc"
};
static Sint16 collision_12_faces[72] = {
#include "stg24/terrain/collision_12_faces.inc"
};
static NJS_MESHSET collision_12_meshsets[1] = {
#include "stg24/terrain/collision_12_meshsets.inc"
};
static NJS_POINT3 collision_12_points[42] = {
#include "stg24/terrain/collision_12_points.inc"
};
static NJS_MODEL collision_12_model = {
#include "stg24/terrain/collision_12_model.inc"
};
static NJS_OBJECT collision_12 = {
#include "stg24/terrain/collision_12.inc"
};
static NJS_MATERIAL collision_13_materials[1] = {
#include "stg24/terrain/collision_13_materials.inc"
};
static Sint16 collision_13_faces[108] = {
#include "stg24/terrain/collision_13_faces.inc"
};
static NJS_MESHSET collision_13_meshsets[1] = {
#include "stg24/terrain/collision_13_meshsets.inc"
};
static NJS_POINT3 collision_13_points[56] = {
#include "stg24/terrain/collision_13_points.inc"
};
static NJS_MODEL collision_13_model = {
#include "stg24/terrain/collision_13_model.inc"
};
static NJS_OBJECT collision_13 = {
#include "stg24/terrain/collision_13.inc"
};
static NJS_MATERIAL collision_14_materials[1] = {
#include "stg24/terrain/collision_14_materials.inc"
};
static Sint16 collision_14_faces[75] = {
#include "stg24/terrain/collision_14_faces.inc"
};
static NJS_MESHSET collision_14_meshsets[1] = {
#include "stg24/terrain/collision_14_meshsets.inc"
};
static NJS_POINT3 collision_14_points[42] = {
#include "stg24/terrain/collision_14_points.inc"
};
static NJS_MODEL collision_14_model = {
#include "stg24/terrain/collision_14_model.inc"
};
static NJS_OBJECT collision_14 = {
#include "stg24/terrain/collision_14.inc"
};
static NJS_MATERIAL collision_15_materials[1] = {
#include "stg24/terrain/collision_15_materials.inc"
};
static Sint16 collision_15_faces[44] = {
#include "stg24/terrain/collision_15_faces.inc"
};
static NJS_MESHSET collision_15_meshsets[1] = {
#include "stg24/terrain/collision_15_meshsets.inc"
};
static NJS_POINT3 collision_15_points[30] = {
#include "stg24/terrain/collision_15_points.inc"
};
static NJS_MODEL collision_15_model = {
#include "stg24/terrain/collision_15_model.inc"
};
static NJS_OBJECT collision_15 = {
#include "stg24/terrain/collision_15.inc"
};
static NJS_MATERIAL collision_16_materials[1] = {
#include "stg24/terrain/collision_16_materials.inc"
};
static Sint16 collision_16_faces[45] = {
#include "stg24/terrain/collision_16_faces.inc"
};
static NJS_MESHSET collision_16_meshsets[1] = {
#include "stg24/terrain/collision_16_meshsets.inc"
};
static NJS_POINT3 collision_16_points[28] = {
#include "stg24/terrain/collision_16_points.inc"
};
static NJS_MODEL collision_16_model = {
#include "stg24/terrain/collision_16_model.inc"
};
static NJS_OBJECT collision_16 = {
#include "stg24/terrain/collision_16.inc"
};
static NJS_MATERIAL collision_17_materials[1] = {
#include "stg24/terrain/collision_17_materials.inc"
};
static Sint16 collision_17_faces[66] = {
#include "stg24/terrain/collision_17_faces.inc"
};
static NJS_MESHSET collision_17_meshsets[1] = {
#include "stg24/terrain/collision_17_meshsets.inc"
};
static NJS_POINT3 collision_17_points[40] = {
#include "stg24/terrain/collision_17_points.inc"
};
static NJS_MODEL collision_17_model = {
#include "stg24/terrain/collision_17_model.inc"
};
static NJS_OBJECT collision_17 = {
#include "stg24/terrain/collision_17.inc"
};
static NJS_MATERIAL collision_18_materials[1] = {
#include "stg24/terrain/collision_18_materials.inc"
};
static Sint16 collision_18_faces[28] = {
#include "stg24/terrain/collision_18_faces.inc"
};
static NJS_MESHSET collision_18_meshsets[1] = {
#include "stg24/terrain/collision_18_meshsets.inc"
};
static NJS_POINT3 collision_18_points[18] = {
#include "stg24/terrain/collision_18_points.inc"
};
static NJS_MODEL collision_18_model = {
#include "stg24/terrain/collision_18_model.inc"
};
static NJS_OBJECT collision_18 = {
#include "stg24/terrain/collision_18.inc"
};
static NJS_MATERIAL collision_22_materials[1] = {
#include "stg24/terrain/collision_22_materials.inc"
};
static Sint16 collision_22_faces[39] = {
#include "stg24/terrain/collision_22_faces.inc"
};
static NJS_MESHSET collision_22_meshsets[1] = {
#include "stg24/terrain/collision_22_meshsets.inc"
};
static NJS_POINT3 collision_22_points[20] = {
#include "stg24/terrain/collision_22_points.inc"
};
static NJS_MODEL collision_22_model = {
#include "stg24/terrain/collision_22_model.inc"
};
static NJS_OBJECT collision_22 = {
#include "stg24/terrain/collision_22.inc"
};
static NJS_MATERIAL collision_27_materials[1] = {
#include "stg24/terrain/collision_27_materials.inc"
};
static Sint16 collision_27_faces[30] = {
#include "stg24/terrain/collision_27_faces.inc"
};
static NJS_MESHSET collision_27_meshsets[1] = {
#include "stg24/terrain/collision_27_meshsets.inc"
};
static NJS_POINT3 collision_27_points[21] = {
#include "stg24/terrain/collision_27_points.inc"
};
static NJS_MODEL collision_27_model = {
#include "stg24/terrain/collision_27_model.inc"
};
static NJS_OBJECT collision_27 = {
#include "stg24/terrain/collision_27.inc"
};
static NJS_MATERIAL collision_28_materials[1] = {
#include "stg24/terrain/collision_28_materials.inc"
};
static Sint16 collision_28_faces[45] = {
#include "stg24/terrain/collision_28_faces.inc"
};
static NJS_MESHSET collision_28_meshsets[1] = {
#include "stg24/terrain/collision_28_meshsets.inc"
};
static NJS_POINT3 collision_28_points[28] = {
#include "stg24/terrain/collision_28_points.inc"
};
static NJS_MODEL collision_28_model = {
#include "stg24/terrain/collision_28_model.inc"
};
static NJS_OBJECT collision_28 = {
#include "stg24/terrain/collision_28.inc"
};
static NJS_MATERIAL collision_30_materials[1] = {
#include "stg24/terrain/collision_30_materials.inc"
};
static Sint16 collision_30_faces[60] = {
#include "stg24/terrain/collision_30_faces.inc"
};
static NJS_MESHSET collision_30_meshsets[1] = {
#include "stg24/terrain/collision_30_meshsets.inc"
};
static NJS_POINT3 collision_30_points[35] = {
#include "stg24/terrain/collision_30_points.inc"
};
static NJS_MODEL collision_30_model = {
#include "stg24/terrain/collision_30_model.inc"
};
static NJS_OBJECT collision_30 = {
#include "stg24/terrain/collision_30.inc"
};
static NJS_MATERIAL collision_31_materials[1] = {
#include "stg24/terrain/collision_31_materials.inc"
};
static Sint16 collision_31_faces[239] = {
#include "stg24/terrain/collision_31_faces.inc"
};
static NJS_MESHSET collision_31_meshsets[1] = {
#include "stg24/terrain/collision_31_meshsets.inc"
};
static NJS_POINT3 collision_31_points[110] = {
#include "stg24/terrain/collision_31_points.inc"
};
static NJS_MODEL collision_31_model = {
#include "stg24/terrain/collision_31_model.inc"
};
static NJS_OBJECT collision_31 = {
#include "stg24/terrain/collision_31.inc"
};
static NJS_MATERIAL collision_33_materials[1] = {
#include "stg24/terrain/collision_33_materials.inc"
};
static Sint16 collision_33_faces[44] = {
#include "stg24/terrain/collision_33_faces.inc"
};
static NJS_MESHSET collision_33_meshsets[1] = {
#include "stg24/terrain/collision_33_meshsets.inc"
};
static NJS_POINT3 collision_33_points[30] = {
#include "stg24/terrain/collision_33_points.inc"
};
static NJS_MODEL collision_33_model = {
#include "stg24/terrain/collision_33_model.inc"
};
static NJS_OBJECT collision_33 = {
#include "stg24/terrain/collision_33.inc"
};
static NJS_MATERIAL collision_34_materials[1] = {
#include "stg24/terrain/collision_34_materials.inc"
};
static Sint16 collision_34_faces[28] = {
#include "stg24/terrain/collision_34_faces.inc"
};
static NJS_MESHSET collision_34_meshsets[1] = {
#include "stg24/terrain/collision_34_meshsets.inc"
};
static NJS_POINT3 collision_34_points[18] = {
#include "stg24/terrain/collision_34_points.inc"
};
static NJS_MODEL collision_34_model = {
#include "stg24/terrain/collision_34_model.inc"
};
static NJS_OBJECT collision_34 = {
#include "stg24/terrain/collision_34.inc"
};
static NJS_MATERIAL collision_38_materials[1] = {
#include "stg24/terrain/collision_38_materials.inc"
};
static Sint16 collision_38_faces[30] = {
#include "stg24/terrain/collision_38_faces.inc"
};
static NJS_MESHSET collision_38_meshsets[1] = {
#include "stg24/terrain/collision_38_meshsets.inc"
};
static NJS_POINT3 collision_38_points[21] = {
#include "stg24/terrain/collision_38_points.inc"
};
static NJS_MODEL collision_38_model = {
#include "stg24/terrain/collision_38_model.inc"
};
static NJS_OBJECT collision_38 = {
#include "stg24/terrain/collision_38.inc"
};
static NJS_MATERIAL collision_39_materials[1] = {
#include "stg24/terrain/collision_39_materials.inc"
};
static Sint16 collision_39_faces[17] = {
#include "stg24/terrain/collision_39_faces.inc"
};
static NJS_MESHSET collision_39_meshsets[1] = {
#include "stg24/terrain/collision_39_meshsets.inc"
};
static NJS_POINT3 collision_39_points[14] = {
#include "stg24/terrain/collision_39_points.inc"
};
static NJS_MODEL collision_39_model = {
#include "stg24/terrain/collision_39_model.inc"
};
static NJS_OBJECT collision_39 = {
#include "stg24/terrain/collision_39.inc"
};
static NJS_MATERIAL collision_42_materials[1] = {
#include "stg24/terrain/collision_42_materials.inc"
};
static Sint16 collision_42_faces[17] = {
#include "stg24/terrain/collision_42_faces.inc"
};
static NJS_MESHSET collision_42_meshsets[1] = {
#include "stg24/terrain/collision_42_meshsets.inc"
};
static NJS_POINT3 collision_42_points[14] = {
#include "stg24/terrain/collision_42_points.inc"
};
static NJS_MODEL collision_42_model = {
#include "stg24/terrain/collision_42_model.inc"
};
static NJS_OBJECT collision_42 = {
#include "stg24/terrain/collision_42.inc"
};
static NJS_MATERIAL collision_43_materials[1] = {
#include "stg24/terrain/collision_43_materials.inc"
};
static Sint16 collision_43_faces[17] = {
#include "stg24/terrain/collision_43_faces.inc"
};
static NJS_MESHSET collision_43_meshsets[1] = {
#include "stg24/terrain/collision_43_meshsets.inc"
};
static NJS_POINT3 collision_43_points[10] = {
#include "stg24/terrain/collision_43_points.inc"
};
static NJS_MODEL collision_43_model = {
#include "stg24/terrain/collision_43_model.inc"
};
static NJS_OBJECT collision_43 = {
#include "stg24/terrain/collision_43.inc"
};
static NJS_MATERIAL collision_44_materials[1] = {
#include "stg24/terrain/collision_44_materials.inc"
};
static Sint16 collision_44_faces[17] = {
#include "stg24/terrain/collision_44_faces.inc"
};
static NJS_MESHSET collision_44_meshsets[1] = {
#include "stg24/terrain/collision_44_meshsets.inc"
};
static NJS_POINT3 collision_44_points[10] = {
#include "stg24/terrain/collision_44_points.inc"
};
static NJS_MODEL collision_44_model = {
#include "stg24/terrain/collision_44_model.inc"
};
static NJS_OBJECT collision_44 = {
#include "stg24/terrain/collision_44.inc"
};
static NJS_MATERIAL collision_45_materials[1] = {
#include "stg24/terrain/collision_45_materials.inc"
};
static Sint16 collision_45_faces[17] = {
#include "stg24/terrain/collision_45_faces.inc"
};
static NJS_MESHSET collision_45_meshsets[1] = {
#include "stg24/terrain/collision_45_meshsets.inc"
};
static NJS_POINT3 collision_45_points[10] = {
#include "stg24/terrain/collision_45_points.inc"
};
static NJS_MODEL collision_45_model = {
#include "stg24/terrain/collision_45_model.inc"
};
static NJS_OBJECT collision_45 = {
#include "stg24/terrain/collision_45.inc"
};
static NJS_MATERIAL collision_46_materials[1] = {
#include "stg24/terrain/collision_46_materials.inc"
};
static Sint16 collision_46_faces[36] = {
#include "stg24/terrain/collision_46_faces.inc"
};
static NJS_MESHSET collision_46_meshsets[1] = {
#include "stg24/terrain/collision_46_meshsets.inc"
};
static NJS_POINT3 collision_46_points[22] = {
#include "stg24/terrain/collision_46_points.inc"
};
static NJS_MODEL collision_46_model = {
#include "stg24/terrain/collision_46_model.inc"
};
static NJS_OBJECT collision_46 = {
#include "stg24/terrain/collision_46.inc"
};
static NJS_MATERIAL collision_47_materials[1] = {
#include "stg24/terrain/collision_47_materials.inc"
};
static Sint16 collision_47_faces[69] = {
#include "stg24/terrain/collision_47_faces.inc"
};
static NJS_MESHSET collision_47_meshsets[1] = {
#include "stg24/terrain/collision_47_meshsets.inc"
};
static NJS_POINT3 collision_47_points[40] = {
#include "stg24/terrain/collision_47_points.inc"
};
static NJS_MODEL collision_47_model = {
#include "stg24/terrain/collision_47_model.inc"
};
static NJS_OBJECT collision_47 = {
#include "stg24/terrain/collision_47.inc"
};
static NJS_MATERIAL collision_48_materials[1] = {
#include "stg24/terrain/collision_48_materials.inc"
};
static Sint16 collision_48_faces[36] = {
#include "stg24/terrain/collision_48_faces.inc"
};
static NJS_MESHSET collision_48_meshsets[1] = {
#include "stg24/terrain/collision_48_meshsets.inc"
};
static NJS_POINT3 collision_48_points[22] = {
#include "stg24/terrain/collision_48_points.inc"
};
static NJS_MODEL collision_48_model = {
#include "stg24/terrain/collision_48_model.inc"
};
static NJS_OBJECT collision_48 = {
#include "stg24/terrain/collision_48.inc"
};
static NJS_MATERIAL collision_50_materials[1] = {
#include "stg24/terrain/collision_50_materials.inc"
};
static Sint16 collision_50_faces[14] = {
#include "stg24/terrain/collision_50_faces.inc"
};
static NJS_MESHSET collision_50_meshsets[1] = {
#include "stg24/terrain/collision_50_meshsets.inc"
};
static NJS_POINT3 collision_50_points[12] = {
#include "stg24/terrain/collision_50_points.inc"
};
static NJS_MODEL collision_50_model = {
#include "stg24/terrain/collision_50_model.inc"
};
static NJS_OBJECT collision_50 = {
#include "stg24/terrain/collision_50.inc"
};
static NJS_MATERIAL collision_64_materials[1] = {
#include "stg24/terrain/collision_64_materials.inc"
};
static Sint16 collision_64_faces[69] = {
#include "stg24/terrain/collision_64_faces.inc"
};
static NJS_MESHSET collision_64_meshsets[1] = {
#include "stg24/terrain/collision_64_meshsets.inc"
};
static NJS_POINT3 collision_64_points[40] = {
#include "stg24/terrain/collision_64_points.inc"
};
static NJS_MODEL collision_64_model = {
#include "stg24/terrain/collision_64_model.inc"
};
static NJS_OBJECT collision_64 = {
#include "stg24/terrain/collision_64.inc"
};
static NJS_MATERIAL collision_65_materials[1] = {
#include "stg24/terrain/collision_65_materials.inc"
};
static Sint16 collision_65_faces[69] = {
#include "stg24/terrain/collision_65_faces.inc"
};
static NJS_MESHSET collision_65_meshsets[1] = {
#include "stg24/terrain/collision_65_meshsets.inc"
};
static NJS_POINT3 collision_65_points[40] = {
#include "stg24/terrain/collision_65_points.inc"
};
static NJS_MODEL collision_65_model = {
#include "stg24/terrain/collision_65_model.inc"
};
static NJS_OBJECT collision_65 = {
#include "stg24/terrain/collision_65.inc"
};
static NJS_MATERIAL collision_66_materials[1] = {
#include "stg24/terrain/collision_66_materials.inc"
};
static Sint16 collision_66_faces[38] = {
#include "stg24/terrain/collision_66_faces.inc"
};
static NJS_MESHSET collision_66_meshsets[1] = {
#include "stg24/terrain/collision_66_meshsets.inc"
};
static NJS_POINT3 collision_66_points[24] = {
#include "stg24/terrain/collision_66_points.inc"
};
static NJS_MODEL collision_66_model = {
#include "stg24/terrain/collision_66_model.inc"
};
static NJS_OBJECT collision_66 = {
#include "stg24/terrain/collision_66.inc"
};
static NJS_MATERIAL collision_78_materials[1] = {
#include "stg24/terrain/collision_78_materials.inc"
};
static Sint16 collision_78_faces[10] = {
#include "stg24/terrain/collision_78_faces.inc"
};
static NJS_MESHSET collision_78_meshsets[1] = {
#include "stg24/terrain/collision_78_meshsets.inc"
};
static NJS_POINT3 collision_78_points[8] = {
#include "stg24/terrain/collision_78_points.inc"
};
static NJS_MODEL collision_78_model = {
#include "stg24/terrain/collision_78_model.inc"
};
static NJS_OBJECT collision_78 = {
#include "stg24/terrain/collision_78.inc"
};
static NJS_MATERIAL collision_120_materials[1] = {
#include "stg24/terrain/collision_120_materials.inc"
};
static Sint16 collision_120_faces[46] = {
#include "stg24/terrain/collision_120_faces.inc"
};
static NJS_MESHSET collision_120_meshsets[1] = {
#include "stg24/terrain/collision_120_meshsets.inc"
};
static NJS_POINT3 collision_120_points[30] = {
#include "stg24/terrain/collision_120_points.inc"
};
static NJS_MODEL collision_120_model = {
#include "stg24/terrain/collision_120_model.inc"
};
static NJS_OBJECT collision_120 = {
#include "stg24/terrain/collision_120.inc"
};
static NJS_MATERIAL collision_121_materials[1] = {
#include "stg24/terrain/collision_121_materials.inc"
};
static Sint16 collision_121_faces[69] = {
#include "stg24/terrain/collision_121_faces.inc"
};
static NJS_MESHSET collision_121_meshsets[1] = {
#include "stg24/terrain/collision_121_meshsets.inc"
};
static NJS_POINT3 collision_121_points[40] = {
#include "stg24/terrain/collision_121_points.inc"
};
static NJS_MODEL collision_121_model = {
#include "stg24/terrain/collision_121_model.inc"
};
static NJS_OBJECT collision_121 = {
#include "stg24/terrain/collision_121.inc"
};
static NJS_MATERIAL collision_122_materials[1] = {
#include "stg24/terrain/collision_122_materials.inc"
};
static Sint16 collision_122_faces[11] = {
#include "stg24/terrain/collision_122_faces.inc"
};
static NJS_MESHSET collision_122_meshsets[1] = {
#include "stg24/terrain/collision_122_meshsets.inc"
};
static NJS_POINT3 collision_122_points[10] = {
#include "stg24/terrain/collision_122_points.inc"
};
static NJS_MODEL collision_122_model = {
#include "stg24/terrain/collision_122_model.inc"
};
static NJS_OBJECT collision_122 = {
#include "stg24/terrain/collision_122.inc"
};
static NJS_MATERIAL collision_129_materials[1] = {
#include "stg24/terrain/collision_129_materials.inc"
};
static Sint16 collision_129_faces[18] = {
#include "stg24/terrain/collision_129_faces.inc"
};
static NJS_MESHSET collision_129_meshsets[1] = {
#include "stg24/terrain/collision_129_meshsets.inc"
};
static NJS_POINT3 collision_129_points[16] = {
#include "stg24/terrain/collision_129_points.inc"
};
static NJS_MODEL collision_129_model = {
#include "stg24/terrain/collision_129_model.inc"
};
static NJS_OBJECT collision_129 = {
#include "stg24/terrain/collision_129.inc"
};
static NJS_MATERIAL collision_137_materials[1] = {
#include "stg24/terrain/collision_137_materials.inc"
};
static Sint16 collision_137_faces[23] = {
#include "stg24/terrain/collision_137_faces.inc"
};
static NJS_MESHSET collision_137_meshsets[1] = {
#include "stg24/terrain/collision_137_meshsets.inc"
};
static NJS_POINT3 collision_137_points[22] = {
#include "stg24/terrain/collision_137_points.inc"
};
static NJS_MODEL collision_137_model = {
#include "stg24/terrain/collision_137_model.inc"
};
static NJS_OBJECT collision_137 = {
#include "stg24/terrain/collision_137.inc"
};
static NJS_MATERIAL collision_138_materials[1] = {
#include "stg24/terrain/collision_138_materials.inc"
};
static Sint16 collision_138_faces[18] = {
#include "stg24/terrain/collision_138_faces.inc"
};
static NJS_MESHSET collision_138_meshsets[1] = {
#include "stg24/terrain/collision_138_meshsets.inc"
};
static NJS_POINT3 collision_138_points[16] = {
#include "stg24/terrain/collision_138_points.inc"
};
static NJS_MODEL collision_138_model = {
#include "stg24/terrain/collision_138_model.inc"
};
static NJS_OBJECT collision_138 = {
#include "stg24/terrain/collision_138.inc"
};
static NJS_MATERIAL collision_149_materials[1] = {
#include "stg24/terrain/collision_149_materials.inc"
};
static Sint16 collision_149_faces[92] = {
#include "stg24/terrain/collision_149_faces.inc"
};
static NJS_MESHSET collision_149_meshsets[1] = {
#include "stg24/terrain/collision_149_meshsets.inc"
};
static NJS_POINT3 collision_149_points[41] = {
#include "stg24/terrain/collision_149_points.inc"
};
static NJS_MODEL collision_149_model = {
#include "stg24/terrain/collision_149_model.inc"
};
static NJS_OBJECT collision_149 = {
#include "stg24/terrain/collision_149.inc"
};
static NJS_MATERIAL collision_150_materials[1] = {
#include "stg24/terrain/collision_150_materials.inc"
};
static Sint16 collision_150_faces[11] = {
#include "stg24/terrain/collision_150_faces.inc"
};
static NJS_MESHSET collision_150_meshsets[1] = {
#include "stg24/terrain/collision_150_meshsets.inc"
};
static NJS_POINT3 collision_150_points[8] = {
#include "stg24/terrain/collision_150_points.inc"
};
static NJS_MODEL collision_150_model = {
#include "stg24/terrain/collision_150_model.inc"
};
static NJS_OBJECT collision_150 = {
#include "stg24/terrain/collision_150.inc"
};
static NJS_MATERIAL collision_151_materials[1] = {
#include "stg24/terrain/collision_151_materials.inc"
};
static Sint16 collision_151_faces[11] = {
#include "stg24/terrain/collision_151_faces.inc"
};
static NJS_MESHSET collision_151_meshsets[1] = {
#include "stg24/terrain/collision_151_meshsets.inc"
};
static NJS_POINT3 collision_151_points[8] = {
#include "stg24/terrain/collision_151_points.inc"
};
static NJS_MODEL collision_151_model = {
#include "stg24/terrain/collision_151_model.inc"
};
static NJS_OBJECT collision_151 = {
#include "stg24/terrain/collision_151.inc"
};
static NJS_MATERIAL collision_152_materials[1] = {
#include "stg24/terrain/collision_152_materials.inc"
};
static Sint16 collision_152_faces[11] = {
#include "stg24/terrain/collision_152_faces.inc"
};
static NJS_MESHSET collision_152_meshsets[1] = {
#include "stg24/terrain/collision_152_meshsets.inc"
};
static NJS_POINT3 collision_152_points[8] = {
#include "stg24/terrain/collision_152_points.inc"
};
static NJS_MODEL collision_152_model = {
#include "stg24/terrain/collision_152_model.inc"
};
static NJS_OBJECT collision_152 = {
#include "stg24/terrain/collision_152.inc"
};
static NJS_MATERIAL collision_153_materials[1] = {
#include "stg24/terrain/collision_153_materials.inc"
};
static Sint16 collision_153_faces[11] = {
#include "stg24/terrain/collision_153_faces.inc"
};
static NJS_MESHSET collision_153_meshsets[1] = {
#include "stg24/terrain/collision_153_meshsets.inc"
};
static NJS_POINT3 collision_153_points[8] = {
#include "stg24/terrain/collision_153_points.inc"
};
static NJS_MODEL collision_153_model = {
#include "stg24/terrain/collision_153_model.inc"
};
static NJS_OBJECT collision_153 = {
#include "stg24/terrain/collision_153.inc"
};
static NJS_MATERIAL collision_154_materials[1] = {
#include "stg24/terrain/collision_154_materials.inc"
};
static Sint16 collision_154_faces[11] = {
#include "stg24/terrain/collision_154_faces.inc"
};
static NJS_MESHSET collision_154_meshsets[1] = {
#include "stg24/terrain/collision_154_meshsets.inc"
};
static NJS_POINT3 collision_154_points[8] = {
#include "stg24/terrain/collision_154_points.inc"
};
static NJS_MODEL collision_154_model = {
#include "stg24/terrain/collision_154_model.inc"
};
static NJS_OBJECT collision_154 = {
#include "stg24/terrain/collision_154.inc"
};
static NJS_MATERIAL collision_155_materials[1] = {
#include "stg24/terrain/collision_155_materials.inc"
};
static Sint16 collision_155_faces[11] = {
#include "stg24/terrain/collision_155_faces.inc"
};
static NJS_MESHSET collision_155_meshsets[1] = {
#include "stg24/terrain/collision_155_meshsets.inc"
};
static NJS_POINT3 collision_155_points[8] = {
#include "stg24/terrain/collision_155_points.inc"
};
static NJS_MODEL collision_155_model = {
#include "stg24/terrain/collision_155_model.inc"
};
static NJS_OBJECT collision_155 = {
#include "stg24/terrain/collision_155.inc"
};
static NJS_MATERIAL collision_156_materials[1] = {
#include "stg24/terrain/collision_156_materials.inc"
};
static Sint16 collision_156_faces[11] = {
#include "stg24/terrain/collision_156_faces.inc"
};
static NJS_MESHSET collision_156_meshsets[1] = {
#include "stg24/terrain/collision_156_meshsets.inc"
};
static NJS_POINT3 collision_156_points[8] = {
#include "stg24/terrain/collision_156_points.inc"
};
static NJS_MODEL collision_156_model = {
#include "stg24/terrain/collision_156_model.inc"
};
static NJS_OBJECT collision_156 = {
#include "stg24/terrain/collision_156.inc"
};
static NJS_MATERIAL collision_157_materials[1] = {
#include "stg24/terrain/collision_157_materials.inc"
};
static Sint16 collision_157_faces[11] = {
#include "stg24/terrain/collision_157_faces.inc"
};
static NJS_MESHSET collision_157_meshsets[1] = {
#include "stg24/terrain/collision_157_meshsets.inc"
};
static NJS_POINT3 collision_157_points[8] = {
#include "stg24/terrain/collision_157_points.inc"
};
static NJS_MODEL collision_157_model = {
#include "stg24/terrain/collision_157_model.inc"
};
static NJS_OBJECT collision_157 = {
#include "stg24/terrain/collision_157.inc"
};
static NJS_MATERIAL collision_158_materials[1] = {
#include "stg24/terrain/collision_158_materials.inc"
};
static Sint16 collision_158_faces[11] = {
#include "stg24/terrain/collision_158_faces.inc"
};
static NJS_MESHSET collision_158_meshsets[1] = {
#include "stg24/terrain/collision_158_meshsets.inc"
};
static NJS_POINT3 collision_158_points[8] = {
#include "stg24/terrain/collision_158_points.inc"
};
static NJS_MODEL collision_158_model = {
#include "stg24/terrain/collision_158_model.inc"
};
static NJS_OBJECT collision_158 = {
#include "stg24/terrain/collision_158.inc"
};
static NJS_MATERIAL collision_159_materials[1] = {
#include "stg24/terrain/collision_159_materials.inc"
};
static Sint16 collision_159_faces[11] = {
#include "stg24/terrain/collision_159_faces.inc"
};
static NJS_MESHSET collision_159_meshsets[1] = {
#include "stg24/terrain/collision_159_meshsets.inc"
};
static NJS_POINT3 collision_159_points[8] = {
#include "stg24/terrain/collision_159_points.inc"
};
static NJS_MODEL collision_159_model = {
#include "stg24/terrain/collision_159_model.inc"
};
static NJS_OBJECT collision_159 = {
#include "stg24/terrain/collision_159.inc"
};
static NJS_MATERIAL collision_160_materials[1] = {
#include "stg24/terrain/collision_160_materials.inc"
};
static Sint16 collision_160_faces[367] = {
#include "stg24/terrain/collision_160_faces.inc"
};
static NJS_MESHSET collision_160_meshsets[1] = {
#include "stg24/terrain/collision_160_meshsets.inc"
};
static NJS_POINT3 collision_160_points[135] = {
#include "stg24/terrain/collision_160_points.inc"
};
static NJS_MODEL collision_160_model = {
#include "stg24/terrain/collision_160_model.inc"
};
static NJS_OBJECT collision_160 = {
#include "stg24/terrain/collision_160.inc"
};
static NJS_MATERIAL collision_167_materials[1] = {
#include "stg24/terrain/collision_167_materials.inc"
};
static Sint16 collision_167_faces[50] = {
#include "stg24/terrain/collision_167_faces.inc"
};
static NJS_MESHSET collision_167_meshsets[1] = {
#include "stg24/terrain/collision_167_meshsets.inc"
};
static NJS_POINT3 collision_167_points[22] = {
#include "stg24/terrain/collision_167_points.inc"
};
static NJS_MODEL collision_167_model = {
#include "stg24/terrain/collision_167_model.inc"
};
static NJS_OBJECT collision_167 = {
#include "stg24/terrain/collision_167.inc"
};
static NJS_MATERIAL collision_169_materials[1] = {
#include "stg24/terrain/collision_169_materials.inc"
};
static Sint16 collision_169_faces[124] = {
#include "stg24/terrain/collision_169_faces.inc"
};
static NJS_MESHSET collision_169_meshsets[1] = {
#include "stg24/terrain/collision_169_meshsets.inc"
};
static NJS_POINT3 collision_169_points[48] = {
#include "stg24/terrain/collision_169_points.inc"
};
static NJS_MODEL collision_169_model = {
#include "stg24/terrain/collision_169_model.inc"
};
static NJS_OBJECT collision_169 = {
#include "stg24/terrain/collision_169.inc"
};
static NJS_MATERIAL collision_171_materials[1] = {
#include "stg24/terrain/collision_171_materials.inc"
};
static Sint16 collision_171_faces[61] = {
#include "stg24/terrain/collision_171_faces.inc"
};
static NJS_MESHSET collision_171_meshsets[1] = {
#include "stg24/terrain/collision_171_meshsets.inc"
};
static NJS_POINT3 collision_171_points[29] = {
#include "stg24/terrain/collision_171_points.inc"
};
static NJS_MODEL collision_171_model = {
#include "stg24/terrain/collision_171_model.inc"
};
static NJS_OBJECT collision_171 = {
#include "stg24/terrain/collision_171.inc"
};
static NJS_MATERIAL collision_172_materials[1] = {
#include "stg24/terrain/collision_172_materials.inc"
};
static Sint16 collision_172_faces[47] = {
#include "stg24/terrain/collision_172_faces.inc"
};
static NJS_MESHSET collision_172_meshsets[1] = {
#include "stg24/terrain/collision_172_meshsets.inc"
};
static NJS_POINT3 collision_172_points[30] = {
#include "stg24/terrain/collision_172_points.inc"
};
static NJS_MODEL collision_172_model = {
#include "stg24/terrain/collision_172_model.inc"
};
static NJS_OBJECT collision_172 = {
#include "stg24/terrain/collision_172.inc"
};
static NJS_MATERIAL collision_173_materials[1] = {
#include "stg24/terrain/collision_173_materials.inc"
};
static Sint16 collision_173_faces[52] = {
#include "stg24/terrain/collision_173_faces.inc"
};
static NJS_MESHSET collision_173_meshsets[1] = {
#include "stg24/terrain/collision_173_meshsets.inc"
};
static NJS_POINT3 collision_173_points[24] = {
#include "stg24/terrain/collision_173_points.inc"
};
static NJS_MODEL collision_173_model = {
#include "stg24/terrain/collision_173_model.inc"
};
static NJS_OBJECT collision_173 = {
#include "stg24/terrain/collision_173.inc"
};
static NJS_MATERIAL collision_176_materials[1] = {
#include "stg24/terrain/collision_176_materials.inc"
};
static Sint16 collision_176_faces[20] = {
#include "stg24/terrain/collision_176_faces.inc"
};
static NJS_MESHSET collision_176_meshsets[1] = {
#include "stg24/terrain/collision_176_meshsets.inc"
};
static NJS_POINT3 collision_176_points[12] = {
#include "stg24/terrain/collision_176_points.inc"
};
static NJS_MODEL collision_176_model = {
#include "stg24/terrain/collision_176_model.inc"
};
static NJS_OBJECT collision_176 = {
#include "stg24/terrain/collision_176.inc"
};
static NJS_MATERIAL collision_190_materials[1] = {
#include "stg24/terrain/collision_190_materials.inc"
};
static Sint16 collision_190_faces[54] = {
#include "stg24/terrain/collision_190_faces.inc"
};
static NJS_MESHSET collision_190_meshsets[1] = {
#include "stg24/terrain/collision_190_meshsets.inc"
};
static NJS_POINT3 collision_190_points[27] = {
#include "stg24/terrain/collision_190_points.inc"
};
static NJS_MODEL collision_190_model = {
#include "stg24/terrain/collision_190_model.inc"
};
static NJS_OBJECT collision_190 = {
#include "stg24/terrain/collision_190.inc"
};
static NJS_MATERIAL collision_191_materials[1] = {
#include "stg24/terrain/collision_191_materials.inc"
};
static Sint16 collision_191_faces[53] = {
#include "stg24/terrain/collision_191_faces.inc"
};
static NJS_MESHSET collision_191_meshsets[1] = {
#include "stg24/terrain/collision_191_meshsets.inc"
};
static NJS_POINT3 collision_191_points[22] = {
#include "stg24/terrain/collision_191_points.inc"
};
static NJS_MODEL collision_191_model = {
#include "stg24/terrain/collision_191_model.inc"
};
static NJS_OBJECT collision_191 = {
#include "stg24/terrain/collision_191.inc"
};
static NJS_MATERIAL collision_192_materials[1] = {
#include "stg24/terrain/collision_192_materials.inc"
};
static Sint16 collision_192_faces[40] = {
#include "stg24/terrain/collision_192_faces.inc"
};
static NJS_MESHSET collision_192_meshsets[1] = {
#include "stg24/terrain/collision_192_meshsets.inc"
};
static NJS_POINT3 collision_192_points[24] = {
#include "stg24/terrain/collision_192_points.inc"
};
static NJS_MODEL collision_192_model = {
#include "stg24/terrain/collision_192_model.inc"
};
static NJS_OBJECT collision_192 = {
#include "stg24/terrain/collision_192.inc"
};
static NJS_MATERIAL collision_194_materials[1] = {
#include "stg24/terrain/collision_194_materials.inc"
};
static Sint16 collision_194_faces[56] = {
#include "stg24/terrain/collision_194_faces.inc"
};
static NJS_MESHSET collision_194_meshsets[1] = {
#include "stg24/terrain/collision_194_meshsets.inc"
};
static NJS_POINT3 collision_194_points[25] = {
#include "stg24/terrain/collision_194_points.inc"
};
static NJS_MODEL collision_194_model = {
#include "stg24/terrain/collision_194_model.inc"
};
static NJS_OBJECT collision_194 = {
#include "stg24/terrain/collision_194.inc"
};
static NJS_MATERIAL collision_198_materials[1] = {
#include "stg24/terrain/collision_198_materials.inc"
};
static Sint16 collision_198_faces[20] = {
#include "stg24/terrain/collision_198_faces.inc"
};
static NJS_MESHSET collision_198_meshsets[1] = {
#include "stg24/terrain/collision_198_meshsets.inc"
};
static NJS_POINT3 collision_198_points[10] = {
#include "stg24/terrain/collision_198_points.inc"
};
static NJS_MODEL collision_198_model = {
#include "stg24/terrain/collision_198_model.inc"
};
static NJS_OBJECT collision_198 = {
#include "stg24/terrain/collision_198.inc"
};
static NJS_MATERIAL collision_240_materials[1] = {
#include "stg24/terrain/collision_240_materials.inc"
};
static Sint16 collision_240_faces[126] = {
#include "stg24/terrain/collision_240_faces.inc"
};
static NJS_MESHSET collision_240_meshsets[1] = {
#include "stg24/terrain/collision_240_meshsets.inc"
};
static NJS_POINT3 collision_240_points[46] = {
#include "stg24/terrain/collision_240_points.inc"
};
static NJS_MODEL collision_240_model = {
#include "stg24/terrain/collision_240_model.inc"
};
static NJS_OBJECT collision_240 = {
#include "stg24/terrain/collision_240.inc"
};
static NJS_MATERIAL collision_242_materials[1] = {
#include "stg24/terrain/collision_242_materials.inc"
};
static Sint16 collision_242_faces[72] = {
#include "stg24/terrain/collision_242_faces.inc"
};
static NJS_MESHSET collision_242_meshsets[1] = {
#include "stg24/terrain/collision_242_meshsets.inc"
};
static NJS_POINT3 collision_242_points[29] = {
#include "stg24/terrain/collision_242_points.inc"
};
static NJS_MODEL collision_242_model = {
#include "stg24/terrain/collision_242_model.inc"
};
static NJS_OBJECT collision_242 = {
#include "stg24/terrain/collision_242.inc"
};
static NJS_MATERIAL collision_247_materials[1] = {
#include "stg24/terrain/collision_247_materials.inc"
};
static Sint16 collision_247_faces[7] = {
#include "stg24/terrain/collision_247_faces.inc"
};
static NJS_MESHSET collision_247_meshsets[1] = {
#include "stg24/terrain/collision_247_meshsets.inc"
};
static NJS_POINT3 collision_247_points[6] = {
#include "stg24/terrain/collision_247_points.inc"
};
static NJS_MODEL collision_247_model = {
#include "stg24/terrain/collision_247_model.inc"
};
static NJS_OBJECT collision_247 = {
#include "stg24/terrain/collision_247.inc"
};
static NJS_MATERIAL collision_254_materials[1] = {
#include "stg24/terrain/collision_254_materials.inc"
};
static Sint16 collision_254_faces[33] = {
#include "stg24/terrain/collision_254_faces.inc"
};
static NJS_MESHSET collision_254_meshsets[1] = {
#include "stg24/terrain/collision_254_meshsets.inc"
};
static NJS_POINT3 collision_254_points[14] = {
#include "stg24/terrain/collision_254_points.inc"
};
static NJS_MODEL collision_254_model = {
#include "stg24/terrain/collision_254_model.inc"
};
static NJS_OBJECT collision_254 = {
#include "stg24/terrain/collision_254.inc"
};
static NJS_MATERIAL collision_261_materials[1] = {
#include "stg24/terrain/collision_261_materials.inc"
};
static Sint16 collision_261_faces[36] = {
#include "stg24/terrain/collision_261_faces.inc"
};
static NJS_MESHSET collision_261_meshsets[1] = {
#include "stg24/terrain/collision_261_meshsets.inc"
};
static NJS_POINT3 collision_261_points[24] = {
#include "stg24/terrain/collision_261_points.inc"
};
static NJS_MODEL collision_261_model = {
#include "stg24/terrain/collision_261_model.inc"
};
static NJS_OBJECT collision_261 = {
#include "stg24/terrain/collision_261.inc"
};
static NJS_MATERIAL collision_262_materials[1] = {
#include "stg24/terrain/collision_262_materials.inc"
};
static Sint16 collision_262_faces[10] = {
#include "stg24/terrain/collision_262_faces.inc"
};
static NJS_MESHSET collision_262_meshsets[1] = {
#include "stg24/terrain/collision_262_meshsets.inc"
};
static NJS_POINT3 collision_262_points[8] = {
#include "stg24/terrain/collision_262_points.inc"
};
static NJS_MODEL collision_262_model = {
#include "stg24/terrain/collision_262_model.inc"
};
static NJS_OBJECT collision_262 = {
#include "stg24/terrain/collision_262.inc"
};
static NJS_MATERIAL collision_269_materials[1] = {
#include "stg24/terrain/collision_269_materials.inc"
};
static Sint16 collision_269_faces[10] = {
#include "stg24/terrain/collision_269_faces.inc"
};
static NJS_MESHSET collision_269_meshsets[1] = {
#include "stg24/terrain/collision_269_meshsets.inc"
};
static NJS_POINT3 collision_269_points[8] = {
#include "stg24/terrain/collision_269_points.inc"
};
static NJS_MODEL collision_269_model = {
#include "stg24/terrain/collision_269_model.inc"
};
static NJS_OBJECT collision_269 = {
#include "stg24/terrain/collision_269.inc"
};
static NJS_MATERIAL collision_270_materials[1] = {
#include "stg24/terrain/collision_270_materials.inc"
};
static Sint16 collision_270_faces[21] = {
#include "stg24/terrain/collision_270_faces.inc"
};
static NJS_MESHSET collision_270_meshsets[1] = {
#include "stg24/terrain/collision_270_meshsets.inc"
};
static NJS_POINT3 collision_270_points[8] = {
#include "stg24/terrain/collision_270_points.inc"
};
static NJS_MODEL collision_270_model = {
#include "stg24/terrain/collision_270_model.inc"
};
static NJS_OBJECT collision_270 = {
#include "stg24/terrain/collision_270.inc"
};
static NJS_MATERIAL collision_278_materials[1] = {
#include "stg24/terrain/collision_278_materials.inc"
};
static Sint16 collision_278_faces[66] = {
#include "stg24/terrain/collision_278_faces.inc"
};
static NJS_MESHSET collision_278_meshsets[1] = {
#include "stg24/terrain/collision_278_meshsets.inc"
};
static NJS_POINT3 collision_278_points[28] = {
#include "stg24/terrain/collision_278_points.inc"
};
static NJS_MODEL collision_278_model = {
#include "stg24/terrain/collision_278_model.inc"
};
static NJS_OBJECT collision_278 = {
#include "stg24/terrain/collision_278.inc"
};
static NJS_MATERIAL collision_279_materials[1] = {
#include "stg24/terrain/collision_279_materials.inc"
};
static Sint16 collision_279_faces[19] = {
#include "stg24/terrain/collision_279_faces.inc"
};
static NJS_MESHSET collision_279_meshsets[1] = {
#include "stg24/terrain/collision_279_meshsets.inc"
};
static NJS_POINT3 collision_279_points[18] = {
#include "stg24/terrain/collision_279_points.inc"
};
static NJS_MODEL collision_279_model = {
#include "stg24/terrain/collision_279_model.inc"
};
static NJS_OBJECT collision_279 = {
#include "stg24/terrain/collision_279.inc"
};
static NJS_MATERIAL collision_280_materials[1] = {
#include "stg24/terrain/collision_280_materials.inc"
};
static Sint16 collision_280_faces[14] = {
#include "stg24/terrain/collision_280_faces.inc"
};
static NJS_MESHSET collision_280_meshsets[1] = {
#include "stg24/terrain/collision_280_meshsets.inc"
};
static NJS_POINT3 collision_280_points[12] = {
#include "stg24/terrain/collision_280_points.inc"
};
static NJS_MODEL collision_280_model = {
#include "stg24/terrain/collision_280_model.inc"
};
static NJS_OBJECT collision_280 = {
#include "stg24/terrain/collision_280.inc"
};
static NJS_MATERIAL collision_291_materials[1] = {
#include "stg24/terrain/collision_291_materials.inc"
};
static Sint16 collision_291_faces[166] = {
#include "stg24/terrain/collision_291_faces.inc"
};
static NJS_MESHSET collision_291_meshsets[1] = {
#include "stg24/terrain/collision_291_meshsets.inc"
};
static NJS_POINT3 collision_291_points[72] = {
#include "stg24/terrain/collision_291_points.inc"
};
static NJS_MODEL collision_291_model = {
#include "stg24/terrain/collision_291_model.inc"
};
static NJS_OBJECT collision_291 = {
#include "stg24/terrain/collision_291.inc"
};
static NJS_MATERIAL collision_292_materials[1] = {
#include "stg24/terrain/collision_292_materials.inc"
};
static Sint16 collision_292_faces[31] = {
#include "stg24/terrain/collision_292_faces.inc"
};
static NJS_MESHSET collision_292_meshsets[1] = {
#include "stg24/terrain/collision_292_meshsets.inc"
};
static NJS_POINT3 collision_292_points[28] = {
#include "stg24/terrain/collision_292_points.inc"
};
static NJS_MODEL collision_292_model = {
#include "stg24/terrain/collision_292_model.inc"
};
static NJS_OBJECT collision_292 = {
#include "stg24/terrain/collision_292.inc"
};
static NJS_MATERIAL collision_293_materials[1] = {
#include "stg24/terrain/collision_293_materials.inc"
};
static Sint16 collision_293_faces[28] = {
#include "stg24/terrain/collision_293_faces.inc"
};
static NJS_MESHSET collision_293_meshsets[1] = {
#include "stg24/terrain/collision_293_meshsets.inc"
};
static NJS_POINT3 collision_293_points[12] = {
#include "stg24/terrain/collision_293_points.inc"
};
static NJS_MODEL collision_293_model = {
#include "stg24/terrain/collision_293_model.inc"
};
static NJS_OBJECT collision_293 = {
#include "stg24/terrain/collision_293.inc"
};
static NJS_MATERIAL collision_304_materials[1] = {
#include "stg24/terrain/collision_304_materials.inc"
};
static Sint16 collision_304_faces[41] = {
#include "stg24/terrain/collision_304_faces.inc"
};
static NJS_MESHSET collision_304_meshsets[1] = {
#include "stg24/terrain/collision_304_meshsets.inc"
};
static NJS_POINT3 collision_304_points[18] = {
#include "stg24/terrain/collision_304_points.inc"
};
static NJS_MODEL collision_304_model = {
#include "stg24/terrain/collision_304_model.inc"
};
static NJS_OBJECT collision_304 = {
#include "stg24/terrain/collision_304.inc"
};
static Sint16 draw_45_polygons[131] = {
#include "stg24/terrain/draw_45_polygons.inc"
};
static Sint32 draw_45_vertices[119] = {
#include "stg24/terrain/draw_45_vertices.inc"
};
static NJS_CNK_MODEL draw_45_model = {
#include "stg24/terrain/draw_45_model.inc"
};
static NJS_CNK_OBJECT draw_390 = {
#include "stg24/terrain/draw_390.inc"
};
static Sint16 draw_65_polygons[127] = {
#include "stg24/terrain/draw_65_polygons.inc"
};
static Sint32 draw_65_vertices[67] = {
#include "stg24/terrain/draw_65_vertices.inc"
};
static NJS_CNK_MODEL draw_65_model = {
#include "stg24/terrain/draw_65_model.inc"
};
static NJS_CNK_OBJECT draw_388 = {
#include "stg24/terrain/draw_388.inc"
};
static Sint16 draw_15_polygons[197] = {
#include "stg24/terrain/draw_15_polygons.inc"
};
static Sint32 draw_15_vertices[163] = {
#include "stg24/terrain/draw_15_vertices.inc"
};
static NJS_CNK_MODEL draw_15_model = {
#include "stg24/terrain/draw_15_model.inc"
};
static NJS_CNK_OBJECT draw_364 = {
#include "stg24/terrain/draw_364.inc"
};
static Sint16 draw_14_polygons[1183] = {
#include "stg24/terrain/draw_14_polygons.inc"
};
static Sint32 draw_14_vertices[1271] = {
#include "stg24/terrain/draw_14_vertices.inc"
};
static NJS_CNK_MODEL draw_14_model = {
#include "stg24/terrain/draw_14_model.inc"
};
static NJS_CNK_OBJECT draw_363 = {
#include "stg24/terrain/draw_363.inc"
};
static Sint16 draw_30_polygons[315] = {
#include "stg24/terrain/draw_30_polygons.inc"
};
static Sint32 draw_30_vertices[351] = {
#include "stg24/terrain/draw_30_vertices.inc"
};
static NJS_CNK_MODEL draw_30_model = {
#include "stg24/terrain/draw_30_model.inc"
};
static NJS_CNK_OBJECT draw_362 = {
#include "stg24/terrain/draw_362.inc"
};
static Sint16 draw_36_polygons[235] = {
#include "stg24/terrain/draw_36_polygons.inc"
};
static Sint32 draw_36_vertices[131] = {
#include "stg24/terrain/draw_36_vertices.inc"
};
static NJS_CNK_MODEL draw_36_model = {
#include "stg24/terrain/draw_36_model.inc"
};
static NJS_CNK_OBJECT draw_361 = {
#include "stg24/terrain/draw_361.inc"
};
static Sint16 draw_33_polygons[1271] = {
#include "stg24/terrain/draw_33_polygons.inc"
};
static Sint32 draw_33_vertices[1059] = {
#include "stg24/terrain/draw_33_vertices.inc"
};
static NJS_CNK_MODEL draw_33_model = {
#include "stg24/terrain/draw_33_model.inc"
};
static NJS_CNK_OBJECT draw_355 = {
#include "stg24/terrain/draw_355.inc"
};
static Sint16 draw_28_polygons[83] = {
#include "stg24/terrain/draw_28_polygons.inc"
};
static Sint32 draw_28_vertices[83] = {
#include "stg24/terrain/draw_28_vertices.inc"
};
static NJS_CNK_MODEL draw_28_model = {
#include "stg24/terrain/draw_28_model.inc"
};
static NJS_CNK_OBJECT draw_354 = {
#include "stg24/terrain/draw_354.inc"
};
static Sint16 draw_13_polygons[455] = {
#include "stg24/terrain/draw_13_polygons.inc"
};
static Sint32 draw_13_vertices[483] = {
#include "stg24/terrain/draw_13_vertices.inc"
};
static NJS_CNK_MODEL draw_13_model = {
#include "stg24/terrain/draw_13_model.inc"
};
static NJS_CNK_OBJECT draw_353 = {
#include "stg24/terrain/draw_353.inc"
};
static Sint16 draw_69_polygons[141] = {
#include "stg24/terrain/draw_69_polygons.inc"
};
static Sint32 draw_69_vertices[83] = {
#include "stg24/terrain/draw_69_vertices.inc"
};
static NJS_CNK_MODEL draw_69_model = {
#include "stg24/terrain/draw_69_model.inc"
};
static NJS_CNK_OBJECT draw_316 = {
#include "stg24/terrain/draw_316.inc"
};
static Sint16 draw_315_polygons[63] = {
#include "stg24/terrain/draw_315_polygons.inc"
};
static Sint32 draw_315_vertices[67] = {
#include "stg24/terrain/draw_315_vertices.inc"
};
static NJS_CNK_MODEL draw_315_model = {
#include "stg24/terrain/draw_315_model.inc"
};
static NJS_CNK_OBJECT draw_315 = {
#include "stg24/terrain/draw_315.inc"
};
static Sint16 draw_44_polygons[421] = {
#include "stg24/terrain/draw_44_polygons.inc"
};
static Sint32 draw_44_vertices[415] = {
#include "stg24/terrain/draw_44_vertices.inc"
};
static NJS_CNK_MODEL draw_44_model = {
#include "stg24/terrain/draw_44_model.inc"
};
static NJS_CNK_OBJECT draw_310 = {
#include "stg24/terrain/draw_310.inc"
};
static Sint16 draw_57_polygons[3869] = {
#include "stg24/terrain/draw_57_polygons.inc"
};
static Sint32 draw_57_vertices[2815] = {
#include "stg24/terrain/draw_57_vertices.inc"
};
static NJS_CNK_MODEL draw_57_model = {
#include "stg24/terrain/draw_57_model.inc"
};
static NJS_CNK_OBJECT draw_309 = {
#include "stg24/terrain/draw_309.inc"
};
static Sint16 draw_148_polygons[207] = {
#include "stg24/terrain/draw_148_polygons.inc"
};
static Sint32 draw_148_vertices[151] = {
#include "stg24/terrain/draw_148_vertices.inc"
};
static NJS_CNK_MODEL draw_148_model = {
#include "stg24/terrain/draw_148_model.inc"
};
static NJS_CNK_OBJECT draw_308 = {
#include "stg24/terrain/draw_308.inc"
};
static Sint16 draw_206_polygons[83] = {
#include "stg24/terrain/draw_206_polygons.inc"
};
static Sint32 draw_206_vertices[51] = {
#include "stg24/terrain/draw_206_vertices.inc"
};
static NJS_CNK_MODEL draw_206_model = {
#include "stg24/terrain/draw_206_model.inc"
};
static NJS_CNK_OBJECT draw_307 = {
#include "stg24/terrain/draw_307.inc"
};
static Sint16 draw_204_polygons[681] = {
#include "stg24/terrain/draw_204_polygons.inc"
};
static Sint32 draw_204_vertices[699] = {
#include "stg24/terrain/draw_204_vertices.inc"
};
static NJS_CNK_MODEL draw_204_model = {
#include "stg24/terrain/draw_204_model.inc"
};
static NJS_CNK_OBJECT draw_304 = {
#include "stg24/terrain/draw_304.inc"
};
static Sint16 draw_202_polygons[1179] = {
#include "stg24/terrain/draw_202_polygons.inc"
};
static Sint32 draw_202_vertices[835] = {
#include "stg24/terrain/draw_202_vertices.inc"
};
static NJS_CNK_MODEL draw_202_model = {
#include "stg24/terrain/draw_202_model.inc"
};
static NJS_CNK_OBJECT draw_303 = {
#include "stg24/terrain/draw_303.inc"
};
static Sint16 draw_94_polygons[83] = {
#include "stg24/terrain/draw_94_polygons.inc"
};
static Sint32 draw_94_vertices[51] = {
#include "stg24/terrain/draw_94_vertices.inc"
};
static NJS_CNK_MODEL draw_94_model = {
#include "stg24/terrain/draw_94_model.inc"
};
static NJS_CNK_OBJECT draw_296 = {
#include "stg24/terrain/draw_296.inc"
};
static Sint16 draw_159_polygons[65] = {
#include "stg24/terrain/draw_159_polygons.inc"
};
static Sint32 draw_159_vertices[43] = {
#include "stg24/terrain/draw_159_vertices.inc"
};
static NJS_CNK_MODEL draw_159_model = {
#include "stg24/terrain/draw_159_model.inc"
};
static NJS_CNK_OBJECT draw_290 = {
#include "stg24/terrain/draw_290.inc"
};
static Sint16 draw_82_polygons[425] = {
#include "stg24/terrain/draw_82_polygons.inc"
};
static Sint32 draw_82_vertices[419] = {
#include "stg24/terrain/draw_82_vertices.inc"
};
static NJS_CNK_MODEL draw_82_model = {
#include "stg24/terrain/draw_82_model.inc"
};
static NJS_CNK_OBJECT draw_289 = {
#include "stg24/terrain/draw_289.inc"
};
static Sint16 draw_154_polygons[1021] = {
#include "stg24/terrain/draw_154_polygons.inc"
};
static Sint32 draw_154_vertices[755] = {
#include "stg24/terrain/draw_154_vertices.inc"
};
static NJS_CNK_MODEL draw_154_model = {
#include "stg24/terrain/draw_154_model.inc"
};
static NJS_CNK_OBJECT draw_288 = {
#include "stg24/terrain/draw_288.inc"
};
static Sint16 draw_91_polygons[915] = {
#include "stg24/terrain/draw_91_polygons.inc"
};
static Sint32 draw_91_vertices[527] = {
#include "stg24/terrain/draw_91_vertices.inc"
};
static NJS_CNK_MODEL draw_91_model = {
#include "stg24/terrain/draw_91_model.inc"
};
static NJS_CNK_OBJECT draw_287 = {
#include "stg24/terrain/draw_287.inc"
};
static Sint16 draw_151_polygons[845] = {
#include "stg24/terrain/draw_151_polygons.inc"
};
static Sint32 draw_151_vertices[907] = {
#include "stg24/terrain/draw_151_vertices.inc"
};
static NJS_CNK_MODEL draw_151_model = {
#include "stg24/terrain/draw_151_model.inc"
};
static NJS_CNK_OBJECT draw_286 = {
#include "stg24/terrain/draw_286.inc"
};
static Sint16 draw_75_polygons[319] = {
#include "stg24/terrain/draw_75_polygons.inc"
};
static Sint32 draw_75_vertices[327] = {
#include "stg24/terrain/draw_75_vertices.inc"
};
static NJS_CNK_MODEL draw_75_model = {
#include "stg24/terrain/draw_75_model.inc"
};
static NJS_CNK_OBJECT draw_279 = {
#include "stg24/terrain/draw_279.inc"
};
static Sint16 draw_73_polygons[311] = {
#include "stg24/terrain/draw_73_polygons.inc"
};
static Sint32 draw_73_vertices[283] = {
#include "stg24/terrain/draw_73_vertices.inc"
};
static NJS_CNK_MODEL draw_73_model = {
#include "stg24/terrain/draw_73_model.inc"
};
static NJS_CNK_OBJECT draw_278 = {
#include "stg24/terrain/draw_278.inc"
};
static Sint16 draw_116_polygons[235] = {
#include "stg24/terrain/draw_116_polygons.inc"
};
static Sint32 draw_116_vertices[211] = {
#include "stg24/terrain/draw_116_vertices.inc"
};
static NJS_CNK_MODEL draw_116_model = {
#include "stg24/terrain/draw_116_model.inc"
};
static NJS_CNK_OBJECT draw_276 = {
#include "stg24/terrain/draw_276.inc"
};
static Sint16 draw_17_polygons[939] = {
#include "stg24/terrain/draw_17_polygons.inc"
};
static Sint32 draw_17_vertices[887] = {
#include "stg24/terrain/draw_17_vertices.inc"
};
static NJS_CNK_MODEL draw_17_model = {
#include "stg24/terrain/draw_17_model.inc"
};
static NJS_CNK_OBJECT draw_274 = {
#include "stg24/terrain/draw_274.inc"
};
static Sint16 draw_9_polygons[711] = {
#include "stg24/terrain/draw_9_polygons.inc"
};
static Sint32 draw_9_vertices[619] = {
#include "stg24/terrain/draw_9_vertices.inc"
};
static NJS_CNK_MODEL draw_9_model = {
#include "stg24/terrain/draw_9_model.inc"
};
static NJS_CNK_OBJECT draw_273 = {
#include "stg24/terrain/draw_273.inc"
};
static Sint16 draw_16_polygons[853] = {
#include "stg24/terrain/draw_16_polygons.inc"
};
static Sint32 draw_16_vertices[735] = {
#include "stg24/terrain/draw_16_vertices.inc"
};
static NJS_CNK_MODEL draw_16_model = {
#include "stg24/terrain/draw_16_model.inc"
};
static NJS_CNK_OBJECT draw_267 = {
#include "stg24/terrain/draw_267.inc"
};
static Sint16 draw_257_polygons[141] = {
#include "stg24/terrain/draw_257_polygons.inc"
};
static Sint32 draw_257_vertices[83] = {
#include "stg24/terrain/draw_257_vertices.inc"
};
static NJS_CNK_MODEL draw_257_model = {
#include "stg24/terrain/draw_257_model.inc"
};
static NJS_CNK_OBJECT draw_257 = {
#include "stg24/terrain/draw_257.inc"
};
static Sint16 draw_256_polygons[141] = {
#include "stg24/terrain/draw_256_polygons.inc"
};
static Sint32 draw_256_vertices[83] = {
#include "stg24/terrain/draw_256_vertices.inc"
};
static NJS_CNK_MODEL draw_256_model = {
#include "stg24/terrain/draw_256_model.inc"
};
static NJS_CNK_OBJECT draw_256 = {
#include "stg24/terrain/draw_256.inc"
};
static Sint16 draw_243_polygons[59] = {
#include "stg24/terrain/draw_243_polygons.inc"
};
static Sint32 draw_243_vertices[35] = {
#include "stg24/terrain/draw_243_vertices.inc"
};
static NJS_CNK_MODEL draw_243_model = {
#include "stg24/terrain/draw_243_model.inc"
};
static NJS_CNK_OBJECT draw_243 = {
#include "stg24/terrain/draw_243.inc"
};
static Sint16 draw_83_polygons[393] = {
#include "stg24/terrain/draw_83_polygons.inc"
};
static Sint32 draw_83_vertices[239] = {
#include "stg24/terrain/draw_83_vertices.inc"
};
static NJS_CNK_MODEL draw_83_model = {
#include "stg24/terrain/draw_83_model.inc"
};
static NJS_CNK_OBJECT draw_242 = {
#include "stg24/terrain/draw_242.inc"
};
static Sint16 draw_71_polygons[515] = {
#include "stg24/terrain/draw_71_polygons.inc"
};
static Sint32 draw_71_vertices[291] = {
#include "stg24/terrain/draw_71_vertices.inc"
};
static NJS_CNK_MODEL draw_71_model = {
#include "stg24/terrain/draw_71_model.inc"
};
static NJS_CNK_OBJECT draw_238 = {
#include "stg24/terrain/draw_238.inc"
};
static Sint16 draw_163_polygons[145] = {
#include "stg24/terrain/draw_163_polygons.inc"
};
static Sint32 draw_163_vertices[163] = {
#include "stg24/terrain/draw_163_vertices.inc"
};
static NJS_CNK_MODEL draw_163_model = {
#include "stg24/terrain/draw_163_model.inc"
};
static NJS_CNK_OBJECT draw_195 = {
#include "stg24/terrain/draw_195.inc"
};
static Sint16 draw_168_polygons[315] = {
#include "stg24/terrain/draw_168_polygons.inc"
};
static Sint32 draw_168_vertices[347] = {
#include "stg24/terrain/draw_168_vertices.inc"
};
static NJS_CNK_MODEL draw_168_model = {
#include "stg24/terrain/draw_168_model.inc"
};
static NJS_CNK_OBJECT draw_194 = {
#include "stg24/terrain/draw_194.inc"
};
static Sint16 draw_175_polygons[135] = {
#include "stg24/terrain/draw_175_polygons.inc"
};
static Sint32 draw_175_vertices[123] = {
#include "stg24/terrain/draw_175_vertices.inc"
};
static NJS_CNK_MODEL draw_175_model = {
#include "stg24/terrain/draw_175_model.inc"
};
static NJS_CNK_OBJECT draw_193 = {
#include "stg24/terrain/draw_193.inc"
};
static Sint16 draw_162_polygons[217] = {
#include "stg24/terrain/draw_162_polygons.inc"
};
static Sint32 draw_162_vertices[243] = {
#include "stg24/terrain/draw_162_vertices.inc"
};
static NJS_CNK_MODEL draw_162_model = {
#include "stg24/terrain/draw_162_model.inc"
};
static NJS_CNK_OBJECT draw_192 = {
#include "stg24/terrain/draw_192.inc"
};
static Sint16 draw_173_polygons[873] = {
#include "stg24/terrain/draw_173_polygons.inc"
};
static Sint32 draw_173_vertices[555] = {
#include "stg24/terrain/draw_173_vertices.inc"
};
static NJS_CNK_MODEL draw_173_model = {
#include "stg24/terrain/draw_173_model.inc"
};
static NJS_CNK_OBJECT draw_191 = {
#include "stg24/terrain/draw_191.inc"
};
static Sint16 draw_190_polygons[707] = {
#include "stg24/terrain/draw_190_polygons.inc"
};
static Sint32 draw_190_vertices[523] = {
#include "stg24/terrain/draw_190_vertices.inc"
};
static NJS_CNK_MODEL draw_190_model = {
#include "stg24/terrain/draw_190_model.inc"
};
static NJS_CNK_OBJECT draw_190 = {
#include "stg24/terrain/draw_190.inc"
};
static Sint16 draw_189_polygons[165] = {
#include "stg24/terrain/draw_189_polygons.inc"
};
static Sint32 draw_189_vertices[151] = {
#include "stg24/terrain/draw_189_vertices.inc"
};
static NJS_CNK_MODEL draw_189_model = {
#include "stg24/terrain/draw_189_model.inc"
};
static NJS_CNK_OBJECT draw_189 = {
#include "stg24/terrain/draw_189.inc"
};
static Sint16 draw_185_polygons[53] = {
#include "stg24/terrain/draw_185_polygons.inc"
};
static Sint32 draw_185_vertices[51] = {
#include "stg24/terrain/draw_185_vertices.inc"
};
static NJS_CNK_MODEL draw_185_model = {
#include "stg24/terrain/draw_185_model.inc"
};
static NJS_CNK_OBJECT draw_187 = {
#include "stg24/terrain/draw_187.inc"
};
static Sint16 draw_184_polygons[165] = {
#include "stg24/terrain/draw_184_polygons.inc"
};
static Sint32 draw_184_vertices[159] = {
#include "stg24/terrain/draw_184_vertices.inc"
};
static NJS_CNK_MODEL draw_184_model = {
#include "stg24/terrain/draw_184_model.inc"
};
static NJS_CNK_OBJECT draw_184 = {
#include "stg24/terrain/draw_184.inc"
};
static Sint16 draw_174_polygons[497] = {
#include "stg24/terrain/draw_174_polygons.inc"
};
static Sint32 draw_174_vertices[579] = {
#include "stg24/terrain/draw_174_vertices.inc"
};
static NJS_CNK_MODEL draw_174_model = {
#include "stg24/terrain/draw_174_model.inc"
};
static NJS_CNK_OBJECT draw_174 = {
#include "stg24/terrain/draw_174.inc"
};
static Sint16 draw_160_polygons[179] = {
#include "stg24/terrain/draw_160_polygons.inc"
};
static Sint32 draw_160_vertices[187] = {
#include "stg24/terrain/draw_160_vertices.inc"
};
static NJS_CNK_MODEL draw_160_model = {
#include "stg24/terrain/draw_160_model.inc"
};
static NJS_CNK_OBJECT draw_161 = {
#include "stg24/terrain/draw_161.inc"
};
static Sint16 draw_121_polygons[611] = {
#include "stg24/terrain/draw_121_polygons.inc"
};
static Sint32 draw_121_vertices[467] = {
#include "stg24/terrain/draw_121_vertices.inc"
};
static NJS_CNK_MODEL draw_121_model = {
#include "stg24/terrain/draw_121_model.inc"
};
static NJS_CNK_OBJECT draw_141 = {
#include "stg24/terrain/draw_141.inc"
};
static Sint16 draw_139_polygons[69] = {
#include "stg24/terrain/draw_139_polygons.inc"
};
static Sint32 draw_139_vertices[39] = {
#include "stg24/terrain/draw_139_vertices.inc"
};
static NJS_CNK_MODEL draw_139_model = {
#include "stg24/terrain/draw_139_model.inc"
};
static NJS_CNK_OBJECT draw_140 = {
#include "stg24/terrain/draw_140.inc"
};
static Sint16 draw_99_polygons[63] = {
#include "stg24/terrain/draw_99_polygons.inc"
};
static Sint32 draw_99_vertices[67] = {
#include "stg24/terrain/draw_99_vertices.inc"
};
static NJS_CNK_MODEL draw_99_model = {
#include "stg24/terrain/draw_99_model.inc"
};
static NJS_CNK_OBJECT draw_99 = {
#include "stg24/terrain/draw_99.inc"
};
static Sint16 draw_77_polygons[193] = {
#include "stg24/terrain/draw_77_polygons.inc"
};
static Sint32 draw_77_vertices[159] = {
#include "stg24/terrain/draw_77_vertices.inc"
};
static NJS_CNK_MODEL draw_77_model = {
#include "stg24/terrain/draw_77_model.inc"
};
static NJS_CNK_OBJECT draw_78 = {
#include "stg24/terrain/draw_78.inc"
};
static Sint16 draw_70_polygons[415] = {
#include "stg24/terrain/draw_70_polygons.inc"
};
static Sint32 draw_70_vertices[251] = {
#include "stg24/terrain/draw_70_vertices.inc"
};
static NJS_CNK_MODEL draw_70_model = {
#include "stg24/terrain/draw_70_model.inc"
};
static NJS_CNK_OBJECT draw_70 = {
#include "stg24/terrain/draw_70.inc"
};
static Sint16 draw_68_polygons[291] = {
#include "stg24/terrain/draw_68_polygons.inc"
};
static Sint32 draw_68_vertices[163] = {
#include "stg24/terrain/draw_68_vertices.inc"
};
static NJS_CNK_MODEL draw_68_model = {
#include "stg24/terrain/draw_68_model.inc"
};
static NJS_CNK_OBJECT draw_68 = {
#include "stg24/terrain/draw_68.inc"
};
static Sint16 draw_2_polygons[677] = {
#include "stg24/terrain/draw_2_polygons.inc"
};
static Sint32 draw_2_vertices[483] = {
#include "stg24/terrain/draw_2_vertices.inc"
};
static NJS_CNK_MODEL draw_2_model = {
#include "stg24/terrain/draw_2_model.inc"
};
static NJS_CNK_OBJECT draw_12 = {
#include "stg24/terrain/draw_12.inc"
};
static Sint16 draw_11_polygons[803] = {
#include "stg24/terrain/draw_11_polygons.inc"
};
static Sint32 draw_11_vertices[523] = {
#include "stg24/terrain/draw_11_vertices.inc"
};
static NJS_CNK_MODEL draw_11_model = {
#include "stg24/terrain/draw_11_model.inc"
};
static NJS_CNK_OBJECT draw_11 = {
#include "stg24/terrain/draw_11.inc"
};
static Sint16 draw_10_polygons[941] = {
#include "stg24/terrain/draw_10_polygons.inc"
};
static Sint32 draw_10_vertices[551] = {
#include "stg24/terrain/draw_10_vertices.inc"
};
static NJS_CNK_MODEL draw_10_model = {
#include "stg24/terrain/draw_10_model.inc"
};
static NJS_CNK_OBJECT draw_10 = {
#include "stg24/terrain/draw_10.inc"
};
static Sint16 draw_1_polygons[23] = {
#include "stg24/terrain/draw_1_polygons.inc"
};
static Sint32 draw_1_vertices[19] = {
#include "stg24/terrain/draw_1_vertices.inc"
};
static NJS_CNK_MODEL draw_1_model = {
#include "stg24/terrain/draw_1_model.inc"
};
static NJS_CNK_OBJECT draw_1 = {
#include "stg24/terrain/draw_1.inc"
};
static Sint16 draw_0_polygons[139] = {
#include "stg24/terrain/draw_0_polygons.inc"
};
static Sint32 draw_0_vertices[139] = {
#include "stg24/terrain/draw_0_vertices.inc"
};
static NJS_CNK_MODEL draw_0_model = {
#include "stg24/terrain/draw_0_model.inc"
};
static NJS_CNK_OBJECT draw_0 = {
#include "stg24/terrain/draw_0.inc"
};
static NJS_OBJECT collision_4 = {
#include "stg24/terrain/collision_4.inc"
};
static NJS_OBJECT collision_7 = {
#include "stg24/terrain/collision_7.inc"
};
static NJS_OBJECT collision_11 = {
#include "stg24/terrain/collision_11.inc"
};
static NJS_OBJECT collision_19 = {
#include "stg24/terrain/collision_19.inc"
};
static NJS_OBJECT collision_20 = {
#include "stg24/terrain/collision_20.inc"
};
static NJS_OBJECT collision_21 = {
#include "stg24/terrain/collision_21.inc"
};
static NJS_OBJECT collision_23 = {
#include "stg24/terrain/collision_23.inc"
};
static NJS_OBJECT collision_24 = {
#include "stg24/terrain/collision_24.inc"
};
static NJS_OBJECT collision_25 = {
#include "stg24/terrain/collision_25.inc"
};
static NJS_OBJECT collision_26 = {
#include "stg24/terrain/collision_26.inc"
};
static NJS_OBJECT collision_29 = {
#include "stg24/terrain/collision_29.inc"
};
static NJS_OBJECT collision_32 = {
#include "stg24/terrain/collision_32.inc"
};
static NJS_OBJECT collision_35 = {
#include "stg24/terrain/collision_35.inc"
};
static NJS_OBJECT collision_36 = {
#include "stg24/terrain/collision_36.inc"
};
static NJS_OBJECT collision_37 = {
#include "stg24/terrain/collision_37.inc"
};
static NJS_OBJECT collision_40 = {
#include "stg24/terrain/collision_40.inc"
};
static NJS_OBJECT collision_41 = {
#include "stg24/terrain/collision_41.inc"
};
static NJS_OBJECT collision_49 = {
#include "stg24/terrain/collision_49.inc"
};
static NJS_OBJECT collision_51 = {
#include "stg24/terrain/collision_51.inc"
};
static NJS_OBJECT collision_52 = {
#include "stg24/terrain/collision_52.inc"
};
static NJS_OBJECT collision_53 = {
#include "stg24/terrain/collision_53.inc"
};
static NJS_OBJECT collision_54 = {
#include "stg24/terrain/collision_54.inc"
};
static NJS_OBJECT collision_55 = {
#include "stg24/terrain/collision_55.inc"
};
static NJS_OBJECT collision_56 = {
#include "stg24/terrain/collision_56.inc"
};
static NJS_OBJECT collision_57 = {
#include "stg24/terrain/collision_57.inc"
};
static NJS_OBJECT collision_58 = {
#include "stg24/terrain/collision_58.inc"
};
static NJS_OBJECT collision_59 = {
#include "stg24/terrain/collision_59.inc"
};
static NJS_OBJECT collision_60 = {
#include "stg24/terrain/collision_60.inc"
};
static NJS_OBJECT collision_61 = {
#include "stg24/terrain/collision_61.inc"
};
static NJS_OBJECT collision_62 = {
#include "stg24/terrain/collision_62.inc"
};
static NJS_OBJECT collision_63 = {
#include "stg24/terrain/collision_63.inc"
};
static NJS_OBJECT collision_67 = {
#include "stg24/terrain/collision_67.inc"
};
static NJS_OBJECT collision_68 = {
#include "stg24/terrain/collision_68.inc"
};
static NJS_OBJECT collision_69 = {
#include "stg24/terrain/collision_69.inc"
};
static NJS_OBJECT collision_70 = {
#include "stg24/terrain/collision_70.inc"
};
static NJS_OBJECT collision_71 = {
#include "stg24/terrain/collision_71.inc"
};
static NJS_OBJECT collision_72 = {
#include "stg24/terrain/collision_72.inc"
};
static NJS_OBJECT collision_73 = {
#include "stg24/terrain/collision_73.inc"
};
static NJS_OBJECT collision_74 = {
#include "stg24/terrain/collision_74.inc"
};
static NJS_OBJECT collision_75 = {
#include "stg24/terrain/collision_75.inc"
};
static NJS_OBJECT collision_76 = {
#include "stg24/terrain/collision_76.inc"
};
static NJS_OBJECT collision_77 = {
#include "stg24/terrain/collision_77.inc"
};
static NJS_OBJECT collision_79 = {
#include "stg24/terrain/collision_79.inc"
};
static NJS_OBJECT collision_80 = {
#include "stg24/terrain/collision_80.inc"
};
static NJS_OBJECT collision_81 = {
#include "stg24/terrain/collision_81.inc"
};
static NJS_OBJECT collision_82 = {
#include "stg24/terrain/collision_82.inc"
};
static NJS_OBJECT collision_83 = {
#include "stg24/terrain/collision_83.inc"
};
static NJS_OBJECT collision_84 = {
#include "stg24/terrain/collision_84.inc"
};
static NJS_OBJECT collision_85 = {
#include "stg24/terrain/collision_85.inc"
};
static NJS_OBJECT collision_86 = {
#include "stg24/terrain/collision_86.inc"
};
static NJS_OBJECT collision_87 = {
#include "stg24/terrain/collision_87.inc"
};
static NJS_OBJECT collision_88 = {
#include "stg24/terrain/collision_88.inc"
};
static NJS_OBJECT collision_89 = {
#include "stg24/terrain/collision_89.inc"
};
static NJS_OBJECT collision_90 = {
#include "stg24/terrain/collision_90.inc"
};
static NJS_OBJECT collision_91 = {
#include "stg24/terrain/collision_91.inc"
};
static NJS_OBJECT collision_92 = {
#include "stg24/terrain/collision_92.inc"
};
static NJS_OBJECT collision_93 = {
#include "stg24/terrain/collision_93.inc"
};
static NJS_OBJECT collision_94 = {
#include "stg24/terrain/collision_94.inc"
};
static NJS_OBJECT collision_95 = {
#include "stg24/terrain/collision_95.inc"
};
static NJS_OBJECT collision_96 = {
#include "stg24/terrain/collision_96.inc"
};
static NJS_OBJECT collision_97 = {
#include "stg24/terrain/collision_97.inc"
};
static NJS_OBJECT collision_98 = {
#include "stg24/terrain/collision_98.inc"
};
static NJS_OBJECT collision_99 = {
#include "stg24/terrain/collision_99.inc"
};
static NJS_OBJECT collision_100 = {
#include "stg24/terrain/collision_100.inc"
};
static NJS_OBJECT collision_101 = {
#include "stg24/terrain/collision_101.inc"
};
static NJS_OBJECT collision_102 = {
#include "stg24/terrain/collision_102.inc"
};
static NJS_OBJECT collision_103 = {
#include "stg24/terrain/collision_103.inc"
};
static NJS_OBJECT collision_104 = {
#include "stg24/terrain/collision_104.inc"
};
static NJS_OBJECT collision_105 = {
#include "stg24/terrain/collision_105.inc"
};
static NJS_OBJECT collision_106 = {
#include "stg24/terrain/collision_106.inc"
};
static NJS_OBJECT collision_107 = {
#include "stg24/terrain/collision_107.inc"
};
static NJS_OBJECT collision_108 = {
#include "stg24/terrain/collision_108.inc"
};
static NJS_OBJECT collision_109 = {
#include "stg24/terrain/collision_109.inc"
};
static NJS_OBJECT collision_110 = {
#include "stg24/terrain/collision_110.inc"
};
static NJS_OBJECT collision_111 = {
#include "stg24/terrain/collision_111.inc"
};
static NJS_OBJECT collision_112 = {
#include "stg24/terrain/collision_112.inc"
};
static NJS_OBJECT collision_113 = {
#include "stg24/terrain/collision_113.inc"
};
static NJS_OBJECT collision_114 = {
#include "stg24/terrain/collision_114.inc"
};
static NJS_OBJECT collision_115 = {
#include "stg24/terrain/collision_115.inc"
};
static NJS_OBJECT collision_116 = {
#include "stg24/terrain/collision_116.inc"
};
static NJS_OBJECT collision_117 = {
#include "stg24/terrain/collision_117.inc"
};
static NJS_OBJECT collision_118 = {
#include "stg24/terrain/collision_118.inc"
};
static NJS_OBJECT collision_119 = {
#include "stg24/terrain/collision_119.inc"
};
static NJS_OBJECT collision_123 = {
#include "stg24/terrain/collision_123.inc"
};
static NJS_OBJECT collision_124 = {
#include "stg24/terrain/collision_124.inc"
};
static NJS_OBJECT collision_125 = {
#include "stg24/terrain/collision_125.inc"
};
static NJS_OBJECT collision_126 = {
#include "stg24/terrain/collision_126.inc"
};
static NJS_OBJECT collision_127 = {
#include "stg24/terrain/collision_127.inc"
};
static NJS_OBJECT collision_128 = {
#include "stg24/terrain/collision_128.inc"
};
static NJS_OBJECT collision_130 = {
#include "stg24/terrain/collision_130.inc"
};
static NJS_OBJECT collision_131 = {
#include "stg24/terrain/collision_131.inc"
};
static NJS_OBJECT collision_132 = {
#include "stg24/terrain/collision_132.inc"
};
static NJS_OBJECT collision_133 = {
#include "stg24/terrain/collision_133.inc"
};
static NJS_OBJECT collision_134 = {
#include "stg24/terrain/collision_134.inc"
};
static NJS_OBJECT collision_135 = {
#include "stg24/terrain/collision_135.inc"
};
static NJS_OBJECT collision_136 = {
#include "stg24/terrain/collision_136.inc"
};
static NJS_OBJECT collision_139 = {
#include "stg24/terrain/collision_139.inc"
};
static NJS_OBJECT collision_140 = {
#include "stg24/terrain/collision_140.inc"
};
static NJS_OBJECT collision_141 = {
#include "stg24/terrain/collision_141.inc"
};
static NJS_OBJECT collision_142 = {
#include "stg24/terrain/collision_142.inc"
};
static NJS_OBJECT collision_143 = {
#include "stg24/terrain/collision_143.inc"
};
static NJS_OBJECT collision_144 = {
#include "stg24/terrain/collision_144.inc"
};
static NJS_OBJECT collision_145 = {
#include "stg24/terrain/collision_145.inc"
};
static NJS_OBJECT collision_146 = {
#include "stg24/terrain/collision_146.inc"
};
static NJS_OBJECT collision_147 = {
#include "stg24/terrain/collision_147.inc"
};
static NJS_OBJECT collision_148 = {
#include "stg24/terrain/collision_148.inc"
};
static NJS_OBJECT collision_161 = {
#include "stg24/terrain/collision_161.inc"
};
static NJS_OBJECT collision_162 = {
#include "stg24/terrain/collision_162.inc"
};
static NJS_OBJECT collision_163 = {
#include "stg24/terrain/collision_163.inc"
};
static NJS_OBJECT collision_164 = {
#include "stg24/terrain/collision_164.inc"
};
static NJS_OBJECT collision_165 = {
#include "stg24/terrain/collision_165.inc"
};
static NJS_OBJECT collision_166 = {
#include "stg24/terrain/collision_166.inc"
};
static NJS_OBJECT collision_168 = {
#include "stg24/terrain/collision_168.inc"
};
static NJS_OBJECT collision_170 = {
#include "stg24/terrain/collision_170.inc"
};
static NJS_OBJECT collision_174 = {
#include "stg24/terrain/collision_174.inc"
};
static NJS_OBJECT collision_175 = {
#include "stg24/terrain/collision_175.inc"
};
static NJS_OBJECT collision_177 = {
#include "stg24/terrain/collision_177.inc"
};
static NJS_OBJECT collision_178 = {
#include "stg24/terrain/collision_178.inc"
};
static NJS_OBJECT collision_179 = {
#include "stg24/terrain/collision_179.inc"
};
static NJS_OBJECT collision_180 = {
#include "stg24/terrain/collision_180.inc"
};
static NJS_OBJECT collision_181 = {
#include "stg24/terrain/collision_181.inc"
};
static NJS_OBJECT collision_182 = {
#include "stg24/terrain/collision_182.inc"
};
static NJS_OBJECT collision_183 = {
#include "stg24/terrain/collision_183.inc"
};
static NJS_OBJECT collision_184 = {
#include "stg24/terrain/collision_184.inc"
};
static NJS_OBJECT collision_185 = {
#include "stg24/terrain/collision_185.inc"
};
static NJS_OBJECT collision_186 = {
#include "stg24/terrain/collision_186.inc"
};
static NJS_OBJECT collision_187 = {
#include "stg24/terrain/collision_187.inc"
};
static NJS_OBJECT collision_188 = {
#include "stg24/terrain/collision_188.inc"
};
static NJS_OBJECT collision_189 = {
#include "stg24/terrain/collision_189.inc"
};
static NJS_OBJECT collision_193 = {
#include "stg24/terrain/collision_193.inc"
};
static NJS_OBJECT collision_195 = {
#include "stg24/terrain/collision_195.inc"
};
static NJS_OBJECT collision_196 = {
#include "stg24/terrain/collision_196.inc"
};
static NJS_OBJECT collision_197 = {
#include "stg24/terrain/collision_197.inc"
};
static NJS_OBJECT collision_199 = {
#include "stg24/terrain/collision_199.inc"
};
static NJS_OBJECT collision_200 = {
#include "stg24/terrain/collision_200.inc"
};
static NJS_OBJECT collision_201 = {
#include "stg24/terrain/collision_201.inc"
};
static NJS_OBJECT collision_202 = {
#include "stg24/terrain/collision_202.inc"
};
static NJS_OBJECT collision_203 = {
#include "stg24/terrain/collision_203.inc"
};
static NJS_OBJECT collision_204 = {
#include "stg24/terrain/collision_204.inc"
};
static NJS_OBJECT collision_205 = {
#include "stg24/terrain/collision_205.inc"
};
static NJS_OBJECT collision_206 = {
#include "stg24/terrain/collision_206.inc"
};
static NJS_OBJECT collision_207 = {
#include "stg24/terrain/collision_207.inc"
};
static NJS_OBJECT collision_208 = {
#include "stg24/terrain/collision_208.inc"
};
static NJS_OBJECT collision_209 = {
#include "stg24/terrain/collision_209.inc"
};
static NJS_OBJECT collision_210 = {
#include "stg24/terrain/collision_210.inc"
};
static NJS_OBJECT collision_211 = {
#include "stg24/terrain/collision_211.inc"
};
static NJS_OBJECT collision_212 = {
#include "stg24/terrain/collision_212.inc"
};
static NJS_OBJECT collision_213 = {
#include "stg24/terrain/collision_213.inc"
};
static NJS_OBJECT collision_214 = {
#include "stg24/terrain/collision_214.inc"
};
static NJS_OBJECT collision_215 = {
#include "stg24/terrain/collision_215.inc"
};
static NJS_OBJECT collision_216 = {
#include "stg24/terrain/collision_216.inc"
};
static NJS_OBJECT collision_217 = {
#include "stg24/terrain/collision_217.inc"
};
static NJS_OBJECT collision_218 = {
#include "stg24/terrain/collision_218.inc"
};
static NJS_OBJECT collision_219 = {
#include "stg24/terrain/collision_219.inc"
};
static NJS_OBJECT collision_220 = {
#include "stg24/terrain/collision_220.inc"
};
static NJS_OBJECT collision_221 = {
#include "stg24/terrain/collision_221.inc"
};
static NJS_OBJECT collision_222 = {
#include "stg24/terrain/collision_222.inc"
};
static NJS_OBJECT collision_223 = {
#include "stg24/terrain/collision_223.inc"
};
static NJS_OBJECT collision_224 = {
#include "stg24/terrain/collision_224.inc"
};
static NJS_OBJECT collision_225 = {
#include "stg24/terrain/collision_225.inc"
};
static NJS_OBJECT collision_226 = {
#include "stg24/terrain/collision_226.inc"
};
static NJS_OBJECT collision_227 = {
#include "stg24/terrain/collision_227.inc"
};
static NJS_OBJECT collision_228 = {
#include "stg24/terrain/collision_228.inc"
};
static NJS_OBJECT collision_229 = {
#include "stg24/terrain/collision_229.inc"
};
static NJS_OBJECT collision_230 = {
#include "stg24/terrain/collision_230.inc"
};
static NJS_OBJECT collision_231 = {
#include "stg24/terrain/collision_231.inc"
};
static NJS_OBJECT collision_232 = {
#include "stg24/terrain/collision_232.inc"
};
static NJS_OBJECT collision_233 = {
#include "stg24/terrain/collision_233.inc"
};
static NJS_OBJECT collision_234 = {
#include "stg24/terrain/collision_234.inc"
};
static NJS_OBJECT collision_235 = {
#include "stg24/terrain/collision_235.inc"
};
static NJS_OBJECT collision_236 = {
#include "stg24/terrain/collision_236.inc"
};
static NJS_OBJECT collision_237 = {
#include "stg24/terrain/collision_237.inc"
};
static NJS_OBJECT collision_238 = {
#include "stg24/terrain/collision_238.inc"
};
static NJS_OBJECT collision_239 = {
#include "stg24/terrain/collision_239.inc"
};
static NJS_OBJECT collision_241 = {
#include "stg24/terrain/collision_241.inc"
};
static NJS_OBJECT collision_243 = {
#include "stg24/terrain/collision_243.inc"
};
static NJS_OBJECT collision_244 = {
#include "stg24/terrain/collision_244.inc"
};
static NJS_OBJECT collision_245 = {
#include "stg24/terrain/collision_245.inc"
};
static NJS_OBJECT collision_246 = {
#include "stg24/terrain/collision_246.inc"
};
static NJS_OBJECT collision_248 = {
#include "stg24/terrain/collision_248.inc"
};
static NJS_OBJECT collision_249 = {
#include "stg24/terrain/collision_249.inc"
};
static NJS_OBJECT collision_250 = {
#include "stg24/terrain/collision_250.inc"
};
static NJS_OBJECT collision_251 = {
#include "stg24/terrain/collision_251.inc"
};
static NJS_OBJECT collision_252 = {
#include "stg24/terrain/collision_252.inc"
};
static NJS_OBJECT collision_253 = {
#include "stg24/terrain/collision_253.inc"
};
static NJS_OBJECT collision_255 = {
#include "stg24/terrain/collision_255.inc"
};
static NJS_OBJECT collision_256 = {
#include "stg24/terrain/collision_256.inc"
};
static NJS_OBJECT collision_257 = {
#include "stg24/terrain/collision_257.inc"
};
static NJS_OBJECT collision_258 = {
#include "stg24/terrain/collision_258.inc"
};
static NJS_OBJECT collision_259 = {
#include "stg24/terrain/collision_259.inc"
};
static NJS_OBJECT collision_260 = {
#include "stg24/terrain/collision_260.inc"
};
static NJS_OBJECT collision_263 = {
#include "stg24/terrain/collision_263.inc"
};
static NJS_OBJECT collision_264 = {
#include "stg24/terrain/collision_264.inc"
};
static NJS_OBJECT collision_265 = {
#include "stg24/terrain/collision_265.inc"
};
static NJS_OBJECT collision_266 = {
#include "stg24/terrain/collision_266.inc"
};
static NJS_OBJECT collision_267 = {
#include "stg24/terrain/collision_267.inc"
};
static NJS_OBJECT collision_268 = {
#include "stg24/terrain/collision_268.inc"
};
static NJS_OBJECT collision_271 = {
#include "stg24/terrain/collision_271.inc"
};
static NJS_OBJECT collision_272 = {
#include "stg24/terrain/collision_272.inc"
};
static NJS_OBJECT collision_273 = {
#include "stg24/terrain/collision_273.inc"
};
static NJS_OBJECT collision_274 = {
#include "stg24/terrain/collision_274.inc"
};
static NJS_OBJECT collision_275 = {
#include "stg24/terrain/collision_275.inc"
};
static NJS_OBJECT collision_276 = {
#include "stg24/terrain/collision_276.inc"
};
static NJS_OBJECT collision_277 = {
#include "stg24/terrain/collision_277.inc"
};
static NJS_OBJECT collision_281 = {
#include "stg24/terrain/collision_281.inc"
};
static NJS_OBJECT collision_282 = {
#include "stg24/terrain/collision_282.inc"
};
static NJS_OBJECT collision_283 = {
#include "stg24/terrain/collision_283.inc"
};
static NJS_OBJECT collision_284 = {
#include "stg24/terrain/collision_284.inc"
};
static NJS_OBJECT collision_285 = {
#include "stg24/terrain/collision_285.inc"
};
static NJS_OBJECT collision_286 = {
#include "stg24/terrain/collision_286.inc"
};
static NJS_OBJECT collision_287 = {
#include "stg24/terrain/collision_287.inc"
};
static NJS_OBJECT collision_288 = {
#include "stg24/terrain/collision_288.inc"
};
static NJS_OBJECT collision_289 = {
#include "stg24/terrain/collision_289.inc"
};
static NJS_OBJECT collision_290 = {
#include "stg24/terrain/collision_290.inc"
};
static NJS_OBJECT collision_294 = {
#include "stg24/terrain/collision_294.inc"
};
static NJS_OBJECT collision_295 = {
#include "stg24/terrain/collision_295.inc"
};
static NJS_OBJECT collision_296 = {
#include "stg24/terrain/collision_296.inc"
};
static NJS_OBJECT collision_297 = {
#include "stg24/terrain/collision_297.inc"
};
static NJS_OBJECT collision_298 = {
#include "stg24/terrain/collision_298.inc"
};
static NJS_OBJECT collision_299 = {
#include "stg24/terrain/collision_299.inc"
};
static NJS_OBJECT collision_300 = {
#include "stg24/terrain/collision_300.inc"
};
static NJS_OBJECT collision_301 = {
#include "stg24/terrain/collision_301.inc"
};
static NJS_OBJECT collision_302 = {
#include "stg24/terrain/collision_302.inc"
};
static NJS_OBJECT collision_303 = {
#include "stg24/terrain/collision_303.inc"
};
static NJS_CNK_OBJECT draw_392 = {
#include "stg24/terrain/draw_392.inc"
};
static NJS_CNK_OBJECT draw_391 = {
#include "stg24/terrain/draw_391.inc"
};
static NJS_CNK_OBJECT draw_389 = {
#include "stg24/terrain/draw_389.inc"
};
static NJS_CNK_OBJECT draw_387 = {
#include "stg24/terrain/draw_387.inc"
};
static NJS_CNK_OBJECT draw_386 = {
#include "stg24/terrain/draw_386.inc"
};
static NJS_CNK_OBJECT draw_385 = {
#include "stg24/terrain/draw_385.inc"
};
static NJS_CNK_OBJECT draw_384 = {
#include "stg24/terrain/draw_384.inc"
};
static NJS_CNK_OBJECT draw_383 = {
#include "stg24/terrain/draw_383.inc"
};
static NJS_CNK_OBJECT draw_382 = {
#include "stg24/terrain/draw_382.inc"
};
static NJS_CNK_OBJECT draw_381 = {
#include "stg24/terrain/draw_381.inc"
};
static NJS_CNK_OBJECT draw_380 = {
#include "stg24/terrain/draw_380.inc"
};
static NJS_CNK_OBJECT draw_379 = {
#include "stg24/terrain/draw_379.inc"
};
static NJS_CNK_OBJECT draw_378 = {
#include "stg24/terrain/draw_378.inc"
};
static NJS_CNK_OBJECT draw_377 = {
#include "stg24/terrain/draw_377.inc"
};
static NJS_CNK_OBJECT draw_376 = {
#include "stg24/terrain/draw_376.inc"
};
static NJS_CNK_OBJECT draw_375 = {
#include "stg24/terrain/draw_375.inc"
};
static NJS_CNK_OBJECT draw_374 = {
#include "stg24/terrain/draw_374.inc"
};
static NJS_CNK_OBJECT draw_373 = {
#include "stg24/terrain/draw_373.inc"
};
static NJS_CNK_OBJECT draw_372 = {
#include "stg24/terrain/draw_372.inc"
};
static NJS_CNK_OBJECT draw_371 = {
#include "stg24/terrain/draw_371.inc"
};
static NJS_CNK_OBJECT draw_370 = {
#include "stg24/terrain/draw_370.inc"
};
static NJS_CNK_OBJECT draw_369 = {
#include "stg24/terrain/draw_369.inc"
};
static NJS_CNK_OBJECT draw_368 = {
#include "stg24/terrain/draw_368.inc"
};
static NJS_CNK_OBJECT draw_367 = {
#include "stg24/terrain/draw_367.inc"
};
static NJS_CNK_OBJECT draw_366 = {
#include "stg24/terrain/draw_366.inc"
};
static NJS_CNK_OBJECT draw_365 = {
#include "stg24/terrain/draw_365.inc"
};
static NJS_CNK_OBJECT draw_360 = {
#include "stg24/terrain/draw_360.inc"
};
static NJS_CNK_OBJECT draw_359 = {
#include "stg24/terrain/draw_359.inc"
};
static NJS_CNK_OBJECT draw_358 = {
#include "stg24/terrain/draw_358.inc"
};
static NJS_CNK_OBJECT draw_357 = {
#include "stg24/terrain/draw_357.inc"
};
static NJS_CNK_OBJECT draw_356 = {
#include "stg24/terrain/draw_356.inc"
};
static NJS_CNK_OBJECT draw_352 = {
#include "stg24/terrain/draw_352.inc"
};
static NJS_CNK_OBJECT draw_351 = {
#include "stg24/terrain/draw_351.inc"
};
static NJS_CNK_OBJECT draw_350 = {
#include "stg24/terrain/draw_350.inc"
};
static NJS_CNK_OBJECT draw_349 = {
#include "stg24/terrain/draw_349.inc"
};
static NJS_CNK_OBJECT draw_348 = {
#include "stg24/terrain/draw_348.inc"
};
static NJS_CNK_OBJECT draw_347 = {
#include "stg24/terrain/draw_347.inc"
};
static NJS_CNK_OBJECT draw_346 = {
#include "stg24/terrain/draw_346.inc"
};
static NJS_CNK_OBJECT draw_345 = {
#include "stg24/terrain/draw_345.inc"
};
static NJS_CNK_OBJECT draw_344 = {
#include "stg24/terrain/draw_344.inc"
};
static NJS_CNK_OBJECT draw_343 = {
#include "stg24/terrain/draw_343.inc"
};
static NJS_CNK_OBJECT draw_342 = {
#include "stg24/terrain/draw_342.inc"
};
static NJS_CNK_OBJECT draw_341 = {
#include "stg24/terrain/draw_341.inc"
};
static NJS_CNK_OBJECT draw_340 = {
#include "stg24/terrain/draw_340.inc"
};
static NJS_CNK_OBJECT draw_339 = {
#include "stg24/terrain/draw_339.inc"
};
static NJS_CNK_OBJECT draw_338 = {
#include "stg24/terrain/draw_338.inc"
};
static NJS_CNK_OBJECT draw_337 = {
#include "stg24/terrain/draw_337.inc"
};
static NJS_CNK_OBJECT draw_336 = {
#include "stg24/terrain/draw_336.inc"
};
static NJS_CNK_OBJECT draw_335 = {
#include "stg24/terrain/draw_335.inc"
};
static NJS_CNK_OBJECT draw_334 = {
#include "stg24/terrain/draw_334.inc"
};
static NJS_CNK_OBJECT draw_333 = {
#include "stg24/terrain/draw_333.inc"
};
static NJS_CNK_OBJECT draw_332 = {
#include "stg24/terrain/draw_332.inc"
};
static NJS_CNK_OBJECT draw_331 = {
#include "stg24/terrain/draw_331.inc"
};
static NJS_CNK_OBJECT draw_330 = {
#include "stg24/terrain/draw_330.inc"
};
static NJS_CNK_OBJECT draw_329 = {
#include "stg24/terrain/draw_329.inc"
};
static NJS_CNK_OBJECT draw_328 = {
#include "stg24/terrain/draw_328.inc"
};
static NJS_CNK_OBJECT draw_327 = {
#include "stg24/terrain/draw_327.inc"
};
static NJS_CNK_OBJECT draw_326 = {
#include "stg24/terrain/draw_326.inc"
};
static NJS_CNK_OBJECT draw_325 = {
#include "stg24/terrain/draw_325.inc"
};
static NJS_CNK_OBJECT draw_324 = {
#include "stg24/terrain/draw_324.inc"
};
static NJS_CNK_OBJECT draw_323 = {
#include "stg24/terrain/draw_323.inc"
};
static NJS_CNK_OBJECT draw_322 = {
#include "stg24/terrain/draw_322.inc"
};
static NJS_CNK_OBJECT draw_321 = {
#include "stg24/terrain/draw_321.inc"
};
static NJS_CNK_OBJECT draw_320 = {
#include "stg24/terrain/draw_320.inc"
};
static NJS_CNK_OBJECT draw_319 = {
#include "stg24/terrain/draw_319.inc"
};
static NJS_CNK_OBJECT draw_318 = {
#include "stg24/terrain/draw_318.inc"
};
static NJS_CNK_OBJECT draw_317 = {
#include "stg24/terrain/draw_317.inc"
};
static NJS_CNK_OBJECT draw_314 = {
#include "stg24/terrain/draw_314.inc"
};
static NJS_CNK_OBJECT draw_313 = {
#include "stg24/terrain/draw_313.inc"
};
static NJS_CNK_OBJECT draw_312 = {
#include "stg24/terrain/draw_312.inc"
};
static NJS_CNK_OBJECT draw_311 = {
#include "stg24/terrain/draw_311.inc"
};
static NJS_CNK_OBJECT draw_306 = {
#include "stg24/terrain/draw_306.inc"
};
static NJS_CNK_OBJECT draw_305 = {
#include "stg24/terrain/draw_305.inc"
};
static NJS_CNK_OBJECT draw_302 = {
#include "stg24/terrain/draw_302.inc"
};
static NJS_CNK_OBJECT draw_301 = {
#include "stg24/terrain/draw_301.inc"
};
static NJS_CNK_OBJECT draw_300 = {
#include "stg24/terrain/draw_300.inc"
};
static NJS_CNK_OBJECT draw_299 = {
#include "stg24/terrain/draw_299.inc"
};
static NJS_CNK_OBJECT draw_298 = {
#include "stg24/terrain/draw_298.inc"
};
static NJS_CNK_OBJECT draw_297 = {
#include "stg24/terrain/draw_297.inc"
};
static NJS_CNK_OBJECT draw_295 = {
#include "stg24/terrain/draw_295.inc"
};
static NJS_CNK_OBJECT draw_294 = {
#include "stg24/terrain/draw_294.inc"
};
static NJS_CNK_OBJECT draw_293 = {
#include "stg24/terrain/draw_293.inc"
};
static NJS_CNK_OBJECT draw_292 = {
#include "stg24/terrain/draw_292.inc"
};
static NJS_CNK_OBJECT draw_291 = {
#include "stg24/terrain/draw_291.inc"
};
static NJS_CNK_OBJECT draw_285 = {
#include "stg24/terrain/draw_285.inc"
};
static NJS_CNK_OBJECT draw_284 = {
#include "stg24/terrain/draw_284.inc"
};
static NJS_CNK_OBJECT draw_283 = {
#include "stg24/terrain/draw_283.inc"
};
static NJS_CNK_OBJECT draw_282 = {
#include "stg24/terrain/draw_282.inc"
};
static NJS_CNK_OBJECT draw_281 = {
#include "stg24/terrain/draw_281.inc"
};
static NJS_CNK_OBJECT draw_280 = {
#include "stg24/terrain/draw_280.inc"
};
static NJS_CNK_OBJECT draw_277 = {
#include "stg24/terrain/draw_277.inc"
};
static NJS_CNK_OBJECT draw_275 = {
#include "stg24/terrain/draw_275.inc"
};
static NJS_CNK_OBJECT draw_272 = {
#include "stg24/terrain/draw_272.inc"
};
static NJS_CNK_OBJECT draw_271 = {
#include "stg24/terrain/draw_271.inc"
};
static NJS_CNK_OBJECT draw_270 = {
#include "stg24/terrain/draw_270.inc"
};
static NJS_CNK_OBJECT draw_269 = {
#include "stg24/terrain/draw_269.inc"
};
static NJS_CNK_OBJECT draw_268 = {
#include "stg24/terrain/draw_268.inc"
};
static NJS_CNK_OBJECT draw_266 = {
#include "stg24/terrain/draw_266.inc"
};
static NJS_CNK_OBJECT draw_265 = {
#include "stg24/terrain/draw_265.inc"
};
static NJS_CNK_OBJECT draw_264 = {
#include "stg24/terrain/draw_264.inc"
};
static NJS_CNK_OBJECT draw_263 = {
#include "stg24/terrain/draw_263.inc"
};
static NJS_CNK_OBJECT draw_262 = {
#include "stg24/terrain/draw_262.inc"
};
static NJS_CNK_OBJECT draw_261 = {
#include "stg24/terrain/draw_261.inc"
};
static NJS_CNK_OBJECT draw_260 = {
#include "stg24/terrain/draw_260.inc"
};
static NJS_CNK_OBJECT draw_259 = {
#include "stg24/terrain/draw_259.inc"
};
static NJS_CNK_OBJECT draw_258 = {
#include "stg24/terrain/draw_258.inc"
};
static NJS_CNK_OBJECT draw_255 = {
#include "stg24/terrain/draw_255.inc"
};
static NJS_CNK_OBJECT draw_254 = {
#include "stg24/terrain/draw_254.inc"
};
static NJS_CNK_OBJECT draw_253 = {
#include "stg24/terrain/draw_253.inc"
};
static NJS_CNK_OBJECT draw_252 = {
#include "stg24/terrain/draw_252.inc"
};
static NJS_CNK_OBJECT draw_251 = {
#include "stg24/terrain/draw_251.inc"
};
static NJS_CNK_OBJECT draw_250 = {
#include "stg24/terrain/draw_250.inc"
};
static NJS_CNK_OBJECT draw_249 = {
#include "stg24/terrain/draw_249.inc"
};
static NJS_CNK_OBJECT draw_248 = {
#include "stg24/terrain/draw_248.inc"
};
static NJS_CNK_OBJECT draw_247 = {
#include "stg24/terrain/draw_247.inc"
};
static NJS_CNK_OBJECT draw_246 = {
#include "stg24/terrain/draw_246.inc"
};
static NJS_CNK_OBJECT draw_245 = {
#include "stg24/terrain/draw_245.inc"
};
static NJS_CNK_OBJECT draw_244 = {
#include "stg24/terrain/draw_244.inc"
};
static NJS_CNK_OBJECT draw_241 = {
#include "stg24/terrain/draw_241.inc"
};
static NJS_CNK_OBJECT draw_240 = {
#include "stg24/terrain/draw_240.inc"
};
static NJS_CNK_OBJECT draw_239 = {
#include "stg24/terrain/draw_239.inc"
};
static NJS_CNK_OBJECT draw_237 = {
#include "stg24/terrain/draw_237.inc"
};
static NJS_CNK_OBJECT draw_236 = {
#include "stg24/terrain/draw_236.inc"
};
static NJS_CNK_OBJECT draw_235 = {
#include "stg24/terrain/draw_235.inc"
};
static NJS_CNK_OBJECT draw_234 = {
#include "stg24/terrain/draw_234.inc"
};
static NJS_CNK_OBJECT draw_233 = {
#include "stg24/terrain/draw_233.inc"
};
static NJS_CNK_OBJECT draw_232 = {
#include "stg24/terrain/draw_232.inc"
};
static NJS_CNK_OBJECT draw_231 = {
#include "stg24/terrain/draw_231.inc"
};
static NJS_CNK_OBJECT draw_230 = {
#include "stg24/terrain/draw_230.inc"
};
static NJS_CNK_OBJECT draw_229 = {
#include "stg24/terrain/draw_229.inc"
};
static NJS_CNK_OBJECT draw_228 = {
#include "stg24/terrain/draw_228.inc"
};
static NJS_CNK_OBJECT draw_227 = {
#include "stg24/terrain/draw_227.inc"
};
static NJS_CNK_OBJECT draw_226 = {
#include "stg24/terrain/draw_226.inc"
};
static NJS_CNK_OBJECT draw_225 = {
#include "stg24/terrain/draw_225.inc"
};
static NJS_CNK_OBJECT draw_224 = {
#include "stg24/terrain/draw_224.inc"
};
static NJS_CNK_OBJECT draw_223 = {
#include "stg24/terrain/draw_223.inc"
};
static NJS_CNK_OBJECT draw_222 = {
#include "stg24/terrain/draw_222.inc"
};
static NJS_CNK_OBJECT draw_221 = {
#include "stg24/terrain/draw_221.inc"
};
static NJS_CNK_OBJECT draw_220 = {
#include "stg24/terrain/draw_220.inc"
};
static NJS_CNK_OBJECT draw_219 = {
#include "stg24/terrain/draw_219.inc"
};
static NJS_CNK_OBJECT draw_218 = {
#include "stg24/terrain/draw_218.inc"
};
static NJS_CNK_OBJECT draw_217 = {
#include "stg24/terrain/draw_217.inc"
};
static NJS_CNK_OBJECT draw_216 = {
#include "stg24/terrain/draw_216.inc"
};
static NJS_CNK_OBJECT draw_215 = {
#include "stg24/terrain/draw_215.inc"
};
static NJS_CNK_OBJECT draw_214 = {
#include "stg24/terrain/draw_214.inc"
};
static NJS_CNK_OBJECT draw_213 = {
#include "stg24/terrain/draw_213.inc"
};
static NJS_CNK_OBJECT draw_212 = {
#include "stg24/terrain/draw_212.inc"
};
static NJS_CNK_OBJECT draw_211 = {
#include "stg24/terrain/draw_211.inc"
};
static NJS_CNK_OBJECT draw_210 = {
#include "stg24/terrain/draw_210.inc"
};
static NJS_CNK_OBJECT draw_209 = {
#include "stg24/terrain/draw_209.inc"
};
static NJS_CNK_OBJECT draw_208 = {
#include "stg24/terrain/draw_208.inc"
};
static NJS_CNK_OBJECT draw_207 = {
#include "stg24/terrain/draw_207.inc"
};
static NJS_CNK_OBJECT draw_206 = {
#include "stg24/terrain/draw_206.inc"
};
static NJS_CNK_OBJECT draw_205 = {
#include "stg24/terrain/draw_205.inc"
};
static NJS_CNK_OBJECT draw_204 = {
#include "stg24/terrain/draw_204.inc"
};
static NJS_CNK_OBJECT draw_203 = {
#include "stg24/terrain/draw_203.inc"
};
static NJS_CNK_OBJECT draw_202 = {
#include "stg24/terrain/draw_202.inc"
};
static NJS_CNK_OBJECT draw_201 = {
#include "stg24/terrain/draw_201.inc"
};
static NJS_CNK_OBJECT draw_200 = {
#include "stg24/terrain/draw_200.inc"
};
static NJS_CNK_OBJECT draw_199 = {
#include "stg24/terrain/draw_199.inc"
};
static NJS_CNK_OBJECT draw_198 = {
#include "stg24/terrain/draw_198.inc"
};
static NJS_CNK_OBJECT draw_197 = {
#include "stg24/terrain/draw_197.inc"
};
static NJS_CNK_OBJECT draw_196 = {
#include "stg24/terrain/draw_196.inc"
};
static NJS_CNK_OBJECT draw_188 = {
#include "stg24/terrain/draw_188.inc"
};
static NJS_CNK_OBJECT draw_186 = {
#include "stg24/terrain/draw_186.inc"
};
static NJS_CNK_OBJECT draw_185 = {
#include "stg24/terrain/draw_185.inc"
};
static NJS_CNK_OBJECT draw_183 = {
#include "stg24/terrain/draw_183.inc"
};
static NJS_CNK_OBJECT draw_182 = {
#include "stg24/terrain/draw_182.inc"
};
static NJS_CNK_OBJECT draw_181 = {
#include "stg24/terrain/draw_181.inc"
};
static NJS_CNK_OBJECT draw_180 = {
#include "stg24/terrain/draw_180.inc"
};
static NJS_CNK_OBJECT draw_179 = {
#include "stg24/terrain/draw_179.inc"
};
static NJS_CNK_OBJECT draw_178 = {
#include "stg24/terrain/draw_178.inc"
};
static NJS_CNK_OBJECT draw_177 = {
#include "stg24/terrain/draw_177.inc"
};
static NJS_CNK_OBJECT draw_176 = {
#include "stg24/terrain/draw_176.inc"
};
static NJS_CNK_OBJECT draw_175 = {
#include "stg24/terrain/draw_175.inc"
};
static NJS_CNK_OBJECT draw_173 = {
#include "stg24/terrain/draw_173.inc"
};
static NJS_CNK_OBJECT draw_172 = {
#include "stg24/terrain/draw_172.inc"
};
static NJS_CNK_OBJECT draw_171 = {
#include "stg24/terrain/draw_171.inc"
};
static NJS_CNK_OBJECT draw_170 = {
#include "stg24/terrain/draw_170.inc"
};
static NJS_CNK_OBJECT draw_169 = {
#include "stg24/terrain/draw_169.inc"
};
static NJS_CNK_OBJECT draw_168 = {
#include "stg24/terrain/draw_168.inc"
};
static NJS_CNK_OBJECT draw_167 = {
#include "stg24/terrain/draw_167.inc"
};
static NJS_CNK_OBJECT draw_166 = {
#include "stg24/terrain/draw_166.inc"
};
static NJS_CNK_OBJECT draw_165 = {
#include "stg24/terrain/draw_165.inc"
};
static NJS_CNK_OBJECT draw_164 = {
#include "stg24/terrain/draw_164.inc"
};
static NJS_CNK_OBJECT draw_163 = {
#include "stg24/terrain/draw_163.inc"
};
static NJS_CNK_OBJECT draw_162 = {
#include "stg24/terrain/draw_162.inc"
};
static NJS_CNK_OBJECT draw_160 = {
#include "stg24/terrain/draw_160.inc"
};
static NJS_CNK_OBJECT draw_159 = {
#include "stg24/terrain/draw_159.inc"
};
static NJS_CNK_OBJECT draw_158 = {
#include "stg24/terrain/draw_158.inc"
};
static NJS_CNK_OBJECT draw_157 = {
#include "stg24/terrain/draw_157.inc"
};
static NJS_CNK_OBJECT draw_156 = {
#include "stg24/terrain/draw_156.inc"
};
static NJS_CNK_OBJECT draw_155 = {
#include "stg24/terrain/draw_155.inc"
};
static NJS_CNK_OBJECT draw_154 = {
#include "stg24/terrain/draw_154.inc"
};
static NJS_CNK_OBJECT draw_153 = {
#include "stg24/terrain/draw_153.inc"
};
static NJS_CNK_OBJECT draw_152 = {
#include "stg24/terrain/draw_152.inc"
};
static NJS_CNK_OBJECT draw_151 = {
#include "stg24/terrain/draw_151.inc"
};
static NJS_CNK_OBJECT draw_150 = {
#include "stg24/terrain/draw_150.inc"
};
static NJS_CNK_OBJECT draw_149 = {
#include "stg24/terrain/draw_149.inc"
};
static NJS_CNK_OBJECT draw_148 = {
#include "stg24/terrain/draw_148.inc"
};
static NJS_CNK_OBJECT draw_147 = {
#include "stg24/terrain/draw_147.inc"
};
static NJS_CNK_OBJECT draw_146 = {
#include "stg24/terrain/draw_146.inc"
};
static NJS_CNK_OBJECT draw_145 = {
#include "stg24/terrain/draw_145.inc"
};
static NJS_CNK_OBJECT draw_144 = {
#include "stg24/terrain/draw_144.inc"
};
static NJS_CNK_OBJECT draw_143 = {
#include "stg24/terrain/draw_143.inc"
};
static NJS_CNK_OBJECT draw_142 = {
#include "stg24/terrain/draw_142.inc"
};
static NJS_CNK_OBJECT draw_139 = {
#include "stg24/terrain/draw_139.inc"
};
static NJS_CNK_OBJECT draw_138 = {
#include "stg24/terrain/draw_138.inc"
};
static NJS_CNK_OBJECT draw_137 = {
#include "stg24/terrain/draw_137.inc"
};
static NJS_CNK_OBJECT draw_136 = {
#include "stg24/terrain/draw_136.inc"
};
static NJS_CNK_OBJECT draw_135 = {
#include "stg24/terrain/draw_135.inc"
};
static NJS_CNK_OBJECT draw_134 = {
#include "stg24/terrain/draw_134.inc"
};
static NJS_CNK_OBJECT draw_133 = {
#include "stg24/terrain/draw_133.inc"
};
static NJS_CNK_OBJECT draw_132 = {
#include "stg24/terrain/draw_132.inc"
};
static NJS_CNK_OBJECT draw_131 = {
#include "stg24/terrain/draw_131.inc"
};
static NJS_CNK_OBJECT draw_130 = {
#include "stg24/terrain/draw_130.inc"
};
static NJS_CNK_OBJECT draw_129 = {
#include "stg24/terrain/draw_129.inc"
};
static NJS_CNK_OBJECT draw_128 = {
#include "stg24/terrain/draw_128.inc"
};
static NJS_CNK_OBJECT draw_127 = {
#include "stg24/terrain/draw_127.inc"
};
static NJS_CNK_OBJECT draw_126 = {
#include "stg24/terrain/draw_126.inc"
};
static NJS_CNK_OBJECT draw_125 = {
#include "stg24/terrain/draw_125.inc"
};
static NJS_CNK_OBJECT draw_124 = {
#include "stg24/terrain/draw_124.inc"
};
static NJS_CNK_OBJECT draw_123 = {
#include "stg24/terrain/draw_123.inc"
};
static NJS_CNK_OBJECT draw_122 = {
#include "stg24/terrain/draw_122.inc"
};
static NJS_CNK_OBJECT draw_121 = {
#include "stg24/terrain/draw_121.inc"
};
static NJS_CNK_OBJECT draw_120 = {
#include "stg24/terrain/draw_120.inc"
};
static NJS_CNK_OBJECT draw_119 = {
#include "stg24/terrain/draw_119.inc"
};
static NJS_CNK_OBJECT draw_118 = {
#include "stg24/terrain/draw_118.inc"
};
static NJS_CNK_OBJECT draw_117 = {
#include "stg24/terrain/draw_117.inc"
};
static NJS_CNK_OBJECT draw_116 = {
#include "stg24/terrain/draw_116.inc"
};
static NJS_CNK_OBJECT draw_115 = {
#include "stg24/terrain/draw_115.inc"
};
static NJS_CNK_OBJECT draw_114 = {
#include "stg24/terrain/draw_114.inc"
};
static NJS_CNK_OBJECT draw_113 = {
#include "stg24/terrain/draw_113.inc"
};
static NJS_CNK_OBJECT draw_112 = {
#include "stg24/terrain/draw_112.inc"
};
static NJS_CNK_OBJECT draw_111 = {
#include "stg24/terrain/draw_111.inc"
};
static NJS_CNK_OBJECT draw_110 = {
#include "stg24/terrain/draw_110.inc"
};
static NJS_CNK_OBJECT draw_109 = {
#include "stg24/terrain/draw_109.inc"
};
static NJS_CNK_OBJECT draw_108 = {
#include "stg24/terrain/draw_108.inc"
};
static NJS_CNK_OBJECT draw_107 = {
#include "stg24/terrain/draw_107.inc"
};
static NJS_CNK_OBJECT draw_106 = {
#include "stg24/terrain/draw_106.inc"
};
static NJS_CNK_OBJECT draw_105 = {
#include "stg24/terrain/draw_105.inc"
};
static NJS_CNK_OBJECT draw_104 = {
#include "stg24/terrain/draw_104.inc"
};
static NJS_CNK_OBJECT draw_103 = {
#include "stg24/terrain/draw_103.inc"
};
static NJS_CNK_OBJECT draw_102 = {
#include "stg24/terrain/draw_102.inc"
};
static NJS_CNK_OBJECT draw_101 = {
#include "stg24/terrain/draw_101.inc"
};
static NJS_CNK_OBJECT draw_100 = {
#include "stg24/terrain/draw_100.inc"
};
static NJS_CNK_OBJECT draw_98 = {
#include "stg24/terrain/draw_98.inc"
};
static NJS_CNK_OBJECT draw_97 = {
#include "stg24/terrain/draw_97.inc"
};
static NJS_CNK_OBJECT draw_96 = {
#include "stg24/terrain/draw_96.inc"
};
static NJS_CNK_OBJECT draw_95 = {
#include "stg24/terrain/draw_95.inc"
};
static NJS_CNK_OBJECT draw_94 = {
#include "stg24/terrain/draw_94.inc"
};
static NJS_CNK_OBJECT draw_93 = {
#include "stg24/terrain/draw_93.inc"
};
static NJS_CNK_OBJECT draw_92 = {
#include "stg24/terrain/draw_92.inc"
};
static NJS_CNK_OBJECT draw_91 = {
#include "stg24/terrain/draw_91.inc"
};
static NJS_CNK_OBJECT draw_90 = {
#include "stg24/terrain/draw_90.inc"
};
static NJS_CNK_OBJECT draw_89 = {
#include "stg24/terrain/draw_89.inc"
};
static NJS_CNK_OBJECT draw_88 = {
#include "stg24/terrain/draw_88.inc"
};
static NJS_CNK_OBJECT draw_87 = {
#include "stg24/terrain/draw_87.inc"
};
static NJS_CNK_OBJECT draw_86 = {
#include "stg24/terrain/draw_86.inc"
};
static NJS_CNK_OBJECT draw_85 = {
#include "stg24/terrain/draw_85.inc"
};
static NJS_CNK_OBJECT draw_84 = {
#include "stg24/terrain/draw_84.inc"
};
static NJS_CNK_OBJECT draw_83 = {
#include "stg24/terrain/draw_83.inc"
};
static NJS_CNK_OBJECT draw_82 = {
#include "stg24/terrain/draw_82.inc"
};
static NJS_CNK_OBJECT draw_81 = {
#include "stg24/terrain/draw_81.inc"
};
static NJS_CNK_OBJECT draw_80 = {
#include "stg24/terrain/draw_80.inc"
};
static NJS_CNK_OBJECT draw_79 = {
#include "stg24/terrain/draw_79.inc"
};
static NJS_CNK_OBJECT draw_77 = {
#include "stg24/terrain/draw_77.inc"
};
static NJS_CNK_OBJECT draw_76 = {
#include "stg24/terrain/draw_76.inc"
};
static NJS_CNK_OBJECT draw_75 = {
#include "stg24/terrain/draw_75.inc"
};
static NJS_CNK_OBJECT draw_74 = {
#include "stg24/terrain/draw_74.inc"
};
static NJS_CNK_OBJECT draw_73 = {
#include "stg24/terrain/draw_73.inc"
};
static NJS_CNK_OBJECT draw_72 = {
#include "stg24/terrain/draw_72.inc"
};
static NJS_CNK_OBJECT draw_71 = {
#include "stg24/terrain/draw_71.inc"
};
static NJS_CNK_OBJECT draw_69 = {
#include "stg24/terrain/draw_69.inc"
};
static NJS_CNK_OBJECT draw_67 = {
#include "stg24/terrain/draw_67.inc"
};
static NJS_CNK_OBJECT draw_66 = {
#include "stg24/terrain/draw_66.inc"
};
static NJS_CNK_OBJECT draw_65 = {
#include "stg24/terrain/draw_65.inc"
};
static NJS_CNK_OBJECT draw_64 = {
#include "stg24/terrain/draw_64.inc"
};
static NJS_CNK_OBJECT draw_63 = {
#include "stg24/terrain/draw_63.inc"
};
static NJS_CNK_OBJECT draw_62 = {
#include "stg24/terrain/draw_62.inc"
};
static NJS_CNK_OBJECT draw_61 = {
#include "stg24/terrain/draw_61.inc"
};
static NJS_CNK_OBJECT draw_60 = {
#include "stg24/terrain/draw_60.inc"
};
static NJS_CNK_OBJECT draw_59 = {
#include "stg24/terrain/draw_59.inc"
};
static NJS_CNK_OBJECT draw_58 = {
#include "stg24/terrain/draw_58.inc"
};
static NJS_CNK_OBJECT draw_57 = {
#include "stg24/terrain/draw_57.inc"
};
static NJS_CNK_OBJECT draw_56 = {
#include "stg24/terrain/draw_56.inc"
};
static NJS_CNK_OBJECT draw_55 = {
#include "stg24/terrain/draw_55.inc"
};
static NJS_CNK_OBJECT draw_54 = {
#include "stg24/terrain/draw_54.inc"
};
static NJS_CNK_OBJECT draw_53 = {
#include "stg24/terrain/draw_53.inc"
};
static NJS_CNK_OBJECT draw_52 = {
#include "stg24/terrain/draw_52.inc"
};
static NJS_CNK_OBJECT draw_51 = {
#include "stg24/terrain/draw_51.inc"
};
static NJS_CNK_OBJECT draw_50 = {
#include "stg24/terrain/draw_50.inc"
};
static NJS_CNK_OBJECT draw_49 = {
#include "stg24/terrain/draw_49.inc"
};
static NJS_CNK_OBJECT draw_48 = {
#include "stg24/terrain/draw_48.inc"
};
static NJS_CNK_OBJECT draw_47 = {
#include "stg24/terrain/draw_47.inc"
};
static NJS_CNK_OBJECT draw_46 = {
#include "stg24/terrain/draw_46.inc"
};
static NJS_CNK_OBJECT draw_45 = {
#include "stg24/terrain/draw_45.inc"
};
static NJS_CNK_OBJECT draw_44 = {
#include "stg24/terrain/draw_44.inc"
};
static NJS_CNK_OBJECT draw_43 = {
#include "stg24/terrain/draw_43.inc"
};
static NJS_CNK_OBJECT draw_42 = {
#include "stg24/terrain/draw_42.inc"
};
static NJS_CNK_OBJECT draw_41 = {
#include "stg24/terrain/draw_41.inc"
};
static NJS_CNK_OBJECT draw_40 = {
#include "stg24/terrain/draw_40.inc"
};
static NJS_CNK_OBJECT draw_39 = {
#include "stg24/terrain/draw_39.inc"
};
static NJS_CNK_OBJECT draw_38 = {
#include "stg24/terrain/draw_38.inc"
};
static NJS_CNK_OBJECT draw_37 = {
#include "stg24/terrain/draw_37.inc"
};
static NJS_CNK_OBJECT draw_36 = {
#include "stg24/terrain/draw_36.inc"
};
static NJS_CNK_OBJECT draw_35 = {
#include "stg24/terrain/draw_35.inc"
};
static NJS_CNK_OBJECT draw_34 = {
#include "stg24/terrain/draw_34.inc"
};
static NJS_CNK_OBJECT draw_33 = {
#include "stg24/terrain/draw_33.inc"
};
static NJS_CNK_OBJECT draw_32 = {
#include "stg24/terrain/draw_32.inc"
};
static NJS_CNK_OBJECT draw_31 = {
#include "stg24/terrain/draw_31.inc"
};
static NJS_CNK_OBJECT draw_30 = {
#include "stg24/terrain/draw_30.inc"
};
static NJS_CNK_OBJECT draw_29 = {
#include "stg24/terrain/draw_29.inc"
};
static NJS_CNK_OBJECT draw_28 = {
#include "stg24/terrain/draw_28.inc"
};
static NJS_CNK_OBJECT draw_27 = {
#include "stg24/terrain/draw_27.inc"
};
static NJS_CNK_OBJECT draw_26 = {
#include "stg24/terrain/draw_26.inc"
};
static NJS_CNK_OBJECT draw_25 = {
#include "stg24/terrain/draw_25.inc"
};
static NJS_CNK_OBJECT draw_24 = {
#include "stg24/terrain/draw_24.inc"
};
static NJS_CNK_OBJECT draw_23 = {
#include "stg24/terrain/draw_23.inc"
};
static NJS_CNK_OBJECT draw_22 = {
#include "stg24/terrain/draw_22.inc"
};
static NJS_CNK_OBJECT draw_21 = {
#include "stg24/terrain/draw_21.inc"
};
static NJS_CNK_OBJECT draw_20 = {
#include "stg24/terrain/draw_20.inc"
};
static NJS_CNK_OBJECT draw_19 = {
#include "stg24/terrain/draw_19.inc"
};
static NJS_CNK_OBJECT draw_18 = {
#include "stg24/terrain/draw_18.inc"
};
static NJS_CNK_OBJECT draw_17 = {
#include "stg24/terrain/draw_17.inc"
};
static NJS_CNK_OBJECT draw_16 = {
#include "stg24/terrain/draw_16.inc"
};
static NJS_CNK_OBJECT draw_15 = {
#include "stg24/terrain/draw_15.inc"
};
static NJS_CNK_OBJECT draw_14 = {
#include "stg24/terrain/draw_14.inc"
};
static NJS_CNK_OBJECT draw_13 = {
#include "stg24/terrain/draw_13.inc"
};
static NJS_CNK_OBJECT draw_9 = {
#include "stg24/terrain/draw_9.inc"
};
static NJS_CNK_OBJECT draw_8 = {
#include "stg24/terrain/draw_8.inc"
};
static NJS_CNK_OBJECT draw_7 = {
#include "stg24/terrain/draw_7.inc"
};
static NJS_CNK_OBJECT draw_6 = {
#include "stg24/terrain/draw_6.inc"
};
static NJS_CNK_OBJECT draw_5 = {
#include "stg24/terrain/draw_5.inc"
};
static NJS_CNK_OBJECT draw_4 = {
#include "stg24/terrain/draw_4.inc"
};
static NJS_CNK_OBJECT draw_3 = {
#include "stg24/terrain/draw_3.inc"
};
static NJS_CNK_OBJECT draw_2 = {
#include "stg24/terrain/draw_2.inc"
};
