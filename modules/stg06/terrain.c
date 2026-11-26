#include <ninja.h>
#include <sg_xpt.h>

#include "collision/world.h"
#include "stg06/sky_rail.h"

/* the land's file names, which its land table and texture list point at. each
 * starts on a 4-byte boundary */
const char sky_rail_land_filenames[302] = {
#include "stg06/terrain/sky_rail_land_filenames.inc"
};

/* STG06.PRS's land, texture lists, Basic models and chunk models, from the
 * disc. its D lies between drum_can.c and object_archives.c */

/* the data up to the last that a pointer names ahead of its definition, as
 * tentative definitions in the disc's order: GCC places each datum at its
 * first definition */
static struct land_entry entries[259];
struct land_table sky_rail_land;
static NJS_TEXNAME textures[26];
NJS_TEXLIST sky_rail_land_texlist;
static NJS_MATERIAL collision_0_materials[1];
static Sint16 collision_0_faces[49];
static NJS_MESHSET collision_0_meshsets[1];
static NJS_POINT3 collision_0_points[24];
static NJS_MODEL collision_0_model;
static NJS_OBJECT collision_0;
static NJS_MATERIAL collision_2_materials[1];
static Sint16 collision_2_faces[251];
static NJS_MESHSET collision_2_meshsets[1];
static NJS_POINT3 collision_2_points[96];
static NJS_MODEL collision_2_model;
static NJS_OBJECT collision_2;
static NJS_MATERIAL collision_3_materials[1];
static Sint16 collision_3_faces[174];
static NJS_MESHSET collision_3_meshsets[1];
static NJS_POINT3 collision_3_points[70];
static NJS_MODEL collision_3_model;
static NJS_OBJECT collision_3;
static NJS_MATERIAL collision_4_materials[1];
static Sint16 collision_4_faces[41];
static NJS_MESHSET collision_4_meshsets[1];
static NJS_POINT3 collision_4_points[19];
static NJS_MODEL collision_4_model;
static NJS_OBJECT collision_4;
static NJS_MATERIAL collision_5_materials[1];
static Sint16 collision_5_faces[156];
static NJS_MESHSET collision_5_meshsets[1];
static NJS_POINT3 collision_5_points[75];
static NJS_MODEL collision_5_model;
static NJS_OBJECT collision_5;
static NJS_MATERIAL collision_6_materials[1];
static Sint16 collision_6_faces[76];
static NJS_MESHSET collision_6_meshsets[1];
static NJS_POINT3 collision_6_points[36];
static NJS_MODEL collision_6_model;
static NJS_OBJECT collision_6;
static NJS_MATERIAL collision_7_materials[1];
static Sint16 collision_7_faces[48];
static NJS_MESHSET collision_7_meshsets[1];
static NJS_POINT3 collision_7_points[22];
static NJS_MODEL collision_7_model;
static NJS_OBJECT collision_7;
static NJS_MATERIAL collision_8_materials[1];
static Sint16 collision_8_faces[10];
static NJS_MESHSET collision_8_meshsets[1];
static NJS_POINT3 collision_8_points[8];
static NJS_MODEL collision_8_model;
static NJS_OBJECT collision_8;
static NJS_MATERIAL collision_9_materials[1];
static Sint16 collision_9_faces[238];
static NJS_MESHSET collision_9_meshsets[1];
static NJS_POINT3 collision_9_points[97];
static NJS_MODEL collision_9_model;
static NJS_OBJECT collision_9;
static NJS_MATERIAL collision_10_materials[1];
static Sint16 collision_10_faces[38];
static NJS_MESHSET collision_10_meshsets[1];
static NJS_POINT3 collision_10_points[20];
static NJS_MODEL collision_10_model;
static NJS_OBJECT collision_10;
static NJS_MATERIAL collision_11_materials[1];
static Sint16 collision_11_faces[20];
static NJS_MESHSET collision_11_meshsets[1];
static NJS_POINT3 collision_11_points[12];
static NJS_MODEL collision_11_model;
static NJS_OBJECT collision_11;
static NJS_MATERIAL collision_12_materials[1];
static Sint16 collision_12_faces[67];
static NJS_MESHSET collision_12_meshsets[1];
static NJS_POINT3 collision_12_points[30];
static NJS_MODEL collision_12_model;
static NJS_OBJECT collision_12;
static NJS_MATERIAL collision_13_materials[1];
static Sint16 collision_13_faces[58];
static NJS_MESHSET collision_13_meshsets[1];
static NJS_POINT3 collision_13_points[27];
static NJS_MODEL collision_13_model;
static NJS_OBJECT collision_13;
static NJS_MATERIAL collision_14_materials[1];
static Sint16 collision_14_faces[78];
static NJS_MESHSET collision_14_meshsets[1];
static NJS_POINT3 collision_14_points[36];
static NJS_MODEL collision_14_model;
static NJS_OBJECT collision_14;
static NJS_MATERIAL collision_15_materials[1];
static Sint16 collision_15_faces[89];
static NJS_MESHSET collision_15_meshsets[1];
static NJS_POINT3 collision_15_points[36];
static NJS_MODEL collision_15_model;
static NJS_OBJECT collision_15;
static NJS_MATERIAL collision_16_materials[1];
static Sint16 collision_16_faces[75];
static NJS_MESHSET collision_16_meshsets[1];
static NJS_POINT3 collision_16_points[32];
static NJS_MODEL collision_16_model;
static NJS_OBJECT collision_16;
static NJS_MATERIAL collision_18_materials[1];
static Sint16 collision_18_faces[119];
static NJS_MESHSET collision_18_meshsets[1];
static NJS_POINT3 collision_18_points[48];
static NJS_MODEL collision_18_model;
static NJS_OBJECT collision_18;
static NJS_MATERIAL collision_19_materials[1];
static Sint16 collision_19_faces[36];
static NJS_MESHSET collision_19_meshsets[1];
static NJS_POINT3 collision_19_points[17];
static NJS_MODEL collision_19_model;
static NJS_OBJECT collision_19;
static NJS_MATERIAL collision_20_materials[1];
static Sint16 collision_20_faces[39];
static NJS_MESHSET collision_20_meshsets[1];
static NJS_POINT3 collision_20_points[17];
static NJS_MODEL collision_20_model;
static NJS_OBJECT collision_20;
static NJS_MATERIAL collision_21_materials[1];
static Sint16 collision_21_faces[157];
static NJS_MESHSET collision_21_meshsets[1];
static NJS_POINT3 collision_21_points[74];
static NJS_MODEL collision_21_model;
static NJS_OBJECT collision_21;
static NJS_MATERIAL collision_22_materials[1];
static Sint16 collision_22_faces[39];
static NJS_MESHSET collision_22_meshsets[1];
static NJS_POINT3 collision_22_points[17];
static NJS_MODEL collision_22_model;
static NJS_OBJECT collision_22;
static NJS_MATERIAL collision_23_materials[1];
static Sint16 collision_23_faces[143];
static NJS_MESHSET collision_23_meshsets[1];
static NJS_POINT3 collision_23_points[56];
static NJS_MODEL collision_23_model;
static NJS_OBJECT collision_23;
static NJS_MATERIAL collision_24_materials[1];
static Sint16 collision_24_faces[125];
static NJS_MESHSET collision_24_meshsets[1];
static NJS_POINT3 collision_24_points[50];
static NJS_MODEL collision_24_model;
static NJS_OBJECT collision_24;
static NJS_MATERIAL collision_25_materials[1];
static Sint16 collision_25_faces[16];
static NJS_MESHSET collision_25_meshsets[1];
static NJS_POINT3 collision_25_points[8];
static NJS_MODEL collision_25_model;
static NJS_OBJECT collision_25;
static NJS_MATERIAL collision_26_materials[1];
static Sint16 collision_26_faces[16];
static NJS_MESHSET collision_26_meshsets[1];
static NJS_POINT3 collision_26_points[8];
static NJS_MODEL collision_26_model;
static NJS_OBJECT collision_26;
static NJS_MATERIAL collision_27_materials[1];
static Sint16 collision_27_faces[19];
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
static Sint16 collision_29_faces[19];
static NJS_MESHSET collision_29_meshsets[1];
static NJS_POINT3 collision_29_points[8];
static NJS_MODEL collision_29_model;
static NJS_OBJECT collision_29;
static NJS_MATERIAL collision_30_materials[1];
static Sint16 collision_30_faces[128];
static NJS_MESHSET collision_30_meshsets[1];
static NJS_POINT3 collision_30_points[50];
static NJS_MODEL collision_30_model;
static NJS_OBJECT collision_30;
static NJS_MATERIAL collision_32_materials[1];
static Sint16 collision_32_faces[117];
static NJS_MESHSET collision_32_meshsets[1];
static NJS_POINT3 collision_32_points[56];
static NJS_MODEL collision_32_model;
static NJS_OBJECT collision_32;
static NJS_MATERIAL collision_33_materials[1];
static Sint16 collision_33_faces[21];
static NJS_MESHSET collision_33_meshsets[1];
static NJS_POINT3 collision_33_points[16];
static NJS_MODEL collision_33_model;
static NJS_OBJECT collision_33;
static NJS_MATERIAL collision_35_materials[1];
static Sint16 collision_35_faces[17];
static NJS_MESHSET collision_35_meshsets[1];
static NJS_POINT3 collision_35_points[12];
static NJS_MODEL collision_35_model;
static NJS_OBJECT collision_35;
static NJS_MATERIAL collision_38_materials[1];
static Sint16 collision_38_faces[11];
static NJS_MESHSET collision_38_meshsets[1];
static NJS_POINT3 collision_38_points[9];
static NJS_MODEL collision_38_model;
static NJS_OBJECT collision_38;
static NJS_MATERIAL collision_41_materials[1];
static Sint16 collision_41_faces[66];
static NJS_MESHSET collision_41_meshsets[1];
static NJS_POINT3 collision_41_points[33];
static NJS_MODEL collision_41_model;
static NJS_OBJECT collision_41;
static NJS_MATERIAL collision_42_materials[1];
static Sint16 collision_42_faces[66];
static NJS_MESHSET collision_42_meshsets[1];
static NJS_POINT3 collision_42_points[33];
static NJS_MODEL collision_42_model;
static NJS_OBJECT collision_42;
static NJS_MATERIAL collision_43_materials[1];
static Sint16 collision_43_faces[60];
static NJS_MESHSET collision_43_meshsets[1];
static NJS_POINT3 collision_43_points[30];
static NJS_MODEL collision_43_model;
static NJS_OBJECT collision_43;
static NJS_MATERIAL collision_44_materials[1];
static Sint16 collision_44_faces[72];
static NJS_MESHSET collision_44_meshsets[1];
static NJS_POINT3 collision_44_points[30];
static NJS_MODEL collision_44_model;
static NJS_OBJECT collision_44;
static NJS_MATERIAL collision_46_materials[1];
static Sint16 collision_46_faces[78];
static NJS_MESHSET collision_46_meshsets[1];
static NJS_POINT3 collision_46_points[36];
static NJS_MODEL collision_46_model;
static NJS_OBJECT collision_46;
static NJS_MATERIAL collision_47_materials[1];
static Sint16 collision_47_faces[75];
static NJS_MESHSET collision_47_meshsets[1];
static NJS_POINT3 collision_47_points[36];
static NJS_MODEL collision_47_model;
static NJS_OBJECT collision_47;
static NJS_MATERIAL collision_49_materials[1];
static Sint16 collision_49_faces[121];
static NJS_MESHSET collision_49_meshsets[1];
static NJS_POINT3 collision_49_points[51];
static NJS_MODEL collision_49_model;
static NJS_OBJECT collision_49;
static NJS_MATERIAL collision_50_materials[1];
static Sint16 collision_50_faces[180];
static NJS_MESHSET collision_50_meshsets[1];
static NJS_POINT3 collision_50_points[93];
static NJS_MODEL collision_50_model;
static NJS_OBJECT collision_50;
static NJS_MATERIAL collision_51_materials[1];
static Sint16 collision_51_faces[321];
static NJS_MESHSET collision_51_meshsets[1];
static NJS_POINT3 collision_51_points[135];
static NJS_MODEL collision_51_model;
static NJS_OBJECT collision_51;
static NJS_MATERIAL collision_52_materials[1];
static Sint16 collision_52_faces[246];
static NJS_MESHSET collision_52_meshsets[1];
static NJS_POINT3 collision_52_points[125];
static NJS_MODEL collision_52_model;
static NJS_OBJECT collision_52;
static NJS_MATERIAL collision_53_materials[1];
static Sint16 collision_53_faces[18];
static NJS_MESHSET collision_53_meshsets[1];
static NJS_POINT3 collision_53_points[9];
static NJS_MODEL collision_53_model;
static NJS_OBJECT collision_53;
static NJS_MATERIAL collision_56_materials[1];
static Sint16 collision_56_faces[18];
static NJS_MESHSET collision_56_meshsets[1];
static NJS_POINT3 collision_56_points[9];
static NJS_MODEL collision_56_model;
static NJS_OBJECT collision_56;
static NJS_MATERIAL collision_57_materials[1];
static Sint16 collision_57_faces[18];
static NJS_MESHSET collision_57_meshsets[1];
static NJS_POINT3 collision_57_points[9];
static NJS_MODEL collision_57_model;
static NJS_OBJECT collision_57;
static NJS_MATERIAL collision_58_materials[1];
static Sint16 collision_58_faces[20];
static NJS_MESHSET collision_58_meshsets[1];
static NJS_POINT3 collision_58_points[11];
static NJS_MODEL collision_58_model;
static NJS_OBJECT collision_58;
static NJS_MATERIAL collision_59_materials[1];
static Sint16 collision_59_faces[18];
static NJS_MESHSET collision_59_meshsets[1];
static NJS_POINT3 collision_59_points[9];
static NJS_MODEL collision_59_model;
static NJS_OBJECT collision_59;
static NJS_MATERIAL collision_60_materials[1];
static Sint16 collision_60_faces[18];
static NJS_MESHSET collision_60_meshsets[1];
static NJS_POINT3 collision_60_points[9];
static NJS_MODEL collision_60_model;
static NJS_OBJECT collision_60;
static NJS_MATERIAL collision_61_materials[1];
static Sint16 collision_61_faces[18];
static NJS_MESHSET collision_61_meshsets[1];
static NJS_POINT3 collision_61_points[9];
static NJS_MODEL collision_61_model;
static NJS_OBJECT collision_61;
static NJS_MATERIAL collision_62_materials[1];
static Sint16 collision_62_faces[9];
static NJS_MESHSET collision_62_meshsets[1];
static NJS_POINT3 collision_62_points[6];
static NJS_MODEL collision_62_model;
static NJS_OBJECT collision_62;
static NJS_MATERIAL collision_64_materials[1];
static Sint16 collision_64_faces[18];
static NJS_MESHSET collision_64_meshsets[1];
static NJS_POINT3 collision_64_points[9];
static NJS_MODEL collision_64_model;
static NJS_OBJECT collision_64;
static NJS_MATERIAL collision_65_materials[1];
static Sint16 collision_65_faces[64];
static NJS_MESHSET collision_65_meshsets[1];
static NJS_POINT3 collision_65_points[28];
static NJS_MODEL collision_65_model;
static NJS_OBJECT collision_65;
static NJS_MATERIAL collision_72_materials[1];
static Sint16 collision_72_faces[74];
static NJS_MESHSET collision_72_meshsets[1];
static NJS_POINT3 collision_72_points[32];
static NJS_MODEL collision_72_model;
static NJS_OBJECT collision_72;
static NJS_MATERIAL collision_73_materials[1];
static Sint16 collision_73_faces[74];
static NJS_MESHSET collision_73_meshsets[1];
static NJS_POINT3 collision_73_points[32];
static NJS_MODEL collision_73_model;
static NJS_OBJECT collision_73;
static NJS_MATERIAL collision_74_materials[1];
static Sint16 collision_74_faces[9];
static NJS_MESHSET collision_74_meshsets[1];
static NJS_POINT3 collision_74_points[6];
static NJS_MODEL collision_74_model;
static NJS_OBJECT collision_74;
static NJS_MATERIAL collision_75_materials[1];
static Sint16 collision_75_faces[74];
static NJS_MESHSET collision_75_meshsets[1];
static NJS_POINT3 collision_75_points[32];
static NJS_MODEL collision_75_model;
static NJS_OBJECT collision_75;
static NJS_MATERIAL collision_76_materials[1];
static Sint16 collision_76_faces[64];
static NJS_MESHSET collision_76_meshsets[1];
static NJS_POINT3 collision_76_points[28];
static NJS_MODEL collision_76_model;
static NJS_OBJECT collision_76;
static NJS_MATERIAL collision_77_materials[1];
static Sint16 collision_77_faces[9];
static NJS_MESHSET collision_77_meshsets[1];
static NJS_POINT3 collision_77_points[6];
static NJS_MODEL collision_77_model;
static NJS_OBJECT collision_77;
static NJS_MATERIAL collision_78_materials[1];
static Sint16 collision_78_faces[7];
static NJS_MESHSET collision_78_meshsets[1];
static NJS_POINT3 collision_78_points[6];
static NJS_MODEL collision_78_model;
static NJS_OBJECT collision_78;
static NJS_MATERIAL collision_79_materials[1];
static Sint16 collision_79_faces[48];
static NJS_MESHSET collision_79_meshsets[1];
static NJS_POINT3 collision_79_points[24];
static NJS_MODEL collision_79_model;
static NJS_OBJECT collision_79;
static NJS_MATERIAL collision_80_materials[1];
static Sint16 collision_80_faces[34];
static NJS_MESHSET collision_80_meshsets[1];
static NJS_POINT3 collision_80_points[28];
static NJS_MODEL collision_80_model;
static NJS_OBJECT collision_80;
static NJS_MATERIAL collision_81_materials[1];
static Sint16 collision_81_faces[22];
static NJS_MESHSET collision_81_meshsets[1];
static NJS_POINT3 collision_81_points[12];
static NJS_MODEL collision_81_model;
static NJS_OBJECT collision_81;
static NJS_MATERIAL collision_82_materials[1];
static Sint16 collision_82_faces[37];
static NJS_MESHSET collision_82_meshsets[1];
static NJS_POINT3 collision_82_points[16];
static NJS_MODEL collision_82_model;
static NJS_OBJECT collision_82;
static NJS_MATERIAL collision_83_materials[1];
static Sint16 collision_83_faces[174];
static NJS_MESHSET collision_83_meshsets[1];
static NJS_POINT3 collision_83_points[66];
static NJS_MODEL collision_83_model;
static NJS_OBJECT collision_83;
static NJS_MATERIAL collision_84_materials[1];
static Sint16 collision_84_faces[122];
static NJS_MESHSET collision_84_meshsets[1];
static NJS_POINT3 collision_84_points[72];
static NJS_MODEL collision_84_model;
static NJS_OBJECT collision_84;
static NJS_MATERIAL collision_85_materials[1];
static Sint16 collision_85_faces[62];
static NJS_MESHSET collision_85_meshsets[1];
static NJS_POINT3 collision_85_points[42];
static NJS_MODEL collision_85_model;
static NJS_OBJECT collision_85;
static NJS_MATERIAL collision_86_materials[1];
static Sint16 collision_86_faces[26];
static NJS_MESHSET collision_86_meshsets[1];
static NJS_POINT3 collision_86_points[18];
static NJS_MODEL collision_86_model;
static NJS_OBJECT collision_86;
static NJS_MATERIAL collision_88_materials[1];
static Sint16 collision_88_faces[22];
static NJS_MESHSET collision_88_meshsets[1];
static NJS_POINT3 collision_88_points[15];
static NJS_MODEL collision_88_model;
static NJS_OBJECT collision_88;
static NJS_MATERIAL collision_91_materials[1];
static Sint16 collision_91_faces[18];
static NJS_MESHSET collision_91_meshsets[1];
static NJS_POINT3 collision_91_points[12];
static NJS_MODEL collision_91_model;
static NJS_OBJECT collision_91;
static NJS_MATERIAL collision_94_materials[1];
static Sint16 collision_94_faces[38];
static NJS_MESHSET collision_94_meshsets[1];
static NJS_POINT3 collision_94_points[27];
static NJS_MODEL collision_94_model;
static NJS_OBJECT collision_94;
static NJS_MATERIAL collision_95_materials[1];
static Sint16 collision_95_faces[10];
static NJS_MESHSET collision_95_meshsets[1];
static NJS_POINT3 collision_95_points[6];
static NJS_MODEL collision_95_model;
static NJS_OBJECT collision_95;
static Sint16 draw_9_polygons[243];
static Sint32 draw_9_vertices[243];
static NJS_CNK_MODEL draw_9_model;
static NJS_CNK_OBJECT draw_161;
static Sint16 draw_89_polygons[71];
static Sint32 draw_89_vertices[83];
static NJS_CNK_MODEL draw_89_model;
static NJS_CNK_OBJECT draw_155;
static Sint16 draw_19_polygons[115];
static Sint32 draw_19_vertices[111];
static NJS_CNK_MODEL draw_19_model;
static NJS_CNK_OBJECT draw_154;
static Sint16 draw_20_polygons[103];
static Sint32 draw_20_vertices[83];
static NJS_CNK_MODEL draw_20_model;
static NJS_CNK_OBJECT draw_153;
static Sint16 draw_151_polygons[245];
static Sint32 draw_151_vertices[183];
static NJS_CNK_MODEL draw_151_model;
static NJS_CNK_OBJECT draw_151;
static Sint16 draw_149_polygons[37];
static Sint32 draw_149_vertices[43];
static NJS_CNK_MODEL draw_149_model;
static NJS_CNK_OBJECT draw_149;
static Sint16 draw_148_polygons[45];
static Sint32 draw_148_vertices[47];
static NJS_CNK_MODEL draw_148_model;
static NJS_CNK_OBJECT draw_148;
static Sint16 draw_147_polygons[399];
static Sint32 draw_147_vertices[479];
static NJS_CNK_MODEL draw_147_model;
static NJS_CNK_OBJECT draw_147;
static Sint16 draw_25_polygons[43];
static Sint32 draw_25_vertices[39];
static NJS_CNK_MODEL draw_25_model;
static NJS_CNK_OBJECT draw_140;
static Sint16 draw_138_polygons[319];
static Sint32 draw_138_vertices[203];
static NJS_CNK_MODEL draw_138_model;
static NJS_CNK_OBJECT draw_138;
static Sint16 draw_137_polygons[131];
static Sint32 draw_137_vertices[75];
static NJS_CNK_MODEL draw_137_model;
static NJS_CNK_OBJECT draw_137;
static Sint16 draw_136_polygons[45];
static Sint32 draw_136_vertices[35];
static NJS_CNK_MODEL draw_136_model;
static NJS_CNK_OBJECT draw_136;
static Sint16 draw_135_polygons[25];
static Sint32 draw_135_vertices[27];
static NJS_CNK_MODEL draw_135_model;
static NJS_CNK_OBJECT draw_135;
static Sint16 draw_134_polygons[25];
static Sint32 draw_134_vertices[27];
static NJS_CNK_MODEL draw_134_model;
static NJS_CNK_OBJECT draw_134;
static Sint16 draw_133_polygons[25];
static Sint32 draw_133_vertices[27];
static NJS_CNK_MODEL draw_133_model;
static NJS_CNK_OBJECT draw_133;
static Sint16 draw_132_polygons[25];
static Sint32 draw_132_vertices[27];
static NJS_CNK_MODEL draw_132_model;
static NJS_CNK_OBJECT draw_132;
static Sint16 draw_131_polygons[25];
static Sint32 draw_131_vertices[27];
static NJS_CNK_MODEL draw_131_model;
static NJS_CNK_OBJECT draw_131;
static Sint16 draw_128_polygons[157];
static Sint32 draw_128_vertices[91];
static NJS_CNK_MODEL draw_128_model;
static NJS_CNK_OBJECT draw_128;
static Sint16 draw_0_polygons[391];
static Sint32 draw_0_vertices[167];
static NJS_CNK_MODEL draw_0_model;
static NJS_CNK_OBJECT draw_123;
static Sint16 draw_121_polygons[271];
static Sint32 draw_121_vertices[331];
static NJS_CNK_MODEL draw_121_model;
static NJS_CNK_OBJECT draw_121;
static Sint16 draw_120_polygons[39];
static Sint32 draw_120_vertices[35];
static NJS_CNK_MODEL draw_120_model;
static NJS_CNK_OBJECT draw_120;
static Sint16 draw_118_polygons[89];
static Sint32 draw_118_vertices[51];
static NJS_CNK_MODEL draw_118_model;
static NJS_CNK_OBJECT draw_118;
static Sint16 draw_28_polygons[69];
static Sint32 draw_28_vertices[47];
static NJS_CNK_MODEL draw_28_model;
static NJS_CNK_OBJECT draw_113;
static Sint16 draw_112_polygons[227];
static Sint32 draw_112_vertices[131];
static NJS_CNK_MODEL draw_112_model;
static NJS_CNK_OBJECT draw_112;
static Sint16 draw_100_polygons[109];
static Sint32 draw_100_vertices[75];
static NJS_CNK_MODEL draw_100_model;
static NJS_CNK_OBJECT draw_111;
static Sint16 draw_99_polygons[233];
static Sint32 draw_99_vertices[151];
static NJS_CNK_MODEL draw_99_model;
static NJS_CNK_OBJECT draw_110;
static Sint16 draw_108_polygons[71];
static Sint32 draw_108_vertices[51];
static NJS_CNK_MODEL draw_108_model;
static NJS_CNK_OBJECT draw_108;
static Sint16 draw_107_polygons[229];
static Sint32 draw_107_vertices[135];
static NJS_CNK_MODEL draw_107_model;
static NJS_CNK_OBJECT draw_107;
static Sint16 draw_105_polygons[221];
static Sint32 draw_105_vertices[119];
static NJS_CNK_MODEL draw_105_model;
static NJS_CNK_OBJECT draw_105;
static Sint16 draw_104_polygons[103];
static Sint32 draw_104_vertices[91];
static NJS_CNK_MODEL draw_104_model;
static NJS_CNK_OBJECT draw_104;
static Sint16 draw_103_polygons[193];
static Sint32 draw_103_vertices[151];
static NJS_CNK_MODEL draw_103_model;
static NJS_CNK_OBJECT draw_103;
static Sint16 draw_98_polygons[145];
static Sint32 draw_98_vertices[79];
static NJS_CNK_MODEL draw_98_model;
static NJS_CNK_OBJECT draw_98;
static Sint16 draw_91_polygons[667];
static Sint32 draw_91_vertices[803];
static NJS_CNK_MODEL draw_91_model;
static NJS_CNK_OBJECT draw_91;
static Sint16 draw_90_polygons[875];
static Sint32 draw_90_vertices[1103];
static NJS_CNK_MODEL draw_90_model;
static NJS_CNK_OBJECT draw_90;
static Sint16 draw_88_polygons[787];
static Sint32 draw_88_vertices[435];
static NJS_CNK_MODEL draw_88_model;
static NJS_CNK_OBJECT draw_88;
static Sint16 draw_87_polygons[57];
static Sint32 draw_87_vertices[51];
static NJS_CNK_MODEL draw_87_model;
static NJS_CNK_OBJECT draw_87;
static Sint16 draw_86_polygons[99];
static Sint32 draw_86_vertices[75];
static NJS_CNK_MODEL draw_86_model;
static NJS_CNK_OBJECT draw_86;
static Sint16 draw_85_polygons[797];
static Sint32 draw_85_vertices[991];
static NJS_CNK_MODEL draw_85_model;
static NJS_CNK_OBJECT draw_85;
static Sint16 draw_81_polygons[185];
static Sint32 draw_81_vertices[83];
static NJS_CNK_MODEL draw_81_model;
static NJS_CNK_OBJECT draw_81;
static Sint16 draw_80_polygons[185];
static Sint32 draw_80_vertices[83];
static NJS_CNK_MODEL draw_80_model;
static NJS_CNK_OBJECT draw_80;
static Sint16 draw_79_polygons[401];
static Sint32 draw_79_vertices[447];
static NJS_CNK_MODEL draw_79_model;
static NJS_CNK_OBJECT draw_79;
static Sint16 draw_78_polygons[509];
static Sint32 draw_78_vertices[563];
static NJS_CNK_MODEL draw_78_model;
static NJS_CNK_OBJECT draw_78;
static Sint16 draw_77_polygons[977];
static Sint32 draw_77_vertices[1211];
static NJS_CNK_MODEL draw_77_model;
static NJS_CNK_OBJECT draw_77;
static Sint16 draw_75_polygons[261];
static Sint32 draw_75_vertices[239];
static NJS_CNK_MODEL draw_75_model;
static NJS_CNK_OBJECT draw_75;
static Sint16 draw_73_polygons[91];
static Sint32 draw_73_vertices[59];
static NJS_CNK_MODEL draw_73_model;
static NJS_CNK_OBJECT draw_73;
static Sint16 draw_71_polygons[131];
static Sint32 draw_71_vertices[87];
static NJS_CNK_MODEL draw_71_model;
static NJS_CNK_OBJECT draw_71;
static Sint16 draw_70_polygons[181];
static Sint32 draw_70_vertices[123];
static NJS_CNK_MODEL draw_70_model;
static NJS_CNK_OBJECT draw_70;
static Sint16 draw_69_polygons[389];
static Sint32 draw_69_vertices[223];
static NJS_CNK_MODEL draw_69_model;
static NJS_CNK_OBJECT draw_69;
static Sint16 draw_68_polygons[1019];
static Sint32 draw_68_vertices[631];
static NJS_CNK_MODEL draw_68_model;
static NJS_CNK_OBJECT draw_68;
static Sint16 draw_67_polygons[99];
static Sint32 draw_67_vertices[75];
static NJS_CNK_MODEL draw_67_model;
static NJS_CNK_OBJECT draw_67;
static Sint16 draw_66_polygons[93];
static Sint32 draw_66_vertices[75];
static NJS_CNK_MODEL draw_66_model;
static NJS_CNK_OBJECT draw_66;
static Sint16 draw_65_polygons[375];
static Sint32 draw_65_vertices[207];
static NJS_CNK_MODEL draw_65_model;
static NJS_CNK_OBJECT draw_65;
static Sint16 draw_64_polygons[385];
static Sint32 draw_64_vertices[183];
static NJS_CNK_MODEL draw_64_model;
static NJS_CNK_OBJECT draw_64;
static Sint16 draw_63_polygons[49];
static Sint32 draw_63_vertices[51];
static NJS_CNK_MODEL draw_63_model;
static NJS_CNK_OBJECT draw_63;
static Sint16 draw_62_polygons[861];
static Sint32 draw_62_vertices[819];
static NJS_CNK_MODEL draw_62_model;
static NJS_CNK_OBJECT draw_62;
static Sint16 draw_61_polygons[709];
static Sint32 draw_61_vertices[415];
static NJS_CNK_MODEL draw_61_model;
static NJS_CNK_OBJECT draw_61;
static Sint16 draw_60_polygons[519];
static Sint32 draw_60_vertices[271];
static NJS_CNK_MODEL draw_60_model;
static NJS_CNK_OBJECT draw_60;
static Sint16 draw_59_polygons[161];
static Sint32 draw_59_vertices[123];
static NJS_CNK_MODEL draw_59_model;
static NJS_CNK_OBJECT draw_59;
static Sint16 draw_58_polygons[49];
static Sint32 draw_58_vertices[51];
static NJS_CNK_MODEL draw_58_model;
static NJS_CNK_OBJECT draw_58;
static Sint16 draw_57_polygons[889];
static Sint32 draw_57_vertices[511];
static NJS_CNK_MODEL draw_57_model;
static NJS_CNK_OBJECT draw_57;
static Sint16 draw_56_polygons[329];
static Sint32 draw_56_vertices[183];
static NJS_CNK_MODEL draw_56_model;
static NJS_CNK_OBJECT draw_56;
static Sint16 draw_55_polygons[1543];
static Sint32 draw_55_vertices[1891];
static NJS_CNK_MODEL draw_55_model;
static NJS_CNK_OBJECT draw_55;
static Sint16 draw_54_polygons[1177];
static Sint32 draw_54_vertices[755];
static NJS_CNK_MODEL draw_54_model;
static NJS_CNK_OBJECT draw_54;
static Sint16 draw_53_polygons[201];
static Sint32 draw_53_vertices[123];
static NJS_CNK_MODEL draw_53_model;
static NJS_CNK_OBJECT draw_53;
static Sint16 draw_52_polygons[221];
static Sint32 draw_52_vertices[239];
static NJS_CNK_MODEL draw_52_model;
static NJS_CNK_OBJECT draw_52;
static Sint16 draw_51_polygons[679];
static Sint32 draw_51_vertices[447];
static NJS_CNK_MODEL draw_51_model;
static NJS_CNK_OBJECT draw_51;
static Sint16 draw_50_polygons[103];
static Sint32 draw_50_vertices[75];
static NJS_CNK_MODEL draw_50_model;
static NJS_CNK_OBJECT draw_50;
static Sint16 draw_48_polygons[261];
static Sint32 draw_48_vertices[159];
static NJS_CNK_MODEL draw_48_model;
static NJS_CNK_OBJECT draw_49;
static Sint16 draw_43_polygons[185];
static Sint32 draw_43_vertices[95];
static NJS_CNK_MODEL draw_43_model;
static NJS_CNK_OBJECT draw_45;
static Sint16 draw_40_polygons[997];
static Sint32 draw_40_vertices[1071];
static NJS_CNK_MODEL draw_40_model;
static NJS_CNK_OBJECT draw_40;
static Sint16 draw_39_polygons[2271];
static Sint32 draw_39_vertices[1227];
static NJS_CNK_MODEL draw_39_model;
static NJS_CNK_OBJECT draw_39;
static Sint16 draw_38_polygons[201];
static Sint32 draw_38_vertices[119];
static NJS_CNK_MODEL draw_38_model;
static NJS_CNK_OBJECT draw_38;
static Sint16 draw_37_polygons[237];
static Sint32 draw_37_vertices[135];
static NJS_CNK_MODEL draw_37_model;
static NJS_CNK_OBJECT draw_37;
static Sint16 draw_36_polygons[587];
static Sint32 draw_36_vertices[611];
static NJS_CNK_MODEL draw_36_model;
static NJS_CNK_OBJECT draw_36;
static Sint16 draw_35_polygons[1155];
static Sint32 draw_35_vertices[1391];
static NJS_CNK_MODEL draw_35_model;
static NJS_CNK_OBJECT draw_35;
static Sint16 draw_34_polygons[839];
static Sint32 draw_34_vertices[927];
static NJS_CNK_MODEL draw_34_model;
static NJS_CNK_OBJECT draw_34;
static Sint16 draw_33_polygons[617];
static Sint32 draw_33_vertices[719];
static NJS_CNK_MODEL draw_33_model;
static NJS_CNK_OBJECT draw_33;
static Sint16 draw_32_polygons[89];
static Sint32 draw_32_vertices[87];
static NJS_CNK_MODEL draw_32_model;
static NJS_CNK_OBJECT draw_32;
static Sint16 draw_31_polygons[475];
static Sint32 draw_31_vertices[259];
static NJS_CNK_MODEL draw_31_model;
static NJS_CNK_OBJECT draw_31;
static Sint16 draw_18_polygons[79];
static Sint32 draw_18_vertices[51];
static NJS_CNK_MODEL draw_18_model;
static NJS_CNK_OBJECT draw_18;
static Sint16 draw_17_polygons[105];
static Sint32 draw_17_vertices[55];
static NJS_CNK_MODEL draw_17_model;
static NJS_CNK_OBJECT draw_17;
static Sint16 draw_16_polygons[187];
static Sint32 draw_16_vertices[103];
static NJS_CNK_MODEL draw_16_model;
static NJS_CNK_OBJECT draw_16;
static Sint16 draw_15_polygons[45];
static Sint32 draw_15_vertices[35];
static NJS_CNK_MODEL draw_15_model;
static NJS_CNK_OBJECT draw_15;
static Sint16 draw_14_polygons[19];
static Sint32 draw_14_vertices[19];
static NJS_CNK_MODEL draw_14_model;
static NJS_CNK_OBJECT draw_14;
static Sint16 draw_13_polygons[67];
static Sint32 draw_13_vertices[83];
static NJS_CNK_MODEL draw_13_model;
static NJS_CNK_OBJECT draw_13;
static Sint16 draw_12_polygons[45];
static Sint32 draw_12_vertices[35];
static NJS_CNK_MODEL draw_12_model;
static NJS_CNK_OBJECT draw_12;
static Sint16 draw_11_polygons[43];
static Sint32 draw_11_vertices[31];
static NJS_CNK_MODEL draw_11_model;
static NJS_CNK_OBJECT draw_11;
static Sint16 draw_10_polygons[43];
static Sint32 draw_10_vertices[35];
static NJS_CNK_MODEL draw_10_model;
static NJS_CNK_OBJECT draw_10;
static Sint16 draw_8_polygons[481];
static Sint32 draw_8_vertices[455];
static NJS_CNK_MODEL draw_8_model;
static NJS_CNK_OBJECT draw_8;
static Sint16 draw_7_polygons[347];
static Sint32 draw_7_vertices[319];
static NJS_CNK_MODEL draw_7_model;
static NJS_CNK_OBJECT draw_7;
static Sint16 draw_6_polygons[239];
static Sint32 draw_6_vertices[267];
static NJS_CNK_MODEL draw_6_model;
static NJS_CNK_OBJECT draw_6;
static Sint16 draw_5_polygons[477];
static Sint32 draw_5_vertices[463];
static NJS_CNK_MODEL draw_5_model;
static NJS_CNK_OBJECT draw_5;
static Sint16 draw_4_polygons[355];
static Sint32 draw_4_vertices[367];
static NJS_CNK_MODEL draw_4_model;
static NJS_CNK_OBJECT draw_4;
static Sint16 draw_3_polygons[233];
static Sint32 draw_3_vertices[135];
static NJS_CNK_MODEL draw_3_model;
static NJS_CNK_OBJECT draw_3;
static Sint16 draw_2_polygons[99];
static Sint32 draw_2_vertices[83];
static NJS_CNK_MODEL draw_2_model;
static NJS_CNK_OBJECT draw_2;
static NJS_OBJECT collision_1;
static NJS_OBJECT collision_17;
static NJS_OBJECT collision_31;
static NJS_OBJECT collision_34;
static NJS_OBJECT collision_36;
static NJS_OBJECT collision_37;
static NJS_OBJECT collision_39;
static NJS_OBJECT collision_40;
static NJS_OBJECT collision_45;
static NJS_OBJECT collision_48;
static NJS_OBJECT collision_54;
static NJS_OBJECT collision_55;
static NJS_OBJECT collision_63;
static NJS_OBJECT collision_66;
static NJS_OBJECT collision_67;
static NJS_OBJECT collision_68;
static NJS_OBJECT collision_69;
static NJS_OBJECT collision_70;
static NJS_OBJECT collision_71;
static NJS_OBJECT collision_87;
static NJS_OBJECT collision_89;
static NJS_OBJECT collision_90;
static NJS_OBJECT collision_92;
static NJS_OBJECT collision_93;
static NJS_CNK_OBJECT draw_162;
static NJS_CNK_OBJECT draw_160;
static NJS_CNK_OBJECT draw_159;
static NJS_CNK_OBJECT draw_158;
static NJS_CNK_OBJECT draw_157;
static NJS_CNK_OBJECT draw_156;
static NJS_CNK_OBJECT draw_152;
static NJS_CNK_OBJECT draw_150;
static NJS_CNK_OBJECT draw_146;
static NJS_CNK_OBJECT draw_145;
static NJS_CNK_OBJECT draw_144;
static NJS_CNK_OBJECT draw_143;
static NJS_CNK_OBJECT draw_142;
static NJS_CNK_OBJECT draw_141;
static NJS_CNK_OBJECT draw_139;
static NJS_CNK_OBJECT draw_130;
static NJS_CNK_OBJECT draw_129;
static NJS_CNK_OBJECT draw_127;
static NJS_CNK_OBJECT draw_126;
static NJS_CNK_OBJECT draw_125;
static NJS_CNK_OBJECT draw_124;
static NJS_CNK_OBJECT draw_122;
static NJS_CNK_OBJECT draw_119;
static NJS_CNK_OBJECT draw_117;
static NJS_CNK_OBJECT draw_116;
static NJS_CNK_OBJECT draw_115;
static NJS_CNK_OBJECT draw_114;
static NJS_CNK_OBJECT draw_109;
static NJS_CNK_OBJECT draw_106;
static NJS_CNK_OBJECT draw_102;
static NJS_CNK_OBJECT draw_101;
static NJS_CNK_OBJECT draw_100;
static NJS_CNK_OBJECT draw_99;
static NJS_CNK_OBJECT draw_97;
static NJS_CNK_OBJECT draw_96;
static NJS_CNK_OBJECT draw_95;
static NJS_CNK_OBJECT draw_94;
static NJS_CNK_OBJECT draw_93;
static NJS_CNK_OBJECT draw_92;
static NJS_CNK_OBJECT draw_89;
static NJS_CNK_OBJECT draw_84;
static NJS_CNK_OBJECT draw_83;
static NJS_CNK_OBJECT draw_82;
static NJS_CNK_OBJECT draw_76;
static NJS_CNK_OBJECT draw_74;
static NJS_CNK_OBJECT draw_72;
static NJS_CNK_OBJECT draw_48;
static NJS_CNK_OBJECT draw_47;
static NJS_CNK_OBJECT draw_46;
static NJS_CNK_OBJECT draw_44;
static NJS_CNK_OBJECT draw_43;
static NJS_CNK_OBJECT draw_42;
static NJS_CNK_OBJECT draw_41;
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
static NJS_CNK_OBJECT draw_9;
static NJS_CNK_OBJECT draw_1;
static NJS_CNK_OBJECT draw_0;

static struct land_entry entries[259] = {
#include "stg06/terrain/entries.inc"
};
struct land_table sky_rail_land = {
#include "stg06/terrain/sky_rail_land.inc"
};
static NJS_TEXNAME textures[26] = {
#include "stg06/terrain/textures.inc"
};
NJS_TEXLIST sky_rail_land_texlist = {
#include "stg06/terrain/sky_rail_land_texlist.inc"
};
static NJS_MATERIAL collision_0_materials[1] = {
#include "stg06/terrain/collision_0_materials.inc"
};
static Sint16 collision_0_faces[49] = {
#include "stg06/terrain/collision_0_faces.inc"
};
static NJS_MESHSET collision_0_meshsets[1] = {
#include "stg06/terrain/collision_0_meshsets.inc"
};
static NJS_POINT3 collision_0_points[24] = {
#include "stg06/terrain/collision_0_points.inc"
};
static NJS_MODEL collision_0_model = {
#include "stg06/terrain/collision_0_model.inc"
};
static NJS_OBJECT collision_0 = {
#include "stg06/terrain/collision_0.inc"
};
static NJS_MATERIAL collision_2_materials[1] = {
#include "stg06/terrain/collision_2_materials.inc"
};
static Sint16 collision_2_faces[251] = {
#include "stg06/terrain/collision_2_faces.inc"
};
static NJS_MESHSET collision_2_meshsets[1] = {
#include "stg06/terrain/collision_2_meshsets.inc"
};
static NJS_POINT3 collision_2_points[96] = {
#include "stg06/terrain/collision_2_points.inc"
};
static NJS_MODEL collision_2_model = {
#include "stg06/terrain/collision_2_model.inc"
};
static NJS_OBJECT collision_2 = {
#include "stg06/terrain/collision_2.inc"
};
static NJS_MATERIAL collision_3_materials[1] = {
#include "stg06/terrain/collision_3_materials.inc"
};
static Sint16 collision_3_faces[174] = {
#include "stg06/terrain/collision_3_faces.inc"
};
static NJS_MESHSET collision_3_meshsets[1] = {
#include "stg06/terrain/collision_3_meshsets.inc"
};
static NJS_POINT3 collision_3_points[70] = {
#include "stg06/terrain/collision_3_points.inc"
};
static NJS_MODEL collision_3_model = {
#include "stg06/terrain/collision_3_model.inc"
};
static NJS_OBJECT collision_3 = {
#include "stg06/terrain/collision_3.inc"
};
static NJS_MATERIAL collision_4_materials[1] = {
#include "stg06/terrain/collision_4_materials.inc"
};
static Sint16 collision_4_faces[41] = {
#include "stg06/terrain/collision_4_faces.inc"
};
static NJS_MESHSET collision_4_meshsets[1] = {
#include "stg06/terrain/collision_4_meshsets.inc"
};
static NJS_POINT3 collision_4_points[19] = {
#include "stg06/terrain/collision_4_points.inc"
};
static NJS_MODEL collision_4_model = {
#include "stg06/terrain/collision_4_model.inc"
};
static NJS_OBJECT collision_4 = {
#include "stg06/terrain/collision_4.inc"
};
static NJS_MATERIAL collision_5_materials[1] = {
#include "stg06/terrain/collision_5_materials.inc"
};
static Sint16 collision_5_faces[156] = {
#include "stg06/terrain/collision_5_faces.inc"
};
static NJS_MESHSET collision_5_meshsets[1] = {
#include "stg06/terrain/collision_5_meshsets.inc"
};
static NJS_POINT3 collision_5_points[75] = {
#include "stg06/terrain/collision_5_points.inc"
};
static NJS_MODEL collision_5_model = {
#include "stg06/terrain/collision_5_model.inc"
};
static NJS_OBJECT collision_5 = {
#include "stg06/terrain/collision_5.inc"
};
static NJS_MATERIAL collision_6_materials[1] = {
#include "stg06/terrain/collision_6_materials.inc"
};
static Sint16 collision_6_faces[76] = {
#include "stg06/terrain/collision_6_faces.inc"
};
static NJS_MESHSET collision_6_meshsets[1] = {
#include "stg06/terrain/collision_6_meshsets.inc"
};
static NJS_POINT3 collision_6_points[36] = {
#include "stg06/terrain/collision_6_points.inc"
};
static NJS_MODEL collision_6_model = {
#include "stg06/terrain/collision_6_model.inc"
};
static NJS_OBJECT collision_6 = {
#include "stg06/terrain/collision_6.inc"
};
static NJS_MATERIAL collision_7_materials[1] = {
#include "stg06/terrain/collision_7_materials.inc"
};
static Sint16 collision_7_faces[48] = {
#include "stg06/terrain/collision_7_faces.inc"
};
static NJS_MESHSET collision_7_meshsets[1] = {
#include "stg06/terrain/collision_7_meshsets.inc"
};
static NJS_POINT3 collision_7_points[22] = {
#include "stg06/terrain/collision_7_points.inc"
};
static NJS_MODEL collision_7_model = {
#include "stg06/terrain/collision_7_model.inc"
};
static NJS_OBJECT collision_7 = {
#include "stg06/terrain/collision_7.inc"
};
static NJS_MATERIAL collision_8_materials[1] = {
#include "stg06/terrain/collision_8_materials.inc"
};
static Sint16 collision_8_faces[10] = {
#include "stg06/terrain/collision_8_faces.inc"
};
static NJS_MESHSET collision_8_meshsets[1] = {
#include "stg06/terrain/collision_8_meshsets.inc"
};
static NJS_POINT3 collision_8_points[8] = {
#include "stg06/terrain/collision_8_points.inc"
};
static NJS_MODEL collision_8_model = {
#include "stg06/terrain/collision_8_model.inc"
};
static NJS_OBJECT collision_8 = {
#include "stg06/terrain/collision_8.inc"
};
static NJS_MATERIAL collision_9_materials[1] = {
#include "stg06/terrain/collision_9_materials.inc"
};
static Sint16 collision_9_faces[238] = {
#include "stg06/terrain/collision_9_faces.inc"
};
static NJS_MESHSET collision_9_meshsets[1] = {
#include "stg06/terrain/collision_9_meshsets.inc"
};
static NJS_POINT3 collision_9_points[97] = {
#include "stg06/terrain/collision_9_points.inc"
};
static NJS_MODEL collision_9_model = {
#include "stg06/terrain/collision_9_model.inc"
};
static NJS_OBJECT collision_9 = {
#include "stg06/terrain/collision_9.inc"
};
static NJS_MATERIAL collision_10_materials[1] = {
#include "stg06/terrain/collision_10_materials.inc"
};
static Sint16 collision_10_faces[38] = {
#include "stg06/terrain/collision_10_faces.inc"
};
static NJS_MESHSET collision_10_meshsets[1] = {
#include "stg06/terrain/collision_10_meshsets.inc"
};
static NJS_POINT3 collision_10_points[20] = {
#include "stg06/terrain/collision_10_points.inc"
};
static NJS_MODEL collision_10_model = {
#include "stg06/terrain/collision_10_model.inc"
};
static NJS_OBJECT collision_10 = {
#include "stg06/terrain/collision_10.inc"
};
static NJS_MATERIAL collision_11_materials[1] = {
#include "stg06/terrain/collision_11_materials.inc"
};
static Sint16 collision_11_faces[20] = {
#include "stg06/terrain/collision_11_faces.inc"
};
static NJS_MESHSET collision_11_meshsets[1] = {
#include "stg06/terrain/collision_11_meshsets.inc"
};
static NJS_POINT3 collision_11_points[12] = {
#include "stg06/terrain/collision_11_points.inc"
};
static NJS_MODEL collision_11_model = {
#include "stg06/terrain/collision_11_model.inc"
};
static NJS_OBJECT collision_11 = {
#include "stg06/terrain/collision_11.inc"
};
static NJS_MATERIAL collision_12_materials[1] = {
#include "stg06/terrain/collision_12_materials.inc"
};
static Sint16 collision_12_faces[67] = {
#include "stg06/terrain/collision_12_faces.inc"
};
static NJS_MESHSET collision_12_meshsets[1] = {
#include "stg06/terrain/collision_12_meshsets.inc"
};
static NJS_POINT3 collision_12_points[30] = {
#include "stg06/terrain/collision_12_points.inc"
};
static NJS_MODEL collision_12_model = {
#include "stg06/terrain/collision_12_model.inc"
};
static NJS_OBJECT collision_12 = {
#include "stg06/terrain/collision_12.inc"
};
static NJS_MATERIAL collision_13_materials[1] = {
#include "stg06/terrain/collision_13_materials.inc"
};
static Sint16 collision_13_faces[58] = {
#include "stg06/terrain/collision_13_faces.inc"
};
static NJS_MESHSET collision_13_meshsets[1] = {
#include "stg06/terrain/collision_13_meshsets.inc"
};
static NJS_POINT3 collision_13_points[27] = {
#include "stg06/terrain/collision_13_points.inc"
};
static NJS_MODEL collision_13_model = {
#include "stg06/terrain/collision_13_model.inc"
};
static NJS_OBJECT collision_13 = {
#include "stg06/terrain/collision_13.inc"
};
static NJS_MATERIAL collision_14_materials[1] = {
#include "stg06/terrain/collision_14_materials.inc"
};
static Sint16 collision_14_faces[78] = {
#include "stg06/terrain/collision_14_faces.inc"
};
static NJS_MESHSET collision_14_meshsets[1] = {
#include "stg06/terrain/collision_14_meshsets.inc"
};
static NJS_POINT3 collision_14_points[36] = {
#include "stg06/terrain/collision_14_points.inc"
};
static NJS_MODEL collision_14_model = {
#include "stg06/terrain/collision_14_model.inc"
};
static NJS_OBJECT collision_14 = {
#include "stg06/terrain/collision_14.inc"
};
static NJS_MATERIAL collision_15_materials[1] = {
#include "stg06/terrain/collision_15_materials.inc"
};
static Sint16 collision_15_faces[89] = {
#include "stg06/terrain/collision_15_faces.inc"
};
static NJS_MESHSET collision_15_meshsets[1] = {
#include "stg06/terrain/collision_15_meshsets.inc"
};
static NJS_POINT3 collision_15_points[36] = {
#include "stg06/terrain/collision_15_points.inc"
};
static NJS_MODEL collision_15_model = {
#include "stg06/terrain/collision_15_model.inc"
};
static NJS_OBJECT collision_15 = {
#include "stg06/terrain/collision_15.inc"
};
static NJS_MATERIAL collision_16_materials[1] = {
#include "stg06/terrain/collision_16_materials.inc"
};
static Sint16 collision_16_faces[75] = {
#include "stg06/terrain/collision_16_faces.inc"
};
static NJS_MESHSET collision_16_meshsets[1] = {
#include "stg06/terrain/collision_16_meshsets.inc"
};
static NJS_POINT3 collision_16_points[32] = {
#include "stg06/terrain/collision_16_points.inc"
};
static NJS_MODEL collision_16_model = {
#include "stg06/terrain/collision_16_model.inc"
};
static NJS_OBJECT collision_16 = {
#include "stg06/terrain/collision_16.inc"
};
static NJS_MATERIAL collision_18_materials[1] = {
#include "stg06/terrain/collision_18_materials.inc"
};
static Sint16 collision_18_faces[119] = {
#include "stg06/terrain/collision_18_faces.inc"
};
static NJS_MESHSET collision_18_meshsets[1] = {
#include "stg06/terrain/collision_18_meshsets.inc"
};
static NJS_POINT3 collision_18_points[48] = {
#include "stg06/terrain/collision_18_points.inc"
};
static NJS_MODEL collision_18_model = {
#include "stg06/terrain/collision_18_model.inc"
};
static NJS_OBJECT collision_18 = {
#include "stg06/terrain/collision_18.inc"
};
static NJS_MATERIAL collision_19_materials[1] = {
#include "stg06/terrain/collision_19_materials.inc"
};
static Sint16 collision_19_faces[36] = {
#include "stg06/terrain/collision_19_faces.inc"
};
static NJS_MESHSET collision_19_meshsets[1] = {
#include "stg06/terrain/collision_19_meshsets.inc"
};
static NJS_POINT3 collision_19_points[17] = {
#include "stg06/terrain/collision_19_points.inc"
};
static NJS_MODEL collision_19_model = {
#include "stg06/terrain/collision_19_model.inc"
};
static NJS_OBJECT collision_19 = {
#include "stg06/terrain/collision_19.inc"
};
static NJS_MATERIAL collision_20_materials[1] = {
#include "stg06/terrain/collision_20_materials.inc"
};
static Sint16 collision_20_faces[39] = {
#include "stg06/terrain/collision_20_faces.inc"
};
static NJS_MESHSET collision_20_meshsets[1] = {
#include "stg06/terrain/collision_20_meshsets.inc"
};
static NJS_POINT3 collision_20_points[17] = {
#include "stg06/terrain/collision_20_points.inc"
};
static NJS_MODEL collision_20_model = {
#include "stg06/terrain/collision_20_model.inc"
};
static NJS_OBJECT collision_20 = {
#include "stg06/terrain/collision_20.inc"
};
static NJS_MATERIAL collision_21_materials[1] = {
#include "stg06/terrain/collision_21_materials.inc"
};
static Sint16 collision_21_faces[157] = {
#include "stg06/terrain/collision_21_faces.inc"
};
static NJS_MESHSET collision_21_meshsets[1] = {
#include "stg06/terrain/collision_21_meshsets.inc"
};
static NJS_POINT3 collision_21_points[74] = {
#include "stg06/terrain/collision_21_points.inc"
};
static NJS_MODEL collision_21_model = {
#include "stg06/terrain/collision_21_model.inc"
};
static NJS_OBJECT collision_21 = {
#include "stg06/terrain/collision_21.inc"
};
static NJS_MATERIAL collision_22_materials[1] = {
#include "stg06/terrain/collision_22_materials.inc"
};
static Sint16 collision_22_faces[39] = {
#include "stg06/terrain/collision_22_faces.inc"
};
static NJS_MESHSET collision_22_meshsets[1] = {
#include "stg06/terrain/collision_22_meshsets.inc"
};
static NJS_POINT3 collision_22_points[17] = {
#include "stg06/terrain/collision_22_points.inc"
};
static NJS_MODEL collision_22_model = {
#include "stg06/terrain/collision_22_model.inc"
};
static NJS_OBJECT collision_22 = {
#include "stg06/terrain/collision_22.inc"
};
static NJS_MATERIAL collision_23_materials[1] = {
#include "stg06/terrain/collision_23_materials.inc"
};
static Sint16 collision_23_faces[143] = {
#include "stg06/terrain/collision_23_faces.inc"
};
static NJS_MESHSET collision_23_meshsets[1] = {
#include "stg06/terrain/collision_23_meshsets.inc"
};
static NJS_POINT3 collision_23_points[56] = {
#include "stg06/terrain/collision_23_points.inc"
};
static NJS_MODEL collision_23_model = {
#include "stg06/terrain/collision_23_model.inc"
};
static NJS_OBJECT collision_23 = {
#include "stg06/terrain/collision_23.inc"
};
static NJS_MATERIAL collision_24_materials[1] = {
#include "stg06/terrain/collision_24_materials.inc"
};
static Sint16 collision_24_faces[125] = {
#include "stg06/terrain/collision_24_faces.inc"
};
static NJS_MESHSET collision_24_meshsets[1] = {
#include "stg06/terrain/collision_24_meshsets.inc"
};
static NJS_POINT3 collision_24_points[50] = {
#include "stg06/terrain/collision_24_points.inc"
};
static NJS_MODEL collision_24_model = {
#include "stg06/terrain/collision_24_model.inc"
};
static NJS_OBJECT collision_24 = {
#include "stg06/terrain/collision_24.inc"
};
static NJS_MATERIAL collision_25_materials[1] = {
#include "stg06/terrain/collision_25_materials.inc"
};
static Sint16 collision_25_faces[16] = {
#include "stg06/terrain/collision_25_faces.inc"
};
static NJS_MESHSET collision_25_meshsets[1] = {
#include "stg06/terrain/collision_25_meshsets.inc"
};
static NJS_POINT3 collision_25_points[8] = {
#include "stg06/terrain/collision_25_points.inc"
};
static NJS_MODEL collision_25_model = {
#include "stg06/terrain/collision_25_model.inc"
};
static NJS_OBJECT collision_25 = {
#include "stg06/terrain/collision_25.inc"
};
static NJS_MATERIAL collision_26_materials[1] = {
#include "stg06/terrain/collision_26_materials.inc"
};
static Sint16 collision_26_faces[16] = {
#include "stg06/terrain/collision_26_faces.inc"
};
static NJS_MESHSET collision_26_meshsets[1] = {
#include "stg06/terrain/collision_26_meshsets.inc"
};
static NJS_POINT3 collision_26_points[8] = {
#include "stg06/terrain/collision_26_points.inc"
};
static NJS_MODEL collision_26_model = {
#include "stg06/terrain/collision_26_model.inc"
};
static NJS_OBJECT collision_26 = {
#include "stg06/terrain/collision_26.inc"
};
static NJS_MATERIAL collision_27_materials[1] = {
#include "stg06/terrain/collision_27_materials.inc"
};
static Sint16 collision_27_faces[19] = {
#include "stg06/terrain/collision_27_faces.inc"
};
static NJS_MESHSET collision_27_meshsets[1] = {
#include "stg06/terrain/collision_27_meshsets.inc"
};
static NJS_POINT3 collision_27_points[8] = {
#include "stg06/terrain/collision_27_points.inc"
};
static NJS_MODEL collision_27_model = {
#include "stg06/terrain/collision_27_model.inc"
};
static NJS_OBJECT collision_27 = {
#include "stg06/terrain/collision_27.inc"
};
static NJS_MATERIAL collision_28_materials[1] = {
#include "stg06/terrain/collision_28_materials.inc"
};
static Sint16 collision_28_faces[16] = {
#include "stg06/terrain/collision_28_faces.inc"
};
static NJS_MESHSET collision_28_meshsets[1] = {
#include "stg06/terrain/collision_28_meshsets.inc"
};
static NJS_POINT3 collision_28_points[8] = {
#include "stg06/terrain/collision_28_points.inc"
};
static NJS_MODEL collision_28_model = {
#include "stg06/terrain/collision_28_model.inc"
};
static NJS_OBJECT collision_28 = {
#include "stg06/terrain/collision_28.inc"
};
static NJS_MATERIAL collision_29_materials[1] = {
#include "stg06/terrain/collision_29_materials.inc"
};
static Sint16 collision_29_faces[19] = {
#include "stg06/terrain/collision_29_faces.inc"
};
static NJS_MESHSET collision_29_meshsets[1] = {
#include "stg06/terrain/collision_29_meshsets.inc"
};
static NJS_POINT3 collision_29_points[8] = {
#include "stg06/terrain/collision_29_points.inc"
};
static NJS_MODEL collision_29_model = {
#include "stg06/terrain/collision_29_model.inc"
};
static NJS_OBJECT collision_29 = {
#include "stg06/terrain/collision_29.inc"
};
static NJS_MATERIAL collision_30_materials[1] = {
#include "stg06/terrain/collision_30_materials.inc"
};
static Sint16 collision_30_faces[128] = {
#include "stg06/terrain/collision_30_faces.inc"
};
static NJS_MESHSET collision_30_meshsets[1] = {
#include "stg06/terrain/collision_30_meshsets.inc"
};
static NJS_POINT3 collision_30_points[50] = {
#include "stg06/terrain/collision_30_points.inc"
};
static NJS_MODEL collision_30_model = {
#include "stg06/terrain/collision_30_model.inc"
};
static NJS_OBJECT collision_30 = {
#include "stg06/terrain/collision_30.inc"
};
static NJS_MATERIAL collision_32_materials[1] = {
#include "stg06/terrain/collision_32_materials.inc"
};
static Sint16 collision_32_faces[117] = {
#include "stg06/terrain/collision_32_faces.inc"
};
static NJS_MESHSET collision_32_meshsets[1] = {
#include "stg06/terrain/collision_32_meshsets.inc"
};
static NJS_POINT3 collision_32_points[56] = {
#include "stg06/terrain/collision_32_points.inc"
};
static NJS_MODEL collision_32_model = {
#include "stg06/terrain/collision_32_model.inc"
};
static NJS_OBJECT collision_32 = {
#include "stg06/terrain/collision_32.inc"
};
static NJS_MATERIAL collision_33_materials[1] = {
#include "stg06/terrain/collision_33_materials.inc"
};
static Sint16 collision_33_faces[21] = {
#include "stg06/terrain/collision_33_faces.inc"
};
static NJS_MESHSET collision_33_meshsets[1] = {
#include "stg06/terrain/collision_33_meshsets.inc"
};
static NJS_POINT3 collision_33_points[16] = {
#include "stg06/terrain/collision_33_points.inc"
};
static NJS_MODEL collision_33_model = {
#include "stg06/terrain/collision_33_model.inc"
};
static NJS_OBJECT collision_33 = {
#include "stg06/terrain/collision_33.inc"
};
static NJS_MATERIAL collision_35_materials[1] = {
#include "stg06/terrain/collision_35_materials.inc"
};
static Sint16 collision_35_faces[17] = {
#include "stg06/terrain/collision_35_faces.inc"
};
static NJS_MESHSET collision_35_meshsets[1] = {
#include "stg06/terrain/collision_35_meshsets.inc"
};
static NJS_POINT3 collision_35_points[12] = {
#include "stg06/terrain/collision_35_points.inc"
};
static NJS_MODEL collision_35_model = {
#include "stg06/terrain/collision_35_model.inc"
};
static NJS_OBJECT collision_35 = {
#include "stg06/terrain/collision_35.inc"
};
static NJS_MATERIAL collision_38_materials[1] = {
#include "stg06/terrain/collision_38_materials.inc"
};
static Sint16 collision_38_faces[11] = {
#include "stg06/terrain/collision_38_faces.inc"
};
static NJS_MESHSET collision_38_meshsets[1] = {
#include "stg06/terrain/collision_38_meshsets.inc"
};
static NJS_POINT3 collision_38_points[9] = {
#include "stg06/terrain/collision_38_points.inc"
};
static NJS_MODEL collision_38_model = {
#include "stg06/terrain/collision_38_model.inc"
};
static NJS_OBJECT collision_38 = {
#include "stg06/terrain/collision_38.inc"
};
static NJS_MATERIAL collision_41_materials[1] = {
#include "stg06/terrain/collision_41_materials.inc"
};
static Sint16 collision_41_faces[66] = {
#include "stg06/terrain/collision_41_faces.inc"
};
static NJS_MESHSET collision_41_meshsets[1] = {
#include "stg06/terrain/collision_41_meshsets.inc"
};
static NJS_POINT3 collision_41_points[33] = {
#include "stg06/terrain/collision_41_points.inc"
};
static NJS_MODEL collision_41_model = {
#include "stg06/terrain/collision_41_model.inc"
};
static NJS_OBJECT collision_41 = {
#include "stg06/terrain/collision_41.inc"
};
static NJS_MATERIAL collision_42_materials[1] = {
#include "stg06/terrain/collision_42_materials.inc"
};
static Sint16 collision_42_faces[66] = {
#include "stg06/terrain/collision_42_faces.inc"
};
static NJS_MESHSET collision_42_meshsets[1] = {
#include "stg06/terrain/collision_42_meshsets.inc"
};
static NJS_POINT3 collision_42_points[33] = {
#include "stg06/terrain/collision_42_points.inc"
};
static NJS_MODEL collision_42_model = {
#include "stg06/terrain/collision_42_model.inc"
};
static NJS_OBJECT collision_42 = {
#include "stg06/terrain/collision_42.inc"
};
static NJS_MATERIAL collision_43_materials[1] = {
#include "stg06/terrain/collision_43_materials.inc"
};
static Sint16 collision_43_faces[60] = {
#include "stg06/terrain/collision_43_faces.inc"
};
static NJS_MESHSET collision_43_meshsets[1] = {
#include "stg06/terrain/collision_43_meshsets.inc"
};
static NJS_POINT3 collision_43_points[30] = {
#include "stg06/terrain/collision_43_points.inc"
};
static NJS_MODEL collision_43_model = {
#include "stg06/terrain/collision_43_model.inc"
};
static NJS_OBJECT collision_43 = {
#include "stg06/terrain/collision_43.inc"
};
static NJS_MATERIAL collision_44_materials[1] = {
#include "stg06/terrain/collision_44_materials.inc"
};
static Sint16 collision_44_faces[72] = {
#include "stg06/terrain/collision_44_faces.inc"
};
static NJS_MESHSET collision_44_meshsets[1] = {
#include "stg06/terrain/collision_44_meshsets.inc"
};
static NJS_POINT3 collision_44_points[30] = {
#include "stg06/terrain/collision_44_points.inc"
};
static NJS_MODEL collision_44_model = {
#include "stg06/terrain/collision_44_model.inc"
};
static NJS_OBJECT collision_44 = {
#include "stg06/terrain/collision_44.inc"
};
static NJS_MATERIAL collision_46_materials[1] = {
#include "stg06/terrain/collision_46_materials.inc"
};
static Sint16 collision_46_faces[78] = {
#include "stg06/terrain/collision_46_faces.inc"
};
static NJS_MESHSET collision_46_meshsets[1] = {
#include "stg06/terrain/collision_46_meshsets.inc"
};
static NJS_POINT3 collision_46_points[36] = {
#include "stg06/terrain/collision_46_points.inc"
};
static NJS_MODEL collision_46_model = {
#include "stg06/terrain/collision_46_model.inc"
};
static NJS_OBJECT collision_46 = {
#include "stg06/terrain/collision_46.inc"
};
static NJS_MATERIAL collision_47_materials[1] = {
#include "stg06/terrain/collision_47_materials.inc"
};
static Sint16 collision_47_faces[75] = {
#include "stg06/terrain/collision_47_faces.inc"
};
static NJS_MESHSET collision_47_meshsets[1] = {
#include "stg06/terrain/collision_47_meshsets.inc"
};
static NJS_POINT3 collision_47_points[36] = {
#include "stg06/terrain/collision_47_points.inc"
};
static NJS_MODEL collision_47_model = {
#include "stg06/terrain/collision_47_model.inc"
};
static NJS_OBJECT collision_47 = {
#include "stg06/terrain/collision_47.inc"
};
static NJS_MATERIAL collision_49_materials[1] = {
#include "stg06/terrain/collision_49_materials.inc"
};
static Sint16 collision_49_faces[121] = {
#include "stg06/terrain/collision_49_faces.inc"
};
static NJS_MESHSET collision_49_meshsets[1] = {
#include "stg06/terrain/collision_49_meshsets.inc"
};
static NJS_POINT3 collision_49_points[51] = {
#include "stg06/terrain/collision_49_points.inc"
};
static NJS_MODEL collision_49_model = {
#include "stg06/terrain/collision_49_model.inc"
};
static NJS_OBJECT collision_49 = {
#include "stg06/terrain/collision_49.inc"
};
static NJS_MATERIAL collision_50_materials[1] = {
#include "stg06/terrain/collision_50_materials.inc"
};
static Sint16 collision_50_faces[180] = {
#include "stg06/terrain/collision_50_faces.inc"
};
static NJS_MESHSET collision_50_meshsets[1] = {
#include "stg06/terrain/collision_50_meshsets.inc"
};
static NJS_POINT3 collision_50_points[93] = {
#include "stg06/terrain/collision_50_points.inc"
};
static NJS_MODEL collision_50_model = {
#include "stg06/terrain/collision_50_model.inc"
};
static NJS_OBJECT collision_50 = {
#include "stg06/terrain/collision_50.inc"
};
static NJS_MATERIAL collision_51_materials[1] = {
#include "stg06/terrain/collision_51_materials.inc"
};
static Sint16 collision_51_faces[321] = {
#include "stg06/terrain/collision_51_faces.inc"
};
static NJS_MESHSET collision_51_meshsets[1] = {
#include "stg06/terrain/collision_51_meshsets.inc"
};
static NJS_POINT3 collision_51_points[135] = {
#include "stg06/terrain/collision_51_points.inc"
};
static NJS_MODEL collision_51_model = {
#include "stg06/terrain/collision_51_model.inc"
};
static NJS_OBJECT collision_51 = {
#include "stg06/terrain/collision_51.inc"
};
static NJS_MATERIAL collision_52_materials[1] = {
#include "stg06/terrain/collision_52_materials.inc"
};
static Sint16 collision_52_faces[246] = {
#include "stg06/terrain/collision_52_faces.inc"
};
static NJS_MESHSET collision_52_meshsets[1] = {
#include "stg06/terrain/collision_52_meshsets.inc"
};
static NJS_POINT3 collision_52_points[125] = {
#include "stg06/terrain/collision_52_points.inc"
};
static NJS_MODEL collision_52_model = {
#include "stg06/terrain/collision_52_model.inc"
};
static NJS_OBJECT collision_52 = {
#include "stg06/terrain/collision_52.inc"
};
static NJS_MATERIAL collision_53_materials[1] = {
#include "stg06/terrain/collision_53_materials.inc"
};
static Sint16 collision_53_faces[18] = {
#include "stg06/terrain/collision_53_faces.inc"
};
static NJS_MESHSET collision_53_meshsets[1] = {
#include "stg06/terrain/collision_53_meshsets.inc"
};
static NJS_POINT3 collision_53_points[9] = {
#include "stg06/terrain/collision_53_points.inc"
};
static NJS_MODEL collision_53_model = {
#include "stg06/terrain/collision_53_model.inc"
};
static NJS_OBJECT collision_53 = {
#include "stg06/terrain/collision_53.inc"
};
static NJS_MATERIAL collision_56_materials[1] = {
#include "stg06/terrain/collision_56_materials.inc"
};
static Sint16 collision_56_faces[18] = {
#include "stg06/terrain/collision_56_faces.inc"
};
static NJS_MESHSET collision_56_meshsets[1] = {
#include "stg06/terrain/collision_56_meshsets.inc"
};
static NJS_POINT3 collision_56_points[9] = {
#include "stg06/terrain/collision_56_points.inc"
};
static NJS_MODEL collision_56_model = {
#include "stg06/terrain/collision_56_model.inc"
};
static NJS_OBJECT collision_56 = {
#include "stg06/terrain/collision_56.inc"
};
static NJS_MATERIAL collision_57_materials[1] = {
#include "stg06/terrain/collision_57_materials.inc"
};
static Sint16 collision_57_faces[18] = {
#include "stg06/terrain/collision_57_faces.inc"
};
static NJS_MESHSET collision_57_meshsets[1] = {
#include "stg06/terrain/collision_57_meshsets.inc"
};
static NJS_POINT3 collision_57_points[9] = {
#include "stg06/terrain/collision_57_points.inc"
};
static NJS_MODEL collision_57_model = {
#include "stg06/terrain/collision_57_model.inc"
};
static NJS_OBJECT collision_57 = {
#include "stg06/terrain/collision_57.inc"
};
static NJS_MATERIAL collision_58_materials[1] = {
#include "stg06/terrain/collision_58_materials.inc"
};
static Sint16 collision_58_faces[20] = {
#include "stg06/terrain/collision_58_faces.inc"
};
static NJS_MESHSET collision_58_meshsets[1] = {
#include "stg06/terrain/collision_58_meshsets.inc"
};
static NJS_POINT3 collision_58_points[11] = {
#include "stg06/terrain/collision_58_points.inc"
};
static NJS_MODEL collision_58_model = {
#include "stg06/terrain/collision_58_model.inc"
};
static NJS_OBJECT collision_58 = {
#include "stg06/terrain/collision_58.inc"
};
static NJS_MATERIAL collision_59_materials[1] = {
#include "stg06/terrain/collision_59_materials.inc"
};
static Sint16 collision_59_faces[18] = {
#include "stg06/terrain/collision_59_faces.inc"
};
static NJS_MESHSET collision_59_meshsets[1] = {
#include "stg06/terrain/collision_59_meshsets.inc"
};
static NJS_POINT3 collision_59_points[9] = {
#include "stg06/terrain/collision_59_points.inc"
};
static NJS_MODEL collision_59_model = {
#include "stg06/terrain/collision_59_model.inc"
};
static NJS_OBJECT collision_59 = {
#include "stg06/terrain/collision_59.inc"
};
static NJS_MATERIAL collision_60_materials[1] = {
#include "stg06/terrain/collision_60_materials.inc"
};
static Sint16 collision_60_faces[18] = {
#include "stg06/terrain/collision_60_faces.inc"
};
static NJS_MESHSET collision_60_meshsets[1] = {
#include "stg06/terrain/collision_60_meshsets.inc"
};
static NJS_POINT3 collision_60_points[9] = {
#include "stg06/terrain/collision_60_points.inc"
};
static NJS_MODEL collision_60_model = {
#include "stg06/terrain/collision_60_model.inc"
};
static NJS_OBJECT collision_60 = {
#include "stg06/terrain/collision_60.inc"
};
static NJS_MATERIAL collision_61_materials[1] = {
#include "stg06/terrain/collision_61_materials.inc"
};
static Sint16 collision_61_faces[18] = {
#include "stg06/terrain/collision_61_faces.inc"
};
static NJS_MESHSET collision_61_meshsets[1] = {
#include "stg06/terrain/collision_61_meshsets.inc"
};
static NJS_POINT3 collision_61_points[9] = {
#include "stg06/terrain/collision_61_points.inc"
};
static NJS_MODEL collision_61_model = {
#include "stg06/terrain/collision_61_model.inc"
};
static NJS_OBJECT collision_61 = {
#include "stg06/terrain/collision_61.inc"
};
static NJS_MATERIAL collision_62_materials[1] = {
#include "stg06/terrain/collision_62_materials.inc"
};
static Sint16 collision_62_faces[9] = {
#include "stg06/terrain/collision_62_faces.inc"
};
static NJS_MESHSET collision_62_meshsets[1] = {
#include "stg06/terrain/collision_62_meshsets.inc"
};
static NJS_POINT3 collision_62_points[6] = {
#include "stg06/terrain/collision_62_points.inc"
};
static NJS_MODEL collision_62_model = {
#include "stg06/terrain/collision_62_model.inc"
};
static NJS_OBJECT collision_62 = {
#include "stg06/terrain/collision_62.inc"
};
static NJS_MATERIAL collision_64_materials[1] = {
#include "stg06/terrain/collision_64_materials.inc"
};
static Sint16 collision_64_faces[18] = {
#include "stg06/terrain/collision_64_faces.inc"
};
static NJS_MESHSET collision_64_meshsets[1] = {
#include "stg06/terrain/collision_64_meshsets.inc"
};
static NJS_POINT3 collision_64_points[9] = {
#include "stg06/terrain/collision_64_points.inc"
};
static NJS_MODEL collision_64_model = {
#include "stg06/terrain/collision_64_model.inc"
};
static NJS_OBJECT collision_64 = {
#include "stg06/terrain/collision_64.inc"
};
static NJS_MATERIAL collision_65_materials[1] = {
#include "stg06/terrain/collision_65_materials.inc"
};
static Sint16 collision_65_faces[64] = {
#include "stg06/terrain/collision_65_faces.inc"
};
static NJS_MESHSET collision_65_meshsets[1] = {
#include "stg06/terrain/collision_65_meshsets.inc"
};
static NJS_POINT3 collision_65_points[28] = {
#include "stg06/terrain/collision_65_points.inc"
};
static NJS_MODEL collision_65_model = {
#include "stg06/terrain/collision_65_model.inc"
};
static NJS_OBJECT collision_65 = {
#include "stg06/terrain/collision_65.inc"
};
static NJS_MATERIAL collision_72_materials[1] = {
#include "stg06/terrain/collision_72_materials.inc"
};
static Sint16 collision_72_faces[74] = {
#include "stg06/terrain/collision_72_faces.inc"
};
static NJS_MESHSET collision_72_meshsets[1] = {
#include "stg06/terrain/collision_72_meshsets.inc"
};
static NJS_POINT3 collision_72_points[32] = {
#include "stg06/terrain/collision_72_points.inc"
};
static NJS_MODEL collision_72_model = {
#include "stg06/terrain/collision_72_model.inc"
};
static NJS_OBJECT collision_72 = {
#include "stg06/terrain/collision_72.inc"
};
static NJS_MATERIAL collision_73_materials[1] = {
#include "stg06/terrain/collision_73_materials.inc"
};
static Sint16 collision_73_faces[74] = {
#include "stg06/terrain/collision_73_faces.inc"
};
static NJS_MESHSET collision_73_meshsets[1] = {
#include "stg06/terrain/collision_73_meshsets.inc"
};
static NJS_POINT3 collision_73_points[32] = {
#include "stg06/terrain/collision_73_points.inc"
};
static NJS_MODEL collision_73_model = {
#include "stg06/terrain/collision_73_model.inc"
};
static NJS_OBJECT collision_73 = {
#include "stg06/terrain/collision_73.inc"
};
static NJS_MATERIAL collision_74_materials[1] = {
#include "stg06/terrain/collision_74_materials.inc"
};
static Sint16 collision_74_faces[9] = {
#include "stg06/terrain/collision_74_faces.inc"
};
static NJS_MESHSET collision_74_meshsets[1] = {
#include "stg06/terrain/collision_74_meshsets.inc"
};
static NJS_POINT3 collision_74_points[6] = {
#include "stg06/terrain/collision_74_points.inc"
};
static NJS_MODEL collision_74_model = {
#include "stg06/terrain/collision_74_model.inc"
};
static NJS_OBJECT collision_74 = {
#include "stg06/terrain/collision_74.inc"
};
static NJS_MATERIAL collision_75_materials[1] = {
#include "stg06/terrain/collision_75_materials.inc"
};
static Sint16 collision_75_faces[74] = {
#include "stg06/terrain/collision_75_faces.inc"
};
static NJS_MESHSET collision_75_meshsets[1] = {
#include "stg06/terrain/collision_75_meshsets.inc"
};
static NJS_POINT3 collision_75_points[32] = {
#include "stg06/terrain/collision_75_points.inc"
};
static NJS_MODEL collision_75_model = {
#include "stg06/terrain/collision_75_model.inc"
};
static NJS_OBJECT collision_75 = {
#include "stg06/terrain/collision_75.inc"
};
static NJS_MATERIAL collision_76_materials[1] = {
#include "stg06/terrain/collision_76_materials.inc"
};
static Sint16 collision_76_faces[64] = {
#include "stg06/terrain/collision_76_faces.inc"
};
static NJS_MESHSET collision_76_meshsets[1] = {
#include "stg06/terrain/collision_76_meshsets.inc"
};
static NJS_POINT3 collision_76_points[28] = {
#include "stg06/terrain/collision_76_points.inc"
};
static NJS_MODEL collision_76_model = {
#include "stg06/terrain/collision_76_model.inc"
};
static NJS_OBJECT collision_76 = {
#include "stg06/terrain/collision_76.inc"
};
static NJS_MATERIAL collision_77_materials[1] = {
#include "stg06/terrain/collision_77_materials.inc"
};
static Sint16 collision_77_faces[9] = {
#include "stg06/terrain/collision_77_faces.inc"
};
static NJS_MESHSET collision_77_meshsets[1] = {
#include "stg06/terrain/collision_77_meshsets.inc"
};
static NJS_POINT3 collision_77_points[6] = {
#include "stg06/terrain/collision_77_points.inc"
};
static NJS_MODEL collision_77_model = {
#include "stg06/terrain/collision_77_model.inc"
};
static NJS_OBJECT collision_77 = {
#include "stg06/terrain/collision_77.inc"
};
static NJS_MATERIAL collision_78_materials[1] = {
#include "stg06/terrain/collision_78_materials.inc"
};
static Sint16 collision_78_faces[7] = {
#include "stg06/terrain/collision_78_faces.inc"
};
static NJS_MESHSET collision_78_meshsets[1] = {
#include "stg06/terrain/collision_78_meshsets.inc"
};
static NJS_POINT3 collision_78_points[6] = {
#include "stg06/terrain/collision_78_points.inc"
};
static NJS_MODEL collision_78_model = {
#include "stg06/terrain/collision_78_model.inc"
};
static NJS_OBJECT collision_78 = {
#include "stg06/terrain/collision_78.inc"
};
static NJS_MATERIAL collision_79_materials[1] = {
#include "stg06/terrain/collision_79_materials.inc"
};
static Sint16 collision_79_faces[48] = {
#include "stg06/terrain/collision_79_faces.inc"
};
static NJS_MESHSET collision_79_meshsets[1] = {
#include "stg06/terrain/collision_79_meshsets.inc"
};
static NJS_POINT3 collision_79_points[24] = {
#include "stg06/terrain/collision_79_points.inc"
};
static NJS_MODEL collision_79_model = {
#include "stg06/terrain/collision_79_model.inc"
};
static NJS_OBJECT collision_79 = {
#include "stg06/terrain/collision_79.inc"
};
static NJS_MATERIAL collision_80_materials[1] = {
#include "stg06/terrain/collision_80_materials.inc"
};
static Sint16 collision_80_faces[34] = {
#include "stg06/terrain/collision_80_faces.inc"
};
static NJS_MESHSET collision_80_meshsets[1] = {
#include "stg06/terrain/collision_80_meshsets.inc"
};
static NJS_POINT3 collision_80_points[28] = {
#include "stg06/terrain/collision_80_points.inc"
};
static NJS_MODEL collision_80_model = {
#include "stg06/terrain/collision_80_model.inc"
};
static NJS_OBJECT collision_80 = {
#include "stg06/terrain/collision_80.inc"
};
static NJS_MATERIAL collision_81_materials[1] = {
#include "stg06/terrain/collision_81_materials.inc"
};
static Sint16 collision_81_faces[22] = {
#include "stg06/terrain/collision_81_faces.inc"
};
static NJS_MESHSET collision_81_meshsets[1] = {
#include "stg06/terrain/collision_81_meshsets.inc"
};
static NJS_POINT3 collision_81_points[12] = {
#include "stg06/terrain/collision_81_points.inc"
};
static NJS_MODEL collision_81_model = {
#include "stg06/terrain/collision_81_model.inc"
};
static NJS_OBJECT collision_81 = {
#include "stg06/terrain/collision_81.inc"
};
static NJS_MATERIAL collision_82_materials[1] = {
#include "stg06/terrain/collision_82_materials.inc"
};
static Sint16 collision_82_faces[37] = {
#include "stg06/terrain/collision_82_faces.inc"
};
static NJS_MESHSET collision_82_meshsets[1] = {
#include "stg06/terrain/collision_82_meshsets.inc"
};
static NJS_POINT3 collision_82_points[16] = {
#include "stg06/terrain/collision_82_points.inc"
};
static NJS_MODEL collision_82_model = {
#include "stg06/terrain/collision_82_model.inc"
};
static NJS_OBJECT collision_82 = {
#include "stg06/terrain/collision_82.inc"
};
static NJS_MATERIAL collision_83_materials[1] = {
#include "stg06/terrain/collision_83_materials.inc"
};
static Sint16 collision_83_faces[174] = {
#include "stg06/terrain/collision_83_faces.inc"
};
static NJS_MESHSET collision_83_meshsets[1] = {
#include "stg06/terrain/collision_83_meshsets.inc"
};
static NJS_POINT3 collision_83_points[66] = {
#include "stg06/terrain/collision_83_points.inc"
};
static NJS_MODEL collision_83_model = {
#include "stg06/terrain/collision_83_model.inc"
};
static NJS_OBJECT collision_83 = {
#include "stg06/terrain/collision_83.inc"
};
static NJS_MATERIAL collision_84_materials[1] = {
#include "stg06/terrain/collision_84_materials.inc"
};
static Sint16 collision_84_faces[122] = {
#include "stg06/terrain/collision_84_faces.inc"
};
static NJS_MESHSET collision_84_meshsets[1] = {
#include "stg06/terrain/collision_84_meshsets.inc"
};
static NJS_POINT3 collision_84_points[72] = {
#include "stg06/terrain/collision_84_points.inc"
};
static NJS_MODEL collision_84_model = {
#include "stg06/terrain/collision_84_model.inc"
};
static NJS_OBJECT collision_84 = {
#include "stg06/terrain/collision_84.inc"
};
static NJS_MATERIAL collision_85_materials[1] = {
#include "stg06/terrain/collision_85_materials.inc"
};
static Sint16 collision_85_faces[62] = {
#include "stg06/terrain/collision_85_faces.inc"
};
static NJS_MESHSET collision_85_meshsets[1] = {
#include "stg06/terrain/collision_85_meshsets.inc"
};
static NJS_POINT3 collision_85_points[42] = {
#include "stg06/terrain/collision_85_points.inc"
};
static NJS_MODEL collision_85_model = {
#include "stg06/terrain/collision_85_model.inc"
};
static NJS_OBJECT collision_85 = {
#include "stg06/terrain/collision_85.inc"
};
static NJS_MATERIAL collision_86_materials[1] = {
#include "stg06/terrain/collision_86_materials.inc"
};
static Sint16 collision_86_faces[26] = {
#include "stg06/terrain/collision_86_faces.inc"
};
static NJS_MESHSET collision_86_meshsets[1] = {
#include "stg06/terrain/collision_86_meshsets.inc"
};
static NJS_POINT3 collision_86_points[18] = {
#include "stg06/terrain/collision_86_points.inc"
};
static NJS_MODEL collision_86_model = {
#include "stg06/terrain/collision_86_model.inc"
};
static NJS_OBJECT collision_86 = {
#include "stg06/terrain/collision_86.inc"
};
static NJS_MATERIAL collision_88_materials[1] = {
#include "stg06/terrain/collision_88_materials.inc"
};
static Sint16 collision_88_faces[22] = {
#include "stg06/terrain/collision_88_faces.inc"
};
static NJS_MESHSET collision_88_meshsets[1] = {
#include "stg06/terrain/collision_88_meshsets.inc"
};
static NJS_POINT3 collision_88_points[15] = {
#include "stg06/terrain/collision_88_points.inc"
};
static NJS_MODEL collision_88_model = {
#include "stg06/terrain/collision_88_model.inc"
};
static NJS_OBJECT collision_88 = {
#include "stg06/terrain/collision_88.inc"
};
static NJS_MATERIAL collision_91_materials[1] = {
#include "stg06/terrain/collision_91_materials.inc"
};
static Sint16 collision_91_faces[18] = {
#include "stg06/terrain/collision_91_faces.inc"
};
static NJS_MESHSET collision_91_meshsets[1] = {
#include "stg06/terrain/collision_91_meshsets.inc"
};
static NJS_POINT3 collision_91_points[12] = {
#include "stg06/terrain/collision_91_points.inc"
};
static NJS_MODEL collision_91_model = {
#include "stg06/terrain/collision_91_model.inc"
};
static NJS_OBJECT collision_91 = {
#include "stg06/terrain/collision_91.inc"
};
static NJS_MATERIAL collision_94_materials[1] = {
#include "stg06/terrain/collision_94_materials.inc"
};
static Sint16 collision_94_faces[38] = {
#include "stg06/terrain/collision_94_faces.inc"
};
static NJS_MESHSET collision_94_meshsets[1] = {
#include "stg06/terrain/collision_94_meshsets.inc"
};
static NJS_POINT3 collision_94_points[27] = {
#include "stg06/terrain/collision_94_points.inc"
};
static NJS_MODEL collision_94_model = {
#include "stg06/terrain/collision_94_model.inc"
};
static NJS_OBJECT collision_94 = {
#include "stg06/terrain/collision_94.inc"
};
static NJS_MATERIAL collision_95_materials[1] = {
#include "stg06/terrain/collision_95_materials.inc"
};
static Sint16 collision_95_faces[10] = {
#include "stg06/terrain/collision_95_faces.inc"
};
static NJS_MESHSET collision_95_meshsets[1] = {
#include "stg06/terrain/collision_95_meshsets.inc"
};
static NJS_POINT3 collision_95_points[6] = {
#include "stg06/terrain/collision_95_points.inc"
};
static NJS_MODEL collision_95_model = {
#include "stg06/terrain/collision_95_model.inc"
};
static NJS_OBJECT collision_95 = {
#include "stg06/terrain/collision_95.inc"
};
static Sint16 draw_9_polygons[243] = {
#include "stg06/terrain/draw_9_polygons.inc"
};
static Sint32 draw_9_vertices[243] = {
#include "stg06/terrain/draw_9_vertices.inc"
};
static NJS_CNK_MODEL draw_9_model = {
#include "stg06/terrain/draw_9_model.inc"
};
static NJS_CNK_OBJECT draw_161 = {
#include "stg06/terrain/draw_161.inc"
};
static Sint16 draw_89_polygons[71] = {
#include "stg06/terrain/draw_89_polygons.inc"
};
static Sint32 draw_89_vertices[83] = {
#include "stg06/terrain/draw_89_vertices.inc"
};
static NJS_CNK_MODEL draw_89_model = {
#include "stg06/terrain/draw_89_model.inc"
};
static NJS_CNK_OBJECT draw_155 = {
#include "stg06/terrain/draw_155.inc"
};
static Sint16 draw_19_polygons[115] = {
#include "stg06/terrain/draw_19_polygons.inc"
};
static Sint32 draw_19_vertices[111] = {
#include "stg06/terrain/draw_19_vertices.inc"
};
static NJS_CNK_MODEL draw_19_model = {
#include "stg06/terrain/draw_19_model.inc"
};
static NJS_CNK_OBJECT draw_154 = {
#include "stg06/terrain/draw_154.inc"
};
static Sint16 draw_20_polygons[103] = {
#include "stg06/terrain/draw_20_polygons.inc"
};
static Sint32 draw_20_vertices[83] = {
#include "stg06/terrain/draw_20_vertices.inc"
};
static NJS_CNK_MODEL draw_20_model = {
#include "stg06/terrain/draw_20_model.inc"
};
static NJS_CNK_OBJECT draw_153 = {
#include "stg06/terrain/draw_153.inc"
};
static Sint16 draw_151_polygons[245] = {
#include "stg06/terrain/draw_151_polygons.inc"
};
static Sint32 draw_151_vertices[183] = {
#include "stg06/terrain/draw_151_vertices.inc"
};
static NJS_CNK_MODEL draw_151_model = {
#include "stg06/terrain/draw_151_model.inc"
};
static NJS_CNK_OBJECT draw_151 = {
#include "stg06/terrain/draw_151.inc"
};
static Sint16 draw_149_polygons[37] = {
#include "stg06/terrain/draw_149_polygons.inc"
};
static Sint32 draw_149_vertices[43] = {
#include "stg06/terrain/draw_149_vertices.inc"
};
static NJS_CNK_MODEL draw_149_model = {
#include "stg06/terrain/draw_149_model.inc"
};
static NJS_CNK_OBJECT draw_149 = {
#include "stg06/terrain/draw_149.inc"
};
static Sint16 draw_148_polygons[45] = {
#include "stg06/terrain/draw_148_polygons.inc"
};
static Sint32 draw_148_vertices[47] = {
#include "stg06/terrain/draw_148_vertices.inc"
};
static NJS_CNK_MODEL draw_148_model = {
#include "stg06/terrain/draw_148_model.inc"
};
static NJS_CNK_OBJECT draw_148 = {
#include "stg06/terrain/draw_148.inc"
};
static Sint16 draw_147_polygons[399] = {
#include "stg06/terrain/draw_147_polygons.inc"
};
static Sint32 draw_147_vertices[479] = {
#include "stg06/terrain/draw_147_vertices.inc"
};
static NJS_CNK_MODEL draw_147_model = {
#include "stg06/terrain/draw_147_model.inc"
};
static NJS_CNK_OBJECT draw_147 = {
#include "stg06/terrain/draw_147.inc"
};
static Sint16 draw_25_polygons[43] = {
#include "stg06/terrain/draw_25_polygons.inc"
};
static Sint32 draw_25_vertices[39] = {
#include "stg06/terrain/draw_25_vertices.inc"
};
static NJS_CNK_MODEL draw_25_model = {
#include "stg06/terrain/draw_25_model.inc"
};
static NJS_CNK_OBJECT draw_140 = {
#include "stg06/terrain/draw_140.inc"
};
static Sint16 draw_138_polygons[319] = {
#include "stg06/terrain/draw_138_polygons.inc"
};
static Sint32 draw_138_vertices[203] = {
#include "stg06/terrain/draw_138_vertices.inc"
};
static NJS_CNK_MODEL draw_138_model = {
#include "stg06/terrain/draw_138_model.inc"
};
static NJS_CNK_OBJECT draw_138 = {
#include "stg06/terrain/draw_138.inc"
};
static Sint16 draw_137_polygons[131] = {
#include "stg06/terrain/draw_137_polygons.inc"
};
static Sint32 draw_137_vertices[75] = {
#include "stg06/terrain/draw_137_vertices.inc"
};
static NJS_CNK_MODEL draw_137_model = {
#include "stg06/terrain/draw_137_model.inc"
};
static NJS_CNK_OBJECT draw_137 = {
#include "stg06/terrain/draw_137.inc"
};
static Sint16 draw_136_polygons[45] = {
#include "stg06/terrain/draw_136_polygons.inc"
};
static Sint32 draw_136_vertices[35] = {
#include "stg06/terrain/draw_136_vertices.inc"
};
static NJS_CNK_MODEL draw_136_model = {
#include "stg06/terrain/draw_136_model.inc"
};
static NJS_CNK_OBJECT draw_136 = {
#include "stg06/terrain/draw_136.inc"
};
static Sint16 draw_135_polygons[25] = {
#include "stg06/terrain/draw_135_polygons.inc"
};
static Sint32 draw_135_vertices[27] = {
#include "stg06/terrain/draw_135_vertices.inc"
};
static NJS_CNK_MODEL draw_135_model = {
#include "stg06/terrain/draw_135_model.inc"
};
static NJS_CNK_OBJECT draw_135 = {
#include "stg06/terrain/draw_135.inc"
};
static Sint16 draw_134_polygons[25] = {
#include "stg06/terrain/draw_134_polygons.inc"
};
static Sint32 draw_134_vertices[27] = {
#include "stg06/terrain/draw_134_vertices.inc"
};
static NJS_CNK_MODEL draw_134_model = {
#include "stg06/terrain/draw_134_model.inc"
};
static NJS_CNK_OBJECT draw_134 = {
#include "stg06/terrain/draw_134.inc"
};
static Sint16 draw_133_polygons[25] = {
#include "stg06/terrain/draw_133_polygons.inc"
};
static Sint32 draw_133_vertices[27] = {
#include "stg06/terrain/draw_133_vertices.inc"
};
static NJS_CNK_MODEL draw_133_model = {
#include "stg06/terrain/draw_133_model.inc"
};
static NJS_CNK_OBJECT draw_133 = {
#include "stg06/terrain/draw_133.inc"
};
static Sint16 draw_132_polygons[25] = {
#include "stg06/terrain/draw_132_polygons.inc"
};
static Sint32 draw_132_vertices[27] = {
#include "stg06/terrain/draw_132_vertices.inc"
};
static NJS_CNK_MODEL draw_132_model = {
#include "stg06/terrain/draw_132_model.inc"
};
static NJS_CNK_OBJECT draw_132 = {
#include "stg06/terrain/draw_132.inc"
};
static Sint16 draw_131_polygons[25] = {
#include "stg06/terrain/draw_131_polygons.inc"
};
static Sint32 draw_131_vertices[27] = {
#include "stg06/terrain/draw_131_vertices.inc"
};
static NJS_CNK_MODEL draw_131_model = {
#include "stg06/terrain/draw_131_model.inc"
};
static NJS_CNK_OBJECT draw_131 = {
#include "stg06/terrain/draw_131.inc"
};
static Sint16 draw_128_polygons[157] = {
#include "stg06/terrain/draw_128_polygons.inc"
};
static Sint32 draw_128_vertices[91] = {
#include "stg06/terrain/draw_128_vertices.inc"
};
static NJS_CNK_MODEL draw_128_model = {
#include "stg06/terrain/draw_128_model.inc"
};
static NJS_CNK_OBJECT draw_128 = {
#include "stg06/terrain/draw_128.inc"
};
static Sint16 draw_0_polygons[391] = {
#include "stg06/terrain/draw_0_polygons.inc"
};
static Sint32 draw_0_vertices[167] = {
#include "stg06/terrain/draw_0_vertices.inc"
};
static NJS_CNK_MODEL draw_0_model = {
#include "stg06/terrain/draw_0_model.inc"
};
static NJS_CNK_OBJECT draw_123 = {
#include "stg06/terrain/draw_123.inc"
};
static Sint16 draw_121_polygons[271] = {
#include "stg06/terrain/draw_121_polygons.inc"
};
static Sint32 draw_121_vertices[331] = {
#include "stg06/terrain/draw_121_vertices.inc"
};
static NJS_CNK_MODEL draw_121_model = {
#include "stg06/terrain/draw_121_model.inc"
};
static NJS_CNK_OBJECT draw_121 = {
#include "stg06/terrain/draw_121.inc"
};
static Sint16 draw_120_polygons[39] = {
#include "stg06/terrain/draw_120_polygons.inc"
};
static Sint32 draw_120_vertices[35] = {
#include "stg06/terrain/draw_120_vertices.inc"
};
static NJS_CNK_MODEL draw_120_model = {
#include "stg06/terrain/draw_120_model.inc"
};
static NJS_CNK_OBJECT draw_120 = {
#include "stg06/terrain/draw_120.inc"
};
static Sint16 draw_118_polygons[89] = {
#include "stg06/terrain/draw_118_polygons.inc"
};
static Sint32 draw_118_vertices[51] = {
#include "stg06/terrain/draw_118_vertices.inc"
};
static NJS_CNK_MODEL draw_118_model = {
#include "stg06/terrain/draw_118_model.inc"
};
static NJS_CNK_OBJECT draw_118 = {
#include "stg06/terrain/draw_118.inc"
};
static Sint16 draw_28_polygons[69] = {
#include "stg06/terrain/draw_28_polygons.inc"
};
static Sint32 draw_28_vertices[47] = {
#include "stg06/terrain/draw_28_vertices.inc"
};
static NJS_CNK_MODEL draw_28_model = {
#include "stg06/terrain/draw_28_model.inc"
};
static NJS_CNK_OBJECT draw_113 = {
#include "stg06/terrain/draw_113.inc"
};
static Sint16 draw_112_polygons[227] = {
#include "stg06/terrain/draw_112_polygons.inc"
};
static Sint32 draw_112_vertices[131] = {
#include "stg06/terrain/draw_112_vertices.inc"
};
static NJS_CNK_MODEL draw_112_model = {
#include "stg06/terrain/draw_112_model.inc"
};
static NJS_CNK_OBJECT draw_112 = {
#include "stg06/terrain/draw_112.inc"
};
static Sint16 draw_100_polygons[109] = {
#include "stg06/terrain/draw_100_polygons.inc"
};
static Sint32 draw_100_vertices[75] = {
#include "stg06/terrain/draw_100_vertices.inc"
};
static NJS_CNK_MODEL draw_100_model = {
#include "stg06/terrain/draw_100_model.inc"
};
static NJS_CNK_OBJECT draw_111 = {
#include "stg06/terrain/draw_111.inc"
};
static Sint16 draw_99_polygons[233] = {
#include "stg06/terrain/draw_99_polygons.inc"
};
static Sint32 draw_99_vertices[151] = {
#include "stg06/terrain/draw_99_vertices.inc"
};
static NJS_CNK_MODEL draw_99_model = {
#include "stg06/terrain/draw_99_model.inc"
};
static NJS_CNK_OBJECT draw_110 = {
#include "stg06/terrain/draw_110.inc"
};
static Sint16 draw_108_polygons[71] = {
#include "stg06/terrain/draw_108_polygons.inc"
};
static Sint32 draw_108_vertices[51] = {
#include "stg06/terrain/draw_108_vertices.inc"
};
static NJS_CNK_MODEL draw_108_model = {
#include "stg06/terrain/draw_108_model.inc"
};
static NJS_CNK_OBJECT draw_108 = {
#include "stg06/terrain/draw_108.inc"
};
static Sint16 draw_107_polygons[229] = {
#include "stg06/terrain/draw_107_polygons.inc"
};
static Sint32 draw_107_vertices[135] = {
#include "stg06/terrain/draw_107_vertices.inc"
};
static NJS_CNK_MODEL draw_107_model = {
#include "stg06/terrain/draw_107_model.inc"
};
static NJS_CNK_OBJECT draw_107 = {
#include "stg06/terrain/draw_107.inc"
};
static Sint16 draw_105_polygons[221] = {
#include "stg06/terrain/draw_105_polygons.inc"
};
static Sint32 draw_105_vertices[119] = {
#include "stg06/terrain/draw_105_vertices.inc"
};
static NJS_CNK_MODEL draw_105_model = {
#include "stg06/terrain/draw_105_model.inc"
};
static NJS_CNK_OBJECT draw_105 = {
#include "stg06/terrain/draw_105.inc"
};
static Sint16 draw_104_polygons[103] = {
#include "stg06/terrain/draw_104_polygons.inc"
};
static Sint32 draw_104_vertices[91] = {
#include "stg06/terrain/draw_104_vertices.inc"
};
static NJS_CNK_MODEL draw_104_model = {
#include "stg06/terrain/draw_104_model.inc"
};
static NJS_CNK_OBJECT draw_104 = {
#include "stg06/terrain/draw_104.inc"
};
static Sint16 draw_103_polygons[193] = {
#include "stg06/terrain/draw_103_polygons.inc"
};
static Sint32 draw_103_vertices[151] = {
#include "stg06/terrain/draw_103_vertices.inc"
};
static NJS_CNK_MODEL draw_103_model = {
#include "stg06/terrain/draw_103_model.inc"
};
static NJS_CNK_OBJECT draw_103 = {
#include "stg06/terrain/draw_103.inc"
};
static Sint16 draw_98_polygons[145] = {
#include "stg06/terrain/draw_98_polygons.inc"
};
static Sint32 draw_98_vertices[79] = {
#include "stg06/terrain/draw_98_vertices.inc"
};
static NJS_CNK_MODEL draw_98_model = {
#include "stg06/terrain/draw_98_model.inc"
};
static NJS_CNK_OBJECT draw_98 = {
#include "stg06/terrain/draw_98.inc"
};
static Sint16 draw_91_polygons[667] = {
#include "stg06/terrain/draw_91_polygons.inc"
};
static Sint32 draw_91_vertices[803] = {
#include "stg06/terrain/draw_91_vertices.inc"
};
static NJS_CNK_MODEL draw_91_model = {
#include "stg06/terrain/draw_91_model.inc"
};
static NJS_CNK_OBJECT draw_91 = {
#include "stg06/terrain/draw_91.inc"
};
static Sint16 draw_90_polygons[875] = {
#include "stg06/terrain/draw_90_polygons.inc"
};
static Sint32 draw_90_vertices[1103] = {
#include "stg06/terrain/draw_90_vertices.inc"
};
static NJS_CNK_MODEL draw_90_model = {
#include "stg06/terrain/draw_90_model.inc"
};
static NJS_CNK_OBJECT draw_90 = {
#include "stg06/terrain/draw_90.inc"
};
static Sint16 draw_88_polygons[787] = {
#include "stg06/terrain/draw_88_polygons.inc"
};
static Sint32 draw_88_vertices[435] = {
#include "stg06/terrain/draw_88_vertices.inc"
};
static NJS_CNK_MODEL draw_88_model = {
#include "stg06/terrain/draw_88_model.inc"
};
static NJS_CNK_OBJECT draw_88 = {
#include "stg06/terrain/draw_88.inc"
};
static Sint16 draw_87_polygons[57] = {
#include "stg06/terrain/draw_87_polygons.inc"
};
static Sint32 draw_87_vertices[51] = {
#include "stg06/terrain/draw_87_vertices.inc"
};
static NJS_CNK_MODEL draw_87_model = {
#include "stg06/terrain/draw_87_model.inc"
};
static NJS_CNK_OBJECT draw_87 = {
#include "stg06/terrain/draw_87.inc"
};
static Sint16 draw_86_polygons[99] = {
#include "stg06/terrain/draw_86_polygons.inc"
};
static Sint32 draw_86_vertices[75] = {
#include "stg06/terrain/draw_86_vertices.inc"
};
static NJS_CNK_MODEL draw_86_model = {
#include "stg06/terrain/draw_86_model.inc"
};
static NJS_CNK_OBJECT draw_86 = {
#include "stg06/terrain/draw_86.inc"
};
static Sint16 draw_85_polygons[797] = {
#include "stg06/terrain/draw_85_polygons.inc"
};
static Sint32 draw_85_vertices[991] = {
#include "stg06/terrain/draw_85_vertices.inc"
};
static NJS_CNK_MODEL draw_85_model = {
#include "stg06/terrain/draw_85_model.inc"
};
static NJS_CNK_OBJECT draw_85 = {
#include "stg06/terrain/draw_85.inc"
};
static Sint16 draw_81_polygons[185] = {
#include "stg06/terrain/draw_81_polygons.inc"
};
static Sint32 draw_81_vertices[83] = {
#include "stg06/terrain/draw_81_vertices.inc"
};
static NJS_CNK_MODEL draw_81_model = {
#include "stg06/terrain/draw_81_model.inc"
};
static NJS_CNK_OBJECT draw_81 = {
#include "stg06/terrain/draw_81.inc"
};
static Sint16 draw_80_polygons[185] = {
#include "stg06/terrain/draw_80_polygons.inc"
};
static Sint32 draw_80_vertices[83] = {
#include "stg06/terrain/draw_80_vertices.inc"
};
static NJS_CNK_MODEL draw_80_model = {
#include "stg06/terrain/draw_80_model.inc"
};
static NJS_CNK_OBJECT draw_80 = {
#include "stg06/terrain/draw_80.inc"
};
static Sint16 draw_79_polygons[401] = {
#include "stg06/terrain/draw_79_polygons.inc"
};
static Sint32 draw_79_vertices[447] = {
#include "stg06/terrain/draw_79_vertices.inc"
};
static NJS_CNK_MODEL draw_79_model = {
#include "stg06/terrain/draw_79_model.inc"
};
static NJS_CNK_OBJECT draw_79 = {
#include "stg06/terrain/draw_79.inc"
};
static Sint16 draw_78_polygons[509] = {
#include "stg06/terrain/draw_78_polygons.inc"
};
static Sint32 draw_78_vertices[563] = {
#include "stg06/terrain/draw_78_vertices.inc"
};
static NJS_CNK_MODEL draw_78_model = {
#include "stg06/terrain/draw_78_model.inc"
};
static NJS_CNK_OBJECT draw_78 = {
#include "stg06/terrain/draw_78.inc"
};
static Sint16 draw_77_polygons[977] = {
#include "stg06/terrain/draw_77_polygons.inc"
};
static Sint32 draw_77_vertices[1211] = {
#include "stg06/terrain/draw_77_vertices.inc"
};
static NJS_CNK_MODEL draw_77_model = {
#include "stg06/terrain/draw_77_model.inc"
};
static NJS_CNK_OBJECT draw_77 = {
#include "stg06/terrain/draw_77.inc"
};
static Sint16 draw_75_polygons[261] = {
#include "stg06/terrain/draw_75_polygons.inc"
};
static Sint32 draw_75_vertices[239] = {
#include "stg06/terrain/draw_75_vertices.inc"
};
static NJS_CNK_MODEL draw_75_model = {
#include "stg06/terrain/draw_75_model.inc"
};
static NJS_CNK_OBJECT draw_75 = {
#include "stg06/terrain/draw_75.inc"
};
static Sint16 draw_73_polygons[91] = {
#include "stg06/terrain/draw_73_polygons.inc"
};
static Sint32 draw_73_vertices[59] = {
#include "stg06/terrain/draw_73_vertices.inc"
};
static NJS_CNK_MODEL draw_73_model = {
#include "stg06/terrain/draw_73_model.inc"
};
static NJS_CNK_OBJECT draw_73 = {
#include "stg06/terrain/draw_73.inc"
};
static Sint16 draw_71_polygons[131] = {
#include "stg06/terrain/draw_71_polygons.inc"
};
static Sint32 draw_71_vertices[87] = {
#include "stg06/terrain/draw_71_vertices.inc"
};
static NJS_CNK_MODEL draw_71_model = {
#include "stg06/terrain/draw_71_model.inc"
};
static NJS_CNK_OBJECT draw_71 = {
#include "stg06/terrain/draw_71.inc"
};
static Sint16 draw_70_polygons[181] = {
#include "stg06/terrain/draw_70_polygons.inc"
};
static Sint32 draw_70_vertices[123] = {
#include "stg06/terrain/draw_70_vertices.inc"
};
static NJS_CNK_MODEL draw_70_model = {
#include "stg06/terrain/draw_70_model.inc"
};
static NJS_CNK_OBJECT draw_70 = {
#include "stg06/terrain/draw_70.inc"
};
static Sint16 draw_69_polygons[389] = {
#include "stg06/terrain/draw_69_polygons.inc"
};
static Sint32 draw_69_vertices[223] = {
#include "stg06/terrain/draw_69_vertices.inc"
};
static NJS_CNK_MODEL draw_69_model = {
#include "stg06/terrain/draw_69_model.inc"
};
static NJS_CNK_OBJECT draw_69 = {
#include "stg06/terrain/draw_69.inc"
};
static Sint16 draw_68_polygons[1019] = {
#include "stg06/terrain/draw_68_polygons.inc"
};
static Sint32 draw_68_vertices[631] = {
#include "stg06/terrain/draw_68_vertices.inc"
};
static NJS_CNK_MODEL draw_68_model = {
#include "stg06/terrain/draw_68_model.inc"
};
static NJS_CNK_OBJECT draw_68 = {
#include "stg06/terrain/draw_68.inc"
};
static Sint16 draw_67_polygons[99] = {
#include "stg06/terrain/draw_67_polygons.inc"
};
static Sint32 draw_67_vertices[75] = {
#include "stg06/terrain/draw_67_vertices.inc"
};
static NJS_CNK_MODEL draw_67_model = {
#include "stg06/terrain/draw_67_model.inc"
};
static NJS_CNK_OBJECT draw_67 = {
#include "stg06/terrain/draw_67.inc"
};
static Sint16 draw_66_polygons[93] = {
#include "stg06/terrain/draw_66_polygons.inc"
};
static Sint32 draw_66_vertices[75] = {
#include "stg06/terrain/draw_66_vertices.inc"
};
static NJS_CNK_MODEL draw_66_model = {
#include "stg06/terrain/draw_66_model.inc"
};
static NJS_CNK_OBJECT draw_66 = {
#include "stg06/terrain/draw_66.inc"
};
static Sint16 draw_65_polygons[375] = {
#include "stg06/terrain/draw_65_polygons.inc"
};
static Sint32 draw_65_vertices[207] = {
#include "stg06/terrain/draw_65_vertices.inc"
};
static NJS_CNK_MODEL draw_65_model = {
#include "stg06/terrain/draw_65_model.inc"
};
static NJS_CNK_OBJECT draw_65 = {
#include "stg06/terrain/draw_65.inc"
};
static Sint16 draw_64_polygons[385] = {
#include "stg06/terrain/draw_64_polygons.inc"
};
static Sint32 draw_64_vertices[183] = {
#include "stg06/terrain/draw_64_vertices.inc"
};
static NJS_CNK_MODEL draw_64_model = {
#include "stg06/terrain/draw_64_model.inc"
};
static NJS_CNK_OBJECT draw_64 = {
#include "stg06/terrain/draw_64.inc"
};
static Sint16 draw_63_polygons[49] = {
#include "stg06/terrain/draw_63_polygons.inc"
};
static Sint32 draw_63_vertices[51] = {
#include "stg06/terrain/draw_63_vertices.inc"
};
static NJS_CNK_MODEL draw_63_model = {
#include "stg06/terrain/draw_63_model.inc"
};
static NJS_CNK_OBJECT draw_63 = {
#include "stg06/terrain/draw_63.inc"
};
static Sint16 draw_62_polygons[861] = {
#include "stg06/terrain/draw_62_polygons.inc"
};
static Sint32 draw_62_vertices[819] = {
#include "stg06/terrain/draw_62_vertices.inc"
};
static NJS_CNK_MODEL draw_62_model = {
#include "stg06/terrain/draw_62_model.inc"
};
static NJS_CNK_OBJECT draw_62 = {
#include "stg06/terrain/draw_62.inc"
};
static Sint16 draw_61_polygons[709] = {
#include "stg06/terrain/draw_61_polygons.inc"
};
static Sint32 draw_61_vertices[415] = {
#include "stg06/terrain/draw_61_vertices.inc"
};
static NJS_CNK_MODEL draw_61_model = {
#include "stg06/terrain/draw_61_model.inc"
};
static NJS_CNK_OBJECT draw_61 = {
#include "stg06/terrain/draw_61.inc"
};
static Sint16 draw_60_polygons[519] = {
#include "stg06/terrain/draw_60_polygons.inc"
};
static Sint32 draw_60_vertices[271] = {
#include "stg06/terrain/draw_60_vertices.inc"
};
static NJS_CNK_MODEL draw_60_model = {
#include "stg06/terrain/draw_60_model.inc"
};
static NJS_CNK_OBJECT draw_60 = {
#include "stg06/terrain/draw_60.inc"
};
static Sint16 draw_59_polygons[161] = {
#include "stg06/terrain/draw_59_polygons.inc"
};
static Sint32 draw_59_vertices[123] = {
#include "stg06/terrain/draw_59_vertices.inc"
};
static NJS_CNK_MODEL draw_59_model = {
#include "stg06/terrain/draw_59_model.inc"
};
static NJS_CNK_OBJECT draw_59 = {
#include "stg06/terrain/draw_59.inc"
};
static Sint16 draw_58_polygons[49] = {
#include "stg06/terrain/draw_58_polygons.inc"
};
static Sint32 draw_58_vertices[51] = {
#include "stg06/terrain/draw_58_vertices.inc"
};
static NJS_CNK_MODEL draw_58_model = {
#include "stg06/terrain/draw_58_model.inc"
};
static NJS_CNK_OBJECT draw_58 = {
#include "stg06/terrain/draw_58.inc"
};
static Sint16 draw_57_polygons[889] = {
#include "stg06/terrain/draw_57_polygons.inc"
};
static Sint32 draw_57_vertices[511] = {
#include "stg06/terrain/draw_57_vertices.inc"
};
static NJS_CNK_MODEL draw_57_model = {
#include "stg06/terrain/draw_57_model.inc"
};
static NJS_CNK_OBJECT draw_57 = {
#include "stg06/terrain/draw_57.inc"
};
static Sint16 draw_56_polygons[329] = {
#include "stg06/terrain/draw_56_polygons.inc"
};
static Sint32 draw_56_vertices[183] = {
#include "stg06/terrain/draw_56_vertices.inc"
};
static NJS_CNK_MODEL draw_56_model = {
#include "stg06/terrain/draw_56_model.inc"
};
static NJS_CNK_OBJECT draw_56 = {
#include "stg06/terrain/draw_56.inc"
};
static Sint16 draw_55_polygons[1543] = {
#include "stg06/terrain/draw_55_polygons.inc"
};
static Sint32 draw_55_vertices[1891] = {
#include "stg06/terrain/draw_55_vertices.inc"
};
static NJS_CNK_MODEL draw_55_model = {
#include "stg06/terrain/draw_55_model.inc"
};
static NJS_CNK_OBJECT draw_55 = {
#include "stg06/terrain/draw_55.inc"
};
static Sint16 draw_54_polygons[1177] = {
#include "stg06/terrain/draw_54_polygons.inc"
};
static Sint32 draw_54_vertices[755] = {
#include "stg06/terrain/draw_54_vertices.inc"
};
static NJS_CNK_MODEL draw_54_model = {
#include "stg06/terrain/draw_54_model.inc"
};
static NJS_CNK_OBJECT draw_54 = {
#include "stg06/terrain/draw_54.inc"
};
static Sint16 draw_53_polygons[201] = {
#include "stg06/terrain/draw_53_polygons.inc"
};
static Sint32 draw_53_vertices[123] = {
#include "stg06/terrain/draw_53_vertices.inc"
};
static NJS_CNK_MODEL draw_53_model = {
#include "stg06/terrain/draw_53_model.inc"
};
static NJS_CNK_OBJECT draw_53 = {
#include "stg06/terrain/draw_53.inc"
};
static Sint16 draw_52_polygons[221] = {
#include "stg06/terrain/draw_52_polygons.inc"
};
static Sint32 draw_52_vertices[239] = {
#include "stg06/terrain/draw_52_vertices.inc"
};
static NJS_CNK_MODEL draw_52_model = {
#include "stg06/terrain/draw_52_model.inc"
};
static NJS_CNK_OBJECT draw_52 = {
#include "stg06/terrain/draw_52.inc"
};
static Sint16 draw_51_polygons[679] = {
#include "stg06/terrain/draw_51_polygons.inc"
};
static Sint32 draw_51_vertices[447] = {
#include "stg06/terrain/draw_51_vertices.inc"
};
static NJS_CNK_MODEL draw_51_model = {
#include "stg06/terrain/draw_51_model.inc"
};
static NJS_CNK_OBJECT draw_51 = {
#include "stg06/terrain/draw_51.inc"
};
static Sint16 draw_50_polygons[103] = {
#include "stg06/terrain/draw_50_polygons.inc"
};
static Sint32 draw_50_vertices[75] = {
#include "stg06/terrain/draw_50_vertices.inc"
};
static NJS_CNK_MODEL draw_50_model = {
#include "stg06/terrain/draw_50_model.inc"
};
static NJS_CNK_OBJECT draw_50 = {
#include "stg06/terrain/draw_50.inc"
};
static Sint16 draw_48_polygons[261] = {
#include "stg06/terrain/draw_48_polygons.inc"
};
static Sint32 draw_48_vertices[159] = {
#include "stg06/terrain/draw_48_vertices.inc"
};
static NJS_CNK_MODEL draw_48_model = {
#include "stg06/terrain/draw_48_model.inc"
};
static NJS_CNK_OBJECT draw_49 = {
#include "stg06/terrain/draw_49.inc"
};
static Sint16 draw_43_polygons[185] = {
#include "stg06/terrain/draw_43_polygons.inc"
};
static Sint32 draw_43_vertices[95] = {
#include "stg06/terrain/draw_43_vertices.inc"
};
static NJS_CNK_MODEL draw_43_model = {
#include "stg06/terrain/draw_43_model.inc"
};
static NJS_CNK_OBJECT draw_45 = {
#include "stg06/terrain/draw_45.inc"
};
static Sint16 draw_40_polygons[997] = {
#include "stg06/terrain/draw_40_polygons.inc"
};
static Sint32 draw_40_vertices[1071] = {
#include "stg06/terrain/draw_40_vertices.inc"
};
static NJS_CNK_MODEL draw_40_model = {
#include "stg06/terrain/draw_40_model.inc"
};
static NJS_CNK_OBJECT draw_40 = {
#include "stg06/terrain/draw_40.inc"
};
static Sint16 draw_39_polygons[2271] = {
#include "stg06/terrain/draw_39_polygons.inc"
};
static Sint32 draw_39_vertices[1227] = {
#include "stg06/terrain/draw_39_vertices.inc"
};
static NJS_CNK_MODEL draw_39_model = {
#include "stg06/terrain/draw_39_model.inc"
};
static NJS_CNK_OBJECT draw_39 = {
#include "stg06/terrain/draw_39.inc"
};
static Sint16 draw_38_polygons[201] = {
#include "stg06/terrain/draw_38_polygons.inc"
};
static Sint32 draw_38_vertices[119] = {
#include "stg06/terrain/draw_38_vertices.inc"
};
static NJS_CNK_MODEL draw_38_model = {
#include "stg06/terrain/draw_38_model.inc"
};
static NJS_CNK_OBJECT draw_38 = {
#include "stg06/terrain/draw_38.inc"
};
static Sint16 draw_37_polygons[237] = {
#include "stg06/terrain/draw_37_polygons.inc"
};
static Sint32 draw_37_vertices[135] = {
#include "stg06/terrain/draw_37_vertices.inc"
};
static NJS_CNK_MODEL draw_37_model = {
#include "stg06/terrain/draw_37_model.inc"
};
static NJS_CNK_OBJECT draw_37 = {
#include "stg06/terrain/draw_37.inc"
};
static Sint16 draw_36_polygons[587] = {
#include "stg06/terrain/draw_36_polygons.inc"
};
static Sint32 draw_36_vertices[611] = {
#include "stg06/terrain/draw_36_vertices.inc"
};
static NJS_CNK_MODEL draw_36_model = {
#include "stg06/terrain/draw_36_model.inc"
};
static NJS_CNK_OBJECT draw_36 = {
#include "stg06/terrain/draw_36.inc"
};
static Sint16 draw_35_polygons[1155] = {
#include "stg06/terrain/draw_35_polygons.inc"
};
static Sint32 draw_35_vertices[1391] = {
#include "stg06/terrain/draw_35_vertices.inc"
};
static NJS_CNK_MODEL draw_35_model = {
#include "stg06/terrain/draw_35_model.inc"
};
static NJS_CNK_OBJECT draw_35 = {
#include "stg06/terrain/draw_35.inc"
};
static Sint16 draw_34_polygons[839] = {
#include "stg06/terrain/draw_34_polygons.inc"
};
static Sint32 draw_34_vertices[927] = {
#include "stg06/terrain/draw_34_vertices.inc"
};
static NJS_CNK_MODEL draw_34_model = {
#include "stg06/terrain/draw_34_model.inc"
};
static NJS_CNK_OBJECT draw_34 = {
#include "stg06/terrain/draw_34.inc"
};
static Sint16 draw_33_polygons[617] = {
#include "stg06/terrain/draw_33_polygons.inc"
};
static Sint32 draw_33_vertices[719] = {
#include "stg06/terrain/draw_33_vertices.inc"
};
static NJS_CNK_MODEL draw_33_model = {
#include "stg06/terrain/draw_33_model.inc"
};
static NJS_CNK_OBJECT draw_33 = {
#include "stg06/terrain/draw_33.inc"
};
static Sint16 draw_32_polygons[89] = {
#include "stg06/terrain/draw_32_polygons.inc"
};
static Sint32 draw_32_vertices[87] = {
#include "stg06/terrain/draw_32_vertices.inc"
};
static NJS_CNK_MODEL draw_32_model = {
#include "stg06/terrain/draw_32_model.inc"
};
static NJS_CNK_OBJECT draw_32 = {
#include "stg06/terrain/draw_32.inc"
};
static Sint16 draw_31_polygons[475] = {
#include "stg06/terrain/draw_31_polygons.inc"
};
static Sint32 draw_31_vertices[259] = {
#include "stg06/terrain/draw_31_vertices.inc"
};
static NJS_CNK_MODEL draw_31_model = {
#include "stg06/terrain/draw_31_model.inc"
};
static NJS_CNK_OBJECT draw_31 = {
#include "stg06/terrain/draw_31.inc"
};
static Sint16 draw_18_polygons[79] = {
#include "stg06/terrain/draw_18_polygons.inc"
};
static Sint32 draw_18_vertices[51] = {
#include "stg06/terrain/draw_18_vertices.inc"
};
static NJS_CNK_MODEL draw_18_model = {
#include "stg06/terrain/draw_18_model.inc"
};
static NJS_CNK_OBJECT draw_18 = {
#include "stg06/terrain/draw_18.inc"
};
static Sint16 draw_17_polygons[105] = {
#include "stg06/terrain/draw_17_polygons.inc"
};
static Sint32 draw_17_vertices[55] = {
#include "stg06/terrain/draw_17_vertices.inc"
};
static NJS_CNK_MODEL draw_17_model = {
#include "stg06/terrain/draw_17_model.inc"
};
static NJS_CNK_OBJECT draw_17 = {
#include "stg06/terrain/draw_17.inc"
};
static Sint16 draw_16_polygons[187] = {
#include "stg06/terrain/draw_16_polygons.inc"
};
static Sint32 draw_16_vertices[103] = {
#include "stg06/terrain/draw_16_vertices.inc"
};
static NJS_CNK_MODEL draw_16_model = {
#include "stg06/terrain/draw_16_model.inc"
};
static NJS_CNK_OBJECT draw_16 = {
#include "stg06/terrain/draw_16.inc"
};
static Sint16 draw_15_polygons[45] = {
#include "stg06/terrain/draw_15_polygons.inc"
};
static Sint32 draw_15_vertices[35] = {
#include "stg06/terrain/draw_15_vertices.inc"
};
static NJS_CNK_MODEL draw_15_model = {
#include "stg06/terrain/draw_15_model.inc"
};
static NJS_CNK_OBJECT draw_15 = {
#include "stg06/terrain/draw_15.inc"
};
static Sint16 draw_14_polygons[19] = {
#include "stg06/terrain/draw_14_polygons.inc"
};
static Sint32 draw_14_vertices[19] = {
#include "stg06/terrain/draw_14_vertices.inc"
};
static NJS_CNK_MODEL draw_14_model = {
#include "stg06/terrain/draw_14_model.inc"
};
static NJS_CNK_OBJECT draw_14 = {
#include "stg06/terrain/draw_14.inc"
};
static Sint16 draw_13_polygons[67] = {
#include "stg06/terrain/draw_13_polygons.inc"
};
static Sint32 draw_13_vertices[83] = {
#include "stg06/terrain/draw_13_vertices.inc"
};
static NJS_CNK_MODEL draw_13_model = {
#include "stg06/terrain/draw_13_model.inc"
};
static NJS_CNK_OBJECT draw_13 = {
#include "stg06/terrain/draw_13.inc"
};
static Sint16 draw_12_polygons[45] = {
#include "stg06/terrain/draw_12_polygons.inc"
};
static Sint32 draw_12_vertices[35] = {
#include "stg06/terrain/draw_12_vertices.inc"
};
static NJS_CNK_MODEL draw_12_model = {
#include "stg06/terrain/draw_12_model.inc"
};
static NJS_CNK_OBJECT draw_12 = {
#include "stg06/terrain/draw_12.inc"
};
static Sint16 draw_11_polygons[43] = {
#include "stg06/terrain/draw_11_polygons.inc"
};
static Sint32 draw_11_vertices[31] = {
#include "stg06/terrain/draw_11_vertices.inc"
};
static NJS_CNK_MODEL draw_11_model = {
#include "stg06/terrain/draw_11_model.inc"
};
static NJS_CNK_OBJECT draw_11 = {
#include "stg06/terrain/draw_11.inc"
};
static Sint16 draw_10_polygons[43] = {
#include "stg06/terrain/draw_10_polygons.inc"
};
static Sint32 draw_10_vertices[35] = {
#include "stg06/terrain/draw_10_vertices.inc"
};
static NJS_CNK_MODEL draw_10_model = {
#include "stg06/terrain/draw_10_model.inc"
};
static NJS_CNK_OBJECT draw_10 = {
#include "stg06/terrain/draw_10.inc"
};
static Sint16 draw_8_polygons[481] = {
#include "stg06/terrain/draw_8_polygons.inc"
};
static Sint32 draw_8_vertices[455] = {
#include "stg06/terrain/draw_8_vertices.inc"
};
static NJS_CNK_MODEL draw_8_model = {
#include "stg06/terrain/draw_8_model.inc"
};
static NJS_CNK_OBJECT draw_8 = {
#include "stg06/terrain/draw_8.inc"
};
static Sint16 draw_7_polygons[347] = {
#include "stg06/terrain/draw_7_polygons.inc"
};
static Sint32 draw_7_vertices[319] = {
#include "stg06/terrain/draw_7_vertices.inc"
};
static NJS_CNK_MODEL draw_7_model = {
#include "stg06/terrain/draw_7_model.inc"
};
static NJS_CNK_OBJECT draw_7 = {
#include "stg06/terrain/draw_7.inc"
};
static Sint16 draw_6_polygons[239] = {
#include "stg06/terrain/draw_6_polygons.inc"
};
static Sint32 draw_6_vertices[267] = {
#include "stg06/terrain/draw_6_vertices.inc"
};
static NJS_CNK_MODEL draw_6_model = {
#include "stg06/terrain/draw_6_model.inc"
};
static NJS_CNK_OBJECT draw_6 = {
#include "stg06/terrain/draw_6.inc"
};
static Sint16 draw_5_polygons[477] = {
#include "stg06/terrain/draw_5_polygons.inc"
};
static Sint32 draw_5_vertices[463] = {
#include "stg06/terrain/draw_5_vertices.inc"
};
static NJS_CNK_MODEL draw_5_model = {
#include "stg06/terrain/draw_5_model.inc"
};
static NJS_CNK_OBJECT draw_5 = {
#include "stg06/terrain/draw_5.inc"
};
static Sint16 draw_4_polygons[355] = {
#include "stg06/terrain/draw_4_polygons.inc"
};
static Sint32 draw_4_vertices[367] = {
#include "stg06/terrain/draw_4_vertices.inc"
};
static NJS_CNK_MODEL draw_4_model = {
#include "stg06/terrain/draw_4_model.inc"
};
static NJS_CNK_OBJECT draw_4 = {
#include "stg06/terrain/draw_4.inc"
};
static Sint16 draw_3_polygons[233] = {
#include "stg06/terrain/draw_3_polygons.inc"
};
static Sint32 draw_3_vertices[135] = {
#include "stg06/terrain/draw_3_vertices.inc"
};
static NJS_CNK_MODEL draw_3_model = {
#include "stg06/terrain/draw_3_model.inc"
};
static NJS_CNK_OBJECT draw_3 = {
#include "stg06/terrain/draw_3.inc"
};
static Sint16 draw_2_polygons[99] = {
#include "stg06/terrain/draw_2_polygons.inc"
};
static Sint32 draw_2_vertices[83] = {
#include "stg06/terrain/draw_2_vertices.inc"
};
static NJS_CNK_MODEL draw_2_model = {
#include "stg06/terrain/draw_2_model.inc"
};
static NJS_CNK_OBJECT draw_2 = {
#include "stg06/terrain/draw_2.inc"
};
static NJS_OBJECT collision_1 = {
#include "stg06/terrain/collision_1.inc"
};
static NJS_OBJECT collision_17 = {
#include "stg06/terrain/collision_17.inc"
};
static NJS_OBJECT collision_31 = {
#include "stg06/terrain/collision_31.inc"
};
static NJS_OBJECT collision_34 = {
#include "stg06/terrain/collision_34.inc"
};
static NJS_OBJECT collision_36 = {
#include "stg06/terrain/collision_36.inc"
};
static NJS_OBJECT collision_37 = {
#include "stg06/terrain/collision_37.inc"
};
static NJS_OBJECT collision_39 = {
#include "stg06/terrain/collision_39.inc"
};
static NJS_OBJECT collision_40 = {
#include "stg06/terrain/collision_40.inc"
};
static NJS_OBJECT collision_45 = {
#include "stg06/terrain/collision_45.inc"
};
static NJS_OBJECT collision_48 = {
#include "stg06/terrain/collision_48.inc"
};
static NJS_OBJECT collision_54 = {
#include "stg06/terrain/collision_54.inc"
};
static NJS_OBJECT collision_55 = {
#include "stg06/terrain/collision_55.inc"
};
static NJS_OBJECT collision_63 = {
#include "stg06/terrain/collision_63.inc"
};
static NJS_OBJECT collision_66 = {
#include "stg06/terrain/collision_66.inc"
};
static NJS_OBJECT collision_67 = {
#include "stg06/terrain/collision_67.inc"
};
static NJS_OBJECT collision_68 = {
#include "stg06/terrain/collision_68.inc"
};
static NJS_OBJECT collision_69 = {
#include "stg06/terrain/collision_69.inc"
};
static NJS_OBJECT collision_70 = {
#include "stg06/terrain/collision_70.inc"
};
static NJS_OBJECT collision_71 = {
#include "stg06/terrain/collision_71.inc"
};
static NJS_OBJECT collision_87 = {
#include "stg06/terrain/collision_87.inc"
};
static NJS_OBJECT collision_89 = {
#include "stg06/terrain/collision_89.inc"
};
static NJS_OBJECT collision_90 = {
#include "stg06/terrain/collision_90.inc"
};
static NJS_OBJECT collision_92 = {
#include "stg06/terrain/collision_92.inc"
};
static NJS_OBJECT collision_93 = {
#include "stg06/terrain/collision_93.inc"
};
static NJS_CNK_OBJECT draw_162 = {
#include "stg06/terrain/draw_162.inc"
};
static NJS_CNK_OBJECT draw_160 = {
#include "stg06/terrain/draw_160.inc"
};
static NJS_CNK_OBJECT draw_159 = {
#include "stg06/terrain/draw_159.inc"
};
static NJS_CNK_OBJECT draw_158 = {
#include "stg06/terrain/draw_158.inc"
};
static NJS_CNK_OBJECT draw_157 = {
#include "stg06/terrain/draw_157.inc"
};
static NJS_CNK_OBJECT draw_156 = {
#include "stg06/terrain/draw_156.inc"
};
static NJS_CNK_OBJECT draw_152 = {
#include "stg06/terrain/draw_152.inc"
};
static NJS_CNK_OBJECT draw_150 = {
#include "stg06/terrain/draw_150.inc"
};
static NJS_CNK_OBJECT draw_146 = {
#include "stg06/terrain/draw_146.inc"
};
static NJS_CNK_OBJECT draw_145 = {
#include "stg06/terrain/draw_145.inc"
};
static NJS_CNK_OBJECT draw_144 = {
#include "stg06/terrain/draw_144.inc"
};
static NJS_CNK_OBJECT draw_143 = {
#include "stg06/terrain/draw_143.inc"
};
static NJS_CNK_OBJECT draw_142 = {
#include "stg06/terrain/draw_142.inc"
};
static NJS_CNK_OBJECT draw_141 = {
#include "stg06/terrain/draw_141.inc"
};
static NJS_CNK_OBJECT draw_139 = {
#include "stg06/terrain/draw_139.inc"
};
static NJS_CNK_OBJECT draw_130 = {
#include "stg06/terrain/draw_130.inc"
};
static NJS_CNK_OBJECT draw_129 = {
#include "stg06/terrain/draw_129.inc"
};
static NJS_CNK_OBJECT draw_127 = {
#include "stg06/terrain/draw_127.inc"
};
static NJS_CNK_OBJECT draw_126 = {
#include "stg06/terrain/draw_126.inc"
};
static NJS_CNK_OBJECT draw_125 = {
#include "stg06/terrain/draw_125.inc"
};
static NJS_CNK_OBJECT draw_124 = {
#include "stg06/terrain/draw_124.inc"
};
static NJS_CNK_OBJECT draw_122 = {
#include "stg06/terrain/draw_122.inc"
};
static NJS_CNK_OBJECT draw_119 = {
#include "stg06/terrain/draw_119.inc"
};
static NJS_CNK_OBJECT draw_117 = {
#include "stg06/terrain/draw_117.inc"
};
static NJS_CNK_OBJECT draw_116 = {
#include "stg06/terrain/draw_116.inc"
};
static NJS_CNK_OBJECT draw_115 = {
#include "stg06/terrain/draw_115.inc"
};
static NJS_CNK_OBJECT draw_114 = {
#include "stg06/terrain/draw_114.inc"
};
static NJS_CNK_OBJECT draw_109 = {
#include "stg06/terrain/draw_109.inc"
};
static NJS_CNK_OBJECT draw_106 = {
#include "stg06/terrain/draw_106.inc"
};
static NJS_CNK_OBJECT draw_102 = {
#include "stg06/terrain/draw_102.inc"
};
static NJS_CNK_OBJECT draw_101 = {
#include "stg06/terrain/draw_101.inc"
};
static NJS_CNK_OBJECT draw_100 = {
#include "stg06/terrain/draw_100.inc"
};
static NJS_CNK_OBJECT draw_99 = {
#include "stg06/terrain/draw_99.inc"
};
static NJS_CNK_OBJECT draw_97 = {
#include "stg06/terrain/draw_97.inc"
};
static NJS_CNK_OBJECT draw_96 = {
#include "stg06/terrain/draw_96.inc"
};
static NJS_CNK_OBJECT draw_95 = {
#include "stg06/terrain/draw_95.inc"
};
static NJS_CNK_OBJECT draw_94 = {
#include "stg06/terrain/draw_94.inc"
};
static NJS_CNK_OBJECT draw_93 = {
#include "stg06/terrain/draw_93.inc"
};
static NJS_CNK_OBJECT draw_92 = {
#include "stg06/terrain/draw_92.inc"
};
static NJS_CNK_OBJECT draw_89 = {
#include "stg06/terrain/draw_89.inc"
};
static NJS_CNK_OBJECT draw_84 = {
#include "stg06/terrain/draw_84.inc"
};
static NJS_CNK_OBJECT draw_83 = {
#include "stg06/terrain/draw_83.inc"
};
static NJS_CNK_OBJECT draw_82 = {
#include "stg06/terrain/draw_82.inc"
};
static NJS_CNK_OBJECT draw_76 = {
#include "stg06/terrain/draw_76.inc"
};
static NJS_CNK_OBJECT draw_74 = {
#include "stg06/terrain/draw_74.inc"
};
static NJS_CNK_OBJECT draw_72 = {
#include "stg06/terrain/draw_72.inc"
};
static NJS_CNK_OBJECT draw_48 = {
#include "stg06/terrain/draw_48.inc"
};
static NJS_CNK_OBJECT draw_47 = {
#include "stg06/terrain/draw_47.inc"
};
static NJS_CNK_OBJECT draw_46 = {
#include "stg06/terrain/draw_46.inc"
};
static NJS_CNK_OBJECT draw_44 = {
#include "stg06/terrain/draw_44.inc"
};
static NJS_CNK_OBJECT draw_43 = {
#include "stg06/terrain/draw_43.inc"
};
static NJS_CNK_OBJECT draw_42 = {
#include "stg06/terrain/draw_42.inc"
};
static NJS_CNK_OBJECT draw_41 = {
#include "stg06/terrain/draw_41.inc"
};
static NJS_CNK_OBJECT draw_30 = {
#include "stg06/terrain/draw_30.inc"
};
static NJS_CNK_OBJECT draw_29 = {
#include "stg06/terrain/draw_29.inc"
};
static NJS_CNK_OBJECT draw_28 = {
#include "stg06/terrain/draw_28.inc"
};
static NJS_CNK_OBJECT draw_27 = {
#include "stg06/terrain/draw_27.inc"
};
static NJS_CNK_OBJECT draw_26 = {
#include "stg06/terrain/draw_26.inc"
};
static NJS_CNK_OBJECT draw_25 = {
#include "stg06/terrain/draw_25.inc"
};
static NJS_CNK_OBJECT draw_24 = {
#include "stg06/terrain/draw_24.inc"
};
static NJS_CNK_OBJECT draw_23 = {
#include "stg06/terrain/draw_23.inc"
};
static NJS_CNK_OBJECT draw_22 = {
#include "stg06/terrain/draw_22.inc"
};
static NJS_CNK_OBJECT draw_21 = {
#include "stg06/terrain/draw_21.inc"
};
static NJS_CNK_OBJECT draw_20 = {
#include "stg06/terrain/draw_20.inc"
};
static NJS_CNK_OBJECT draw_19 = {
#include "stg06/terrain/draw_19.inc"
};
static NJS_CNK_OBJECT draw_9 = {
#include "stg06/terrain/draw_9.inc"
};
static NJS_CNK_OBJECT draw_1 = {
#include "stg06/terrain/draw_1.inc"
};
static NJS_CNK_OBJECT draw_0 = {
#include "stg06/terrain/draw_0.inc"
};
