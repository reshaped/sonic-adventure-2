#include <ninja.h>
#include <sg_xpt.h>

#include "collision/world.h"
#include "stg21/sand_ocean.h"

/* the land's file names, which its land table and texture list point at. each
 * starts on a 4-byte boundary */
const char sand_ocean_land_filenames[226] = {
#include "stg21/terrain/sand_ocean_land_filenames.inc"
};

/* STG21.PRS's land, texture lists, chunk models and Basic models, from the
 * disc. its D lies between sand_ocean.c and sand_textures.c */

/* the data up to the last that a pointer names ahead of its definition, as
 * tentative definitions in the disc's order: GCC places each datum at its
 * first definition */
static struct land_entry entries[397];
struct land_table sand_ocean_land;
static NJS_TEXNAME textures[15];
NJS_TEXLIST sand_ocean_land_texlist;
static Sint16 draw_309_polygons[843];
static Sint32 draw_309_vertices[699];
static NJS_CNK_MODEL draw_309_model;
static NJS_CNK_OBJECT draw_309;
static Sint16 draw_306_polygons[275];
static Sint32 draw_306_vertices[183];
static NJS_CNK_MODEL draw_306_model;
static NJS_CNK_OBJECT draw_308;
static Sint16 draw_305_polygons[71];
static Sint32 draw_305_vertices[47];
static NJS_CNK_MODEL draw_305_model;
static NJS_CNK_OBJECT draw_305;
static Sint16 draw_273_polygons[129];
static Sint32 draw_273_vertices[99];
static NJS_CNK_MODEL draw_273_model;
static NJS_CNK_OBJECT draw_273;
static Sint16 draw_272_polygons[149];
static Sint32 draw_272_vertices[115];
static NJS_CNK_MODEL draw_272_model;
static NJS_CNK_OBJECT draw_272;
static Sint16 draw_271_polygons[129];
static Sint32 draw_271_vertices[107];
static NJS_CNK_MODEL draw_271_model;
static NJS_CNK_OBJECT draw_271;
static Sint16 draw_270_polygons[203];
static Sint32 draw_270_vertices[159];
static NJS_CNK_MODEL draw_270_model;
static NJS_CNK_OBJECT draw_270;
static Sint16 draw_264_polygons[69];
static Sint32 draw_264_vertices[67];
static NJS_CNK_MODEL draw_264_model;
static NJS_CNK_OBJECT draw_269;
static Sint16 draw_263_polygons[63];
static Sint32 draw_263_vertices[51];
static NJS_CNK_MODEL draw_263_model;
static NJS_CNK_OBJECT draw_263;
static Sint16 draw_262_polygons[63];
static Sint32 draw_262_vertices[51];
static NJS_CNK_MODEL draw_262_model;
static NJS_CNK_OBJECT draw_262;
static Sint16 draw_261_polygons[63];
static Sint32 draw_261_vertices[51];
static NJS_CNK_MODEL draw_261_model;
static NJS_CNK_OBJECT draw_261;
static Sint16 draw_258_polygons[63];
static Sint32 draw_258_vertices[51];
static NJS_CNK_MODEL draw_258_model;
static NJS_CNK_OBJECT draw_260;
static Sint16 draw_257_polygons[157];
static Sint32 draw_257_vertices[107];
static NJS_CNK_MODEL draw_257_model;
static NJS_CNK_OBJECT draw_257;
static Sint16 draw_256_polygons[323];
static Sint32 draw_256_vertices[239];
static NJS_CNK_MODEL draw_256_model;
static NJS_CNK_OBJECT draw_256;
static Sint16 draw_255_polygons[185];
static Sint32 draw_255_vertices[127];
static NJS_CNK_MODEL draw_255_model;
static NJS_CNK_OBJECT draw_255;
static Sint16 draw_254_polygons[57];
static Sint32 draw_254_vertices[31];
static NJS_CNK_MODEL draw_254_model;
static NJS_CNK_OBJECT draw_254;
static Sint16 draw_253_polygons[325];
static Sint32 draw_253_vertices[387];
static NJS_CNK_MODEL draw_253_model;
static NJS_CNK_OBJECT draw_253;
static Sint16 draw_252_polygons[181];
static Sint32 draw_252_vertices[211];
static NJS_CNK_MODEL draw_252_model;
static NJS_CNK_OBJECT draw_252;
static Sint16 draw_246_polygons[359];
static Sint32 draw_246_vertices[211];
static NJS_CNK_MODEL draw_246_model;
static NJS_CNK_OBJECT draw_251;
static Sint16 draw_240_polygons[435];
static Sint32 draw_240_vertices[195];
static NJS_CNK_MODEL draw_240_model;
static NJS_CNK_OBJECT draw_245;
static Sint16 draw_234_polygons[447];
static Sint32 draw_234_vertices[243];
static NJS_CNK_MODEL draw_234_model;
static NJS_CNK_OBJECT draw_239;
static Sint16 draw_233_polygons[347];
static Sint32 draw_233_vertices[279];
static NJS_CNK_MODEL draw_233_model;
static NJS_CNK_OBJECT draw_233;
static Sint16 draw_232_polygons[133];
static Sint32 draw_232_vertices[91];
static NJS_CNK_MODEL draw_232_model;
static NJS_CNK_OBJECT draw_232;
static Sint16 draw_228_polygons[25];
static Sint32 draw_228_vertices[19];
static NJS_CNK_MODEL draw_228_model;
static NJS_CNK_OBJECT draw_231;
static Sint16 draw_224_polygons[183];
static Sint32 draw_224_vertices[139];
static NJS_CNK_MODEL draw_224_model;
static NJS_CNK_OBJECT draw_224;
static Sint16 draw_223_polygons[31];
static Sint32 draw_223_vertices[27];
static NJS_CNK_MODEL draw_223_model;
static NJS_CNK_OBJECT draw_223;
static Sint16 draw_222_polygons[39];
static Sint32 draw_222_vertices[35];
static NJS_CNK_MODEL draw_222_model;
static NJS_CNK_OBJECT draw_222;
static Sint16 draw_221_polygons[39];
static Sint32 draw_221_vertices[35];
static NJS_CNK_MODEL draw_221_model;
static NJS_CNK_OBJECT draw_221;
static Sint16 draw_220_polygons[211];
static Sint32 draw_220_vertices[243];
static NJS_CNK_MODEL draw_220_model;
static NJS_CNK_OBJECT draw_220;
static Sint16 draw_219_polygons[65];
static Sint32 draw_219_vertices[67];
static NJS_CNK_MODEL draw_219_model;
static NJS_CNK_OBJECT draw_219;
static Sint16 draw_218_polygons[97];
static Sint32 draw_218_vertices[99];
static NJS_CNK_MODEL draw_218_model;
static NJS_CNK_OBJECT draw_218;
static Sint16 draw_212_polygons[213];
static Sint32 draw_212_vertices[127];
static NJS_CNK_MODEL draw_212_model;
static NJS_CNK_OBJECT draw_217;
static Sint16 draw_211_polygons[187];
static Sint32 draw_211_vertices[103];
static NJS_CNK_MODEL draw_211_model;
static NJS_CNK_OBJECT draw_211;
static Sint16 draw_210_polygons[195];
static Sint32 draw_210_vertices[103];
static NJS_CNK_MODEL draw_210_model;
static NJS_CNK_OBJECT draw_210;
static Sint16 draw_204_polygons[365];
static Sint32 draw_204_vertices[199];
static NJS_CNK_MODEL draw_204_model;
static NJS_CNK_OBJECT draw_209;
static Sint16 draw_203_polygons[147];
static Sint32 draw_203_vertices[123];
static NJS_CNK_MODEL draw_203_model;
static NJS_CNK_OBJECT draw_203;
static Sint16 draw_202_polygons[107];
static Sint32 draw_202_vertices[67];
static NJS_CNK_MODEL draw_202_model;
static NJS_CNK_OBJECT draw_202;
static Sint16 draw_201_polygons[281];
static Sint32 draw_201_vertices[211];
static NJS_CNK_MODEL draw_201_model;
static NJS_CNK_OBJECT draw_201;
static Sint16 draw_200_polygons[231];
static Sint32 draw_200_vertices[167];
static NJS_CNK_MODEL draw_200_model;
static NJS_CNK_OBJECT draw_200;
static Sint16 draw_199_polygons[159];
static Sint32 draw_199_vertices[131];
static NJS_CNK_MODEL draw_199_model;
static NJS_CNK_OBJECT draw_199;
static Sint16 draw_198_polygons[113];
static Sint32 draw_198_vertices[83];
static NJS_CNK_MODEL draw_198_model;
static NJS_CNK_OBJECT draw_198;
static Sint16 draw_197_polygons[141];
static Sint32 draw_197_vertices[99];
static NJS_CNK_MODEL draw_197_model;
static NJS_CNK_OBJECT draw_197;
static Sint16 draw_196_polygons[71];
static Sint32 draw_196_vertices[51];
static NJS_CNK_MODEL draw_196_model;
static NJS_CNK_OBJECT draw_196;
static Sint16 draw_145_polygons[273];
static Sint32 draw_145_vertices[187];
static NJS_CNK_MODEL draw_145_model;
static NJS_CNK_OBJECT draw_147;
static Sint16 draw_142_polygons[205];
static Sint32 draw_142_vertices[147];
static NJS_CNK_MODEL draw_142_model;
static NJS_CNK_OBJECT draw_144;
static Sint16 draw_139_polygons[211];
static Sint32 draw_139_vertices[143];
static NJS_CNK_MODEL draw_139_model;
static NJS_CNK_OBJECT draw_141;
static Sint16 draw_136_polygons[239];
static Sint32 draw_136_vertices[163];
static NJS_CNK_MODEL draw_136_model;
static NJS_CNK_OBJECT draw_138;
static Sint16 draw_133_polygons[321];
static Sint32 draw_133_vertices[203];
static NJS_CNK_MODEL draw_133_model;
static NJS_CNK_OBJECT draw_135;
static Sint16 draw_132_polygons[385];
static Sint32 draw_132_vertices[279];
static NJS_CNK_MODEL draw_132_model;
static NJS_CNK_OBJECT draw_132;
static Sint16 draw_131_polygons[207];
static Sint32 draw_131_vertices[139];
static NJS_CNK_MODEL draw_131_model;
static NJS_CNK_OBJECT draw_131;
static Sint16 draw_130_polygons[289];
static Sint32 draw_130_vertices[227];
static NJS_CNK_MODEL draw_130_model;
static NJS_CNK_OBJECT draw_130;
static Sint16 draw_129_polygons[145];
static Sint32 draw_129_vertices[99];
static NJS_CNK_MODEL draw_129_model;
static NJS_CNK_OBJECT draw_129;
static Sint16 draw_128_polygons[89];
static Sint32 draw_128_vertices[83];
static NJS_CNK_MODEL draw_128_model;
static NJS_CNK_OBJECT draw_128;
static Sint16 draw_127_polygons[295];
static Sint32 draw_127_vertices[195];
static NJS_CNK_MODEL draw_127_model;
static NJS_CNK_OBJECT draw_127;
static Sint16 draw_126_polygons[183];
static Sint32 draw_126_vertices[131];
static NJS_CNK_MODEL draw_126_model;
static NJS_CNK_OBJECT draw_126;
static Sint16 draw_125_polygons[89];
static Sint32 draw_125_vertices[67];
static NJS_CNK_MODEL draw_125_model;
static NJS_CNK_OBJECT draw_125;
static Sint16 draw_124_polygons[91];
static Sint32 draw_124_vertices[51];
static NJS_CNK_MODEL draw_124_model;
static NJS_CNK_OBJECT draw_124;
static Sint16 draw_123_polygons[69];
static Sint32 draw_123_vertices[51];
static NJS_CNK_MODEL draw_123_model;
static NJS_CNK_OBJECT draw_123;
static Sint16 draw_122_polygons[69];
static Sint32 draw_122_vertices[51];
static NJS_CNK_MODEL draw_122_model;
static NJS_CNK_OBJECT draw_122;
static Sint16 draw_120_polygons[203];
static Sint32 draw_120_vertices[99];
static NJS_CNK_MODEL draw_120_model;
static NJS_CNK_OBJECT draw_121;
static Sint16 draw_119_polygons[69];
static Sint32 draw_119_vertices[51];
static NJS_CNK_MODEL draw_119_model;
static NJS_CNK_OBJECT draw_119;
static Sint16 draw_118_polygons[69];
static Sint32 draw_118_vertices[51];
static NJS_CNK_MODEL draw_118_model;
static NJS_CNK_OBJECT draw_118;
static Sint16 draw_110_polygons[107];
static Sint32 draw_110_vertices[67];
static NJS_CNK_MODEL draw_110_model;
static NJS_CNK_OBJECT draw_117;
static Sint16 draw_109_polygons[69];
static Sint32 draw_109_vertices[51];
static NJS_CNK_MODEL draw_109_model;
static NJS_CNK_OBJECT draw_109;
static Sint16 draw_108_polygons[69];
static Sint32 draw_108_vertices[51];
static NJS_CNK_MODEL draw_108_model;
static NJS_CNK_OBJECT draw_108;
static Sint16 draw_107_polygons[83];
static Sint32 draw_107_vertices[51];
static NJS_CNK_MODEL draw_107_model;
static NJS_CNK_OBJECT draw_107;
static Sint16 draw_106_polygons[83];
static Sint32 draw_106_vertices[67];
static NJS_CNK_MODEL draw_106_model;
static NJS_CNK_OBJECT draw_106;
static Sint16 draw_105_polygons[195];
static Sint32 draw_105_vertices[147];
static NJS_CNK_MODEL draw_105_model;
static NJS_CNK_OBJECT draw_105;
static Sint16 draw_104_polygons[49];
static Sint32 draw_104_vertices[35];
static NJS_CNK_MODEL draw_104_model;
static NJS_CNK_OBJECT draw_104;
static Sint16 draw_103_polygons[271];
static Sint32 draw_103_vertices[195];
static NJS_CNK_MODEL draw_103_model;
static NJS_CNK_OBJECT draw_103;
static Sint16 draw_102_polygons[399];
static Sint32 draw_102_vertices[311];
static NJS_CNK_MODEL draw_102_model;
static NJS_CNK_OBJECT draw_102;
static Sint16 draw_101_polygons[207];
static Sint32 draw_101_vertices[243];
static NJS_CNK_MODEL draw_101_model;
static NJS_CNK_OBJECT draw_101;
static Sint16 draw_100_polygons[233];
static Sint32 draw_100_vertices[275];
static NJS_CNK_MODEL draw_100_model;
static NJS_CNK_OBJECT draw_100;
static Sint16 draw_99_polygons[273];
static Sint32 draw_99_vertices[323];
static NJS_CNK_MODEL draw_99_model;
static NJS_CNK_OBJECT draw_99;
static Sint16 draw_18_polygons[87];
static Sint32 draw_18_vertices[51];
static NJS_CNK_MODEL draw_18_model;
static NJS_CNK_OBJECT draw_98;
static Sint16 draw_96_polygons[181];
static Sint32 draw_96_vertices[143];
static NJS_CNK_MODEL draw_96_model;
static NJS_CNK_OBJECT draw_96;
static Sint16 draw_95_polygons[71];
static Sint32 draw_95_vertices[47];
static NJS_CNK_MODEL draw_95_model;
static NJS_CNK_OBJECT draw_95;
static Sint16 draw_94_polygons[91];
static Sint32 draw_94_vertices[99];
static NJS_CNK_MODEL draw_94_model;
static NJS_CNK_OBJECT draw_94;
static Sint16 draw_17_polygons[243];
static Sint32 draw_17_vertices[147];
static NJS_CNK_MODEL draw_17_model;
static NJS_CNK_OBJECT draw_93;
static Sint16 draw_16_polygons[569];
static Sint32 draw_16_vertices[587];
static NJS_CNK_MODEL draw_16_model;
static NJS_CNK_OBJECT draw_92;
static Sint16 draw_91_polygons[141];
static Sint32 draw_91_vertices[99];
static NJS_CNK_MODEL draw_91_model;
static NJS_CNK_OBJECT draw_91;
static Sint16 draw_85_polygons[141];
static Sint32 draw_85_vertices[99];
static NJS_CNK_MODEL draw_85_model;
static NJS_CNK_OBJECT draw_90;
static Sint16 draw_61_polygons[199];
static Sint32 draw_61_vertices[187];
static NJS_CNK_MODEL draw_61_model;
static NJS_CNK_OBJECT draw_75;
static Sint16 draw_57_polygons[455];
static Sint32 draw_57_vertices[383];
static NJS_CNK_MODEL draw_57_model;
static NJS_CNK_OBJECT draw_60;
static Sint16 draw_53_polygons[367];
static Sint32 draw_53_vertices[299];
static NJS_CNK_MODEL draw_53_model;
static NJS_CNK_OBJECT draw_56;
static Sint16 draw_49_polygons[117];
static Sint32 draw_49_vertices[131];
static NJS_CNK_MODEL draw_49_model;
static NJS_CNK_OBJECT draw_52;
static Sint16 draw_46_polygons[977];
static Sint32 draw_46_vertices[639];
static NJS_CNK_MODEL draw_46_model;
static NJS_CNK_OBJECT draw_48;
static Sint16 draw_41_polygons[211];
static Sint32 draw_41_vertices[135];
static NJS_CNK_MODEL draw_41_model;
static NJS_CNK_OBJECT draw_45;
static Sint16 draw_36_polygons[569];
static Sint32 draw_36_vertices[591];
static NJS_CNK_MODEL draw_36_model;
static NJS_CNK_OBJECT draw_40;
static Sint16 draw_34_polygons[87];
static Sint32 draw_34_vertices[51];
static NJS_CNK_MODEL draw_34_model;
static NJS_CNK_OBJECT draw_35;
static Sint16 draw_33_polygons[259];
static Sint32 draw_33_vertices[207];
static NJS_CNK_MODEL draw_33_model;
static NJS_CNK_OBJECT draw_33;
static Sint16 draw_32_polygons[259];
static Sint32 draw_32_vertices[207];
static NJS_CNK_MODEL draw_32_model;
static NJS_CNK_OBJECT draw_32;
static Sint16 draw_31_polygons[117];
static Sint32 draw_31_vertices[131];
static NJS_CNK_MODEL draw_31_model;
static NJS_CNK_OBJECT draw_31;
static Sint16 draw_30_polygons[117];
static Sint32 draw_30_vertices[131];
static NJS_CNK_MODEL draw_30_model;
static NJS_CNK_OBJECT draw_30;
static Sint16 draw_25_polygons[81];
static Sint32 draw_25_vertices[55];
static NJS_CNK_MODEL draw_25_model;
static NJS_CNK_OBJECT draw_29;
static Sint16 draw_20_polygons[81];
static Sint32 draw_20_vertices[55];
static NJS_CNK_MODEL draw_20_model;
static NJS_CNK_OBJECT draw_24;
static Sint16 draw_14_polygons[205];
static Sint32 draw_14_vertices[135];
static NJS_CNK_MODEL draw_14_model;
static NJS_CNK_OBJECT draw_15;
static Sint16 draw_12_polygons[435];
static Sint32 draw_12_vertices[195];
static NJS_CNK_MODEL draw_12_model;
static NJS_CNK_OBJECT draw_13;
static Sint16 draw_10_polygons[447];
static Sint32 draw_10_vertices[243];
static NJS_CNK_MODEL draw_10_model;
static NJS_CNK_OBJECT draw_11;
static Sint16 draw_8_polygons[101];
static Sint32 draw_8_vertices[71];
static NJS_CNK_MODEL draw_8_model;
static NJS_CNK_OBJECT draw_9;
static Sint16 draw_6_polygons[195];
static Sint32 draw_6_vertices[115];
static NJS_CNK_MODEL draw_6_model;
static NJS_CNK_OBJECT draw_7;
static Sint16 draw_4_polygons[167];
static Sint32 draw_4_vertices[111];
static NJS_CNK_MODEL draw_4_model;
static NJS_CNK_OBJECT draw_5;
static Sint16 draw_3_polygons[315];
static Sint32 draw_3_vertices[255];
static NJS_CNK_MODEL draw_3_model;
static NJS_CNK_OBJECT draw_3;
static Sint16 draw_0_polygons[449];
static Sint32 draw_0_vertices[431];
static NJS_CNK_MODEL draw_0_model;
static NJS_CNK_OBJECT draw_2;
static NJS_MATERIAL collision_0_materials[1];
static Sint16 collision_0_faces[11];
static NJS_MESHSET collision_0_meshsets[1];
static NJS_POINT3 collision_0_points[8];
static NJS_MODEL collision_0_model;
static NJS_OBJECT collision_5;
static NJS_MATERIAL collision_20_materials[1];
static Sint16 collision_20_faces[22];
static NJS_MESHSET collision_20_meshsets[1];
static NJS_POINT3 collision_20_points[8];
static NJS_MODEL collision_20_model;
static NJS_OBJECT collision_20;
static NJS_MATERIAL collision_22_materials[1];
static Sint16 collision_22_faces[17];
static NJS_MESHSET collision_22_meshsets[1];
static NJS_POINT3 collision_22_points[8];
static NJS_MODEL collision_22_model;
static NJS_OBJECT collision_22;
static NJS_MATERIAL collision_24_materials[1];
static Sint16 collision_24_faces[11];
static NJS_MESHSET collision_24_meshsets[1];
static NJS_POINT3 collision_24_points[8];
static NJS_MODEL collision_24_model;
static NJS_OBJECT collision_24;
static NJS_MATERIAL collision_25_materials[1];
static Sint16 collision_25_faces[11];
static NJS_MESHSET collision_25_meshsets[1];
static NJS_POINT3 collision_25_points[8];
static NJS_MODEL collision_25_model;
static NJS_OBJECT collision_25;
static NJS_MATERIAL collision_26_materials[1];
static Sint16 collision_26_faces[11];
static NJS_MESHSET collision_26_meshsets[1];
static NJS_POINT3 collision_26_points[8];
static NJS_MODEL collision_26_model;
static NJS_OBJECT collision_26;
static NJS_MATERIAL collision_27_materials[1];
static Sint16 collision_27_faces[11];
static NJS_MESHSET collision_27_meshsets[1];
static NJS_POINT3 collision_27_points[8];
static NJS_MODEL collision_27_model;
static NJS_OBJECT collision_27;
static NJS_MATERIAL collision_28_materials[1];
static Sint16 collision_28_faces[11];
static NJS_MESHSET collision_28_meshsets[1];
static NJS_POINT3 collision_28_points[8];
static NJS_MODEL collision_28_model;
static NJS_OBJECT collision_28;
static NJS_MATERIAL collision_29_materials[1];
static Sint16 collision_29_faces[19];
static NJS_MESHSET collision_29_meshsets[1];
static NJS_POINT3 collision_29_points[8];
static NJS_MODEL collision_29_model;
static NJS_OBJECT collision_29;
static NJS_MATERIAL collision_33_materials[1];
static Sint16 collision_33_faces[51];
static NJS_MESHSET collision_33_meshsets[1];
static NJS_POINT3 collision_33_points[27];
static NJS_MODEL collision_33_model;
static NJS_OBJECT collision_33;
static NJS_MATERIAL collision_35_materials[1];
static Sint16 collision_35_faces[45];
static NJS_MESHSET collision_35_meshsets[1];
static NJS_POINT3 collision_35_points[20];
static NJS_MODEL collision_35_model;
static NJS_OBJECT collision_35;
static NJS_MATERIAL collision_37_materials[1];
static Sint16 collision_37_faces[32];
static NJS_MESHSET collision_37_meshsets[1];
static NJS_POINT3 collision_37_points[14];
static NJS_MODEL collision_37_model;
static NJS_OBJECT collision_37;
static NJS_MATERIAL collision_38_materials[1];
static Sint16 collision_38_faces[23];
static NJS_MESHSET collision_38_meshsets[1];
static NJS_POINT3 collision_38_points[12];
static NJS_MODEL collision_38_model;
static NJS_OBJECT collision_38;
static NJS_MATERIAL collision_39_materials[1];
static Sint16 collision_39_faces[26];
static NJS_MESHSET collision_39_meshsets[1];
static NJS_POINT3 collision_39_points[12];
static NJS_MODEL collision_39_model;
static NJS_OBJECT collision_39;
static NJS_MATERIAL collision_40_materials[1];
static Sint16 collision_40_faces[30];
static NJS_MESHSET collision_40_meshsets[1];
static NJS_POINT3 collision_40_points[12];
static NJS_MODEL collision_40_model;
static NJS_OBJECT collision_40;
static NJS_MATERIAL collision_41_materials[1];
static Sint16 collision_41_faces[14];
static NJS_MESHSET collision_41_meshsets[1];
static NJS_POINT3 collision_41_points[8];
static NJS_MODEL collision_41_model;
static NJS_OBJECT collision_41;
static NJS_MATERIAL collision_42_materials[1];
static Sint16 collision_42_faces[16];
static NJS_MESHSET collision_42_meshsets[1];
static NJS_POINT3 collision_42_points[8];
static NJS_MODEL collision_42_model;
static NJS_OBJECT collision_42;
static NJS_MATERIAL collision_43_materials[1];
static Sint16 collision_43_faces[66];
static NJS_MESHSET collision_43_meshsets[1];
static NJS_POINT3 collision_43_points[32];
static NJS_MODEL collision_43_model;
static NJS_OBJECT collision_43;
static NJS_MATERIAL collision_44_materials[1];
static Sint16 collision_44_faces[39];
static NJS_MESHSET collision_44_meshsets[1];
static NJS_POINT3 collision_44_points[18];
static NJS_MODEL collision_44_model;
static NJS_OBJECT collision_44;
static NJS_MATERIAL collision_46_materials[1];
static Sint16 collision_46_faces[97];
static NJS_MESHSET collision_46_meshsets[1];
static NJS_POINT3 collision_46_points[37];
static NJS_MODEL collision_46_model;
static NJS_OBJECT collision_46;
static NJS_MATERIAL collision_50_materials[1];
static Sint16 collision_50_faces[120];
static NJS_MESHSET collision_50_meshsets[1];
static NJS_POINT3 collision_50_points[44];
static NJS_MODEL collision_50_model;
static NJS_OBJECT collision_50;
static NJS_MATERIAL collision_53_materials[1];
static Sint16 collision_53_faces[172];
static NJS_MESHSET collision_53_meshsets[1];
static NJS_POINT3 collision_53_points[54];
static NJS_MODEL collision_53_model;
static NJS_OBJECT collision_53;
static NJS_MATERIAL collision_62_materials[1];
static Sint16 collision_62_faces[73];
static NJS_MESHSET collision_62_meshsets[1];
static NJS_POINT3 collision_62_points[32];
static NJS_MODEL collision_62_model;
static NJS_OBJECT collision_62;
static NJS_MATERIAL collision_63_materials[1];
static Sint16 collision_63_faces[31];
static NJS_MESHSET collision_63_meshsets[1];
static NJS_POINT3 collision_63_points[14];
static NJS_MODEL collision_63_model;
static NJS_OBJECT collision_63;
static NJS_MATERIAL collision_64_materials[1];
static Sint16 collision_64_faces[198];
static NJS_MESHSET collision_64_meshsets[1];
static NJS_POINT3 collision_64_points[81];
static NJS_MODEL collision_64_model;
static NJS_OBJECT collision_64;
static NJS_MATERIAL collision_66_materials[1];
static Sint16 collision_66_faces[35];
static NJS_MESHSET collision_66_meshsets[1];
static NJS_POINT3 collision_66_points[14];
static NJS_MODEL collision_66_model;
static NJS_OBJECT collision_66;
static NJS_MATERIAL collision_67_materials[1];
static Sint16 collision_67_faces[97];
static NJS_MESHSET collision_67_meshsets[1];
static NJS_POINT3 collision_67_points[44];
static NJS_MODEL collision_67_model;
static NJS_OBJECT collision_67;
static NJS_MATERIAL collision_69_materials[1];
static Sint16 collision_69_faces[124];
static NJS_MESHSET collision_69_meshsets[1];
static NJS_POINT3 collision_69_points[53];
static NJS_MODEL collision_69_model;
static NJS_OBJECT collision_69;
static NJS_MATERIAL collision_71_materials[1];
static Sint16 collision_71_faces[59];
static NJS_MESHSET collision_71_meshsets[1];
static NJS_POINT3 collision_71_points[26];
static NJS_MODEL collision_71_model;
static NJS_OBJECT collision_71;
static NJS_MATERIAL collision_73_materials[1];
static Sint16 collision_73_faces[59];
static NJS_MESHSET collision_73_meshsets[1];
static NJS_POINT3 collision_73_points[26];
static NJS_MODEL collision_73_model;
static NJS_OBJECT collision_73;
static NJS_MATERIAL collision_75_materials[1];
static Sint16 collision_75_faces[60];
static NJS_MESHSET collision_75_meshsets[1];
static NJS_POINT3 collision_75_points[29];
static NJS_MODEL collision_75_model;
static NJS_OBJECT collision_75;
static NJS_MATERIAL collision_76_materials[1];
static Sint16 collision_76_faces[92];
static NJS_MESHSET collision_76_meshsets[1];
static NJS_POINT3 collision_76_points[53];
static NJS_MODEL collision_76_model;
static NJS_OBJECT collision_76;
static NJS_MATERIAL collision_77_materials[1];
static Sint16 collision_77_faces[193];
static NJS_MESHSET collision_77_meshsets[1];
static NJS_POINT3 collision_77_points[90];
static NJS_MODEL collision_77_model;
static NJS_OBJECT collision_77;
static NJS_MATERIAL collision_78_materials[1];
static Sint16 collision_78_faces[16];
static NJS_MESHSET collision_78_meshsets[1];
static NJS_POINT3 collision_78_points[8];
static NJS_MODEL collision_78_model;
static NJS_OBJECT collision_78;
static NJS_MATERIAL collision_79_materials[1];
static Sint16 collision_79_faces[16];
static NJS_MESHSET collision_79_meshsets[1];
static NJS_POINT3 collision_79_points[8];
static NJS_MODEL collision_79_model;
static NJS_OBJECT collision_79;
static NJS_MATERIAL collision_85_materials[1];
static Sint16 collision_85_faces[21];
static NJS_MESHSET collision_85_meshsets[1];
static NJS_POINT3 collision_85_points[12];
static NJS_MODEL collision_85_model;
static NJS_OBJECT collision_85;
static NJS_CNK_OBJECT draw_307;
static NJS_CNK_OBJECT draw_306;
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
static NJS_CNK_OBJECT draw_268;
static NJS_CNK_OBJECT draw_267;
static NJS_CNK_OBJECT draw_266;
static NJS_CNK_OBJECT draw_265;
static NJS_CNK_OBJECT draw_264;
static NJS_CNK_OBJECT draw_259;
static NJS_CNK_OBJECT draw_258;
static NJS_CNK_OBJECT draw_250;
static NJS_CNK_OBJECT draw_249;
static NJS_CNK_OBJECT draw_248;
static NJS_CNK_OBJECT draw_247;
static NJS_CNK_OBJECT draw_246;
static NJS_CNK_OBJECT draw_244;
static NJS_CNK_OBJECT draw_243;
static NJS_CNK_OBJECT draw_242;
static NJS_CNK_OBJECT draw_241;
static NJS_CNK_OBJECT draw_240;
static NJS_CNK_OBJECT draw_238;
static NJS_CNK_OBJECT draw_237;
static NJS_CNK_OBJECT draw_236;
static NJS_CNK_OBJECT draw_235;
static NJS_CNK_OBJECT draw_234;
static NJS_CNK_OBJECT draw_230;
static NJS_CNK_OBJECT draw_229;
static NJS_CNK_OBJECT draw_228;
static NJS_CNK_OBJECT draw_227;
static NJS_CNK_OBJECT draw_226;
static NJS_CNK_OBJECT draw_225;
static NJS_CNK_OBJECT draw_216;
static NJS_CNK_OBJECT draw_215;
static NJS_CNK_OBJECT draw_214;
static NJS_CNK_OBJECT draw_213;
static NJS_CNK_OBJECT draw_212;
static NJS_CNK_OBJECT draw_208;
static NJS_CNK_OBJECT draw_207;
static NJS_CNK_OBJECT draw_206;
static NJS_CNK_OBJECT draw_205;
static NJS_CNK_OBJECT draw_204;
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
static NJS_CNK_OBJECT draw_146;
static NJS_CNK_OBJECT draw_145;
static NJS_CNK_OBJECT draw_143;
static NJS_CNK_OBJECT draw_142;
static NJS_CNK_OBJECT draw_140;
static NJS_CNK_OBJECT draw_139;
static NJS_CNK_OBJECT draw_137;
static NJS_CNK_OBJECT draw_136;
static NJS_CNK_OBJECT draw_134;
static NJS_CNK_OBJECT draw_133;
static NJS_CNK_OBJECT draw_120;
static NJS_CNK_OBJECT draw_116;
static NJS_CNK_OBJECT draw_115;
static NJS_CNK_OBJECT draw_114;
static NJS_CNK_OBJECT draw_113;
static NJS_CNK_OBJECT draw_112;
static NJS_CNK_OBJECT draw_111;
static NJS_CNK_OBJECT draw_110;
static NJS_CNK_OBJECT draw_97;
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
static NJS_CNK_OBJECT draw_59;
static NJS_CNK_OBJECT draw_58;
static NJS_CNK_OBJECT draw_57;
static NJS_CNK_OBJECT draw_55;
static NJS_CNK_OBJECT draw_54;
static NJS_CNK_OBJECT draw_53;
static NJS_CNK_OBJECT draw_51;
static NJS_CNK_OBJECT draw_50;
static NJS_CNK_OBJECT draw_49;
static NJS_CNK_OBJECT draw_47;
static NJS_CNK_OBJECT draw_46;
static NJS_CNK_OBJECT draw_44;
static NJS_CNK_OBJECT draw_43;
static NJS_CNK_OBJECT draw_42;
static NJS_CNK_OBJECT draw_41;
static NJS_CNK_OBJECT draw_39;
static NJS_CNK_OBJECT draw_38;
static NJS_CNK_OBJECT draw_37;
static NJS_CNK_OBJECT draw_36;
static NJS_CNK_OBJECT draw_34;
static NJS_CNK_OBJECT draw_28;
static NJS_CNK_OBJECT draw_27;
static NJS_CNK_OBJECT draw_26;
static NJS_CNK_OBJECT draw_25;
static NJS_CNK_OBJECT draw_23;
static NJS_CNK_OBJECT draw_22;
static NJS_CNK_OBJECT draw_21;
static NJS_CNK_OBJECT draw_20;
static NJS_CNK_OBJECT draw_19;
static NJS_CNK_OBJECT draw_18;
static NJS_CNK_OBJECT draw_17;
static NJS_CNK_OBJECT draw_16;
static NJS_CNK_OBJECT draw_14;
static NJS_CNK_OBJECT draw_12;
static NJS_CNK_OBJECT draw_10;
static NJS_CNK_OBJECT draw_8;
static NJS_CNK_OBJECT draw_6;
static NJS_CNK_OBJECT draw_4;
static NJS_CNK_OBJECT draw_1;
static NJS_CNK_OBJECT draw_0;
static NJS_OBJECT collision_0;
static NJS_OBJECT collision_1;
static NJS_OBJECT collision_2;
static NJS_OBJECT collision_3;
static NJS_OBJECT collision_4;
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
static NJS_OBJECT collision_21;
static NJS_OBJECT collision_23;
static NJS_OBJECT collision_30;
static NJS_OBJECT collision_31;
static NJS_OBJECT collision_32;
static NJS_OBJECT collision_34;
static NJS_OBJECT collision_36;
static NJS_OBJECT collision_45;
static NJS_OBJECT collision_47;
static NJS_OBJECT collision_48;
static NJS_OBJECT collision_49;
static NJS_OBJECT collision_51;
static NJS_OBJECT collision_52;
static NJS_OBJECT collision_54;
static NJS_OBJECT collision_55;
static NJS_OBJECT collision_56;
static NJS_OBJECT collision_57;
static NJS_OBJECT collision_58;
static NJS_OBJECT collision_59;
static NJS_OBJECT collision_60;
static NJS_OBJECT collision_61;
static NJS_OBJECT collision_65;
static NJS_OBJECT collision_68;
static NJS_OBJECT collision_70;
static NJS_OBJECT collision_72;
static NJS_OBJECT collision_74;
static NJS_OBJECT collision_80;
static NJS_OBJECT collision_81;
static NJS_OBJECT collision_82;
static NJS_OBJECT collision_83;
static NJS_OBJECT collision_84;
static NJS_OBJECT collision_86;

static struct land_entry entries[397] = {
#include "stg21/terrain/entries.inc"
};
struct land_table sand_ocean_land = {
#include "stg21/terrain/sand_ocean_land.inc"
};
static NJS_TEXNAME textures[15] = {
#include "stg21/terrain/textures.inc"
};
NJS_TEXLIST sand_ocean_land_texlist = {
#include "stg21/terrain/sand_ocean_land_texlist.inc"
};
static Sint16 draw_309_polygons[843] = {
#include "stg21/terrain/draw_309_polygons.inc"
};
static Sint32 draw_309_vertices[699] = {
#include "stg21/terrain/draw_309_vertices.inc"
};
static NJS_CNK_MODEL draw_309_model = {
#include "stg21/terrain/draw_309_model.inc"
};
static NJS_CNK_OBJECT draw_309 = {
#include "stg21/terrain/draw_309.inc"
};
static Sint16 draw_306_polygons[275] = {
#include "stg21/terrain/draw_306_polygons.inc"
};
static Sint32 draw_306_vertices[183] = {
#include "stg21/terrain/draw_306_vertices.inc"
};
static NJS_CNK_MODEL draw_306_model = {
#include "stg21/terrain/draw_306_model.inc"
};
static NJS_CNK_OBJECT draw_308 = {
#include "stg21/terrain/draw_308.inc"
};
static Sint16 draw_305_polygons[71] = {
#include "stg21/terrain/draw_305_polygons.inc"
};
static Sint32 draw_305_vertices[47] = {
#include "stg21/terrain/draw_305_vertices.inc"
};
static NJS_CNK_MODEL draw_305_model = {
#include "stg21/terrain/draw_305_model.inc"
};
static NJS_CNK_OBJECT draw_305 = {
#include "stg21/terrain/draw_305.inc"
};
static Sint16 draw_273_polygons[129] = {
#include "stg21/terrain/draw_273_polygons.inc"
};
static Sint32 draw_273_vertices[99] = {
#include "stg21/terrain/draw_273_vertices.inc"
};
static NJS_CNK_MODEL draw_273_model = {
#include "stg21/terrain/draw_273_model.inc"
};
static NJS_CNK_OBJECT draw_273 = {
#include "stg21/terrain/draw_273.inc"
};
static Sint16 draw_272_polygons[149] = {
#include "stg21/terrain/draw_272_polygons.inc"
};
static Sint32 draw_272_vertices[115] = {
#include "stg21/terrain/draw_272_vertices.inc"
};
static NJS_CNK_MODEL draw_272_model = {
#include "stg21/terrain/draw_272_model.inc"
};
static NJS_CNK_OBJECT draw_272 = {
#include "stg21/terrain/draw_272.inc"
};
static Sint16 draw_271_polygons[129] = {
#include "stg21/terrain/draw_271_polygons.inc"
};
static Sint32 draw_271_vertices[107] = {
#include "stg21/terrain/draw_271_vertices.inc"
};
static NJS_CNK_MODEL draw_271_model = {
#include "stg21/terrain/draw_271_model.inc"
};
static NJS_CNK_OBJECT draw_271 = {
#include "stg21/terrain/draw_271.inc"
};
static Sint16 draw_270_polygons[203] = {
#include "stg21/terrain/draw_270_polygons.inc"
};
static Sint32 draw_270_vertices[159] = {
#include "stg21/terrain/draw_270_vertices.inc"
};
static NJS_CNK_MODEL draw_270_model = {
#include "stg21/terrain/draw_270_model.inc"
};
static NJS_CNK_OBJECT draw_270 = {
#include "stg21/terrain/draw_270.inc"
};
static Sint16 draw_264_polygons[69] = {
#include "stg21/terrain/draw_264_polygons.inc"
};
static Sint32 draw_264_vertices[67] = {
#include "stg21/terrain/draw_264_vertices.inc"
};
static NJS_CNK_MODEL draw_264_model = {
#include "stg21/terrain/draw_264_model.inc"
};
static NJS_CNK_OBJECT draw_269 = {
#include "stg21/terrain/draw_269.inc"
};
static Sint16 draw_263_polygons[63] = {
#include "stg21/terrain/draw_263_polygons.inc"
};
static Sint32 draw_263_vertices[51] = {
#include "stg21/terrain/draw_263_vertices.inc"
};
static NJS_CNK_MODEL draw_263_model = {
#include "stg21/terrain/draw_263_model.inc"
};
static NJS_CNK_OBJECT draw_263 = {
#include "stg21/terrain/draw_263.inc"
};
static Sint16 draw_262_polygons[63] = {
#include "stg21/terrain/draw_262_polygons.inc"
};
static Sint32 draw_262_vertices[51] = {
#include "stg21/terrain/draw_262_vertices.inc"
};
static NJS_CNK_MODEL draw_262_model = {
#include "stg21/terrain/draw_262_model.inc"
};
static NJS_CNK_OBJECT draw_262 = {
#include "stg21/terrain/draw_262.inc"
};
static Sint16 draw_261_polygons[63] = {
#include "stg21/terrain/draw_261_polygons.inc"
};
static Sint32 draw_261_vertices[51] = {
#include "stg21/terrain/draw_261_vertices.inc"
};
static NJS_CNK_MODEL draw_261_model = {
#include "stg21/terrain/draw_261_model.inc"
};
static NJS_CNK_OBJECT draw_261 = {
#include "stg21/terrain/draw_261.inc"
};
static Sint16 draw_258_polygons[63] = {
#include "stg21/terrain/draw_258_polygons.inc"
};
static Sint32 draw_258_vertices[51] = {
#include "stg21/terrain/draw_258_vertices.inc"
};
static NJS_CNK_MODEL draw_258_model = {
#include "stg21/terrain/draw_258_model.inc"
};
static NJS_CNK_OBJECT draw_260 = {
#include "stg21/terrain/draw_260.inc"
};
static Sint16 draw_257_polygons[157] = {
#include "stg21/terrain/draw_257_polygons.inc"
};
static Sint32 draw_257_vertices[107] = {
#include "stg21/terrain/draw_257_vertices.inc"
};
static NJS_CNK_MODEL draw_257_model = {
#include "stg21/terrain/draw_257_model.inc"
};
static NJS_CNK_OBJECT draw_257 = {
#include "stg21/terrain/draw_257.inc"
};
static Sint16 draw_256_polygons[323] = {
#include "stg21/terrain/draw_256_polygons.inc"
};
static Sint32 draw_256_vertices[239] = {
#include "stg21/terrain/draw_256_vertices.inc"
};
static NJS_CNK_MODEL draw_256_model = {
#include "stg21/terrain/draw_256_model.inc"
};
static NJS_CNK_OBJECT draw_256 = {
#include "stg21/terrain/draw_256.inc"
};
static Sint16 draw_255_polygons[185] = {
#include "stg21/terrain/draw_255_polygons.inc"
};
static Sint32 draw_255_vertices[127] = {
#include "stg21/terrain/draw_255_vertices.inc"
};
static NJS_CNK_MODEL draw_255_model = {
#include "stg21/terrain/draw_255_model.inc"
};
static NJS_CNK_OBJECT draw_255 = {
#include "stg21/terrain/draw_255.inc"
};
static Sint16 draw_254_polygons[57] = {
#include "stg21/terrain/draw_254_polygons.inc"
};
static Sint32 draw_254_vertices[31] = {
#include "stg21/terrain/draw_254_vertices.inc"
};
static NJS_CNK_MODEL draw_254_model = {
#include "stg21/terrain/draw_254_model.inc"
};
static NJS_CNK_OBJECT draw_254 = {
#include "stg21/terrain/draw_254.inc"
};
static Sint16 draw_253_polygons[325] = {
#include "stg21/terrain/draw_253_polygons.inc"
};
static Sint32 draw_253_vertices[387] = {
#include "stg21/terrain/draw_253_vertices.inc"
};
static NJS_CNK_MODEL draw_253_model = {
#include "stg21/terrain/draw_253_model.inc"
};
static NJS_CNK_OBJECT draw_253 = {
#include "stg21/terrain/draw_253.inc"
};
static Sint16 draw_252_polygons[181] = {
#include "stg21/terrain/draw_252_polygons.inc"
};
static Sint32 draw_252_vertices[211] = {
#include "stg21/terrain/draw_252_vertices.inc"
};
static NJS_CNK_MODEL draw_252_model = {
#include "stg21/terrain/draw_252_model.inc"
};
static NJS_CNK_OBJECT draw_252 = {
#include "stg21/terrain/draw_252.inc"
};
static Sint16 draw_246_polygons[359] = {
#include "stg21/terrain/draw_246_polygons.inc"
};
static Sint32 draw_246_vertices[211] = {
#include "stg21/terrain/draw_246_vertices.inc"
};
static NJS_CNK_MODEL draw_246_model = {
#include "stg21/terrain/draw_246_model.inc"
};
static NJS_CNK_OBJECT draw_251 = {
#include "stg21/terrain/draw_251.inc"
};
static Sint16 draw_240_polygons[435] = {
#include "stg21/terrain/draw_240_polygons.inc"
};
static Sint32 draw_240_vertices[195] = {
#include "stg21/terrain/draw_240_vertices.inc"
};
static NJS_CNK_MODEL draw_240_model = {
#include "stg21/terrain/draw_240_model.inc"
};
static NJS_CNK_OBJECT draw_245 = {
#include "stg21/terrain/draw_245.inc"
};
static Sint16 draw_234_polygons[447] = {
#include "stg21/terrain/draw_234_polygons.inc"
};
static Sint32 draw_234_vertices[243] = {
#include "stg21/terrain/draw_234_vertices.inc"
};
static NJS_CNK_MODEL draw_234_model = {
#include "stg21/terrain/draw_234_model.inc"
};
static NJS_CNK_OBJECT draw_239 = {
#include "stg21/terrain/draw_239.inc"
};
static Sint16 draw_233_polygons[347] = {
#include "stg21/terrain/draw_233_polygons.inc"
};
static Sint32 draw_233_vertices[279] = {
#include "stg21/terrain/draw_233_vertices.inc"
};
static NJS_CNK_MODEL draw_233_model = {
#include "stg21/terrain/draw_233_model.inc"
};
static NJS_CNK_OBJECT draw_233 = {
#include "stg21/terrain/draw_233.inc"
};
static Sint16 draw_232_polygons[133] = {
#include "stg21/terrain/draw_232_polygons.inc"
};
static Sint32 draw_232_vertices[91] = {
#include "stg21/terrain/draw_232_vertices.inc"
};
static NJS_CNK_MODEL draw_232_model = {
#include "stg21/terrain/draw_232_model.inc"
};
static NJS_CNK_OBJECT draw_232 = {
#include "stg21/terrain/draw_232.inc"
};
static Sint16 draw_228_polygons[25] = {
#include "stg21/terrain/draw_228_polygons.inc"
};
static Sint32 draw_228_vertices[19] = {
#include "stg21/terrain/draw_228_vertices.inc"
};
static NJS_CNK_MODEL draw_228_model = {
#include "stg21/terrain/draw_228_model.inc"
};
static NJS_CNK_OBJECT draw_231 = {
#include "stg21/terrain/draw_231.inc"
};
static Sint16 draw_224_polygons[183] = {
#include "stg21/terrain/draw_224_polygons.inc"
};
static Sint32 draw_224_vertices[139] = {
#include "stg21/terrain/draw_224_vertices.inc"
};
static NJS_CNK_MODEL draw_224_model = {
#include "stg21/terrain/draw_224_model.inc"
};
static NJS_CNK_OBJECT draw_224 = {
#include "stg21/terrain/draw_224.inc"
};
static Sint16 draw_223_polygons[31] = {
#include "stg21/terrain/draw_223_polygons.inc"
};
static Sint32 draw_223_vertices[27] = {
#include "stg21/terrain/draw_223_vertices.inc"
};
static NJS_CNK_MODEL draw_223_model = {
#include "stg21/terrain/draw_223_model.inc"
};
static NJS_CNK_OBJECT draw_223 = {
#include "stg21/terrain/draw_223.inc"
};
static Sint16 draw_222_polygons[39] = {
#include "stg21/terrain/draw_222_polygons.inc"
};
static Sint32 draw_222_vertices[35] = {
#include "stg21/terrain/draw_222_vertices.inc"
};
static NJS_CNK_MODEL draw_222_model = {
#include "stg21/terrain/draw_222_model.inc"
};
static NJS_CNK_OBJECT draw_222 = {
#include "stg21/terrain/draw_222.inc"
};
static Sint16 draw_221_polygons[39] = {
#include "stg21/terrain/draw_221_polygons.inc"
};
static Sint32 draw_221_vertices[35] = {
#include "stg21/terrain/draw_221_vertices.inc"
};
static NJS_CNK_MODEL draw_221_model = {
#include "stg21/terrain/draw_221_model.inc"
};
static NJS_CNK_OBJECT draw_221 = {
#include "stg21/terrain/draw_221.inc"
};
static Sint16 draw_220_polygons[211] = {
#include "stg21/terrain/draw_220_polygons.inc"
};
static Sint32 draw_220_vertices[243] = {
#include "stg21/terrain/draw_220_vertices.inc"
};
static NJS_CNK_MODEL draw_220_model = {
#include "stg21/terrain/draw_220_model.inc"
};
static NJS_CNK_OBJECT draw_220 = {
#include "stg21/terrain/draw_220.inc"
};
static Sint16 draw_219_polygons[65] = {
#include "stg21/terrain/draw_219_polygons.inc"
};
static Sint32 draw_219_vertices[67] = {
#include "stg21/terrain/draw_219_vertices.inc"
};
static NJS_CNK_MODEL draw_219_model = {
#include "stg21/terrain/draw_219_model.inc"
};
static NJS_CNK_OBJECT draw_219 = {
#include "stg21/terrain/draw_219.inc"
};
static Sint16 draw_218_polygons[97] = {
#include "stg21/terrain/draw_218_polygons.inc"
};
static Sint32 draw_218_vertices[99] = {
#include "stg21/terrain/draw_218_vertices.inc"
};
static NJS_CNK_MODEL draw_218_model = {
#include "stg21/terrain/draw_218_model.inc"
};
static NJS_CNK_OBJECT draw_218 = {
#include "stg21/terrain/draw_218.inc"
};
static Sint16 draw_212_polygons[213] = {
#include "stg21/terrain/draw_212_polygons.inc"
};
static Sint32 draw_212_vertices[127] = {
#include "stg21/terrain/draw_212_vertices.inc"
};
static NJS_CNK_MODEL draw_212_model = {
#include "stg21/terrain/draw_212_model.inc"
};
static NJS_CNK_OBJECT draw_217 = {
#include "stg21/terrain/draw_217.inc"
};
static Sint16 draw_211_polygons[187] = {
#include "stg21/terrain/draw_211_polygons.inc"
};
static Sint32 draw_211_vertices[103] = {
#include "stg21/terrain/draw_211_vertices.inc"
};
static NJS_CNK_MODEL draw_211_model = {
#include "stg21/terrain/draw_211_model.inc"
};
static NJS_CNK_OBJECT draw_211 = {
#include "stg21/terrain/draw_211.inc"
};
static Sint16 draw_210_polygons[195] = {
#include "stg21/terrain/draw_210_polygons.inc"
};
static Sint32 draw_210_vertices[103] = {
#include "stg21/terrain/draw_210_vertices.inc"
};
static NJS_CNK_MODEL draw_210_model = {
#include "stg21/terrain/draw_210_model.inc"
};
static NJS_CNK_OBJECT draw_210 = {
#include "stg21/terrain/draw_210.inc"
};
static Sint16 draw_204_polygons[365] = {
#include "stg21/terrain/draw_204_polygons.inc"
};
static Sint32 draw_204_vertices[199] = {
#include "stg21/terrain/draw_204_vertices.inc"
};
static NJS_CNK_MODEL draw_204_model = {
#include "stg21/terrain/draw_204_model.inc"
};
static NJS_CNK_OBJECT draw_209 = {
#include "stg21/terrain/draw_209.inc"
};
static Sint16 draw_203_polygons[147] = {
#include "stg21/terrain/draw_203_polygons.inc"
};
static Sint32 draw_203_vertices[123] = {
#include "stg21/terrain/draw_203_vertices.inc"
};
static NJS_CNK_MODEL draw_203_model = {
#include "stg21/terrain/draw_203_model.inc"
};
static NJS_CNK_OBJECT draw_203 = {
#include "stg21/terrain/draw_203.inc"
};
static Sint16 draw_202_polygons[107] = {
#include "stg21/terrain/draw_202_polygons.inc"
};
static Sint32 draw_202_vertices[67] = {
#include "stg21/terrain/draw_202_vertices.inc"
};
static NJS_CNK_MODEL draw_202_model = {
#include "stg21/terrain/draw_202_model.inc"
};
static NJS_CNK_OBJECT draw_202 = {
#include "stg21/terrain/draw_202.inc"
};
static Sint16 draw_201_polygons[281] = {
#include "stg21/terrain/draw_201_polygons.inc"
};
static Sint32 draw_201_vertices[211] = {
#include "stg21/terrain/draw_201_vertices.inc"
};
static NJS_CNK_MODEL draw_201_model = {
#include "stg21/terrain/draw_201_model.inc"
};
static NJS_CNK_OBJECT draw_201 = {
#include "stg21/terrain/draw_201.inc"
};
static Sint16 draw_200_polygons[231] = {
#include "stg21/terrain/draw_200_polygons.inc"
};
static Sint32 draw_200_vertices[167] = {
#include "stg21/terrain/draw_200_vertices.inc"
};
static NJS_CNK_MODEL draw_200_model = {
#include "stg21/terrain/draw_200_model.inc"
};
static NJS_CNK_OBJECT draw_200 = {
#include "stg21/terrain/draw_200.inc"
};
static Sint16 draw_199_polygons[159] = {
#include "stg21/terrain/draw_199_polygons.inc"
};
static Sint32 draw_199_vertices[131] = {
#include "stg21/terrain/draw_199_vertices.inc"
};
static NJS_CNK_MODEL draw_199_model = {
#include "stg21/terrain/draw_199_model.inc"
};
static NJS_CNK_OBJECT draw_199 = {
#include "stg21/terrain/draw_199.inc"
};
static Sint16 draw_198_polygons[113] = {
#include "stg21/terrain/draw_198_polygons.inc"
};
static Sint32 draw_198_vertices[83] = {
#include "stg21/terrain/draw_198_vertices.inc"
};
static NJS_CNK_MODEL draw_198_model = {
#include "stg21/terrain/draw_198_model.inc"
};
static NJS_CNK_OBJECT draw_198 = {
#include "stg21/terrain/draw_198.inc"
};
static Sint16 draw_197_polygons[141] = {
#include "stg21/terrain/draw_197_polygons.inc"
};
static Sint32 draw_197_vertices[99] = {
#include "stg21/terrain/draw_197_vertices.inc"
};
static NJS_CNK_MODEL draw_197_model = {
#include "stg21/terrain/draw_197_model.inc"
};
static NJS_CNK_OBJECT draw_197 = {
#include "stg21/terrain/draw_197.inc"
};
static Sint16 draw_196_polygons[71] = {
#include "stg21/terrain/draw_196_polygons.inc"
};
static Sint32 draw_196_vertices[51] = {
#include "stg21/terrain/draw_196_vertices.inc"
};
static NJS_CNK_MODEL draw_196_model = {
#include "stg21/terrain/draw_196_model.inc"
};
static NJS_CNK_OBJECT draw_196 = {
#include "stg21/terrain/draw_196.inc"
};
static Sint16 draw_145_polygons[273] = {
#include "stg21/terrain/draw_145_polygons.inc"
};
static Sint32 draw_145_vertices[187] = {
#include "stg21/terrain/draw_145_vertices.inc"
};
static NJS_CNK_MODEL draw_145_model = {
#include "stg21/terrain/draw_145_model.inc"
};
static NJS_CNK_OBJECT draw_147 = {
#include "stg21/terrain/draw_147.inc"
};
static Sint16 draw_142_polygons[205] = {
#include "stg21/terrain/draw_142_polygons.inc"
};
static Sint32 draw_142_vertices[147] = {
#include "stg21/terrain/draw_142_vertices.inc"
};
static NJS_CNK_MODEL draw_142_model = {
#include "stg21/terrain/draw_142_model.inc"
};
static NJS_CNK_OBJECT draw_144 = {
#include "stg21/terrain/draw_144.inc"
};
static Sint16 draw_139_polygons[211] = {
#include "stg21/terrain/draw_139_polygons.inc"
};
static Sint32 draw_139_vertices[143] = {
#include "stg21/terrain/draw_139_vertices.inc"
};
static NJS_CNK_MODEL draw_139_model = {
#include "stg21/terrain/draw_139_model.inc"
};
static NJS_CNK_OBJECT draw_141 = {
#include "stg21/terrain/draw_141.inc"
};
static Sint16 draw_136_polygons[239] = {
#include "stg21/terrain/draw_136_polygons.inc"
};
static Sint32 draw_136_vertices[163] = {
#include "stg21/terrain/draw_136_vertices.inc"
};
static NJS_CNK_MODEL draw_136_model = {
#include "stg21/terrain/draw_136_model.inc"
};
static NJS_CNK_OBJECT draw_138 = {
#include "stg21/terrain/draw_138.inc"
};
static Sint16 draw_133_polygons[321] = {
#include "stg21/terrain/draw_133_polygons.inc"
};
static Sint32 draw_133_vertices[203] = {
#include "stg21/terrain/draw_133_vertices.inc"
};
static NJS_CNK_MODEL draw_133_model = {
#include "stg21/terrain/draw_133_model.inc"
};
static NJS_CNK_OBJECT draw_135 = {
#include "stg21/terrain/draw_135.inc"
};
static Sint16 draw_132_polygons[385] = {
#include "stg21/terrain/draw_132_polygons.inc"
};
static Sint32 draw_132_vertices[279] = {
#include "stg21/terrain/draw_132_vertices.inc"
};
static NJS_CNK_MODEL draw_132_model = {
#include "stg21/terrain/draw_132_model.inc"
};
static NJS_CNK_OBJECT draw_132 = {
#include "stg21/terrain/draw_132.inc"
};
static Sint16 draw_131_polygons[207] = {
#include "stg21/terrain/draw_131_polygons.inc"
};
static Sint32 draw_131_vertices[139] = {
#include "stg21/terrain/draw_131_vertices.inc"
};
static NJS_CNK_MODEL draw_131_model = {
#include "stg21/terrain/draw_131_model.inc"
};
static NJS_CNK_OBJECT draw_131 = {
#include "stg21/terrain/draw_131.inc"
};
static Sint16 draw_130_polygons[289] = {
#include "stg21/terrain/draw_130_polygons.inc"
};
static Sint32 draw_130_vertices[227] = {
#include "stg21/terrain/draw_130_vertices.inc"
};
static NJS_CNK_MODEL draw_130_model = {
#include "stg21/terrain/draw_130_model.inc"
};
static NJS_CNK_OBJECT draw_130 = {
#include "stg21/terrain/draw_130.inc"
};
static Sint16 draw_129_polygons[145] = {
#include "stg21/terrain/draw_129_polygons.inc"
};
static Sint32 draw_129_vertices[99] = {
#include "stg21/terrain/draw_129_vertices.inc"
};
static NJS_CNK_MODEL draw_129_model = {
#include "stg21/terrain/draw_129_model.inc"
};
static NJS_CNK_OBJECT draw_129 = {
#include "stg21/terrain/draw_129.inc"
};
static Sint16 draw_128_polygons[89] = {
#include "stg21/terrain/draw_128_polygons.inc"
};
static Sint32 draw_128_vertices[83] = {
#include "stg21/terrain/draw_128_vertices.inc"
};
static NJS_CNK_MODEL draw_128_model = {
#include "stg21/terrain/draw_128_model.inc"
};
static NJS_CNK_OBJECT draw_128 = {
#include "stg21/terrain/draw_128.inc"
};
static Sint16 draw_127_polygons[295] = {
#include "stg21/terrain/draw_127_polygons.inc"
};
static Sint32 draw_127_vertices[195] = {
#include "stg21/terrain/draw_127_vertices.inc"
};
static NJS_CNK_MODEL draw_127_model = {
#include "stg21/terrain/draw_127_model.inc"
};
static NJS_CNK_OBJECT draw_127 = {
#include "stg21/terrain/draw_127.inc"
};
static Sint16 draw_126_polygons[183] = {
#include "stg21/terrain/draw_126_polygons.inc"
};
static Sint32 draw_126_vertices[131] = {
#include "stg21/terrain/draw_126_vertices.inc"
};
static NJS_CNK_MODEL draw_126_model = {
#include "stg21/terrain/draw_126_model.inc"
};
static NJS_CNK_OBJECT draw_126 = {
#include "stg21/terrain/draw_126.inc"
};
static Sint16 draw_125_polygons[89] = {
#include "stg21/terrain/draw_125_polygons.inc"
};
static Sint32 draw_125_vertices[67] = {
#include "stg21/terrain/draw_125_vertices.inc"
};
static NJS_CNK_MODEL draw_125_model = {
#include "stg21/terrain/draw_125_model.inc"
};
static NJS_CNK_OBJECT draw_125 = {
#include "stg21/terrain/draw_125.inc"
};
static Sint16 draw_124_polygons[91] = {
#include "stg21/terrain/draw_124_polygons.inc"
};
static Sint32 draw_124_vertices[51] = {
#include "stg21/terrain/draw_124_vertices.inc"
};
static NJS_CNK_MODEL draw_124_model = {
#include "stg21/terrain/draw_124_model.inc"
};
static NJS_CNK_OBJECT draw_124 = {
#include "stg21/terrain/draw_124.inc"
};
static Sint16 draw_123_polygons[69] = {
#include "stg21/terrain/draw_123_polygons.inc"
};
static Sint32 draw_123_vertices[51] = {
#include "stg21/terrain/draw_123_vertices.inc"
};
static NJS_CNK_MODEL draw_123_model = {
#include "stg21/terrain/draw_123_model.inc"
};
static NJS_CNK_OBJECT draw_123 = {
#include "stg21/terrain/draw_123.inc"
};
static Sint16 draw_122_polygons[69] = {
#include "stg21/terrain/draw_122_polygons.inc"
};
static Sint32 draw_122_vertices[51] = {
#include "stg21/terrain/draw_122_vertices.inc"
};
static NJS_CNK_MODEL draw_122_model = {
#include "stg21/terrain/draw_122_model.inc"
};
static NJS_CNK_OBJECT draw_122 = {
#include "stg21/terrain/draw_122.inc"
};
static Sint16 draw_120_polygons[203] = {
#include "stg21/terrain/draw_120_polygons.inc"
};
static Sint32 draw_120_vertices[99] = {
#include "stg21/terrain/draw_120_vertices.inc"
};
static NJS_CNK_MODEL draw_120_model = {
#include "stg21/terrain/draw_120_model.inc"
};
static NJS_CNK_OBJECT draw_121 = {
#include "stg21/terrain/draw_121.inc"
};
static Sint16 draw_119_polygons[69] = {
#include "stg21/terrain/draw_119_polygons.inc"
};
static Sint32 draw_119_vertices[51] = {
#include "stg21/terrain/draw_119_vertices.inc"
};
static NJS_CNK_MODEL draw_119_model = {
#include "stg21/terrain/draw_119_model.inc"
};
static NJS_CNK_OBJECT draw_119 = {
#include "stg21/terrain/draw_119.inc"
};
static Sint16 draw_118_polygons[69] = {
#include "stg21/terrain/draw_118_polygons.inc"
};
static Sint32 draw_118_vertices[51] = {
#include "stg21/terrain/draw_118_vertices.inc"
};
static NJS_CNK_MODEL draw_118_model = {
#include "stg21/terrain/draw_118_model.inc"
};
static NJS_CNK_OBJECT draw_118 = {
#include "stg21/terrain/draw_118.inc"
};
static Sint16 draw_110_polygons[107] = {
#include "stg21/terrain/draw_110_polygons.inc"
};
static Sint32 draw_110_vertices[67] = {
#include "stg21/terrain/draw_110_vertices.inc"
};
static NJS_CNK_MODEL draw_110_model = {
#include "stg21/terrain/draw_110_model.inc"
};
static NJS_CNK_OBJECT draw_117 = {
#include "stg21/terrain/draw_117.inc"
};
static Sint16 draw_109_polygons[69] = {
#include "stg21/terrain/draw_109_polygons.inc"
};
static Sint32 draw_109_vertices[51] = {
#include "stg21/terrain/draw_109_vertices.inc"
};
static NJS_CNK_MODEL draw_109_model = {
#include "stg21/terrain/draw_109_model.inc"
};
static NJS_CNK_OBJECT draw_109 = {
#include "stg21/terrain/draw_109.inc"
};
static Sint16 draw_108_polygons[69] = {
#include "stg21/terrain/draw_108_polygons.inc"
};
static Sint32 draw_108_vertices[51] = {
#include "stg21/terrain/draw_108_vertices.inc"
};
static NJS_CNK_MODEL draw_108_model = {
#include "stg21/terrain/draw_108_model.inc"
};
static NJS_CNK_OBJECT draw_108 = {
#include "stg21/terrain/draw_108.inc"
};
static Sint16 draw_107_polygons[83] = {
#include "stg21/terrain/draw_107_polygons.inc"
};
static Sint32 draw_107_vertices[51] = {
#include "stg21/terrain/draw_107_vertices.inc"
};
static NJS_CNK_MODEL draw_107_model = {
#include "stg21/terrain/draw_107_model.inc"
};
static NJS_CNK_OBJECT draw_107 = {
#include "stg21/terrain/draw_107.inc"
};
static Sint16 draw_106_polygons[83] = {
#include "stg21/terrain/draw_106_polygons.inc"
};
static Sint32 draw_106_vertices[67] = {
#include "stg21/terrain/draw_106_vertices.inc"
};
static NJS_CNK_MODEL draw_106_model = {
#include "stg21/terrain/draw_106_model.inc"
};
static NJS_CNK_OBJECT draw_106 = {
#include "stg21/terrain/draw_106.inc"
};
static Sint16 draw_105_polygons[195] = {
#include "stg21/terrain/draw_105_polygons.inc"
};
static Sint32 draw_105_vertices[147] = {
#include "stg21/terrain/draw_105_vertices.inc"
};
static NJS_CNK_MODEL draw_105_model = {
#include "stg21/terrain/draw_105_model.inc"
};
static NJS_CNK_OBJECT draw_105 = {
#include "stg21/terrain/draw_105.inc"
};
static Sint16 draw_104_polygons[49] = {
#include "stg21/terrain/draw_104_polygons.inc"
};
static Sint32 draw_104_vertices[35] = {
#include "stg21/terrain/draw_104_vertices.inc"
};
static NJS_CNK_MODEL draw_104_model = {
#include "stg21/terrain/draw_104_model.inc"
};
static NJS_CNK_OBJECT draw_104 = {
#include "stg21/terrain/draw_104.inc"
};
static Sint16 draw_103_polygons[271] = {
#include "stg21/terrain/draw_103_polygons.inc"
};
static Sint32 draw_103_vertices[195] = {
#include "stg21/terrain/draw_103_vertices.inc"
};
static NJS_CNK_MODEL draw_103_model = {
#include "stg21/terrain/draw_103_model.inc"
};
static NJS_CNK_OBJECT draw_103 = {
#include "stg21/terrain/draw_103.inc"
};
static Sint16 draw_102_polygons[399] = {
#include "stg21/terrain/draw_102_polygons.inc"
};
static Sint32 draw_102_vertices[311] = {
#include "stg21/terrain/draw_102_vertices.inc"
};
static NJS_CNK_MODEL draw_102_model = {
#include "stg21/terrain/draw_102_model.inc"
};
static NJS_CNK_OBJECT draw_102 = {
#include "stg21/terrain/draw_102.inc"
};
static Sint16 draw_101_polygons[207] = {
#include "stg21/terrain/draw_101_polygons.inc"
};
static Sint32 draw_101_vertices[243] = {
#include "stg21/terrain/draw_101_vertices.inc"
};
static NJS_CNK_MODEL draw_101_model = {
#include "stg21/terrain/draw_101_model.inc"
};
static NJS_CNK_OBJECT draw_101 = {
#include "stg21/terrain/draw_101.inc"
};
static Sint16 draw_100_polygons[233] = {
#include "stg21/terrain/draw_100_polygons.inc"
};
static Sint32 draw_100_vertices[275] = {
#include "stg21/terrain/draw_100_vertices.inc"
};
static NJS_CNK_MODEL draw_100_model = {
#include "stg21/terrain/draw_100_model.inc"
};
static NJS_CNK_OBJECT draw_100 = {
#include "stg21/terrain/draw_100.inc"
};
static Sint16 draw_99_polygons[273] = {
#include "stg21/terrain/draw_99_polygons.inc"
};
static Sint32 draw_99_vertices[323] = {
#include "stg21/terrain/draw_99_vertices.inc"
};
static NJS_CNK_MODEL draw_99_model = {
#include "stg21/terrain/draw_99_model.inc"
};
static NJS_CNK_OBJECT draw_99 = {
#include "stg21/terrain/draw_99.inc"
};
static Sint16 draw_18_polygons[87] = {
#include "stg21/terrain/draw_18_polygons.inc"
};
static Sint32 draw_18_vertices[51] = {
#include "stg21/terrain/draw_18_vertices.inc"
};
static NJS_CNK_MODEL draw_18_model = {
#include "stg21/terrain/draw_18_model.inc"
};
static NJS_CNK_OBJECT draw_98 = {
#include "stg21/terrain/draw_98.inc"
};
static Sint16 draw_96_polygons[181] = {
#include "stg21/terrain/draw_96_polygons.inc"
};
static Sint32 draw_96_vertices[143] = {
#include "stg21/terrain/draw_96_vertices.inc"
};
static NJS_CNK_MODEL draw_96_model = {
#include "stg21/terrain/draw_96_model.inc"
};
static NJS_CNK_OBJECT draw_96 = {
#include "stg21/terrain/draw_96.inc"
};
static Sint16 draw_95_polygons[71] = {
#include "stg21/terrain/draw_95_polygons.inc"
};
static Sint32 draw_95_vertices[47] = {
#include "stg21/terrain/draw_95_vertices.inc"
};
static NJS_CNK_MODEL draw_95_model = {
#include "stg21/terrain/draw_95_model.inc"
};
static NJS_CNK_OBJECT draw_95 = {
#include "stg21/terrain/draw_95.inc"
};
static Sint16 draw_94_polygons[91] = {
#include "stg21/terrain/draw_94_polygons.inc"
};
static Sint32 draw_94_vertices[99] = {
#include "stg21/terrain/draw_94_vertices.inc"
};
static NJS_CNK_MODEL draw_94_model = {
#include "stg21/terrain/draw_94_model.inc"
};
static NJS_CNK_OBJECT draw_94 = {
#include "stg21/terrain/draw_94.inc"
};
static Sint16 draw_17_polygons[243] = {
#include "stg21/terrain/draw_17_polygons.inc"
};
static Sint32 draw_17_vertices[147] = {
#include "stg21/terrain/draw_17_vertices.inc"
};
static NJS_CNK_MODEL draw_17_model = {
#include "stg21/terrain/draw_17_model.inc"
};
static NJS_CNK_OBJECT draw_93 = {
#include "stg21/terrain/draw_93.inc"
};
static Sint16 draw_16_polygons[569] = {
#include "stg21/terrain/draw_16_polygons.inc"
};
static Sint32 draw_16_vertices[587] = {
#include "stg21/terrain/draw_16_vertices.inc"
};
static NJS_CNK_MODEL draw_16_model = {
#include "stg21/terrain/draw_16_model.inc"
};
static NJS_CNK_OBJECT draw_92 = {
#include "stg21/terrain/draw_92.inc"
};
static Sint16 draw_91_polygons[141] = {
#include "stg21/terrain/draw_91_polygons.inc"
};
static Sint32 draw_91_vertices[99] = {
#include "stg21/terrain/draw_91_vertices.inc"
};
static NJS_CNK_MODEL draw_91_model = {
#include "stg21/terrain/draw_91_model.inc"
};
static NJS_CNK_OBJECT draw_91 = {
#include "stg21/terrain/draw_91.inc"
};
static Sint16 draw_85_polygons[141] = {
#include "stg21/terrain/draw_85_polygons.inc"
};
static Sint32 draw_85_vertices[99] = {
#include "stg21/terrain/draw_85_vertices.inc"
};
static NJS_CNK_MODEL draw_85_model = {
#include "stg21/terrain/draw_85_model.inc"
};
static NJS_CNK_OBJECT draw_90 = {
#include "stg21/terrain/draw_90.inc"
};
static Sint16 draw_61_polygons[199] = {
#include "stg21/terrain/draw_61_polygons.inc"
};
static Sint32 draw_61_vertices[187] = {
#include "stg21/terrain/draw_61_vertices.inc"
};
static NJS_CNK_MODEL draw_61_model = {
#include "stg21/terrain/draw_61_model.inc"
};
static NJS_CNK_OBJECT draw_75 = {
#include "stg21/terrain/draw_75.inc"
};
static Sint16 draw_57_polygons[455] = {
#include "stg21/terrain/draw_57_polygons.inc"
};
static Sint32 draw_57_vertices[383] = {
#include "stg21/terrain/draw_57_vertices.inc"
};
static NJS_CNK_MODEL draw_57_model = {
#include "stg21/terrain/draw_57_model.inc"
};
static NJS_CNK_OBJECT draw_60 = {
#include "stg21/terrain/draw_60.inc"
};
static Sint16 draw_53_polygons[367] = {
#include "stg21/terrain/draw_53_polygons.inc"
};
static Sint32 draw_53_vertices[299] = {
#include "stg21/terrain/draw_53_vertices.inc"
};
static NJS_CNK_MODEL draw_53_model = {
#include "stg21/terrain/draw_53_model.inc"
};
static NJS_CNK_OBJECT draw_56 = {
#include "stg21/terrain/draw_56.inc"
};
static Sint16 draw_49_polygons[117] = {
#include "stg21/terrain/draw_49_polygons.inc"
};
static Sint32 draw_49_vertices[131] = {
#include "stg21/terrain/draw_49_vertices.inc"
};
static NJS_CNK_MODEL draw_49_model = {
#include "stg21/terrain/draw_49_model.inc"
};
static NJS_CNK_OBJECT draw_52 = {
#include "stg21/terrain/draw_52.inc"
};
static Sint16 draw_46_polygons[977] = {
#include "stg21/terrain/draw_46_polygons.inc"
};
static Sint32 draw_46_vertices[639] = {
#include "stg21/terrain/draw_46_vertices.inc"
};
static NJS_CNK_MODEL draw_46_model = {
#include "stg21/terrain/draw_46_model.inc"
};
static NJS_CNK_OBJECT draw_48 = {
#include "stg21/terrain/draw_48.inc"
};
static Sint16 draw_41_polygons[211] = {
#include "stg21/terrain/draw_41_polygons.inc"
};
static Sint32 draw_41_vertices[135] = {
#include "stg21/terrain/draw_41_vertices.inc"
};
static NJS_CNK_MODEL draw_41_model = {
#include "stg21/terrain/draw_41_model.inc"
};
static NJS_CNK_OBJECT draw_45 = {
#include "stg21/terrain/draw_45.inc"
};
static Sint16 draw_36_polygons[569] = {
#include "stg21/terrain/draw_36_polygons.inc"
};
static Sint32 draw_36_vertices[591] = {
#include "stg21/terrain/draw_36_vertices.inc"
};
static NJS_CNK_MODEL draw_36_model = {
#include "stg21/terrain/draw_36_model.inc"
};
static NJS_CNK_OBJECT draw_40 = {
#include "stg21/terrain/draw_40.inc"
};
static Sint16 draw_34_polygons[87] = {
#include "stg21/terrain/draw_34_polygons.inc"
};
static Sint32 draw_34_vertices[51] = {
#include "stg21/terrain/draw_34_vertices.inc"
};
static NJS_CNK_MODEL draw_34_model = {
#include "stg21/terrain/draw_34_model.inc"
};
static NJS_CNK_OBJECT draw_35 = {
#include "stg21/terrain/draw_35.inc"
};
static Sint16 draw_33_polygons[259] = {
#include "stg21/terrain/draw_33_polygons.inc"
};
static Sint32 draw_33_vertices[207] = {
#include "stg21/terrain/draw_33_vertices.inc"
};
static NJS_CNK_MODEL draw_33_model = {
#include "stg21/terrain/draw_33_model.inc"
};
static NJS_CNK_OBJECT draw_33 = {
#include "stg21/terrain/draw_33.inc"
};
static Sint16 draw_32_polygons[259] = {
#include "stg21/terrain/draw_32_polygons.inc"
};
static Sint32 draw_32_vertices[207] = {
#include "stg21/terrain/draw_32_vertices.inc"
};
static NJS_CNK_MODEL draw_32_model = {
#include "stg21/terrain/draw_32_model.inc"
};
static NJS_CNK_OBJECT draw_32 = {
#include "stg21/terrain/draw_32.inc"
};
static Sint16 draw_31_polygons[117] = {
#include "stg21/terrain/draw_31_polygons.inc"
};
static Sint32 draw_31_vertices[131] = {
#include "stg21/terrain/draw_31_vertices.inc"
};
static NJS_CNK_MODEL draw_31_model = {
#include "stg21/terrain/draw_31_model.inc"
};
static NJS_CNK_OBJECT draw_31 = {
#include "stg21/terrain/draw_31.inc"
};
static Sint16 draw_30_polygons[117] = {
#include "stg21/terrain/draw_30_polygons.inc"
};
static Sint32 draw_30_vertices[131] = {
#include "stg21/terrain/draw_30_vertices.inc"
};
static NJS_CNK_MODEL draw_30_model = {
#include "stg21/terrain/draw_30_model.inc"
};
static NJS_CNK_OBJECT draw_30 = {
#include "stg21/terrain/draw_30.inc"
};
static Sint16 draw_25_polygons[81] = {
#include "stg21/terrain/draw_25_polygons.inc"
};
static Sint32 draw_25_vertices[55] = {
#include "stg21/terrain/draw_25_vertices.inc"
};
static NJS_CNK_MODEL draw_25_model = {
#include "stg21/terrain/draw_25_model.inc"
};
static NJS_CNK_OBJECT draw_29 = {
#include "stg21/terrain/draw_29.inc"
};
static Sint16 draw_20_polygons[81] = {
#include "stg21/terrain/draw_20_polygons.inc"
};
static Sint32 draw_20_vertices[55] = {
#include "stg21/terrain/draw_20_vertices.inc"
};
static NJS_CNK_MODEL draw_20_model = {
#include "stg21/terrain/draw_20_model.inc"
};
static NJS_CNK_OBJECT draw_24 = {
#include "stg21/terrain/draw_24.inc"
};
static Sint16 draw_14_polygons[205] = {
#include "stg21/terrain/draw_14_polygons.inc"
};
static Sint32 draw_14_vertices[135] = {
#include "stg21/terrain/draw_14_vertices.inc"
};
static NJS_CNK_MODEL draw_14_model = {
#include "stg21/terrain/draw_14_model.inc"
};
static NJS_CNK_OBJECT draw_15 = {
#include "stg21/terrain/draw_15.inc"
};
static Sint16 draw_12_polygons[435] = {
#include "stg21/terrain/draw_12_polygons.inc"
};
static Sint32 draw_12_vertices[195] = {
#include "stg21/terrain/draw_12_vertices.inc"
};
static NJS_CNK_MODEL draw_12_model = {
#include "stg21/terrain/draw_12_model.inc"
};
static NJS_CNK_OBJECT draw_13 = {
#include "stg21/terrain/draw_13.inc"
};
static Sint16 draw_10_polygons[447] = {
#include "stg21/terrain/draw_10_polygons.inc"
};
static Sint32 draw_10_vertices[243] = {
#include "stg21/terrain/draw_10_vertices.inc"
};
static NJS_CNK_MODEL draw_10_model = {
#include "stg21/terrain/draw_10_model.inc"
};
static NJS_CNK_OBJECT draw_11 = {
#include "stg21/terrain/draw_11.inc"
};
static Sint16 draw_8_polygons[101] = {
#include "stg21/terrain/draw_8_polygons.inc"
};
static Sint32 draw_8_vertices[71] = {
#include "stg21/terrain/draw_8_vertices.inc"
};
static NJS_CNK_MODEL draw_8_model = {
#include "stg21/terrain/draw_8_model.inc"
};
static NJS_CNK_OBJECT draw_9 = {
#include "stg21/terrain/draw_9.inc"
};
static Sint16 draw_6_polygons[195] = {
#include "stg21/terrain/draw_6_polygons.inc"
};
static Sint32 draw_6_vertices[115] = {
#include "stg21/terrain/draw_6_vertices.inc"
};
static NJS_CNK_MODEL draw_6_model = {
#include "stg21/terrain/draw_6_model.inc"
};
static NJS_CNK_OBJECT draw_7 = {
#include "stg21/terrain/draw_7.inc"
};
static Sint16 draw_4_polygons[167] = {
#include "stg21/terrain/draw_4_polygons.inc"
};
static Sint32 draw_4_vertices[111] = {
#include "stg21/terrain/draw_4_vertices.inc"
};
static NJS_CNK_MODEL draw_4_model = {
#include "stg21/terrain/draw_4_model.inc"
};
static NJS_CNK_OBJECT draw_5 = {
#include "stg21/terrain/draw_5.inc"
};
static Sint16 draw_3_polygons[315] = {
#include "stg21/terrain/draw_3_polygons.inc"
};
static Sint32 draw_3_vertices[255] = {
#include "stg21/terrain/draw_3_vertices.inc"
};
static NJS_CNK_MODEL draw_3_model = {
#include "stg21/terrain/draw_3_model.inc"
};
static NJS_CNK_OBJECT draw_3 = {
#include "stg21/terrain/draw_3.inc"
};
static Sint16 draw_0_polygons[449] = {
#include "stg21/terrain/draw_0_polygons.inc"
};
static Sint32 draw_0_vertices[431] = {
#include "stg21/terrain/draw_0_vertices.inc"
};
static NJS_CNK_MODEL draw_0_model = {
#include "stg21/terrain/draw_0_model.inc"
};
static NJS_CNK_OBJECT draw_2 = {
#include "stg21/terrain/draw_2.inc"
};
static NJS_MATERIAL collision_0_materials[1] = {
#include "stg21/terrain/collision_0_materials.inc"
};
static Sint16 collision_0_faces[11] = {
#include "stg21/terrain/collision_0_faces.inc"
};
static NJS_MESHSET collision_0_meshsets[1] = {
#include "stg21/terrain/collision_0_meshsets.inc"
};
static NJS_POINT3 collision_0_points[8] = {
#include "stg21/terrain/collision_0_points.inc"
};
static NJS_MODEL collision_0_model = {
#include "stg21/terrain/collision_0_model.inc"
};
static NJS_OBJECT collision_5 = {
#include "stg21/terrain/collision_5.inc"
};
static NJS_MATERIAL collision_20_materials[1] = {
#include "stg21/terrain/collision_20_materials.inc"
};
static Sint16 collision_20_faces[22] = {
#include "stg21/terrain/collision_20_faces.inc"
};
static NJS_MESHSET collision_20_meshsets[1] = {
#include "stg21/terrain/collision_20_meshsets.inc"
};
static NJS_POINT3 collision_20_points[8] = {
#include "stg21/terrain/collision_20_points.inc"
};
static NJS_MODEL collision_20_model = {
#include "stg21/terrain/collision_20_model.inc"
};
static NJS_OBJECT collision_20 = {
#include "stg21/terrain/collision_20.inc"
};
static NJS_MATERIAL collision_22_materials[1] = {
#include "stg21/terrain/collision_22_materials.inc"
};
static Sint16 collision_22_faces[17] = {
#include "stg21/terrain/collision_22_faces.inc"
};
static NJS_MESHSET collision_22_meshsets[1] = {
#include "stg21/terrain/collision_22_meshsets.inc"
};
static NJS_POINT3 collision_22_points[8] = {
#include "stg21/terrain/collision_22_points.inc"
};
static NJS_MODEL collision_22_model = {
#include "stg21/terrain/collision_22_model.inc"
};
static NJS_OBJECT collision_22 = {
#include "stg21/terrain/collision_22.inc"
};
static NJS_MATERIAL collision_24_materials[1] = {
#include "stg21/terrain/collision_24_materials.inc"
};
static Sint16 collision_24_faces[11] = {
#include "stg21/terrain/collision_24_faces.inc"
};
static NJS_MESHSET collision_24_meshsets[1] = {
#include "stg21/terrain/collision_24_meshsets.inc"
};
static NJS_POINT3 collision_24_points[8] = {
#include "stg21/terrain/collision_24_points.inc"
};
static NJS_MODEL collision_24_model = {
#include "stg21/terrain/collision_24_model.inc"
};
static NJS_OBJECT collision_24 = {
#include "stg21/terrain/collision_24.inc"
};
static NJS_MATERIAL collision_25_materials[1] = {
#include "stg21/terrain/collision_25_materials.inc"
};
static Sint16 collision_25_faces[11] = {
#include "stg21/terrain/collision_25_faces.inc"
};
static NJS_MESHSET collision_25_meshsets[1] = {
#include "stg21/terrain/collision_25_meshsets.inc"
};
static NJS_POINT3 collision_25_points[8] = {
#include "stg21/terrain/collision_25_points.inc"
};
static NJS_MODEL collision_25_model = {
#include "stg21/terrain/collision_25_model.inc"
};
static NJS_OBJECT collision_25 = {
#include "stg21/terrain/collision_25.inc"
};
static NJS_MATERIAL collision_26_materials[1] = {
#include "stg21/terrain/collision_26_materials.inc"
};
static Sint16 collision_26_faces[11] = {
#include "stg21/terrain/collision_26_faces.inc"
};
static NJS_MESHSET collision_26_meshsets[1] = {
#include "stg21/terrain/collision_26_meshsets.inc"
};
static NJS_POINT3 collision_26_points[8] = {
#include "stg21/terrain/collision_26_points.inc"
};
static NJS_MODEL collision_26_model = {
#include "stg21/terrain/collision_26_model.inc"
};
static NJS_OBJECT collision_26 = {
#include "stg21/terrain/collision_26.inc"
};
static NJS_MATERIAL collision_27_materials[1] = {
#include "stg21/terrain/collision_27_materials.inc"
};
static Sint16 collision_27_faces[11] = {
#include "stg21/terrain/collision_27_faces.inc"
};
static NJS_MESHSET collision_27_meshsets[1] = {
#include "stg21/terrain/collision_27_meshsets.inc"
};
static NJS_POINT3 collision_27_points[8] = {
#include "stg21/terrain/collision_27_points.inc"
};
static NJS_MODEL collision_27_model = {
#include "stg21/terrain/collision_27_model.inc"
};
static NJS_OBJECT collision_27 = {
#include "stg21/terrain/collision_27.inc"
};
static NJS_MATERIAL collision_28_materials[1] = {
#include "stg21/terrain/collision_28_materials.inc"
};
static Sint16 collision_28_faces[11] = {
#include "stg21/terrain/collision_28_faces.inc"
};
static NJS_MESHSET collision_28_meshsets[1] = {
#include "stg21/terrain/collision_28_meshsets.inc"
};
static NJS_POINT3 collision_28_points[8] = {
#include "stg21/terrain/collision_28_points.inc"
};
static NJS_MODEL collision_28_model = {
#include "stg21/terrain/collision_28_model.inc"
};
static NJS_OBJECT collision_28 = {
#include "stg21/terrain/collision_28.inc"
};
static NJS_MATERIAL collision_29_materials[1] = {
#include "stg21/terrain/collision_29_materials.inc"
};
static Sint16 collision_29_faces[19] = {
#include "stg21/terrain/collision_29_faces.inc"
};
static NJS_MESHSET collision_29_meshsets[1] = {
#include "stg21/terrain/collision_29_meshsets.inc"
};
static NJS_POINT3 collision_29_points[8] = {
#include "stg21/terrain/collision_29_points.inc"
};
static NJS_MODEL collision_29_model = {
#include "stg21/terrain/collision_29_model.inc"
};
static NJS_OBJECT collision_29 = {
#include "stg21/terrain/collision_29.inc"
};
static NJS_MATERIAL collision_33_materials[1] = {
#include "stg21/terrain/collision_33_materials.inc"
};
static Sint16 collision_33_faces[51] = {
#include "stg21/terrain/collision_33_faces.inc"
};
static NJS_MESHSET collision_33_meshsets[1] = {
#include "stg21/terrain/collision_33_meshsets.inc"
};
static NJS_POINT3 collision_33_points[27] = {
#include "stg21/terrain/collision_33_points.inc"
};
static NJS_MODEL collision_33_model = {
#include "stg21/terrain/collision_33_model.inc"
};
static NJS_OBJECT collision_33 = {
#include "stg21/terrain/collision_33.inc"
};
static NJS_MATERIAL collision_35_materials[1] = {
#include "stg21/terrain/collision_35_materials.inc"
};
static Sint16 collision_35_faces[45] = {
#include "stg21/terrain/collision_35_faces.inc"
};
static NJS_MESHSET collision_35_meshsets[1] = {
#include "stg21/terrain/collision_35_meshsets.inc"
};
static NJS_POINT3 collision_35_points[20] = {
#include "stg21/terrain/collision_35_points.inc"
};
static NJS_MODEL collision_35_model = {
#include "stg21/terrain/collision_35_model.inc"
};
static NJS_OBJECT collision_35 = {
#include "stg21/terrain/collision_35.inc"
};
static NJS_MATERIAL collision_37_materials[1] = {
#include "stg21/terrain/collision_37_materials.inc"
};
static Sint16 collision_37_faces[32] = {
#include "stg21/terrain/collision_37_faces.inc"
};
static NJS_MESHSET collision_37_meshsets[1] = {
#include "stg21/terrain/collision_37_meshsets.inc"
};
static NJS_POINT3 collision_37_points[14] = {
#include "stg21/terrain/collision_37_points.inc"
};
static NJS_MODEL collision_37_model = {
#include "stg21/terrain/collision_37_model.inc"
};
static NJS_OBJECT collision_37 = {
#include "stg21/terrain/collision_37.inc"
};
static NJS_MATERIAL collision_38_materials[1] = {
#include "stg21/terrain/collision_38_materials.inc"
};
static Sint16 collision_38_faces[23] = {
#include "stg21/terrain/collision_38_faces.inc"
};
static NJS_MESHSET collision_38_meshsets[1] = {
#include "stg21/terrain/collision_38_meshsets.inc"
};
static NJS_POINT3 collision_38_points[12] = {
#include "stg21/terrain/collision_38_points.inc"
};
static NJS_MODEL collision_38_model = {
#include "stg21/terrain/collision_38_model.inc"
};
static NJS_OBJECT collision_38 = {
#include "stg21/terrain/collision_38.inc"
};
static NJS_MATERIAL collision_39_materials[1] = {
#include "stg21/terrain/collision_39_materials.inc"
};
static Sint16 collision_39_faces[26] = {
#include "stg21/terrain/collision_39_faces.inc"
};
static NJS_MESHSET collision_39_meshsets[1] = {
#include "stg21/terrain/collision_39_meshsets.inc"
};
static NJS_POINT3 collision_39_points[12] = {
#include "stg21/terrain/collision_39_points.inc"
};
static NJS_MODEL collision_39_model = {
#include "stg21/terrain/collision_39_model.inc"
};
static NJS_OBJECT collision_39 = {
#include "stg21/terrain/collision_39.inc"
};
static NJS_MATERIAL collision_40_materials[1] = {
#include "stg21/terrain/collision_40_materials.inc"
};
static Sint16 collision_40_faces[30] = {
#include "stg21/terrain/collision_40_faces.inc"
};
static NJS_MESHSET collision_40_meshsets[1] = {
#include "stg21/terrain/collision_40_meshsets.inc"
};
static NJS_POINT3 collision_40_points[12] = {
#include "stg21/terrain/collision_40_points.inc"
};
static NJS_MODEL collision_40_model = {
#include "stg21/terrain/collision_40_model.inc"
};
static NJS_OBJECT collision_40 = {
#include "stg21/terrain/collision_40.inc"
};
static NJS_MATERIAL collision_41_materials[1] = {
#include "stg21/terrain/collision_41_materials.inc"
};
static Sint16 collision_41_faces[14] = {
#include "stg21/terrain/collision_41_faces.inc"
};
static NJS_MESHSET collision_41_meshsets[1] = {
#include "stg21/terrain/collision_41_meshsets.inc"
};
static NJS_POINT3 collision_41_points[8] = {
#include "stg21/terrain/collision_41_points.inc"
};
static NJS_MODEL collision_41_model = {
#include "stg21/terrain/collision_41_model.inc"
};
static NJS_OBJECT collision_41 = {
#include "stg21/terrain/collision_41.inc"
};
static NJS_MATERIAL collision_42_materials[1] = {
#include "stg21/terrain/collision_42_materials.inc"
};
static Sint16 collision_42_faces[16] = {
#include "stg21/terrain/collision_42_faces.inc"
};
static NJS_MESHSET collision_42_meshsets[1] = {
#include "stg21/terrain/collision_42_meshsets.inc"
};
static NJS_POINT3 collision_42_points[8] = {
#include "stg21/terrain/collision_42_points.inc"
};
static NJS_MODEL collision_42_model = {
#include "stg21/terrain/collision_42_model.inc"
};
static NJS_OBJECT collision_42 = {
#include "stg21/terrain/collision_42.inc"
};
static NJS_MATERIAL collision_43_materials[1] = {
#include "stg21/terrain/collision_43_materials.inc"
};
static Sint16 collision_43_faces[66] = {
#include "stg21/terrain/collision_43_faces.inc"
};
static NJS_MESHSET collision_43_meshsets[1] = {
#include "stg21/terrain/collision_43_meshsets.inc"
};
static NJS_POINT3 collision_43_points[32] = {
#include "stg21/terrain/collision_43_points.inc"
};
static NJS_MODEL collision_43_model = {
#include "stg21/terrain/collision_43_model.inc"
};
static NJS_OBJECT collision_43 = {
#include "stg21/terrain/collision_43.inc"
};
static NJS_MATERIAL collision_44_materials[1] = {
#include "stg21/terrain/collision_44_materials.inc"
};
static Sint16 collision_44_faces[39] = {
#include "stg21/terrain/collision_44_faces.inc"
};
static NJS_MESHSET collision_44_meshsets[1] = {
#include "stg21/terrain/collision_44_meshsets.inc"
};
static NJS_POINT3 collision_44_points[18] = {
#include "stg21/terrain/collision_44_points.inc"
};
static NJS_MODEL collision_44_model = {
#include "stg21/terrain/collision_44_model.inc"
};
static NJS_OBJECT collision_44 = {
#include "stg21/terrain/collision_44.inc"
};
static NJS_MATERIAL collision_46_materials[1] = {
#include "stg21/terrain/collision_46_materials.inc"
};
static Sint16 collision_46_faces[97] = {
#include "stg21/terrain/collision_46_faces.inc"
};
static NJS_MESHSET collision_46_meshsets[1] = {
#include "stg21/terrain/collision_46_meshsets.inc"
};
static NJS_POINT3 collision_46_points[37] = {
#include "stg21/terrain/collision_46_points.inc"
};
static NJS_MODEL collision_46_model = {
#include "stg21/terrain/collision_46_model.inc"
};
static NJS_OBJECT collision_46 = {
#include "stg21/terrain/collision_46.inc"
};
static NJS_MATERIAL collision_50_materials[1] = {
#include "stg21/terrain/collision_50_materials.inc"
};
static Sint16 collision_50_faces[120] = {
#include "stg21/terrain/collision_50_faces.inc"
};
static NJS_MESHSET collision_50_meshsets[1] = {
#include "stg21/terrain/collision_50_meshsets.inc"
};
static NJS_POINT3 collision_50_points[44] = {
#include "stg21/terrain/collision_50_points.inc"
};
static NJS_MODEL collision_50_model = {
#include "stg21/terrain/collision_50_model.inc"
};
static NJS_OBJECT collision_50 = {
#include "stg21/terrain/collision_50.inc"
};
static NJS_MATERIAL collision_53_materials[1] = {
#include "stg21/terrain/collision_53_materials.inc"
};
static Sint16 collision_53_faces[172] = {
#include "stg21/terrain/collision_53_faces.inc"
};
static NJS_MESHSET collision_53_meshsets[1] = {
#include "stg21/terrain/collision_53_meshsets.inc"
};
static NJS_POINT3 collision_53_points[54] = {
#include "stg21/terrain/collision_53_points.inc"
};
static NJS_MODEL collision_53_model = {
#include "stg21/terrain/collision_53_model.inc"
};
static NJS_OBJECT collision_53 = {
#include "stg21/terrain/collision_53.inc"
};
static NJS_MATERIAL collision_62_materials[1] = {
#include "stg21/terrain/collision_62_materials.inc"
};
static Sint16 collision_62_faces[73] = {
#include "stg21/terrain/collision_62_faces.inc"
};
static NJS_MESHSET collision_62_meshsets[1] = {
#include "stg21/terrain/collision_62_meshsets.inc"
};
static NJS_POINT3 collision_62_points[32] = {
#include "stg21/terrain/collision_62_points.inc"
};
static NJS_MODEL collision_62_model = {
#include "stg21/terrain/collision_62_model.inc"
};
static NJS_OBJECT collision_62 = {
#include "stg21/terrain/collision_62.inc"
};
static NJS_MATERIAL collision_63_materials[1] = {
#include "stg21/terrain/collision_63_materials.inc"
};
static Sint16 collision_63_faces[31] = {
#include "stg21/terrain/collision_63_faces.inc"
};
static NJS_MESHSET collision_63_meshsets[1] = {
#include "stg21/terrain/collision_63_meshsets.inc"
};
static NJS_POINT3 collision_63_points[14] = {
#include "stg21/terrain/collision_63_points.inc"
};
static NJS_MODEL collision_63_model = {
#include "stg21/terrain/collision_63_model.inc"
};
static NJS_OBJECT collision_63 = {
#include "stg21/terrain/collision_63.inc"
};
static NJS_MATERIAL collision_64_materials[1] = {
#include "stg21/terrain/collision_64_materials.inc"
};
static Sint16 collision_64_faces[198] = {
#include "stg21/terrain/collision_64_faces.inc"
};
static NJS_MESHSET collision_64_meshsets[1] = {
#include "stg21/terrain/collision_64_meshsets.inc"
};
static NJS_POINT3 collision_64_points[81] = {
#include "stg21/terrain/collision_64_points.inc"
};
static NJS_MODEL collision_64_model = {
#include "stg21/terrain/collision_64_model.inc"
};
static NJS_OBJECT collision_64 = {
#include "stg21/terrain/collision_64.inc"
};
static NJS_MATERIAL collision_66_materials[1] = {
#include "stg21/terrain/collision_66_materials.inc"
};
static Sint16 collision_66_faces[35] = {
#include "stg21/terrain/collision_66_faces.inc"
};
static NJS_MESHSET collision_66_meshsets[1] = {
#include "stg21/terrain/collision_66_meshsets.inc"
};
static NJS_POINT3 collision_66_points[14] = {
#include "stg21/terrain/collision_66_points.inc"
};
static NJS_MODEL collision_66_model = {
#include "stg21/terrain/collision_66_model.inc"
};
static NJS_OBJECT collision_66 = {
#include "stg21/terrain/collision_66.inc"
};
static NJS_MATERIAL collision_67_materials[1] = {
#include "stg21/terrain/collision_67_materials.inc"
};
static Sint16 collision_67_faces[97] = {
#include "stg21/terrain/collision_67_faces.inc"
};
static NJS_MESHSET collision_67_meshsets[1] = {
#include "stg21/terrain/collision_67_meshsets.inc"
};
static NJS_POINT3 collision_67_points[44] = {
#include "stg21/terrain/collision_67_points.inc"
};
static NJS_MODEL collision_67_model = {
#include "stg21/terrain/collision_67_model.inc"
};
static NJS_OBJECT collision_67 = {
#include "stg21/terrain/collision_67.inc"
};
static NJS_MATERIAL collision_69_materials[1] = {
#include "stg21/terrain/collision_69_materials.inc"
};
static Sint16 collision_69_faces[124] = {
#include "stg21/terrain/collision_69_faces.inc"
};
static NJS_MESHSET collision_69_meshsets[1] = {
#include "stg21/terrain/collision_69_meshsets.inc"
};
static NJS_POINT3 collision_69_points[53] = {
#include "stg21/terrain/collision_69_points.inc"
};
static NJS_MODEL collision_69_model = {
#include "stg21/terrain/collision_69_model.inc"
};
static NJS_OBJECT collision_69 = {
#include "stg21/terrain/collision_69.inc"
};
static NJS_MATERIAL collision_71_materials[1] = {
#include "stg21/terrain/collision_71_materials.inc"
};
static Sint16 collision_71_faces[59] = {
#include "stg21/terrain/collision_71_faces.inc"
};
static NJS_MESHSET collision_71_meshsets[1] = {
#include "stg21/terrain/collision_71_meshsets.inc"
};
static NJS_POINT3 collision_71_points[26] = {
#include "stg21/terrain/collision_71_points.inc"
};
static NJS_MODEL collision_71_model = {
#include "stg21/terrain/collision_71_model.inc"
};
static NJS_OBJECT collision_71 = {
#include "stg21/terrain/collision_71.inc"
};
static NJS_MATERIAL collision_73_materials[1] = {
#include "stg21/terrain/collision_73_materials.inc"
};
static Sint16 collision_73_faces[59] = {
#include "stg21/terrain/collision_73_faces.inc"
};
static NJS_MESHSET collision_73_meshsets[1] = {
#include "stg21/terrain/collision_73_meshsets.inc"
};
static NJS_POINT3 collision_73_points[26] = {
#include "stg21/terrain/collision_73_points.inc"
};
static NJS_MODEL collision_73_model = {
#include "stg21/terrain/collision_73_model.inc"
};
static NJS_OBJECT collision_73 = {
#include "stg21/terrain/collision_73.inc"
};
static NJS_MATERIAL collision_75_materials[1] = {
#include "stg21/terrain/collision_75_materials.inc"
};
static Sint16 collision_75_faces[60] = {
#include "stg21/terrain/collision_75_faces.inc"
};
static NJS_MESHSET collision_75_meshsets[1] = {
#include "stg21/terrain/collision_75_meshsets.inc"
};
static NJS_POINT3 collision_75_points[29] = {
#include "stg21/terrain/collision_75_points.inc"
};
static NJS_MODEL collision_75_model = {
#include "stg21/terrain/collision_75_model.inc"
};
static NJS_OBJECT collision_75 = {
#include "stg21/terrain/collision_75.inc"
};
static NJS_MATERIAL collision_76_materials[1] = {
#include "stg21/terrain/collision_76_materials.inc"
};
static Sint16 collision_76_faces[92] = {
#include "stg21/terrain/collision_76_faces.inc"
};
static NJS_MESHSET collision_76_meshsets[1] = {
#include "stg21/terrain/collision_76_meshsets.inc"
};
static NJS_POINT3 collision_76_points[53] = {
#include "stg21/terrain/collision_76_points.inc"
};
static NJS_MODEL collision_76_model = {
#include "stg21/terrain/collision_76_model.inc"
};
static NJS_OBJECT collision_76 = {
#include "stg21/terrain/collision_76.inc"
};
static NJS_MATERIAL collision_77_materials[1] = {
#include "stg21/terrain/collision_77_materials.inc"
};
static Sint16 collision_77_faces[193] = {
#include "stg21/terrain/collision_77_faces.inc"
};
static NJS_MESHSET collision_77_meshsets[1] = {
#include "stg21/terrain/collision_77_meshsets.inc"
};
static NJS_POINT3 collision_77_points[90] = {
#include "stg21/terrain/collision_77_points.inc"
};
static NJS_MODEL collision_77_model = {
#include "stg21/terrain/collision_77_model.inc"
};
static NJS_OBJECT collision_77 = {
#include "stg21/terrain/collision_77.inc"
};
static NJS_MATERIAL collision_78_materials[1] = {
#include "stg21/terrain/collision_78_materials.inc"
};
static Sint16 collision_78_faces[16] = {
#include "stg21/terrain/collision_78_faces.inc"
};
static NJS_MESHSET collision_78_meshsets[1] = {
#include "stg21/terrain/collision_78_meshsets.inc"
};
static NJS_POINT3 collision_78_points[8] = {
#include "stg21/terrain/collision_78_points.inc"
};
static NJS_MODEL collision_78_model = {
#include "stg21/terrain/collision_78_model.inc"
};
static NJS_OBJECT collision_78 = {
#include "stg21/terrain/collision_78.inc"
};
static NJS_MATERIAL collision_79_materials[1] = {
#include "stg21/terrain/collision_79_materials.inc"
};
static Sint16 collision_79_faces[16] = {
#include "stg21/terrain/collision_79_faces.inc"
};
static NJS_MESHSET collision_79_meshsets[1] = {
#include "stg21/terrain/collision_79_meshsets.inc"
};
static NJS_POINT3 collision_79_points[8] = {
#include "stg21/terrain/collision_79_points.inc"
};
static NJS_MODEL collision_79_model = {
#include "stg21/terrain/collision_79_model.inc"
};
static NJS_OBJECT collision_79 = {
#include "stg21/terrain/collision_79.inc"
};
static NJS_MATERIAL collision_85_materials[1] = {
#include "stg21/terrain/collision_85_materials.inc"
};
static Sint16 collision_85_faces[21] = {
#include "stg21/terrain/collision_85_faces.inc"
};
static NJS_MESHSET collision_85_meshsets[1] = {
#include "stg21/terrain/collision_85_meshsets.inc"
};
static NJS_POINT3 collision_85_points[12] = {
#include "stg21/terrain/collision_85_points.inc"
};
static NJS_MODEL collision_85_model = {
#include "stg21/terrain/collision_85_model.inc"
};
static NJS_OBJECT collision_85 = {
#include "stg21/terrain/collision_85.inc"
};
static NJS_CNK_OBJECT draw_307 = {
#include "stg21/terrain/draw_307.inc"
};
static NJS_CNK_OBJECT draw_306 = {
#include "stg21/terrain/draw_306.inc"
};
static NJS_CNK_OBJECT draw_304 = {
#include "stg21/terrain/draw_304.inc"
};
static NJS_CNK_OBJECT draw_303 = {
#include "stg21/terrain/draw_303.inc"
};
static NJS_CNK_OBJECT draw_302 = {
#include "stg21/terrain/draw_302.inc"
};
static NJS_CNK_OBJECT draw_301 = {
#include "stg21/terrain/draw_301.inc"
};
static NJS_CNK_OBJECT draw_300 = {
#include "stg21/terrain/draw_300.inc"
};
static NJS_CNK_OBJECT draw_299 = {
#include "stg21/terrain/draw_299.inc"
};
static NJS_CNK_OBJECT draw_298 = {
#include "stg21/terrain/draw_298.inc"
};
static NJS_CNK_OBJECT draw_297 = {
#include "stg21/terrain/draw_297.inc"
};
static NJS_CNK_OBJECT draw_296 = {
#include "stg21/terrain/draw_296.inc"
};
static NJS_CNK_OBJECT draw_295 = {
#include "stg21/terrain/draw_295.inc"
};
static NJS_CNK_OBJECT draw_294 = {
#include "stg21/terrain/draw_294.inc"
};
static NJS_CNK_OBJECT draw_293 = {
#include "stg21/terrain/draw_293.inc"
};
static NJS_CNK_OBJECT draw_292 = {
#include "stg21/terrain/draw_292.inc"
};
static NJS_CNK_OBJECT draw_291 = {
#include "stg21/terrain/draw_291.inc"
};
static NJS_CNK_OBJECT draw_290 = {
#include "stg21/terrain/draw_290.inc"
};
static NJS_CNK_OBJECT draw_289 = {
#include "stg21/terrain/draw_289.inc"
};
static NJS_CNK_OBJECT draw_288 = {
#include "stg21/terrain/draw_288.inc"
};
static NJS_CNK_OBJECT draw_287 = {
#include "stg21/terrain/draw_287.inc"
};
static NJS_CNK_OBJECT draw_286 = {
#include "stg21/terrain/draw_286.inc"
};
static NJS_CNK_OBJECT draw_285 = {
#include "stg21/terrain/draw_285.inc"
};
static NJS_CNK_OBJECT draw_284 = {
#include "stg21/terrain/draw_284.inc"
};
static NJS_CNK_OBJECT draw_283 = {
#include "stg21/terrain/draw_283.inc"
};
static NJS_CNK_OBJECT draw_282 = {
#include "stg21/terrain/draw_282.inc"
};
static NJS_CNK_OBJECT draw_281 = {
#include "stg21/terrain/draw_281.inc"
};
static NJS_CNK_OBJECT draw_280 = {
#include "stg21/terrain/draw_280.inc"
};
static NJS_CNK_OBJECT draw_279 = {
#include "stg21/terrain/draw_279.inc"
};
static NJS_CNK_OBJECT draw_278 = {
#include "stg21/terrain/draw_278.inc"
};
static NJS_CNK_OBJECT draw_277 = {
#include "stg21/terrain/draw_277.inc"
};
static NJS_CNK_OBJECT draw_276 = {
#include "stg21/terrain/draw_276.inc"
};
static NJS_CNK_OBJECT draw_275 = {
#include "stg21/terrain/draw_275.inc"
};
static NJS_CNK_OBJECT draw_274 = {
#include "stg21/terrain/draw_274.inc"
};
static NJS_CNK_OBJECT draw_268 = {
#include "stg21/terrain/draw_268.inc"
};
static NJS_CNK_OBJECT draw_267 = {
#include "stg21/terrain/draw_267.inc"
};
static NJS_CNK_OBJECT draw_266 = {
#include "stg21/terrain/draw_266.inc"
};
static NJS_CNK_OBJECT draw_265 = {
#include "stg21/terrain/draw_265.inc"
};
static NJS_CNK_OBJECT draw_264 = {
#include "stg21/terrain/draw_264.inc"
};
static NJS_CNK_OBJECT draw_259 = {
#include "stg21/terrain/draw_259.inc"
};
static NJS_CNK_OBJECT draw_258 = {
#include "stg21/terrain/draw_258.inc"
};
static NJS_CNK_OBJECT draw_250 = {
#include "stg21/terrain/draw_250.inc"
};
static NJS_CNK_OBJECT draw_249 = {
#include "stg21/terrain/draw_249.inc"
};
static NJS_CNK_OBJECT draw_248 = {
#include "stg21/terrain/draw_248.inc"
};
static NJS_CNK_OBJECT draw_247 = {
#include "stg21/terrain/draw_247.inc"
};
static NJS_CNK_OBJECT draw_246 = {
#include "stg21/terrain/draw_246.inc"
};
static NJS_CNK_OBJECT draw_244 = {
#include "stg21/terrain/draw_244.inc"
};
static NJS_CNK_OBJECT draw_243 = {
#include "stg21/terrain/draw_243.inc"
};
static NJS_CNK_OBJECT draw_242 = {
#include "stg21/terrain/draw_242.inc"
};
static NJS_CNK_OBJECT draw_241 = {
#include "stg21/terrain/draw_241.inc"
};
static NJS_CNK_OBJECT draw_240 = {
#include "stg21/terrain/draw_240.inc"
};
static NJS_CNK_OBJECT draw_238 = {
#include "stg21/terrain/draw_238.inc"
};
static NJS_CNK_OBJECT draw_237 = {
#include "stg21/terrain/draw_237.inc"
};
static NJS_CNK_OBJECT draw_236 = {
#include "stg21/terrain/draw_236.inc"
};
static NJS_CNK_OBJECT draw_235 = {
#include "stg21/terrain/draw_235.inc"
};
static NJS_CNK_OBJECT draw_234 = {
#include "stg21/terrain/draw_234.inc"
};
static NJS_CNK_OBJECT draw_230 = {
#include "stg21/terrain/draw_230.inc"
};
static NJS_CNK_OBJECT draw_229 = {
#include "stg21/terrain/draw_229.inc"
};
static NJS_CNK_OBJECT draw_228 = {
#include "stg21/terrain/draw_228.inc"
};
static NJS_CNK_OBJECT draw_227 = {
#include "stg21/terrain/draw_227.inc"
};
static NJS_CNK_OBJECT draw_226 = {
#include "stg21/terrain/draw_226.inc"
};
static NJS_CNK_OBJECT draw_225 = {
#include "stg21/terrain/draw_225.inc"
};
static NJS_CNK_OBJECT draw_216 = {
#include "stg21/terrain/draw_216.inc"
};
static NJS_CNK_OBJECT draw_215 = {
#include "stg21/terrain/draw_215.inc"
};
static NJS_CNK_OBJECT draw_214 = {
#include "stg21/terrain/draw_214.inc"
};
static NJS_CNK_OBJECT draw_213 = {
#include "stg21/terrain/draw_213.inc"
};
static NJS_CNK_OBJECT draw_212 = {
#include "stg21/terrain/draw_212.inc"
};
static NJS_CNK_OBJECT draw_208 = {
#include "stg21/terrain/draw_208.inc"
};
static NJS_CNK_OBJECT draw_207 = {
#include "stg21/terrain/draw_207.inc"
};
static NJS_CNK_OBJECT draw_206 = {
#include "stg21/terrain/draw_206.inc"
};
static NJS_CNK_OBJECT draw_205 = {
#include "stg21/terrain/draw_205.inc"
};
static NJS_CNK_OBJECT draw_204 = {
#include "stg21/terrain/draw_204.inc"
};
static NJS_CNK_OBJECT draw_195 = {
#include "stg21/terrain/draw_195.inc"
};
static NJS_CNK_OBJECT draw_194 = {
#include "stg21/terrain/draw_194.inc"
};
static NJS_CNK_OBJECT draw_193 = {
#include "stg21/terrain/draw_193.inc"
};
static NJS_CNK_OBJECT draw_192 = {
#include "stg21/terrain/draw_192.inc"
};
static NJS_CNK_OBJECT draw_191 = {
#include "stg21/terrain/draw_191.inc"
};
static NJS_CNK_OBJECT draw_190 = {
#include "stg21/terrain/draw_190.inc"
};
static NJS_CNK_OBJECT draw_189 = {
#include "stg21/terrain/draw_189.inc"
};
static NJS_CNK_OBJECT draw_188 = {
#include "stg21/terrain/draw_188.inc"
};
static NJS_CNK_OBJECT draw_187 = {
#include "stg21/terrain/draw_187.inc"
};
static NJS_CNK_OBJECT draw_186 = {
#include "stg21/terrain/draw_186.inc"
};
static NJS_CNK_OBJECT draw_185 = {
#include "stg21/terrain/draw_185.inc"
};
static NJS_CNK_OBJECT draw_184 = {
#include "stg21/terrain/draw_184.inc"
};
static NJS_CNK_OBJECT draw_183 = {
#include "stg21/terrain/draw_183.inc"
};
static NJS_CNK_OBJECT draw_182 = {
#include "stg21/terrain/draw_182.inc"
};
static NJS_CNK_OBJECT draw_181 = {
#include "stg21/terrain/draw_181.inc"
};
static NJS_CNK_OBJECT draw_180 = {
#include "stg21/terrain/draw_180.inc"
};
static NJS_CNK_OBJECT draw_179 = {
#include "stg21/terrain/draw_179.inc"
};
static NJS_CNK_OBJECT draw_178 = {
#include "stg21/terrain/draw_178.inc"
};
static NJS_CNK_OBJECT draw_177 = {
#include "stg21/terrain/draw_177.inc"
};
static NJS_CNK_OBJECT draw_176 = {
#include "stg21/terrain/draw_176.inc"
};
static NJS_CNK_OBJECT draw_175 = {
#include "stg21/terrain/draw_175.inc"
};
static NJS_CNK_OBJECT draw_174 = {
#include "stg21/terrain/draw_174.inc"
};
static NJS_CNK_OBJECT draw_173 = {
#include "stg21/terrain/draw_173.inc"
};
static NJS_CNK_OBJECT draw_172 = {
#include "stg21/terrain/draw_172.inc"
};
static NJS_CNK_OBJECT draw_171 = {
#include "stg21/terrain/draw_171.inc"
};
static NJS_CNK_OBJECT draw_170 = {
#include "stg21/terrain/draw_170.inc"
};
static NJS_CNK_OBJECT draw_169 = {
#include "stg21/terrain/draw_169.inc"
};
static NJS_CNK_OBJECT draw_168 = {
#include "stg21/terrain/draw_168.inc"
};
static NJS_CNK_OBJECT draw_167 = {
#include "stg21/terrain/draw_167.inc"
};
static NJS_CNK_OBJECT draw_166 = {
#include "stg21/terrain/draw_166.inc"
};
static NJS_CNK_OBJECT draw_165 = {
#include "stg21/terrain/draw_165.inc"
};
static NJS_CNK_OBJECT draw_164 = {
#include "stg21/terrain/draw_164.inc"
};
static NJS_CNK_OBJECT draw_163 = {
#include "stg21/terrain/draw_163.inc"
};
static NJS_CNK_OBJECT draw_162 = {
#include "stg21/terrain/draw_162.inc"
};
static NJS_CNK_OBJECT draw_161 = {
#include "stg21/terrain/draw_161.inc"
};
static NJS_CNK_OBJECT draw_160 = {
#include "stg21/terrain/draw_160.inc"
};
static NJS_CNK_OBJECT draw_159 = {
#include "stg21/terrain/draw_159.inc"
};
static NJS_CNK_OBJECT draw_158 = {
#include "stg21/terrain/draw_158.inc"
};
static NJS_CNK_OBJECT draw_157 = {
#include "stg21/terrain/draw_157.inc"
};
static NJS_CNK_OBJECT draw_156 = {
#include "stg21/terrain/draw_156.inc"
};
static NJS_CNK_OBJECT draw_155 = {
#include "stg21/terrain/draw_155.inc"
};
static NJS_CNK_OBJECT draw_154 = {
#include "stg21/terrain/draw_154.inc"
};
static NJS_CNK_OBJECT draw_153 = {
#include "stg21/terrain/draw_153.inc"
};
static NJS_CNK_OBJECT draw_152 = {
#include "stg21/terrain/draw_152.inc"
};
static NJS_CNK_OBJECT draw_151 = {
#include "stg21/terrain/draw_151.inc"
};
static NJS_CNK_OBJECT draw_150 = {
#include "stg21/terrain/draw_150.inc"
};
static NJS_CNK_OBJECT draw_149 = {
#include "stg21/terrain/draw_149.inc"
};
static NJS_CNK_OBJECT draw_148 = {
#include "stg21/terrain/draw_148.inc"
};
static NJS_CNK_OBJECT draw_146 = {
#include "stg21/terrain/draw_146.inc"
};
static NJS_CNK_OBJECT draw_145 = {
#include "stg21/terrain/draw_145.inc"
};
static NJS_CNK_OBJECT draw_143 = {
#include "stg21/terrain/draw_143.inc"
};
static NJS_CNK_OBJECT draw_142 = {
#include "stg21/terrain/draw_142.inc"
};
static NJS_CNK_OBJECT draw_140 = {
#include "stg21/terrain/draw_140.inc"
};
static NJS_CNK_OBJECT draw_139 = {
#include "stg21/terrain/draw_139.inc"
};
static NJS_CNK_OBJECT draw_137 = {
#include "stg21/terrain/draw_137.inc"
};
static NJS_CNK_OBJECT draw_136 = {
#include "stg21/terrain/draw_136.inc"
};
static NJS_CNK_OBJECT draw_134 = {
#include "stg21/terrain/draw_134.inc"
};
static NJS_CNK_OBJECT draw_133 = {
#include "stg21/terrain/draw_133.inc"
};
static NJS_CNK_OBJECT draw_120 = {
#include "stg21/terrain/draw_120.inc"
};
static NJS_CNK_OBJECT draw_116 = {
#include "stg21/terrain/draw_116.inc"
};
static NJS_CNK_OBJECT draw_115 = {
#include "stg21/terrain/draw_115.inc"
};
static NJS_CNK_OBJECT draw_114 = {
#include "stg21/terrain/draw_114.inc"
};
static NJS_CNK_OBJECT draw_113 = {
#include "stg21/terrain/draw_113.inc"
};
static NJS_CNK_OBJECT draw_112 = {
#include "stg21/terrain/draw_112.inc"
};
static NJS_CNK_OBJECT draw_111 = {
#include "stg21/terrain/draw_111.inc"
};
static NJS_CNK_OBJECT draw_110 = {
#include "stg21/terrain/draw_110.inc"
};
static NJS_CNK_OBJECT draw_97 = {
#include "stg21/terrain/draw_97.inc"
};
static NJS_CNK_OBJECT draw_89 = {
#include "stg21/terrain/draw_89.inc"
};
static NJS_CNK_OBJECT draw_88 = {
#include "stg21/terrain/draw_88.inc"
};
static NJS_CNK_OBJECT draw_87 = {
#include "stg21/terrain/draw_87.inc"
};
static NJS_CNK_OBJECT draw_86 = {
#include "stg21/terrain/draw_86.inc"
};
static NJS_CNK_OBJECT draw_85 = {
#include "stg21/terrain/draw_85.inc"
};
static NJS_CNK_OBJECT draw_84 = {
#include "stg21/terrain/draw_84.inc"
};
static NJS_CNK_OBJECT draw_83 = {
#include "stg21/terrain/draw_83.inc"
};
static NJS_CNK_OBJECT draw_82 = {
#include "stg21/terrain/draw_82.inc"
};
static NJS_CNK_OBJECT draw_81 = {
#include "stg21/terrain/draw_81.inc"
};
static NJS_CNK_OBJECT draw_80 = {
#include "stg21/terrain/draw_80.inc"
};
static NJS_CNK_OBJECT draw_79 = {
#include "stg21/terrain/draw_79.inc"
};
static NJS_CNK_OBJECT draw_78 = {
#include "stg21/terrain/draw_78.inc"
};
static NJS_CNK_OBJECT draw_77 = {
#include "stg21/terrain/draw_77.inc"
};
static NJS_CNK_OBJECT draw_76 = {
#include "stg21/terrain/draw_76.inc"
};
static NJS_CNK_OBJECT draw_74 = {
#include "stg21/terrain/draw_74.inc"
};
static NJS_CNK_OBJECT draw_73 = {
#include "stg21/terrain/draw_73.inc"
};
static NJS_CNK_OBJECT draw_72 = {
#include "stg21/terrain/draw_72.inc"
};
static NJS_CNK_OBJECT draw_71 = {
#include "stg21/terrain/draw_71.inc"
};
static NJS_CNK_OBJECT draw_70 = {
#include "stg21/terrain/draw_70.inc"
};
static NJS_CNK_OBJECT draw_69 = {
#include "stg21/terrain/draw_69.inc"
};
static NJS_CNK_OBJECT draw_68 = {
#include "stg21/terrain/draw_68.inc"
};
static NJS_CNK_OBJECT draw_67 = {
#include "stg21/terrain/draw_67.inc"
};
static NJS_CNK_OBJECT draw_66 = {
#include "stg21/terrain/draw_66.inc"
};
static NJS_CNK_OBJECT draw_65 = {
#include "stg21/terrain/draw_65.inc"
};
static NJS_CNK_OBJECT draw_64 = {
#include "stg21/terrain/draw_64.inc"
};
static NJS_CNK_OBJECT draw_63 = {
#include "stg21/terrain/draw_63.inc"
};
static NJS_CNK_OBJECT draw_62 = {
#include "stg21/terrain/draw_62.inc"
};
static NJS_CNK_OBJECT draw_61 = {
#include "stg21/terrain/draw_61.inc"
};
static NJS_CNK_OBJECT draw_59 = {
#include "stg21/terrain/draw_59.inc"
};
static NJS_CNK_OBJECT draw_58 = {
#include "stg21/terrain/draw_58.inc"
};
static NJS_CNK_OBJECT draw_57 = {
#include "stg21/terrain/draw_57.inc"
};
static NJS_CNK_OBJECT draw_55 = {
#include "stg21/terrain/draw_55.inc"
};
static NJS_CNK_OBJECT draw_54 = {
#include "stg21/terrain/draw_54.inc"
};
static NJS_CNK_OBJECT draw_53 = {
#include "stg21/terrain/draw_53.inc"
};
static NJS_CNK_OBJECT draw_51 = {
#include "stg21/terrain/draw_51.inc"
};
static NJS_CNK_OBJECT draw_50 = {
#include "stg21/terrain/draw_50.inc"
};
static NJS_CNK_OBJECT draw_49 = {
#include "stg21/terrain/draw_49.inc"
};
static NJS_CNK_OBJECT draw_47 = {
#include "stg21/terrain/draw_47.inc"
};
static NJS_CNK_OBJECT draw_46 = {
#include "stg21/terrain/draw_46.inc"
};
static NJS_CNK_OBJECT draw_44 = {
#include "stg21/terrain/draw_44.inc"
};
static NJS_CNK_OBJECT draw_43 = {
#include "stg21/terrain/draw_43.inc"
};
static NJS_CNK_OBJECT draw_42 = {
#include "stg21/terrain/draw_42.inc"
};
static NJS_CNK_OBJECT draw_41 = {
#include "stg21/terrain/draw_41.inc"
};
static NJS_CNK_OBJECT draw_39 = {
#include "stg21/terrain/draw_39.inc"
};
static NJS_CNK_OBJECT draw_38 = {
#include "stg21/terrain/draw_38.inc"
};
static NJS_CNK_OBJECT draw_37 = {
#include "stg21/terrain/draw_37.inc"
};
static NJS_CNK_OBJECT draw_36 = {
#include "stg21/terrain/draw_36.inc"
};
static NJS_CNK_OBJECT draw_34 = {
#include "stg21/terrain/draw_34.inc"
};
static NJS_CNK_OBJECT draw_28 = {
#include "stg21/terrain/draw_28.inc"
};
static NJS_CNK_OBJECT draw_27 = {
#include "stg21/terrain/draw_27.inc"
};
static NJS_CNK_OBJECT draw_26 = {
#include "stg21/terrain/draw_26.inc"
};
static NJS_CNK_OBJECT draw_25 = {
#include "stg21/terrain/draw_25.inc"
};
static NJS_CNK_OBJECT draw_23 = {
#include "stg21/terrain/draw_23.inc"
};
static NJS_CNK_OBJECT draw_22 = {
#include "stg21/terrain/draw_22.inc"
};
static NJS_CNK_OBJECT draw_21 = {
#include "stg21/terrain/draw_21.inc"
};
static NJS_CNK_OBJECT draw_20 = {
#include "stg21/terrain/draw_20.inc"
};
static NJS_CNK_OBJECT draw_19 = {
#include "stg21/terrain/draw_19.inc"
};
static NJS_CNK_OBJECT draw_18 = {
#include "stg21/terrain/draw_18.inc"
};
static NJS_CNK_OBJECT draw_17 = {
#include "stg21/terrain/draw_17.inc"
};
static NJS_CNK_OBJECT draw_16 = {
#include "stg21/terrain/draw_16.inc"
};
static NJS_CNK_OBJECT draw_14 = {
#include "stg21/terrain/draw_14.inc"
};
static NJS_CNK_OBJECT draw_12 = {
#include "stg21/terrain/draw_12.inc"
};
static NJS_CNK_OBJECT draw_10 = {
#include "stg21/terrain/draw_10.inc"
};
static NJS_CNK_OBJECT draw_8 = {
#include "stg21/terrain/draw_8.inc"
};
static NJS_CNK_OBJECT draw_6 = {
#include "stg21/terrain/draw_6.inc"
};
static NJS_CNK_OBJECT draw_4 = {
#include "stg21/terrain/draw_4.inc"
};
static NJS_CNK_OBJECT draw_1 = {
#include "stg21/terrain/draw_1.inc"
};
static NJS_CNK_OBJECT draw_0 = {
#include "stg21/terrain/draw_0.inc"
};
static NJS_OBJECT collision_0 = {
#include "stg21/terrain/collision_0.inc"
};
static NJS_OBJECT collision_1 = {
#include "stg21/terrain/collision_1.inc"
};
static NJS_OBJECT collision_2 = {
#include "stg21/terrain/collision_2.inc"
};
static NJS_OBJECT collision_3 = {
#include "stg21/terrain/collision_3.inc"
};
static NJS_OBJECT collision_4 = {
#include "stg21/terrain/collision_4.inc"
};
static NJS_OBJECT collision_6 = {
#include "stg21/terrain/collision_6.inc"
};
static NJS_OBJECT collision_7 = {
#include "stg21/terrain/collision_7.inc"
};
static NJS_OBJECT collision_8 = {
#include "stg21/terrain/collision_8.inc"
};
static NJS_OBJECT collision_9 = {
#include "stg21/terrain/collision_9.inc"
};
static NJS_OBJECT collision_10 = {
#include "stg21/terrain/collision_10.inc"
};
static NJS_OBJECT collision_11 = {
#include "stg21/terrain/collision_11.inc"
};
static NJS_OBJECT collision_12 = {
#include "stg21/terrain/collision_12.inc"
};
static NJS_OBJECT collision_13 = {
#include "stg21/terrain/collision_13.inc"
};
static NJS_OBJECT collision_14 = {
#include "stg21/terrain/collision_14.inc"
};
static NJS_OBJECT collision_15 = {
#include "stg21/terrain/collision_15.inc"
};
static NJS_OBJECT collision_16 = {
#include "stg21/terrain/collision_16.inc"
};
static NJS_OBJECT collision_17 = {
#include "stg21/terrain/collision_17.inc"
};
static NJS_OBJECT collision_18 = {
#include "stg21/terrain/collision_18.inc"
};
static NJS_OBJECT collision_19 = {
#include "stg21/terrain/collision_19.inc"
};
static NJS_OBJECT collision_21 = {
#include "stg21/terrain/collision_21.inc"
};
static NJS_OBJECT collision_23 = {
#include "stg21/terrain/collision_23.inc"
};
static NJS_OBJECT collision_30 = {
#include "stg21/terrain/collision_30.inc"
};
static NJS_OBJECT collision_31 = {
#include "stg21/terrain/collision_31.inc"
};
static NJS_OBJECT collision_32 = {
#include "stg21/terrain/collision_32.inc"
};
static NJS_OBJECT collision_34 = {
#include "stg21/terrain/collision_34.inc"
};
static NJS_OBJECT collision_36 = {
#include "stg21/terrain/collision_36.inc"
};
static NJS_OBJECT collision_45 = {
#include "stg21/terrain/collision_45.inc"
};
static NJS_OBJECT collision_47 = {
#include "stg21/terrain/collision_47.inc"
};
static NJS_OBJECT collision_48 = {
#include "stg21/terrain/collision_48.inc"
};
static NJS_OBJECT collision_49 = {
#include "stg21/terrain/collision_49.inc"
};
static NJS_OBJECT collision_51 = {
#include "stg21/terrain/collision_51.inc"
};
static NJS_OBJECT collision_52 = {
#include "stg21/terrain/collision_52.inc"
};
static NJS_OBJECT collision_54 = {
#include "stg21/terrain/collision_54.inc"
};
static NJS_OBJECT collision_55 = {
#include "stg21/terrain/collision_55.inc"
};
static NJS_OBJECT collision_56 = {
#include "stg21/terrain/collision_56.inc"
};
static NJS_OBJECT collision_57 = {
#include "stg21/terrain/collision_57.inc"
};
static NJS_OBJECT collision_58 = {
#include "stg21/terrain/collision_58.inc"
};
static NJS_OBJECT collision_59 = {
#include "stg21/terrain/collision_59.inc"
};
static NJS_OBJECT collision_60 = {
#include "stg21/terrain/collision_60.inc"
};
static NJS_OBJECT collision_61 = {
#include "stg21/terrain/collision_61.inc"
};
static NJS_OBJECT collision_65 = {
#include "stg21/terrain/collision_65.inc"
};
static NJS_OBJECT collision_68 = {
#include "stg21/terrain/collision_68.inc"
};
static NJS_OBJECT collision_70 = {
#include "stg21/terrain/collision_70.inc"
};
static NJS_OBJECT collision_72 = {
#include "stg21/terrain/collision_72.inc"
};
static NJS_OBJECT collision_74 = {
#include "stg21/terrain/collision_74.inc"
};
static NJS_OBJECT collision_80 = {
#include "stg21/terrain/collision_80.inc"
};
static NJS_OBJECT collision_81 = {
#include "stg21/terrain/collision_81.inc"
};
static NJS_OBJECT collision_82 = {
#include "stg21/terrain/collision_82.inc"
};
static NJS_OBJECT collision_83 = {
#include "stg21/terrain/collision_83.inc"
};
static NJS_OBJECT collision_84 = {
#include "stg21/terrain/collision_84.inc"
};
static NJS_OBJECT collision_86 = {
#include "stg21/terrain/collision_86.inc"
};
