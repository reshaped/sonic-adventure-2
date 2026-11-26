#include <ninja.h>
#include <sg_xpt.h>

#include "collision/world.h"
#include "stg43/terrain.h"

/* the land's file names, which its land table and texture list point at. each
 * starts on a 4-byte boundary */
const char cosmic_wall_land_filenames[479] = {
#include "stg43/terrain/cosmic_wall_land_filenames.inc"
};

/* STG43.PRS's land, texture lists, chunk models and Basic models, from the
 * disc. its D lies between sky_land.c and object_archives.c */

/* the data up to the last that a pointer names ahead of its definition, as
 * tentative definitions in the disc's order: GCC places each datum at its
 * first definition */
static struct land_entry entries[506];
struct land_table cosmic_wall_land;
static NJS_TEXNAME textures[40];
NJS_TEXLIST cosmic_wall_land_texlist;
static Sint16 draw_233_polygons[395];
static Sint32 draw_233_vertices[223];
static NJS_CNK_MODEL draw_233_model;
static NJS_CNK_OBJECT draw_347;
static Sint16 draw_3_polygons[561];
static Sint32 draw_3_vertices[451];
static NJS_CNK_MODEL draw_3_model;
static NJS_CNK_OBJECT draw_332;
static Sint16 draw_212_polygons[41];
static Sint32 draw_212_vertices[35];
static NJS_CNK_MODEL draw_212_model;
static NJS_CNK_OBJECT draw_331;
static Sint16 draw_65_polygons[271];
static Sint32 draw_65_vertices[279];
static NJS_CNK_MODEL draw_65_model;
static NJS_CNK_OBJECT draw_330;
static Sint16 draw_239_polygons[135];
static Sint32 draw_239_vertices[75];
static NJS_CNK_MODEL draw_239_model;
static NJS_CNK_OBJECT draw_329;
static Sint16 draw_246_polygons[341];
static Sint32 draw_246_vertices[287];
static NJS_CNK_MODEL draw_246_model;
static NJS_CNK_OBJECT draw_328;
static Sint16 draw_245_polygons[649];
static Sint32 draw_245_vertices[603];
static NJS_CNK_MODEL draw_245_model;
static NJS_CNK_OBJECT draw_327;
static Sint16 draw_247_polygons[29];
static Sint32 draw_247_vertices[27];
static NJS_CNK_MODEL draw_247_model;
static NJS_CNK_OBJECT draw_326;
static Sint16 draw_309_polygons[169];
static Sint32 draw_309_vertices[155];
static NJS_CNK_MODEL draw_309_model;
static NJS_CNK_OBJECT draw_325;
static Sint16 draw_61_polygons[271];
static Sint32 draw_61_vertices[303];
static NJS_CNK_MODEL draw_61_model;
static NJS_CNK_OBJECT draw_324;
static Sint16 draw_222_polygons[391];
static Sint32 draw_222_vertices[279];
static NJS_CNK_MODEL draw_222_model;
static NJS_CNK_OBJECT draw_310;
static Sint16 draw_218_polygons[101];
static Sint32 draw_218_vertices[51];
static NJS_CNK_MODEL draw_218_model;
static NJS_CNK_OBJECT draw_308;
static Sint16 draw_221_polygons[399];
static Sint32 draw_221_vertices[295];
static NJS_CNK_MODEL draw_221_model;
static NJS_CNK_OBJECT draw_307;
static Sint16 draw_217_polygons[413];
static Sint32 draw_217_vertices[295];
static NJS_CNK_MODEL draw_217_model;
static NJS_CNK_OBJECT draw_304;
static Sint16 draw_216_polygons[387];
static Sint32 draw_216_vertices[271];
static NJS_CNK_MODEL draw_216_model;
static NJS_CNK_OBJECT draw_303;
static Sint16 draw_215_polygons[859];
static Sint32 draw_215_vertices[579];
static NJS_CNK_MODEL draw_215_model;
static NJS_CNK_OBJECT draw_302;
static Sint16 draw_214_polygons[167];
static Sint32 draw_214_vertices[139];
static NJS_CNK_MODEL draw_214_model;
static NJS_CNK_OBJECT draw_300;
static Sint16 draw_296_polygons[231];
static Sint32 draw_296_vertices[267];
static NJS_CNK_MODEL draw_296_model;
static NJS_CNK_OBJECT draw_296;
static Sint16 draw_295_polygons[137];
static Sint32 draw_295_vertices[171];
static NJS_CNK_MODEL draw_295_model;
static NJS_CNK_OBJECT draw_295;
static Sint16 draw_294_polygons[71];
static Sint32 draw_294_vertices[83];
static NJS_CNK_MODEL draw_294_model;
static NJS_CNK_OBJECT draw_294;
static Sint16 draw_284_polygons[161];
static Sint32 draw_284_vertices[203];
static NJS_CNK_MODEL draw_284_model;
static NJS_CNK_OBJECT draw_284;
static Sint16 draw_283_polygons[65];
static Sint32 draw_283_vertices[75];
static NJS_CNK_MODEL draw_283_model;
static NJS_CNK_OBJECT draw_283;
static Sint16 draw_76_polygons[59];
static Sint32 draw_76_vertices[43];
static NJS_CNK_MODEL draw_76_model;
static NJS_CNK_OBJECT draw_209;
static Sint16 draw_74_polygons[307];
static Sint32 draw_74_vertices[199];
static NJS_CNK_MODEL draw_74_model;
static NJS_CNK_OBJECT draw_208;
static Sint16 draw_73_polygons[55];
static Sint32 draw_73_vertices[35];
static NJS_CNK_MODEL draw_73_model;
static NJS_CNK_OBJECT draw_207;
static Sint16 draw_66_polygons[301];
static Sint32 draw_66_vertices[195];
static NJS_CNK_MODEL draw_66_model;
static NJS_CNK_OBJECT draw_206;
static Sint16 draw_154_polygons[451];
static Sint32 draw_154_vertices[327];
static NJS_CNK_MODEL draw_154_model;
static NJS_CNK_OBJECT draw_205;
static Sint16 draw_129_polygons[511];
static Sint32 draw_129_vertices[423];
static NJS_CNK_MODEL draw_129_model;
static NJS_CNK_OBJECT draw_204;
static Sint16 draw_83_polygons[519];
static Sint32 draw_83_vertices[431];
static NJS_CNK_MODEL draw_83_model;
static NJS_CNK_OBJECT draw_203;
static Sint16 draw_82_polygons[519];
static Sint32 draw_82_vertices[419];
static NJS_CNK_MODEL draw_82_model;
static NJS_CNK_OBJECT draw_201;
static Sint16 draw_185_polygons[77];
static Sint32 draw_185_vertices[63];
static NJS_CNK_MODEL draw_185_model;
static NJS_CNK_OBJECT draw_185;
static Sint16 draw_79_polygons[59];
static Sint32 draw_79_vertices[43];
static NJS_CNK_MODEL draw_79_model;
static NJS_CNK_OBJECT draw_153;
static Sint16 draw_123_polygons[77];
static Sint32 draw_123_vertices[91];
static NJS_CNK_MODEL draw_123_model;
static NJS_CNK_OBJECT draw_123;
static Sint16 draw_122_polygons[95];
static Sint32 draw_122_vertices[115];
static NJS_CNK_MODEL draw_122_model;
static NJS_CNK_OBJECT draw_122;
static Sint16 draw_121_polygons[143];
static Sint32 draw_121_vertices[179];
static NJS_CNK_MODEL draw_121_model;
static NJS_CNK_OBJECT draw_121;
static Sint16 draw_120_polygons[137];
static Sint32 draw_120_vertices[171];
static NJS_CNK_MODEL draw_120_model;
static NJS_CNK_OBJECT draw_120;
static Sint16 draw_86_polygons[113];
static Sint32 draw_86_vertices[139];
static NJS_CNK_MODEL draw_86_model;
static NJS_CNK_OBJECT draw_86;
static Sint16 draw_69_polygons[21];
static Sint32 draw_69_vertices[51];
static NJS_CNK_MODEL draw_69_model;
static NJS_CNK_OBJECT draw_69;
static Sint16 draw_68_polygons[115];
static Sint32 draw_68_vertices[131];
static NJS_CNK_MODEL draw_68_model;
static NJS_CNK_OBJECT draw_68;
static Sint16 draw_64_polygons[55];
static Sint32 draw_64_vertices[43];
static NJS_CNK_MODEL draw_64_model;
static NJS_CNK_OBJECT draw_64;
static Sint16 draw_63_polygons[93];
static Sint32 draw_63_vertices[79];
static NJS_CNK_MODEL draw_63_model;
static NJS_CNK_OBJECT draw_63;
static Sint16 draw_60_polygons[323];
static Sint32 draw_60_vertices[259];
static NJS_CNK_MODEL draw_60_model;
static NJS_CNK_OBJECT draw_60;
static Sint16 draw_7_polygons[307];
static Sint32 draw_7_vertices[259];
static NJS_CNK_MODEL draw_7_model;
static NJS_CNK_OBJECT draw_52;
static Sint16 draw_26_polygons[323];
static Sint32 draw_26_vertices[259];
static NJS_CNK_MODEL draw_26_model;
static NJS_CNK_OBJECT draw_26;
static Sint16 draw_4_polygons[429];
static Sint32 draw_4_vertices[319];
static NJS_CNK_MODEL draw_4_model;
static NJS_CNK_OBJECT draw_6;
static Sint16 draw_0_polygons[381];
static Sint32 draw_0_vertices[315];
static NJS_CNK_MODEL draw_0_model;
static NJS_CNK_OBJECT draw_2;
static NJS_MATERIAL collision_0_materials[2];
static Sint16 collision_0_faces[115];
static NJS_MESHSET collision_0_meshsets[1];
static NJS_POINT3 collision_0_points[66];
static NJS_MODEL collision_0_model;
static NJS_OBJECT collision_0;
static NJS_MATERIAL collision_2_materials[1];
static Sint16 collision_2_faces[71];
static NJS_MESHSET collision_2_meshsets[1];
static NJS_POINT3 collision_2_points[30];
static NJS_MODEL collision_2_model;
static NJS_OBJECT collision_2;
static NJS_MATERIAL collision_5_materials[1];
static Sint16 collision_5_faces[91];
static NJS_MESHSET collision_5_meshsets[1];
static NJS_POINT3 collision_5_points[45];
static NJS_MODEL collision_5_model;
static NJS_OBJECT collision_5;
static NJS_MATERIAL collision_8_materials[1];
static Sint16 collision_8_faces[57];
static NJS_MESHSET collision_8_meshsets[1];
static NJS_POINT3 collision_8_points[32];
static NJS_MODEL collision_8_model;
static NJS_OBJECT collision_8;
static NJS_MATERIAL collision_11_materials[1];
static Sint16 collision_11_faces[93];
static NJS_MESHSET collision_11_meshsets[1];
static NJS_POINT3 collision_11_points[56];
static NJS_MODEL collision_11_model;
static NJS_OBJECT collision_11;
static NJS_MATERIAL collision_14_materials[1];
static Sint16 collision_14_faces[86];
static NJS_MESHSET collision_14_meshsets[1];
static NJS_POINT3 collision_14_points[44];
static NJS_MODEL collision_14_model;
static NJS_OBJECT collision_14;
static NJS_MATERIAL collision_17_materials[1];
static Sint16 collision_17_faces[93];
static NJS_MESHSET collision_17_meshsets[1];
static NJS_POINT3 collision_17_points[56];
static NJS_MODEL collision_17_model;
static NJS_OBJECT collision_17;
static NJS_MATERIAL collision_19_materials[1];
static Sint16 collision_19_faces[81];
static NJS_MESHSET collision_19_meshsets[1];
static NJS_POINT3 collision_19_points[48];
static NJS_MODEL collision_19_model;
static NJS_OBJECT collision_19;
static NJS_MATERIAL collision_20_materials[1];
static Sint16 collision_20_faces[124];
static NJS_MESHSET collision_20_meshsets[1];
static NJS_POINT3 collision_20_points[70];
static NJS_MODEL collision_20_model;
static NJS_OBJECT collision_20;
static NJS_MATERIAL collision_23_materials[1];
static Sint16 collision_23_faces[22];
static NJS_MESHSET collision_23_meshsets[1];
static NJS_POINT3 collision_23_points[15];
static NJS_MODEL collision_23_model;
static NJS_OBJECT collision_23;
static NJS_MATERIAL collision_24_materials[1];
static Sint16 collision_24_faces[22];
static NJS_MESHSET collision_24_meshsets[1];
static NJS_POINT3 collision_24_points[15];
static NJS_MODEL collision_24_model;
static NJS_OBJECT collision_24;
static NJS_MATERIAL collision_26_materials[1];
static Sint16 collision_26_faces[83];
static NJS_MESHSET collision_26_meshsets[1];
static NJS_POINT3 collision_26_points[44];
static NJS_MODEL collision_26_model;
static NJS_OBJECT collision_26;
static NJS_MATERIAL collision_27_materials[1];
static Sint16 collision_27_faces[26];
static NJS_MESHSET collision_27_meshsets[1];
static NJS_POINT3 collision_27_points[18];
static NJS_MODEL collision_27_model;
static NJS_OBJECT collision_27;
static NJS_MATERIAL collision_29_materials[1];
static Sint16 collision_29_faces[26];
static NJS_MESHSET collision_29_meshsets[1];
static NJS_POINT3 collision_29_points[18];
static NJS_MODEL collision_29_model;
static NJS_OBJECT collision_29;
static NJS_MATERIAL collision_31_materials[1];
static Sint16 collision_31_faces[46];
static NJS_MESHSET collision_31_meshsets[1];
static NJS_POINT3 collision_31_points[33];
static NJS_MODEL collision_31_model;
static NJS_OBJECT collision_31;
static NJS_MATERIAL collision_32_materials[1];
static Sint16 collision_32_faces[45];
static NJS_MESHSET collision_32_meshsets[1];
static NJS_POINT3 collision_32_points[24];
static NJS_MODEL collision_32_model;
static NJS_OBJECT collision_32;
static NJS_MATERIAL collision_46_materials[1];
static Sint16 collision_46_faces[19];
static NJS_MESHSET collision_46_meshsets[1];
static NJS_POINT3 collision_46_points[12];
static NJS_MODEL collision_46_model;
static NJS_OBJECT collision_46;
static NJS_MATERIAL collision_49_materials[1];
static Sint16 collision_49_faces[35];
static NJS_MESHSET collision_49_meshsets[1];
static NJS_POINT3 collision_49_points[20];
static NJS_MODEL collision_49_model;
static NJS_OBJECT collision_49;
static NJS_MATERIAL collision_52_materials[1];
static Sint16 collision_52_faces[52];
static NJS_MESHSET collision_52_meshsets[1];
static NJS_POINT3 collision_52_points[30];
static NJS_MODEL collision_52_model;
static NJS_OBJECT collision_52;
static NJS_MATERIAL collision_56_materials[1];
static Sint16 collision_56_faces[38];
static NJS_MESHSET collision_56_meshsets[1];
static NJS_POINT3 collision_56_points[22];
static NJS_MODEL collision_56_model;
static NJS_OBJECT collision_56;
static NJS_MATERIAL collision_59_materials[1];
static Sint16 collision_59_faces[24];
static NJS_MESHSET collision_59_meshsets[1];
static NJS_POINT3 collision_59_points[15];
static NJS_MODEL collision_59_model;
static NJS_OBJECT collision_59;
static NJS_MATERIAL collision_65_materials[1];
static Sint16 collision_65_faces[14];
static NJS_MESHSET collision_65_meshsets[1];
static NJS_POINT3 collision_65_points[12];
static NJS_MODEL collision_65_model;
static NJS_OBJECT collision_65;
static NJS_MATERIAL collision_96_materials[1];
static Sint16 collision_96_faces[36];
static NJS_MESHSET collision_96_meshsets[1];
static NJS_POINT3 collision_96_points[24];
static NJS_MODEL collision_96_model;
static NJS_OBJECT collision_96;
static NJS_MATERIAL collision_98_materials[1];
static Sint16 collision_98_faces[14];
static NJS_MESHSET collision_98_meshsets[1];
static NJS_POINT3 collision_98_points[9];
static NJS_MODEL collision_98_model;
static NJS_OBJECT collision_98;
static NJS_MATERIAL collision_99_materials[1];
static Sint16 collision_99_faces[115];
static NJS_MESHSET collision_99_meshsets[1];
static NJS_POINT3 collision_99_points[66];
static NJS_MODEL collision_99_model;
static NJS_OBJECT collision_99;
static NJS_MATERIAL collision_100_materials[1];
static Sint16 collision_100_faces[223];
static NJS_MESHSET collision_100_meshsets[1];
static NJS_POINT3 collision_100_points[117];
static NJS_MODEL collision_100_model;
static NJS_OBJECT collision_100;
static NJS_MATERIAL collision_101_materials[1];
static Sint16 collision_101_faces[70];
static NJS_MESHSET collision_101_meshsets[1];
static NJS_POINT3 collision_101_points[29];
static NJS_MODEL collision_101_model;
static NJS_OBJECT collision_101;
static NJS_MATERIAL collision_102_materials[1];
static Sint16 collision_102_faces[70];
static NJS_MESHSET collision_102_meshsets[1];
static NJS_POINT3 collision_102_points[29];
static NJS_MODEL collision_102_model;
static NJS_OBJECT collision_102;
static NJS_MATERIAL collision_103_materials[1];
static Sint16 collision_103_faces[30];
static NJS_MESHSET collision_103_meshsets[1];
static NJS_POINT3 collision_103_points[20];
static NJS_MODEL collision_103_model;
static NJS_OBJECT collision_103;
static NJS_MATERIAL collision_106_materials[1];
static Sint16 collision_106_faces[14];
static NJS_MESHSET collision_106_meshsets[1];
static NJS_POINT3 collision_106_points[9];
static NJS_MODEL collision_106_model;
static NJS_OBJECT collision_106;
static NJS_MATERIAL collision_107_materials[1];
static Sint16 collision_107_faces[29];
static NJS_MESHSET collision_107_meshsets[1];
static NJS_POINT3 collision_107_points[18];
static NJS_MODEL collision_107_model;
static NJS_OBJECT collision_107;
static NJS_MATERIAL collision_110_materials[1];
static Sint16 collision_110_faces[21];
static NJS_MESHSET collision_110_meshsets[1];
static NJS_POINT3 collision_110_points[8];
static NJS_MODEL collision_110_model;
static NJS_OBJECT collision_110;
static NJS_MATERIAL collision_122_materials[1];
static Sint16 collision_122_faces[16];
static NJS_MESHSET collision_122_meshsets[1];
static NJS_POINT3 collision_122_points[11];
static NJS_MODEL collision_122_model;
static NJS_OBJECT collision_122;
static NJS_MATERIAL collision_124_materials[1];
static Sint16 collision_124_faces[16];
static NJS_MESHSET collision_124_meshsets[1];
static NJS_POINT3 collision_124_points[11];
static NJS_MODEL collision_124_model;
static NJS_OBJECT collision_124;
static NJS_MATERIAL collision_125_materials[1];
static Sint16 collision_125_faces[54];
static NJS_MESHSET collision_125_meshsets[1];
static NJS_POINT3 collision_125_points[24];
static NJS_MODEL collision_125_model;
static NJS_OBJECT collision_125;
static NJS_MATERIAL collision_156_materials[1];
static Sint16 collision_156_faces[14];
static NJS_MESHSET collision_156_meshsets[1];
static NJS_POINT3 collision_156_points[9];
static NJS_MODEL collision_156_model;
static NJS_OBJECT collision_156;
static NJS_MATERIAL collision_157_materials[1];
static Sint16 collision_157_faces[9];
static NJS_MESHSET collision_157_meshsets[1];
static NJS_POINT3 collision_157_points[8];
static NJS_MODEL collision_157_model;
static NJS_OBJECT collision_157;
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
static NJS_CNK_OBJECT draw_309;
static NJS_CNK_OBJECT draw_306;
static NJS_CNK_OBJECT draw_305;
static NJS_CNK_OBJECT draw_301;
static NJS_CNK_OBJECT draw_299;
static NJS_CNK_OBJECT draw_298;
static NJS_CNK_OBJECT draw_297;
static NJS_CNK_OBJECT draw_293;
static NJS_CNK_OBJECT draw_292;
static NJS_CNK_OBJECT draw_291;
static NJS_CNK_OBJECT draw_290;
static NJS_CNK_OBJECT draw_289;
static NJS_CNK_OBJECT draw_288;
static NJS_CNK_OBJECT draw_287;
static NJS_CNK_OBJECT draw_286;
static NJS_CNK_OBJECT draw_285;
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
static NJS_CNK_OBJECT draw_67;
static NJS_CNK_OBJECT draw_66;
static NJS_CNK_OBJECT draw_65;
static NJS_CNK_OBJECT draw_62;
static NJS_CNK_OBJECT draw_61;
static NJS_CNK_OBJECT draw_59;
static NJS_CNK_OBJECT draw_58;
static NJS_CNK_OBJECT draw_57;
static NJS_CNK_OBJECT draw_56;
static NJS_CNK_OBJECT draw_55;
static NJS_CNK_OBJECT draw_54;
static NJS_CNK_OBJECT draw_53;
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
static NJS_CNK_OBJECT draw_5;
static NJS_CNK_OBJECT draw_4;
static NJS_CNK_OBJECT draw_3;
static NJS_CNK_OBJECT draw_1;
static NJS_CNK_OBJECT draw_0;
static NJS_OBJECT collision_1;
static NJS_OBJECT collision_3;
static NJS_OBJECT collision_4;
static NJS_OBJECT collision_6;
static NJS_OBJECT collision_7;
static NJS_OBJECT collision_9;
static NJS_OBJECT collision_10;
static NJS_OBJECT collision_12;
static NJS_OBJECT collision_13;
static NJS_OBJECT collision_15;
static NJS_OBJECT collision_16;
static NJS_OBJECT collision_18;
static NJS_OBJECT collision_21;
static NJS_OBJECT collision_22;
static NJS_OBJECT collision_25;
static NJS_OBJECT collision_28;
static NJS_OBJECT collision_30;
static NJS_OBJECT collision_33;
static NJS_OBJECT collision_34;
static NJS_OBJECT collision_35;
static NJS_OBJECT collision_36;
static NJS_OBJECT collision_37;
static NJS_OBJECT collision_38;
static NJS_OBJECT collision_39;
static NJS_OBJECT collision_40;
static NJS_OBJECT collision_41;
static NJS_OBJECT collision_42;
static NJS_OBJECT collision_43;
static NJS_OBJECT collision_44;
static NJS_OBJECT collision_45;
static NJS_OBJECT collision_47;
static NJS_OBJECT collision_48;
static NJS_OBJECT collision_50;
static NJS_OBJECT collision_51;
static NJS_OBJECT collision_53;
static NJS_OBJECT collision_54;
static NJS_OBJECT collision_55;
static NJS_OBJECT collision_57;
static NJS_OBJECT collision_58;
static NJS_OBJECT collision_60;
static NJS_OBJECT collision_61;
static NJS_OBJECT collision_62;
static NJS_OBJECT collision_63;
static NJS_OBJECT collision_64;
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
static NJS_OBJECT collision_97;
static NJS_OBJECT collision_104;
static NJS_OBJECT collision_105;
static NJS_OBJECT collision_108;
static NJS_OBJECT collision_109;
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
static NJS_OBJECT collision_121;
static NJS_OBJECT collision_123;
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
static NJS_OBJECT collision_148;
static NJS_OBJECT collision_149;
static NJS_OBJECT collision_150;
static NJS_OBJECT collision_151;
static NJS_OBJECT collision_152;
static NJS_OBJECT collision_153;
static NJS_OBJECT collision_154;
static NJS_OBJECT collision_155;

static struct land_entry entries[506] = {
#include "stg43/terrain/entries.inc"
};
struct land_table cosmic_wall_land = {
#include "stg43/terrain/cosmic_wall_land.inc"
};
static NJS_TEXNAME textures[40] = {
#include "stg43/terrain/textures.inc"
};
NJS_TEXLIST cosmic_wall_land_texlist = {
#include "stg43/terrain/cosmic_wall_land_texlist.inc"
};
static Sint16 draw_233_polygons[395] = {
#include "stg43/terrain/draw_233_polygons.inc"
};
static Sint32 draw_233_vertices[223] = {
#include "stg43/terrain/draw_233_vertices.inc"
};
static NJS_CNK_MODEL draw_233_model = {
#include "stg43/terrain/draw_233_model.inc"
};
static NJS_CNK_OBJECT draw_347 = {
#include "stg43/terrain/draw_347.inc"
};
static Sint16 draw_3_polygons[561] = {
#include "stg43/terrain/draw_3_polygons.inc"
};
static Sint32 draw_3_vertices[451] = {
#include "stg43/terrain/draw_3_vertices.inc"
};
static NJS_CNK_MODEL draw_3_model = {
#include "stg43/terrain/draw_3_model.inc"
};
static NJS_CNK_OBJECT draw_332 = {
#include "stg43/terrain/draw_332.inc"
};
static Sint16 draw_212_polygons[41] = {
#include "stg43/terrain/draw_212_polygons.inc"
};
static Sint32 draw_212_vertices[35] = {
#include "stg43/terrain/draw_212_vertices.inc"
};
static NJS_CNK_MODEL draw_212_model = {
#include "stg43/terrain/draw_212_model.inc"
};
static NJS_CNK_OBJECT draw_331 = {
#include "stg43/terrain/draw_331.inc"
};
static Sint16 draw_65_polygons[271] = {
#include "stg43/terrain/draw_65_polygons.inc"
};
static Sint32 draw_65_vertices[279] = {
#include "stg43/terrain/draw_65_vertices.inc"
};
static NJS_CNK_MODEL draw_65_model = {
#include "stg43/terrain/draw_65_model.inc"
};
static NJS_CNK_OBJECT draw_330 = {
#include "stg43/terrain/draw_330.inc"
};
static Sint16 draw_239_polygons[135] = {
#include "stg43/terrain/draw_239_polygons.inc"
};
static Sint32 draw_239_vertices[75] = {
#include "stg43/terrain/draw_239_vertices.inc"
};
static NJS_CNK_MODEL draw_239_model = {
#include "stg43/terrain/draw_239_model.inc"
};
static NJS_CNK_OBJECT draw_329 = {
#include "stg43/terrain/draw_329.inc"
};
static Sint16 draw_246_polygons[341] = {
#include "stg43/terrain/draw_246_polygons.inc"
};
static Sint32 draw_246_vertices[287] = {
#include "stg43/terrain/draw_246_vertices.inc"
};
static NJS_CNK_MODEL draw_246_model = {
#include "stg43/terrain/draw_246_model.inc"
};
static NJS_CNK_OBJECT draw_328 = {
#include "stg43/terrain/draw_328.inc"
};
static Sint16 draw_245_polygons[649] = {
#include "stg43/terrain/draw_245_polygons.inc"
};
static Sint32 draw_245_vertices[603] = {
#include "stg43/terrain/draw_245_vertices.inc"
};
static NJS_CNK_MODEL draw_245_model = {
#include "stg43/terrain/draw_245_model.inc"
};
static NJS_CNK_OBJECT draw_327 = {
#include "stg43/terrain/draw_327.inc"
};
static Sint16 draw_247_polygons[29] = {
#include "stg43/terrain/draw_247_polygons.inc"
};
static Sint32 draw_247_vertices[27] = {
#include "stg43/terrain/draw_247_vertices.inc"
};
static NJS_CNK_MODEL draw_247_model = {
#include "stg43/terrain/draw_247_model.inc"
};
static NJS_CNK_OBJECT draw_326 = {
#include "stg43/terrain/draw_326.inc"
};
static Sint16 draw_309_polygons[169] = {
#include "stg43/terrain/draw_309_polygons.inc"
};
static Sint32 draw_309_vertices[155] = {
#include "stg43/terrain/draw_309_vertices.inc"
};
static NJS_CNK_MODEL draw_309_model = {
#include "stg43/terrain/draw_309_model.inc"
};
static NJS_CNK_OBJECT draw_325 = {
#include "stg43/terrain/draw_325.inc"
};
static Sint16 draw_61_polygons[271] = {
#include "stg43/terrain/draw_61_polygons.inc"
};
static Sint32 draw_61_vertices[303] = {
#include "stg43/terrain/draw_61_vertices.inc"
};
static NJS_CNK_MODEL draw_61_model = {
#include "stg43/terrain/draw_61_model.inc"
};
static NJS_CNK_OBJECT draw_324 = {
#include "stg43/terrain/draw_324.inc"
};
static Sint16 draw_222_polygons[391] = {
#include "stg43/terrain/draw_222_polygons.inc"
};
static Sint32 draw_222_vertices[279] = {
#include "stg43/terrain/draw_222_vertices.inc"
};
static NJS_CNK_MODEL draw_222_model = {
#include "stg43/terrain/draw_222_model.inc"
};
static NJS_CNK_OBJECT draw_310 = {
#include "stg43/terrain/draw_310.inc"
};
static Sint16 draw_218_polygons[101] = {
#include "stg43/terrain/draw_218_polygons.inc"
};
static Sint32 draw_218_vertices[51] = {
#include "stg43/terrain/draw_218_vertices.inc"
};
static NJS_CNK_MODEL draw_218_model = {
#include "stg43/terrain/draw_218_model.inc"
};
static NJS_CNK_OBJECT draw_308 = {
#include "stg43/terrain/draw_308.inc"
};
static Sint16 draw_221_polygons[399] = {
#include "stg43/terrain/draw_221_polygons.inc"
};
static Sint32 draw_221_vertices[295] = {
#include "stg43/terrain/draw_221_vertices.inc"
};
static NJS_CNK_MODEL draw_221_model = {
#include "stg43/terrain/draw_221_model.inc"
};
static NJS_CNK_OBJECT draw_307 = {
#include "stg43/terrain/draw_307.inc"
};
static Sint16 draw_217_polygons[413] = {
#include "stg43/terrain/draw_217_polygons.inc"
};
static Sint32 draw_217_vertices[295] = {
#include "stg43/terrain/draw_217_vertices.inc"
};
static NJS_CNK_MODEL draw_217_model = {
#include "stg43/terrain/draw_217_model.inc"
};
static NJS_CNK_OBJECT draw_304 = {
#include "stg43/terrain/draw_304.inc"
};
static Sint16 draw_216_polygons[387] = {
#include "stg43/terrain/draw_216_polygons.inc"
};
static Sint32 draw_216_vertices[271] = {
#include "stg43/terrain/draw_216_vertices.inc"
};
static NJS_CNK_MODEL draw_216_model = {
#include "stg43/terrain/draw_216_model.inc"
};
static NJS_CNK_OBJECT draw_303 = {
#include "stg43/terrain/draw_303.inc"
};
static Sint16 draw_215_polygons[859] = {
#include "stg43/terrain/draw_215_polygons.inc"
};
static Sint32 draw_215_vertices[579] = {
#include "stg43/terrain/draw_215_vertices.inc"
};
static NJS_CNK_MODEL draw_215_model = {
#include "stg43/terrain/draw_215_model.inc"
};
static NJS_CNK_OBJECT draw_302 = {
#include "stg43/terrain/draw_302.inc"
};
static Sint16 draw_214_polygons[167] = {
#include "stg43/terrain/draw_214_polygons.inc"
};
static Sint32 draw_214_vertices[139] = {
#include "stg43/terrain/draw_214_vertices.inc"
};
static NJS_CNK_MODEL draw_214_model = {
#include "stg43/terrain/draw_214_model.inc"
};
static NJS_CNK_OBJECT draw_300 = {
#include "stg43/terrain/draw_300.inc"
};
static Sint16 draw_296_polygons[231] = {
#include "stg43/terrain/draw_296_polygons.inc"
};
static Sint32 draw_296_vertices[267] = {
#include "stg43/terrain/draw_296_vertices.inc"
};
static NJS_CNK_MODEL draw_296_model = {
#include "stg43/terrain/draw_296_model.inc"
};
static NJS_CNK_OBJECT draw_296 = {
#include "stg43/terrain/draw_296.inc"
};
static Sint16 draw_295_polygons[137] = {
#include "stg43/terrain/draw_295_polygons.inc"
};
static Sint32 draw_295_vertices[171] = {
#include "stg43/terrain/draw_295_vertices.inc"
};
static NJS_CNK_MODEL draw_295_model = {
#include "stg43/terrain/draw_295_model.inc"
};
static NJS_CNK_OBJECT draw_295 = {
#include "stg43/terrain/draw_295.inc"
};
static Sint16 draw_294_polygons[71] = {
#include "stg43/terrain/draw_294_polygons.inc"
};
static Sint32 draw_294_vertices[83] = {
#include "stg43/terrain/draw_294_vertices.inc"
};
static NJS_CNK_MODEL draw_294_model = {
#include "stg43/terrain/draw_294_model.inc"
};
static NJS_CNK_OBJECT draw_294 = {
#include "stg43/terrain/draw_294.inc"
};
static Sint16 draw_284_polygons[161] = {
#include "stg43/terrain/draw_284_polygons.inc"
};
static Sint32 draw_284_vertices[203] = {
#include "stg43/terrain/draw_284_vertices.inc"
};
static NJS_CNK_MODEL draw_284_model = {
#include "stg43/terrain/draw_284_model.inc"
};
static NJS_CNK_OBJECT draw_284 = {
#include "stg43/terrain/draw_284.inc"
};
static Sint16 draw_283_polygons[65] = {
#include "stg43/terrain/draw_283_polygons.inc"
};
static Sint32 draw_283_vertices[75] = {
#include "stg43/terrain/draw_283_vertices.inc"
};
static NJS_CNK_MODEL draw_283_model = {
#include "stg43/terrain/draw_283_model.inc"
};
static NJS_CNK_OBJECT draw_283 = {
#include "stg43/terrain/draw_283.inc"
};
static Sint16 draw_76_polygons[59] = {
#include "stg43/terrain/draw_76_polygons.inc"
};
static Sint32 draw_76_vertices[43] = {
#include "stg43/terrain/draw_76_vertices.inc"
};
static NJS_CNK_MODEL draw_76_model = {
#include "stg43/terrain/draw_76_model.inc"
};
static NJS_CNK_OBJECT draw_209 = {
#include "stg43/terrain/draw_209.inc"
};
static Sint16 draw_74_polygons[307] = {
#include "stg43/terrain/draw_74_polygons.inc"
};
static Sint32 draw_74_vertices[199] = {
#include "stg43/terrain/draw_74_vertices.inc"
};
static NJS_CNK_MODEL draw_74_model = {
#include "stg43/terrain/draw_74_model.inc"
};
static NJS_CNK_OBJECT draw_208 = {
#include "stg43/terrain/draw_208.inc"
};
static Sint16 draw_73_polygons[55] = {
#include "stg43/terrain/draw_73_polygons.inc"
};
static Sint32 draw_73_vertices[35] = {
#include "stg43/terrain/draw_73_vertices.inc"
};
static NJS_CNK_MODEL draw_73_model = {
#include "stg43/terrain/draw_73_model.inc"
};
static NJS_CNK_OBJECT draw_207 = {
#include "stg43/terrain/draw_207.inc"
};
static Sint16 draw_66_polygons[301] = {
#include "stg43/terrain/draw_66_polygons.inc"
};
static Sint32 draw_66_vertices[195] = {
#include "stg43/terrain/draw_66_vertices.inc"
};
static NJS_CNK_MODEL draw_66_model = {
#include "stg43/terrain/draw_66_model.inc"
};
static NJS_CNK_OBJECT draw_206 = {
#include "stg43/terrain/draw_206.inc"
};
static Sint16 draw_154_polygons[451] = {
#include "stg43/terrain/draw_154_polygons.inc"
};
static Sint32 draw_154_vertices[327] = {
#include "stg43/terrain/draw_154_vertices.inc"
};
static NJS_CNK_MODEL draw_154_model = {
#include "stg43/terrain/draw_154_model.inc"
};
static NJS_CNK_OBJECT draw_205 = {
#include "stg43/terrain/draw_205.inc"
};
static Sint16 draw_129_polygons[511] = {
#include "stg43/terrain/draw_129_polygons.inc"
};
static Sint32 draw_129_vertices[423] = {
#include "stg43/terrain/draw_129_vertices.inc"
};
static NJS_CNK_MODEL draw_129_model = {
#include "stg43/terrain/draw_129_model.inc"
};
static NJS_CNK_OBJECT draw_204 = {
#include "stg43/terrain/draw_204.inc"
};
static Sint16 draw_83_polygons[519] = {
#include "stg43/terrain/draw_83_polygons.inc"
};
static Sint32 draw_83_vertices[431] = {
#include "stg43/terrain/draw_83_vertices.inc"
};
static NJS_CNK_MODEL draw_83_model = {
#include "stg43/terrain/draw_83_model.inc"
};
static NJS_CNK_OBJECT draw_203 = {
#include "stg43/terrain/draw_203.inc"
};
static Sint16 draw_82_polygons[519] = {
#include "stg43/terrain/draw_82_polygons.inc"
};
static Sint32 draw_82_vertices[419] = {
#include "stg43/terrain/draw_82_vertices.inc"
};
static NJS_CNK_MODEL draw_82_model = {
#include "stg43/terrain/draw_82_model.inc"
};
static NJS_CNK_OBJECT draw_201 = {
#include "stg43/terrain/draw_201.inc"
};
static Sint16 draw_185_polygons[77] = {
#include "stg43/terrain/draw_185_polygons.inc"
};
static Sint32 draw_185_vertices[63] = {
#include "stg43/terrain/draw_185_vertices.inc"
};
static NJS_CNK_MODEL draw_185_model = {
#include "stg43/terrain/draw_185_model.inc"
};
static NJS_CNK_OBJECT draw_185 = {
#include "stg43/terrain/draw_185.inc"
};
static Sint16 draw_79_polygons[59] = {
#include "stg43/terrain/draw_79_polygons.inc"
};
static Sint32 draw_79_vertices[43] = {
#include "stg43/terrain/draw_79_vertices.inc"
};
static NJS_CNK_MODEL draw_79_model = {
#include "stg43/terrain/draw_79_model.inc"
};
static NJS_CNK_OBJECT draw_153 = {
#include "stg43/terrain/draw_153.inc"
};
static Sint16 draw_123_polygons[77] = {
#include "stg43/terrain/draw_123_polygons.inc"
};
static Sint32 draw_123_vertices[91] = {
#include "stg43/terrain/draw_123_vertices.inc"
};
static NJS_CNK_MODEL draw_123_model = {
#include "stg43/terrain/draw_123_model.inc"
};
static NJS_CNK_OBJECT draw_123 = {
#include "stg43/terrain/draw_123.inc"
};
static Sint16 draw_122_polygons[95] = {
#include "stg43/terrain/draw_122_polygons.inc"
};
static Sint32 draw_122_vertices[115] = {
#include "stg43/terrain/draw_122_vertices.inc"
};
static NJS_CNK_MODEL draw_122_model = {
#include "stg43/terrain/draw_122_model.inc"
};
static NJS_CNK_OBJECT draw_122 = {
#include "stg43/terrain/draw_122.inc"
};
static Sint16 draw_121_polygons[143] = {
#include "stg43/terrain/draw_121_polygons.inc"
};
static Sint32 draw_121_vertices[179] = {
#include "stg43/terrain/draw_121_vertices.inc"
};
static NJS_CNK_MODEL draw_121_model = {
#include "stg43/terrain/draw_121_model.inc"
};
static NJS_CNK_OBJECT draw_121 = {
#include "stg43/terrain/draw_121.inc"
};
static Sint16 draw_120_polygons[137] = {
#include "stg43/terrain/draw_120_polygons.inc"
};
static Sint32 draw_120_vertices[171] = {
#include "stg43/terrain/draw_120_vertices.inc"
};
static NJS_CNK_MODEL draw_120_model = {
#include "stg43/terrain/draw_120_model.inc"
};
static NJS_CNK_OBJECT draw_120 = {
#include "stg43/terrain/draw_120.inc"
};
static Sint16 draw_86_polygons[113] = {
#include "stg43/terrain/draw_86_polygons.inc"
};
static Sint32 draw_86_vertices[139] = {
#include "stg43/terrain/draw_86_vertices.inc"
};
static NJS_CNK_MODEL draw_86_model = {
#include "stg43/terrain/draw_86_model.inc"
};
static NJS_CNK_OBJECT draw_86 = {
#include "stg43/terrain/draw_86.inc"
};
static Sint16 draw_69_polygons[21] = {
#include "stg43/terrain/draw_69_polygons.inc"
};
static Sint32 draw_69_vertices[51] = {
#include "stg43/terrain/draw_69_vertices.inc"
};
static NJS_CNK_MODEL draw_69_model = {
#include "stg43/terrain/draw_69_model.inc"
};
static NJS_CNK_OBJECT draw_69 = {
#include "stg43/terrain/draw_69.inc"
};
static Sint16 draw_68_polygons[115] = {
#include "stg43/terrain/draw_68_polygons.inc"
};
static Sint32 draw_68_vertices[131] = {
#include "stg43/terrain/draw_68_vertices.inc"
};
static NJS_CNK_MODEL draw_68_model = {
#include "stg43/terrain/draw_68_model.inc"
};
static NJS_CNK_OBJECT draw_68 = {
#include "stg43/terrain/draw_68.inc"
};
static Sint16 draw_64_polygons[55] = {
#include "stg43/terrain/draw_64_polygons.inc"
};
static Sint32 draw_64_vertices[43] = {
#include "stg43/terrain/draw_64_vertices.inc"
};
static NJS_CNK_MODEL draw_64_model = {
#include "stg43/terrain/draw_64_model.inc"
};
static NJS_CNK_OBJECT draw_64 = {
#include "stg43/terrain/draw_64.inc"
};
static Sint16 draw_63_polygons[93] = {
#include "stg43/terrain/draw_63_polygons.inc"
};
static Sint32 draw_63_vertices[79] = {
#include "stg43/terrain/draw_63_vertices.inc"
};
static NJS_CNK_MODEL draw_63_model = {
#include "stg43/terrain/draw_63_model.inc"
};
static NJS_CNK_OBJECT draw_63 = {
#include "stg43/terrain/draw_63.inc"
};
static Sint16 draw_60_polygons[323] = {
#include "stg43/terrain/draw_60_polygons.inc"
};
static Sint32 draw_60_vertices[259] = {
#include "stg43/terrain/draw_60_vertices.inc"
};
static NJS_CNK_MODEL draw_60_model = {
#include "stg43/terrain/draw_60_model.inc"
};
static NJS_CNK_OBJECT draw_60 = {
#include "stg43/terrain/draw_60.inc"
};
static Sint16 draw_7_polygons[307] = {
#include "stg43/terrain/draw_7_polygons.inc"
};
static Sint32 draw_7_vertices[259] = {
#include "stg43/terrain/draw_7_vertices.inc"
};
static NJS_CNK_MODEL draw_7_model = {
#include "stg43/terrain/draw_7_model.inc"
};
static NJS_CNK_OBJECT draw_52 = {
#include "stg43/terrain/draw_52.inc"
};
static Sint16 draw_26_polygons[323] = {
#include "stg43/terrain/draw_26_polygons.inc"
};
static Sint32 draw_26_vertices[259] = {
#include "stg43/terrain/draw_26_vertices.inc"
};
static NJS_CNK_MODEL draw_26_model = {
#include "stg43/terrain/draw_26_model.inc"
};
static NJS_CNK_OBJECT draw_26 = {
#include "stg43/terrain/draw_26.inc"
};
static Sint16 draw_4_polygons[429] = {
#include "stg43/terrain/draw_4_polygons.inc"
};
static Sint32 draw_4_vertices[319] = {
#include "stg43/terrain/draw_4_vertices.inc"
};
static NJS_CNK_MODEL draw_4_model = {
#include "stg43/terrain/draw_4_model.inc"
};
static NJS_CNK_OBJECT draw_6 = {
#include "stg43/terrain/draw_6.inc"
};
static Sint16 draw_0_polygons[381] = {
#include "stg43/terrain/draw_0_polygons.inc"
};
static Sint32 draw_0_vertices[315] = {
#include "stg43/terrain/draw_0_vertices.inc"
};
static NJS_CNK_MODEL draw_0_model = {
#include "stg43/terrain/draw_0_model.inc"
};
static NJS_CNK_OBJECT draw_2 = {
#include "stg43/terrain/draw_2.inc"
};
static NJS_MATERIAL collision_0_materials[2] = {
#include "stg43/terrain/collision_0_materials.inc"
};
static Sint16 collision_0_faces[115] = {
#include "stg43/terrain/collision_0_faces.inc"
};
static NJS_MESHSET collision_0_meshsets[1] = {
#include "stg43/terrain/collision_0_meshsets.inc"
};
static NJS_POINT3 collision_0_points[66] = {
#include "stg43/terrain/collision_0_points.inc"
};
static NJS_MODEL collision_0_model = {
#include "stg43/terrain/collision_0_model.inc"
};
static NJS_OBJECT collision_0 = {
#include "stg43/terrain/collision_0.inc"
};
static NJS_MATERIAL collision_2_materials[1] = {
#include "stg43/terrain/collision_2_materials.inc"
};
static Sint16 collision_2_faces[71] = {
#include "stg43/terrain/collision_2_faces.inc"
};
static NJS_MESHSET collision_2_meshsets[1] = {
#include "stg43/terrain/collision_2_meshsets.inc"
};
static NJS_POINT3 collision_2_points[30] = {
#include "stg43/terrain/collision_2_points.inc"
};
static NJS_MODEL collision_2_model = {
#include "stg43/terrain/collision_2_model.inc"
};
static NJS_OBJECT collision_2 = {
#include "stg43/terrain/collision_2.inc"
};
static NJS_MATERIAL collision_5_materials[1] = {
#include "stg43/terrain/collision_5_materials.inc"
};
static Sint16 collision_5_faces[91] = {
#include "stg43/terrain/collision_5_faces.inc"
};
static NJS_MESHSET collision_5_meshsets[1] = {
#include "stg43/terrain/collision_5_meshsets.inc"
};
static NJS_POINT3 collision_5_points[45] = {
#include "stg43/terrain/collision_5_points.inc"
};
static NJS_MODEL collision_5_model = {
#include "stg43/terrain/collision_5_model.inc"
};
static NJS_OBJECT collision_5 = {
#include "stg43/terrain/collision_5.inc"
};
static NJS_MATERIAL collision_8_materials[1] = {
#include "stg43/terrain/collision_8_materials.inc"
};
static Sint16 collision_8_faces[57] = {
#include "stg43/terrain/collision_8_faces.inc"
};
static NJS_MESHSET collision_8_meshsets[1] = {
#include "stg43/terrain/collision_8_meshsets.inc"
};
static NJS_POINT3 collision_8_points[32] = {
#include "stg43/terrain/collision_8_points.inc"
};
static NJS_MODEL collision_8_model = {
#include "stg43/terrain/collision_8_model.inc"
};
static NJS_OBJECT collision_8 = {
#include "stg43/terrain/collision_8.inc"
};
static NJS_MATERIAL collision_11_materials[1] = {
#include "stg43/terrain/collision_11_materials.inc"
};
static Sint16 collision_11_faces[93] = {
#include "stg43/terrain/collision_11_faces.inc"
};
static NJS_MESHSET collision_11_meshsets[1] = {
#include "stg43/terrain/collision_11_meshsets.inc"
};
static NJS_POINT3 collision_11_points[56] = {
#include "stg43/terrain/collision_11_points.inc"
};
static NJS_MODEL collision_11_model = {
#include "stg43/terrain/collision_11_model.inc"
};
static NJS_OBJECT collision_11 = {
#include "stg43/terrain/collision_11.inc"
};
static NJS_MATERIAL collision_14_materials[1] = {
#include "stg43/terrain/collision_14_materials.inc"
};
static Sint16 collision_14_faces[86] = {
#include "stg43/terrain/collision_14_faces.inc"
};
static NJS_MESHSET collision_14_meshsets[1] = {
#include "stg43/terrain/collision_14_meshsets.inc"
};
static NJS_POINT3 collision_14_points[44] = {
#include "stg43/terrain/collision_14_points.inc"
};
static NJS_MODEL collision_14_model = {
#include "stg43/terrain/collision_14_model.inc"
};
static NJS_OBJECT collision_14 = {
#include "stg43/terrain/collision_14.inc"
};
static NJS_MATERIAL collision_17_materials[1] = {
#include "stg43/terrain/collision_17_materials.inc"
};
static Sint16 collision_17_faces[93] = {
#include "stg43/terrain/collision_17_faces.inc"
};
static NJS_MESHSET collision_17_meshsets[1] = {
#include "stg43/terrain/collision_17_meshsets.inc"
};
static NJS_POINT3 collision_17_points[56] = {
#include "stg43/terrain/collision_17_points.inc"
};
static NJS_MODEL collision_17_model = {
#include "stg43/terrain/collision_17_model.inc"
};
static NJS_OBJECT collision_17 = {
#include "stg43/terrain/collision_17.inc"
};
static NJS_MATERIAL collision_19_materials[1] = {
#include "stg43/terrain/collision_19_materials.inc"
};
static Sint16 collision_19_faces[81] = {
#include "stg43/terrain/collision_19_faces.inc"
};
static NJS_MESHSET collision_19_meshsets[1] = {
#include "stg43/terrain/collision_19_meshsets.inc"
};
static NJS_POINT3 collision_19_points[48] = {
#include "stg43/terrain/collision_19_points.inc"
};
static NJS_MODEL collision_19_model = {
#include "stg43/terrain/collision_19_model.inc"
};
static NJS_OBJECT collision_19 = {
#include "stg43/terrain/collision_19.inc"
};
static NJS_MATERIAL collision_20_materials[1] = {
#include "stg43/terrain/collision_20_materials.inc"
};
static Sint16 collision_20_faces[124] = {
#include "stg43/terrain/collision_20_faces.inc"
};
static NJS_MESHSET collision_20_meshsets[1] = {
#include "stg43/terrain/collision_20_meshsets.inc"
};
static NJS_POINT3 collision_20_points[70] = {
#include "stg43/terrain/collision_20_points.inc"
};
static NJS_MODEL collision_20_model = {
#include "stg43/terrain/collision_20_model.inc"
};
static NJS_OBJECT collision_20 = {
#include "stg43/terrain/collision_20.inc"
};
static NJS_MATERIAL collision_23_materials[1] = {
#include "stg43/terrain/collision_23_materials.inc"
};
static Sint16 collision_23_faces[22] = {
#include "stg43/terrain/collision_23_faces.inc"
};
static NJS_MESHSET collision_23_meshsets[1] = {
#include "stg43/terrain/collision_23_meshsets.inc"
};
static NJS_POINT3 collision_23_points[15] = {
#include "stg43/terrain/collision_23_points.inc"
};
static NJS_MODEL collision_23_model = {
#include "stg43/terrain/collision_23_model.inc"
};
static NJS_OBJECT collision_23 = {
#include "stg43/terrain/collision_23.inc"
};
static NJS_MATERIAL collision_24_materials[1] = {
#include "stg43/terrain/collision_24_materials.inc"
};
static Sint16 collision_24_faces[22] = {
#include "stg43/terrain/collision_24_faces.inc"
};
static NJS_MESHSET collision_24_meshsets[1] = {
#include "stg43/terrain/collision_24_meshsets.inc"
};
static NJS_POINT3 collision_24_points[15] = {
#include "stg43/terrain/collision_24_points.inc"
};
static NJS_MODEL collision_24_model = {
#include "stg43/terrain/collision_24_model.inc"
};
static NJS_OBJECT collision_24 = {
#include "stg43/terrain/collision_24.inc"
};
static NJS_MATERIAL collision_26_materials[1] = {
#include "stg43/terrain/collision_26_materials.inc"
};
static Sint16 collision_26_faces[83] = {
#include "stg43/terrain/collision_26_faces.inc"
};
static NJS_MESHSET collision_26_meshsets[1] = {
#include "stg43/terrain/collision_26_meshsets.inc"
};
static NJS_POINT3 collision_26_points[44] = {
#include "stg43/terrain/collision_26_points.inc"
};
static NJS_MODEL collision_26_model = {
#include "stg43/terrain/collision_26_model.inc"
};
static NJS_OBJECT collision_26 = {
#include "stg43/terrain/collision_26.inc"
};
static NJS_MATERIAL collision_27_materials[1] = {
#include "stg43/terrain/collision_27_materials.inc"
};
static Sint16 collision_27_faces[26] = {
#include "stg43/terrain/collision_27_faces.inc"
};
static NJS_MESHSET collision_27_meshsets[1] = {
#include "stg43/terrain/collision_27_meshsets.inc"
};
static NJS_POINT3 collision_27_points[18] = {
#include "stg43/terrain/collision_27_points.inc"
};
static NJS_MODEL collision_27_model = {
#include "stg43/terrain/collision_27_model.inc"
};
static NJS_OBJECT collision_27 = {
#include "stg43/terrain/collision_27.inc"
};
static NJS_MATERIAL collision_29_materials[1] = {
#include "stg43/terrain/collision_29_materials.inc"
};
static Sint16 collision_29_faces[26] = {
#include "stg43/terrain/collision_29_faces.inc"
};
static NJS_MESHSET collision_29_meshsets[1] = {
#include "stg43/terrain/collision_29_meshsets.inc"
};
static NJS_POINT3 collision_29_points[18] = {
#include "stg43/terrain/collision_29_points.inc"
};
static NJS_MODEL collision_29_model = {
#include "stg43/terrain/collision_29_model.inc"
};
static NJS_OBJECT collision_29 = {
#include "stg43/terrain/collision_29.inc"
};
static NJS_MATERIAL collision_31_materials[1] = {
#include "stg43/terrain/collision_31_materials.inc"
};
static Sint16 collision_31_faces[46] = {
#include "stg43/terrain/collision_31_faces.inc"
};
static NJS_MESHSET collision_31_meshsets[1] = {
#include "stg43/terrain/collision_31_meshsets.inc"
};
static NJS_POINT3 collision_31_points[33] = {
#include "stg43/terrain/collision_31_points.inc"
};
static NJS_MODEL collision_31_model = {
#include "stg43/terrain/collision_31_model.inc"
};
static NJS_OBJECT collision_31 = {
#include "stg43/terrain/collision_31.inc"
};
static NJS_MATERIAL collision_32_materials[1] = {
#include "stg43/terrain/collision_32_materials.inc"
};
static Sint16 collision_32_faces[45] = {
#include "stg43/terrain/collision_32_faces.inc"
};
static NJS_MESHSET collision_32_meshsets[1] = {
#include "stg43/terrain/collision_32_meshsets.inc"
};
static NJS_POINT3 collision_32_points[24] = {
#include "stg43/terrain/collision_32_points.inc"
};
static NJS_MODEL collision_32_model = {
#include "stg43/terrain/collision_32_model.inc"
};
static NJS_OBJECT collision_32 = {
#include "stg43/terrain/collision_32.inc"
};
static NJS_MATERIAL collision_46_materials[1] = {
#include "stg43/terrain/collision_46_materials.inc"
};
static Sint16 collision_46_faces[19] = {
#include "stg43/terrain/collision_46_faces.inc"
};
static NJS_MESHSET collision_46_meshsets[1] = {
#include "stg43/terrain/collision_46_meshsets.inc"
};
static NJS_POINT3 collision_46_points[12] = {
#include "stg43/terrain/collision_46_points.inc"
};
static NJS_MODEL collision_46_model = {
#include "stg43/terrain/collision_46_model.inc"
};
static NJS_OBJECT collision_46 = {
#include "stg43/terrain/collision_46.inc"
};
static NJS_MATERIAL collision_49_materials[1] = {
#include "stg43/terrain/collision_49_materials.inc"
};
static Sint16 collision_49_faces[35] = {
#include "stg43/terrain/collision_49_faces.inc"
};
static NJS_MESHSET collision_49_meshsets[1] = {
#include "stg43/terrain/collision_49_meshsets.inc"
};
static NJS_POINT3 collision_49_points[20] = {
#include "stg43/terrain/collision_49_points.inc"
};
static NJS_MODEL collision_49_model = {
#include "stg43/terrain/collision_49_model.inc"
};
static NJS_OBJECT collision_49 = {
#include "stg43/terrain/collision_49.inc"
};
static NJS_MATERIAL collision_52_materials[1] = {
#include "stg43/terrain/collision_52_materials.inc"
};
static Sint16 collision_52_faces[52] = {
#include "stg43/terrain/collision_52_faces.inc"
};
static NJS_MESHSET collision_52_meshsets[1] = {
#include "stg43/terrain/collision_52_meshsets.inc"
};
static NJS_POINT3 collision_52_points[30] = {
#include "stg43/terrain/collision_52_points.inc"
};
static NJS_MODEL collision_52_model = {
#include "stg43/terrain/collision_52_model.inc"
};
static NJS_OBJECT collision_52 = {
#include "stg43/terrain/collision_52.inc"
};
static NJS_MATERIAL collision_56_materials[1] = {
#include "stg43/terrain/collision_56_materials.inc"
};
static Sint16 collision_56_faces[38] = {
#include "stg43/terrain/collision_56_faces.inc"
};
static NJS_MESHSET collision_56_meshsets[1] = {
#include "stg43/terrain/collision_56_meshsets.inc"
};
static NJS_POINT3 collision_56_points[22] = {
#include "stg43/terrain/collision_56_points.inc"
};
static NJS_MODEL collision_56_model = {
#include "stg43/terrain/collision_56_model.inc"
};
static NJS_OBJECT collision_56 = {
#include "stg43/terrain/collision_56.inc"
};
static NJS_MATERIAL collision_59_materials[1] = {
#include "stg43/terrain/collision_59_materials.inc"
};
static Sint16 collision_59_faces[24] = {
#include "stg43/terrain/collision_59_faces.inc"
};
static NJS_MESHSET collision_59_meshsets[1] = {
#include "stg43/terrain/collision_59_meshsets.inc"
};
static NJS_POINT3 collision_59_points[15] = {
#include "stg43/terrain/collision_59_points.inc"
};
static NJS_MODEL collision_59_model = {
#include "stg43/terrain/collision_59_model.inc"
};
static NJS_OBJECT collision_59 = {
#include "stg43/terrain/collision_59.inc"
};
static NJS_MATERIAL collision_65_materials[1] = {
#include "stg43/terrain/collision_65_materials.inc"
};
static Sint16 collision_65_faces[14] = {
#include "stg43/terrain/collision_65_faces.inc"
};
static NJS_MESHSET collision_65_meshsets[1] = {
#include "stg43/terrain/collision_65_meshsets.inc"
};
static NJS_POINT3 collision_65_points[12] = {
#include "stg43/terrain/collision_65_points.inc"
};
static NJS_MODEL collision_65_model = {
#include "stg43/terrain/collision_65_model.inc"
};
static NJS_OBJECT collision_65 = {
#include "stg43/terrain/collision_65.inc"
};
static NJS_MATERIAL collision_96_materials[1] = {
#include "stg43/terrain/collision_96_materials.inc"
};
static Sint16 collision_96_faces[36] = {
#include "stg43/terrain/collision_96_faces.inc"
};
static NJS_MESHSET collision_96_meshsets[1] = {
#include "stg43/terrain/collision_96_meshsets.inc"
};
static NJS_POINT3 collision_96_points[24] = {
#include "stg43/terrain/collision_96_points.inc"
};
static NJS_MODEL collision_96_model = {
#include "stg43/terrain/collision_96_model.inc"
};
static NJS_OBJECT collision_96 = {
#include "stg43/terrain/collision_96.inc"
};
static NJS_MATERIAL collision_98_materials[1] = {
#include "stg43/terrain/collision_98_materials.inc"
};
static Sint16 collision_98_faces[14] = {
#include "stg43/terrain/collision_98_faces.inc"
};
static NJS_MESHSET collision_98_meshsets[1] = {
#include "stg43/terrain/collision_98_meshsets.inc"
};
static NJS_POINT3 collision_98_points[9] = {
#include "stg43/terrain/collision_98_points.inc"
};
static NJS_MODEL collision_98_model = {
#include "stg43/terrain/collision_98_model.inc"
};
static NJS_OBJECT collision_98 = {
#include "stg43/terrain/collision_98.inc"
};
static NJS_MATERIAL collision_99_materials[1] = {
#include "stg43/terrain/collision_99_materials.inc"
};
static Sint16 collision_99_faces[115] = {
#include "stg43/terrain/collision_99_faces.inc"
};
static NJS_MESHSET collision_99_meshsets[1] = {
#include "stg43/terrain/collision_99_meshsets.inc"
};
static NJS_POINT3 collision_99_points[66] = {
#include "stg43/terrain/collision_99_points.inc"
};
static NJS_MODEL collision_99_model = {
#include "stg43/terrain/collision_99_model.inc"
};
static NJS_OBJECT collision_99 = {
#include "stg43/terrain/collision_99.inc"
};
static NJS_MATERIAL collision_100_materials[1] = {
#include "stg43/terrain/collision_100_materials.inc"
};
static Sint16 collision_100_faces[223] = {
#include "stg43/terrain/collision_100_faces.inc"
};
static NJS_MESHSET collision_100_meshsets[1] = {
#include "stg43/terrain/collision_100_meshsets.inc"
};
static NJS_POINT3 collision_100_points[117] = {
#include "stg43/terrain/collision_100_points.inc"
};
static NJS_MODEL collision_100_model = {
#include "stg43/terrain/collision_100_model.inc"
};
static NJS_OBJECT collision_100 = {
#include "stg43/terrain/collision_100.inc"
};
static NJS_MATERIAL collision_101_materials[1] = {
#include "stg43/terrain/collision_101_materials.inc"
};
static Sint16 collision_101_faces[70] = {
#include "stg43/terrain/collision_101_faces.inc"
};
static NJS_MESHSET collision_101_meshsets[1] = {
#include "stg43/terrain/collision_101_meshsets.inc"
};
static NJS_POINT3 collision_101_points[29] = {
#include "stg43/terrain/collision_101_points.inc"
};
static NJS_MODEL collision_101_model = {
#include "stg43/terrain/collision_101_model.inc"
};
static NJS_OBJECT collision_101 = {
#include "stg43/terrain/collision_101.inc"
};
static NJS_MATERIAL collision_102_materials[1] = {
#include "stg43/terrain/collision_102_materials.inc"
};
static Sint16 collision_102_faces[70] = {
#include "stg43/terrain/collision_102_faces.inc"
};
static NJS_MESHSET collision_102_meshsets[1] = {
#include "stg43/terrain/collision_102_meshsets.inc"
};
static NJS_POINT3 collision_102_points[29] = {
#include "stg43/terrain/collision_102_points.inc"
};
static NJS_MODEL collision_102_model = {
#include "stg43/terrain/collision_102_model.inc"
};
static NJS_OBJECT collision_102 = {
#include "stg43/terrain/collision_102.inc"
};
static NJS_MATERIAL collision_103_materials[1] = {
#include "stg43/terrain/collision_103_materials.inc"
};
static Sint16 collision_103_faces[30] = {
#include "stg43/terrain/collision_103_faces.inc"
};
static NJS_MESHSET collision_103_meshsets[1] = {
#include "stg43/terrain/collision_103_meshsets.inc"
};
static NJS_POINT3 collision_103_points[20] = {
#include "stg43/terrain/collision_103_points.inc"
};
static NJS_MODEL collision_103_model = {
#include "stg43/terrain/collision_103_model.inc"
};
static NJS_OBJECT collision_103 = {
#include "stg43/terrain/collision_103.inc"
};
static NJS_MATERIAL collision_106_materials[1] = {
#include "stg43/terrain/collision_106_materials.inc"
};
static Sint16 collision_106_faces[14] = {
#include "stg43/terrain/collision_106_faces.inc"
};
static NJS_MESHSET collision_106_meshsets[1] = {
#include "stg43/terrain/collision_106_meshsets.inc"
};
static NJS_POINT3 collision_106_points[9] = {
#include "stg43/terrain/collision_106_points.inc"
};
static NJS_MODEL collision_106_model = {
#include "stg43/terrain/collision_106_model.inc"
};
static NJS_OBJECT collision_106 = {
#include "stg43/terrain/collision_106.inc"
};
static NJS_MATERIAL collision_107_materials[1] = {
#include "stg43/terrain/collision_107_materials.inc"
};
static Sint16 collision_107_faces[29] = {
#include "stg43/terrain/collision_107_faces.inc"
};
static NJS_MESHSET collision_107_meshsets[1] = {
#include "stg43/terrain/collision_107_meshsets.inc"
};
static NJS_POINT3 collision_107_points[18] = {
#include "stg43/terrain/collision_107_points.inc"
};
static NJS_MODEL collision_107_model = {
#include "stg43/terrain/collision_107_model.inc"
};
static NJS_OBJECT collision_107 = {
#include "stg43/terrain/collision_107.inc"
};
static NJS_MATERIAL collision_110_materials[1] = {
#include "stg43/terrain/collision_110_materials.inc"
};
static Sint16 collision_110_faces[21] = {
#include "stg43/terrain/collision_110_faces.inc"
};
static NJS_MESHSET collision_110_meshsets[1] = {
#include "stg43/terrain/collision_110_meshsets.inc"
};
static NJS_POINT3 collision_110_points[8] = {
#include "stg43/terrain/collision_110_points.inc"
};
static NJS_MODEL collision_110_model = {
#include "stg43/terrain/collision_110_model.inc"
};
static NJS_OBJECT collision_110 = {
#include "stg43/terrain/collision_110.inc"
};
static NJS_MATERIAL collision_122_materials[1] = {
#include "stg43/terrain/collision_122_materials.inc"
};
static Sint16 collision_122_faces[16] = {
#include "stg43/terrain/collision_122_faces.inc"
};
static NJS_MESHSET collision_122_meshsets[1] = {
#include "stg43/terrain/collision_122_meshsets.inc"
};
static NJS_POINT3 collision_122_points[11] = {
#include "stg43/terrain/collision_122_points.inc"
};
static NJS_MODEL collision_122_model = {
#include "stg43/terrain/collision_122_model.inc"
};
static NJS_OBJECT collision_122 = {
#include "stg43/terrain/collision_122.inc"
};
static NJS_MATERIAL collision_124_materials[1] = {
#include "stg43/terrain/collision_124_materials.inc"
};
static Sint16 collision_124_faces[16] = {
#include "stg43/terrain/collision_124_faces.inc"
};
static NJS_MESHSET collision_124_meshsets[1] = {
#include "stg43/terrain/collision_124_meshsets.inc"
};
static NJS_POINT3 collision_124_points[11] = {
#include "stg43/terrain/collision_124_points.inc"
};
static NJS_MODEL collision_124_model = {
#include "stg43/terrain/collision_124_model.inc"
};
static NJS_OBJECT collision_124 = {
#include "stg43/terrain/collision_124.inc"
};
static NJS_MATERIAL collision_125_materials[1] = {
#include "stg43/terrain/collision_125_materials.inc"
};
static Sint16 collision_125_faces[54] = {
#include "stg43/terrain/collision_125_faces.inc"
};
static NJS_MESHSET collision_125_meshsets[1] = {
#include "stg43/terrain/collision_125_meshsets.inc"
};
static NJS_POINT3 collision_125_points[24] = {
#include "stg43/terrain/collision_125_points.inc"
};
static NJS_MODEL collision_125_model = {
#include "stg43/terrain/collision_125_model.inc"
};
static NJS_OBJECT collision_125 = {
#include "stg43/terrain/collision_125.inc"
};
static NJS_MATERIAL collision_156_materials[1] = {
#include "stg43/terrain/collision_156_materials.inc"
};
static Sint16 collision_156_faces[14] = {
#include "stg43/terrain/collision_156_faces.inc"
};
static NJS_MESHSET collision_156_meshsets[1] = {
#include "stg43/terrain/collision_156_meshsets.inc"
};
static NJS_POINT3 collision_156_points[9] = {
#include "stg43/terrain/collision_156_points.inc"
};
static NJS_MODEL collision_156_model = {
#include "stg43/terrain/collision_156_model.inc"
};
static NJS_OBJECT collision_156 = {
#include "stg43/terrain/collision_156.inc"
};
static NJS_MATERIAL collision_157_materials[1] = {
#include "stg43/terrain/collision_157_materials.inc"
};
static Sint16 collision_157_faces[9] = {
#include "stg43/terrain/collision_157_faces.inc"
};
static NJS_MESHSET collision_157_meshsets[1] = {
#include "stg43/terrain/collision_157_meshsets.inc"
};
static NJS_POINT3 collision_157_points[8] = {
#include "stg43/terrain/collision_157_points.inc"
};
static NJS_MODEL collision_157_model = {
#include "stg43/terrain/collision_157_model.inc"
};
static NJS_OBJECT collision_157 = {
#include "stg43/terrain/collision_157.inc"
};
static NJS_CNK_OBJECT draw_346 = {
#include "stg43/terrain/draw_346.inc"
};
static NJS_CNK_OBJECT draw_345 = {
#include "stg43/terrain/draw_345.inc"
};
static NJS_CNK_OBJECT draw_344 = {
#include "stg43/terrain/draw_344.inc"
};
static NJS_CNK_OBJECT draw_343 = {
#include "stg43/terrain/draw_343.inc"
};
static NJS_CNK_OBJECT draw_342 = {
#include "stg43/terrain/draw_342.inc"
};
static NJS_CNK_OBJECT draw_341 = {
#include "stg43/terrain/draw_341.inc"
};
static NJS_CNK_OBJECT draw_340 = {
#include "stg43/terrain/draw_340.inc"
};
static NJS_CNK_OBJECT draw_339 = {
#include "stg43/terrain/draw_339.inc"
};
static NJS_CNK_OBJECT draw_338 = {
#include "stg43/terrain/draw_338.inc"
};
static NJS_CNK_OBJECT draw_337 = {
#include "stg43/terrain/draw_337.inc"
};
static NJS_CNK_OBJECT draw_336 = {
#include "stg43/terrain/draw_336.inc"
};
static NJS_CNK_OBJECT draw_335 = {
#include "stg43/terrain/draw_335.inc"
};
static NJS_CNK_OBJECT draw_334 = {
#include "stg43/terrain/draw_334.inc"
};
static NJS_CNK_OBJECT draw_333 = {
#include "stg43/terrain/draw_333.inc"
};
static NJS_CNK_OBJECT draw_323 = {
#include "stg43/terrain/draw_323.inc"
};
static NJS_CNK_OBJECT draw_322 = {
#include "stg43/terrain/draw_322.inc"
};
static NJS_CNK_OBJECT draw_321 = {
#include "stg43/terrain/draw_321.inc"
};
static NJS_CNK_OBJECT draw_320 = {
#include "stg43/terrain/draw_320.inc"
};
static NJS_CNK_OBJECT draw_319 = {
#include "stg43/terrain/draw_319.inc"
};
static NJS_CNK_OBJECT draw_318 = {
#include "stg43/terrain/draw_318.inc"
};
static NJS_CNK_OBJECT draw_317 = {
#include "stg43/terrain/draw_317.inc"
};
static NJS_CNK_OBJECT draw_316 = {
#include "stg43/terrain/draw_316.inc"
};
static NJS_CNK_OBJECT draw_315 = {
#include "stg43/terrain/draw_315.inc"
};
static NJS_CNK_OBJECT draw_314 = {
#include "stg43/terrain/draw_314.inc"
};
static NJS_CNK_OBJECT draw_313 = {
#include "stg43/terrain/draw_313.inc"
};
static NJS_CNK_OBJECT draw_312 = {
#include "stg43/terrain/draw_312.inc"
};
static NJS_CNK_OBJECT draw_311 = {
#include "stg43/terrain/draw_311.inc"
};
static NJS_CNK_OBJECT draw_309 = {
#include "stg43/terrain/draw_309.inc"
};
static NJS_CNK_OBJECT draw_306 = {
#include "stg43/terrain/draw_306.inc"
};
static NJS_CNK_OBJECT draw_305 = {
#include "stg43/terrain/draw_305.inc"
};
static NJS_CNK_OBJECT draw_301 = {
#include "stg43/terrain/draw_301.inc"
};
static NJS_CNK_OBJECT draw_299 = {
#include "stg43/terrain/draw_299.inc"
};
static NJS_CNK_OBJECT draw_298 = {
#include "stg43/terrain/draw_298.inc"
};
static NJS_CNK_OBJECT draw_297 = {
#include "stg43/terrain/draw_297.inc"
};
static NJS_CNK_OBJECT draw_293 = {
#include "stg43/terrain/draw_293.inc"
};
static NJS_CNK_OBJECT draw_292 = {
#include "stg43/terrain/draw_292.inc"
};
static NJS_CNK_OBJECT draw_291 = {
#include "stg43/terrain/draw_291.inc"
};
static NJS_CNK_OBJECT draw_290 = {
#include "stg43/terrain/draw_290.inc"
};
static NJS_CNK_OBJECT draw_289 = {
#include "stg43/terrain/draw_289.inc"
};
static NJS_CNK_OBJECT draw_288 = {
#include "stg43/terrain/draw_288.inc"
};
static NJS_CNK_OBJECT draw_287 = {
#include "stg43/terrain/draw_287.inc"
};
static NJS_CNK_OBJECT draw_286 = {
#include "stg43/terrain/draw_286.inc"
};
static NJS_CNK_OBJECT draw_285 = {
#include "stg43/terrain/draw_285.inc"
};
static NJS_CNK_OBJECT draw_282 = {
#include "stg43/terrain/draw_282.inc"
};
static NJS_CNK_OBJECT draw_281 = {
#include "stg43/terrain/draw_281.inc"
};
static NJS_CNK_OBJECT draw_280 = {
#include "stg43/terrain/draw_280.inc"
};
static NJS_CNK_OBJECT draw_279 = {
#include "stg43/terrain/draw_279.inc"
};
static NJS_CNK_OBJECT draw_278 = {
#include "stg43/terrain/draw_278.inc"
};
static NJS_CNK_OBJECT draw_277 = {
#include "stg43/terrain/draw_277.inc"
};
static NJS_CNK_OBJECT draw_276 = {
#include "stg43/terrain/draw_276.inc"
};
static NJS_CNK_OBJECT draw_275 = {
#include "stg43/terrain/draw_275.inc"
};
static NJS_CNK_OBJECT draw_274 = {
#include "stg43/terrain/draw_274.inc"
};
static NJS_CNK_OBJECT draw_273 = {
#include "stg43/terrain/draw_273.inc"
};
static NJS_CNK_OBJECT draw_272 = {
#include "stg43/terrain/draw_272.inc"
};
static NJS_CNK_OBJECT draw_271 = {
#include "stg43/terrain/draw_271.inc"
};
static NJS_CNK_OBJECT draw_270 = {
#include "stg43/terrain/draw_270.inc"
};
static NJS_CNK_OBJECT draw_269 = {
#include "stg43/terrain/draw_269.inc"
};
static NJS_CNK_OBJECT draw_268 = {
#include "stg43/terrain/draw_268.inc"
};
static NJS_CNK_OBJECT draw_267 = {
#include "stg43/terrain/draw_267.inc"
};
static NJS_CNK_OBJECT draw_266 = {
#include "stg43/terrain/draw_266.inc"
};
static NJS_CNK_OBJECT draw_265 = {
#include "stg43/terrain/draw_265.inc"
};
static NJS_CNK_OBJECT draw_264 = {
#include "stg43/terrain/draw_264.inc"
};
static NJS_CNK_OBJECT draw_263 = {
#include "stg43/terrain/draw_263.inc"
};
static NJS_CNK_OBJECT draw_262 = {
#include "stg43/terrain/draw_262.inc"
};
static NJS_CNK_OBJECT draw_261 = {
#include "stg43/terrain/draw_261.inc"
};
static NJS_CNK_OBJECT draw_260 = {
#include "stg43/terrain/draw_260.inc"
};
static NJS_CNK_OBJECT draw_259 = {
#include "stg43/terrain/draw_259.inc"
};
static NJS_CNK_OBJECT draw_258 = {
#include "stg43/terrain/draw_258.inc"
};
static NJS_CNK_OBJECT draw_257 = {
#include "stg43/terrain/draw_257.inc"
};
static NJS_CNK_OBJECT draw_256 = {
#include "stg43/terrain/draw_256.inc"
};
static NJS_CNK_OBJECT draw_255 = {
#include "stg43/terrain/draw_255.inc"
};
static NJS_CNK_OBJECT draw_254 = {
#include "stg43/terrain/draw_254.inc"
};
static NJS_CNK_OBJECT draw_253 = {
#include "stg43/terrain/draw_253.inc"
};
static NJS_CNK_OBJECT draw_252 = {
#include "stg43/terrain/draw_252.inc"
};
static NJS_CNK_OBJECT draw_251 = {
#include "stg43/terrain/draw_251.inc"
};
static NJS_CNK_OBJECT draw_250 = {
#include "stg43/terrain/draw_250.inc"
};
static NJS_CNK_OBJECT draw_249 = {
#include "stg43/terrain/draw_249.inc"
};
static NJS_CNK_OBJECT draw_248 = {
#include "stg43/terrain/draw_248.inc"
};
static NJS_CNK_OBJECT draw_247 = {
#include "stg43/terrain/draw_247.inc"
};
static NJS_CNK_OBJECT draw_246 = {
#include "stg43/terrain/draw_246.inc"
};
static NJS_CNK_OBJECT draw_245 = {
#include "stg43/terrain/draw_245.inc"
};
static NJS_CNK_OBJECT draw_244 = {
#include "stg43/terrain/draw_244.inc"
};
static NJS_CNK_OBJECT draw_243 = {
#include "stg43/terrain/draw_243.inc"
};
static NJS_CNK_OBJECT draw_242 = {
#include "stg43/terrain/draw_242.inc"
};
static NJS_CNK_OBJECT draw_241 = {
#include "stg43/terrain/draw_241.inc"
};
static NJS_CNK_OBJECT draw_240 = {
#include "stg43/terrain/draw_240.inc"
};
static NJS_CNK_OBJECT draw_239 = {
#include "stg43/terrain/draw_239.inc"
};
static NJS_CNK_OBJECT draw_238 = {
#include "stg43/terrain/draw_238.inc"
};
static NJS_CNK_OBJECT draw_237 = {
#include "stg43/terrain/draw_237.inc"
};
static NJS_CNK_OBJECT draw_236 = {
#include "stg43/terrain/draw_236.inc"
};
static NJS_CNK_OBJECT draw_235 = {
#include "stg43/terrain/draw_235.inc"
};
static NJS_CNK_OBJECT draw_234 = {
#include "stg43/terrain/draw_234.inc"
};
static NJS_CNK_OBJECT draw_233 = {
#include "stg43/terrain/draw_233.inc"
};
static NJS_CNK_OBJECT draw_232 = {
#include "stg43/terrain/draw_232.inc"
};
static NJS_CNK_OBJECT draw_231 = {
#include "stg43/terrain/draw_231.inc"
};
static NJS_CNK_OBJECT draw_230 = {
#include "stg43/terrain/draw_230.inc"
};
static NJS_CNK_OBJECT draw_229 = {
#include "stg43/terrain/draw_229.inc"
};
static NJS_CNK_OBJECT draw_228 = {
#include "stg43/terrain/draw_228.inc"
};
static NJS_CNK_OBJECT draw_227 = {
#include "stg43/terrain/draw_227.inc"
};
static NJS_CNK_OBJECT draw_226 = {
#include "stg43/terrain/draw_226.inc"
};
static NJS_CNK_OBJECT draw_225 = {
#include "stg43/terrain/draw_225.inc"
};
static NJS_CNK_OBJECT draw_224 = {
#include "stg43/terrain/draw_224.inc"
};
static NJS_CNK_OBJECT draw_223 = {
#include "stg43/terrain/draw_223.inc"
};
static NJS_CNK_OBJECT draw_222 = {
#include "stg43/terrain/draw_222.inc"
};
static NJS_CNK_OBJECT draw_221 = {
#include "stg43/terrain/draw_221.inc"
};
static NJS_CNK_OBJECT draw_220 = {
#include "stg43/terrain/draw_220.inc"
};
static NJS_CNK_OBJECT draw_219 = {
#include "stg43/terrain/draw_219.inc"
};
static NJS_CNK_OBJECT draw_218 = {
#include "stg43/terrain/draw_218.inc"
};
static NJS_CNK_OBJECT draw_217 = {
#include "stg43/terrain/draw_217.inc"
};
static NJS_CNK_OBJECT draw_216 = {
#include "stg43/terrain/draw_216.inc"
};
static NJS_CNK_OBJECT draw_215 = {
#include "stg43/terrain/draw_215.inc"
};
static NJS_CNK_OBJECT draw_214 = {
#include "stg43/terrain/draw_214.inc"
};
static NJS_CNK_OBJECT draw_213 = {
#include "stg43/terrain/draw_213.inc"
};
static NJS_CNK_OBJECT draw_212 = {
#include "stg43/terrain/draw_212.inc"
};
static NJS_CNK_OBJECT draw_211 = {
#include "stg43/terrain/draw_211.inc"
};
static NJS_CNK_OBJECT draw_210 = {
#include "stg43/terrain/draw_210.inc"
};
static NJS_CNK_OBJECT draw_202 = {
#include "stg43/terrain/draw_202.inc"
};
static NJS_CNK_OBJECT draw_200 = {
#include "stg43/terrain/draw_200.inc"
};
static NJS_CNK_OBJECT draw_199 = {
#include "stg43/terrain/draw_199.inc"
};
static NJS_CNK_OBJECT draw_198 = {
#include "stg43/terrain/draw_198.inc"
};
static NJS_CNK_OBJECT draw_197 = {
#include "stg43/terrain/draw_197.inc"
};
static NJS_CNK_OBJECT draw_196 = {
#include "stg43/terrain/draw_196.inc"
};
static NJS_CNK_OBJECT draw_195 = {
#include "stg43/terrain/draw_195.inc"
};
static NJS_CNK_OBJECT draw_194 = {
#include "stg43/terrain/draw_194.inc"
};
static NJS_CNK_OBJECT draw_193 = {
#include "stg43/terrain/draw_193.inc"
};
static NJS_CNK_OBJECT draw_192 = {
#include "stg43/terrain/draw_192.inc"
};
static NJS_CNK_OBJECT draw_191 = {
#include "stg43/terrain/draw_191.inc"
};
static NJS_CNK_OBJECT draw_190 = {
#include "stg43/terrain/draw_190.inc"
};
static NJS_CNK_OBJECT draw_189 = {
#include "stg43/terrain/draw_189.inc"
};
static NJS_CNK_OBJECT draw_188 = {
#include "stg43/terrain/draw_188.inc"
};
static NJS_CNK_OBJECT draw_187 = {
#include "stg43/terrain/draw_187.inc"
};
static NJS_CNK_OBJECT draw_186 = {
#include "stg43/terrain/draw_186.inc"
};
static NJS_CNK_OBJECT draw_184 = {
#include "stg43/terrain/draw_184.inc"
};
static NJS_CNK_OBJECT draw_183 = {
#include "stg43/terrain/draw_183.inc"
};
static NJS_CNK_OBJECT draw_182 = {
#include "stg43/terrain/draw_182.inc"
};
static NJS_CNK_OBJECT draw_181 = {
#include "stg43/terrain/draw_181.inc"
};
static NJS_CNK_OBJECT draw_180 = {
#include "stg43/terrain/draw_180.inc"
};
static NJS_CNK_OBJECT draw_179 = {
#include "stg43/terrain/draw_179.inc"
};
static NJS_CNK_OBJECT draw_178 = {
#include "stg43/terrain/draw_178.inc"
};
static NJS_CNK_OBJECT draw_177 = {
#include "stg43/terrain/draw_177.inc"
};
static NJS_CNK_OBJECT draw_176 = {
#include "stg43/terrain/draw_176.inc"
};
static NJS_CNK_OBJECT draw_175 = {
#include "stg43/terrain/draw_175.inc"
};
static NJS_CNK_OBJECT draw_174 = {
#include "stg43/terrain/draw_174.inc"
};
static NJS_CNK_OBJECT draw_173 = {
#include "stg43/terrain/draw_173.inc"
};
static NJS_CNK_OBJECT draw_172 = {
#include "stg43/terrain/draw_172.inc"
};
static NJS_CNK_OBJECT draw_171 = {
#include "stg43/terrain/draw_171.inc"
};
static NJS_CNK_OBJECT draw_170 = {
#include "stg43/terrain/draw_170.inc"
};
static NJS_CNK_OBJECT draw_169 = {
#include "stg43/terrain/draw_169.inc"
};
static NJS_CNK_OBJECT draw_168 = {
#include "stg43/terrain/draw_168.inc"
};
static NJS_CNK_OBJECT draw_167 = {
#include "stg43/terrain/draw_167.inc"
};
static NJS_CNK_OBJECT draw_166 = {
#include "stg43/terrain/draw_166.inc"
};
static NJS_CNK_OBJECT draw_165 = {
#include "stg43/terrain/draw_165.inc"
};
static NJS_CNK_OBJECT draw_164 = {
#include "stg43/terrain/draw_164.inc"
};
static NJS_CNK_OBJECT draw_163 = {
#include "stg43/terrain/draw_163.inc"
};
static NJS_CNK_OBJECT draw_162 = {
#include "stg43/terrain/draw_162.inc"
};
static NJS_CNK_OBJECT draw_161 = {
#include "stg43/terrain/draw_161.inc"
};
static NJS_CNK_OBJECT draw_160 = {
#include "stg43/terrain/draw_160.inc"
};
static NJS_CNK_OBJECT draw_159 = {
#include "stg43/terrain/draw_159.inc"
};
static NJS_CNK_OBJECT draw_158 = {
#include "stg43/terrain/draw_158.inc"
};
static NJS_CNK_OBJECT draw_157 = {
#include "stg43/terrain/draw_157.inc"
};
static NJS_CNK_OBJECT draw_156 = {
#include "stg43/terrain/draw_156.inc"
};
static NJS_CNK_OBJECT draw_155 = {
#include "stg43/terrain/draw_155.inc"
};
static NJS_CNK_OBJECT draw_154 = {
#include "stg43/terrain/draw_154.inc"
};
static NJS_CNK_OBJECT draw_152 = {
#include "stg43/terrain/draw_152.inc"
};
static NJS_CNK_OBJECT draw_151 = {
#include "stg43/terrain/draw_151.inc"
};
static NJS_CNK_OBJECT draw_150 = {
#include "stg43/terrain/draw_150.inc"
};
static NJS_CNK_OBJECT draw_149 = {
#include "stg43/terrain/draw_149.inc"
};
static NJS_CNK_OBJECT draw_148 = {
#include "stg43/terrain/draw_148.inc"
};
static NJS_CNK_OBJECT draw_147 = {
#include "stg43/terrain/draw_147.inc"
};
static NJS_CNK_OBJECT draw_146 = {
#include "stg43/terrain/draw_146.inc"
};
static NJS_CNK_OBJECT draw_145 = {
#include "stg43/terrain/draw_145.inc"
};
static NJS_CNK_OBJECT draw_144 = {
#include "stg43/terrain/draw_144.inc"
};
static NJS_CNK_OBJECT draw_143 = {
#include "stg43/terrain/draw_143.inc"
};
static NJS_CNK_OBJECT draw_142 = {
#include "stg43/terrain/draw_142.inc"
};
static NJS_CNK_OBJECT draw_141 = {
#include "stg43/terrain/draw_141.inc"
};
static NJS_CNK_OBJECT draw_140 = {
#include "stg43/terrain/draw_140.inc"
};
static NJS_CNK_OBJECT draw_139 = {
#include "stg43/terrain/draw_139.inc"
};
static NJS_CNK_OBJECT draw_138 = {
#include "stg43/terrain/draw_138.inc"
};
static NJS_CNK_OBJECT draw_137 = {
#include "stg43/terrain/draw_137.inc"
};
static NJS_CNK_OBJECT draw_136 = {
#include "stg43/terrain/draw_136.inc"
};
static NJS_CNK_OBJECT draw_135 = {
#include "stg43/terrain/draw_135.inc"
};
static NJS_CNK_OBJECT draw_134 = {
#include "stg43/terrain/draw_134.inc"
};
static NJS_CNK_OBJECT draw_133 = {
#include "stg43/terrain/draw_133.inc"
};
static NJS_CNK_OBJECT draw_132 = {
#include "stg43/terrain/draw_132.inc"
};
static NJS_CNK_OBJECT draw_131 = {
#include "stg43/terrain/draw_131.inc"
};
static NJS_CNK_OBJECT draw_130 = {
#include "stg43/terrain/draw_130.inc"
};
static NJS_CNK_OBJECT draw_129 = {
#include "stg43/terrain/draw_129.inc"
};
static NJS_CNK_OBJECT draw_128 = {
#include "stg43/terrain/draw_128.inc"
};
static NJS_CNK_OBJECT draw_127 = {
#include "stg43/terrain/draw_127.inc"
};
static NJS_CNK_OBJECT draw_126 = {
#include "stg43/terrain/draw_126.inc"
};
static NJS_CNK_OBJECT draw_125 = {
#include "stg43/terrain/draw_125.inc"
};
static NJS_CNK_OBJECT draw_124 = {
#include "stg43/terrain/draw_124.inc"
};
static NJS_CNK_OBJECT draw_119 = {
#include "stg43/terrain/draw_119.inc"
};
static NJS_CNK_OBJECT draw_118 = {
#include "stg43/terrain/draw_118.inc"
};
static NJS_CNK_OBJECT draw_117 = {
#include "stg43/terrain/draw_117.inc"
};
static NJS_CNK_OBJECT draw_116 = {
#include "stg43/terrain/draw_116.inc"
};
static NJS_CNK_OBJECT draw_115 = {
#include "stg43/terrain/draw_115.inc"
};
static NJS_CNK_OBJECT draw_114 = {
#include "stg43/terrain/draw_114.inc"
};
static NJS_CNK_OBJECT draw_113 = {
#include "stg43/terrain/draw_113.inc"
};
static NJS_CNK_OBJECT draw_112 = {
#include "stg43/terrain/draw_112.inc"
};
static NJS_CNK_OBJECT draw_111 = {
#include "stg43/terrain/draw_111.inc"
};
static NJS_CNK_OBJECT draw_110 = {
#include "stg43/terrain/draw_110.inc"
};
static NJS_CNK_OBJECT draw_109 = {
#include "stg43/terrain/draw_109.inc"
};
static NJS_CNK_OBJECT draw_108 = {
#include "stg43/terrain/draw_108.inc"
};
static NJS_CNK_OBJECT draw_107 = {
#include "stg43/terrain/draw_107.inc"
};
static NJS_CNK_OBJECT draw_106 = {
#include "stg43/terrain/draw_106.inc"
};
static NJS_CNK_OBJECT draw_105 = {
#include "stg43/terrain/draw_105.inc"
};
static NJS_CNK_OBJECT draw_104 = {
#include "stg43/terrain/draw_104.inc"
};
static NJS_CNK_OBJECT draw_103 = {
#include "stg43/terrain/draw_103.inc"
};
static NJS_CNK_OBJECT draw_102 = {
#include "stg43/terrain/draw_102.inc"
};
static NJS_CNK_OBJECT draw_101 = {
#include "stg43/terrain/draw_101.inc"
};
static NJS_CNK_OBJECT draw_100 = {
#include "stg43/terrain/draw_100.inc"
};
static NJS_CNK_OBJECT draw_99 = {
#include "stg43/terrain/draw_99.inc"
};
static NJS_CNK_OBJECT draw_98 = {
#include "stg43/terrain/draw_98.inc"
};
static NJS_CNK_OBJECT draw_97 = {
#include "stg43/terrain/draw_97.inc"
};
static NJS_CNK_OBJECT draw_96 = {
#include "stg43/terrain/draw_96.inc"
};
static NJS_CNK_OBJECT draw_95 = {
#include "stg43/terrain/draw_95.inc"
};
static NJS_CNK_OBJECT draw_94 = {
#include "stg43/terrain/draw_94.inc"
};
static NJS_CNK_OBJECT draw_93 = {
#include "stg43/terrain/draw_93.inc"
};
static NJS_CNK_OBJECT draw_92 = {
#include "stg43/terrain/draw_92.inc"
};
static NJS_CNK_OBJECT draw_91 = {
#include "stg43/terrain/draw_91.inc"
};
static NJS_CNK_OBJECT draw_90 = {
#include "stg43/terrain/draw_90.inc"
};
static NJS_CNK_OBJECT draw_89 = {
#include "stg43/terrain/draw_89.inc"
};
static NJS_CNK_OBJECT draw_88 = {
#include "stg43/terrain/draw_88.inc"
};
static NJS_CNK_OBJECT draw_87 = {
#include "stg43/terrain/draw_87.inc"
};
static NJS_CNK_OBJECT draw_85 = {
#include "stg43/terrain/draw_85.inc"
};
static NJS_CNK_OBJECT draw_84 = {
#include "stg43/terrain/draw_84.inc"
};
static NJS_CNK_OBJECT draw_83 = {
#include "stg43/terrain/draw_83.inc"
};
static NJS_CNK_OBJECT draw_82 = {
#include "stg43/terrain/draw_82.inc"
};
static NJS_CNK_OBJECT draw_81 = {
#include "stg43/terrain/draw_81.inc"
};
static NJS_CNK_OBJECT draw_80 = {
#include "stg43/terrain/draw_80.inc"
};
static NJS_CNK_OBJECT draw_79 = {
#include "stg43/terrain/draw_79.inc"
};
static NJS_CNK_OBJECT draw_78 = {
#include "stg43/terrain/draw_78.inc"
};
static NJS_CNK_OBJECT draw_77 = {
#include "stg43/terrain/draw_77.inc"
};
static NJS_CNK_OBJECT draw_76 = {
#include "stg43/terrain/draw_76.inc"
};
static NJS_CNK_OBJECT draw_75 = {
#include "stg43/terrain/draw_75.inc"
};
static NJS_CNK_OBJECT draw_74 = {
#include "stg43/terrain/draw_74.inc"
};
static NJS_CNK_OBJECT draw_73 = {
#include "stg43/terrain/draw_73.inc"
};
static NJS_CNK_OBJECT draw_72 = {
#include "stg43/terrain/draw_72.inc"
};
static NJS_CNK_OBJECT draw_71 = {
#include "stg43/terrain/draw_71.inc"
};
static NJS_CNK_OBJECT draw_70 = {
#include "stg43/terrain/draw_70.inc"
};
static NJS_CNK_OBJECT draw_67 = {
#include "stg43/terrain/draw_67.inc"
};
static NJS_CNK_OBJECT draw_66 = {
#include "stg43/terrain/draw_66.inc"
};
static NJS_CNK_OBJECT draw_65 = {
#include "stg43/terrain/draw_65.inc"
};
static NJS_CNK_OBJECT draw_62 = {
#include "stg43/terrain/draw_62.inc"
};
static NJS_CNK_OBJECT draw_61 = {
#include "stg43/terrain/draw_61.inc"
};
static NJS_CNK_OBJECT draw_59 = {
#include "stg43/terrain/draw_59.inc"
};
static NJS_CNK_OBJECT draw_58 = {
#include "stg43/terrain/draw_58.inc"
};
static NJS_CNK_OBJECT draw_57 = {
#include "stg43/terrain/draw_57.inc"
};
static NJS_CNK_OBJECT draw_56 = {
#include "stg43/terrain/draw_56.inc"
};
static NJS_CNK_OBJECT draw_55 = {
#include "stg43/terrain/draw_55.inc"
};
static NJS_CNK_OBJECT draw_54 = {
#include "stg43/terrain/draw_54.inc"
};
static NJS_CNK_OBJECT draw_53 = {
#include "stg43/terrain/draw_53.inc"
};
static NJS_CNK_OBJECT draw_51 = {
#include "stg43/terrain/draw_51.inc"
};
static NJS_CNK_OBJECT draw_50 = {
#include "stg43/terrain/draw_50.inc"
};
static NJS_CNK_OBJECT draw_49 = {
#include "stg43/terrain/draw_49.inc"
};
static NJS_CNK_OBJECT draw_48 = {
#include "stg43/terrain/draw_48.inc"
};
static NJS_CNK_OBJECT draw_47 = {
#include "stg43/terrain/draw_47.inc"
};
static NJS_CNK_OBJECT draw_46 = {
#include "stg43/terrain/draw_46.inc"
};
static NJS_CNK_OBJECT draw_45 = {
#include "stg43/terrain/draw_45.inc"
};
static NJS_CNK_OBJECT draw_44 = {
#include "stg43/terrain/draw_44.inc"
};
static NJS_CNK_OBJECT draw_43 = {
#include "stg43/terrain/draw_43.inc"
};
static NJS_CNK_OBJECT draw_42 = {
#include "stg43/terrain/draw_42.inc"
};
static NJS_CNK_OBJECT draw_41 = {
#include "stg43/terrain/draw_41.inc"
};
static NJS_CNK_OBJECT draw_40 = {
#include "stg43/terrain/draw_40.inc"
};
static NJS_CNK_OBJECT draw_39 = {
#include "stg43/terrain/draw_39.inc"
};
static NJS_CNK_OBJECT draw_38 = {
#include "stg43/terrain/draw_38.inc"
};
static NJS_CNK_OBJECT draw_37 = {
#include "stg43/terrain/draw_37.inc"
};
static NJS_CNK_OBJECT draw_36 = {
#include "stg43/terrain/draw_36.inc"
};
static NJS_CNK_OBJECT draw_35 = {
#include "stg43/terrain/draw_35.inc"
};
static NJS_CNK_OBJECT draw_34 = {
#include "stg43/terrain/draw_34.inc"
};
static NJS_CNK_OBJECT draw_33 = {
#include "stg43/terrain/draw_33.inc"
};
static NJS_CNK_OBJECT draw_32 = {
#include "stg43/terrain/draw_32.inc"
};
static NJS_CNK_OBJECT draw_31 = {
#include "stg43/terrain/draw_31.inc"
};
static NJS_CNK_OBJECT draw_30 = {
#include "stg43/terrain/draw_30.inc"
};
static NJS_CNK_OBJECT draw_29 = {
#include "stg43/terrain/draw_29.inc"
};
static NJS_CNK_OBJECT draw_28 = {
#include "stg43/terrain/draw_28.inc"
};
static NJS_CNK_OBJECT draw_27 = {
#include "stg43/terrain/draw_27.inc"
};
static NJS_CNK_OBJECT draw_25 = {
#include "stg43/terrain/draw_25.inc"
};
static NJS_CNK_OBJECT draw_24 = {
#include "stg43/terrain/draw_24.inc"
};
static NJS_CNK_OBJECT draw_23 = {
#include "stg43/terrain/draw_23.inc"
};
static NJS_CNK_OBJECT draw_22 = {
#include "stg43/terrain/draw_22.inc"
};
static NJS_CNK_OBJECT draw_21 = {
#include "stg43/terrain/draw_21.inc"
};
static NJS_CNK_OBJECT draw_20 = {
#include "stg43/terrain/draw_20.inc"
};
static NJS_CNK_OBJECT draw_19 = {
#include "stg43/terrain/draw_19.inc"
};
static NJS_CNK_OBJECT draw_18 = {
#include "stg43/terrain/draw_18.inc"
};
static NJS_CNK_OBJECT draw_17 = {
#include "stg43/terrain/draw_17.inc"
};
static NJS_CNK_OBJECT draw_16 = {
#include "stg43/terrain/draw_16.inc"
};
static NJS_CNK_OBJECT draw_15 = {
#include "stg43/terrain/draw_15.inc"
};
static NJS_CNK_OBJECT draw_14 = {
#include "stg43/terrain/draw_14.inc"
};
static NJS_CNK_OBJECT draw_13 = {
#include "stg43/terrain/draw_13.inc"
};
static NJS_CNK_OBJECT draw_12 = {
#include "stg43/terrain/draw_12.inc"
};
static NJS_CNK_OBJECT draw_11 = {
#include "stg43/terrain/draw_11.inc"
};
static NJS_CNK_OBJECT draw_10 = {
#include "stg43/terrain/draw_10.inc"
};
static NJS_CNK_OBJECT draw_9 = {
#include "stg43/terrain/draw_9.inc"
};
static NJS_CNK_OBJECT draw_8 = {
#include "stg43/terrain/draw_8.inc"
};
static NJS_CNK_OBJECT draw_7 = {
#include "stg43/terrain/draw_7.inc"
};
static NJS_CNK_OBJECT draw_5 = {
#include "stg43/terrain/draw_5.inc"
};
static NJS_CNK_OBJECT draw_4 = {
#include "stg43/terrain/draw_4.inc"
};
static NJS_CNK_OBJECT draw_3 = {
#include "stg43/terrain/draw_3.inc"
};
static NJS_CNK_OBJECT draw_1 = {
#include "stg43/terrain/draw_1.inc"
};
static NJS_CNK_OBJECT draw_0 = {
#include "stg43/terrain/draw_0.inc"
};
static NJS_OBJECT collision_1 = {
#include "stg43/terrain/collision_1.inc"
};
static NJS_OBJECT collision_3 = {
#include "stg43/terrain/collision_3.inc"
};
static NJS_OBJECT collision_4 = {
#include "stg43/terrain/collision_4.inc"
};
static NJS_OBJECT collision_6 = {
#include "stg43/terrain/collision_6.inc"
};
static NJS_OBJECT collision_7 = {
#include "stg43/terrain/collision_7.inc"
};
static NJS_OBJECT collision_9 = {
#include "stg43/terrain/collision_9.inc"
};
static NJS_OBJECT collision_10 = {
#include "stg43/terrain/collision_10.inc"
};
static NJS_OBJECT collision_12 = {
#include "stg43/terrain/collision_12.inc"
};
static NJS_OBJECT collision_13 = {
#include "stg43/terrain/collision_13.inc"
};
static NJS_OBJECT collision_15 = {
#include "stg43/terrain/collision_15.inc"
};
static NJS_OBJECT collision_16 = {
#include "stg43/terrain/collision_16.inc"
};
static NJS_OBJECT collision_18 = {
#include "stg43/terrain/collision_18.inc"
};
static NJS_OBJECT collision_21 = {
#include "stg43/terrain/collision_21.inc"
};
static NJS_OBJECT collision_22 = {
#include "stg43/terrain/collision_22.inc"
};
static NJS_OBJECT collision_25 = {
#include "stg43/terrain/collision_25.inc"
};
static NJS_OBJECT collision_28 = {
#include "stg43/terrain/collision_28.inc"
};
static NJS_OBJECT collision_30 = {
#include "stg43/terrain/collision_30.inc"
};
static NJS_OBJECT collision_33 = {
#include "stg43/terrain/collision_33.inc"
};
static NJS_OBJECT collision_34 = {
#include "stg43/terrain/collision_34.inc"
};
static NJS_OBJECT collision_35 = {
#include "stg43/terrain/collision_35.inc"
};
static NJS_OBJECT collision_36 = {
#include "stg43/terrain/collision_36.inc"
};
static NJS_OBJECT collision_37 = {
#include "stg43/terrain/collision_37.inc"
};
static NJS_OBJECT collision_38 = {
#include "stg43/terrain/collision_38.inc"
};
static NJS_OBJECT collision_39 = {
#include "stg43/terrain/collision_39.inc"
};
static NJS_OBJECT collision_40 = {
#include "stg43/terrain/collision_40.inc"
};
static NJS_OBJECT collision_41 = {
#include "stg43/terrain/collision_41.inc"
};
static NJS_OBJECT collision_42 = {
#include "stg43/terrain/collision_42.inc"
};
static NJS_OBJECT collision_43 = {
#include "stg43/terrain/collision_43.inc"
};
static NJS_OBJECT collision_44 = {
#include "stg43/terrain/collision_44.inc"
};
static NJS_OBJECT collision_45 = {
#include "stg43/terrain/collision_45.inc"
};
static NJS_OBJECT collision_47 = {
#include "stg43/terrain/collision_47.inc"
};
static NJS_OBJECT collision_48 = {
#include "stg43/terrain/collision_48.inc"
};
static NJS_OBJECT collision_50 = {
#include "stg43/terrain/collision_50.inc"
};
static NJS_OBJECT collision_51 = {
#include "stg43/terrain/collision_51.inc"
};
static NJS_OBJECT collision_53 = {
#include "stg43/terrain/collision_53.inc"
};
static NJS_OBJECT collision_54 = {
#include "stg43/terrain/collision_54.inc"
};
static NJS_OBJECT collision_55 = {
#include "stg43/terrain/collision_55.inc"
};
static NJS_OBJECT collision_57 = {
#include "stg43/terrain/collision_57.inc"
};
static NJS_OBJECT collision_58 = {
#include "stg43/terrain/collision_58.inc"
};
static NJS_OBJECT collision_60 = {
#include "stg43/terrain/collision_60.inc"
};
static NJS_OBJECT collision_61 = {
#include "stg43/terrain/collision_61.inc"
};
static NJS_OBJECT collision_62 = {
#include "stg43/terrain/collision_62.inc"
};
static NJS_OBJECT collision_63 = {
#include "stg43/terrain/collision_63.inc"
};
static NJS_OBJECT collision_64 = {
#include "stg43/terrain/collision_64.inc"
};
static NJS_OBJECT collision_66 = {
#include "stg43/terrain/collision_66.inc"
};
static NJS_OBJECT collision_67 = {
#include "stg43/terrain/collision_67.inc"
};
static NJS_OBJECT collision_68 = {
#include "stg43/terrain/collision_68.inc"
};
static NJS_OBJECT collision_69 = {
#include "stg43/terrain/collision_69.inc"
};
static NJS_OBJECT collision_70 = {
#include "stg43/terrain/collision_70.inc"
};
static NJS_OBJECT collision_71 = {
#include "stg43/terrain/collision_71.inc"
};
static NJS_OBJECT collision_72 = {
#include "stg43/terrain/collision_72.inc"
};
static NJS_OBJECT collision_73 = {
#include "stg43/terrain/collision_73.inc"
};
static NJS_OBJECT collision_74 = {
#include "stg43/terrain/collision_74.inc"
};
static NJS_OBJECT collision_75 = {
#include "stg43/terrain/collision_75.inc"
};
static NJS_OBJECT collision_76 = {
#include "stg43/terrain/collision_76.inc"
};
static NJS_OBJECT collision_77 = {
#include "stg43/terrain/collision_77.inc"
};
static NJS_OBJECT collision_78 = {
#include "stg43/terrain/collision_78.inc"
};
static NJS_OBJECT collision_79 = {
#include "stg43/terrain/collision_79.inc"
};
static NJS_OBJECT collision_80 = {
#include "stg43/terrain/collision_80.inc"
};
static NJS_OBJECT collision_81 = {
#include "stg43/terrain/collision_81.inc"
};
static NJS_OBJECT collision_82 = {
#include "stg43/terrain/collision_82.inc"
};
static NJS_OBJECT collision_83 = {
#include "stg43/terrain/collision_83.inc"
};
static NJS_OBJECT collision_84 = {
#include "stg43/terrain/collision_84.inc"
};
static NJS_OBJECT collision_85 = {
#include "stg43/terrain/collision_85.inc"
};
static NJS_OBJECT collision_86 = {
#include "stg43/terrain/collision_86.inc"
};
static NJS_OBJECT collision_87 = {
#include "stg43/terrain/collision_87.inc"
};
static NJS_OBJECT collision_88 = {
#include "stg43/terrain/collision_88.inc"
};
static NJS_OBJECT collision_89 = {
#include "stg43/terrain/collision_89.inc"
};
static NJS_OBJECT collision_90 = {
#include "stg43/terrain/collision_90.inc"
};
static NJS_OBJECT collision_91 = {
#include "stg43/terrain/collision_91.inc"
};
static NJS_OBJECT collision_92 = {
#include "stg43/terrain/collision_92.inc"
};
static NJS_OBJECT collision_93 = {
#include "stg43/terrain/collision_93.inc"
};
static NJS_OBJECT collision_94 = {
#include "stg43/terrain/collision_94.inc"
};
static NJS_OBJECT collision_95 = {
#include "stg43/terrain/collision_95.inc"
};
static NJS_OBJECT collision_97 = {
#include "stg43/terrain/collision_97.inc"
};
static NJS_OBJECT collision_104 = {
#include "stg43/terrain/collision_104.inc"
};
static NJS_OBJECT collision_105 = {
#include "stg43/terrain/collision_105.inc"
};
static NJS_OBJECT collision_108 = {
#include "stg43/terrain/collision_108.inc"
};
static NJS_OBJECT collision_109 = {
#include "stg43/terrain/collision_109.inc"
};
static NJS_OBJECT collision_111 = {
#include "stg43/terrain/collision_111.inc"
};
static NJS_OBJECT collision_112 = {
#include "stg43/terrain/collision_112.inc"
};
static NJS_OBJECT collision_113 = {
#include "stg43/terrain/collision_113.inc"
};
static NJS_OBJECT collision_114 = {
#include "stg43/terrain/collision_114.inc"
};
static NJS_OBJECT collision_115 = {
#include "stg43/terrain/collision_115.inc"
};
static NJS_OBJECT collision_116 = {
#include "stg43/terrain/collision_116.inc"
};
static NJS_OBJECT collision_117 = {
#include "stg43/terrain/collision_117.inc"
};
static NJS_OBJECT collision_118 = {
#include "stg43/terrain/collision_118.inc"
};
static NJS_OBJECT collision_119 = {
#include "stg43/terrain/collision_119.inc"
};
static NJS_OBJECT collision_120 = {
#include "stg43/terrain/collision_120.inc"
};
static NJS_OBJECT collision_121 = {
#include "stg43/terrain/collision_121.inc"
};
static NJS_OBJECT collision_123 = {
#include "stg43/terrain/collision_123.inc"
};
static NJS_OBJECT collision_126 = {
#include "stg43/terrain/collision_126.inc"
};
static NJS_OBJECT collision_127 = {
#include "stg43/terrain/collision_127.inc"
};
static NJS_OBJECT collision_128 = {
#include "stg43/terrain/collision_128.inc"
};
static NJS_OBJECT collision_129 = {
#include "stg43/terrain/collision_129.inc"
};
static NJS_OBJECT collision_130 = {
#include "stg43/terrain/collision_130.inc"
};
static NJS_OBJECT collision_131 = {
#include "stg43/terrain/collision_131.inc"
};
static NJS_OBJECT collision_132 = {
#include "stg43/terrain/collision_132.inc"
};
static NJS_OBJECT collision_133 = {
#include "stg43/terrain/collision_133.inc"
};
static NJS_OBJECT collision_134 = {
#include "stg43/terrain/collision_134.inc"
};
static NJS_OBJECT collision_135 = {
#include "stg43/terrain/collision_135.inc"
};
static NJS_OBJECT collision_136 = {
#include "stg43/terrain/collision_136.inc"
};
static NJS_OBJECT collision_137 = {
#include "stg43/terrain/collision_137.inc"
};
static NJS_OBJECT collision_138 = {
#include "stg43/terrain/collision_138.inc"
};
static NJS_OBJECT collision_139 = {
#include "stg43/terrain/collision_139.inc"
};
static NJS_OBJECT collision_140 = {
#include "stg43/terrain/collision_140.inc"
};
static NJS_OBJECT collision_141 = {
#include "stg43/terrain/collision_141.inc"
};
static NJS_OBJECT collision_142 = {
#include "stg43/terrain/collision_142.inc"
};
static NJS_OBJECT collision_143 = {
#include "stg43/terrain/collision_143.inc"
};
static NJS_OBJECT collision_144 = {
#include "stg43/terrain/collision_144.inc"
};
static NJS_OBJECT collision_145 = {
#include "stg43/terrain/collision_145.inc"
};
static NJS_OBJECT collision_146 = {
#include "stg43/terrain/collision_146.inc"
};
static NJS_OBJECT collision_147 = {
#include "stg43/terrain/collision_147.inc"
};
static NJS_OBJECT collision_148 = {
#include "stg43/terrain/collision_148.inc"
};
static NJS_OBJECT collision_149 = {
#include "stg43/terrain/collision_149.inc"
};
static NJS_OBJECT collision_150 = {
#include "stg43/terrain/collision_150.inc"
};
static NJS_OBJECT collision_151 = {
#include "stg43/terrain/collision_151.inc"
};
static NJS_OBJECT collision_152 = {
#include "stg43/terrain/collision_152.inc"
};
static NJS_OBJECT collision_153 = {
#include "stg43/terrain/collision_153.inc"
};
static NJS_OBJECT collision_154 = {
#include "stg43/terrain/collision_154.inc"
};
static NJS_OBJECT collision_155 = {
#include "stg43/terrain/collision_155.inc"
};
