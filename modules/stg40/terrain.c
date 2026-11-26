#include <ninja.h>
#include <sg_xpt.h>

#include "collision/world.h"
#include "stg40/terrain.h"

/* the land's file names, which its land table and texture list point at. each
 * starts on a 4-byte boundary */
const char final_chase_land_filenames[411] = {
#include "stg40/terrain/final_chase_land_filenames.inc"
};

/* STG40.PRS's land, texture lists, chunk models and Basic models, from the
 * disc. its D lies between final_chase.c and sky_land.c */

/* the data up to the last that a pointer names ahead of its definition, as
 * tentative definitions in the disc's order: GCC places each datum at its
 * first definition */
static struct land_entry entries[928];
struct land_table final_chase_land;
static NJS_TEXNAME textures[34];
NJS_TEXLIST final_chase_land_texlist;
static Sint16 draw_1_polygons[271];
static Sint32 draw_1_vertices[279];
static NJS_CNK_MODEL draw_1_model;
static NJS_CNK_OBJECT draw_533;
static Sint16 draw_2_polygons[271];
static Sint32 draw_2_vertices[303];
static NJS_CNK_MODEL draw_2_model;
static NJS_CNK_OBJECT draw_532;
static Sint16 draw_19_polygons[555];
static Sint32 draw_19_vertices[379];
static NJS_CNK_MODEL draw_19_model;
static NJS_CNK_OBJECT draw_508;
static Sint16 draw_164_polygons[551];
static Sint32 draw_164_vertices[415];
static NJS_CNK_MODEL draw_164_model;
static NJS_CNK_OBJECT draw_496;
static Sint16 draw_14_polygons[387];
static Sint32 draw_14_vertices[339];
static NJS_CNK_MODEL draw_14_model;
static NJS_CNK_OBJECT draw_495;
static Sint16 draw_18_polygons[429];
static Sint32 draw_18_vertices[319];
static NJS_CNK_MODEL draw_18_model;
static NJS_CNK_OBJECT draw_494;
static Sint16 draw_0_polygons[391];
static Sint32 draw_0_vertices[279];
static NJS_CNK_MODEL draw_0_model;
static NJS_CNK_OBJECT draw_488;
static Sint16 draw_39_polygons[173];
static Sint32 draw_39_vertices[139];
static NJS_CNK_MODEL draw_39_model;
static NJS_CNK_OBJECT draw_476;
static Sint16 draw_306_polygons[355];
static Sint32 draw_306_vertices[243];
static NJS_CNK_MODEL draw_306_model;
static NJS_CNK_OBJECT draw_468;
static Sint16 draw_6_polygons[169];
static Sint32 draw_6_vertices[155];
static NJS_CNK_MODEL draw_6_model;
static NJS_CNK_OBJECT draw_467;
static Sint16 draw_22_polygons[141];
static Sint32 draw_22_vertices[99];
static NJS_CNK_MODEL draw_22_model;
static NJS_CNK_OBJECT draw_402;
static Sint16 draw_20_polygons[155];
static Sint32 draw_20_vertices[115];
static NJS_CNK_MODEL draw_20_model;
static NJS_CNK_OBJECT draw_401;
static Sint16 draw_23_polygons[735];
static Sint32 draw_23_vertices[447];
static NJS_CNK_MODEL draw_23_model;
static NJS_CNK_OBJECT draw_391;
static Sint16 draw_25_polygons[515];
static Sint32 draw_25_vertices[383];
static NJS_CNK_MODEL draw_25_model;
static NJS_CNK_OBJECT draw_390;
static Sint16 draw_24_polygons[55];
static Sint32 draw_24_vertices[51];
static NJS_CNK_MODEL draw_24_model;
static NJS_CNK_OBJECT draw_389;
static Sint16 draw_377_polygons[45];
static Sint32 draw_377_vertices[31];
static NJS_CNK_MODEL draw_377_model;
static NJS_CNK_OBJECT draw_377;
static Sint16 draw_370_polygons[145];
static Sint32 draw_370_vertices[99];
static NJS_CNK_MODEL draw_370_model;
static NJS_CNK_OBJECT draw_370;
static Sint16 draw_305_polygons[91];
static Sint32 draw_305_vertices[83];
static NJS_CNK_MODEL draw_305_model;
static NJS_CNK_OBJECT draw_305;
static Sint16 draw_132_polygons[307];
static Sint32 draw_132_vertices[199];
static NJS_CNK_MODEL draw_132_model;
static NJS_CNK_OBJECT draw_275;
static Sint16 draw_149_polygons[519];
static Sint32 draw_149_vertices[419];
static NJS_CNK_MODEL draw_149_model;
static NJS_CNK_OBJECT draw_274;
static Sint16 draw_198_polygons[451];
static Sint32 draw_198_vertices[327];
static NJS_CNK_MODEL draw_198_model;
static NJS_CNK_OBJECT draw_268;
static Sint16 draw_137_polygons[59];
static Sint32 draw_137_vertices[43];
static NJS_CNK_MODEL draw_137_model;
static NJS_CNK_OBJECT draw_260;
static Sint16 draw_131_polygons[519];
static Sint32 draw_131_vertices[431];
static NJS_CNK_MODEL draw_131_model;
static NJS_CNK_OBJECT draw_248;
static Sint16 draw_143_polygons[59];
static Sint32 draw_143_vertices[43];
static NJS_CNK_MODEL draw_143_model;
static NJS_CNK_OBJECT draw_247;
static Sint16 draw_118_polygons[301];
static Sint32 draw_118_vertices[195];
static NJS_CNK_MODEL draw_118_model;
static NJS_CNK_OBJECT draw_225;
static Sint16 draw_185_polygons[511];
static Sint32 draw_185_vertices[423];
static NJS_CNK_MODEL draw_185_model;
static NJS_CNK_OBJECT draw_224;
static Sint16 draw_128_polygons[55];
static Sint32 draw_128_vertices[35];
static NJS_CNK_MODEL draw_128_model;
static NJS_CNK_OBJECT draw_223;
static Sint16 draw_216_polygons[77];
static Sint32 draw_216_vertices[63];
static NJS_CNK_MODEL draw_216_model;
static NJS_CNK_OBJECT draw_216;
static Sint16 draw_189_polygons[525];
static Sint32 draw_189_vertices[615];
static NJS_CNK_MODEL draw_189_model;
static NJS_CNK_OBJECT draw_189;
static Sint16 draw_124_polygons[21];
static Sint32 draw_124_vertices[51];
static NJS_CNK_MODEL draw_124_model;
static NJS_CNK_OBJECT draw_124;
static Sint16 draw_29_polygons[157];
static Sint32 draw_29_vertices[107];
static NJS_CNK_MODEL draw_29_model;
static NJS_CNK_OBJECT draw_29;
static Sint16 draw_28_polygons[133];
static Sint32 draw_28_vertices[83];
static NJS_CNK_MODEL draw_28_model;
static NJS_CNK_OBJECT draw_28;
static Sint16 draw_27_polygons[183];
static Sint32 draw_27_vertices[123];
static NJS_CNK_MODEL draw_27_model;
static NJS_CNK_OBJECT draw_27;
static Sint16 draw_26_polygons[173];
static Sint32 draw_26_vertices[83];
static NJS_CNK_MODEL draw_26_model;
static NJS_CNK_OBJECT draw_26;
static NJS_MATERIAL collision_0_materials[1];
static Sint16 collision_0_faces[28];
static NJS_MESHSET collision_0_meshsets[1];
static NJS_POINT3 collision_0_points[18];
static NJS_MODEL collision_0_model;
static NJS_OBJECT collision_0;
static NJS_MATERIAL collision_1_materials[1];
static Sint16 collision_1_faces[33];
static NJS_MESHSET collision_1_meshsets[1];
static NJS_POINT3 collision_1_points[18];
static NJS_MODEL collision_1_model;
static NJS_OBJECT collision_1;
static NJS_MATERIAL collision_2_materials[1];
static Sint16 collision_2_faces[46];
static NJS_MESHSET collision_2_meshsets[1];
static NJS_POINT3 collision_2_points[30];
static NJS_MODEL collision_2_model;
static NJS_OBJECT collision_2;
static NJS_MATERIAL collision_3_materials[1];
static Sint16 collision_3_faces[27];
static NJS_MESHSET collision_3_meshsets[1];
static NJS_POINT3 collision_3_points[16];
static NJS_MODEL collision_3_model;
static NJS_OBJECT collision_3;
static NJS_MATERIAL collision_7_materials[1];
static Sint16 collision_7_faces[46];
static NJS_MESHSET collision_7_meshsets[1];
static NJS_POINT3 collision_7_points[30];
static NJS_MODEL collision_7_model;
static NJS_OBJECT collision_7;
static NJS_MATERIAL collision_8_materials[1];
static Sint16 collision_8_faces[48];
static NJS_MESHSET collision_8_meshsets[1];
static NJS_POINT3 collision_8_points[30];
static NJS_MODEL collision_8_model;
static NJS_OBJECT collision_8;
static NJS_MATERIAL collision_9_materials[1];
static Sint16 collision_9_faces[46];
static NJS_MESHSET collision_9_meshsets[1];
static NJS_POINT3 collision_9_points[30];
static NJS_MODEL collision_9_model;
static NJS_OBJECT collision_9;
static NJS_MATERIAL collision_12_materials[1];
static Sint16 collision_12_faces[39];
static NJS_MESHSET collision_12_meshsets[1];
static NJS_POINT3 collision_12_points[30];
static NJS_MODEL collision_12_model;
static NJS_OBJECT collision_12;
static NJS_MATERIAL collision_13_materials[1];
static Sint16 collision_13_faces[39];
static NJS_MESHSET collision_13_meshsets[1];
static NJS_POINT3 collision_13_points[30];
static NJS_MODEL collision_13_model;
static NJS_OBJECT collision_13;
static NJS_MATERIAL collision_16_materials[1];
static Sint16 collision_16_faces[53];
static NJS_MESHSET collision_16_meshsets[1];
static NJS_POINT3 collision_16_points[24];
static NJS_MODEL collision_16_model;
static NJS_OBJECT collision_16;
static NJS_MATERIAL collision_23_materials[1];
static Sint16 collision_23_faces[152];
static NJS_MESHSET collision_23_meshsets[1];
static NJS_POINT3 collision_23_points[73];
static NJS_MODEL collision_23_model;
static NJS_OBJECT collision_23;
static NJS_MATERIAL collision_24_materials[1];
static Sint16 collision_24_faces[27];
static NJS_MESHSET collision_24_meshsets[1];
static NJS_POINT3 collision_24_points[16];
static NJS_MODEL collision_24_model;
static NJS_OBJECT collision_24;
static NJS_MATERIAL collision_28_materials[1];
static Sint16 collision_28_faces[27];
static NJS_MESHSET collision_28_meshsets[1];
static NJS_POINT3 collision_28_points[16];
static NJS_MODEL collision_28_model;
static NJS_OBJECT collision_28;
static NJS_MATERIAL collision_98_materials[1];
static Sint16 collision_98_faces[77];
static NJS_MESHSET collision_98_meshsets[1];
static NJS_POINT3 collision_98_points[34];
static NJS_MODEL collision_98_model;
static NJS_OBJECT collision_98;
static NJS_MATERIAL collision_100_materials[1];
static Sint16 collision_100_faces[22];
static NJS_MESHSET collision_100_meshsets[1];
static NJS_POINT3 collision_100_points[12];
static NJS_MODEL collision_100_model;
static NJS_OBJECT collision_100;
static NJS_MATERIAL collision_106_materials[1];
static Sint16 collision_106_faces[60];
static NJS_MESHSET collision_106_meshsets[1];
static NJS_POINT3 collision_106_points[32];
static NJS_MODEL collision_106_model;
static NJS_OBJECT collision_106;
static NJS_MATERIAL collision_108_materials[1];
static Sint16 collision_108_faces[27];
static NJS_MESHSET collision_108_meshsets[1];
static NJS_POINT3 collision_108_points[16];
static NJS_MODEL collision_108_model;
static NJS_OBJECT collision_108;
static NJS_MATERIAL collision_112_materials[1];
static Sint16 collision_112_faces[33];
static NJS_MESHSET collision_112_meshsets[1];
static NJS_POINT3 collision_112_points[18];
static NJS_MODEL collision_112_model;
static NJS_OBJECT collision_112;
static NJS_MATERIAL collision_113_materials[1];
static Sint16 collision_113_faces[55];
static NJS_MESHSET collision_113_meshsets[1];
static NJS_POINT3 collision_113_points[30];
static NJS_MODEL collision_113_model;
static NJS_OBJECT collision_113;
static NJS_MATERIAL collision_114_materials[1];
static Sint16 collision_114_faces[236];
static NJS_MESHSET collision_114_meshsets[1];
static NJS_POINT3 collision_114_points[120];
static NJS_MODEL collision_114_model;
static NJS_OBJECT collision_114;
static NJS_MATERIAL collision_115_materials[1];
static Sint16 collision_115_faces[46];
static NJS_MESHSET collision_115_meshsets[1];
static NJS_POINT3 collision_115_points[19];
static NJS_MODEL collision_115_model;
static NJS_OBJECT collision_115;
static NJS_MATERIAL collision_117_materials[1];
static Sint16 collision_117_faces[51];
static NJS_MESHSET collision_117_meshsets[1];
static NJS_POINT3 collision_117_points[26];
static NJS_MODEL collision_117_model;
static NJS_OBJECT collision_117;
static NJS_MATERIAL collision_118_materials[1];
static Sint16 collision_118_faces[215];
static NJS_MESHSET collision_118_meshsets[1];
static NJS_POINT3 collision_118_points[120];
static NJS_MODEL collision_118_model;
static NJS_OBJECT collision_118;
static NJS_MATERIAL collision_119_materials[1];
static Sint16 collision_119_faces[63];
static NJS_MESHSET collision_119_meshsets[1];
static NJS_POINT3 collision_119_points[32];
static NJS_MODEL collision_119_model;
static NJS_OBJECT collision_119;
static NJS_MATERIAL collision_120_materials[1];
static Sint16 collision_120_faces[129];
static NJS_MESHSET collision_120_meshsets[1];
static NJS_POINT3 collision_120_points[80];
static NJS_MODEL collision_120_model;
static NJS_OBJECT collision_120;
static NJS_MATERIAL collision_121_materials[1];
static Sint16 collision_121_faces[33];
static NJS_MESHSET collision_121_meshsets[1];
static NJS_POINT3 collision_121_points[20];
static NJS_MODEL collision_121_model;
static NJS_OBJECT collision_121;
static NJS_MATERIAL collision_125_materials[1];
static Sint16 collision_125_faces[36];
static NJS_MESHSET collision_125_meshsets[1];
static NJS_POINT3 collision_125_points[20];
static NJS_MODEL collision_125_model;
static NJS_OBJECT collision_125;
static NJS_MATERIAL collision_130_materials[1];
static Sint16 collision_130_faces[263];
static NJS_MESHSET collision_130_meshsets[1];
static NJS_POINT3 collision_130_points[120];
static NJS_MODEL collision_130_model;
static NJS_OBJECT collision_130;
static NJS_MATERIAL collision_131_materials[1];
static Sint16 collision_131_faces[45];
static NJS_MESHSET collision_131_meshsets[1];
static NJS_POINT3 collision_131_points[24];
static NJS_MODEL collision_131_model;
static NJS_OBJECT collision_131;
static NJS_MATERIAL collision_133_materials[1];
static Sint16 collision_133_faces[13];
static NJS_MESHSET collision_133_meshsets[1];
static NJS_POINT3 collision_133_points[12];
static NJS_MODEL collision_133_model;
static NJS_OBJECT collision_133;
static NJS_MATERIAL collision_135_materials[1];
static Sint16 collision_135_faces[64];
static NJS_MESHSET collision_135_meshsets[1];
static NJS_POINT3 collision_135_points[33];
static NJS_MODEL collision_135_model;
static NJS_OBJECT collision_135;
static NJS_MATERIAL collision_136_materials[1];
static Sint16 collision_136_faces[81];
static NJS_MESHSET collision_136_meshsets[1];
static NJS_POINT3 collision_136_points[48];
static NJS_MODEL collision_136_model;
static NJS_OBJECT collision_136;
static NJS_MATERIAL collision_139_materials[1];
static Sint16 collision_139_faces[91];
static NJS_MESHSET collision_139_meshsets[1];
static NJS_POINT3 collision_139_points[40];
static NJS_MODEL collision_139_model;
static NJS_OBJECT collision_139;
static NJS_MATERIAL collision_140_materials[1];
static Sint16 collision_140_faces[98];
static NJS_MESHSET collision_140_meshsets[1];
static NJS_POINT3 collision_140_points[52];
static NJS_MODEL collision_140_model;
static NJS_OBJECT collision_140;
static NJS_MATERIAL collision_141_materials[1];
static Sint16 collision_141_faces[36];
static NJS_MESHSET collision_141_meshsets[1];
static NJS_POINT3 collision_141_points[20];
static NJS_MODEL collision_141_model;
static NJS_OBJECT collision_141;
static NJS_MATERIAL collision_143_materials[1];
static Sint16 collision_143_faces[125];
static NJS_MESHSET collision_143_meshsets[1];
static NJS_POINT3 collision_143_points[61];
static NJS_MODEL collision_143_model;
static NJS_OBJECT collision_143;
static NJS_MATERIAL collision_144_materials[1];
static Sint16 collision_144_faces[67];
static NJS_MESHSET collision_144_meshsets[1];
static NJS_POINT3 collision_144_points[33];
static NJS_MODEL collision_144_model;
static NJS_OBJECT collision_144;
static NJS_MATERIAL collision_145_materials[1];
static Sint16 collision_145_faces[146];
static NJS_MESHSET collision_145_meshsets[1];
static NJS_POINT3 collision_145_points[77];
static NJS_MODEL collision_145_model;
static NJS_OBJECT collision_145;
static NJS_MATERIAL collision_146_materials[1];
static Sint16 collision_146_faces[64];
static NJS_MESHSET collision_146_meshsets[1];
static NJS_POINT3 collision_146_points[33];
static NJS_MODEL collision_146_model;
static NJS_OBJECT collision_146;
static NJS_MATERIAL collision_147_materials[1];
static Sint16 collision_147_faces[68];
static NJS_MESHSET collision_147_meshsets[1];
static NJS_POINT3 collision_147_points[42];
static NJS_MODEL collision_147_model;
static NJS_OBJECT collision_147;
static NJS_MATERIAL collision_148_materials[1];
static Sint16 collision_148_faces[88];
static NJS_MESHSET collision_148_meshsets[1];
static NJS_POINT3 collision_148_points[52];
static NJS_MODEL collision_148_model;
static NJS_OBJECT collision_148;
static NJS_MATERIAL collision_149_materials[1];
static Sint16 collision_149_faces[185];
static NJS_MESHSET collision_149_meshsets[1];
static NJS_POINT3 collision_149_points[81];
static NJS_MODEL collision_149_model;
static NJS_OBJECT collision_149;
static NJS_MATERIAL collision_150_materials[1];
static Sint16 collision_150_faces[145];
static NJS_MESHSET collision_150_meshsets[1];
static NJS_POINT3 collision_150_points[71];
static NJS_MODEL collision_150_model;
static NJS_OBJECT collision_150;
static NJS_MATERIAL collision_151_materials[1];
static Sint16 collision_151_faces[122];
static NJS_MESHSET collision_151_meshsets[1];
static NJS_POINT3 collision_151_points[59];
static NJS_MODEL collision_151_model;
static NJS_OBJECT collision_151;
static NJS_MATERIAL collision_152_materials[1];
static Sint16 collision_152_faces[82];
static NJS_MESHSET collision_152_meshsets[1];
static NJS_POINT3 collision_152_points[45];
static NJS_MODEL collision_152_model;
static NJS_OBJECT collision_152;
static NJS_MATERIAL collision_153_materials[1];
static Sint16 collision_153_faces[132];
static NJS_MESHSET collision_153_meshsets[1];
static NJS_POINT3 collision_153_points[62];
static NJS_MODEL collision_153_model;
static NJS_OBJECT collision_153;
static NJS_MATERIAL collision_154_materials[1];
static Sint16 collision_154_faces[63];
static NJS_MESHSET collision_154_meshsets[1];
static NJS_POINT3 collision_154_points[36];
static NJS_MODEL collision_154_model;
static NJS_OBJECT collision_154;
static NJS_MATERIAL collision_155_materials[1];
static Sint16 collision_155_faces[181];
static NJS_MESHSET collision_155_meshsets[1];
static NJS_POINT3 collision_155_points[80];
static NJS_MODEL collision_155_model;
static NJS_OBJECT collision_155;
static NJS_MATERIAL collision_156_materials[1];
static Sint16 collision_156_faces[166];
static NJS_MESHSET collision_156_meshsets[1];
static NJS_POINT3 collision_156_points[75];
static NJS_MODEL collision_156_model;
static NJS_OBJECT collision_156;
static NJS_MATERIAL collision_157_materials[1];
static Sint16 collision_157_faces[67];
static NJS_MESHSET collision_157_meshsets[1];
static NJS_POINT3 collision_157_points[41];
static NJS_MODEL collision_157_model;
static NJS_OBJECT collision_157;
static NJS_MATERIAL collision_158_materials[1];
static Sint16 collision_158_faces[90];
static NJS_MESHSET collision_158_meshsets[1];
static NJS_POINT3 collision_158_points[48];
static NJS_MODEL collision_158_model;
static NJS_OBJECT collision_158;
static NJS_MATERIAL collision_159_materials[1];
static Sint16 collision_159_faces[154];
static NJS_MESHSET collision_159_meshsets[1];
static NJS_POINT3 collision_159_points[68];
static NJS_MODEL collision_159_model;
static NJS_OBJECT collision_159;
static NJS_MATERIAL collision_160_materials[1];
static Sint16 collision_160_faces[188];
static NJS_MESHSET collision_160_meshsets[1];
static NJS_POINT3 collision_160_points[83];
static NJS_MODEL collision_160_model;
static NJS_OBJECT collision_160;
static NJS_MATERIAL collision_161_materials[1];
static Sint16 collision_161_faces[72];
static NJS_MESHSET collision_161_meshsets[1];
static NJS_POINT3 collision_161_points[40];
static NJS_MODEL collision_161_model;
static NJS_OBJECT collision_161;
static NJS_MATERIAL collision_162_materials[1];
static Sint16 collision_162_faces[107];
static NJS_MESHSET collision_162_meshsets[1];
static NJS_POINT3 collision_162_points[58];
static NJS_MODEL collision_162_model;
static NJS_OBJECT collision_162;
static NJS_MATERIAL collision_163_materials[1];
static Sint16 collision_163_faces[64];
static NJS_MESHSET collision_163_meshsets[1];
static NJS_POINT3 collision_163_points[33];
static NJS_MODEL collision_163_model;
static NJS_OBJECT collision_163;
static NJS_MATERIAL collision_164_materials[1];
static Sint16 collision_164_faces[72];
static NJS_MESHSET collision_164_meshsets[1];
static NJS_POINT3 collision_164_points[40];
static NJS_MODEL collision_164_model;
static NJS_OBJECT collision_164;
static NJS_MATERIAL collision_165_materials[1];
static Sint16 collision_165_faces[44];
static NJS_MESHSET collision_165_meshsets[1];
static NJS_POINT3 collision_165_points[25];
static NJS_MODEL collision_165_model;
static NJS_OBJECT collision_165;
static NJS_MATERIAL collision_166_materials[1];
static Sint16 collision_166_faces[135];
static NJS_MESHSET collision_166_meshsets[1];
static NJS_POINT3 collision_166_points[59];
static NJS_MODEL collision_166_model;
static NJS_OBJECT collision_166;
static NJS_MATERIAL collision_167_materials[1];
static Sint16 collision_167_faces[155];
static NJS_MESHSET collision_167_meshsets[1];
static NJS_POINT3 collision_167_points[69];
static NJS_MODEL collision_167_model;
static NJS_OBJECT collision_167;
static NJS_MATERIAL collision_168_materials[1];
static Sint16 collision_168_faces[71];
static NJS_MESHSET collision_168_meshsets[1];
static NJS_POINT3 collision_168_points[34];
static NJS_MODEL collision_168_model;
static NJS_OBJECT collision_168;
static NJS_MATERIAL collision_169_materials[1];
static Sint16 collision_169_faces[50];
static NJS_MESHSET collision_169_meshsets[1];
static NJS_POINT3 collision_169_points[25];
static NJS_MODEL collision_169_model;
static NJS_OBJECT collision_169;
static NJS_MATERIAL collision_170_materials[1];
static Sint16 collision_170_faces[137];
static NJS_MESHSET collision_170_meshsets[1];
static NJS_POINT3 collision_170_points[65];
static NJS_MODEL collision_170_model;
static NJS_OBJECT collision_170;
static NJS_MATERIAL collision_171_materials[1];
static Sint16 collision_171_faces[154];
static NJS_MESHSET collision_171_meshsets[1];
static NJS_POINT3 collision_171_points[68];
static NJS_MODEL collision_171_model;
static NJS_OBJECT collision_171;
static NJS_MATERIAL collision_172_materials[1];
static Sint16 collision_172_faces[162];
static NJS_MESHSET collision_172_meshsets[1];
static NJS_POINT3 collision_172_points[77];
static NJS_MODEL collision_172_model;
static NJS_OBJECT collision_172;
static NJS_MATERIAL collision_173_materials[1];
static Sint16 collision_173_faces[67];
static NJS_MESHSET collision_173_meshsets[1];
static NJS_POINT3 collision_173_points[33];
static NJS_MODEL collision_173_model;
static NJS_OBJECT collision_173;
static NJS_MATERIAL collision_174_materials[1];
static Sint16 collision_174_faces[64];
static NJS_MESHSET collision_174_meshsets[1];
static NJS_POINT3 collision_174_points[33];
static NJS_MODEL collision_174_model;
static NJS_OBJECT collision_174;
static NJS_MATERIAL collision_175_materials[1];
static Sint16 collision_175_faces[145];
static NJS_MESHSET collision_175_meshsets[1];
static NJS_POINT3 collision_175_points[66];
static NJS_MODEL collision_175_model;
static NJS_OBJECT collision_175;
static NJS_MATERIAL collision_176_materials[1];
static Sint16 collision_176_faces[67];
static NJS_MESHSET collision_176_meshsets[1];
static NJS_POINT3 collision_176_points[33];
static NJS_MODEL collision_176_model;
static NJS_OBJECT collision_176;
static NJS_MATERIAL collision_177_materials[1];
static Sint16 collision_177_faces[63];
static NJS_MESHSET collision_177_meshsets[1];
static NJS_POINT3 collision_177_points[36];
static NJS_MODEL collision_177_model;
static NJS_OBJECT collision_177;
static NJS_MATERIAL collision_178_materials[1];
static Sint16 collision_178_faces[63];
static NJS_MESHSET collision_178_meshsets[1];
static NJS_POINT3 collision_178_points[36];
static NJS_MODEL collision_178_model;
static NJS_OBJECT collision_178;
static NJS_MATERIAL collision_181_materials[1];
static Sint16 collision_181_faces[67];
static NJS_MESHSET collision_181_meshsets[1];
static NJS_POINT3 collision_181_points[33];
static NJS_MODEL collision_181_model;
static NJS_OBJECT collision_181;
static NJS_MATERIAL collision_182_materials[1];
static Sint16 collision_182_faces[67];
static NJS_MESHSET collision_182_meshsets[1];
static NJS_POINT3 collision_182_points[33];
static NJS_MODEL collision_182_model;
static NJS_OBJECT collision_182;
static NJS_MATERIAL collision_183_materials[1];
static Sint16 collision_183_faces[36];
static NJS_MESHSET collision_183_meshsets[1];
static NJS_POINT3 collision_183_points[20];
static NJS_MODEL collision_183_model;
static NJS_OBJECT collision_183;
static NJS_MATERIAL collision_184_materials[1];
static Sint16 collision_184_faces[36];
static NJS_MESHSET collision_184_meshsets[1];
static NJS_POINT3 collision_184_points[20];
static NJS_MODEL collision_184_model;
static NJS_OBJECT collision_184;
static NJS_MATERIAL collision_185_materials[1];
static Sint16 collision_185_faces[106];
static NJS_MESHSET collision_185_meshsets[1];
static NJS_POINT3 collision_185_points[59];
static NJS_MODEL collision_185_model;
static NJS_OBJECT collision_185;
static NJS_MATERIAL collision_186_materials[1];
static Sint16 collision_186_faces[116];
static NJS_MESHSET collision_186_meshsets[1];
static NJS_POINT3 collision_186_points[57];
static NJS_MODEL collision_186_model;
static NJS_OBJECT collision_186;
static NJS_MATERIAL collision_187_materials[1];
static Sint16 collision_187_faces[57];
static NJS_MESHSET collision_187_meshsets[1];
static NJS_POINT3 collision_187_points[26];
static NJS_MODEL collision_187_model;
static NJS_OBJECT collision_187;
static NJS_MATERIAL collision_189_materials[1];
static Sint16 collision_189_faces[72];
static NJS_MESHSET collision_189_meshsets[1];
static NJS_POINT3 collision_189_points[33];
static NJS_MODEL collision_189_model;
static NJS_OBJECT collision_189;
static NJS_MATERIAL collision_213_materials[1];
static Sint16 collision_213_faces[97];
static NJS_MESHSET collision_213_meshsets[1];
static NJS_POINT3 collision_213_points[42];
static NJS_MODEL collision_213_model;
static NJS_OBJECT collision_213;
static NJS_MATERIAL collision_222_materials[1];
static Sint16 collision_222_faces[83];
static NJS_MESHSET collision_222_meshsets[1];
static NJS_POINT3 collision_222_points[40];
static NJS_MODEL collision_222_model;
static NJS_OBJECT collision_222;
static NJS_MATERIAL collision_224_materials[1];
static Sint16 collision_224_faces[47];
static NJS_MESHSET collision_224_meshsets[1];
static NJS_POINT3 collision_224_points[25];
static NJS_MODEL collision_224_model;
static NJS_OBJECT collision_224;
static NJS_MATERIAL collision_225_materials[1];
static Sint16 collision_225_faces[50];
static NJS_MESHSET collision_225_meshsets[1];
static NJS_POINT3 collision_225_points[25];
static NJS_MODEL collision_225_model;
static NJS_OBJECT collision_225;
static NJS_MATERIAL collision_227_materials[1];
static Sint16 collision_227_faces[63];
static NJS_MESHSET collision_227_meshsets[1];
static NJS_POINT3 collision_227_points[35];
static NJS_MODEL collision_227_model;
static NJS_OBJECT collision_227;
static NJS_MATERIAL collision_229_materials[1];
static Sint16 collision_229_faces[34];
static NJS_MESHSET collision_229_meshsets[1];
static NJS_POINT3 collision_229_points[16];
static NJS_MODEL collision_229_model;
static NJS_OBJECT collision_229;
static NJS_MATERIAL collision_232_materials[1];
static Sint16 collision_232_faces[28];
static NJS_MESHSET collision_232_meshsets[1];
static NJS_POINT3 collision_232_points[18];
static NJS_MODEL collision_232_model;
static NJS_OBJECT collision_232;
static NJS_MATERIAL collision_234_materials[1];
static Sint16 collision_234_faces[40];
static NJS_MESHSET collision_234_meshsets[1];
static NJS_POINT3 collision_234_points[24];
static NJS_MODEL collision_234_model;
static NJS_OBJECT collision_234;
static NJS_MATERIAL collision_236_materials[1];
static Sint16 collision_236_faces[59];
static NJS_MESHSET collision_236_meshsets[1];
static NJS_POINT3 collision_236_points[24];
static NJS_MODEL collision_236_model;
static NJS_OBJECT collision_236;
static NJS_MATERIAL collision_237_materials[1];
static Sint16 collision_237_faces[59];
static NJS_MESHSET collision_237_meshsets[1];
static NJS_POINT3 collision_237_points[24];
static NJS_MODEL collision_237_model;
static NJS_OBJECT collision_237;
static NJS_MATERIAL collision_274_materials[1];
static Sint16 collision_274_faces[28];
static NJS_MESHSET collision_274_meshsets[1];
static NJS_POINT3 collision_274_points[18];
static NJS_MODEL collision_274_model;
static NJS_OBJECT collision_274;
static NJS_MATERIAL collision_275_materials[1];
static Sint16 collision_275_faces[40];
static NJS_MESHSET collision_275_meshsets[1];
static NJS_POINT3 collision_275_points[24];
static NJS_MODEL collision_275_model;
static NJS_OBJECT collision_275;
static NJS_MATERIAL collision_276_materials[1];
static Sint16 collision_276_faces[52];
static NJS_MESHSET collision_276_meshsets[1];
static NJS_POINT3 collision_276_points[24];
static NJS_MODEL collision_276_model;
static NJS_OBJECT collision_276;
static NJS_MATERIAL collision_277_materials[1];
static Sint16 collision_277_faces[43];
static NJS_MESHSET collision_277_meshsets[1];
static NJS_POINT3 collision_277_points[24];
static NJS_MODEL collision_277_model;
static NJS_OBJECT collision_277;
static NJS_MATERIAL collision_278_materials[1];
static Sint16 collision_278_faces[29];
static NJS_MESHSET collision_278_meshsets[1];
static NJS_POINT3 collision_278_points[16];
static NJS_MODEL collision_278_model;
static NJS_OBJECT collision_278;
static NJS_MATERIAL collision_280_materials[1];
static Sint16 collision_280_faces[43];
static NJS_MESHSET collision_280_meshsets[1];
static NJS_POINT3 collision_280_points[24];
static NJS_MODEL collision_280_model;
static NJS_OBJECT collision_280;
static NJS_MATERIAL collision_281_materials[1];
static Sint16 collision_281_faces[142];
static NJS_MESHSET collision_281_meshsets[1];
static NJS_POINT3 collision_281_points[52];
static NJS_MODEL collision_281_model;
static NJS_OBJECT collision_281;
static NJS_MATERIAL collision_285_materials[1];
static Sint16 collision_285_faces[30];
static NJS_MESHSET collision_285_meshsets[1];
static NJS_POINT3 collision_285_points[16];
static NJS_MODEL collision_285_model;
static NJS_OBJECT collision_285;
static NJS_MATERIAL collision_287_materials[1];
static Sint16 collision_287_faces[9];
static NJS_MESHSET collision_287_meshsets[1];
static NJS_POINT3 collision_287_points[8];
static NJS_MODEL collision_287_model;
static NJS_OBJECT collision_287;
static NJS_MATERIAL collision_289_materials[1];
static Sint16 collision_289_faces[168];
static NJS_MESHSET collision_289_meshsets[1];
static NJS_POINT3 collision_289_points[81];
static NJS_MODEL collision_289_model;
static NJS_OBJECT collision_289;
static NJS_MATERIAL collision_290_materials[1];
static Sint16 collision_290_faces[46];
static NJS_MESHSET collision_290_meshsets[1];
static NJS_POINT3 collision_290_points[26];
static NJS_MODEL collision_290_model;
static NJS_OBJECT collision_290;
static NJS_MATERIAL collision_292_materials[1];
static Sint16 collision_292_faces[25];
static NJS_MESHSET collision_292_meshsets[1];
static NJS_POINT3 collision_292_points[10];
static NJS_MODEL collision_292_model;
static NJS_OBJECT collision_292;
static NJS_MATERIAL collision_364_materials[1];
static Sint16 collision_364_faces[7];
static NJS_MESHSET collision_364_meshsets[1];
static NJS_POINT3 collision_364_points[6];
static NJS_MODEL collision_364_model;
static NJS_OBJECT collision_364;
static NJS_MATERIAL collision_389_materials[1];
static Sint16 collision_389_faces[28];
static NJS_MESHSET collision_389_meshsets[1];
static NJS_POINT3 collision_389_points[18];
static NJS_MODEL collision_389_model;
static NJS_OBJECT collision_389;
static NJS_MATERIAL collision_391_materials[1];
static Sint16 collision_391_faces[40];
static NJS_MESHSET collision_391_meshsets[1];
static NJS_POINT3 collision_391_points[24];
static NJS_MODEL collision_391_model;
static NJS_OBJECT collision_391;
static NJS_CNK_OBJECT draw_534;
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
static NJS_CNK_OBJECT draw_493;
static NJS_CNK_OBJECT draw_492;
static NJS_CNK_OBJECT draw_491;
static NJS_CNK_OBJECT draw_490;
static NJS_CNK_OBJECT draw_489;
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
static NJS_CNK_OBJECT draw_475;
static NJS_CNK_OBJECT draw_474;
static NJS_CNK_OBJECT draw_473;
static NJS_CNK_OBJECT draw_472;
static NJS_CNK_OBJECT draw_471;
static NJS_CNK_OBJECT draw_470;
static NJS_CNK_OBJECT draw_469;
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
static NJS_CNK_OBJECT draw_400;
static NJS_CNK_OBJECT draw_399;
static NJS_CNK_OBJECT draw_398;
static NJS_CNK_OBJECT draw_397;
static NJS_CNK_OBJECT draw_396;
static NJS_CNK_OBJECT draw_395;
static NJS_CNK_OBJECT draw_394;
static NJS_CNK_OBJECT draw_393;
static NJS_CNK_OBJECT draw_392;
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
static NJS_CNK_OBJECT draw_376;
static NJS_CNK_OBJECT draw_375;
static NJS_CNK_OBJECT draw_374;
static NJS_CNK_OBJECT draw_373;
static NJS_CNK_OBJECT draw_372;
static NJS_CNK_OBJECT draw_371;
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
static NJS_CNK_OBJECT draw_273;
static NJS_CNK_OBJECT draw_272;
static NJS_CNK_OBJECT draw_271;
static NJS_CNK_OBJECT draw_270;
static NJS_CNK_OBJECT draw_269;
static NJS_CNK_OBJECT draw_267;
static NJS_CNK_OBJECT draw_266;
static NJS_CNK_OBJECT draw_265;
static NJS_CNK_OBJECT draw_264;
static NJS_CNK_OBJECT draw_263;
static NJS_CNK_OBJECT draw_262;
static NJS_CNK_OBJECT draw_261;
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
static NJS_CNK_OBJECT draw_222;
static NJS_CNK_OBJECT draw_221;
static NJS_CNK_OBJECT draw_220;
static NJS_CNK_OBJECT draw_219;
static NJS_CNK_OBJECT draw_218;
static NJS_CNK_OBJECT draw_217;
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
static NJS_CNK_OBJECT draw_194;
static NJS_CNK_OBJECT draw_193;
static NJS_CNK_OBJECT draw_192;
static NJS_CNK_OBJECT draw_191;
static NJS_CNK_OBJECT draw_190;
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
static NJS_CNK_OBJECT draw_133;
static NJS_CNK_OBJECT draw_132;
static NJS_CNK_OBJECT draw_131;
static NJS_CNK_OBJECT draw_130;
static NJS_CNK_OBJECT draw_129;
static NJS_CNK_OBJECT draw_128;
static NJS_CNK_OBJECT draw_127;
static NJS_CNK_OBJECT draw_126;
static NJS_CNK_OBJECT draw_125;
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
static NJS_CNK_OBJECT draw_38;
static NJS_CNK_OBJECT draw_37;
static NJS_CNK_OBJECT draw_36;
static NJS_CNK_OBJECT draw_35;
static NJS_CNK_OBJECT draw_34;
static NJS_CNK_OBJECT draw_33;
static NJS_CNK_OBJECT draw_32;
static NJS_CNK_OBJECT draw_31;
static NJS_CNK_OBJECT draw_30;
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
static NJS_CNK_OBJECT draw_3;
static NJS_CNK_OBJECT draw_2;
static NJS_CNK_OBJECT draw_1;
static NJS_CNK_OBJECT draw_0;
static NJS_OBJECT collision_4;
static NJS_OBJECT collision_5;
static NJS_OBJECT collision_6;
static NJS_OBJECT collision_10;
static NJS_OBJECT collision_11;
static NJS_OBJECT collision_14;
static NJS_OBJECT collision_15;
static NJS_OBJECT collision_17;
static NJS_OBJECT collision_18;
static NJS_OBJECT collision_19;
static NJS_OBJECT collision_20;
static NJS_OBJECT collision_21;
static NJS_OBJECT collision_22;
static NJS_OBJECT collision_25;
static NJS_OBJECT collision_26;
static NJS_OBJECT collision_27;
static NJS_OBJECT collision_29;
static NJS_OBJECT collision_30;
static NJS_OBJECT collision_31;
static NJS_OBJECT collision_32;
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
static NJS_OBJECT collision_99;
static NJS_OBJECT collision_101;
static NJS_OBJECT collision_102;
static NJS_OBJECT collision_103;
static NJS_OBJECT collision_104;
static NJS_OBJECT collision_105;
static NJS_OBJECT collision_107;
static NJS_OBJECT collision_109;
static NJS_OBJECT collision_110;
static NJS_OBJECT collision_111;
static NJS_OBJECT collision_116;
static NJS_OBJECT collision_122;
static NJS_OBJECT collision_123;
static NJS_OBJECT collision_124;
static NJS_OBJECT collision_126;
static NJS_OBJECT collision_127;
static NJS_OBJECT collision_128;
static NJS_OBJECT collision_129;
static NJS_OBJECT collision_132;
static NJS_OBJECT collision_134;
static NJS_OBJECT collision_137;
static NJS_OBJECT collision_138;
static NJS_OBJECT collision_142;
static NJS_OBJECT collision_179;
static NJS_OBJECT collision_180;
static NJS_OBJECT collision_188;
static NJS_OBJECT collision_190;
static NJS_OBJECT collision_191;
static NJS_OBJECT collision_192;
static NJS_OBJECT collision_193;
static NJS_OBJECT collision_194;
static NJS_OBJECT collision_195;
static NJS_OBJECT collision_196;
static NJS_OBJECT collision_197;
static NJS_OBJECT collision_198;
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
static NJS_OBJECT collision_214;
static NJS_OBJECT collision_215;
static NJS_OBJECT collision_216;
static NJS_OBJECT collision_217;
static NJS_OBJECT collision_218;
static NJS_OBJECT collision_219;
static NJS_OBJECT collision_220;
static NJS_OBJECT collision_221;
static NJS_OBJECT collision_223;
static NJS_OBJECT collision_226;
static NJS_OBJECT collision_228;
static NJS_OBJECT collision_230;
static NJS_OBJECT collision_231;
static NJS_OBJECT collision_233;
static NJS_OBJECT collision_235;
static NJS_OBJECT collision_238;
static NJS_OBJECT collision_239;
static NJS_OBJECT collision_240;
static NJS_OBJECT collision_241;
static NJS_OBJECT collision_242;
static NJS_OBJECT collision_243;
static NJS_OBJECT collision_244;
static NJS_OBJECT collision_245;
static NJS_OBJECT collision_246;
static NJS_OBJECT collision_247;
static NJS_OBJECT collision_248;
static NJS_OBJECT collision_249;
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
static NJS_OBJECT collision_265;
static NJS_OBJECT collision_266;
static NJS_OBJECT collision_267;
static NJS_OBJECT collision_268;
static NJS_OBJECT collision_269;
static NJS_OBJECT collision_270;
static NJS_OBJECT collision_271;
static NJS_OBJECT collision_272;
static NJS_OBJECT collision_273;
static NJS_OBJECT collision_279;
static NJS_OBJECT collision_282;
static NJS_OBJECT collision_283;
static NJS_OBJECT collision_284;
static NJS_OBJECT collision_286;
static NJS_OBJECT collision_288;
static NJS_OBJECT collision_291;
static NJS_OBJECT collision_293;
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
static NJS_OBJECT collision_304;
static NJS_OBJECT collision_305;
static NJS_OBJECT collision_306;
static NJS_OBJECT collision_307;
static NJS_OBJECT collision_308;
static NJS_OBJECT collision_309;
static NJS_OBJECT collision_310;
static NJS_OBJECT collision_311;
static NJS_OBJECT collision_312;
static NJS_OBJECT collision_313;
static NJS_OBJECT collision_314;
static NJS_OBJECT collision_315;
static NJS_OBJECT collision_316;
static NJS_OBJECT collision_317;
static NJS_OBJECT collision_318;
static NJS_OBJECT collision_319;
static NJS_OBJECT collision_320;
static NJS_OBJECT collision_321;
static NJS_OBJECT collision_322;
static NJS_OBJECT collision_323;
static NJS_OBJECT collision_324;
static NJS_OBJECT collision_325;
static NJS_OBJECT collision_326;
static NJS_OBJECT collision_327;
static NJS_OBJECT collision_328;
static NJS_OBJECT collision_329;
static NJS_OBJECT collision_330;
static NJS_OBJECT collision_331;
static NJS_OBJECT collision_332;
static NJS_OBJECT collision_333;
static NJS_OBJECT collision_334;
static NJS_OBJECT collision_335;
static NJS_OBJECT collision_336;
static NJS_OBJECT collision_337;
static NJS_OBJECT collision_338;
static NJS_OBJECT collision_339;
static NJS_OBJECT collision_340;
static NJS_OBJECT collision_341;
static NJS_OBJECT collision_342;
static NJS_OBJECT collision_343;
static NJS_OBJECT collision_344;
static NJS_OBJECT collision_345;
static NJS_OBJECT collision_346;
static NJS_OBJECT collision_347;
static NJS_OBJECT collision_348;
static NJS_OBJECT collision_349;
static NJS_OBJECT collision_350;
static NJS_OBJECT collision_351;
static NJS_OBJECT collision_352;
static NJS_OBJECT collision_353;
static NJS_OBJECT collision_354;
static NJS_OBJECT collision_355;
static NJS_OBJECT collision_356;
static NJS_OBJECT collision_357;
static NJS_OBJECT collision_358;
static NJS_OBJECT collision_359;
static NJS_OBJECT collision_360;
static NJS_OBJECT collision_361;
static NJS_OBJECT collision_362;
static NJS_OBJECT collision_363;
static NJS_OBJECT collision_365;
static NJS_OBJECT collision_366;
static NJS_OBJECT collision_367;
static NJS_OBJECT collision_368;
static NJS_OBJECT collision_369;
static NJS_OBJECT collision_370;
static NJS_OBJECT collision_371;
static NJS_OBJECT collision_372;
static NJS_OBJECT collision_373;
static NJS_OBJECT collision_374;
static NJS_OBJECT collision_375;
static NJS_OBJECT collision_376;
static NJS_OBJECT collision_377;
static NJS_OBJECT collision_378;
static NJS_OBJECT collision_379;
static NJS_OBJECT collision_380;
static NJS_OBJECT collision_381;
static NJS_OBJECT collision_382;
static NJS_OBJECT collision_383;
static NJS_OBJECT collision_384;
static NJS_OBJECT collision_385;
static NJS_OBJECT collision_386;
static NJS_OBJECT collision_387;
static NJS_OBJECT collision_388;
static NJS_OBJECT collision_390;
static NJS_OBJECT collision_392;

static struct land_entry entries[928] = {
#include "stg40/terrain/entries.inc"
};
struct land_table final_chase_land = {
#include "stg40/terrain/final_chase_land.inc"
};
static NJS_TEXNAME textures[34] = {
#include "stg40/terrain/textures.inc"
};
NJS_TEXLIST final_chase_land_texlist = {
#include "stg40/terrain/final_chase_land_texlist.inc"
};
static Sint16 draw_1_polygons[271] = {
#include "stg40/terrain/draw_1_polygons.inc"
};
static Sint32 draw_1_vertices[279] = {
#include "stg40/terrain/draw_1_vertices.inc"
};
static NJS_CNK_MODEL draw_1_model = {
#include "stg40/terrain/draw_1_model.inc"
};
static NJS_CNK_OBJECT draw_533 = {
#include "stg40/terrain/draw_533.inc"
};
static Sint16 draw_2_polygons[271] = {
#include "stg40/terrain/draw_2_polygons.inc"
};
static Sint32 draw_2_vertices[303] = {
#include "stg40/terrain/draw_2_vertices.inc"
};
static NJS_CNK_MODEL draw_2_model = {
#include "stg40/terrain/draw_2_model.inc"
};
static NJS_CNK_OBJECT draw_532 = {
#include "stg40/terrain/draw_532.inc"
};
static Sint16 draw_19_polygons[555] = {
#include "stg40/terrain/draw_19_polygons.inc"
};
static Sint32 draw_19_vertices[379] = {
#include "stg40/terrain/draw_19_vertices.inc"
};
static NJS_CNK_MODEL draw_19_model = {
#include "stg40/terrain/draw_19_model.inc"
};
static NJS_CNK_OBJECT draw_508 = {
#include "stg40/terrain/draw_508.inc"
};
static Sint16 draw_164_polygons[551] = {
#include "stg40/terrain/draw_164_polygons.inc"
};
static Sint32 draw_164_vertices[415] = {
#include "stg40/terrain/draw_164_vertices.inc"
};
static NJS_CNK_MODEL draw_164_model = {
#include "stg40/terrain/draw_164_model.inc"
};
static NJS_CNK_OBJECT draw_496 = {
#include "stg40/terrain/draw_496.inc"
};
static Sint16 draw_14_polygons[387] = {
#include "stg40/terrain/draw_14_polygons.inc"
};
static Sint32 draw_14_vertices[339] = {
#include "stg40/terrain/draw_14_vertices.inc"
};
static NJS_CNK_MODEL draw_14_model = {
#include "stg40/terrain/draw_14_model.inc"
};
static NJS_CNK_OBJECT draw_495 = {
#include "stg40/terrain/draw_495.inc"
};
static Sint16 draw_18_polygons[429] = {
#include "stg40/terrain/draw_18_polygons.inc"
};
static Sint32 draw_18_vertices[319] = {
#include "stg40/terrain/draw_18_vertices.inc"
};
static NJS_CNK_MODEL draw_18_model = {
#include "stg40/terrain/draw_18_model.inc"
};
static NJS_CNK_OBJECT draw_494 = {
#include "stg40/terrain/draw_494.inc"
};
static Sint16 draw_0_polygons[391] = {
#include "stg40/terrain/draw_0_polygons.inc"
};
static Sint32 draw_0_vertices[279] = {
#include "stg40/terrain/draw_0_vertices.inc"
};
static NJS_CNK_MODEL draw_0_model = {
#include "stg40/terrain/draw_0_model.inc"
};
static NJS_CNK_OBJECT draw_488 = {
#include "stg40/terrain/draw_488.inc"
};
static Sint16 draw_39_polygons[173] = {
#include "stg40/terrain/draw_39_polygons.inc"
};
static Sint32 draw_39_vertices[139] = {
#include "stg40/terrain/draw_39_vertices.inc"
};
static NJS_CNK_MODEL draw_39_model = {
#include "stg40/terrain/draw_39_model.inc"
};
static NJS_CNK_OBJECT draw_476 = {
#include "stg40/terrain/draw_476.inc"
};
static Sint16 draw_306_polygons[355] = {
#include "stg40/terrain/draw_306_polygons.inc"
};
static Sint32 draw_306_vertices[243] = {
#include "stg40/terrain/draw_306_vertices.inc"
};
static NJS_CNK_MODEL draw_306_model = {
#include "stg40/terrain/draw_306_model.inc"
};
static NJS_CNK_OBJECT draw_468 = {
#include "stg40/terrain/draw_468.inc"
};
static Sint16 draw_6_polygons[169] = {
#include "stg40/terrain/draw_6_polygons.inc"
};
static Sint32 draw_6_vertices[155] = {
#include "stg40/terrain/draw_6_vertices.inc"
};
static NJS_CNK_MODEL draw_6_model = {
#include "stg40/terrain/draw_6_model.inc"
};
static NJS_CNK_OBJECT draw_467 = {
#include "stg40/terrain/draw_467.inc"
};
static Sint16 draw_22_polygons[141] = {
#include "stg40/terrain/draw_22_polygons.inc"
};
static Sint32 draw_22_vertices[99] = {
#include "stg40/terrain/draw_22_vertices.inc"
};
static NJS_CNK_MODEL draw_22_model = {
#include "stg40/terrain/draw_22_model.inc"
};
static NJS_CNK_OBJECT draw_402 = {
#include "stg40/terrain/draw_402.inc"
};
static Sint16 draw_20_polygons[155] = {
#include "stg40/terrain/draw_20_polygons.inc"
};
static Sint32 draw_20_vertices[115] = {
#include "stg40/terrain/draw_20_vertices.inc"
};
static NJS_CNK_MODEL draw_20_model = {
#include "stg40/terrain/draw_20_model.inc"
};
static NJS_CNK_OBJECT draw_401 = {
#include "stg40/terrain/draw_401.inc"
};
static Sint16 draw_23_polygons[735] = {
#include "stg40/terrain/draw_23_polygons.inc"
};
static Sint32 draw_23_vertices[447] = {
#include "stg40/terrain/draw_23_vertices.inc"
};
static NJS_CNK_MODEL draw_23_model = {
#include "stg40/terrain/draw_23_model.inc"
};
static NJS_CNK_OBJECT draw_391 = {
#include "stg40/terrain/draw_391.inc"
};
static Sint16 draw_25_polygons[515] = {
#include "stg40/terrain/draw_25_polygons.inc"
};
static Sint32 draw_25_vertices[383] = {
#include "stg40/terrain/draw_25_vertices.inc"
};
static NJS_CNK_MODEL draw_25_model = {
#include "stg40/terrain/draw_25_model.inc"
};
static NJS_CNK_OBJECT draw_390 = {
#include "stg40/terrain/draw_390.inc"
};
static Sint16 draw_24_polygons[55] = {
#include "stg40/terrain/draw_24_polygons.inc"
};
static Sint32 draw_24_vertices[51] = {
#include "stg40/terrain/draw_24_vertices.inc"
};
static NJS_CNK_MODEL draw_24_model = {
#include "stg40/terrain/draw_24_model.inc"
};
static NJS_CNK_OBJECT draw_389 = {
#include "stg40/terrain/draw_389.inc"
};
static Sint16 draw_377_polygons[45] = {
#include "stg40/terrain/draw_377_polygons.inc"
};
static Sint32 draw_377_vertices[31] = {
#include "stg40/terrain/draw_377_vertices.inc"
};
static NJS_CNK_MODEL draw_377_model = {
#include "stg40/terrain/draw_377_model.inc"
};
static NJS_CNK_OBJECT draw_377 = {
#include "stg40/terrain/draw_377.inc"
};
static Sint16 draw_370_polygons[145] = {
#include "stg40/terrain/draw_370_polygons.inc"
};
static Sint32 draw_370_vertices[99] = {
#include "stg40/terrain/draw_370_vertices.inc"
};
static NJS_CNK_MODEL draw_370_model = {
#include "stg40/terrain/draw_370_model.inc"
};
static NJS_CNK_OBJECT draw_370 = {
#include "stg40/terrain/draw_370.inc"
};
static Sint16 draw_305_polygons[91] = {
#include "stg40/terrain/draw_305_polygons.inc"
};
static Sint32 draw_305_vertices[83] = {
#include "stg40/terrain/draw_305_vertices.inc"
};
static NJS_CNK_MODEL draw_305_model = {
#include "stg40/terrain/draw_305_model.inc"
};
static NJS_CNK_OBJECT draw_305 = {
#include "stg40/terrain/draw_305.inc"
};
static Sint16 draw_132_polygons[307] = {
#include "stg40/terrain/draw_132_polygons.inc"
};
static Sint32 draw_132_vertices[199] = {
#include "stg40/terrain/draw_132_vertices.inc"
};
static NJS_CNK_MODEL draw_132_model = {
#include "stg40/terrain/draw_132_model.inc"
};
static NJS_CNK_OBJECT draw_275 = {
#include "stg40/terrain/draw_275.inc"
};
static Sint16 draw_149_polygons[519] = {
#include "stg40/terrain/draw_149_polygons.inc"
};
static Sint32 draw_149_vertices[419] = {
#include "stg40/terrain/draw_149_vertices.inc"
};
static NJS_CNK_MODEL draw_149_model = {
#include "stg40/terrain/draw_149_model.inc"
};
static NJS_CNK_OBJECT draw_274 = {
#include "stg40/terrain/draw_274.inc"
};
static Sint16 draw_198_polygons[451] = {
#include "stg40/terrain/draw_198_polygons.inc"
};
static Sint32 draw_198_vertices[327] = {
#include "stg40/terrain/draw_198_vertices.inc"
};
static NJS_CNK_MODEL draw_198_model = {
#include "stg40/terrain/draw_198_model.inc"
};
static NJS_CNK_OBJECT draw_268 = {
#include "stg40/terrain/draw_268.inc"
};
static Sint16 draw_137_polygons[59] = {
#include "stg40/terrain/draw_137_polygons.inc"
};
static Sint32 draw_137_vertices[43] = {
#include "stg40/terrain/draw_137_vertices.inc"
};
static NJS_CNK_MODEL draw_137_model = {
#include "stg40/terrain/draw_137_model.inc"
};
static NJS_CNK_OBJECT draw_260 = {
#include "stg40/terrain/draw_260.inc"
};
static Sint16 draw_131_polygons[519] = {
#include "stg40/terrain/draw_131_polygons.inc"
};
static Sint32 draw_131_vertices[431] = {
#include "stg40/terrain/draw_131_vertices.inc"
};
static NJS_CNK_MODEL draw_131_model = {
#include "stg40/terrain/draw_131_model.inc"
};
static NJS_CNK_OBJECT draw_248 = {
#include "stg40/terrain/draw_248.inc"
};
static Sint16 draw_143_polygons[59] = {
#include "stg40/terrain/draw_143_polygons.inc"
};
static Sint32 draw_143_vertices[43] = {
#include "stg40/terrain/draw_143_vertices.inc"
};
static NJS_CNK_MODEL draw_143_model = {
#include "stg40/terrain/draw_143_model.inc"
};
static NJS_CNK_OBJECT draw_247 = {
#include "stg40/terrain/draw_247.inc"
};
static Sint16 draw_118_polygons[301] = {
#include "stg40/terrain/draw_118_polygons.inc"
};
static Sint32 draw_118_vertices[195] = {
#include "stg40/terrain/draw_118_vertices.inc"
};
static NJS_CNK_MODEL draw_118_model = {
#include "stg40/terrain/draw_118_model.inc"
};
static NJS_CNK_OBJECT draw_225 = {
#include "stg40/terrain/draw_225.inc"
};
static Sint16 draw_185_polygons[511] = {
#include "stg40/terrain/draw_185_polygons.inc"
};
static Sint32 draw_185_vertices[423] = {
#include "stg40/terrain/draw_185_vertices.inc"
};
static NJS_CNK_MODEL draw_185_model = {
#include "stg40/terrain/draw_185_model.inc"
};
static NJS_CNK_OBJECT draw_224 = {
#include "stg40/terrain/draw_224.inc"
};
static Sint16 draw_128_polygons[55] = {
#include "stg40/terrain/draw_128_polygons.inc"
};
static Sint32 draw_128_vertices[35] = {
#include "stg40/terrain/draw_128_vertices.inc"
};
static NJS_CNK_MODEL draw_128_model = {
#include "stg40/terrain/draw_128_model.inc"
};
static NJS_CNK_OBJECT draw_223 = {
#include "stg40/terrain/draw_223.inc"
};
static Sint16 draw_216_polygons[77] = {
#include "stg40/terrain/draw_216_polygons.inc"
};
static Sint32 draw_216_vertices[63] = {
#include "stg40/terrain/draw_216_vertices.inc"
};
static NJS_CNK_MODEL draw_216_model = {
#include "stg40/terrain/draw_216_model.inc"
};
static NJS_CNK_OBJECT draw_216 = {
#include "stg40/terrain/draw_216.inc"
};
static Sint16 draw_189_polygons[525] = {
#include "stg40/terrain/draw_189_polygons.inc"
};
static Sint32 draw_189_vertices[615] = {
#include "stg40/terrain/draw_189_vertices.inc"
};
static NJS_CNK_MODEL draw_189_model = {
#include "stg40/terrain/draw_189_model.inc"
};
static NJS_CNK_OBJECT draw_189 = {
#include "stg40/terrain/draw_189.inc"
};
static Sint16 draw_124_polygons[21] = {
#include "stg40/terrain/draw_124_polygons.inc"
};
static Sint32 draw_124_vertices[51] = {
#include "stg40/terrain/draw_124_vertices.inc"
};
static NJS_CNK_MODEL draw_124_model = {
#include "stg40/terrain/draw_124_model.inc"
};
static NJS_CNK_OBJECT draw_124 = {
#include "stg40/terrain/draw_124.inc"
};
static Sint16 draw_29_polygons[157] = {
#include "stg40/terrain/draw_29_polygons.inc"
};
static Sint32 draw_29_vertices[107] = {
#include "stg40/terrain/draw_29_vertices.inc"
};
static NJS_CNK_MODEL draw_29_model = {
#include "stg40/terrain/draw_29_model.inc"
};
static NJS_CNK_OBJECT draw_29 = {
#include "stg40/terrain/draw_29.inc"
};
static Sint16 draw_28_polygons[133] = {
#include "stg40/terrain/draw_28_polygons.inc"
};
static Sint32 draw_28_vertices[83] = {
#include "stg40/terrain/draw_28_vertices.inc"
};
static NJS_CNK_MODEL draw_28_model = {
#include "stg40/terrain/draw_28_model.inc"
};
static NJS_CNK_OBJECT draw_28 = {
#include "stg40/terrain/draw_28.inc"
};
static Sint16 draw_27_polygons[183] = {
#include "stg40/terrain/draw_27_polygons.inc"
};
static Sint32 draw_27_vertices[123] = {
#include "stg40/terrain/draw_27_vertices.inc"
};
static NJS_CNK_MODEL draw_27_model = {
#include "stg40/terrain/draw_27_model.inc"
};
static NJS_CNK_OBJECT draw_27 = {
#include "stg40/terrain/draw_27.inc"
};
static Sint16 draw_26_polygons[173] = {
#include "stg40/terrain/draw_26_polygons.inc"
};
static Sint32 draw_26_vertices[83] = {
#include "stg40/terrain/draw_26_vertices.inc"
};
static NJS_CNK_MODEL draw_26_model = {
#include "stg40/terrain/draw_26_model.inc"
};
static NJS_CNK_OBJECT draw_26 = {
#include "stg40/terrain/draw_26.inc"
};
static NJS_MATERIAL collision_0_materials[1] = {
#include "stg40/terrain/collision_0_materials.inc"
};
static Sint16 collision_0_faces[28] = {
#include "stg40/terrain/collision_0_faces.inc"
};
static NJS_MESHSET collision_0_meshsets[1] = {
#include "stg40/terrain/collision_0_meshsets.inc"
};
static NJS_POINT3 collision_0_points[18] = {
#include "stg40/terrain/collision_0_points.inc"
};
static NJS_MODEL collision_0_model = {
#include "stg40/terrain/collision_0_model.inc"
};
static NJS_OBJECT collision_0 = {
#include "stg40/terrain/collision_0.inc"
};
static NJS_MATERIAL collision_1_materials[1] = {
#include "stg40/terrain/collision_1_materials.inc"
};
static Sint16 collision_1_faces[33] = {
#include "stg40/terrain/collision_1_faces.inc"
};
static NJS_MESHSET collision_1_meshsets[1] = {
#include "stg40/terrain/collision_1_meshsets.inc"
};
static NJS_POINT3 collision_1_points[18] = {
#include "stg40/terrain/collision_1_points.inc"
};
static NJS_MODEL collision_1_model = {
#include "stg40/terrain/collision_1_model.inc"
};
static NJS_OBJECT collision_1 = {
#include "stg40/terrain/collision_1.inc"
};
static NJS_MATERIAL collision_2_materials[1] = {
#include "stg40/terrain/collision_2_materials.inc"
};
static Sint16 collision_2_faces[46] = {
#include "stg40/terrain/collision_2_faces.inc"
};
static NJS_MESHSET collision_2_meshsets[1] = {
#include "stg40/terrain/collision_2_meshsets.inc"
};
static NJS_POINT3 collision_2_points[30] = {
#include "stg40/terrain/collision_2_points.inc"
};
static NJS_MODEL collision_2_model = {
#include "stg40/terrain/collision_2_model.inc"
};
static NJS_OBJECT collision_2 = {
#include "stg40/terrain/collision_2.inc"
};
static NJS_MATERIAL collision_3_materials[1] = {
#include "stg40/terrain/collision_3_materials.inc"
};
static Sint16 collision_3_faces[27] = {
#include "stg40/terrain/collision_3_faces.inc"
};
static NJS_MESHSET collision_3_meshsets[1] = {
#include "stg40/terrain/collision_3_meshsets.inc"
};
static NJS_POINT3 collision_3_points[16] = {
#include "stg40/terrain/collision_3_points.inc"
};
static NJS_MODEL collision_3_model = {
#include "stg40/terrain/collision_3_model.inc"
};
static NJS_OBJECT collision_3 = {
#include "stg40/terrain/collision_3.inc"
};
static NJS_MATERIAL collision_7_materials[1] = {
#include "stg40/terrain/collision_7_materials.inc"
};
static Sint16 collision_7_faces[46] = {
#include "stg40/terrain/collision_7_faces.inc"
};
static NJS_MESHSET collision_7_meshsets[1] = {
#include "stg40/terrain/collision_7_meshsets.inc"
};
static NJS_POINT3 collision_7_points[30] = {
#include "stg40/terrain/collision_7_points.inc"
};
static NJS_MODEL collision_7_model = {
#include "stg40/terrain/collision_7_model.inc"
};
static NJS_OBJECT collision_7 = {
#include "stg40/terrain/collision_7.inc"
};
static NJS_MATERIAL collision_8_materials[1] = {
#include "stg40/terrain/collision_8_materials.inc"
};
static Sint16 collision_8_faces[48] = {
#include "stg40/terrain/collision_8_faces.inc"
};
static NJS_MESHSET collision_8_meshsets[1] = {
#include "stg40/terrain/collision_8_meshsets.inc"
};
static NJS_POINT3 collision_8_points[30] = {
#include "stg40/terrain/collision_8_points.inc"
};
static NJS_MODEL collision_8_model = {
#include "stg40/terrain/collision_8_model.inc"
};
static NJS_OBJECT collision_8 = {
#include "stg40/terrain/collision_8.inc"
};
static NJS_MATERIAL collision_9_materials[1] = {
#include "stg40/terrain/collision_9_materials.inc"
};
static Sint16 collision_9_faces[46] = {
#include "stg40/terrain/collision_9_faces.inc"
};
static NJS_MESHSET collision_9_meshsets[1] = {
#include "stg40/terrain/collision_9_meshsets.inc"
};
static NJS_POINT3 collision_9_points[30] = {
#include "stg40/terrain/collision_9_points.inc"
};
static NJS_MODEL collision_9_model = {
#include "stg40/terrain/collision_9_model.inc"
};
static NJS_OBJECT collision_9 = {
#include "stg40/terrain/collision_9.inc"
};
static NJS_MATERIAL collision_12_materials[1] = {
#include "stg40/terrain/collision_12_materials.inc"
};
static Sint16 collision_12_faces[39] = {
#include "stg40/terrain/collision_12_faces.inc"
};
static NJS_MESHSET collision_12_meshsets[1] = {
#include "stg40/terrain/collision_12_meshsets.inc"
};
static NJS_POINT3 collision_12_points[30] = {
#include "stg40/terrain/collision_12_points.inc"
};
static NJS_MODEL collision_12_model = {
#include "stg40/terrain/collision_12_model.inc"
};
static NJS_OBJECT collision_12 = {
#include "stg40/terrain/collision_12.inc"
};
static NJS_MATERIAL collision_13_materials[1] = {
#include "stg40/terrain/collision_13_materials.inc"
};
static Sint16 collision_13_faces[39] = {
#include "stg40/terrain/collision_13_faces.inc"
};
static NJS_MESHSET collision_13_meshsets[1] = {
#include "stg40/terrain/collision_13_meshsets.inc"
};
static NJS_POINT3 collision_13_points[30] = {
#include "stg40/terrain/collision_13_points.inc"
};
static NJS_MODEL collision_13_model = {
#include "stg40/terrain/collision_13_model.inc"
};
static NJS_OBJECT collision_13 = {
#include "stg40/terrain/collision_13.inc"
};
static NJS_MATERIAL collision_16_materials[1] = {
#include "stg40/terrain/collision_16_materials.inc"
};
static Sint16 collision_16_faces[53] = {
#include "stg40/terrain/collision_16_faces.inc"
};
static NJS_MESHSET collision_16_meshsets[1] = {
#include "stg40/terrain/collision_16_meshsets.inc"
};
static NJS_POINT3 collision_16_points[24] = {
#include "stg40/terrain/collision_16_points.inc"
};
static NJS_MODEL collision_16_model = {
#include "stg40/terrain/collision_16_model.inc"
};
static NJS_OBJECT collision_16 = {
#include "stg40/terrain/collision_16.inc"
};
static NJS_MATERIAL collision_23_materials[1] = {
#include "stg40/terrain/collision_23_materials.inc"
};
static Sint16 collision_23_faces[152] = {
#include "stg40/terrain/collision_23_faces.inc"
};
static NJS_MESHSET collision_23_meshsets[1] = {
#include "stg40/terrain/collision_23_meshsets.inc"
};
static NJS_POINT3 collision_23_points[73] = {
#include "stg40/terrain/collision_23_points.inc"
};
static NJS_MODEL collision_23_model = {
#include "stg40/terrain/collision_23_model.inc"
};
static NJS_OBJECT collision_23 = {
#include "stg40/terrain/collision_23.inc"
};
static NJS_MATERIAL collision_24_materials[1] = {
#include "stg40/terrain/collision_24_materials.inc"
};
static Sint16 collision_24_faces[27] = {
#include "stg40/terrain/collision_24_faces.inc"
};
static NJS_MESHSET collision_24_meshsets[1] = {
#include "stg40/terrain/collision_24_meshsets.inc"
};
static NJS_POINT3 collision_24_points[16] = {
#include "stg40/terrain/collision_24_points.inc"
};
static NJS_MODEL collision_24_model = {
#include "stg40/terrain/collision_24_model.inc"
};
static NJS_OBJECT collision_24 = {
#include "stg40/terrain/collision_24.inc"
};
static NJS_MATERIAL collision_28_materials[1] = {
#include "stg40/terrain/collision_28_materials.inc"
};
static Sint16 collision_28_faces[27] = {
#include "stg40/terrain/collision_28_faces.inc"
};
static NJS_MESHSET collision_28_meshsets[1] = {
#include "stg40/terrain/collision_28_meshsets.inc"
};
static NJS_POINT3 collision_28_points[16] = {
#include "stg40/terrain/collision_28_points.inc"
};
static NJS_MODEL collision_28_model = {
#include "stg40/terrain/collision_28_model.inc"
};
static NJS_OBJECT collision_28 = {
#include "stg40/terrain/collision_28.inc"
};
static NJS_MATERIAL collision_98_materials[1] = {
#include "stg40/terrain/collision_98_materials.inc"
};
static Sint16 collision_98_faces[77] = {
#include "stg40/terrain/collision_98_faces.inc"
};
static NJS_MESHSET collision_98_meshsets[1] = {
#include "stg40/terrain/collision_98_meshsets.inc"
};
static NJS_POINT3 collision_98_points[34] = {
#include "stg40/terrain/collision_98_points.inc"
};
static NJS_MODEL collision_98_model = {
#include "stg40/terrain/collision_98_model.inc"
};
static NJS_OBJECT collision_98 = {
#include "stg40/terrain/collision_98.inc"
};
static NJS_MATERIAL collision_100_materials[1] = {
#include "stg40/terrain/collision_100_materials.inc"
};
static Sint16 collision_100_faces[22] = {
#include "stg40/terrain/collision_100_faces.inc"
};
static NJS_MESHSET collision_100_meshsets[1] = {
#include "stg40/terrain/collision_100_meshsets.inc"
};
static NJS_POINT3 collision_100_points[12] = {
#include "stg40/terrain/collision_100_points.inc"
};
static NJS_MODEL collision_100_model = {
#include "stg40/terrain/collision_100_model.inc"
};
static NJS_OBJECT collision_100 = {
#include "stg40/terrain/collision_100.inc"
};
static NJS_MATERIAL collision_106_materials[1] = {
#include "stg40/terrain/collision_106_materials.inc"
};
static Sint16 collision_106_faces[60] = {
#include "stg40/terrain/collision_106_faces.inc"
};
static NJS_MESHSET collision_106_meshsets[1] = {
#include "stg40/terrain/collision_106_meshsets.inc"
};
static NJS_POINT3 collision_106_points[32] = {
#include "stg40/terrain/collision_106_points.inc"
};
static NJS_MODEL collision_106_model = {
#include "stg40/terrain/collision_106_model.inc"
};
static NJS_OBJECT collision_106 = {
#include "stg40/terrain/collision_106.inc"
};
static NJS_MATERIAL collision_108_materials[1] = {
#include "stg40/terrain/collision_108_materials.inc"
};
static Sint16 collision_108_faces[27] = {
#include "stg40/terrain/collision_108_faces.inc"
};
static NJS_MESHSET collision_108_meshsets[1] = {
#include "stg40/terrain/collision_108_meshsets.inc"
};
static NJS_POINT3 collision_108_points[16] = {
#include "stg40/terrain/collision_108_points.inc"
};
static NJS_MODEL collision_108_model = {
#include "stg40/terrain/collision_108_model.inc"
};
static NJS_OBJECT collision_108 = {
#include "stg40/terrain/collision_108.inc"
};
static NJS_MATERIAL collision_112_materials[1] = {
#include "stg40/terrain/collision_112_materials.inc"
};
static Sint16 collision_112_faces[33] = {
#include "stg40/terrain/collision_112_faces.inc"
};
static NJS_MESHSET collision_112_meshsets[1] = {
#include "stg40/terrain/collision_112_meshsets.inc"
};
static NJS_POINT3 collision_112_points[18] = {
#include "stg40/terrain/collision_112_points.inc"
};
static NJS_MODEL collision_112_model = {
#include "stg40/terrain/collision_112_model.inc"
};
static NJS_OBJECT collision_112 = {
#include "stg40/terrain/collision_112.inc"
};
static NJS_MATERIAL collision_113_materials[1] = {
#include "stg40/terrain/collision_113_materials.inc"
};
static Sint16 collision_113_faces[55] = {
#include "stg40/terrain/collision_113_faces.inc"
};
static NJS_MESHSET collision_113_meshsets[1] = {
#include "stg40/terrain/collision_113_meshsets.inc"
};
static NJS_POINT3 collision_113_points[30] = {
#include "stg40/terrain/collision_113_points.inc"
};
static NJS_MODEL collision_113_model = {
#include "stg40/terrain/collision_113_model.inc"
};
static NJS_OBJECT collision_113 = {
#include "stg40/terrain/collision_113.inc"
};
static NJS_MATERIAL collision_114_materials[1] = {
#include "stg40/terrain/collision_114_materials.inc"
};
static Sint16 collision_114_faces[236] = {
#include "stg40/terrain/collision_114_faces.inc"
};
static NJS_MESHSET collision_114_meshsets[1] = {
#include "stg40/terrain/collision_114_meshsets.inc"
};
static NJS_POINT3 collision_114_points[120] = {
#include "stg40/terrain/collision_114_points.inc"
};
static NJS_MODEL collision_114_model = {
#include "stg40/terrain/collision_114_model.inc"
};
static NJS_OBJECT collision_114 = {
#include "stg40/terrain/collision_114.inc"
};
static NJS_MATERIAL collision_115_materials[1] = {
#include "stg40/terrain/collision_115_materials.inc"
};
static Sint16 collision_115_faces[46] = {
#include "stg40/terrain/collision_115_faces.inc"
};
static NJS_MESHSET collision_115_meshsets[1] = {
#include "stg40/terrain/collision_115_meshsets.inc"
};
static NJS_POINT3 collision_115_points[19] = {
#include "stg40/terrain/collision_115_points.inc"
};
static NJS_MODEL collision_115_model = {
#include "stg40/terrain/collision_115_model.inc"
};
static NJS_OBJECT collision_115 = {
#include "stg40/terrain/collision_115.inc"
};
static NJS_MATERIAL collision_117_materials[1] = {
#include "stg40/terrain/collision_117_materials.inc"
};
static Sint16 collision_117_faces[51] = {
#include "stg40/terrain/collision_117_faces.inc"
};
static NJS_MESHSET collision_117_meshsets[1] = {
#include "stg40/terrain/collision_117_meshsets.inc"
};
static NJS_POINT3 collision_117_points[26] = {
#include "stg40/terrain/collision_117_points.inc"
};
static NJS_MODEL collision_117_model = {
#include "stg40/terrain/collision_117_model.inc"
};
static NJS_OBJECT collision_117 = {
#include "stg40/terrain/collision_117.inc"
};
static NJS_MATERIAL collision_118_materials[1] = {
#include "stg40/terrain/collision_118_materials.inc"
};
static Sint16 collision_118_faces[215] = {
#include "stg40/terrain/collision_118_faces.inc"
};
static NJS_MESHSET collision_118_meshsets[1] = {
#include "stg40/terrain/collision_118_meshsets.inc"
};
static NJS_POINT3 collision_118_points[120] = {
#include "stg40/terrain/collision_118_points.inc"
};
static NJS_MODEL collision_118_model = {
#include "stg40/terrain/collision_118_model.inc"
};
static NJS_OBJECT collision_118 = {
#include "stg40/terrain/collision_118.inc"
};
static NJS_MATERIAL collision_119_materials[1] = {
#include "stg40/terrain/collision_119_materials.inc"
};
static Sint16 collision_119_faces[63] = {
#include "stg40/terrain/collision_119_faces.inc"
};
static NJS_MESHSET collision_119_meshsets[1] = {
#include "stg40/terrain/collision_119_meshsets.inc"
};
static NJS_POINT3 collision_119_points[32] = {
#include "stg40/terrain/collision_119_points.inc"
};
static NJS_MODEL collision_119_model = {
#include "stg40/terrain/collision_119_model.inc"
};
static NJS_OBJECT collision_119 = {
#include "stg40/terrain/collision_119.inc"
};
static NJS_MATERIAL collision_120_materials[1] = {
#include "stg40/terrain/collision_120_materials.inc"
};
static Sint16 collision_120_faces[129] = {
#include "stg40/terrain/collision_120_faces.inc"
};
static NJS_MESHSET collision_120_meshsets[1] = {
#include "stg40/terrain/collision_120_meshsets.inc"
};
static NJS_POINT3 collision_120_points[80] = {
#include "stg40/terrain/collision_120_points.inc"
};
static NJS_MODEL collision_120_model = {
#include "stg40/terrain/collision_120_model.inc"
};
static NJS_OBJECT collision_120 = {
#include "stg40/terrain/collision_120.inc"
};
static NJS_MATERIAL collision_121_materials[1] = {
#include "stg40/terrain/collision_121_materials.inc"
};
static Sint16 collision_121_faces[33] = {
#include "stg40/terrain/collision_121_faces.inc"
};
static NJS_MESHSET collision_121_meshsets[1] = {
#include "stg40/terrain/collision_121_meshsets.inc"
};
static NJS_POINT3 collision_121_points[20] = {
#include "stg40/terrain/collision_121_points.inc"
};
static NJS_MODEL collision_121_model = {
#include "stg40/terrain/collision_121_model.inc"
};
static NJS_OBJECT collision_121 = {
#include "stg40/terrain/collision_121.inc"
};
static NJS_MATERIAL collision_125_materials[1] = {
#include "stg40/terrain/collision_125_materials.inc"
};
static Sint16 collision_125_faces[36] = {
#include "stg40/terrain/collision_125_faces.inc"
};
static NJS_MESHSET collision_125_meshsets[1] = {
#include "stg40/terrain/collision_125_meshsets.inc"
};
static NJS_POINT3 collision_125_points[20] = {
#include "stg40/terrain/collision_125_points.inc"
};
static NJS_MODEL collision_125_model = {
#include "stg40/terrain/collision_125_model.inc"
};
static NJS_OBJECT collision_125 = {
#include "stg40/terrain/collision_125.inc"
};
static NJS_MATERIAL collision_130_materials[1] = {
#include "stg40/terrain/collision_130_materials.inc"
};
static Sint16 collision_130_faces[263] = {
#include "stg40/terrain/collision_130_faces.inc"
};
static NJS_MESHSET collision_130_meshsets[1] = {
#include "stg40/terrain/collision_130_meshsets.inc"
};
static NJS_POINT3 collision_130_points[120] = {
#include "stg40/terrain/collision_130_points.inc"
};
static NJS_MODEL collision_130_model = {
#include "stg40/terrain/collision_130_model.inc"
};
static NJS_OBJECT collision_130 = {
#include "stg40/terrain/collision_130.inc"
};
static NJS_MATERIAL collision_131_materials[1] = {
#include "stg40/terrain/collision_131_materials.inc"
};
static Sint16 collision_131_faces[45] = {
#include "stg40/terrain/collision_131_faces.inc"
};
static NJS_MESHSET collision_131_meshsets[1] = {
#include "stg40/terrain/collision_131_meshsets.inc"
};
static NJS_POINT3 collision_131_points[24] = {
#include "stg40/terrain/collision_131_points.inc"
};
static NJS_MODEL collision_131_model = {
#include "stg40/terrain/collision_131_model.inc"
};
static NJS_OBJECT collision_131 = {
#include "stg40/terrain/collision_131.inc"
};
static NJS_MATERIAL collision_133_materials[1] = {
#include "stg40/terrain/collision_133_materials.inc"
};
static Sint16 collision_133_faces[13] = {
#include "stg40/terrain/collision_133_faces.inc"
};
static NJS_MESHSET collision_133_meshsets[1] = {
#include "stg40/terrain/collision_133_meshsets.inc"
};
static NJS_POINT3 collision_133_points[12] = {
#include "stg40/terrain/collision_133_points.inc"
};
static NJS_MODEL collision_133_model = {
#include "stg40/terrain/collision_133_model.inc"
};
static NJS_OBJECT collision_133 = {
#include "stg40/terrain/collision_133.inc"
};
static NJS_MATERIAL collision_135_materials[1] = {
#include "stg40/terrain/collision_135_materials.inc"
};
static Sint16 collision_135_faces[64] = {
#include "stg40/terrain/collision_135_faces.inc"
};
static NJS_MESHSET collision_135_meshsets[1] = {
#include "stg40/terrain/collision_135_meshsets.inc"
};
static NJS_POINT3 collision_135_points[33] = {
#include "stg40/terrain/collision_135_points.inc"
};
static NJS_MODEL collision_135_model = {
#include "stg40/terrain/collision_135_model.inc"
};
static NJS_OBJECT collision_135 = {
#include "stg40/terrain/collision_135.inc"
};
static NJS_MATERIAL collision_136_materials[1] = {
#include "stg40/terrain/collision_136_materials.inc"
};
static Sint16 collision_136_faces[81] = {
#include "stg40/terrain/collision_136_faces.inc"
};
static NJS_MESHSET collision_136_meshsets[1] = {
#include "stg40/terrain/collision_136_meshsets.inc"
};
static NJS_POINT3 collision_136_points[48] = {
#include "stg40/terrain/collision_136_points.inc"
};
static NJS_MODEL collision_136_model = {
#include "stg40/terrain/collision_136_model.inc"
};
static NJS_OBJECT collision_136 = {
#include "stg40/terrain/collision_136.inc"
};
static NJS_MATERIAL collision_139_materials[1] = {
#include "stg40/terrain/collision_139_materials.inc"
};
static Sint16 collision_139_faces[91] = {
#include "stg40/terrain/collision_139_faces.inc"
};
static NJS_MESHSET collision_139_meshsets[1] = {
#include "stg40/terrain/collision_139_meshsets.inc"
};
static NJS_POINT3 collision_139_points[40] = {
#include "stg40/terrain/collision_139_points.inc"
};
static NJS_MODEL collision_139_model = {
#include "stg40/terrain/collision_139_model.inc"
};
static NJS_OBJECT collision_139 = {
#include "stg40/terrain/collision_139.inc"
};
static NJS_MATERIAL collision_140_materials[1] = {
#include "stg40/terrain/collision_140_materials.inc"
};
static Sint16 collision_140_faces[98] = {
#include "stg40/terrain/collision_140_faces.inc"
};
static NJS_MESHSET collision_140_meshsets[1] = {
#include "stg40/terrain/collision_140_meshsets.inc"
};
static NJS_POINT3 collision_140_points[52] = {
#include "stg40/terrain/collision_140_points.inc"
};
static NJS_MODEL collision_140_model = {
#include "stg40/terrain/collision_140_model.inc"
};
static NJS_OBJECT collision_140 = {
#include "stg40/terrain/collision_140.inc"
};
static NJS_MATERIAL collision_141_materials[1] = {
#include "stg40/terrain/collision_141_materials.inc"
};
static Sint16 collision_141_faces[36] = {
#include "stg40/terrain/collision_141_faces.inc"
};
static NJS_MESHSET collision_141_meshsets[1] = {
#include "stg40/terrain/collision_141_meshsets.inc"
};
static NJS_POINT3 collision_141_points[20] = {
#include "stg40/terrain/collision_141_points.inc"
};
static NJS_MODEL collision_141_model = {
#include "stg40/terrain/collision_141_model.inc"
};
static NJS_OBJECT collision_141 = {
#include "stg40/terrain/collision_141.inc"
};
static NJS_MATERIAL collision_143_materials[1] = {
#include "stg40/terrain/collision_143_materials.inc"
};
static Sint16 collision_143_faces[125] = {
#include "stg40/terrain/collision_143_faces.inc"
};
static NJS_MESHSET collision_143_meshsets[1] = {
#include "stg40/terrain/collision_143_meshsets.inc"
};
static NJS_POINT3 collision_143_points[61] = {
#include "stg40/terrain/collision_143_points.inc"
};
static NJS_MODEL collision_143_model = {
#include "stg40/terrain/collision_143_model.inc"
};
static NJS_OBJECT collision_143 = {
#include "stg40/terrain/collision_143.inc"
};
static NJS_MATERIAL collision_144_materials[1] = {
#include "stg40/terrain/collision_144_materials.inc"
};
static Sint16 collision_144_faces[67] = {
#include "stg40/terrain/collision_144_faces.inc"
};
static NJS_MESHSET collision_144_meshsets[1] = {
#include "stg40/terrain/collision_144_meshsets.inc"
};
static NJS_POINT3 collision_144_points[33] = {
#include "stg40/terrain/collision_144_points.inc"
};
static NJS_MODEL collision_144_model = {
#include "stg40/terrain/collision_144_model.inc"
};
static NJS_OBJECT collision_144 = {
#include "stg40/terrain/collision_144.inc"
};
static NJS_MATERIAL collision_145_materials[1] = {
#include "stg40/terrain/collision_145_materials.inc"
};
static Sint16 collision_145_faces[146] = {
#include "stg40/terrain/collision_145_faces.inc"
};
static NJS_MESHSET collision_145_meshsets[1] = {
#include "stg40/terrain/collision_145_meshsets.inc"
};
static NJS_POINT3 collision_145_points[77] = {
#include "stg40/terrain/collision_145_points.inc"
};
static NJS_MODEL collision_145_model = {
#include "stg40/terrain/collision_145_model.inc"
};
static NJS_OBJECT collision_145 = {
#include "stg40/terrain/collision_145.inc"
};
static NJS_MATERIAL collision_146_materials[1] = {
#include "stg40/terrain/collision_146_materials.inc"
};
static Sint16 collision_146_faces[64] = {
#include "stg40/terrain/collision_146_faces.inc"
};
static NJS_MESHSET collision_146_meshsets[1] = {
#include "stg40/terrain/collision_146_meshsets.inc"
};
static NJS_POINT3 collision_146_points[33] = {
#include "stg40/terrain/collision_146_points.inc"
};
static NJS_MODEL collision_146_model = {
#include "stg40/terrain/collision_146_model.inc"
};
static NJS_OBJECT collision_146 = {
#include "stg40/terrain/collision_146.inc"
};
static NJS_MATERIAL collision_147_materials[1] = {
#include "stg40/terrain/collision_147_materials.inc"
};
static Sint16 collision_147_faces[68] = {
#include "stg40/terrain/collision_147_faces.inc"
};
static NJS_MESHSET collision_147_meshsets[1] = {
#include "stg40/terrain/collision_147_meshsets.inc"
};
static NJS_POINT3 collision_147_points[42] = {
#include "stg40/terrain/collision_147_points.inc"
};
static NJS_MODEL collision_147_model = {
#include "stg40/terrain/collision_147_model.inc"
};
static NJS_OBJECT collision_147 = {
#include "stg40/terrain/collision_147.inc"
};
static NJS_MATERIAL collision_148_materials[1] = {
#include "stg40/terrain/collision_148_materials.inc"
};
static Sint16 collision_148_faces[88] = {
#include "stg40/terrain/collision_148_faces.inc"
};
static NJS_MESHSET collision_148_meshsets[1] = {
#include "stg40/terrain/collision_148_meshsets.inc"
};
static NJS_POINT3 collision_148_points[52] = {
#include "stg40/terrain/collision_148_points.inc"
};
static NJS_MODEL collision_148_model = {
#include "stg40/terrain/collision_148_model.inc"
};
static NJS_OBJECT collision_148 = {
#include "stg40/terrain/collision_148.inc"
};
static NJS_MATERIAL collision_149_materials[1] = {
#include "stg40/terrain/collision_149_materials.inc"
};
static Sint16 collision_149_faces[185] = {
#include "stg40/terrain/collision_149_faces.inc"
};
static NJS_MESHSET collision_149_meshsets[1] = {
#include "stg40/terrain/collision_149_meshsets.inc"
};
static NJS_POINT3 collision_149_points[81] = {
#include "stg40/terrain/collision_149_points.inc"
};
static NJS_MODEL collision_149_model = {
#include "stg40/terrain/collision_149_model.inc"
};
static NJS_OBJECT collision_149 = {
#include "stg40/terrain/collision_149.inc"
};
static NJS_MATERIAL collision_150_materials[1] = {
#include "stg40/terrain/collision_150_materials.inc"
};
static Sint16 collision_150_faces[145] = {
#include "stg40/terrain/collision_150_faces.inc"
};
static NJS_MESHSET collision_150_meshsets[1] = {
#include "stg40/terrain/collision_150_meshsets.inc"
};
static NJS_POINT3 collision_150_points[71] = {
#include "stg40/terrain/collision_150_points.inc"
};
static NJS_MODEL collision_150_model = {
#include "stg40/terrain/collision_150_model.inc"
};
static NJS_OBJECT collision_150 = {
#include "stg40/terrain/collision_150.inc"
};
static NJS_MATERIAL collision_151_materials[1] = {
#include "stg40/terrain/collision_151_materials.inc"
};
static Sint16 collision_151_faces[122] = {
#include "stg40/terrain/collision_151_faces.inc"
};
static NJS_MESHSET collision_151_meshsets[1] = {
#include "stg40/terrain/collision_151_meshsets.inc"
};
static NJS_POINT3 collision_151_points[59] = {
#include "stg40/terrain/collision_151_points.inc"
};
static NJS_MODEL collision_151_model = {
#include "stg40/terrain/collision_151_model.inc"
};
static NJS_OBJECT collision_151 = {
#include "stg40/terrain/collision_151.inc"
};
static NJS_MATERIAL collision_152_materials[1] = {
#include "stg40/terrain/collision_152_materials.inc"
};
static Sint16 collision_152_faces[82] = {
#include "stg40/terrain/collision_152_faces.inc"
};
static NJS_MESHSET collision_152_meshsets[1] = {
#include "stg40/terrain/collision_152_meshsets.inc"
};
static NJS_POINT3 collision_152_points[45] = {
#include "stg40/terrain/collision_152_points.inc"
};
static NJS_MODEL collision_152_model = {
#include "stg40/terrain/collision_152_model.inc"
};
static NJS_OBJECT collision_152 = {
#include "stg40/terrain/collision_152.inc"
};
static NJS_MATERIAL collision_153_materials[1] = {
#include "stg40/terrain/collision_153_materials.inc"
};
static Sint16 collision_153_faces[132] = {
#include "stg40/terrain/collision_153_faces.inc"
};
static NJS_MESHSET collision_153_meshsets[1] = {
#include "stg40/terrain/collision_153_meshsets.inc"
};
static NJS_POINT3 collision_153_points[62] = {
#include "stg40/terrain/collision_153_points.inc"
};
static NJS_MODEL collision_153_model = {
#include "stg40/terrain/collision_153_model.inc"
};
static NJS_OBJECT collision_153 = {
#include "stg40/terrain/collision_153.inc"
};
static NJS_MATERIAL collision_154_materials[1] = {
#include "stg40/terrain/collision_154_materials.inc"
};
static Sint16 collision_154_faces[63] = {
#include "stg40/terrain/collision_154_faces.inc"
};
static NJS_MESHSET collision_154_meshsets[1] = {
#include "stg40/terrain/collision_154_meshsets.inc"
};
static NJS_POINT3 collision_154_points[36] = {
#include "stg40/terrain/collision_154_points.inc"
};
static NJS_MODEL collision_154_model = {
#include "stg40/terrain/collision_154_model.inc"
};
static NJS_OBJECT collision_154 = {
#include "stg40/terrain/collision_154.inc"
};
static NJS_MATERIAL collision_155_materials[1] = {
#include "stg40/terrain/collision_155_materials.inc"
};
static Sint16 collision_155_faces[181] = {
#include "stg40/terrain/collision_155_faces.inc"
};
static NJS_MESHSET collision_155_meshsets[1] = {
#include "stg40/terrain/collision_155_meshsets.inc"
};
static NJS_POINT3 collision_155_points[80] = {
#include "stg40/terrain/collision_155_points.inc"
};
static NJS_MODEL collision_155_model = {
#include "stg40/terrain/collision_155_model.inc"
};
static NJS_OBJECT collision_155 = {
#include "stg40/terrain/collision_155.inc"
};
static NJS_MATERIAL collision_156_materials[1] = {
#include "stg40/terrain/collision_156_materials.inc"
};
static Sint16 collision_156_faces[166] = {
#include "stg40/terrain/collision_156_faces.inc"
};
static NJS_MESHSET collision_156_meshsets[1] = {
#include "stg40/terrain/collision_156_meshsets.inc"
};
static NJS_POINT3 collision_156_points[75] = {
#include "stg40/terrain/collision_156_points.inc"
};
static NJS_MODEL collision_156_model = {
#include "stg40/terrain/collision_156_model.inc"
};
static NJS_OBJECT collision_156 = {
#include "stg40/terrain/collision_156.inc"
};
static NJS_MATERIAL collision_157_materials[1] = {
#include "stg40/terrain/collision_157_materials.inc"
};
static Sint16 collision_157_faces[67] = {
#include "stg40/terrain/collision_157_faces.inc"
};
static NJS_MESHSET collision_157_meshsets[1] = {
#include "stg40/terrain/collision_157_meshsets.inc"
};
static NJS_POINT3 collision_157_points[41] = {
#include "stg40/terrain/collision_157_points.inc"
};
static NJS_MODEL collision_157_model = {
#include "stg40/terrain/collision_157_model.inc"
};
static NJS_OBJECT collision_157 = {
#include "stg40/terrain/collision_157.inc"
};
static NJS_MATERIAL collision_158_materials[1] = {
#include "stg40/terrain/collision_158_materials.inc"
};
static Sint16 collision_158_faces[90] = {
#include "stg40/terrain/collision_158_faces.inc"
};
static NJS_MESHSET collision_158_meshsets[1] = {
#include "stg40/terrain/collision_158_meshsets.inc"
};
static NJS_POINT3 collision_158_points[48] = {
#include "stg40/terrain/collision_158_points.inc"
};
static NJS_MODEL collision_158_model = {
#include "stg40/terrain/collision_158_model.inc"
};
static NJS_OBJECT collision_158 = {
#include "stg40/terrain/collision_158.inc"
};
static NJS_MATERIAL collision_159_materials[1] = {
#include "stg40/terrain/collision_159_materials.inc"
};
static Sint16 collision_159_faces[154] = {
#include "stg40/terrain/collision_159_faces.inc"
};
static NJS_MESHSET collision_159_meshsets[1] = {
#include "stg40/terrain/collision_159_meshsets.inc"
};
static NJS_POINT3 collision_159_points[68] = {
#include "stg40/terrain/collision_159_points.inc"
};
static NJS_MODEL collision_159_model = {
#include "stg40/terrain/collision_159_model.inc"
};
static NJS_OBJECT collision_159 = {
#include "stg40/terrain/collision_159.inc"
};
static NJS_MATERIAL collision_160_materials[1] = {
#include "stg40/terrain/collision_160_materials.inc"
};
static Sint16 collision_160_faces[188] = {
#include "stg40/terrain/collision_160_faces.inc"
};
static NJS_MESHSET collision_160_meshsets[1] = {
#include "stg40/terrain/collision_160_meshsets.inc"
};
static NJS_POINT3 collision_160_points[83] = {
#include "stg40/terrain/collision_160_points.inc"
};
static NJS_MODEL collision_160_model = {
#include "stg40/terrain/collision_160_model.inc"
};
static NJS_OBJECT collision_160 = {
#include "stg40/terrain/collision_160.inc"
};
static NJS_MATERIAL collision_161_materials[1] = {
#include "stg40/terrain/collision_161_materials.inc"
};
static Sint16 collision_161_faces[72] = {
#include "stg40/terrain/collision_161_faces.inc"
};
static NJS_MESHSET collision_161_meshsets[1] = {
#include "stg40/terrain/collision_161_meshsets.inc"
};
static NJS_POINT3 collision_161_points[40] = {
#include "stg40/terrain/collision_161_points.inc"
};
static NJS_MODEL collision_161_model = {
#include "stg40/terrain/collision_161_model.inc"
};
static NJS_OBJECT collision_161 = {
#include "stg40/terrain/collision_161.inc"
};
static NJS_MATERIAL collision_162_materials[1] = {
#include "stg40/terrain/collision_162_materials.inc"
};
static Sint16 collision_162_faces[107] = {
#include "stg40/terrain/collision_162_faces.inc"
};
static NJS_MESHSET collision_162_meshsets[1] = {
#include "stg40/terrain/collision_162_meshsets.inc"
};
static NJS_POINT3 collision_162_points[58] = {
#include "stg40/terrain/collision_162_points.inc"
};
static NJS_MODEL collision_162_model = {
#include "stg40/terrain/collision_162_model.inc"
};
static NJS_OBJECT collision_162 = {
#include "stg40/terrain/collision_162.inc"
};
static NJS_MATERIAL collision_163_materials[1] = {
#include "stg40/terrain/collision_163_materials.inc"
};
static Sint16 collision_163_faces[64] = {
#include "stg40/terrain/collision_163_faces.inc"
};
static NJS_MESHSET collision_163_meshsets[1] = {
#include "stg40/terrain/collision_163_meshsets.inc"
};
static NJS_POINT3 collision_163_points[33] = {
#include "stg40/terrain/collision_163_points.inc"
};
static NJS_MODEL collision_163_model = {
#include "stg40/terrain/collision_163_model.inc"
};
static NJS_OBJECT collision_163 = {
#include "stg40/terrain/collision_163.inc"
};
static NJS_MATERIAL collision_164_materials[1] = {
#include "stg40/terrain/collision_164_materials.inc"
};
static Sint16 collision_164_faces[72] = {
#include "stg40/terrain/collision_164_faces.inc"
};
static NJS_MESHSET collision_164_meshsets[1] = {
#include "stg40/terrain/collision_164_meshsets.inc"
};
static NJS_POINT3 collision_164_points[40] = {
#include "stg40/terrain/collision_164_points.inc"
};
static NJS_MODEL collision_164_model = {
#include "stg40/terrain/collision_164_model.inc"
};
static NJS_OBJECT collision_164 = {
#include "stg40/terrain/collision_164.inc"
};
static NJS_MATERIAL collision_165_materials[1] = {
#include "stg40/terrain/collision_165_materials.inc"
};
static Sint16 collision_165_faces[44] = {
#include "stg40/terrain/collision_165_faces.inc"
};
static NJS_MESHSET collision_165_meshsets[1] = {
#include "stg40/terrain/collision_165_meshsets.inc"
};
static NJS_POINT3 collision_165_points[25] = {
#include "stg40/terrain/collision_165_points.inc"
};
static NJS_MODEL collision_165_model = {
#include "stg40/terrain/collision_165_model.inc"
};
static NJS_OBJECT collision_165 = {
#include "stg40/terrain/collision_165.inc"
};
static NJS_MATERIAL collision_166_materials[1] = {
#include "stg40/terrain/collision_166_materials.inc"
};
static Sint16 collision_166_faces[135] = {
#include "stg40/terrain/collision_166_faces.inc"
};
static NJS_MESHSET collision_166_meshsets[1] = {
#include "stg40/terrain/collision_166_meshsets.inc"
};
static NJS_POINT3 collision_166_points[59] = {
#include "stg40/terrain/collision_166_points.inc"
};
static NJS_MODEL collision_166_model = {
#include "stg40/terrain/collision_166_model.inc"
};
static NJS_OBJECT collision_166 = {
#include "stg40/terrain/collision_166.inc"
};
static NJS_MATERIAL collision_167_materials[1] = {
#include "stg40/terrain/collision_167_materials.inc"
};
static Sint16 collision_167_faces[155] = {
#include "stg40/terrain/collision_167_faces.inc"
};
static NJS_MESHSET collision_167_meshsets[1] = {
#include "stg40/terrain/collision_167_meshsets.inc"
};
static NJS_POINT3 collision_167_points[69] = {
#include "stg40/terrain/collision_167_points.inc"
};
static NJS_MODEL collision_167_model = {
#include "stg40/terrain/collision_167_model.inc"
};
static NJS_OBJECT collision_167 = {
#include "stg40/terrain/collision_167.inc"
};
static NJS_MATERIAL collision_168_materials[1] = {
#include "stg40/terrain/collision_168_materials.inc"
};
static Sint16 collision_168_faces[71] = {
#include "stg40/terrain/collision_168_faces.inc"
};
static NJS_MESHSET collision_168_meshsets[1] = {
#include "stg40/terrain/collision_168_meshsets.inc"
};
static NJS_POINT3 collision_168_points[34] = {
#include "stg40/terrain/collision_168_points.inc"
};
static NJS_MODEL collision_168_model = {
#include "stg40/terrain/collision_168_model.inc"
};
static NJS_OBJECT collision_168 = {
#include "stg40/terrain/collision_168.inc"
};
static NJS_MATERIAL collision_169_materials[1] = {
#include "stg40/terrain/collision_169_materials.inc"
};
static Sint16 collision_169_faces[50] = {
#include "stg40/terrain/collision_169_faces.inc"
};
static NJS_MESHSET collision_169_meshsets[1] = {
#include "stg40/terrain/collision_169_meshsets.inc"
};
static NJS_POINT3 collision_169_points[25] = {
#include "stg40/terrain/collision_169_points.inc"
};
static NJS_MODEL collision_169_model = {
#include "stg40/terrain/collision_169_model.inc"
};
static NJS_OBJECT collision_169 = {
#include "stg40/terrain/collision_169.inc"
};
static NJS_MATERIAL collision_170_materials[1] = {
#include "stg40/terrain/collision_170_materials.inc"
};
static Sint16 collision_170_faces[137] = {
#include "stg40/terrain/collision_170_faces.inc"
};
static NJS_MESHSET collision_170_meshsets[1] = {
#include "stg40/terrain/collision_170_meshsets.inc"
};
static NJS_POINT3 collision_170_points[65] = {
#include "stg40/terrain/collision_170_points.inc"
};
static NJS_MODEL collision_170_model = {
#include "stg40/terrain/collision_170_model.inc"
};
static NJS_OBJECT collision_170 = {
#include "stg40/terrain/collision_170.inc"
};
static NJS_MATERIAL collision_171_materials[1] = {
#include "stg40/terrain/collision_171_materials.inc"
};
static Sint16 collision_171_faces[154] = {
#include "stg40/terrain/collision_171_faces.inc"
};
static NJS_MESHSET collision_171_meshsets[1] = {
#include "stg40/terrain/collision_171_meshsets.inc"
};
static NJS_POINT3 collision_171_points[68] = {
#include "stg40/terrain/collision_171_points.inc"
};
static NJS_MODEL collision_171_model = {
#include "stg40/terrain/collision_171_model.inc"
};
static NJS_OBJECT collision_171 = {
#include "stg40/terrain/collision_171.inc"
};
static NJS_MATERIAL collision_172_materials[1] = {
#include "stg40/terrain/collision_172_materials.inc"
};
static Sint16 collision_172_faces[162] = {
#include "stg40/terrain/collision_172_faces.inc"
};
static NJS_MESHSET collision_172_meshsets[1] = {
#include "stg40/terrain/collision_172_meshsets.inc"
};
static NJS_POINT3 collision_172_points[77] = {
#include "stg40/terrain/collision_172_points.inc"
};
static NJS_MODEL collision_172_model = {
#include "stg40/terrain/collision_172_model.inc"
};
static NJS_OBJECT collision_172 = {
#include "stg40/terrain/collision_172.inc"
};
static NJS_MATERIAL collision_173_materials[1] = {
#include "stg40/terrain/collision_173_materials.inc"
};
static Sint16 collision_173_faces[67] = {
#include "stg40/terrain/collision_173_faces.inc"
};
static NJS_MESHSET collision_173_meshsets[1] = {
#include "stg40/terrain/collision_173_meshsets.inc"
};
static NJS_POINT3 collision_173_points[33] = {
#include "stg40/terrain/collision_173_points.inc"
};
static NJS_MODEL collision_173_model = {
#include "stg40/terrain/collision_173_model.inc"
};
static NJS_OBJECT collision_173 = {
#include "stg40/terrain/collision_173.inc"
};
static NJS_MATERIAL collision_174_materials[1] = {
#include "stg40/terrain/collision_174_materials.inc"
};
static Sint16 collision_174_faces[64] = {
#include "stg40/terrain/collision_174_faces.inc"
};
static NJS_MESHSET collision_174_meshsets[1] = {
#include "stg40/terrain/collision_174_meshsets.inc"
};
static NJS_POINT3 collision_174_points[33] = {
#include "stg40/terrain/collision_174_points.inc"
};
static NJS_MODEL collision_174_model = {
#include "stg40/terrain/collision_174_model.inc"
};
static NJS_OBJECT collision_174 = {
#include "stg40/terrain/collision_174.inc"
};
static NJS_MATERIAL collision_175_materials[1] = {
#include "stg40/terrain/collision_175_materials.inc"
};
static Sint16 collision_175_faces[145] = {
#include "stg40/terrain/collision_175_faces.inc"
};
static NJS_MESHSET collision_175_meshsets[1] = {
#include "stg40/terrain/collision_175_meshsets.inc"
};
static NJS_POINT3 collision_175_points[66] = {
#include "stg40/terrain/collision_175_points.inc"
};
static NJS_MODEL collision_175_model = {
#include "stg40/terrain/collision_175_model.inc"
};
static NJS_OBJECT collision_175 = {
#include "stg40/terrain/collision_175.inc"
};
static NJS_MATERIAL collision_176_materials[1] = {
#include "stg40/terrain/collision_176_materials.inc"
};
static Sint16 collision_176_faces[67] = {
#include "stg40/terrain/collision_176_faces.inc"
};
static NJS_MESHSET collision_176_meshsets[1] = {
#include "stg40/terrain/collision_176_meshsets.inc"
};
static NJS_POINT3 collision_176_points[33] = {
#include "stg40/terrain/collision_176_points.inc"
};
static NJS_MODEL collision_176_model = {
#include "stg40/terrain/collision_176_model.inc"
};
static NJS_OBJECT collision_176 = {
#include "stg40/terrain/collision_176.inc"
};
static NJS_MATERIAL collision_177_materials[1] = {
#include "stg40/terrain/collision_177_materials.inc"
};
static Sint16 collision_177_faces[63] = {
#include "stg40/terrain/collision_177_faces.inc"
};
static NJS_MESHSET collision_177_meshsets[1] = {
#include "stg40/terrain/collision_177_meshsets.inc"
};
static NJS_POINT3 collision_177_points[36] = {
#include "stg40/terrain/collision_177_points.inc"
};
static NJS_MODEL collision_177_model = {
#include "stg40/terrain/collision_177_model.inc"
};
static NJS_OBJECT collision_177 = {
#include "stg40/terrain/collision_177.inc"
};
static NJS_MATERIAL collision_178_materials[1] = {
#include "stg40/terrain/collision_178_materials.inc"
};
static Sint16 collision_178_faces[63] = {
#include "stg40/terrain/collision_178_faces.inc"
};
static NJS_MESHSET collision_178_meshsets[1] = {
#include "stg40/terrain/collision_178_meshsets.inc"
};
static NJS_POINT3 collision_178_points[36] = {
#include "stg40/terrain/collision_178_points.inc"
};
static NJS_MODEL collision_178_model = {
#include "stg40/terrain/collision_178_model.inc"
};
static NJS_OBJECT collision_178 = {
#include "stg40/terrain/collision_178.inc"
};
static NJS_MATERIAL collision_181_materials[1] = {
#include "stg40/terrain/collision_181_materials.inc"
};
static Sint16 collision_181_faces[67] = {
#include "stg40/terrain/collision_181_faces.inc"
};
static NJS_MESHSET collision_181_meshsets[1] = {
#include "stg40/terrain/collision_181_meshsets.inc"
};
static NJS_POINT3 collision_181_points[33] = {
#include "stg40/terrain/collision_181_points.inc"
};
static NJS_MODEL collision_181_model = {
#include "stg40/terrain/collision_181_model.inc"
};
static NJS_OBJECT collision_181 = {
#include "stg40/terrain/collision_181.inc"
};
static NJS_MATERIAL collision_182_materials[1] = {
#include "stg40/terrain/collision_182_materials.inc"
};
static Sint16 collision_182_faces[67] = {
#include "stg40/terrain/collision_182_faces.inc"
};
static NJS_MESHSET collision_182_meshsets[1] = {
#include "stg40/terrain/collision_182_meshsets.inc"
};
static NJS_POINT3 collision_182_points[33] = {
#include "stg40/terrain/collision_182_points.inc"
};
static NJS_MODEL collision_182_model = {
#include "stg40/terrain/collision_182_model.inc"
};
static NJS_OBJECT collision_182 = {
#include "stg40/terrain/collision_182.inc"
};
static NJS_MATERIAL collision_183_materials[1] = {
#include "stg40/terrain/collision_183_materials.inc"
};
static Sint16 collision_183_faces[36] = {
#include "stg40/terrain/collision_183_faces.inc"
};
static NJS_MESHSET collision_183_meshsets[1] = {
#include "stg40/terrain/collision_183_meshsets.inc"
};
static NJS_POINT3 collision_183_points[20] = {
#include "stg40/terrain/collision_183_points.inc"
};
static NJS_MODEL collision_183_model = {
#include "stg40/terrain/collision_183_model.inc"
};
static NJS_OBJECT collision_183 = {
#include "stg40/terrain/collision_183.inc"
};
static NJS_MATERIAL collision_184_materials[1] = {
#include "stg40/terrain/collision_184_materials.inc"
};
static Sint16 collision_184_faces[36] = {
#include "stg40/terrain/collision_184_faces.inc"
};
static NJS_MESHSET collision_184_meshsets[1] = {
#include "stg40/terrain/collision_184_meshsets.inc"
};
static NJS_POINT3 collision_184_points[20] = {
#include "stg40/terrain/collision_184_points.inc"
};
static NJS_MODEL collision_184_model = {
#include "stg40/terrain/collision_184_model.inc"
};
static NJS_OBJECT collision_184 = {
#include "stg40/terrain/collision_184.inc"
};
static NJS_MATERIAL collision_185_materials[1] = {
#include "stg40/terrain/collision_185_materials.inc"
};
static Sint16 collision_185_faces[106] = {
#include "stg40/terrain/collision_185_faces.inc"
};
static NJS_MESHSET collision_185_meshsets[1] = {
#include "stg40/terrain/collision_185_meshsets.inc"
};
static NJS_POINT3 collision_185_points[59] = {
#include "stg40/terrain/collision_185_points.inc"
};
static NJS_MODEL collision_185_model = {
#include "stg40/terrain/collision_185_model.inc"
};
static NJS_OBJECT collision_185 = {
#include "stg40/terrain/collision_185.inc"
};
static NJS_MATERIAL collision_186_materials[1] = {
#include "stg40/terrain/collision_186_materials.inc"
};
static Sint16 collision_186_faces[116] = {
#include "stg40/terrain/collision_186_faces.inc"
};
static NJS_MESHSET collision_186_meshsets[1] = {
#include "stg40/terrain/collision_186_meshsets.inc"
};
static NJS_POINT3 collision_186_points[57] = {
#include "stg40/terrain/collision_186_points.inc"
};
static NJS_MODEL collision_186_model = {
#include "stg40/terrain/collision_186_model.inc"
};
static NJS_OBJECT collision_186 = {
#include "stg40/terrain/collision_186.inc"
};
static NJS_MATERIAL collision_187_materials[1] = {
#include "stg40/terrain/collision_187_materials.inc"
};
static Sint16 collision_187_faces[57] = {
#include "stg40/terrain/collision_187_faces.inc"
};
static NJS_MESHSET collision_187_meshsets[1] = {
#include "stg40/terrain/collision_187_meshsets.inc"
};
static NJS_POINT3 collision_187_points[26] = {
#include "stg40/terrain/collision_187_points.inc"
};
static NJS_MODEL collision_187_model = {
#include "stg40/terrain/collision_187_model.inc"
};
static NJS_OBJECT collision_187 = {
#include "stg40/terrain/collision_187.inc"
};
static NJS_MATERIAL collision_189_materials[1] = {
#include "stg40/terrain/collision_189_materials.inc"
};
static Sint16 collision_189_faces[72] = {
#include "stg40/terrain/collision_189_faces.inc"
};
static NJS_MESHSET collision_189_meshsets[1] = {
#include "stg40/terrain/collision_189_meshsets.inc"
};
static NJS_POINT3 collision_189_points[33] = {
#include "stg40/terrain/collision_189_points.inc"
};
static NJS_MODEL collision_189_model = {
#include "stg40/terrain/collision_189_model.inc"
};
static NJS_OBJECT collision_189 = {
#include "stg40/terrain/collision_189.inc"
};
static NJS_MATERIAL collision_213_materials[1] = {
#include "stg40/terrain/collision_213_materials.inc"
};
static Sint16 collision_213_faces[97] = {
#include "stg40/terrain/collision_213_faces.inc"
};
static NJS_MESHSET collision_213_meshsets[1] = {
#include "stg40/terrain/collision_213_meshsets.inc"
};
static NJS_POINT3 collision_213_points[42] = {
#include "stg40/terrain/collision_213_points.inc"
};
static NJS_MODEL collision_213_model = {
#include "stg40/terrain/collision_213_model.inc"
};
static NJS_OBJECT collision_213 = {
#include "stg40/terrain/collision_213.inc"
};
static NJS_MATERIAL collision_222_materials[1] = {
#include "stg40/terrain/collision_222_materials.inc"
};
static Sint16 collision_222_faces[83] = {
#include "stg40/terrain/collision_222_faces.inc"
};
static NJS_MESHSET collision_222_meshsets[1] = {
#include "stg40/terrain/collision_222_meshsets.inc"
};
static NJS_POINT3 collision_222_points[40] = {
#include "stg40/terrain/collision_222_points.inc"
};
static NJS_MODEL collision_222_model = {
#include "stg40/terrain/collision_222_model.inc"
};
static NJS_OBJECT collision_222 = {
#include "stg40/terrain/collision_222.inc"
};
static NJS_MATERIAL collision_224_materials[1] = {
#include "stg40/terrain/collision_224_materials.inc"
};
static Sint16 collision_224_faces[47] = {
#include "stg40/terrain/collision_224_faces.inc"
};
static NJS_MESHSET collision_224_meshsets[1] = {
#include "stg40/terrain/collision_224_meshsets.inc"
};
static NJS_POINT3 collision_224_points[25] = {
#include "stg40/terrain/collision_224_points.inc"
};
static NJS_MODEL collision_224_model = {
#include "stg40/terrain/collision_224_model.inc"
};
static NJS_OBJECT collision_224 = {
#include "stg40/terrain/collision_224.inc"
};
static NJS_MATERIAL collision_225_materials[1] = {
#include "stg40/terrain/collision_225_materials.inc"
};
static Sint16 collision_225_faces[50] = {
#include "stg40/terrain/collision_225_faces.inc"
};
static NJS_MESHSET collision_225_meshsets[1] = {
#include "stg40/terrain/collision_225_meshsets.inc"
};
static NJS_POINT3 collision_225_points[25] = {
#include "stg40/terrain/collision_225_points.inc"
};
static NJS_MODEL collision_225_model = {
#include "stg40/terrain/collision_225_model.inc"
};
static NJS_OBJECT collision_225 = {
#include "stg40/terrain/collision_225.inc"
};
static NJS_MATERIAL collision_227_materials[1] = {
#include "stg40/terrain/collision_227_materials.inc"
};
static Sint16 collision_227_faces[63] = {
#include "stg40/terrain/collision_227_faces.inc"
};
static NJS_MESHSET collision_227_meshsets[1] = {
#include "stg40/terrain/collision_227_meshsets.inc"
};
static NJS_POINT3 collision_227_points[35] = {
#include "stg40/terrain/collision_227_points.inc"
};
static NJS_MODEL collision_227_model = {
#include "stg40/terrain/collision_227_model.inc"
};
static NJS_OBJECT collision_227 = {
#include "stg40/terrain/collision_227.inc"
};
static NJS_MATERIAL collision_229_materials[1] = {
#include "stg40/terrain/collision_229_materials.inc"
};
static Sint16 collision_229_faces[34] = {
#include "stg40/terrain/collision_229_faces.inc"
};
static NJS_MESHSET collision_229_meshsets[1] = {
#include "stg40/terrain/collision_229_meshsets.inc"
};
static NJS_POINT3 collision_229_points[16] = {
#include "stg40/terrain/collision_229_points.inc"
};
static NJS_MODEL collision_229_model = {
#include "stg40/terrain/collision_229_model.inc"
};
static NJS_OBJECT collision_229 = {
#include "stg40/terrain/collision_229.inc"
};
static NJS_MATERIAL collision_232_materials[1] = {
#include "stg40/terrain/collision_232_materials.inc"
};
static Sint16 collision_232_faces[28] = {
#include "stg40/terrain/collision_232_faces.inc"
};
static NJS_MESHSET collision_232_meshsets[1] = {
#include "stg40/terrain/collision_232_meshsets.inc"
};
static NJS_POINT3 collision_232_points[18] = {
#include "stg40/terrain/collision_232_points.inc"
};
static NJS_MODEL collision_232_model = {
#include "stg40/terrain/collision_232_model.inc"
};
static NJS_OBJECT collision_232 = {
#include "stg40/terrain/collision_232.inc"
};
static NJS_MATERIAL collision_234_materials[1] = {
#include "stg40/terrain/collision_234_materials.inc"
};
static Sint16 collision_234_faces[40] = {
#include "stg40/terrain/collision_234_faces.inc"
};
static NJS_MESHSET collision_234_meshsets[1] = {
#include "stg40/terrain/collision_234_meshsets.inc"
};
static NJS_POINT3 collision_234_points[24] = {
#include "stg40/terrain/collision_234_points.inc"
};
static NJS_MODEL collision_234_model = {
#include "stg40/terrain/collision_234_model.inc"
};
static NJS_OBJECT collision_234 = {
#include "stg40/terrain/collision_234.inc"
};
static NJS_MATERIAL collision_236_materials[1] = {
#include "stg40/terrain/collision_236_materials.inc"
};
static Sint16 collision_236_faces[59] = {
#include "stg40/terrain/collision_236_faces.inc"
};
static NJS_MESHSET collision_236_meshsets[1] = {
#include "stg40/terrain/collision_236_meshsets.inc"
};
static NJS_POINT3 collision_236_points[24] = {
#include "stg40/terrain/collision_236_points.inc"
};
static NJS_MODEL collision_236_model = {
#include "stg40/terrain/collision_236_model.inc"
};
static NJS_OBJECT collision_236 = {
#include "stg40/terrain/collision_236.inc"
};
static NJS_MATERIAL collision_237_materials[1] = {
#include "stg40/terrain/collision_237_materials.inc"
};
static Sint16 collision_237_faces[59] = {
#include "stg40/terrain/collision_237_faces.inc"
};
static NJS_MESHSET collision_237_meshsets[1] = {
#include "stg40/terrain/collision_237_meshsets.inc"
};
static NJS_POINT3 collision_237_points[24] = {
#include "stg40/terrain/collision_237_points.inc"
};
static NJS_MODEL collision_237_model = {
#include "stg40/terrain/collision_237_model.inc"
};
static NJS_OBJECT collision_237 = {
#include "stg40/terrain/collision_237.inc"
};
static NJS_MATERIAL collision_274_materials[1] = {
#include "stg40/terrain/collision_274_materials.inc"
};
static Sint16 collision_274_faces[28] = {
#include "stg40/terrain/collision_274_faces.inc"
};
static NJS_MESHSET collision_274_meshsets[1] = {
#include "stg40/terrain/collision_274_meshsets.inc"
};
static NJS_POINT3 collision_274_points[18] = {
#include "stg40/terrain/collision_274_points.inc"
};
static NJS_MODEL collision_274_model = {
#include "stg40/terrain/collision_274_model.inc"
};
static NJS_OBJECT collision_274 = {
#include "stg40/terrain/collision_274.inc"
};
static NJS_MATERIAL collision_275_materials[1] = {
#include "stg40/terrain/collision_275_materials.inc"
};
static Sint16 collision_275_faces[40] = {
#include "stg40/terrain/collision_275_faces.inc"
};
static NJS_MESHSET collision_275_meshsets[1] = {
#include "stg40/terrain/collision_275_meshsets.inc"
};
static NJS_POINT3 collision_275_points[24] = {
#include "stg40/terrain/collision_275_points.inc"
};
static NJS_MODEL collision_275_model = {
#include "stg40/terrain/collision_275_model.inc"
};
static NJS_OBJECT collision_275 = {
#include "stg40/terrain/collision_275.inc"
};
static NJS_MATERIAL collision_276_materials[1] = {
#include "stg40/terrain/collision_276_materials.inc"
};
static Sint16 collision_276_faces[52] = {
#include "stg40/terrain/collision_276_faces.inc"
};
static NJS_MESHSET collision_276_meshsets[1] = {
#include "stg40/terrain/collision_276_meshsets.inc"
};
static NJS_POINT3 collision_276_points[24] = {
#include "stg40/terrain/collision_276_points.inc"
};
static NJS_MODEL collision_276_model = {
#include "stg40/terrain/collision_276_model.inc"
};
static NJS_OBJECT collision_276 = {
#include "stg40/terrain/collision_276.inc"
};
static NJS_MATERIAL collision_277_materials[1] = {
#include "stg40/terrain/collision_277_materials.inc"
};
static Sint16 collision_277_faces[43] = {
#include "stg40/terrain/collision_277_faces.inc"
};
static NJS_MESHSET collision_277_meshsets[1] = {
#include "stg40/terrain/collision_277_meshsets.inc"
};
static NJS_POINT3 collision_277_points[24] = {
#include "stg40/terrain/collision_277_points.inc"
};
static NJS_MODEL collision_277_model = {
#include "stg40/terrain/collision_277_model.inc"
};
static NJS_OBJECT collision_277 = {
#include "stg40/terrain/collision_277.inc"
};
static NJS_MATERIAL collision_278_materials[1] = {
#include "stg40/terrain/collision_278_materials.inc"
};
static Sint16 collision_278_faces[29] = {
#include "stg40/terrain/collision_278_faces.inc"
};
static NJS_MESHSET collision_278_meshsets[1] = {
#include "stg40/terrain/collision_278_meshsets.inc"
};
static NJS_POINT3 collision_278_points[16] = {
#include "stg40/terrain/collision_278_points.inc"
};
static NJS_MODEL collision_278_model = {
#include "stg40/terrain/collision_278_model.inc"
};
static NJS_OBJECT collision_278 = {
#include "stg40/terrain/collision_278.inc"
};
static NJS_MATERIAL collision_280_materials[1] = {
#include "stg40/terrain/collision_280_materials.inc"
};
static Sint16 collision_280_faces[43] = {
#include "stg40/terrain/collision_280_faces.inc"
};
static NJS_MESHSET collision_280_meshsets[1] = {
#include "stg40/terrain/collision_280_meshsets.inc"
};
static NJS_POINT3 collision_280_points[24] = {
#include "stg40/terrain/collision_280_points.inc"
};
static NJS_MODEL collision_280_model = {
#include "stg40/terrain/collision_280_model.inc"
};
static NJS_OBJECT collision_280 = {
#include "stg40/terrain/collision_280.inc"
};
static NJS_MATERIAL collision_281_materials[1] = {
#include "stg40/terrain/collision_281_materials.inc"
};
static Sint16 collision_281_faces[142] = {
#include "stg40/terrain/collision_281_faces.inc"
};
static NJS_MESHSET collision_281_meshsets[1] = {
#include "stg40/terrain/collision_281_meshsets.inc"
};
static NJS_POINT3 collision_281_points[52] = {
#include "stg40/terrain/collision_281_points.inc"
};
static NJS_MODEL collision_281_model = {
#include "stg40/terrain/collision_281_model.inc"
};
static NJS_OBJECT collision_281 = {
#include "stg40/terrain/collision_281.inc"
};
static NJS_MATERIAL collision_285_materials[1] = {
#include "stg40/terrain/collision_285_materials.inc"
};
static Sint16 collision_285_faces[30] = {
#include "stg40/terrain/collision_285_faces.inc"
};
static NJS_MESHSET collision_285_meshsets[1] = {
#include "stg40/terrain/collision_285_meshsets.inc"
};
static NJS_POINT3 collision_285_points[16] = {
#include "stg40/terrain/collision_285_points.inc"
};
static NJS_MODEL collision_285_model = {
#include "stg40/terrain/collision_285_model.inc"
};
static NJS_OBJECT collision_285 = {
#include "stg40/terrain/collision_285.inc"
};
static NJS_MATERIAL collision_287_materials[1] = {
#include "stg40/terrain/collision_287_materials.inc"
};
static Sint16 collision_287_faces[9] = {
#include "stg40/terrain/collision_287_faces.inc"
};
static NJS_MESHSET collision_287_meshsets[1] = {
#include "stg40/terrain/collision_287_meshsets.inc"
};
static NJS_POINT3 collision_287_points[8] = {
#include "stg40/terrain/collision_287_points.inc"
};
static NJS_MODEL collision_287_model = {
#include "stg40/terrain/collision_287_model.inc"
};
static NJS_OBJECT collision_287 = {
#include "stg40/terrain/collision_287.inc"
};
static NJS_MATERIAL collision_289_materials[1] = {
#include "stg40/terrain/collision_289_materials.inc"
};
static Sint16 collision_289_faces[168] = {
#include "stg40/terrain/collision_289_faces.inc"
};
static NJS_MESHSET collision_289_meshsets[1] = {
#include "stg40/terrain/collision_289_meshsets.inc"
};
static NJS_POINT3 collision_289_points[81] = {
#include "stg40/terrain/collision_289_points.inc"
};
static NJS_MODEL collision_289_model = {
#include "stg40/terrain/collision_289_model.inc"
};
static NJS_OBJECT collision_289 = {
#include "stg40/terrain/collision_289.inc"
};
static NJS_MATERIAL collision_290_materials[1] = {
#include "stg40/terrain/collision_290_materials.inc"
};
static Sint16 collision_290_faces[46] = {
#include "stg40/terrain/collision_290_faces.inc"
};
static NJS_MESHSET collision_290_meshsets[1] = {
#include "stg40/terrain/collision_290_meshsets.inc"
};
static NJS_POINT3 collision_290_points[26] = {
#include "stg40/terrain/collision_290_points.inc"
};
static NJS_MODEL collision_290_model = {
#include "stg40/terrain/collision_290_model.inc"
};
static NJS_OBJECT collision_290 = {
#include "stg40/terrain/collision_290.inc"
};
static NJS_MATERIAL collision_292_materials[1] = {
#include "stg40/terrain/collision_292_materials.inc"
};
static Sint16 collision_292_faces[25] = {
#include "stg40/terrain/collision_292_faces.inc"
};
static NJS_MESHSET collision_292_meshsets[1] = {
#include "stg40/terrain/collision_292_meshsets.inc"
};
static NJS_POINT3 collision_292_points[10] = {
#include "stg40/terrain/collision_292_points.inc"
};
static NJS_MODEL collision_292_model = {
#include "stg40/terrain/collision_292_model.inc"
};
static NJS_OBJECT collision_292 = {
#include "stg40/terrain/collision_292.inc"
};
static NJS_MATERIAL collision_364_materials[1] = {
#include "stg40/terrain/collision_364_materials.inc"
};
static Sint16 collision_364_faces[7] = {
#include "stg40/terrain/collision_364_faces.inc"
};
static NJS_MESHSET collision_364_meshsets[1] = {
#include "stg40/terrain/collision_364_meshsets.inc"
};
static NJS_POINT3 collision_364_points[6] = {
#include "stg40/terrain/collision_364_points.inc"
};
static NJS_MODEL collision_364_model = {
#include "stg40/terrain/collision_364_model.inc"
};
static NJS_OBJECT collision_364 = {
#include "stg40/terrain/collision_364.inc"
};
static NJS_MATERIAL collision_389_materials[1] = {
#include "stg40/terrain/collision_389_materials.inc"
};
static Sint16 collision_389_faces[28] = {
#include "stg40/terrain/collision_389_faces.inc"
};
static NJS_MESHSET collision_389_meshsets[1] = {
#include "stg40/terrain/collision_389_meshsets.inc"
};
static NJS_POINT3 collision_389_points[18] = {
#include "stg40/terrain/collision_389_points.inc"
};
static NJS_MODEL collision_389_model = {
#include "stg40/terrain/collision_389_model.inc"
};
static NJS_OBJECT collision_389 = {
#include "stg40/terrain/collision_389.inc"
};
static NJS_MATERIAL collision_391_materials[1] = {
#include "stg40/terrain/collision_391_materials.inc"
};
static Sint16 collision_391_faces[40] = {
#include "stg40/terrain/collision_391_faces.inc"
};
static NJS_MESHSET collision_391_meshsets[1] = {
#include "stg40/terrain/collision_391_meshsets.inc"
};
static NJS_POINT3 collision_391_points[24] = {
#include "stg40/terrain/collision_391_points.inc"
};
static NJS_MODEL collision_391_model = {
#include "stg40/terrain/collision_391_model.inc"
};
static NJS_OBJECT collision_391 = {
#include "stg40/terrain/collision_391.inc"
};
static NJS_CNK_OBJECT draw_534 = {
#include "stg40/terrain/draw_534.inc"
};
static NJS_CNK_OBJECT draw_531 = {
#include "stg40/terrain/draw_531.inc"
};
static NJS_CNK_OBJECT draw_530 = {
#include "stg40/terrain/draw_530.inc"
};
static NJS_CNK_OBJECT draw_529 = {
#include "stg40/terrain/draw_529.inc"
};
static NJS_CNK_OBJECT draw_528 = {
#include "stg40/terrain/draw_528.inc"
};
static NJS_CNK_OBJECT draw_527 = {
#include "stg40/terrain/draw_527.inc"
};
static NJS_CNK_OBJECT draw_526 = {
#include "stg40/terrain/draw_526.inc"
};
static NJS_CNK_OBJECT draw_525 = {
#include "stg40/terrain/draw_525.inc"
};
static NJS_CNK_OBJECT draw_524 = {
#include "stg40/terrain/draw_524.inc"
};
static NJS_CNK_OBJECT draw_523 = {
#include "stg40/terrain/draw_523.inc"
};
static NJS_CNK_OBJECT draw_522 = {
#include "stg40/terrain/draw_522.inc"
};
static NJS_CNK_OBJECT draw_521 = {
#include "stg40/terrain/draw_521.inc"
};
static NJS_CNK_OBJECT draw_520 = {
#include "stg40/terrain/draw_520.inc"
};
static NJS_CNK_OBJECT draw_519 = {
#include "stg40/terrain/draw_519.inc"
};
static NJS_CNK_OBJECT draw_518 = {
#include "stg40/terrain/draw_518.inc"
};
static NJS_CNK_OBJECT draw_517 = {
#include "stg40/terrain/draw_517.inc"
};
static NJS_CNK_OBJECT draw_516 = {
#include "stg40/terrain/draw_516.inc"
};
static NJS_CNK_OBJECT draw_515 = {
#include "stg40/terrain/draw_515.inc"
};
static NJS_CNK_OBJECT draw_514 = {
#include "stg40/terrain/draw_514.inc"
};
static NJS_CNK_OBJECT draw_513 = {
#include "stg40/terrain/draw_513.inc"
};
static NJS_CNK_OBJECT draw_512 = {
#include "stg40/terrain/draw_512.inc"
};
static NJS_CNK_OBJECT draw_511 = {
#include "stg40/terrain/draw_511.inc"
};
static NJS_CNK_OBJECT draw_510 = {
#include "stg40/terrain/draw_510.inc"
};
static NJS_CNK_OBJECT draw_509 = {
#include "stg40/terrain/draw_509.inc"
};
static NJS_CNK_OBJECT draw_507 = {
#include "stg40/terrain/draw_507.inc"
};
static NJS_CNK_OBJECT draw_506 = {
#include "stg40/terrain/draw_506.inc"
};
static NJS_CNK_OBJECT draw_505 = {
#include "stg40/terrain/draw_505.inc"
};
static NJS_CNK_OBJECT draw_504 = {
#include "stg40/terrain/draw_504.inc"
};
static NJS_CNK_OBJECT draw_503 = {
#include "stg40/terrain/draw_503.inc"
};
static NJS_CNK_OBJECT draw_502 = {
#include "stg40/terrain/draw_502.inc"
};
static NJS_CNK_OBJECT draw_501 = {
#include "stg40/terrain/draw_501.inc"
};
static NJS_CNK_OBJECT draw_500 = {
#include "stg40/terrain/draw_500.inc"
};
static NJS_CNK_OBJECT draw_499 = {
#include "stg40/terrain/draw_499.inc"
};
static NJS_CNK_OBJECT draw_498 = {
#include "stg40/terrain/draw_498.inc"
};
static NJS_CNK_OBJECT draw_497 = {
#include "stg40/terrain/draw_497.inc"
};
static NJS_CNK_OBJECT draw_493 = {
#include "stg40/terrain/draw_493.inc"
};
static NJS_CNK_OBJECT draw_492 = {
#include "stg40/terrain/draw_492.inc"
};
static NJS_CNK_OBJECT draw_491 = {
#include "stg40/terrain/draw_491.inc"
};
static NJS_CNK_OBJECT draw_490 = {
#include "stg40/terrain/draw_490.inc"
};
static NJS_CNK_OBJECT draw_489 = {
#include "stg40/terrain/draw_489.inc"
};
static NJS_CNK_OBJECT draw_487 = {
#include "stg40/terrain/draw_487.inc"
};
static NJS_CNK_OBJECT draw_486 = {
#include "stg40/terrain/draw_486.inc"
};
static NJS_CNK_OBJECT draw_485 = {
#include "stg40/terrain/draw_485.inc"
};
static NJS_CNK_OBJECT draw_484 = {
#include "stg40/terrain/draw_484.inc"
};
static NJS_CNK_OBJECT draw_483 = {
#include "stg40/terrain/draw_483.inc"
};
static NJS_CNK_OBJECT draw_482 = {
#include "stg40/terrain/draw_482.inc"
};
static NJS_CNK_OBJECT draw_481 = {
#include "stg40/terrain/draw_481.inc"
};
static NJS_CNK_OBJECT draw_480 = {
#include "stg40/terrain/draw_480.inc"
};
static NJS_CNK_OBJECT draw_479 = {
#include "stg40/terrain/draw_479.inc"
};
static NJS_CNK_OBJECT draw_478 = {
#include "stg40/terrain/draw_478.inc"
};
static NJS_CNK_OBJECT draw_477 = {
#include "stg40/terrain/draw_477.inc"
};
static NJS_CNK_OBJECT draw_475 = {
#include "stg40/terrain/draw_475.inc"
};
static NJS_CNK_OBJECT draw_474 = {
#include "stg40/terrain/draw_474.inc"
};
static NJS_CNK_OBJECT draw_473 = {
#include "stg40/terrain/draw_473.inc"
};
static NJS_CNK_OBJECT draw_472 = {
#include "stg40/terrain/draw_472.inc"
};
static NJS_CNK_OBJECT draw_471 = {
#include "stg40/terrain/draw_471.inc"
};
static NJS_CNK_OBJECT draw_470 = {
#include "stg40/terrain/draw_470.inc"
};
static NJS_CNK_OBJECT draw_469 = {
#include "stg40/terrain/draw_469.inc"
};
static NJS_CNK_OBJECT draw_466 = {
#include "stg40/terrain/draw_466.inc"
};
static NJS_CNK_OBJECT draw_465 = {
#include "stg40/terrain/draw_465.inc"
};
static NJS_CNK_OBJECT draw_464 = {
#include "stg40/terrain/draw_464.inc"
};
static NJS_CNK_OBJECT draw_463 = {
#include "stg40/terrain/draw_463.inc"
};
static NJS_CNK_OBJECT draw_462 = {
#include "stg40/terrain/draw_462.inc"
};
static NJS_CNK_OBJECT draw_461 = {
#include "stg40/terrain/draw_461.inc"
};
static NJS_CNK_OBJECT draw_460 = {
#include "stg40/terrain/draw_460.inc"
};
static NJS_CNK_OBJECT draw_459 = {
#include "stg40/terrain/draw_459.inc"
};
static NJS_CNK_OBJECT draw_458 = {
#include "stg40/terrain/draw_458.inc"
};
static NJS_CNK_OBJECT draw_457 = {
#include "stg40/terrain/draw_457.inc"
};
static NJS_CNK_OBJECT draw_456 = {
#include "stg40/terrain/draw_456.inc"
};
static NJS_CNK_OBJECT draw_455 = {
#include "stg40/terrain/draw_455.inc"
};
static NJS_CNK_OBJECT draw_454 = {
#include "stg40/terrain/draw_454.inc"
};
static NJS_CNK_OBJECT draw_453 = {
#include "stg40/terrain/draw_453.inc"
};
static NJS_CNK_OBJECT draw_452 = {
#include "stg40/terrain/draw_452.inc"
};
static NJS_CNK_OBJECT draw_451 = {
#include "stg40/terrain/draw_451.inc"
};
static NJS_CNK_OBJECT draw_450 = {
#include "stg40/terrain/draw_450.inc"
};
static NJS_CNK_OBJECT draw_449 = {
#include "stg40/terrain/draw_449.inc"
};
static NJS_CNK_OBJECT draw_448 = {
#include "stg40/terrain/draw_448.inc"
};
static NJS_CNK_OBJECT draw_447 = {
#include "stg40/terrain/draw_447.inc"
};
static NJS_CNK_OBJECT draw_446 = {
#include "stg40/terrain/draw_446.inc"
};
static NJS_CNK_OBJECT draw_445 = {
#include "stg40/terrain/draw_445.inc"
};
static NJS_CNK_OBJECT draw_444 = {
#include "stg40/terrain/draw_444.inc"
};
static NJS_CNK_OBJECT draw_443 = {
#include "stg40/terrain/draw_443.inc"
};
static NJS_CNK_OBJECT draw_442 = {
#include "stg40/terrain/draw_442.inc"
};
static NJS_CNK_OBJECT draw_441 = {
#include "stg40/terrain/draw_441.inc"
};
static NJS_CNK_OBJECT draw_440 = {
#include "stg40/terrain/draw_440.inc"
};
static NJS_CNK_OBJECT draw_439 = {
#include "stg40/terrain/draw_439.inc"
};
static NJS_CNK_OBJECT draw_438 = {
#include "stg40/terrain/draw_438.inc"
};
static NJS_CNK_OBJECT draw_437 = {
#include "stg40/terrain/draw_437.inc"
};
static NJS_CNK_OBJECT draw_436 = {
#include "stg40/terrain/draw_436.inc"
};
static NJS_CNK_OBJECT draw_435 = {
#include "stg40/terrain/draw_435.inc"
};
static NJS_CNK_OBJECT draw_434 = {
#include "stg40/terrain/draw_434.inc"
};
static NJS_CNK_OBJECT draw_433 = {
#include "stg40/terrain/draw_433.inc"
};
static NJS_CNK_OBJECT draw_432 = {
#include "stg40/terrain/draw_432.inc"
};
static NJS_CNK_OBJECT draw_431 = {
#include "stg40/terrain/draw_431.inc"
};
static NJS_CNK_OBJECT draw_430 = {
#include "stg40/terrain/draw_430.inc"
};
static NJS_CNK_OBJECT draw_429 = {
#include "stg40/terrain/draw_429.inc"
};
static NJS_CNK_OBJECT draw_428 = {
#include "stg40/terrain/draw_428.inc"
};
static NJS_CNK_OBJECT draw_427 = {
#include "stg40/terrain/draw_427.inc"
};
static NJS_CNK_OBJECT draw_426 = {
#include "stg40/terrain/draw_426.inc"
};
static NJS_CNK_OBJECT draw_425 = {
#include "stg40/terrain/draw_425.inc"
};
static NJS_CNK_OBJECT draw_424 = {
#include "stg40/terrain/draw_424.inc"
};
static NJS_CNK_OBJECT draw_423 = {
#include "stg40/terrain/draw_423.inc"
};
static NJS_CNK_OBJECT draw_422 = {
#include "stg40/terrain/draw_422.inc"
};
static NJS_CNK_OBJECT draw_421 = {
#include "stg40/terrain/draw_421.inc"
};
static NJS_CNK_OBJECT draw_420 = {
#include "stg40/terrain/draw_420.inc"
};
static NJS_CNK_OBJECT draw_419 = {
#include "stg40/terrain/draw_419.inc"
};
static NJS_CNK_OBJECT draw_418 = {
#include "stg40/terrain/draw_418.inc"
};
static NJS_CNK_OBJECT draw_417 = {
#include "stg40/terrain/draw_417.inc"
};
static NJS_CNK_OBJECT draw_416 = {
#include "stg40/terrain/draw_416.inc"
};
static NJS_CNK_OBJECT draw_415 = {
#include "stg40/terrain/draw_415.inc"
};
static NJS_CNK_OBJECT draw_414 = {
#include "stg40/terrain/draw_414.inc"
};
static NJS_CNK_OBJECT draw_413 = {
#include "stg40/terrain/draw_413.inc"
};
static NJS_CNK_OBJECT draw_412 = {
#include "stg40/terrain/draw_412.inc"
};
static NJS_CNK_OBJECT draw_411 = {
#include "stg40/terrain/draw_411.inc"
};
static NJS_CNK_OBJECT draw_410 = {
#include "stg40/terrain/draw_410.inc"
};
static NJS_CNK_OBJECT draw_409 = {
#include "stg40/terrain/draw_409.inc"
};
static NJS_CNK_OBJECT draw_408 = {
#include "stg40/terrain/draw_408.inc"
};
static NJS_CNK_OBJECT draw_407 = {
#include "stg40/terrain/draw_407.inc"
};
static NJS_CNK_OBJECT draw_406 = {
#include "stg40/terrain/draw_406.inc"
};
static NJS_CNK_OBJECT draw_405 = {
#include "stg40/terrain/draw_405.inc"
};
static NJS_CNK_OBJECT draw_404 = {
#include "stg40/terrain/draw_404.inc"
};
static NJS_CNK_OBJECT draw_403 = {
#include "stg40/terrain/draw_403.inc"
};
static NJS_CNK_OBJECT draw_400 = {
#include "stg40/terrain/draw_400.inc"
};
static NJS_CNK_OBJECT draw_399 = {
#include "stg40/terrain/draw_399.inc"
};
static NJS_CNK_OBJECT draw_398 = {
#include "stg40/terrain/draw_398.inc"
};
static NJS_CNK_OBJECT draw_397 = {
#include "stg40/terrain/draw_397.inc"
};
static NJS_CNK_OBJECT draw_396 = {
#include "stg40/terrain/draw_396.inc"
};
static NJS_CNK_OBJECT draw_395 = {
#include "stg40/terrain/draw_395.inc"
};
static NJS_CNK_OBJECT draw_394 = {
#include "stg40/terrain/draw_394.inc"
};
static NJS_CNK_OBJECT draw_393 = {
#include "stg40/terrain/draw_393.inc"
};
static NJS_CNK_OBJECT draw_392 = {
#include "stg40/terrain/draw_392.inc"
};
static NJS_CNK_OBJECT draw_388 = {
#include "stg40/terrain/draw_388.inc"
};
static NJS_CNK_OBJECT draw_387 = {
#include "stg40/terrain/draw_387.inc"
};
static NJS_CNK_OBJECT draw_386 = {
#include "stg40/terrain/draw_386.inc"
};
static NJS_CNK_OBJECT draw_385 = {
#include "stg40/terrain/draw_385.inc"
};
static NJS_CNK_OBJECT draw_384 = {
#include "stg40/terrain/draw_384.inc"
};
static NJS_CNK_OBJECT draw_383 = {
#include "stg40/terrain/draw_383.inc"
};
static NJS_CNK_OBJECT draw_382 = {
#include "stg40/terrain/draw_382.inc"
};
static NJS_CNK_OBJECT draw_381 = {
#include "stg40/terrain/draw_381.inc"
};
static NJS_CNK_OBJECT draw_380 = {
#include "stg40/terrain/draw_380.inc"
};
static NJS_CNK_OBJECT draw_379 = {
#include "stg40/terrain/draw_379.inc"
};
static NJS_CNK_OBJECT draw_378 = {
#include "stg40/terrain/draw_378.inc"
};
static NJS_CNK_OBJECT draw_376 = {
#include "stg40/terrain/draw_376.inc"
};
static NJS_CNK_OBJECT draw_375 = {
#include "stg40/terrain/draw_375.inc"
};
static NJS_CNK_OBJECT draw_374 = {
#include "stg40/terrain/draw_374.inc"
};
static NJS_CNK_OBJECT draw_373 = {
#include "stg40/terrain/draw_373.inc"
};
static NJS_CNK_OBJECT draw_372 = {
#include "stg40/terrain/draw_372.inc"
};
static NJS_CNK_OBJECT draw_371 = {
#include "stg40/terrain/draw_371.inc"
};
static NJS_CNK_OBJECT draw_369 = {
#include "stg40/terrain/draw_369.inc"
};
static NJS_CNK_OBJECT draw_368 = {
#include "stg40/terrain/draw_368.inc"
};
static NJS_CNK_OBJECT draw_367 = {
#include "stg40/terrain/draw_367.inc"
};
static NJS_CNK_OBJECT draw_366 = {
#include "stg40/terrain/draw_366.inc"
};
static NJS_CNK_OBJECT draw_365 = {
#include "stg40/terrain/draw_365.inc"
};
static NJS_CNK_OBJECT draw_364 = {
#include "stg40/terrain/draw_364.inc"
};
static NJS_CNK_OBJECT draw_363 = {
#include "stg40/terrain/draw_363.inc"
};
static NJS_CNK_OBJECT draw_362 = {
#include "stg40/terrain/draw_362.inc"
};
static NJS_CNK_OBJECT draw_361 = {
#include "stg40/terrain/draw_361.inc"
};
static NJS_CNK_OBJECT draw_360 = {
#include "stg40/terrain/draw_360.inc"
};
static NJS_CNK_OBJECT draw_359 = {
#include "stg40/terrain/draw_359.inc"
};
static NJS_CNK_OBJECT draw_358 = {
#include "stg40/terrain/draw_358.inc"
};
static NJS_CNK_OBJECT draw_357 = {
#include "stg40/terrain/draw_357.inc"
};
static NJS_CNK_OBJECT draw_356 = {
#include "stg40/terrain/draw_356.inc"
};
static NJS_CNK_OBJECT draw_355 = {
#include "stg40/terrain/draw_355.inc"
};
static NJS_CNK_OBJECT draw_354 = {
#include "stg40/terrain/draw_354.inc"
};
static NJS_CNK_OBJECT draw_353 = {
#include "stg40/terrain/draw_353.inc"
};
static NJS_CNK_OBJECT draw_352 = {
#include "stg40/terrain/draw_352.inc"
};
static NJS_CNK_OBJECT draw_351 = {
#include "stg40/terrain/draw_351.inc"
};
static NJS_CNK_OBJECT draw_350 = {
#include "stg40/terrain/draw_350.inc"
};
static NJS_CNK_OBJECT draw_349 = {
#include "stg40/terrain/draw_349.inc"
};
static NJS_CNK_OBJECT draw_348 = {
#include "stg40/terrain/draw_348.inc"
};
static NJS_CNK_OBJECT draw_347 = {
#include "stg40/terrain/draw_347.inc"
};
static NJS_CNK_OBJECT draw_346 = {
#include "stg40/terrain/draw_346.inc"
};
static NJS_CNK_OBJECT draw_345 = {
#include "stg40/terrain/draw_345.inc"
};
static NJS_CNK_OBJECT draw_344 = {
#include "stg40/terrain/draw_344.inc"
};
static NJS_CNK_OBJECT draw_343 = {
#include "stg40/terrain/draw_343.inc"
};
static NJS_CNK_OBJECT draw_342 = {
#include "stg40/terrain/draw_342.inc"
};
static NJS_CNK_OBJECT draw_341 = {
#include "stg40/terrain/draw_341.inc"
};
static NJS_CNK_OBJECT draw_340 = {
#include "stg40/terrain/draw_340.inc"
};
static NJS_CNK_OBJECT draw_339 = {
#include "stg40/terrain/draw_339.inc"
};
static NJS_CNK_OBJECT draw_338 = {
#include "stg40/terrain/draw_338.inc"
};
static NJS_CNK_OBJECT draw_337 = {
#include "stg40/terrain/draw_337.inc"
};
static NJS_CNK_OBJECT draw_336 = {
#include "stg40/terrain/draw_336.inc"
};
static NJS_CNK_OBJECT draw_335 = {
#include "stg40/terrain/draw_335.inc"
};
static NJS_CNK_OBJECT draw_334 = {
#include "stg40/terrain/draw_334.inc"
};
static NJS_CNK_OBJECT draw_333 = {
#include "stg40/terrain/draw_333.inc"
};
static NJS_CNK_OBJECT draw_332 = {
#include "stg40/terrain/draw_332.inc"
};
static NJS_CNK_OBJECT draw_331 = {
#include "stg40/terrain/draw_331.inc"
};
static NJS_CNK_OBJECT draw_330 = {
#include "stg40/terrain/draw_330.inc"
};
static NJS_CNK_OBJECT draw_329 = {
#include "stg40/terrain/draw_329.inc"
};
static NJS_CNK_OBJECT draw_328 = {
#include "stg40/terrain/draw_328.inc"
};
static NJS_CNK_OBJECT draw_327 = {
#include "stg40/terrain/draw_327.inc"
};
static NJS_CNK_OBJECT draw_326 = {
#include "stg40/terrain/draw_326.inc"
};
static NJS_CNK_OBJECT draw_325 = {
#include "stg40/terrain/draw_325.inc"
};
static NJS_CNK_OBJECT draw_324 = {
#include "stg40/terrain/draw_324.inc"
};
static NJS_CNK_OBJECT draw_323 = {
#include "stg40/terrain/draw_323.inc"
};
static NJS_CNK_OBJECT draw_322 = {
#include "stg40/terrain/draw_322.inc"
};
static NJS_CNK_OBJECT draw_321 = {
#include "stg40/terrain/draw_321.inc"
};
static NJS_CNK_OBJECT draw_320 = {
#include "stg40/terrain/draw_320.inc"
};
static NJS_CNK_OBJECT draw_319 = {
#include "stg40/terrain/draw_319.inc"
};
static NJS_CNK_OBJECT draw_318 = {
#include "stg40/terrain/draw_318.inc"
};
static NJS_CNK_OBJECT draw_317 = {
#include "stg40/terrain/draw_317.inc"
};
static NJS_CNK_OBJECT draw_316 = {
#include "stg40/terrain/draw_316.inc"
};
static NJS_CNK_OBJECT draw_315 = {
#include "stg40/terrain/draw_315.inc"
};
static NJS_CNK_OBJECT draw_314 = {
#include "stg40/terrain/draw_314.inc"
};
static NJS_CNK_OBJECT draw_313 = {
#include "stg40/terrain/draw_313.inc"
};
static NJS_CNK_OBJECT draw_312 = {
#include "stg40/terrain/draw_312.inc"
};
static NJS_CNK_OBJECT draw_311 = {
#include "stg40/terrain/draw_311.inc"
};
static NJS_CNK_OBJECT draw_310 = {
#include "stg40/terrain/draw_310.inc"
};
static NJS_CNK_OBJECT draw_309 = {
#include "stg40/terrain/draw_309.inc"
};
static NJS_CNK_OBJECT draw_308 = {
#include "stg40/terrain/draw_308.inc"
};
static NJS_CNK_OBJECT draw_307 = {
#include "stg40/terrain/draw_307.inc"
};
static NJS_CNK_OBJECT draw_306 = {
#include "stg40/terrain/draw_306.inc"
};
static NJS_CNK_OBJECT draw_304 = {
#include "stg40/terrain/draw_304.inc"
};
static NJS_CNK_OBJECT draw_303 = {
#include "stg40/terrain/draw_303.inc"
};
static NJS_CNK_OBJECT draw_302 = {
#include "stg40/terrain/draw_302.inc"
};
static NJS_CNK_OBJECT draw_301 = {
#include "stg40/terrain/draw_301.inc"
};
static NJS_CNK_OBJECT draw_300 = {
#include "stg40/terrain/draw_300.inc"
};
static NJS_CNK_OBJECT draw_299 = {
#include "stg40/terrain/draw_299.inc"
};
static NJS_CNK_OBJECT draw_298 = {
#include "stg40/terrain/draw_298.inc"
};
static NJS_CNK_OBJECT draw_297 = {
#include "stg40/terrain/draw_297.inc"
};
static NJS_CNK_OBJECT draw_296 = {
#include "stg40/terrain/draw_296.inc"
};
static NJS_CNK_OBJECT draw_295 = {
#include "stg40/terrain/draw_295.inc"
};
static NJS_CNK_OBJECT draw_294 = {
#include "stg40/terrain/draw_294.inc"
};
static NJS_CNK_OBJECT draw_293 = {
#include "stg40/terrain/draw_293.inc"
};
static NJS_CNK_OBJECT draw_292 = {
#include "stg40/terrain/draw_292.inc"
};
static NJS_CNK_OBJECT draw_291 = {
#include "stg40/terrain/draw_291.inc"
};
static NJS_CNK_OBJECT draw_290 = {
#include "stg40/terrain/draw_290.inc"
};
static NJS_CNK_OBJECT draw_289 = {
#include "stg40/terrain/draw_289.inc"
};
static NJS_CNK_OBJECT draw_288 = {
#include "stg40/terrain/draw_288.inc"
};
static NJS_CNK_OBJECT draw_287 = {
#include "stg40/terrain/draw_287.inc"
};
static NJS_CNK_OBJECT draw_286 = {
#include "stg40/terrain/draw_286.inc"
};
static NJS_CNK_OBJECT draw_285 = {
#include "stg40/terrain/draw_285.inc"
};
static NJS_CNK_OBJECT draw_284 = {
#include "stg40/terrain/draw_284.inc"
};
static NJS_CNK_OBJECT draw_283 = {
#include "stg40/terrain/draw_283.inc"
};
static NJS_CNK_OBJECT draw_282 = {
#include "stg40/terrain/draw_282.inc"
};
static NJS_CNK_OBJECT draw_281 = {
#include "stg40/terrain/draw_281.inc"
};
static NJS_CNK_OBJECT draw_280 = {
#include "stg40/terrain/draw_280.inc"
};
static NJS_CNK_OBJECT draw_279 = {
#include "stg40/terrain/draw_279.inc"
};
static NJS_CNK_OBJECT draw_278 = {
#include "stg40/terrain/draw_278.inc"
};
static NJS_CNK_OBJECT draw_277 = {
#include "stg40/terrain/draw_277.inc"
};
static NJS_CNK_OBJECT draw_276 = {
#include "stg40/terrain/draw_276.inc"
};
static NJS_CNK_OBJECT draw_273 = {
#include "stg40/terrain/draw_273.inc"
};
static NJS_CNK_OBJECT draw_272 = {
#include "stg40/terrain/draw_272.inc"
};
static NJS_CNK_OBJECT draw_271 = {
#include "stg40/terrain/draw_271.inc"
};
static NJS_CNK_OBJECT draw_270 = {
#include "stg40/terrain/draw_270.inc"
};
static NJS_CNK_OBJECT draw_269 = {
#include "stg40/terrain/draw_269.inc"
};
static NJS_CNK_OBJECT draw_267 = {
#include "stg40/terrain/draw_267.inc"
};
static NJS_CNK_OBJECT draw_266 = {
#include "stg40/terrain/draw_266.inc"
};
static NJS_CNK_OBJECT draw_265 = {
#include "stg40/terrain/draw_265.inc"
};
static NJS_CNK_OBJECT draw_264 = {
#include "stg40/terrain/draw_264.inc"
};
static NJS_CNK_OBJECT draw_263 = {
#include "stg40/terrain/draw_263.inc"
};
static NJS_CNK_OBJECT draw_262 = {
#include "stg40/terrain/draw_262.inc"
};
static NJS_CNK_OBJECT draw_261 = {
#include "stg40/terrain/draw_261.inc"
};
static NJS_CNK_OBJECT draw_259 = {
#include "stg40/terrain/draw_259.inc"
};
static NJS_CNK_OBJECT draw_258 = {
#include "stg40/terrain/draw_258.inc"
};
static NJS_CNK_OBJECT draw_257 = {
#include "stg40/terrain/draw_257.inc"
};
static NJS_CNK_OBJECT draw_256 = {
#include "stg40/terrain/draw_256.inc"
};
static NJS_CNK_OBJECT draw_255 = {
#include "stg40/terrain/draw_255.inc"
};
static NJS_CNK_OBJECT draw_254 = {
#include "stg40/terrain/draw_254.inc"
};
static NJS_CNK_OBJECT draw_253 = {
#include "stg40/terrain/draw_253.inc"
};
static NJS_CNK_OBJECT draw_252 = {
#include "stg40/terrain/draw_252.inc"
};
static NJS_CNK_OBJECT draw_251 = {
#include "stg40/terrain/draw_251.inc"
};
static NJS_CNK_OBJECT draw_250 = {
#include "stg40/terrain/draw_250.inc"
};
static NJS_CNK_OBJECT draw_249 = {
#include "stg40/terrain/draw_249.inc"
};
static NJS_CNK_OBJECT draw_246 = {
#include "stg40/terrain/draw_246.inc"
};
static NJS_CNK_OBJECT draw_245 = {
#include "stg40/terrain/draw_245.inc"
};
static NJS_CNK_OBJECT draw_244 = {
#include "stg40/terrain/draw_244.inc"
};
static NJS_CNK_OBJECT draw_243 = {
#include "stg40/terrain/draw_243.inc"
};
static NJS_CNK_OBJECT draw_242 = {
#include "stg40/terrain/draw_242.inc"
};
static NJS_CNK_OBJECT draw_241 = {
#include "stg40/terrain/draw_241.inc"
};
static NJS_CNK_OBJECT draw_240 = {
#include "stg40/terrain/draw_240.inc"
};
static NJS_CNK_OBJECT draw_239 = {
#include "stg40/terrain/draw_239.inc"
};
static NJS_CNK_OBJECT draw_238 = {
#include "stg40/terrain/draw_238.inc"
};
static NJS_CNK_OBJECT draw_237 = {
#include "stg40/terrain/draw_237.inc"
};
static NJS_CNK_OBJECT draw_236 = {
#include "stg40/terrain/draw_236.inc"
};
static NJS_CNK_OBJECT draw_235 = {
#include "stg40/terrain/draw_235.inc"
};
static NJS_CNK_OBJECT draw_234 = {
#include "stg40/terrain/draw_234.inc"
};
static NJS_CNK_OBJECT draw_233 = {
#include "stg40/terrain/draw_233.inc"
};
static NJS_CNK_OBJECT draw_232 = {
#include "stg40/terrain/draw_232.inc"
};
static NJS_CNK_OBJECT draw_231 = {
#include "stg40/terrain/draw_231.inc"
};
static NJS_CNK_OBJECT draw_230 = {
#include "stg40/terrain/draw_230.inc"
};
static NJS_CNK_OBJECT draw_229 = {
#include "stg40/terrain/draw_229.inc"
};
static NJS_CNK_OBJECT draw_228 = {
#include "stg40/terrain/draw_228.inc"
};
static NJS_CNK_OBJECT draw_227 = {
#include "stg40/terrain/draw_227.inc"
};
static NJS_CNK_OBJECT draw_226 = {
#include "stg40/terrain/draw_226.inc"
};
static NJS_CNK_OBJECT draw_222 = {
#include "stg40/terrain/draw_222.inc"
};
static NJS_CNK_OBJECT draw_221 = {
#include "stg40/terrain/draw_221.inc"
};
static NJS_CNK_OBJECT draw_220 = {
#include "stg40/terrain/draw_220.inc"
};
static NJS_CNK_OBJECT draw_219 = {
#include "stg40/terrain/draw_219.inc"
};
static NJS_CNK_OBJECT draw_218 = {
#include "stg40/terrain/draw_218.inc"
};
static NJS_CNK_OBJECT draw_217 = {
#include "stg40/terrain/draw_217.inc"
};
static NJS_CNK_OBJECT draw_215 = {
#include "stg40/terrain/draw_215.inc"
};
static NJS_CNK_OBJECT draw_214 = {
#include "stg40/terrain/draw_214.inc"
};
static NJS_CNK_OBJECT draw_213 = {
#include "stg40/terrain/draw_213.inc"
};
static NJS_CNK_OBJECT draw_212 = {
#include "stg40/terrain/draw_212.inc"
};
static NJS_CNK_OBJECT draw_211 = {
#include "stg40/terrain/draw_211.inc"
};
static NJS_CNK_OBJECT draw_210 = {
#include "stg40/terrain/draw_210.inc"
};
static NJS_CNK_OBJECT draw_209 = {
#include "stg40/terrain/draw_209.inc"
};
static NJS_CNK_OBJECT draw_208 = {
#include "stg40/terrain/draw_208.inc"
};
static NJS_CNK_OBJECT draw_207 = {
#include "stg40/terrain/draw_207.inc"
};
static NJS_CNK_OBJECT draw_206 = {
#include "stg40/terrain/draw_206.inc"
};
static NJS_CNK_OBJECT draw_205 = {
#include "stg40/terrain/draw_205.inc"
};
static NJS_CNK_OBJECT draw_204 = {
#include "stg40/terrain/draw_204.inc"
};
static NJS_CNK_OBJECT draw_203 = {
#include "stg40/terrain/draw_203.inc"
};
static NJS_CNK_OBJECT draw_202 = {
#include "stg40/terrain/draw_202.inc"
};
static NJS_CNK_OBJECT draw_201 = {
#include "stg40/terrain/draw_201.inc"
};
static NJS_CNK_OBJECT draw_200 = {
#include "stg40/terrain/draw_200.inc"
};
static NJS_CNK_OBJECT draw_199 = {
#include "stg40/terrain/draw_199.inc"
};
static NJS_CNK_OBJECT draw_198 = {
#include "stg40/terrain/draw_198.inc"
};
static NJS_CNK_OBJECT draw_197 = {
#include "stg40/terrain/draw_197.inc"
};
static NJS_CNK_OBJECT draw_196 = {
#include "stg40/terrain/draw_196.inc"
};
static NJS_CNK_OBJECT draw_195 = {
#include "stg40/terrain/draw_195.inc"
};
static NJS_CNK_OBJECT draw_194 = {
#include "stg40/terrain/draw_194.inc"
};
static NJS_CNK_OBJECT draw_193 = {
#include "stg40/terrain/draw_193.inc"
};
static NJS_CNK_OBJECT draw_192 = {
#include "stg40/terrain/draw_192.inc"
};
static NJS_CNK_OBJECT draw_191 = {
#include "stg40/terrain/draw_191.inc"
};
static NJS_CNK_OBJECT draw_190 = {
#include "stg40/terrain/draw_190.inc"
};
static NJS_CNK_OBJECT draw_188 = {
#include "stg40/terrain/draw_188.inc"
};
static NJS_CNK_OBJECT draw_187 = {
#include "stg40/terrain/draw_187.inc"
};
static NJS_CNK_OBJECT draw_186 = {
#include "stg40/terrain/draw_186.inc"
};
static NJS_CNK_OBJECT draw_185 = {
#include "stg40/terrain/draw_185.inc"
};
static NJS_CNK_OBJECT draw_184 = {
#include "stg40/terrain/draw_184.inc"
};
static NJS_CNK_OBJECT draw_183 = {
#include "stg40/terrain/draw_183.inc"
};
static NJS_CNK_OBJECT draw_182 = {
#include "stg40/terrain/draw_182.inc"
};
static NJS_CNK_OBJECT draw_181 = {
#include "stg40/terrain/draw_181.inc"
};
static NJS_CNK_OBJECT draw_180 = {
#include "stg40/terrain/draw_180.inc"
};
static NJS_CNK_OBJECT draw_179 = {
#include "stg40/terrain/draw_179.inc"
};
static NJS_CNK_OBJECT draw_178 = {
#include "stg40/terrain/draw_178.inc"
};
static NJS_CNK_OBJECT draw_177 = {
#include "stg40/terrain/draw_177.inc"
};
static NJS_CNK_OBJECT draw_176 = {
#include "stg40/terrain/draw_176.inc"
};
static NJS_CNK_OBJECT draw_175 = {
#include "stg40/terrain/draw_175.inc"
};
static NJS_CNK_OBJECT draw_174 = {
#include "stg40/terrain/draw_174.inc"
};
static NJS_CNK_OBJECT draw_173 = {
#include "stg40/terrain/draw_173.inc"
};
static NJS_CNK_OBJECT draw_172 = {
#include "stg40/terrain/draw_172.inc"
};
static NJS_CNK_OBJECT draw_171 = {
#include "stg40/terrain/draw_171.inc"
};
static NJS_CNK_OBJECT draw_170 = {
#include "stg40/terrain/draw_170.inc"
};
static NJS_CNK_OBJECT draw_169 = {
#include "stg40/terrain/draw_169.inc"
};
static NJS_CNK_OBJECT draw_168 = {
#include "stg40/terrain/draw_168.inc"
};
static NJS_CNK_OBJECT draw_167 = {
#include "stg40/terrain/draw_167.inc"
};
static NJS_CNK_OBJECT draw_166 = {
#include "stg40/terrain/draw_166.inc"
};
static NJS_CNK_OBJECT draw_165 = {
#include "stg40/terrain/draw_165.inc"
};
static NJS_CNK_OBJECT draw_164 = {
#include "stg40/terrain/draw_164.inc"
};
static NJS_CNK_OBJECT draw_163 = {
#include "stg40/terrain/draw_163.inc"
};
static NJS_CNK_OBJECT draw_162 = {
#include "stg40/terrain/draw_162.inc"
};
static NJS_CNK_OBJECT draw_161 = {
#include "stg40/terrain/draw_161.inc"
};
static NJS_CNK_OBJECT draw_160 = {
#include "stg40/terrain/draw_160.inc"
};
static NJS_CNK_OBJECT draw_159 = {
#include "stg40/terrain/draw_159.inc"
};
static NJS_CNK_OBJECT draw_158 = {
#include "stg40/terrain/draw_158.inc"
};
static NJS_CNK_OBJECT draw_157 = {
#include "stg40/terrain/draw_157.inc"
};
static NJS_CNK_OBJECT draw_156 = {
#include "stg40/terrain/draw_156.inc"
};
static NJS_CNK_OBJECT draw_155 = {
#include "stg40/terrain/draw_155.inc"
};
static NJS_CNK_OBJECT draw_154 = {
#include "stg40/terrain/draw_154.inc"
};
static NJS_CNK_OBJECT draw_153 = {
#include "stg40/terrain/draw_153.inc"
};
static NJS_CNK_OBJECT draw_152 = {
#include "stg40/terrain/draw_152.inc"
};
static NJS_CNK_OBJECT draw_151 = {
#include "stg40/terrain/draw_151.inc"
};
static NJS_CNK_OBJECT draw_150 = {
#include "stg40/terrain/draw_150.inc"
};
static NJS_CNK_OBJECT draw_149 = {
#include "stg40/terrain/draw_149.inc"
};
static NJS_CNK_OBJECT draw_148 = {
#include "stg40/terrain/draw_148.inc"
};
static NJS_CNK_OBJECT draw_147 = {
#include "stg40/terrain/draw_147.inc"
};
static NJS_CNK_OBJECT draw_146 = {
#include "stg40/terrain/draw_146.inc"
};
static NJS_CNK_OBJECT draw_145 = {
#include "stg40/terrain/draw_145.inc"
};
static NJS_CNK_OBJECT draw_144 = {
#include "stg40/terrain/draw_144.inc"
};
static NJS_CNK_OBJECT draw_143 = {
#include "stg40/terrain/draw_143.inc"
};
static NJS_CNK_OBJECT draw_142 = {
#include "stg40/terrain/draw_142.inc"
};
static NJS_CNK_OBJECT draw_141 = {
#include "stg40/terrain/draw_141.inc"
};
static NJS_CNK_OBJECT draw_140 = {
#include "stg40/terrain/draw_140.inc"
};
static NJS_CNK_OBJECT draw_139 = {
#include "stg40/terrain/draw_139.inc"
};
static NJS_CNK_OBJECT draw_138 = {
#include "stg40/terrain/draw_138.inc"
};
static NJS_CNK_OBJECT draw_137 = {
#include "stg40/terrain/draw_137.inc"
};
static NJS_CNK_OBJECT draw_136 = {
#include "stg40/terrain/draw_136.inc"
};
static NJS_CNK_OBJECT draw_135 = {
#include "stg40/terrain/draw_135.inc"
};
static NJS_CNK_OBJECT draw_134 = {
#include "stg40/terrain/draw_134.inc"
};
static NJS_CNK_OBJECT draw_133 = {
#include "stg40/terrain/draw_133.inc"
};
static NJS_CNK_OBJECT draw_132 = {
#include "stg40/terrain/draw_132.inc"
};
static NJS_CNK_OBJECT draw_131 = {
#include "stg40/terrain/draw_131.inc"
};
static NJS_CNK_OBJECT draw_130 = {
#include "stg40/terrain/draw_130.inc"
};
static NJS_CNK_OBJECT draw_129 = {
#include "stg40/terrain/draw_129.inc"
};
static NJS_CNK_OBJECT draw_128 = {
#include "stg40/terrain/draw_128.inc"
};
static NJS_CNK_OBJECT draw_127 = {
#include "stg40/terrain/draw_127.inc"
};
static NJS_CNK_OBJECT draw_126 = {
#include "stg40/terrain/draw_126.inc"
};
static NJS_CNK_OBJECT draw_125 = {
#include "stg40/terrain/draw_125.inc"
};
static NJS_CNK_OBJECT draw_123 = {
#include "stg40/terrain/draw_123.inc"
};
static NJS_CNK_OBJECT draw_122 = {
#include "stg40/terrain/draw_122.inc"
};
static NJS_CNK_OBJECT draw_121 = {
#include "stg40/terrain/draw_121.inc"
};
static NJS_CNK_OBJECT draw_120 = {
#include "stg40/terrain/draw_120.inc"
};
static NJS_CNK_OBJECT draw_119 = {
#include "stg40/terrain/draw_119.inc"
};
static NJS_CNK_OBJECT draw_118 = {
#include "stg40/terrain/draw_118.inc"
};
static NJS_CNK_OBJECT draw_117 = {
#include "stg40/terrain/draw_117.inc"
};
static NJS_CNK_OBJECT draw_116 = {
#include "stg40/terrain/draw_116.inc"
};
static NJS_CNK_OBJECT draw_115 = {
#include "stg40/terrain/draw_115.inc"
};
static NJS_CNK_OBJECT draw_114 = {
#include "stg40/terrain/draw_114.inc"
};
static NJS_CNK_OBJECT draw_113 = {
#include "stg40/terrain/draw_113.inc"
};
static NJS_CNK_OBJECT draw_112 = {
#include "stg40/terrain/draw_112.inc"
};
static NJS_CNK_OBJECT draw_111 = {
#include "stg40/terrain/draw_111.inc"
};
static NJS_CNK_OBJECT draw_110 = {
#include "stg40/terrain/draw_110.inc"
};
static NJS_CNK_OBJECT draw_109 = {
#include "stg40/terrain/draw_109.inc"
};
static NJS_CNK_OBJECT draw_108 = {
#include "stg40/terrain/draw_108.inc"
};
static NJS_CNK_OBJECT draw_107 = {
#include "stg40/terrain/draw_107.inc"
};
static NJS_CNK_OBJECT draw_106 = {
#include "stg40/terrain/draw_106.inc"
};
static NJS_CNK_OBJECT draw_105 = {
#include "stg40/terrain/draw_105.inc"
};
static NJS_CNK_OBJECT draw_104 = {
#include "stg40/terrain/draw_104.inc"
};
static NJS_CNK_OBJECT draw_103 = {
#include "stg40/terrain/draw_103.inc"
};
static NJS_CNK_OBJECT draw_102 = {
#include "stg40/terrain/draw_102.inc"
};
static NJS_CNK_OBJECT draw_101 = {
#include "stg40/terrain/draw_101.inc"
};
static NJS_CNK_OBJECT draw_100 = {
#include "stg40/terrain/draw_100.inc"
};
static NJS_CNK_OBJECT draw_99 = {
#include "stg40/terrain/draw_99.inc"
};
static NJS_CNK_OBJECT draw_98 = {
#include "stg40/terrain/draw_98.inc"
};
static NJS_CNK_OBJECT draw_97 = {
#include "stg40/terrain/draw_97.inc"
};
static NJS_CNK_OBJECT draw_96 = {
#include "stg40/terrain/draw_96.inc"
};
static NJS_CNK_OBJECT draw_95 = {
#include "stg40/terrain/draw_95.inc"
};
static NJS_CNK_OBJECT draw_94 = {
#include "stg40/terrain/draw_94.inc"
};
static NJS_CNK_OBJECT draw_93 = {
#include "stg40/terrain/draw_93.inc"
};
static NJS_CNK_OBJECT draw_92 = {
#include "stg40/terrain/draw_92.inc"
};
static NJS_CNK_OBJECT draw_91 = {
#include "stg40/terrain/draw_91.inc"
};
static NJS_CNK_OBJECT draw_90 = {
#include "stg40/terrain/draw_90.inc"
};
static NJS_CNK_OBJECT draw_89 = {
#include "stg40/terrain/draw_89.inc"
};
static NJS_CNK_OBJECT draw_88 = {
#include "stg40/terrain/draw_88.inc"
};
static NJS_CNK_OBJECT draw_87 = {
#include "stg40/terrain/draw_87.inc"
};
static NJS_CNK_OBJECT draw_86 = {
#include "stg40/terrain/draw_86.inc"
};
static NJS_CNK_OBJECT draw_85 = {
#include "stg40/terrain/draw_85.inc"
};
static NJS_CNK_OBJECT draw_84 = {
#include "stg40/terrain/draw_84.inc"
};
static NJS_CNK_OBJECT draw_83 = {
#include "stg40/terrain/draw_83.inc"
};
static NJS_CNK_OBJECT draw_82 = {
#include "stg40/terrain/draw_82.inc"
};
static NJS_CNK_OBJECT draw_81 = {
#include "stg40/terrain/draw_81.inc"
};
static NJS_CNK_OBJECT draw_80 = {
#include "stg40/terrain/draw_80.inc"
};
static NJS_CNK_OBJECT draw_79 = {
#include "stg40/terrain/draw_79.inc"
};
static NJS_CNK_OBJECT draw_78 = {
#include "stg40/terrain/draw_78.inc"
};
static NJS_CNK_OBJECT draw_77 = {
#include "stg40/terrain/draw_77.inc"
};
static NJS_CNK_OBJECT draw_76 = {
#include "stg40/terrain/draw_76.inc"
};
static NJS_CNK_OBJECT draw_75 = {
#include "stg40/terrain/draw_75.inc"
};
static NJS_CNK_OBJECT draw_74 = {
#include "stg40/terrain/draw_74.inc"
};
static NJS_CNK_OBJECT draw_73 = {
#include "stg40/terrain/draw_73.inc"
};
static NJS_CNK_OBJECT draw_72 = {
#include "stg40/terrain/draw_72.inc"
};
static NJS_CNK_OBJECT draw_71 = {
#include "stg40/terrain/draw_71.inc"
};
static NJS_CNK_OBJECT draw_70 = {
#include "stg40/terrain/draw_70.inc"
};
static NJS_CNK_OBJECT draw_69 = {
#include "stg40/terrain/draw_69.inc"
};
static NJS_CNK_OBJECT draw_68 = {
#include "stg40/terrain/draw_68.inc"
};
static NJS_CNK_OBJECT draw_67 = {
#include "stg40/terrain/draw_67.inc"
};
static NJS_CNK_OBJECT draw_66 = {
#include "stg40/terrain/draw_66.inc"
};
static NJS_CNK_OBJECT draw_65 = {
#include "stg40/terrain/draw_65.inc"
};
static NJS_CNK_OBJECT draw_64 = {
#include "stg40/terrain/draw_64.inc"
};
static NJS_CNK_OBJECT draw_63 = {
#include "stg40/terrain/draw_63.inc"
};
static NJS_CNK_OBJECT draw_62 = {
#include "stg40/terrain/draw_62.inc"
};
static NJS_CNK_OBJECT draw_61 = {
#include "stg40/terrain/draw_61.inc"
};
static NJS_CNK_OBJECT draw_60 = {
#include "stg40/terrain/draw_60.inc"
};
static NJS_CNK_OBJECT draw_59 = {
#include "stg40/terrain/draw_59.inc"
};
static NJS_CNK_OBJECT draw_58 = {
#include "stg40/terrain/draw_58.inc"
};
static NJS_CNK_OBJECT draw_57 = {
#include "stg40/terrain/draw_57.inc"
};
static NJS_CNK_OBJECT draw_56 = {
#include "stg40/terrain/draw_56.inc"
};
static NJS_CNK_OBJECT draw_55 = {
#include "stg40/terrain/draw_55.inc"
};
static NJS_CNK_OBJECT draw_54 = {
#include "stg40/terrain/draw_54.inc"
};
static NJS_CNK_OBJECT draw_53 = {
#include "stg40/terrain/draw_53.inc"
};
static NJS_CNK_OBJECT draw_52 = {
#include "stg40/terrain/draw_52.inc"
};
static NJS_CNK_OBJECT draw_51 = {
#include "stg40/terrain/draw_51.inc"
};
static NJS_CNK_OBJECT draw_50 = {
#include "stg40/terrain/draw_50.inc"
};
static NJS_CNK_OBJECT draw_49 = {
#include "stg40/terrain/draw_49.inc"
};
static NJS_CNK_OBJECT draw_48 = {
#include "stg40/terrain/draw_48.inc"
};
static NJS_CNK_OBJECT draw_47 = {
#include "stg40/terrain/draw_47.inc"
};
static NJS_CNK_OBJECT draw_46 = {
#include "stg40/terrain/draw_46.inc"
};
static NJS_CNK_OBJECT draw_45 = {
#include "stg40/terrain/draw_45.inc"
};
static NJS_CNK_OBJECT draw_44 = {
#include "stg40/terrain/draw_44.inc"
};
static NJS_CNK_OBJECT draw_43 = {
#include "stg40/terrain/draw_43.inc"
};
static NJS_CNK_OBJECT draw_42 = {
#include "stg40/terrain/draw_42.inc"
};
static NJS_CNK_OBJECT draw_41 = {
#include "stg40/terrain/draw_41.inc"
};
static NJS_CNK_OBJECT draw_40 = {
#include "stg40/terrain/draw_40.inc"
};
static NJS_CNK_OBJECT draw_39 = {
#include "stg40/terrain/draw_39.inc"
};
static NJS_CNK_OBJECT draw_38 = {
#include "stg40/terrain/draw_38.inc"
};
static NJS_CNK_OBJECT draw_37 = {
#include "stg40/terrain/draw_37.inc"
};
static NJS_CNK_OBJECT draw_36 = {
#include "stg40/terrain/draw_36.inc"
};
static NJS_CNK_OBJECT draw_35 = {
#include "stg40/terrain/draw_35.inc"
};
static NJS_CNK_OBJECT draw_34 = {
#include "stg40/terrain/draw_34.inc"
};
static NJS_CNK_OBJECT draw_33 = {
#include "stg40/terrain/draw_33.inc"
};
static NJS_CNK_OBJECT draw_32 = {
#include "stg40/terrain/draw_32.inc"
};
static NJS_CNK_OBJECT draw_31 = {
#include "stg40/terrain/draw_31.inc"
};
static NJS_CNK_OBJECT draw_30 = {
#include "stg40/terrain/draw_30.inc"
};
static NJS_CNK_OBJECT draw_25 = {
#include "stg40/terrain/draw_25.inc"
};
static NJS_CNK_OBJECT draw_24 = {
#include "stg40/terrain/draw_24.inc"
};
static NJS_CNK_OBJECT draw_23 = {
#include "stg40/terrain/draw_23.inc"
};
static NJS_CNK_OBJECT draw_22 = {
#include "stg40/terrain/draw_22.inc"
};
static NJS_CNK_OBJECT draw_21 = {
#include "stg40/terrain/draw_21.inc"
};
static NJS_CNK_OBJECT draw_20 = {
#include "stg40/terrain/draw_20.inc"
};
static NJS_CNK_OBJECT draw_19 = {
#include "stg40/terrain/draw_19.inc"
};
static NJS_CNK_OBJECT draw_18 = {
#include "stg40/terrain/draw_18.inc"
};
static NJS_CNK_OBJECT draw_17 = {
#include "stg40/terrain/draw_17.inc"
};
static NJS_CNK_OBJECT draw_16 = {
#include "stg40/terrain/draw_16.inc"
};
static NJS_CNK_OBJECT draw_15 = {
#include "stg40/terrain/draw_15.inc"
};
static NJS_CNK_OBJECT draw_14 = {
#include "stg40/terrain/draw_14.inc"
};
static NJS_CNK_OBJECT draw_13 = {
#include "stg40/terrain/draw_13.inc"
};
static NJS_CNK_OBJECT draw_12 = {
#include "stg40/terrain/draw_12.inc"
};
static NJS_CNK_OBJECT draw_11 = {
#include "stg40/terrain/draw_11.inc"
};
static NJS_CNK_OBJECT draw_10 = {
#include "stg40/terrain/draw_10.inc"
};
static NJS_CNK_OBJECT draw_9 = {
#include "stg40/terrain/draw_9.inc"
};
static NJS_CNK_OBJECT draw_8 = {
#include "stg40/terrain/draw_8.inc"
};
static NJS_CNK_OBJECT draw_7 = {
#include "stg40/terrain/draw_7.inc"
};
static NJS_CNK_OBJECT draw_6 = {
#include "stg40/terrain/draw_6.inc"
};
static NJS_CNK_OBJECT draw_5 = {
#include "stg40/terrain/draw_5.inc"
};
static NJS_CNK_OBJECT draw_4 = {
#include "stg40/terrain/draw_4.inc"
};
static NJS_CNK_OBJECT draw_3 = {
#include "stg40/terrain/draw_3.inc"
};
static NJS_CNK_OBJECT draw_2 = {
#include "stg40/terrain/draw_2.inc"
};
static NJS_CNK_OBJECT draw_1 = {
#include "stg40/terrain/draw_1.inc"
};
static NJS_CNK_OBJECT draw_0 = {
#include "stg40/terrain/draw_0.inc"
};
static NJS_OBJECT collision_4 = {
#include "stg40/terrain/collision_4.inc"
};
static NJS_OBJECT collision_5 = {
#include "stg40/terrain/collision_5.inc"
};
static NJS_OBJECT collision_6 = {
#include "stg40/terrain/collision_6.inc"
};
static NJS_OBJECT collision_10 = {
#include "stg40/terrain/collision_10.inc"
};
static NJS_OBJECT collision_11 = {
#include "stg40/terrain/collision_11.inc"
};
static NJS_OBJECT collision_14 = {
#include "stg40/terrain/collision_14.inc"
};
static NJS_OBJECT collision_15 = {
#include "stg40/terrain/collision_15.inc"
};
static NJS_OBJECT collision_17 = {
#include "stg40/terrain/collision_17.inc"
};
static NJS_OBJECT collision_18 = {
#include "stg40/terrain/collision_18.inc"
};
static NJS_OBJECT collision_19 = {
#include "stg40/terrain/collision_19.inc"
};
static NJS_OBJECT collision_20 = {
#include "stg40/terrain/collision_20.inc"
};
static NJS_OBJECT collision_21 = {
#include "stg40/terrain/collision_21.inc"
};
static NJS_OBJECT collision_22 = {
#include "stg40/terrain/collision_22.inc"
};
static NJS_OBJECT collision_25 = {
#include "stg40/terrain/collision_25.inc"
};
static NJS_OBJECT collision_26 = {
#include "stg40/terrain/collision_26.inc"
};
static NJS_OBJECT collision_27 = {
#include "stg40/terrain/collision_27.inc"
};
static NJS_OBJECT collision_29 = {
#include "stg40/terrain/collision_29.inc"
};
static NJS_OBJECT collision_30 = {
#include "stg40/terrain/collision_30.inc"
};
static NJS_OBJECT collision_31 = {
#include "stg40/terrain/collision_31.inc"
};
static NJS_OBJECT collision_32 = {
#include "stg40/terrain/collision_32.inc"
};
static NJS_OBJECT collision_33 = {
#include "stg40/terrain/collision_33.inc"
};
static NJS_OBJECT collision_34 = {
#include "stg40/terrain/collision_34.inc"
};
static NJS_OBJECT collision_35 = {
#include "stg40/terrain/collision_35.inc"
};
static NJS_OBJECT collision_36 = {
#include "stg40/terrain/collision_36.inc"
};
static NJS_OBJECT collision_37 = {
#include "stg40/terrain/collision_37.inc"
};
static NJS_OBJECT collision_38 = {
#include "stg40/terrain/collision_38.inc"
};
static NJS_OBJECT collision_39 = {
#include "stg40/terrain/collision_39.inc"
};
static NJS_OBJECT collision_40 = {
#include "stg40/terrain/collision_40.inc"
};
static NJS_OBJECT collision_41 = {
#include "stg40/terrain/collision_41.inc"
};
static NJS_OBJECT collision_42 = {
#include "stg40/terrain/collision_42.inc"
};
static NJS_OBJECT collision_43 = {
#include "stg40/terrain/collision_43.inc"
};
static NJS_OBJECT collision_44 = {
#include "stg40/terrain/collision_44.inc"
};
static NJS_OBJECT collision_45 = {
#include "stg40/terrain/collision_45.inc"
};
static NJS_OBJECT collision_46 = {
#include "stg40/terrain/collision_46.inc"
};
static NJS_OBJECT collision_47 = {
#include "stg40/terrain/collision_47.inc"
};
static NJS_OBJECT collision_48 = {
#include "stg40/terrain/collision_48.inc"
};
static NJS_OBJECT collision_49 = {
#include "stg40/terrain/collision_49.inc"
};
static NJS_OBJECT collision_50 = {
#include "stg40/terrain/collision_50.inc"
};
static NJS_OBJECT collision_51 = {
#include "stg40/terrain/collision_51.inc"
};
static NJS_OBJECT collision_52 = {
#include "stg40/terrain/collision_52.inc"
};
static NJS_OBJECT collision_53 = {
#include "stg40/terrain/collision_53.inc"
};
static NJS_OBJECT collision_54 = {
#include "stg40/terrain/collision_54.inc"
};
static NJS_OBJECT collision_55 = {
#include "stg40/terrain/collision_55.inc"
};
static NJS_OBJECT collision_56 = {
#include "stg40/terrain/collision_56.inc"
};
static NJS_OBJECT collision_57 = {
#include "stg40/terrain/collision_57.inc"
};
static NJS_OBJECT collision_58 = {
#include "stg40/terrain/collision_58.inc"
};
static NJS_OBJECT collision_59 = {
#include "stg40/terrain/collision_59.inc"
};
static NJS_OBJECT collision_60 = {
#include "stg40/terrain/collision_60.inc"
};
static NJS_OBJECT collision_61 = {
#include "stg40/terrain/collision_61.inc"
};
static NJS_OBJECT collision_62 = {
#include "stg40/terrain/collision_62.inc"
};
static NJS_OBJECT collision_63 = {
#include "stg40/terrain/collision_63.inc"
};
static NJS_OBJECT collision_64 = {
#include "stg40/terrain/collision_64.inc"
};
static NJS_OBJECT collision_65 = {
#include "stg40/terrain/collision_65.inc"
};
static NJS_OBJECT collision_66 = {
#include "stg40/terrain/collision_66.inc"
};
static NJS_OBJECT collision_67 = {
#include "stg40/terrain/collision_67.inc"
};
static NJS_OBJECT collision_68 = {
#include "stg40/terrain/collision_68.inc"
};
static NJS_OBJECT collision_69 = {
#include "stg40/terrain/collision_69.inc"
};
static NJS_OBJECT collision_70 = {
#include "stg40/terrain/collision_70.inc"
};
static NJS_OBJECT collision_71 = {
#include "stg40/terrain/collision_71.inc"
};
static NJS_OBJECT collision_72 = {
#include "stg40/terrain/collision_72.inc"
};
static NJS_OBJECT collision_73 = {
#include "stg40/terrain/collision_73.inc"
};
static NJS_OBJECT collision_74 = {
#include "stg40/terrain/collision_74.inc"
};
static NJS_OBJECT collision_75 = {
#include "stg40/terrain/collision_75.inc"
};
static NJS_OBJECT collision_76 = {
#include "stg40/terrain/collision_76.inc"
};
static NJS_OBJECT collision_77 = {
#include "stg40/terrain/collision_77.inc"
};
static NJS_OBJECT collision_78 = {
#include "stg40/terrain/collision_78.inc"
};
static NJS_OBJECT collision_79 = {
#include "stg40/terrain/collision_79.inc"
};
static NJS_OBJECT collision_80 = {
#include "stg40/terrain/collision_80.inc"
};
static NJS_OBJECT collision_81 = {
#include "stg40/terrain/collision_81.inc"
};
static NJS_OBJECT collision_82 = {
#include "stg40/terrain/collision_82.inc"
};
static NJS_OBJECT collision_83 = {
#include "stg40/terrain/collision_83.inc"
};
static NJS_OBJECT collision_84 = {
#include "stg40/terrain/collision_84.inc"
};
static NJS_OBJECT collision_85 = {
#include "stg40/terrain/collision_85.inc"
};
static NJS_OBJECT collision_86 = {
#include "stg40/terrain/collision_86.inc"
};
static NJS_OBJECT collision_87 = {
#include "stg40/terrain/collision_87.inc"
};
static NJS_OBJECT collision_88 = {
#include "stg40/terrain/collision_88.inc"
};
static NJS_OBJECT collision_89 = {
#include "stg40/terrain/collision_89.inc"
};
static NJS_OBJECT collision_90 = {
#include "stg40/terrain/collision_90.inc"
};
static NJS_OBJECT collision_91 = {
#include "stg40/terrain/collision_91.inc"
};
static NJS_OBJECT collision_92 = {
#include "stg40/terrain/collision_92.inc"
};
static NJS_OBJECT collision_93 = {
#include "stg40/terrain/collision_93.inc"
};
static NJS_OBJECT collision_94 = {
#include "stg40/terrain/collision_94.inc"
};
static NJS_OBJECT collision_95 = {
#include "stg40/terrain/collision_95.inc"
};
static NJS_OBJECT collision_96 = {
#include "stg40/terrain/collision_96.inc"
};
static NJS_OBJECT collision_97 = {
#include "stg40/terrain/collision_97.inc"
};
static NJS_OBJECT collision_99 = {
#include "stg40/terrain/collision_99.inc"
};
static NJS_OBJECT collision_101 = {
#include "stg40/terrain/collision_101.inc"
};
static NJS_OBJECT collision_102 = {
#include "stg40/terrain/collision_102.inc"
};
static NJS_OBJECT collision_103 = {
#include "stg40/terrain/collision_103.inc"
};
static NJS_OBJECT collision_104 = {
#include "stg40/terrain/collision_104.inc"
};
static NJS_OBJECT collision_105 = {
#include "stg40/terrain/collision_105.inc"
};
static NJS_OBJECT collision_107 = {
#include "stg40/terrain/collision_107.inc"
};
static NJS_OBJECT collision_109 = {
#include "stg40/terrain/collision_109.inc"
};
static NJS_OBJECT collision_110 = {
#include "stg40/terrain/collision_110.inc"
};
static NJS_OBJECT collision_111 = {
#include "stg40/terrain/collision_111.inc"
};
static NJS_OBJECT collision_116 = {
#include "stg40/terrain/collision_116.inc"
};
static NJS_OBJECT collision_122 = {
#include "stg40/terrain/collision_122.inc"
};
static NJS_OBJECT collision_123 = {
#include "stg40/terrain/collision_123.inc"
};
static NJS_OBJECT collision_124 = {
#include "stg40/terrain/collision_124.inc"
};
static NJS_OBJECT collision_126 = {
#include "stg40/terrain/collision_126.inc"
};
static NJS_OBJECT collision_127 = {
#include "stg40/terrain/collision_127.inc"
};
static NJS_OBJECT collision_128 = {
#include "stg40/terrain/collision_128.inc"
};
static NJS_OBJECT collision_129 = {
#include "stg40/terrain/collision_129.inc"
};
static NJS_OBJECT collision_132 = {
#include "stg40/terrain/collision_132.inc"
};
static NJS_OBJECT collision_134 = {
#include "stg40/terrain/collision_134.inc"
};
static NJS_OBJECT collision_137 = {
#include "stg40/terrain/collision_137.inc"
};
static NJS_OBJECT collision_138 = {
#include "stg40/terrain/collision_138.inc"
};
static NJS_OBJECT collision_142 = {
#include "stg40/terrain/collision_142.inc"
};
static NJS_OBJECT collision_179 = {
#include "stg40/terrain/collision_179.inc"
};
static NJS_OBJECT collision_180 = {
#include "stg40/terrain/collision_180.inc"
};
static NJS_OBJECT collision_188 = {
#include "stg40/terrain/collision_188.inc"
};
static NJS_OBJECT collision_190 = {
#include "stg40/terrain/collision_190.inc"
};
static NJS_OBJECT collision_191 = {
#include "stg40/terrain/collision_191.inc"
};
static NJS_OBJECT collision_192 = {
#include "stg40/terrain/collision_192.inc"
};
static NJS_OBJECT collision_193 = {
#include "stg40/terrain/collision_193.inc"
};
static NJS_OBJECT collision_194 = {
#include "stg40/terrain/collision_194.inc"
};
static NJS_OBJECT collision_195 = {
#include "stg40/terrain/collision_195.inc"
};
static NJS_OBJECT collision_196 = {
#include "stg40/terrain/collision_196.inc"
};
static NJS_OBJECT collision_197 = {
#include "stg40/terrain/collision_197.inc"
};
static NJS_OBJECT collision_198 = {
#include "stg40/terrain/collision_198.inc"
};
static NJS_OBJECT collision_199 = {
#include "stg40/terrain/collision_199.inc"
};
static NJS_OBJECT collision_200 = {
#include "stg40/terrain/collision_200.inc"
};
static NJS_OBJECT collision_201 = {
#include "stg40/terrain/collision_201.inc"
};
static NJS_OBJECT collision_202 = {
#include "stg40/terrain/collision_202.inc"
};
static NJS_OBJECT collision_203 = {
#include "stg40/terrain/collision_203.inc"
};
static NJS_OBJECT collision_204 = {
#include "stg40/terrain/collision_204.inc"
};
static NJS_OBJECT collision_205 = {
#include "stg40/terrain/collision_205.inc"
};
static NJS_OBJECT collision_206 = {
#include "stg40/terrain/collision_206.inc"
};
static NJS_OBJECT collision_207 = {
#include "stg40/terrain/collision_207.inc"
};
static NJS_OBJECT collision_208 = {
#include "stg40/terrain/collision_208.inc"
};
static NJS_OBJECT collision_209 = {
#include "stg40/terrain/collision_209.inc"
};
static NJS_OBJECT collision_210 = {
#include "stg40/terrain/collision_210.inc"
};
static NJS_OBJECT collision_211 = {
#include "stg40/terrain/collision_211.inc"
};
static NJS_OBJECT collision_212 = {
#include "stg40/terrain/collision_212.inc"
};
static NJS_OBJECT collision_214 = {
#include "stg40/terrain/collision_214.inc"
};
static NJS_OBJECT collision_215 = {
#include "stg40/terrain/collision_215.inc"
};
static NJS_OBJECT collision_216 = {
#include "stg40/terrain/collision_216.inc"
};
static NJS_OBJECT collision_217 = {
#include "stg40/terrain/collision_217.inc"
};
static NJS_OBJECT collision_218 = {
#include "stg40/terrain/collision_218.inc"
};
static NJS_OBJECT collision_219 = {
#include "stg40/terrain/collision_219.inc"
};
static NJS_OBJECT collision_220 = {
#include "stg40/terrain/collision_220.inc"
};
static NJS_OBJECT collision_221 = {
#include "stg40/terrain/collision_221.inc"
};
static NJS_OBJECT collision_223 = {
#include "stg40/terrain/collision_223.inc"
};
static NJS_OBJECT collision_226 = {
#include "stg40/terrain/collision_226.inc"
};
static NJS_OBJECT collision_228 = {
#include "stg40/terrain/collision_228.inc"
};
static NJS_OBJECT collision_230 = {
#include "stg40/terrain/collision_230.inc"
};
static NJS_OBJECT collision_231 = {
#include "stg40/terrain/collision_231.inc"
};
static NJS_OBJECT collision_233 = {
#include "stg40/terrain/collision_233.inc"
};
static NJS_OBJECT collision_235 = {
#include "stg40/terrain/collision_235.inc"
};
static NJS_OBJECT collision_238 = {
#include "stg40/terrain/collision_238.inc"
};
static NJS_OBJECT collision_239 = {
#include "stg40/terrain/collision_239.inc"
};
static NJS_OBJECT collision_240 = {
#include "stg40/terrain/collision_240.inc"
};
static NJS_OBJECT collision_241 = {
#include "stg40/terrain/collision_241.inc"
};
static NJS_OBJECT collision_242 = {
#include "stg40/terrain/collision_242.inc"
};
static NJS_OBJECT collision_243 = {
#include "stg40/terrain/collision_243.inc"
};
static NJS_OBJECT collision_244 = {
#include "stg40/terrain/collision_244.inc"
};
static NJS_OBJECT collision_245 = {
#include "stg40/terrain/collision_245.inc"
};
static NJS_OBJECT collision_246 = {
#include "stg40/terrain/collision_246.inc"
};
static NJS_OBJECT collision_247 = {
#include "stg40/terrain/collision_247.inc"
};
static NJS_OBJECT collision_248 = {
#include "stg40/terrain/collision_248.inc"
};
static NJS_OBJECT collision_249 = {
#include "stg40/terrain/collision_249.inc"
};
static NJS_OBJECT collision_250 = {
#include "stg40/terrain/collision_250.inc"
};
static NJS_OBJECT collision_251 = {
#include "stg40/terrain/collision_251.inc"
};
static NJS_OBJECT collision_252 = {
#include "stg40/terrain/collision_252.inc"
};
static NJS_OBJECT collision_253 = {
#include "stg40/terrain/collision_253.inc"
};
static NJS_OBJECT collision_254 = {
#include "stg40/terrain/collision_254.inc"
};
static NJS_OBJECT collision_255 = {
#include "stg40/terrain/collision_255.inc"
};
static NJS_OBJECT collision_256 = {
#include "stg40/terrain/collision_256.inc"
};
static NJS_OBJECT collision_257 = {
#include "stg40/terrain/collision_257.inc"
};
static NJS_OBJECT collision_258 = {
#include "stg40/terrain/collision_258.inc"
};
static NJS_OBJECT collision_259 = {
#include "stg40/terrain/collision_259.inc"
};
static NJS_OBJECT collision_260 = {
#include "stg40/terrain/collision_260.inc"
};
static NJS_OBJECT collision_261 = {
#include "stg40/terrain/collision_261.inc"
};
static NJS_OBJECT collision_262 = {
#include "stg40/terrain/collision_262.inc"
};
static NJS_OBJECT collision_263 = {
#include "stg40/terrain/collision_263.inc"
};
static NJS_OBJECT collision_264 = {
#include "stg40/terrain/collision_264.inc"
};
static NJS_OBJECT collision_265 = {
#include "stg40/terrain/collision_265.inc"
};
static NJS_OBJECT collision_266 = {
#include "stg40/terrain/collision_266.inc"
};
static NJS_OBJECT collision_267 = {
#include "stg40/terrain/collision_267.inc"
};
static NJS_OBJECT collision_268 = {
#include "stg40/terrain/collision_268.inc"
};
static NJS_OBJECT collision_269 = {
#include "stg40/terrain/collision_269.inc"
};
static NJS_OBJECT collision_270 = {
#include "stg40/terrain/collision_270.inc"
};
static NJS_OBJECT collision_271 = {
#include "stg40/terrain/collision_271.inc"
};
static NJS_OBJECT collision_272 = {
#include "stg40/terrain/collision_272.inc"
};
static NJS_OBJECT collision_273 = {
#include "stg40/terrain/collision_273.inc"
};
static NJS_OBJECT collision_279 = {
#include "stg40/terrain/collision_279.inc"
};
static NJS_OBJECT collision_282 = {
#include "stg40/terrain/collision_282.inc"
};
static NJS_OBJECT collision_283 = {
#include "stg40/terrain/collision_283.inc"
};
static NJS_OBJECT collision_284 = {
#include "stg40/terrain/collision_284.inc"
};
static NJS_OBJECT collision_286 = {
#include "stg40/terrain/collision_286.inc"
};
static NJS_OBJECT collision_288 = {
#include "stg40/terrain/collision_288.inc"
};
static NJS_OBJECT collision_291 = {
#include "stg40/terrain/collision_291.inc"
};
static NJS_OBJECT collision_293 = {
#include "stg40/terrain/collision_293.inc"
};
static NJS_OBJECT collision_294 = {
#include "stg40/terrain/collision_294.inc"
};
static NJS_OBJECT collision_295 = {
#include "stg40/terrain/collision_295.inc"
};
static NJS_OBJECT collision_296 = {
#include "stg40/terrain/collision_296.inc"
};
static NJS_OBJECT collision_297 = {
#include "stg40/terrain/collision_297.inc"
};
static NJS_OBJECT collision_298 = {
#include "stg40/terrain/collision_298.inc"
};
static NJS_OBJECT collision_299 = {
#include "stg40/terrain/collision_299.inc"
};
static NJS_OBJECT collision_300 = {
#include "stg40/terrain/collision_300.inc"
};
static NJS_OBJECT collision_301 = {
#include "stg40/terrain/collision_301.inc"
};
static NJS_OBJECT collision_302 = {
#include "stg40/terrain/collision_302.inc"
};
static NJS_OBJECT collision_303 = {
#include "stg40/terrain/collision_303.inc"
};
static NJS_OBJECT collision_304 = {
#include "stg40/terrain/collision_304.inc"
};
static NJS_OBJECT collision_305 = {
#include "stg40/terrain/collision_305.inc"
};
static NJS_OBJECT collision_306 = {
#include "stg40/terrain/collision_306.inc"
};
static NJS_OBJECT collision_307 = {
#include "stg40/terrain/collision_307.inc"
};
static NJS_OBJECT collision_308 = {
#include "stg40/terrain/collision_308.inc"
};
static NJS_OBJECT collision_309 = {
#include "stg40/terrain/collision_309.inc"
};
static NJS_OBJECT collision_310 = {
#include "stg40/terrain/collision_310.inc"
};
static NJS_OBJECT collision_311 = {
#include "stg40/terrain/collision_311.inc"
};
static NJS_OBJECT collision_312 = {
#include "stg40/terrain/collision_312.inc"
};
static NJS_OBJECT collision_313 = {
#include "stg40/terrain/collision_313.inc"
};
static NJS_OBJECT collision_314 = {
#include "stg40/terrain/collision_314.inc"
};
static NJS_OBJECT collision_315 = {
#include "stg40/terrain/collision_315.inc"
};
static NJS_OBJECT collision_316 = {
#include "stg40/terrain/collision_316.inc"
};
static NJS_OBJECT collision_317 = {
#include "stg40/terrain/collision_317.inc"
};
static NJS_OBJECT collision_318 = {
#include "stg40/terrain/collision_318.inc"
};
static NJS_OBJECT collision_319 = {
#include "stg40/terrain/collision_319.inc"
};
static NJS_OBJECT collision_320 = {
#include "stg40/terrain/collision_320.inc"
};
static NJS_OBJECT collision_321 = {
#include "stg40/terrain/collision_321.inc"
};
static NJS_OBJECT collision_322 = {
#include "stg40/terrain/collision_322.inc"
};
static NJS_OBJECT collision_323 = {
#include "stg40/terrain/collision_323.inc"
};
static NJS_OBJECT collision_324 = {
#include "stg40/terrain/collision_324.inc"
};
static NJS_OBJECT collision_325 = {
#include "stg40/terrain/collision_325.inc"
};
static NJS_OBJECT collision_326 = {
#include "stg40/terrain/collision_326.inc"
};
static NJS_OBJECT collision_327 = {
#include "stg40/terrain/collision_327.inc"
};
static NJS_OBJECT collision_328 = {
#include "stg40/terrain/collision_328.inc"
};
static NJS_OBJECT collision_329 = {
#include "stg40/terrain/collision_329.inc"
};
static NJS_OBJECT collision_330 = {
#include "stg40/terrain/collision_330.inc"
};
static NJS_OBJECT collision_331 = {
#include "stg40/terrain/collision_331.inc"
};
static NJS_OBJECT collision_332 = {
#include "stg40/terrain/collision_332.inc"
};
static NJS_OBJECT collision_333 = {
#include "stg40/terrain/collision_333.inc"
};
static NJS_OBJECT collision_334 = {
#include "stg40/terrain/collision_334.inc"
};
static NJS_OBJECT collision_335 = {
#include "stg40/terrain/collision_335.inc"
};
static NJS_OBJECT collision_336 = {
#include "stg40/terrain/collision_336.inc"
};
static NJS_OBJECT collision_337 = {
#include "stg40/terrain/collision_337.inc"
};
static NJS_OBJECT collision_338 = {
#include "stg40/terrain/collision_338.inc"
};
static NJS_OBJECT collision_339 = {
#include "stg40/terrain/collision_339.inc"
};
static NJS_OBJECT collision_340 = {
#include "stg40/terrain/collision_340.inc"
};
static NJS_OBJECT collision_341 = {
#include "stg40/terrain/collision_341.inc"
};
static NJS_OBJECT collision_342 = {
#include "stg40/terrain/collision_342.inc"
};
static NJS_OBJECT collision_343 = {
#include "stg40/terrain/collision_343.inc"
};
static NJS_OBJECT collision_344 = {
#include "stg40/terrain/collision_344.inc"
};
static NJS_OBJECT collision_345 = {
#include "stg40/terrain/collision_345.inc"
};
static NJS_OBJECT collision_346 = {
#include "stg40/terrain/collision_346.inc"
};
static NJS_OBJECT collision_347 = {
#include "stg40/terrain/collision_347.inc"
};
static NJS_OBJECT collision_348 = {
#include "stg40/terrain/collision_348.inc"
};
static NJS_OBJECT collision_349 = {
#include "stg40/terrain/collision_349.inc"
};
static NJS_OBJECT collision_350 = {
#include "stg40/terrain/collision_350.inc"
};
static NJS_OBJECT collision_351 = {
#include "stg40/terrain/collision_351.inc"
};
static NJS_OBJECT collision_352 = {
#include "stg40/terrain/collision_352.inc"
};
static NJS_OBJECT collision_353 = {
#include "stg40/terrain/collision_353.inc"
};
static NJS_OBJECT collision_354 = {
#include "stg40/terrain/collision_354.inc"
};
static NJS_OBJECT collision_355 = {
#include "stg40/terrain/collision_355.inc"
};
static NJS_OBJECT collision_356 = {
#include "stg40/terrain/collision_356.inc"
};
static NJS_OBJECT collision_357 = {
#include "stg40/terrain/collision_357.inc"
};
static NJS_OBJECT collision_358 = {
#include "stg40/terrain/collision_358.inc"
};
static NJS_OBJECT collision_359 = {
#include "stg40/terrain/collision_359.inc"
};
static NJS_OBJECT collision_360 = {
#include "stg40/terrain/collision_360.inc"
};
static NJS_OBJECT collision_361 = {
#include "stg40/terrain/collision_361.inc"
};
static NJS_OBJECT collision_362 = {
#include "stg40/terrain/collision_362.inc"
};
static NJS_OBJECT collision_363 = {
#include "stg40/terrain/collision_363.inc"
};
static NJS_OBJECT collision_365 = {
#include "stg40/terrain/collision_365.inc"
};
static NJS_OBJECT collision_366 = {
#include "stg40/terrain/collision_366.inc"
};
static NJS_OBJECT collision_367 = {
#include "stg40/terrain/collision_367.inc"
};
static NJS_OBJECT collision_368 = {
#include "stg40/terrain/collision_368.inc"
};
static NJS_OBJECT collision_369 = {
#include "stg40/terrain/collision_369.inc"
};
static NJS_OBJECT collision_370 = {
#include "stg40/terrain/collision_370.inc"
};
static NJS_OBJECT collision_371 = {
#include "stg40/terrain/collision_371.inc"
};
static NJS_OBJECT collision_372 = {
#include "stg40/terrain/collision_372.inc"
};
static NJS_OBJECT collision_373 = {
#include "stg40/terrain/collision_373.inc"
};
static NJS_OBJECT collision_374 = {
#include "stg40/terrain/collision_374.inc"
};
static NJS_OBJECT collision_375 = {
#include "stg40/terrain/collision_375.inc"
};
static NJS_OBJECT collision_376 = {
#include "stg40/terrain/collision_376.inc"
};
static NJS_OBJECT collision_377 = {
#include "stg40/terrain/collision_377.inc"
};
static NJS_OBJECT collision_378 = {
#include "stg40/terrain/collision_378.inc"
};
static NJS_OBJECT collision_379 = {
#include "stg40/terrain/collision_379.inc"
};
static NJS_OBJECT collision_380 = {
#include "stg40/terrain/collision_380.inc"
};
static NJS_OBJECT collision_381 = {
#include "stg40/terrain/collision_381.inc"
};
static NJS_OBJECT collision_382 = {
#include "stg40/terrain/collision_382.inc"
};
static NJS_OBJECT collision_383 = {
#include "stg40/terrain/collision_383.inc"
};
static NJS_OBJECT collision_384 = {
#include "stg40/terrain/collision_384.inc"
};
static NJS_OBJECT collision_385 = {
#include "stg40/terrain/collision_385.inc"
};
static NJS_OBJECT collision_386 = {
#include "stg40/terrain/collision_386.inc"
};
static NJS_OBJECT collision_387 = {
#include "stg40/terrain/collision_387.inc"
};
static NJS_OBJECT collision_388 = {
#include "stg40/terrain/collision_388.inc"
};
static NJS_OBJECT collision_390 = {
#include "stg40/terrain/collision_390.inc"
};
static NJS_OBJECT collision_392 = {
#include "stg40/terrain/collision_392.inc"
};
