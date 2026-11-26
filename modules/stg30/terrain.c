#include <ninja.h>
#include <sg_xpt.h>

#include "collision/world.h"
#include "stg30/terrain.h"

/* the land's file names, which its land table and texture list point at. each
 * starts on a 4-byte boundary */
const char rush_land_filenames[462] = {
#include "stg30/terrain/rush_land_filenames.inc"
};

/* STG30.PRS's land, texture lists, Basic models and chunk models, from the
 * disc. its D lies between sky_land_tex.c and object_archives.c */

/* the data up to the last that a pointer names ahead of its definition, as
 * tentative definitions in the disc's order: GCC places each datum at its
 * first definition */
static struct land_entry entries[1527];
struct land_table rush_land;
static NJS_TEXNAME textures[38];
NJS_TEXLIST rush_land_texlist;
static NJS_MATERIAL collision_0_materials[1];
static Sint16 collision_0_faces[46];
static NJS_MESHSET collision_0_meshsets[1];
static NJS_POINT3 collision_0_points[30];
static NJS_MODEL collision_0_model;
static NJS_OBJECT collision_0;
static NJS_MATERIAL collision_1_materials[1];
static Sint16 collision_1_faces[27];
static NJS_MESHSET collision_1_meshsets[1];
static NJS_POINT3 collision_1_points[16];
static NJS_MODEL collision_1_model;
static NJS_OBJECT collision_1;
static NJS_MATERIAL collision_5_materials[1];
static Sint16 collision_5_faces[46];
static NJS_MESHSET collision_5_meshsets[1];
static NJS_POINT3 collision_5_points[30];
static NJS_MODEL collision_5_model;
static NJS_OBJECT collision_5;
static NJS_MATERIAL collision_6_materials[1];
static Sint16 collision_6_faces[48];
static NJS_MESHSET collision_6_meshsets[1];
static NJS_POINT3 collision_6_points[30];
static NJS_MODEL collision_6_model;
static NJS_OBJECT collision_6;
static NJS_MATERIAL collision_7_materials[1];
static Sint16 collision_7_faces[46];
static NJS_MESHSET collision_7_meshsets[1];
static NJS_POINT3 collision_7_points[30];
static NJS_MODEL collision_7_model;
static NJS_OBJECT collision_7;
static NJS_MATERIAL collision_10_materials[1];
static Sint16 collision_10_faces[39];
static NJS_MESHSET collision_10_meshsets[1];
static NJS_POINT3 collision_10_points[30];
static NJS_MODEL collision_10_model;
static NJS_OBJECT collision_10;
static NJS_MATERIAL collision_11_materials[1];
static Sint16 collision_11_faces[39];
static NJS_MESHSET collision_11_meshsets[1];
static NJS_POINT3 collision_11_points[30];
static NJS_MODEL collision_11_model;
static NJS_OBJECT collision_11;
static NJS_MATERIAL collision_14_materials[1];
static Sint16 collision_14_faces[53];
static NJS_MESHSET collision_14_meshsets[1];
static NJS_POINT3 collision_14_points[24];
static NJS_MODEL collision_14_model;
static NJS_OBJECT collision_14;
static NJS_MATERIAL collision_19_materials[1];
static Sint16 collision_19_faces[188];
static NJS_MESHSET collision_19_meshsets[1];
static NJS_POINT3 collision_19_points[80];
static NJS_MODEL collision_19_model;
static NJS_OBJECT collision_19;
static NJS_MATERIAL collision_20_materials[1];
static Sint16 collision_20_faces[152];
static NJS_MESHSET collision_20_meshsets[1];
static NJS_POINT3 collision_20_points[73];
static NJS_MODEL collision_20_model;
static NJS_OBJECT collision_20;
static NJS_MATERIAL collision_21_materials[1];
static Sint16 collision_21_faces[27];
static NJS_MESHSET collision_21_meshsets[1];
static NJS_POINT3 collision_21_points[16];
static NJS_MODEL collision_21_model;
static NJS_OBJECT collision_21;
static NJS_MATERIAL collision_25_materials[1];
static Sint16 collision_25_faces[27];
static NJS_MESHSET collision_25_meshsets[1];
static NJS_POINT3 collision_25_points[16];
static NJS_MODEL collision_25_model;
static NJS_OBJECT collision_25;
static NJS_MATERIAL collision_95_materials[1];
static Sint16 collision_95_faces[77];
static NJS_MESHSET collision_95_meshsets[1];
static NJS_POINT3 collision_95_points[34];
static NJS_MODEL collision_95_model;
static NJS_OBJECT collision_95;
static NJS_MATERIAL collision_98_materials[1];
static Sint16 collision_98_faces[22];
static NJS_MESHSET collision_98_meshsets[1];
static NJS_POINT3 collision_98_points[12];
static NJS_MODEL collision_98_model;
static NJS_OBJECT collision_98;
static NJS_MATERIAL collision_104_materials[1];
static Sint16 collision_104_faces[6];
static NJS_MESHSET collision_104_meshsets[1];
static NJS_POINT3 collision_104_points[5];
static NJS_MODEL collision_104_model;
static NJS_OBJECT collision_104;
static NJS_MATERIAL collision_105_materials[1];
static Sint16 collision_105_faces[39];
static NJS_MESHSET collision_105_meshsets[1];
static NJS_POINT3 collision_105_points[24];
static NJS_MODEL collision_105_model;
static NJS_OBJECT collision_105;
static NJS_MATERIAL collision_106_materials[1];
static Sint16 collision_106_faces[27];
static NJS_MESHSET collision_106_meshsets[1];
static NJS_POINT3 collision_106_points[16];
static NJS_MODEL collision_106_model;
static NJS_OBJECT collision_106;
static NJS_MATERIAL collision_110_materials[1];
static Sint16 collision_110_faces[55];
static NJS_MESHSET collision_110_meshsets[1];
static NJS_POINT3 collision_110_points[30];
static NJS_MODEL collision_110_model;
static NJS_OBJECT collision_110;
static NJS_MATERIAL collision_111_materials[1];
static Sint16 collision_111_faces[39];
static NJS_MESHSET collision_111_meshsets[1];
static NJS_POINT3 collision_111_points[18];
static NJS_MODEL collision_111_model;
static NJS_OBJECT collision_111;
static NJS_MATERIAL collision_112_materials[1];
static Sint16 collision_112_faces[260];
static NJS_MESHSET collision_112_meshsets[1];
static NJS_POINT3 collision_112_points[120];
static NJS_MODEL collision_112_model;
static NJS_OBJECT collision_112;
static NJS_MATERIAL collision_113_materials[1];
static Sint16 collision_113_faces[46];
static NJS_MESHSET collision_113_meshsets[1];
static NJS_POINT3 collision_113_points[19];
static NJS_MODEL collision_113_model;
static NJS_OBJECT collision_113;
static NJS_MATERIAL collision_115_materials[1];
static Sint16 collision_115_faces[51];
static NJS_MESHSET collision_115_meshsets[1];
static NJS_POINT3 collision_115_points[26];
static NJS_MODEL collision_115_model;
static NJS_OBJECT collision_115;
static NJS_MATERIAL collision_119_materials[1];
static Sint16 collision_119_faces[260];
static NJS_MESHSET collision_119_meshsets[1];
static NJS_POINT3 collision_119_points[120];
static NJS_MODEL collision_119_model;
static NJS_OBJECT collision_119;
static NJS_MATERIAL collision_120_materials[1];
static Sint16 collision_120_faces[263];
static NJS_MESHSET collision_120_meshsets[1];
static NJS_POINT3 collision_120_points[120];
static NJS_MODEL collision_120_model;
static NJS_OBJECT collision_120;
static NJS_MATERIAL collision_121_materials[1];
static Sint16 collision_121_faces[122];
static NJS_MESHSET collision_121_meshsets[1];
static NJS_POINT3 collision_121_points[63];
static NJS_MODEL collision_121_model;
static NJS_OBJECT collision_121;
static NJS_MATERIAL collision_122_materials[1];
static Sint16 collision_122_faces[63];
static NJS_MESHSET collision_122_meshsets[1];
static NJS_POINT3 collision_122_points[32];
static NJS_MODEL collision_122_model;
static NJS_OBJECT collision_122;
static NJS_MATERIAL collision_125_materials[1];
static Sint16 collision_125_faces[18];
static NJS_MESHSET collision_125_meshsets[1];
static NJS_POINT3 collision_125_points[12];
static NJS_MODEL collision_125_model;
static NJS_OBJECT collision_125;
static NJS_MATERIAL collision_127_materials[1];
static Sint16 collision_127_faces[132];
static NJS_MESHSET collision_127_meshsets[1];
static NJS_POINT3 collision_127_points[77];
static NJS_MODEL collision_127_model;
static NJS_OBJECT collision_127;
static NJS_MATERIAL collision_129_materials[1];
static Sint16 collision_129_faces[33];
static NJS_MESHSET collision_129_meshsets[1];
static NJS_POINT3 collision_129_points[20];
static NJS_MODEL collision_129_model;
static NJS_OBJECT collision_129;
static NJS_MATERIAL collision_133_materials[1];
static Sint16 collision_133_faces[36];
static NJS_MESHSET collision_133_meshsets[1];
static NJS_POINT3 collision_133_points[20];
static NJS_MODEL collision_133_model;
static NJS_OBJECT collision_133;
static NJS_MATERIAL collision_144_materials[1];
static Sint16 collision_144_faces[202];
static NJS_MESHSET collision_144_meshsets[1];
static NJS_POINT3 collision_144_points[93];
static NJS_MODEL collision_144_model;
static NJS_OBJECT collision_144;
static NJS_MATERIAL collision_146_materials[1];
static Sint16 collision_146_faces[45];
static NJS_MESHSET collision_146_meshsets[1];
static NJS_POINT3 collision_146_points[24];
static NJS_MODEL collision_146_model;
static NJS_OBJECT collision_146;
static NJS_MATERIAL collision_148_materials[1];
static Sint16 collision_148_faces[13];
static NJS_MESHSET collision_148_meshsets[1];
static NJS_POINT3 collision_148_points[12];
static NJS_MODEL collision_148_model;
static NJS_OBJECT collision_148;
static NJS_MATERIAL collision_150_materials[1];
static Sint16 collision_150_faces[26];
static NJS_MESHSET collision_150_meshsets[1];
static NJS_POINT3 collision_150_points[16];
static NJS_MODEL collision_150_model;
static NJS_OBJECT collision_150;
static NJS_MATERIAL collision_151_materials[1];
static Sint16 collision_151_faces[64];
static NJS_MESHSET collision_151_meshsets[1];
static NJS_POINT3 collision_151_points[33];
static NJS_MODEL collision_151_model;
static NJS_OBJECT collision_151;
static NJS_MATERIAL collision_152_materials[1];
static Sint16 collision_152_faces[81];
static NJS_MESHSET collision_152_meshsets[1];
static NJS_POINT3 collision_152_points[48];
static NJS_MODEL collision_152_model;
static NJS_OBJECT collision_152;
static NJS_MATERIAL collision_153_materials[1];
static Sint16 collision_153_faces[26];
static NJS_MESHSET collision_153_meshsets[1];
static NJS_POINT3 collision_153_points[16];
static NJS_MODEL collision_153_model;
static NJS_OBJECT collision_153;
static NJS_MATERIAL collision_154_materials[1];
static Sint16 collision_154_faces[91];
static NJS_MESHSET collision_154_meshsets[1];
static NJS_POINT3 collision_154_points[40];
static NJS_MODEL collision_154_model;
static NJS_OBJECT collision_154;
static NJS_MATERIAL collision_155_materials[1];
static Sint16 collision_155_faces[98];
static NJS_MESHSET collision_155_meshsets[1];
static NJS_POINT3 collision_155_points[52];
static NJS_MODEL collision_155_model;
static NJS_OBJECT collision_155;
static NJS_MATERIAL collision_156_materials[1];
static Sint16 collision_156_faces[63];
static NJS_MESHSET collision_156_meshsets[1];
static NJS_POINT3 collision_156_points[36];
static NJS_MODEL collision_156_model;
static NJS_OBJECT collision_156;
static NJS_MATERIAL collision_157_materials[1];
static Sint16 collision_157_faces[63];
static NJS_MESHSET collision_157_meshsets[1];
static NJS_POINT3 collision_157_points[36];
static NJS_MODEL collision_157_model;
static NJS_OBJECT collision_157;
static NJS_MATERIAL collision_158_materials[1];
static Sint16 collision_158_faces[19];
static NJS_MESHSET collision_158_meshsets[1];
static NJS_POINT3 collision_158_points[16];
static NJS_MODEL collision_158_model;
static NJS_OBJECT collision_158;
static NJS_MATERIAL collision_159_materials[1];
static Sint16 collision_159_faces[19];
static NJS_MESHSET collision_159_meshsets[1];
static NJS_POINT3 collision_159_points[16];
static NJS_MODEL collision_159_model;
static NJS_OBJECT collision_159;
static NJS_MATERIAL collision_160_materials[1];
static Sint16 collision_160_faces[42];
static NJS_MESHSET collision_160_meshsets[1];
static NJS_POINT3 collision_160_points[30];
static NJS_MODEL collision_160_model;
static NJS_OBJECT collision_160;
static NJS_MATERIAL collision_163_materials[1];
static Sint16 collision_163_faces[273];
static NJS_MESHSET collision_163_meshsets[1];
static NJS_POINT3 collision_163_points[140];
static NJS_MODEL collision_163_model;
static NJS_OBJECT collision_163;
static NJS_MATERIAL collision_166_materials[1];
static Sint16 collision_166_faces[193];
static NJS_MESHSET collision_166_meshsets[1];
static NJS_POINT3 collision_166_points[93];
static NJS_MODEL collision_166_model;
static NJS_OBJECT collision_166;
static NJS_MATERIAL collision_167_materials[1];
static Sint16 collision_167_faces[166];
static NJS_MESHSET collision_167_meshsets[1];
static NJS_POINT3 collision_167_points[75];
static NJS_MODEL collision_167_model;
static NJS_OBJECT collision_167;
static NJS_MATERIAL collision_168_materials[1];
static Sint16 collision_168_faces[22];
static NJS_MESHSET collision_168_meshsets[1];
static NJS_POINT3 collision_168_points[12];
static NJS_MODEL collision_168_model;
static NJS_OBJECT collision_168;
static NJS_MATERIAL collision_169_materials[1];
static Sint16 collision_169_faces[42];
static NJS_MESHSET collision_169_meshsets[1];
static NJS_POINT3 collision_169_points[30];
static NJS_MODEL collision_169_model;
static NJS_OBJECT collision_169;
static NJS_MATERIAL collision_172_materials[1];
static Sint16 collision_172_faces[273];
static NJS_MESHSET collision_172_meshsets[1];
static NJS_POINT3 collision_172_points[140];
static NJS_MODEL collision_172_model;
static NJS_OBJECT collision_172;
static NJS_MATERIAL collision_174_materials[1];
static Sint16 collision_174_faces[215];
static NJS_MESHSET collision_174_meshsets[1];
static NJS_POINT3 collision_174_points[102];
static NJS_MODEL collision_174_model;
static NJS_OBJECT collision_174;
static NJS_MATERIAL collision_175_materials[1];
static Sint16 collision_175_faces[139];
static NJS_MESHSET collision_175_meshsets[1];
static NJS_POINT3 collision_175_points[84];
static NJS_MODEL collision_175_model;
static NJS_OBJECT collision_175;
static NJS_MATERIAL collision_176_materials[1];
static Sint16 collision_176_faces[125];
static NJS_MESHSET collision_176_meshsets[1];
static NJS_POINT3 collision_176_points[61];
static NJS_MODEL collision_176_model;
static NJS_OBJECT collision_176;
static NJS_MATERIAL collision_177_materials[1];
static Sint16 collision_177_faces[67];
static NJS_MESHSET collision_177_meshsets[1];
static NJS_POINT3 collision_177_points[33];
static NJS_MODEL collision_177_model;
static NJS_OBJECT collision_177;
static NJS_MATERIAL collision_178_materials[1];
static Sint16 collision_178_faces[146];
static NJS_MESHSET collision_178_meshsets[1];
static NJS_POINT3 collision_178_points[77];
static NJS_MODEL collision_178_model;
static NJS_OBJECT collision_178;
static NJS_MATERIAL collision_179_materials[1];
static Sint16 collision_179_faces[64];
static NJS_MESHSET collision_179_meshsets[1];
static NJS_POINT3 collision_179_points[33];
static NJS_MODEL collision_179_model;
static NJS_OBJECT collision_179;
static NJS_MATERIAL collision_180_materials[1];
static Sint16 collision_180_faces[68];
static NJS_MESHSET collision_180_meshsets[1];
static NJS_POINT3 collision_180_points[42];
static NJS_MODEL collision_180_model;
static NJS_OBJECT collision_180;
static NJS_MATERIAL collision_181_materials[1];
static Sint16 collision_181_faces[88];
static NJS_MESHSET collision_181_meshsets[1];
static NJS_POINT3 collision_181_points[52];
static NJS_MODEL collision_181_model;
static NJS_OBJECT collision_181;
static NJS_MATERIAL collision_182_materials[1];
static Sint16 collision_182_faces[185];
static NJS_MESHSET collision_182_meshsets[1];
static NJS_POINT3 collision_182_points[81];
static NJS_MODEL collision_182_model;
static NJS_OBJECT collision_182;
static NJS_MATERIAL collision_183_materials[1];
static Sint16 collision_183_faces[145];
static NJS_MESHSET collision_183_meshsets[1];
static NJS_POINT3 collision_183_points[71];
static NJS_MODEL collision_183_model;
static NJS_OBJECT collision_183;
static NJS_MATERIAL collision_184_materials[1];
static Sint16 collision_184_faces[122];
static NJS_MESHSET collision_184_meshsets[1];
static NJS_POINT3 collision_184_points[59];
static NJS_MODEL collision_184_model;
static NJS_OBJECT collision_184;
static NJS_MATERIAL collision_185_materials[1];
static Sint16 collision_185_faces[19];
static NJS_MESHSET collision_185_meshsets[1];
static NJS_POINT3 collision_185_points[16];
static NJS_MODEL collision_185_model;
static NJS_OBJECT collision_185;
static NJS_MATERIAL collision_186_materials[1];
static Sint16 collision_186_faces[105];
static NJS_MESHSET collision_186_meshsets[1];
static NJS_POINT3 collision_186_points[47];
static NJS_MODEL collision_186_model;
static NJS_OBJECT collision_186;
static NJS_MATERIAL collision_187_materials[1];
static Sint16 collision_187_faces[221];
static NJS_MESHSET collision_187_meshsets[1];
static NJS_POINT3 collision_187_points[112];
static NJS_MODEL collision_187_model;
static NJS_OBJECT collision_187;
static NJS_MATERIAL collision_188_materials[1];
static Sint16 collision_188_faces[82];
static NJS_MESHSET collision_188_meshsets[1];
static NJS_POINT3 collision_188_points[45];
static NJS_MODEL collision_188_model;
static NJS_OBJECT collision_188;
static NJS_MATERIAL collision_189_materials[1];
static Sint16 collision_189_faces[132];
static NJS_MESHSET collision_189_meshsets[1];
static NJS_POINT3 collision_189_points[62];
static NJS_MODEL collision_189_model;
static NJS_OBJECT collision_189;
static NJS_MATERIAL collision_190_materials[1];
static Sint16 collision_190_faces[63];
static NJS_MESHSET collision_190_meshsets[1];
static NJS_POINT3 collision_190_points[36];
static NJS_MODEL collision_190_model;
static NJS_OBJECT collision_190;
static NJS_MATERIAL collision_191_materials[1];
static Sint16 collision_191_faces[181];
static NJS_MESHSET collision_191_meshsets[1];
static NJS_POINT3 collision_191_points[80];
static NJS_MODEL collision_191_model;
static NJS_OBJECT collision_191;
static NJS_MATERIAL collision_192_materials[1];
static Sint16 collision_192_faces[166];
static NJS_MESHSET collision_192_meshsets[1];
static NJS_POINT3 collision_192_points[75];
static NJS_MODEL collision_192_model;
static NJS_OBJECT collision_192;
static NJS_MATERIAL collision_193_materials[1];
static Sint16 collision_193_faces[11];
static NJS_MESHSET collision_193_meshsets[1];
static NJS_POINT3 collision_193_points[8];
static NJS_MODEL collision_193_model;
static NJS_OBJECT collision_193;
static NJS_MATERIAL collision_194_materials[1];
static Sint16 collision_194_faces[65];
static NJS_MESHSET collision_194_meshsets[1];
static NJS_POINT3 collision_194_points[33];
static NJS_MODEL collision_194_model;
static NJS_OBJECT collision_194;
static NJS_MATERIAL collision_195_materials[1];
static Sint16 collision_195_faces[288];
static NJS_MESHSET collision_195_meshsets[1];
static NJS_POINT3 collision_195_points[141];
static NJS_MODEL collision_195_model;
static NJS_OBJECT collision_195;
static NJS_MATERIAL collision_196_materials[1];
static Sint16 collision_196_faces[67];
static NJS_MESHSET collision_196_meshsets[1];
static NJS_POINT3 collision_196_points[41];
static NJS_MODEL collision_196_model;
static NJS_OBJECT collision_196;
static NJS_MATERIAL collision_197_materials[1];
static Sint16 collision_197_faces[90];
static NJS_MESHSET collision_197_meshsets[1];
static NJS_POINT3 collision_197_points[48];
static NJS_MODEL collision_197_model;
static NJS_OBJECT collision_197;
static NJS_MATERIAL collision_198_materials[1];
static Sint16 collision_198_faces[154];
static NJS_MESHSET collision_198_meshsets[1];
static NJS_POINT3 collision_198_points[68];
static NJS_MODEL collision_198_model;
static NJS_OBJECT collision_198;
static NJS_MATERIAL collision_199_materials[1];
static Sint16 collision_199_faces[188];
static NJS_MESHSET collision_199_meshsets[1];
static NJS_POINT3 collision_199_points[83];
static NJS_MODEL collision_199_model;
static NJS_OBJECT collision_199;
static NJS_MATERIAL collision_200_materials[1];
static Sint16 collision_200_faces[72];
static NJS_MESHSET collision_200_meshsets[1];
static NJS_POINT3 collision_200_points[40];
static NJS_MODEL collision_200_model;
static NJS_OBJECT collision_200;
static NJS_MATERIAL collision_201_materials[1];
static Sint16 collision_201_faces[107];
static NJS_MESHSET collision_201_meshsets[1];
static NJS_POINT3 collision_201_points[58];
static NJS_MODEL collision_201_model;
static NJS_OBJECT collision_201;
static NJS_MATERIAL collision_202_materials[1];
static Sint16 collision_202_faces[64];
static NJS_MESHSET collision_202_meshsets[1];
static NJS_POINT3 collision_202_points[33];
static NJS_MODEL collision_202_model;
static NJS_OBJECT collision_202;
static NJS_MATERIAL collision_203_materials[1];
static Sint16 collision_203_faces[72];
static NJS_MESHSET collision_203_meshsets[1];
static NJS_POINT3 collision_203_points[40];
static NJS_MODEL collision_203_model;
static NJS_OBJECT collision_203;
static NJS_MATERIAL collision_204_materials[1];
static Sint16 collision_204_faces[44];
static NJS_MESHSET collision_204_meshsets[1];
static NJS_POINT3 collision_204_points[25];
static NJS_MODEL collision_204_model;
static NJS_OBJECT collision_204;
static NJS_MATERIAL collision_205_materials[1];
static Sint16 collision_205_faces[135];
static NJS_MESHSET collision_205_meshsets[1];
static NJS_POINT3 collision_205_points[59];
static NJS_MODEL collision_205_model;
static NJS_OBJECT collision_205;
static NJS_MATERIAL collision_206_materials[1];
static Sint16 collision_206_faces[155];
static NJS_MESHSET collision_206_meshsets[1];
static NJS_POINT3 collision_206_points[69];
static NJS_MODEL collision_206_model;
static NJS_OBJECT collision_206;
static NJS_MATERIAL collision_207_materials[1];
static Sint16 collision_207_faces[71];
static NJS_MESHSET collision_207_meshsets[1];
static NJS_POINT3 collision_207_points[34];
static NJS_MODEL collision_207_model;
static NJS_OBJECT collision_207;
static NJS_MATERIAL collision_208_materials[1];
static Sint16 collision_208_faces[64];
static NJS_MESHSET collision_208_meshsets[1];
static NJS_POINT3 collision_208_points[33];
static NJS_MODEL collision_208_model;
static NJS_OBJECT collision_208;
static NJS_MATERIAL collision_209_materials[1];
static Sint16 collision_209_faces[138];
static NJS_MESHSET collision_209_meshsets[1];
static NJS_POINT3 collision_209_points[77];
static NJS_MODEL collision_209_model;
static NJS_OBJECT collision_209;
static NJS_MATERIAL collision_210_materials[1];
static Sint16 collision_210_faces[137];
static NJS_MESHSET collision_210_meshsets[1];
static NJS_POINT3 collision_210_points[65];
static NJS_MODEL collision_210_model;
static NJS_OBJECT collision_210;
static NJS_MATERIAL collision_211_materials[1];
static Sint16 collision_211_faces[154];
static NJS_MESHSET collision_211_meshsets[1];
static NJS_POINT3 collision_211_points[68];
static NJS_MODEL collision_211_model;
static NJS_OBJECT collision_211;
static NJS_MATERIAL collision_212_materials[1];
static Sint16 collision_212_faces[45];
static NJS_MESHSET collision_212_meshsets[1];
static NJS_POINT3 collision_212_points[28];
static NJS_MODEL collision_212_model;
static NJS_OBJECT collision_212;
static NJS_MATERIAL collision_213_materials[1];
static Sint16 collision_213_faces[45];
static NJS_MESHSET collision_213_meshsets[1];
static NJS_POINT3 collision_213_points[28];
static NJS_MODEL collision_213_model;
static NJS_OBJECT collision_213;
static NJS_MATERIAL collision_214_materials[1];
static Sint16 collision_214_faces[92];
static NJS_MESHSET collision_214_meshsets[1];
static NJS_POINT3 collision_214_points[55];
static NJS_MODEL collision_214_model;
static NJS_OBJECT collision_214;
static NJS_MATERIAL collision_215_materials[1];
static Sint16 collision_215_faces[92];
static NJS_MESHSET collision_215_meshsets[1];
static NJS_POINT3 collision_215_points[55];
static NJS_MODEL collision_215_model;
static NJS_OBJECT collision_215;
static NJS_MATERIAL collision_216_materials[1];
static Sint16 collision_216_faces[145];
static NJS_MESHSET collision_216_meshsets[1];
static NJS_POINT3 collision_216_points[66];
static NJS_MODEL collision_216_model;
static NJS_OBJECT collision_216;
static NJS_MATERIAL collision_217_materials[1];
static Sint16 collision_217_faces[67];
static NJS_MESHSET collision_217_meshsets[1];
static NJS_POINT3 collision_217_points[33];
static NJS_MODEL collision_217_model;
static NJS_OBJECT collision_217;
static NJS_MATERIAL collision_218_materials[1];
static Sint16 collision_218_faces[63];
static NJS_MESHSET collision_218_meshsets[1];
static NJS_POINT3 collision_218_points[36];
static NJS_MODEL collision_218_model;
static NJS_OBJECT collision_218;
static NJS_MATERIAL collision_219_materials[1];
static Sint16 collision_219_faces[63];
static NJS_MESHSET collision_219_meshsets[1];
static NJS_POINT3 collision_219_points[36];
static NJS_MODEL collision_219_model;
static NJS_OBJECT collision_219;
static NJS_MATERIAL collision_220_materials[1];
static Sint16 collision_220_faces[92];
static NJS_MESHSET collision_220_meshsets[1];
static NJS_POINT3 collision_220_points[55];
static NJS_MODEL collision_220_model;
static NJS_OBJECT collision_220;
static NJS_MATERIAL collision_221_materials[1];
static Sint16 collision_221_faces[188];
static NJS_MESHSET collision_221_meshsets[1];
static NJS_POINT3 collision_221_points[81];
static NJS_MODEL collision_221_model;
static NJS_OBJECT collision_221;
static NJS_MATERIAL collision_222_materials[1];
static Sint16 collision_222_faces[82];
static NJS_MESHSET collision_222_meshsets[1];
static NJS_POINT3 collision_222_points[41];
static NJS_MODEL collision_222_model;
static NJS_OBJECT collision_222;
static NJS_MATERIAL collision_223_materials[1];
static Sint16 collision_223_faces[145];
static NJS_MESHSET collision_223_meshsets[1];
static NJS_POINT3 collision_223_points[68];
static NJS_MODEL collision_223_model;
static NJS_OBJECT collision_223;
static NJS_MATERIAL collision_224_materials[1];
static Sint16 collision_224_faces[43];
static NJS_MESHSET collision_224_meshsets[1];
static NJS_POINT3 collision_224_points[22];
static NJS_MODEL collision_224_model;
static NJS_OBJECT collision_224;
static NJS_MATERIAL collision_225_materials[1];
static Sint16 collision_225_faces[67];
static NJS_MESHSET collision_225_meshsets[1];
static NJS_POINT3 collision_225_points[33];
static NJS_MODEL collision_225_model;
static NJS_OBJECT collision_225;
static NJS_MATERIAL collision_226_materials[1];
static Sint16 collision_226_faces[64];
static NJS_MESHSET collision_226_meshsets[1];
static NJS_POINT3 collision_226_points[33];
static NJS_MODEL collision_226_model;
static NJS_OBJECT collision_226;
static NJS_MATERIAL collision_227_materials[1];
static Sint16 collision_227_faces[63];
static NJS_MESHSET collision_227_meshsets[1];
static NJS_POINT3 collision_227_points[36];
static NJS_MODEL collision_227_model;
static NJS_OBJECT collision_227;
static NJS_MATERIAL collision_228_materials[1];
static Sint16 collision_228_faces[20];
static NJS_MESHSET collision_228_meshsets[1];
static NJS_POINT3 collision_228_points[18];
static NJS_MODEL collision_228_model;
static NJS_OBJECT collision_228;
static NJS_MATERIAL collision_229_materials[1];
static Sint16 collision_229_faces[14];
static NJS_MESHSET collision_229_meshsets[1];
static NJS_POINT3 collision_229_points[12];
static NJS_MODEL collision_229_model;
static NJS_OBJECT collision_229;
static NJS_MATERIAL collision_230_materials[1];
static Sint16 collision_230_faces[14];
static NJS_MESHSET collision_230_meshsets[1];
static NJS_POINT3 collision_230_points[12];
static NJS_MODEL collision_230_model;
static NJS_OBJECT collision_230;
static NJS_MATERIAL collision_231_materials[1];
static Sint16 collision_231_faces[14];
static NJS_MESHSET collision_231_meshsets[1];
static NJS_POINT3 collision_231_points[12];
static NJS_MODEL collision_231_model;
static NJS_OBJECT collision_231;
static NJS_MATERIAL collision_232_materials[1];
static Sint16 collision_232_faces[14];
static NJS_MESHSET collision_232_meshsets[1];
static NJS_POINT3 collision_232_points[12];
static NJS_MODEL collision_232_model;
static NJS_OBJECT collision_232;
static NJS_MATERIAL collision_233_materials[1];
static Sint16 collision_233_faces[14];
static NJS_MESHSET collision_233_meshsets[1];
static NJS_POINT3 collision_233_points[12];
static NJS_MODEL collision_233_model;
static NJS_OBJECT collision_233;
static NJS_MATERIAL collision_234_materials[1];
static Sint16 collision_234_faces[14];
static NJS_MESHSET collision_234_meshsets[1];
static NJS_POINT3 collision_234_points[12];
static NJS_MODEL collision_234_model;
static NJS_OBJECT collision_234;
static NJS_MATERIAL collision_235_materials[1];
static Sint16 collision_235_faces[14];
static NJS_MESHSET collision_235_meshsets[1];
static NJS_POINT3 collision_235_points[12];
static NJS_MODEL collision_235_model;
static NJS_OBJECT collision_235;
static NJS_MATERIAL collision_236_materials[1];
static Sint16 collision_236_faces[14];
static NJS_MESHSET collision_236_meshsets[1];
static NJS_POINT3 collision_236_points[12];
static NJS_MODEL collision_236_model;
static NJS_OBJECT collision_236;
static NJS_MATERIAL collision_237_materials[1];
static Sint16 collision_237_faces[14];
static NJS_MESHSET collision_237_meshsets[1];
static NJS_POINT3 collision_237_points[12];
static NJS_MODEL collision_237_model;
static NJS_OBJECT collision_237;
static NJS_MATERIAL collision_238_materials[1];
static Sint16 collision_238_faces[14];
static NJS_MESHSET collision_238_meshsets[1];
static NJS_POINT3 collision_238_points[12];
static NJS_MODEL collision_238_model;
static NJS_OBJECT collision_238;
static NJS_MATERIAL collision_239_materials[1];
static Sint16 collision_239_faces[14];
static NJS_MESHSET collision_239_meshsets[1];
static NJS_POINT3 collision_239_points[12];
static NJS_MODEL collision_239_model;
static NJS_OBJECT collision_239;
static NJS_MATERIAL collision_240_materials[1];
static Sint16 collision_240_faces[14];
static NJS_MESHSET collision_240_meshsets[1];
static NJS_POINT3 collision_240_points[12];
static NJS_MODEL collision_240_model;
static NJS_OBJECT collision_240;
static NJS_MATERIAL collision_241_materials[1];
static Sint16 collision_241_faces[14];
static NJS_MESHSET collision_241_meshsets[1];
static NJS_POINT3 collision_241_points[12];
static NJS_MODEL collision_241_model;
static NJS_OBJECT collision_241;
static NJS_MATERIAL collision_242_materials[1];
static Sint16 collision_242_faces[14];
static NJS_MESHSET collision_242_meshsets[1];
static NJS_POINT3 collision_242_points[12];
static NJS_MODEL collision_242_model;
static NJS_OBJECT collision_242;
static NJS_MATERIAL collision_243_materials[1];
static Sint16 collision_243_faces[14];
static NJS_MESHSET collision_243_meshsets[1];
static NJS_POINT3 collision_243_points[12];
static NJS_MODEL collision_243_model;
static NJS_OBJECT collision_243;
static NJS_MATERIAL collision_244_materials[1];
static Sint16 collision_244_faces[14];
static NJS_MESHSET collision_244_meshsets[1];
static NJS_POINT3 collision_244_points[12];
static NJS_MODEL collision_244_model;
static NJS_OBJECT collision_244;
static NJS_MATERIAL collision_245_materials[1];
static Sint16 collision_245_faces[14];
static NJS_MESHSET collision_245_meshsets[1];
static NJS_POINT3 collision_245_points[12];
static NJS_MODEL collision_245_model;
static NJS_OBJECT collision_245;
static NJS_MATERIAL collision_246_materials[1];
static Sint16 collision_246_faces[14];
static NJS_MESHSET collision_246_meshsets[1];
static NJS_POINT3 collision_246_points[12];
static NJS_MODEL collision_246_model;
static NJS_OBJECT collision_246;
static NJS_MATERIAL collision_247_materials[1];
static Sint16 collision_247_faces[14];
static NJS_MESHSET collision_247_meshsets[1];
static NJS_POINT3 collision_247_points[12];
static NJS_MODEL collision_247_model;
static NJS_OBJECT collision_247;
static NJS_MATERIAL collision_248_materials[1];
static Sint16 collision_248_faces[14];
static NJS_MESHSET collision_248_meshsets[1];
static NJS_POINT3 collision_248_points[12];
static NJS_MODEL collision_248_model;
static NJS_OBJECT collision_248;
static NJS_MATERIAL collision_249_materials[1];
static Sint16 collision_249_faces[14];
static NJS_MESHSET collision_249_meshsets[1];
static NJS_POINT3 collision_249_points[12];
static NJS_MODEL collision_249_model;
static NJS_OBJECT collision_249;
static NJS_MATERIAL collision_250_materials[1];
static Sint16 collision_250_faces[14];
static NJS_MESHSET collision_250_meshsets[1];
static NJS_POINT3 collision_250_points[12];
static NJS_MODEL collision_250_model;
static NJS_OBJECT collision_250;
static NJS_MATERIAL collision_251_materials[1];
static Sint16 collision_251_faces[14];
static NJS_MESHSET collision_251_meshsets[1];
static NJS_POINT3 collision_251_points[12];
static NJS_MODEL collision_251_model;
static NJS_OBJECT collision_251;
static NJS_MATERIAL collision_252_materials[1];
static Sint16 collision_252_faces[14];
static NJS_MESHSET collision_252_meshsets[1];
static NJS_POINT3 collision_252_points[12];
static NJS_MODEL collision_252_model;
static NJS_OBJECT collision_252;
static NJS_MATERIAL collision_253_materials[1];
static Sint16 collision_253_faces[14];
static NJS_MESHSET collision_253_meshsets[1];
static NJS_POINT3 collision_253_points[12];
static NJS_MODEL collision_253_model;
static NJS_OBJECT collision_253;
static NJS_MATERIAL collision_254_materials[1];
static Sint16 collision_254_faces[14];
static NJS_MESHSET collision_254_meshsets[1];
static NJS_POINT3 collision_254_points[12];
static NJS_MODEL collision_254_model;
static NJS_OBJECT collision_254;
static NJS_MATERIAL collision_255_materials[1];
static Sint16 collision_255_faces[14];
static NJS_MESHSET collision_255_meshsets[1];
static NJS_POINT3 collision_255_points[12];
static NJS_MODEL collision_255_model;
static NJS_OBJECT collision_255;
static NJS_MATERIAL collision_256_materials[1];
static Sint16 collision_256_faces[14];
static NJS_MESHSET collision_256_meshsets[1];
static NJS_POINT3 collision_256_points[12];
static NJS_MODEL collision_256_model;
static NJS_OBJECT collision_256;
static NJS_MATERIAL collision_257_materials[1];
static Sint16 collision_257_faces[14];
static NJS_MESHSET collision_257_meshsets[1];
static NJS_POINT3 collision_257_points[12];
static NJS_MODEL collision_257_model;
static NJS_OBJECT collision_257;
static NJS_MATERIAL collision_258_materials[1];
static Sint16 collision_258_faces[14];
static NJS_MESHSET collision_258_meshsets[1];
static NJS_POINT3 collision_258_points[12];
static NJS_MODEL collision_258_model;
static NJS_OBJECT collision_258;
static NJS_MATERIAL collision_259_materials[1];
static Sint16 collision_259_faces[14];
static NJS_MESHSET collision_259_meshsets[1];
static NJS_POINT3 collision_259_points[12];
static NJS_MODEL collision_259_model;
static NJS_OBJECT collision_259;
static NJS_MATERIAL collision_260_materials[1];
static Sint16 collision_260_faces[14];
static NJS_MESHSET collision_260_meshsets[1];
static NJS_POINT3 collision_260_points[12];
static NJS_MODEL collision_260_model;
static NJS_OBJECT collision_260;
static NJS_MATERIAL collision_261_materials[1];
static Sint16 collision_261_faces[14];
static NJS_MESHSET collision_261_meshsets[1];
static NJS_POINT3 collision_261_points[12];
static NJS_MODEL collision_261_model;
static NJS_OBJECT collision_261;
static NJS_MATERIAL collision_262_materials[1];
static Sint16 collision_262_faces[14];
static NJS_MESHSET collision_262_meshsets[1];
static NJS_POINT3 collision_262_points[12];
static NJS_MODEL collision_262_model;
static NJS_OBJECT collision_262;
static NJS_MATERIAL collision_263_materials[1];
static Sint16 collision_263_faces[14];
static NJS_MESHSET collision_263_meshsets[1];
static NJS_POINT3 collision_263_points[12];
static NJS_MODEL collision_263_model;
static NJS_OBJECT collision_263;
static NJS_MATERIAL collision_264_materials[1];
static Sint16 collision_264_faces[14];
static NJS_MESHSET collision_264_meshsets[1];
static NJS_POINT3 collision_264_points[12];
static NJS_MODEL collision_264_model;
static NJS_OBJECT collision_264;
static NJS_MATERIAL collision_265_materials[1];
static Sint16 collision_265_faces[14];
static NJS_MESHSET collision_265_meshsets[1];
static NJS_POINT3 collision_265_points[12];
static NJS_MODEL collision_265_model;
static NJS_OBJECT collision_265;
static NJS_MATERIAL collision_266_materials[1];
static Sint16 collision_266_faces[60];
static NJS_MESHSET collision_266_meshsets[1];
static NJS_POINT3 collision_266_points[35];
static NJS_MODEL collision_266_model;
static NJS_OBJECT collision_266;
static NJS_MATERIAL collision_267_materials[1];
static Sint16 collision_267_faces[72];
static NJS_MESHSET collision_267_meshsets[1];
static NJS_POINT3 collision_267_points[44];
static NJS_MODEL collision_267_model;
static NJS_OBJECT collision_267;
static NJS_MATERIAL collision_268_materials[1];
static Sint16 collision_268_faces[87];
static NJS_MESHSET collision_268_meshsets[1];
static NJS_POINT3 collision_268_points[50];
static NJS_MODEL collision_268_model;
static NJS_OBJECT collision_268;
static NJS_MATERIAL collision_269_materials[1];
static Sint16 collision_269_faces[216];
static NJS_MESHSET collision_269_meshsets[1];
static NJS_POINT3 collision_269_points[90];
static NJS_MODEL collision_269_model;
static NJS_OBJECT collision_269;
static NJS_MATERIAL collision_270_materials[1];
static Sint16 collision_270_faces[106];
static NJS_MESHSET collision_270_meshsets[1];
static NJS_POINT3 collision_270_points[59];
static NJS_MODEL collision_270_model;
static NJS_OBJECT collision_270;
static NJS_MATERIAL collision_271_materials[1];
static Sint16 collision_271_faces[90];
static NJS_MESHSET collision_271_meshsets[1];
static NJS_POINT3 collision_271_points[49];
static NJS_MODEL collision_271_model;
static NJS_OBJECT collision_271;
static NJS_MATERIAL collision_272_materials[1];
static Sint16 collision_272_faces[143];
static NJS_MESHSET collision_272_meshsets[1];
static NJS_POINT3 collision_272_points[66];
static NJS_MODEL collision_272_model;
static NJS_OBJECT collision_272;
static NJS_MATERIAL collision_273_materials[1];
static Sint16 collision_273_faces[63];
static NJS_MESHSET collision_273_meshsets[1];
static NJS_POINT3 collision_273_points[36];
static NJS_MODEL collision_273_model;
static NJS_OBJECT collision_273;
static NJS_MATERIAL collision_274_materials[1];
static Sint16 collision_274_faces[36];
static NJS_MESHSET collision_274_meshsets[1];
static NJS_POINT3 collision_274_points[20];
static NJS_MODEL collision_274_model;
static NJS_OBJECT collision_274;
static NJS_MATERIAL collision_275_materials[1];
static Sint16 collision_275_faces[60];
static NJS_MESHSET collision_275_meshsets[1];
static NJS_POINT3 collision_275_points[35];
static NJS_MODEL collision_275_model;
static NJS_OBJECT collision_275;
static NJS_MATERIAL collision_276_materials[1];
static Sint16 collision_276_faces[67];
static NJS_MESHSET collision_276_meshsets[1];
static NJS_POINT3 collision_276_points[33];
static NJS_MODEL collision_276_model;
static NJS_OBJECT collision_276;
static NJS_MATERIAL collision_277_materials[1];
static Sint16 collision_277_faces[122];
static NJS_MESHSET collision_277_meshsets[1];
static NJS_POINT3 collision_277_points[56];
static NJS_MODEL collision_277_model;
static NJS_OBJECT collision_277;
static NJS_MATERIAL collision_278_materials[1];
static Sint16 collision_278_faces[73];
static NJS_MESHSET collision_278_meshsets[1];
static NJS_POINT3 collision_278_points[41];
static NJS_MODEL collision_278_model;
static NJS_OBJECT collision_278;
static NJS_MATERIAL collision_279_materials[1];
static Sint16 collision_279_faces[72];
static NJS_MESHSET collision_279_meshsets[1];
static NJS_POINT3 collision_279_points[33];
static NJS_MODEL collision_279_model;
static NJS_OBJECT collision_279;
static NJS_MATERIAL collision_306_materials[1];
static Sint16 collision_306_faces[63];
static NJS_MESHSET collision_306_meshsets[1];
static NJS_POINT3 collision_306_points[42];
static NJS_MODEL collision_306_model;
static NJS_OBJECT collision_306;
static NJS_MATERIAL collision_307_materials[1];
static Sint16 collision_307_faces[74];
static NJS_MESHSET collision_307_meshsets[1];
static NJS_POINT3 collision_307_points[54];
static NJS_MODEL collision_307_model;
static NJS_OBJECT collision_307;
static NJS_MATERIAL collision_308_materials[1];
static Sint16 collision_308_faces[104];
static NJS_MESHSET collision_308_meshsets[1];
static NJS_POINT3 collision_308_points[60];
static NJS_MODEL collision_308_model;
static NJS_OBJECT collision_308;
static NJS_MATERIAL collision_309_materials[1];
static Sint16 collision_309_faces[97];
static NJS_MESHSET collision_309_meshsets[1];
static NJS_POINT3 collision_309_points[42];
static NJS_MODEL collision_309_model;
static NJS_OBJECT collision_309;
static NJS_MATERIAL collision_310_materials[1];
static Sint16 collision_310_faces[97];
static NJS_MESHSET collision_310_meshsets[1];
static NJS_POINT3 collision_310_points[42];
static NJS_MODEL collision_310_model;
static NJS_OBJECT collision_310;
static NJS_MATERIAL collision_311_materials[1];
static Sint16 collision_311_faces[97];
static NJS_MESHSET collision_311_meshsets[1];
static NJS_POINT3 collision_311_points[42];
static NJS_MODEL collision_311_model;
static NJS_OBJECT collision_311;
static NJS_MATERIAL collision_312_materials[1];
static Sint16 collision_312_faces[97];
static NJS_MESHSET collision_312_meshsets[1];
static NJS_POINT3 collision_312_points[42];
static NJS_MODEL collision_312_model;
static NJS_OBJECT collision_312;
static NJS_MATERIAL collision_313_materials[1];
static Sint16 collision_313_faces[97];
static NJS_MESHSET collision_313_meshsets[1];
static NJS_POINT3 collision_313_points[42];
static NJS_MODEL collision_313_model;
static NJS_OBJECT collision_313;
static NJS_MATERIAL collision_314_materials[1];
static Sint16 collision_314_faces[97];
static NJS_MESHSET collision_314_meshsets[1];
static NJS_POINT3 collision_314_points[42];
static NJS_MODEL collision_314_model;
static NJS_OBJECT collision_314;
static NJS_MATERIAL collision_315_materials[1];
static Sint16 collision_315_faces[97];
static NJS_MESHSET collision_315_meshsets[1];
static NJS_POINT3 collision_315_points[42];
static NJS_MODEL collision_315_model;
static NJS_OBJECT collision_315;
static NJS_MATERIAL collision_316_materials[1];
static Sint16 collision_316_faces[97];
static NJS_MESHSET collision_316_meshsets[1];
static NJS_POINT3 collision_316_points[42];
static NJS_MODEL collision_316_model;
static NJS_OBJECT collision_316;
static NJS_MATERIAL collision_317_materials[1];
static Sint16 collision_317_faces[97];
static NJS_MESHSET collision_317_meshsets[1];
static NJS_POINT3 collision_317_points[42];
static NJS_MODEL collision_317_model;
static NJS_OBJECT collision_317;
static NJS_MATERIAL collision_318_materials[1];
static Sint16 collision_318_faces[97];
static NJS_MESHSET collision_318_meshsets[1];
static NJS_POINT3 collision_318_points[42];
static NJS_MODEL collision_318_model;
static NJS_OBJECT collision_318;
static NJS_MATERIAL collision_319_materials[1];
static Sint16 collision_319_faces[83];
static NJS_MESHSET collision_319_meshsets[1];
static NJS_POINT3 collision_319_points[40];
static NJS_MODEL collision_319_model;
static NJS_OBJECT collision_319;
static NJS_MATERIAL collision_324_materials[1];
static Sint16 collision_324_faces[29];
static NJS_MESHSET collision_324_meshsets[1];
static NJS_POINT3 collision_324_points[18];
static NJS_MODEL collision_324_model;
static NJS_OBJECT collision_324;
static NJS_MATERIAL collision_325_materials[1];
static Sint16 collision_325_faces[63];
static NJS_MESHSET collision_325_meshsets[1];
static NJS_POINT3 collision_325_points[35];
static NJS_MODEL collision_325_model;
static NJS_OBJECT collision_325;
static NJS_MATERIAL collision_327_materials[1];
static Sint16 collision_327_faces[34];
static NJS_MESHSET collision_327_meshsets[1];
static NJS_POINT3 collision_327_points[16];
static NJS_MODEL collision_327_model;
static NJS_OBJECT collision_327;
static NJS_MATERIAL collision_335_materials[1];
static Sint16 collision_335_faces[36];
static NJS_MESHSET collision_335_meshsets[1];
static NJS_POINT3 collision_335_points[24];
static NJS_MODEL collision_335_model;
static NJS_OBJECT collision_335;
static NJS_MATERIAL collision_336_materials[1];
static Sint16 collision_336_faces[75];
static NJS_MESHSET collision_336_meshsets[1];
static NJS_POINT3 collision_336_points[42];
static NJS_MODEL collision_336_model;
static NJS_OBJECT collision_336;
static NJS_MATERIAL collision_340_materials[1];
static Sint16 collision_340_faces[75];
static NJS_MESHSET collision_340_meshsets[1];
static NJS_POINT3 collision_340_points[42];
static NJS_MODEL collision_340_model;
static NJS_OBJECT collision_340;
static NJS_MATERIAL collision_344_materials[1];
static Sint16 collision_344_faces[75];
static NJS_MESHSET collision_344_meshsets[1];
static NJS_POINT3 collision_344_points[42];
static NJS_MODEL collision_344_model;
static NJS_OBJECT collision_344;
static NJS_MATERIAL collision_348_materials[1];
static Sint16 collision_348_faces[60];
static NJS_MESHSET collision_348_meshsets[1];
static NJS_POINT3 collision_348_points[35];
static NJS_MODEL collision_348_model;
static NJS_OBJECT collision_348;
static NJS_MATERIAL collision_352_materials[1];
static Sint16 collision_352_faces[87];
static NJS_MESHSET collision_352_meshsets[1];
static NJS_POINT3 collision_352_points[42];
static NJS_MODEL collision_352_model;
static NJS_OBJECT collision_352;
static NJS_MATERIAL collision_355_materials[1];
static Sint16 collision_355_faces[59];
static NJS_MESHSET collision_355_meshsets[1];
static NJS_POINT3 collision_355_points[24];
static NJS_MODEL collision_355_model;
static NJS_OBJECT collision_355;
static NJS_MATERIAL collision_418_materials[1];
static Sint16 collision_418_faces[14];
static NJS_MESHSET collision_418_meshsets[1];
static NJS_POINT3 collision_418_points[12];
static NJS_MODEL collision_418_model;
static NJS_OBJECT collision_418;
static NJS_MATERIAL collision_420_materials[1];
static Sint16 collision_420_faces[40];
static NJS_MESHSET collision_420_meshsets[1];
static NJS_POINT3 collision_420_points[24];
static NJS_MODEL collision_420_model;
static NJS_OBJECT collision_420;
static NJS_MATERIAL collision_423_materials[1];
static Sint16 collision_423_faces[29];
static NJS_MESHSET collision_423_meshsets[1];
static NJS_POINT3 collision_423_points[16];
static NJS_MODEL collision_423_model;
static NJS_OBJECT collision_423;
static NJS_MATERIAL collision_425_materials[1];
static Sint16 collision_425_faces[142];
static NJS_MESHSET collision_425_meshsets[1];
static NJS_POINT3 collision_425_points[52];
static NJS_MODEL collision_425_model;
static NJS_OBJECT collision_425;
static NJS_MATERIAL collision_428_materials[1];
static Sint16 collision_428_faces[30];
static NJS_MESHSET collision_428_meshsets[1];
static NJS_POINT3 collision_428_points[16];
static NJS_MODEL collision_428_model;
static NJS_OBJECT collision_428;
static NJS_MATERIAL collision_430_materials[1];
static Sint16 collision_430_faces[9];
static NJS_MESHSET collision_430_meshsets[1];
static NJS_POINT3 collision_430_points[8];
static NJS_MODEL collision_430_model;
static NJS_OBJECT collision_430;
static NJS_MATERIAL collision_432_materials[1];
static Sint16 collision_432_faces[46];
static NJS_MESHSET collision_432_meshsets[1];
static NJS_POINT3 collision_432_points[26];
static NJS_MODEL collision_432_model;
static NJS_OBJECT collision_432;
static NJS_MATERIAL collision_434_materials[1];
static Sint16 collision_434_faces[7];
static NJS_MESHSET collision_434_meshsets[1];
static NJS_POINT3 collision_434_points[6];
static NJS_MODEL collision_434_model;
static NJS_OBJECT collision_434;
static NJS_MATERIAL collision_471_materials[1];
static Sint16 collision_471_faces[25];
static NJS_MESHSET collision_471_meshsets[1];
static NJS_POINT3 collision_471_points[10];
static NJS_MODEL collision_471_model;
static NJS_OBJECT collision_471;
static NJS_MATERIAL collision_626_materials[1];
static Sint16 collision_626_faces[7];
static NJS_MESHSET collision_626_meshsets[1];
static NJS_POINT3 collision_626_points[6];
static NJS_MODEL collision_626_model;
static NJS_OBJECT collision_626;
static NJS_MATERIAL collision_653_materials[1];
static Sint16 collision_653_faces[14];
static NJS_MESHSET collision_653_meshsets[1];
static NJS_POINT3 collision_653_points[12];
static NJS_MODEL collision_653_model;
static NJS_OBJECT collision_653;
static NJS_MATERIAL collision_661_materials[1];
static Sint16 collision_661_faces[40];
static NJS_MESHSET collision_661_meshsets[1];
static NJS_POINT3 collision_661_points[24];
static NJS_MODEL collision_661_model;
static NJS_OBJECT collision_661;
static NJS_MATERIAL collision_669_materials[1];
static Sint16 collision_669_faces[85];
static NJS_MESHSET collision_669_meshsets[1];
static NJS_POINT3 collision_669_points[43];
static NJS_MODEL collision_669_model;
static NJS_OBJECT collision_669;
static Sint16 draw_8_polygons[185];
static Sint32 draw_8_vertices[159];
static NJS_CNK_MODEL draw_8_model;
static NJS_CNK_OBJECT draw_856;
static Sint16 draw_100_polygons[355];
static Sint32 draw_100_vertices[243];
static NJS_CNK_MODEL draw_100_model;
static NJS_CNK_OBJECT draw_855;
static Sint16 draw_17_polygons[555];
static Sint32 draw_17_vertices[379];
static NJS_CNK_MODEL draw_17_model;
static NJS_CNK_OBJECT draw_854;
static Sint16 draw_11_polygons[429];
static Sint32 draw_11_vertices[319];
static NJS_CNK_MODEL draw_11_model;
static NJS_CNK_OBJECT draw_849;
static Sint16 draw_6_polygons[391];
static Sint32 draw_6_vertices[279];
static NJS_CNK_MODEL draw_6_model;
static NJS_CNK_OBJECT draw_836;
static Sint16 draw_1_polygons[169];
static Sint32 draw_1_vertices[155];
static NJS_CNK_MODEL draw_1_model;
static NJS_CNK_OBJECT draw_834;
static Sint16 draw_0_polygons[381];
static Sint32 draw_0_vertices[339];
static NJS_CNK_MODEL draw_0_model;
static NJS_CNK_OBJECT draw_810;
static Sint16 draw_761_polygons[169];
static Sint32 draw_761_vertices[155];
static NJS_CNK_MODEL draw_761_model;
static NJS_CNK_OBJECT draw_808;
static Sint16 draw_760_polygons[391];
static Sint32 draw_760_vertices[279];
static NJS_CNK_MODEL draw_760_model;
static NJS_CNK_OBJECT draw_760;
static Sint16 draw_12_polygons[271];
static Sint32 draw_12_vertices[303];
static NJS_CNK_MODEL draw_12_model;
static NJS_CNK_OBJECT draw_732;
static Sint16 draw_447_polygons[551];
static Sint32 draw_447_vertices[415];
static NJS_CNK_MODEL draw_447_model;
static NJS_CNK_OBJECT draw_731;
static Sint16 draw_19_polygons[271];
static Sint32 draw_19_vertices[279];
static NJS_CNK_MODEL draw_19_model;
static NJS_CNK_OBJECT draw_730;
static Sint16 draw_728_polygons[193];
static Sint32 draw_728_vertices[135];
static NJS_CNK_MODEL draw_728_model;
static NJS_CNK_OBJECT draw_728;
static Sint16 draw_723_polygons[93];
static Sint32 draw_723_vertices[75];
static NJS_CNK_MODEL draw_723_model;
static NJS_CNK_OBJECT draw_725;
static Sint16 draw_717_polygons[93];
static Sint32 draw_717_vertices[75];
static NJS_CNK_MODEL draw_717_model;
static NJS_CNK_OBJECT draw_722;
static Sint16 draw_721_polygons[187];
static Sint32 draw_721_vertices[143];
static NJS_CNK_MODEL draw_721_model;
static NJS_CNK_OBJECT draw_721;
static Sint16 draw_720_polygons[229];
static Sint32 draw_720_vertices[227];
static NJS_CNK_MODEL draw_720_model;
static NJS_CNK_OBJECT draw_720;
static Sint16 draw_719_polygons[103];
static Sint32 draw_719_vertices[99];
static NJS_CNK_MODEL draw_719_model;
static NJS_CNK_OBJECT draw_719;
static Sint16 draw_716_polygons[69];
static Sint32 draw_716_vertices[51];
static NJS_CNK_MODEL draw_716_model;
static NJS_CNK_OBJECT draw_716;
static Sint16 draw_715_polygons[211];
static Sint32 draw_715_vertices[135];
static NJS_CNK_MODEL draw_715_model;
static NJS_CNK_OBJECT draw_715;
static Sint16 draw_714_polygons[69];
static Sint32 draw_714_vertices[51];
static NJS_CNK_MODEL draw_714_model;
static NJS_CNK_OBJECT draw_714;
static Sint16 draw_712_polygons[271];
static Sint32 draw_712_vertices[263];
static NJS_CNK_MODEL draw_712_model;
static NJS_CNK_OBJECT draw_713;
static Sint16 draw_711_polygons[271];
static Sint32 draw_711_vertices[311];
static NJS_CNK_MODEL draw_711_model;
static NJS_CNK_OBJECT draw_711;
static Sint16 draw_710_polygons[271];
static Sint32 draw_710_vertices[303];
static NJS_CNK_MODEL draw_710_model;
static NJS_CNK_OBJECT draw_710;
static Sint16 draw_18_polygons[155];
static Sint32 draw_18_vertices[115];
static NJS_CNK_MODEL draw_18_model;
static NJS_CNK_OBJECT draw_591;
static Sint16 draw_16_polygons[141];
static Sint32 draw_16_vertices[99];
static NJS_CNK_MODEL draw_16_model;
static NJS_CNK_OBJECT draw_588;
static Sint16 draw_65_polygons[515];
static Sint32 draw_65_vertices[427];
static NJS_CNK_MODEL draw_65_model;
static NJS_CNK_OBJECT draw_587;
static Sint16 draw_62_polygons[735];
static Sint32 draw_62_vertices[479];
static NJS_CNK_MODEL draw_62_model;
static NJS_CNK_OBJECT draw_586;
static Sint16 draw_58_polygons[55];
static Sint32 draw_58_vertices[51];
static NJS_CNK_MODEL draw_58_model;
static NJS_CNK_OBJECT draw_585;
static Sint16 draw_15_polygons[183];
static Sint32 draw_15_vertices[123];
static NJS_CNK_MODEL draw_15_model;
static NJS_CNK_OBJECT draw_584;
static Sint16 draw_581_polygons[173];
static Sint32 draw_581_vertices[83];
static NJS_CNK_MODEL draw_581_model;
static NJS_CNK_OBJECT draw_581;
static Sint16 draw_14_polygons[133];
static Sint32 draw_14_vertices[83];
static NJS_CNK_MODEL draw_14_model;
static NJS_CNK_OBJECT draw_578;
static Sint16 draw_10_polygons[157];
static Sint32 draw_10_vertices[107];
static NJS_CNK_MODEL draw_10_model;
static NJS_CNK_OBJECT draw_577;
static Sint16 draw_473_polygons[277];
static Sint32 draw_473_vertices[247];
static NJS_CNK_MODEL draw_473_model;
static NJS_CNK_OBJECT draw_575;
static Sint16 draw_470_polygons[41];
static Sint32 draw_470_vertices[35];
static NJS_CNK_MODEL draw_470_model;
static NJS_CNK_OBJECT draw_574;
static Sint16 draw_481_polygons[59];
static Sint32 draw_481_vertices[51];
static NJS_CNK_MODEL draw_481_model;
static NJS_CNK_OBJECT draw_570;
static Sint16 draw_569_polygons[145];
static Sint32 draw_569_vertices[99];
static NJS_CNK_MODEL draw_569_model;
static NJS_CNK_OBJECT draw_569;
static Sint16 draw_556_polygons[45];
static Sint32 draw_556_vertices[31];
static NJS_CNK_MODEL draw_556_model;
static NJS_CNK_OBJECT draw_556;
static Sint16 draw_456_polygons[91];
static Sint32 draw_456_vertices[83];
static NJS_CNK_MODEL draw_456_model;
static NJS_CNK_OBJECT draw_456;
static Sint16 draw_281_polygons[59];
static Sint32 draw_281_vertices[43];
static NJS_CNK_MODEL draw_281_model;
static NJS_CNK_OBJECT draw_426;
static Sint16 draw_276_polygons[307];
static Sint32 draw_276_vertices[199];
static NJS_CNK_MODEL draw_276_model;
static NJS_CNK_OBJECT draw_425;
static Sint16 draw_251_polygons[301];
static Sint32 draw_251_vertices[195];
static NJS_CNK_MODEL draw_251_model;
static NJS_CNK_OBJECT draw_424;
static Sint16 draw_342_polygons[451];
static Sint32 draw_342_vertices[327];
static NJS_CNK_MODEL draw_342_model;
static NJS_CNK_OBJECT draw_423;
static Sint16 draw_293_polygons[519];
static Sint32 draw_293_vertices[419];
static NJS_CNK_MODEL draw_293_model;
static NJS_CNK_OBJECT draw_421;
static Sint16 draw_287_polygons[59];
static Sint32 draw_287_vertices[43];
static NJS_CNK_MODEL draw_287_model;
static NJS_CNK_OBJECT draw_397;
static Sint16 draw_269_polygons[519];
static Sint32 draw_269_vertices[431];
static NJS_CNK_MODEL draw_269_model;
static NJS_CNK_OBJECT draw_394;
static Sint16 draw_272_polygons[55];
static Sint32 draw_272_vertices[35];
static NJS_CNK_MODEL draw_272_model;
static NJS_CNK_OBJECT draw_362;
static Sint16 draw_361_polygons[77];
static Sint32 draw_361_vertices[63];
static NJS_CNK_MODEL draw_361_model;
static NJS_CNK_OBJECT draw_361;
static Sint16 draw_239_polygons[511];
static Sint32 draw_239_vertices[423];
static NJS_CNK_MODEL draw_239_model;
static NJS_CNK_OBJECT draw_360;
static Sint16 draw_264_polygons[21];
static Sint32 draw_264_vertices[51];
static NJS_CNK_MODEL draw_264_model;
static NJS_CNK_OBJECT draw_264;
static Sint16 draw_240_polygons[445];
static Sint32 draw_240_vertices[391];
static NJS_CNK_MODEL draw_240_model;
static NJS_CNK_OBJECT draw_240;
static Sint16 draw_23_polygons[579];
static Sint32 draw_23_vertices[427];
static NJS_CNK_MODEL draw_23_model;
static NJS_CNK_OBJECT draw_66;
static NJS_OBJECT collision_2;
static NJS_OBJECT collision_3;
static NJS_OBJECT collision_4;
static NJS_OBJECT collision_8;
static NJS_OBJECT collision_9;
static NJS_OBJECT collision_12;
static NJS_OBJECT collision_13;
static NJS_OBJECT collision_15;
static NJS_OBJECT collision_16;
static NJS_OBJECT collision_17;
static NJS_OBJECT collision_18;
static NJS_OBJECT collision_22;
static NJS_OBJECT collision_23;
static NJS_OBJECT collision_24;
static NJS_OBJECT collision_26;
static NJS_OBJECT collision_27;
static NJS_OBJECT collision_28;
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
static NJS_OBJECT collision_96;
static NJS_OBJECT collision_97;
static NJS_OBJECT collision_99;
static NJS_OBJECT collision_100;
static NJS_OBJECT collision_101;
static NJS_OBJECT collision_102;
static NJS_OBJECT collision_103;
static NJS_OBJECT collision_107;
static NJS_OBJECT collision_108;
static NJS_OBJECT collision_109;
static NJS_OBJECT collision_114;
static NJS_OBJECT collision_116;
static NJS_OBJECT collision_117;
static NJS_OBJECT collision_118;
static NJS_OBJECT collision_123;
static NJS_OBJECT collision_124;
static NJS_OBJECT collision_126;
static NJS_OBJECT collision_128;
static NJS_OBJECT collision_130;
static NJS_OBJECT collision_131;
static NJS_OBJECT collision_132;
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
static NJS_OBJECT collision_145;
static NJS_OBJECT collision_147;
static NJS_OBJECT collision_149;
static NJS_OBJECT collision_161;
static NJS_OBJECT collision_162;
static NJS_OBJECT collision_164;
static NJS_OBJECT collision_165;
static NJS_OBJECT collision_170;
static NJS_OBJECT collision_171;
static NJS_OBJECT collision_173;
static NJS_OBJECT collision_280;
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
static NJS_OBJECT collision_291;
static NJS_OBJECT collision_292;
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
static NJS_OBJECT collision_320;
static NJS_OBJECT collision_321;
static NJS_OBJECT collision_322;
static NJS_OBJECT collision_323;
static NJS_OBJECT collision_326;
static NJS_OBJECT collision_328;
static NJS_OBJECT collision_329;
static NJS_OBJECT collision_330;
static NJS_OBJECT collision_331;
static NJS_OBJECT collision_332;
static NJS_OBJECT collision_333;
static NJS_OBJECT collision_334;
static NJS_OBJECT collision_337;
static NJS_OBJECT collision_338;
static NJS_OBJECT collision_339;
static NJS_OBJECT collision_341;
static NJS_OBJECT collision_342;
static NJS_OBJECT collision_343;
static NJS_OBJECT collision_345;
static NJS_OBJECT collision_346;
static NJS_OBJECT collision_347;
static NJS_OBJECT collision_349;
static NJS_OBJECT collision_350;
static NJS_OBJECT collision_351;
static NJS_OBJECT collision_353;
static NJS_OBJECT collision_354;
static NJS_OBJECT collision_356;
static NJS_OBJECT collision_357;
static NJS_OBJECT collision_358;
static NJS_OBJECT collision_359;
static NJS_OBJECT collision_360;
static NJS_OBJECT collision_361;
static NJS_OBJECT collision_362;
static NJS_OBJECT collision_363;
static NJS_OBJECT collision_364;
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
static NJS_OBJECT collision_389;
static NJS_OBJECT collision_390;
static NJS_OBJECT collision_391;
static NJS_OBJECT collision_392;
static NJS_OBJECT collision_393;
static NJS_OBJECT collision_394;
static NJS_OBJECT collision_395;
static NJS_OBJECT collision_396;
static NJS_OBJECT collision_397;
static NJS_OBJECT collision_398;
static NJS_OBJECT collision_399;
static NJS_OBJECT collision_400;
static NJS_OBJECT collision_401;
static NJS_OBJECT collision_402;
static NJS_OBJECT collision_403;
static NJS_OBJECT collision_404;
static NJS_OBJECT collision_405;
static NJS_OBJECT collision_406;
static NJS_OBJECT collision_407;
static NJS_OBJECT collision_408;
static NJS_OBJECT collision_409;
static NJS_OBJECT collision_410;
static NJS_OBJECT collision_411;
static NJS_OBJECT collision_412;
static NJS_OBJECT collision_413;
static NJS_OBJECT collision_414;
static NJS_OBJECT collision_415;
static NJS_OBJECT collision_416;
static NJS_OBJECT collision_417;
static NJS_OBJECT collision_419;
static NJS_OBJECT collision_421;
static NJS_OBJECT collision_422;
static NJS_OBJECT collision_424;
static NJS_OBJECT collision_426;
static NJS_OBJECT collision_427;
static NJS_OBJECT collision_429;
static NJS_OBJECT collision_431;
static NJS_OBJECT collision_433;
static NJS_OBJECT collision_435;
static NJS_OBJECT collision_436;
static NJS_OBJECT collision_437;
static NJS_OBJECT collision_438;
static NJS_OBJECT collision_439;
static NJS_OBJECT collision_440;
static NJS_OBJECT collision_441;
static NJS_OBJECT collision_442;
static NJS_OBJECT collision_443;
static NJS_OBJECT collision_444;
static NJS_OBJECT collision_445;
static NJS_OBJECT collision_446;
static NJS_OBJECT collision_447;
static NJS_OBJECT collision_448;
static NJS_OBJECT collision_449;
static NJS_OBJECT collision_450;
static NJS_OBJECT collision_451;
static NJS_OBJECT collision_452;
static NJS_OBJECT collision_453;
static NJS_OBJECT collision_454;
static NJS_OBJECT collision_455;
static NJS_OBJECT collision_456;
static NJS_OBJECT collision_457;
static NJS_OBJECT collision_458;
static NJS_OBJECT collision_459;
static NJS_OBJECT collision_460;
static NJS_OBJECT collision_461;
static NJS_OBJECT collision_462;
static NJS_OBJECT collision_463;
static NJS_OBJECT collision_464;
static NJS_OBJECT collision_465;
static NJS_OBJECT collision_466;
static NJS_OBJECT collision_467;
static NJS_OBJECT collision_468;
static NJS_OBJECT collision_469;
static NJS_OBJECT collision_470;
static NJS_OBJECT collision_472;
static NJS_OBJECT collision_473;
static NJS_OBJECT collision_474;
static NJS_OBJECT collision_475;
static NJS_OBJECT collision_476;
static NJS_OBJECT collision_477;
static NJS_OBJECT collision_478;
static NJS_OBJECT collision_479;
static NJS_OBJECT collision_480;
static NJS_OBJECT collision_481;
static NJS_OBJECT collision_482;
static NJS_OBJECT collision_483;
static NJS_OBJECT collision_484;
static NJS_OBJECT collision_485;
static NJS_OBJECT collision_486;
static NJS_OBJECT collision_487;
static NJS_OBJECT collision_488;
static NJS_OBJECT collision_489;
static NJS_OBJECT collision_490;
static NJS_OBJECT collision_491;
static NJS_OBJECT collision_492;
static NJS_OBJECT collision_493;
static NJS_OBJECT collision_494;
static NJS_OBJECT collision_495;
static NJS_OBJECT collision_496;
static NJS_OBJECT collision_497;
static NJS_OBJECT collision_498;
static NJS_OBJECT collision_499;
static NJS_OBJECT collision_500;
static NJS_OBJECT collision_501;
static NJS_OBJECT collision_502;
static NJS_OBJECT collision_503;
static NJS_OBJECT collision_504;
static NJS_OBJECT collision_505;
static NJS_OBJECT collision_506;
static NJS_OBJECT collision_507;
static NJS_OBJECT collision_508;
static NJS_OBJECT collision_509;
static NJS_OBJECT collision_510;
static NJS_OBJECT collision_511;
static NJS_OBJECT collision_512;
static NJS_OBJECT collision_513;
static NJS_OBJECT collision_514;
static NJS_OBJECT collision_515;
static NJS_OBJECT collision_516;
static NJS_OBJECT collision_517;
static NJS_OBJECT collision_518;
static NJS_OBJECT collision_519;
static NJS_OBJECT collision_520;
static NJS_OBJECT collision_521;
static NJS_OBJECT collision_522;
static NJS_OBJECT collision_523;
static NJS_OBJECT collision_524;
static NJS_OBJECT collision_525;
static NJS_OBJECT collision_526;
static NJS_OBJECT collision_527;
static NJS_OBJECT collision_528;
static NJS_OBJECT collision_529;
static NJS_OBJECT collision_530;
static NJS_OBJECT collision_531;
static NJS_OBJECT collision_532;
static NJS_OBJECT collision_533;
static NJS_OBJECT collision_534;
static NJS_OBJECT collision_535;
static NJS_OBJECT collision_536;
static NJS_OBJECT collision_537;
static NJS_OBJECT collision_538;
static NJS_OBJECT collision_539;
static NJS_OBJECT collision_540;
static NJS_OBJECT collision_541;
static NJS_OBJECT collision_542;
static NJS_OBJECT collision_543;
static NJS_OBJECT collision_544;
static NJS_OBJECT collision_545;
static NJS_OBJECT collision_546;
static NJS_OBJECT collision_547;
static NJS_OBJECT collision_548;
static NJS_OBJECT collision_549;
static NJS_OBJECT collision_550;
static NJS_OBJECT collision_551;
static NJS_OBJECT collision_552;
static NJS_OBJECT collision_553;
static NJS_OBJECT collision_554;
static NJS_OBJECT collision_555;
static NJS_OBJECT collision_556;
static NJS_OBJECT collision_557;
static NJS_OBJECT collision_558;
static NJS_OBJECT collision_559;
static NJS_OBJECT collision_560;
static NJS_OBJECT collision_561;
static NJS_OBJECT collision_562;
static NJS_OBJECT collision_563;
static NJS_OBJECT collision_564;
static NJS_OBJECT collision_565;
static NJS_OBJECT collision_566;
static NJS_OBJECT collision_567;
static NJS_OBJECT collision_568;
static NJS_OBJECT collision_569;
static NJS_OBJECT collision_570;
static NJS_OBJECT collision_571;
static NJS_OBJECT collision_572;
static NJS_OBJECT collision_573;
static NJS_OBJECT collision_574;
static NJS_OBJECT collision_575;
static NJS_OBJECT collision_576;
static NJS_OBJECT collision_577;
static NJS_OBJECT collision_578;
static NJS_OBJECT collision_579;
static NJS_OBJECT collision_580;
static NJS_OBJECT collision_581;
static NJS_OBJECT collision_582;
static NJS_OBJECT collision_583;
static NJS_OBJECT collision_584;
static NJS_OBJECT collision_585;
static NJS_OBJECT collision_586;
static NJS_OBJECT collision_587;
static NJS_OBJECT collision_588;
static NJS_OBJECT collision_589;
static NJS_OBJECT collision_590;
static NJS_OBJECT collision_591;
static NJS_OBJECT collision_592;
static NJS_OBJECT collision_593;
static NJS_OBJECT collision_594;
static NJS_OBJECT collision_595;
static NJS_OBJECT collision_596;
static NJS_OBJECT collision_597;
static NJS_OBJECT collision_598;
static NJS_OBJECT collision_599;
static NJS_OBJECT collision_600;
static NJS_OBJECT collision_601;
static NJS_OBJECT collision_602;
static NJS_OBJECT collision_603;
static NJS_OBJECT collision_604;
static NJS_OBJECT collision_605;
static NJS_OBJECT collision_606;
static NJS_OBJECT collision_607;
static NJS_OBJECT collision_608;
static NJS_OBJECT collision_609;
static NJS_OBJECT collision_610;
static NJS_OBJECT collision_611;
static NJS_OBJECT collision_612;
static NJS_OBJECT collision_613;
static NJS_OBJECT collision_614;
static NJS_OBJECT collision_615;
static NJS_OBJECT collision_616;
static NJS_OBJECT collision_617;
static NJS_OBJECT collision_618;
static NJS_OBJECT collision_619;
static NJS_OBJECT collision_620;
static NJS_OBJECT collision_621;
static NJS_OBJECT collision_622;
static NJS_OBJECT collision_623;
static NJS_OBJECT collision_624;
static NJS_OBJECT collision_625;
static NJS_OBJECT collision_627;
static NJS_OBJECT collision_628;
static NJS_OBJECT collision_629;
static NJS_OBJECT collision_630;
static NJS_OBJECT collision_631;
static NJS_OBJECT collision_632;
static NJS_OBJECT collision_633;
static NJS_OBJECT collision_634;
static NJS_OBJECT collision_635;
static NJS_OBJECT collision_636;
static NJS_OBJECT collision_637;
static NJS_OBJECT collision_638;
static NJS_OBJECT collision_639;
static NJS_OBJECT collision_640;
static NJS_OBJECT collision_641;
static NJS_OBJECT collision_642;
static NJS_OBJECT collision_643;
static NJS_OBJECT collision_644;
static NJS_OBJECT collision_645;
static NJS_OBJECT collision_646;
static NJS_OBJECT collision_647;
static NJS_OBJECT collision_648;
static NJS_OBJECT collision_649;
static NJS_OBJECT collision_650;
static NJS_OBJECT collision_651;
static NJS_OBJECT collision_652;
static NJS_OBJECT collision_654;
static NJS_OBJECT collision_655;
static NJS_OBJECT collision_656;
static NJS_OBJECT collision_657;
static NJS_OBJECT collision_658;
static NJS_OBJECT collision_659;
static NJS_OBJECT collision_660;
static NJS_OBJECT collision_662;
static NJS_OBJECT collision_663;
static NJS_OBJECT collision_664;
static NJS_OBJECT collision_665;
static NJS_OBJECT collision_666;
static NJS_OBJECT collision_667;
static NJS_OBJECT collision_668;
static NJS_CNK_OBJECT draw_853;
static NJS_CNK_OBJECT draw_852;
static NJS_CNK_OBJECT draw_851;
static NJS_CNK_OBJECT draw_850;
static NJS_CNK_OBJECT draw_848;
static NJS_CNK_OBJECT draw_847;
static NJS_CNK_OBJECT draw_846;
static NJS_CNK_OBJECT draw_845;
static NJS_CNK_OBJECT draw_844;
static NJS_CNK_OBJECT draw_843;
static NJS_CNK_OBJECT draw_842;
static NJS_CNK_OBJECT draw_841;
static NJS_CNK_OBJECT draw_840;
static NJS_CNK_OBJECT draw_839;
static NJS_CNK_OBJECT draw_838;
static NJS_CNK_OBJECT draw_837;
static NJS_CNK_OBJECT draw_835;
static NJS_CNK_OBJECT draw_833;
static NJS_CNK_OBJECT draw_832;
static NJS_CNK_OBJECT draw_831;
static NJS_CNK_OBJECT draw_830;
static NJS_CNK_OBJECT draw_829;
static NJS_CNK_OBJECT draw_828;
static NJS_CNK_OBJECT draw_827;
static NJS_CNK_OBJECT draw_826;
static NJS_CNK_OBJECT draw_825;
static NJS_CNK_OBJECT draw_824;
static NJS_CNK_OBJECT draw_823;
static NJS_CNK_OBJECT draw_822;
static NJS_CNK_OBJECT draw_821;
static NJS_CNK_OBJECT draw_820;
static NJS_CNK_OBJECT draw_819;
static NJS_CNK_OBJECT draw_818;
static NJS_CNK_OBJECT draw_817;
static NJS_CNK_OBJECT draw_816;
static NJS_CNK_OBJECT draw_815;
static NJS_CNK_OBJECT draw_814;
static NJS_CNK_OBJECT draw_813;
static NJS_CNK_OBJECT draw_812;
static NJS_CNK_OBJECT draw_811;
static NJS_CNK_OBJECT draw_809;
static NJS_CNK_OBJECT draw_807;
static NJS_CNK_OBJECT draw_806;
static NJS_CNK_OBJECT draw_805;
static NJS_CNK_OBJECT draw_804;
static NJS_CNK_OBJECT draw_803;
static NJS_CNK_OBJECT draw_802;
static NJS_CNK_OBJECT draw_801;
static NJS_CNK_OBJECT draw_800;
static NJS_CNK_OBJECT draw_799;
static NJS_CNK_OBJECT draw_798;
static NJS_CNK_OBJECT draw_797;
static NJS_CNK_OBJECT draw_796;
static NJS_CNK_OBJECT draw_795;
static NJS_CNK_OBJECT draw_794;
static NJS_CNK_OBJECT draw_793;
static NJS_CNK_OBJECT draw_792;
static NJS_CNK_OBJECT draw_791;
static NJS_CNK_OBJECT draw_790;
static NJS_CNK_OBJECT draw_789;
static NJS_CNK_OBJECT draw_788;
static NJS_CNK_OBJECT draw_787;
static NJS_CNK_OBJECT draw_786;
static NJS_CNK_OBJECT draw_785;
static NJS_CNK_OBJECT draw_784;
static NJS_CNK_OBJECT draw_783;
static NJS_CNK_OBJECT draw_782;
static NJS_CNK_OBJECT draw_781;
static NJS_CNK_OBJECT draw_780;
static NJS_CNK_OBJECT draw_779;
static NJS_CNK_OBJECT draw_778;
static NJS_CNK_OBJECT draw_777;
static NJS_CNK_OBJECT draw_776;
static NJS_CNK_OBJECT draw_775;
static NJS_CNK_OBJECT draw_774;
static NJS_CNK_OBJECT draw_773;
static NJS_CNK_OBJECT draw_772;
static NJS_CNK_OBJECT draw_771;
static NJS_CNK_OBJECT draw_770;
static NJS_CNK_OBJECT draw_769;
static NJS_CNK_OBJECT draw_768;
static NJS_CNK_OBJECT draw_767;
static NJS_CNK_OBJECT draw_766;
static NJS_CNK_OBJECT draw_765;
static NJS_CNK_OBJECT draw_764;
static NJS_CNK_OBJECT draw_763;
static NJS_CNK_OBJECT draw_762;
static NJS_CNK_OBJECT draw_761;
static NJS_CNK_OBJECT draw_759;
static NJS_CNK_OBJECT draw_758;
static NJS_CNK_OBJECT draw_757;
static NJS_CNK_OBJECT draw_756;
static NJS_CNK_OBJECT draw_755;
static NJS_CNK_OBJECT draw_754;
static NJS_CNK_OBJECT draw_753;
static NJS_CNK_OBJECT draw_752;
static NJS_CNK_OBJECT draw_751;
static NJS_CNK_OBJECT draw_750;
static NJS_CNK_OBJECT draw_749;
static NJS_CNK_OBJECT draw_748;
static NJS_CNK_OBJECT draw_747;
static NJS_CNK_OBJECT draw_746;
static NJS_CNK_OBJECT draw_745;
static NJS_CNK_OBJECT draw_744;
static NJS_CNK_OBJECT draw_743;
static NJS_CNK_OBJECT draw_742;
static NJS_CNK_OBJECT draw_741;
static NJS_CNK_OBJECT draw_740;
static NJS_CNK_OBJECT draw_739;
static NJS_CNK_OBJECT draw_738;
static NJS_CNK_OBJECT draw_737;
static NJS_CNK_OBJECT draw_736;
static NJS_CNK_OBJECT draw_735;
static NJS_CNK_OBJECT draw_734;
static NJS_CNK_OBJECT draw_733;
static NJS_CNK_OBJECT draw_729;
static NJS_CNK_OBJECT draw_727;
static NJS_CNK_OBJECT draw_726;
static NJS_CNK_OBJECT draw_724;
static NJS_CNK_OBJECT draw_723;
static NJS_CNK_OBJECT draw_718;
static NJS_CNK_OBJECT draw_717;
static NJS_CNK_OBJECT draw_712;
static NJS_CNK_OBJECT draw_709;
static NJS_CNK_OBJECT draw_708;
static NJS_CNK_OBJECT draw_707;
static NJS_CNK_OBJECT draw_706;
static NJS_CNK_OBJECT draw_705;
static NJS_CNK_OBJECT draw_704;
static NJS_CNK_OBJECT draw_703;
static NJS_CNK_OBJECT draw_702;
static NJS_CNK_OBJECT draw_701;
static NJS_CNK_OBJECT draw_700;
static NJS_CNK_OBJECT draw_699;
static NJS_CNK_OBJECT draw_698;
static NJS_CNK_OBJECT draw_697;
static NJS_CNK_OBJECT draw_696;
static NJS_CNK_OBJECT draw_695;
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
static NJS_CNK_OBJECT draw_679;
static NJS_CNK_OBJECT draw_678;
static NJS_CNK_OBJECT draw_677;
static NJS_CNK_OBJECT draw_676;
static NJS_CNK_OBJECT draw_675;
static NJS_CNK_OBJECT draw_674;
static NJS_CNK_OBJECT draw_673;
static NJS_CNK_OBJECT draw_672;
static NJS_CNK_OBJECT draw_671;
static NJS_CNK_OBJECT draw_670;
static NJS_CNK_OBJECT draw_669;
static NJS_CNK_OBJECT draw_668;
static NJS_CNK_OBJECT draw_667;
static NJS_CNK_OBJECT draw_666;
static NJS_CNK_OBJECT draw_665;
static NJS_CNK_OBJECT draw_664;
static NJS_CNK_OBJECT draw_663;
static NJS_CNK_OBJECT draw_662;
static NJS_CNK_OBJECT draw_661;
static NJS_CNK_OBJECT draw_660;
static NJS_CNK_OBJECT draw_659;
static NJS_CNK_OBJECT draw_658;
static NJS_CNK_OBJECT draw_657;
static NJS_CNK_OBJECT draw_656;
static NJS_CNK_OBJECT draw_655;
static NJS_CNK_OBJECT draw_654;
static NJS_CNK_OBJECT draw_653;
static NJS_CNK_OBJECT draw_652;
static NJS_CNK_OBJECT draw_651;
static NJS_CNK_OBJECT draw_650;
static NJS_CNK_OBJECT draw_649;
static NJS_CNK_OBJECT draw_648;
static NJS_CNK_OBJECT draw_647;
static NJS_CNK_OBJECT draw_646;
static NJS_CNK_OBJECT draw_645;
static NJS_CNK_OBJECT draw_644;
static NJS_CNK_OBJECT draw_643;
static NJS_CNK_OBJECT draw_642;
static NJS_CNK_OBJECT draw_641;
static NJS_CNK_OBJECT draw_640;
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
static NJS_CNK_OBJECT draw_626;
static NJS_CNK_OBJECT draw_625;
static NJS_CNK_OBJECT draw_624;
static NJS_CNK_OBJECT draw_623;
static NJS_CNK_OBJECT draw_622;
static NJS_CNK_OBJECT draw_621;
static NJS_CNK_OBJECT draw_620;
static NJS_CNK_OBJECT draw_619;
static NJS_CNK_OBJECT draw_618;
static NJS_CNK_OBJECT draw_617;
static NJS_CNK_OBJECT draw_616;
static NJS_CNK_OBJECT draw_615;
static NJS_CNK_OBJECT draw_614;
static NJS_CNK_OBJECT draw_613;
static NJS_CNK_OBJECT draw_612;
static NJS_CNK_OBJECT draw_611;
static NJS_CNK_OBJECT draw_610;
static NJS_CNK_OBJECT draw_609;
static NJS_CNK_OBJECT draw_608;
static NJS_CNK_OBJECT draw_607;
static NJS_CNK_OBJECT draw_606;
static NJS_CNK_OBJECT draw_605;
static NJS_CNK_OBJECT draw_604;
static NJS_CNK_OBJECT draw_603;
static NJS_CNK_OBJECT draw_602;
static NJS_CNK_OBJECT draw_601;
static NJS_CNK_OBJECT draw_600;
static NJS_CNK_OBJECT draw_599;
static NJS_CNK_OBJECT draw_598;
static NJS_CNK_OBJECT draw_597;
static NJS_CNK_OBJECT draw_596;
static NJS_CNK_OBJECT draw_595;
static NJS_CNK_OBJECT draw_594;
static NJS_CNK_OBJECT draw_593;
static NJS_CNK_OBJECT draw_592;
static NJS_CNK_OBJECT draw_590;
static NJS_CNK_OBJECT draw_589;
static NJS_CNK_OBJECT draw_583;
static NJS_CNK_OBJECT draw_582;
static NJS_CNK_OBJECT draw_580;
static NJS_CNK_OBJECT draw_579;
static NJS_CNK_OBJECT draw_576;
static NJS_CNK_OBJECT draw_573;
static NJS_CNK_OBJECT draw_572;
static NJS_CNK_OBJECT draw_571;
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
static NJS_CNK_OBJECT draw_557;
static NJS_CNK_OBJECT draw_555;
static NJS_CNK_OBJECT draw_554;
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
static NJS_CNK_OBJECT draw_543;
static NJS_CNK_OBJECT draw_542;
static NJS_CNK_OBJECT draw_541;
static NJS_CNK_OBJECT draw_540;
static NJS_CNK_OBJECT draw_539;
static NJS_CNK_OBJECT draw_538;
static NJS_CNK_OBJECT draw_537;
static NJS_CNK_OBJECT draw_536;
static NJS_CNK_OBJECT draw_535;
static NJS_CNK_OBJECT draw_534;
static NJS_CNK_OBJECT draw_533;
static NJS_CNK_OBJECT draw_532;
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
static NJS_CNK_OBJECT draw_508;
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
static NJS_CNK_OBJECT draw_496;
static NJS_CNK_OBJECT draw_495;
static NJS_CNK_OBJECT draw_494;
static NJS_CNK_OBJECT draw_493;
static NJS_CNK_OBJECT draw_492;
static NJS_CNK_OBJECT draw_491;
static NJS_CNK_OBJECT draw_490;
static NJS_CNK_OBJECT draw_489;
static NJS_CNK_OBJECT draw_488;
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
static NJS_CNK_OBJECT draw_476;
static NJS_CNK_OBJECT draw_475;
static NJS_CNK_OBJECT draw_474;
static NJS_CNK_OBJECT draw_473;
static NJS_CNK_OBJECT draw_472;
static NJS_CNK_OBJECT draw_471;
static NJS_CNK_OBJECT draw_470;
static NJS_CNK_OBJECT draw_469;
static NJS_CNK_OBJECT draw_468;
static NJS_CNK_OBJECT draw_467;
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
static NJS_CNK_OBJECT draw_422;
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
static NJS_CNK_OBJECT draw_402;
static NJS_CNK_OBJECT draw_401;
static NJS_CNK_OBJECT draw_400;
static NJS_CNK_OBJECT draw_399;
static NJS_CNK_OBJECT draw_398;
static NJS_CNK_OBJECT draw_396;
static NJS_CNK_OBJECT draw_395;
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

static struct land_entry entries[1527] = {
#include "stg30/terrain/entries.inc"
};
struct land_table rush_land = {
#include "stg30/terrain/rush_land.inc"
};
static NJS_TEXNAME textures[38] = {
#include "stg30/terrain/textures.inc"
};
NJS_TEXLIST rush_land_texlist = {
#include "stg30/terrain/rush_land_texlist.inc"
};
static NJS_MATERIAL collision_0_materials[1] = {
#include "stg30/terrain/collision_0_materials.inc"
};
static Sint16 collision_0_faces[46] = {
#include "stg30/terrain/collision_0_faces.inc"
};
static NJS_MESHSET collision_0_meshsets[1] = {
#include "stg30/terrain/collision_0_meshsets.inc"
};
static NJS_POINT3 collision_0_points[30] = {
#include "stg30/terrain/collision_0_points.inc"
};
static NJS_MODEL collision_0_model = {
#include "stg30/terrain/collision_0_model.inc"
};
static NJS_OBJECT collision_0 = {
#include "stg30/terrain/collision_0.inc"
};
static NJS_MATERIAL collision_1_materials[1] = {
#include "stg30/terrain/collision_1_materials.inc"
};
static Sint16 collision_1_faces[27] = {
#include "stg30/terrain/collision_1_faces.inc"
};
static NJS_MESHSET collision_1_meshsets[1] = {
#include "stg30/terrain/collision_1_meshsets.inc"
};
static NJS_POINT3 collision_1_points[16] = {
#include "stg30/terrain/collision_1_points.inc"
};
static NJS_MODEL collision_1_model = {
#include "stg30/terrain/collision_1_model.inc"
};
static NJS_OBJECT collision_1 = {
#include "stg30/terrain/collision_1.inc"
};
static NJS_MATERIAL collision_5_materials[1] = {
#include "stg30/terrain/collision_5_materials.inc"
};
static Sint16 collision_5_faces[46] = {
#include "stg30/terrain/collision_5_faces.inc"
};
static NJS_MESHSET collision_5_meshsets[1] = {
#include "stg30/terrain/collision_5_meshsets.inc"
};
static NJS_POINT3 collision_5_points[30] = {
#include "stg30/terrain/collision_5_points.inc"
};
static NJS_MODEL collision_5_model = {
#include "stg30/terrain/collision_5_model.inc"
};
static NJS_OBJECT collision_5 = {
#include "stg30/terrain/collision_5.inc"
};
static NJS_MATERIAL collision_6_materials[1] = {
#include "stg30/terrain/collision_6_materials.inc"
};
static Sint16 collision_6_faces[48] = {
#include "stg30/terrain/collision_6_faces.inc"
};
static NJS_MESHSET collision_6_meshsets[1] = {
#include "stg30/terrain/collision_6_meshsets.inc"
};
static NJS_POINT3 collision_6_points[30] = {
#include "stg30/terrain/collision_6_points.inc"
};
static NJS_MODEL collision_6_model = {
#include "stg30/terrain/collision_6_model.inc"
};
static NJS_OBJECT collision_6 = {
#include "stg30/terrain/collision_6.inc"
};
static NJS_MATERIAL collision_7_materials[1] = {
#include "stg30/terrain/collision_7_materials.inc"
};
static Sint16 collision_7_faces[46] = {
#include "stg30/terrain/collision_7_faces.inc"
};
static NJS_MESHSET collision_7_meshsets[1] = {
#include "stg30/terrain/collision_7_meshsets.inc"
};
static NJS_POINT3 collision_7_points[30] = {
#include "stg30/terrain/collision_7_points.inc"
};
static NJS_MODEL collision_7_model = {
#include "stg30/terrain/collision_7_model.inc"
};
static NJS_OBJECT collision_7 = {
#include "stg30/terrain/collision_7.inc"
};
static NJS_MATERIAL collision_10_materials[1] = {
#include "stg30/terrain/collision_10_materials.inc"
};
static Sint16 collision_10_faces[39] = {
#include "stg30/terrain/collision_10_faces.inc"
};
static NJS_MESHSET collision_10_meshsets[1] = {
#include "stg30/terrain/collision_10_meshsets.inc"
};
static NJS_POINT3 collision_10_points[30] = {
#include "stg30/terrain/collision_10_points.inc"
};
static NJS_MODEL collision_10_model = {
#include "stg30/terrain/collision_10_model.inc"
};
static NJS_OBJECT collision_10 = {
#include "stg30/terrain/collision_10.inc"
};
static NJS_MATERIAL collision_11_materials[1] = {
#include "stg30/terrain/collision_11_materials.inc"
};
static Sint16 collision_11_faces[39] = {
#include "stg30/terrain/collision_11_faces.inc"
};
static NJS_MESHSET collision_11_meshsets[1] = {
#include "stg30/terrain/collision_11_meshsets.inc"
};
static NJS_POINT3 collision_11_points[30] = {
#include "stg30/terrain/collision_11_points.inc"
};
static NJS_MODEL collision_11_model = {
#include "stg30/terrain/collision_11_model.inc"
};
static NJS_OBJECT collision_11 = {
#include "stg30/terrain/collision_11.inc"
};
static NJS_MATERIAL collision_14_materials[1] = {
#include "stg30/terrain/collision_14_materials.inc"
};
static Sint16 collision_14_faces[53] = {
#include "stg30/terrain/collision_14_faces.inc"
};
static NJS_MESHSET collision_14_meshsets[1] = {
#include "stg30/terrain/collision_14_meshsets.inc"
};
static NJS_POINT3 collision_14_points[24] = {
#include "stg30/terrain/collision_14_points.inc"
};
static NJS_MODEL collision_14_model = {
#include "stg30/terrain/collision_14_model.inc"
};
static NJS_OBJECT collision_14 = {
#include "stg30/terrain/collision_14.inc"
};
static NJS_MATERIAL collision_19_materials[1] = {
#include "stg30/terrain/collision_19_materials.inc"
};
static Sint16 collision_19_faces[188] = {
#include "stg30/terrain/collision_19_faces.inc"
};
static NJS_MESHSET collision_19_meshsets[1] = {
#include "stg30/terrain/collision_19_meshsets.inc"
};
static NJS_POINT3 collision_19_points[80] = {
#include "stg30/terrain/collision_19_points.inc"
};
static NJS_MODEL collision_19_model = {
#include "stg30/terrain/collision_19_model.inc"
};
static NJS_OBJECT collision_19 = {
#include "stg30/terrain/collision_19.inc"
};
static NJS_MATERIAL collision_20_materials[1] = {
#include "stg30/terrain/collision_20_materials.inc"
};
static Sint16 collision_20_faces[152] = {
#include "stg30/terrain/collision_20_faces.inc"
};
static NJS_MESHSET collision_20_meshsets[1] = {
#include "stg30/terrain/collision_20_meshsets.inc"
};
static NJS_POINT3 collision_20_points[73] = {
#include "stg30/terrain/collision_20_points.inc"
};
static NJS_MODEL collision_20_model = {
#include "stg30/terrain/collision_20_model.inc"
};
static NJS_OBJECT collision_20 = {
#include "stg30/terrain/collision_20.inc"
};
static NJS_MATERIAL collision_21_materials[1] = {
#include "stg30/terrain/collision_21_materials.inc"
};
static Sint16 collision_21_faces[27] = {
#include "stg30/terrain/collision_21_faces.inc"
};
static NJS_MESHSET collision_21_meshsets[1] = {
#include "stg30/terrain/collision_21_meshsets.inc"
};
static NJS_POINT3 collision_21_points[16] = {
#include "stg30/terrain/collision_21_points.inc"
};
static NJS_MODEL collision_21_model = {
#include "stg30/terrain/collision_21_model.inc"
};
static NJS_OBJECT collision_21 = {
#include "stg30/terrain/collision_21.inc"
};
static NJS_MATERIAL collision_25_materials[1] = {
#include "stg30/terrain/collision_25_materials.inc"
};
static Sint16 collision_25_faces[27] = {
#include "stg30/terrain/collision_25_faces.inc"
};
static NJS_MESHSET collision_25_meshsets[1] = {
#include "stg30/terrain/collision_25_meshsets.inc"
};
static NJS_POINT3 collision_25_points[16] = {
#include "stg30/terrain/collision_25_points.inc"
};
static NJS_MODEL collision_25_model = {
#include "stg30/terrain/collision_25_model.inc"
};
static NJS_OBJECT collision_25 = {
#include "stg30/terrain/collision_25.inc"
};
static NJS_MATERIAL collision_95_materials[1] = {
#include "stg30/terrain/collision_95_materials.inc"
};
static Sint16 collision_95_faces[77] = {
#include "stg30/terrain/collision_95_faces.inc"
};
static NJS_MESHSET collision_95_meshsets[1] = {
#include "stg30/terrain/collision_95_meshsets.inc"
};
static NJS_POINT3 collision_95_points[34] = {
#include "stg30/terrain/collision_95_points.inc"
};
static NJS_MODEL collision_95_model = {
#include "stg30/terrain/collision_95_model.inc"
};
static NJS_OBJECT collision_95 = {
#include "stg30/terrain/collision_95.inc"
};
static NJS_MATERIAL collision_98_materials[1] = {
#include "stg30/terrain/collision_98_materials.inc"
};
static Sint16 collision_98_faces[22] = {
#include "stg30/terrain/collision_98_faces.inc"
};
static NJS_MESHSET collision_98_meshsets[1] = {
#include "stg30/terrain/collision_98_meshsets.inc"
};
static NJS_POINT3 collision_98_points[12] = {
#include "stg30/terrain/collision_98_points.inc"
};
static NJS_MODEL collision_98_model = {
#include "stg30/terrain/collision_98_model.inc"
};
static NJS_OBJECT collision_98 = {
#include "stg30/terrain/collision_98.inc"
};
static NJS_MATERIAL collision_104_materials[1] = {
#include "stg30/terrain/collision_104_materials.inc"
};
static Sint16 collision_104_faces[6] = {
#include "stg30/terrain/collision_104_faces.inc"
};
static NJS_MESHSET collision_104_meshsets[1] = {
#include "stg30/terrain/collision_104_meshsets.inc"
};
static NJS_POINT3 collision_104_points[5] = {
#include "stg30/terrain/collision_104_points.inc"
};
static NJS_MODEL collision_104_model = {
#include "stg30/terrain/collision_104_model.inc"
};
static NJS_OBJECT collision_104 = {
#include "stg30/terrain/collision_104.inc"
};
static NJS_MATERIAL collision_105_materials[1] = {
#include "stg30/terrain/collision_105_materials.inc"
};
static Sint16 collision_105_faces[39] = {
#include "stg30/terrain/collision_105_faces.inc"
};
static NJS_MESHSET collision_105_meshsets[1] = {
#include "stg30/terrain/collision_105_meshsets.inc"
};
static NJS_POINT3 collision_105_points[24] = {
#include "stg30/terrain/collision_105_points.inc"
};
static NJS_MODEL collision_105_model = {
#include "stg30/terrain/collision_105_model.inc"
};
static NJS_OBJECT collision_105 = {
#include "stg30/terrain/collision_105.inc"
};
static NJS_MATERIAL collision_106_materials[1] = {
#include "stg30/terrain/collision_106_materials.inc"
};
static Sint16 collision_106_faces[27] = {
#include "stg30/terrain/collision_106_faces.inc"
};
static NJS_MESHSET collision_106_meshsets[1] = {
#include "stg30/terrain/collision_106_meshsets.inc"
};
static NJS_POINT3 collision_106_points[16] = {
#include "stg30/terrain/collision_106_points.inc"
};
static NJS_MODEL collision_106_model = {
#include "stg30/terrain/collision_106_model.inc"
};
static NJS_OBJECT collision_106 = {
#include "stg30/terrain/collision_106.inc"
};
static NJS_MATERIAL collision_110_materials[1] = {
#include "stg30/terrain/collision_110_materials.inc"
};
static Sint16 collision_110_faces[55] = {
#include "stg30/terrain/collision_110_faces.inc"
};
static NJS_MESHSET collision_110_meshsets[1] = {
#include "stg30/terrain/collision_110_meshsets.inc"
};
static NJS_POINT3 collision_110_points[30] = {
#include "stg30/terrain/collision_110_points.inc"
};
static NJS_MODEL collision_110_model = {
#include "stg30/terrain/collision_110_model.inc"
};
static NJS_OBJECT collision_110 = {
#include "stg30/terrain/collision_110.inc"
};
static NJS_MATERIAL collision_111_materials[1] = {
#include "stg30/terrain/collision_111_materials.inc"
};
static Sint16 collision_111_faces[39] = {
#include "stg30/terrain/collision_111_faces.inc"
};
static NJS_MESHSET collision_111_meshsets[1] = {
#include "stg30/terrain/collision_111_meshsets.inc"
};
static NJS_POINT3 collision_111_points[18] = {
#include "stg30/terrain/collision_111_points.inc"
};
static NJS_MODEL collision_111_model = {
#include "stg30/terrain/collision_111_model.inc"
};
static NJS_OBJECT collision_111 = {
#include "stg30/terrain/collision_111.inc"
};
static NJS_MATERIAL collision_112_materials[1] = {
#include "stg30/terrain/collision_112_materials.inc"
};
static Sint16 collision_112_faces[260] = {
#include "stg30/terrain/collision_112_faces.inc"
};
static NJS_MESHSET collision_112_meshsets[1] = {
#include "stg30/terrain/collision_112_meshsets.inc"
};
static NJS_POINT3 collision_112_points[120] = {
#include "stg30/terrain/collision_112_points.inc"
};
static NJS_MODEL collision_112_model = {
#include "stg30/terrain/collision_112_model.inc"
};
static NJS_OBJECT collision_112 = {
#include "stg30/terrain/collision_112.inc"
};
static NJS_MATERIAL collision_113_materials[1] = {
#include "stg30/terrain/collision_113_materials.inc"
};
static Sint16 collision_113_faces[46] = {
#include "stg30/terrain/collision_113_faces.inc"
};
static NJS_MESHSET collision_113_meshsets[1] = {
#include "stg30/terrain/collision_113_meshsets.inc"
};
static NJS_POINT3 collision_113_points[19] = {
#include "stg30/terrain/collision_113_points.inc"
};
static NJS_MODEL collision_113_model = {
#include "stg30/terrain/collision_113_model.inc"
};
static NJS_OBJECT collision_113 = {
#include "stg30/terrain/collision_113.inc"
};
static NJS_MATERIAL collision_115_materials[1] = {
#include "stg30/terrain/collision_115_materials.inc"
};
static Sint16 collision_115_faces[51] = {
#include "stg30/terrain/collision_115_faces.inc"
};
static NJS_MESHSET collision_115_meshsets[1] = {
#include "stg30/terrain/collision_115_meshsets.inc"
};
static NJS_POINT3 collision_115_points[26] = {
#include "stg30/terrain/collision_115_points.inc"
};
static NJS_MODEL collision_115_model = {
#include "stg30/terrain/collision_115_model.inc"
};
static NJS_OBJECT collision_115 = {
#include "stg30/terrain/collision_115.inc"
};
static NJS_MATERIAL collision_119_materials[1] = {
#include "stg30/terrain/collision_119_materials.inc"
};
static Sint16 collision_119_faces[260] = {
#include "stg30/terrain/collision_119_faces.inc"
};
static NJS_MESHSET collision_119_meshsets[1] = {
#include "stg30/terrain/collision_119_meshsets.inc"
};
static NJS_POINT3 collision_119_points[120] = {
#include "stg30/terrain/collision_119_points.inc"
};
static NJS_MODEL collision_119_model = {
#include "stg30/terrain/collision_119_model.inc"
};
static NJS_OBJECT collision_119 = {
#include "stg30/terrain/collision_119.inc"
};
static NJS_MATERIAL collision_120_materials[1] = {
#include "stg30/terrain/collision_120_materials.inc"
};
static Sint16 collision_120_faces[263] = {
#include "stg30/terrain/collision_120_faces.inc"
};
static NJS_MESHSET collision_120_meshsets[1] = {
#include "stg30/terrain/collision_120_meshsets.inc"
};
static NJS_POINT3 collision_120_points[120] = {
#include "stg30/terrain/collision_120_points.inc"
};
static NJS_MODEL collision_120_model = {
#include "stg30/terrain/collision_120_model.inc"
};
static NJS_OBJECT collision_120 = {
#include "stg30/terrain/collision_120.inc"
};
static NJS_MATERIAL collision_121_materials[1] = {
#include "stg30/terrain/collision_121_materials.inc"
};
static Sint16 collision_121_faces[122] = {
#include "stg30/terrain/collision_121_faces.inc"
};
static NJS_MESHSET collision_121_meshsets[1] = {
#include "stg30/terrain/collision_121_meshsets.inc"
};
static NJS_POINT3 collision_121_points[63] = {
#include "stg30/terrain/collision_121_points.inc"
};
static NJS_MODEL collision_121_model = {
#include "stg30/terrain/collision_121_model.inc"
};
static NJS_OBJECT collision_121 = {
#include "stg30/terrain/collision_121.inc"
};
static NJS_MATERIAL collision_122_materials[1] = {
#include "stg30/terrain/collision_122_materials.inc"
};
static Sint16 collision_122_faces[63] = {
#include "stg30/terrain/collision_122_faces.inc"
};
static NJS_MESHSET collision_122_meshsets[1] = {
#include "stg30/terrain/collision_122_meshsets.inc"
};
static NJS_POINT3 collision_122_points[32] = {
#include "stg30/terrain/collision_122_points.inc"
};
static NJS_MODEL collision_122_model = {
#include "stg30/terrain/collision_122_model.inc"
};
static NJS_OBJECT collision_122 = {
#include "stg30/terrain/collision_122.inc"
};
static NJS_MATERIAL collision_125_materials[1] = {
#include "stg30/terrain/collision_125_materials.inc"
};
static Sint16 collision_125_faces[18] = {
#include "stg30/terrain/collision_125_faces.inc"
};
static NJS_MESHSET collision_125_meshsets[1] = {
#include "stg30/terrain/collision_125_meshsets.inc"
};
static NJS_POINT3 collision_125_points[12] = {
#include "stg30/terrain/collision_125_points.inc"
};
static NJS_MODEL collision_125_model = {
#include "stg30/terrain/collision_125_model.inc"
};
static NJS_OBJECT collision_125 = {
#include "stg30/terrain/collision_125.inc"
};
static NJS_MATERIAL collision_127_materials[1] = {
#include "stg30/terrain/collision_127_materials.inc"
};
static Sint16 collision_127_faces[132] = {
#include "stg30/terrain/collision_127_faces.inc"
};
static NJS_MESHSET collision_127_meshsets[1] = {
#include "stg30/terrain/collision_127_meshsets.inc"
};
static NJS_POINT3 collision_127_points[77] = {
#include "stg30/terrain/collision_127_points.inc"
};
static NJS_MODEL collision_127_model = {
#include "stg30/terrain/collision_127_model.inc"
};
static NJS_OBJECT collision_127 = {
#include "stg30/terrain/collision_127.inc"
};
static NJS_MATERIAL collision_129_materials[1] = {
#include "stg30/terrain/collision_129_materials.inc"
};
static Sint16 collision_129_faces[33] = {
#include "stg30/terrain/collision_129_faces.inc"
};
static NJS_MESHSET collision_129_meshsets[1] = {
#include "stg30/terrain/collision_129_meshsets.inc"
};
static NJS_POINT3 collision_129_points[20] = {
#include "stg30/terrain/collision_129_points.inc"
};
static NJS_MODEL collision_129_model = {
#include "stg30/terrain/collision_129_model.inc"
};
static NJS_OBJECT collision_129 = {
#include "stg30/terrain/collision_129.inc"
};
static NJS_MATERIAL collision_133_materials[1] = {
#include "stg30/terrain/collision_133_materials.inc"
};
static Sint16 collision_133_faces[36] = {
#include "stg30/terrain/collision_133_faces.inc"
};
static NJS_MESHSET collision_133_meshsets[1] = {
#include "stg30/terrain/collision_133_meshsets.inc"
};
static NJS_POINT3 collision_133_points[20] = {
#include "stg30/terrain/collision_133_points.inc"
};
static NJS_MODEL collision_133_model = {
#include "stg30/terrain/collision_133_model.inc"
};
static NJS_OBJECT collision_133 = {
#include "stg30/terrain/collision_133.inc"
};
static NJS_MATERIAL collision_144_materials[1] = {
#include "stg30/terrain/collision_144_materials.inc"
};
static Sint16 collision_144_faces[202] = {
#include "stg30/terrain/collision_144_faces.inc"
};
static NJS_MESHSET collision_144_meshsets[1] = {
#include "stg30/terrain/collision_144_meshsets.inc"
};
static NJS_POINT3 collision_144_points[93] = {
#include "stg30/terrain/collision_144_points.inc"
};
static NJS_MODEL collision_144_model = {
#include "stg30/terrain/collision_144_model.inc"
};
static NJS_OBJECT collision_144 = {
#include "stg30/terrain/collision_144.inc"
};
static NJS_MATERIAL collision_146_materials[1] = {
#include "stg30/terrain/collision_146_materials.inc"
};
static Sint16 collision_146_faces[45] = {
#include "stg30/terrain/collision_146_faces.inc"
};
static NJS_MESHSET collision_146_meshsets[1] = {
#include "stg30/terrain/collision_146_meshsets.inc"
};
static NJS_POINT3 collision_146_points[24] = {
#include "stg30/terrain/collision_146_points.inc"
};
static NJS_MODEL collision_146_model = {
#include "stg30/terrain/collision_146_model.inc"
};
static NJS_OBJECT collision_146 = {
#include "stg30/terrain/collision_146.inc"
};
static NJS_MATERIAL collision_148_materials[1] = {
#include "stg30/terrain/collision_148_materials.inc"
};
static Sint16 collision_148_faces[13] = {
#include "stg30/terrain/collision_148_faces.inc"
};
static NJS_MESHSET collision_148_meshsets[1] = {
#include "stg30/terrain/collision_148_meshsets.inc"
};
static NJS_POINT3 collision_148_points[12] = {
#include "stg30/terrain/collision_148_points.inc"
};
static NJS_MODEL collision_148_model = {
#include "stg30/terrain/collision_148_model.inc"
};
static NJS_OBJECT collision_148 = {
#include "stg30/terrain/collision_148.inc"
};
static NJS_MATERIAL collision_150_materials[1] = {
#include "stg30/terrain/collision_150_materials.inc"
};
static Sint16 collision_150_faces[26] = {
#include "stg30/terrain/collision_150_faces.inc"
};
static NJS_MESHSET collision_150_meshsets[1] = {
#include "stg30/terrain/collision_150_meshsets.inc"
};
static NJS_POINT3 collision_150_points[16] = {
#include "stg30/terrain/collision_150_points.inc"
};
static NJS_MODEL collision_150_model = {
#include "stg30/terrain/collision_150_model.inc"
};
static NJS_OBJECT collision_150 = {
#include "stg30/terrain/collision_150.inc"
};
static NJS_MATERIAL collision_151_materials[1] = {
#include "stg30/terrain/collision_151_materials.inc"
};
static Sint16 collision_151_faces[64] = {
#include "stg30/terrain/collision_151_faces.inc"
};
static NJS_MESHSET collision_151_meshsets[1] = {
#include "stg30/terrain/collision_151_meshsets.inc"
};
static NJS_POINT3 collision_151_points[33] = {
#include "stg30/terrain/collision_151_points.inc"
};
static NJS_MODEL collision_151_model = {
#include "stg30/terrain/collision_151_model.inc"
};
static NJS_OBJECT collision_151 = {
#include "stg30/terrain/collision_151.inc"
};
static NJS_MATERIAL collision_152_materials[1] = {
#include "stg30/terrain/collision_152_materials.inc"
};
static Sint16 collision_152_faces[81] = {
#include "stg30/terrain/collision_152_faces.inc"
};
static NJS_MESHSET collision_152_meshsets[1] = {
#include "stg30/terrain/collision_152_meshsets.inc"
};
static NJS_POINT3 collision_152_points[48] = {
#include "stg30/terrain/collision_152_points.inc"
};
static NJS_MODEL collision_152_model = {
#include "stg30/terrain/collision_152_model.inc"
};
static NJS_OBJECT collision_152 = {
#include "stg30/terrain/collision_152.inc"
};
static NJS_MATERIAL collision_153_materials[1] = {
#include "stg30/terrain/collision_153_materials.inc"
};
static Sint16 collision_153_faces[26] = {
#include "stg30/terrain/collision_153_faces.inc"
};
static NJS_MESHSET collision_153_meshsets[1] = {
#include "stg30/terrain/collision_153_meshsets.inc"
};
static NJS_POINT3 collision_153_points[16] = {
#include "stg30/terrain/collision_153_points.inc"
};
static NJS_MODEL collision_153_model = {
#include "stg30/terrain/collision_153_model.inc"
};
static NJS_OBJECT collision_153 = {
#include "stg30/terrain/collision_153.inc"
};
static NJS_MATERIAL collision_154_materials[1] = {
#include "stg30/terrain/collision_154_materials.inc"
};
static Sint16 collision_154_faces[91] = {
#include "stg30/terrain/collision_154_faces.inc"
};
static NJS_MESHSET collision_154_meshsets[1] = {
#include "stg30/terrain/collision_154_meshsets.inc"
};
static NJS_POINT3 collision_154_points[40] = {
#include "stg30/terrain/collision_154_points.inc"
};
static NJS_MODEL collision_154_model = {
#include "stg30/terrain/collision_154_model.inc"
};
static NJS_OBJECT collision_154 = {
#include "stg30/terrain/collision_154.inc"
};
static NJS_MATERIAL collision_155_materials[1] = {
#include "stg30/terrain/collision_155_materials.inc"
};
static Sint16 collision_155_faces[98] = {
#include "stg30/terrain/collision_155_faces.inc"
};
static NJS_MESHSET collision_155_meshsets[1] = {
#include "stg30/terrain/collision_155_meshsets.inc"
};
static NJS_POINT3 collision_155_points[52] = {
#include "stg30/terrain/collision_155_points.inc"
};
static NJS_MODEL collision_155_model = {
#include "stg30/terrain/collision_155_model.inc"
};
static NJS_OBJECT collision_155 = {
#include "stg30/terrain/collision_155.inc"
};
static NJS_MATERIAL collision_156_materials[1] = {
#include "stg30/terrain/collision_156_materials.inc"
};
static Sint16 collision_156_faces[63] = {
#include "stg30/terrain/collision_156_faces.inc"
};
static NJS_MESHSET collision_156_meshsets[1] = {
#include "stg30/terrain/collision_156_meshsets.inc"
};
static NJS_POINT3 collision_156_points[36] = {
#include "stg30/terrain/collision_156_points.inc"
};
static NJS_MODEL collision_156_model = {
#include "stg30/terrain/collision_156_model.inc"
};
static NJS_OBJECT collision_156 = {
#include "stg30/terrain/collision_156.inc"
};
static NJS_MATERIAL collision_157_materials[1] = {
#include "stg30/terrain/collision_157_materials.inc"
};
static Sint16 collision_157_faces[63] = {
#include "stg30/terrain/collision_157_faces.inc"
};
static NJS_MESHSET collision_157_meshsets[1] = {
#include "stg30/terrain/collision_157_meshsets.inc"
};
static NJS_POINT3 collision_157_points[36] = {
#include "stg30/terrain/collision_157_points.inc"
};
static NJS_MODEL collision_157_model = {
#include "stg30/terrain/collision_157_model.inc"
};
static NJS_OBJECT collision_157 = {
#include "stg30/terrain/collision_157.inc"
};
static NJS_MATERIAL collision_158_materials[1] = {
#include "stg30/terrain/collision_158_materials.inc"
};
static Sint16 collision_158_faces[19] = {
#include "stg30/terrain/collision_158_faces.inc"
};
static NJS_MESHSET collision_158_meshsets[1] = {
#include "stg30/terrain/collision_158_meshsets.inc"
};
static NJS_POINT3 collision_158_points[16] = {
#include "stg30/terrain/collision_158_points.inc"
};
static NJS_MODEL collision_158_model = {
#include "stg30/terrain/collision_158_model.inc"
};
static NJS_OBJECT collision_158 = {
#include "stg30/terrain/collision_158.inc"
};
static NJS_MATERIAL collision_159_materials[1] = {
#include "stg30/terrain/collision_159_materials.inc"
};
static Sint16 collision_159_faces[19] = {
#include "stg30/terrain/collision_159_faces.inc"
};
static NJS_MESHSET collision_159_meshsets[1] = {
#include "stg30/terrain/collision_159_meshsets.inc"
};
static NJS_POINT3 collision_159_points[16] = {
#include "stg30/terrain/collision_159_points.inc"
};
static NJS_MODEL collision_159_model = {
#include "stg30/terrain/collision_159_model.inc"
};
static NJS_OBJECT collision_159 = {
#include "stg30/terrain/collision_159.inc"
};
static NJS_MATERIAL collision_160_materials[1] = {
#include "stg30/terrain/collision_160_materials.inc"
};
static Sint16 collision_160_faces[42] = {
#include "stg30/terrain/collision_160_faces.inc"
};
static NJS_MESHSET collision_160_meshsets[1] = {
#include "stg30/terrain/collision_160_meshsets.inc"
};
static NJS_POINT3 collision_160_points[30] = {
#include "stg30/terrain/collision_160_points.inc"
};
static NJS_MODEL collision_160_model = {
#include "stg30/terrain/collision_160_model.inc"
};
static NJS_OBJECT collision_160 = {
#include "stg30/terrain/collision_160.inc"
};
static NJS_MATERIAL collision_163_materials[1] = {
#include "stg30/terrain/collision_163_materials.inc"
};
static Sint16 collision_163_faces[273] = {
#include "stg30/terrain/collision_163_faces.inc"
};
static NJS_MESHSET collision_163_meshsets[1] = {
#include "stg30/terrain/collision_163_meshsets.inc"
};
static NJS_POINT3 collision_163_points[140] = {
#include "stg30/terrain/collision_163_points.inc"
};
static NJS_MODEL collision_163_model = {
#include "stg30/terrain/collision_163_model.inc"
};
static NJS_OBJECT collision_163 = {
#include "stg30/terrain/collision_163.inc"
};
static NJS_MATERIAL collision_166_materials[1] = {
#include "stg30/terrain/collision_166_materials.inc"
};
static Sint16 collision_166_faces[193] = {
#include "stg30/terrain/collision_166_faces.inc"
};
static NJS_MESHSET collision_166_meshsets[1] = {
#include "stg30/terrain/collision_166_meshsets.inc"
};
static NJS_POINT3 collision_166_points[93] = {
#include "stg30/terrain/collision_166_points.inc"
};
static NJS_MODEL collision_166_model = {
#include "stg30/terrain/collision_166_model.inc"
};
static NJS_OBJECT collision_166 = {
#include "stg30/terrain/collision_166.inc"
};
static NJS_MATERIAL collision_167_materials[1] = {
#include "stg30/terrain/collision_167_materials.inc"
};
static Sint16 collision_167_faces[166] = {
#include "stg30/terrain/collision_167_faces.inc"
};
static NJS_MESHSET collision_167_meshsets[1] = {
#include "stg30/terrain/collision_167_meshsets.inc"
};
static NJS_POINT3 collision_167_points[75] = {
#include "stg30/terrain/collision_167_points.inc"
};
static NJS_MODEL collision_167_model = {
#include "stg30/terrain/collision_167_model.inc"
};
static NJS_OBJECT collision_167 = {
#include "stg30/terrain/collision_167.inc"
};
static NJS_MATERIAL collision_168_materials[1] = {
#include "stg30/terrain/collision_168_materials.inc"
};
static Sint16 collision_168_faces[22] = {
#include "stg30/terrain/collision_168_faces.inc"
};
static NJS_MESHSET collision_168_meshsets[1] = {
#include "stg30/terrain/collision_168_meshsets.inc"
};
static NJS_POINT3 collision_168_points[12] = {
#include "stg30/terrain/collision_168_points.inc"
};
static NJS_MODEL collision_168_model = {
#include "stg30/terrain/collision_168_model.inc"
};
static NJS_OBJECT collision_168 = {
#include "stg30/terrain/collision_168.inc"
};
static NJS_MATERIAL collision_169_materials[1] = {
#include "stg30/terrain/collision_169_materials.inc"
};
static Sint16 collision_169_faces[42] = {
#include "stg30/terrain/collision_169_faces.inc"
};
static NJS_MESHSET collision_169_meshsets[1] = {
#include "stg30/terrain/collision_169_meshsets.inc"
};
static NJS_POINT3 collision_169_points[30] = {
#include "stg30/terrain/collision_169_points.inc"
};
static NJS_MODEL collision_169_model = {
#include "stg30/terrain/collision_169_model.inc"
};
static NJS_OBJECT collision_169 = {
#include "stg30/terrain/collision_169.inc"
};
static NJS_MATERIAL collision_172_materials[1] = {
#include "stg30/terrain/collision_172_materials.inc"
};
static Sint16 collision_172_faces[273] = {
#include "stg30/terrain/collision_172_faces.inc"
};
static NJS_MESHSET collision_172_meshsets[1] = {
#include "stg30/terrain/collision_172_meshsets.inc"
};
static NJS_POINT3 collision_172_points[140] = {
#include "stg30/terrain/collision_172_points.inc"
};
static NJS_MODEL collision_172_model = {
#include "stg30/terrain/collision_172_model.inc"
};
static NJS_OBJECT collision_172 = {
#include "stg30/terrain/collision_172.inc"
};
static NJS_MATERIAL collision_174_materials[1] = {
#include "stg30/terrain/collision_174_materials.inc"
};
static Sint16 collision_174_faces[215] = {
#include "stg30/terrain/collision_174_faces.inc"
};
static NJS_MESHSET collision_174_meshsets[1] = {
#include "stg30/terrain/collision_174_meshsets.inc"
};
static NJS_POINT3 collision_174_points[102] = {
#include "stg30/terrain/collision_174_points.inc"
};
static NJS_MODEL collision_174_model = {
#include "stg30/terrain/collision_174_model.inc"
};
static NJS_OBJECT collision_174 = {
#include "stg30/terrain/collision_174.inc"
};
static NJS_MATERIAL collision_175_materials[1] = {
#include "stg30/terrain/collision_175_materials.inc"
};
static Sint16 collision_175_faces[139] = {
#include "stg30/terrain/collision_175_faces.inc"
};
static NJS_MESHSET collision_175_meshsets[1] = {
#include "stg30/terrain/collision_175_meshsets.inc"
};
static NJS_POINT3 collision_175_points[84] = {
#include "stg30/terrain/collision_175_points.inc"
};
static NJS_MODEL collision_175_model = {
#include "stg30/terrain/collision_175_model.inc"
};
static NJS_OBJECT collision_175 = {
#include "stg30/terrain/collision_175.inc"
};
static NJS_MATERIAL collision_176_materials[1] = {
#include "stg30/terrain/collision_176_materials.inc"
};
static Sint16 collision_176_faces[125] = {
#include "stg30/terrain/collision_176_faces.inc"
};
static NJS_MESHSET collision_176_meshsets[1] = {
#include "stg30/terrain/collision_176_meshsets.inc"
};
static NJS_POINT3 collision_176_points[61] = {
#include "stg30/terrain/collision_176_points.inc"
};
static NJS_MODEL collision_176_model = {
#include "stg30/terrain/collision_176_model.inc"
};
static NJS_OBJECT collision_176 = {
#include "stg30/terrain/collision_176.inc"
};
static NJS_MATERIAL collision_177_materials[1] = {
#include "stg30/terrain/collision_177_materials.inc"
};
static Sint16 collision_177_faces[67] = {
#include "stg30/terrain/collision_177_faces.inc"
};
static NJS_MESHSET collision_177_meshsets[1] = {
#include "stg30/terrain/collision_177_meshsets.inc"
};
static NJS_POINT3 collision_177_points[33] = {
#include "stg30/terrain/collision_177_points.inc"
};
static NJS_MODEL collision_177_model = {
#include "stg30/terrain/collision_177_model.inc"
};
static NJS_OBJECT collision_177 = {
#include "stg30/terrain/collision_177.inc"
};
static NJS_MATERIAL collision_178_materials[1] = {
#include "stg30/terrain/collision_178_materials.inc"
};
static Sint16 collision_178_faces[146] = {
#include "stg30/terrain/collision_178_faces.inc"
};
static NJS_MESHSET collision_178_meshsets[1] = {
#include "stg30/terrain/collision_178_meshsets.inc"
};
static NJS_POINT3 collision_178_points[77] = {
#include "stg30/terrain/collision_178_points.inc"
};
static NJS_MODEL collision_178_model = {
#include "stg30/terrain/collision_178_model.inc"
};
static NJS_OBJECT collision_178 = {
#include "stg30/terrain/collision_178.inc"
};
static NJS_MATERIAL collision_179_materials[1] = {
#include "stg30/terrain/collision_179_materials.inc"
};
static Sint16 collision_179_faces[64] = {
#include "stg30/terrain/collision_179_faces.inc"
};
static NJS_MESHSET collision_179_meshsets[1] = {
#include "stg30/terrain/collision_179_meshsets.inc"
};
static NJS_POINT3 collision_179_points[33] = {
#include "stg30/terrain/collision_179_points.inc"
};
static NJS_MODEL collision_179_model = {
#include "stg30/terrain/collision_179_model.inc"
};
static NJS_OBJECT collision_179 = {
#include "stg30/terrain/collision_179.inc"
};
static NJS_MATERIAL collision_180_materials[1] = {
#include "stg30/terrain/collision_180_materials.inc"
};
static Sint16 collision_180_faces[68] = {
#include "stg30/terrain/collision_180_faces.inc"
};
static NJS_MESHSET collision_180_meshsets[1] = {
#include "stg30/terrain/collision_180_meshsets.inc"
};
static NJS_POINT3 collision_180_points[42] = {
#include "stg30/terrain/collision_180_points.inc"
};
static NJS_MODEL collision_180_model = {
#include "stg30/terrain/collision_180_model.inc"
};
static NJS_OBJECT collision_180 = {
#include "stg30/terrain/collision_180.inc"
};
static NJS_MATERIAL collision_181_materials[1] = {
#include "stg30/terrain/collision_181_materials.inc"
};
static Sint16 collision_181_faces[88] = {
#include "stg30/terrain/collision_181_faces.inc"
};
static NJS_MESHSET collision_181_meshsets[1] = {
#include "stg30/terrain/collision_181_meshsets.inc"
};
static NJS_POINT3 collision_181_points[52] = {
#include "stg30/terrain/collision_181_points.inc"
};
static NJS_MODEL collision_181_model = {
#include "stg30/terrain/collision_181_model.inc"
};
static NJS_OBJECT collision_181 = {
#include "stg30/terrain/collision_181.inc"
};
static NJS_MATERIAL collision_182_materials[1] = {
#include "stg30/terrain/collision_182_materials.inc"
};
static Sint16 collision_182_faces[185] = {
#include "stg30/terrain/collision_182_faces.inc"
};
static NJS_MESHSET collision_182_meshsets[1] = {
#include "stg30/terrain/collision_182_meshsets.inc"
};
static NJS_POINT3 collision_182_points[81] = {
#include "stg30/terrain/collision_182_points.inc"
};
static NJS_MODEL collision_182_model = {
#include "stg30/terrain/collision_182_model.inc"
};
static NJS_OBJECT collision_182 = {
#include "stg30/terrain/collision_182.inc"
};
static NJS_MATERIAL collision_183_materials[1] = {
#include "stg30/terrain/collision_183_materials.inc"
};
static Sint16 collision_183_faces[145] = {
#include "stg30/terrain/collision_183_faces.inc"
};
static NJS_MESHSET collision_183_meshsets[1] = {
#include "stg30/terrain/collision_183_meshsets.inc"
};
static NJS_POINT3 collision_183_points[71] = {
#include "stg30/terrain/collision_183_points.inc"
};
static NJS_MODEL collision_183_model = {
#include "stg30/terrain/collision_183_model.inc"
};
static NJS_OBJECT collision_183 = {
#include "stg30/terrain/collision_183.inc"
};
static NJS_MATERIAL collision_184_materials[1] = {
#include "stg30/terrain/collision_184_materials.inc"
};
static Sint16 collision_184_faces[122] = {
#include "stg30/terrain/collision_184_faces.inc"
};
static NJS_MESHSET collision_184_meshsets[1] = {
#include "stg30/terrain/collision_184_meshsets.inc"
};
static NJS_POINT3 collision_184_points[59] = {
#include "stg30/terrain/collision_184_points.inc"
};
static NJS_MODEL collision_184_model = {
#include "stg30/terrain/collision_184_model.inc"
};
static NJS_OBJECT collision_184 = {
#include "stg30/terrain/collision_184.inc"
};
static NJS_MATERIAL collision_185_materials[1] = {
#include "stg30/terrain/collision_185_materials.inc"
};
static Sint16 collision_185_faces[19] = {
#include "stg30/terrain/collision_185_faces.inc"
};
static NJS_MESHSET collision_185_meshsets[1] = {
#include "stg30/terrain/collision_185_meshsets.inc"
};
static NJS_POINT3 collision_185_points[16] = {
#include "stg30/terrain/collision_185_points.inc"
};
static NJS_MODEL collision_185_model = {
#include "stg30/terrain/collision_185_model.inc"
};
static NJS_OBJECT collision_185 = {
#include "stg30/terrain/collision_185.inc"
};
static NJS_MATERIAL collision_186_materials[1] = {
#include "stg30/terrain/collision_186_materials.inc"
};
static Sint16 collision_186_faces[105] = {
#include "stg30/terrain/collision_186_faces.inc"
};
static NJS_MESHSET collision_186_meshsets[1] = {
#include "stg30/terrain/collision_186_meshsets.inc"
};
static NJS_POINT3 collision_186_points[47] = {
#include "stg30/terrain/collision_186_points.inc"
};
static NJS_MODEL collision_186_model = {
#include "stg30/terrain/collision_186_model.inc"
};
static NJS_OBJECT collision_186 = {
#include "stg30/terrain/collision_186.inc"
};
static NJS_MATERIAL collision_187_materials[1] = {
#include "stg30/terrain/collision_187_materials.inc"
};
static Sint16 collision_187_faces[221] = {
#include "stg30/terrain/collision_187_faces.inc"
};
static NJS_MESHSET collision_187_meshsets[1] = {
#include "stg30/terrain/collision_187_meshsets.inc"
};
static NJS_POINT3 collision_187_points[112] = {
#include "stg30/terrain/collision_187_points.inc"
};
static NJS_MODEL collision_187_model = {
#include "stg30/terrain/collision_187_model.inc"
};
static NJS_OBJECT collision_187 = {
#include "stg30/terrain/collision_187.inc"
};
static NJS_MATERIAL collision_188_materials[1] = {
#include "stg30/terrain/collision_188_materials.inc"
};
static Sint16 collision_188_faces[82] = {
#include "stg30/terrain/collision_188_faces.inc"
};
static NJS_MESHSET collision_188_meshsets[1] = {
#include "stg30/terrain/collision_188_meshsets.inc"
};
static NJS_POINT3 collision_188_points[45] = {
#include "stg30/terrain/collision_188_points.inc"
};
static NJS_MODEL collision_188_model = {
#include "stg30/terrain/collision_188_model.inc"
};
static NJS_OBJECT collision_188 = {
#include "stg30/terrain/collision_188.inc"
};
static NJS_MATERIAL collision_189_materials[1] = {
#include "stg30/terrain/collision_189_materials.inc"
};
static Sint16 collision_189_faces[132] = {
#include "stg30/terrain/collision_189_faces.inc"
};
static NJS_MESHSET collision_189_meshsets[1] = {
#include "stg30/terrain/collision_189_meshsets.inc"
};
static NJS_POINT3 collision_189_points[62] = {
#include "stg30/terrain/collision_189_points.inc"
};
static NJS_MODEL collision_189_model = {
#include "stg30/terrain/collision_189_model.inc"
};
static NJS_OBJECT collision_189 = {
#include "stg30/terrain/collision_189.inc"
};
static NJS_MATERIAL collision_190_materials[1] = {
#include "stg30/terrain/collision_190_materials.inc"
};
static Sint16 collision_190_faces[63] = {
#include "stg30/terrain/collision_190_faces.inc"
};
static NJS_MESHSET collision_190_meshsets[1] = {
#include "stg30/terrain/collision_190_meshsets.inc"
};
static NJS_POINT3 collision_190_points[36] = {
#include "stg30/terrain/collision_190_points.inc"
};
static NJS_MODEL collision_190_model = {
#include "stg30/terrain/collision_190_model.inc"
};
static NJS_OBJECT collision_190 = {
#include "stg30/terrain/collision_190.inc"
};
static NJS_MATERIAL collision_191_materials[1] = {
#include "stg30/terrain/collision_191_materials.inc"
};
static Sint16 collision_191_faces[181] = {
#include "stg30/terrain/collision_191_faces.inc"
};
static NJS_MESHSET collision_191_meshsets[1] = {
#include "stg30/terrain/collision_191_meshsets.inc"
};
static NJS_POINT3 collision_191_points[80] = {
#include "stg30/terrain/collision_191_points.inc"
};
static NJS_MODEL collision_191_model = {
#include "stg30/terrain/collision_191_model.inc"
};
static NJS_OBJECT collision_191 = {
#include "stg30/terrain/collision_191.inc"
};
static NJS_MATERIAL collision_192_materials[1] = {
#include "stg30/terrain/collision_192_materials.inc"
};
static Sint16 collision_192_faces[166] = {
#include "stg30/terrain/collision_192_faces.inc"
};
static NJS_MESHSET collision_192_meshsets[1] = {
#include "stg30/terrain/collision_192_meshsets.inc"
};
static NJS_POINT3 collision_192_points[75] = {
#include "stg30/terrain/collision_192_points.inc"
};
static NJS_MODEL collision_192_model = {
#include "stg30/terrain/collision_192_model.inc"
};
static NJS_OBJECT collision_192 = {
#include "stg30/terrain/collision_192.inc"
};
static NJS_MATERIAL collision_193_materials[1] = {
#include "stg30/terrain/collision_193_materials.inc"
};
static Sint16 collision_193_faces[11] = {
#include "stg30/terrain/collision_193_faces.inc"
};
static NJS_MESHSET collision_193_meshsets[1] = {
#include "stg30/terrain/collision_193_meshsets.inc"
};
static NJS_POINT3 collision_193_points[8] = {
#include "stg30/terrain/collision_193_points.inc"
};
static NJS_MODEL collision_193_model = {
#include "stg30/terrain/collision_193_model.inc"
};
static NJS_OBJECT collision_193 = {
#include "stg30/terrain/collision_193.inc"
};
static NJS_MATERIAL collision_194_materials[1] = {
#include "stg30/terrain/collision_194_materials.inc"
};
static Sint16 collision_194_faces[65] = {
#include "stg30/terrain/collision_194_faces.inc"
};
static NJS_MESHSET collision_194_meshsets[1] = {
#include "stg30/terrain/collision_194_meshsets.inc"
};
static NJS_POINT3 collision_194_points[33] = {
#include "stg30/terrain/collision_194_points.inc"
};
static NJS_MODEL collision_194_model = {
#include "stg30/terrain/collision_194_model.inc"
};
static NJS_OBJECT collision_194 = {
#include "stg30/terrain/collision_194.inc"
};
static NJS_MATERIAL collision_195_materials[1] = {
#include "stg30/terrain/collision_195_materials.inc"
};
static Sint16 collision_195_faces[288] = {
#include "stg30/terrain/collision_195_faces.inc"
};
static NJS_MESHSET collision_195_meshsets[1] = {
#include "stg30/terrain/collision_195_meshsets.inc"
};
static NJS_POINT3 collision_195_points[141] = {
#include "stg30/terrain/collision_195_points.inc"
};
static NJS_MODEL collision_195_model = {
#include "stg30/terrain/collision_195_model.inc"
};
static NJS_OBJECT collision_195 = {
#include "stg30/terrain/collision_195.inc"
};
static NJS_MATERIAL collision_196_materials[1] = {
#include "stg30/terrain/collision_196_materials.inc"
};
static Sint16 collision_196_faces[67] = {
#include "stg30/terrain/collision_196_faces.inc"
};
static NJS_MESHSET collision_196_meshsets[1] = {
#include "stg30/terrain/collision_196_meshsets.inc"
};
static NJS_POINT3 collision_196_points[41] = {
#include "stg30/terrain/collision_196_points.inc"
};
static NJS_MODEL collision_196_model = {
#include "stg30/terrain/collision_196_model.inc"
};
static NJS_OBJECT collision_196 = {
#include "stg30/terrain/collision_196.inc"
};
static NJS_MATERIAL collision_197_materials[1] = {
#include "stg30/terrain/collision_197_materials.inc"
};
static Sint16 collision_197_faces[90] = {
#include "stg30/terrain/collision_197_faces.inc"
};
static NJS_MESHSET collision_197_meshsets[1] = {
#include "stg30/terrain/collision_197_meshsets.inc"
};
static NJS_POINT3 collision_197_points[48] = {
#include "stg30/terrain/collision_197_points.inc"
};
static NJS_MODEL collision_197_model = {
#include "stg30/terrain/collision_197_model.inc"
};
static NJS_OBJECT collision_197 = {
#include "stg30/terrain/collision_197.inc"
};
static NJS_MATERIAL collision_198_materials[1] = {
#include "stg30/terrain/collision_198_materials.inc"
};
static Sint16 collision_198_faces[154] = {
#include "stg30/terrain/collision_198_faces.inc"
};
static NJS_MESHSET collision_198_meshsets[1] = {
#include "stg30/terrain/collision_198_meshsets.inc"
};
static NJS_POINT3 collision_198_points[68] = {
#include "stg30/terrain/collision_198_points.inc"
};
static NJS_MODEL collision_198_model = {
#include "stg30/terrain/collision_198_model.inc"
};
static NJS_OBJECT collision_198 = {
#include "stg30/terrain/collision_198.inc"
};
static NJS_MATERIAL collision_199_materials[1] = {
#include "stg30/terrain/collision_199_materials.inc"
};
static Sint16 collision_199_faces[188] = {
#include "stg30/terrain/collision_199_faces.inc"
};
static NJS_MESHSET collision_199_meshsets[1] = {
#include "stg30/terrain/collision_199_meshsets.inc"
};
static NJS_POINT3 collision_199_points[83] = {
#include "stg30/terrain/collision_199_points.inc"
};
static NJS_MODEL collision_199_model = {
#include "stg30/terrain/collision_199_model.inc"
};
static NJS_OBJECT collision_199 = {
#include "stg30/terrain/collision_199.inc"
};
static NJS_MATERIAL collision_200_materials[1] = {
#include "stg30/terrain/collision_200_materials.inc"
};
static Sint16 collision_200_faces[72] = {
#include "stg30/terrain/collision_200_faces.inc"
};
static NJS_MESHSET collision_200_meshsets[1] = {
#include "stg30/terrain/collision_200_meshsets.inc"
};
static NJS_POINT3 collision_200_points[40] = {
#include "stg30/terrain/collision_200_points.inc"
};
static NJS_MODEL collision_200_model = {
#include "stg30/terrain/collision_200_model.inc"
};
static NJS_OBJECT collision_200 = {
#include "stg30/terrain/collision_200.inc"
};
static NJS_MATERIAL collision_201_materials[1] = {
#include "stg30/terrain/collision_201_materials.inc"
};
static Sint16 collision_201_faces[107] = {
#include "stg30/terrain/collision_201_faces.inc"
};
static NJS_MESHSET collision_201_meshsets[1] = {
#include "stg30/terrain/collision_201_meshsets.inc"
};
static NJS_POINT3 collision_201_points[58] = {
#include "stg30/terrain/collision_201_points.inc"
};
static NJS_MODEL collision_201_model = {
#include "stg30/terrain/collision_201_model.inc"
};
static NJS_OBJECT collision_201 = {
#include "stg30/terrain/collision_201.inc"
};
static NJS_MATERIAL collision_202_materials[1] = {
#include "stg30/terrain/collision_202_materials.inc"
};
static Sint16 collision_202_faces[64] = {
#include "stg30/terrain/collision_202_faces.inc"
};
static NJS_MESHSET collision_202_meshsets[1] = {
#include "stg30/terrain/collision_202_meshsets.inc"
};
static NJS_POINT3 collision_202_points[33] = {
#include "stg30/terrain/collision_202_points.inc"
};
static NJS_MODEL collision_202_model = {
#include "stg30/terrain/collision_202_model.inc"
};
static NJS_OBJECT collision_202 = {
#include "stg30/terrain/collision_202.inc"
};
static NJS_MATERIAL collision_203_materials[1] = {
#include "stg30/terrain/collision_203_materials.inc"
};
static Sint16 collision_203_faces[72] = {
#include "stg30/terrain/collision_203_faces.inc"
};
static NJS_MESHSET collision_203_meshsets[1] = {
#include "stg30/terrain/collision_203_meshsets.inc"
};
static NJS_POINT3 collision_203_points[40] = {
#include "stg30/terrain/collision_203_points.inc"
};
static NJS_MODEL collision_203_model = {
#include "stg30/terrain/collision_203_model.inc"
};
static NJS_OBJECT collision_203 = {
#include "stg30/terrain/collision_203.inc"
};
static NJS_MATERIAL collision_204_materials[1] = {
#include "stg30/terrain/collision_204_materials.inc"
};
static Sint16 collision_204_faces[44] = {
#include "stg30/terrain/collision_204_faces.inc"
};
static NJS_MESHSET collision_204_meshsets[1] = {
#include "stg30/terrain/collision_204_meshsets.inc"
};
static NJS_POINT3 collision_204_points[25] = {
#include "stg30/terrain/collision_204_points.inc"
};
static NJS_MODEL collision_204_model = {
#include "stg30/terrain/collision_204_model.inc"
};
static NJS_OBJECT collision_204 = {
#include "stg30/terrain/collision_204.inc"
};
static NJS_MATERIAL collision_205_materials[1] = {
#include "stg30/terrain/collision_205_materials.inc"
};
static Sint16 collision_205_faces[135] = {
#include "stg30/terrain/collision_205_faces.inc"
};
static NJS_MESHSET collision_205_meshsets[1] = {
#include "stg30/terrain/collision_205_meshsets.inc"
};
static NJS_POINT3 collision_205_points[59] = {
#include "stg30/terrain/collision_205_points.inc"
};
static NJS_MODEL collision_205_model = {
#include "stg30/terrain/collision_205_model.inc"
};
static NJS_OBJECT collision_205 = {
#include "stg30/terrain/collision_205.inc"
};
static NJS_MATERIAL collision_206_materials[1] = {
#include "stg30/terrain/collision_206_materials.inc"
};
static Sint16 collision_206_faces[155] = {
#include "stg30/terrain/collision_206_faces.inc"
};
static NJS_MESHSET collision_206_meshsets[1] = {
#include "stg30/terrain/collision_206_meshsets.inc"
};
static NJS_POINT3 collision_206_points[69] = {
#include "stg30/terrain/collision_206_points.inc"
};
static NJS_MODEL collision_206_model = {
#include "stg30/terrain/collision_206_model.inc"
};
static NJS_OBJECT collision_206 = {
#include "stg30/terrain/collision_206.inc"
};
static NJS_MATERIAL collision_207_materials[1] = {
#include "stg30/terrain/collision_207_materials.inc"
};
static Sint16 collision_207_faces[71] = {
#include "stg30/terrain/collision_207_faces.inc"
};
static NJS_MESHSET collision_207_meshsets[1] = {
#include "stg30/terrain/collision_207_meshsets.inc"
};
static NJS_POINT3 collision_207_points[34] = {
#include "stg30/terrain/collision_207_points.inc"
};
static NJS_MODEL collision_207_model = {
#include "stg30/terrain/collision_207_model.inc"
};
static NJS_OBJECT collision_207 = {
#include "stg30/terrain/collision_207.inc"
};
static NJS_MATERIAL collision_208_materials[1] = {
#include "stg30/terrain/collision_208_materials.inc"
};
static Sint16 collision_208_faces[64] = {
#include "stg30/terrain/collision_208_faces.inc"
};
static NJS_MESHSET collision_208_meshsets[1] = {
#include "stg30/terrain/collision_208_meshsets.inc"
};
static NJS_POINT3 collision_208_points[33] = {
#include "stg30/terrain/collision_208_points.inc"
};
static NJS_MODEL collision_208_model = {
#include "stg30/terrain/collision_208_model.inc"
};
static NJS_OBJECT collision_208 = {
#include "stg30/terrain/collision_208.inc"
};
static NJS_MATERIAL collision_209_materials[1] = {
#include "stg30/terrain/collision_209_materials.inc"
};
static Sint16 collision_209_faces[138] = {
#include "stg30/terrain/collision_209_faces.inc"
};
static NJS_MESHSET collision_209_meshsets[1] = {
#include "stg30/terrain/collision_209_meshsets.inc"
};
static NJS_POINT3 collision_209_points[77] = {
#include "stg30/terrain/collision_209_points.inc"
};
static NJS_MODEL collision_209_model = {
#include "stg30/terrain/collision_209_model.inc"
};
static NJS_OBJECT collision_209 = {
#include "stg30/terrain/collision_209.inc"
};
static NJS_MATERIAL collision_210_materials[1] = {
#include "stg30/terrain/collision_210_materials.inc"
};
static Sint16 collision_210_faces[137] = {
#include "stg30/terrain/collision_210_faces.inc"
};
static NJS_MESHSET collision_210_meshsets[1] = {
#include "stg30/terrain/collision_210_meshsets.inc"
};
static NJS_POINT3 collision_210_points[65] = {
#include "stg30/terrain/collision_210_points.inc"
};
static NJS_MODEL collision_210_model = {
#include "stg30/terrain/collision_210_model.inc"
};
static NJS_OBJECT collision_210 = {
#include "stg30/terrain/collision_210.inc"
};
static NJS_MATERIAL collision_211_materials[1] = {
#include "stg30/terrain/collision_211_materials.inc"
};
static Sint16 collision_211_faces[154] = {
#include "stg30/terrain/collision_211_faces.inc"
};
static NJS_MESHSET collision_211_meshsets[1] = {
#include "stg30/terrain/collision_211_meshsets.inc"
};
static NJS_POINT3 collision_211_points[68] = {
#include "stg30/terrain/collision_211_points.inc"
};
static NJS_MODEL collision_211_model = {
#include "stg30/terrain/collision_211_model.inc"
};
static NJS_OBJECT collision_211 = {
#include "stg30/terrain/collision_211.inc"
};
static NJS_MATERIAL collision_212_materials[1] = {
#include "stg30/terrain/collision_212_materials.inc"
};
static Sint16 collision_212_faces[45] = {
#include "stg30/terrain/collision_212_faces.inc"
};
static NJS_MESHSET collision_212_meshsets[1] = {
#include "stg30/terrain/collision_212_meshsets.inc"
};
static NJS_POINT3 collision_212_points[28] = {
#include "stg30/terrain/collision_212_points.inc"
};
static NJS_MODEL collision_212_model = {
#include "stg30/terrain/collision_212_model.inc"
};
static NJS_OBJECT collision_212 = {
#include "stg30/terrain/collision_212.inc"
};
static NJS_MATERIAL collision_213_materials[1] = {
#include "stg30/terrain/collision_213_materials.inc"
};
static Sint16 collision_213_faces[45] = {
#include "stg30/terrain/collision_213_faces.inc"
};
static NJS_MESHSET collision_213_meshsets[1] = {
#include "stg30/terrain/collision_213_meshsets.inc"
};
static NJS_POINT3 collision_213_points[28] = {
#include "stg30/terrain/collision_213_points.inc"
};
static NJS_MODEL collision_213_model = {
#include "stg30/terrain/collision_213_model.inc"
};
static NJS_OBJECT collision_213 = {
#include "stg30/terrain/collision_213.inc"
};
static NJS_MATERIAL collision_214_materials[1] = {
#include "stg30/terrain/collision_214_materials.inc"
};
static Sint16 collision_214_faces[92] = {
#include "stg30/terrain/collision_214_faces.inc"
};
static NJS_MESHSET collision_214_meshsets[1] = {
#include "stg30/terrain/collision_214_meshsets.inc"
};
static NJS_POINT3 collision_214_points[55] = {
#include "stg30/terrain/collision_214_points.inc"
};
static NJS_MODEL collision_214_model = {
#include "stg30/terrain/collision_214_model.inc"
};
static NJS_OBJECT collision_214 = {
#include "stg30/terrain/collision_214.inc"
};
static NJS_MATERIAL collision_215_materials[1] = {
#include "stg30/terrain/collision_215_materials.inc"
};
static Sint16 collision_215_faces[92] = {
#include "stg30/terrain/collision_215_faces.inc"
};
static NJS_MESHSET collision_215_meshsets[1] = {
#include "stg30/terrain/collision_215_meshsets.inc"
};
static NJS_POINT3 collision_215_points[55] = {
#include "stg30/terrain/collision_215_points.inc"
};
static NJS_MODEL collision_215_model = {
#include "stg30/terrain/collision_215_model.inc"
};
static NJS_OBJECT collision_215 = {
#include "stg30/terrain/collision_215.inc"
};
static NJS_MATERIAL collision_216_materials[1] = {
#include "stg30/terrain/collision_216_materials.inc"
};
static Sint16 collision_216_faces[145] = {
#include "stg30/terrain/collision_216_faces.inc"
};
static NJS_MESHSET collision_216_meshsets[1] = {
#include "stg30/terrain/collision_216_meshsets.inc"
};
static NJS_POINT3 collision_216_points[66] = {
#include "stg30/terrain/collision_216_points.inc"
};
static NJS_MODEL collision_216_model = {
#include "stg30/terrain/collision_216_model.inc"
};
static NJS_OBJECT collision_216 = {
#include "stg30/terrain/collision_216.inc"
};
static NJS_MATERIAL collision_217_materials[1] = {
#include "stg30/terrain/collision_217_materials.inc"
};
static Sint16 collision_217_faces[67] = {
#include "stg30/terrain/collision_217_faces.inc"
};
static NJS_MESHSET collision_217_meshsets[1] = {
#include "stg30/terrain/collision_217_meshsets.inc"
};
static NJS_POINT3 collision_217_points[33] = {
#include "stg30/terrain/collision_217_points.inc"
};
static NJS_MODEL collision_217_model = {
#include "stg30/terrain/collision_217_model.inc"
};
static NJS_OBJECT collision_217 = {
#include "stg30/terrain/collision_217.inc"
};
static NJS_MATERIAL collision_218_materials[1] = {
#include "stg30/terrain/collision_218_materials.inc"
};
static Sint16 collision_218_faces[63] = {
#include "stg30/terrain/collision_218_faces.inc"
};
static NJS_MESHSET collision_218_meshsets[1] = {
#include "stg30/terrain/collision_218_meshsets.inc"
};
static NJS_POINT3 collision_218_points[36] = {
#include "stg30/terrain/collision_218_points.inc"
};
static NJS_MODEL collision_218_model = {
#include "stg30/terrain/collision_218_model.inc"
};
static NJS_OBJECT collision_218 = {
#include "stg30/terrain/collision_218.inc"
};
static NJS_MATERIAL collision_219_materials[1] = {
#include "stg30/terrain/collision_219_materials.inc"
};
static Sint16 collision_219_faces[63] = {
#include "stg30/terrain/collision_219_faces.inc"
};
static NJS_MESHSET collision_219_meshsets[1] = {
#include "stg30/terrain/collision_219_meshsets.inc"
};
static NJS_POINT3 collision_219_points[36] = {
#include "stg30/terrain/collision_219_points.inc"
};
static NJS_MODEL collision_219_model = {
#include "stg30/terrain/collision_219_model.inc"
};
static NJS_OBJECT collision_219 = {
#include "stg30/terrain/collision_219.inc"
};
static NJS_MATERIAL collision_220_materials[1] = {
#include "stg30/terrain/collision_220_materials.inc"
};
static Sint16 collision_220_faces[92] = {
#include "stg30/terrain/collision_220_faces.inc"
};
static NJS_MESHSET collision_220_meshsets[1] = {
#include "stg30/terrain/collision_220_meshsets.inc"
};
static NJS_POINT3 collision_220_points[55] = {
#include "stg30/terrain/collision_220_points.inc"
};
static NJS_MODEL collision_220_model = {
#include "stg30/terrain/collision_220_model.inc"
};
static NJS_OBJECT collision_220 = {
#include "stg30/terrain/collision_220.inc"
};
static NJS_MATERIAL collision_221_materials[1] = {
#include "stg30/terrain/collision_221_materials.inc"
};
static Sint16 collision_221_faces[188] = {
#include "stg30/terrain/collision_221_faces.inc"
};
static NJS_MESHSET collision_221_meshsets[1] = {
#include "stg30/terrain/collision_221_meshsets.inc"
};
static NJS_POINT3 collision_221_points[81] = {
#include "stg30/terrain/collision_221_points.inc"
};
static NJS_MODEL collision_221_model = {
#include "stg30/terrain/collision_221_model.inc"
};
static NJS_OBJECT collision_221 = {
#include "stg30/terrain/collision_221.inc"
};
static NJS_MATERIAL collision_222_materials[1] = {
#include "stg30/terrain/collision_222_materials.inc"
};
static Sint16 collision_222_faces[82] = {
#include "stg30/terrain/collision_222_faces.inc"
};
static NJS_MESHSET collision_222_meshsets[1] = {
#include "stg30/terrain/collision_222_meshsets.inc"
};
static NJS_POINT3 collision_222_points[41] = {
#include "stg30/terrain/collision_222_points.inc"
};
static NJS_MODEL collision_222_model = {
#include "stg30/terrain/collision_222_model.inc"
};
static NJS_OBJECT collision_222 = {
#include "stg30/terrain/collision_222.inc"
};
static NJS_MATERIAL collision_223_materials[1] = {
#include "stg30/terrain/collision_223_materials.inc"
};
static Sint16 collision_223_faces[145] = {
#include "stg30/terrain/collision_223_faces.inc"
};
static NJS_MESHSET collision_223_meshsets[1] = {
#include "stg30/terrain/collision_223_meshsets.inc"
};
static NJS_POINT3 collision_223_points[68] = {
#include "stg30/terrain/collision_223_points.inc"
};
static NJS_MODEL collision_223_model = {
#include "stg30/terrain/collision_223_model.inc"
};
static NJS_OBJECT collision_223 = {
#include "stg30/terrain/collision_223.inc"
};
static NJS_MATERIAL collision_224_materials[1] = {
#include "stg30/terrain/collision_224_materials.inc"
};
static Sint16 collision_224_faces[43] = {
#include "stg30/terrain/collision_224_faces.inc"
};
static NJS_MESHSET collision_224_meshsets[1] = {
#include "stg30/terrain/collision_224_meshsets.inc"
};
static NJS_POINT3 collision_224_points[22] = {
#include "stg30/terrain/collision_224_points.inc"
};
static NJS_MODEL collision_224_model = {
#include "stg30/terrain/collision_224_model.inc"
};
static NJS_OBJECT collision_224 = {
#include "stg30/terrain/collision_224.inc"
};
static NJS_MATERIAL collision_225_materials[1] = {
#include "stg30/terrain/collision_225_materials.inc"
};
static Sint16 collision_225_faces[67] = {
#include "stg30/terrain/collision_225_faces.inc"
};
static NJS_MESHSET collision_225_meshsets[1] = {
#include "stg30/terrain/collision_225_meshsets.inc"
};
static NJS_POINT3 collision_225_points[33] = {
#include "stg30/terrain/collision_225_points.inc"
};
static NJS_MODEL collision_225_model = {
#include "stg30/terrain/collision_225_model.inc"
};
static NJS_OBJECT collision_225 = {
#include "stg30/terrain/collision_225.inc"
};
static NJS_MATERIAL collision_226_materials[1] = {
#include "stg30/terrain/collision_226_materials.inc"
};
static Sint16 collision_226_faces[64] = {
#include "stg30/terrain/collision_226_faces.inc"
};
static NJS_MESHSET collision_226_meshsets[1] = {
#include "stg30/terrain/collision_226_meshsets.inc"
};
static NJS_POINT3 collision_226_points[33] = {
#include "stg30/terrain/collision_226_points.inc"
};
static NJS_MODEL collision_226_model = {
#include "stg30/terrain/collision_226_model.inc"
};
static NJS_OBJECT collision_226 = {
#include "stg30/terrain/collision_226.inc"
};
static NJS_MATERIAL collision_227_materials[1] = {
#include "stg30/terrain/collision_227_materials.inc"
};
static Sint16 collision_227_faces[63] = {
#include "stg30/terrain/collision_227_faces.inc"
};
static NJS_MESHSET collision_227_meshsets[1] = {
#include "stg30/terrain/collision_227_meshsets.inc"
};
static NJS_POINT3 collision_227_points[36] = {
#include "stg30/terrain/collision_227_points.inc"
};
static NJS_MODEL collision_227_model = {
#include "stg30/terrain/collision_227_model.inc"
};
static NJS_OBJECT collision_227 = {
#include "stg30/terrain/collision_227.inc"
};
static NJS_MATERIAL collision_228_materials[1] = {
#include "stg30/terrain/collision_228_materials.inc"
};
static Sint16 collision_228_faces[20] = {
#include "stg30/terrain/collision_228_faces.inc"
};
static NJS_MESHSET collision_228_meshsets[1] = {
#include "stg30/terrain/collision_228_meshsets.inc"
};
static NJS_POINT3 collision_228_points[18] = {
#include "stg30/terrain/collision_228_points.inc"
};
static NJS_MODEL collision_228_model = {
#include "stg30/terrain/collision_228_model.inc"
};
static NJS_OBJECT collision_228 = {
#include "stg30/terrain/collision_228.inc"
};
static NJS_MATERIAL collision_229_materials[1] = {
#include "stg30/terrain/collision_229_materials.inc"
};
static Sint16 collision_229_faces[14] = {
#include "stg30/terrain/collision_229_faces.inc"
};
static NJS_MESHSET collision_229_meshsets[1] = {
#include "stg30/terrain/collision_229_meshsets.inc"
};
static NJS_POINT3 collision_229_points[12] = {
#include "stg30/terrain/collision_229_points.inc"
};
static NJS_MODEL collision_229_model = {
#include "stg30/terrain/collision_229_model.inc"
};
static NJS_OBJECT collision_229 = {
#include "stg30/terrain/collision_229.inc"
};
static NJS_MATERIAL collision_230_materials[1] = {
#include "stg30/terrain/collision_230_materials.inc"
};
static Sint16 collision_230_faces[14] = {
#include "stg30/terrain/collision_230_faces.inc"
};
static NJS_MESHSET collision_230_meshsets[1] = {
#include "stg30/terrain/collision_230_meshsets.inc"
};
static NJS_POINT3 collision_230_points[12] = {
#include "stg30/terrain/collision_230_points.inc"
};
static NJS_MODEL collision_230_model = {
#include "stg30/terrain/collision_230_model.inc"
};
static NJS_OBJECT collision_230 = {
#include "stg30/terrain/collision_230.inc"
};
static NJS_MATERIAL collision_231_materials[1] = {
#include "stg30/terrain/collision_231_materials.inc"
};
static Sint16 collision_231_faces[14] = {
#include "stg30/terrain/collision_231_faces.inc"
};
static NJS_MESHSET collision_231_meshsets[1] = {
#include "stg30/terrain/collision_231_meshsets.inc"
};
static NJS_POINT3 collision_231_points[12] = {
#include "stg30/terrain/collision_231_points.inc"
};
static NJS_MODEL collision_231_model = {
#include "stg30/terrain/collision_231_model.inc"
};
static NJS_OBJECT collision_231 = {
#include "stg30/terrain/collision_231.inc"
};
static NJS_MATERIAL collision_232_materials[1] = {
#include "stg30/terrain/collision_232_materials.inc"
};
static Sint16 collision_232_faces[14] = {
#include "stg30/terrain/collision_232_faces.inc"
};
static NJS_MESHSET collision_232_meshsets[1] = {
#include "stg30/terrain/collision_232_meshsets.inc"
};
static NJS_POINT3 collision_232_points[12] = {
#include "stg30/terrain/collision_232_points.inc"
};
static NJS_MODEL collision_232_model = {
#include "stg30/terrain/collision_232_model.inc"
};
static NJS_OBJECT collision_232 = {
#include "stg30/terrain/collision_232.inc"
};
static NJS_MATERIAL collision_233_materials[1] = {
#include "stg30/terrain/collision_233_materials.inc"
};
static Sint16 collision_233_faces[14] = {
#include "stg30/terrain/collision_233_faces.inc"
};
static NJS_MESHSET collision_233_meshsets[1] = {
#include "stg30/terrain/collision_233_meshsets.inc"
};
static NJS_POINT3 collision_233_points[12] = {
#include "stg30/terrain/collision_233_points.inc"
};
static NJS_MODEL collision_233_model = {
#include "stg30/terrain/collision_233_model.inc"
};
static NJS_OBJECT collision_233 = {
#include "stg30/terrain/collision_233.inc"
};
static NJS_MATERIAL collision_234_materials[1] = {
#include "stg30/terrain/collision_234_materials.inc"
};
static Sint16 collision_234_faces[14] = {
#include "stg30/terrain/collision_234_faces.inc"
};
static NJS_MESHSET collision_234_meshsets[1] = {
#include "stg30/terrain/collision_234_meshsets.inc"
};
static NJS_POINT3 collision_234_points[12] = {
#include "stg30/terrain/collision_234_points.inc"
};
static NJS_MODEL collision_234_model = {
#include "stg30/terrain/collision_234_model.inc"
};
static NJS_OBJECT collision_234 = {
#include "stg30/terrain/collision_234.inc"
};
static NJS_MATERIAL collision_235_materials[1] = {
#include "stg30/terrain/collision_235_materials.inc"
};
static Sint16 collision_235_faces[14] = {
#include "stg30/terrain/collision_235_faces.inc"
};
static NJS_MESHSET collision_235_meshsets[1] = {
#include "stg30/terrain/collision_235_meshsets.inc"
};
static NJS_POINT3 collision_235_points[12] = {
#include "stg30/terrain/collision_235_points.inc"
};
static NJS_MODEL collision_235_model = {
#include "stg30/terrain/collision_235_model.inc"
};
static NJS_OBJECT collision_235 = {
#include "stg30/terrain/collision_235.inc"
};
static NJS_MATERIAL collision_236_materials[1] = {
#include "stg30/terrain/collision_236_materials.inc"
};
static Sint16 collision_236_faces[14] = {
#include "stg30/terrain/collision_236_faces.inc"
};
static NJS_MESHSET collision_236_meshsets[1] = {
#include "stg30/terrain/collision_236_meshsets.inc"
};
static NJS_POINT3 collision_236_points[12] = {
#include "stg30/terrain/collision_236_points.inc"
};
static NJS_MODEL collision_236_model = {
#include "stg30/terrain/collision_236_model.inc"
};
static NJS_OBJECT collision_236 = {
#include "stg30/terrain/collision_236.inc"
};
static NJS_MATERIAL collision_237_materials[1] = {
#include "stg30/terrain/collision_237_materials.inc"
};
static Sint16 collision_237_faces[14] = {
#include "stg30/terrain/collision_237_faces.inc"
};
static NJS_MESHSET collision_237_meshsets[1] = {
#include "stg30/terrain/collision_237_meshsets.inc"
};
static NJS_POINT3 collision_237_points[12] = {
#include "stg30/terrain/collision_237_points.inc"
};
static NJS_MODEL collision_237_model = {
#include "stg30/terrain/collision_237_model.inc"
};
static NJS_OBJECT collision_237 = {
#include "stg30/terrain/collision_237.inc"
};
static NJS_MATERIAL collision_238_materials[1] = {
#include "stg30/terrain/collision_238_materials.inc"
};
static Sint16 collision_238_faces[14] = {
#include "stg30/terrain/collision_238_faces.inc"
};
static NJS_MESHSET collision_238_meshsets[1] = {
#include "stg30/terrain/collision_238_meshsets.inc"
};
static NJS_POINT3 collision_238_points[12] = {
#include "stg30/terrain/collision_238_points.inc"
};
static NJS_MODEL collision_238_model = {
#include "stg30/terrain/collision_238_model.inc"
};
static NJS_OBJECT collision_238 = {
#include "stg30/terrain/collision_238.inc"
};
static NJS_MATERIAL collision_239_materials[1] = {
#include "stg30/terrain/collision_239_materials.inc"
};
static Sint16 collision_239_faces[14] = {
#include "stg30/terrain/collision_239_faces.inc"
};
static NJS_MESHSET collision_239_meshsets[1] = {
#include "stg30/terrain/collision_239_meshsets.inc"
};
static NJS_POINT3 collision_239_points[12] = {
#include "stg30/terrain/collision_239_points.inc"
};
static NJS_MODEL collision_239_model = {
#include "stg30/terrain/collision_239_model.inc"
};
static NJS_OBJECT collision_239 = {
#include "stg30/terrain/collision_239.inc"
};
static NJS_MATERIAL collision_240_materials[1] = {
#include "stg30/terrain/collision_240_materials.inc"
};
static Sint16 collision_240_faces[14] = {
#include "stg30/terrain/collision_240_faces.inc"
};
static NJS_MESHSET collision_240_meshsets[1] = {
#include "stg30/terrain/collision_240_meshsets.inc"
};
static NJS_POINT3 collision_240_points[12] = {
#include "stg30/terrain/collision_240_points.inc"
};
static NJS_MODEL collision_240_model = {
#include "stg30/terrain/collision_240_model.inc"
};
static NJS_OBJECT collision_240 = {
#include "stg30/terrain/collision_240.inc"
};
static NJS_MATERIAL collision_241_materials[1] = {
#include "stg30/terrain/collision_241_materials.inc"
};
static Sint16 collision_241_faces[14] = {
#include "stg30/terrain/collision_241_faces.inc"
};
static NJS_MESHSET collision_241_meshsets[1] = {
#include "stg30/terrain/collision_241_meshsets.inc"
};
static NJS_POINT3 collision_241_points[12] = {
#include "stg30/terrain/collision_241_points.inc"
};
static NJS_MODEL collision_241_model = {
#include "stg30/terrain/collision_241_model.inc"
};
static NJS_OBJECT collision_241 = {
#include "stg30/terrain/collision_241.inc"
};
static NJS_MATERIAL collision_242_materials[1] = {
#include "stg30/terrain/collision_242_materials.inc"
};
static Sint16 collision_242_faces[14] = {
#include "stg30/terrain/collision_242_faces.inc"
};
static NJS_MESHSET collision_242_meshsets[1] = {
#include "stg30/terrain/collision_242_meshsets.inc"
};
static NJS_POINT3 collision_242_points[12] = {
#include "stg30/terrain/collision_242_points.inc"
};
static NJS_MODEL collision_242_model = {
#include "stg30/terrain/collision_242_model.inc"
};
static NJS_OBJECT collision_242 = {
#include "stg30/terrain/collision_242.inc"
};
static NJS_MATERIAL collision_243_materials[1] = {
#include "stg30/terrain/collision_243_materials.inc"
};
static Sint16 collision_243_faces[14] = {
#include "stg30/terrain/collision_243_faces.inc"
};
static NJS_MESHSET collision_243_meshsets[1] = {
#include "stg30/terrain/collision_243_meshsets.inc"
};
static NJS_POINT3 collision_243_points[12] = {
#include "stg30/terrain/collision_243_points.inc"
};
static NJS_MODEL collision_243_model = {
#include "stg30/terrain/collision_243_model.inc"
};
static NJS_OBJECT collision_243 = {
#include "stg30/terrain/collision_243.inc"
};
static NJS_MATERIAL collision_244_materials[1] = {
#include "stg30/terrain/collision_244_materials.inc"
};
static Sint16 collision_244_faces[14] = {
#include "stg30/terrain/collision_244_faces.inc"
};
static NJS_MESHSET collision_244_meshsets[1] = {
#include "stg30/terrain/collision_244_meshsets.inc"
};
static NJS_POINT3 collision_244_points[12] = {
#include "stg30/terrain/collision_244_points.inc"
};
static NJS_MODEL collision_244_model = {
#include "stg30/terrain/collision_244_model.inc"
};
static NJS_OBJECT collision_244 = {
#include "stg30/terrain/collision_244.inc"
};
static NJS_MATERIAL collision_245_materials[1] = {
#include "stg30/terrain/collision_245_materials.inc"
};
static Sint16 collision_245_faces[14] = {
#include "stg30/terrain/collision_245_faces.inc"
};
static NJS_MESHSET collision_245_meshsets[1] = {
#include "stg30/terrain/collision_245_meshsets.inc"
};
static NJS_POINT3 collision_245_points[12] = {
#include "stg30/terrain/collision_245_points.inc"
};
static NJS_MODEL collision_245_model = {
#include "stg30/terrain/collision_245_model.inc"
};
static NJS_OBJECT collision_245 = {
#include "stg30/terrain/collision_245.inc"
};
static NJS_MATERIAL collision_246_materials[1] = {
#include "stg30/terrain/collision_246_materials.inc"
};
static Sint16 collision_246_faces[14] = {
#include "stg30/terrain/collision_246_faces.inc"
};
static NJS_MESHSET collision_246_meshsets[1] = {
#include "stg30/terrain/collision_246_meshsets.inc"
};
static NJS_POINT3 collision_246_points[12] = {
#include "stg30/terrain/collision_246_points.inc"
};
static NJS_MODEL collision_246_model = {
#include "stg30/terrain/collision_246_model.inc"
};
static NJS_OBJECT collision_246 = {
#include "stg30/terrain/collision_246.inc"
};
static NJS_MATERIAL collision_247_materials[1] = {
#include "stg30/terrain/collision_247_materials.inc"
};
static Sint16 collision_247_faces[14] = {
#include "stg30/terrain/collision_247_faces.inc"
};
static NJS_MESHSET collision_247_meshsets[1] = {
#include "stg30/terrain/collision_247_meshsets.inc"
};
static NJS_POINT3 collision_247_points[12] = {
#include "stg30/terrain/collision_247_points.inc"
};
static NJS_MODEL collision_247_model = {
#include "stg30/terrain/collision_247_model.inc"
};
static NJS_OBJECT collision_247 = {
#include "stg30/terrain/collision_247.inc"
};
static NJS_MATERIAL collision_248_materials[1] = {
#include "stg30/terrain/collision_248_materials.inc"
};
static Sint16 collision_248_faces[14] = {
#include "stg30/terrain/collision_248_faces.inc"
};
static NJS_MESHSET collision_248_meshsets[1] = {
#include "stg30/terrain/collision_248_meshsets.inc"
};
static NJS_POINT3 collision_248_points[12] = {
#include "stg30/terrain/collision_248_points.inc"
};
static NJS_MODEL collision_248_model = {
#include "stg30/terrain/collision_248_model.inc"
};
static NJS_OBJECT collision_248 = {
#include "stg30/terrain/collision_248.inc"
};
static NJS_MATERIAL collision_249_materials[1] = {
#include "stg30/terrain/collision_249_materials.inc"
};
static Sint16 collision_249_faces[14] = {
#include "stg30/terrain/collision_249_faces.inc"
};
static NJS_MESHSET collision_249_meshsets[1] = {
#include "stg30/terrain/collision_249_meshsets.inc"
};
static NJS_POINT3 collision_249_points[12] = {
#include "stg30/terrain/collision_249_points.inc"
};
static NJS_MODEL collision_249_model = {
#include "stg30/terrain/collision_249_model.inc"
};
static NJS_OBJECT collision_249 = {
#include "stg30/terrain/collision_249.inc"
};
static NJS_MATERIAL collision_250_materials[1] = {
#include "stg30/terrain/collision_250_materials.inc"
};
static Sint16 collision_250_faces[14] = {
#include "stg30/terrain/collision_250_faces.inc"
};
static NJS_MESHSET collision_250_meshsets[1] = {
#include "stg30/terrain/collision_250_meshsets.inc"
};
static NJS_POINT3 collision_250_points[12] = {
#include "stg30/terrain/collision_250_points.inc"
};
static NJS_MODEL collision_250_model = {
#include "stg30/terrain/collision_250_model.inc"
};
static NJS_OBJECT collision_250 = {
#include "stg30/terrain/collision_250.inc"
};
static NJS_MATERIAL collision_251_materials[1] = {
#include "stg30/terrain/collision_251_materials.inc"
};
static Sint16 collision_251_faces[14] = {
#include "stg30/terrain/collision_251_faces.inc"
};
static NJS_MESHSET collision_251_meshsets[1] = {
#include "stg30/terrain/collision_251_meshsets.inc"
};
static NJS_POINT3 collision_251_points[12] = {
#include "stg30/terrain/collision_251_points.inc"
};
static NJS_MODEL collision_251_model = {
#include "stg30/terrain/collision_251_model.inc"
};
static NJS_OBJECT collision_251 = {
#include "stg30/terrain/collision_251.inc"
};
static NJS_MATERIAL collision_252_materials[1] = {
#include "stg30/terrain/collision_252_materials.inc"
};
static Sint16 collision_252_faces[14] = {
#include "stg30/terrain/collision_252_faces.inc"
};
static NJS_MESHSET collision_252_meshsets[1] = {
#include "stg30/terrain/collision_252_meshsets.inc"
};
static NJS_POINT3 collision_252_points[12] = {
#include "stg30/terrain/collision_252_points.inc"
};
static NJS_MODEL collision_252_model = {
#include "stg30/terrain/collision_252_model.inc"
};
static NJS_OBJECT collision_252 = {
#include "stg30/terrain/collision_252.inc"
};
static NJS_MATERIAL collision_253_materials[1] = {
#include "stg30/terrain/collision_253_materials.inc"
};
static Sint16 collision_253_faces[14] = {
#include "stg30/terrain/collision_253_faces.inc"
};
static NJS_MESHSET collision_253_meshsets[1] = {
#include "stg30/terrain/collision_253_meshsets.inc"
};
static NJS_POINT3 collision_253_points[12] = {
#include "stg30/terrain/collision_253_points.inc"
};
static NJS_MODEL collision_253_model = {
#include "stg30/terrain/collision_253_model.inc"
};
static NJS_OBJECT collision_253 = {
#include "stg30/terrain/collision_253.inc"
};
static NJS_MATERIAL collision_254_materials[1] = {
#include "stg30/terrain/collision_254_materials.inc"
};
static Sint16 collision_254_faces[14] = {
#include "stg30/terrain/collision_254_faces.inc"
};
static NJS_MESHSET collision_254_meshsets[1] = {
#include "stg30/terrain/collision_254_meshsets.inc"
};
static NJS_POINT3 collision_254_points[12] = {
#include "stg30/terrain/collision_254_points.inc"
};
static NJS_MODEL collision_254_model = {
#include "stg30/terrain/collision_254_model.inc"
};
static NJS_OBJECT collision_254 = {
#include "stg30/terrain/collision_254.inc"
};
static NJS_MATERIAL collision_255_materials[1] = {
#include "stg30/terrain/collision_255_materials.inc"
};
static Sint16 collision_255_faces[14] = {
#include "stg30/terrain/collision_255_faces.inc"
};
static NJS_MESHSET collision_255_meshsets[1] = {
#include "stg30/terrain/collision_255_meshsets.inc"
};
static NJS_POINT3 collision_255_points[12] = {
#include "stg30/terrain/collision_255_points.inc"
};
static NJS_MODEL collision_255_model = {
#include "stg30/terrain/collision_255_model.inc"
};
static NJS_OBJECT collision_255 = {
#include "stg30/terrain/collision_255.inc"
};
static NJS_MATERIAL collision_256_materials[1] = {
#include "stg30/terrain/collision_256_materials.inc"
};
static Sint16 collision_256_faces[14] = {
#include "stg30/terrain/collision_256_faces.inc"
};
static NJS_MESHSET collision_256_meshsets[1] = {
#include "stg30/terrain/collision_256_meshsets.inc"
};
static NJS_POINT3 collision_256_points[12] = {
#include "stg30/terrain/collision_256_points.inc"
};
static NJS_MODEL collision_256_model = {
#include "stg30/terrain/collision_256_model.inc"
};
static NJS_OBJECT collision_256 = {
#include "stg30/terrain/collision_256.inc"
};
static NJS_MATERIAL collision_257_materials[1] = {
#include "stg30/terrain/collision_257_materials.inc"
};
static Sint16 collision_257_faces[14] = {
#include "stg30/terrain/collision_257_faces.inc"
};
static NJS_MESHSET collision_257_meshsets[1] = {
#include "stg30/terrain/collision_257_meshsets.inc"
};
static NJS_POINT3 collision_257_points[12] = {
#include "stg30/terrain/collision_257_points.inc"
};
static NJS_MODEL collision_257_model = {
#include "stg30/terrain/collision_257_model.inc"
};
static NJS_OBJECT collision_257 = {
#include "stg30/terrain/collision_257.inc"
};
static NJS_MATERIAL collision_258_materials[1] = {
#include "stg30/terrain/collision_258_materials.inc"
};
static Sint16 collision_258_faces[14] = {
#include "stg30/terrain/collision_258_faces.inc"
};
static NJS_MESHSET collision_258_meshsets[1] = {
#include "stg30/terrain/collision_258_meshsets.inc"
};
static NJS_POINT3 collision_258_points[12] = {
#include "stg30/terrain/collision_258_points.inc"
};
static NJS_MODEL collision_258_model = {
#include "stg30/terrain/collision_258_model.inc"
};
static NJS_OBJECT collision_258 = {
#include "stg30/terrain/collision_258.inc"
};
static NJS_MATERIAL collision_259_materials[1] = {
#include "stg30/terrain/collision_259_materials.inc"
};
static Sint16 collision_259_faces[14] = {
#include "stg30/terrain/collision_259_faces.inc"
};
static NJS_MESHSET collision_259_meshsets[1] = {
#include "stg30/terrain/collision_259_meshsets.inc"
};
static NJS_POINT3 collision_259_points[12] = {
#include "stg30/terrain/collision_259_points.inc"
};
static NJS_MODEL collision_259_model = {
#include "stg30/terrain/collision_259_model.inc"
};
static NJS_OBJECT collision_259 = {
#include "stg30/terrain/collision_259.inc"
};
static NJS_MATERIAL collision_260_materials[1] = {
#include "stg30/terrain/collision_260_materials.inc"
};
static Sint16 collision_260_faces[14] = {
#include "stg30/terrain/collision_260_faces.inc"
};
static NJS_MESHSET collision_260_meshsets[1] = {
#include "stg30/terrain/collision_260_meshsets.inc"
};
static NJS_POINT3 collision_260_points[12] = {
#include "stg30/terrain/collision_260_points.inc"
};
static NJS_MODEL collision_260_model = {
#include "stg30/terrain/collision_260_model.inc"
};
static NJS_OBJECT collision_260 = {
#include "stg30/terrain/collision_260.inc"
};
static NJS_MATERIAL collision_261_materials[1] = {
#include "stg30/terrain/collision_261_materials.inc"
};
static Sint16 collision_261_faces[14] = {
#include "stg30/terrain/collision_261_faces.inc"
};
static NJS_MESHSET collision_261_meshsets[1] = {
#include "stg30/terrain/collision_261_meshsets.inc"
};
static NJS_POINT3 collision_261_points[12] = {
#include "stg30/terrain/collision_261_points.inc"
};
static NJS_MODEL collision_261_model = {
#include "stg30/terrain/collision_261_model.inc"
};
static NJS_OBJECT collision_261 = {
#include "stg30/terrain/collision_261.inc"
};
static NJS_MATERIAL collision_262_materials[1] = {
#include "stg30/terrain/collision_262_materials.inc"
};
static Sint16 collision_262_faces[14] = {
#include "stg30/terrain/collision_262_faces.inc"
};
static NJS_MESHSET collision_262_meshsets[1] = {
#include "stg30/terrain/collision_262_meshsets.inc"
};
static NJS_POINT3 collision_262_points[12] = {
#include "stg30/terrain/collision_262_points.inc"
};
static NJS_MODEL collision_262_model = {
#include "stg30/terrain/collision_262_model.inc"
};
static NJS_OBJECT collision_262 = {
#include "stg30/terrain/collision_262.inc"
};
static NJS_MATERIAL collision_263_materials[1] = {
#include "stg30/terrain/collision_263_materials.inc"
};
static Sint16 collision_263_faces[14] = {
#include "stg30/terrain/collision_263_faces.inc"
};
static NJS_MESHSET collision_263_meshsets[1] = {
#include "stg30/terrain/collision_263_meshsets.inc"
};
static NJS_POINT3 collision_263_points[12] = {
#include "stg30/terrain/collision_263_points.inc"
};
static NJS_MODEL collision_263_model = {
#include "stg30/terrain/collision_263_model.inc"
};
static NJS_OBJECT collision_263 = {
#include "stg30/terrain/collision_263.inc"
};
static NJS_MATERIAL collision_264_materials[1] = {
#include "stg30/terrain/collision_264_materials.inc"
};
static Sint16 collision_264_faces[14] = {
#include "stg30/terrain/collision_264_faces.inc"
};
static NJS_MESHSET collision_264_meshsets[1] = {
#include "stg30/terrain/collision_264_meshsets.inc"
};
static NJS_POINT3 collision_264_points[12] = {
#include "stg30/terrain/collision_264_points.inc"
};
static NJS_MODEL collision_264_model = {
#include "stg30/terrain/collision_264_model.inc"
};
static NJS_OBJECT collision_264 = {
#include "stg30/terrain/collision_264.inc"
};
static NJS_MATERIAL collision_265_materials[1] = {
#include "stg30/terrain/collision_265_materials.inc"
};
static Sint16 collision_265_faces[14] = {
#include "stg30/terrain/collision_265_faces.inc"
};
static NJS_MESHSET collision_265_meshsets[1] = {
#include "stg30/terrain/collision_265_meshsets.inc"
};
static NJS_POINT3 collision_265_points[12] = {
#include "stg30/terrain/collision_265_points.inc"
};
static NJS_MODEL collision_265_model = {
#include "stg30/terrain/collision_265_model.inc"
};
static NJS_OBJECT collision_265 = {
#include "stg30/terrain/collision_265.inc"
};
static NJS_MATERIAL collision_266_materials[1] = {
#include "stg30/terrain/collision_266_materials.inc"
};
static Sint16 collision_266_faces[60] = {
#include "stg30/terrain/collision_266_faces.inc"
};
static NJS_MESHSET collision_266_meshsets[1] = {
#include "stg30/terrain/collision_266_meshsets.inc"
};
static NJS_POINT3 collision_266_points[35] = {
#include "stg30/terrain/collision_266_points.inc"
};
static NJS_MODEL collision_266_model = {
#include "stg30/terrain/collision_266_model.inc"
};
static NJS_OBJECT collision_266 = {
#include "stg30/terrain/collision_266.inc"
};
static NJS_MATERIAL collision_267_materials[1] = {
#include "stg30/terrain/collision_267_materials.inc"
};
static Sint16 collision_267_faces[72] = {
#include "stg30/terrain/collision_267_faces.inc"
};
static NJS_MESHSET collision_267_meshsets[1] = {
#include "stg30/terrain/collision_267_meshsets.inc"
};
static NJS_POINT3 collision_267_points[44] = {
#include "stg30/terrain/collision_267_points.inc"
};
static NJS_MODEL collision_267_model = {
#include "stg30/terrain/collision_267_model.inc"
};
static NJS_OBJECT collision_267 = {
#include "stg30/terrain/collision_267.inc"
};
static NJS_MATERIAL collision_268_materials[1] = {
#include "stg30/terrain/collision_268_materials.inc"
};
static Sint16 collision_268_faces[87] = {
#include "stg30/terrain/collision_268_faces.inc"
};
static NJS_MESHSET collision_268_meshsets[1] = {
#include "stg30/terrain/collision_268_meshsets.inc"
};
static NJS_POINT3 collision_268_points[50] = {
#include "stg30/terrain/collision_268_points.inc"
};
static NJS_MODEL collision_268_model = {
#include "stg30/terrain/collision_268_model.inc"
};
static NJS_OBJECT collision_268 = {
#include "stg30/terrain/collision_268.inc"
};
static NJS_MATERIAL collision_269_materials[1] = {
#include "stg30/terrain/collision_269_materials.inc"
};
static Sint16 collision_269_faces[216] = {
#include "stg30/terrain/collision_269_faces.inc"
};
static NJS_MESHSET collision_269_meshsets[1] = {
#include "stg30/terrain/collision_269_meshsets.inc"
};
static NJS_POINT3 collision_269_points[90] = {
#include "stg30/terrain/collision_269_points.inc"
};
static NJS_MODEL collision_269_model = {
#include "stg30/terrain/collision_269_model.inc"
};
static NJS_OBJECT collision_269 = {
#include "stg30/terrain/collision_269.inc"
};
static NJS_MATERIAL collision_270_materials[1] = {
#include "stg30/terrain/collision_270_materials.inc"
};
static Sint16 collision_270_faces[106] = {
#include "stg30/terrain/collision_270_faces.inc"
};
static NJS_MESHSET collision_270_meshsets[1] = {
#include "stg30/terrain/collision_270_meshsets.inc"
};
static NJS_POINT3 collision_270_points[59] = {
#include "stg30/terrain/collision_270_points.inc"
};
static NJS_MODEL collision_270_model = {
#include "stg30/terrain/collision_270_model.inc"
};
static NJS_OBJECT collision_270 = {
#include "stg30/terrain/collision_270.inc"
};
static NJS_MATERIAL collision_271_materials[1] = {
#include "stg30/terrain/collision_271_materials.inc"
};
static Sint16 collision_271_faces[90] = {
#include "stg30/terrain/collision_271_faces.inc"
};
static NJS_MESHSET collision_271_meshsets[1] = {
#include "stg30/terrain/collision_271_meshsets.inc"
};
static NJS_POINT3 collision_271_points[49] = {
#include "stg30/terrain/collision_271_points.inc"
};
static NJS_MODEL collision_271_model = {
#include "stg30/terrain/collision_271_model.inc"
};
static NJS_OBJECT collision_271 = {
#include "stg30/terrain/collision_271.inc"
};
static NJS_MATERIAL collision_272_materials[1] = {
#include "stg30/terrain/collision_272_materials.inc"
};
static Sint16 collision_272_faces[143] = {
#include "stg30/terrain/collision_272_faces.inc"
};
static NJS_MESHSET collision_272_meshsets[1] = {
#include "stg30/terrain/collision_272_meshsets.inc"
};
static NJS_POINT3 collision_272_points[66] = {
#include "stg30/terrain/collision_272_points.inc"
};
static NJS_MODEL collision_272_model = {
#include "stg30/terrain/collision_272_model.inc"
};
static NJS_OBJECT collision_272 = {
#include "stg30/terrain/collision_272.inc"
};
static NJS_MATERIAL collision_273_materials[1] = {
#include "stg30/terrain/collision_273_materials.inc"
};
static Sint16 collision_273_faces[63] = {
#include "stg30/terrain/collision_273_faces.inc"
};
static NJS_MESHSET collision_273_meshsets[1] = {
#include "stg30/terrain/collision_273_meshsets.inc"
};
static NJS_POINT3 collision_273_points[36] = {
#include "stg30/terrain/collision_273_points.inc"
};
static NJS_MODEL collision_273_model = {
#include "stg30/terrain/collision_273_model.inc"
};
static NJS_OBJECT collision_273 = {
#include "stg30/terrain/collision_273.inc"
};
static NJS_MATERIAL collision_274_materials[1] = {
#include "stg30/terrain/collision_274_materials.inc"
};
static Sint16 collision_274_faces[36] = {
#include "stg30/terrain/collision_274_faces.inc"
};
static NJS_MESHSET collision_274_meshsets[1] = {
#include "stg30/terrain/collision_274_meshsets.inc"
};
static NJS_POINT3 collision_274_points[20] = {
#include "stg30/terrain/collision_274_points.inc"
};
static NJS_MODEL collision_274_model = {
#include "stg30/terrain/collision_274_model.inc"
};
static NJS_OBJECT collision_274 = {
#include "stg30/terrain/collision_274.inc"
};
static NJS_MATERIAL collision_275_materials[1] = {
#include "stg30/terrain/collision_275_materials.inc"
};
static Sint16 collision_275_faces[60] = {
#include "stg30/terrain/collision_275_faces.inc"
};
static NJS_MESHSET collision_275_meshsets[1] = {
#include "stg30/terrain/collision_275_meshsets.inc"
};
static NJS_POINT3 collision_275_points[35] = {
#include "stg30/terrain/collision_275_points.inc"
};
static NJS_MODEL collision_275_model = {
#include "stg30/terrain/collision_275_model.inc"
};
static NJS_OBJECT collision_275 = {
#include "stg30/terrain/collision_275.inc"
};
static NJS_MATERIAL collision_276_materials[1] = {
#include "stg30/terrain/collision_276_materials.inc"
};
static Sint16 collision_276_faces[67] = {
#include "stg30/terrain/collision_276_faces.inc"
};
static NJS_MESHSET collision_276_meshsets[1] = {
#include "stg30/terrain/collision_276_meshsets.inc"
};
static NJS_POINT3 collision_276_points[33] = {
#include "stg30/terrain/collision_276_points.inc"
};
static NJS_MODEL collision_276_model = {
#include "stg30/terrain/collision_276_model.inc"
};
static NJS_OBJECT collision_276 = {
#include "stg30/terrain/collision_276.inc"
};
static NJS_MATERIAL collision_277_materials[1] = {
#include "stg30/terrain/collision_277_materials.inc"
};
static Sint16 collision_277_faces[122] = {
#include "stg30/terrain/collision_277_faces.inc"
};
static NJS_MESHSET collision_277_meshsets[1] = {
#include "stg30/terrain/collision_277_meshsets.inc"
};
static NJS_POINT3 collision_277_points[56] = {
#include "stg30/terrain/collision_277_points.inc"
};
static NJS_MODEL collision_277_model = {
#include "stg30/terrain/collision_277_model.inc"
};
static NJS_OBJECT collision_277 = {
#include "stg30/terrain/collision_277.inc"
};
static NJS_MATERIAL collision_278_materials[1] = {
#include "stg30/terrain/collision_278_materials.inc"
};
static Sint16 collision_278_faces[73] = {
#include "stg30/terrain/collision_278_faces.inc"
};
static NJS_MESHSET collision_278_meshsets[1] = {
#include "stg30/terrain/collision_278_meshsets.inc"
};
static NJS_POINT3 collision_278_points[41] = {
#include "stg30/terrain/collision_278_points.inc"
};
static NJS_MODEL collision_278_model = {
#include "stg30/terrain/collision_278_model.inc"
};
static NJS_OBJECT collision_278 = {
#include "stg30/terrain/collision_278.inc"
};
static NJS_MATERIAL collision_279_materials[1] = {
#include "stg30/terrain/collision_279_materials.inc"
};
static Sint16 collision_279_faces[72] = {
#include "stg30/terrain/collision_279_faces.inc"
};
static NJS_MESHSET collision_279_meshsets[1] = {
#include "stg30/terrain/collision_279_meshsets.inc"
};
static NJS_POINT3 collision_279_points[33] = {
#include "stg30/terrain/collision_279_points.inc"
};
static NJS_MODEL collision_279_model = {
#include "stg30/terrain/collision_279_model.inc"
};
static NJS_OBJECT collision_279 = {
#include "stg30/terrain/collision_279.inc"
};
static NJS_MATERIAL collision_306_materials[1] = {
#include "stg30/terrain/collision_306_materials.inc"
};
static Sint16 collision_306_faces[63] = {
#include "stg30/terrain/collision_306_faces.inc"
};
static NJS_MESHSET collision_306_meshsets[1] = {
#include "stg30/terrain/collision_306_meshsets.inc"
};
static NJS_POINT3 collision_306_points[42] = {
#include "stg30/terrain/collision_306_points.inc"
};
static NJS_MODEL collision_306_model = {
#include "stg30/terrain/collision_306_model.inc"
};
static NJS_OBJECT collision_306 = {
#include "stg30/terrain/collision_306.inc"
};
static NJS_MATERIAL collision_307_materials[1] = {
#include "stg30/terrain/collision_307_materials.inc"
};
static Sint16 collision_307_faces[74] = {
#include "stg30/terrain/collision_307_faces.inc"
};
static NJS_MESHSET collision_307_meshsets[1] = {
#include "stg30/terrain/collision_307_meshsets.inc"
};
static NJS_POINT3 collision_307_points[54] = {
#include "stg30/terrain/collision_307_points.inc"
};
static NJS_MODEL collision_307_model = {
#include "stg30/terrain/collision_307_model.inc"
};
static NJS_OBJECT collision_307 = {
#include "stg30/terrain/collision_307.inc"
};
static NJS_MATERIAL collision_308_materials[1] = {
#include "stg30/terrain/collision_308_materials.inc"
};
static Sint16 collision_308_faces[104] = {
#include "stg30/terrain/collision_308_faces.inc"
};
static NJS_MESHSET collision_308_meshsets[1] = {
#include "stg30/terrain/collision_308_meshsets.inc"
};
static NJS_POINT3 collision_308_points[60] = {
#include "stg30/terrain/collision_308_points.inc"
};
static NJS_MODEL collision_308_model = {
#include "stg30/terrain/collision_308_model.inc"
};
static NJS_OBJECT collision_308 = {
#include "stg30/terrain/collision_308.inc"
};
static NJS_MATERIAL collision_309_materials[1] = {
#include "stg30/terrain/collision_309_materials.inc"
};
static Sint16 collision_309_faces[97] = {
#include "stg30/terrain/collision_309_faces.inc"
};
static NJS_MESHSET collision_309_meshsets[1] = {
#include "stg30/terrain/collision_309_meshsets.inc"
};
static NJS_POINT3 collision_309_points[42] = {
#include "stg30/terrain/collision_309_points.inc"
};
static NJS_MODEL collision_309_model = {
#include "stg30/terrain/collision_309_model.inc"
};
static NJS_OBJECT collision_309 = {
#include "stg30/terrain/collision_309.inc"
};
static NJS_MATERIAL collision_310_materials[1] = {
#include "stg30/terrain/collision_310_materials.inc"
};
static Sint16 collision_310_faces[97] = {
#include "stg30/terrain/collision_310_faces.inc"
};
static NJS_MESHSET collision_310_meshsets[1] = {
#include "stg30/terrain/collision_310_meshsets.inc"
};
static NJS_POINT3 collision_310_points[42] = {
#include "stg30/terrain/collision_310_points.inc"
};
static NJS_MODEL collision_310_model = {
#include "stg30/terrain/collision_310_model.inc"
};
static NJS_OBJECT collision_310 = {
#include "stg30/terrain/collision_310.inc"
};
static NJS_MATERIAL collision_311_materials[1] = {
#include "stg30/terrain/collision_311_materials.inc"
};
static Sint16 collision_311_faces[97] = {
#include "stg30/terrain/collision_311_faces.inc"
};
static NJS_MESHSET collision_311_meshsets[1] = {
#include "stg30/terrain/collision_311_meshsets.inc"
};
static NJS_POINT3 collision_311_points[42] = {
#include "stg30/terrain/collision_311_points.inc"
};
static NJS_MODEL collision_311_model = {
#include "stg30/terrain/collision_311_model.inc"
};
static NJS_OBJECT collision_311 = {
#include "stg30/terrain/collision_311.inc"
};
static NJS_MATERIAL collision_312_materials[1] = {
#include "stg30/terrain/collision_312_materials.inc"
};
static Sint16 collision_312_faces[97] = {
#include "stg30/terrain/collision_312_faces.inc"
};
static NJS_MESHSET collision_312_meshsets[1] = {
#include "stg30/terrain/collision_312_meshsets.inc"
};
static NJS_POINT3 collision_312_points[42] = {
#include "stg30/terrain/collision_312_points.inc"
};
static NJS_MODEL collision_312_model = {
#include "stg30/terrain/collision_312_model.inc"
};
static NJS_OBJECT collision_312 = {
#include "stg30/terrain/collision_312.inc"
};
static NJS_MATERIAL collision_313_materials[1] = {
#include "stg30/terrain/collision_313_materials.inc"
};
static Sint16 collision_313_faces[97] = {
#include "stg30/terrain/collision_313_faces.inc"
};
static NJS_MESHSET collision_313_meshsets[1] = {
#include "stg30/terrain/collision_313_meshsets.inc"
};
static NJS_POINT3 collision_313_points[42] = {
#include "stg30/terrain/collision_313_points.inc"
};
static NJS_MODEL collision_313_model = {
#include "stg30/terrain/collision_313_model.inc"
};
static NJS_OBJECT collision_313 = {
#include "stg30/terrain/collision_313.inc"
};
static NJS_MATERIAL collision_314_materials[1] = {
#include "stg30/terrain/collision_314_materials.inc"
};
static Sint16 collision_314_faces[97] = {
#include "stg30/terrain/collision_314_faces.inc"
};
static NJS_MESHSET collision_314_meshsets[1] = {
#include "stg30/terrain/collision_314_meshsets.inc"
};
static NJS_POINT3 collision_314_points[42] = {
#include "stg30/terrain/collision_314_points.inc"
};
static NJS_MODEL collision_314_model = {
#include "stg30/terrain/collision_314_model.inc"
};
static NJS_OBJECT collision_314 = {
#include "stg30/terrain/collision_314.inc"
};
static NJS_MATERIAL collision_315_materials[1] = {
#include "stg30/terrain/collision_315_materials.inc"
};
static Sint16 collision_315_faces[97] = {
#include "stg30/terrain/collision_315_faces.inc"
};
static NJS_MESHSET collision_315_meshsets[1] = {
#include "stg30/terrain/collision_315_meshsets.inc"
};
static NJS_POINT3 collision_315_points[42] = {
#include "stg30/terrain/collision_315_points.inc"
};
static NJS_MODEL collision_315_model = {
#include "stg30/terrain/collision_315_model.inc"
};
static NJS_OBJECT collision_315 = {
#include "stg30/terrain/collision_315.inc"
};
static NJS_MATERIAL collision_316_materials[1] = {
#include "stg30/terrain/collision_316_materials.inc"
};
static Sint16 collision_316_faces[97] = {
#include "stg30/terrain/collision_316_faces.inc"
};
static NJS_MESHSET collision_316_meshsets[1] = {
#include "stg30/terrain/collision_316_meshsets.inc"
};
static NJS_POINT3 collision_316_points[42] = {
#include "stg30/terrain/collision_316_points.inc"
};
static NJS_MODEL collision_316_model = {
#include "stg30/terrain/collision_316_model.inc"
};
static NJS_OBJECT collision_316 = {
#include "stg30/terrain/collision_316.inc"
};
static NJS_MATERIAL collision_317_materials[1] = {
#include "stg30/terrain/collision_317_materials.inc"
};
static Sint16 collision_317_faces[97] = {
#include "stg30/terrain/collision_317_faces.inc"
};
static NJS_MESHSET collision_317_meshsets[1] = {
#include "stg30/terrain/collision_317_meshsets.inc"
};
static NJS_POINT3 collision_317_points[42] = {
#include "stg30/terrain/collision_317_points.inc"
};
static NJS_MODEL collision_317_model = {
#include "stg30/terrain/collision_317_model.inc"
};
static NJS_OBJECT collision_317 = {
#include "stg30/terrain/collision_317.inc"
};
static NJS_MATERIAL collision_318_materials[1] = {
#include "stg30/terrain/collision_318_materials.inc"
};
static Sint16 collision_318_faces[97] = {
#include "stg30/terrain/collision_318_faces.inc"
};
static NJS_MESHSET collision_318_meshsets[1] = {
#include "stg30/terrain/collision_318_meshsets.inc"
};
static NJS_POINT3 collision_318_points[42] = {
#include "stg30/terrain/collision_318_points.inc"
};
static NJS_MODEL collision_318_model = {
#include "stg30/terrain/collision_318_model.inc"
};
static NJS_OBJECT collision_318 = {
#include "stg30/terrain/collision_318.inc"
};
static NJS_MATERIAL collision_319_materials[1] = {
#include "stg30/terrain/collision_319_materials.inc"
};
static Sint16 collision_319_faces[83] = {
#include "stg30/terrain/collision_319_faces.inc"
};
static NJS_MESHSET collision_319_meshsets[1] = {
#include "stg30/terrain/collision_319_meshsets.inc"
};
static NJS_POINT3 collision_319_points[40] = {
#include "stg30/terrain/collision_319_points.inc"
};
static NJS_MODEL collision_319_model = {
#include "stg30/terrain/collision_319_model.inc"
};
static NJS_OBJECT collision_319 = {
#include "stg30/terrain/collision_319.inc"
};
static NJS_MATERIAL collision_324_materials[1] = {
#include "stg30/terrain/collision_324_materials.inc"
};
static Sint16 collision_324_faces[29] = {
#include "stg30/terrain/collision_324_faces.inc"
};
static NJS_MESHSET collision_324_meshsets[1] = {
#include "stg30/terrain/collision_324_meshsets.inc"
};
static NJS_POINT3 collision_324_points[18] = {
#include "stg30/terrain/collision_324_points.inc"
};
static NJS_MODEL collision_324_model = {
#include "stg30/terrain/collision_324_model.inc"
};
static NJS_OBJECT collision_324 = {
#include "stg30/terrain/collision_324.inc"
};
static NJS_MATERIAL collision_325_materials[1] = {
#include "stg30/terrain/collision_325_materials.inc"
};
static Sint16 collision_325_faces[63] = {
#include "stg30/terrain/collision_325_faces.inc"
};
static NJS_MESHSET collision_325_meshsets[1] = {
#include "stg30/terrain/collision_325_meshsets.inc"
};
static NJS_POINT3 collision_325_points[35] = {
#include "stg30/terrain/collision_325_points.inc"
};
static NJS_MODEL collision_325_model = {
#include "stg30/terrain/collision_325_model.inc"
};
static NJS_OBJECT collision_325 = {
#include "stg30/terrain/collision_325.inc"
};
static NJS_MATERIAL collision_327_materials[1] = {
#include "stg30/terrain/collision_327_materials.inc"
};
static Sint16 collision_327_faces[34] = {
#include "stg30/terrain/collision_327_faces.inc"
};
static NJS_MESHSET collision_327_meshsets[1] = {
#include "stg30/terrain/collision_327_meshsets.inc"
};
static NJS_POINT3 collision_327_points[16] = {
#include "stg30/terrain/collision_327_points.inc"
};
static NJS_MODEL collision_327_model = {
#include "stg30/terrain/collision_327_model.inc"
};
static NJS_OBJECT collision_327 = {
#include "stg30/terrain/collision_327.inc"
};
static NJS_MATERIAL collision_335_materials[1] = {
#include "stg30/terrain/collision_335_materials.inc"
};
static Sint16 collision_335_faces[36] = {
#include "stg30/terrain/collision_335_faces.inc"
};
static NJS_MESHSET collision_335_meshsets[1] = {
#include "stg30/terrain/collision_335_meshsets.inc"
};
static NJS_POINT3 collision_335_points[24] = {
#include "stg30/terrain/collision_335_points.inc"
};
static NJS_MODEL collision_335_model = {
#include "stg30/terrain/collision_335_model.inc"
};
static NJS_OBJECT collision_335 = {
#include "stg30/terrain/collision_335.inc"
};
static NJS_MATERIAL collision_336_materials[1] = {
#include "stg30/terrain/collision_336_materials.inc"
};
static Sint16 collision_336_faces[75] = {
#include "stg30/terrain/collision_336_faces.inc"
};
static NJS_MESHSET collision_336_meshsets[1] = {
#include "stg30/terrain/collision_336_meshsets.inc"
};
static NJS_POINT3 collision_336_points[42] = {
#include "stg30/terrain/collision_336_points.inc"
};
static NJS_MODEL collision_336_model = {
#include "stg30/terrain/collision_336_model.inc"
};
static NJS_OBJECT collision_336 = {
#include "stg30/terrain/collision_336.inc"
};
static NJS_MATERIAL collision_340_materials[1] = {
#include "stg30/terrain/collision_340_materials.inc"
};
static Sint16 collision_340_faces[75] = {
#include "stg30/terrain/collision_340_faces.inc"
};
static NJS_MESHSET collision_340_meshsets[1] = {
#include "stg30/terrain/collision_340_meshsets.inc"
};
static NJS_POINT3 collision_340_points[42] = {
#include "stg30/terrain/collision_340_points.inc"
};
static NJS_MODEL collision_340_model = {
#include "stg30/terrain/collision_340_model.inc"
};
static NJS_OBJECT collision_340 = {
#include "stg30/terrain/collision_340.inc"
};
static NJS_MATERIAL collision_344_materials[1] = {
#include "stg30/terrain/collision_344_materials.inc"
};
static Sint16 collision_344_faces[75] = {
#include "stg30/terrain/collision_344_faces.inc"
};
static NJS_MESHSET collision_344_meshsets[1] = {
#include "stg30/terrain/collision_344_meshsets.inc"
};
static NJS_POINT3 collision_344_points[42] = {
#include "stg30/terrain/collision_344_points.inc"
};
static NJS_MODEL collision_344_model = {
#include "stg30/terrain/collision_344_model.inc"
};
static NJS_OBJECT collision_344 = {
#include "stg30/terrain/collision_344.inc"
};
static NJS_MATERIAL collision_348_materials[1] = {
#include "stg30/terrain/collision_348_materials.inc"
};
static Sint16 collision_348_faces[60] = {
#include "stg30/terrain/collision_348_faces.inc"
};
static NJS_MESHSET collision_348_meshsets[1] = {
#include "stg30/terrain/collision_348_meshsets.inc"
};
static NJS_POINT3 collision_348_points[35] = {
#include "stg30/terrain/collision_348_points.inc"
};
static NJS_MODEL collision_348_model = {
#include "stg30/terrain/collision_348_model.inc"
};
static NJS_OBJECT collision_348 = {
#include "stg30/terrain/collision_348.inc"
};
static NJS_MATERIAL collision_352_materials[1] = {
#include "stg30/terrain/collision_352_materials.inc"
};
static Sint16 collision_352_faces[87] = {
#include "stg30/terrain/collision_352_faces.inc"
};
static NJS_MESHSET collision_352_meshsets[1] = {
#include "stg30/terrain/collision_352_meshsets.inc"
};
static NJS_POINT3 collision_352_points[42] = {
#include "stg30/terrain/collision_352_points.inc"
};
static NJS_MODEL collision_352_model = {
#include "stg30/terrain/collision_352_model.inc"
};
static NJS_OBJECT collision_352 = {
#include "stg30/terrain/collision_352.inc"
};
static NJS_MATERIAL collision_355_materials[1] = {
#include "stg30/terrain/collision_355_materials.inc"
};
static Sint16 collision_355_faces[59] = {
#include "stg30/terrain/collision_355_faces.inc"
};
static NJS_MESHSET collision_355_meshsets[1] = {
#include "stg30/terrain/collision_355_meshsets.inc"
};
static NJS_POINT3 collision_355_points[24] = {
#include "stg30/terrain/collision_355_points.inc"
};
static NJS_MODEL collision_355_model = {
#include "stg30/terrain/collision_355_model.inc"
};
static NJS_OBJECT collision_355 = {
#include "stg30/terrain/collision_355.inc"
};
static NJS_MATERIAL collision_418_materials[1] = {
#include "stg30/terrain/collision_418_materials.inc"
};
static Sint16 collision_418_faces[14] = {
#include "stg30/terrain/collision_418_faces.inc"
};
static NJS_MESHSET collision_418_meshsets[1] = {
#include "stg30/terrain/collision_418_meshsets.inc"
};
static NJS_POINT3 collision_418_points[12] = {
#include "stg30/terrain/collision_418_points.inc"
};
static NJS_MODEL collision_418_model = {
#include "stg30/terrain/collision_418_model.inc"
};
static NJS_OBJECT collision_418 = {
#include "stg30/terrain/collision_418.inc"
};
static NJS_MATERIAL collision_420_materials[1] = {
#include "stg30/terrain/collision_420_materials.inc"
};
static Sint16 collision_420_faces[40] = {
#include "stg30/terrain/collision_420_faces.inc"
};
static NJS_MESHSET collision_420_meshsets[1] = {
#include "stg30/terrain/collision_420_meshsets.inc"
};
static NJS_POINT3 collision_420_points[24] = {
#include "stg30/terrain/collision_420_points.inc"
};
static NJS_MODEL collision_420_model = {
#include "stg30/terrain/collision_420_model.inc"
};
static NJS_OBJECT collision_420 = {
#include "stg30/terrain/collision_420.inc"
};
static NJS_MATERIAL collision_423_materials[1] = {
#include "stg30/terrain/collision_423_materials.inc"
};
static Sint16 collision_423_faces[29] = {
#include "stg30/terrain/collision_423_faces.inc"
};
static NJS_MESHSET collision_423_meshsets[1] = {
#include "stg30/terrain/collision_423_meshsets.inc"
};
static NJS_POINT3 collision_423_points[16] = {
#include "stg30/terrain/collision_423_points.inc"
};
static NJS_MODEL collision_423_model = {
#include "stg30/terrain/collision_423_model.inc"
};
static NJS_OBJECT collision_423 = {
#include "stg30/terrain/collision_423.inc"
};
static NJS_MATERIAL collision_425_materials[1] = {
#include "stg30/terrain/collision_425_materials.inc"
};
static Sint16 collision_425_faces[142] = {
#include "stg30/terrain/collision_425_faces.inc"
};
static NJS_MESHSET collision_425_meshsets[1] = {
#include "stg30/terrain/collision_425_meshsets.inc"
};
static NJS_POINT3 collision_425_points[52] = {
#include "stg30/terrain/collision_425_points.inc"
};
static NJS_MODEL collision_425_model = {
#include "stg30/terrain/collision_425_model.inc"
};
static NJS_OBJECT collision_425 = {
#include "stg30/terrain/collision_425.inc"
};
static NJS_MATERIAL collision_428_materials[1] = {
#include "stg30/terrain/collision_428_materials.inc"
};
static Sint16 collision_428_faces[30] = {
#include "stg30/terrain/collision_428_faces.inc"
};
static NJS_MESHSET collision_428_meshsets[1] = {
#include "stg30/terrain/collision_428_meshsets.inc"
};
static NJS_POINT3 collision_428_points[16] = {
#include "stg30/terrain/collision_428_points.inc"
};
static NJS_MODEL collision_428_model = {
#include "stg30/terrain/collision_428_model.inc"
};
static NJS_OBJECT collision_428 = {
#include "stg30/terrain/collision_428.inc"
};
static NJS_MATERIAL collision_430_materials[1] = {
#include "stg30/terrain/collision_430_materials.inc"
};
static Sint16 collision_430_faces[9] = {
#include "stg30/terrain/collision_430_faces.inc"
};
static NJS_MESHSET collision_430_meshsets[1] = {
#include "stg30/terrain/collision_430_meshsets.inc"
};
static NJS_POINT3 collision_430_points[8] = {
#include "stg30/terrain/collision_430_points.inc"
};
static NJS_MODEL collision_430_model = {
#include "stg30/terrain/collision_430_model.inc"
};
static NJS_OBJECT collision_430 = {
#include "stg30/terrain/collision_430.inc"
};
static NJS_MATERIAL collision_432_materials[1] = {
#include "stg30/terrain/collision_432_materials.inc"
};
static Sint16 collision_432_faces[46] = {
#include "stg30/terrain/collision_432_faces.inc"
};
static NJS_MESHSET collision_432_meshsets[1] = {
#include "stg30/terrain/collision_432_meshsets.inc"
};
static NJS_POINT3 collision_432_points[26] = {
#include "stg30/terrain/collision_432_points.inc"
};
static NJS_MODEL collision_432_model = {
#include "stg30/terrain/collision_432_model.inc"
};
static NJS_OBJECT collision_432 = {
#include "stg30/terrain/collision_432.inc"
};
static NJS_MATERIAL collision_434_materials[1] = {
#include "stg30/terrain/collision_434_materials.inc"
};
static Sint16 collision_434_faces[7] = {
#include "stg30/terrain/collision_434_faces.inc"
};
static NJS_MESHSET collision_434_meshsets[1] = {
#include "stg30/terrain/collision_434_meshsets.inc"
};
static NJS_POINT3 collision_434_points[6] = {
#include "stg30/terrain/collision_434_points.inc"
};
static NJS_MODEL collision_434_model = {
#include "stg30/terrain/collision_434_model.inc"
};
static NJS_OBJECT collision_434 = {
#include "stg30/terrain/collision_434.inc"
};
static NJS_MATERIAL collision_471_materials[1] = {
#include "stg30/terrain/collision_471_materials.inc"
};
static Sint16 collision_471_faces[25] = {
#include "stg30/terrain/collision_471_faces.inc"
};
static NJS_MESHSET collision_471_meshsets[1] = {
#include "stg30/terrain/collision_471_meshsets.inc"
};
static NJS_POINT3 collision_471_points[10] = {
#include "stg30/terrain/collision_471_points.inc"
};
static NJS_MODEL collision_471_model = {
#include "stg30/terrain/collision_471_model.inc"
};
static NJS_OBJECT collision_471 = {
#include "stg30/terrain/collision_471.inc"
};
static NJS_MATERIAL collision_626_materials[1] = {
#include "stg30/terrain/collision_626_materials.inc"
};
static Sint16 collision_626_faces[7] = {
#include "stg30/terrain/collision_626_faces.inc"
};
static NJS_MESHSET collision_626_meshsets[1] = {
#include "stg30/terrain/collision_626_meshsets.inc"
};
static NJS_POINT3 collision_626_points[6] = {
#include "stg30/terrain/collision_626_points.inc"
};
static NJS_MODEL collision_626_model = {
#include "stg30/terrain/collision_626_model.inc"
};
static NJS_OBJECT collision_626 = {
#include "stg30/terrain/collision_626.inc"
};
static NJS_MATERIAL collision_653_materials[1] = {
#include "stg30/terrain/collision_653_materials.inc"
};
static Sint16 collision_653_faces[14] = {
#include "stg30/terrain/collision_653_faces.inc"
};
static NJS_MESHSET collision_653_meshsets[1] = {
#include "stg30/terrain/collision_653_meshsets.inc"
};
static NJS_POINT3 collision_653_points[12] = {
#include "stg30/terrain/collision_653_points.inc"
};
static NJS_MODEL collision_653_model = {
#include "stg30/terrain/collision_653_model.inc"
};
static NJS_OBJECT collision_653 = {
#include "stg30/terrain/collision_653.inc"
};
static NJS_MATERIAL collision_661_materials[1] = {
#include "stg30/terrain/collision_661_materials.inc"
};
static Sint16 collision_661_faces[40] = {
#include "stg30/terrain/collision_661_faces.inc"
};
static NJS_MESHSET collision_661_meshsets[1] = {
#include "stg30/terrain/collision_661_meshsets.inc"
};
static NJS_POINT3 collision_661_points[24] = {
#include "stg30/terrain/collision_661_points.inc"
};
static NJS_MODEL collision_661_model = {
#include "stg30/terrain/collision_661_model.inc"
};
static NJS_OBJECT collision_661 = {
#include "stg30/terrain/collision_661.inc"
};
static NJS_MATERIAL collision_669_materials[1] = {
#include "stg30/terrain/collision_669_materials.inc"
};
static Sint16 collision_669_faces[85] = {
#include "stg30/terrain/collision_669_faces.inc"
};
static NJS_MESHSET collision_669_meshsets[1] = {
#include "stg30/terrain/collision_669_meshsets.inc"
};
static NJS_POINT3 collision_669_points[43] = {
#include "stg30/terrain/collision_669_points.inc"
};
static NJS_MODEL collision_669_model = {
#include "stg30/terrain/collision_669_model.inc"
};
static NJS_OBJECT collision_669 = {
#include "stg30/terrain/collision_669.inc"
};
static Sint16 draw_8_polygons[185] = {
#include "stg30/terrain/draw_8_polygons.inc"
};
static Sint32 draw_8_vertices[159] = {
#include "stg30/terrain/draw_8_vertices.inc"
};
static NJS_CNK_MODEL draw_8_model = {
#include "stg30/terrain/draw_8_model.inc"
};
static NJS_CNK_OBJECT draw_856 = {
#include "stg30/terrain/draw_856.inc"
};
static Sint16 draw_100_polygons[355] = {
#include "stg30/terrain/draw_100_polygons.inc"
};
static Sint32 draw_100_vertices[243] = {
#include "stg30/terrain/draw_100_vertices.inc"
};
static NJS_CNK_MODEL draw_100_model = {
#include "stg30/terrain/draw_100_model.inc"
};
static NJS_CNK_OBJECT draw_855 = {
#include "stg30/terrain/draw_855.inc"
};
static Sint16 draw_17_polygons[555] = {
#include "stg30/terrain/draw_17_polygons.inc"
};
static Sint32 draw_17_vertices[379] = {
#include "stg30/terrain/draw_17_vertices.inc"
};
static NJS_CNK_MODEL draw_17_model = {
#include "stg30/terrain/draw_17_model.inc"
};
static NJS_CNK_OBJECT draw_854 = {
#include "stg30/terrain/draw_854.inc"
};
static Sint16 draw_11_polygons[429] = {
#include "stg30/terrain/draw_11_polygons.inc"
};
static Sint32 draw_11_vertices[319] = {
#include "stg30/terrain/draw_11_vertices.inc"
};
static NJS_CNK_MODEL draw_11_model = {
#include "stg30/terrain/draw_11_model.inc"
};
static NJS_CNK_OBJECT draw_849 = {
#include "stg30/terrain/draw_849.inc"
};
static Sint16 draw_6_polygons[391] = {
#include "stg30/terrain/draw_6_polygons.inc"
};
static Sint32 draw_6_vertices[279] = {
#include "stg30/terrain/draw_6_vertices.inc"
};
static NJS_CNK_MODEL draw_6_model = {
#include "stg30/terrain/draw_6_model.inc"
};
static NJS_CNK_OBJECT draw_836 = {
#include "stg30/terrain/draw_836.inc"
};
static Sint16 draw_1_polygons[169] = {
#include "stg30/terrain/draw_1_polygons.inc"
};
static Sint32 draw_1_vertices[155] = {
#include "stg30/terrain/draw_1_vertices.inc"
};
static NJS_CNK_MODEL draw_1_model = {
#include "stg30/terrain/draw_1_model.inc"
};
static NJS_CNK_OBJECT draw_834 = {
#include "stg30/terrain/draw_834.inc"
};
static Sint16 draw_0_polygons[381] = {
#include "stg30/terrain/draw_0_polygons.inc"
};
static Sint32 draw_0_vertices[339] = {
#include "stg30/terrain/draw_0_vertices.inc"
};
static NJS_CNK_MODEL draw_0_model = {
#include "stg30/terrain/draw_0_model.inc"
};
static NJS_CNK_OBJECT draw_810 = {
#include "stg30/terrain/draw_810.inc"
};
static Sint16 draw_761_polygons[169] = {
#include "stg30/terrain/draw_761_polygons.inc"
};
static Sint32 draw_761_vertices[155] = {
#include "stg30/terrain/draw_761_vertices.inc"
};
static NJS_CNK_MODEL draw_761_model = {
#include "stg30/terrain/draw_761_model.inc"
};
static NJS_CNK_OBJECT draw_808 = {
#include "stg30/terrain/draw_808.inc"
};
static Sint16 draw_760_polygons[391] = {
#include "stg30/terrain/draw_760_polygons.inc"
};
static Sint32 draw_760_vertices[279] = {
#include "stg30/terrain/draw_760_vertices.inc"
};
static NJS_CNK_MODEL draw_760_model = {
#include "stg30/terrain/draw_760_model.inc"
};
static NJS_CNK_OBJECT draw_760 = {
#include "stg30/terrain/draw_760.inc"
};
static Sint16 draw_12_polygons[271] = {
#include "stg30/terrain/draw_12_polygons.inc"
};
static Sint32 draw_12_vertices[303] = {
#include "stg30/terrain/draw_12_vertices.inc"
};
static NJS_CNK_MODEL draw_12_model = {
#include "stg30/terrain/draw_12_model.inc"
};
static NJS_CNK_OBJECT draw_732 = {
#include "stg30/terrain/draw_732.inc"
};
static Sint16 draw_447_polygons[551] = {
#include "stg30/terrain/draw_447_polygons.inc"
};
static Sint32 draw_447_vertices[415] = {
#include "stg30/terrain/draw_447_vertices.inc"
};
static NJS_CNK_MODEL draw_447_model = {
#include "stg30/terrain/draw_447_model.inc"
};
static NJS_CNK_OBJECT draw_731 = {
#include "stg30/terrain/draw_731.inc"
};
static Sint16 draw_19_polygons[271] = {
#include "stg30/terrain/draw_19_polygons.inc"
};
static Sint32 draw_19_vertices[279] = {
#include "stg30/terrain/draw_19_vertices.inc"
};
static NJS_CNK_MODEL draw_19_model = {
#include "stg30/terrain/draw_19_model.inc"
};
static NJS_CNK_OBJECT draw_730 = {
#include "stg30/terrain/draw_730.inc"
};
static Sint16 draw_728_polygons[193] = {
#include "stg30/terrain/draw_728_polygons.inc"
};
static Sint32 draw_728_vertices[135] = {
#include "stg30/terrain/draw_728_vertices.inc"
};
static NJS_CNK_MODEL draw_728_model = {
#include "stg30/terrain/draw_728_model.inc"
};
static NJS_CNK_OBJECT draw_728 = {
#include "stg30/terrain/draw_728.inc"
};
static Sint16 draw_723_polygons[93] = {
#include "stg30/terrain/draw_723_polygons.inc"
};
static Sint32 draw_723_vertices[75] = {
#include "stg30/terrain/draw_723_vertices.inc"
};
static NJS_CNK_MODEL draw_723_model = {
#include "stg30/terrain/draw_723_model.inc"
};
static NJS_CNK_OBJECT draw_725 = {
#include "stg30/terrain/draw_725.inc"
};
static Sint16 draw_717_polygons[93] = {
#include "stg30/terrain/draw_717_polygons.inc"
};
static Sint32 draw_717_vertices[75] = {
#include "stg30/terrain/draw_717_vertices.inc"
};
static NJS_CNK_MODEL draw_717_model = {
#include "stg30/terrain/draw_717_model.inc"
};
static NJS_CNK_OBJECT draw_722 = {
#include "stg30/terrain/draw_722.inc"
};
static Sint16 draw_721_polygons[187] = {
#include "stg30/terrain/draw_721_polygons.inc"
};
static Sint32 draw_721_vertices[143] = {
#include "stg30/terrain/draw_721_vertices.inc"
};
static NJS_CNK_MODEL draw_721_model = {
#include "stg30/terrain/draw_721_model.inc"
};
static NJS_CNK_OBJECT draw_721 = {
#include "stg30/terrain/draw_721.inc"
};
static Sint16 draw_720_polygons[229] = {
#include "stg30/terrain/draw_720_polygons.inc"
};
static Sint32 draw_720_vertices[227] = {
#include "stg30/terrain/draw_720_vertices.inc"
};
static NJS_CNK_MODEL draw_720_model = {
#include "stg30/terrain/draw_720_model.inc"
};
static NJS_CNK_OBJECT draw_720 = {
#include "stg30/terrain/draw_720.inc"
};
static Sint16 draw_719_polygons[103] = {
#include "stg30/terrain/draw_719_polygons.inc"
};
static Sint32 draw_719_vertices[99] = {
#include "stg30/terrain/draw_719_vertices.inc"
};
static NJS_CNK_MODEL draw_719_model = {
#include "stg30/terrain/draw_719_model.inc"
};
static NJS_CNK_OBJECT draw_719 = {
#include "stg30/terrain/draw_719.inc"
};
static Sint16 draw_716_polygons[69] = {
#include "stg30/terrain/draw_716_polygons.inc"
};
static Sint32 draw_716_vertices[51] = {
#include "stg30/terrain/draw_716_vertices.inc"
};
static NJS_CNK_MODEL draw_716_model = {
#include "stg30/terrain/draw_716_model.inc"
};
static NJS_CNK_OBJECT draw_716 = {
#include "stg30/terrain/draw_716.inc"
};
static Sint16 draw_715_polygons[211] = {
#include "stg30/terrain/draw_715_polygons.inc"
};
static Sint32 draw_715_vertices[135] = {
#include "stg30/terrain/draw_715_vertices.inc"
};
static NJS_CNK_MODEL draw_715_model = {
#include "stg30/terrain/draw_715_model.inc"
};
static NJS_CNK_OBJECT draw_715 = {
#include "stg30/terrain/draw_715.inc"
};
static Sint16 draw_714_polygons[69] = {
#include "stg30/terrain/draw_714_polygons.inc"
};
static Sint32 draw_714_vertices[51] = {
#include "stg30/terrain/draw_714_vertices.inc"
};
static NJS_CNK_MODEL draw_714_model = {
#include "stg30/terrain/draw_714_model.inc"
};
static NJS_CNK_OBJECT draw_714 = {
#include "stg30/terrain/draw_714.inc"
};
static Sint16 draw_712_polygons[271] = {
#include "stg30/terrain/draw_712_polygons.inc"
};
static Sint32 draw_712_vertices[263] = {
#include "stg30/terrain/draw_712_vertices.inc"
};
static NJS_CNK_MODEL draw_712_model = {
#include "stg30/terrain/draw_712_model.inc"
};
static NJS_CNK_OBJECT draw_713 = {
#include "stg30/terrain/draw_713.inc"
};
static Sint16 draw_711_polygons[271] = {
#include "stg30/terrain/draw_711_polygons.inc"
};
static Sint32 draw_711_vertices[311] = {
#include "stg30/terrain/draw_711_vertices.inc"
};
static NJS_CNK_MODEL draw_711_model = {
#include "stg30/terrain/draw_711_model.inc"
};
static NJS_CNK_OBJECT draw_711 = {
#include "stg30/terrain/draw_711.inc"
};
static Sint16 draw_710_polygons[271] = {
#include "stg30/terrain/draw_710_polygons.inc"
};
static Sint32 draw_710_vertices[303] = {
#include "stg30/terrain/draw_710_vertices.inc"
};
static NJS_CNK_MODEL draw_710_model = {
#include "stg30/terrain/draw_710_model.inc"
};
static NJS_CNK_OBJECT draw_710 = {
#include "stg30/terrain/draw_710.inc"
};
static Sint16 draw_18_polygons[155] = {
#include "stg30/terrain/draw_18_polygons.inc"
};
static Sint32 draw_18_vertices[115] = {
#include "stg30/terrain/draw_18_vertices.inc"
};
static NJS_CNK_MODEL draw_18_model = {
#include "stg30/terrain/draw_18_model.inc"
};
static NJS_CNK_OBJECT draw_591 = {
#include "stg30/terrain/draw_591.inc"
};
static Sint16 draw_16_polygons[141] = {
#include "stg30/terrain/draw_16_polygons.inc"
};
static Sint32 draw_16_vertices[99] = {
#include "stg30/terrain/draw_16_vertices.inc"
};
static NJS_CNK_MODEL draw_16_model = {
#include "stg30/terrain/draw_16_model.inc"
};
static NJS_CNK_OBJECT draw_588 = {
#include "stg30/terrain/draw_588.inc"
};
static Sint16 draw_65_polygons[515] = {
#include "stg30/terrain/draw_65_polygons.inc"
};
static Sint32 draw_65_vertices[427] = {
#include "stg30/terrain/draw_65_vertices.inc"
};
static NJS_CNK_MODEL draw_65_model = {
#include "stg30/terrain/draw_65_model.inc"
};
static NJS_CNK_OBJECT draw_587 = {
#include "stg30/terrain/draw_587.inc"
};
static Sint16 draw_62_polygons[735] = {
#include "stg30/terrain/draw_62_polygons.inc"
};
static Sint32 draw_62_vertices[479] = {
#include "stg30/terrain/draw_62_vertices.inc"
};
static NJS_CNK_MODEL draw_62_model = {
#include "stg30/terrain/draw_62_model.inc"
};
static NJS_CNK_OBJECT draw_586 = {
#include "stg30/terrain/draw_586.inc"
};
static Sint16 draw_58_polygons[55] = {
#include "stg30/terrain/draw_58_polygons.inc"
};
static Sint32 draw_58_vertices[51] = {
#include "stg30/terrain/draw_58_vertices.inc"
};
static NJS_CNK_MODEL draw_58_model = {
#include "stg30/terrain/draw_58_model.inc"
};
static NJS_CNK_OBJECT draw_585 = {
#include "stg30/terrain/draw_585.inc"
};
static Sint16 draw_15_polygons[183] = {
#include "stg30/terrain/draw_15_polygons.inc"
};
static Sint32 draw_15_vertices[123] = {
#include "stg30/terrain/draw_15_vertices.inc"
};
static NJS_CNK_MODEL draw_15_model = {
#include "stg30/terrain/draw_15_model.inc"
};
static NJS_CNK_OBJECT draw_584 = {
#include "stg30/terrain/draw_584.inc"
};
static Sint16 draw_581_polygons[173] = {
#include "stg30/terrain/draw_581_polygons.inc"
};
static Sint32 draw_581_vertices[83] = {
#include "stg30/terrain/draw_581_vertices.inc"
};
static NJS_CNK_MODEL draw_581_model = {
#include "stg30/terrain/draw_581_model.inc"
};
static NJS_CNK_OBJECT draw_581 = {
#include "stg30/terrain/draw_581.inc"
};
static Sint16 draw_14_polygons[133] = {
#include "stg30/terrain/draw_14_polygons.inc"
};
static Sint32 draw_14_vertices[83] = {
#include "stg30/terrain/draw_14_vertices.inc"
};
static NJS_CNK_MODEL draw_14_model = {
#include "stg30/terrain/draw_14_model.inc"
};
static NJS_CNK_OBJECT draw_578 = {
#include "stg30/terrain/draw_578.inc"
};
static Sint16 draw_10_polygons[157] = {
#include "stg30/terrain/draw_10_polygons.inc"
};
static Sint32 draw_10_vertices[107] = {
#include "stg30/terrain/draw_10_vertices.inc"
};
static NJS_CNK_MODEL draw_10_model = {
#include "stg30/terrain/draw_10_model.inc"
};
static NJS_CNK_OBJECT draw_577 = {
#include "stg30/terrain/draw_577.inc"
};
static Sint16 draw_473_polygons[277] = {
#include "stg30/terrain/draw_473_polygons.inc"
};
static Sint32 draw_473_vertices[247] = {
#include "stg30/terrain/draw_473_vertices.inc"
};
static NJS_CNK_MODEL draw_473_model = {
#include "stg30/terrain/draw_473_model.inc"
};
static NJS_CNK_OBJECT draw_575 = {
#include "stg30/terrain/draw_575.inc"
};
static Sint16 draw_470_polygons[41] = {
#include "stg30/terrain/draw_470_polygons.inc"
};
static Sint32 draw_470_vertices[35] = {
#include "stg30/terrain/draw_470_vertices.inc"
};
static NJS_CNK_MODEL draw_470_model = {
#include "stg30/terrain/draw_470_model.inc"
};
static NJS_CNK_OBJECT draw_574 = {
#include "stg30/terrain/draw_574.inc"
};
static Sint16 draw_481_polygons[59] = {
#include "stg30/terrain/draw_481_polygons.inc"
};
static Sint32 draw_481_vertices[51] = {
#include "stg30/terrain/draw_481_vertices.inc"
};
static NJS_CNK_MODEL draw_481_model = {
#include "stg30/terrain/draw_481_model.inc"
};
static NJS_CNK_OBJECT draw_570 = {
#include "stg30/terrain/draw_570.inc"
};
static Sint16 draw_569_polygons[145] = {
#include "stg30/terrain/draw_569_polygons.inc"
};
static Sint32 draw_569_vertices[99] = {
#include "stg30/terrain/draw_569_vertices.inc"
};
static NJS_CNK_MODEL draw_569_model = {
#include "stg30/terrain/draw_569_model.inc"
};
static NJS_CNK_OBJECT draw_569 = {
#include "stg30/terrain/draw_569.inc"
};
static Sint16 draw_556_polygons[45] = {
#include "stg30/terrain/draw_556_polygons.inc"
};
static Sint32 draw_556_vertices[31] = {
#include "stg30/terrain/draw_556_vertices.inc"
};
static NJS_CNK_MODEL draw_556_model = {
#include "stg30/terrain/draw_556_model.inc"
};
static NJS_CNK_OBJECT draw_556 = {
#include "stg30/terrain/draw_556.inc"
};
static Sint16 draw_456_polygons[91] = {
#include "stg30/terrain/draw_456_polygons.inc"
};
static Sint32 draw_456_vertices[83] = {
#include "stg30/terrain/draw_456_vertices.inc"
};
static NJS_CNK_MODEL draw_456_model = {
#include "stg30/terrain/draw_456_model.inc"
};
static NJS_CNK_OBJECT draw_456 = {
#include "stg30/terrain/draw_456.inc"
};
static Sint16 draw_281_polygons[59] = {
#include "stg30/terrain/draw_281_polygons.inc"
};
static Sint32 draw_281_vertices[43] = {
#include "stg30/terrain/draw_281_vertices.inc"
};
static NJS_CNK_MODEL draw_281_model = {
#include "stg30/terrain/draw_281_model.inc"
};
static NJS_CNK_OBJECT draw_426 = {
#include "stg30/terrain/draw_426.inc"
};
static Sint16 draw_276_polygons[307] = {
#include "stg30/terrain/draw_276_polygons.inc"
};
static Sint32 draw_276_vertices[199] = {
#include "stg30/terrain/draw_276_vertices.inc"
};
static NJS_CNK_MODEL draw_276_model = {
#include "stg30/terrain/draw_276_model.inc"
};
static NJS_CNK_OBJECT draw_425 = {
#include "stg30/terrain/draw_425.inc"
};
static Sint16 draw_251_polygons[301] = {
#include "stg30/terrain/draw_251_polygons.inc"
};
static Sint32 draw_251_vertices[195] = {
#include "stg30/terrain/draw_251_vertices.inc"
};
static NJS_CNK_MODEL draw_251_model = {
#include "stg30/terrain/draw_251_model.inc"
};
static NJS_CNK_OBJECT draw_424 = {
#include "stg30/terrain/draw_424.inc"
};
static Sint16 draw_342_polygons[451] = {
#include "stg30/terrain/draw_342_polygons.inc"
};
static Sint32 draw_342_vertices[327] = {
#include "stg30/terrain/draw_342_vertices.inc"
};
static NJS_CNK_MODEL draw_342_model = {
#include "stg30/terrain/draw_342_model.inc"
};
static NJS_CNK_OBJECT draw_423 = {
#include "stg30/terrain/draw_423.inc"
};
static Sint16 draw_293_polygons[519] = {
#include "stg30/terrain/draw_293_polygons.inc"
};
static Sint32 draw_293_vertices[419] = {
#include "stg30/terrain/draw_293_vertices.inc"
};
static NJS_CNK_MODEL draw_293_model = {
#include "stg30/terrain/draw_293_model.inc"
};
static NJS_CNK_OBJECT draw_421 = {
#include "stg30/terrain/draw_421.inc"
};
static Sint16 draw_287_polygons[59] = {
#include "stg30/terrain/draw_287_polygons.inc"
};
static Sint32 draw_287_vertices[43] = {
#include "stg30/terrain/draw_287_vertices.inc"
};
static NJS_CNK_MODEL draw_287_model = {
#include "stg30/terrain/draw_287_model.inc"
};
static NJS_CNK_OBJECT draw_397 = {
#include "stg30/terrain/draw_397.inc"
};
static Sint16 draw_269_polygons[519] = {
#include "stg30/terrain/draw_269_polygons.inc"
};
static Sint32 draw_269_vertices[431] = {
#include "stg30/terrain/draw_269_vertices.inc"
};
static NJS_CNK_MODEL draw_269_model = {
#include "stg30/terrain/draw_269_model.inc"
};
static NJS_CNK_OBJECT draw_394 = {
#include "stg30/terrain/draw_394.inc"
};
static Sint16 draw_272_polygons[55] = {
#include "stg30/terrain/draw_272_polygons.inc"
};
static Sint32 draw_272_vertices[35] = {
#include "stg30/terrain/draw_272_vertices.inc"
};
static NJS_CNK_MODEL draw_272_model = {
#include "stg30/terrain/draw_272_model.inc"
};
static NJS_CNK_OBJECT draw_362 = {
#include "stg30/terrain/draw_362.inc"
};
static Sint16 draw_361_polygons[77] = {
#include "stg30/terrain/draw_361_polygons.inc"
};
static Sint32 draw_361_vertices[63] = {
#include "stg30/terrain/draw_361_vertices.inc"
};
static NJS_CNK_MODEL draw_361_model = {
#include "stg30/terrain/draw_361_model.inc"
};
static NJS_CNK_OBJECT draw_361 = {
#include "stg30/terrain/draw_361.inc"
};
static Sint16 draw_239_polygons[511] = {
#include "stg30/terrain/draw_239_polygons.inc"
};
static Sint32 draw_239_vertices[423] = {
#include "stg30/terrain/draw_239_vertices.inc"
};
static NJS_CNK_MODEL draw_239_model = {
#include "stg30/terrain/draw_239_model.inc"
};
static NJS_CNK_OBJECT draw_360 = {
#include "stg30/terrain/draw_360.inc"
};
static Sint16 draw_264_polygons[21] = {
#include "stg30/terrain/draw_264_polygons.inc"
};
static Sint32 draw_264_vertices[51] = {
#include "stg30/terrain/draw_264_vertices.inc"
};
static NJS_CNK_MODEL draw_264_model = {
#include "stg30/terrain/draw_264_model.inc"
};
static NJS_CNK_OBJECT draw_264 = {
#include "stg30/terrain/draw_264.inc"
};
static Sint16 draw_240_polygons[445] = {
#include "stg30/terrain/draw_240_polygons.inc"
};
static Sint32 draw_240_vertices[391] = {
#include "stg30/terrain/draw_240_vertices.inc"
};
static NJS_CNK_MODEL draw_240_model = {
#include "stg30/terrain/draw_240_model.inc"
};
static NJS_CNK_OBJECT draw_240 = {
#include "stg30/terrain/draw_240.inc"
};
static Sint16 draw_23_polygons[579] = {
#include "stg30/terrain/draw_23_polygons.inc"
};
static Sint32 draw_23_vertices[427] = {
#include "stg30/terrain/draw_23_vertices.inc"
};
static NJS_CNK_MODEL draw_23_model = {
#include "stg30/terrain/draw_23_model.inc"
};
static NJS_CNK_OBJECT draw_66 = {
#include "stg30/terrain/draw_66.inc"
};
static NJS_OBJECT collision_2 = {
#include "stg30/terrain/collision_2.inc"
};
static NJS_OBJECT collision_3 = {
#include "stg30/terrain/collision_3.inc"
};
static NJS_OBJECT collision_4 = {
#include "stg30/terrain/collision_4.inc"
};
static NJS_OBJECT collision_8 = {
#include "stg30/terrain/collision_8.inc"
};
static NJS_OBJECT collision_9 = {
#include "stg30/terrain/collision_9.inc"
};
static NJS_OBJECT collision_12 = {
#include "stg30/terrain/collision_12.inc"
};
static NJS_OBJECT collision_13 = {
#include "stg30/terrain/collision_13.inc"
};
static NJS_OBJECT collision_15 = {
#include "stg30/terrain/collision_15.inc"
};
static NJS_OBJECT collision_16 = {
#include "stg30/terrain/collision_16.inc"
};
static NJS_OBJECT collision_17 = {
#include "stg30/terrain/collision_17.inc"
};
static NJS_OBJECT collision_18 = {
#include "stg30/terrain/collision_18.inc"
};
static NJS_OBJECT collision_22 = {
#include "stg30/terrain/collision_22.inc"
};
static NJS_OBJECT collision_23 = {
#include "stg30/terrain/collision_23.inc"
};
static NJS_OBJECT collision_24 = {
#include "stg30/terrain/collision_24.inc"
};
static NJS_OBJECT collision_26 = {
#include "stg30/terrain/collision_26.inc"
};
static NJS_OBJECT collision_27 = {
#include "stg30/terrain/collision_27.inc"
};
static NJS_OBJECT collision_28 = {
#include "stg30/terrain/collision_28.inc"
};
static NJS_OBJECT collision_29 = {
#include "stg30/terrain/collision_29.inc"
};
static NJS_OBJECT collision_30 = {
#include "stg30/terrain/collision_30.inc"
};
static NJS_OBJECT collision_31 = {
#include "stg30/terrain/collision_31.inc"
};
static NJS_OBJECT collision_32 = {
#include "stg30/terrain/collision_32.inc"
};
static NJS_OBJECT collision_33 = {
#include "stg30/terrain/collision_33.inc"
};
static NJS_OBJECT collision_34 = {
#include "stg30/terrain/collision_34.inc"
};
static NJS_OBJECT collision_35 = {
#include "stg30/terrain/collision_35.inc"
};
static NJS_OBJECT collision_36 = {
#include "stg30/terrain/collision_36.inc"
};
static NJS_OBJECT collision_37 = {
#include "stg30/terrain/collision_37.inc"
};
static NJS_OBJECT collision_38 = {
#include "stg30/terrain/collision_38.inc"
};
static NJS_OBJECT collision_39 = {
#include "stg30/terrain/collision_39.inc"
};
static NJS_OBJECT collision_40 = {
#include "stg30/terrain/collision_40.inc"
};
static NJS_OBJECT collision_41 = {
#include "stg30/terrain/collision_41.inc"
};
static NJS_OBJECT collision_42 = {
#include "stg30/terrain/collision_42.inc"
};
static NJS_OBJECT collision_43 = {
#include "stg30/terrain/collision_43.inc"
};
static NJS_OBJECT collision_44 = {
#include "stg30/terrain/collision_44.inc"
};
static NJS_OBJECT collision_45 = {
#include "stg30/terrain/collision_45.inc"
};
static NJS_OBJECT collision_46 = {
#include "stg30/terrain/collision_46.inc"
};
static NJS_OBJECT collision_47 = {
#include "stg30/terrain/collision_47.inc"
};
static NJS_OBJECT collision_48 = {
#include "stg30/terrain/collision_48.inc"
};
static NJS_OBJECT collision_49 = {
#include "stg30/terrain/collision_49.inc"
};
static NJS_OBJECT collision_50 = {
#include "stg30/terrain/collision_50.inc"
};
static NJS_OBJECT collision_51 = {
#include "stg30/terrain/collision_51.inc"
};
static NJS_OBJECT collision_52 = {
#include "stg30/terrain/collision_52.inc"
};
static NJS_OBJECT collision_53 = {
#include "stg30/terrain/collision_53.inc"
};
static NJS_OBJECT collision_54 = {
#include "stg30/terrain/collision_54.inc"
};
static NJS_OBJECT collision_55 = {
#include "stg30/terrain/collision_55.inc"
};
static NJS_OBJECT collision_56 = {
#include "stg30/terrain/collision_56.inc"
};
static NJS_OBJECT collision_57 = {
#include "stg30/terrain/collision_57.inc"
};
static NJS_OBJECT collision_58 = {
#include "stg30/terrain/collision_58.inc"
};
static NJS_OBJECT collision_59 = {
#include "stg30/terrain/collision_59.inc"
};
static NJS_OBJECT collision_60 = {
#include "stg30/terrain/collision_60.inc"
};
static NJS_OBJECT collision_61 = {
#include "stg30/terrain/collision_61.inc"
};
static NJS_OBJECT collision_62 = {
#include "stg30/terrain/collision_62.inc"
};
static NJS_OBJECT collision_63 = {
#include "stg30/terrain/collision_63.inc"
};
static NJS_OBJECT collision_64 = {
#include "stg30/terrain/collision_64.inc"
};
static NJS_OBJECT collision_65 = {
#include "stg30/terrain/collision_65.inc"
};
static NJS_OBJECT collision_66 = {
#include "stg30/terrain/collision_66.inc"
};
static NJS_OBJECT collision_67 = {
#include "stg30/terrain/collision_67.inc"
};
static NJS_OBJECT collision_68 = {
#include "stg30/terrain/collision_68.inc"
};
static NJS_OBJECT collision_69 = {
#include "stg30/terrain/collision_69.inc"
};
static NJS_OBJECT collision_70 = {
#include "stg30/terrain/collision_70.inc"
};
static NJS_OBJECT collision_71 = {
#include "stg30/terrain/collision_71.inc"
};
static NJS_OBJECT collision_72 = {
#include "stg30/terrain/collision_72.inc"
};
static NJS_OBJECT collision_73 = {
#include "stg30/terrain/collision_73.inc"
};
static NJS_OBJECT collision_74 = {
#include "stg30/terrain/collision_74.inc"
};
static NJS_OBJECT collision_75 = {
#include "stg30/terrain/collision_75.inc"
};
static NJS_OBJECT collision_76 = {
#include "stg30/terrain/collision_76.inc"
};
static NJS_OBJECT collision_77 = {
#include "stg30/terrain/collision_77.inc"
};
static NJS_OBJECT collision_78 = {
#include "stg30/terrain/collision_78.inc"
};
static NJS_OBJECT collision_79 = {
#include "stg30/terrain/collision_79.inc"
};
static NJS_OBJECT collision_80 = {
#include "stg30/terrain/collision_80.inc"
};
static NJS_OBJECT collision_81 = {
#include "stg30/terrain/collision_81.inc"
};
static NJS_OBJECT collision_82 = {
#include "stg30/terrain/collision_82.inc"
};
static NJS_OBJECT collision_83 = {
#include "stg30/terrain/collision_83.inc"
};
static NJS_OBJECT collision_84 = {
#include "stg30/terrain/collision_84.inc"
};
static NJS_OBJECT collision_85 = {
#include "stg30/terrain/collision_85.inc"
};
static NJS_OBJECT collision_86 = {
#include "stg30/terrain/collision_86.inc"
};
static NJS_OBJECT collision_87 = {
#include "stg30/terrain/collision_87.inc"
};
static NJS_OBJECT collision_88 = {
#include "stg30/terrain/collision_88.inc"
};
static NJS_OBJECT collision_89 = {
#include "stg30/terrain/collision_89.inc"
};
static NJS_OBJECT collision_90 = {
#include "stg30/terrain/collision_90.inc"
};
static NJS_OBJECT collision_91 = {
#include "stg30/terrain/collision_91.inc"
};
static NJS_OBJECT collision_92 = {
#include "stg30/terrain/collision_92.inc"
};
static NJS_OBJECT collision_93 = {
#include "stg30/terrain/collision_93.inc"
};
static NJS_OBJECT collision_94 = {
#include "stg30/terrain/collision_94.inc"
};
static NJS_OBJECT collision_96 = {
#include "stg30/terrain/collision_96.inc"
};
static NJS_OBJECT collision_97 = {
#include "stg30/terrain/collision_97.inc"
};
static NJS_OBJECT collision_99 = {
#include "stg30/terrain/collision_99.inc"
};
static NJS_OBJECT collision_100 = {
#include "stg30/terrain/collision_100.inc"
};
static NJS_OBJECT collision_101 = {
#include "stg30/terrain/collision_101.inc"
};
static NJS_OBJECT collision_102 = {
#include "stg30/terrain/collision_102.inc"
};
static NJS_OBJECT collision_103 = {
#include "stg30/terrain/collision_103.inc"
};
static NJS_OBJECT collision_107 = {
#include "stg30/terrain/collision_107.inc"
};
static NJS_OBJECT collision_108 = {
#include "stg30/terrain/collision_108.inc"
};
static NJS_OBJECT collision_109 = {
#include "stg30/terrain/collision_109.inc"
};
static NJS_OBJECT collision_114 = {
#include "stg30/terrain/collision_114.inc"
};
static NJS_OBJECT collision_116 = {
#include "stg30/terrain/collision_116.inc"
};
static NJS_OBJECT collision_117 = {
#include "stg30/terrain/collision_117.inc"
};
static NJS_OBJECT collision_118 = {
#include "stg30/terrain/collision_118.inc"
};
static NJS_OBJECT collision_123 = {
#include "stg30/terrain/collision_123.inc"
};
static NJS_OBJECT collision_124 = {
#include "stg30/terrain/collision_124.inc"
};
static NJS_OBJECT collision_126 = {
#include "stg30/terrain/collision_126.inc"
};
static NJS_OBJECT collision_128 = {
#include "stg30/terrain/collision_128.inc"
};
static NJS_OBJECT collision_130 = {
#include "stg30/terrain/collision_130.inc"
};
static NJS_OBJECT collision_131 = {
#include "stg30/terrain/collision_131.inc"
};
static NJS_OBJECT collision_132 = {
#include "stg30/terrain/collision_132.inc"
};
static NJS_OBJECT collision_134 = {
#include "stg30/terrain/collision_134.inc"
};
static NJS_OBJECT collision_135 = {
#include "stg30/terrain/collision_135.inc"
};
static NJS_OBJECT collision_136 = {
#include "stg30/terrain/collision_136.inc"
};
static NJS_OBJECT collision_137 = {
#include "stg30/terrain/collision_137.inc"
};
static NJS_OBJECT collision_138 = {
#include "stg30/terrain/collision_138.inc"
};
static NJS_OBJECT collision_139 = {
#include "stg30/terrain/collision_139.inc"
};
static NJS_OBJECT collision_140 = {
#include "stg30/terrain/collision_140.inc"
};
static NJS_OBJECT collision_141 = {
#include "stg30/terrain/collision_141.inc"
};
static NJS_OBJECT collision_142 = {
#include "stg30/terrain/collision_142.inc"
};
static NJS_OBJECT collision_143 = {
#include "stg30/terrain/collision_143.inc"
};
static NJS_OBJECT collision_145 = {
#include "stg30/terrain/collision_145.inc"
};
static NJS_OBJECT collision_147 = {
#include "stg30/terrain/collision_147.inc"
};
static NJS_OBJECT collision_149 = {
#include "stg30/terrain/collision_149.inc"
};
static NJS_OBJECT collision_161 = {
#include "stg30/terrain/collision_161.inc"
};
static NJS_OBJECT collision_162 = {
#include "stg30/terrain/collision_162.inc"
};
static NJS_OBJECT collision_164 = {
#include "stg30/terrain/collision_164.inc"
};
static NJS_OBJECT collision_165 = {
#include "stg30/terrain/collision_165.inc"
};
static NJS_OBJECT collision_170 = {
#include "stg30/terrain/collision_170.inc"
};
static NJS_OBJECT collision_171 = {
#include "stg30/terrain/collision_171.inc"
};
static NJS_OBJECT collision_173 = {
#include "stg30/terrain/collision_173.inc"
};
static NJS_OBJECT collision_280 = {
#include "stg30/terrain/collision_280.inc"
};
static NJS_OBJECT collision_281 = {
#include "stg30/terrain/collision_281.inc"
};
static NJS_OBJECT collision_282 = {
#include "stg30/terrain/collision_282.inc"
};
static NJS_OBJECT collision_283 = {
#include "stg30/terrain/collision_283.inc"
};
static NJS_OBJECT collision_284 = {
#include "stg30/terrain/collision_284.inc"
};
static NJS_OBJECT collision_285 = {
#include "stg30/terrain/collision_285.inc"
};
static NJS_OBJECT collision_286 = {
#include "stg30/terrain/collision_286.inc"
};
static NJS_OBJECT collision_287 = {
#include "stg30/terrain/collision_287.inc"
};
static NJS_OBJECT collision_288 = {
#include "stg30/terrain/collision_288.inc"
};
static NJS_OBJECT collision_289 = {
#include "stg30/terrain/collision_289.inc"
};
static NJS_OBJECT collision_290 = {
#include "stg30/terrain/collision_290.inc"
};
static NJS_OBJECT collision_291 = {
#include "stg30/terrain/collision_291.inc"
};
static NJS_OBJECT collision_292 = {
#include "stg30/terrain/collision_292.inc"
};
static NJS_OBJECT collision_293 = {
#include "stg30/terrain/collision_293.inc"
};
static NJS_OBJECT collision_294 = {
#include "stg30/terrain/collision_294.inc"
};
static NJS_OBJECT collision_295 = {
#include "stg30/terrain/collision_295.inc"
};
static NJS_OBJECT collision_296 = {
#include "stg30/terrain/collision_296.inc"
};
static NJS_OBJECT collision_297 = {
#include "stg30/terrain/collision_297.inc"
};
static NJS_OBJECT collision_298 = {
#include "stg30/terrain/collision_298.inc"
};
static NJS_OBJECT collision_299 = {
#include "stg30/terrain/collision_299.inc"
};
static NJS_OBJECT collision_300 = {
#include "stg30/terrain/collision_300.inc"
};
static NJS_OBJECT collision_301 = {
#include "stg30/terrain/collision_301.inc"
};
static NJS_OBJECT collision_302 = {
#include "stg30/terrain/collision_302.inc"
};
static NJS_OBJECT collision_303 = {
#include "stg30/terrain/collision_303.inc"
};
static NJS_OBJECT collision_304 = {
#include "stg30/terrain/collision_304.inc"
};
static NJS_OBJECT collision_305 = {
#include "stg30/terrain/collision_305.inc"
};
static NJS_OBJECT collision_320 = {
#include "stg30/terrain/collision_320.inc"
};
static NJS_OBJECT collision_321 = {
#include "stg30/terrain/collision_321.inc"
};
static NJS_OBJECT collision_322 = {
#include "stg30/terrain/collision_322.inc"
};
static NJS_OBJECT collision_323 = {
#include "stg30/terrain/collision_323.inc"
};
static NJS_OBJECT collision_326 = {
#include "stg30/terrain/collision_326.inc"
};
static NJS_OBJECT collision_328 = {
#include "stg30/terrain/collision_328.inc"
};
static NJS_OBJECT collision_329 = {
#include "stg30/terrain/collision_329.inc"
};
static NJS_OBJECT collision_330 = {
#include "stg30/terrain/collision_330.inc"
};
static NJS_OBJECT collision_331 = {
#include "stg30/terrain/collision_331.inc"
};
static NJS_OBJECT collision_332 = {
#include "stg30/terrain/collision_332.inc"
};
static NJS_OBJECT collision_333 = {
#include "stg30/terrain/collision_333.inc"
};
static NJS_OBJECT collision_334 = {
#include "stg30/terrain/collision_334.inc"
};
static NJS_OBJECT collision_337 = {
#include "stg30/terrain/collision_337.inc"
};
static NJS_OBJECT collision_338 = {
#include "stg30/terrain/collision_338.inc"
};
static NJS_OBJECT collision_339 = {
#include "stg30/terrain/collision_339.inc"
};
static NJS_OBJECT collision_341 = {
#include "stg30/terrain/collision_341.inc"
};
static NJS_OBJECT collision_342 = {
#include "stg30/terrain/collision_342.inc"
};
static NJS_OBJECT collision_343 = {
#include "stg30/terrain/collision_343.inc"
};
static NJS_OBJECT collision_345 = {
#include "stg30/terrain/collision_345.inc"
};
static NJS_OBJECT collision_346 = {
#include "stg30/terrain/collision_346.inc"
};
static NJS_OBJECT collision_347 = {
#include "stg30/terrain/collision_347.inc"
};
static NJS_OBJECT collision_349 = {
#include "stg30/terrain/collision_349.inc"
};
static NJS_OBJECT collision_350 = {
#include "stg30/terrain/collision_350.inc"
};
static NJS_OBJECT collision_351 = {
#include "stg30/terrain/collision_351.inc"
};
static NJS_OBJECT collision_353 = {
#include "stg30/terrain/collision_353.inc"
};
static NJS_OBJECT collision_354 = {
#include "stg30/terrain/collision_354.inc"
};
static NJS_OBJECT collision_356 = {
#include "stg30/terrain/collision_356.inc"
};
static NJS_OBJECT collision_357 = {
#include "stg30/terrain/collision_357.inc"
};
static NJS_OBJECT collision_358 = {
#include "stg30/terrain/collision_358.inc"
};
static NJS_OBJECT collision_359 = {
#include "stg30/terrain/collision_359.inc"
};
static NJS_OBJECT collision_360 = {
#include "stg30/terrain/collision_360.inc"
};
static NJS_OBJECT collision_361 = {
#include "stg30/terrain/collision_361.inc"
};
static NJS_OBJECT collision_362 = {
#include "stg30/terrain/collision_362.inc"
};
static NJS_OBJECT collision_363 = {
#include "stg30/terrain/collision_363.inc"
};
static NJS_OBJECT collision_364 = {
#include "stg30/terrain/collision_364.inc"
};
static NJS_OBJECT collision_365 = {
#include "stg30/terrain/collision_365.inc"
};
static NJS_OBJECT collision_366 = {
#include "stg30/terrain/collision_366.inc"
};
static NJS_OBJECT collision_367 = {
#include "stg30/terrain/collision_367.inc"
};
static NJS_OBJECT collision_368 = {
#include "stg30/terrain/collision_368.inc"
};
static NJS_OBJECT collision_369 = {
#include "stg30/terrain/collision_369.inc"
};
static NJS_OBJECT collision_370 = {
#include "stg30/terrain/collision_370.inc"
};
static NJS_OBJECT collision_371 = {
#include "stg30/terrain/collision_371.inc"
};
static NJS_OBJECT collision_372 = {
#include "stg30/terrain/collision_372.inc"
};
static NJS_OBJECT collision_373 = {
#include "stg30/terrain/collision_373.inc"
};
static NJS_OBJECT collision_374 = {
#include "stg30/terrain/collision_374.inc"
};
static NJS_OBJECT collision_375 = {
#include "stg30/terrain/collision_375.inc"
};
static NJS_OBJECT collision_376 = {
#include "stg30/terrain/collision_376.inc"
};
static NJS_OBJECT collision_377 = {
#include "stg30/terrain/collision_377.inc"
};
static NJS_OBJECT collision_378 = {
#include "stg30/terrain/collision_378.inc"
};
static NJS_OBJECT collision_379 = {
#include "stg30/terrain/collision_379.inc"
};
static NJS_OBJECT collision_380 = {
#include "stg30/terrain/collision_380.inc"
};
static NJS_OBJECT collision_381 = {
#include "stg30/terrain/collision_381.inc"
};
static NJS_OBJECT collision_382 = {
#include "stg30/terrain/collision_382.inc"
};
static NJS_OBJECT collision_383 = {
#include "stg30/terrain/collision_383.inc"
};
static NJS_OBJECT collision_384 = {
#include "stg30/terrain/collision_384.inc"
};
static NJS_OBJECT collision_385 = {
#include "stg30/terrain/collision_385.inc"
};
static NJS_OBJECT collision_386 = {
#include "stg30/terrain/collision_386.inc"
};
static NJS_OBJECT collision_387 = {
#include "stg30/terrain/collision_387.inc"
};
static NJS_OBJECT collision_388 = {
#include "stg30/terrain/collision_388.inc"
};
static NJS_OBJECT collision_389 = {
#include "stg30/terrain/collision_389.inc"
};
static NJS_OBJECT collision_390 = {
#include "stg30/terrain/collision_390.inc"
};
static NJS_OBJECT collision_391 = {
#include "stg30/terrain/collision_391.inc"
};
static NJS_OBJECT collision_392 = {
#include "stg30/terrain/collision_392.inc"
};
static NJS_OBJECT collision_393 = {
#include "stg30/terrain/collision_393.inc"
};
static NJS_OBJECT collision_394 = {
#include "stg30/terrain/collision_394.inc"
};
static NJS_OBJECT collision_395 = {
#include "stg30/terrain/collision_395.inc"
};
static NJS_OBJECT collision_396 = {
#include "stg30/terrain/collision_396.inc"
};
static NJS_OBJECT collision_397 = {
#include "stg30/terrain/collision_397.inc"
};
static NJS_OBJECT collision_398 = {
#include "stg30/terrain/collision_398.inc"
};
static NJS_OBJECT collision_399 = {
#include "stg30/terrain/collision_399.inc"
};
static NJS_OBJECT collision_400 = {
#include "stg30/terrain/collision_400.inc"
};
static NJS_OBJECT collision_401 = {
#include "stg30/terrain/collision_401.inc"
};
static NJS_OBJECT collision_402 = {
#include "stg30/terrain/collision_402.inc"
};
static NJS_OBJECT collision_403 = {
#include "stg30/terrain/collision_403.inc"
};
static NJS_OBJECT collision_404 = {
#include "stg30/terrain/collision_404.inc"
};
static NJS_OBJECT collision_405 = {
#include "stg30/terrain/collision_405.inc"
};
static NJS_OBJECT collision_406 = {
#include "stg30/terrain/collision_406.inc"
};
static NJS_OBJECT collision_407 = {
#include "stg30/terrain/collision_407.inc"
};
static NJS_OBJECT collision_408 = {
#include "stg30/terrain/collision_408.inc"
};
static NJS_OBJECT collision_409 = {
#include "stg30/terrain/collision_409.inc"
};
static NJS_OBJECT collision_410 = {
#include "stg30/terrain/collision_410.inc"
};
static NJS_OBJECT collision_411 = {
#include "stg30/terrain/collision_411.inc"
};
static NJS_OBJECT collision_412 = {
#include "stg30/terrain/collision_412.inc"
};
static NJS_OBJECT collision_413 = {
#include "stg30/terrain/collision_413.inc"
};
static NJS_OBJECT collision_414 = {
#include "stg30/terrain/collision_414.inc"
};
static NJS_OBJECT collision_415 = {
#include "stg30/terrain/collision_415.inc"
};
static NJS_OBJECT collision_416 = {
#include "stg30/terrain/collision_416.inc"
};
static NJS_OBJECT collision_417 = {
#include "stg30/terrain/collision_417.inc"
};
static NJS_OBJECT collision_419 = {
#include "stg30/terrain/collision_419.inc"
};
static NJS_OBJECT collision_421 = {
#include "stg30/terrain/collision_421.inc"
};
static NJS_OBJECT collision_422 = {
#include "stg30/terrain/collision_422.inc"
};
static NJS_OBJECT collision_424 = {
#include "stg30/terrain/collision_424.inc"
};
static NJS_OBJECT collision_426 = {
#include "stg30/terrain/collision_426.inc"
};
static NJS_OBJECT collision_427 = {
#include "stg30/terrain/collision_427.inc"
};
static NJS_OBJECT collision_429 = {
#include "stg30/terrain/collision_429.inc"
};
static NJS_OBJECT collision_431 = {
#include "stg30/terrain/collision_431.inc"
};
static NJS_OBJECT collision_433 = {
#include "stg30/terrain/collision_433.inc"
};
static NJS_OBJECT collision_435 = {
#include "stg30/terrain/collision_435.inc"
};
static NJS_OBJECT collision_436 = {
#include "stg30/terrain/collision_436.inc"
};
static NJS_OBJECT collision_437 = {
#include "stg30/terrain/collision_437.inc"
};
static NJS_OBJECT collision_438 = {
#include "stg30/terrain/collision_438.inc"
};
static NJS_OBJECT collision_439 = {
#include "stg30/terrain/collision_439.inc"
};
static NJS_OBJECT collision_440 = {
#include "stg30/terrain/collision_440.inc"
};
static NJS_OBJECT collision_441 = {
#include "stg30/terrain/collision_441.inc"
};
static NJS_OBJECT collision_442 = {
#include "stg30/terrain/collision_442.inc"
};
static NJS_OBJECT collision_443 = {
#include "stg30/terrain/collision_443.inc"
};
static NJS_OBJECT collision_444 = {
#include "stg30/terrain/collision_444.inc"
};
static NJS_OBJECT collision_445 = {
#include "stg30/terrain/collision_445.inc"
};
static NJS_OBJECT collision_446 = {
#include "stg30/terrain/collision_446.inc"
};
static NJS_OBJECT collision_447 = {
#include "stg30/terrain/collision_447.inc"
};
static NJS_OBJECT collision_448 = {
#include "stg30/terrain/collision_448.inc"
};
static NJS_OBJECT collision_449 = {
#include "stg30/terrain/collision_449.inc"
};
static NJS_OBJECT collision_450 = {
#include "stg30/terrain/collision_450.inc"
};
static NJS_OBJECT collision_451 = {
#include "stg30/terrain/collision_451.inc"
};
static NJS_OBJECT collision_452 = {
#include "stg30/terrain/collision_452.inc"
};
static NJS_OBJECT collision_453 = {
#include "stg30/terrain/collision_453.inc"
};
static NJS_OBJECT collision_454 = {
#include "stg30/terrain/collision_454.inc"
};
static NJS_OBJECT collision_455 = {
#include "stg30/terrain/collision_455.inc"
};
static NJS_OBJECT collision_456 = {
#include "stg30/terrain/collision_456.inc"
};
static NJS_OBJECT collision_457 = {
#include "stg30/terrain/collision_457.inc"
};
static NJS_OBJECT collision_458 = {
#include "stg30/terrain/collision_458.inc"
};
static NJS_OBJECT collision_459 = {
#include "stg30/terrain/collision_459.inc"
};
static NJS_OBJECT collision_460 = {
#include "stg30/terrain/collision_460.inc"
};
static NJS_OBJECT collision_461 = {
#include "stg30/terrain/collision_461.inc"
};
static NJS_OBJECT collision_462 = {
#include "stg30/terrain/collision_462.inc"
};
static NJS_OBJECT collision_463 = {
#include "stg30/terrain/collision_463.inc"
};
static NJS_OBJECT collision_464 = {
#include "stg30/terrain/collision_464.inc"
};
static NJS_OBJECT collision_465 = {
#include "stg30/terrain/collision_465.inc"
};
static NJS_OBJECT collision_466 = {
#include "stg30/terrain/collision_466.inc"
};
static NJS_OBJECT collision_467 = {
#include "stg30/terrain/collision_467.inc"
};
static NJS_OBJECT collision_468 = {
#include "stg30/terrain/collision_468.inc"
};
static NJS_OBJECT collision_469 = {
#include "stg30/terrain/collision_469.inc"
};
static NJS_OBJECT collision_470 = {
#include "stg30/terrain/collision_470.inc"
};
static NJS_OBJECT collision_472 = {
#include "stg30/terrain/collision_472.inc"
};
static NJS_OBJECT collision_473 = {
#include "stg30/terrain/collision_473.inc"
};
static NJS_OBJECT collision_474 = {
#include "stg30/terrain/collision_474.inc"
};
static NJS_OBJECT collision_475 = {
#include "stg30/terrain/collision_475.inc"
};
static NJS_OBJECT collision_476 = {
#include "stg30/terrain/collision_476.inc"
};
static NJS_OBJECT collision_477 = {
#include "stg30/terrain/collision_477.inc"
};
static NJS_OBJECT collision_478 = {
#include "stg30/terrain/collision_478.inc"
};
static NJS_OBJECT collision_479 = {
#include "stg30/terrain/collision_479.inc"
};
static NJS_OBJECT collision_480 = {
#include "stg30/terrain/collision_480.inc"
};
static NJS_OBJECT collision_481 = {
#include "stg30/terrain/collision_481.inc"
};
static NJS_OBJECT collision_482 = {
#include "stg30/terrain/collision_482.inc"
};
static NJS_OBJECT collision_483 = {
#include "stg30/terrain/collision_483.inc"
};
static NJS_OBJECT collision_484 = {
#include "stg30/terrain/collision_484.inc"
};
static NJS_OBJECT collision_485 = {
#include "stg30/terrain/collision_485.inc"
};
static NJS_OBJECT collision_486 = {
#include "stg30/terrain/collision_486.inc"
};
static NJS_OBJECT collision_487 = {
#include "stg30/terrain/collision_487.inc"
};
static NJS_OBJECT collision_488 = {
#include "stg30/terrain/collision_488.inc"
};
static NJS_OBJECT collision_489 = {
#include "stg30/terrain/collision_489.inc"
};
static NJS_OBJECT collision_490 = {
#include "stg30/terrain/collision_490.inc"
};
static NJS_OBJECT collision_491 = {
#include "stg30/terrain/collision_491.inc"
};
static NJS_OBJECT collision_492 = {
#include "stg30/terrain/collision_492.inc"
};
static NJS_OBJECT collision_493 = {
#include "stg30/terrain/collision_493.inc"
};
static NJS_OBJECT collision_494 = {
#include "stg30/terrain/collision_494.inc"
};
static NJS_OBJECT collision_495 = {
#include "stg30/terrain/collision_495.inc"
};
static NJS_OBJECT collision_496 = {
#include "stg30/terrain/collision_496.inc"
};
static NJS_OBJECT collision_497 = {
#include "stg30/terrain/collision_497.inc"
};
static NJS_OBJECT collision_498 = {
#include "stg30/terrain/collision_498.inc"
};
static NJS_OBJECT collision_499 = {
#include "stg30/terrain/collision_499.inc"
};
static NJS_OBJECT collision_500 = {
#include "stg30/terrain/collision_500.inc"
};
static NJS_OBJECT collision_501 = {
#include "stg30/terrain/collision_501.inc"
};
static NJS_OBJECT collision_502 = {
#include "stg30/terrain/collision_502.inc"
};
static NJS_OBJECT collision_503 = {
#include "stg30/terrain/collision_503.inc"
};
static NJS_OBJECT collision_504 = {
#include "stg30/terrain/collision_504.inc"
};
static NJS_OBJECT collision_505 = {
#include "stg30/terrain/collision_505.inc"
};
static NJS_OBJECT collision_506 = {
#include "stg30/terrain/collision_506.inc"
};
static NJS_OBJECT collision_507 = {
#include "stg30/terrain/collision_507.inc"
};
static NJS_OBJECT collision_508 = {
#include "stg30/terrain/collision_508.inc"
};
static NJS_OBJECT collision_509 = {
#include "stg30/terrain/collision_509.inc"
};
static NJS_OBJECT collision_510 = {
#include "stg30/terrain/collision_510.inc"
};
static NJS_OBJECT collision_511 = {
#include "stg30/terrain/collision_511.inc"
};
static NJS_OBJECT collision_512 = {
#include "stg30/terrain/collision_512.inc"
};
static NJS_OBJECT collision_513 = {
#include "stg30/terrain/collision_513.inc"
};
static NJS_OBJECT collision_514 = {
#include "stg30/terrain/collision_514.inc"
};
static NJS_OBJECT collision_515 = {
#include "stg30/terrain/collision_515.inc"
};
static NJS_OBJECT collision_516 = {
#include "stg30/terrain/collision_516.inc"
};
static NJS_OBJECT collision_517 = {
#include "stg30/terrain/collision_517.inc"
};
static NJS_OBJECT collision_518 = {
#include "stg30/terrain/collision_518.inc"
};
static NJS_OBJECT collision_519 = {
#include "stg30/terrain/collision_519.inc"
};
static NJS_OBJECT collision_520 = {
#include "stg30/terrain/collision_520.inc"
};
static NJS_OBJECT collision_521 = {
#include "stg30/terrain/collision_521.inc"
};
static NJS_OBJECT collision_522 = {
#include "stg30/terrain/collision_522.inc"
};
static NJS_OBJECT collision_523 = {
#include "stg30/terrain/collision_523.inc"
};
static NJS_OBJECT collision_524 = {
#include "stg30/terrain/collision_524.inc"
};
static NJS_OBJECT collision_525 = {
#include "stg30/terrain/collision_525.inc"
};
static NJS_OBJECT collision_526 = {
#include "stg30/terrain/collision_526.inc"
};
static NJS_OBJECT collision_527 = {
#include "stg30/terrain/collision_527.inc"
};
static NJS_OBJECT collision_528 = {
#include "stg30/terrain/collision_528.inc"
};
static NJS_OBJECT collision_529 = {
#include "stg30/terrain/collision_529.inc"
};
static NJS_OBJECT collision_530 = {
#include "stg30/terrain/collision_530.inc"
};
static NJS_OBJECT collision_531 = {
#include "stg30/terrain/collision_531.inc"
};
static NJS_OBJECT collision_532 = {
#include "stg30/terrain/collision_532.inc"
};
static NJS_OBJECT collision_533 = {
#include "stg30/terrain/collision_533.inc"
};
static NJS_OBJECT collision_534 = {
#include "stg30/terrain/collision_534.inc"
};
static NJS_OBJECT collision_535 = {
#include "stg30/terrain/collision_535.inc"
};
static NJS_OBJECT collision_536 = {
#include "stg30/terrain/collision_536.inc"
};
static NJS_OBJECT collision_537 = {
#include "stg30/terrain/collision_537.inc"
};
static NJS_OBJECT collision_538 = {
#include "stg30/terrain/collision_538.inc"
};
static NJS_OBJECT collision_539 = {
#include "stg30/terrain/collision_539.inc"
};
static NJS_OBJECT collision_540 = {
#include "stg30/terrain/collision_540.inc"
};
static NJS_OBJECT collision_541 = {
#include "stg30/terrain/collision_541.inc"
};
static NJS_OBJECT collision_542 = {
#include "stg30/terrain/collision_542.inc"
};
static NJS_OBJECT collision_543 = {
#include "stg30/terrain/collision_543.inc"
};
static NJS_OBJECT collision_544 = {
#include "stg30/terrain/collision_544.inc"
};
static NJS_OBJECT collision_545 = {
#include "stg30/terrain/collision_545.inc"
};
static NJS_OBJECT collision_546 = {
#include "stg30/terrain/collision_546.inc"
};
static NJS_OBJECT collision_547 = {
#include "stg30/terrain/collision_547.inc"
};
static NJS_OBJECT collision_548 = {
#include "stg30/terrain/collision_548.inc"
};
static NJS_OBJECT collision_549 = {
#include "stg30/terrain/collision_549.inc"
};
static NJS_OBJECT collision_550 = {
#include "stg30/terrain/collision_550.inc"
};
static NJS_OBJECT collision_551 = {
#include "stg30/terrain/collision_551.inc"
};
static NJS_OBJECT collision_552 = {
#include "stg30/terrain/collision_552.inc"
};
static NJS_OBJECT collision_553 = {
#include "stg30/terrain/collision_553.inc"
};
static NJS_OBJECT collision_554 = {
#include "stg30/terrain/collision_554.inc"
};
static NJS_OBJECT collision_555 = {
#include "stg30/terrain/collision_555.inc"
};
static NJS_OBJECT collision_556 = {
#include "stg30/terrain/collision_556.inc"
};
static NJS_OBJECT collision_557 = {
#include "stg30/terrain/collision_557.inc"
};
static NJS_OBJECT collision_558 = {
#include "stg30/terrain/collision_558.inc"
};
static NJS_OBJECT collision_559 = {
#include "stg30/terrain/collision_559.inc"
};
static NJS_OBJECT collision_560 = {
#include "stg30/terrain/collision_560.inc"
};
static NJS_OBJECT collision_561 = {
#include "stg30/terrain/collision_561.inc"
};
static NJS_OBJECT collision_562 = {
#include "stg30/terrain/collision_562.inc"
};
static NJS_OBJECT collision_563 = {
#include "stg30/terrain/collision_563.inc"
};
static NJS_OBJECT collision_564 = {
#include "stg30/terrain/collision_564.inc"
};
static NJS_OBJECT collision_565 = {
#include "stg30/terrain/collision_565.inc"
};
static NJS_OBJECT collision_566 = {
#include "stg30/terrain/collision_566.inc"
};
static NJS_OBJECT collision_567 = {
#include "stg30/terrain/collision_567.inc"
};
static NJS_OBJECT collision_568 = {
#include "stg30/terrain/collision_568.inc"
};
static NJS_OBJECT collision_569 = {
#include "stg30/terrain/collision_569.inc"
};
static NJS_OBJECT collision_570 = {
#include "stg30/terrain/collision_570.inc"
};
static NJS_OBJECT collision_571 = {
#include "stg30/terrain/collision_571.inc"
};
static NJS_OBJECT collision_572 = {
#include "stg30/terrain/collision_572.inc"
};
static NJS_OBJECT collision_573 = {
#include "stg30/terrain/collision_573.inc"
};
static NJS_OBJECT collision_574 = {
#include "stg30/terrain/collision_574.inc"
};
static NJS_OBJECT collision_575 = {
#include "stg30/terrain/collision_575.inc"
};
static NJS_OBJECT collision_576 = {
#include "stg30/terrain/collision_576.inc"
};
static NJS_OBJECT collision_577 = {
#include "stg30/terrain/collision_577.inc"
};
static NJS_OBJECT collision_578 = {
#include "stg30/terrain/collision_578.inc"
};
static NJS_OBJECT collision_579 = {
#include "stg30/terrain/collision_579.inc"
};
static NJS_OBJECT collision_580 = {
#include "stg30/terrain/collision_580.inc"
};
static NJS_OBJECT collision_581 = {
#include "stg30/terrain/collision_581.inc"
};
static NJS_OBJECT collision_582 = {
#include "stg30/terrain/collision_582.inc"
};
static NJS_OBJECT collision_583 = {
#include "stg30/terrain/collision_583.inc"
};
static NJS_OBJECT collision_584 = {
#include "stg30/terrain/collision_584.inc"
};
static NJS_OBJECT collision_585 = {
#include "stg30/terrain/collision_585.inc"
};
static NJS_OBJECT collision_586 = {
#include "stg30/terrain/collision_586.inc"
};
static NJS_OBJECT collision_587 = {
#include "stg30/terrain/collision_587.inc"
};
static NJS_OBJECT collision_588 = {
#include "stg30/terrain/collision_588.inc"
};
static NJS_OBJECT collision_589 = {
#include "stg30/terrain/collision_589.inc"
};
static NJS_OBJECT collision_590 = {
#include "stg30/terrain/collision_590.inc"
};
static NJS_OBJECT collision_591 = {
#include "stg30/terrain/collision_591.inc"
};
static NJS_OBJECT collision_592 = {
#include "stg30/terrain/collision_592.inc"
};
static NJS_OBJECT collision_593 = {
#include "stg30/terrain/collision_593.inc"
};
static NJS_OBJECT collision_594 = {
#include "stg30/terrain/collision_594.inc"
};
static NJS_OBJECT collision_595 = {
#include "stg30/terrain/collision_595.inc"
};
static NJS_OBJECT collision_596 = {
#include "stg30/terrain/collision_596.inc"
};
static NJS_OBJECT collision_597 = {
#include "stg30/terrain/collision_597.inc"
};
static NJS_OBJECT collision_598 = {
#include "stg30/terrain/collision_598.inc"
};
static NJS_OBJECT collision_599 = {
#include "stg30/terrain/collision_599.inc"
};
static NJS_OBJECT collision_600 = {
#include "stg30/terrain/collision_600.inc"
};
static NJS_OBJECT collision_601 = {
#include "stg30/terrain/collision_601.inc"
};
static NJS_OBJECT collision_602 = {
#include "stg30/terrain/collision_602.inc"
};
static NJS_OBJECT collision_603 = {
#include "stg30/terrain/collision_603.inc"
};
static NJS_OBJECT collision_604 = {
#include "stg30/terrain/collision_604.inc"
};
static NJS_OBJECT collision_605 = {
#include "stg30/terrain/collision_605.inc"
};
static NJS_OBJECT collision_606 = {
#include "stg30/terrain/collision_606.inc"
};
static NJS_OBJECT collision_607 = {
#include "stg30/terrain/collision_607.inc"
};
static NJS_OBJECT collision_608 = {
#include "stg30/terrain/collision_608.inc"
};
static NJS_OBJECT collision_609 = {
#include "stg30/terrain/collision_609.inc"
};
static NJS_OBJECT collision_610 = {
#include "stg30/terrain/collision_610.inc"
};
static NJS_OBJECT collision_611 = {
#include "stg30/terrain/collision_611.inc"
};
static NJS_OBJECT collision_612 = {
#include "stg30/terrain/collision_612.inc"
};
static NJS_OBJECT collision_613 = {
#include "stg30/terrain/collision_613.inc"
};
static NJS_OBJECT collision_614 = {
#include "stg30/terrain/collision_614.inc"
};
static NJS_OBJECT collision_615 = {
#include "stg30/terrain/collision_615.inc"
};
static NJS_OBJECT collision_616 = {
#include "stg30/terrain/collision_616.inc"
};
static NJS_OBJECT collision_617 = {
#include "stg30/terrain/collision_617.inc"
};
static NJS_OBJECT collision_618 = {
#include "stg30/terrain/collision_618.inc"
};
static NJS_OBJECT collision_619 = {
#include "stg30/terrain/collision_619.inc"
};
static NJS_OBJECT collision_620 = {
#include "stg30/terrain/collision_620.inc"
};
static NJS_OBJECT collision_621 = {
#include "stg30/terrain/collision_621.inc"
};
static NJS_OBJECT collision_622 = {
#include "stg30/terrain/collision_622.inc"
};
static NJS_OBJECT collision_623 = {
#include "stg30/terrain/collision_623.inc"
};
static NJS_OBJECT collision_624 = {
#include "stg30/terrain/collision_624.inc"
};
static NJS_OBJECT collision_625 = {
#include "stg30/terrain/collision_625.inc"
};
static NJS_OBJECT collision_627 = {
#include "stg30/terrain/collision_627.inc"
};
static NJS_OBJECT collision_628 = {
#include "stg30/terrain/collision_628.inc"
};
static NJS_OBJECT collision_629 = {
#include "stg30/terrain/collision_629.inc"
};
static NJS_OBJECT collision_630 = {
#include "stg30/terrain/collision_630.inc"
};
static NJS_OBJECT collision_631 = {
#include "stg30/terrain/collision_631.inc"
};
static NJS_OBJECT collision_632 = {
#include "stg30/terrain/collision_632.inc"
};
static NJS_OBJECT collision_633 = {
#include "stg30/terrain/collision_633.inc"
};
static NJS_OBJECT collision_634 = {
#include "stg30/terrain/collision_634.inc"
};
static NJS_OBJECT collision_635 = {
#include "stg30/terrain/collision_635.inc"
};
static NJS_OBJECT collision_636 = {
#include "stg30/terrain/collision_636.inc"
};
static NJS_OBJECT collision_637 = {
#include "stg30/terrain/collision_637.inc"
};
static NJS_OBJECT collision_638 = {
#include "stg30/terrain/collision_638.inc"
};
static NJS_OBJECT collision_639 = {
#include "stg30/terrain/collision_639.inc"
};
static NJS_OBJECT collision_640 = {
#include "stg30/terrain/collision_640.inc"
};
static NJS_OBJECT collision_641 = {
#include "stg30/terrain/collision_641.inc"
};
static NJS_OBJECT collision_642 = {
#include "stg30/terrain/collision_642.inc"
};
static NJS_OBJECT collision_643 = {
#include "stg30/terrain/collision_643.inc"
};
static NJS_OBJECT collision_644 = {
#include "stg30/terrain/collision_644.inc"
};
static NJS_OBJECT collision_645 = {
#include "stg30/terrain/collision_645.inc"
};
static NJS_OBJECT collision_646 = {
#include "stg30/terrain/collision_646.inc"
};
static NJS_OBJECT collision_647 = {
#include "stg30/terrain/collision_647.inc"
};
static NJS_OBJECT collision_648 = {
#include "stg30/terrain/collision_648.inc"
};
static NJS_OBJECT collision_649 = {
#include "stg30/terrain/collision_649.inc"
};
static NJS_OBJECT collision_650 = {
#include "stg30/terrain/collision_650.inc"
};
static NJS_OBJECT collision_651 = {
#include "stg30/terrain/collision_651.inc"
};
static NJS_OBJECT collision_652 = {
#include "stg30/terrain/collision_652.inc"
};
static NJS_OBJECT collision_654 = {
#include "stg30/terrain/collision_654.inc"
};
static NJS_OBJECT collision_655 = {
#include "stg30/terrain/collision_655.inc"
};
static NJS_OBJECT collision_656 = {
#include "stg30/terrain/collision_656.inc"
};
static NJS_OBJECT collision_657 = {
#include "stg30/terrain/collision_657.inc"
};
static NJS_OBJECT collision_658 = {
#include "stg30/terrain/collision_658.inc"
};
static NJS_OBJECT collision_659 = {
#include "stg30/terrain/collision_659.inc"
};
static NJS_OBJECT collision_660 = {
#include "stg30/terrain/collision_660.inc"
};
static NJS_OBJECT collision_662 = {
#include "stg30/terrain/collision_662.inc"
};
static NJS_OBJECT collision_663 = {
#include "stg30/terrain/collision_663.inc"
};
static NJS_OBJECT collision_664 = {
#include "stg30/terrain/collision_664.inc"
};
static NJS_OBJECT collision_665 = {
#include "stg30/terrain/collision_665.inc"
};
static NJS_OBJECT collision_666 = {
#include "stg30/terrain/collision_666.inc"
};
static NJS_OBJECT collision_667 = {
#include "stg30/terrain/collision_667.inc"
};
static NJS_OBJECT collision_668 = {
#include "stg30/terrain/collision_668.inc"
};
static NJS_CNK_OBJECT draw_853 = {
#include "stg30/terrain/draw_853.inc"
};
static NJS_CNK_OBJECT draw_852 = {
#include "stg30/terrain/draw_852.inc"
};
static NJS_CNK_OBJECT draw_851 = {
#include "stg30/terrain/draw_851.inc"
};
static NJS_CNK_OBJECT draw_850 = {
#include "stg30/terrain/draw_850.inc"
};
static NJS_CNK_OBJECT draw_848 = {
#include "stg30/terrain/draw_848.inc"
};
static NJS_CNK_OBJECT draw_847 = {
#include "stg30/terrain/draw_847.inc"
};
static NJS_CNK_OBJECT draw_846 = {
#include "stg30/terrain/draw_846.inc"
};
static NJS_CNK_OBJECT draw_845 = {
#include "stg30/terrain/draw_845.inc"
};
static NJS_CNK_OBJECT draw_844 = {
#include "stg30/terrain/draw_844.inc"
};
static NJS_CNK_OBJECT draw_843 = {
#include "stg30/terrain/draw_843.inc"
};
static NJS_CNK_OBJECT draw_842 = {
#include "stg30/terrain/draw_842.inc"
};
static NJS_CNK_OBJECT draw_841 = {
#include "stg30/terrain/draw_841.inc"
};
static NJS_CNK_OBJECT draw_840 = {
#include "stg30/terrain/draw_840.inc"
};
static NJS_CNK_OBJECT draw_839 = {
#include "stg30/terrain/draw_839.inc"
};
static NJS_CNK_OBJECT draw_838 = {
#include "stg30/terrain/draw_838.inc"
};
static NJS_CNK_OBJECT draw_837 = {
#include "stg30/terrain/draw_837.inc"
};
static NJS_CNK_OBJECT draw_835 = {
#include "stg30/terrain/draw_835.inc"
};
static NJS_CNK_OBJECT draw_833 = {
#include "stg30/terrain/draw_833.inc"
};
static NJS_CNK_OBJECT draw_832 = {
#include "stg30/terrain/draw_832.inc"
};
static NJS_CNK_OBJECT draw_831 = {
#include "stg30/terrain/draw_831.inc"
};
static NJS_CNK_OBJECT draw_830 = {
#include "stg30/terrain/draw_830.inc"
};
static NJS_CNK_OBJECT draw_829 = {
#include "stg30/terrain/draw_829.inc"
};
static NJS_CNK_OBJECT draw_828 = {
#include "stg30/terrain/draw_828.inc"
};
static NJS_CNK_OBJECT draw_827 = {
#include "stg30/terrain/draw_827.inc"
};
static NJS_CNK_OBJECT draw_826 = {
#include "stg30/terrain/draw_826.inc"
};
static NJS_CNK_OBJECT draw_825 = {
#include "stg30/terrain/draw_825.inc"
};
static NJS_CNK_OBJECT draw_824 = {
#include "stg30/terrain/draw_824.inc"
};
static NJS_CNK_OBJECT draw_823 = {
#include "stg30/terrain/draw_823.inc"
};
static NJS_CNK_OBJECT draw_822 = {
#include "stg30/terrain/draw_822.inc"
};
static NJS_CNK_OBJECT draw_821 = {
#include "stg30/terrain/draw_821.inc"
};
static NJS_CNK_OBJECT draw_820 = {
#include "stg30/terrain/draw_820.inc"
};
static NJS_CNK_OBJECT draw_819 = {
#include "stg30/terrain/draw_819.inc"
};
static NJS_CNK_OBJECT draw_818 = {
#include "stg30/terrain/draw_818.inc"
};
static NJS_CNK_OBJECT draw_817 = {
#include "stg30/terrain/draw_817.inc"
};
static NJS_CNK_OBJECT draw_816 = {
#include "stg30/terrain/draw_816.inc"
};
static NJS_CNK_OBJECT draw_815 = {
#include "stg30/terrain/draw_815.inc"
};
static NJS_CNK_OBJECT draw_814 = {
#include "stg30/terrain/draw_814.inc"
};
static NJS_CNK_OBJECT draw_813 = {
#include "stg30/terrain/draw_813.inc"
};
static NJS_CNK_OBJECT draw_812 = {
#include "stg30/terrain/draw_812.inc"
};
static NJS_CNK_OBJECT draw_811 = {
#include "stg30/terrain/draw_811.inc"
};
static NJS_CNK_OBJECT draw_809 = {
#include "stg30/terrain/draw_809.inc"
};
static NJS_CNK_OBJECT draw_807 = {
#include "stg30/terrain/draw_807.inc"
};
static NJS_CNK_OBJECT draw_806 = {
#include "stg30/terrain/draw_806.inc"
};
static NJS_CNK_OBJECT draw_805 = {
#include "stg30/terrain/draw_805.inc"
};
static NJS_CNK_OBJECT draw_804 = {
#include "stg30/terrain/draw_804.inc"
};
static NJS_CNK_OBJECT draw_803 = {
#include "stg30/terrain/draw_803.inc"
};
static NJS_CNK_OBJECT draw_802 = {
#include "stg30/terrain/draw_802.inc"
};
static NJS_CNK_OBJECT draw_801 = {
#include "stg30/terrain/draw_801.inc"
};
static NJS_CNK_OBJECT draw_800 = {
#include "stg30/terrain/draw_800.inc"
};
static NJS_CNK_OBJECT draw_799 = {
#include "stg30/terrain/draw_799.inc"
};
static NJS_CNK_OBJECT draw_798 = {
#include "stg30/terrain/draw_798.inc"
};
static NJS_CNK_OBJECT draw_797 = {
#include "stg30/terrain/draw_797.inc"
};
static NJS_CNK_OBJECT draw_796 = {
#include "stg30/terrain/draw_796.inc"
};
static NJS_CNK_OBJECT draw_795 = {
#include "stg30/terrain/draw_795.inc"
};
static NJS_CNK_OBJECT draw_794 = {
#include "stg30/terrain/draw_794.inc"
};
static NJS_CNK_OBJECT draw_793 = {
#include "stg30/terrain/draw_793.inc"
};
static NJS_CNK_OBJECT draw_792 = {
#include "stg30/terrain/draw_792.inc"
};
static NJS_CNK_OBJECT draw_791 = {
#include "stg30/terrain/draw_791.inc"
};
static NJS_CNK_OBJECT draw_790 = {
#include "stg30/terrain/draw_790.inc"
};
static NJS_CNK_OBJECT draw_789 = {
#include "stg30/terrain/draw_789.inc"
};
static NJS_CNK_OBJECT draw_788 = {
#include "stg30/terrain/draw_788.inc"
};
static NJS_CNK_OBJECT draw_787 = {
#include "stg30/terrain/draw_787.inc"
};
static NJS_CNK_OBJECT draw_786 = {
#include "stg30/terrain/draw_786.inc"
};
static NJS_CNK_OBJECT draw_785 = {
#include "stg30/terrain/draw_785.inc"
};
static NJS_CNK_OBJECT draw_784 = {
#include "stg30/terrain/draw_784.inc"
};
static NJS_CNK_OBJECT draw_783 = {
#include "stg30/terrain/draw_783.inc"
};
static NJS_CNK_OBJECT draw_782 = {
#include "stg30/terrain/draw_782.inc"
};
static NJS_CNK_OBJECT draw_781 = {
#include "stg30/terrain/draw_781.inc"
};
static NJS_CNK_OBJECT draw_780 = {
#include "stg30/terrain/draw_780.inc"
};
static NJS_CNK_OBJECT draw_779 = {
#include "stg30/terrain/draw_779.inc"
};
static NJS_CNK_OBJECT draw_778 = {
#include "stg30/terrain/draw_778.inc"
};
static NJS_CNK_OBJECT draw_777 = {
#include "stg30/terrain/draw_777.inc"
};
static NJS_CNK_OBJECT draw_776 = {
#include "stg30/terrain/draw_776.inc"
};
static NJS_CNK_OBJECT draw_775 = {
#include "stg30/terrain/draw_775.inc"
};
static NJS_CNK_OBJECT draw_774 = {
#include "stg30/terrain/draw_774.inc"
};
static NJS_CNK_OBJECT draw_773 = {
#include "stg30/terrain/draw_773.inc"
};
static NJS_CNK_OBJECT draw_772 = {
#include "stg30/terrain/draw_772.inc"
};
static NJS_CNK_OBJECT draw_771 = {
#include "stg30/terrain/draw_771.inc"
};
static NJS_CNK_OBJECT draw_770 = {
#include "stg30/terrain/draw_770.inc"
};
static NJS_CNK_OBJECT draw_769 = {
#include "stg30/terrain/draw_769.inc"
};
static NJS_CNK_OBJECT draw_768 = {
#include "stg30/terrain/draw_768.inc"
};
static NJS_CNK_OBJECT draw_767 = {
#include "stg30/terrain/draw_767.inc"
};
static NJS_CNK_OBJECT draw_766 = {
#include "stg30/terrain/draw_766.inc"
};
static NJS_CNK_OBJECT draw_765 = {
#include "stg30/terrain/draw_765.inc"
};
static NJS_CNK_OBJECT draw_764 = {
#include "stg30/terrain/draw_764.inc"
};
static NJS_CNK_OBJECT draw_763 = {
#include "stg30/terrain/draw_763.inc"
};
static NJS_CNK_OBJECT draw_762 = {
#include "stg30/terrain/draw_762.inc"
};
static NJS_CNK_OBJECT draw_761 = {
#include "stg30/terrain/draw_761.inc"
};
static NJS_CNK_OBJECT draw_759 = {
#include "stg30/terrain/draw_759.inc"
};
static NJS_CNK_OBJECT draw_758 = {
#include "stg30/terrain/draw_758.inc"
};
static NJS_CNK_OBJECT draw_757 = {
#include "stg30/terrain/draw_757.inc"
};
static NJS_CNK_OBJECT draw_756 = {
#include "stg30/terrain/draw_756.inc"
};
static NJS_CNK_OBJECT draw_755 = {
#include "stg30/terrain/draw_755.inc"
};
static NJS_CNK_OBJECT draw_754 = {
#include "stg30/terrain/draw_754.inc"
};
static NJS_CNK_OBJECT draw_753 = {
#include "stg30/terrain/draw_753.inc"
};
static NJS_CNK_OBJECT draw_752 = {
#include "stg30/terrain/draw_752.inc"
};
static NJS_CNK_OBJECT draw_751 = {
#include "stg30/terrain/draw_751.inc"
};
static NJS_CNK_OBJECT draw_750 = {
#include "stg30/terrain/draw_750.inc"
};
static NJS_CNK_OBJECT draw_749 = {
#include "stg30/terrain/draw_749.inc"
};
static NJS_CNK_OBJECT draw_748 = {
#include "stg30/terrain/draw_748.inc"
};
static NJS_CNK_OBJECT draw_747 = {
#include "stg30/terrain/draw_747.inc"
};
static NJS_CNK_OBJECT draw_746 = {
#include "stg30/terrain/draw_746.inc"
};
static NJS_CNK_OBJECT draw_745 = {
#include "stg30/terrain/draw_745.inc"
};
static NJS_CNK_OBJECT draw_744 = {
#include "stg30/terrain/draw_744.inc"
};
static NJS_CNK_OBJECT draw_743 = {
#include "stg30/terrain/draw_743.inc"
};
static NJS_CNK_OBJECT draw_742 = {
#include "stg30/terrain/draw_742.inc"
};
static NJS_CNK_OBJECT draw_741 = {
#include "stg30/terrain/draw_741.inc"
};
static NJS_CNK_OBJECT draw_740 = {
#include "stg30/terrain/draw_740.inc"
};
static NJS_CNK_OBJECT draw_739 = {
#include "stg30/terrain/draw_739.inc"
};
static NJS_CNK_OBJECT draw_738 = {
#include "stg30/terrain/draw_738.inc"
};
static NJS_CNK_OBJECT draw_737 = {
#include "stg30/terrain/draw_737.inc"
};
static NJS_CNK_OBJECT draw_736 = {
#include "stg30/terrain/draw_736.inc"
};
static NJS_CNK_OBJECT draw_735 = {
#include "stg30/terrain/draw_735.inc"
};
static NJS_CNK_OBJECT draw_734 = {
#include "stg30/terrain/draw_734.inc"
};
static NJS_CNK_OBJECT draw_733 = {
#include "stg30/terrain/draw_733.inc"
};
static NJS_CNK_OBJECT draw_729 = {
#include "stg30/terrain/draw_729.inc"
};
static NJS_CNK_OBJECT draw_727 = {
#include "stg30/terrain/draw_727.inc"
};
static NJS_CNK_OBJECT draw_726 = {
#include "stg30/terrain/draw_726.inc"
};
static NJS_CNK_OBJECT draw_724 = {
#include "stg30/terrain/draw_724.inc"
};
static NJS_CNK_OBJECT draw_723 = {
#include "stg30/terrain/draw_723.inc"
};
static NJS_CNK_OBJECT draw_718 = {
#include "stg30/terrain/draw_718.inc"
};
static NJS_CNK_OBJECT draw_717 = {
#include "stg30/terrain/draw_717.inc"
};
static NJS_CNK_OBJECT draw_712 = {
#include "stg30/terrain/draw_712.inc"
};
static NJS_CNK_OBJECT draw_709 = {
#include "stg30/terrain/draw_709.inc"
};
static NJS_CNK_OBJECT draw_708 = {
#include "stg30/terrain/draw_708.inc"
};
static NJS_CNK_OBJECT draw_707 = {
#include "stg30/terrain/draw_707.inc"
};
static NJS_CNK_OBJECT draw_706 = {
#include "stg30/terrain/draw_706.inc"
};
static NJS_CNK_OBJECT draw_705 = {
#include "stg30/terrain/draw_705.inc"
};
static NJS_CNK_OBJECT draw_704 = {
#include "stg30/terrain/draw_704.inc"
};
static NJS_CNK_OBJECT draw_703 = {
#include "stg30/terrain/draw_703.inc"
};
static NJS_CNK_OBJECT draw_702 = {
#include "stg30/terrain/draw_702.inc"
};
static NJS_CNK_OBJECT draw_701 = {
#include "stg30/terrain/draw_701.inc"
};
static NJS_CNK_OBJECT draw_700 = {
#include "stg30/terrain/draw_700.inc"
};
static NJS_CNK_OBJECT draw_699 = {
#include "stg30/terrain/draw_699.inc"
};
static NJS_CNK_OBJECT draw_698 = {
#include "stg30/terrain/draw_698.inc"
};
static NJS_CNK_OBJECT draw_697 = {
#include "stg30/terrain/draw_697.inc"
};
static NJS_CNK_OBJECT draw_696 = {
#include "stg30/terrain/draw_696.inc"
};
static NJS_CNK_OBJECT draw_695 = {
#include "stg30/terrain/draw_695.inc"
};
static NJS_CNK_OBJECT draw_694 = {
#include "stg30/terrain/draw_694.inc"
};
static NJS_CNK_OBJECT draw_693 = {
#include "stg30/terrain/draw_693.inc"
};
static NJS_CNK_OBJECT draw_692 = {
#include "stg30/terrain/draw_692.inc"
};
static NJS_CNK_OBJECT draw_691 = {
#include "stg30/terrain/draw_691.inc"
};
static NJS_CNK_OBJECT draw_690 = {
#include "stg30/terrain/draw_690.inc"
};
static NJS_CNK_OBJECT draw_689 = {
#include "stg30/terrain/draw_689.inc"
};
static NJS_CNK_OBJECT draw_688 = {
#include "stg30/terrain/draw_688.inc"
};
static NJS_CNK_OBJECT draw_687 = {
#include "stg30/terrain/draw_687.inc"
};
static NJS_CNK_OBJECT draw_686 = {
#include "stg30/terrain/draw_686.inc"
};
static NJS_CNK_OBJECT draw_685 = {
#include "stg30/terrain/draw_685.inc"
};
static NJS_CNK_OBJECT draw_684 = {
#include "stg30/terrain/draw_684.inc"
};
static NJS_CNK_OBJECT draw_683 = {
#include "stg30/terrain/draw_683.inc"
};
static NJS_CNK_OBJECT draw_682 = {
#include "stg30/terrain/draw_682.inc"
};
static NJS_CNK_OBJECT draw_681 = {
#include "stg30/terrain/draw_681.inc"
};
static NJS_CNK_OBJECT draw_680 = {
#include "stg30/terrain/draw_680.inc"
};
static NJS_CNK_OBJECT draw_679 = {
#include "stg30/terrain/draw_679.inc"
};
static NJS_CNK_OBJECT draw_678 = {
#include "stg30/terrain/draw_678.inc"
};
static NJS_CNK_OBJECT draw_677 = {
#include "stg30/terrain/draw_677.inc"
};
static NJS_CNK_OBJECT draw_676 = {
#include "stg30/terrain/draw_676.inc"
};
static NJS_CNK_OBJECT draw_675 = {
#include "stg30/terrain/draw_675.inc"
};
static NJS_CNK_OBJECT draw_674 = {
#include "stg30/terrain/draw_674.inc"
};
static NJS_CNK_OBJECT draw_673 = {
#include "stg30/terrain/draw_673.inc"
};
static NJS_CNK_OBJECT draw_672 = {
#include "stg30/terrain/draw_672.inc"
};
static NJS_CNK_OBJECT draw_671 = {
#include "stg30/terrain/draw_671.inc"
};
static NJS_CNK_OBJECT draw_670 = {
#include "stg30/terrain/draw_670.inc"
};
static NJS_CNK_OBJECT draw_669 = {
#include "stg30/terrain/draw_669.inc"
};
static NJS_CNK_OBJECT draw_668 = {
#include "stg30/terrain/draw_668.inc"
};
static NJS_CNK_OBJECT draw_667 = {
#include "stg30/terrain/draw_667.inc"
};
static NJS_CNK_OBJECT draw_666 = {
#include "stg30/terrain/draw_666.inc"
};
static NJS_CNK_OBJECT draw_665 = {
#include "stg30/terrain/draw_665.inc"
};
static NJS_CNK_OBJECT draw_664 = {
#include "stg30/terrain/draw_664.inc"
};
static NJS_CNK_OBJECT draw_663 = {
#include "stg30/terrain/draw_663.inc"
};
static NJS_CNK_OBJECT draw_662 = {
#include "stg30/terrain/draw_662.inc"
};
static NJS_CNK_OBJECT draw_661 = {
#include "stg30/terrain/draw_661.inc"
};
static NJS_CNK_OBJECT draw_660 = {
#include "stg30/terrain/draw_660.inc"
};
static NJS_CNK_OBJECT draw_659 = {
#include "stg30/terrain/draw_659.inc"
};
static NJS_CNK_OBJECT draw_658 = {
#include "stg30/terrain/draw_658.inc"
};
static NJS_CNK_OBJECT draw_657 = {
#include "stg30/terrain/draw_657.inc"
};
static NJS_CNK_OBJECT draw_656 = {
#include "stg30/terrain/draw_656.inc"
};
static NJS_CNK_OBJECT draw_655 = {
#include "stg30/terrain/draw_655.inc"
};
static NJS_CNK_OBJECT draw_654 = {
#include "stg30/terrain/draw_654.inc"
};
static NJS_CNK_OBJECT draw_653 = {
#include "stg30/terrain/draw_653.inc"
};
static NJS_CNK_OBJECT draw_652 = {
#include "stg30/terrain/draw_652.inc"
};
static NJS_CNK_OBJECT draw_651 = {
#include "stg30/terrain/draw_651.inc"
};
static NJS_CNK_OBJECT draw_650 = {
#include "stg30/terrain/draw_650.inc"
};
static NJS_CNK_OBJECT draw_649 = {
#include "stg30/terrain/draw_649.inc"
};
static NJS_CNK_OBJECT draw_648 = {
#include "stg30/terrain/draw_648.inc"
};
static NJS_CNK_OBJECT draw_647 = {
#include "stg30/terrain/draw_647.inc"
};
static NJS_CNK_OBJECT draw_646 = {
#include "stg30/terrain/draw_646.inc"
};
static NJS_CNK_OBJECT draw_645 = {
#include "stg30/terrain/draw_645.inc"
};
static NJS_CNK_OBJECT draw_644 = {
#include "stg30/terrain/draw_644.inc"
};
static NJS_CNK_OBJECT draw_643 = {
#include "stg30/terrain/draw_643.inc"
};
static NJS_CNK_OBJECT draw_642 = {
#include "stg30/terrain/draw_642.inc"
};
static NJS_CNK_OBJECT draw_641 = {
#include "stg30/terrain/draw_641.inc"
};
static NJS_CNK_OBJECT draw_640 = {
#include "stg30/terrain/draw_640.inc"
};
static NJS_CNK_OBJECT draw_639 = {
#include "stg30/terrain/draw_639.inc"
};
static NJS_CNK_OBJECT draw_638 = {
#include "stg30/terrain/draw_638.inc"
};
static NJS_CNK_OBJECT draw_637 = {
#include "stg30/terrain/draw_637.inc"
};
static NJS_CNK_OBJECT draw_636 = {
#include "stg30/terrain/draw_636.inc"
};
static NJS_CNK_OBJECT draw_635 = {
#include "stg30/terrain/draw_635.inc"
};
static NJS_CNK_OBJECT draw_634 = {
#include "stg30/terrain/draw_634.inc"
};
static NJS_CNK_OBJECT draw_633 = {
#include "stg30/terrain/draw_633.inc"
};
static NJS_CNK_OBJECT draw_632 = {
#include "stg30/terrain/draw_632.inc"
};
static NJS_CNK_OBJECT draw_631 = {
#include "stg30/terrain/draw_631.inc"
};
static NJS_CNK_OBJECT draw_630 = {
#include "stg30/terrain/draw_630.inc"
};
static NJS_CNK_OBJECT draw_629 = {
#include "stg30/terrain/draw_629.inc"
};
static NJS_CNK_OBJECT draw_628 = {
#include "stg30/terrain/draw_628.inc"
};
static NJS_CNK_OBJECT draw_627 = {
#include "stg30/terrain/draw_627.inc"
};
static NJS_CNK_OBJECT draw_626 = {
#include "stg30/terrain/draw_626.inc"
};
static NJS_CNK_OBJECT draw_625 = {
#include "stg30/terrain/draw_625.inc"
};
static NJS_CNK_OBJECT draw_624 = {
#include "stg30/terrain/draw_624.inc"
};
static NJS_CNK_OBJECT draw_623 = {
#include "stg30/terrain/draw_623.inc"
};
static NJS_CNK_OBJECT draw_622 = {
#include "stg30/terrain/draw_622.inc"
};
static NJS_CNK_OBJECT draw_621 = {
#include "stg30/terrain/draw_621.inc"
};
static NJS_CNK_OBJECT draw_620 = {
#include "stg30/terrain/draw_620.inc"
};
static NJS_CNK_OBJECT draw_619 = {
#include "stg30/terrain/draw_619.inc"
};
static NJS_CNK_OBJECT draw_618 = {
#include "stg30/terrain/draw_618.inc"
};
static NJS_CNK_OBJECT draw_617 = {
#include "stg30/terrain/draw_617.inc"
};
static NJS_CNK_OBJECT draw_616 = {
#include "stg30/terrain/draw_616.inc"
};
static NJS_CNK_OBJECT draw_615 = {
#include "stg30/terrain/draw_615.inc"
};
static NJS_CNK_OBJECT draw_614 = {
#include "stg30/terrain/draw_614.inc"
};
static NJS_CNK_OBJECT draw_613 = {
#include "stg30/terrain/draw_613.inc"
};
static NJS_CNK_OBJECT draw_612 = {
#include "stg30/terrain/draw_612.inc"
};
static NJS_CNK_OBJECT draw_611 = {
#include "stg30/terrain/draw_611.inc"
};
static NJS_CNK_OBJECT draw_610 = {
#include "stg30/terrain/draw_610.inc"
};
static NJS_CNK_OBJECT draw_609 = {
#include "stg30/terrain/draw_609.inc"
};
static NJS_CNK_OBJECT draw_608 = {
#include "stg30/terrain/draw_608.inc"
};
static NJS_CNK_OBJECT draw_607 = {
#include "stg30/terrain/draw_607.inc"
};
static NJS_CNK_OBJECT draw_606 = {
#include "stg30/terrain/draw_606.inc"
};
static NJS_CNK_OBJECT draw_605 = {
#include "stg30/terrain/draw_605.inc"
};
static NJS_CNK_OBJECT draw_604 = {
#include "stg30/terrain/draw_604.inc"
};
static NJS_CNK_OBJECT draw_603 = {
#include "stg30/terrain/draw_603.inc"
};
static NJS_CNK_OBJECT draw_602 = {
#include "stg30/terrain/draw_602.inc"
};
static NJS_CNK_OBJECT draw_601 = {
#include "stg30/terrain/draw_601.inc"
};
static NJS_CNK_OBJECT draw_600 = {
#include "stg30/terrain/draw_600.inc"
};
static NJS_CNK_OBJECT draw_599 = {
#include "stg30/terrain/draw_599.inc"
};
static NJS_CNK_OBJECT draw_598 = {
#include "stg30/terrain/draw_598.inc"
};
static NJS_CNK_OBJECT draw_597 = {
#include "stg30/terrain/draw_597.inc"
};
static NJS_CNK_OBJECT draw_596 = {
#include "stg30/terrain/draw_596.inc"
};
static NJS_CNK_OBJECT draw_595 = {
#include "stg30/terrain/draw_595.inc"
};
static NJS_CNK_OBJECT draw_594 = {
#include "stg30/terrain/draw_594.inc"
};
static NJS_CNK_OBJECT draw_593 = {
#include "stg30/terrain/draw_593.inc"
};
static NJS_CNK_OBJECT draw_592 = {
#include "stg30/terrain/draw_592.inc"
};
static NJS_CNK_OBJECT draw_590 = {
#include "stg30/terrain/draw_590.inc"
};
static NJS_CNK_OBJECT draw_589 = {
#include "stg30/terrain/draw_589.inc"
};
static NJS_CNK_OBJECT draw_583 = {
#include "stg30/terrain/draw_583.inc"
};
static NJS_CNK_OBJECT draw_582 = {
#include "stg30/terrain/draw_582.inc"
};
static NJS_CNK_OBJECT draw_580 = {
#include "stg30/terrain/draw_580.inc"
};
static NJS_CNK_OBJECT draw_579 = {
#include "stg30/terrain/draw_579.inc"
};
static NJS_CNK_OBJECT draw_576 = {
#include "stg30/terrain/draw_576.inc"
};
static NJS_CNK_OBJECT draw_573 = {
#include "stg30/terrain/draw_573.inc"
};
static NJS_CNK_OBJECT draw_572 = {
#include "stg30/terrain/draw_572.inc"
};
static NJS_CNK_OBJECT draw_571 = {
#include "stg30/terrain/draw_571.inc"
};
static NJS_CNK_OBJECT draw_568 = {
#include "stg30/terrain/draw_568.inc"
};
static NJS_CNK_OBJECT draw_567 = {
#include "stg30/terrain/draw_567.inc"
};
static NJS_CNK_OBJECT draw_566 = {
#include "stg30/terrain/draw_566.inc"
};
static NJS_CNK_OBJECT draw_565 = {
#include "stg30/terrain/draw_565.inc"
};
static NJS_CNK_OBJECT draw_564 = {
#include "stg30/terrain/draw_564.inc"
};
static NJS_CNK_OBJECT draw_563 = {
#include "stg30/terrain/draw_563.inc"
};
static NJS_CNK_OBJECT draw_562 = {
#include "stg30/terrain/draw_562.inc"
};
static NJS_CNK_OBJECT draw_561 = {
#include "stg30/terrain/draw_561.inc"
};
static NJS_CNK_OBJECT draw_560 = {
#include "stg30/terrain/draw_560.inc"
};
static NJS_CNK_OBJECT draw_559 = {
#include "stg30/terrain/draw_559.inc"
};
static NJS_CNK_OBJECT draw_558 = {
#include "stg30/terrain/draw_558.inc"
};
static NJS_CNK_OBJECT draw_557 = {
#include "stg30/terrain/draw_557.inc"
};
static NJS_CNK_OBJECT draw_555 = {
#include "stg30/terrain/draw_555.inc"
};
static NJS_CNK_OBJECT draw_554 = {
#include "stg30/terrain/draw_554.inc"
};
static NJS_CNK_OBJECT draw_553 = {
#include "stg30/terrain/draw_553.inc"
};
static NJS_CNK_OBJECT draw_552 = {
#include "stg30/terrain/draw_552.inc"
};
static NJS_CNK_OBJECT draw_551 = {
#include "stg30/terrain/draw_551.inc"
};
static NJS_CNK_OBJECT draw_550 = {
#include "stg30/terrain/draw_550.inc"
};
static NJS_CNK_OBJECT draw_549 = {
#include "stg30/terrain/draw_549.inc"
};
static NJS_CNK_OBJECT draw_548 = {
#include "stg30/terrain/draw_548.inc"
};
static NJS_CNK_OBJECT draw_547 = {
#include "stg30/terrain/draw_547.inc"
};
static NJS_CNK_OBJECT draw_546 = {
#include "stg30/terrain/draw_546.inc"
};
static NJS_CNK_OBJECT draw_545 = {
#include "stg30/terrain/draw_545.inc"
};
static NJS_CNK_OBJECT draw_544 = {
#include "stg30/terrain/draw_544.inc"
};
static NJS_CNK_OBJECT draw_543 = {
#include "stg30/terrain/draw_543.inc"
};
static NJS_CNK_OBJECT draw_542 = {
#include "stg30/terrain/draw_542.inc"
};
static NJS_CNK_OBJECT draw_541 = {
#include "stg30/terrain/draw_541.inc"
};
static NJS_CNK_OBJECT draw_540 = {
#include "stg30/terrain/draw_540.inc"
};
static NJS_CNK_OBJECT draw_539 = {
#include "stg30/terrain/draw_539.inc"
};
static NJS_CNK_OBJECT draw_538 = {
#include "stg30/terrain/draw_538.inc"
};
static NJS_CNK_OBJECT draw_537 = {
#include "stg30/terrain/draw_537.inc"
};
static NJS_CNK_OBJECT draw_536 = {
#include "stg30/terrain/draw_536.inc"
};
static NJS_CNK_OBJECT draw_535 = {
#include "stg30/terrain/draw_535.inc"
};
static NJS_CNK_OBJECT draw_534 = {
#include "stg30/terrain/draw_534.inc"
};
static NJS_CNK_OBJECT draw_533 = {
#include "stg30/terrain/draw_533.inc"
};
static NJS_CNK_OBJECT draw_532 = {
#include "stg30/terrain/draw_532.inc"
};
static NJS_CNK_OBJECT draw_531 = {
#include "stg30/terrain/draw_531.inc"
};
static NJS_CNK_OBJECT draw_530 = {
#include "stg30/terrain/draw_530.inc"
};
static NJS_CNK_OBJECT draw_529 = {
#include "stg30/terrain/draw_529.inc"
};
static NJS_CNK_OBJECT draw_528 = {
#include "stg30/terrain/draw_528.inc"
};
static NJS_CNK_OBJECT draw_527 = {
#include "stg30/terrain/draw_527.inc"
};
static NJS_CNK_OBJECT draw_526 = {
#include "stg30/terrain/draw_526.inc"
};
static NJS_CNK_OBJECT draw_525 = {
#include "stg30/terrain/draw_525.inc"
};
static NJS_CNK_OBJECT draw_524 = {
#include "stg30/terrain/draw_524.inc"
};
static NJS_CNK_OBJECT draw_523 = {
#include "stg30/terrain/draw_523.inc"
};
static NJS_CNK_OBJECT draw_522 = {
#include "stg30/terrain/draw_522.inc"
};
static NJS_CNK_OBJECT draw_521 = {
#include "stg30/terrain/draw_521.inc"
};
static NJS_CNK_OBJECT draw_520 = {
#include "stg30/terrain/draw_520.inc"
};
static NJS_CNK_OBJECT draw_519 = {
#include "stg30/terrain/draw_519.inc"
};
static NJS_CNK_OBJECT draw_518 = {
#include "stg30/terrain/draw_518.inc"
};
static NJS_CNK_OBJECT draw_517 = {
#include "stg30/terrain/draw_517.inc"
};
static NJS_CNK_OBJECT draw_516 = {
#include "stg30/terrain/draw_516.inc"
};
static NJS_CNK_OBJECT draw_515 = {
#include "stg30/terrain/draw_515.inc"
};
static NJS_CNK_OBJECT draw_514 = {
#include "stg30/terrain/draw_514.inc"
};
static NJS_CNK_OBJECT draw_513 = {
#include "stg30/terrain/draw_513.inc"
};
static NJS_CNK_OBJECT draw_512 = {
#include "stg30/terrain/draw_512.inc"
};
static NJS_CNK_OBJECT draw_511 = {
#include "stg30/terrain/draw_511.inc"
};
static NJS_CNK_OBJECT draw_510 = {
#include "stg30/terrain/draw_510.inc"
};
static NJS_CNK_OBJECT draw_509 = {
#include "stg30/terrain/draw_509.inc"
};
static NJS_CNK_OBJECT draw_508 = {
#include "stg30/terrain/draw_508.inc"
};
static NJS_CNK_OBJECT draw_507 = {
#include "stg30/terrain/draw_507.inc"
};
static NJS_CNK_OBJECT draw_506 = {
#include "stg30/terrain/draw_506.inc"
};
static NJS_CNK_OBJECT draw_505 = {
#include "stg30/terrain/draw_505.inc"
};
static NJS_CNK_OBJECT draw_504 = {
#include "stg30/terrain/draw_504.inc"
};
static NJS_CNK_OBJECT draw_503 = {
#include "stg30/terrain/draw_503.inc"
};
static NJS_CNK_OBJECT draw_502 = {
#include "stg30/terrain/draw_502.inc"
};
static NJS_CNK_OBJECT draw_501 = {
#include "stg30/terrain/draw_501.inc"
};
static NJS_CNK_OBJECT draw_500 = {
#include "stg30/terrain/draw_500.inc"
};
static NJS_CNK_OBJECT draw_499 = {
#include "stg30/terrain/draw_499.inc"
};
static NJS_CNK_OBJECT draw_498 = {
#include "stg30/terrain/draw_498.inc"
};
static NJS_CNK_OBJECT draw_497 = {
#include "stg30/terrain/draw_497.inc"
};
static NJS_CNK_OBJECT draw_496 = {
#include "stg30/terrain/draw_496.inc"
};
static NJS_CNK_OBJECT draw_495 = {
#include "stg30/terrain/draw_495.inc"
};
static NJS_CNK_OBJECT draw_494 = {
#include "stg30/terrain/draw_494.inc"
};
static NJS_CNK_OBJECT draw_493 = {
#include "stg30/terrain/draw_493.inc"
};
static NJS_CNK_OBJECT draw_492 = {
#include "stg30/terrain/draw_492.inc"
};
static NJS_CNK_OBJECT draw_491 = {
#include "stg30/terrain/draw_491.inc"
};
static NJS_CNK_OBJECT draw_490 = {
#include "stg30/terrain/draw_490.inc"
};
static NJS_CNK_OBJECT draw_489 = {
#include "stg30/terrain/draw_489.inc"
};
static NJS_CNK_OBJECT draw_488 = {
#include "stg30/terrain/draw_488.inc"
};
static NJS_CNK_OBJECT draw_487 = {
#include "stg30/terrain/draw_487.inc"
};
static NJS_CNK_OBJECT draw_486 = {
#include "stg30/terrain/draw_486.inc"
};
static NJS_CNK_OBJECT draw_485 = {
#include "stg30/terrain/draw_485.inc"
};
static NJS_CNK_OBJECT draw_484 = {
#include "stg30/terrain/draw_484.inc"
};
static NJS_CNK_OBJECT draw_483 = {
#include "stg30/terrain/draw_483.inc"
};
static NJS_CNK_OBJECT draw_482 = {
#include "stg30/terrain/draw_482.inc"
};
static NJS_CNK_OBJECT draw_481 = {
#include "stg30/terrain/draw_481.inc"
};
static NJS_CNK_OBJECT draw_480 = {
#include "stg30/terrain/draw_480.inc"
};
static NJS_CNK_OBJECT draw_479 = {
#include "stg30/terrain/draw_479.inc"
};
static NJS_CNK_OBJECT draw_478 = {
#include "stg30/terrain/draw_478.inc"
};
static NJS_CNK_OBJECT draw_477 = {
#include "stg30/terrain/draw_477.inc"
};
static NJS_CNK_OBJECT draw_476 = {
#include "stg30/terrain/draw_476.inc"
};
static NJS_CNK_OBJECT draw_475 = {
#include "stg30/terrain/draw_475.inc"
};
static NJS_CNK_OBJECT draw_474 = {
#include "stg30/terrain/draw_474.inc"
};
static NJS_CNK_OBJECT draw_473 = {
#include "stg30/terrain/draw_473.inc"
};
static NJS_CNK_OBJECT draw_472 = {
#include "stg30/terrain/draw_472.inc"
};
static NJS_CNK_OBJECT draw_471 = {
#include "stg30/terrain/draw_471.inc"
};
static NJS_CNK_OBJECT draw_470 = {
#include "stg30/terrain/draw_470.inc"
};
static NJS_CNK_OBJECT draw_469 = {
#include "stg30/terrain/draw_469.inc"
};
static NJS_CNK_OBJECT draw_468 = {
#include "stg30/terrain/draw_468.inc"
};
static NJS_CNK_OBJECT draw_467 = {
#include "stg30/terrain/draw_467.inc"
};
static NJS_CNK_OBJECT draw_466 = {
#include "stg30/terrain/draw_466.inc"
};
static NJS_CNK_OBJECT draw_465 = {
#include "stg30/terrain/draw_465.inc"
};
static NJS_CNK_OBJECT draw_464 = {
#include "stg30/terrain/draw_464.inc"
};
static NJS_CNK_OBJECT draw_463 = {
#include "stg30/terrain/draw_463.inc"
};
static NJS_CNK_OBJECT draw_462 = {
#include "stg30/terrain/draw_462.inc"
};
static NJS_CNK_OBJECT draw_461 = {
#include "stg30/terrain/draw_461.inc"
};
static NJS_CNK_OBJECT draw_460 = {
#include "stg30/terrain/draw_460.inc"
};
static NJS_CNK_OBJECT draw_459 = {
#include "stg30/terrain/draw_459.inc"
};
static NJS_CNK_OBJECT draw_458 = {
#include "stg30/terrain/draw_458.inc"
};
static NJS_CNK_OBJECT draw_457 = {
#include "stg30/terrain/draw_457.inc"
};
static NJS_CNK_OBJECT draw_455 = {
#include "stg30/terrain/draw_455.inc"
};
static NJS_CNK_OBJECT draw_454 = {
#include "stg30/terrain/draw_454.inc"
};
static NJS_CNK_OBJECT draw_453 = {
#include "stg30/terrain/draw_453.inc"
};
static NJS_CNK_OBJECT draw_452 = {
#include "stg30/terrain/draw_452.inc"
};
static NJS_CNK_OBJECT draw_451 = {
#include "stg30/terrain/draw_451.inc"
};
static NJS_CNK_OBJECT draw_450 = {
#include "stg30/terrain/draw_450.inc"
};
static NJS_CNK_OBJECT draw_449 = {
#include "stg30/terrain/draw_449.inc"
};
static NJS_CNK_OBJECT draw_448 = {
#include "stg30/terrain/draw_448.inc"
};
static NJS_CNK_OBJECT draw_447 = {
#include "stg30/terrain/draw_447.inc"
};
static NJS_CNK_OBJECT draw_446 = {
#include "stg30/terrain/draw_446.inc"
};
static NJS_CNK_OBJECT draw_445 = {
#include "stg30/terrain/draw_445.inc"
};
static NJS_CNK_OBJECT draw_444 = {
#include "stg30/terrain/draw_444.inc"
};
static NJS_CNK_OBJECT draw_443 = {
#include "stg30/terrain/draw_443.inc"
};
static NJS_CNK_OBJECT draw_442 = {
#include "stg30/terrain/draw_442.inc"
};
static NJS_CNK_OBJECT draw_441 = {
#include "stg30/terrain/draw_441.inc"
};
static NJS_CNK_OBJECT draw_440 = {
#include "stg30/terrain/draw_440.inc"
};
static NJS_CNK_OBJECT draw_439 = {
#include "stg30/terrain/draw_439.inc"
};
static NJS_CNK_OBJECT draw_438 = {
#include "stg30/terrain/draw_438.inc"
};
static NJS_CNK_OBJECT draw_437 = {
#include "stg30/terrain/draw_437.inc"
};
static NJS_CNK_OBJECT draw_436 = {
#include "stg30/terrain/draw_436.inc"
};
static NJS_CNK_OBJECT draw_435 = {
#include "stg30/terrain/draw_435.inc"
};
static NJS_CNK_OBJECT draw_434 = {
#include "stg30/terrain/draw_434.inc"
};
static NJS_CNK_OBJECT draw_433 = {
#include "stg30/terrain/draw_433.inc"
};
static NJS_CNK_OBJECT draw_432 = {
#include "stg30/terrain/draw_432.inc"
};
static NJS_CNK_OBJECT draw_431 = {
#include "stg30/terrain/draw_431.inc"
};
static NJS_CNK_OBJECT draw_430 = {
#include "stg30/terrain/draw_430.inc"
};
static NJS_CNK_OBJECT draw_429 = {
#include "stg30/terrain/draw_429.inc"
};
static NJS_CNK_OBJECT draw_428 = {
#include "stg30/terrain/draw_428.inc"
};
static NJS_CNK_OBJECT draw_427 = {
#include "stg30/terrain/draw_427.inc"
};
static NJS_CNK_OBJECT draw_422 = {
#include "stg30/terrain/draw_422.inc"
};
static NJS_CNK_OBJECT draw_420 = {
#include "stg30/terrain/draw_420.inc"
};
static NJS_CNK_OBJECT draw_419 = {
#include "stg30/terrain/draw_419.inc"
};
static NJS_CNK_OBJECT draw_418 = {
#include "stg30/terrain/draw_418.inc"
};
static NJS_CNK_OBJECT draw_417 = {
#include "stg30/terrain/draw_417.inc"
};
static NJS_CNK_OBJECT draw_416 = {
#include "stg30/terrain/draw_416.inc"
};
static NJS_CNK_OBJECT draw_415 = {
#include "stg30/terrain/draw_415.inc"
};
static NJS_CNK_OBJECT draw_414 = {
#include "stg30/terrain/draw_414.inc"
};
static NJS_CNK_OBJECT draw_413 = {
#include "stg30/terrain/draw_413.inc"
};
static NJS_CNK_OBJECT draw_412 = {
#include "stg30/terrain/draw_412.inc"
};
static NJS_CNK_OBJECT draw_411 = {
#include "stg30/terrain/draw_411.inc"
};
static NJS_CNK_OBJECT draw_410 = {
#include "stg30/terrain/draw_410.inc"
};
static NJS_CNK_OBJECT draw_409 = {
#include "stg30/terrain/draw_409.inc"
};
static NJS_CNK_OBJECT draw_408 = {
#include "stg30/terrain/draw_408.inc"
};
static NJS_CNK_OBJECT draw_407 = {
#include "stg30/terrain/draw_407.inc"
};
static NJS_CNK_OBJECT draw_406 = {
#include "stg30/terrain/draw_406.inc"
};
static NJS_CNK_OBJECT draw_405 = {
#include "stg30/terrain/draw_405.inc"
};
static NJS_CNK_OBJECT draw_404 = {
#include "stg30/terrain/draw_404.inc"
};
static NJS_CNK_OBJECT draw_403 = {
#include "stg30/terrain/draw_403.inc"
};
static NJS_CNK_OBJECT draw_402 = {
#include "stg30/terrain/draw_402.inc"
};
static NJS_CNK_OBJECT draw_401 = {
#include "stg30/terrain/draw_401.inc"
};
static NJS_CNK_OBJECT draw_400 = {
#include "stg30/terrain/draw_400.inc"
};
static NJS_CNK_OBJECT draw_399 = {
#include "stg30/terrain/draw_399.inc"
};
static NJS_CNK_OBJECT draw_398 = {
#include "stg30/terrain/draw_398.inc"
};
static NJS_CNK_OBJECT draw_396 = {
#include "stg30/terrain/draw_396.inc"
};
static NJS_CNK_OBJECT draw_395 = {
#include "stg30/terrain/draw_395.inc"
};
static NJS_CNK_OBJECT draw_393 = {
#include "stg30/terrain/draw_393.inc"
};
static NJS_CNK_OBJECT draw_392 = {
#include "stg30/terrain/draw_392.inc"
};
static NJS_CNK_OBJECT draw_391 = {
#include "stg30/terrain/draw_391.inc"
};
static NJS_CNK_OBJECT draw_390 = {
#include "stg30/terrain/draw_390.inc"
};
static NJS_CNK_OBJECT draw_389 = {
#include "stg30/terrain/draw_389.inc"
};
static NJS_CNK_OBJECT draw_388 = {
#include "stg30/terrain/draw_388.inc"
};
static NJS_CNK_OBJECT draw_387 = {
#include "stg30/terrain/draw_387.inc"
};
static NJS_CNK_OBJECT draw_386 = {
#include "stg30/terrain/draw_386.inc"
};
static NJS_CNK_OBJECT draw_385 = {
#include "stg30/terrain/draw_385.inc"
};
static NJS_CNK_OBJECT draw_384 = {
#include "stg30/terrain/draw_384.inc"
};
static NJS_CNK_OBJECT draw_383 = {
#include "stg30/terrain/draw_383.inc"
};
static NJS_CNK_OBJECT draw_382 = {
#include "stg30/terrain/draw_382.inc"
};
static NJS_CNK_OBJECT draw_381 = {
#include "stg30/terrain/draw_381.inc"
};
static NJS_CNK_OBJECT draw_380 = {
#include "stg30/terrain/draw_380.inc"
};
static NJS_CNK_OBJECT draw_379 = {
#include "stg30/terrain/draw_379.inc"
};
static NJS_CNK_OBJECT draw_378 = {
#include "stg30/terrain/draw_378.inc"
};
static NJS_CNK_OBJECT draw_377 = {
#include "stg30/terrain/draw_377.inc"
};
static NJS_CNK_OBJECT draw_376 = {
#include "stg30/terrain/draw_376.inc"
};
static NJS_CNK_OBJECT draw_375 = {
#include "stg30/terrain/draw_375.inc"
};
static NJS_CNK_OBJECT draw_374 = {
#include "stg30/terrain/draw_374.inc"
};
static NJS_CNK_OBJECT draw_373 = {
#include "stg30/terrain/draw_373.inc"
};
static NJS_CNK_OBJECT draw_372 = {
#include "stg30/terrain/draw_372.inc"
};
static NJS_CNK_OBJECT draw_371 = {
#include "stg30/terrain/draw_371.inc"
};
static NJS_CNK_OBJECT draw_370 = {
#include "stg30/terrain/draw_370.inc"
};
static NJS_CNK_OBJECT draw_369 = {
#include "stg30/terrain/draw_369.inc"
};
static NJS_CNK_OBJECT draw_368 = {
#include "stg30/terrain/draw_368.inc"
};
static NJS_CNK_OBJECT draw_367 = {
#include "stg30/terrain/draw_367.inc"
};
static NJS_CNK_OBJECT draw_366 = {
#include "stg30/terrain/draw_366.inc"
};
static NJS_CNK_OBJECT draw_365 = {
#include "stg30/terrain/draw_365.inc"
};
static NJS_CNK_OBJECT draw_364 = {
#include "stg30/terrain/draw_364.inc"
};
static NJS_CNK_OBJECT draw_363 = {
#include "stg30/terrain/draw_363.inc"
};
static NJS_CNK_OBJECT draw_359 = {
#include "stg30/terrain/draw_359.inc"
};
static NJS_CNK_OBJECT draw_358 = {
#include "stg30/terrain/draw_358.inc"
};
static NJS_CNK_OBJECT draw_357 = {
#include "stg30/terrain/draw_357.inc"
};
static NJS_CNK_OBJECT draw_356 = {
#include "stg30/terrain/draw_356.inc"
};
static NJS_CNK_OBJECT draw_355 = {
#include "stg30/terrain/draw_355.inc"
};
static NJS_CNK_OBJECT draw_354 = {
#include "stg30/terrain/draw_354.inc"
};
static NJS_CNK_OBJECT draw_353 = {
#include "stg30/terrain/draw_353.inc"
};
static NJS_CNK_OBJECT draw_352 = {
#include "stg30/terrain/draw_352.inc"
};
static NJS_CNK_OBJECT draw_351 = {
#include "stg30/terrain/draw_351.inc"
};
static NJS_CNK_OBJECT draw_350 = {
#include "stg30/terrain/draw_350.inc"
};
static NJS_CNK_OBJECT draw_349 = {
#include "stg30/terrain/draw_349.inc"
};
static NJS_CNK_OBJECT draw_348 = {
#include "stg30/terrain/draw_348.inc"
};
static NJS_CNK_OBJECT draw_347 = {
#include "stg30/terrain/draw_347.inc"
};
static NJS_CNK_OBJECT draw_346 = {
#include "stg30/terrain/draw_346.inc"
};
static NJS_CNK_OBJECT draw_345 = {
#include "stg30/terrain/draw_345.inc"
};
static NJS_CNK_OBJECT draw_344 = {
#include "stg30/terrain/draw_344.inc"
};
static NJS_CNK_OBJECT draw_343 = {
#include "stg30/terrain/draw_343.inc"
};
static NJS_CNK_OBJECT draw_342 = {
#include "stg30/terrain/draw_342.inc"
};
static NJS_CNK_OBJECT draw_341 = {
#include "stg30/terrain/draw_341.inc"
};
static NJS_CNK_OBJECT draw_340 = {
#include "stg30/terrain/draw_340.inc"
};
static NJS_CNK_OBJECT draw_339 = {
#include "stg30/terrain/draw_339.inc"
};
static NJS_CNK_OBJECT draw_338 = {
#include "stg30/terrain/draw_338.inc"
};
static NJS_CNK_OBJECT draw_337 = {
#include "stg30/terrain/draw_337.inc"
};
static NJS_CNK_OBJECT draw_336 = {
#include "stg30/terrain/draw_336.inc"
};
static NJS_CNK_OBJECT draw_335 = {
#include "stg30/terrain/draw_335.inc"
};
static NJS_CNK_OBJECT draw_334 = {
#include "stg30/terrain/draw_334.inc"
};
static NJS_CNK_OBJECT draw_333 = {
#include "stg30/terrain/draw_333.inc"
};
static NJS_CNK_OBJECT draw_332 = {
#include "stg30/terrain/draw_332.inc"
};
static NJS_CNK_OBJECT draw_331 = {
#include "stg30/terrain/draw_331.inc"
};
static NJS_CNK_OBJECT draw_330 = {
#include "stg30/terrain/draw_330.inc"
};
static NJS_CNK_OBJECT draw_329 = {
#include "stg30/terrain/draw_329.inc"
};
static NJS_CNK_OBJECT draw_328 = {
#include "stg30/terrain/draw_328.inc"
};
static NJS_CNK_OBJECT draw_327 = {
#include "stg30/terrain/draw_327.inc"
};
static NJS_CNK_OBJECT draw_326 = {
#include "stg30/terrain/draw_326.inc"
};
static NJS_CNK_OBJECT draw_325 = {
#include "stg30/terrain/draw_325.inc"
};
static NJS_CNK_OBJECT draw_324 = {
#include "stg30/terrain/draw_324.inc"
};
static NJS_CNK_OBJECT draw_323 = {
#include "stg30/terrain/draw_323.inc"
};
static NJS_CNK_OBJECT draw_322 = {
#include "stg30/terrain/draw_322.inc"
};
static NJS_CNK_OBJECT draw_321 = {
#include "stg30/terrain/draw_321.inc"
};
static NJS_CNK_OBJECT draw_320 = {
#include "stg30/terrain/draw_320.inc"
};
static NJS_CNK_OBJECT draw_319 = {
#include "stg30/terrain/draw_319.inc"
};
static NJS_CNK_OBJECT draw_318 = {
#include "stg30/terrain/draw_318.inc"
};
static NJS_CNK_OBJECT draw_317 = {
#include "stg30/terrain/draw_317.inc"
};
static NJS_CNK_OBJECT draw_316 = {
#include "stg30/terrain/draw_316.inc"
};
static NJS_CNK_OBJECT draw_315 = {
#include "stg30/terrain/draw_315.inc"
};
static NJS_CNK_OBJECT draw_314 = {
#include "stg30/terrain/draw_314.inc"
};
static NJS_CNK_OBJECT draw_313 = {
#include "stg30/terrain/draw_313.inc"
};
static NJS_CNK_OBJECT draw_312 = {
#include "stg30/terrain/draw_312.inc"
};
static NJS_CNK_OBJECT draw_311 = {
#include "stg30/terrain/draw_311.inc"
};
static NJS_CNK_OBJECT draw_310 = {
#include "stg30/terrain/draw_310.inc"
};
static NJS_CNK_OBJECT draw_309 = {
#include "stg30/terrain/draw_309.inc"
};
static NJS_CNK_OBJECT draw_308 = {
#include "stg30/terrain/draw_308.inc"
};
static NJS_CNK_OBJECT draw_307 = {
#include "stg30/terrain/draw_307.inc"
};
static NJS_CNK_OBJECT draw_306 = {
#include "stg30/terrain/draw_306.inc"
};
static NJS_CNK_OBJECT draw_305 = {
#include "stg30/terrain/draw_305.inc"
};
static NJS_CNK_OBJECT draw_304 = {
#include "stg30/terrain/draw_304.inc"
};
static NJS_CNK_OBJECT draw_303 = {
#include "stg30/terrain/draw_303.inc"
};
static NJS_CNK_OBJECT draw_302 = {
#include "stg30/terrain/draw_302.inc"
};
static NJS_CNK_OBJECT draw_301 = {
#include "stg30/terrain/draw_301.inc"
};
static NJS_CNK_OBJECT draw_300 = {
#include "stg30/terrain/draw_300.inc"
};
static NJS_CNK_OBJECT draw_299 = {
#include "stg30/terrain/draw_299.inc"
};
static NJS_CNK_OBJECT draw_298 = {
#include "stg30/terrain/draw_298.inc"
};
static NJS_CNK_OBJECT draw_297 = {
#include "stg30/terrain/draw_297.inc"
};
static NJS_CNK_OBJECT draw_296 = {
#include "stg30/terrain/draw_296.inc"
};
static NJS_CNK_OBJECT draw_295 = {
#include "stg30/terrain/draw_295.inc"
};
static NJS_CNK_OBJECT draw_294 = {
#include "stg30/terrain/draw_294.inc"
};
static NJS_CNK_OBJECT draw_293 = {
#include "stg30/terrain/draw_293.inc"
};
static NJS_CNK_OBJECT draw_292 = {
#include "stg30/terrain/draw_292.inc"
};
static NJS_CNK_OBJECT draw_291 = {
#include "stg30/terrain/draw_291.inc"
};
static NJS_CNK_OBJECT draw_290 = {
#include "stg30/terrain/draw_290.inc"
};
static NJS_CNK_OBJECT draw_289 = {
#include "stg30/terrain/draw_289.inc"
};
static NJS_CNK_OBJECT draw_288 = {
#include "stg30/terrain/draw_288.inc"
};
static NJS_CNK_OBJECT draw_287 = {
#include "stg30/terrain/draw_287.inc"
};
static NJS_CNK_OBJECT draw_286 = {
#include "stg30/terrain/draw_286.inc"
};
static NJS_CNK_OBJECT draw_285 = {
#include "stg30/terrain/draw_285.inc"
};
static NJS_CNK_OBJECT draw_284 = {
#include "stg30/terrain/draw_284.inc"
};
static NJS_CNK_OBJECT draw_283 = {
#include "stg30/terrain/draw_283.inc"
};
static NJS_CNK_OBJECT draw_282 = {
#include "stg30/terrain/draw_282.inc"
};
static NJS_CNK_OBJECT draw_281 = {
#include "stg30/terrain/draw_281.inc"
};
static NJS_CNK_OBJECT draw_280 = {
#include "stg30/terrain/draw_280.inc"
};
static NJS_CNK_OBJECT draw_279 = {
#include "stg30/terrain/draw_279.inc"
};
static NJS_CNK_OBJECT draw_278 = {
#include "stg30/terrain/draw_278.inc"
};
static NJS_CNK_OBJECT draw_277 = {
#include "stg30/terrain/draw_277.inc"
};
static NJS_CNK_OBJECT draw_276 = {
#include "stg30/terrain/draw_276.inc"
};
static NJS_CNK_OBJECT draw_275 = {
#include "stg30/terrain/draw_275.inc"
};
static NJS_CNK_OBJECT draw_274 = {
#include "stg30/terrain/draw_274.inc"
};
static NJS_CNK_OBJECT draw_273 = {
#include "stg30/terrain/draw_273.inc"
};
static NJS_CNK_OBJECT draw_272 = {
#include "stg30/terrain/draw_272.inc"
};
static NJS_CNK_OBJECT draw_271 = {
#include "stg30/terrain/draw_271.inc"
};
static NJS_CNK_OBJECT draw_270 = {
#include "stg30/terrain/draw_270.inc"
};
static NJS_CNK_OBJECT draw_269 = {
#include "stg30/terrain/draw_269.inc"
};
static NJS_CNK_OBJECT draw_268 = {
#include "stg30/terrain/draw_268.inc"
};
static NJS_CNK_OBJECT draw_267 = {
#include "stg30/terrain/draw_267.inc"
};
static NJS_CNK_OBJECT draw_266 = {
#include "stg30/terrain/draw_266.inc"
};
static NJS_CNK_OBJECT draw_265 = {
#include "stg30/terrain/draw_265.inc"
};
static NJS_CNK_OBJECT draw_263 = {
#include "stg30/terrain/draw_263.inc"
};
static NJS_CNK_OBJECT draw_262 = {
#include "stg30/terrain/draw_262.inc"
};
static NJS_CNK_OBJECT draw_261 = {
#include "stg30/terrain/draw_261.inc"
};
static NJS_CNK_OBJECT draw_260 = {
#include "stg30/terrain/draw_260.inc"
};
static NJS_CNK_OBJECT draw_259 = {
#include "stg30/terrain/draw_259.inc"
};
static NJS_CNK_OBJECT draw_258 = {
#include "stg30/terrain/draw_258.inc"
};
static NJS_CNK_OBJECT draw_257 = {
#include "stg30/terrain/draw_257.inc"
};
static NJS_CNK_OBJECT draw_256 = {
#include "stg30/terrain/draw_256.inc"
};
static NJS_CNK_OBJECT draw_255 = {
#include "stg30/terrain/draw_255.inc"
};
static NJS_CNK_OBJECT draw_254 = {
#include "stg30/terrain/draw_254.inc"
};
static NJS_CNK_OBJECT draw_253 = {
#include "stg30/terrain/draw_253.inc"
};
static NJS_CNK_OBJECT draw_252 = {
#include "stg30/terrain/draw_252.inc"
};
static NJS_CNK_OBJECT draw_251 = {
#include "stg30/terrain/draw_251.inc"
};
static NJS_CNK_OBJECT draw_250 = {
#include "stg30/terrain/draw_250.inc"
};
static NJS_CNK_OBJECT draw_249 = {
#include "stg30/terrain/draw_249.inc"
};
static NJS_CNK_OBJECT draw_248 = {
#include "stg30/terrain/draw_248.inc"
};
static NJS_CNK_OBJECT draw_247 = {
#include "stg30/terrain/draw_247.inc"
};
static NJS_CNK_OBJECT draw_246 = {
#include "stg30/terrain/draw_246.inc"
};
static NJS_CNK_OBJECT draw_245 = {
#include "stg30/terrain/draw_245.inc"
};
static NJS_CNK_OBJECT draw_244 = {
#include "stg30/terrain/draw_244.inc"
};
static NJS_CNK_OBJECT draw_243 = {
#include "stg30/terrain/draw_243.inc"
};
static NJS_CNK_OBJECT draw_242 = {
#include "stg30/terrain/draw_242.inc"
};
static NJS_CNK_OBJECT draw_241 = {
#include "stg30/terrain/draw_241.inc"
};
static NJS_CNK_OBJECT draw_239 = {
#include "stg30/terrain/draw_239.inc"
};
static NJS_CNK_OBJECT draw_238 = {
#include "stg30/terrain/draw_238.inc"
};
static NJS_CNK_OBJECT draw_237 = {
#include "stg30/terrain/draw_237.inc"
};
static NJS_CNK_OBJECT draw_236 = {
#include "stg30/terrain/draw_236.inc"
};
static NJS_CNK_OBJECT draw_235 = {
#include "stg30/terrain/draw_235.inc"
};
static NJS_CNK_OBJECT draw_234 = {
#include "stg30/terrain/draw_234.inc"
};
static NJS_CNK_OBJECT draw_233 = {
#include "stg30/terrain/draw_233.inc"
};
static NJS_CNK_OBJECT draw_232 = {
#include "stg30/terrain/draw_232.inc"
};
static NJS_CNK_OBJECT draw_231 = {
#include "stg30/terrain/draw_231.inc"
};
static NJS_CNK_OBJECT draw_230 = {
#include "stg30/terrain/draw_230.inc"
};
static NJS_CNK_OBJECT draw_229 = {
#include "stg30/terrain/draw_229.inc"
};
static NJS_CNK_OBJECT draw_228 = {
#include "stg30/terrain/draw_228.inc"
};
static NJS_CNK_OBJECT draw_227 = {
#include "stg30/terrain/draw_227.inc"
};
static NJS_CNK_OBJECT draw_226 = {
#include "stg30/terrain/draw_226.inc"
};
static NJS_CNK_OBJECT draw_225 = {
#include "stg30/terrain/draw_225.inc"
};
static NJS_CNK_OBJECT draw_224 = {
#include "stg30/terrain/draw_224.inc"
};
static NJS_CNK_OBJECT draw_223 = {
#include "stg30/terrain/draw_223.inc"
};
static NJS_CNK_OBJECT draw_222 = {
#include "stg30/terrain/draw_222.inc"
};
static NJS_CNK_OBJECT draw_221 = {
#include "stg30/terrain/draw_221.inc"
};
static NJS_CNK_OBJECT draw_220 = {
#include "stg30/terrain/draw_220.inc"
};
static NJS_CNK_OBJECT draw_219 = {
#include "stg30/terrain/draw_219.inc"
};
static NJS_CNK_OBJECT draw_218 = {
#include "stg30/terrain/draw_218.inc"
};
static NJS_CNK_OBJECT draw_217 = {
#include "stg30/terrain/draw_217.inc"
};
static NJS_CNK_OBJECT draw_216 = {
#include "stg30/terrain/draw_216.inc"
};
static NJS_CNK_OBJECT draw_215 = {
#include "stg30/terrain/draw_215.inc"
};
static NJS_CNK_OBJECT draw_214 = {
#include "stg30/terrain/draw_214.inc"
};
static NJS_CNK_OBJECT draw_213 = {
#include "stg30/terrain/draw_213.inc"
};
static NJS_CNK_OBJECT draw_212 = {
#include "stg30/terrain/draw_212.inc"
};
static NJS_CNK_OBJECT draw_211 = {
#include "stg30/terrain/draw_211.inc"
};
static NJS_CNK_OBJECT draw_210 = {
#include "stg30/terrain/draw_210.inc"
};
static NJS_CNK_OBJECT draw_209 = {
#include "stg30/terrain/draw_209.inc"
};
static NJS_CNK_OBJECT draw_208 = {
#include "stg30/terrain/draw_208.inc"
};
static NJS_CNK_OBJECT draw_207 = {
#include "stg30/terrain/draw_207.inc"
};
static NJS_CNK_OBJECT draw_206 = {
#include "stg30/terrain/draw_206.inc"
};
static NJS_CNK_OBJECT draw_205 = {
#include "stg30/terrain/draw_205.inc"
};
static NJS_CNK_OBJECT draw_204 = {
#include "stg30/terrain/draw_204.inc"
};
static NJS_CNK_OBJECT draw_203 = {
#include "stg30/terrain/draw_203.inc"
};
static NJS_CNK_OBJECT draw_202 = {
#include "stg30/terrain/draw_202.inc"
};
static NJS_CNK_OBJECT draw_201 = {
#include "stg30/terrain/draw_201.inc"
};
static NJS_CNK_OBJECT draw_200 = {
#include "stg30/terrain/draw_200.inc"
};
static NJS_CNK_OBJECT draw_199 = {
#include "stg30/terrain/draw_199.inc"
};
static NJS_CNK_OBJECT draw_198 = {
#include "stg30/terrain/draw_198.inc"
};
static NJS_CNK_OBJECT draw_197 = {
#include "stg30/terrain/draw_197.inc"
};
static NJS_CNK_OBJECT draw_196 = {
#include "stg30/terrain/draw_196.inc"
};
static NJS_CNK_OBJECT draw_195 = {
#include "stg30/terrain/draw_195.inc"
};
static NJS_CNK_OBJECT draw_194 = {
#include "stg30/terrain/draw_194.inc"
};
static NJS_CNK_OBJECT draw_193 = {
#include "stg30/terrain/draw_193.inc"
};
static NJS_CNK_OBJECT draw_192 = {
#include "stg30/terrain/draw_192.inc"
};
static NJS_CNK_OBJECT draw_191 = {
#include "stg30/terrain/draw_191.inc"
};
static NJS_CNK_OBJECT draw_190 = {
#include "stg30/terrain/draw_190.inc"
};
static NJS_CNK_OBJECT draw_189 = {
#include "stg30/terrain/draw_189.inc"
};
static NJS_CNK_OBJECT draw_188 = {
#include "stg30/terrain/draw_188.inc"
};
static NJS_CNK_OBJECT draw_187 = {
#include "stg30/terrain/draw_187.inc"
};
static NJS_CNK_OBJECT draw_186 = {
#include "stg30/terrain/draw_186.inc"
};
static NJS_CNK_OBJECT draw_185 = {
#include "stg30/terrain/draw_185.inc"
};
static NJS_CNK_OBJECT draw_184 = {
#include "stg30/terrain/draw_184.inc"
};
static NJS_CNK_OBJECT draw_183 = {
#include "stg30/terrain/draw_183.inc"
};
static NJS_CNK_OBJECT draw_182 = {
#include "stg30/terrain/draw_182.inc"
};
static NJS_CNK_OBJECT draw_181 = {
#include "stg30/terrain/draw_181.inc"
};
static NJS_CNK_OBJECT draw_180 = {
#include "stg30/terrain/draw_180.inc"
};
static NJS_CNK_OBJECT draw_179 = {
#include "stg30/terrain/draw_179.inc"
};
static NJS_CNK_OBJECT draw_178 = {
#include "stg30/terrain/draw_178.inc"
};
static NJS_CNK_OBJECT draw_177 = {
#include "stg30/terrain/draw_177.inc"
};
static NJS_CNK_OBJECT draw_176 = {
#include "stg30/terrain/draw_176.inc"
};
static NJS_CNK_OBJECT draw_175 = {
#include "stg30/terrain/draw_175.inc"
};
static NJS_CNK_OBJECT draw_174 = {
#include "stg30/terrain/draw_174.inc"
};
static NJS_CNK_OBJECT draw_173 = {
#include "stg30/terrain/draw_173.inc"
};
static NJS_CNK_OBJECT draw_172 = {
#include "stg30/terrain/draw_172.inc"
};
static NJS_CNK_OBJECT draw_171 = {
#include "stg30/terrain/draw_171.inc"
};
static NJS_CNK_OBJECT draw_170 = {
#include "stg30/terrain/draw_170.inc"
};
static NJS_CNK_OBJECT draw_169 = {
#include "stg30/terrain/draw_169.inc"
};
static NJS_CNK_OBJECT draw_168 = {
#include "stg30/terrain/draw_168.inc"
};
static NJS_CNK_OBJECT draw_167 = {
#include "stg30/terrain/draw_167.inc"
};
static NJS_CNK_OBJECT draw_166 = {
#include "stg30/terrain/draw_166.inc"
};
static NJS_CNK_OBJECT draw_165 = {
#include "stg30/terrain/draw_165.inc"
};
static NJS_CNK_OBJECT draw_164 = {
#include "stg30/terrain/draw_164.inc"
};
static NJS_CNK_OBJECT draw_163 = {
#include "stg30/terrain/draw_163.inc"
};
static NJS_CNK_OBJECT draw_162 = {
#include "stg30/terrain/draw_162.inc"
};
static NJS_CNK_OBJECT draw_161 = {
#include "stg30/terrain/draw_161.inc"
};
static NJS_CNK_OBJECT draw_160 = {
#include "stg30/terrain/draw_160.inc"
};
static NJS_CNK_OBJECT draw_159 = {
#include "stg30/terrain/draw_159.inc"
};
static NJS_CNK_OBJECT draw_158 = {
#include "stg30/terrain/draw_158.inc"
};
static NJS_CNK_OBJECT draw_157 = {
#include "stg30/terrain/draw_157.inc"
};
static NJS_CNK_OBJECT draw_156 = {
#include "stg30/terrain/draw_156.inc"
};
static NJS_CNK_OBJECT draw_155 = {
#include "stg30/terrain/draw_155.inc"
};
static NJS_CNK_OBJECT draw_154 = {
#include "stg30/terrain/draw_154.inc"
};
static NJS_CNK_OBJECT draw_153 = {
#include "stg30/terrain/draw_153.inc"
};
static NJS_CNK_OBJECT draw_152 = {
#include "stg30/terrain/draw_152.inc"
};
static NJS_CNK_OBJECT draw_151 = {
#include "stg30/terrain/draw_151.inc"
};
static NJS_CNK_OBJECT draw_150 = {
#include "stg30/terrain/draw_150.inc"
};
static NJS_CNK_OBJECT draw_149 = {
#include "stg30/terrain/draw_149.inc"
};
static NJS_CNK_OBJECT draw_148 = {
#include "stg30/terrain/draw_148.inc"
};
static NJS_CNK_OBJECT draw_147 = {
#include "stg30/terrain/draw_147.inc"
};
static NJS_CNK_OBJECT draw_146 = {
#include "stg30/terrain/draw_146.inc"
};
static NJS_CNK_OBJECT draw_145 = {
#include "stg30/terrain/draw_145.inc"
};
static NJS_CNK_OBJECT draw_144 = {
#include "stg30/terrain/draw_144.inc"
};
static NJS_CNK_OBJECT draw_143 = {
#include "stg30/terrain/draw_143.inc"
};
static NJS_CNK_OBJECT draw_142 = {
#include "stg30/terrain/draw_142.inc"
};
static NJS_CNK_OBJECT draw_141 = {
#include "stg30/terrain/draw_141.inc"
};
static NJS_CNK_OBJECT draw_140 = {
#include "stg30/terrain/draw_140.inc"
};
static NJS_CNK_OBJECT draw_139 = {
#include "stg30/terrain/draw_139.inc"
};
static NJS_CNK_OBJECT draw_138 = {
#include "stg30/terrain/draw_138.inc"
};
static NJS_CNK_OBJECT draw_137 = {
#include "stg30/terrain/draw_137.inc"
};
static NJS_CNK_OBJECT draw_136 = {
#include "stg30/terrain/draw_136.inc"
};
static NJS_CNK_OBJECT draw_135 = {
#include "stg30/terrain/draw_135.inc"
};
static NJS_CNK_OBJECT draw_134 = {
#include "stg30/terrain/draw_134.inc"
};
static NJS_CNK_OBJECT draw_133 = {
#include "stg30/terrain/draw_133.inc"
};
static NJS_CNK_OBJECT draw_132 = {
#include "stg30/terrain/draw_132.inc"
};
static NJS_CNK_OBJECT draw_131 = {
#include "stg30/terrain/draw_131.inc"
};
static NJS_CNK_OBJECT draw_130 = {
#include "stg30/terrain/draw_130.inc"
};
static NJS_CNK_OBJECT draw_129 = {
#include "stg30/terrain/draw_129.inc"
};
static NJS_CNK_OBJECT draw_128 = {
#include "stg30/terrain/draw_128.inc"
};
static NJS_CNK_OBJECT draw_127 = {
#include "stg30/terrain/draw_127.inc"
};
static NJS_CNK_OBJECT draw_126 = {
#include "stg30/terrain/draw_126.inc"
};
static NJS_CNK_OBJECT draw_125 = {
#include "stg30/terrain/draw_125.inc"
};
static NJS_CNK_OBJECT draw_124 = {
#include "stg30/terrain/draw_124.inc"
};
static NJS_CNK_OBJECT draw_123 = {
#include "stg30/terrain/draw_123.inc"
};
static NJS_CNK_OBJECT draw_122 = {
#include "stg30/terrain/draw_122.inc"
};
static NJS_CNK_OBJECT draw_121 = {
#include "stg30/terrain/draw_121.inc"
};
static NJS_CNK_OBJECT draw_120 = {
#include "stg30/terrain/draw_120.inc"
};
static NJS_CNK_OBJECT draw_119 = {
#include "stg30/terrain/draw_119.inc"
};
static NJS_CNK_OBJECT draw_118 = {
#include "stg30/terrain/draw_118.inc"
};
static NJS_CNK_OBJECT draw_117 = {
#include "stg30/terrain/draw_117.inc"
};
static NJS_CNK_OBJECT draw_116 = {
#include "stg30/terrain/draw_116.inc"
};
static NJS_CNK_OBJECT draw_115 = {
#include "stg30/terrain/draw_115.inc"
};
static NJS_CNK_OBJECT draw_114 = {
#include "stg30/terrain/draw_114.inc"
};
static NJS_CNK_OBJECT draw_113 = {
#include "stg30/terrain/draw_113.inc"
};
static NJS_CNK_OBJECT draw_112 = {
#include "stg30/terrain/draw_112.inc"
};
static NJS_CNK_OBJECT draw_111 = {
#include "stg30/terrain/draw_111.inc"
};
static NJS_CNK_OBJECT draw_110 = {
#include "stg30/terrain/draw_110.inc"
};
static NJS_CNK_OBJECT draw_109 = {
#include "stg30/terrain/draw_109.inc"
};
static NJS_CNK_OBJECT draw_108 = {
#include "stg30/terrain/draw_108.inc"
};
static NJS_CNK_OBJECT draw_107 = {
#include "stg30/terrain/draw_107.inc"
};
static NJS_CNK_OBJECT draw_106 = {
#include "stg30/terrain/draw_106.inc"
};
static NJS_CNK_OBJECT draw_105 = {
#include "stg30/terrain/draw_105.inc"
};
static NJS_CNK_OBJECT draw_104 = {
#include "stg30/terrain/draw_104.inc"
};
static NJS_CNK_OBJECT draw_103 = {
#include "stg30/terrain/draw_103.inc"
};
static NJS_CNK_OBJECT draw_102 = {
#include "stg30/terrain/draw_102.inc"
};
static NJS_CNK_OBJECT draw_101 = {
#include "stg30/terrain/draw_101.inc"
};
static NJS_CNK_OBJECT draw_100 = {
#include "stg30/terrain/draw_100.inc"
};
static NJS_CNK_OBJECT draw_99 = {
#include "stg30/terrain/draw_99.inc"
};
static NJS_CNK_OBJECT draw_98 = {
#include "stg30/terrain/draw_98.inc"
};
static NJS_CNK_OBJECT draw_97 = {
#include "stg30/terrain/draw_97.inc"
};
static NJS_CNK_OBJECT draw_96 = {
#include "stg30/terrain/draw_96.inc"
};
static NJS_CNK_OBJECT draw_95 = {
#include "stg30/terrain/draw_95.inc"
};
static NJS_CNK_OBJECT draw_94 = {
#include "stg30/terrain/draw_94.inc"
};
static NJS_CNK_OBJECT draw_93 = {
#include "stg30/terrain/draw_93.inc"
};
static NJS_CNK_OBJECT draw_92 = {
#include "stg30/terrain/draw_92.inc"
};
static NJS_CNK_OBJECT draw_91 = {
#include "stg30/terrain/draw_91.inc"
};
static NJS_CNK_OBJECT draw_90 = {
#include "stg30/terrain/draw_90.inc"
};
static NJS_CNK_OBJECT draw_89 = {
#include "stg30/terrain/draw_89.inc"
};
static NJS_CNK_OBJECT draw_88 = {
#include "stg30/terrain/draw_88.inc"
};
static NJS_CNK_OBJECT draw_87 = {
#include "stg30/terrain/draw_87.inc"
};
static NJS_CNK_OBJECT draw_86 = {
#include "stg30/terrain/draw_86.inc"
};
static NJS_CNK_OBJECT draw_85 = {
#include "stg30/terrain/draw_85.inc"
};
static NJS_CNK_OBJECT draw_84 = {
#include "stg30/terrain/draw_84.inc"
};
static NJS_CNK_OBJECT draw_83 = {
#include "stg30/terrain/draw_83.inc"
};
static NJS_CNK_OBJECT draw_82 = {
#include "stg30/terrain/draw_82.inc"
};
static NJS_CNK_OBJECT draw_81 = {
#include "stg30/terrain/draw_81.inc"
};
static NJS_CNK_OBJECT draw_80 = {
#include "stg30/terrain/draw_80.inc"
};
static NJS_CNK_OBJECT draw_79 = {
#include "stg30/terrain/draw_79.inc"
};
static NJS_CNK_OBJECT draw_78 = {
#include "stg30/terrain/draw_78.inc"
};
static NJS_CNK_OBJECT draw_77 = {
#include "stg30/terrain/draw_77.inc"
};
static NJS_CNK_OBJECT draw_76 = {
#include "stg30/terrain/draw_76.inc"
};
static NJS_CNK_OBJECT draw_75 = {
#include "stg30/terrain/draw_75.inc"
};
static NJS_CNK_OBJECT draw_74 = {
#include "stg30/terrain/draw_74.inc"
};
static NJS_CNK_OBJECT draw_73 = {
#include "stg30/terrain/draw_73.inc"
};
static NJS_CNK_OBJECT draw_72 = {
#include "stg30/terrain/draw_72.inc"
};
static NJS_CNK_OBJECT draw_71 = {
#include "stg30/terrain/draw_71.inc"
};
static NJS_CNK_OBJECT draw_70 = {
#include "stg30/terrain/draw_70.inc"
};
static NJS_CNK_OBJECT draw_69 = {
#include "stg30/terrain/draw_69.inc"
};
static NJS_CNK_OBJECT draw_68 = {
#include "stg30/terrain/draw_68.inc"
};
static NJS_CNK_OBJECT draw_67 = {
#include "stg30/terrain/draw_67.inc"
};
static NJS_CNK_OBJECT draw_65 = {
#include "stg30/terrain/draw_65.inc"
};
static NJS_CNK_OBJECT draw_64 = {
#include "stg30/terrain/draw_64.inc"
};
static NJS_CNK_OBJECT draw_63 = {
#include "stg30/terrain/draw_63.inc"
};
static NJS_CNK_OBJECT draw_62 = {
#include "stg30/terrain/draw_62.inc"
};
static NJS_CNK_OBJECT draw_61 = {
#include "stg30/terrain/draw_61.inc"
};
static NJS_CNK_OBJECT draw_60 = {
#include "stg30/terrain/draw_60.inc"
};
static NJS_CNK_OBJECT draw_59 = {
#include "stg30/terrain/draw_59.inc"
};
static NJS_CNK_OBJECT draw_58 = {
#include "stg30/terrain/draw_58.inc"
};
static NJS_CNK_OBJECT draw_57 = {
#include "stg30/terrain/draw_57.inc"
};
static NJS_CNK_OBJECT draw_56 = {
#include "stg30/terrain/draw_56.inc"
};
static NJS_CNK_OBJECT draw_55 = {
#include "stg30/terrain/draw_55.inc"
};
static NJS_CNK_OBJECT draw_54 = {
#include "stg30/terrain/draw_54.inc"
};
static NJS_CNK_OBJECT draw_53 = {
#include "stg30/terrain/draw_53.inc"
};
static NJS_CNK_OBJECT draw_52 = {
#include "stg30/terrain/draw_52.inc"
};
static NJS_CNK_OBJECT draw_51 = {
#include "stg30/terrain/draw_51.inc"
};
static NJS_CNK_OBJECT draw_50 = {
#include "stg30/terrain/draw_50.inc"
};
static NJS_CNK_OBJECT draw_49 = {
#include "stg30/terrain/draw_49.inc"
};
static NJS_CNK_OBJECT draw_48 = {
#include "stg30/terrain/draw_48.inc"
};
static NJS_CNK_OBJECT draw_47 = {
#include "stg30/terrain/draw_47.inc"
};
static NJS_CNK_OBJECT draw_46 = {
#include "stg30/terrain/draw_46.inc"
};
static NJS_CNK_OBJECT draw_45 = {
#include "stg30/terrain/draw_45.inc"
};
static NJS_CNK_OBJECT draw_44 = {
#include "stg30/terrain/draw_44.inc"
};
static NJS_CNK_OBJECT draw_43 = {
#include "stg30/terrain/draw_43.inc"
};
static NJS_CNK_OBJECT draw_42 = {
#include "stg30/terrain/draw_42.inc"
};
static NJS_CNK_OBJECT draw_41 = {
#include "stg30/terrain/draw_41.inc"
};
static NJS_CNK_OBJECT draw_40 = {
#include "stg30/terrain/draw_40.inc"
};
static NJS_CNK_OBJECT draw_39 = {
#include "stg30/terrain/draw_39.inc"
};
static NJS_CNK_OBJECT draw_38 = {
#include "stg30/terrain/draw_38.inc"
};
static NJS_CNK_OBJECT draw_37 = {
#include "stg30/terrain/draw_37.inc"
};
static NJS_CNK_OBJECT draw_36 = {
#include "stg30/terrain/draw_36.inc"
};
static NJS_CNK_OBJECT draw_35 = {
#include "stg30/terrain/draw_35.inc"
};
static NJS_CNK_OBJECT draw_34 = {
#include "stg30/terrain/draw_34.inc"
};
static NJS_CNK_OBJECT draw_33 = {
#include "stg30/terrain/draw_33.inc"
};
static NJS_CNK_OBJECT draw_32 = {
#include "stg30/terrain/draw_32.inc"
};
static NJS_CNK_OBJECT draw_31 = {
#include "stg30/terrain/draw_31.inc"
};
static NJS_CNK_OBJECT draw_30 = {
#include "stg30/terrain/draw_30.inc"
};
static NJS_CNK_OBJECT draw_29 = {
#include "stg30/terrain/draw_29.inc"
};
static NJS_CNK_OBJECT draw_28 = {
#include "stg30/terrain/draw_28.inc"
};
static NJS_CNK_OBJECT draw_27 = {
#include "stg30/terrain/draw_27.inc"
};
static NJS_CNK_OBJECT draw_26 = {
#include "stg30/terrain/draw_26.inc"
};
static NJS_CNK_OBJECT draw_25 = {
#include "stg30/terrain/draw_25.inc"
};
static NJS_CNK_OBJECT draw_24 = {
#include "stg30/terrain/draw_24.inc"
};
static NJS_CNK_OBJECT draw_23 = {
#include "stg30/terrain/draw_23.inc"
};
static NJS_CNK_OBJECT draw_22 = {
#include "stg30/terrain/draw_22.inc"
};
static NJS_CNK_OBJECT draw_21 = {
#include "stg30/terrain/draw_21.inc"
};
static NJS_CNK_OBJECT draw_20 = {
#include "stg30/terrain/draw_20.inc"
};
static NJS_CNK_OBJECT draw_19 = {
#include "stg30/terrain/draw_19.inc"
};
static NJS_CNK_OBJECT draw_18 = {
#include "stg30/terrain/draw_18.inc"
};
static NJS_CNK_OBJECT draw_17 = {
#include "stg30/terrain/draw_17.inc"
};
static NJS_CNK_OBJECT draw_16 = {
#include "stg30/terrain/draw_16.inc"
};
static NJS_CNK_OBJECT draw_15 = {
#include "stg30/terrain/draw_15.inc"
};
static NJS_CNK_OBJECT draw_14 = {
#include "stg30/terrain/draw_14.inc"
};
static NJS_CNK_OBJECT draw_13 = {
#include "stg30/terrain/draw_13.inc"
};
static NJS_CNK_OBJECT draw_12 = {
#include "stg30/terrain/draw_12.inc"
};
static NJS_CNK_OBJECT draw_11 = {
#include "stg30/terrain/draw_11.inc"
};
static NJS_CNK_OBJECT draw_10 = {
#include "stg30/terrain/draw_10.inc"
};
static NJS_CNK_OBJECT draw_9 = {
#include "stg30/terrain/draw_9.inc"
};
static NJS_CNK_OBJECT draw_8 = {
#include "stg30/terrain/draw_8.inc"
};
static NJS_CNK_OBJECT draw_7 = {
#include "stg30/terrain/draw_7.inc"
};
static NJS_CNK_OBJECT draw_6 = {
#include "stg30/terrain/draw_6.inc"
};
static NJS_CNK_OBJECT draw_5 = {
#include "stg30/terrain/draw_5.inc"
};
static NJS_CNK_OBJECT draw_4 = {
#include "stg30/terrain/draw_4.inc"
};
static NJS_CNK_OBJECT draw_3 = {
#include "stg30/terrain/draw_3.inc"
};
static NJS_CNK_OBJECT draw_2 = {
#include "stg30/terrain/draw_2.inc"
};
static NJS_CNK_OBJECT draw_1 = {
#include "stg30/terrain/draw_1.inc"
};
static NJS_CNK_OBJECT draw_0 = {
#include "stg30/terrain/draw_0.inc"
};
