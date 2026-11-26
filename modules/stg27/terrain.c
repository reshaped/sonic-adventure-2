#include <ninja.h>
#include <sg_xpt.h>

#include "collision/world.h"
#include "stg27/lost_colony.h"

/* the land's file names, which its land table and texture list point at. each
 * starts on a 4-byte boundary */
const char lost_colony_land_filenames[489] = {
#include "stg27/terrain/lost_colony_land_filenames.inc"
};

/* STG27.PRS's land, texture lists, chunk models and Basic models, from the
 * disc. its D lies between lost_colony.c and poison_land.c */

/* the data up to the last that a pointer names ahead of its definition, as
 * tentative definitions in the disc's order: GCC places each datum at its
 * first definition */
static struct land_entry entries[506];
struct land_table lost_colony_land;
static NJS_TEXNAME textures[36];
NJS_TEXLIST lost_colony_land_texlist;
static Sint16 draw_199_polygons[125];
static Sint32 draw_199_vertices[115];
static NJS_CNK_MODEL draw_199_model;
static NJS_CNK_OBJECT draw_267;
static Sint16 draw_97_polygons[311];
static Sint32 draw_97_vertices[351];
static NJS_CNK_MODEL draw_97_model;
static NJS_CNK_OBJECT draw_266;
static Sint16 draw_102_polygons[233];
static Sint32 draw_102_vertices[131];
static NJS_CNK_MODEL draw_102_model;
static NJS_CNK_OBJECT draw_265;
static Sint16 draw_223_polygons[127];
static Sint32 draw_223_vertices[67];
static NJS_CNK_MODEL draw_223_model;
static NJS_CNK_OBJECT draw_263;
static Sint16 draw_95_polygons[81];
static Sint32 draw_95_vertices[83];
static NJS_CNK_MODEL draw_95_model;
static NJS_CNK_OBJECT draw_260;
static Sint16 draw_100_polygons[1269];
static Sint32 draw_100_vertices[1059];
static NJS_CNK_MODEL draw_100_model;
static NJS_CNK_OBJECT draw_259;
static Sint16 draw_10_polygons[453];
static Sint32 draw_10_vertices[483];
static NJS_CNK_MODEL draw_10_model;
static NJS_CNK_OBJECT draw_245;
static Sint16 draw_5_polygons[1225];
static Sint32 draw_5_vertices[1467];
static NJS_CNK_MODEL draw_5_model;
static NJS_CNK_OBJECT draw_239;
static Sint16 draw_1_polygons[197];
static Sint32 draw_1_vertices[163];
static NJS_CNK_MODEL draw_1_model;
static NJS_CNK_OBJECT draw_234;
static Sint16 draw_182_polygons[1805];
static Sint32 draw_182_vertices[2159];
static NJS_CNK_MODEL draw_182_model;
static NJS_CNK_OBJECT draw_182;
static Sint16 draw_181_polygons[197];
static Sint32 draw_181_vertices[163];
static NJS_CNK_MODEL draw_181_model;
static NJS_CNK_OBJECT draw_181;
static Sint16 draw_55_polygons[171];
static Sint32 draw_55_vertices[99];
static NJS_CNK_MODEL draw_55_model;
static NJS_CNK_OBJECT draw_180;
static Sint16 draw_176_polygons[109];
static Sint32 draw_176_vertices[91];
static NJS_CNK_MODEL draw_176_model;
static NJS_CNK_OBJECT draw_176;
static Sint16 draw_175_polygons[225];
static Sint32 draw_175_vertices[167];
static NJS_CNK_MODEL draw_175_model;
static NJS_CNK_OBJECT draw_175;
static Sint16 draw_174_polygons[133];
static Sint32 draw_174_vertices[83];
static NJS_CNK_MODEL draw_174_model;
static NJS_CNK_OBJECT draw_174;
static Sint16 draw_173_polygons[615];
static Sint32 draw_173_vertices[475];
static NJS_CNK_MODEL draw_173_model;
static NJS_CNK_OBJECT draw_173;
static Sint16 draw_153_polygons[207];
static Sint32 draw_153_vertices[131];
static NJS_CNK_MODEL draw_153_model;
static NJS_CNK_OBJECT draw_171;
static Sint16 draw_152_polygons[73];
static Sint32 draw_152_vertices[83];
static NJS_CNK_MODEL draw_152_model;
static NJS_CNK_OBJECT draw_170;
static Sint16 draw_157_polygons[391];
static Sint32 draw_157_vertices[203];
static NJS_CNK_MODEL draw_157_model;
static NJS_CNK_OBJECT draw_169;
static Sint16 draw_166_polygons[371];
static Sint32 draw_166_vertices[251];
static NJS_CNK_MODEL draw_166_model;
static NJS_CNK_OBJECT draw_166;
static Sint16 draw_155_polygons[189];
static Sint32 draw_155_vertices[115];
static NJS_CNK_MODEL draw_155_model;
static NJS_CNK_OBJECT draw_161;
static Sint16 draw_117_polygons[53];
static Sint32 draw_117_vertices[51];
static NJS_CNK_MODEL draw_117_model;
static NJS_CNK_OBJECT draw_151;
static Sint16 draw_122_polygons[135];
static Sint32 draw_122_vertices[123];
static NJS_CNK_MODEL draw_122_model;
static NJS_CNK_OBJECT draw_150;
static Sint16 draw_125_polygons[879];
static Sint32 draw_125_vertices[495];
static NJS_CNK_MODEL draw_125_model;
static NJS_CNK_OBJECT draw_149;
static Sint16 draw_132_polygons[145];
static Sint32 draw_132_vertices[163];
static NJS_CNK_MODEL draw_132_model;
static NJS_CNK_OBJECT draw_147;
static Sint16 draw_126_polygons[315];
static Sint32 draw_126_vertices[347];
static NJS_CNK_MODEL draw_126_model;
static NJS_CNK_OBJECT draw_146;
static Sint16 draw_138_polygons[179];
static Sint32 draw_138_vertices[187];
static NJS_CNK_MODEL draw_138_model;
static NJS_CNK_OBJECT draw_145;
static Sint16 draw_136_polygons[217];
static Sint32 draw_136_vertices[243];
static NJS_CNK_MODEL draw_136_model;
static NJS_CNK_OBJECT draw_137;
static Sint16 draw_129_polygons[37];
static Sint32 draw_129_vertices[35];
static NJS_CNK_MODEL draw_129_model;
static NJS_CNK_OBJECT draw_129;
static Sint16 draw_120_polygons[165];
static Sint32 draw_120_vertices[159];
static NJS_CNK_MODEL draw_120_model;
static NJS_CNK_OBJECT draw_120;
static Sint16 draw_115_polygons[165];
static Sint32 draw_115_vertices[151];
static NJS_CNK_MODEL draw_115_model;
static NJS_CNK_OBJECT draw_115;
static Sint16 draw_114_polygons[707];
static Sint32 draw_114_vertices[523];
static NJS_CNK_MODEL draw_114_model;
static NJS_CNK_OBJECT draw_114;
static Sint16 draw_84_polygons[473];
static Sint32 draw_84_vertices[351];
static NJS_CNK_MODEL draw_84_model;
static NJS_CNK_OBJECT draw_87;
static Sint16 draw_16_polygons[937];
static Sint32 draw_16_vertices[903];
static NJS_CNK_MODEL draw_16_model;
static NJS_CNK_OBJECT draw_77;
static Sint16 draw_14_polygons[507];
static Sint32 draw_14_vertices[427];
static NJS_CNK_MODEL draw_14_model;
static NJS_CNK_OBJECT draw_76;
static Sint16 draw_9_polygons[829];
static Sint32 draw_9_vertices[715];
static NJS_CNK_MODEL draw_9_model;
static NJS_CNK_OBJECT draw_75;
static Sint16 draw_70_polygons[515];
static Sint32 draw_70_vertices[267];
static NJS_CNK_MODEL draw_70_model;
static NJS_CNK_OBJECT draw_70;
static Sint16 draw_12_polygons[233];
static Sint32 draw_12_vertices[179];
static NJS_CNK_MODEL draw_12_model;
static NJS_CNK_OBJECT draw_60;
static Sint16 draw_7_polygons[201];
static Sint32 draw_7_vertices[179];
static NJS_CNK_MODEL draw_7_model;
static NJS_CNK_OBJECT draw_59;
static Sint16 draw_3_polygons[223];
static Sint32 draw_3_vertices[203];
static NJS_CNK_MODEL draw_3_model;
static NJS_CNK_OBJECT draw_58;
static Sint16 draw_50_polygons[393];
static Sint32 draw_50_vertices[203];
static NJS_CNK_MODEL draw_50_model;
static NJS_CNK_OBJECT draw_50;
static Sint16 draw_31_polygons[673];
static Sint32 draw_31_vertices[483];
static NJS_CNK_MODEL draw_31_model;
static NJS_CNK_OBJECT draw_42;
static Sint16 draw_36_polygons[23];
static Sint32 draw_36_vertices[19];
static NJS_CNK_MODEL draw_36_model;
static NJS_CNK_OBJECT draw_36;
static Sint16 draw_34_polygons[943];
static Sint32 draw_34_vertices[543];
static NJS_CNK_MODEL draw_34_model;
static NJS_CNK_OBJECT draw_34;
static Sint16 draw_33_polygons[797];
static Sint32 draw_33_vertices[523];
static NJS_CNK_MODEL draw_33_model;
static NJS_CNK_OBJECT draw_33;
static Sint16 draw_20_polygons[225];
static Sint32 draw_20_vertices[167];
static NJS_CNK_MODEL draw_20_model;
static NJS_CNK_OBJECT draw_20;
static Sint16 draw_19_polygons[133];
static Sint32 draw_19_vertices[83];
static NJS_CNK_MODEL draw_19_model;
static NJS_CNK_OBJECT draw_19;
static Sint16 draw_15_polygons[615];
static Sint32 draw_15_vertices[475];
static NJS_CNK_MODEL draw_15_model;
static NJS_CNK_OBJECT draw_15;
static Sint16 draw_0_polygons[23];
static Sint32 draw_0_vertices[19];
static NJS_CNK_MODEL draw_0_model;
static NJS_CNK_OBJECT draw_0;
static NJS_MATERIAL collision_0_materials[1];
static Sint16 collision_0_faces[204];
static NJS_MESHSET collision_0_meshsets[1];
static NJS_POINT3 collision_0_points[112];
static NJS_MODEL collision_0_model;
static NJS_OBJECT collision_0;
static NJS_MATERIAL collision_1_materials[1];
static Sint16 collision_1_faces[139];
static NJS_MESHSET collision_1_meshsets[1];
static NJS_POINT3 collision_1_points[80];
static NJS_MODEL collision_1_model;
static NJS_OBJECT collision_1;
static NJS_MATERIAL collision_2_materials[1];
static Sint16 collision_2_faces[78];
static NJS_MESHSET collision_2_meshsets[1];
static NJS_POINT3 collision_2_points[48];
static NJS_MODEL collision_2_model;
static NJS_OBJECT collision_2;
static NJS_MATERIAL collision_4_materials[1];
static Sint16 collision_4_faces[17];
static NJS_MESHSET collision_4_meshsets[1];
static NJS_POINT3 collision_4_points[14];
static NJS_MODEL collision_4_model;
static NJS_OBJECT collision_4;
static NJS_MATERIAL collision_5_materials[1];
static Sint16 collision_5_faces[17];
static NJS_MESHSET collision_5_meshsets[1];
static NJS_POINT3 collision_5_points[14];
static NJS_MODEL collision_5_model;
static NJS_OBJECT collision_5;
static NJS_MATERIAL collision_6_materials[1];
static Sint16 collision_6_faces[17];
static NJS_MESHSET collision_6_meshsets[1];
static NJS_POINT3 collision_6_points[14];
static NJS_MODEL collision_6_model;
static NJS_OBJECT collision_6;
static NJS_MATERIAL collision_8_materials[1];
static Sint16 collision_8_faces[36];
static NJS_MESHSET collision_8_meshsets[1];
static NJS_POINT3 collision_8_points[22];
static NJS_MODEL collision_8_model;
static NJS_OBJECT collision_8;
static NJS_MATERIAL collision_9_materials[1];
static Sint16 collision_9_faces[56];
static NJS_MESHSET collision_9_meshsets[1];
static NJS_POINT3 collision_9_points[24];
static NJS_MODEL collision_9_model;
static NJS_OBJECT collision_9;
static NJS_MATERIAL collision_11_materials[1];
static Sint16 collision_11_faces[38];
static NJS_MESHSET collision_11_meshsets[1];
static NJS_POINT3 collision_11_points[24];
static NJS_MODEL collision_11_model;
static NJS_OBJECT collision_11;
static NJS_MATERIAL collision_12_materials[1];
static Sint16 collision_12_faces[10];
static NJS_MESHSET collision_12_meshsets[1];
static NJS_POINT3 collision_12_points[8];
static NJS_MODEL collision_12_model;
static NJS_OBJECT collision_12;
static NJS_MATERIAL collision_13_materials[1];
static Sint16 collision_13_faces[69];
static NJS_MESHSET collision_13_meshsets[1];
static NJS_POINT3 collision_13_points[40];
static NJS_MODEL collision_13_model;
static NJS_OBJECT collision_13;
static NJS_MATERIAL collision_14_materials[1];
static Sint16 collision_14_faces[14];
static NJS_MESHSET collision_14_meshsets[1];
static NJS_POINT3 collision_14_points[12];
static NJS_MODEL collision_14_model;
static NJS_OBJECT collision_14;
static NJS_MATERIAL collision_18_materials[1];
static Sint16 collision_18_faces[36];
static NJS_MESHSET collision_18_meshsets[1];
static NJS_POINT3 collision_18_points[22];
static NJS_MODEL collision_18_model;
static NJS_OBJECT collision_18;
static NJS_MATERIAL collision_19_materials[1];
static Sint16 collision_19_faces[14];
static NJS_MESHSET collision_19_meshsets[1];
static NJS_POINT3 collision_19_points[12];
static NJS_MODEL collision_19_model;
static NJS_OBJECT collision_19;
static NJS_MATERIAL collision_37_materials[1];
static Sint16 collision_37_faces[69];
static NJS_MESHSET collision_37_meshsets[1];
static NJS_POINT3 collision_37_points[40];
static NJS_MODEL collision_37_model;
static NJS_OBJECT collision_37;
static NJS_MATERIAL collision_38_materials[1];
static Sint16 collision_38_faces[21];
static NJS_MESHSET collision_38_meshsets[1];
static NJS_POINT3 collision_38_points[12];
static NJS_MODEL collision_38_model;
static NJS_OBJECT collision_38;
static NJS_MATERIAL collision_41_materials[1];
static Sint16 collision_41_faces[39];
static NJS_MESHSET collision_41_meshsets[1];
static NJS_POINT3 collision_41_points[20];
static NJS_MODEL collision_41_model;
static NJS_OBJECT collision_41;
static NJS_MATERIAL collision_44_materials[1];
static Sint16 collision_44_faces[9];
static NJS_MESHSET collision_44_meshsets[1];
static NJS_POINT3 collision_44_points[8];
static NJS_MODEL collision_44_model;
static NJS_OBJECT collision_44;
static NJS_MATERIAL collision_45_materials[1];
static Sint16 collision_45_faces[14];
static NJS_MESHSET collision_45_meshsets[1];
static NJS_POINT3 collision_45_points[12];
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
static Sint16 collision_48_faces[45];
static NJS_MESHSET collision_48_meshsets[1];
static NJS_POINT3 collision_48_points[28];
static NJS_MODEL collision_48_model;
static NJS_OBJECT collision_48;
static NJS_MATERIAL collision_52_materials[1];
static Sint16 collision_52_faces[38];
static NJS_MESHSET collision_52_meshsets[1];
static NJS_POINT3 collision_52_points[24];
static NJS_MODEL collision_52_model;
static NJS_OBJECT collision_52;
static NJS_MATERIAL collision_58_materials[1];
static Sint16 collision_58_faces[10];
static NJS_MESHSET collision_58_meshsets[1];
static NJS_POINT3 collision_58_points[8];
static NJS_MODEL collision_58_model;
static NJS_OBJECT collision_58;
static NJS_MATERIAL collision_85_materials[1];
static Sint16 collision_85_faces[36];
static NJS_MESHSET collision_85_meshsets[1];
static NJS_POINT3 collision_85_points[22];
static NJS_MODEL collision_85_model;
static NJS_OBJECT collision_85;
static NJS_MATERIAL collision_86_materials[1];
static Sint16 collision_86_faces[46];
static NJS_MESHSET collision_86_meshsets[1];
static NJS_POINT3 collision_86_points[30];
static NJS_MODEL collision_86_model;
static NJS_OBJECT collision_86;
static NJS_MATERIAL collision_87_materials[1];
static Sint16 collision_87_faces[19];
static NJS_MESHSET collision_87_meshsets[1];
static NJS_POINT3 collision_87_points[14];
static NJS_MODEL collision_87_model;
static NJS_OBJECT collision_87;
static NJS_MATERIAL collision_88_materials[1];
static Sint16 collision_88_faces[69];
static NJS_MESHSET collision_88_meshsets[1];
static NJS_POINT3 collision_88_points[40];
static NJS_MODEL collision_88_model;
static NJS_OBJECT collision_88;
static NJS_MATERIAL collision_89_materials[1];
static Sint16 collision_89_faces[11];
static NJS_MESHSET collision_89_meshsets[1];
static NJS_POINT3 collision_89_points[10];
static NJS_MODEL collision_89_model;
static NJS_OBJECT collision_89;
static NJS_MATERIAL collision_95_materials[1];
static Sint16 collision_95_faces[18];
static NJS_MESHSET collision_95_meshsets[1];
static NJS_POINT3 collision_95_points[16];
static NJS_MODEL collision_95_model;
static NJS_OBJECT collision_95;
static NJS_MATERIAL collision_100_materials[1];
static Sint16 collision_100_faces[18];
static NJS_MESHSET collision_100_meshsets[1];
static NJS_POINT3 collision_100_points[16];
static NJS_MODEL collision_100_model;
static NJS_OBJECT collision_100;
static NJS_MATERIAL collision_110_materials[1];
static Sint16 collision_110_faces[92];
static NJS_MESHSET collision_110_meshsets[1];
static NJS_POINT3 collision_110_points[41];
static NJS_MODEL collision_110_model;
static NJS_OBJECT collision_110;
static NJS_MATERIAL collision_111_materials[1];
static Sint16 collision_111_faces[118];
static NJS_MESHSET collision_111_meshsets[1];
static NJS_POINT3 collision_111_points[53];
static NJS_MODEL collision_111_model;
static NJS_OBJECT collision_111;
static NJS_MATERIAL collision_113_materials[1];
static Sint16 collision_113_faces[90];
static NJS_MESHSET collision_113_meshsets[1];
static NJS_POINT3 collision_113_points[49];
static NJS_MODEL collision_113_model;
static NJS_OBJECT collision_113;
static NJS_MATERIAL collision_115_materials[1];
static Sint16 collision_115_faces[9];
static NJS_MESHSET collision_115_meshsets[1];
static NJS_POINT3 collision_115_points[8];
static NJS_MODEL collision_115_model;
static NJS_OBJECT collision_115;
static NJS_MATERIAL collision_116_materials[1];
static Sint16 collision_116_faces[50];
static NJS_MESHSET collision_116_meshsets[1];
static NJS_POINT3 collision_116_points[22];
static NJS_MODEL collision_116_model;
static NJS_OBJECT collision_116;
static NJS_MATERIAL collision_117_materials[1];
static Sint16 collision_117_faces[20];
static NJS_MESHSET collision_117_meshsets[1];
static NJS_POINT3 collision_117_points[10];
static NJS_MODEL collision_117_model;
static NJS_OBJECT collision_117;
static NJS_MATERIAL collision_118_materials[1];
static Sint16 collision_118_faces[61];
static NJS_MESHSET collision_118_meshsets[1];
static NJS_POINT3 collision_118_points[29];
static NJS_MODEL collision_118_model;
static NJS_OBJECT collision_118;
static NJS_MATERIAL collision_119_materials[1];
static Sint16 collision_119_faces[47];
static NJS_MESHSET collision_119_meshsets[1];
static NJS_POINT3 collision_119_points[30];
static NJS_MODEL collision_119_model;
static NJS_OBJECT collision_119;
static NJS_MATERIAL collision_120_materials[1];
static Sint16 collision_120_faces[14];
static NJS_MESHSET collision_120_meshsets[1];
static NJS_POINT3 collision_120_points[12];
static NJS_MODEL collision_120_model;
static NJS_OBJECT collision_120;
static NJS_MATERIAL collision_124_materials[1];
static Sint16 collision_124_faces[52];
static NJS_MESHSET collision_124_meshsets[1];
static NJS_POINT3 collision_124_points[24];
static NJS_MODEL collision_124_model;
static NJS_OBJECT collision_124;
static NJS_MATERIAL collision_128_materials[1];
static Sint16 collision_128_faces[20];
static NJS_MESHSET collision_128_meshsets[1];
static NJS_POINT3 collision_128_points[12];
static NJS_MODEL collision_128_model;
static NJS_OBJECT collision_128;
static NJS_MATERIAL collision_146_materials[1];
static Sint16 collision_146_faces[54];
static NJS_MESHSET collision_146_meshsets[1];
static NJS_POINT3 collision_146_points[27];
static NJS_MODEL collision_146_model;
static NJS_OBJECT collision_146;
static NJS_MATERIAL collision_147_materials[1];
static Sint16 collision_147_faces[14];
static NJS_MESHSET collision_147_meshsets[1];
static NJS_POINT3 collision_147_points[12];
static NJS_MODEL collision_147_model;
static NJS_OBJECT collision_147;
static NJS_MATERIAL collision_150_materials[1];
static Sint16 collision_150_faces[42];
static NJS_MESHSET collision_150_meshsets[1];
static NJS_POINT3 collision_150_points[21];
static NJS_MODEL collision_150_model;
static NJS_OBJECT collision_150;
static NJS_MATERIAL collision_151_materials[1];
static Sint16 collision_151_faces[14];
static NJS_MESHSET collision_151_meshsets[1];
static NJS_POINT3 collision_151_points[12];
static NJS_MODEL collision_151_model;
static NJS_OBJECT collision_151;
static NJS_MATERIAL collision_152_materials[1];
static Sint16 collision_152_faces[40];
static NJS_MESHSET collision_152_meshsets[1];
static NJS_POINT3 collision_152_points[24];
static NJS_MODEL collision_152_model;
static NJS_OBJECT collision_152;
static NJS_MATERIAL collision_154_materials[1];
static Sint16 collision_154_faces[25];
static NJS_MESHSET collision_154_meshsets[1];
static NJS_POINT3 collision_154_points[12];
static NJS_MODEL collision_154_model;
static NJS_OBJECT collision_154;
static NJS_MATERIAL collision_158_materials[1];
static Sint16 collision_158_faces[56];
static NJS_MESHSET collision_158_meshsets[1];
static NJS_POINT3 collision_158_points[25];
static NJS_MODEL collision_158_model;
static NJS_OBJECT collision_158;
static NJS_MATERIAL collision_159_materials[1];
static Sint16 collision_159_faces[20];
static NJS_MESHSET collision_159_meshsets[1];
static NJS_POINT3 collision_159_points[10];
static NJS_MODEL collision_159_model;
static NJS_OBJECT collision_159;
static NJS_MATERIAL collision_188_materials[1];
static Sint16 collision_188_faces[28];
static NJS_MESHSET collision_188_meshsets[1];
static NJS_POINT3 collision_188_points[21];
static NJS_MODEL collision_188_model;
static NJS_OBJECT collision_188;
static NJS_MATERIAL collision_189_materials[1];
static Sint16 collision_189_faces[7];
static NJS_MESHSET collision_189_meshsets[1];
static NJS_POINT3 collision_189_points[6];
static NJS_MODEL collision_189_model;
static NJS_OBJECT collision_189;
static NJS_MATERIAL collision_195_materials[1];
static Sint16 collision_195_faces[33];
static NJS_MESHSET collision_195_meshsets[1];
static NJS_POINT3 collision_195_points[14];
static NJS_MODEL collision_195_model;
static NJS_OBJECT collision_195;
static NJS_MATERIAL collision_199_materials[1];
static Sint16 collision_199_faces[10];
static NJS_MESHSET collision_199_meshsets[1];
static NJS_POINT3 collision_199_points[8];
static NJS_MODEL collision_199_model;
static NJS_OBJECT collision_199;
static NJS_MATERIAL collision_204_materials[1];
static Sint16 collision_204_faces[21];
static NJS_MESHSET collision_204_meshsets[1];
static NJS_POINT3 collision_204_points[8];
static NJS_MODEL collision_204_model;
static NJS_OBJECT collision_204;
static NJS_MATERIAL collision_209_materials[1];
static Sint16 collision_209_faces[239];
static NJS_MESHSET collision_209_meshsets[1];
static NJS_POINT3 collision_209_points[110];
static NJS_MODEL collision_209_model;
static NJS_OBJECT collision_209;
static NJS_MATERIAL collision_210_materials[1];
static Sint16 collision_210_faces[14];
static NJS_MESHSET collision_210_meshsets[1];
static NJS_POINT3 collision_210_points[12];
static NJS_MODEL collision_210_model;
static NJS_OBJECT collision_210;
static NJS_MATERIAL collision_220_materials[1];
static Sint16 collision_220_faces[166];
static NJS_MESHSET collision_220_meshsets[1];
static NJS_POINT3 collision_220_points[72];
static NJS_MODEL collision_220_model;
static NJS_OBJECT collision_220;
static NJS_MATERIAL collision_221_materials[1];
static Sint16 collision_221_faces[28];
static NJS_MESHSET collision_221_meshsets[1];
static NJS_POINT3 collision_221_points[12];
static NJS_MODEL collision_221_model;
static NJS_OBJECT collision_221;
static NJS_MATERIAL collision_231_materials[1];
static Sint16 collision_231_faces[27];
static NJS_MESHSET collision_231_meshsets[1];
static NJS_POINT3 collision_231_points[24];
static NJS_MODEL collision_231_model;
static NJS_OBJECT collision_231;
static NJS_MATERIAL collision_233_materials[1];
static Sint16 collision_233_faces[21];
static NJS_MESHSET collision_233_meshsets[1];
static NJS_POINT3 collision_233_points[12];
static NJS_MODEL collision_233_model;
static NJS_OBJECT collision_233;
static NJS_MATERIAL collision_235_materials[1];
static Sint16 collision_235_faces[16];
static NJS_MESHSET collision_235_meshsets[1];
static NJS_POINT3 collision_235_points[8];
static NJS_MODEL collision_235_model;
static NJS_OBJECT collision_235;
static NJS_MATERIAL collision_236_materials[1];
static Sint16 collision_236_faces[28];
static NJS_MESHSET collision_236_meshsets[1];
static NJS_POINT3 collision_236_points[20];
static NJS_MODEL collision_236_model;
static NJS_OBJECT collision_236;
static NJS_CNK_OBJECT draw_264;
static NJS_CNK_OBJECT draw_262;
static NJS_CNK_OBJECT draw_261;
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
static NJS_CNK_OBJECT draw_244;
static NJS_CNK_OBJECT draw_243;
static NJS_CNK_OBJECT draw_242;
static NJS_CNK_OBJECT draw_241;
static NJS_CNK_OBJECT draw_240;
static NJS_CNK_OBJECT draw_238;
static NJS_CNK_OBJECT draw_237;
static NJS_CNK_OBJECT draw_236;
static NJS_CNK_OBJECT draw_235;
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
static NJS_CNK_OBJECT draw_179;
static NJS_CNK_OBJECT draw_178;
static NJS_CNK_OBJECT draw_177;
static NJS_CNK_OBJECT draw_172;
static NJS_CNK_OBJECT draw_168;
static NJS_CNK_OBJECT draw_167;
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
static NJS_CNK_OBJECT draw_148;
static NJS_CNK_OBJECT draw_144;
static NJS_CNK_OBJECT draw_143;
static NJS_CNK_OBJECT draw_142;
static NJS_CNK_OBJECT draw_141;
static NJS_CNK_OBJECT draw_140;
static NJS_CNK_OBJECT draw_139;
static NJS_CNK_OBJECT draw_138;
static NJS_CNK_OBJECT draw_136;
static NJS_CNK_OBJECT draw_135;
static NJS_CNK_OBJECT draw_134;
static NJS_CNK_OBJECT draw_133;
static NJS_CNK_OBJECT draw_132;
static NJS_CNK_OBJECT draw_131;
static NJS_CNK_OBJECT draw_130;
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
static NJS_CNK_OBJECT draw_86;
static NJS_CNK_OBJECT draw_85;
static NJS_CNK_OBJECT draw_84;
static NJS_CNK_OBJECT draw_83;
static NJS_CNK_OBJECT draw_82;
static NJS_CNK_OBJECT draw_81;
static NJS_CNK_OBJECT draw_80;
static NJS_CNK_OBJECT draw_79;
static NJS_CNK_OBJECT draw_78;
static NJS_CNK_OBJECT draw_74;
static NJS_CNK_OBJECT draw_73;
static NJS_CNK_OBJECT draw_72;
static NJS_CNK_OBJECT draw_71;
static NJS_CNK_OBJECT draw_69;
static NJS_CNK_OBJECT draw_68;
static NJS_CNK_OBJECT draw_67;
static NJS_CNK_OBJECT draw_66;
static NJS_CNK_OBJECT draw_65;
static NJS_CNK_OBJECT draw_64;
static NJS_CNK_OBJECT draw_63;
static NJS_CNK_OBJECT draw_62;
static NJS_CNK_OBJECT draw_61;
static NJS_CNK_OBJECT draw_57;
static NJS_CNK_OBJECT draw_56;
static NJS_CNK_OBJECT draw_55;
static NJS_CNK_OBJECT draw_54;
static NJS_CNK_OBJECT draw_53;
static NJS_CNK_OBJECT draw_52;
static NJS_CNK_OBJECT draw_51;
static NJS_CNK_OBJECT draw_49;
static NJS_CNK_OBJECT draw_48;
static NJS_CNK_OBJECT draw_47;
static NJS_CNK_OBJECT draw_46;
static NJS_CNK_OBJECT draw_45;
static NJS_CNK_OBJECT draw_44;
static NJS_CNK_OBJECT draw_43;
static NJS_CNK_OBJECT draw_41;
static NJS_CNK_OBJECT draw_40;
static NJS_CNK_OBJECT draw_39;
static NJS_CNK_OBJECT draw_38;
static NJS_CNK_OBJECT draw_37;
static NJS_CNK_OBJECT draw_35;
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
static NJS_CNK_OBJECT draw_18;
static NJS_CNK_OBJECT draw_17;
static NJS_CNK_OBJECT draw_16;
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
static NJS_OBJECT collision_3;
static NJS_OBJECT collision_7;
static NJS_OBJECT collision_10;
static NJS_OBJECT collision_15;
static NJS_OBJECT collision_16;
static NJS_OBJECT collision_17;
static NJS_OBJECT collision_20;
static NJS_OBJECT collision_21;
static NJS_OBJECT collision_22;
static NJS_OBJECT collision_23;
static NJS_OBJECT collision_24;
static NJS_OBJECT collision_25;
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
static NJS_OBJECT collision_39;
static NJS_OBJECT collision_40;
static NJS_OBJECT collision_42;
static NJS_OBJECT collision_43;
static NJS_OBJECT collision_49;
static NJS_OBJECT collision_50;
static NJS_OBJECT collision_51;
static NJS_OBJECT collision_53;
static NJS_OBJECT collision_54;
static NJS_OBJECT collision_55;
static NJS_OBJECT collision_56;
static NJS_OBJECT collision_57;
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
static NJS_OBJECT collision_90;
static NJS_OBJECT collision_91;
static NJS_OBJECT collision_92;
static NJS_OBJECT collision_93;
static NJS_OBJECT collision_94;
static NJS_OBJECT collision_96;
static NJS_OBJECT collision_97;
static NJS_OBJECT collision_98;
static NJS_OBJECT collision_99;
static NJS_OBJECT collision_101;
static NJS_OBJECT collision_102;
static NJS_OBJECT collision_103;
static NJS_OBJECT collision_104;
static NJS_OBJECT collision_105;
static NJS_OBJECT collision_106;
static NJS_OBJECT collision_107;
static NJS_OBJECT collision_108;
static NJS_OBJECT collision_109;
static NJS_OBJECT collision_112;
static NJS_OBJECT collision_114;
static NJS_OBJECT collision_121;
static NJS_OBJECT collision_122;
static NJS_OBJECT collision_123;
static NJS_OBJECT collision_125;
static NJS_OBJECT collision_126;
static NJS_OBJECT collision_127;
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
static NJS_OBJECT collision_148;
static NJS_OBJECT collision_149;
static NJS_OBJECT collision_153;
static NJS_OBJECT collision_155;
static NJS_OBJECT collision_156;
static NJS_OBJECT collision_157;
static NJS_OBJECT collision_160;
static NJS_OBJECT collision_161;
static NJS_OBJECT collision_162;
static NJS_OBJECT collision_163;
static NJS_OBJECT collision_164;
static NJS_OBJECT collision_165;
static NJS_OBJECT collision_166;
static NJS_OBJECT collision_167;
static NJS_OBJECT collision_168;
static NJS_OBJECT collision_169;
static NJS_OBJECT collision_170;
static NJS_OBJECT collision_171;
static NJS_OBJECT collision_172;
static NJS_OBJECT collision_173;
static NJS_OBJECT collision_174;
static NJS_OBJECT collision_175;
static NJS_OBJECT collision_176;
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
static NJS_OBJECT collision_190;
static NJS_OBJECT collision_191;
static NJS_OBJECT collision_192;
static NJS_OBJECT collision_193;
static NJS_OBJECT collision_194;
static NJS_OBJECT collision_196;
static NJS_OBJECT collision_197;
static NJS_OBJECT collision_198;
static NJS_OBJECT collision_200;
static NJS_OBJECT collision_201;
static NJS_OBJECT collision_202;
static NJS_OBJECT collision_203;
static NJS_OBJECT collision_205;
static NJS_OBJECT collision_206;
static NJS_OBJECT collision_207;
static NJS_OBJECT collision_208;
static NJS_OBJECT collision_211;
static NJS_OBJECT collision_212;
static NJS_OBJECT collision_213;
static NJS_OBJECT collision_214;
static NJS_OBJECT collision_215;
static NJS_OBJECT collision_216;
static NJS_OBJECT collision_217;
static NJS_OBJECT collision_218;
static NJS_OBJECT collision_219;
static NJS_OBJECT collision_222;
static NJS_OBJECT collision_223;
static NJS_OBJECT collision_224;
static NJS_OBJECT collision_225;
static NJS_OBJECT collision_226;
static NJS_OBJECT collision_227;
static NJS_OBJECT collision_228;
static NJS_OBJECT collision_229;
static NJS_OBJECT collision_230;
static NJS_OBJECT collision_232;
static NJS_OBJECT collision_234;
static NJS_OBJECT collision_237;

static struct land_entry entries[506] = {
#include "stg27/terrain/entries.inc"
};
struct land_table lost_colony_land = {
#include "stg27/terrain/lost_colony_land.inc"
};
static NJS_TEXNAME textures[36] = {
#include "stg27/terrain/textures.inc"
};
NJS_TEXLIST lost_colony_land_texlist = {
#include "stg27/terrain/lost_colony_land_texlist.inc"
};
static Sint16 draw_199_polygons[125] = {
#include "stg27/terrain/draw_199_polygons.inc"
};
static Sint32 draw_199_vertices[115] = {
#include "stg27/terrain/draw_199_vertices.inc"
};
static NJS_CNK_MODEL draw_199_model = {
#include "stg27/terrain/draw_199_model.inc"
};
static NJS_CNK_OBJECT draw_267 = {
#include "stg27/terrain/draw_267.inc"
};
static Sint16 draw_97_polygons[311] = {
#include "stg27/terrain/draw_97_polygons.inc"
};
static Sint32 draw_97_vertices[351] = {
#include "stg27/terrain/draw_97_vertices.inc"
};
static NJS_CNK_MODEL draw_97_model = {
#include "stg27/terrain/draw_97_model.inc"
};
static NJS_CNK_OBJECT draw_266 = {
#include "stg27/terrain/draw_266.inc"
};
static Sint16 draw_102_polygons[233] = {
#include "stg27/terrain/draw_102_polygons.inc"
};
static Sint32 draw_102_vertices[131] = {
#include "stg27/terrain/draw_102_vertices.inc"
};
static NJS_CNK_MODEL draw_102_model = {
#include "stg27/terrain/draw_102_model.inc"
};
static NJS_CNK_OBJECT draw_265 = {
#include "stg27/terrain/draw_265.inc"
};
static Sint16 draw_223_polygons[127] = {
#include "stg27/terrain/draw_223_polygons.inc"
};
static Sint32 draw_223_vertices[67] = {
#include "stg27/terrain/draw_223_vertices.inc"
};
static NJS_CNK_MODEL draw_223_model = {
#include "stg27/terrain/draw_223_model.inc"
};
static NJS_CNK_OBJECT draw_263 = {
#include "stg27/terrain/draw_263.inc"
};
static Sint16 draw_95_polygons[81] = {
#include "stg27/terrain/draw_95_polygons.inc"
};
static Sint32 draw_95_vertices[83] = {
#include "stg27/terrain/draw_95_vertices.inc"
};
static NJS_CNK_MODEL draw_95_model = {
#include "stg27/terrain/draw_95_model.inc"
};
static NJS_CNK_OBJECT draw_260 = {
#include "stg27/terrain/draw_260.inc"
};
static Sint16 draw_100_polygons[1269] = {
#include "stg27/terrain/draw_100_polygons.inc"
};
static Sint32 draw_100_vertices[1059] = {
#include "stg27/terrain/draw_100_vertices.inc"
};
static NJS_CNK_MODEL draw_100_model = {
#include "stg27/terrain/draw_100_model.inc"
};
static NJS_CNK_OBJECT draw_259 = {
#include "stg27/terrain/draw_259.inc"
};
static Sint16 draw_10_polygons[453] = {
#include "stg27/terrain/draw_10_polygons.inc"
};
static Sint32 draw_10_vertices[483] = {
#include "stg27/terrain/draw_10_vertices.inc"
};
static NJS_CNK_MODEL draw_10_model = {
#include "stg27/terrain/draw_10_model.inc"
};
static NJS_CNK_OBJECT draw_245 = {
#include "stg27/terrain/draw_245.inc"
};
static Sint16 draw_5_polygons[1225] = {
#include "stg27/terrain/draw_5_polygons.inc"
};
static Sint32 draw_5_vertices[1467] = {
#include "stg27/terrain/draw_5_vertices.inc"
};
static NJS_CNK_MODEL draw_5_model = {
#include "stg27/terrain/draw_5_model.inc"
};
static NJS_CNK_OBJECT draw_239 = {
#include "stg27/terrain/draw_239.inc"
};
static Sint16 draw_1_polygons[197] = {
#include "stg27/terrain/draw_1_polygons.inc"
};
static Sint32 draw_1_vertices[163] = {
#include "stg27/terrain/draw_1_vertices.inc"
};
static NJS_CNK_MODEL draw_1_model = {
#include "stg27/terrain/draw_1_model.inc"
};
static NJS_CNK_OBJECT draw_234 = {
#include "stg27/terrain/draw_234.inc"
};
static Sint16 draw_182_polygons[1805] = {
#include "stg27/terrain/draw_182_polygons.inc"
};
static Sint32 draw_182_vertices[2159] = {
#include "stg27/terrain/draw_182_vertices.inc"
};
static NJS_CNK_MODEL draw_182_model = {
#include "stg27/terrain/draw_182_model.inc"
};
static NJS_CNK_OBJECT draw_182 = {
#include "stg27/terrain/draw_182.inc"
};
static Sint16 draw_181_polygons[197] = {
#include "stg27/terrain/draw_181_polygons.inc"
};
static Sint32 draw_181_vertices[163] = {
#include "stg27/terrain/draw_181_vertices.inc"
};
static NJS_CNK_MODEL draw_181_model = {
#include "stg27/terrain/draw_181_model.inc"
};
static NJS_CNK_OBJECT draw_181 = {
#include "stg27/terrain/draw_181.inc"
};
static Sint16 draw_55_polygons[171] = {
#include "stg27/terrain/draw_55_polygons.inc"
};
static Sint32 draw_55_vertices[99] = {
#include "stg27/terrain/draw_55_vertices.inc"
};
static NJS_CNK_MODEL draw_55_model = {
#include "stg27/terrain/draw_55_model.inc"
};
static NJS_CNK_OBJECT draw_180 = {
#include "stg27/terrain/draw_180.inc"
};
static Sint16 draw_176_polygons[109] = {
#include "stg27/terrain/draw_176_polygons.inc"
};
static Sint32 draw_176_vertices[91] = {
#include "stg27/terrain/draw_176_vertices.inc"
};
static NJS_CNK_MODEL draw_176_model = {
#include "stg27/terrain/draw_176_model.inc"
};
static NJS_CNK_OBJECT draw_176 = {
#include "stg27/terrain/draw_176.inc"
};
static Sint16 draw_175_polygons[225] = {
#include "stg27/terrain/draw_175_polygons.inc"
};
static Sint32 draw_175_vertices[167] = {
#include "stg27/terrain/draw_175_vertices.inc"
};
static NJS_CNK_MODEL draw_175_model = {
#include "stg27/terrain/draw_175_model.inc"
};
static NJS_CNK_OBJECT draw_175 = {
#include "stg27/terrain/draw_175.inc"
};
static Sint16 draw_174_polygons[133] = {
#include "stg27/terrain/draw_174_polygons.inc"
};
static Sint32 draw_174_vertices[83] = {
#include "stg27/terrain/draw_174_vertices.inc"
};
static NJS_CNK_MODEL draw_174_model = {
#include "stg27/terrain/draw_174_model.inc"
};
static NJS_CNK_OBJECT draw_174 = {
#include "stg27/terrain/draw_174.inc"
};
static Sint16 draw_173_polygons[615] = {
#include "stg27/terrain/draw_173_polygons.inc"
};
static Sint32 draw_173_vertices[475] = {
#include "stg27/terrain/draw_173_vertices.inc"
};
static NJS_CNK_MODEL draw_173_model = {
#include "stg27/terrain/draw_173_model.inc"
};
static NJS_CNK_OBJECT draw_173 = {
#include "stg27/terrain/draw_173.inc"
};
static Sint16 draw_153_polygons[207] = {
#include "stg27/terrain/draw_153_polygons.inc"
};
static Sint32 draw_153_vertices[131] = {
#include "stg27/terrain/draw_153_vertices.inc"
};
static NJS_CNK_MODEL draw_153_model = {
#include "stg27/terrain/draw_153_model.inc"
};
static NJS_CNK_OBJECT draw_171 = {
#include "stg27/terrain/draw_171.inc"
};
static Sint16 draw_152_polygons[73] = {
#include "stg27/terrain/draw_152_polygons.inc"
};
static Sint32 draw_152_vertices[83] = {
#include "stg27/terrain/draw_152_vertices.inc"
};
static NJS_CNK_MODEL draw_152_model = {
#include "stg27/terrain/draw_152_model.inc"
};
static NJS_CNK_OBJECT draw_170 = {
#include "stg27/terrain/draw_170.inc"
};
static Sint16 draw_157_polygons[391] = {
#include "stg27/terrain/draw_157_polygons.inc"
};
static Sint32 draw_157_vertices[203] = {
#include "stg27/terrain/draw_157_vertices.inc"
};
static NJS_CNK_MODEL draw_157_model = {
#include "stg27/terrain/draw_157_model.inc"
};
static NJS_CNK_OBJECT draw_169 = {
#include "stg27/terrain/draw_169.inc"
};
static Sint16 draw_166_polygons[371] = {
#include "stg27/terrain/draw_166_polygons.inc"
};
static Sint32 draw_166_vertices[251] = {
#include "stg27/terrain/draw_166_vertices.inc"
};
static NJS_CNK_MODEL draw_166_model = {
#include "stg27/terrain/draw_166_model.inc"
};
static NJS_CNK_OBJECT draw_166 = {
#include "stg27/terrain/draw_166.inc"
};
static Sint16 draw_155_polygons[189] = {
#include "stg27/terrain/draw_155_polygons.inc"
};
static Sint32 draw_155_vertices[115] = {
#include "stg27/terrain/draw_155_vertices.inc"
};
static NJS_CNK_MODEL draw_155_model = {
#include "stg27/terrain/draw_155_model.inc"
};
static NJS_CNK_OBJECT draw_161 = {
#include "stg27/terrain/draw_161.inc"
};
static Sint16 draw_117_polygons[53] = {
#include "stg27/terrain/draw_117_polygons.inc"
};
static Sint32 draw_117_vertices[51] = {
#include "stg27/terrain/draw_117_vertices.inc"
};
static NJS_CNK_MODEL draw_117_model = {
#include "stg27/terrain/draw_117_model.inc"
};
static NJS_CNK_OBJECT draw_151 = {
#include "stg27/terrain/draw_151.inc"
};
static Sint16 draw_122_polygons[135] = {
#include "stg27/terrain/draw_122_polygons.inc"
};
static Sint32 draw_122_vertices[123] = {
#include "stg27/terrain/draw_122_vertices.inc"
};
static NJS_CNK_MODEL draw_122_model = {
#include "stg27/terrain/draw_122_model.inc"
};
static NJS_CNK_OBJECT draw_150 = {
#include "stg27/terrain/draw_150.inc"
};
static Sint16 draw_125_polygons[879] = {
#include "stg27/terrain/draw_125_polygons.inc"
};
static Sint32 draw_125_vertices[495] = {
#include "stg27/terrain/draw_125_vertices.inc"
};
static NJS_CNK_MODEL draw_125_model = {
#include "stg27/terrain/draw_125_model.inc"
};
static NJS_CNK_OBJECT draw_149 = {
#include "stg27/terrain/draw_149.inc"
};
static Sint16 draw_132_polygons[145] = {
#include "stg27/terrain/draw_132_polygons.inc"
};
static Sint32 draw_132_vertices[163] = {
#include "stg27/terrain/draw_132_vertices.inc"
};
static NJS_CNK_MODEL draw_132_model = {
#include "stg27/terrain/draw_132_model.inc"
};
static NJS_CNK_OBJECT draw_147 = {
#include "stg27/terrain/draw_147.inc"
};
static Sint16 draw_126_polygons[315] = {
#include "stg27/terrain/draw_126_polygons.inc"
};
static Sint32 draw_126_vertices[347] = {
#include "stg27/terrain/draw_126_vertices.inc"
};
static NJS_CNK_MODEL draw_126_model = {
#include "stg27/terrain/draw_126_model.inc"
};
static NJS_CNK_OBJECT draw_146 = {
#include "stg27/terrain/draw_146.inc"
};
static Sint16 draw_138_polygons[179] = {
#include "stg27/terrain/draw_138_polygons.inc"
};
static Sint32 draw_138_vertices[187] = {
#include "stg27/terrain/draw_138_vertices.inc"
};
static NJS_CNK_MODEL draw_138_model = {
#include "stg27/terrain/draw_138_model.inc"
};
static NJS_CNK_OBJECT draw_145 = {
#include "stg27/terrain/draw_145.inc"
};
static Sint16 draw_136_polygons[217] = {
#include "stg27/terrain/draw_136_polygons.inc"
};
static Sint32 draw_136_vertices[243] = {
#include "stg27/terrain/draw_136_vertices.inc"
};
static NJS_CNK_MODEL draw_136_model = {
#include "stg27/terrain/draw_136_model.inc"
};
static NJS_CNK_OBJECT draw_137 = {
#include "stg27/terrain/draw_137.inc"
};
static Sint16 draw_129_polygons[37] = {
#include "stg27/terrain/draw_129_polygons.inc"
};
static Sint32 draw_129_vertices[35] = {
#include "stg27/terrain/draw_129_vertices.inc"
};
static NJS_CNK_MODEL draw_129_model = {
#include "stg27/terrain/draw_129_model.inc"
};
static NJS_CNK_OBJECT draw_129 = {
#include "stg27/terrain/draw_129.inc"
};
static Sint16 draw_120_polygons[165] = {
#include "stg27/terrain/draw_120_polygons.inc"
};
static Sint32 draw_120_vertices[159] = {
#include "stg27/terrain/draw_120_vertices.inc"
};
static NJS_CNK_MODEL draw_120_model = {
#include "stg27/terrain/draw_120_model.inc"
};
static NJS_CNK_OBJECT draw_120 = {
#include "stg27/terrain/draw_120.inc"
};
static Sint16 draw_115_polygons[165] = {
#include "stg27/terrain/draw_115_polygons.inc"
};
static Sint32 draw_115_vertices[151] = {
#include "stg27/terrain/draw_115_vertices.inc"
};
static NJS_CNK_MODEL draw_115_model = {
#include "stg27/terrain/draw_115_model.inc"
};
static NJS_CNK_OBJECT draw_115 = {
#include "stg27/terrain/draw_115.inc"
};
static Sint16 draw_114_polygons[707] = {
#include "stg27/terrain/draw_114_polygons.inc"
};
static Sint32 draw_114_vertices[523] = {
#include "stg27/terrain/draw_114_vertices.inc"
};
static NJS_CNK_MODEL draw_114_model = {
#include "stg27/terrain/draw_114_model.inc"
};
static NJS_CNK_OBJECT draw_114 = {
#include "stg27/terrain/draw_114.inc"
};
static Sint16 draw_84_polygons[473] = {
#include "stg27/terrain/draw_84_polygons.inc"
};
static Sint32 draw_84_vertices[351] = {
#include "stg27/terrain/draw_84_vertices.inc"
};
static NJS_CNK_MODEL draw_84_model = {
#include "stg27/terrain/draw_84_model.inc"
};
static NJS_CNK_OBJECT draw_87 = {
#include "stg27/terrain/draw_87.inc"
};
static Sint16 draw_16_polygons[937] = {
#include "stg27/terrain/draw_16_polygons.inc"
};
static Sint32 draw_16_vertices[903] = {
#include "stg27/terrain/draw_16_vertices.inc"
};
static NJS_CNK_MODEL draw_16_model = {
#include "stg27/terrain/draw_16_model.inc"
};
static NJS_CNK_OBJECT draw_77 = {
#include "stg27/terrain/draw_77.inc"
};
static Sint16 draw_14_polygons[507] = {
#include "stg27/terrain/draw_14_polygons.inc"
};
static Sint32 draw_14_vertices[427] = {
#include "stg27/terrain/draw_14_vertices.inc"
};
static NJS_CNK_MODEL draw_14_model = {
#include "stg27/terrain/draw_14_model.inc"
};
static NJS_CNK_OBJECT draw_76 = {
#include "stg27/terrain/draw_76.inc"
};
static Sint16 draw_9_polygons[829] = {
#include "stg27/terrain/draw_9_polygons.inc"
};
static Sint32 draw_9_vertices[715] = {
#include "stg27/terrain/draw_9_vertices.inc"
};
static NJS_CNK_MODEL draw_9_model = {
#include "stg27/terrain/draw_9_model.inc"
};
static NJS_CNK_OBJECT draw_75 = {
#include "stg27/terrain/draw_75.inc"
};
static Sint16 draw_70_polygons[515] = {
#include "stg27/terrain/draw_70_polygons.inc"
};
static Sint32 draw_70_vertices[267] = {
#include "stg27/terrain/draw_70_vertices.inc"
};
static NJS_CNK_MODEL draw_70_model = {
#include "stg27/terrain/draw_70_model.inc"
};
static NJS_CNK_OBJECT draw_70 = {
#include "stg27/terrain/draw_70.inc"
};
static Sint16 draw_12_polygons[233] = {
#include "stg27/terrain/draw_12_polygons.inc"
};
static Sint32 draw_12_vertices[179] = {
#include "stg27/terrain/draw_12_vertices.inc"
};
static NJS_CNK_MODEL draw_12_model = {
#include "stg27/terrain/draw_12_model.inc"
};
static NJS_CNK_OBJECT draw_60 = {
#include "stg27/terrain/draw_60.inc"
};
static Sint16 draw_7_polygons[201] = {
#include "stg27/terrain/draw_7_polygons.inc"
};
static Sint32 draw_7_vertices[179] = {
#include "stg27/terrain/draw_7_vertices.inc"
};
static NJS_CNK_MODEL draw_7_model = {
#include "stg27/terrain/draw_7_model.inc"
};
static NJS_CNK_OBJECT draw_59 = {
#include "stg27/terrain/draw_59.inc"
};
static Sint16 draw_3_polygons[223] = {
#include "stg27/terrain/draw_3_polygons.inc"
};
static Sint32 draw_3_vertices[203] = {
#include "stg27/terrain/draw_3_vertices.inc"
};
static NJS_CNK_MODEL draw_3_model = {
#include "stg27/terrain/draw_3_model.inc"
};
static NJS_CNK_OBJECT draw_58 = {
#include "stg27/terrain/draw_58.inc"
};
static Sint16 draw_50_polygons[393] = {
#include "stg27/terrain/draw_50_polygons.inc"
};
static Sint32 draw_50_vertices[203] = {
#include "stg27/terrain/draw_50_vertices.inc"
};
static NJS_CNK_MODEL draw_50_model = {
#include "stg27/terrain/draw_50_model.inc"
};
static NJS_CNK_OBJECT draw_50 = {
#include "stg27/terrain/draw_50.inc"
};
static Sint16 draw_31_polygons[673] = {
#include "stg27/terrain/draw_31_polygons.inc"
};
static Sint32 draw_31_vertices[483] = {
#include "stg27/terrain/draw_31_vertices.inc"
};
static NJS_CNK_MODEL draw_31_model = {
#include "stg27/terrain/draw_31_model.inc"
};
static NJS_CNK_OBJECT draw_42 = {
#include "stg27/terrain/draw_42.inc"
};
static Sint16 draw_36_polygons[23] = {
#include "stg27/terrain/draw_36_polygons.inc"
};
static Sint32 draw_36_vertices[19] = {
#include "stg27/terrain/draw_36_vertices.inc"
};
static NJS_CNK_MODEL draw_36_model = {
#include "stg27/terrain/draw_36_model.inc"
};
static NJS_CNK_OBJECT draw_36 = {
#include "stg27/terrain/draw_36.inc"
};
static Sint16 draw_34_polygons[943] = {
#include "stg27/terrain/draw_34_polygons.inc"
};
static Sint32 draw_34_vertices[543] = {
#include "stg27/terrain/draw_34_vertices.inc"
};
static NJS_CNK_MODEL draw_34_model = {
#include "stg27/terrain/draw_34_model.inc"
};
static NJS_CNK_OBJECT draw_34 = {
#include "stg27/terrain/draw_34.inc"
};
static Sint16 draw_33_polygons[797] = {
#include "stg27/terrain/draw_33_polygons.inc"
};
static Sint32 draw_33_vertices[523] = {
#include "stg27/terrain/draw_33_vertices.inc"
};
static NJS_CNK_MODEL draw_33_model = {
#include "stg27/terrain/draw_33_model.inc"
};
static NJS_CNK_OBJECT draw_33 = {
#include "stg27/terrain/draw_33.inc"
};
static Sint16 draw_20_polygons[225] = {
#include "stg27/terrain/draw_20_polygons.inc"
};
static Sint32 draw_20_vertices[167] = {
#include "stg27/terrain/draw_20_vertices.inc"
};
static NJS_CNK_MODEL draw_20_model = {
#include "stg27/terrain/draw_20_model.inc"
};
static NJS_CNK_OBJECT draw_20 = {
#include "stg27/terrain/draw_20.inc"
};
static Sint16 draw_19_polygons[133] = {
#include "stg27/terrain/draw_19_polygons.inc"
};
static Sint32 draw_19_vertices[83] = {
#include "stg27/terrain/draw_19_vertices.inc"
};
static NJS_CNK_MODEL draw_19_model = {
#include "stg27/terrain/draw_19_model.inc"
};
static NJS_CNK_OBJECT draw_19 = {
#include "stg27/terrain/draw_19.inc"
};
static Sint16 draw_15_polygons[615] = {
#include "stg27/terrain/draw_15_polygons.inc"
};
static Sint32 draw_15_vertices[475] = {
#include "stg27/terrain/draw_15_vertices.inc"
};
static NJS_CNK_MODEL draw_15_model = {
#include "stg27/terrain/draw_15_model.inc"
};
static NJS_CNK_OBJECT draw_15 = {
#include "stg27/terrain/draw_15.inc"
};
static Sint16 draw_0_polygons[23] = {
#include "stg27/terrain/draw_0_polygons.inc"
};
static Sint32 draw_0_vertices[19] = {
#include "stg27/terrain/draw_0_vertices.inc"
};
static NJS_CNK_MODEL draw_0_model = {
#include "stg27/terrain/draw_0_model.inc"
};
static NJS_CNK_OBJECT draw_0 = {
#include "stg27/terrain/draw_0.inc"
};
static NJS_MATERIAL collision_0_materials[1] = {
#include "stg27/terrain/collision_0_materials.inc"
};
static Sint16 collision_0_faces[204] = {
#include "stg27/terrain/collision_0_faces.inc"
};
static NJS_MESHSET collision_0_meshsets[1] = {
#include "stg27/terrain/collision_0_meshsets.inc"
};
static NJS_POINT3 collision_0_points[112] = {
#include "stg27/terrain/collision_0_points.inc"
};
static NJS_MODEL collision_0_model = {
#include "stg27/terrain/collision_0_model.inc"
};
static NJS_OBJECT collision_0 = {
#include "stg27/terrain/collision_0.inc"
};
static NJS_MATERIAL collision_1_materials[1] = {
#include "stg27/terrain/collision_1_materials.inc"
};
static Sint16 collision_1_faces[139] = {
#include "stg27/terrain/collision_1_faces.inc"
};
static NJS_MESHSET collision_1_meshsets[1] = {
#include "stg27/terrain/collision_1_meshsets.inc"
};
static NJS_POINT3 collision_1_points[80] = {
#include "stg27/terrain/collision_1_points.inc"
};
static NJS_MODEL collision_1_model = {
#include "stg27/terrain/collision_1_model.inc"
};
static NJS_OBJECT collision_1 = {
#include "stg27/terrain/collision_1.inc"
};
static NJS_MATERIAL collision_2_materials[1] = {
#include "stg27/terrain/collision_2_materials.inc"
};
static Sint16 collision_2_faces[78] = {
#include "stg27/terrain/collision_2_faces.inc"
};
static NJS_MESHSET collision_2_meshsets[1] = {
#include "stg27/terrain/collision_2_meshsets.inc"
};
static NJS_POINT3 collision_2_points[48] = {
#include "stg27/terrain/collision_2_points.inc"
};
static NJS_MODEL collision_2_model = {
#include "stg27/terrain/collision_2_model.inc"
};
static NJS_OBJECT collision_2 = {
#include "stg27/terrain/collision_2.inc"
};
static NJS_MATERIAL collision_4_materials[1] = {
#include "stg27/terrain/collision_4_materials.inc"
};
static Sint16 collision_4_faces[17] = {
#include "stg27/terrain/collision_4_faces.inc"
};
static NJS_MESHSET collision_4_meshsets[1] = {
#include "stg27/terrain/collision_4_meshsets.inc"
};
static NJS_POINT3 collision_4_points[14] = {
#include "stg27/terrain/collision_4_points.inc"
};
static NJS_MODEL collision_4_model = {
#include "stg27/terrain/collision_4_model.inc"
};
static NJS_OBJECT collision_4 = {
#include "stg27/terrain/collision_4.inc"
};
static NJS_MATERIAL collision_5_materials[1] = {
#include "stg27/terrain/collision_5_materials.inc"
};
static Sint16 collision_5_faces[17] = {
#include "stg27/terrain/collision_5_faces.inc"
};
static NJS_MESHSET collision_5_meshsets[1] = {
#include "stg27/terrain/collision_5_meshsets.inc"
};
static NJS_POINT3 collision_5_points[14] = {
#include "stg27/terrain/collision_5_points.inc"
};
static NJS_MODEL collision_5_model = {
#include "stg27/terrain/collision_5_model.inc"
};
static NJS_OBJECT collision_5 = {
#include "stg27/terrain/collision_5.inc"
};
static NJS_MATERIAL collision_6_materials[1] = {
#include "stg27/terrain/collision_6_materials.inc"
};
static Sint16 collision_6_faces[17] = {
#include "stg27/terrain/collision_6_faces.inc"
};
static NJS_MESHSET collision_6_meshsets[1] = {
#include "stg27/terrain/collision_6_meshsets.inc"
};
static NJS_POINT3 collision_6_points[14] = {
#include "stg27/terrain/collision_6_points.inc"
};
static NJS_MODEL collision_6_model = {
#include "stg27/terrain/collision_6_model.inc"
};
static NJS_OBJECT collision_6 = {
#include "stg27/terrain/collision_6.inc"
};
static NJS_MATERIAL collision_8_materials[1] = {
#include "stg27/terrain/collision_8_materials.inc"
};
static Sint16 collision_8_faces[36] = {
#include "stg27/terrain/collision_8_faces.inc"
};
static NJS_MESHSET collision_8_meshsets[1] = {
#include "stg27/terrain/collision_8_meshsets.inc"
};
static NJS_POINT3 collision_8_points[22] = {
#include "stg27/terrain/collision_8_points.inc"
};
static NJS_MODEL collision_8_model = {
#include "stg27/terrain/collision_8_model.inc"
};
static NJS_OBJECT collision_8 = {
#include "stg27/terrain/collision_8.inc"
};
static NJS_MATERIAL collision_9_materials[1] = {
#include "stg27/terrain/collision_9_materials.inc"
};
static Sint16 collision_9_faces[56] = {
#include "stg27/terrain/collision_9_faces.inc"
};
static NJS_MESHSET collision_9_meshsets[1] = {
#include "stg27/terrain/collision_9_meshsets.inc"
};
static NJS_POINT3 collision_9_points[24] = {
#include "stg27/terrain/collision_9_points.inc"
};
static NJS_MODEL collision_9_model = {
#include "stg27/terrain/collision_9_model.inc"
};
static NJS_OBJECT collision_9 = {
#include "stg27/terrain/collision_9.inc"
};
static NJS_MATERIAL collision_11_materials[1] = {
#include "stg27/terrain/collision_11_materials.inc"
};
static Sint16 collision_11_faces[38] = {
#include "stg27/terrain/collision_11_faces.inc"
};
static NJS_MESHSET collision_11_meshsets[1] = {
#include "stg27/terrain/collision_11_meshsets.inc"
};
static NJS_POINT3 collision_11_points[24] = {
#include "stg27/terrain/collision_11_points.inc"
};
static NJS_MODEL collision_11_model = {
#include "stg27/terrain/collision_11_model.inc"
};
static NJS_OBJECT collision_11 = {
#include "stg27/terrain/collision_11.inc"
};
static NJS_MATERIAL collision_12_materials[1] = {
#include "stg27/terrain/collision_12_materials.inc"
};
static Sint16 collision_12_faces[10] = {
#include "stg27/terrain/collision_12_faces.inc"
};
static NJS_MESHSET collision_12_meshsets[1] = {
#include "stg27/terrain/collision_12_meshsets.inc"
};
static NJS_POINT3 collision_12_points[8] = {
#include "stg27/terrain/collision_12_points.inc"
};
static NJS_MODEL collision_12_model = {
#include "stg27/terrain/collision_12_model.inc"
};
static NJS_OBJECT collision_12 = {
#include "stg27/terrain/collision_12.inc"
};
static NJS_MATERIAL collision_13_materials[1] = {
#include "stg27/terrain/collision_13_materials.inc"
};
static Sint16 collision_13_faces[69] = {
#include "stg27/terrain/collision_13_faces.inc"
};
static NJS_MESHSET collision_13_meshsets[1] = {
#include "stg27/terrain/collision_13_meshsets.inc"
};
static NJS_POINT3 collision_13_points[40] = {
#include "stg27/terrain/collision_13_points.inc"
};
static NJS_MODEL collision_13_model = {
#include "stg27/terrain/collision_13_model.inc"
};
static NJS_OBJECT collision_13 = {
#include "stg27/terrain/collision_13.inc"
};
static NJS_MATERIAL collision_14_materials[1] = {
#include "stg27/terrain/collision_14_materials.inc"
};
static Sint16 collision_14_faces[14] = {
#include "stg27/terrain/collision_14_faces.inc"
};
static NJS_MESHSET collision_14_meshsets[1] = {
#include "stg27/terrain/collision_14_meshsets.inc"
};
static NJS_POINT3 collision_14_points[12] = {
#include "stg27/terrain/collision_14_points.inc"
};
static NJS_MODEL collision_14_model = {
#include "stg27/terrain/collision_14_model.inc"
};
static NJS_OBJECT collision_14 = {
#include "stg27/terrain/collision_14.inc"
};
static NJS_MATERIAL collision_18_materials[1] = {
#include "stg27/terrain/collision_18_materials.inc"
};
static Sint16 collision_18_faces[36] = {
#include "stg27/terrain/collision_18_faces.inc"
};
static NJS_MESHSET collision_18_meshsets[1] = {
#include "stg27/terrain/collision_18_meshsets.inc"
};
static NJS_POINT3 collision_18_points[22] = {
#include "stg27/terrain/collision_18_points.inc"
};
static NJS_MODEL collision_18_model = {
#include "stg27/terrain/collision_18_model.inc"
};
static NJS_OBJECT collision_18 = {
#include "stg27/terrain/collision_18.inc"
};
static NJS_MATERIAL collision_19_materials[1] = {
#include "stg27/terrain/collision_19_materials.inc"
};
static Sint16 collision_19_faces[14] = {
#include "stg27/terrain/collision_19_faces.inc"
};
static NJS_MESHSET collision_19_meshsets[1] = {
#include "stg27/terrain/collision_19_meshsets.inc"
};
static NJS_POINT3 collision_19_points[12] = {
#include "stg27/terrain/collision_19_points.inc"
};
static NJS_MODEL collision_19_model = {
#include "stg27/terrain/collision_19_model.inc"
};
static NJS_OBJECT collision_19 = {
#include "stg27/terrain/collision_19.inc"
};
static NJS_MATERIAL collision_37_materials[1] = {
#include "stg27/terrain/collision_37_materials.inc"
};
static Sint16 collision_37_faces[69] = {
#include "stg27/terrain/collision_37_faces.inc"
};
static NJS_MESHSET collision_37_meshsets[1] = {
#include "stg27/terrain/collision_37_meshsets.inc"
};
static NJS_POINT3 collision_37_points[40] = {
#include "stg27/terrain/collision_37_points.inc"
};
static NJS_MODEL collision_37_model = {
#include "stg27/terrain/collision_37_model.inc"
};
static NJS_OBJECT collision_37 = {
#include "stg27/terrain/collision_37.inc"
};
static NJS_MATERIAL collision_38_materials[1] = {
#include "stg27/terrain/collision_38_materials.inc"
};
static Sint16 collision_38_faces[21] = {
#include "stg27/terrain/collision_38_faces.inc"
};
static NJS_MESHSET collision_38_meshsets[1] = {
#include "stg27/terrain/collision_38_meshsets.inc"
};
static NJS_POINT3 collision_38_points[12] = {
#include "stg27/terrain/collision_38_points.inc"
};
static NJS_MODEL collision_38_model = {
#include "stg27/terrain/collision_38_model.inc"
};
static NJS_OBJECT collision_38 = {
#include "stg27/terrain/collision_38.inc"
};
static NJS_MATERIAL collision_41_materials[1] = {
#include "stg27/terrain/collision_41_materials.inc"
};
static Sint16 collision_41_faces[39] = {
#include "stg27/terrain/collision_41_faces.inc"
};
static NJS_MESHSET collision_41_meshsets[1] = {
#include "stg27/terrain/collision_41_meshsets.inc"
};
static NJS_POINT3 collision_41_points[20] = {
#include "stg27/terrain/collision_41_points.inc"
};
static NJS_MODEL collision_41_model = {
#include "stg27/terrain/collision_41_model.inc"
};
static NJS_OBJECT collision_41 = {
#include "stg27/terrain/collision_41.inc"
};
static NJS_MATERIAL collision_44_materials[1] = {
#include "stg27/terrain/collision_44_materials.inc"
};
static Sint16 collision_44_faces[9] = {
#include "stg27/terrain/collision_44_faces.inc"
};
static NJS_MESHSET collision_44_meshsets[1] = {
#include "stg27/terrain/collision_44_meshsets.inc"
};
static NJS_POINT3 collision_44_points[8] = {
#include "stg27/terrain/collision_44_points.inc"
};
static NJS_MODEL collision_44_model = {
#include "stg27/terrain/collision_44_model.inc"
};
static NJS_OBJECT collision_44 = {
#include "stg27/terrain/collision_44.inc"
};
static NJS_MATERIAL collision_45_materials[1] = {
#include "stg27/terrain/collision_45_materials.inc"
};
static Sint16 collision_45_faces[14] = {
#include "stg27/terrain/collision_45_faces.inc"
};
static NJS_MESHSET collision_45_meshsets[1] = {
#include "stg27/terrain/collision_45_meshsets.inc"
};
static NJS_POINT3 collision_45_points[12] = {
#include "stg27/terrain/collision_45_points.inc"
};
static NJS_MODEL collision_45_model = {
#include "stg27/terrain/collision_45_model.inc"
};
static NJS_OBJECT collision_45 = {
#include "stg27/terrain/collision_45.inc"
};
static NJS_MATERIAL collision_46_materials[1] = {
#include "stg27/terrain/collision_46_materials.inc"
};
static Sint16 collision_46_faces[36] = {
#include "stg27/terrain/collision_46_faces.inc"
};
static NJS_MESHSET collision_46_meshsets[1] = {
#include "stg27/terrain/collision_46_meshsets.inc"
};
static NJS_POINT3 collision_46_points[22] = {
#include "stg27/terrain/collision_46_points.inc"
};
static NJS_MODEL collision_46_model = {
#include "stg27/terrain/collision_46_model.inc"
};
static NJS_OBJECT collision_46 = {
#include "stg27/terrain/collision_46.inc"
};
static NJS_MATERIAL collision_47_materials[1] = {
#include "stg27/terrain/collision_47_materials.inc"
};
static Sint16 collision_47_faces[69] = {
#include "stg27/terrain/collision_47_faces.inc"
};
static NJS_MESHSET collision_47_meshsets[1] = {
#include "stg27/terrain/collision_47_meshsets.inc"
};
static NJS_POINT3 collision_47_points[40] = {
#include "stg27/terrain/collision_47_points.inc"
};
static NJS_MODEL collision_47_model = {
#include "stg27/terrain/collision_47_model.inc"
};
static NJS_OBJECT collision_47 = {
#include "stg27/terrain/collision_47.inc"
};
static NJS_MATERIAL collision_48_materials[1] = {
#include "stg27/terrain/collision_48_materials.inc"
};
static Sint16 collision_48_faces[45] = {
#include "stg27/terrain/collision_48_faces.inc"
};
static NJS_MESHSET collision_48_meshsets[1] = {
#include "stg27/terrain/collision_48_meshsets.inc"
};
static NJS_POINT3 collision_48_points[28] = {
#include "stg27/terrain/collision_48_points.inc"
};
static NJS_MODEL collision_48_model = {
#include "stg27/terrain/collision_48_model.inc"
};
static NJS_OBJECT collision_48 = {
#include "stg27/terrain/collision_48.inc"
};
static NJS_MATERIAL collision_52_materials[1] = {
#include "stg27/terrain/collision_52_materials.inc"
};
static Sint16 collision_52_faces[38] = {
#include "stg27/terrain/collision_52_faces.inc"
};
static NJS_MESHSET collision_52_meshsets[1] = {
#include "stg27/terrain/collision_52_meshsets.inc"
};
static NJS_POINT3 collision_52_points[24] = {
#include "stg27/terrain/collision_52_points.inc"
};
static NJS_MODEL collision_52_model = {
#include "stg27/terrain/collision_52_model.inc"
};
static NJS_OBJECT collision_52 = {
#include "stg27/terrain/collision_52.inc"
};
static NJS_MATERIAL collision_58_materials[1] = {
#include "stg27/terrain/collision_58_materials.inc"
};
static Sint16 collision_58_faces[10] = {
#include "stg27/terrain/collision_58_faces.inc"
};
static NJS_MESHSET collision_58_meshsets[1] = {
#include "stg27/terrain/collision_58_meshsets.inc"
};
static NJS_POINT3 collision_58_points[8] = {
#include "stg27/terrain/collision_58_points.inc"
};
static NJS_MODEL collision_58_model = {
#include "stg27/terrain/collision_58_model.inc"
};
static NJS_OBJECT collision_58 = {
#include "stg27/terrain/collision_58.inc"
};
static NJS_MATERIAL collision_85_materials[1] = {
#include "stg27/terrain/collision_85_materials.inc"
};
static Sint16 collision_85_faces[36] = {
#include "stg27/terrain/collision_85_faces.inc"
};
static NJS_MESHSET collision_85_meshsets[1] = {
#include "stg27/terrain/collision_85_meshsets.inc"
};
static NJS_POINT3 collision_85_points[22] = {
#include "stg27/terrain/collision_85_points.inc"
};
static NJS_MODEL collision_85_model = {
#include "stg27/terrain/collision_85_model.inc"
};
static NJS_OBJECT collision_85 = {
#include "stg27/terrain/collision_85.inc"
};
static NJS_MATERIAL collision_86_materials[1] = {
#include "stg27/terrain/collision_86_materials.inc"
};
static Sint16 collision_86_faces[46] = {
#include "stg27/terrain/collision_86_faces.inc"
};
static NJS_MESHSET collision_86_meshsets[1] = {
#include "stg27/terrain/collision_86_meshsets.inc"
};
static NJS_POINT3 collision_86_points[30] = {
#include "stg27/terrain/collision_86_points.inc"
};
static NJS_MODEL collision_86_model = {
#include "stg27/terrain/collision_86_model.inc"
};
static NJS_OBJECT collision_86 = {
#include "stg27/terrain/collision_86.inc"
};
static NJS_MATERIAL collision_87_materials[1] = {
#include "stg27/terrain/collision_87_materials.inc"
};
static Sint16 collision_87_faces[19] = {
#include "stg27/terrain/collision_87_faces.inc"
};
static NJS_MESHSET collision_87_meshsets[1] = {
#include "stg27/terrain/collision_87_meshsets.inc"
};
static NJS_POINT3 collision_87_points[14] = {
#include "stg27/terrain/collision_87_points.inc"
};
static NJS_MODEL collision_87_model = {
#include "stg27/terrain/collision_87_model.inc"
};
static NJS_OBJECT collision_87 = {
#include "stg27/terrain/collision_87.inc"
};
static NJS_MATERIAL collision_88_materials[1] = {
#include "stg27/terrain/collision_88_materials.inc"
};
static Sint16 collision_88_faces[69] = {
#include "stg27/terrain/collision_88_faces.inc"
};
static NJS_MESHSET collision_88_meshsets[1] = {
#include "stg27/terrain/collision_88_meshsets.inc"
};
static NJS_POINT3 collision_88_points[40] = {
#include "stg27/terrain/collision_88_points.inc"
};
static NJS_MODEL collision_88_model = {
#include "stg27/terrain/collision_88_model.inc"
};
static NJS_OBJECT collision_88 = {
#include "stg27/terrain/collision_88.inc"
};
static NJS_MATERIAL collision_89_materials[1] = {
#include "stg27/terrain/collision_89_materials.inc"
};
static Sint16 collision_89_faces[11] = {
#include "stg27/terrain/collision_89_faces.inc"
};
static NJS_MESHSET collision_89_meshsets[1] = {
#include "stg27/terrain/collision_89_meshsets.inc"
};
static NJS_POINT3 collision_89_points[10] = {
#include "stg27/terrain/collision_89_points.inc"
};
static NJS_MODEL collision_89_model = {
#include "stg27/terrain/collision_89_model.inc"
};
static NJS_OBJECT collision_89 = {
#include "stg27/terrain/collision_89.inc"
};
static NJS_MATERIAL collision_95_materials[1] = {
#include "stg27/terrain/collision_95_materials.inc"
};
static Sint16 collision_95_faces[18] = {
#include "stg27/terrain/collision_95_faces.inc"
};
static NJS_MESHSET collision_95_meshsets[1] = {
#include "stg27/terrain/collision_95_meshsets.inc"
};
static NJS_POINT3 collision_95_points[16] = {
#include "stg27/terrain/collision_95_points.inc"
};
static NJS_MODEL collision_95_model = {
#include "stg27/terrain/collision_95_model.inc"
};
static NJS_OBJECT collision_95 = {
#include "stg27/terrain/collision_95.inc"
};
static NJS_MATERIAL collision_100_materials[1] = {
#include "stg27/terrain/collision_100_materials.inc"
};
static Sint16 collision_100_faces[18] = {
#include "stg27/terrain/collision_100_faces.inc"
};
static NJS_MESHSET collision_100_meshsets[1] = {
#include "stg27/terrain/collision_100_meshsets.inc"
};
static NJS_POINT3 collision_100_points[16] = {
#include "stg27/terrain/collision_100_points.inc"
};
static NJS_MODEL collision_100_model = {
#include "stg27/terrain/collision_100_model.inc"
};
static NJS_OBJECT collision_100 = {
#include "stg27/terrain/collision_100.inc"
};
static NJS_MATERIAL collision_110_materials[1] = {
#include "stg27/terrain/collision_110_materials.inc"
};
static Sint16 collision_110_faces[92] = {
#include "stg27/terrain/collision_110_faces.inc"
};
static NJS_MESHSET collision_110_meshsets[1] = {
#include "stg27/terrain/collision_110_meshsets.inc"
};
static NJS_POINT3 collision_110_points[41] = {
#include "stg27/terrain/collision_110_points.inc"
};
static NJS_MODEL collision_110_model = {
#include "stg27/terrain/collision_110_model.inc"
};
static NJS_OBJECT collision_110 = {
#include "stg27/terrain/collision_110.inc"
};
static NJS_MATERIAL collision_111_materials[1] = {
#include "stg27/terrain/collision_111_materials.inc"
};
static Sint16 collision_111_faces[118] = {
#include "stg27/terrain/collision_111_faces.inc"
};
static NJS_MESHSET collision_111_meshsets[1] = {
#include "stg27/terrain/collision_111_meshsets.inc"
};
static NJS_POINT3 collision_111_points[53] = {
#include "stg27/terrain/collision_111_points.inc"
};
static NJS_MODEL collision_111_model = {
#include "stg27/terrain/collision_111_model.inc"
};
static NJS_OBJECT collision_111 = {
#include "stg27/terrain/collision_111.inc"
};
static NJS_MATERIAL collision_113_materials[1] = {
#include "stg27/terrain/collision_113_materials.inc"
};
static Sint16 collision_113_faces[90] = {
#include "stg27/terrain/collision_113_faces.inc"
};
static NJS_MESHSET collision_113_meshsets[1] = {
#include "stg27/terrain/collision_113_meshsets.inc"
};
static NJS_POINT3 collision_113_points[49] = {
#include "stg27/terrain/collision_113_points.inc"
};
static NJS_MODEL collision_113_model = {
#include "stg27/terrain/collision_113_model.inc"
};
static NJS_OBJECT collision_113 = {
#include "stg27/terrain/collision_113.inc"
};
static NJS_MATERIAL collision_115_materials[1] = {
#include "stg27/terrain/collision_115_materials.inc"
};
static Sint16 collision_115_faces[9] = {
#include "stg27/terrain/collision_115_faces.inc"
};
static NJS_MESHSET collision_115_meshsets[1] = {
#include "stg27/terrain/collision_115_meshsets.inc"
};
static NJS_POINT3 collision_115_points[8] = {
#include "stg27/terrain/collision_115_points.inc"
};
static NJS_MODEL collision_115_model = {
#include "stg27/terrain/collision_115_model.inc"
};
static NJS_OBJECT collision_115 = {
#include "stg27/terrain/collision_115.inc"
};
static NJS_MATERIAL collision_116_materials[1] = {
#include "stg27/terrain/collision_116_materials.inc"
};
static Sint16 collision_116_faces[50] = {
#include "stg27/terrain/collision_116_faces.inc"
};
static NJS_MESHSET collision_116_meshsets[1] = {
#include "stg27/terrain/collision_116_meshsets.inc"
};
static NJS_POINT3 collision_116_points[22] = {
#include "stg27/terrain/collision_116_points.inc"
};
static NJS_MODEL collision_116_model = {
#include "stg27/terrain/collision_116_model.inc"
};
static NJS_OBJECT collision_116 = {
#include "stg27/terrain/collision_116.inc"
};
static NJS_MATERIAL collision_117_materials[1] = {
#include "stg27/terrain/collision_117_materials.inc"
};
static Sint16 collision_117_faces[20] = {
#include "stg27/terrain/collision_117_faces.inc"
};
static NJS_MESHSET collision_117_meshsets[1] = {
#include "stg27/terrain/collision_117_meshsets.inc"
};
static NJS_POINT3 collision_117_points[10] = {
#include "stg27/terrain/collision_117_points.inc"
};
static NJS_MODEL collision_117_model = {
#include "stg27/terrain/collision_117_model.inc"
};
static NJS_OBJECT collision_117 = {
#include "stg27/terrain/collision_117.inc"
};
static NJS_MATERIAL collision_118_materials[1] = {
#include "stg27/terrain/collision_118_materials.inc"
};
static Sint16 collision_118_faces[61] = {
#include "stg27/terrain/collision_118_faces.inc"
};
static NJS_MESHSET collision_118_meshsets[1] = {
#include "stg27/terrain/collision_118_meshsets.inc"
};
static NJS_POINT3 collision_118_points[29] = {
#include "stg27/terrain/collision_118_points.inc"
};
static NJS_MODEL collision_118_model = {
#include "stg27/terrain/collision_118_model.inc"
};
static NJS_OBJECT collision_118 = {
#include "stg27/terrain/collision_118.inc"
};
static NJS_MATERIAL collision_119_materials[1] = {
#include "stg27/terrain/collision_119_materials.inc"
};
static Sint16 collision_119_faces[47] = {
#include "stg27/terrain/collision_119_faces.inc"
};
static NJS_MESHSET collision_119_meshsets[1] = {
#include "stg27/terrain/collision_119_meshsets.inc"
};
static NJS_POINT3 collision_119_points[30] = {
#include "stg27/terrain/collision_119_points.inc"
};
static NJS_MODEL collision_119_model = {
#include "stg27/terrain/collision_119_model.inc"
};
static NJS_OBJECT collision_119 = {
#include "stg27/terrain/collision_119.inc"
};
static NJS_MATERIAL collision_120_materials[1] = {
#include "stg27/terrain/collision_120_materials.inc"
};
static Sint16 collision_120_faces[14] = {
#include "stg27/terrain/collision_120_faces.inc"
};
static NJS_MESHSET collision_120_meshsets[1] = {
#include "stg27/terrain/collision_120_meshsets.inc"
};
static NJS_POINT3 collision_120_points[12] = {
#include "stg27/terrain/collision_120_points.inc"
};
static NJS_MODEL collision_120_model = {
#include "stg27/terrain/collision_120_model.inc"
};
static NJS_OBJECT collision_120 = {
#include "stg27/terrain/collision_120.inc"
};
static NJS_MATERIAL collision_124_materials[1] = {
#include "stg27/terrain/collision_124_materials.inc"
};
static Sint16 collision_124_faces[52] = {
#include "stg27/terrain/collision_124_faces.inc"
};
static NJS_MESHSET collision_124_meshsets[1] = {
#include "stg27/terrain/collision_124_meshsets.inc"
};
static NJS_POINT3 collision_124_points[24] = {
#include "stg27/terrain/collision_124_points.inc"
};
static NJS_MODEL collision_124_model = {
#include "stg27/terrain/collision_124_model.inc"
};
static NJS_OBJECT collision_124 = {
#include "stg27/terrain/collision_124.inc"
};
static NJS_MATERIAL collision_128_materials[1] = {
#include "stg27/terrain/collision_128_materials.inc"
};
static Sint16 collision_128_faces[20] = {
#include "stg27/terrain/collision_128_faces.inc"
};
static NJS_MESHSET collision_128_meshsets[1] = {
#include "stg27/terrain/collision_128_meshsets.inc"
};
static NJS_POINT3 collision_128_points[12] = {
#include "stg27/terrain/collision_128_points.inc"
};
static NJS_MODEL collision_128_model = {
#include "stg27/terrain/collision_128_model.inc"
};
static NJS_OBJECT collision_128 = {
#include "stg27/terrain/collision_128.inc"
};
static NJS_MATERIAL collision_146_materials[1] = {
#include "stg27/terrain/collision_146_materials.inc"
};
static Sint16 collision_146_faces[54] = {
#include "stg27/terrain/collision_146_faces.inc"
};
static NJS_MESHSET collision_146_meshsets[1] = {
#include "stg27/terrain/collision_146_meshsets.inc"
};
static NJS_POINT3 collision_146_points[27] = {
#include "stg27/terrain/collision_146_points.inc"
};
static NJS_MODEL collision_146_model = {
#include "stg27/terrain/collision_146_model.inc"
};
static NJS_OBJECT collision_146 = {
#include "stg27/terrain/collision_146.inc"
};
static NJS_MATERIAL collision_147_materials[1] = {
#include "stg27/terrain/collision_147_materials.inc"
};
static Sint16 collision_147_faces[14] = {
#include "stg27/terrain/collision_147_faces.inc"
};
static NJS_MESHSET collision_147_meshsets[1] = {
#include "stg27/terrain/collision_147_meshsets.inc"
};
static NJS_POINT3 collision_147_points[12] = {
#include "stg27/terrain/collision_147_points.inc"
};
static NJS_MODEL collision_147_model = {
#include "stg27/terrain/collision_147_model.inc"
};
static NJS_OBJECT collision_147 = {
#include "stg27/terrain/collision_147.inc"
};
static NJS_MATERIAL collision_150_materials[1] = {
#include "stg27/terrain/collision_150_materials.inc"
};
static Sint16 collision_150_faces[42] = {
#include "stg27/terrain/collision_150_faces.inc"
};
static NJS_MESHSET collision_150_meshsets[1] = {
#include "stg27/terrain/collision_150_meshsets.inc"
};
static NJS_POINT3 collision_150_points[21] = {
#include "stg27/terrain/collision_150_points.inc"
};
static NJS_MODEL collision_150_model = {
#include "stg27/terrain/collision_150_model.inc"
};
static NJS_OBJECT collision_150 = {
#include "stg27/terrain/collision_150.inc"
};
static NJS_MATERIAL collision_151_materials[1] = {
#include "stg27/terrain/collision_151_materials.inc"
};
static Sint16 collision_151_faces[14] = {
#include "stg27/terrain/collision_151_faces.inc"
};
static NJS_MESHSET collision_151_meshsets[1] = {
#include "stg27/terrain/collision_151_meshsets.inc"
};
static NJS_POINT3 collision_151_points[12] = {
#include "stg27/terrain/collision_151_points.inc"
};
static NJS_MODEL collision_151_model = {
#include "stg27/terrain/collision_151_model.inc"
};
static NJS_OBJECT collision_151 = {
#include "stg27/terrain/collision_151.inc"
};
static NJS_MATERIAL collision_152_materials[1] = {
#include "stg27/terrain/collision_152_materials.inc"
};
static Sint16 collision_152_faces[40] = {
#include "stg27/terrain/collision_152_faces.inc"
};
static NJS_MESHSET collision_152_meshsets[1] = {
#include "stg27/terrain/collision_152_meshsets.inc"
};
static NJS_POINT3 collision_152_points[24] = {
#include "stg27/terrain/collision_152_points.inc"
};
static NJS_MODEL collision_152_model = {
#include "stg27/terrain/collision_152_model.inc"
};
static NJS_OBJECT collision_152 = {
#include "stg27/terrain/collision_152.inc"
};
static NJS_MATERIAL collision_154_materials[1] = {
#include "stg27/terrain/collision_154_materials.inc"
};
static Sint16 collision_154_faces[25] = {
#include "stg27/terrain/collision_154_faces.inc"
};
static NJS_MESHSET collision_154_meshsets[1] = {
#include "stg27/terrain/collision_154_meshsets.inc"
};
static NJS_POINT3 collision_154_points[12] = {
#include "stg27/terrain/collision_154_points.inc"
};
static NJS_MODEL collision_154_model = {
#include "stg27/terrain/collision_154_model.inc"
};
static NJS_OBJECT collision_154 = {
#include "stg27/terrain/collision_154.inc"
};
static NJS_MATERIAL collision_158_materials[1] = {
#include "stg27/terrain/collision_158_materials.inc"
};
static Sint16 collision_158_faces[56] = {
#include "stg27/terrain/collision_158_faces.inc"
};
static NJS_MESHSET collision_158_meshsets[1] = {
#include "stg27/terrain/collision_158_meshsets.inc"
};
static NJS_POINT3 collision_158_points[25] = {
#include "stg27/terrain/collision_158_points.inc"
};
static NJS_MODEL collision_158_model = {
#include "stg27/terrain/collision_158_model.inc"
};
static NJS_OBJECT collision_158 = {
#include "stg27/terrain/collision_158.inc"
};
static NJS_MATERIAL collision_159_materials[1] = {
#include "stg27/terrain/collision_159_materials.inc"
};
static Sint16 collision_159_faces[20] = {
#include "stg27/terrain/collision_159_faces.inc"
};
static NJS_MESHSET collision_159_meshsets[1] = {
#include "stg27/terrain/collision_159_meshsets.inc"
};
static NJS_POINT3 collision_159_points[10] = {
#include "stg27/terrain/collision_159_points.inc"
};
static NJS_MODEL collision_159_model = {
#include "stg27/terrain/collision_159_model.inc"
};
static NJS_OBJECT collision_159 = {
#include "stg27/terrain/collision_159.inc"
};
static NJS_MATERIAL collision_188_materials[1] = {
#include "stg27/terrain/collision_188_materials.inc"
};
static Sint16 collision_188_faces[28] = {
#include "stg27/terrain/collision_188_faces.inc"
};
static NJS_MESHSET collision_188_meshsets[1] = {
#include "stg27/terrain/collision_188_meshsets.inc"
};
static NJS_POINT3 collision_188_points[21] = {
#include "stg27/terrain/collision_188_points.inc"
};
static NJS_MODEL collision_188_model = {
#include "stg27/terrain/collision_188_model.inc"
};
static NJS_OBJECT collision_188 = {
#include "stg27/terrain/collision_188.inc"
};
static NJS_MATERIAL collision_189_materials[1] = {
#include "stg27/terrain/collision_189_materials.inc"
};
static Sint16 collision_189_faces[7] = {
#include "stg27/terrain/collision_189_faces.inc"
};
static NJS_MESHSET collision_189_meshsets[1] = {
#include "stg27/terrain/collision_189_meshsets.inc"
};
static NJS_POINT3 collision_189_points[6] = {
#include "stg27/terrain/collision_189_points.inc"
};
static NJS_MODEL collision_189_model = {
#include "stg27/terrain/collision_189_model.inc"
};
static NJS_OBJECT collision_189 = {
#include "stg27/terrain/collision_189.inc"
};
static NJS_MATERIAL collision_195_materials[1] = {
#include "stg27/terrain/collision_195_materials.inc"
};
static Sint16 collision_195_faces[33] = {
#include "stg27/terrain/collision_195_faces.inc"
};
static NJS_MESHSET collision_195_meshsets[1] = {
#include "stg27/terrain/collision_195_meshsets.inc"
};
static NJS_POINT3 collision_195_points[14] = {
#include "stg27/terrain/collision_195_points.inc"
};
static NJS_MODEL collision_195_model = {
#include "stg27/terrain/collision_195_model.inc"
};
static NJS_OBJECT collision_195 = {
#include "stg27/terrain/collision_195.inc"
};
static NJS_MATERIAL collision_199_materials[1] = {
#include "stg27/terrain/collision_199_materials.inc"
};
static Sint16 collision_199_faces[10] = {
#include "stg27/terrain/collision_199_faces.inc"
};
static NJS_MESHSET collision_199_meshsets[1] = {
#include "stg27/terrain/collision_199_meshsets.inc"
};
static NJS_POINT3 collision_199_points[8] = {
#include "stg27/terrain/collision_199_points.inc"
};
static NJS_MODEL collision_199_model = {
#include "stg27/terrain/collision_199_model.inc"
};
static NJS_OBJECT collision_199 = {
#include "stg27/terrain/collision_199.inc"
};
static NJS_MATERIAL collision_204_materials[1] = {
#include "stg27/terrain/collision_204_materials.inc"
};
static Sint16 collision_204_faces[21] = {
#include "stg27/terrain/collision_204_faces.inc"
};
static NJS_MESHSET collision_204_meshsets[1] = {
#include "stg27/terrain/collision_204_meshsets.inc"
};
static NJS_POINT3 collision_204_points[8] = {
#include "stg27/terrain/collision_204_points.inc"
};
static NJS_MODEL collision_204_model = {
#include "stg27/terrain/collision_204_model.inc"
};
static NJS_OBJECT collision_204 = {
#include "stg27/terrain/collision_204.inc"
};
static NJS_MATERIAL collision_209_materials[1] = {
#include "stg27/terrain/collision_209_materials.inc"
};
static Sint16 collision_209_faces[239] = {
#include "stg27/terrain/collision_209_faces.inc"
};
static NJS_MESHSET collision_209_meshsets[1] = {
#include "stg27/terrain/collision_209_meshsets.inc"
};
static NJS_POINT3 collision_209_points[110] = {
#include "stg27/terrain/collision_209_points.inc"
};
static NJS_MODEL collision_209_model = {
#include "stg27/terrain/collision_209_model.inc"
};
static NJS_OBJECT collision_209 = {
#include "stg27/terrain/collision_209.inc"
};
static NJS_MATERIAL collision_210_materials[1] = {
#include "stg27/terrain/collision_210_materials.inc"
};
static Sint16 collision_210_faces[14] = {
#include "stg27/terrain/collision_210_faces.inc"
};
static NJS_MESHSET collision_210_meshsets[1] = {
#include "stg27/terrain/collision_210_meshsets.inc"
};
static NJS_POINT3 collision_210_points[12] = {
#include "stg27/terrain/collision_210_points.inc"
};
static NJS_MODEL collision_210_model = {
#include "stg27/terrain/collision_210_model.inc"
};
static NJS_OBJECT collision_210 = {
#include "stg27/terrain/collision_210.inc"
};
static NJS_MATERIAL collision_220_materials[1] = {
#include "stg27/terrain/collision_220_materials.inc"
};
static Sint16 collision_220_faces[166] = {
#include "stg27/terrain/collision_220_faces.inc"
};
static NJS_MESHSET collision_220_meshsets[1] = {
#include "stg27/terrain/collision_220_meshsets.inc"
};
static NJS_POINT3 collision_220_points[72] = {
#include "stg27/terrain/collision_220_points.inc"
};
static NJS_MODEL collision_220_model = {
#include "stg27/terrain/collision_220_model.inc"
};
static NJS_OBJECT collision_220 = {
#include "stg27/terrain/collision_220.inc"
};
static NJS_MATERIAL collision_221_materials[1] = {
#include "stg27/terrain/collision_221_materials.inc"
};
static Sint16 collision_221_faces[28] = {
#include "stg27/terrain/collision_221_faces.inc"
};
static NJS_MESHSET collision_221_meshsets[1] = {
#include "stg27/terrain/collision_221_meshsets.inc"
};
static NJS_POINT3 collision_221_points[12] = {
#include "stg27/terrain/collision_221_points.inc"
};
static NJS_MODEL collision_221_model = {
#include "stg27/terrain/collision_221_model.inc"
};
static NJS_OBJECT collision_221 = {
#include "stg27/terrain/collision_221.inc"
};
static NJS_MATERIAL collision_231_materials[1] = {
#include "stg27/terrain/collision_231_materials.inc"
};
static Sint16 collision_231_faces[27] = {
#include "stg27/terrain/collision_231_faces.inc"
};
static NJS_MESHSET collision_231_meshsets[1] = {
#include "stg27/terrain/collision_231_meshsets.inc"
};
static NJS_POINT3 collision_231_points[24] = {
#include "stg27/terrain/collision_231_points.inc"
};
static NJS_MODEL collision_231_model = {
#include "stg27/terrain/collision_231_model.inc"
};
static NJS_OBJECT collision_231 = {
#include "stg27/terrain/collision_231.inc"
};
static NJS_MATERIAL collision_233_materials[1] = {
#include "stg27/terrain/collision_233_materials.inc"
};
static Sint16 collision_233_faces[21] = {
#include "stg27/terrain/collision_233_faces.inc"
};
static NJS_MESHSET collision_233_meshsets[1] = {
#include "stg27/terrain/collision_233_meshsets.inc"
};
static NJS_POINT3 collision_233_points[12] = {
#include "stg27/terrain/collision_233_points.inc"
};
static NJS_MODEL collision_233_model = {
#include "stg27/terrain/collision_233_model.inc"
};
static NJS_OBJECT collision_233 = {
#include "stg27/terrain/collision_233.inc"
};
static NJS_MATERIAL collision_235_materials[1] = {
#include "stg27/terrain/collision_235_materials.inc"
};
static Sint16 collision_235_faces[16] = {
#include "stg27/terrain/collision_235_faces.inc"
};
static NJS_MESHSET collision_235_meshsets[1] = {
#include "stg27/terrain/collision_235_meshsets.inc"
};
static NJS_POINT3 collision_235_points[8] = {
#include "stg27/terrain/collision_235_points.inc"
};
static NJS_MODEL collision_235_model = {
#include "stg27/terrain/collision_235_model.inc"
};
static NJS_OBJECT collision_235 = {
#include "stg27/terrain/collision_235.inc"
};
static NJS_MATERIAL collision_236_materials[1] = {
#include "stg27/terrain/collision_236_materials.inc"
};
static Sint16 collision_236_faces[28] = {
#include "stg27/terrain/collision_236_faces.inc"
};
static NJS_MESHSET collision_236_meshsets[1] = {
#include "stg27/terrain/collision_236_meshsets.inc"
};
static NJS_POINT3 collision_236_points[20] = {
#include "stg27/terrain/collision_236_points.inc"
};
static NJS_MODEL collision_236_model = {
#include "stg27/terrain/collision_236_model.inc"
};
static NJS_OBJECT collision_236 = {
#include "stg27/terrain/collision_236.inc"
};
static NJS_CNK_OBJECT draw_264 = {
#include "stg27/terrain/draw_264.inc"
};
static NJS_CNK_OBJECT draw_262 = {
#include "stg27/terrain/draw_262.inc"
};
static NJS_CNK_OBJECT draw_261 = {
#include "stg27/terrain/draw_261.inc"
};
static NJS_CNK_OBJECT draw_258 = {
#include "stg27/terrain/draw_258.inc"
};
static NJS_CNK_OBJECT draw_257 = {
#include "stg27/terrain/draw_257.inc"
};
static NJS_CNK_OBJECT draw_256 = {
#include "stg27/terrain/draw_256.inc"
};
static NJS_CNK_OBJECT draw_255 = {
#include "stg27/terrain/draw_255.inc"
};
static NJS_CNK_OBJECT draw_254 = {
#include "stg27/terrain/draw_254.inc"
};
static NJS_CNK_OBJECT draw_253 = {
#include "stg27/terrain/draw_253.inc"
};
static NJS_CNK_OBJECT draw_252 = {
#include "stg27/terrain/draw_252.inc"
};
static NJS_CNK_OBJECT draw_251 = {
#include "stg27/terrain/draw_251.inc"
};
static NJS_CNK_OBJECT draw_250 = {
#include "stg27/terrain/draw_250.inc"
};
static NJS_CNK_OBJECT draw_249 = {
#include "stg27/terrain/draw_249.inc"
};
static NJS_CNK_OBJECT draw_248 = {
#include "stg27/terrain/draw_248.inc"
};
static NJS_CNK_OBJECT draw_247 = {
#include "stg27/terrain/draw_247.inc"
};
static NJS_CNK_OBJECT draw_246 = {
#include "stg27/terrain/draw_246.inc"
};
static NJS_CNK_OBJECT draw_244 = {
#include "stg27/terrain/draw_244.inc"
};
static NJS_CNK_OBJECT draw_243 = {
#include "stg27/terrain/draw_243.inc"
};
static NJS_CNK_OBJECT draw_242 = {
#include "stg27/terrain/draw_242.inc"
};
static NJS_CNK_OBJECT draw_241 = {
#include "stg27/terrain/draw_241.inc"
};
static NJS_CNK_OBJECT draw_240 = {
#include "stg27/terrain/draw_240.inc"
};
static NJS_CNK_OBJECT draw_238 = {
#include "stg27/terrain/draw_238.inc"
};
static NJS_CNK_OBJECT draw_237 = {
#include "stg27/terrain/draw_237.inc"
};
static NJS_CNK_OBJECT draw_236 = {
#include "stg27/terrain/draw_236.inc"
};
static NJS_CNK_OBJECT draw_235 = {
#include "stg27/terrain/draw_235.inc"
};
static NJS_CNK_OBJECT draw_233 = {
#include "stg27/terrain/draw_233.inc"
};
static NJS_CNK_OBJECT draw_232 = {
#include "stg27/terrain/draw_232.inc"
};
static NJS_CNK_OBJECT draw_231 = {
#include "stg27/terrain/draw_231.inc"
};
static NJS_CNK_OBJECT draw_230 = {
#include "stg27/terrain/draw_230.inc"
};
static NJS_CNK_OBJECT draw_229 = {
#include "stg27/terrain/draw_229.inc"
};
static NJS_CNK_OBJECT draw_228 = {
#include "stg27/terrain/draw_228.inc"
};
static NJS_CNK_OBJECT draw_227 = {
#include "stg27/terrain/draw_227.inc"
};
static NJS_CNK_OBJECT draw_226 = {
#include "stg27/terrain/draw_226.inc"
};
static NJS_CNK_OBJECT draw_225 = {
#include "stg27/terrain/draw_225.inc"
};
static NJS_CNK_OBJECT draw_224 = {
#include "stg27/terrain/draw_224.inc"
};
static NJS_CNK_OBJECT draw_223 = {
#include "stg27/terrain/draw_223.inc"
};
static NJS_CNK_OBJECT draw_222 = {
#include "stg27/terrain/draw_222.inc"
};
static NJS_CNK_OBJECT draw_221 = {
#include "stg27/terrain/draw_221.inc"
};
static NJS_CNK_OBJECT draw_220 = {
#include "stg27/terrain/draw_220.inc"
};
static NJS_CNK_OBJECT draw_219 = {
#include "stg27/terrain/draw_219.inc"
};
static NJS_CNK_OBJECT draw_218 = {
#include "stg27/terrain/draw_218.inc"
};
static NJS_CNK_OBJECT draw_217 = {
#include "stg27/terrain/draw_217.inc"
};
static NJS_CNK_OBJECT draw_216 = {
#include "stg27/terrain/draw_216.inc"
};
static NJS_CNK_OBJECT draw_215 = {
#include "stg27/terrain/draw_215.inc"
};
static NJS_CNK_OBJECT draw_214 = {
#include "stg27/terrain/draw_214.inc"
};
static NJS_CNK_OBJECT draw_213 = {
#include "stg27/terrain/draw_213.inc"
};
static NJS_CNK_OBJECT draw_212 = {
#include "stg27/terrain/draw_212.inc"
};
static NJS_CNK_OBJECT draw_211 = {
#include "stg27/terrain/draw_211.inc"
};
static NJS_CNK_OBJECT draw_210 = {
#include "stg27/terrain/draw_210.inc"
};
static NJS_CNK_OBJECT draw_209 = {
#include "stg27/terrain/draw_209.inc"
};
static NJS_CNK_OBJECT draw_208 = {
#include "stg27/terrain/draw_208.inc"
};
static NJS_CNK_OBJECT draw_207 = {
#include "stg27/terrain/draw_207.inc"
};
static NJS_CNK_OBJECT draw_206 = {
#include "stg27/terrain/draw_206.inc"
};
static NJS_CNK_OBJECT draw_205 = {
#include "stg27/terrain/draw_205.inc"
};
static NJS_CNK_OBJECT draw_204 = {
#include "stg27/terrain/draw_204.inc"
};
static NJS_CNK_OBJECT draw_203 = {
#include "stg27/terrain/draw_203.inc"
};
static NJS_CNK_OBJECT draw_202 = {
#include "stg27/terrain/draw_202.inc"
};
static NJS_CNK_OBJECT draw_201 = {
#include "stg27/terrain/draw_201.inc"
};
static NJS_CNK_OBJECT draw_200 = {
#include "stg27/terrain/draw_200.inc"
};
static NJS_CNK_OBJECT draw_199 = {
#include "stg27/terrain/draw_199.inc"
};
static NJS_CNK_OBJECT draw_198 = {
#include "stg27/terrain/draw_198.inc"
};
static NJS_CNK_OBJECT draw_197 = {
#include "stg27/terrain/draw_197.inc"
};
static NJS_CNK_OBJECT draw_196 = {
#include "stg27/terrain/draw_196.inc"
};
static NJS_CNK_OBJECT draw_195 = {
#include "stg27/terrain/draw_195.inc"
};
static NJS_CNK_OBJECT draw_194 = {
#include "stg27/terrain/draw_194.inc"
};
static NJS_CNK_OBJECT draw_193 = {
#include "stg27/terrain/draw_193.inc"
};
static NJS_CNK_OBJECT draw_192 = {
#include "stg27/terrain/draw_192.inc"
};
static NJS_CNK_OBJECT draw_191 = {
#include "stg27/terrain/draw_191.inc"
};
static NJS_CNK_OBJECT draw_190 = {
#include "stg27/terrain/draw_190.inc"
};
static NJS_CNK_OBJECT draw_189 = {
#include "stg27/terrain/draw_189.inc"
};
static NJS_CNK_OBJECT draw_188 = {
#include "stg27/terrain/draw_188.inc"
};
static NJS_CNK_OBJECT draw_187 = {
#include "stg27/terrain/draw_187.inc"
};
static NJS_CNK_OBJECT draw_186 = {
#include "stg27/terrain/draw_186.inc"
};
static NJS_CNK_OBJECT draw_185 = {
#include "stg27/terrain/draw_185.inc"
};
static NJS_CNK_OBJECT draw_184 = {
#include "stg27/terrain/draw_184.inc"
};
static NJS_CNK_OBJECT draw_183 = {
#include "stg27/terrain/draw_183.inc"
};
static NJS_CNK_OBJECT draw_179 = {
#include "stg27/terrain/draw_179.inc"
};
static NJS_CNK_OBJECT draw_178 = {
#include "stg27/terrain/draw_178.inc"
};
static NJS_CNK_OBJECT draw_177 = {
#include "stg27/terrain/draw_177.inc"
};
static NJS_CNK_OBJECT draw_172 = {
#include "stg27/terrain/draw_172.inc"
};
static NJS_CNK_OBJECT draw_168 = {
#include "stg27/terrain/draw_168.inc"
};
static NJS_CNK_OBJECT draw_167 = {
#include "stg27/terrain/draw_167.inc"
};
static NJS_CNK_OBJECT draw_165 = {
#include "stg27/terrain/draw_165.inc"
};
static NJS_CNK_OBJECT draw_164 = {
#include "stg27/terrain/draw_164.inc"
};
static NJS_CNK_OBJECT draw_163 = {
#include "stg27/terrain/draw_163.inc"
};
static NJS_CNK_OBJECT draw_162 = {
#include "stg27/terrain/draw_162.inc"
};
static NJS_CNK_OBJECT draw_160 = {
#include "stg27/terrain/draw_160.inc"
};
static NJS_CNK_OBJECT draw_159 = {
#include "stg27/terrain/draw_159.inc"
};
static NJS_CNK_OBJECT draw_158 = {
#include "stg27/terrain/draw_158.inc"
};
static NJS_CNK_OBJECT draw_157 = {
#include "stg27/terrain/draw_157.inc"
};
static NJS_CNK_OBJECT draw_156 = {
#include "stg27/terrain/draw_156.inc"
};
static NJS_CNK_OBJECT draw_155 = {
#include "stg27/terrain/draw_155.inc"
};
static NJS_CNK_OBJECT draw_154 = {
#include "stg27/terrain/draw_154.inc"
};
static NJS_CNK_OBJECT draw_153 = {
#include "stg27/terrain/draw_153.inc"
};
static NJS_CNK_OBJECT draw_152 = {
#include "stg27/terrain/draw_152.inc"
};
static NJS_CNK_OBJECT draw_148 = {
#include "stg27/terrain/draw_148.inc"
};
static NJS_CNK_OBJECT draw_144 = {
#include "stg27/terrain/draw_144.inc"
};
static NJS_CNK_OBJECT draw_143 = {
#include "stg27/terrain/draw_143.inc"
};
static NJS_CNK_OBJECT draw_142 = {
#include "stg27/terrain/draw_142.inc"
};
static NJS_CNK_OBJECT draw_141 = {
#include "stg27/terrain/draw_141.inc"
};
static NJS_CNK_OBJECT draw_140 = {
#include "stg27/terrain/draw_140.inc"
};
static NJS_CNK_OBJECT draw_139 = {
#include "stg27/terrain/draw_139.inc"
};
static NJS_CNK_OBJECT draw_138 = {
#include "stg27/terrain/draw_138.inc"
};
static NJS_CNK_OBJECT draw_136 = {
#include "stg27/terrain/draw_136.inc"
};
static NJS_CNK_OBJECT draw_135 = {
#include "stg27/terrain/draw_135.inc"
};
static NJS_CNK_OBJECT draw_134 = {
#include "stg27/terrain/draw_134.inc"
};
static NJS_CNK_OBJECT draw_133 = {
#include "stg27/terrain/draw_133.inc"
};
static NJS_CNK_OBJECT draw_132 = {
#include "stg27/terrain/draw_132.inc"
};
static NJS_CNK_OBJECT draw_131 = {
#include "stg27/terrain/draw_131.inc"
};
static NJS_CNK_OBJECT draw_130 = {
#include "stg27/terrain/draw_130.inc"
};
static NJS_CNK_OBJECT draw_128 = {
#include "stg27/terrain/draw_128.inc"
};
static NJS_CNK_OBJECT draw_127 = {
#include "stg27/terrain/draw_127.inc"
};
static NJS_CNK_OBJECT draw_126 = {
#include "stg27/terrain/draw_126.inc"
};
static NJS_CNK_OBJECT draw_125 = {
#include "stg27/terrain/draw_125.inc"
};
static NJS_CNK_OBJECT draw_124 = {
#include "stg27/terrain/draw_124.inc"
};
static NJS_CNK_OBJECT draw_123 = {
#include "stg27/terrain/draw_123.inc"
};
static NJS_CNK_OBJECT draw_122 = {
#include "stg27/terrain/draw_122.inc"
};
static NJS_CNK_OBJECT draw_121 = {
#include "stg27/terrain/draw_121.inc"
};
static NJS_CNK_OBJECT draw_119 = {
#include "stg27/terrain/draw_119.inc"
};
static NJS_CNK_OBJECT draw_118 = {
#include "stg27/terrain/draw_118.inc"
};
static NJS_CNK_OBJECT draw_117 = {
#include "stg27/terrain/draw_117.inc"
};
static NJS_CNK_OBJECT draw_116 = {
#include "stg27/terrain/draw_116.inc"
};
static NJS_CNK_OBJECT draw_113 = {
#include "stg27/terrain/draw_113.inc"
};
static NJS_CNK_OBJECT draw_112 = {
#include "stg27/terrain/draw_112.inc"
};
static NJS_CNK_OBJECT draw_111 = {
#include "stg27/terrain/draw_111.inc"
};
static NJS_CNK_OBJECT draw_110 = {
#include "stg27/terrain/draw_110.inc"
};
static NJS_CNK_OBJECT draw_109 = {
#include "stg27/terrain/draw_109.inc"
};
static NJS_CNK_OBJECT draw_108 = {
#include "stg27/terrain/draw_108.inc"
};
static NJS_CNK_OBJECT draw_107 = {
#include "stg27/terrain/draw_107.inc"
};
static NJS_CNK_OBJECT draw_106 = {
#include "stg27/terrain/draw_106.inc"
};
static NJS_CNK_OBJECT draw_105 = {
#include "stg27/terrain/draw_105.inc"
};
static NJS_CNK_OBJECT draw_104 = {
#include "stg27/terrain/draw_104.inc"
};
static NJS_CNK_OBJECT draw_103 = {
#include "stg27/terrain/draw_103.inc"
};
static NJS_CNK_OBJECT draw_102 = {
#include "stg27/terrain/draw_102.inc"
};
static NJS_CNK_OBJECT draw_101 = {
#include "stg27/terrain/draw_101.inc"
};
static NJS_CNK_OBJECT draw_100 = {
#include "stg27/terrain/draw_100.inc"
};
static NJS_CNK_OBJECT draw_99 = {
#include "stg27/terrain/draw_99.inc"
};
static NJS_CNK_OBJECT draw_98 = {
#include "stg27/terrain/draw_98.inc"
};
static NJS_CNK_OBJECT draw_97 = {
#include "stg27/terrain/draw_97.inc"
};
static NJS_CNK_OBJECT draw_96 = {
#include "stg27/terrain/draw_96.inc"
};
static NJS_CNK_OBJECT draw_95 = {
#include "stg27/terrain/draw_95.inc"
};
static NJS_CNK_OBJECT draw_94 = {
#include "stg27/terrain/draw_94.inc"
};
static NJS_CNK_OBJECT draw_93 = {
#include "stg27/terrain/draw_93.inc"
};
static NJS_CNK_OBJECT draw_92 = {
#include "stg27/terrain/draw_92.inc"
};
static NJS_CNK_OBJECT draw_91 = {
#include "stg27/terrain/draw_91.inc"
};
static NJS_CNK_OBJECT draw_90 = {
#include "stg27/terrain/draw_90.inc"
};
static NJS_CNK_OBJECT draw_89 = {
#include "stg27/terrain/draw_89.inc"
};
static NJS_CNK_OBJECT draw_88 = {
#include "stg27/terrain/draw_88.inc"
};
static NJS_CNK_OBJECT draw_86 = {
#include "stg27/terrain/draw_86.inc"
};
static NJS_CNK_OBJECT draw_85 = {
#include "stg27/terrain/draw_85.inc"
};
static NJS_CNK_OBJECT draw_84 = {
#include "stg27/terrain/draw_84.inc"
};
static NJS_CNK_OBJECT draw_83 = {
#include "stg27/terrain/draw_83.inc"
};
static NJS_CNK_OBJECT draw_82 = {
#include "stg27/terrain/draw_82.inc"
};
static NJS_CNK_OBJECT draw_81 = {
#include "stg27/terrain/draw_81.inc"
};
static NJS_CNK_OBJECT draw_80 = {
#include "stg27/terrain/draw_80.inc"
};
static NJS_CNK_OBJECT draw_79 = {
#include "stg27/terrain/draw_79.inc"
};
static NJS_CNK_OBJECT draw_78 = {
#include "stg27/terrain/draw_78.inc"
};
static NJS_CNK_OBJECT draw_74 = {
#include "stg27/terrain/draw_74.inc"
};
static NJS_CNK_OBJECT draw_73 = {
#include "stg27/terrain/draw_73.inc"
};
static NJS_CNK_OBJECT draw_72 = {
#include "stg27/terrain/draw_72.inc"
};
static NJS_CNK_OBJECT draw_71 = {
#include "stg27/terrain/draw_71.inc"
};
static NJS_CNK_OBJECT draw_69 = {
#include "stg27/terrain/draw_69.inc"
};
static NJS_CNK_OBJECT draw_68 = {
#include "stg27/terrain/draw_68.inc"
};
static NJS_CNK_OBJECT draw_67 = {
#include "stg27/terrain/draw_67.inc"
};
static NJS_CNK_OBJECT draw_66 = {
#include "stg27/terrain/draw_66.inc"
};
static NJS_CNK_OBJECT draw_65 = {
#include "stg27/terrain/draw_65.inc"
};
static NJS_CNK_OBJECT draw_64 = {
#include "stg27/terrain/draw_64.inc"
};
static NJS_CNK_OBJECT draw_63 = {
#include "stg27/terrain/draw_63.inc"
};
static NJS_CNK_OBJECT draw_62 = {
#include "stg27/terrain/draw_62.inc"
};
static NJS_CNK_OBJECT draw_61 = {
#include "stg27/terrain/draw_61.inc"
};
static NJS_CNK_OBJECT draw_57 = {
#include "stg27/terrain/draw_57.inc"
};
static NJS_CNK_OBJECT draw_56 = {
#include "stg27/terrain/draw_56.inc"
};
static NJS_CNK_OBJECT draw_55 = {
#include "stg27/terrain/draw_55.inc"
};
static NJS_CNK_OBJECT draw_54 = {
#include "stg27/terrain/draw_54.inc"
};
static NJS_CNK_OBJECT draw_53 = {
#include "stg27/terrain/draw_53.inc"
};
static NJS_CNK_OBJECT draw_52 = {
#include "stg27/terrain/draw_52.inc"
};
static NJS_CNK_OBJECT draw_51 = {
#include "stg27/terrain/draw_51.inc"
};
static NJS_CNK_OBJECT draw_49 = {
#include "stg27/terrain/draw_49.inc"
};
static NJS_CNK_OBJECT draw_48 = {
#include "stg27/terrain/draw_48.inc"
};
static NJS_CNK_OBJECT draw_47 = {
#include "stg27/terrain/draw_47.inc"
};
static NJS_CNK_OBJECT draw_46 = {
#include "stg27/terrain/draw_46.inc"
};
static NJS_CNK_OBJECT draw_45 = {
#include "stg27/terrain/draw_45.inc"
};
static NJS_CNK_OBJECT draw_44 = {
#include "stg27/terrain/draw_44.inc"
};
static NJS_CNK_OBJECT draw_43 = {
#include "stg27/terrain/draw_43.inc"
};
static NJS_CNK_OBJECT draw_41 = {
#include "stg27/terrain/draw_41.inc"
};
static NJS_CNK_OBJECT draw_40 = {
#include "stg27/terrain/draw_40.inc"
};
static NJS_CNK_OBJECT draw_39 = {
#include "stg27/terrain/draw_39.inc"
};
static NJS_CNK_OBJECT draw_38 = {
#include "stg27/terrain/draw_38.inc"
};
static NJS_CNK_OBJECT draw_37 = {
#include "stg27/terrain/draw_37.inc"
};
static NJS_CNK_OBJECT draw_35 = {
#include "stg27/terrain/draw_35.inc"
};
static NJS_CNK_OBJECT draw_32 = {
#include "stg27/terrain/draw_32.inc"
};
static NJS_CNK_OBJECT draw_31 = {
#include "stg27/terrain/draw_31.inc"
};
static NJS_CNK_OBJECT draw_30 = {
#include "stg27/terrain/draw_30.inc"
};
static NJS_CNK_OBJECT draw_29 = {
#include "stg27/terrain/draw_29.inc"
};
static NJS_CNK_OBJECT draw_28 = {
#include "stg27/terrain/draw_28.inc"
};
static NJS_CNK_OBJECT draw_27 = {
#include "stg27/terrain/draw_27.inc"
};
static NJS_CNK_OBJECT draw_26 = {
#include "stg27/terrain/draw_26.inc"
};
static NJS_CNK_OBJECT draw_25 = {
#include "stg27/terrain/draw_25.inc"
};
static NJS_CNK_OBJECT draw_24 = {
#include "stg27/terrain/draw_24.inc"
};
static NJS_CNK_OBJECT draw_23 = {
#include "stg27/terrain/draw_23.inc"
};
static NJS_CNK_OBJECT draw_22 = {
#include "stg27/terrain/draw_22.inc"
};
static NJS_CNK_OBJECT draw_21 = {
#include "stg27/terrain/draw_21.inc"
};
static NJS_CNK_OBJECT draw_18 = {
#include "stg27/terrain/draw_18.inc"
};
static NJS_CNK_OBJECT draw_17 = {
#include "stg27/terrain/draw_17.inc"
};
static NJS_CNK_OBJECT draw_16 = {
#include "stg27/terrain/draw_16.inc"
};
static NJS_CNK_OBJECT draw_14 = {
#include "stg27/terrain/draw_14.inc"
};
static NJS_CNK_OBJECT draw_13 = {
#include "stg27/terrain/draw_13.inc"
};
static NJS_CNK_OBJECT draw_12 = {
#include "stg27/terrain/draw_12.inc"
};
static NJS_CNK_OBJECT draw_11 = {
#include "stg27/terrain/draw_11.inc"
};
static NJS_CNK_OBJECT draw_10 = {
#include "stg27/terrain/draw_10.inc"
};
static NJS_CNK_OBJECT draw_9 = {
#include "stg27/terrain/draw_9.inc"
};
static NJS_CNK_OBJECT draw_8 = {
#include "stg27/terrain/draw_8.inc"
};
static NJS_CNK_OBJECT draw_7 = {
#include "stg27/terrain/draw_7.inc"
};
static NJS_CNK_OBJECT draw_6 = {
#include "stg27/terrain/draw_6.inc"
};
static NJS_CNK_OBJECT draw_5 = {
#include "stg27/terrain/draw_5.inc"
};
static NJS_CNK_OBJECT draw_4 = {
#include "stg27/terrain/draw_4.inc"
};
static NJS_CNK_OBJECT draw_3 = {
#include "stg27/terrain/draw_3.inc"
};
static NJS_CNK_OBJECT draw_2 = {
#include "stg27/terrain/draw_2.inc"
};
static NJS_CNK_OBJECT draw_1 = {
#include "stg27/terrain/draw_1.inc"
};
static NJS_OBJECT collision_3 = {
#include "stg27/terrain/collision_3.inc"
};
static NJS_OBJECT collision_7 = {
#include "stg27/terrain/collision_7.inc"
};
static NJS_OBJECT collision_10 = {
#include "stg27/terrain/collision_10.inc"
};
static NJS_OBJECT collision_15 = {
#include "stg27/terrain/collision_15.inc"
};
static NJS_OBJECT collision_16 = {
#include "stg27/terrain/collision_16.inc"
};
static NJS_OBJECT collision_17 = {
#include "stg27/terrain/collision_17.inc"
};
static NJS_OBJECT collision_20 = {
#include "stg27/terrain/collision_20.inc"
};
static NJS_OBJECT collision_21 = {
#include "stg27/terrain/collision_21.inc"
};
static NJS_OBJECT collision_22 = {
#include "stg27/terrain/collision_22.inc"
};
static NJS_OBJECT collision_23 = {
#include "stg27/terrain/collision_23.inc"
};
static NJS_OBJECT collision_24 = {
#include "stg27/terrain/collision_24.inc"
};
static NJS_OBJECT collision_25 = {
#include "stg27/terrain/collision_25.inc"
};
static NJS_OBJECT collision_26 = {
#include "stg27/terrain/collision_26.inc"
};
static NJS_OBJECT collision_27 = {
#include "stg27/terrain/collision_27.inc"
};
static NJS_OBJECT collision_28 = {
#include "stg27/terrain/collision_28.inc"
};
static NJS_OBJECT collision_29 = {
#include "stg27/terrain/collision_29.inc"
};
static NJS_OBJECT collision_30 = {
#include "stg27/terrain/collision_30.inc"
};
static NJS_OBJECT collision_31 = {
#include "stg27/terrain/collision_31.inc"
};
static NJS_OBJECT collision_32 = {
#include "stg27/terrain/collision_32.inc"
};
static NJS_OBJECT collision_33 = {
#include "stg27/terrain/collision_33.inc"
};
static NJS_OBJECT collision_34 = {
#include "stg27/terrain/collision_34.inc"
};
static NJS_OBJECT collision_35 = {
#include "stg27/terrain/collision_35.inc"
};
static NJS_OBJECT collision_36 = {
#include "stg27/terrain/collision_36.inc"
};
static NJS_OBJECT collision_39 = {
#include "stg27/terrain/collision_39.inc"
};
static NJS_OBJECT collision_40 = {
#include "stg27/terrain/collision_40.inc"
};
static NJS_OBJECT collision_42 = {
#include "stg27/terrain/collision_42.inc"
};
static NJS_OBJECT collision_43 = {
#include "stg27/terrain/collision_43.inc"
};
static NJS_OBJECT collision_49 = {
#include "stg27/terrain/collision_49.inc"
};
static NJS_OBJECT collision_50 = {
#include "stg27/terrain/collision_50.inc"
};
static NJS_OBJECT collision_51 = {
#include "stg27/terrain/collision_51.inc"
};
static NJS_OBJECT collision_53 = {
#include "stg27/terrain/collision_53.inc"
};
static NJS_OBJECT collision_54 = {
#include "stg27/terrain/collision_54.inc"
};
static NJS_OBJECT collision_55 = {
#include "stg27/terrain/collision_55.inc"
};
static NJS_OBJECT collision_56 = {
#include "stg27/terrain/collision_56.inc"
};
static NJS_OBJECT collision_57 = {
#include "stg27/terrain/collision_57.inc"
};
static NJS_OBJECT collision_59 = {
#include "stg27/terrain/collision_59.inc"
};
static NJS_OBJECT collision_60 = {
#include "stg27/terrain/collision_60.inc"
};
static NJS_OBJECT collision_61 = {
#include "stg27/terrain/collision_61.inc"
};
static NJS_OBJECT collision_62 = {
#include "stg27/terrain/collision_62.inc"
};
static NJS_OBJECT collision_63 = {
#include "stg27/terrain/collision_63.inc"
};
static NJS_OBJECT collision_64 = {
#include "stg27/terrain/collision_64.inc"
};
static NJS_OBJECT collision_65 = {
#include "stg27/terrain/collision_65.inc"
};
static NJS_OBJECT collision_66 = {
#include "stg27/terrain/collision_66.inc"
};
static NJS_OBJECT collision_67 = {
#include "stg27/terrain/collision_67.inc"
};
static NJS_OBJECT collision_68 = {
#include "stg27/terrain/collision_68.inc"
};
static NJS_OBJECT collision_69 = {
#include "stg27/terrain/collision_69.inc"
};
static NJS_OBJECT collision_70 = {
#include "stg27/terrain/collision_70.inc"
};
static NJS_OBJECT collision_71 = {
#include "stg27/terrain/collision_71.inc"
};
static NJS_OBJECT collision_72 = {
#include "stg27/terrain/collision_72.inc"
};
static NJS_OBJECT collision_73 = {
#include "stg27/terrain/collision_73.inc"
};
static NJS_OBJECT collision_74 = {
#include "stg27/terrain/collision_74.inc"
};
static NJS_OBJECT collision_75 = {
#include "stg27/terrain/collision_75.inc"
};
static NJS_OBJECT collision_76 = {
#include "stg27/terrain/collision_76.inc"
};
static NJS_OBJECT collision_77 = {
#include "stg27/terrain/collision_77.inc"
};
static NJS_OBJECT collision_78 = {
#include "stg27/terrain/collision_78.inc"
};
static NJS_OBJECT collision_79 = {
#include "stg27/terrain/collision_79.inc"
};
static NJS_OBJECT collision_80 = {
#include "stg27/terrain/collision_80.inc"
};
static NJS_OBJECT collision_81 = {
#include "stg27/terrain/collision_81.inc"
};
static NJS_OBJECT collision_82 = {
#include "stg27/terrain/collision_82.inc"
};
static NJS_OBJECT collision_83 = {
#include "stg27/terrain/collision_83.inc"
};
static NJS_OBJECT collision_84 = {
#include "stg27/terrain/collision_84.inc"
};
static NJS_OBJECT collision_90 = {
#include "stg27/terrain/collision_90.inc"
};
static NJS_OBJECT collision_91 = {
#include "stg27/terrain/collision_91.inc"
};
static NJS_OBJECT collision_92 = {
#include "stg27/terrain/collision_92.inc"
};
static NJS_OBJECT collision_93 = {
#include "stg27/terrain/collision_93.inc"
};
static NJS_OBJECT collision_94 = {
#include "stg27/terrain/collision_94.inc"
};
static NJS_OBJECT collision_96 = {
#include "stg27/terrain/collision_96.inc"
};
static NJS_OBJECT collision_97 = {
#include "stg27/terrain/collision_97.inc"
};
static NJS_OBJECT collision_98 = {
#include "stg27/terrain/collision_98.inc"
};
static NJS_OBJECT collision_99 = {
#include "stg27/terrain/collision_99.inc"
};
static NJS_OBJECT collision_101 = {
#include "stg27/terrain/collision_101.inc"
};
static NJS_OBJECT collision_102 = {
#include "stg27/terrain/collision_102.inc"
};
static NJS_OBJECT collision_103 = {
#include "stg27/terrain/collision_103.inc"
};
static NJS_OBJECT collision_104 = {
#include "stg27/terrain/collision_104.inc"
};
static NJS_OBJECT collision_105 = {
#include "stg27/terrain/collision_105.inc"
};
static NJS_OBJECT collision_106 = {
#include "stg27/terrain/collision_106.inc"
};
static NJS_OBJECT collision_107 = {
#include "stg27/terrain/collision_107.inc"
};
static NJS_OBJECT collision_108 = {
#include "stg27/terrain/collision_108.inc"
};
static NJS_OBJECT collision_109 = {
#include "stg27/terrain/collision_109.inc"
};
static NJS_OBJECT collision_112 = {
#include "stg27/terrain/collision_112.inc"
};
static NJS_OBJECT collision_114 = {
#include "stg27/terrain/collision_114.inc"
};
static NJS_OBJECT collision_121 = {
#include "stg27/terrain/collision_121.inc"
};
static NJS_OBJECT collision_122 = {
#include "stg27/terrain/collision_122.inc"
};
static NJS_OBJECT collision_123 = {
#include "stg27/terrain/collision_123.inc"
};
static NJS_OBJECT collision_125 = {
#include "stg27/terrain/collision_125.inc"
};
static NJS_OBJECT collision_126 = {
#include "stg27/terrain/collision_126.inc"
};
static NJS_OBJECT collision_127 = {
#include "stg27/terrain/collision_127.inc"
};
static NJS_OBJECT collision_129 = {
#include "stg27/terrain/collision_129.inc"
};
static NJS_OBJECT collision_130 = {
#include "stg27/terrain/collision_130.inc"
};
static NJS_OBJECT collision_131 = {
#include "stg27/terrain/collision_131.inc"
};
static NJS_OBJECT collision_132 = {
#include "stg27/terrain/collision_132.inc"
};
static NJS_OBJECT collision_133 = {
#include "stg27/terrain/collision_133.inc"
};
static NJS_OBJECT collision_134 = {
#include "stg27/terrain/collision_134.inc"
};
static NJS_OBJECT collision_135 = {
#include "stg27/terrain/collision_135.inc"
};
static NJS_OBJECT collision_136 = {
#include "stg27/terrain/collision_136.inc"
};
static NJS_OBJECT collision_137 = {
#include "stg27/terrain/collision_137.inc"
};
static NJS_OBJECT collision_138 = {
#include "stg27/terrain/collision_138.inc"
};
static NJS_OBJECT collision_139 = {
#include "stg27/terrain/collision_139.inc"
};
static NJS_OBJECT collision_140 = {
#include "stg27/terrain/collision_140.inc"
};
static NJS_OBJECT collision_141 = {
#include "stg27/terrain/collision_141.inc"
};
static NJS_OBJECT collision_142 = {
#include "stg27/terrain/collision_142.inc"
};
static NJS_OBJECT collision_143 = {
#include "stg27/terrain/collision_143.inc"
};
static NJS_OBJECT collision_144 = {
#include "stg27/terrain/collision_144.inc"
};
static NJS_OBJECT collision_145 = {
#include "stg27/terrain/collision_145.inc"
};
static NJS_OBJECT collision_148 = {
#include "stg27/terrain/collision_148.inc"
};
static NJS_OBJECT collision_149 = {
#include "stg27/terrain/collision_149.inc"
};
static NJS_OBJECT collision_153 = {
#include "stg27/terrain/collision_153.inc"
};
static NJS_OBJECT collision_155 = {
#include "stg27/terrain/collision_155.inc"
};
static NJS_OBJECT collision_156 = {
#include "stg27/terrain/collision_156.inc"
};
static NJS_OBJECT collision_157 = {
#include "stg27/terrain/collision_157.inc"
};
static NJS_OBJECT collision_160 = {
#include "stg27/terrain/collision_160.inc"
};
static NJS_OBJECT collision_161 = {
#include "stg27/terrain/collision_161.inc"
};
static NJS_OBJECT collision_162 = {
#include "stg27/terrain/collision_162.inc"
};
static NJS_OBJECT collision_163 = {
#include "stg27/terrain/collision_163.inc"
};
static NJS_OBJECT collision_164 = {
#include "stg27/terrain/collision_164.inc"
};
static NJS_OBJECT collision_165 = {
#include "stg27/terrain/collision_165.inc"
};
static NJS_OBJECT collision_166 = {
#include "stg27/terrain/collision_166.inc"
};
static NJS_OBJECT collision_167 = {
#include "stg27/terrain/collision_167.inc"
};
static NJS_OBJECT collision_168 = {
#include "stg27/terrain/collision_168.inc"
};
static NJS_OBJECT collision_169 = {
#include "stg27/terrain/collision_169.inc"
};
static NJS_OBJECT collision_170 = {
#include "stg27/terrain/collision_170.inc"
};
static NJS_OBJECT collision_171 = {
#include "stg27/terrain/collision_171.inc"
};
static NJS_OBJECT collision_172 = {
#include "stg27/terrain/collision_172.inc"
};
static NJS_OBJECT collision_173 = {
#include "stg27/terrain/collision_173.inc"
};
static NJS_OBJECT collision_174 = {
#include "stg27/terrain/collision_174.inc"
};
static NJS_OBJECT collision_175 = {
#include "stg27/terrain/collision_175.inc"
};
static NJS_OBJECT collision_176 = {
#include "stg27/terrain/collision_176.inc"
};
static NJS_OBJECT collision_177 = {
#include "stg27/terrain/collision_177.inc"
};
static NJS_OBJECT collision_178 = {
#include "stg27/terrain/collision_178.inc"
};
static NJS_OBJECT collision_179 = {
#include "stg27/terrain/collision_179.inc"
};
static NJS_OBJECT collision_180 = {
#include "stg27/terrain/collision_180.inc"
};
static NJS_OBJECT collision_181 = {
#include "stg27/terrain/collision_181.inc"
};
static NJS_OBJECT collision_182 = {
#include "stg27/terrain/collision_182.inc"
};
static NJS_OBJECT collision_183 = {
#include "stg27/terrain/collision_183.inc"
};
static NJS_OBJECT collision_184 = {
#include "stg27/terrain/collision_184.inc"
};
static NJS_OBJECT collision_185 = {
#include "stg27/terrain/collision_185.inc"
};
static NJS_OBJECT collision_186 = {
#include "stg27/terrain/collision_186.inc"
};
static NJS_OBJECT collision_187 = {
#include "stg27/terrain/collision_187.inc"
};
static NJS_OBJECT collision_190 = {
#include "stg27/terrain/collision_190.inc"
};
static NJS_OBJECT collision_191 = {
#include "stg27/terrain/collision_191.inc"
};
static NJS_OBJECT collision_192 = {
#include "stg27/terrain/collision_192.inc"
};
static NJS_OBJECT collision_193 = {
#include "stg27/terrain/collision_193.inc"
};
static NJS_OBJECT collision_194 = {
#include "stg27/terrain/collision_194.inc"
};
static NJS_OBJECT collision_196 = {
#include "stg27/terrain/collision_196.inc"
};
static NJS_OBJECT collision_197 = {
#include "stg27/terrain/collision_197.inc"
};
static NJS_OBJECT collision_198 = {
#include "stg27/terrain/collision_198.inc"
};
static NJS_OBJECT collision_200 = {
#include "stg27/terrain/collision_200.inc"
};
static NJS_OBJECT collision_201 = {
#include "stg27/terrain/collision_201.inc"
};
static NJS_OBJECT collision_202 = {
#include "stg27/terrain/collision_202.inc"
};
static NJS_OBJECT collision_203 = {
#include "stg27/terrain/collision_203.inc"
};
static NJS_OBJECT collision_205 = {
#include "stg27/terrain/collision_205.inc"
};
static NJS_OBJECT collision_206 = {
#include "stg27/terrain/collision_206.inc"
};
static NJS_OBJECT collision_207 = {
#include "stg27/terrain/collision_207.inc"
};
static NJS_OBJECT collision_208 = {
#include "stg27/terrain/collision_208.inc"
};
static NJS_OBJECT collision_211 = {
#include "stg27/terrain/collision_211.inc"
};
static NJS_OBJECT collision_212 = {
#include "stg27/terrain/collision_212.inc"
};
static NJS_OBJECT collision_213 = {
#include "stg27/terrain/collision_213.inc"
};
static NJS_OBJECT collision_214 = {
#include "stg27/terrain/collision_214.inc"
};
static NJS_OBJECT collision_215 = {
#include "stg27/terrain/collision_215.inc"
};
static NJS_OBJECT collision_216 = {
#include "stg27/terrain/collision_216.inc"
};
static NJS_OBJECT collision_217 = {
#include "stg27/terrain/collision_217.inc"
};
static NJS_OBJECT collision_218 = {
#include "stg27/terrain/collision_218.inc"
};
static NJS_OBJECT collision_219 = {
#include "stg27/terrain/collision_219.inc"
};
static NJS_OBJECT collision_222 = {
#include "stg27/terrain/collision_222.inc"
};
static NJS_OBJECT collision_223 = {
#include "stg27/terrain/collision_223.inc"
};
static NJS_OBJECT collision_224 = {
#include "stg27/terrain/collision_224.inc"
};
static NJS_OBJECT collision_225 = {
#include "stg27/terrain/collision_225.inc"
};
static NJS_OBJECT collision_226 = {
#include "stg27/terrain/collision_226.inc"
};
static NJS_OBJECT collision_227 = {
#include "stg27/terrain/collision_227.inc"
};
static NJS_OBJECT collision_228 = {
#include "stg27/terrain/collision_228.inc"
};
static NJS_OBJECT collision_229 = {
#include "stg27/terrain/collision_229.inc"
};
static NJS_OBJECT collision_230 = {
#include "stg27/terrain/collision_230.inc"
};
static NJS_OBJECT collision_232 = {
#include "stg27/terrain/collision_232.inc"
};
static NJS_OBJECT collision_234 = {
#include "stg27/terrain/collision_234.inc"
};
static NJS_OBJECT collision_237 = {
#include "stg27/terrain/collision_237.inc"
};
