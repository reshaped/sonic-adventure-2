#include <ninja.h>
#include <sg_xpt.h>

#include "collision/world.h"
#include "stg11/iron_gate.h"

/* the land's file names, which its land table and texture list point at. each
 * starts on a 4-byte boundary */
const char iron_gate_land_filenames[343] = {
#include "stg11/terrain/iron_gate_land_filenames.inc"
};

/* STG11.PRS's land, texture lists, Basic models and chunk models, from the
 * disc. its D lies between iron_gate.c and objects.c */

/* the data up to the last that a pointer names ahead of its definition, as
 * tentative definitions in the disc's order: GCC places each datum at its
 * first definition */
static struct land_entry entries[372];
struct land_table iron_gate_land;
static NJS_TEXNAME textures[35];
NJS_TEXLIST iron_gate_terrain_textures;
static NJS_MATERIAL collision_0_materials[1];
static Sint16 collision_0_faces[62];
static NJS_MESHSET collision_0_meshsets[1];
static NJS_POINT3 collision_0_points[33];
static NJS_MODEL collision_0_model;
static NJS_OBJECT collision_0;
static NJS_MATERIAL collision_1_materials[1];
static Sint16 collision_1_faces[375];
static NJS_MESHSET collision_1_meshsets[1];
static NJS_POINT3 collision_1_points[166];
static NJS_MODEL collision_1_model;
static NJS_OBJECT collision_1;
static NJS_MATERIAL collision_2_materials[1];
static Sint16 collision_2_faces[12];
static NJS_MESHSET collision_2_meshsets[1];
static NJS_POINT3 collision_2_points[10];
static NJS_MODEL collision_2_model;
static NJS_OBJECT collision_2;
static NJS_MATERIAL collision_3_materials[1];
static Sint16 collision_3_faces[10];
static NJS_MESHSET collision_3_meshsets[1];
static NJS_POINT3 collision_3_points[8];
static NJS_MODEL collision_3_model;
static NJS_OBJECT collision_3;
static NJS_MATERIAL collision_4_materials[1];
static Sint16 collision_4_faces[14];
static NJS_MESHSET collision_4_meshsets[1];
static NJS_POINT3 collision_4_points[12];
static NJS_MODEL collision_4_model;
static NJS_OBJECT collision_4;
static NJS_MATERIAL collision_5_materials[1];
static Sint16 collision_5_faces[16];
static NJS_MESHSET collision_5_meshsets[1];
static NJS_POINT3 collision_5_points[12];
static NJS_MODEL collision_5_model;
static NJS_OBJECT collision_5;
static NJS_MATERIAL collision_6_materials[1];
static Sint16 collision_6_faces[15];
static NJS_MESHSET collision_6_meshsets[1];
static NJS_POINT3 collision_6_points[8];
static NJS_MODEL collision_6_model;
static NJS_OBJECT collision_6;
static NJS_MATERIAL collision_8_materials[1];
static Sint16 collision_8_faces[9];
static NJS_MESHSET collision_8_meshsets[1];
static NJS_POINT3 collision_8_points[8];
static NJS_MODEL collision_8_model;
static NJS_OBJECT collision_8;
static NJS_MATERIAL collision_9_materials[1];
static Sint16 collision_9_faces[12];
static NJS_MESHSET collision_9_meshsets[1];
static NJS_POINT3 collision_9_points[10];
static NJS_MODEL collision_9_model;
static NJS_OBJECT collision_9;
static NJS_MATERIAL collision_10_materials[1];
static Sint16 collision_10_faces[12];
static NJS_MESHSET collision_10_meshsets[1];
static NJS_POINT3 collision_10_points[10];
static NJS_MODEL collision_10_model;
static NJS_OBJECT collision_10;
static NJS_MATERIAL collision_11_materials[1];
static Sint16 collision_11_faces[15];
static NJS_MESHSET collision_11_meshsets[1];
static NJS_POINT3 collision_11_points[8];
static NJS_MODEL collision_11_model;
static NJS_OBJECT collision_11;
static NJS_MATERIAL collision_12_materials[1];
static Sint16 collision_12_faces[14];
static NJS_MESHSET collision_12_meshsets[1];
static NJS_POINT3 collision_12_points[12];
static NJS_MODEL collision_12_model;
static NJS_OBJECT collision_12;
static NJS_MATERIAL collision_13_materials[1];
static Sint16 collision_13_faces[20];
static NJS_MESHSET collision_13_meshsets[1];
static NJS_POINT3 collision_13_points[12];
static NJS_MODEL collision_13_model;
static NJS_OBJECT collision_13;
static NJS_MATERIAL collision_14_materials[1];
static Sint16 collision_14_faces[129];
static NJS_MESHSET collision_14_meshsets[1];
static NJS_POINT3 collision_14_points[66];
static NJS_MODEL collision_14_model;
static NJS_OBJECT collision_14;
static NJS_MATERIAL collision_15_materials[1];
static Sint16 collision_15_faces[50];
static NJS_MESHSET collision_15_meshsets[1];
static NJS_POINT3 collision_15_points[27];
static NJS_MODEL collision_15_model;
static NJS_OBJECT collision_15;
static NJS_MATERIAL collision_16_materials[1];
static Sint16 collision_16_faces[38];
static NJS_MESHSET collision_16_meshsets[1];
static NJS_POINT3 collision_16_points[24];
static NJS_MODEL collision_16_model;
static NJS_OBJECT collision_16;
static NJS_MATERIAL collision_18_materials[1];
static Sint16 collision_18_faces[50];
static NJS_MESHSET collision_18_meshsets[1];
static NJS_POINT3 collision_18_points[27];
static NJS_MODEL collision_18_model;
static NJS_OBJECT collision_18;
static NJS_MATERIAL collision_20_materials[1];
static Sint16 collision_20_faces[25];
static NJS_MESHSET collision_20_meshsets[1];
static NJS_POINT3 collision_20_points[14];
static NJS_MODEL collision_20_model;
static NJS_OBJECT collision_20;
static NJS_MATERIAL collision_22_materials[1];
static Sint16 collision_22_faces[64];
static NJS_MESHSET collision_22_meshsets[1];
static NJS_POINT3 collision_22_points[26];
static NJS_MODEL collision_22_model;
static NJS_OBJECT collision_22;
static NJS_MATERIAL collision_23_materials[1];
static Sint16 collision_23_faces[45];
static NJS_MESHSET collision_23_meshsets[1];
static NJS_POINT3 collision_23_points[24];
static NJS_MODEL collision_23_model;
static NJS_OBJECT collision_23;
static NJS_MATERIAL collision_24_materials[1];
static Sint16 collision_24_faces[11];
static NJS_MESHSET collision_24_meshsets[1];
static NJS_POINT3 collision_24_points[8];
static NJS_MODEL collision_24_model;
static NJS_OBJECT collision_24;
static NJS_MATERIAL collision_26_materials[1];
static Sint16 collision_26_faces[30];
static NJS_MESHSET collision_26_meshsets[1];
static NJS_POINT3 collision_26_points[18];
static NJS_MODEL collision_26_model;
static NJS_OBJECT collision_26;
static NJS_MATERIAL collision_29_materials[1];
static Sint16 collision_29_faces[14];
static NJS_MESHSET collision_29_meshsets[1];
static NJS_POINT3 collision_29_points[8];
static NJS_MODEL collision_29_model;
static NJS_OBJECT collision_29;
static NJS_MATERIAL collision_31_materials[1];
static Sint16 collision_31_faces[97];
static NJS_MESHSET collision_31_meshsets[1];
static NJS_POINT3 collision_31_points[36];
static NJS_MODEL collision_31_model;
static NJS_OBJECT collision_31;
static NJS_MATERIAL collision_35_materials[1];
static Sint16 collision_35_faces[36];
static NJS_MESHSET collision_35_meshsets[1];
static NJS_POINT3 collision_35_points[16];
static NJS_MODEL collision_35_model;
static NJS_OBJECT collision_35;
static NJS_MATERIAL collision_39_materials[1];
static Sint16 collision_39_faces[46];
static NJS_MESHSET collision_39_meshsets[1];
static NJS_POINT3 collision_39_points[22];
static NJS_MODEL collision_39_model;
static NJS_OBJECT collision_39;
static NJS_MATERIAL collision_40_materials[1];
static Sint16 collision_40_faces[20];
static NJS_MESHSET collision_40_meshsets[1];
static NJS_POINT3 collision_40_points[16];
static NJS_MODEL collision_40_model;
static NJS_OBJECT collision_40;
static NJS_MATERIAL collision_41_materials[1];
static Sint16 collision_41_faces[11];
static NJS_MESHSET collision_41_meshsets[1];
static NJS_POINT3 collision_41_points[8];
static NJS_MODEL collision_41_model;
static NJS_OBJECT collision_41;
static NJS_MATERIAL collision_42_materials[1];
static Sint16 collision_42_faces[36];
static NJS_MESHSET collision_42_meshsets[1];
static NJS_POINT3 collision_42_points[20];
static NJS_MODEL collision_42_model;
static NJS_OBJECT collision_42;
static NJS_MATERIAL collision_46_materials[1];
static Sint16 collision_46_faces[104];
static NJS_MESHSET collision_46_meshsets[1];
static NJS_POINT3 collision_46_points[39];
static NJS_MODEL collision_46_model;
static NJS_OBJECT collision_46;
static NJS_MATERIAL collision_50_materials[1];
static Sint16 collision_50_faces[19];
static NJS_MESHSET collision_50_meshsets[1];
static NJS_POINT3 collision_50_points[8];
static NJS_MODEL collision_50_model;
static NJS_OBJECT collision_50;
static NJS_MATERIAL collision_56_materials[1];
static Sint16 collision_56_faces[21];
static NJS_MESHSET collision_56_meshsets[1];
static NJS_POINT3 collision_56_points[8];
static NJS_MODEL collision_56_model;
static NJS_OBJECT collision_56;
static NJS_MATERIAL collision_57_materials[1];
static Sint16 collision_57_faces[19];
static NJS_MESHSET collision_57_meshsets[1];
static NJS_POINT3 collision_57_points[8];
static NJS_MODEL collision_57_model;
static NJS_OBJECT collision_57;
static NJS_MATERIAL collision_58_materials[1];
static Sint16 collision_58_faces[11];
static NJS_MESHSET collision_58_meshsets[1];
static NJS_POINT3 collision_58_points[8];
static NJS_MODEL collision_58_model;
static NJS_OBJECT collision_58;
static NJS_MATERIAL collision_59_materials[1];
static Sint16 collision_59_faces[16];
static NJS_MESHSET collision_59_meshsets[1];
static NJS_POINT3 collision_59_points[8];
static NJS_MODEL collision_59_model;
static NJS_OBJECT collision_59;
static NJS_MATERIAL collision_60_materials[1];
static Sint16 collision_60_faces[16];
static NJS_MESHSET collision_60_meshsets[1];
static NJS_POINT3 collision_60_points[8];
static NJS_MODEL collision_60_model;
static NJS_OBJECT collision_60;
static NJS_MATERIAL collision_63_materials[1];
static Sint16 collision_63_faces[11];
static NJS_MESHSET collision_63_meshsets[1];
static NJS_POINT3 collision_63_points[8];
static NJS_MODEL collision_63_model;
static NJS_OBJECT collision_63;
static NJS_MATERIAL collision_64_materials[1];
static Sint16 collision_64_faces[16];
static NJS_MESHSET collision_64_meshsets[1];
static NJS_POINT3 collision_64_points[8];
static NJS_MODEL collision_64_model;
static NJS_OBJECT collision_64;
static NJS_MATERIAL collision_66_materials[1];
static Sint16 collision_66_faces[19];
static NJS_MESHSET collision_66_meshsets[1];
static NJS_POINT3 collision_66_points[8];
static NJS_MODEL collision_66_model;
static NJS_OBJECT collision_66;
static NJS_MATERIAL collision_68_materials[1];
static Sint16 collision_68_faces[16];
static NJS_MESHSET collision_68_meshsets[1];
static NJS_POINT3 collision_68_points[8];
static NJS_MODEL collision_68_model;
static NJS_OBJECT collision_68;
static NJS_MATERIAL collision_69_materials[1];
static Sint16 collision_69_faces[21];
static NJS_MESHSET collision_69_meshsets[1];
static NJS_POINT3 collision_69_points[8];
static NJS_MODEL collision_69_model;
static NJS_OBJECT collision_69;
static NJS_MATERIAL collision_70_materials[1];
static Sint16 collision_70_faces[9];
static NJS_MESHSET collision_70_meshsets[1];
static NJS_POINT3 collision_70_points[8];
static NJS_MODEL collision_70_model;
static NJS_OBJECT collision_70;
static NJS_MATERIAL collision_71_materials[1];
static Sint16 collision_71_faces[14];
static NJS_MESHSET collision_71_meshsets[1];
static NJS_POINT3 collision_71_points[8];
static NJS_MODEL collision_71_model;
static NJS_OBJECT collision_71;
static NJS_MATERIAL collision_72_materials[1];
static Sint16 collision_72_faces[36];
static NJS_MESHSET collision_72_meshsets[1];
static NJS_POINT3 collision_72_points[16];
static NJS_MODEL collision_72_model;
static NJS_OBJECT collision_72;
static NJS_MATERIAL collision_73_materials[1];
static Sint16 collision_73_faces[29];
static NJS_MESHSET collision_73_meshsets[1];
static NJS_POINT3 collision_73_points[22];
static NJS_MODEL collision_73_model;
static NJS_OBJECT collision_73;
static NJS_MATERIAL collision_74_materials[1];
static Sint16 collision_74_faces[36];
static NJS_MESHSET collision_74_meshsets[1];
static NJS_POINT3 collision_74_points[16];
static NJS_MODEL collision_74_model;
static NJS_OBJECT collision_74;
static NJS_MATERIAL collision_75_materials[1];
static Sint16 collision_75_faces[25];
static NJS_MESHSET collision_75_meshsets[1];
static NJS_POINT3 collision_75_points[12];
static NJS_MODEL collision_75_model;
static NJS_OBJECT collision_75;
static NJS_MATERIAL collision_61_materials[1];
static Sint16 collision_61_faces[46];
static NJS_MESHSET collision_61_meshsets[1];
static NJS_POINT3 collision_61_points[30];
static NJS_MODEL collision_61_model;
static NJS_OBJECT collision_76;
static NJS_MATERIAL collision_79_materials[1];
static Sint16 collision_79_faces[120];
static NJS_MESHSET collision_79_meshsets[1];
static NJS_POINT3 collision_79_points[57];
static NJS_MODEL collision_79_model;
static NJS_OBJECT collision_79;
static NJS_MATERIAL collision_80_materials[1];
static Sint16 collision_80_faces[250];
static NJS_MESHSET collision_80_meshsets[1];
static NJS_POINT3 collision_80_points[130];
static NJS_MODEL collision_80_model;
static NJS_OBJECT collision_80;
static NJS_MATERIAL collision_81_materials[1];
static Sint16 collision_81_faces[171];
static NJS_MESHSET collision_81_meshsets[1];
static NJS_POINT3 collision_81_points[87];
static NJS_MODEL collision_81_model;
static NJS_OBJECT collision_81;
static NJS_MATERIAL collision_85_materials[1];
static Sint16 collision_85_faces[21];
static NJS_MESHSET collision_85_meshsets[1];
static NJS_POINT3 collision_85_points[8];
static NJS_MODEL collision_85_model;
static NJS_OBJECT collision_85;
static NJS_MATERIAL collision_88_materials[1];
static Sint16 collision_88_faces[14];
static NJS_MESHSET collision_88_meshsets[1];
static NJS_POINT3 collision_88_points[8];
static NJS_MODEL collision_88_model;
static NJS_OBJECT collision_88;
static NJS_MATERIAL collision_99_materials[1];
static Sint16 collision_99_faces[100];
static NJS_MESHSET collision_99_meshsets[1];
static NJS_POINT3 collision_99_points[36];
static NJS_MODEL collision_99_model;
static NJS_OBJECT collision_99;
static NJS_MATERIAL collision_101_materials[1];
static Sint16 collision_101_faces[11];
static NJS_MESHSET collision_101_meshsets[1];
static NJS_POINT3 collision_101_points[8];
static NJS_MODEL collision_101_model;
static NJS_OBJECT collision_101;
static NJS_MATERIAL collision_102_materials[1];
static Sint16 collision_102_faces[54];
static NJS_MESHSET collision_102_meshsets[1];
static NJS_POINT3 collision_102_points[28];
static NJS_MODEL collision_102_model;
static NJS_OBJECT collision_102;
static NJS_MATERIAL collision_103_materials[1];
static Sint16 collision_103_faces[31];
static NJS_MESHSET collision_103_meshsets[1];
static NJS_POINT3 collision_103_points[18];
static NJS_MODEL collision_103_model;
static NJS_OBJECT collision_103;
static NJS_MATERIAL collision_110_materials[1];
static Sint16 collision_110_faces[19];
static NJS_MESHSET collision_110_meshsets[1];
static NJS_POINT3 collision_110_points[8];
static NJS_MODEL collision_110_model;
static NJS_OBJECT collision_110;
static NJS_MATERIAL collision_111_materials[1];
static Sint16 collision_111_faces[160];
static NJS_MESHSET collision_111_meshsets[1];
static NJS_POINT3 collision_111_points[57];
static NJS_MODEL collision_111_model;
static NJS_OBJECT collision_111;
static Sint16 draw_253_polygons[65];
static Sint32 draw_253_vertices[59];
static NJS_CNK_MODEL draw_253_model;
static NJS_CNK_OBJECT draw_259;
static Sint16 draw_257_polygons[153];
static Sint32 draw_257_vertices[95];
static NJS_CNK_MODEL draw_257_model;
static NJS_CNK_OBJECT draw_258;
static Sint16 draw_256_polygons[311];
static Sint32 draw_256_vertices[175];
static NJS_CNK_MODEL draw_256_model;
static NJS_CNK_OBJECT draw_256;
static Sint16 draw_255_polygons[23];
static Sint32 draw_255_vertices[19];
static NJS_CNK_MODEL draw_255_model;
static NJS_CNK_OBJECT draw_255;
static Sint16 draw_250_polygons[91];
static Sint32 draw_250_vertices[83];
static NJS_CNK_MODEL draw_250_model;
static NJS_CNK_OBJECT draw_252;
static Sint16 draw_247_polygons[65];
static Sint32 draw_247_vertices[59];
static NJS_CNK_MODEL draw_247_model;
static NJS_CNK_OBJECT draw_249;
static Sint16 draw_246_polygons[669];
static Sint32 draw_246_vertices[399];
static NJS_CNK_MODEL draw_246_model;
static NJS_CNK_OBJECT draw_246;
static Sint16 draw_245_polygons[155];
static Sint32 draw_245_vertices[115];
static NJS_CNK_MODEL draw_245_model;
static NJS_CNK_OBJECT draw_245;
static Sint16 draw_243_polygons[125];
static Sint32 draw_243_vertices[67];
static NJS_CNK_MODEL draw_243_model;
static NJS_CNK_OBJECT draw_244;
static Sint16 draw_240_polygons[263];
static Sint32 draw_240_vertices[223];
static NJS_CNK_MODEL draw_240_model;
static NJS_CNK_OBJECT draw_242;
static Sint16 draw_234_polygons[35];
static Sint32 draw_234_vertices[35];
static NJS_CNK_MODEL draw_234_model;
static NJS_CNK_OBJECT draw_239;
static Sint16 draw_15_polygons[119];
static Sint32 draw_15_vertices[67];
static NJS_CNK_MODEL draw_15_model;
static NJS_CNK_OBJECT draw_233;
static Sint16 draw_9_polygons[359];
static Sint32 draw_9_vertices[363];
static NJS_CNK_MODEL draw_9_model;
static NJS_CNK_OBJECT draw_231;
static Sint16 draw_49_polygons[329];
static Sint32 draw_49_vertices[303];
static NJS_CNK_MODEL draw_49_model;
static NJS_CNK_OBJECT draw_229;
static Sint16 draw_227_polygons[359];
static Sint32 draw_227_vertices[359];
static NJS_CNK_MODEL draw_227_model;
static NJS_CNK_OBJECT draw_227;
static Sint16 draw_21_polygons[123];
static Sint32 draw_21_vertices[91];
static NJS_CNK_MODEL draw_21_model;
static NJS_CNK_OBJECT draw_226;
static Sint16 draw_7_polygons[375];
static Sint32 draw_7_vertices[395];
static NJS_CNK_MODEL draw_7_model;
static NJS_CNK_OBJECT draw_224;
static Sint16 draw_46_polygons[273];
static Sint32 draw_46_vertices[187];
static NJS_CNK_MODEL draw_46_model;
static NJS_CNK_OBJECT draw_222;
static Sint16 draw_45_polygons[423];
static Sint32 draw_45_vertices[491];
static NJS_CNK_MODEL draw_45_model;
static NJS_CNK_OBJECT draw_220;
static Sint16 draw_44_polygons[147];
static Sint32 draw_44_vertices[83];
static NJS_CNK_MODEL draw_44_model;
static NJS_CNK_OBJECT draw_218;
static Sint16 draw_56_polygons[153];
static Sint32 draw_56_vertices[131];
static NJS_CNK_MODEL draw_56_model;
static NJS_CNK_OBJECT draw_216;
static Sint16 draw_6_polygons[359];
static Sint32 draw_6_vertices[351];
static NJS_CNK_MODEL draw_6_model;
static NJS_CNK_OBJECT draw_214;
static Sint16 draw_154_polygons[263];
static Sint32 draw_154_vertices[135];
static NJS_CNK_MODEL draw_154_model;
static NJS_CNK_OBJECT draw_208;
static Sint16 draw_153_polygons[701];
static Sint32 draw_153_vertices[523];
static NJS_CNK_MODEL draw_153_model;
static NJS_CNK_OBJECT draw_207;
static Sint16 draw_80_polygons[247];
static Sint32 draw_80_vertices[187];
static NJS_CNK_MODEL draw_80_model;
static NJS_CNK_OBJECT draw_200;
static Sint16 draw_79_polygons[95];
static Sint32 draw_79_vertices[59];
static NJS_CNK_MODEL draw_79_model;
static NJS_CNK_OBJECT draw_199;
static Sint16 draw_78_polygons[413];
static Sint32 draw_78_vertices[411];
static NJS_CNK_MODEL draw_78_model;
static NJS_CNK_OBJECT draw_198;
static Sint16 draw_77_polygons[277];
static Sint32 draw_77_vertices[179];
static NJS_CNK_MODEL draw_77_model;
static NJS_CNK_OBJECT draw_197;
static Sint16 draw_75_polygons[215];
static Sint32 draw_75_vertices[243];
static NJS_CNK_MODEL draw_75_model;
static NJS_CNK_OBJECT draw_196;
static Sint16 draw_74_polygons[121];
static Sint32 draw_74_vertices[115];
static NJS_CNK_MODEL draw_74_model;
static NJS_CNK_OBJECT draw_194;
static Sint16 draw_73_polygons[387];
static Sint32 draw_73_vertices[391];
static NJS_CNK_MODEL draw_73_model;
static NJS_CNK_OBJECT draw_193;
static Sint16 draw_72_polygons[431];
static Sint32 draw_72_vertices[323];
static NJS_CNK_MODEL draw_72_model;
static NJS_CNK_OBJECT draw_192;
static Sint16 draw_68_polygons[215];
static Sint32 draw_68_vertices[243];
static NJS_CNK_MODEL draw_68_model;
static NJS_CNK_OBJECT draw_191;
static Sint16 draw_66_polygons[121];
static Sint32 draw_66_vertices[115];
static NJS_CNK_MODEL draw_66_model;
static NJS_CNK_OBJECT draw_187;
static Sint16 draw_63_polygons[389];
static Sint32 draw_63_vertices[415];
static NJS_CNK_MODEL draw_63_model;
static NJS_CNK_OBJECT draw_185;
static Sint16 draw_62_polygons[121];
static Sint32 draw_62_vertices[115];
static NJS_CNK_MODEL draw_62_model;
static NJS_CNK_OBJECT draw_182;
static Sint16 draw_61_polygons[385];
static Sint32 draw_61_vertices[259];
static NJS_CNK_MODEL draw_61_model;
static NJS_CNK_OBJECT draw_181;
static Sint16 draw_60_polygons[167];
static Sint32 draw_60_vertices[107];
static NJS_CNK_MODEL draw_60_model;
static NJS_CNK_OBJECT draw_180;
static Sint16 draw_58_polygons[431];
static Sint32 draw_58_vertices[323];
static NJS_CNK_MODEL draw_58_model;
static NJS_CNK_OBJECT draw_179;
static Sint16 draw_57_polygons[189];
static Sint32 draw_57_vertices[147];
static NJS_CNK_MODEL draw_57_model;
static NJS_CNK_OBJECT draw_177;
static Sint16 draw_166_polygons[465];
static Sint32 draw_166_vertices[339];
static NJS_CNK_MODEL draw_166_model;
static NJS_CNK_OBJECT draw_166;
static Sint16 draw_165_polygons[699];
static Sint32 draw_165_vertices[767];
static NJS_CNK_MODEL draw_165_model;
static NJS_CNK_OBJECT draw_165;
static Sint16 draw_164_polygons[375];
static Sint32 draw_164_vertices[387];
static NJS_CNK_MODEL draw_164_model;
static NJS_CNK_OBJECT draw_164;
static Sint16 draw_163_polygons[177];
static Sint32 draw_163_vertices[83];
static NJS_CNK_MODEL draw_163_model;
static NJS_CNK_OBJECT draw_163;
static Sint16 draw_159_polygons[133];
static Sint32 draw_159_vertices[67];
static NJS_CNK_MODEL draw_159_model;
static NJS_CNK_OBJECT draw_159;
static Sint16 draw_155_polygons[71];
static Sint32 draw_155_vertices[51];
static NJS_CNK_MODEL draw_155_model;
static NJS_CNK_OBJECT draw_155;
static Sint16 draw_97_polygons[617];
static Sint32 draw_97_vertices[635];
static NJS_CNK_MODEL draw_97_model;
static NJS_CNK_OBJECT draw_144;
static Sint16 draw_8_polygons[359];
static Sint32 draw_8_vertices[355];
static NJS_CNK_MODEL draw_8_model;
static NJS_CNK_OBJECT draw_143;
static Sint16 draw_91_polygons[309];
static Sint32 draw_91_vertices[323];
static NJS_CNK_MODEL draw_91_model;
static NJS_CNK_OBJECT draw_138;
static Sint16 draw_136_polygons[35];
static Sint32 draw_136_vertices[35];
static NJS_CNK_MODEL draw_136_model;
static NJS_CNK_OBJECT draw_136;
static Sint16 draw_85_polygons[129];
static Sint32 draw_85_vertices[111];
static NJS_CNK_MODEL draw_85_model;
static NJS_CNK_OBJECT draw_133;
static Sint16 draw_90_polygons[377];
static Sint32 draw_90_vertices[399];
static NJS_CNK_MODEL draw_90_model;
static NJS_CNK_OBJECT draw_116;
static Sint16 draw_115_polygons[521];
static Sint32 draw_115_vertices[279];
static NJS_CNK_MODEL draw_115_model;
static NJS_CNK_OBJECT draw_115;
static Sint16 draw_114_polygons[301];
static Sint32 draw_114_vertices[223];
static NJS_CNK_MODEL draw_114_model;
static NJS_CNK_OBJECT draw_114;
static Sint16 draw_113_polygons[105];
static Sint32 draw_113_vertices[59];
static NJS_CNK_MODEL draw_113_model;
static NJS_CNK_OBJECT draw_113;
static Sint16 draw_112_polygons[275];
static Sint32 draw_112_vertices[187];
static NJS_CNK_MODEL draw_112_model;
static NJS_CNK_OBJECT draw_112;
static Sint16 draw_111_polygons[49];
static Sint32 draw_111_vertices[35];
static NJS_CNK_MODEL draw_111_model;
static NJS_CNK_OBJECT draw_111;
static Sint16 draw_110_polygons[319];
static Sint32 draw_110_vertices[251];
static NJS_CNK_MODEL draw_110_model;
static NJS_CNK_OBJECT draw_110;
static Sint16 draw_109_polygons[29];
static Sint32 draw_109_vertices[27];
static NJS_CNK_MODEL draw_109_model;
static NJS_CNK_OBJECT draw_109;
static Sint16 draw_86_polygons[205];
static Sint32 draw_86_vertices[175];
static NJS_CNK_MODEL draw_86_model;
static NJS_CNK_OBJECT draw_108;
static Sint16 draw_105_polygons[501];
static Sint32 draw_105_vertices[235];
static NJS_CNK_MODEL draw_105_model;
static NJS_CNK_OBJECT draw_105;
static Sint16 draw_104_polygons[403];
static Sint32 draw_104_vertices[447];
static NJS_CNK_MODEL draw_104_model;
static NJS_CNK_OBJECT draw_104;
static Sint16 draw_84_polygons[189];
static Sint32 draw_84_vertices[67];
static NJS_CNK_MODEL draw_84_model;
static NJS_CNK_OBJECT draw_103;
static Sint16 draw_102_polygons[527];
static Sint32 draw_102_vertices[519];
static NJS_CNK_MODEL draw_102_model;
static NJS_CNK_OBJECT draw_102;
static Sint16 draw_101_polygons[145];
static Sint32 draw_101_vertices[67];
static NJS_CNK_MODEL draw_101_model;
static NJS_CNK_OBJECT draw_101;
static Sint16 draw_100_polygons[355];
static Sint32 draw_100_vertices[407];
static NJS_CNK_MODEL draw_100_model;
static NJS_CNK_OBJECT draw_100;
static Sint16 draw_99_polygons[407];
static Sint32 draw_99_vertices[243];
static NJS_CNK_MODEL draw_99_model;
static NJS_CNK_OBJECT draw_99;
static Sint16 draw_98_polygons[221];
static Sint32 draw_98_vertices[163];
static NJS_CNK_MODEL draw_98_model;
static NJS_CNK_OBJECT draw_98;
static Sint16 draw_89_polygons[49];
static Sint32 draw_89_vertices[35];
static NJS_CNK_MODEL draw_89_model;
static NJS_CNK_OBJECT draw_89;
static Sint16 draw_88_polygons[29];
static Sint32 draw_88_vertices[27];
static NJS_CNK_MODEL draw_88_model;
static NJS_CNK_OBJECT draw_88;
static Sint16 draw_38_polygons[329];
static Sint32 draw_38_vertices[303];
static NJS_CNK_MODEL draw_38_model;
static NJS_CNK_OBJECT draw_38;
static Sint16 draw_37_polygons[359];
static Sint32 draw_37_vertices[359];
static NJS_CNK_MODEL draw_37_model;
static NJS_CNK_OBJECT draw_37;
static Sint16 draw_36_polygons[129];
static Sint32 draw_36_vertices[95];
static NJS_CNK_MODEL draw_36_model;
static NJS_CNK_OBJECT draw_36;
static Sint16 draw_35_polygons[375];
static Sint32 draw_35_vertices[395];
static NJS_CNK_MODEL draw_35_model;
static NJS_CNK_OBJECT draw_35;
static Sint16 draw_34_polygons[289];
static Sint32 draw_34_vertices[223];
static NJS_CNK_MODEL draw_34_model;
static NJS_CNK_OBJECT draw_34;
static Sint16 draw_33_polygons[425];
static Sint32 draw_33_vertices[491];
static NJS_CNK_MODEL draw_33_model;
static NJS_CNK_OBJECT draw_33;
static Sint16 draw_32_polygons[163];
static Sint32 draw_32_vertices[87];
static NJS_CNK_MODEL draw_32_model;
static NJS_CNK_OBJECT draw_32;
static Sint16 draw_30_polygons[153];
static Sint32 draw_30_vertices[131];
static NJS_CNK_MODEL draw_30_model;
static NJS_CNK_OBJECT draw_30;
static Sint16 draw_26_polygons[205];
static Sint32 draw_26_vertices[151];
static NJS_CNK_MODEL draw_26_model;
static NJS_CNK_OBJECT draw_26;
static Sint16 draw_25_polygons[205];
static Sint32 draw_25_vertices[211];
static NJS_CNK_MODEL draw_25_model;
static NJS_CNK_OBJECT draw_25;
static Sint16 draw_24_polygons[175];
static Sint32 draw_24_vertices[179];
static NJS_CNK_MODEL draw_24_model;
static NJS_CNK_OBJECT draw_24;
static Sint16 draw_23_polygons[743];
static Sint32 draw_23_vertices[395];
static NJS_CNK_MODEL draw_23_model;
static NJS_CNK_OBJECT draw_23;
static Sint16 draw_22_polygons[95];
static Sint32 draw_22_vertices[75];
static NJS_CNK_MODEL draw_22_model;
static NJS_CNK_OBJECT draw_22;
static Sint16 draw_5_polygons[137];
static Sint32 draw_5_vertices[91];
static NJS_CNK_MODEL draw_5_model;
static NJS_CNK_OBJECT draw_5;
static Sint16 draw_4_polygons[385];
static Sint32 draw_4_vertices[419];
static NJS_CNK_MODEL draw_4_model;
static NJS_CNK_OBJECT draw_4;
static Sint16 draw_3_polygons[287];
static Sint32 draw_3_vertices[307];
static NJS_CNK_MODEL draw_3_model;
static NJS_CNK_OBJECT draw_3;
static Sint16 draw_2_polygons[503];
static Sint32 draw_2_vertices[347];
static NJS_CNK_MODEL draw_2_model;
static NJS_CNK_OBJECT draw_2;
static Sint16 draw_0_polygons[23];
static Sint32 draw_0_vertices[19];
static NJS_CNK_MODEL draw_0_model;
static NJS_CNK_OBJECT draw_1;
static NJS_OBJECT collision_7;
static NJS_OBJECT collision_17;
static NJS_OBJECT collision_19;
static NJS_OBJECT collision_21;
static NJS_OBJECT collision_25;
static NJS_OBJECT collision_27;
static NJS_OBJECT collision_28;
static NJS_OBJECT collision_30;
static NJS_OBJECT collision_32;
static NJS_OBJECT collision_33;
static NJS_OBJECT collision_34;
static NJS_OBJECT collision_36;
static NJS_OBJECT collision_37;
static NJS_OBJECT collision_38;
static NJS_OBJECT collision_43;
static NJS_OBJECT collision_44;
static NJS_OBJECT collision_45;
static NJS_OBJECT collision_47;
static NJS_OBJECT collision_48;
static NJS_OBJECT collision_49;
static NJS_OBJECT collision_51;
static NJS_OBJECT collision_52;
static NJS_OBJECT collision_53;
static NJS_OBJECT collision_54;
static NJS_OBJECT collision_55;
static NJS_OBJECT collision_61;
static NJS_OBJECT collision_62;
static NJS_OBJECT collision_65;
static NJS_OBJECT collision_67;
static NJS_OBJECT collision_77;
static NJS_OBJECT collision_78;
static NJS_OBJECT collision_82;
static NJS_OBJECT collision_83;
static NJS_OBJECT collision_84;
static NJS_OBJECT collision_86;
static NJS_OBJECT collision_87;
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
static NJS_OBJECT collision_100;
static NJS_OBJECT collision_104;
static NJS_OBJECT collision_105;
static NJS_OBJECT collision_106;
static NJS_OBJECT collision_107;
static NJS_OBJECT collision_108;
static NJS_OBJECT collision_109;
static NJS_CNK_OBJECT draw_257;
static NJS_CNK_OBJECT draw_254;
static NJS_CNK_OBJECT draw_253;
static NJS_CNK_OBJECT draw_251;
static NJS_CNK_OBJECT draw_250;
static NJS_CNK_OBJECT draw_248;
static NJS_CNK_OBJECT draw_247;
static NJS_CNK_OBJECT draw_243;
static NJS_CNK_OBJECT draw_241;
static NJS_CNK_OBJECT draw_240;
static NJS_CNK_OBJECT draw_238;
static NJS_CNK_OBJECT draw_237;
static NJS_CNK_OBJECT draw_236;
static NJS_CNK_OBJECT draw_235;
static NJS_CNK_OBJECT draw_234;
static NJS_CNK_OBJECT draw_232;
static NJS_CNK_OBJECT draw_230;
static NJS_CNK_OBJECT draw_228;
static NJS_CNK_OBJECT draw_225;
static NJS_CNK_OBJECT draw_223;
static NJS_CNK_OBJECT draw_221;
static NJS_CNK_OBJECT draw_219;
static NJS_CNK_OBJECT draw_217;
static NJS_CNK_OBJECT draw_215;
static NJS_CNK_OBJECT draw_213;
static NJS_CNK_OBJECT draw_212;
static NJS_CNK_OBJECT draw_211;
static NJS_CNK_OBJECT draw_210;
static NJS_CNK_OBJECT draw_209;
static NJS_CNK_OBJECT draw_206;
static NJS_CNK_OBJECT draw_205;
static NJS_CNK_OBJECT draw_204;
static NJS_CNK_OBJECT draw_203;
static NJS_CNK_OBJECT draw_202;
static NJS_CNK_OBJECT draw_201;
static NJS_CNK_OBJECT draw_195;
static NJS_CNK_OBJECT draw_190;
static NJS_CNK_OBJECT draw_189;
static NJS_CNK_OBJECT draw_188;
static NJS_CNK_OBJECT draw_186;
static NJS_CNK_OBJECT draw_184;
static NJS_CNK_OBJECT draw_183;
static NJS_CNK_OBJECT draw_178;
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
static NJS_CNK_OBJECT draw_162;
static NJS_CNK_OBJECT draw_161;
static NJS_CNK_OBJECT draw_160;
static NJS_CNK_OBJECT draw_158;
static NJS_CNK_OBJECT draw_157;
static NJS_CNK_OBJECT draw_156;
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
static NJS_CNK_OBJECT draw_142;
static NJS_CNK_OBJECT draw_141;
static NJS_CNK_OBJECT draw_140;
static NJS_CNK_OBJECT draw_139;
static NJS_CNK_OBJECT draw_137;
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
static NJS_CNK_OBJECT draw_107;
static NJS_CNK_OBJECT draw_106;
static NJS_CNK_OBJECT draw_97;
static NJS_CNK_OBJECT draw_96;
static NJS_CNK_OBJECT draw_95;
static NJS_CNK_OBJECT draw_94;
static NJS_CNK_OBJECT draw_93;
static NJS_CNK_OBJECT draw_92;
static NJS_CNK_OBJECT draw_91;
static NJS_CNK_OBJECT draw_90;
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
static NJS_CNK_OBJECT draw_31;
static NJS_CNK_OBJECT draw_29;
static NJS_CNK_OBJECT draw_28;
static NJS_CNK_OBJECT draw_27;
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
static NJS_CNK_OBJECT draw_0;

static struct land_entry entries[372] = {
#include "stg11/terrain/entries.inc"
};
struct land_table iron_gate_land = {
#include "stg11/terrain/iron_gate_land.inc"
};
static NJS_TEXNAME textures[35] = {
#include "stg11/terrain/textures.inc"
};
NJS_TEXLIST iron_gate_terrain_textures = {
#include "stg11/terrain/iron_gate_terrain_textures.inc"
};
static NJS_MATERIAL collision_0_materials[1] = {
#include "stg11/terrain/collision_0_materials.inc"
};
static Sint16 collision_0_faces[62] = {
#include "stg11/terrain/collision_0_faces.inc"
};
static NJS_MESHSET collision_0_meshsets[1] = {
#include "stg11/terrain/collision_0_meshsets.inc"
};
static NJS_POINT3 collision_0_points[33] = {
#include "stg11/terrain/collision_0_points.inc"
};
static NJS_MODEL collision_0_model = {
#include "stg11/terrain/collision_0_model.inc"
};
static NJS_OBJECT collision_0 = {
#include "stg11/terrain/collision_0.inc"
};
static NJS_MATERIAL collision_1_materials[1] = {
#include "stg11/terrain/collision_1_materials.inc"
};
static Sint16 collision_1_faces[375] = {
#include "stg11/terrain/collision_1_faces.inc"
};
static NJS_MESHSET collision_1_meshsets[1] = {
#include "stg11/terrain/collision_1_meshsets.inc"
};
static NJS_POINT3 collision_1_points[166] = {
#include "stg11/terrain/collision_1_points.inc"
};
static NJS_MODEL collision_1_model = {
#include "stg11/terrain/collision_1_model.inc"
};
static NJS_OBJECT collision_1 = {
#include "stg11/terrain/collision_1.inc"
};
static NJS_MATERIAL collision_2_materials[1] = {
#include "stg11/terrain/collision_2_materials.inc"
};
static Sint16 collision_2_faces[12] = {
#include "stg11/terrain/collision_2_faces.inc"
};
static NJS_MESHSET collision_2_meshsets[1] = {
#include "stg11/terrain/collision_2_meshsets.inc"
};
static NJS_POINT3 collision_2_points[10] = {
#include "stg11/terrain/collision_2_points.inc"
};
static NJS_MODEL collision_2_model = {
#include "stg11/terrain/collision_2_model.inc"
};
static NJS_OBJECT collision_2 = {
#include "stg11/terrain/collision_2.inc"
};
static NJS_MATERIAL collision_3_materials[1] = {
#include "stg11/terrain/collision_3_materials.inc"
};
static Sint16 collision_3_faces[10] = {
#include "stg11/terrain/collision_3_faces.inc"
};
static NJS_MESHSET collision_3_meshsets[1] = {
#include "stg11/terrain/collision_3_meshsets.inc"
};
static NJS_POINT3 collision_3_points[8] = {
#include "stg11/terrain/collision_3_points.inc"
};
static NJS_MODEL collision_3_model = {
#include "stg11/terrain/collision_3_model.inc"
};
static NJS_OBJECT collision_3 = {
#include "stg11/terrain/collision_3.inc"
};
static NJS_MATERIAL collision_4_materials[1] = {
#include "stg11/terrain/collision_4_materials.inc"
};
static Sint16 collision_4_faces[14] = {
#include "stg11/terrain/collision_4_faces.inc"
};
static NJS_MESHSET collision_4_meshsets[1] = {
#include "stg11/terrain/collision_4_meshsets.inc"
};
static NJS_POINT3 collision_4_points[12] = {
#include "stg11/terrain/collision_4_points.inc"
};
static NJS_MODEL collision_4_model = {
#include "stg11/terrain/collision_4_model.inc"
};
static NJS_OBJECT collision_4 = {
#include "stg11/terrain/collision_4.inc"
};
static NJS_MATERIAL collision_5_materials[1] = {
#include "stg11/terrain/collision_5_materials.inc"
};
static Sint16 collision_5_faces[16] = {
#include "stg11/terrain/collision_5_faces.inc"
};
static NJS_MESHSET collision_5_meshsets[1] = {
#include "stg11/terrain/collision_5_meshsets.inc"
};
static NJS_POINT3 collision_5_points[12] = {
#include "stg11/terrain/collision_5_points.inc"
};
static NJS_MODEL collision_5_model = {
#include "stg11/terrain/collision_5_model.inc"
};
static NJS_OBJECT collision_5 = {
#include "stg11/terrain/collision_5.inc"
};
static NJS_MATERIAL collision_6_materials[1] = {
#include "stg11/terrain/collision_6_materials.inc"
};
static Sint16 collision_6_faces[15] = {
#include "stg11/terrain/collision_6_faces.inc"
};
static NJS_MESHSET collision_6_meshsets[1] = {
#include "stg11/terrain/collision_6_meshsets.inc"
};
static NJS_POINT3 collision_6_points[8] = {
#include "stg11/terrain/collision_6_points.inc"
};
static NJS_MODEL collision_6_model = {
#include "stg11/terrain/collision_6_model.inc"
};
static NJS_OBJECT collision_6 = {
#include "stg11/terrain/collision_6.inc"
};
static NJS_MATERIAL collision_8_materials[1] = {
#include "stg11/terrain/collision_8_materials.inc"
};
static Sint16 collision_8_faces[9] = {
#include "stg11/terrain/collision_8_faces.inc"
};
static NJS_MESHSET collision_8_meshsets[1] = {
#include "stg11/terrain/collision_8_meshsets.inc"
};
static NJS_POINT3 collision_8_points[8] = {
#include "stg11/terrain/collision_8_points.inc"
};
static NJS_MODEL collision_8_model = {
#include "stg11/terrain/collision_8_model.inc"
};
static NJS_OBJECT collision_8 = {
#include "stg11/terrain/collision_8.inc"
};
static NJS_MATERIAL collision_9_materials[1] = {
#include "stg11/terrain/collision_9_materials.inc"
};
static Sint16 collision_9_faces[12] = {
#include "stg11/terrain/collision_9_faces.inc"
};
static NJS_MESHSET collision_9_meshsets[1] = {
#include "stg11/terrain/collision_9_meshsets.inc"
};
static NJS_POINT3 collision_9_points[10] = {
#include "stg11/terrain/collision_9_points.inc"
};
static NJS_MODEL collision_9_model = {
#include "stg11/terrain/collision_9_model.inc"
};
static NJS_OBJECT collision_9 = {
#include "stg11/terrain/collision_9.inc"
};
static NJS_MATERIAL collision_10_materials[1] = {
#include "stg11/terrain/collision_10_materials.inc"
};
static Sint16 collision_10_faces[12] = {
#include "stg11/terrain/collision_10_faces.inc"
};
static NJS_MESHSET collision_10_meshsets[1] = {
#include "stg11/terrain/collision_10_meshsets.inc"
};
static NJS_POINT3 collision_10_points[10] = {
#include "stg11/terrain/collision_10_points.inc"
};
static NJS_MODEL collision_10_model = {
#include "stg11/terrain/collision_10_model.inc"
};
static NJS_OBJECT collision_10 = {
#include "stg11/terrain/collision_10.inc"
};
static NJS_MATERIAL collision_11_materials[1] = {
#include "stg11/terrain/collision_11_materials.inc"
};
static Sint16 collision_11_faces[15] = {
#include "stg11/terrain/collision_11_faces.inc"
};
static NJS_MESHSET collision_11_meshsets[1] = {
#include "stg11/terrain/collision_11_meshsets.inc"
};
static NJS_POINT3 collision_11_points[8] = {
#include "stg11/terrain/collision_11_points.inc"
};
static NJS_MODEL collision_11_model = {
#include "stg11/terrain/collision_11_model.inc"
};
static NJS_OBJECT collision_11 = {
#include "stg11/terrain/collision_11.inc"
};
static NJS_MATERIAL collision_12_materials[1] = {
#include "stg11/terrain/collision_12_materials.inc"
};
static Sint16 collision_12_faces[14] = {
#include "stg11/terrain/collision_12_faces.inc"
};
static NJS_MESHSET collision_12_meshsets[1] = {
#include "stg11/terrain/collision_12_meshsets.inc"
};
static NJS_POINT3 collision_12_points[12] = {
#include "stg11/terrain/collision_12_points.inc"
};
static NJS_MODEL collision_12_model = {
#include "stg11/terrain/collision_12_model.inc"
};
static NJS_OBJECT collision_12 = {
#include "stg11/terrain/collision_12.inc"
};
static NJS_MATERIAL collision_13_materials[1] = {
#include "stg11/terrain/collision_13_materials.inc"
};
static Sint16 collision_13_faces[20] = {
#include "stg11/terrain/collision_13_faces.inc"
};
static NJS_MESHSET collision_13_meshsets[1] = {
#include "stg11/terrain/collision_13_meshsets.inc"
};
static NJS_POINT3 collision_13_points[12] = {
#include "stg11/terrain/collision_13_points.inc"
};
static NJS_MODEL collision_13_model = {
#include "stg11/terrain/collision_13_model.inc"
};
static NJS_OBJECT collision_13 = {
#include "stg11/terrain/collision_13.inc"
};
static NJS_MATERIAL collision_14_materials[1] = {
#include "stg11/terrain/collision_14_materials.inc"
};
static Sint16 collision_14_faces[129] = {
#include "stg11/terrain/collision_14_faces.inc"
};
static NJS_MESHSET collision_14_meshsets[1] = {
#include "stg11/terrain/collision_14_meshsets.inc"
};
static NJS_POINT3 collision_14_points[66] = {
#include "stg11/terrain/collision_14_points.inc"
};
static NJS_MODEL collision_14_model = {
#include "stg11/terrain/collision_14_model.inc"
};
static NJS_OBJECT collision_14 = {
#include "stg11/terrain/collision_14.inc"
};
static NJS_MATERIAL collision_15_materials[1] = {
#include "stg11/terrain/collision_15_materials.inc"
};
static Sint16 collision_15_faces[50] = {
#include "stg11/terrain/collision_15_faces.inc"
};
static NJS_MESHSET collision_15_meshsets[1] = {
#include "stg11/terrain/collision_15_meshsets.inc"
};
static NJS_POINT3 collision_15_points[27] = {
#include "stg11/terrain/collision_15_points.inc"
};
static NJS_MODEL collision_15_model = {
#include "stg11/terrain/collision_15_model.inc"
};
static NJS_OBJECT collision_15 = {
#include "stg11/terrain/collision_15.inc"
};
static NJS_MATERIAL collision_16_materials[1] = {
#include "stg11/terrain/collision_16_materials.inc"
};
static Sint16 collision_16_faces[38] = {
#include "stg11/terrain/collision_16_faces.inc"
};
static NJS_MESHSET collision_16_meshsets[1] = {
#include "stg11/terrain/collision_16_meshsets.inc"
};
static NJS_POINT3 collision_16_points[24] = {
#include "stg11/terrain/collision_16_points.inc"
};
static NJS_MODEL collision_16_model = {
#include "stg11/terrain/collision_16_model.inc"
};
static NJS_OBJECT collision_16 = {
#include "stg11/terrain/collision_16.inc"
};
static NJS_MATERIAL collision_18_materials[1] = {
#include "stg11/terrain/collision_18_materials.inc"
};
static Sint16 collision_18_faces[50] = {
#include "stg11/terrain/collision_18_faces.inc"
};
static NJS_MESHSET collision_18_meshsets[1] = {
#include "stg11/terrain/collision_18_meshsets.inc"
};
static NJS_POINT3 collision_18_points[27] = {
#include "stg11/terrain/collision_18_points.inc"
};
static NJS_MODEL collision_18_model = {
#include "stg11/terrain/collision_18_model.inc"
};
static NJS_OBJECT collision_18 = {
#include "stg11/terrain/collision_18.inc"
};
static NJS_MATERIAL collision_20_materials[1] = {
#include "stg11/terrain/collision_20_materials.inc"
};
static Sint16 collision_20_faces[25] = {
#include "stg11/terrain/collision_20_faces.inc"
};
static NJS_MESHSET collision_20_meshsets[1] = {
#include "stg11/terrain/collision_20_meshsets.inc"
};
static NJS_POINT3 collision_20_points[14] = {
#include "stg11/terrain/collision_20_points.inc"
};
static NJS_MODEL collision_20_model = {
#include "stg11/terrain/collision_20_model.inc"
};
static NJS_OBJECT collision_20 = {
#include "stg11/terrain/collision_20.inc"
};
static NJS_MATERIAL collision_22_materials[1] = {
#include "stg11/terrain/collision_22_materials.inc"
};
static Sint16 collision_22_faces[64] = {
#include "stg11/terrain/collision_22_faces.inc"
};
static NJS_MESHSET collision_22_meshsets[1] = {
#include "stg11/terrain/collision_22_meshsets.inc"
};
static NJS_POINT3 collision_22_points[26] = {
#include "stg11/terrain/collision_22_points.inc"
};
static NJS_MODEL collision_22_model = {
#include "stg11/terrain/collision_22_model.inc"
};
static NJS_OBJECT collision_22 = {
#include "stg11/terrain/collision_22.inc"
};
static NJS_MATERIAL collision_23_materials[1] = {
#include "stg11/terrain/collision_23_materials.inc"
};
static Sint16 collision_23_faces[45] = {
#include "stg11/terrain/collision_23_faces.inc"
};
static NJS_MESHSET collision_23_meshsets[1] = {
#include "stg11/terrain/collision_23_meshsets.inc"
};
static NJS_POINT3 collision_23_points[24] = {
#include "stg11/terrain/collision_23_points.inc"
};
static NJS_MODEL collision_23_model = {
#include "stg11/terrain/collision_23_model.inc"
};
static NJS_OBJECT collision_23 = {
#include "stg11/terrain/collision_23.inc"
};
static NJS_MATERIAL collision_24_materials[1] = {
#include "stg11/terrain/collision_24_materials.inc"
};
static Sint16 collision_24_faces[11] = {
#include "stg11/terrain/collision_24_faces.inc"
};
static NJS_MESHSET collision_24_meshsets[1] = {
#include "stg11/terrain/collision_24_meshsets.inc"
};
static NJS_POINT3 collision_24_points[8] = {
#include "stg11/terrain/collision_24_points.inc"
};
static NJS_MODEL collision_24_model = {
#include "stg11/terrain/collision_24_model.inc"
};
static NJS_OBJECT collision_24 = {
#include "stg11/terrain/collision_24.inc"
};
static NJS_MATERIAL collision_26_materials[1] = {
#include "stg11/terrain/collision_26_materials.inc"
};
static Sint16 collision_26_faces[30] = {
#include "stg11/terrain/collision_26_faces.inc"
};
static NJS_MESHSET collision_26_meshsets[1] = {
#include "stg11/terrain/collision_26_meshsets.inc"
};
static NJS_POINT3 collision_26_points[18] = {
#include "stg11/terrain/collision_26_points.inc"
};
static NJS_MODEL collision_26_model = {
#include "stg11/terrain/collision_26_model.inc"
};
static NJS_OBJECT collision_26 = {
#include "stg11/terrain/collision_26.inc"
};
static NJS_MATERIAL collision_29_materials[1] = {
#include "stg11/terrain/collision_29_materials.inc"
};
static Sint16 collision_29_faces[14] = {
#include "stg11/terrain/collision_29_faces.inc"
};
static NJS_MESHSET collision_29_meshsets[1] = {
#include "stg11/terrain/collision_29_meshsets.inc"
};
static NJS_POINT3 collision_29_points[8] = {
#include "stg11/terrain/collision_29_points.inc"
};
static NJS_MODEL collision_29_model = {
#include "stg11/terrain/collision_29_model.inc"
};
static NJS_OBJECT collision_29 = {
#include "stg11/terrain/collision_29.inc"
};
static NJS_MATERIAL collision_31_materials[1] = {
#include "stg11/terrain/collision_31_materials.inc"
};
static Sint16 collision_31_faces[97] = {
#include "stg11/terrain/collision_31_faces.inc"
};
static NJS_MESHSET collision_31_meshsets[1] = {
#include "stg11/terrain/collision_31_meshsets.inc"
};
static NJS_POINT3 collision_31_points[36] = {
#include "stg11/terrain/collision_31_points.inc"
};
static NJS_MODEL collision_31_model = {
#include "stg11/terrain/collision_31_model.inc"
};
static NJS_OBJECT collision_31 = {
#include "stg11/terrain/collision_31.inc"
};
static NJS_MATERIAL collision_35_materials[1] = {
#include "stg11/terrain/collision_35_materials.inc"
};
static Sint16 collision_35_faces[36] = {
#include "stg11/terrain/collision_35_faces.inc"
};
static NJS_MESHSET collision_35_meshsets[1] = {
#include "stg11/terrain/collision_35_meshsets.inc"
};
static NJS_POINT3 collision_35_points[16] = {
#include "stg11/terrain/collision_35_points.inc"
};
static NJS_MODEL collision_35_model = {
#include "stg11/terrain/collision_35_model.inc"
};
static NJS_OBJECT collision_35 = {
#include "stg11/terrain/collision_35.inc"
};
static NJS_MATERIAL collision_39_materials[1] = {
#include "stg11/terrain/collision_39_materials.inc"
};
static Sint16 collision_39_faces[46] = {
#include "stg11/terrain/collision_39_faces.inc"
};
static NJS_MESHSET collision_39_meshsets[1] = {
#include "stg11/terrain/collision_39_meshsets.inc"
};
static NJS_POINT3 collision_39_points[22] = {
#include "stg11/terrain/collision_39_points.inc"
};
static NJS_MODEL collision_39_model = {
#include "stg11/terrain/collision_39_model.inc"
};
static NJS_OBJECT collision_39 = {
#include "stg11/terrain/collision_39.inc"
};
static NJS_MATERIAL collision_40_materials[1] = {
#include "stg11/terrain/collision_40_materials.inc"
};
static Sint16 collision_40_faces[20] = {
#include "stg11/terrain/collision_40_faces.inc"
};
static NJS_MESHSET collision_40_meshsets[1] = {
#include "stg11/terrain/collision_40_meshsets.inc"
};
static NJS_POINT3 collision_40_points[16] = {
#include "stg11/terrain/collision_40_points.inc"
};
static NJS_MODEL collision_40_model = {
#include "stg11/terrain/collision_40_model.inc"
};
static NJS_OBJECT collision_40 = {
#include "stg11/terrain/collision_40.inc"
};
static NJS_MATERIAL collision_41_materials[1] = {
#include "stg11/terrain/collision_41_materials.inc"
};
static Sint16 collision_41_faces[11] = {
#include "stg11/terrain/collision_41_faces.inc"
};
static NJS_MESHSET collision_41_meshsets[1] = {
#include "stg11/terrain/collision_41_meshsets.inc"
};
static NJS_POINT3 collision_41_points[8] = {
#include "stg11/terrain/collision_41_points.inc"
};
static NJS_MODEL collision_41_model = {
#include "stg11/terrain/collision_41_model.inc"
};
static NJS_OBJECT collision_41 = {
#include "stg11/terrain/collision_41.inc"
};
static NJS_MATERIAL collision_42_materials[1] = {
#include "stg11/terrain/collision_42_materials.inc"
};
static Sint16 collision_42_faces[36] = {
#include "stg11/terrain/collision_42_faces.inc"
};
static NJS_MESHSET collision_42_meshsets[1] = {
#include "stg11/terrain/collision_42_meshsets.inc"
};
static NJS_POINT3 collision_42_points[20] = {
#include "stg11/terrain/collision_42_points.inc"
};
static NJS_MODEL collision_42_model = {
#include "stg11/terrain/collision_42_model.inc"
};
static NJS_OBJECT collision_42 = {
#include "stg11/terrain/collision_42.inc"
};
static NJS_MATERIAL collision_46_materials[1] = {
#include "stg11/terrain/collision_46_materials.inc"
};
static Sint16 collision_46_faces[104] = {
#include "stg11/terrain/collision_46_faces.inc"
};
static NJS_MESHSET collision_46_meshsets[1] = {
#include "stg11/terrain/collision_46_meshsets.inc"
};
static NJS_POINT3 collision_46_points[39] = {
#include "stg11/terrain/collision_46_points.inc"
};
static NJS_MODEL collision_46_model = {
#include "stg11/terrain/collision_46_model.inc"
};
static NJS_OBJECT collision_46 = {
#include "stg11/terrain/collision_46.inc"
};
static NJS_MATERIAL collision_50_materials[1] = {
#include "stg11/terrain/collision_50_materials.inc"
};
static Sint16 collision_50_faces[19] = {
#include "stg11/terrain/collision_50_faces.inc"
};
static NJS_MESHSET collision_50_meshsets[1] = {
#include "stg11/terrain/collision_50_meshsets.inc"
};
static NJS_POINT3 collision_50_points[8] = {
#include "stg11/terrain/collision_50_points.inc"
};
static NJS_MODEL collision_50_model = {
#include "stg11/terrain/collision_50_model.inc"
};
static NJS_OBJECT collision_50 = {
#include "stg11/terrain/collision_50.inc"
};
static NJS_MATERIAL collision_56_materials[1] = {
#include "stg11/terrain/collision_56_materials.inc"
};
static Sint16 collision_56_faces[21] = {
#include "stg11/terrain/collision_56_faces.inc"
};
static NJS_MESHSET collision_56_meshsets[1] = {
#include "stg11/terrain/collision_56_meshsets.inc"
};
static NJS_POINT3 collision_56_points[8] = {
#include "stg11/terrain/collision_56_points.inc"
};
static NJS_MODEL collision_56_model = {
#include "stg11/terrain/collision_56_model.inc"
};
static NJS_OBJECT collision_56 = {
#include "stg11/terrain/collision_56.inc"
};
static NJS_MATERIAL collision_57_materials[1] = {
#include "stg11/terrain/collision_57_materials.inc"
};
static Sint16 collision_57_faces[19] = {
#include "stg11/terrain/collision_57_faces.inc"
};
static NJS_MESHSET collision_57_meshsets[1] = {
#include "stg11/terrain/collision_57_meshsets.inc"
};
static NJS_POINT3 collision_57_points[8] = {
#include "stg11/terrain/collision_57_points.inc"
};
static NJS_MODEL collision_57_model = {
#include "stg11/terrain/collision_57_model.inc"
};
static NJS_OBJECT collision_57 = {
#include "stg11/terrain/collision_57.inc"
};
static NJS_MATERIAL collision_58_materials[1] = {
#include "stg11/terrain/collision_58_materials.inc"
};
static Sint16 collision_58_faces[11] = {
#include "stg11/terrain/collision_58_faces.inc"
};
static NJS_MESHSET collision_58_meshsets[1] = {
#include "stg11/terrain/collision_58_meshsets.inc"
};
static NJS_POINT3 collision_58_points[8] = {
#include "stg11/terrain/collision_58_points.inc"
};
static NJS_MODEL collision_58_model = {
#include "stg11/terrain/collision_58_model.inc"
};
static NJS_OBJECT collision_58 = {
#include "stg11/terrain/collision_58.inc"
};
static NJS_MATERIAL collision_59_materials[1] = {
#include "stg11/terrain/collision_59_materials.inc"
};
static Sint16 collision_59_faces[16] = {
#include "stg11/terrain/collision_59_faces.inc"
};
static NJS_MESHSET collision_59_meshsets[1] = {
#include "stg11/terrain/collision_59_meshsets.inc"
};
static NJS_POINT3 collision_59_points[8] = {
#include "stg11/terrain/collision_59_points.inc"
};
static NJS_MODEL collision_59_model = {
#include "stg11/terrain/collision_59_model.inc"
};
static NJS_OBJECT collision_59 = {
#include "stg11/terrain/collision_59.inc"
};
static NJS_MATERIAL collision_60_materials[1] = {
#include "stg11/terrain/collision_60_materials.inc"
};
static Sint16 collision_60_faces[16] = {
#include "stg11/terrain/collision_60_faces.inc"
};
static NJS_MESHSET collision_60_meshsets[1] = {
#include "stg11/terrain/collision_60_meshsets.inc"
};
static NJS_POINT3 collision_60_points[8] = {
#include "stg11/terrain/collision_60_points.inc"
};
static NJS_MODEL collision_60_model = {
#include "stg11/terrain/collision_60_model.inc"
};
static NJS_OBJECT collision_60 = {
#include "stg11/terrain/collision_60.inc"
};
static NJS_MATERIAL collision_63_materials[1] = {
#include "stg11/terrain/collision_63_materials.inc"
};
static Sint16 collision_63_faces[11] = {
#include "stg11/terrain/collision_63_faces.inc"
};
static NJS_MESHSET collision_63_meshsets[1] = {
#include "stg11/terrain/collision_63_meshsets.inc"
};
static NJS_POINT3 collision_63_points[8] = {
#include "stg11/terrain/collision_63_points.inc"
};
static NJS_MODEL collision_63_model = {
#include "stg11/terrain/collision_63_model.inc"
};
static NJS_OBJECT collision_63 = {
#include "stg11/terrain/collision_63.inc"
};
static NJS_MATERIAL collision_64_materials[1] = {
#include "stg11/terrain/collision_64_materials.inc"
};
static Sint16 collision_64_faces[16] = {
#include "stg11/terrain/collision_64_faces.inc"
};
static NJS_MESHSET collision_64_meshsets[1] = {
#include "stg11/terrain/collision_64_meshsets.inc"
};
static NJS_POINT3 collision_64_points[8] = {
#include "stg11/terrain/collision_64_points.inc"
};
static NJS_MODEL collision_64_model = {
#include "stg11/terrain/collision_64_model.inc"
};
static NJS_OBJECT collision_64 = {
#include "stg11/terrain/collision_64.inc"
};
static NJS_MATERIAL collision_66_materials[1] = {
#include "stg11/terrain/collision_66_materials.inc"
};
static Sint16 collision_66_faces[19] = {
#include "stg11/terrain/collision_66_faces.inc"
};
static NJS_MESHSET collision_66_meshsets[1] = {
#include "stg11/terrain/collision_66_meshsets.inc"
};
static NJS_POINT3 collision_66_points[8] = {
#include "stg11/terrain/collision_66_points.inc"
};
static NJS_MODEL collision_66_model = {
#include "stg11/terrain/collision_66_model.inc"
};
static NJS_OBJECT collision_66 = {
#include "stg11/terrain/collision_66.inc"
};
static NJS_MATERIAL collision_68_materials[1] = {
#include "stg11/terrain/collision_68_materials.inc"
};
static Sint16 collision_68_faces[16] = {
#include "stg11/terrain/collision_68_faces.inc"
};
static NJS_MESHSET collision_68_meshsets[1] = {
#include "stg11/terrain/collision_68_meshsets.inc"
};
static NJS_POINT3 collision_68_points[8] = {
#include "stg11/terrain/collision_68_points.inc"
};
static NJS_MODEL collision_68_model = {
#include "stg11/terrain/collision_68_model.inc"
};
static NJS_OBJECT collision_68 = {
#include "stg11/terrain/collision_68.inc"
};
static NJS_MATERIAL collision_69_materials[1] = {
#include "stg11/terrain/collision_69_materials.inc"
};
static Sint16 collision_69_faces[21] = {
#include "stg11/terrain/collision_69_faces.inc"
};
static NJS_MESHSET collision_69_meshsets[1] = {
#include "stg11/terrain/collision_69_meshsets.inc"
};
static NJS_POINT3 collision_69_points[8] = {
#include "stg11/terrain/collision_69_points.inc"
};
static NJS_MODEL collision_69_model = {
#include "stg11/terrain/collision_69_model.inc"
};
static NJS_OBJECT collision_69 = {
#include "stg11/terrain/collision_69.inc"
};
static NJS_MATERIAL collision_70_materials[1] = {
#include "stg11/terrain/collision_70_materials.inc"
};
static Sint16 collision_70_faces[9] = {
#include "stg11/terrain/collision_70_faces.inc"
};
static NJS_MESHSET collision_70_meshsets[1] = {
#include "stg11/terrain/collision_70_meshsets.inc"
};
static NJS_POINT3 collision_70_points[8] = {
#include "stg11/terrain/collision_70_points.inc"
};
static NJS_MODEL collision_70_model = {
#include "stg11/terrain/collision_70_model.inc"
};
static NJS_OBJECT collision_70 = {
#include "stg11/terrain/collision_70.inc"
};
static NJS_MATERIAL collision_71_materials[1] = {
#include "stg11/terrain/collision_71_materials.inc"
};
static Sint16 collision_71_faces[14] = {
#include "stg11/terrain/collision_71_faces.inc"
};
static NJS_MESHSET collision_71_meshsets[1] = {
#include "stg11/terrain/collision_71_meshsets.inc"
};
static NJS_POINT3 collision_71_points[8] = {
#include "stg11/terrain/collision_71_points.inc"
};
static NJS_MODEL collision_71_model = {
#include "stg11/terrain/collision_71_model.inc"
};
static NJS_OBJECT collision_71 = {
#include "stg11/terrain/collision_71.inc"
};
static NJS_MATERIAL collision_72_materials[1] = {
#include "stg11/terrain/collision_72_materials.inc"
};
static Sint16 collision_72_faces[36] = {
#include "stg11/terrain/collision_72_faces.inc"
};
static NJS_MESHSET collision_72_meshsets[1] = {
#include "stg11/terrain/collision_72_meshsets.inc"
};
static NJS_POINT3 collision_72_points[16] = {
#include "stg11/terrain/collision_72_points.inc"
};
static NJS_MODEL collision_72_model = {
#include "stg11/terrain/collision_72_model.inc"
};
static NJS_OBJECT collision_72 = {
#include "stg11/terrain/collision_72.inc"
};
static NJS_MATERIAL collision_73_materials[1] = {
#include "stg11/terrain/collision_73_materials.inc"
};
static Sint16 collision_73_faces[29] = {
#include "stg11/terrain/collision_73_faces.inc"
};
static NJS_MESHSET collision_73_meshsets[1] = {
#include "stg11/terrain/collision_73_meshsets.inc"
};
static NJS_POINT3 collision_73_points[22] = {
#include "stg11/terrain/collision_73_points.inc"
};
static NJS_MODEL collision_73_model = {
#include "stg11/terrain/collision_73_model.inc"
};
static NJS_OBJECT collision_73 = {
#include "stg11/terrain/collision_73.inc"
};
static NJS_MATERIAL collision_74_materials[1] = {
#include "stg11/terrain/collision_74_materials.inc"
};
static Sint16 collision_74_faces[36] = {
#include "stg11/terrain/collision_74_faces.inc"
};
static NJS_MESHSET collision_74_meshsets[1] = {
#include "stg11/terrain/collision_74_meshsets.inc"
};
static NJS_POINT3 collision_74_points[16] = {
#include "stg11/terrain/collision_74_points.inc"
};
static NJS_MODEL collision_74_model = {
#include "stg11/terrain/collision_74_model.inc"
};
static NJS_OBJECT collision_74 = {
#include "stg11/terrain/collision_74.inc"
};
static NJS_MATERIAL collision_75_materials[1] = {
#include "stg11/terrain/collision_75_materials.inc"
};
static Sint16 collision_75_faces[25] = {
#include "stg11/terrain/collision_75_faces.inc"
};
static NJS_MESHSET collision_75_meshsets[1] = {
#include "stg11/terrain/collision_75_meshsets.inc"
};
static NJS_POINT3 collision_75_points[12] = {
#include "stg11/terrain/collision_75_points.inc"
};
static NJS_MODEL collision_75_model = {
#include "stg11/terrain/collision_75_model.inc"
};
static NJS_OBJECT collision_75 = {
#include "stg11/terrain/collision_75.inc"
};
static NJS_MATERIAL collision_61_materials[1] = {
#include "stg11/terrain/collision_61_materials.inc"
};
static Sint16 collision_61_faces[46] = {
#include "stg11/terrain/collision_61_faces.inc"
};
static NJS_MESHSET collision_61_meshsets[1] = {
#include "stg11/terrain/collision_61_meshsets.inc"
};
static NJS_POINT3 collision_61_points[30] = {
#include "stg11/terrain/collision_61_points.inc"
};
static NJS_MODEL collision_61_model = {
#include "stg11/terrain/collision_61_model.inc"
};
static NJS_OBJECT collision_76 = {
#include "stg11/terrain/collision_76.inc"
};
static NJS_MATERIAL collision_79_materials[1] = {
#include "stg11/terrain/collision_79_materials.inc"
};
static Sint16 collision_79_faces[120] = {
#include "stg11/terrain/collision_79_faces.inc"
};
static NJS_MESHSET collision_79_meshsets[1] = {
#include "stg11/terrain/collision_79_meshsets.inc"
};
static NJS_POINT3 collision_79_points[57] = {
#include "stg11/terrain/collision_79_points.inc"
};
static NJS_MODEL collision_79_model = {
#include "stg11/terrain/collision_79_model.inc"
};
static NJS_OBJECT collision_79 = {
#include "stg11/terrain/collision_79.inc"
};
static NJS_MATERIAL collision_80_materials[1] = {
#include "stg11/terrain/collision_80_materials.inc"
};
static Sint16 collision_80_faces[250] = {
#include "stg11/terrain/collision_80_faces.inc"
};
static NJS_MESHSET collision_80_meshsets[1] = {
#include "stg11/terrain/collision_80_meshsets.inc"
};
static NJS_POINT3 collision_80_points[130] = {
#include "stg11/terrain/collision_80_points.inc"
};
static NJS_MODEL collision_80_model = {
#include "stg11/terrain/collision_80_model.inc"
};
static NJS_OBJECT collision_80 = {
#include "stg11/terrain/collision_80.inc"
};
static NJS_MATERIAL collision_81_materials[1] = {
#include "stg11/terrain/collision_81_materials.inc"
};
static Sint16 collision_81_faces[171] = {
#include "stg11/terrain/collision_81_faces.inc"
};
static NJS_MESHSET collision_81_meshsets[1] = {
#include "stg11/terrain/collision_81_meshsets.inc"
};
static NJS_POINT3 collision_81_points[87] = {
#include "stg11/terrain/collision_81_points.inc"
};
static NJS_MODEL collision_81_model = {
#include "stg11/terrain/collision_81_model.inc"
};
static NJS_OBJECT collision_81 = {
#include "stg11/terrain/collision_81.inc"
};
static NJS_MATERIAL collision_85_materials[1] = {
#include "stg11/terrain/collision_85_materials.inc"
};
static Sint16 collision_85_faces[21] = {
#include "stg11/terrain/collision_85_faces.inc"
};
static NJS_MESHSET collision_85_meshsets[1] = {
#include "stg11/terrain/collision_85_meshsets.inc"
};
static NJS_POINT3 collision_85_points[8] = {
#include "stg11/terrain/collision_85_points.inc"
};
static NJS_MODEL collision_85_model = {
#include "stg11/terrain/collision_85_model.inc"
};
static NJS_OBJECT collision_85 = {
#include "stg11/terrain/collision_85.inc"
};
static NJS_MATERIAL collision_88_materials[1] = {
#include "stg11/terrain/collision_88_materials.inc"
};
static Sint16 collision_88_faces[14] = {
#include "stg11/terrain/collision_88_faces.inc"
};
static NJS_MESHSET collision_88_meshsets[1] = {
#include "stg11/terrain/collision_88_meshsets.inc"
};
static NJS_POINT3 collision_88_points[8] = {
#include "stg11/terrain/collision_88_points.inc"
};
static NJS_MODEL collision_88_model = {
#include "stg11/terrain/collision_88_model.inc"
};
static NJS_OBJECT collision_88 = {
#include "stg11/terrain/collision_88.inc"
};
static NJS_MATERIAL collision_99_materials[1] = {
#include "stg11/terrain/collision_99_materials.inc"
};
static Sint16 collision_99_faces[100] = {
#include "stg11/terrain/collision_99_faces.inc"
};
static NJS_MESHSET collision_99_meshsets[1] = {
#include "stg11/terrain/collision_99_meshsets.inc"
};
static NJS_POINT3 collision_99_points[36] = {
#include "stg11/terrain/collision_99_points.inc"
};
static NJS_MODEL collision_99_model = {
#include "stg11/terrain/collision_99_model.inc"
};
static NJS_OBJECT collision_99 = {
#include "stg11/terrain/collision_99.inc"
};
static NJS_MATERIAL collision_101_materials[1] = {
#include "stg11/terrain/collision_101_materials.inc"
};
static Sint16 collision_101_faces[11] = {
#include "stg11/terrain/collision_101_faces.inc"
};
static NJS_MESHSET collision_101_meshsets[1] = {
#include "stg11/terrain/collision_101_meshsets.inc"
};
static NJS_POINT3 collision_101_points[8] = {
#include "stg11/terrain/collision_101_points.inc"
};
static NJS_MODEL collision_101_model = {
#include "stg11/terrain/collision_101_model.inc"
};
static NJS_OBJECT collision_101 = {
#include "stg11/terrain/collision_101.inc"
};
static NJS_MATERIAL collision_102_materials[1] = {
#include "stg11/terrain/collision_102_materials.inc"
};
static Sint16 collision_102_faces[54] = {
#include "stg11/terrain/collision_102_faces.inc"
};
static NJS_MESHSET collision_102_meshsets[1] = {
#include "stg11/terrain/collision_102_meshsets.inc"
};
static NJS_POINT3 collision_102_points[28] = {
#include "stg11/terrain/collision_102_points.inc"
};
static NJS_MODEL collision_102_model = {
#include "stg11/terrain/collision_102_model.inc"
};
static NJS_OBJECT collision_102 = {
#include "stg11/terrain/collision_102.inc"
};
static NJS_MATERIAL collision_103_materials[1] = {
#include "stg11/terrain/collision_103_materials.inc"
};
static Sint16 collision_103_faces[31] = {
#include "stg11/terrain/collision_103_faces.inc"
};
static NJS_MESHSET collision_103_meshsets[1] = {
#include "stg11/terrain/collision_103_meshsets.inc"
};
static NJS_POINT3 collision_103_points[18] = {
#include "stg11/terrain/collision_103_points.inc"
};
static NJS_MODEL collision_103_model = {
#include "stg11/terrain/collision_103_model.inc"
};
static NJS_OBJECT collision_103 = {
#include "stg11/terrain/collision_103.inc"
};
static NJS_MATERIAL collision_110_materials[1] = {
#include "stg11/terrain/collision_110_materials.inc"
};
static Sint16 collision_110_faces[19] = {
#include "stg11/terrain/collision_110_faces.inc"
};
static NJS_MESHSET collision_110_meshsets[1] = {
#include "stg11/terrain/collision_110_meshsets.inc"
};
static NJS_POINT3 collision_110_points[8] = {
#include "stg11/terrain/collision_110_points.inc"
};
static NJS_MODEL collision_110_model = {
#include "stg11/terrain/collision_110_model.inc"
};
static NJS_OBJECT collision_110 = {
#include "stg11/terrain/collision_110.inc"
};
static NJS_MATERIAL collision_111_materials[1] = {
#include "stg11/terrain/collision_111_materials.inc"
};
static Sint16 collision_111_faces[160] = {
#include "stg11/terrain/collision_111_faces.inc"
};
static NJS_MESHSET collision_111_meshsets[1] = {
#include "stg11/terrain/collision_111_meshsets.inc"
};
static NJS_POINT3 collision_111_points[57] = {
#include "stg11/terrain/collision_111_points.inc"
};
static NJS_MODEL collision_111_model = {
#include "stg11/terrain/collision_111_model.inc"
};
static NJS_OBJECT collision_111 = {
#include "stg11/terrain/collision_111.inc"
};
static Sint16 draw_253_polygons[65] = {
#include "stg11/terrain/draw_253_polygons.inc"
};
static Sint32 draw_253_vertices[59] = {
#include "stg11/terrain/draw_253_vertices.inc"
};
static NJS_CNK_MODEL draw_253_model = {
#include "stg11/terrain/draw_253_model.inc"
};
static NJS_CNK_OBJECT draw_259 = {
#include "stg11/terrain/draw_259.inc"
};
static Sint16 draw_257_polygons[153] = {
#include "stg11/terrain/draw_257_polygons.inc"
};
static Sint32 draw_257_vertices[95] = {
#include "stg11/terrain/draw_257_vertices.inc"
};
static NJS_CNK_MODEL draw_257_model = {
#include "stg11/terrain/draw_257_model.inc"
};
static NJS_CNK_OBJECT draw_258 = {
#include "stg11/terrain/draw_258.inc"
};
static Sint16 draw_256_polygons[311] = {
#include "stg11/terrain/draw_256_polygons.inc"
};
static Sint32 draw_256_vertices[175] = {
#include "stg11/terrain/draw_256_vertices.inc"
};
static NJS_CNK_MODEL draw_256_model = {
#include "stg11/terrain/draw_256_model.inc"
};
static NJS_CNK_OBJECT draw_256 = {
#include "stg11/terrain/draw_256.inc"
};
static Sint16 draw_255_polygons[23] = {
#include "stg11/terrain/draw_255_polygons.inc"
};
static Sint32 draw_255_vertices[19] = {
#include "stg11/terrain/draw_255_vertices.inc"
};
static NJS_CNK_MODEL draw_255_model = {
#include "stg11/terrain/draw_255_model.inc"
};
static NJS_CNK_OBJECT draw_255 = {
#include "stg11/terrain/draw_255.inc"
};
static Sint16 draw_250_polygons[91] = {
#include "stg11/terrain/draw_250_polygons.inc"
};
static Sint32 draw_250_vertices[83] = {
#include "stg11/terrain/draw_250_vertices.inc"
};
static NJS_CNK_MODEL draw_250_model = {
#include "stg11/terrain/draw_250_model.inc"
};
static NJS_CNK_OBJECT draw_252 = {
#include "stg11/terrain/draw_252.inc"
};
static Sint16 draw_247_polygons[65] = {
#include "stg11/terrain/draw_247_polygons.inc"
};
static Sint32 draw_247_vertices[59] = {
#include "stg11/terrain/draw_247_vertices.inc"
};
static NJS_CNK_MODEL draw_247_model = {
#include "stg11/terrain/draw_247_model.inc"
};
static NJS_CNK_OBJECT draw_249 = {
#include "stg11/terrain/draw_249.inc"
};
static Sint16 draw_246_polygons[669] = {
#include "stg11/terrain/draw_246_polygons.inc"
};
static Sint32 draw_246_vertices[399] = {
#include "stg11/terrain/draw_246_vertices.inc"
};
static NJS_CNK_MODEL draw_246_model = {
#include "stg11/terrain/draw_246_model.inc"
};
static NJS_CNK_OBJECT draw_246 = {
#include "stg11/terrain/draw_246.inc"
};
static Sint16 draw_245_polygons[155] = {
#include "stg11/terrain/draw_245_polygons.inc"
};
static Sint32 draw_245_vertices[115] = {
#include "stg11/terrain/draw_245_vertices.inc"
};
static NJS_CNK_MODEL draw_245_model = {
#include "stg11/terrain/draw_245_model.inc"
};
static NJS_CNK_OBJECT draw_245 = {
#include "stg11/terrain/draw_245.inc"
};
static Sint16 draw_243_polygons[125] = {
#include "stg11/terrain/draw_243_polygons.inc"
};
static Sint32 draw_243_vertices[67] = {
#include "stg11/terrain/draw_243_vertices.inc"
};
static NJS_CNK_MODEL draw_243_model = {
#include "stg11/terrain/draw_243_model.inc"
};
static NJS_CNK_OBJECT draw_244 = {
#include "stg11/terrain/draw_244.inc"
};
static Sint16 draw_240_polygons[263] = {
#include "stg11/terrain/draw_240_polygons.inc"
};
static Sint32 draw_240_vertices[223] = {
#include "stg11/terrain/draw_240_vertices.inc"
};
static NJS_CNK_MODEL draw_240_model = {
#include "stg11/terrain/draw_240_model.inc"
};
static NJS_CNK_OBJECT draw_242 = {
#include "stg11/terrain/draw_242.inc"
};
static Sint16 draw_234_polygons[35] = {
#include "stg11/terrain/draw_234_polygons.inc"
};
static Sint32 draw_234_vertices[35] = {
#include "stg11/terrain/draw_234_vertices.inc"
};
static NJS_CNK_MODEL draw_234_model = {
#include "stg11/terrain/draw_234_model.inc"
};
static NJS_CNK_OBJECT draw_239 = {
#include "stg11/terrain/draw_239.inc"
};
static Sint16 draw_15_polygons[119] = {
#include "stg11/terrain/draw_15_polygons.inc"
};
static Sint32 draw_15_vertices[67] = {
#include "stg11/terrain/draw_15_vertices.inc"
};
static NJS_CNK_MODEL draw_15_model = {
#include "stg11/terrain/draw_15_model.inc"
};
static NJS_CNK_OBJECT draw_233 = {
#include "stg11/terrain/draw_233.inc"
};
static Sint16 draw_9_polygons[359] = {
#include "stg11/terrain/draw_9_polygons.inc"
};
static Sint32 draw_9_vertices[363] = {
#include "stg11/terrain/draw_9_vertices.inc"
};
static NJS_CNK_MODEL draw_9_model = {
#include "stg11/terrain/draw_9_model.inc"
};
static NJS_CNK_OBJECT draw_231 = {
#include "stg11/terrain/draw_231.inc"
};
static Sint16 draw_49_polygons[329] = {
#include "stg11/terrain/draw_49_polygons.inc"
};
static Sint32 draw_49_vertices[303] = {
#include "stg11/terrain/draw_49_vertices.inc"
};
static NJS_CNK_MODEL draw_49_model = {
#include "stg11/terrain/draw_49_model.inc"
};
static NJS_CNK_OBJECT draw_229 = {
#include "stg11/terrain/draw_229.inc"
};
static Sint16 draw_227_polygons[359] = {
#include "stg11/terrain/draw_227_polygons.inc"
};
static Sint32 draw_227_vertices[359] = {
#include "stg11/terrain/draw_227_vertices.inc"
};
static NJS_CNK_MODEL draw_227_model = {
#include "stg11/terrain/draw_227_model.inc"
};
static NJS_CNK_OBJECT draw_227 = {
#include "stg11/terrain/draw_227.inc"
};
static Sint16 draw_21_polygons[123] = {
#include "stg11/terrain/draw_21_polygons.inc"
};
static Sint32 draw_21_vertices[91] = {
#include "stg11/terrain/draw_21_vertices.inc"
};
static NJS_CNK_MODEL draw_21_model = {
#include "stg11/terrain/draw_21_model.inc"
};
static NJS_CNK_OBJECT draw_226 = {
#include "stg11/terrain/draw_226.inc"
};
static Sint16 draw_7_polygons[375] = {
#include "stg11/terrain/draw_7_polygons.inc"
};
static Sint32 draw_7_vertices[395] = {
#include "stg11/terrain/draw_7_vertices.inc"
};
static NJS_CNK_MODEL draw_7_model = {
#include "stg11/terrain/draw_7_model.inc"
};
static NJS_CNK_OBJECT draw_224 = {
#include "stg11/terrain/draw_224.inc"
};
static Sint16 draw_46_polygons[273] = {
#include "stg11/terrain/draw_46_polygons.inc"
};
static Sint32 draw_46_vertices[187] = {
#include "stg11/terrain/draw_46_vertices.inc"
};
static NJS_CNK_MODEL draw_46_model = {
#include "stg11/terrain/draw_46_model.inc"
};
static NJS_CNK_OBJECT draw_222 = {
#include "stg11/terrain/draw_222.inc"
};
static Sint16 draw_45_polygons[423] = {
#include "stg11/terrain/draw_45_polygons.inc"
};
static Sint32 draw_45_vertices[491] = {
#include "stg11/terrain/draw_45_vertices.inc"
};
static NJS_CNK_MODEL draw_45_model = {
#include "stg11/terrain/draw_45_model.inc"
};
static NJS_CNK_OBJECT draw_220 = {
#include "stg11/terrain/draw_220.inc"
};
static Sint16 draw_44_polygons[147] = {
#include "stg11/terrain/draw_44_polygons.inc"
};
static Sint32 draw_44_vertices[83] = {
#include "stg11/terrain/draw_44_vertices.inc"
};
static NJS_CNK_MODEL draw_44_model = {
#include "stg11/terrain/draw_44_model.inc"
};
static NJS_CNK_OBJECT draw_218 = {
#include "stg11/terrain/draw_218.inc"
};
static Sint16 draw_56_polygons[153] = {
#include "stg11/terrain/draw_56_polygons.inc"
};
static Sint32 draw_56_vertices[131] = {
#include "stg11/terrain/draw_56_vertices.inc"
};
static NJS_CNK_MODEL draw_56_model = {
#include "stg11/terrain/draw_56_model.inc"
};
static NJS_CNK_OBJECT draw_216 = {
#include "stg11/terrain/draw_216.inc"
};
static Sint16 draw_6_polygons[359] = {
#include "stg11/terrain/draw_6_polygons.inc"
};
static Sint32 draw_6_vertices[351] = {
#include "stg11/terrain/draw_6_vertices.inc"
};
static NJS_CNK_MODEL draw_6_model = {
#include "stg11/terrain/draw_6_model.inc"
};
static NJS_CNK_OBJECT draw_214 = {
#include "stg11/terrain/draw_214.inc"
};
static Sint16 draw_154_polygons[263] = {
#include "stg11/terrain/draw_154_polygons.inc"
};
static Sint32 draw_154_vertices[135] = {
#include "stg11/terrain/draw_154_vertices.inc"
};
static NJS_CNK_MODEL draw_154_model = {
#include "stg11/terrain/draw_154_model.inc"
};
static NJS_CNK_OBJECT draw_208 = {
#include "stg11/terrain/draw_208.inc"
};
static Sint16 draw_153_polygons[701] = {
#include "stg11/terrain/draw_153_polygons.inc"
};
static Sint32 draw_153_vertices[523] = {
#include "stg11/terrain/draw_153_vertices.inc"
};
static NJS_CNK_MODEL draw_153_model = {
#include "stg11/terrain/draw_153_model.inc"
};
static NJS_CNK_OBJECT draw_207 = {
#include "stg11/terrain/draw_207.inc"
};
static Sint16 draw_80_polygons[247] = {
#include "stg11/terrain/draw_80_polygons.inc"
};
static Sint32 draw_80_vertices[187] = {
#include "stg11/terrain/draw_80_vertices.inc"
};
static NJS_CNK_MODEL draw_80_model = {
#include "stg11/terrain/draw_80_model.inc"
};
static NJS_CNK_OBJECT draw_200 = {
#include "stg11/terrain/draw_200.inc"
};
static Sint16 draw_79_polygons[95] = {
#include "stg11/terrain/draw_79_polygons.inc"
};
static Sint32 draw_79_vertices[59] = {
#include "stg11/terrain/draw_79_vertices.inc"
};
static NJS_CNK_MODEL draw_79_model = {
#include "stg11/terrain/draw_79_model.inc"
};
static NJS_CNK_OBJECT draw_199 = {
#include "stg11/terrain/draw_199.inc"
};
static Sint16 draw_78_polygons[413] = {
#include "stg11/terrain/draw_78_polygons.inc"
};
static Sint32 draw_78_vertices[411] = {
#include "stg11/terrain/draw_78_vertices.inc"
};
static NJS_CNK_MODEL draw_78_model = {
#include "stg11/terrain/draw_78_model.inc"
};
static NJS_CNK_OBJECT draw_198 = {
#include "stg11/terrain/draw_198.inc"
};
static Sint16 draw_77_polygons[277] = {
#include "stg11/terrain/draw_77_polygons.inc"
};
static Sint32 draw_77_vertices[179] = {
#include "stg11/terrain/draw_77_vertices.inc"
};
static NJS_CNK_MODEL draw_77_model = {
#include "stg11/terrain/draw_77_model.inc"
};
static NJS_CNK_OBJECT draw_197 = {
#include "stg11/terrain/draw_197.inc"
};
static Sint16 draw_75_polygons[215] = {
#include "stg11/terrain/draw_75_polygons.inc"
};
static Sint32 draw_75_vertices[243] = {
#include "stg11/terrain/draw_75_vertices.inc"
};
static NJS_CNK_MODEL draw_75_model = {
#include "stg11/terrain/draw_75_model.inc"
};
static NJS_CNK_OBJECT draw_196 = {
#include "stg11/terrain/draw_196.inc"
};
static Sint16 draw_74_polygons[121] = {
#include "stg11/terrain/draw_74_polygons.inc"
};
static Sint32 draw_74_vertices[115] = {
#include "stg11/terrain/draw_74_vertices.inc"
};
static NJS_CNK_MODEL draw_74_model = {
#include "stg11/terrain/draw_74_model.inc"
};
static NJS_CNK_OBJECT draw_194 = {
#include "stg11/terrain/draw_194.inc"
};
static Sint16 draw_73_polygons[387] = {
#include "stg11/terrain/draw_73_polygons.inc"
};
static Sint32 draw_73_vertices[391] = {
#include "stg11/terrain/draw_73_vertices.inc"
};
static NJS_CNK_MODEL draw_73_model = {
#include "stg11/terrain/draw_73_model.inc"
};
static NJS_CNK_OBJECT draw_193 = {
#include "stg11/terrain/draw_193.inc"
};
static Sint16 draw_72_polygons[431] = {
#include "stg11/terrain/draw_72_polygons.inc"
};
static Sint32 draw_72_vertices[323] = {
#include "stg11/terrain/draw_72_vertices.inc"
};
static NJS_CNK_MODEL draw_72_model = {
#include "stg11/terrain/draw_72_model.inc"
};
static NJS_CNK_OBJECT draw_192 = {
#include "stg11/terrain/draw_192.inc"
};
static Sint16 draw_68_polygons[215] = {
#include "stg11/terrain/draw_68_polygons.inc"
};
static Sint32 draw_68_vertices[243] = {
#include "stg11/terrain/draw_68_vertices.inc"
};
static NJS_CNK_MODEL draw_68_model = {
#include "stg11/terrain/draw_68_model.inc"
};
static NJS_CNK_OBJECT draw_191 = {
#include "stg11/terrain/draw_191.inc"
};
static Sint16 draw_66_polygons[121] = {
#include "stg11/terrain/draw_66_polygons.inc"
};
static Sint32 draw_66_vertices[115] = {
#include "stg11/terrain/draw_66_vertices.inc"
};
static NJS_CNK_MODEL draw_66_model = {
#include "stg11/terrain/draw_66_model.inc"
};
static NJS_CNK_OBJECT draw_187 = {
#include "stg11/terrain/draw_187.inc"
};
static Sint16 draw_63_polygons[389] = {
#include "stg11/terrain/draw_63_polygons.inc"
};
static Sint32 draw_63_vertices[415] = {
#include "stg11/terrain/draw_63_vertices.inc"
};
static NJS_CNK_MODEL draw_63_model = {
#include "stg11/terrain/draw_63_model.inc"
};
static NJS_CNK_OBJECT draw_185 = {
#include "stg11/terrain/draw_185.inc"
};
static Sint16 draw_62_polygons[121] = {
#include "stg11/terrain/draw_62_polygons.inc"
};
static Sint32 draw_62_vertices[115] = {
#include "stg11/terrain/draw_62_vertices.inc"
};
static NJS_CNK_MODEL draw_62_model = {
#include "stg11/terrain/draw_62_model.inc"
};
static NJS_CNK_OBJECT draw_182 = {
#include "stg11/terrain/draw_182.inc"
};
static Sint16 draw_61_polygons[385] = {
#include "stg11/terrain/draw_61_polygons.inc"
};
static Sint32 draw_61_vertices[259] = {
#include "stg11/terrain/draw_61_vertices.inc"
};
static NJS_CNK_MODEL draw_61_model = {
#include "stg11/terrain/draw_61_model.inc"
};
static NJS_CNK_OBJECT draw_181 = {
#include "stg11/terrain/draw_181.inc"
};
static Sint16 draw_60_polygons[167] = {
#include "stg11/terrain/draw_60_polygons.inc"
};
static Sint32 draw_60_vertices[107] = {
#include "stg11/terrain/draw_60_vertices.inc"
};
static NJS_CNK_MODEL draw_60_model = {
#include "stg11/terrain/draw_60_model.inc"
};
static NJS_CNK_OBJECT draw_180 = {
#include "stg11/terrain/draw_180.inc"
};
static Sint16 draw_58_polygons[431] = {
#include "stg11/terrain/draw_58_polygons.inc"
};
static Sint32 draw_58_vertices[323] = {
#include "stg11/terrain/draw_58_vertices.inc"
};
static NJS_CNK_MODEL draw_58_model = {
#include "stg11/terrain/draw_58_model.inc"
};
static NJS_CNK_OBJECT draw_179 = {
#include "stg11/terrain/draw_179.inc"
};
static Sint16 draw_57_polygons[189] = {
#include "stg11/terrain/draw_57_polygons.inc"
};
static Sint32 draw_57_vertices[147] = {
#include "stg11/terrain/draw_57_vertices.inc"
};
static NJS_CNK_MODEL draw_57_model = {
#include "stg11/terrain/draw_57_model.inc"
};
static NJS_CNK_OBJECT draw_177 = {
#include "stg11/terrain/draw_177.inc"
};
static Sint16 draw_166_polygons[465] = {
#include "stg11/terrain/draw_166_polygons.inc"
};
static Sint32 draw_166_vertices[339] = {
#include "stg11/terrain/draw_166_vertices.inc"
};
static NJS_CNK_MODEL draw_166_model = {
#include "stg11/terrain/draw_166_model.inc"
};
static NJS_CNK_OBJECT draw_166 = {
#include "stg11/terrain/draw_166.inc"
};
static Sint16 draw_165_polygons[699] = {
#include "stg11/terrain/draw_165_polygons.inc"
};
static Sint32 draw_165_vertices[767] = {
#include "stg11/terrain/draw_165_vertices.inc"
};
static NJS_CNK_MODEL draw_165_model = {
#include "stg11/terrain/draw_165_model.inc"
};
static NJS_CNK_OBJECT draw_165 = {
#include "stg11/terrain/draw_165.inc"
};
static Sint16 draw_164_polygons[375] = {
#include "stg11/terrain/draw_164_polygons.inc"
};
static Sint32 draw_164_vertices[387] = {
#include "stg11/terrain/draw_164_vertices.inc"
};
static NJS_CNK_MODEL draw_164_model = {
#include "stg11/terrain/draw_164_model.inc"
};
static NJS_CNK_OBJECT draw_164 = {
#include "stg11/terrain/draw_164.inc"
};
static Sint16 draw_163_polygons[177] = {
#include "stg11/terrain/draw_163_polygons.inc"
};
static Sint32 draw_163_vertices[83] = {
#include "stg11/terrain/draw_163_vertices.inc"
};
static NJS_CNK_MODEL draw_163_model = {
#include "stg11/terrain/draw_163_model.inc"
};
static NJS_CNK_OBJECT draw_163 = {
#include "stg11/terrain/draw_163.inc"
};
static Sint16 draw_159_polygons[133] = {
#include "stg11/terrain/draw_159_polygons.inc"
};
static Sint32 draw_159_vertices[67] = {
#include "stg11/terrain/draw_159_vertices.inc"
};
static NJS_CNK_MODEL draw_159_model = {
#include "stg11/terrain/draw_159_model.inc"
};
static NJS_CNK_OBJECT draw_159 = {
#include "stg11/terrain/draw_159.inc"
};
static Sint16 draw_155_polygons[71] = {
#include "stg11/terrain/draw_155_polygons.inc"
};
static Sint32 draw_155_vertices[51] = {
#include "stg11/terrain/draw_155_vertices.inc"
};
static NJS_CNK_MODEL draw_155_model = {
#include "stg11/terrain/draw_155_model.inc"
};
static NJS_CNK_OBJECT draw_155 = {
#include "stg11/terrain/draw_155.inc"
};
static Sint16 draw_97_polygons[617] = {
#include "stg11/terrain/draw_97_polygons.inc"
};
static Sint32 draw_97_vertices[635] = {
#include "stg11/terrain/draw_97_vertices.inc"
};
static NJS_CNK_MODEL draw_97_model = {
#include "stg11/terrain/draw_97_model.inc"
};
static NJS_CNK_OBJECT draw_144 = {
#include "stg11/terrain/draw_144.inc"
};
static Sint16 draw_8_polygons[359] = {
#include "stg11/terrain/draw_8_polygons.inc"
};
static Sint32 draw_8_vertices[355] = {
#include "stg11/terrain/draw_8_vertices.inc"
};
static NJS_CNK_MODEL draw_8_model = {
#include "stg11/terrain/draw_8_model.inc"
};
static NJS_CNK_OBJECT draw_143 = {
#include "stg11/terrain/draw_143.inc"
};
static Sint16 draw_91_polygons[309] = {
#include "stg11/terrain/draw_91_polygons.inc"
};
static Sint32 draw_91_vertices[323] = {
#include "stg11/terrain/draw_91_vertices.inc"
};
static NJS_CNK_MODEL draw_91_model = {
#include "stg11/terrain/draw_91_model.inc"
};
static NJS_CNK_OBJECT draw_138 = {
#include "stg11/terrain/draw_138.inc"
};
static Sint16 draw_136_polygons[35] = {
#include "stg11/terrain/draw_136_polygons.inc"
};
static Sint32 draw_136_vertices[35] = {
#include "stg11/terrain/draw_136_vertices.inc"
};
static NJS_CNK_MODEL draw_136_model = {
#include "stg11/terrain/draw_136_model.inc"
};
static NJS_CNK_OBJECT draw_136 = {
#include "stg11/terrain/draw_136.inc"
};
static Sint16 draw_85_polygons[129] = {
#include "stg11/terrain/draw_85_polygons.inc"
};
static Sint32 draw_85_vertices[111] = {
#include "stg11/terrain/draw_85_vertices.inc"
};
static NJS_CNK_MODEL draw_85_model = {
#include "stg11/terrain/draw_85_model.inc"
};
static NJS_CNK_OBJECT draw_133 = {
#include "stg11/terrain/draw_133.inc"
};
static Sint16 draw_90_polygons[377] = {
#include "stg11/terrain/draw_90_polygons.inc"
};
static Sint32 draw_90_vertices[399] = {
#include "stg11/terrain/draw_90_vertices.inc"
};
static NJS_CNK_MODEL draw_90_model = {
#include "stg11/terrain/draw_90_model.inc"
};
static NJS_CNK_OBJECT draw_116 = {
#include "stg11/terrain/draw_116.inc"
};
static Sint16 draw_115_polygons[521] = {
#include "stg11/terrain/draw_115_polygons.inc"
};
static Sint32 draw_115_vertices[279] = {
#include "stg11/terrain/draw_115_vertices.inc"
};
static NJS_CNK_MODEL draw_115_model = {
#include "stg11/terrain/draw_115_model.inc"
};
static NJS_CNK_OBJECT draw_115 = {
#include "stg11/terrain/draw_115.inc"
};
static Sint16 draw_114_polygons[301] = {
#include "stg11/terrain/draw_114_polygons.inc"
};
static Sint32 draw_114_vertices[223] = {
#include "stg11/terrain/draw_114_vertices.inc"
};
static NJS_CNK_MODEL draw_114_model = {
#include "stg11/terrain/draw_114_model.inc"
};
static NJS_CNK_OBJECT draw_114 = {
#include "stg11/terrain/draw_114.inc"
};
static Sint16 draw_113_polygons[105] = {
#include "stg11/terrain/draw_113_polygons.inc"
};
static Sint32 draw_113_vertices[59] = {
#include "stg11/terrain/draw_113_vertices.inc"
};
static NJS_CNK_MODEL draw_113_model = {
#include "stg11/terrain/draw_113_model.inc"
};
static NJS_CNK_OBJECT draw_113 = {
#include "stg11/terrain/draw_113.inc"
};
static Sint16 draw_112_polygons[275] = {
#include "stg11/terrain/draw_112_polygons.inc"
};
static Sint32 draw_112_vertices[187] = {
#include "stg11/terrain/draw_112_vertices.inc"
};
static NJS_CNK_MODEL draw_112_model = {
#include "stg11/terrain/draw_112_model.inc"
};
static NJS_CNK_OBJECT draw_112 = {
#include "stg11/terrain/draw_112.inc"
};
static Sint16 draw_111_polygons[49] = {
#include "stg11/terrain/draw_111_polygons.inc"
};
static Sint32 draw_111_vertices[35] = {
#include "stg11/terrain/draw_111_vertices.inc"
};
static NJS_CNK_MODEL draw_111_model = {
#include "stg11/terrain/draw_111_model.inc"
};
static NJS_CNK_OBJECT draw_111 = {
#include "stg11/terrain/draw_111.inc"
};
static Sint16 draw_110_polygons[319] = {
#include "stg11/terrain/draw_110_polygons.inc"
};
static Sint32 draw_110_vertices[251] = {
#include "stg11/terrain/draw_110_vertices.inc"
};
static NJS_CNK_MODEL draw_110_model = {
#include "stg11/terrain/draw_110_model.inc"
};
static NJS_CNK_OBJECT draw_110 = {
#include "stg11/terrain/draw_110.inc"
};
static Sint16 draw_109_polygons[29] = {
#include "stg11/terrain/draw_109_polygons.inc"
};
static Sint32 draw_109_vertices[27] = {
#include "stg11/terrain/draw_109_vertices.inc"
};
static NJS_CNK_MODEL draw_109_model = {
#include "stg11/terrain/draw_109_model.inc"
};
static NJS_CNK_OBJECT draw_109 = {
#include "stg11/terrain/draw_109.inc"
};
static Sint16 draw_86_polygons[205] = {
#include "stg11/terrain/draw_86_polygons.inc"
};
static Sint32 draw_86_vertices[175] = {
#include "stg11/terrain/draw_86_vertices.inc"
};
static NJS_CNK_MODEL draw_86_model = {
#include "stg11/terrain/draw_86_model.inc"
};
static NJS_CNK_OBJECT draw_108 = {
#include "stg11/terrain/draw_108.inc"
};
static Sint16 draw_105_polygons[501] = {
#include "stg11/terrain/draw_105_polygons.inc"
};
static Sint32 draw_105_vertices[235] = {
#include "stg11/terrain/draw_105_vertices.inc"
};
static NJS_CNK_MODEL draw_105_model = {
#include "stg11/terrain/draw_105_model.inc"
};
static NJS_CNK_OBJECT draw_105 = {
#include "stg11/terrain/draw_105.inc"
};
static Sint16 draw_104_polygons[403] = {
#include "stg11/terrain/draw_104_polygons.inc"
};
static Sint32 draw_104_vertices[447] = {
#include "stg11/terrain/draw_104_vertices.inc"
};
static NJS_CNK_MODEL draw_104_model = {
#include "stg11/terrain/draw_104_model.inc"
};
static NJS_CNK_OBJECT draw_104 = {
#include "stg11/terrain/draw_104.inc"
};
static Sint16 draw_84_polygons[189] = {
#include "stg11/terrain/draw_84_polygons.inc"
};
static Sint32 draw_84_vertices[67] = {
#include "stg11/terrain/draw_84_vertices.inc"
};
static NJS_CNK_MODEL draw_84_model = {
#include "stg11/terrain/draw_84_model.inc"
};
static NJS_CNK_OBJECT draw_103 = {
#include "stg11/terrain/draw_103.inc"
};
static Sint16 draw_102_polygons[527] = {
#include "stg11/terrain/draw_102_polygons.inc"
};
static Sint32 draw_102_vertices[519] = {
#include "stg11/terrain/draw_102_vertices.inc"
};
static NJS_CNK_MODEL draw_102_model = {
#include "stg11/terrain/draw_102_model.inc"
};
static NJS_CNK_OBJECT draw_102 = {
#include "stg11/terrain/draw_102.inc"
};
static Sint16 draw_101_polygons[145] = {
#include "stg11/terrain/draw_101_polygons.inc"
};
static Sint32 draw_101_vertices[67] = {
#include "stg11/terrain/draw_101_vertices.inc"
};
static NJS_CNK_MODEL draw_101_model = {
#include "stg11/terrain/draw_101_model.inc"
};
static NJS_CNK_OBJECT draw_101 = {
#include "stg11/terrain/draw_101.inc"
};
static Sint16 draw_100_polygons[355] = {
#include "stg11/terrain/draw_100_polygons.inc"
};
static Sint32 draw_100_vertices[407] = {
#include "stg11/terrain/draw_100_vertices.inc"
};
static NJS_CNK_MODEL draw_100_model = {
#include "stg11/terrain/draw_100_model.inc"
};
static NJS_CNK_OBJECT draw_100 = {
#include "stg11/terrain/draw_100.inc"
};
static Sint16 draw_99_polygons[407] = {
#include "stg11/terrain/draw_99_polygons.inc"
};
static Sint32 draw_99_vertices[243] = {
#include "stg11/terrain/draw_99_vertices.inc"
};
static NJS_CNK_MODEL draw_99_model = {
#include "stg11/terrain/draw_99_model.inc"
};
static NJS_CNK_OBJECT draw_99 = {
#include "stg11/terrain/draw_99.inc"
};
static Sint16 draw_98_polygons[221] = {
#include "stg11/terrain/draw_98_polygons.inc"
};
static Sint32 draw_98_vertices[163] = {
#include "stg11/terrain/draw_98_vertices.inc"
};
static NJS_CNK_MODEL draw_98_model = {
#include "stg11/terrain/draw_98_model.inc"
};
static NJS_CNK_OBJECT draw_98 = {
#include "stg11/terrain/draw_98.inc"
};
static Sint16 draw_89_polygons[49] = {
#include "stg11/terrain/draw_89_polygons.inc"
};
static Sint32 draw_89_vertices[35] = {
#include "stg11/terrain/draw_89_vertices.inc"
};
static NJS_CNK_MODEL draw_89_model = {
#include "stg11/terrain/draw_89_model.inc"
};
static NJS_CNK_OBJECT draw_89 = {
#include "stg11/terrain/draw_89.inc"
};
static Sint16 draw_88_polygons[29] = {
#include "stg11/terrain/draw_88_polygons.inc"
};
static Sint32 draw_88_vertices[27] = {
#include "stg11/terrain/draw_88_vertices.inc"
};
static NJS_CNK_MODEL draw_88_model = {
#include "stg11/terrain/draw_88_model.inc"
};
static NJS_CNK_OBJECT draw_88 = {
#include "stg11/terrain/draw_88.inc"
};
static Sint16 draw_38_polygons[329] = {
#include "stg11/terrain/draw_38_polygons.inc"
};
static Sint32 draw_38_vertices[303] = {
#include "stg11/terrain/draw_38_vertices.inc"
};
static NJS_CNK_MODEL draw_38_model = {
#include "stg11/terrain/draw_38_model.inc"
};
static NJS_CNK_OBJECT draw_38 = {
#include "stg11/terrain/draw_38.inc"
};
static Sint16 draw_37_polygons[359] = {
#include "stg11/terrain/draw_37_polygons.inc"
};
static Sint32 draw_37_vertices[359] = {
#include "stg11/terrain/draw_37_vertices.inc"
};
static NJS_CNK_MODEL draw_37_model = {
#include "stg11/terrain/draw_37_model.inc"
};
static NJS_CNK_OBJECT draw_37 = {
#include "stg11/terrain/draw_37.inc"
};
static Sint16 draw_36_polygons[129] = {
#include "stg11/terrain/draw_36_polygons.inc"
};
static Sint32 draw_36_vertices[95] = {
#include "stg11/terrain/draw_36_vertices.inc"
};
static NJS_CNK_MODEL draw_36_model = {
#include "stg11/terrain/draw_36_model.inc"
};
static NJS_CNK_OBJECT draw_36 = {
#include "stg11/terrain/draw_36.inc"
};
static Sint16 draw_35_polygons[375] = {
#include "stg11/terrain/draw_35_polygons.inc"
};
static Sint32 draw_35_vertices[395] = {
#include "stg11/terrain/draw_35_vertices.inc"
};
static NJS_CNK_MODEL draw_35_model = {
#include "stg11/terrain/draw_35_model.inc"
};
static NJS_CNK_OBJECT draw_35 = {
#include "stg11/terrain/draw_35.inc"
};
static Sint16 draw_34_polygons[289] = {
#include "stg11/terrain/draw_34_polygons.inc"
};
static Sint32 draw_34_vertices[223] = {
#include "stg11/terrain/draw_34_vertices.inc"
};
static NJS_CNK_MODEL draw_34_model = {
#include "stg11/terrain/draw_34_model.inc"
};
static NJS_CNK_OBJECT draw_34 = {
#include "stg11/terrain/draw_34.inc"
};
static Sint16 draw_33_polygons[425] = {
#include "stg11/terrain/draw_33_polygons.inc"
};
static Sint32 draw_33_vertices[491] = {
#include "stg11/terrain/draw_33_vertices.inc"
};
static NJS_CNK_MODEL draw_33_model = {
#include "stg11/terrain/draw_33_model.inc"
};
static NJS_CNK_OBJECT draw_33 = {
#include "stg11/terrain/draw_33.inc"
};
static Sint16 draw_32_polygons[163] = {
#include "stg11/terrain/draw_32_polygons.inc"
};
static Sint32 draw_32_vertices[87] = {
#include "stg11/terrain/draw_32_vertices.inc"
};
static NJS_CNK_MODEL draw_32_model = {
#include "stg11/terrain/draw_32_model.inc"
};
static NJS_CNK_OBJECT draw_32 = {
#include "stg11/terrain/draw_32.inc"
};
static Sint16 draw_30_polygons[153] = {
#include "stg11/terrain/draw_30_polygons.inc"
};
static Sint32 draw_30_vertices[131] = {
#include "stg11/terrain/draw_30_vertices.inc"
};
static NJS_CNK_MODEL draw_30_model = {
#include "stg11/terrain/draw_30_model.inc"
};
static NJS_CNK_OBJECT draw_30 = {
#include "stg11/terrain/draw_30.inc"
};
static Sint16 draw_26_polygons[205] = {
#include "stg11/terrain/draw_26_polygons.inc"
};
static Sint32 draw_26_vertices[151] = {
#include "stg11/terrain/draw_26_vertices.inc"
};
static NJS_CNK_MODEL draw_26_model = {
#include "stg11/terrain/draw_26_model.inc"
};
static NJS_CNK_OBJECT draw_26 = {
#include "stg11/terrain/draw_26.inc"
};
static Sint16 draw_25_polygons[205] = {
#include "stg11/terrain/draw_25_polygons.inc"
};
static Sint32 draw_25_vertices[211] = {
#include "stg11/terrain/draw_25_vertices.inc"
};
static NJS_CNK_MODEL draw_25_model = {
#include "stg11/terrain/draw_25_model.inc"
};
static NJS_CNK_OBJECT draw_25 = {
#include "stg11/terrain/draw_25.inc"
};
static Sint16 draw_24_polygons[175] = {
#include "stg11/terrain/draw_24_polygons.inc"
};
static Sint32 draw_24_vertices[179] = {
#include "stg11/terrain/draw_24_vertices.inc"
};
static NJS_CNK_MODEL draw_24_model = {
#include "stg11/terrain/draw_24_model.inc"
};
static NJS_CNK_OBJECT draw_24 = {
#include "stg11/terrain/draw_24.inc"
};
static Sint16 draw_23_polygons[743] = {
#include "stg11/terrain/draw_23_polygons.inc"
};
static Sint32 draw_23_vertices[395] = {
#include "stg11/terrain/draw_23_vertices.inc"
};
static NJS_CNK_MODEL draw_23_model = {
#include "stg11/terrain/draw_23_model.inc"
};
static NJS_CNK_OBJECT draw_23 = {
#include "stg11/terrain/draw_23.inc"
};
static Sint16 draw_22_polygons[95] = {
#include "stg11/terrain/draw_22_polygons.inc"
};
static Sint32 draw_22_vertices[75] = {
#include "stg11/terrain/draw_22_vertices.inc"
};
static NJS_CNK_MODEL draw_22_model = {
#include "stg11/terrain/draw_22_model.inc"
};
static NJS_CNK_OBJECT draw_22 = {
#include "stg11/terrain/draw_22.inc"
};
static Sint16 draw_5_polygons[137] = {
#include "stg11/terrain/draw_5_polygons.inc"
};
static Sint32 draw_5_vertices[91] = {
#include "stg11/terrain/draw_5_vertices.inc"
};
static NJS_CNK_MODEL draw_5_model = {
#include "stg11/terrain/draw_5_model.inc"
};
static NJS_CNK_OBJECT draw_5 = {
#include "stg11/terrain/draw_5.inc"
};
static Sint16 draw_4_polygons[385] = {
#include "stg11/terrain/draw_4_polygons.inc"
};
static Sint32 draw_4_vertices[419] = {
#include "stg11/terrain/draw_4_vertices.inc"
};
static NJS_CNK_MODEL draw_4_model = {
#include "stg11/terrain/draw_4_model.inc"
};
static NJS_CNK_OBJECT draw_4 = {
#include "stg11/terrain/draw_4.inc"
};
static Sint16 draw_3_polygons[287] = {
#include "stg11/terrain/draw_3_polygons.inc"
};
static Sint32 draw_3_vertices[307] = {
#include "stg11/terrain/draw_3_vertices.inc"
};
static NJS_CNK_MODEL draw_3_model = {
#include "stg11/terrain/draw_3_model.inc"
};
static NJS_CNK_OBJECT draw_3 = {
#include "stg11/terrain/draw_3.inc"
};
static Sint16 draw_2_polygons[503] = {
#include "stg11/terrain/draw_2_polygons.inc"
};
static Sint32 draw_2_vertices[347] = {
#include "stg11/terrain/draw_2_vertices.inc"
};
static NJS_CNK_MODEL draw_2_model = {
#include "stg11/terrain/draw_2_model.inc"
};
static NJS_CNK_OBJECT draw_2 = {
#include "stg11/terrain/draw_2.inc"
};
static Sint16 draw_0_polygons[23] = {
#include "stg11/terrain/draw_0_polygons.inc"
};
static Sint32 draw_0_vertices[19] = {
#include "stg11/terrain/draw_0_vertices.inc"
};
static NJS_CNK_MODEL draw_0_model = {
#include "stg11/terrain/draw_0_model.inc"
};
static NJS_CNK_OBJECT draw_1 = {
#include "stg11/terrain/draw_1.inc"
};
static NJS_OBJECT collision_7 = {
#include "stg11/terrain/collision_7.inc"
};
static NJS_OBJECT collision_17 = {
#include "stg11/terrain/collision_17.inc"
};
static NJS_OBJECT collision_19 = {
#include "stg11/terrain/collision_19.inc"
};
static NJS_OBJECT collision_21 = {
#include "stg11/terrain/collision_21.inc"
};
static NJS_OBJECT collision_25 = {
#include "stg11/terrain/collision_25.inc"
};
static NJS_OBJECT collision_27 = {
#include "stg11/terrain/collision_27.inc"
};
static NJS_OBJECT collision_28 = {
#include "stg11/terrain/collision_28.inc"
};
static NJS_OBJECT collision_30 = {
#include "stg11/terrain/collision_30.inc"
};
static NJS_OBJECT collision_32 = {
#include "stg11/terrain/collision_32.inc"
};
static NJS_OBJECT collision_33 = {
#include "stg11/terrain/collision_33.inc"
};
static NJS_OBJECT collision_34 = {
#include "stg11/terrain/collision_34.inc"
};
static NJS_OBJECT collision_36 = {
#include "stg11/terrain/collision_36.inc"
};
static NJS_OBJECT collision_37 = {
#include "stg11/terrain/collision_37.inc"
};
static NJS_OBJECT collision_38 = {
#include "stg11/terrain/collision_38.inc"
};
static NJS_OBJECT collision_43 = {
#include "stg11/terrain/collision_43.inc"
};
static NJS_OBJECT collision_44 = {
#include "stg11/terrain/collision_44.inc"
};
static NJS_OBJECT collision_45 = {
#include "stg11/terrain/collision_45.inc"
};
static NJS_OBJECT collision_47 = {
#include "stg11/terrain/collision_47.inc"
};
static NJS_OBJECT collision_48 = {
#include "stg11/terrain/collision_48.inc"
};
static NJS_OBJECT collision_49 = {
#include "stg11/terrain/collision_49.inc"
};
static NJS_OBJECT collision_51 = {
#include "stg11/terrain/collision_51.inc"
};
static NJS_OBJECT collision_52 = {
#include "stg11/terrain/collision_52.inc"
};
static NJS_OBJECT collision_53 = {
#include "stg11/terrain/collision_53.inc"
};
static NJS_OBJECT collision_54 = {
#include "stg11/terrain/collision_54.inc"
};
static NJS_OBJECT collision_55 = {
#include "stg11/terrain/collision_55.inc"
};
static NJS_OBJECT collision_61 = {
#include "stg11/terrain/collision_61.inc"
};
static NJS_OBJECT collision_62 = {
#include "stg11/terrain/collision_62.inc"
};
static NJS_OBJECT collision_65 = {
#include "stg11/terrain/collision_65.inc"
};
static NJS_OBJECT collision_67 = {
#include "stg11/terrain/collision_67.inc"
};
static NJS_OBJECT collision_77 = {
#include "stg11/terrain/collision_77.inc"
};
static NJS_OBJECT collision_78 = {
#include "stg11/terrain/collision_78.inc"
};
static NJS_OBJECT collision_82 = {
#include "stg11/terrain/collision_82.inc"
};
static NJS_OBJECT collision_83 = {
#include "stg11/terrain/collision_83.inc"
};
static NJS_OBJECT collision_84 = {
#include "stg11/terrain/collision_84.inc"
};
static NJS_OBJECT collision_86 = {
#include "stg11/terrain/collision_86.inc"
};
static NJS_OBJECT collision_87 = {
#include "stg11/terrain/collision_87.inc"
};
static NJS_OBJECT collision_89 = {
#include "stg11/terrain/collision_89.inc"
};
static NJS_OBJECT collision_90 = {
#include "stg11/terrain/collision_90.inc"
};
static NJS_OBJECT collision_91 = {
#include "stg11/terrain/collision_91.inc"
};
static NJS_OBJECT collision_92 = {
#include "stg11/terrain/collision_92.inc"
};
static NJS_OBJECT collision_93 = {
#include "stg11/terrain/collision_93.inc"
};
static NJS_OBJECT collision_94 = {
#include "stg11/terrain/collision_94.inc"
};
static NJS_OBJECT collision_95 = {
#include "stg11/terrain/collision_95.inc"
};
static NJS_OBJECT collision_96 = {
#include "stg11/terrain/collision_96.inc"
};
static NJS_OBJECT collision_97 = {
#include "stg11/terrain/collision_97.inc"
};
static NJS_OBJECT collision_98 = {
#include "stg11/terrain/collision_98.inc"
};
static NJS_OBJECT collision_100 = {
#include "stg11/terrain/collision_100.inc"
};
static NJS_OBJECT collision_104 = {
#include "stg11/terrain/collision_104.inc"
};
static NJS_OBJECT collision_105 = {
#include "stg11/terrain/collision_105.inc"
};
static NJS_OBJECT collision_106 = {
#include "stg11/terrain/collision_106.inc"
};
static NJS_OBJECT collision_107 = {
#include "stg11/terrain/collision_107.inc"
};
static NJS_OBJECT collision_108 = {
#include "stg11/terrain/collision_108.inc"
};
static NJS_OBJECT collision_109 = {
#include "stg11/terrain/collision_109.inc"
};
static NJS_CNK_OBJECT draw_257 = {
#include "stg11/terrain/draw_257.inc"
};
static NJS_CNK_OBJECT draw_254 = {
#include "stg11/terrain/draw_254.inc"
};
static NJS_CNK_OBJECT draw_253 = {
#include "stg11/terrain/draw_253.inc"
};
static NJS_CNK_OBJECT draw_251 = {
#include "stg11/terrain/draw_251.inc"
};
static NJS_CNK_OBJECT draw_250 = {
#include "stg11/terrain/draw_250.inc"
};
static NJS_CNK_OBJECT draw_248 = {
#include "stg11/terrain/draw_248.inc"
};
static NJS_CNK_OBJECT draw_247 = {
#include "stg11/terrain/draw_247.inc"
};
static NJS_CNK_OBJECT draw_243 = {
#include "stg11/terrain/draw_243.inc"
};
static NJS_CNK_OBJECT draw_241 = {
#include "stg11/terrain/draw_241.inc"
};
static NJS_CNK_OBJECT draw_240 = {
#include "stg11/terrain/draw_240.inc"
};
static NJS_CNK_OBJECT draw_238 = {
#include "stg11/terrain/draw_238.inc"
};
static NJS_CNK_OBJECT draw_237 = {
#include "stg11/terrain/draw_237.inc"
};
static NJS_CNK_OBJECT draw_236 = {
#include "stg11/terrain/draw_236.inc"
};
static NJS_CNK_OBJECT draw_235 = {
#include "stg11/terrain/draw_235.inc"
};
static NJS_CNK_OBJECT draw_234 = {
#include "stg11/terrain/draw_234.inc"
};
static NJS_CNK_OBJECT draw_232 = {
#include "stg11/terrain/draw_232.inc"
};
static NJS_CNK_OBJECT draw_230 = {
#include "stg11/terrain/draw_230.inc"
};
static NJS_CNK_OBJECT draw_228 = {
#include "stg11/terrain/draw_228.inc"
};
static NJS_CNK_OBJECT draw_225 = {
#include "stg11/terrain/draw_225.inc"
};
static NJS_CNK_OBJECT draw_223 = {
#include "stg11/terrain/draw_223.inc"
};
static NJS_CNK_OBJECT draw_221 = {
#include "stg11/terrain/draw_221.inc"
};
static NJS_CNK_OBJECT draw_219 = {
#include "stg11/terrain/draw_219.inc"
};
static NJS_CNK_OBJECT draw_217 = {
#include "stg11/terrain/draw_217.inc"
};
static NJS_CNK_OBJECT draw_215 = {
#include "stg11/terrain/draw_215.inc"
};
static NJS_CNK_OBJECT draw_213 = {
#include "stg11/terrain/draw_213.inc"
};
static NJS_CNK_OBJECT draw_212 = {
#include "stg11/terrain/draw_212.inc"
};
static NJS_CNK_OBJECT draw_211 = {
#include "stg11/terrain/draw_211.inc"
};
static NJS_CNK_OBJECT draw_210 = {
#include "stg11/terrain/draw_210.inc"
};
static NJS_CNK_OBJECT draw_209 = {
#include "stg11/terrain/draw_209.inc"
};
static NJS_CNK_OBJECT draw_206 = {
#include "stg11/terrain/draw_206.inc"
};
static NJS_CNK_OBJECT draw_205 = {
#include "stg11/terrain/draw_205.inc"
};
static NJS_CNK_OBJECT draw_204 = {
#include "stg11/terrain/draw_204.inc"
};
static NJS_CNK_OBJECT draw_203 = {
#include "stg11/terrain/draw_203.inc"
};
static NJS_CNK_OBJECT draw_202 = {
#include "stg11/terrain/draw_202.inc"
};
static NJS_CNK_OBJECT draw_201 = {
#include "stg11/terrain/draw_201.inc"
};
static NJS_CNK_OBJECT draw_195 = {
#include "stg11/terrain/draw_195.inc"
};
static NJS_CNK_OBJECT draw_190 = {
#include "stg11/terrain/draw_190.inc"
};
static NJS_CNK_OBJECT draw_189 = {
#include "stg11/terrain/draw_189.inc"
};
static NJS_CNK_OBJECT draw_188 = {
#include "stg11/terrain/draw_188.inc"
};
static NJS_CNK_OBJECT draw_186 = {
#include "stg11/terrain/draw_186.inc"
};
static NJS_CNK_OBJECT draw_184 = {
#include "stg11/terrain/draw_184.inc"
};
static NJS_CNK_OBJECT draw_183 = {
#include "stg11/terrain/draw_183.inc"
};
static NJS_CNK_OBJECT draw_178 = {
#include "stg11/terrain/draw_178.inc"
};
static NJS_CNK_OBJECT draw_176 = {
#include "stg11/terrain/draw_176.inc"
};
static NJS_CNK_OBJECT draw_175 = {
#include "stg11/terrain/draw_175.inc"
};
static NJS_CNK_OBJECT draw_174 = {
#include "stg11/terrain/draw_174.inc"
};
static NJS_CNK_OBJECT draw_173 = {
#include "stg11/terrain/draw_173.inc"
};
static NJS_CNK_OBJECT draw_172 = {
#include "stg11/terrain/draw_172.inc"
};
static NJS_CNK_OBJECT draw_171 = {
#include "stg11/terrain/draw_171.inc"
};
static NJS_CNK_OBJECT draw_170 = {
#include "stg11/terrain/draw_170.inc"
};
static NJS_CNK_OBJECT draw_169 = {
#include "stg11/terrain/draw_169.inc"
};
static NJS_CNK_OBJECT draw_168 = {
#include "stg11/terrain/draw_168.inc"
};
static NJS_CNK_OBJECT draw_167 = {
#include "stg11/terrain/draw_167.inc"
};
static NJS_CNK_OBJECT draw_162 = {
#include "stg11/terrain/draw_162.inc"
};
static NJS_CNK_OBJECT draw_161 = {
#include "stg11/terrain/draw_161.inc"
};
static NJS_CNK_OBJECT draw_160 = {
#include "stg11/terrain/draw_160.inc"
};
static NJS_CNK_OBJECT draw_158 = {
#include "stg11/terrain/draw_158.inc"
};
static NJS_CNK_OBJECT draw_157 = {
#include "stg11/terrain/draw_157.inc"
};
static NJS_CNK_OBJECT draw_156 = {
#include "stg11/terrain/draw_156.inc"
};
static NJS_CNK_OBJECT draw_154 = {
#include "stg11/terrain/draw_154.inc"
};
static NJS_CNK_OBJECT draw_153 = {
#include "stg11/terrain/draw_153.inc"
};
static NJS_CNK_OBJECT draw_152 = {
#include "stg11/terrain/draw_152.inc"
};
static NJS_CNK_OBJECT draw_151 = {
#include "stg11/terrain/draw_151.inc"
};
static NJS_CNK_OBJECT draw_150 = {
#include "stg11/terrain/draw_150.inc"
};
static NJS_CNK_OBJECT draw_149 = {
#include "stg11/terrain/draw_149.inc"
};
static NJS_CNK_OBJECT draw_148 = {
#include "stg11/terrain/draw_148.inc"
};
static NJS_CNK_OBJECT draw_147 = {
#include "stg11/terrain/draw_147.inc"
};
static NJS_CNK_OBJECT draw_146 = {
#include "stg11/terrain/draw_146.inc"
};
static NJS_CNK_OBJECT draw_145 = {
#include "stg11/terrain/draw_145.inc"
};
static NJS_CNK_OBJECT draw_142 = {
#include "stg11/terrain/draw_142.inc"
};
static NJS_CNK_OBJECT draw_141 = {
#include "stg11/terrain/draw_141.inc"
};
static NJS_CNK_OBJECT draw_140 = {
#include "stg11/terrain/draw_140.inc"
};
static NJS_CNK_OBJECT draw_139 = {
#include "stg11/terrain/draw_139.inc"
};
static NJS_CNK_OBJECT draw_137 = {
#include "stg11/terrain/draw_137.inc"
};
static NJS_CNK_OBJECT draw_135 = {
#include "stg11/terrain/draw_135.inc"
};
static NJS_CNK_OBJECT draw_134 = {
#include "stg11/terrain/draw_134.inc"
};
static NJS_CNK_OBJECT draw_132 = {
#include "stg11/terrain/draw_132.inc"
};
static NJS_CNK_OBJECT draw_131 = {
#include "stg11/terrain/draw_131.inc"
};
static NJS_CNK_OBJECT draw_130 = {
#include "stg11/terrain/draw_130.inc"
};
static NJS_CNK_OBJECT draw_129 = {
#include "stg11/terrain/draw_129.inc"
};
static NJS_CNK_OBJECT draw_128 = {
#include "stg11/terrain/draw_128.inc"
};
static NJS_CNK_OBJECT draw_127 = {
#include "stg11/terrain/draw_127.inc"
};
static NJS_CNK_OBJECT draw_126 = {
#include "stg11/terrain/draw_126.inc"
};
static NJS_CNK_OBJECT draw_125 = {
#include "stg11/terrain/draw_125.inc"
};
static NJS_CNK_OBJECT draw_124 = {
#include "stg11/terrain/draw_124.inc"
};
static NJS_CNK_OBJECT draw_123 = {
#include "stg11/terrain/draw_123.inc"
};
static NJS_CNK_OBJECT draw_122 = {
#include "stg11/terrain/draw_122.inc"
};
static NJS_CNK_OBJECT draw_121 = {
#include "stg11/terrain/draw_121.inc"
};
static NJS_CNK_OBJECT draw_120 = {
#include "stg11/terrain/draw_120.inc"
};
static NJS_CNK_OBJECT draw_119 = {
#include "stg11/terrain/draw_119.inc"
};
static NJS_CNK_OBJECT draw_118 = {
#include "stg11/terrain/draw_118.inc"
};
static NJS_CNK_OBJECT draw_117 = {
#include "stg11/terrain/draw_117.inc"
};
static NJS_CNK_OBJECT draw_107 = {
#include "stg11/terrain/draw_107.inc"
};
static NJS_CNK_OBJECT draw_106 = {
#include "stg11/terrain/draw_106.inc"
};
static NJS_CNK_OBJECT draw_97 = {
#include "stg11/terrain/draw_97.inc"
};
static NJS_CNK_OBJECT draw_96 = {
#include "stg11/terrain/draw_96.inc"
};
static NJS_CNK_OBJECT draw_95 = {
#include "stg11/terrain/draw_95.inc"
};
static NJS_CNK_OBJECT draw_94 = {
#include "stg11/terrain/draw_94.inc"
};
static NJS_CNK_OBJECT draw_93 = {
#include "stg11/terrain/draw_93.inc"
};
static NJS_CNK_OBJECT draw_92 = {
#include "stg11/terrain/draw_92.inc"
};
static NJS_CNK_OBJECT draw_91 = {
#include "stg11/terrain/draw_91.inc"
};
static NJS_CNK_OBJECT draw_90 = {
#include "stg11/terrain/draw_90.inc"
};
static NJS_CNK_OBJECT draw_87 = {
#include "stg11/terrain/draw_87.inc"
};
static NJS_CNK_OBJECT draw_86 = {
#include "stg11/terrain/draw_86.inc"
};
static NJS_CNK_OBJECT draw_85 = {
#include "stg11/terrain/draw_85.inc"
};
static NJS_CNK_OBJECT draw_84 = {
#include "stg11/terrain/draw_84.inc"
};
static NJS_CNK_OBJECT draw_83 = {
#include "stg11/terrain/draw_83.inc"
};
static NJS_CNK_OBJECT draw_82 = {
#include "stg11/terrain/draw_82.inc"
};
static NJS_CNK_OBJECT draw_81 = {
#include "stg11/terrain/draw_81.inc"
};
static NJS_CNK_OBJECT draw_80 = {
#include "stg11/terrain/draw_80.inc"
};
static NJS_CNK_OBJECT draw_79 = {
#include "stg11/terrain/draw_79.inc"
};
static NJS_CNK_OBJECT draw_78 = {
#include "stg11/terrain/draw_78.inc"
};
static NJS_CNK_OBJECT draw_77 = {
#include "stg11/terrain/draw_77.inc"
};
static NJS_CNK_OBJECT draw_76 = {
#include "stg11/terrain/draw_76.inc"
};
static NJS_CNK_OBJECT draw_75 = {
#include "stg11/terrain/draw_75.inc"
};
static NJS_CNK_OBJECT draw_74 = {
#include "stg11/terrain/draw_74.inc"
};
static NJS_CNK_OBJECT draw_73 = {
#include "stg11/terrain/draw_73.inc"
};
static NJS_CNK_OBJECT draw_72 = {
#include "stg11/terrain/draw_72.inc"
};
static NJS_CNK_OBJECT draw_71 = {
#include "stg11/terrain/draw_71.inc"
};
static NJS_CNK_OBJECT draw_70 = {
#include "stg11/terrain/draw_70.inc"
};
static NJS_CNK_OBJECT draw_69 = {
#include "stg11/terrain/draw_69.inc"
};
static NJS_CNK_OBJECT draw_68 = {
#include "stg11/terrain/draw_68.inc"
};
static NJS_CNK_OBJECT draw_67 = {
#include "stg11/terrain/draw_67.inc"
};
static NJS_CNK_OBJECT draw_66 = {
#include "stg11/terrain/draw_66.inc"
};
static NJS_CNK_OBJECT draw_65 = {
#include "stg11/terrain/draw_65.inc"
};
static NJS_CNK_OBJECT draw_64 = {
#include "stg11/terrain/draw_64.inc"
};
static NJS_CNK_OBJECT draw_63 = {
#include "stg11/terrain/draw_63.inc"
};
static NJS_CNK_OBJECT draw_62 = {
#include "stg11/terrain/draw_62.inc"
};
static NJS_CNK_OBJECT draw_61 = {
#include "stg11/terrain/draw_61.inc"
};
static NJS_CNK_OBJECT draw_60 = {
#include "stg11/terrain/draw_60.inc"
};
static NJS_CNK_OBJECT draw_59 = {
#include "stg11/terrain/draw_59.inc"
};
static NJS_CNK_OBJECT draw_58 = {
#include "stg11/terrain/draw_58.inc"
};
static NJS_CNK_OBJECT draw_57 = {
#include "stg11/terrain/draw_57.inc"
};
static NJS_CNK_OBJECT draw_56 = {
#include "stg11/terrain/draw_56.inc"
};
static NJS_CNK_OBJECT draw_55 = {
#include "stg11/terrain/draw_55.inc"
};
static NJS_CNK_OBJECT draw_54 = {
#include "stg11/terrain/draw_54.inc"
};
static NJS_CNK_OBJECT draw_53 = {
#include "stg11/terrain/draw_53.inc"
};
static NJS_CNK_OBJECT draw_52 = {
#include "stg11/terrain/draw_52.inc"
};
static NJS_CNK_OBJECT draw_51 = {
#include "stg11/terrain/draw_51.inc"
};
static NJS_CNK_OBJECT draw_50 = {
#include "stg11/terrain/draw_50.inc"
};
static NJS_CNK_OBJECT draw_49 = {
#include "stg11/terrain/draw_49.inc"
};
static NJS_CNK_OBJECT draw_48 = {
#include "stg11/terrain/draw_48.inc"
};
static NJS_CNK_OBJECT draw_47 = {
#include "stg11/terrain/draw_47.inc"
};
static NJS_CNK_OBJECT draw_46 = {
#include "stg11/terrain/draw_46.inc"
};
static NJS_CNK_OBJECT draw_45 = {
#include "stg11/terrain/draw_45.inc"
};
static NJS_CNK_OBJECT draw_44 = {
#include "stg11/terrain/draw_44.inc"
};
static NJS_CNK_OBJECT draw_43 = {
#include "stg11/terrain/draw_43.inc"
};
static NJS_CNK_OBJECT draw_42 = {
#include "stg11/terrain/draw_42.inc"
};
static NJS_CNK_OBJECT draw_41 = {
#include "stg11/terrain/draw_41.inc"
};
static NJS_CNK_OBJECT draw_40 = {
#include "stg11/terrain/draw_40.inc"
};
static NJS_CNK_OBJECT draw_39 = {
#include "stg11/terrain/draw_39.inc"
};
static NJS_CNK_OBJECT draw_31 = {
#include "stg11/terrain/draw_31.inc"
};
static NJS_CNK_OBJECT draw_29 = {
#include "stg11/terrain/draw_29.inc"
};
static NJS_CNK_OBJECT draw_28 = {
#include "stg11/terrain/draw_28.inc"
};
static NJS_CNK_OBJECT draw_27 = {
#include "stg11/terrain/draw_27.inc"
};
static NJS_CNK_OBJECT draw_21 = {
#include "stg11/terrain/draw_21.inc"
};
static NJS_CNK_OBJECT draw_20 = {
#include "stg11/terrain/draw_20.inc"
};
static NJS_CNK_OBJECT draw_19 = {
#include "stg11/terrain/draw_19.inc"
};
static NJS_CNK_OBJECT draw_18 = {
#include "stg11/terrain/draw_18.inc"
};
static NJS_CNK_OBJECT draw_17 = {
#include "stg11/terrain/draw_17.inc"
};
static NJS_CNK_OBJECT draw_16 = {
#include "stg11/terrain/draw_16.inc"
};
static NJS_CNK_OBJECT draw_15 = {
#include "stg11/terrain/draw_15.inc"
};
static NJS_CNK_OBJECT draw_14 = {
#include "stg11/terrain/draw_14.inc"
};
static NJS_CNK_OBJECT draw_13 = {
#include "stg11/terrain/draw_13.inc"
};
static NJS_CNK_OBJECT draw_12 = {
#include "stg11/terrain/draw_12.inc"
};
static NJS_CNK_OBJECT draw_11 = {
#include "stg11/terrain/draw_11.inc"
};
static NJS_CNK_OBJECT draw_10 = {
#include "stg11/terrain/draw_10.inc"
};
static NJS_CNK_OBJECT draw_9 = {
#include "stg11/terrain/draw_9.inc"
};
static NJS_CNK_OBJECT draw_8 = {
#include "stg11/terrain/draw_8.inc"
};
static NJS_CNK_OBJECT draw_7 = {
#include "stg11/terrain/draw_7.inc"
};
static NJS_CNK_OBJECT draw_6 = {
#include "stg11/terrain/draw_6.inc"
};
static NJS_CNK_OBJECT draw_0 = {
#include "stg11/terrain/draw_0.inc"
};
