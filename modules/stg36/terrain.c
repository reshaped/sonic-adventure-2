/* STG36.PRS's land, texture lists, chunk models, Basic models and texture
 * names, from the disc. its C lies between background_archives.c and
 * object_archives.c, and its D lies between machine_lights.c and
 * object_archives.c */
#include <ninja.h>
#include <sg_xpt.h>

#include "collision/world.h"
#include "stg36/core_tails.h"
#include "stg36/route.h"

#include "stg36/terrain/text.inc"

/* the data up to the last that a pointer names ahead of its definition, as
 * tentative definitions in the disc's order: GCC places each datum at its
 * first definition */
static struct land_entry entries[208];
struct land_table core_tails_land;
static NJS_TEXNAME textures[36];
NJS_TEXLIST tails_glow_texlist;
static Sint16 draw_135_polygons[1223];
static Sint32 draw_135_vertices[963];
static NJS_CNK_MODEL draw_135_model;
static NJS_CNK_OBJECT draw_135;
static Sint16 draw_134_polygons[95];
static Sint32 draw_134_vertices[83];
static NJS_CNK_MODEL draw_134_model;
static NJS_CNK_OBJECT draw_134;
static Sint16 draw_129_polygons[523];
static Sint32 draw_129_vertices[283];
static NJS_CNK_MODEL draw_129_model;
static NJS_CNK_OBJECT draw_129;
static Sint16 draw_128_polygons[543];
static Sint32 draw_128_vertices[343];
static NJS_CNK_MODEL draw_128_model;
static NJS_CNK_OBJECT draw_128;
static Sint16 draw_127_polygons[543];
static Sint32 draw_127_vertices[343];
static NJS_CNK_MODEL draw_127_model;
static NJS_CNK_OBJECT draw_127;
static Sint16 draw_126_polygons[401];
static Sint32 draw_126_vertices[239];
static NJS_CNK_MODEL draw_126_model;
static NJS_CNK_OBJECT draw_126;
static Sint16 draw_125_polygons[411];
static Sint32 draw_125_vertices[251];
static NJS_CNK_MODEL draw_125_model;
static NJS_CNK_OBJECT draw_125;
static Sint16 draw_123_polygons[543];
static Sint32 draw_123_vertices[331];
static NJS_CNK_MODEL draw_123_model;
static NJS_CNK_OBJECT draw_124;
static Sint16 draw_122_polygons[543];
static Sint32 draw_122_vertices[323];
static NJS_CNK_MODEL draw_122_model;
static NJS_CNK_OBJECT draw_122;
static Sint16 draw_121_polygons[543];
static Sint32 draw_121_vertices[331];
static NJS_CNK_MODEL draw_121_model;
static NJS_CNK_OBJECT draw_121;
static Sint16 draw_120_polygons[411];
static Sint32 draw_120_vertices[251];
static NJS_CNK_MODEL draw_120_model;
static NJS_CNK_OBJECT draw_120;
static Sint16 draw_118_polygons[543];
static Sint32 draw_118_vertices[331];
static NJS_CNK_MODEL draw_118_model;
static NJS_CNK_OBJECT draw_119;
static Sint16 draw_117_polygons[2079];
static Sint32 draw_117_vertices[2243];
static NJS_CNK_MODEL draw_117_model;
static NJS_CNK_OBJECT draw_117;
static Sint16 draw_95_polygons[1299];
static Sint32 draw_95_vertices[1091];
static NJS_CNK_MODEL draw_95_model;
static NJS_CNK_OBJECT draw_95;
static Sint16 draw_94_polygons[1243];
static Sint32 draw_94_vertices[1071];
static NJS_CNK_MODEL draw_94_model;
static NJS_CNK_OBJECT draw_94;
static Sint16 draw_93_polygons[1235];
static Sint32 draw_93_vertices[1067];
static NJS_CNK_MODEL draw_93_model;
static NJS_CNK_OBJECT draw_93;
static Sint16 draw_92_polygons[1229];
static Sint32 draw_92_vertices[1059];
static NJS_CNK_MODEL draw_92_model;
static NJS_CNK_OBJECT draw_92;
static Sint16 draw_73_polygons[1277];
static Sint32 draw_73_vertices[1071];
static NJS_CNK_MODEL draw_73_model;
static NJS_CNK_OBJECT draw_73;
static Sint16 draw_72_polygons[3331];
static Sint32 draw_72_vertices[2695];
static NJS_CNK_MODEL draw_72_model;
static NJS_CNK_OBJECT draw_72;
static Sint16 draw_71_polygons[345];
static Sint32 draw_71_vertices[355];
static NJS_CNK_MODEL draw_71_model;
static NJS_CNK_OBJECT draw_71;
static Sint16 draw_70_polygons[1115];
static Sint32 draw_70_vertices[635];
static NJS_CNK_MODEL draw_70_model;
static NJS_CNK_OBJECT draw_70;
static Sint16 draw_69_polygons[709];
static Sint32 draw_69_vertices[619];
static NJS_CNK_MODEL draw_69_model;
static NJS_CNK_OBJECT draw_69;
static Sint16 draw_68_polygons[2453];
static Sint32 draw_68_vertices[1419];
static NJS_CNK_MODEL draw_68_model;
static NJS_CNK_OBJECT draw_68;
static Sint16 draw_67_polygons[1213];
static Sint32 draw_67_vertices[1203];
static NJS_CNK_MODEL draw_67_model;
static NJS_CNK_OBJECT draw_67;
static Sint16 draw_66_polygons[187];
static Sint32 draw_66_vertices[211];
static NJS_CNK_MODEL draw_66_model;
static NJS_CNK_OBJECT draw_66;
static Sint16 draw_61_polygons[455];
static Sint32 draw_61_vertices[483];
static NJS_CNK_MODEL draw_61_model;
static NJS_CNK_OBJECT draw_65;
static Sint16 draw_60_polygons[65];
static Sint32 draw_60_vertices[67];
static NJS_CNK_MODEL draw_60_model;
static NJS_CNK_OBJECT draw_60;
static Sint16 draw_53_polygons[1271];
static Sint32 draw_53_vertices[1059];
static NJS_CNK_MODEL draw_53_model;
static NJS_CNK_OBJECT draw_59;
static Sint16 draw_47_polygons[99];
static Sint32 draw_47_vertices[95];
static NJS_CNK_MODEL draw_47_model;
static NJS_CNK_OBJECT draw_52;
static Sint16 draw_46_polygons[1111];
static Sint32 draw_46_vertices[1175];
static NJS_CNK_MODEL draw_46_model;
static NJS_CNK_OBJECT draw_46;
static Sint16 draw_43_polygons[129];
static Sint32 draw_43_vertices[99];
static NJS_CNK_MODEL draw_43_model;
static NJS_CNK_OBJECT draw_45;
static Sint16 draw_41_polygons[235];
static Sint32 draw_41_vertices[199];
static NJS_CNK_MODEL draw_41_model;
static NJS_CNK_OBJECT draw_42;
static Sint16 draw_40_polygons[199];
static Sint32 draw_40_vertices[163];
static NJS_CNK_MODEL draw_40_model;
static NJS_CNK_OBJECT draw_40;
static Sint16 draw_36_polygons[315];
static Sint32 draw_36_vertices[351];
static NJS_CNK_MODEL draw_36_model;
static NJS_CNK_OBJECT draw_39;
static Sint16 draw_35_polygons[871];
static Sint32 draw_35_vertices[675];
static NJS_CNK_MODEL draw_35_model;
static NJS_CNK_OBJECT draw_35;
static Sint16 draw_33_polygons[871];
static Sint32 draw_33_vertices[675];
static NJS_CNK_MODEL draw_33_model;
static NJS_CNK_OBJECT draw_34;
static Sint16 draw_0_polygons[121];
static Sint32 draw_0_vertices[115];
static NJS_CNK_MODEL draw_0_model;
static NJS_CNK_OBJECT draw_32;
static Sint16 draw_2_polygons[293];
static Sint32 draw_2_vertices[231];
static NJS_CNK_MODEL draw_2_model;
static NJS_CNK_OBJECT draw_6;
static Sint16 draw_1_polygons[709];
static Sint32 draw_1_vertices[619];
static NJS_CNK_MODEL draw_1_model;
static NJS_CNK_OBJECT draw_1;
static NJS_MATERIAL collision_0_materials[1];
static Sint16 collision_0_faces[14];
static NJS_MESHSET collision_0_meshsets[1];
static NJS_POINT3 collision_0_points[12];
static NJS_MODEL collision_0_model;
static NJS_OBJECT collision_0;
static NJS_MATERIAL collision_1_materials[1];
static Sint16 collision_1_faces[245];
static NJS_MESHSET collision_1_meshsets[1];
static NJS_POINT3 collision_1_points[112];
static NJS_MODEL collision_1_model;
static NJS_OBJECT collision_1;
static NJS_MATERIAL collision_2_materials[1];
static Sint16 collision_2_faces[36];
static NJS_MESHSET collision_2_meshsets[1];
static NJS_POINT3 collision_2_points[22];
static NJS_MODEL collision_2_model;
static NJS_OBJECT collision_2;
static NJS_MATERIAL collision_3_materials[1];
static Sint16 collision_3_faces[45];
static NJS_MESHSET collision_3_meshsets[1];
static NJS_POINT3 collision_3_points[22];
static NJS_MODEL collision_3_model;
static NJS_OBJECT collision_3;
static NJS_MATERIAL collision_4_materials[1];
static Sint16 collision_4_faces[36];
static NJS_MESHSET collision_4_meshsets[1];
static NJS_POINT3 collision_4_points[22];
static NJS_MODEL collision_4_model;
static NJS_OBJECT collision_4;
static NJS_MATERIAL collision_5_materials[1];
static Sint16 collision_5_faces[164];
static NJS_MESHSET collision_5_meshsets[1];
static NJS_POINT3 collision_5_points[90];
static NJS_MODEL collision_5_model;
static NJS_OBJECT collision_5;
static NJS_MATERIAL collision_8_materials[1];
static Sint16 collision_8_faces[38];
static NJS_MESHSET collision_8_meshsets[1];
static NJS_POINT3 collision_8_points[24];
static NJS_MODEL collision_8_model;
static NJS_OBJECT collision_8;
static NJS_MATERIAL collision_9_materials[1];
static Sint16 collision_9_faces[38];
static NJS_MESHSET collision_9_meshsets[1];
static NJS_POINT3 collision_9_points[24];
static NJS_MODEL collision_9_model;
static NJS_OBJECT collision_9;
static NJS_MATERIAL collision_10_materials[1];
static Sint16 collision_10_faces[38];
static NJS_MESHSET collision_10_meshsets[1];
static NJS_POINT3 collision_10_points[24];
static NJS_MODEL collision_10_model;
static NJS_OBJECT collision_10;
static NJS_MATERIAL collision_11_materials[1];
static Sint16 collision_11_faces[38];
static NJS_MESHSET collision_11_meshsets[1];
static NJS_POINT3 collision_11_points[24];
static NJS_MODEL collision_11_model;
static NJS_OBJECT collision_11;
static NJS_MATERIAL collision_12_materials[1];
static Sint16 collision_12_faces[38];
static NJS_MESHSET collision_12_meshsets[1];
static NJS_POINT3 collision_12_points[24];
static NJS_MODEL collision_12_model;
static NJS_OBJECT collision_12;
static NJS_MATERIAL collision_13_materials[1];
static Sint16 collision_13_faces[38];
static NJS_MESHSET collision_13_meshsets[1];
static NJS_POINT3 collision_13_points[24];
static NJS_MODEL collision_13_model;
static NJS_OBJECT collision_13;
static NJS_MATERIAL collision_14_materials[1];
static Sint16 collision_14_faces[38];
static NJS_MESHSET collision_14_meshsets[1];
static NJS_POINT3 collision_14_points[24];
static NJS_MODEL collision_14_model;
static NJS_OBJECT collision_14;
static NJS_MATERIAL collision_15_materials[1];
static Sint16 collision_15_faces[38];
static NJS_MESHSET collision_15_meshsets[1];
static NJS_POINT3 collision_15_points[24];
static NJS_MODEL collision_15_model;
static NJS_OBJECT collision_15;
static NJS_MATERIAL collision_16_materials[1];
static Sint16 collision_16_faces[38];
static NJS_MESHSET collision_16_meshsets[1];
static NJS_POINT3 collision_16_points[24];
static NJS_MODEL collision_16_model;
static NJS_OBJECT collision_16;
static NJS_MATERIAL collision_17_materials[1];
static Sint16 collision_17_faces[38];
static NJS_MESHSET collision_17_meshsets[1];
static NJS_POINT3 collision_17_points[24];
static NJS_MODEL collision_17_model;
static NJS_OBJECT collision_17;
static NJS_MATERIAL collision_18_materials[1];
static Sint16 collision_18_faces[38];
static NJS_MESHSET collision_18_meshsets[1];
static NJS_POINT3 collision_18_points[24];
static NJS_MODEL collision_18_model;
static NJS_OBJECT collision_18;
static NJS_MATERIAL collision_19_materials[1];
static Sint16 collision_19_faces[38];
static NJS_MESHSET collision_19_meshsets[1];
static NJS_POINT3 collision_19_points[24];
static NJS_MODEL collision_19_model;
static NJS_OBJECT collision_19;
static NJS_MATERIAL collision_20_materials[1];
static Sint16 collision_20_faces[38];
static NJS_MESHSET collision_20_meshsets[1];
static NJS_POINT3 collision_20_points[24];
static NJS_MODEL collision_20_model;
static NJS_OBJECT collision_20;
static NJS_MATERIAL collision_21_materials[1];
static Sint16 collision_21_faces[38];
static NJS_MESHSET collision_21_meshsets[1];
static NJS_POINT3 collision_21_points[24];
static NJS_MODEL collision_21_model;
static NJS_OBJECT collision_21;
static NJS_MATERIAL collision_22_materials[1];
static Sint16 collision_22_faces[38];
static NJS_MESHSET collision_22_meshsets[1];
static NJS_POINT3 collision_22_points[24];
static NJS_MODEL collision_22_model;
static NJS_OBJECT collision_22;
static NJS_MATERIAL collision_23_materials[1];
static Sint16 collision_23_faces[38];
static NJS_MESHSET collision_23_meshsets[1];
static NJS_POINT3 collision_23_points[24];
static NJS_MODEL collision_23_model;
static NJS_OBJECT collision_23;
static NJS_MATERIAL collision_24_materials[1];
static Sint16 collision_24_faces[38];
static NJS_MESHSET collision_24_meshsets[1];
static NJS_POINT3 collision_24_points[24];
static NJS_MODEL collision_24_model;
static NJS_OBJECT collision_24;
static NJS_MATERIAL collision_25_materials[1];
static Sint16 collision_25_faces[10];
static NJS_MESHSET collision_25_meshsets[1];
static NJS_POINT3 collision_25_points[8];
static NJS_MODEL collision_25_model;
static NJS_OBJECT collision_25;
static NJS_MATERIAL collision_38_materials[1];
static Sint16 collision_38_faces[18];
static NJS_MESHSET collision_38_meshsets[1];
static NJS_POINT3 collision_38_points[16];
static NJS_MODEL collision_38_model;
static NJS_OBJECT collision_38;
static NJS_MATERIAL collision_39_materials[1];
static Sint16 collision_39_faces[128];
static NJS_MESHSET collision_39_meshsets[1];
static NJS_POINT3 collision_39_points[60];
static NJS_MODEL collision_39_model;
static NJS_OBJECT collision_39;
static NJS_MATERIAL collision_43_materials[1];
static Sint16 collision_43_faces[58];
static NJS_MESHSET collision_43_meshsets[1];
static NJS_POINT3 collision_43_points[23];
static NJS_MODEL collision_43_model;
static NJS_OBJECT collision_43;
static NJS_MATERIAL collision_44_materials[1];
static Sint16 collision_44_faces[33];
static NJS_MESHSET collision_44_meshsets[1];
static NJS_POINT3 collision_44_points[21];
static NJS_MODEL collision_44_model;
static NJS_OBJECT collision_44;
static NJS_MATERIAL collision_45_materials[1];
static Sint16 collision_45_faces[133];
static NJS_MESHSET collision_45_meshsets[1];
static NJS_POINT3 collision_45_points[60];
static NJS_MODEL collision_45_model;
static NJS_OBJECT collision_45;
static NJS_MATERIAL collision_46_materials[1];
static Sint16 collision_46_faces[52];
static NJS_MESHSET collision_46_meshsets[1];
static NJS_POINT3 collision_46_points[24];
static NJS_MODEL collision_46_model;
static NJS_OBJECT collision_46;
static NJS_MATERIAL collision_49_materials[1];
static Sint16 collision_49_faces[20];
static NJS_MESHSET collision_49_meshsets[1];
static NJS_POINT3 collision_49_points[12];
static NJS_MODEL collision_49_model;
static NJS_OBJECT collision_49;
static NJS_MATERIAL collision_52_materials[1];
static Sint16 collision_52_faces[133];
static NJS_MESHSET collision_52_meshsets[1];
static NJS_POINT3 collision_52_points[60];
static NJS_MODEL collision_52_model;
static NJS_OBJECT collision_52;
static NJS_MATERIAL collision_53_materials[1];
static Sint16 collision_53_faces[56];
static NJS_MESHSET collision_53_meshsets[1];
static NJS_POINT3 collision_53_points[25];
static NJS_MODEL collision_53_model;
static NJS_OBJECT collision_53;
static NJS_MATERIAL collision_55_materials[1];
static Sint16 collision_55_faces[20];
static NJS_MESHSET collision_55_meshsets[1];
static NJS_POINT3 collision_55_points[10];
static NJS_MODEL collision_55_model;
static NJS_OBJECT collision_55;
static NJS_MATERIAL collision_68_materials[1];
static Sint16 collision_68_faces[62];
static NJS_MESHSET collision_68_meshsets[1];
static NJS_POINT3 collision_68_points[26];
static NJS_MODEL collision_68_model;
static NJS_OBJECT collision_68;
static NJS_MATERIAL collision_69_materials[1];
static Sint16 collision_69_faces[14];
static NJS_MESHSET collision_69_meshsets[1];
static NJS_POINT3 collision_69_points[12];
static NJS_MODEL collision_69_model;
static NJS_OBJECT collision_69;
static NJS_MATERIAL collision_70_materials[1];
static Sint16 collision_70_faces[150];
static NJS_MESHSET collision_70_meshsets[1];
static NJS_POINT3 collision_70_points[54];
static NJS_MODEL collision_70_model;
static NJS_OBJECT collision_70;
static NJS_MATERIAL collision_71_materials[1];
static Sint16 collision_71_faces[16];
static NJS_MESHSET collision_71_meshsets[1];
static NJS_POINT3 collision_71_points[8];
static NJS_MODEL collision_71_model;
static NJS_OBJECT collision_71;
static NJS_CNK_OBJECT draw_133;
static NJS_CNK_OBJECT draw_132;
static NJS_CNK_OBJECT draw_131;
static NJS_CNK_OBJECT draw_130;
static NJS_CNK_OBJECT draw_123;
static NJS_CNK_OBJECT draw_118;
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
static NJS_CNK_OBJECT draw_64;
static NJS_CNK_OBJECT draw_63;
static NJS_CNK_OBJECT draw_62;
static NJS_CNK_OBJECT draw_61;
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
static NJS_CNK_OBJECT draw_44;
static NJS_CNK_OBJECT draw_43;
static NJS_CNK_OBJECT draw_41;
static NJS_CNK_OBJECT draw_38;
static NJS_CNK_OBJECT draw_37;
static NJS_CNK_OBJECT draw_36;
static NJS_CNK_OBJECT draw_33;
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
static NJS_CNK_OBJECT draw_5;
static NJS_CNK_OBJECT draw_4;
static NJS_CNK_OBJECT draw_3;
static NJS_CNK_OBJECT draw_2;
static NJS_CNK_OBJECT draw_0;
static NJS_OBJECT collision_6;
static NJS_OBJECT collision_7;
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
static NJS_OBJECT collision_40;
static NJS_OBJECT collision_41;
static NJS_OBJECT collision_42;
static NJS_OBJECT collision_47;
static NJS_OBJECT collision_48;
static NJS_OBJECT collision_50;
static NJS_OBJECT collision_51;
static NJS_OBJECT collision_54;
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

static struct land_entry entries[208] = {
#include "stg36/terrain/entries.inc"
};
struct land_table core_tails_land = {
#include "stg36/terrain/core_tails_land.inc"
};
static NJS_TEXNAME textures[36] = {
#include "stg36/terrain/textures.inc"
};
NJS_TEXLIST tails_glow_texlist = {
#include "stg36/terrain/tails_glow_texlist.inc"
};
static Sint16 draw_135_polygons[1223] = {
#include "stg36/terrain/draw_135_polygons.inc"
};
static Sint32 draw_135_vertices[963] = {
#include "stg36/terrain/draw_135_vertices.inc"
};
static NJS_CNK_MODEL draw_135_model = {
#include "stg36/terrain/draw_135_model.inc"
};
static NJS_CNK_OBJECT draw_135 = {
#include "stg36/terrain/draw_135.inc"
};
static Sint16 draw_134_polygons[95] = {
#include "stg36/terrain/draw_134_polygons.inc"
};
static Sint32 draw_134_vertices[83] = {
#include "stg36/terrain/draw_134_vertices.inc"
};
static NJS_CNK_MODEL draw_134_model = {
#include "stg36/terrain/draw_134_model.inc"
};
static NJS_CNK_OBJECT draw_134 = {
#include "stg36/terrain/draw_134.inc"
};
static Sint16 draw_129_polygons[523] = {
#include "stg36/terrain/draw_129_polygons.inc"
};
static Sint32 draw_129_vertices[283] = {
#include "stg36/terrain/draw_129_vertices.inc"
};
static NJS_CNK_MODEL draw_129_model = {
#include "stg36/terrain/draw_129_model.inc"
};
static NJS_CNK_OBJECT draw_129 = {
#include "stg36/terrain/draw_129.inc"
};
static Sint16 draw_128_polygons[543] = {
#include "stg36/terrain/draw_128_polygons.inc"
};
static Sint32 draw_128_vertices[343] = {
#include "stg36/terrain/draw_128_vertices.inc"
};
static NJS_CNK_MODEL draw_128_model = {
#include "stg36/terrain/draw_128_model.inc"
};
static NJS_CNK_OBJECT draw_128 = {
#include "stg36/terrain/draw_128.inc"
};
static Sint16 draw_127_polygons[543] = {
#include "stg36/terrain/draw_127_polygons.inc"
};
static Sint32 draw_127_vertices[343] = {
#include "stg36/terrain/draw_127_vertices.inc"
};
static NJS_CNK_MODEL draw_127_model = {
#include "stg36/terrain/draw_127_model.inc"
};
static NJS_CNK_OBJECT draw_127 = {
#include "stg36/terrain/draw_127.inc"
};
static Sint16 draw_126_polygons[401] = {
#include "stg36/terrain/draw_126_polygons.inc"
};
static Sint32 draw_126_vertices[239] = {
#include "stg36/terrain/draw_126_vertices.inc"
};
static NJS_CNK_MODEL draw_126_model = {
#include "stg36/terrain/draw_126_model.inc"
};
static NJS_CNK_OBJECT draw_126 = {
#include "stg36/terrain/draw_126.inc"
};
static Sint16 draw_125_polygons[411] = {
#include "stg36/terrain/draw_125_polygons.inc"
};
static Sint32 draw_125_vertices[251] = {
#include "stg36/terrain/draw_125_vertices.inc"
};
static NJS_CNK_MODEL draw_125_model = {
#include "stg36/terrain/draw_125_model.inc"
};
static NJS_CNK_OBJECT draw_125 = {
#include "stg36/terrain/draw_125.inc"
};
static Sint16 draw_123_polygons[543] = {
#include "stg36/terrain/draw_123_polygons.inc"
};
static Sint32 draw_123_vertices[331] = {
#include "stg36/terrain/draw_123_vertices.inc"
};
static NJS_CNK_MODEL draw_123_model = {
#include "stg36/terrain/draw_123_model.inc"
};
static NJS_CNK_OBJECT draw_124 = {
#include "stg36/terrain/draw_124.inc"
};
static Sint16 draw_122_polygons[543] = {
#include "stg36/terrain/draw_122_polygons.inc"
};
static Sint32 draw_122_vertices[323] = {
#include "stg36/terrain/draw_122_vertices.inc"
};
static NJS_CNK_MODEL draw_122_model = {
#include "stg36/terrain/draw_122_model.inc"
};
static NJS_CNK_OBJECT draw_122 = {
#include "stg36/terrain/draw_122.inc"
};
static Sint16 draw_121_polygons[543] = {
#include "stg36/terrain/draw_121_polygons.inc"
};
static Sint32 draw_121_vertices[331] = {
#include "stg36/terrain/draw_121_vertices.inc"
};
static NJS_CNK_MODEL draw_121_model = {
#include "stg36/terrain/draw_121_model.inc"
};
static NJS_CNK_OBJECT draw_121 = {
#include "stg36/terrain/draw_121.inc"
};
static Sint16 draw_120_polygons[411] = {
#include "stg36/terrain/draw_120_polygons.inc"
};
static Sint32 draw_120_vertices[251] = {
#include "stg36/terrain/draw_120_vertices.inc"
};
static NJS_CNK_MODEL draw_120_model = {
#include "stg36/terrain/draw_120_model.inc"
};
static NJS_CNK_OBJECT draw_120 = {
#include "stg36/terrain/draw_120.inc"
};
static Sint16 draw_118_polygons[543] = {
#include "stg36/terrain/draw_118_polygons.inc"
};
static Sint32 draw_118_vertices[331] = {
#include "stg36/terrain/draw_118_vertices.inc"
};
static NJS_CNK_MODEL draw_118_model = {
#include "stg36/terrain/draw_118_model.inc"
};
static NJS_CNK_OBJECT draw_119 = {
#include "stg36/terrain/draw_119.inc"
};
static Sint16 draw_117_polygons[2079] = {
#include "stg36/terrain/draw_117_polygons.inc"
};
static Sint32 draw_117_vertices[2243] = {
#include "stg36/terrain/draw_117_vertices.inc"
};
static NJS_CNK_MODEL draw_117_model = {
#include "stg36/terrain/draw_117_model.inc"
};
static NJS_CNK_OBJECT draw_117 = {
#include "stg36/terrain/draw_117.inc"
};
static Sint16 draw_95_polygons[1299] = {
#include "stg36/terrain/draw_95_polygons.inc"
};
static Sint32 draw_95_vertices[1091] = {
#include "stg36/terrain/draw_95_vertices.inc"
};
static NJS_CNK_MODEL draw_95_model = {
#include "stg36/terrain/draw_95_model.inc"
};
static NJS_CNK_OBJECT draw_95 = {
#include "stg36/terrain/draw_95.inc"
};
static Sint16 draw_94_polygons[1243] = {
#include "stg36/terrain/draw_94_polygons.inc"
};
static Sint32 draw_94_vertices[1071] = {
#include "stg36/terrain/draw_94_vertices.inc"
};
static NJS_CNK_MODEL draw_94_model = {
#include "stg36/terrain/draw_94_model.inc"
};
static NJS_CNK_OBJECT draw_94 = {
#include "stg36/terrain/draw_94.inc"
};
static Sint16 draw_93_polygons[1235] = {
#include "stg36/terrain/draw_93_polygons.inc"
};
static Sint32 draw_93_vertices[1067] = {
#include "stg36/terrain/draw_93_vertices.inc"
};
static NJS_CNK_MODEL draw_93_model = {
#include "stg36/terrain/draw_93_model.inc"
};
static NJS_CNK_OBJECT draw_93 = {
#include "stg36/terrain/draw_93.inc"
};
static Sint16 draw_92_polygons[1229] = {
#include "stg36/terrain/draw_92_polygons.inc"
};
static Sint32 draw_92_vertices[1059] = {
#include "stg36/terrain/draw_92_vertices.inc"
};
static NJS_CNK_MODEL draw_92_model = {
#include "stg36/terrain/draw_92_model.inc"
};
static NJS_CNK_OBJECT draw_92 = {
#include "stg36/terrain/draw_92.inc"
};
static Sint16 draw_73_polygons[1277] = {
#include "stg36/terrain/draw_73_polygons.inc"
};
static Sint32 draw_73_vertices[1071] = {
#include "stg36/terrain/draw_73_vertices.inc"
};
static NJS_CNK_MODEL draw_73_model = {
#include "stg36/terrain/draw_73_model.inc"
};
static NJS_CNK_OBJECT draw_73 = {
#include "stg36/terrain/draw_73.inc"
};
static Sint16 draw_72_polygons[3331] = {
#include "stg36/terrain/draw_72_polygons.inc"
};
static Sint32 draw_72_vertices[2695] = {
#include "stg36/terrain/draw_72_vertices.inc"
};
static NJS_CNK_MODEL draw_72_model = {
#include "stg36/terrain/draw_72_model.inc"
};
static NJS_CNK_OBJECT draw_72 = {
#include "stg36/terrain/draw_72.inc"
};
static Sint16 draw_71_polygons[345] = {
#include "stg36/terrain/draw_71_polygons.inc"
};
static Sint32 draw_71_vertices[355] = {
#include "stg36/terrain/draw_71_vertices.inc"
};
static NJS_CNK_MODEL draw_71_model = {
#include "stg36/terrain/draw_71_model.inc"
};
static NJS_CNK_OBJECT draw_71 = {
#include "stg36/terrain/draw_71.inc"
};
static Sint16 draw_70_polygons[1115] = {
#include "stg36/terrain/draw_70_polygons.inc"
};
static Sint32 draw_70_vertices[635] = {
#include "stg36/terrain/draw_70_vertices.inc"
};
static NJS_CNK_MODEL draw_70_model = {
#include "stg36/terrain/draw_70_model.inc"
};
static NJS_CNK_OBJECT draw_70 = {
#include "stg36/terrain/draw_70.inc"
};
static Sint16 draw_69_polygons[709] = {
#include "stg36/terrain/draw_69_polygons.inc"
};
static Sint32 draw_69_vertices[619] = {
#include "stg36/terrain/draw_69_vertices.inc"
};
static NJS_CNK_MODEL draw_69_model = {
#include "stg36/terrain/draw_69_model.inc"
};
static NJS_CNK_OBJECT draw_69 = {
#include "stg36/terrain/draw_69.inc"
};
static Sint16 draw_68_polygons[2453] = {
#include "stg36/terrain/draw_68_polygons.inc"
};
static Sint32 draw_68_vertices[1419] = {
#include "stg36/terrain/draw_68_vertices.inc"
};
static NJS_CNK_MODEL draw_68_model = {
#include "stg36/terrain/draw_68_model.inc"
};
static NJS_CNK_OBJECT draw_68 = {
#include "stg36/terrain/draw_68.inc"
};
static Sint16 draw_67_polygons[1213] = {
#include "stg36/terrain/draw_67_polygons.inc"
};
static Sint32 draw_67_vertices[1203] = {
#include "stg36/terrain/draw_67_vertices.inc"
};
static NJS_CNK_MODEL draw_67_model = {
#include "stg36/terrain/draw_67_model.inc"
};
static NJS_CNK_OBJECT draw_67 = {
#include "stg36/terrain/draw_67.inc"
};
static Sint16 draw_66_polygons[187] = {
#include "stg36/terrain/draw_66_polygons.inc"
};
static Sint32 draw_66_vertices[211] = {
#include "stg36/terrain/draw_66_vertices.inc"
};
static NJS_CNK_MODEL draw_66_model = {
#include "stg36/terrain/draw_66_model.inc"
};
static NJS_CNK_OBJECT draw_66 = {
#include "stg36/terrain/draw_66.inc"
};
static Sint16 draw_61_polygons[455] = {
#include "stg36/terrain/draw_61_polygons.inc"
};
static Sint32 draw_61_vertices[483] = {
#include "stg36/terrain/draw_61_vertices.inc"
};
static NJS_CNK_MODEL draw_61_model = {
#include "stg36/terrain/draw_61_model.inc"
};
static NJS_CNK_OBJECT draw_65 = {
#include "stg36/terrain/draw_65.inc"
};
static Sint16 draw_60_polygons[65] = {
#include "stg36/terrain/draw_60_polygons.inc"
};
static Sint32 draw_60_vertices[67] = {
#include "stg36/terrain/draw_60_vertices.inc"
};
static NJS_CNK_MODEL draw_60_model = {
#include "stg36/terrain/draw_60_model.inc"
};
static NJS_CNK_OBJECT draw_60 = {
#include "stg36/terrain/draw_60.inc"
};
static Sint16 draw_53_polygons[1271] = {
#include "stg36/terrain/draw_53_polygons.inc"
};
static Sint32 draw_53_vertices[1059] = {
#include "stg36/terrain/draw_53_vertices.inc"
};
static NJS_CNK_MODEL draw_53_model = {
#include "stg36/terrain/draw_53_model.inc"
};
static NJS_CNK_OBJECT draw_59 = {
#include "stg36/terrain/draw_59.inc"
};
static Sint16 draw_47_polygons[99] = {
#include "stg36/terrain/draw_47_polygons.inc"
};
static Sint32 draw_47_vertices[95] = {
#include "stg36/terrain/draw_47_vertices.inc"
};
static NJS_CNK_MODEL draw_47_model = {
#include "stg36/terrain/draw_47_model.inc"
};
static NJS_CNK_OBJECT draw_52 = {
#include "stg36/terrain/draw_52.inc"
};
static Sint16 draw_46_polygons[1111] = {
#include "stg36/terrain/draw_46_polygons.inc"
};
static Sint32 draw_46_vertices[1175] = {
#include "stg36/terrain/draw_46_vertices.inc"
};
static NJS_CNK_MODEL draw_46_model = {
#include "stg36/terrain/draw_46_model.inc"
};
static NJS_CNK_OBJECT draw_46 = {
#include "stg36/terrain/draw_46.inc"
};
static Sint16 draw_43_polygons[129] = {
#include "stg36/terrain/draw_43_polygons.inc"
};
static Sint32 draw_43_vertices[99] = {
#include "stg36/terrain/draw_43_vertices.inc"
};
static NJS_CNK_MODEL draw_43_model = {
#include "stg36/terrain/draw_43_model.inc"
};
static NJS_CNK_OBJECT draw_45 = {
#include "stg36/terrain/draw_45.inc"
};
static Sint16 draw_41_polygons[235] = {
#include "stg36/terrain/draw_41_polygons.inc"
};
static Sint32 draw_41_vertices[199] = {
#include "stg36/terrain/draw_41_vertices.inc"
};
static NJS_CNK_MODEL draw_41_model = {
#include "stg36/terrain/draw_41_model.inc"
};
static NJS_CNK_OBJECT draw_42 = {
#include "stg36/terrain/draw_42.inc"
};
static Sint16 draw_40_polygons[199] = {
#include "stg36/terrain/draw_40_polygons.inc"
};
static Sint32 draw_40_vertices[163] = {
#include "stg36/terrain/draw_40_vertices.inc"
};
static NJS_CNK_MODEL draw_40_model = {
#include "stg36/terrain/draw_40_model.inc"
};
static NJS_CNK_OBJECT draw_40 = {
#include "stg36/terrain/draw_40.inc"
};
static Sint16 draw_36_polygons[315] = {
#include "stg36/terrain/draw_36_polygons.inc"
};
static Sint32 draw_36_vertices[351] = {
#include "stg36/terrain/draw_36_vertices.inc"
};
static NJS_CNK_MODEL draw_36_model = {
#include "stg36/terrain/draw_36_model.inc"
};
static NJS_CNK_OBJECT draw_39 = {
#include "stg36/terrain/draw_39.inc"
};
static Sint16 draw_35_polygons[871] = {
#include "stg36/terrain/draw_35_polygons.inc"
};
static Sint32 draw_35_vertices[675] = {
#include "stg36/terrain/draw_35_vertices.inc"
};
static NJS_CNK_MODEL draw_35_model = {
#include "stg36/terrain/draw_35_model.inc"
};
static NJS_CNK_OBJECT draw_35 = {
#include "stg36/terrain/draw_35.inc"
};
static Sint16 draw_33_polygons[871] = {
#include "stg36/terrain/draw_33_polygons.inc"
};
static Sint32 draw_33_vertices[675] = {
#include "stg36/terrain/draw_33_vertices.inc"
};
static NJS_CNK_MODEL draw_33_model = {
#include "stg36/terrain/draw_33_model.inc"
};
static NJS_CNK_OBJECT draw_34 = {
#include "stg36/terrain/draw_34.inc"
};
static Sint16 draw_0_polygons[121] = {
#include "stg36/terrain/draw_0_polygons.inc"
};
static Sint32 draw_0_vertices[115] = {
#include "stg36/terrain/draw_0_vertices.inc"
};
static NJS_CNK_MODEL draw_0_model = {
#include "stg36/terrain/draw_0_model.inc"
};
static NJS_CNK_OBJECT draw_32 = {
#include "stg36/terrain/draw_32.inc"
};
static Sint16 draw_2_polygons[293] = {
#include "stg36/terrain/draw_2_polygons.inc"
};
static Sint32 draw_2_vertices[231] = {
#include "stg36/terrain/draw_2_vertices.inc"
};
static NJS_CNK_MODEL draw_2_model = {
#include "stg36/terrain/draw_2_model.inc"
};
static NJS_CNK_OBJECT draw_6 = {
#include "stg36/terrain/draw_6.inc"
};
static Sint16 draw_1_polygons[709] = {
#include "stg36/terrain/draw_1_polygons.inc"
};
static Sint32 draw_1_vertices[619] = {
#include "stg36/terrain/draw_1_vertices.inc"
};
static NJS_CNK_MODEL draw_1_model = {
#include "stg36/terrain/draw_1_model.inc"
};
static NJS_CNK_OBJECT draw_1 = {
#include "stg36/terrain/draw_1.inc"
};
static NJS_MATERIAL collision_0_materials[1] = {
#include "stg36/terrain/collision_0_materials.inc"
};
static Sint16 collision_0_faces[14] = {
#include "stg36/terrain/collision_0_faces.inc"
};
static NJS_MESHSET collision_0_meshsets[1] = {
#include "stg36/terrain/collision_0_meshsets.inc"
};
static NJS_POINT3 collision_0_points[12] = {
#include "stg36/terrain/collision_0_points.inc"
};
static NJS_MODEL collision_0_model = {
#include "stg36/terrain/collision_0_model.inc"
};
static NJS_OBJECT collision_0 = {
#include "stg36/terrain/collision_0.inc"
};
static NJS_MATERIAL collision_1_materials[1] = {
#include "stg36/terrain/collision_1_materials.inc"
};
static Sint16 collision_1_faces[245] = {
#include "stg36/terrain/collision_1_faces.inc"
};
static NJS_MESHSET collision_1_meshsets[1] = {
#include "stg36/terrain/collision_1_meshsets.inc"
};
static NJS_POINT3 collision_1_points[112] = {
#include "stg36/terrain/collision_1_points.inc"
};
static NJS_MODEL collision_1_model = {
#include "stg36/terrain/collision_1_model.inc"
};
static NJS_OBJECT collision_1 = {
#include "stg36/terrain/collision_1.inc"
};
static NJS_MATERIAL collision_2_materials[1] = {
#include "stg36/terrain/collision_2_materials.inc"
};
static Sint16 collision_2_faces[36] = {
#include "stg36/terrain/collision_2_faces.inc"
};
static NJS_MESHSET collision_2_meshsets[1] = {
#include "stg36/terrain/collision_2_meshsets.inc"
};
static NJS_POINT3 collision_2_points[22] = {
#include "stg36/terrain/collision_2_points.inc"
};
static NJS_MODEL collision_2_model = {
#include "stg36/terrain/collision_2_model.inc"
};
static NJS_OBJECT collision_2 = {
#include "stg36/terrain/collision_2.inc"
};
static NJS_MATERIAL collision_3_materials[1] = {
#include "stg36/terrain/collision_3_materials.inc"
};
static Sint16 collision_3_faces[45] = {
#include "stg36/terrain/collision_3_faces.inc"
};
static NJS_MESHSET collision_3_meshsets[1] = {
#include "stg36/terrain/collision_3_meshsets.inc"
};
static NJS_POINT3 collision_3_points[22] = {
#include "stg36/terrain/collision_3_points.inc"
};
static NJS_MODEL collision_3_model = {
#include "stg36/terrain/collision_3_model.inc"
};
static NJS_OBJECT collision_3 = {
#include "stg36/terrain/collision_3.inc"
};
static NJS_MATERIAL collision_4_materials[1] = {
#include "stg36/terrain/collision_4_materials.inc"
};
static Sint16 collision_4_faces[36] = {
#include "stg36/terrain/collision_4_faces.inc"
};
static NJS_MESHSET collision_4_meshsets[1] = {
#include "stg36/terrain/collision_4_meshsets.inc"
};
static NJS_POINT3 collision_4_points[22] = {
#include "stg36/terrain/collision_4_points.inc"
};
static NJS_MODEL collision_4_model = {
#include "stg36/terrain/collision_4_model.inc"
};
static NJS_OBJECT collision_4 = {
#include "stg36/terrain/collision_4.inc"
};
static NJS_MATERIAL collision_5_materials[1] = {
#include "stg36/terrain/collision_5_materials.inc"
};
static Sint16 collision_5_faces[164] = {
#include "stg36/terrain/collision_5_faces.inc"
};
static NJS_MESHSET collision_5_meshsets[1] = {
#include "stg36/terrain/collision_5_meshsets.inc"
};
static NJS_POINT3 collision_5_points[90] = {
#include "stg36/terrain/collision_5_points.inc"
};
static NJS_MODEL collision_5_model = {
#include "stg36/terrain/collision_5_model.inc"
};
static NJS_OBJECT collision_5 = {
#include "stg36/terrain/collision_5.inc"
};
static NJS_MATERIAL collision_8_materials[1] = {
#include "stg36/terrain/collision_8_materials.inc"
};
static Sint16 collision_8_faces[38] = {
#include "stg36/terrain/collision_8_faces.inc"
};
static NJS_MESHSET collision_8_meshsets[1] = {
#include "stg36/terrain/collision_8_meshsets.inc"
};
static NJS_POINT3 collision_8_points[24] = {
#include "stg36/terrain/collision_8_points.inc"
};
static NJS_MODEL collision_8_model = {
#include "stg36/terrain/collision_8_model.inc"
};
static NJS_OBJECT collision_8 = {
#include "stg36/terrain/collision_8.inc"
};
static NJS_MATERIAL collision_9_materials[1] = {
#include "stg36/terrain/collision_9_materials.inc"
};
static Sint16 collision_9_faces[38] = {
#include "stg36/terrain/collision_9_faces.inc"
};
static NJS_MESHSET collision_9_meshsets[1] = {
#include "stg36/terrain/collision_9_meshsets.inc"
};
static NJS_POINT3 collision_9_points[24] = {
#include "stg36/terrain/collision_9_points.inc"
};
static NJS_MODEL collision_9_model = {
#include "stg36/terrain/collision_9_model.inc"
};
static NJS_OBJECT collision_9 = {
#include "stg36/terrain/collision_9.inc"
};
static NJS_MATERIAL collision_10_materials[1] = {
#include "stg36/terrain/collision_10_materials.inc"
};
static Sint16 collision_10_faces[38] = {
#include "stg36/terrain/collision_10_faces.inc"
};
static NJS_MESHSET collision_10_meshsets[1] = {
#include "stg36/terrain/collision_10_meshsets.inc"
};
static NJS_POINT3 collision_10_points[24] = {
#include "stg36/terrain/collision_10_points.inc"
};
static NJS_MODEL collision_10_model = {
#include "stg36/terrain/collision_10_model.inc"
};
static NJS_OBJECT collision_10 = {
#include "stg36/terrain/collision_10.inc"
};
static NJS_MATERIAL collision_11_materials[1] = {
#include "stg36/terrain/collision_11_materials.inc"
};
static Sint16 collision_11_faces[38] = {
#include "stg36/terrain/collision_11_faces.inc"
};
static NJS_MESHSET collision_11_meshsets[1] = {
#include "stg36/terrain/collision_11_meshsets.inc"
};
static NJS_POINT3 collision_11_points[24] = {
#include "stg36/terrain/collision_11_points.inc"
};
static NJS_MODEL collision_11_model = {
#include "stg36/terrain/collision_11_model.inc"
};
static NJS_OBJECT collision_11 = {
#include "stg36/terrain/collision_11.inc"
};
static NJS_MATERIAL collision_12_materials[1] = {
#include "stg36/terrain/collision_12_materials.inc"
};
static Sint16 collision_12_faces[38] = {
#include "stg36/terrain/collision_12_faces.inc"
};
static NJS_MESHSET collision_12_meshsets[1] = {
#include "stg36/terrain/collision_12_meshsets.inc"
};
static NJS_POINT3 collision_12_points[24] = {
#include "stg36/terrain/collision_12_points.inc"
};
static NJS_MODEL collision_12_model = {
#include "stg36/terrain/collision_12_model.inc"
};
static NJS_OBJECT collision_12 = {
#include "stg36/terrain/collision_12.inc"
};
static NJS_MATERIAL collision_13_materials[1] = {
#include "stg36/terrain/collision_13_materials.inc"
};
static Sint16 collision_13_faces[38] = {
#include "stg36/terrain/collision_13_faces.inc"
};
static NJS_MESHSET collision_13_meshsets[1] = {
#include "stg36/terrain/collision_13_meshsets.inc"
};
static NJS_POINT3 collision_13_points[24] = {
#include "stg36/terrain/collision_13_points.inc"
};
static NJS_MODEL collision_13_model = {
#include "stg36/terrain/collision_13_model.inc"
};
static NJS_OBJECT collision_13 = {
#include "stg36/terrain/collision_13.inc"
};
static NJS_MATERIAL collision_14_materials[1] = {
#include "stg36/terrain/collision_14_materials.inc"
};
static Sint16 collision_14_faces[38] = {
#include "stg36/terrain/collision_14_faces.inc"
};
static NJS_MESHSET collision_14_meshsets[1] = {
#include "stg36/terrain/collision_14_meshsets.inc"
};
static NJS_POINT3 collision_14_points[24] = {
#include "stg36/terrain/collision_14_points.inc"
};
static NJS_MODEL collision_14_model = {
#include "stg36/terrain/collision_14_model.inc"
};
static NJS_OBJECT collision_14 = {
#include "stg36/terrain/collision_14.inc"
};
static NJS_MATERIAL collision_15_materials[1] = {
#include "stg36/terrain/collision_15_materials.inc"
};
static Sint16 collision_15_faces[38] = {
#include "stg36/terrain/collision_15_faces.inc"
};
static NJS_MESHSET collision_15_meshsets[1] = {
#include "stg36/terrain/collision_15_meshsets.inc"
};
static NJS_POINT3 collision_15_points[24] = {
#include "stg36/terrain/collision_15_points.inc"
};
static NJS_MODEL collision_15_model = {
#include "stg36/terrain/collision_15_model.inc"
};
static NJS_OBJECT collision_15 = {
#include "stg36/terrain/collision_15.inc"
};
static NJS_MATERIAL collision_16_materials[1] = {
#include "stg36/terrain/collision_16_materials.inc"
};
static Sint16 collision_16_faces[38] = {
#include "stg36/terrain/collision_16_faces.inc"
};
static NJS_MESHSET collision_16_meshsets[1] = {
#include "stg36/terrain/collision_16_meshsets.inc"
};
static NJS_POINT3 collision_16_points[24] = {
#include "stg36/terrain/collision_16_points.inc"
};
static NJS_MODEL collision_16_model = {
#include "stg36/terrain/collision_16_model.inc"
};
static NJS_OBJECT collision_16 = {
#include "stg36/terrain/collision_16.inc"
};
static NJS_MATERIAL collision_17_materials[1] = {
#include "stg36/terrain/collision_17_materials.inc"
};
static Sint16 collision_17_faces[38] = {
#include "stg36/terrain/collision_17_faces.inc"
};
static NJS_MESHSET collision_17_meshsets[1] = {
#include "stg36/terrain/collision_17_meshsets.inc"
};
static NJS_POINT3 collision_17_points[24] = {
#include "stg36/terrain/collision_17_points.inc"
};
static NJS_MODEL collision_17_model = {
#include "stg36/terrain/collision_17_model.inc"
};
static NJS_OBJECT collision_17 = {
#include "stg36/terrain/collision_17.inc"
};
static NJS_MATERIAL collision_18_materials[1] = {
#include "stg36/terrain/collision_18_materials.inc"
};
static Sint16 collision_18_faces[38] = {
#include "stg36/terrain/collision_18_faces.inc"
};
static NJS_MESHSET collision_18_meshsets[1] = {
#include "stg36/terrain/collision_18_meshsets.inc"
};
static NJS_POINT3 collision_18_points[24] = {
#include "stg36/terrain/collision_18_points.inc"
};
static NJS_MODEL collision_18_model = {
#include "stg36/terrain/collision_18_model.inc"
};
static NJS_OBJECT collision_18 = {
#include "stg36/terrain/collision_18.inc"
};
static NJS_MATERIAL collision_19_materials[1] = {
#include "stg36/terrain/collision_19_materials.inc"
};
static Sint16 collision_19_faces[38] = {
#include "stg36/terrain/collision_19_faces.inc"
};
static NJS_MESHSET collision_19_meshsets[1] = {
#include "stg36/terrain/collision_19_meshsets.inc"
};
static NJS_POINT3 collision_19_points[24] = {
#include "stg36/terrain/collision_19_points.inc"
};
static NJS_MODEL collision_19_model = {
#include "stg36/terrain/collision_19_model.inc"
};
static NJS_OBJECT collision_19 = {
#include "stg36/terrain/collision_19.inc"
};
static NJS_MATERIAL collision_20_materials[1] = {
#include "stg36/terrain/collision_20_materials.inc"
};
static Sint16 collision_20_faces[38] = {
#include "stg36/terrain/collision_20_faces.inc"
};
static NJS_MESHSET collision_20_meshsets[1] = {
#include "stg36/terrain/collision_20_meshsets.inc"
};
static NJS_POINT3 collision_20_points[24] = {
#include "stg36/terrain/collision_20_points.inc"
};
static NJS_MODEL collision_20_model = {
#include "stg36/terrain/collision_20_model.inc"
};
static NJS_OBJECT collision_20 = {
#include "stg36/terrain/collision_20.inc"
};
static NJS_MATERIAL collision_21_materials[1] = {
#include "stg36/terrain/collision_21_materials.inc"
};
static Sint16 collision_21_faces[38] = {
#include "stg36/terrain/collision_21_faces.inc"
};
static NJS_MESHSET collision_21_meshsets[1] = {
#include "stg36/terrain/collision_21_meshsets.inc"
};
static NJS_POINT3 collision_21_points[24] = {
#include "stg36/terrain/collision_21_points.inc"
};
static NJS_MODEL collision_21_model = {
#include "stg36/terrain/collision_21_model.inc"
};
static NJS_OBJECT collision_21 = {
#include "stg36/terrain/collision_21.inc"
};
static NJS_MATERIAL collision_22_materials[1] = {
#include "stg36/terrain/collision_22_materials.inc"
};
static Sint16 collision_22_faces[38] = {
#include "stg36/terrain/collision_22_faces.inc"
};
static NJS_MESHSET collision_22_meshsets[1] = {
#include "stg36/terrain/collision_22_meshsets.inc"
};
static NJS_POINT3 collision_22_points[24] = {
#include "stg36/terrain/collision_22_points.inc"
};
static NJS_MODEL collision_22_model = {
#include "stg36/terrain/collision_22_model.inc"
};
static NJS_OBJECT collision_22 = {
#include "stg36/terrain/collision_22.inc"
};
static NJS_MATERIAL collision_23_materials[1] = {
#include "stg36/terrain/collision_23_materials.inc"
};
static Sint16 collision_23_faces[38] = {
#include "stg36/terrain/collision_23_faces.inc"
};
static NJS_MESHSET collision_23_meshsets[1] = {
#include "stg36/terrain/collision_23_meshsets.inc"
};
static NJS_POINT3 collision_23_points[24] = {
#include "stg36/terrain/collision_23_points.inc"
};
static NJS_MODEL collision_23_model = {
#include "stg36/terrain/collision_23_model.inc"
};
static NJS_OBJECT collision_23 = {
#include "stg36/terrain/collision_23.inc"
};
static NJS_MATERIAL collision_24_materials[1] = {
#include "stg36/terrain/collision_24_materials.inc"
};
static Sint16 collision_24_faces[38] = {
#include "stg36/terrain/collision_24_faces.inc"
};
static NJS_MESHSET collision_24_meshsets[1] = {
#include "stg36/terrain/collision_24_meshsets.inc"
};
static NJS_POINT3 collision_24_points[24] = {
#include "stg36/terrain/collision_24_points.inc"
};
static NJS_MODEL collision_24_model = {
#include "stg36/terrain/collision_24_model.inc"
};
static NJS_OBJECT collision_24 = {
#include "stg36/terrain/collision_24.inc"
};
static NJS_MATERIAL collision_25_materials[1] = {
#include "stg36/terrain/collision_25_materials.inc"
};
static Sint16 collision_25_faces[10] = {
#include "stg36/terrain/collision_25_faces.inc"
};
static NJS_MESHSET collision_25_meshsets[1] = {
#include "stg36/terrain/collision_25_meshsets.inc"
};
static NJS_POINT3 collision_25_points[8] = {
#include "stg36/terrain/collision_25_points.inc"
};
static NJS_MODEL collision_25_model = {
#include "stg36/terrain/collision_25_model.inc"
};
static NJS_OBJECT collision_25 = {
#include "stg36/terrain/collision_25.inc"
};
static NJS_MATERIAL collision_38_materials[1] = {
#include "stg36/terrain/collision_38_materials.inc"
};
static Sint16 collision_38_faces[18] = {
#include "stg36/terrain/collision_38_faces.inc"
};
static NJS_MESHSET collision_38_meshsets[1] = {
#include "stg36/terrain/collision_38_meshsets.inc"
};
static NJS_POINT3 collision_38_points[16] = {
#include "stg36/terrain/collision_38_points.inc"
};
static NJS_MODEL collision_38_model = {
#include "stg36/terrain/collision_38_model.inc"
};
static NJS_OBJECT collision_38 = {
#include "stg36/terrain/collision_38.inc"
};
static NJS_MATERIAL collision_39_materials[1] = {
#include "stg36/terrain/collision_39_materials.inc"
};
static Sint16 collision_39_faces[128] = {
#include "stg36/terrain/collision_39_faces.inc"
};
static NJS_MESHSET collision_39_meshsets[1] = {
#include "stg36/terrain/collision_39_meshsets.inc"
};
static NJS_POINT3 collision_39_points[60] = {
#include "stg36/terrain/collision_39_points.inc"
};
static NJS_MODEL collision_39_model = {
#include "stg36/terrain/collision_39_model.inc"
};
static NJS_OBJECT collision_39 = {
#include "stg36/terrain/collision_39.inc"
};
static NJS_MATERIAL collision_43_materials[1] = {
#include "stg36/terrain/collision_43_materials.inc"
};
static Sint16 collision_43_faces[58] = {
#include "stg36/terrain/collision_43_faces.inc"
};
static NJS_MESHSET collision_43_meshsets[1] = {
#include "stg36/terrain/collision_43_meshsets.inc"
};
static NJS_POINT3 collision_43_points[23] = {
#include "stg36/terrain/collision_43_points.inc"
};
static NJS_MODEL collision_43_model = {
#include "stg36/terrain/collision_43_model.inc"
};
static NJS_OBJECT collision_43 = {
#include "stg36/terrain/collision_43.inc"
};
static NJS_MATERIAL collision_44_materials[1] = {
#include "stg36/terrain/collision_44_materials.inc"
};
static Sint16 collision_44_faces[33] = {
#include "stg36/terrain/collision_44_faces.inc"
};
static NJS_MESHSET collision_44_meshsets[1] = {
#include "stg36/terrain/collision_44_meshsets.inc"
};
static NJS_POINT3 collision_44_points[21] = {
#include "stg36/terrain/collision_44_points.inc"
};
static NJS_MODEL collision_44_model = {
#include "stg36/terrain/collision_44_model.inc"
};
static NJS_OBJECT collision_44 = {
#include "stg36/terrain/collision_44.inc"
};
static NJS_MATERIAL collision_45_materials[1] = {
#include "stg36/terrain/collision_45_materials.inc"
};
static Sint16 collision_45_faces[133] = {
#include "stg36/terrain/collision_45_faces.inc"
};
static NJS_MESHSET collision_45_meshsets[1] = {
#include "stg36/terrain/collision_45_meshsets.inc"
};
static NJS_POINT3 collision_45_points[60] = {
#include "stg36/terrain/collision_45_points.inc"
};
static NJS_MODEL collision_45_model = {
#include "stg36/terrain/collision_45_model.inc"
};
static NJS_OBJECT collision_45 = {
#include "stg36/terrain/collision_45.inc"
};
static NJS_MATERIAL collision_46_materials[1] = {
#include "stg36/terrain/collision_46_materials.inc"
};
static Sint16 collision_46_faces[52] = {
#include "stg36/terrain/collision_46_faces.inc"
};
static NJS_MESHSET collision_46_meshsets[1] = {
#include "stg36/terrain/collision_46_meshsets.inc"
};
static NJS_POINT3 collision_46_points[24] = {
#include "stg36/terrain/collision_46_points.inc"
};
static NJS_MODEL collision_46_model = {
#include "stg36/terrain/collision_46_model.inc"
};
static NJS_OBJECT collision_46 = {
#include "stg36/terrain/collision_46.inc"
};
static NJS_MATERIAL collision_49_materials[1] = {
#include "stg36/terrain/collision_49_materials.inc"
};
static Sint16 collision_49_faces[20] = {
#include "stg36/terrain/collision_49_faces.inc"
};
static NJS_MESHSET collision_49_meshsets[1] = {
#include "stg36/terrain/collision_49_meshsets.inc"
};
static NJS_POINT3 collision_49_points[12] = {
#include "stg36/terrain/collision_49_points.inc"
};
static NJS_MODEL collision_49_model = {
#include "stg36/terrain/collision_49_model.inc"
};
static NJS_OBJECT collision_49 = {
#include "stg36/terrain/collision_49.inc"
};
static NJS_MATERIAL collision_52_materials[1] = {
#include "stg36/terrain/collision_52_materials.inc"
};
static Sint16 collision_52_faces[133] = {
#include "stg36/terrain/collision_52_faces.inc"
};
static NJS_MESHSET collision_52_meshsets[1] = {
#include "stg36/terrain/collision_52_meshsets.inc"
};
static NJS_POINT3 collision_52_points[60] = {
#include "stg36/terrain/collision_52_points.inc"
};
static NJS_MODEL collision_52_model = {
#include "stg36/terrain/collision_52_model.inc"
};
static NJS_OBJECT collision_52 = {
#include "stg36/terrain/collision_52.inc"
};
static NJS_MATERIAL collision_53_materials[1] = {
#include "stg36/terrain/collision_53_materials.inc"
};
static Sint16 collision_53_faces[56] = {
#include "stg36/terrain/collision_53_faces.inc"
};
static NJS_MESHSET collision_53_meshsets[1] = {
#include "stg36/terrain/collision_53_meshsets.inc"
};
static NJS_POINT3 collision_53_points[25] = {
#include "stg36/terrain/collision_53_points.inc"
};
static NJS_MODEL collision_53_model = {
#include "stg36/terrain/collision_53_model.inc"
};
static NJS_OBJECT collision_53 = {
#include "stg36/terrain/collision_53.inc"
};
static NJS_MATERIAL collision_55_materials[1] = {
#include "stg36/terrain/collision_55_materials.inc"
};
static Sint16 collision_55_faces[20] = {
#include "stg36/terrain/collision_55_faces.inc"
};
static NJS_MESHSET collision_55_meshsets[1] = {
#include "stg36/terrain/collision_55_meshsets.inc"
};
static NJS_POINT3 collision_55_points[10] = {
#include "stg36/terrain/collision_55_points.inc"
};
static NJS_MODEL collision_55_model = {
#include "stg36/terrain/collision_55_model.inc"
};
static NJS_OBJECT collision_55 = {
#include "stg36/terrain/collision_55.inc"
};
static NJS_MATERIAL collision_68_materials[1] = {
#include "stg36/terrain/collision_68_materials.inc"
};
static Sint16 collision_68_faces[62] = {
#include "stg36/terrain/collision_68_faces.inc"
};
static NJS_MESHSET collision_68_meshsets[1] = {
#include "stg36/terrain/collision_68_meshsets.inc"
};
static NJS_POINT3 collision_68_points[26] = {
#include "stg36/terrain/collision_68_points.inc"
};
static NJS_MODEL collision_68_model = {
#include "stg36/terrain/collision_68_model.inc"
};
static NJS_OBJECT collision_68 = {
#include "stg36/terrain/collision_68.inc"
};
static NJS_MATERIAL collision_69_materials[1] = {
#include "stg36/terrain/collision_69_materials.inc"
};
static Sint16 collision_69_faces[14] = {
#include "stg36/terrain/collision_69_faces.inc"
};
static NJS_MESHSET collision_69_meshsets[1] = {
#include "stg36/terrain/collision_69_meshsets.inc"
};
static NJS_POINT3 collision_69_points[12] = {
#include "stg36/terrain/collision_69_points.inc"
};
static NJS_MODEL collision_69_model = {
#include "stg36/terrain/collision_69_model.inc"
};
static NJS_OBJECT collision_69 = {
#include "stg36/terrain/collision_69.inc"
};
static NJS_MATERIAL collision_70_materials[1] = {
#include "stg36/terrain/collision_70_materials.inc"
};
static Sint16 collision_70_faces[150] = {
#include "stg36/terrain/collision_70_faces.inc"
};
static NJS_MESHSET collision_70_meshsets[1] = {
#include "stg36/terrain/collision_70_meshsets.inc"
};
static NJS_POINT3 collision_70_points[54] = {
#include "stg36/terrain/collision_70_points.inc"
};
static NJS_MODEL collision_70_model = {
#include "stg36/terrain/collision_70_model.inc"
};
static NJS_OBJECT collision_70 = {
#include "stg36/terrain/collision_70.inc"
};
static NJS_MATERIAL collision_71_materials[1] = {
#include "stg36/terrain/collision_71_materials.inc"
};
static Sint16 collision_71_faces[16] = {
#include "stg36/terrain/collision_71_faces.inc"
};
static NJS_MESHSET collision_71_meshsets[1] = {
#include "stg36/terrain/collision_71_meshsets.inc"
};
static NJS_POINT3 collision_71_points[8] = {
#include "stg36/terrain/collision_71_points.inc"
};
static NJS_MODEL collision_71_model = {
#include "stg36/terrain/collision_71_model.inc"
};
static NJS_OBJECT collision_71 = {
#include "stg36/terrain/collision_71.inc"
};
static NJS_CNK_OBJECT draw_133 = {
#include "stg36/terrain/draw_133.inc"
};
static NJS_CNK_OBJECT draw_132 = {
#include "stg36/terrain/draw_132.inc"
};
static NJS_CNK_OBJECT draw_131 = {
#include "stg36/terrain/draw_131.inc"
};
static NJS_CNK_OBJECT draw_130 = {
#include "stg36/terrain/draw_130.inc"
};
static NJS_CNK_OBJECT draw_123 = {
#include "stg36/terrain/draw_123.inc"
};
static NJS_CNK_OBJECT draw_118 = {
#include "stg36/terrain/draw_118.inc"
};
static NJS_CNK_OBJECT draw_116 = {
#include "stg36/terrain/draw_116.inc"
};
static NJS_CNK_OBJECT draw_115 = {
#include "stg36/terrain/draw_115.inc"
};
static NJS_CNK_OBJECT draw_114 = {
#include "stg36/terrain/draw_114.inc"
};
static NJS_CNK_OBJECT draw_113 = {
#include "stg36/terrain/draw_113.inc"
};
static NJS_CNK_OBJECT draw_112 = {
#include "stg36/terrain/draw_112.inc"
};
static NJS_CNK_OBJECT draw_111 = {
#include "stg36/terrain/draw_111.inc"
};
static NJS_CNK_OBJECT draw_110 = {
#include "stg36/terrain/draw_110.inc"
};
static NJS_CNK_OBJECT draw_109 = {
#include "stg36/terrain/draw_109.inc"
};
static NJS_CNK_OBJECT draw_108 = {
#include "stg36/terrain/draw_108.inc"
};
static NJS_CNK_OBJECT draw_107 = {
#include "stg36/terrain/draw_107.inc"
};
static NJS_CNK_OBJECT draw_106 = {
#include "stg36/terrain/draw_106.inc"
};
static NJS_CNK_OBJECT draw_105 = {
#include "stg36/terrain/draw_105.inc"
};
static NJS_CNK_OBJECT draw_104 = {
#include "stg36/terrain/draw_104.inc"
};
static NJS_CNK_OBJECT draw_103 = {
#include "stg36/terrain/draw_103.inc"
};
static NJS_CNK_OBJECT draw_102 = {
#include "stg36/terrain/draw_102.inc"
};
static NJS_CNK_OBJECT draw_101 = {
#include "stg36/terrain/draw_101.inc"
};
static NJS_CNK_OBJECT draw_100 = {
#include "stg36/terrain/draw_100.inc"
};
static NJS_CNK_OBJECT draw_99 = {
#include "stg36/terrain/draw_99.inc"
};
static NJS_CNK_OBJECT draw_98 = {
#include "stg36/terrain/draw_98.inc"
};
static NJS_CNK_OBJECT draw_97 = {
#include "stg36/terrain/draw_97.inc"
};
static NJS_CNK_OBJECT draw_96 = {
#include "stg36/terrain/draw_96.inc"
};
static NJS_CNK_OBJECT draw_91 = {
#include "stg36/terrain/draw_91.inc"
};
static NJS_CNK_OBJECT draw_90 = {
#include "stg36/terrain/draw_90.inc"
};
static NJS_CNK_OBJECT draw_89 = {
#include "stg36/terrain/draw_89.inc"
};
static NJS_CNK_OBJECT draw_88 = {
#include "stg36/terrain/draw_88.inc"
};
static NJS_CNK_OBJECT draw_87 = {
#include "stg36/terrain/draw_87.inc"
};
static NJS_CNK_OBJECT draw_86 = {
#include "stg36/terrain/draw_86.inc"
};
static NJS_CNK_OBJECT draw_85 = {
#include "stg36/terrain/draw_85.inc"
};
static NJS_CNK_OBJECT draw_84 = {
#include "stg36/terrain/draw_84.inc"
};
static NJS_CNK_OBJECT draw_83 = {
#include "stg36/terrain/draw_83.inc"
};
static NJS_CNK_OBJECT draw_82 = {
#include "stg36/terrain/draw_82.inc"
};
static NJS_CNK_OBJECT draw_81 = {
#include "stg36/terrain/draw_81.inc"
};
static NJS_CNK_OBJECT draw_80 = {
#include "stg36/terrain/draw_80.inc"
};
static NJS_CNK_OBJECT draw_79 = {
#include "stg36/terrain/draw_79.inc"
};
static NJS_CNK_OBJECT draw_78 = {
#include "stg36/terrain/draw_78.inc"
};
static NJS_CNK_OBJECT draw_77 = {
#include "stg36/terrain/draw_77.inc"
};
static NJS_CNK_OBJECT draw_76 = {
#include "stg36/terrain/draw_76.inc"
};
static NJS_CNK_OBJECT draw_75 = {
#include "stg36/terrain/draw_75.inc"
};
static NJS_CNK_OBJECT draw_74 = {
#include "stg36/terrain/draw_74.inc"
};
static NJS_CNK_OBJECT draw_64 = {
#include "stg36/terrain/draw_64.inc"
};
static NJS_CNK_OBJECT draw_63 = {
#include "stg36/terrain/draw_63.inc"
};
static NJS_CNK_OBJECT draw_62 = {
#include "stg36/terrain/draw_62.inc"
};
static NJS_CNK_OBJECT draw_61 = {
#include "stg36/terrain/draw_61.inc"
};
static NJS_CNK_OBJECT draw_58 = {
#include "stg36/terrain/draw_58.inc"
};
static NJS_CNK_OBJECT draw_57 = {
#include "stg36/terrain/draw_57.inc"
};
static NJS_CNK_OBJECT draw_56 = {
#include "stg36/terrain/draw_56.inc"
};
static NJS_CNK_OBJECT draw_55 = {
#include "stg36/terrain/draw_55.inc"
};
static NJS_CNK_OBJECT draw_54 = {
#include "stg36/terrain/draw_54.inc"
};
static NJS_CNK_OBJECT draw_53 = {
#include "stg36/terrain/draw_53.inc"
};
static NJS_CNK_OBJECT draw_51 = {
#include "stg36/terrain/draw_51.inc"
};
static NJS_CNK_OBJECT draw_50 = {
#include "stg36/terrain/draw_50.inc"
};
static NJS_CNK_OBJECT draw_49 = {
#include "stg36/terrain/draw_49.inc"
};
static NJS_CNK_OBJECT draw_48 = {
#include "stg36/terrain/draw_48.inc"
};
static NJS_CNK_OBJECT draw_47 = {
#include "stg36/terrain/draw_47.inc"
};
static NJS_CNK_OBJECT draw_44 = {
#include "stg36/terrain/draw_44.inc"
};
static NJS_CNK_OBJECT draw_43 = {
#include "stg36/terrain/draw_43.inc"
};
static NJS_CNK_OBJECT draw_41 = {
#include "stg36/terrain/draw_41.inc"
};
static NJS_CNK_OBJECT draw_38 = {
#include "stg36/terrain/draw_38.inc"
};
static NJS_CNK_OBJECT draw_37 = {
#include "stg36/terrain/draw_37.inc"
};
static NJS_CNK_OBJECT draw_36 = {
#include "stg36/terrain/draw_36.inc"
};
static NJS_CNK_OBJECT draw_33 = {
#include "stg36/terrain/draw_33.inc"
};
static NJS_CNK_OBJECT draw_31 = {
#include "stg36/terrain/draw_31.inc"
};
static NJS_CNK_OBJECT draw_30 = {
#include "stg36/terrain/draw_30.inc"
};
static NJS_CNK_OBJECT draw_29 = {
#include "stg36/terrain/draw_29.inc"
};
static NJS_CNK_OBJECT draw_28 = {
#include "stg36/terrain/draw_28.inc"
};
static NJS_CNK_OBJECT draw_27 = {
#include "stg36/terrain/draw_27.inc"
};
static NJS_CNK_OBJECT draw_26 = {
#include "stg36/terrain/draw_26.inc"
};
static NJS_CNK_OBJECT draw_25 = {
#include "stg36/terrain/draw_25.inc"
};
static NJS_CNK_OBJECT draw_24 = {
#include "stg36/terrain/draw_24.inc"
};
static NJS_CNK_OBJECT draw_23 = {
#include "stg36/terrain/draw_23.inc"
};
static NJS_CNK_OBJECT draw_22 = {
#include "stg36/terrain/draw_22.inc"
};
static NJS_CNK_OBJECT draw_21 = {
#include "stg36/terrain/draw_21.inc"
};
static NJS_CNK_OBJECT draw_20 = {
#include "stg36/terrain/draw_20.inc"
};
static NJS_CNK_OBJECT draw_19 = {
#include "stg36/terrain/draw_19.inc"
};
static NJS_CNK_OBJECT draw_18 = {
#include "stg36/terrain/draw_18.inc"
};
static NJS_CNK_OBJECT draw_17 = {
#include "stg36/terrain/draw_17.inc"
};
static NJS_CNK_OBJECT draw_16 = {
#include "stg36/terrain/draw_16.inc"
};
static NJS_CNK_OBJECT draw_15 = {
#include "stg36/terrain/draw_15.inc"
};
static NJS_CNK_OBJECT draw_14 = {
#include "stg36/terrain/draw_14.inc"
};
static NJS_CNK_OBJECT draw_13 = {
#include "stg36/terrain/draw_13.inc"
};
static NJS_CNK_OBJECT draw_12 = {
#include "stg36/terrain/draw_12.inc"
};
static NJS_CNK_OBJECT draw_11 = {
#include "stg36/terrain/draw_11.inc"
};
static NJS_CNK_OBJECT draw_10 = {
#include "stg36/terrain/draw_10.inc"
};
static NJS_CNK_OBJECT draw_9 = {
#include "stg36/terrain/draw_9.inc"
};
static NJS_CNK_OBJECT draw_8 = {
#include "stg36/terrain/draw_8.inc"
};
static NJS_CNK_OBJECT draw_7 = {
#include "stg36/terrain/draw_7.inc"
};
static NJS_CNK_OBJECT draw_5 = {
#include "stg36/terrain/draw_5.inc"
};
static NJS_CNK_OBJECT draw_4 = {
#include "stg36/terrain/draw_4.inc"
};
static NJS_CNK_OBJECT draw_3 = {
#include "stg36/terrain/draw_3.inc"
};
static NJS_CNK_OBJECT draw_2 = {
#include "stg36/terrain/draw_2.inc"
};
static NJS_CNK_OBJECT draw_0 = {
#include "stg36/terrain/draw_0.inc"
};
static NJS_OBJECT collision_6 = {
#include "stg36/terrain/collision_6.inc"
};
static NJS_OBJECT collision_7 = {
#include "stg36/terrain/collision_7.inc"
};
static NJS_OBJECT collision_26 = {
#include "stg36/terrain/collision_26.inc"
};
static NJS_OBJECT collision_27 = {
#include "stg36/terrain/collision_27.inc"
};
static NJS_OBJECT collision_28 = {
#include "stg36/terrain/collision_28.inc"
};
static NJS_OBJECT collision_29 = {
#include "stg36/terrain/collision_29.inc"
};
static NJS_OBJECT collision_30 = {
#include "stg36/terrain/collision_30.inc"
};
static NJS_OBJECT collision_31 = {
#include "stg36/terrain/collision_31.inc"
};
static NJS_OBJECT collision_32 = {
#include "stg36/terrain/collision_32.inc"
};
static NJS_OBJECT collision_33 = {
#include "stg36/terrain/collision_33.inc"
};
static NJS_OBJECT collision_34 = {
#include "stg36/terrain/collision_34.inc"
};
static NJS_OBJECT collision_35 = {
#include "stg36/terrain/collision_35.inc"
};
static NJS_OBJECT collision_36 = {
#include "stg36/terrain/collision_36.inc"
};
static NJS_OBJECT collision_37 = {
#include "stg36/terrain/collision_37.inc"
};
static NJS_OBJECT collision_40 = {
#include "stg36/terrain/collision_40.inc"
};
static NJS_OBJECT collision_41 = {
#include "stg36/terrain/collision_41.inc"
};
static NJS_OBJECT collision_42 = {
#include "stg36/terrain/collision_42.inc"
};
static NJS_OBJECT collision_47 = {
#include "stg36/terrain/collision_47.inc"
};
static NJS_OBJECT collision_48 = {
#include "stg36/terrain/collision_48.inc"
};
static NJS_OBJECT collision_50 = {
#include "stg36/terrain/collision_50.inc"
};
static NJS_OBJECT collision_51 = {
#include "stg36/terrain/collision_51.inc"
};
static NJS_OBJECT collision_54 = {
#include "stg36/terrain/collision_54.inc"
};
static NJS_OBJECT collision_56 = {
#include "stg36/terrain/collision_56.inc"
};
static NJS_OBJECT collision_57 = {
#include "stg36/terrain/collision_57.inc"
};
static NJS_OBJECT collision_58 = {
#include "stg36/terrain/collision_58.inc"
};
static NJS_OBJECT collision_59 = {
#include "stg36/terrain/collision_59.inc"
};
static NJS_OBJECT collision_60 = {
#include "stg36/terrain/collision_60.inc"
};
static NJS_OBJECT collision_61 = {
#include "stg36/terrain/collision_61.inc"
};
static NJS_OBJECT collision_62 = {
#include "stg36/terrain/collision_62.inc"
};
static NJS_OBJECT collision_63 = {
#include "stg36/terrain/collision_63.inc"
};
static NJS_OBJECT collision_64 = {
#include "stg36/terrain/collision_64.inc"
};
static NJS_OBJECT collision_65 = {
#include "stg36/terrain/collision_65.inc"
};
static NJS_OBJECT collision_66 = {
#include "stg36/terrain/collision_66.inc"
};
static NJS_OBJECT collision_67 = {
#include "stg36/terrain/collision_67.inc"
};
