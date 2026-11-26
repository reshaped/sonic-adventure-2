#include <ninja.h>
#include <sg_xpt.h>

#include "collision/world.h"
#include "stg08/background.h"

/* the land's file names, which its land table and texture list point at. each
 * starts on a 4-byte boundary */
const char security_land_filenames[301] = {
#include "stg08/terrain/security_land_filenames.inc"
};

/* STG08.PRS's land, texture lists, chunk models and Basic models, from the
 * disc. its D lies between security_hall.c and object_textures.c */

/* the data up to the last that a pointer names ahead of its definition, as
 * tentative definitions in the disc's order: GCC places each datum at its
 * first definition */
static struct land_entry entries[375];
struct land_table security_land;
static NJS_TEXNAME textures[29];
NJS_TEXLIST security_hall_terrain_textures;
static Sint16 draw_272_polygons[25];
static Sint32 draw_272_vertices[19];
static NJS_CNK_MODEL draw_272_model;
static NJS_CNK_OBJECT draw_272;
static Sint16 draw_271_polygons[51];
static Sint32 draw_271_vertices[47];
static NJS_CNK_MODEL draw_271_model;
static NJS_CNK_OBJECT draw_271;
static Sint16 draw_270_polygons[149];
static Sint32 draw_270_vertices[107];
static NJS_CNK_MODEL draw_270_model;
static NJS_CNK_OBJECT draw_270;
static Sint16 draw_268_polygons[331];
static Sint32 draw_268_vertices[259];
static NJS_CNK_MODEL draw_268_model;
static NJS_CNK_OBJECT draw_269;
static Sint16 draw_239_polygons[211];
static Sint32 draw_239_vertices[107];
static NJS_CNK_MODEL draw_239_model;
static NJS_CNK_OBJECT draw_267;
static Sint16 draw_238_polygons[163];
static Sint32 draw_238_vertices[139];
static NJS_CNK_MODEL draw_238_model;
static NJS_CNK_OBJECT draw_238;
static Sint16 draw_209_polygons[211];
static Sint32 draw_209_vertices[107];
static NJS_CNK_MODEL draw_209_model;
static NJS_CNK_OBJECT draw_237;
static Sint16 draw_208_polygons[57];
static Sint32 draw_208_vertices[35];
static NJS_CNK_MODEL draw_208_model;
static NJS_CNK_OBJECT draw_208;
static Sint16 draw_179_polygons[217];
static Sint32 draw_179_vertices[107];
static NJS_CNK_MODEL draw_179_model;
static NJS_CNK_OBJECT draw_207;
static Sint16 draw_150_polygons[243];
static Sint32 draw_150_vertices[151];
static NJS_CNK_MODEL draw_150_model;
static NJS_CNK_OBJECT draw_178;
static Sint16 draw_121_polygons[243];
static Sint32 draw_121_vertices[147];
static NJS_CNK_MODEL draw_121_model;
static NJS_CNK_OBJECT draw_149;
static Sint16 draw_92_polygons[245];
static Sint32 draw_92_vertices[131];
static NJS_CNK_MODEL draw_92_model;
static NJS_CNK_OBJECT draw_120;
static Sint16 draw_90_polygons[145];
static Sint32 draw_90_vertices[103];
static NJS_CNK_MODEL draw_90_model;
static NJS_CNK_OBJECT draw_91;
static Sint16 draw_82_polygons[79];
static Sint32 draw_82_vertices[59];
static NJS_CNK_MODEL draw_82_model;
static NJS_CNK_OBJECT draw_89;
static Sint16 draw_78_polygons[79];
static Sint32 draw_78_vertices[59];
static NJS_CNK_MODEL draw_78_model;
static NJS_CNK_OBJECT draw_81;
static Sint16 draw_77_polygons[133];
static Sint32 draw_77_vertices[67];
static NJS_CNK_MODEL draw_77_model;
static NJS_CNK_OBJECT draw_77;
static Sint16 draw_75_polygons[79];
static Sint32 draw_75_vertices[43];
static NJS_CNK_MODEL draw_75_model;
static NJS_CNK_OBJECT draw_76;
static Sint16 draw_74_polygons[145];
static Sint32 draw_74_vertices[87];
static NJS_CNK_MODEL draw_74_model;
static NJS_CNK_OBJECT draw_74;
static Sint16 draw_70_polygons[79];
static Sint32 draw_70_vertices[43];
static NJS_CNK_MODEL draw_70_model;
static NJS_CNK_OBJECT draw_73;
static Sint16 draw_68_polygons[79];
static Sint32 draw_68_vertices[47];
static NJS_CNK_MODEL draw_68_model;
static NJS_CNK_OBJECT draw_69;
static Sint16 draw_67_polygons[145];
static Sint32 draw_67_vertices[87];
static NJS_CNK_MODEL draw_67_model;
static NJS_CNK_OBJECT draw_67;
static Sint16 draw_65_polygons[145];
static Sint32 draw_65_vertices[103];
static NJS_CNK_MODEL draw_65_model;
static NJS_CNK_OBJECT draw_66;
static Sint16 draw_64_polygons[133];
static Sint32 draw_64_vertices[87];
static NJS_CNK_MODEL draw_64_model;
static NJS_CNK_OBJECT draw_64;
static Sint16 draw_63_polygons[25];
static Sint32 draw_63_vertices[19];
static NJS_CNK_MODEL draw_63_model;
static NJS_CNK_OBJECT draw_63;
static Sint16 draw_62_polygons[25];
static Sint32 draw_62_vertices[19];
static NJS_CNK_MODEL draw_62_model;
static NJS_CNK_OBJECT draw_62;
static Sint16 draw_61_polygons[169];
static Sint32 draw_61_vertices[99];
static NJS_CNK_MODEL draw_61_model;
static NJS_CNK_OBJECT draw_61;
static Sint16 draw_60_polygons[229];
static Sint32 draw_60_vertices[155];
static NJS_CNK_MODEL draw_60_model;
static NJS_CNK_OBJECT draw_60;
static Sint16 draw_59_polygons[51];
static Sint32 draw_59_vertices[51];
static NJS_CNK_MODEL draw_59_model;
static NJS_CNK_OBJECT draw_59;
static Sint16 draw_58_polygons[145];
static Sint32 draw_58_vertices[71];
static NJS_CNK_MODEL draw_58_model;
static NJS_CNK_OBJECT draw_58;
static Sint16 draw_57_polygons[51];
static Sint32 draw_57_vertices[35];
static NJS_CNK_MODEL draw_57_model;
static NJS_CNK_OBJECT draw_57;
static Sint16 draw_49_polygons[211];
static Sint32 draw_49_vertices[219];
static NJS_CNK_MODEL draw_49_model;
static NJS_CNK_OBJECT draw_56;
static Sint16 draw_48_polygons[201];
static Sint32 draw_48_vertices[91];
static NJS_CNK_MODEL draw_48_model;
static NJS_CNK_OBJECT draw_48;
static Sint16 draw_46_polygons[181];
static Sint32 draw_46_vertices[107];
static NJS_CNK_MODEL draw_46_model;
static NJS_CNK_OBJECT draw_47;
static Sint16 draw_44_polygons[299];
static Sint32 draw_44_vertices[183];
static NJS_CNK_MODEL draw_44_model;
static NJS_CNK_OBJECT draw_45;
static Sint16 draw_43_polygons[961];
static Sint32 draw_43_vertices[803];
static NJS_CNK_MODEL draw_43_model;
static NJS_CNK_OBJECT draw_43;
static Sint16 draw_42_polygons[357];
static Sint32 draw_42_vertices[387];
static NJS_CNK_MODEL draw_42_model;
static NJS_CNK_OBJECT draw_42;
static Sint16 draw_41_polygons[221];
static Sint32 draw_41_vertices[259];
static NJS_CNK_MODEL draw_41_model;
static NJS_CNK_OBJECT draw_41;
static Sint16 draw_40_polygons[187];
static Sint32 draw_40_vertices[83];
static NJS_CNK_MODEL draw_40_model;
static NJS_CNK_OBJECT draw_40;
static Sint16 draw_39_polygons[161];
static Sint32 draw_39_vertices[135];
static NJS_CNK_MODEL draw_39_model;
static NJS_CNK_OBJECT draw_39;
static Sint16 draw_38_polygons[25];
static Sint32 draw_38_vertices[19];
static NJS_CNK_MODEL draw_38_model;
static NJS_CNK_OBJECT draw_38;
static Sint16 draw_37_polygons[25];
static Sint32 draw_37_vertices[19];
static NJS_CNK_MODEL draw_37_model;
static NJS_CNK_OBJECT draw_37;
static Sint16 draw_36_polygons[25];
static Sint32 draw_36_vertices[19];
static NJS_CNK_MODEL draw_36_model;
static NJS_CNK_OBJECT draw_36;
static Sint16 draw_35_polygons[25];
static Sint32 draw_35_vertices[19];
static NJS_CNK_MODEL draw_35_model;
static NJS_CNK_OBJECT draw_35;
static Sint16 draw_34_polygons[89];
static Sint32 draw_34_vertices[67];
static NJS_CNK_MODEL draw_34_model;
static NJS_CNK_OBJECT draw_34;
static Sint16 draw_33_polygons[131];
static Sint32 draw_33_vertices[91];
static NJS_CNK_MODEL draw_33_model;
static NJS_CNK_OBJECT draw_33;
static Sint16 draw_31_polygons[379];
static Sint32 draw_31_vertices[355];
static NJS_CNK_MODEL draw_31_model;
static NJS_CNK_OBJECT draw_32;
static Sint16 draw_29_polygons[409];
static Sint32 draw_29_vertices[315];
static NJS_CNK_MODEL draw_29_model;
static NJS_CNK_OBJECT draw_30;
static Sint16 draw_23_polygons[437];
static Sint32 draw_23_vertices[379];
static NJS_CNK_MODEL draw_23_model;
static NJS_CNK_OBJECT draw_28;
static Sint16 draw_20_polygons[897];
static Sint32 draw_20_vertices[1007];
static NJS_CNK_MODEL draw_20_model;
static NJS_CNK_OBJECT draw_22;
static Sint16 draw_18_polygons[1067];
static Sint32 draw_18_vertices[1047];
static NJS_CNK_MODEL draw_18_model;
static NJS_CNK_OBJECT draw_19;
static Sint16 draw_15_polygons[83];
static Sint32 draw_15_vertices[83];
static NJS_CNK_MODEL draw_15_model;
static NJS_CNK_OBJECT draw_17;
static Sint16 draw_7_polygons[161];
static Sint32 draw_7_vertices[179];
static NJS_CNK_MODEL draw_7_model;
static NJS_CNK_OBJECT draw_14;
static Sint16 draw_4_polygons[117];
static Sint32 draw_4_vertices[131];
static NJS_CNK_MODEL draw_4_model;
static NJS_CNK_OBJECT draw_6;
static Sint16 draw_3_polygons[383];
static Sint32 draw_3_vertices[299];
static NJS_CNK_MODEL draw_3_model;
static NJS_CNK_OBJECT draw_3;
static Sint16 draw_1_polygons[395];
static Sint32 draw_1_vertices[451];
static NJS_CNK_MODEL draw_1_model;
static NJS_CNK_OBJECT draw_2;
static Sint16 draw_0_polygons[475];
static Sint32 draw_0_vertices[403];
static NJS_CNK_MODEL draw_0_model;
static NJS_CNK_OBJECT draw_0;
static NJS_MATERIAL collision_0_materials[1];
static Sint16 collision_0_faces[51];
static NJS_MESHSET collision_0_meshsets[1];
static NJS_POINT3 collision_0_points[32];
static NJS_MODEL collision_0_model;
static NJS_OBJECT collision_0;
static NJS_MATERIAL collision_3_materials[1];
static Sint16 collision_3_faces[46];
static NJS_MESHSET collision_3_meshsets[1];
static NJS_POINT3 collision_3_points[34];
static NJS_MODEL collision_3_model;
static NJS_OBJECT collision_3;
static NJS_MATERIAL collision_4_materials[1];
static Sint16 collision_4_faces[33];
static NJS_MESHSET collision_4_meshsets[1];
static NJS_POINT3 collision_4_points[20];
static NJS_MODEL collision_4_model;
static NJS_OBJECT collision_4;
static NJS_MATERIAL collision_10_materials[1];
static Sint16 collision_10_faces[36];
static NJS_MESHSET collision_10_meshsets[1];
static NJS_POINT3 collision_10_points[20];
static NJS_MODEL collision_10_model;
static NJS_OBJECT collision_10;
static NJS_MATERIAL collision_11_materials[1];
static Sint16 collision_11_faces[124];
static NJS_MESHSET collision_11_meshsets[1];
static NJS_POINT3 collision_11_points[60];
static NJS_MODEL collision_11_model;
static NJS_OBJECT collision_11;
static NJS_MATERIAL collision_12_materials[1];
static Sint16 collision_12_faces[48];
static NJS_MESHSET collision_12_meshsets[1];
static NJS_POINT3 collision_12_points[36];
static NJS_MODEL collision_12_model;
static NJS_OBJECT collision_12;
static NJS_MATERIAL collision_13_materials[1];
static Sint16 collision_13_faces[115];
static NJS_MESHSET collision_13_meshsets[1];
static NJS_POINT3 collision_13_points[60];
static NJS_MODEL collision_13_model;
static NJS_OBJECT collision_13;
static NJS_MATERIAL collision_14_materials[1];
static Sint16 collision_14_faces[36];
static NJS_MESHSET collision_14_meshsets[1];
static NJS_POINT3 collision_14_points[20];
static NJS_MODEL collision_14_model;
static NJS_OBJECT collision_14;
static NJS_MATERIAL collision_15_materials[1];
static Sint16 collision_15_faces[106];
static NJS_MESHSET collision_15_meshsets[1];
static NJS_POINT3 collision_15_points[72];
static NJS_MODEL collision_15_model;
static NJS_OBJECT collision_15;
static NJS_MATERIAL collision_16_materials[1];
static Sint16 collision_16_faces[35];
static NJS_MESHSET collision_16_meshsets[1];
static NJS_POINT3 collision_16_points[20];
static NJS_MODEL collision_16_model;
static NJS_OBJECT collision_16;
static NJS_MATERIAL collision_17_materials[1];
static Sint16 collision_17_faces[27];
static NJS_MESHSET collision_17_meshsets[1];
static NJS_POINT3 collision_17_points[16];
static NJS_MODEL collision_17_model;
static NJS_OBJECT collision_17;
static NJS_MATERIAL collision_19_materials[1];
static Sint16 collision_19_faces[41];
static NJS_MESHSET collision_19_meshsets[1];
static NJS_POINT3 collision_19_points[22];
static NJS_MODEL collision_19_model;
static NJS_OBJECT collision_19;
static NJS_MATERIAL collision_20_materials[1];
static Sint16 collision_20_faces[47];
static NJS_MESHSET collision_20_meshsets[1];
static NJS_POINT3 collision_20_points[25];
static NJS_MODEL collision_20_model;
static NJS_OBJECT collision_20;
static NJS_MATERIAL collision_21_materials[1];
static Sint16 collision_21_faces[53];
static NJS_MESHSET collision_21_meshsets[1];
static NJS_POINT3 collision_21_points[29];
static NJS_MODEL collision_21_model;
static NJS_OBJECT collision_21;
static NJS_MATERIAL collision_22_materials[1];
static Sint16 collision_22_faces[27];
static NJS_MESHSET collision_22_meshsets[1];
static NJS_POINT3 collision_22_points[12];
static NJS_MODEL collision_22_model;
static NJS_OBJECT collision_22;
static NJS_MATERIAL collision_23_materials[1];
static Sint16 collision_23_faces[55];
static NJS_MESHSET collision_23_meshsets[1];
static NJS_POINT3 collision_23_points[40];
static NJS_MODEL collision_23_model;
static NJS_OBJECT collision_23;
static NJS_MATERIAL collision_24_materials[1];
static Sint16 collision_24_faces[29];
static NJS_MESHSET collision_24_meshsets[1];
static NJS_POINT3 collision_24_points[22];
static NJS_MODEL collision_24_model;
static NJS_OBJECT collision_24;
static NJS_MATERIAL collision_25_materials[1];
static Sint16 collision_25_faces[27];
static NJS_MESHSET collision_25_meshsets[1];
static NJS_POINT3 collision_25_points[12];
static NJS_MODEL collision_25_model;
static NJS_OBJECT collision_25;
static NJS_MATERIAL collision_26_materials[1];
static Sint16 collision_26_faces[58];
static NJS_MESHSET collision_26_meshsets[1];
static NJS_POINT3 collision_26_points[32];
static NJS_MODEL collision_26_model;
static NJS_OBJECT collision_26;
static NJS_MATERIAL collision_29_materials[1];
static Sint16 collision_29_faces[68];
static NJS_MESHSET collision_29_meshsets[1];
static NJS_POINT3 collision_29_points[40];
static NJS_MODEL collision_29_model;
static NJS_OBJECT collision_29;
static NJS_MATERIAL collision_30_materials[1];
static Sint16 collision_30_faces[82];
static NJS_MESHSET collision_30_meshsets[1];
static NJS_POINT3 collision_30_points[45];
static NJS_MODEL collision_30_model;
static NJS_OBJECT collision_30;
static NJS_MATERIAL collision_31_materials[1];
static Sint16 collision_31_faces[60];
static NJS_MESHSET collision_31_meshsets[1];
static NJS_POINT3 collision_31_points[35];
static NJS_MODEL collision_31_model;
static NJS_OBJECT collision_31;
static NJS_MATERIAL collision_32_materials[1];
static Sint16 collision_32_faces[68];
static NJS_MESHSET collision_32_meshsets[1];
static NJS_POINT3 collision_32_points[40];
static NJS_MODEL collision_32_model;
static NJS_OBJECT collision_32;
static NJS_MATERIAL collision_33_materials[1];
static Sint16 collision_33_faces[22];
static NJS_MESHSET collision_33_meshsets[1];
static NJS_POINT3 collision_33_points[12];
static NJS_MODEL collision_33_model;
static NJS_OBJECT collision_33;
static NJS_MATERIAL collision_41_materials[1];
static Sint16 collision_41_faces[56];
static NJS_MESHSET collision_41_meshsets[1];
static NJS_POINT3 collision_41_points[38];
static NJS_MODEL collision_41_model;
static NJS_OBJECT collision_41;
static NJS_MATERIAL collision_42_materials[1];
static Sint16 collision_42_faces[106];
static NJS_MESHSET collision_42_meshsets[1];
static NJS_POINT3 collision_42_points[56];
static NJS_MODEL collision_42_model;
static NJS_OBJECT collision_42;
static NJS_MATERIAL collision_84_materials[1];
static Sint16 collision_84_faces[124];
static NJS_MESHSET collision_84_meshsets[1];
static NJS_POINT3 collision_84_points[59];
static NJS_MODEL collision_84_model;
static NJS_OBJECT collision_84;
static NJS_MATERIAL collision_85_materials[1];
static Sint16 collision_85_faces[60];
static NJS_MESHSET collision_85_meshsets[1];
static NJS_POINT3 collision_85_points[35];
static NJS_MODEL collision_85_model;
static NJS_OBJECT collision_85;
static NJS_MATERIAL collision_86_materials[1];
static Sint16 collision_86_faces[106];
static NJS_MESHSET collision_86_meshsets[1];
static NJS_POINT3 collision_86_points[59];
static NJS_MODEL collision_86_model;
static NJS_OBJECT collision_86;
static NJS_MATERIAL collision_87_materials[1];
static Sint16 collision_87_faces[17];
static NJS_MESHSET collision_87_meshsets[1];
static NJS_POINT3 collision_87_points[16];
static NJS_MODEL collision_87_model;
static NJS_OBJECT collision_87;
static NJS_MATERIAL collision_88_materials[1];
static Sint16 collision_88_faces[15];
static NJS_MESHSET collision_88_meshsets[1];
static NJS_POINT3 collision_88_points[14];
static NJS_MODEL collision_88_model;
static NJS_OBJECT collision_88;
static NJS_MATERIAL collision_89_materials[1];
static Sint16 collision_89_faces[17];
static NJS_MESHSET collision_89_meshsets[1];
static NJS_POINT3 collision_89_points[16];
static NJS_MODEL collision_89_model;
static NJS_OBJECT collision_89;
static NJS_MATERIAL collision_90_materials[1];
static Sint16 collision_90_faces[15];
static NJS_MESHSET collision_90_meshsets[1];
static NJS_POINT3 collision_90_points[14];
static NJS_MODEL collision_90_model;
static NJS_OBJECT collision_90;
static NJS_MATERIAL collision_91_materials[1];
static Sint16 collision_91_faces[32];
static NJS_MESHSET collision_91_meshsets[1];
static NJS_POINT3 collision_91_points[16];
static NJS_MODEL collision_91_model;
static NJS_OBJECT collision_91;
static NJS_MATERIAL collision_93_materials[1];
static Sint16 collision_93_faces[26];
static NJS_MESHSET collision_93_meshsets[1];
static NJS_POINT3 collision_93_points[12];
static NJS_MODEL collision_93_model;
static NJS_OBJECT collision_93;
static NJS_MATERIAL collision_95_materials[1];
static Sint16 collision_95_faces[32];
static NJS_MESHSET collision_95_meshsets[1];
static NJS_POINT3 collision_95_points[16];
static NJS_MODEL collision_95_model;
static NJS_OBJECT collision_95;
static NJS_MATERIAL collision_98_materials[1];
static Sint16 collision_98_faces[28];
static NJS_MESHSET collision_98_meshsets[1];
static NJS_POINT3 collision_98_points[16];
static NJS_MODEL collision_98_model;
static NJS_OBJECT collision_98;
static NJS_MATERIAL collision_100_materials[1];
static Sint16 collision_100_faces[56];
static NJS_MESHSET collision_100_meshsets[1];
static NJS_POINT3 collision_100_points[24];
static NJS_MODEL collision_100_model;
static NJS_OBJECT collision_100;
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
static NJS_CNK_OBJECT draw_90;
static NJS_CNK_OBJECT draw_88;
static NJS_CNK_OBJECT draw_87;
static NJS_CNK_OBJECT draw_86;
static NJS_CNK_OBJECT draw_85;
static NJS_CNK_OBJECT draw_84;
static NJS_CNK_OBJECT draw_83;
static NJS_CNK_OBJECT draw_82;
static NJS_CNK_OBJECT draw_80;
static NJS_CNK_OBJECT draw_79;
static NJS_CNK_OBJECT draw_78;
static NJS_CNK_OBJECT draw_75;
static NJS_CNK_OBJECT draw_72;
static NJS_CNK_OBJECT draw_71;
static NJS_CNK_OBJECT draw_70;
static NJS_CNK_OBJECT draw_68;
static NJS_CNK_OBJECT draw_65;
static NJS_CNK_OBJECT draw_55;
static NJS_CNK_OBJECT draw_54;
static NJS_CNK_OBJECT draw_53;
static NJS_CNK_OBJECT draw_52;
static NJS_CNK_OBJECT draw_51;
static NJS_CNK_OBJECT draw_50;
static NJS_CNK_OBJECT draw_49;
static NJS_CNK_OBJECT draw_46;
static NJS_CNK_OBJECT draw_44;
static NJS_CNK_OBJECT draw_31;
static NJS_CNK_OBJECT draw_29;
static NJS_CNK_OBJECT draw_27;
static NJS_CNK_OBJECT draw_26;
static NJS_CNK_OBJECT draw_25;
static NJS_CNK_OBJECT draw_24;
static NJS_CNK_OBJECT draw_23;
static NJS_CNK_OBJECT draw_21;
static NJS_CNK_OBJECT draw_20;
static NJS_CNK_OBJECT draw_18;
static NJS_CNK_OBJECT draw_16;
static NJS_CNK_OBJECT draw_15;
static NJS_CNK_OBJECT draw_13;
static NJS_CNK_OBJECT draw_12;
static NJS_CNK_OBJECT draw_11;
static NJS_CNK_OBJECT draw_10;
static NJS_CNK_OBJECT draw_9;
static NJS_CNK_OBJECT draw_8;
static NJS_CNK_OBJECT draw_7;
static NJS_CNK_OBJECT draw_5;
static NJS_CNK_OBJECT draw_4;
static NJS_CNK_OBJECT draw_1;
static NJS_OBJECT collision_1;
static NJS_OBJECT collision_2;
static NJS_OBJECT collision_5;
static NJS_OBJECT collision_6;
static NJS_OBJECT collision_7;
static NJS_OBJECT collision_8;
static NJS_OBJECT collision_9;
static NJS_OBJECT collision_18;
static NJS_OBJECT collision_27;
static NJS_OBJECT collision_28;
static NJS_OBJECT collision_34;
static NJS_OBJECT collision_35;
static NJS_OBJECT collision_36;
static NJS_OBJECT collision_37;
static NJS_OBJECT collision_38;
static NJS_OBJECT collision_39;
static NJS_OBJECT collision_40;
static NJS_OBJECT collision_43;
static NJS_OBJECT collision_44;
static NJS_OBJECT collision_45;
static NJS_OBJECT collision_46;
static NJS_OBJECT collision_47;
static NJS_OBJECT collision_48;
static NJS_OBJECT collision_49;
static NJS_OBJECT collision_50;
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
static NJS_OBJECT collision_64;
static NJS_OBJECT collision_65;
static NJS_OBJECT collision_66;
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
static NJS_OBJECT collision_78;
static NJS_OBJECT collision_79;
static NJS_OBJECT collision_80;
static NJS_OBJECT collision_81;
static NJS_OBJECT collision_82;
static NJS_OBJECT collision_83;
static NJS_OBJECT collision_92;
static NJS_OBJECT collision_94;
static NJS_OBJECT collision_96;
static NJS_OBJECT collision_97;
static NJS_OBJECT collision_99;
static NJS_OBJECT collision_101;

static struct land_entry entries[375] = {
#include "stg08/terrain/entries.inc"
};
struct land_table security_land = {
#include "stg08/terrain/security_land.inc"
};
static NJS_TEXNAME textures[29] = {
#include "stg08/terrain/textures.inc"
};
NJS_TEXLIST security_hall_terrain_textures = {
#include "stg08/terrain/security_hall_terrain_textures.inc"
};
static Sint16 draw_272_polygons[25] = {
#include "stg08/terrain/draw_272_polygons.inc"
};
static Sint32 draw_272_vertices[19] = {
#include "stg08/terrain/draw_272_vertices.inc"
};
static NJS_CNK_MODEL draw_272_model = {
#include "stg08/terrain/draw_272_model.inc"
};
static NJS_CNK_OBJECT draw_272 = {
#include "stg08/terrain/draw_272.inc"
};
static Sint16 draw_271_polygons[51] = {
#include "stg08/terrain/draw_271_polygons.inc"
};
static Sint32 draw_271_vertices[47] = {
#include "stg08/terrain/draw_271_vertices.inc"
};
static NJS_CNK_MODEL draw_271_model = {
#include "stg08/terrain/draw_271_model.inc"
};
static NJS_CNK_OBJECT draw_271 = {
#include "stg08/terrain/draw_271.inc"
};
static Sint16 draw_270_polygons[149] = {
#include "stg08/terrain/draw_270_polygons.inc"
};
static Sint32 draw_270_vertices[107] = {
#include "stg08/terrain/draw_270_vertices.inc"
};
static NJS_CNK_MODEL draw_270_model = {
#include "stg08/terrain/draw_270_model.inc"
};
static NJS_CNK_OBJECT draw_270 = {
#include "stg08/terrain/draw_270.inc"
};
static Sint16 draw_268_polygons[331] = {
#include "stg08/terrain/draw_268_polygons.inc"
};
static Sint32 draw_268_vertices[259] = {
#include "stg08/terrain/draw_268_vertices.inc"
};
static NJS_CNK_MODEL draw_268_model = {
#include "stg08/terrain/draw_268_model.inc"
};
static NJS_CNK_OBJECT draw_269 = {
#include "stg08/terrain/draw_269.inc"
};
static Sint16 draw_239_polygons[211] = {
#include "stg08/terrain/draw_239_polygons.inc"
};
static Sint32 draw_239_vertices[107] = {
#include "stg08/terrain/draw_239_vertices.inc"
};
static NJS_CNK_MODEL draw_239_model = {
#include "stg08/terrain/draw_239_model.inc"
};
static NJS_CNK_OBJECT draw_267 = {
#include "stg08/terrain/draw_267.inc"
};
static Sint16 draw_238_polygons[163] = {
#include "stg08/terrain/draw_238_polygons.inc"
};
static Sint32 draw_238_vertices[139] = {
#include "stg08/terrain/draw_238_vertices.inc"
};
static NJS_CNK_MODEL draw_238_model = {
#include "stg08/terrain/draw_238_model.inc"
};
static NJS_CNK_OBJECT draw_238 = {
#include "stg08/terrain/draw_238.inc"
};
static Sint16 draw_209_polygons[211] = {
#include "stg08/terrain/draw_209_polygons.inc"
};
static Sint32 draw_209_vertices[107] = {
#include "stg08/terrain/draw_209_vertices.inc"
};
static NJS_CNK_MODEL draw_209_model = {
#include "stg08/terrain/draw_209_model.inc"
};
static NJS_CNK_OBJECT draw_237 = {
#include "stg08/terrain/draw_237.inc"
};
static Sint16 draw_208_polygons[57] = {
#include "stg08/terrain/draw_208_polygons.inc"
};
static Sint32 draw_208_vertices[35] = {
#include "stg08/terrain/draw_208_vertices.inc"
};
static NJS_CNK_MODEL draw_208_model = {
#include "stg08/terrain/draw_208_model.inc"
};
static NJS_CNK_OBJECT draw_208 = {
#include "stg08/terrain/draw_208.inc"
};
static Sint16 draw_179_polygons[217] = {
#include "stg08/terrain/draw_179_polygons.inc"
};
static Sint32 draw_179_vertices[107] = {
#include "stg08/terrain/draw_179_vertices.inc"
};
static NJS_CNK_MODEL draw_179_model = {
#include "stg08/terrain/draw_179_model.inc"
};
static NJS_CNK_OBJECT draw_207 = {
#include "stg08/terrain/draw_207.inc"
};
static Sint16 draw_150_polygons[243] = {
#include "stg08/terrain/draw_150_polygons.inc"
};
static Sint32 draw_150_vertices[151] = {
#include "stg08/terrain/draw_150_vertices.inc"
};
static NJS_CNK_MODEL draw_150_model = {
#include "stg08/terrain/draw_150_model.inc"
};
static NJS_CNK_OBJECT draw_178 = {
#include "stg08/terrain/draw_178.inc"
};
static Sint16 draw_121_polygons[243] = {
#include "stg08/terrain/draw_121_polygons.inc"
};
static Sint32 draw_121_vertices[147] = {
#include "stg08/terrain/draw_121_vertices.inc"
};
static NJS_CNK_MODEL draw_121_model = {
#include "stg08/terrain/draw_121_model.inc"
};
static NJS_CNK_OBJECT draw_149 = {
#include "stg08/terrain/draw_149.inc"
};
static Sint16 draw_92_polygons[245] = {
#include "stg08/terrain/draw_92_polygons.inc"
};
static Sint32 draw_92_vertices[131] = {
#include "stg08/terrain/draw_92_vertices.inc"
};
static NJS_CNK_MODEL draw_92_model = {
#include "stg08/terrain/draw_92_model.inc"
};
static NJS_CNK_OBJECT draw_120 = {
#include "stg08/terrain/draw_120.inc"
};
static Sint16 draw_90_polygons[145] = {
#include "stg08/terrain/draw_90_polygons.inc"
};
static Sint32 draw_90_vertices[103] = {
#include "stg08/terrain/draw_90_vertices.inc"
};
static NJS_CNK_MODEL draw_90_model = {
#include "stg08/terrain/draw_90_model.inc"
};
static NJS_CNK_OBJECT draw_91 = {
#include "stg08/terrain/draw_91.inc"
};
static Sint16 draw_82_polygons[79] = {
#include "stg08/terrain/draw_82_polygons.inc"
};
static Sint32 draw_82_vertices[59] = {
#include "stg08/terrain/draw_82_vertices.inc"
};
static NJS_CNK_MODEL draw_82_model = {
#include "stg08/terrain/draw_82_model.inc"
};
static NJS_CNK_OBJECT draw_89 = {
#include "stg08/terrain/draw_89.inc"
};
static Sint16 draw_78_polygons[79] = {
#include "stg08/terrain/draw_78_polygons.inc"
};
static Sint32 draw_78_vertices[59] = {
#include "stg08/terrain/draw_78_vertices.inc"
};
static NJS_CNK_MODEL draw_78_model = {
#include "stg08/terrain/draw_78_model.inc"
};
static NJS_CNK_OBJECT draw_81 = {
#include "stg08/terrain/draw_81.inc"
};
static Sint16 draw_77_polygons[133] = {
#include "stg08/terrain/draw_77_polygons.inc"
};
static Sint32 draw_77_vertices[67] = {
#include "stg08/terrain/draw_77_vertices.inc"
};
static NJS_CNK_MODEL draw_77_model = {
#include "stg08/terrain/draw_77_model.inc"
};
static NJS_CNK_OBJECT draw_77 = {
#include "stg08/terrain/draw_77.inc"
};
static Sint16 draw_75_polygons[79] = {
#include "stg08/terrain/draw_75_polygons.inc"
};
static Sint32 draw_75_vertices[43] = {
#include "stg08/terrain/draw_75_vertices.inc"
};
static NJS_CNK_MODEL draw_75_model = {
#include "stg08/terrain/draw_75_model.inc"
};
static NJS_CNK_OBJECT draw_76 = {
#include "stg08/terrain/draw_76.inc"
};
static Sint16 draw_74_polygons[145] = {
#include "stg08/terrain/draw_74_polygons.inc"
};
static Sint32 draw_74_vertices[87] = {
#include "stg08/terrain/draw_74_vertices.inc"
};
static NJS_CNK_MODEL draw_74_model = {
#include "stg08/terrain/draw_74_model.inc"
};
static NJS_CNK_OBJECT draw_74 = {
#include "stg08/terrain/draw_74.inc"
};
static Sint16 draw_70_polygons[79] = {
#include "stg08/terrain/draw_70_polygons.inc"
};
static Sint32 draw_70_vertices[43] = {
#include "stg08/terrain/draw_70_vertices.inc"
};
static NJS_CNK_MODEL draw_70_model = {
#include "stg08/terrain/draw_70_model.inc"
};
static NJS_CNK_OBJECT draw_73 = {
#include "stg08/terrain/draw_73.inc"
};
static Sint16 draw_68_polygons[79] = {
#include "stg08/terrain/draw_68_polygons.inc"
};
static Sint32 draw_68_vertices[47] = {
#include "stg08/terrain/draw_68_vertices.inc"
};
static NJS_CNK_MODEL draw_68_model = {
#include "stg08/terrain/draw_68_model.inc"
};
static NJS_CNK_OBJECT draw_69 = {
#include "stg08/terrain/draw_69.inc"
};
static Sint16 draw_67_polygons[145] = {
#include "stg08/terrain/draw_67_polygons.inc"
};
static Sint32 draw_67_vertices[87] = {
#include "stg08/terrain/draw_67_vertices.inc"
};
static NJS_CNK_MODEL draw_67_model = {
#include "stg08/terrain/draw_67_model.inc"
};
static NJS_CNK_OBJECT draw_67 = {
#include "stg08/terrain/draw_67.inc"
};
static Sint16 draw_65_polygons[145] = {
#include "stg08/terrain/draw_65_polygons.inc"
};
static Sint32 draw_65_vertices[103] = {
#include "stg08/terrain/draw_65_vertices.inc"
};
static NJS_CNK_MODEL draw_65_model = {
#include "stg08/terrain/draw_65_model.inc"
};
static NJS_CNK_OBJECT draw_66 = {
#include "stg08/terrain/draw_66.inc"
};
static Sint16 draw_64_polygons[133] = {
#include "stg08/terrain/draw_64_polygons.inc"
};
static Sint32 draw_64_vertices[87] = {
#include "stg08/terrain/draw_64_vertices.inc"
};
static NJS_CNK_MODEL draw_64_model = {
#include "stg08/terrain/draw_64_model.inc"
};
static NJS_CNK_OBJECT draw_64 = {
#include "stg08/terrain/draw_64.inc"
};
static Sint16 draw_63_polygons[25] = {
#include "stg08/terrain/draw_63_polygons.inc"
};
static Sint32 draw_63_vertices[19] = {
#include "stg08/terrain/draw_63_vertices.inc"
};
static NJS_CNK_MODEL draw_63_model = {
#include "stg08/terrain/draw_63_model.inc"
};
static NJS_CNK_OBJECT draw_63 = {
#include "stg08/terrain/draw_63.inc"
};
static Sint16 draw_62_polygons[25] = {
#include "stg08/terrain/draw_62_polygons.inc"
};
static Sint32 draw_62_vertices[19] = {
#include "stg08/terrain/draw_62_vertices.inc"
};
static NJS_CNK_MODEL draw_62_model = {
#include "stg08/terrain/draw_62_model.inc"
};
static NJS_CNK_OBJECT draw_62 = {
#include "stg08/terrain/draw_62.inc"
};
static Sint16 draw_61_polygons[169] = {
#include "stg08/terrain/draw_61_polygons.inc"
};
static Sint32 draw_61_vertices[99] = {
#include "stg08/terrain/draw_61_vertices.inc"
};
static NJS_CNK_MODEL draw_61_model = {
#include "stg08/terrain/draw_61_model.inc"
};
static NJS_CNK_OBJECT draw_61 = {
#include "stg08/terrain/draw_61.inc"
};
static Sint16 draw_60_polygons[229] = {
#include "stg08/terrain/draw_60_polygons.inc"
};
static Sint32 draw_60_vertices[155] = {
#include "stg08/terrain/draw_60_vertices.inc"
};
static NJS_CNK_MODEL draw_60_model = {
#include "stg08/terrain/draw_60_model.inc"
};
static NJS_CNK_OBJECT draw_60 = {
#include "stg08/terrain/draw_60.inc"
};
static Sint16 draw_59_polygons[51] = {
#include "stg08/terrain/draw_59_polygons.inc"
};
static Sint32 draw_59_vertices[51] = {
#include "stg08/terrain/draw_59_vertices.inc"
};
static NJS_CNK_MODEL draw_59_model = {
#include "stg08/terrain/draw_59_model.inc"
};
static NJS_CNK_OBJECT draw_59 = {
#include "stg08/terrain/draw_59.inc"
};
static Sint16 draw_58_polygons[145] = {
#include "stg08/terrain/draw_58_polygons.inc"
};
static Sint32 draw_58_vertices[71] = {
#include "stg08/terrain/draw_58_vertices.inc"
};
static NJS_CNK_MODEL draw_58_model = {
#include "stg08/terrain/draw_58_model.inc"
};
static NJS_CNK_OBJECT draw_58 = {
#include "stg08/terrain/draw_58.inc"
};
static Sint16 draw_57_polygons[51] = {
#include "stg08/terrain/draw_57_polygons.inc"
};
static Sint32 draw_57_vertices[35] = {
#include "stg08/terrain/draw_57_vertices.inc"
};
static NJS_CNK_MODEL draw_57_model = {
#include "stg08/terrain/draw_57_model.inc"
};
static NJS_CNK_OBJECT draw_57 = {
#include "stg08/terrain/draw_57.inc"
};
static Sint16 draw_49_polygons[211] = {
#include "stg08/terrain/draw_49_polygons.inc"
};
static Sint32 draw_49_vertices[219] = {
#include "stg08/terrain/draw_49_vertices.inc"
};
static NJS_CNK_MODEL draw_49_model = {
#include "stg08/terrain/draw_49_model.inc"
};
static NJS_CNK_OBJECT draw_56 = {
#include "stg08/terrain/draw_56.inc"
};
static Sint16 draw_48_polygons[201] = {
#include "stg08/terrain/draw_48_polygons.inc"
};
static Sint32 draw_48_vertices[91] = {
#include "stg08/terrain/draw_48_vertices.inc"
};
static NJS_CNK_MODEL draw_48_model = {
#include "stg08/terrain/draw_48_model.inc"
};
static NJS_CNK_OBJECT draw_48 = {
#include "stg08/terrain/draw_48.inc"
};
static Sint16 draw_46_polygons[181] = {
#include "stg08/terrain/draw_46_polygons.inc"
};
static Sint32 draw_46_vertices[107] = {
#include "stg08/terrain/draw_46_vertices.inc"
};
static NJS_CNK_MODEL draw_46_model = {
#include "stg08/terrain/draw_46_model.inc"
};
static NJS_CNK_OBJECT draw_47 = {
#include "stg08/terrain/draw_47.inc"
};
static Sint16 draw_44_polygons[299] = {
#include "stg08/terrain/draw_44_polygons.inc"
};
static Sint32 draw_44_vertices[183] = {
#include "stg08/terrain/draw_44_vertices.inc"
};
static NJS_CNK_MODEL draw_44_model = {
#include "stg08/terrain/draw_44_model.inc"
};
static NJS_CNK_OBJECT draw_45 = {
#include "stg08/terrain/draw_45.inc"
};
static Sint16 draw_43_polygons[961] = {
#include "stg08/terrain/draw_43_polygons.inc"
};
static Sint32 draw_43_vertices[803] = {
#include "stg08/terrain/draw_43_vertices.inc"
};
static NJS_CNK_MODEL draw_43_model = {
#include "stg08/terrain/draw_43_model.inc"
};
static NJS_CNK_OBJECT draw_43 = {
#include "stg08/terrain/draw_43.inc"
};
static Sint16 draw_42_polygons[357] = {
#include "stg08/terrain/draw_42_polygons.inc"
};
static Sint32 draw_42_vertices[387] = {
#include "stg08/terrain/draw_42_vertices.inc"
};
static NJS_CNK_MODEL draw_42_model = {
#include "stg08/terrain/draw_42_model.inc"
};
static NJS_CNK_OBJECT draw_42 = {
#include "stg08/terrain/draw_42.inc"
};
static Sint16 draw_41_polygons[221] = {
#include "stg08/terrain/draw_41_polygons.inc"
};
static Sint32 draw_41_vertices[259] = {
#include "stg08/terrain/draw_41_vertices.inc"
};
static NJS_CNK_MODEL draw_41_model = {
#include "stg08/terrain/draw_41_model.inc"
};
static NJS_CNK_OBJECT draw_41 = {
#include "stg08/terrain/draw_41.inc"
};
static Sint16 draw_40_polygons[187] = {
#include "stg08/terrain/draw_40_polygons.inc"
};
static Sint32 draw_40_vertices[83] = {
#include "stg08/terrain/draw_40_vertices.inc"
};
static NJS_CNK_MODEL draw_40_model = {
#include "stg08/terrain/draw_40_model.inc"
};
static NJS_CNK_OBJECT draw_40 = {
#include "stg08/terrain/draw_40.inc"
};
static Sint16 draw_39_polygons[161] = {
#include "stg08/terrain/draw_39_polygons.inc"
};
static Sint32 draw_39_vertices[135] = {
#include "stg08/terrain/draw_39_vertices.inc"
};
static NJS_CNK_MODEL draw_39_model = {
#include "stg08/terrain/draw_39_model.inc"
};
static NJS_CNK_OBJECT draw_39 = {
#include "stg08/terrain/draw_39.inc"
};
static Sint16 draw_38_polygons[25] = {
#include "stg08/terrain/draw_38_polygons.inc"
};
static Sint32 draw_38_vertices[19] = {
#include "stg08/terrain/draw_38_vertices.inc"
};
static NJS_CNK_MODEL draw_38_model = {
#include "stg08/terrain/draw_38_model.inc"
};
static NJS_CNK_OBJECT draw_38 = {
#include "stg08/terrain/draw_38.inc"
};
static Sint16 draw_37_polygons[25] = {
#include "stg08/terrain/draw_37_polygons.inc"
};
static Sint32 draw_37_vertices[19] = {
#include "stg08/terrain/draw_37_vertices.inc"
};
static NJS_CNK_MODEL draw_37_model = {
#include "stg08/terrain/draw_37_model.inc"
};
static NJS_CNK_OBJECT draw_37 = {
#include "stg08/terrain/draw_37.inc"
};
static Sint16 draw_36_polygons[25] = {
#include "stg08/terrain/draw_36_polygons.inc"
};
static Sint32 draw_36_vertices[19] = {
#include "stg08/terrain/draw_36_vertices.inc"
};
static NJS_CNK_MODEL draw_36_model = {
#include "stg08/terrain/draw_36_model.inc"
};
static NJS_CNK_OBJECT draw_36 = {
#include "stg08/terrain/draw_36.inc"
};
static Sint16 draw_35_polygons[25] = {
#include "stg08/terrain/draw_35_polygons.inc"
};
static Sint32 draw_35_vertices[19] = {
#include "stg08/terrain/draw_35_vertices.inc"
};
static NJS_CNK_MODEL draw_35_model = {
#include "stg08/terrain/draw_35_model.inc"
};
static NJS_CNK_OBJECT draw_35 = {
#include "stg08/terrain/draw_35.inc"
};
static Sint16 draw_34_polygons[89] = {
#include "stg08/terrain/draw_34_polygons.inc"
};
static Sint32 draw_34_vertices[67] = {
#include "stg08/terrain/draw_34_vertices.inc"
};
static NJS_CNK_MODEL draw_34_model = {
#include "stg08/terrain/draw_34_model.inc"
};
static NJS_CNK_OBJECT draw_34 = {
#include "stg08/terrain/draw_34.inc"
};
static Sint16 draw_33_polygons[131] = {
#include "stg08/terrain/draw_33_polygons.inc"
};
static Sint32 draw_33_vertices[91] = {
#include "stg08/terrain/draw_33_vertices.inc"
};
static NJS_CNK_MODEL draw_33_model = {
#include "stg08/terrain/draw_33_model.inc"
};
static NJS_CNK_OBJECT draw_33 = {
#include "stg08/terrain/draw_33.inc"
};
static Sint16 draw_31_polygons[379] = {
#include "stg08/terrain/draw_31_polygons.inc"
};
static Sint32 draw_31_vertices[355] = {
#include "stg08/terrain/draw_31_vertices.inc"
};
static NJS_CNK_MODEL draw_31_model = {
#include "stg08/terrain/draw_31_model.inc"
};
static NJS_CNK_OBJECT draw_32 = {
#include "stg08/terrain/draw_32.inc"
};
static Sint16 draw_29_polygons[409] = {
#include "stg08/terrain/draw_29_polygons.inc"
};
static Sint32 draw_29_vertices[315] = {
#include "stg08/terrain/draw_29_vertices.inc"
};
static NJS_CNK_MODEL draw_29_model = {
#include "stg08/terrain/draw_29_model.inc"
};
static NJS_CNK_OBJECT draw_30 = {
#include "stg08/terrain/draw_30.inc"
};
static Sint16 draw_23_polygons[437] = {
#include "stg08/terrain/draw_23_polygons.inc"
};
static Sint32 draw_23_vertices[379] = {
#include "stg08/terrain/draw_23_vertices.inc"
};
static NJS_CNK_MODEL draw_23_model = {
#include "stg08/terrain/draw_23_model.inc"
};
static NJS_CNK_OBJECT draw_28 = {
#include "stg08/terrain/draw_28.inc"
};
static Sint16 draw_20_polygons[897] = {
#include "stg08/terrain/draw_20_polygons.inc"
};
static Sint32 draw_20_vertices[1007] = {
#include "stg08/terrain/draw_20_vertices.inc"
};
static NJS_CNK_MODEL draw_20_model = {
#include "stg08/terrain/draw_20_model.inc"
};
static NJS_CNK_OBJECT draw_22 = {
#include "stg08/terrain/draw_22.inc"
};
static Sint16 draw_18_polygons[1067] = {
#include "stg08/terrain/draw_18_polygons.inc"
};
static Sint32 draw_18_vertices[1047] = {
#include "stg08/terrain/draw_18_vertices.inc"
};
static NJS_CNK_MODEL draw_18_model = {
#include "stg08/terrain/draw_18_model.inc"
};
static NJS_CNK_OBJECT draw_19 = {
#include "stg08/terrain/draw_19.inc"
};
static Sint16 draw_15_polygons[83] = {
#include "stg08/terrain/draw_15_polygons.inc"
};
static Sint32 draw_15_vertices[83] = {
#include "stg08/terrain/draw_15_vertices.inc"
};
static NJS_CNK_MODEL draw_15_model = {
#include "stg08/terrain/draw_15_model.inc"
};
static NJS_CNK_OBJECT draw_17 = {
#include "stg08/terrain/draw_17.inc"
};
static Sint16 draw_7_polygons[161] = {
#include "stg08/terrain/draw_7_polygons.inc"
};
static Sint32 draw_7_vertices[179] = {
#include "stg08/terrain/draw_7_vertices.inc"
};
static NJS_CNK_MODEL draw_7_model = {
#include "stg08/terrain/draw_7_model.inc"
};
static NJS_CNK_OBJECT draw_14 = {
#include "stg08/terrain/draw_14.inc"
};
static Sint16 draw_4_polygons[117] = {
#include "stg08/terrain/draw_4_polygons.inc"
};
static Sint32 draw_4_vertices[131] = {
#include "stg08/terrain/draw_4_vertices.inc"
};
static NJS_CNK_MODEL draw_4_model = {
#include "stg08/terrain/draw_4_model.inc"
};
static NJS_CNK_OBJECT draw_6 = {
#include "stg08/terrain/draw_6.inc"
};
static Sint16 draw_3_polygons[383] = {
#include "stg08/terrain/draw_3_polygons.inc"
};
static Sint32 draw_3_vertices[299] = {
#include "stg08/terrain/draw_3_vertices.inc"
};
static NJS_CNK_MODEL draw_3_model = {
#include "stg08/terrain/draw_3_model.inc"
};
static NJS_CNK_OBJECT draw_3 = {
#include "stg08/terrain/draw_3.inc"
};
static Sint16 draw_1_polygons[395] = {
#include "stg08/terrain/draw_1_polygons.inc"
};
static Sint32 draw_1_vertices[451] = {
#include "stg08/terrain/draw_1_vertices.inc"
};
static NJS_CNK_MODEL draw_1_model = {
#include "stg08/terrain/draw_1_model.inc"
};
static NJS_CNK_OBJECT draw_2 = {
#include "stg08/terrain/draw_2.inc"
};
static Sint16 draw_0_polygons[475] = {
#include "stg08/terrain/draw_0_polygons.inc"
};
static Sint32 draw_0_vertices[403] = {
#include "stg08/terrain/draw_0_vertices.inc"
};
static NJS_CNK_MODEL draw_0_model = {
#include "stg08/terrain/draw_0_model.inc"
};
static NJS_CNK_OBJECT draw_0 = {
#include "stg08/terrain/draw_0.inc"
};
static NJS_MATERIAL collision_0_materials[1] = {
#include "stg08/terrain/collision_0_materials.inc"
};
static Sint16 collision_0_faces[51] = {
#include "stg08/terrain/collision_0_faces.inc"
};
static NJS_MESHSET collision_0_meshsets[1] = {
#include "stg08/terrain/collision_0_meshsets.inc"
};
static NJS_POINT3 collision_0_points[32] = {
#include "stg08/terrain/collision_0_points.inc"
};
static NJS_MODEL collision_0_model = {
#include "stg08/terrain/collision_0_model.inc"
};
static NJS_OBJECT collision_0 = {
#include "stg08/terrain/collision_0.inc"
};
static NJS_MATERIAL collision_3_materials[1] = {
#include "stg08/terrain/collision_3_materials.inc"
};
static Sint16 collision_3_faces[46] = {
#include "stg08/terrain/collision_3_faces.inc"
};
static NJS_MESHSET collision_3_meshsets[1] = {
#include "stg08/terrain/collision_3_meshsets.inc"
};
static NJS_POINT3 collision_3_points[34] = {
#include "stg08/terrain/collision_3_points.inc"
};
static NJS_MODEL collision_3_model = {
#include "stg08/terrain/collision_3_model.inc"
};
static NJS_OBJECT collision_3 = {
#include "stg08/terrain/collision_3.inc"
};
static NJS_MATERIAL collision_4_materials[1] = {
#include "stg08/terrain/collision_4_materials.inc"
};
static Sint16 collision_4_faces[33] = {
#include "stg08/terrain/collision_4_faces.inc"
};
static NJS_MESHSET collision_4_meshsets[1] = {
#include "stg08/terrain/collision_4_meshsets.inc"
};
static NJS_POINT3 collision_4_points[20] = {
#include "stg08/terrain/collision_4_points.inc"
};
static NJS_MODEL collision_4_model = {
#include "stg08/terrain/collision_4_model.inc"
};
static NJS_OBJECT collision_4 = {
#include "stg08/terrain/collision_4.inc"
};
static NJS_MATERIAL collision_10_materials[1] = {
#include "stg08/terrain/collision_10_materials.inc"
};
static Sint16 collision_10_faces[36] = {
#include "stg08/terrain/collision_10_faces.inc"
};
static NJS_MESHSET collision_10_meshsets[1] = {
#include "stg08/terrain/collision_10_meshsets.inc"
};
static NJS_POINT3 collision_10_points[20] = {
#include "stg08/terrain/collision_10_points.inc"
};
static NJS_MODEL collision_10_model = {
#include "stg08/terrain/collision_10_model.inc"
};
static NJS_OBJECT collision_10 = {
#include "stg08/terrain/collision_10.inc"
};
static NJS_MATERIAL collision_11_materials[1] = {
#include "stg08/terrain/collision_11_materials.inc"
};
static Sint16 collision_11_faces[124] = {
#include "stg08/terrain/collision_11_faces.inc"
};
static NJS_MESHSET collision_11_meshsets[1] = {
#include "stg08/terrain/collision_11_meshsets.inc"
};
static NJS_POINT3 collision_11_points[60] = {
#include "stg08/terrain/collision_11_points.inc"
};
static NJS_MODEL collision_11_model = {
#include "stg08/terrain/collision_11_model.inc"
};
static NJS_OBJECT collision_11 = {
#include "stg08/terrain/collision_11.inc"
};
static NJS_MATERIAL collision_12_materials[1] = {
#include "stg08/terrain/collision_12_materials.inc"
};
static Sint16 collision_12_faces[48] = {
#include "stg08/terrain/collision_12_faces.inc"
};
static NJS_MESHSET collision_12_meshsets[1] = {
#include "stg08/terrain/collision_12_meshsets.inc"
};
static NJS_POINT3 collision_12_points[36] = {
#include "stg08/terrain/collision_12_points.inc"
};
static NJS_MODEL collision_12_model = {
#include "stg08/terrain/collision_12_model.inc"
};
static NJS_OBJECT collision_12 = {
#include "stg08/terrain/collision_12.inc"
};
static NJS_MATERIAL collision_13_materials[1] = {
#include "stg08/terrain/collision_13_materials.inc"
};
static Sint16 collision_13_faces[115] = {
#include "stg08/terrain/collision_13_faces.inc"
};
static NJS_MESHSET collision_13_meshsets[1] = {
#include "stg08/terrain/collision_13_meshsets.inc"
};
static NJS_POINT3 collision_13_points[60] = {
#include "stg08/terrain/collision_13_points.inc"
};
static NJS_MODEL collision_13_model = {
#include "stg08/terrain/collision_13_model.inc"
};
static NJS_OBJECT collision_13 = {
#include "stg08/terrain/collision_13.inc"
};
static NJS_MATERIAL collision_14_materials[1] = {
#include "stg08/terrain/collision_14_materials.inc"
};
static Sint16 collision_14_faces[36] = {
#include "stg08/terrain/collision_14_faces.inc"
};
static NJS_MESHSET collision_14_meshsets[1] = {
#include "stg08/terrain/collision_14_meshsets.inc"
};
static NJS_POINT3 collision_14_points[20] = {
#include "stg08/terrain/collision_14_points.inc"
};
static NJS_MODEL collision_14_model = {
#include "stg08/terrain/collision_14_model.inc"
};
static NJS_OBJECT collision_14 = {
#include "stg08/terrain/collision_14.inc"
};
static NJS_MATERIAL collision_15_materials[1] = {
#include "stg08/terrain/collision_15_materials.inc"
};
static Sint16 collision_15_faces[106] = {
#include "stg08/terrain/collision_15_faces.inc"
};
static NJS_MESHSET collision_15_meshsets[1] = {
#include "stg08/terrain/collision_15_meshsets.inc"
};
static NJS_POINT3 collision_15_points[72] = {
#include "stg08/terrain/collision_15_points.inc"
};
static NJS_MODEL collision_15_model = {
#include "stg08/terrain/collision_15_model.inc"
};
static NJS_OBJECT collision_15 = {
#include "stg08/terrain/collision_15.inc"
};
static NJS_MATERIAL collision_16_materials[1] = {
#include "stg08/terrain/collision_16_materials.inc"
};
static Sint16 collision_16_faces[35] = {
#include "stg08/terrain/collision_16_faces.inc"
};
static NJS_MESHSET collision_16_meshsets[1] = {
#include "stg08/terrain/collision_16_meshsets.inc"
};
static NJS_POINT3 collision_16_points[20] = {
#include "stg08/terrain/collision_16_points.inc"
};
static NJS_MODEL collision_16_model = {
#include "stg08/terrain/collision_16_model.inc"
};
static NJS_OBJECT collision_16 = {
#include "stg08/terrain/collision_16.inc"
};
static NJS_MATERIAL collision_17_materials[1] = {
#include "stg08/terrain/collision_17_materials.inc"
};
static Sint16 collision_17_faces[27] = {
#include "stg08/terrain/collision_17_faces.inc"
};
static NJS_MESHSET collision_17_meshsets[1] = {
#include "stg08/terrain/collision_17_meshsets.inc"
};
static NJS_POINT3 collision_17_points[16] = {
#include "stg08/terrain/collision_17_points.inc"
};
static NJS_MODEL collision_17_model = {
#include "stg08/terrain/collision_17_model.inc"
};
static NJS_OBJECT collision_17 = {
#include "stg08/terrain/collision_17.inc"
};
static NJS_MATERIAL collision_19_materials[1] = {
#include "stg08/terrain/collision_19_materials.inc"
};
static Sint16 collision_19_faces[41] = {
#include "stg08/terrain/collision_19_faces.inc"
};
static NJS_MESHSET collision_19_meshsets[1] = {
#include "stg08/terrain/collision_19_meshsets.inc"
};
static NJS_POINT3 collision_19_points[22] = {
#include "stg08/terrain/collision_19_points.inc"
};
static NJS_MODEL collision_19_model = {
#include "stg08/terrain/collision_19_model.inc"
};
static NJS_OBJECT collision_19 = {
#include "stg08/terrain/collision_19.inc"
};
static NJS_MATERIAL collision_20_materials[1] = {
#include "stg08/terrain/collision_20_materials.inc"
};
static Sint16 collision_20_faces[47] = {
#include "stg08/terrain/collision_20_faces.inc"
};
static NJS_MESHSET collision_20_meshsets[1] = {
#include "stg08/terrain/collision_20_meshsets.inc"
};
static NJS_POINT3 collision_20_points[25] = {
#include "stg08/terrain/collision_20_points.inc"
};
static NJS_MODEL collision_20_model = {
#include "stg08/terrain/collision_20_model.inc"
};
static NJS_OBJECT collision_20 = {
#include "stg08/terrain/collision_20.inc"
};
static NJS_MATERIAL collision_21_materials[1] = {
#include "stg08/terrain/collision_21_materials.inc"
};
static Sint16 collision_21_faces[53] = {
#include "stg08/terrain/collision_21_faces.inc"
};
static NJS_MESHSET collision_21_meshsets[1] = {
#include "stg08/terrain/collision_21_meshsets.inc"
};
static NJS_POINT3 collision_21_points[29] = {
#include "stg08/terrain/collision_21_points.inc"
};
static NJS_MODEL collision_21_model = {
#include "stg08/terrain/collision_21_model.inc"
};
static NJS_OBJECT collision_21 = {
#include "stg08/terrain/collision_21.inc"
};
static NJS_MATERIAL collision_22_materials[1] = {
#include "stg08/terrain/collision_22_materials.inc"
};
static Sint16 collision_22_faces[27] = {
#include "stg08/terrain/collision_22_faces.inc"
};
static NJS_MESHSET collision_22_meshsets[1] = {
#include "stg08/terrain/collision_22_meshsets.inc"
};
static NJS_POINT3 collision_22_points[12] = {
#include "stg08/terrain/collision_22_points.inc"
};
static NJS_MODEL collision_22_model = {
#include "stg08/terrain/collision_22_model.inc"
};
static NJS_OBJECT collision_22 = {
#include "stg08/terrain/collision_22.inc"
};
static NJS_MATERIAL collision_23_materials[1] = {
#include "stg08/terrain/collision_23_materials.inc"
};
static Sint16 collision_23_faces[55] = {
#include "stg08/terrain/collision_23_faces.inc"
};
static NJS_MESHSET collision_23_meshsets[1] = {
#include "stg08/terrain/collision_23_meshsets.inc"
};
static NJS_POINT3 collision_23_points[40] = {
#include "stg08/terrain/collision_23_points.inc"
};
static NJS_MODEL collision_23_model = {
#include "stg08/terrain/collision_23_model.inc"
};
static NJS_OBJECT collision_23 = {
#include "stg08/terrain/collision_23.inc"
};
static NJS_MATERIAL collision_24_materials[1] = {
#include "stg08/terrain/collision_24_materials.inc"
};
static Sint16 collision_24_faces[29] = {
#include "stg08/terrain/collision_24_faces.inc"
};
static NJS_MESHSET collision_24_meshsets[1] = {
#include "stg08/terrain/collision_24_meshsets.inc"
};
static NJS_POINT3 collision_24_points[22] = {
#include "stg08/terrain/collision_24_points.inc"
};
static NJS_MODEL collision_24_model = {
#include "stg08/terrain/collision_24_model.inc"
};
static NJS_OBJECT collision_24 = {
#include "stg08/terrain/collision_24.inc"
};
static NJS_MATERIAL collision_25_materials[1] = {
#include "stg08/terrain/collision_25_materials.inc"
};
static Sint16 collision_25_faces[27] = {
#include "stg08/terrain/collision_25_faces.inc"
};
static NJS_MESHSET collision_25_meshsets[1] = {
#include "stg08/terrain/collision_25_meshsets.inc"
};
static NJS_POINT3 collision_25_points[12] = {
#include "stg08/terrain/collision_25_points.inc"
};
static NJS_MODEL collision_25_model = {
#include "stg08/terrain/collision_25_model.inc"
};
static NJS_OBJECT collision_25 = {
#include "stg08/terrain/collision_25.inc"
};
static NJS_MATERIAL collision_26_materials[1] = {
#include "stg08/terrain/collision_26_materials.inc"
};
static Sint16 collision_26_faces[58] = {
#include "stg08/terrain/collision_26_faces.inc"
};
static NJS_MESHSET collision_26_meshsets[1] = {
#include "stg08/terrain/collision_26_meshsets.inc"
};
static NJS_POINT3 collision_26_points[32] = {
#include "stg08/terrain/collision_26_points.inc"
};
static NJS_MODEL collision_26_model = {
#include "stg08/terrain/collision_26_model.inc"
};
static NJS_OBJECT collision_26 = {
#include "stg08/terrain/collision_26.inc"
};
static NJS_MATERIAL collision_29_materials[1] = {
#include "stg08/terrain/collision_29_materials.inc"
};
static Sint16 collision_29_faces[68] = {
#include "stg08/terrain/collision_29_faces.inc"
};
static NJS_MESHSET collision_29_meshsets[1] = {
#include "stg08/terrain/collision_29_meshsets.inc"
};
static NJS_POINT3 collision_29_points[40] = {
#include "stg08/terrain/collision_29_points.inc"
};
static NJS_MODEL collision_29_model = {
#include "stg08/terrain/collision_29_model.inc"
};
static NJS_OBJECT collision_29 = {
#include "stg08/terrain/collision_29.inc"
};
static NJS_MATERIAL collision_30_materials[1] = {
#include "stg08/terrain/collision_30_materials.inc"
};
static Sint16 collision_30_faces[82] = {
#include "stg08/terrain/collision_30_faces.inc"
};
static NJS_MESHSET collision_30_meshsets[1] = {
#include "stg08/terrain/collision_30_meshsets.inc"
};
static NJS_POINT3 collision_30_points[45] = {
#include "stg08/terrain/collision_30_points.inc"
};
static NJS_MODEL collision_30_model = {
#include "stg08/terrain/collision_30_model.inc"
};
static NJS_OBJECT collision_30 = {
#include "stg08/terrain/collision_30.inc"
};
static NJS_MATERIAL collision_31_materials[1] = {
#include "stg08/terrain/collision_31_materials.inc"
};
static Sint16 collision_31_faces[60] = {
#include "stg08/terrain/collision_31_faces.inc"
};
static NJS_MESHSET collision_31_meshsets[1] = {
#include "stg08/terrain/collision_31_meshsets.inc"
};
static NJS_POINT3 collision_31_points[35] = {
#include "stg08/terrain/collision_31_points.inc"
};
static NJS_MODEL collision_31_model = {
#include "stg08/terrain/collision_31_model.inc"
};
static NJS_OBJECT collision_31 = {
#include "stg08/terrain/collision_31.inc"
};
static NJS_MATERIAL collision_32_materials[1] = {
#include "stg08/terrain/collision_32_materials.inc"
};
static Sint16 collision_32_faces[68] = {
#include "stg08/terrain/collision_32_faces.inc"
};
static NJS_MESHSET collision_32_meshsets[1] = {
#include "stg08/terrain/collision_32_meshsets.inc"
};
static NJS_POINT3 collision_32_points[40] = {
#include "stg08/terrain/collision_32_points.inc"
};
static NJS_MODEL collision_32_model = {
#include "stg08/terrain/collision_32_model.inc"
};
static NJS_OBJECT collision_32 = {
#include "stg08/terrain/collision_32.inc"
};
static NJS_MATERIAL collision_33_materials[1] = {
#include "stg08/terrain/collision_33_materials.inc"
};
static Sint16 collision_33_faces[22] = {
#include "stg08/terrain/collision_33_faces.inc"
};
static NJS_MESHSET collision_33_meshsets[1] = {
#include "stg08/terrain/collision_33_meshsets.inc"
};
static NJS_POINT3 collision_33_points[12] = {
#include "stg08/terrain/collision_33_points.inc"
};
static NJS_MODEL collision_33_model = {
#include "stg08/terrain/collision_33_model.inc"
};
static NJS_OBJECT collision_33 = {
#include "stg08/terrain/collision_33.inc"
};
static NJS_MATERIAL collision_41_materials[1] = {
#include "stg08/terrain/collision_41_materials.inc"
};
static Sint16 collision_41_faces[56] = {
#include "stg08/terrain/collision_41_faces.inc"
};
static NJS_MESHSET collision_41_meshsets[1] = {
#include "stg08/terrain/collision_41_meshsets.inc"
};
static NJS_POINT3 collision_41_points[38] = {
#include "stg08/terrain/collision_41_points.inc"
};
static NJS_MODEL collision_41_model = {
#include "stg08/terrain/collision_41_model.inc"
};
static NJS_OBJECT collision_41 = {
#include "stg08/terrain/collision_41.inc"
};
static NJS_MATERIAL collision_42_materials[1] = {
#include "stg08/terrain/collision_42_materials.inc"
};
static Sint16 collision_42_faces[106] = {
#include "stg08/terrain/collision_42_faces.inc"
};
static NJS_MESHSET collision_42_meshsets[1] = {
#include "stg08/terrain/collision_42_meshsets.inc"
};
static NJS_POINT3 collision_42_points[56] = {
#include "stg08/terrain/collision_42_points.inc"
};
static NJS_MODEL collision_42_model = {
#include "stg08/terrain/collision_42_model.inc"
};
static NJS_OBJECT collision_42 = {
#include "stg08/terrain/collision_42.inc"
};
static NJS_MATERIAL collision_84_materials[1] = {
#include "stg08/terrain/collision_84_materials.inc"
};
static Sint16 collision_84_faces[124] = {
#include "stg08/terrain/collision_84_faces.inc"
};
static NJS_MESHSET collision_84_meshsets[1] = {
#include "stg08/terrain/collision_84_meshsets.inc"
};
static NJS_POINT3 collision_84_points[59] = {
#include "stg08/terrain/collision_84_points.inc"
};
static NJS_MODEL collision_84_model = {
#include "stg08/terrain/collision_84_model.inc"
};
static NJS_OBJECT collision_84 = {
#include "stg08/terrain/collision_84.inc"
};
static NJS_MATERIAL collision_85_materials[1] = {
#include "stg08/terrain/collision_85_materials.inc"
};
static Sint16 collision_85_faces[60] = {
#include "stg08/terrain/collision_85_faces.inc"
};
static NJS_MESHSET collision_85_meshsets[1] = {
#include "stg08/terrain/collision_85_meshsets.inc"
};
static NJS_POINT3 collision_85_points[35] = {
#include "stg08/terrain/collision_85_points.inc"
};
static NJS_MODEL collision_85_model = {
#include "stg08/terrain/collision_85_model.inc"
};
static NJS_OBJECT collision_85 = {
#include "stg08/terrain/collision_85.inc"
};
static NJS_MATERIAL collision_86_materials[1] = {
#include "stg08/terrain/collision_86_materials.inc"
};
static Sint16 collision_86_faces[106] = {
#include "stg08/terrain/collision_86_faces.inc"
};
static NJS_MESHSET collision_86_meshsets[1] = {
#include "stg08/terrain/collision_86_meshsets.inc"
};
static NJS_POINT3 collision_86_points[59] = {
#include "stg08/terrain/collision_86_points.inc"
};
static NJS_MODEL collision_86_model = {
#include "stg08/terrain/collision_86_model.inc"
};
static NJS_OBJECT collision_86 = {
#include "stg08/terrain/collision_86.inc"
};
static NJS_MATERIAL collision_87_materials[1] = {
#include "stg08/terrain/collision_87_materials.inc"
};
static Sint16 collision_87_faces[17] = {
#include "stg08/terrain/collision_87_faces.inc"
};
static NJS_MESHSET collision_87_meshsets[1] = {
#include "stg08/terrain/collision_87_meshsets.inc"
};
static NJS_POINT3 collision_87_points[16] = {
#include "stg08/terrain/collision_87_points.inc"
};
static NJS_MODEL collision_87_model = {
#include "stg08/terrain/collision_87_model.inc"
};
static NJS_OBJECT collision_87 = {
#include "stg08/terrain/collision_87.inc"
};
static NJS_MATERIAL collision_88_materials[1] = {
#include "stg08/terrain/collision_88_materials.inc"
};
static Sint16 collision_88_faces[15] = {
#include "stg08/terrain/collision_88_faces.inc"
};
static NJS_MESHSET collision_88_meshsets[1] = {
#include "stg08/terrain/collision_88_meshsets.inc"
};
static NJS_POINT3 collision_88_points[14] = {
#include "stg08/terrain/collision_88_points.inc"
};
static NJS_MODEL collision_88_model = {
#include "stg08/terrain/collision_88_model.inc"
};
static NJS_OBJECT collision_88 = {
#include "stg08/terrain/collision_88.inc"
};
static NJS_MATERIAL collision_89_materials[1] = {
#include "stg08/terrain/collision_89_materials.inc"
};
static Sint16 collision_89_faces[17] = {
#include "stg08/terrain/collision_89_faces.inc"
};
static NJS_MESHSET collision_89_meshsets[1] = {
#include "stg08/terrain/collision_89_meshsets.inc"
};
static NJS_POINT3 collision_89_points[16] = {
#include "stg08/terrain/collision_89_points.inc"
};
static NJS_MODEL collision_89_model = {
#include "stg08/terrain/collision_89_model.inc"
};
static NJS_OBJECT collision_89 = {
#include "stg08/terrain/collision_89.inc"
};
static NJS_MATERIAL collision_90_materials[1] = {
#include "stg08/terrain/collision_90_materials.inc"
};
static Sint16 collision_90_faces[15] = {
#include "stg08/terrain/collision_90_faces.inc"
};
static NJS_MESHSET collision_90_meshsets[1] = {
#include "stg08/terrain/collision_90_meshsets.inc"
};
static NJS_POINT3 collision_90_points[14] = {
#include "stg08/terrain/collision_90_points.inc"
};
static NJS_MODEL collision_90_model = {
#include "stg08/terrain/collision_90_model.inc"
};
static NJS_OBJECT collision_90 = {
#include "stg08/terrain/collision_90.inc"
};
static NJS_MATERIAL collision_91_materials[1] = {
#include "stg08/terrain/collision_91_materials.inc"
};
static Sint16 collision_91_faces[32] = {
#include "stg08/terrain/collision_91_faces.inc"
};
static NJS_MESHSET collision_91_meshsets[1] = {
#include "stg08/terrain/collision_91_meshsets.inc"
};
static NJS_POINT3 collision_91_points[16] = {
#include "stg08/terrain/collision_91_points.inc"
};
static NJS_MODEL collision_91_model = {
#include "stg08/terrain/collision_91_model.inc"
};
static NJS_OBJECT collision_91 = {
#include "stg08/terrain/collision_91.inc"
};
static NJS_MATERIAL collision_93_materials[1] = {
#include "stg08/terrain/collision_93_materials.inc"
};
static Sint16 collision_93_faces[26] = {
#include "stg08/terrain/collision_93_faces.inc"
};
static NJS_MESHSET collision_93_meshsets[1] = {
#include "stg08/terrain/collision_93_meshsets.inc"
};
static NJS_POINT3 collision_93_points[12] = {
#include "stg08/terrain/collision_93_points.inc"
};
static NJS_MODEL collision_93_model = {
#include "stg08/terrain/collision_93_model.inc"
};
static NJS_OBJECT collision_93 = {
#include "stg08/terrain/collision_93.inc"
};
static NJS_MATERIAL collision_95_materials[1] = {
#include "stg08/terrain/collision_95_materials.inc"
};
static Sint16 collision_95_faces[32] = {
#include "stg08/terrain/collision_95_faces.inc"
};
static NJS_MESHSET collision_95_meshsets[1] = {
#include "stg08/terrain/collision_95_meshsets.inc"
};
static NJS_POINT3 collision_95_points[16] = {
#include "stg08/terrain/collision_95_points.inc"
};
static NJS_MODEL collision_95_model = {
#include "stg08/terrain/collision_95_model.inc"
};
static NJS_OBJECT collision_95 = {
#include "stg08/terrain/collision_95.inc"
};
static NJS_MATERIAL collision_98_materials[1] = {
#include "stg08/terrain/collision_98_materials.inc"
};
static Sint16 collision_98_faces[28] = {
#include "stg08/terrain/collision_98_faces.inc"
};
static NJS_MESHSET collision_98_meshsets[1] = {
#include "stg08/terrain/collision_98_meshsets.inc"
};
static NJS_POINT3 collision_98_points[16] = {
#include "stg08/terrain/collision_98_points.inc"
};
static NJS_MODEL collision_98_model = {
#include "stg08/terrain/collision_98_model.inc"
};
static NJS_OBJECT collision_98 = {
#include "stg08/terrain/collision_98.inc"
};
static NJS_MATERIAL collision_100_materials[1] = {
#include "stg08/terrain/collision_100_materials.inc"
};
static Sint16 collision_100_faces[56] = {
#include "stg08/terrain/collision_100_faces.inc"
};
static NJS_MESHSET collision_100_meshsets[1] = {
#include "stg08/terrain/collision_100_meshsets.inc"
};
static NJS_POINT3 collision_100_points[24] = {
#include "stg08/terrain/collision_100_points.inc"
};
static NJS_MODEL collision_100_model = {
#include "stg08/terrain/collision_100_model.inc"
};
static NJS_OBJECT collision_100 = {
#include "stg08/terrain/collision_100.inc"
};
static NJS_CNK_OBJECT draw_268 = {
#include "stg08/terrain/draw_268.inc"
};
static NJS_CNK_OBJECT draw_266 = {
#include "stg08/terrain/draw_266.inc"
};
static NJS_CNK_OBJECT draw_265 = {
#include "stg08/terrain/draw_265.inc"
};
static NJS_CNK_OBJECT draw_264 = {
#include "stg08/terrain/draw_264.inc"
};
static NJS_CNK_OBJECT draw_263 = {
#include "stg08/terrain/draw_263.inc"
};
static NJS_CNK_OBJECT draw_262 = {
#include "stg08/terrain/draw_262.inc"
};
static NJS_CNK_OBJECT draw_261 = {
#include "stg08/terrain/draw_261.inc"
};
static NJS_CNK_OBJECT draw_260 = {
#include "stg08/terrain/draw_260.inc"
};
static NJS_CNK_OBJECT draw_259 = {
#include "stg08/terrain/draw_259.inc"
};
static NJS_CNK_OBJECT draw_258 = {
#include "stg08/terrain/draw_258.inc"
};
static NJS_CNK_OBJECT draw_257 = {
#include "stg08/terrain/draw_257.inc"
};
static NJS_CNK_OBJECT draw_256 = {
#include "stg08/terrain/draw_256.inc"
};
static NJS_CNK_OBJECT draw_255 = {
#include "stg08/terrain/draw_255.inc"
};
static NJS_CNK_OBJECT draw_254 = {
#include "stg08/terrain/draw_254.inc"
};
static NJS_CNK_OBJECT draw_253 = {
#include "stg08/terrain/draw_253.inc"
};
static NJS_CNK_OBJECT draw_252 = {
#include "stg08/terrain/draw_252.inc"
};
static NJS_CNK_OBJECT draw_251 = {
#include "stg08/terrain/draw_251.inc"
};
static NJS_CNK_OBJECT draw_250 = {
#include "stg08/terrain/draw_250.inc"
};
static NJS_CNK_OBJECT draw_249 = {
#include "stg08/terrain/draw_249.inc"
};
static NJS_CNK_OBJECT draw_248 = {
#include "stg08/terrain/draw_248.inc"
};
static NJS_CNK_OBJECT draw_247 = {
#include "stg08/terrain/draw_247.inc"
};
static NJS_CNK_OBJECT draw_246 = {
#include "stg08/terrain/draw_246.inc"
};
static NJS_CNK_OBJECT draw_245 = {
#include "stg08/terrain/draw_245.inc"
};
static NJS_CNK_OBJECT draw_244 = {
#include "stg08/terrain/draw_244.inc"
};
static NJS_CNK_OBJECT draw_243 = {
#include "stg08/terrain/draw_243.inc"
};
static NJS_CNK_OBJECT draw_242 = {
#include "stg08/terrain/draw_242.inc"
};
static NJS_CNK_OBJECT draw_241 = {
#include "stg08/terrain/draw_241.inc"
};
static NJS_CNK_OBJECT draw_240 = {
#include "stg08/terrain/draw_240.inc"
};
static NJS_CNK_OBJECT draw_239 = {
#include "stg08/terrain/draw_239.inc"
};
static NJS_CNK_OBJECT draw_236 = {
#include "stg08/terrain/draw_236.inc"
};
static NJS_CNK_OBJECT draw_235 = {
#include "stg08/terrain/draw_235.inc"
};
static NJS_CNK_OBJECT draw_234 = {
#include "stg08/terrain/draw_234.inc"
};
static NJS_CNK_OBJECT draw_233 = {
#include "stg08/terrain/draw_233.inc"
};
static NJS_CNK_OBJECT draw_232 = {
#include "stg08/terrain/draw_232.inc"
};
static NJS_CNK_OBJECT draw_231 = {
#include "stg08/terrain/draw_231.inc"
};
static NJS_CNK_OBJECT draw_230 = {
#include "stg08/terrain/draw_230.inc"
};
static NJS_CNK_OBJECT draw_229 = {
#include "stg08/terrain/draw_229.inc"
};
static NJS_CNK_OBJECT draw_228 = {
#include "stg08/terrain/draw_228.inc"
};
static NJS_CNK_OBJECT draw_227 = {
#include "stg08/terrain/draw_227.inc"
};
static NJS_CNK_OBJECT draw_226 = {
#include "stg08/terrain/draw_226.inc"
};
static NJS_CNK_OBJECT draw_225 = {
#include "stg08/terrain/draw_225.inc"
};
static NJS_CNK_OBJECT draw_224 = {
#include "stg08/terrain/draw_224.inc"
};
static NJS_CNK_OBJECT draw_223 = {
#include "stg08/terrain/draw_223.inc"
};
static NJS_CNK_OBJECT draw_222 = {
#include "stg08/terrain/draw_222.inc"
};
static NJS_CNK_OBJECT draw_221 = {
#include "stg08/terrain/draw_221.inc"
};
static NJS_CNK_OBJECT draw_220 = {
#include "stg08/terrain/draw_220.inc"
};
static NJS_CNK_OBJECT draw_219 = {
#include "stg08/terrain/draw_219.inc"
};
static NJS_CNK_OBJECT draw_218 = {
#include "stg08/terrain/draw_218.inc"
};
static NJS_CNK_OBJECT draw_217 = {
#include "stg08/terrain/draw_217.inc"
};
static NJS_CNK_OBJECT draw_216 = {
#include "stg08/terrain/draw_216.inc"
};
static NJS_CNK_OBJECT draw_215 = {
#include "stg08/terrain/draw_215.inc"
};
static NJS_CNK_OBJECT draw_214 = {
#include "stg08/terrain/draw_214.inc"
};
static NJS_CNK_OBJECT draw_213 = {
#include "stg08/terrain/draw_213.inc"
};
static NJS_CNK_OBJECT draw_212 = {
#include "stg08/terrain/draw_212.inc"
};
static NJS_CNK_OBJECT draw_211 = {
#include "stg08/terrain/draw_211.inc"
};
static NJS_CNK_OBJECT draw_210 = {
#include "stg08/terrain/draw_210.inc"
};
static NJS_CNK_OBJECT draw_209 = {
#include "stg08/terrain/draw_209.inc"
};
static NJS_CNK_OBJECT draw_206 = {
#include "stg08/terrain/draw_206.inc"
};
static NJS_CNK_OBJECT draw_205 = {
#include "stg08/terrain/draw_205.inc"
};
static NJS_CNK_OBJECT draw_204 = {
#include "stg08/terrain/draw_204.inc"
};
static NJS_CNK_OBJECT draw_203 = {
#include "stg08/terrain/draw_203.inc"
};
static NJS_CNK_OBJECT draw_202 = {
#include "stg08/terrain/draw_202.inc"
};
static NJS_CNK_OBJECT draw_201 = {
#include "stg08/terrain/draw_201.inc"
};
static NJS_CNK_OBJECT draw_200 = {
#include "stg08/terrain/draw_200.inc"
};
static NJS_CNK_OBJECT draw_199 = {
#include "stg08/terrain/draw_199.inc"
};
static NJS_CNK_OBJECT draw_198 = {
#include "stg08/terrain/draw_198.inc"
};
static NJS_CNK_OBJECT draw_197 = {
#include "stg08/terrain/draw_197.inc"
};
static NJS_CNK_OBJECT draw_196 = {
#include "stg08/terrain/draw_196.inc"
};
static NJS_CNK_OBJECT draw_195 = {
#include "stg08/terrain/draw_195.inc"
};
static NJS_CNK_OBJECT draw_194 = {
#include "stg08/terrain/draw_194.inc"
};
static NJS_CNK_OBJECT draw_193 = {
#include "stg08/terrain/draw_193.inc"
};
static NJS_CNK_OBJECT draw_192 = {
#include "stg08/terrain/draw_192.inc"
};
static NJS_CNK_OBJECT draw_191 = {
#include "stg08/terrain/draw_191.inc"
};
static NJS_CNK_OBJECT draw_190 = {
#include "stg08/terrain/draw_190.inc"
};
static NJS_CNK_OBJECT draw_189 = {
#include "stg08/terrain/draw_189.inc"
};
static NJS_CNK_OBJECT draw_188 = {
#include "stg08/terrain/draw_188.inc"
};
static NJS_CNK_OBJECT draw_187 = {
#include "stg08/terrain/draw_187.inc"
};
static NJS_CNK_OBJECT draw_186 = {
#include "stg08/terrain/draw_186.inc"
};
static NJS_CNK_OBJECT draw_185 = {
#include "stg08/terrain/draw_185.inc"
};
static NJS_CNK_OBJECT draw_184 = {
#include "stg08/terrain/draw_184.inc"
};
static NJS_CNK_OBJECT draw_183 = {
#include "stg08/terrain/draw_183.inc"
};
static NJS_CNK_OBJECT draw_182 = {
#include "stg08/terrain/draw_182.inc"
};
static NJS_CNK_OBJECT draw_181 = {
#include "stg08/terrain/draw_181.inc"
};
static NJS_CNK_OBJECT draw_180 = {
#include "stg08/terrain/draw_180.inc"
};
static NJS_CNK_OBJECT draw_179 = {
#include "stg08/terrain/draw_179.inc"
};
static NJS_CNK_OBJECT draw_177 = {
#include "stg08/terrain/draw_177.inc"
};
static NJS_CNK_OBJECT draw_176 = {
#include "stg08/terrain/draw_176.inc"
};
static NJS_CNK_OBJECT draw_175 = {
#include "stg08/terrain/draw_175.inc"
};
static NJS_CNK_OBJECT draw_174 = {
#include "stg08/terrain/draw_174.inc"
};
static NJS_CNK_OBJECT draw_173 = {
#include "stg08/terrain/draw_173.inc"
};
static NJS_CNK_OBJECT draw_172 = {
#include "stg08/terrain/draw_172.inc"
};
static NJS_CNK_OBJECT draw_171 = {
#include "stg08/terrain/draw_171.inc"
};
static NJS_CNK_OBJECT draw_170 = {
#include "stg08/terrain/draw_170.inc"
};
static NJS_CNK_OBJECT draw_169 = {
#include "stg08/terrain/draw_169.inc"
};
static NJS_CNK_OBJECT draw_168 = {
#include "stg08/terrain/draw_168.inc"
};
static NJS_CNK_OBJECT draw_167 = {
#include "stg08/terrain/draw_167.inc"
};
static NJS_CNK_OBJECT draw_166 = {
#include "stg08/terrain/draw_166.inc"
};
static NJS_CNK_OBJECT draw_165 = {
#include "stg08/terrain/draw_165.inc"
};
static NJS_CNK_OBJECT draw_164 = {
#include "stg08/terrain/draw_164.inc"
};
static NJS_CNK_OBJECT draw_163 = {
#include "stg08/terrain/draw_163.inc"
};
static NJS_CNK_OBJECT draw_162 = {
#include "stg08/terrain/draw_162.inc"
};
static NJS_CNK_OBJECT draw_161 = {
#include "stg08/terrain/draw_161.inc"
};
static NJS_CNK_OBJECT draw_160 = {
#include "stg08/terrain/draw_160.inc"
};
static NJS_CNK_OBJECT draw_159 = {
#include "stg08/terrain/draw_159.inc"
};
static NJS_CNK_OBJECT draw_158 = {
#include "stg08/terrain/draw_158.inc"
};
static NJS_CNK_OBJECT draw_157 = {
#include "stg08/terrain/draw_157.inc"
};
static NJS_CNK_OBJECT draw_156 = {
#include "stg08/terrain/draw_156.inc"
};
static NJS_CNK_OBJECT draw_155 = {
#include "stg08/terrain/draw_155.inc"
};
static NJS_CNK_OBJECT draw_154 = {
#include "stg08/terrain/draw_154.inc"
};
static NJS_CNK_OBJECT draw_153 = {
#include "stg08/terrain/draw_153.inc"
};
static NJS_CNK_OBJECT draw_152 = {
#include "stg08/terrain/draw_152.inc"
};
static NJS_CNK_OBJECT draw_151 = {
#include "stg08/terrain/draw_151.inc"
};
static NJS_CNK_OBJECT draw_150 = {
#include "stg08/terrain/draw_150.inc"
};
static NJS_CNK_OBJECT draw_148 = {
#include "stg08/terrain/draw_148.inc"
};
static NJS_CNK_OBJECT draw_147 = {
#include "stg08/terrain/draw_147.inc"
};
static NJS_CNK_OBJECT draw_146 = {
#include "stg08/terrain/draw_146.inc"
};
static NJS_CNK_OBJECT draw_145 = {
#include "stg08/terrain/draw_145.inc"
};
static NJS_CNK_OBJECT draw_144 = {
#include "stg08/terrain/draw_144.inc"
};
static NJS_CNK_OBJECT draw_143 = {
#include "stg08/terrain/draw_143.inc"
};
static NJS_CNK_OBJECT draw_142 = {
#include "stg08/terrain/draw_142.inc"
};
static NJS_CNK_OBJECT draw_141 = {
#include "stg08/terrain/draw_141.inc"
};
static NJS_CNK_OBJECT draw_140 = {
#include "stg08/terrain/draw_140.inc"
};
static NJS_CNK_OBJECT draw_139 = {
#include "stg08/terrain/draw_139.inc"
};
static NJS_CNK_OBJECT draw_138 = {
#include "stg08/terrain/draw_138.inc"
};
static NJS_CNK_OBJECT draw_137 = {
#include "stg08/terrain/draw_137.inc"
};
static NJS_CNK_OBJECT draw_136 = {
#include "stg08/terrain/draw_136.inc"
};
static NJS_CNK_OBJECT draw_135 = {
#include "stg08/terrain/draw_135.inc"
};
static NJS_CNK_OBJECT draw_134 = {
#include "stg08/terrain/draw_134.inc"
};
static NJS_CNK_OBJECT draw_133 = {
#include "stg08/terrain/draw_133.inc"
};
static NJS_CNK_OBJECT draw_132 = {
#include "stg08/terrain/draw_132.inc"
};
static NJS_CNK_OBJECT draw_131 = {
#include "stg08/terrain/draw_131.inc"
};
static NJS_CNK_OBJECT draw_130 = {
#include "stg08/terrain/draw_130.inc"
};
static NJS_CNK_OBJECT draw_129 = {
#include "stg08/terrain/draw_129.inc"
};
static NJS_CNK_OBJECT draw_128 = {
#include "stg08/terrain/draw_128.inc"
};
static NJS_CNK_OBJECT draw_127 = {
#include "stg08/terrain/draw_127.inc"
};
static NJS_CNK_OBJECT draw_126 = {
#include "stg08/terrain/draw_126.inc"
};
static NJS_CNK_OBJECT draw_125 = {
#include "stg08/terrain/draw_125.inc"
};
static NJS_CNK_OBJECT draw_124 = {
#include "stg08/terrain/draw_124.inc"
};
static NJS_CNK_OBJECT draw_123 = {
#include "stg08/terrain/draw_123.inc"
};
static NJS_CNK_OBJECT draw_122 = {
#include "stg08/terrain/draw_122.inc"
};
static NJS_CNK_OBJECT draw_121 = {
#include "stg08/terrain/draw_121.inc"
};
static NJS_CNK_OBJECT draw_119 = {
#include "stg08/terrain/draw_119.inc"
};
static NJS_CNK_OBJECT draw_118 = {
#include "stg08/terrain/draw_118.inc"
};
static NJS_CNK_OBJECT draw_117 = {
#include "stg08/terrain/draw_117.inc"
};
static NJS_CNK_OBJECT draw_116 = {
#include "stg08/terrain/draw_116.inc"
};
static NJS_CNK_OBJECT draw_115 = {
#include "stg08/terrain/draw_115.inc"
};
static NJS_CNK_OBJECT draw_114 = {
#include "stg08/terrain/draw_114.inc"
};
static NJS_CNK_OBJECT draw_113 = {
#include "stg08/terrain/draw_113.inc"
};
static NJS_CNK_OBJECT draw_112 = {
#include "stg08/terrain/draw_112.inc"
};
static NJS_CNK_OBJECT draw_111 = {
#include "stg08/terrain/draw_111.inc"
};
static NJS_CNK_OBJECT draw_110 = {
#include "stg08/terrain/draw_110.inc"
};
static NJS_CNK_OBJECT draw_109 = {
#include "stg08/terrain/draw_109.inc"
};
static NJS_CNK_OBJECT draw_108 = {
#include "stg08/terrain/draw_108.inc"
};
static NJS_CNK_OBJECT draw_107 = {
#include "stg08/terrain/draw_107.inc"
};
static NJS_CNK_OBJECT draw_106 = {
#include "stg08/terrain/draw_106.inc"
};
static NJS_CNK_OBJECT draw_105 = {
#include "stg08/terrain/draw_105.inc"
};
static NJS_CNK_OBJECT draw_104 = {
#include "stg08/terrain/draw_104.inc"
};
static NJS_CNK_OBJECT draw_103 = {
#include "stg08/terrain/draw_103.inc"
};
static NJS_CNK_OBJECT draw_102 = {
#include "stg08/terrain/draw_102.inc"
};
static NJS_CNK_OBJECT draw_101 = {
#include "stg08/terrain/draw_101.inc"
};
static NJS_CNK_OBJECT draw_100 = {
#include "stg08/terrain/draw_100.inc"
};
static NJS_CNK_OBJECT draw_99 = {
#include "stg08/terrain/draw_99.inc"
};
static NJS_CNK_OBJECT draw_98 = {
#include "stg08/terrain/draw_98.inc"
};
static NJS_CNK_OBJECT draw_97 = {
#include "stg08/terrain/draw_97.inc"
};
static NJS_CNK_OBJECT draw_96 = {
#include "stg08/terrain/draw_96.inc"
};
static NJS_CNK_OBJECT draw_95 = {
#include "stg08/terrain/draw_95.inc"
};
static NJS_CNK_OBJECT draw_94 = {
#include "stg08/terrain/draw_94.inc"
};
static NJS_CNK_OBJECT draw_93 = {
#include "stg08/terrain/draw_93.inc"
};
static NJS_CNK_OBJECT draw_92 = {
#include "stg08/terrain/draw_92.inc"
};
static NJS_CNK_OBJECT draw_90 = {
#include "stg08/terrain/draw_90.inc"
};
static NJS_CNK_OBJECT draw_88 = {
#include "stg08/terrain/draw_88.inc"
};
static NJS_CNK_OBJECT draw_87 = {
#include "stg08/terrain/draw_87.inc"
};
static NJS_CNK_OBJECT draw_86 = {
#include "stg08/terrain/draw_86.inc"
};
static NJS_CNK_OBJECT draw_85 = {
#include "stg08/terrain/draw_85.inc"
};
static NJS_CNK_OBJECT draw_84 = {
#include "stg08/terrain/draw_84.inc"
};
static NJS_CNK_OBJECT draw_83 = {
#include "stg08/terrain/draw_83.inc"
};
static NJS_CNK_OBJECT draw_82 = {
#include "stg08/terrain/draw_82.inc"
};
static NJS_CNK_OBJECT draw_80 = {
#include "stg08/terrain/draw_80.inc"
};
static NJS_CNK_OBJECT draw_79 = {
#include "stg08/terrain/draw_79.inc"
};
static NJS_CNK_OBJECT draw_78 = {
#include "stg08/terrain/draw_78.inc"
};
static NJS_CNK_OBJECT draw_75 = {
#include "stg08/terrain/draw_75.inc"
};
static NJS_CNK_OBJECT draw_72 = {
#include "stg08/terrain/draw_72.inc"
};
static NJS_CNK_OBJECT draw_71 = {
#include "stg08/terrain/draw_71.inc"
};
static NJS_CNK_OBJECT draw_70 = {
#include "stg08/terrain/draw_70.inc"
};
static NJS_CNK_OBJECT draw_68 = {
#include "stg08/terrain/draw_68.inc"
};
static NJS_CNK_OBJECT draw_65 = {
#include "stg08/terrain/draw_65.inc"
};
static NJS_CNK_OBJECT draw_55 = {
#include "stg08/terrain/draw_55.inc"
};
static NJS_CNK_OBJECT draw_54 = {
#include "stg08/terrain/draw_54.inc"
};
static NJS_CNK_OBJECT draw_53 = {
#include "stg08/terrain/draw_53.inc"
};
static NJS_CNK_OBJECT draw_52 = {
#include "stg08/terrain/draw_52.inc"
};
static NJS_CNK_OBJECT draw_51 = {
#include "stg08/terrain/draw_51.inc"
};
static NJS_CNK_OBJECT draw_50 = {
#include "stg08/terrain/draw_50.inc"
};
static NJS_CNK_OBJECT draw_49 = {
#include "stg08/terrain/draw_49.inc"
};
static NJS_CNK_OBJECT draw_46 = {
#include "stg08/terrain/draw_46.inc"
};
static NJS_CNK_OBJECT draw_44 = {
#include "stg08/terrain/draw_44.inc"
};
static NJS_CNK_OBJECT draw_31 = {
#include "stg08/terrain/draw_31.inc"
};
static NJS_CNK_OBJECT draw_29 = {
#include "stg08/terrain/draw_29.inc"
};
static NJS_CNK_OBJECT draw_27 = {
#include "stg08/terrain/draw_27.inc"
};
static NJS_CNK_OBJECT draw_26 = {
#include "stg08/terrain/draw_26.inc"
};
static NJS_CNK_OBJECT draw_25 = {
#include "stg08/terrain/draw_25.inc"
};
static NJS_CNK_OBJECT draw_24 = {
#include "stg08/terrain/draw_24.inc"
};
static NJS_CNK_OBJECT draw_23 = {
#include "stg08/terrain/draw_23.inc"
};
static NJS_CNK_OBJECT draw_21 = {
#include "stg08/terrain/draw_21.inc"
};
static NJS_CNK_OBJECT draw_20 = {
#include "stg08/terrain/draw_20.inc"
};
static NJS_CNK_OBJECT draw_18 = {
#include "stg08/terrain/draw_18.inc"
};
static NJS_CNK_OBJECT draw_16 = {
#include "stg08/terrain/draw_16.inc"
};
static NJS_CNK_OBJECT draw_15 = {
#include "stg08/terrain/draw_15.inc"
};
static NJS_CNK_OBJECT draw_13 = {
#include "stg08/terrain/draw_13.inc"
};
static NJS_CNK_OBJECT draw_12 = {
#include "stg08/terrain/draw_12.inc"
};
static NJS_CNK_OBJECT draw_11 = {
#include "stg08/terrain/draw_11.inc"
};
static NJS_CNK_OBJECT draw_10 = {
#include "stg08/terrain/draw_10.inc"
};
static NJS_CNK_OBJECT draw_9 = {
#include "stg08/terrain/draw_9.inc"
};
static NJS_CNK_OBJECT draw_8 = {
#include "stg08/terrain/draw_8.inc"
};
static NJS_CNK_OBJECT draw_7 = {
#include "stg08/terrain/draw_7.inc"
};
static NJS_CNK_OBJECT draw_5 = {
#include "stg08/terrain/draw_5.inc"
};
static NJS_CNK_OBJECT draw_4 = {
#include "stg08/terrain/draw_4.inc"
};
static NJS_CNK_OBJECT draw_1 = {
#include "stg08/terrain/draw_1.inc"
};
static NJS_OBJECT collision_1 = {
#include "stg08/terrain/collision_1.inc"
};
static NJS_OBJECT collision_2 = {
#include "stg08/terrain/collision_2.inc"
};
static NJS_OBJECT collision_5 = {
#include "stg08/terrain/collision_5.inc"
};
static NJS_OBJECT collision_6 = {
#include "stg08/terrain/collision_6.inc"
};
static NJS_OBJECT collision_7 = {
#include "stg08/terrain/collision_7.inc"
};
static NJS_OBJECT collision_8 = {
#include "stg08/terrain/collision_8.inc"
};
static NJS_OBJECT collision_9 = {
#include "stg08/terrain/collision_9.inc"
};
static NJS_OBJECT collision_18 = {
#include "stg08/terrain/collision_18.inc"
};
static NJS_OBJECT collision_27 = {
#include "stg08/terrain/collision_27.inc"
};
static NJS_OBJECT collision_28 = {
#include "stg08/terrain/collision_28.inc"
};
static NJS_OBJECT collision_34 = {
#include "stg08/terrain/collision_34.inc"
};
static NJS_OBJECT collision_35 = {
#include "stg08/terrain/collision_35.inc"
};
static NJS_OBJECT collision_36 = {
#include "stg08/terrain/collision_36.inc"
};
static NJS_OBJECT collision_37 = {
#include "stg08/terrain/collision_37.inc"
};
static NJS_OBJECT collision_38 = {
#include "stg08/terrain/collision_38.inc"
};
static NJS_OBJECT collision_39 = {
#include "stg08/terrain/collision_39.inc"
};
static NJS_OBJECT collision_40 = {
#include "stg08/terrain/collision_40.inc"
};
static NJS_OBJECT collision_43 = {
#include "stg08/terrain/collision_43.inc"
};
static NJS_OBJECT collision_44 = {
#include "stg08/terrain/collision_44.inc"
};
static NJS_OBJECT collision_45 = {
#include "stg08/terrain/collision_45.inc"
};
static NJS_OBJECT collision_46 = {
#include "stg08/terrain/collision_46.inc"
};
static NJS_OBJECT collision_47 = {
#include "stg08/terrain/collision_47.inc"
};
static NJS_OBJECT collision_48 = {
#include "stg08/terrain/collision_48.inc"
};
static NJS_OBJECT collision_49 = {
#include "stg08/terrain/collision_49.inc"
};
static NJS_OBJECT collision_50 = {
#include "stg08/terrain/collision_50.inc"
};
static NJS_OBJECT collision_51 = {
#include "stg08/terrain/collision_51.inc"
};
static NJS_OBJECT collision_52 = {
#include "stg08/terrain/collision_52.inc"
};
static NJS_OBJECT collision_53 = {
#include "stg08/terrain/collision_53.inc"
};
static NJS_OBJECT collision_54 = {
#include "stg08/terrain/collision_54.inc"
};
static NJS_OBJECT collision_55 = {
#include "stg08/terrain/collision_55.inc"
};
static NJS_OBJECT collision_56 = {
#include "stg08/terrain/collision_56.inc"
};
static NJS_OBJECT collision_57 = {
#include "stg08/terrain/collision_57.inc"
};
static NJS_OBJECT collision_58 = {
#include "stg08/terrain/collision_58.inc"
};
static NJS_OBJECT collision_59 = {
#include "stg08/terrain/collision_59.inc"
};
static NJS_OBJECT collision_60 = {
#include "stg08/terrain/collision_60.inc"
};
static NJS_OBJECT collision_61 = {
#include "stg08/terrain/collision_61.inc"
};
static NJS_OBJECT collision_62 = {
#include "stg08/terrain/collision_62.inc"
};
static NJS_OBJECT collision_63 = {
#include "stg08/terrain/collision_63.inc"
};
static NJS_OBJECT collision_64 = {
#include "stg08/terrain/collision_64.inc"
};
static NJS_OBJECT collision_65 = {
#include "stg08/terrain/collision_65.inc"
};
static NJS_OBJECT collision_66 = {
#include "stg08/terrain/collision_66.inc"
};
static NJS_OBJECT collision_67 = {
#include "stg08/terrain/collision_67.inc"
};
static NJS_OBJECT collision_68 = {
#include "stg08/terrain/collision_68.inc"
};
static NJS_OBJECT collision_69 = {
#include "stg08/terrain/collision_69.inc"
};
static NJS_OBJECT collision_70 = {
#include "stg08/terrain/collision_70.inc"
};
static NJS_OBJECT collision_71 = {
#include "stg08/terrain/collision_71.inc"
};
static NJS_OBJECT collision_72 = {
#include "stg08/terrain/collision_72.inc"
};
static NJS_OBJECT collision_73 = {
#include "stg08/terrain/collision_73.inc"
};
static NJS_OBJECT collision_74 = {
#include "stg08/terrain/collision_74.inc"
};
static NJS_OBJECT collision_75 = {
#include "stg08/terrain/collision_75.inc"
};
static NJS_OBJECT collision_76 = {
#include "stg08/terrain/collision_76.inc"
};
static NJS_OBJECT collision_77 = {
#include "stg08/terrain/collision_77.inc"
};
static NJS_OBJECT collision_78 = {
#include "stg08/terrain/collision_78.inc"
};
static NJS_OBJECT collision_79 = {
#include "stg08/terrain/collision_79.inc"
};
static NJS_OBJECT collision_80 = {
#include "stg08/terrain/collision_80.inc"
};
static NJS_OBJECT collision_81 = {
#include "stg08/terrain/collision_81.inc"
};
static NJS_OBJECT collision_82 = {
#include "stg08/terrain/collision_82.inc"
};
static NJS_OBJECT collision_83 = {
#include "stg08/terrain/collision_83.inc"
};
static NJS_OBJECT collision_92 = {
#include "stg08/terrain/collision_92.inc"
};
static NJS_OBJECT collision_94 = {
#include "stg08/terrain/collision_94.inc"
};
static NJS_OBJECT collision_96 = {
#include "stg08/terrain/collision_96.inc"
};
static NJS_OBJECT collision_97 = {
#include "stg08/terrain/collision_97.inc"
};
static NJS_OBJECT collision_99 = {
#include "stg08/terrain/collision_99.inc"
};
static NJS_OBJECT collision_101 = {
#include "stg08/terrain/collision_101.inc"
};
