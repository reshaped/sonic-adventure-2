/* STG26.PRS's land, texture lists, chunk models, Basic models and texture
 * names, from the disc. its C lies between room_scenery.c and
 * object_archives.c, and its D lies between egg_quarters.c and
 * object_archives.c */
#include <ninja.h>
#include <sg_xpt.h>

#include "collision/world.h"
#include "stg26/egg_quarters.h"

#include "stg26/terrain/text.inc"

/* the data up to the last that a pointer names ahead of its definition, as
 * tentative definitions in the disc's order: GCC places each datum at its
 * first definition */
static struct land_entry entries[360];
struct land_table quarters_land;
static NJS_TEXNAME textures[42];
NJS_TEXLIST egg_quarters_land_texlist;
static Sint16 draw_199_polygons[147];
static Sint32 draw_199_vertices[99];
static NJS_CNK_MODEL draw_199_model;
static NJS_CNK_OBJECT draw_199;
static Sint16 draw_198_polygons[153];
static Sint32 draw_198_vertices[83];
static NJS_CNK_MODEL draw_198_model;
static NJS_CNK_OBJECT draw_198;
static Sint16 draw_197_polygons[109];
static Sint32 draw_197_vertices[91];
static NJS_CNK_MODEL draw_197_model;
static NJS_CNK_OBJECT draw_197;
static Sint16 draw_196_polygons[129];
static Sint32 draw_196_vertices[123];
static NJS_CNK_MODEL draw_196_model;
static NJS_CNK_OBJECT draw_196;
static Sint16 draw_195_polygons[953];
static Sint32 draw_195_vertices[775];
static NJS_CNK_MODEL draw_195_model;
static NJS_CNK_OBJECT draw_195;
static Sint16 draw_194_polygons[285];
static Sint32 draw_194_vertices[203];
static NJS_CNK_MODEL draw_194_model;
static NJS_CNK_OBJECT draw_194;
static Sint16 draw_193_polygons[109];
static Sint32 draw_193_vertices[95];
static NJS_CNK_MODEL draw_193_model;
static NJS_CNK_OBJECT draw_193;
static Sint16 draw_191_polygons[197];
static Sint32 draw_191_vertices[99];
static NJS_CNK_MODEL draw_191_model;
static NJS_CNK_OBJECT draw_192;
static Sint16 draw_190_polygons[161];
static Sint32 draw_190_vertices[147];
static NJS_CNK_MODEL draw_190_model;
static NJS_CNK_OBJECT draw_190;
static Sint16 draw_188_polygons[189];
static Sint32 draw_188_vertices[99];
static NJS_CNK_MODEL draw_188_model;
static NJS_CNK_OBJECT draw_189;
static Sint16 draw_187_polygons[153];
static Sint32 draw_187_vertices[79];
static NJS_CNK_MODEL draw_187_model;
static NJS_CNK_OBJECT draw_187;
static Sint16 draw_186_polygons[319];
static Sint32 draw_186_vertices[239];
static NJS_CNK_MODEL draw_186_model;
static NJS_CNK_OBJECT draw_186;
static Sint16 draw_185_polygons[503];
static Sint32 draw_185_vertices[375];
static NJS_CNK_MODEL draw_185_model;
static NJS_CNK_OBJECT draw_185;
static Sint16 draw_183_polygons[147];
static Sint32 draw_183_vertices[99];
static NJS_CNK_MODEL draw_183_model;
static NJS_CNK_OBJECT draw_184;
static Sint16 draw_181_polygons[147];
static Sint32 draw_181_vertices[99];
static NJS_CNK_MODEL draw_181_model;
static NJS_CNK_OBJECT draw_182;
static Sint16 draw_180_polygons[899];
static Sint32 draw_180_vertices[459];
static NJS_CNK_MODEL draw_180_model;
static NJS_CNK_OBJECT draw_180;
static Sint16 draw_179_polygons[57];
static Sint32 draw_179_vertices[43];
static NJS_CNK_MODEL draw_179_model;
static NJS_CNK_OBJECT draw_179;
static Sint16 draw_178_polygons[719];
static Sint32 draw_178_vertices[403];
static NJS_CNK_MODEL draw_178_model;
static NJS_CNK_OBJECT draw_178;
static Sint16 draw_177_polygons[307];
static Sint32 draw_177_vertices[251];
static NJS_CNK_MODEL draw_177_model;
static NJS_CNK_OBJECT draw_177;
static Sint16 draw_176_polygons[57];
static Sint32 draw_176_vertices[51];
static NJS_CNK_MODEL draw_176_model;
static NJS_CNK_OBJECT draw_176;
static Sint16 draw_175_polygons[65];
static Sint32 draw_175_vertices[67];
static NJS_CNK_MODEL draw_175_model;
static NJS_CNK_OBJECT draw_175;
static Sint16 draw_174_polygons[601];
static Sint32 draw_174_vertices[491];
static NJS_CNK_MODEL draw_174_model;
static NJS_CNK_OBJECT draw_174;
static Sint16 draw_173_polygons[427];
static Sint32 draw_173_vertices[351];
static NJS_CNK_MODEL draw_173_model;
static NJS_CNK_OBJECT draw_173;
static Sint16 draw_172_polygons[255];
static Sint32 draw_172_vertices[91];
static NJS_CNK_MODEL draw_172_model;
static NJS_CNK_OBJECT draw_172;
static Sint16 draw_171_polygons[521];
static Sint32 draw_171_vertices[279];
static NJS_CNK_MODEL draw_171_model;
static NJS_CNK_OBJECT draw_171;
static Sint16 draw_170_polygons[153];
static Sint32 draw_170_vertices[83];
static NJS_CNK_MODEL draw_170_model;
static NJS_CNK_OBJECT draw_170;
static Sint16 draw_169_polygons[109];
static Sint32 draw_169_vertices[91];
static NJS_CNK_MODEL draw_169_model;
static NJS_CNK_OBJECT draw_169;
static Sint16 draw_168_polygons[129];
static Sint32 draw_168_vertices[119];
static NJS_CNK_MODEL draw_168_model;
static NJS_CNK_OBJECT draw_168;
static Sint16 draw_167_polygons[953];
static Sint32 draw_167_vertices[763];
static NJS_CNK_MODEL draw_167_model;
static NJS_CNK_OBJECT draw_167;
static Sint16 draw_166_polygons[285];
static Sint32 draw_166_vertices[199];
static NJS_CNK_MODEL draw_166_model;
static NJS_CNK_OBJECT draw_166;
static Sint16 draw_165_polygons[95];
static Sint32 draw_165_vertices[91];
static NJS_CNK_MODEL draw_165_model;
static NJS_CNK_OBJECT draw_165;
static Sint16 draw_163_polygons[189];
static Sint32 draw_163_vertices[103];
static NJS_CNK_MODEL draw_163_model;
static NJS_CNK_OBJECT draw_164;
static Sint16 draw_162_polygons[161];
static Sint32 draw_162_vertices[131];
static NJS_CNK_MODEL draw_162_model;
static NJS_CNK_OBJECT draw_162;
static Sint16 draw_160_polygons[189];
static Sint32 draw_160_vertices[99];
static NJS_CNK_MODEL draw_160_model;
static NJS_CNK_OBJECT draw_161;
static Sint16 draw_159_polygons[147];
static Sint32 draw_159_vertices[79];
static NJS_CNK_MODEL draw_159_model;
static NJS_CNK_OBJECT draw_159;
static Sint16 draw_158_polygons[503];
static Sint32 draw_158_vertices[375];
static NJS_CNK_MODEL draw_158_model;
static NJS_CNK_OBJECT draw_158;
static Sint16 draw_157_polygons[319];
static Sint32 draw_157_vertices[239];
static NJS_CNK_MODEL draw_157_model;
static NJS_CNK_OBJECT draw_157;
static Sint16 draw_154_polygons[55];
static Sint32 draw_154_vertices[51];
static NJS_CNK_MODEL draw_154_model;
static NJS_CNK_OBJECT draw_155;
static Sint16 draw_152_polygons[203];
static Sint32 draw_152_vertices[151];
static NJS_CNK_MODEL draw_152_model;
static NJS_CNK_OBJECT draw_153;
static Sint16 draw_150_polygons[55];
static Sint32 draw_150_vertices[51];
static NJS_CNK_MODEL draw_150_model;
static NJS_CNK_OBJECT draw_151;
static Sint16 draw_149_polygons[683];
static Sint32 draw_149_vertices[363];
static NJS_CNK_MODEL draw_149_model;
static NJS_CNK_OBJECT draw_149;
static Sint16 draw_148_polygons[429];
static Sint32 draw_148_vertices[399];
static NJS_CNK_MODEL draw_148_model;
static NJS_CNK_OBJECT draw_148;
static Sint16 draw_147_polygons[65];
static Sint32 draw_147_vertices[67];
static NJS_CNK_MODEL draw_147_model;
static NJS_CNK_OBJECT draw_147;
static Sint16 draw_146_polygons[423];
static Sint32 draw_146_vertices[379];
static NJS_CNK_MODEL draw_146_model;
static NJS_CNK_OBJECT draw_146;
static Sint16 draw_145_polygons[609];
static Sint32 draw_145_vertices[491];
static NJS_CNK_MODEL draw_145_model;
static NJS_CNK_OBJECT draw_145;
static Sint16 draw_144_polygons[255];
static Sint32 draw_144_vertices[91];
static NJS_CNK_MODEL draw_144_model;
static NJS_CNK_OBJECT draw_144;
static Sint16 draw_143_polygons[1147];
static Sint32 draw_143_vertices[651];
static NJS_CNK_MODEL draw_143_model;
static NJS_CNK_OBJECT draw_143;
static Sint16 draw_142_polygons[103];
static Sint32 draw_142_vertices[75];
static NJS_CNK_MODEL draw_142_model;
static NJS_CNK_OBJECT draw_142;
static Sint16 draw_141_polygons[529];
static Sint32 draw_141_vertices[295];
static NJS_CNK_MODEL draw_141_model;
static NJS_CNK_OBJECT draw_141;
static Sint16 draw_140_polygons[203];
static Sint32 draw_140_vertices[147];
static NJS_CNK_MODEL draw_140_model;
static NJS_CNK_OBJECT draw_140;
static Sint16 draw_139_polygons[989];
static Sint32 draw_139_vertices[835];
static NJS_CNK_MODEL draw_139_model;
static NJS_CNK_OBJECT draw_139;
static Sint16 draw_138_polygons[355];
static Sint32 draw_138_vertices[259];
static NJS_CNK_MODEL draw_138_model;
static NJS_CNK_OBJECT draw_138;
static Sint16 draw_137_polygons[715];
static Sint32 draw_137_vertices[415];
static NJS_CNK_MODEL draw_137_model;
static NJS_CNK_OBJECT draw_137;
static Sint16 draw_136_polygons[103];
static Sint32 draw_136_vertices[99];
static NJS_CNK_MODEL draw_136_model;
static NJS_CNK_OBJECT draw_136;
static Sint16 draw_135_polygons[527];
static Sint32 draw_135_vertices[315];
static NJS_CNK_MODEL draw_135_model;
static NJS_CNK_OBJECT draw_135;
static Sint16 draw_134_polygons[139];
static Sint32 draw_134_vertices[107];
static NJS_CNK_MODEL draw_134_model;
static NJS_CNK_OBJECT draw_134;
static Sint16 draw_133_polygons[257];
static Sint32 draw_133_vertices[179];
static NJS_CNK_MODEL draw_133_model;
static NJS_CNK_OBJECT draw_133;
static Sint16 draw_132_polygons[41];
static Sint32 draw_132_vertices[27];
static NJS_CNK_MODEL draw_132_model;
static NJS_CNK_OBJECT draw_132;
static Sint16 draw_130_polygons[473];
static Sint32 draw_130_vertices[343];
static NJS_CNK_MODEL draw_130_model;
static NJS_CNK_OBJECT draw_130;
static Sint16 draw_128_polygons[119];
static Sint32 draw_128_vertices[75];
static NJS_CNK_MODEL draw_128_model;
static NJS_CNK_OBJECT draw_128;
static Sint16 draw_126_polygons[81];
static Sint32 draw_126_vertices[75];
static NJS_CNK_MODEL draw_126_model;
static NJS_CNK_OBJECT draw_126;
static Sint16 draw_125_polygons[523];
static Sint32 draw_125_vertices[363];
static NJS_CNK_MODEL draw_125_model;
static NJS_CNK_OBJECT draw_125;
static Sint16 draw_124_polygons[593];
static Sint32 draw_124_vertices[419];
static NJS_CNK_MODEL draw_124_model;
static NJS_CNK_OBJECT draw_124;
static Sint16 draw_116_polygons[269];
static Sint32 draw_116_vertices[147];
static NJS_CNK_MODEL draw_116_model;
static NJS_CNK_OBJECT draw_123;
static Sint16 draw_115_polygons[2341];
static Sint32 draw_115_vertices[1991];
static NJS_CNK_MODEL draw_115_model;
static NJS_CNK_OBJECT draw_115;
static Sint16 draw_114_polygons[171];
static Sint32 draw_114_vertices[147];
static NJS_CNK_MODEL draw_114_model;
static NJS_CNK_OBJECT draw_114;
static Sint16 draw_113_polygons[121];
static Sint32 draw_113_vertices[83];
static NJS_CNK_MODEL draw_113_model;
static NJS_CNK_OBJECT draw_113;
static Sint16 draw_112_polygons[341];
static Sint32 draw_112_vertices[147];
static NJS_CNK_MODEL draw_112_model;
static NJS_CNK_OBJECT draw_112;
static Sint16 draw_111_polygons[353];
static Sint32 draw_111_vertices[179];
static NJS_CNK_MODEL draw_111_model;
static NJS_CNK_OBJECT draw_111;
static Sint16 draw_110_polygons[331];
static Sint32 draw_110_vertices[171];
static NJS_CNK_MODEL draw_110_model;
static NJS_CNK_OBJECT draw_110;
static Sint16 draw_109_polygons[343];
static Sint32 draw_109_vertices[163];
static NJS_CNK_MODEL draw_109_model;
static NJS_CNK_OBJECT draw_109;
static Sint16 draw_107_polygons[845];
static Sint32 draw_107_vertices[699];
static NJS_CNK_MODEL draw_107_model;
static NJS_CNK_OBJECT draw_107;
static Sint16 draw_105_polygons[189];
static Sint32 draw_105_vertices[195];
static NJS_CNK_MODEL draw_105_model;
static NJS_CNK_OBJECT draw_105;
static Sint16 draw_103_polygons[165];
static Sint32 draw_103_vertices[195];
static NJS_CNK_MODEL draw_103_model;
static NJS_CNK_OBJECT draw_103;
static Sint16 draw_76_polygons[937];
static Sint32 draw_76_vertices[623];
static NJS_CNK_MODEL draw_76_model;
static NJS_CNK_OBJECT draw_76;
static Sint16 draw_75_polygons[147];
static Sint32 draw_75_vertices[99];
static NJS_CNK_MODEL draw_75_model;
static NJS_CNK_OBJECT draw_75;
static Sint16 draw_73_polygons[147];
static Sint32 draw_73_vertices[99];
static NJS_CNK_MODEL draw_73_model;
static NJS_CNK_OBJECT draw_73;
static Sint16 draw_72_polygons[57];
static Sint32 draw_72_vertices[43];
static NJS_CNK_MODEL draw_72_model;
static NJS_CNK_OBJECT draw_72;
static Sint16 draw_71_polygons[307];
static Sint32 draw_71_vertices[251];
static NJS_CNK_MODEL draw_71_model;
static NJS_CNK_OBJECT draw_71;
static Sint16 draw_70_polygons[697];
static Sint32 draw_70_vertices[335];
static NJS_CNK_MODEL draw_70_model;
static NJS_CNK_OBJECT draw_70;
static Sint16 draw_68_polygons[147];
static Sint32 draw_68_vertices[99];
static NJS_CNK_MODEL draw_68_model;
static NJS_CNK_OBJECT draw_69;
static Sint16 draw_66_polygons[147];
static Sint32 draw_66_vertices[99];
static NJS_CNK_MODEL draw_66_model;
static NJS_CNK_OBJECT draw_67;
static Sint16 draw_65_polygons[1013];
static Sint32 draw_65_vertices[867];
static NJS_CNK_MODEL draw_65_model;
static NJS_CNK_OBJECT draw_65;
static Sint16 draw_64_polygons[555];
static Sint32 draw_64_vertices[239];
static NJS_CNK_MODEL draw_64_model;
static NJS_CNK_OBJECT draw_64;
static Sint16 draw_60_polygons[57];
static Sint32 draw_60_vertices[43];
static NJS_CNK_MODEL draw_60_model;
static NJS_CNK_OBJECT draw_63;
static Sint16 draw_59_polygons[575];
static Sint32 draw_59_vertices[247];
static NJS_CNK_MODEL draw_59_model;
static NJS_CNK_OBJECT draw_59;
static Sint16 draw_55_polygons[57];
static Sint32 draw_55_vertices[43];
static NJS_CNK_MODEL draw_55_model;
static NJS_CNK_OBJECT draw_58;
static Sint16 draw_39_polygons[61];
static Sint32 draw_39_vertices[43];
static NJS_CNK_MODEL draw_39_model;
static NJS_CNK_OBJECT draw_54;
static Sint16 draw_37_polygons[63];
static Sint32 draw_37_vertices[35];
static NJS_CNK_MODEL draw_37_model;
static NJS_CNK_OBJECT draw_38;
static Sint16 draw_35_polygons[81];
static Sint32 draw_35_vertices[59];
static NJS_CNK_MODEL draw_35_model;
static NJS_CNK_OBJECT draw_36;
static Sint16 draw_33_polygons[1327];
static Sint32 draw_33_vertices[999];
static NJS_CNK_MODEL draw_33_model;
static NJS_CNK_OBJECT draw_34;
static Sint16 draw_25_polygons[51];
static Sint32 draw_25_vertices[35];
static NJS_CNK_MODEL draw_25_model;
static NJS_CNK_OBJECT draw_32;
static Sint16 draw_19_polygons[141];
static Sint32 draw_19_vertices[123];
static NJS_CNK_MODEL draw_19_model;
static NJS_CNK_OBJECT draw_24;
static Sint16 draw_17_polygons[101];
static Sint32 draw_17_vertices[67];
static NJS_CNK_MODEL draw_17_model;
static NJS_CNK_OBJECT draw_18;
static Sint16 draw_1_polygons[63];
static Sint32 draw_1_vertices[59];
static NJS_CNK_MODEL draw_1_model;
static NJS_CNK_OBJECT draw_16;
static Sint16 draw_0_polygons[991];
static Sint32 draw_0_vertices[599];
static NJS_CNK_MODEL draw_0_model;
static NJS_CNK_OBJECT draw_0;
static NJS_MATERIAL collision_1_materials[1];
static Sint16 collision_1_faces[14];
static NJS_MESHSET collision_1_meshsets[1];
static NJS_POINT3 collision_1_points[8];
static NJS_MODEL collision_1_model;
static NJS_OBJECT collision_1;
static NJS_MATERIAL collision_2_materials[1];
static Sint16 collision_2_faces[20];
static NJS_MESHSET collision_2_meshsets[1];
static NJS_POINT3 collision_2_points[8];
static NJS_MODEL collision_2_model;
static NJS_OBJECT collision_2;
static NJS_MATERIAL collision_0_materials[1];
static Sint16 collision_0_faces[34];
static NJS_MESHSET collision_0_meshsets[1];
static NJS_POINT3 collision_0_points[32];
static NJS_MODEL collision_0_model;
static NJS_OBJECT collision_4;
static NJS_MATERIAL collision_5_materials[1];
static Sint16 collision_5_faces[14];
static NJS_MESHSET collision_5_meshsets[1];
static NJS_POINT3 collision_5_points[8];
static NJS_MODEL collision_5_model;
static NJS_OBJECT collision_5;
static NJS_MATERIAL collision_6_materials[1];
static Sint16 collision_6_faces[103];
static NJS_MESHSET collision_6_meshsets[1];
static NJS_POINT3 collision_6_points[44];
static NJS_MODEL collision_6_model;
static NJS_OBJECT collision_6;
static NJS_MATERIAL collision_8_materials[1];
static Sint16 collision_8_faces[85];
static NJS_MESHSET collision_8_meshsets[1];
static NJS_POINT3 collision_8_points[68];
static NJS_MODEL collision_8_model;
static NJS_OBJECT collision_8;
static NJS_MATERIAL collision_9_materials[1];
static Sint16 collision_9_faces[85];
static NJS_MESHSET collision_9_meshsets[1];
static NJS_POINT3 collision_9_points[68];
static NJS_MODEL collision_9_model;
static NJS_OBJECT collision_9;
static NJS_MATERIAL collision_11_materials[1];
static Sint16 collision_11_faces[45];
static NJS_MESHSET collision_11_meshsets[1];
static NJS_POINT3 collision_11_points[36];
static NJS_MODEL collision_11_model;
static NJS_OBJECT collision_11;
static NJS_MATERIAL collision_13_materials[1];
static Sint16 collision_13_faces[117];
static NJS_MESHSET collision_13_meshsets[1];
static NJS_POINT3 collision_13_points[48];
static NJS_MODEL collision_13_model;
static NJS_OBJECT collision_13;
static NJS_MATERIAL collision_14_materials[1];
static Sint16 collision_14_faces[15];
static NJS_MESHSET collision_14_meshsets[1];
static NJS_POINT3 collision_14_points[8];
static NJS_MODEL collision_14_model;
static NJS_OBJECT collision_14;
static NJS_MATERIAL collision_15_materials[1];
static Sint16 collision_15_faces[44];
static NJS_MESHSET collision_15_meshsets[1];
static NJS_POINT3 collision_15_points[19];
static NJS_MODEL collision_15_model;
static NJS_OBJECT collision_15;
static NJS_MATERIAL collision_16_materials[1];
static Sint16 collision_16_faces[25];
static NJS_MESHSET collision_16_meshsets[1];
static NJS_POINT3 collision_16_points[12];
static NJS_MODEL collision_16_model;
static NJS_OBJECT collision_16;
static NJS_MATERIAL collision_17_materials[1];
static Sint16 collision_17_faces[28];
static NJS_MESHSET collision_17_meshsets[1];
static NJS_POINT3 collision_17_points[12];
static NJS_MODEL collision_17_model;
static NJS_OBJECT collision_17;
static NJS_MATERIAL collision_18_materials[1];
static Sint16 collision_18_faces[39];
static NJS_MESHSET collision_18_meshsets[1];
static NJS_POINT3 collision_18_points[14];
static NJS_MODEL collision_18_model;
static NJS_OBJECT collision_18;
static NJS_MATERIAL collision_19_materials[1];
static Sint16 collision_19_faces[62];
static NJS_MESHSET collision_19_meshsets[1];
static NJS_POINT3 collision_19_points[32];
static NJS_MODEL collision_19_model;
static NJS_OBJECT collision_19;
static NJS_MATERIAL collision_20_materials[1];
static Sint16 collision_20_faces[25];
static NJS_MESHSET collision_20_meshsets[1];
static NJS_POINT3 collision_20_points[8];
static NJS_MODEL collision_20_model;
static NJS_OBJECT collision_20;
static NJS_MATERIAL collision_21_materials[1];
static Sint16 collision_21_faces[108];
static NJS_MESHSET collision_21_meshsets[1];
static NJS_POINT3 collision_21_points[44];
static NJS_MODEL collision_21_model;
static NJS_OBJECT collision_21;
static NJS_MATERIAL collision_22_materials[1];
static Sint16 collision_22_faces[15];
static NJS_MESHSET collision_22_meshsets[1];
static NJS_POINT3 collision_22_points[8];
static NJS_MODEL collision_22_model;
static NJS_OBJECT collision_22;
static NJS_MATERIAL collision_23_materials[1];
static Sint16 collision_23_faces[53];
static NJS_MESHSET collision_23_meshsets[1];
static NJS_POINT3 collision_23_points[20];
static NJS_MODEL collision_23_model;
static NJS_OBJECT collision_23;
static NJS_MATERIAL collision_24_materials[1];
static Sint16 collision_24_faces[28];
static NJS_MESHSET collision_24_meshsets[1];
static NJS_POINT3 collision_24_points[14];
static NJS_MODEL collision_24_model;
static NJS_OBJECT collision_24;
static NJS_MATERIAL collision_25_materials[1];
static Sint16 collision_25_faces[28];
static NJS_MESHSET collision_25_meshsets[1];
static NJS_POINT3 collision_25_points[14];
static NJS_MODEL collision_25_model;
static NJS_OBJECT collision_25;
static NJS_MATERIAL collision_26_materials[1];
static Sint16 collision_26_faces[39];
static NJS_MESHSET collision_26_meshsets[1];
static NJS_POINT3 collision_26_points[26];
static NJS_MODEL collision_26_model;
static NJS_OBJECT collision_26;
static NJS_MATERIAL collision_30_materials[1];
static Sint16 collision_30_faces[54];
static NJS_MESHSET collision_30_meshsets[1];
static NJS_POINT3 collision_30_points[24];
static NJS_MODEL collision_30_model;
static NJS_OBJECT collision_30;
static NJS_MATERIAL collision_34_materials[1];
static Sint16 collision_34_faces[16];
static NJS_MESHSET collision_34_meshsets[1];
static NJS_POINT3 collision_34_points[8];
static NJS_MODEL collision_34_model;
static NJS_OBJECT collision_34;
static NJS_MATERIAL collision_36_materials[1];
static Sint16 collision_36_faces[39];
static NJS_MESHSET collision_36_meshsets[1];
static NJS_POINT3 collision_36_points[14];
static NJS_MODEL collision_36_model;
static NJS_OBJECT collision_36;
static NJS_MATERIAL collision_37_materials[1];
static Sint16 collision_37_faces[56];
static NJS_MESHSET collision_37_meshsets[1];
static NJS_POINT3 collision_37_points[32];
static NJS_MODEL collision_37_model;
static NJS_OBJECT collision_37;
static NJS_MATERIAL collision_38_materials[1];
static Sint16 collision_38_faces[25];
static NJS_MESHSET collision_38_meshsets[1];
static NJS_POINT3 collision_38_points[8];
static NJS_MODEL collision_38_model;
static NJS_OBJECT collision_38;
static NJS_MATERIAL collision_39_materials[1];
static Sint16 collision_39_faces[105];
static NJS_MESHSET collision_39_meshsets[1];
static NJS_POINT3 collision_39_points[44];
static NJS_MODEL collision_39_model;
static NJS_OBJECT collision_39;
static NJS_MATERIAL collision_40_materials[1];
static Sint16 collision_40_faces[116];
static NJS_MESHSET collision_40_meshsets[1];
static NJS_POINT3 collision_40_points[42];
static NJS_MODEL collision_40_model;
static NJS_OBJECT collision_40;
static NJS_MATERIAL collision_43_materials[1];
static Sint16 collision_43_faces[40];
static NJS_MESHSET collision_43_meshsets[1];
static NJS_POINT3 collision_43_points[32];
static NJS_MODEL collision_43_model;
static NJS_OBJECT collision_43;
static NJS_MATERIAL collision_44_materials[1];
static Sint16 collision_44_faces[15];
static NJS_MESHSET collision_44_meshsets[1];
static NJS_POINT3 collision_44_points[8];
static NJS_MODEL collision_44_model;
static NJS_OBJECT collision_44;
static NJS_MATERIAL collision_45_materials[1];
static Sint16 collision_45_faces[51];
static NJS_MESHSET collision_45_meshsets[1];
static NJS_POINT3 collision_45_points[20];
static NJS_MODEL collision_45_model;
static NJS_OBJECT collision_45;
static NJS_MATERIAL collision_46_materials[1];
static Sint16 collision_46_faces[28];
static NJS_MESHSET collision_46_meshsets[1];
static NJS_POINT3 collision_46_points[12];
static NJS_MODEL collision_46_model;
static NJS_OBJECT collision_46;
static NJS_MATERIAL collision_47_materials[1];
static Sint16 collision_47_faces[25];
static NJS_MESHSET collision_47_meshsets[1];
static NJS_POINT3 collision_47_points[12];
static NJS_MODEL collision_47_model;
static NJS_OBJECT collision_47;
static NJS_MATERIAL collision_48_materials[1];
static Sint16 collision_48_faces[30];
static NJS_MESHSET collision_48_meshsets[1];
static NJS_POINT3 collision_48_points[12];
static NJS_MODEL collision_48_model;
static NJS_OBJECT collision_48;
static NJS_MATERIAL collision_49_materials[1];
static Sint16 collision_49_faces[10];
static NJS_MESHSET collision_49_meshsets[1];
static NJS_POINT3 collision_49_points[8];
static NJS_MODEL collision_49_model;
static NJS_OBJECT collision_49;
static NJS_MATERIAL collision_51_materials[1];
static Sint16 collision_51_faces[125];
static NJS_MESHSET collision_51_meshsets[1];
static NJS_POINT3 collision_51_points[46];
static NJS_MODEL collision_51_model;
static NJS_OBJECT collision_51;
static NJS_MATERIAL collision_52_materials[1];
static Sint16 collision_52_faces[5];
static NJS_MESHSET collision_52_meshsets[1];
static NJS_POINT3 collision_52_points[4];
static NJS_MODEL collision_52_model;
static NJS_OBJECT collision_52;
static NJS_MATERIAL collision_53_materials[1];
static Sint16 collision_53_faces[146];
static NJS_MESHSET collision_53_meshsets[1];
static NJS_POINT3 collision_53_points[67];
static NJS_MODEL collision_53_model;
static NJS_OBJECT collision_53;
static NJS_MATERIAL collision_54_materials[1];
static Sint16 collision_54_faces[46];
static NJS_MESHSET collision_54_meshsets[1];
static NJS_POINT3 collision_54_points[22];
static NJS_MODEL collision_54_model;
static NJS_OBJECT collision_54;
static NJS_MATERIAL collision_55_materials[1];
static Sint16 collision_55_faces[49];
static NJS_MESHSET collision_55_meshsets[1];
static NJS_POINT3 collision_55_points[22];
static NJS_MODEL collision_55_model;
static NJS_OBJECT collision_55;
static NJS_MATERIAL collision_56_materials[1];
static Sint16 collision_56_faces[76];
static NJS_MESHSET collision_56_meshsets[1];
static NJS_POINT3 collision_56_points[44];
static NJS_MODEL collision_56_model;
static NJS_OBJECT collision_56;
static NJS_MATERIAL collision_58_materials[1];
static Sint16 collision_58_faces[40];
static NJS_MESHSET collision_58_meshsets[1];
static NJS_POINT3 collision_58_points[18];
static NJS_MODEL collision_58_model;
static NJS_OBJECT collision_58;
static NJS_MATERIAL collision_60_materials[1];
static Sint16 collision_60_faces[12];
static NJS_MESHSET collision_60_meshsets[1];
static NJS_POINT3 collision_60_points[7];
static NJS_MODEL collision_60_model;
static NJS_OBJECT collision_60;
static NJS_MATERIAL collision_68_materials[1];
static Sint16 collision_68_faces[360];
static NJS_MESHSET collision_68_meshsets[1];
static NJS_POINT3 collision_68_points[131];
static NJS_MODEL collision_68_model;
static NJS_OBJECT collision_68;
static NJS_MATERIAL collision_70_materials[1];
static Sint16 collision_70_faces[374];
static NJS_MESHSET collision_70_meshsets[1];
static NJS_POINT3 collision_70_points[148];
static NJS_MODEL collision_70_model;
static NJS_OBJECT collision_70;
static NJS_MATERIAL collision_73_materials[1];
static Sint16 collision_73_faces[5];
static NJS_MESHSET collision_73_meshsets[1];
static NJS_POINT3 collision_73_points[4];
static NJS_MODEL collision_73_model;
static NJS_OBJECT collision_73;
static NJS_MATERIAL collision_74_materials[1];
static Sint16 collision_74_faces[5];
static NJS_MESHSET collision_74_meshsets[1];
static NJS_POINT3 collision_74_points[4];
static NJS_MODEL collision_74_model;
static NJS_OBJECT collision_74;
static NJS_MATERIAL collision_75_materials[1];
static Sint16 collision_75_faces[5];
static NJS_MESHSET collision_75_meshsets[1];
static NJS_POINT3 collision_75_points[4];
static NJS_MODEL collision_75_model;
static NJS_OBJECT collision_75;
static NJS_MATERIAL collision_76_materials[1];
static Sint16 collision_76_faces[44];
static NJS_MESHSET collision_76_meshsets[1];
static NJS_POINT3 collision_76_points[20];
static NJS_MODEL collision_76_model;
static NJS_OBJECT collision_76;
static NJS_MATERIAL collision_78_materials[1];
static Sint16 collision_78_faces[15];
static NJS_MESHSET collision_78_meshsets[1];
static NJS_POINT3 collision_78_points[12];
static NJS_MODEL collision_78_model;
static NJS_OBJECT collision_78;
static NJS_MATERIAL collision_79_materials[1];
static Sint16 collision_79_faces[55];
static NJS_MESHSET collision_79_meshsets[1];
static NJS_POINT3 collision_79_points[24];
static NJS_MODEL collision_79_model;
static NJS_OBJECT collision_79;
static NJS_MATERIAL collision_80_materials[1];
static Sint16 collision_80_faces[39];
static NJS_MESHSET collision_80_meshsets[1];
static NJS_POINT3 collision_80_points[15];
static NJS_MODEL collision_80_model;
static NJS_OBJECT collision_80;
static NJS_MATERIAL collision_81_materials[1];
static Sint16 collision_81_faces[50];
static NJS_MESHSET collision_81_meshsets[1];
static NJS_POINT3 collision_81_points[22];
static NJS_MODEL collision_81_model;
static NJS_OBJECT collision_81;
static NJS_MATERIAL collision_82_materials[1];
static Sint16 collision_82_faces[32];
static NJS_MESHSET collision_82_meshsets[1];
static NJS_POINT3 collision_82_points[12];
static NJS_MODEL collision_82_model;
static NJS_OBJECT collision_82;
static NJS_MATERIAL collision_83_materials[1];
static Sint16 collision_83_faces[32];
static NJS_MESHSET collision_83_meshsets[1];
static NJS_POINT3 collision_83_points[12];
static NJS_MODEL collision_83_model;
static NJS_OBJECT collision_83;
static NJS_MATERIAL collision_84_materials[1];
static Sint16 collision_84_faces[38];
static NJS_MESHSET collision_84_meshsets[1];
static NJS_POINT3 collision_84_points[15];
static NJS_MODEL collision_84_model;
static NJS_OBJECT collision_84;
static NJS_MATERIAL collision_85_materials[1];
static Sint16 collision_85_faces[93];
static NJS_MESHSET collision_85_meshsets[1];
static NJS_POINT3 collision_85_points[56];
static NJS_MODEL collision_85_model;
static NJS_OBJECT collision_85;
static NJS_MATERIAL collision_86_materials[1];
static Sint16 collision_86_faces[27];
static NJS_MESHSET collision_86_meshsets[1];
static NJS_POINT3 collision_86_points[12];
static NJS_MODEL collision_86_model;
static NJS_OBJECT collision_86;
static NJS_MATERIAL collision_98_materials[1];
static Sint16 collision_98_faces[30];
static NJS_MESHSET collision_98_meshsets[1];
static NJS_POINT3 collision_98_points[24];
static NJS_MODEL collision_98_model;
static NJS_OBJECT collision_98;
static NJS_MATERIAL collision_102_materials[1];
static Sint16 collision_102_faces[22];
static NJS_MESHSET collision_102_meshsets[1];
static NJS_POINT3 collision_102_points[12];
static NJS_MODEL collision_102_model;
static NJS_OBJECT collision_102;
static NJS_MATERIAL collision_103_materials[1];
static Sint16 collision_103_faces[32];
static NJS_MESHSET collision_103_meshsets[1];
static NJS_POINT3 collision_103_points[12];
static NJS_MODEL collision_103_model;
static NJS_OBJECT collision_103;
static NJS_MATERIAL collision_104_materials[1];
static Sint16 collision_104_faces[32];
static NJS_MESHSET collision_104_meshsets[1];
static NJS_POINT3 collision_104_points[12];
static NJS_MODEL collision_104_model;
static NJS_OBJECT collision_104;
static NJS_MATERIAL collision_105_materials[1];
static Sint16 collision_105_faces[32];
static NJS_MESHSET collision_105_meshsets[1];
static NJS_POINT3 collision_105_points[12];
static NJS_MODEL collision_105_model;
static NJS_OBJECT collision_105;
static NJS_MATERIAL collision_106_materials[1];
static Sint16 collision_106_faces[150];
static NJS_MESHSET collision_106_meshsets[1];
static NJS_POINT3 collision_106_points[68];
static NJS_MODEL collision_106_model;
static NJS_OBJECT collision_106;
static NJS_MATERIAL collision_110_materials[1];
static Sint16 collision_110_faces[9];
static NJS_MESHSET collision_110_meshsets[1];
static NJS_POINT3 collision_110_points[8];
static NJS_MODEL collision_110_model;
static NJS_OBJECT collision_110;
static NJS_MATERIAL collision_112_materials[1];
static Sint16 collision_112_faces[18];
static NJS_MESHSET collision_112_meshsets[1];
static NJS_POINT3 collision_112_points[10];
static NJS_MODEL collision_112_model;
static NJS_OBJECT collision_112;
static NJS_MATERIAL collision_113_materials[1];
static Sint16 collision_113_faces[22];
static NJS_MESHSET collision_113_meshsets[1];
static NJS_POINT3 collision_113_points[12];
static NJS_MODEL collision_113_model;
static NJS_OBJECT collision_113;
static NJS_MATERIAL collision_114_materials[1];
static Sint16 collision_114_faces[16];
static NJS_MESHSET collision_114_meshsets[1];
static NJS_POINT3 collision_114_points[8];
static NJS_MODEL collision_114_model;
static NJS_OBJECT collision_114;
static NJS_MATERIAL collision_115_materials[1];
static Sint16 collision_115_faces[22];
static NJS_MESHSET collision_115_meshsets[1];
static NJS_POINT3 collision_115_points[12];
static NJS_MODEL collision_115_model;
static NJS_OBJECT collision_115;
static NJS_MATERIAL collision_116_materials[1];
static Sint16 collision_116_faces[27];
static NJS_MESHSET collision_116_meshsets[1];
static NJS_POINT3 collision_116_points[12];
static NJS_MODEL collision_116_model;
static NJS_OBJECT collision_116;
static NJS_MATERIAL collision_117_materials[1];
static Sint16 collision_117_faces[30];
static NJS_MESHSET collision_117_meshsets[1];
static NJS_POINT3 collision_117_points[18];
static NJS_MODEL collision_117_model;
static NJS_OBJECT collision_117;
static NJS_MATERIAL collision_118_materials[1];
static Sint16 collision_118_faces[37];
static NJS_MESHSET collision_118_meshsets[1];
static NJS_POINT3 collision_118_points[15];
static NJS_MODEL collision_118_model;
static NJS_OBJECT collision_118;
static NJS_MATERIAL collision_119_materials[1];
static Sint16 collision_119_faces[40];
static NJS_MESHSET collision_119_meshsets[1];
static NJS_POINT3 collision_119_points[17];
static NJS_MODEL collision_119_model;
static NJS_OBJECT collision_119;
static NJS_MATERIAL collision_120_materials[1];
static Sint16 collision_120_faces[18];
static NJS_MESHSET collision_120_meshsets[1];
static NJS_POINT3 collision_120_points[12];
static NJS_MODEL collision_120_model;
static NJS_OBJECT collision_120;
static NJS_MATERIAL collision_124_materials[1];
static Sint16 collision_124_faces[96];
static NJS_MESHSET collision_124_meshsets[1];
static NJS_POINT3 collision_124_points[56];
static NJS_MODEL collision_124_model;
static NJS_OBJECT collision_124;
static NJS_MATERIAL collision_125_materials[1];
static Sint16 collision_125_faces[21];
static NJS_MESHSET collision_125_meshsets[1];
static NJS_POINT3 collision_125_points[12];
static NJS_MODEL collision_125_model;
static NJS_OBJECT collision_125;
static NJS_MATERIAL collision_129_materials[1];
static Sint16 collision_129_faces[357];
static NJS_MESHSET collision_129_meshsets[1];
static NJS_POINT3 collision_129_points[131];
static NJS_MODEL collision_129_model;
static NJS_OBJECT collision_129;
static NJS_MATERIAL collision_130_materials[1];
static Sint16 collision_130_faces[32];
static NJS_MESHSET collision_130_meshsets[1];
static NJS_POINT3 collision_130_points[12];
static NJS_MODEL collision_130_model;
static NJS_OBJECT collision_130;
static NJS_MATERIAL collision_131_materials[1];
static Sint16 collision_131_faces[32];
static NJS_MESHSET collision_131_meshsets[1];
static NJS_POINT3 collision_131_points[12];
static NJS_MODEL collision_131_model;
static NJS_OBJECT collision_131;
static NJS_MATERIAL collision_132_materials[1];
static Sint16 collision_132_faces[32];
static NJS_MESHSET collision_132_meshsets[1];
static NJS_POINT3 collision_132_points[12];
static NJS_MODEL collision_132_model;
static NJS_OBJECT collision_132;
static NJS_MATERIAL collision_133_materials[1];
static Sint16 collision_133_faces[82];
static NJS_MESHSET collision_133_meshsets[1];
static NJS_POINT3 collision_133_points[32];
static NJS_MODEL collision_133_model;
static NJS_OBJECT collision_133;
static NJS_MATERIAL collision_134_materials[1];
static Sint16 collision_134_faces[8];
static NJS_MESHSET collision_134_meshsets[1];
static NJS_POINT3 collision_134_points[5];
static NJS_MODEL collision_134_model;
static NJS_OBJECT collision_134;
static NJS_MATERIAL collision_135_materials[1];
static Sint16 collision_135_faces[8];
static NJS_MESHSET collision_135_meshsets[1];
static NJS_POINT3 collision_135_points[5];
static NJS_MODEL collision_135_model;
static NJS_OBJECT collision_135;
static NJS_MATERIAL collision_136_materials[1];
static Sint16 collision_136_faces[14];
static NJS_MESHSET collision_136_meshsets[1];
static NJS_POINT3 collision_136_points[8];
static NJS_MODEL collision_136_model;
static NJS_OBJECT collision_136;
static NJS_MATERIAL collision_137_materials[1];
static Sint16 collision_137_faces[107];
static NJS_MESHSET collision_137_meshsets[1];
static NJS_POINT3 collision_137_points[34];
static NJS_MODEL collision_137_model;
static NJS_OBJECT collision_137;
static NJS_MATERIAL collision_139_materials[1];
static Sint16 collision_139_faces[49];
static NJS_MESHSET collision_139_meshsets[1];
static NJS_POINT3 collision_139_points[20];
static NJS_MODEL collision_139_model;
static NJS_OBJECT collision_139;
static NJS_MATERIAL collision_140_materials[1];
static Sint16 collision_140_faces[70];
static NJS_MESHSET collision_140_meshsets[1];
static NJS_POINT3 collision_140_points[24];
static NJS_MODEL collision_140_model;
static NJS_OBJECT collision_140;
static NJS_MATERIAL collision_148_materials[1];
static Sint16 collision_148_faces[279];
static NJS_MESHSET collision_148_meshsets[1];
static NJS_POINT3 collision_148_points[101];
static NJS_MODEL collision_148_model;
static NJS_OBJECT collision_148;
static NJS_MATERIAL collision_149_materials[1];
static Sint16 collision_149_faces[151];
static NJS_MESHSET collision_149_meshsets[1];
static NJS_POINT3 collision_149_points[54];
static NJS_MODEL collision_149_model;
static NJS_OBJECT collision_149;
static NJS_MATERIAL collision_150_materials[1];
static Sint16 collision_150_faces[23];
static NJS_MESHSET collision_150_meshsets[1];
static NJS_POINT3 collision_150_points[16];
static NJS_MODEL collision_150_model;
static NJS_OBJECT collision_150;
static NJS_MATERIAL collision_152_materials[1];
static Sint16 collision_152_faces[15];
static NJS_MESHSET collision_152_meshsets[1];
static NJS_POINT3 collision_152_points[12];
static NJS_MODEL collision_152_model;
static NJS_OBJECT collision_152;
static NJS_MATERIAL collision_156_materials[1];
static Sint16 collision_156_faces[5];
static NJS_MESHSET collision_156_meshsets[1];
static NJS_POINT3 collision_156_points[4];
static NJS_MODEL collision_156_model;
static NJS_OBJECT collision_156;
static NJS_MATERIAL collision_158_materials[1];
static Sint16 collision_158_faces[5];
static NJS_MESHSET collision_158_meshsets[1];
static NJS_POINT3 collision_158_points[4];
static NJS_MODEL collision_158_model;
static NJS_OBJECT collision_158;
static NJS_MATERIAL collision_159_materials[1];
static Sint16 collision_159_faces[5];
static NJS_MESHSET collision_159_meshsets[1];
static NJS_POINT3 collision_159_points[4];
static NJS_MODEL collision_159_model;
static NJS_OBJECT collision_159;
static NJS_CNK_OBJECT draw_191;
static NJS_CNK_OBJECT draw_188;
static NJS_CNK_OBJECT draw_183;
static NJS_CNK_OBJECT draw_181;
static NJS_CNK_OBJECT draw_163;
static NJS_CNK_OBJECT draw_160;
static NJS_CNK_OBJECT draw_156;
static NJS_CNK_OBJECT draw_154;
static NJS_CNK_OBJECT draw_152;
static NJS_CNK_OBJECT draw_150;
static NJS_CNK_OBJECT draw_131;
static NJS_CNK_OBJECT draw_129;
static NJS_CNK_OBJECT draw_127;
static NJS_CNK_OBJECT draw_122;
static NJS_CNK_OBJECT draw_121;
static NJS_CNK_OBJECT draw_120;
static NJS_CNK_OBJECT draw_119;
static NJS_CNK_OBJECT draw_118;
static NJS_CNK_OBJECT draw_117;
static NJS_CNK_OBJECT draw_116;
static NJS_CNK_OBJECT draw_108;
static NJS_CNK_OBJECT draw_106;
static NJS_CNK_OBJECT draw_104;
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
static NJS_CNK_OBJECT draw_74;
static NJS_CNK_OBJECT draw_68;
static NJS_CNK_OBJECT draw_66;
static NJS_CNK_OBJECT draw_62;
static NJS_CNK_OBJECT draw_61;
static NJS_CNK_OBJECT draw_60;
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
static NJS_CNK_OBJECT draw_45;
static NJS_CNK_OBJECT draw_44;
static NJS_CNK_OBJECT draw_43;
static NJS_CNK_OBJECT draw_42;
static NJS_CNK_OBJECT draw_41;
static NJS_CNK_OBJECT draw_40;
static NJS_CNK_OBJECT draw_39;
static NJS_CNK_OBJECT draw_37;
static NJS_CNK_OBJECT draw_35;
static NJS_CNK_OBJECT draw_33;
static NJS_CNK_OBJECT draw_31;
static NJS_CNK_OBJECT draw_30;
static NJS_CNK_OBJECT draw_29;
static NJS_CNK_OBJECT draw_28;
static NJS_CNK_OBJECT draw_27;
static NJS_CNK_OBJECT draw_26;
static NJS_CNK_OBJECT draw_25;
static NJS_CNK_OBJECT draw_23;
static NJS_CNK_OBJECT draw_22;
static NJS_CNK_OBJECT draw_21;
static NJS_CNK_OBJECT draw_20;
static NJS_CNK_OBJECT draw_19;
static NJS_CNK_OBJECT draw_17;
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
static NJS_OBJECT collision_0;
static NJS_OBJECT collision_3;
static NJS_OBJECT collision_7;
static NJS_OBJECT collision_10;
static NJS_OBJECT collision_12;
static NJS_OBJECT collision_27;
static NJS_OBJECT collision_28;
static NJS_OBJECT collision_29;
static NJS_OBJECT collision_31;
static NJS_OBJECT collision_32;
static NJS_OBJECT collision_33;
static NJS_OBJECT collision_35;
static NJS_OBJECT collision_41;
static NJS_OBJECT collision_42;
static NJS_OBJECT collision_50;
static NJS_OBJECT collision_57;
static NJS_OBJECT collision_59;
static NJS_OBJECT collision_61;
static NJS_OBJECT collision_62;
static NJS_OBJECT collision_63;
static NJS_OBJECT collision_64;
static NJS_OBJECT collision_65;
static NJS_OBJECT collision_66;
static NJS_OBJECT collision_67;
static NJS_OBJECT collision_69;
static NJS_OBJECT collision_71;
static NJS_OBJECT collision_72;
static NJS_OBJECT collision_77;
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
static NJS_OBJECT collision_100;
static NJS_OBJECT collision_101;
static NJS_OBJECT collision_107;
static NJS_OBJECT collision_108;
static NJS_OBJECT collision_109;
static NJS_OBJECT collision_111;
static NJS_OBJECT collision_121;
static NJS_OBJECT collision_122;
static NJS_OBJECT collision_123;
static NJS_OBJECT collision_126;
static NJS_OBJECT collision_127;
static NJS_OBJECT collision_128;
static NJS_OBJECT collision_138;
static NJS_OBJECT collision_141;
static NJS_OBJECT collision_142;
static NJS_OBJECT collision_143;
static NJS_OBJECT collision_144;
static NJS_OBJECT collision_145;
static NJS_OBJECT collision_146;
static NJS_OBJECT collision_147;
static NJS_OBJECT collision_151;
static NJS_OBJECT collision_153;
static NJS_OBJECT collision_154;
static NJS_OBJECT collision_155;
static NJS_OBJECT collision_157;

static struct land_entry entries[360] = {
#include "stg26/terrain/entries.inc"
};
struct land_table quarters_land = {
#include "stg26/terrain/quarters_land.inc"
};
static NJS_TEXNAME textures[42] = {
#include "stg26/terrain/textures.inc"
};
NJS_TEXLIST egg_quarters_land_texlist = {
#include "stg26/terrain/egg_quarters_land_texlist.inc"
};
static Sint16 draw_199_polygons[147] = {
#include "stg26/terrain/draw_199_polygons.inc"
};
static Sint32 draw_199_vertices[99] = {
#include "stg26/terrain/draw_199_vertices.inc"
};
static NJS_CNK_MODEL draw_199_model = {
#include "stg26/terrain/draw_199_model.inc"
};
static NJS_CNK_OBJECT draw_199 = {
#include "stg26/terrain/draw_199.inc"
};
static Sint16 draw_198_polygons[153] = {
#include "stg26/terrain/draw_198_polygons.inc"
};
static Sint32 draw_198_vertices[83] = {
#include "stg26/terrain/draw_198_vertices.inc"
};
static NJS_CNK_MODEL draw_198_model = {
#include "stg26/terrain/draw_198_model.inc"
};
static NJS_CNK_OBJECT draw_198 = {
#include "stg26/terrain/draw_198.inc"
};
static Sint16 draw_197_polygons[109] = {
#include "stg26/terrain/draw_197_polygons.inc"
};
static Sint32 draw_197_vertices[91] = {
#include "stg26/terrain/draw_197_vertices.inc"
};
static NJS_CNK_MODEL draw_197_model = {
#include "stg26/terrain/draw_197_model.inc"
};
static NJS_CNK_OBJECT draw_197 = {
#include "stg26/terrain/draw_197.inc"
};
static Sint16 draw_196_polygons[129] = {
#include "stg26/terrain/draw_196_polygons.inc"
};
static Sint32 draw_196_vertices[123] = {
#include "stg26/terrain/draw_196_vertices.inc"
};
static NJS_CNK_MODEL draw_196_model = {
#include "stg26/terrain/draw_196_model.inc"
};
static NJS_CNK_OBJECT draw_196 = {
#include "stg26/terrain/draw_196.inc"
};
static Sint16 draw_195_polygons[953] = {
#include "stg26/terrain/draw_195_polygons.inc"
};
static Sint32 draw_195_vertices[775] = {
#include "stg26/terrain/draw_195_vertices.inc"
};
static NJS_CNK_MODEL draw_195_model = {
#include "stg26/terrain/draw_195_model.inc"
};
static NJS_CNK_OBJECT draw_195 = {
#include "stg26/terrain/draw_195.inc"
};
static Sint16 draw_194_polygons[285] = {
#include "stg26/terrain/draw_194_polygons.inc"
};
static Sint32 draw_194_vertices[203] = {
#include "stg26/terrain/draw_194_vertices.inc"
};
static NJS_CNK_MODEL draw_194_model = {
#include "stg26/terrain/draw_194_model.inc"
};
static NJS_CNK_OBJECT draw_194 = {
#include "stg26/terrain/draw_194.inc"
};
static Sint16 draw_193_polygons[109] = {
#include "stg26/terrain/draw_193_polygons.inc"
};
static Sint32 draw_193_vertices[95] = {
#include "stg26/terrain/draw_193_vertices.inc"
};
static NJS_CNK_MODEL draw_193_model = {
#include "stg26/terrain/draw_193_model.inc"
};
static NJS_CNK_OBJECT draw_193 = {
#include "stg26/terrain/draw_193.inc"
};
static Sint16 draw_191_polygons[197] = {
#include "stg26/terrain/draw_191_polygons.inc"
};
static Sint32 draw_191_vertices[99] = {
#include "stg26/terrain/draw_191_vertices.inc"
};
static NJS_CNK_MODEL draw_191_model = {
#include "stg26/terrain/draw_191_model.inc"
};
static NJS_CNK_OBJECT draw_192 = {
#include "stg26/terrain/draw_192.inc"
};
static Sint16 draw_190_polygons[161] = {
#include "stg26/terrain/draw_190_polygons.inc"
};
static Sint32 draw_190_vertices[147] = {
#include "stg26/terrain/draw_190_vertices.inc"
};
static NJS_CNK_MODEL draw_190_model = {
#include "stg26/terrain/draw_190_model.inc"
};
static NJS_CNK_OBJECT draw_190 = {
#include "stg26/terrain/draw_190.inc"
};
static Sint16 draw_188_polygons[189] = {
#include "stg26/terrain/draw_188_polygons.inc"
};
static Sint32 draw_188_vertices[99] = {
#include "stg26/terrain/draw_188_vertices.inc"
};
static NJS_CNK_MODEL draw_188_model = {
#include "stg26/terrain/draw_188_model.inc"
};
static NJS_CNK_OBJECT draw_189 = {
#include "stg26/terrain/draw_189.inc"
};
static Sint16 draw_187_polygons[153] = {
#include "stg26/terrain/draw_187_polygons.inc"
};
static Sint32 draw_187_vertices[79] = {
#include "stg26/terrain/draw_187_vertices.inc"
};
static NJS_CNK_MODEL draw_187_model = {
#include "stg26/terrain/draw_187_model.inc"
};
static NJS_CNK_OBJECT draw_187 = {
#include "stg26/terrain/draw_187.inc"
};
static Sint16 draw_186_polygons[319] = {
#include "stg26/terrain/draw_186_polygons.inc"
};
static Sint32 draw_186_vertices[239] = {
#include "stg26/terrain/draw_186_vertices.inc"
};
static NJS_CNK_MODEL draw_186_model = {
#include "stg26/terrain/draw_186_model.inc"
};
static NJS_CNK_OBJECT draw_186 = {
#include "stg26/terrain/draw_186.inc"
};
static Sint16 draw_185_polygons[503] = {
#include "stg26/terrain/draw_185_polygons.inc"
};
static Sint32 draw_185_vertices[375] = {
#include "stg26/terrain/draw_185_vertices.inc"
};
static NJS_CNK_MODEL draw_185_model = {
#include "stg26/terrain/draw_185_model.inc"
};
static NJS_CNK_OBJECT draw_185 = {
#include "stg26/terrain/draw_185.inc"
};
static Sint16 draw_183_polygons[147] = {
#include "stg26/terrain/draw_183_polygons.inc"
};
static Sint32 draw_183_vertices[99] = {
#include "stg26/terrain/draw_183_vertices.inc"
};
static NJS_CNK_MODEL draw_183_model = {
#include "stg26/terrain/draw_183_model.inc"
};
static NJS_CNK_OBJECT draw_184 = {
#include "stg26/terrain/draw_184.inc"
};
static Sint16 draw_181_polygons[147] = {
#include "stg26/terrain/draw_181_polygons.inc"
};
static Sint32 draw_181_vertices[99] = {
#include "stg26/terrain/draw_181_vertices.inc"
};
static NJS_CNK_MODEL draw_181_model = {
#include "stg26/terrain/draw_181_model.inc"
};
static NJS_CNK_OBJECT draw_182 = {
#include "stg26/terrain/draw_182.inc"
};
static Sint16 draw_180_polygons[899] = {
#include "stg26/terrain/draw_180_polygons.inc"
};
static Sint32 draw_180_vertices[459] = {
#include "stg26/terrain/draw_180_vertices.inc"
};
static NJS_CNK_MODEL draw_180_model = {
#include "stg26/terrain/draw_180_model.inc"
};
static NJS_CNK_OBJECT draw_180 = {
#include "stg26/terrain/draw_180.inc"
};
static Sint16 draw_179_polygons[57] = {
#include "stg26/terrain/draw_179_polygons.inc"
};
static Sint32 draw_179_vertices[43] = {
#include "stg26/terrain/draw_179_vertices.inc"
};
static NJS_CNK_MODEL draw_179_model = {
#include "stg26/terrain/draw_179_model.inc"
};
static NJS_CNK_OBJECT draw_179 = {
#include "stg26/terrain/draw_179.inc"
};
static Sint16 draw_178_polygons[719] = {
#include "stg26/terrain/draw_178_polygons.inc"
};
static Sint32 draw_178_vertices[403] = {
#include "stg26/terrain/draw_178_vertices.inc"
};
static NJS_CNK_MODEL draw_178_model = {
#include "stg26/terrain/draw_178_model.inc"
};
static NJS_CNK_OBJECT draw_178 = {
#include "stg26/terrain/draw_178.inc"
};
static Sint16 draw_177_polygons[307] = {
#include "stg26/terrain/draw_177_polygons.inc"
};
static Sint32 draw_177_vertices[251] = {
#include "stg26/terrain/draw_177_vertices.inc"
};
static NJS_CNK_MODEL draw_177_model = {
#include "stg26/terrain/draw_177_model.inc"
};
static NJS_CNK_OBJECT draw_177 = {
#include "stg26/terrain/draw_177.inc"
};
static Sint16 draw_176_polygons[57] = {
#include "stg26/terrain/draw_176_polygons.inc"
};
static Sint32 draw_176_vertices[51] = {
#include "stg26/terrain/draw_176_vertices.inc"
};
static NJS_CNK_MODEL draw_176_model = {
#include "stg26/terrain/draw_176_model.inc"
};
static NJS_CNK_OBJECT draw_176 = {
#include "stg26/terrain/draw_176.inc"
};
static Sint16 draw_175_polygons[65] = {
#include "stg26/terrain/draw_175_polygons.inc"
};
static Sint32 draw_175_vertices[67] = {
#include "stg26/terrain/draw_175_vertices.inc"
};
static NJS_CNK_MODEL draw_175_model = {
#include "stg26/terrain/draw_175_model.inc"
};
static NJS_CNK_OBJECT draw_175 = {
#include "stg26/terrain/draw_175.inc"
};
static Sint16 draw_174_polygons[601] = {
#include "stg26/terrain/draw_174_polygons.inc"
};
static Sint32 draw_174_vertices[491] = {
#include "stg26/terrain/draw_174_vertices.inc"
};
static NJS_CNK_MODEL draw_174_model = {
#include "stg26/terrain/draw_174_model.inc"
};
static NJS_CNK_OBJECT draw_174 = {
#include "stg26/terrain/draw_174.inc"
};
static Sint16 draw_173_polygons[427] = {
#include "stg26/terrain/draw_173_polygons.inc"
};
static Sint32 draw_173_vertices[351] = {
#include "stg26/terrain/draw_173_vertices.inc"
};
static NJS_CNK_MODEL draw_173_model = {
#include "stg26/terrain/draw_173_model.inc"
};
static NJS_CNK_OBJECT draw_173 = {
#include "stg26/terrain/draw_173.inc"
};
static Sint16 draw_172_polygons[255] = {
#include "stg26/terrain/draw_172_polygons.inc"
};
static Sint32 draw_172_vertices[91] = {
#include "stg26/terrain/draw_172_vertices.inc"
};
static NJS_CNK_MODEL draw_172_model = {
#include "stg26/terrain/draw_172_model.inc"
};
static NJS_CNK_OBJECT draw_172 = {
#include "stg26/terrain/draw_172.inc"
};
static Sint16 draw_171_polygons[521] = {
#include "stg26/terrain/draw_171_polygons.inc"
};
static Sint32 draw_171_vertices[279] = {
#include "stg26/terrain/draw_171_vertices.inc"
};
static NJS_CNK_MODEL draw_171_model = {
#include "stg26/terrain/draw_171_model.inc"
};
static NJS_CNK_OBJECT draw_171 = {
#include "stg26/terrain/draw_171.inc"
};
static Sint16 draw_170_polygons[153] = {
#include "stg26/terrain/draw_170_polygons.inc"
};
static Sint32 draw_170_vertices[83] = {
#include "stg26/terrain/draw_170_vertices.inc"
};
static NJS_CNK_MODEL draw_170_model = {
#include "stg26/terrain/draw_170_model.inc"
};
static NJS_CNK_OBJECT draw_170 = {
#include "stg26/terrain/draw_170.inc"
};
static Sint16 draw_169_polygons[109] = {
#include "stg26/terrain/draw_169_polygons.inc"
};
static Sint32 draw_169_vertices[91] = {
#include "stg26/terrain/draw_169_vertices.inc"
};
static NJS_CNK_MODEL draw_169_model = {
#include "stg26/terrain/draw_169_model.inc"
};
static NJS_CNK_OBJECT draw_169 = {
#include "stg26/terrain/draw_169.inc"
};
static Sint16 draw_168_polygons[129] = {
#include "stg26/terrain/draw_168_polygons.inc"
};
static Sint32 draw_168_vertices[119] = {
#include "stg26/terrain/draw_168_vertices.inc"
};
static NJS_CNK_MODEL draw_168_model = {
#include "stg26/terrain/draw_168_model.inc"
};
static NJS_CNK_OBJECT draw_168 = {
#include "stg26/terrain/draw_168.inc"
};
static Sint16 draw_167_polygons[953] = {
#include "stg26/terrain/draw_167_polygons.inc"
};
static Sint32 draw_167_vertices[763] = {
#include "stg26/terrain/draw_167_vertices.inc"
};
static NJS_CNK_MODEL draw_167_model = {
#include "stg26/terrain/draw_167_model.inc"
};
static NJS_CNK_OBJECT draw_167 = {
#include "stg26/terrain/draw_167.inc"
};
static Sint16 draw_166_polygons[285] = {
#include "stg26/terrain/draw_166_polygons.inc"
};
static Sint32 draw_166_vertices[199] = {
#include "stg26/terrain/draw_166_vertices.inc"
};
static NJS_CNK_MODEL draw_166_model = {
#include "stg26/terrain/draw_166_model.inc"
};
static NJS_CNK_OBJECT draw_166 = {
#include "stg26/terrain/draw_166.inc"
};
static Sint16 draw_165_polygons[95] = {
#include "stg26/terrain/draw_165_polygons.inc"
};
static Sint32 draw_165_vertices[91] = {
#include "stg26/terrain/draw_165_vertices.inc"
};
static NJS_CNK_MODEL draw_165_model = {
#include "stg26/terrain/draw_165_model.inc"
};
static NJS_CNK_OBJECT draw_165 = {
#include "stg26/terrain/draw_165.inc"
};
static Sint16 draw_163_polygons[189] = {
#include "stg26/terrain/draw_163_polygons.inc"
};
static Sint32 draw_163_vertices[103] = {
#include "stg26/terrain/draw_163_vertices.inc"
};
static NJS_CNK_MODEL draw_163_model = {
#include "stg26/terrain/draw_163_model.inc"
};
static NJS_CNK_OBJECT draw_164 = {
#include "stg26/terrain/draw_164.inc"
};
static Sint16 draw_162_polygons[161] = {
#include "stg26/terrain/draw_162_polygons.inc"
};
static Sint32 draw_162_vertices[131] = {
#include "stg26/terrain/draw_162_vertices.inc"
};
static NJS_CNK_MODEL draw_162_model = {
#include "stg26/terrain/draw_162_model.inc"
};
static NJS_CNK_OBJECT draw_162 = {
#include "stg26/terrain/draw_162.inc"
};
static Sint16 draw_160_polygons[189] = {
#include "stg26/terrain/draw_160_polygons.inc"
};
static Sint32 draw_160_vertices[99] = {
#include "stg26/terrain/draw_160_vertices.inc"
};
static NJS_CNK_MODEL draw_160_model = {
#include "stg26/terrain/draw_160_model.inc"
};
static NJS_CNK_OBJECT draw_161 = {
#include "stg26/terrain/draw_161.inc"
};
static Sint16 draw_159_polygons[147] = {
#include "stg26/terrain/draw_159_polygons.inc"
};
static Sint32 draw_159_vertices[79] = {
#include "stg26/terrain/draw_159_vertices.inc"
};
static NJS_CNK_MODEL draw_159_model = {
#include "stg26/terrain/draw_159_model.inc"
};
static NJS_CNK_OBJECT draw_159 = {
#include "stg26/terrain/draw_159.inc"
};
static Sint16 draw_158_polygons[503] = {
#include "stg26/terrain/draw_158_polygons.inc"
};
static Sint32 draw_158_vertices[375] = {
#include "stg26/terrain/draw_158_vertices.inc"
};
static NJS_CNK_MODEL draw_158_model = {
#include "stg26/terrain/draw_158_model.inc"
};
static NJS_CNK_OBJECT draw_158 = {
#include "stg26/terrain/draw_158.inc"
};
static Sint16 draw_157_polygons[319] = {
#include "stg26/terrain/draw_157_polygons.inc"
};
static Sint32 draw_157_vertices[239] = {
#include "stg26/terrain/draw_157_vertices.inc"
};
static NJS_CNK_MODEL draw_157_model = {
#include "stg26/terrain/draw_157_model.inc"
};
static NJS_CNK_OBJECT draw_157 = {
#include "stg26/terrain/draw_157.inc"
};
static Sint16 draw_154_polygons[55] = {
#include "stg26/terrain/draw_154_polygons.inc"
};
static Sint32 draw_154_vertices[51] = {
#include "stg26/terrain/draw_154_vertices.inc"
};
static NJS_CNK_MODEL draw_154_model = {
#include "stg26/terrain/draw_154_model.inc"
};
static NJS_CNK_OBJECT draw_155 = {
#include "stg26/terrain/draw_155.inc"
};
static Sint16 draw_152_polygons[203] = {
#include "stg26/terrain/draw_152_polygons.inc"
};
static Sint32 draw_152_vertices[151] = {
#include "stg26/terrain/draw_152_vertices.inc"
};
static NJS_CNK_MODEL draw_152_model = {
#include "stg26/terrain/draw_152_model.inc"
};
static NJS_CNK_OBJECT draw_153 = {
#include "stg26/terrain/draw_153.inc"
};
static Sint16 draw_150_polygons[55] = {
#include "stg26/terrain/draw_150_polygons.inc"
};
static Sint32 draw_150_vertices[51] = {
#include "stg26/terrain/draw_150_vertices.inc"
};
static NJS_CNK_MODEL draw_150_model = {
#include "stg26/terrain/draw_150_model.inc"
};
static NJS_CNK_OBJECT draw_151 = {
#include "stg26/terrain/draw_151.inc"
};
static Sint16 draw_149_polygons[683] = {
#include "stg26/terrain/draw_149_polygons.inc"
};
static Sint32 draw_149_vertices[363] = {
#include "stg26/terrain/draw_149_vertices.inc"
};
static NJS_CNK_MODEL draw_149_model = {
#include "stg26/terrain/draw_149_model.inc"
};
static NJS_CNK_OBJECT draw_149 = {
#include "stg26/terrain/draw_149.inc"
};
static Sint16 draw_148_polygons[429] = {
#include "stg26/terrain/draw_148_polygons.inc"
};
static Sint32 draw_148_vertices[399] = {
#include "stg26/terrain/draw_148_vertices.inc"
};
static NJS_CNK_MODEL draw_148_model = {
#include "stg26/terrain/draw_148_model.inc"
};
static NJS_CNK_OBJECT draw_148 = {
#include "stg26/terrain/draw_148.inc"
};
static Sint16 draw_147_polygons[65] = {
#include "stg26/terrain/draw_147_polygons.inc"
};
static Sint32 draw_147_vertices[67] = {
#include "stg26/terrain/draw_147_vertices.inc"
};
static NJS_CNK_MODEL draw_147_model = {
#include "stg26/terrain/draw_147_model.inc"
};
static NJS_CNK_OBJECT draw_147 = {
#include "stg26/terrain/draw_147.inc"
};
static Sint16 draw_146_polygons[423] = {
#include "stg26/terrain/draw_146_polygons.inc"
};
static Sint32 draw_146_vertices[379] = {
#include "stg26/terrain/draw_146_vertices.inc"
};
static NJS_CNK_MODEL draw_146_model = {
#include "stg26/terrain/draw_146_model.inc"
};
static NJS_CNK_OBJECT draw_146 = {
#include "stg26/terrain/draw_146.inc"
};
static Sint16 draw_145_polygons[609] = {
#include "stg26/terrain/draw_145_polygons.inc"
};
static Sint32 draw_145_vertices[491] = {
#include "stg26/terrain/draw_145_vertices.inc"
};
static NJS_CNK_MODEL draw_145_model = {
#include "stg26/terrain/draw_145_model.inc"
};
static NJS_CNK_OBJECT draw_145 = {
#include "stg26/terrain/draw_145.inc"
};
static Sint16 draw_144_polygons[255] = {
#include "stg26/terrain/draw_144_polygons.inc"
};
static Sint32 draw_144_vertices[91] = {
#include "stg26/terrain/draw_144_vertices.inc"
};
static NJS_CNK_MODEL draw_144_model = {
#include "stg26/terrain/draw_144_model.inc"
};
static NJS_CNK_OBJECT draw_144 = {
#include "stg26/terrain/draw_144.inc"
};
static Sint16 draw_143_polygons[1147] = {
#include "stg26/terrain/draw_143_polygons.inc"
};
static Sint32 draw_143_vertices[651] = {
#include "stg26/terrain/draw_143_vertices.inc"
};
static NJS_CNK_MODEL draw_143_model = {
#include "stg26/terrain/draw_143_model.inc"
};
static NJS_CNK_OBJECT draw_143 = {
#include "stg26/terrain/draw_143.inc"
};
static Sint16 draw_142_polygons[103] = {
#include "stg26/terrain/draw_142_polygons.inc"
};
static Sint32 draw_142_vertices[75] = {
#include "stg26/terrain/draw_142_vertices.inc"
};
static NJS_CNK_MODEL draw_142_model = {
#include "stg26/terrain/draw_142_model.inc"
};
static NJS_CNK_OBJECT draw_142 = {
#include "stg26/terrain/draw_142.inc"
};
static Sint16 draw_141_polygons[529] = {
#include "stg26/terrain/draw_141_polygons.inc"
};
static Sint32 draw_141_vertices[295] = {
#include "stg26/terrain/draw_141_vertices.inc"
};
static NJS_CNK_MODEL draw_141_model = {
#include "stg26/terrain/draw_141_model.inc"
};
static NJS_CNK_OBJECT draw_141 = {
#include "stg26/terrain/draw_141.inc"
};
static Sint16 draw_140_polygons[203] = {
#include "stg26/terrain/draw_140_polygons.inc"
};
static Sint32 draw_140_vertices[147] = {
#include "stg26/terrain/draw_140_vertices.inc"
};
static NJS_CNK_MODEL draw_140_model = {
#include "stg26/terrain/draw_140_model.inc"
};
static NJS_CNK_OBJECT draw_140 = {
#include "stg26/terrain/draw_140.inc"
};
static Sint16 draw_139_polygons[989] = {
#include "stg26/terrain/draw_139_polygons.inc"
};
static Sint32 draw_139_vertices[835] = {
#include "stg26/terrain/draw_139_vertices.inc"
};
static NJS_CNK_MODEL draw_139_model = {
#include "stg26/terrain/draw_139_model.inc"
};
static NJS_CNK_OBJECT draw_139 = {
#include "stg26/terrain/draw_139.inc"
};
static Sint16 draw_138_polygons[355] = {
#include "stg26/terrain/draw_138_polygons.inc"
};
static Sint32 draw_138_vertices[259] = {
#include "stg26/terrain/draw_138_vertices.inc"
};
static NJS_CNK_MODEL draw_138_model = {
#include "stg26/terrain/draw_138_model.inc"
};
static NJS_CNK_OBJECT draw_138 = {
#include "stg26/terrain/draw_138.inc"
};
static Sint16 draw_137_polygons[715] = {
#include "stg26/terrain/draw_137_polygons.inc"
};
static Sint32 draw_137_vertices[415] = {
#include "stg26/terrain/draw_137_vertices.inc"
};
static NJS_CNK_MODEL draw_137_model = {
#include "stg26/terrain/draw_137_model.inc"
};
static NJS_CNK_OBJECT draw_137 = {
#include "stg26/terrain/draw_137.inc"
};
static Sint16 draw_136_polygons[103] = {
#include "stg26/terrain/draw_136_polygons.inc"
};
static Sint32 draw_136_vertices[99] = {
#include "stg26/terrain/draw_136_vertices.inc"
};
static NJS_CNK_MODEL draw_136_model = {
#include "stg26/terrain/draw_136_model.inc"
};
static NJS_CNK_OBJECT draw_136 = {
#include "stg26/terrain/draw_136.inc"
};
static Sint16 draw_135_polygons[527] = {
#include "stg26/terrain/draw_135_polygons.inc"
};
static Sint32 draw_135_vertices[315] = {
#include "stg26/terrain/draw_135_vertices.inc"
};
static NJS_CNK_MODEL draw_135_model = {
#include "stg26/terrain/draw_135_model.inc"
};
static NJS_CNK_OBJECT draw_135 = {
#include "stg26/terrain/draw_135.inc"
};
static Sint16 draw_134_polygons[139] = {
#include "stg26/terrain/draw_134_polygons.inc"
};
static Sint32 draw_134_vertices[107] = {
#include "stg26/terrain/draw_134_vertices.inc"
};
static NJS_CNK_MODEL draw_134_model = {
#include "stg26/terrain/draw_134_model.inc"
};
static NJS_CNK_OBJECT draw_134 = {
#include "stg26/terrain/draw_134.inc"
};
static Sint16 draw_133_polygons[257] = {
#include "stg26/terrain/draw_133_polygons.inc"
};
static Sint32 draw_133_vertices[179] = {
#include "stg26/terrain/draw_133_vertices.inc"
};
static NJS_CNK_MODEL draw_133_model = {
#include "stg26/terrain/draw_133_model.inc"
};
static NJS_CNK_OBJECT draw_133 = {
#include "stg26/terrain/draw_133.inc"
};
static Sint16 draw_132_polygons[41] = {
#include "stg26/terrain/draw_132_polygons.inc"
};
static Sint32 draw_132_vertices[27] = {
#include "stg26/terrain/draw_132_vertices.inc"
};
static NJS_CNK_MODEL draw_132_model = {
#include "stg26/terrain/draw_132_model.inc"
};
static NJS_CNK_OBJECT draw_132 = {
#include "stg26/terrain/draw_132.inc"
};
static Sint16 draw_130_polygons[473] = {
#include "stg26/terrain/draw_130_polygons.inc"
};
static Sint32 draw_130_vertices[343] = {
#include "stg26/terrain/draw_130_vertices.inc"
};
static NJS_CNK_MODEL draw_130_model = {
#include "stg26/terrain/draw_130_model.inc"
};
static NJS_CNK_OBJECT draw_130 = {
#include "stg26/terrain/draw_130.inc"
};
static Sint16 draw_128_polygons[119] = {
#include "stg26/terrain/draw_128_polygons.inc"
};
static Sint32 draw_128_vertices[75] = {
#include "stg26/terrain/draw_128_vertices.inc"
};
static NJS_CNK_MODEL draw_128_model = {
#include "stg26/terrain/draw_128_model.inc"
};
static NJS_CNK_OBJECT draw_128 = {
#include "stg26/terrain/draw_128.inc"
};
static Sint16 draw_126_polygons[81] = {
#include "stg26/terrain/draw_126_polygons.inc"
};
static Sint32 draw_126_vertices[75] = {
#include "stg26/terrain/draw_126_vertices.inc"
};
static NJS_CNK_MODEL draw_126_model = {
#include "stg26/terrain/draw_126_model.inc"
};
static NJS_CNK_OBJECT draw_126 = {
#include "stg26/terrain/draw_126.inc"
};
static Sint16 draw_125_polygons[523] = {
#include "stg26/terrain/draw_125_polygons.inc"
};
static Sint32 draw_125_vertices[363] = {
#include "stg26/terrain/draw_125_vertices.inc"
};
static NJS_CNK_MODEL draw_125_model = {
#include "stg26/terrain/draw_125_model.inc"
};
static NJS_CNK_OBJECT draw_125 = {
#include "stg26/terrain/draw_125.inc"
};
static Sint16 draw_124_polygons[593] = {
#include "stg26/terrain/draw_124_polygons.inc"
};
static Sint32 draw_124_vertices[419] = {
#include "stg26/terrain/draw_124_vertices.inc"
};
static NJS_CNK_MODEL draw_124_model = {
#include "stg26/terrain/draw_124_model.inc"
};
static NJS_CNK_OBJECT draw_124 = {
#include "stg26/terrain/draw_124.inc"
};
static Sint16 draw_116_polygons[269] = {
#include "stg26/terrain/draw_116_polygons.inc"
};
static Sint32 draw_116_vertices[147] = {
#include "stg26/terrain/draw_116_vertices.inc"
};
static NJS_CNK_MODEL draw_116_model = {
#include "stg26/terrain/draw_116_model.inc"
};
static NJS_CNK_OBJECT draw_123 = {
#include "stg26/terrain/draw_123.inc"
};
static Sint16 draw_115_polygons[2341] = {
#include "stg26/terrain/draw_115_polygons.inc"
};
static Sint32 draw_115_vertices[1991] = {
#include "stg26/terrain/draw_115_vertices.inc"
};
static NJS_CNK_MODEL draw_115_model = {
#include "stg26/terrain/draw_115_model.inc"
};
static NJS_CNK_OBJECT draw_115 = {
#include "stg26/terrain/draw_115.inc"
};
static Sint16 draw_114_polygons[171] = {
#include "stg26/terrain/draw_114_polygons.inc"
};
static Sint32 draw_114_vertices[147] = {
#include "stg26/terrain/draw_114_vertices.inc"
};
static NJS_CNK_MODEL draw_114_model = {
#include "stg26/terrain/draw_114_model.inc"
};
static NJS_CNK_OBJECT draw_114 = {
#include "stg26/terrain/draw_114.inc"
};
static Sint16 draw_113_polygons[121] = {
#include "stg26/terrain/draw_113_polygons.inc"
};
static Sint32 draw_113_vertices[83] = {
#include "stg26/terrain/draw_113_vertices.inc"
};
static NJS_CNK_MODEL draw_113_model = {
#include "stg26/terrain/draw_113_model.inc"
};
static NJS_CNK_OBJECT draw_113 = {
#include "stg26/terrain/draw_113.inc"
};
static Sint16 draw_112_polygons[341] = {
#include "stg26/terrain/draw_112_polygons.inc"
};
static Sint32 draw_112_vertices[147] = {
#include "stg26/terrain/draw_112_vertices.inc"
};
static NJS_CNK_MODEL draw_112_model = {
#include "stg26/terrain/draw_112_model.inc"
};
static NJS_CNK_OBJECT draw_112 = {
#include "stg26/terrain/draw_112.inc"
};
static Sint16 draw_111_polygons[353] = {
#include "stg26/terrain/draw_111_polygons.inc"
};
static Sint32 draw_111_vertices[179] = {
#include "stg26/terrain/draw_111_vertices.inc"
};
static NJS_CNK_MODEL draw_111_model = {
#include "stg26/terrain/draw_111_model.inc"
};
static NJS_CNK_OBJECT draw_111 = {
#include "stg26/terrain/draw_111.inc"
};
static Sint16 draw_110_polygons[331] = {
#include "stg26/terrain/draw_110_polygons.inc"
};
static Sint32 draw_110_vertices[171] = {
#include "stg26/terrain/draw_110_vertices.inc"
};
static NJS_CNK_MODEL draw_110_model = {
#include "stg26/terrain/draw_110_model.inc"
};
static NJS_CNK_OBJECT draw_110 = {
#include "stg26/terrain/draw_110.inc"
};
static Sint16 draw_109_polygons[343] = {
#include "stg26/terrain/draw_109_polygons.inc"
};
static Sint32 draw_109_vertices[163] = {
#include "stg26/terrain/draw_109_vertices.inc"
};
static NJS_CNK_MODEL draw_109_model = {
#include "stg26/terrain/draw_109_model.inc"
};
static NJS_CNK_OBJECT draw_109 = {
#include "stg26/terrain/draw_109.inc"
};
static Sint16 draw_107_polygons[845] = {
#include "stg26/terrain/draw_107_polygons.inc"
};
static Sint32 draw_107_vertices[699] = {
#include "stg26/terrain/draw_107_vertices.inc"
};
static NJS_CNK_MODEL draw_107_model = {
#include "stg26/terrain/draw_107_model.inc"
};
static NJS_CNK_OBJECT draw_107 = {
#include "stg26/terrain/draw_107.inc"
};
static Sint16 draw_105_polygons[189] = {
#include "stg26/terrain/draw_105_polygons.inc"
};
static Sint32 draw_105_vertices[195] = {
#include "stg26/terrain/draw_105_vertices.inc"
};
static NJS_CNK_MODEL draw_105_model = {
#include "stg26/terrain/draw_105_model.inc"
};
static NJS_CNK_OBJECT draw_105 = {
#include "stg26/terrain/draw_105.inc"
};
static Sint16 draw_103_polygons[165] = {
#include "stg26/terrain/draw_103_polygons.inc"
};
static Sint32 draw_103_vertices[195] = {
#include "stg26/terrain/draw_103_vertices.inc"
};
static NJS_CNK_MODEL draw_103_model = {
#include "stg26/terrain/draw_103_model.inc"
};
static NJS_CNK_OBJECT draw_103 = {
#include "stg26/terrain/draw_103.inc"
};
static Sint16 draw_76_polygons[937] = {
#include "stg26/terrain/draw_76_polygons.inc"
};
static Sint32 draw_76_vertices[623] = {
#include "stg26/terrain/draw_76_vertices.inc"
};
static NJS_CNK_MODEL draw_76_model = {
#include "stg26/terrain/draw_76_model.inc"
};
static NJS_CNK_OBJECT draw_76 = {
#include "stg26/terrain/draw_76.inc"
};
static Sint16 draw_75_polygons[147] = {
#include "stg26/terrain/draw_75_polygons.inc"
};
static Sint32 draw_75_vertices[99] = {
#include "stg26/terrain/draw_75_vertices.inc"
};
static NJS_CNK_MODEL draw_75_model = {
#include "stg26/terrain/draw_75_model.inc"
};
static NJS_CNK_OBJECT draw_75 = {
#include "stg26/terrain/draw_75.inc"
};
static Sint16 draw_73_polygons[147] = {
#include "stg26/terrain/draw_73_polygons.inc"
};
static Sint32 draw_73_vertices[99] = {
#include "stg26/terrain/draw_73_vertices.inc"
};
static NJS_CNK_MODEL draw_73_model = {
#include "stg26/terrain/draw_73_model.inc"
};
static NJS_CNK_OBJECT draw_73 = {
#include "stg26/terrain/draw_73.inc"
};
static Sint16 draw_72_polygons[57] = {
#include "stg26/terrain/draw_72_polygons.inc"
};
static Sint32 draw_72_vertices[43] = {
#include "stg26/terrain/draw_72_vertices.inc"
};
static NJS_CNK_MODEL draw_72_model = {
#include "stg26/terrain/draw_72_model.inc"
};
static NJS_CNK_OBJECT draw_72 = {
#include "stg26/terrain/draw_72.inc"
};
static Sint16 draw_71_polygons[307] = {
#include "stg26/terrain/draw_71_polygons.inc"
};
static Sint32 draw_71_vertices[251] = {
#include "stg26/terrain/draw_71_vertices.inc"
};
static NJS_CNK_MODEL draw_71_model = {
#include "stg26/terrain/draw_71_model.inc"
};
static NJS_CNK_OBJECT draw_71 = {
#include "stg26/terrain/draw_71.inc"
};
static Sint16 draw_70_polygons[697] = {
#include "stg26/terrain/draw_70_polygons.inc"
};
static Sint32 draw_70_vertices[335] = {
#include "stg26/terrain/draw_70_vertices.inc"
};
static NJS_CNK_MODEL draw_70_model = {
#include "stg26/terrain/draw_70_model.inc"
};
static NJS_CNK_OBJECT draw_70 = {
#include "stg26/terrain/draw_70.inc"
};
static Sint16 draw_68_polygons[147] = {
#include "stg26/terrain/draw_68_polygons.inc"
};
static Sint32 draw_68_vertices[99] = {
#include "stg26/terrain/draw_68_vertices.inc"
};
static NJS_CNK_MODEL draw_68_model = {
#include "stg26/terrain/draw_68_model.inc"
};
static NJS_CNK_OBJECT draw_69 = {
#include "stg26/terrain/draw_69.inc"
};
static Sint16 draw_66_polygons[147] = {
#include "stg26/terrain/draw_66_polygons.inc"
};
static Sint32 draw_66_vertices[99] = {
#include "stg26/terrain/draw_66_vertices.inc"
};
static NJS_CNK_MODEL draw_66_model = {
#include "stg26/terrain/draw_66_model.inc"
};
static NJS_CNK_OBJECT draw_67 = {
#include "stg26/terrain/draw_67.inc"
};
static Sint16 draw_65_polygons[1013] = {
#include "stg26/terrain/draw_65_polygons.inc"
};
static Sint32 draw_65_vertices[867] = {
#include "stg26/terrain/draw_65_vertices.inc"
};
static NJS_CNK_MODEL draw_65_model = {
#include "stg26/terrain/draw_65_model.inc"
};
static NJS_CNK_OBJECT draw_65 = {
#include "stg26/terrain/draw_65.inc"
};
static Sint16 draw_64_polygons[555] = {
#include "stg26/terrain/draw_64_polygons.inc"
};
static Sint32 draw_64_vertices[239] = {
#include "stg26/terrain/draw_64_vertices.inc"
};
static NJS_CNK_MODEL draw_64_model = {
#include "stg26/terrain/draw_64_model.inc"
};
static NJS_CNK_OBJECT draw_64 = {
#include "stg26/terrain/draw_64.inc"
};
static Sint16 draw_60_polygons[57] = {
#include "stg26/terrain/draw_60_polygons.inc"
};
static Sint32 draw_60_vertices[43] = {
#include "stg26/terrain/draw_60_vertices.inc"
};
static NJS_CNK_MODEL draw_60_model = {
#include "stg26/terrain/draw_60_model.inc"
};
static NJS_CNK_OBJECT draw_63 = {
#include "stg26/terrain/draw_63.inc"
};
static Sint16 draw_59_polygons[575] = {
#include "stg26/terrain/draw_59_polygons.inc"
};
static Sint32 draw_59_vertices[247] = {
#include "stg26/terrain/draw_59_vertices.inc"
};
static NJS_CNK_MODEL draw_59_model = {
#include "stg26/terrain/draw_59_model.inc"
};
static NJS_CNK_OBJECT draw_59 = {
#include "stg26/terrain/draw_59.inc"
};
static Sint16 draw_55_polygons[57] = {
#include "stg26/terrain/draw_55_polygons.inc"
};
static Sint32 draw_55_vertices[43] = {
#include "stg26/terrain/draw_55_vertices.inc"
};
static NJS_CNK_MODEL draw_55_model = {
#include "stg26/terrain/draw_55_model.inc"
};
static NJS_CNK_OBJECT draw_58 = {
#include "stg26/terrain/draw_58.inc"
};
static Sint16 draw_39_polygons[61] = {
#include "stg26/terrain/draw_39_polygons.inc"
};
static Sint32 draw_39_vertices[43] = {
#include "stg26/terrain/draw_39_vertices.inc"
};
static NJS_CNK_MODEL draw_39_model = {
#include "stg26/terrain/draw_39_model.inc"
};
static NJS_CNK_OBJECT draw_54 = {
#include "stg26/terrain/draw_54.inc"
};
static Sint16 draw_37_polygons[63] = {
#include "stg26/terrain/draw_37_polygons.inc"
};
static Sint32 draw_37_vertices[35] = {
#include "stg26/terrain/draw_37_vertices.inc"
};
static NJS_CNK_MODEL draw_37_model = {
#include "stg26/terrain/draw_37_model.inc"
};
static NJS_CNK_OBJECT draw_38 = {
#include "stg26/terrain/draw_38.inc"
};
static Sint16 draw_35_polygons[81] = {
#include "stg26/terrain/draw_35_polygons.inc"
};
static Sint32 draw_35_vertices[59] = {
#include "stg26/terrain/draw_35_vertices.inc"
};
static NJS_CNK_MODEL draw_35_model = {
#include "stg26/terrain/draw_35_model.inc"
};
static NJS_CNK_OBJECT draw_36 = {
#include "stg26/terrain/draw_36.inc"
};
static Sint16 draw_33_polygons[1327] = {
#include "stg26/terrain/draw_33_polygons.inc"
};
static Sint32 draw_33_vertices[999] = {
#include "stg26/terrain/draw_33_vertices.inc"
};
static NJS_CNK_MODEL draw_33_model = {
#include "stg26/terrain/draw_33_model.inc"
};
static NJS_CNK_OBJECT draw_34 = {
#include "stg26/terrain/draw_34.inc"
};
static Sint16 draw_25_polygons[51] = {
#include "stg26/terrain/draw_25_polygons.inc"
};
static Sint32 draw_25_vertices[35] = {
#include "stg26/terrain/draw_25_vertices.inc"
};
static NJS_CNK_MODEL draw_25_model = {
#include "stg26/terrain/draw_25_model.inc"
};
static NJS_CNK_OBJECT draw_32 = {
#include "stg26/terrain/draw_32.inc"
};
static Sint16 draw_19_polygons[141] = {
#include "stg26/terrain/draw_19_polygons.inc"
};
static Sint32 draw_19_vertices[123] = {
#include "stg26/terrain/draw_19_vertices.inc"
};
static NJS_CNK_MODEL draw_19_model = {
#include "stg26/terrain/draw_19_model.inc"
};
static NJS_CNK_OBJECT draw_24 = {
#include "stg26/terrain/draw_24.inc"
};
static Sint16 draw_17_polygons[101] = {
#include "stg26/terrain/draw_17_polygons.inc"
};
static Sint32 draw_17_vertices[67] = {
#include "stg26/terrain/draw_17_vertices.inc"
};
static NJS_CNK_MODEL draw_17_model = {
#include "stg26/terrain/draw_17_model.inc"
};
static NJS_CNK_OBJECT draw_18 = {
#include "stg26/terrain/draw_18.inc"
};
static Sint16 draw_1_polygons[63] = {
#include "stg26/terrain/draw_1_polygons.inc"
};
static Sint32 draw_1_vertices[59] = {
#include "stg26/terrain/draw_1_vertices.inc"
};
static NJS_CNK_MODEL draw_1_model = {
#include "stg26/terrain/draw_1_model.inc"
};
static NJS_CNK_OBJECT draw_16 = {
#include "stg26/terrain/draw_16.inc"
};
static Sint16 draw_0_polygons[991] = {
#include "stg26/terrain/draw_0_polygons.inc"
};
static Sint32 draw_0_vertices[599] = {
#include "stg26/terrain/draw_0_vertices.inc"
};
static NJS_CNK_MODEL draw_0_model = {
#include "stg26/terrain/draw_0_model.inc"
};
static NJS_CNK_OBJECT draw_0 = {
#include "stg26/terrain/draw_0.inc"
};
static NJS_MATERIAL collision_1_materials[1] = {
#include "stg26/terrain/collision_1_materials.inc"
};
static Sint16 collision_1_faces[14] = {
#include "stg26/terrain/collision_1_faces.inc"
};
static NJS_MESHSET collision_1_meshsets[1] = {
#include "stg26/terrain/collision_1_meshsets.inc"
};
static NJS_POINT3 collision_1_points[8] = {
#include "stg26/terrain/collision_1_points.inc"
};
static NJS_MODEL collision_1_model = {
#include "stg26/terrain/collision_1_model.inc"
};
static NJS_OBJECT collision_1 = {
#include "stg26/terrain/collision_1.inc"
};
static NJS_MATERIAL collision_2_materials[1] = {
#include "stg26/terrain/collision_2_materials.inc"
};
static Sint16 collision_2_faces[20] = {
#include "stg26/terrain/collision_2_faces.inc"
};
static NJS_MESHSET collision_2_meshsets[1] = {
#include "stg26/terrain/collision_2_meshsets.inc"
};
static NJS_POINT3 collision_2_points[8] = {
#include "stg26/terrain/collision_2_points.inc"
};
static NJS_MODEL collision_2_model = {
#include "stg26/terrain/collision_2_model.inc"
};
static NJS_OBJECT collision_2 = {
#include "stg26/terrain/collision_2.inc"
};
static NJS_MATERIAL collision_0_materials[1] = {
#include "stg26/terrain/collision_0_materials.inc"
};
static Sint16 collision_0_faces[34] = {
#include "stg26/terrain/collision_0_faces.inc"
};
static NJS_MESHSET collision_0_meshsets[1] = {
#include "stg26/terrain/collision_0_meshsets.inc"
};
static NJS_POINT3 collision_0_points[32] = {
#include "stg26/terrain/collision_0_points.inc"
};
static NJS_MODEL collision_0_model = {
#include "stg26/terrain/collision_0_model.inc"
};
static NJS_OBJECT collision_4 = {
#include "stg26/terrain/collision_4.inc"
};
static NJS_MATERIAL collision_5_materials[1] = {
#include "stg26/terrain/collision_5_materials.inc"
};
static Sint16 collision_5_faces[14] = {
#include "stg26/terrain/collision_5_faces.inc"
};
static NJS_MESHSET collision_5_meshsets[1] = {
#include "stg26/terrain/collision_5_meshsets.inc"
};
static NJS_POINT3 collision_5_points[8] = {
#include "stg26/terrain/collision_5_points.inc"
};
static NJS_MODEL collision_5_model = {
#include "stg26/terrain/collision_5_model.inc"
};
static NJS_OBJECT collision_5 = {
#include "stg26/terrain/collision_5.inc"
};
static NJS_MATERIAL collision_6_materials[1] = {
#include "stg26/terrain/collision_6_materials.inc"
};
static Sint16 collision_6_faces[103] = {
#include "stg26/terrain/collision_6_faces.inc"
};
static NJS_MESHSET collision_6_meshsets[1] = {
#include "stg26/terrain/collision_6_meshsets.inc"
};
static NJS_POINT3 collision_6_points[44] = {
#include "stg26/terrain/collision_6_points.inc"
};
static NJS_MODEL collision_6_model = {
#include "stg26/terrain/collision_6_model.inc"
};
static NJS_OBJECT collision_6 = {
#include "stg26/terrain/collision_6.inc"
};
static NJS_MATERIAL collision_8_materials[1] = {
#include "stg26/terrain/collision_8_materials.inc"
};
static Sint16 collision_8_faces[85] = {
#include "stg26/terrain/collision_8_faces.inc"
};
static NJS_MESHSET collision_8_meshsets[1] = {
#include "stg26/terrain/collision_8_meshsets.inc"
};
static NJS_POINT3 collision_8_points[68] = {
#include "stg26/terrain/collision_8_points.inc"
};
static NJS_MODEL collision_8_model = {
#include "stg26/terrain/collision_8_model.inc"
};
static NJS_OBJECT collision_8 = {
#include "stg26/terrain/collision_8.inc"
};
static NJS_MATERIAL collision_9_materials[1] = {
#include "stg26/terrain/collision_9_materials.inc"
};
static Sint16 collision_9_faces[85] = {
#include "stg26/terrain/collision_9_faces.inc"
};
static NJS_MESHSET collision_9_meshsets[1] = {
#include "stg26/terrain/collision_9_meshsets.inc"
};
static NJS_POINT3 collision_9_points[68] = {
#include "stg26/terrain/collision_9_points.inc"
};
static NJS_MODEL collision_9_model = {
#include "stg26/terrain/collision_9_model.inc"
};
static NJS_OBJECT collision_9 = {
#include "stg26/terrain/collision_9.inc"
};
static NJS_MATERIAL collision_11_materials[1] = {
#include "stg26/terrain/collision_11_materials.inc"
};
static Sint16 collision_11_faces[45] = {
#include "stg26/terrain/collision_11_faces.inc"
};
static NJS_MESHSET collision_11_meshsets[1] = {
#include "stg26/terrain/collision_11_meshsets.inc"
};
static NJS_POINT3 collision_11_points[36] = {
#include "stg26/terrain/collision_11_points.inc"
};
static NJS_MODEL collision_11_model = {
#include "stg26/terrain/collision_11_model.inc"
};
static NJS_OBJECT collision_11 = {
#include "stg26/terrain/collision_11.inc"
};
static NJS_MATERIAL collision_13_materials[1] = {
#include "stg26/terrain/collision_13_materials.inc"
};
static Sint16 collision_13_faces[117] = {
#include "stg26/terrain/collision_13_faces.inc"
};
static NJS_MESHSET collision_13_meshsets[1] = {
#include "stg26/terrain/collision_13_meshsets.inc"
};
static NJS_POINT3 collision_13_points[48] = {
#include "stg26/terrain/collision_13_points.inc"
};
static NJS_MODEL collision_13_model = {
#include "stg26/terrain/collision_13_model.inc"
};
static NJS_OBJECT collision_13 = {
#include "stg26/terrain/collision_13.inc"
};
static NJS_MATERIAL collision_14_materials[1] = {
#include "stg26/terrain/collision_14_materials.inc"
};
static Sint16 collision_14_faces[15] = {
#include "stg26/terrain/collision_14_faces.inc"
};
static NJS_MESHSET collision_14_meshsets[1] = {
#include "stg26/terrain/collision_14_meshsets.inc"
};
static NJS_POINT3 collision_14_points[8] = {
#include "stg26/terrain/collision_14_points.inc"
};
static NJS_MODEL collision_14_model = {
#include "stg26/terrain/collision_14_model.inc"
};
static NJS_OBJECT collision_14 = {
#include "stg26/terrain/collision_14.inc"
};
static NJS_MATERIAL collision_15_materials[1] = {
#include "stg26/terrain/collision_15_materials.inc"
};
static Sint16 collision_15_faces[44] = {
#include "stg26/terrain/collision_15_faces.inc"
};
static NJS_MESHSET collision_15_meshsets[1] = {
#include "stg26/terrain/collision_15_meshsets.inc"
};
static NJS_POINT3 collision_15_points[19] = {
#include "stg26/terrain/collision_15_points.inc"
};
static NJS_MODEL collision_15_model = {
#include "stg26/terrain/collision_15_model.inc"
};
static NJS_OBJECT collision_15 = {
#include "stg26/terrain/collision_15.inc"
};
static NJS_MATERIAL collision_16_materials[1] = {
#include "stg26/terrain/collision_16_materials.inc"
};
static Sint16 collision_16_faces[25] = {
#include "stg26/terrain/collision_16_faces.inc"
};
static NJS_MESHSET collision_16_meshsets[1] = {
#include "stg26/terrain/collision_16_meshsets.inc"
};
static NJS_POINT3 collision_16_points[12] = {
#include "stg26/terrain/collision_16_points.inc"
};
static NJS_MODEL collision_16_model = {
#include "stg26/terrain/collision_16_model.inc"
};
static NJS_OBJECT collision_16 = {
#include "stg26/terrain/collision_16.inc"
};
static NJS_MATERIAL collision_17_materials[1] = {
#include "stg26/terrain/collision_17_materials.inc"
};
static Sint16 collision_17_faces[28] = {
#include "stg26/terrain/collision_17_faces.inc"
};
static NJS_MESHSET collision_17_meshsets[1] = {
#include "stg26/terrain/collision_17_meshsets.inc"
};
static NJS_POINT3 collision_17_points[12] = {
#include "stg26/terrain/collision_17_points.inc"
};
static NJS_MODEL collision_17_model = {
#include "stg26/terrain/collision_17_model.inc"
};
static NJS_OBJECT collision_17 = {
#include "stg26/terrain/collision_17.inc"
};
static NJS_MATERIAL collision_18_materials[1] = {
#include "stg26/terrain/collision_18_materials.inc"
};
static Sint16 collision_18_faces[39] = {
#include "stg26/terrain/collision_18_faces.inc"
};
static NJS_MESHSET collision_18_meshsets[1] = {
#include "stg26/terrain/collision_18_meshsets.inc"
};
static NJS_POINT3 collision_18_points[14] = {
#include "stg26/terrain/collision_18_points.inc"
};
static NJS_MODEL collision_18_model = {
#include "stg26/terrain/collision_18_model.inc"
};
static NJS_OBJECT collision_18 = {
#include "stg26/terrain/collision_18.inc"
};
static NJS_MATERIAL collision_19_materials[1] = {
#include "stg26/terrain/collision_19_materials.inc"
};
static Sint16 collision_19_faces[62] = {
#include "stg26/terrain/collision_19_faces.inc"
};
static NJS_MESHSET collision_19_meshsets[1] = {
#include "stg26/terrain/collision_19_meshsets.inc"
};
static NJS_POINT3 collision_19_points[32] = {
#include "stg26/terrain/collision_19_points.inc"
};
static NJS_MODEL collision_19_model = {
#include "stg26/terrain/collision_19_model.inc"
};
static NJS_OBJECT collision_19 = {
#include "stg26/terrain/collision_19.inc"
};
static NJS_MATERIAL collision_20_materials[1] = {
#include "stg26/terrain/collision_20_materials.inc"
};
static Sint16 collision_20_faces[25] = {
#include "stg26/terrain/collision_20_faces.inc"
};
static NJS_MESHSET collision_20_meshsets[1] = {
#include "stg26/terrain/collision_20_meshsets.inc"
};
static NJS_POINT3 collision_20_points[8] = {
#include "stg26/terrain/collision_20_points.inc"
};
static NJS_MODEL collision_20_model = {
#include "stg26/terrain/collision_20_model.inc"
};
static NJS_OBJECT collision_20 = {
#include "stg26/terrain/collision_20.inc"
};
static NJS_MATERIAL collision_21_materials[1] = {
#include "stg26/terrain/collision_21_materials.inc"
};
static Sint16 collision_21_faces[108] = {
#include "stg26/terrain/collision_21_faces.inc"
};
static NJS_MESHSET collision_21_meshsets[1] = {
#include "stg26/terrain/collision_21_meshsets.inc"
};
static NJS_POINT3 collision_21_points[44] = {
#include "stg26/terrain/collision_21_points.inc"
};
static NJS_MODEL collision_21_model = {
#include "stg26/terrain/collision_21_model.inc"
};
static NJS_OBJECT collision_21 = {
#include "stg26/terrain/collision_21.inc"
};
static NJS_MATERIAL collision_22_materials[1] = {
#include "stg26/terrain/collision_22_materials.inc"
};
static Sint16 collision_22_faces[15] = {
#include "stg26/terrain/collision_22_faces.inc"
};
static NJS_MESHSET collision_22_meshsets[1] = {
#include "stg26/terrain/collision_22_meshsets.inc"
};
static NJS_POINT3 collision_22_points[8] = {
#include "stg26/terrain/collision_22_points.inc"
};
static NJS_MODEL collision_22_model = {
#include "stg26/terrain/collision_22_model.inc"
};
static NJS_OBJECT collision_22 = {
#include "stg26/terrain/collision_22.inc"
};
static NJS_MATERIAL collision_23_materials[1] = {
#include "stg26/terrain/collision_23_materials.inc"
};
static Sint16 collision_23_faces[53] = {
#include "stg26/terrain/collision_23_faces.inc"
};
static NJS_MESHSET collision_23_meshsets[1] = {
#include "stg26/terrain/collision_23_meshsets.inc"
};
static NJS_POINT3 collision_23_points[20] = {
#include "stg26/terrain/collision_23_points.inc"
};
static NJS_MODEL collision_23_model = {
#include "stg26/terrain/collision_23_model.inc"
};
static NJS_OBJECT collision_23 = {
#include "stg26/terrain/collision_23.inc"
};
static NJS_MATERIAL collision_24_materials[1] = {
#include "stg26/terrain/collision_24_materials.inc"
};
static Sint16 collision_24_faces[28] = {
#include "stg26/terrain/collision_24_faces.inc"
};
static NJS_MESHSET collision_24_meshsets[1] = {
#include "stg26/terrain/collision_24_meshsets.inc"
};
static NJS_POINT3 collision_24_points[14] = {
#include "stg26/terrain/collision_24_points.inc"
};
static NJS_MODEL collision_24_model = {
#include "stg26/terrain/collision_24_model.inc"
};
static NJS_OBJECT collision_24 = {
#include "stg26/terrain/collision_24.inc"
};
static NJS_MATERIAL collision_25_materials[1] = {
#include "stg26/terrain/collision_25_materials.inc"
};
static Sint16 collision_25_faces[28] = {
#include "stg26/terrain/collision_25_faces.inc"
};
static NJS_MESHSET collision_25_meshsets[1] = {
#include "stg26/terrain/collision_25_meshsets.inc"
};
static NJS_POINT3 collision_25_points[14] = {
#include "stg26/terrain/collision_25_points.inc"
};
static NJS_MODEL collision_25_model = {
#include "stg26/terrain/collision_25_model.inc"
};
static NJS_OBJECT collision_25 = {
#include "stg26/terrain/collision_25.inc"
};
static NJS_MATERIAL collision_26_materials[1] = {
#include "stg26/terrain/collision_26_materials.inc"
};
static Sint16 collision_26_faces[39] = {
#include "stg26/terrain/collision_26_faces.inc"
};
static NJS_MESHSET collision_26_meshsets[1] = {
#include "stg26/terrain/collision_26_meshsets.inc"
};
static NJS_POINT3 collision_26_points[26] = {
#include "stg26/terrain/collision_26_points.inc"
};
static NJS_MODEL collision_26_model = {
#include "stg26/terrain/collision_26_model.inc"
};
static NJS_OBJECT collision_26 = {
#include "stg26/terrain/collision_26.inc"
};
static NJS_MATERIAL collision_30_materials[1] = {
#include "stg26/terrain/collision_30_materials.inc"
};
static Sint16 collision_30_faces[54] = {
#include "stg26/terrain/collision_30_faces.inc"
};
static NJS_MESHSET collision_30_meshsets[1] = {
#include "stg26/terrain/collision_30_meshsets.inc"
};
static NJS_POINT3 collision_30_points[24] = {
#include "stg26/terrain/collision_30_points.inc"
};
static NJS_MODEL collision_30_model = {
#include "stg26/terrain/collision_30_model.inc"
};
static NJS_OBJECT collision_30 = {
#include "stg26/terrain/collision_30.inc"
};
static NJS_MATERIAL collision_34_materials[1] = {
#include "stg26/terrain/collision_34_materials.inc"
};
static Sint16 collision_34_faces[16] = {
#include "stg26/terrain/collision_34_faces.inc"
};
static NJS_MESHSET collision_34_meshsets[1] = {
#include "stg26/terrain/collision_34_meshsets.inc"
};
static NJS_POINT3 collision_34_points[8] = {
#include "stg26/terrain/collision_34_points.inc"
};
static NJS_MODEL collision_34_model = {
#include "stg26/terrain/collision_34_model.inc"
};
static NJS_OBJECT collision_34 = {
#include "stg26/terrain/collision_34.inc"
};
static NJS_MATERIAL collision_36_materials[1] = {
#include "stg26/terrain/collision_36_materials.inc"
};
static Sint16 collision_36_faces[39] = {
#include "stg26/terrain/collision_36_faces.inc"
};
static NJS_MESHSET collision_36_meshsets[1] = {
#include "stg26/terrain/collision_36_meshsets.inc"
};
static NJS_POINT3 collision_36_points[14] = {
#include "stg26/terrain/collision_36_points.inc"
};
static NJS_MODEL collision_36_model = {
#include "stg26/terrain/collision_36_model.inc"
};
static NJS_OBJECT collision_36 = {
#include "stg26/terrain/collision_36.inc"
};
static NJS_MATERIAL collision_37_materials[1] = {
#include "stg26/terrain/collision_37_materials.inc"
};
static Sint16 collision_37_faces[56] = {
#include "stg26/terrain/collision_37_faces.inc"
};
static NJS_MESHSET collision_37_meshsets[1] = {
#include "stg26/terrain/collision_37_meshsets.inc"
};
static NJS_POINT3 collision_37_points[32] = {
#include "stg26/terrain/collision_37_points.inc"
};
static NJS_MODEL collision_37_model = {
#include "stg26/terrain/collision_37_model.inc"
};
static NJS_OBJECT collision_37 = {
#include "stg26/terrain/collision_37.inc"
};
static NJS_MATERIAL collision_38_materials[1] = {
#include "stg26/terrain/collision_38_materials.inc"
};
static Sint16 collision_38_faces[25] = {
#include "stg26/terrain/collision_38_faces.inc"
};
static NJS_MESHSET collision_38_meshsets[1] = {
#include "stg26/terrain/collision_38_meshsets.inc"
};
static NJS_POINT3 collision_38_points[8] = {
#include "stg26/terrain/collision_38_points.inc"
};
static NJS_MODEL collision_38_model = {
#include "stg26/terrain/collision_38_model.inc"
};
static NJS_OBJECT collision_38 = {
#include "stg26/terrain/collision_38.inc"
};
static NJS_MATERIAL collision_39_materials[1] = {
#include "stg26/terrain/collision_39_materials.inc"
};
static Sint16 collision_39_faces[105] = {
#include "stg26/terrain/collision_39_faces.inc"
};
static NJS_MESHSET collision_39_meshsets[1] = {
#include "stg26/terrain/collision_39_meshsets.inc"
};
static NJS_POINT3 collision_39_points[44] = {
#include "stg26/terrain/collision_39_points.inc"
};
static NJS_MODEL collision_39_model = {
#include "stg26/terrain/collision_39_model.inc"
};
static NJS_OBJECT collision_39 = {
#include "stg26/terrain/collision_39.inc"
};
static NJS_MATERIAL collision_40_materials[1] = {
#include "stg26/terrain/collision_40_materials.inc"
};
static Sint16 collision_40_faces[116] = {
#include "stg26/terrain/collision_40_faces.inc"
};
static NJS_MESHSET collision_40_meshsets[1] = {
#include "stg26/terrain/collision_40_meshsets.inc"
};
static NJS_POINT3 collision_40_points[42] = {
#include "stg26/terrain/collision_40_points.inc"
};
static NJS_MODEL collision_40_model = {
#include "stg26/terrain/collision_40_model.inc"
};
static NJS_OBJECT collision_40 = {
#include "stg26/terrain/collision_40.inc"
};
static NJS_MATERIAL collision_43_materials[1] = {
#include "stg26/terrain/collision_43_materials.inc"
};
static Sint16 collision_43_faces[40] = {
#include "stg26/terrain/collision_43_faces.inc"
};
static NJS_MESHSET collision_43_meshsets[1] = {
#include "stg26/terrain/collision_43_meshsets.inc"
};
static NJS_POINT3 collision_43_points[32] = {
#include "stg26/terrain/collision_43_points.inc"
};
static NJS_MODEL collision_43_model = {
#include "stg26/terrain/collision_43_model.inc"
};
static NJS_OBJECT collision_43 = {
#include "stg26/terrain/collision_43.inc"
};
static NJS_MATERIAL collision_44_materials[1] = {
#include "stg26/terrain/collision_44_materials.inc"
};
static Sint16 collision_44_faces[15] = {
#include "stg26/terrain/collision_44_faces.inc"
};
static NJS_MESHSET collision_44_meshsets[1] = {
#include "stg26/terrain/collision_44_meshsets.inc"
};
static NJS_POINT3 collision_44_points[8] = {
#include "stg26/terrain/collision_44_points.inc"
};
static NJS_MODEL collision_44_model = {
#include "stg26/terrain/collision_44_model.inc"
};
static NJS_OBJECT collision_44 = {
#include "stg26/terrain/collision_44.inc"
};
static NJS_MATERIAL collision_45_materials[1] = {
#include "stg26/terrain/collision_45_materials.inc"
};
static Sint16 collision_45_faces[51] = {
#include "stg26/terrain/collision_45_faces.inc"
};
static NJS_MESHSET collision_45_meshsets[1] = {
#include "stg26/terrain/collision_45_meshsets.inc"
};
static NJS_POINT3 collision_45_points[20] = {
#include "stg26/terrain/collision_45_points.inc"
};
static NJS_MODEL collision_45_model = {
#include "stg26/terrain/collision_45_model.inc"
};
static NJS_OBJECT collision_45 = {
#include "stg26/terrain/collision_45.inc"
};
static NJS_MATERIAL collision_46_materials[1] = {
#include "stg26/terrain/collision_46_materials.inc"
};
static Sint16 collision_46_faces[28] = {
#include "stg26/terrain/collision_46_faces.inc"
};
static NJS_MESHSET collision_46_meshsets[1] = {
#include "stg26/terrain/collision_46_meshsets.inc"
};
static NJS_POINT3 collision_46_points[12] = {
#include "stg26/terrain/collision_46_points.inc"
};
static NJS_MODEL collision_46_model = {
#include "stg26/terrain/collision_46_model.inc"
};
static NJS_OBJECT collision_46 = {
#include "stg26/terrain/collision_46.inc"
};
static NJS_MATERIAL collision_47_materials[1] = {
#include "stg26/terrain/collision_47_materials.inc"
};
static Sint16 collision_47_faces[25] = {
#include "stg26/terrain/collision_47_faces.inc"
};
static NJS_MESHSET collision_47_meshsets[1] = {
#include "stg26/terrain/collision_47_meshsets.inc"
};
static NJS_POINT3 collision_47_points[12] = {
#include "stg26/terrain/collision_47_points.inc"
};
static NJS_MODEL collision_47_model = {
#include "stg26/terrain/collision_47_model.inc"
};
static NJS_OBJECT collision_47 = {
#include "stg26/terrain/collision_47.inc"
};
static NJS_MATERIAL collision_48_materials[1] = {
#include "stg26/terrain/collision_48_materials.inc"
};
static Sint16 collision_48_faces[30] = {
#include "stg26/terrain/collision_48_faces.inc"
};
static NJS_MESHSET collision_48_meshsets[1] = {
#include "stg26/terrain/collision_48_meshsets.inc"
};
static NJS_POINT3 collision_48_points[12] = {
#include "stg26/terrain/collision_48_points.inc"
};
static NJS_MODEL collision_48_model = {
#include "stg26/terrain/collision_48_model.inc"
};
static NJS_OBJECT collision_48 = {
#include "stg26/terrain/collision_48.inc"
};
static NJS_MATERIAL collision_49_materials[1] = {
#include "stg26/terrain/collision_49_materials.inc"
};
static Sint16 collision_49_faces[10] = {
#include "stg26/terrain/collision_49_faces.inc"
};
static NJS_MESHSET collision_49_meshsets[1] = {
#include "stg26/terrain/collision_49_meshsets.inc"
};
static NJS_POINT3 collision_49_points[8] = {
#include "stg26/terrain/collision_49_points.inc"
};
static NJS_MODEL collision_49_model = {
#include "stg26/terrain/collision_49_model.inc"
};
static NJS_OBJECT collision_49 = {
#include "stg26/terrain/collision_49.inc"
};
static NJS_MATERIAL collision_51_materials[1] = {
#include "stg26/terrain/collision_51_materials.inc"
};
static Sint16 collision_51_faces[125] = {
#include "stg26/terrain/collision_51_faces.inc"
};
static NJS_MESHSET collision_51_meshsets[1] = {
#include "stg26/terrain/collision_51_meshsets.inc"
};
static NJS_POINT3 collision_51_points[46] = {
#include "stg26/terrain/collision_51_points.inc"
};
static NJS_MODEL collision_51_model = {
#include "stg26/terrain/collision_51_model.inc"
};
static NJS_OBJECT collision_51 = {
#include "stg26/terrain/collision_51.inc"
};
static NJS_MATERIAL collision_52_materials[1] = {
#include "stg26/terrain/collision_52_materials.inc"
};
static Sint16 collision_52_faces[5] = {
#include "stg26/terrain/collision_52_faces.inc"
};
static NJS_MESHSET collision_52_meshsets[1] = {
#include "stg26/terrain/collision_52_meshsets.inc"
};
static NJS_POINT3 collision_52_points[4] = {
#include "stg26/terrain/collision_52_points.inc"
};
static NJS_MODEL collision_52_model = {
#include "stg26/terrain/collision_52_model.inc"
};
static NJS_OBJECT collision_52 = {
#include "stg26/terrain/collision_52.inc"
};
static NJS_MATERIAL collision_53_materials[1] = {
#include "stg26/terrain/collision_53_materials.inc"
};
static Sint16 collision_53_faces[146] = {
#include "stg26/terrain/collision_53_faces.inc"
};
static NJS_MESHSET collision_53_meshsets[1] = {
#include "stg26/terrain/collision_53_meshsets.inc"
};
static NJS_POINT3 collision_53_points[67] = {
#include "stg26/terrain/collision_53_points.inc"
};
static NJS_MODEL collision_53_model = {
#include "stg26/terrain/collision_53_model.inc"
};
static NJS_OBJECT collision_53 = {
#include "stg26/terrain/collision_53.inc"
};
static NJS_MATERIAL collision_54_materials[1] = {
#include "stg26/terrain/collision_54_materials.inc"
};
static Sint16 collision_54_faces[46] = {
#include "stg26/terrain/collision_54_faces.inc"
};
static NJS_MESHSET collision_54_meshsets[1] = {
#include "stg26/terrain/collision_54_meshsets.inc"
};
static NJS_POINT3 collision_54_points[22] = {
#include "stg26/terrain/collision_54_points.inc"
};
static NJS_MODEL collision_54_model = {
#include "stg26/terrain/collision_54_model.inc"
};
static NJS_OBJECT collision_54 = {
#include "stg26/terrain/collision_54.inc"
};
static NJS_MATERIAL collision_55_materials[1] = {
#include "stg26/terrain/collision_55_materials.inc"
};
static Sint16 collision_55_faces[49] = {
#include "stg26/terrain/collision_55_faces.inc"
};
static NJS_MESHSET collision_55_meshsets[1] = {
#include "stg26/terrain/collision_55_meshsets.inc"
};
static NJS_POINT3 collision_55_points[22] = {
#include "stg26/terrain/collision_55_points.inc"
};
static NJS_MODEL collision_55_model = {
#include "stg26/terrain/collision_55_model.inc"
};
static NJS_OBJECT collision_55 = {
#include "stg26/terrain/collision_55.inc"
};
static NJS_MATERIAL collision_56_materials[1] = {
#include "stg26/terrain/collision_56_materials.inc"
};
static Sint16 collision_56_faces[76] = {
#include "stg26/terrain/collision_56_faces.inc"
};
static NJS_MESHSET collision_56_meshsets[1] = {
#include "stg26/terrain/collision_56_meshsets.inc"
};
static NJS_POINT3 collision_56_points[44] = {
#include "stg26/terrain/collision_56_points.inc"
};
static NJS_MODEL collision_56_model = {
#include "stg26/terrain/collision_56_model.inc"
};
static NJS_OBJECT collision_56 = {
#include "stg26/terrain/collision_56.inc"
};
static NJS_MATERIAL collision_58_materials[1] = {
#include "stg26/terrain/collision_58_materials.inc"
};
static Sint16 collision_58_faces[40] = {
#include "stg26/terrain/collision_58_faces.inc"
};
static NJS_MESHSET collision_58_meshsets[1] = {
#include "stg26/terrain/collision_58_meshsets.inc"
};
static NJS_POINT3 collision_58_points[18] = {
#include "stg26/terrain/collision_58_points.inc"
};
static NJS_MODEL collision_58_model = {
#include "stg26/terrain/collision_58_model.inc"
};
static NJS_OBJECT collision_58 = {
#include "stg26/terrain/collision_58.inc"
};
static NJS_MATERIAL collision_60_materials[1] = {
#include "stg26/terrain/collision_60_materials.inc"
};
static Sint16 collision_60_faces[12] = {
#include "stg26/terrain/collision_60_faces.inc"
};
static NJS_MESHSET collision_60_meshsets[1] = {
#include "stg26/terrain/collision_60_meshsets.inc"
};
static NJS_POINT3 collision_60_points[7] = {
#include "stg26/terrain/collision_60_points.inc"
};
static NJS_MODEL collision_60_model = {
#include "stg26/terrain/collision_60_model.inc"
};
static NJS_OBJECT collision_60 = {
#include "stg26/terrain/collision_60.inc"
};
static NJS_MATERIAL collision_68_materials[1] = {
#include "stg26/terrain/collision_68_materials.inc"
};
static Sint16 collision_68_faces[360] = {
#include "stg26/terrain/collision_68_faces.inc"
};
static NJS_MESHSET collision_68_meshsets[1] = {
#include "stg26/terrain/collision_68_meshsets.inc"
};
static NJS_POINT3 collision_68_points[131] = {
#include "stg26/terrain/collision_68_points.inc"
};
static NJS_MODEL collision_68_model = {
#include "stg26/terrain/collision_68_model.inc"
};
static NJS_OBJECT collision_68 = {
#include "stg26/terrain/collision_68.inc"
};
static NJS_MATERIAL collision_70_materials[1] = {
#include "stg26/terrain/collision_70_materials.inc"
};
static Sint16 collision_70_faces[374] = {
#include "stg26/terrain/collision_70_faces.inc"
};
static NJS_MESHSET collision_70_meshsets[1] = {
#include "stg26/terrain/collision_70_meshsets.inc"
};
static NJS_POINT3 collision_70_points[148] = {
#include "stg26/terrain/collision_70_points.inc"
};
static NJS_MODEL collision_70_model = {
#include "stg26/terrain/collision_70_model.inc"
};
static NJS_OBJECT collision_70 = {
#include "stg26/terrain/collision_70.inc"
};
static NJS_MATERIAL collision_73_materials[1] = {
#include "stg26/terrain/collision_73_materials.inc"
};
static Sint16 collision_73_faces[5] = {
#include "stg26/terrain/collision_73_faces.inc"
};
static NJS_MESHSET collision_73_meshsets[1] = {
#include "stg26/terrain/collision_73_meshsets.inc"
};
static NJS_POINT3 collision_73_points[4] = {
#include "stg26/terrain/collision_73_points.inc"
};
static NJS_MODEL collision_73_model = {
#include "stg26/terrain/collision_73_model.inc"
};
static NJS_OBJECT collision_73 = {
#include "stg26/terrain/collision_73.inc"
};
static NJS_MATERIAL collision_74_materials[1] = {
#include "stg26/terrain/collision_74_materials.inc"
};
static Sint16 collision_74_faces[5] = {
#include "stg26/terrain/collision_74_faces.inc"
};
static NJS_MESHSET collision_74_meshsets[1] = {
#include "stg26/terrain/collision_74_meshsets.inc"
};
static NJS_POINT3 collision_74_points[4] = {
#include "stg26/terrain/collision_74_points.inc"
};
static NJS_MODEL collision_74_model = {
#include "stg26/terrain/collision_74_model.inc"
};
static NJS_OBJECT collision_74 = {
#include "stg26/terrain/collision_74.inc"
};
static NJS_MATERIAL collision_75_materials[1] = {
#include "stg26/terrain/collision_75_materials.inc"
};
static Sint16 collision_75_faces[5] = {
#include "stg26/terrain/collision_75_faces.inc"
};
static NJS_MESHSET collision_75_meshsets[1] = {
#include "stg26/terrain/collision_75_meshsets.inc"
};
static NJS_POINT3 collision_75_points[4] = {
#include "stg26/terrain/collision_75_points.inc"
};
static NJS_MODEL collision_75_model = {
#include "stg26/terrain/collision_75_model.inc"
};
static NJS_OBJECT collision_75 = {
#include "stg26/terrain/collision_75.inc"
};
static NJS_MATERIAL collision_76_materials[1] = {
#include "stg26/terrain/collision_76_materials.inc"
};
static Sint16 collision_76_faces[44] = {
#include "stg26/terrain/collision_76_faces.inc"
};
static NJS_MESHSET collision_76_meshsets[1] = {
#include "stg26/terrain/collision_76_meshsets.inc"
};
static NJS_POINT3 collision_76_points[20] = {
#include "stg26/terrain/collision_76_points.inc"
};
static NJS_MODEL collision_76_model = {
#include "stg26/terrain/collision_76_model.inc"
};
static NJS_OBJECT collision_76 = {
#include "stg26/terrain/collision_76.inc"
};
static NJS_MATERIAL collision_78_materials[1] = {
#include "stg26/terrain/collision_78_materials.inc"
};
static Sint16 collision_78_faces[15] = {
#include "stg26/terrain/collision_78_faces.inc"
};
static NJS_MESHSET collision_78_meshsets[1] = {
#include "stg26/terrain/collision_78_meshsets.inc"
};
static NJS_POINT3 collision_78_points[12] = {
#include "stg26/terrain/collision_78_points.inc"
};
static NJS_MODEL collision_78_model = {
#include "stg26/terrain/collision_78_model.inc"
};
static NJS_OBJECT collision_78 = {
#include "stg26/terrain/collision_78.inc"
};
static NJS_MATERIAL collision_79_materials[1] = {
#include "stg26/terrain/collision_79_materials.inc"
};
static Sint16 collision_79_faces[55] = {
#include "stg26/terrain/collision_79_faces.inc"
};
static NJS_MESHSET collision_79_meshsets[1] = {
#include "stg26/terrain/collision_79_meshsets.inc"
};
static NJS_POINT3 collision_79_points[24] = {
#include "stg26/terrain/collision_79_points.inc"
};
static NJS_MODEL collision_79_model = {
#include "stg26/terrain/collision_79_model.inc"
};
static NJS_OBJECT collision_79 = {
#include "stg26/terrain/collision_79.inc"
};
static NJS_MATERIAL collision_80_materials[1] = {
#include "stg26/terrain/collision_80_materials.inc"
};
static Sint16 collision_80_faces[39] = {
#include "stg26/terrain/collision_80_faces.inc"
};
static NJS_MESHSET collision_80_meshsets[1] = {
#include "stg26/terrain/collision_80_meshsets.inc"
};
static NJS_POINT3 collision_80_points[15] = {
#include "stg26/terrain/collision_80_points.inc"
};
static NJS_MODEL collision_80_model = {
#include "stg26/terrain/collision_80_model.inc"
};
static NJS_OBJECT collision_80 = {
#include "stg26/terrain/collision_80.inc"
};
static NJS_MATERIAL collision_81_materials[1] = {
#include "stg26/terrain/collision_81_materials.inc"
};
static Sint16 collision_81_faces[50] = {
#include "stg26/terrain/collision_81_faces.inc"
};
static NJS_MESHSET collision_81_meshsets[1] = {
#include "stg26/terrain/collision_81_meshsets.inc"
};
static NJS_POINT3 collision_81_points[22] = {
#include "stg26/terrain/collision_81_points.inc"
};
static NJS_MODEL collision_81_model = {
#include "stg26/terrain/collision_81_model.inc"
};
static NJS_OBJECT collision_81 = {
#include "stg26/terrain/collision_81.inc"
};
static NJS_MATERIAL collision_82_materials[1] = {
#include "stg26/terrain/collision_82_materials.inc"
};
static Sint16 collision_82_faces[32] = {
#include "stg26/terrain/collision_82_faces.inc"
};
static NJS_MESHSET collision_82_meshsets[1] = {
#include "stg26/terrain/collision_82_meshsets.inc"
};
static NJS_POINT3 collision_82_points[12] = {
#include "stg26/terrain/collision_82_points.inc"
};
static NJS_MODEL collision_82_model = {
#include "stg26/terrain/collision_82_model.inc"
};
static NJS_OBJECT collision_82 = {
#include "stg26/terrain/collision_82.inc"
};
static NJS_MATERIAL collision_83_materials[1] = {
#include "stg26/terrain/collision_83_materials.inc"
};
static Sint16 collision_83_faces[32] = {
#include "stg26/terrain/collision_83_faces.inc"
};
static NJS_MESHSET collision_83_meshsets[1] = {
#include "stg26/terrain/collision_83_meshsets.inc"
};
static NJS_POINT3 collision_83_points[12] = {
#include "stg26/terrain/collision_83_points.inc"
};
static NJS_MODEL collision_83_model = {
#include "stg26/terrain/collision_83_model.inc"
};
static NJS_OBJECT collision_83 = {
#include "stg26/terrain/collision_83.inc"
};
static NJS_MATERIAL collision_84_materials[1] = {
#include "stg26/terrain/collision_84_materials.inc"
};
static Sint16 collision_84_faces[38] = {
#include "stg26/terrain/collision_84_faces.inc"
};
static NJS_MESHSET collision_84_meshsets[1] = {
#include "stg26/terrain/collision_84_meshsets.inc"
};
static NJS_POINT3 collision_84_points[15] = {
#include "stg26/terrain/collision_84_points.inc"
};
static NJS_MODEL collision_84_model = {
#include "stg26/terrain/collision_84_model.inc"
};
static NJS_OBJECT collision_84 = {
#include "stg26/terrain/collision_84.inc"
};
static NJS_MATERIAL collision_85_materials[1] = {
#include "stg26/terrain/collision_85_materials.inc"
};
static Sint16 collision_85_faces[93] = {
#include "stg26/terrain/collision_85_faces.inc"
};
static NJS_MESHSET collision_85_meshsets[1] = {
#include "stg26/terrain/collision_85_meshsets.inc"
};
static NJS_POINT3 collision_85_points[56] = {
#include "stg26/terrain/collision_85_points.inc"
};
static NJS_MODEL collision_85_model = {
#include "stg26/terrain/collision_85_model.inc"
};
static NJS_OBJECT collision_85 = {
#include "stg26/terrain/collision_85.inc"
};
static NJS_MATERIAL collision_86_materials[1] = {
#include "stg26/terrain/collision_86_materials.inc"
};
static Sint16 collision_86_faces[27] = {
#include "stg26/terrain/collision_86_faces.inc"
};
static NJS_MESHSET collision_86_meshsets[1] = {
#include "stg26/terrain/collision_86_meshsets.inc"
};
static NJS_POINT3 collision_86_points[12] = {
#include "stg26/terrain/collision_86_points.inc"
};
static NJS_MODEL collision_86_model = {
#include "stg26/terrain/collision_86_model.inc"
};
static NJS_OBJECT collision_86 = {
#include "stg26/terrain/collision_86.inc"
};
static NJS_MATERIAL collision_98_materials[1] = {
#include "stg26/terrain/collision_98_materials.inc"
};
static Sint16 collision_98_faces[30] = {
#include "stg26/terrain/collision_98_faces.inc"
};
static NJS_MESHSET collision_98_meshsets[1] = {
#include "stg26/terrain/collision_98_meshsets.inc"
};
static NJS_POINT3 collision_98_points[24] = {
#include "stg26/terrain/collision_98_points.inc"
};
static NJS_MODEL collision_98_model = {
#include "stg26/terrain/collision_98_model.inc"
};
static NJS_OBJECT collision_98 = {
#include "stg26/terrain/collision_98.inc"
};
static NJS_MATERIAL collision_102_materials[1] = {
#include "stg26/terrain/collision_102_materials.inc"
};
static Sint16 collision_102_faces[22] = {
#include "stg26/terrain/collision_102_faces.inc"
};
static NJS_MESHSET collision_102_meshsets[1] = {
#include "stg26/terrain/collision_102_meshsets.inc"
};
static NJS_POINT3 collision_102_points[12] = {
#include "stg26/terrain/collision_102_points.inc"
};
static NJS_MODEL collision_102_model = {
#include "stg26/terrain/collision_102_model.inc"
};
static NJS_OBJECT collision_102 = {
#include "stg26/terrain/collision_102.inc"
};
static NJS_MATERIAL collision_103_materials[1] = {
#include "stg26/terrain/collision_103_materials.inc"
};
static Sint16 collision_103_faces[32] = {
#include "stg26/terrain/collision_103_faces.inc"
};
static NJS_MESHSET collision_103_meshsets[1] = {
#include "stg26/terrain/collision_103_meshsets.inc"
};
static NJS_POINT3 collision_103_points[12] = {
#include "stg26/terrain/collision_103_points.inc"
};
static NJS_MODEL collision_103_model = {
#include "stg26/terrain/collision_103_model.inc"
};
static NJS_OBJECT collision_103 = {
#include "stg26/terrain/collision_103.inc"
};
static NJS_MATERIAL collision_104_materials[1] = {
#include "stg26/terrain/collision_104_materials.inc"
};
static Sint16 collision_104_faces[32] = {
#include "stg26/terrain/collision_104_faces.inc"
};
static NJS_MESHSET collision_104_meshsets[1] = {
#include "stg26/terrain/collision_104_meshsets.inc"
};
static NJS_POINT3 collision_104_points[12] = {
#include "stg26/terrain/collision_104_points.inc"
};
static NJS_MODEL collision_104_model = {
#include "stg26/terrain/collision_104_model.inc"
};
static NJS_OBJECT collision_104 = {
#include "stg26/terrain/collision_104.inc"
};
static NJS_MATERIAL collision_105_materials[1] = {
#include "stg26/terrain/collision_105_materials.inc"
};
static Sint16 collision_105_faces[32] = {
#include "stg26/terrain/collision_105_faces.inc"
};
static NJS_MESHSET collision_105_meshsets[1] = {
#include "stg26/terrain/collision_105_meshsets.inc"
};
static NJS_POINT3 collision_105_points[12] = {
#include "stg26/terrain/collision_105_points.inc"
};
static NJS_MODEL collision_105_model = {
#include "stg26/terrain/collision_105_model.inc"
};
static NJS_OBJECT collision_105 = {
#include "stg26/terrain/collision_105.inc"
};
static NJS_MATERIAL collision_106_materials[1] = {
#include "stg26/terrain/collision_106_materials.inc"
};
static Sint16 collision_106_faces[150] = {
#include "stg26/terrain/collision_106_faces.inc"
};
static NJS_MESHSET collision_106_meshsets[1] = {
#include "stg26/terrain/collision_106_meshsets.inc"
};
static NJS_POINT3 collision_106_points[68] = {
#include "stg26/terrain/collision_106_points.inc"
};
static NJS_MODEL collision_106_model = {
#include "stg26/terrain/collision_106_model.inc"
};
static NJS_OBJECT collision_106 = {
#include "stg26/terrain/collision_106.inc"
};
static NJS_MATERIAL collision_110_materials[1] = {
#include "stg26/terrain/collision_110_materials.inc"
};
static Sint16 collision_110_faces[9] = {
#include "stg26/terrain/collision_110_faces.inc"
};
static NJS_MESHSET collision_110_meshsets[1] = {
#include "stg26/terrain/collision_110_meshsets.inc"
};
static NJS_POINT3 collision_110_points[8] = {
#include "stg26/terrain/collision_110_points.inc"
};
static NJS_MODEL collision_110_model = {
#include "stg26/terrain/collision_110_model.inc"
};
static NJS_OBJECT collision_110 = {
#include "stg26/terrain/collision_110.inc"
};
static NJS_MATERIAL collision_112_materials[1] = {
#include "stg26/terrain/collision_112_materials.inc"
};
static Sint16 collision_112_faces[18] = {
#include "stg26/terrain/collision_112_faces.inc"
};
static NJS_MESHSET collision_112_meshsets[1] = {
#include "stg26/terrain/collision_112_meshsets.inc"
};
static NJS_POINT3 collision_112_points[10] = {
#include "stg26/terrain/collision_112_points.inc"
};
static NJS_MODEL collision_112_model = {
#include "stg26/terrain/collision_112_model.inc"
};
static NJS_OBJECT collision_112 = {
#include "stg26/terrain/collision_112.inc"
};
static NJS_MATERIAL collision_113_materials[1] = {
#include "stg26/terrain/collision_113_materials.inc"
};
static Sint16 collision_113_faces[22] = {
#include "stg26/terrain/collision_113_faces.inc"
};
static NJS_MESHSET collision_113_meshsets[1] = {
#include "stg26/terrain/collision_113_meshsets.inc"
};
static NJS_POINT3 collision_113_points[12] = {
#include "stg26/terrain/collision_113_points.inc"
};
static NJS_MODEL collision_113_model = {
#include "stg26/terrain/collision_113_model.inc"
};
static NJS_OBJECT collision_113 = {
#include "stg26/terrain/collision_113.inc"
};
static NJS_MATERIAL collision_114_materials[1] = {
#include "stg26/terrain/collision_114_materials.inc"
};
static Sint16 collision_114_faces[16] = {
#include "stg26/terrain/collision_114_faces.inc"
};
static NJS_MESHSET collision_114_meshsets[1] = {
#include "stg26/terrain/collision_114_meshsets.inc"
};
static NJS_POINT3 collision_114_points[8] = {
#include "stg26/terrain/collision_114_points.inc"
};
static NJS_MODEL collision_114_model = {
#include "stg26/terrain/collision_114_model.inc"
};
static NJS_OBJECT collision_114 = {
#include "stg26/terrain/collision_114.inc"
};
static NJS_MATERIAL collision_115_materials[1] = {
#include "stg26/terrain/collision_115_materials.inc"
};
static Sint16 collision_115_faces[22] = {
#include "stg26/terrain/collision_115_faces.inc"
};
static NJS_MESHSET collision_115_meshsets[1] = {
#include "stg26/terrain/collision_115_meshsets.inc"
};
static NJS_POINT3 collision_115_points[12] = {
#include "stg26/terrain/collision_115_points.inc"
};
static NJS_MODEL collision_115_model = {
#include "stg26/terrain/collision_115_model.inc"
};
static NJS_OBJECT collision_115 = {
#include "stg26/terrain/collision_115.inc"
};
static NJS_MATERIAL collision_116_materials[1] = {
#include "stg26/terrain/collision_116_materials.inc"
};
static Sint16 collision_116_faces[27] = {
#include "stg26/terrain/collision_116_faces.inc"
};
static NJS_MESHSET collision_116_meshsets[1] = {
#include "stg26/terrain/collision_116_meshsets.inc"
};
static NJS_POINT3 collision_116_points[12] = {
#include "stg26/terrain/collision_116_points.inc"
};
static NJS_MODEL collision_116_model = {
#include "stg26/terrain/collision_116_model.inc"
};
static NJS_OBJECT collision_116 = {
#include "stg26/terrain/collision_116.inc"
};
static NJS_MATERIAL collision_117_materials[1] = {
#include "stg26/terrain/collision_117_materials.inc"
};
static Sint16 collision_117_faces[30] = {
#include "stg26/terrain/collision_117_faces.inc"
};
static NJS_MESHSET collision_117_meshsets[1] = {
#include "stg26/terrain/collision_117_meshsets.inc"
};
static NJS_POINT3 collision_117_points[18] = {
#include "stg26/terrain/collision_117_points.inc"
};
static NJS_MODEL collision_117_model = {
#include "stg26/terrain/collision_117_model.inc"
};
static NJS_OBJECT collision_117 = {
#include "stg26/terrain/collision_117.inc"
};
static NJS_MATERIAL collision_118_materials[1] = {
#include "stg26/terrain/collision_118_materials.inc"
};
static Sint16 collision_118_faces[37] = {
#include "stg26/terrain/collision_118_faces.inc"
};
static NJS_MESHSET collision_118_meshsets[1] = {
#include "stg26/terrain/collision_118_meshsets.inc"
};
static NJS_POINT3 collision_118_points[15] = {
#include "stg26/terrain/collision_118_points.inc"
};
static NJS_MODEL collision_118_model = {
#include "stg26/terrain/collision_118_model.inc"
};
static NJS_OBJECT collision_118 = {
#include "stg26/terrain/collision_118.inc"
};
static NJS_MATERIAL collision_119_materials[1] = {
#include "stg26/terrain/collision_119_materials.inc"
};
static Sint16 collision_119_faces[40] = {
#include "stg26/terrain/collision_119_faces.inc"
};
static NJS_MESHSET collision_119_meshsets[1] = {
#include "stg26/terrain/collision_119_meshsets.inc"
};
static NJS_POINT3 collision_119_points[17] = {
#include "stg26/terrain/collision_119_points.inc"
};
static NJS_MODEL collision_119_model = {
#include "stg26/terrain/collision_119_model.inc"
};
static NJS_OBJECT collision_119 = {
#include "stg26/terrain/collision_119.inc"
};
static NJS_MATERIAL collision_120_materials[1] = {
#include "stg26/terrain/collision_120_materials.inc"
};
static Sint16 collision_120_faces[18] = {
#include "stg26/terrain/collision_120_faces.inc"
};
static NJS_MESHSET collision_120_meshsets[1] = {
#include "stg26/terrain/collision_120_meshsets.inc"
};
static NJS_POINT3 collision_120_points[12] = {
#include "stg26/terrain/collision_120_points.inc"
};
static NJS_MODEL collision_120_model = {
#include "stg26/terrain/collision_120_model.inc"
};
static NJS_OBJECT collision_120 = {
#include "stg26/terrain/collision_120.inc"
};
static NJS_MATERIAL collision_124_materials[1] = {
#include "stg26/terrain/collision_124_materials.inc"
};
static Sint16 collision_124_faces[96] = {
#include "stg26/terrain/collision_124_faces.inc"
};
static NJS_MESHSET collision_124_meshsets[1] = {
#include "stg26/terrain/collision_124_meshsets.inc"
};
static NJS_POINT3 collision_124_points[56] = {
#include "stg26/terrain/collision_124_points.inc"
};
static NJS_MODEL collision_124_model = {
#include "stg26/terrain/collision_124_model.inc"
};
static NJS_OBJECT collision_124 = {
#include "stg26/terrain/collision_124.inc"
};
static NJS_MATERIAL collision_125_materials[1] = {
#include "stg26/terrain/collision_125_materials.inc"
};
static Sint16 collision_125_faces[21] = {
#include "stg26/terrain/collision_125_faces.inc"
};
static NJS_MESHSET collision_125_meshsets[1] = {
#include "stg26/terrain/collision_125_meshsets.inc"
};
static NJS_POINT3 collision_125_points[12] = {
#include "stg26/terrain/collision_125_points.inc"
};
static NJS_MODEL collision_125_model = {
#include "stg26/terrain/collision_125_model.inc"
};
static NJS_OBJECT collision_125 = {
#include "stg26/terrain/collision_125.inc"
};
static NJS_MATERIAL collision_129_materials[1] = {
#include "stg26/terrain/collision_129_materials.inc"
};
static Sint16 collision_129_faces[357] = {
#include "stg26/terrain/collision_129_faces.inc"
};
static NJS_MESHSET collision_129_meshsets[1] = {
#include "stg26/terrain/collision_129_meshsets.inc"
};
static NJS_POINT3 collision_129_points[131] = {
#include "stg26/terrain/collision_129_points.inc"
};
static NJS_MODEL collision_129_model = {
#include "stg26/terrain/collision_129_model.inc"
};
static NJS_OBJECT collision_129 = {
#include "stg26/terrain/collision_129.inc"
};
static NJS_MATERIAL collision_130_materials[1] = {
#include "stg26/terrain/collision_130_materials.inc"
};
static Sint16 collision_130_faces[32] = {
#include "stg26/terrain/collision_130_faces.inc"
};
static NJS_MESHSET collision_130_meshsets[1] = {
#include "stg26/terrain/collision_130_meshsets.inc"
};
static NJS_POINT3 collision_130_points[12] = {
#include "stg26/terrain/collision_130_points.inc"
};
static NJS_MODEL collision_130_model = {
#include "stg26/terrain/collision_130_model.inc"
};
static NJS_OBJECT collision_130 = {
#include "stg26/terrain/collision_130.inc"
};
static NJS_MATERIAL collision_131_materials[1] = {
#include "stg26/terrain/collision_131_materials.inc"
};
static Sint16 collision_131_faces[32] = {
#include "stg26/terrain/collision_131_faces.inc"
};
static NJS_MESHSET collision_131_meshsets[1] = {
#include "stg26/terrain/collision_131_meshsets.inc"
};
static NJS_POINT3 collision_131_points[12] = {
#include "stg26/terrain/collision_131_points.inc"
};
static NJS_MODEL collision_131_model = {
#include "stg26/terrain/collision_131_model.inc"
};
static NJS_OBJECT collision_131 = {
#include "stg26/terrain/collision_131.inc"
};
static NJS_MATERIAL collision_132_materials[1] = {
#include "stg26/terrain/collision_132_materials.inc"
};
static Sint16 collision_132_faces[32] = {
#include "stg26/terrain/collision_132_faces.inc"
};
static NJS_MESHSET collision_132_meshsets[1] = {
#include "stg26/terrain/collision_132_meshsets.inc"
};
static NJS_POINT3 collision_132_points[12] = {
#include "stg26/terrain/collision_132_points.inc"
};
static NJS_MODEL collision_132_model = {
#include "stg26/terrain/collision_132_model.inc"
};
static NJS_OBJECT collision_132 = {
#include "stg26/terrain/collision_132.inc"
};
static NJS_MATERIAL collision_133_materials[1] = {
#include "stg26/terrain/collision_133_materials.inc"
};
static Sint16 collision_133_faces[82] = {
#include "stg26/terrain/collision_133_faces.inc"
};
static NJS_MESHSET collision_133_meshsets[1] = {
#include "stg26/terrain/collision_133_meshsets.inc"
};
static NJS_POINT3 collision_133_points[32] = {
#include "stg26/terrain/collision_133_points.inc"
};
static NJS_MODEL collision_133_model = {
#include "stg26/terrain/collision_133_model.inc"
};
static NJS_OBJECT collision_133 = {
#include "stg26/terrain/collision_133.inc"
};
static NJS_MATERIAL collision_134_materials[1] = {
#include "stg26/terrain/collision_134_materials.inc"
};
static Sint16 collision_134_faces[8] = {
#include "stg26/terrain/collision_134_faces.inc"
};
static NJS_MESHSET collision_134_meshsets[1] = {
#include "stg26/terrain/collision_134_meshsets.inc"
};
static NJS_POINT3 collision_134_points[5] = {
#include "stg26/terrain/collision_134_points.inc"
};
static NJS_MODEL collision_134_model = {
#include "stg26/terrain/collision_134_model.inc"
};
static NJS_OBJECT collision_134 = {
#include "stg26/terrain/collision_134.inc"
};
static NJS_MATERIAL collision_135_materials[1] = {
#include "stg26/terrain/collision_135_materials.inc"
};
static Sint16 collision_135_faces[8] = {
#include "stg26/terrain/collision_135_faces.inc"
};
static NJS_MESHSET collision_135_meshsets[1] = {
#include "stg26/terrain/collision_135_meshsets.inc"
};
static NJS_POINT3 collision_135_points[5] = {
#include "stg26/terrain/collision_135_points.inc"
};
static NJS_MODEL collision_135_model = {
#include "stg26/terrain/collision_135_model.inc"
};
static NJS_OBJECT collision_135 = {
#include "stg26/terrain/collision_135.inc"
};
static NJS_MATERIAL collision_136_materials[1] = {
#include "stg26/terrain/collision_136_materials.inc"
};
static Sint16 collision_136_faces[14] = {
#include "stg26/terrain/collision_136_faces.inc"
};
static NJS_MESHSET collision_136_meshsets[1] = {
#include "stg26/terrain/collision_136_meshsets.inc"
};
static NJS_POINT3 collision_136_points[8] = {
#include "stg26/terrain/collision_136_points.inc"
};
static NJS_MODEL collision_136_model = {
#include "stg26/terrain/collision_136_model.inc"
};
static NJS_OBJECT collision_136 = {
#include "stg26/terrain/collision_136.inc"
};
static NJS_MATERIAL collision_137_materials[1] = {
#include "stg26/terrain/collision_137_materials.inc"
};
static Sint16 collision_137_faces[107] = {
#include "stg26/terrain/collision_137_faces.inc"
};
static NJS_MESHSET collision_137_meshsets[1] = {
#include "stg26/terrain/collision_137_meshsets.inc"
};
static NJS_POINT3 collision_137_points[34] = {
#include "stg26/terrain/collision_137_points.inc"
};
static NJS_MODEL collision_137_model = {
#include "stg26/terrain/collision_137_model.inc"
};
static NJS_OBJECT collision_137 = {
#include "stg26/terrain/collision_137.inc"
};
static NJS_MATERIAL collision_139_materials[1] = {
#include "stg26/terrain/collision_139_materials.inc"
};
static Sint16 collision_139_faces[49] = {
#include "stg26/terrain/collision_139_faces.inc"
};
static NJS_MESHSET collision_139_meshsets[1] = {
#include "stg26/terrain/collision_139_meshsets.inc"
};
static NJS_POINT3 collision_139_points[20] = {
#include "stg26/terrain/collision_139_points.inc"
};
static NJS_MODEL collision_139_model = {
#include "stg26/terrain/collision_139_model.inc"
};
static NJS_OBJECT collision_139 = {
#include "stg26/terrain/collision_139.inc"
};
static NJS_MATERIAL collision_140_materials[1] = {
#include "stg26/terrain/collision_140_materials.inc"
};
static Sint16 collision_140_faces[70] = {
#include "stg26/terrain/collision_140_faces.inc"
};
static NJS_MESHSET collision_140_meshsets[1] = {
#include "stg26/terrain/collision_140_meshsets.inc"
};
static NJS_POINT3 collision_140_points[24] = {
#include "stg26/terrain/collision_140_points.inc"
};
static NJS_MODEL collision_140_model = {
#include "stg26/terrain/collision_140_model.inc"
};
static NJS_OBJECT collision_140 = {
#include "stg26/terrain/collision_140.inc"
};
static NJS_MATERIAL collision_148_materials[1] = {
#include "stg26/terrain/collision_148_materials.inc"
};
static Sint16 collision_148_faces[279] = {
#include "stg26/terrain/collision_148_faces.inc"
};
static NJS_MESHSET collision_148_meshsets[1] = {
#include "stg26/terrain/collision_148_meshsets.inc"
};
static NJS_POINT3 collision_148_points[101] = {
#include "stg26/terrain/collision_148_points.inc"
};
static NJS_MODEL collision_148_model = {
#include "stg26/terrain/collision_148_model.inc"
};
static NJS_OBJECT collision_148 = {
#include "stg26/terrain/collision_148.inc"
};
static NJS_MATERIAL collision_149_materials[1] = {
#include "stg26/terrain/collision_149_materials.inc"
};
static Sint16 collision_149_faces[151] = {
#include "stg26/terrain/collision_149_faces.inc"
};
static NJS_MESHSET collision_149_meshsets[1] = {
#include "stg26/terrain/collision_149_meshsets.inc"
};
static NJS_POINT3 collision_149_points[54] = {
#include "stg26/terrain/collision_149_points.inc"
};
static NJS_MODEL collision_149_model = {
#include "stg26/terrain/collision_149_model.inc"
};
static NJS_OBJECT collision_149 = {
#include "stg26/terrain/collision_149.inc"
};
static NJS_MATERIAL collision_150_materials[1] = {
#include "stg26/terrain/collision_150_materials.inc"
};
static Sint16 collision_150_faces[23] = {
#include "stg26/terrain/collision_150_faces.inc"
};
static NJS_MESHSET collision_150_meshsets[1] = {
#include "stg26/terrain/collision_150_meshsets.inc"
};
static NJS_POINT3 collision_150_points[16] = {
#include "stg26/terrain/collision_150_points.inc"
};
static NJS_MODEL collision_150_model = {
#include "stg26/terrain/collision_150_model.inc"
};
static NJS_OBJECT collision_150 = {
#include "stg26/terrain/collision_150.inc"
};
static NJS_MATERIAL collision_152_materials[1] = {
#include "stg26/terrain/collision_152_materials.inc"
};
static Sint16 collision_152_faces[15] = {
#include "stg26/terrain/collision_152_faces.inc"
};
static NJS_MESHSET collision_152_meshsets[1] = {
#include "stg26/terrain/collision_152_meshsets.inc"
};
static NJS_POINT3 collision_152_points[12] = {
#include "stg26/terrain/collision_152_points.inc"
};
static NJS_MODEL collision_152_model = {
#include "stg26/terrain/collision_152_model.inc"
};
static NJS_OBJECT collision_152 = {
#include "stg26/terrain/collision_152.inc"
};
static NJS_MATERIAL collision_156_materials[1] = {
#include "stg26/terrain/collision_156_materials.inc"
};
static Sint16 collision_156_faces[5] = {
#include "stg26/terrain/collision_156_faces.inc"
};
static NJS_MESHSET collision_156_meshsets[1] = {
#include "stg26/terrain/collision_156_meshsets.inc"
};
static NJS_POINT3 collision_156_points[4] = {
#include "stg26/terrain/collision_156_points.inc"
};
static NJS_MODEL collision_156_model = {
#include "stg26/terrain/collision_156_model.inc"
};
static NJS_OBJECT collision_156 = {
#include "stg26/terrain/collision_156.inc"
};
static NJS_MATERIAL collision_158_materials[1] = {
#include "stg26/terrain/collision_158_materials.inc"
};
static Sint16 collision_158_faces[5] = {
#include "stg26/terrain/collision_158_faces.inc"
};
static NJS_MESHSET collision_158_meshsets[1] = {
#include "stg26/terrain/collision_158_meshsets.inc"
};
static NJS_POINT3 collision_158_points[4] = {
#include "stg26/terrain/collision_158_points.inc"
};
static NJS_MODEL collision_158_model = {
#include "stg26/terrain/collision_158_model.inc"
};
static NJS_OBJECT collision_158 = {
#include "stg26/terrain/collision_158.inc"
};
static NJS_MATERIAL collision_159_materials[1] = {
#include "stg26/terrain/collision_159_materials.inc"
};
static Sint16 collision_159_faces[5] = {
#include "stg26/terrain/collision_159_faces.inc"
};
static NJS_MESHSET collision_159_meshsets[1] = {
#include "stg26/terrain/collision_159_meshsets.inc"
};
static NJS_POINT3 collision_159_points[4] = {
#include "stg26/terrain/collision_159_points.inc"
};
static NJS_MODEL collision_159_model = {
#include "stg26/terrain/collision_159_model.inc"
};
static NJS_OBJECT collision_159 = {
#include "stg26/terrain/collision_159.inc"
};
static NJS_CNK_OBJECT draw_191 = {
#include "stg26/terrain/draw_191.inc"
};
static NJS_CNK_OBJECT draw_188 = {
#include "stg26/terrain/draw_188.inc"
};
static NJS_CNK_OBJECT draw_183 = {
#include "stg26/terrain/draw_183.inc"
};
static NJS_CNK_OBJECT draw_181 = {
#include "stg26/terrain/draw_181.inc"
};
static NJS_CNK_OBJECT draw_163 = {
#include "stg26/terrain/draw_163.inc"
};
static NJS_CNK_OBJECT draw_160 = {
#include "stg26/terrain/draw_160.inc"
};
static NJS_CNK_OBJECT draw_156 = {
#include "stg26/terrain/draw_156.inc"
};
static NJS_CNK_OBJECT draw_154 = {
#include "stg26/terrain/draw_154.inc"
};
static NJS_CNK_OBJECT draw_152 = {
#include "stg26/terrain/draw_152.inc"
};
static NJS_CNK_OBJECT draw_150 = {
#include "stg26/terrain/draw_150.inc"
};
static NJS_CNK_OBJECT draw_131 = {
#include "stg26/terrain/draw_131.inc"
};
static NJS_CNK_OBJECT draw_129 = {
#include "stg26/terrain/draw_129.inc"
};
static NJS_CNK_OBJECT draw_127 = {
#include "stg26/terrain/draw_127.inc"
};
static NJS_CNK_OBJECT draw_122 = {
#include "stg26/terrain/draw_122.inc"
};
static NJS_CNK_OBJECT draw_121 = {
#include "stg26/terrain/draw_121.inc"
};
static NJS_CNK_OBJECT draw_120 = {
#include "stg26/terrain/draw_120.inc"
};
static NJS_CNK_OBJECT draw_119 = {
#include "stg26/terrain/draw_119.inc"
};
static NJS_CNK_OBJECT draw_118 = {
#include "stg26/terrain/draw_118.inc"
};
static NJS_CNK_OBJECT draw_117 = {
#include "stg26/terrain/draw_117.inc"
};
static NJS_CNK_OBJECT draw_116 = {
#include "stg26/terrain/draw_116.inc"
};
static NJS_CNK_OBJECT draw_108 = {
#include "stg26/terrain/draw_108.inc"
};
static NJS_CNK_OBJECT draw_106 = {
#include "stg26/terrain/draw_106.inc"
};
static NJS_CNK_OBJECT draw_104 = {
#include "stg26/terrain/draw_104.inc"
};
static NJS_CNK_OBJECT draw_102 = {
#include "stg26/terrain/draw_102.inc"
};
static NJS_CNK_OBJECT draw_101 = {
#include "stg26/terrain/draw_101.inc"
};
static NJS_CNK_OBJECT draw_100 = {
#include "stg26/terrain/draw_100.inc"
};
static NJS_CNK_OBJECT draw_99 = {
#include "stg26/terrain/draw_99.inc"
};
static NJS_CNK_OBJECT draw_98 = {
#include "stg26/terrain/draw_98.inc"
};
static NJS_CNK_OBJECT draw_97 = {
#include "stg26/terrain/draw_97.inc"
};
static NJS_CNK_OBJECT draw_96 = {
#include "stg26/terrain/draw_96.inc"
};
static NJS_CNK_OBJECT draw_95 = {
#include "stg26/terrain/draw_95.inc"
};
static NJS_CNK_OBJECT draw_94 = {
#include "stg26/terrain/draw_94.inc"
};
static NJS_CNK_OBJECT draw_93 = {
#include "stg26/terrain/draw_93.inc"
};
static NJS_CNK_OBJECT draw_92 = {
#include "stg26/terrain/draw_92.inc"
};
static NJS_CNK_OBJECT draw_91 = {
#include "stg26/terrain/draw_91.inc"
};
static NJS_CNK_OBJECT draw_90 = {
#include "stg26/terrain/draw_90.inc"
};
static NJS_CNK_OBJECT draw_89 = {
#include "stg26/terrain/draw_89.inc"
};
static NJS_CNK_OBJECT draw_88 = {
#include "stg26/terrain/draw_88.inc"
};
static NJS_CNK_OBJECT draw_87 = {
#include "stg26/terrain/draw_87.inc"
};
static NJS_CNK_OBJECT draw_86 = {
#include "stg26/terrain/draw_86.inc"
};
static NJS_CNK_OBJECT draw_85 = {
#include "stg26/terrain/draw_85.inc"
};
static NJS_CNK_OBJECT draw_84 = {
#include "stg26/terrain/draw_84.inc"
};
static NJS_CNK_OBJECT draw_83 = {
#include "stg26/terrain/draw_83.inc"
};
static NJS_CNK_OBJECT draw_82 = {
#include "stg26/terrain/draw_82.inc"
};
static NJS_CNK_OBJECT draw_81 = {
#include "stg26/terrain/draw_81.inc"
};
static NJS_CNK_OBJECT draw_80 = {
#include "stg26/terrain/draw_80.inc"
};
static NJS_CNK_OBJECT draw_79 = {
#include "stg26/terrain/draw_79.inc"
};
static NJS_CNK_OBJECT draw_78 = {
#include "stg26/terrain/draw_78.inc"
};
static NJS_CNK_OBJECT draw_77 = {
#include "stg26/terrain/draw_77.inc"
};
static NJS_CNK_OBJECT draw_74 = {
#include "stg26/terrain/draw_74.inc"
};
static NJS_CNK_OBJECT draw_68 = {
#include "stg26/terrain/draw_68.inc"
};
static NJS_CNK_OBJECT draw_66 = {
#include "stg26/terrain/draw_66.inc"
};
static NJS_CNK_OBJECT draw_62 = {
#include "stg26/terrain/draw_62.inc"
};
static NJS_CNK_OBJECT draw_61 = {
#include "stg26/terrain/draw_61.inc"
};
static NJS_CNK_OBJECT draw_60 = {
#include "stg26/terrain/draw_60.inc"
};
static NJS_CNK_OBJECT draw_57 = {
#include "stg26/terrain/draw_57.inc"
};
static NJS_CNK_OBJECT draw_56 = {
#include "stg26/terrain/draw_56.inc"
};
static NJS_CNK_OBJECT draw_55 = {
#include "stg26/terrain/draw_55.inc"
};
static NJS_CNK_OBJECT draw_53 = {
#include "stg26/terrain/draw_53.inc"
};
static NJS_CNK_OBJECT draw_52 = {
#include "stg26/terrain/draw_52.inc"
};
static NJS_CNK_OBJECT draw_51 = {
#include "stg26/terrain/draw_51.inc"
};
static NJS_CNK_OBJECT draw_50 = {
#include "stg26/terrain/draw_50.inc"
};
static NJS_CNK_OBJECT draw_49 = {
#include "stg26/terrain/draw_49.inc"
};
static NJS_CNK_OBJECT draw_48 = {
#include "stg26/terrain/draw_48.inc"
};
static NJS_CNK_OBJECT draw_47 = {
#include "stg26/terrain/draw_47.inc"
};
static NJS_CNK_OBJECT draw_46 = {
#include "stg26/terrain/draw_46.inc"
};
static NJS_CNK_OBJECT draw_45 = {
#include "stg26/terrain/draw_45.inc"
};
static NJS_CNK_OBJECT draw_44 = {
#include "stg26/terrain/draw_44.inc"
};
static NJS_CNK_OBJECT draw_43 = {
#include "stg26/terrain/draw_43.inc"
};
static NJS_CNK_OBJECT draw_42 = {
#include "stg26/terrain/draw_42.inc"
};
static NJS_CNK_OBJECT draw_41 = {
#include "stg26/terrain/draw_41.inc"
};
static NJS_CNK_OBJECT draw_40 = {
#include "stg26/terrain/draw_40.inc"
};
static NJS_CNK_OBJECT draw_39 = {
#include "stg26/terrain/draw_39.inc"
};
static NJS_CNK_OBJECT draw_37 = {
#include "stg26/terrain/draw_37.inc"
};
static NJS_CNK_OBJECT draw_35 = {
#include "stg26/terrain/draw_35.inc"
};
static NJS_CNK_OBJECT draw_33 = {
#include "stg26/terrain/draw_33.inc"
};
static NJS_CNK_OBJECT draw_31 = {
#include "stg26/terrain/draw_31.inc"
};
static NJS_CNK_OBJECT draw_30 = {
#include "stg26/terrain/draw_30.inc"
};
static NJS_CNK_OBJECT draw_29 = {
#include "stg26/terrain/draw_29.inc"
};
static NJS_CNK_OBJECT draw_28 = {
#include "stg26/terrain/draw_28.inc"
};
static NJS_CNK_OBJECT draw_27 = {
#include "stg26/terrain/draw_27.inc"
};
static NJS_CNK_OBJECT draw_26 = {
#include "stg26/terrain/draw_26.inc"
};
static NJS_CNK_OBJECT draw_25 = {
#include "stg26/terrain/draw_25.inc"
};
static NJS_CNK_OBJECT draw_23 = {
#include "stg26/terrain/draw_23.inc"
};
static NJS_CNK_OBJECT draw_22 = {
#include "stg26/terrain/draw_22.inc"
};
static NJS_CNK_OBJECT draw_21 = {
#include "stg26/terrain/draw_21.inc"
};
static NJS_CNK_OBJECT draw_20 = {
#include "stg26/terrain/draw_20.inc"
};
static NJS_CNK_OBJECT draw_19 = {
#include "stg26/terrain/draw_19.inc"
};
static NJS_CNK_OBJECT draw_17 = {
#include "stg26/terrain/draw_17.inc"
};
static NJS_CNK_OBJECT draw_15 = {
#include "stg26/terrain/draw_15.inc"
};
static NJS_CNK_OBJECT draw_14 = {
#include "stg26/terrain/draw_14.inc"
};
static NJS_CNK_OBJECT draw_13 = {
#include "stg26/terrain/draw_13.inc"
};
static NJS_CNK_OBJECT draw_12 = {
#include "stg26/terrain/draw_12.inc"
};
static NJS_CNK_OBJECT draw_11 = {
#include "stg26/terrain/draw_11.inc"
};
static NJS_CNK_OBJECT draw_10 = {
#include "stg26/terrain/draw_10.inc"
};
static NJS_CNK_OBJECT draw_9 = {
#include "stg26/terrain/draw_9.inc"
};
static NJS_CNK_OBJECT draw_8 = {
#include "stg26/terrain/draw_8.inc"
};
static NJS_CNK_OBJECT draw_7 = {
#include "stg26/terrain/draw_7.inc"
};
static NJS_CNK_OBJECT draw_6 = {
#include "stg26/terrain/draw_6.inc"
};
static NJS_CNK_OBJECT draw_5 = {
#include "stg26/terrain/draw_5.inc"
};
static NJS_CNK_OBJECT draw_4 = {
#include "stg26/terrain/draw_4.inc"
};
static NJS_CNK_OBJECT draw_3 = {
#include "stg26/terrain/draw_3.inc"
};
static NJS_CNK_OBJECT draw_2 = {
#include "stg26/terrain/draw_2.inc"
};
static NJS_CNK_OBJECT draw_1 = {
#include "stg26/terrain/draw_1.inc"
};
static NJS_OBJECT collision_0 = {
#include "stg26/terrain/collision_0.inc"
};
static NJS_OBJECT collision_3 = {
#include "stg26/terrain/collision_3.inc"
};
static NJS_OBJECT collision_7 = {
#include "stg26/terrain/collision_7.inc"
};
static NJS_OBJECT collision_10 = {
#include "stg26/terrain/collision_10.inc"
};
static NJS_OBJECT collision_12 = {
#include "stg26/terrain/collision_12.inc"
};
static NJS_OBJECT collision_27 = {
#include "stg26/terrain/collision_27.inc"
};
static NJS_OBJECT collision_28 = {
#include "stg26/terrain/collision_28.inc"
};
static NJS_OBJECT collision_29 = {
#include "stg26/terrain/collision_29.inc"
};
static NJS_OBJECT collision_31 = {
#include "stg26/terrain/collision_31.inc"
};
static NJS_OBJECT collision_32 = {
#include "stg26/terrain/collision_32.inc"
};
static NJS_OBJECT collision_33 = {
#include "stg26/terrain/collision_33.inc"
};
static NJS_OBJECT collision_35 = {
#include "stg26/terrain/collision_35.inc"
};
static NJS_OBJECT collision_41 = {
#include "stg26/terrain/collision_41.inc"
};
static NJS_OBJECT collision_42 = {
#include "stg26/terrain/collision_42.inc"
};
static NJS_OBJECT collision_50 = {
#include "stg26/terrain/collision_50.inc"
};
static NJS_OBJECT collision_57 = {
#include "stg26/terrain/collision_57.inc"
};
static NJS_OBJECT collision_59 = {
#include "stg26/terrain/collision_59.inc"
};
static NJS_OBJECT collision_61 = {
#include "stg26/terrain/collision_61.inc"
};
static NJS_OBJECT collision_62 = {
#include "stg26/terrain/collision_62.inc"
};
static NJS_OBJECT collision_63 = {
#include "stg26/terrain/collision_63.inc"
};
static NJS_OBJECT collision_64 = {
#include "stg26/terrain/collision_64.inc"
};
static NJS_OBJECT collision_65 = {
#include "stg26/terrain/collision_65.inc"
};
static NJS_OBJECT collision_66 = {
#include "stg26/terrain/collision_66.inc"
};
static NJS_OBJECT collision_67 = {
#include "stg26/terrain/collision_67.inc"
};
static NJS_OBJECT collision_69 = {
#include "stg26/terrain/collision_69.inc"
};
static NJS_OBJECT collision_71 = {
#include "stg26/terrain/collision_71.inc"
};
static NJS_OBJECT collision_72 = {
#include "stg26/terrain/collision_72.inc"
};
static NJS_OBJECT collision_77 = {
#include "stg26/terrain/collision_77.inc"
};
static NJS_OBJECT collision_87 = {
#include "stg26/terrain/collision_87.inc"
};
static NJS_OBJECT collision_88 = {
#include "stg26/terrain/collision_88.inc"
};
static NJS_OBJECT collision_89 = {
#include "stg26/terrain/collision_89.inc"
};
static NJS_OBJECT collision_90 = {
#include "stg26/terrain/collision_90.inc"
};
static NJS_OBJECT collision_91 = {
#include "stg26/terrain/collision_91.inc"
};
static NJS_OBJECT collision_92 = {
#include "stg26/terrain/collision_92.inc"
};
static NJS_OBJECT collision_93 = {
#include "stg26/terrain/collision_93.inc"
};
static NJS_OBJECT collision_94 = {
#include "stg26/terrain/collision_94.inc"
};
static NJS_OBJECT collision_95 = {
#include "stg26/terrain/collision_95.inc"
};
static NJS_OBJECT collision_96 = {
#include "stg26/terrain/collision_96.inc"
};
static NJS_OBJECT collision_97 = {
#include "stg26/terrain/collision_97.inc"
};
static NJS_OBJECT collision_99 = {
#include "stg26/terrain/collision_99.inc"
};
static NJS_OBJECT collision_100 = {
#include "stg26/terrain/collision_100.inc"
};
static NJS_OBJECT collision_101 = {
#include "stg26/terrain/collision_101.inc"
};
static NJS_OBJECT collision_107 = {
#include "stg26/terrain/collision_107.inc"
};
static NJS_OBJECT collision_108 = {
#include "stg26/terrain/collision_108.inc"
};
static NJS_OBJECT collision_109 = {
#include "stg26/terrain/collision_109.inc"
};
static NJS_OBJECT collision_111 = {
#include "stg26/terrain/collision_111.inc"
};
static NJS_OBJECT collision_121 = {
#include "stg26/terrain/collision_121.inc"
};
static NJS_OBJECT collision_122 = {
#include "stg26/terrain/collision_122.inc"
};
static NJS_OBJECT collision_123 = {
#include "stg26/terrain/collision_123.inc"
};
static NJS_OBJECT collision_126 = {
#include "stg26/terrain/collision_126.inc"
};
static NJS_OBJECT collision_127 = {
#include "stg26/terrain/collision_127.inc"
};
static NJS_OBJECT collision_128 = {
#include "stg26/terrain/collision_128.inc"
};
static NJS_OBJECT collision_138 = {
#include "stg26/terrain/collision_138.inc"
};
static NJS_OBJECT collision_141 = {
#include "stg26/terrain/collision_141.inc"
};
static NJS_OBJECT collision_142 = {
#include "stg26/terrain/collision_142.inc"
};
static NJS_OBJECT collision_143 = {
#include "stg26/terrain/collision_143.inc"
};
static NJS_OBJECT collision_144 = {
#include "stg26/terrain/collision_144.inc"
};
static NJS_OBJECT collision_145 = {
#include "stg26/terrain/collision_145.inc"
};
static NJS_OBJECT collision_146 = {
#include "stg26/terrain/collision_146.inc"
};
static NJS_OBJECT collision_147 = {
#include "stg26/terrain/collision_147.inc"
};
static NJS_OBJECT collision_151 = {
#include "stg26/terrain/collision_151.inc"
};
static NJS_OBJECT collision_153 = {
#include "stg26/terrain/collision_153.inc"
};
static NJS_OBJECT collision_154 = {
#include "stg26/terrain/collision_154.inc"
};
static NJS_OBJECT collision_155 = {
#include "stg26/terrain/collision_155.inc"
};
static NJS_OBJECT collision_157 = {
#include "stg26/terrain/collision_157.inc"
};
